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
 * @file si_drv_cra.c
 *
 * @brief CRA (common register access) driver
 *
 *****************************************************************************/
//#define SII_DEBUG 3

/***** #include statements ***************************************************/

#include "sii9777_platform_api.h"
#include "si_drv_cra_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(DrvCra);

/***** local macro definitions ***********************************************/

#define BITS_SET(inp, mask, bset)   ( (bSet) ? (inp | mask) : (inp & (~mask)) )
#define BITS_MOD(inp, mask, val)    ( (inp & (~mask)) | (val & mask) )

/***** local prototypes ******************************************************/

static void sInt32ToData( uint8_t bytes, uint8_t* pArr, uint32_t val );
static uint32_t sDataToInt32( uint8_t bytes, uint8_t* pArr );
static void sRegWrite( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t bytes, uint32_t val );
static uint32_t sRegRead( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t bytes );

/***** local data objects ****************************************************/

/***** public functions ******************************************************/

void SiiDrvCraCreate( uint8_t devId )
{
    devId = devId;
}

void SiiDrvCraDelete( uint8_t devId )
{
    devId = devId;
}

void SiiDrvCraWrite8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t val )
{
    sRegWrite(devId, addr, 1, val);
}

uint8_t SiiDrvCraRead8( uint8_t devId, SiiDrvCraAddr_t addr )
{
    return (uint8_t)sRegRead(devId, addr, 1);
}

void SiiDrvCraBitsSet8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t mask, bool_t bSet )
{
	uint8_t temp;

    temp = (uint8_t)sRegRead(devId, addr, 1);
    temp = BITS_SET(temp, mask, bSet);
    sRegWrite(devId, addr, 1, temp);
}

void SiiDrvCraBitsMod8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t mask, uint8_t val )
{ 
	uint8_t temp;

    temp = (uint8_t)sRegRead(devId, addr, 1);
    temp = BITS_MOD(temp, mask, val);
    sRegWrite(devId, addr, 1, temp);
}

void SiiDrvCraWrite16( uint8_t devId, SiiDrvCraAddr_t addr, uint16_t val )
{   
    sRegWrite(devId, addr, 2, val);
}

uint16_t SiiDrvCraRead16( uint8_t devId, SiiDrvCraAddr_t addr )
{
    return (uint16_t)sRegRead(devId, addr, 2);
}

void SiiDrvCraBitsSet16( uint8_t devId, SiiDrvCraAddr_t addr, uint16_t mask, bool_t bSet )
{
	uint16_t temp;

    temp = (uint16_t)sRegRead(devId, addr, 2);
    temp = BITS_SET(temp, mask, bSet);
    sRegWrite(devId, addr, 2, temp);
}

void SiiDrvCraBitsMod16( uint8_t devId, SiiDrvCraAddr_t addr, uint16_t mask, uint16_t val )
{
	uint16_t temp;

    temp = (uint16_t)sRegRead(devId, addr, 2);
    temp = BITS_MOD(temp, mask, val);
    sRegWrite(devId, addr, 2, temp);
}

void SiiDrvCraWrite24( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t val )
{
    sRegWrite(devId, addr, 3, val);
}

uint32_t SiiDrvCraRead24( uint8_t devId, SiiDrvCraAddr_t addr )
{  
    return (uint32_t)sRegRead(devId, addr, 3);
}

void SiiDrvCraBitsSet24( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, bool_t bSet )
{ 
	uint32_t temp;

    temp = (uint32_t)sRegRead(devId, addr, 3);
    temp = BITS_SET(temp, mask, bSet);
    sRegWrite(devId, addr, 3, temp);
}

void SiiDrvCraBitsMod24( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, uint32_t val )
{
	uint32_t temp;

    temp = (uint32_t)sRegRead(devId, addr, 3);
    temp = BITS_MOD(temp, mask, val);
    sRegWrite(devId, addr, 3, temp);
}

void SiiDrvCraWrite32( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t val )
{
    sRegWrite(devId, addr, 4, val);
}

uint32_t SiiDrvCraRead32( uint8_t devId, SiiDrvCraAddr_t addr )
{
    return (uint32_t)sRegRead(devId, addr, 4);
}

void SiiDrvCraBitsSet32( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, bool_t bSet )
{   
	uint32_t temp;

    temp = (uint32_t)sRegRead(devId, addr, 4);
    temp = BITS_SET(temp, mask, bSet);
    sRegWrite(devId, addr, 4, temp);
}

void SiiDrvCraBitsMod32( uint8_t devId, SiiDrvCraAddr_t addr, uint32_t mask, uint32_t val )
{
	uint32_t temp;

    temp = (uint32_t)sRegRead(devId, addr, 4);
    temp = BITS_MOD(temp, mask, val);
    sRegWrite(devId, addr, 4, temp);
}

void SiiDrvCraFifoWrite8( uint8_t devId, SiiDrvCraAddr_t addr, const uint8_t* pData, SiiDrvCraSize_t size )
{
	while( size-- )
	{
        sRegWrite(devId, addr, 1, *pData);
        pData++;
	}		
}

void SiiDrvCraFifoRead8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t* pData, SiiDrvCraSize_t size )
{
	while( size-- )
	{
        *pData = (uint8_t)sRegRead(devId, addr, 1);
        pData++;
	}		
}

void SiiDrvCraBlockWrite8( uint8_t devId, SiiDrvCraAddr_t addr, const uint8_t* pData, SiiDrvCraSize_t size )
{
    SiiPlatformHostBlockWrite(devId, addr, pData, size);
}

void SiiDrvCraBlockRead8( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t* pData, SiiDrvCraSize_t size )
{
    SiiPlatformHostBlockRead(devId, addr, pData, size);
}

/***** local functions *******************************************************/

static void sInt32ToData( uint8_t bytes, uint8_t* pArr, uint32_t val )
{
    while( bytes-- )
    {
        *pArr = (uint8_t)val;
        pArr++;
        val >>= 8;
    }
}

static uint32_t sDataToInt32( uint8_t bytes, uint8_t* pArr )
{
    uint32_t val = 0;

    pArr += bytes;
    while( bytes-- )
    {
        val <<= 8;
        pArr--;
        val += *pArr;
    }
    return val;
}

static void sRegWrite( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t bytes, uint32_t val )
{
    uint8_t tmp[4];

    sInt32ToData(bytes, tmp, val);
    SiiPlatformHostBlockWrite(devId, addr, tmp, bytes);
}

static uint32_t sRegRead( uint8_t devId, SiiDrvCraAddr_t addr, uint8_t bytes )
{
    uint8_t tmp[4];

    SiiPlatformHostBlockRead(devId, addr, tmp, bytes);
    return sDataToInt32(bytes, tmp);
}

/** END of File *********************************************************/
