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
* Module name: zf_common_function
* Description: common function library for MSPM0G3507
* Version: see libraries/doc version description
* Toolchain: CCS / Keil MDK
* Platform: MSPM0G3507
* Shop: https://seekfree.taobao.com/
*
* Changelog:
* Date Author Notes
* 2023-03-15 pudding first version
********************************************************************************************************************/

#include "math.h"

#include "zf_common_debug.h"
#include "zf_common_function.h"

int32 value_default_addr = 0;

//-------------------------------------------------------------------------------------------------------------------
// Function brief Sine wave conversion function, get specified sample point
// Parameter *data_buffer data buffer
// Parameter sample_max number of sample points
// Parameter amplitude_max maximum amplitude
// Parameter offset_degree offset phase angle
// Return void
// Usage example func_get_sin_amplitude_table(sound_amplitude, 1024, 512, 270);
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_get_sin_amplitude_table (uint32 *data_buffer, uint32 sample_max, uint32 amplitude_max, uint32 offset_degree)
{
 uint32 i = 0, j = 0;
 double rad = 0.0;
 double amplitude = 0.0;

 offset_degree = offset_degree % 360;
 offset_degree = (uint32)((float)offset_degree / 360.0 * sample_max);

 j = amplitude_max / 2; // bias amplitude, defaults to half of max amplitude
 rad = 3.1415926535898 * 2 / sample_max; // calculate radian difference per sample point
 for(i = offset_degree; i < sample_max + offset_degree; i ++)
 {
 amplitude = j * sin(rad * i) + j;
 *data_buffer = (uint32)amplitude;
 data_buffer ++;
 }
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Get greatest common divisor of two integers (subtraction method from Nine Chapters of Arithmetic)
// Parameter num1 number 1
// Parameter num2 number 2
// Return uint32 greatest common divisor
// Usage example return func_get_greatest_common_divisor(144, 36); // Get GCD of 144 and 36
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32 func_get_greatest_common_divisor (uint32 num1, uint32 num2)
{
 while(num1 != num2)
 {
 if(num1 > num2)
 {
 num1 = num1 - num2;
 }
 if(num1 < num2)
 {
 num2 = num2 - num1;
 }
 }
 return num1;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Software delay
// Parameter tick delay count
// Return void
// Usage example func_soft_delay(100);
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_soft_delay (volatile long tick)
{
 while(tick --);
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Data element location
// Parameter data_size data element type (see common_data_size_enum definition in zf_common_typedef.h)
// Parameter *buffer data buffer
// Parameter buffer_length data buffer length
// Parameter data data value to search for
// Parameter id data ID to search for, e.g. to find the first occurrence pass 1, range is [1 - n]
// Return uint32 index position
// Usage example uint32 index = func_get_data_index(COMMON_DATA_SIZE_8BIT, (void *)"-100", 4, '-', 1);
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32 func_get_data_index (common_data_size_enum data_size, void *buffer, uint32 buffer_length, uint32 data, uint32 id)
{
 zf_assert(
 COMMON_DATA_SIZE_8BIT != data_size
 || COMMON_DATA_SIZE_16BIT != data_size
 || COMMON_DATA_SIZE_32BIT != data_size);
 zf_assert(NULL != buffer);
 zf_assert(0 < id);

 uint32 index = 0;
 uint32 data_temp = 0;
 uint32 data_mask = 0;

 switch(data_size)
 {
 default:
 case COMMON_DATA_SIZE_8BIT : data_mask = 0x000000FF; break;
 case COMMON_DATA_SIZE_16BIT: data_mask = 0x0000FFFF; break;
 case COMMON_DATA_SIZE_32BIT: data_mask = 0xFFFFFFFF; break;
 }

 do
 {
 switch(data_size)
 {
 default:
 case COMMON_DATA_SIZE_8BIT : data_temp = *((uint8 *)buffer + index); break;
 case COMMON_DATA_SIZE_16BIT: data_temp = *((uint16 *)buffer + index); break;
 case COMMON_DATA_SIZE_32BIT: data_temp = *((uint32 *)buffer + index); break;
 }
 if((data & data_mask) == data_temp)
 {
 if(!-- id)
 {
 break;
 }
 }
 index ++;
 }
 while(index < buffer_length);
 return index;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief String to integer. Data range is [-32768,32767]
// Parameter *str input string, can include sign
// Return int32 converted value
// Usage example int32 dat = func_str_to_int("-100");
// Note
//-------------------------------------------------------------------------------------------------------------------
int32 func_str_to_int (char *str)
{
 zf_assert(NULL != str);
 uint8 sign = 0; // sign flag, 0-positive 1-negative
 int32 temp = 0; // temporary calculation variable
 do
 {
 if(NULL == str)
 {
 break;
 }

 if('-' == *str) // if first character is minus sign
 {
 sign = 1; // mark as negative
 str ++;
 }
 else if('+' == *str) // if first character is plus sign
 {
 str ++;
 }

 while(('0' <= *str) && ('9' >= *str)) // confirm this is a digit
 {
 temp = temp * 10 + ((uint8)(*str) - 0x30); // calculate value
 str ++;
 }

 if(sign)
 {
 temp = -temp;
 }
 }while(0);
 return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Integer to string. Data range is [-32768,32767]
// Parameter *str string pointer
// Parameter number input value
// Return void
// Usage example func_int_to_str(data_buffer, -300);
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_int_to_str (char *str, int32 number)
{
 zf_assert(NULL != str);
 uint8 data_temp[16]; // buffer
 uint8 bit = 0; // digit count
 int32 number_temp = 0;

 do
 {
 if(NULL == str)
 {
 break;
 }

 if(0 > number) // negative number
 {
 *str ++ = '-';
 number = -number;
 }
 else if(0 == number) // or it's zero
 {
 *str = '0';
 break;
 }

 while(0 != number) // loop until value reaches zero
 {
 number_temp = number % 10;
 data_temp[bit ++] = func_abs(number_temp); // extract digits in reverse order
 number /= 10; // remove the extracted units digit
 }
 while(0 != bit) // process extracted digits in decreasing order
 {
 *str ++ = (data_temp[bit - 1] + 0x30); // retrieve digits from reverse-order array to produce forward-order string
 bit --;
 }
 }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief String to unsigned integer. Data range is [0,65535]
// Parameter *str input string, no sign
// Return uint32 converted value
// Usage example uint32 dat = func_str_to_uint("100");
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32 func_str_to_uint (char *str)
{
 zf_assert(NULL != str);
 uint32 temp = 0; // temporary calculation variable

 do
 {
 if(NULL == str)
 {
 break;
 }

 while(('0' <= *str) && ('9' >= *str)) // confirm this is a digit
 {
 temp = temp * 10 + ((uint8)(*str) - 0x30); // calculate value
 str ++;
 }
 }while(0);

 return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Unsigned integer to string. Data range is [0,65535]
// Parameter *str string pointer
// Parameter number input value
// Return void
// Usage example func_uint_to_str(data_buffer, 300);
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_uint_to_str (char *str, uint32 number)
{
 zf_assert(NULL != str);
 int8 data_temp[16]; // buffer
 uint8 bit = 0; // digit count

 do
 {
 if(NULL == str)
 {
 break;
 }

 if(0 == number) // it's zero
 {
 *str = '0';
 break;
 }

 while(0 != number) // loop until value reaches zero
 {
 data_temp[bit ++] = (number % 10); // extract digits in reverse order
 number /= 10; // remove the extracted units digit
 }
 while(0 != bit) // process extracted digits in decreasing order
 {
 *str ++ = (data_temp[bit - 1] + 0x30); // retrieve digits from reverse-order array to produce forward-order string
 bit --;
 }
 }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief String to float. Effective cumulative precision is 6 decimal places
// Parameter *str input string, can include sign
// Return float converted value
// Usage example float dat = func_str_to_float("-100.2");
// Note
//-------------------------------------------------------------------------------------------------------------------
float func_str_to_float (char *str)
{
 zf_assert(NULL != str);
 uint8 sign = 0; // sign flag, 0-positive 1-negative
 float temp = 0.0; // temporary calculation variable, integer part
 float temp_point = 0.0; // temporary calculation variable, fractional part
 float point_bit = 1; // fractional cumulative divisor

 do
 {
 if(NULL == str)
 {
 break;
 }

 if('-' == *str) // negative number
 {
 sign = 1; // mark as negative
 str ++;
 }
 else if('+' == *str) // if first character is plus sign
 {
 str ++;
 }

 // Extract integer part
 while(('0' <= *str) && ('9' >= *str)) // confirm this is a digit
 {
 temp = temp * 10 + ((uint8)(*str) - 0x30); // extract numeric value
 str ++;
 }
 if('.' == *str)
 {
 str ++;
 while(('0' <= *str) && ('9' >= *str) && 1000000.0 > point_bit) // confirm digit and precision has not reached 6 places
 {
 temp_point = temp_point * 10 + ((uint8)(*str) - 0x30); // extract fractional part value
 point_bit *= 10; // calculate divisor for this fractional part
 str ++;
 }
 temp_point /= point_bit; // calculate fraction
 }
 temp += temp_point; // combine the values

 if(sign)
 {
 temp = -temp;
 }
 }while(0);
 return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Float to string
// Parameter *str string pointer
// Parameter number input value
// Parameter point_bit decimal point precision
// Return void
// Usage example func_float_to_str(data_buffer, 3.1415, 2); // Result output: data_buffer = "3.14"
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_float_to_str (char *str, float number, uint8 point_bit)
{
 zf_assert(NULL != str);
 int data_int = 0; // integer part
 int data_float = 0.0; // fractional part
 int data_temp[8]; // integer char buffer
 int data_temp_point[6]; // fractional char buffer
 uint8 bit = point_bit; // conversion precision digits

 do
 {
 if(NULL == str)
 {
 break;
 }

 // Extract integer part
 data_int = (int)number; // directly cast to int
 if(0 > number) // check if source data is positive or negative
 {
 *str ++ = '-';
 }
 else if(0.0 == number) // if it's zero
 {
 *str ++ = '0';
 *str ++ = '.';
 *str = '0';
 break;
 }

 // Extract fractional part
 number = number - data_int; // subtract integer part
 while(bit --)
 {
 number = number * 10; // shift required decimal places into integer part
 }
 data_float = (int)number; // get this portion of the value

 // Convert integer part to string
 bit = 0;
 do
 {
 data_temp[bit ++] = data_int % 10; // write integer part digits to char buffer in reverse order
 data_int /= 10;
 }while(0 != data_int);
 while(0 != bit)
 {
 *str ++ = (func_abs(data_temp[bit - 1]) + 0x30); // reverse the reversed digits back into string, getting forward-order value
 bit --;
 }

 // Convert fractional part to string
 if(0 != point_bit)
 {
 bit = 0;
 *str ++ = '.';
 if(0 == data_float)
 {
 *str = '0';
 }
 else
 {
 while(0 != point_bit) // check effective digit count
 {
 data_temp_point[bit ++] = data_float % 10; // write to char buffer in reverse order
 data_float /= 10;
 point_bit --;
 }
 while(0 != bit)
 {
 *str ++ = (func_abs(data_temp_point[bit - 1]) + 0x30); // reverse the reversed digits back into string, getting forward-order value
 bit --;
 }
 }
 }
 }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief String to double. Effective cumulative precision is 9 decimal places
// Parameter str input string, can include sign
// Return double converted value
// Usage example double dat = func_str_to_double("-100.2");
// Note
//-------------------------------------------------------------------------------------------------------------------
double func_str_to_double (char *str)
{
 zf_assert(NULL != str);
 uint8 sign = 0; // sign flag, 0-positive 1-negative
 double temp = 0.0; // temporary calculation variable, integer part
 double temp_point = 0.0; // temporary calculation variable, fractional part
 double point_bit = 1; // fractional cumulative divisor

 do
 {
 if(NULL == str)
 {
 break;
 }

 if('-' == *str) // negative number
 {
 sign = 1; // mark as negative
 str ++;
 }
 else if('+' == *str) // if first character is plus sign
 {
 str ++;
 }

 // Extract integer part
 while(('0' <= *str) && ('9' >= *str)) // confirm this is a digit
 {
 temp = temp * 10 + ((uint8)(*str) - 0x30); // extract numeric value
 str ++;
 }
 if('.' == *str)
 {
 str ++;
 while(('0' <= *str) && ('9' >= *str) && 1000000000.0 > point_bit) // confirm digit and precision has not reached 9 places
 {
 temp_point = temp_point * 10 + ((uint8)(*str) - 0x30); // extract fractional part value
 point_bit *= 10; // calculate divisor for this fractional part
 str ++;
 }
 temp_point /= point_bit; // calculate fraction
 }
 temp += temp_point; // combine the values

 if(sign)
 {
 temp = -temp;
 }
 }while(0);
 return temp;

}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Double to string
// Parameter *str string pointer
// Parameter number input value
// Parameter point_bit decimal point precision
// Return void
// Usage example func_double_to_str(data_buffer, 3.1415, 2); // Result output: data_buffer = "3.14"
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_double_to_str (char *str, double number, uint8 point_bit)
{
 zf_assert(NULL != str);
 int data_int = 0; // integer part
 int data_float = 0.0; // fractional part
 int data_temp[12]; // integer char buffer
 int data_temp_point[9]; // fractional char buffer
 uint8 bit = point_bit; // conversion precision digits

 do
 {
 if(NULL == str)
 {
 break;
 }

 // Extract integer part
 data_int = (int)number; // directly cast to int
 if(0 > number) // check if source data is positive or negative
 {
 *str ++ = '-';
 }
 else if(0.0 == number) // if it's zero
 {
 *str ++ = '0';
 *str ++ = '.';
 *str = '0';
 break;
 }

 // Extract fractional part
 number = number - data_int; // subtract integer part
 while(bit --)
 {
 number = number * 10; // shift required decimal places into integer part
 }
 data_float = (int)number; // get this portion of the value

 // Convert integer part to string
 bit = 0;
 do
 {
 data_temp[bit ++] = data_int % 10; // write integer part digits to char buffer in reverse order
 data_int /= 10;
 }while(0 != data_int);
 while(0 != bit)
 {
 *str ++ = (func_abs(data_temp[bit - 1]) + 0x30); // reverse the reversed digits back into string, getting forward-order value
 bit --;
 }

 // Convert fractional part to string
 if(0 != point_bit)
 {
 bit = 0;
 *str ++ = '.';
 if(0 == data_float)
 {
 *str = '0';
 }
 else
 {
 while(0 != point_bit) // check effective digit count
 {
 data_temp_point[bit ++] = data_float % 10; // write to char buffer in reverse order
 data_float /= 10;
 point_bit --;
 }
 while(0 != bit)
 {
 *str ++ = (func_abs(data_temp_point[bit - 1]) + 0x30); // reverse the reversed digits back into string, getting forward-order value
 bit --;
 }
 }
 }
 }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief String to Hex
// Parameter str input string, no sign
// Return uint32 converted value
// Usage example uint32 dat = func_str_to_hex("0x11");
// Note Must start with 0x
//-------------------------------------------------------------------------------------------------------------------
uint32 func_str_to_hex (char *str)
{
 zf_assert(NULL != str);
 uint32 str_len = strlen(str); // string length
 uint32 result_data = 0; // result buffer
 uint8 temp = 0; // calculation variable
 uint8 flag = 0; // flag bit

 do
 {
 if(NULL == str)
 {
 break;
 }

 if(flag)
 {
 if(('a' <= *str) && ('f' >= *str))
 {
 temp = (*str - 87);
 }
 else if(('A' <= *str) && ('F' >= *str))
 {
 temp = (*str - 55);
 }
 else if(('0' <= *str) && ('9' >= *str))
 {
 temp = (*str - 48);
 }
 else
 {
 break;
 }
 result_data = ((result_data << 4) | (temp & 0x0F));
 }
 else
 {
// if(strncmp("0x", str, 2))
 if(('0' == *str) && ('x' == *(str + 1)))
 {
 str ++;
 flag = 1;
 }
 }
 str ++;
 }while(str_len --);

 return result_data;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Hex to string
// Parameter *str string pointer
// Parameter number input value
// Return void
// Usage example func_hex_to_str(data_buffer, 0x11); // Result output: data_buffer = "0x11"
// Note
//-------------------------------------------------------------------------------------------------------------------
void func_hex_to_str (char *str, uint32 number)
{
 zf_assert(NULL != str);
 const char hex_index[16] = {
 '0', '1', '2', '3',
 '4', '5', '6', '7',
 '8', '9', 'A', 'B',
 'C', 'D', 'E', 'F'};
 int8 data_temp[12]; // buffer
 uint8 bit = 0; // digit count

 *str ++ = '0';
 *str ++ = 'x';
 do
 {
 if(NULL == str)
 {
 break;
 }

 if(0 == number) // it's zero
 {
 *str = '0';
 break;
 }

 while(0 != number) // loop until value reaches zero
 {
 data_temp[bit ++] = (number & 0xF); // extract nibbles in reverse order
 number >>= 4; // remove the extracted nibble
 }
 while(0 != bit) // process extracted nibbles in decreasing order
 {
 *str ++ = hex_index[data_temp[bit - 1]]; // retrieve nibbles from reverse-order array to produce forward-order string
 bit --;
 }
 }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter void
// Return void
// Usage example void_function_void something_callback = void_function_void_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void void_function_void_default (void)
{
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter parameter parameter
// Return void
// Usage example void_function_uint32 something_callback = void_function_uint32_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void void_function_uint32_default (uint32 parameter)
{
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter *ptr user parameter pointer
// Return void
// Usage example void_function_ptr something_callback = void_function_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void void_function_ptr_default (void *ptr)
{
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter void
// Return uint32 returns a uint32
// Usage example uint32_function_void something_callback = uint32_function_void_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32 uint32_function_void_default (void)
{
 return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter parameter parameter
// Return uint32 returns a uint32
// Usage example uint32_function_uint32 something_callback = uint32_function_uint32_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32 uint32_function_uint32_default (uint32 parameter)
{
 return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter *ptr user parameter pointer
// Return uint32 returns a uint32
// Usage example uint32_function_ptr something_callback = uint32_function_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32 uint32_function_ptr_default (void *ptr)
{
 return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter void
// Return void * returns a pointer
// Usage example ptr_function_void something_callback = ptr_function_void_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void *ptr_function_void_default (void)
{
 return NULL;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter parameter parameter
// Return void * returns a pointer
// Usage example ptr_function_uint32 something_callback = ptr_function_uint32_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void *ptr_function_uint32_default (uint32 parameter)
{
 return NULL;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter *ptr user parameter pointer
// Return void * returns a pointer
// Usage example ptr_function_ptr something_callback = ptr_function_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void *ptr_function_ptr_default (void *ptr)
{
 return NULL;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Default handler placeholder
// Parameter state interrupt state passed in
// Parameter *ptr user parameter pointer
// Return void
// Usage example void_callback_uint32_ptr something_callback = void_callback_uint32_ptr_default;
// Note
//-------------------------------------------------------------------------------------------------------------------
void void_callback_uint32_ptr_default (uint32 state, void *ptr)
{
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief Convert number to ASCII value
// Parameter dat input data
// Parameter *p data buffer
// Parameter neg_type data type
// Parameter radix radix/base
// Return uint8 data
// Usage example number_conversion_ascii((uint32)ival, vstr, 1, 10);
// Note This function is called internally. Users do not need to care about it and should not modify it.
//-------------------------------------------------------------------------------------------------------------------
static uint8 number_conversion_ascii (uint32 dat, int8 *p, uint8 neg_type, uint8 radix)
{
 int32 neg_dat = 0;
 uint32 pos_dat = 0;
 uint8 temp_data = 0;
 uint8 valid_num = 0;

 if(neg_type)
 {
 neg_dat = (int32)dat;
 if(0 > neg_dat)
 {
 neg_dat = -neg_dat;
 }
 while(1)
 {
 *p = neg_dat % radix + '0';
 neg_dat = neg_dat / radix;
 valid_num ++;

 if(!neg_dat)
 {
 break;
 }
 p ++;
 }
 }
 else
 {
 pos_dat = dat;
 while(1)
 {
 temp_data = pos_dat % radix;
 if(10 <= temp_data)
 {
 temp_data += 'A' - 10;
 }
 else
 {
 temp_data += '0';
 }

 *p = temp_data;

 pos_dat = pos_dat / radix;
 valid_num ++;

 if(!pos_dat)
 {
 break;
 }
 p ++;
 }
 }
 return valid_num;
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief printf display conversion (reverse order)
// Parameter *d_buff buffer
// Parameter len length
// Return void
// Usage example printf_reverse_order(vstr, vlen);
// Note This function is called internally. Users do not need to care about it and should not modify it.
//-------------------------------------------------------------------------------------------------------------------
static void printf_reverse_order (int8 *d_buff, uint32 len)
{
 uint32 i = 0;
 int8 temp_data = 0;
 for(i = 0; len / 2 > i; i ++)
 {
 temp_data = d_buff[len - 1 - i];
 d_buff[len - 1 - i] = d_buff[i];
 d_buff[i] = temp_data;
 }
}

//-------------------------------------------------------------------------------------------------------------------
// Function brief sprintf function implementation
// Parameter *buff buffer
// Parameter *format source string
// Parameter... variable parameter list
// Return uint32 processed data length
// Usage example zf_sprintf(buff, "Data : %d", 100);
// Note This function is called internally. Users do not need to care about it and should not modify it.
//-------------------------------------------------------------------------------------------------------------------
uint32 zf_sprintf (int8 *buff, const int8 *format,...)
{
 uint32 buff_len = 0;
 va_list arg;
 va_start(arg, format);

 while (*format)
 {
 int8 ret = *format;
 if ('%' == ret)
 {
 switch (*++ format)
 {
 case 'a':// hexadecimal p-notation output float, not yet implemented
 {
 }break;

 case 'c':// a single character
 {
 int8 ch = (int8)va_arg(arg, uint32);
 *buff = ch;
 buff ++;
 buff_len ++;
 }break;

 case 'd':
 case 'i':// signed decimal integer
 {
 int8 vstr[33];
 int32 ival = (int32)va_arg(arg, int32);
 uint8 vlen = number_conversion_ascii((uint32)ival, vstr, 1, 10);

 if(0 > ival)
 {
 vstr[vlen] = '-';
 vlen ++;
 }
 printf_reverse_order(vstr, vlen);
 memcpy(buff, vstr, vlen);
 buff += vlen;
 buff_len += vlen;
 }break;

 case 'f':// floating point, output six decimal places, cannot specify output precision
 case 'F':// floating point, output six decimal places, cannot specify output precision
 {
 int8 vstr[33];
 double ival = (double)va_arg(arg, double);
 uint8 vlen = number_conversion_ascii((uint32)(int32)ival, vstr, 1, 10);

 if(0 > ival)
 {
 vstr[vlen] = '-';
 vlen ++;
 }
 printf_reverse_order(vstr, vlen);
 memcpy(buff, vstr, vlen);
 buff += vlen;
 buff_len += vlen;

 ival = ((double)ival - (int32)ival) * 1000000;
 if(ival)
 {
 vlen = number_conversion_ascii((uint32)(int32)ival, vstr, 1, 10);
 }
 else
 {
 vstr[0] = vstr[1] = vstr[2] = vstr[3] = vstr[4] = vstr[5] = '0';
 vlen = 6;
 }

 while(6 > vlen)
 {
 vstr[vlen] = '0';
 vlen ++;
 }

 vstr[vlen] = '.';
 vlen ++;

 printf_reverse_order(vstr, vlen);
 memcpy(buff, vstr, vlen);
 buff_len += vlen;
 }break;

 case 'u':// unsigned decimal integer
 {
 int8 vstr[33];
 uint32 ival = (uint32)va_arg(arg, uint32);
 uint8 vlen = number_conversion_ascii(ival, vstr, 0, 10);

 printf_reverse_order(vstr, vlen);
 memcpy(buff, vstr, vlen);
 buff += vlen;
 buff_len += vlen;
 }break;

 case 'o':// unsigned octal integer
 {
 int8 vstr[33];
 uint32 ival = (uint32)va_arg(arg, uint32);
 uint8 vlen = number_conversion_ascii(ival, vstr, 0, 8);

 printf_reverse_order(vstr, vlen);
 memcpy(buff, vstr, vlen);
 buff += vlen;
 buff_len += vlen;

 }break;

 case 'x':// unsigned hexadecimal integer
 case 'X':// unsigned hexadecimal integer
 {
 int8 vstr[33];
 uint32 ival = (uint32)va_arg(arg, uint32);
 uint8 vlen = number_conversion_ascii(ival, vstr, 0, 16);

 printf_reverse_order(vstr, vlen);
 memcpy(buff, vstr, vlen);
 buff += vlen;
 buff_len += vlen;
 }break;

 case 's':// string
 {
 int8 *pc = va_arg(arg, int8 *);
 while (*pc)
 {
 *buff = *pc;
 buff ++;
 buff_len ++;
 pc ++;
 }
 }break;

 case 'p':// output pointer in hexadecimal form
 {
 int8 vstr[33];
 uint32 ival = (uint32)va_arg(arg, uint32);
 uint8 vlen = number_conversion_ascii(ival, vstr, 0, 16);

 printf_reverse_order(vstr, 8);
 memcpy(buff, vstr, 8);
 buff += 8;
 buff_len += 8;
 }break;

 case '%':// output '%' character
 {
 *buff = '%';
 buff ++;
 buff_len ++;
 }break;

 default:
 {
 }break;
 }
 }
 else
 {
 *buff = (int8)(*format);
 buff ++;
 buff_len ++;
 }
 format ++;
 }
 va_end(arg);

 return buff_len;
}
