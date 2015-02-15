//**************************************************************************
//!file     si_drv_tx_simple.c
//!brief    SiI9687 Tx Simple Driver API functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_device.h"
#include "si_scheduler.h"
#include "si_drv_tx_simple.h"
#include "si_drv_tx_simple_internal.h"
#include "si_drv_switch.h"



//------------------------------------------------------------------------------
//  Module Data
//------------------------------------------------------------------------------

static TxSimpleInstanceData_t txSimpleInstance[SII_NUM_SWITCH] =
{
{
    0,                          // structVersion
    SII_SUCCESS,             // lastResultCode Contains the result of the last API function called
    0,                          // statusFlags
}
};
static TxSimpleInstanceData_t *pTxSimple = &txSimpleInstance[0];

//------------------------------------------------------------------------------
// Function:    SiiDrvTxSimpleInitialize
// Description:
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvTxSimpleInitialize ( void )
{
    // Enable the interrupts for this driver
    Printf("\r\n SiiDrvTxSimpleInitialize...\n");

    SiiRegBitsSet( REG_PA_INTR7_MASK, BIT_MP_MUTE_CONDITION | BIT_MP_UNMUTE_CONDITION, SET_BITS );

    // Enable global TMDS control
    // Individual TX0 TMDS outputs are disabled
    SiiRegModify(REG_TMDST_CTRL1, 0x01, 0x01);

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvTxSimpleStatus
// Description: Returns a status flag word containing TX0-specific
//              information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the Tx0 Component
//------------------------------------------------------------------------------

uint16_t SiiDrvTxSimpleStatus ( void )
{
    uint16_t statusFlags = pTxSimple->statusFlags;

    pTxSimple->statusFlags &= ~SiiTX_MUTE_CHANGE;  // INT flag only valid first time it is read.
    return( statusFlags );
}
//------------------------------------------------------------------------------
// Function:    SiiDrvTxSimpleMute
// Description: Mute the device output.  The OEM can replace the guts of this
//              function with a call to the main mute function of the SOC
//              if desired.
//------------------------------------------------------------------------------

void SiiDrvTxSimpleMute ( bool_t doMute )
{
    if ( doMute )
    {
        /*do not mute audio, loss of AV unmute packet may happen and TV could not recover*/
        SiiRegModify( REG_PAUTH_MISC_CTRL0, BIT_VIDEO_MUTE_SYNC , SET_BITS );
    }
    else
    {
        /* clear Audio mute since it is active by default*/
        SiiRegModify( REG_PAUTH_MISC_CTRL0, BIT_VIDEO_MUTE_SYNC | BIT_AUDIO_MUTE_SYNC, CLEAR_BITS );
    }
}

//------------------------------------------------------------------------------
// Function:    SiiDrvTxSimpleTmdsEnable
// Description: Enables or disables the TMDS output engine.
//------------------------------------------------------------------------------

void SiiDrvTxSimpleTmdsEnable ( bool_t enable )
{
    uint8_t enableBits = MSK_TMDS_EN; // Global TMDS bit must be always asserted
                                      // as it controls TX TMDS outputs as well
    enableBits |= ( enable ) ? MSK_TMDS_OE : 0;

    SiiRegWrite( REG_TMDST_CTRL1, enableBits );
}

//------------------------------------------------------------------------------
// Function:    UpdateTxSwingCtrl
// Description: Control the TX0 output swing based format resolution
//------------------------------------------------------------------------------
// TODO: tmds swing/term
static void UpdateTxSwingCtrl ( void )
{
    //SiiVideoFormat_t format;

    //SiiDrvSwitchPortInfoGet( ... );         // Get input format from Switch driver

    /*
	if ( format.vRes >= 1080 && format.hRes >= 1920)
    	SiiRegWrite( REG_TX_CTRL, 0x88 );
	else
    	SiiRegWrite( REG_TX_CTRL, 0x66 );
    */

	SiiRegWrite( REG_TX_CTRL, 0x0D );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvTxSimpleProcessInterrupts
// Description:
//------------------------------------------------------------------------------

void SiiDrvTxSimpleProcessInterrupts ( void )
{
    uint8_t     intStatus;
    bool_t      outputMuted=1;

    /* Check mute on/off interrupt  */
//	Printf("\r\nSiiDrvTxSimpleProcessInterrupts\n");
    intStatus = SiiRegRead( REG_PA_INTR7 ) & (BIT_MP_MUTE_CONDITION | BIT_MP_UNMUTE_CONDITION);
//	Printf("\r\nSiiDrvTxSimpleProcessInterrupts intStatus=%02x \n",intStatus);

    if ( intStatus )
    {
        SiiRegWrite( REG_PA_INTR7, intStatus );  // Clear these interrupts only
        if (intStatus & BIT_MP_MUTE_CONDITION)
        {
            Printf("\r\nMute MP\n" );
            //outputMuted = true;
            //pTxSimple->statusFlags |= SiiTX_MUTE_ON;
            //pTxSimple->statusFlags &= ~SiiTX_MUTE_OFF;
        }
        if (intStatus & BIT_MP_UNMUTE_CONDITION)
        {
            Printf("\r\nUnmute MP\n" );
            //outputMuted = false;
            //pTxSimple->statusFlags |= SiiTX_MUTE_OFF;
            //pTxSimple->statusFlags &= ~SiiTX_MUTE_ON;

            UpdateTxSwingCtrl();    // Update TX swing if color depth has changed.
        }
#if (USE_INTERNAL_MUTE == 1)
        {
            static bool_t   oldMute = false;

            if ( oldMute != outputMuted  )
            {
                SiiDrvTxSimpleMute( outputMuted  );
            }
            oldMute = outputMuted;
        }
#endif

        pTxSimple->statusFlags |= SiiTX_MUTE_CHANGE;
        SiiSchedSendSignal(SI_EVENT_ID_TX_INT, 0);
    }
}

