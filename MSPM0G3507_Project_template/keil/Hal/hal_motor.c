#include "hal_motor.h"
#include "hal_encode.h"
#include "hal_tb6612.h"
#include "hal_pid.h"

void Pid_Motor_Control(void)
{
    uint8_t left_dir, right_dir;
    float target_speed = motorL.target;

    if (target_speed >= 0) {
        left_dir = 1; right_dir = 1;
        motorL.target = target_speed;
        motorR.target = target_speed;
    } else {
        left_dir = 1; right_dir = 1;
        motorL.target = -target_speed;
        motorR.target = -target_speed;
    }

    motorL.now = (float)(Get_Encoder_Count(&motor_left_encoder) >= 0
        ? Get_Encoder_Count(&motor_left_encoder)
        : -Get_Encoder_Count(&motor_left_encoder));
    motorR.now = (float)(Get_Encoder_Count(&motor_right_encoder) >= 0
        ? Get_Encoder_Count(&motor_right_encoder)
        : -Get_Encoder_Count(&motor_right_encoder));

    Pid_Cal(&motorL);
    Pid_Cal(&motorR);

    Set_MotorL_Speed(left_dir, (uint32_t)motorL.out);
    Set_MotorR_Speed(right_dir, (uint32_t)motorR.out);
}
