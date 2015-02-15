//***************************************************************************
//!file     sk_app_cbus.c
//!brief    Wraps board and device functions for the CBUS component
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
#include "si_drv_gpio.h"
#include "si_drv_switch.h"
#include "si_drv_device.h"
#include "si_arm.h"

#include <linux/string.h>

//------------------------------------------------------------------------------
// Function:    SkAppDeviceInitGpio
// Description: Perform any board-level initialization required at the same
//              time as GPIO driver initialization
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

bool_t SkAppDeviceInitGpio ( void )
{
    Printf("\r\n SkAppDeviceInitGpio...\n");
	SiiDrvGpioInitialize();

    // Set up MHL cable connection GPIO

	SiiDrvGpioPinType( SII_GPIO_PIN_0, SII_GPIO_ALT_MHL_CABLE_CONN );
	SiiDrvGpioPinConfigure(
			SII_GPIO_PIN_0,
	        SII_GPIO_INPUT | SII_GPIO_INT_RISING_EDGE |
	        SII_GPIO_INT_FALLING_EDGE );

    return( true);
}

//------------------------------------------------------------------------------
// Function:    SkAppTaskGpio
// Description: Wrapper for the GPIO Component at the application level
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskGpio ( uint_t inst )
{
    Printf("\r\nSkAppTaskGpio\n");

	if ( SiiDrvGpioStatus() )
	{
		if ( SiiDrvGpioPinRead( SII_GPIO_PIN_0 ) )
		{
			Printf("\r\nSkAppTaskGpio:: SII_GPIO_PIN_0 is high\n" );
			pAppCbus->cbusEnable = true;
		}
		else
		{
			Printf("\r\nSkAppTaskGpio:: SII_GPIO_PIN_0 is low\n" );
			pAppCbus->cbusEnable = false;
		}

        // MHL connection status has changed,
        // Kick-off CBUS task to react on the change
        SkAppTaskCbus(0);
	}
}



