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


//// Include header files
#include "zf_common_debug.h"
#include "ti_msp_dl_config.h"
//// This section lists all function definitions in this file. Do not modify.
//// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug delay function, used when the microcontroller needs to relay clock test parameters to estimate a time delay
//// Parameter       : void
//// Return Value    : void
//// Usage Example   : debug_delay();
//// Note            : This is an internal function. Users do not need to pay attention and should not modify it.
////-------------------------------------------------------------------------------------------------------------------
//static void debug_delay (void)
//{
//    vuint32 loop_1 = 0, loop_2 = 0;
//    for(loop_1 = 0; 0xFF >= loop_1; loop_1 ++)
//    {
//        for(loop_2 = 0; 0xFFFF >= loop_2; loop_2 ++)
//        {
//            __NOP();
//        }
//    }
//}

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug idle function, mainly to prevent the debug output pin from holding a level and damaging the hardware
//// Parameter       : void
//// Return Value    : void
//// Usage Example   : debug_protective_handler();
//// Note            : This is an internal function. Users do not need to pay attention and should not modify it.
////-------------------------------------------------------------------------------------------------------------------
//static void debug_protective_handler (void)
//{
//}

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug string output interface
//// Parameter       : *str        String to output
//// Return Value    : void
//// Usage Example   : debug_uart_str_output("Log message");
//// Note            : This is an internal function. Users do not need to pay attention and should not modify it.
////-------------------------------------------------------------------------------------------------------------------
//static void debug_uart_str_output (const char *str)
//{
//}

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug output interface
//// Parameter       : *type       log type
//// Parameter       : *file       file name
//// Parameter       : line        target line number
//// Parameter       : *str        message
//// Return Value    : void
//// Usage Example   : debug_output("Log message", file, line, str);
//// Note            : This is an internal function. Users do not need to pay attention and should not modify it.
////-------------------------------------------------------------------------------------------------------------------
//static void debug_output (char *type, char *file, int line, char *str)
//{
//}

//#if DEBUG_UART_USE_INTERRUPT                                                    // Conditional compilation: only compiled when serial interrupt is enabled
////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug serial interrupt handler, called in isr.c from the corresponding serial interrupt service routine
//// Parameter       : void
//// Return Value    : void
//// Usage Example   : debug_interrupr_handler();
//// Note            : Requires DEBUG_UART_USE_INTERRUPT macro definition to be enabled for use
////-------------------------------------------------------------------------------------------------------------------
//static void debug_interrupr_handler (uint32 event, void *ptr)
//{
//}
//#endif

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : Read debug serial ring buffer data
//// Parameter       : *data       Data buffer pointer for storing read data
//// Return Value    : uint32      Actual length of data read
//// Usage Example   : uint8 data[64]; uint32 len = debug_read_ring_buffer(data);
//// Note            : Requires DEBUG_UART_USE_INTERRUPT macro definition; returns directly if not enabled
////-------------------------------------------------------------------------------------------------------------------
//uint32 debug_read_ring_buffer (uint8 *data)
//{
//    return 0;
//}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : debug output message processing
// Parameter       : output_level    output level
// Parameter       : pass            condition check
// Parameter       : *str            output message
// Parameter       : *file           file name
// Parameter       : line            target line number
// Return Value    : uint8           return pass check value
// Usage Example   : debug_message_handler(ZF_DEBUG_FILE, 0, "Error", ZF_DEBUG_FILE, ZF_DEBUG_LINE);
// Note            : This function is generally not called directly by users
//                   Recommended to use zf_log(x, str) interface in zf_commmon_debug.h
//-------------------------------------------------------------------------------------------------------------------
uint8 debug_message_handler (uint8 output_level, uint8 pass, char *str, char *file, int line)
{
    return !pass;
}

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug output struct init
//// Parameter       : *info       debug output info struct
//// Return Value    : void
//// Usage Example   : debug_output_struct_init(info);
//// Note            : This function is generally not called directly by users
////-------------------------------------------------------------------------------------------------------------------
//void debug_output_struct_init (debug_obj_struct *obj)
//{
//}

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug output bind init
//// Parameter       : *info       debug output info struct
//// Return Value    : void
//// Usage Example   : debug_output_init(info);
//// Note            : This function is generally not called directly by users
////-------------------------------------------------------------------------------------------------------------------
//void debug_output_init (debug_obj_struct *obj)
//{
//}

////-------------------------------------------------------------------------------------------------------------------
//// Function Name  : debug serial port init
//// Parameter       : void
//// Return Value    : void
//// Usage Example   : debug_init();
//// Note            : Default call in the open source demo, interrupt reception enabled by default
////-------------------------------------------------------------------------------------------------------------------
//void debug_init (void)
//{
//}
//// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
