#include "hal_imu.h"
#include "imu_icm42688.h"

void hal_imu_init(void)
{
    IMU_init();
}

void hal_imu_update(void)
{
    imu_icm42688_update();
}
