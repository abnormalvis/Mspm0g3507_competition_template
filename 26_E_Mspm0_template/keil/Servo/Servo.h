#ifndef __Servo_H__
#define __Servo_H__

#include "ti_msp_dl_config.h"

/* ---- Servo angle limits (degrees) ---- */
#define SERVO1_SIZE 180
#define SERVO1_MAX_ANGLE 149
#define SERVO1_MIN_ANGLE 0

#define SERVO2_SIZE 180
#define SERVO2_MAX_ANGLE 180
#define SERVO2_MIN_ANGLE 0

/* ---- PWM channel indices (auto-detected from SysConfig "PWM_Servo") ---- */
#ifndef PWM_SERVO_C0_IDX
#define PWM_SERVO_C0_IDX  GPIO_PWM_Servo_C0_IDX
#endif
#ifndef PWM_SERVO_C1_IDX
#define PWM_SERVO_C1_IDX  GPIO_PWM_Servo_C1_IDX
#endif

void Servo_setAngle1(float Angle1);
void Servo_setAngle2(float Angle2);

#endif
