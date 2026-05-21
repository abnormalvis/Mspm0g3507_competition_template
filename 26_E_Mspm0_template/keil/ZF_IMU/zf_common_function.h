/*********************************************************************************************************************
* MSPM0G3507 Opensource Library is a third-party open source library based on the official SDK interface
* Copyright (c) 2022 SEEKFREE (ZhuFei Technology)
*
* This file is part of the MSPM0G3507 open source library
*
* MSPM0G3507 open source library is free software
* You can redistribute and/or modify it under the terms of the GPL
* (GNU General Public License) as published by the Free Software Foundation,
* either version 3 of the GPL (GPL 3.0) or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GPL for more details.
*
* You should have received a copy of the GPL along with this library.
*
* Commercial use of this library requires written permission beyond GPL 3.0.
* See libraries/doc/GPL3_permission_statement.txt or the LICENSE file.
* For inquiries contact the author via official channels.
*
* Module name:    zf_common_function
* Description:    common function library for MSPM0G3507
* Version:         see libraries/doc version description
* Toolchain:      CCS / Keil MDK
* Platform:       MSPM0G3507
* Shop:           https://seekfree.taobao.com/
*
* Changelog:
* Date              Author              Notes
* 2023-03-15       pudding             first version
********************************************************************************************************************/

#ifndef _zf_common_function_h_
#define _zf_common_function_h_

#include "zf_common_typedef.h"

extern int32 value_default_addr;

//================================================== Macro Function Section ==================================================
//-------------------------------------------------------------------------------------------------------------------
// Function brief     Absolute value function. Data range is [-32767,32767]
// Parameter          dat             number to get absolute value
// Return             int             absolute value
// Usage example      dat = func_abs(dat);                            // Convert dat to positive
// Note
//-------------------------------------------------------------------------------------------------------------------
#define     func_abs(x)             ((x) >= 0 ? (x): -(x))

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Limiter. Data range is [-32768,32767]
// Parameter          x               value to be limited
// Parameter          y               limit range (value will be limited to [-y, +y])
// Return             int             limited value
// Usage example      int dat = func_limit(500, 300);                 // Value is limited to [-300, +300], so the result is 300
// Note
//-------------------------------------------------------------------------------------------------------------------
#define     func_limit(x, y)        ((x) > (y) ? (y) : ((x) < -(y) ? -(y) : (x)))

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Two-sided limiter. Data range is [-32768,32767]
// Parameter          x               value to be limited
// Parameter          a               limit range left boundary
// Parameter          b               limit range right boundary
// Return             int             limited value
// Usage example      int dat = func_limit_ab(500, -300, 400);        // Value is limited to [-300, +400], so the result is 400
// Note
//-------------------------------------------------------------------------------------------------------------------
#define     func_limit_ab(x, a, b)  ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

//================================================== Macro Function Section ==================================================

//================================================== Standard Function Section ==================================================

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Sine wave conversion function, get specified sample point
// Parameter          *data_buffer    data buffer
// Parameter          sample_max      number of sample points
// Parameter          amplitude_max   maximum amplitude
// Parameter          offset_degree   offset phase angle
// Return             void
// Usage example      func_get_sin_amplitude_table(sound_amplitude, 1024, 512, 270);
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_get_sin_amplitude_table        (uint32 *data_buffer, uint32 sample_max, uint32 amplitude_max, uint32 offset_degree);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Get greatest common divisor of two integers (Euclidean algorithm / subtraction method from Nine Chapters of Arithmetic)
// Parameter          num1            number 1
// Parameter          num2            number 2
// Return             uint32          greatest common divisor
// Usage example      return func_get_greatest_common_divisor(144, 36);               // Get GCD of 144 and 36
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32      func_get_greatest_common_divisor    (uint32 num1, uint32 num2);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Software delay
// Parameter          tick            delay count
// Return             void
// Usage example      func_soft_delay(100);
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_soft_delay                     (volatile long tick);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Data element location
// Parameter          data_size       data element type (see common_data_size_enum definition in zf_common_typedef.h)
// Parameter          *buffer         data buffer
// Parameter          buffer_length   data buffer length
// Parameter          data            data value to search for
// Parameter          id              data ID to search for, e.g. to find the first occurrence pass 1, range is [1 - n]
// Return             uint32          index position
// Usage example      uint32 index = func_get_data_index(COMMON_DATA_SIZE_8BIT, (void *)"-100", 4, '-', 1);
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32      func_get_data_index                 (common_data_size_enum data_size, void *buffer, uint32 buffer_length, uint32 data, uint32 id);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     String to integer. Data range is [-32768,32767]
// Parameter          *str            input string, can include sign
// Return             int32           converted value
// Usage example      int32 dat = func_str_to_int("-100");
// Note
//-------------------------------------------------------------------------------------------------------------------
int32       func_str_to_int                     (char *str);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Integer to string. Data range is [-32768,32767]
// Parameter          *str            string pointer
// Parameter          number          input value
// Return             void
// Usage example      func_int_to_str(data_buffer, -300);
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_int_to_str                     (char *str, int32 number);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     String to unsigned integer. Data range is [0,65535]
// Parameter          *str            input string, no sign
// Return             uint32          converted value
// Usage example      uint32 dat = func_str_to_uint("100");
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32      func_str_to_uint                    (char *str);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Unsigned integer to string. Data range is [0,65535]
// Parameter          *str            string pointer
// Parameter          number          input value
// Return             void
// Usage example      func_uint_to_str(data_buffer, 300);
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_uint_to_str                    (char *str, uint32 number);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     String to float. Effective cumulative precision is 6 decimal places
// Parameter          *str            input string, can include sign
// Return             float           converted value
// Usage example      float dat = func_str_to_float("-100.2");
// Note
//-------------------------------------------------------------------------------------------------------------------
float       func_str_to_float                   (char *str);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Float to string
// Parameter          *str            string pointer
// Parameter          number          input value
// Parameter          point_bit       decimal point precision
// Return             void
// Usage example      func_float_to_str(data_buffer, 3.1415, 2);                      // Result output: data_buffer = "3.14"
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_float_to_str                   (char *str, float number, uint8 point_bit);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     String to double. Effective cumulative precision is 9 decimal places
// Parameter          str             input string, can include sign
// Return             double          converted value
// Usage example      double dat = func_str_to_double("-100.2");
// Note
//-------------------------------------------------------------------------------------------------------------------
double      func_str_to_double                  (char *str);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Double to string
// Parameter          *str            string pointer
// Parameter          number          input value
// Parameter          point_bit       decimal point precision
// Return             void
// Usage example      func_double_to_str(data_buffer, 3.1415, 2);                     // Result output: data_buffer = "3.14"
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_double_to_str                  (char *str, double number, uint8 point_bit);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     String to Hex
// Parameter          str             input string, no sign
// Return             uint32          converted value
// Usage example      uint32 dat = func_str_to_hex("0x11");
// Note               Must start with 0x
//-------------------------------------------------------------------------------------------------------------------
uint32      func_str_to_hex                     (char *str);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Hex to string
// Parameter          *str            string pointer
// Parameter          number          input value
// Return             void
// Usage example      func_hex_to_str(data_buffer, 0x11);                             // Result output: data_buffer = "0x11"
// Note
//-------------------------------------------------------------------------------------------------------------------
void        func_hex_to_str                     (char *str, uint32 number);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          void
// Return             void
// Usage example      void_function_void something_callback = void_function_void_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        void_function_void_default          (void);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          parameter       parameter
// Return             void
// Usage example      void_function_uint32 something_callback = void_function_uint32_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        void_function_uint32_default        (uint32 parameter);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          *ptr            user parameter pointer
// Return             void
// Usage example      void_function_ptr something_callback = void_function_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        void_function_ptr_default           (void *ptr);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          void
// Return             uint32          returns a uint32
// Usage example      uint32_function_void something_callback = uint32_function_void_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32      uint32_function_void_default        (void);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          parameter       parameter
// Return             uint32          returns a uint32
// Usage example      uint32_function_uint32 something_callback = uint32_function_uint32_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32      uint32_function_uint32_default      (uint32 parameter);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          *ptr            user parameter pointer
// Return             uint32          returns a uint32
// Usage example      uint32_function_ptr something_callback = uint32_function_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32      uint32_function_ptr_default         (void *ptr);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          void
// Return             void *          returns a pointer
// Usage example      ptr_function_void something_callback = ptr_function_void_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        *ptr_function_void_default          (void);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          parameter       parameter
// Return             void *          returns a pointer
// Usage example      ptr_function_uint32 something_callback = ptr_function_uint32_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        *ptr_function_uint32_default        (uint32 parameter);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          *ptr            user parameter pointer
// Return             void *          returns a pointer
// Usage example      ptr_function_ptr something_callback = ptr_function_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        *ptr_function_ptr_default           (void *ptr);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     Default handler placeholder
// Parameter          state           interrupt state passed in
// Parameter          *ptr            user parameter pointer
// Return             void
// Usage example      void_callback_uint32_ptr something_callback = void_callback_uint32_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void        void_callback_uint32_ptr_default    (uint32 state, void *ptr);

//-------------------------------------------------------------------------------------------------------------------
// Function brief     sprintf function implementation
// Parameter          *buff           buffer
// Parameter          *format         source string
// Parameter          ...             variable parameter list
// Return             uint32          processed data length
// Usage example      zf_sprintf(buff, "Data : %d", 100);
// Note               This function is called internally. Users do not need to care about it and should not modify it.
//-------------------------------------------------------------------------------------------------------------------
uint32      zf_sprintf                          (int8 *buff, const int8 *format, ...);
//================================================== Standard Function Section ==================================================

#endif
