#include "hal_timer.h"
#include <ti/driverlib/dl_timerg.h>
#include "ti/driverlib/dl_gpio.h"
#include "hal_led.h"
#include "hal_encode.h"

void TIMG0_Init(void)
{
	NVIC_EnableIRQ(TIMG0_INT_IRQn);//使能中断
//	NVIC_DisableIRQ(TIMG0_INT_IRQn);
	DL_TimerG_startCounter(TIMG0);	//使能定时器

}


 
void TIMG6_Init(void)
{
	NVIC_EnableIRQ(TIMG6_INT_IRQn);//使能中断
	DL_TimerG_startCounter(TIMG6);	//使能定时器

}


void TIMG7_Init(void)
{
	NVIC_EnableIRQ(TIMG7_INT_IRQn);//使能中断
	DL_TimerG_startCounter(TIMG7);//使能定时器

}

