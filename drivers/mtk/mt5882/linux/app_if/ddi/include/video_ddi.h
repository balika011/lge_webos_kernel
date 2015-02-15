/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file video_ddi.h
 *
 *  It is the header file for controling VIDEO function.
 *  The header of VIDEO DDI is controlled by this file.
 *
 *  @author		Lee, Jong-Hun(drmyung@lge.com)
 *  @author		Lee, illo(b215@lge.com)
 *  @version	0.3
 *  @date		2006.03.16
 *  @date		2008.03.06
 *  @note
 *  @see		video_ddi.c
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef	_VIDEO_DDI_H_
#define	_VIDEO_DDI_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/
#ifdef INCLUDE_VCS
#define PIP_SUPPORT 1 // NetCast 3.0 SU1�� PIP Spec. In
#else
#define PIP_SUPPORT 1 // NetCast 3.0 Initial Spec. In
#endif

#define HDMI_444_MODE 1
/*-----------------------------------------------------------------------------
    File Inclusions
------------------------------------------------------------------------------*/
#include "branches.h"
#ifndef WIN32_SIM // Modified by dilly97: fix include
#include "common.h"
#else
#include "common_win32.h"
#endif
#include "global_configurations.h"
#include "osa_api.h"
#include "nvm_ddi.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef ADDING_VIDEO_PORCH_TIMING
#define ADDING_VIDEO_PORCH_TIMING	// jongheon.lee 20111024
#endif
/*-----------------------------------------------------------------------------
	Constant Definitions
------------------------------------------------------------------------------*/
#define		BIT0		0x00000001
#define		BIT1		0x00000002
#define		BIT2		0x00000004
#define		BIT3		0x00000008
#define		BIT4		0x00000010
#define		BIT5		0x00000020
#define		BIT6		0x00000040
#define		BIT7		0x00000080
#define		BIT8		0x00000100
/*-----------------------------------------------------------------------------
	Macro Definitions
------------------------------------------------------------------------------*/
#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x)	(sizeof(x)/sizeof(x[0]))
#endif
#define VIDEO_SUCCESS(e)      ((e) == API_OK)
#define VIDEO_ERROR(e)        ((e) != API_OK)

#define VIDEO_HDMI_PACKET_DATA_LENGTH 28
#define VIDEO_HDMI_INFOFRAME_PACKET_LEN 28
#define VIDEO_HDMI_VENDOR_SPECIFIC_REGID_LEN 3
#define VIDEO_HDMI_VENDOR_SPECIFIC_PAYLOAD_LEN \
    (VIDEO_HDMI_INFOFRAME_PACKET_LEN - VIDEO_HDMI_VENDOR_SPECIFIC_REGID_LEN)

#define VIDEO_HDMI_SPD_IF_VENDOR_LEN          8
#define VIDEO_HDMI_SPD_IF_DESC_LEN            16

//(ykkim5 091207).Range of RGB Control....VPosition  .
//UI  Matching ... ui_menu_subpccontrol.h ġؾ .
#define MIN_HPOS_OF_RGB       (-255) //̵  .
#define MAX_HPOS_OF_RGB        256

#define MIN_SIZE_OF_RGB       (-127)
#define MAX_SIZE_OF_RGB       126

#define MIN_PHASE_OF_RGB        0
#define MAX_PHASE_OF_RGB        63

#define NEXUS_VIDEOFORMAT_E1080P50HZ_LG NEXUS_VideoFormat_eCustom1
#define NEXUS_VIDEOFORMAT_E1080P60HZ_LG NEXUS_VideoFormat_eCustom0

#define VIDEO_ERR_PRINT		dbgprint

/*----------------------------------------------------------------------------
    Type Definitions
------------------------------------------------------------------------------*/

/**
 * Video Window ID.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_WID_MIN	= 0,				/**< WID min */
	VIDEO_WID_0 	= VIDEO_WID_MIN,	/**< WID main window */
	VIDEO_WID_1,						/**< WID sub window */
	VIDEO_WID_MAX	= VIDEO_WID_1,		/**< WID max */
	VIDEO_WID_MAXN,						/**< WID max number */
} VIDEO_WID_T;

/**
 * VBI Type.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_VBI_PAL_TELETEXT = 0,	/**< VBI teletxet */
	VIDEO_DDI_VBI_PAL_WSS,			/**< VBI wss */
	VIDEO_DDI_VBI_PAL_VPS,			/**< VBI vps */
	VIDEO_DDI_VBI_NTSC_CC608,		/**< VBI cc608 */
//	VIDEO_DDI_VBI_DVB_TELETEXT,
//	VIDEO_DDI_VBI_DVB_SUBTITLE,
//	VIDEO_DDI_VBI_ATSC_CC608,
//	VIDEO_DDI_VBI_ATSC_CC708,
	VIDEO_DDI_VBI_MAX_SERVICE,		/**< VBI max */
} VIDEO_DDI_VBI_TYPE_T;

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
 * Video Timing Mode
 *
*/
typedef enum
{
	VIDEO_TV_TIMING_MODE = 0,
	VIDEO_PC_TIMING_MODE,
	VIDEO_OVERLAP_TIMING_MODE,
} VIDEO_DDI_TIMING_MODE_T;


/**
 * PC SCREEN adjust mode.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_ADJ_PC_AUTO = 0,		/**< pc auto */
	VIDEO_DDI_ADJ_PC_H_POS,			/**< pc h position */
	VIDEO_DDI_ADJ_PC_V_POS,			/**< pc v positon */
	VIDEO_DDI_ADJ_PC_HV_POS,		/**< manual update flag (Mstar:h/v pos, BCM:clock&size */
	VIDEO_DDI_ADJ_PC_CLOCK,			/**< pc clock */
	VIDEO_DDI_ADJ_PC_PHASE,			/**< pc phase */
} VIDEO_DDI_ADJ_PC_MODE_T;

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
} VIDEO_DDI_WSS_ARC_MODE_T;

/**
 * Frond-End intput source type.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	VIDEO_DDI_FB_STATUS_CVBS,		/**< scart RF status info*/
	VIDEO_DDI_FB_STATUS_RGB,		/**< scart RF status info*/
} VIDEO_DDI_SCART_FB_STATUS_T;

/**
 * Scart Aspect Ratio.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
    VIDEO_DDI_SCART_AR_16_9,	/**< scart aspect ratio [5 ~ 8V] */
    VIDEO_DDI_SCART_AR_4_3,  	/**< scart aspect ratio [9.5 ~ 12V] */
    VIDEO_DDI_SCART_AR_INVALID
} VIDEO_DDI_SCART_AR_T;

/**
 * type of adc adjusting.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	ADC_COMPONENT,		/**< adc input*/
	ADC_RGB,				/**< adc input*/
	ADC_MAX_INPUT			/**< adc input*/
} VIDEO_DDI_ADC_SRC_T;


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
 * Scart input attribute recognized from SCART FB.
 *
 */
typedef enum VIDEO_SCART_SRC_ATTR
{
	VIDEO_SCART_ATTR_CVBS = 0,	/**< ... */
	VIDEO_SCART_ATTR_RGB		/**< ... */

} VIDEO_SCART_SRC_ATTR_T;

/**
 * Composite input attribute recognized from gpio port.
 *
 */
typedef enum VIDEO_RCA_SRC_ATTR
{
	VIDEO_RCA_ATTR_CVBS = 0,	/**< ... */
	VIDEO_RCA_ATTR_SVIDEO		/**< ... */

} VIDEO_RCA_SRC_ATTR_T;

/**
 * MEDIASHARE Src Index definition.
 *
 */
typedef enum VIDEO_MEDIASHARE_SRC_INDEX
{
	VIDEO_MEDIASHARE_INDEX_DLNA = 0,	/**< ... */
	VIDEO_MEDIASHARE_INDEX_CIFS,		/**< ... */

} VIDEO_MEDIASHARE_SRC_INDEX_T;

/**
 * BB Src Index definition.
 *
 */
typedef enum VIDEO_BB_SRC_INDEX
{
	VIDEO_BB_INDEX_NETFLIX = 0,	/**< ... */
	VIDEO_BB_INDEX_YAHOO,		/**< ... */
	VIDEO_BB_INDEX_YOUTUBE,		/**< ... */
	VIDEO_BB_INDEX_IPLAYER,		/**< ... */
	VIDEO_BB_INDEX_ORANGETV,	/**< ... */

} VIDEO_BB_SRC_INDEX_T;

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
 * Video Path Index.
 *
 */
typedef	enum VIDEO_PATH_INDEX
{
	MAIN_VIDEO_PATH	= 0,	/**< ... */
	SUB_VIDEO_PATH,			/**< ... */
	MAX_VIDEO_PATH			/**< ... */

} VIDEO_PATH_INDEX_T;

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
 * Video Source Group Information.
 *
 */
typedef enum VIDEO_ARC_INDEX
{
	VIDEO_ARC_DTV	= 0,	/**< ... */	// DTV/DVR_READY Index
	VIDEO_ARC_ATV,			/**< ... */			// ATV Index
	VIDEO_ARC_SCART,		/**< ... */		// SCART Index
	VIDEO_ARC_AV,			/**< ... */			// AV Index
	VIDEO_ARC_AUTOAV,		/**< ... */		// AUTOAV Index
	VIDEO_ARC_COMP,			/**< ... */			// COMP Index
	VIDEO_ARC_RGB,			/**< ... */			// RGB Index
	VIDEO_ARC_HDMI,			/**< ... */			// HDMI Index
	VIDEO_ARC_MAX			/**< ... */			// MAX Index Table

} VIDEO_ARC_INDEX_T;

/**
 * 3D Video Format Information.
 * 3D pattren recognition without 3D VSI or SEI  info
 * @see DDI Implementation Guide

typedef enum
{
	VIDEO_2D_PATTERN,
	VIDEO_3D_PATTERN_TOPNBOTTOM,
	VIDEO_3D_PATTERN_SIDEBYSIDE,
	VIDEO_3D_PATTERN_CHECKERBOARD,
	VIDEO_3D_PATTERN_FRAMESEQ,
	VIDEO_3D_PATTERN_ROW_INTERLEAVING,
	VIDEO_3D_PATTERN_COLUMN_INTERLEAVING,
} VIDEO_3D_PATTERN_RECOGNITION_T;
*/

/**
 * 3D Video Full Timing Information.
 *
 * @see DDI Implementation Guide
*/
/*  have to refer timing info, doesn't refer 3D tag or header info */
typedef enum
{
	/*     Ʒ enum value  .  */
	VIDEO_DDI_3D_FORMAT_2D,					/**< for No 3D (OFF)..... INPUT_TYPE_2DTO3D */

	/*      From UI, 3D ON̸�?Ʒ Format Ѿ Ϲ 3D ON .    */
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
	/*    From UI, 3D ON̸�?Ʒ Format Ѿ 2D to 3D      */
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
	/*     Ʒ enum value  .  */
	VIDEO_DDI_3D_PATTERN_2D			        = BIT0 << VIDEO_DDI_3D_FORMAT_2D                ,

	/*      From UI, 3D ON̸�?Ʒ Format Ѿ Ϲ 3D ON .    */
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
	VIDEO_DDI_3D_LR,	//Driver�� ��ġ.
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
 * Video Window Rect.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	UINT16		w;	/**< horizontal Size(Width)    in pixel [0,].*/
	UINT16		h;	/**< vertical   Size(Height)   in pixel [0,].*/
	UINT16		x;	/**< horizontal Start Position in pixel [0,].*/
	UINT16		y;	/**< vertical   Start Position in pixel [0,].*/
} VIDEO_DDI_RECT_T;


// 2012.10.31 yongsu.yoo move the position of this enum for Late-return of analog color system

/**
 * AVDEC : Analog Video Decoder Sync Locking Sensitivity.
 *
 * @see
 */
typedef enum
{
    VIDEO_AVDEC_MODE_NTSC        = 0,
    VIDEO_AVDEC_MODE_PAL            ,
    VIDEO_AVDEC_MODE_PAL_N          ,
    VIDEO_AVDEC_MODE_PAL_M          ,
    VIDEO_AVDEC_MODE_SECAM          ,
    VIDEO_AVDEC_MODE_NTSC_443       ,
    VIDEO_AVDEC_MODE_PAL_60         ,
    VIDEO_AVDEC_MODE_UNKNOWN_525    ,
    VIDEO_AVDEC_MODE_UNKNOWN_625    ,
    VIDEO_AVDEC_MODE_UNKNOWN        ,
}VIDEO_AVDECODER_VIDEO_MODE_T;


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
	VIDEO_DDI_RECT_T	active;	/**< ... */
	UINT16 scanType;	/**< ... */
	UINT16 phase;	    /**< ... */  // need to check UINT16
	UINT8  mode;	    /**< update by LG */
	VIDEO_AVDECODER_VIDEO_MODE_T  standard;	/**< for color standard*/ // 2012.10.31 yongsu.yoo change this for Late-return of analog color system
	UINT8  vdLock;	/**< for ATV */
	/* 3D restructuring by seongcheoll.kim 2011.07.01 */
	VIDEO_DDI_3D_FORMAT_TYPE_T full_3d_timing;
	VIDEO_DDI_3D_LR_TYPE_T	video_LR_order;
} VIDEO_TIMING_INFO_T;


/**
 * Each component of adjustment RGB-PC.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	SINT16	pcHDelay;	   /**< Horizontal Sync Delay �� */
	SINT16	pcVDelay;	   /**< Vertical Sync Delay �� */
	SINT8	pcClock;		 /**< Clock �� */
	UINT8	pcPhase;		 /**< Phase �� */
} VIDEO_DDI_ADJ_PC_T;

#if (PLATFORM_TYPE == BCM_PLATFORM)
/**
 * pixel grab color.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	UINT32 pixelGrabCb;	/**< Cb */
	UINT32 pixelGrabY;	/**< Y */
	UINT32 pixelGrabCr;	/**< CR */
}VIDEO_DDI_PIXEL_GRAB_COLOR_T;

#else
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
#endif

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

// 2012.06.20 yongsu.yoo
typedef enum{
	VIDEO_DDI_ADC_MODE_EXTERNAL = 0,
	VIDEO_DDI_ADC_MODE_INTERNAL = 1,
	VIDEO_DDI_ADC_MODE_OTP      ,
	VIDEO_DDI_MAXOF_ADC_MODE,
}VIDEO_ADC_MODE_T;

typedef struct
{
	UINT32 CGMS20BitsVBI          : 20; // Header[6-bits] + {PayLoad + CRC}[14-bits]
										// CGMS-A 2-bits = CGMS20BitsVBI & (BIT6 | BIT7) >> 6
										// APS        2-bits = CGMS20BitsVBI & (BIT8 | BIT9) >> 8
	UINT32 CopyInfo576i576pVBI    : 2;  // Copy Right		 	= CopyInfo576i576p >> 0
										// Copy Protection		= CopyInfo576i576p >> 1
	UINT32 PseudosyncColorStripe  : 2;  // 00:no apps, 01:PSP on,cstripe off, 10:PSP on,2-line cstripe on, 11:PSP on,4-line cstripe on
	UINT32 ACPReserved            : 8;  // not used
}VIDEO_DDI_ANALOG_COPY_PROTECT;

///////////////////////////////////////////////////////////////////////////////

/**
 * group of HDMI info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
    UINT8 type;            /**< packet type */
    UINT8 version;         /**< packet version */
    UINT8 length;          /**< packet length */
    UINT8 dataBytes[VIDEO_HDMI_PACKET_DATA_LENGTH]; /**< packet data */
} VIDEO_HDMI_PACKET_T;

/**
 * HDMI or DVI mode.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_DDI_HDMI_DVI,		/**< DVI signal */
    VIDEO_DDI_HDMI_HDMI,	/**< HDMI signal */
} VIDEO_DDI_HDMI_MODE_T;

/**
 * HDMI packet status.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_PACKET_STATUS_NOT_RECEIVED = 0,		/**< HDMI packet status */
    VIDEO_HDMI_PACKET_STATUS_STOPPED,				/**< HDMI packet status */
    VIDEO_HDMI_PACKET_STATUS_UPDATED,				/**< HDMI packet status */
    VIDEO_HDMI_PACKET_STATUS_MAX					/**< HDMI packet status */
} VIDEO_HDMI_PACKET_STATUS_T;

/**
 * HDMI video format
 *
*/
typedef enum
{
	VIDEO_HDMI_VSI_VIDEO_FORMAT_NO_ADDITIONAL_FORMAT, //2D
	VIDEO_HDMI_VSI_VIDEO_FORMAT_EXTENDED_RESOLUTION_FORMAT,
	VIDEO_HDMI_VSI_VIDEO_FORMAT_3D_FORMAT,
}VIDEO_DDI_HDMI_VSI_VIDEO_FORMAT_T;

/**
 * HDMI Video Format Identification Code for extended resolution 0~ff
 *
*/
typedef enum
{
	VIDEO_HDMI_VSI_VIC_RESERVED,
	VIDEO_HDMI_VSI_VIC_4K2K_30HZ,
	VIDEO_HDMI_VSI_VIC_4K2K_25HZ,
	VIDEO_HDMI_VSI_VIC_4K2K_24HZ,
	VIDEO_HDMI_VSI_VIC_4K2K_24HZ_SMPTE,
}VIDEO_DDI_HDMI_VSI_VIC_T;

/**
 * HDMI Video Format Identification Code for 2D resolution 0~ff
 * See CEA-861-D section 6.4 for details
*/
typedef enum
{
	VIDEO_HDMI_AVI_VIC720P_60HZ = 4,
	VIDEO_HDMI_AVI_VIC720P_50HZ = 19,
	VIDEO_HDMI_AVI_VIC1080P_24HZ = 32,

/*
	AVI_VIC720P_24HZ = 60,
	AVI_VIC720P_25HZ,
	AVI_VIC720P_30HZ,
	AVI_VIC1080P_120HZ,
	AVI_VIC1080P_100HZ,
*/

}VIDEO_DDI_HDMI_AVI_VIC_T;


/**
 * HDMI Video VSI 3D Info
 * see HDMI Spec. v1.4a
*/
typedef enum
{
	VIDEO_HDMI_VSI_3D_STRUCTURE_FRAME_PACKING,
	VIDEO_HDMI_VSI_3D_STRUCTURE_FIELD_ALTERNATIVE,
	VIDEO_HDMI_VSI_3D_STRUCTURE_LINE_ALTERNATIVE,
	VIDEO_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_FULL,

	VIDEO_HDMI_VSI_3D_STRUCTURE_L_DEPTH,
	VIDEO_HDMI_VSI_3D_STRUCTURE_L_DEPTH_GRAPHICS,

	VIDEO_HDMI_VSI_3D_STRUCTURE_TOP_BOTTOM,			//6
	VIDEO_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_HALF = 0x08,
}VIDEO_DDI_HDMI_VSI_3D_STRUCTURE_T;

/**
 * HDMI Video VSI 3D Info
 * see HDMI Spec. v1.4a
*/
typedef enum
{
	VIDEO_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_0,
	VIDEO_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_1,
	VIDEO_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_2,
	VIDEO_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_3,

	VIDEO_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_0,
	VIDEO_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_1,
	VIDEO_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_2,
	VIDEO_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_3,

}VIDEO_DDI_HDMI_VSI_3D_EXT_DATA_T;

/**
 * HDMI VSI Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
	VIDEO_DDI_HDMI_VSI_VIDEO_FORMAT_T eVideoFormat;	/**< HDMI VSI info */
	VIDEO_DDI_HDMI_VSI_3D_STRUCTURE_T e3DStructure;	/**< HDMI VSI info */
	VIDEO_DDI_HDMI_VSI_3D_EXT_DATA_T e3DExtData;		/**< HDMI VSI info */

	VIDEO_DDI_HDMI_VSI_VIC_T eVSiVIC;					/**< HDMI VSI info */

    UINT8 IEERegId[VIDEO_HDMI_VENDOR_SPECIFIC_REGID_LEN];	/**< HDMI VSI info */
    UINT8 PayLoad[VIDEO_HDMI_VENDOR_SPECIFIC_PAYLOAD_LEN];	/**< HDMI VSI info */
    VIDEO_HDMI_PACKET_STATUS_T packetStatus;				/**< HDMI VSI info */
    VIDEO_HDMI_PACKET_T packet;								/**< HDMI VSI info */
} VIDEO_HDMI_VSI_T;

/**
 * HDMI GCP Info.
 *
 * @see HDMI Spec.
*/
typedef enum
{
  VIDEO_DDI_HDMI_GCP_NOT_INDICATED = 0,
  VIDEO_DDI_HDMI_GCP_24BITS_PER_PIXEL = 4,
  VIDEO_DDI_HDMI_GCP_30BITS_PER_PIXEL = 5,
  VIDEO_DDI_HDMI_GCP_36BITS_PER_PIXEL = 6,
  VIDEO_DDI_HDMI_GCP_48BITS_PER_PIXEL = 7,

} VIDEO_DDI_HDMI_GCP_COLOR_DEPTH;

/**
 * HDMI GCP Info.
 *
 * @see HDMI Spec.
*/
typedef enum
{
    VIDEO_DDI_HDMI_GCP_PP_PHASE4 = 0,
    VIDEO_DDI_HDMI_GCP_PP_PHASE1 = 1,
    VIDEO_DDI_HDMI_GCP_PP_PHASE2 = 2,
    VIDEO_DDI_HDMI_GCP_PP_PHASE3 = 3,
    VIDEO_DDI_HDMI_GCP_PP_RESERVED = 4,
} VIDEO_DDI_HDMI_GCP_PIXEL_PACKING_PHASE;

/**
 *
 * HDMI GCP Packet Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
    VIDEO_DDI_HDMI_GCP_COLOR_DEPTH e_CD;
    VIDEO_DDI_HDMI_GCP_PIXEL_PACKING_PHASE e_PP;
    BOOLEAN bSet_AVMUTE;
    BOOLEAN bDefault_Phase;

} VIDEO_HDMI_GCP_T;

/**
 *
 * HDMI SPD Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
    UINT8 VendorName[VIDEO_HDMI_SPD_IF_VENDOR_LEN+1];  /* add space for NULL */
    UINT8 ProductDescription[VIDEO_HDMI_SPD_IF_DESC_LEN+1];
    UINT8 SourceDeviceInfo;
    VIDEO_HDMI_PACKET_STATUS_T packetStatus;				/**< HDMI VSI info */
    VIDEO_HDMI_PACKET_T packet;								/**< HDMI VSI info */
} VIDEO_HDMI_SPD_T;

/**


/**
 * HDMI AVI CSC Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_CSC_RGB = 0,		/**< HDMI CSC info */
    VIDEO_HDMI_AVI_CSC_YCBCR422,	/**< HDMI CSC info */
    VIDEO_HDMI_AVI_CSC_YCBCR444,	/**< HDMI CSC info */
    VIDEO_HDMI_AVI_CSC_FUTURE		/**< HDMI CSC info */
} VIDEO_HDMI_AVI_CSC_T;

/**
 * HDMI AVI ACTIVE Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_ACTIVE_INFO_INVALID = 0,		/**< HDMI info */
    VIDEO_HDMI_AVI_ACTIVE_INFO_VALID			/**< HDMI info */
} VIDEO_HDMI_AVI_ACTIVE_INFO_T;

/**
 * HDMI AVI BAR Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_BAR_INFO_INVALID,			/**< HDMI info */
    VIDEO_HDMI_AVI_BAR_INFO_VERTICALVALID,		/**< HDMI info */
    VIDEO_HDMI_AVI_BAR_INFO_HORIZVALID,			/**< HDMI info */
    VIDEO_HDMI_AVI_BAR_INFO_VERTHORIZVALID		/**< HDMI info */
} VIDEO_HDMI_AVI_BAR_INFO_T;

/**
 * HDMI AVI SCAN Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_SCAN_INFO_NODATA,			/**< HDMI info */
    VIDEO_HDMI_AVI_SCAN_INFO_OVERSCANNED,		/**< HDMI info */
    VIDEO_HDMI_AVI_SCAN_INFO_UNDERSCANNED		/**< HDMI info */
} VIDEO_HDMI_AVI_SCAN_INFO_T;

/**
 * HDMI Colorimetry.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_COLORIMETRY_NODATA =  0,		/**< HDMI info */
    VIDEO_HDMI_AVI_COLORIMETRY_SMPTE170,		/**< HDMI info */
    VIDEO_HDMI_AVI_COLORIMETRY_ITU709,			/**< HDMI info */
    VIDEO_HDMI_AVI_COLORIMETRY_FUTURE,			/**< HDMI info */
    VIDEO_HDMI_AVI_COLORIMETRY_EXTENDED = VIDEO_HDMI_AVI_COLORIMETRY_FUTURE

} VIDEO_HDMI_AVI_COLORIMETRY_T;

/**
 * HDMI AVI Picture ARC.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_PICTURE_ARC_NODATA = 0,		/**< HDMI info */
    VIDEO_HDMI_AVI_PICTURE_ARC_4_3,				/**< HDMI info */
    VIDEO_HDMI_AVI_PICTURE_ARC_16_9,			/**< HDMI info */
    VIDEO_HDMI_AVI_PICTURE_ARC_FUTURE			/**< HDMI info */
} VIDEO_HDMI_AVI_PICTURE_ARC_T;

/**
 * HDMI AVI ACTIVE FORMAT ARC.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_PICTURE    =  8,	/**< HDMI info */
    VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_4_3CENTER  =  9,	/**< HDMI info */
    VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_16_9CENTER = 10,	/**< HDMI info */
    VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_14_9CENTER = 11,	/**< HDMI info */
    VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_OTHER      =  0	/**< HDMI info */
} VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_T;

/**
 * HDMI AVI Scaling info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_SCALING_NOSCALING = 0,		/**< HDMI info */
    VIDEO_HDMI_AVI_SCALING_HSCALING,			/**< HDMI info */
    VIDEO_HDMI_AVI_SCALING_VSCALING,			/**< HDMI info */
    VIDEO_HDMI_AVI_SCALING_HVSCALING			/**< HDMI info */
} VIDEO_HDMI_AVI_SCALING_T;

/**
 * HDMI AVI IT Content.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_IT_CONTENT_NODATA    =  0,	/**< HDMI info */
    VIDEO_HDMI_AVI_IT_CONTENT_ITCONTENT  =  1	/**< HDMI info */
} VIDEO_HDMI_AVI_IT_CONTENT_T;

/**
 * HDMI Extended Colorimetry.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_EXT_COLORIMETRY_XVYCC601 = 0,	/**< HDMI info */
    VIDEO_HDMI_AVI_EXT_COLORIMETRY_XVYCC709 ,		/**< HDMI info */
    VIDEO_HDMI_AVI_EXT_COLORIMETRY_XVRESERED		/**< HDMI info */
} VIDEO_HDMI_AVI_EXT_COLORIMETRY_T;

/**
 * HDMI AVI RGB Quantization range.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_RGB_QUANTIZATION_RANGE_DEFAULT    =  0,		/**< HDMI info */
    VIDEO_HDMI_AVI_RGB_QUANTIZATION_RANGE_LIMITEDRANGE  =  1,	/**< HDMI info */
    VIDEO_HDMI_AVI_RGB_QUANTIZATION_RANGE_FULLRANGE = 2,		/**< HDMI info */
    VIDEO_HDMI_AVI_RGB_QUANTIZATION_RANGE_RESERVED = 3			/**< HDMI info */
} VIDEO_HDMI_AVI_RGB_QUANTIZATION_RANGE_T;

//#if (PLATFORM_TYPE == LG_PLATFORM)
/**
 * HDMI AVI YCC Quantization range.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    VIDEO_HDMI_AVI_YCC_QUANTIZATION_RANGE_LIMITEDRANGE  =  0,	/**< HDMI info */
    VIDEO_HDMI_AVI_YCC_QUANTIZATION_RANGE_FULLRANGE = 1,		/**< HDMI info */
    VIDEO_HDMI_AVI_YCC_QUANTIZATION_RANGE_RESERVED = 2			/**< HDMI info */
} VIDEO_HDMI_AVI_YCC_QUANTIZATION_RANGE_T;

/**
 * HDMI AVI Content Type.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	VIDEO_HDMI_AVI_CONTENT_TYPE_GRAPHICS = 0,
	VIDEO_HDMI_AVI_CONTENT_TYPE_PHOTO,
	VIDEO_HDMI_AVI_CONTENT_TYPE_CINEMA,
	VIDEO_HDMI_AVI_CONTENT_TYPE_GAME,
	VIDEO_HDMI_AVI_CONTENT_TYPE_MAX
} VIDEO_HDMI_AVI_CONTENT_TYPE_T;

//#endif /* PLATFORM_TYPE == LG_PLATFORM */

/**
 * Video RGB-PC Index
 *
 */
typedef enum {
	PC_INDEX_1 = 0,
	NUM_PC_INPUTS
} VIDEO_PC_INDEX_T;


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
 * Video Scart Index
 *
 */
typedef enum {
	SCART_INDEX_1 = 0,
	SCART_INDEX_2,
	NUM_SCART_INPUTS
} VIDEO_SCART_INDEX_T;


/**
 * Video Composite Index
 *
 */
typedef enum {
	CVBS_INDEX_1 = 0,
	CVBS_INDEX_2,
	CVBS_INDEX_3,
	CVBS_INDEX_4,
	CVBS_INDEX_5,
	NUM_CVBS_INPUTS
} VIDEO_CVBS_INDEX_T;

/**
 * Video S-Video Index
 *
 */
typedef enum {
	SVIDEO_INDEX_1 = 0,
	SVIDEO_INDEX_2,
	SVIDEO_INDEX_3,
	NUM_SVIDEO_INPUTS
} VIDEO_SVIDEO_INDEX_T;

/**
 * Video HDMI Index
 *
 */
typedef enum {
	HDMI_INDEX_1 = 0,
	HDMI_INDEX_2,
	NUM_HDMI_INPUTS
} VIDEO_HDMI_INDEX_T;

/**
 * HDMI AVI RGB INFO.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
	VIDEO_DDI_HDMI_MODE_T eHDMIMode;				/**< HDMI info */

    VIDEO_HDMI_AVI_CSC_T ePixelEncoding;			/**< HDMI info */

    VIDEO_HDMI_AVI_ACTIVE_INFO_T eActiveInfo;		/**< HDMI info */	/* A0 */
    VIDEO_HDMI_AVI_BAR_INFO_T   eBarInfo;			/**< HDMI info */   /* B1B0 */
    VIDEO_HDMI_AVI_SCAN_INFO_T   eScanInfo;			/**< HDMI info */   /* S1S0 */

    VIDEO_HDMI_AVI_COLORIMETRY_T eColorimetry;		/**< HDMI info */	/* C1C0 */

    VIDEO_HDMI_AVI_PICTURE_ARC_T ePictureAspectRatio;	/**< HDMI info */	/* M1M0 */

    VIDEO_HDMI_AVI_ACTIVE_FORMAT_ARC_T eActiveFormatAspectRatio;/**< HDMI info */	/* R3R0 */

    VIDEO_HDMI_AVI_SCALING_T eScaling;						/**< HDMI info */	/* SC1SC0 */

    UINT8 VideoIdCode;								/**< HDMI info */	/* VICn */

    UINT8 PixelRepeat;								/**< HDMI info */

    VIDEO_HDMI_AVI_IT_CONTENT_T eITContent;			/**< HDMI info */	/*ITC */

    VIDEO_HDMI_AVI_EXT_COLORIMETRY_T eExtendedColorimetry;	/**< HDMI info */	/* EC2EC1EC0 */

    VIDEO_HDMI_AVI_RGB_QUANTIZATION_RANGE_T eRGBQuantizationRange;	/**< HDMI info */	/* Q1Q0 */

	VIDEO_HDMI_AVI_YCC_QUANTIZATION_RANGE_T eYCCQuantizationRange;	/**< HDMI info */	/* YQ1YQ0 */

	VIDEO_HDMI_AVI_CONTENT_TYPE_T eContentType;			/**< HDMI info */	/* Contents Type */

    /* bar info */
    UINT16 TopBarEndLineNumber;				/**< HDMI info */
    UINT16 BottomBarStartLineNumber;		/**< HDMI info */

    UINT16 LeftBarEndPixelNumber;			/**< HDMI info */
    UINT16 RightBarEndPixelNumber;			/**< HDMI info */

    VIDEO_HDMI_PACKET_STATUS_T packetStatus;	/**< HDMI info */
    VIDEO_HDMI_PACKET_T packet;					/**< HDMI info */
} VIDEO_HDMI_AVI_T;

/**
 * Tcon Panel power on sequence timing.
 *
 * @see Panel Spec
*/
typedef enum
{
    VIDEO_TCON_GST,		/**< refer to panel cas */
    VIDEO_TCON_MCLK,	/**< refer to panel cas */
    VIDEO_TCON_GCLK,	/**< refer to panel cas */
    VIDEO_TCON_EO,		/**< refer to panel cas */
    VIDEO_TCON_VCOM_DYN,/**< refer to panel cas */
    VIDEO_TCON_EPISIGNAL,/**< refer to panel cas */

    VIDEO_TCON_POW_SEQ_NUMS
} VIDEO_TCON_POWER_SEQ_T;

// 2012.10.31 yongsu.yoo move the position of this enum for Late-return of analog color system
/**
 * AVDEC : Analog Video Decoder Sync Locking Sensitivity.
 *
 * @see
 */
//typedef enum
//{
//    VIDEO_AVDEC_MODE_NTSC        = 0,
//    VIDEO_AVDEC_MODE_PAL            ,
//    VIDEO_AVDEC_MODE_PAL_N          ,
//    VIDEO_AVDEC_MODE_PAL_M          ,
//    VIDEO_AVDEC_MODE_SECAM          ,
//    VIDEO_AVDEC_MODE_NTSC_443       ,
//    VIDEO_AVDEC_MODE_PAL_60         ,
//    VIDEO_AVDEC_MODE_UNKNOWN_525    ,
//    VIDEO_AVDEC_MODE_UNKNOWN_625    ,
//    VIDEO_AVDEC_MODE_UNKNOWN        ,
//}VIDEO_AVDECODER_VIDEO_MODE_T;




/**
 * Analog TV color system.
 *
 * @see
*/
typedef enum
{
     COLOR_SYSTEM_NTSC_M    = BIT0 << VIDEO_AVDEC_MODE_NTSC     ,
     COLOR_SYSTEM_PAL_G     = BIT0 << VIDEO_AVDEC_MODE_PAL      ,
     COLOR_SYSTEM_PAL_NC    = BIT0 << VIDEO_AVDEC_MODE_PAL_N    ,
     COLOR_SYSTEM_PAL_M     = BIT0 << VIDEO_AVDEC_MODE_PAL_M    ,
     COLOR_SYSTEM_SECAM     = BIT0 << VIDEO_AVDEC_MODE_SECAM    ,
     COLOR_SYSTEM_NTSC_443  = BIT0 << VIDEO_AVDEC_MODE_NTSC_443 ,
     COLOR_SYSTEM_PAL_60    = BIT0 << VIDEO_AVDEC_MODE_PAL_60   ,
     COLOR_SYSTEM_PAL_N                                         ,  /* TBD should be remove */
     COLOR_SYSTEM_UNKNOWN                                       ,  /* TBD should be remove */

}VIDEO_DDI_COLOR_SYSTEM_T;




/**
 * HDMI DIAG INFO.
 * For HDMI Host Diagnostics.
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
	UINT8		hdmimode;			/**< ... */
	BOOLEAN		device_HDCP_status;	/**< ... */
	UINT16		horizontal_lines;	/**< ... */
	UINT16		vertical_lines;		/**< ... */
	UINT16		framerate;			/**< ... */
	BOOLEAN		prog_inter_type;	/**< ... */
	BOOLEAN		avmute;				/**< ... */
	UINT8		colorspace;			/**< ... */
} HDMI_DIAG_INFO_T;

/**
 * VIDEO_WIN_STATUS.
 *
 * @see
*/
typedef struct {
	UINT32	bActive :1, /**< if be active or not.  (0/1) */
			bBlanked	:1, /**< if be blanked or not. (0/1)*/
			bFreezed	:1, /**< if be freezed or not. (0/1)*/
			_rsvd00 :29;	/**< Reserved Bits. */
	SINT32	nUnBlank;		/**< Number of UnBlank Area. */
} VIDEO_WIN_STATUS_T;

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

}VIDEO_DDI_VDEC_TUNE_T;

/**
 * HDMI fine tune info
 *
 */
typedef struct
{
	UINT8 nHPD;
	UINT8 nAVMute;

}VIDEO_DDI_HDMI_TUNE_T;

/**
 * Sync Level fine tune info
 *
 */
typedef struct
{
	SINT8 nRGB;
	SINT8 nComp;
	SINT8 nHDMI;

}VIDEO_DDI_SYNCLEVEL_TUNE_T;

/**
 * Video fine tune info
 *
 */
typedef struct
{
	VIDEO_DDI_VDEC_TUNE_T		stVDECTune;
	VIDEO_DDI_HDMI_TUNE_T		stHDMITune;
	VIDEO_DDI_SYNCLEVEL_TUNE_T	stSyncLevelTune;
}VIDEO_DDI_TUNE_INFO_T;


typedef enum
{
    SC_OUT_DDI_SINGLE_2D  = 0 ,///< single nomal
    SC_OUT_DDI_SINGLE_TB      ,///< top and bottom
    SC_OUT_DDI_SINGLE_SS      ,///< side by side
    SC_OUT_DDI_SINGLE_FS      ,///< Frame Sequential(Interleaving)

    SC_OUT_DDI_DUAL_FHD       ,///< dual screen
    SC_OUT_DDI_DUAL_TB        ,///< dual top and bottom for H13 internal type
    SC_OUT_DDI_DUAL_SS        ,///< dual side by side for H13 internal type
}SC_OUTPUT_DDI_TYPE_T;

typedef struct {
	TRIDTV_3DMODE_FLAG_T	mode;			///< select formatter operation mode. it controls 3D mode ON/OFF.
	TRIDTV_INPUT_TYPE_T		in_fmt;			///< input image format(top/bottom, sidebyside, checkboard, frame, field seq, dual 2d)
	SC_OUTPUT_DDI_TYPE_T    out_fmt;		///< output image format(top/bottom, sidebyside, checkboard, frame, field seq)
	TRIDTV_INPUT_LR_SEQ_T   lr;		///< output left/right first order
}VIDEO_DDI_3D_SCALER_CTRL_T;


typedef struct {
	TRIDTV_3DMODE_FLAG_T	mode;			///< select formatter operation mode. it controls 3D mode ON/OFF.
	TRIDTV_INPUT_TYPE_T		in_fmt;			///< input image format(top/bottom, sidebyside, checkboard, frame, field seq, dual 2d)
	SC_OUTPUT_DDI_TYPE_T    out_fmt;		///< output image format(top/bottom, sidebyside, checkboard, frame, field seq)
	TRIDTV_INPUT_LR_SEQ_T   lr;		///< output left/right first order
}VIDEO_DDI_3D_FRC_CTRL_T;





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
}VIDEO_DDI_CAPTURE_INFO_T;

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



typedef enum
{
	VIDEO_INNER_PATTERN_IRE_0=0,
	VIDEO_INNER_PATTERN_IRE_5,
	VIDEO_INNER_PATTERN_IRE_10,
	VIDEO_INNER_PATTERN_IRE_15,
	VIDEO_INNER_PATTERN_IRE_20,
	VIDEO_INNER_PATTERN_IRE_25,
	VIDEO_INNER_PATTERN_IRE_30,
	VIDEO_INNER_PATTERN_IRE_35,
	VIDEO_INNER_PATTERN_IRE_40,
	VIDEO_INNER_PATTERN_IRE_45,
	VIDEO_INNER_PATTERN_IRE_50,
	VIDEO_INNER_PATTERN_IRE_55,
	VIDEO_INNER_PATTERN_IRE_60,
	VIDEO_INNER_PATTERN_IRE_65,
	VIDEO_INNER_PATTERN_IRE_70,
	VIDEO_INNER_PATTERN_IRE_75,
	VIDEO_INNER_PATTERN_IRE_80,
	VIDEO_INNER_PATTERN_IRE_85,
	VIDEO_INNER_PATTERN_IRE_90,
	VIDEO_INNER_PATTERN_IRE_95,
	VIDEO_INNER_PATTERN_IRE_100
}VIDEO_INNER_PATTERN_IRE_T;


/*-----------------------------------------------------------------------------
	(Extern Variables & Function Prototype Declarations)
------------------------------------------------------------------------------*/
//////////////////////////////////////
// start of video common ddi functions
DTV_STATUS_T	DDI_VIDEO_Initialize(void);
DTV_STATUS_T	DDI_VIDEO_Uninitialize(void);
DTV_STATUS_T	DDI_VIDEO_SetBackgroundColor(UINT8 red, UINT8 green, UINT8 blue);
BOOLEAN 		DDI_VIDEO_IsAVDECSyncExist(void);
DTV_STATUS_T	DDI_VIDEO_SetScartOverlay(UINT8 scartId, BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_GetOutputFrameRate(UINT8 *pFrameRate);
DTV_STATUS_T    DDI_VIDEO_SetPCResolution(SINT32 remappedResolution);
DTV_STATUS_T	DDI_VIDEO_SetWinBlank(VIDEO_WID_T wId, BOOLEAN bOnOff, VIDEO_DDI_WIN_COLOR_T color);
DTV_STATUS_T	DDI_VIDEO_SetVCRWinBlank(VIDEO_WID_T wId, BOOLEAN bOnOff);
#ifdef GRAY_MUTE_TEST_FOR_LIGHT_LEAKAGE
DTV_STATUS_T DDI_VIDEO_GetMuteAndBgColorForLightLeakage(BOOLEAN mode, UINT8* red, UINT8* green, UINT8* blue);
#endif
DTV_STATUS_T	DDI_VIDEO_SetMultiWindowMode(VIDEO_MULTI_WIN_MODE_T multiWinMode);
DTV_STATUS_T	DDI_VIDEO_GetInputScalerRegion(VIDEO_WID_T wId, VIDEO_DDI_RECT_T *pInRegion);
DTV_STATUS_T	DDI_VIDEO_SetInputScalerRegion(VIDEO_WID_T wId, VIDEO_DDI_RECT_T inRegion);
DTV_STATUS_T	DDI_VIDEO_GetOutputScalerRegion(VIDEO_WID_T wId, VIDEO_DDI_RECT_T *pOutRegion);
DTV_STATUS_T	DDI_VIDEO_SetOutputScalerRegion(VIDEO_WID_T wId, VIDEO_DDI_RECT_T outRegion);
DTV_STATUS_T	DDI_VIDEO_SetWinFreeze(VIDEO_WID_T wId, BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_SetInput(VIDEO_WID_T wId, VIDEO_SRC_INFO_T inSrc);
DTV_STATUS_T	DDI_VIDEO_GetTimingInfo(VIDEO_WID_T wId, VIDEO_TIMING_INFO_T *pInfo);
DTV_STATUS_T	DDI_VIDEO_UpdateTimingInfo(VIDEO_WID_T wId, VIDEO_TIMING_INFO_T *pInfo);
DTV_STATUS_T	DDI_VIDEO_SetDisplayOutput(BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_SetDisplayFormat(const PANEL_ATTRIBUTE_T * pPanelAttr, UINT8 frameRate);

#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T 	DDI_VIDEO_GetFrameRateForPWM(UINT8 *pFrameRate);
#endif

//DTV_STATUS_T	DDI_VIDEO_SetWBTestPattern(BOOLEAN bOnOff, BOOLEAN bIs216);
DTV_STATUS_T	DDI_VIDEO_SetIREInnerPattern(UINT8 patternType, VIDEO_INNER_PATTERN_IRE_T patternIre);
DTV_STATUS_T	DDI_VIDEO_SetSpreadSpectrum(BOOLEAN bEnable, UINT16 u16Percent, UINT16 u16Period);
DTV_STATUS_T	DDI_VIDEO_SetAnalogColorSystem(VIDEO_DDI_COLOR_SYSTEM_T colorSystem);
DTV_STATUS_T	DDI_VIDEO_GetAnalogColorSystem(VIDEO_AVDECODER_VIDEO_MODE_T *pColorSystem);
DTV_STATUS_T	DDI_VIDEO_GrabPixels(VIDEO_DDI_PIXEL_GRAB_REGION_T * pstGrabRegion, VIDEO_DDI_PIXEL_GRAB_COLOR_T *pstGrabColor); // TODO: ��� �缳�� �� ��.
DTV_STATUS_T	DDI_VIDEO_SetADCGainOffset(UINT16 *pADCValue);
BOOLEAN			DDI_VIDEO_Is3DFramePacking(void);

DTV_STATUS_T	DDI_VIDEO_AdjustPCAuto(VIDEO_DDI_ADJ_PC_T *pAdjResult);
DTV_STATUS_T	DDI_VIDEO_AdjustPC(VIDEO_DDI_ADJ_PC_MODE_T mode, SINT16 value);
DTV_STATUS_T	DDI_VIDEO_GetADCValue(ADC_CALIB_DATA_T *pCalibData);
DTV_STATUS_T    DDI_VIDEO_SetADCValue(VIDEO_DDI_ADC_SRC_T srcType, ADC_CALIB_DATA_T adcValue );
DTV_STATUS_T    DDI_VIDEO_AdjustADCCalibration(VIDEO_ADC_MODE_T adcType);

// 2012.06.20 yongsu.yoo
#if 0
DTV_STATUS_T DDI_VIDEO_AdjustALLTypeADCCalibration(VIDEO_ADC_MODE_T adcType);
#endif

// 2012.07.25 yongsu.yoo for Analog Contents Copy Protect
DTV_STATUS_T DDI_VIDEO_GetAnalogCopyProtect(VIDEO_WID_T wId, VIDEO_DDI_ANALOG_COPY_PROTECT *psAnalogCopyPro);

// 2012.08.25 yongsu.yoo for prohibiting ATV mute
DTV_STATUS_T DDI_VIDEO_MTK_FRCMuteOnOff(BOOLEAN bOnOff);

void            DDI_VIDEO_SetADCMode(UINT32 adcMode );
UINT32          DDI_VIDEO_GetADCMode(void );

DTV_STATUS_T 	DDI_VIDEO_OTPADCOnOff(BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_GetHDMIAspectRatio(UINT16 *pAr);
DTV_STATUS_T	DDI_VIDEO_GetHDMIMode(UINT8 port, UINT8 *pMode);
DTV_STATUS_T	DDI_VIDEO_GetHDMIVSI(VIDEO_HDMI_VSI_T *pPacket);
DTV_STATUS_T    DDI_VIDEO_GetHDMISPD(void *pPacket);
DTV_STATUS_T	DDI_VIDEO_GetHDMIAVI(VIDEO_HDMI_AVI_T *pPacket);
DTV_STATUS_T	DDI_VIDEO_GetFBStatus(VIDEO_DDI_SCART_FB_STATUS_T *pStatus);
DTV_STATUS_T	DDI_VIDEO_GetScartInputMode(UINT8 scartId, VIDEO_DDI_SCART_AR_T *pScartAr);
DTV_STATUS_T	DDI_VIDEO_SetFrameRate(BOOLEAN isForceFreeRun, UINT8 frameRates);
DTV_STATUS_T	DDI_VIDEO_SetVideoMirror(BOOLEAN bIsH, BOOLEAN bIsV);
DTV_STATUS_T	DDI_VIDEO_SetCompSyncSliceLevel(UINT8 nSliceLevel);
DTV_STATUS_T	DDI_VIDEO_Set3DForScaler(TRIDTV_SCALER_CTRL_T *p3DCfgInfo);

DTV_STATUS_T	DDI_VIDEO_FreezeVideoFrameBuffer(BOOLEAN bFreezeOn);
DTV_STATUS_T	DDI_VIDEO_ReadVideoFrameBuffer(VIDEO_WID_T wId, VIDEO_DDI_RECT_T *pIn, VIDEO_DDI_PIXEL_STANDARD_COLOR_T *pRead,
													VIDEO_DDI_COLOR_STANDARD_T *pcolor_standard, VIDEO_DDI_PIXEL_COLOR_FORMAT_T * pPixelColorFormat);
DTV_STATUS_T	DDI_VIDEO_WriteVideoFrameBuffer(VIDEO_WID_T wId, VIDEO_DDI_RECT_T *pIn, VIDEO_DDI_PIXEL_STANDARD_COLOR_T *pWrite);
DTV_STATUS_T	DDI_VIDEO_CaptureCurrentDisplay(VIDEO_DDI_CAPTURE_INFO_T* pCaptureInfo);
DTV_STATUS_T	DDI_VIDEO_FinalizeDisplayCaputre(const UINT8* pCaptureBuffer);
DTV_STATUS_T	DDI_VIDEO_CaptureVideoFrame(VIDEO_DDI_CAPTURE_PLACE_T place, VIDEO_DDI_CAPTURE_INFO_T* pCaptureInfo);

#ifdef INCLUDE_WIRELESS_READY
DTV_STATUS_T	DDI_VIDEO_SetWirelessInputConnectStatus(BOOLEAN bSet);
BOOLEAN			DDI_VIDEO_IsWirelessInputConnected(void);
#endif

//SINT32			DDI_VIDEO_CheckVideoModeResolution(VIDEO_WID_T wId, SINT32 resolution, UINT16 vFreq, UINT8 scanType, UINT8* mode);
//BOOLEAN			DDI_VIDEO_IsPCLabel4HDMI(void);
#ifdef	HDMI_444_MODE
DTV_STATUS_T	DDI_VIDEO_SetHDMI444Mode(BOOLEAN bOnOff);
#else
DTV_STATUS_T	DDI_VIDEO_SetPCLabel4HDMI(BOOLEAN bIsPCLabel);
#endif
DTV_STATUS_T	DDI_VIDEO_SetVdecTuning(VIDEO_DDI_COLOR_SYSTEM_T colorSystem);
DTV_STATUS_T	DDI_VIDEO_GetVideoDelayedTime(VIDEO_WID_T wId, UINT16* pDelayedTime);
DTV_STATUS_T	DDI_VIDEO_Get3DAutoDetectmode(VIDEO_WID_T wId, VIDEO_DDI_3D_FORMAT_TYPE_T* pVideo3Dtype);
DTV_STATUS_T    DDI_VIDEO_Set3DpatternMode(VIDEO_WID_T wId,VIDEO_DDI_3D_PATTERN_TYPE_T Video3Dtype);
DTV_STATUS_T 	DDI_VIDEO_Get3DDualMode(VIDEO_WID_T wId, VIDEO_DDI_DUAL_TYPE_T *pDualMode);


DTV_STATUS_T	DDI_VIDEO_Get3DAPLLevel(UINT8 *p3DAPLLeft, UINT8 *p3DAPLRight);
DTV_STATUS_T	DDI_VIDEO_Set3DSyncOut(BOOLEAN bOnOff);
DTV_STATUS_T 	DDI_VIDEO_SetLVDS2ndChannelOnOff(BOOLEAN bOnOff);
DTV_STATUS_T 	DDI_VIDEO_Set3DSyncDelay(UINT16 u2Delay);

DTV_STATUS_T	DDI_VIDEO_SetODC_LUTable(const UINT8 *pODTbl);
DTV_STATUS_T	DDI_VIDEO_GetODC_LUTable(UINT8 *pODTbl);
DTV_STATUS_T	DDI_VIDEO_EnableODC_LUTable(BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_SetPCID_LUTable(const UINT8 *pPCIDTbl);
DTV_STATUS_T	DDI_VIDEO_GetPCID_LUTable(UINT8 *pPCIDTbl);
DTV_STATUS_T	DDI_VIDEO_EnablePCID_LUTable(BOOLEAN bOnOff);

DTV_STATUS_T	DDI_VIDEO_GetMultimediaPAR(UINT8 *width, UINT8 *height);

//for Analog Contents Copy Protection
DTV_STATUS_T DDI_VIDEO_SOC_GetAnalogCopyProtect(VIDEO_WID_T wId, VIDEO_DDI_ANALOG_COPY_PROTECT *psAnalogCopyPro);

#ifdef INCLUDE_PENTOUCH_BUILTIN
DTV_STATUS_T	DDI_VIDEO_SetSupportPentouch(BOOLEAN bOnOff);
#endif

//end of video common ddi functions
///////////////////////////////////
DTV_STATUS_T DDI_VIDEO_Debug(void);


/////////////////////////////////////////////
// start of video soc dependent ddi functions
#if (PLATFORM_TYPE == BCM_PLATFORM)
DTV_STATUS_T	DDI_VIDEO_BCM_SetAdjustADCMode(UINT8 bEnable);
DTV_STATUS_T	DDI_VIDEO_BCM_SetForcePending(void);
DTV_STATUS_T	DDI_VIDEO_BCM_SetVdecClampLevel(UINT32 nClampLevel);
DTV_STATUS_T	DDI_VIDEO_BCM_SetVdecHPLL(UINT8 hPLLNTSC, UINT8 hPLLPAL, VIDEO_DDI_COLOR_SYSTEM_T colorSystem);
DTV_STATUS_T	DDI_VIDEO_BCM_SetVdecSyncSensitivity(UINT32 shortLockFound, UINT32 shortLockLost);
DTV_STATUS_T	DDI_VIDEO_BCM_SetPCManualUpdateMode(BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_BCM_SetFlagOfPCManualUpdateMode(BOOLEAN bOnOff);
BOOLEAN			DDI_VIDEO_BCM_IsSupportedOTPforADCCalibration(void);
DTV_STATUS_T	DDI_VIDEO_BCM_SetAdjustedData(VIDEO_DDI_ADC_SRC_T adj_src);
DTV_STATUS_T	DDI_VIDEO_BCM_Set3DFramePacking(BOOLEAN on_off);
DTV_STATUS_T	DDI_VIDEO_BCM_SetAGCMode(BOOLEAN bEnable);
DTV_STATUS_T	DDI_VIDEO_BCM_SetLockUpdateOSDFlag(BOOLEAN bSet);
BOOLEAN			DDI_VIDEO_BCM_GetLockUpdateOSDFlag(void);
UINT16			DDI_VIDEO_BCM_GetOriginalFrameRateForHDMI(void); // TODO: �缳���ؼ� ���� �� �Լ�.
DTV_STATUS_T	DDI_VIDEO_BCM_SetAutoADCCalibrationMode(BOOLEAN bAutoMode);
DTV_STATUS_T	DDI_VIDEO_BCM_CaptureVideoBuffer(void); 
DTV_STATUS_T	DDI_VIDEO_BCM_ReleaseVideoBuffer(void); 
DTV_STATUS_T	DDI_VIDEO_BCM_ReadVideoBuffer(UINT8 winId, UINT8 YCbCr,
				UINT32 *pX, UINT32 *pY, UINT8 blockX, UINT8 blockY, float *pRetMean); 
#endif
#if (PLATFORM_TYPE == MSTAR_PLATFORM)
DTV_STATUS_T	DDI_VIDEO_MSTAR_SetColorWash( BOOLEAN bIsColorWash );
DTV_STATUS_T	DDI_VIDEO_MSTAR_Set3DCombSpeedUp(BOOLEAN bEnable);
DTV_STATUS_T	DDI_VIDEO_MSTAR_ResetAGC(void);
DTV_STATUS_T	DDI_VIDEO_MSTAR_SetSwingThreshold(UINT8 u8SwingThreshold);
DTV_STATUS_T	DDI_VIDEO_MSTAR_SetTCONPowerSequence(VIDEO_TCON_POWER_SEQ_T eTconPowSeq, BOOLEAN bOnOff);
#endif
#if (PLATFORM_TYPE == LG_PLATFORM)
DTV_STATUS_T	DDI_VIDEO_LGE_SetMultiWindowEnable(BOOLEAN bOnOff);
DTV_STATUS_T 	DDI_VIDEO_LGE_SetVCS(UINT32 index, UINT32 data);/* [L8] 2010. 10. 25 by dongho7.park */
DTV_STATUS_T	DDI_VIDEO_LGE_SetSeparateOSD(BOOLEAN bOnOff);
BOOLEAN			DDI_VIDEO_LGE_IsSeparateOSDMode(void);
DTV_STATUS_T	DDI_VIDEO_LGE_SetDepthDisparityData(UINT16 *sbs_disparity_data, UINT16 *tnb_disparity_data);
DTV_STATUS_T	DDI_VIDEO_LGE_SetAdaptiveStream(BOOLEAN bOnOff);
// SCKIM
#if 0 //def INCLUDE_3D_DUALSTREAM
DTV_STATUS_T	DDI_VIDEO_LGE_Set3DInOutCtrl(TRIDTV_INPUT_CONFIG_INFO_T *pVideo3DCfg);
#endif
#ifdef INCLUDE_UD
DTV_STATUS_T 	DDI_VIDEO_LGE_SetUDMode(VIDEO_UD_MODE_T videoUDMode);
VIDEO_UD_MODE_T 		DDI_VIDEO_LGE_GetUDMode(void);
DTV_STATUS_T 	DDI_VIDEO_LGE_SetUDDisFmt(VIDEO_DDI_DIS_FMT_T de_dis_fmt);
DTV_STATUS_T DDI_VIDEO_LGE_SetUDOutput(VIDEO_DDI_DIS_FMT_T disFmt);
DTV_STATUS_T DDI_VIDEO_LGE_SetUDInput(VIDEO_UD_MODE_T videoUDMode);

#endif
DTV_STATUS_T	DDI_VIDEO_LGE_SetOIF_SetDarkScreen(BOOLEAN bOnOff);
DTV_STATUS_T	DDI_VIDEO_LGE_Set_SCART_RGB_Bypass(VIDEO_WID_T wId, BOOLEAN bOnOff);
#endif
//end of video soc dependent ddi functions
//////////////////////////////////////////



//////////////////////////////////////
// start of vbi common ddi functions
DTV_STATUS_T	DDI_VIDEO_VBI_Enable(void);
DTV_STATUS_T	DDI_VIDEO_VBI_Disable(void);
DTV_STATUS_T    DDI_VIDEO_VBI_SetSlicing (VIDEO_WID_T wId, VIDEO_DDI_VBI_TYPE_T type, void *pfnVBICB);
DTV_STATUS_T    DDI_VIDEO_VBI_SetInsert(VIDEO_DDI_VBI_TYPE_T type,  UINT8 OnOff);
DTV_STATUS_T	DDI_VIDEO_VBI_InsertData (VIDEO_DDI_VBI_TYPE_T type, UINT32 nrLine, UINT32 *pLines, UINT32 size, UINT8 *pData);
DTV_STATUS_T	DDI_VIDEO_VBI_SetMessageIDOfACC(OSA_MSGQ_T qid4ACCTx, UINT32 oddMsgID, UINT32 evenMsgID, UINT32 mainSrcID, UINT32 subSrcID);
DTV_STATUS_T	DDI_VIDEO_VBI_GetWSSData(UINT16 * pWss);
DTV_STATUS_T	DDI_VIDEO_VBI_FlushData(VIDEO_DDI_VBI_TYPE_T type);
//end of video common ddi functions
///////////////////////////////////


/////////////////////////////////////////////
// start of vbi soc dependent ddi functions
#if (PLATFORM_TYPE == BCM_PLATFORM)
#ifdef INCLUDE_ANALOGTTX
DTV_STATUS_T    DDI_VIDEO_BCM_VBI_SetTtx(UINT8 OnOff);
#endif
#endif
#if (PLATFORM_TYPE == MSTAR_PLATFORM)
BOOLEAN			DDI_VIDEO_VBI_MSTAR_IsVPSDataReady(void);
#endif
//end of vbi soc dependent ddi functions
////////////////////////////////////////


#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_VIDEO_MTK_GetHdmiEdid(UINT8 uPortNum, UINT8 pData[256]);
DTV_STATUS_T DDI_VIDEO_MTK_GetVgaEdid(UINT8 pData[256]);
DTV_STATUS_T DDI_VIDEO_MTK_SetAutoTuning(BOOLEAN bOnOff);
DTV_STATUS_T DDI_VIDEO_GetSiganlStrenthLevel(UINT8 u1Type, UINT8 *pu1Level);
DTV_STATUS_T DDI_VIDEO_SetATVChannelChange(VIDEO_WID_T wId);	// 2012.11.28 yongsu.yoo ATV-Channel-Change-Garbage
#endif
DTV_STATUS_T DDI_VIDEO_SetScartOnOff(BOOLEAN bOnOff);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _VIDEO_DDI_H_ */
