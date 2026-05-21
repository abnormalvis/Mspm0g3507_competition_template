#include "Encoder.h"

static int32_t Count1 = 0;
static int32_t Count2 = 0;
static int32_t LastCount1 = 0;
static int32_t LastCount2 = 0;
int32_t Motor_speedL, Motor_speedR, Motor_distanceL, Motor_distanceR;

/**
 * 编码器 GPIO 中断处理，由统一的 GROUP1_IRQHandler 调用
 * 入参 gpioB 是 GPIOB 已使能的中断状态位掩码
 */
void Encoder_OnGroupIRQ(uint32_t gpioB)
{
    if (gpioB & Encoder_Encoder1_A_PIN)
    {
        if (DL_GPIO_readPins(Encoder_Encoder1_A_PORT, Encoder_Encoder1_B_PIN))
            Count1++;
        else
            Count1--;
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
}
