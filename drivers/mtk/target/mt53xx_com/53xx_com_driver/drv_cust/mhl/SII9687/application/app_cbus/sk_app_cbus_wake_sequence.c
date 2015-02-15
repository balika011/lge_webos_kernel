//***************************************************************************
//!file     sk_app_cbus.c
//!brief    Wraps board and device functions for the CBUS component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if  1

#include "sk_application.h"

#include "si_osal_timer.h"
#include "si_gpio.h"
#include "sk_app_cbus.h"


#include "si_platform.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "debug.h"
#include "gpio.h"
#include "interrupt.h"
#include "rom.h"
#include "sysctl.h"
#include "timer.h"
#include "hw_timer.h"

#include <linux/string.h>


//------------------------------------------------------------------------------
// Function:    SkCbusPulseReset
// Description:  Reset all the Cbus Wake up Status
//------------------------------------------------------------------------------
#if 0
static void SkCbusPulseReset(void )
{
	memset( &pAppCbus->cbusWakeup, 0, sizeof(SiiCbusWakeUpData_t));
}
//------------------------------------------------------------------------------
// Function:    SkCbusPulseCheck
// Description:  Check the pulse status
//------------------------------------------------------------------------------

static bool_t SkCbusWakeupPulseCheck(void )
{
	//cBusPin has changed the state
	if ( pAppCbus->cbusWakeup.cbusPin )
	{
		//ok the pulse is in range
		if ((pAppCbus->cbusWakeup.cbusTimeCount >= pAppCbus->cbusWakeup.cbusMin) &&
		    (pAppCbus->cbusWakeup.cbusTimeCount <= pAppCbus->cbusWakeup.cbusMax))
		{
		    pAppCbus->cbusWakeup.cbusPulse++;
		    pAppCbus->cbusWakeup.cbusTimeCount=0;
		}
		else
		{	//pulse is not in the range.
			//pAppCbus->cbusWakeup.cbusTimeCount=0;
			return false;
		}
	}
	else
	{
		//too low or too hi for too long
		if (pAppCbus->cbusWakeup.cbusTimeCount > pAppCbus->cbusWakeup.cbusMax)
		{
		    pAppCbus->cbusWakeup.cbusTimeCount=0;
			return false;
		}
		pAppCbus->cbusWakeup.cbusTimeCount++;
	}
	return true;
}

#endif
//------------------------------------------------------------------------------
// Function:    SkAppCbusWakeup
// Description:  Check the Cbus Wake up Sequence.
//------------------------------------------------------------------------------
#if 0
bool_t SkAppCbusWakeupSequence( uint8_t pwrState)
{
	bool_t cbusPin = false;
	bool_t results=false;
	
	if (!pwrState)
	{
		if (!SiiOsTimerExpired( pAppCbus->wupTimer ))
		{
			return results;
		}
		SiiOsTimerSet( &pAppCbus->wupTimer, 1);
		cbusPin = SiiPlatformCbusWakeupPinGet();

		if ((cbusPin == CBUS_SRC_WP_POLARITY) && !pAppCbus->wakeDetect)
		{
			SkCbusPulseReset();
			pAppCbus->cbusWakeup.cbusState = CBUS_SRC_WAKE_PULSE;
			return false;
		}

		pAppCbus->wakeDetect = true;
		switch (pAppCbus->cbusWakeup.cbusState)
		{
			case CBUS_SRC_WAKE_PULSE:
			    //pAppCbus->cbusWakeup.cbusTimeCount++;
				if (pAppCbus->cbusWakeup.cbusPulse == 3)
				{
				    pAppCbus->cbusWakeup.cbusMin = CBUS_SRC_WP_WIDTH2_MIN;
				    pAppCbus->cbusWakeup.cbusMax = CBUS_SRC_WP_WIDTH2_MAX;
				}
				else
				{
				    pAppCbus->cbusWakeup.cbusMin = CBUS_SRC_WP_WIDTH1_MIN;
				    pAppCbus->cbusWakeup.cbusMax = CBUS_SRC_WP_WIDTH1_MAX;
				}

				//complement the cbusPin for Odd pulse
				if (pAppCbus->cbusWakeup.cbusPulse % 2)
				{
				    pAppCbus->cbusWakeup.cbusPin = !cbusPin;
				}
				else
				{
				    pAppCbus->cbusWakeup.cbusPin = cbusPin;
				}

				results = SkCbusWakeupPulseCheck();

				//Number of pulse has been reached.
				if (pAppCbus->cbusWakeup.cbusPulse > CBUS_SRC_PULSE_MAX)
				{
				    pAppCbus->cbusWakeup.cbusState = CBUS_SRC_DISCOVERY_PULSE;
				}
				else
				{
					break;
				}
			case CBUS_SRC_DISCOVERY_PULSE:
			    pAppCbus->cbusWakeup.cbusMin = CBUS_SRC_WP_DISCOVER_MIN;
			    pAppCbus->cbusWakeup.cbusMin = CBUS_SRC_WP_DISCOVER_MAX;
			    pAppCbus->cbusWakeup.cbusState = CBUS_SRC_WAKE_PULSE;
			    pAppCbus->wakeDetect = false;
			    
				DEBUG_PRINT(MSG_DBG, "\n************************************************");
				DEBUG_PRINT(MSG_DBG, "\nCBUS Power Up by detecting wake up pulse sequence");
				DEBUG_PRINT(MSG_DBG, "\n************************************************");

				if ( app.powerState != APP_POWERSTATUS_ON )
				{
				    app.powerState = APP_POWERSTATUS_STANDBY_TO_ON;
				}
				SkCbusPulseReset();
				return true;
		}

		if (!results)
		{
		    pAppCbus->cbusWakeup.cbusState = CBUS_SRC_WAKE_PULSE;
		    pAppCbus->wakeDetect = false;
			SkCbusPulseReset();
		}
	}
	return false;
}
#endif
#endif


