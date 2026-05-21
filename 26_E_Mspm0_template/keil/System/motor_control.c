#include "motor_control.h"
float target_speed;

void Pid_Motor_Control()
{
	uint8_t left_dir;
	uint8_t right_dir;
	/********************速度环***********************/
	//设置目标速度：上方角度环已设置，下方注释代码为调试代码
	if(target_speed >= 0)
	{
		left_dir = 1;
		right_dir = 1;
		motorL.target = target_speed;
		motorR.target = target_speed;
	}
	else
	{
		left_dir = 1;
		right_dir = 1;
		motorL.target = -target_speed;
		motorR.target = -target_speed;
	}
	//获取当前速度(但为确保传入参数均为正数，这里根据方向取数据绝对值)
	motorL.now = abs(Get_Encoder_Count(&motor_left_encoder));
	motorR.now = abs(Get_Encoder_Count(&motor_right_encoder));
	//经过PID控制
	Pid_Cal(&motorL);
	Pid_Cal(&motorR);
	//输出电机占空比
	Set_MotorL_Speed(left_dir, motorL.out);
	Set_MotorR_Speed(right_dir, motorR.out);
}