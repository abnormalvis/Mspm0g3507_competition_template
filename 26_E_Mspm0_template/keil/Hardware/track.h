#ifndef __TRACK_H__
#define __TRACK_H__


#include "headfile.h"


#define TRACK_CLK_PORT  TRACK_PORT
#define TRACK_CLK_Pin		TRACK_CLK_PIN
#define TRACK_DAT_PORT  TRACK_PORT
#define TRACK_DAT_Pin		TRACK_DAT_PIN

//转向次数
extern uint8_t turn_count;
extern uint8_t track_value;
extern uint8_t turn_flag;
extern uint8_t turn_flag_for_track;
extern float track_depart_level;
extern uint8_t pre_turn_flag;
void Update_Tarck_Depart_Level();
void Turn();
uint8_t Gray_Serial_Read();
#endif
