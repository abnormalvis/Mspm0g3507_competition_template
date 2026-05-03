#ifndef _HAL_JY62_H
#define _HAL_JY62_H

//**************************************************
typedef struct {
    float x;
    float y;
    float z;
}imu_data_t;

#define ERROR   0
#define DOING   1
#define SUCCESS 2

void imu_init(void);
void imu_analysis(void);
extern imu_data_t acceleration, gyroscope, angle;
//extern int32_t state;
extern char analysis_flag;

#endif





