#ifndef __GIMBAL_CONTROL_H
#define __GIMBAL_CONTROL_H

#include "StandardPid.h"

#define GIMBAL_MOTOR_COUNT 4

/* Per-motor control state */
typedef struct {
    PidStruct pid;
    float speed_target;      /* rpm from host */
    float angle_target;      /* accumulated from speed input (rad) */
    float angle_manual;      /* direct angle via VOFA (rad, 0~2pi) */
} GimbalMotor;

typedef struct {
    GimbalMotor motor[GIMBAL_MOTOR_COUNT];
    uint8_t stability_enabled;   /* 1 = PID angle control, 0 = direct speed */
    uint8_t motors_enabled;
    uint8_t target_latched;      /* 1 = angle target synced to current IMU position */
    uint8_t manual_angle_mode;   /* 1 = manual ANGLE mode (skip auto speed/stability) */
    uint16_t enable_retry[GIMBAL_MOTOR_COUNT];  /* per-motor ENABLE retry counter */
} GimbalController;

extern GimbalController g_gimbal;

void Gimbal_Init(void);
void Gimbal_Run(void);           /* called from 10ms ISR */

#endif /* __GIMBAL_CONTROL_H */
