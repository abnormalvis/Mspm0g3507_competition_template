#ifndef _SEEKFREE_ASSISTANT_H_
#define _SEEKFREE_ASSISTANT_H_

#include "stdint.h"
#include "zf_common_fifo.h" 



//typedef enum
//{
//    FIFO_SUCCESS,                                                               // FIFO 操作成功

//    FIFO_RESET_UNDO,                                                            // FIFO 重置操作未执行
//    FIFO_CLEAR_UNDO,                                                            // FIFO 清空操作未执行
//    FIFO_BUFFER_NULL,                                                           // FIFO 用户缓冲区异常
//    FIFO_WRITE_UNDO,                                                            // FIFO 写入操作未执行
//    FIFO_SPACE_NO_ENOUGH,                                                       // FIFO 写入操作 缓冲区空间不足
//    FIFO_READ_UNDO,                                                             // FIFO 读取操作未执行
//    FIFO_DATA_NO_ENOUGH,                                                        // FIFO 读取操作 数据长度不足
//}fifo_state_enum;                                                               // FIFO 操作结果
//// 操作逻辑
//// 整体重置操作   将会强制清空 FIFO 谨慎使用
//// 数据写入操作   不能在重置以及写入操作时进行
//// 顺序读取操作   不能在清空和重置操作时进行
//// 尾部读取操作   不能在清空和重置以及写入操作时进行
//// 读取清空操作   不能在清空和重置以及读取操作时进行
//// 这是为了防止中断嵌套导致数据混乱
//typedef enum
//{
//    FIFO_IDLE       = 0x00,                                                     // 空闲状态

//    FIFO_RESET      = 0x01,                                                     // 正在执行重置缓冲区
//    FIFO_CLEAR      = 0x02,                                                     // 正在执行清空缓冲区
//    FIFO_WRITE      = 0x04,                                                     // 正在执行写入缓冲区
//    FIFO_READ       = 0x08,                                                     // 正在执行读取缓冲区
//}fifo_execution_enum;                                                           // FIFO 操作状态 为嵌套使用预留 无法完全避免误操作


//typedef enum
//{
//    FIFO_DATA_8BIT,                                                             // FIFO 数据位宽 8bit
//    FIFO_DATA_16BIT,                                                            // FIFO 数据位宽 16bit
//    FIFO_DATA_32BIT,                                                            // FIFO 数据位宽 32bit
//}fifo_data_type_enum;
//typedef struct
//{
//    uint8_t               execution;                                              // 执行步骤
//    fifo_data_type_enum type;                                                   // 数据类型
//    void                *buffer;                                                // 缓存指针
//    uint32_t              head;                                                   // 缓存头指针 总是指向空的缓存
//    uint32_t             end;                                                    // 缓存尾指针 总是指向非空缓存（缓存全空除外）
//    uint32_t              size;                                                   // 缓存剩余大小
//    uint32_t              max;                                                    // 缓存总大小
//}fifo_obj_struct;

typedef volatile uint8_t      vuint8_t;                                             // 易变性修饰 无符号  8 bits


// 定义接收FIFO大小
#define SEEKFREE_ASSISTANT_BUFFER_SIZE             ( 0x80 )

// 定义示波器的最大通道数量
#define SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT  ( 0x08 )

// 定义参数调试的最大通道数量
#define SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT      ( 0x08 )

// 定义图像边线最大数量
#define SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY     ( 0x08 )

// 单片机往上位机发送的帧头
#define SEEKFREE_ASSISTANT_SEND_HEAD               ( 0xAA )

// 摄像头类
#define SEEKFREE_ASSISTANT_CAMERA_FUNCTION         ( 0x02 )
#define SEEKFREE_ASSISTANT_CAMERA_DOT_FUNCTION     ( 0x03 )
#define SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE     ( 0x10 )

// 上位机往单片机发送的帧头
#define SEEKFREE_ASSISTANT_RECEIVE_HEAD            ( 0x55 )

// 参数设置类
#define SEEKFREE_ASSISTANT_RECEIVE_SET_PARAMETER   ( 0x20 )

// 摄像头类型枚举
typedef enum
{
    // 按照摄像头型号定义
    SEEKFREE_ASSISTANT_OV7725_BIN = 1,
    SEEKFREE_ASSISTANT_MT9V03X,
    SEEKFREE_ASSISTANT_SCC8660,

    // 按照图像类型定义
    SEEKFREE_ASSISTANT_BINARY = 1,
    SEEKFREE_ASSISTANT_GRAY,
    SEEKFREE_ASSISTANT_RGB565,
}seekfree_assistant_image_type_enum;

// 摄像头类型枚举
typedef enum
{
    // 按照摄像头型号定义
    X_BOUNDARY,     // 发送的图像中边界信息只包含X，也就是只有横坐标信息，纵坐标根据图像高度得到
    Y_BOUNDARY,     // 发送的图像中边界信息只包含Y，也就是只有纵坐标信息，横坐标根据图像宽度得到，通常很少有这样的需求
    XY_BOUNDARY,    // 发送的图像中边界信息包含X与Y，这样可以指定点在任意位置，就可以方便显示出回弯的效果
    NO_BOUNDARY,    // 发送的图像中没有边线信息
}seekfree_assistant_boundary_type_enum;

typedef struct
{
    uint8_t head;                                     // 帧头
    uint8_t channel_num;                              // 高四位为功能字  低四位为通道数量
    uint8_t check_sum;                                // 和校验
    uint8_t length;                                   // 包长度
    float data[SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT];    // 通道数据
}seekfree_assistant_oscilloscope_struct;


typedef struct
{
    uint8_t head;                                     // 帧头
    uint8_t function;                                 // 功能字
    uint8_t camera_type;                              // 低四位表示边界数量 第四位表示是否有图像数据  例如0x13：其中3表示一副图像有三条边界（通常是左边界、中线、右边界）、1表示没有图像数据
    uint8_t length;                                   // 包长度（仅包含协议部分）
    uint16_t image_width;                             // 图像宽度
    uint16_t image_height;                            // 图像高度
}seekfree_assistant_camera_struct;


typedef struct
{
    uint8_t head;                                     // 帧头
    uint8_t function;                                 // 功能字
    uint8_t dot_type;                                 // 点类型  BIT5：1：坐标是16位的 0：坐标是8位的    BIT7-BIT6：0：只有X坐标 1：只有Y坐标 2：X和Y坐标都有    BIT3-BIT0：边界数量
    uint8_t length;                                   // 包长度（仅包含协议部分）
    uint16_t dot_num;                                 // 画点数量
    uint8_t  valid_flag;                              // 通道标识
    uint8_t  reserve;                                 // 保留
}seekfree_assistant_camera_dot_struct;

typedef struct
{
    void *image_addr;                               // 摄像头地址
    uint16_t width;                                   // 图像宽度
    uint16_t height;                                  // 图像高度
    seekfree_assistant_image_type_enum camera_type;           // 摄像头类型
    void *boundary_x[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY]; // 边界横坐标数组地址
    void *boundary_y[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY]; // 边界纵坐标数组地址
}seekfree_assistant_camera_buffer_struct;

typedef struct
{
    uint8_t head;                                     // 帧头
    uint8_t function;                                 // 功能字
    uint8_t channel;                                  // 通道
    uint8_t check_sum;                                // 和校验
    float data;                                     // 数据
}seekfree_assistant_parameter_struct;


extern seekfree_assistant_oscilloscope_struct               seekfree_assistant_oscilloscope_data;                                               // 虚拟示波器数据
extern float                                                seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];                // 保存接收到的参数
extern vuint8_t                                             seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];    // 参数更新标志位
extern fifo_obj_struct                 debug_uart_fifo;

void  seekfree_assistant_transfer                         (const unsigned char *buff, uint32_t length);
uint32_t  seekfree_assistant_receive                          (uint8_t *buff, uint32_t length);

void seekfree_assistant_init (void);
void seekfree_assistant_oscilloscope_send (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope);
void seekfree_assistant_data_analysis (void);
#endif

