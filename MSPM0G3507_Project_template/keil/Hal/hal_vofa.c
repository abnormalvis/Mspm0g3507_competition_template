/*******************************************************************************
  * @brief     : VOFA+ protocol implementation (send/receive)
  * @author   : wangming
  * @wechat   : DeepCoderMing
  * @qq       : 3201935299
  * @date     : 2025-05-01
  * @copyright: Confidential - for demo purposes only
********************************************************************************/
#include "hal_vofa.h"
#include "hal_uart.h"

#define VOFA_RX_BUFFER_SIZE  64

/* 接收缓冲区 */
static uint8_t vofa_rx_buffer[VOFA_RX_BUFFER_SIZE];
static uint16_t vofa_tx_cnt = 0;

/* 协议状态机 */
typedef enum {
    RX_IDLE = 0,
    RX_WAIT_HASH,
    RX_WAIT_P,
    RX_WAIT_ID,
    RX_WAIT_EQUAL,
    RX_DATA
} vofa_rx_state_t;

static volatile vofa_rx_state_t rx_state = RX_IDLE;
static uint8_t rx_index = 0;
static volatile uint8_t vofa_rx_flag = 0;

/* PID参数存储 (可通过VOFA动态调整) */
float vofa_param[16] = {0};

/* 协议解析: #P<id>=<value>! */
void vofa_rx_parse(uint8_t byte)
{
    switch(rx_state)
    {
        case RX_IDLE:
            if(byte == '#')
                rx_state = RX_WAIT_HASH;
            break;

        case RX_WAIT_HASH:
            if(byte == 'P')
                rx_state = RX_WAIT_P;
            else
                rx_state = RX_IDLE;
            break;

        case RX_WAIT_P:
            if(byte >= '0' && byte <= '9')
            {
                rx_index = byte - '0';
                rx_state = RX_WAIT_ID;
            }
            else
                rx_state = RX_IDLE;
            break;

        case RX_WAIT_ID:
            if(byte == '=')
                rx_state = RX_WAIT_EQUAL;
            else
                rx_state = RX_IDLE;
            break;

        case RX_WAIT_EQUAL:
            if(byte == '!')
            {
                vofa_rx_buffer[rx_index] = '\0';
                rx_index = 0;
                rx_state = RX_IDLE;
                vofa_rx_flag = 1;
            }
            else
            {
                if(rx_index < VOFA_RX_BUFFER_SIZE - 1)
                    vofa_rx_buffer[rx_index++] = byte;
                else
                    rx_state = RX_IDLE;
            }
            break;

        default:
            rx_state = RX_IDLE;
            break;
    }
}

/* 将字符串转换为浮点数 */
static float vofa_str_to_float(uint8_t *str)
{
    float result = 0.0f;
    float sign = 1.0f;
    float decimal = 0.1f;
    uint8_t has_dot = 0;
    uint8_t i = 0;

    if(str[0] == '-')
    {
        sign = -1.0f;
        i = 1;
    }

    while(str[i])
    {
        if(str[i] == '.')
        {
            has_dot = 1;
            i++;
            continue;
        }
        if(str[i] >= '0' && str[i] <= '9')
        {
            if(has_dot)
            {
                result += (str[i] - '0') * decimal;
                decimal *= 0.1f;
            }
            else
            {
                result = result * 10.0f + (str[i] - '0');
            }
        }
        i++;
    }

    return result * sign;
}

/* 处理接收到的参数 */
void vofa_param_update(void)
{
    if(vofa_rx_flag)
    {
        vofa_rx_flag = 0;
        uint8_t id = vofa_rx_buffer[0] - '0';
        float value = vofa_str_to_float(&vofa_rx_buffer[1]);

        if(id < 16)
            vofa_param[id] = value;

        /* 清空缓冲区 */
        for(uint8_t i = 0; i < VOFA_RX_BUFFER_SIZE; i++)
            vofa_rx_buffer[i] = 0;
    }
}

/* 获取参数 */
float vofa_get_param(uint8_t id)
{
    if(id < 16)
        return vofa_param[id];
    return 0;
}

/* 是否有新数据 */
uint8_t vofa_new_data(void)
{
    return vofa_rx_flag;
}

/* 添加发送数据 */
void vofa_add_data(float data)
{
    vofa_rx_buffer[vofa_tx_cnt ++] = *((uint8_t *)(&data));
    vofa_rx_buffer[vofa_tx_cnt ++] = *((uint8_t *)(&data)+1);
    vofa_rx_buffer[vofa_tx_cnt ++] = *((uint8_t *)(&data)+2);
    vofa_rx_buffer[vofa_tx_cnt ++] = *((uint8_t *)(&data)+3);
}

/* 发送数据 */
void vofa_send(void)
{
    vofa_rx_buffer[vofa_tx_cnt ++] = 0x00;
    vofa_rx_buffer[vofa_tx_cnt ++] = 0x00;
    vofa_rx_buffer[vofa_tx_cnt ++] = 0x80;
    vofa_rx_buffer[vofa_tx_cnt ++] = 0x7f;

    UART1_send(vofa_rx_buffer, vofa_tx_cnt);
    vofa_tx_cnt = 0;
}

/* 接收一字节处理 (供中断调用) */
void vofa_rx_byte(uint8_t byte)
{
    vofa_rx_parse(byte);
}