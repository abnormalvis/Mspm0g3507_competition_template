#include "track_position_ctrl.h"

float last_posx = 0, last_posy = 0;

void set_crawler_position(float posx,float posy){
    if(posx >10.5){
        posx = 10.5;
    }
    if(posy >14){
        posy = 14;
    }
    StepperMotor1_SetPos((posx - last_posx)*90);
    StepperMotor2_SetPos(-(posy - last_posy)*90);
    last_posx = posx;
    last_posy = posy;
}

void play_chess(float x1,float y1,float x2,float y2){
    set_crawler_position(x1,y1);
    Delay_ms(1000);
    Servo_setAngle1(148);
    Delay_ms(250);
    Laser_1_on();
    Delay_ms(500);
    Servo_setAngle1(0);
    Delay_ms(800);
    set_crawler_position(x2,y2);
    Delay_ms(1000);
    Servo_setAngle1(148);
    Delay_ms(250);
    Laser_1_off();
    Delay_ms(800);
    Servo_setAngle1(0);
    Delay_ms(800);
    set_crawler_position(10.5,0);
    Delay_ms(2000);
}



