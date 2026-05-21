/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include "ti_msp_dl_config.h"
#include "interrupt_config.h"
#include <stdio.h>
#include <string.h>
#include "Servo.h"
#include "Delay.h"
#include "StandardPid.h"

#include "OLED.h"
#include "Motor.h"
#include "Serial.h"
#include "vofa.h"
#include "Key.h"
#include "Encoder.h"
#include "Buzzer.h"
#include "Laser.h"
#include "StepperMotor.h"
#include "Navigation.h"
#include "Track.h"
#include "K230.h"
#include "Solve.h"
#include "track_position_ctrl.h"


uint32_t TimerCount=0;
uint8_t vofa_flag=0;
uint8_t task_pos_i=0;
uint8_t task2_pos[9] = {0};
float Pos_black[5][2]={
    0.8,0.1,3.0625,0.1,5.325,0.1,
    7.5875,0.1,9.85,0.1,
};
float Pos_white[5][2]={
    0.8,13.6,3.0625,13.6,5.325,13.6,
    7.5875,13.6,9.85,13.6,
};

float Pos_pieces_base[9][2]={
    2.2,3.8,2.2,6.9,2.2,10.0,
    5.375,3.8,5.375,6.9,5.375,10.0,
    8.55,3.8,8.55,6.9,8.55,10.0,
};
float Pos_pieces_task3[9][2]={0};
int pieces_state[9]={0};

int main(void)
{
    SYSCFG_DL_init();
    // 添加新中断要在这里添加NVIC
    Laser_1_off();
    interrupt_init();
    OLED_Init();
    Delay_ms(2000);//延迟300ms，消除垃圾数据
    set_crawler_position(10.5,0);
    Delay_ms(1000);
    Delay_ms(5000);
    // Servo_setAngle1(120);
    // Delay_ms(10000;)
   
    while (1)
    {
        if(Key_Num[0] == 0) {
            OLED_ShowString(1, 1, "task1");
            OLED_ShowString(2, 1, "wait Key 0");
        }
        if(Key_Num[0] == 1){
            play_chess(Pos_black[0][0],Pos_black[0][1],Pos_pieces_base[4][0],Pos_pieces_base[4][1]);
            OLED_Clear();
            Key_Num[0] = 2;
        }
        if(Key_Num[0] == 2){
            OLED_ShowChess(Key_Num[1] % 9);
            OLED_ShowString(4, 1, "task2 select");
            if(Key_Num[3] == 1){
                task2_pos[task_pos_i] = Key_Num[1] % 9;
                OLED_Clear();
                OLED_ShowString(1, 1, "task2_ok!");
                Delay_ms(2000);
                OLED_Clear();
                task_pos_i ++;
                Key_Num[1] = 0;
                Key_Num[3] = 0;
            }
        }
        if(Key_Num[0] == 3){
            task_pos_i = 0;Key_Num[1] = 0;
            play_chess(Pos_black[0][0],Pos_black[0][1],Pos_pieces_base[task2_pos[0]][0],Pos_pieces_base[task2_pos[0]][1]);
            play_chess(Pos_black[1][0],Pos_black[1][1],Pos_pieces_base[task2_pos[1]][0],Pos_pieces_base[task2_pos[1]][1]);
            play_chess(Pos_white[0][0],Pos_white[0][1],Pos_pieces_base[task2_pos[2]][0],Pos_pieces_base[task2_pos[2]][1]);
            play_chess(Pos_white[1][0],Pos_white[1][1],Pos_pieces_base[task2_pos[3]][0],Pos_pieces_base[task2_pos[3]][1]);
            Key_Num[0] = 4;
        }
        if(Key_Num[0] == 4){
            OLED_Clear();
            while(Key_Num[3] != 1){
                OLED_ShowString(1, 1, "wait Key_Num[3]!");
            }
            Key_Num[3] = 0;
            k230_send_string("STATE=3\n");
            OLED_Clear();
            while (k230_RxPacket[12] != 1234) {
                OLED_ShowString(1, 1, "task3_wait!");
            }
            cameraToReal();
            calcNineGridCenter();
            k230_RxPacket[12] = 0;
            OLED_Clear();
            OLED_ShowString(1, 1, "task3_k230_ok!");
            Delay_ms(1000);
            OLED_Clear();
            Key_Num[0] = 5;
        }
        if(Key_Num[0] == 5){
            OLED_ShowChess(Key_Num[1] % 9);
            if(Key_Num[3] == 1){
                task2_pos[task_pos_i] = Key_Num[1] % 9;
                OLED_Clear();
                OLED_ShowString(1, 1, "task3_ok!");
                Delay_ms(2000);
                OLED_Clear();
                task_pos_i ++;
                Key_Num[1] = 0;
                Key_Num[3] = 0;
            }
        }
        if(Key_Num[0] == 6){
            task_pos_i = 0;Key_Num[1] = 0;
            play_chess(Pos_black[0][0],Pos_black[0][1],Pos_pieces_task3[task2_pos[0]][0],Pos_pieces_task3[task2_pos[0]][1]);
            play_chess(Pos_black[4][0],Pos_black[4][1],Pos_pieces_task3[task2_pos[1]][0],Pos_pieces_task3[task2_pos[1]][1]);
            play_chess(Pos_white[0][0],Pos_white[0][1],Pos_pieces_task3[task2_pos[2]][0],Pos_pieces_task3[task2_pos[2]][1]);
            play_chess(Pos_white[4][0],Pos_white[4][1],Pos_pieces_task3[task2_pos[3]][0],Pos_pieces_task3[task2_pos[3]][1]);
            while(Key_Num[0] != 7){}    
        }
        if(Key_Num[0] == 7){
            task_pos_i = 0;
            OLED_Clear();
            OLED_ShowString(1, 1, "task4_start!");
            OLED_Clear();
            k230_send_string("STATE=4\n");
            while (1) {            
                if(Key_Num[3] >= 1){
                    k230_send_string("PHOTO=1\n");
                    LED_on();
                    Delay_ms(1000);
                    LED_off();
                    while (k230_RxPacket[9] != 1234) {
                        OLED_ShowString(4, 1, "task4_wait_k230!");
                    }            
                    for(int i = 0; i < 9; i++){
                        pieces_state[i] = (int)k230_RxPacket[i];
                    }
                    OLED_Clear();
                    OLED_ShowChess_pro(pieces_state);
                    check_board_change(pieces_state);
                    if(check_chess[0]){
                        play_chess(Pos_pieces_base[check_chess[2]][0],Pos_pieces_base[check_chess[2]][1],Pos_pieces_base[check_chess[1]][0],Pos_pieces_base[check_chess[1]][1]);
                    }else {
                        play_chess(Pos_black[task_pos_i][0],Pos_black[task_pos_i][1],Pos_pieces_base[best_move(pieces_state)][0],Pos_pieces_base[best_move(pieces_state)][1]);
                        task_pos_i++;
                    }
                    k230_RxPacket[9] = 0;
                    Key_Num[3] = 0;
                }
                if(Key_Num[0] == 8){
                    break;
                }
            }
        }
        if (Key_Num[0] == 8) {
            task_pos_i = 0;
            OLED_Clear();
            OLED_ShowString(1, 1, "task5_start!");
            
            // OLED_Clear();
            k230_send_string("STATR=5\n");
            while (1) {            
                if(Key_Num[3] == 1){
                    k230_send_string("PHOTO=1\n");
                    LED_on();
                    Delay_ms(1000);
                    LED_off();
                    while (k230_RxPacket[9] != 1234) {
                        // vofa_WaveSend(1, 1, 1, 1, 1, 1, 1, 1, 1);
                        OLED_ShowString(4, 1, "task5_wait_k230!");
                    }            
                    for(int i = 0; i < 9; i++){
                        pieces_state[i] = (int)k230_RxPacket[i];
                    }
                    OLED_Clear();
                    OLED_ShowChess_pro(pieces_state);
                    check_board_change(pieces_state);
                    if(check_chess[0]){
                        play_chess(Pos_pieces_base[check_chess[2]][0],Pos_pieces_base[check_chess[2]][1],Pos_pieces_base[check_chess[1]][0],Pos_pieces_base[check_chess[1]][1]);
                    }else {
                        play_chess(Pos_white[task_pos_i][0],Pos_white[task_pos_i][1],Pos_pieces_base[best_move(pieces_state)][0],Pos_pieces_base[best_move(pieces_state)][1]);
                        task_pos_i++;
                    }
                    k230_RxPacket[9] = 0;
                    Key_Num[3] = 0;
                }
            }
            Key_Num[0] = 0;
        }
    }
}

void TIMER_0_INST_IRQHandler(void){
    switch(DL_TimerG_getPendingInterrupt(TIMER_0_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            TimerCount++;
            
            //获取键码值
            if(TimerCount%20==0){
                Get_Key();
            }

            //vofa示波器
            if(TimerCount%100==0){
                vofa_flag=1;            
            }

            NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);    //清除中断标志位
            break;
        default :
            NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
            break;
    }
}






