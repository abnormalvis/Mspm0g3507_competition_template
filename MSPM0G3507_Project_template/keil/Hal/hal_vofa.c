/*******************************************************************************
  * @author     : wangming
  * @wechat     :DeepCoderMing
  * @qq         : 3201935299
  * @date       : 2025年05月01日
  * @copyright  : 仅供参考学习，禁止商业传播
********************************************************************************/
#include "hal_vofa.h"
#include "hal_uart.h"

static uint8_t vofa_buffer[VOFA_FLOAT_NUM*4+4];
static uint16_t cnt = 0;

/* VOFA parameter receive related */
static uint8_t vofa_rx_buffer[32];
static uint8_t vofa_rx_index = 0;
static uint8_t vofa_rx_id = 0;
static uint8_t vofa_param_update = 0;
static float vofa_params[VOFA_PARAM_MAX + 1] = {0};

void vofa_add_data(float data)
{
    vofa_buffer[cnt ++] = *((uint8_t *)(&data));
    vofa_buffer[cnt ++] = *((uint8_t *)(&data)+1);
    vofa_buffer[cnt ++] = *((uint8_t *)(&data)+2);
    vofa_buffer[cnt ++] = *((uint8_t *)(&data)+3);
}

void vofa_send(void)
{
    vofa_buffer[cnt ++] = 0x00;
    vofa_buffer[cnt ++] = 0x00;
    vofa_buffer[cnt ++] = 0x80;
    vofa_buffer[cnt ++] = 0x7f;

    UART1_send(vofa_buffer, cnt);
    cnt = 0;
}

void vofa_param_init(void)
{
    vofa_rx_index = 0;
    vofa_rx_id = 0;
    vofa_param_update = 0;
}

/* Parse value from receive buffer */
static float vofa_parse_float(uint8_t *buf, uint8_t len)
{
    float data = 0.0f;
    uint8_t i;
    uint8_t dot_pos = 0;
    int8_t sign = 1;
    float div = 1.0f;

    for(i = 0; i < len; i++)
    {
        if(buf[i] == '-')
        {
            sign = -1;
        }
        else if(buf[i] == '.')
        {
            dot_pos = i;
        }
        else if(buf[i] >= '0' && buf[i] <= '9')
        {
            if(dot_pos == 0)
            {
                data = data * 10.0f + (buf[i] - '0');
            }
            else
            {
                div *= 10.0f;
                data += (buf[i] - '0') / div;
            }
        }
    }
    return data * sign;
}

/* UART receive callback - called by interrupt */
void vofa_uart_rx_callback(uint8_t byte)
{
    static uint8_t state = 0;
    static uint8_t data_start = 0;

    switch(state)
    {
        case 0: /* wait for # */
            if(byte == '#')
            {
                vofa_rx_index = 0;
                state = 1;
            }
            break;
        case 1: /* wait for ID */
            if(byte >= '0' && byte <= '9')
            {
                vofa_rx_id = vofa_rx_id * 10 + (byte - '0');
            }
            else if(byte == ':')
            {
                data_start = vofa_rx_index;
                state = 2;
            }
            else if(byte == '#')
            {
                vofa_rx_index = 0;
                vofa_rx_id = 0;
            }
            else
            {
                state = 0;
                vofa_rx_id = 0;
            }
            break;
        case 2: /* wait for value end */
            if(byte == '#' || vofa_rx_index >= 31)
            {
                if(vofa_rx_id > 0 && vofa_rx_id <= VOFA_PARAM_MAX)
                {
                    vofa_params[vofa_rx_id] = vofa_parse_float(&vofa_rx_buffer[data_start], vofa_rx_index - data_start);
                    vofa_param_update = 1;
                }
                vofa_rx_index = 0;
                vofa_rx_id = 0;
                state = (byte == '#') ? 1 : 0;
            }
            else
            {
                vofa_rx_buffer[vofa_rx_index++] = byte;
            }
            break;
    }
}

uint8_t vofa_get_param_id(void)
{
    uint8_t id = vofa_param_update ? vofa_rx_id : 0;
    vofa_param_update = 0;
    return id;
}

float vofa_get_param_value(uint8_t id)
{
    if(id >= 1 && id <= VOFA_PARAM_MAX)
    {
        return vofa_params[id];
    }
    return 0.0f;
}