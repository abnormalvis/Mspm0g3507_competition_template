#include "interrupt_config.h"

void interrupt_init(void)
{
    __disable_irq();

    NVIC_ClearPendingIRQ(UART_debug_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_display_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_vision_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOB_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOA_INT_IRQN);

    NVIC_SetPriority(UART_debug_INST_INT_IRQN, 2);
    NVIC_EnableIRQ  (UART_debug_INST_INT_IRQN);

    NVIC_SetPriority(UART_display_INST_INT_IRQN, 2);
    NVIC_EnableIRQ  (UART_display_INST_INT_IRQN);

    NVIC_SetPriority(UART_stepmotor_INST_INT_IRQN, 1);
    NVIC_EnableIRQ  (UART_stepmotor_INST_INT_IRQN);

    NVIC_SetPriority(TIMER_0_INST_INT_IRQN, 3);
    NVIC_EnableIRQ  (TIMER_0_INST_INT_IRQN);

    NVIC_SetPriority(TIMER_1_INST_INT_IRQN, 2);
    NVIC_EnableIRQ  (TIMER_1_INST_INT_IRQN);

    NVIC_SetPriority(Encoder_GPIOB_INT_IRQN, 2);
    NVIC_EnableIRQ  (Encoder_GPIOB_INT_IRQN);

    NVIC_SetPriority(Encoder_GPIOA_INT_IRQN, 2);
    NVIC_EnableIRQ  (Encoder_GPIOA_INT_IRQN);

    __enable_irq();
}