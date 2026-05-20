#include "hal_tb6612.h"
#include <ti/driverlib/dl_timera.h>

void Motor_Stop(void)
{
    AIN1_OUT(1); AIN2_OUT(1); BIN1_OUT(1); BIN2_OUT(1);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C3_IDX);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C1_IDX);
}

/* Left motor = B channels + PWM C3 (template convention) */
void Set_MotorL_Speed(uint8_t dir, uint32_t speed)
{
    if (dir == 1) { BIN1_OUT(1); BIN2_OUT(0); }
    else          { BIN1_OUT(0); BIN2_OUT(1); }
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, speed, GPIO_PWM_0_C3_IDX);
}

/* Right motor = A channels + PWM C1 (template convention) */
void Set_MotorR_Speed(uint8_t dir, uint32_t speed)
{
    if (dir == 1) { AIN1_OUT(1); AIN2_OUT(0); }
    else          { AIN1_OUT(0); AIN2_OUT(1); }
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, speed, GPIO_PWM_0_C1_IDX);
}
