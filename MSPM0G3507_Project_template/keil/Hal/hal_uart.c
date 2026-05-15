#include "ti_msp_dl_config.h"
#include "string.h"
#include "hal_uart.h"
#include "hal_led.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hal_encode.h"

/* UART1 receive callback pointer */
static uart_rx_callback_t uart1_rx_cb = NULL;

void uart1_rx_register(uart_rx_callback_t callback)
{
    uart1_rx_cb = callback;
}

void uart1_rx_feed(uint8_t byte)
{
    if(uart1_rx_cb != NULL)
    {
        uart1_rx_cb(byte);
    }
}

//PA10 PA11
void hal_uart1_Init(void)
{
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);

}
void UART0_send(unsigned char *buff,int len)
{
    while(len--)
    {
        DL_UART_Main_transmitDataBlocking(UART_1_INST, *buff);    // Send character via UART
        buff++;
    }
}
void UART1_send(const unsigned char *buff,int len)
{
	while(len--)
	{
	
        DL_UART_Main_transmitDataBlocking(UART_1_INST, *buff);    // Send character via UART
  	buff++;	
	}
}
void UART2_send(unsigned char *buff,int len)
{
	while(len--)
	{
	
	  DL_UART_Main_transmitDataBlocking(UART_2_INST, *buff);	// Send character string
  	buff++;	
	}
}

// Send a single character via UART
void uart0_send_char(char ch)
{
    while( DL_UART_isBusy(UART_1_INST) == true );
    DL_UART_Main_transmitData(UART_1_INST, ch);
}
// Send a single character via UART
void uart1_send_char(char ch)
{
    // Wait until UART is not busy, then send character
    while( DL_UART_isBusy(UART_1_INST) == true );
    // Send single character
    DL_UART_Main_transmitData(UART_1_INST, ch);
}
// Send a single character via UART
void uart2_send_char(char ch)
{
    // Wait until UART is not busy, then send character
    while( DL_UART_isBusy(UART_2_INST) == true );
    // Send single character
    DL_UART_Main_transmitData(UART_2_INST, ch);
}
// Usage: u3_printf("received %c", UART3_recevie);
//void u0_printf(const char *fmt, ...) {
//    int i;
//    int len;
    char buffer[128]; // Large enough buffer
//    va_list args;
//    va_start(args, fmt);
//    vsprintf(buffer, fmt, args); 
//    va_end(args);

//    len = strlen(buffer);

//		UART0_send((unsigned char*)buffer,len);
////    for (i = 0; i < len; i++) {
////			//uart0_send_char(buffer[i]);
////				// Wait until UART is not busy, then send character
//////				while( DL_UART_isBusy(UART_0_INST) == true );
//////				// Send single character
////				DL_UART_Main_transmitData(UART_0_INST, buffer[i]);			

////    }
//}
void u1_printf(const char *fmt, ...) {
    int i;
    int len;
    char buffer[128]; // Large enough buffer
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args); 
    va_end(args);

    len = strlen(buffer);

    for (i = 0; i < len; i++) {
			uart1_send_char(buffer[i]);
				// Wait until UART is not busy, then send character
//				while( DL_UART_isBusy(UART_0_INST) == true );
//				// Send single character
//				DL_UART_Main_transmitData(UART_0_INST, buffer[i]);			

    }
}

#if !defined(__MICROLIB)
// If not using Microlib, implement the following functions
#if (__ARMCLIB_VERSION <= 6000000)
// For AC5 compiler and below, define FILE struct
struct __FILE
{
        int handle;
};
#endif
FILE __stdout;
// Define _sys_exit() for semi-hosting mode
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
//	//u0_printf("abc");// Test UART0
//}


