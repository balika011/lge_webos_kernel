//***************************************************************************

//!file     si_datatypes.h
//!brief    Silicon Image data type header (conforms to C99).
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DATATYPES_H__
#define __SI_DATATYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "x_typedef.h"



/* C99 defined data types.  */

#if 0
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed long    int32_t;

//typedef uint8_t UINT8;
#endif

#define ROM     //code        // 8051 type of ROM memory
#define XDATA   //xdata       // 8051 type of external memory

#ifdef __linux__
typedef BOOL bool_t;
#else
typedef enum
{
	false   = 0,
	true    = !false
} bool_t;
#endif

#define	SUCCESS false
#define	FAIL true

//------------------------------------------------------------------------------
// Configuration defines used by hal_config.h
//------------------------------------------------------------------------------
#if 0
#define ENABLE (0xFF)
#define DISABLE (0x00)

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80
#endif

#ifdef __cplusplus
}
#endif
#endif  // __SI_DATATYPES_H__
