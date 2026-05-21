#include "Buzzer.h"

bool beep_on_1s_flag;
uint32_t Last_TimerCount;

void Buzzer_on(){
    DL_GPIO_setPins(Beep_PORT, Beep_buzzer_PIN);
};

void Buzzer_off(){
    DL_GPIO_clearPins(Beep_PORT, Beep_buzzer_PIN);
};

//放1ms中断，参数是TimerCount,通过置一beep_on_1s_flag使蜂鸣器响1s
void Buzzer_on_1s(uint32_t TimerCount){
    if(beep_on_1s_flag == 1){
        Last_TimerCount = TimerCount;
        Buzzer_on();
        beep_on_1s_flag = 0;
    }
    if (TimerCount-Last_TimerCount >= 1000) {
        Buzzer_off();
    }
};