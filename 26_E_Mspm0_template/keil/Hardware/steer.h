#ifndef __STEER_H__
#define __STEER_H__

#include "headfile.h"
/******************************************************************
		ARR = 1000
   配置占空比 范围 0 ~ (per-1)
   t = 0.5ms-------------------舵机会转动 0 °--25
   t = 1.0ms-------------------舵机会转动 45°--50
   t = 1.5ms-------------------舵机会转动 90°--75
   t = 2.0ms-------------------舵机会转动 135°--100
   t = 2.5ms-------------------舵机会转动180°--125
******************************************************************/
// 计算PWM占空比
// 0.5ms对应的计数 = 25
// 2.5ms对应的计数 = 125
#define MIN_COUNT  25
#define MAX_COUNT  125
void Set_X_Servo_Angle(float angle);
void Set_Y_Servo_Angle(float angle);
float Get_X_Servo_Angle(void);
float Get_Y_Servo_Angle(void);
#endif
