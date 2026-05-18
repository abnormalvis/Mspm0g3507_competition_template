/*******************************************************************************
  * @author      :  wangming
   * @wechat     : DeepCoderMing
  * @qq      :  3201935299
  * @date      :  2025-05-01
  * @copyright  :  For reference only, commercial use prohibited
********************************************************************************/
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_timera.h>
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
#include "hal_vofa.h"
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
stu_mode_menu *pModeMenu;		//Current active menu struct pointer 

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

/***Parameters**/
//Menu unique ID
//Current menu position info
//Points to current mode type
//Current mode response function
//Screen refresh command
//Reserved for parameter passing
//Key value, 0xFF = no key triggered
//Points to previous option
//Points to next option
//Points to parent menu
//Points to child menu
 
//Init desktop menu (L1 menu): general mode menu
stu_mode_menu generalModeMenu[GNL_MENU_SUM] = //Define struct array and initialize 
{
	{GNL_MENU_DESKTOP1,DESKTOP_MENU_POS,"Desktop1",gnlMenu_Desktop1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{GNL_MENU_DESKTOP2,DESKTOP_MENU_POS,"Desktop2",gnlMenu_Desktop2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{GNL_MENU_DESKTOP3,DESKTOP_MENU_POS,"Desktop3",gnlMenu_Desktop3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	//Menu unique ID  | position info (enum) | string ptr | Desktop menu handler       
};	

//Init settings menu (L1 menu) - struct array, contains all lists
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

//Init settings menu (L1 menu) - struct array, contains all lists
stu_mode_menu settingModeMenu2[STG2_MENU_SUM] = 
{
	{STG2_MENU_MAIN_SETTING,STG_MENU_POS,"Main Menu2",stg2Menu_MainMenuCBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA1,STG_SUB_2_MENU_POS,"1. task1",stg2Menu_Para1CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//Reset task
	{STG2_MENU_PARA2,STG_SUB_2_MENU_POS,"2. task2",stg2Menu_Para2CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	//Follow pencil frame border clockwise
	{STG2_MENU_PARA3,STG_SUB_2_MENU_POS,"3. task3",stg2Menu_Para3CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0}, //Follow tape border clockwise
	{STG2_MENU_PARA4,STG_SUB_2_MENU_POS,"4. task4 ",stg2Menu_Para4CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"5. Dire ",stg2Menu_Para5CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"6. Clear ",stg2Menu_Para6CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},
	{STG2_MENU_PARA5,STG_SUB_2_MENU_POS,"7. Orignal ",stg2Menu_Para7CBS,SCREEN_CMD_RESET,0,0xFF,0,0,0,0},	
};

//Init settings menu (L1 menu) - struct array, contains all lists
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
	menuInit();    //Must init before displaying as desktop menu

	hal_KeyScanCBSRegister(KeyEventHandle);

}

//Menu init function - builds doubly linked list
static void menuInit(void)
{
	unsigned char i;
	/*!!! Code order below MUST NOT be changed !!!*/
	//Link L2 submenu list into circular doubly linked list
	//Connect special head and tail 
	settingModeMenu1[1].pLase = &settingModeMenu1[STG1_MENU_SUM-1];	//settingModeMenu1[1].pLase previous option
	settingModeMenu1[1].pNext = &settingModeMenu1[2];	//settingModeMenu1[1].pNext next option
	settingModeMenu1[1].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent parent menu

	//Connect from 2nd settings item; 1st is main page; prev option; next option; parent menu
	for(i=2; i<STG1_MENU_SUM-1; i++)
	{
		settingModeMenu1[i].pLase = &settingModeMenu1[i-1];
		settingModeMenu1[i].pNext = &settingModeMenu1[i+1];
		settingModeMenu1[i].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];
	}	
	
	//Connect special tail and head (last settings item); prev option; next option; parent menu
	settingModeMenu1[STG1_MENU_SUM-1].pLase = &settingModeMenu1[i-1];
	settingModeMenu1[STG1_MENU_SUM-1].pNext = &settingModeMenu1[1];
	settingModeMenu1[STG1_MENU_SUM-1].pParent = &settingModeMenu1[STG1_MENU_MAIN_SETTING];	
	//Above code builds circular doubly linked list; order MUST NOT change!!!!!	
	/*!!! Code order above MUST NOT be changed !!!*/

	/*!!! Code order below MUST NOT be changed !!!*/
	//Link L2 submenu list into circular doubly linked list
	//Connect special head and tail 
	settingModeMenu2[1].pLase = &settingModeMenu2[STG2_MENU_SUM-1];	//settingModeMenu1[1].pLase previous option
	settingModeMenu2[1].pNext = &settingModeMenu2[2];	//settingModeMenu1[1].pNext next option
	settingModeMenu2[1].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent parent menu

	//Connect from 2nd settings item; 1st is main page; prev option; next option; parent menu
	for(i=2; i<STG2_MENU_SUM-1; i++)
	{
		settingModeMenu2[i].pLase = &settingModeMenu2[i-1];
		settingModeMenu2[i].pNext = &settingModeMenu2[i+1];
		settingModeMenu2[i].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];
	}	
	
	//Connect special tail and head (last settings item); prev option; next option; parent menu
	settingModeMenu2[STG2_MENU_SUM-1].pLase = &settingModeMenu2[i-1];
	settingModeMenu2[STG2_MENU_SUM-1].pNext = &settingModeMenu2[1];
	settingModeMenu2[STG2_MENU_SUM-1].pParent = &settingModeMenu2[STG2_MENU_MAIN_SETTING];	
	//Above code builds circular doubly linked list; order MUST NOT change!!!!!	
	/*!!! Code order above MUST NOT be changed !!!*/
	
	/*!!! Code order below MUST NOT be changed !!!*/
	//Link L2 submenu list into circular doubly linked list
	//Connect special head and tail 
	settingModeMenu3[1].pLase = &settingModeMenu3[STG3_MENU_SUM-1];	//settingModeMenu1[1].pLase previous option
	settingModeMenu3[1].pNext = &settingModeMenu3[2];	//settingModeMenu1[1].pNext next option
	settingModeMenu3[1].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];	//settingModeMenu1[1].pParent parent menu

	//Connect from 2nd settings item; 1st is main page; prev option; next option; parent menu
	for(i=2; i<STG3_MENU_SUM-1; i++)
	{
		settingModeMenu3[i].pLase = &settingModeMenu3[i-1];
		settingModeMenu3[i].pNext = &settingModeMenu3[i+1];
		settingModeMenu3[i].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];
	}	
	
	//Connect special tail and head (last settings item); prev option; next option; parent menu
	settingModeMenu3[STG3_MENU_SUM-1].pLase = &settingModeMenu3[i-1];
	settingModeMenu3[STG3_MENU_SUM-1].pNext = &settingModeMenu3[1];
	settingModeMenu3[STG3_MENU_SUM-1].pParent = &settingModeMenu3[STG3_MENU_MAIN_SETTING];	
	//Above code builds circular doubly linked list; order MUST NOT change!!!!!	
	/*!!! Code order above MUST NOT be changed !!!*/	
	
	
	//Point current menu ptr to general mode desktop menu so params can be changed
	pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];	//Set power-on default display to desktop
	pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;	//Update refresh flag to refresh full UI on entry
	//Above is redundant here since desktop menu definition already initializes it to SCREEN_CMD_RESET
}


int key_count;    //Key value
int task_num = 0;
char stop_task = 0;

float pwm_x0 = 1950,pwm_y0 = 1050;	//Reset position PWM
float pwm_x1 = 1950,pwm_y1 = 1050;	//Perimeter top-left corner PWM
float pwm_x2 = 1950,pwm_y2 = 1050;	//Perimeter bottom-right corner PWM
char Beep_toggle_flag = 0;
//Desktop1 menu service function
static void gnlMenu_Desktop1CBS(void)
{
	unsigned char keys;
	static uint8_t gray_display_state = 1;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)// Enter when refresh state
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;// Do not refresh next time, refresh once
		pModeMenu->keyVal = 0xFF;// Init: no key event
		 
		hal_Oled_Clear();
		//display_6_8_string(110,0,"M1");
	}

	switch(gray_display_state)
	{	
		case 1:
			gray_display_state++;
			LCD_clear_L(0,1);
			display_6_8_string(0,1,"cmps:");			  //cm/s   
				display_6_8_number(30,5,gray_status);
			display_6_8_number(40,1,smartcar_imu.left_motor_speed_cmps);  //smartcar_imu.left_motor_speed_cmps EncoderA
			display_6_8_number(85,1,smartcar_imu.right_motor_speed_cmps); 		
	
			break;		
		case 2:
			gray_display_state++;
			LCD_clear_L(0,2);
		//distance_l
			display_6_8_string(0,2,"dtas:");			  //cm/s   
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
				display_6_8_number(50,5,gray_threshold[0]);
		display_6_8_number(100,5,Flag.Start_Car); 
		//	display_6_8_number(60,5,Storage_ReadFloatNum(7).value); 
			break;
		case 6:
			gray_display_state++;
			LCD_clear_L(0,6);
			LCD_clear_L(50,6);
				display_6_8_number(30,5,gray_status);

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
	//In desktop menu service: long-press center key to enter main menu
	if(pModeMenu->keyVal != 0xff)//Key triggered
	{
		keys = pModeMenu->keyVal; //keys is a temp variable in this function

		pModeMenu->keyVal = 0xFF;	// Reset menu key value; keys now holds the value
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu1[0];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	// Right key
				pModeMenu = &generalModeMenu[1];	//Jump to desktop camera view
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_CLICK_RELEASE:	// Left key
				pModeMenu = &generalModeMenu[2];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:	// Up key, Start car
						Flag.Start_Car = 1;
			break;				
			case KEY2_CLICK_RELEASE:	// Up key, Start car
						Flag.Start_Car = 0;			
			break;				
		}
	}
	
}

//Desktop2 menu service function
static void gnlMenu_Desktop2CBS(void)
{
	unsigned char keys;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)// Enter when refresh state
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;// Do not refresh next time, refresh once
		pModeMenu->keyVal = 0xFF;// Init: no key event
		 
		hal_Oled_Clear();
		//display_6_8_string(110,0,"M2");

		
	}
	display_6_8_string(50,4,"TASK");
	//In desktop menu service: long-press center key to enter main menu
	if(pModeMenu->keyVal != 0xff)//Key triggered
	{
		keys = pModeMenu->keyVal; //keys is a temp variable in this function

		pModeMenu->keyVal = 0xFF;	// Reset menu key value; keys now holds the value
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu2[0];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
				
			break;
			case KEY3_CLICK_RELEASE:	// Left key
				pModeMenu = &generalModeMenu[0];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	// Right key
				pModeMenu = &generalModeMenu[2];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:	// Up key, Start car
						
			break;			
			
		}
	}
	
}

//Desktop3 control menu service function
static void gnlMenu_Desktop3CBS(void)
{
	unsigned char keys;
	static uint8_t gray_display_state = 1;
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)// Enter when refresh state
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;// Do not refresh next time, refresh once
		pModeMenu->keyVal = 0xFF;// Init: no key event
		 
		hal_Oled_Clear();
	//	display_6_8_string(110,0,"M3");

	}
	switch(gray_display_state)
	{	
		case 1:
			gray_display_state++;
			LCD_clear_L(0,1);
			display_6_8_string(0,1,"pointA:");			  //cm/s   
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
	//In desktop menu service: long-press center key to enter main menu
	if(pModeMenu->keyVal != 0xff)//Key triggered
	{
		keys = pModeMenu->keyVal; //keys is a temp variable in this function

		pModeMenu->keyVal = 0xFF;	// Reset menu key value; keys now holds the value
		switch(keys)
		{
			case KEY5_CLICK_RELEASE:
				pModeMenu = &settingModeMenu3[0];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_CLICK_RELEASE:	// Left key
				pModeMenu = &generalModeMenu[1];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY4_CLICK_RELEASE:	// Right key
				pModeMenu = &generalModeMenu[0];	// Jump to settings menu
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
		}
	}
	
}

//Main menu 1 handler
static void stg1Menu_MainMenuCBS(void)
{
	//pMenu, MHead, MTail are independent struct ptrs with separate resources
	//static vars retain their value across function calls
	unsigned char keys;//Temp key value variable
	unsigned char i;//for loop counter
	unsigned char ClrScreenFlag;//Screen refresh flag: refresh on page flip to clear/redraw list area; skip if no flip
	static stu_mode_menu *pMenu;		//Stores currently selected menu item
	static stu_mode_menu *bpMenu=0;		//Backup of last menu item for refresh check; refresh when pMenu != bpMenu
	static unsigned char stgMainMenuSelectedPos=0;	//Records selected menu position in list; default = 1, starting from first item
	static stu_mode_menu *MHead,*MTail;		//These two struct ptrs handle page flip on up/down menu navigation
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//Need to refresh screen
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//Only refresh once; next entry skips refresh; long-press center key triggers page refresh
		pMenu = &settingModeMenu1[0];//Execute main menu
		hal_Oled_Clear();// Refresh screen
		
		//display_6_8_string(10,0,pMenu->pModeType);//Write to OLED buffer; display updates on next refresh step

		
	// Refresh screen and display update is slow; only do it on key change
		
		pMenu = &settingModeMenu1[1];	//First item in settings menu list
		
		MHead = pMenu;			//Record current settings menu first item
		MTail = pMenu+6;		//Record current settings menu last item; 4 items per page, 5 total items
		bpMenu = 0;	//bpMenu != pMenu, enter UI display routine below
 
		ClrScreenFlag = 1;	//Set to 1 so first entry into display routine shows the menu list
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//Key menu operations including page flip
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//Up key
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//Deselect current menu item display
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//Down key
				if(stgMainMenuSelectedPos == 7)///Page flip: 4th item selected, down key pressed
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos max is 4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//Deselect current menu item display
					pMenu = pMenu->pNext;																			//Switch to next option
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	// Right key Confirm
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			
		}
	}
	
	//UI display: show main page menu list
	if(bpMenu != pMenu)//If address differs, update display (forced mismatch above)
	{
		bpMenu = pMenu;//bpMenu updated to current state; if pMenu changed, bpMenu != pMenu means key pressed, refresh display
		if(ClrScreenFlag)// Refresh screen flag, used on page flip to clear screen
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		// Clear screen (clear the list area)

			for(i=1; i<8; i++)	//Must be i<5 since only 4 menu items fit per page
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//Loop to display menu list names

				pMenu = pMenu->pNext;//Next
			} 
			pMenu = bpMenu;//Restore pMenu to first item; it was modified by pMenu->pNext in loop above
			//Cursor display position code below
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//Inverse highlight
		 
		}else//No clear needed when not flipping page
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//Inverse highlight

		}	
			
				 
	}
}
//Main menu 2 handler
static void stg2Menu_MainMenuCBS(void)
{
	//pMenu, MHead, MTail are independent struct ptrs with separate resources
	//static vars retain their value across function calls
	unsigned char keys;//Temp key value variable
	unsigned char i;//for loop counter
	unsigned char ClrScreenFlag;//Screen refresh flag: refresh on page flip to clear/redraw list area; skip if no flip
	static stu_mode_menu *pMenu;		//Stores currently selected menu item
	static stu_mode_menu *bpMenu=0;		//Backup of last menu item for refresh check; refresh when pMenu != bpMenu
	static unsigned char stgMainMenuSelectedPos=0;	//Records selected menu position in list; default = 1, starting from first item
	static stu_mode_menu *MHead,*MTail;		//These two struct ptrs handle page flip on up/down menu navigation
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//Need to refresh screen
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//Only refresh once; next entry skips refresh; long-press center key triggers page refresh
		pMenu = &settingModeMenu2[0];//Execute main menu
		hal_Oled_Clear();// Refresh screen
		
		//display_6_8_string(37,0,pMenu->pModeType);//Write to OLED buffer; display updates on next refresh step
		
	// Refresh screen and display update is slow; only do it on key change
		
		pMenu = &settingModeMenu2[1];	//First item in settings menu list
		
		MHead = pMenu;			//Record current settings menu first item
		MTail = pMenu+6;		//Record current settings menu last item; 4 items per page, 5 total items
		bpMenu = 0;	//bpMenu != pMenu, enter UI display routine below
 
		ClrScreenFlag = 1;	//Set to 1 so first entry into display routine shows the menu list
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//Key menu operations including page flip
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//Up key
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//Deselect current menu item display
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//Down key
				if(stgMainMenuSelectedPos == 7)///Page flip: 4th item selected, down key pressed
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos max is 4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//Deselect current menu item display
					pMenu = pMenu->pNext;																			//Switch to next option
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	// Right key Confirm
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			case KEY5_CLICK_RELEASE:	//Center key / Confirm
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
	
	//UI display: show main page menu list
	if(bpMenu != pMenu)//If address differs, update display (forced mismatch above)
	{
		bpMenu = pMenu;//bpMenu updated to current state; if pMenu changed, bpMenu != pMenu means key pressed, refresh display
		if(ClrScreenFlag)// Refresh screen flag, used on page flip to clear screen
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		// Clear screen (clear the list area)
			for(i=1; i<8; i++)	//Must be i<5 since only 4 menu items fit per page
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//Loop to display menu list names
				pMenu = pMenu->pNext;//Next
			} 
			pMenu = bpMenu;//Restore pMenu to first item; it was modified by pMenu->pNext in loop above
			//Cursor display position code below
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//Inverse highlight
		}else//No clear needed when not flipping page
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//Inverse highlight
		}	
			
				 
	}
}
//Main menu 3 handler
static void stg3Menu_MainMenuCBS(void)
{
	//pMenu, MHead, MTail are independent struct ptrs with separate resources
	//static vars retain their value across function calls
	unsigned char keys;//Temp key value variable
	unsigned char i;//for loop counter
	unsigned char ClrScreenFlag;//Screen refresh flag: refresh on page flip to clear/redraw list area; skip if no flip
	static stu_mode_menu *pMenu;		//Stores currently selected menu item
	static stu_mode_menu *bpMenu=0;		//Backup of last menu item for refresh check; refresh when pMenu != bpMenu
	static unsigned char stgMainMenuSelectedPos=0;	//Records selected menu position in list; default = 1, starting from first item
	static stu_mode_menu *MHead,*MTail;		//These two struct ptrs handle page flip on up/down menu navigation
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)	//Need to refresh screen
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;	//Only refresh once; next entry skips refresh; long-press center key triggers page refresh
		pMenu = &settingModeMenu3[0];//Execute main menu
		hal_Oled_Clear();// Refresh screen
		
		//display_6_8_string(37,0,pMenu->pModeType);//Write to OLED buffer; display updates on next refresh step
		
	// Refresh screen and display update is slow; only do it on key change
		
		pMenu = &settingModeMenu3[1];	//First item in settings menu list
		
		MHead = pMenu;			//Record current settings menu first item
		MTail = pMenu+6;		//Record current settings menu last item; 4 items per page, 5 total items		What is the +3 offset for??
		bpMenu = 0;	//bpMenu != pMenu, enter UI display routine below
 
		ClrScreenFlag = 1;	//Set to 1 so first entry into display routine shows the menu list
		stgMainMenuSelectedPos = 1;
		keys = 0xFF;
 
	}
	
	//Key menu operations including page flip
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY1_CLICK_RELEASE:		//Up key
				
				if(stgMainMenuSelectedPos ==1)
				{
					MHead = MHead->pLase;
					pMenu = pMenu->pLase;
					MTail = MTail->pLase;
					stgMainMenuSelectedPos = 1;
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//Deselect current menu item display
					pMenu = pMenu->pLase;
					stgMainMenuSelectedPos--;
				}
			break;
			case KEY2_CLICK_RELEASE:		//Down key
				if(stgMainMenuSelectedPos == 7)///Page flip: 4th item selected, down key pressed
				{
					MHead = MHead->pNext;	
					pMenu = pMenu->pNext;
					MTail = pMenu;
					stgMainMenuSelectedPos = 7;//stgMainMenuSelectedPos max is 4
					ClrScreenFlag = 1;
				}else
				{
					display_6_8_string(0,1*stgMainMenuSelectedPos," ");		//Deselect current menu item display
					pMenu = pMenu->pNext;																			//Switch to next option
					stgMainMenuSelectedPos++;
				}

			break;
			
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			
			case KEY4_CLICK_RELEASE:	// Right key Confirm
				pModeMenu->pChild = pMenu;
				pModeMenu = pModeMenu->pChild;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;		
			break;			
			
		}
	}
	
	//UI display: show main page menu list
	if(bpMenu != pMenu)//If address differs, update display (forced mismatch above)
	{
		bpMenu = pMenu;//bpMenu updated to current state; if pMenu changed, bpMenu != pMenu means key pressed, refresh display
		if(ClrScreenFlag)// Refresh screen flag, used on page flip to clear screen
		{
			
			ClrScreenFlag = 0;
			pMenu = MHead;
			hal_Oled_Clear();		// Clear screen (clear the list area)
			for(i=1; i<8; i++)	//Must be i<5 since only 4 menu items fit per page
			{
				display_6_8_string(10,1*i,(char*)pMenu->pModeType);//Loop to display menu list names
				pMenu = pMenu->pNext;//Next
			} 
			pMenu = bpMenu;//Restore pMenu to first item; it was modified by pMenu->pNext in loop above
			//Cursor display position code below
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//Inverse highlight
		}else//No clear needed when not flipping page
		{ 
			display_6_8_string(0,1*stgMainMenuSelectedPos,">");//Inverse highlight
		}	
			
				 
	}
}

static void stg1Menu_Para1CBS(void)
{
	unsigned char keys;
	
	if(pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET)
	{
		pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
		hal_Oled_Clear();	// Clear screen
		
		//hal_Oled_ShowString(20,0,"1. Inductance",12,1);
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		
		keys = 0xFF;
		 
	}	
	//LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_out_limH); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				pos_out_limH+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_out_limL); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				pos_out_limL+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,speed_adjust); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				speed_adjust+=1;
				if(speed_adjust > 50)
					speed_adjust = 50;
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,tar_theta_limit); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				tar_theta_limit+=1;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,pos_boudary); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				pos_boudary+=1;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,turn_kp); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				turn_kp+=0.2;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,sleep_time); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				sleep_time+=5;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,yaw_track_kp); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				yaw_track_kp+=0.01;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
				yaw_track_kp-=0.01;

			break;	//
			case KEY1_LONG_PRESS_CONTINUOUS:		// Up key, Increase value
				yaw_track_kp+=0.01;
	
			break;			
			case KEY2_LONG_PRESS_CONTINUOUS:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);				
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,3);
	display_6_8_number(40,3,yaw_track_kd); 
	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				yaw_track_kd+=0.01;
	
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
				yaw_track_kd-=0.01;

			break;	//
			case KEY1_LONG_PRESS_CONTINUOUS:		// Up key, Increase value
				yaw_track_kd+=0.01;
	
			break;			
			case KEY2_LONG_PRESS_CONTINUOUS:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"TASK1:");
		
		keys = 0xFF;
		 
	}	
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
				
			break;
			case KEY5_CLICK_RELEASE: //Center key short press
//				Time0A_init(5,USER_INT0);   //Enable servo when PWM is correct
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"TASK2:");
		
		keys = 0xFF;
		 
	}	
	LCD_clear_L(0,6);
	//display_6_8_number(60,6,Param.Send_Step); 


	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
				
			break;
			case KEY5_CLICK_RELEASE: //Center key short press
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		display_6_8_string(0,2,"stg2Para3:");

		keys = 0xFF;
		 
	}	

	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP2];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
//				kp4++;
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
//				kp4--;
			break;
			case KEY5_CLICK_RELEASE: //Center key short press
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para4:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		// Up key, Increase value
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //Center key short press
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);
		//display_6_8_string(0,2,"stg2Para5:");
				 
	}	
	Flag.Start_Car = 0;
	
			display_6_8_string(0,2,"A:");			  //cm/s   
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				if(key_value < 3)
				key_value++;
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
				if(key_value > 0)
					key_value--;
			break;
//			case KEY4_CLICK_RELEASE:		// Down key, Decrease value
//				point_C[0] = point_actual[0];
//				point_C[1] = point_actual[1];
//			break;			
			case KEY5_CLICK_RELEASE:		//Center key / Decrease parameter

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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para6:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		// Up key, Increase value
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //Center key short press
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,0,(char*)pModeMenu->pModeType);
		display_6_8_string(0,28,"stg2Para6:");
				 
	}		
	if(pModeMenu->keyVal != 0xff)
	{
		keys = pModeMenu->keyVal;
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP1];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
//			case KEY1_CLICK_RELEASE:		// Up key, Increase value
//				kp2++;
//			break;			
//			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
//				kp2--;
//			break;
			case KEY5_CLICK_RELEASE: //Center key short press
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_A[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
				point_A[0] -= 1;
//				point_B[0] = point_actual[0];
//				point_B[1] = point_actual[1];
			break;
//			case KEY4_CLICK_RELEASE:		// Down key, Decrease value
//				point_C[0] = point_actual[0];
//				point_C[1] = point_actual[1];
//			break;			
//			case KEY5_CLICK_RELEASE:		//Center key / Decrease parameter
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
		hal_Oled_Clear();	// Clear screen
		
		display_6_8_string(0,1,(char*)pModeMenu->pModeType);		
		
		keys = 0xFF;
		 
	}	
//			display_6_8_string(0,2,"A:");			  //cm/s   
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_A[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_B[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_B[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_C[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_C[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_D[0] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
		hal_Oled_Clear();	// Clear screen
		
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
		pModeMenu->keyVal = 0xFF;	// Reset menu key value
		switch(keys)
		{
			case KEY3_CLICK_RELEASE:	// Left key, Cancel
				pModeMenu = pModeMenu->pParent;
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY3_LONG_PRESS_RELEASE:		// Left key, Return to desktop
				pModeMenu = &generalModeMenu[GNL_MENU_DESKTOP3];
				pModeMenu->refreshScreenCmd = SCREEN_CMD_RESET;
			break;
			case KEY1_CLICK_RELEASE:		// Up key, Increase value
				point_D[1] += 1;
//				point_A[1] = point_actual[1];
			break;			
			case KEY2_CLICK_RELEASE:		// Down key, Decrease value
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
	if (gray_sample_req)
	{
		gray_sample_req = 0;
		gray_8data_read();
		/* Gray VOFA send removed - to be reimplemented by user */
	}
	pModeMenu->action();//Execute current menu service function in loop
	//Equivalent to gnlMenu_Desktop1CBS() in current menu position
}
//-----------------Driver layer callback handlers------------------------


//Key callback function
static void KeyEventHandle(KEY_VALUE_TYPEDEF keys)
{
	pModeMenu->keyVal = keys;//Assign key value to current menu
	
}
