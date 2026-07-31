/**
 * @file arm_home.c
 * @brief Auto-home state machine — returns arm to workpiece coordinate on power-up
 *
 * Called from Arm_Run() in the 10ms TIMER_1 ISR.
 * Independent of the arm_task pick-and-place FSM.
 *
 * Workpiece coordinate angles are stored in a global variable (s_home.angles_rad).
 * Lost on power cycle — always starts from compile-time defaults on boot.
 * Updated at runtime via VOFA P62, reverted to defaults via VOFA P63.
 */

#include "arm_home.h"
#include "arm_control.h"
#include "arm_protocol.h"
#include "hal_qgimbal_can.h"
#include "vofa.h"

/* ---- sys_tick_ms (declared in main.c) ---- */
extern volatile uint32_t sys_tick_ms;

/* ---- static context ---- */
static AutoHomeContext s_home;

/* ================================================================ */
/*  Public API                                                       */
/* ================================================================ */

void ArmHome_Init(void)
{
    s_home.state          = AUTOHOME_IDLE;
    s_home.state_enter_ms = 0;
    s_home.home_performed = 0;

    /* Always use compile-time defaults at power-up */
    s_home.angles_rad[0] = AUTOHOME_DEFAULT_M0_RAD;
    s_home.angles_rad[1] = AUTOHOME_DEFAULT_M1_RAD;
    s_home.angles_rad[2] = AUTOHOME_DEFAULT_M2_RAD;

#if AUTO_HOME_ON_STARTUP
    s_home.state = AUTOHOME_WAIT_DELAY;
    s_home.state_enter_ms = sys_tick_ms;
#endif
}

uint8_t ArmHome_IsActive(void)
{
    return (s_home.state != AUTOHOME_IDLE &&
            s_home.state != AUTOHOME_DONE &&
            s_home.state != AUTOHOME_ERROR) ? 1 : 0;
}

void ArmHome_Run(void)
{
#if !AUTO_HOME_ON_STARTUP
    return;
#endif

    if (s_home.home_performed) return;
    if (s_home.state == AUTOHOME_IDLE) return;

    uint32_t elapsed = sys_tick_ms - s_home.state_enter_ms;

    switch (s_home.state) {

    case AUTOHOME_WAIT_DELAY:
        if (elapsed >= AUTOHOME_POWERUP_DELAY_MS) {
            s_home.state = AUTOHOME_START_MOTORS;
            s_home.state_enter_ms = sys_tick_ms;
        }
        break;

    case AUTOHOME_START_MOTORS:
        {
            uint8_t i;
            for (i = 0; i < ARM_MOTOR_COUNT; i++) {
                QGimbal_Enable(i);
                QGimbal_SetAngle(i, s_home.angles_rad[i]);
                g_arm.motor[i].enabled            = 1;
                g_arm.motor[i].target_angle_rad   = s_home.angles_rad[i];
                g_arm.motor[i].last_sent_angle_rad = s_home.angles_rad[i];
                g_arm.motor[i].speed_mode          = 0;
                g_arm.motor[i].enable_retry        = 0;
            }
            g_arm.all_enabled    = 1;
            g_arm.rk3588_active  = 0;  /* VOFA/auto-home takes priority */
        }
        s_home.state = AUTOHOME_WAIT_ARRIVAL;
        s_home.state_enter_ms = sys_tick_ms;
        break;

    case AUTOHOME_WAIT_ARRIVAL:
        if (arm_is_at_target(ARMP_MOVE_DEADBAND_RAD)) {
            s_home.state = AUTOHOME_DONE;
        } else {
            elapsed = sys_tick_ms - s_home.state_enter_ms;
            if (elapsed >= AUTOHOME_ARRIVAL_TIMEOUT_MS) {
                s_home.state = AUTOHOME_DONE;  /* timeout: finish anyway */
            }
        }
        break;

    case AUTOHOME_DONE:
        s_home.home_performed = 1;
        s_home.state = AUTOHOME_IDLE;
        /* Motors stay enabled, arm sits at workpiece coordinate */
        break;

    case AUTOHOME_ERROR:
        s_home.home_performed = 1;
        s_home.state = AUTOHOME_IDLE;
        break;

    default:
        break;
    }
}

/* ================================================================ */
/*  VOFA command handler (P62, P63)                                  */
/* ================================================================ */

uint8_t ArmHome_HandleVofa(uint16_t id, float value)
{
    /* P62: Save current motor angles to global variable (RAM only, lost on power cycle) */
    if (id == 62 && value != 0.0f) {
        s_home.angles_rad[0] = g_motor_state[0].angle_rad;
        s_home.angles_rad[1] = g_motor_state[1].angle_rad;
        s_home.angles_rad[2] = g_motor_state[2].angle_rad;

        /* Acknowledge: send saved angles back */
        vofa_send_floats(s_home.angles_rad, 3);
        return 1;
    }

    /* P63: Revert workpiece coordinate to compile-time defaults */
    if (id == 63 && value != 0.0f) {
        s_home.angles_rad[0] = AUTOHOME_DEFAULT_M0_RAD;
        s_home.angles_rad[1] = AUTOHOME_DEFAULT_M1_RAD;
        s_home.angles_rad[2] = AUTOHOME_DEFAULT_M2_RAD;

        /* Acknowledge */
        {
            float ack[1] = {0.0f};
            vofa_send_floats(ack, 1);
        }
        return 1;
    }

    return 0;  /* ID not consumed */
}
