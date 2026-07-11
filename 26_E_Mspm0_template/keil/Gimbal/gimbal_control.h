#ifndef __GIMBAL_CONTROL_H
#define __GIMBAL_CONTROL_H

#include "StandardPid.h"

typedef struct {
    PidStruct yaw_pid;
    PidStruct pitch_pid;
    float yaw_speed_target;      /* rpm from host */
    float pitch_speed_target;    /* rpm from host */
    float yaw_angle_target;      /* accumulated from speed input (rad) */
    float pitch_angle_target;    /* accumulated from speed input (rad) */
    uint8_t stability_enabled;   /* 1 = PID angle control, 0 = direct speed */
    uint8_t motors_enabled;
} GimbalController;

extern GimbalController g_gimbal;

void Gimbal_Init(void);
void Gimbal_Run(void);           /* called from 10ms ISR */

#endif /* __GIMBAL_CONTROL_H */
