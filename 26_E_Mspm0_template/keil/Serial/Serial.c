#include "Serial.h"

/* UART aliases per syscfg:
 *   UART_debug_INST       = UART0 (PA10/11)   ï¿?? debug + VOFA
 *   UART_vision_INST      = UART3 (PB2/3)     ï¿?? K230
 *   UART_display_INST     = UART1 (PA17/18)   ï¿?? HMI serial screen (ISR in zuolan_usart.c)
 *   UART_wired_INST       = UART2 (PA21/PB16) ï¿?? stepmotor (stub ISR below)
 */

void uart_debug_send_byte(uint8_t data)
{
	while( (UART_debug_INST->STAT & UART_STAT_TXFE_MASK) == 0 );
	DL_UART_Main_transmitData(UART_debug_INST, data);
}

void uart1_send_char(char ch)
{
	while ((UART_display_INST->STAT & UART_STAT_TXFE_MASK) == 0);
	DL_UART_Main_transmitData(UART_display_INST, ch);
}

void uart1_send_byte(uint8_t data)
{
	while ((UART_display_INST->STAT & UART_STAT_TXFE_MASK) == 0);
	DL_UART_Main_transmitData(UART_display_INST, data);
}

void uart3_send_byte(uint8_t data)
{
	while( (UART_vision_INST->STAT & UART_STAT_TXFE_MASK) == 0 );
	DL_UART_Main_transmitData(UART_vision_INST, data);
}

void usart_debug_SendCmd(volatile uint8_t *cmd, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++) { uart_debug_send_byte(cmd[i]); }
}

void uart1_send_string(char* str)
{
	while(*str != 0)
	{
		uart1_send_char(*str++);
	}
}

void usart3_SendCmd(volatile uint8_t *cmd, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++) { uart3_send_byte(cmd[i]); }
}

void uart3_send_string(char *str)
{
	while(*str != 0)
	{
		uart3_send_byte((uint8_t)(*str++));
	}
}

int fputc(int ch, FILE *stream)
{
	(void)stream;
	while (DL_UART_isBusy(UART_display_INST) == true);
	uart1_send_char(ch);
	return ch;
}

int fputs(const char* restrict s, FILE* restrict stream)
{
	uint16_t i, len;
	len = strlen(s);
	for(i = 0; i < len; i++)
	{
		fputc(s[i], stream);
	}
	return len;
}

/* ---- VOFA RX FIFO: ISR pushes bytes, main loop parses ---- */
static uint8_t vofa_rx_fifo_buf[128];
fifo_obj_struct vofa_rx_fifo;

void vofa_rx_fifo_init(void)
{
    fifo_init(&vofa_rx_fifo, FIFO_DATA_8BIT, vofa_rx_fifo_buf, 128);
}

void UART_debug_INST_IRQHandler(void)
{
	uint8_t RxData;
	switch( DL_UART_getPendingInterrupt(UART_debug_INST) )
	{
		case DL_UART_IIDX_RX:
			RxData = DL_UART_Main_receiveData(UART_debug_INST);
			fifo_write_element(&vofa_rx_fifo, RxData);
			break;
		default:
			break;
	}
}

/* UART1 ISR moved to keil/HMI/zuolan_usart.c (HMI serial screen handler) */

void UART_vision_INST_IRQHandler(void)
{
	uint8_t RxData;
	switch( DL_UART_getPendingInterrupt(UART_vision_INST) )
	{
		case DL_UART_IIDX_RX:
			RxData = DL_UART_Main_receiveData(UART_vision_INST);
			K230_ReceiveData(RxData);
			break;
		default:
			break;
	}
}

void UART_wired_INST_IRQHandler(void)
{
	switch( DL_UART_getPendingInterrupt(UART_wired_INST) )
	{
		case DL_UART_IIDX_RX:
			(void)DL_UART_Main_receiveData(UART_wired_INST);
			break;
		default:
			break;
	}
}
