#include "uart.h"

/******************串口*************************************/
volatile uint8_t uart_data;
//串口发送单个字符
void uart0_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_0_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_0_INST, ch);
}
//串口发送字符串
void Uart0SendString(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0 && str!= 0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart0_send_char(*str++);
    }
}


//串口1变量：调试/调参/总线舵机接收当前角度
uint8_t head1 = 0x03;
uint8_t head2 = 0xfc;
uint8_t tail1 = 0xfc;
uint8_t tail2 = 0x03;
/**************************上位机调参******************************/
void DataVision_Send()
{
	uint8_t target = (uint8_t)motorL.target;
	uint8_t now = (uint8_t)motorL.now;
	uart0_send_char(head1);
	uart0_send_char(head2);
	
	uart0_send_char(target);
	uart0_send_char(now);
	
	uart0_send_char(tail1);
	uart0_send_char(tail2);
}