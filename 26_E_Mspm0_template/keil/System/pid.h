#ifndef __PID_H__
#define __PID_H__

#include "headfile.h"
#define MAX_DUTY 100
#define MIX_DUTY 0
#define MAX_Speed 30
#define MIX_Speed -30


enum
{
  POSITION_PID = 0,  // 位置式
  DELTA_PID,         // 增量式
};
typedef struct
{
	float target;	
	float now;
	float error[3];		
	float p,i,d;
	float pout, dout, iout;
	float out;   
	
	uint32_t pid_mode;

}pid_t;
//转向环PID结构体
extern pid_t turn_pid;
//循迹环PID结构体
extern pid_t track_pid;
//速度环PID结构体
extern pid_t motorL;
extern pid_t motorR;


float Float_Abs(float value);
float Yaw_Error_Cal(float Target, float Now);
void Pid_Turn_Cal(pid_t *pid);
void Pid_Cal(pid_t *pid);
void Pid_Init(pid_t *pid, uint32_t mode, float p, float i, float d);
#endif
