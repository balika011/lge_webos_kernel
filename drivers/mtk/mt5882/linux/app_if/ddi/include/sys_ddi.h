/******************************************************************************

 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA

 *   Copyright(c) 2008 by LG Electronics Inc.

 *

 *   All rights reserved. No part of this work may be reproduced, stored in a

 *   retrieval system, or transmitted by any means without prior written

 *   permission of LG Electronics Inc.

 *****************************************************************************/

/** @file sys_ddi.h
  *
  *  System related header file
  *
  *  @author		dhjung(dhjung77@lge.com)
  *  @version		 1.0
  *  @date		2009.10.08
  *  @note
  *  @see			 sys_ddi.c
*/

#ifndef _SYS_DDI_H_
#define _SYS_DDI_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#include "common.h"
#include "osa_api.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/
#define MAX_AVAIL_MMAP		10

#define WATCHDOG_TIMER_INTERVAL_SEC		(5)// 5 sec

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/**
* Available Memory information
*
* @see	API_DDM_SYS_GetAvailMmap(void)
*/
typedef struct
{
	UINT32	avail_mmapcount;
	UINT8 	*start_addr[MAX_AVAIL_MMAP];
	UINT32 	size[MAX_AVAIL_MMAP];
} AVAIL_MMAP_T;

/**
* Watch dog status
*
* @see	DDI_SYS_GetPreviousWatchDogResetStatus(void)
*/
typedef enum WDT_RESET_STATUS
{
	WDT_STAT_RESET_BY_OTHER = 0,
	WDT_STAT_RESET_BY_WATCHDOG
} WDT_RESET_STATUS_T;

/**
* DRAM bandwidth  information
*
* @see	
*/
typedef struct
{
    UINT32  totalBytes_chA;
    UINT32  percent_chA;                //ratio for how much bandwidth of the channel A is used.
    UINT32  totalBytes_chB;
    UINT32  percent_chB;                //ratio for how much bandwidth of the channel B is used.
} DRAM_BW_INFO_T;

/******************************************************************************
	Extern 전역변수와 함수 prototype 선언
	(Extern Variables & Function Prototype Declarations)
*******************************************************************************/
AVAIL_MMAP_T *DDI_SYS_GetMmapAvailable(void);
AVAIL_MMAP_T *DDI_SYS_GetMmapAvailable_FactoryOTA(void);
void *DDI_SYS_GetMmapBtPool(UINT32 ssize);
BOOLEAN DDI_SYS_ReadSpiBoot(UINT32 size, UINT8* buf, UINT32 addr);
BOOLEAN DDI_SYS_WriteSpiBoot(UINT32 size, UINT8* buf, UINT32 addr);
BOOLEAN DDI_SYS_EraseSpiBoot(UINT32 size, UINT32 addr);
DTV_STATUS_T DDI_SYS_AddMtd(void);
DTV_STATUS_T DDI_SYS_GetDeviceDynamicMemInfo(UINT32* ptotal, UINT32* pfree);

#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_SYS_SetWatchDogTimer(BOOLEAN bEnable, UINT32 timer);
void  DDI_SYS_GetWatchDogTimerStatus(BOOLEAN *pbEnable, UINT32 *ptimer);
WDT_RESET_STATUS_T DDI_SYS_GetPreviousWatchDogResetStatus(void);
DTV_STATUS_T DDI_SYS_SetWOL(BOOLEAN bEnableWol, BOOLEAN bEntryStandby);
#endif

DTV_STATUS_T DDI_SYS_SetDramBWEnable(BOOLEAN bOnOff);
DTV_STATUS_T DDI_SYS_GetDramBWInfo(DRAM_BW_INFO_T *pstBWInfo);
DTV_STATUS_T DDI_SYS_SetJtagPort(BOOLEAN bEnable);


#endif  /* _SYS_DDI_H_ */
