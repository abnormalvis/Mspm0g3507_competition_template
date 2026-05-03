#include "mt_test.h"
#include "hal_encode.h"
#include "hal_math.h"
#include "hal_tb6612.h"
#include "hal_jy62.h"
#include "hal_gray.h"
#include "hal_pid.h"
#include "hal_uart.h"
#include "mt_flag.h"
#include "user_interrupt.h"
#include "hal_led.h"
#include "2024DS_Duty.h"
#include "math.h"
#include "hal_beep.h"
#include "app.h"

static float speed_error[2]={0,0},speed_expect[2]={speed_expect_default,speed_expect_default},speed_feedback[2]={0,0};//速度误差 期望速度 反馈速度
static float speed_error_last[2]={0,0};
//static float position_kp = position_kp_default,position_ki = position_ki_default,position_kd = position_kd_default;
//蓝色车
//float speed_kp_l=0.5f,speed_ki_l=0.12f,speed_kd=speed_kd_default;
//float speed_kp_r=0.64f,speed_ki_r=0.15f;
float speed_kp_l=0.9f,speed_ki_l=0.2f,speed_kd=speed_kd_default;
float speed_kp_r=0.9f,speed_ki_r=0.2f;

float position_kp = 6.0;
float position_ki = 0;
float position_kd = 0.3;

float yaw_kp = 0.6;
float yaw_ki = 0;
float yaw_kd = 0.5;
	//黑车
//float speed_kp_l=1.1f,speed_ki_l=0.2f;
//float speed_kp_r=0.9f,speed_ki_r=0.23f;
//float speed_kp=speed_kp_default,speed_ki=speed_ki_default,speed_kd=speed_kd_default;
static float position_error[2]={0,0},position_feedback[2]={0,0},position_output[2]={0,0},yaw_out[2]={0,0};//位置误差 位置速度 反馈位置
static float yaw_feedback = 0,yaw_error = 0,yaw = 0,target_yaw = 0;//角度环 （和速度位置串级Pid一致）

float v_target_l=0,v_target_r=0;//单位cm/s 初始化速度环目标速度
float p_target_l=100,p_target_r=100;
float	speed_integral[2]={0,0},speed_output[2]={0,0};
float left_pwm,right_pwm;//左右电机最终的输出值

float turn_output=0,turn_output_last=0;//控制器输出值
controller seektrack_ctrl[2];		//自主寻迹控制器结构体,seektrack_ctrl[0]灰度管循迹;seektrack_ctrl[1]OpenMV循迹
float turn_ctrl_pwm=0;//转向控制输出
//float	turn_scale=turn_scale_default;//转向控制差速系数  0.15 转向控制器输出转换到轮子期望差速时的量程转换系数
float	turn_scale=0.06;//转向控制差速系数  0.15 转向控制器输出转换到轮子期望差速时的量程转换系数
float speed_setup = 70,speed_adjust = 35;//速度设定值  速度目标值必须这个形式才有用
float yaw_target=0;
/*速度环 位置换 串级 角度环*/
//void speed_control(void)
//{
//	speed_feedback[0]=smartcar_imu.left_motor_speed_cmps;//获取左轮实际值  
//	speed_error[0]=v_target_l-speed_feedback[0];
//	speed_error[0]=Xianfu_float(speed_error[0],speed_err_max);
//	speed_integral[0]+=speed_ki_l*speed_error[0];
//	speed_integral[0]=Xianfu_float(speed_integral[0],speed_integral_max);
//	speed_output[0]=speed_integral[0]+speed_kp_l*speed_error[0];
//	speed_output[0]=Xianfu_float(speed_output[0],speed_ctrl_output_max);
//		
//	speed_feedback[1]=smartcar_imu.right_motor_speed_cmps;//右轮
//	speed_error[1]=v_target_r-speed_feedback[1];//期望速度减去实际速度得到速度误差
//	
//	speed_error[1]=Xianfu_float(speed_error[1],speed_err_max);//对速度误差做约束
//	speed_integral[1]+=speed_ki_r*speed_error[1]; //速度积分
//	speed_integral[1]=Xianfu_float(speed_integral[1],speed_integral_max);//对得到的速度积分做约束
//	speed_output[1]=speed_integral[1]+speed_kp_r*speed_error[1];//pid得到输出
//	speed_output[1]=Xianfu_float(speed_output[1],speed_ctrl_output_max);//对输出做约束
//	
//}
float add_limit[2] ={}; 
float speed_theta = 0; 
void speed_control(void)
{

	
	speed_feedback[0]=smartcar_imu.left_motor_speed_cmps;//获取左轮实际值  
	speed_error[0]=v_target_l-speed_feedback[0];
	speed_error[0]=Xianfu_float(speed_error[0],speed_err_max);
	speed_integral[0]=speed_ki_l*speed_error[0];
//	speed_integral[0]=Xianfu_float(speed_integral[0],speed_integral_max);
	add_limit[0] = speed_integral[0]+speed_kp_l*(speed_error[0]- speed_error_last[0]) ;
	if(ABS(add_limit[0]) >0.5)
	speed_output[0]+=add_limit[0] ;
	speed_output[0]=Xianfu_float(speed_output[0],speed_ctrl_output_max);
	speed_error_last[0] = speed_error[0];
		
	speed_feedback[1]=smartcar_imu.right_motor_speed_cmps;//右轮
	speed_error[1]=v_target_r-speed_feedback[1];//期望速度减去实际速度得到速度误差
	speed_error[1]=Xianfu_float(speed_error[1],speed_err_max);//对速度误差做约束
	speed_integral[1]=speed_ki_r*speed_error[1]; //速度积分
//	speed_integral[1]=Xianfu_float(speed_integral[1],speed_integral_max);//对得到的速度积分做约束
	add_limit[1] = speed_integral[1]+speed_kp_l*(speed_error[1]- speed_error_last[1]) ;
	if(ABS(add_limit[1])  > 0.5)
	speed_output[1]+=add_limit[1];//pid得到输出
	speed_output[1]=Xianfu_float(speed_output[1],speed_ctrl_output_max);//对输出做约束
	speed_error_last[1] = speed_error[1];
}

void position_control(void)//脉冲速度位置串级PID 
{
	static float err_l,err_last_l,err_r,err_last_r,err_sum_l,err_sum_r;
	
	position_feedback[0] = distance_l;
	err_l = position_error[0] = p_target_l- position_feedback[0];
	
	if(ABS(err_l) < 0.8)
	{
		position_output[0] = 0;
	}
	else//位置环控制 PID 
	{	
		err_sum_l  += err_l;
		err_sum_l = Xianfu_float(err_sum_l,60); //积分限幅
		position_output[0] = position_kp*err_l + position_ki*err_sum_l + position_kd*(err_l-err_last_l);//位置环输出
		if(ABS(err_r)<4 && ABS(err_r)>0.8)
		{
			position_output[0] =  (position_output[0]/ABS(position_output[0]))*10 + position_output[0];
		}		
		err_last_l = err_l;
		
		position_output[0] = Xianfu_float(position_output[0],30);//50是目标速度 对位置输出进行限幅
	}
		v_target_l = position_output[0];

	
	position_feedback[1] = distance_r;
	err_r = position_error[1] = p_target_r- position_feedback[1];
	if(ABS(err_r) < 0.8)
	{
		position_output[1] = 0;
	}
	else
	{
		err_sum_r  += err_r;
		err_sum_r = Xianfu_float(err_sum_r,60); //积分限幅
		position_output[1] = position_kp*err_r + position_ki*err_sum_r + position_kd*(err_r - err_last_r);//位置环输出
		if(ABS(err_r)<4 && ABS(err_r)>0.8)
		{
			position_output[1] =  (position_output[1]/ABS(position_output[1]))*10 + position_output[1];
		}
		err_last_r = err_r;
		position_output[1] = Xianfu_float(position_output[1],30);//50是目标速度 对位置输出进行限幅
	}
		v_target_r = position_output[1];

}

//偏航角角度环  使用jy62
void Yaw_control(float target)
{
	static float err,err_last,err_sum;
	yaw_feedback  = angle.z;
  err = target - yaw_feedback;	
	if(err >= 180)
	{
		err = 360 - err; 
	}
	else if(err <= (-180))
	{
		err = -360 - err; 		
	}
	if(ABS(err)<2)
	{
		yaw_out[1] = 0;
		yaw_out[0] = 0;
	}
	else
	{
		err_sum += err;
		err_sum = Xianfu_float(err_sum,400);
		yaw_out[0] = yaw_kp*err + yaw_ki*err_sum + yaw_kd*(err - err_last);  
		if(ABS(err)<5 && ABS(err)>1)
		{
			yaw_out[0] = (yaw_out[0]/ABS(yaw_out[0]))*10 + yaw_out[0];
		}
		yaw_out[0] = -Xianfu_float(yaw_out[0],60);
		yaw_out[1] = -yaw_out[0];
	  err_last = err;		
	}
	
	v_target_l = yaw_out[0];
	v_target_r = yaw_out[1];
}


//灰度循迹
void ctrl_params_init(void)
{
	pid_control_init(&seektrack_ctrl[0],//待初始化控制器结构体，灰度管循迹 
										turn_kp_default1,//比例参数
										turn_ki_default1,//积分参数
										turn_kd_default1,//微分参数
										20, //偏差限幅值
										0,  //积分限幅值
										500,//控制器输出限幅值
										1,	//偏差限幅标志位
										0,0,//积分分离标志位与引入积分控制时的限幅值
										6); //微分间隔时间
	

//	pid_control_init(&seektrack_ctrl[1],//待初始化控制器结构体，OpenMV循迹
//										turn_kp_default2,//比例参数
//										turn_ki_default2,//积分参数
//										turn_kd_default2,//微分参数
//										20, //偏差限幅值
//										0,  //积分限幅值
//										500,//控制器输出限幅值
//										1,	//偏差限幅标志位
//										0,0,//积分分离标志位与引入积分控制时的限幅值
//										1); //微分间隔时间

}
//float turn_kp	=		20.0f ;	//基于红外对管识别轨迹时,自主寻迹的位置控制器比例参数KP	 位置控制器
//float turn_ki	=		0.0f	;	//基于红外对管识别轨迹时,自主寻迹的位置控制器积分参数KI	
//float turn_kd	=		110		;	//基于红外对管识别轨迹时,自主寻迹的位置控制器微分参数KD	
//20速度
float turn_kp	=	3.9f ;	//基于红外对管识别轨迹时,自主寻迹的位置控制器比例参数KP	 位置控制器
float turn_ki	=	0.0f;	//基于红外对管识别轨迹时,自主寻迹的位置控制器积分参数KI	
float turn_kd	=	5.2f;	//基于红外对管识别轨迹时,自主寻迹的位置控制器微分参数KD	

float turn_kp_L	=	2.f	;	//基于红外对管识别轨迹时,自主寻迹的位置控制器比例参数KP	 位置控制器
float turn_ki_L	=	0.0f		;	//基于红外对管识别轨迹时,自主寻迹的位置控制器积分参数KI	
float turn_kd_L	=	0.6f	;	//基于红外对管识别轨迹时,自主寻迹的位置控制器微分参数KD
//turn_ctrl_pwm = output  (-500,500)
float turn_theta = 0.5f;
void gray_turn_control_200hz(float *output)//200HZ=5ms
{
	float kp,kd;
	
	
	if(task_num < 4)
	{
			kp = turn_kp_L;
			kd = turn_kd_L;			

	}
	else
	{
			
			kp = turn_kp;
			kd = turn_kd;	
	}
		float a = 1 + turn_theta * ABS(gray_status)/15;
		kp *= a;

	
		pid_control_init(&seektrack_ctrl[0],//待初始化控制器结构体，灰度管循迹 
										kp,//比例参数
										0,//积分参数
										kd,//微分参数
										10, //偏差限幅值
										0,  //积分限幅值
										90,//控制器输出限幅值
										1,	//偏差限幅标志位
										0,0,//积分分离标志位与引入积分控制时的限幅值
										6); //微分间隔时间	
	//保存上次控制器输出  
	turn_output_last=turn_output;
	//转向PD控制输出，舵向控制实时性要求高，引入积分I会使舵向响应滞后
	seektrack_ctrl[0].expect=0;							//期望   seektrack是循迹的意思
	seektrack_ctrl[0].feedback=-gray_status;	//反馈 gray_status[0]是赛道元素状态值-11~11
	pid_control_run(&seektrack_ctrl[0]);		  //控制器运算
	turn_output=seektrack_ctrl[0].output;
	//叠加死区控制  有输出就加上死区
//	if(turn_output>0) turn_output+=steer_deadzone;//steer_deadzone转向死区 值为50
//	if(turn_output<0) turn_output-=steer_deadzone;
	//输出限幅
	turn_output=Xianfu_float(turn_output,70);//转向控制turn_ctrl_pwm输出限幅
	
	*output=0.75f*turn_output+0.25f*turn_output_last;//输出为前后两次计算的均值
}
float cam_turn_kp	=		0.f ;	//基于红外对管识别轨迹时,自主寻迹的位置控制器比例参数KP	 位置控制器
float cam_turn_ki	=		0.0f	;	//基于红外对管识别轨迹时,自主寻迹的位置控制器积分参数KI	
float cam_turn_kd	=		0.0f;	//基于红外对管识别轨迹时,自主寻迹的位置控制器微分参数KD	
void openmv_openmv_duty_run(float *output)
{
	pid_control_init(&seektrack_ctrl[1],//待初始化控制器结构体，灰度管循迹 
										cam_turn_kp,//比例参数
										cam_turn_ki,//积分参数
										cam_turn_kd,//微分参数
										10, //偏差限幅值
										0,  //积分限幅值
										50,//控制器输出限幅值
										1,	//偏差限幅标志位
										0,0,//积分分离标志位与引入积分控制时的限幅值
										6); //微分间隔时间
	
	//保存上次控制器输出  
	turn_output_last=turn_output;
	//转向PD控制输出，舵向控制实时性要求高，引入积分I会使舵向响应滞后
	seektrack_ctrl[1].expect=0;							//期望   seektrack是循迹的意思
	seektrack_ctrl[1].feedback=error_openmv;	//反馈 gray_status[0]是赛道元素状态值-11~11
	pid_control_run(&seektrack_ctrl[1]);		  //控制器运算
	turn_output=seektrack_ctrl[1].output;
	//叠加死区控制  有输出就加上死区
//	if(turn_output>0) turn_output+=steer_deadzone;//steer_deadzone转向死区 值为50
//	if(turn_output<0) turn_output-=steer_deadzone;
	//输出限幅
	turn_output=Xianfu_float(turn_output,50);//转向控制turn_ctrl_pwm输出限幅
	
	*output=0.75f*turn_output+0.25f*turn_output_last;//输出为前后两次计算的均值	
}
void openmv_duty_run(void)
{
	openmv_openmv_duty_run(&turn_ctrl_pwm);
	v_target_l = speed_setup+turn_ctrl_pwm;//*turn_scale;//左边轮子速度期望  两者相差两个turn_ctrl_pwm*turn_scale
	v_target_r = speed_setup-turn_ctrl_pwm;//*turn_scale;//右边轮子速度期望  所以turn_ctrl_pwm*turn_scale肯定与赛道元素有关
	speed_control();//这里面用到采集的实际脉冲数
}

void sdk_duty_run(float *a,float *b)
{
	float x1 = a[0],y1 = a[1], x2 = b[0],y2 = b[1];
	float detla_x = (x2-x1);
	float leng = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	
	//2022年7月份省赛小车跟随行驶系统赛道,内外圈交替循迹，
	gray_turn_control_200hz(&turn_ctrl_pwm);//基于灰度对管的转向控制，gray_turn_control_200hz函数（PID控制器运算）运算出来的值传入turn_ctrl_pwm
	
	if(task_num == 4) 
	{
		speed_setup = pos_auto_track(speed_adjust);
		//speed_setup = pos_auto_track(15);
	}
	else
		speed_setup = 30;
	
	//期望速度
	v_target_l = speed_setup+turn_ctrl_pwm;//*turn_scale;//左边轮子速度期望  两者相差两个turn_ctrl_pwm*turn_scale
	v_target_r = speed_setup-turn_ctrl_pwm;//*turn_scale;//右边轮子速度期望  所以turn_ctrl_pwm*turn_scale肯定与赛道元素有关

}


void nmotor_output(void)
{
	if(Flag.Start_Car == 1)
	{	
		if(speed_output[1])
			right_pwm= 4 * (speed_output[1] / ABS(speed_output[1])) + speed_output[1]; //两轮速度单独控制
		else
			right_pwm = 0;
		
		if(speed_output[0])
			left_pwm= 4 * (speed_output[0] / ABS(speed_output[0])) + speed_output[0]; //两轮速度单独控制	
		else
			left_pwm = 0;	
		
		left_pwm =Xianfu_float(left_pwm,motor_max_default);
		right_pwm=Xianfu_float(right_pwm,motor_max_default);
	
		if(right_pwm>=0)
		{	
			Motor_Foreward_Right(99-right_pwm);
		}
		else
		{
			Motor_Backward_Right(99 - ABS(right_pwm)); 
		}
		
		if(left_pwm>=0)
		{
			Motor_Foreward_Left(99 -left_pwm);
		}
		else
		{
			Motor_Backward_Left(99 - ABS(left_pwm));
		}		
	}
	else
	{

		AIN1_OUT(0);
		AIN2_OUT(0);
						 
		BIN1_OUT(0);
		BIN2_OUT(0);		
	}		

}
#define distance 25
uint16_t gray_cnt = 0;

float tar_theta_limit = 15.0f,sleep_time = 30.f;
static uint8_t Start_duty3_3_cnt = 0,duty3_8_cnt = 0;
float yaw_angle = 0;
void TIMG0_IRQHandler(void)	//10ms
{
	
	if(Flag.Start_duty_1)	//(1) A -> B 15s  位置角度闭环100cm 
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty_1 = 0;		
		//auto_track(point_actual,point_B);//自动循迹
		if(gray_state.state)//L < distance||
		{
				gray_cnt++;
				if(gray_cnt>5&&L < distance)
				{
					v_target_l = 0;
					v_target_r = 0;							
					gray_cnt = 0;
					Flag.beep_on = 1;	
				//	Flag.Start_Car  = 0;	
					Flag.Start_duty_1 = 0;
					Flag.Success_duty_1 = 1;
				}
				else
				{
						v_target_l = 15;
						v_target_r = 15;						
				}	
		}
	}/*以下是第二问*/
	else if(Flag.Start_duty2_1)	//(2) A -> B 30s
	{	
	
		Flag.Start_Car  = 1;
		Flag.Success_duty2_1 = 0;
		auto_track(point_actual,point_B);
		if(gray_state.state)//L < distance||
		{
				gray_cnt++;
				if(gray_cnt>5&&L < distance)
				{
					gray_cnt = 0;			
					Start_duty3_3_cnt = Num2;
					Flag.beep_on = 1;	
				//	Flag.Start_Car  = 0;	
					Flag.Start_duty2_1 = 0;
					Flag.Success_duty2_1 = 1;	
				}					
				else
				{
						v_target_l = 15;
						v_target_r = 15;						
				}	
		}		
		
	}
	else if(Flag.Start_duty2_7 == 1)	//（2） 停止
	{
		//Flag.Start_Car  = 1;	
		Flag.Success_duty2_7 = 0;		
		v_target_l = 0;
		v_target_r = 0;		
		duty3_8_cnt++;
		if(duty3_8_cnt > sleep_time)//识别到黑线 先停住200ms 
		{
			duty3_8_cnt = 0;
			Flag.Start_duty2_7 = 0;
			Flag.Success_duty2_7 = 1;		
		}
	}		
	else if(Flag.Start_duty2_2 == 1)	//(2) B -> C 30s 循迹
	{
		//speed_setup = 70; 
		Flag.Start_Car  = 1;
		Flag.Success_duty2_2 = 0;
		sdk_duty_run(point_actual,point_A);//得到速度目标值
		if(((Num2 - Start_duty3_3_cnt) > 1 &&  Flag.gray_worse==1)&&sqrt((point_C[1] - point_actual[1]) * (point_C[1] - point_actual[1]) + (point_C[0] - point_actual[0]) * (point_C[0] - point_actual[0]))<distance)//||sqrt((point_C[1] - point_actual[1]) * (point_C[1] - point_actual[1]) + (point_C[0] - point_actual[0]) * (point_C[0] - point_actual[0]))<distance
		{
			Flag.beep_on = 1;

			//Flag.Start_Car  = 0;
			Flag.Start_duty2_2 = 0;
			Flag.Success_duty2_2 = 1;
		}

	}
	else if(Flag.Start_duty2_3) //(2) 在C点校准角度
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty2_3 = 0;
		//Yaw_control(-180);
		v_target_l = 5;
		v_target_r = -5;
		if(angle.z < -178 || angle.z > 179 )
		{

			Flag.Start_duty2_3 = 0;
			Flag.Success_duty2_3 = 1;
		//	Flag.Start_Car  = 0;						
		}
	}
	else if(Flag.Start_duty2_4) //(2)  C -> D 30s 
	{
		Flag.Start_Car  = 1;
		
		Flag.Success_duty2_4 = 0;

		auto_track(point_actual,point_D);
		if(gray_state.state)//L < distance||
		{
				gray_cnt++;
				if(gray_cnt>5&&L < distance)
				{
					gray_cnt = 0;			
					Start_duty3_3_cnt = Num2;
					Flag.beep_on = 1;	

					//Flag.Start_Car  = 0;	
					v_target_l = 0;
					v_target_r = 0;					
					Flag.Start_duty2_4 = 0;
					Flag.Success_duty2_4 = 1;
				}
				else
				{
						v_target_l = 15;
						v_target_r = 15;						
				}	
		}		
	}	
	else if(Flag.Start_duty2_6 == 1)	//（2） 停止
	{
		//Flag.Start_Car  = 1;	
		Flag.Success_duty2_6 = 0;		
		v_target_l = 0;
		v_target_r = 0;		
		duty3_8_cnt++;
		if(duty3_8_cnt > sleep_time)//识别到黑线 先停住200ms 
		{
			duty3_8_cnt = 0;
			Flag.Start_duty2_6 = 0;
			Flag.Success_duty2_6 = 1;		
		}
	}	
	else if(Flag.Start_duty2_5)	//（2） D -> A 30s
	{
		//speed_setup = 70; 
		Flag.Start_Car  = 1;
		Flag.Success_duty2_5 = 0;
		sdk_duty_run(point_actual,point_A);//得到速度目标值
		if(((Num2 - Start_duty3_3_cnt) > 1 &&  Flag.gray_worse==1)&&sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance)//||sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance
		{
			v_target_l = 0;
			v_target_r = 0;			
			Flag.gray_worse = 0;	
			Flag.beep_on = 1;

			//Flag.Start_Car  = 0;
			Flag.Start_duty2_5 = 0;
			Flag.Success_duty2_5 = 1;
		}		
	}/*以下是第三问*/
	else if(Flag.Start_duty3_0 == 1)	//(3)   Send3_Step = -1
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty3_0 = 0;		
		v_target_l = 40;
		v_target_r = -30;


		if(ABS(angle.z) > 30)
		{
			Start_duty3_3_cnt = Num2;
			gray_cnt = 0;
			//Flag.Start_Car  = 0;
			Flag.Start_duty3_0 = 0;
			Flag.Success_duty3_0 = 1;			
		}			
	}	
	else if(Flag.Start_duty3_1 == 1)	//（3） A -> C 40s  Send3_Step = 0
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty3_1 = 0;		
		auto_track(point_actual,point_C);
//		if(L < distance || ABS(gray_status) <= 10)
	//	if(L < distance ||Flag.gray_worse == 0)
		if(gray_state.state && ((Num2 - Start_duty3_3_cnt) > 1))//识别到黑线 先停住
		{
			gray_cnt++;
			if(L < distance&&gray_cnt>2)
			{
							gray_cnt = 0;
				Flag.yaw_loss_ahead = 0;
	//			v_target_l = 0;
	//			v_target_r = 0;			
					Flag.yaw_loss = 0;
				Flag.beep_on = 1;		
				Start_duty3_3_cnt = Num2;
				//Flag.Start_Car  = 0;	
				Flag.Start_duty3_1 = 0;
				Flag.Success_duty3_1 = 1;		
			}
		}
			else if(L < 5.f)
			{
				if(Flag.yaw_loss == 0)	
				{
					Flag.yaw_loss = 1;
					yaw_angle = angle.z;
				}
				if((yaw_angle - angle.z) < 10&&(yaw_angle - angle.z) >0)
				{
					Flag.yaw_loss_ahead = 1;
				}
				if(Flag.yaw_loss_ahead)
				{
					v_target_l = 10;
					v_target_r = 15;					
				}
				else
				{
							v_target_l = -10;
							v_target_r = 10;
				}
			}
	}
	else if(Flag.Start_duty3_5 == 1)	//(3)C点循迹前  Send3_Step = 4
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty3_5 = 0;		
		v_target_l = -10;
		v_target_r = 15;
//		Flag.beep_on = 1;
		if(gray_state.state)//(gray_state.state <8 && gray_state.state!= 0)
		{
						gray_cnt = 0;

//			Flag.beep_on = 1;	
			Start_duty3_3_cnt = Num2;
			v_target_l = 0;
			v_target_r = 0;
			//Flag.Start_Car  = 0;
			Flag.Start_duty3_5 = 0;
			Flag.Success_duty3_5 = 1;			
		}	
	}
//到达C点先停住 500ms
	else if(Flag.Start_duty3_8 == 1)	//（3） A -> C 40s  Send3_Step = 7
	{
		Flag.Success_duty3_8 = 0;		
		v_target_l = 0;
		v_target_r = 0;
		duty3_8_cnt++;
		if(duty3_8_cnt > sleep_time)//识别到黑线 先停住200ms 
		{
						gray_cnt = 0;

			duty3_8_cnt = 0;
			Flag.Start_duty3_8 = 0;
			Flag.Success_duty3_8 = 1;		
		}
	}			
	else if(Flag.Start_duty3_2 == 1)	//(3) C -> B 40s Send3_Step = 1
	{
		//speed_setup = 70; 
		Flag.Start_Car  = 1;
		Flag.Success_duty3_2 = 0;
		sdk_duty_run(point_actual,point_B);//得到速度目标值
//		if( distance_inter >= 105 + distance_point)
		if(((Num2 - Start_duty3_3_cnt) > 1 &&  Flag.gray_worse==1)&&(sqrt((point_B[1] - point_actual[1]) * (point_B[1] - point_actual[1]) + (point_B[0] - point_actual[0]) * (point_B[0] - point_actual[0]))<distance))//||sqrt((point_B[1] - point_actual[1]) * (point_B[1] - point_actual[1]) + (point_B[0] - point_actual[0]) * (point_B[0] - point_actual[0]))<distance
		{
			Flag.gray_worse = 0;	
			Flag.beep_on = 1;
			gray_cnt = 0;

			//Flag.Start_Car  = 0;
			Flag.Start_duty3_2 = 0;
			Flag.Success_duty3_2 = 1;
			Start_duty3_3_cnt = Num2;
		}
//		else if(L < 7.5)
//			{
//				v_target_l = 5;
//				v_target_r = -5;
//			}
	}
	else if(Flag.Start_duty3_7 == 1)	//(3)智能循迹前 调整车位 Send3_Step = 6
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty3_7 = 0;		
		v_target_l = -30;
		v_target_r = 40;

		if(ABS(angle.z) < 150)
		{
			Start_duty3_3_cnt = Num2;
			gray_cnt = 0;

			//Flag.Start_Car  = 0;
			Flag.Start_duty3_7 = 0;
			Flag.Success_duty3_7 = 1;		
		}	

	}	
	else if(Flag.Start_duty3_3 == 1)	//(3) B -> D 40s   Send3_Step = 8
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty3_3 = 0;		
		auto_track(point_actual,point_D);
		
		if((Num2 - Start_duty3_3_cnt) > 1 && gray_state.state)	//到D || L <distance(gray_state.state <10 && gray_state.state!= 0)) 
		{
			gray_cnt++;
			if(L < distance&&gray_cnt>2)
			{
				Flag.beep_on = 1;	
			gray_cnt = 0;
					Flag.yaw_loss = 0;


				Flag.Start_duty3_3 = 0;
				Flag.Success_duty3_3 = 1;			
				Start_duty3_3_cnt = Num2;
			}
		}		
			else if(L < 5.f)
			{
				if(Flag.yaw_loss == 0)	
				{
					Flag.yaw_loss = 1;
					yaw_angle = angle.z;
				}
			float err = (yaw_angle - angle.z);
				if(err >180)
				{
					err -= 360;
				}
				if(err<0 && err>-10)
				{
					Flag.yaw_loss_ahead = 1;
				}
				if(Flag.yaw_loss_ahead)
				{
					v_target_l = 15;
					v_target_r = 10;					
				}
				else
				{
							v_target_l = 10;
							v_target_r = -10;
				}
			}
	}	
	else if(Flag.Start_duty3_6 == 1)	//(3)D点循迹前 Send3_Step = 2
	{
		Flag.Start_Car  = 1;
		Flag.Success_duty3_6 = 0;		
		v_target_l = 15;
		v_target_r = -10;
		if(gray_state.state)//gray_state.state <8 && gray_state.state!= 0
		{
						gray_cnt = 0;

			v_target_l = 0;
			v_target_r = 0;			
			Start_duty3_3_cnt = Num2;
//			Flag.beep_on = 1;	
			//Flag.Start_Car  = 0;
			Flag.Start_duty3_6 = 0;
			Flag.Success_duty3_6 = 1;			
		}
	}
	else if(Flag.Start_duty3_9 == 1)	//（3） A -> C 40s  Send3_Step = 5
	{
		Flag.Success_duty3_9 = 0;		
		v_target_l = 0;
		v_target_r = 0;		
		duty3_8_cnt++;
		if(duty3_8_cnt > sleep_time)//识别到黑线 先停住200ms 
		{			gray_cnt = 0;

			duty3_8_cnt = 0;
			Flag.Start_duty3_9 = 0;
			Flag.Success_duty3_9 = 1;		
		}
	}		

	else if(Flag.Start_duty3_4 == 1)	//(3) D -> A 40s Send3_Step = 3
	{
		//speed_setup = 70; 
		Flag.Start_Car  = 1;
		Flag.Success_duty3_4 = 0;
		sdk_duty_run(point_actual,point_A);//得到速度目标值
//		if( distance_inter >= 105 + distance_point)
		if(((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1))// &&sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance
		{			gray_cnt = 0;

			Flag.gray_worse = 0;
			Flag.beep_on = 1;
			v_target_l = 0;
			v_target_r = 0;	
			//Flag.Start_Car  = 0;
			Flag.Start_duty3_4 = 0;
			Flag.Success_duty3_4 = 1;
		}
	
	}	

		get_wheel_speed(); 	//获取速度
		speed_control();     //速度环
		nmotor_output();	//电机输出
	
	
	  /*速度环*/
//	get_wheel_speed();
//	speed_control();
//	nmotor_output();
	
		/*速度位置串级*/
//	get_wheel_speed();
//	position_control();  //得到速度目标值
//	speed_control();     //速度环
//	nmotor_output();	
	
		/*角度环*/
//	Yaw_control(yaw_target);
//	get_wheel_speed();
//	speed_control();
//	nmotor_output();	
	
	/*灰度巡线小车*/
//	  get_wheel_speed(); 
//		sdk_duty_run();
//		nmotor_output();	

	/*摄像头循迹小车*/
//	  get_wheel_speed(); 
//		//sdk_duty_run();
//		openmv_duty_run();
//		nmotor_output();
}

