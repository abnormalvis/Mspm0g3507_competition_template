#include "StandardPid.h"

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
 * �????? �????? 值：�?????
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
 * �?????    数：赋值PID结构体，调节Kp、Ki、Kd、输出限幅�?
 * �?????    数：PidStruct* Handler：待赋值的PID结构体变�?????
 * �?????    数：float kp,float ki,float kd,float min,float max
 * �????? �????? 值：�?????
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
 * �?????    数：计算位置式PID的输出值并赋�?
 * �?????    数：PidStruct* Handler：待赋值的PID结构体变�?????
 * �?????    数：float Target：目标�?
 * �?????    数：float Actual：实际值，由传感器读取
 * �????? �????? 值：�?????
 */
void ComputePos(PidStruct *Handler, float Target, float Actual)
{
	float PosOut;
	Handler->Error1 = Handler->Error0;
	// 一阶低通滤�?????
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
 * �?????    数：计算增量式PID的输出值并赋�?
 * �?????    数：PidStruct* Handler：待赋值的PID结构体变�?????
 * �?????    数：float Target：目标�?
 * �?????    数：float Actual：实际值，由传感器读取
 * �????? �????? 值：�?????
 */
void ComputeInc(PidStruct *Handler, float Target, float Actual)
{
	float IncOut;
	Handler->Error2 = Handler->Error1;
	Handler->Error1 = Handler->Error0;
	// 一阶低通滤�?????
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
/*
 * Yaw angle PID - position-form with shortest-angular-distance error wrapping.
 *
 * Handles the -180/+180 wrap-around so the PID sees the geometrically
 * shortest error (e.g. from +170 to -170 gives -20, not +340).
 *
 * Features:
 *   - shortest-angular-distance error wrapping
 *   - +/-2 degree dead band (resets integral to prevent windup)
 *   - D-term dead-zone: |derror| < 0.5 -> dout = 0 (gyro noise suppression)
 *   - integral anti-windup via OutMax / Ki clamp
 *   - output clamped to [OutMin, OutMax]
 *
 * Reference: keil/System/pid.c Pid_Turn_Cal()
 */
void ComputeYaw(PidStruct *pid, float Target, float Actual)
{
	/* 1. Yaw error with shortest-angular-distance wrapping */
	float err = Target - Actual;
	if (err > 180.0f)  err -= 360.0f;
	if (err < -180.0f) err += 360.0f;

	/* 2. Dead band: +/-2 degrees */
	if (err < 2.0f && err > -2.0f)
	{
		err = 0.0f;
		pid->ErrorInt = 0.0f;
	}

	/* 3. Shift error history */
	pid->Error1 = pid->Error0;
	pid->Error0 = err;

	/* 4. Proportional term */
	float pout = pid->Kp * err;

	/* 5. Integral term (accumulate + anti-windup) */
	pid->ErrorInt += err;
	if (pid->Ki != 0.0f)
	{
		float i_limit = pid->OutMax / pid->Ki;
		if (i_limit < 0.0f) i_limit = -i_limit;
		if (pid->ErrorInt > i_limit)  pid->ErrorInt = i_limit;
		if (pid->ErrorInt < -i_limit) pid->ErrorInt = -i_limit;
	}
	float iout = pid->Ki * pid->ErrorInt;

	/* 6. Derivative term with dead-zone */
	float derr = pid->Error0 - pid->Error1;
	float dout;
	if (derr < 0.5f && derr > -0.5f)
		dout = 0.0f;
	else
		dout = pid->Kd * derr;

	/* 7. Sum and clamp */
	float output = pout + iout + dout;
	if (output > pid->OutMax)  output = pid->OutMax;
	if (output < pid->OutMin)  output = pid->OutMin;

	pid->CurrentOut = output;
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
	SetPidStruct(&yaw_pid,
				 YAW_PID_KP, YAW_PID_KI, YAW_PID_KD,
				 YAW_PID_IN_A, YAW_PID_OUT_MIN, YAW_PID_OUT_MAX);
	SetPidStruct(&track_pid,
				 TRACK_PID_KP, TRACK_PID_KI, TRACK_PID_KD,
				 TRACK_PID_IN_A, TRACK_PID_OUT_MIN, TRACK_PID_OUT_MAX);
}

void SystemControl()
{
}
