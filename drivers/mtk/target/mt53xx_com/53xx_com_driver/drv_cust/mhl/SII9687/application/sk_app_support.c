//***************************************************************************
//!file     sk_app_support.c
//!brief    Silicon Image Starter Kit Application support functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#include "si_osal_timer.h"
#include "sk_application.h"
#include "si_edid_component.h"
#include "si_eeprom.h"
#include <linux/string.h>

#if 0
#include "../si_osal_timer.h"
#include "sk_application.h"
#include "../component/edid_inp_proc/si_edid_component.h"
#include "../platform/hal/eeprom/si_eeprom.h"
#include <linux/string.h>
#endif

//------------------------------------------------------------------------------
// Application Signature data
//------------------------------------------------------------------------------

//static char lSignonMsg [] = "SK9687 Firmware v";
//static char lCopyrightMsg [] = "Copyright Silicon Image Inc, 2011";
//extern char *buildTime;     // From buildtime.c
//extern char *buildVersion;  // From buildtime.c


//------------------------------------------------------------------------------
// Application EDID data
//------------------------------------------------------------------------------

extern const uint8_t gEdidFlashEdidTable [ EDID_TABLE_LEN ];
extern const uint8_t gEdidFlashEdidVgaTable [ EDID_VGA_TABLE_LEN ];
extern const SiiDeviceBootData_t gEdidFlashDevBootData;

char gLcdStr[LCD_MENU_MAX_LINES][LCD_LIST_MAX_CHARS];

//-------------------------------------------------------------------------------------------------
//! @brief      Display standard header and version information.
//-------------------------------------------------------------------------------------------------

void SkAppSignon ( void )
{
   // Printf("\r\n\n\n%s%s\n%s\n", lSignonMsg, buildVersion, lCopyrightMsg );
  //  DEBUG_PRINT( MSG_ALWAYS, buildTime );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Display the passed buffer in ASCII-HEX block format.
//!
//! @param[in]  pData  - pointer to array of bytes
//! @param[in]  length - length of the array
//-------------------------------------------------------------------------------------------------

void SkAppDisplayData ( uint8_t msgLevel, uint8_t *pData, uint16_t length )
{
    uint16_t    i = 0;
    uint8_t     x, y;

    for ( y = 0; i < length; y++ )
    {
        DEBUG_PRINT( msgLevel, "\n%02X: ", i );
        for ( x = 0; (i < length) && (x < 16); x++ )
        {
            DEBUG_PRINT( msgLevel, " %02X", (uint16_t)pData[ i++] );
        }
    }
    DEBUG_PRINT( msgLevel, "\n" );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Display the passed buffer in ASCII-HEX block format on LCD list.
//!
//! @param[in]  pData  - pointer to array of bytes
//! @param[in]  length - length of the array
//-------------------------------------------------------------------------------------------------

void SkAppDisplayDataLcd ( uint8_t *pData, uint16_t length )
{
    int_t    i;

    // Protection
    if (length >= LCD_MENU_MAX_LINES)
    {
        length = LCD_MENU_MAX_LINES - 1;
    }

    // Add menu header
    app.lcdMenu[0] = "Back to Menu";

    for (i = 0; i < length; i++)
    {
     //   SiiSprintf(gLcdStr[i], "NV %02X : %02X", i, (int_t) pData[i]);
        app.lcdMenu[i + 1] = gLcdStr[i];
    }
    SkAppLcdMenuListSet(app.lcdMenu, length + 1);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Handler for SiiMon communication.
//-------------------------------------------------------------------------------------------------

void SkAppExtMonitorHandler(uint_t inst)
{
   /* // Only check the debugger if power is on.
    if (app.powerState == APP_POWERSTATUS_ON)
    {
        if (SkRemoteRequestHandler())
        {
            if (!app.commMode)
            {
                app.commMode = true;
                SiiSchedHoldMode(true);
                SkAppLcdMessageBox("      COMM", "      MODE");
            }
        }
        else
        {
            if (app.commMode)
            {
                app.commMode = false;
                SiiSchedHoldMode(false);
                SkAppSwitchPortUpdate(true);
            }
        }
    }*/
}

//------------------------------------------------------------------------------
// Function:    SkAppEepromInitialize
// Description: Initialize any uninitialized areas of the EEPROM
//-----------------------------------------------------------------------------c

bool_t SkAppEepromInitialize( bool_t useDefaults, bool_t forceInit )
{
#if 0
    bool_t  updated = false;
    uint8_t controlByte;
    uint8_t initBuffer [256];

    /* Use the current settings to initialize control table.    */

    if (( SiiPlatformEepromReadByte( EE_CONFIG_VALID ) != CONFIG_VALID) || forceInit )
    {
        if ( useDefaults )
        {
            SiiPlatformEepromWriteByte( EE_CONFIG_VALID, 0 );    // In case we get interrupted before we are finished
            memset( initBuffer, 0xFF, 256 );
            SiiPlatformEepromWriteBlock( 0, initBuffer, 256 );
            SiiPlatformEepromWriteByte( EE_FW_CONTROL, (EEFLAG_ROTARY_ENABLE | EEFLAG_IR_ENABLE ));
            SiiPlatformEepromWriteByte( EEPROM_EDID_LOADSEL, 0 );
            SiiPlatformEepromWriteByte( EE_DB_VERSION, EEPROM_DB_VERSION );
            SiiPlatformEepromWriteByte( EE_UART_MSG_LEVEL, MSG_DBG );
        }
        else
        {
            controlByte = app.irEnable     ? EEFLAG_IR_ENABLE      : 0;
            SiiPlatformEepromWriteByte( EE_FW_CONTROL, controlByte );
            SiiPlatformEepromWriteByte( EEPROM_EDID_LOADSEL, app.edidLoad );
            SiiPlatformEepromWriteByte( EE_DB_VERSION, EEPROM_DB_VERSION );
            SiiPlatformEepromWriteByte( EE_UART_MSG_LEVEL, app.dbgMsgLevel );
        }
        SiiPlatformEepromWriteByte( EE_CONFIG_VALID, CONFIG_VALID );
        updated = true;
    }

    if (( SiiPlatformEepromReadByte( EE_DEVBOOTDATA_VALID ) != CONFIG_VALID ) || forceInit )
    {
        SkAppEdidWrite( EDID_EEPROM_BOOT, (uint8_t *)&gEdidFlashDevBootData );
        updated = true;
    }
    if (( SiiPlatformEepromReadByte( EE_EDID_VALID ) != CONFIG_VALID ) || forceInit )
    {
        SkAppEdidWrite( EDID_EEPROM, gEdidFlashEdidTable );
        updated = true;
    }
    if (( SiiPlatformEepromReadByte( EE_EDIDVGA_VALID ) != CONFIG_VALID ) || forceInit )
    {
        SkAppEdidWrite( EDID_EEPROM_VGA, gEdidFlashEdidVgaTable );
        updated = true;
    }

    return( updated );
   #else
    return (1);
   #endif
}


//-------------------------------------------------------------------------------------------------
//! @brief      Application task selector.
//-------------------------------------------------------------------------------------------------

void SkAppUiTask(uint_t inst)
{
  //  Printf("\r\nSkAppUiTask\n");

    SkAppUiInputControlsUpdate();

    switch (app.modeSelect)
    {
        case APP_MODE_TV:
            SkAppTvTask();
            break;
        case APP_MODE_DEMO_MENU:
            break;
        case APP_MODE_SETUP_MENU:
            break;
#if 0// INC_BENCH_TEST
            case APP_MODE_BTST:
            SkAppBenchTest();
            break;
#endif
        default:
            break;
    }

    // Check for port updates from any of the tasks.
    SkAppSwitchPortUpdate(false);

    // Indicate status of interrupt line
    // it has to remain high most of the time
  //  SiiPlatformGpioLedStatus2(!SiiPlatformInterruptPinStateGet());
}


//------------------------------------------------------------------------------
// Function:    SkAppUiInputControlsUpdate
// Description:
//------------------------------------------------------------------------------

void SkAppUiInputControlsUpdate ( void )
{
 //   bool_t isEnabledBySwitch;

    // Read rotary switch selection
  //  app.rotarySwitch    = SiiPlatformReadRotary(false);

    // Read dip switch states
    
    
        if (app.autoSwitch)
        {
            SkAppPortAutoSwitchInit();
        }

     //   Printf("\r\nRx Port Switching: %s\n", app.autoSwitch  ? "AUTO" : "MANUAL" );
        
#if 0
        SI_LCD_CLEAR();
        SI_LCD_PRINTF(0,"Rx Port Switch:");
        SI_LCD_PRINTF(1,"%s", app.autoSwitch  ? "AUTO" : "MANUAL");
#endif
    
#if 0

    // Read dip switch states
    isEnabledBySwitch = !SiiPlatformSwitchGet(UI_ROVING_ENABLE);
    if (app.rovingEnabled != isEnabledBySwitch)
    {
#if (INC_9617 == 0)
    	// InstaPort roving enable change has detected
        app.rovingEnabled = isEnabledBySwitch;
        Printf("\r\nInstaPort Roving: %s\n", app.rovingEnabled  ? "ON" : "OFF" );

        SI_LCD_CLEAR();
        SI_LCD_PRINTF(0,"InstaPort Roving");
        SI_LCD_PRINTF(1,"%s", app.rovingEnabled  ? "ON" : "OFF");

        SiiDrvSwitchRovingEnable(app.rovingEnabled);
#endif
    }

    isEnabledBySwitch = !SiiPlatformSwitchGet(UI_RC_ENABLE);
    if (app.irEnable != isEnabledBySwitch)
    {   // RC enable switch change has detected
        app.irEnable = isEnabledBySwitch;
        Printf("\r\nIR Remote Control: %s\n", app.irEnable  ? "ON" : "OFF" );

        SI_LCD_CLEAR();
        SI_LCD_PRINTF(0,"IR Remote:");
        SI_LCD_PRINTF(1,"%s", app.irEnable  ? "ON" : "OFF");
    }

    isEnabledBySwitch = SiiPlatformSwitchGet(UI_FLASH_UPDATE);
    if (app.fwUpdEnabled != isEnabledBySwitch)
    {
        app.fwUpdEnabled = isEnabledBySwitch;
        Printf("\r\n%s\n", app.fwUpdEnabled  ? "RESET Main MCU to enter F/W update mode." : "Canceled." );

        SI_LCD_CLEAR();
        SI_LCD_PRINTF(0,"F/W UPDATE MODE:");
        SI_LCD_PRINTF(1,"%s", app.fwUpdEnabled  ? "RESET Main MCU" : "Canceled");
    }

  #endif

}

//------------------------------------------------------------------------------
// Function:    SkAppInstaPortLedUpdate
// Description:
//------------------------------------------------------------------------------

void SkAppInstaPortLedUpdate ( uint8_t portBits )
{
  //  SiiPlatformGpioLedPorts(portBits);
}

//------------------------------------------------------------------------------
// Function:    SkAppBlinkTilReset
// Description:
//-----------------------------------------------------------------------------c

void SkAppBlinkTilReset ( uint8_t errorLedPattern )
{
/*
    SI_LCD_CLEAR();
    SI_LCD_PRINTF(0, "Device Startup");
    SI_LCD_PRINTF(1, "FAILED!");

    for ( ;; )
    {
        SkRemoteRequestHandler();

        SiiPlatformGpioLedPorts( errorLedPattern );
        SiiOsTimerWait( 300 );
        errorLedPattern ^= 0x0F;
        SkAppTaskIrRemote(0);
    }
    */
}

//-------------------------------------------------------------------------------------------------
//! @brief      Indicator of running firmware.
//-------------------------------------------------------------------------------------------------

void SkAppBlinkAlive(uint_t inst)
{
    static bool_t isOn = false;
  //  SiiPlatformGpioLedStatus1(isOn);
    isOn = !isOn;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Indicator of incoming device interrupts.
//-------------------------------------------------------------------------------------------------

void SkAppBlinkInterrupts(void)
{
    static bool_t isOn = false;
//    SiiPlatformGpioLedStatus2(isOn);
    isOn = !isOn;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Print version on LCD display.
//-------------------------------------------------------------------------------------------------

void SkAppLcdSignOn(void)
{
    SI_LCD_CLEAR();
#if 0//(INC_9617 == 1)
    SI_LCD_PRINTF(0, "SK9617");
#else
//    SI_LCD_PRINTF(0, "SK9687");
#endif
//    SI_LCD_PRINTF(1, "FW Ver: %s", buildVersion);
}



//-------------------------------------------------------------------------------------------------
//! @brief      Initialize application instance data structures.
//-------------------------------------------------------------------------------------------------

void SkAppInitInstanceData ( void )
{
    memset( &app, 0, sizeof( AppInstanceData_t ));

    app.structVersion   = 0;
    app.lastResultCode  = SII_SUCCESS; // lastResultCode Contains the result of the last API function called
    app.statusFlags     = 0;
    app.powerState      = APP_POWERSTATUS_STANDBY_TO_ON;
    app.oldPowerState   = APP_POWERSTATUS_STANDBY;

    app.modeSelect      = APP_MODE_TV;

    // EEPROM settings
    app.irEnable        =false;// true;//liuqing changed 20130111 
    app.dontUseNvram    = false; // dontUseNvram - Do not perform any NVRAM access, load SRAMs from EEPROM
    app.dbgMsgLevel     = MSG_PRINT_ALL;
    app.edidLoad        = 0x00;

    // ARC
    app.arcEnabled      = false;

    // TV App
    app.rotarySwitch    = RSW_NO_CHANGE;
    app.lastRotarySwitch= RSW_NO_CHANGE;
#if 0//9786
	if(1 == GetCurrentSourceInputVarant())
	{
		  app.portSelect      = 1;
		  app.oldPort         = 1;
		 // Printf("\n@@@@Jane: app.portSelect      = 1.\n");
	}
	else 
	{
			app.portSelect      =0;
			app.oldPort         = 0;
		// Printf("\n@@@@Jane: app.portSelect      = 0.\n");
	}
  #else
            app.portSelect      =0;
			app.oldPort         = 0;
  #endif
		
   // app.portSelect      = 0;
   // app.oldPort         = 0xFF;
    app.autoSwitch      = false;
    //app.autoSwitch      = true;
    app.rovingEnabled   = true;

    // TPG
    app.tpgMode         = APP_TPG_OFF;
    app.isTpgActiveDemo = false;
    app.tpgVideoFormat  = SI_TPG_FMT_VID_720_60;
    app.tpgPattern      = SI_ETPG_PATTERN_BLUE;
    app.tpgClockSrc     = SI_TPG_CLK_INTERNAL;

    // IPV
    app.ipvAllowed      = true;
    app.ipvEnabled      = false;
    app.ipvPipEnabled   = false;
#if INC_IPV
    app.ipvMode         = SiiIpvModeALL;
    app.ipvTheme        = SiiIpvLEFT_MIDDLE;
#endif
    app.ipvHighlightWindow = 0;
    app.pipSource       = 0;

}


//-------------------------------------------------------------------------------------------------
//! @brief      Initialize application during restart.
//!
//!             Preserves power state.
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SkAppInitialize ( void )
{
 //   uint8_t flags;
    SiiAppPowerStatus_t powerState      = app.powerState;     // Need to remember these
    SiiAppPowerStatus_t oldPowerState   = app.oldPowerState;

    SkAppInitInstanceData();
    app.powerState    = powerState;
    app.oldPowerState = oldPowerState;

    /* Load control flags from EE_PROM. */
#if 0
    flags = SiiPlatformEepromReadByte( EE_FW_CONTROL );
    app.irEnable      = ((flags & EEFLAG_IR_ENABLE) == EEFLAG_IR_ENABLE );
    app.edidLoad      = SiiPlatformEepromReadByte( EEPROM_EDID_LOADSEL );
    app.dbgMsgLevel   = SiiPlatformEepromReadByte( EE_UART_MSG_LEVEL );
#else
    app.irEnable        = true;
    app.edidLoad        = 0x00;
    app.dbgMsgLevel     = MSG_PRINT_ALL;

#endif
    app.tpgVideoFormat = SI_TPG_FMT_VID_480_60;
    app.tpgClockSrc   = SI_TPG_CLK_MAIN_PIPE;

   // SiiPlatformLoggerMessageLevelSet( app.dbgMsgLevel );

    SkAppUiInputControlsUpdate();
   // app.portSelect = 0;//SiiPlatformReadRotary( true ); // Read immediate value of RSW (not debounced)

    Printf("IR Enabled:    %s\n", app.irEnable  ? "YES" : "NO" );
    Printf("\r\n\n" );

    SkAppLcdSignOn();
    return( true );
}


void Set_input_PORT(uint8_t port)
{
     //if( !app.autoSwitch )
    {
        app.portSelect= port;
        Printf("\r\n 9678-HDMI Switch=%d", port);
        // Printf("\r\n9687 Set_input_PORT: %d\n" ,app.portSelect);//0121
        SkAppSwitchPortUpdate ( true);
        SiiMhlRxPathEnable( true );//jane 20130124
    }
}
