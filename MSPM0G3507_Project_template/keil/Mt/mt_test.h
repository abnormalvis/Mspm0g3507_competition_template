#ifndef _MT_TEST_H
#define _MT_TEST_H

#include "stdint.h"

#define speed_expect_default 			0.0f			//自主寻迹时，速度默认设定值

#define speed_err_max  80.0f				//速度偏差限幅值
#define speed_integral_max  60.0f	//积分限幅值
#define speed_ctrl_output_max 70.0f  	//控制器限幅值
#define  motor_max_default (85-1)


#define speed_kp_default  				0.0f//8.0f	26.0f		//速度控制器比例参数KP  速度环pi控制的参数  50.0f
#define	speed_ki_default					0.0f//0.6f	 0.85f		//速度控制器积分参数KI  50.0f
#define	speed_kd_default					0.0f			//速度控制器微分参数KD

//位置PID控制参数
//#define position_kp_default       4.5f
//#define position_ki_default       0.05f
//#define position_kd_default       0.001f

//角度PD控制参数（调好）
//#define yaw_kp                    0.4f//30.0f
//#define yaw_ki                    0.0001f//0.5
//#define yaw_kd                    0.8f//2.0f


#define steer_deadzone 50//50  转向控制死区 
#define turn_scale_default        	0.06		//转向控制器输出转换到轮子期望差速时的量程转换系数 0.15f
#define turn_kp_default1						30.0f 	//基于红外对管识别轨迹时,自主寻迹的位置控制器比例参数KP	 位置控制器
#define turn_ki_default1						0.0f		//基于红外对管识别轨迹时,自主寻迹的位置控制器积分参数KI	
#define turn_kd_default1						150			//基于红外对管识别轨迹时,自主寻迹的位置控制器微分参数KD	
extern float turn_kp,turn_ki,turn_kd;
extern float cam_turn_kp,cam_turn_ki,cam_turn_kd;
extern uint16_t gray_cnt;
extern float v_target_l,v_target_r,p_target_l,p_target_r;
extern float speed_kp_l,speed_ki_l,speed_kp_r,speed_ki_r;
extern float	turn_scale,speed_setup, turn_output,turn_ctrl_pwm,speed_adjust;
extern  float left_pwm,right_pwm;
extern float yaw_target;
extern float turn_kp_L;
extern float turn_ki_L;
extern float turn_kd_L;
extern float yaw_kp,yaw_ki,yaw_kd;
extern float position_kp,position_kd;
void ctrl_params_init(void);
extern float tar_theta_limit,sleep_time;
#endif

