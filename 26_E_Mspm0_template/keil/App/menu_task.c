#include "menu_task.h"
#include "lcd.h"
#include "hal_key.h"
#include "task_manager.h"
#include "StandardPid.h"
#include "Encoder.h"
#include <stdio.h>

/* ---- Global state ---- */
uint8_t  menu_active = 1;
uint8_t  task_running = 0;
uint8_t  g_stop_requested = 0;
TaskID   g_current_task = TASK_NONE;
float    g_motor_left_out = 0;
float    g_motor_right_out = 0;

/* ---- Task params (adjustable via menu) ---- */
int task1_max_laps = 1;
int task4_max_laps = 1;
int task_speed_base = 280;

/* ---- Key value from callback ---- */
static volatile KEY_VALUE_TYPEDEF g_key_val = KEY_IDLE_VAL;
static uint8_t func_index = 0;

/* ---- Forward declarations ---- */
static void fun_main0(void);
static void fun_main1(void);
static void fun_main2(void);
static void fun_main3(void);
static void fun_cfg1(void);
static void fun_cfg2(void);
static void fun_cfg3(void);
static void fun_cfg4(void);
static void fun_ok1(void);
static void fun_ok2(void);
static void fun_ok3(void);
static void fun_ok4(void);

/* ---- Key callback ---- */
static void MenuKeyCallback(KEY_VALUE_TYPEDEF keys)
{
    if (!menu_active && keys == KEY3_LONG_PRESS_RELEASE)
    {
        g_stop_requested = 1;
        return;
    }
    g_key_val = keys;
}

/* ---- Menu jump table ---- */
static Menu_table table[12] = {
    /* main menu: current, next, last, enter, function */
    {0, 3, 1, 4, (*fun_main0)},
    {1, 0, 2, 5, (*fun_main1)},
    {2, 1, 3, 6, (*fun_main2)},
    {3, 2, 0, 7, (*fun_main3)},

    /* config pages: self-loop on up/down, enter goes to OK */
    {4, 4, 4, 8,  (*fun_cfg1)},
    {5, 5, 5, 9,  (*fun_cfg2)},
    {6, 6, 6, 10, (*fun_cfg3)},
    {7, 7, 7, 11, (*fun_cfg4)},

    /* OK confirm pages: all keys stay on self */
    {8,  8, 8, 8, (*fun_ok1)},
    {9,  9, 9, 9, (*fun_ok2)},
    {10, 10, 10, 10, (*fun_ok3)},
    {11, 11, 11, 11, (*fun_ok4)},
};

void menu_init(void)
{
    hal_KeyScanCBSRegister(MenuKeyCallback);
    func_index = 0;
    menu_active = 1;
    task_running = 0;
    g_current_task = TASK_NONE;
    OLED_CLS();
}

void menu_key_set(void)
{
    KEY_VALUE_TYPEDEF key = g_key_val;
    uint8_t next_idx = func_index;

    switch (key)
    {
        case KEY1_CLICK_RELEASE:         next_idx = table[func_index].last;  break;
        case KEY2_CLICK_RELEASE:         next_idx = table[func_index].next;  break;
        case KEY4_CLICK_RELEASE:         next_idx = table[func_index].enter; break;
        case KEY3_CLICK_RELEASE:         next_idx = 0;                       break;
        case KEY3_LONG_PRESS_RELEASE:    next_idx = 0;                       break;
        default: break;
    }

    if (next_idx != func_index)
    {
        func_index = next_idx;
        OLED_CLS();
        g_key_val = KEY_IDLE_VAL;
    }

    if (table[func_index].current_operation)
    {
        table[func_index].current_operation();
    }
}

/* ======== Main menu rendering (4 tasks, arrow on selected) ======== */
static void fun_main0(void)
{
    display_6_8_string(0, 1, ">1.Circle Track");
    display_6_8_string(0, 2, " 2.Complex Trk");
    display_6_8_string(0, 3, " 3.Probe Task");
    display_6_8_string(0, 4, " 4.Multi-Lap");
}

static void fun_main1(void)
{
    display_6_8_string(0, 1, " 1.Circle Track");
    display_6_8_string(0, 2, ">2.Complex Trk");
    display_6_8_string(0, 3, " 3.Probe Task");
    display_6_8_string(0, 4, " 4.Multi-Lap");
}

static void fun_main2(void)
{
    display_6_8_string(0, 1, " 1.Circle Track");
    display_6_8_string(0, 2, " 2.Complex Trk");
    display_6_8_string(0, 3, ">3.Probe Task");
    display_6_8_string(0, 4, " 4.Multi-Lap");
}

static void fun_main3(void)
{
    display_6_8_string(0, 1, " 1.Circle Track");
    display_6_8_string(0, 2, " 2.Complex Trk");
    display_6_8_string(0, 3, " 3.Probe Task");
    display_6_8_string(0, 4, ">4.Multi-Lap");
}

/* ======== Config pages (parameter adjustment) ======== */
static void fun_cfg1(void)
{
    KEY_VALUE_TYPEDEF key = g_key_val;

    if (key == KEY1_CLICK_RELEASE)
    {
        task1_max_laps++;
        if (task1_max_laps > 5) task1_max_laps = 5;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }
    else if (key == KEY2_CLICK_RELEASE)
    {
        task1_max_laps--;
        if (task1_max_laps < 1) task1_max_laps = 1;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }

    display_6_8_string(0, 1, "Task1:Circle");
    display_6_8_string(0, 3, "Laps:");
    display_6_8_number(30, 3, task1_max_laps);
    display_6_8_string(0, 6, "UP/DOWN:+- RIGHT:OK");
}

static void fun_cfg2(void)
{
    KEY_VALUE_TYPEDEF key = g_key_val;

    if (key == KEY1_CLICK_RELEASE)
    {
        task_speed_base += 10;
        if (task_speed_base > 500) task_speed_base = 500;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }
    else if (key == KEY2_CLICK_RELEASE)
    {
        task_speed_base -= 10;
        if (task_speed_base < 100) task_speed_base = 100;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }

    display_6_8_string(0, 1, "Task2:Complex");
    display_6_8_string(0, 3, "Speed:");
    display_6_8_number(36, 3, task_speed_base);
    display_6_8_string(0, 6, "UP/DOWN:+- RIGHT:OK");
}

static void fun_cfg3(void)
{
    KEY_VALUE_TYPEDEF key = g_key_val;

    if (key == KEY1_CLICK_RELEASE)
    {
        task_speed_base += 10;
        if (task_speed_base > 500) task_speed_base = 500;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }
    else if (key == KEY2_CLICK_RELEASE)
    {
        task_speed_base -= 10;
        if (task_speed_base < 100) task_speed_base = 100;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }

    display_6_8_string(0, 1, "Task3:Probe");
    display_6_8_string(0, 3, "Speed:");
    display_6_8_number(36, 3, task_speed_base);
    display_6_8_string(0, 6, "UP/DOWN:+- RIGHT:OK");
}

static void fun_cfg4(void)
{
    KEY_VALUE_TYPEDEF key = g_key_val;

    if (key == KEY1_CLICK_RELEASE)
    {
        task4_max_laps++;
        if (task4_max_laps > 4) task4_max_laps = 4;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }
    else if (key == KEY2_CLICK_RELEASE)
    {
        task4_max_laps--;
        if (task4_max_laps < 1) task4_max_laps = 1;
        g_key_val = KEY_IDLE_VAL;
        OLED_CLS();
    }

    display_6_8_string(0, 1, "Task4:Multi-Lap");
    display_6_8_string(0, 3, "Laps:");
    display_6_8_number(30, 3, task4_max_laps);
    display_6_8_string(0, 6, "UP/DOWN:+- RIGHT:OK");
}

/* ======== OK confirm pages (activate task) ======== */
static void fun_ok1(void)
{
    display_6_8_string(0, 1, "TASK1 START");
    display_6_8_string(0, 3, "OK");
    g_current_task = TASK_ONE;
    menu_active = 0;
    task_running = 1;
    g_motor_left_out = 0;
    g_motor_right_out = 0;
}

static void fun_ok2(void)
{
    display_6_8_string(0, 1, "TASK2 START");
    display_6_8_string(0, 3, "OK");
    g_current_task = TASK_TWO;
    menu_active = 0;
    task_running = 1;
    g_motor_left_out = 0;
    g_motor_right_out = 0;
}

static void fun_ok3(void)
{
    display_6_8_string(0, 1, "TASK3 START");
    display_6_8_string(0, 3, "OK");
    g_current_task = TASK_THREE;
    menu_active = 0;
    task_running = 1;
    g_motor_left_out = 0;
    g_motor_right_out = 0;
}

static void fun_ok4(void)
{
    display_6_8_string(0, 1, "TASK4 START");
    display_6_8_string(0, 3, "OK");
    g_current_task = TASK_FOUR;
    menu_active = 0;
    task_running = 1;
    g_motor_left_out = 0;
    g_motor_right_out = 0;
}
