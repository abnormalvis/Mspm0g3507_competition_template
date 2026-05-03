#include "ti_msp_dl_config.h"
#include "string.h"
#include "hal_uart.h"
#include "hal_led.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hal_encode.h"

//PA8 PA9
//void hal_uart0_Init(void)
//{
//    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
//    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

//}
//PA10 PA11
void hal_uart1_Init(void)
{
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);

}
//PB2 PB3
void hal_uart2_Init(void)
{
    NVIC_ClearPendingIRQ(UART_2_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_2_INST_INT_IRQN);

}
//void UART0_send(unsigned char *buff,int len)
//{
//	while(len--)
//	{
//	
//	  DL_UART_Main_transmitDataBlocking(UART_0_INST, *buff);	//发送字符串
//  	buff++;	
//	}
//}
void UART1_send(const unsigned char *buff,int len)
{
	while(len--)
	{
	
	  DL_UART_Main_transmitDataBlocking(UART_1_INST, *buff);	//发送字符串
  	buff++;	
	}
}
void UART2_send(unsigned char *buff,int len)
{
	while(len--)
	{
	
	  DL_UART_Main_transmitDataBlocking(UART_2_INST, *buff);	//发送字符串
  	buff++;	
	}
}

//串口发送单个字符
//void uart0_send_char(char ch)
//{
//    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
//    while( DL_UART_isBusy(UART_0_INST) == true );
//    //发送单个字符
//    DL_UART_Main_transmitData(UART_0_INST, ch);
//}
//串口发送单个字符
void uart1_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_1_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_1_INST, ch);
}
//串口发送单个字符
void uart2_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_2_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_2_INST, ch);
}
//使用案例：u3_printf("收到%c",UART3_recevie);
//void u0_printf(const char *fmt, ...) {
//    int i;
//    int len;
//    char buffer[128]; //足够容纳才可以，可以搞大点
//    va_list args;
//    va_start(args, fmt);
//    vsprintf(buffer, fmt, args); 
//    va_end(args);

//    len = strlen(buffer);

//		UART0_send((unsigned char*)buffer,len);
////    for (i = 0; i < len; i++) {
////			//uart0_send_char(buffer[i]);
////				//当串口0忙的时候等待，不忙的时候再发送传进来的字符
//////				while( DL_UART_isBusy(UART_0_INST) == true );
//////				//发送单个字符
////				DL_UART_Main_transmitData(UART_0_INST, buffer[i]);			

////    }
//}
void u1_printf(const char *fmt, ...) {
    int i;
    int len;
    char buffer[128]; //足够容纳才可以，可以搞大点
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args); 
    va_end(args);

    len = strlen(buffer);

    for (i = 0; i < len; i++) {
			uart1_send_char(buffer[i]);
				//当串口0忙的时候等待，不忙的时候再发送传进来的字符
//				while( DL_UART_isBusy(UART_0_INST) == true );
//				//发送单个字符
//				DL_UART_Main_transmitData(UART_0_INST, buffer[i]);			

    }
}

#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
        int handle;
};
#endif
FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
        x = x;
}
#endif

int fputc(int ch, FILE *stream)
{
        while( DL_UART_isBusy(UART_1_INST) == true );

        DL_UART_Main_transmitData(UART_1_INST, ch);

        return ch;
}


//void UsartProc(void)
//{
//	//u0_printf("%f,%f\n",smartcar_imu.left_motor_speed_cmps,2.f);
//	printf("%d\n",50);
//	//u0_printf("abc");//测试串口0
//}


