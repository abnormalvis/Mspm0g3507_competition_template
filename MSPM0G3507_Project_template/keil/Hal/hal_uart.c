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
        DL_UART_Main_transmitDataBlocking(UART_1_INST, *buff);    // �����ַ�����������
        buff++;
    }
}
void UART1_send(const unsigned char *buff,int len)
{
	while(len--)
	{
	
	  DL_UART_Main_transmitDataBlocking(UART_1_INST, *buff);	//�����ַ���
  	buff++;	
	}
}
void UART2_send(unsigned char *buff,int len)
{
	while(len--)
	{
	
	  DL_UART_Main_transmitDataBlocking(UART_2_INST, *buff);	//�����ַ���
  	buff++;	
	}
}

//���ڷ��͵����ַ�
void uart0_send_char(char ch)
{
    while( DL_UART_isBusy(UART_1_INST) == true );
    DL_UART_Main_transmitData(UART_1_INST, ch);
}
//���ڷ��͵����ַ�
void uart1_send_char(char ch)
{
    //������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
    while( DL_UART_isBusy(UART_1_INST) == true );
    //���͵����ַ�
    DL_UART_Main_transmitData(UART_1_INST, ch);
}
//���ڷ��͵����ַ�
void uart2_send_char(char ch)
{
    //������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
    while( DL_UART_isBusy(UART_2_INST) == true );
    //���͵����ַ�
    DL_UART_Main_transmitData(UART_2_INST, ch);
}
//ʹ�ð�����u3_printf("�յ�%c",UART3_recevie);
//void u0_printf(const char *fmt, ...) {
//    int i;
//    int len;
//    char buffer[128]; //�㹻���ɲſ��ԣ����Ը���
//    va_list args;
//    va_start(args, fmt);
//    vsprintf(buffer, fmt, args); 
//    va_end(args);

//    len = strlen(buffer);

//		UART0_send((unsigned char*)buffer,len);
////    for (i = 0; i < len; i++) {
////			//uart0_send_char(buffer[i]);
////				//������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
//////				while( DL_UART_isBusy(UART_0_INST) == true );
//////				//���͵����ַ�
////				DL_UART_Main_transmitData(UART_0_INST, buffer[i]);			

////    }
//}
void u1_printf(const char *fmt, ...) {
    int i;
    int len;
    char buffer[128]; //�㹻���ɲſ��ԣ����Ը���
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args); 
    va_end(args);

    len = strlen(buffer);

    for (i = 0; i < len; i++) {
			uart1_send_char(buffer[i]);
				//������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
//				while( DL_UART_isBusy(UART_0_INST) == true );
//				//���͵����ַ�
//				DL_UART_Main_transmitData(UART_0_INST, buffer[i]);			

    }
}

#if !defined(__MICROLIB)
//��ʹ��΢��Ļ�����Ҫ��������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
//�����������AC5  �Ͷ�����������ṹ��
struct __FILE
{
        int handle;
};
#endif
FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
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
//	//u0_printf("abc");//���Դ���0
//}


