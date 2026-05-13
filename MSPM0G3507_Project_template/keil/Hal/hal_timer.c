#include "ti_msp_dl_config.h"
#include "hal_timer.h"
#include <ti/driverlib/dl_timerg.h>
#include "ti/driverlib/dl_gpio.h"
#include "hal_led.h"
#include "hal_encode.h"

void TIMG0_Init(void)
{
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);//使能中断
//	NVIC_DisableIRQ(TIMG0_INT_IRQn);
	DL_TimerG_startCounter(TIMER_0_INST);	//使能定时器

}


 
void TIMG6_Init(void)
{
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);//使能中断
	DL_TimerG_startCounter(TIMER_1_INST);	//使能定时器

}


void TIMG7_Init(void)
{
	NVIC_EnableIRQ(TIMER_2_INST_INT_IRQN);//使能中断
	DL_TimerG_startCounter(TIMER_2_INST);//使能定时器

}

