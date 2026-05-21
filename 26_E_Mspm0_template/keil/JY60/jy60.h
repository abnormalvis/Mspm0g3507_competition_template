#ifndef __JY60_H
#define __JY60_H

#include "ti_msp_dl_config.h" 
//串口中断里调用
void jy60_ReceiveData(uint8_t RxData);

extern float Roll,Pitch,Yaw,ax,ay,az,wx,wy,wz,YawInt;

#endif
