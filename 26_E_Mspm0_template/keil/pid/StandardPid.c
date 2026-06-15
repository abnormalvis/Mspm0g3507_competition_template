#include "StandardPid.h"

// -------------------------- 左电机PID参数（独立配�????�???? --------------------------
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

// // -------------------------- 步进电机1 PID参数（独立配�????�???? --------------------------
// #define STEPPER_MOTOR1_PID_KP       -0.006f    // 比例系数
// #define STEPPER_MOTOR1_PID_KI       -0.000012f    // �????分系�????
// #define STEPPER_MOTOR1_PID_KD       -0.05f    // �????分系�????
// #define STEPPER_MOTOR1_PID_IN_A     1.0f    // 输入系数
// #define STEPPER_MOTOR1_PID_OUT_MIN  -3200.0f// 输出最小�?
// #define STEPPER_MOTOR1_PID_OUT_MAX   3200.0f// 输出最大�?

// // -------------------------- 步进电机2 PID参数（独立配�????�???? --------------------------
// #define STEPPER_MOTOR2_PID_KP       0.005f    // 比例系数
// #define STEPPER_MOTOR2_PID_KI       0.000012f    // �????分系�????
// #define STEPPER_MOTOR2_PID_KD       0.05f    // �????分系�????
// #define STEPPER_MOTOR2_PID_IN_A     1.0f    // 输入系数
// #define STEPPER_MOTOR2_PID_OUT_MIN  -3200.0f// 输出最小�?
// #define STEPPER_MOTOR2_PID_OUT_MAX   3200.0f// 输出最大�?

// -------------------------- 步进电机1 PID参数（独立配�????�???? --------------------------
#define STEPPER_MOTOR1_PID_KP -0.012f		// 比例系数
#define STEPPER_MOTOR1_PID_KI -0.00006f		// �????分系�????
#define STEPPER_MOTOR1_PID_KD -0.02f		// �????分系�????
#define STEPPER_MOTOR1_PID_IN_A 1.0f		// 输入系数
#define STEPPER_MOTOR1_PID_OUT_MIN -3200.0f // 输出最小�?
#define STEPPER_MOTOR1_PID_OUT_MAX 3200.0f	// 输出最大�?

// -------------------------- 步进电机2 PID参数（独立配�????�???? --------------------------
#define STEPPER_MOTOR2_PID_KP 0.008f		// 比例系数
#define STEPPER_MOTOR2_PID_KI 0.00003f		// �????分系�????
#define STEPPER_MOTOR2_PID_KD 0.015f		// �????分系�????
#define STEPPER_MOTOR2_PID_IN_A 1.0f		// 输入系数
#define STEPPER_MOTOR2_PID_OUT_MIN -3200.0f // 输出最小�?
#define STEPPER_MOTOR2_PID_OUT_MAX 3200.0f	// 输出最大�?

uint32_t TimeCount;
PidStruct MotorLSpeedPID;
PidStruct MotorRSpeedPID;
PidStruct StepperMotor1PID;
PidStruct StepperMotor2PID;
PidStruct yaw_pid;
PidStruct track_pid;

/**
 * �????    数：初�?�化PID结构体，将所有值赋0
 * �????    数：PidStruct* Handler：待初�?�化的PID结构体变�????
 * �???? �???? 值：�????
 */
void InitPidStruct(PidStruct *Handler)
{
	Handler->Kp = 0;
	Handler->Ki = 0;
	Handler->Kd = 0;
	Handler->in_a = 0;
	Handler->LastActual = 0;
	Handler->Error0 = 0;
	Handler->Error1 = 0;
	Handler->Error2 = 0;
	Handler->ErrorInt = 0;
	Handler->CurrentOut = 0;
	Handler->OutMin = 0;
	Handler->OutMax = 0;
}

/**
 * �????    数：赋值PID结构体，调节Kp、Ki、Kd、输出限幅�?
 * �????    数：PidStruct* Handler：待赋值的PID结构体变�????
 * �????    数：float kp,float ki,float kd,float min,float max
 * �???? �???? 值：�????
 */
void SetPidStruct(PidStruct *Handler, float kp, float ki, float kd, float in_a, float min, float max)
{
	Handler->Kp = kp;
	Handler->Ki = ki;
	Handler->Kd = kd;
	Handler->in_a = in_a;
	Handler->OutMin = min;
	Handler->OutMax = max;
}

/**
 * �????    数：计算位置式PID的输出值并赋�?
 * �????    数：PidStruct* Handler：待赋值的PID结构体变�????
 * �????    数：float Target：目标�?
 * �????    数：float Actual：实际值，由传感器读取
 * �???? �???? 值：�????
 */
void ComputePos(PidStruct *Handler, float Target, float Actual)
{
	float PosOut;
	Handler->Error1 = Handler->Error0;
	// 一阶低通滤�????
	Actual = Handler->in_a * Actual + (1 - Handler->in_a) * Handler->LastActual;
	Handler->Error0 = Target - Actual;
	Handler->ErrorInt += Handler->Error0;

	// Integral clamping: prevent integrator from saturating output alone
	if (Handler->Ki != 0.0f)
	{
		float i_limit = Handler->OutMax / Handler->Ki;
		if (i_limit < 0.0f)
			i_limit = -i_limit;
		if (Handler->ErrorInt > i_limit)
			Handler->ErrorInt = i_limit;
		if (Handler->ErrorInt < -i_limit)
			Handler->ErrorInt = -i_limit;
	}

	PosOut = Handler->Kp * Handler->Error0 + Handler->Ki * Handler->ErrorInt + Handler->Kd * (Handler->Error0 - Handler->Error1);

	// Dead-zone compensation: boost small output above motor stall threshold
// #define PID_DEADZONE 400.0f
// 	if (PosOut > 0.0f && PosOut < PID_DEADZONE)
// 		PosOut = PID_DEADZONE;
// 	if (PosOut < 0.0f && PosOut > -PID_DEADZONE)
// 		PosOut = -PID_DEADZONE;

	// Output clamping
	if (PosOut > Handler->OutMax)
		PosOut = Handler->OutMax;
	if (PosOut < Handler->OutMin)
		PosOut = Handler->OutMin;

	Handler->CurrentOut = PosOut;
	Handler->LastActual = Actual;
}

/**
 * �????    数：计算增量式PID的输出值并赋�?
 * �????    数：PidStruct* Handler：待赋值的PID结构体变�????
 * �????    数：float Target：目标�?
 * �????    数：float Actual：实际值，由传感器读取
 * �???? �???? 值：�????
 */
void ComputeInc(PidStruct *Handler, float Target, float Actual)
{
	float IncOut;
	Handler->Error2 = Handler->Error1;
	Handler->Error1 = Handler->Error0;
	// 一阶低通滤�????
	Actual = Handler->in_a * Actual + (1 - Handler->in_a) * Handler->LastActual;
	Handler->Error0 = Target - Actual;

	Handler->DeltaOut = Handler->Kp * (Handler->Error0 - Handler->Error1) + Handler->Ki * Handler->Error0 + Handler->Kd * (Handler->Error0 - 2 * Handler->Error1 + Handler->Error2);
	IncOut = Handler->CurrentOut + Handler->DeltaOut;

	// /* dead-zone compensation */
	// if (IncOut > 0.0f && IncOut < PID_DEADZONE)
	// 	IncOut = PID_DEADZONE;

	/* output clamping */
	if (IncOut > Handler->OutMax)
		IncOut = Handler->OutMax;
	if (IncOut < Handler->OutMin)
		IncOut = Handler->OutMin;

	Handler->CurrentOut = IncOut;
	Handler->LastActual = Actual;
}
// pid参数初�?�化
void SysPidInit()
{
	InitPidStruct(&MotorLSpeedPID);
	InitPidStruct(&MotorRSpeedPID);
	InitPidStruct(&StepperMotor1PID);
	InitPidStruct(&StepperMotor2PID);
	InitPidStruct(&yaw_pid);
	InitPidStruct(&track_pid);

	SetPidStruct(&MotorLSpeedPID,
				 MOTOR_L_SPEED_PID_KP, MOTOR_L_SPEED_PID_KI, MOTOR_L_SPEED_PID_KD,
				 MOTOR_L_SPEED_PID_IN_A, MOTOR_L_SPEED_PID_OUT_MIN, MOTOR_L_SPEED_PID_OUT_MAX);
	SetPidStruct(&MotorRSpeedPID,
				 MOTOR_R_SPEED_PID_KP, MOTOR_R_SPEED_PID_KI, MOTOR_R_SPEED_PID_KD,
				 MOTOR_R_SPEED_PID_IN_A, MOTOR_R_SPEED_PID_OUT_MIN, MOTOR_R_SPEED_PID_OUT_MAX);
	SetPidStruct(&StepperMotor1PID,
				 STEPPER_MOTOR1_PID_KP, STEPPER_MOTOR1_PID_KI, STEPPER_MOTOR1_PID_KD,
				 STEPPER_MOTOR1_PID_IN_A, STEPPER_MOTOR1_PID_OUT_MIN, STEPPER_MOTOR1_PID_OUT_MAX);
	SetPidStruct(&StepperMotor2PID,
				 STEPPER_MOTOR2_PID_KP, STEPPER_MOTOR2_PID_KI, STEPPER_MOTOR2_PID_KD,
				 STEPPER_MOTOR2_PID_IN_A, STEPPER_MOTOR2_PID_OUT_MIN, STEPPER_MOTOR2_PID_OUT_MAX);
}

void SystemControl()
{
}