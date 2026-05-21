#include "StandardPid.h"

// -------------------------- 左电机PID参数（独立配置） --------------------------
#define MOTOR_L_SPEED_PID_KP     60.0f    
#define MOTOR_L_SPEED_PID_KI     0.3f   
#define MOTOR_L_SPEED_PID_KD     0.0f    
#define MOTOR_L_SPEED_PID_IN_A     0.5f    
#define MOTOR_L_SPEED_PID_OUT_MIN -3200.0f  
#define MOTOR_L_SPEED_PID_OUT_MAX  3200.0f  

// -------------------------- 右电机PID参数（独立配置） --------------------------
#define MOTOR_R_SPEED_PID_KP     -60.0f    // 修正：宏名改为R（右电机）
#define MOTOR_R_SPEED_PID_KI     -0.3f   //0.08
#define MOTOR_R_SPEED_PID_KD     0.0f    
#define MOTOR_R_SPEED_PID_IN_A     0.5f    
#define MOTOR_R_SPEED_PID_OUT_MIN -3200.0f  
#define MOTOR_R_SPEED_PID_OUT_MAX  3200.0f  

// // -------------------------- 步进电机1 PID参数（独立配置） --------------------------
// #define STEPPER_MOTOR1_PID_KP       -0.006f    // 比例系数
// #define STEPPER_MOTOR1_PID_KI       -0.000012f    // 积分系数
// #define STEPPER_MOTOR1_PID_KD       -0.05f    // 微分系数
// #define STEPPER_MOTOR1_PID_IN_A     1.0f    // 输入系数
// #define STEPPER_MOTOR1_PID_OUT_MIN  -3200.0f// 输出最小值
// #define STEPPER_MOTOR1_PID_OUT_MAX   3200.0f// 输出最大值

// // -------------------------- 步进电机2 PID参数（独立配置） --------------------------
// #define STEPPER_MOTOR2_PID_KP       0.005f    // 比例系数
// #define STEPPER_MOTOR2_PID_KI       0.000012f    // 积分系数
// #define STEPPER_MOTOR2_PID_KD       0.05f    // 微分系数
// #define STEPPER_MOTOR2_PID_IN_A     1.0f    // 输入系数
// #define STEPPER_MOTOR2_PID_OUT_MIN  -3200.0f// 输出最小值
// #define STEPPER_MOTOR2_PID_OUT_MAX   3200.0f// 输出最大值

// -------------------------- 步进电机1 PID参数（独立配置） --------------------------
#define STEPPER_MOTOR1_PID_KP       -0.012f    // 比例系数
#define STEPPER_MOTOR1_PID_KI       -0.00006f    // 积分系数
#define STEPPER_MOTOR1_PID_KD       -0.02f    // 微分系数
#define STEPPER_MOTOR1_PID_IN_A     1.0f    // 输入系数
#define STEPPER_MOTOR1_PID_OUT_MIN  -3200.0f// 输出最小值
#define STEPPER_MOTOR1_PID_OUT_MAX   3200.0f// 输出最大值

// -------------------------- 步进电机2 PID参数（独立配置） --------------------------
#define STEPPER_MOTOR2_PID_KP       0.008f    // 比例系数
#define STEPPER_MOTOR2_PID_KI       0.00003f    // 积分系数
#define STEPPER_MOTOR2_PID_KD       0.015f    // 微分系数
#define STEPPER_MOTOR2_PID_IN_A     1.0f    // 输入系数
#define STEPPER_MOTOR2_PID_OUT_MIN  -3200.0f// 输出最小值
#define STEPPER_MOTOR2_PID_OUT_MAX   3200.0f// 输出最大值

uint32_t TimeCount;
PidStruct MotorLSpeedPID;
PidStruct MotorRSpeedPID;
PidStruct StepperMotor1PID;
PidStruct StepperMotor2PID;
PidStruct yaw_pid;
PidStruct track_pid;

/**
  * 函    数：初始化PID结构体，将所有值赋0
  * 参    数：PidStruct* Handler：待初始化的PID结构体变量
  * 返 回 值：无
  */
void InitPidStruct(PidStruct* Handler)
{
	Handler->Kp=0;
	Handler->Ki=0;
	Handler->Kd=0;
	Handler->in_a=0;
	Handler->LastActual=0;
	Handler->Error0=0;
	Handler->Error1=0;
	Handler->Error2=0;
	Handler->ErrorInt=0;
	Handler->CurrentOut=0;
	Handler->OutMin=0;
	Handler->OutMax=0;
}

/**
  * 函    数：赋值PID结构体，调节Kp、Ki、Kd、输出限幅值
  * 参    数：PidStruct* Handler：待赋值的PID结构体变量
  * 参    数：float kp,float ki,float kd,float min,float max  
  * 返 回 值：无
  */
void SetPidStruct(PidStruct* Handler,float kp,float ki,float kd,float in_a,float min,float max)
{
	Handler->Kp=kp;
	Handler->Ki=ki;
	Handler->Kd=kd;
	Handler->in_a=in_a;
	Handler->OutMin=min;
	Handler->OutMax=max;
}

/**
  * 函    数：计算位置式PID的输出值并赋值
  * 参    数：PidStruct* Handler：待赋值的PID结构体变量
  * 参    数：float Target：目标值
  * 参    数：float Actual：实际值，由传感器读取
  * 返 回 值：无
  */
void ComputePos(PidStruct* Handler,float Target,float Actual)
{
	float PosOut;
	Handler->Error1 = Handler->Error0;
	//一阶低通滤波
	Actual = Handler->in_a * Actual + (1 - Handler->in_a) * Handler->LastActual;
	Handler->Error0=Target-Actual;
	Handler->ErrorInt+=Handler->Error0;
	
	PosOut=Handler->Kp * Handler->Error0 + Handler->Ki * Handler->ErrorInt
			+ Handler->Kd * (Handler->Error0 - Handler->Error1);
			
	if(PosOut>Handler->OutMax)
		PosOut=Handler->OutMax;
	if(PosOut<Handler->OutMin)
		PosOut=Handler->OutMin;
	
	Handler->CurrentOut = PosOut;
	Handler->LastActual = Actual;
}

/**
  * 函    数：计算增量式PID的输出值并赋值
  * 参    数：PidStruct* Handler：待赋值的PID结构体变量
  * 参    数：float Target：目标值
  * 参    数：float Actual：实际值，由传感器读取
  * 返 回 值：无
  */
void ComputInc(PidStruct* Handler,float Target,float Actual)
{
	float IncOut;
	Handler->Error2 = Handler->Error1;
	Handler->Error1 = Handler->Error0;
	//一阶低通滤波
	Actual = Handler->in_a * Actual + (1 - Handler->in_a) * Handler->LastActual;
	Handler->Error0 = Target - Actual;
	
	Handler->DeltaOut = Handler->Kp * (Handler->Error0 - Handler->Error1) + Handler->Ki * Handler->Error0
					+ Handler->Kd * (Handler->Error0 - 2 * Handler->Error1 + Handler->Error2);
	IncOut = Handler->CurrentOut + Handler->DeltaOut;
	
	if(IncOut>Handler->OutMax)
		IncOut=Handler->OutMax;
	if(IncOut<Handler->OutMin)
		IncOut=Handler->OutMin;
	
	Handler->CurrentOut = IncOut;
	Handler->LastActual = Actual;
}
//pid参数初始化
void SysPidInit(){
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



void SystemControl(){

}