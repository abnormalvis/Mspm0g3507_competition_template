#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "hal_tb6612.h"
#include <ti/driverlib/dl_timera.h>

/* Usage: Left motor forward
 * Input params:
 * PWM_Hight: target high-level pulse width
 * Return params: None
 */
void Motor_Foreward_Left(uint16_t PWM_Hight)
{
    BIN1_OUT(0);
    BIN2_OUT(1);

    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C3_IDX);
}
/* Usage: Left motor reverse
 * Input params:
 * PWM_Hight: target high-level pulse width
 * Return params: None
 */
void Motor_Backward_Left(uint16_t PWM_Hight)
{
    BIN1_OUT(1);
    BIN2_OUT(0);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C3_IDX);
}

/* Usage: Right motor forward
 * Input params:
 * PWM_Hight: target high-level pulse width, unit 0.8us
 * Return params: None
 */
void Motor_Foreward_Right(uint16_t PWM_Hight)
{
    AIN1_OUT(1);
    AIN2_OUT(0);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C1_IDX);
}

/* Usage: Right motor reverse
 * Input params:
 * PWM_Hight: target high-level pulse width, unit 0.8us
 * Return params: None
 */
void Motor_Backward_Right(uint16_t PWM_Hight)
{
    AIN1_OUT(0);
    AIN2_OUT(1);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C1_IDX);
}

/* Usage: Stop all motors
 * Input params: None
 * Return params: None
 */

void Motor_Stop_All(void)
{
    AIN1_OUT(0);
    AIN2_OUT(0);
    BIN1_OUT(0);
    BIN2_OUT(0);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C1_IDX);
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C3_IDX);
}

/* Set PWM for all 4 channels - matching reference project interface
 * a: left motor forward PWM
 * b: left motor backward PWM
 * c: right motor forward PWM
 * d: right motor backward PWM */
void Set_Pwm(int a, int b, int c, int d)
{
    if (a > 0)
    {
        AIN1_OUT(0);
        AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)a, GPIO_PWM_0_C3_IDX);
    }
    else if (a < 0)
    {
        AIN1_OUT(1);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-a), GPIO_PWM_0_C3_IDX);
    }
    else
    {
        AIN1_OUT(0);
        AIN2_OUT(0);
    }

    if (b > 0)
    {
        BIN1_OUT(0);
        BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)b, GPIO_PWM_0_C1_IDX);
    }
    else if (b < 0)
    {
        BIN1_OUT(1);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-b), GPIO_PWM_0_C1_IDX);
    }
    else
    {
        BIN1_OUT(0);
        BIN2_OUT(0);
    }

    if (c > 0)
    {
        AIN1_OUT(0);
        AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)c, GPIO_PWM_0_C3_IDX);
    }
    else if (c < 0)
    {
        AIN1_OUT(1);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-c), GPIO_PWM_0_C3_IDX);
    }
    else
    {
        AIN1_OUT(0);
        AIN2_OUT(0);
    }

    if (d > 0)
    {
        BIN1_OUT(0);
        BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)d, GPIO_PWM_0_C1_IDX);
    }
    else if (d < 0)
    {
        BIN1_OUT(1);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-d), GPIO_PWM_0_C1_IDX);
    }
    else
    {
        BIN1_OUT(0);
        BIN2_OUT(0);
    }
}

/* Debug: direct PWM output bypassing PID and direction logic
 * left_duty/right_duty: positive=forward, negative=backward, 0=stop */
void Set_Pwm_Debug(int left_duty, int right_duty)
{
    if (left_duty > 0)
    {
        AIN1_OUT(0);
        AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)left_duty, GPIO_PWM_0_C3_IDX);
    }
    else if (left_duty < 0)
    {
        AIN1_OUT(1);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-left_duty), GPIO_PWM_0_C3_IDX);
    }
    else
    {
        AIN1_OUT(0);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C3_IDX);
    }

    if (right_duty > 0)
    {
        BIN1_OUT(0);
        BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)right_duty, GPIO_PWM_0_C1_IDX);
    }
    else if (right_duty < 0)
    {
        BIN1_OUT(1);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-right_duty), GPIO_PWM_0_C1_IDX);
    }
    else
    {
        BIN1_OUT(0);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C1_IDX);
    }
}
