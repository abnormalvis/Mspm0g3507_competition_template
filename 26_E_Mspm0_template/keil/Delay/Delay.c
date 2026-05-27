#include "ti_msp_dl_config.h"
#include "Delay.h"

static uint32_t Delay_Count = 0, Delay_Last_Count = 0;
bool Count_Flag = 0;

void Delay_us(uint32_t us)
{
    hal_delay_us(us);
}

void Delay_ms(uint32_t ms)
{
    hal_delay_ms((uint16_t)ms);
}

void Delay_s(uint32_t s)
{
    while(s--)
    {
        hal_delay_ms(1000);
    }
}

int get_clock_ms(uint32_t *count)
{
    if (count == NULL)
        return 1;
    *count = tick_ms;
    return 0;
}

bool State_Delay(uint32_t T_cycle_ms , uint8_t Flag , uint8_t Flag_State , uint32_t Delay_ms )
{
    Delay_Count++;
    if(Count_Flag==0)
    {
        if(Flag==Flag_State)
        {
            Delay_Last_Count=Delay_Count;
            Count_Flag=1;
        }
        return false;
    }
    else if((Delay_Count - Delay_Last_Count) *T_cycle_ms < Delay_ms)
    {
        return true;
    }
    else
    {
        Count_Flag=0;
        return false;
    }
}
