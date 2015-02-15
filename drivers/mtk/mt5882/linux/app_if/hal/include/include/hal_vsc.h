/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_vsc.h
 *
 *  It is the header file for controling VIDEO function.
 *  The header of VIDEO DDI is controlled by this file.
 *
 *  @author	Lee, Jong-Heon(jongheon.lee@lge.com)
 *  @version	0.1
 *  @date		2011.03.18
 *  @note
 *  @see
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef _HAL_VIDEO_DDI_H_
#define _HAL_VIDEO_DDI_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 	File Inclusions
------------------------------------------------------------------------------*/
//#include "branches.h"
#ifndef WIN32_SIM // Modified by dilly97: fix include
#include "hal_common.h"
#else
#include "common_win32.h"
#endif
//#include "global_configurations.h"

#ifdef	__cplusplus
	extern "C"
	{
#endif /* __cplusplus */

//#include "dil_vsc.h"

/*-----------------------------------------------------------------------------
	Constant Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Macro Definitions
------------------------------------------------------------------------------*/
#if !defined(BIT0) && !defined(BIT1)
#define		BIT0		0x00000001
#define		BIT1		0x00000002
#define		BIT2		0x00000004
#define		BIT3		0x00000008
#define		BIT4		0x00000010
#define		BIT5		0x00000020
#define		BIT6		0x00000040
#define		BIT7		0x00000080
#define		BIT8		0x00000100
#endif

#define FRC_MUTE_MASK (BIT7)
#define VIDEO_DDI_RECT_T VIDEO_RECT_T

/**
 * Video Window Rect.
 *
 * @see DDI Implementation Guide
*/
#ifndef VIDEO_WID_T
	typedef enum
	{
		VIDEO_WID_MIN	= 0,				/**< WID min */
		VIDEO_WID_0 	= VIDEO_WID_MIN,	/**< WID main window */
		VIDEO_WID_1,						/**< WID sub window */
		VIDEO_WID_2,
		VIDEO_WID_3,
		VIDEO_WID_MAX	= VIDEO_WID_3,		/**< WID max */
		VIDEO_WID_MAXN, 					/**< WID max number */
	} __VIDEO_WID_T;
#define VIDEO_WID_T __VIDEO_WID_T
#endif
	/**
	 * Video Window Rect.
	 *
	 * @see DDI Implementation Guide
	*/
#ifndef VIDEO_RECT_T
	typedef struct
	{
		UINT16		x;	/**< horizontal Start Position in pixel [0,].*/
		UINT16		y;	/**< vertical	Start Position in pixel [0,].*/
		UINT16		w;	/**< horizontal Size(Width)    in pixel [0,].*/
		UINT16		h;	/**< vertical	Size(Height)   in pixel [0,].*/
	} __VIDEO_RECT_T;
#define VIDEO_RECT_T __VIDEO_RECT_T
#endif

typedef enum VSC_INPUT_TYPE
{
	VSC_INPUTSRC_AVD = 0,
	VSC_INPUTSRC_ADC,
	VSC_INPUTSRC_HDMI,
	VSC_INPUTSRC_VDEC,
	VSC_INPUTSRC_JPEG,
	VSC_INPUTSRC_MAX = VSC_INPUTSRC_JPEG,
	VSC_INPUTSRC_MAXN,
} VSC_INPUT_TYPE_T;

typedef struct VSC_INPUT_SRC_INFO
{
	VSC_INPUT_TYPE_T type;
	UINT8 attr;
	UINT8 resourceIndex;
} VSC_INPUT_SRC_INFO_T;

/**
 * Scaler Output Mode.
 *
 * @see HAL Implementation Guide
*/
typedef enum VSC_OUTPUT_MODE
{
	VSC_OUTPUT_DISPLAY_MODE = 0,
	VSC_OUTPUT_VENC_MODE,
	VSC_OUTPUT_MEMORY_MODE,
	VSC_OUTPUT_AVE_MODE,
	VSC_OUTPUT_MAX = VSC_OUTPUT_AVE_MODE,
	VSC_OUTPUT_MAXN,
} VSC_OUTPUT_MODE_T;
/**
 * Color of Video Window.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_WIN_COLOR_BLACK,		/**< window color black */
	VIDEO_DDI_WIN_COLOR_BLUE,		/**< window color blue */
	VIDEO_DDI_WIN_COLOR_GRAY		/**< window color gray */
} VIDEO_DDI_WIN_COLOR_T;

/**
 * Multi Window Mode.
 *
*/
typedef enum
{
	VIDEO_MULTI_WIN_MODE_MIN = 0,
	VIDEO_MULTI_WIN_MODE_OFF = VIDEO_MULTI_WIN_MODE_MIN,
	VIDEO_MULTI_WIN_MODE_PIP,
	VIDEO_MULTI_WIN_MODE_TRANSCODING,
	VIDEO_MULTI_WIN_MODE_2ND_TV,
	VIDEO_MULTI_WIN_MODE_CAPTURE,
	VIDEO_MULTI_WIN_MODE_MAXN,
} VIDEO_MULTI_WIN_MODE_T;

/**
 * Video Input source Information.
 *
 */
typedef struct VIDEO_SRC_INFO
{
	UINT32	Type:8;		/**< ... */
	UINT32	Index:8;	/**< ... */
	UINT32	Attr:8;		/**< ... */
	UINT32	Resv:8;		/**< ... */
} VIDEO_SRC_INFO_T;

/**
 * Video Pixel format
 *
 */
typedef enum
{
	VIDEO_DDI_PIXEL_FORMAT_RGB10,
	VIDEO_DDI_PIXEL_FORMAT_RGB8,
	VIDEO_DDI_PIXEL_FORMAT_YCBCR444_10,
	VIDEO_DDI_PIXEL_FORMAT_YCBCR444_8,
	VIDEO_DDI_PIXEL_FORMAT_YCBCR422_10,
	VIDEO_DDI_PIXEL_FORMAT_YCBCR422_8,
	VIDEO_DDI_PIXEL_FORMAT_YCBCR420_10,
	VIDEO_DDI_PIXEL_FORMAT_YCBCR420_8,
	VIDEO_DDI_PIXEL_FORMAT_ARGB8,
	VIDEO_DDI_PIXEL_FORMAT_ARGB10,
   	VIDEO_DDI_PIXEL_FORMAT_YCSEPARATED,
}VIDEO_DDI_PIXEL_FORMAT_T;

/**
 * Video Capture Info
 *
 */
typedef struct
{
	UINT16 width;
	UINT16 height;
	VIDEO_DDI_PIXEL_FORMAT_T pixelFormat;
	const UINT8* pData;
	const UINT8* pCbCr;
	UINT16* pScanTypeCapImg; // 0:I, 1:P
}VIDEO_DDI_CAPTURE_INFO_T;

/**
 * 3D Video Full Timing Information.
 *
 * @see DDI Implementation Guide
*/
/*  have to refer timing info, doesn't refer 3D tag or header info */
typedef enum
{
	/*     아래 enum value는 관리 안함.  */
	VIDEO_DDI_3D_FORMAT_2D,					/**< for No 3D (OFF)..... INPUT_TYPE_2DTO3D */

	/*      From UI, 3D ON이면서 아래 Format이 넘어오면 일반 3D ON이 됨.    */
	//interim format - half
	VIDEO_DDI_3D_FORMAT_TOP_BOTTOM,			/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_TOPNBOTTOM*/
	VIDEO_DDI_3D_FORMAT_SIDEBYSIDE_HALF,	/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_SIDEBYSIDE*/
	VIDEO_DDI_3D_FORMAT_CHECKER_BOARD,		/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_CHECKERBOARD*/
	VIDEO_DDI_3D_FORMAT_FRAME_SEQ,			/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_FRAMESEQ*/
	VIDEO_DDI_3D_FORMAT_COLUMN_INTERLEAVE,	/**< for H.264*/
	VIDEO_DDI_3D_FORMAT_LINE_INTERLEAVE_HALF,	/**< for H.264*/

	//Full format
	VIDEO_DDI_3D_FORMAT_FRAME_PACKING,			/**< Full format*/
	VIDEO_DDI_3D_FORMAT_FIELD_ALTERNATIVE,		/**< Full format*/
	VIDEO_DDI_3D_FORMAT_LINE_ALTERNATIVE,		/**< Full format (row interleave)*/
	VIDEO_DDI_3D_FORMAT_SIDEBYSIDE_FULL,		/**< Full format*/
	VIDEO_DDI_3D_FORMAT_DUALSTREAM,		/**< Full format*/
	/*    From UI, 3D ON이면서 아래 Format이 넘어오면 2D to 3D 가 됨    */
	VIDEO_DDI_3D_FORMAT_2DTO3D,					/**< for2D TO  3D..... INPUT_TYPE_2DTO3D */
	VIDEO_DDI_3D_FORMAT_MAX,
}VIDEO_DDI_3D_FORMAT_TYPE_T;

/**
 * 3D Video Full Timing Information.
 *
 * @see DDI Implementation Guide
*/
/*  have to refer timing info, doesn't refer 3D tag or header info */
typedef enum
{
	/*     아래 enum value는 관리 안함.  */
//	VIDEO_DDI_3D_PATTERN_2D			        = BIT0 << VIDEO_DDI_3D_FORMAT_2D                ,

	/*      From UI, 3D ON이면서 아래 Format이 넘어오면 일반 3D ON이 됨.    */
	//interim format - half
	VIDEO_DDI_3D_PATTERN_TOP_BOTTOM         = BIT0 << VIDEO_DDI_3D_FORMAT_TOP_BOTTOM        ,
	VIDEO_DDI_3D_PATTERN_SIDEBYSIDE_HALF    = BIT0 << VIDEO_DDI_3D_FORMAT_SIDEBYSIDE_HALF   ,
	VIDEO_DDI_3D_PATTERN_CHECKER_BOARD      = BIT0 << VIDEO_DDI_3D_FORMAT_CHECKER_BOARD     ,
	VIDEO_DDI_3D_PATTERN_FRAME_SEQ          = BIT0 << VIDEO_DDI_3D_FORMAT_FRAME_SEQ         ,
	VIDEO_DDI_3D_PATTERN_COLUMN_INTERLEAVE  = BIT0 << VIDEO_DDI_3D_FORMAT_COLUMN_INTERLEAVE ,
	VIDEO_DDI_3D_PATTERN_LINE_INTERLEAVE_HALF	= BIT0 << VIDEO_DDI_3D_FORMAT_LINE_INTERLEAVE_HALF ,

	//Full format
	VIDEO_DDI_3D_PATTERN_FRAME_PACKING       = BIT0 << VIDEO_DDI_3D_FORMAT_FRAME_PACKING     ,
	VIDEO_DDI_3D_PATTERN_FIELD_ALTERNATIVE   = BIT0 << VIDEO_DDI_3D_FORMAT_FIELD_ALTERNATIVE ,
	VIDEO_DDI_3D_PATTERN_LINE_ALTERNATIVE    = BIT0 << VIDEO_DDI_3D_FORMAT_LINE_ALTERNATIVE  ,
	VIDEO_DDI_3D_PATTERN_SIDEBYSIDE_FULL     = BIT0 << VIDEO_DDI_3D_FORMAT_SIDEBYSIDE_FULL   ,
	VIDEO_DDI_3D_PATTERN_DUALSTREAM          = BIT0 << VIDEO_DDI_3D_FORMAT_DUALSTREAM        ,

}VIDEO_DDI_3D_PATTERN_TYPE_T;

/**
 * 3D L/R selection
 * .
*/
typedef enum
{
	VIDEO_DDI_3D_LR,	//Driver와 일치.
	VIDEO_DDI_3D_RL,
}VIDEO_DDI_3D_LR_TYPE_T;

/**
 * 3D Dual Mode Information.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_DUAL_2D,			/**< for No Dual (OFF) */
	VIDEO_DDI_DUAL_TOP_BOTTOM,	/**< for T/B Dual Mode */
	VIDEO_DDI_DUAL_SIDEBYSIDE,	/**< for S/S Dual Mode */
}VIDEO_DDI_DUAL_TYPE_T;

/**
 * Video Timing Information.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
   	SINT32 format;	/**< update by LG */
	UINT16 hFreq;	/**< ... */
	UINT16 vFreq;	/**< ... */
	UINT16 hTotal;	/**< ... */
	UINT16 vTotal;	/**< ... */
	UINT16 hPorch;	/**< ... */
	UINT16 vPorch;	/**< ... */
	VIDEO_RECT_T	active;	/**< ... */
	UINT16 scanType;	/**< ... */
	UINT16 phase;	    /**< ... */  // need to check UINT16
	UINT8  mode;	    /**< update by LG */
//	VFE_AVD_AVDECODER_VIDEO_MODE_T  standard;	/**< for color standard*/ // 2012.10.31 yongsu.yoo change this for Late-return of analog color system
	UINT8  vdLock;	/**< for ATV */
	/* 3D restructuring by seongcheoll.kim 2011.07.01 */
	VIDEO_DDI_3D_FORMAT_TYPE_T full_3d_timing;
	VIDEO_DDI_3D_LR_TYPE_T	video_LR_order;
} VIDEO_TIMING_INFO_T;

#if 1
/**
 * pixel grab color.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	UINT8 pixelGrabCb;	/**< Cb */
	UINT8 pixelGrabY;	/**< Y */
	UINT8 pixelGrabCr;	/**< CR */
}VIDEO_DDI_PIXEL_GRAB_COLOR_T;

/**
 * pixel grab region.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	VIDEO_WID_T wID;	/**< select main or sub window */
	UINT32 pixelGrabX;	/**< x position */
	UINT32 pixelGrabY;	/**< y position */
	UINT32 pixelGrabW;	/**< capture width */
	UINT32 pixelGrabH;	/**< capture h */

	UINT32 realPixelGrabW;	/**< real W in memory */
	UINT32 realPixelGrabH;	/**< real H in memory */
	UINT8 colorSpace;	/**< 0: YUV 1:RGB */
}VIDEO_DDI_PIXEL_GRAB_REGION_T;

/**
 * standard pixel color.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	UINT16 pixelStandardCb;	/**< standard pixel color Cb */
	UINT16 pixelStandardY;	/**< standard pixel color Y */
	UINT16 pixelStandardCr;	/**< standard pixel color Cr */
}VIDEO_DDI_PIXEL_STANDARD_COLOR_T;

/**
 * color standard.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_COLOR_STANDARD_YUV = 0,
	VIDEO_DDI_COLOR_STANDARD_RGB,
} VIDEO_DDI_COLOR_STANDARD_T;

/**
 * color standard.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_PIXEL_8BIT = 0,
	VIDEO_DDI_PIXEL_10BIT,
} VIDEO_DDI_PIXEL_COLOR_FORMAT_T;

/**
 * capture location
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_SCALER_OUTPUT = 0,
	VIDEO_DDI_DISPLAY_OUTPUT,
}VIDEO_DDI_CAPTURE_PLACE_T;
#endif

/**
 * Video output format type for UD display
 */
typedef struct {
	BOOLEAN isUD;
	UINT16 fr_rate;						///< display frame rate
	UINT16 h_total;						///< pixels per line
	UINT16 v_total;						///< lines per frame
}VIDEO_DDI_DIS_FMT_T;

/**
 * Video UD format
 * 2011.11.08 by daegun.park
 */
typedef enum
{
	UD_MODE_FHD,
	UD_MODE_UD,
	UD_MODE_UD_HALF,
}VIDEO_UD_MODE_T;


/**
 * Video Component Index
 *
 */
typedef enum {
	COMP_INDEX_1 = 0,
	COMP_INDEX_2,
	COMP_INDEX_3,	// 20091111 drmyung : add
	NUM_COMP_INPUTS
} VIDEO_COMP_INDEX_T;

/**
 * Video Input signal source definition.
 *
 */
typedef enum VIDEO_SRC_TYPE
{
	VIDEO_SRC_TV	= 0,	/**< TV Input */
	VIDEO_SRC_SCART,		/**< Scart Input */
	VIDEO_SRC_AV,			/**< AV Input */
	VIDEO_SRC_AUTOAV,		/**< Auto AV(SCART) Input */
	VIDEO_SRC_COMP,			/**< Component(YPbPr) Input */
	VIDEO_SRC_RGB,			/**< RGB Input */
	VIDEO_SRC_HDMI,			/**< HDMI Input */
#ifdef INCLUDE_DVR_READY
	VIDEO_SRC_PLAYHDD,		/**< PVR : Recorded Playback mode */
#endif
	VIDEO_SRC_USB,			/**< ... */
	VIDEO_SRC_BT,			/**< ... */
	VIDEO_SRC_MEDIASHARE,	/**< ... */
	VIDEO_SRC_PICWIZARD,	/**< ... */
	VIDEO_SRC_SUPPORT,		/**< ... */
	VIDEO_SRC_BB,			/**< ... */
	VIDEO_SRC_FLASH,		/**< ... */
	VIDEO_SRC_VCS,			/**< ... */
	VIDEO_SRC_INVALID,		/**< Invalid Source Input */
	VIDEO_SRC_MAX = VIDEO_SRC_INVALID,	/**< MAX Input Number */
} VIDEO_SRC_TYPE_T;

/**
 * TV Src Attribute definition.
 *
 */
typedef enum VIDEO_TV_SRC_ATTR
{
	VIDEO_TV_ATTR_ANALOG = 0,	/**< ... */
	VIDEO_TV_ATTR_DIGITAL		/**< ... */

} VIDEO_TV_SRC_ATTR_T;

/**
 * EMF attribute definitions.
 *
 */
typedef enum VIDEO_EMF_SRC_ATTR
{
	VIDEO_EMF_ATTR_PHOTO_MUSIC		= 0,	/**< ... */
	VIDEO_EMF_ATTR_MOVIE,					/**< ... */
	VIDEO_EMF_ATTR_VCS, 	/**< ... */

} VIDEO_EMF_SRC_ATTR_T;

#ifndef TRIDTV_SC_OUTPUT_TYPE_T
typedef enum
{
	SC_OUT_SINGLE_2D  = 0 ,///< single nomal
	SC_OUT_SINGLE_TB      ,///< top and bottom
	SC_OUT_SINGLE_SS      ,///< side by side
	SC_OUT_SINGLE_FS      ,///< Frame Sequential(Interleaving)

	SC_OUT_DUAL_FHD       ,///< dual screen
	SC_OUT_DUAL_TB        ,///< dual top and bottom for H13 internal type
	SC_OUT_DUAL_SS        ,///< dual side by side for H13 internal type
	SC_OUT_SINGLE_T       ,//<H13 DRIVE 요청 SINGLE TOP ONLY>
	SC_OUT_MAX            ,//MAX
}__TRIDTV_SC_OUTPUT_TYPE_T;
#define TRIDTV_SC_OUTPUT_TYPE_T __TRIDTV_SC_OUTPUT_TYPE_T
#endif

#ifndef TRIDTV_INPUT_TYPE_T
typedef enum
{
	/*	Drive에서 아래 value 사용 안함.	*/
	INPUT_TYPE_3DOFF = 0,			//VIDEO_3D_FORMAT_2D,

	//interim format - half
	INPUT_TYPE_TOPNBOTTOM,		//VIDEO_3D_FORMAT_TOP_BOTTOM,
	INPUT_TYPE_SIDEBYSIDE,		//VIDEO_3D_FORMAT_SIDEBYSIDE_HALF,
	INPUT_TYPE_CHECKERBOARD,	//VIDEO_3D_FORMAT_CHECKER_BOARD,
	INPUT_TYPE_FRAMESEQ,		//VIDEO_3D_FORMAT_FRAME_SEQ
	INPUT_TYPE_COLUMN_INTERLEAVE,	/**< for H.264*/
	INPUT_TYPE_LINE_INTERLEAVE_HALF,	/**< for H.264*/

	//Full format
	INPUT_TYPE_FRAMEPAC,			//VIDEO_3D_FORMAT_FRAME_PACKING
	INPUT_TYPE_FIELD_ALTERNATIVE,	//VIDEO_3D_FORMAT_FIELD_ALTERNATIVE
	INPUT_TYPE_LINE_ALTERNATIVE,	//VIDEO_3D_FORMAT_LINE_ALTERNATIVE
	INPUT_TYPE_SIDEBYSIDE_FULL,		//VIDEO_3D_FORMAT_SIDEBYSIDE_FULL,
	INPUT_TYPE_DUALSTREAM, 	/**< Full format*/

	/*	Drive에서 아래 value 사용 안함. */
	INPUT_TYPE_2DTO3D,			//VIDEO_3D_FORMAT_2DTO3D,

	INPUT_TYPE_MAX,
}__TRIDTV_INPUT_TYPE_T;
#define TRIDTV_INPUT_TYPE_T __TRIDTV_INPUT_TYPE_T
#endif
/**
 * Define 3D Input LR Type.
 */
#ifndef  TRIDTV_INPUT_LR_SEQ_T
typedef enum
{
	INPUT_LR_SEQ = 0,
	INPUT_RL_SEQ,
	INPUT_LR_MAX,
} __TRIDTV_INPUT_LR_SEQ_T;
#define TRIDTV_INPUT_LR_SEQ_T __TRIDTV_INPUT_LR_SEQ_T
#endif

#ifndef TRIDTV_3DMODE_FLAG_T
typedef enum
{
	TRIDTV_3DMODE_OFF= 0,
	TRIDTV_3DMODE_ON 	,
	TRIDTV_3DMODE_2DTO3D,
	TRIDTV_3DMODE_3DTO2D,
	TRIDTV_3DMODE_MAX,
} __TRIDTV_3DMODE_FLAG_T;
#define TRIDTV_3DMODE_FLAG_T __TRIDTV_3DMODE_FLAG_T
#endif

#ifndef TRIDTV_SCALER_CTRL_T
typedef struct {

	TRIDTV_3DMODE_FLAG_T     mode;		///< select formatter operation mode. it controls 3D mode ON/OFF.
	TRIDTV_INPUT_TYPE_T		 in_fmt;	///< input image format(top/bottom, sidebyside, checkboard, frame, field seq, dual 2d)
	TRIDTV_SC_OUTPUT_TYPE_T  out_fmt;	///< output image format(top/bottom, sidebyside, checkboard, frame, field seq)
	TRIDTV_INPUT_LR_SEQ_T    lr;		///< output left/right first order
}__TRIDTV_SCALER_CTRL_T;
#define TRIDTV_SCALER_CTRL_T __TRIDTV_SCALER_CTRL_T
#endif

typedef enum {
	VIDEO_SEAMLESS_OFF,
	VIDEO_SEAMLESS_AUTO,      ///< Neflix
	VIDEO_SEAMLESS_EXTERNAL,  ///< BBC
	VIDEO_SEAMLESS_INTERNAL,  ///< Amazon
	VIDEO_SEAMLESS_MAX
} VIDEO_SEAMLESS_MODE_T;

/*-----------------------------------------------------------------------------
	Extern Variables & Function Prototype Declarations
------------------------------------------------------------------------------*/
DTV_STATUS_T	HAL_VSC_Initialize(void);
DTV_STATUS_T	HAL_VSC_Uninitialize(void);
DTV_STATUS_T	HAL_VSC_Open(VIDEO_WID_T wId);
DTV_STATUS_T	HAL_VSC_Close(VIDEO_WID_T wId);
DTV_STATUS_T	HAL_VSC_Connect(VIDEO_WID_T wId, VSC_INPUT_SRC_INFO_T vscInput, VSC_OUTPUT_MODE_T outputmode);
DTV_STATUS_T	HAL_VSC_Disconnect(VIDEO_WID_T wId, VSC_INPUT_SRC_INFO_T vscInput, VSC_OUTPUT_MODE_T outputmode);

DTV_STATUS_T	HAL_VSC_SetWinBlank(VIDEO_WID_T wId, BOOLEAN bOnOff, VIDEO_DDI_WIN_COLOR_T color);
DTV_STATUS_T	HAL_VSC_GetInputRegion(VIDEO_WID_T wId, VIDEO_RECT_T *pInRegion);
DTV_STATUS_T	HAL_VSC_SetInputRegion(VIDEO_WID_T wId, VIDEO_RECT_T inRegion);
DTV_STATUS_T	HAL_VSC_GetOutputRegion(VIDEO_WID_T wId, VIDEO_RECT_T *pOutRegion);
DTV_STATUS_T	HAL_VSC_SetOutputRegion(VIDEO_WID_T wId, VIDEO_RECT_T outRegion);
DTV_STATUS_T	HAL_VSC_SetVENCMode (UINT16 frame_rate, UINT16 scan_type);
DTV_STATUS_T	HAL_VSC_SetWinFreeze(VIDEO_WID_T wId, BOOLEAN bOnOff);

DTV_STATUS_T	HAL_VSC_CaptureVideoFrame(VIDEO_DDI_CAPTURE_PLACE_T place, VIDEO_DDI_CAPTURE_INFO_T* pCaptureInfo);
DTV_STATUS_T	HAL_VSC_FreezeVideoFrameBuffer(BOOLEAN bFreezeOn);
DTV_STATUS_T	HAL_VSC_ReadVideoFrameBuffer(VIDEO_WID_T wId, VIDEO_RECT_T *pIn, VIDEO_DDI_PIXEL_STANDARD_COLOR_T *pRead, VIDEO_DDI_COLOR_STANDARD_T *pcolor_standard, VIDEO_DDI_PIXEL_COLOR_FORMAT_T * pPixelColorFormat);
DTV_STATUS_T	HAL_VSC_WriteVideoFrameBuffer(VIDEO_WID_T wId, VIDEO_RECT_T *pIn, VIDEO_DDI_PIXEL_STANDARD_COLOR_T *pWrite);

DTV_STATUS_T	HAL_VSC_SetRGB444Mode(BOOLEAN bOnOff);
DTV_STATUS_T	HAL_VSC_Set3DForScaler(TRIDTV_SCALER_CTRL_T *p3DCfgInfo);
DTV_STATUS_T	HAL_VSC_Set3DpatternMode(VIDEO_WID_T wId, VIDEO_DDI_3D_PATTERN_TYPE_T mode);
DTV_STATUS_T	HAL_VSC_Get3DAutoDetectmode(VIDEO_WID_T wId, VIDEO_DDI_3D_FORMAT_TYPE_T* pVideo3Dtype);

DTV_STATUS_T 	HAL_VSC_OpenV2G(VIDEO_WID_T wId);
DTV_STATUS_T 	HAL_VSC_CloseV2G(VIDEO_WID_T wId);
DTV_STATUS_T 	HAL_VSC_GetV2GFramebuffer(VIDEO_WID_T wId, VIDEO_DDI_CAPTURE_INFO_T* pCaptureInfo);

DTV_STATUS_T	HAL_VSC_SetUDInput(VIDEO_UD_MODE_T videoUDMode);
DTV_STATUS_T 	HAL_VSC_SetUDOutput(VIDEO_DDI_DIS_FMT_T disFmt);

DTV_STATUS_T	HAL_VSC_GetVideoDelayedTime (VIDEO_WID_T eWinId, UINT16 *pDelayedTime);

DTV_STATUS_T 	HAL_VSC_SetTileMode(BOOLEAN bOnOff);
DTV_STATUS_T 	HAL_VSC_SetTileMaxRowVal(UINT8 uMaxRow, UINT8 uMaxCol);
DTV_STATUS_T 	HAL_VSC_SetTileID(UINT8 uTileID);

DTV_STATUS_T 	HAL_VSC_SetAdaptiveStream(BOOLEAN bOnOff);
DTV_STATUS_T 	HAL_VSC_SetSeamlessMode( UINT16 seamMode );
DTV_STATUS_T    HAL_VSC_SetLocalMute( UINT16 bOnOff );

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
	Global Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
 /*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
#ifdef	__cplusplus
	}
#endif /* __cplusplus */

#endif /* _HAL_VIDEO_DDI_H_ */
