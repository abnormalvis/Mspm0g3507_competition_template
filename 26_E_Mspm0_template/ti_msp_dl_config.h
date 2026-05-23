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


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000



/* Defines for PWM_Motor */
#define PWM_Motor_INST                                                     TIMA0
#define PWM_Motor_INST_IRQHandler                               TIMA0_IRQHandler
#define PWM_Motor_INST_INT_IRQN                                 (TIMA0_INT_IRQn)
#define PWM_Motor_INST_CLK_FREQ                                         80000000
/* GPIO defines for channel 1 */
#define GPIO_PWM_Motor_C1_PORT                                             GPIOA
#define GPIO_PWM_Motor_C1_PIN                                      DL_GPIO_PIN_7
#define GPIO_PWM_Motor_C1_IOMUX                                  (IOMUX_PINCM14)
#define GPIO_PWM_Motor_C1_IOMUX_FUNC                 IOMUX_PINCM14_PF_TIMA0_CCP1
#define GPIO_PWM_Motor_C1_IDX                                DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 3 */
#define GPIO_PWM_Motor_C3_PORT                                             GPIOA
#define GPIO_PWM_Motor_C3_PIN                                     DL_GPIO_PIN_23
#define GPIO_PWM_Motor_C3_IOMUX                                  (IOMUX_PINCM53)
#define GPIO_PWM_Motor_C3_IOMUX_FUNC                 IOMUX_PINCM53_PF_TIMA0_CCP3
#define GPIO_PWM_Motor_C3_IDX                                DL_TIMER_CC_3_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                            (49U)
/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMA1)
#define TIMER_1_INST_IRQHandler                                 TIMA1_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMA1_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                          (4999U)



/* Defines for UART_debug */
#define UART_debug_INST                                                    UART0
#define UART_debug_INST_FREQUENCY                                       40000000
#define UART_debug_INST_IRQHandler                              UART0_IRQHandler
#define UART_debug_INST_INT_IRQN                                  UART0_INT_IRQn
#define GPIO_UART_debug_RX_PORT                                            GPIOA
#define GPIO_UART_debug_TX_PORT                                            GPIOA
#define GPIO_UART_debug_RX_PIN                                    DL_GPIO_PIN_11
#define GPIO_UART_debug_TX_PIN                                    DL_GPIO_PIN_10
#define GPIO_UART_debug_IOMUX_RX                                 (IOMUX_PINCM22)
#define GPIO_UART_debug_IOMUX_TX                                 (IOMUX_PINCM21)
#define GPIO_UART_debug_IOMUX_RX_FUNC                  IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_debug_IOMUX_TX_FUNC                  IOMUX_PINCM21_PF_UART0_TX
#define UART_debug_BAUD_RATE                                            (115200)
#define UART_debug_IBRD_40_MHZ_115200_BAUD                                  (21)
#define UART_debug_FBRD_40_MHZ_115200_BAUD                                  (45)
/* Defines for UART_3 */
#define UART_3_INST                                                        UART3
#define UART_3_INST_FREQUENCY                                           80000000
#define UART_3_INST_IRQHandler                                  UART3_IRQHandler
#define UART_3_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_3_RX_PORT                                                GPIOB
#define GPIO_UART_3_TX_PORT                                                GPIOB
#define GPIO_UART_3_RX_PIN                                         DL_GPIO_PIN_3
#define GPIO_UART_3_TX_PIN                                         DL_GPIO_PIN_2
#define GPIO_UART_3_IOMUX_RX                                     (IOMUX_PINCM16)
#define GPIO_UART_3_IOMUX_TX                                     (IOMUX_PINCM15)
#define GPIO_UART_3_IOMUX_RX_FUNC                      IOMUX_PINCM16_PF_UART3_RX
#define GPIO_UART_3_IOMUX_TX_FUNC                      IOMUX_PINCM15_PF_UART3_TX
#define UART_3_BAUD_RATE                                                  (9600)
#define UART_3_IBRD_80_MHZ_9600_BAUD                                       (520)
#define UART_3_FBRD_80_MHZ_9600_BAUD                                        (53)
/* Defines for UART_1 */
#define UART_1_INST                                                        UART1
#define UART_1_INST_FREQUENCY                                           40000000
#define UART_1_INST_IRQHandler                                  UART1_IRQHandler
#define UART_1_INST_INT_IRQN                                      UART1_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOA
#define GPIO_UART_1_RX_PIN                                        DL_GPIO_PIN_18
#define GPIO_UART_1_TX_PIN                                        DL_GPIO_PIN_17
#define GPIO_UART_1_IOMUX_RX                                     (IOMUX_PINCM40)
#define GPIO_UART_1_IOMUX_TX                                     (IOMUX_PINCM39)
#define GPIO_UART_1_IOMUX_RX_FUNC                      IOMUX_PINCM40_PF_UART1_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                      IOMUX_PINCM39_PF_UART1_TX
#define UART_1_BAUD_RATE                                                (115200)
#define UART_1_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_1_FBRD_40_MHZ_115200_BAUD                                      (45)




/* Defines for SPI_LCD */
#define SPI_LCD_INST                                                       SPI1
#define SPI_LCD_INST_IRQHandler                                 SPI1_IRQHandler
#define SPI_LCD_INST_INT_IRQN                                     SPI1_INT_IRQn
#define GPIO_SPI_LCD_PICO_PORT                                            GPIOB
#define GPIO_SPI_LCD_PICO_PIN                                    DL_GPIO_PIN_15
#define GPIO_SPI_LCD_IOMUX_PICO                                 (IOMUX_PINCM32)
#define GPIO_SPI_LCD_IOMUX_PICO_FUNC                 IOMUX_PINCM32_PF_SPI1_PICO
/* GPIO configuration for SPI_LCD */
#define GPIO_SPI_LCD_SCLK_PORT                                            GPIOB
#define GPIO_SPI_LCD_SCLK_PIN                                    DL_GPIO_PIN_16
#define GPIO_SPI_LCD_IOMUX_SCLK                                 (IOMUX_PINCM33)
#define GPIO_SPI_LCD_IOMUX_SCLK_FUNC                 IOMUX_PINCM33_PF_SPI1_SCLK
/* Defines for SPI_IMU */
#define SPI_IMU_INST                                                       SPI0
#define SPI_IMU_INST_IRQHandler                                 SPI0_IRQHandler
#define SPI_IMU_INST_INT_IRQN                                     SPI0_INT_IRQn
#define GPIO_SPI_IMU_PICO_PORT                                            GPIOA
#define GPIO_SPI_IMU_PICO_PIN                                     DL_GPIO_PIN_9
#define GPIO_SPI_IMU_IOMUX_PICO                                 (IOMUX_PINCM20)
#define GPIO_SPI_IMU_IOMUX_PICO_FUNC                 IOMUX_PINCM20_PF_SPI0_PICO
#define GPIO_SPI_IMU_POCI_PORT                                            GPIOA
#define GPIO_SPI_IMU_POCI_PIN                                    DL_GPIO_PIN_13
#define GPIO_SPI_IMU_IOMUX_POCI                                 (IOMUX_PINCM35)
#define GPIO_SPI_IMU_IOMUX_POCI_FUNC                 IOMUX_PINCM35_PF_SPI0_POCI
/* GPIO configuration for SPI_IMU */
#define GPIO_SPI_IMU_SCLK_PORT                                            GPIOA
#define GPIO_SPI_IMU_SCLK_PIN                                    DL_GPIO_PIN_12
#define GPIO_SPI_IMU_IOMUX_SCLK                                 (IOMUX_PINCM34)
#define GPIO_SPI_IMU_IOMUX_SCLK_FUNC                 IOMUX_PINCM34_PF_SPI0_SCLK
#define GPIO_SPI_IMU_CS0_PORT                                             GPIOA
#define GPIO_SPI_IMU_CS0_PIN                                      DL_GPIO_PIN_2
#define GPIO_SPI_IMU_IOMUX_CS0                                   (IOMUX_PINCM7)
#define GPIO_SPI_IMU_IOMUX_CS0_FUNC                    IOMUX_PINCM7_PF_SPI0_CS0



/* Defines for ADC_track */
#define ADC_track_INST                                                      ADC0
#define ADC_track_INST_IRQHandler                                ADC0_IRQHandler
#define ADC_track_INST_INT_IRQN                                  (ADC0_INT_IRQn)
#define ADC_track_ADCMEM_0                                    DL_ADC12_MEM_IDX_0
#define ADC_track_ADCMEM_0_REF                   DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_track_ADCMEM_0_REF_VOLTAGE_V                                     3.3
#define GPIO_ADC_track_C0_PORT                                             GPIOA
#define GPIO_ADC_track_C0_PIN                                     DL_GPIO_PIN_27



/* Port definition for Pin Group Beep */
#define Beep_PORT                                                        (GPIOB)

/* Defines for buzzer: GPIOB.12 with pinCMx 29 on package pin 64 */
#define Beep_buzzer_PIN                                         (DL_GPIO_PIN_12)
#define Beep_buzzer_IOMUX                                        (IOMUX_PINCM29)
/* Port definition for Pin Group KEY */
#define KEY_PORT                                                         (GPIOB)

/* Defines for KEY_UP: GPIOB.13 with pinCMx 30 on package pin 1 */
// groups represented: ["Encoder","KEY"]
// pins affected: ["Encoder1_A","Encoder1_B","KEY_UP","KEY_DOWN","KEY_LEFT","KEY_RIGHT","KEY_MID"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define KEY_KEY_UP_IIDX                                     (DL_GPIO_IIDX_DIO13)
#define KEY_KEY_UP_PIN                                          (DL_GPIO_PIN_13)
#define KEY_KEY_UP_IOMUX                                         (IOMUX_PINCM30)
/* Defines for KEY_DOWN: GPIOB.19 with pinCMx 45 on package pin 16 */
#define KEY_KEY_DOWN_IIDX                                   (DL_GPIO_IIDX_DIO19)
#define KEY_KEY_DOWN_PIN                                        (DL_GPIO_PIN_19)
#define KEY_KEY_DOWN_IOMUX                                       (IOMUX_PINCM45)
/* Defines for KEY_LEFT: GPIOB.23 with pinCMx 51 on package pin 22 */
#define KEY_KEY_LEFT_IIDX                                   (DL_GPIO_IIDX_DIO23)
#define KEY_KEY_LEFT_PIN                                        (DL_GPIO_PIN_23)
#define KEY_KEY_LEFT_IOMUX                                       (IOMUX_PINCM51)
/* Defines for KEY_RIGHT: GPIOB.18 with pinCMx 44 on package pin 15 */
#define KEY_KEY_RIGHT_IIDX                                  (DL_GPIO_IIDX_DIO18)
#define KEY_KEY_RIGHT_PIN                                       (DL_GPIO_PIN_18)
#define KEY_KEY_RIGHT_IOMUX                                      (IOMUX_PINCM44)
/* Defines for KEY_MID: GPIOB.24 with pinCMx 52 on package pin 23 */
#define KEY_KEY_MID_IIDX                                    (DL_GPIO_IIDX_DIO24)
#define KEY_KEY_MID_PIN                                         (DL_GPIO_PIN_24)
#define KEY_KEY_MID_IOMUX                                        (IOMUX_PINCM52)
/* Port definition for Pin Group MOTOR_DIR */
#define MOTOR_DIR_PORT                                                   (GPIOA)

/* Defines for Left_A: GPIOA.24 with pinCMx 54 on package pin 25 */
#define MOTOR_DIR_Left_A_PIN                                    (DL_GPIO_PIN_24)
#define MOTOR_DIR_Left_A_IOMUX                                   (IOMUX_PINCM54)
/* Defines for Left_B: GPIOA.25 with pinCMx 55 on package pin 26 */
#define MOTOR_DIR_Left_B_PIN                                    (DL_GPIO_PIN_25)
#define MOTOR_DIR_Left_B_IOMUX                                   (IOMUX_PINCM55)
/* Defines for Right_A: GPIOA.26 with pinCMx 59 on package pin 30 */
#define MOTOR_DIR_Right_A_PIN                                   (DL_GPIO_PIN_26)
#define MOTOR_DIR_Right_A_IOMUX                                  (IOMUX_PINCM59)
/* Defines for Right_B: GPIOA.22 with pinCMx 47 on package pin 18 */
#define MOTOR_DIR_Right_B_PIN                                   (DL_GPIO_PIN_22)
#define MOTOR_DIR_Right_B_IOMUX                                  (IOMUX_PINCM47)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOB)

/* Defines for SCL: GPIOB.9 with pinCMx 26 on package pin 61 */
#define OLED_SCL_PIN                                             (DL_GPIO_PIN_9)
#define OLED_SCL_IOMUX                                           (IOMUX_PINCM26)
/* Defines for SDA: GPIOB.8 with pinCMx 25 on package pin 60 */
#define OLED_SDA_PIN                                             (DL_GPIO_PIN_8)
#define OLED_SDA_IOMUX                                           (IOMUX_PINCM25)
/* Defines for S1: GPIOB.14 with pinCMx 31 on package pin 2 */
#define TRACK_S1_PORT                                                    (GPIOB)
#define TRACK_S1_PIN                                            (DL_GPIO_PIN_14)
#define TRACK_S1_IOMUX                                           (IOMUX_PINCM31)
/* Defines for S2: GPIOA.14 with pinCMx 36 on package pin 7 */
#define TRACK_S2_PORT                                                    (GPIOA)
#define TRACK_S2_PIN                                            (DL_GPIO_PIN_14)
#define TRACK_S2_IOMUX                                           (IOMUX_PINCM36)
/* Defines for S3: GPIOB.21 with pinCMx 49 on package pin 20 */
#define TRACK_S3_PORT                                                    (GPIOB)
#define TRACK_S3_PIN                                            (DL_GPIO_PIN_21)
#define TRACK_S3_IOMUX                                           (IOMUX_PINCM49)
/* Defines for Encoder1_A: GPIOB.6 with pinCMx 23 on package pin 58 */
#define Encoder_Encoder1_A_PORT                                          (GPIOB)
#define Encoder_Encoder1_A_IIDX                              (DL_GPIO_IIDX_DIO6)
#define Encoder_Encoder1_A_PIN                                   (DL_GPIO_PIN_6)
#define Encoder_Encoder1_A_IOMUX                                 (IOMUX_PINCM23)
/* Defines for Encoder1_B: GPIOB.5 with pinCMx 18 on package pin 53 */
#define Encoder_Encoder1_B_PORT                                          (GPIOB)
#define Encoder_Encoder1_B_IIDX                              (DL_GPIO_IIDX_DIO5)
#define Encoder_Encoder1_B_PIN                                   (DL_GPIO_PIN_5)
#define Encoder_Encoder1_B_IOMUX                                 (IOMUX_PINCM18)
/* Defines for Encoder2_A: GPIOA.29 with pinCMx 4 on package pin 36 */
#define Encoder_Encoder2_A_PORT                                          (GPIOA)
// pins affected by this interrupt request:["Encoder2_A","Encoder2_B"]
#define Encoder_GPIOA_INT_IRQN                                  (GPIOA_INT_IRQn)
#define Encoder_GPIOA_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define Encoder_Encoder2_A_IIDX                             (DL_GPIO_IIDX_DIO29)
#define Encoder_Encoder2_A_PIN                                  (DL_GPIO_PIN_29)
#define Encoder_Encoder2_A_IOMUX                                  (IOMUX_PINCM4)
/* Defines for Encoder2_B: GPIOA.30 with pinCMx 5 on package pin 37 */
#define Encoder_Encoder2_B_PORT                                          (GPIOA)
#define Encoder_Encoder2_B_IIDX                             (DL_GPIO_IIDX_DIO30)
#define Encoder_Encoder2_B_PIN                                  (DL_GPIO_PIN_30)
#define Encoder_Encoder2_B_IOMUX                                  (IOMUX_PINCM5)
/* Defines for IMU_CS: GPIOA.8 with pinCMx 19 on package pin 54 */
#define IMU_IMU_CS_PORT                                                  (GPIOA)
#define IMU_IMU_CS_PIN                                           (DL_GPIO_PIN_8)
#define IMU_IMU_CS_IOMUX                                         (IOMUX_PINCM19)
/* Defines for INT1: GPIOB.7 with pinCMx 24 on package pin 59 */
#define IMU_INT1_PORT                                                    (GPIOB)
#define IMU_INT1_PIN                                             (DL_GPIO_PIN_7)
#define IMU_INT1_IOMUX                                           (IOMUX_PINCM24)
/* Defines for INT2: GPIOB.4 with pinCMx 17 on package pin 52 */
#define IMU_INT2_PORT                                                    (GPIOB)
#define IMU_INT2_PIN                                             (DL_GPIO_PIN_4)
#define IMU_INT2_IOMUX                                           (IOMUX_PINCM17)
/* Defines for LCD_CS: GPIOA.15 with pinCMx 37 on package pin 8 */
#define LCD_LCD_CS_PORT                                                  (GPIOA)
#define LCD_LCD_CS_PIN                                          (DL_GPIO_PIN_15)
#define LCD_LCD_CS_IOMUX                                         (IOMUX_PINCM37)
/* Defines for RST: GPIOA.16 with pinCMx 38 on package pin 9 */
#define LCD_RST_PORT                                                     (GPIOA)
#define LCD_RST_PIN                                             (DL_GPIO_PIN_16)
#define LCD_RST_IOMUX                                            (IOMUX_PINCM38)
/* Defines for DC: GPIOB.17 with pinCMx 43 on package pin 14 */
#define LCD_DC_PORT                                                      (GPIOB)
#define LCD_DC_PIN                                              (DL_GPIO_PIN_17)
#define LCD_DC_IOMUX                                             (IOMUX_PINCM43)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_Motor_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_UART_debug_init(void);
void SYSCFG_DL_UART_3_init(void);
void SYSCFG_DL_UART_1_init(void);
void SYSCFG_DL_SPI_LCD_init(void);
void SYSCFG_DL_SPI_IMU_init(void);
void SYSCFG_DL_ADC_track_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
