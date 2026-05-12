/*******************************************************************************
  * @作者      ： wangming
  * @wechat    :DeepCoderMing
  * @qq      ： 3201935299
  * @日期      ： 2025年05月01日
  * @版权声明  ： 仅供参考学习，未经允许禁止商用
********************************************************************************/
#include "user_interrupt.h"
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
//volatile uint8_t UART0_recevie;
int16_t error_openmv = 0,error_polarity = 0;  
//void UART_0_INST_IRQHandler(void)
//{
//		static int16_t date_openmv[4];	//摄像头端发送几个数据就是几，此处摄像头端发送6个数据
//		static int i = 0;	
//	
//		hal_BeepON();
//	
//    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
//        case DL_UART_MAIN_IIDX_RX:
//		
//				date_openmv[i++]=DL_UART_Main_receiveData(UART_0_INST); //接收数据
//				if(date_openmv[0]!=0xfe) i=0;             		  //判断帧头
//				if((i==4)&&(date_openmv[3]!=0xff)) i=0;    		//判断帧尾 
//				if(i==4)                           			//代表一组数据传输完毕
//				{
//					 i=0;
//					
//					if(date_openmv[1] == 0)
//					{
//						error_polarity = - 1;
//						//hal_BeepOFF();
//					}
//					else if(date_openmv[1] == 1)
//					{
//						error_polarity =  1;
//					//	hal_BeepOFF();
//					}
//					else if(date_openmv[1] == 2)
//					{
//						//error_polarity =  0;
//						hal_BeepON();
//						Flag.Start_Car = 0;
//					}
//					
//					
//					error_openmv = date_openmv[2] * error_polarity;
//					//error_openmv = Xianfu_int(error_openmv,60);
//		
//				}					
//            DL_GPIO_togglePins(RGB_GPIO,
//                RGB_Red_Pin);
//				
//            break;
//        default:
//            break;
//    }
//}

/*zigbee*/
uint8_t  UART1_recevie;
uint8_t   debug_uart_data;

void UART_1_INST_IRQHandler(void) //逐飞串口调试助手
{

    switch (DL_UART_Main_getPendingInterrupt(UART_1_INST)) {
        case DL_UART_MAIN_IIDX_RX:
//            DL_GPIO_togglePins(RGB_GPIO,
//                RGB_Red_Pin);
           UART1_recevie = DL_UART_Main_receiveData(UART_1_INST); 
        fifo_write_buffer(&debug_uart_fifo, &UART1_recevie, 1);               // 存入 FIFO	
				
					//DL_UART_Main_transmitData(UART_1_INST, UART1_recevie);
//				date_openmv[i++]=DL_UART_Main_receiveData(UART_1_INST)&0xff; //接收数据
//		//data =  USART_ReceiveData(USART3)&0xff;
//				if(date_openmv[0]!=0xfe) i=0;             		  //判断帧头
//				if((i==6)&&(date_openmv[5]!=0xff)) i=0;    		//判断帧尾
//				if(i==6)                           			//代表一组数据传输完毕
//				{
//					 i=0;
//					 //Usart3_Send_function(10,data0);        //写你要操作的事情
//		//			l_error_data = date_openmv[1];
//		//			r_error_data = date_openmv[2];
//		//			ul_error_data = date_openmv[3];
//		//			ur_error_data = date_openmv[4];			
//				}	
            break;
        default:
            break;
    }




}

uint16_t Num1 = 0,Num2 = 0;
static uint16_t beep_on_cnt = 0,task_start_cnt = 0;
void TIMG6_IRQHandler(void)//5ms 循迹
{
		gray_8data_read();
		// Num1++;
			if(Flag.beep_on == 1)//蜂鸣器鸣响1秒
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

		// 	if(Flag.task_start == 1) //用中断计时切换任务状态
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



//volatile uint8_t UART2_recevie;
//void UART_2_INST_IRQHandler(void)
//{
//    switch (DL_UART_Main_getPendingInterrupt(UART_2_INST)) {
//        case DL_UART_MAIN_IIDX_RX:
//					
//            DL_GPIO_togglePins(RGB_GPIO,
//                RGB_Red_Pin);
//				
//            UART2_recevie = DL_UART_Main_receiveData(UART_2_INST);
//				
//            uart2_send_char(UART2_recevie);
//            break;
//        default:
//            break;
//    }
//}
