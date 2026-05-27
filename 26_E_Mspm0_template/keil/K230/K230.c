#include "K230.h"
#include <math.h>
#include <stdio.h>

static uint8_t Serial_RxFlag = 0;
static uint8_t Serial_RxPacket[100]; //最多接收100个数据
float k230_RxPacket[50];             //存储经过码值转换的数据

/**
 *  ASCII字符串 → float 数组（支持整数/小数/负数，分隔符','）
 */
static void AsciiToFloat(uint8_t *str1, float *str2)
{
    float    integer_part = 0.0f;
    float    decimal_part = 0.0f;
    uint8_t  decimal_digit = 0;
    uint8_t  is_decimal   = 0;
    uint8_t  has_digit    = 0;
    uint8_t  is_negative  = 0;

    while (*str1 != '\0')
    {
        if (*str1 == '-' && has_digit == 0 && is_decimal == 0)
        {
            is_negative = 1;
        }
        else if (*str1 >= '0' && *str1 <= '9')
        {
            has_digit = 1;
            if (is_decimal == 0)
                integer_part = integer_part * 10.0f + (*str1 - '0');
            else
            {
                decimal_part = decimal_part * 10.0f + (*str1 - '0');
                decimal_digit++;
            }
        }
        else if (*str1 == '.')
        {
            is_decimal = 1;
        }
        else if (*str1 == ',' && has_digit == 1)
        {
            float final_val = integer_part + (decimal_part / powf(10.0f, decimal_digit));
            if (is_negative) final_val = -final_val;
            *str2++ = final_val;
            integer_part = 0.0f;
            decimal_part = 0.0f;
            decimal_digit = 0;
            is_decimal = 0;
            has_digit = 0;
            is_negative = 0;
        }
        str1++;
    }

    if (has_digit == 1)
    {
        float final_val = integer_part + (decimal_part / powf(10.0f, decimal_digit));
        if (is_negative) final_val = -final_val;
        *str2 = final_val;
    }
}

/**
 *  '['..']' 框架解包，状态机
 */
static void TransDataBag(uint8_t ReceiveData, uint8_t *ReceiveFlag, uint8_t *ReceivePacket)
{
    static uint8_t pReceivePacket = 0;
    static uint8_t State = 0;

    if (State == 0)
    {
        if (ReceiveData == '[' && *ReceiveFlag == 0)
        {
            State = 1;
            pReceivePacket = 0;
        }
    }
    else if (State == 1)
    {
        if (ReceiveData == ']')
        {
            State = 0;
            ReceivePacket[pReceivePacket] = '\0';
            *ReceiveFlag = 1;
        }
        else
        {
            ReceivePacket[pReceivePacket++] = ReceiveData;
        }
    }
}

void K230_SendByte(uint8_t data)
{
    uart3_send_byte(data);
}

void K230_ReceiveData(uint8_t RxData)
{
    TransDataBag(RxData, &Serial_RxFlag, Serial_RxPacket);
    if (Serial_RxFlag == 1)
    {
        AsciiToFloat(Serial_RxPacket, k230_RxPacket);
        Serial_RxFlag = 0;
    }
}

void K230_SendDataPkg(float Channel1, float Channel2, float Channel3, float Channel4,
                      float Channel5, float Channel6, float Channel7, float Channel8, uint8_t ChannelNum)
{
    if (ChannelNum < 1 || ChannelNum > 8) return;

    float channels[8] = { Channel1, Channel2, Channel3, Channel4,
                          Channel5, Channel6, Channel7, Channel8 };

    char send_buf[128] = { 0 };
    int  len = snprintf(send_buf, sizeof(send_buf), "[");

    for (uint8_t i = 0; i < ChannelNum; i++)
    {
        len += snprintf(send_buf + len, sizeof(send_buf) - len, "%.2f", channels[i]);
        if (i != ChannelNum - 1)
            len += snprintf(send_buf + len, sizeof(send_buf) - len, ",");
    }
    snprintf(send_buf + len, sizeof(send_buf) - len, "]\r\n");

    uart3_send_string(send_buf);
}

void k230_send_string(char *str)
{
    uart3_send_string(str);
}
