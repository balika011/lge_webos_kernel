//***************************************************************************
//!file     si_switch_component.c
//!brief    Silicon Image Switch Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_switch_component.h"            // Application layer
#include "si_drv_switch.h"                  // Driver layer
#include "si_c99support.h"

//------------------------------------------------------------------------------
//  Module Data
//! \addtogroup si_switch_component
//! @{
//------------------------------------------------------------------------------

SiiSwitchInstanceData_t switchInstance[SII_NUM_SWITCH] =
{
	{
		0,							// structVersion
	    SII_SUCCESS,             // lastResultCode Contains the result of the last API function called
	    0,                          // statusFlags
	    0,                          // selPort;
	}
};
SiiSwitchInstanceData_t *pSwitch = &switchInstance[0];

//-------------------------------------------------------------------------------------------------
//! @brief      Configure the Switch component
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

bool_t SiiSwitchConfigure ( void )
{

	return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Initialize the Switch component
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

bool_t SiiSwitchInitialize ( void )
{
    return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns a status flag word containing Switch-specific
//! @brief      information about the state of the device.
//!
//! @retval     Sixteen-bit status flags word for the Switch Component
//-------------------------------------------------------------------------------------------------

uint16_t SiiSwitchStatus ( void )
{

	return( pSwitch->statusFlags );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Places the Switch component into standby mode if available
//!
//! @return     Result of standby attempt
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

bool_t SiiSwitchStandby ( void )
{

	return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Restores the Switch component to full operation if in standby
//!
//! @return     Result of resume attempt
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

bool_t SiiSwitchResume ( void )
{

	return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the result of the last SiiSwitchxxx function called
//!
//! @return     Result of last Switch component function
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function:    SiiSwitchGetLastResult
// Description: Returns the result of the last SiiSwitchxxx function called
// Parameters:  none
// Returns:     Result of last Switch component function.
//------------------------------------------------------------------------------

int_t SiiSwitchGetLastResult ( void )
{

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiSwitchSelectSource
// Description: Connect the passed input port to the main pipe.
//------------------------------------------------------------------------------

void SiiSwitchSelectSource ( uint_t portIndex )
{
    // Switch user select to the requested port.

    pSwitch->selPort = portIndex;
    SiiDrvSwitchSourceSelect( portIndex );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Determine the next available HDMI/MHL input (circular) for display.
//!             Starts from the passed current port and skips any unconnected
//!             ports.
//!
//! @param[in]  currentPort - Rx port (0-3).
//! @param[in]  isForward   - Next port is given by increment (true), or decrement (false) of port's index
//!
//! @return     Next sequential active HDMI/MHL port.
//-------------------------------------------------------------------------------------------------
uint8_t SiiSwitchGetNextPort ( uint8_t currentPort, bool_t isForward )
{
    uint8_t i, stateValue;

    stateValue = SiiDrvSwitchStateGet( SiiSwitch_INPUT_PORTS_CONNECT_STATE );

    for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
    {
        if (isForward)
        {
            currentPort = (currentPort + 1) % SII_INPUT_PORT_COUNT;
        }
        else
        {
            currentPort = (currentPort == 0) ? (SII_INPUT_PORT_COUNT - 1) : (currentPort - 1);
        }

            /* Only choose port if it is plugged in.    */

        if ( stateValue & ( 0x01 << currentPort))
        {
            break;
        }
    }

    return( currentPort );
}

//------------------------------------------------------------------------------
// Close the Doxygen group.
//! @}
//------------------------------------------------------------------------------
