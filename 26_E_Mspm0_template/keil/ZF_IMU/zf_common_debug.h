/*********************************************************************************************************************
* MSPM0G3507 Open Source Library - a lightweight open source library for official SDK interfaces
* Copyright (c) 2022 SEEKFREE (SeekFree Technology)
*
* This file is part of the MSPM0G3507 Open Source Library
*
* MSPM0G3507 Open Source Library is free software.
* You can redistribute it and/or modify it under the terms of the
* GPL (GNU General Public License) version 3 (GPL3.0) or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GPL for more details.
*
* You should have received a copy of the GPL along with this library.
* If not, see <https://www.gnu.org/licenses/>
*
* Important note:
* This source code uses the GPL3.0 open source license agreement.
* Please refer to the English version in GPL3_permission_statement.txt under libraries/doc directory.
* See the LICENSE file under the libraries directory for details.
* Welcome to use and distribute. When modifying, please retain the SeekFree copyright.
*
* File name          : zf_common_debug
* Company name       : Chengdu SeekFree Technology Co., Ltd.
* Version info       : See version file under libraries/doc directory
* Dev environment    : MDK 5.38a
* Target platform    : MSPM0G3507
* Contact            : https://seekfree.taobao.com/
********************************************************************************************************************/

#ifndef _zf_common_debug_h_
#define _zf_common_debug_h_

// zf_common header
#include "zf_common_typedef.h"

// This section lists currently supported functions
// Search in the source file for the actual definition, then navigate to the corresponding function
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// zf_assert                                                                    // Assertion macro
// zf_log                                                                       // Log output macro

// debug_read_ring_buffer                                                       // Read debug ring buffer data

// debug_message_handler                                                        // Debug output message processing
// debug_output_struct_init                                                     // Debug output struct init, generally not called directly by users
// debug_output_init                                                            // Debug output bind init, generally not called directly by users
// debug_init                                                                   // Debug serial port init
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//// This section configures the default serial port resource used. Users can modify as needed.
//// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//#define DEBUG_UART_INDEX                ( UART_1 )                              // Specify debug uart serial port number
//#define DEBUG_UART_BAUDRATE             ( 460800 )                              // Specify debug uart baud rate
//#define DEBUG_UART_TX_PIN               ( UART1_TX_A9  )                        // Specify debug uart TX pin
//#define DEBUG_UART_RX_PIN               ( UART1_RX_A10 )                        // Specify debug uart RX pin
//// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//// This section configures the serial port reception mode. Users can modify as needed.
//// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//#define DEBUG_UART_USE_INTERRUPT        ( 1 )                                   // Whether to enable debug uart serial interrupt
//#if DEBUG_UART_USE_INTERRUPT                                                    // If debug uart serial interrupt is enabled
//#define DEBUG_RING_BUFFER_LEN           ( 64 )                                  // Serial ring buffer size, default 64 bytes
//#endif
//// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section defines output levels. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define DEBUG_OUTPUT_LEVEL_DISABLE      ( 0 )                                   // Disable output level. When this define is used, no output is produced.
#define DEBUG_OUTPUT_LEVEL_LOG          ( 1 )                                   // Log output level. This define is for general informational messages and does NOT halt the program.
#define DEBUG_OUTPUT_LEVEL_ASSERT       ( 2 )                                   // Assertion output level. This define is for checking informational messages and will halt the program, unlike Log.
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section defines driver-level output levels. Users can modify as needed.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define ZF_DEBUG_OUTPUT_LEVEL_DRIVER    ( DEBUG_OUTPUT_LEVEL_ASSERT )           // Chip driver layer output level, defaults to assertion
#define ZF_DEBUG_OUTPUT_LEVEL_DEVICE    ( DEBUG_OUTPUT_LEVEL_ASSERT )           // Device layer output level, defaults to assertion
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section defines the main enums and structs for this file. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
typedef enum                                                                    // Enum: Debug object type. Do not modify.
{
    DEBUG_OBJ_TYPE_NULL             = 0,                                        // Default state when not initialized

    DEBUG_OBJ_TYPE_UART             = 1,                                        // Use serial port
    DEBUG_OBJ_TYPE_UART_INTERRUPT   = 2,                                        // Use serial port with interrupt enabled
    DEBUG_OBJ_TYPE_SCREEN           = 3,                                        // Use screen
}debug_obj_type_enum;

typedef struct                                                                  // Struct: Debug output object. Do not modify.
{
    debug_obj_type_enum     type_index;                                         // Type index

    uint16                  display_x_max;                                      // Screen display range X
    uint16                  display_y_max;                                      // Screen display range Y

    uint8                   font_x_size;                                        // Screen display font X size
    uint8                   font_y_size;                                        // Screen display font Y size

    void                    *device_obj;                                        //
    union                                                                       // Serial output function pointer / Screen string display function pointer, share the same address
    {
        void                (*output_uart)          (const char *str);
        void                (*output_screen)        (void *screen_obj, uint16 x, uint16 y, const char *str);
    };
    void                    (*output_screen_clear)  (void *screen_obj);         // Screen clear function pointer
}debug_obj_struct;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section defines the specific macros for chip driver layer output. Users can modify as needed.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// If the output level is disabled, directly return a constant value
// If the output level is enabled, output according to the corresponding level when an error occurs during driver operation
#if (DEBUG_OUTPUT_LEVEL_DISABLE == ZF_DEBUG_OUTPUT_LEVEL_DRIVER)
#define debug_message_driver_handler(pass, str) (pass ? ZF_NO_ERROR : !ZF_NO_ERROR)
#else
#define debug_message_driver_handler(pass, str) (pass ? ZF_NO_ERROR : debug_message_handler(ZF_DEBUG_OUTPUT_LEVEL_DRIVER, (pass), (str), ZF_FILE_MESSAGE, ZF_LINE_MESSAGE))
#endif
#if (DEBUG_OUTPUT_LEVEL_DISABLE == ZF_DEBUG_OUTPUT_LEVEL_DEVICE)
#define debug_message_device_handler(pass, str) (pass ? ZF_NO_ERROR : !ZF_NO_ERROR)
#else
#define debug_message_device_handler(pass, str) (pass ? ZF_NO_ERROR : debug_message_handler(ZF_DEBUG_OUTPUT_LEVEL_DEVICE, (pass), (str), ZF_FILE_MESSAGE, ZF_LINE_MESSAGE))
#endif
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section lists all function declarations [ including macro-defined functions ]. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assertion macro
// Description    : pass            Condition check. Returns no error if true; triggers assertion handling if false.
// Return Value   : uint8           ZF_NO_ERROR - 0 - OK / !ZF_NO_ERROR - !0 - Error
// Usage Example  : zf_assert(pass_flag);
// Note
//-------------------------------------------------------------------------------------------------------------------
#define zf_assert(pass)     (pass ? ZF_NO_ERROR : debug_message_handler(DEBUG_OUTPUT_LEVEL_ASSERT, (pass), (NULL), ZF_FILE_MESSAGE, ZF_LINE_MESSAGE))

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Log macro
// Description    : pass            Condition check. Returns no error if true; triggers log handling if false.
// Description    : str             Message to output when log is triggered
// Return Value   : uint8           ZF_NO_ERROR - 0 - OK / !ZF_NO_ERROR - !0 - Error
// Usage Example  : zf_log(pass_flag, "Pass flag set.");
// Note
//-------------------------------------------------------------------------------------------------------------------
#define zf_log(pass, str)   (pass ? ZF_NO_ERROR : debug_message_handler(DEBUG_OUTPUT_LEVEL_LOG, (pass), (str), ZF_FILE_MESSAGE, ZF_LINE_MESSAGE))

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read debug ring buffer data
// Description    : *data       Data buffer pointer for storing read data
// Return Value   : uint32      Actual length of data read
// Usage Example  : uint8 data[64]; uint32 len = debug_read_ring_buffer(data);
// Note           : Requires DEBUG_UART_USE_INTERRUPT macro definition; returns directly if not enabled
//-------------------------------------------------------------------------------------------------------------------
uint32 debug_read_ring_buffer (uint8 *data);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Debug output message processing
// Description    : output_level    output level
// Description    : pass            condition check
// Description    : *str            output message
// Description    : *file           file name
// Description    : line            target line number
// Return Value   : uint8           return pass check value
// Usage Example  : debug_message_handler(ZF_DEBUG_FILE, 0, "Error", ZF_DEBUG_FILE, ZF_DEBUG_LINE);
// Note           : This function is generally not called directly by users
//                  Recommended to use zf_log(x, str) interface in zf_commmon_debug.h
//-------------------------------------------------------------------------------------------------------------------
uint8 debug_message_handler (uint8 output_level, uint8 pass, char *str, char *file, int line);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Debug output struct init
// Description    : *info       debug output info struct
// Return Value   : void
// Usage Example  : debug_output_struct_init(info);
// Note           : This function is generally not called directly by users
//-------------------------------------------------------------------------------------------------------------------
void debug_output_struct_init (debug_obj_struct *obj);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Debug output bind init
// Description    : *info       debug output info struct
// Return Value   : void
// Usage Example  : debug_output_init(info);
// Note           : This function is generally not called directly by users
//-------------------------------------------------------------------------------------------------------------------
void debug_output_init (debug_obj_struct *obj);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Debug serial port init
// Description    : void
// Return Value   : void
// Usage Example  : debug_init();
// Note           : Default call in the open source demo, interrupt reception enabled by default
//-------------------------------------------------------------------------------------------------------------------
void debug_init (void);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#endif
