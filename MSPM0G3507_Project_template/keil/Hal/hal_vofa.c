/*******************************************************************************
  * @作者      ： wangming
  * @wechat    :DeepCoderMing
  * @qq      ： 3201935299
  * @日期      ： 2025年05月01日
  * @版权声明  ： 仅供参考学习，未经允许禁止商用
********************************************************************************/
#include "hal_vofa.h"
#include "hal_uart.h"

static uint8_t vofa_buffer[VOFA_FLOAT_NUM*4+4];
static uint16_t cnt = 0;

void vofa_add_data(float data)
{
    vofa_buffer[cnt ++] = *((uint8_t *)(&data));
    vofa_buffer[cnt ++] = *((uint8_t *)(&data)+1);
    vofa_buffer[cnt ++] = *((uint8_t *)(&data)+2);
    vofa_buffer[cnt ++] = *((uint8_t *)(&data)+3);
}

void vofa_send()
{
    vofa_buffer[cnt ++] = 0x00;
    vofa_buffer[cnt ++] = 0x00;
    vofa_buffer[cnt ++] = 0x80;
    vofa_buffer[cnt ++] = 0x7f;

    UART0_send(vofa_buffer, cnt);
    cnt = 0;
}
