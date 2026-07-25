#include "hal_imu.h"

#if IMU_USE_IMU660RA
    #include "zf_device_imu660ra.h"
    #include "imu_filter.h"
#else
    #include "imu_icm42688.h"
#endif

void hal_imu_init(void)
{
#if IMU_USE_IMU660RA
    imu660ra_init();
    gyroOffsetInit();
    IIR_imu();
#else
    IMU_init();
#endif
}

void hal_imu_update(void)
{
#if IMU_USE_IMU660RA
    _IMU();
#else
    imu_icm42688_update();
#endif
}
