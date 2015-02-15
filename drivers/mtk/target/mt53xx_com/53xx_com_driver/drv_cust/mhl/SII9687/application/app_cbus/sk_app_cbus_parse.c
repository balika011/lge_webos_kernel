//***************************************************************************
//!file     sk_app_cbus_parser.c
//!brief    RCP to CEC to RC5 decoding
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_cbus_component.h"
#include "sk_app_cbus.h"
#include "si_drv_cbus.h"
#include "si_drv_switch.h"
#include <linux/string.h>

//------------------------------------------------------------------------------
// Module variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Structure:   SiiCecRcpConversion_t
// Description: maps RCP codes to RC5 codes
// Commented out commands below are not required for this product
//------------------------------------------------------------------------------
SiiRcpTranslate_t rcpTranslate[] =
{
    {    MHL_RCP_CMD_SELECT,          RC5_SELECT},
    {    MHL_RCP_CMD_UP,              RC5_CURSOR_STEP_UP},
    {    MHL_RCP_CMD_DOWN,            RC5_CURSOR_STEP_DOWN},
    {    MHL_RCP_CMD_LEFT,            RC5_CURSOR_STEP_LEFT},
    {    MHL_RCP_CMD_RIGHT,           RC5_CURSOR_STEP_RIGHT},
    /*
    {    MHL_RCP_CMD_RIGHT_UP,        RC5_UNKNOWN},
    {    MHL_RCP_CMD_RIGHT_DOWN,      RC5_UNKNOWN},
    {    MHL_RCP_CMD_LEFT_UP,         RC5_UNKNOWN},
    {    MHL_RCP_CMD_LEFT_DOWN,       RC5_UNKNOWN},
    */
    {    MHL_RCP_CMD_ROOT_MENU,       RC5_MENU},
    /*
    {    MHL_RCP_CMD_SETUP_MENU,      RC5_UNKNOWN},
    {    MHL_RCP_CMD_CONTENTS_MENU,   RC5_UNKNOWN},
    {    MHL_RCP_CMD_FAVORITE_MENU,   RC5_UNKNOWN},
    */
    {    MHL_RCP_CMD_EXIT,            RC5_UNKNOWN},
      // 0x0E - 0x1F Reserved
    {    MHL_RCP_CMD_NUM_0,           RC5_0},
    {    MHL_RCP_CMD_NUM_1,           RC5_1},
    {    MHL_RCP_CMD_NUM_2,           RC5_2},
    {    MHL_RCP_CMD_NUM_3,           RC5_3},
    {    MHL_RCP_CMD_NUM_4,           RC5_4},
    {    MHL_RCP_CMD_NUM_5,           RC5_5},
    {    MHL_RCP_CMD_NUM_6,           RC5_6},
    {    MHL_RCP_CMD_NUM_7,           RC5_7},
    {    MHL_RCP_CMD_NUM_8,           RC5_8},
    {    MHL_RCP_CMD_NUM_9,           RC5_9},
    /*
    {    MHL_RCP_CMD_DOT,             RC5_UNKNOWN},
    */
    {    MHL_RCP_CMD_ENTER,           RC5_UNKNOWN},
    {    MHL_RCP_CMD_CLEAR,           RC5_UNKNOWN},
      // 0x2D - 0x2F Reserved
    {    MHL_RCP_CMD_CH_UP,           RC5_PRG_UP},
    {    MHL_RCP_CMD_CH_DOWN,         RC5_PRG_DOWN},
    {    MHL_RCP_CMD_PRE_CH,          RC5_ALT_CHANNEL},
    {    MHL_RCP_CMD_SOUND_SELECT,    RC5_UNKNOWN},
    /*
    {    MHL_RCP_CMD_INPUT_SELECT,    RC5_INPUT},
    {    MHL_RCP_CMD_SHOW_INFO,       RC5_},
    {    MHL_RCP_CMD_HELP,            RC5_UNKNOWN},
    {    MHL_RCP_CMD_PAGE_UP,         RC5_UNKNOWN},
    {    MHL_RCP_CMD_PAGE_DOWN,       RC5_UNKNOWN},
    */
      // 0x39 - 0x3F Reserved
    {    MHL_RAP_CONTENT_ON,          RC5_STANDBY},
    {    MHL_RCP_CMD_VOL_UP,          RC5_VOL_UP},
    {    MHL_RCP_CMD_VOL_DOWN,        RC5_VOL_DOWN},
    {    MHL_RCP_CMD_MUTE,            RC5_MUTE_OK},
    {    MHL_RCP_CMD_PLAY,            RC5_PLAY},
    {    MHL_RCP_CMD_STOP,            RC5_STOP},
    {    MHL_RCP_CMD_PAUSE,           RC5_PAUSE},
    {    MHL_RCP_CMD_RECORD,          RC5_TOGGLE},
    {    MHL_RCP_CMD_REWIND,          RC5_FAST_REW},
    {    MHL_RCP_CMD_FAST_FWD,        RC5_FAST_FWD},
    {    MHL_RCP_CMD_EJECT,           RC5_UNKNOWN},
    {    MHL_RCP_CMD_FWD,             RC5_UNKNOWN},
    {    MHL_RCP_CMD_BKWD,            RC5_UNKNOWN},
      // 0X4D - 0x4F Reserved
    /*
    {    MHL_RCP_CMD_ANGLE,           RC5_UNKNOWN},
    {    MHL_RCP_CMD_SUBPICTURE,      RC5_UNKNOWN},
    */
      // 0x52 - 0x5F Reserved
    {    MHL_RCP_CMD_PLAY_FUNC,       RC5_UNKNOWN},
    {    MHL_RCP_CMD_PAUSE_PLAY_FUNC, RC5_UNKNOWN},
    {    MHL_RCP_CMD_RECORD_FUNC,     RC5_UNKNOWN},
    {    MHL_RCP_CMD_PAUSE_REC_FUNC,  RC5_UNKNOWN},
    {    MHL_RCP_CMD_STOP_FUNC,       RC5_UNKNOWN},
    {    MHL_RCP_CMD_MUTE_FUNC,       RC5_UNKNOWN},
    {    MHL_RCP_CMD_UN_MUTE_FUNC,    RC5_UNKNOWN},
    /*
    {    MHL_RCP_CMD_TUNE_FUNC,       RC5_UNKNOWN},
    {    MHL_RCP_CMD_MEDIA_FUNC,      RC5_UNKNOWN},
      // 0x69 - 0x70 Reserved
    {    MHL_RCP_CMD_F1,              RC5_UNKNOWN},
    {    MHL_RCP_CMD_F2,              RC5_UNKNOWN},
    {    MHL_RCP_CMD_F3,              RC5_UNKNOWN},
    {    MHL_RCP_CMD_F4,              RC5_UNKNOWN},
    {    MHL_RCP_CMD_F5,              RC5_UNKNOWN}
    {   = 0x76
    */
                                        // 0x77 - 0x7F Reserved
};

//------------------------------------------------------------------------------
// Function:    CbusConvertToRcp
// Description: Translate RC5 RC command to CBUS RCP command
// Parameters:  keyData: RC5 key code
// Returns:     RCP code equivalent or 0xFF if not in list.
//------------------------------------------------------------------------------

uint8_t CbusConvertToRcp ( uint8_t keyData )
 {
    uint8_t i;
    uint8_t retVal = 0xFF;
    uint8_t length = sizeof(rcpTranslate)/sizeof(SiiRcpTranslate_t);

    // translate RC5 command to RCP command
    for ( i = 0; i < length ; i++ )
    {
        if ( keyData == rcpTranslate[i].rc5KeyCode )
        {
            retVal = rcpTranslate[i].rcpKeyCode;
            break;
        }
    }

    /* Return the new code or 0xFF if not found.    */
    return( ( i == length ) ? 0xFF : retVal );
 }

//------------------------------------------------------------------------------
// Function:    CbusConvertFromRcp
// Description: Translate CBUS RCP command to CEC RC command or RC5 key
// Parameters:  keyData: key code from the RCP Message
// Returns:     CEC or RC5 code equivalent or 0xFF if not in list.
//------------------------------------------------------------------------------

uint8_t CbusConvertFromRcp ( uint8_t keyData, bool_t returnCec )
 {
    uint8_t i;
    uint8_t retVal = 0xFF;
    uint8_t length = sizeof(rcpTranslate)/sizeof(SiiRcpTranslate_t);

    // translate RC5 command to RCP command

    for ( i = 0; i < length ; i++ )
    {
        if ( keyData == rcpTranslate[i].rcpKeyCode )
        {
            retVal = rcpTranslate[i].rc5KeyCode;
            break;
        }
    }

    /* Return the new code or 0xFF if not found.    */

    return( ( i == length ) ? 0xFF : retVal );
 }

//------------------------------------------------------------------------------
// Function:    SkAppCbusRemoteControlHandler
// Description: Pass the appropriate remote control keys over the CBUS.
//------------------------------------------------------------------------------
bool_t SkAppCbusRemoteControlHandler ( uint8_t keyCode )
{

    if ( !pAppCbus->cbusEnable )
    {
        return( false );
    }

    switch ( keyCode )
    {
			case RC5_PAUSE:
			case RC5_FAST_REW:
			case RC5_FAST_FWD:
			case RC5_PLAY:
			case RC5_STOP:
			case RC5_TOGGLE:	// RC5_REC
			case RC5_VOL_UP:
			case RC5_VOL_DOWN:
			case RC5_MUTE_OK:
			case RC5_PRG_UP:
			case RC5_PRG_DOWN:

            // If the source is MHL and
            // connected, pass the key code to the source device by RCP.
            if (( SK_CBUS_PORT == app.portSelect) && SiiMhlRxCbusConnected())
            {
                SkAppCbusSendRcpMessage( keyCode );
            }
            break;
    }

    // Don't mark any keys as being used, because they can also be used by other handlers
    return( false );
}
