#ifndef _HAL_IMU_H
#define _HAL_IMU_H

#include <stdint.h>

/* Select exactly one IMU driver:
 *   0 = ICM42688 (SPI, Madgwick AHRS on MCU)
 *   1 = IMU660RA  (SPI, SeekFree driver + Mahony AHRS)
 *   2 = BNO080    (SPI, on-chip fusion via SHTP/SH-2)
 */
#define IMU_USE_ICM42688 1
#define IMU_USE_IMU660RA  0
#define IMU_USE_BNO080    0

void hal_imu_init(void);
void hal_imu_update(void);

#endif
