#ifndef _SEEKFREE_ASSISTANT_H_
#define _SEEKFREE_ASSISTANT_H_

#include "stdint.h"
#include "zf_common_fifo.h"



//typedef enum
//{
//    FIFO_SUCCESS,                                                               // FIFO operation success

//    FIFO_RESET_UNDO,                                                            // FIFO reset operation not executed
//    FIFO_CLEAR_UNDO,                                                            // FIFO clear operation not executed
//    FIFO_BUFFER_NULL,                                                           // FIFO user buffer exception
//    FIFO_WRITE_UNDO,                                                            // FIFO write operation not executed
//    FIFO_SPACE_NO_ENOUGH,                                                       // FIFO write operation insufficient space
//    FIFO_READ_UNDO,                                                             // FIFO read operation not executed
//    FIFO_DATA_NO_ENOUGH,                                                        // FIFO read operation insufficient data length
//}fifo_state_enum;                                                               // FIFO state enum
//// State machine logic
//// Reset operation   Must force FIFO to idle before use
//// Write operation   Must complete its own write operation before returning
//// Sequential read    Must complete clear and reset operations before returning
//// Tail read          Must complete clear and its own write operation before returning
//// Read and clear     Must complete clear and its own read operation before returning
//// Designed to prevent interrupt nesting from damaging data
//typedef enum
//{
//    FIFO_IDLE       = 0x00,                                                     // Idle state

//    FIFO_RESET      = 0x01,                                                     // Executing reset buffer
//    FIFO_CLEAR      = 0x02,                                                     // Executing clear buffer
//    FIFO_WRITE      = 0x04,                                                     // Executing write buffer
//    FIFO_READ       = 0x08,                                                     // Executing read buffer
//}fifo_execution_enum;                                                           // FIFO operation state Reserved for nesting use, cannot fully avoid concurrency


//typedef enum
//{
//    FIFO_DATA_8BIT,                                                             // FIFO data bit width 8bit
//    FIFO_DATA_16BIT,                                                            // FIFO data bit width 16bit
//    FIFO_DATA_32BIT,                                                            // FIFO data bit width 32bit
//}fifo_data_type_enum;
//typedef struct
//{
//    uint8_t               execution;                                              // Execution operation
//    fifo_data_type_enum type;                                                   // Data type
//    void                *buffer;                                                // Buffer pointer
//    uint32_t              head;                                                   // Buffer head pointer, always points to empty buffer
//    uint32_t             end;                                                    // Buffer tail pointer, always points to non-empty buffer (except when fully empty)
//    uint32_t              size;                                                   // Buffer remaining size
//    uint32_t              max;                                                    // Buffer total size
//}fifo_obj_struct;

typedef volatile uint8_t      vuint8_t;                                             // Hardware accessible, uncacheable 8 bits


// Assistant FIFO size
#define SEEKFREE_ASSISTANT_BUFFER_SIZE             ( 0x80 )

// Oscilloscope max channel count
#define SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT  ( 0x08 )

// Assistant tunable channel count
#define SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT      ( 0x08 )

// Image transmission max boundaries
#define SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY     ( 0x08 )

// Frame header sent from microcontroller to host computer
#define SEEKFREE_ASSISTANT_SEND_HEAD               ( 0xAA )

// Image transmission function
#define SEEKFREE_ASSISTANT_CAMERA_FUNCTION         ( 0x02 )
#define SEEKFREE_ASSISTANT_CAMERA_DOT_FUNCTION     ( 0x03 )
#define SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE     ( 0x10 )

// Frame header sent from host computer to microcontroller
#define SEEKFREE_ASSISTANT_RECEIVE_HEAD            ( 0x55 )

// Parameter settings
#define SEEKFREE_ASSISTANT_RECEIVE_SET_PARAMETER   ( 0x20 )

// Image transmission type enum
typedef enum
{
    // Image transmission camera model definitions
    SEEKFREE_ASSISTANT_OV7725_BIN = 1,
    SEEKFREE_ASSISTANT_MT9V03X,
    SEEKFREE_ASSISTANT_SCC8660,

    // Image transmission image type definitions
    SEEKFREE_ASSISTANT_BINARY = 1,
    SEEKFREE_ASSISTANT_GRAY,
    SEEKFREE_ASSISTANT_RGB565,
}seekfree_assistant_image_type_enum;

// Image transmission boundary type enum
typedef enum
{
    // Image transmission camera model definitions
    X_BOUNDARY,     // Transmitted image boundary info only includes X axis, i.e. only horizontal info, combined with image height
    Y_BOUNDARY,     // Transmitted image boundary info only includes Y axis, i.e. only vertical info, combined with image width via calculation
    XY_BOUNDARY,    // Transmitted image boundary info includes both X and Y axes. If multiple boundary positions are specified, an indicator light effect can be displayed
    NO_BOUNDARY,    // Transmitted image data has no boundary info
}seekfree_assistant_boundary_type_enum;

typedef struct
{
    uint8_t head;                                     // Frame header
    uint8_t channel_num;                              // High 4 bits: function; Low 4 bits: channel count
    uint8_t check_sum;                                // Checksum
    uint8_t length;                                   // Data length
    float data[SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT];    // Channel data
}seekfree_assistant_oscilloscope_struct;


typedef struct
{
    uint8_t head;                                     // Frame header
    uint8_t function;                                 // Function
    uint8_t camera_type;                              // High 4 bits: boundary type; Low 4 bits: whether image data is included. Example 0x13 means: 3 indicates one image with boundaries (general boundaries: left, right, bottom), 1 indicates no image data
    uint8_t length;                                   // Length (protocol section)
    uint16_t image_width;                             // Image width
    uint16_t image_height;                            // Image height
}seekfree_assistant_camera_struct;


typedef struct
{
    uint8_t head;                                     // Frame header
    uint8_t function;                                 // Function
    uint8_t dot_type;                                 // Point type: BIT5=1 uses 16-bit coords, 0 uses 8-bit; BIT7-BIT6: 0=X only, 1=Y only, 2=X and Y; BIT3-BIT0: boundary type
    uint8_t length;                                   // Length (protocol section)
    uint16_t dot_num;                                 // Point count
    uint8_t  valid_flag;                              // Channel identifier
    uint8_t  reserve;                                 // Reserved
}seekfree_assistant_camera_dot_struct;

typedef struct
{
    void *image_addr;                               // Camera image data address
    uint16_t width;                                   // Image width
    uint16_t height;                                  // Image height
    seekfree_assistant_image_type_enum camera_type;           // Camera type
    void *boundary_x[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY]; // Boundary result X coordinate address
    void *boundary_y[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY]; // Boundary result Y coordinate address
}seekfree_assistant_camera_buffer_struct;

typedef struct
{
    uint8_t head;                                     // Frame header
    uint8_t function;                                 // Function
    uint8_t channel;                                  // Channel
    uint8_t check_sum;                                // Checksum
    float data;                                     // Parameter
}seekfree_assistant_parameter_struct;


extern seekfree_assistant_oscilloscope_struct               seekfree_assistant_oscilloscope_data;                                               // Oscilloscope data
extern float                                                seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];                // Parameters received from host computer
extern vuint8_t                                             seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];    // Parameter update flags
extern fifo_obj_struct                 debug_uart_fifo;

void  seekfree_assistant_transfer                         (const unsigned char *buff, uint32_t length);
uint32_t  seekfree_assistant_receive                          (uint8_t *buff, uint32_t length);

void seekfree_assistant_init (void);
void seekfree_assistant_oscilloscope_send (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope);
void seekfree_assistant_data_analysis (void);
#endif
