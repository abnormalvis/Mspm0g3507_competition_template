#include "duty_chess.h"
#include "lcd.h"
#include "Delay.h"
#include "Laser.h"
#include "K230.h"
#include "Solve.h"
#include "Servo.h"
#include "StepperMotor.h"
#include "track_position_ctrl.h"

float Pos_black[5][2] = {
    {0.8f,  0.1f}, {3.0625f, 0.1f}, {5.325f, 0.1f},
    {7.5875f, 0.1f}, {9.85f, 0.1f}
};
float Pos_white[5][2] = {
    {0.8f, 13.6f}, {3.0625f, 13.6f}, {5.325f, 13.6f},
    {7.5875f, 13.6f}, {9.85f, 13.6f}
};
float Pos_pieces_base[9][2] = {
    {2.2f, 3.8f}, {2.2f,  6.9f}, {2.2f, 10.0f},
    {5.375f, 3.8f}, {5.375f, 6.9f}, {5.375f, 10.0f},
    {8.55f, 3.8f}, {8.55f,  6.9f}, {8.55f, 10.0f},
};
int   pieces_state[9] = { 0 };

static volatile uint8_t  s_step       = 0;
static volatile uint8_t  s_pos_cursor = 0;
static volatile uint8_t  s_confirm    = 0;
static volatile uint8_t  s_advance    = 0;
static          uint8_t  s_task_pos[9] = { 0 };
static          uint8_t  s_task_pos_i  = 0;

void duty_chess_init(void)
{
    s_step = 0;
    s_pos_cursor = 0;
    s_confirm = 0;
    s_advance = 0;
    s_task_pos_i = 0;
    for (uint8_t i = 0; i < 9; i++) s_task_pos[i] = 0;
}

/**
 * 按键事件 → 棋局子状态推进
 *  KEY1_CLICK 上 : 步进 +1（光标+1）
 *  KEY2_CLICK 下 : 步进 -1
 *  KEY4_CLICK 确认: s_confirm=1
 *  KEY5_CLICK 中 : s_advance=1（推进到下一个 step）
 */
void duty_chess_on_key(KEY_VALUE_TYPEDEF k)
{
    switch (k)
    {
        case KEY1_CLICK: s_pos_cursor = (uint8_t)((s_pos_cursor + 1) % 9); break;
        case KEY2_CLICK: s_pos_cursor = (uint8_t)((s_pos_cursor + 8) % 9); break;
        case KEY4_CLICK: s_confirm = 1; break;
        case KEY5_CLICK: s_advance = 1; break;
        default: break;
    }
}

static void wait_advance(void)
{
    while (!s_advance) { /* spin until KEY5 click */ }
    s_advance = 0;
}

static void wait_confirm(void)
{
    while (!s_confirm) { /* spin until KEY4 click */ }
    s_confirm = 0;
}

void duty_chess_run(void)
{
    Laser_1_off();
    OLED_CLS();
    Delay_ms(2000);
    set_crawler_position(10.5f, 0.0f);
    Delay_ms(1000);
    Delay_ms(5000);

    while (1)
    {
        if (s_step == 0)
        {
            OLED_ShowString(1, 1, "task1");
            OLED_ShowString(2, 1, "wait Key 0");
            wait_advance();
            s_step = 1;
        }
        if (s_step == 1)
        {
            play_chess(Pos_black[0][0], Pos_black[0][1],
                       Pos_pieces_base[4][0], Pos_pieces_base[4][1]);
            OLED_CLS();
            s_step = 2;
        }
        if (s_step == 2)
        {
            OLED_ShowChess(s_pos_cursor);
            OLED_ShowString(4, 1, "task2 select");
            if (s_confirm)
            {
                s_task_pos[s_task_pos_i] = s_pos_cursor;
                OLED_CLS();
                OLED_ShowString(1, 1, "task2_ok!");
                Delay_ms(2000);
                OLED_CLS();
                s_task_pos_i++;
                s_pos_cursor = 0;
                s_confirm = 0;
                if (s_task_pos_i >= 4) { s_task_pos_i = 0; s_step = 3; }
            }
        }
        if (s_step == 3)
        {
            play_chess(Pos_black[0][0], Pos_black[0][1], Pos_pieces_base[s_task_pos[0]][0], Pos_pieces_base[s_task_pos[0]][1]);
            play_chess(Pos_black[1][0], Pos_black[1][1], Pos_pieces_base[s_task_pos[1]][0], Pos_pieces_base[s_task_pos[1]][1]);
            play_chess(Pos_white[0][0], Pos_white[0][1], Pos_pieces_base[s_task_pos[2]][0], Pos_pieces_base[s_task_pos[2]][1]);
            play_chess(Pos_white[1][0], Pos_white[1][1], Pos_pieces_base[s_task_pos[3]][0], Pos_pieces_base[s_task_pos[3]][1]);
            s_step = 4;
        }
        if (s_step == 4)
        {
            OLED_CLS();
            OLED_ShowString(1, 1, "wait Key4!");
            wait_confirm();
            k230_send_string("STATE=3\n");
            OLED_CLS();
            while (k230_RxPacket[12] != 1234)
            {
                OLED_ShowString(1, 1, "task3_wait!");
            }
            cameraToReal();
            calcNineGridCenter();
            k230_RxPacket[12] = 0;
            OLED_CLS();
            OLED_ShowString(1, 1, "task3_k230_ok!");
            Delay_ms(1000);
            OLED_CLS();
            s_step = 5;
        }
        if (s_step == 5)
        {
            OLED_ShowChess(s_pos_cursor);
            if (s_confirm)
            {
                s_task_pos[s_task_pos_i] = s_pos_cursor;
                OLED_CLS();
                OLED_ShowString(1, 1, "task3_ok!");
                Delay_ms(2000);
                OLED_CLS();
                s_task_pos_i++;
                s_pos_cursor = 0;
                s_confirm = 0;
                if (s_task_pos_i >= 4) { s_task_pos_i = 0; s_step = 6; }
            }
        }
        if (s_step == 6)
        {
            play_chess(Pos_black[0][0], Pos_black[0][1], Pos_pieces_task3[s_task_pos[0]][0], Pos_pieces_task3[s_task_pos[0]][1]);
            play_chess(Pos_black[4][0], Pos_black[4][1], Pos_pieces_task3[s_task_pos[1]][0], Pos_pieces_task3[s_task_pos[1]][1]);
            play_chess(Pos_white[0][0], Pos_white[0][1], Pos_pieces_task3[s_task_pos[2]][0], Pos_pieces_task3[s_task_pos[2]][1]);
            play_chess(Pos_white[4][0], Pos_white[4][1], Pos_pieces_task3[s_task_pos[3]][0], Pos_pieces_task3[s_task_pos[3]][1]);
            wait_advance();
            s_step = 7;
        }
        if (s_step == 7)
        {
            s_task_pos_i = 0;
            OLED_CLS();
            OLED_ShowString(1, 1, "task4_start!");
            OLED_CLS();
            k230_send_string("STATE=4\n");
            while (s_step == 7)
            {
                if (s_confirm)
                {
                    k230_send_string("PHOTO=1\n");
                    LED_on();
                    Delay_ms(1000);
                    LED_off();
                    while (k230_RxPacket[9] != 1234)
                    {
                        OLED_ShowString(4, 1, "task4_wait_k230!");
                    }
                    for (int i = 0; i < 9; i++)
                        pieces_state[i] = (int)k230_RxPacket[i];
                    OLED_CLS();
                    OLED_ShowChess_pro(pieces_state);
                    check_board_change(pieces_state);
                    if (check_chess[0])
                    {
                        play_chess(Pos_pieces_base[check_chess[2]][0], Pos_pieces_base[check_chess[2]][1],
                                   Pos_pieces_base[check_chess[1]][0], Pos_pieces_base[check_chess[1]][1]);
                    }
                    else
                    {
                        play_chess(Pos_black[s_task_pos_i][0], Pos_black[s_task_pos_i][1],
                                   Pos_pieces_base[best_move(pieces_state)][0],
                                   Pos_pieces_base[best_move(pieces_state)][1]);
                        s_task_pos_i++;
                    }
                    k230_RxPacket[9] = 0;
                    s_confirm = 0;
                }
                if (s_advance) { s_advance = 0; s_step = 8; }
            }
        }
        if (s_step == 8)
        {
            s_task_pos_i = 0;
            OLED_CLS();
            OLED_ShowString(1, 1, "task5_start!");
            k230_send_string("STATE=5\n");
            while (s_step == 8)
            {
                if (s_confirm)
                {
                    k230_send_string("PHOTO=1\n");
                    LED_on();
                    Delay_ms(1000);
                    LED_off();
                    while (k230_RxPacket[9] != 1234)
                    {
                        OLED_ShowString(4, 1, "task5_wait_k230!");
                    }
                    for (int i = 0; i < 9; i++)
                        pieces_state[i] = (int)k230_RxPacket[i];
                    OLED_CLS();
                    OLED_ShowChess_pro(pieces_state);
                    check_board_change(pieces_state);
                    if (check_chess[0])
                    {
                        play_chess(Pos_pieces_base[check_chess[2]][0], Pos_pieces_base[check_chess[2]][1],
                                   Pos_pieces_base[check_chess[1]][0], Pos_pieces_base[check_chess[1]][1]);
                    }
                    else
                    {
                        play_chess(Pos_white[s_task_pos_i][0], Pos_white[s_task_pos_i][1],
                                   Pos_pieces_base[best_move(pieces_state)][0],
                                   Pos_pieces_base[best_move(pieces_state)][1]);
                        s_task_pos_i++;
                    }
                    k230_RxPacket[9] = 0;
                    s_confirm = 0;
                }
                if (s_advance) { s_advance = 0; s_step = 0; return; }
            }
        }
    }
}
