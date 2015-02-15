/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_vbe_tridfmt.h
 *
 *  It is the header file for controling 3D formatter function.
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
#ifndef _HAL_VBE_TRIDFMT_H_
#define _HAL_VBE_TRIDFMT_H_

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

/**
 * Define 3D Input LR Type.
 */
typedef enum
{
	VBE_INPUT_LR_SEQ = 0,
	VBE_INPUT_RL_SEQ,
	VBE_INPUT_LR_MAX,
} HAL_TRIDTV_INPUT_LR_SEQ_T;


typedef enum
{
	/*	Drive에서 아래 value 사용 안함.	*/
	VBE_INPUT_TYPE_3DOFF = 0,			//VIDEO_3D_FORMAT_2D,

	//interim format - half
	VBE_INPUT_TYPE_TOPNBOTTOM,		//VIDEO_3D_FORMAT_TOP_BOTTOM,
	VBE_INPUT_TYPE_SIDEBYSIDE,		//VIDEO_3D_FORMAT_SIDEBYSIDE_HALF,
	VBE_INPUT_TYPE_CHECKERBOARD,	//VIDEO_3D_FORMAT_CHECKER_BOARD,
	VBE_INPUT_TYPE_FRAMESEQ,		//VIDEO_3D_FORMAT_FRAME_SEQ
	VBE_INPUT_TYPE_COLUMN_INTERLEAVE,	/**< for H.264*/
	VBE_INPUT_TYPE_LINE_INTERLEAVE_HALF,	/**< for H.264*/

	//Full format
	VBE_INPUT_TYPE_FRAMEPAC,			//VIDEO_3D_FORMAT_FRAME_PACKING
	VBE_INPUT_TYPE_FIELD_ALTERNATIVE,	//VIDEO_3D_FORMAT_FIELD_ALTERNATIVE
	VBE_INPUT_TYPE_LINE_ALTERNATIVE,	//VIDEO_3D_FORMAT_LINE_ALTERNATIVE
	VBE_INPUT_TYPE_SIDEBYSIDE_FULL,		//VIDEO_3D_FORMAT_SIDEBYSIDE_FULL,
	VBE_INPUT_TYPE_DUALSTREAM, 	/**< Full format*/

	/*	Drive에서 아래 value 사용 안함. */
	VBE_INPUT_TYPE_2DTO3D,			//VIDEO_3D_FORMAT_2DTO3D,

	VBE_INPUT_TYPE_MAX,
} HAL_TRIDTV_INPUT_TYPE_T;


/**
 * Define 3D Input Freq. (MAIN SOC OUTPUT FREQ TYPE= FORMATTER INPUT FREQ TYPE)
 */

typedef enum
{
	//scaler output frame rate.
	VBE_INPUT_FREQ_24HZ = 0,
	VBE_INPUT_FREQ_30HZ,
	VBE_INPUT_FREQ_48HZ,
	VBE_INPUT_FREQ_50HZ,
	VBE_INPUT_FREQ_60HZ,

	VBE_INPUT_FREQ_MAX,

} HAL_TRIDTV_INPUT_FREQ_T;


/**
 * Define 3D Output LR Type.
 */
typedef enum
{
	VBE_OUTPUT_RL_SEQ = 0,
	VBE_OUTPUT_LR_SEQ,
	VBE_OUTPUT_LR_MAX,
} HAL_TRIDTV_OUTPUT_LR_SEQ_T;


/**
 * Define 3D Output Type.
 */
typedef enum
{
	VBE_OUTPUT_TYPE_DUAL = 0,
	VBE_OUTPUT_TYPE_INTERIM,
	VBE_OUTPUT_TYPE_FRAMESEQ,
	VBE_OUTPUT_TYPE_FRAMESEQ_LRLR, //60Hz/120Hz/240Hz output
	VBE_OUTPUT_TYPE_FRAMESEQ_LLRR, //120Hz/240Hz output
	VBE_OUTPUT_TYPE_FRAMESEQ_LBRB, //240Hz
	VBE_OUTPUT_TYPE_FRAMESEQ_LURU, //240Hz
	VBE_OUTPUT_TYPE_LINEINTERLEAVED,
	VBE_OUTPUT_TYPE_MAX,
} HAL_TRIDTV_OUTPUT_TYPE_T;


/**
 * Define 3D Output Freq.
 */
typedef enum
{
	VBE_OUTPUT_FREQ_60HZ = 0,
	VBE_OUTPUT_FREQ_100HZ,
	VBE_OUTPUT_FREQ_120HZ,
	VBE_OUTPUT_FREQ_240HZ,
	VBE_OUTPUT_FREQ_50HZ,
	VBE_OUTPUT_FREQ_BYPASS,
	VBE_OUTPUT_FREQ_MAX,
} HAL_TRIDTV_OUTPUT_FREQ_T;


/**
 * Define 3D Cfg items.
 */
typedef enum
{
	VBE_TRIDTV_3DMODE_STATUS = 0,
	VBE_TRIDTV_3D_INPUT_TYPE,
	VBE_TRIDTV_3D_INPUT_LR_SEQ,
//	TRIDTV_3D_INPUT_FREQ,
#if 0
	TRIDTV_3D_OUTPUT_TYPE,
	TRIDTV_3D_OUTPUT_LR_SEQ,
	TRIDTV_3D_OUTPUT_FREQ,
#endif
	VBE_TRIDTV_CFG_ITEMS_MAX,
} HAL_TRIDTV_CFG_ITEMS_T;


 /**
 * Video Main SOC output type
 *
 */
typedef enum
{
	VBE_TRIDTV_3DMODE_OFF= 0,
	VBE_TRIDTV_3DMODE_ON 	,
	VBE_TRIDTV_3DMODE_2DTO3D,
	VBE_TRIDTV_3DMODE_3DTO2D,
	VBE_TRIDTV_3DMODE_MAX,
} HAL_TRIDTV_3DMODE_FLAG_T;

typedef enum
{
    VBE_SC_OUT_SINGLE_2D  = 0 ,///< single nomal
    VBE_SC_OUT_SINGLE_TB      ,///< top and bottom
    VBE_SC_OUT_SINGLE_SS      ,///< side by side
    VBE_SC_OUT_SINGLE_FS      ,///< Frame Sequential(Interleaving)

    VBE_SC_OUT_DUAL_FHD       ,///< dual screen
    VBE_SC_OUT_DUAL_TB        ,///< dual top and bottom for H13 internal type
    VBE_SC_OUT_DUAL_SS        ,///< dual side by side for H13 internal type
    VBE_SC_OUT_SINGLE_T       ,//<H13 DRIVE 요청 SINGLE TOP ONLY>
    VBE_SC_OUT_MAX            ,//MAX
}HAL_TRIDTV_SC_OUTPUT_TYPE_T; // WebOS 1.0

typedef enum
{
    VBE_FMT_SINGLE_2D  = 0 ,///< single nomal
    VBE_FMT_SINGLE_TB      ,///< top and bottom
    VBE_FMT_SINGLE_SS      ,///< side by side
    VBE_FMT_SINGLE_FS      ,///< Frame Sequential(Interleaving)
    VBE_FMT_DUAL_FHD       ,///< dual screen
    VBE_FMT_DUAL_TB        ,///< dual top and bottom for H13 internal type
    VBE_FMT_DUAL_SS        ,///< dual side by side for H13 internal type
    VBE_FMT_SINGLE_T       ,//<H13 DRIVE 요청 SINGLE TOP ONLY>
    VBE_FMT_SINGLE_LA	  ,///< line alternative >
    VBE_FMT_MAX            ,//MAX
}HAL_TRIDTV_FMT_IF_TYPE_T; // 3D formatter interface type (WebOS 1.5)

typedef enum
{
	VBE_HAL_WIN_MODE_MIN = 0,
	VBE_HAL_WIN_MODE_FULLSIZE = VBE_HAL_WIN_MODE_MIN,
	VBE_HAL_WIN_MODE_RESIZE,
	VBE_HAL_WIN_MODE_4BY3,
	VBE_HAL_WIN_MODE_MAXN,
} VBE_WIN_MODE_T;

typedef struct {
	HAL_TRIDTV_3DMODE_FLAG_T	 mode;		///< select formatter operation mode. it controls 3D mode ON/OFF.
	HAL_TRIDTV_SC_OUTPUT_TYPE_T  in_fmt;    ///< output image format(top/bottom, sidebyside, checkboard, frame, field seq)
	HAL_TRIDTV_INPUT_LR_SEQ_T    lr;		///< output left/right first order
	UINT8                    depthCtrl; //0~20
      SINT8                    convCtrl;  //-10~10
	BOOLEAN					bAutoConvMode;	/* TRUE: 3D auto convergence, FALSE: 3D manual convergence */
}HAL_TRIDTV_FMT_CTRL_T; // WebOS 1.0

typedef struct {
	HAL_TRIDTV_3DMODE_FLAG_T	mode;		///< select formatter operation mode. it controls 3D mode ON/OFF.
	HAL_TRIDTV_FMT_IF_TYPE_T  	n3DIfFmtInput;    ///< 3D formatter input
	HAL_TRIDTV_FMT_IF_TYPE_T  	n3DIfFmtOutput;    ///< 3D formatter output
	HAL_TRIDTV_INPUT_LR_SEQ_T    lr;		///< output left/right first order
	UINT8                    depthCtrl; //0~20
	SINT8                    convCtrl;  //-10~10
	BOOLEAN					bAutoConvMode;	/* TRUE: 3D auto convergence, FALSE: 3D manual convergence */
}HAL_TRIDTV_FORMAT_CTRL_T; // WebOS 1.5

typedef struct HAL_TRIDTV_INPUT_CONFIG_INFO
{
	HAL_TRIDTV_3DMODE_FLAG_T	n3DInputMode;	// 0: 3D ON, 1:3D OFF, 2:2DTO3D, 3:3DTO2D
	HAL_TRIDTV_INPUT_TYPE_T 	n3DInputType;	// 1:TNB 2:SBS, 3:
	HAL_TRIDTV_INPUT_LR_SEQ_T	n3DInputLRSeq;	// 0:LR, 1:RL
//	TRIDTV_INPUT_FREQ_T 	n3DInputFreq;	// NOT USED.

	UINT8					n3DInputDepthCtrl;		// 0~10
	SINT8					n3DInputConvertCtrl;	//-10~10
//	UINT16					nVerticalSize;			//
	BOOLEAN					bPositiveDepth;	/* TRUE: Positive depth, FALSE: Negative depth */
	BOOLEAN					bAutoConvMode;	/* TRUE: 3D auto convergence, FALSE: 3D manual convergence */
} HAL_TRIDTV_INPUT_CONFIG_INFO_T;


typedef struct
{
	UINT16		w;	/**< horizontal Size(Width)    in pixel [0,].*/
	UINT16		h;	/**< vertical	Size(Height)   in pixel [0,].*/
	UINT16		x;	/**< horizontal Start Position in pixel [0,].*/
	UINT16		y;	/**< vertical	Start Position in pixel [0,].*/
} VBE_WIN_T;

/*-----------------------------------------------------------------------------
	Static Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

DTV_STATUS_T HAL_VBE_TRIDFMT_Initialize(void);
DTV_STATUS_T HAL_VBE_TRIDFMT_Uninitialize(void);
DTV_STATUS_T HAL_VBE_TRIDFMT_Open(void);
DTV_STATUS_T HAL_VBE_TRIDFMT_Close(void);
DTV_STATUS_T HAL_VBE_TRIDFMT_SetScreenOnOff(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_TRIDFMT_SetFMTMode(HAL_TRIDTV_FMT_CTRL_T *TriFRC);
DTV_STATUS_T HAL_VBE_TRIDFMT_Set3DFormat(HAL_TRIDTV_FORMAT_CTRL_T *p3DFmtInfo);
DTV_STATUS_T HAL_VBE_TRIDFMT_Set3DModeOnOff(HAL_TRIDTV_INPUT_CONFIG_INFO_T *p3DModeInfo);
DTV_STATUS_T HAL_VBE_TRIDFMT_ToggleInputLRFrame(HAL_TRIDTV_INPUT_LR_SEQ_T type);
DTV_STATUS_T HAL_VBE_TRIDFMT_SelectInputFormat(HAL_TRIDTV_INPUT_TYPE_T type);
DTV_STATUS_T HAL_VBE_TRIDFMT_SetInputFrameRate(HAL_TRIDTV_INPUT_FREQ_T type);
DTV_STATUS_T HAL_VBE_TRIDFMT_ToggleOutputLRFrame(HAL_TRIDTV_OUTPUT_LR_SEQ_T type);
DTV_STATUS_T HAL_VBE_TRIDFMT_SelectOutputFormat(HAL_TRIDTV_OUTPUT_TYPE_T type);
DTV_STATUS_T HAL_VBE_TRIDFMT_SetOutputFrameRate(HAL_TRIDTV_OUTPUT_FREQ_T type);
DTV_STATUS_T HAL_VBE_TRIDFMT_Get3DCfgInfo(HAL_TRIDTV_CFG_ITEMS_T n3DCfgItems, UINT8 *nCurCfgInfo);
DTV_STATUS_T HAL_VBE_TRIDFMT_Set3DCfgInfo(HAL_TRIDTV_INPUT_CONFIG_INFO_T *p3DModeInfo);
DTV_STATUS_T HAL_VBE_TRIDFMT_SetConvValue(SINT8 s8ConvValue);
DTV_STATUS_T HAL_VBE_TRIDFMT_CtrlDepth(HAL_TRIDTV_3DMODE_FLAG_T n3Dmode, UINT8 u8DepthLevel);
DTV_STATUS_T HAL_VBE_TRIDFMT_AutoConvergenceEnable(BOOLEAN bOnOff);

DTV_STATUS_T HAL_VBE_TRIDFMT_Set3DVideoOutputMode(UINT32 bIsSGMode);
DTV_STATUS_T HAL_VBE_TRIDFMT_Set3DDepthController(UINT32 bOnOff);
void HAL_VBE_TRIDFMT_Debug(void);


#endif /* _HAL_VBE_TRIDFMT_H_ */
