#ifndef _MT_FLAG_H
#define _MT_FLAG_H

#include "stdint.h"

typedef struct//用于存放各种参数的结构体，这里的参数是慢慢写程序，慢慢添加的，需要用到什么了，就在这里面加
{
	uint8_t  Send1_Step;//工程步骤
	uint8_t  Send2_Step;//工程步骤
	int8_t  Send3_Step;//工程步骤
	uint8_t  Send4_Step;//工程步骤
}Param_InitTypeDef;

typedef struct//用于存放各种标志位的结构体  出库 循迹 入环 环内 出环 入岔 岔内 出岔 障碍（3个子状态） 入库
{
	uint8_t Start_Car;//开始发车

	uint8_t Stop_Car;
	
	uint8_t Start_duty_1;
	uint8_t Success_duty_1;
	
	uint8_t Start_duty2_1;
	uint8_t Success_duty2_1;	

	uint8_t Start_duty2_2;
	uint8_t Success_duty2_2;	

	uint8_t Start_duty2_3;
	uint8_t Success_duty2_3;	

	uint8_t Start_duty2_4;
	uint8_t Success_duty2_4;	

	uint8_t Start_duty2_5;
	uint8_t Success_duty2_5;	
	
	uint8_t Start_duty2_6;
	uint8_t Success_duty2_6;	

	uint8_t Start_duty2_7;
	uint8_t Success_duty2_7;
	
	uint8_t Start_duty3_0;
	uint8_t Success_duty3_0;	

	uint8_t Start_duty3_1;
	uint8_t Success_duty3_1;	

	uint8_t Start_duty3_2;
	uint8_t Success_duty3_2;	

	uint8_t Start_duty3_3;
	uint8_t Success_duty3_3;	

	uint8_t Start_duty3_7;
	uint8_t Success_duty3_7;	

	uint8_t Start_duty3_4;
	uint8_t Success_duty3_4;	

	uint8_t Start_duty3_5;
	uint8_t Success_duty3_5;	
	
	uint8_t Start_duty3_6;
	uint8_t Success_duty3_6;	
	
	uint8_t Start_duty3_8;
	uint8_t Success_duty3_8;	
	
	uint8_t Start_duty3_9;
	uint8_t Success_duty3_9;	
	uint8_t gray_worse;
	
	uint8_t yaw_loss;
	uint8_t yaw_loss_ahead;
	
	uint8_t beep_on;
	uint8_t task_start;	
	uint8_t angle_loss;	
}Flag_InitTypeDef;

extern Param_InitTypeDef Param;//定义了要用的参数结构体
extern Flag_InitTypeDef  Flag;//定义了要用的标志位结构体
void Flag_Init(void);
#endif
