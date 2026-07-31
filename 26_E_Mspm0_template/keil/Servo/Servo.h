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

void Servo_setAngle1(float Angle1);
void Servo_setAngle2(float Angle2);

/* ---- Lift servo (Servo 2, PA7, CC1) ---- */
void Servo_LiftRaise(void);   /* 500us pulse -> servo up */
void Servo_LiftLower(void);   /* 1854us pulse -> servo down */

#endif
