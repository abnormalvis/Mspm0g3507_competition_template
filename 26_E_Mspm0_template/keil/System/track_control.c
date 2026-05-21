#include "track_control.h"
float target_yaw;

//循迹环PID执行函数
void Pid_Track_Control()
{
	uint8_t left_dir;
	uint8_t right_dir;
	/********************循迹环***********************/
	// 设置目标偏差
	track_pid.target = 0;
	// 设置当前偏差：为负：右转；为正：左转
	if(track_depart_level > 1)
	{
		left_dir = 0;
		right_dir = 1;
		track_pid.now = -track_depart_level;
		Pid_Cal(&track_pid);
	}
	else if(track_depart_level < -1)
	{
		left_dir = 1;
		right_dir = 0;
		track_pid.now = track_depart_level;
		Pid_Cal(&track_pid);
	}
	//偏差值小于一定程度就加速
	else if(Float_Abs(track_depart_level) <= 1)
	{
		left_dir = 1;
		right_dir = 1;
		track_pid.out = 15;
	}
	/********************速度环***********************/
	//设置目标速度：上方角度环已设置，下方注释代码为调试代码
	if(track_pid.out >= 0)
	{
		motorL.target = track_pid.out;
		motorR.target = track_pid.out;
	}
	else
	{
		motorL.target = -track_pid.out;
		motorR.target = -track_pid.out;
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