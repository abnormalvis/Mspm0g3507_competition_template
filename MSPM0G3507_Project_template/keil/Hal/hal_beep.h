#ifndef _HAL_BEEP_H
#define _HAL_BEEP_H

#include "ti_msp_dl_config.h"

#define BEEP_PORT			GPIOA
#define BEEP_PIN			DL_GPIO_PIN_27

void hal_BeepON(void);
void hal_BeepOFF(void);
#endif

