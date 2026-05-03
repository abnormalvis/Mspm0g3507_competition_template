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
								
  

unsigned char KeyStep[KEYNUM];								//按键检测流程
unsigned short KeyScanTime[KEYNUM];							//去抖延时
unsigned short KeyPressLongTimer[KEYNUM];						//长按延时
unsigned short KeyContPressTimer[KEYNUM];						//连续长按延时	

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
	for(i=0; i<KEYNUM; i++)//依次轮询每个按键的状态
	{	
		keys = 0; 
 
		KeyState[i] = getKeysState[i]();
		switch(KeyStep[i])//判断每个按键的姿态
		{
			case KEY_STEP_WAIT:		//等待按键
				if(KeyState[i])
				{
					KeyStep[i] = KEY_STEP_CLICK;	
				}
			break;
			case KEY_STEP_CLICK:				//按键单击按下
				if(KeyState[i])
				{
					if(!(--KeyScanTime[i]))  //消抖
					{
						KeyScanTime[i] = KEY_SCANTIME;
						KeyStep[i] = KEY_STEP_LONG_PRESS;
						//keys = i+1;										//记录按键ID号
						//state = KEY_CLICK;								//按键单击按下
						keys = (i*5)+1;		
					}
				}else
				{
					KeyScanTime[i] = KEY_SCANTIME;
					KeyStep[i] = KEY_STEP_WAIT;  //又赋值为0  误触或者弹片抖动的情况
				}
			break;
			case KEY_STEP_LONG_PRESS:			//按键长按
				if(KeyState[i])
				{	
					if(!(--KeyPressLongTimer[i]))
					{
						KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
						KeyStep[i] = KEY_STEP_CONTINUOUS_PRESS;
						
						keys = (i*5)+3;								//长按确认
					 
					}
				}else
				{
					KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
					KeyStep[i] = KEY_STEP_WAIT;
					keys = (i*5)+2;										//单击释放
				}
			break;
			case KEY_STEP_CONTINUOUS_PRESS:
				if(KeyState[i])
				{
					if(!(--KeyContPressTimer[i]))
					{
						KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
						keys = (i*5)+4;					//持续长按
					}
				}else
				{
					KeyStep[i] = KEY_STEP_WAIT;
					KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
					keys = (i*5)+5;								//长按释放
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
		display_6_8_number_f1(30,0,Num2);//显示计数器值
		display_6_8_string(120,0,"s");	//
		display_6_8_number_f1(100,0,task_num);
		display_6_8_number_f1(60,0,Param.Send3_Step);//显示计数器值
			//USART_SendData(USART2, 'h');
//	auto_track(point_actual,point_B);
	if(Flag.task_start == 2)
	{
		switch (task_num)//通过按键键值选择赛题哪一问
		{
			case 1:
						DS2024_duty1();//任务状态机 赛题第一问
					break;
			case 2:
						DS2024_duty2();//任务状态机 赛题第二问
					break;		
			case 3:
						DS2024_duty3();//任务状态机 赛题第三问
					break;	
			case 4:
						DS2024_duty4();//任务状态机 赛题第四问
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
			//DS2024_duty2();//任务状态机
	//串口发送
		uartwork();


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

////自定义延时（不精确）
//void delay_ms(unsigned int ms)
//{
//    unsigned int i, j;
//    // 下面的嵌套循环的次数是根据主控频率和编译器生成的指令周期大致计算出来的，
//    // 需要通过实际测试调整来达到所需的延时。
//    for (i = 0; i < ms; i++)
//    {
//        for (j = 0; j < 8000; j++)
//        {
//            // 仅执行一个足够简单以致于可以预测其执行时间的操作
//            __asm__("nop"); // "nop" 代表“无操作”，在大多数架构中，这会消耗一个或几个时钟周期
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
