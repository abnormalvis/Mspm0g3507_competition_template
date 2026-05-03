#ifndef _HAL_ENCODE_H
#define _HAL_ENCODE_H

#include <stdint.h>
#include "move_filter.h"

//**************************************************

typedef struct
{
	float speed;
	float distance;
}two_wheel_model;

typedef struct
{
	float left_motor_speed_cmps;
	float right_motor_speed_cmps;
	float average_speed_cmps;
	
	two_wheel_model state_estimation;
	
	uint8_t temperature_stable_flag;
	uint8_t imu_cal_flag;
}sensor;	


typedef struct
{
	int32_t left_motor_cnt,right_motor_cnt;//单个采样周期内的脉冲数量
	float left_motor_dir,right_motor_dir; //运动方向
	float left_motor_speed_rpm,right_motor_speed_rpm;//转速单位转每分钟
	float left_motor_speed_cmps,right_motor_speed_cmps;//转速c单位为cm/s
	
}encoder;

#define pulse_num_per_circle 1057.0f //轮子转一圈的脉冲数  

#define left_motor_period_ms 10.0f //脉冲的采样周期
#define right_motor_period_ms 10.0f //脉冲的采样周期

#define wheel_radius_cm 2.3f //轮胎长度半径

#define pi 3.1415f
extern encoder NEncoder;
extern sensor smartcar_imu;
extern float distance_inter,distance_l,distance_r,distance_point;
extern move_filter_struct left_speed_cmps,right_speed_cmps;
extern float point_A[2],point_B[2],point_C[2],point_D[2],point_actual[2];
void hal_Encoder_Init(void);
void get_wheel_speed(void);
//void encoder_read(int32_t *a,int32_t *b);
//void encoder_read_l(int32_t *l);
//void encoder_read_r(int32_t *r);
//void encoder_clear_l(int32_t *l);
#endif

