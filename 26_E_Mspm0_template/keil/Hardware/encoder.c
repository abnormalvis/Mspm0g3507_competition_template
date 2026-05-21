#include "encoder.h"
ENCODER_RES motor_left_encoder;
ENCODER_RES motor_right_encoder;

// 初始化
// 获取速度值：时间取决定时器中断
int Get_Encoder_Count(ENCODER_RES *encoder)
{
    return encoder->count;
}

// 获取转向
ENCODER_DIR Get_Encoder_Dir(ENCODER_RES *encoder)
{
    return encoder->dir;
}

// 更新速度值：放在定时器内
void Encoder_Update(ENCODER_RES *encoder)
{
    encoder->count = encoder->temp_count;

    // 确定方向
    encoder->dir = (encoder->count >= 0) ? FORWARD : REVERSAL;

    encoder->temp_count = 0; // 编码器计数值清零
}
