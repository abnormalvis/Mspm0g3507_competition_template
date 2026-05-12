/*******************************************************************************
  * @作者      ： wangming
  * @wechat    :DeepCoderMing
  * @qq      ： 3201935299
  * @日期      ： 2025年05月01日
  * @版权声明  ： 仅供参考学习，未经允许禁止商用
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
stu_mode_menu *pModeMenu;		//系统当前执行菜单的结构体指针 

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

/***参数**/
//菜单唯一ID号
//当前菜单的位置信息
//指向当前模式类型
//当前模式下的响应函数
//刷新屏显示命令
//预留，方便参数传递
//按键值,0xFF代表无按键触发
//指向上一个选项
//指向下一个选项
//指向父级菜单
//指向子级菜单
 
//初始化桌面菜单（一级菜单） 普通模式菜单
stu_mode_menu generalModeMenu[GNL_MENU_SUM] = //定义结构体数组，并且进行初始化 
{
	{GNL_MENU_DESKTOP1,DESKTOP_MENU_POS,"Desktop1",gnlMenu_Desktop1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{GNL_MENU_DESKTOP2,DESKTOP_MENU_POS,"Desktop2",gnlMenu_Desktop2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{GNL_MENU_DESKTOP3,DESKTOP_MENU_POS,"Desktop3",gnlMenu_Desktop3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	//菜单唯一ID号  菜单位置信息（是枚举变量）字符指针 桌面菜单的处理函数       
};	

//（1）初始化设置菜单（一级菜单） 结构体数组 设置菜单包含了所有列表
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

//（2）初始化设置菜单（一级菜单） 结构体数组 设置菜单包含了所有列表
stu_mode_menu settingModeMenu2[STG2_MENU_SUM] = 
{
	{STG2_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu2",stg2Menu_MainMenuCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA1,STG_SUB_2_MENU_POS,"1. task1",stg2Menu_Para1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//复位任务
	{STG2_MENU_PARA2,STG_SUB_2_MENU_POS,"2. task2",stg2Menu_Para2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//沿铅笔框边线顺时针一周
	{STG2_MENU_PARA3,STG_SUB_2_MENU_POS,"3. task3",stg2Menu_Para3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0}, //沿胶布边线顺时针一周
	{STG2_MENU_PARA4,STG_SUB_2_MENU_POS,"4. task4 ",stg2Menu_Para4CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"5. Dire ",stg2Menu_Para5CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"6. Clear ",stg2Menu_Para6CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"7. Orignal ",stg2Menu_Para7CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
};

//（2）初始化设置菜单（一级菜单） 结构体数组 设置菜单包含了所有列表
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
	menuInit();    //必须要初始化，才能将菜单显示为桌面菜单

	hal_KeyScanCBSRegister(KeyEventHandle);

}

//菜单初始化函数 实现菜单双向链表功能
static void menuInit(void)
{
	unsigned char i;
	/*！！！以下代码顺序不能调换！！！*/
	//把第二级子菜单列表形成循环双向链表形式，方便调用
	//特殊的头和尾进行连接 
	settingModeMenu1[1].pLase = &settingModeMenu1[STG1_MENU_SUM-1];	//settingModeMenu1[1].pLase的上一个选项
	settingModeMenu1[1].pNext = &settingModeMenu1[2];	//settingModeMenu1[1].pNext的下一个选项
	settingModeMenu1[1].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent的父级菜单

	//从设置菜单的第二个开始连接 第一个是主菜单页面 上一个选项 下一个选项 父级菜单
	for(i=2; i<STG1_MENU_SUM-1; i++)
	{
		settingModeMenu1[i].pLase = &settingModeMenu1[i-1];
		settingModeMenu1[i].pNext = &settingModeMenu1[i+1];
		settingModeMenu1[i].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];
	}	
	
	//特殊的尾和头进行连接 设置菜单的最后一个选项 的上一个选项 下一个选项 父级菜单
	settingModeMenu1[STG1_MENU_SUM-1].pLase = &settingModeMenu1[i-1];
	settingModeMenu1[STG1_MENU_SUM-1].pNext = &settingModeMenu1[1];
	settingModeMenu1[STG1_MENU_SUM-1].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];	
	//以上代码实现循环双向链表,顺序不能调换！！！！！	
	/*！！！以上代码顺序不能调换！！！*/

	/*！！！以下代码顺序不能调换！！！*/
	//把第二级子菜单列表形成循环双向链表形式，方便调用
	//特殊的头和尾进行连接 
	settingModeMenu2[1].pLase = &settingModeMenu2[STG2_MENU_SUM-1];	//settingModeMenu1[1].pLase的上一个选项
	settingModeMenu2[1].pNext = &settingModeMenu2[2];	//settingModeMenu1[1].pNext的下一个选项
	settingModeMenu2[1].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent的父级菜单

	//从设置菜单的第二个开始连接 第一个是主菜单页面 上一个选项 下一个选项 父级菜单
	for(i=2; i<STG2_MENU_SUM-1; i++)
	{
		settingModeMenu2[i].pLase = &settingModeMenu2[i-1];
		settingModeMenu2[i].pNext = &settingModeMenu2[i+1];
		settingModeMenu2[i].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];
	}	
	
	//特殊的尾和头进行连接 设置菜单的最后一个选项 的上一个选项 下一个选项 父级菜单
	settingModeMenu2[STG2_MENU_SUM-1].pLase = &settingModeMenu2[i-1];
	settingModeMenu2[STG2_MENU_SUM-1].pNext = &settingModeMenu2[1];
	settingModeMenu2[STG2_MENU_SUM-1].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];	
	//以上代码实现循环双向链表,顺序不能调换！！！！！	
	/*！！！以上代码顺序不能调换！！！*/
	
	/*！！！以下代码顺序不能调换！！！*/
	//把第二级子菜单列表形成循环双向链表形式，方便调用
	//特殊的头和尾进行连接 
	settingModeMenu3[1].pLase = &settingModeMenu3[STG3_MENU_SUM-1];	//settingModeMenu1[1].pLase的上一个选项
	settingModeMenu3[1].pNext = &settingModeMenu3[2];	//settingModeMenu1[1].pNext的下一个选项
	settingModeMenu3[1].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent的父级菜单

	//从设置菜单的第二个开始连接 第一个是主菜单页面 上一个选项 下一个选项 父级菜单
	for(i=2; i<STG3_MENU_SUM-1; i++)
	{
		settingModeMenu3[i].pLase = &settingModeMenu3[i-1];
		settingModeMenu3[i].pNext = &settingModeMenu3[i+1];
		settingModeMenu3[i].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];
	}	
	
	//特殊的尾和头进行连接 设置菜单的最后一个选项 的上一个选项 下一个选项 父级菜单
	settingModeMenu3[STG3_MENU_SUM-1].pLase = &settingModeMenu3[i-1];
	settingModeMenu3[STG3_MENU_SUM-1].pNext = &settingModeMenu3[1];
	settingModeMenu3[STG3_MENU_SUM-1].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];	
	//以上代码实现循环双向链表,顺序不能调换！！！！！	
	/*！！！以上代码顺序不能调换！！！*/	
	
	
	//系统当前执行菜单的结构体指针指向 普通模式的桌面菜单 然后就可以改变普通模式的桌面菜单里的参数
	pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];	//设置上电显示的菜单界面为桌面显示
	pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;	//更新刷新界面标志，进入界面后刷新全界面UI
	//上行代码其实不写在这个初始化函数中也是这个值，因为定义普通模式的桌面菜单时就已经将它初始化为了SCREEN_CMD_RESET
}


int key_count;    //按键值
int task_num = 0;
char stop_task = 0;

float pwm_x0 = 1950,pwm_y0 = 1050;	//复位位置的pwm
float pwm_x1 = 1950,pwm_y1 = 1050;	//周边左上角点的pwm
float pwm_x2 = 1950,pwm_y2 = 1050;	//周边右下角点的pwm
char Beep_toggle_flag = 0;
//桌面1菜单服务函数
static void gnlMenu_Desktop1CBS(void)
{
	unsigned char keys;
	static uint8_t gray_display_state = 1;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)//状态为刷新时满足条件
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;//下次不刷新了，刷新一次
		pModeMenu->keyVal = 0xFF;//初始化为没有按键按下
		 
		hal_Oled_Clear();
		//display_6_8_string(110,0,"M1");
	}

	switch(gray_display_state)
	{	
		case 1:
			gray_display_state++;
			LCD_clear_L(0,1);
			display_6_8_string(0,1,"cmps:");			  //厘米/秒   
			display_6_8_number(40,1,smartcar_imu.left_motor_speed_cmps);  //smartcar_imu.left_motor_speed_cmps EncoderA
			display_6_8_number(85,1,smartcar_imu.right_motor_speed_cmps); 		
	
			break;		
		case 2:
			gray_display_state++;
			LCD_clear_L(0,2);
		//distance_l
			display_6_8_string(0,2,"dtas:");			  //厘米/秒   
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
	//以下代码在桌面菜单服务函数中，表示在桌面菜单下长按中键才会进入主界面
	if(pModeMenu->keyVal != 0xff)//有按键触发
	{
		keys = pModeMenu->keyVal; //keys是这个函数里的临时变量

		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值 keys中有值了
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu1[0];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	//右键
				pModeMenu = &generalModeMenu[1];	//跳转到桌面摄像头界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_CLICK_RELEASE:	//左键
				pModeMenu = &generalModeMenu[2];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:	//上键 启动发车
						Flag.Start_Car = 1;
			break;				
			case KEY2_CLICK_RELEASE:	//上键 启动发车
						Flag.Start_Car = 0;			
			break;				
		}
	}
	
}

//桌面2菜单服务函数
static void gnlMenu_Desktop2CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)//状态为刷新时满足条件
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;//下次不刷新了，刷新一次
		pModeMenu->keyVal = 0xFF;//初始化为没有按键按下
		 
		hal_Oled_Clear();
		//display_6_8_string(110,0,"M2");

		
	}
	display_6_8_string(50,4,"TASK");
	//以下代码在桌面菜单服务函数中，表示在桌面菜单下长按中键才会进入主界面
	if(pModeMenu->keyVal != 0xff)//有按键触发
	{
		keys = pModeMenu->keyVal; //keys是这个函数里的临时变量

		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值 keys中有值了
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu2[0];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
				
			break;
			case KEY3_CLICK_RELEASE:	//左键
				pModeMenu = &generalModeMenu[0];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	//右键
				pModeMenu = &generalModeMenu[2];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:	//上键 启动发车
						
			break;			
			
		}
	}
	
}

//桌面3控制菜单服务函数
static void gnlMenu_Desktop3CBS(void)
{
	unsigned char keys;
	static uint8_t gray_display_state = 1;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)//状态为刷新时满足条件
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;//下次不刷新了，刷新一次
		pModeMenu->keyVal = 0xFF;//初始化为没有按键按下
		 
		hal_Oled_Clear();
	//	display_6_8_string(110,0,"M3");

	}
	switch(gray_display_state)
	{	
		case 1:
			gray_display_state++;
			LCD_clear_L(0,1);
			display_6_8_string(0,1,"pointA:");			  //厘米/秒   
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
	//以下代码在桌面菜单服务函数中，表示在桌面菜单下长按中键才会进入主界面
	if(pModeMenu->keyVal != 0xff)//有按键触发
	{
		keys = pModeMenu->keyVal; //keys是这个函数里的临时变量

		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值 keys中有值了
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu3[0];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_CLICK_RELEASE:	//左键
				pModeMenu = &generalModeMenu[1];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	//右键
				pModeMenu = &generalModeMenu[0];	//跳转到设置菜单界面
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
		}
	}
	
}

//主菜单1处理函数
static void stg1Menu_MainMenuCBS(void)
{
	//pMenu  MHead  MTail是独立的结构体指针，分配了独立的资源
	//static定义的变量，下次再回到这个函数中，变量的值保持上一个不变
	unsigned char keys;//键值临时储存的变量
	unsigned char i;//用于for循环
	unsigned char ClrScreenFlag;//要不要去刷屏的标志位，翻页的时候刷屏，清楚列表区域的显示，重新显示，不翻页的话不用刷屏
	static stu_mode_menu *pMenu;		//用来保存当前选中的菜单
	static stu_mode_menu *bpMenu=0;		//用来备份上一次菜单选项，主要用于刷屏判断，当pMenu！= bpMenu时，刷屏
	static unsigned char stgMainMenuSelectedPos=0;	//用来记录当前选中菜单的位置，选择列表中选中菜单的位置，默认等于1，从第一个菜单开始
	static stu_mode_menu *MHead,*MTail;		//这两个结构体指针是为了上下切换菜单时做翻页处理
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//需要刷新屏幕
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//下次进来这个函数不刷新了，只刷新一次，长按中键后加入主页面刷新一次
		pMenu = &settingModeMenu1[0];//执行主菜单
		hal_Oled_Clear();//刷屏
		
		//display_6_8_string(10,0,pMenu->pModeType);//将要显示的数据写入oled缓存，oled还不会更新显示，要运行下一步刷新

		
	//刷屏和更新显示费时间，只有键值改变的时候才进行
		
		pMenu = &settingModeMenu1[1];	//设置菜单列表的第一个选项
		
		MHead = pMenu;			//记录当前设置菜单第一项
		MTail = pMenu+6;		//记录当前设置菜单最后一项,一页显示4个选项，共5个选项
		bpMenu = 0;	//bpMenu与pMenu不相等，进入下面的ui显示程序
 
		ClrScreenFlag = 1;	//写成1，使得第一次进入下面的显示程序时，把菜单列表显示出来，否则选择列表显示不出来
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//按键操作菜单 包括翻页操作
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//上
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//取消选中本菜单显示
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//下
				if(stgMainMenuSelectedPos == 7)///翻页：选中第4个菜单按下键时
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos最大是4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//取消选中本菜单显示
					pMenu = pMenu->pNext;																			//切换下一个选项
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	//右键 确定
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			
		}
	}
	
	//以下代码用于UI显示，显示主页面的菜单列表
	if(bpMenu != pMenu)//如果指向地址不一致，需要更新显示，上面已经强制地址不一致了
	{
		bpMenu = pMenu;//bpMenu更新为现在菜单状态，若pMenu改变，则bpMenu != pMenu，表示有按键按下，要更新屏幕显示
		if(ClrScreenFlag)//刷屏标志位，翻页的时候起作用，翻页的时候要清屏
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		//清屏 清除列表的那块区域

			for(i=1; i<8; i++)	//这里一定是i<5，因为一页只能显示4个菜单列表
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//循环显示菜单列表的名字

				pMenu = pMenu->pNext;//下一个
			} 
			pMenu = bpMenu;//将pMenu指向&settingModeMenu1[1];指向第一个菜单，前面for循环中pMenu = pMenu->pNext;pMenu一直变化
			//以下代码为光标显示的位置
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//反色显示
		 
		}else//不需要清屏，不翻页的时候
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//反色显示

		}	
			
				 
	}
}
//主菜单2处理函数
static void stg2Menu_MainMenuCBS(void)
{
	//pMenu  MHead  MTail是独立的结构体指针，分配了独立的资源
	//static定义的变量，下次再回到这个函数中，变量的值保持上一个不变
	unsigned char keys;//键值临时储存的变量
	unsigned char i;//用于for循环
	unsigned char ClrScreenFlag;//要不要去刷屏的标志位，翻页的时候刷屏，清楚列表区域的显示，重新显示，不翻页的话不用刷屏
	static stu_mode_menu *pMenu;		//用来保存当前选中的菜单
	static stu_mode_menu *bpMenu=0;		//用来备份上一次菜单选项，主要用于刷屏判断，当pMenu！= bpMenu时，刷屏
	static unsigned char stgMainMenuSelectedPos=0;	//用来记录当前选中菜单的位置，选择列表中选中菜单的位置，默认等于1，从第一个菜单开始
	static stu_mode_menu *MHead,*MTail;		//这两个结构体指针是为了上下切换菜单时做翻页处理
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//需要刷新屏幕
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//下次进来这个函数不刷新了，只刷新一次，长按中键后加入主页面刷新一次
		pMenu = &settingModeMenu2[0];//执行主菜单
		hal_Oled_Clear();//刷屏
		
		//display_6_8_string(37,0,pMenu->pModeType);//将要显示的数据写入oled缓存，oled还不会更新显示，要运行下一步刷新
		
	//刷屏和更新显示费时间，只有键值改变的时候才进行
		
		pMenu = &settingModeMenu2[1];	//设置菜单列表的第一个选项
		
		MHead = pMenu;			//记录当前设置菜单第一项
		MTail = pMenu+6;		//记录当前设置菜单最后一项,一页显示4个选项，共5个选项
		bpMenu = 0;	//bpMenu与pMenu不相等，进入下面的ui显示程序
 
		ClrScreenFlag = 1;	//写成1，使得第一次进入下面的显示程序时，把菜单列表显示出来，否则选择列表显示不出来
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//按键操作菜单 包括翻页操作
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//上
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//取消选中本菜单显示
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//下
				if(stgMainMenuSelectedPos == 7)///翻页：选中第4个菜单按下键时
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos最大是4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//取消选中本菜单显示
					pMenu = pMenu->pNext;																			//切换下一个选项
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	//右键 确定
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			case KEY5_CLICK_RELEASE:	//中键 确定
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
	
	//以下代码用于UI显示，显示主页面的菜单列表
	if(bpMenu != pMenu)//如果指向地址不一致，需要更新显示，上面已经强制地址不一致了
	{
		bpMenu = pMenu;//bpMenu更新为现在菜单状态，若pMenu改变，则bpMenu != pMenu，表示有按键按下，要更新屏幕显示
		if(ClrScreenFlag)//刷屏标志位，翻页的时候起作用，翻页的时候要清屏
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		//清屏 清除列表的那块区域
			for(i=1; i<8; i++)	//这里一定是i<5，因为一页只能显示4个菜单列表
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//循环显示菜单列表的名字
				pMenu = pMenu->pNext;//下一个
			} 
			pMenu = bpMenu;//将pMenu指向&settingModeMenu1[1];指向第一个菜单，前面for循环中pMenu = pMenu->pNext;pMenu一直变化
			//以下代码为光标显示的位置
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//反色显示
		}else//不需要清屏，不翻页的时候
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//反色显示
		}	
			
				 
	}
}
//主菜单3处理函数
static void stg3Menu_MainMenuCBS(void)
{
	//pMenu  MHead  MTail是独立的结构体指针，分配了独立的资源
	//static定义的变量，下次再回到这个函数中，变量的值保持上一个不变
	unsigned char keys;//键值临时储存的变量
	unsigned char i;//用于for循环
	unsigned char ClrScreenFlag;//要不要去刷屏的标志位，翻页的时候刷屏，清楚列表区域的显示，重新显示，不翻页的话不用刷屏
	static stu_mode_menu *pMenu;		//用来保存当前选中的菜单
	static stu_mode_menu *bpMenu=0;		//用来备份上一次菜单选项，主要用于刷屏判断，当pMenu！= bpMenu时，刷屏
	static unsigned char stgMainMenuSelectedPos=0;	//用来记录当前选中菜单的位置，选择列表中选中菜单的位置，默认等于1，从第一个菜单开始
	static stu_mode_menu *MHead,*MTail;		//这两个结构体指针是为了上下切换菜单时做翻页处理
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//需要刷新屏幕
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//下次进来这个函数不刷新了，只刷新一次，长按中键后加入主页面刷新一次
		pMenu = &settingModeMenu3[0];//执行主菜单
		hal_Oled_Clear();//刷屏
		
		//display_6_8_string(37,0,pMenu->pModeType);//将要显示的数据写入oled缓存，oled还不会更新显示，要运行下一步刷新
		
	//刷屏和更新显示费时间，只有键值改变的时候才进行
		
		pMenu = &settingModeMenu3[1];	//设置菜单列表的第一个选项
		
		MHead = pMenu;			//记录当前设置菜单第一项
		MTail = pMenu+6;		//记录当前设置菜单最后一项,一页显示4个选项，共5个选项		这个加值3有什么用？？
		bpMenu = 0;	//bpMenu与pMenu不相等，进入下面的ui显示程序
 
		ClrScreenFlag = 1;	//写成1，使得第一次进入下面的显示程序时，把菜单列表显示出来，否则选择列表显示不出来
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//按键操作菜单 包括翻页操作
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//上
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//取消选中本菜单显示
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//下
				if(stgMainMenuSelectedPos == 7)///翻页：选中第4个菜单按下键时
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos最大是4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//取消选中本菜单显示
					pMenu = pMenu->pNext;																			//切换下一个选项
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	//右键 确定
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			
		}
	}
	
	//以下代码用于UI显示，显示主页面的菜单列表
	if(bpMenu != pMenu)//如果指向地址不一致，需要更新显示，上面已经强制地址不一致了
	{
		bpMenu = pMenu;//bpMenu更新为现在菜单状态，若pMenu改变，则bpMenu != pMenu，表示有按键按下，要更新屏幕显示
		if(ClrScreenFlag)//刷屏标志位，翻页的时候起作用，翻页的时候要清屏
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		//清屏 清除列表的那块区域
			for(i=1; i<8; i++)	//这里一定是i<5，因为一页只能显示4个菜单列表
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//循环显示菜单列表的名字
				pMenu = pMenu->pNext;//下一个
			} 
			pMenu = bpMenu;//将pMenu指向&settingModeMenu1[1];指向第一个菜单，前面for循环中pMenu = pMenu->pNext;pMenu一直变化
			//以下代码为光标显示的位置
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//反色显示
		}else//不需要清屏，不翻页的时候
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//反色显示
		}	
			
				 
	}
}

static void stg1Menu_Para1CBS(void)
{
	unsigned char keys;
	
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	//清屏
		
		//hal_Oled_ShowString(20,0,"1. Inductance",12,1);
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		
		keys = 0xFF;
		 
	}	
	//LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_out_limH); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				pos_out_limH+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_out_limL); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				pos_out_limL+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,speed_adjust); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				speed_adjust+=1;
				if(speed_adjust > 50)
					speed_adjust = 50;
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,tar_theta_limit); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				tar_theta_limit+=1;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_boudary); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				pos_boudary+=1;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,turn_kp); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				turn_kp+=0.2;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,sleep_time); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				sleep_time+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,yaw_track_kp); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				yaw_track_kp+=0.01;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
				yaw_track_kp-=0.01;

			break;	//
			case KEY1_LONG_PRESS_CONTINUOUS:		//上键 增大参数值
				yaw_track_kp+=0.01;
	
			break;			
			case KEY2_LONG_PRESS_CONTINUOUS:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,yaw_track_kd); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				yaw_track_kd+=0.01;
	
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
				yaw_track_kd-=0.01;

			break;	//
			case KEY1_LONG_PRESS_CONTINUOUS:		//上键 增大参数值
				yaw_track_kd+=0.01;
	
			break;			
			case KEY2_LONG_PRESS_CONTINUOUS:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"TASK1:");
		
		keys = 0xFF;
		 
	}	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
				
			break;
			case KEY5_CLICK_RELEASE: //中键短按
//				Time0A_init(5,USER_INT0);   //有正确PWM时候，打开舵机
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"TASK2:");
		
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,6);
	//display_6_8_number(60,6,Param.Send_Step); 


	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
				
			break;
			case KEY5_CLICK_RELEASE: //中键短按
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"stg2Para3:");

		keys = 0xFF;
		 
	}	

	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
//				kp4++;
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
//				kp4--;
			break;
			case KEY5_CLICK_RELEASE: //中键短按
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para4:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		//上键 增大参数值
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		//下键 减小参数值
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //中键短按
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		//display_6_8_string(0,2,"stg2Para5:");
				 
	}	
	Flag.Start_Car = 0;
	
			display_6_8_string(0,2,"A:");			  //厘米/秒   
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				if(key_value < 3)
				key_value++;
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
				if(key_value > 0)
					key_value--;
			break;
//			case KEY4_CLICK_RELEASE:		//下键 减小参数值
//				point_C[0] = point_actual[0];
//				point_C[1] = point_actual[1];
//			break;			
			case KEY5_CLICK_RELEASE:		//中键 减小参数值

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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para6:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		//上键 增大参数值
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		//下键 减小参数值
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //中键短按
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para6:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		//上键 增大参数值
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		//下键 减小参数值
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //中键短按
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_A[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
				point_A[0] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
//			case KEY4_CLICK_RELEASE:		//下键 减小参数值
//				point_C[0] = point_actual[0];
//				point_C[1] = point_actual[1];
//			break;			
//			case KEY5_CLICK_RELEASE:		//中键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);		
		
		keys = 0xFF;
		 
	}	
//			display_6_8_string(0,2,"A:");			  //厘米/秒   
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_A[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_B[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_B[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_C[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_C[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_D[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
		hal_Oled_Clear();	//清屏
		
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
		pModeMenu->keyVal = 0xFF;	//恢复菜单按键值
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	//左键 取消
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		//左键 返回桌面
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		//上键 增大参数值
				point_D[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		//下键 减小参数值
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
	pModeMenu->action();//执行当前菜单的服务函数，不断循环
	//上行代码在当前菜单的位置等同于gnlMenu_Desktop1CBS();
}
//-----------------驱动层回调处理函数------------------------


//按键回调函数
static void KeyEventHandle(KEY_VALUE_TYPEDEF keys)
{
	pModeMenu->keyVal = keys;//将键值赋值给当前菜单的键值
	
}
