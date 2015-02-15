//***************************************************************************
//!file     si_debug.h
//!brief    Silicon Image Debug header.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_DEBUG_H__
#define __SI_DEBUG_H__

//------------------------------------------------------------------------------
// Debug Flags
//------------------------------------------------------------------------------
#define API_DEBUG_CODE     0      // Set to 0 to eliminate debug print messages from code
                                    // Set to 1 to allow debug print messages in code
//------------------------------------------------------------------------------
// Debug Macros
//------------------------------------------------------------------------------

    // Debug print message level

#define MSG_ALWAYS              0x00
#define MSG_ERR                 0x01
#define MSG_STAT                0x02
#define MSG_DBG                 0x03

#if (API_DEBUG_CODE==1)

    #ifndef DEBUG_PRINT
    #define DEBUG_PRINT(x,...)  SiiPlatformLoggerPrintf(x,__VA_ARGS__)
    #endif
    #define _DEBUG_(x)              x

#else
    #ifndef DEBUG_PRINT
        #define DEBUG_PRINT(...)
    #endif
    #define _DEBUG_(x)
#endif

#if 0//INC_LCD_DISP
#define SI_LCD_INIT(x)          SiiPlatformLcdInit(x)
#define SI_LCD_CLEAR()          SiiPlatformLcdClear();
#define SI_LCD_PRINT(x,y)       SiiPlatformLcdPrint(x,y);
#define SI_LCD_Printf(x,...)    SiiPlatformLcdPrintf(x,__VA_ARGS__);
#define SI_LCD_TEXT_MOVE(x)     SiiPlatformLcdMoveText(x);
#else
#define SI_LCD_INIT(x)          false
#define SI_LCD_CLEAR()
#define SI_LCD_PRINT(x,y)
#define SI_LCD_Printf(x,...)
#define SI_LCD_TEXT_MOVE(x)
#endif

//------------------------------------------------------------------------------
// Debug Status Codes
//------------------------------------------------------------------------------

enum
{
    I2C_SUCCESS,
    I2C_READ_FAIL,
    I2C_WRITE_FAIL,
};

//------------------------------------------------------------------------------
// Debug Trace
//------------------------------------------------------------------------------

#if (API_DEBUG_CODE==1)
    #define DF1_SW_HDCPGOOD         0x01
    #define DF1_HW_HDCPGOOD         0x02
    #define DF1_SCDT_INT            0x04
    #define DF1_SCDT_HI             0x08
    #define DF1_SCDT_STABLE         0x10
    #define DF1_HDCP_STABLE         0x20
    #define DF1_NON_HDCP_STABLE     0x40
    #define DF1_RI_CLEARED          0x80

    #define DF2_MP_AUTH             0x01
    #define DF2_MP_DECRYPT          0x02
    #define DF2_HPD                 0x04
    #define DF2_HDMI_MODE           0x08
    #define DF2_MUTE_ON             0x10
    #define DF2_PORT_SWITCH_REQ     0x20
    #define DF2_PIPE_PWR            0x40
    #define DF2_PORT_PWR_CHG        0x80

#endif

//------------------------------------------------------------------------------
//  From si_platform.h
//------------------------------------------------------------------------------
#if !defined __PLATFORM_STATUS_CODES__
#define __PLATFORM_STATUS_CODES__

// NOTE: the following codes must match dbgMsgComponentNames[] in si_dbgprint.c
typedef enum _SkDebugPrintModule_t
{
    DBG_BRD     = 0,
    DBG_DEV     = 1,
    DBG_CBUS    = 2,
    DBG_EDID    = 6,
    DBG_HEAC    = 7,
    DBG_SWCH    = 9,
    DBG_VSIF    = 10,
    DBG_TX      = 11,

    DBG_MAX_COMPONENTS
} SkDebugPrintModule_t;

typedef enum _SkDebugPrintflags_t
{
    DBGF_TS     = 0x0100,       // Add Time Stamp to debug message output
    DBGF_CN     = 0x0200,       // Add Component Name to debug message output
                                // (must include component name index as first
                                // parameter after format string)
    DBGF_CP     = 0x0400,       // Add Component Name to debug message output
                                // (must include pointer to component instance
                                // data as first parameter after format string)
    DBGF_CS     = 0x0800,       // Add Component Name to debug message output
                                // (must include pointer to display string
                                // as first parameter after format string)
} SkDebugPrintflags_t;

void    SiiPlatformLoggerPrintf( uint_t Printflags, ... );

#endif // __PLATFORM_STATUS_CODES__


#endif // __SI_DEBUG_H__
