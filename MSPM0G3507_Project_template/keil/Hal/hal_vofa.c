/**
 * hal_vofa.c
 * VOFA+ protocol parser implementation
 * Protocol: #P<id>=<value>!
 */
#include "hal_vofa.h"
#include "hal_uart.h"
#include "hal_encode.h"
#include "seekfree_assistant.h"

/* VOFA parameter receive related */
static uint8_t vofa_rx_buffer[32];
static uint8_t vofa_rx_index = 0;
static uint8_t vofa_rx_id = 0;
static uint8_t vofa_last_param_id = 0;
static uint8_t vofa_param_update = 0;
static float vofa_params[VOFA_PARAM_MAX + 1] = {0};

/* Parameter mapping table entry */
typedef struct {
    uint8_t param_id;
    float *target;
} vofa_param_entry_t;

/* Helper macro for parameter table */
#define VOFA_PARAM_ENTRY(id, ptr) { id, ptr },

/* Parameter mapping table - maps VOFA param ID to target variable */
static const vofa_param_entry_t vofa_param_table[] = {
    VOFA_PARAM_MAP(VOFA_PARAM_ENTRY)
};

/* Count of parameter entries in mapping table */
#define VOFA_PARAM_TABLE_SIZE (sizeof(vofa_param_table) / sizeof(vofa_param_entry_t))

/* User callback for parameter updates */
static vofa_param_callback_t vofa_callback = 0;

void vofa_param_init(void)
{
    vofa_rx_index = 0;
    vofa_rx_id = 0;
    vofa_last_param_id = 0;
    vofa_param_update = 0;
}

void vofa_register_param_callback(vofa_param_callback_t callback)
{
    vofa_callback = callback;
}

/* Lightweight UART poll - reads from debug_uart_fifo and forwards to VOFA parser */
void vofa_uart_poll(void)
{
    uint8_t temp_buffer[32];
    uint32_t len = sizeof(temp_buffer);
    fifo_read_buffer(&debug_uart_fifo, temp_buffer, &len, FIFO_READ_AND_CLEAN);
    for (uint32_t i = 0; i < len; i++) {
        vofa_uart_rx_callback(temp_buffer[i]);
    }
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

    if(vofa_rx_id > 0 && vofa_rx_id <= VOFA_PARAM_MAX)
    {
        value = vofa_parse_float(vofa_rx_buffer, vofa_rx_index);
        vofa_params[vofa_rx_id] = value;
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

/* Apply a parameter value to all its mapped targets */
void vofa_apply_param(uint8_t id, float value)
{
    for(uint8_t i = 0; i < VOFA_PARAM_TABLE_SIZE; i++)
    {
        if(vofa_param_table[i].param_id == id && vofa_param_table[i].target != 0)
        {
            *(vofa_param_table[i].target) = value;
        }
    }
}

/* VOFA 10ms task - process received parameters */
void vofa_task_10ms(void)
{
    /* Process all pending parameter updates */
    while(vofa_has_param_update())
    {
        uint8_t id = vofa_get_param_id();
        float value = vofa_get_param_value(id);

        /* Apply value to all mapped targets */
        vofa_apply_param(id, value);

        /* Notify callback if registered */
        if(vofa_callback != 0)
        {
            vofa_callback(id, value);
        }
    }

    /* Call speed_control to run PID loop and drive motors */
    extern void speed_control(void);
    speed_control();

    /* Send feedback data to VOFA upper computer - JustFloat protocol */
    /* Every ~100ms: target speed, actual speed, PID params */
    static uint8_t vofa_send_counter = 0;
    if(++vofa_send_counter >= 1)  // ~10ms interval (every OS tick)
    {
        vofa_send_counter = 0;
        uint8_t i;
        float vofa_buf[5];

        vofa_buf[0] = motor_left.target;           // Target speed
        vofa_buf[1] = motor_get_left_speed();      // Actual left speed
        vofa_buf[2] = motor_left.speed_ctrl.kp;    // KP
        vofa_buf[3] = motor_left.speed_ctrl.ki;    // KI
        vofa_buf[4] = motor_left.speed_ctrl.kd;    // KD

        /* JustFloat format: 4 bytes per float + 0x00 0x00 0x80 0x7F footer */
        // uart1_send_char(0x00);
        for(i = 0; i < 5; i++)
        {
            uart1_send_char(((uint8_t*)&vofa_buf[i])[0]);
            uart1_send_char(((uint8_t*)&vofa_buf[i])[1]);
            uart1_send_char(((uint8_t*)&vofa_buf[i])[2]);
            uart1_send_char(((uint8_t*)&vofa_buf[i])[3]);
        }
        uart1_send_char(0x00);
        uart1_send_char(0x00);
        uart1_send_char(0x80);
        uart1_send_char(0x7F);
    }
}