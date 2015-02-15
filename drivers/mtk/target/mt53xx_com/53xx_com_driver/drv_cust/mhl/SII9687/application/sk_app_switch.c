//***************************************************************************
//! @file   sk_app_switch.c
//! @brief  Wraps platform and device functions for the SWITCH component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_drv_device.h"
#include "si_drv_tx_simple.h"
#include "si_switch_component.h"


//------------------------------------------------------------------------------
// Current Video Width
//------------------------------------------------------------------------------
static uint16_t currrentVieoWidth;

//-------------------------------------------------------------------------------------------------
//! @brief      Display video format information for a given port.
//!
//! @param[in]  port        - Rx port index
//! @param[in]  pFormatData - pointer to video format structure
//-------------------------------------------------------------------------------------------------

static void VideoInputFormatDisplay (int_t port, SiiVideoFormat_t *pFormatData)
{
    int_t colDpt = 8;

     Printf("\r\n========================================================\n" );
     Printf("\r\nPort:          %d\n", port);
     Printf("\r\nVideo:         %s\n", pFormatData->isHdmi ? (pFormatData->isMhl ? "MHL" : "HDMI") : "DVI");
     Printf("\r\nVIC:           %d\n", pFormatData->vic);

    switch ( pFormatData->colorDepth )
    {
        case SiiCOLORDEPTH_10:
            colDpt = 10;
            break;
        case SiiCOLORDEPTH_12:
            colDpt = 12;
            break;
        case SiiCOLORDEPTH_16:
            colDpt = 16;
            break;
        default:
            colDpt = 8;
            break;
    }
     Printf("\r\nResolution:    %d x %d %s %d\n", (int)(pFormatData->hRes),
                               (int)pFormatData->vRes, (pFormatData->isInterlace) ? "i" : "p",
                               (int)pFormatData->vRate);
     Printf("\r\nPixel Repl.:   %d\n", pFormatData->pRepl );
     Printf("\r\nColor Depth:   %d bit \n", colDpt );
     Printf("\r\nTMDS Clock:    %d0 kHz \n", pFormatData->tmdsClock );

     currrentVieoWidth = (int)pFormatData->hRes;     
     Printf("\r\n========================================================\n" );

// Show info on LCD display
/*    SI_LCD_CLEAR();
    SI_LCD_PRINTF(0, "%dx%d%s %dHz", (int)(pFormatData->hRes),
            (int)pFormatData->vRes, (pFormatData->isInterlace) ? "i" : "p",
            (int)pFormatData->vRate);
    SI_LCD_PRINTF(1, "P%d %s %db %dr %dM", port, pFormatData->isHdmi ? (pFormatData->isMhl ? "M" : "H") : "D",
                        colDpt, pFormatData->pRepl, pFormatData->tmdsClock / 100);
                        */
}


//-------------------------------------------------------------------------------------------------
//! @brief      Shows port info on a display
//-------------------------------------------------------------------------------------------------

static void PortInfoShow(uint_t inst)
{
    SkAppSwitchPortUpdate(true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Video source disconnection handler
//-------------------------------------------------------------------------------------------------

static void OnCableDisconnection(void)
{
    // Avoid showing "No Input" message right after power up
    // to not obscure f/w version information
    if (SiiOsTimerTotalElapsed() > 2000)
    {
        SI_LCD_CLEAR();
      //  SI_LCD_PRINTF(0, "P%d: No Input", app.portSelect);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Display No Signal status if Tx is still muted
//-------------------------------------------------------------------------------------------------

static void CheckTxMuteDelayed(uint_t inst)
{
    uint16_t statusFlags = SiiDrvTxSimpleStatus();
    if ( statusFlags & SiiTX_MUTE_ON )
    {
        SI_LCD_CLEAR();
     //   SI_LCD_PRINTF(0, "P%d: No Signal", app.portSelect);
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Video off  handler
//-------------------------------------------------------------------------------------------------

static void OnVideoOff(void)
{
    bool_t isMpConnected = (SiiDrvSwitchStateGet( SiiSwitch_SELECTED_INPUT_CONNECT_STATE ) != 0);

    if (isMpConnected)
    {
        // Defer indication of disconnection when cable is connected.
        // Chances are it is a very short event being a result of switching from one active port to another.
        // Deferred call will check the Tx Mute status again and, if it is the case, will not show
        // the disconnection message at all
        SiiSchedTimerSet(3000, CheckTxMuteDelayed, 0, false, SI_SCHED_PRIORITY_NORMAL);
    }
    else
    {
        OnCableDisconnection();
    }
}

uint16_t SkAppGetCurrentVideoWidth(void)
{
    Printf("!!!!!!!!!!getcurrrentVieoWidth is %d\n",currrentVieoWidth);
    return currrentVieoWidth;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Do necessary steps and display video information each time resolution changes.
//!
//-------------------------------------------------------------------------------------------------

void SkAppSwitchOnVideoResolutionChange ( void )
{
    static SiiVideoFormat_t oldFormatData = {0};
    SiiVideoFormat_t newFormatData;

    if(!SiiDrvSwitchPortInfoGet(app.portSelect, true, &newFormatData))
    {
        // Don't do anything if video resolution detection is bad
        DEBUG_PRINT( MSG_ERR, "ERROR: Failed to detect video resolution at port %d!\n", app.portSelect);
        return;
    }

    // Do necessary steps if video timing has changed
    if ( memcmp( &oldFormatData, &newFormatData, sizeof( SiiVideoFormat_t )) != 0 )
    {
        memcpy( &oldFormatData, &newFormatData, sizeof( SiiVideoFormat_t ) );
    }
    // Display the resolution even if it has not changed.
    VideoInputFormatDisplay( app.portSelect, &newFormatData );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Do necessary steps and display video information each time TX mutes or unmutes.
//!
//-------------------------------------------------------------------------------------------------

void SkAppSwitchOnTxMuteChange( uint_t inst  )
{

    uint16_t statusFlags;

    Printf("\r\nSkAppSwitchOnTxMuteChange\n");


    statusFlags = SiiDrvTxSimpleStatus();
    if ( statusFlags & SiiTX_MUTE_CHANGE )
    {
        if ( statusFlags & SiiTX_MUTE_OFF )
        {
            SkAppSwitchOnVideoResolutionChange();
        }
        else if (statusFlags & SiiTX_MUTE_ON)
        {
            OnVideoOff();
        }

#   if (USE_INTERNAL_MUTE != 0)
        SiiDrvTxSimpleMute(( statusFlags & SiiTX_MUTE_ON ) != 0 );
#   endif
    }

}



//------------------------------------------------------------------------------
// Function:    SkAppSwitchPortUpdate
// Description: Calls the appropriate Silicon Image functions for switching
//              to a new port.
//------------------------------------------------------------------------------

void SkAppSwitchPortUpdate ( bool_t isForced )
{
    SiiVideoFormat_t vFmt;
    int_t  i;
    bool_t isSuccess = false;

    if (( (app.oldPort == app.portSelect) && !isForced) || ( app.portSelect >= SII_INPUT_PORT_COUNT ))
    {
        return;
    }

    // Attempt a few times to get roving port video information
    // This information is not available at any given time, so there is
    // a chance valid indicators aren't set at a random moment
    for (i = 0; i < 10; i++)
    {
        if(SiiDrvSwitchPortInfoGet( app.portSelect, true, &vFmt))
        {
            isSuccess = true;
            break;
        }
      //  SiiOsTimerWait(10);//liuqing
    }

    // Display the Res info for the port we're switching to.
    // If we were an actual DTV, we could use this information to prepare
    // our receiver hardware for the new format.

    if ( !app.autoSwitch )
    {
        if ( isSuccess )
        {
            Printf("\r\n*************************************************\n");
            Printf("\r\n**        Switch from port %d to port %d         **\n", app.oldPort, app.portSelect );
            Printf("\r\n**        InstaPort quick info:                **");

            // Display new port video info (if available)
            VideoInputFormatDisplay(app.portSelect, &vFmt);
        }
        else
        {
           Printf("\r\n*************************************************\n");
            Printf("\r\n** InstaPort info not available yet for port %d **\n", app.portSelect );
            Printf("\r\n*************************************************\n");

            OnVideoOff();
        }
    }
    else
    {
        // Display current port in auto mode
        Printf("\r\nAuto port: %d\n", (int)app.portSelect );
    }

    // Perform the actual switch...
    SiiSwitchSelectSource( app.portSelect );
    app.oldPort = app.portSelect;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Shows port info after a delay.
//!
//! @param[in]  delay - waiting time in ms before showing the port info
//-------------------------------------------------------------------------------------------------

void SkAppSwitchDeferredPortUpdate ( clock_time_t delay )
{
    SiiSchedTimerSet(delay, PortInfoShow, 0, false, SI_SCHED_PRIORITY_NORMAL);
}

//------------------------------------------------------------------------------
// Function:    SkAppTaskSwitch
// Description: Switch control task.
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskSwitch ( uint_t inst )
{
    uint8_t     ifFlags;
    uint16_t    statusFlags;
	
    Printf("\r\nSkAppTaskSwitch\n");


    // Check the application Switch interrupt flag to see if
    // anything happened that involves the switch

    if ( SiiDrvSwitchStatus( SiiSWITCH_PEEK_STATUS ) & SiiSWITCH_INT )
    {
        statusFlags = SiiDrvSwitchStatus( SiiSWITCH_INTFLAGS );

        if ( statusFlags & SiiSWITCH_CABLE_CHANGE )    // Connection change on main input port.
        {
            bool_t isMpConnected = (SiiDrvSwitchStateGet( SiiSwitch_SELECTED_INPUT_CONNECT_STATE ) != 0);

            if (app.tpgMode == APP_TPG_AUTO_ENABLE)
            {
                if (isMpConnected)
                {
                    app.tpgClockSrc = SI_TPG_CLK_MAIN_PIPE;
                    SkAppDemoEtpgModeEnable(false);
                }
                else
                {
                    // Enable internal video pattern generator, if main port is not connected
                    app.tpgClockSrc = SI_TPG_CLK_INTERNAL;
                    SkAppDemoEtpgModeEnable(true);
                }
            }
            else
            {
                if (!isMpConnected)
                {
                    OnCableDisconnection();
                }
            }

            if (app.tpgMode != APP_TPG_OFF)
            {
                // Always enable Tx when TPG or MP is on
                SiiDrvTxSimpleTmdsEnable( true );
            }
            else
            {
                // When TPG and MP are off, disable Tx
                SiiDrvTxSimpleTmdsEnable( isMpConnected );
            }
        }

        // If a port is marked for firmware trigger of a hot plug event (HPE), required for
        // CDC (HEC) and CBUS (MHL) ports, trigger it now.

        if ( statusFlags & SiiSWITCH_MP_HPE_TRIGGER )
        {
            Printf("\r\nTrigger HPE for MP\n" );
            SiiDrvSwitchPipeHpeTrigger( SiiPipeType_Main, false );
        }
        if ( statusFlags & SiiSWITCH_RP_HPE_TRIGGER )
        {
            Printf("\r\nTrigger HPE for RP\n" );
            SiiDrvSwitchPipeHpeTrigger( SiiPipeType_Background, false );
        }

        if ( statusFlags & SiiSWITCH_HPE_COMPLETE )
        {
            // The first HPE after power-up is considered the 'first pass' and
            // can be used to enable things that shouldn't be enabled until everything
            // in the chip is ready.

            if ( SiiDrvSwitchStateGet( SiiSwitch_FIRST_PASS ) != 0)
            {
            }
        }

        // Info frame change checks

        if ( SiiDrvSwitchIfInterruptStatusGet( &ifFlags ))
        {
            if ( ifFlags & (SiiSwitch_NO_AIF | SiiSwitch_NO_AVI | SiiSwitch_NO_VSI))
            {
                Printf("\r\nInfoFrames Missing: %s%s%s\n",
                          (ifFlags & SiiSwitch_NO_AIF) ? "AIF " : "",
                          (ifFlags & SiiSwitch_NO_AVI) ? "AVI " : "",
                          (ifFlags & SiiSwitch_NO_VSI) ? "VSI ": "");
            }
            if ( ifFlags & (SiiSwitch_NEW_AIF | SiiSwitch_NEW_AVI | SiiSwitch_NEW_VSI))
            {
                Printf("\r\nInfoFrames Changed: %s%s%s\n",
                          (ifFlags & SiiSwitch_NEW_AIF) ? "AIF " : "",
                          (ifFlags & SiiSwitch_NEW_AVI) ? "AVI " : "",
                          (ifFlags & SiiSwitch_NEW_VSI) ? "VSI ": "");

                // Update A/V info displayed
                PortInfoShow(0);
            }
        }

    }
}



