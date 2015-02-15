
/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file frc_ddi.h
 *
 * This source file defines the DDI functions related to FRC Driver control
 *
 *  @author	Hong, Young Ho(longsky@lge.com)
 *  @version	0.1
 *  @date		2009.06.25
 *  @note		Additional information.
 *  @see		frc_ddi.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_FRC_DDI_H_
#define	_FRC_DDI_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#ifndef WIN32_SIM // modified by dilly97 : fix include
#include "common.h"
#else
#include "common_win32.h"
#endif
#include "i2c_ddi.h"
#include "global_configurations.h"


/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
   로컬 상수 정의 (Local Constant Definitions)
******************************************************************************/

/******************************************************************************
    로컬 형 정의 (Local Type Definitions)
******************************************************************************/

/**
 * color temperature 값.
 *
 * @see
 */
typedef struct COLOR_TEMP
{
	UINT8	rgbGain[3]; 		/**< Gain. 0:Red, 1:Green, 2:Blue*/
	UINT8	rgbOffset[3];		/**< Offset. 0:Red, 1:Green, 2:Blue*/

}COLOR_TEMP_T; /* should be sync with PQL_COLOR_TEMP_T */


/******************************************************************************
    전역 형 정의 (Global Type Definitions)
******************************************************************************/
/** GPIO Write functions */
typedef enum
{
	GPIO_WR_FRC3_START 		= 0,
	GPIO_WR_FRC3_SELECTTCON,		//TCON IC가 2개인경우.
	GPIO_WR_FRC3_PCID,				//Line OD=PCID사용여부. TCON으로 보냄.
	GPIO_WR_FRC3_OD,				//Frame OD사용여부. TCON으로 보냄.
	GPIO_WR_FRC3_TCONWRITE_PROTECT,	//TCON NVRAM의 Write protection.
	GPIO_WR_FRC3_LG5822_RESET,		//TCON RESET..TCON.
	GPIO_WR_FRC3_LG5822_I2CEN,		//TCON I2C MASTER와 SLAVE로 변환해 주는 GPIO.
	GPIO_WR_FRC3_END,
} GPIO_WR_FRC3_TYPE_T;

/**
 * pointer strut for FRC function.
*/
typedef struct VBE_FRC_DDI_CHIP
{
	DTV_STATUS_T	(*pfnUpdateFirmwareFRC) (UINT8 *pbuffer, UINT32 size);
	DTV_STATUS_T	(*pfnInitializeFRC) (void);
	DTV_STATUS_T	(*pfnResetSWFRC) (void);
	DTV_STATUS_T	(*pfnMotionCompensationOnOffFRC) (BOOLEAN bCtrl);
	DTV_STATUS_T	(*pfnSetMotionCompensationFRC) (UINT8 blurLevel, UINT8 judderLevel, FRC_MC_TYPE_T motion);
	DTV_STATUS_T	(*pfnSetTrueMotionDemoFRC) (BOOLEAN bOnOff);
	DTV_STATUS_T	(*pfnSetDemoBarFRC) (BOOLEAN bOnOff, UINT8 r, UINT8 g, UINT8 b);
	DTV_STATUS_T	(*pfnSetVideoBlockFRC) (FRC_MUTE_TYPE_T type, BOOLEAN bOnOff);
	DTV_STATUS_T	(*pfnSetDisplayOutputFRC) (BOOLEAN bCtrl);
	DTV_STATUS_T	(*pfnSetBypassMEMCRegionFRC) (BOOLEAN bOnOff, FRC_REGION_T  region, FRC_RECT_T rect);
	DTV_STATUS_T	(*pfnGetFWVersionFRC) (UINT16 *u16Version);
	DTV_STATUS_T	(*pfnSetSpreadSpectrumFRC) (UINT16 u16Period, UINT16 u16Percent, BOOLEAN bEnable, BOOLEAN bMiuLVDS);
	DTV_STATUS_T	(*pfnSetDebugBlockFRC) (BOOLEAN bEnable);
	DTV_STATUS_T	(*pfnSetReverseControlFRC) (UINT8 u8Mode);
	DTV_STATUS_T	(*pfnSetRGB444Mode) (UINT8 u8mode);
	DTV_STATUS_T	(*pfnSetSeparateOSD) (BOOLEAN bOnOff);
	DTV_STATUS_T	(*pfnGetVideoDelayedTime) (UINT16 *pDelayedTime);
	DTV_STATUS_T	(*pfnDebugMenu) (void);
	// PQL, BLU
	DTV_STATUS_T	(*pfnSetODCTableFRC) (const UINT8* pODTbl);
	DTV_STATUS_T	(*pfnSetWBColorTempFRC) (VBE_COLOR_TEMP_T cscPcCtData);
	DTV_STATUS_T	(*pfnSetWBColorTempFRCAtFactory) (VBE_COLOR_TEMP_T cscPcCtData);
	DTV_STATUS_T	(*pfnSetGammaTableFRC) (UINT32 *pRedGammaTable, UINT32 *pGreenGammaTable, UINT32 *pBlueGammaTable);
	DTV_STATUS_T	(*pfnLowDelayMode) (UINT8 type);
	// GPIO
	DTV_STATUS_T	(*pfnGpioControl) (UINT8 u8GpioNum, BOOLEAN bData);
	DTV_STATUS_T	(*pfnGpioRead) (UINT8 u8GpioNum, UINT8 *bData);
} VBE_FRC_DDI_CHIP_T;

/**
 * Runtime binding을 위한 structure
 */
typedef struct FRC_DDI_RUNBIND
{
	FRC_CHIP_TYPE_T	frcType;
	VBE_FRC_DDI_CHIP_T	*pFuncSet;
} FRC_DDI_RUNBIND_T;

/******************************************************************************
    Extern 전역변수와 함수 prototype 선언
    (Extern Variables & Function Prototype Declarations)
******************************************************************************/

/******************************************************************************
    Static 변수와 함수 prototype 선언
    (Static Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    로컬 변수와 함수 prototype 선언
    (Local Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    전역 변수와 함수 prototype 선언
    (Global Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
	함수 정의 (Function Definitions)
******************************************************************************/
DTV_STATUS_T DDI_VBE_FRC_UpdateFirmware(UINT8 *pbuffer, UINT32 size);
DTV_STATUS_T DDI_VBE_FRC_Initialize(void);
DTV_STATUS_T DDI_VBE_FRC_ResetSW(void);
DTV_STATUS_T DDI_VBE_FRC_MotionCompensationOnOff(BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_SetMotionCompensation(UINT8 blurLevel, UINT8 judderLevel,FRC_MC_TYPE_T motion);
DTV_STATUS_T DDI_VBE_FRC_SetTrueMotionDemo(BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_SetDemoBar(BOOLEAN bOnOff, UINT8 r, UINT8 g, UINT8 b);
DTV_STATUS_T DDI_VBE_FRC_SetVideoBlock (FRC_MUTE_TYPE_T type, BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_SetDisplayOutput (BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_SetBypassMEMCRegion(BOOLEAN bOnOff, FRC_REGION_T region, FRC_RECT_T rect);
UINT8	DDI_VBE_FRC_GetAvailableBypassRegion(void);
DTV_STATUS_T DDI_VBE_FRC_GetFirmwareVersion (UINT16 *pVersion);
DTV_STATUS_T DDI_VBE_FRC_SetSpreadSpectrum(BOOLEAN bEnable, UINT16 u16Percent, UINT16 u16Period);
DTV_STATUS_T DDI_VBE_FRC_FreezeControl(UINT8 type);
DTV_STATUS_T DDI_VBE_FRC_LowDelayMode(UINT8 type);
DTV_STATUS_T DDI_VBE_FRC_SetLEDClearPlus (BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_SetSeparateOSD(BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_SetOSDTxMute(BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_GetVideoDelayedTime(UINT16 *pDelayedTime);
DTV_STATUS_T DDI_VBE_FRC_GpioControl(UINT8 u8GpioNum,BOOLEAN bData);
DTV_STATUS_T DDI_VBE_FRC_GpioRead(UINT8 u8GpioNum,UINT8 *bData);
BOOLEAN DDI_VBE_FRC_GetGpioFrameRate240Hz(void);
BOOLEAN DDI_VBE_FRC_GetGpioWithTconMode(void);
BOOLEAN DDI_VBE_FRC_GetGpioMTKSocMode(void);
BOOLEAN DDI_VBE_FRC_GetGpioOLEDDisplay(void);
BOOLEAN DDI_VBE_FRC_GetGpioLD_OnFRC(void);
BOOLEAN DDI_VBE_FRC_GetGpioUDModule(void);
BOOLEAN DDI_VBE_FRC_GetFRC3ChipID(void);
DTV_STATUS_T DDI_VBE_FRC_HOA_LowDelayMode(UINT8 type);
DTV_STATUS_T DDI_VBE_FRC_HOA_MotionCompensationOnOff(BOOLEAN bOnOff);
DTV_STATUS_T DDI_VBE_FRC_HOA_SetBypassMEMCRegion(BOOLEAN bOnOff, FRC_REGION_T region, FRC_RECT_T rect);
DTV_STATUS_T DDI_VBE_FRC_SetRGB444Mode(UINT8 u8mode);
DTV_STATUS_T DDI_VBE_FRC_SetWBColorTemp(VBE_COLOR_TEMP_T cscPcCtData);
DTV_STATUS_T DDI_VBE_FRC_SetInputMode(UINT8 mode);
void DDI_VBE_FRC_Debug(void);

// ************Internal FRC Control function.******************
DTV_STATUS_T	DDI_VIDEO_FRC_Initialize(void);
DTV_STATUS_T 	DDI_VIDEO_FRC_MotionCompensationOnOff(BOOLEAN bOnOff);
DTV_STATUS_T 	DDI_VIDEO_FRC_SetMotionCompensation(UINT8 blurLevel, UINT8 judderLevel, FRC_MC_TYPE_T motion);
DTV_STATUS_T 	DDI_VIDEO_FRC_SetTrueMotionDemo(BOOLEAN bOnOff);
DTV_STATUS_T 	DDI_VIDEO_FRC_SetDemoBar(BOOLEAN bOnOff, UINT8 r, UINT8 g, UINT8 b);
DTV_STATUS_T	DDI_VIDEO_FRC_GetFirmwareVersion(UINT16 *pVersion);
DTV_STATUS_T	DDI_VIDEO_FRC_SetBypassMEMCRegion(BOOLEAN bOnOff, FRC_REGION_T  region, FRC_RECT_T rect);
DTV_STATUS_T DDI_VIDEO_FRC_SetVideoBlock(FRC_MUTE_TYPE_T type, BOOLEAN bOnOff);
DTV_STATUS_T DDI_VIDEO_FRC_GetFrameDelay (UINT16 *pFrameDelay);
DTV_STATUS_T	DDI_VIDEO_FRC_DEBUG(void);

#endif
