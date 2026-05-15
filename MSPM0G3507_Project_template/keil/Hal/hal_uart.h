#ifndef _HAL_UART_H
#define _HAL_UART_H
#include "ti_msp_dl_config.h"

#include <stdio.h>



void hal_uart0_Init(void);
void hal_uart1_Init(void);
void hal_uart2_Init(void);

void UART0_send(unsigned char *buff,int len);
void UART1_send(const unsigned char *buff,int len);
void UART2_send(unsigned char *buff,int len);

void uart0_send_char(char ch);
void uart1_send_char(char ch);
void uart2_send_char(char ch);

void u0_printf(const char *fmt, ...);
void u1_printf(const char *fmt, ...);
void UsartProc(void);

/* UART receive callback type */
typedef void (*uart_rx_callback_t)(uint8_t byte);
void uart1_rx_register(uart_rx_callback_t callback);
void uart1_rx_feed(uint8_t byte);

#endif

