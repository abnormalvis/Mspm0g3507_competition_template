#include "Navigation.h"
#include "Encoder.h"
#include "StandardPid.h"
#include "imu_filter.h"
#include <math.h>

#define DegtoRad  0.01745329251f 
#define EnctoCm   1.19209492056f    //将编码器脉冲转为cm
#define WheelBase 15.0f  // 两轮间距(cm)

float X_Pos = 0.0f;   // X坐标
float Y_Pos = 0.0f;   // Y坐标
float Dist = 0.0f;
static float v_filtered = 0.0f;  // 滤波后的速度
float w_disp = 0.0f;

/**
  * 函    数：惯导解算函数，通过速度积分和车身角度得到xy方向坐标
  * 参    数：左轮速度、右轮速度、车身角度、积分时间
  * 返 回 值：无
  */
void GetPosition(float SpeedL, float SpeedR, float YawAngle, float Dt)
{
    // 1. 计算平均速度、角速度
    float v_raw = (SpeedL + SpeedR) * 0.5f;
    v_filtered = 1*v_raw + (1-1)*v_filtered;    //滤波，根据实际情况调整
    float w = (SpeedR - SpeedL) / WheelBase;
    
    w_disp = w;
    // 2. 角度转弧度
    float yaw_rad = YawAngle  * DegtoRad;

     // 3. 判断是否接近直线
    if(fabs(w) < 1.2)
    {
        // ===== 直线运动 =====
        X_Pos += v_filtered * sin(yaw_rad) * Dt/1000*EnctoCm;
        Y_Pos += v_filtered * cos(yaw_rad) * Dt/1000*EnctoCm;    //除以1000，防止溢出，乘以系数转化为cm
    }
    else
    {
        // ===== 弧线运动（核心优化）=====
        float delta_yaw = w * Dt;   // 角度变化（弧度）

        float R = v_filtered / w;   // 转弯半径

        // 使用弧线积分公式（关键！）
        X_Pos += R * (sin(yaw_rad + delta_yaw) - sin(yaw_rad)) * EnctoCm/1000;
        Y_Pos += R * (cos(yaw_rad) - cos(yaw_rad + delta_yaw)) * EnctoCm/1000;
    }

    // // 3. 坐标积分
    // X_Pos += v_filtered * sin(yaw_rad) * Dt/1000*EnctoCm;
    // Y_Pos += v_filtered * cos(yaw_rad) * Dt/1000*EnctoCm;    //除以1000，防止溢出，乘以系数转化为cm
    Dist = sqrt(X_Pos * X_Pos + Y_Pos * Y_Pos);
}

void SetDistance(float Target,float Actual)
{
  if(Target < Actual)
    {
      Track_out[0] = 0;
      Track_out[1] = 0;
    }

}