#ifndef __STANDARDPID_H__
#define __STANDARDPID_H__

#include "ti_msp_dl_config.h"
#include "Motor.h"
#include "Encoder.h"
#include "StepperMotor.h"
#include "vofa.h"
#include "math.h"
#include "Laser.h"
#include "Track.h"
#include "Solve.h"

typedef struct
{
	float Kp;
	float Ki;
	float Kd;

	float in_a;//输入一阶低通滤波系数

	float LastActual;

	float Error0;//本次误差
	float Error1;//上次误差
	
	float Error2;//上上次误差，用于增量式计算
	float ErrorInt;//误差积分，用于位置式计算
	
	float DeltaOut;//增量输出值
	float CurrentOut;//当前输出值
	float OutMin;//输出限幅
	float OutMax;
}PidStruct;//PID结构体变量

extern PidStruct MotorLSpeedPID;
extern PidStruct MotorRSpeedPID;
extern PidStruct StepperMotor1PID;
extern PidStruct StepperMotor2PID;
extern PidStruct yaw_pid;
extern PidStruct track_pid;

void InitPidStruct(PidStruct* Handler);
void SetPidStruct(PidStruct* Handler,float kp,float ki,float kd,float in_a,float min,float max);
void ComputePos(PidStruct* Handler,float Target,float Actual);
void ComputInc(PidStruct* Handler,float Target,float Actual);
void SysPidInit();
void SystemControl();

#endif
