/**
 * @file zuolan_usart.c
 * @brief 串口通信实现文件 (MSPM0G3507适配版)
 *
 * 提供:
 * - zuolan_printf(): 格式化UART发送
 * - hmi_send_buf():  原始数据块发送
 * - UART_display_INST_IRQHandler(): HMI模式接收ISR
 */
#include "zuolan_usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ---- 全局接收缓冲区 (ISR写入, 主循环读取) ---- */
volatile uint8_t hmi_rx_buf[HMI_RX_BUF_SIZE];
volatile uint8_t hmi_rx_idx = 0;
volatile uint8_t hmi_rx_ready = 0;

/* ---- 内部发送函数 ---- */

/** 阻塞发送单字节 */
static void hmi_tx_byte(uint8_t data)
{
    while ((HMI_UART->STAT & UART_STAT_TXFE_MASK) == 0);
    DL_UART_Main_transmitData(HMI_UART, data);
}

/* ---- 公共API ---- */

/** 发送原始字节块 */
void hmi_send_buf(const uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        hmi_tx_byte(buf[i]);
    }
}

/** 格式化串口打印 */
int zuolan_printf(const char *format, ...)
{
    char buffer[256];
    va_list arg;
    int len;

    va_start(arg, format);
    len = vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);

    for (int i = 0; i < len; i++) {
        hmi_tx_byte((uint8_t)buffer[i]);
    }
    return len;
}

/* ---- UART1 接收中断 (HMI模式) ---- */

/**
 * @brief UART_display (UART1) 接收中断处理
 *
 * HMI模式: 缓冲字节直到收到 \r\n, 然后设置 hmi_rx_ready 标志.
 * 帧格式: [data...]\r\n
 * 对于触控事件: 65 00 pageH pageL widget value 0D 0A
 */
void UART_display_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(HMI_UART)) {
    case DL_UART_IIDX_RX: {
        uint8_t ch = DL_UART_Main_receiveData(HMI_UART);

        /* Frame terminator: \r\n or standalone \n */
        if (ch == 0x0A) {
            if (hmi_rx_idx > 0 && hmi_rx_buf[hmi_rx_idx - 1] == 0x0D) {
                hmi_rx_idx--;  /* discard trailing \r */
            }
            if (hmi_rx_idx > 0) {
                /* snapshot length before ISR could append more bytes */
                hmi_rx_ready = 1;
            }
        } else if (hmi_rx_idx < HMI_RX_BUF_SIZE) {
            hmi_rx_buf[hmi_rx_idx++] = ch;
        } else {
            hmi_rx_idx = 0;  /* overflow reset */
        }
        break;
    }
    default:
        break;
    }
}
