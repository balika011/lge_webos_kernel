//***************************************************************************
//!file     sk_app_ipv.c
//!brief    Application InstaPrevue control
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#include "si_osal_timer.h"
#include "sk_app_ipv.h"
#include "si_drv_ipv.h"
#include "sk_application.h"

//-------------------------------------------------------------------------------------------------
//! @brief      Move selection to the next or previous IPV window.
//!
//! @param[in]  isForward - select next input (true) or previous input (false)
//-------------------------------------------------------------------------------------------------

void SkAppIpvCycleInstaPrevueWindows ( bool_t isForward )
{

    // If not enabled, enable InstaPrevue
    if ( !app.ipvEnabled )
    {
        app.ipvHighlightWindow = 0;
        if ( SiiIpvEnable( false ))  // In case PIP was enabled
        {
            SiiIpvModeSet( app.ipvMode );
            SiiIpvThemeSet( app.ipvTheme );
            SiiIpvEnable( true );   // Don't need to check return value here
            SiiOsTimerWait(10);
            app.ipvEnabled = true;
            app.ipvHighlightWindow = SiiIpvHighlightWindow( app.ipvHighlightWindow, isForward );
        }
    }
    // Otherwise, cycle through the highlight window selection
    else
    {
        if (isForward)
        {
            app.ipvHighlightWindow++;
            if ( app.ipvHighlightWindow >= IPV_WINDOW_COUNT )
            {
                app.ipvHighlightWindow = 0;
            }
        }
        else
        {
            if ( app.ipvHighlightWindow == 0 )
            {
                app.ipvHighlightWindow = IPV_WINDOW_COUNT;
            }
            else
            {
                app.ipvHighlightWindow--;
            }
        }
        app.ipvHighlightWindow = SiiIpvHighlightWindow( app.ipvHighlightWindow, isForward );
    }

    SkAppTaskIpv(0);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable PIP wrapper.
//-------------------------------------------------------------------------------------------------

bool_t SkAppIpvPipModeEnable ( uint8_t pipSource )
{
    if ( pipSource == SiiDrvSwitchSourceGet())
    {
        DEBUG_PRINT( MSG_ERR, " Cannot use the main pipe source!\n" );
        return( false );
    }

    SiiDrvSwitchSubPortSelect( pipSource );
    SiiIpvPipModeEnable();
    SkAppTaskIpv(0);
    return true;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Close all IPV windows including the PIP window.
//-------------------------------------------------------------------------------------------------

void SkAppIpvDisableInstaPrevue ( void )
{
    if (app.ipvEnabled)
    {
        app.ipvEnabled = false;
        if ( SiiIpvEnable( false ))
        {
            // If PIP previously enabled, restore it
            if ( app.ipvPipEnabled )
            {
                // PIP source cannot be the same as main pipe.
                if ( app.pipSource == app.portSelect )
                {
                    app.pipSource = SiiSwitchGetNextPort( app.portSelect, true );
                }
                SkAppIpvPipModeEnable( app.pipSource );
                SiiIpvEnable( true );
            }
        }
    }
    else
    {
        // If PIP previously enabled, close it
        if ( app.ipvPipEnabled )
        {
            SiiIpvEnable( false );
            app.ipvPipEnabled = false;
        }
    }

    // Disable roving if requested
    SiiDrvSwitchRovingEnable(app.rovingEnabled);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Initialize IPV Task
//-------------------------------------------------------------------------------------------------
bool_t SkAppTaskIpvInitialize ( void )
{
    Printf("\r\n SkAppTaskIpvInitialize.\n");
	
    SiiIpvInitialize();

    // Initialize the component window data
    SiiIpvBorderColorsLoad( siiIpvBorderColorsCrCbY );
    SiiIpvAlphaLoad( siiIpvAlphaMax, siiIpvAlphaStep, SII_IPV_ALPHA_STEP_MS );

    SiiDrvIpvEnableMaskSet( 0x07 );

    // Set the default theme (window locations and size) into the hardware.
    SiiIpvModeSet( app.rovingEnabled ? SiiIpvModeALL : SiiIpvModeSELECT);

    // Enable IPV highlighting feature to display current window selection
    SiiIpvHighlightEnable(true);

    return( true );
}


//-------------------------------------------------------------------------------------------------
//! @brief      IPV Task function. Services IPV component state machine.
//-------------------------------------------------------------------------------------------------
void SkAppTaskIpv ( uint_t inst )
{
    // Call IPV handler with the number of milliseconds since the last call
    if (SiiIpvHandler( SkTimeDiffMs( app.ipvTimeLastMs, SiiOsTimerTotalElapsed())))
    {
        // Schedule one more single call if rescheduling was requested
        SiiSchedTimerSet(50, SkAppTaskIpv, 0, false, SI_SCHED_PRIORITY_NORMAL);
    }
    app.ipvTimeLastMs = SiiOsTimerTotalElapsed();
}

//-------------------------------------------------------------------------------------------------
//! @brief      IPV Callback function. Retrieves video resolution data for current port.
//-------------------------------------------------------------------------------------------------


