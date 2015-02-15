//***************************************************************************
//!file     si_cpIr.c
//!brief    CP 9387 Starter Kit top level IR Remote command parser.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

/***** #include statements ***************************************************/

#include "si_RegioCbus.h"
#include "si_regio.h"
#include "si_hal.h"
#include "si_datatypes.h"
#include "si_cp1292.h"
#include "si_cbusDefs.h"
#include "si_cbus_regs.h"
#include "si_cbus.h"
#include "si_apiRCP.h"
#include "si_apiRAP.h"
#include "si_apiMSC.h"
#include "si_apiConfigure.h"
#include "si_apiCbus.h"
#include "si_api1292.h"
#include "si_1292regs.h"
#include "drv_sil_if.h"

// unused,  for warning free
//static bool_t   l_returnRC5 = false;


//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t timer;  //!< UI event type
    uint8_t command;    //!< UI command
} Irlast_t;

Irlast_t irlast = {0, 0};

typedef enum
{
    RC5_0,
    RC5_1,
    RC5_2,
    RC5_3,
    RC5_4,
    RC5_5,
    RC5_6,
    RC5_7,
    RC5_8,
    RC5_9,
    RC5_STANDBY = 0x0C,     //
    RC5_MUTE_OK = 0x0D,     //
    RC5_QUIT         	= 0x0F,
    RC5_VOL_UP   = 0x10,            //
    RC5_VOL_DOWN = 0x11,     //
    RC5_ENTER		= 0x1a,
    RC5_PRG_UP   = 0x20,            //
    RC5_PRG_DOWN = 0x21,     //

    RC5__PAUSE   = 0x29,            // Device 5
    RC5_UP			= 0x1c,
    RC5_DOWN		= 0x1d,
    RC5_RIGHT		= 0x2b,
    RC5_LEFT			= 0x2c,
    RC5_MENU		= 0x2e,
    RC5_FAST_REW = 0x32,     // Device 5
    RC5_FAST_FWD = 0x34,     // Device 5
    RC5_PLAY = 0x35,     // Device 5
    RC5_STOP = 0x36,     // Device 5
    RC5_REC = 0x37,     // Device 5

    RC5__INPUT  = 0x38,             // Device 5
    RC5__TV_VCR = 0x3E,     // Device 5
}ENUM_RC_CODE;

//------------------------------------------------------------------------------
// Function:    CpIrHandler
// Description: Check the IR port for a command and process it if found.
//------------------------------------------------------------------------------

bool_t CpIrHandler ( void )
{
    IrCommand_t  irCommand;

	if (g_currentInputMode == MHL)
	{
		SI_IrRcpKeys( irCommand.command, irCommand.eventType );
	}
    return( true );
}

BOOL fgSil_RcpKeys(uint8_t key)
{
   	IrCommand_t  irCommand;
	irCommand.command = key;
	if (g_currentInputMode == MHL)
	{
	   SI_IrRcpKeys( irCommand.command, 0);
	}
    return( true );
}

BOOL fgSil_RapKeys(uint8_t key)
{
	bool_t result = FAIL;

	if (g_currentInputMode == MHL)
	{
		if(key == MHL_RAP_CMD_CONTENT_ON)
		{

			result = SI_RapSendMsg(0, MHL_RAP_CMD_CONTENT_ON);
			DEBUG_PRINT(MSG_ALWAYS, ("RAP:: Send MHL_RAP_CMD_CONTENT_ON\n"));
		}
		else if(key == MHL_RAP_CMD_CONTENT_OFF)
		{

			result = SI_RapSendMsg(0, MHL_RAP_CMD_CONTENT_OFF);
			DEBUG_PRINT(MSG_ALWAYS, ("RAP:: Send MHL_RAP_CMD_CONTENT_OFF\n"));

		}
		else if(key == MHL_RAP_CMD_POLL)
		{

			result = SI_RapSendMsg(0, MHL_RAP_CMD_POLL);
			DEBUG_PRINT(MSG_ALWAYS, ("RAP:: Send MHL_RAP_CMD_POLL\n"));

		}
	}
	
    return result;

}

