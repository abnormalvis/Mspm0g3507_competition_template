/**
 * @file uart_wired_test.c
 * @brief UART_wired wireless serial link diagnostic
 *
 * Two test modes help isolate the direction that's broken:
 *   1. Heartbeat (UART_WIRED_HEARTBEAT=1):
 *      M0 sends "TICK\r\n" on UART_wired every 1s.
 *      PC serial assistant should see "TICK\r\n" (as hex: 54 49 43 4B 0D 0A).
 *      If you DON'T see it, the problem is M0 TX or the wireless module TX path.
 *
 *   2. Sniffer (UART_WIRED_SNIFFER=1):
 *      Every byte received on UART_wired RX is printed in hex via UART0 debug.
 *      NOT echoed back on UART_wired (avoids wireless module TX→RX loopback).
 *      Send known hex from PC; watch UART0 output. If you see the right bytes,
 *      RX path is OK. If bytes are garbled, check baud rate / module config.
 *
 * Both modes can run simultaneously.
 */

#include "uart_wired_test.h"
#include "lora.h"          /* lora_send_byte() for TX over UART2 */
#include "Serial.h"        /* uart_debug_send_byte() for hex dump via UART0 */
#include <stdint.h>

/* ---- dedicated FIFO (256 bytes to match lora_rx_fifo) ---- */
#define TEST_FIFO_SIZE  256u
static uint8_t test_fifo_buf[TEST_FIFO_SIZE];
fifo_obj_struct uart_wired_test_fifo;

/* ---- heartbeat state ---- */
static uint32_t s_last_hb_ms = 0;

/* ---- hex nibble → ASCII ---- */
static uint8_t hex_char(uint8_t nib)
{
    return (nib < 10) ? ('0' + nib) : ('A' + nib - 10);
}

void UART_WiredTest_Init(void)
{
    fifo_init(&uart_wired_test_fifo, FIFO_DATA_8BIT,
              test_fifo_buf, TEST_FIFO_SIZE);
    s_last_hb_ms = 0;
}

/**
 * Drain the test FIFO and dump every received byte as hex over UART0.
 * Runs in main loop.  Bytes are NOT echoed back on UART_wired to avoid
 * wireless module TX→RX loopback (some modules echo their own TX locally).
 */
void UART_WiredTest_Run(void)
{
    uint32_t pending = fifo_used(&uart_wired_test_fifo);

#if UART_WIRED_SNIFFER
    if (pending > 0) {
        /* header: "<N>" where N = byte count */
        uart_debug_send_byte('<');
        {
            uint8_t d100 = pending / 100;
            uint8_t d10  = (pending / 10) % 10;
            uint8_t d1   = pending % 10;
            if (d100) uart_debug_send_byte('0' + d100);
            if (d100 || d10) uart_debug_send_byte('0' + d10);
            uart_debug_send_byte('0' + d1);
        }
        uart_debug_send_byte('>');
        uart_debug_send_byte(' ');

        while (pending > 0) {
            uint8_t byte;
            if (fifo_read_element(&uart_wired_test_fifo, &byte,
                    FIFO_READ_AND_CLEAN) == FIFO_SUCCESS) {
                /* print as "XX " via UART0 */
                uart_debug_send_byte(hex_char(byte >> 4));
                uart_debug_send_byte(hex_char(byte & 0x0F));
                uart_debug_send_byte(' ');
            }
            pending--;
        }
        uart_debug_send_byte('\r');
        uart_debug_send_byte('\n');
    }
#else
    /* Sniffer off: echo back on UART_wired or just drain. */
    while (pending > 0) {
        uint8_t byte;
        if (fifo_read_element(&uart_wired_test_fifo, &byte,
                FIFO_READ_AND_CLEAN) == FIFO_SUCCESS) {
#if UART_WIRED_ECHO
            lora_send_byte(byte);   /* raw echo back via UART_wired */
#endif
        }
        pending--;
    }
#endif

#if UART_WIRED_HEARTBEAT
    /* Send "TICK\r\n" over UART_wired every 1000ms.
     * Requires sys_tick_ms from main.c (1ms tick in TIMER_0 ISR). */
    {
        extern volatile uint32_t sys_tick_ms;
        if (sys_tick_ms - s_last_hb_ms >= 1000) {
            s_last_hb_ms = sys_tick_ms;
            lora_send_byte('T');
            lora_send_byte('I');
            lora_send_byte('C');
            lora_send_byte('K');
            lora_send_byte('\r');
            lora_send_byte('\n');
        }
    }
#endif
}
