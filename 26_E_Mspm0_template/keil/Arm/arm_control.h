/**
 * @file arm_control.h
 * @brief 3-QD4310 robotic arm control module
 *
 * Dual input:
 *   - VOFA UART0  (#P=! text)  -- manual enable/angle test
 *   - RK3588 UART2 (JustFloat) -- real-time kinematics angles
 *
 * Output:
 *   - CAN (0x400-0x402) angle commands to QD4310 motors
 */

#ifndef __ARM_CONTROL_H
#define __ARM_CONTROL_H

#include <stdint.h>

#define ARM_MOTOR_COUNT 3

/* Per-motor control state */
typedef struct {
    float    target_angle_rad;      /* current target (VOFA or RK3588 source), radians */
    float    last_sent_angle_rad;   /* debounce: last value sent via CAN, radians */
    float    target_speed_rpm;      /* target speed (rpm) �? active when speed_mode=1 */
    float    last_sent_speed_rpm;   /* debounce: last speed value sent via CAN */
    uint8_t  enabled;               /* 1 = motor enabled */
    uint8_t  speed_mode;            /* 1 = send CMD_SPEED, 0 = send CMD_ANGLE */
    uint16_t enable_retry;          /* re-enable counter on feedback loss */
} ArmMotor;

/* Global arm controller */
typedef struct {
    ArmMotor motor[ARM_MOTOR_COUNT];
    uint8_t  all_enabled;           /* global enable/disable */
    uint8_t  mode;                  /* 0 = angle mode */
    uint8_t  telemetry_enabled;        /* 1 = send 6ch feedback via VOFA JustFloat (P54) */
    uint8_t  telemetry_angles_active;  /* 1 = send 3ch angle telemetry via VOFA JustFloat (P68) */
    uint8_t  rk3588_active;            /* 1 = receiving valid RK3588 frames */
    float    rk3588_angles[3];      /* latest angles from RK3588 (rad) */
    float    rk3588_speeds[3];      /* latest speeds from RK3588 (rpm) */

    /* ---- sinusoidal speed tracking ---- */
    uint8_t  sine_mode;             /* 1 = sinusoidal speed tracking active */
    float    sine_amplitude;        /* max speed amplitude (rpm), >0 enables */
    uint16_t sine_period;           /* N: steps per full sine cycle (default 100) */
    uint16_t sine_step;             /* running step counter i */
    uint8_t  sine_motor_mask;       /* bitmask: bit0=m0, bit1=m1, bit2=m2 */
    uint8_t  zero_set_pending;      /* 1 = send set-zero on next Arm_Run() */
} ArmController;

extern ArmController g_arm;

/* ---- Public API ---- */

void    Arm_Init(void);
void    Arm_Run(void);                                /* 10ms ISR call */
uint8_t Arm_HandleVofa(uint16_t id, float value);     /* returns 1 if consumed */
void    Arm_SendTelemetry(void);                      /* 6-ch JustFloat on UART0 */

/* ---- UART2 TX (to RK3588) ---- */
void    arm_uart2_send_byte(uint8_t data);
void    arm_uart2_send_bytes(const uint8_t *data, uint16_t len);
void    arm_uart2_send_floats(const float *data, uint8_t count);

/* ---- Motor arrival check ---- */
uint8_t arm_is_at_target(float threshold_rad);

/* ---- JustFloat frame callback (registered by arm_task) ---- */
typedef void (*arm_jf_frame_cb_t)(const float angles[6]);
void    arm_jf_set_frame_callback(arm_jf_frame_cb_t cb);

#endif /* __ARM_CONTROL_H__ */
