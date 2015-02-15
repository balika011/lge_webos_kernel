/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file frc_sr_ddi.h
 *
 * This source file defines the DDI functions related to SuperResolution Device Driver control
 *
 *  @author	Seol, Seong Woon (sw.seol@lge.com)
 *  @version	0.1
 *  @date	2011.07.29
 *  @note	Additional information.
 *  @see frc_sr_ddi.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_FRC_SR_DDI_H_
#define	_FRC_SR_DDI_H_

/******************************************************************************
 #include ���ϵ� (File Inclusions)
*****************************************************************************/
#ifndef WIN32_SIM
#include "common.h"
#else
#include "common_win32.h"
#endif

#include "global_configurations.h"
#include "pql_ddi.h"


/******************************************************************************
    ��ũ�� �Լ� ���� (Macro Definitions)
******************************************************************************/

/******************************************************************************
   ���� ��� ���� (Local Constant Definitions)
******************************************************************************/

/*****************************************************************************4b
    ���� �� ���� (Local Type Definitions)
******************************************************************************/

/******************************************************************************
    ���� �� ���� (Global Type Definitions)
******************************************************************************/

/******************************************************************************
    Extern ���������� �Լ� prototype ����
    (Extern Variables & Function Prototype Declarations)
******************************************************************************/

/******************************************************************************
    Static ������ �Լ� prototype ����
    (Static Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    ���� ������ �Լ� prototype ����
    (Local Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    ���� ������ �Լ� prototype ����
    (Global Variables & Function Prototypes Declarations)
******************************************************************************/

/**
 * pointer structure for SR function.
*/
typedef struct FRC_SR_DDI_CHIP
{
	DTV_STATUS_T (*pfnInitialize)(void);
	DTV_STATUS_T (*pfnSetSROnOff)(BOOLEAN bSROnOff);
	DTV_STATUS_T (*pfnSetInputFormat)(UINT8 uIndex);
	DTV_STATUS_T (*pfnSetUISharpness)(UINT8 u8Value);	
	//DTV_STATUS_T (*pfnSetPSM)(UINT8 uIndex);
	//DTV_STATUS_T (*pfnSetStrength)(UINT8 u8Value);
	//DTV_STATUS_T (*pfnSet3DMode)(BOOLEAN b3DOnOff);
	//DTV_STATUS_T (*pfnReadWriteAddr)(UINT32 u32Addr);
	//DTV_STATUS_T (*pfnWriteData)(UINT32 u32Data);
	//DTV_STATUS_T (*pfnReadData)(UINT32 u32Data);
	DTV_STATUS_T (*pfnUpdateBinary)(UINT8 *pbuffer, UINT32 size);
	DTV_STATUS_T (*pfnGetFWVersionSR)(UINT8 *pVersion);//���⼭ version�� �ѱ�.
	void	(*pfnDebug) (void);
	
} FRC_SR_DDI_CHIP_T;

/******************************************************************************
	�Լ� ���� (Function Definitions)
******************************************************************************/

DTV_STATUS_T DDI_FRC_SR_Initialize(void);
DTV_STATUS_T DDI_FRC_SR_SetSROnOff(BOOLEAN bSROnOff);
DTV_STATUS_T DDI_FRC_SR_SetSRInputFormat(PQL_STATUS_T pqlStatus);
DTV_STATUS_T DDI_FRC_SR_SetSRUISharpness(UINT8 u8SRSharpness);
//20120108 ykkim no need) DTV_STATUS_T DDI_FRC_SR_SetPSM(UINT8 uIndex);
//20120108 ykkim no need) DTV_STATUS_T DDI_FRC_SR_SetStrength(UINT8 u8Value);
//20120108 ykkim no need) DTV_STATUS_T DDI_FRC_SR_Set3DMode(BOOLEAN b3DOnOff);
//20120108 ykkim no need) DTV_STATUS_T DDI_FRC_SR_ReadWriteAddr(UINT32 u32Addr);
//20120108 ykkim no need) DTV_STATUS_T DDI_FRC_SR_WriteData(UINT32 u32Data);
//20120108 ykkim no need) DTV_STATUS_T DDI_FRC_SR_ReadData(UINT32 u32Data);
DTV_STATUS_T DDI_FRC_SR_UpdateBinary(UINT8 *pbuffer, UINT32 size);
DTV_STATUS_T DDI_FRC_SR_GetFirmwareVersion(UINT8 *pVersion);
//DTV_STATUS_T DDI_FRC_SR_GetFirmwareVersionFromReq(UINT8 *pVersion);

void DDI_FRC_SR_Debug(void);

#endif /* _FRC_SR_DDI_H_ */

 
