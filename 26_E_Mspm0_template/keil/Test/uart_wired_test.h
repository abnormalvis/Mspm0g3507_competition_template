/**
 * @file uart_wired_test.h
 * @brief UART_wired wireless serial loopback test
 *
 * Provides a raw byte echo over UART_wired (UART2 / PA21-PB16) to verify
 * the wireless serial link end-to-end.  The ISR writes every received byte
 * into a dedicated FIFO; the main loop drains it and echoes each byte back.
 *
 * This FIFO is separate from lora_rx_fifo so the LoRa protocol parser is
 * unaffected.
 */
#ifndef __UART_WIRED_TEST_H__
#define __UART_WIRED_TEST_H__

#include "ti_msp_dl_config.h"
#include "zf_common_fifo.h"

/* FIFO shared with ISR in Serial.c */
extern fifo_obj_struct uart_wired_test_fifo;

void UART_WiredTest_Init(void);
void UART_WiredTest_Run(void);

/* 1 = send "TICK\r\n" every 1s on UART_wired (M0->PC aliveness) */
#define UART_WIRED_HEARTBEAT 0

/* 1 = hex-dump received bytes via UART0 (sniff only, no echo on UART_wired) */
#define UART_WIRED_SNIFFER   0

/* 1 = echo every received byte back on UART_wired (raw loopback test) */
#define UART_WIRED_ECHO      0

#endif /* __UART_WIRED_TEST_H__ */
