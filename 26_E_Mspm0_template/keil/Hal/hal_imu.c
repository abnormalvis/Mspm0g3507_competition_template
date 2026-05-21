#include "hal_imu.h"
#include "zf_device_imu660ra.h"
#include "imu_filter.h"

void hal_imu_init(void)
{
    imu660ra_init();
    gyroOffsetInit();
    IIR_imu();
}

void hal_imu_update(void)
{
    _IMU();
}
