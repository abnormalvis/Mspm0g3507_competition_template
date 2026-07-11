#include "gimbal_control.h"
#include "hal_qgimbal_can.h"
#include "imu_filter.h"

/* ---- global controller ---- */
GimbalController g_gimbal;

/* ---- PID parameters (from QGimbal reference, tuned for 1kHz) ----
 * At 100Hz the integral/derivative scaling differs because StandardPid
 * does not apply Ts internally. Start with the original gains and
 * retune via VOFA channels as needed.
 *   Yaw:   Kp=5.0,  Ki=0.1,  Kd=110.0
 *   Pitch: Kp=4.6,  Ki=0.17, Kd=30.0
 * Output limits: +/- 1.8 A (motor current command)
 * Integral auto-limit: OutMax / Ki computed inside ComputePos()
 */
#define GIMBAL_YAW_KP    5.0f
#define GIMBAL_YAW_KI    0.1f
#define GIMBAL_YAW_KD    110.0f
#define GIMBAL_PITCH_KP  4.6f
#define GIMBAL_PITCH_KI  0.17f
#define GIMBAL_PITCH_KD  30.0f
#define GIMBAL_OUT_MIN   (-1.8f)
#define GIMBAL_OUT_MAX   (1.8f)
#define GIMBAL_IN_A      1.0f    /* no low-pass filter (pass-through) */

/* Pitch mechanical limit (radians) */
#define PITCH_LIMIT_RAD   0.5f

/* Control period (seconds) - matches TIMER_1 ISR period */
#define GIMBAL_DT         0.01f

#define PI_F              3.1415926f

void Gimbal_Init(void)
{
    /* Initialize CAN communication */
    QGimbal_CAN_Init();

    /* Initialize yaw PID */
    InitPidStruct(&g_gimbal.yaw_pid);
    SetPidStruct(&g_gimbal.yaw_pid,
        GIMBAL_YAW_KP, GIMBAL_YAW_KI, GIMBAL_YAW_KD,
        GIMBAL_IN_A, GIMBAL_OUT_MIN, GIMBAL_OUT_MAX);

    /* Initialize pitch PID */
    InitPidStruct(&g_gimbal.pitch_pid);
    SetPidStruct(&g_gimbal.pitch_pid,
        GIMBAL_PITCH_KP, GIMBAL_PITCH_KI, GIMBAL_PITCH_KD,
        GIMBAL_IN_A, GIMBAL_OUT_MIN, GIMBAL_OUT_MAX);

    /* Default: speed mode (stability disabled), motors off */
    g_gimbal.stability_enabled  = 0;
    g_gimbal.motors_enabled     = 0;
    g_gimbal.yaw_speed_target   = 0.0f;
    g_gimbal.pitch_speed_target = 0.0f;
    g_gimbal.yaw_angle_target   = 0.0f;
    g_gimbal.pitch_angle_target = 0.0f;

    /* Enable both motors */
    QGimbal_Enable(QGIMBAL_MOTOR_YAW);
    QGimbal_Enable(QGIMBAL_MOTOR_PITCH);
    g_gimbal.motors_enabled = 1;
}

/**
 * Angle error wrapping to [-PI, PI]
 */
static float wrap_error(float error)
{
    while (error > PI_F)  error -= 2.0f * PI_F;
    while (error < -PI_F) error += 2.0f * PI_F;
    return error;
}

/**
 * Gimbal_Run - Main control loop, called from 10ms TIMER_1 ISR
 *
 * Two modes:
 *   1. Speed mode (stability_enabled = 0):
 *      - Send speed commands directly to motors
 *      - Integrate speed target into angle target (for smooth transition)
 *   2. Stability mode (stability_enabled = 1):
 *      - PID position control using IMU angle feedback
 *      - Output = motor current command
 */
void Gimbal_Run(void)
{
    float yaw_error, pitch_error;
    float yaw_measured, pitch_measured;

    if (!g_gimbal.motors_enabled) {
        return;
    }

    /* Convert IMU angles from degrees to radians */
    yaw_measured   = imu.yaw   * (PI_F / 180.0f);
    pitch_measured = imu.pitch * (PI_F / 180.0f);

    if (!g_gimbal.stability_enabled) {
        /* ---- Direct speed mode ----
         * Integrate speed into angle target for smooth mode transition */
        g_gimbal.yaw_angle_target   += g_gimbal.yaw_speed_target
                                       * (PI_F / 30.0f) * GIMBAL_DT;
        /* rpm -> rad/s: rpm * 2pi/60 = rpm * pi/30 */
        g_gimbal.pitch_angle_target += g_gimbal.pitch_speed_target
                                       * (PI_F / 30.0f) * GIMBAL_DT;

        QGimbal_SetSpeed(QGIMBAL_MOTOR_YAW,   g_gimbal.yaw_speed_target);
        QGimbal_SetSpeed(QGIMBAL_MOTOR_PITCH, g_gimbal.pitch_speed_target);
    } else {
        /* ---- Stability mode (PID position control) ---- */

        /* Yaw: wrap error to shortest angular distance */
        yaw_error = wrap_error(g_gimbal.yaw_angle_target - yaw_measured);
        ComputePos(&g_gimbal.yaw_pid, 0.0f, -yaw_error);
        /* Note: ComputePos(target=0, actual=-error) == target - actual = 0 - (-error) = error.
         * We negate error so that positive error -> positive PID output. */

        /* Pitch: with mechanical limit */
        pitch_error = g_gimbal.pitch_angle_target - pitch_measured;
        pitch_error = wrap_error(pitch_error);

        /* Clip pitch to +/- PITCH_LIMIT_RAD */
        if (pitch_error > PITCH_LIMIT_RAD)  pitch_error = PITCH_LIMIT_RAD;
        if (pitch_error < -PITCH_LIMIT_RAD) pitch_error = -PITCH_LIMIT_RAD;

        ComputePos(&g_gimbal.pitch_pid, 0.0f, -pitch_error);

        /* Send current commands */
        QGimbal_SetCurrent(QGIMBAL_MOTOR_YAW,   g_gimbal.yaw_pid.CurrentOut);
        QGimbal_SetCurrent(QGIMBAL_MOTOR_PITCH, g_gimbal.pitch_pid.CurrentOut);
    }
}
