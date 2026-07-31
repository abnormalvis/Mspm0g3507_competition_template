/**
 * @file arm_task.c
 * @brief SCARA robotic arm pick-and-place task state machine
 *
 * Handshake protocol with RK3588 over UART2 (reuses arm_control UART2 TX).
 *
 * Flow:
 *   1. VOFA #P52=1  -> ArmTask_Start()
 *   2. MSPM0 -> RK3588: 0x01 START + JustFloat arm state
 *   3. RK3588 -> MSPM0: JustFloat pick[3] + place[3] angles
 *   4. Arm moves to pick -> Relay_On (grab) -> moves to place -> Relay_Off
 *   5. MSPM0 -> RK3588: 0x02 CONTINUE (loop for next fragment, max 4)
 *   6. RK3588 -> MSPM0: JustFloat with sentinel (-1.0 = no more fragments)
 *   7. Arm returns to zero -> MSPM0 -> RK3588: 0x03 COMPLETE
 *
 * Called from TIMER_1 ISR (10ms).  Uses sys_tick_ms for timing.
 */

#include "arm_task.h"
#include "arm_control.h"
#include "arm_protocol.h"
#include "hal_qgimbal_can.h"
#include "hal_relay.h"
#include "Servo.h"
#include "Buzzer.h"

/* ---- sys_tick_ms (declared in main.c) ---- */
extern volatile uint32_t sys_tick_ms;

/* ---- beep_on_1s_flag (declared in Buzzer.h) ---- */
extern bool beep_on_1s_flag;

/* ---- static task context ---- */
static ArmTaskContext s_ctx;
static uint8_t         s_task_cmd = ARMP_CMD_TASK_START;   /* command byte for RK3588: 0x01=chess, 0x05=poker */

/* ---- static helpers ---- */

/**
 * @brief Build MSPM0->RK3588 frame: cmd_byte + 6 floats arm state + tail.
 *
 * 6 JustFloat channels:
 *   ch0-ch2: actual motor angles from CAN feedback (deg)
 *   ch3:     relay state (1.0=on, 0.0=off)
 *   ch4:     current fragment index
 *   ch5:     status code (0.0=OK)
 */
static void arm_task_build_tx_frame(uint8_t cmd)
{
    float fb[6];
    const uint8_t *fb_bytes;
    uint8_t i;

    /* Arm state feedback */
    fb[0] = ARMP_RAD2DEG(g_motor_state[0].angle_rad);
    fb[1] = ARMP_RAD2DEG(g_motor_state[1].angle_rad);
    fb[2] = ARMP_RAD2DEG(g_motor_state[2].angle_rad);
    fb[3] = (float)Relay_GetState();
    fb[4] = (float)s_ctx.fragment_index;
    fb[5] = 0.0f;  /* status: OK */

    /* Build buffer: cmd + 24 bytes floats + 4 bytes tail */
    s_ctx.tx_buf[0] = cmd;
    fb_bytes = (const uint8_t *)fb;
    for (i = 0; i < 24; i++) {
        s_ctx.tx_buf[1 + i] = fb_bytes[i];
    }
    s_ctx.tx_buf[25] = 0x00U;
    s_ctx.tx_buf[26] = 0x00U;
    s_ctx.tx_buf[27] = 0x80U;
    s_ctx.tx_buf[28] = 0x7FU;
}

/**
 * @brief Set motor targets and force immediate CAN send next tick.
 */
static void arm_task_set_targets(const float angles[3])
{
    uint8_t i;
    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        g_arm.motor[i].target_angle_rad  = angles[i];
        g_arm.motor[i].last_sent_angle_rad = -1.0f;  /* force send */
        g_arm.motor[i].speed_mode        = 0;         /* ensure angle mode */
    }
}

/**
 * @brief Enable all 3 motors (called once at task start).
 */
static void arm_task_enable_all(void)
{
    uint8_t i;
    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        QGimbal_Enable(i);
        g_arm.motor[i].enabled      = 1;
        g_arm.motor[i].enable_retry = 0;
        g_arm.motor[i].speed_mode   = 0;  /* ensure angle mode for task */
    }
    g_arm.all_enabled = 1;
}

/**
 * @brief Disable all motors + release relay (cleanup on abort/complete).
 */
static void arm_task_disable_all(void)
{
    uint8_t i;
    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        QGimbal_Disable(i);
        g_arm.motor[i].enabled = 0;
    }
    g_arm.all_enabled = 0;
    Relay_Off();
}

/**
 * @brief Complete the task: deregister callback, disable motors, go idle.
 */
static void arm_task_complete(void)
{
    arm_jf_set_frame_callback((arm_jf_frame_cb_t)0);
    g_arm.rk3588_active = 0;
    arm_task_disable_all();
    s_ctx.state = ARMP_IDLE;
    s_ctx.trace_mode = 0;
}

/* ================================================================ */
/*  RK3588 JustFloat frame callback (UART2 ISR context, prio 1)      */
/* ================================================================ */

/**
 * @brief Called from arm_control's JustFloat parser when a valid
 *        6-float frame arrives from RK3588.
 *
 * Only acts when the state machine is in ARMP_WAIT_TARGET.
 * Checks sentinel (-1.0 in angles[0]) for "no more fragments".
 */
static void arm_task_on_rk3588_frame(const float angles[6])
{
    if (s_ctx.state != ARMP_WAIT_TARGET) {
        return;  /* ignore frames when not waiting */
    }

    /* Sentinel check: angles[0] == -1.0f means "no more fragments/waypoints" */
    if (angles[0] <= ARMP_SENTINEL_NO_FRAGMENTS) {
        s_ctx.state = ARMP_RETURN_ZERO;
        s_ctx.state_enter_ms = sys_tick_ms;
        return;
    }

    if (s_ctx.trace_mode) {
        /* Trace mode: angles[0-2] = waypoint (deg from RK3588 -> rad internal) */
        s_ctx.pick_angles[0]  = ARMP_DEG2RAD(angles[0]);
        s_ctx.pick_angles[1]  = ARMP_DEG2RAD(angles[1]);
        s_ctx.pick_angles[2]  = ARMP_DEG2RAD(angles[2]);
        s_ctx.state = ARMP_TRACE_MOVE_TO_WAYPOINT;
    } else {
        /* Pick-and-place: angles[0-2]=pick, angles[3-5]=place (deg -> rad) */
        s_ctx.pick_angles[0]  = ARMP_DEG2RAD(angles[0]);
        s_ctx.pick_angles[1]  = ARMP_DEG2RAD(angles[1]);
        s_ctx.pick_angles[2]  = ARMP_DEG2RAD(angles[2]);
        s_ctx.place_angles[0] = ARMP_DEG2RAD(angles[3]);
        s_ctx.place_angles[1] = ARMP_DEG2RAD(angles[4]);
        s_ctx.place_angles[2] = ARMP_DEG2RAD(angles[5]);

        s_ctx.state = ARMP_MOVE_TO_PICK;
    }
}

/* ================================================================ */
/*  Public API                                                       */
/* ================================================================ */

void ArmTask_Init(void)
{
    s_ctx.state          = ARMP_IDLE;
    s_ctx.fragment_index = 0;
    s_ctx.state_enter_ms = 0;
    s_ctx.task_start_ms  = 0;
    s_ctx.trace_mode     = 0;
}

void ArmTask_Run(void)
{
    uint32_t elapsed;

    /* ---- Timeout check (only in WAIT_TARGET state) ---- */
    if (s_ctx.state == ARMP_WAIT_TARGET) {
        elapsed = sys_tick_ms - s_ctx.state_enter_ms;
        if (elapsed >= ARMP_RK3588_TIMEOUT_MS) {
            s_ctx.state = ARMP_RETURN_ZERO;
            s_ctx.state_enter_ms = sys_tick_ms;
        }
    }

    switch (s_ctx.state) {

    /* ---- IDLE: nothing to do ---- */
    case ARMP_IDLE:
        break;

    /* ---- Send START command to RK3588 ---- */
    case ARMP_SEND_START:
        s_ctx.fragment_index = 0;
        arm_task_enable_all();
        if (s_ctx.trace_mode) {
            arm_task_build_tx_frame(ARMP_CMD_TRACE);
        } else {
            arm_task_build_tx_frame(s_task_cmd);  /* 0x01=chess, 0x05=poker */
        }
        arm_uart2_send_bytes(s_ctx.tx_buf, ARMP_TX_FRAME_BYTES);
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_WAIT_TARGET;
        break;

    /* ---- Waiting for RK3588 JustFloat frame ---- */
    case ARMP_WAIT_TARGET:
        /* State changes only via arm_task_on_rk3588_frame() callback */
        break;

    /* ---- Set pick-up targets, begin moving ---- */
    case ARMP_MOVE_TO_PICK:
        arm_task_set_targets(s_ctx.pick_angles);
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_WAIT_PICK_ARRIVAL;
        break;

    /* ---- Wait until arm reaches pick-up position ---- */
    case ARMP_WAIT_PICK_ARRIVAL:
        if (arm_is_at_target(ARMP_MOVE_DEADBAND_RAD)) {
            s_ctx.lift_next_state = ARMP_PICK;
            s_ctx.state_enter_ms = sys_tick_ms;
            s_ctx.state = ARMP_LIFT_DOWN;
        } else {
            elapsed = sys_tick_ms - s_ctx.state_enter_ms;
            if (elapsed >= ARMP_ARRIVAL_TIMEOUT_MS) {
                s_ctx.state = ARMP_ERROR;
            }
        }
        break;

    /* ---- Magnet settling: wait PICK_DELAY_MS ---- */
    case ARMP_PICK:
        Relay_On();
        elapsed = sys_tick_ms - s_ctx.state_enter_ms;
        if (elapsed >= ARMP_PICK_DELAY_MS) {
            s_ctx.lift_next_state = ARMP_MOVE_TO_PLACE;
            s_ctx.state_enter_ms = sys_tick_ms;
            s_ctx.state = ARMP_LIFT_UP;
        }
        break;

    /* ---- Set place targets, begin moving ---- */
    case ARMP_MOVE_TO_PLACE:
        arm_task_set_targets(s_ctx.place_angles);
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_WAIT_PLACE_ARRIVAL;
        break;

    /* ---- Wait until arm reaches place position ---- */
    case ARMP_WAIT_PLACE_ARRIVAL:
        if (arm_is_at_target(ARMP_MOVE_DEADBAND_RAD)) {
            s_ctx.lift_next_state = ARMP_PLACE;
            s_ctx.state_enter_ms = sys_tick_ms;
            s_ctx.state = ARMP_LIFT_DOWN;
        } else {
            elapsed = sys_tick_ms - s_ctx.state_enter_ms;
            if (elapsed >= ARMP_ARRIVAL_TIMEOUT_MS) {
                s_ctx.state = ARMP_ERROR;
            }
        }
        break;

    /* ---- Magnet release: wait PLACE_DELAY_MS ---- */
    case ARMP_PLACE:
        Relay_Off();
        elapsed = sys_tick_ms - s_ctx.state_enter_ms;
        if (elapsed >= ARMP_PLACE_DELAY_MS) {
            s_ctx.fragment_index++;
            if (s_ctx.fragment_index < ARMP_MAX_FRAGMENTS) {
                s_ctx.lift_next_state = ARMP_SEND_CONTINUE;
            } else {
                s_ctx.lift_next_state = ARMP_RETURN_ZERO;
            }
            s_ctx.state_enter_ms = sys_tick_ms;
            s_ctx.state = ARMP_LIFT_UP;
        }
        break;

    /* ---- Lower servo before pick/place ---- */
    case ARMP_LIFT_DOWN:
        Servo_LiftLower();
        elapsed = sys_tick_ms - s_ctx.state_enter_ms;
        if (elapsed >= ARMP_LIFT_DELAY_MS) {
            s_ctx.state_enter_ms = sys_tick_ms;
            s_ctx.state = s_ctx.lift_next_state;
        }
        break;

    /* ---- Raise servo after pick/place ---- */
    case ARMP_LIFT_UP:
        Servo_LiftRaise();
        elapsed = sys_tick_ms - s_ctx.state_enter_ms;
        if (elapsed >= ARMP_LIFT_DELAY_MS) {
            s_ctx.state_enter_ms = sys_tick_ms;
            s_ctx.state = s_ctx.lift_next_state;
        }
        break;

    /* ---- Send 0x02 CONTINUE, request next fragment ---- */
    case ARMP_SEND_CONTINUE:
        arm_task_build_tx_frame(ARMP_CMD_CONTINUE);
        arm_uart2_send_bytes(s_ctx.tx_buf, ARMP_TX_FRAME_BYTES);
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_WAIT_TARGET;
        break;

    /* ---- Return arm to zero/home position ---- */
    case ARMP_RETURN_ZERO:
        {
            float zeros[3] = {ARMP_ZERO_ANGLE_RAD, ARMP_ZERO_ANGLE_RAD, ARMP_ZERO_ANGLE_RAD};
            arm_task_set_targets(zeros);
        }
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_WAIT_ZERO_ARRIVAL;
        break;

    /* ---- Wait for arm to reach zero ---- */
    case ARMP_WAIT_ZERO_ARRIVAL:
        if (arm_is_at_target(ARMP_MOVE_DEADBAND_RAD)) {
            s_ctx.state = ARMP_SEND_COMPLETE;
        } else {
            elapsed = sys_tick_ms - s_ctx.state_enter_ms;
            if (elapsed >= ARMP_ARRIVAL_TIMEOUT_MS) {
                s_ctx.state = ARMP_SEND_COMPLETE;  /* timeout: still send complete */
            }
        }
        break;

    /* ---- Send 0x03 COMPLETE, task done ---- */
    case ARMP_SEND_COMPLETE:
        arm_task_build_tx_frame(ARMP_CMD_TASK_COMPLETE);
        arm_uart2_send_bytes(s_ctx.tx_buf, ARMP_TX_FRAME_BYTES);
        arm_task_complete();
        beep_on_1s_flag = 1;  /* task complete, beep 1s */
        break;

    /* ---- Error: disable motors, release relay, go idle ---- */
    case ARMP_ERROR:
        arm_task_disable_all();
        arm_jf_set_frame_callback((arm_jf_frame_cb_t)0);
        g_arm.rk3588_active = 0;
        s_ctx.state = ARMP_IDLE;
        s_ctx.trace_mode = 0;
        break;

    /* ---- Trace: set waypoint targets, begin moving ---- */
    case ARMP_TRACE_MOVE_TO_WAYPOINT:
        arm_task_set_targets(s_ctx.pick_angles);
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_TRACE_WAIT_ARRIVAL;
        break;

    /* ---- Trace: wait for arrival at waypoint ---- */
    case ARMP_TRACE_WAIT_ARRIVAL:
        if (arm_is_at_target(ARMP_MOVE_DEADBAND_RAD)) {
            s_ctx.fragment_index++;
            s_ctx.state = ARMP_TRACE_SEND_CONTINUE;
        } else {
            elapsed = sys_tick_ms - s_ctx.state_enter_ms;
            if (elapsed >= ARMP_ARRIVAL_TIMEOUT_MS) {
                s_ctx.state = ARMP_ERROR;
            }
        }
        break;

    /* ---- Trace: send 0x02 CONTINUE, request next waypoint ---- */
    case ARMP_TRACE_SEND_CONTINUE:
        arm_task_build_tx_frame(ARMP_CMD_CONTINUE);
        arm_uart2_send_bytes(s_ctx.tx_buf, ARMP_TX_FRAME_BYTES);
        s_ctx.state_enter_ms = sys_tick_ms;
        s_ctx.state = ARMP_WAIT_TARGET;
        break;

    default:
        s_ctx.state = ARMP_IDLE;
        break;
    }
}

void ArmTask_Start(void)
{
    if (s_ctx.state != ARMP_IDLE) {
        return;  /* already running */
    }

    /* Reset context */
    s_ctx.fragment_index = 0;
    s_ctx.trace_mode     = 0;
    s_ctx.state_enter_ms = 0;
    s_ctx.task_start_ms  = sys_tick_ms;
    s_task_cmd           = ARMP_CMD_TASK_START;  /* chess */

    /* Register JustFloat callback to receive RK3588 frames */
    arm_jf_set_frame_callback(arm_task_on_rk3588_frame);

    /* Audible task start */
    beep_on_1s_flag = 1;

    /* Kick off state machine */
    s_ctx.state = ARMP_SEND_START;
}

void ArmTask_StartPoker(void)
{
    if (s_ctx.state != ARMP_IDLE) {
        return;  /* already running */
    }

    /* Reset context */
    s_ctx.fragment_index = 0;
    s_ctx.trace_mode     = 0;
    s_ctx.state_enter_ms = 0;
    s_ctx.task_start_ms  = sys_tick_ms;
    s_task_cmd           = ARMP_CMD_POKER_START;  /* poker */

    /* Register JustFloat callback to receive RK3588 frames */
    arm_jf_set_frame_callback(arm_task_on_rk3588_frame);

    /* Audible task start */
    beep_on_1s_flag = 1;

    /* Kick off state machine */
    s_ctx.state = ARMP_SEND_START;
}

void ArmTask_StartTrace(void)
{
    if (s_ctx.state != ARMP_IDLE) {
        return;  /* already running */
    }

    /* Reset context */
    s_ctx.fragment_index = 0;
    s_ctx.trace_mode     = 1;
    s_ctx.state_enter_ms = 0;
    s_ctx.task_start_ms  = sys_tick_ms;

    /* Register JustFloat callback to receive RK3588 waypoint frames */
    arm_jf_set_frame_callback(arm_task_on_rk3588_frame);

    /* Audible task start */
    beep_on_1s_flag = 1;

    /* Kick off state machine */
    s_ctx.state = ARMP_SEND_START;
}

void ArmTask_Abort(void)
{
    arm_task_disable_all();
    arm_jf_set_frame_callback((arm_jf_frame_cb_t)0);
    g_arm.rk3588_active = 0;
    s_ctx.state = ARMP_IDLE;
    s_ctx.trace_mode = 0;
}

uint8_t ArmTask_IsActive(void)
{
    return (s_ctx.state != ARMP_IDLE) ? 1 : 0;
}

/* ================================================================ */
/*  VOFA command handler (P52-P53)                                   */
/* ================================================================ */

uint32_t ArmTask_GetElapsedSec(void)
{
    if (s_ctx.state == ARMP_IDLE) return 0;
    return (sys_tick_ms - s_ctx.task_start_ms) / 1000;
}

uint8_t ArmTask_HandleVofa(uint16_t id, float value)
{
    /* P52: start arm pick-and-place task */
    if (id == 52 && value != 0.0f) {
        ArmTask_Start();
        return 1;
    }

    /* P53: abort arm task (emergency stop) */
    if (id == 53 && value != 0.0f) {
        ArmTask_Abort();
        return 1;
    }

    /* P60: start shape tracing mode */
    if (id == 60 && value != 0.0f) {
        ArmTask_StartTrace();
        return 1;
    }

    /* P61: lift servo control (0=raise, !=0=lower) */
    if (id == 61) {
        if (value == 0.0f) {
            Servo_LiftRaise();
        } else {
            Servo_LiftLower();
        }
        return 1;
    }

    return 0;  /* ID not consumed */
}
