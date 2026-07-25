#include "lora.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_uart.h>

/* ====================================================================
 * RX FIFO â€? same zf_common_fifo pattern as vofa_rx_fifo
 * ==================================================================== */
static uint8_t lora_rx_fifo_buf[LORA_FIFO_SIZE];
fifo_obj_struct lora_rx_fifo;

/* ====================================================================
 * Parser state machine
 * ==================================================================== */
typedef enum {
    LORA_S_SYNC_AA = 0,
    LORA_S_SYNC_55,
    LORA_S_READ_CMD,
    LORA_S_READ_LEN,
    LORA_S_READ_DATA,
    LORA_S_READ_XOR,
    LORA_S_READ_TAIL
} lora_state_t;

static lora_state_t    s_state     = LORA_S_SYNC_AA;
static uint8_t         s_cmd       = 0;
static uint8_t         s_len       = 0;
static uint8_t         s_data_idx  = 0;
static uint8_t         s_data_buf[LORA_PAYLOAD_MAX];
static uint8_t         s_xor_acc   = 0;
static uint16_t        s_stale_ticks = 0;
static lora_packet_cb_t s_on_packet = 0;

#define LORA_STALE_TIMEOUT  500u   /* max main-loop iterations before auto-reset */

/* ---- helper: reset parser to initial state ---- */
static void parser_reset(void)
{
    s_state       = LORA_S_SYNC_AA;
    s_cmd         = 0;
    s_len         = 0;
    s_data_idx    = 0;
    s_xor_acc     = 0;
    s_stale_ticks = 0;
}

/* ---- per-byte parser: feed one byte, fire callback on complete frame ---- */
static void lora_parse_byte(uint8_t byte)
{
    switch (s_state) {

    case LORA_S_SYNC_AA:
        if (byte == LORA_HEADER0) {
            s_state = LORA_S_SYNC_55;
            s_stale_ticks = 0;
        }
        /* else: stay in SYNC_AA, byte is noise */
        break;

    case LORA_S_SYNC_55:
        if (byte == LORA_HEADER1) {
            s_state = LORA_S_READ_CMD;
        } else if (byte == LORA_HEADER0) {
            /* back-to-back 0xAA: restart, don't lose this sync */
            /* s_state stays SYNC_55, wait for 0x55 after this */
        } else {
            parser_reset();
        }
        break;

    case LORA_S_READ_CMD:
        s_cmd     = byte;
        s_xor_acc = byte;    /* XOR starts with CMD */
        s_state   = LORA_S_READ_LEN;
        break;

    case LORA_S_READ_LEN:
        s_len      = byte;
        s_xor_acc ^= byte;   /* XOR includes LEN */
        s_data_idx = 0;
        if (s_len == 0) {
            /* empty payload â€? next byte is XOR */
            s_state = LORA_S_READ_XOR;
        } else {
            s_state = LORA_S_READ_DATA;
        }
        break;

    case LORA_S_READ_DATA:
        /* collect payload bytes; transition to XOR state when all data received */
        s_data_buf[s_data_idx] = byte;
        s_xor_acc ^= byte;       /* XOR includes each payload byte */
        s_data_idx++;
        if (s_data_idx >= s_len) {
            s_state = LORA_S_READ_XOR;
        }
        break;

    case LORA_S_READ_XOR:
        /* validate XOR checksum */
        if (byte == s_xor_acc) {
            s_state = LORA_S_READ_TAIL;
        } else {
            /* checksum mismatch â€? discard frame */
            parser_reset();
        }
        break;

    case LORA_S_READ_TAIL:
        if (byte == LORA_FRAME_TAIL) {
            /* complete valid frame received */
            if (s_on_packet) {
                s_on_packet(s_cmd, s_data_buf, s_len);
            }
        }
        /* invalid tail or valid tail â€? either way, reset for next frame */
        parser_reset();
        break;

    default:
        parser_reset();
        break;
    }
}

/* ====================================================================
 * Public API
 * ==================================================================== */

void lora_init(void)
{
    fifo_init(&lora_rx_fifo, FIFO_DATA_8BIT, lora_rx_fifo_buf, LORA_FIFO_SIZE);
    parser_reset();
    s_on_packet = 0;
}

void lora_set_on_packet(lora_packet_cb_t cb)
{
    s_on_packet = cb;
}

void lora_rx_drain(void)
{
    uint32_t pending = fifo_used(&lora_rx_fifo);
    while (pending > 0) {
        uint8_t byte;
        if (fifo_read_element(&lora_rx_fifo, &byte, FIFO_READ_AND_CLEAN) == FIFO_SUCCESS) {
            lora_parse_byte(byte);
        }
        pending--;
    }

    /* timeout guard: if parser is stuck mid-frame for too many iterations,
     * reset it so a partial frame doesn't block future valid frames */
    if (s_state != LORA_S_SYNC_AA) {
        s_stale_ticks++;
        if (s_stale_ticks >= LORA_STALE_TIMEOUT) {
            parser_reset();
        }
    }
}

/* ---- blocking single-byte TX on UART2 ---- */
void lora_send_byte(uint8_t data)
{
    while ((UART_wired_INST->STAT & UART_STAT_TXFE_MASK) == 0);
    DL_UART_Main_transmitData(UART_wired_INST, data);
}

/* ---- build and send a complete frame ---- */
void lora_send_packet(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    uint8_t xor_acc;
    uint8_t i;

    if (len > LORA_PAYLOAD_MAX) {
        return;
    }

    /* header */
    lora_send_byte(LORA_HEADER0);
    lora_send_byte(LORA_HEADER1);

    /* CMD */
    lora_send_byte(cmd);

    /* LEN */
    lora_send_byte(len);

    /* XOR: CMD ^ LEN ^ ... */
    xor_acc = cmd ^ len;

    /* PAYLOAD */
    if (len > 0 && data != 0) {
        for (i = 0; i < len; i++) {
            lora_send_byte(data[i]);
            xor_acc ^= data[i];
        }
    }

    /* XOR checksum */
    lora_send_byte(xor_acc);

    /* tail */
    lora_send_byte(LORA_FRAME_TAIL);
}
