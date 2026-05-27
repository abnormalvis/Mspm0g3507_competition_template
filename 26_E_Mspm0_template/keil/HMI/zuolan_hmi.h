/**
 * @file zuolan_hmi.h
 * @brief HMI人机交互接口头文件 (MSPM0G3507适配版)
 */
#ifndef __ZUOLAN_HMI_H__
#define __ZUOLAN_HMI_H__

#include "zuolan_usart.h"

void zuolan_HMI_Send_String(char *obj_name, char *show_data);
void zuolan_HMI_Send_Int(char *obj_name, int show_data);
void zuolan_HMI_Send_Float(char *obj_name, float show_data, int point_index);
void zuolan_HMI_Wave_Clear(char *obj_name, int ch);
void zuolan_HMI_Write_Wave_Low(char *obj_name, int ch, int val);
void zuolan_HMI_Write_Wave_Fast(char *obj_name, int ch, int len, int *val);

#endif // __ZUOLAN_HMI_H__
