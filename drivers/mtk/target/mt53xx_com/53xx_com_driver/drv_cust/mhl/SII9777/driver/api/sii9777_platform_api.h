/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file si_platform_api.h
 *
 * @brief Minimum Platform API function set for SII9679 Driver
 *
 *****************************************************************************/

#ifndef SI_PLATFORM_API_H
#define SI_PLATFORM_API_H

#ifdef __cplusplus
extern "C"{
#endif

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public macro definitions **********************************************/

/***** public functions ******************************************************/

/* Sleep */ 
extern void SiiPlatformSleepMsec( uint16_t mSec );

/* Host interface write data */
extern uint16_t SiiPlatformHostBlockWrite( uint8_t devId, uint16_t addr, const uint8_t *pData, uint16_t size );

/* Host interface read data */
extern uint16_t SiiPlatformHostBlockRead( uint8_t devId, uint16_t addr, uint8_t *pData, uint16_t size );

/* Receive number of milli second ticks passed */ 
extern uint32_t SiiPlatformTimeMilliGet( void );

/* text logger */ 
extern void SiiPlatformLogPutString( const char* pStr );

#ifdef __cplusplus
}
#endif

#endif // SI_PLATFORM_API_H
