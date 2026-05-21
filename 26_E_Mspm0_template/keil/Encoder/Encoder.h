#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ti_msp_dl_config.h"


extern int32_t Motor_speedL,Motor_speedR,Motor_distanceL,Motor_distanceR;

void Encoder_OnGroupIRQ(uint32_t gpioB);
void EncoderGetValue(void);


#endif
