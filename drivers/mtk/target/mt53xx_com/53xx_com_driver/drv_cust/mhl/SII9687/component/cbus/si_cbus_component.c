//***************************************************************************
//!file     si_cbus_component.c
//!brief    Silicon Image CBUS Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#include <linux/string.h>
#include "si_cbus_internal.h"
#include "si_common.h"
#include "si_drv_cbus.h"
#include "si_debug.h"
//#include "si_cra_cfg.h"
//#include "si_cra.h"

#if (MHL_EXTRA == ENABLE)
#include "si_edid_component.h"
#include "sk_app_ediddata.h"

// 2D SVDs and DTDs that are supported in 3D
//extern const uint8_t g3DEdidTableVIC [ EDID_3D_VIC_TABLE_LEN ];
//extern const uint8_t g3DEdidTableDTD [ EDID_3D_DTD_TABLE_LEN ];
#endif

//  active index in cbus queue
#define CH_ACTIVE_INDEX     (pCbus->activeIndex)

//static functions
static void 	CBusProcessSubCommand (uint8_t* vsCmdData);
static void 	CBusResetToIdle (void);
static int_t 	CBusSendNextInQueue (void);
static void 	CBusDumpQueueContents ( void );
static void 	CBusClearQueueEntry ( uint8_t cmd );
static bool_t 	CBusWriteCommand ( cbus_req_t *pReq  );
static bool_t 	CBusWritePeersScratchpad(void);
static uint8_t 	CBusCheckInterruptStatus (void);
static bool_t 	CBusSendDcapRdyMsg (void);
static bool_t 	CBusSetInt ( uint8_t regOffset, uint8_t regBit );

static bool_t 	CBusSendMscCommand(uint8_t cmd);
static bool_t 	CBusSendDscrChange (void);
static bool_t 	CBusReqWrt (void);
static bool_t 	CBusGrtWrt (void);
static bool_t 	CBusWriteStatus ( uint8_t regOffset, uint8_t value );
static bool_t 	CBusSendDcapRdy (void);
static bool_t 	CBusTimerExpired(clock_time_t *pTimer, uint16_t waitTime);
static void 	CBusChkTimers (void);

#if (MHL_EXTRA == ENABLE)
static void 	CBusSend3DInfo(void );
#endif


//------------------------------------------------------------------------------
//  CBUS Component Instance Data
//------------------------------------------------------------------------------

CbusInstanceData_t cbusInstance[MHL_MAX_CHANNELS];
CbusInstanceData_t *pCbus = &cbusInstance[0];

//------------------------------------------------------------------------------
//! @brief  Assign port and support mask to this CBUS instance (channel)
//! @param[in]  port - Receiver input port
//! @param[in]  supportMask - combination of SiiMhlLogicalDeviceTypes_t types
//!                           supported by this device.
//------------------------------------------------------------------------------
void SiiCbusConfigure ( uint8_t port, uint8_t supportMask )
{
    pCbus->port = port;
    pCbus->supportMask  = supportMask;
    SiiDrvCbusConfigure(port);
}

//------------------------------------------------------------------------------
// Description: Set the component global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------

bool_t SiiCbusInstanceSet ( uint_t instanceIndex )
{
    if ( instanceIndex < MHL_MAX_CHANNELS )
    {
        SiiDrvCbusInstanceSet( instanceIndex );
        pCbus = &cbusInstance[ instanceIndex];
        pCbus->instanceIndex = instanceIndex;
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Description: Set the component global data pointer to the requested instance
//				based on Port
// Parameters:  portIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------
void SiiCbusInstanceSetBasedOnPort ( uint_t portIndex )
{
	uint8_t i;

	for ( i=0; i < MHL_MAX_CHANNELS; i++ )
	{
		pCbus = &cbusInstance[ i ];
		if( pCbus->port == portIndex )
		{
			//Printf("\r\n\n\n*******Setting Instance: %02X\n", i);
			SiiCbusInstanceSet(i);
			break;
		}
	}
}

//------------------------------------------------------------------------------
// Function:    SiiCbusStatus
// Description: Returns a status flag word containing CBUS-specific
//				information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit misc flags word for the CBUS Component
//------------------------------------------------------------------------------

uint16_t SiiCbusStatus ( void )
{
	return( pCbus->miscFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiCbusStandby
// Description: Places the CBUS component into standby mode if available
// Parameters:  none
// Returns:     true if the CBUS component entered standby mode successfully,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiCbusStandby ( void )
{
	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiCbusResume
// Description: Causes operation of the CBUS component to return to the state
//				it was prior to the previous call to SiiCbusStandby.
// Parameters:  none
// Returns:     true if the CBUS component successfully exited standby mode,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiCbusResume ( void )
{
	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxCbusConnected
// Description: Return the CBUS channel connected status for this channel.
// Returns:     true if connected.
//              false if disconnected.
//------------------------------------------------------------------------------

bool_t SiiMhlRxCbusConnected (void)
{
    return( pCbus->connected );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxChannelEnable
// Description: Enable or disable the specified CBUS channel.
// param		isEnable - true to enable, false to disable
//------------------------------------------------------------------------------
void SiiMhlRxChannelEnable ( bool_t isEnable )
{
	SiiDrvCbusChnEn( isEnable, pCbus->port );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxIsQueueFull
// Description: is cbus queue full ?
// Parameters:  none
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t SiiMhlRxIsQueueFull ( void )
{
    int_t   queueIndex;

	for ( queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE; queueIndex++ )
	{
		if ( pCbus->request[ queueIndex].reqStatus == CBUS_REQ_IDLE )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxIsQueueEmpty
// Description: is cbus queue empty ?
// Parameters:  none
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t SiiMhlRxIsQueueEmpty ( void )
{
    int_t   queueIndex;

    for ( queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE; queueIndex++ )
    {
        if ( pCbus->request[ queueIndex].reqStatus == CBUS_REQ_PENDING )
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendRAPCmd
// Description: Send MSC_MSG (RAP) message to the specified CBUS channel (port)
//
// Parameters:  actCode 	- RAP action code
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------
bool_t SiiMhlRxSendRAPCmd ( uint8_t actCode )
{
    return CBusSendMscMsgCmd(MHL_MSC_MSG_RAP, actCode);
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendUCPCmd
// Description: Send MSC_MSG (UCP) message to the specified CBUS channel (port)
//
// Parameters:  asciiCode 	- UCP ASCII character
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------
bool_t SiiMhlRxSendUCPCmd ( uint8_t asciiCode )
{
    return CBusSendMscMsgCmd(MHL_MSC_MSG_UCP, asciiCode);
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxGetMscMsgCmd
// Description: Get MSC_MSG (RCP/RAP) message that came on CBUS channel
//
// Parameters:  cmd    		- MSC_MSG cmd (RCP/RAP/UCP)
//              data     	- keyCode
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------

bool_t SiiMhlRxGetMscMsgCmd ( uint8_t* cmd, uint8_t* data )
{
    *cmd = pCbus->lastCbusRcvdCmd;
    *data = pCbus->lastCbusRcvdData;
	return true;
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendRcpk
// Description: Send RCPK (ack) message
//
// Parameters:  keyCode
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------

bool_t SiiMhlRxSendRcpk ( uint8_t keyCode)
{
    return( CBusSendMscMsgCmd( MHL_MSC_MSG_RCPK, keyCode ));
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendUcpe
// Description: Send UCPE (error) message
//
// Parameters:  cmdStatus
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------
bool_t SiiMhlRxSendUcpe ( uint8_t cmdStatus )
{
    return( CBusSendMscMsgCmd( MHL_MSC_MSG_UCPE, cmdStatus ));
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendUcpk
// Description: Send UCPK (ack) message
//
// Parameters:  asciiCode
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------

bool_t SiiMhlRxSendUcpk ( uint8_t asciiCode)
{
    return( CBusSendMscMsgCmd( MHL_MSC_MSG_UCPK, asciiCode ));
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendRcpe
// Description: Send RCPE (error) message
//
// Parameters:  cmdStatus
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------
bool_t SiiMhlRxSendRcpe ( uint8_t cmdStatus )
{
    return( CBusSendMscMsgCmd( MHL_MSC_MSG_RCPE, cmdStatus ));
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendRapk
// Description: Send RAPK (acknowledge) message to the specified CBUS channel
//              and set the request status to idle.
//
// Parameters:  cmdStatus
// Returns:     true        - successful queue/write
//              false       - write and/or queue failed
//------------------------------------------------------------------------------

bool_t SiiMhlRxSendRapk ( uint8_t cmdStatus )
{
    return( CBusSendMscMsgCmd( MHL_MSC_MSG_RAPK, cmdStatus ));
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendMsge
// Description: Send MSGE msg back if the MSC command received is not recognized
//
// Returns:     true        - successful
//              false       - failed
//------------------------------------------------------------------------------

bool_t SiiMhlRxSendMsge (void)
{
    return( CBusSendMscMsgCmd( MHL_MSC_MSG_E, MHL_MSC_INVALID_SUBCMD ));
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxHpdSet
// Description: Send MHL_SET_HPD to source
// parameters:	setHpd - self explanatory :)
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t SiiMhlRxHpdSet (bool_t setHpd)
{
	if( setHpd )
	{
		if( SiiMhlRxCbusConnected() )
		{
			if( !CBusSendMscCommand( MHL_SET_HPD ) )
				return false;
			if( !SiiMhlRxPathEnable(true) )
				return false;
		}
    }
	else
	{
		if( SiiMhlRxCbusConnected() )
		{
			if( !SiiMhlRxPathEnable(false) )
				return false;
			if( !CBusSendMscCommand( MHL_CLR_HPD ) )
				return false;
		}
    }

	return true;
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxWritePeerScratchpad
// Description: sends MHL write burst cmd to peer
// Parameters:  startOffset - start offset of peer's scratchpad register
//				length - total# of scratchpad registers to be written
//				pMsgData - pointer to data needs to be written in peer's scratchpad
// Returns:     SUCCESS or one of CBUS_SOFTWARE_ERRORS_t
//------------------------------------------------------------------------------

bool_t SiiMhlRxWritePeerScratchpad(uint8_t startOffset, uint8_t length, uint8_t* pMsgData)
{
	if( pCbus->miscFlags & FLAGS_SCRATCHPAD_BUSY )
	{
		Printf("\r\nScratchpad busy!! Please come back later!!\n" );
		return false;
	}

	pCbus->wbLength = length;
	pCbus->wbStartOffset = startOffset;

	// write different values in different registers
	SiiDrvCbusWriteLocalScratchpad( startOffset, length, pMsgData );

	// send REQ_WRT interrupt to peer
	if( !(CBusReqWrt()) )
	{
		Printf("\r\nCouldn't send REQ_WRT to peer\n" );
		return false;
	}

	pCbus->miscFlags |= FLAGS_SCRATCHPAD_BUSY;

	return true;
}


//------------------------------------------------------------------------------
// Function:    SiiMhlRxReadDevCapReg
// Description: Read device capability register
// Parameters:  regOffset - offset of peer's device capability register
// Returns:     true/false
//------------------------------------------------------------------------------

bool_t SiiMhlRxReadDevCapReg(uint8_t regOffset)
{
	cbus_req_t 	req;
	bool_t		success;

	pCbus->lastCbusSentCmd = req.command = MHL_READ_DEVCAP;
	req.offsetData = regOffset;

	if( !(success = CBusWriteCommand(&req)) )
	{
		Printf("\r\nCouldn't send MHL_READ_DEVCAP to peer\n" );
	}

	return success;
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendEdidChange
// Description: set edid_chg interrupt
//------------------------------------------------------------------------------
bool_t SiiMhlRxSendEdidChange ( void )
{
	return ( CBusSetInt(0x01, BIT1) );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxSendDevCapChange
// Description: sends device capability change interrupt to peer
// Parameters:  none
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t SiiMhlRxSendDevCapChange ( void )
{
	return ( CBusSetInt(0x00, BIT0) );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxPathEnable
//------------------------------------------------------------------------------
bool_t SiiMhlRxPathEnable ( bool_t enable )
{
	bool_t success = true;

//	DEBUG_PRINT( MSG_DBG, "\nSiiMhlRxPathEnable:: pCbus->pathEnableSent = %X.\n", pCbus->pathEnableSent );
	if ( enable )
    {
		if ( !pCbus->pathEnableSent )
		{
			// enable PATH_EN bit on peer's appropriate status register (offset 0x31)
			Printf("\r\n\nSiiMhlRxPathEnable:: Setting bit 3 to peer's status register.\n" );
			if( (success = CBusWriteStatus(0x01, BIT3)) == true )
			{
				pCbus->pathEnableSent = true;
			}
		}
    }
    else
    {
    	if ( pCbus->pathEnableSent )
    	{
            // disable PATH_EN bit on peer's appropriate status register (offset 0x31)
//    		DEBUG_PRINT( MSG_DBG, "\nSiiMhlRxPathEnable:: Clearing bit 3 to peer's status register.\n" );
    		if( (success = CBusWriteStatus(0x01, 0)) == true )
    		{
    			pCbus->pathEnableSent = false;
    		}
    	}
    }

	return success;
}


//------------------------------------------------------------------------------
// Function:    SiiMhlRxHandler
// Description: Check the state of any current CBUS message on specified channel.
//              Handle responses or failures and send any pending message if
//              channel is IDLE.
// Parameters:  channel - CBUS channel to check, must be in range, NOT 0xFF
// Returns:     SUCCESS or one of CBUS_SOFTWARE_ERRORS_t
//------------------------------------------------------------------------------

uint8_t SiiMhlRxHandler ( void )
{
    uint8_t result = SUCCESS;

    /* Check the channel interrupt status to see if anybody is  */
    /* talking to us. If they are, talk back.                   */

	if( !(pCbus->miscFlags & FLAGS_SENT_DCAP_RDY) && !pCbus->initTimer && SiiMhlRxCbusConnected())
	{
		// Send MHL_SET_HPD command
		SiiMhlRxHpdSet(true);

		// send DCAP_RDY and DCAP_CHG
		CBusSendDcapRdyMsg();
	}

	result = CBusCheckInterruptStatus();

    // check for all the f/w timers
    CBusChkTimers();

    /* Update the channel state machine as necessary.   */
	if ( pCbus->state == CBUS_IDLE )
	{
		result = CBusSendNextInQueue(); // No command in progress, write new command immediately.
	}

	if ( pCbus->state & CBUS_XFR_DONE )
	{
		pCbus->state = pCbus->state & ~CBUS_XFR_DONE;
		if ( pCbus->lastCbusSentCmd == MHL_READ_DEVCAP )
		{
			pCbus->lastReadDevCapReg = pCbus->request[ CH_ACTIVE_INDEX ].msgData[0];
			SiiMhlCbRxMscCmdResponseData( pCbus->lastCbusSentCmd, pCbus->lastReadDevCapReg );
		}

		if ( pCbus->lastCbusSentCmd == MHL_WRITE_BURST )
		{
			// send DSCR_CHG interrupt to peer
			if( !(CBusSendDscrChange()) )
			{
				Printf("\r\nCouldn't send DSCR_CHG to peer\n" );
			}
			pCbus->miscFlags &= ~FLAGS_SCRATCHPAD_BUSY;

#if (MHL_EXTRA == ENABLE)
			if ( pCbus->serving3dVicReq || pCbus->serving3dDtdReq )
			{
				CBusSend3DInfo();
			}
#endif
		}

		pCbus->waitTimer = 0;
		if( pCbus->queueDepth )
		{
			pCbus->queueDepth--;
		}
		Printf("\r\n\n-------Transfer Done!!------ "
								"Queue Depth: %02X active index(cbus): %02X\n", pCbus->queueDepth, CH_ACTIVE_INDEX);
		memset( &(pCbus->request[CH_ACTIVE_INDEX]), 0, sizeof( cbus_req_t ) );
	}

	if ( pCbus->state & CBUS_RECEIVED )
	{
	    // Process RCPx/RAPx messages
		pCbus->state = pCbus->state & ~CBUS_RECEIVED;
		if( (pCbus->lastCbusRcvdCmd == MHL_MSC_MSG_RCPK) || (pCbus->lastCbusRcvdCmd == MHL_MSC_MSG_RAPK))
		{
			pCbus->rcpRapAbortTimer = 0;
		}
//		DEBUG_PRINT( MSG_DBG, "\nMSG MSG received!! active index(cbus): %02X\n", CH_ACTIVE_INDEX);
		CbusProcessRcpRap( pCbus->lastCbusRcvdCmd, pCbus->lastCbusRcvdData);
	}

	if ( pCbus->state & CBUS_FAILED )
	{
		pCbus->state = pCbus->state & ~CBUS_FAILED;
		pCbus->request[ CH_ACTIVE_INDEX ].reqStatus = CBUS_REQ_IDLE;
		if( pCbus->queueDepth )
		{
			pCbus->queueDepth--;
		}
		Printf("\r\n\n-----Last Cbus cmd failed!!------"
						"Queue Depth: %02X cmd failed: %02X\n", pCbus->queueDepth, pCbus->request[ CH_ACTIVE_INDEX ].command);
		memset( &(pCbus->request[CH_ACTIVE_INDEX]), 0, sizeof( cbus_req_t ) );
	}

    return( result );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlRxInitialize
// Description: Attempts to initialize the CBUS. If register reads return 0xFF,
//              it declares error in initialization.
//              Initializes discovery enabling registers and anything needed in
//              config register, interrupt masks.
// Returns:     TRUE if no problem
//------------------------------------------------------------------------------

bool_t SiiMhlRxInitialize ( void )
{
    int_t instanceIndex;

    // Clear instance data
    instanceIndex = pCbus->instanceIndex;       // Remember our instance
    memset( pCbus, 0, sizeof( CbusInstanceData_t ));
    memset( &(pCbus->request), 0, sizeof( cbus_req_t ) * CBUS_MAX_COMMAND_QUEUE);
    pCbus->instanceIndex = instanceIndex;       // Restore our instance
    return( SiiDrvCbusInitialize() );
}

//------------------------------------------------------------------------------
// Function:    SiiMhlCbHpdSetState
// Description: This callback is used when the Receiver finishes with all its
//              initialization and ready for Transmitter to read
//              EDID/KSV FIFO/BCAPS ...
// Returns:     true: success
//              false: either port is not MHL or CBUS is not ready.
//------------------------------------------------------------------------------
bool_t SiiMhlCbHpdSetState ( int_t portIndex, SiiDrvHpdActions_t newState )
{
    bool_t success;

    success = false;

    // Translate request to CBUS command.
    switch ( newState )
    {
    case SiiHPD_INACTIVE:
    case SiiHPD_TOGGLE:             // Start with a toggle to OFF
        success = SiiMhlRxHpdSet(false);
        break;
    case SiiHPD_ACTIVE:
    	success = SiiMhlRxHpdSet(true);
        break;
    default:
        break;
    }

    // If a toggle, queue up the second command.
    if ( success && ( newState == SiiHPD_TOGGLE ))
    {
        success = SiiMhlRxHpdSet(true);
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    CBusSendNextInQueue
// Description: Starting at the current active index, send the next pending
//              entry, if any
//------------------------------------------------------------------------------

static int_t CBusSendNextInQueue (void)
{
    int_t   result = SUCCESS;
    int_t   nextIndex = 0;

    if ( ( !pCbus->hpdWaitTimer ) && ( !pCbus->abortTimer ) && ( !pCbus->waitTimer )  )
    {
		nextIndex = (CH_ACTIVE_INDEX == ( CBUS_MAX_COMMAND_QUEUE - 1 )) ? 0 : (CH_ACTIVE_INDEX + 1);

		while (	( pCbus->request[ nextIndex].reqStatus != CBUS_REQ_PENDING ) ||
				( pCbus->rcpeRcpkGapTimer && ( pCbus->request[ nextIndex].msgData[0] == MHL_MSC_MSG_RCPK )) ||
				( pCbus->rcpRapAbortTimer && ( pCbus->request[ nextIndex].msgData[0] == MHL_MSC_MSG )))
		{
			if ( nextIndex == CH_ACTIVE_INDEX )     // Searched whole queue, no pending
			{
				CH_ACTIVE_INDEX = CBUS_MAX_COMMAND_QUEUE - 1;
				return( SUCCESS );  // No pending messages is still success
			}
			nextIndex = ( nextIndex == (CBUS_MAX_COMMAND_QUEUE - 1)) ? 0 : (nextIndex + 1);
		}

//		DEBUG_PRINT( MSG_DBG, "Active Index: %0x\n", nextIndex);
		CBusDumpQueueContents();

		// Found a pending message, send it out
		if ( SiiDrvInternalCBusWriteCommand( &pCbus->request[ nextIndex] ))
		{
			CH_ACTIVE_INDEX = nextIndex;
			pCbus->waitTimer = SiiOsTimerTotalElapsed();
			pCbus->state = CBUS_SENT;
			pCbus->lastCbusSentCmd = pCbus->request[CH_ACTIVE_INDEX].command;

			if( pCbus->request[CH_ACTIVE_INDEX].command == MHL_CLR_HPD )
			{
				pCbus->hpdWaitTimer = SiiOsTimerTotalElapsed();
			}
			if( (pCbus->request[CH_ACTIVE_INDEX].command == MHL_SET_INT) && (pCbus->request[CH_ACTIVE_INDEX].msgData[0] == BIT2) )
			{
				Printf("\r\n\n\nWaiting for peer to respond to reqWrt!!\n");
				pCbus->reqWrtTimer = SiiOsTimerTotalElapsed();
			}
			if( pCbus->request[ nextIndex].command == MHL_MSC_MSG )
			{
				if( (pCbus->request[ nextIndex].msgData[0] != MHL_MSC_MSG_RCPK) && (pCbus->request[ nextIndex].msgData[0] != MHL_MSC_MSG_RAPK) )
				{
    				pCbus->rcpRapAbortTimer = SiiOsTimerTotalElapsed();
				}
				if( pCbus->request[ nextIndex].msgData[0] == MHL_MSC_MSG_RCPE )
				{
					pCbus->rcpeRcpkGapTimer = SiiOsTimerTotalElapsed();
				}
			}
		}
		else
		{
			result = ERROR_WRITE_FAILED;
		}
    }

    return( result );
}

//------------------------------------------------------------------------------
// Function:    CBusProcessSubCommand
// Description: Process a sub-command
//------------------------------------------------------------------------------
static void CBusProcessSubCommand (uint8_t* vsCmdData)
{
    uint8_t vs_cmd, vs_data;

    vs_cmd = vsCmdData[0];
    vs_data = vsCmdData[1];

    // Save MSC_MSG data in the request structure to be returned
    // to the upper level.

    pCbus->lastCbusRcvdCmd = vs_cmd;
    pCbus->lastCbusRcvdData = vs_data;

//    DEBUG_PRINT( MSG_DBG, "CBUS:: MSG_MSC CMD:  0x%02X\n", vsCmdData[0] );
//    DEBUG_PRINT( MSG_DBG, "CBUS:: MSG_MSC Data: 0x%02X\n", vsCmdData[1] );
}

//------------------------------------------------------------------------------
// Function:    CBusResetToIdle
// Description: Set the specified channel state to IDLE. Clears any messages that
//              are in progress or queued.  Usually used if a channel connection
//              changed or the channel heartbeat has been lost.
//------------------------------------------------------------------------------

static void CBusResetToIdle (void)
{
	uint8_t port, instanceIndex, supportMask;

	// we have to preserve the port and instance information
	port = pCbus->port;
	instanceIndex = pCbus->instanceIndex;
	supportMask = pCbus->supportMask;

	memset( pCbus, 0, sizeof( CbusInstanceData_t ));
    memset( &(pCbus->request), 0, sizeof( cbus_req_t ) * CBUS_MAX_COMMAND_QUEUE);

    // fill back the port and instance information
	pCbus->port = port;
	pCbus->instanceIndex = instanceIndex;
	pCbus->supportMask = supportMask;

}


//------------------------------------------------------------------------------
// Function:    CBusSendMscMsgCmd
// Description: Send MSC_MSG message to the specified CBUS channel (port)
//
// Parameters:  subCmd   - MSC_MSG cmd (RCP/RAP/UCP)
//              data     - MSC_MSG data
// Returns:     true     - successful queue/write
//              false    - write and/or queue failed
//------------------------------------------------------------------------------

bool_t CBusSendMscMsgCmd ( uint8_t subCmd, uint8_t data )
{
    cbus_req_t  req;

    // Send MSC_MSG command (Vendor Specific command)

    req.command     = MHL_MSC_MSG;
    req.msgData[0]  = subCmd;
    req.msgData[1]  = data;
	if( !(CBusWriteCommand(&req)) )
	{
		Printf("\r\nCouldn't send MHL_MSC_MSG to peer\n");
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
// Function:    CBusDumpQueueContents
// Description: print the cbus queue contents
// Parameters:  none
// Returns:     void
//------------------------------------------------------------------------------
static void CBusDumpQueueContents ( void )
{
//    int_t   	queueIndex;
//    cbus_req_t 	temp;
//
//    DEBUG_PRINT( MSG_DBG, "SiiMhlRxDumpQueueContents:: ---- \n", pCbus->queueDepth );
//	for ( queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE; queueIndex++ )
//	{
//		temp = pCbus->request[ queueIndex];
//		if( temp.reqStatus != CBUS_REQ_IDLE )
//		{
//			DEBUG_PRINT( MSG_DBG, "index: %0x requestStatus: %0x, cmd: %0x, subcmd: %0x, data: %0x \n", queueIndex, temp.reqStatus, temp.command, temp.msgData[0], temp.msgData[1] );
//		}
//	}
}

//------------------------------------------------------------------------------
// Function:    CBusClearQueueEntry
// Description: clear a particular entry from the queue
// Parameters:  cmd - the one that needs to be removed
// Returns:     void
//------------------------------------------------------------------------------
static void CBusClearQueueEntry ( uint8_t cmd )
{
    int_t   	queueIndex;

//    DEBUG_PRINT( MSG_DBG, "CBusClearQueueEntry:: clearing cmd: %02X\n", cmd );
	for ( queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE; queueIndex++ )
	{
		if( pCbus->request[queueIndex].command == cmd )
		{
			memset( &(pCbus->request[queueIndex]), 0, sizeof( cbus_req_t ) );
			break;
		}
	}
}

//------------------------------------------------------------------------------
// Function:    CBusWriteCommand
// Description: Place a command in the CBUS message queue.
//
// Parameters:  pReq    - Pointer to a cbus_req_t structure containing the
//                        command to write
// Returns:     true    - successful queue/write
//              false   - write and/or queue failed
//------------------------------------------------------------------------------

static bool_t CBusWriteCommand ( cbus_req_t *pReq  )
{
    int_t   queueIndex, loop_count = 0;
    bool_t  success = false;

    /* Copy the request to the queue.   */

    if( SiiMhlRxCbusConnected() )
    {
    	queueIndex = (CH_ACTIVE_INDEX == ( CBUS_MAX_COMMAND_QUEUE - 1 )) ? 0 : (CH_ACTIVE_INDEX + 1);
		for ( loop_count = 0; loop_count < CBUS_MAX_COMMAND_QUEUE; loop_count++ )
		{
			if ( pCbus->request[ queueIndex].reqStatus == CBUS_REQ_IDLE )
			{
				// Found an idle queue entry, copy the request and set to pending.

				memcpy( &pCbus->request[ queueIndex], pReq, sizeof( cbus_req_t ));
				pCbus->request[ queueIndex].reqStatus = CBUS_REQ_PENDING;
				pCbus->queueDepth++;
				success = true;
//				DEBUG_PRINT( MSG_DBG, "\nCBUS:: CBusWriteCommand:: Queue Depth: %02X\n", pCbus->queueDepth );
				break;
			}
			queueIndex++;
			if( queueIndex == CBUS_MAX_COMMAND_QUEUE )
			{
				queueIndex = 0;
			}
		}

		if ( !success )
		{
			Printf("\r\n\nCBUS:: Queue Full\n" );
			for ( queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE; queueIndex++ )
			{
				Printf("\r\n\nCBUS:: CBusWriteCommand:: Queue Index: %02X, Cmd: %02X data: %02X\n",
						queueIndex, pCbus->request[ queueIndex].command, pCbus->request[ queueIndex].offsetData );
			}
		}
    }
    else
    {
    	Printf("\r\nCBus is not connected yet! MHL command could not be sent!\n" );
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    CBusWritePeersScratchpad
// Description: sends MHL write burst cmd
//------------------------------------------------------------------------------
static bool_t CBusWritePeersScratchpad()
{
	cbus_req_t req;

	req.command = MHL_WRITE_BURST;
	req.offsetData = pCbus->wbStartOffset;
	req.length = pCbus->wbLength;

	if( !(CBusWriteCommand(&req)) )
	{
		Printf("\r\nCouldn't send Write Burst to peer\n" );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// Function:    CBusCheckInterruptStatus
// Description: If any interrupts on the specified channel are set, process them.
// Parameters:  none
// Returns:     success or error code
//------------------------------------------------------------------------------
static uint8_t CBusCheckInterruptStatus ( void )
{
    uint8_t result;
    uint8_t busStatus;
    uint8_t	temp;
    uint8_t data[2];

    result = SUCCESS;

    if ( SiiDrvCbusIntrFlagGet() )
    {
        if ( SiiDrvCbusVsDataGet( &data[0] ) )
        {
            CBusProcessSubCommand( &data[0] );
            pCbus->state |= CBUS_RECEIVED;
        }
        if ( SiiDrvCbusCmdRetDataGet( &pCbus->request[ CH_ACTIVE_INDEX ].msgData[0] ) )
        {
        	pCbus->state &= ~CBUS_SENT;

			/* A previous MSC sub-command has been acknowledged by the responder.   */
			pCbus->state |= CBUS_XFR_DONE;

        }
        else
        {
    		if( SiiDrvCbusDdcAbortReasonGet( &temp ) )
    		{
    			Printf("\r\nCBUS DDC ABORT happened, reason: %02X\n", temp );
    			pCbus->state &= ~CBUS_SENT;
    			pCbus->abortTimer = SiiOsTimerTotalElapsed();
    			result = ERROR_CBUS_ABORT;
    			pCbus->state |= CBUS_FAILED;
    		}


    		if ( SiiDrvCbusMscAbortTransReasonGet( &temp ) )
    		{
    			Printf("\r\nMSC CMD aborted (this device was a transmitter), reason: %02X\n", temp );
    			pCbus->state &= ~CBUS_SENT;
    			pCbus->abortTimer = SiiOsTimerTotalElapsed();
    			result = ERROR_CBUS_ABORT;
    			pCbus->state |= CBUS_FAILED;
    		}

    		if ( SiiDrvCbusMscAbortRcvrReasonGet( &temp ) )
    		{
    			Printf("\r\nMSC CMD aborted (this device was a receiver) , reason: %02X\n", temp );
    			pCbus->state &= ~CBUS_SENT;
    			pCbus->abortTimer = SiiOsTimerTotalElapsed();
    			result = ERROR_CBUS_ABORT;
    			pCbus->state |= CBUS_FAILED;
    		}
        }

        if ( SiiDrvCbusBusStatusGet( &busStatus ) )
        {
        	/* The connection change interrupt has been received.   */
			pCbus->connected = busStatus ? true : false;
        	Printf("\r\n\nCBUS:: ----Connection Change---- %s \n", pCbus->connected ? "Connected" : "Disconnected" );
			if( pCbus->connected )
			{
				pCbus->initTimer = SiiOsTimerTotalElapsed();
			}
			else
			{
				//set the cbus to idle
				CBusResetToIdle();
			}

			SiiMhlCbRxConnectChange(pCbus->connected);
        }

    	// request received from peer to write into scratchpad
    	if ( SiiDrvCbusReqWrtGet() )
    	{
    		if( !(pCbus->miscFlags & FLAGS_SCRATCHPAD_BUSY) )
			{
//				DEBUG_PRINT( MSG_DBG, "\n\ngranting peer's request to write scratchpad!!\n");
				CBusGrtWrt();
				pCbus->miscFlags |= FLAGS_SCRATCHPAD_BUSY;
			}
			else
			{
				Printf("\r\n\nGot request from peer for scratchpad write, "
						"couldn't grant request as the scartchpad is busy!!\n" );
			}
		}

    	// scratchpad write notification received from peer
    	if ( SiiDrvCbusScratchpadWrtnGet() )
		{
    		pCbus->miscFlags &= ~FLAGS_SCRATCHPAD_BUSY;
			// send it to app layer
			SiiMhlCbRxScratchpadWritten();
		}

    	// request to write into peer's scratchpad is granted
    	if ( SiiDrvCbusGrtWrtGet() )
    	{
//    		DEBUG_PRINT( MSG_DBG, "\n\npeer sent grtWrt!!\n");
    		if( pCbus->miscFlags & FLAGS_SCRATCHPAD_BUSY )
			{
				pCbus->reqWrtTimer = 0;
				CBusWritePeersScratchpad();
			}
        }

#if (MHL_EXTRA == ENABLE)
    	// request to send over 3D information
    	if ( SiiDrvCbus3DReqGet() )
    	{
//    		DEBUG_PRINT( MSG_DBG, "\n\npeer sent 3D data request!!\n");
    		pCbus->serving3dVicReq = true;
    		CBusSend3DInfo();
        }
#endif

    }

    SiiDrvCbusIntrFlagSet();
    SiiDrvCbusInterruptStatusSet();
    return( result );
}

#if (MHL_EXTRA == ENABLE)
//------------------------------------------------------------------------------
// Function:    CBusSend3DInfo
// Description: send over 3D write bursts
// Parameters:  none
// Returns:     void
//------------------------------------------------------------------------------
static void CBusSend3DInfo()
{
	if( EDID_MHL_3D_SUPPORT )
	{
		if( pCbus->serving3dVicReq )
		{
			if ( pCbus->cur3dVicSeq ==  EDID_3D_VIC_TOT_ENT )
			{
				pCbus->serving3dVicReq = false;
				pCbus->serving3dDtdReq = true;
				pCbus->cur3dVicSeq = 0;
			}
			else
			{
				if ( pCbus->cur3dVicSeq ==  0 )
				{
					pCbus->ptr3dVic = (uint8_t*)g3DEdidTableVIC;
				}
				SiiMhlRxWritePeerScratchpad(0, MHL_WRITE_BURST_LENGTH, pCbus->ptr3dVic);
				pCbus->ptr3dVic += MHL_WRITE_BURST_LENGTH;
				pCbus->cur3dVicSeq++;
			}
		}
		if( pCbus->serving3dDtdReq )
		{
			if ( pCbus->cur3dDtdSeq == EDID_3D_DTD_TOT_ENT )
			{
				pCbus->serving3dDtdReq = false;
				pCbus->cur3dDtdSeq = 0;
			}
			else
			{
				if ( pCbus->cur3dDtdSeq ==  0 )
				{
					pCbus->ptr3dDtd = (uint8_t*)g3DEdidTableDTD;
				}
				SiiMhlRxWritePeerScratchpad(0, MHL_WRITE_BURST_LENGTH, pCbus->ptr3dDtd);
				pCbus->ptr3dDtd += MHL_WRITE_BURST_LENGTH;
				pCbus->cur3dDtdSeq++;
			}
		}
	}
	else
	{
		uint8_t	data[MHL_WRITE_BURST_LENGTH];
		if( pCbus->serving3dVicReq )
		{
			memcpy( &data, (uint8_t*)g3DEdidTableVIC, MHL_WRITE_BURST_LENGTH );
			data[3]=0x00;	// setting the TOT_ENT to 0x00
			data[5]=0x00;	// setting the TOT_ENT to 0x00
			SiiMhlRxWritePeerScratchpad(0, MHL_WRITE_BURST_LENGTH, &data[0]);
			pCbus->serving3dVicReq = false;
		}
		if( pCbus->serving3dDtdReq )
		{
			memcpy( &data, (uint8_t*)g3DEdidTableDTD, MHL_WRITE_BURST_LENGTH );
			data[3]=0x00;	// setting the TOT_ENT to 0x00
			data[5]=0x00;	// setting the TOT_ENT to 0x00
			SiiMhlRxWritePeerScratchpad(0, MHL_WRITE_BURST_LENGTH, &data[0]);
			pCbus->serving3dDtdReq = false;
		}
		else
		{
			pCbus->serving3dDtdReq = true;
		}
	}
}
#endif

//------------------------------------------------------------------------------
// Function:    CBusSendDcapRdyMsg
// Description: Send a msg to peer informing the devive capability registers are
//				ready to be read.
// Returns:     TRUE    - success
//              FALSE   - failure
//------------------------------------------------------------------------------

static bool_t CBusSendDcapRdyMsg (void)
{
	bool_t result = true;

	if( SiiMhlRxCbusConnected() )
	{
//		DEBUG_PRINT( MSG_DBG, ( "cbusSendDcapRdyMsg() Called!!\n"));
		pCbus->miscFlags |= FLAGS_SENT_DCAP_RDY;

		//send a msg to peer that the device capability registers are ready to be read.
		//set DCAP_RDY bit
		result = CBusSendDcapRdy();

		//set DCAP_CHG bit
		result = SiiMhlRxSendDevCapChange();
	}

	return result;
}

//------------------------------------------------------------------------------
// Function:    CBusSendMscCommand
// Description: sends general MHL commands
//------------------------------------------------------------------------------
static bool_t CBusSendMscCommand(uint8_t cmd)
{
	cbus_req_t 	req;
	bool_t		success = true;

	req.command = cmd;

	switch(cmd)
	{
		case MHL_GET_STATE:
		case MHL_GET_VENDOR_ID:
		case MHL_SET_HPD:
		case MHL_CLR_HPD:
		case MHL_GET_SC1_ERRORCODE:
		case MHL_GET_DDC_ERRORCODE:
		case MHL_GET_MSC_ERRORCODE:
		case MHL_GET_SC3_ERRORCODE:

			if( !(success = CBusWriteCommand(&req)) )
			{
				Printf("\r\nCouldn't send cmd: %02X to peer\n", cmd );
				return false;
			}
			break;

		default:
			Printf("\r\nInvalid command send request!!\n", cmd );
			success = false;
	}
	return success;
}

//------------------------------------------------------------------------------
// Function:    CBusSetInt
// Description: write peer's status registers
//				regOffset - peer's register offset
//				regBit - bit to be set
//------------------------------------------------------------------------------
static bool_t CBusSetInt ( uint8_t regOffset, uint8_t regBit )
{
	cbus_req_t req;

	req.command = MHL_SET_INT;
	req.offsetData = regOffset;
	req.msgData[0] = regBit;

	if( SiiMhlRxCbusConnected() )
	{
		if( !(CBusWriteCommand(&req)) )
		{
			Printf("\r\nCouldn't send MHL_SET_INT to peer\n" );
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
// Function:    CBusSendDscrChange
//------------------------------------------------------------------------------
static bool_t CBusSendDscrChange (void)
{
	return ( CBusSetInt(0x00, BIT1) );
}

//------------------------------------------------------------------------------
// Function:    CBusReqWrt
//------------------------------------------------------------------------------
static bool_t CBusReqWrt (void)
{
	return ( CBusSetInt(0x00, BIT2) );
}

//------------------------------------------------------------------------------
// Function:    CBusGrtWrt
// Description:
//------------------------------------------------------------------------------
static bool_t CBusGrtWrt (void)
{
	return ( CBusSetInt(0x00, BIT3) );
}

//------------------------------------------------------------------------------
// Function:    CBusWriteStatus
// Description: write peer's status registers
// Parameters:  regOffset - peer's register offset
//				value - value to be written
//------------------------------------------------------------------------------
static bool_t CBusWriteStatus ( uint8_t regOffset, uint8_t value )
{
	cbus_req_t req;

	req.command = MHL_WRITE_STAT;
	req.offsetData = regOffset;
	req.msgData[0] = value;

	if( !(CBusWriteCommand(&req)) )
	{
		Printf("\r\nCouldn't send MHL_WRITE_STAT to peer\n" );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// Function:    CBusSendDcapRdy
//------------------------------------------------------------------------------
static bool_t CBusSendDcapRdy (void)
{
	return ( CBusWriteStatus(0x00, BIT0) );

}

static bool_t CBusTimerExpired(clock_time_t *pTimer, uint16_t waitTime)
{
	if( (*pTimer > 0) && ((SiiOsTimerTotalElapsed() - *pTimer ) > waitTime) )
	{
//		DEBUG_PRINT( MSG_DBG, "\n*pTimer = %02X SiiOsTimerTotalElapsed(): %02X\n", *pTimer, SiiOsTimerTotalElapsed());
		*pTimer = 0;
		return true;
	}
	else
	{
		return false;
	}

}

// This is to check all the Timers
static void CBusChkTimers ()
{
	CBusTimerExpired(&(pCbus->hpdWaitTimer), CBUS_HPD_WAIT_TIMER);
	CBusTimerExpired(&(pCbus->abortTimer), CBUS_ABORT_TIMER);
	CBusTimerExpired(&(pCbus->initTimer), CBUS_INIT_TIMER);
	CBusTimerExpired(&(pCbus->rcpeRcpkGapTimer), CBUS_RCPE_RCPK_GAP_TIMER);

	if( CBusTimerExpired(&(pCbus->reqWrtTimer), CBUS_WB_REQ_TIMER) )
	{
		CBusClearQueueEntry( MHL_WRITE_BURST );
		pCbus->miscFlags &= ~FLAGS_SCRATCHPAD_BUSY;
		Printf("\r\nGrtWrt interrupt did not come from peer within the timeout limit!! \n");
	}

	if( CBusTimerExpired(&(pCbus->waitTimer), CBUS_MSG_RSP_TIMER) || CBusTimerExpired(&(pCbus->rcpRapAbortTimer), CBUS_RCP_RCP_ABORT_TIMER))
	{
		pCbus->state |= CBUS_FAILED;
	}
}

