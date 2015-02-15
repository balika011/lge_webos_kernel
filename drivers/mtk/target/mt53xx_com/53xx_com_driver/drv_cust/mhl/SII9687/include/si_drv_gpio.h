//***************************************************************************
//!file     si_drv_gpio.h
//!brief    Silicon Image GPIO driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_GPIO_DRV_H__
#define __SI_GPIO_DRV_H__
#include "si_common.h"

typedef enum _SiiGpioPinTypes_t
{
    SII_GPIO_ALT_MP_MUTE            = 0x0400,
    SII_GPIO_ALT_RP_MUTE            = 0x0200,
    SII_GPIO_ALT_MHL_CABLE_CONN     = 0x0010,
    SII_GPIO_ALT_EXT_RESET          = 0x0008,
    SII_GPIO_ALT_EXT_CLK            = 0x0004,
    SII_GPIO_STANDARD               = 0x8000
} SiiGpioPinTypes_t;

typedef enum _SiiGpioModes_t
{
    SII_GPIO_OUTPUT             = 0x0001,
    SII_GPIO_INPUT              = 0x0002,
    SII_GPIO_PU                 = 0x0004,
    SII_GPIO_PD                 = 0x0008,
    SII_GPIO_INT_RISING_EDGE    = 0x0010,
    SII_GPIO_INT_FALLING_EDGE   = 0x0020,
} SiiGpioModes_t;

typedef enum _SiiGpioPins_t
{
    SII_GPIO_PIN_0        = 0x0001,
    SII_GPIO_PIN_1        = 0x0002,
    SII_GPIO_PIN_2        = 0x0004,
    SII_GPIO_PIN_3        = 0x0008,
    SII_GPIO_PIN_4        = 0x0010,
    SII_GPIO_PIN_5        = 0x0020,
    SII_GPIO_PIN_6        = 0x0040,
    SII_GPIO_PIN_7        = 0x0080,
    SII_GPIO_PIN_8        = 0x0100,
    SII_GPIO_PIN_9        = 0x0200,
    SII_GPIO_PIN_10        = 0x0400,
} SiiGpioPins_t;

bool_t      SiiDrvGpioInitialize( void );
uint16_t    SiiDrvGpioStatus( void );
bool_t      SiiDrvGpioPinType( uint16_t pins, SiiGpioPinTypes_t pinType );
void        SiiDrvGpioPinConfigure( uint16_t pins, SiiGpioModes_t pinMode );
uint16_t    SiiDrvGpioPinRead( SiiGpioPins_t pins );
void        SiiDrvGpioPinWrite( SiiGpioPins_t pins, uint16_t pinValue );
uint16_t    SiiDrvGpioPinIntStatus( SiiGpioPins_t pins );
void        SiiDrvGpioPinIntClear( SiiGpioPins_t pins );

void        SiiDrvGpioProcessInterrupts ( void );

#endif // __SI_GPIO_DRV_H__
