#ifndef __Servo_H__
#define __Servo_H__

#include "ti_msp_dl_config.h"

#define SERVO1_SIZE 180				//舵机类型
#define SERVO1_MAX_ANGLE 149		//舵机限幅
#define SERVO1_MIN_ANGLE 0			//

#define SERVO2_SIZE 180
#define SERVO2_MAX_ANGLE 180
#define SERVO2_MIN_ANGLE 0

void Servo_setAngle1(float Angle1);
void Servo_setAngle2(float Angle2);

#endif
