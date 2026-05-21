#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "ti_msp_dl_config.h"
#include "vofa.h"
#include "K230.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void uart_debug_send_byte(uint8_t data);
void uart1_send_char(char ch);
void uart1_send_byte(uint8_t data);
void uart2_send_byte(uint8_t data);
void usart_debug_SendCmd(volatile uint8_t *cmd, uint8_t len);
void uart1_send_string(char* str);
void usart2_SendCmd(volatile uint8_t *cmd, uint8_t len);
void uart2_send_string(char *str);
int fputc(int ch, FILE *stream);
int fputs(const char* restrict s,FILE* restrict stream);

#endif
