/*******************************************************************************
 * @author      : wangming
 * @wechat    :DeepCoderMing
 * @qq      : 3201935299
 * @date      : 2025-05-01
 * @copyright  : For reference only, commercial use prohibited
 ********************************************************************************/
#include "ti_msp_dl_config.h"
#include "OS_System.h"
#include "CPU.h"
#include "hal_timer.h"
/********************************************************************************************************
 *  @fn   hal_CoreClockInit
 *  @brief     CPU system clock init
 *  @param     None
 *  @return   None
 *  @note     Task scheduling tick; typically set to 10ms for real-time performance
 ********************************************************************************************************/
static void hal_CoreClockInit(void)
{
	TIMG7_Init(); // Timer interrupt
}

/********************************************************************************************************
 *  @fn   SysTick_Handler
 *  @brief     CPU system tick interrupt
 *  @param     None
 *  @return   None
 *  @note     Core 10ms tick ISR callback; must call system clock handler
 ********************************************************************************************************/
// void SysTick_Handler(void)
//{
//	OS_ClockInterruptHandle();
// }

void TIMG7_IRQHandler(void) // 10ms
{
	OS_ClockInterruptHandle();

	//		Num1++;
	//		if(Num1%100 == 0)
	//		{
	//			Num2++;
	//			Num1=0;
	//			DL_GPIO_togglePins(RGB_GPIO,RGB_Red_Pin);
	//		}
}

/********************************************************************************************************
 *  @fn   hal_getprimask
 *  @brief     Get CPU global interrupt state
 *  @param     None
 *  @return   0-global interrupt off, 1-global interrupt on
 *  @note     None
 ********************************************************************************************************/
static unsigned char hal_getprimask(void)
{
	return (!__get_PRIMASK()); // 0=interrupts on, 1=interrupts off; invert logic
}

/********************************************************************************************************
 *  @fn   hal_CPU_Critical_Control
 *  @brief     CPU critical section control
 *  @param     cmd: control command, *pSta: global interrupt state
 *  @return   None
 *  @note     None
 ********************************************************************************************************/
void hal_CPU_Critical_Control(CPU_EA_TYPEDEF cmd, unsigned char *pSta)
{
	if (cmd == CPU_ENTER_CRITICAL)
	{
		*pSta = hal_getprimask(); // Save interrupt state
		__disable_irq();		  // Disable CPU global interrupt
	}
	else if (cmd == CPU_EXIT_CRITICAL)
	{
		if (*pSta)
		{
			__enable_irq(); // Enable interrupt
		}
		else
		{
			__disable_irq(); // Disable interrupt
		}
	}
}

/********************************************************************************************************
 *  @fn   hal_CPUInit
 *  @brief     CPU system clock related init
 *  @param     None
 *  @return   0-global interrupt off, 1-global interrupt on
 *  @note     None
 ********************************************************************************************************/
void hal_CPUInit(void)
{
	hal_CoreClockInit(); // SysTick auto-start, 10ms period
	OS_CPUInterruptCBSRegister(hal_CPU_Critical_Control);
}
