#include "pid.h"
//转向环PID结构体
pid_t turn_pid;
// 循迹环PID结构体
pid_t track_pid;
// 速度环PID结构体
pid_t motorL;
pid_t motorR;


// 角度环角度取绝对值
float Float_Abs(float value)
{
	if (value < 0)
	{
		return -value;
	}
	else
	{
		return value;
	}
}
// 转向环误差计算函数
// 右转时误差输出应为正，左转时误差输出应为负，-180到0到180之间任意两个角度计算都为距离最近的值：避免在-180到180之间抽搐
float Yaw_Error_Cal(float Target, float Now)
{
	static float error;
	if (Target >= 0)
	{
		if (Now < 0)
		{
			// 左转：输出为负
			if (Float_Abs(Now) < (180 - Target))
			{
				error = -(Float_Abs(Now) + Target);
			}
			// 右转：输出为正
			else
			{
				error = (180 - Target) + (180 - Float_Abs(Now));
			}
		}
		// 同时满足左转为负右转为正
		else
		{
			error = Now - Target;
		}
	}
	else
	{
		if (Now >= 0)
		{
			// 左转：输出为负
			if (Now >= (Target + 180))
			{
				error = -((180 - Now) + (180 - Float_Abs(Target)));
			}
			// 右转：输出为正
			else
			{
				error = (Float_Abs(Target) + Now);
			}
		}
		// 同时满足左转为负右转为正
		else
		{
			error = Float_Abs(Target) - Float_Abs(Now);
		}
	}
	return error;
}
//转向环PID控制
void Pid_Turn_Cal(pid_t *pid)
{
	// 计算当前偏差
	pid->error[0] = Yaw_Error_Cal(pid->target, pid->now);

	// 计算输出
	if(pid->pid_mode == DELTA_PID)  // 增量式
	{
		pid->pout = pid->p * (pid->error[0] - pid->error[1]);
		pid->iout = pid->i * pid->error[0];
		pid->dout = pid->d * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
		pid->out += pid->pout + pid->iout + pid->dout;
	}
	else if(pid->pid_mode == POSITION_PID)  // 位置式
	{
		pid->pout = pid->p * pid->error[0];
		pid->iout += pid->i * pid->error[0];
		
		// D项滤波，减少噪声影响
		float d_error = pid->error[0] - pid->error[1];
		if(Float_Abs(d_error) < 0.5)  // 如果误差变化很小，忽略D项
		{
			pid->dout = 0;
		}
		else
		{
			pid->dout = pid->d * d_error;
		}
		
		pid->out = pid->pout + pid->iout + pid->dout;
	}

	// 记录前两次偏差
	pid->error[2] = pid->error[1];
	pid->error[1] = pid->error[0];

	// 输出限幅
	if(pid->out>=MAX_Speed)	
		pid->out=MAX_Speed;
	if(pid->out<=MIX_Speed)	
		pid->out=MIX_Speed;
	
}
void Pid_Cal(pid_t *pid)
{
	
	// 计算当前偏差
	pid->error[0] = pid->target - pid->now;

	// 计算输出
	if (pid->pid_mode == DELTA_PID) // 增量式
	{
		pid->pout = pid->p * (pid->error[0] - pid->error[1]);
		pid->iout = pid->i * pid->error[0];
		pid->dout = pid->d * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
		pid->out += pid->pout + pid->iout + pid->dout;
	}
	else if (pid->pid_mode == POSITION_PID) // 位置式
	{
		pid->pout = pid->p * pid->error[0];
		pid->iout += pid->i * pid->error[0];
		pid->dout = pid->d * (pid->error[0] - pid->error[1]);
		pid->out = pid->pout + pid->iout + pid->dout;
	}

	// 记录前两次偏差
	pid->error[2] = pid->error[1];
	pid->error[1] = pid->error[0];

	// 输出限幅
	if (pid->out >= MAX_DUTY)
		pid->out = MAX_DUTY;
	if (pid->out <= MIX_DUTY)
		pid->out = MIX_DUTY;
}
// PID初始参数设定
void Pid_Init(pid_t *pid, uint32_t mode, float p, float i, float d)
{
	pid->pid_mode = mode;
	pid->p = p;
	pid->i = i;
	pid->d = d;
}
