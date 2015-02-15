//***************************************************************************
//!file     si_datatypes.h
//!brief    Silicon Image data type header (tries to conform to C99).
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DATATYPES_H__
#define __SI_DATATYPES_H__

//#include "limits.h"
//#include "x_debug.h"

//#include <linux/types.h>
#include "x_typedef.h"
#include "x_hal_io.h"




#define CBUS_WAKE_SEQUENCE  0
#define FPGA_BUILD 0
#define INC_RTPI 0
#define PART_LM3S5956 1
#define TARGET_IS_TEMPEST_RC1 1
#define VERBOSE_COMPILE 1



    // Use these two types most of the time, such as for index variables,
    // etc. when you want the native integer type and don't
    // care what size it is (within reason; most int types are 16
    // or 32 bit, not 8 bit -- even on the 8051)
    // On 8051 compilers, these types can be defined as 8-bit to
    // use the most efficient data size for that processor.  When you
    // absolutely need an integer of at least 16 bits, use the native type int
typedef signed int     int_t;
typedef unsigned int   uint_t;
typedef int            prefuint_t; // Platform-specific efficient integers
typedef unsigned long   clock_time_t;   // The clock type used for returning system ticks (1ms).

#if 0//ndef uint8_t
typedef signed int     int_t;

    /* C99 defined data types.  */

typedef unsigned char  uint8_t;		// Don't use this unless you truly need an unsigned 8 bit variable
typedef unsigned short uint16_t;	// Don't use this unless you truly need an unsigned 16 bit variable
//typedef unsigned long  uint32_t;    // Don't use this unless you truly need an unsigned 32 bit variable
typedef unsigned int  uint32_t;    // Don't use this unless you truly need an unsigned 32 bit variable

typedef signed char    int8_t;
typedef signed short   int16_t;
//typedef signed long    int32_t;
typedef signed int    int32_t;

typedef unsigned long   clock_time_t;   // The clock type used for returning system ticks (1ms).

#endif
//#define CLOCK_TIME_MAX  ULONG_MAX       // Maximum value of time type
#define CLOCK_TIME_MAX  0xffffffff       // Maximum value of time type


// gcc -pedantic does not allow bit field operation, so to avoid the warning, we need the following trick
typedef unsigned char   bit_fld_t_t;    // bit field type used in structures
#define bit_fld_t       __extension__ bit_fld_t_t

    /* Emulate C99/C++ bool type    */

#if 1//def __cplusplus
typedef unsigned char  bool_t;
#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#else
typedef enum
{
	false   = 0,
	true    = !(false)
} bool_t;
#endif // __cplusplus



#if 0
#if !defined ON
#define ON  true
#define OFF false
#endif

#if !defined TRUE
#define TRUE  true
#define FALSE false
#endif
#endif

#define SET_BITS    0xFF
#define CLEAR_BITS  0x00

#define ROM            // 8051 type of ROM memory
#define XDATA          // 8051 type of external memory

#define PACKED

#define ABS_DIFF(A, B) ((A>B) ? (A-B) : (B-A))

//------------------------------------------------------------------------------
// Configuration defines used by hal_config.h
//------------------------------------------------------------------------------

#ifndef ENABLE
#define ENABLE      (1)
#endif

#ifndef DISABLE
#define DISABLE     (0)
#endif

#if 0
#ifndef BIT0
#define BIT0 		 0x01
#endif

#ifndef BIT1
#define BIT1 		 0x02
#endif

#ifndef BIT2
#define BIT2 		 0x04
#endif

#ifndef BIT3
#define BIT3 		 0x08
#endif

#ifndef BIT4
#define BIT4 		 0x10
#endif

#ifndef BIT5
#define BIT5 		 0x20
#endif

#ifndef BIT6
#define BIT6 		 0x40
#endif

#ifndef BIT7
#define BIT7 		 0x80
#endif

#ifndef BIT8
#define BIT8        0x0100
#endif

#ifndef BIT9
#define BIT9        0x0200
#endif

#ifndef BIT10
#define BIT10       0x0400
#endif

#ifndef BIT11
#define BIT11       0x0800
#endif

#ifndef BIT12
#define BIT12       0x1000
#endif

#ifndef BIT13
#define BIT13       0x2000
#endif

#ifndef BIT14
#define BIT14       0x4000
#endif

#ifndef BIT15
#define BIT15       0x8000
#endif
#endif
#endif  // __SI_DATATYPES_H__
