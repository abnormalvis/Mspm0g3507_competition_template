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
    float yaw_target, pitch_target;
    float yaw_measured, pitch_measured;

    if (!g_gimbal.motors_enabled) {
        return;
    }

    /* Convert IMU angles from degrees to radians */
    yaw_measured   = imu.yaw   * (PI_F / 180.0f);
    pitch_measured = imu.pitch * (PI_F / 180.0f);

    if (!g_gimbal.stability_enabled) {
        /* ---- Direct speed mode ----
         * Integrate speed into angle target for smooth mode transition.
         * Yaw always integrates; pitch stops at mechanical limit (uses motor
         * feedback angle from CAN, matching QGimbal reference). */

        /* Yaw: rpm -> rad/tick: rpm * 2pi/60 * dt = rpm * pi/30 * dt */
        g_gimbal.yaw_angle_target += g_gimbal.yaw_speed_target
                                     * (PI_F / 30.0f) * GIMBAL_DT;
        g_gimbal.yaw_angle_target  = wrap_2pi(g_gimbal.yaw_angle_target);

        /* Pitch: conditional integration — when the motor is already at the
         * mechanical limit, stop accumulating so the target does not drift. */
        if (!((g_gimbal.pitch_speed_target > 0.0f
                    && g_gimbal_pitch.angle_rad >  PITCH_LIMIT_RAD) ||
              (g_gimbal.pitch_speed_target < 0.0f
                    && g_gimbal_pitch.angle_rad < -PITCH_LIMIT_RAD)))
        {
            g_gimbal.pitch_angle_target += g_gimbal.pitch_speed_target
                                           * (PI_F / 30.0f) * GIMBAL_DT;
        }
        g_gimbal.pitch_angle_target = wrap_2pi(g_gimbal.pitch_angle_target);

        QGimbal_SetSpeed(QGIMBAL_MOTOR_YAW,   g_gimbal.yaw_speed_target);
        QGimbal_SetSpeed(QGIMBAL_MOTOR_PITCH, g_gimbal.pitch_speed_target);
    } else {
        /* ---- Stability mode (PID position control) ----
         * Matches QGimbal reference Ctrl_ISR: update PID target from host
         * speed, unroll the IMU measurement when the shortest angular path
         * crosses the 0/2*pi wrap-around, feed current to motors. */

        /* Update yaw target from host speed, wrap to [0, 2pi) */
        g_gimbal.yaw_angle_target += g_gimbal.yaw_speed_target
                                     * (PI_F / 30.0f) * GIMBAL_DT;
        yaw_target = wrap_2pi(g_gimbal.yaw_angle_target);

        /* Yaw PID: unroll measurement so the PID always sees the shortest
         * angular path. If target - measurement > +pi, feed measurement+2pi
         * so the error stays in [-pi,0]; if < -pi, feed measurement-2pi so
         * the error stays in [0,+pi]. */
        if (yaw_target - yaw_measured > PI_F) {
            ComputePos(&g_gimbal.yaw_pid, yaw_target,
                       yaw_measured + TWO_PI_F);
        } else if (yaw_target - yaw_measured < -PI_F) {
            ComputePos(&g_gimbal.yaw_pid, yaw_target,
                       yaw_measured - TWO_PI_F);
        } else {
            ComputePos(&g_gimbal.yaw_pid, yaw_target, yaw_measured);
        }

        /* Pitch: conditional target update based on motor feedback angle */
        if (!((g_gimbal.pitch_speed_target > 0.0f
                    && g_gimbal_pitch.angle_rad >  PITCH_LIMIT_RAD) ||
              (g_gimbal.pitch_speed_target < 0.0f
                    && g_gimbal_pitch.angle_rad < -PITCH_LIMIT_RAD)))
        {
            g_gimbal.pitch_angle_target += g_gimbal.pitch_speed_target
                                           * (PI_F / 30.0f) * GIMBAL_DT;
        }
        pitch_target = wrap_2pi(g_gimbal.pitch_angle_target);

        /* Pitch PID: same measurement unrolling as yaw */
        if (pitch_target - pitch_measured > PI_F) {
            ComputePos(&g_gimbal.pitch_pid, pitch_target,
                       pitch_measured + TWO_PI_F);
        } else if (pitch_target - pitch_measured < -PI_F) {
            ComputePos(&g_gimbal.pitch_pid, pitch_target,
                       pitch_measured - TWO_PI_F);
        } else {
            ComputePos(&g_gimbal.pitch_pid, pitch_target, pitch_measured);
        }

        /* Send current commands (PID output = motor current in A) */
        QGimbal_SetCurrent(QGIMBAL_MOTOR_YAW,   g_gimbal.yaw_pid.CurrentOut);
        QGimbal_SetCurrent(QGIMBAL_MOTOR_PITCH, g_gimbal.pitch_pid.CurrentOut);
    }
}
