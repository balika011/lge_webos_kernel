/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2008 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file dil_vfe_hdmi.h
 *
 * This source file defines the DDI functions related to HDMI Switch Device Driver control
 *
 *  @author	Kim Jihoon(jihoons.kim@lge.com)
 *  @version	0.1
 *  @date		2013.03.06
 *  @note		Additional information.
 *  @see		dil_vfe_hdmi.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _HAL_VFE_HDMI_H_
#define	_HAL_VFE_HDMI_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
//#include "common.h"
#include "hal_common.h"
#include "hal_vsc.h"

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

#define HAL_VFE_HDMI_PACKET_DATA_LENGTH 28
#define HAL_VFE_HDMI_INFOFRAME_PACKET_LEN 28
#define HAL_VFE_HDMI_VENDOR_SPECIFIC_REGID_LEN 3
#define HAL_VFE_HDMI_VENDOR_SPECIFIC_PAYLOAD_LEN \
    (HAL_VFE_HDMI_INFOFRAME_PACKET_LEN - HAL_VFE_HDMI_VENDOR_SPECIFIC_REGID_LEN)

#define HAL_VFE_HDMI_SPD_IF_VENDOR_LEN          8
#define HAL_VFE_HDMI_SPD_IF_DESC_LEN            16


/******************************************************************************
   로컬 상수 정의 (Local Constant Definitions)
******************************************************************************/

/******************************************************************************
    로컬 형 정의 (Local Type Definitions)
******************************************************************************/
#if 0
/**
 * for other hdmi switch.
*/
typedef enum
{
	HAL_VFE_HDMI_PORT1,
	HAL_VFE_HDMI_PORT2,
	HAL_VFE_HDMI_PORT3,
	HAL_VFE_HDMI_PORT4,
	HAL_VFE_RGB_PORT, //ieeum_rgb
}HAL_VFE_HDMI_PORT_T;


/** HDMI aspect ratio
*/
typedef enum {
    HAL_VFE_HDMI_AFD_NODATA = 0,
    HAL_VFE_HDMI_AFD_4_3,
    HAL_VFE_HDMI_AFD_16_9,
    HAL_VFE_HDMI_AFD_RESERVED,
    HAL_VFE_HDMI_AFD_MAX
}HAL_VFE_HDMI_AFD_T;
#endif

/**
 * HDMI or DVI mode.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_DVI,		/**< DVI signal */
    HAL_VFE_HDMI_HDMI,	/**< HDMI signal */
} HAL_VFE_HDMI_MODE_T;

/**
 * HDMI packet status.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_PACKET_STATUS_NOT_RECEIVED = 0,		/**< HDMI packet status */
    HAL_VFE_HDMI_PACKET_STATUS_STOPPED,				/**< HDMI packet status */
    HAL_VFE_HDMI_PACKET_STATUS_UPDATED,				/**< HDMI packet status */
    HAL_VFE_HDMI_PACKET_STATUS_MAX					/**< HDMI packet status */
} HAL_VFE_HDMI_PACKET_STATUS_T;



/**
 * HDMI AVI CSC Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_AVI_CSC_RGB = 0,		/**< HDMI CSC info */
    HAL_VFE_HDMI_AVI_CSC_YCBCR422,	/**< HDMI CSC info */
    HAL_VFE_HDMI_AVI_CSC_YCBCR444,	/**< HDMI CSC info */
    HAL_VFE_HDMI_AVI_CSC_YCBCR420		/**< HDMI CSC info */
} HAL_VFE_HDMI_AVI_CSC_T;


/**
 * HDMI AVI ACTIVE Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_AVI_ACTIVE_INFO_INVALID = 0,		/**< HDMI info */
    HAL_VFE_HDMI_AVI_ACTIVE_INFO_VALID			/**< HDMI info */
} HAL_VFE_HDMI_AVI_ACTIVE_INFO_T;


/**
 * HDMI AVI BAR Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_AVI_BAR_INFO_INVALID,			/**< HDMI info */
    HAL_VFE_HDMI_AVI_BAR_INFO_VERTICALVALID,		/**< HDMI info */
    HAL_VFE_HDMI_AVI_BAR_INFO_HORIZVALID,			/**< HDMI info */
    HAL_VFE_HDMI_AVI_BAR_INFO_VERTHORIZVALID		/**< HDMI info */
} HAL_VFE_HDMI_AVI_BAR_INFO_T;


/**
 * HDMI AVI SCAN Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_AVI_SCAN_INFO_NODATA,			/**< HDMI info */
    HAL_VFE_HDMI_AVI_SCAN_INFO_OVERSCANNED,		/**< HDMI info */
    HAL_VFE_HDMI_AVI_SCAN_INFO_UNDERSCANNED,		/**< HDMI info */
    HAL_VFE_HDMI_AVI_SCAN_INFO_FUTURE		= 3
} HAL_VFE_HDMI_AVI_SCAN_INFO_T;


/**
 * HDMI Colorimetry.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
    HAL_VFE_HDMI_AVI_COLORIMETRY_NODATA =  0,		/**< HDMI info */
    HAL_VFE_HDMI_AVI_COLORIMETRY_SMPTE170,		/**< HDMI info */
    HAL_VFE_HDMI_AVI_COLORIMETRY_ITU709,			/**< HDMI info */
    HAL_VFE_HDMI_AVI_COLORIMETRY_FUTURE,			/**< HDMI info */
    HAL_VFE_HDMI_AVI_COLORIMETRY_EXTENDED = HAL_VFE_HDMI_AVI_COLORIMETRY_FUTURE

} HAL_VFE_HDMI_AVI_COLORIMETRY_T;


/**
 * HDMI AVI Picture ARC.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_PICTURE_ARC_NODATA = 0,		/**< HDMI info */
	HAL_VFE_HDMI_AVI_PICTURE_ARC_4_3,				/**< HDMI info */
	HAL_VFE_HDMI_AVI_PICTURE_ARC_16_9,			/**< HDMI info */
	HAL_VFE_HDMI_AVI_PICTURE_ARC_FUTURE			/**< HDMI info */
} HAL_VFE_HDMI_AVI_PICTURE_ARC_T;


/**
 * HDMI AVI ACTIVE FORMAT ARC.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_PICTURE    =  8,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_4_3CENTER  =  9,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_16_9CENTER = 10,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_14_9CENTER = 11,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_OTHER      =  0	/**< HDMI info */
} HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_T;


/**
 * HDMI AVI Scaling info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_SCALING_NOSCALING = 0,		/**< HDMI info */
	HAL_VFE_HDMI_AVI_SCALING_HSCALING,			/**< HDMI info */
	HAL_VFE_HDMI_AVI_SCALING_VSCALING,			/**< HDMI info */
	HAL_VFE_HDMI_AVI_SCALING_HVSCALING			/**< HDMI info */
} HAL_VFE_HDMI_AVI_SCALING_T;


/**
 * HDMI AVI IT Content.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_IT_CONTENT_NODATA    =  0,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_IT_CONTENT_ITCONTENT  =  1	/**< HDMI info */
} HAL_VFE_HDMI_AVI_IT_CONTENT_T;


/**
 * HDMI Extended Colorimetry.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_EXT_COLORIMETRY_XVYCC601 = 0,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_EXT_COLORIMETRY_XVYCC709 ,		/**< HDMI info */
	HAL_VFE_HDMI_AVI_EXT_COLORIMETRY_XVRESERED		/**< HDMI info */
} HAL_VFE_HDMI_AVI_EXT_COLORIMETRY_T;


/**
 * HDMI AVI RGB Quantization range.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_RGB_QUANTIZATION_RANGE_DEFAULT    =  0,		/**< HDMI info */
	HAL_VFE_HDMI_AVI_RGB_QUANTIZATION_RANGE_LIMITEDRANGE  =  1,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_RGB_QUANTIZATION_RANGE_FULLRANGE = 2,		/**< HDMI info */
	HAL_VFE_HDMI_AVI_RGB_QUANTIZATION_RANGE_RESERVED = 3			/**< HDMI info */
} HAL_VFE_HDMI_AVI_RGB_QUANTIZATION_RANGE_T;


/**
 * HDMI AVI YCC Quantization range.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_YCC_QUANTIZATION_RANGE_LIMITEDRANGE  =  0,	/**< HDMI info */
	HAL_VFE_HDMI_AVI_YCC_QUANTIZATION_RANGE_FULLRANGE = 1,		/**< HDMI info */
	HAL_VFE_HDMI_AVI_YCC_QUANTIZATION_RANGE_RESERVED = 2			/**< HDMI info */
} HAL_VFE_HDMI_AVI_YCC_QUANTIZATION_RANGE_T;


/**
 * HDMI AVI Content Type.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef enum
{
	HAL_VFE_HDMI_AVI_CONTENT_TYPE_GRAPHICS = 0,
	HAL_VFE_HDMI_AVI_CONTENT_TYPE_PHOTO,
	HAL_VFE_HDMI_AVI_CONTENT_TYPE_CINEMA,
	HAL_VFE_HDMI_AVI_CONTENT_TYPE_GAME,
	HAL_VFE_HDMI_AVI_CONTENT_TYPE_MAX
} HAL_VFE_HDMI_AVI_CONTENT_TYPE_T;


/**
 * group of HDMI info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
	int		prt_num;				/// < port selection information(-1: use previous selected port)
	UINT8 type;            /**< packet type */
	UINT8 version;         /**< packet version */
	UINT8 length;          /**< packet length */
	UINT8 dataBytes[HAL_VFE_HDMI_PACKET_DATA_LENGTH]; /**< packet data */
} HAL_VFE_HDMI_IN_PACKET_T;


//=============================================================
//							 HDMI AVI Info.
//=============================================================

/**
 * HDMI AVI RGB INFO.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
	HAL_VFE_HDMI_MODE_T eHDMIMode;				/**< HDMI info */

	HAL_VFE_HDMI_AVI_CSC_T ePixelEncoding;			/**< HDMI info */

	HAL_VFE_HDMI_AVI_ACTIVE_INFO_T eActiveInfo;		/**< HDMI info */	/* A0 */
	HAL_VFE_HDMI_AVI_BAR_INFO_T   eBarInfo;			/**< HDMI info */   /* B1B0 */
	HAL_VFE_HDMI_AVI_SCAN_INFO_T   eScanInfo;			/**< HDMI info */   /* S1S0 */

	HAL_VFE_HDMI_AVI_COLORIMETRY_T eColorimetry;		/**< HDMI info */	/* C1C0 */

	HAL_VFE_HDMI_AVI_PICTURE_ARC_T ePictureAspectRatio;	/**< HDMI info */	/* M1M0 */

	HAL_VFE_HDMI_AVI_ACTIVE_FORMAT_ARC_T eActiveFormatAspectRatio;/**< HDMI info */	/* R3R0 */

	HAL_VFE_HDMI_AVI_SCALING_T eScaling;						/**< HDMI info */	/* SC1SC0 */

	UINT8 VideoIdCode;								/**< HDMI info */	/* VICn */

	UINT8 PixelRepeat;								/**< HDMI info */

	HAL_VFE_HDMI_AVI_IT_CONTENT_T eITContent;			/**< HDMI info */	/*ITC */

	HAL_VFE_HDMI_AVI_EXT_COLORIMETRY_T eExtendedColorimetry;	/**< HDMI info */	/* EC2EC1EC0 */

	HAL_VFE_HDMI_AVI_RGB_QUANTIZATION_RANGE_T eRGBQuantizationRange;	/**< HDMI info */	/* Q1Q0 */

	HAL_VFE_HDMI_AVI_YCC_QUANTIZATION_RANGE_T eYCCQuantizationRange;	/**< HDMI info */	/* YQ1YQ0 */

	HAL_VFE_HDMI_AVI_CONTENT_TYPE_T eContentType;			/**< HDMI info */	/* Contents Type */

	/* bar info */
	UINT16 TopBarEndLineNumber;				/**< HDMI info */
	UINT16 BottomBarStartLineNumber;		/**< HDMI info */
	UINT16 LeftBarEndPixelNumber;			/**< HDMI info */
	UINT16 RightBarEndPixelNumber;			/**< HDMI info */

	HAL_VFE_HDMI_PACKET_STATUS_T packetStatus;	/**< HDMI info */

	HAL_VFE_HDMI_IN_PACKET_T packet;					/**< HDMI info */
} HAL_VFE_HDMI_AVI_T;


//=============================================================
//							 HDMI SPD Info.
//=============================================================


/**
 *
 * HDMI SPD Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
    UINT8 VendorName[HAL_VFE_HDMI_SPD_IF_VENDOR_LEN+1];  /* add space for NULL */
    UINT8 ProductDescription[HAL_VFE_HDMI_SPD_IF_DESC_LEN+1];
    UINT8 SourceDeviceInfo;
    HAL_VFE_HDMI_PACKET_STATUS_T packetStatus;				/**< HDMI VSI info */
    HAL_VFE_HDMI_IN_PACKET_T packet;								/**< HDMI VSI info */
} HAL_VFE_HDMI_SPD_T;

/**


//=============================================================
//							 HDMI VSI Info.
//=============================================================


/**
 * HDMI video format
 *
*/
typedef enum
{
	HAL_VFE_HDMI_VSI_VIDEO_FORMAT_NO_ADDITIONAL_FORMAT, //2D
	HAL_VFE_HDMI_VSI_VIDEO_FORMAT_EXTENDED_RESOLUTION_FORMAT,
	HAL_VFE_HDMI_VSI_VIDEO_FORMAT_3D_FORMAT,
}HAL_VFE_HDMI_VSI_VIDEO_FORMAT_T;

/**
 * HDMI Video VSI 3D Info
 * see HDMI Spec. v1.4a
*/
typedef enum
{
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_FRAME_PACKING,			//*0

	HAL_VFE_HDMI_VSI_3D_STRUCTURE_FIELD_ALTERNATIVE,		//*1
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_LINE_ALTERNATIVE,		//*2
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_FULL,		//*3
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_L_DEPTH,				//*4
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_L_DEPTH_GRAPHICS,		//*5

	HAL_VFE_HDMI_VSI_3D_STRUCTURE_TOP_BOTTOM,			//*6
	/*reserved 7*/
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_HALF	=	0x08,
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_TOP_BOTTOM_DIRECTV =	0x09,

}HAL_VFE_HDMI_VSI_3D_STRUCTURE_T;


/**
 * HDMI Video VSI 3D Info
 * see HDMI Spec. v1.4a
*/
typedef enum
{
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_0,
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_1,
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_2,
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_3,

	HAL_VFE_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_0 = 0x04,
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_1,
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_2,
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_3,

}HAL_VFE_HDMI_VSI_3D_EXT_DATA_T;


/**
 * HDMI Video Format Identification Code for extended resolution 0~ff
 *
*/
typedef enum
{
	HAL_VFE_HDMI_RESERVED,
	HAL_VFE_HDMI_VSI_VIC_4K2K_30HZ,
	HAL_VFE_HDMI_VSI_VIC_4K2K_25HZ,
	HAL_VFE_HDMI_VSI_VIC_4K2K_24HZ,
	HAL_VFE_HDMI_VSI_VIC_4K2K_24HZ_SMPTE,
}HAL_VFE_HDMI_VSI_VIC_T;




/**
 * HDMI VSI Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
	HAL_VFE_HDMI_VSI_VIDEO_FORMAT_T eVideoFormat;	/**< HDMI VSI info */
	HAL_VFE_HDMI_VSI_3D_STRUCTURE_T e3DStructure;	/**< HDMI VSI info */
	HAL_VFE_HDMI_VSI_3D_EXT_DATA_T e3DExtData;		/**< HDMI VSI info */

	HAL_VFE_HDMI_VSI_VIC_T eVSiVIC;					/**< HDMI VSI info */

    UINT8 IEERegId[HAL_VFE_HDMI_VENDOR_SPECIFIC_REGID_LEN];	/**< HDMI VSI info */
    UINT8 PayLoad[HAL_VFE_HDMI_VENDOR_SPECIFIC_PAYLOAD_LEN];	/**< HDMI VSI info */
    HAL_VFE_HDMI_PACKET_STATUS_T packetStatus;				/**< HDMI VSI info */
    HAL_VFE_HDMI_IN_PACKET_T packet;								/**< HDMI VSI info */
} HAL_VFE_HDMI_VSI_T;




//=============================================================
//							 HDMI PACKET Info.
//=============================================================


/**
 * HDMI PACKET Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{

	HAL_VFE_HDMI_MODE_T eHDMIMode;				/**< HDMI info */
	HAL_VFE_HDMI_AVI_T	stAVI;		/**< HDMI AVI info */
	HAL_VFE_HDMI_SPD_T	stSPD;		/**< HDMI SPD info */
	HAL_VFE_HDMI_VSI_T	stVSI;		/**< HDMI VSI info */

} HAL_VFE_HDMI_ALL_PACKET_T;



#if 0

/**
 * HDMI GCP Info.
 *
 * @see HDMI Spec.
*/
typedef enum
{
  HAL_VFE_HDMI_GCP_NOT_INDICATED = 0,
  HAL_VFE_HDMI_GCP_24BITS_PER_PIXEL = 4,
  HAL_VFE_HDMI_GCP_30BITS_PER_PIXEL = 5,
  HAL_VFE_HDMI_GCP_36BITS_PER_PIXEL = 6,
  HAL_VFE_HDMI_GCP_48BITS_PER_PIXEL = 7,
} HAL_VFE_HDMI_GCP_COLOR_DEPTH;

/**
 * HDMI GCP Info.
 *
 * @see HDMI Spec.
*/
typedef enum
{
    HAL_VFE_HDMI_GCP_PP_PHASE4 = 0,
    HAL_VFE_HDMI_GCP_PP_PHASE1 = 1,
    HAL_VFE_HDMI_GCP_PP_PHASE2 = 2,
    HAL_VFE_HDMI_GCP_PP_PHASE3 = 3,
    HAL_VFE_HDMI_GCP_PP_RESERVED = 4,
} HAL_VFE_HDMI_GCP_PIXEL_PACKING_PHASE;

/**
 *
 * HDMI GCP Packet Info.
 *
 * @see HDMI Spec. v1.3a or above
*/
typedef struct
{
    HAL_VFE_HDMI_GCP_COLOR_DEPTH e_CD;
    HAL_VFE_HDMI_GCP_PIXEL_PACKING_PHASE e_PP;
    BOOLEAN bSet_AVMUTE;
    BOOLEAN bDefault_Phase;

} HAL_VFE_HDMI_GCP_T;

#endif

/**
 * HDMI Rect.
 *
 * @see DDI Implementation Guide
*/
typedef struct
{
	UINT16		x;	/**< horizontal Start Position in pixel [0,].*/
	UINT16		y;	/**< vertical   Start Position in pixel [0,].*/	
	UINT16		w;	/**< horizontal Size(Width)    in pixel [0,].*/
	UINT16		h;	/**< vertical   Size(Height)   in pixel [0,].*/
} HAL_VFE_HDMI_RECT_T;

/**
 * 3D Video Full Timing Information.
 *
 * @see DDI Implementation Guide
*/
/*  have to refer timing info, doesn't refer 3D tag or header info */
typedef enum
{
	/*     아래 enum value는 관리 안함.  */
	HAL_VFE_HDMI_3D_FORMAT_2D,					/**< for No 3D (OFF)..... INPUT_TYPE_2DTO3D */

	/*      From UI, 3D ON이면서 아래 Format이 넘어오면 일반 3D ON이 됨.    */
	//interim format - half
	HAL_VFE_HDMI_3D_FORMAT_TOP_BOTTOM,			/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_TOPNBOTTOM*/
	HAL_VFE_HDMI_3D_FORMAT_SIDEBYSIDE_HALF,	/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_SIDEBYSIDE*/
	HAL_VFE_HDMI_3D_FORMAT_CHECKER_BOARD,		/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_CHECKERBOARD*/
	HAL_VFE_HDMI_3D_FORMAT_FRAME_SEQ,			/**< for T/B, S/S, Checker, Frame Seq...INPUT_TYPE_FRAMESEQ*/
	HAL_VFE_HDMI_3D_FORMAT_COLUMN_INTERLEAVE,	/**< for H.264*/
	HAL_VFE_HDMI_3D_FORMAT_LINE_INTERLEAVE_HALF,	/**< for H.264*/

	//Full format
	HAL_VFE_HDMI_3D_FORMAT_FRAME_PACKING,			/**< Full format*/
	HAL_VFE_HDMI_3D_FORMAT_FIELD_ALTERNATIVE,		/**< Full format*/
	HAL_VFE_HDMI_3D_FORMAT_LINE_ALTERNATIVE,		/**< Full format (row interleave)*/
	HAL_VFE_HDMI_3D_FORMAT_SIDEBYSIDE_FULL,		/**< Full format*/
	HAL_VFE_HDMI_3D_FORMAT_DUALSTREAM,		/**< Full format*/
	/*    From UI, 3D ON이면서 아래 Format이 넘어오면 2D to 3D 가 됨    */
	HAL_VFE_HDMI_3D_FORMAT_2DTO3D,					/**< for2D TO  3D..... INPUT_TYPE_2DTO3D */
	HAL_VFE_HDMI_3D_FORMAT_MAX,
	
}HAL_VFE_HDMI_3D_FORMAT_TYPE_T;


/**
 * 3D L/R selection
 * .
*/
typedef enum
{
	HAL_VFE_HDMI_3D_LR,	//Driver와 일치.
	HAL_VFE_HDMI_3D_RL,
}HAL_VFE_HDMI_3D_LR_TYPE_T;


//=============================================================
//							 HDMI TIMING Info.
//=============================================================

/**
 * HDMI Timing Information.
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
	HAL_VFE_HDMI_RECT_T	active;	/**< ... */	
	UINT16 scanType;	/**< ... */
	UINT8  mode;	    /**< update by LG */

	HAL_VFE_HDMI_3D_FORMAT_TYPE_T full_3d_timing;	// 3D format information base on timing info
	HAL_VFE_HDMI_3D_LR_TYPE_T	video_LR_order;

} HAL_VFE_HDMI_TIMING_INFO_T;


/******************************************************************************
    전역 형 정의 (Global Type Definitions)
******************************************************************************/

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

DTV_STATUS_T  	HAL_VFE_HDMI_Initialize(void);
DTV_STATUS_T  	HAL_VFE_HDMI_Uninitialize(void);
DTV_STATUS_T  	HAL_VFE_HDMI_Open(void);
DTV_STATUS_T  	HAL_VFE_HDMI_Close(void);
DTV_STATUS_T  	HAL_VFE_HDMI_Connect(UINT8 port);
DTV_STATUS_T  	HAL_VFE_HDMI_Disconnect(UINT8 port);
DTV_STATUS_T	HAL_VFE_HDMI_GetTimingInfo(HAL_VFE_HDMI_TIMING_INFO_T *pInfo);


DTV_STATUS_T	HAL_VFE_HDMI_GetVSIInfo(HAL_VFE_HDMI_VSI_T *pPacket);
DTV_STATUS_T    HAL_VFE_HDMI_GetSPDInfo(HAL_VFE_HDMI_SPD_T *pPacket);
DTV_STATUS_T	HAL_VFE_HDMI_GetAVIInfo(HAL_VFE_HDMI_AVI_T *pPacket);
DTV_STATUS_T 	HAL_VFE_HDMI_GetPacketInfo(HAL_VFE_HDMI_ALL_PACKET_T *pPacket);

DTV_STATUS_T HAL_VFE_HDMI_GetHDMISW5V(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI_GetConnectionState(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI_ReadEDID(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI_WriteEDID(UINT8 port, UINT8 *pData);

DTV_STATUS_T HAL_VFE_HDMI_WriteHDCP(UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI_ReadHDCP(UINT8 *pData);

DTV_STATUS_T HAL_VFE_HDMI_GetSourceKSVData(UINT8 port, UINT8 *pKSVData);
BOOLEAN HAL_VFE_HDMI_MHL_IsMode(void);
DTV_STATUS_T HAL_VFE_HDMI_MHL_SendMSCMsg(UINT8 rcp);
DTV_STATUS_T HAL_VFE_HDMI_MHL_SendWriteBurst(UINT8 *mhl_msg);
DTV_STATUS_T HAL_VFE_HDMI_MHL_ReadWriteBurst(char *mhl_msg);

DTV_STATUS_T HAL_VFE_HDMI_EnableHPD(void);
DTV_STATUS_T HAL_VFE_HDMI_DisableHPD(void);

DTV_STATUS_T HAL_VFE_HDMI_DisableInternalEDIDAccess(UINT8 port);
DTV_STATUS_T HAL_VFE_HDMI_WriteExternalEDID(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI_ReadExternalEDID(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI20_ReadExternalEDID(UINT8 port, UINT8 *pData);
BOOLEAN HAL_VFE_HDMI_GetMHLMode(UINT8 port);
BOOLEAN HAL_VFE_HDMI_MHL_CheckRCPMsg(void);
DTV_STATUS_T HAL_VFE_HDMI_MHL_ReceiveRCPMsg(UINT8* rcp);
DTV_STATUS_T HAL_VFE_HDMI_MHL_CleaerRCPReg();

DTV_STATUS_T HAL_VFE_HDMI20_WriteExternalEDID(UINT8 port, UINT8 *pData, int size, UINT8 addr);
DTV_STATUS_T HAL_VFE_HDMI_GetPortTimingInfo(UINT8 port, HAL_VFE_HDMI_TIMING_INFO_T *pInfo);
DTV_STATUS_T HAL_VFE_HDMI_GetPortVSIInfo(UINT8 port, HAL_VFE_HDMI_VSI_T *pVSIPacket);
DTV_STATUS_T HAL_VFE_HDMI_GetPortSPDInfo(UINT8 port, HAL_VFE_HDMI_SPD_T *pPacket);
DTV_STATUS_T HAL_VFE_HDMI_GetPortAVIInfo(UINT8 port, HAL_VFE_HDMI_AVI_T *pPacket);
DTV_STATUS_T HAL_VFE_HDMI_GetPortPacketInfo(UINT8 port, HAL_VFE_HDMI_ALL_PACKET_T *pPacket);
BOOLEAN HAL_VFE_HDMI_CheckErrorHDMI20(UINT8 port);
DTV_STATUS_T HAL_VFE_HDMI_WriteHDCP22(UINT8 *pData, UINT32 size);

DTV_STATUS_T HAL_VFE_HDMI_ReadHDCP14(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI_EnableMHL(BOOLEAN value);

DTV_STATUS_T HAL_VFE_HDMI20_ReadInternalEDID(UINT8 port, UINT8 *pData);
DTV_STATUS_T HAL_VFE_HDMI20_WriteInternalEDID(UINT8 port, UINT8 *pData, int size);
#endif
