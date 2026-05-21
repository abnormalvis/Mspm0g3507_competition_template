#include "Key.h"

static uint8_t Key_State[10]={1,1,1,1,1,1,1,1,1,1,},last_Key_State[10]={1,1,1,1,1,1,1,1,1,1,};
uint8_t Key_Num[10];


/**
  * @brief  按键按一次（抬起）键值++，在20ms中断调用,按键按下是低电平
  * @param  无
  * @retval 键值
  */

void Get_Key(){				
    uint8_t Key;			
	for(uint8_t i=0; i<5 ;i++){
        switch (i) {
            case 0:Key = (bool)DL_GPIO_readPins(Key_PORT, Key_Key1_PIN);break;
            case 1:Key = (bool)DL_GPIO_readPins(Key_PORT, Key_Key2_PIN);break;
            case 2:Key = (bool)DL_GPIO_readPins(Key_PORT, Key_Key3_PIN);break;
            case 3:Key = (bool)DL_GPIO_readPins(Key_PORT, Key_Key4_PIN);break;
        }
        Key_State[i]=Key;
        if(Key_State[i]==1&&last_Key_State[i]==0){
            Key_Num[i]++;
        }
        last_Key_State[i]=Key_State[i];
    }
}
