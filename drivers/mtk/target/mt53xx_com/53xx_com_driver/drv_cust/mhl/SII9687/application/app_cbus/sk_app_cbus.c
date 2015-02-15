//***************************************************************************
//!file     sk_app_cbus.c
//!brief    Wraps board and device functions for the CBUS component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#include <linux/string.h>
#include "sk_app_cbus.h"
#include "si_common.h"
#include "si_gpio.h"
#include "si_cbus_component.h"
#include "si_drv_cbus.h"
#include "si_drv_switch.h"

#if 0
#include "../si_common.h"
#include "../sk_application.h"
#include "../platform/board/si_gpio.h"
#include "../component/cbus/si_cbus_component.h"
#include "../driver/cbus_drv/si_drv_cbus.h"
#include "../driver/switch_drv/si_drv_switch.h"
#endif


//------------------------------------------------------------------------------
// Module variables
//------------------------------------------------------------------------------

SiiAppCbusInstanceData_t appCbus[MHL_MAX_CHANNELS];
SiiAppCbusInstanceData_t *pAppCbus;

uint8_t portArray[MHL_MAX_CHANNELS] = {SK_CBUS_PORT};


//------------------------------------------------------------------------------
// Description: Set the driver global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------

bool_t SkAppCbusInstanceSet ( uint_t instanceIndex )
{
    if ( instanceIndex < SII_NUM_SWITCH )
    {
        SiiCbusInstanceSet( instanceIndex );
    	pAppCbus = &appCbus[instanceIndex];
    	pAppCbus->instanceIndex = instanceIndex;
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
// Function:    SkAppCbusSendRcpMessage
// Description: Convert input port number to CBUS channel and send the
//              passed RC5 key code as a CBUS RCP key code.
// Parameters:  keyCode - Remote control button code.
// Returns:     true if successful, false if not MHL port or other failure.
//------------------------------------------------------------------------------

bool_t SkAppCbusSendRcpMessage ( uint8_t keyCode )
{
    bool_t  success;

    success = false;
    for ( ;; )
    {
        keyCode = CbusConvertToRcp( keyCode );   // Converting RC5 code to RCP
        if (keyCode == 0xFF)
        {
        	Printf("\r\nCPCBUS:: This device can not send this key code::  %02X \n", (int)keyCode);
            break;
        }

        Printf("\r\nCPCBUS:: Sending %02X keycode.\n", keyCode );
        success = SiiMhlRxSendRCPCmd( keyCode );
        break;
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SkAppCbusSendRapMessage
// Parameters:  actCode - Action code.
// Returns:     true if successful, false if not MHL port or other failure.
//------------------------------------------------------------------------------

bool_t SkAppCbusSendRapMessage ( uint8_t actCode )
{
    bool_t  success = false;

	switch ( actCode )
	{
		case MHL_RAP_CMD_POLL:
		case MHL_RAP_CONTENT_ON:
		case MHL_RAP_CONTENT_OFF:
			if((success = SiiMhlRxSendRAPCmd( actCode )) == false)
			{
				Printf("\r\nUnable to send command....\n" );
			}
			break;
		default:
			Printf("\r\nAction code not recognized!!\n" );
			break;
	}

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SkAppCbusSendUcpMessage
// Parameters:  ucpData - ucp ASCII character
//
// Returns:     true if successful, false if not MHL port or other failure.
//------------------------------------------------------------------------------

bool_t  SkAppCbusSendUcpMessage ( uint8_t ucpData )
{
    bool_t  success;

	if((success = SiiMhlRxSendUCPCmd( ucpData )) == false)
	{
		Printf("\r\nUnable to send ucp....\n" );
	}

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SkAppCbusProcessRcpMessage
// Description: Process the passed RCP message.
// Returns:     The RCPK status code.
//------------------------------------------------------------------------------
static uint8_t SkAppCbusProcessRcpMessage ( uint8_t rcpData )
{
    uint8_t rcpkStatus  = MHL_MSC_MSG_RCP_NO_ERROR;

    Printf("\r\nRCP Key Code: 0x%02X\n", (int)rcpData );

	return( rcpkStatus );
}

//------------------------------------------------------------------------------
//! @brief  Part of the callback from the CBUS component used for RAPx messages
// Returns:     The RAPK status code.
//------------------------------------------------------------------------------
static uint8_t SkAppCbusProcessRapMessage ( uint8_t cmd, uint8_t rapData )
{
    uint8_t rapkStatus = MHL_MSC_MSG_RAP_NO_ERROR;

    Printf("\r\nRAP Action Code: 0x%02X\n", (int)rapData );

	switch ( rapData )
	{
		case MHL_RAP_CMD_POLL:
			break;

		case MHL_RAP_CONTENT_ON:
			// MHL device sent a CONTENT ON message, change our source
			// selection to match the MHL device. DEBUG_PRINT( MSG_DBG, "\nChange TO CONTENT_ON STATE received\n\n", rapData );
			app.portSelect = SK_CBUS_PORT;
			SkAppSwitchPortUpdate(true);
			SiiMhlRxPathEnable( true );
			break;
		case MHL_RAP_CONTENT_OFF:
			// MHL device sent a CONTENT OFF message.  We need do nothing
			break;
		default:
			rapkStatus = MHL_MSC_MSG_RAP_UNRECOGNIZED_ACT_CODE;
			break;
	}

	if (( rapkStatus == MHL_MSC_MSG_RAP_UNSUPPORTED_ACT_CODE ) || ( rapkStatus == MHL_MSC_MSG_RAP_UNRECOGNIZED_ACT_CODE ))
	{
		Printf("\r\nnUnsupported or unrecognized MHL RAP Action Code!!\n" );
	}

    return( rapkStatus );
}
//------------------------------------------------------------------------------
// Function:    SkAppCbusProcessUcpMessage
// Description: Process the passed UCP message.
// Returns:     The UCPK status code.
//------------------------------------------------------------------------------

static uint8_t SkAppCbusProcessUcpMessage ( uint8_t ucpData )
{
    uint8_t ucpkStatus = MHL_MSC_MSG_UCP_NO_ERROR;

    Printf("\r\nUCP ascii Code: 0x%02X\n", ucpData );

    if( ucpData > CBUS_UCP_ASCII_LIMIT )
    {
    	ucpkStatus = MHL_MSC_MSG_UCP_INEFFECTIVE_KEY_CODE;
    	Printf("\r\ncode not effective!!\n" );
    }

	return( ucpkStatus );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlCbRxConnectChange
// Description: This is a callback API for Cbus connection change, prototype
//				is defined in si_cbus_component.h
// Parameters:  connected - self explanatory :)
// Returns:     void
//------------------------------------------------------------------------------
void SiiMhlCbRxConnectChange(bool_t connected)
{
	Printf("\r\nn Notification to Application:: Cbus Connection Status :: %s\n", connected ? "Connected" : "Disconnected" );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlCbRxScratchpadWritten
// Description: This is a callback API for scratchpad being written by peer, prototype
//				is defined in si_cbus_component.h
// Parameters:  none
// Returns:     void
//------------------------------------------------------------------------------
void SiiMhlCbRxScratchpadWritten()
{
	Printf("\r\nnNotification to Application:: Scratchpad written!!\n" );
}

//------------------------------------------------------------------------------
//! @brief  This function is called from the CBUS component when a
//!         MSC_MSG (RCP/RAP) is received.  The prototype is defined in
//!         si_cbus_component.h
//! @param[in]  cmd     - RCP/RCPK/RCPE, RAP/RAPK
//!				msgData - RCP/RAP data
//! @return     RCP/RAP error status
//------------------------------------------------------------------------------
uint8_t SiiMhlCbRxRcpRapReceived( uint8_t cmd, uint8_t msgData )
{
    uint8_t     status = MHL_MSC_MSG_RCP_NO_ERROR;

    Printf("\r\nnApplication layer:: SiiMhlCbRxRcpRapReceived( %02X, %02X )\n", cmd, msgData );
    switch ( cmd )
    {
        case MHL_MSC_MSG_RCP:
            Printf("\r\nRCP received by app.\n" );
            status = SkAppCbusProcessRcpMessage( msgData );
            break;
        case MHL_MSC_MSG_RCPK:
            Printf("\r\nRCPK received by app.\n" );
            break;
        case MHL_MSC_MSG_RCPE:
            Printf("\r\nRCPE received by app.\n" );
            break;
        case MHL_MSC_MSG_RAP:
        	Printf("\r\nRAP received by app.\n" );
			status = SkAppCbusProcessRapMessage( cmd, msgData );
		    break;
        case MHL_MSC_MSG_RAPK:
            Printf("\r\nRAPK received by app.\n" );
            break;
        case MHL_MSC_MSG_UCP:
        	Printf("\r\nUCP received.\n" );
        	status = SkAppCbusProcessUcpMessage( msgData );
            break;
        case MHL_MSC_MSG_UCPK:
        	Printf("\r\nUCPK received.\n" );
            break;
        case MHL_MSC_MSG_UCPE:
        	Printf("\r\nUCPE received.\n" );
            break;
        default:
            break;
    }

    return( status );
}

//------------------------------------------------------------------------------
//! @brief:     Perform any board-level initialization required at the same
//! @brief:     time as CBUS component initialization
//! @params[in] none
// Returns:     none
//------------------------------------------------------------------------------
bool_t SkAppDeviceInitCbus ( void )
{
    uint8_t i;
    Printf("\r\n SkAppDeviceInitCbus...\n");
    memset(&appCbus[0], 0, sizeof(SiiAppCbusInstanceData_t) * MHL_MAX_CHANNELS);

    pAppCbus = &appCbus[0];

    for (i = 0; i < MHL_MAX_CHANNELS; i++)
    {
        appCbus[i].port = portArray[i];
        SiiCbusInstanceSet(i);
        SiiMhlRxInitialize();

		// We are not MHL_LD_VIDEO or MHL_LD_AUDIO because we do not actually
		// source the video or audio (at least from the TMDS perspective)
		appCbus[i].supportMask  = MHL_LD_DISPLAY | MHL_LD_SPEAKER;
		SiiCbusConfigure( appCbus[i].port, appCbus[i].supportMask );
    }
    return (true);
}

//------------------------------------------------------------------------------
// Function:    SkAppCbusResume
//! @brief      Restore operation to all Cbus components/drivers.
// Returns:     true if the CEC component successfully exited standby mode,
//              or false if some failure occurred.
//------------------------------------------------------------------------------
bool_t SkAppCbusResume ( void )
{
    SiiCbusResume();
    return( true );
}

//------------------------------------------------------------------------------
// Function:    SkAppCbusStandby
//! @brief      Places all CBUS components/drivers into standby
// Returns:     true if the CBUS component entered standby mode successfully,
//              or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SkAppCbusStandby ( void )
{
    SiiCbusStandby();
    memset(&appCbus[0], 0, sizeof(SiiAppCbusInstanceData_t) * MHL_MAX_CHANNELS);
    return (true);
}

//------------------------------------------------------------------------------
// Function:    SkAppTaskCbus
// Description: Wrapper for the CBUS Component at the application level
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskCbus ( uint_t inst )
{
    uint8_t status;
    Printf("\r\nSkAppTaskCbus\n");

    SkAppCbusInstanceSet(inst);
    SiiCbusInstanceSet(inst);
    SiiDrvCbusInstanceSet(inst);
    //DEBUG_PRINT( MSG_DBG, "SkAppCbusChannelEnable:%d %d\n", i, pAppCbus->port );

    if (pAppCbus->cbusEnable != pAppCbus->oldCbusEnable)
    {
        DEBUG_PRINT(MSG_DBG,"\nCdSense changed pApp->cbusEnable: %02X, pApp->oldCbusEnable: %02X.\n", pAppCbus->cbusEnable, pAppCbus->oldCbusEnable );
        pAppCbus->oldCbusEnable = pAppCbus->cbusEnable;
    }
    // Is there anything to do?
    if (!pAppCbus->cbusEnable)
    {
        return;
    }

    pAppCbus->cbusInterrupt = false;

    if ((status = SiiMhlRxHandler()) != SUCCESS) // Monitor CBUS interrupts.
    {
        Printf("\r\nSiiMhlRxHandler() failed with error :: %02X\n", status );
    }
    if (!SiiMhlRxIsQueueEmpty() || !(SiiCbusStatus() & FLAGS_SENT_DCAP_RDY))
    {
        // There are unfinished tasks in the CBUS component
        // Re-schedule the task
        SiiSchedTimerSet(15, SkAppTaskCbus, 0, false, SI_SCHED_PRIORITY_NORMAL);
    }
}
