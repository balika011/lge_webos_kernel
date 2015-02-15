//***************************************************************************
//!file     sk_app_infrared.c
//!brief    Processes RC commands.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_drv_arc.h"


//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function:    SkAppIrGetSingleDigit
// Description: Waits for user to press digit key within passed range.
// Parameters:  rangeEnd - maximum value (0 is minimum)
// Returns:     Valid digit pressed by user.
//------------------------------------------------------------------------------

uint8_t SkAppIrGetSingleDigit ( uint8_t rangeEnd )
{
#if 0
    uint16_t    rcKey;
    uint8_t     digitValue = 0xFF;
    bool_t      done = false;

    while ( !done )
    {
        rcKey = SiiPlatformInfraRedGetKey();
        switch ( rcKey )
        {
            case RC5_0:
            case RC5_1:
            case RC5_2:
            case RC5_3:
            case RC5_4:
            case RC5_5:
            case RC5_6:
            case RC5_7:
            case RC5_8:
            case RC5_9:
                digitValue = rcKey;
                if ( digitValue > rangeEnd )
                    break;

                Printf("\r\n%d \n", (int)digitValue );
                done = true;
                break;
            case RC5_BUFFER_EMPTY:
                break;
        }

        // Joystick activity makes the cycle to break with zero output
        if (SiiPlatformJstkStateGet() != JNAV_NONE)
        {
            return 0;
        }
    }

    return( digitValue );
    #else
    return( 1 );
    #endif 
}



//------------------------------------------------------------------------------
// Function:    SkAppIrRemoteReturnToTV
// Description: 
//------------------------------------------------------------------------------

void SkAppIrRemoteReturnToTV ( void )
{
    app.modeSelect = APP_MODE_TV;
    Printf("\r\n\nTV Mode -- Press MENU key to select new mode\n" );
    SkAppSwitchPortUpdate(true);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set application mode based on Main menu selection.
//!
//! @param[in]  selection - menu selection code
//-------------------------------------------------------------------------------------------------
#if 0
static void AppModeSet(int_t selection)
{
    if (selection == 0)
    {
        app.modeSelect = APP_MODE_TV;
        // Show current port video info (and wipe out menu text from LCD screen)
        SkAppSwitchPortUpdate(true);
    }
    else
    {
        app.modeSelect = selection + 1; // to skip APP_MODE_MAIN_MENU
    }
}

static void MainMenuPrint(void)
{
    Printf("\r\n\n Main Menu\n" );
    DEBUG_PRINT( MSG_ALWAYS,   " ----------------\n" );
    Printf("\r\n0 - Exit to TV Mode\n" );
    Printf("\r\n1 - Demo Menu\n" );
    Printf("\r\n2 - Setup Menu\n" );
    Printf("\r\n\n Mode (0-%d): ", 3 );

    // Fill up LCD menu
    app.lcdMenu[0] = "TV Mode";
    app.lcdMenu[1] = "Demo Menu";
    app.lcdMenu[2] = "Setup Menu";
    SkAppLcdMenuListSet(app.lcdMenu, 3);
}
//------------------------------------------------------------------------------
// Function:    IrRemoteKeyParser
// Description: Parse special 'superuser' keys for the IR task.
//------------------------------------------------------------------------------

static bool_t IrRemoteKeyParser ( RC5Commands_t *pKey )
{
    bool_t  usedKey = false;

    switch ( *pKey )
    {
#if (FPGA_BUILD == ENABLE)
        case RC5_STANDBY:
        case RC5_SYSTEM_STANDBY:
            if ( app.powerState == APP_POWERSTATUS_ON )
            {
                app.powerState = APP_POWERSTATUS_ON_TO_STANDBY;
            }
            else if (( app.powerState == APP_POWERSTATUS_STANDBY ) || ( app.powerState == APP_POWERSTATUS_INITIAL ))
            {
                app.powerState = APP_POWERSTATUS_STANDBY_TO_ON;
            }
            usedKey = true;
            break;
#endif

        case RC5_MENU:

           // Application Mode select only available in TV Mode (Main App Mode)

           if ( app.modeSelect == APP_MODE_TV )
           {
               MainMenuPrint();
               app.modeSelect = APP_MODE_MAIN_MENU;
           }
           usedKey = true;
           break;

        case RC5_0:
        case RC5_1:
        case RC5_2:
            if ( app.modeSelect == APP_MODE_MAIN_MENU )
            {
                AppModeSet(*pKey - RC5_0);
                *pKey = RC5_MENU;
                // Leave the key unused so that it gets passed to the new mode to display its menu
            }
            break;

        case RC5_ENTER:
        case RC5_INCREMENT: // OK
            if ( app.modeSelect == APP_MODE_MAIN_MENU )
            {
                AppModeSet(SkAppLcdMenuSelectionGet());
                *pKey = RC5_MENU;
                // Leave the key unused so that it gets passed to the new mode to display its menu
            }
            break;

        case RC5_CONTRAST_DOWN: // Down
            if ( app.modeSelect != APP_MODE_TV )
            {
                SkAppLcdMenuSelectionMove(false);
            }
            break;

        case RC5_CONTRAST_UP:  // Up
            if ( app.modeSelect != APP_MODE_TV )
            {
                SkAppLcdMenuSelectionMove(true);
            }
            break;

       default:
           break;
    }

    return( usedKey );
}
#endif
//------------------------------------------------------------------------------
// Function:    SkAppTaskIrRemote
// Description: IR Remote Control task.
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskIrRemote ( uint_t inst  )
{
/*
    RC5Commands_t   key;
    uint16_t        irKey;
    int_t           rc5Address;

    // fullAccess - true if allowed to respond to all normal keys
    //              false if only allowed to respond to power keys
    bool_t fullAccess = (app.powerState == APP_POWERSTATUS_ON);

    if ( app.irEnable )
    {
        irKey = SiiPlatformInfraRedGetKey();
    }
    else
    {
        irKey = RC5_BUFFER_EMPTY;
    }


    if ( irKey != RC5_BUFFER_EMPTY )
    {
        rc5Address = (irKey & RC5_ADDRESS_FIELD) >> 6;
//        DEBUG_PRINT( MSG_DBG, "IR KEY: %04X(%d) DEV TYPE: %d\n", irKey, irKey, rc5Address );
        irKey &= RC5_COMMAND_FIELD;     // We don't care if it's a TV or VCR
    }
    else
    {
        JoystickState_t joyState;

        // Check out commands from Joystick and translate them into equivalent RC control events
        // when nothing comes from IR
        joyState = SiiPlatformJstkStateGet();
        switch (joyState)
        {
            default:
                break;
            case JNAV_SELECT:
                if (app.modeSelect == APP_MODE_TV)
                {
                    // Select enters Menu in TV mode
                    irKey = RC5_MENU;
                }
                else
                {
                    // Select works as Enter in all other modes
                    irKey = RC5_ENTER;
                }
                break;
            case JNAV_LEFT:
                if (app.modeSelect == APP_MODE_TV)
                {
                    irKey = RC5_DECREMENT;
                }
                else
                {
                    irKey = RC5_0; // Level up in menus
                }
                break;
            case JNAV_RIGHT:
                irKey = RC5_INCREMENT;
                break;
            case JNAV_UP:
                irKey = RC5_CONTRAST_UP;
                break;
            case JNAV_DOWN:
                irKey = RC5_CONTRAST_DOWN;
                break;
        }
    }

    if ( irKey != RC5_BUFFER_EMPTY)
    {
        key = (RC5Commands_t)irKey;

        for ( ;; )
        {
            if ( IrRemoteKeyParser( &key ))
            {
                break;
            }

            // Run the appropriate key parser

            switch ( app.modeSelect )
            {
                case APP_MODE_TV:
                    SkAppRcTvMode( key, fullAccess );
                    break;

                case APP_MODE_DEMO_MENU:
                    SkAppRcDemoMenu( key, fullAccess );
                    break;

                case APP_MODE_SETUP_MENU:
                    SkAppRcSetupMenu( key, fullAccess );
                    break;

                default:
                    break;
            }

            break;  // Always break if you get this far...
        }
    }
    */
}

//-------------------------------------------------------------------------------------------------
//! @brief      Stub function for SiIMon IR simulation callback.
//-------------------------------------------------------------------------------------------------

void SiiMonCbIrCmdSet(uint8_t irKey, uint8_t irAddr)
{
}
