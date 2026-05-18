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
	int32_t left_motor_cnt,right_motor_cnt;// Pulse count per sampling period (left/right)
	float left_motor_dir,right_motor_dir; // Motion direction
	float left_motor_speed_rpm,right_motor_speed_rpm;// Speed in rpm
	float left_motor_speed_cmps,right_motor_speed_cmps;// Speed in cm/s
	
}encoder;

#define pulse_num_per_circle 28000.0f // 500 lines * 2x factor * 1:28 gear ratio = 28000 pulses/rev  

#define left_motor_period_ms 10.0f // Left motor sampling period (ms)
#define right_motor_period_ms 10.0f // Right motor sampling period (ms)
#define motor_countint_phases 2
#define motor_reduction_ratio 28
extern float wheel_radius_cm;

#define pi 3.1415f
extern encoder NEncoder;
extern sensor smartcar_imu;
extern float distance_inter,distance_l,distance_r,distance_point;
extern move_filter_struct left_speed_cmps,right_speed_cmps;
extern float point_A[2],point_B[2],point_C[2],point_D[2],point_actual[2];
extern int32_t enc_cnt[2]; // Raw encoder count for debugging
void hal_Encoder_Init(void);
void get_wheel_speed(void);
float convert_current_rpm_to_cms(int Encoder_lines, int Encoder_counts, int Phases, int Reduction_ratio, float wheel_radius_cms);
//void encoder_read(int32_t *a,int32_t *b);
//void encoder_read_l(int32_t *l);
//void encoder_read_r(int32_t *r);
//void encoder_clear_l(int32_t *l);
#endif

