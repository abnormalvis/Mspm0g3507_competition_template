#ifndef __HEADFILE_H__
#define __HEADFILE_H__
//zigbee：自身RX：PA26；自身TX：PA25
//K210：自身RX：PA8；自身TX：PA9
//JY61P：自身RX：PB17， 自身TX：PB16
//调试(或总线舵机)：模块自身RX：PA0；自身TX：PA1
//OLED：SDA：PA10；SCL：PA11
//循迹：SDA：PA28；SCL：PA31
//TB6612：PWMA：PA12(TIMG0C0)；PWMB：PA13(TIMG0C1)
//左编码器：PA18(TIMG7C1)；PA17(TIMA1C0)
//右编码器：PB19(TIMG8C1)；PB21(TIMG8C0)
//KEY1~6：PB27、PB22、PB1、PA29、PB23、PB20
//超声波：Trig：PB25；Echo：PB24
//烧录器：PA20；PA19
//蜂鸣器：PB13
//TB6612：
//​	AIN1：PA24；AIN2：PA27
//​	BIN1：PA07；BIN2：PA16
//LED1~3：PB3、PB4、PB5
#include "ti_msp_dl_config.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "interrupt.h"
#include "delay.h"
#include "oled.h"
#include "jy61p.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "track.h"

#include "track_control.h"
#include "turn_control.h"
#include "motor_control.h"

#include "soft_i2c.h"
#include "nchd12.h"
#include "gray_detection.h"

extern uint8_t jy61p_data;
extern float coord_x;
extern float coord_y;
extern uint8_t task_flag;
extern float target_yaw;
extern uint8_t motor_control_flag;


#endif
