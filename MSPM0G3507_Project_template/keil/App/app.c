/*******************************************************************************
  * @brief     : Application layer
  * @author   : wangming
  * @wechat   : DeepCoderMing
  * @qq       : 3201935299
  * @date     : 2025-05-01
  * @copyright: Confidential - for demo purposes only
********************************************************************************/
#include "ti_msp_dl_config.h"
#include "OS_System.h"
#include "hal_led.h"
#include "hal_key.h"
#include "drv_oled.h"
#include "hal_beep.h"
#include "app.h"
#include "hal_encode.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include "hal_uart.h"
#include "hal_timer.h" 
#include "mt_test.h"
#include "hal_jy62.h"
#include "hal_gray.h"
#include "hal_at24c02.h"
#include "hal_delay.h"
#include "mt_flag.h"
#include "user_interrupt.h"
#include "math.h"
#include "2024DS_Duty.h"
#include "imu_filter.h"                  // Device header

static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);

static void gnlMenu_Desktop1CBS(void);
static void gnlMenu_Desktop2CBS(void);
static void gnlMenu_Desktop3CBS(void);

static void menuInit(void);

static void stg1Menu_MainMenuCBS(void);
static void stg1Menu_Para1CBS(void);
static void stg1Menu_Para2CBS(void);
static void stg1Menu_Para3CBS(void);
static void stg1Menu_Para4CBS(void);
static void stg1Menu_Para5CBS(void);
static void stg1Menu_Para6CBS(void);
static void stg1Menu_Para7CBS(void);
static void stg1Menu_Para8CBS(void);
static void stg1Menu_Para9CBS(void);


static void stg2Menu_MainMenuCBS(void);
static void stg2Menu_Para1CBS(void);
static void stg2Menu_Para2CBS(void);
static void stg2Menu_Para3CBS(void);
static void stg2Menu_Para4CBS(void);
static void stg2Menu_Para5CBS(void);
static void stg2Menu_Para6CBS(void);
static void stg2Menu_Para7CBS(void);

static void stg3Menu_MainMenuCBS(void);
static void stg3Menu_Para1CBS(void);
static void stg3Menu_Para2CBS(void);
static void stg3Menu_Para3CBS(void);
static void stg3Menu_Para4CBS(void);
static void stg3Menu_Para5CBS(void);
static void stg3Menu_Para6CBS(void);
static void stg3Menu_Para7CBS(void);
static void stg3Menu_Para8CBS(void);
stu_mode_menu *pModeMenu;		//ϵͳ��ǰִ�в˵��Ľṹ��ָ�� 

static void hal_Oled_Clear(void)
{
	LCD_clear_L(0,1);
	LCD_clear_L(0,2);
	LCD_clear_L(0,3);
	LCD_clear_L(0,4);
	LCD_clear_L(0,5);
	LCD_clear_L(0,6);
	LCD_clear_L(0,7);
}

/***����**/
//�˵�ΨһID��
//��ǰ�˵���λ����Ϣ
//ָ��ǰģʽ����
//��ǰģʽ�µ���Ӧ����
//ˢ������ʾ����
//Ԥ���������������
//����ֵ,0xFF�����ް�������
//ָ����һ��ѡ��
//ָ����һ��ѡ��
//ָ�򸸼��˵�
//ָ���Ӽ��˵�
 
//��ʼ������˵���һ���˵��� ��ͨģʽ�˵�
stu_mode_menu generalModeMenu[GNL_MENU_SUM] = //����ṹ�����飬���ҽ��г�ʼ�� 
{
	{GNL_MENU_DESKTOP1,DESKTOP_MENU_POS,"Desktop1",gnlMenu_Desktop1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{GNL_MENU_DESKTOP2,DESKTOP_MENU_POS,"Desktop2",gnlMenu_Desktop2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{GNL_MENU_DESKTOP3,DESKTOP_MENU_POS,"Desktop3",gnlMenu_Desktop3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	//�˵�ΨһID��  �˵�λ����Ϣ����ö�ٱ������ַ�ָ�� ����˵��Ĵ�������       
};	

//��1����ʼ�����ò˵���һ���˵��� �ṹ������ ���ò˵������������б�
stu_mode_menu settingModeMenu1[STG1_MENU_SUM] = 
{
	{STG1_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu1",stg1Menu_MainMenuCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA1,STG_SUB_2_MENU_POS,"1. pos_out_limH",stg1Menu_Para1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA2,STG_SUB_2_MENU_POS,"2. pos_out_limL",stg1Menu_Para2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA3,STG_SUB_2_MENU_POS,"3. speed_adjust",stg1Menu_Para3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA4,STG_SUB_2_MENU_POS,"4. theta_limit ",stg1Menu_Para4CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA5,STG_SUB_2_MENU_POS,"5. pos_boudary ",stg1Menu_Para5CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA6,STG_SUB_2_MENU_POS,"6. turn_kp ",stg1Menu_Para6CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA7,STG_SUB_2_MENU_POS,"7. sleep_time ",stg1Menu_Para7CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//
	{STG1_MENU_PARA8,STG_SUB_2_MENU_POS,"8. yaw_track_kp ",stg1Menu_Para8CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG1_MENU_PARA9,STG_SUB_2_MENU_POS,"9. yaw_track_kd ",stg1Menu_Para9CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
};

//��2����ʼ�����ò˵���һ���˵��� �ṹ������ ���ò˵������������б�
stu_mode_menu settingModeMenu2[STG2_MENU_SUM] = 
{
	{STG2_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu2",stg2Menu_MainMenuCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA1,STG_SUB_2_MENU_POS,"1. task1",stg2Menu_Para1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//��λ����
	{STG2_MENU_PARA2,STG_SUB_2_MENU_POS,"2. task2",stg2Menu_Para2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//��Ǧ�ʿ����˳ʱ��һ��
	{STG2_MENU_PARA3,STG_SUB_2_MENU_POS,"3. task3",stg2Menu_Para3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0}, //�ؽ�������˳ʱ��һ��
	{STG2_MENU_PARA4,STG_SUB_2_MENU_POS,"4. task4 ",stg2Menu_Para4CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"5. Dire ",stg2Menu_Para5CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"6. Clear ",stg2Menu_Para6CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"7. Orignal ",stg2Menu_Para7CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
};

//��2����ʼ�����ò˵���һ���˵��� �ṹ������ ���ò˵������������б�
stu_mode_menu settingModeMenu3[STG3_MENU_SUM] = 
{
	{STG3_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu3",stg3Menu_MainMenuCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA1,STG_SUB_2_MENU_POS,"1. A_x",stg3Menu_Para1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA2,STG_SUB_2_MENU_POS,"2. A_y",stg3Menu_Para2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA3,STG_SUB_2_MENU_POS,"3. B_x",stg3Menu_Para3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA4,STG_SUB_2_MENU_POS,"4. B_y",stg3Menu_Para4CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA5,STG_SUB_2_MENU_POS,"5. C_x",stg3Menu_Para5CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA5,STG_SUB_2_MENU_POS,"6. C_y",stg3Menu_Para6CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA5,STG_SUB_2_MENU_POS,"7. D_x",stg3Menu_Para7CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG3_MENU_PARA5,STG_SUB_2_MENU_POS,"8. D_y",stg3Menu_Para8CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	
	
};

void AppInit(void)
{
	menuInit();    //����Ҫ��ʼ�������ܽ��˵���ʾΪ����˵�

	hal_KeyScanCBSRegister(KeyEventHandle);

}

//�˵���ʼ������ ʵ�ֲ˵�˫����������
static void menuInit(void)
{
	unsigned char i;
	/*���������´���˳���ܵ���������*/
	//�ѵڶ����Ӳ˵��б��γ�ѭ��˫��������ʽ���������
	//�����ͷ��β�������� 
	settingModeMenu1[1].pLase = &settingModeMenu1[STG1_MENU_SUM-1];	//settingModeMenu1[1].pLase����һ��ѡ��
	settingModeMenu1[1].pNext = &settingModeMenu1[2];	//settingModeMenu1[1].pNext����һ��ѡ��
	settingModeMenu1[1].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent�ĸ����˵�

	//�����ò˵��ĵڶ�����ʼ���� ��һ�������˵�ҳ�� ��һ��ѡ�� ��һ��ѡ�� �����˵�
	for(i=2; i<STG1_MENU_SUM-1; i++)
	{
		settingModeMenu1[i].pLase = &settingModeMenu1[i-1];
		settingModeMenu1[i].pNext = &settingModeMenu1[i+1];
		settingModeMenu1[i].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];
	}	
	
	//�����β��ͷ�������� ���ò˵������һ��ѡ�� ����һ��ѡ�� ��һ��ѡ�� �����˵�
	settingModeMenu1[STG1_MENU_SUM-1].pLase = &settingModeMenu1[i-1];
	settingModeMenu1[STG1_MENU_SUM-1].pNext = &settingModeMenu1[1];
	settingModeMenu1[STG1_MENU_SUM-1].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];	
	//���ϴ���ʵ��ѭ��˫������,˳���ܵ�������������	
	/*���������ϴ���˳���ܵ���������*/

	/*���������´���˳���ܵ���������*/
	//�ѵڶ����Ӳ˵��б��γ�ѭ��˫��������ʽ���������
	//�����ͷ��β�������� 
	settingModeMenu2[1].pLase = &settingModeMenu2[STG2_MENU_SUM-1];	//settingModeMenu1[1].pLase����һ��ѡ��
	settingModeMenu2[1].pNext = &settingModeMenu2[2];	//settingModeMenu1[1].pNext����һ��ѡ��
	settingModeMenu2[1].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent�ĸ����˵�

	//�����ò˵��ĵڶ�����ʼ���� ��һ�������˵�ҳ�� ��һ��ѡ�� ��һ��ѡ�� �����˵�
	for(i=2; i<STG2_MENU_SUM-1; i++)
	{
		settingModeMenu2[i].pLase = &settingModeMenu2[i-1];
		settingModeMenu2[i].pNext = &settingModeMenu2[i+1];
		settingModeMenu2[i].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];
	}	
	
	//�����β��ͷ�������� ���ò˵������һ��ѡ�� ����һ��ѡ�� ��һ��ѡ�� �����˵�
	settingModeMenu2[STG2_MENU_SUM-1].pLase = &settingModeMenu2[i-1];
	settingModeMenu2[STG2_MENU_SUM-1].pNext = &settingModeMenu2[1];
	settingModeMenu2[STG2_MENU_SUM-1].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];	
	//���ϴ���ʵ��ѭ��˫������,˳���ܵ�������������	
	/*���������ϴ���˳���ܵ���������*/
	
	/*���������´���˳���ܵ���������*/
	//�ѵڶ����Ӳ˵��б��γ�ѭ��˫��������ʽ���������
	//�����ͷ��β�������� 
	settingModeMenu3[1].pLase = &settingModeMenu3[STG3_MENU_SUM-1];	//settingModeMenu1[1].pLase����һ��ѡ��
	settingModeMenu3[1].pNext = &settingModeMenu3[2];	//settingModeMenu1[1].pNext����һ��ѡ��
	settingModeMenu3[1].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent�ĸ����˵�

	//�����ò˵��ĵڶ�����ʼ���� ��һ�������˵�ҳ�� ��һ��ѡ�� ��һ��ѡ�� �����˵�
	for(i=2; i<STG3_MENU_SUM-1; i++)
	{
		settingModeMenu3[i].pLase = &settingModeMenu3[i-1];
		settingModeMenu3[i].pNext = &settingModeMenu3[i+1];
		settingModeMenu3[i].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];
	}	
	
	//�����β��ͷ�������� ���ò˵������һ��ѡ�� ����һ��ѡ�� ��һ��ѡ�� �����˵�
	settingModeMenu3[STG3_MENU_SUM-1].pLase = &settingModeMenu3[i-1];
	settingModeMenu3[STG3_MENU_SUM-1].pNext = &settingModeMenu3[1];
	settingModeMenu3[STG3_MENU_SUM-1].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];	
	//���ϴ���ʵ��ѭ��˫������,˳���ܵ�������������	
	/*���������ϴ���˳���ܵ���������*/	
	
	
	//ϵͳ��ǰִ�в˵��Ľṹ��ָ��ָ�� ��ͨģʽ������˵� Ȼ��Ϳ��Ըı���ͨģʽ������˵���Ĳ���
	pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];	//�����ϵ���ʾ�Ĳ˵�����Ϊ������ʾ
	pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;	//����ˢ�½����־����������ˢ��ȫ����UI
	//���д�����ʵ��д�������ʼ��������Ҳ�����ֵ����Ϊ������ͨģʽ������˵�ʱ���Ѿ�������ʼ��Ϊ��SCREEN_CMD_RESET
}


int key_count;    //����ֵ
int task_num = 0;
char stop_task = 0;

float pwm_x0 = 1950,pwm_y0 = 1050;	//��λλ�õ�pwm
float pwm_x1 = 1950,pwm_y1 = 1050;	//�ܱ����Ͻǵ��pwm
float pwm_x2 = 1950,pwm_y2 = 1050;	//�ܱ����½ǵ��pwm
char Beep_toggle_flag = 0;
//����1�˵�������
static void gnlMenu_Desktop1CBS(void)
{
	unsigned char keys;
	static uint8_t gray_display_state = 1;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)//״̬Ϊˢ��ʱ��������
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;//�´β�ˢ���ˣ�ˢ��һ��
		pModeMenu->keyVal = 0xFF;//��ʼ��Ϊû�а�������
		 
		hal_Oled_Clear();
		//display_6_8_string(110,0,"M1");
	}

	switch(gray_display_state)
	{	
		case 1:
			gray_display_state++;
			LCD_clear_L(0,1);
			display_6_8_string(0,1,"cmps:");			  //����/��   
			display_6_8_number(40,1,smartcar_imu.left_motor_speed_cmps);  //smartcar_imu.left_motor_speed_cmps EncoderA
			display_6_8_number(85,1,smartcar_imu.right_motor_speed_cmps); 		
	
			break;		
		case 2:
			gray_display_state++;
			LCD_clear_L(0,2);
		//distance_l
			display_6_8_string(0,2,"dtas:");			  //����/��   
			display_6_8_number(30,2,distance_inter);  
			//display_6_8_number(80,2,distance_r); 			
//			display_6_8_string(0,2,"p_x1:");				         
//			display_6_8_number(30,2,pwm_x1);
//			display_6_8_string(60,2,"p_y1:");				         
//			display_6_8_number(90,2,pwm_y1);
			break;
		case 3:
			gray_display_state++;
			LCD_clear_L(0,3);
			display_6_8_string(0,3,"vtar:");				         
			display_6_8_number(30,3,(int)v_target_l);
			display_6_8_number(60,3,(int)v_target_r);//cam_turn_kp
		display_6_8_number(90,3,speed_setup); 
//		display_6_8_number(30,3,(int)left_pwm);
//		display_6_8_number(50,3,(int)right_pwm);
//			display_6_8_number(80,3,(int)turn_ctrl_pwm);//
//		display_6_8_number(110,3,(int)turn_scale);
			break;
		case 4:
			gray_display_state++;		
			LCD_clear_L(0,4);	  
			LCD_clear_L(80,4);	
			display_6_8_string(0,4,"ang:");
			display_6_8_number(30,4,cos(pi/180*60)); 	
			//display_6_8_number(70,4,angle.y); 	
	  	display_6_8_number(80,4,imu.yaw); 		  
			break;
		case 5:
			gray_display_state++;
			LCD_clear_L(0,5);
			display_6_8_string(0,5,"gray:");
		//	display_6_8_number(40,5,error_openmv); 
//			display_6_8_number(70,5,AT24C02_ReadByte(0)); 
//			display_6_8_number(90,5,AT24C02_ReadByte(8)); 
			//display_6_8_number(90,5,test_eepro); 
//			display_6_8_number(60,5,speed_setup);// 
		display_6_8_number(60,5,Flag.Start_duty_1);
		display_6_8_number(80,5,Param.Send2_Step);
		display_6_8_number(100,5,Flag.Start_Car); 
		//	display_6_8_number(60,5,Storage_ReadFloatNum(7).value); 
			break;
		case 6:
			gray_display_state++;
			LCD_clear_L(0,6);
			LCD_clear_L(50,6);

			break;
		case 7:
			gray_display_state++;
			LCD_clear_L(0,7);
			LCD_clear_L(50,7);
			display_6_8_number(0,7,gray_state.gray.bit12);
			display_6_8_number(10,7,gray_state.gray.bit11);
			display_6_8_number(20,7,gray_state.gray.bit10);
			display_6_8_number(30,7,gray_state.gray.bit9);
			display_6_8_number(40,7,gray_state.gray.bit8);
			display_6_8_number(50,7,gray_state.gray.bit7);			
			display_6_8_number(60,7,gray_state.gray.bit6);
			display_6_8_number(70,7,gray_state.gray.bit5);
			display_6_8_number(80,7,gray_state.gray.bit4);
			display_6_8_number(90,7,gray_state.gray.bit3);
			display_6_8_number(100,7,gray_state.gray.bit2);
			display_6_8_number(110,7,gray_state.gray.bit1);		
			break;			
		default:
			gray_display_state = 1;
	}
	//���´���������˵��������У���ʾ������˵��³����м��Ż����������
	if(pModeMenu->keyVal != 0xff)//�а�������
	{
		keys = pModeMenu->keyVal; //keys��������������ʱ����

		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ keys����ֵ��
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu1[0];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	//�Ҽ�
				pModeMenu = &generalModeMenu[1];	//��ת����������ͷ����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_CLICK_RELEASE:	//���
				pModeMenu = &generalModeMenu[2];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:	//�ϼ� ��������
						Flag.Start_Car = 1;
			break;				
			case KEY2_CLICK_RELEASE:	//�ϼ� ��������
						Flag.Start_Car = 0;			
			break;				
		}
	}
	
}

//����2�˵�������
static void gnlMenu_Desktop2CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)//״̬Ϊˢ��ʱ��������
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;//�´β�ˢ���ˣ�ˢ��һ��
		pModeMenu->keyVal = 0xFF;//��ʼ��Ϊû�а�������
		 
		hal_Oled_Clear();
		//display_6_8_string(110,0,"M2");

		
	}
	display_6_8_string(50,4,"TASK");
	//���´���������˵��������У���ʾ������˵��³����м��Ż����������
	if(pModeMenu->keyVal != 0xff)//�а�������
	{
		keys = pModeMenu->keyVal; //keys��������������ʱ����

		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ keys����ֵ��
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu2[0];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
				
			break;
			case KEY3_CLICK_RELEASE:	//���
				pModeMenu = &generalModeMenu[0];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	//�Ҽ�
				pModeMenu = &generalModeMenu[2];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:	//�ϼ� ��������
						
			break;			
			
		}
	}
	
}

//����3���Ʋ˵�������
static void gnlMenu_Desktop3CBS(void)
{
	unsigned char keys;
	static uint8_t gray_display_state = 1;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)//״̬Ϊˢ��ʱ��������
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;//�´β�ˢ���ˣ�ˢ��һ��
		pModeMenu->keyVal = 0xFF;//��ʼ��Ϊû�а�������
		 
		hal_Oled_Clear();
	//	display_6_8_string(110,0,"M3");

	}
	switch(gray_display_state)
	{	
		case 1:
			gray_display_state++;
			LCD_clear_L(0,1);
			display_6_8_string(0,1,"pointA:");			  //����/��   
			display_6_8_number(40,1,point_A[0]);  //smartcar_imu.left_motor_speed_cmps EncoderA
			display_6_8_number(80,1,point_A[1]); 		
	
			break;		
		case 2:
			gray_display_state++;
			LCD_clear_L(0,2);
			display_6_8_string(0,2,"pointB:");			 
			display_6_8_number(40,2,point_B[0]);  
			display_6_8_number(80,2,point_B[1]); 	
			break;
		case 3:
			gray_display_state++;
			LCD_clear_L(0,3);
			display_6_8_string(0,3,"pointC:");			 
			display_6_8_number(40,3,point_C[0]);  
			display_6_8_number(80,3,point_C[1]); 	

			break;
		case 4:
			gray_display_state++;		
			LCD_clear_L(0,4);	  
			display_6_8_string(0,4,"pointD:");			 
			display_6_8_number(40,4,point_D[0]);  
			display_6_8_number(80,4,point_D[1]); 		  
			break;
		case 5:
			gray_display_state++;
			LCD_clear_L(0,5);
//			display_6_8_string(0,5,"gray:");

			break;
		case 6:
			gray_display_state++;
			LCD_clear_L(0,6);

			break;
		case 7:
			gray_display_state++;
			LCD_clear_L(0,7);


			break;			
		default:
			gray_display_state = 1;
	}	
	//���´���������˵��������У���ʾ������˵��³����м��Ż����������
	if(pModeMenu->keyVal != 0xff)//�а�������
	{
		keys = pModeMenu->keyVal; //keys��������������ʱ����

		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ keys����ֵ��
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu3[0];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_CLICK_RELEASE:	//���
				pModeMenu = &generalModeMenu[1];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	//�Ҽ�
				pModeMenu = &generalModeMenu[0];	//��ת�����ò˵�����
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
		}
	}
	
}

//���˵�1��������
static void stg1Menu_MainMenuCBS(void)
{
	//pMenu  MHead  MTail�Ƕ����Ľṹ��ָ�룬�����˶�������Դ
	//static����ı������´��ٻص���������У�������ֵ������һ������
	unsigned char keys;//��ֵ��ʱ����ı���
	unsigned char i;//����forѭ��
	unsigned char ClrScreenFlag;//Ҫ��Ҫȥˢ���ı�־λ����ҳ��ʱ��ˢ��������б��������ʾ��������ʾ������ҳ�Ļ�����ˢ��
	static stu_mode_menu *pMenu;		//�������浱ǰѡ�еĲ˵�
	static stu_mode_menu *bpMenu=0;		//����������һ�β˵�ѡ���Ҫ����ˢ���жϣ���pMenu��= bpMenuʱ��ˢ��
	static unsigned char stgMainMenuSelectedPos=0;	//������¼��ǰѡ�в˵���λ�ã�ѡ���б���ѡ�в˵���λ�ã�Ĭ�ϵ���1���ӵ�һ���˵���ʼ
	static stu_mode_menu *MHead,*MTail;		//�������ṹ��ָ����Ϊ�������л��˵�ʱ����ҳ����
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//��Ҫˢ����Ļ
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//�´ν������������ˢ���ˣ�ֻˢ��һ�Σ������м��������ҳ��ˢ��һ��
		pMenu = &settingModeMenu1[0];//ִ�����˵�
		hal_Oled_Clear();//ˢ��
		
		//display_6_8_string(10,0,pMenu->pModeType);//��Ҫ��ʾ������д��oled���棬oled�����������ʾ��Ҫ������һ��ˢ��

		
	//ˢ���͸�����ʾ��ʱ�䣬ֻ�м�ֵ�ı��ʱ��Ž���
		
		pMenu = &settingModeMenu1[1];	//���ò˵��б��ĵ�һ��ѡ��
		
		MHead = pMenu;			//��¼��ǰ���ò˵���һ��
		MTail = pMenu+6;		//��¼��ǰ���ò˵����һ��,һҳ��ʾ4��ѡ���5��ѡ��
		bpMenu = 0;	//bpMenu��pMenu����ȣ����������ui��ʾ����
 
		ClrScreenFlag = 1;	//д��1��ʹ�õ�һ�ν����������ʾ����ʱ���Ѳ˵��б���ʾ����������ѡ���б���ʾ������
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//���������˵� ������ҳ����
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//��
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//ȡ��ѡ�б��˵���ʾ
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//��
				if(stgMainMenuSelectedPos == 7)///��ҳ��ѡ�е�4���˵����¼�ʱ
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos�����4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//ȡ��ѡ�б��˵���ʾ
					pMenu = pMenu->pNext;																			//�л���һ��ѡ��
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	//�Ҽ� ȷ��
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			
		}
	}
	
	//���´�������UI��ʾ����ʾ��ҳ��Ĳ˵��б�
	if(bpMenu != pMenu)//���ָ���ַ��һ�£���Ҫ������ʾ�������Ѿ�ǿ�Ƶ�ַ��һ����
	{
		bpMenu = pMenu;//bpMenu����Ϊ���ڲ˵�״̬����pMenu�ı䣬��bpMenu != pMenu����ʾ�а������£�Ҫ������Ļ��ʾ
		if(ClrScreenFlag)//ˢ����־λ����ҳ��ʱ�������ã���ҳ��ʱ��Ҫ����
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		//���� ����б����ǿ�����

			for(i=1; i<8; i++)	//����һ����i<5����Ϊһҳֻ����ʾ4���˵��б�
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//ѭ����ʾ�˵��б�������

				pMenu = pMenu->pNext;//��һ��
			} 
			pMenu = bpMenu;//��pMenuָ��&settingModeMenu1[1];ָ���һ���˵���ǰ��forѭ����pMenu = pMenu->pNext;pMenuһֱ�仯
			//���´���Ϊ�����ʾ��λ��
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//��ɫ��ʾ
		 
		}else//����Ҫ����������ҳ��ʱ��
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//��ɫ��ʾ

		}	
			
				 
	}
}
//���˵�2��������
static void stg2Menu_MainMenuCBS(void)
{
	//pMenu  MHead  MTail�Ƕ����Ľṹ��ָ�룬�����˶�������Դ
	//static����ı������´��ٻص���������У�������ֵ������һ������
	unsigned char keys;//��ֵ��ʱ����ı���
	unsigned char i;//����forѭ��
	unsigned char ClrScreenFlag;//Ҫ��Ҫȥˢ���ı�־λ����ҳ��ʱ��ˢ��������б��������ʾ��������ʾ������ҳ�Ļ�����ˢ��
	static stu_mode_menu *pMenu;		//�������浱ǰѡ�еĲ˵�
	static stu_mode_menu *bpMenu=0;		//����������һ�β˵�ѡ���Ҫ����ˢ���жϣ���pMenu��= bpMenuʱ��ˢ��
	static unsigned char stgMainMenuSelectedPos=0;	//������¼��ǰѡ�в˵���λ�ã�ѡ���б���ѡ�в˵���λ�ã�Ĭ�ϵ���1���ӵ�һ���˵���ʼ
	static stu_mode_menu *MHead,*MTail;		//�������ṹ��ָ����Ϊ�������л��˵�ʱ����ҳ����
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//��Ҫˢ����Ļ
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//�´ν������������ˢ���ˣ�ֻˢ��һ�Σ������м��������ҳ��ˢ��һ��
		pMenu = &settingModeMenu2[0];//ִ�����˵�
		hal_Oled_Clear();//ˢ��
		
		//display_6_8_string(37,0,pMenu->pModeType);//��Ҫ��ʾ������д��oled���棬oled�����������ʾ��Ҫ������һ��ˢ��
		
	//ˢ���͸�����ʾ��ʱ�䣬ֻ�м�ֵ�ı��ʱ��Ž���
		
		pMenu = &settingModeMenu2[1];	//���ò˵��б��ĵ�һ��ѡ��
		
		MHead = pMenu;			//��¼��ǰ���ò˵���һ��
		MTail = pMenu+6;		//��¼��ǰ���ò˵����һ��,һҳ��ʾ4��ѡ���5��ѡ��
		bpMenu = 0;	//bpMenu��pMenu����ȣ����������ui��ʾ����
 
		ClrScreenFlag = 1;	//д��1��ʹ�õ�һ�ν����������ʾ����ʱ���Ѳ˵��б���ʾ����������ѡ���б���ʾ������
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//���������˵� ������ҳ����
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//��
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//ȡ��ѡ�б��˵���ʾ
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//��
				if(stgMainMenuSelectedPos == 7)///��ҳ��ѡ�е�4���˵����¼�ʱ
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos�����4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//ȡ��ѡ�б��˵���ʾ
					pMenu = pMenu->pNext;																			//�л���һ��ѡ��
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	//�Ҽ� ȷ��
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			case KEY5_CLICK_RELEASE:	//�м� ȷ��
					if(stgMainMenuSelectedPos < 5)
					{
						task_num = (stgMainMenuSelectedPos-1)%5 + 1;
						Flag.task_start = 1;
						Flag.beep_on = 1;
					}
					else if(stgMainMenuSelectedPos == 6)
					{
						Flag_Init();
						Flag.beep_on = 1;						
					}
					else if(stgMainMenuSelectedPos == 7)
					{
						Flag_Init();
						point_actual[0] = point_A[0];
						point_actual[1] = point_A[1];
						Flag.beep_on = 1;						
					}
			break;			
		}
	}
	
	//���´�������UI��ʾ����ʾ��ҳ��Ĳ˵��б�
	if(bpMenu != pMenu)//���ָ���ַ��һ�£���Ҫ������ʾ�������Ѿ�ǿ�Ƶ�ַ��һ����
	{
		bpMenu = pMenu;//bpMenu����Ϊ���ڲ˵�״̬����pMenu�ı䣬��bpMenu != pMenu����ʾ�а������£�Ҫ������Ļ��ʾ
		if(ClrScreenFlag)//ˢ����־λ����ҳ��ʱ�������ã���ҳ��ʱ��Ҫ����
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		//���� ����б����ǿ�����
			for(i=1; i<8; i++)	//����һ����i<5����Ϊһҳֻ����ʾ4���˵��б�
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//ѭ����ʾ�˵��б�������
				pMenu = pMenu->pNext;//��һ��
			} 
			pMenu = bpMenu;//��pMenuָ��&settingModeMenu1[1];ָ���һ���˵���ǰ��forѭ����pMenu = pMenu->pNext;pMenuһֱ�仯
			//���´���Ϊ�����ʾ��λ��
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//��ɫ��ʾ
		}else//����Ҫ����������ҳ��ʱ��
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//��ɫ��ʾ
		}	
			
				 
	}
}
//���˵�3��������
static void stg3Menu_MainMenuCBS(void)
{
	//pMenu  MHead  MTail�Ƕ����Ľṹ��ָ�룬�����˶�������Դ
	//static����ı������´��ٻص���������У�������ֵ������һ������
	unsigned char keys;//��ֵ��ʱ����ı���
	unsigned char i;//����forѭ��
	unsigned char ClrScreenFlag;//Ҫ��Ҫȥˢ���ı�־λ����ҳ��ʱ��ˢ��������б��������ʾ��������ʾ������ҳ�Ļ�����ˢ��
	static stu_mode_menu *pMenu;		//�������浱ǰѡ�еĲ˵�
	static stu_mode_menu *bpMenu=0;		//����������һ�β˵�ѡ���Ҫ����ˢ���жϣ���pMenu��= bpMenuʱ��ˢ��
	static unsigned char stgMainMenuSelectedPos=0;	//������¼��ǰѡ�в˵���λ�ã�ѡ���б���ѡ�в˵���λ�ã�Ĭ�ϵ���1���ӵ�һ���˵���ʼ
	static stu_mode_menu *MHead,*MTail;		//�������ṹ��ָ����Ϊ�������л��˵�ʱ����ҳ����
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//��Ҫˢ����Ļ
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//�´ν������������ˢ���ˣ�ֻˢ��һ�Σ������м��������ҳ��ˢ��һ��
		pMenu = &settingModeMenu3[0];//ִ�����˵�
		hal_Oled_Clear();//ˢ��
		
		//display_6_8_string(37,0,pMenu->pModeType);//��Ҫ��ʾ������д��oled���棬oled�����������ʾ��Ҫ������һ��ˢ��
		
	//ˢ���͸�����ʾ��ʱ�䣬ֻ�м�ֵ�ı��ʱ��Ž���
		
		pMenu = &settingModeMenu3[1];	//���ò˵��б��ĵ�һ��ѡ��
		
		MHead = pMenu;			//��¼��ǰ���ò˵���һ��
		MTail = pMenu+6;		//��¼��ǰ���ò˵����һ��,һҳ��ʾ4��ѡ���5��ѡ��		�����ֵ3��ʲô�ã���
		bpMenu = 0;	//bpMenu��pMenu����ȣ����������ui��ʾ����
 
		ClrScreenFlag = 1;	//д��1��ʹ�õ�һ�ν����������ʾ����ʱ���Ѳ˵��б���ʾ����������ѡ���б���ʾ������
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//���������˵� ������ҳ����
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//��
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//ȡ��ѡ�б��˵���ʾ
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//��
				if(stgMainMenuSelectedPos == 7)///��ҳ��ѡ�е�4���˵����¼�ʱ
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos�����4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//ȡ��ѡ�б��˵���ʾ
					pMenu = pMenu->pNext;																			//�л���һ��ѡ��
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	//�Ҽ� ȷ��
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			
		}
	}
	
	//���´�������UI��ʾ����ʾ��ҳ��Ĳ˵��б�
	if(bpMenu != pMenu)//���ָ���ַ��һ�£���Ҫ������ʾ�������Ѿ�ǿ�Ƶ�ַ��һ����
	{
		bpMenu = pMenu;//bpMenu����Ϊ���ڲ˵�״̬����pMenu�ı䣬��bpMenu != pMenu����ʾ�а������£�Ҫ������Ļ��ʾ
		if(ClrScreenFlag)//ˢ����־λ����ҳ��ʱ�������ã���ҳ��ʱ��Ҫ����
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		//���� ����б����ǿ�����
			for(i=1; i<8; i++)	//����һ����i<5����Ϊһҳֻ����ʾ4���˵��б�
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//ѭ����ʾ�˵��б�������
				pMenu = pMenu->pNext;//��һ��
			} 
			pMenu = bpMenu;//��pMenuָ��&settingModeMenu1[1];ָ���һ���˵���ǰ��forѭ����pMenu = pMenu->pNext;pMenuһֱ�仯
			//���´���Ϊ�����ʾ��λ��
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//��ɫ��ʾ
		}else//����Ҫ����������ҳ��ʱ��
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//��ɫ��ʾ
		}	
			
				 
	}
}

static void stg1Menu_Para1CBS(void)
{
	unsigned char keys;
	
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		//hal_Oled_ShowString(20,0,"1. Inductance",12,1);
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		
		keys = 0xFF;
		 
	}	
	//LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_out_limH); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				pos_out_limH+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				pos_out_limH-=5;

			break;			
		} 
				data_test.value = pos_out_limH;
				Storage_WriteFloatNum(8*4,data_test);				

	}
}

static void stg1Menu_Para2CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_out_limL); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				pos_out_limL+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				pos_out_limL-=5;

			break;			
		} 
				data_test.value = pos_out_limL;
				Storage_WriteFloatNum(9*4,data_test);				

	}

}

static void stg1Menu_Para3CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,speed_adjust); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				speed_adjust+=1;
				if(speed_adjust > 50)
					speed_adjust = 50;
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				speed_adjust-=1;
				if(speed_adjust < 1)
					speed_adjust = 1;
			break;			
		} 
				data_test.value = speed_adjust;
				Storage_WriteFloatNum(10*4,data_test);				

	}
}

static void stg1Menu_Para4CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,tar_theta_limit); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				tar_theta_limit+=1;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				tar_theta_limit-=1;

			break;			
		} 
				data_test.value = tar_theta_limit;
				Storage_WriteFloatNum(11*4,data_test);				

	}

}

static void stg1Menu_Para5CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_boudary); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				pos_boudary+=1;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				pos_boudary-=1;

			break;			
		} 
				data_test.value = pos_boudary;
				Storage_WriteFloatNum(12*4,data_test);				

	}
}
static void stg1Menu_Para6CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,turn_kp); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				turn_kp+=0.2;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				turn_kp-=0.2;

			break;			
		} 
				data_test.value = turn_kp;
				Storage_WriteFloatNum(13*4,data_test);				

	}
}

static void stg1Menu_Para7CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,sleep_time); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				sleep_time+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				sleep_time-=5;

			break;			
		} 
				data_test.value = sleep_time;
				Storage_WriteFloatNum(14*4,data_test);				

	}	
}

static void stg1Menu_Para8CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,yaw_track_kp); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				yaw_track_kp+=0.01;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				yaw_track_kp-=0.01;

			break;	//
			case KEY1_LONG_PRESS_CONTINUOUS:		//�ϼ� �������ֵ
				yaw_track_kp+=0.01;
	
			break;			
			case KEY2_LONG_PRESS_CONTINUOUS:		//�¼� ��С����ֵ
				yaw_track_kp-=0.01;

			break;			
		} 
				data_test.value = yaw_track_kp;
				Storage_WriteFloatNum(15*4,data_test);				

	}	
}
static void stg1Menu_Para9CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,yaw_track_kd); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				yaw_track_kd+=0.01;
	
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				yaw_track_kd-=0.01;

			break;	//
			case KEY1_LONG_PRESS_CONTINUOUS:		//�ϼ� �������ֵ
				yaw_track_kd+=0.01;
	
			break;			
			case KEY2_LONG_PRESS_CONTINUOUS:		//�¼� ��С����ֵ
				yaw_track_kd-=0.01;

			break;			
		} 
				data_test.value = yaw_track_kd;
				Storage_WriteFloatNum(16*4,data_test);				

	}	
}
static void stg2Menu_Para1CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"TASK1:");
		
		keys = 0xFF;
		 
	}	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				
			break;
			case KEY5_CLICK_RELEASE: //�м��̰�
//				Time0A_init(5,USER_INT0);   //����ȷPWMʱ�򣬴򿪶��
//				display_6_8_string(5,4,"Reset Action!");

			break;			
		}
	}
}

static void stg2Menu_Para2CBS(void)
{
	unsigned char keys;

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"TASK2:");
		
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,6);
	//display_6_8_number(60,6,Param.Send_Step); 


	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				
			break;
			case KEY5_CLICK_RELEASE: //�м��̰�
				//display_6_8_string(5,4,"TASK2 Action!");

			break;			
		}
	}
}
static void stg2Menu_Para3CBS(void)
{
	unsigned char keys;
	
	//hal_Oled_ShowString(10,38,"kp4:",8,1);
//	hal_Oled_ShowNum(38, 38, kp4, 2,8,1);
//	hal_Oled_Refresh();
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"stg2Para3:");

		keys = 0xFF;
		 
	}	

	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
//				kp4++;
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
//				kp4--;
			break;
			case KEY5_CLICK_RELEASE: //�м��̰�
				//display_6_8_string(5,4,"TASK2 Action!");

			break;				
			
		}
	}		
}
static void stg2Menu_Para4CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para4:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //�м��̰�
				//display_6_8_string(5,4,"TASK2 Action!");

			break;				
		}
	}		

}
static void stg2Menu_Para5CBS(void)
{
	
	static char key_value ;
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		//display_6_8_string(0,2,"stg2Para5:");
				 
	}	
	Flag.Start_Car = 0;
	
			display_6_8_string(0,2,"A:");			  //����/��   
			display_6_8_number(40,2,point_A[0]);  //smartcar_imu.left_motor_speed_cmps EncoderA
			display_6_8_number(80,2,point_A[1]); 		

			display_6_8_string(0,3,"B:");			 
			display_6_8_number(40,3,point_B[0]);  
			display_6_8_number(80,3,point_B[1]); 	

			display_6_8_string(0,4,"C:");			 
			display_6_8_number(40,4,point_C[0]);  
			display_6_8_number(80,4,point_C[1]); 	
 
			display_6_8_string(0,5,"D:");			 
			display_6_8_number(40,5,point_D[0]);  
			display_6_8_number(80,5,point_D[1]); 		  

			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); //		
			
	display_6_8_number(80,6,Flag.Start_Car);
	switch(key_value)
	{
		case 0:
			display_6_8_string(50,6,"!A!");		
		break;
		case 1:
			display_6_8_string(50,6,"!C!");		
		break;		
		case 2:
			display_6_8_string(50,6,"!B!");		
		break;			
		case 3:
			display_6_8_string(50,6,"!D!");		
		break;			
		
	}
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				if(key_value < 3)
				key_value++;
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				if(key_value > 0)
					key_value--;
			break;
//			case KEY4_CLICK_RELEASE:		//�¼� ��С����ֵ
//				point_C[0] = point_actual[0];
//				point_C[1] = point_actual[1];
//			break;			
			case KEY5_CLICK_RELEASE:		//�м� ��С����ֵ

			switch(key_value)
			{
				case 0:
				point_A[0] = point_actual[0];
				point_A[1] = point_actual[1];			

				data_test.value = point_actual[0];
				Storage_WriteFloatNum(0*4,data_test);		
					data_test.value = point_actual[1];
				Storage_WriteFloatNum(1*4,data_test);			
					break;
				case 2:
				point_B[0] = point_actual[0];
				point_B[1] = point_actual[1];	
				
				data_test.value = point_actual[0];
				Storage_WriteFloatNum(2*4,data_test);		
				data_test.value = point_actual[1];
				Storage_WriteFloatNum(3*4,data_test);					
					break;		
				case 1:
				point_C[0] = point_actual[0];
				point_C[1] = point_actual[1];			
				data_test.value = point_actual[0];
				Storage_WriteFloatNum(4*4,data_test);		
				data_test.value = point_actual[1];
				Storage_WriteFloatNum(5*4,data_test);					
					break;			
				case 3:
				point_D[0] = point_actual[0];
				point_D[1] = point_actual[1];
				data_test.value = point_actual[0];				
				Storage_WriteFloatNum(6*4,data_test);		
				data_test.value = point_actual[1];
				Storage_WriteFloatNum(7*4,data_test);					
					break;	
				default:
					break;
			}
				key_value++;
			  key_value %= 4;
//				point_D[0] = point_actual[0];
//				point_D[1] = point_actual[1];
//				data_test.value = point_actual[0];
//				Storage_WriteFloatNum(0,data_test);		
			
			break;	
			
		}
	}	
}
static void stg2Menu_Para6CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para6:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //�м��̰�
				//display_6_8_string(5,4,"TASK2 Action!");

			break;				
		}
	}		

}
static void stg2Menu_Para7CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para6:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //�м��̰�
				//display_6_8_string(5,4,"TASK2 Action!");

			break;				
		}
	}		

}

static void stg3Menu_Para1CBS(void)
{
	unsigned char keys;
	
	//hal_Oled_ShowString(10,38,"kp2:",8,1);
	//hal_Oled_ShowNum(38, 38, kp2, 2,8,1);

	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;

	}	

			display_6_8_number(40,2,point_A[0]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_A[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_A[0] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
//			case KEY4_CLICK_RELEASE:		//�¼� ��С����ֵ
//				point_C[0] = point_actual[0];
//				point_C[1] = point_actual[1];
//			break;			
//			case KEY5_CLICK_RELEASE:		//�м� ��С����ֵ
//				point_D[0] = point_actual[0];
//				point_D[1] = point_actual[1];
////				data_test.value = point_actual[0];
////				Storage_WriteFloatNum(0,data_test);		
//			
//			break;			
			
		}
				data_test.value = point_A[0];
				Storage_WriteFloatNum(0*4,data_test);	
	}
}


static void stg3Menu_Para2CBS(void)
{
	unsigned char keys;
	
	//hal_Oled_ShowString(10,38,"kp3:",8,1);
//	hal_Oled_ShowNum(38, 38, kp3, 2,8,1);
//	hal_Oled_Refresh();
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);		
		
		keys = 0xFF;
		 
	}	
//			display_6_8_string(0,2,"A:");			  //����/��   
			display_6_8_number(40,2,point_A[1]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_A[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_A[1] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		
		}
				data_test.value = point_A[1];
				Storage_WriteFloatNum(1*4,data_test);	
	}	
}

static void stg3Menu_Para3CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);		
		
		keys = 0xFF;
		 
	}		
			display_6_8_number(40,2,point_B[0]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_B[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_B[0] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		
		}
				data_test.value = point_B[0];
				Storage_WriteFloatNum(2*4,data_test);			
	}	
}

static void stg3Menu_Para4CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
//		display_6_8_string(0,2,"stg3Para4:");
				 
	}		
			display_6_8_number(40,2,point_B[1]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_B[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_B[1] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		
		}
				data_test.value = point_B[1];
				Storage_WriteFloatNum(3*4,data_test);			
	}

}

static void stg3Menu_Para5CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
//		display_6_8_string(0,2,"stg3Para5:");
				 
	}	
			display_6_8_number(40,2,point_C[0]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_C[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_C[0] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		}
				data_test.value = point_C[0];
				Storage_WriteFloatNum(4*4,data_test);			
	}
}
static void stg3Menu_Para6CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				 
	}	
			display_6_8_number(40,2,point_C[1]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_C[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_C[1] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		}
				data_test.value = point_C[1];
				Storage_WriteFloatNum(5*4,data_test);			
	}
}
static void stg3Menu_Para7CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
//		display_6_8_string(0,2,"stg3Para5:");
				 
	}	
			display_6_8_number(40,2,point_D[0]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_D[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_D[0] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		}
				data_test.value = point_D[0];
				Storage_WriteFloatNum(6*4,data_test);			
	}
}

static void stg3Menu_Para8CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//����
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				 
	}	
			display_6_8_number(40,2,point_D[1]);  //smartcar_imu.left_motor_speed_cmps EncoderA
//			display_6_8_number(80,1,point_A[1]); 		

//			display_6_8_string(0,2,"B:");			 
//			display_6_8_number(40,2,point_B[0]);  
//			display_6_8_number(80,2,point_B[1]); 	

//			display_6_8_string(0,3,"C:");			 
//			display_6_8_number(40,3,point_C[0]);  
//			display_6_8_number(80,3,point_C[1]); 	
// 
//			display_6_8_string(0,4,"D:");			 
//			display_6_8_number(40,4,point_D[0]);  
//			display_6_8_number(80,4,point_D[1]); 		  

		//point_actual[1]
			display_6_8_number(40,7,point_actual[0]);  
			display_6_8_number(80,7,point_actual[1]); 	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//�ָ��˵�����ֵ
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//��� ȡ��
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//��� ��������
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//�ϼ� �������ֵ
				point_D[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//�¼� ��С����ֵ
				point_D[1] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
		}
				data_test.value = point_D[1];
				Storage_WriteFloatNum(7*4,data_test);			
	}	
}
void AppProc(void)
{
	pModeMenu->action();//ִ�е�ǰ�˵��ķ�����������ѭ��
	//���д����ڵ�ǰ�˵���λ�õ�ͬ��gnlMenu_Desktop1CBS();
}
//-----------------������ص���������------------------------


//�����ص�����
static void KeyEventHandle(KEY_VALUE_TYPEDEF keys)
{
	pModeMenu->keyVal = keys;//����ֵ��ֵ����ǰ�˵��ļ�ֵ
	
}
