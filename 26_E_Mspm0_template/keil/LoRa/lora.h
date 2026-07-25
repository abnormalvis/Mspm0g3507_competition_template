#ifndef __LORA_H__
#define __LORA_H__

#include <stdint.h>
#include "zf_common_fifo.h"

/* ---- frame protocol constants ---- */
#define LORA_HEADER0       0xAAu
#define LORA_HEADER1       0x55u
#define LORA_FRAME_TAIL    0xEDu

#define LORA_PAYLOAD_MAX   255u
#define LORA_FIFO_SIZE     256u
#define LORA_FRAME_OVERHEAD 6u   /* HEAD0 + HEAD1 + CMD + LEN + XOR + TAIL */

/* ---- callback type: invoked from main loop when a valid packet arrives ---- */
typedef void (*lora_packet_cb_t)(uint8_t cmd, const uint8_t *data, uint8_t len);

/* ---- RX FIFO (ISR writes, main loop reads) ---- */
extern fifo_obj_struct lora_rx_fifo;

/* ---- API ---- */
void lora_init(void);
void lora_set_on_packet(lora_packet_cb_t cb);

/* Drain FIFO and feed bytes into parser. Call from main loop. */
void lora_rx_drain(void);

/* Build and send a complete frame over UART2 (blocking TX).
 * cmd  : packet type identifier
 * data : payload buffer (can be NULL if len == 0)
 * len  : payload length in bytes (0..LORA_PAYLOAD_MAX) */
void lora_send_packet(uint8_t cmd, const uint8_t *data, uint8_t len);

/* Send a single raw byte over UART2 (for AT-command configuration). */
void lora_send_byte(uint8_t data);

#endif /* __LORA_H__ */
