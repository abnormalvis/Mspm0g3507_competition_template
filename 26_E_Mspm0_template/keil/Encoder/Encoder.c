#include "Encoder.h"
#include "../App/move_filter.h"

int32_t Count1 = 0;
int32_t Count2 = 0;
static int32_t LastCount1 = 0;
static int32_t LastCount2 = 0;
int32_t Motor_speedL, Motor_speedR, Motor_distanceL, Motor_distanceR;

static move_filter_struct left_speed_filter;
static move_filter_struct right_speed_filter;
static lowpass_filter_struct left_lpf;
static lowpass_filter_struct right_lpf;

void EncoderFilterInit(void)
{
    move_filter_init(&left_speed_filter);
    move_filter_init(&right_speed_filter);
    lowpass_filter_init(&left_lpf, SPEED_LOWPASS_ALPHA);
    lowpass_filter_init(&right_lpf, SPEED_LOWPASS_ALPHA);
}

/**
 * 编码�?? GPIO �??�??处理，由统一�?? GROUP1_IRQHandler 调用
 * 入参 gpioB �?? GPIOB 已使能的�??�??状态位掩码
 */
void Encoder_OnGroupIRQ(uint32_t gpioB)
{
    if (gpioB & Encoder_Encoder1_A_PIN)
    {
        if (DL_GPIO_readPins(Encoder_Encoder1_A_PORT, Encoder_Encoder1_B_PIN))
            Count1--;
        else
            Count1++;
        DL_GPIO_clearInterruptStatus(GPIOB, Encoder_Encoder1_A_PIN);
    }
    if (gpioB & Encoder_Encoder2_A_PIN)
    {
        if (DL_GPIO_readPins(Encoder_Encoder2_A_PORT, Encoder_Encoder2_B_PIN))
            Count2--;
        else
            Count2++;
        DL_GPIO_clearInterruptStatus(GPIOB, Encoder_Encoder2_A_PIN);
    }
}

void EncoderGetValue(void)
{
    int32_t temp = LastCount1;
    LastCount1 = Count1;
    Motor_speedL = Count1 - temp;
    temp = LastCount2;
    LastCount2 = Count2;
    Motor_speedR = Count2 - temp;
    Motor_distanceL = Count1;
    Motor_distanceR = Count2;

    move_filter_calc(&left_speed_filter, (float)Motor_speedL);
    move_filter_calc(&right_speed_filter, (float)Motor_speedR);
    // Motor_speedL = (int32_t)lowpass_filter_calc(&left_lpf, left_speed_filter.data_average);
    // Motor_speedR = (int32_t)lowpass_filter_calc(&right_lpf, right_speed_filter.data_average);
    Motor_speedL = -left_speed_filter.data_average;
    Motor_speedR = -right_speed_filter.data_average;
}
