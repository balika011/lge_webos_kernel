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
 * file si_platform.c
 *
 * @brief SII9679 platform implementation example
 *
 *****************************************************************************/

/***** #include statements ***************************************************/

//#include <time.h>
//#include <windows.h>
//#include <unistd.h>
#include "si_datatypes.h"
#include "sii9777_platform_api.h"
#include "sii9777rx_api.h"
#include "x_timer.h"
/***** local macro definitions ***********************************************/

/***** public functions ******************************************************/
extern void x_thread_delay(UINT32 u4Ms);
extern void HAL_GetRawTime(HAL_RAW_TIME_T* pRawTime);
void SiiPlatformSleepMsec( uint16_t mSec )
{
    x_thread_delay(mSec);
}
extern UINT16 SIF_X_Write(UINT8 u1CHannelID, UINT16 u2ClkDiv, UINT8 u1DevAddr, UINT8 u1WordAddrNum, UINT32 u4WordAddr,
                   UINT8 *pu1Buf, UINT16 u2ByteCnt);
extern UINT16 SIF_X_Read(UINT8 u1CHannelID, UINT16 u2ClkDiv, UINT8 u1DevAddr, UINT8 u1WordAddrNum, UINT32 u4WordAddr,
                  UINT8 *pu1Buf, UINT16 u2ByteCnt);
uint16_t SiiHalI2cBlockWrite(uint8_t hwId,uint16_t addr,const uint8_t *pData,uint16_t size )
{
	UINT16 u2Ret = 0;
	/*
	if(u1LogCtrl == 1)
	{
		printf("function: %s, hwID: %d, addr: %x,size:%d data: ",__FUNCTION__,hwId,addr,size);
		for(i=0;i<size;i++)
		{
			printf("%d ,",pData[i]);
		}
		printf("\n");
	}
	*/
    u2Ret = SIF_X_Write(0, 0x100, 0x40, 2, addr, pData, size);
	return u2Ret;
}
uint16_t SiiHalI2cBlockRead(uint8_t hwId,uint16_t addr,const uint8_t *pData,uint16_t size )
{
	UINT16 u2Ret = 0;
	
    u2Ret = SIF_X_Read(0, 0x100, 0x40, 2, addr, pData, size);	
	/*
	if(u1LogCtrl == 1)
	{
		printf("function: %s, hwID: %d, addr: %x,size:%d data: ",__FUNCTION__,hwId,addr,size);
		for(i=0;i<size;i++)
			{
				printf("%d ,",pData[i]);
			}
		printf("\n");
	}
	*/
	return u2Ret;
}

uint16_t SiiPlatformHostBlockWrite( uint8_t hwId, uint16_t addr, const uint8_t *pData, uint16_t size )
{
    return SiiHalI2cBlockWrite(hwId, addr, pData, size);
}

uint16_t SiiPlatformHostBlockRead( uint8_t hwId, uint16_t addr, uint8_t *pData, uint16_t size )
{
   return SiiHalI2cBlockRead(hwId, addr, pData, size);
}
uint32_t SiiPlatformTimeMilliGet( void )
{
	HAL_TIME_T rTime;
	HAL_GetTime(&rTime);
    return (rTime.u4Seconds*1000 + rTime.u4Micros/1000);
}
void SiiPlatformLogPutString( const char* pStr )
{
	printf("%s\n",pStr);
}

