
#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "hal_beep.h"



void hal_BeepON(void)
{
	DL_GPIO_setPins(BEEP_PORT,BEEP_PIN); 
}

void hal_BeepOFF(void)
{
	DL_GPIO_clearPins(BEEP_PORT,BEEP_PIN); //?????? 
}
