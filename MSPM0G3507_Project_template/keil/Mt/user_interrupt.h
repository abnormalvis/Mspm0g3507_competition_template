#ifndef __USER_INTERRUPT_H
#define __USER_INTERRUPT_H
#include "stdint.h"

extern uint16_t Num2;			//Variable incremented in timer ISR
extern int16_t error_openmv, error_polarity;
extern volatile uint8_t gray_sample_req;
#endif
