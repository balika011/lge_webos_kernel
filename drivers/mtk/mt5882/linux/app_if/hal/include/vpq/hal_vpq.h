/******************************************************************
 *	 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 1999 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 ******************************************************************/

/** @file hal_vpq.h
 *
 *  hal vpq driver header.
 *
 *  @author	Youm, Seung-Jun(sjyoum@lge.com)
 *  @version	1.0
 *  @date		2013.05.22
 *  @see
 */

#ifndef	_HAL_VPQ_H_
#define	_HAL_VPQ_H_

/*******************************************************************
	#include 파일들
	(File Inclusions)
*******************************************************************/
#include "hal_common.h"
#include "hal_vsc.h"

/*******************************************************************
	상수 정의
	(Constant Definitions)
*******************************************************************/
/* histogram bin, chroma bin num */
#define HAL_VPQ_BIN_NUM			64
#define HAL_VPQ_C_BIN_NUM		32
#define HAL_VPQ_H_BIN_NUM		32

/* preferred color selection */
#define HAL_VPQ_PREFERRED_SKIN		0
#define HAL_VPQ_PREFERRED_BSKY		1
#define HAL_VPQ_PREFERRED_GRASS		2

/*******************************************************************
	매크로 함수 정의
	(Macro Definitions)
*******************************************************************/

/*******************************************************************
   	 형 정의
 	 (Type Definitions)
*******************************************************************/
/**
*	inner pattern ire enum
*	- should be same with DIL_VPQ_INNER_PATTERN_IRE_T
*/
typedef enum
{
	HAL_VPQ_INNER_PATTERN_IRE_0 = 0,	///< ire 0
	HAL_VPQ_INNER_PATTERN_IRE_5,		///< ire 5
	HAL_VPQ_INNER_PATTERN_IRE_10,		///< ire 10
	HAL_VPQ_INNER_PATTERN_IRE_15,		///< ire 15
	HAL_VPQ_INNER_PATTERN_IRE_20,		///< ire 20
	HAL_VPQ_INNER_PATTERN_IRE_25,		///< ire 25
	HAL_VPQ_INNER_PATTERN_IRE_30,		///< ire 30
	HAL_VPQ_INNER_PATTERN_IRE_35,		///< ire 35
	HAL_VPQ_INNER_PATTERN_IRE_40,		///< ire 40
	HAL_VPQ_INNER_PATTERN_IRE_45,		///< ire 45
	HAL_VPQ_INNER_PATTERN_IRE_50,		///< ire 50
	HAL_VPQ_INNER_PATTERN_IRE_55,		///< ire 55
	HAL_VPQ_INNER_PATTERN_IRE_60,		///< ire 60
	HAL_VPQ_INNER_PATTERN_IRE_65,		///< ire 65
	HAL_VPQ_INNER_PATTERN_IRE_70,		///< ire 70
	HAL_VPQ_INNER_PATTERN_IRE_75,		///< ire 75
	HAL_VPQ_INNER_PATTERN_IRE_80,		///< ire 80
	HAL_VPQ_INNER_PATTERN_IRE_85,		///< ire 85
	HAL_VPQ_INNER_PATTERN_IRE_90,		///< ire 90
	HAL_VPQ_INNER_PATTERN_IRE_95,		///< ire 95
	HAL_VPQ_INNER_PATTERN_IRE_100		///< ire 100
}
HAL_VPQ_INNER_PATTERN_IRE_T;

/**
*	input type enum
*/
typedef enum
{
	HAL_VPQ_INPUT_ATV = 0,			///< analog rf
	HAL_VPQ_INPUT_AV,				///< cvbs,svideo
	HAL_VPQ_INPUT_SCARTRGB,			///< scart-rgb
	HAL_VPQ_INPUT_COMP,				///< component
	HAL_VPQ_INPUT_RGB_PC,			///< rgb-pc
	HAL_VPQ_INPUT_HDMI_TV,			///< hdmi non-pc(hdmi-av,dvi-dtv,mcam)
	HAL_VPQ_INPUT_HDMI_PC,			///< hdmi pc(dvi-pc)
	HAL_VPQ_INPUT_DTV,				///< digital rf
	HAL_VPQ_INPUT_PICWIZ,			///< dtv pic wiz
	HAL_VPQ_INPUT_PICTEST,			///< dtv pic test
	HAL_VPQ_INPUT_MEDIA_MOVIE,		///< divx, cp-divx
	HAL_VPQ_INPUT_MEDIA_PHOTO,		///< photo, cp-photo
	HAL_VPQ_INPUT_CAMERA,			///< vcs
	HAL_VPQ_INPUT_PVR_DTV,			///< pvr dtv
	HAL_VPQ_INPUT_PVR_ATV,			///< pvr atv
	HAL_VPQ_INPUT_PVR_AV,			///< pvr av,scart-rgb
	HAL_VPQ_INPUT_MAX				///< max num
}
HAL_VPQ_INPUT_T;

/**
*	3d mode type enum
*/
typedef enum
{
	HAL_VPQ_3D_MODE_OFF = 0,		///< 3d off, 3d to 2d
	HAL_VPQ_3D_MODE_3D_TB,			///< 3d top and botom
	HAL_VPQ_3D_MODE_3D_SS,			///< 3d side by side
	HAL_VPQ_3D_MODE_3D_CB,			///< 3d checker board
	HAL_VPQ_3D_MODE_3D_FS,			///< 3d frame seq
	HAL_VPQ_3D_MODE_3D_CI,			///< 3d column interleave
	HAL_VPQ_3D_MODE_3D_LIH,			///< 3d line interleave half
	HAL_VPQ_3D_MODE_3D_FP,			///< 3d frame packing
	HAL_VPQ_3D_MODE_3D_FA,			///< 3d field alternative
	HAL_VPQ_3D_MODE_3D_LA,			///< 3d line alternative
	HAL_VPQ_3D_MODE_3D_SSF,			///< 3d side by side full
	HAL_VPQ_3D_MODE_3D_DS,			///< 3d dual stream
	HAL_VPQ_3D_MODE_2DTO3D,			///< 2d to 3d
	HAL_VPQ_3D_MODE_MAX				///< max num
}
HAL_VPQ_3D_MODE_T;

/**
*	dc2p histogram data info
*/
typedef struct
{
	UINT32 apl;								///< apl
	SINT32 min;								///< min
	SINT32 max;								///< max
	SINT32 peak_low;						///< peak low
	SINT32 peak_high;						///< peak high
	UINT32 skin_count;						///< skin count
	UINT32 sat_status;						///< saturation status
	UINT32 diff_sum;						///< diff sum
	UINT32 motion;							///< motion
	UINT32 texture;							///< texture
	UINT32 bin[HAL_VPQ_BIN_NUM];			///< count bin
	UINT32 chrm_bin[HAL_VPQ_C_BIN_NUM];		///< count chroma bin
	UINT32 hue_bin[HAL_VPQ_H_BIN_NUM];		///< count chroma bin
}
HAL_VPQ_DC2P_HISTODATA_INFO_T;

/**
*	color temperature
*/
typedef struct
{
	UINT8 rgb_gain[3];		///< gain. 0:red, 1:green, 2:blue
	UINT8 rgb_offset[3];	///< offset. 0:red, 1:green, 2:blue
}
HAL_VPQ_COLOR_TEMP_T;

/*******************************************************************/
/**************** 	SOC DDI (defined in pql_soc.c)	****************/
/*******************************************************************/

/* Picture Control */
DTV_STATUS_T HAL_VPQ_SetPictureCtrl(VIDEO_WID_T wId,
									SINT32 *pPcVal,
									void *pstData);
DTV_STATUS_T HAL_VPQ_SetColorTemp(HAL_VPQ_COLOR_TEMP_T *pCscPcCtData);
DTV_STATUS_T HAL_VPQ_SetColorTemperature(HAL_VPQ_COLOR_TEMP_T *pCscPcCtData,
                                         void *pstData);

/* Sharpness */
DTV_STATUS_T HAL_VPQ_SetSuperResolution(VIDEO_WID_T wId,
										UINT16 uSrVal,
										void *pstData);
DTV_STATUS_T HAL_VPQ_SetSharpness(VIDEO_WID_T wId,
								UINT16 *pShpVal,
								void *pstData);
DTV_STATUS_T HAL_VPQ_SetEdgeEnhance(VIDEO_WID_T wId,
									UINT16 uEeVal,
									void *pstData);

/* Init, Open, Close */
DTV_STATUS_T HAL_VPQ_Initialize(void);
DTV_STATUS_T HAL_VPQ_Uninitialize(void);
DTV_STATUS_T HAL_VPQ_Open(void);
DTV_STATUS_T HAL_VPQ_Close(void);

/* Histogram */
DTV_STATUS_T HAL_VPQ_GetFreshContrastBypassLUT(SINT32 *pDCBypassLUT);
DTV_STATUS_T HAL_VPQ_GetHistoData(HAL_VPQ_DC2P_HISTODATA_INFO_T *pHistoInfo);

/* Dynamic Conrast */
DTV_STATUS_T HAL_VPQ_SetFreshContrast(VIDEO_WID_T wId,
									UINT16 uDcVal,
									void *pstData);
DTV_STATUS_T HAL_VPQ_SetFreshContrastLUT(SINT32 *pAccLUT);
DTV_STATUS_T HAL_VPQ_SetSaturationLUT(UINT32 *pSaturationX,
									UINT32 *pSaturationY);

/* Dynamic Color */
DTV_STATUS_T HAL_VPQ_InitColorForAdvanced(VIDEO_WID_T wId,
										void *pstData);
DTV_STATUS_T HAL_VPQ_SetFreshColor(VIDEO_WID_T wId,
								UINT32 uFcVal,
								void *pstData);
DTV_STATUS_T HAL_VPQ_SetPreferredColor(VIDEO_WID_T wId,
										SINT8 *pstPfVal,
										void *pstData);
DTV_STATUS_T HAL_VPQ_SetClearWhiteGainCtrl(UINT32 *pCwGain);
DTV_STATUS_T HAL_VPQ_SetClearWhite(VIDEO_WID_T wId,
                                  UINT8 uCwVal,
                                  void *pstData);

/* 3D Color Correction */
DTV_STATUS_T HAL_VPQ_Set3DColorCorrection(UINT8 *pCcVal);

/* Noise Reduction */
DTV_STATUS_T HAL_VPQ_SetNoiseReduction(VIDEO_WID_T wId,
										UINT16 *pNrVal,
										void *pstData);
DTV_STATUS_T HAL_VPQ_SetMpegNoiseReduction(VIDEO_WID_T wId,
										UINT16 uMpegNrVal,
										void *pstData);
DTV_STATUS_T HAL_VPQ_GetNoiseLevel(UINT16 *uLevel);
DTV_STATUS_T HAL_VPQ_GetMotionLevel(UINT32 *uLevel);

/* Deinterlace */
DTV_STATUS_T HAL_VPQ_SetFilmMode(VIDEO_WID_T wId,
                                 BOOLEAN bFilmMode);
DTV_STATUS_T HAL_VPQ_SetRealCinema(VIDEO_WID_T wId,
                                   BOOLEAN bFilmMode,
                                   void *pstData);

/* Blacklevel */
DTV_STATUS_T HAL_VPQ_SetBlackLevel(VIDEO_WID_T wId,
									UINT8 *pBlVal,
									void *pstData);

/* Gamma */
DTV_STATUS_T HAL_VPQ_SetGammaLUT(VIDEO_WID_T wId,
								UINT32 *pRedGammaTable,
								UINT32 *pGreenGammaTable,
								UINT32 *pBlueGammaTable);

/* Color Gamut */
DTV_STATUS_T HAL_VPQ_SetColorGamut(VIDEO_WID_T wId,
                                   UINT8 *pCgVal);
DTV_STATUS_T HAL_VPQ_SetGamut(VIDEO_WID_T wId,
                              UINT8 *pCgVal,
                              void *pstData);

/* Color Filter */
DTV_STATUS_T HAL_VPQ_SetColorFilter(UINT16 uCfVal);

/* Color Manage */
DTV_STATUS_T HAL_VPQ_InitColorForExpert(VIDEO_WID_T wId,
										void *pstData);
DTV_STATUS_T HAL_VPQ_SetColorManagement(VIDEO_WID_T wId,
										SINT8 *pCmVal,
										void *pstData);

/* Demo Mode */
DTV_STATUS_T HAL_VPQ_SetSplitDemo( BOOLEAN bOnOff);

/* Game Mode */
DTV_STATUS_T HAL_VPQ_SetLowDelayMode(BOOLEAN bGameMode);

/* Color Gain */
DTV_STATUS_T HAL_VPQ_SetColorGain(VIDEO_WID_T wId,
								SINT32 uCgVal);

/* IRE pattern */
DTV_STATUS_T HAL_VPQ_SetIREInnerPattern(UINT8 patternType,
								HAL_VPQ_INNER_PATTERN_IRE_T patternIre);

DTV_STATUS_T HAL_VPQ_LED_LDEnableLocalDimming(BOOLEAN bCtrl);

#endif	/* _HAL_VPQ_H_ */

