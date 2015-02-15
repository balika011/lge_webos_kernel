//***************************************************************************
//!file     si_api1292.h
//!brief    SiI1292 API functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_API1292_H__
#define __SI_API1292_H__
#include "si_datatypes.h"
#include "si_hal.h"
//------------------------------------------------------------------------------
//  Compile-time build options
//------------------------------------------------------------------------------
#define API_DEBUG_CODE          1       // Set to 0 to eliminate debug print messages from code

    /* The following flags determine what feature set is compiled.  */

extern char strDeviceID [];

#define VBUS_ON 1
#define VBUS_OFF 0

    // Debug Macros


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

typedef struct
{
    char    debugTraceVerStr[5];    // Changed each time the debug labels are
                                    // changed to keep track of what values
                                    // a specific version is monitoring.
    uint8_t demFlags1;
    uint8_t demFlags2;
} DEBUG_DATA_t;

extern DEBUG_DATA_t g_dbg;

#endif

// MSG API   - For Debug purposes

#define MSG_ALWAYS              0x00
#define MSG_STAT                0x01
#define MSG_DBG                 0x00

//void    * malloc( uint16_t byteCount );
//void      free( void *pData );

//------------------------------------------------------------------------------
//
//  Other Manifest Constants
//
//------------------------------------------------------------------------------

typedef enum
{
    SI_DEV_INPUT_CONNECTED = 1,
    SI_DEV_BOOT_STATE_MACHINE,
    SI_DEV_NVRAM,
    SI_DEV_IDH,
    SI_DEV_IDL,
    SI_DEV_REV,
    SI_DEV_ACTIVE_PORT
} SI_DEV_STATUS;

//-------------------------------------------------------------------------------
//  API Function Prototypes
//-------------------------------------------------------------------------------

uint8_t SI_DeviceStatus( uint8_t statusIndex );
enum
{
    SI_PWRUP_FAIL_DEVID_READ = 0x00,
    SI_PWRUP_FAIL_BOOT = 0x01,
    SI_PWRUP_FAIL_NVRAM_INIT = 0x02,
    SI_PWRUP_BASE_ADDR_B0 = 0xB0,
    SI_PWRUP_BASE_ADDR_B2 = 0xB2
};

void	SI_DeviceEventMonitor( void );

enum
{
    SI_HPD_ACTIVE = 0x00,     // HPD HI, HDCP, EDID, RX Term enabled
    SI_HPD_INACTIVE,                        // HPD LOW, HDCP, RX Term disabled
    SI_HPD_ACTIVE_EX,                       // EDID, RX Term enabled
    SI_HPD_INACTIVE_EX,                     // HPD HI, HDCP, EDID, RX Term disabled
    SI_HPD_TOGGLE,                  // Special use for CBUS connect
};

enum
{
    SI_TERM_HDMI = 0x00,     // Enable for HDMI mode
    SI_TERM_MHL = 0x55,     // Enable for MHL mode
    SI_TERM_DISABLE = 0xFF,     // Disable
};

#define RCP_DEBUG 0


#define CBUS_CMD_RESPONSE_TIMEOUT 10      // In 100ms increments
#define CBUS_CONNECTION_CHECK_INTERVAL 40      // In 100ms increments

#define CBUS_FW_COMMAND_TIMEOUT_SECS 1       // No response for 1 second means we hold off and move
#define CBUS_FW_INTR_POLL_MILLISECS 50      // wait this to issue next status poll i2c read
#define CBUS_FW_HOLD_OFF_SECS 2       // Allow bus to quieten down when ABORTs are received.

//Events definition
#define MHL_EVENT 0x01
#define HDMI_EVENT 0x02
#define TV_EVENT 0x04
#define RSEN_EVENT 0x08
#define SCDT_EVENT 0x10

//Wake up pulse definition
#define MIN_WAKE_PULSE_WIDTH_1 5
#define MAX_WAKE_PULSE_WIDTH_1 40
#define MIN_WAKE_PULSE_WIDTH_2 20
#define MAX_WAKE_PULSE_WIDTH_2 100

#define DEM_HDMI_VALID_TIME 500     // HDMI valid time 500ms

//when AC/DC PwrOn (1292 no pwr in STBY), HPD_IN will low to high.
//per 100ms to 300ms (DEM_RSEN_VALID_TIME), PS_CTRL# will change to default open drain high 5v and 0xD0:0x7A=0xD4.
//therefore re-set 0xD0:0x7A again.
#define DEM_RSEN_VALID_TIME							300

//Keno17MAY2012, !!!Warning!!!
//Root Cause::	MHL CTS4.2.6.2, TE is not following MHL spec to waiting SINK SET_HPD, TE just read EDID & do HDCP.
//Woragound:: 	reduce DEM_MHL_WAIT_TIME 600 to 100.
//Explain::		let SINK send SET_HPD quickly, TE will get SET_HPD CMD before do EDID/HDCP.
//SL860 TE SW Ver::	1.02.0400
#define DEM_MHL_WAIT_TIME							100//Keno20120820-1, 600 changes to 100, because Google Nexus phone detects connection timer. If Nexus no connection too long, Nexus will disable D+/-.			// MHL wait time after connection 600ms
#define DEM_MHL_RCP_TIMEOUT							1000		// MHL Rcp/Rap wait time	
#define DEM_MSC_MAX_REPLY_DELAY						25			// MSC Max reply delay for transfer done, 100ms
#define DEM_CBUS_ABORT_WAIT							2500		// CBUS Abort wait time, 2500ms

void vPsCtrl(uint8_t value);

#endif  // __SI_API1292_H__

