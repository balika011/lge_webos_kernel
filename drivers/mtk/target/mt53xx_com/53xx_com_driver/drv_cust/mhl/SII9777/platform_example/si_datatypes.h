/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file si_datatypes.h
 *
 * @brief Silicon Image common types
 *
 *****************************************************************************/


#ifndef SI_DATATYPES_H
#define SI_DATATYPES_H

#ifdef __cplusplus
extern "C"{
#endif

/***** #include statements ***************************************************/

//#include <stdio.h>
#include <string.h>
//#include <stdarg.h>
#include "x_debug.h"
/***** public macro definitions **********************************************/
#include "typedef.h"
#ifndef ENABLE
/*****************************************************************************/
/** @defgroup SII_ENABLE_DISABLE Enable/disable definitions
* @brief Enable/disable definitions used in definitions
*
******************************************************************************/
/* @{ */
#define ENABLE      (1)
#define DISABLE     (0)
/* @} */
#endif // ENABLE

#ifndef BIT0
/*****************************************************************************/
/** @defgroup SII_BIT_DEFINITIONS Generic bit definitions
* @brief Generic bit definitions
*
******************************************************************************/
/* @{ */
#define BIT0        0x01
#define BIT1        0x02
#define BIT2        0x04
#define BIT3        0x08
#define BIT4        0x10
#define BIT5        0x20
#define BIT6        0x40
#define BIT7        0x80
#define BIT8        0x0100
#define BIT9        0x0200
#define BIT10       0x0400
#define BIT11       0x0800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000
/* @} */
#endif // BIT0

#ifndef NULL
#define NULL       ((void*)0)
#endif // NULL

#ifndef INST_NULL
#define INST_NULL  ((SiiInst_t)0)
#endif // INST_NULL

#ifndef PACKED
//#define PACKED // 8 bit microcontroller version
#define PACKED __attribute__  ((packed)) // GCC version
#endif // PACKED

#ifndef SII_INST2OBJ
#define SII_INST2OBJ(inst)   ((void*)inst)
#define SII_OBJ2INST(pObj)   ((SiiInst_t)pObj)
#endif // SII_INST2OBJ

#ifndef ON
#define ON  true
#define OFF false
#endif

#ifndef TRUE
#define TRUE  true
#define FALSE false
#endif

#define ROM const      // ROM memory
#define SII_XDATA      // 8051 type of external memory

#define ABS_DIFF(A, B) ((A>B) ? (A-B) : (B-A))

// Test chain macro
// Note: "switch" operator must be used with caution as PASS_IF includes "break"
#define BEGIN_TEST do{
#define PASS_IF(a) {if(!(a)) break;}
#define FAIL_IF(a) {if(a) break;}
#define END_TEST }while(false);

/* Bit manipulation macros */
#define SII_SET_BIT(p, bit)     ( *(p) |= (1<<(bit)) )
#define SII_CLR_BIT(p, bit)     ( *(p) &= (~(1<<(bit))) )
#define SII_PUT_BIT(p, bit, b)  ( *(p) = (b) ? (*(p)|(1<<(bit))) : (*(p)&(~(1<<(bit)))) )

#define SET_BITS    0xFF
#define CLEAR_BITS  0x00

/***** public type definitions ***********************************************/

typedef signed int        int_t;
typedef unsigned int      uint_t;
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
//typedef unsigned long int uint32_t;
typedef signed char       int8_t;
typedef signed short      int16_t;
//typedef signed long int   int32_t;
typedef int               prefuint_t; // Platform-specific efficient integers

/**
* @brief Instance type
*/
typedef void*             SiiInst_t;
#define SII_INST_NULL     ((SiiInst_t)0)

#ifdef __cplusplus
typedef bool bool_t;
#else
/**
* @brief C++ -like Boolean type
*/
/*
typedef enum
{
    false   = 0,
    true    = !(false)
} bool_t;
*/
typedef UINT8 bool_t;

#endif // __cplusplus

/**
* @brief Type to use with bitfields
*/
//typedef unsigned char bit_fld_t;
// gcc -pedantic does not allow bit field operation, so to avoid the warning, we need the following trick
typedef unsigned char   bit_fld_t_t;    // bit field type used in structures
typedef unsigned short  bit_fld16_t_t;
#define bit_fld_t       bit_fld_t_t
#define bit_fld16_t     bit_fld16_t_t

/* Construct to stringify defines */
//#define SII_STRINGIFY(x)                 #x
#define SII_STRINGIFY(x)                 #x

#define SII_DEF2STR(x)                   SII_STRINGIFY(x)

#define SII_MODULE_NAME_SET(name)        static const char* sSiiModuleNameStr = SII_DEF2STR(name)
//#define SII_MODULE_NAME_SET(name)          
#define SII_MODULE_NAME_GET()            sSiiModuleNameStr

/***** standard functions ****************************************************/

#define SII_MEMCPY(pdes, psrc, size)     memcpy(pdes, psrc, size)
#define SII_MEMSET(pdes, value, size)    memset(pdes, value, size)

// _vsnprintf_s is Visual Studio version of vsprintf with known buffer size (SII use 160 char)
// Visual Studio definition:
// int _vsnprintf_s( char *buffer, size_t sizeOfBuffer, size_t count, const char *format, va_list argptr );
//      buffer: Storage location for output.
//      sizeOfBuffer: The size of the buffer for output, as the character count.
//      count: Maximum number of characters to write (not including the terminating null), or _TRUNCATE.
//      format: Format specification.
//      argptr: Pointer to list of arguments.
//#define SII_VSPRINTF(dstr, fstr, arg)    _vsnprintf_s(dstr, 160, 159, fstr, arg)
#define SII_VSPRINTF(dstr, fstr, arg)  1
#define SII_STRLEN(str)                  strlen(str)
#define SII_SPRINTF(str, frm, arg) 1
//#define SII_SPRINTF(str, frm, arg)       sprintf_s(str, 40, frm, arg)
//#define SII_ASSERT(expr) LOG(0,"sii9777: expr,%s,%d,%s\n"__FILE__,__LINE__,__FUNCTION__)
//#define SII_ASSERT(expr) printf("sii9777: %s,%d,%s\n",__FILE__,__LINE__,__FUNCTION__)
#define SII_ASSERT(expr)

#ifdef __cplusplus
}
#endif

#endif // SI_DATATYPES_H

