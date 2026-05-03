#ifndef __IMU_FILTER_H
#define __IMU_FILTER_H
#include "ti_msp_dl_config.h"

typedef struct
{
  float x;
  float y;
  float z;
} SI_F_XYZ;

typedef struct
{
	SI_F_XYZ deg_s; // 度每秒
	SI_F_XYZ rad_s; // 弧度每秒
	SI_F_XYZ acc_g; // 加速度

	float att_acc_factor;
	float att_gryo_factor;
	float roll;
	float pitch;
	float yaw;
	} imu660_data;


typedef struct
{
	int16_t Xdata;
	int16_t Ydata;
	int16_t Zdata;
} gyro_param_t;

#define PI 3.1415926f

extern int16_t AX, AY, AZ, GX, GY, GZ;
extern int16_t acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;
extern float filter_yaw;  
extern gyro_param_t GyroOffset;
extern imu660_data imu; // 陀螺仪数据存储
extern float mag_ratio;      //?????   

extern float gyro_ratio;    //?????   
void IIR_imu(void);
void gyroOffsetInit(void);
void _IMU(void);
float angle_calc(float mag_angle_m,float gyro_m);   

#endif
