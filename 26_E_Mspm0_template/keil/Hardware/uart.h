#ifndef __UART_H__
#define __UART_H__

#include "headfile.h"
extern volatile uint8_t uart_data;
//串口发送单个字符
void uart0_send_char(char ch);
void Uart0SendString(char* str);
void DataVision_Send();
#endif
