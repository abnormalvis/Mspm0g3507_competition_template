#include "interrupt.h"
uint8_t encoder_left_timeslow;  // 定时器10ms读一次
uint8_t encoder_right_timeslow; // 定时器10ms读一次
uint8_t gray_timeslow;          // 定时器10ms读一次
uint8_t pid_timeslow;           // 定时器10ms读一次
uint8_t continue_pre_turn_timeslow;
uint8_t finish_pre_turn_timeslow;
uint8_t uartsend_slowtime;
void Handle_Key_Interrupt(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        // 按键中断
    case GPIO_MULTIPLE_GPIOA_INT_IRQN:
        delay_ms(10);
        if (DL_GPIO_readPins(KEY_PORT, KEY_KEY_6_PIN) == 0)
        {
            //					target_yaw += 10;
            target_speed += 10;
            if (target_speed == 80)
            {
                target_speed = 0;
            }
        }

        if (DL_GPIO_readPins(KEY_PORT, KEY_KEY_1_PIN) == 0)
        {
            task_flag = !task_flag;
        }
        break;
        //		case GPIO_MULTIPLE_GPIOA_INT_IIDX:
        //				if( DL_GPIO_readPins(KEY_KEY_4_PORT, KEY_KEY_4_PIN)== 0)
        //				{
        //	//					task_flag = !task_flag;
        //					target_speed -= 10;
        //					if(target_speed == -80)
        //					{
        //						target_speed = 0;
        //					}
        //				}
        //				break;
    default:
        break;
    }
}
void Left_Encoder_Interrupt(void)
{
    // 编码器中断
    uint32_t gpio_status;
    // 获取中断信号情况
    gpio_status = DL_GPIO_getEnabledInterruptStatus(Encoder_Left_PORT, Encoder_Left_A_PIN | Encoder_Left_B_PIN);

    // 编码器A相上升沿触发
    if ((gpio_status & Encoder_Left_A_PIN) == Encoder_Left_A_PIN)
    {
        // 如果在A相上升沿下，B相为低电平
        if (!DL_GPIO_readPins(Encoder_Left_PORT, Encoder_Left_B_PIN))
        {
            motor_left_encoder.temp_count--;
        }
        else
        {
            motor_left_encoder.temp_count++;
        }
    } // 编码器B相上升沿触发
    else if ((gpio_status & Encoder_Left_B_PIN) == Encoder_Left_B_PIN)
    {
        // 如果在B相上升沿下，A相为低电平
        if (!DL_GPIO_readPins(Encoder_Left_PORT, Encoder_Left_A_PIN))
        {
            motor_left_encoder.temp_count++;
        }
        else
        {
            motor_left_encoder.temp_count--;
        }
    }
    // 清除状态
    DL_GPIO_clearInterruptStatus(Encoder_Left_PORT, Encoder_Left_A_PIN | Encoder_Left_B_PIN);
}
void Right_Encoder_Interrupt(void)
{
    // 编码器中断
    uint32_t gpio_status;
    // 获取中断信号情况
    gpio_status = DL_GPIO_getEnabledInterruptStatus(Encoder_Right_PORT, Encoder_Right_C_PIN | Encoder_Right_D_PIN);

    // 编码器A相上升沿触发
    if ((gpio_status & Encoder_Right_C_PIN) == Encoder_Right_C_PIN)
    {
        // 如果在A相上升沿下，B相为低电平
        if (!DL_GPIO_readPins(Encoder_Right_PORT, Encoder_Right_D_PIN))
        {
            motor_right_encoder.temp_count++;
        }
        else
        {
            motor_right_encoder.temp_count--;
        }
    } // 编码器B相上升沿触发
    else if ((gpio_status & Encoder_Right_D_PIN) == Encoder_Right_D_PIN)
    {
        // 如果在B相上升沿下，A相为低电平
        if (!DL_GPIO_readPins(Encoder_Right_PORT, Encoder_Right_C_PIN))
        {
            motor_right_encoder.temp_count--;
        }
        else
        {
            motor_right_encoder.temp_count++;
        }
    }
    // 清除状态
    DL_GPIO_clearInterruptStatus(Encoder_Right_PORT, Encoder_Right_C_PIN | Encoder_Right_D_PIN);
}
/******************外部中断*************************************/
void GROUP1_IRQHandler(void)
{
    // 检查左编码器中断
    if (DL_GPIO_getEnabledInterruptStatus(Encoder_Left_PORT, Encoder_Left_A_PIN | Encoder_Left_B_PIN))
    {
        // 处理左编码器中断
        Left_Encoder_Interrupt();
    }

    // 检查右编码器中断
    if (DL_GPIO_getEnabledInterruptStatus(Encoder_Right_PORT, Encoder_Right_C_PIN | Encoder_Right_D_PIN))
    {
        // 处理右编码器中断
        Right_Encoder_Interrupt();
    }

    // 处理按键中断
    Handle_Key_Interrupt();
}

/******************定时器A0中断每一毫秒进入一次*************************************/
void TIMA0_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST))
    {
    case DL_TIMER_IIDX_LOAD:
        //			//循迹与转向
        //			if(++gray_timeslow == 15)
        //			{
        //				gray_timeslow = 5;
        //				if(motor_control_flag)
        //				{
        //					Pid_Motor_Control();
        //				}
        ////				track_value = Gray_Serial_Read();
        //				if(!turn_flag)
        //				{
        ////					Update_Tarck_Depart_Level();
        //				}
        ////				Turn();
        //			}
        // 左编码器
        if (++encoder_left_timeslow == 10)
        {
            encoder_left_timeslow = 0;
            Encoder_Update(&motor_left_encoder);
        }
        // 右编码器
        if (++encoder_right_timeslow == 11)
        {
            encoder_right_timeslow = 1;
            Encoder_Update(&motor_right_encoder);
        }
        break;
    default:
        break;
    }
}

/************************定时器A1中断每一毫秒进入一次**************************/
void TIMA1_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_1_INST))
    {
    case DL_TIMER_IIDX_LOAD:
        if (task_flag)
        {
            // PID控制器
            if (++pid_timeslow == 13)
            {
                pid_timeslow = 3;
                Pid_Motor_Control();
                //					if(turn_flag)
                //					{
                ////						Pid_Turn_Control();
                //						//转向结束判断
                //						if(Float_Abs(turn_pid.now-turn_pid.target)<=3)
                //						{
                //							turn_flag = 0;
                //						}
                //					}
                //					else
                //					{
                ////						Pid_Track_Control();
                //					}
                //				}
                //				//预转向(车头到达黑线):参数需要调整:目前为10编码器速度500ms路程
                //				if(pre_turn_flag)
                //				{
                //					if(++continue_pre_turn_timeslow == 10)
                //					{
                //						target_speed = 10;
                ////						Pid_Motor_Control();
                //						continue_pre_turn_timeslow = 0;
                //					}
                //					//转向环开启
                //					if(++finish_pre_turn_timeslow == 100)
                //					{
                //						finish_pre_turn_timeslow = 0;
                //						continue_pre_turn_timeslow = 0;
                //						pre_turn_flag = 0;
                //						turn_count++;
                //						turn_flag = 1;
                //						if(Yaw > 90)
                //						{
                //							turn_pid.target = Yaw - 270;
                //						}
                //						else
                //						{
                //							turn_pid.target = Yaw + 90;
                //						}
                //					}
            }
        }

        // 上位机调参发送数据帧
        if (++uartsend_slowtime == 10)
        {
            uartsend_slowtime = 0;
            DataVision_Send();
        }
        break;
    default:
        break;
    }
}

void UART_0_INST_IRQHandler(void)
{
    // 如果产生了串口中断
    switch (DL_UART_getPendingInterrupt(UART_0_INST))
    {
    case DL_UART_IIDX_RX: // 如果是接收中断

        break;
    default: // 其他的串口中断
        break;
    }
}

/**************************串口1中断**************************/
void JY61_P_INST_IRQHandler(void)
{
    // 如果产生了串口中断
    switch (DL_UART_getPendingInterrupt(JY61_P_INST))
    {
    case DL_UART_IIDX_RX: // 如果是接收中断
        // 接发送过来的数据保存在变量中
        jy61p_data = DL_UART_Main_receiveData(JY61_P_INST);
        jy61p_ReceiveData(jy61p_data);
        break;
    default: // 串口其他形式中断
        break;
    }
}
