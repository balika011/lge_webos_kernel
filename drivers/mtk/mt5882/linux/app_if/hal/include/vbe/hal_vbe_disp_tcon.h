/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file vbe_tcon_soc_ddi.h
 *
 * This source file defines the DDI functions related to panel spec
 *
 *  @author	Song, Taesun(taesun.song@lge.com)
 *  @author Seol Seong Woon(sw.seol@lge.com)
 *  @version	1.0
 *  @date	2011. 7. 30
 *  @note	Additional information.
 *  @see		vbe_tcon_soc_ddi.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_HAL_VBE_DISP_TCON_H_
#define	_HAL_VBE_DISP_TCON_H_

/******************************************************************************
 #include ���ϵ� (File Inclusions)
******************************************************************************/
#include "hal_common.h"

/******************************************************************************
    ��ũ�� �Լ� ���� (Macro Definitions)
******************************************************************************/

/******************************************************************************
   ���� ��� ���� (Local Constant Definitions)
******************************************************************************/

/******************************************************************************
    ���� �� ���� (Local Type Definitions)
******************************************************************************/

/******************************************************************************
    ���� �� ���� (Global Type Definitions)
******************************************************************************/
typedef enum
{
	DISP_TCON_DITHER_TRUNC		= 0,
	DISP_TCON_DITHER_ROUND,
	DISP_TCON_DITHER_RANDOM
} DISP_TCON_DITHER_T;

typedef enum
{
	HAL_VBE_DISP_TCON_LVDS_JEIDA = 0,
	HAL_VBE_DISP_TCON_LVDS_VESA,
	HAL_VBE_DISP_TCON_LVDS_MAXNUM
} HAL_VBE_DISP_TCON_LVDS_TYPE_T;

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

/******************************************************************************
	�Լ� ���� (Function Definitions)
******************************************************************************/

DTV_STATUS_T HAL_VBE_DISP_TCON_Initialize(void);
DTV_STATUS_T HAL_VBE_DISP_TCON_Uninitialize(void);
DTV_STATUS_T HAL_VBE_DISP_TCON_EnableTCon(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_DISP_TCON_EnableColorTemp(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_DISP_TCON_EnableDGA(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_DISP_TCON_EnableDither(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_DISP_TCON_SetDitherMode(DISP_TCON_DITHER_T mode);
DTV_STATUS_T HAL_VBE_DISP_TCON_SetGammaTable(UINT32 *pRedGammaTable, UINT32 *pGreenGammaTable, UINT32 *pBlueGammaTable);
DTV_STATUS_T HAL_VBE_DISP_TCON_H_Reverse(UINT8 u8Mode);
DTV_STATUS_T HAL_VBE_DISP_TCON_SetClock(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_DISP_TCON_WriteRegister(UINT32 u32Address, UINT32 u32Data);
DTV_STATUS_T HAL_VBE_DISP_TCON_ReadRegister(UINT32 u32Address, UINT32 *pData);
DTV_STATUS_T HAL_VBE_DISP_TCON_SetLvdsFormat(HAL_VBE_DISP_TCON_LVDS_TYPE_T type);

void HAL_VBE_DISP_TCON_Debug(void);

#endif

