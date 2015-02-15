//***************************************************************************
//!file     sk_app_device.c
//!brief    Silicon Image SiI9687 Starter Kit Application device functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#include "sk_application.h"
#include "si_cbus_component.h"
#include "si_edid_component.h"
#include "si_switch_component.h"

#include "si_drv_device.h"
#include "si_drv_cbus.h"
#include "si_drv_arc.h"
#include "si_drv_switch.h"
#include "si_drv_tx_simple.h"
#include "si_drv_gpio.h"

#include <linux/string.h>





//------------------------------------------------------------------------------
// Function:    AppDeviceInitialize
// Description: Initialize the various components of the SiI9687 device
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

static bool_t AppDeviceInitialize ( void )
{
    bool_t  success = false;

    Printf("\r\nDevice Initialization Started...\n");

    for ( ;; )
    {
        // Initialize device drivers.
        /*if ( !SiiCraInitialize())           break;*/
        if ( !SiiDrvDeviceInitialize())     break;
        if ( !SiiDrvTxSimpleInitialize())   break;
        if ( !SiiDrvSwitchInitialize( SK_ARC_PORT ))   break;

        // Initialize device components that need application wrappers
        if ( !SkAppInitEdid())              break;
        if ( !SkAppDeviceInitArc())         break;
        if ( !SkAppDeviceInitGpio())        break;
        if ( !SkAppDeviceInitCbus())        break;

        if ( !SiiDrvDeviceRelease())        break;//in test in 0114

#if INC_IPV
        Printf("\r\n INC_IPV==1.\n");
        if ( !SkAppTaskIpvInitialize())     break;
#endif

        success = true;
        break;
    }

    if ( success )
    {
        Printf("\r\nDevice Initialization Successful\n",
                SiiDrvDeviceInfo( SiiDEV_ID ), SiiDrvDeviceInfo( SiiDEV_REV ) );
        	// Send message to interrupt manager
       	 SiiSchedSendSignal(SI_EVENT_ID_PLATFORM_DEV_INT, 0);
    }
    else
    {
        Printf("\r\n!!!!Failure, halting...\n" );
    }
    if ( success && !SiiDrvDeviceStart())
    {
        success = false;
        Printf("\r\n!!!!Device Driver Start failure, halting...\n" );
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    AppDeviceStandby
// Description: Places the device into standby mode if available
// Parameters:  none
// Returns:     true if the device entered standby mode successfully, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

static void AppDeviceStandby ( void )
{
    bool_t failure = false;

    Printf("\r\n********************************\n");
    Printf("\r\n** DEVICE STANDBY (POWER OFF) **\n");
    Printf("\r\n********************************\n");

    // Place all components in standby

    failure |= !SkAppCbusStandby();
    failure |= !SiiSwitchStandby();
    failure |= !SiiEdidStandby();

    // Place all driver-only modules in standby

    failure |= !SiiDrvDeviceStandby();  // Last standby called is always the device main driver.

    if ( failure )
    {
        Printf("\r\n\n!!!!!! Error occurred entering Standby !!!!!!\n");
    }
}

//------------------------------------------------------------------------------
// Function:    AppDeviceResume
// Description: Causes operation of the device to return to the state it was
//              prior to the previous call to SiiDeviceStandby.
// Parameters:  none
// Returns:     true if the device successfully exited standby mode , or false
//              if some failure occurred.
// NOTE:        TX module is not restored to operation by this function.  When
//              the first HOTPLUG detect interrupt occurs following the power on
//              state, the TX module will automatically be restarted
//------------------------------------------------------------------------------

static void AppDeviceResume ( void )
{
    bool_t failure = false;

    Printf("\r\n********************************\n");
    Printf("\r\n** DEVICE RESUME (POWER ON)   **\n");
    Printf("\r\n********************************\n");

    // Power is back on, so restore any configuration information that
    // was saved at the last standby.

    failure |= !SiiDrvDeviceResume();   // First resume called is always the device main driver.
    //failure |= !SiiDrvHeacResume();

    failure |= !SiiSwitchResume();
    failure |= !SiiEdidResume();
    failure |= !SkAppCbusResume();

    // Tx will resume when a downstream hotplug is detected

    if ( failure )
    {
        Printf("\r\n\n!!!!!! Error occurred during Resume !!!!!!\n");
    }
}

//------------------------------------------------------------------------------
// Function:    SkAppDevicePower
// !brief       Monitor and implement device power requests.
// Description: If any module has changed the app.powerState value, apply
//              the appropriate action for the current application power state.
//              This state may have been modified by the physical UI, the remote
//              control, a CEC or CBUS message, or some other external influence.
//------------------------------------------------------------------------------

void SkAppDevicePower ( uint_t inst )
{
	Printf("\r\n app.powerState == %x \n",app.powerState);
	Printf("\r\n app.oldPowerState == %x \n",app.oldPowerState);
	
        if ( app.powerState == APP_POWERSTATUS_ON )
        {
        	app.powerState = APP_POWERSTATUS_ON_TO_STANDBY;
        }
        else if (( app.powerState == APP_POWERSTATUS_STANDBY ) || ( app.powerState == APP_POWERSTATUS_INITIAL ))
        {
        	app.powerState = APP_POWERSTATUS_STANDBY_TO_ON;
        }

    // If power state was changed by somebody, act on it.
	Printf("\r\n app.powerState == %x \n",app.powerState);
	Printf("\r\n app.oldPowerState == %x \n",app.oldPowerState);

    if ( app.powerState != app.oldPowerState )
    {
        Printf("\r\n\n====================================================\n");
        Printf("\r\n==  Main Power: %-7s                           ==\n",
            ( app.powerState == APP_POWERSTATUS_STANDBY_TO_ON )? "ON     " : "STANDBY");
        Printf("\r\n====================================================\n");

        switch ( app.powerState )
        {
            case APP_POWERSTATUS_STANDBY_TO_ON:

                // Turn on the device power
              //  SiiPlatformGpioSetPower(SII_GPIO_PDN_POWER, ON );
                if ( SkAppInitialize())
                {
                    app.powerState = APP_POWERSTATUS_ON;
                    if ( AppDeviceInitialize())
                    {
                        AppDeviceResume();
                        break;
                    }
                }
                // Initialization failed somehow, let the user know it.
                SkAppBlinkTilReset( ERRLED_BAD_INIT );
                break;

            case APP_POWERSTATUS_ON_TO_STANDBY:
                AppDeviceStandby();
                app.powerState = APP_POWERSTATUS_STANDBY;

                // Power down the device
   //             SiiPlatformGpioSetPower(SII_GPIO_PDN_POWER, OFF );
                break;

            case APP_POWERSTATUS_ON:
            case APP_POWERSTATUS_STANDBY:
            default:
                break;
        }
        app.oldPowerState     = app.powerState;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief
//! @param[in]
//! @param[in]
//-------------------------------------------------------------------------------------------------
void SkAppProcess( SiiAppPowerStatus_t powerState, int source )
{
	powerState = powerState;
	app.portSelect 	= source;

	// Service the Silicon Image device interrupts.  If the system is in
	// standby, a reduced interrupt check is performed only on those
	// modules that are capable of waking up the system.
	SiiDrvDeviceManageInterrupts(0);

	// Run background tasks
    //  CbusCdSensePinGet();		// Check for MHL cable connect/disconnect
    
	SkAppTaskCbus(0);
	SkAppTaskSwitch(0);
	//SkAppTaskRepeater();
	SkAppSwitchOnTxMuteChange(0);
	//SkAppTaskThxDemo( app.thxDemoEnabled );
	SkAppUiTask(0);
	//SkAppSwitchPortUpdate(0);		// Check for port updates from any of the tasks.
}


