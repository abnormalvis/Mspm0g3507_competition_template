#include "hal_imu.h"

#if IMU_USE_BNO080
    #include "bno080.h"
#elif IMU_USE_IMU660RA
    #include "zf_device_imu660ra.h"
    #include "imu_filter.h"
#elif IMU_USE_ICM42688
    #include "imu_icm42688.h"
#else
    #error "No IMU driver selected! Define one of IMU_USE_ICM42688, IMU_USE_IMU660RA, IMU_USE_BNO080 in hal_imu.h"
#endif

void hal_imu_init(void)
{
#if IMU_USE_BNO080
    bno080_init();
#elif IMU_USE_IMU660RA
    imu660ra_init();
    gyroOffsetInit();
    IIR_imu();
#elif IMU_USE_ICM42688
    IMU_init();
#endif
}

void hal_imu_update(void)
{
#if IMU_USE_BNO080
    bno080_update();
#elif IMU_USE_IMU660RA
    _IMU();
#elif IMU_USE_ICM42688
    imu_icm42688_update();
#endif
}
