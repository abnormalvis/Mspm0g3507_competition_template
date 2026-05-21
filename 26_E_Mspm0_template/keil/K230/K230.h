#ifndef __K230_H
#define __K230_H

#include "ti_msp_dl_config.h"
#include "Serial.h"

//接受数据包数组
extern float k230_RxPacket[50];
//K230发送一个字节
void K230_SendByte(uint8_t data);
//K230发送数据包，格式为[1,2,3]
void K230_SendDataPkg(float Channel1, float Channel2, float Channel3, float Channel4,
                      float Channel5, float Channel6, float Channel7, float Channel8, uint8_t ChannelNum);
//保证在串口中断函数调用
//K230发送格式[1,2,...]
void K230_ReceiveData(uint8_t RxData);
void k230_send_string(char *str);

#endif
