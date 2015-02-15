
/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 		hal_spi.h
 *
 *  header of HAL SPI Fuction.
 *
 *
 *  @author		jun.kong (jun.kong@lge.com)
 *  @author
 *  @version	1.0
 *  @date		2013.10.28
 *  @date
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _SPI_HAL_H_
#define _SPI_HAL_H_





/******************************************************************************
    전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include "hal_common.h"


/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/
extern SINT32 gHalSpiLogmFd;

#define HAL_SPI_LOGM 						"hal-spi"
#define HAL_SPI_ERROR(fmt,args...)		KADP_LOGM_PRINT(gHalSpiLogmFd, LX_LOGM_LEVEL_ERROR, 	fmt, ##args)
#define HAL_SPI_WARN(fmt,args...)		KADP_LOGM_PRINT(gHalSpiLogmFd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define HAL_SPI_NOTI(fmt,args...)		KADP_LOGM_PRINT(gHalSpiLogmFd, LX_LOGM_LEVEL_NOTI, 	fmt, ##args)
#define HAL_SPI_INFO(fmt,args...)		KADP_LOGM_PRINT(gHalSpiLogmFd, LX_LOGM_LEVEL_INFO, 	fmt, ##args)
#define HAL_SPI_DEBUG(fmt,args...)		KADP_LOGM_PRINT(gHalSpiLogmFd, LX_LOGM_LEVEL_DEBUG, 	fmt, ##args)
#define HAL_SPI_TRACE(fmt,args...)		KADP_LOGM_PRINT(gHalSpiLogmFd, LX_LOGM_LEVEL_TRACE, 	fmt, ##args)
/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/

extern DTV_STATUS_T HAL_SPI_Init (void);
extern DTV_STATUS_T HAL_SPI_Config ( UINT8 channel , UINT8 mode, UINT8 transferbit, UINT32 speed );
extern DTV_STATUS_T HAL_SPI_OnOffFlag(BOOLEAN OnOff);
extern DTV_STATUS_T HAL_SPI_Write( UINT8 channel,  UINT8 *txBuf , UINT16 nBytes);
extern DTV_STATUS_T HAL_SPI_Read( UINT8 channel,  UINT8 *rxBuf,UINT16 nBytes);

extern UINT32 HAL_SPI_D13_Read( UINT32 addrToRead );
extern DTV_STATUS_T HAL_SPI_D13_Write( UINT32 addrToWrite, UINT32 data );
extern DTV_STATUS_T HAL_SPI_D13_WriteBurstMode( UINT32 addrToWrite, UINT8 *pBuf, UINT32 size );
extern DTV_STATUS_T HAL_SPI_D13_DataWrite( UINT8* pBuf, UINT16 size  );

extern UINT32 HAL_SPI_U14_Read( UINT32 addrToRead );
extern DTV_STATUS_T HAL_SPI_U14_Write( UINT32 addrToWrite, UINT32 data );
extern DTV_STATUS_T HAL_SPI_U14_WriteBurstMode( UINT32 addrToWrite, UINT8 *pBuf, UINT32 size );

#endif /*_SPI_HAL_H_ */


