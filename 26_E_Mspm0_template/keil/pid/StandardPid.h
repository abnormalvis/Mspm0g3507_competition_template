#ifndef __STANDARDPID_H__
#define __STANDARDPID_H__

#include "ti_msp_dl_config.h"
#include "hal_gray.h"
#include "Motor.h"
#include "Encoder.h"
#include "StepperMotor.h"
#include "vofa.h"
#include "math.h"
#include "Laser.h"
#include "Track.h"
#include "Solve.h"

// -------------------------- 左电机PID参数（独立配�??????�?????? --------------------------
#define MOTOR_L_SPEED_PID_KP 0.015f
#define MOTOR_L_SPEED_PID_KI 0.635f
#define MOTOR_L_SPEED_PID_KD 0.01f
#define MOTOR_L_SPEED_PID_IN_A 0.5f
#define MOTOR_L_SPEED_PID_OUT_MIN 0.0f
#define MOTOR_L_SPEED_PID_OUT_MAX 5000.0f

// -------------------------- 右电机PID参数（独立配�� --------------------------
#define MOTOR_R_SPEED_PID_KP 0.015f
#define MOTOR_R_SPEED_PID_KI 0.635f
#define MOTOR_R_SPEED_PID_KD 0.01f
#define MOTOR_R_SPEED_PID_IN_A 0.5f
#define MOTOR_R_SPEED_PID_OUT_MIN 0.0f
#define MOTOR_R_SPEED_PID_OUT_MAX 5000.0f

// // -------------------------- 步进电机1 PID参数（独立配�??????�?????? --------------------------
// #define STEPPER_MOTOR1_PID_KP       -0.006f    // 比例系数
// #define STEPPER_MOTOR1_PID_KI       -0.000012f    // �??????分系�??????
// #define STEPPER_MOTOR1_PID_KD       -0.05f    // �??????分系�??????
// #define STEPPER_MOTOR1_PID_IN_A     1.0f    // 输入系数
// #define STEPPER_MOTOR1_PID_OUT_MIN  -3200.0f// 输出最小�?
// #define STEPPER_MOTOR1_PID_OUT_MAX   3200.0f// 输出最大�?

// // -------------------------- 步进电机2 PID参数（独立配�??????�?????? --------------------------
// #define STEPPER_MOTOR2_PID_KP       0.005f    // 比例系数
// #define STEPPER_MOTOR2_PID_KI       0.000012f    // �??????分系�??????
// #define STEPPER_MOTOR2_PID_KD       0.05f    // �??????分系�??????
// #define STEPPER_MOTOR2_PID_IN_A     1.0f    // 输入系数
// #define STEPPER_MOTOR2_PID_OUT_MIN  -3200.0f// 输出最小�?
// #define STEPPER_MOTOR2_PID_OUT_MAX   3200.0f// 输出最大�?

// -------------------------- 步进电机1 PID参数（独立配�??????�?????? --------------------------
#define STEPPER_MOTOR1_PID_KP -0.012f		// 比例系数
#define STEPPER_MOTOR1_PID_KI -0.00006f		// �??????分系�??????
#define STEPPER_MOTOR1_PID_KD -0.02f		// �??????分系�??????
#define STEPPER_MOTOR1_PID_IN_A 1.0f		// 输入系数
#define STEPPER_MOTOR1_PID_OUT_MIN -3200.0f // 输出最小�?
#define STEPPER_MOTOR1_PID_OUT_MAX 3200.0f	// 输出最大�?

// -------------------------- 步进电机2 PID参数（独立配�??????�?????? --------------------------
#define STEPPER_MOTOR2_PID_KP 0.008f		// 比例系数
#define STEPPER_MOTOR2_PID_KI 0.00003f		// �??????分系�??????
#define STEPPER_MOTOR2_PID_KD 0.015f		// �??????分系�??????
#define STEPPER_MOTOR2_PID_IN_A 1.0f		// 输入系数
#define STEPPER_MOTOR2_PID_OUT_MIN -3200.0f // 输出最小�?
#define STEPPER_MOTOR2_PID_OUT_MAX 3200.0f	// 输出最大�?

// -------------------------- �ǶȻ�PID���� --------------------------
#define YAW_PID_KP 2.0f
#define YAW_PID_KI 0.0f
#define YAW_PID_KD 0.05f
#define YAW_PID_IN_A 0.5f
#define YAW_PID_OUT_MIN -1000.0f
#define YAW_PID_OUT_MAX 1000.0f

// -------------------------- ѭ����PID���� --------------------------
#ifdef GRAY_SENSOR_16CH
#define TRACK_PID_KP 0.3f
#elif defined(GRAY_SENSOR_12CH)
#define TRACK_PID_KP 10.0f
#else
#define TRACK_PID_KP 15.0f
#endif
#define TRACK_PID_KI 0.0f
#define TRACK_PID_KD 120.0f
#define TRACK_PID_IN_A 0.5f
#define TRACK_PID_OUT_MIN -1000.0f
#define TRACK_PID_OUT_MAX 1000.0f

typedef struct
{
	float Kp;
	float Ki;
	float Kd;

	float in_a;//输入一阶低通滤波系�???

	float LastActual;

	float Error0;//�???次�??�???
	float Error1;//上�?��??�???
	
	float Error2;//上上次�??�???，用于�?�量式�?�算
	float ErrorInt;//�???�???�???分，用于位置式�?�算
	
	float DeltaOut;//增量输出�???
	float CurrentOut;//当前输出�???
	float OutMin;//输出限幅
	float OutMax;
}PidStruct;//PID结构体变�???

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
void ComputeYaw(PidStruct* Handler,float Target,float Actual);
void SysPidInit();
void SystemControl();

#endif
