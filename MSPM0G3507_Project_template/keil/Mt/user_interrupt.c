/*******************************************************************************
    * @editor      : wangming
    * @wechat      : DeepCoderMing
    * @qq          : 3201935299
    * @date        : 2025-05-01
    * @copyright   : Confidential - used for sample/demo purposes only
********************************************************************************/
#include "user_interrupt.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_gray.h"
#include "zf_common_fifo.h"
#include "seekfree_assistant.h"
#include "mt_test.h"
#include  "mt_flag.h"
#include "hal_beep.h"
#include "hal_math.h"
#include "hal_encode.h"
#include "imu_filter.h"
#include "hal_vofa.h"

/*zigbee*/
uint8_t  UART1_recevie;
uint8_t   debug_uart_data;

void UART_1_INST_IRQHandler(void) // UART1 interrupt handler
{
    switch (DL_UART_Main_getPendingInterrupt(UART_1_INST)) {
        case DL_UART_MAIN_IIDX_RX:
           UART1_recevie = DL_UART_Main_receiveData(UART_1_INST);
           fifo_write_buffer(&debug_uart_fifo, &UART1_recevie, 1);
           /* VOFA receive parsing */
           vofa_rx_byte(UART1_recevie);
            break;
        default:
            break;
    }
}

uint16_t Num1 = 0,Num2 = 0;
static uint16_t beep_on_cnt = 0,task_start_cnt = 0;
void TIMG6_IRQHandler(void) // 5ms timer interrupt
{
    gray_8data_read();
    if(Flag.beep_on == 1)
    {
        beep_on_cnt++;
        hal_BeepON();
        RGB_Blue_open();
        if(beep_on_cnt > 100)
        {
            beep_on_cnt = 0;
            Flag.beep_on = 0;
            hal_BeepOFF();
            RGB_Blue_close();
        }
    }
}