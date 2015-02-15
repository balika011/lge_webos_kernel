//***************************************************************************
//!file     si_hal.h
//!brief    Silicon Image HAL function library header.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __HAL_H__
#define __HAL_H__
#include "si_datatypes.h"
#include "drv_sil_if.h"


#define API_DEBUG_CODE 1
//-------------------------------------------------------------------------------
//  HAL Macros
//-------------------------------------------------------------------------------
extern uint32_t g_pass;

#if defined(GPIO0_DET_MHLmode)
#define BOARD_CP1292//using CP1292 board
#elif defined(PWR5V_DET_MHLmode)
#define BOARD_JULIPORT//using RK1292 board
#else
#define BOARD_JULIPORT//using RK1292 board
#endif

#ifdef SIL_DEBUG
	//#define DEBUG_PRINT SIL_DEBUG
	#define DEBUG_PRINT(l,x)  do{ if(l){printf x;} }while(0);

#else
#if (API_DEBUG_CODE==1)
    extern  uint8_t     g_halMsgLevel;
    	#define DEBUG_PRINT(l,x)        if (l<=g_halMsgLevel) {printf("[%d]: ", (int)g_pass);  printf x;}
#else
        #define DEBUG_PRINT(l,x)
#endif
#endif

//-------------------------------------------------------------------------------
//  UART functions
//-------------------------------------------------------------------------------

#define MSG_ALWAYS              0x00
#define MSG_STAT                0x01
#define MSG_DBG                 0x00

extern  uint8_t     g_halMsgLevel;

//-------------------------------------------------------------------------------
//  IR Interface functions
//-------------------------------------------------------------------------------

typedef enum
{
    KEY_DOWN,  //!< remote control key pressed down
}ENUM_KEY;

typedef struct
{
    uint8_t eventType;  //!< UI event type
    uint8_t command;    //!< UI command
} IrCommand_t;

#endif  // _HAL_H__
