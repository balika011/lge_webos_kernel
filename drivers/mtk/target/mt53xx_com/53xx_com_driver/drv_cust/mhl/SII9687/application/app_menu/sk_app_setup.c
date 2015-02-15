//***************************************************************************
//!file     sk_app_setup.c
//!brief    Built-in Setup manager
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_eeprom.h"
#include "sk_application.h"

// Included here only because this is a 'special' app layer module

#include "si_drv_device.h"
#include "si_drv_internal.h"
#include "si_cbus_component.h"

static uint8_t  dispData[ EDID_TABLE_LEN];


//------------------------------------------------------------------------------
// Function:    SetupLevelOne
// Description: Handles commands on the second level of command options
// Returns:     TRUE if exit from setup, false if return to previous level
//------------------------------------------------------------------------------

static bool_t SetupLevelOne ( RC5Commands_t key )
{
    uint8_t     i;

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        // Quit upon entering
        // since this menu is not mirrored on LCD menu
        key = RC5_0;
    }

    switch ( key )
    {
        case RC5_SELECT:
            Printf("\r\n\nSetup Menu Two\n" );
            Printf("\r\n--------------\n" );
            Printf("\r\nINFO - Display this menu\n" );
            Printf("\r\n0 - Return to main Setup Menu\n" );
            Printf("\r\n1 - Display EEPROM EDID #\n" );
            Printf("\r\n2 - Display Port EDID SRAM\n" );
            Printf("\r\n3 - LCD Display Print Test\n" );
            Printf("\r\n4 - LCD Display Text Moving Test\n" );
            break;

        case RC5_0:
            app.setupMenuLevel = 0;
            Printf("\r\n\n Setup Level -- Press MENU key to select Setup Menu\n" );
            SkAppSetupMenuPrint();
            break;
        case RC5_1:
            Printf("\r\n\n EEPROM EDID (0-%d): ", SII_INPUT_PORT_COUNT - 1 );
            i = SkAppIrGetSingleDigit( SII_INPUT_PORT_COUNT - 1 );
            if ( SkAppEdidRead( EDID0_EEPROM + i, dispData ))
            {
                SkAppDisplayData( MSG_ALWAYS, dispData, 256 );
            }
            else
            {
                Printf("\r\nError reading EEPROM\n" );
            }
            break;

        case RC5_2:
            {
                uint8_t portIndex;
                Printf("\r\n\n Display SRAM for Port (0-%d): ", SII_INPUT_PORT_COUNT - 1 );
                portIndex = SkAppIrGetSingleDigit( SII_INPUT_PORT_COUNT - 1 );
                if ( SiiEdidRead( EDID_RAM_0 + portIndex, dispData ))
                {
                    SkAppDisplayData( MSG_ALWAYS, dispData, 256 );
                }
                else
                {
                    Printf("\r\nError reading SRAM for port %d\n", portIndex );
                }
            }
            break;
        case RC5_3:
            i = 1;
          //  SI_LCD_INIT(1);
            while (i)
            {
                Printf("\r\n\n Enter 0 to quit or 1 or 2 to continue\n" );
                i = SkAppIrGetSingleDigit(2);
                SI_LCD_PRINT("SIMG 2011", i-1);
                Printf("\r\n\n LCD Test\n" );
            }
            break;
        case RC5_4:
            i = 1;
            while (i)
            {
                Printf("\r\n\n Enter 0 to quit, 1 - left and 2 - right move\n" );
                i = SkAppIrGetSingleDigit(2);
                switch (i)
                {
                    case 1: SI_LCD_TEXT_MOVE(-1); break;
                    case 2: SI_LCD_TEXT_MOVE(1); break;
                }
            }
            break;
        default:
            break;
    }

    return( false );
}


#if 0
static void SetupLevelZeroPrintMenu(void)
{
    Printf("\r\n\nSetup Menu One\n" );
    Printf("\r\n--------------\n" );
    Printf("\r\nINFO - Display this menu\n" );
    Printf("\r\n0 - Return to main Setup Menu\n" );
    Printf("\r\n1 - Set message level (0x%02X)\n", app.dbgMsgLevel );
    Printf("\r\n2 - Display NVRAM EDID\n" );
    Printf("\r\n3 - Display NVRAM Boot Data\n" );
    Printf("\r\n4 - Reset NVRAM to default\n" );
    Printf("\r\n5 - Firmware version\n" );
    Printf("\r\n6 - Reset EEPROM to default\n" );
    /*
    Printf("\r\n7 - Reset MHL Port\n" );
    */

    app.lcdMenu[0] = "Return to main  ";
    app.lcdMenu[1] = "Firmware version";
    app.lcdMenu[2] = "Reset NVRAM";
    app.lcdMenu[3] = "Show NVRAM Boot";
    app.lcdMenu[4] = "Reset EEPROM";
    SkAppLcdMenuListSet(app.lcdMenu, 5);

}
#endif
//------------------------------------------------------------------------------
// Function:    SetupLevelZero
// Description: Command Mode processor
//------------------------------------------------------------------------------

static void SetupLevelZero ( RC5Commands_t key )
#if 1
{}
#else
{
    uint16_t    rcKey;
    bool_t      done = false;
    //uint8_t 	mhlPort;

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
         switch(SkAppLcdMenuSelectionGet())
         {
             case 0: key = RC5_0; break;
             case 1: key = RC5_5; break;
             case 2: key = RC5_4; break;
             case 3: key = RC5_3; break;
             case 4: key = RC5_6; break;
             default: break;
         }

    }

    switch ( key )
    {
        case RC5_SELECT:
            SetupLevelZeroPrintMenu();
            break;

        case RC5_0:
            app.setupMenuLevel = 0;
            //Printf("\r\n\n Setup Level -- Press MENU key to select Setup Menu\n" );
            SkAppSetupMenuPrint();
            break;
        case RC5_1:
            Printf("\r\n (0-3): " );
            app.dbgMsgLevel = SkAppIrGetSingleDigit( 3 );
            if ( app.dbgMsgLevel == 3 )
            {
                app.dbgMsgLevel = MSG_PRINT_ALL;
            }
            SiiPlatformEepromWriteByte( EE_UART_MSG_LEVEL, app.dbgMsgLevel );
            SiiPlatformLoggerMessageLevelSet( app.dbgMsgLevel );
            break;
        case RC5_2:
            if ( SiiEdidRead( EDID_NVRAM, dispData ))
            {
                SkAppDisplayData( MSG_ALWAYS, dispData, EDID_TABLE_LEN );
            }
            else
            {
                Printf("\r\nError reading NVRAM\n" );
            }
            break;
        case RC5_3:
            if ( SiiEdidRead( EDID_NVRAM_BOOT, dispData ))
            {
                SkAppDisplayData( MSG_ALWAYS, dispData, EDID_DEVBOOT_LEN );
                SkAppDisplayDataLcd(dispData, EDID_DEVBOOT_LEN);
            }
            else
            {
                Printf("\r\nError reading NVRAM BOOT DATA\n" );
                SkAppLcdMessageBox("Error reading", "NVRAM BOOT DATA!");
            }
            break;
        case RC5_4:
            Printf("\r\n Press OK to confirm NVRAM restore: " );
            SkAppLcdMessageBox("Press OK to", "restore NVRAM");
            done = false;
            while ( !done )
            {
                rcKey = SkAppGetOkCancelKey();
                switch ( rcKey )
                {
                    case RC5_INCREMENT:
                        Printf("\r\n\n" );
                        if ( !SiiDrvDeviceNvramInitialize( true ))
                        {
                            Printf("\r\n---ERROR IN NVRAM UPDATE\n" );
                            SkAppLcdMessageBox("ERROR IN", "NVRAM UPDATE");
                        }
                        else
                        {
                            Printf("\r\n---NVRAM restore complete\n" );
                            SkAppLcdMessageBox("NVRAM Restore", "COMPLETE");
                        }
                        done = true;
                        break;
                    case RC5_BUFFER_EMPTY:
                        break;
                    default:
                        Printf("\r\n---Canceled\n" );
                        SkAppLcdMessageBox("NVRAM Restore", "Canceled");
                        done = true;
                        break;
                }
            }
            break;
        case RC5_5:
            SkAppSignon();
            SkAppLcdSignOn();
            break;
        case RC5_6:
            Printf("\r\n Press OK to confirm EEPROM restore: " );
            SkAppLcdMessageBox("Press OK to", "restore EEPROM");
            done = false;
            while ( !done )
            {
                rcKey = SkAppGetOkCancelKey();
                switch ( rcKey )
                {
                    case RC5_INCREMENT:
                        SkAppEepromInitialize( true, true );
                        Printf("\r\n---EEPROM restore complete\n" );
                        SkAppLcdMessageBox("EEPROM Restore", "COMPLETE");
                        done = true;
                        break;
                    case RC5_BUFFER_EMPTY:
                        break;
                    default:
                        Printf("\r\n---Cancelled\n" );
                        SkAppLcdMessageBox("EEPROM Restore", "Canceled");
                        done = true;
                        break;
                }
            }
            break;
        case RC5_7:
        	/*
			Printf("\r\n (0-4): " );
			mhlPort = SkAppIrGetSingleDigit( 3 );
			if ( !SiiDrvSwitchInitialize( mhlPort, SK_ARC_PORT ) )
			{
				Printf("\r\nSiiDrvSwitchInitialize Failed!!\n" );
			}
			if ( !SiiCbusInitialize() )
			{
				Printf("\r\nSiiCbusInitialize() Failed!!\n" );
			}
			*/
			break;

        default:
            break;
    }
}
#endif
//-------------------------------------------------------------------------------------------------
//! @brief      Take input from either RC or Joystick and convert it into equivalent RC key command.
//-------------------------------------------------------------------------------------------------

uint16_t SkAppGetOkCancelKey(void)
{
    #if 0
    uint16_t rcKey;
    JoystickState_t jstat;

    rcKey = SiiPlatformInfraRedGetKey();
    jstat = SiiPlatformJstkStateGet();
    if (jstat != JNAV_NONE)
    {
        if (jstat == JNAV_SELECT)
        {
            rcKey = RC5_INCREMENT;
        }
        else
        {
            rcKey = 0; // cancel
        }
    }
    return rcKey;
    #else
    return 0;
    #endif
}


void SkAppSetupMenuPrint(void)
{
    Printf("\r\n\n Setup Menu\n" );
    DEBUG_PRINT( MSG_ALWAYS,   " -----------\n" );
    Printf("\r\n0 - Return to TV Mode\n" );
    Printf("\r\n1 - Setup Menu One\n" );
    Printf("\r\n2 - Setup Menu Two\n" );
    Printf("\r\n3 - Diagnostics Menu\n" );
    Printf("\r\n\n Select Setup Menu (0-3): " );

    // Fill up LCD menu
    app.lcdMenu[0] = "TV Mode";
    app.lcdMenu[1] = "Setup One";
    app.lcdMenu[2] = "Setup Two";
    app.lcdMenu[3] = "Diagnostics";
    SkAppLcdMenuListSet(app.lcdMenu, 4);
}

//------------------------------------------------------------------------------
// Function:    SkAppRcSetupMenu
// Description: Parse remote control keys for the setup application
//------------------------------------------------------------------------------

bool_t SkAppRcSetupMenu ( RC5Commands_t key, bool_t fullAccess )
{
    bool_t  usedKey = false;

    switch ( key )
    {
        case RC5_MENU:
           SkAppSetupMenuPrint();
           app.setupMenuLevel = 0; // switch to menu idle mode
           usedKey = true;
           break;

        case RC5_0:
        case RC5_1:
        case RC5_2:
        case RC5_3:
            if (app.setupMenuLevel == 0)
            {
               app.setupMenuLevel = key - RC5_0 + 2;
               // Leave the key unused so that it gets passed to the new mode to display its menu
               key = RC5_SELECT;    // Cause new level menu to be displayed
            }
           break;

        case RC5_INCREMENT: // OK
        case RC5_ENTER:
            if (app.setupMenuLevel == 0)
            {
                app.setupMenuLevel = SkAppLcdMenuSelectionGet() + 2;
                key = RC5_SELECT;    // Cause new level menu to be displayed
            }
            break;

        default:
            break;
    }

    switch ( app.setupMenuLevel )
    {
        case 0:         // Menu idle mode
            break;
        case 1:
            break;      // Show Menu mode
        case 2:
            SkAppIrRemoteReturnToTV();
            break;
        case 3:
            SetupLevelZero( key );
            break;
        case 4:
            SetupLevelOne( key );
            break;
        case 5:
            SkAppDiagMenu( key );
            break;
        default:
            break;
    }

    return( usedKey );
}



