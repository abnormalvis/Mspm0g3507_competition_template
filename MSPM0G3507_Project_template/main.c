/*******************************************************************************
  * @作者      ： wangming
  * @wechat    :DeepCoderMing
  * @qq      ： 3201935299
  * @日期      ： 2025年05月01日
  * @版权声明  ： 仅供参考学习，未经允许禁止商用
********************************************************************************/
#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_beep.h"
#include "hal_timer.h"
#include "hal_encode.h"
#include "hal_tb6612.h"
#include "hal_uart.h"
#include "string.h"
#include "CPU.h"
#include "OS_System.h"
#include "App.h"
#include "hal_delay.h"
#include "hal_tb6612.h"
#include "mt_test.h"
#include "seekfree_uart.h"
#include "seekfree_assistant.h"
#include "move_filter.h"
#include "hal_at24c02.h"
#include "mt_flag.h"
#include "2024DS_Duty.h"
#include "zf_device_imu660ra.h"
#include "imu_filter.h"                  // Device header


int keynum = 0;
float test_eepro = 0.f;
unsigned char send_buff[] ={"hello,world!"};
uint8_t     debug_uart_buffer[64];           // 数据存放数组
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	hal_CPUInit();	//定时器7初始化 用于任务调度
	OS_TaskInit();	//任务初始化
	oled_init();	//oled显示屏初始化
	ctrl_params_init();//循迹、速度环PID控制器参数初始化
	hal_Encoder_Init();	//外部中断采集编码器脉冲
	//hal_uart1_Init();	//串口1初始化 PA10 PA11 ——zigbee
	//fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, 64);	//zigbee逐飞串口调试助手
	DL_TimerG_setCaptureCompareValue(TIMA0,99,DL_TIMER_CC_0_INDEX);	//开启四路PWM
  	DL_TimerG_setCaptureCompareValue(TIMA0,99,DL_TIMER_CC_1_INDEX);	
	DL_TimerG_setCaptureCompareValue(TIMA0,99,DL_TIMER_CC_2_INDEX);	
	DL_TimerG_setCaptureCompareValue(TIMA0,99,DL_TIMER_CC_3_INDEX);	


	TIMG0_Init();	//定时器中断——获取脉冲值 5ms
	TIMG6_Init(); //定时器中断——实时时间 5ms	

	move_filter_init(&left_speed_cmps);	//编码器速度值滤波
	move_filter_init(&right_speed_cmps);	//编码器速度值滤波

	AppInit();
	Flag_Init();

	OS_CreatTask(OS_TASK1,hal_KeyProc,1,OS_RUN);  //任务1：五向按键扫描
		
	OS_CreatTask(OS_TASK2,AppProc,1,OS_RUN); //任务2：多级菜单
	
	OS_Start(); //主循环在这里 前面都是初始化	
}

