//***************************************************************************
//!file     si_gpio.c
//!brief    Platform GPIO driver, ARM (Stellaris) version.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "hw_gpio.h"
#undef GPIO_LOCK_KEY
#include "si_platform.h"
#include "si_osal_timer.h"
#include "si_i2c.h"
#include "si_gpio.h"
#include "si_arm.h"


//*****************************************************************************
//
// The base addresses of all the GPIO modules.  Both the APB and AHB apertures
// are provided.
//
//*****************************************************************************

static const unsigned long g_pulGPIOBaseAddrs[] =
{
    GPIO_PORTA_BASE, GPIO_PORTA_AHB_BASE,
    GPIO_PORTB_BASE, GPIO_PORTB_AHB_BASE,
    GPIO_PORTC_BASE, GPIO_PORTC_AHB_BASE,
    GPIO_PORTD_BASE, GPIO_PORTD_AHB_BASE,
    GPIO_PORTE_BASE, GPIO_PORTE_AHB_BASE,
    GPIO_PORTF_BASE, GPIO_PORTF_AHB_BASE,
    GPIO_PORTG_BASE, GPIO_PORTG_AHB_BASE,
    GPIO_PORTH_BASE, GPIO_PORTH_AHB_BASE,
    GPIO_PORTJ_BASE, GPIO_PORTJ_AHB_BASE,
};

//*****************************************************************************
//
//! Configures the alternate function of a GPIO pin.
//!
//! \param ulPinConfig is the pin configuration value, specified as one of the
//! \b GPIO_P??_??? values.
//!
//! This function configures the pin mux that selects the peripheral function
//! associated with a particular GPIO pin.  Only one peripheral function at a
//! time can be associated with a GPIO pin, and each peripheral function should
//! only be associated with a single GPIO pin at a time (despite the fact that
//! many of them can be associated with more than one GPIO pin).
//!
//! \note This function is only valid on Tempest-class devices.
//!
//! \return None.
//
//*****************************************************************************

void SiiPlatformGPIOPinConfigure ( unsigned long ulPinConfig )
{
    unsigned long ulBase, ulShift;

    // Extract the base address index from the input value.

    ulBase = (ulPinConfig >> 16) & 0xff;

    // Get the base address of the GPIO module, selecting either the APB or the
    // AHB aperture as appropriate.

    if (HWREG(SYSCTL_GPIOHSCTL) & (1 << ulBase))
    {
        ulBase = g_pulGPIOBaseAddrs[(ulBase << 1) + 1];
    }
    else
    {
        ulBase = g_pulGPIOBaseAddrs[ulBase << 1];
    }

    // Extract the shift from the input value.

    ulShift = (ulPinConfig >> 8) & 0xff;

    // Write the requested pin muxing value for this GPIO pin.

    HWREG(ulBase + GPIO_O_PCTL) = ((HWREG(ulBase + GPIO_O_PCTL) &
                                    ~(0xfUL << ulShift)) |
                                   ((ulPinConfig & 0xf) << ulShift));

}

//*****************************************************************************
//
//! Configures pin(s) for use as GPIO inputs.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ucPins is the bit-packed representation of the pin(s).
//!
//! The GPIO pins must be properly configured in order to function correctly as
//! GPIO inputs; this is especially true of Fury-class devices where the
//! digital input enable is turned off by default.  This function provides the
//! proper configuration for those pin(s).
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************

static void SiiPlatformGPIOPinTypeGPIOInput( unsigned long ulPort, unsigned char ucPins, unsigned long ulPinType )
{
    //
    // Make the pin(s) be inputs.
    //
    ROM_GPIODirModeSet(ulPort, ucPins, GPIO_DIR_MODE_IN);
  
    
    //
    // Set the pad(s) for standard push-pull operation.
    //
    ROM_GPIOPadConfigSet(ulPort, ucPins, GPIO_STRENGTH_2MA, ulPinType );
}

//------------------------------------------------------------------------------
// Function:    Pca9557Read
// Description: Read the PCA9557PW device at the passed I2C device ID.
//              Mask the result with bitMask and return the result.
// Parameters:  devId - I2C address of port extender to read.
//              bitMask - Value to AND with port extender data before returning.
// Returns:     Value of selected PCA9557PW, masked with bitMask
//------------------------------------------------------------------------------

static uint8_t  Pca9557Read ( uint8_t devId, uint8_t bitMask )
{
    uint8_t value;

    value = SiiPlatformI2cReadByte( 0, devId, PCA_INPUT );

    return( value & bitMask );
}

//------------------------------------------------------------------------------
// Function:    Pca9557ReadModifyWrite
// Description: Read/Modify/Write the PCA9557PW device at the passed I2C device
//              ID.
// Parameters:  devId - I2C address of port extender to read.
//              bitMask - Value to NAND with port extender data before writeback.
//              bitValue- Value to OR with port extender data before writeback.
// Returns:     none
//------------------------------------------------------------------------------

static void Pca9557ReadModifyWrite ( uint8_t devId, uint8_t bitMask, uint8_t bitValue  )
{
    uint8_t value;


    value = SiiPlatformI2cReadByte( 0, devId, PCA_INPUT );
    value &= ~bitMask;
    value |= (bitValue & bitMask);

    SiiPlatformI2cWriteByte( 0, devId, PCA_OUTPUT, value );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Resets U1 by toggling h/w reset line \__/ for 5 ms.
//!
//!             Note: the function blocks for 5 ms
//-------------------------------------------------------------------------------------------------

void SiiPlatformGpioMainReset ( void )
{
    // Switch pin to output mode (SWWA:24700)
    ROM_GPIOPinTypeGPIOOutput( GPIO_PORTA_BASE, GPIO_PIN_2 );

    ROM_GPIOPinWrite( GPIO_PORTA_BASE, GPIO_PIN_2, 0 );
    SiiOsTimerWait(5);
    ROM_GPIOPinWrite( GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2 );

    // Switch pin back to input mode (SWWA:24700)
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_TYPE_OD );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Resets GPIO extenders and LCD display by toggling reset line \__/ for a short time.
//-------------------------------------------------------------------------------------------------

void SiiPlatformGpioPeripheryReset ( void )
{
    int i;
    const int delay = 1000; // ~500us

    // Non-timer delay
    for (i = 0; i < delay; i++)
    {
        ROM_GPIOPinWrite( GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1 );
    }
    for (i = 0; i < delay; i++)
    {
        ROM_GPIOPinWrite( GPIO_PORTD_BASE, GPIO_PIN_1, 0 );
    }
    ROM_GPIOPinWrite( GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1 );
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformGpioLedPorts
// Description: Turn port LEDs on or off
// Parameters:  portBits
// Returns:     none
//------------------------------------------------------------------------------

void SiiPlatformGpioLedPorts ( uint8_t portBits )
{
    Pca9557ReadModifyWrite( I2C_SADDR_38, BIT3|BIT2|BIT1|BIT0, ~portBits & LED_STATUS_MASK );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformGpioLedStatus1
// Description: Turn Status LED 1 on or off
// Parameters:  ledOn   - true: turn LED on
// Returns:     none
//------------------------------------------------------------------------------

void SiiPlatformGpioLedStatus1 ( bool_t ledOn )
{
    Pca9557ReadModifyWrite( I2C_SADDR_3A, BIT0, (ledOn) ? LED_ON : LED_OFF );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformGpioLedStatus2
// Description: Turn Status LED 2 on or off
// Parameters:  ledOn   - true: turn LED on
// Returns:     none
//------------------------------------------------------------------------------

void SiiPlatformGpioLedStatus2 ( bool_t ledOn )
{
    Pca9557ReadModifyWrite( I2C_SADDR_3A, BIT1, ledOn ? LED_ON : LED_OFF );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformGpioLedArc
// Description: Turn ARC LED on or off
// Parameters:  ledOn   - true: turn LED on
// Returns:     none
//------------------------------------------------------------------------------

void SiiPlatformGpioLedArc ( bool_t ledOn )
{
    Pca9557ReadModifyWrite( I2C_SADDR_38, BIT5, ledOn ? LED_ON : LED_OFF );
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformGpioReadSwitch
// Description: Read the state of a switch on the Starter kit board
// Parameters:  gpioSw: Upper nibble is the read function switch case, which
//                      chooses the GPIO bank
//                      Lower nibble is the bit position in the selected GPIO bank
// Returns:     true if Switch position closed, false if open
//------------------------------------------------------------------------------

bool_t SiiPlatformGpioReadSwitch ( GpioSw_t gpioSw )
{
    bool_t result = false;

    switch ( (gpioSw >> 8) & 0xFF )
    {
        case 0:     // PORT A Bank
            result = (ROM_GPIOPinRead( GPIO_PORTA_BASE, gpioSw & 0xFF ) == 0);
            break;
        case 1:     // PORT B Bank
            result = (ROM_GPIOPinRead( GPIO_PORTB_BASE, gpioSw & 0xFF ) == 0);
            break;
        case 2:     // PORT C Bank
            result = (ROM_GPIOPinRead( GPIO_PORTC_BASE, gpioSw & 0xFF ) == 0);
            break;
        case 3:     // PORT D Bank
            result = (ROM_GPIOPinRead( GPIO_PORTD_BASE, gpioSw & 0xFF ) == 0);
            break;

        case 0x10:  // These switch positions are on an I/O extender
            result = (Pca9557Read( I2C_SADDR_3A, gpioSw & 0xFF ) != 0 );
            break;

        case 0x20:  // These switch positions are on an I/O extender
            result = (Pca9557Read( I2C_SADDR_38, gpioSw & 0xFF ) != 0 );
            break;

        default:
            break;
    }

    return( result );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return status of Joystick.
//!
//!             When Joystick is not touched, returns JNAV_NONE. Otherwise returns status
//!             corresponding to pressed button and repeating indicator. E.g., JNAV_LEFT is
//!             returned when left button pressed for a first time. JNAV_LEFT_RPT will be
//!             returned for second and all following consecutive presses.
//!
//! @return     JNAV_NONE or status corresponding to one of the Joystick's buttons.
//-------------------------------------------------------------------------------------------------

JoystickState_t SiiPlatformGpioReadJoystick(void)
{
    JoystickState_t newState = JNAV_NONE;
    int_t nav_state;
    int_t select;

    nav_state = ~ROM_GPIOPinRead( GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    select = ~ROM_GPIOPinRead( GPIO_PORTA_BASE, GPIO_PIN_4);

    if (nav_state & GPIO_PIN_4)
    {
        newState = JNAV_LEFT;
    }
    else if (nav_state & GPIO_PIN_5)
    {
        newState = JNAV_DOWN;
    }
    else if (nav_state & GPIO_PIN_6)
    {
        newState = JNAV_RIGHT;
    }
    else if (nav_state & GPIO_PIN_7)
    {
        newState = JNAV_UP;
    }
    else if (select & GPIO_PIN_4)
    {
        newState = JNAV_SELECT;
    }

    return newState;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Control VBUS power for port specified
//!
//! @param[in]  vbusPort - port index (only port 3 has the function)
//!
//! @return     success flag indicating if port's VBUS power control has been done.
//-------------------------------------------------------------------------------------------------

bool_t SiiPlatformGpioVbusCtrl ( uint8_t vbusPort, uint8_t ctrl )
{
    bool_t success = false;
	// Only port 3 has VBUS control circuitry
	if (vbusPort == MHL_RPWR_EN3)
	{
		Pca9557ReadModifyWrite( I2C_SADDR_3A, BIT3, ctrl ? BIT3 : 0x00 );  // active high switch
		success = true;
	}
    return( success );
}
void SiiPlatformGpioSetPower (uint8_t pwrType, uint8_t pwrCtrl )
{
    // Power is controlled by Standby MCU
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformGpioInit
// Description:
// Parameters:
// Returns:
//------------------------------------------------------------------------------

void SiiPlatformGpioInit (void)
{
    // Enable the GPIO ports used for the board UI
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA );
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );

    // Configure the GPIO pin used to reset the I/O extenders
    ROM_GPIOPadConfigSet( GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU );
    ROM_GPIOPinWrite( GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1 );
    ROM_GPIOPinTypeGPIOOutput( GPIO_PORTD_BASE, GPIO_PIN_1 );
    ROM_GPIOPinWrite( GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1 );
    // Reset peripherals
    SiiPlatformGpioPeripheryReset();

    // Port B, bit 7 is special, because it CAN be an NMI pin, and TI didn't
    // want it to be programmed accidently.  So they 'locked' it and it takes a
    // special sequence to unlock so that we can actually change any of its features.
    HWREG( GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD; // LOCK code
    HWREG( GPIO_PORTB_BASE + GPIO_O_CR)   = 0x000000FF;       // Enable all 8 bits
    HWREG( GPIO_PORTB_BASE + GPIO_O_LOCK) = 0x00000000;       // re-lock

    // Configure SW2 GPIOs (remaining 4 pins are from an extender)
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_TYPE_STD_WPU );
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_TYPE_STD_WPU );
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_TYPE_STD_WPU );

    // U1 RESET# output
    ROM_GPIOPadConfigSet( GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD );
    //SWWA:24700 Keep Reset pin in input mode to prevent driving currents to come out
    //ROM_GPIOPinWrite( GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2 );
    //ROM_GPIOPinTypeGPIOOutput( GPIO_PORTA_BASE, GPIO_PIN_2 );

    // CEC_A0 input
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_TYPE_STD_WPU );

    // Configure I2C1 Master pins
    ROM_GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);

    // Configure the GPIO pins used for the Rotary Switch.
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5, GPIO_PIN_TYPE_STD_WPU );

    // Configure the GPIO pin used for the Tx0 HPD Input.
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_TYPE_STD_WPU  );

    // Configure the GPIO pins used for Joystick controls and set up interrupts
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_4);
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_TYPE_STD_WPU );
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_PORTA_BASE, GPIO_PIN_4 , GPIO_PIN_TYPE_STD_WPU );
    ROM_GPIOIntTypeSet( GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_FALLING_EDGE );
    ROM_GPIOIntTypeSet( GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE );
    ROM_IntEnable(INT_GPIOC);
    ROM_IntEnable(INT_GPIOA);
    ROM_GPIOPinIntEnable(GPIO_PORTA_BASE, GPIO_PIN_4);
    ROM_GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    // Configure the GPIO pin used external Interrupt from the SIMG chip
    SiiPlatformGPIOPinTypeGPIOInput( GPIO_INT_PORT_BASE, GPIO_INT_PIN_IDX, GPIO_PIN_TYPE_STD_WPU );
    ROM_GPIOIntTypeSet( GPIO_INT_PORT_BASE, GPIO_INT_PIN_IDX, GPIO_FALLING_EDGE );
    ROM_IntEnable(GPIO_INT_PORT);

    // Extended GPIO chip initialization

    SiiPlatformI2cWriteByte( 0, I2C_SADDR_38, PCA_POLARITY, PCA_DEFAULT_POLARITY );
    SiiPlatformI2cWriteByte( 0, I2C_SADDR_38, PCA_CONFIG, 0x00 );                      // 7:0 output
    SiiPlatformI2cWriteByte( 0, I2C_SADDR_38, PCA_OUTPUT, PCA_DEFAULT_OUTPUT );

    SiiPlatformI2cWriteByte( 0, I2C_SADDR_3A, PCA_POLARITY, (BIT7|BIT6|BIT5|BIT4) );   // Upper nibble inverted inputs
    SiiPlatformI2cWriteByte( 0, I2C_SADDR_3A, PCA_CONFIG, (BIT7|BIT6|BIT5|BIT4) );     // 7:4 input, 3:0 output
    SiiPlatformI2cWriteByte( 0, I2C_SADDR_3A, PCA_OUTPUT, PCA_DEFAULT_OUTPUT );
}


