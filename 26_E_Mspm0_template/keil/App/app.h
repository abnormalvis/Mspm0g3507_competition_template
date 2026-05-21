#ifndef _APP_H
#define _APP_H

void AppInit(void);
void AppProc(void);

//---- Menu-related macro definitions ----
typedef enum
{
	SCREEN_CMD_NULL,		// Null command 0
	SCREEN_CMD_RESET,		// Set command display 1
	SCREEN_CMD_RECOVER,		// Restore original display 2
	SCREEN_CMD_UPDATE,		// Update original display 3
}SCREEN_CMD;		// Refresh screen display flag



// Define current menu position, mainly for timeout exit judgment
typedef enum
{
	DESKTOP_MENU_POS,	// Desktop  1
	STG_MENU_POS,// Level-1 menu     2
//	STG_WIFI_MENU_POS,// Wifi menu
	STG_SUB_2_MENU_POS,// Level-2 menu 3
	STG_SUB_3_MENU_POS,// Level-3 menu 4
	STG_SUB_4_MENU_POS,// Level-4 menu 5
}MENU_POS;

//(1) General menu list - only one level menu
typedef enum
{
	GNL_MENU_DESKTOP1,		// Desktop 1
	GNL_MENU_DESKTOP2,		// Desktop 2
	GNL_MENU_DESKTOP3,		// Desktop 3
	GNL_MENU_SUM,
}GENERAL_MENU_LIST;			// General menu list

//(2) Settings menu list ID
typedef enum
{
	STG1_MENU_MAIN_SETTING,//0
	STG1_MENU_PARA1,//1
	STG1_MENU_PARA2,//2
	STG1_MENU_PARA3,//3
	STG1_MENU_PARA4,//4
	STG1_MENU_PARA5,//5
	STG1_MENU_PARA6,//4
	STG1_MENU_PARA7,//5
	STG1_MENU_PARA8,//4
	STG1_MENU_PARA9,//5
	STG1_MENU_SUM//6
}STG1_MENU_LIST;

//(2) Settings menu list ID
typedef enum
{
	STG2_MENU_MAIN_SETTING,//0
	STG2_MENU_PARA1,//1
	STG2_MENU_PARA2,//2
	STG2_MENU_PARA3,//3
	STG2_MENU_PARA4,//4
	STG2_MENU_PARA5,//5
	STG2_MENU_PARA6,//4
	STG2_MENU_PARA7,//5
	STG2_MENU_SUM//6
}STG2_MENU_LIST;

//(3) Settings menu list ID
typedef enum
{
	STG3_MENU_MAIN_SETTING,//0
	STG3_MENU_PARA1,//1
	STG3_MENU_PARA2,//2
	STG3_MENU_PARA3,//3
	STG3_MENU_PARA4,//4
	STG3_MENU_PARA5,//5
	STG3_MENU_PARA6,//3
	STG3_MENU_PARA7,//4
	STG3_MENU_PARA8,//5
	STG3_MENU_SUM//6
}STG3_MENU_LIST;

// Common menu struct - struct nesting requires correct format
typedef struct MODE_MENU
{
	unsigned char ID;				// Menu unique ID
	MENU_POS menuPos;				// Current menu position info
	const char *pModeType;		// Current mode name string
	void (*action)(void);				// Action function for current mode
	SCREEN_CMD refreshScreenCmd;		// Refresh screen display command
	unsigned char reserved;				// Reserved variable
	unsigned char keyVal;				// Key value, 0xFF = no key (to avoid conflict with 0,1,2,3,4)
	struct MODE_MENU *pLase;			// Point to previous option (nested struct)
	struct MODE_MENU *pNext;			// Point to next option (nested struct)
	struct MODE_MENU *pParent;			// Point to parent menu (nested struct)
	struct MODE_MENU *pChild;			// Point to child menu (nested struct)
}stu_mode_menu;
//---------------------------------

typedef struct SYSTEM_TIME
{
	unsigned short year;
	unsigned char mon;
	unsigned char day;
	unsigned char week;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
}stu_system_time;

extern float test_eepro;
extern int task_num;


#endif

