/*******************************************************************************
  * @����      �� wangming
  * @wechat    :DeepCoderMing
  * @qq      �� 3201935299
  * @����      �� 2025��05��01��
  * @��Ȩ����  �� �����ο�ѧϰ��δ��������ֹ����
********************************************************************************/
#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "ndrivers/LCD/lcd_init.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_beep.h"
#include "hal_timer.h"
#include "hal_encode.h"
#include "hal_tb6612.h"
#include "hal_uart.h"
#include "hal_vofa.h"
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
uint8_t     debug_uart_buffer[64];           // ���ݴ������
int main(void)
{
	SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig���������Զ�����
	hal_CPUInit();	//��ʱ��7��ʼ�� �����������
	OS_TaskInit();	//�����ʼ��
	oled_init();	//oled��ʾ����ʼ��
	LCD_Init();	// ��ֲ�� LCD ��ʼ��
	OLED_CLS();	// LCD ȫ������������������Ӱ�ͻ���
	hal_KeyInit();	// ����״̬����ʼ��
	ctrl_params_init();//ѭ�����ٶȻ�PID������������ʼ��
	hal_Encoder_Init();	//�ⲿ�жϲɼ�����������
	//hal_uart1_Init();	//����1��ʼ�� PA10 PA11 ����zigbee
	//fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, 64);	//zigbee��ɴ��ڵ�������
	TIMG0_Init();	//��ʱ���жϡ�����ȡ����ֵ 5ms
	TIMG6_Init(); //��ʱ���жϡ���ʵʱʱ�� 5ms	

	move_filter_init(&left_speed_cmps);	//�������ٶ�ֵ�˲�
	move_filter_init(&right_speed_cmps);	//�������ٶ�ֵ�˲�
	hal_uart1_Init();
	uart1_rx_register(vofa_uart_rx_callback);
	fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, 64);
	vofa_param_init();

	seekfree_assistant_init(); // Initialize seekfree assistant FIFO

	AppInit();
	Flag_Init();

	OS_CreatTask(OS_TASK1,hal_KeyProc,1,OS_RUN);  //����1�����򰴼�ɨ��
		
	OS_CreatTask(OS_TASK2,AppProc,1,OS_RUN); //����2���༶�˵�
	
	OS_Start(); //��ѭ�������� ǰ�涼�ǳ�ʼ��	
}

