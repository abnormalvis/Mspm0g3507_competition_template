/**
 * @file zuolan_usart.h
 * @brief 串口通信基础功能头文件 (MSPM0G3507适配版)
 */
#ifndef __ZUOLAN_USART_H__
#define __ZUOLAN_USART_H__

#include "hmi_config.h"
#include <stdint.h>

/* 接收缓冲区和状态 (ISR与主循环共享) */
extern volatile uint8_t hmi_rx_buf[HMI_RX_BUF_SIZE];
extern volatile uint8_t hmi_rx_idx;
extern volatile uint8_t hmi_rx_ready;

/* 格式化串口发送 */
int zuolan_printf(const char *format, ...);

/* 发送原始字节块 */
void hmi_send_buf(const uint8_t *buf, uint16_t len);

#endif // __ZUOLAN_USART_H__
