/**
 * @file arm_control.c
 * @brief 3-QD4310 robotic arm control �?? CAN + VOFA + RK3588 UART
 *
 * Reuses hal_qgimbal_can.c (same QD4310 CAN protocol) without modification.
 * Provides:
 *   - UART2_IRQHandler  �?? VOFA ASCII passthrough to rx_fifo (RK3588 @ 115200, 8X)
 *   - Arm_Run()         �?? 10ms CAN angle sender
 *   - Arm_HandleVofa()  �?? VOFA #P42..P51 command handler
 */

#include "arm_control.h"
#include "arm_protocol.h"
#include "Serial.h"
#include "arm_task.h"
#include "arm_home.h"
#include "end_effector.h"
#include "telemetry_proto.h"
#include "hal_qgimbal_can.h"
#include "hal_relay.h"
#include "Buzzer.h"
#include "vofa.h"
#include "ti_msp_dl_config.h"
#include <math.h>

/* ---- sys_tick_ms (declared in main.c, incremented in TIMER_0 ISR) ---- */
extern volatile uint32_t sys_tick_ms;

/* ---- constants ---- */

#define PI_F              3.1415926f
#define TWO_PI_F          (PI_F * 2.0f)
#define DEG2RAD(d)        ((d) * PI_F / 180.0f)
#define RAD2DEG(r)        ((r) * 180.0f / PI_F)

#define ANGLE_EPSILON      0.0002f    /* 0.01 deg deadband �?? skip duplicate CAN send */
#define SPEED_EPSILON       0.1f    /* 0.1 rpm deadband �?? skip duplicate CAN speed send */
#define ENABLE_RETRY_MAX   200      /* re-enable attempts on feedback loss */

/* RK3588 JustFloat frame: 6 floats (3 angles + 3 speeds) */
#define RK3588_FLOAT_COUNT  6
#define RK3588_DATA_BYTES   (RK3588_FLOAT_COUNT * 4)   /* 24 bytes */

/* UART2 RX buffer for JustFloat parser */
#define UART2_RX_BUF_SIZE   64

/* ---- JustFloat frame tail (IEEE 754 +Infinity, little-endian) ---- */
#define JF_TAIL_0  0x00U
#define JF_TAIL_1  0x00U
#define JF_TAIL_2  0x80U
#define JF_TAIL_3  0x7FU

/* ---- parser state machine ---- */
enum {
    ARM_JF_COLLECT = 0,   /* collecting data bytes */
    ARM_JF_TAIL_0,        /* saw 0x00 */
    ARM_JF_TAIL_1,        /* saw 0x00 0x00 */
    ARM_JF_TAIL_2,        /* saw 0x00 0x00 0x80 */
    ARM_JF_TAIL_3         /* saw 0x00 0x00 0x80 0x7F �?? frame done */
};

/* ---- static parser state ---- */
static uint8_t  jf_state     = ARM_JF_COLLECT;
static uint8_t  jf_buf[UART2_RX_BUF_SIZE];
static uint16_t jf_data_cnt  = 0;

/* ---- global instance ---- */
ArmController g_arm;

/* ---- JustFloat frame callback (arm_task registers to receive RK3588 frames) ---- */
static arm_jf_frame_cb_t s_arm_jf_cb = (arm_jf_frame_cb_t)0;

void arm_jf_set_frame_callback(arm_jf_frame_cb_t cb)
{
    s_arm_jf_cb = cb;
}

/* ---- UART2 TX (to RK3588) ---- */

void arm_uart2_send_byte(uint8_t data)
{
    while ((UART_wired_INST->STAT & UART_STAT_TXFE_MASK) == 0);
    DL_UART_Main_transmitData(UART_wired_INST, data);
}

void arm_uart2_send_bytes(const uint8_t *data, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++) {
        arm_uart2_send_byte(data[i]);
    }
}

/**
 * @brief Send N floats as JustFloat frame on UART2 (to RK3588).
 *
 * Pure JustFloat: [N x float LE] [00 00 80 7F] tail, NO cmd byte.
 * Uses blocking arm_uart2_send_byte() — call from main loop only,
 * NOT from ISR.
 *
 * @param data   Pointer to float array.
 * @param count  Number of floats to send.
 */
void arm_uart2_send_floats(const float *data, uint8_t count)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint16_t total = (uint16_t)count * 4u;
    uint16_t i;

    /* ---- float data (little-endian, no packing needed on Cortex-M) ---- */
    for (i = 0; i < total; i++) {
        arm_uart2_send_byte(bytes[i]);
    }

    /* ---- JustFloat tail: IEEE 754 +Infinity in little-endian ---- */
    arm_uart2_send_byte(0x00U);
    arm_uart2_send_byte(0x00U);
    arm_uart2_send_byte(0x80U);
    arm_uart2_send_byte(0x7FU);
}

/* ---- motor arrival check ---- */

/**
 * @brief Angle error with wrap-around for 0/2pi discontinuity.
 * Differences crossing the 0/2pi boundary are handled correctly
 * so that angles like 6.27 rad and 0.01 rad yield ~0.023 rad error.
 */
static float angle_error_abs(float actual, float target)
{
    float diff = actual - target;
    if (diff < 0.0f) diff = -diff;
    if (diff > PI_F) diff = TWO_PI_F - diff;
    return diff;
}

uint8_t arm_is_at_target(float threshold_rad)
{
    uint8_t i;
    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        if (!g_arm.motor[i].enabled) continue;
        if (angle_error_abs(g_motor_state[i].angle_rad,
                            g_arm.motor[i].target_angle_rad) > threshold_rad) {
            return 0;
        }
    }
    return 1;
}

/* ---- static helpers ---- */

/**
 * @brief Parse 4 little-endian bytes into a float (no stdlib dependency).
 */
static float parse_float_le(const uint8_t *b)
{
    union { float f; uint32_t u; } v;
    v.u = (uint32_t)b[0]
        | ((uint32_t)b[1] << 8)
        | ((uint32_t)b[2] << 16)
        | ((uint32_t)b[3] << 24);
    return v.f;
}

/**
 * @brief Process one byte through the JustFloat frame parser.
 *
 * Called from UART2_IRQHandler.  When a complete 24-byte + tail frame
 * is detected, parses the 6 floats and updates g_arm.rk3588_*.
 *
 * Frame format (RK3588 �?? MSPM0):
 *   [m0_deg] [m1_deg] [m2_deg] [m0_rpm] [m1_rpm] [m2_rpm] [00 00 80 7F]
 *    4B LE    4B LE    4B LE    4B LE     4B LE     4B LE     4B tail
 */
static void arm_jf_parse_byte(uint8_t byte)
{
    switch (jf_state) {

    case ARM_JF_COLLECT:
        if (byte == JF_TAIL_0) {
            jf_state = ARM_JF_TAIL_0;
        } else {
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = byte;
            } else {
                /* buffer overflow �?? reset */
                jf_data_cnt = 0;
            }
        }
        break;

    case ARM_JF_TAIL_0:
        if (byte == JF_TAIL_1) {
            jf_state = ARM_JF_TAIL_1;
        } else {
            /* false tail �?? rewind tail byte + current byte into buffer */
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = JF_TAIL_0;
            }
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = byte;
            }
            jf_state = ARM_JF_COLLECT;
        }
        break;

    case ARM_JF_TAIL_1:
        if (byte == JF_TAIL_2) {
            jf_state = ARM_JF_TAIL_2;
        } else {
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = JF_TAIL_0;
            }
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = JF_TAIL_1;
            }
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = byte;
            }
            jf_state = ARM_JF_COLLECT;
        }
        break;

    case ARM_JF_TAIL_2:
        if (byte == JF_TAIL_3) {
            /* Tail complete! Verify data length before accepting. */
            jf_state = ARM_JF_TAIL_3;
            if (jf_data_cnt == RK3588_DATA_BYTES) {
                /* Valid frame �?? parse 6 floats */
                uint8_t i;
                float parsed[6];
                for (i = 0; i < 6; i++) {
                    parsed[i] = parse_float_le(&jf_buf[i * 4]);
                }
                /* Always cache in g_arm for backward compatibility */
                for (i = 0; i < 3; i++) {
                    g_arm.rk3588_angles[i] = parsed[i];
                    g_arm.rk3588_speeds[i] = parsed[3 + i];
                }
                /* Route to task callback if registered, else legacy mode */
                if (s_arm_jf_cb) {
                    s_arm_jf_cb(parsed);
                } else {
                    g_arm.rk3588_active = 1;
                }
            }
            /* Reset for next frame regardless */
            jf_data_cnt = 0;
            jf_state = ARM_JF_COLLECT;
        } else {
            /* false tail �?? rewind accumulated tail bytes + current byte */
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = JF_TAIL_0;
            }
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = JF_TAIL_1;
            }
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = JF_TAIL_2;
            }
            if (jf_data_cnt < UART2_RX_BUF_SIZE) {
                jf_buf[jf_data_cnt++] = byte;
            }
            jf_state = ARM_JF_COLLECT;
        }
        break;

    default:
        /* safety reset */
        jf_state    = ARM_JF_COLLECT;
        jf_data_cnt = 0;
        break;
    }
}

/* ================================================================ */
/*  Public API                                                       */
/* ================================================================ */

/**
 * @brief Initialize arm controller: CAN, UART2 baud, motors.
 *
 * Must be called after SYSCFG_DL_init() (which configures UART2
 * pins and clock).  Safe to call after Gimbal_Init() �?? CAN init
 * is idempotent.
 */
void Arm_Init(void)
{
    uint8_t i;

    /* ---- CAN (shared with gimbal) ---- */
    QGimbal_CAN_Init();

    /* ---- Motor state init ---- */
    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        g_arm.motor[i].target_angle_rad  = 0.0f;
        g_arm.motor[i].last_sent_angle_rad = 0.0f;   /* match default target — no auto-send on enable */
        g_arm.motor[i].target_speed_rpm  = 0.0f;
        g_arm.motor[i].last_sent_speed_rpm = -1.0f;
        g_arm.motor[i].enabled           = 0;
        g_arm.motor[i].speed_mode        = 0;
        g_arm.motor[i].enable_retry      = 0;
    }

    g_arm.all_enabled      = 0;   /* motors start DISABLED �?? enable via VOFA P42=1 */
    g_arm.mode             = 0;
    g_arm.telemetry_enabled       = 0;
    g_arm.telemetry_angles_active = 0;
    g_arm.rk3588_active           = 0;

    /* ---- sinusoidal speed tracking init ---- */
    g_arm.sine_mode        = 0;
    g_arm.sine_amplitude   = 0.0f;
    g_arm.sine_period      = 100;
    g_arm.sine_step        = 0;
    g_arm.sine_motor_mask  = 0x07;  /* default: all 3 motors */
    /* ---- RK3588 angles/speeds init ---- */
    for (i = 0; i < 3; i++) {
        g_arm.rk3588_angles[i] = 0.0f;
        g_arm.rk3588_speeds[i] = 0.0f;
    }

    /* ---- Reconfigure UART2 baud: 9600 �?? 115200 @ 8X ---- */
    /* BUSCLK = 40 MHz, divisor = 40M / (8 * 115200) = 43.403 */
    DL_UART_setOversampling(UART_wired_INST,
        DL_UART_OVERSAMPLING_RATE_8X);
    DL_UART_setBaudRateDivisor(UART_wired_INST, 43, 26);

    /* ---- JustFloat parser init ---- */
    jf_state    = ARM_JF_COLLECT;
    jf_data_cnt = 0;
}

/**
 * @brief 10ms control loop �?? send angle commands to enabled motors.
 *
 * Called from TIMER_1 ISR (10ms period, NVIC priority 2).
 * Sources target angles from RK3588 when active, else from VOFA.
 */
void Arm_Run(void)
{
    uint8_t i;

    /* ---- one-shot: send set-zero to all motors on first tick ---- */
    ArmHome_Run();
    EndEffector_Run();

    if (!g_arm.all_enabled) {
        return;
    }

    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        ArmMotor *m = &g_arm.motor[i];
        float target;
        float diff;
        uint8_t command_sent = 0U;

        if (!m->enabled) {
            continue;
        }

        /* ---- enable-retry: heal motors that lost enable state ---- */
        if (!g_motor_state[i].enabled && m->enable_retry < ENABLE_RETRY_MAX) {
            QGimbal_Enable(i);
            m->enable_retry++;
            command_sent = 1U;
            continue;   /* skip angle send this cycle — let enable take effect */
        }

        /* ---- speed mode: send CMD_SPEED (manual debugging, task overrides) ---- */
        if (m->speed_mode && !ArmTask_IsActive()) {
            float speed_cmd;
            float speed_diff;

            if (g_arm.sine_mode && g_arm.sine_period > 0
                && (g_arm.sine_motor_mask & (1 << i))) {
                /* sinusoidal speed tracking — this motor is selected */
                float phase = 2.0f * PI_F * (float)g_arm.sine_step
                            / (float)g_arm.sine_period;
                speed_cmd = g_arm.sine_amplitude * sinf(phase);
            } else {
                speed_cmd = m->target_speed_rpm;
            }

            speed_diff = speed_cmd - m->last_sent_speed_rpm;
            if (speed_diff < 0.0f) speed_diff = -speed_diff;
            if (speed_diff >= SPEED_EPSILON) {
                QGimbal_SetSpeed(i, speed_cmd);
                m->last_sent_speed_rpm = speed_cmd;
                command_sent = 1U;
            }
        }

        /* ---- angle path: active when NOT in speed mode, OR task is running ---- */
        if (!command_sent && (!m->speed_mode || ArmTask_IsActive())) {
            /* ---- select angle source ---- */
            if (g_arm.rk3588_active) {
                target = g_arm.rk3588_angles[i];
            } else {
                target = m->target_angle_rad;
            }

            /* ---- debounce: skip if angle hasn't changed meaningfully ---- */
            diff = target - m->last_sent_angle_rad;
            if (diff < 0.0f) diff = -diff;
            if (diff >= ANGLE_EPSILON) {
                QGimbal_SetAngle(i, target);
                m->last_sent_angle_rad = target;
                command_sent = 1U;
            }
        }

        /*
         * If no control command was sent this tick, send a NOP to keep the
         * feedback channel alive. QD4310 only replies to received commands;
         * without this, angle_rad freezes when the target is stable.
         */
        if (!command_sent) {
            QGimbal_RequestFeedback(i);
        }
    }

    /* ---- advance sine step each 10ms tick (shared by all motors) ---- */
    if (g_arm.sine_mode) {
        g_arm.sine_step++;
    }
}

/**
 * @brief VOFA parameter handler for arm control (P42-P51).
 * @return 1 if the ID was consumed, 0 otherwise.
 */
uint8_t Arm_HandleVofa(uint16_t id, float value)
{
    uint8_t m;

    /* P42: enable/disable ALL 3 motors */
    if (id == 42) {
        if (value != 0.0f) {
            uint8_t j;
            for (j = 0; j < ARM_MOTOR_COUNT; j++) {
                QGimbal_Enable(j);
                g_arm.motor[j].enabled      = 1;
                g_arm.motor[j].enable_retry = 0;
            }
            g_arm.all_enabled = 1;
        } else {
            uint8_t j;
            for (j = 0; j < ARM_MOTOR_COUNT; j++) {
                QGimbal_Disable(j);
                g_arm.motor[j].enabled = 0;
            }
            g_arm.all_enabled = 0;
        }
        return 1;
    }

    /* P43-P45: enable/disable motor 0-2 individually */
    if (id >= 43 && id <= 45) {
        m = (uint8_t)(id - 43);
        if (value != 0.0f) {
            QGimbal_Enable(m);
            g_arm.motor[m].enabled      = 1;
            g_arm.motor[m].enable_retry = 0;
            g_arm.all_enabled           = 1;
        } else {
            QGimbal_Disable(m);
            g_arm.motor[m].enabled = 0;
        }
        return 1;
    }

    /* P46-P48: target angle motor 0-2 (radians, wrapped to [0, 2*PI)) */
    if (id >= 46 && id <= 48) {
        m = (uint8_t)(id - 46);
        /* Wrap to [0, 2*PI) */
        while (value < 0.0f)      value += TWO_PI_F;
        while (value >= TWO_PI_F) value -= TWO_PI_F;
        g_arm.motor[m].target_angle_rad = value;
        g_arm.motor[m].speed_mode       = 0;    /* exit speed mode */
        g_arm.motor[m].last_sent_angle_rad = -1.0f;  /* force immediate send */

        /* Auto-enable motor + switch to VOFA source */
        QGimbal_Enable(m);
        g_arm.motor[m].enabled      = 1;
        g_arm.motor[m].enable_retry = 0;
        g_arm.all_enabled           = 1;
        g_arm.rk3588_active         = 0;   /* VOFA takes priority */
        return 1;
    }

    /* P49-P51: target speed motor 0-2 (rpm, manual debug mode) */
    if (id >= 49 && id <= 51) {
        m = (uint8_t)(id - 49);
        g_arm.motor[m].target_speed_rpm   = value;
        g_arm.motor[m].speed_mode         = 1;
        g_arm.motor[m].last_sent_speed_rpm = -1.0f;  /* force first send */
        g_arm.rk3588_active               = 0;        /* VOFA takes priority */

        /* Auto-enable motor */
        QGimbal_Enable(m);
        g_arm.motor[m].enabled      = 1;
        g_arm.motor[m].enable_retry = 0;
        g_arm.all_enabled           = 1;
        return 1;
    }

    /* P54: arm VOFA telemetry toggle (0=off, 1=on) */
    if (id == 54) {
        g_arm.telemetry_enabled = (value != 0.0f) ? 1 : 0;
        return 1;
    }

    /* P55: sinusoidal speed amplitude (rpm). >0 enables, 0 disables */
    if (id == 55) {
        g_arm.sine_amplitude = value;
        if (value > 0.0f) {
            g_arm.sine_mode = 1;
            g_arm.sine_step = 0;  /* reset phase on enable */
        } else {
            g_arm.sine_mode = 0;
        }
        return 1;
    }

    /* P56: sinusoidal speed period N (steps per cycle, min 1) */
    if (id == 56) {
        if (value < 1.0f) value = 1.0f;
        g_arm.sine_period = (uint16_t)value;
        return 1;
    }

    /* P57: sine motor bitmask (bit0=m0, bit1=m1, bit2=m2) */
    if (id == 57) {
        g_arm.sine_motor_mask = (uint8_t)value & 0x07;
        return 1;
    }

    /* P58: manual set-zero (re-trigger hardware set-zero on all motors) */
    if (id == 58 && value != 0.0f) {
        for (m = 0; m < ARM_MOTOR_COUNT; m++) {
            QGimbal_SetZero(m);
        }
        return 1;
    }

    /* P64: relay manual test (0=off, !=0=on) */
    if (id == 64) {
        if (value != 0.0f) {
            Relay_On();
        } else {
            Relay_Off();
        }
        return 1;
    }

    /* P65: buzzer test (0=off, !=0=on) */
    if (id == 65) {
        if (value != 0.0f) {
            Buzzer_on();
        } else {
            Buzzer_off();
        }
        return 1;
    }

    /* P68: toggle continuous 3ch angle telemetry on UART0 (1=start, 0=stop) */
    if (id == 68) {
        g_arm.telemetry_angles_active = (value != 0.0f) ? 1 : 0;
        /* Send one-shot NOP to refresh angles immediately on enable */
        if (g_arm.telemetry_angles_active && g_arm.all_enabled) {
            uint8_t m;
            for (m = 0; m < ARM_MOTOR_COUNT; m++) {
                QGimbal_SendCommand(m, QGIMBAL_CMD_NOP, 0);
            }
        }
        /* Also send one frame now so the user sees immediate response */
        Telemetry_SendAngles();
        return 1;
    }

    /* P69: send 3 motor full state (angle+speed+current) via custom protocol */
    if (id == 69 && value != 0.0f) {
        if (g_arm.all_enabled) {
            uint8_t m;
            for (m = 0; m < ARM_MOTOR_COUNT; m++) {
                QGimbal_SendCommand(m, QGIMBAL_CMD_NOP, 0);
            }
            {
                volatile uint32_t wait_start = sys_tick_ms;
                while ((sys_tick_ms - wait_start) < 20);
            }
        }
        Telemetry_SendMotorState();
        return 1;
    }

    return 0;   /* ID not consumed */
}

/**
 * @brief Send motor feedback telemetry via VOFA JustFloat (UART0).
 *
 * 6 channels:
 *   ch0-ch2: motor 0-2 actual angle (deg)
 *   ch3-ch5: motor 0-2 actual speed (rpm)
 */
void Arm_SendTelemetry(void)
{
    float ch[6];
    ch[0] = RAD2DEG(g_motor_state[0].angle_rad);          /* motor 0 angle (deg) */
    ch[1] = RAD2DEG(g_motor_state[1].angle_rad);          /* motor 1 angle (deg) */
    ch[2] = RAD2DEG(g_motor_state[2].angle_rad);          /* motor 2 angle (deg) */
    ch[3] = g_motor_state[0].speed_rpm;                   /* motor 0 speed (rpm) */
    ch[4] = g_motor_state[1].speed_rpm;                   /* motor 1 speed (rpm) */
    ch[5] = g_motor_state[2].speed_rpm;                   /* motor 2 speed (rpm) */
    vofa_send_floats(ch, 6);
}

/* ================================================================ */
/*  UART2 ISR �?? RK3588 JustFloat frame reception                     */
/* ================================================================ */

/**
 * @brief UART2 interrupt handler (replaces LoRa ISR in Serial.c).
 *
 * UART2 is repurposed for RK3588 communication at 115200 baud.
 * Each received byte is routed to the shared VOFA ASCII rx fifo.
 *
 * NVIC priority: 1 (higher than TIMER_1 at priority 2).
 */
void UART2_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_wired_INST)) {
    case DL_UART_IIDX_RX: {
        uint8_t byte = DL_UART_Main_receiveData(UART_wired_INST);
        arm_jf_parse_byte(byte);   /* route RK3588 JustFloat frames to dedicated parser */
        break;
    }
    default:
        break;
    }
}
