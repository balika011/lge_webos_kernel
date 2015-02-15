/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_vfe_avd.h
 *
 *  It is the header file for controling Analog-Video-Decoder function.
 *  The header of Analog-Video-Decoder function DDI is controlled by this file.
 *
 *  @author	yongsu.yoo(yongsu.yoo@lge.com)
 *  @version	0.1
 *  @date		2013.03.14
 *  @note
 *  @see
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef _HAL_VFE_AVD_H_
#define _HAL_VFE_AVD_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 	File Inclusions
------------------------------------------------------------------------------*/
#include "hal_common.h"

#ifdef	__cplusplus
	extern "C"
	{
#endif /* __cplusplus */

/*-----------------------------------------------------------------------------
	Constant Definitions
------------------------------------------------------------------------------*/
#define 	BIT0_VFE_AVD	0x00000001
/*-----------------------------------------------------------------------------
	Macro Definitions
------------------------------------------------------------------------------*/
#ifndef MSG_TYPE_T
typedef struct
{
	UINT32	msgID;
	UINT32	data[3];
} _MSG_TYPE_T;

#define MSG_TYPE_T	_MSG_TYPE_T
#endif
	
#ifndef OSA_MSGQ_T
typedef unsigned long OSA_MSGQ_T;
#endif

typedef enum
{
	VIDEO_AVDEC_MODE_NTSC		 = 0,
	VIDEO_AVDEC_MODE_PAL			,
	VIDEO_AVDEC_MODE_PAL_NC_358		,
	VIDEO_AVDEC_MODE_PAL_M			,
	VIDEO_AVDEC_MODE_SECAM			,
	VIDEO_AVDEC_MODE_NTSC_443		,
	VIDEO_AVDEC_MODE_PAL_60 		,
	VIDEO_AVDEC_MODE_UNKNOWN_525	,
	VIDEO_AVDEC_MODE_UNKNOWN_625	,
	VIDEO_AVDEC_MODE_UNKNOWN		,
}VFE_AVD_AVDECODER_VIDEO_MODE_T;
	
typedef enum
{
	 COLOR_SYSTEM_NTSC_M		 = BIT0_VFE_AVD << VIDEO_AVDEC_MODE_NTSC 	,
	 COLOR_SYSTEM_PAL_G 		= BIT0_VFE_AVD << VIDEO_AVDEC_MODE_PAL		,
	 COLOR_SYSTEM_PAL_NC_358	= BIT0_VFE_AVD << VIDEO_AVDEC_MODE_PAL_NC_358	,
	 COLOR_SYSTEM_PAL_M 		= BIT0_VFE_AVD << VIDEO_AVDEC_MODE_PAL_M	,
	 COLOR_SYSTEM_SECAM 		= BIT0_VFE_AVD << VIDEO_AVDEC_MODE_SECAM	,
	 COLOR_SYSTEM_NTSC_443		= BIT0_VFE_AVD << VIDEO_AVDEC_MODE_NTSC_443 ,
	 COLOR_SYSTEM_PAL_60		= BIT0_VFE_AVD << VIDEO_AVDEC_MODE_PAL_60	,
	 COLOR_SYSTEM_PAL_N 										,  /* TBD should be remove */
	 COLOR_SYSTEM_UNKNOWN										,  /* TBD should be remove */
	
}VFE_AVD_COLOR_SYSTEM_T;
	
typedef enum
{
	AVD_SRC_TYPE_ATV	   ,
	AVD_SRC_TYPE_AV 	   ,
	AVD_SRC_TYPE_SCART ,
	AVD_SRC_TYPE_MAX	   ,
} VFE_AVD_SRC_TYPE_T;


typedef enum
{
	AVD_PORT_FIRST	   ,
	AVD_PORT_SECOND 	   ,
	AVD_PORT_THIRD ,
	AVD_PORT_FOURTH	   ,
} VFE_AVD_PORT_TYPE_T;
	
typedef struct
{
	UINT16		w;	/**< horizontal Size(Width)    in pixel [0,].*/
	UINT16		h;	/**< vertical	Size(Height)   in pixel [0,].*/
	UINT16		x;	/**< horizontal Start Position in pixel [0,].*/
	UINT16		y;	/**< vertical	Start Position in pixel [0,].*/
} VFE_AVD_RECT_T;
	
/**
 * Video Timing Information.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	SINT32 format;
	UINT16 hFreq;	/**< ... */
	UINT16 vFreq;	/**< ... */
	UINT16 hTotal;	/**< ... */
	UINT16 vTotal;	/**< ... */
	UINT16 hPorch;	/**< ... */
	UINT16 vPorch;	/**< ... */
	VFE_AVD_RECT_T	active; /**< ... */
	UINT8 scanType; /**< ... */
	BOOLEAN  vdLock;	/**< for ATV */
	BOOLEAN  HLock;
	BOOLEAN  VLock;
	VFE_AVD_AVDECODER_VIDEO_MODE_T	standard;
} VFE_AVD_TIMING_INFO_T;
	
	
/**
 * VDEC fine tune info
 *
 */
typedef struct
{
	UINT8 nVBICCLine;	/* for VBI parsing line select */
	
	UINT8 nHPLLEnable;	/* for enable HPLL tune */
	SINT8 nHPLLTune1;	/* for tunning HPLL */
	SINT8 nHPLLTune2;	/* for tunning HPLL */
	SINT8 nHPLLTune3;	/* for tunning HPLL */
	SINT8 nHPLLTune4;	/* for tunning HPLL */
	
	UINT8 nColorKillEnable;
	SINT8 nColorKillTune1;	/* for tunning Color Kill Level */
	SINT8 nColorKillTune2;	/* for tunning Color Kill Level */
	SINT8 nColorKillTune3;	/* for tunning Color Kill Level */
	SINT8 nColorKillTune4;	/* for tunning Color Kill Level */
	
	SINT8 nLockThreshold1;	/* for tunning Lock sensitivity */
	SINT8 nLockThreshold2;	/* for tunning Lock sensitivity */
	
}VFE_AVD_VDEC_TUNE_T;
	
typedef enum
{
	VIDEO_DDI_VBI_PAL_TELETEXT = 0, /**< VBI teletxet */
	VIDEO_DDI_VBI_PAL_WSS,			/**< VBI wss */
	VIDEO_DDI_VBI_PAL_VPS,			/**< VBI vps */
	VIDEO_DDI_VBI_NTSC_CC608,		/**< VBI cc608 */
	VIDEO_DDI_VBI_NTSC_WSS,
	VIDEO_DDI_VBI_ALL,
	VIDEO_DDI_VBI_MAX_SERVICE,		/**< VBI max */
} VFE_AVD_VBI_TYPE_T;
	
/**
*	VBI Line
*/
typedef enum
{
	VBI_18_LINE = 0,
	VBI_21_LINE,
	VBI_22_LINE,
	VBI_MULTI_LINE,
}VFE_AVD_VBI_LINE_T;
	
typedef enum {
	CVBS_INDEX_1 = 0,
	CVBS_INDEX_2,
	CVBS_INDEX_3,
	CVBS_INDEX_4,
	CVBS_INDEX_5,
	NUM_CVBS_INPUTS
} VIDEO_CVBS_INDEX_T;
	
/**
 * PAL WSS Aspect Ratio.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_WSS_FULLFORMAT_4_3			= 0x8,	/**< wss info*/
	VIDEO_DDI_WSS_LETTERBOX_14_9_CENT		= 0x1,	/**< wss info*/
	VIDEO_DDI_WSS_LETTERBOX_14_9_TOP		= 0x2,	/**< wss info*/
	VIDEO_DDI_WSS_LETTERBOX_16_9_CENT		= 0xb,	/**< wss info*/
	VIDEO_DDI_WSS_LETTERBOX_16_9_TOP		= 0x4,	/**< wss info*/
	VIDEO_DDI_WSS_LETTERBOX_16_9_ZOOM		= 0xd,	/**< wss info*/
	VIDEO_DDI_WSS_FULLFORMAT_14_9			= 0xe,	/**< wss info*/
	VIDEO_DDI_WSS_FULLFORMAT_16_9			= 0x7,	/**< wss info*/
	VIDEO_DDI_WSS_INVALID					= 0x0	/**< wss info*/
} VFE_AVD_WSS_ARC_MODE_T;
	
	
typedef enum
{
	VFE_AVD_PSP_OFF,
	VFE_AVD_PSP_ON_2_LINE_SPLIT_BURST_ON,
	VFE_AVD_PSP_ON_SPLIT_BURST_OFF,
	VFE_AVD_PSP_ON_4LINE_SPLIT_BURST_ON,
} VFE_AVD_PHYSICAL_COPY_PROTECT;
	
typedef enum
{
	VIDEO_DDI_SCART_AR_16_9,	/**< scart aspect ratio [5 ~ 8V] */
	VIDEO_DDI_SCART_AR_4_3, 	/**< scart aspect ratio [9.5 ~ 12V] */
	VIDEO_DDI_SCART_AR_INVALID
}VFE_AVD_SCART_AR_T;
	
typedef enum
{
	VIDEO_DDI_FB_STATUS_CVBS,		/**< scart RF status info*/
	VIDEO_DDI_FB_STATUS_RGB,		/**< scart RF status info*/
}VFE_AVD_SCART_FB_STATUS_T;

typedef enum
{
	VFE_AVD_NOTHING_IS_SUPPORTED,
	VFE_AVD_CC_IS_SUPPORTED,
	VFE_AVD_TTX_IS_SUPPORTED,
	VFE_AVD_CC_AND_TTX_ARE_SUPPORTED,
} VFE_AVD_CC_TTX_STATUS_T;

typedef enum
{
	AVD_INTERNAL_DEMOD,
	AVD_EXTERNAL_DEMOD,
} VFE_AVD_DEMOD_TYPE;
/*-----------------------------------------------------------------------------
	Extern Variables & Function Prototype Declarations
------------------------------------------------------------------------------*/
DTV_STATUS_T 		HAL_AVD_InitializeVBIModule(VFE_AVD_CC_TTX_STATUS_T supportingVBI);

extern DTV_STATUS_T HAL_VFE_AVD_VBI_SetMessageIDOfACC(OSA_MSGQ_T qid4ACCTx, UINT32 oddMsgID, UINT32 evenMsgID,UINT32 mainSrcID, UINT32 subSrcID);

DTV_STATUS_T		HAL_VFE_AVD_Initialize(VFE_AVD_CC_TTX_STATUS_T supportingVBI);
DTV_STATUS_T 		HAL_VFE_AVD_Uninitialize(void);
DTV_STATUS_T  		HAL_VFE_AVD_Open(void);
DTV_STATUS_T  		HAL_VFE_AVD_Close(void);
DTV_STATUS_T  		HAL_VFE_AVD_Connect(VFE_AVD_SRC_TYPE_T SrcType, UINT8 PinPortNumber);
DTV_STATUS_T  		HAL_VFE_AVD_Disconnect(void);
DTV_STATUS_T		HAL_VFE_AVD_SetSyncDetectionForTuning(BOOLEAN bEnable);

#if 0
DTV_STATUS_T 		HAL_VFE_AVD_GetSignalStrengthLevel(UINT8 u1Type, UINT8 *pu1Level);
#endif

BOOLEAN 			HAL_VFE_AVD_DoesSyncExist(void);
DTV_STATUS_T		HAL_VFE_AVD_SetColorSystem(VFE_AVD_COLOR_SYSTEM_T colorSystem);
DTV_STATUS_T		HAL_VFE_AVD_GetColorSystem(VFE_AVD_AVDECODER_VIDEO_MODE_T *pColorSystem);
DTV_STATUS_T 		HAL_VFE_AVD_GetTimingInfo(VFE_AVD_TIMING_INFO_T *pInfo);

#if 0
DTV_STATUS_T 		HAL_VFE_AVD_SetVdecTuning(VFE_AVD_VDEC_TUNE_T * pstVdecTuneInfo, VFE_AVD_COLOR_SYSTEM_T colorSystem);
#endif
DTV_STATUS_T 		HAL_VFE_AVD_SetATVChannelChange(void);


extern DTV_STATUS_T HAL_VFE_AVD_EnableVBI(VFE_AVD_CC_TTX_STATUS_T supportingVBI);
extern DTV_STATUS_T HAL_VFE_AVD_DisableVBI(VFE_AVD_CC_TTX_STATUS_T supportingVBI);
extern DTV_STATUS_T HAL_VFE_AVD_RegisterVBICallBackFunction (VFE_AVD_VBI_TYPE_T type, void *pfnVBICB1);

DTV_STATUS_T  		HAL_VFE_AVD_Get480iWSS20Bits(UINT32 * pWss);
DTV_STATUS_T  		HAL_VFE_AVD_Get576iWSS14Bits(UINT16 * pWss);
DTV_STATUS_T 		HAL_VFE_AVD_GetPhysicalSignalStatusOfCopyProtect(VFE_AVD_PHYSICAL_COPY_PROTECT *pPhysical_Copy);

extern DTV_STATUS_T HAL_VFE_AVD_FlushVBIData(VFE_AVD_VBI_TYPE_T type);
DTV_STATUS_T 		HAL_VFE_AVD_GetScartID(VFE_AVD_SCART_AR_T *pScartAr);
DTV_STATUS_T 		HAL_VFE_AVD_GetScartFastBlankingStatus(VFE_AVD_SCART_FB_STATUS_T *pStatus);

extern void 		HAL_VFE_AVD_SendCCData(unsigned char *buf, UINT32 len);	// 2013.02.19 yongsu.yoo Reconstructing Source Code for WebOS

DTV_STATUS_T HAL_AVD_SetDemodType (VFE_AVD_DEMOD_TYPE demodType);

DTV_STATUS_T HAL_VFE_AVD_Resume(void);


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

/*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
#ifdef	__cplusplus
	}
#endif /* __cplusplus */

#endif /* _VIDEO_SOC_VBI_DDI_H_ */

