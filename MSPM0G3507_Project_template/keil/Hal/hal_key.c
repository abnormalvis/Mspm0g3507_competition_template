#include "ti/driverlib/dl_gpio.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "drv_oled.h"
#include "hal_uart.h"
#include "mt_test.h"
#include "hal_encode.h"
#include "user_interrupt.h"
#include "string.h"
#include "hal_vofa.h"
#include "seekfree_uart.h"
#include "2024DS_Duty.h"
#include "app.h"
#include "mt_flag.h"

#include "zf_device_imu660ra.h"

KeyEvent_CallBack_t KeyScanCBS;
 

static unsigned char hal_getKey1Sta(void);
static unsigned char hal_getKey2Sta(void);
static unsigned char hal_getKey3Sta(void);
static unsigned char hal_getKey4Sta(void);
static unsigned char hal_getKey5Sta(void);
	
unsigned char (*getKeysState[KEYNUM])() = {  hal_getKey1Sta,hal_getKey2Sta,hal_getKey3Sta,hal_getKey4Sta,hal_getKey5Sta
								};
								
  

unsigned char KeyStep[KEYNUM];                              // Key scan step state
unsigned short KeyScanTime[KEYNUM];                         // Debounce counter
unsigned short KeyPressLongTimer[KEYNUM];                   // Long press counter
unsigned short KeyContPressTimer[KEYNUM];                   // Continuous press counter

void hal_KeyInit(void)
{
	unsigned char i;
	KeyScanCBS = 0;
 
	for(i=0; i<KEYNUM; i++)
	{
		KeyStep[i] = KEY_STEP_WAIT;
		KeyScanTime[i] = KEY_SCANTIME;
		KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
		KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
	}
 
}
 
void hal_KeyScanCBSRegister(KeyEvent_CallBack_t pCBS)
{
	if(KeyScanCBS == 0)
	{
			KeyScanCBS = pCBS;
	}
}	

unsigned char  keys;
unsigned char send_buff1[] ={"hello,world!"};
void hal_KeyProc(void)
{
	unsigned char i,KeyState[KEYNUM];
    for(i=0; i<KEYNUM; i++)// Poll each key state
	{	
		keys = 0; 
 
		KeyState[i] = getKeysState[i]();
      switch(KeyStep[i])// Determine each key scan state
		{
        case KEY_STEP_WAIT:    // Wait for press
				if(KeyState[i])
				{
					KeyStep[i] = KEY_STEP_CLICK;	
				}
			break;
        case KEY_STEP_CLICK:        // Key debounce
				if(KeyState[i])
				{
          if(!(--KeyScanTime[i]))  // Debounce
					{
						KeyScanTime[i] = KEY_SCANTIME;
						KeyStep[i] = KEY_STEP_LONG_PRESS;
              //keys = i+1;                    // Record key ID
              //state = KEY_CLICK;             // Key click event
						keys = (i*5)+1;		
					}
				}else
				{
					KeyScanTime[i] = KEY_SCANTIME;
          KeyStep[i] = KEY_STEP_WAIT;  // Reset to avoid false trigger
				}
			break;
        case KEY_STEP_LONG_PRESS:     // Long press detection
				if(KeyState[i])
				{	
					if(!(--KeyPressLongTimer[i]))
					{
						KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
						KeyStep[i] = KEY_STEP_CONTINUOUS_PRESS;
						
              keys = (i*5)+3;              // Long press confirm
					 
					}
				}else
				{
					KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
					KeyStep[i] = KEY_STEP_WAIT;
            keys = (i*5)+2;                // Key release (short click)
				}
			break;
			case KEY_STEP_CONTINUOUS_PRESS:
				if(KeyState[i])
				{
					if(!(--KeyContPressTimer[i]))
					{
						KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
              keys = (i*5)+4;          // Continuous press
					}
				}else
				{
					KeyStep[i] = KEY_STEP_WAIT;
					KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
              keys = (i*5)+5;              // Key release (long press)
				}
				 
			break;
			 		
		}
		if(keys)
		{
			if(KeyScanCBS)
			{	 
				KeyScanCBS((KEY_VALUE_TYPEDEF)keys);
			}
		}	
	}
		LCD_clear_L(0,0);
		display_6_8_string(0,0,"Time:");	
      display_6_8_number_f1(30,0,Num2);// Display timer value
		display_6_8_string(120,0,"s");	//
		display_6_8_number_f1(100,0,task_num);
      display_6_8_number_f1(60,0,Param.Send3_Step);// Display step value
			//USART_SendData(USART2, 'h');
//	auto_track(point_actual,point_B);
	if(Flag.task_start == 2)
	{
      switch (task_num)// Select task by key value
		{
			case 1:
                DS2024_duty1();// Competition state machine - stage 1
					break;
			case 2:
                DS2024_duty2();// Competition state machine - stage 2
					break;		
			case 3:
                DS2024_duty3();// Competition state machine - stage 3
					break;	
			case 4:
                DS2024_duty4();// Competition state machine - stage 4
					break;			
			default:
				break;
		}		
	}	

//            imu660ra_get_acc(&acc_data);
//           imu660ra_get_gyro(&gyro_data);	
//	           printf("IMU660RA acc data: x=%5d, y=%5d, z=%5d\r\n", acc_data.x, acc_data.y, acc_data.z);
//            printf("IMU660RA gyro data:  x=%5d, y=%5d, z=%5d\r\n", gyro_data.x, gyro_data.y, gyro_data.z);
		//
                DS2024_duty2();// Competition state machine - stage 2
	// Serial send
		vofa_uart_poll(); // lightweight VOFA-only poll, bypasses seekfree assistant


}

static unsigned char hal_getKey1Sta(void)
{
	return (!DL_GPIO_readPins(K1_PORT, K1_PIN));		
} 

static unsigned char hal_getKey2Sta(void)
{
	return (!DL_GPIO_readPins(K2_PORT, K2_PIN));		
}

 
static unsigned char hal_getKey3Sta(void)
{
	return (!DL_GPIO_readPins(K3_PORT, K3_PIN));		
}

static unsigned char hal_getKey4Sta(void)
{
	return (!DL_GPIO_readPins(K4_PORT, K4_PIN));		
}

static unsigned char hal_getKey5Sta(void)
{
	return (!DL_GPIO_readPins(K5_PORT, K5_PIN));		
}

//// Custom delay function (not accurate)
//void delay_ms(unsigned int ms)
//{
//    unsigned int i, j;
//    // Loop count estimated from CPU frequency and instruction cycles
//    // Requires actual measurement and calibration for correct timing
//    for (i = 0; i < ms; i++)
//    {
//        for (j = 0; j < 8000; j++)
//        {
//            // Execute a simple operation with predictable execution time
//            __asm__("nop"); // "nop" means no operation, consumes 1 or several clock cycles
//        }
//    }
//}


//uint8_t Key_GetNum(void)
//{
//	uint8_t KeyNum = 0;
//	if(DL_GPIO_readPins(K1_PORT,K1_PIN) == 0)
//	{
//		delay_ms(20);
//		while(DL_GPIO_readPins(K1_PORT,K1_PIN) == 0);
//		delay_ms(20);
//		KeyNum = 1;
//	}
//	else if(DL_GPIO_readPins(K2_PORT,K2_PIN) == 0)
//    {
//        delay_ms(20);
//        while(DL_GPIO_readPins(K2_PORT,K2_PIN) == 0);
//        delay_ms(20);
//        KeyNum = 2;
//    }
//		else if(DL_GPIO_readPins(K3_PORT,K3_PIN) == 0)
//    {
//        delay_ms(20);
//        while(DL_GPIO_readPins(K3_PORT,K3_PIN) == 0);
//        delay_ms(20);
//        KeyNum = 3;
//    }
//		else if(DL_GPIO_readPins(K4_PORT,K4_PIN) == 0)
//    {
//        delay_ms(20);
//        while(DL_GPIO_readPins(K4_PORT,K4_PIN) == 0);
//        delay_ms(20);
//        KeyNum = 4;
//    }
//		else if(DL_GPIO_readPins(K5_PORT,K5_PIN) == 0)
//    {
//        delay_ms(20);
//        while(DL_GPIO_readPins(K5_PORT,K5_PIN) == 0);
//        delay_ms(20);
//        KeyNum = 5;
//    }
//	return KeyNum;
//}
