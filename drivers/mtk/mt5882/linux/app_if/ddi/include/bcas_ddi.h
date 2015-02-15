/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 		bcas_ddi.h
 *
 *  header of BCAS DDI Fuction.
 *
 *
 *  @author		Yunam Kim (yunam.kim@lge.com)
 *  @author
 *  @version	1.0
 *  @date		2011.2.18
 *  @date
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _BCAS_DDI_H_
#define _BCAS_DDI_H_





/******************************************************************************
    전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include <osa_api.h>
#include <common.h>
#include "sdec_ddi.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
DTV_STATUS_T DDI_BCAS_InitCard(void);
DTV_STATUS_T DDI_BCAS_ResetCard(void);
int DDI_BCAS_IsCardInserted(void);
DTV_STATUS_T DDI_BCAS_RegisterEvent_CardAction( UINT32 (*pfn)(UINT8, UINT32) );



//static int _DDI_BCAS_ResetCard(NEXUS_SmartcardHandle smartcard);
int _DDI_BCAS_ResetCard(void);
void DDI_BCAS_Deactivate(void);//Deactivate smartcard
void _DDI_BCAS_DecodeATR(void);
//void DDI_BCAS_TransferAPDU (UINT8	cardIndex, UINT8 *commAPDU, UINT16 commAPDULen, UINT8 *respAPDU, UINT16 *respAPDULen);

DTV_STATUS_T DDI_BCAS_TransferAPDU(UINT8 cardIndex,
				   UINT8 *commandAPDU,
				   UINT32 commandAPDULen,
				   UINT8 *responseAPDU,
				   UINT32 *responseAPDULen,
				   UINT32 maxresponseAPDULen);


#if (PLATFORM_TYPE == MTK_PLATFORM)
void DDI_BCAS_ClearKey(UINT8 Index, UINT8 keyType);
#else	// LG_PLATFORM
void DDI_BCAS_ClearKey(UINT8 ubyDescramblerIndex, UINT8 keyType, UINT16 uwPidHandle);
#endif


void DDI_BCAS_SetKey(UINT8 ubyDescramblerIndex, UINT16 uwPidHandle, UINT8 keyType, UINT8 *Key);
void DDI_BCAS_ClearPid(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex, UINT16 uwPidHandle);
void DDI_BCAS_InitDescrambler(UINT8 Index);
void DDI_BCAS_ResetDescrambler(UINT8 Index);
void DDI_BCAS_SetCBC(UINT8 Index, UINT8 *CBC );
void DDI_BCAS_SetSysKey(UINT8 Index, UINT8 *SysKey);

#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_BCAS_SetPid (
							SDEC_CHANNEL_T ch,
							UINT8 ubyIndex,
							UINT16 PidToDescramble,
							SDEC_PID_TYPE_T ePidType,
							BOOLEAN bEnableDescrambling,
							BOOLEAN bIsRecordingPID,
							UINT16 *puwPidHandle );

DTV_STATUS_T DDI_BCAS_IsPidActive(SDEC_CHANNEL_T ch,
								  UINT16 uwPid, 
								  SDEC_PID_TYPE_T ePidType, 
								  BOOLEAN bIsRecordingPID,
								  UINT16 *pubyPidIndex );

#else	// LG_PLATFORM

DTV_STATUS_T DDI_BCAS_SetPid(
					  SDEC_CHANNEL_T ch,
					  UINT8 ubyDescramblerIndex,
					  UINT16 uwPid,
					  SDEC_PID_TYPE_T ePidType,
					  BOOLEAN bEnableDescrambling,
					  UINT16 *puwPidHandle);

DTV_STATUS_T DDI_BCAS_IsPidActive(
								SDEC_CHANNEL_T 	ch,
								UINT16 			uwPid, 
								SDEC_PID_TYPE_T ePidType, 
								UINT16 *pubyPidIndex );
#endif

DTV_STATUS_T DDI_BCAS_EnableDescrambler(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex, UINT16 uwPidHandle, BOOLEAN bEnableDescrambler);



#endif /*_BCAS_DDI_H_ */

