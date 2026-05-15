/*******************************************************************************
  * @author      : wangming
  * @wechat    :DeepCoderMing
  * @qq      : 3201935299
  * @date      : 2025-05-01
  * @copyright  : For reference only, commercial use prohibited
********************************************************************************/
#include "hal_jy62.h"
#include <string.h>
#include <stdio.h>
#include "hal_uart.h"
#include "ti/driverlib/dl_gpio.h"
#include "hal_led.h"

static int32_t n = 0 ;
int32_t state = 0;
static uint8_t buffer[11];
imu_data_t acceleration, gyroscope, angle; 
int32_t k = 0;
char analysis_flag = 0;

void imu_analysis(void)
{
    static float last_angle_z = 0, angle_z = 0;
	
    switch (buffer[1]) {
        case 0x51:
            acceleration.x = (float )((int16_t)(buffer[2]|(buffer[3]<<8)))/32768.f*16;
            acceleration.y = (float )((int16_t)(buffer[4]|(buffer[5]<<8)))/32768.f*16;
            acceleration.z = (float )((int16_t)(buffer[6]|(buffer[7]<<8)))/32768.f*16;
            break;
        case 0x52:
            gyroscope.x = (float )((int16_t)(buffer[2]|(buffer[3]<<8)))/32768.f*2000;
            gyroscope.y = (float )((int16_t)(buffer[4]|(buffer[5]<<8)))/32768.f*2000;
            gyroscope.z = (float )((int16_t)(buffer[6]|(buffer[7]<<8)))/32768.f*2000;
            break;
        case 0x53:
            angle.x = (float )((int16_t)(buffer[2]|(buffer[3]<<8)))/32768.f*180;
            angle.y = (float )((int16_t)(buffer[4]|(buffer[5]<<8)))/32768.f*180;
            angle_z = (float )((int16_t)(buffer[6]|(buffer[7]<<8)))/32768.f*180;
            break;
    }
    if (last_angle_z < -90 && angle_z > 90)
    {
		//	k = -1;
			 k--;
		}
    if (last_angle_z > 90 && angle_z < -90)
		{ 
		//	k = 1;
			 k++;
		}
	
    last_angle_z = angle_z;

    angle.z = angle_z;
		
		
}

static uint8_t get_verify_code()
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 10; ++i) {
        sum += buffer[i];
    }
    return sum&0xff;
}

//UART2 receive interrupt
////volatile uint8_t UART2_recevie;
////void UART_2_INST_IRQHandler(void)
////{
////	uint32_t data;
////    switch (DL_UART_Main_getPendingInterrupt(UART_2_INST)) {
////        case DL_UART_MAIN_IIDX_RX:
////					
//////            DL_GPIO_togglePins(RGB_GPIO,
//////                RGB_Red_Pin);
////				
//////            UART2_recevie = DL_UART_Main_receiveData(UART_2_INST);
//////				
//////            uart2_send_char(UART2_recevie);

////				
////            data = DL_UART_Main_receiveData(UART_2_INST)&0xff;
////		 if (state == ERROR){
////			n = 0;
////		 }

////		buffer[n] = (uint8_t)data;
////		if (n == 0){
////			if (buffer[0] == 0x55){
////				state = DOING;
////			} else{
////				state = ERROR;
////			}
////		} else if (n == 10){
////			if (buffer[10] == get_verify_code()){
////				state = SUCCESS;
////			} else{
////				state = ERROR;
////			}
////		}
////		n += 1;
////		if (state == SUCCESS){
//////			printf("OK\n");
////			uart2_send_char('h');
////			imu_analysis();
////			state = ERROR;
////		}
////											break;
////									default:
////											break;
////							}
////}


