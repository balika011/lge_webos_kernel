//***************************************************************************
//!file     si_apiConfigure.h
//!brief    Configure the SiI1292 API for a specific system design.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_APICONFIGURE_H__
#define __SI_APICONFIGURE_H__
#include "si_datatypes.h"
#include "si_regio.h"              
#include "si_api1292.h"


//------------------------------------------------------------------------------
//  Compile-time build options
//------------------------------------------------------------------------------

#define API_DEBUG_CODE          1       // Set to 0 to eliminate debug print messages from code
//#define CHIP_REVISION           ES10    // Set according to silicon revision
#define MHL_WORKAROUND          1       // Set to 1 to enable MHL workaround

//------------------------------------------------------------------------------
//  Device-specific constants
//------------------------------------------------------------------------------

#define ES00            0
#define ES01            1
#define ES10            2

#define SII_MAX_PORT    1

//------------------------------------------------------------------------------
// Target System specific
//------------------------------------------------------------------------------

#if (API_DEBUG_CODE==1)
#ifndef SUPPORT_ARM_CC
#include <stdio.h>
#endif
#endif

extern char g_strDeviceID [];

    // HDMI port on board that is connected to ARC 

#define HEAC_ARC_PORT                   1

    // MSG API   - For Debug purposes

#define MSG_ALWAYS              0x00
#define MSG_STAT                0x01
#define MSG_DBG                 0x00

    // Timer API - Target system must create these functions
#if 0
#define ELAPSED_TIMER               0xFF
#define ELAPSED_TIMER1              0xFE
#define TIMER_0                     0   // DO NOT USE - reserved for TimerWait()
#define TIMER_POLLING               1   // Reserved for main polling loop
#define TIMER_2                     2   // Available
#define TIMER_3                     3   // Available
void        HalTimerSet( uint8_t timer, uint16_t m_sec );
void        HalTimerWait( uint16_t m_sec );
uint16_t    HalTimerElapsed( uint8_t index );
uint8_t     HalTimerExpired( uint8_t timer );
#endif

//void    *malloc( uint16_t byteCount );
//void    free( void *pData );

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

extern DEBUG_DATA_t g_dbg;

#endif

//------------------------------------------------------------------------------
//  MHL/CBUS
//------------------------------------------------------------------------------

#define RCP_DEBUG                       0

#define CBUS_CMD_RESPONSE_TIMEOUT       10      // In 100ms increments
#define CBUS_CONNECTION_CHECK_INTERVAL  40      // In 100ms increments

#define	CBUS_FW_COMMAND_TIMEOUT_SECS	1		// No response for 1 second means we hold off and move
#define	CBUS_FW_INTR_POLL_MILLISECS		50		// wait this to issue next status poll i2c read
#define	CBUS_FW_HOLD_OFF_SECS			2		// Allow bus to quieten down when ABORTs are received.

//------------------------------------------------------------------------------
//  API_SWITCH definitions
//------------------------------------------------------------------------------

#define DEM_HDMI_VALID_TIME		500    	// HDMI valid time 500ms

//when AC/DC PwrOn (1292 no pwr in STBY), HPD_IN will low to high.
//per 100ms to 300ms (DEM_RSEN_VALID_TIME), PS_CTRL# will change to default open drain high 5v and 0xD0:0x7A=0xD4.
//therefore re-set 0xD0:0x7A again.
#define DEM_RSEN_VALID_TIME		300
#define DEM_MHL_WAIT_TIME			100		// MHL wait time after connection 600ms
#define DEM_MHL_RCP_TIMEOUT		1000	// MHL Rcp/Rap wait time	

#endif  // __SI_APICONFIGURE_H__
