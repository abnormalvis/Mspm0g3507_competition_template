/*******************************************************************************
  * @����      �� wangming
  * @wechat    :DeepCoderMing
  * @qq      �� 3201935299
  * @����      �� 2025��05��01��
  * @��Ȩ����  �� �����ο�ѧϰ��δ��������ֹ����
********************************************************************************/
#include "user_interrupt.h"
#include "ti_msp_dl_config.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_gray.h"
#include "zf_common_fifo.h" 
#include "seekfree_assistant.h"
#include "mt_test.h"
#include  "mt_flag.h"
#include "hal_beep.h"
#include "hal_math.h"
#include "hal_encode.h"
#include "imu_filter.h"                  // Device header
#include "hal_vofa.h"
//volatile uint8_t UART0_recevie;
int16_t error_openmv = 0,error_polarity = 0;  
/*zigbee*/
uint8_t  UART1_recevie;
uint8_t   debug_uart_data;

void UART0_IRQHandler(void)
{
	if(DL_UART_Main_getPendingInterrupt(UART0) != 0)
	{
		UART1_recevie = DL_UART_Main_receiveData(UART0);
		fifo_write_buffer(&debug_uart_fifo, &UART1_recevie, 1);               // ���� FIFO	
		/* VOFA parsing moved to seekfree_assistant_data_analysis() main loop,
		   to avoid double-feeding static state machine from ISR context */

//            DL_GPIO_togglePins(RGB_GPIO,
//                RGB_Red_Pin);
//				//DL_UART_Main_transmitData(UART0, UART1_recevie);
//				date_openmv[i++]=DL_UART_Main_receiveData(UART0)&0xff; //��������
//		//data =  USART_ReceiveData(USART3)&0xff;
//				if(date_openmv[0]!=0xfe) i=0;              	  //�ж�֡ͷ
//				if((i==6)&&(date_openmv[5]!=0xff)) i=0;			//�ж�֡β
//				if(i==6)                            			//����һ�����ݴ������?
//				{
//					 i=0;
//					 //Usart3_Send_function(10,data0);        //д��Ҫ����������
//		//			l_error_data = date_openmv[1];
//		//			r_error_data = date_openmv[2];
//		//			ul_error_data = date_openmv[3];
//		//			ur_error_data = date_openmv[4];			
		}	
}

uint16_t Num1 = 0,Num2 = 0;
static uint16_t beep_on_cnt = 0,task_start_cnt = 0;
static uint16_t vofa_send_cnt = 0;
volatile uint8_t gray_sample_req = 0;
void TIMG6_IRQHandler(void)//5ms ѭ��
{
		gray_sample_req = 1;
		// Num1++;
			if(Flag.beep_on == 1)//����������1��
			{
				
				beep_on_cnt++;
				hal_BeepON(); 
				RGB_Blue_open();
				if(beep_on_cnt >100)
				{

					beep_on_cnt = 0;
					Flag.beep_on = 0;
					hal_BeepOFF(); 
					RGB_Blue_close();
				}
			}	
		// if(Num1%200 == 0)
		// {
		// 	Num2++;
		// 	Num1=0;
		// 	//DL_GPIO_togglePins(RGB_GPIO,RGB_Red_Pin);

		// 	if(Flag.task_start == 1) //���жϼ�ʱ�л�����״̬
		// 	{
		// 			task_start_cnt++;
		// 		if(task_start_cnt > 1)
		// 		{
		// 			Flag.task_start = 2;		
		// 			task_start_cnt = 0;
		// 		}
			
		// 	}
		// }	
	
}




