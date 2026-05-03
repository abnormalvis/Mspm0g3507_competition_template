#ifndef    __HAL_GRAY_H
#define    __HAL_GRAY_H

#include "stdint.h"

/* 8·灰度传感器ADC值 */
extern uint16_t LQ_Tracking_Value[8];
/* 阈值设置 */
extern uint16_t gray_threshold[8];

/*****    灰度状态     *******/
typedef struct
{
	uint8_t bit1	:1;
	uint8_t bit2	:1;
	uint8_t bit3	:1;
	uint8_t bit4	:1;
	uint8_t bit5	:1;
	uint8_t bit6	:1;
	uint8_t bit7	:1;
	uint8_t bit8	:1;
	uint8_t bit9	:1;
	uint8_t bit10	:1;
	uint8_t bit11	:1;
	uint8_t bit12	:1;
}gray_flags;    //8·灰度，共使用bit1--bit8


typedef union    //联合体，用于存储灰度传感器的状态信息
{
	gray_flags gray;
	uint16_t state;
}_gray_state;

extern _gray_state gray_state;
extern float gray_status;
extern char stop_flag;

/* 灰度传感器初始化函数 */
void gray_8data_read(void);

/* 设置阈值 */
void gray_set_threshold(uint16_t* threshold);

#endif