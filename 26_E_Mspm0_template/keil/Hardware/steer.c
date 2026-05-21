#include "steer.h"

float now_x_angle = 0;//当前舵机角度
float now_y_angle = 0;//当前舵机角度

void Set_X_Servo_Angle(float angle)
{
    if(angle > 180)
    {
        angle = 180; // 限制角度在0到180度之间
    }

    now_x_angle = angle;


    float range = MAX_COUNT - MIN_COUNT;
    float ServoAngle = MIN_COUNT + ((angle / 180.0f) * range);

    DL_TimerA_setCaptureCompareValue(STEER_INST, (unsigned int)(ServoAngle + 0.5f), GPIO_STEER_C0_IDX);
}

void Set_Y_Servo_Angle(float angle)
{
    if(angle > 180)
    {
        angle = 180; // 限制角度在0到180度之间
    }

    now_y_angle = angle;

    float range = MAX_COUNT - MIN_COUNT;
    float ServoAngle = MIN_COUNT + ((angle / 180.0f) * range);

    DL_TimerA_setCaptureCompareValue(STEER_INST, (unsigned int)(ServoAngle + 0.5f), GPIO_STEER_C1_IDX);
}

float Get_X_Servo_Angle(void)
{
    return now_x_angle;
}


float Get_Y_Servo_Angle(void)
{
    return now_y_angle;
}