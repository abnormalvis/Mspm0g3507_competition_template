#include "Serial.h"

/* UART aliases per syscfg:
 *   UART_debug_INST = UART0 (PA10/11) â€? debug + stepper1
 *   UART_1_INST     = UART3           â€? VOFA
 *   UART_2_INST     = UART1           â€? K230 + stepper2
 */

void uart_debug_send_byte(uint8_t data)
{
	while( (UART_debug_INST->STAT & UART_STAT_TXFE_MASK) == 0 );
	DL_UART_Main_transmitData(UART_debug_INST, data);
}

void uart1_send_char(char ch)
{
	while( (UART_1_INST->STAT & UART_STAT_TXFE_MASK) == 0 );
	DL_UART_Main_transmitData(UART_1_INST, ch);
}

void uart1_send_byte(uint8_t data)
{
	while( (UART_1_INST->STAT & UART_STAT_TXFE_MASK) == 0 );
	DL_UART_Main_transmitData(UART_1_INST, data);
}

void uart2_send_byte(uint8_t data)
{
	while( (UART_2_INST->STAT & UART_STAT_TXFE_MASK) == 0 );
	DL_UART_Main_transmitData(UART_2_INST, data);
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

void usart2_SendCmd(volatile uint8_t *cmd, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++) { uart2_send_byte(cmd[i]); }
}

void uart2_send_string(char *str)
{
	while(*str != 0)
	{
		uart2_send_byte((uint8_t)(*str++));
	}
}

int fputc(int ch, FILE *stream)
{
	(void)stream;
	while( DL_UART_isBusy(UART_1_INST) == true );
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

void UART_debug_INST_IRQHandler(void)
{
	uint8_t RxData;
	switch( DL_UART_getPendingInterrupt(UART_debug_INST) )
	{
		case DL_UART_IIDX_RX:
			RxData = DL_UART_Main_receiveData(UART_debug_INST);
			vofa_rx_byte(RxData);   // VOFA protocol parser on debug UART (physical UART0)
			break;
		default:
			break;
	}
}

void UART_1_INST_IRQHandler(void)
{
	// uint8_t RxData;
	switch( DL_UART_getPendingInterrupt(UART_1_INST) )
	{
		case DL_UART_IIDX_RX:
			// RxData = DL_UART_Main_receiveData(UART_1_INST);
			// vofa_rx_byte(RxData);
			break;
		default:
			break;
	}
}

void UART_2_INST_IRQHandler(void)
{
	uint8_t RxData;
	switch( DL_UART_getPendingInterrupt(UART_2_INST) )
	{
		case DL_UART_IIDX_RX:
			RxData = DL_UART_Main_receiveData(UART_2_INST);
			K230_ReceiveData(RxData);
			break;
		default:
			break;
	}
}
