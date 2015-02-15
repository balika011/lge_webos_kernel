//***************************************************************************
//!file     si_drv_gpio.c
//!brief    Silicon Image CPI Driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_osal_timer.h"
#include "si_scheduler.h"
#include "si_drv_gpio.h"
#include "si_drv_gpio_internal.h"
#include "si_drv_internal.h"

//------------------------------------------------------------------------------
//  Driver Data
//------------------------------------------------------------------------------

// Available GPIO alternate pin functions

static uint16_t l_pinTypeMask =
    {
    SII_GPIO_ALT_MP_MUTE            |
    SII_GPIO_ALT_RP_MUTE            |
    SII_GPIO_ALT_MHL_CABLE_CONN     |
    SII_GPIO_ALT_EXT_RESET          |
    SII_GPIO_ALT_EXT_CLK            |
    0
    };

//------------------------------------------------------------------------------
//  GPIO Driver Instance Data
//------------------------------------------------------------------------------

uint16_t    l_statusFlags = 0;

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioInitialize
// Description:
//
// Returns:     TRUE if no problem
//------------------------------------------------------------------------------

bool_t SiiDrvGpioInitialize ( void )
{

    l_statusFlags = 0;

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioStatus
// Description: Returns a status flag word containing GPIO driver-specific
//              information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the GPIO Driver
//------------------------------------------------------------------------------

uint16_t SiiDrvGpioStatus ( void )
{
    uint16_t statusFlags = l_statusFlags;

    l_statusFlags = 0;          // INT flags only valid first time it is read.
    return( statusFlags );
}
//------------------------------------------------------------------------------
// Function:    SiiDrvGpioPinType
// ! @brief     Enable/disable the passed pin alternative function
// Parameters:  pins -- GPIO pins to be configured, GPIO0 == 0x0001,
//                      GPIO1 == 0x0002, etc.
//              type -- one of SiiGpioTypes_t
// Returns:     true if success, false if requested type not available for
//              one or more of the specified pin(s).
//------------------------------------------------------------------------------

bool_t SiiDrvGpioPinType ( uint16_t pins, SiiGpioPinTypes_t pinType )
{
    bool_t success = true;
    uint16_t pinValue = 0;
    uint8_t pinsLo;

    pinsLo = (uint8_t)(pins);

    if ( pinType == SII_GPIO_STANDARD )
    {
        // Clear all specified pins to GPIO functionality

        SiiRegBitsSet( REG_GPIO_HW_OUT_EN0, pinsLo, CLEAR_BITS );
    }
    else
    {
        // Determine if pins requested are valid against the pin type requested

        pinValue = pinType & pins & l_pinTypeMask;
        if ( pinValue != pins )
        {
            success = false;
            pinValue = 0;
        }
    }

    // Each pin has only one possible function

    if ( pinValue )
    {
        SiiRegBitsSet( REG_GPIO_HW_OUT_EN0, (uint8_t)pinValue, SET_BITS );
    }
    else
    {
        success = false;
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioPinConfigure
// ! @brief     Configure the passed pins as specified by the mode.
// Parameters:  pins -- GPIO pins to be configured, GPIO0 == 0x0001,
//                      GPIO1 == 0x0002, etc.
//              mode -- logical 'OR' of SiiGpioModes_t values
//------------------------------------------------------------------------------

void SiiDrvGpioPinConfigure ( uint16_t pins, SiiGpioModes_t pinMode )
{
    uint8_t pinsLo;

    pinsLo = (uint8_t)(pins);

    // Set the pull up/down mode requested

    if ( pinMode & SII_GPIO_PU )
    {
        SiiRegBitsSet( REG_GPIO_PAD_PU0, pinsLo, SET_BITS );
        SiiRegBitsSet( REG_GPIO_PAD_PE0, pinsLo, SET_BITS );
    }
    else if ( pinMode & SII_GPIO_PD )
    {
        SiiRegBitsSet( REG_GPIO_PAD_PU0, pinsLo, CLEAR_BITS );
        SiiRegBitsSet( REG_GPIO_PAD_PE0, pinsLo, SET_BITS );
    }
    else
    {
        SiiRegBitsSet( REG_GPIO_PAD_PE0,   pinsLo, CLEAR_BITS );
    }

    // Pin direction INPUT

    if ( pinMode & SII_GPIO_INPUT )
    {
        SiiRegBitsSet( REG_GPIO_DIR_EN0,     pinsLo, SET_BITS );

        // Inputs can also be interrupts

        if ( pinMode & ( SII_GPIO_INT_RISING_EDGE | SII_GPIO_INT_FALLING_EDGE ))
        {
            // Either edge or both...

            if ( pinMode & SII_GPIO_INT_RISING_EDGE )
            {
                SiiRegBitsSet( REG_GPIO_INT_EDGE_RISE0,   pinsLo, SET_BITS );
                SiiRegBitsSet( REG_INTR9_MASK,            pinsLo, SET_BITS );
            }
            if ( pinMode & SII_GPIO_INT_FALLING_EDGE )
            {
                SiiRegBitsSet( REG_GPIO_INT_EDGE_FALL0,   pinsLo, SET_BITS );
                SiiRegBitsSet( REG_INTR9_MASK,            pinsLo, SET_BITS );
            }
        }
        // Or not...
        else
        {
            SiiRegBitsSet( REG_INTR9_MASK,     pinsLo, CLEAR_BITS );
        }
    }

    // Pin direction OUTPUT

    else
    {
        SiiRegBitsSet( REG_GPIO_DIR_EN0,   pinsLo, CLEAR_BITS );

        // Outputs are never interrupts

        SiiRegBitsSet( REG_INTR9_MASK,     pinsLo, CLEAR_BITS );
    }
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioPinRead
//! @brief      Read the GPIO pins specified
// Parameters:  pins -- GPIO pins to be read, GPIO0 == 0x0001,
//                      GPIO1 == 0x0002, etc.
//------------------------------------------------------------------------------

uint16_t    SiiDrvGpioPinRead ( SiiGpioPins_t pins )
{
    uint8_t     pinsLo;
    uint16_t    pinValue = 0;

    pinsLo = (uint8_t)(pins);

    if ( pinsLo )
    {
        pinValue = SiiRegRead( REG_GPIO_READ_0 ) & pinsLo;
    }

    return( pinValue );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioPinWrite
//! @brief      Read the GPIO pins specified
// Parameters:  pins -- GPIO pins to be read, GPIO0 == 0x0001,
//                      GPIO1 == 0x0002, etc.
//------------------------------------------------------------------------------

void SiiDrvGpioPinWrite ( SiiGpioPins_t pins, uint16_t pinValue )
{
    uint8_t pinsLo;

    pinsLo = (uint8_t)(pins);

    if ( pinsLo )
    {
        SiiRegModify( REG_GPIO_WRITE_0, pinsLo, (uint8_t)pinValue );
    }
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioPinIntStatus
//! @brief      Read the interrupt status of the specified GPIO pins
// Parameters:  pins -- GPIO pins to be read, GPIO0 == 0x0001,
//                      GPIO1 == 0x0002, etc.
//------------------------------------------------------------------------------

uint16_t    SiiDrvGpioPinIntStatus ( SiiGpioPins_t pins )
{
    uint8_t     pinsLo;
    uint16_t    pinValue = 0;

    pinsLo = (uint8_t)(pins);

    if ( pinsLo )
    {
        pinValue = SiiRegRead( REG_INTR9 ) & pinsLo;
    }

    return( pinValue );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioPinIntClear
//! @brief      Clear the interrupt status of the specified GPIO pins
// Parameters:  pins -- GPIO pins to be read, GPIO0 == 0x0001,
//                      GPIO1 == 0x0002, etc.
//------------------------------------------------------------------------------

void SiiDrvGpioPinIntClear ( SiiGpioPins_t pins )
{
    uint8_t pinsLo;

    pinsLo = (uint8_t)(pins);

    if ( pinsLo )
    {
        SiiRegBitsSet( REG_INTR9, pinsLo, SET_BITS );
    }
}

//------------------------------------------------------------------------------
// Function:    SiiDrvGpioProcessInterrupts
// Description:
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SiiDrvGpioProcessInterrupts ( void )
{    
    uint16_t    intStatus = 0;



    // Check GPIO interrupts.

    intStatus = SiiDrvGpioPinIntStatus( SII_GPIO_PIN_0 );

    // Update a change in GPIO 4 status.

 	Printf("\r\nSiiDrvGpioProcessInterrupts\n");
    if ( intStatus )
    {
        SiiDrvGpioPinIntClear( SII_GPIO_PIN_0 );
    }

    if ( intStatus )
    {
        l_statusFlags |= intStatus;
      //  SiiSchedSendSignal(SI_EVENT_ID_GPIO_INT, 0);
    }
}



