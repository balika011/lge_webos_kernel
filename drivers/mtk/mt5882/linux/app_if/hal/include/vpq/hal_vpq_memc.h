/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_vpq_memc.h
 *
 *  It is the header file for controling Video Backend FRC function.
 *  The header of VBE DDI is controlled by this file.
 *
 *  @author	Song, Taesun(taesun.song@lge.com)
 *  @version	0.1
 *  @date		2011.03.18
 *  @note
 *  @see
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef _HAL_VPQ_MEMC_H_
#define _HAL_VPQ_MEMC_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 	File Inclusions
------------------------------------------------------------------------------*/
#include "hal_common.h"

/*-----------------------------------------------------------------------------
	Constant Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Macro Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Extern Variables & Function Prototype Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Constant Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Type Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global Type Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Static Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
/**
 * FRC MC type
 *
 */
typedef enum {
	MEMC_TYPE_OFF = 0,
	MEMC_TYPE_LOW,
	MEMC_TYPE_HIGH,
	MEMC_TYPE_USER,
	MEMC_TYPE_55_PULLDOWN,
} HAL_MEMC_TYPE_T;

/**
*FRC bypass Region
*
*/
typedef enum {
	BYPASS_REGION_0	= 0,
	BYPASS_REGION_1,
	BYPASS_REGION_2,
	BYPASS_REGION_3,
	BYPASS_REGION_4,
	BYPASS_REGION_MAX,
} HAL_BYPASS_REGION_T;

/**
*FRC Rect
*
*/
typedef struct {
	UINT16		w;
	UINT16		h;
	UINT16		x;
	UINT16		y;
} HAL_BYPASS_REGION_RECT_T;


/**
 *FRC video mute type
 *
 */
typedef enum {
	MEMC_MUTE_TYPE_INPUT = 0,
	MEMC_MUTE_TYPE_OUTPUT,
	MEMC_MUTE_TYPE_LVDS,
	MEMC_MUTE_TYPE_MAX
} MEMC_MUTE_TYPE_T;

/*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
DTV_STATUS_T HAL_VPQ_MEMC_Initialize(void);
DTV_STATUS_T HAL_VPQ_MEMC_Uninitialize(void);
DTV_STATUS_T HAL_VPQ_MEMC_SetMotionComp(UINT8 blurLevel, UINT8 judderLevel, HAL_MEMC_TYPE_T motion);
DTV_STATUS_T HAL_VPQ_MEMC_SetBlurLevel(UINT8 blurLevel);
DTV_STATUS_T HAL_VPQ_MEMC_SetJudderLevel(UINT8 judderLevel);
DTV_STATUS_T HAL_VPQ_MEMC_MotionCompOnOff(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VPQ_MEMC_LowDelayMode(UINT8 type);
DTV_STATUS_T HAL_VPQ_MEMC_SetRGBYUVMode(UINT8 mode);
DTV_STATUS_T HAL_VPQ_MEMC_GetFrameDelay (UINT16 *pFrameDelay);
DTV_STATUS_T HAL_VPQ_MEMC_SetVideoBlock(MEMC_MUTE_TYPE_T type, BOOLEAN bOnOff);
DTV_STATUS_T HAL_VPQ_MEMC_SetTrueMotionDemo(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VPQ_MEMC_GetFirmwareVersion(UINT16 *pVersion);
DTV_STATUS_T HAL_VPQ_MEMC_SetBypassRegion(BOOLEAN bOnOff, HAL_BYPASS_REGION_T region, HAL_BYPASS_REGION_RECT_T rect);
DTV_STATUS_T HAL_VPQ_MEMC_SetReverseControl(UINT8 u8Mode);
DTV_STATUS_T HAL_VPQ_MEMC_Freeze(UINT8 type);
DTV_STATUS_T HAL_VPQ_MEMC_SetDemoBar(BOOLEAN bOnOff, UINT8 r, UINT8 g, UINT8 b);
DTV_STATUS_T HAL_VPQ_MEMC_DEBUG(void);

#endif /* _HAL_VPQ_MEMC_DDI_H_ */
