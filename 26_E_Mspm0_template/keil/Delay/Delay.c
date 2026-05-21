#include "ti_msp_dl_config.h"

volatile uint32_t tick_ms = 0;
static uint32_t Delay_Count = 0,Delay_Last_Count = 0;
bool Count_Flag = 0;

void Delay_us(uint32_t us){
    while(us--){
        delay_cycles(32);
    }
}
void Delay_ms(uint32_t ms){
    while(ms--){
        delay_cycles(32000);
    }
}
void Delay_s(uint32_t s){
    while(s--){
        Delay_ms(1000);
    }
}

// 简化版获取时钟（仅用于OLED超时判断，可选）
int get_clock_ms(uint32_t *count)
{
    if (count == NULL)
        return 1;
    *count = tick_ms; // 若不需要计数，可返回0或固定值
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Flag与Flag_State相等时延时 Delay_ms
// 参数说明     T_cycle_ms          定时器中断周期
//            Flag                标志位
//            Flag_State          目标标志位
//            Delay_ms            延时毫秒数
// 参数说明     data            数据
// 返回参数     void
// 使用示例     State_Delay(1,Flag,2,1000);
// 备注信息     定时器中断调用
//-------------------------------------------------------------------------------------------------------------------

bool State_Delay(uint32_t T_cycle_ms , uint8_t Flag , uint8_t Flag_State , uint32_t Delay_ms ){
    Delay_Count++;
    if(Count_Flag==0){
        if(Flag==Flag_State){
            Delay_Last_Count=Delay_Count;
            Count_Flag=1;
        }
        return false;
    }else if((Delay_Count - Delay_Last_Count) *T_cycle_ms < Delay_ms){
        return true;
    }else{
        Count_Flag=0;
        return false;
    }
}