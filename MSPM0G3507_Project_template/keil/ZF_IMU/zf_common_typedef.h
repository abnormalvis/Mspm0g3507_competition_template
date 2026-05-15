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
* Module name:    zf_common_typedef
* Description:    common typedef library for MSPM0G3507
* Version:         see libraries/doc version description
* Toolchain:      CCS / Keil MDK
* Platform:       MSPM0G3507
* Shop:           https://seekfree.taobao.com/
*
* Changelog:
* Date              Author              Notes
* 2023-03-15       pudding             first version
********************************************************************************************************************/

#ifndef _zf_common_typedef_h_
#define _zf_common_typedef_h_

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdarg.h"
#include "string.h"
#include "stdlib.h"

                                                                                // Standard comment indentation, based on 20 Tab keys for blank lines

//=================================================== typeDefine ===================================================
#define USE_ZF_TYPEDEF      (1)                                                 // Whether to enable typeDefine declaration

#if USE_ZF_TYPEDEF
// Data type declaration
// Prefer stdint.h defined type names to avoid conflicts, types can be trimmed here
typedef unsigned char       uint8;                                              // Unsigned  8 bits
typedef unsigned short int  uint16;                                             // Unsigned 16 bits
typedef unsigned int        uint32;                                             // Unsigned 32 bits
typedef unsigned long long  uint64;                                             // Unsigned 64 bits

typedef signed char         int8;                                               // Signed  8 bits
typedef signed short int    int16;                                              // Signed 16 bits
typedef signed int          int32;                                              // Signed 32 bits
typedef signed long long    int64;                                              // Signed 64 bits

typedef volatile uint8      vuint8;                                             // volatile Unsigned  8 bits
typedef volatile uint16     vuint16;                                            // volatile Unsigned 16 bits
typedef volatile uint32     vuint32;                                            // volatile Unsigned 32 bits
typedef volatile uint64     vuint64;                                            // volatile Unsigned 64 bits

typedef volatile int8       vint8;                                              // volatile Signed  8 bits
typedef volatile int16      vint16;                                             // volatile Signed 16 bits
typedef volatile int32      vint32;                                             // volatile Signed 32 bits
typedef volatile int64      vint64;                                             // volatile Signed 64 bits

typedef enum
{
    COMMON_DATA_SIZE_8BIT   = 1,                                                // data bit width 8bit
    COMMON_DATA_SIZE_16BIT  = 2,                                                // data bit width 16bit
    COMMON_DATA_SIZE_32BIT  = 4,                                                // data bit width 32bit
}common_data_size_enum;

#define ZF_NO_ERROR     ( 0 )                                                   // Common status return check value, typically returning 0 means no error
#define ZF_ERROR        ( 1 )                                                   // Common status return check value, typically returning non-zero means error code

#define ZF_ENABLE       ( 1 )                                                   // Enable definition
#define ZF_DISABLE      ( 0 )                                                   // Disable definition

#define ZF_TRUE         ( 1 )                                                   // Boolean true definition
#define ZF_FALSE        ( 0 )                                                   // Boolean false definition

typedef void    (*void_function_void            )   (void);
typedef void    (*void_function_uint32          )   (uint32 parameter);
typedef void    (*void_function_ptr             )   (void *ptr);

typedef uint32  (*uint32_function_void          )   (void);
typedef uint32  (*uint32_function_uint32        )   (uint32 parameter);
typedef uint32  (*uint32_function_ptr           )   (void *ptr);

typedef void*   (*ptr_function_void             )   (void);
typedef void*   (*ptr_function_uint32           )   (uint32 parameter);
typedef void*   (*ptr_function_ptr              )   (void *ptr);

typedef void    (*void_callback_uint32_ptr      )   (uint32 state, void *ptr);

#endif
//=================================================== typeDefine ===================================================

//=================================================== IDE adaptation Define ===================================================
#define IDE_MDK         ( 0x01 )
#define IDE_IAR         ( 0x02 )
#define IDE_ADS         ( 0x04 )
#define IDE_MRS         ( 0x08 )

#if defined(__ICCARM__)
#define IDE_TYPE        ( IDE_IAR )
#else
#define IDE_TYPE        ( IDE_MDK )
#endif

#if (IDE_MDK & IDE_TYPE)
// -------------------------- MDK IDE adaptation --------------------------
// MDK uses GNUC toolchain
// Inline function modifier
#define ZF_INLINE           static inline
#define ZF_WEAK             __attribute__((weak))

// Memory alignment and compact struct
#define ZF_PACKED           __attribute__((packed))
#define ZF_PACKED_ENABLE
#define ZF_PACKED_DISABLE

// Memory barrier for synchronizing data and instruction, prevents issues from compiler optimization
#define ZF_DSB()            __DSB()
#define ZF_ISB()            __ISB()
#define ZF_DMB()            __DMB()

// File identification
#define ZF_FILE_MESSAGE     ( __FILE__ )
#define ZF_LINE_MESSAGE     ( __LINE__ )
// -------------------------- MDK IDE adaptation --------------------------
#elif (IDE_IAR & IDE_TYPE)
// -------------------------- IAR IDE adaptation --------------------------
// IAR uses ICCARM toolchain
// Inline function modifier
#define ZF_INLINE           static inline
#define ZF_WEAK             __attribute__((weak))

// Memory alignment and compact struct
// IAR supports __attribute__((packed)) so _Pragma is not needed
#define ZF_PACKED           __attribute__((packed))
#define ZF_PACKED_ENABLE    // _Pragma("pack(push,1)")
#define ZF_PACKED_DISABLE   // _Pragma("pack(pop)")

// Memory barrier for synchronizing data and instruction, prevents issues from compiler optimization
#define ZF_DSB()            __DSB()
#define ZF_ISB()            __ISB()
#define ZF_DMB()            __DMB()

// File identification
#define ZF_FILE_MESSAGE     ( __FILE__ )
#define ZF_LINE_MESSAGE     ( __LINE__ )
// -------------------------- IAR IDE adaptation --------------------------
#elif (IDE_ADS & IDE_TYPE)
// -------------------------- ADS IDE adaptation --------------------------
// ADS uses ADS* toolchain
// Inline function modifier
#define ZF_INLINE           static inline
#define ZF_WEAK             __attribute__((weak))

// ADS only supports 2/4 byte alignment
#define ZF_PACKED
#define ZF_PACKED_ENABLE    _Pragma("pack 2")
#define ZF_PACKED_DISABLE   _Pragma("pack 0")

// Memory barrier for synchronizing data and instruction, prevents issues from compiler optimization
#define ZF_DSB()
#define ZF_ISB()
#define ZF_DMB()

// File identification
#define ZF_FILE_MESSAGE     ( __FILE__ )
#define ZF_LINE_MESSAGE     ( __LINE__ )
// -------------------------- ADS IDE adaptation --------------------------
#elif (IDE_MRS & IDE_TYPE)
// -------------------------- MRS IDE adaptation --------------------------
// MRS uses GNUC toolchain
// Inline function modifier
#define ZF_INLINE           static inline
#define ZF_WEAK             __attribute__((weak))

// Memory alignment and compact struct
#define ZF_PACKED           __attribute__((packed))
#define ZF_PACKED_ENABLE
#define ZF_PACKED_DISABLE

// Memory barrier for synchronizing data and instruction, prevents issues from compiler optimization
#define ZF_DSB()
#define ZF_ISB()
#define ZF_DMB()

// File identification
#define ZF_FILE_MESSAGE     ( __FILE__ )
#define ZF_LINE_MESSAGE     ( __LINE__ )
// -------------------------- MRS IDE adaptation --------------------------
#else
#error "IDE Type error!"
#endif
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#endif
