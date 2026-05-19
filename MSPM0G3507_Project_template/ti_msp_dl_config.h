/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000



/* Defines for PWM_0 */
#define PWM_0_INST                                                         TIMA0
#define PWM_0_INST_IRQHandler                                   TIMA0_IRQHandler
#define PWM_0_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define PWM_0_INST_CLK_FREQ                                             80000000
/* GPIO defines for channel 1 */
#define GPIO_PWM_0_C1_PORT                                                 GPIOA
#define GPIO_PWM_0_C1_PIN                                          DL_GPIO_PIN_7
#define GPIO_PWM_0_C1_IOMUX                                      (IOMUX_PINCM14)
#define GPIO_PWM_0_C1_IOMUX_FUNC                     IOMUX_PINCM14_PF_TIMA0_CCP1
#define GPIO_PWM_0_C1_IDX                                    DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 3 */
#define GPIO_PWM_0_C3_PORT                                                 GPIOA
#define GPIO_PWM_0_C3_PIN                                         DL_GPIO_PIN_23
#define GPIO_PWM_0_C3_IOMUX                                      (IOMUX_PINCM53)
#define GPIO_PWM_0_C3_IOMUX_FUNC                     IOMUX_PINCM53_PF_TIMA0_CCP3
#define GPIO_PWM_0_C3_IDX                                    DL_TIMER_CC_3_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                          (1562U)
/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMG6)
#define TIMER_1_INST_IRQHandler                                 TIMG6_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMG6_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                          (1562U)
/* Defines for TIMER_2 */
#define TIMER_2_INST                                                     (TIMG7)
#define TIMER_2_INST_IRQHandler                                 TIMG7_IRQHandler
#define TIMER_2_INST_INT_IRQN                                   (TIMG7_INT_IRQn)
#define TIMER_2_INST_LOAD_VALUE                                          (3124U)



/* Defines for UART_1 */
#define UART_1_INST                                                        UART0
#define UART_1_INST_FREQUENCY                                           40000000
#define UART_1_INST_IRQHandler                                  UART0_IRQHandler
#define UART_1_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOA
#define GPIO_UART_1_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_1_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_1_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_1_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_1_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_1_BAUD_RATE                                                (115200)
#define UART_1_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_1_FBRD_40_MHZ_115200_BAUD                                      (45)
/* Defines for UART_2 */
#define UART_2_INST                                                        UART3
#define UART_2_INST_FREQUENCY                                           80000000
#define UART_2_INST_IRQHandler                                  UART3_IRQHandler
#define UART_2_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_2_RX_PORT                                                GPIOB
#define GPIO_UART_2_TX_PORT                                                GPIOB
#define GPIO_UART_2_RX_PIN                                         DL_GPIO_PIN_3
#define GPIO_UART_2_TX_PIN                                         DL_GPIO_PIN_2
#define GPIO_UART_2_IOMUX_RX                                     (IOMUX_PINCM16)
#define GPIO_UART_2_IOMUX_TX                                     (IOMUX_PINCM15)
#define GPIO_UART_2_IOMUX_RX_FUNC                      IOMUX_PINCM16_PF_UART3_RX
#define GPIO_UART_2_IOMUX_TX_FUNC                      IOMUX_PINCM15_PF_UART3_TX
#define UART_2_BAUD_RATE                                                (115200)
#define UART_2_IBRD_80_MHZ_115200_BAUD                                      (43)
#define UART_2_FBRD_80_MHZ_115200_BAUD                                      (26)
/* Defines for UART_3 */
#define UART_3_INST                                                        UART1
#define UART_3_INST_FREQUENCY                                           40000000
#define UART_3_INST_IRQHandler                                  UART1_IRQHandler
#define UART_3_INST_INT_IRQN                                      UART1_INT_IRQn
#define GPIO_UART_3_RX_PORT                                                GPIOA
#define GPIO_UART_3_TX_PORT                                                GPIOA
#define GPIO_UART_3_RX_PIN                                        DL_GPIO_PIN_18
#define GPIO_UART_3_TX_PIN                                        DL_GPIO_PIN_17
#define GPIO_UART_3_IOMUX_RX                                     (IOMUX_PINCM40)
#define GPIO_UART_3_IOMUX_TX                                     (IOMUX_PINCM39)
#define GPIO_UART_3_IOMUX_RX_FUNC                      IOMUX_PINCM40_PF_UART1_RX
#define GPIO_UART_3_IOMUX_TX_FUNC                      IOMUX_PINCM39_PF_UART1_TX
#define UART_3_BAUD_RATE                                                (115200)
#define UART_3_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_3_FBRD_40_MHZ_115200_BAUD                                      (45)




/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI1
#define SPI_0_INST_IRQHandler                                   SPI1_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI1_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                       DL_GPIO_PIN_8
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM25)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM25_PF_SPI1_PICO
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOB
#define GPIO_SPI_0_SCLK_PIN                                       DL_GPIO_PIN_9
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM26)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM26_PF_SPI1_SCLK
/* Defines for SPI_FOR_IMU */
#define SPI_FOR_IMU_INST                                                   SPI0
#define SPI_FOR_IMU_INST_IRQHandler                             SPI0_IRQHandler
#define SPI_FOR_IMU_INST_INT_IRQN                                 SPI0_INT_IRQn
#define GPIO_SPI_FOR_IMU_PICO_PORT                                        GPIOA
#define GPIO_SPI_FOR_IMU_PICO_PIN                                 DL_GPIO_PIN_9
#define GPIO_SPI_FOR_IMU_IOMUX_PICO                             (IOMUX_PINCM20)
#define GPIO_SPI_FOR_IMU_IOMUX_PICO_FUNC             IOMUX_PINCM20_PF_SPI0_PICO
#define GPIO_SPI_FOR_IMU_POCI_PORT                                        GPIOA
#define GPIO_SPI_FOR_IMU_POCI_PIN                                DL_GPIO_PIN_13
#define GPIO_SPI_FOR_IMU_IOMUX_POCI                             (IOMUX_PINCM35)
#define GPIO_SPI_FOR_IMU_IOMUX_POCI_FUNC             IOMUX_PINCM35_PF_SPI0_POCI
/* GPIO configuration for SPI_FOR_IMU */
#define GPIO_SPI_FOR_IMU_SCLK_PORT                                        GPIOA
#define GPIO_SPI_FOR_IMU_SCLK_PIN                                DL_GPIO_PIN_12
#define GPIO_SPI_FOR_IMU_IOMUX_SCLK                             (IOMUX_PINCM34)
#define GPIO_SPI_FOR_IMU_IOMUX_SCLK_FUNC             IOMUX_PINCM34_PF_SPI0_SCLK
#define GPIO_SPI_FOR_IMU_CS0_PORT                                         GPIOA
#define GPIO_SPI_FOR_IMU_CS0_PIN                                  DL_GPIO_PIN_2
#define GPIO_SPI_FOR_IMU_IOMUX_CS0                               (IOMUX_PINCM7)
#define GPIO_SPI_FOR_IMU_IOMUX_CS0_FUNC                IOMUX_PINCM7_PF_SPI0_CS0



/* Defines for ADC12_0 */
#define ADC12_0_INST                                                        ADC0
#define ADC12_0_INST_IRQHandler                                  ADC0_IRQHandler
#define ADC12_0_INST_INT_IRQN                                    (ADC0_INT_IRQn)
#define ADC12_0_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_0_ADCMEM_0_REF                     DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC12_0_ADCMEM_0_REF_VOLTAGE_V                                       3.3
#define GPIO_ADC12_0_C0_PORT                                               GPIOA
#define GPIO_ADC12_0_C0_PIN                                       DL_GPIO_PIN_27



/* Port definition for Pin Group GPIO_Key_up */
#define GPIO_Key_up_PORT                                                 (GPIOB)

/* Defines for PIN_13: GPIOB.13 with pinCMx 30 on package pin 1 */
#define GPIO_Key_up_PIN_13_PIN                                  (DL_GPIO_PIN_13)
#define GPIO_Key_up_PIN_13_IOMUX                                 (IOMUX_PINCM30)
/* Port definition for Pin Group GPIO_Key_down */
#define GPIO_Key_down_PORT                                               (GPIOB)

/* Defines for PIN_19: GPIOB.19 with pinCMx 45 on package pin 16 */
#define GPIO_Key_down_PIN_19_PIN                                (DL_GPIO_PIN_19)
#define GPIO_Key_down_PIN_19_IOMUX                               (IOMUX_PINCM45)
/* Port definition for Pin Group GPIO_Key_left */
#define GPIO_Key_left_PORT                                               (GPIOB)

/* Defines for PIN_23: GPIOB.23 with pinCMx 51 on package pin 22 */
#define GPIO_Key_left_PIN_23_PIN                                (DL_GPIO_PIN_23)
#define GPIO_Key_left_PIN_23_IOMUX                               (IOMUX_PINCM51)
/* Port definition for Pin Group GPIO_Key_right */
#define GPIO_Key_right_PORT                                              (GPIOB)

/* Defines for PIN_18: GPIOB.18 with pinCMx 44 on package pin 15 */
#define GPIO_Key_right_PIN_18_PIN                               (DL_GPIO_PIN_18)
#define GPIO_Key_right_PIN_18_IOMUX                              (IOMUX_PINCM44)
/* Port definition for Pin Group GPIO_Key_medium */
#define GPIO_Key_medium_PORT                                             (GPIOB)

/* Defines for PIN_24: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GPIO_Key_medium_PIN_24_PIN                              (DL_GPIO_PIN_24)
#define GPIO_Key_medium_PIN_24_IOMUX                             (IOMUX_PINCM52)
/* Port definition for Pin Group SPI_FOR_IMU_CS */
#define SPI_FOR_IMU_CS_PORT                                              (GPIOA)

/* Defines for PIN_8: GPIOA.8 with pinCMx 19 on package pin 54 */
#define SPI_FOR_IMU_CS_PIN_8_PIN                                 (DL_GPIO_PIN_8)
#define SPI_FOR_IMU_CS_PIN_8_IOMUX                               (IOMUX_PINCM19)
/* Port definition for Pin Group GPIO_BEEP */
#define GPIO_BEEP_PORT                                                   (GPIOB)

/* Defines for PIN_0: GPIOB.12 with pinCMx 29 on package pin 64 */
#define GPIO_BEEP_PIN_0_PIN                                     (DL_GPIO_PIN_12)
#define GPIO_BEEP_PIN_0_IOMUX                                    (IOMUX_PINCM29)
/* Port definition for Pin Group PORTB */
#define PORTB_PORT                                                       (GPIOB)

/* Defines for RST: GPIOB.10 with pinCMx 27 on package pin 62 */
#define PORTB_RST_PIN                                           (DL_GPIO_PIN_10)
#define PORTB_RST_IOMUX                                          (IOMUX_PINCM27)
/* Defines for DC: GPIOB.11 with pinCMx 28 on package pin 63 */
#define PORTB_DC_PIN                                            (DL_GPIO_PIN_11)
#define PORTB_DC_IOMUX                                           (IOMUX_PINCM28)
/* Defines for CS: GPIOB.14 with pinCMx 31 on package pin 2 */
#define PORTB_CS_PIN                                            (DL_GPIO_PIN_14)
#define PORTB_CS_IOMUX                                           (IOMUX_PINCM31)
/* Defines for BLK: GPIOB.26 with pinCMx 57 on package pin 28 */
#define PORTB_BLK_PIN                                           (DL_GPIO_PIN_26)
#define PORTB_BLK_IOMUX                                          (IOMUX_PINCM57)
/* Defines for E2A: GPIOA.29 with pinCMx 4 on package pin 36 */
#define Encoder_E2A_PORT                                                 (GPIOA)
// pins affected by this interrupt request:["E2A","E2B"]
#define Encoder_GPIOA_INT_IRQN                                  (GPIOA_INT_IRQn)
#define Encoder_GPIOA_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define Encoder_E2A_IIDX                                    (DL_GPIO_IIDX_DIO29)
#define Encoder_E2A_PIN                                         (DL_GPIO_PIN_29)
#define Encoder_E2A_IOMUX                                         (IOMUX_PINCM4)
/* Defines for E2B: GPIOA.30 with pinCMx 5 on package pin 37 */
#define Encoder_E2B_PORT                                                 (GPIOA)
#define Encoder_E2B_IIDX                                    (DL_GPIO_IIDX_DIO30)
#define Encoder_E2B_PIN                                         (DL_GPIO_PIN_30)
#define Encoder_E2B_IOMUX                                         (IOMUX_PINCM5)
/* Defines for E1B: GPIOB.5 with pinCMx 18 on package pin 53 */
#define Encoder_E1B_PORT                                                 (GPIOB)
// pins affected by this interrupt request:["E1B","E1A"]
#define Encoder_GPIOB_INT_IRQN                                  (GPIOB_INT_IRQn)
#define Encoder_GPIOB_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define Encoder_E1B_IIDX                                     (DL_GPIO_IIDX_DIO5)
#define Encoder_E1B_PIN                                          (DL_GPIO_PIN_5)
#define Encoder_E1B_IOMUX                                        (IOMUX_PINCM18)
/* Defines for E1A: GPIOB.6 with pinCMx 23 on package pin 58 */
#define Encoder_E1A_PORT                                                 (GPIOB)
#define Encoder_E1A_IIDX                                     (DL_GPIO_IIDX_DIO6)
#define Encoder_E1A_PIN                                          (DL_GPIO_PIN_6)
#define Encoder_E1A_IOMUX                                        (IOMUX_PINCM23)
/* Defines for IMU_INT1: GPIOB.7 with pinCMx 24 on package pin 59 */
#define Encoder_IMU_INT1_PORT                                            (GPIOB)
#define Encoder_IMU_INT1_PIN                                     (DL_GPIO_PIN_7)
#define Encoder_IMU_INT1_IOMUX                                   (IOMUX_PINCM24)
/* Defines for IMU_INT2: GPIOB.4 with pinCMx 17 on package pin 52 */
#define Encoder_IMU_INT2_PORT                                            (GPIOB)
#define Encoder_IMU_INT2_PIN                                     (DL_GPIO_PIN_4)
#define Encoder_IMU_INT2_IOMUX                                   (IOMUX_PINCM17)
/* Port definition for Pin Group Motor_diection */
#define Motor_diection_PORT                                              (GPIOA)

/* Defines for BIN1: GPIOA.26 with pinCMx 59 on package pin 30 */
#define Motor_diection_BIN1_PIN                                 (DL_GPIO_PIN_26)
#define Motor_diection_BIN1_IOMUX                                (IOMUX_PINCM59)
/* Defines for AIN1: GPIOA.24 with pinCMx 54 on package pin 25 */
#define Motor_diection_AIN1_PIN                                 (DL_GPIO_PIN_24)
#define Motor_diection_AIN1_IOMUX                                (IOMUX_PINCM54)
/* Defines for AIN2: GPIOA.25 with pinCMx 55 on package pin 26 */
#define Motor_diection_AIN2_PIN                                 (DL_GPIO_PIN_25)
#define Motor_diection_AIN2_IOMUX                                (IOMUX_PINCM55)
/* Defines for BIN2: GPIOA.22 with pinCMx 47 on package pin 18 */
#define Motor_diection_BIN2_PIN                                 (DL_GPIO_PIN_22)
#define Motor_diection_BIN2_IOMUX                                (IOMUX_PINCM47)
/* Port definition for Pin Group GPIO */
#define GPIO_PORT                                                        (GPIOA)

/* Defines for SCL: GPIOA.0 with pinCMx 1 on package pin 33 */
#define GPIO_SCL_PIN                                             (DL_GPIO_PIN_0)
#define GPIO_SCL_IOMUX                                            (IOMUX_PINCM1)
/* Defines for SDA: GPIOA.1 with pinCMx 2 on package pin 34 */
#define GPIO_SDA_PIN                                             (DL_GPIO_PIN_1)
#define GPIO_SDA_IOMUX                                            (IOMUX_PINCM2)
/* Defines for SO: GPIOA.15 with pinCMx 37 on package pin 8 */
#define Tracking_SO_PORT                                                 (GPIOA)
#define Tracking_SO_PIN                                         (DL_GPIO_PIN_15)
#define Tracking_SO_IOMUX                                        (IOMUX_PINCM37)
/* Defines for S1: GPIOA.28 with pinCMx 3 on package pin 35 */
#define Tracking_S1_PORT                                                 (GPIOA)
#define Tracking_S1_PIN                                         (DL_GPIO_PIN_28)
#define Tracking_S1_IOMUX                                         (IOMUX_PINCM3)
/* Defines for S2: GPIOB.21 with pinCMx 49 on package pin 20 */
#define Tracking_S2_PORT                                                 (GPIOB)
#define Tracking_S2_PIN                                         (DL_GPIO_PIN_21)
#define Tracking_S2_IOMUX                                        (IOMUX_PINCM49)
/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for LED_R: GPIOB.27 with pinCMx 58 on package pin 29 */
#define LED_LED_R_PIN                                           (DL_GPIO_PIN_27)
#define LED_LED_R_IOMUX                                          (IOMUX_PINCM58)
/* Defines for LED_B: GPIOB.16 with pinCMx 33 on package pin 4 */
#define LED_LED_B_PIN                                           (DL_GPIO_PIN_16)
#define LED_LED_B_IOMUX                                          (IOMUX_PINCM33)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_TIMER_2_init(void);
void SYSCFG_DL_UART_1_init(void);
void SYSCFG_DL_UART_2_init(void);
void SYSCFG_DL_UART_3_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_SPI_FOR_IMU_init(void);
void SYSCFG_DL_ADC12_0_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
