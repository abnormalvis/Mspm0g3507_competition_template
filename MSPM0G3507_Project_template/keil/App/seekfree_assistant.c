#include "seekfree_assistant.h"
#include "hal_uart.h"
#include "hal_vofa.h"


static fifo_obj_struct  seekfree_assistant_fifo;
static uint8_t        seekfree_assistant_buffer[SEEKFREE_ASSISTANT_BUFFER_SIZE];                                  // 数据存放数组
float               seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT] = {0};                  // 保存接收到的参数
vuint8_t              seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT] = {0};      // 参数更新标志位
seekfree_assistant_oscilloscope_struct          seekfree_assistant_oscilloscope_data;                         // 虚拟示波器数据

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手发送函数
// 参数说明     *buff           需要发送的数据地址
// 参数说明     length          需要发送的长度
// 返回参数     uint32_t          剩余未发送数据长度
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
 void seekfree_assistant_transfer (const uint8_t *buff, uint32_t length)
{

    UART1_send(buff, length);

}

fifo_obj_struct                 debug_uart_fifo;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     读取 debug 环形缓冲区数据
// 参数说明     *buff       读出数据存放的数组指针
// 参数说明     len         需要读取的长度
// 返回参数     uint32_t      读出数据的实际长度
// 使用示例
// 备注信息     本函数需要开启 DEBUG_UART_USE_INTERRUPT 宏定义才可使用
//-------------------------------------------------------------------------------------------------------------------
uint32_t debug_read_ring_buffer (uint8_t *buff, uint32_t len)
{
    fifo_read_buffer(&debug_uart_fifo, buff, &len, FIFO_READ_AND_CLEAN);
    return len;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手接收数据函数
// 参数说明     *buff           需要接收的数据地址
// 参数说明     length          要接收的数据最大长度
// 返回参数     uint32_t          接收到的数据长度
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
uint32_t seekfree_assistant_receive (uint8_t *buff, uint32_t length)
{
    uint32_t len = 0;

    len = debug_read_ring_buffer(buff, length);

    return len;
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手求和函数
// 参数说明     *buffer         需要校验的数据地址
// 参数说明     length          校验长度
// 返回参数     uint8_t           和值
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
static uint8_t seekfree_assistant_sum (uint8_t *buffer, uint32_t length)
{
    uint8_t temp_sum = 0;

    while(length--)
    {
        temp_sum += *buffer++;
    }

    return temp_sum;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手 初始化
// 参数说明
// 返回参数     void
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_init (void)
{
    fifo_init(&seekfree_assistant_fifo, FIFO_DATA_8BIT, seekfree_assistant_buffer, SEEKFREE_ASSISTANT_BUFFER_SIZE);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手解析接收到的数据
// 参数说明     void
// 返回参数     void
// 使用示例     函数只需要放到周期运行的PIT中断或者主循环即可
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_data_analysis (void)
{
    uint8_t  temp_sum;
    uint32_t read_length;
    seekfree_assistant_parameter_struct *receive_packet;

    // 这里使用uint32_t进行定义，目的是为了保证数组四字节对齐
    uint32_t  temp_buffer[SEEKFREE_ASSISTANT_BUFFER_SIZE / 4];

    // 尝试读取数据, 如果不是自定义的传输方式则从接收回调中读取数据
    read_length = seekfree_assistant_receive((uint8_t *)temp_buffer, SEEKFREE_ASSISTANT_BUFFER_SIZE);
		
    if(read_length)
    {
        // 将读取到的数据写入FIFO（原逐飞协议处理）
        fifo_write_buffer(&seekfree_assistant_fifo, (uint8_t *)temp_buffer, read_length);

        // 同步将原始字节交给 VOFA 解析器处理（在任务上下文，避免在 ISR 中解析浮点）
        uint8_t *p = (uint8_t *)temp_buffer;
        for(uint32_t i = 0; i < read_length; i++)
        {
            vofa_uart_rx_callback(p[i]);
        }
    }

		
    while(sizeof(seekfree_assistant_parameter_struct) <= fifo_used(&seekfree_assistant_fifo))
    {
        read_length = sizeof(seekfree_assistant_parameter_struct);
        fifo_read_buffer(&seekfree_assistant_fifo, (uint8_t *)temp_buffer, &read_length, FIFO_READ_ONLY);

        if(SEEKFREE_ASSISTANT_RECEIVE_HEAD != ((uint8_t *)temp_buffer)[0])
        {
            // 没有帧头则从FIFO中去掉第一个数据
            read_length = 1;
        }
        else
        {
            // 找到帧头
            receive_packet = (seekfree_assistant_parameter_struct *)temp_buffer;
            temp_sum = receive_packet->check_sum;
            receive_packet->check_sum = 0;
            if(temp_sum == seekfree_assistant_sum((uint8_t *)temp_buffer, sizeof(seekfree_assistant_parameter_struct)))
            {
                // 和校验成功保存数据
                seekfree_assistant_parameter[receive_packet->channel - 1] = receive_packet->data;
                seekfree_assistant_parameter_update_flag[receive_packet->channel - 1] = 1;
            }
            else
            {
                read_length = 1;
            }
        }


        // 丢弃无需使用的数据
        fifo_read_buffer(&seekfree_assistant_fifo, (uint8_t *)temp_buffer, &read_length, FIFO_READ_AND_CLEAN);
    }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手 虚拟示波器发送函数
// 参数说明     *seekfree_assistant_oscilloscope  示波器数据结构体
// 返回参数     void
// 使用示例     seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_oscilloscope_send (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope)
{
    uint8_t packet_size;

    // 将高四位清空
    seekfree_assistant_oscilloscope->channel_num &= 0x0f;


    // 帧头
    seekfree_assistant_oscilloscope->head         = SEEKFREE_ASSISTANT_SEND_HEAD;

    // 写入包长度信息
    packet_size                         = sizeof(seekfree_assistant_oscilloscope_struct) - (SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT - seekfree_assistant_oscilloscope->channel_num) * 4;
    seekfree_assistant_oscilloscope->length       = packet_size;

    // 写入功能字与通道数量
    seekfree_assistant_oscilloscope->channel_num |= SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE;

    // 和校验计算
    seekfree_assistant_oscilloscope->check_sum    = 0;
    seekfree_assistant_oscilloscope->check_sum    = seekfree_assistant_sum((uint8_t *)seekfree_assistant_oscilloscope, packet_size);

    // 数据在调用本函数之前，由用户将需要发送的数据写入seekfree_assistant_oscilloscope_data.data[]

    seekfree_assistant_transfer((const uint8_t *)seekfree_assistant_oscilloscope, packet_size);
}