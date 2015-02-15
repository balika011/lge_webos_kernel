/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_vpq_led.h
 *
 *  It is the header file for controling Video Backend LED function.
 *  The header of VBE DDI is controlled by this file.
 *
 *  @author	Seol, SeongWoon(sw.seol@lge.com)
 *  @version	0.1
 *  @date		2012.06.13
 *  @note
 *  @see
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef _HAL_VPQ_LED_H_
#define _HAL_VPQ_LED_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 	File Inclusions
------------------------------------------------------------------------------*/
#include "../include/hal_common.h"

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
 * LOCALDIMMING DemoMode Type Enum.
*/
typedef enum {
	LED_ONOFF = 0,	// each led block flicker sequentially.
	LED_ONOFF_EXT,	// led moving box pattern on for external localdimming chip.
	SPLIT_SCREEN,	// half of screen is made from white pattern, and show local dimming effect.
	DEMOTYPE_SIZE_MAX
} LOCALDIMMING_DEMO_TYPE_T;

/**
 * Backlight Type Enum.
*/
typedef enum {
	HAL_BL_DIRECT_M = 0,
	HAL_BL_DIRECT_L,
	HAL_BL_EDGE_LED,
	HAL_BL_ALEF_LED,
	HAL_BL_OLED,
	HAL_BL_END,
} HAL_BACKLIGHT_TYPE_T;

/**
 * Old Backlight Type Enum.
*/
typedef enum {
	HAL_BL_ROW = HAL_BL_END,
	HAL_BL_NOR_LED,
	HAL_BL_IOL_LED,
	HAL_BL_POLA,
	HAL_BL_CCFL,

	HAL_BL_IOP_LED,
	HAL_BL_ALEF_LGD,
	HAL_BL_ALEF_FPR,
} OLD_HAL_BACKLIGHT_TYPE_T;

/**
 * Panel size Enum.
*/
typedef enum {
	HAL_INCH_32 = 0,
	HAL_INCH_39,
	HAL_INCH_42,
	HAL_INCH_47,
	HAL_INCH_49,
	HAL_INCH_50,
	HAL_INCH_55,
	HAL_INCH_58,
	HAL_INCH_60,
	HAL_INCH_65,
	HAL_INCH_70,
	HAL_INCH_77,
	HAL_INCH_79,
	HAL_INCH_84,
	HAL_INCH_98,
	HAL_INCH_105, // TV model

	HAL_INCH_23,
	HAL_INCH_24,
	HAL_INCH_26,
	HAL_INCH_27, // Smart Monitor TV

	HAL_INCH_22,
	HAL_INCH_28,

	HAL_INCH_BASE
} HAL_INCH_TYPE_T;

/**
 * Panel Led_bar Type Enum.
*/
typedef enum {
	HAL_LED_BAR_DIRECTM_8	= 0,
	HAL_LED_BAR_DIRECTM_6	= 1,
	HAL_LED_BAR_DIRECTM_10	= 2,
	HAL_LED_BAR_DIRECTM_12	= 3,
	HAL_LED_BAR_DIRECTM_15	= 4,
	HAL_LED_BAR_DIRECTM_16	= 5,
	HAL_LED_BAR_DIRECTM_288	= 6,

	HAL_LED_BAR_EDGE_6		= 7,
	HAL_LED_BAR_EDGE_12		= 8,
	HAL_LED_BAR_EDGE_16		= 9,
	HAL_LED_BAR_EDGE_20		= 10,
	HAL_LED_BAR_EDGE_32		= 11,
	HAL_LED_BAR_EDGE_80		= 12,

	HAL_LED_BAR_MAX,
	HAL_LED_BAR_DEFALT = HAL_LED_BAR_MAX,
} HAL_LED_BAR_TYPE_T;

/**
 * Panel Old_Led_bar Type Enum.
*/
typedef enum {
	HAL_LED_BAR_REV_BLK	= HAL_LED_BAR_MAX+1,
	HAL_LED_BAR_H_4BLK,
	HAL_LED_BAR_H_12BLK,
	HAL_LED_BAR_H_6BLK,

	HAL_LED_BAR_D_10BLK,
	HAL_LED_BAR_REV1_BLK,
	HAL_LED_BAR_REV2_BLK,
	HAL_LED_BAR_REV3_BLK,
} OLD_HAL_LED_BAR_TYPE_T;


/**
 * Panel Led_bar Type Enum.
*/
typedef enum {
	HAL_MODULE_LGD	= 0,
	HAL_MODULE_CMI,
	HAL_MODULE_AUO,
	HAL_MODULE_SHARP,
	HAL_MODULE_IPS,
	HAL_MODULE_BOE,
	HAL_MODULE_CSOT,
	HAL_MODULE_INNOLUX,
	HAL_MODULE_LCD_END,

	HAL_MODULE_LGE = HAL_MODULE_LCD_END,
	HAL_MODULE_PANASONIC,
	HAL_MODULE_PDP_END,
	HAL_MODULE_BASE= HAL_MODULE_PDP_END,
} HAL_MODULE_MAKER_TYPE_T;

/**
 * Panel Led_Lut index.
*/
typedef enum {
	HAL_LUT_FRIST		= 0,
	HAL_LUT_SECOND,
	HAL_LUT_THIRD,
	HAL_LUT_MAX
} HAL_LUT_NUMBER_TYPE_T;

/**
  * Local dimming block type
  */
 typedef enum {
	HAL_LDIM_NONE			=0,		// Not support Local dimming.
	HAL_LDIM_INTERNAL		=1,		// Use internal Local dimming block.
	HAL_LDIM_EXTERNAL		=2,		// Use external Local dimming IC.
} HAL_LDIM_IC_T;

/**
  * Panel type
  */
typedef enum {
	HAL_WCG_PANEL_LED				=0,
	HAL_WCG_PANEL_LED_ULTRAHD		=1,
	HAL_WCG_PANEL_OLED				=2,
	HAL_WCG_PANEL_OLED_ULTRAHD		=3
} HAL_WCG_PANEL_TYPE_T;

/**
 * Panel information
 */
typedef struct {
	UINT8 hal_inch;				// panel size			ex) 47, 55
	UINT8 hal_bl_type;			// led backlight type		ex) alef, edge
	UINT8 hal_bar_type;			// led bar type			ex) h6,h12, v12
	UINT8 hal_maker;			// panel maker			ex) lgd, auo
	HAL_LDIM_IC_T hal_icType;	// localdimming control type	ex) internal localdiming block
	HAL_WCG_PANEL_TYPE_T hal_panel_type;
} HAL_LED_PANEL_INFO_T;

typedef struct
{
	UINT32 *pu32Ptr;
	UINT32 u32Size;
} HAL_LED_LUT_INFO;

typedef struct
{
	HAL_LED_LUT_INFO uiOff;
	HAL_LED_LUT_INFO uiLow;
	HAL_LED_LUT_INFO uiMedium;
	HAL_LED_LUT_INFO uiHigh;
} HAL_LED_UI_LUT_INFO_T;

typedef struct
{
	HAL_LED_LUT_INFO sbiHCoef;
	HAL_LED_LUT_INFO sbiVCoef;
	HAL_LED_LUT_INFO lres1TabCoef;
	HAL_LED_LUT_INFO lres5TabCoef;
} HAL_LED_DB_LIGHT_PROFILE_LUT_T;

typedef struct
{
	HAL_LED_UI_LUT_INFO_T upperCurve;
	HAL_LED_UI_LUT_INFO_T lowerCurve;
} HAL_LED_DB_DIMMING_CURVE_LUT_T;

/**
 * LED LUT DB
 */
typedef struct
{
	UINT32 u32LutVersion;
	HAL_LED_LUT_INFO pq_common_lut;
	HAL_LED_LUT_INFO module_lut;
	HAL_LED_LUT_INFO unsharpMask_lut;
	HAL_LED_DB_LIGHT_PROFILE_LUT_T lightProfile_lut;
	HAL_LED_DB_DIMMING_CURVE_LUT_T dimmingCurve_lut;
	HAL_LED_UI_LUT_INFO_T pqUi_lut;
	HAL_LED_UI_LUT_INFO_T pixelCompensation_lut;
} HAL_LED_DB_LUT_T;

/*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

DTV_STATUS_T HAL_VPQ_LED_Initialize(HAL_LED_PANEL_INFO_T stPanelInfo);
DTV_STATUS_T HAL_VPQ_LED_Uninitialize(void);
DTV_STATUS_T HAL_VPQ_LED_LDSetDBLUT(HAL_LED_DB_LUT_T *pstLedDbLut);
DTV_STATUS_T HAL_VPQ_LED_LDEnable(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_LDSetLUT(UINT8 lutTableIndex);
DTV_STATUS_T HAL_VPQ_LED_LDEnablePixelCompensation(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_LDCtrlDemoMode(LOCALDIMMING_DEMO_TYPE_T bType, BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_LDCtrlSPI(UINT8 bitMask, BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_LDGetFirmwareVersion(UINT8 *);
DTV_STATUS_T HAL_VPQ_LED_LDGetAPL(UINT16 *u16Max, UINT16 *u16Min);
DTV_STATUS_T HAL_VPQ_LED_LDSetClippingValue(UINT16 u16ClippingValue);

DTV_STATUS_T HAL_VPQ_LED_LDSetClearPlus(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_LDCtrlStoreDimmingLuxUp(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_LDSetLedPKGControl(BOOLEAN bCtrl);

/*HDR */
DTV_STATUS_T HAL_VPQ_LED_HDRInitialize(void);
DTV_STATUS_T HAL_VPQ_LED_HDRSetMode(UINT8 hdrDCMode);
DTV_STATUS_T HAL_VPQ_LED_HDRSetDECMode(UINT8 hdrDECMode);
DTV_STATUS_T HAL_VPQ_LED_HDRBypassMode(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_HDRBypassInternalControl(BOOLEAN hdrBypass);
DTV_STATUS_T HAL_VPQ_LED_HDRSetSceneChange(BOOLEAN bIsSceneChange);
DTV_STATUS_T HAL_VPQ_LED_HDRCtrlStoreMode(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_HDRGetFirmwareVersion(UINT8 *pVersion);

/*WCG */
DTV_STATUS_T HAL_VPQ_LED_WCGInitialize(HAL_LED_PANEL_INFO_T panelInfo);
DTV_STATUS_T HAL_VPQ_LED_WCGEnable(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_WCGSetMode(UINT8 wcgMode);
DTV_STATUS_T HAL_VPQ_LED_WCGBypassMode(BOOLEAN bCtrl);
DTV_STATUS_T HAL_VPQ_LED_WCGGetFirmwareVersion(UINT8 *);

DTV_STATUS_T HAL_VPQ_LED_WCGSetLUT(UINT8 lutTableIndex);

/* DEBUG Function */
DTV_STATUS_T HAL_VPQ_LED_Debug(void);

#endif /* _HAL_VPQ_LED_DDI_H_ */
