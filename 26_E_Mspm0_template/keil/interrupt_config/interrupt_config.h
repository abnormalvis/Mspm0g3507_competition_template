#ifndef INTERRUPT_CONFIG_H
#define INTERRUPT_CONFIG_H

#include "ti_msp_dl_config.h"

/* UART_2 is not configured in current syscfg; provide fallback */
#ifndef UART_2_INST_INT_IRQN
#define UART_2_INST_INT_IRQN       UART2_INT_IRQn
#endif

/**
 * @brief 初始化所有中断（NVIC配置 + 外设中断开启）
 * @note 需在SYSCFG_DL_init()之后调用
 */
void interrupt_init(void);

#endif // INTERRUPT_CONFIG_H