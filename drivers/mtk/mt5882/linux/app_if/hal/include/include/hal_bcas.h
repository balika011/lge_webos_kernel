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
#ifndef _BCAS_HAL_H_
#define _BCAS_HAL_H_





/******************************************************************************
    전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
//#include <osa_api.h>
//#include <common.h>
//#include "sdec_ddi.h"
#include "hal_common.h"
#include "hal_sdec.h"

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
DTV_STATUS_T HAL_BCAS_InitCard(void);
DTV_STATUS_T HAL_BCAS_ResetCard(void);
int HAL_BCAS_IsCardInserted(void);
DTV_STATUS_T HAL_BCAS_RegisterEvent_CardAction( UINT32 (*pfn)(UINT8, UINT32) );
int HAL_BCAS_SMARTCARD_IsCardInserted(void);


//static int _HAL_BCAS_ResetCard(NEXUS_SmartcardHandle smartcard);
int _HAL_BCAS_ResetCard(void);
void HAL_BCAS_Deactivate(void);//Deactivate smartcard
void _HAL_BCAS_DecodeATR(void);
//void HAL_BCAS_TransferAPDU (UINT8	cardIndex, UINT8 *commAPDU, UINT16 commAPDULen, UINT8 *respAPDU, UINT16 *respAPDULen);

DTV_STATUS_T HAL_BCAS_TransferAPDU(UINT8 cardIndex,
				   UINT8 *commandAPDU,
				   UINT32 commandAPDULen,
				   UINT8 *responseAPDU,
				   UINT32 *responseAPDULen,
				   UINT32 maxresponseAPDULen);



void HAL_BCAS_ClearKey(UINT8 ubyDescramblerIndex, UINT8 keyType, UINT16 uwPidHandle);



void HAL_BCAS_SetKey(UINT8 ubyDescramblerIndex, UINT16 uwPidHandle, UINT8 keyType, UINT8 *Key);
void HAL_BCAS_ClearPid(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex, UINT16 uwPidHandle);
void HAL_BCAS_InitDescrambler(UINT8 Index);
void HAL_BCAS_ResetDescrambler(UINT8 Index);
void HAL_BCAS_SetCBC(UINT8 Index, UINT8 *CBC );
void HAL_BCAS_SetSysKey(UINT8 Index, UINT8 *SysKey);


DTV_STATUS_T HAL_BCAS_SetPid(
					  SDEC_CHANNEL_T ch,
					  UINT8 ubyDescramblerIndex,
					  UINT16 uwPid,
					  SDEC_PID_TYPE_T ePidType,
					  BOOLEAN bEnableDescrambling,
					  UINT16 *puwPidHandle);

DTV_STATUS_T HAL_BCAS_IsPidActive(
								SDEC_CHANNEL_T 	ch,
								UINT16 			uwPid, 
								SDEC_PID_TYPE_T ePidType, 
								UINT16 *pubyPidIndex );


DTV_STATUS_T HAL_BCAS_EnableDescrambler(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex, UINT16 uwPidHandle, BOOLEAN bEnableDescrambler);


DTV_STATUS_T HAL_BCAS_EnableDescrambler(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex, UINT16 uwPidHandle, BOOLEAN bEnableDescrambler);
DTV_STATUS_T HAL_BCAS_DESCRAMBLER_EnableDescrambler(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex,UINT16 uwPidHandle,BOOLEAN bEnableDescrambler);
void HAL_BCAS_DESCRAMBLER_InitDescrambler(UINT8 ubyDescramblerIndex);
void HAL_BCAS_DESCRAMBLER_ResetDescrambler(UINT8 ubyDescramblerIndex);
void HAL_BCAS_DESCRAMBLER_SetCBC(UINT8 ubyDescramblerIndex, UINT8 *CBC);
void HAL_BCAS_DESCRAMBLER_ClearCBC(UINT8 ubyDescramblerIndex);
void HAL_BCAS_DESCRAMBLER_SetKey(UINT8 ubyDescramblerIndex, UINT16 uwPidHandle, UINT8 keyType, UINT8 *key);
void HAL_BCAS_DESCRAMBLER_ClearPid(SDEC_CHANNEL_T ch, UINT8 ubyDescramblerIndex, UINT16 uwPidHandle);
void HAL_BCAS_DESCRAMBLER_SetSysKey(UINT8 ubyDescramblerIndex, UINT8 *SysKey);
void HAL_BCAS_DESCRAMBLER_ClearSysKey(UINT8 ubyDescramblerIndex);
DTV_STATUS_T HAL_BCAS_SMARTCARD_InitSmartCard(void);
DTV_STATUS_T HAL_BCAS_SMARTCARD_ResetSmartCard(void);
DTV_STATUS_T HAL_BCAS_SMARTCARD_SmartCardTransferAPDU(UINT8 cardIndex, UINT8 *commandAPDU, UINT32 commandAPDULen, UINT8 *responseAPDU, UINT32 *responseAPDULen, UINT32 maxresponseAPDULen);
DTV_STATUS_T HAL_BCAS_SMARTCARD_RegisterEvent_CardAction(UINT32 (*pfn)(UINT8, UINT32));
int HAL_BCAS_SMARTCARD_IsCardInserted(void);

#endif /*_BCAS_HAL_H_ */

