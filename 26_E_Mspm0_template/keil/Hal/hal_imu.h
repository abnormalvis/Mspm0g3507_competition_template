#ifndef _HAL_IMU_H
#define _HAL_IMU_H

#include <stdint.h>

/* 1 = use IMU660RA (SPI), 0 = use ICM42688 (SPI) */
//#define IMU_USE_IMU660RA 1

void hal_imu_init(void);
void hal_imu_update(void);

#endif
