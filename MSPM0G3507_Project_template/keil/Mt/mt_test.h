#ifndef _MT_TEST_H
#define _MT_TEST_H

#include "stdint.h"

#define speed_expect_default 			0.0f			// Default speed when line following

#define speed_err_max  80.0f				// Speed error limit
#define speed_integral_max  60.0f	// Integral limit
#define speed_ctrl_output_max 70.0f  	// Control output limit
#define  motor_max_default (85-1)


#define speed_kp_default  				0.0f//8.0f	26.0f		// Speed loop P coefficient
#define	speed_ki_default					0.0f//0.6f	 0.85f		// Speed loop I coefficient
#define	speed_kd_default					0.0f			// Speed loop D coefficient

// Position PID control parameters
//#define position_kp_default       4.5f
//#define position_ki_default       0.05f
//#define position_kd_default       0.001f

// Angle PD control parameters (comment config)
//#define yaw_kp                    0.4f//30.0f
//#define yaw_ki                    0.0001f//0.5
//#define yaw_kd                    0.8f//2.0f


#define steer_deadzone 50//50  Steering dead zone
#define turn_scale_default        	0.06		// Turn coefficient (used when no grayscale cross-track detection)
#define turn_kp_default1						30.0f 	// Turn loop P coefficient (grayscale cross-track position control)
#define turn_ki_default1						0.0f		// Turn loop I coefficient (grayscale cross-track position control)
#define turn_kd_default1						150			// Turn loop D coefficient (grayscale cross-track position control)	
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

