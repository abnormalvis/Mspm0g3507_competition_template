#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "headfile.h"
//左右满转70
typedef enum
{
    FORWARD, // 正向  1
    REVERSAL // 反向 -1
} ENCODER_DIR;

typedef struct
{
    volatile long long temp_count; // 保存实时计数值
    int count;                     // 根据定时器时间更新的计数值
    ENCODER_DIR dir;               // 旋转方向
} ENCODER_RES;

extern ENCODER_RES motor_left_encoder;
extern ENCODER_RES motor_right_encoder;

int Get_Encoder_Count(ENCODER_RES *encoder);
ENCODER_DIR Get_Encoder_Dir(ENCODER_RES *encoder);
void Encoder_Update(ENCODER_RES *encoder);

#endif