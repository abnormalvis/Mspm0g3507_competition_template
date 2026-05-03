/*******************************************************************************
  * @brief     : 8-channel grayscale sensor driver
  * @author   : wangming
  * @wechat   : DeepCoderMing
  * @qq       : 3201935299
  * @date     : 2025-05-01
  * @copyright: Confidential - for demo purposes only
********************************************************************************/
#include "hal_gray.h"
#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "ti/driverlib/dl_adc12.h"
#include "mt_flag.h"

/* 8路灰度传感器的ADC值 (0-100) */
uint16_t LQ_Tracking_Value[8] = {0};

/* 阈值设置 (0-100范围,默认50) */
uint16_t gray_threshold[8] = {50, 50, 50, 50, 50, 50, 50, 50};

/* 12路灰度传感器的信息
 * 注意: 共用8路ADC+3路IO
 * 用S2,S1,S0选通通道
 */
_gray_state gray_state;
float gray_status = 0, gray_status_backup = 0;
uint32_t gray_status_worse = 0;
char stop_flag = 0;

/* 用S2,S1,S0选通通道(S2,S1,S0)=(0,0,0)~(1,1,1) 对应通道1~8 */
void Tracking_IO_Set(unsigned char s2, unsigned char s1, unsigned char s0)
{
    if(s0) DL_GPIO_setPins(Tracking_SO_PORT, Tracking_SO_PIN);
    else DL_GPIO_clearPins(Tracking_SO_PORT, Tracking_SO_PIN);

    if(s1) DL_GPIO_setPins(Tracking_S1_PORT, Tracking_S1_PIN);
    else DL_GPIO_clearPins(Tracking_S1_PORT, Tracking_S1_PIN);

    if(s2) DL_GPIO_setPins(Tracking_S2_PORT, Tracking_S2_PIN);
    else DL_GPIO_clearPins(Tracking_S2_PORT, Tracking_S2_PIN);
}

/* 读取8路灰度传感器的ADC值 (与LQ一致: 5次采样,去3取2,转换为0-100) */
void gray_8data_read(void)
{
    unsigned char i, j;
    uint16_t sum = 0;
    uint16_t data = 0;

    for(i = 0; i < 8; i++)
    {
        Tracking_IO_Set(i >> 2, (i >> 1) & 0x01, i & 0x01);  //选通通道 i

        /* 5次采样,去掉前3次,取后2次均值 (与LQ一致) */
        sum = 0;
        for(j = 0; j < 5; j++)
        {
            /* 延时等待ADC稳定 */
            for(volatile uint16_t d = 0; d < 50; d++);
            data = DL_ADC12_getMemResult(ADC12_0_INST, ADC12_0_ADCMEM_0);

            /* 转换为0-100百分比 (与LQ一致) */
            data = data * 0.02442;
            if(data > 100) data = 100;

            /* 去掉前3次,只累加后2次 */
            if(j >= 3)
                sum += data;
        }
        LQ_Tracking_Value[i] = sum / 2;
    }

    /* 灰度读取状态 */
    gray_state.gray.bit1 = LQ_Tracking_Value[0] < gray_threshold[0] ? 1 : 0;
    gray_state.gray.bit2 = LQ_Tracking_Value[1] < gray_threshold[1] ? 1 : 0;
    gray_state.gray.bit3 = LQ_Tracking_Value[2] < gray_threshold[2] ? 1 : 0;
    gray_state.gray.bit4 = LQ_Tracking_Value[3] < gray_threshold[3] ? 1 : 0;
    gray_state.gray.bit5 = LQ_Tracking_Value[4] < gray_threshold[4] ? 1 : 0;
    gray_state.gray.bit6 = LQ_Tracking_Value[5] < gray_threshold[5] ? 1 : 0;
    gray_state.gray.bit7 = LQ_Tracking_Value[6] < gray_threshold[6] ? 1 : 0;
    gray_state.gray.bit8 = LQ_Tracking_Value[7] < gray_threshold[7] ? 1 : 0;

    /* 计算当前状态为位置量 */
    gray_status_backup = gray_status;
    switch(gray_state.state)
    {
        case 0x0001: gray_status = 14; gray_status_worse /= 2; break;
        case 0x0003: gray_status = 13; gray_status_worse /= 2; break;
        case 0x0002: gray_status = 12; gray_status_worse /= 2; break;
        case 0x0006: gray_status = 11; gray_status_worse /= 2; break;
        case 0x0004: gray_status = 10; gray_status_worse /= 2; break;
        case 0x000C: gray_status = 9; gray_status_worse /= 2; break;
        case 0x0008: gray_status = 8; gray_status_worse /= 2; break;
        case 0x0018: gray_status = 7; gray_status_worse /= 2; break;
        case 0x0010: gray_status = 6; gray_status_worse /= 2; break;
        case 0x0030: gray_status = 5; gray_status_worse /= 2; break;
        case 0x0020: gray_status = 4; gray_status_worse /= 2; break;
        case 0x0060: gray_status = 3; gray_status_worse /= 2; break;
        case 0x0040: gray_status = 2; gray_status_worse /= 2; break;
        case 0x00C0: gray_status = 1; gray_status_worse /= 2; break;
        case 0x0080: gray_status = 0; gray_status_worse /= 2; break;
        case 0x0000: gray_status = gray_status_backup; gray_status_worse++; break;
        default:
            gray_status = 0;
            gray_status_worse++;
            break;
    }

    if(gray_status_worse > 25)
    {
        Flag.gray_worse = 1;
    }
    else
    {
        Flag.gray_worse = 0;
    }
}

/* 设置灰度的阈值 */
void gray_set_threshold(uint16_t* threshold)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        gray_threshold[i] = threshold[i];
    }
}