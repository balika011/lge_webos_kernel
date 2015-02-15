//***************************************************************************
//!file     si_platform.h
//!brief    Silicon Image SiI9687 Starter Kit board Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if !defined __SI_PLATFORM_H__
#define __SI_PLATFORM_H__

#include "si_common.h"
#include "si_gpio.h"
#include "si_logger.h"

typedef enum _SiiSwitchNumbers_t
{
    SW_1,
    SW_2,
    SW_3,
    SW_4,
    SW_5,
    SW_6,
    SW_7,
} SiiSwitchNumbers_t;

//------------------------------------------------------------------------------
//  Board function result status codes
//------------------------------------------------------------------------------

#define MSG_PLATFORM       (MSG_DBG | DBGF_TS | DBGF_CN), DBG_BRD

typedef enum _SiiPlatformStatus_t
{
    PLATFORM_SUCCESS,
    PLATFORM_FAIL,              // General fail
    PLATFORM_INVALID_PARAMETER,
    PLATFORM_I2C_READ_FAIL,
    PLATFORM_I2C_WRITE_FAIL,
} SiiPlatformStatus_t;

//------------------------------------------------------------------------------
//  Platform Instance Data
//------------------------------------------------------------------------------

typedef struct BoardInstanceData
{
    int                 structVersion;
    SiiPlatformStatus_t lastResultCode;     // Contains the result of the last API function called
    uint16_t            statusFlags;

    bool_t              i2CError;	        // I2C error pass/fail
    uint32_t            i2cErrorCode;       // Sticky flags for I2C I/O failure
    uint8_t             dbgMsgLevel;

    bool_t              i2c16bitAddr;       // 16 bit address flag for I2C transactions
    bool_t              i2cNoInterimStops;  // True to avoid stop condition followed by start condition immediately

    bool_t              hardwareInt;        // Set by interrupt handler from H/W INT pin

    JoystickState_t     jstate;             // Last status of Joystick
}	SiiBoardInstanceData_t;

extern SiiBoardInstanceData_t platform;

//------------------------------------------------------------------------------
//  Platform I/O functions
//------------------------------------------------------------------------------


bool_t  SiiPlatformInitialize( void );
uint32_t SiiPlatformLastI2cStatus( void );
int_t   SiiPlatformGetLastResult( void );

bool_t  SiiPlatformPowerSwitchGet( void );
uint8_t SiiPlatformReadRotary( uint8_t iWantItNow );
bool_t  SiiPlatformSwitchGet( SiiSwitchNumbers_t switchNum );
bool_t  SiiPlatformSiimonBusRequest ( void );
void    SiiPlatformSiimonBusGrant ( bool_t grant );

void    SiiPlatformAllInterruptsEnable ( void );
void    SiiPlatformAllInterruptsDisable ( void );

void    SiiPlatformInterruptEnable( void );
void    SiiPlatformInterruptDisable( void );
void    SiiPlatformInterruptHandler( void );
bool_t  SiiPlatformInterruptPinStateGet ( void );
bool_t  SiiPlatformInterruptStateGet( void );
void    SiiPlatformInterruptClear ( void );

void    SiiPlatformJstkGpioIntHandler ( void );
JoystickState_t SiiPlatformJstkStateGet ( void );

#endif // __SI_PLATFORM_H__
