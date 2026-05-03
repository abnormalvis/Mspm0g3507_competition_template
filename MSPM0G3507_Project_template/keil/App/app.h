#ifndef _APP_H
#define _APP_H

void AppInit(void);
void AppProc(void);

//----菜单相关声明区域
typedef enum
{
	SCREEN_CMD_NULL,		//无用命令 0
	SCREEN_CMD_RESET,		//重置屏显示 1
	SCREEN_CMD_RECOVER,		//恢复原来显示 2
	SCREEN_CMD_UPDATE,		//更新原来显示 3
}SCREEN_CMD;		//刷新屏显示标志



//定义当前菜单的位置，主要用于超时退出判断，有的菜单超时就退出
typedef enum
{
	DESKTOP_MENU_POS,	//桌面  1
	STG_MENU_POS,//主菜单     2
//	STG_WIFI_MENU_POS,//wifi菜单
	STG_SUB_2_MENU_POS,//二级菜单 3
	STG_SUB_3_MENU_POS,//三级菜单 4
	STG_SUB_4_MENU_POS,//四级菜单 5
}MENU_POS;

//(1)普通菜单列表 只有一个桌面菜单
typedef enum
{
	GNL_MENU_DESKTOP1,		//桌面1
	GNL_MENU_DESKTOP2,		//桌面2
	GNL_MENU_DESKTOP3,		//桌面3
	GNL_MENU_SUM,
}GENERAL_MENU_LIST;			//普通菜单列表

//(2)设置菜单列表ID
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

//(2)设置菜单列表ID
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

//(3)设置菜单列表ID
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

//所有菜单的共性 结构体嵌套要注意这种格式
typedef struct MODE_MENU
{
	unsigned char ID;				//菜单唯一ID号
	MENU_POS menuPos;				//当前菜单的位置信息
	const char *pModeType;		//指向当前模式类型
	void (*action)(void);				//当前模式下的响应函数
	SCREEN_CMD refreshScreenCmd;		//刷新屏显示命令
	unsigned char reserved;				//预留，方便参数传递
	unsigned char keyVal;				//按键值,0xFF代表无按键触发 为什么要用0xff是为了避免用0 1 2 3 4…… 会冲突
	struct MODE_MENU *pLase;			//指向上一个选项 结构体嵌套 做链表用的
	struct MODE_MENU *pNext;			//指向下一个选项 结构体嵌套 做链表用的
	struct MODE_MENU *pParent;			//指向父级菜单 结构体嵌套
	struct MODE_MENU *pChild;			//指向子级菜单 结构体嵌套
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

