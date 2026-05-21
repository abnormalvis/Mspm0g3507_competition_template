#include "turn_control.h"


//转向环PID执行函数
void Pid_Turn_Control()
{
	uint8_t left_dir;
	uint8_t right_dir;
	/********************转向环***********************/
	//设置转向目标值turn_pid.target = target_yaw;
	//已在定时器开启转向后设置，目标角度不需要连续变化
	//设置转向当前值
	turn_pid.now = Yaw;
	Pid_Turn_Cal(&turn_pid);
	/********************速度环***********************/
	//设置目标速度:左转为负，右转为正
	if(turn_pid.out > 0)
	{
		left_dir = 1;
		right_dir = 0;
		motorL.target = turn_pid.out;
		motorR.target = turn_pid.out;
	}
	else
	{
		left_dir = 0;
		right_dir = 1;
		motorL.target = -turn_pid.out;
		motorR.target = -turn_pid.out;
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

