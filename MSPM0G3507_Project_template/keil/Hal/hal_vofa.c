/*******************************************************************************
  * @author     : wangming
  * @wechat     :DeepCoderMing
  * @qq         : 3201935299
  * @date       : 2025-05-01
  * @copyright  : For reference and learning only. Redistribution prohibited.
********************************************************************************/
#include "hal_vofa.h"
#include "hal_uart.h"
#include "hal_encode.h"

/* VOFA parameter receive related */
static uint8_t vofa_rx_buffer[32];
static uint8_t vofa_rx_index = 0;
static uint8_t vofa_rx_id = 0;
static uint8_t vofa_last_param_id = 0;
static uint8_t vofa_param_update = 0;
static float vofa_params[VOFA_PARAM_MAX + 1] = {0};

void vofa_param_init(void)
{
    vofa_rx_index = 0;
    vofa_rx_id = 0;
    vofa_last_param_id = 0;
    vofa_param_update = 0;
}

void vofa_register_param_callback(vofa_param_callback_t callback, void *user)
{
    (void)callback;
    (void)user;
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

static void vofa_store_received_value(void)
{
    float value = 0.0f;

    if(vofa_rx_id == 99)
    {
        /* Debug mode command - value parsed but not used (debug mode functions removed) */
        vofa_last_param_id = vofa_rx_id;
        vofa_param_update = 1;
    }
    else if(vofa_rx_id > 0 && vofa_rx_id <= VOFA_PARAM_MAX)
    {
        value = vofa_parse_float(vofa_rx_buffer, vofa_rx_index);
        vofa_params[vofa_rx_id] = value;
        vofa_last_param_id = vofa_rx_id;
        vofa_param_update = 1;
    }

    if(vofa_rx_id == VOFA_PARAM_WHEEL_RADIUS)
    {
        wheel_radius_cm = vofa_params[vofa_rx_id];
    }

    vofa_rx_index = 0;
    vofa_rx_id = 0;
}

/* UART receive callback - called by interrupt */
void vofa_uart_rx_callback(uint8_t byte)
{
    static uint8_t state = 0;

    switch(state)
    {
        case 0: /* wait for # */
            if(byte == '#')
            {
                vofa_rx_index = 0;
                vofa_rx_id = 0;
                state = 1;
            }
            break;
        case 1: /* wait for optional P and ID */
            if(byte == 'P' || byte == 'p')
            {
                /* reference protocol prefix: #P1=12.34! */
            }
            else if(byte >= '0' && byte <= '9')
            {
                vofa_rx_id = vofa_rx_id * 10 + (byte - '0');
            }
            else if((byte == '=' || byte == ':') && vofa_rx_id > 0)
            {
                vofa_rx_index = 0;
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
        case 2: /* collect value */
            if(byte == '!' || vofa_rx_index >= 31)
            {
                vofa_store_received_value();
                state = (byte == '#') ? 1 : 0;
            }
            else if(byte == '#')
            {
                vofa_store_received_value();
                state = 1;
            }
            else
            {
                if(vofa_rx_index < sizeof(vofa_rx_buffer))
                {
                    vofa_rx_buffer[vofa_rx_index++] = byte;
                }
                else
                {
                    vofa_rx_index = 0;
                    vofa_rx_id = 0;
                    state = 0;
                }
            }
            break;
    }
}

uint8_t vofa_get_param_id(void)
{
    uint8_t id = vofa_param_update ? vofa_last_param_id : 0;
    vofa_param_update = 0;
    return id;
}

/* Peek at parameter ID without clearing update flag */
uint8_t vofa_peek_param_id(void)
{
    return vofa_param_update ? vofa_last_param_id : 0;
}

/* Check if there's a parameter update */
uint8_t vofa_has_param_update(void)
{
    return vofa_param_update;
}

float vofa_get_param_value(uint8_t id)
{
    if(id >= 1 && id <= VOFA_PARAM_MAX)
    {
        return vofa_params[id];
    }
    return 0.0f;
}