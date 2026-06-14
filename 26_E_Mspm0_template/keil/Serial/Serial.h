#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "ti_msp_dl_config.h"
#include "vofa.h"
#include "K230.h"
#include "zf_common_fifo.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void uart_debug_send_byte(uint8_t data);
void uart1_send_char(char ch);
void uart1_send_byte(uint8_t data);
void uart3_send_byte(uint8_t data);
void usart_debug_SendCmd(volatile uint8_t *cmd, uint8_t len);
void uart1_send_string(char* str);
void usart3_SendCmd(volatile uint8_t *cmd, uint8_t len);
void uart3_send_string(char *str);
int fputc(int ch, FILE *stream);
int fputs(const char* restrict s,FILE* restrict stream);

/* VOFA RX FIFO (ISR writes, main loop reads) */
extern fifo_obj_struct vofa_rx_fifo;
void vofa_rx_fifo_init(void);

#endif
