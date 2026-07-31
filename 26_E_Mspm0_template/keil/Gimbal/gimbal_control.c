#include "gimbal_control.h"
#include "hal_qgimbal_can.h"
#include "imu_filter.h"

/* ---- global controller ---- */
GimbalController g_gimbal;

/* ---- PID parameters (scaled for 10ms from QGimbal 1ms reference) ----
 * Original QGimbal reference gains: Yaw Kp=5.0 Ki=0.1 Kd=110.0, Pitch Kp=4.6 Ki=0.17 Kd=30.0
 * Because StandardPid does not apply Ts internally, Ki and Kd are per-sample gains.
 * At 100Hz (10ms) vs 1kHz (1ms): Ki x10, Kd /10. Kp is instantaneous — unchanged.
 * Retune via VOFA channels as needed.
 * Output limits: +/- 1.8 A (motor current command)
 * Integral auto-limit: OutMax / Ki computed inside ComputePos()
 */
/* PID gains scaled for 10ms control period (QGimbal reference gains were tuned
 * at 1ms/1kHz. Both PID libraries do NOT apply Ts internally, so Ki accumulates
 * 10x slower and Kd sees 10x larger error differences at 100Hz. */
#define GIMBAL_YAW_KP    5.0f
#define GIMBAL_YAW_KI    1.0f    /* scaled x10 (ref: 0.1 @ 1ms) */
#define GIMBAL_YAW_KD    11.0f   /* scaled /10 (ref: 110.0 @ 1ms) */
#define GIMBAL_PITCH_KP  4.6f
#define GIMBAL_PITCH_KI  1.7f    /* scaled x10 (ref: 0.17 @ 1ms) */
#define GIMBAL_PITCH_KD  3.0f    /* scaled /10 (ref: 30.0 @ 1ms) */
#define GIMBAL_OUT_MIN   (-1.8f)
#define GIMBAL_OUT_MAX   (1.8f)
#define GIMBAL_IN_A      1.0f    /* no low-pass filter (pass-through) */

/* Pitch mechanical limit (radians) */
#define PITCH_LIMIT_RAD   0.5f

/* Control period (seconds) - matches TIMER_1 ISR period */
#define GIMBAL_DT         0.01f

#define PI_F              3.1415926f
#define TWO_PI_F          (2.0f * PI_F)

/*
 * PID gain lookup table indexed by motor_id.
 * Motors 0 (yaw) and 1 (pitch) use tuned gains; motors 2,3 default to yaw gains.
 */
static const float g_pid_kp[GIMBAL_MOTOR_COUNT] = { GIMBAL_YAW_KP,  GIMBAL_PITCH_KP,  GIMBAL_YAW_KP, GIMBAL_YAW_KP };
static const float g_pid_ki[GIMBAL_MOTOR_COUNT] = { GIMBAL_YAW_KI,  GIMBAL_PITCH_KI,  GIMBAL_YAW_KI, GIMBAL_YAW_KI };
static const float g_pid_kd[GIMBAL_MOTOR_COUNT] = { GIMBAL_YAW_KD,  GIMBAL_PITCH_KD,  GIMBAL_YAW_KD, GIMBAL_YAW_KD };

void Gimbal_Init(void)
{
    uint8_t i;

    /* Initialize CAN communication */
    QGimbal_CAN_Init();

    /* Initialize all motor PIDs */
    for (i = 0; i < GIMBAL_MOTOR_COUNT; i++) {
        InitPidStruct(&g_gimbal.motor[i].pid);
        SetPidStruct(&g_gimbal.motor[i].pid,
            g_pid_kp[i], g_pid_ki[i], g_pid_kd[i],
            GIMBAL_IN_A, GIMBAL_OUT_MIN, GIMBAL_OUT_MAX);
    }

    /* DEBUG: speed mode — verify CAN communication first, then re-enable stability */
    g_gimbal.stability_enabled  = 0;    /* 0 = speed mode, skip PID */
    g_gimbal.motors_enabled     = 0;
    g_gimbal.target_latched     = 0;
    g_gimbal.manual_angle_mode  = 0;

    for (i = 0; i < GIMBAL_MOTOR_COUNT; i++) {
        g_gimbal.motor[i].speed_target  = 0.0f;
        g_gimbal.motor[i].angle_target  = 0.0f;
        g_gimbal.motor[i].angle_manual  = 0.0f;
        g_gimbal.enable_retry[i]        = 0;
    }

    /* Motors are NOT auto-enabled here — enable is controlled via
     * VOFA commands (P28/P29-P32) or arm task start. */
}

/**
 * Angle error wrapping to [-PI, PI]
 */
static float wrap_error(float error)
{
    while (error > PI_F)  error -= TWO_PI_F;
    while (error < -PI_F) error += TWO_PI_F;
    return error;
}

/**
 * Wrap an angle to [0, 2*PI) — used for PID target accumulation so the target
 * stays in a bounded range (matches QGimbal reference).
 */
static float wrap_2pi(float angle)
{
    while (angle >= TWO_PI_F)  angle -= TWO_PI_F;
    while (angle < 0.0f)       angle += TWO_PI_F;
    return angle;
}

/**
 * Gimbal_Run - Main control loop, called from 10ms TIMER_1 ISR
 *
 * Three modes:
 *   1. Manual angle mode (manual_angle_mode = 1):
 *      - Loop all motors, re-send ENABLE if needed
 *      - Send ANGLE commands with stored manual targets
 *   2. Speed mode (stability_enabled = 0):
 *      - Motors 0,1: integrate speed into angle target (IMU-based)
 *      - Motors 2,3: send speed directly
 *   3. Stability mode (stability_enabled = 1):
 *      - Motors 0,1: PID position control using IMU angle feedback
 *      - Motors 2,3: send speed directly (no IMU stabilization for aux motors)
 */
void Gimbal_Run(void)
{
    float yaw_measured, pitch_measured;
    float target;
    uint8_t i;

    if (!g_gimbal.motors_enabled) {
        return;
    }

    /* ---- Manual angle mode: send stored angle targets directly ----
     * Skips speed/stability loops so VOFA angle commands are not
     * overwritten by automatic control. ENABLE retry per motor. */
    if (g_gimbal.manual_angle_mode) {
        for (i = 0; i < GIMBAL_MOTOR_COUNT; i++) {
            if (!g_motor_state[i].enabled && g_gimbal.enable_retry[i] < 200) {
                QGimbal_Enable(i);
                g_gimbal.enable_retry[i]++;
            }
            QGimbal_SetAngle(i, g_gimbal.motor[i].angle_manual);
        }
        return;
    }

    /* ENABLE retry for yaw motor (motor 0) — keep-alive in auto modes */
    if (!g_motor_state[0].enabled && g_gimbal.enable_retry[0] < 200) {
        QGimbal_Enable(0);
        g_gimbal.enable_retry[0]++;
    }

    /* Convert IMU angles from degrees to radians */
    yaw_measured   = imu.yaw   * (PI_F / 180.0f);
    pitch_measured = imu.pitch * (PI_F / 180.0f);

    /* ---- Power-on latch: sync target to current IMU angle so motor
     *     does not jump, then hold position with PID. ---- */
    if (!g_gimbal.target_latched) {
        g_gimbal.target_latched              = 1;
        g_gimbal.motor[0].angle_target       = yaw_measured;
        g_gimbal.motor[1].angle_target       = pitch_measured;
    }

    if (!g_gimbal.stability_enabled) {
        /* ---- Direct speed mode ----
         * Motors 0,1 (yaw/pitch): integrate speed into angle target for smooth
         * transition to stability mode. Pitch stops at mechanical limit.
         * Motors 2,3 (aux): send speed directly (no IMU integration). */

        /* Yaw (motor 0): rpm -> rad/tick: rpm * 2pi/60 * dt = rpm * pi/30 * dt */
        g_gimbal.motor[0].angle_target += g_gimbal.motor[0].speed_target
                                        * (PI_F / 30.0f) * GIMBAL_DT;
        g_gimbal.motor[0].angle_target  = wrap_2pi(g_gimbal.motor[0].angle_target);

        /* Pitch (motor 1): conditional integration — when the motor is already at
         * the mechanical limit, stop accumulating so the target does not drift. */
        if (!((g_gimbal.motor[1].speed_target > 0.0f
                    && g_motor_state[1].angle_rad >  PITCH_LIMIT_RAD) ||
              (g_gimbal.motor[1].speed_target < 0.0f
                    && g_motor_state[1].angle_rad < -PITCH_LIMIT_RAD)))
        {
            g_gimbal.motor[1].angle_target += g_gimbal.motor[1].speed_target
                                            * (PI_F / 30.0f) * GIMBAL_DT;
        }
        g_gimbal.motor[1].angle_target = wrap_2pi(g_gimbal.motor[1].angle_target);

        /* Send speed commands to all 4 motors */
        for (i = 0; i < GIMBAL_MOTOR_COUNT; i++) {
            QGimbal_SetSpeed(i, g_gimbal.motor[i].speed_target);
        }
    } else {
        /* ---- Stability mode (PID position control) ----
         * Motor 0 (yaw) and motor 1 (pitch) use IMU-based PID.
         * Motors 2,3 (aux) send speed directly. */

        /* Yaw (motor 0): update target from host speed, run PID */
        g_gimbal.motor[0].angle_target += g_gimbal.motor[0].speed_target
                                        * (PI_F / 30.0f) * GIMBAL_DT;
        target = wrap_2pi(g_gimbal.motor[0].angle_target);

        if (target - yaw_measured > PI_F) {
            ComputePos(&g_gimbal.motor[0].pid, target,
                       yaw_measured + TWO_PI_F);
        } else if (target - yaw_measured < -PI_F) {
            ComputePos(&g_gimbal.motor[0].pid, target,
                       yaw_measured - TWO_PI_F);
        } else {
            ComputePos(&g_gimbal.motor[0].pid, target, yaw_measured);
        }

        /* Pitch (motor 1): conditional target update based on motor feedback */
        if (!((g_gimbal.motor[1].speed_target > 0.0f
                    && g_motor_state[1].angle_rad >  PITCH_LIMIT_RAD) ||
              (g_gimbal.motor[1].speed_target < 0.0f
                    && g_motor_state[1].angle_rad < -PITCH_LIMIT_RAD)))
        {
            g_gimbal.motor[1].angle_target += g_gimbal.motor[1].speed_target
                                            * (PI_F / 30.0f) * GIMBAL_DT;
        }
        target = wrap_2pi(g_gimbal.motor[1].angle_target);

        if (target - pitch_measured > PI_F) {
            ComputePos(&g_gimbal.motor[1].pid, target,
                       pitch_measured + TWO_PI_F);
        } else if (target - pitch_measured < -PI_F) {
            ComputePos(&g_gimbal.motor[1].pid, target,
                       pitch_measured - TWO_PI_F);
        } else {
            ComputePos(&g_gimbal.motor[1].pid, target, pitch_measured);
        }

        /* Send current commands for yaw/pitch (PID output) */
        QGimbal_SetCurrent(0, g_gimbal.motor[0].pid.CurrentOut);
        QGimbal_SetCurrent(1, g_gimbal.motor[1].pid.CurrentOut);

        /* Motors 2,3: send speed directly (no IMU stabilization) */
        QGimbal_SetSpeed(2, g_gimbal.motor[2].speed_target);
        QGimbal_SetSpeed(3, g_gimbal.motor[3].speed_target);
    }
}
