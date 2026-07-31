#include "Buzzer.h"

bool beep_on_1s_flag;
uint32_t Last_TimerCount;

void Buzzer_on(void){
		DL_GPIO_setPins(Beep_PORT, Beep_buzzer_PIN);     /* active-low: HIGH = buzzer off */
}

void Buzzer_off(void){
    DL_GPIO_clearPins(Beep_PORT, Beep_buzzer_PIN);   /* active-low: LOW = buzzer on */
}

void Buzzer_Init(void){
    BUZZER_OFF();   /* ensure buzzer starts silent */
}

//放1ms中断，参数是TimerCount,通过置一beep_on_1s_flag使蜂鸣器响1s
void Buzzer_on_1s(uint32_t TimerCount){
    static uint8_t beep1s_done = 0;
    if(beep_on_1s_flag == 1){
        Last_TimerCount = TimerCount;
        beep1s_done = 0;
        BUZZER_ON();
        beep_on_1s_flag = 0;
    }
    if (!beep1s_done && (TimerCount - Last_TimerCount >= 1000)) {
        BUZZER_OFF();
        beep1s_done = 1;
    }
}