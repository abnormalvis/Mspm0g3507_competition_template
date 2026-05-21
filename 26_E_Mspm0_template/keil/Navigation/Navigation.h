#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__
extern float X_Pos;
extern float Y_Pos;
extern float Dist;
extern float w_disp;
void GetPosition(float SpeedL, float SpeedR, float YawAngle, float Dt);
void SetDistance(float Target,float Actual);
#endif