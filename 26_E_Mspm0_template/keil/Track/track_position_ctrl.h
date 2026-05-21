#ifndef __TRACK_POSITION_CTRL_H
#define __TRACK_POSITION_CTRL_H


#include "ti_msp_dl_config.h" 
#include "StepperMotor.h"
#include "Delay.h"
#include "Servo.h"
#include "Laser.h"


void set_crawler_position(float posx,float posy);
void play_chess(float x1,float y1,float x2,float y2);

#endif
