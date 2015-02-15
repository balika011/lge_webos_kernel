//***************************************************************************
//!file     sk_app_tv.c
//!brief    Starter Kit firmware TV emulation module
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_drv_device.h"
#include "si_switch_component.h"
#include "si_gpio.h"


//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

static uint16_t l_autoSwitchDelaysMs [8] =
{
    5000, 10000, 15000, 20000, 30000, 45000, 60000, 3000
};

//------------------------------------------------------------------------------
// Function:    SkAppRcTvMode
// Description: Parse remote control keys for the TV application.
// Parameters:  fullAccess - true if allowed to respond to all normal keys
//                           false if only allowed to respond to power keys
// Returns:     none
//------------------------------------------------------------------------------

void SkAppRcTvMode ( RC5Commands_t key, bool_t fullAccess )
{
    bool_t  isForward = true;

    while ( fullAccess )
    {
        // Send appropriate keys over CBUS

        if ( SkAppCbusRemoteControlHandler( key ))
        {
            break;
        }

        // Now handle TV action keys

        switch ( key )
        {
            case RC5_0:
                break;

            case RC5_1:
            case RC5_2:
#if 0// (INC_9617 == 0)
            case RC5_3:
            case RC5_4:
#endif
                app.portSelect = key - RC5_1;
                break;

            case RC5_6:
                break;

            case RC5_7:
                app.tpgMode = APP_TPG_ON_AUTO_FMT;
                SkAppDemoEtpgModeEnable(true);
                SkAppLcdMessageBox("VPG ON", "Auto Format");
                SkAppSwitchDeferredPortUpdate(1000);
                break;

            case RC5_8:
                if (app.tpgMode != APP_TPG_OFF)
                {
                    // Change pattern every time the button has been pressed
                    app.isTpgActiveDemo = false; // Disable auto mode if manual pattern selector is touched
                    if (app.tpgPattern < SI_ETPG_PATTERN_COLORBAR)
                    {
                        app.tpgPattern++;
                    }
                    else
                    {
                        app.tpgPattern = SI_ETPG_PATTERN_RED;
                    }
                    SiiDrvExtTpgPatternSet(app.tpgPattern);
                    SkAppLcdMessageBox("VPG Pattern", "Changed");
                    SkAppSwitchDeferredPortUpdate(1000);
                }
                break;

            case RC5_9:
                SkAppDemoEtpgModeEnable(false);
                app.tpgMode = APP_TPG_OFF;
                SkAppLcdMessageBox("VPG OFF", "");
                SkAppSwitchDeferredPortUpdate(1000);
                break;

            case RC5_SELECT: //QUIT
#if INC_IPV
                if (!app.ipvPipEnabled && !app.ipvEnabled)
                {
                    // Cancel everything
                    // Switch to input 0 or any available, if port 0 is not connected
                    app.portSelect = SiiSwitchGetNextPort( 1, false );
                }

                if (app.ipvPipEnabled)
                {
                    SiiIpvEnable(false);
                    app.ipvPipEnabled = false;
                }

                if (app.ipvEnabled)
                {
                    SkAppIpvDisableInstaPrevue();
                }
#endif
                Printf("\r\nQUIT\n" );
                break;

            case RC5_DECREMENT: // Left
                isForward = false;

            case RC5_INCREMENT: // Right
#if INC_IPV
                if ( app.ipvEnabled )
                {
                    // Change selection
                    SkAppIpvCycleInstaPrevueWindows(isForward);
                }
                else if (app.ipvPipEnabled)
                {
                    // Switch to PIP source
                    app.portSelect = app.pipSource;
                    SkAppIpvDisableInstaPrevue();
                }
                else
#endif
                {
                    app.portSelect = SiiSwitchGetNextPort( app.portSelect, isForward );
                    Printf("\r\nSRC: %d\n", app.portSelect);
                }
                break;

            case RC5_CONTRAST_DOWN: // Down
                isForward = false;

            case RC5_CONTRAST_UP:  // Up
#if INC_IPV
                if ( app.ipvEnabled )
                {
                    // Change selection
                    SkAppIpvCycleInstaPrevueWindows(!isForward);
                }
                else if (app.ipvPipEnabled)
                {
                    // Change PIP source
                    app.pipSource = SiiSwitchGetNextPort( app.pipSource, isForward );
                    // PIP source cannot be the same as main pipe.
                    if ( app.pipSource == app.portSelect )
                    {
                        app.pipSource = SiiSwitchGetNextPort( app.pipSource, isForward );
                    }
                    SkAppIpvPipModeEnable( app.pipSource );
                }
                else
#endif
                {
                    app.portSelect = SiiSwitchGetNextPort( app.portSelect, isForward );
                    Printf("\r\nSRC: %d\n", app.portSelect);
                }
                break;

            case RC5_PIP_MODE:
#if INC_IPV
                if ( app.ipvPipEnabled )
                {
                    SiiIpvModeSet( SiiIpvModeALL );  // Go back to roving mode (if were in PIP mode)
                    SiiIpvEnable( false );
                    app.ipvPipEnabled = false;
                    app.ipvEnabled = false;
                    SkAppLcdMessageBox("PIP OFF", "");
                }
                else
#endif
                {
#if INC_IPV
                    if ( app.ipvEnabled )
                    {
                        // Switch to port highlighted in InstaPrevue
                        app.pipSource = SiiIpvHighlightPortGet( app.portSelect, app.ipvHighlightWindow );
                    }
#endif
                    // PIP source cannot be the same as main pipe.
                    if ( app.pipSource == app.portSelect )
                    {
                        app.pipSource = SiiSwitchGetNextPort( app.pipSource, true );
                    }
#if INC_IPV
                    if ( SkAppIpvPipModeEnable( app.pipSource ))
                    {
                        app.ipvPipEnabled = true;
                        app.ipvEnabled = false;
                    }
                    SkAppLcdMessageBox("PIP ON", "");
#endif
                }
                SkAppSwitchDeferredPortUpdate(1500);
                break;
#if INC_IPV
            case RC5_VOL_DOWN:
                SkAppIpvDisableInstaPrevue();

                break;

            case RC5_PRG_UP:
                if ( app.ipvEnabled && !app.ipvPipEnabled )
                {
                    app.ipvMode = (app.ipvMode == SiiIpvModeACTIVE) ? SiiIpvModeALL : SiiIpvModeACTIVE;
                    if ( SiiIpvEnable( false ))
                    {
                        SiiIpvModeSet( app.ipvMode );
                        SiiIpvEnable( true );
                        SkAppTaskIpv(0);
                    }
                }
                break;

            case RC5_PRG_DOWN:
                if ( app.ipvEnabled && !app.ipvPipEnabled )
                {
                    if ( SiiIpvEnable( false ))
                    {
                        app.ipvTheme++;
                        if ( app.ipvTheme >= SiiIpvPIP )
                        {
                            app.ipvTheme = 0;
                        }
                        SiiIpvThemeSet( app.ipvTheme );
                        SiiIpvEnable( true );
                        app.ipvHighlightWindow = SiiIpvHighlightWindow( app.ipvHighlightWindow, true );
                        SkAppTaskIpv(0);
                    }
                }
                break;
#endif
            case RC5_INPUT:
#if INC_IPV
                // When we allow InstaPrevue, the INPUT key shows the preview windows
                if (app.ipvAllowed)
                {
                    SkAppLcdMessageBox("InstaPrevue ON", "");
                    SkAppIpvCycleInstaPrevueWindows(true);
                    SkAppSwitchDeferredPortUpdate(1000);
                }
                else
#endif
                {
                    // Rotate through the available ports
                    app.portSelect = SiiSwitchGetNextPort(app.portSelect, true);
                }
                break;

#if INC_IPV
            case RC5_ENTER:
            case RC5_OK:
                if ( app.ipvPipEnabled )
                {
                    app.portSelect = app.pipSource;
                    app.pipSource = app.portSelect;
                    SkAppIpvDisableInstaPrevue();
                }
                else if ( app.ipvEnabled )
                {
                    // Switch to port highlighted in InstaPrevue
                    app.portSelect = SiiIpvHighlightPortGet( app.portSelect, app.ipvHighlightWindow );
                    SkAppIpvDisableInstaPrevue();
                }
                break;
#endif

            case RC5_CC:
                SkAppIrRemoteReturnToTV();
                Printf("\r\n------\n");
                Printf("\r\n1-4   - Select HDMI/MHL Input Port 1 - 4\n" );
                Printf("\r\n7     - Enable VPG\n" );
                Printf("\r\n8     - Change VPG pattern\n" );
                Printf("\r\n9     - Disable VPG\n" );

                Printf("\r\nMENU   - Open Text based Menu\n" );
                Printf("\r\nINFO   - Print menu items (in sub menus)\n" );
                Printf("\r\nQUIT   - Cancel mode\n" );
                Printf("\r\nINPUT  - Select Input via InstaPrevue menu\n" );
                Printf("\r\nENTER  - Change to selected Input\n" );
                Printf("\r\nPIP    - PIP with selected source ON/OFF\n" );
                Printf("\r\nVOL-   - InstaPrevue menu off\n" );
                Printf("\r\nCHAN+  - InstaPrevue ALL/ACTIVE\n" );
                Printf("\r\nCHAN-  - InstaPrevue layout change\n" );
                Printf("\r\nLT/RT  - Change main source / InstaPrevue selection / PIP source\n" );
                Printf("\r\nUP/DWN - Change main source / InstaPrevue selection / PIP source\n" );
                Printf("\r\nCC     - Print this help\n" );
                break;

            default:
                break;
        }
        break;  // Always break;
    }
}


//------------------------------------------------------------------------------
// Function:    SkAppPortAutoSwitchInit
// Description: Initialize automatic port switching mode
//------------------------------------------------------------------------------

void SkAppPortAutoSwitchInit(void)
{
    SiiOsTimerSet(&app.AutoSwTimer, l_autoSwitchDelaysMs[app.portSelect]);
}

//------------------------------------------------------------------------------
// Function:    SkAppTvTask
// Description: TV emulation control
//------------------------------------------------------------------------------

void SkAppTvTask ( void )
{


    /****************************************************************************************/
    /* Display Port Processor State                                                         */
    /****************************************************************************************/

  //  SkAppInstaPortLedUpdate( SiiDrvDeviceInfo(SiiDEV_SELECTED_PORT_BITFIELD));

    /****************************************************************************************/
    /* Rotary Switch handling                                                               */
    /****************************************************************************************/

    if ( app.autoSwitch )
    {
        uint8_t tempPort = 1;//SiiPlatformReadRotary( true );
        if ( SiiOsTimerExpired(app.AutoSwTimer))
        {
            SiiOsTimerSet(&app.AutoSwTimer, l_autoSwitchDelaysMs[ tempPort ]);
            app.portSelect = SiiSwitchGetNextPort( app.portSelect, true );

        }
    }
    /*
    else if ( app.rotarySwitch != RSW_NO_CHANGE )
    {
        app.lastRotarySwitch = app.rotarySwitch;

        //  Switch positions 0-3, select new active port on SiI9687
        if (app.rotarySwitch < SII_INPUT_PORT_COUNT)
        {
#if INC_IPV
            if ( app.ipvEnabled )
            {
                // Change IPV window selection
                SkAppIpvCycleInstaPrevueWindows(true);
            }
            else
#endif
            {
                // Select new input port
                app.portSelect = app.rotarySwitch;
            }
        }
    }
    */

    /****************************************************************************************/
    /* Port Selection                                                                       */
    /****************************************************************************************/

    SkAppSwitchPortUpdate(false);    // Check for port updates

    /****************************************************************************************/
    /* ETPG auto pattern change mode                                                        */
    /****************************************************************************************/

    if ((app.tpgMode != APP_TPG_OFF) && app.isTpgActiveDemo && (app.tpgPatternChangeTime++ > 50 /* 5 sec*/))
    {
        app.tpgPatternChangeTime = 0;
        SiiDrvExtTpgPatternSet(app.tpgPattern);
        if (app.tpgPattern == SI_ETPG_PATTERN_COLORBAR)
        {
            app.tpgPattern = 0;
        }
        else
        {
            app.tpgPattern++;
        }
    }
}

uint8_t  SkAppPortAutoSwitchGet(void)//Jane add 0205
{
	 uint8_t tempValue;
	 tempValue = SiiDrvSwitchStateGet( SiiSwitch_INPUT_PORTS_CONNECT_STATE );
	//Printf("\n@@@@Jane: tempValue ==%x.\n", tempValue );
	return tempValue ;

    
}

