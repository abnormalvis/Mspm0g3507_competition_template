/*******************************************************************************
  * @author     : wangming
  * @wechat     :DeepCoderMing
  * @qq         : 3201935299
  * @date       : 2025�??????05�??????01�??????
  * @copyright  : 仅供参考�?�习，�?��?�商业传�??????
********************************************************************************/
#include "hal_vofa.h"
#include "hal_uart.h"
#include "hal_encode.h"

static uint8_t vofa_buffer[VOFA_FLOAT_NUM*4+4];
static uint16_t cnt = 0;

/* VOFA parameter receive related */
static uint8_t vofa_rx_buffer[32];
static uint8_t vofa_rx_index = 0;
static uint8_t vofa_rx_id = 0;
static uint8_t vofa_last_param_id = 0;
static uint8_t vofa_param_update = 0;
static float vofa_params[VOFA_PARAM_MAX + 1] = {0};

/* Debug mode: only send feedback when enabled */
static uint8_t vofa_debug_enabled = 0;
static uint8_t vofa_send_counter = 0;
static uint8_t vofa_send_mode = 0;  /* 0=off, 1=speed, 2=angle, 3=seek */

/* Pending PID parameters to apply - per-parameter flags to avoid clearing unset values */
static float vofa_pending_kp = 0.0f;
static float vofa_pending_ki = 0.0f;
static uint8_t vofa_kp_pending = 0;
static uint8_t vofa_ki_pending = 0;

/* Set VOFA debug mode:
   mode: 0=off, 1=speed loop, 2=angle loop, 3=seek loop */
void vofa_set_debug_mode(uint8_t mode)
{
    vofa_send_mode = mode;
    vofa_debug_enabled = (mode > 0) ? 1 : 0;
    vofa_send_counter = 0;
}

/* Get current debug mode */
uint8_t vofa_get_debug_mode(void)
{
    return vofa_send_mode;
}

/* Enable/disable VOFA debug output */
void vofa_debug_enable(uint8_t enable)
{
    vofa_debug_enabled = enable;
    vofa_send_counter = 0;
}

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
    vofa_last_param_id = 0;
    vofa_param_update = 0;
    vofa_debug_enabled = 0;  /* Disabled by default */
    vofa_send_mode = 0;
    vofa_send_counter = 0;
    vofa_pending_kp = 0.0f;
    vofa_pending_ki = 0.0f;
    vofa_kp_pending = 0;
    vofa_ki_pending = 0;
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
    if(vofa_rx_id > 0 && vofa_rx_id <= VOFA_PARAM_MAX)
    {
        vofa_params[vofa_rx_id] = vofa_parse_float(vofa_rx_buffer, vofa_rx_index);
        vofa_last_param_id = vofa_rx_id;
        vofa_param_update = 1;

        /* Mark pending if KP or KI */
        if(vofa_rx_id == VOFA_PARAM_SPEED_KP)
        {
            vofa_pending_kp = vofa_params[vofa_rx_id];
            vofa_kp_pending = 1;
        }
        else if(vofa_rx_id == VOFA_PARAM_SPEED_KI)
        {
            vofa_pending_ki = vofa_params[vofa_rx_id];
            vofa_ki_pending = 1;
        }
    }

    if(vofa_rx_id == VOFA_PARAM_WHEEL_RADIUS)
    {
        wheel_radius_cm = vofa_params[vofa_rx_id];
    }

    if(vofa_rx_id == 99)
    {
        uint8_t mode = (uint8_t)vofa_parse_float(vofa_rx_buffer, vofa_rx_index);
        vofa_set_debug_mode(mode);
        vofa_last_param_id = vofa_rx_id;
        vofa_param_update = 1;
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

/* Send speed loop data to VOFA - 5 channels: target, actual_L, actual_R, kp, ki */
void vofa_send_speed_feedback(float target_l, float actual_l, float actual_r, float kp, float ki)
{
    if(!vofa_debug_enabled || vofa_send_mode != 1) return;

    vofa_add_data(target_l);
    vofa_add_data(actual_l);
    vofa_add_data(actual_r);
    vofa_add_data(kp);
    vofa_add_data(ki);
    vofa_send();
}

/* Send angle loop data to VOFA */
void vofa_send_angle_feedback(float target, float actual)
{
    if(!vofa_debug_enabled || vofa_send_mode != 2) return;

    vofa_add_data(target);
    vofa_add_data(actual);
    vofa_send();
}

/* Send seektrack loop data to VOFA */
void vofa_send_seek_feedback(float target, float actual)
{
    if(!vofa_debug_enabled || vofa_send_mode != 3) return;

    vofa_add_data(target);
    vofa_add_data(actual);
    vofa_send();
}

/* Get speed PID parameters - returns bitmask: bit0=KP updated, bit1=KI updated, 0=nothing */
uint8_t vofa_get_speed_pid(float *kp, float *ki)
{
    uint8_t result = 0;

    if(vofa_kp_pending)
    {
        if(kp) *kp = vofa_pending_kp;
        vofa_pending_kp = 0.0f;
        vofa_kp_pending = 0;
        result |= 1;
    }

    if(vofa_ki_pending)
    {
        if(ki) *ki = vofa_pending_ki;
        vofa_pending_ki = 0.0f;
        vofa_ki_pending = 0;
        result |= 2;
    }

    return result;
}

/* Check if new speed target received */
float vofa_get_speed_target(void)
{
    if(vofa_param_update && vofa_last_param_id == VOFA_PARAM_SPEED_TARGET)
    {
        float target = vofa_params[VOFA_PARAM_SPEED_TARGET];
        vofa_param_update = 0;  /* Clear flag after reading */
        return target;
    }
    return 0.0f;
}

/* Get latest speed target without clearing - for continuous display */
float vofa_peek_speed_target(void)
{
    if(vofa_last_param_id == VOFA_PARAM_SPEED_TARGET)
    {
        return vofa_params[VOFA_PARAM_SPEED_TARGET];
    }
    return 0.0f;
}