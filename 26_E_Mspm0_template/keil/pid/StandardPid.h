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

	float in_a;//输入一阶低通滤波系�?

	float LastActual;

	float Error0;//�?次�??�?
	float Error1;//上�?��??�?
	
	float Error2;//上上次�??�?，用于�?�量式�?�算
	float ErrorInt;//�?�?�?分，用于位置式�?�算
	
	float DeltaOut;//增量输出�?
	float CurrentOut;//当前输出�?
	float OutMin;//输出限幅
	float OutMax;
}PidStruct;//PID结构体变�?

extern PidStruct MotorLSpeedPID;
extern PidStruct MotorRSpeedPID;
extern PidStruct StepperMotor1PID;
extern PidStruct StepperMotor2PID;
extern PidStruct yaw_pid;
extern PidStruct track_pid;

void InitPidStruct(PidStruct* Handler);
void SetPidStruct(PidStruct* Handler,float kp,float ki,float kd,float in_a,float min,float max);
void ComputePos(PidStruct* Handler,float Target,float Actual);
void ComputeInc(PidStruct* Handler,float Target,float Actual);
void SysPidInit();
void SystemControl();

#endif
