/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef _VDEC_INFO_COMMON_H_
#define _VDEC_INFO_COMMON_H_

#include "drv_config.h"
#if(CONFIG_DRV_VERIFY_SUPPORT)
#include "vdec_verify_general.h"
#include "vdec_info_h264.h"
#include "vdec_info_h265.h"
#include "vdec_info_wmv.h"
#include "vdec_info_mpeg.h"
#include "vdec_info_rm.h"
#include "vdec_info_vp6.h"
#include "vdec_info_vp8.h"
#include "vdec_info_avs.h"

#endif
//#include "vdec_verify_hal_errcode.h"
//#include "vdec_info_verify.h"
//#include "typedef.h"

#define START_CODE 0x00000001

#define NO_PIC 0
#define TOP_FIELD 1
#define BOTTOM_FIELD 2
#define FRAME 3
#define TOP_BOTTOM_FIELD 4
//#define HEVC_VP_MODE

#define HEVC_DUMP_MC_VLDTOP_LOG

#if 0
typedef enum
{
    VDEC_UNKNOWN = 0xFF,
    VDEC_MPEG       = 0x0,                 ///< MPEG Deocde Request
    VDEC_MPEG1      = 0x1,               ///< MPEG1 Deocde Request
    VDEC_MPEG2      = 0x2,               ///< MPEG2 Deocde Request
    VDEC_DIVX3       = 0x3,              ///< MPEG3 Deocde Request
    VDEC_MPEG4      = 0x4,              ///< MPEG4 Deocde Request
    VDEC_WMV         = 0x10,            /// < WMV Decode Request
    VDEC_WMV1       = 0x11,             ///< WMV7 Deocde Request
    VDEC_WMV2       = 0x12,            ///< WMV8 Deocde Request
    VDEC_WMV3       = 0x13,            ///< WMV9 Deocde Request
    VDEC_VC1        = 0x111,          ///< VC1 Deocde Request
    VDEC_H265        = 0x265,           ///< H265 Deocde Request
    VDEC_H264        = 0x264,           ///< H264 Deocde Request
    VDEC_H263        = 0x263,           ///< H263 Deocde Request
    VDEC_RM          = 0x300,             ///< RM Decode Request
    VDEC_VP6         = 0x600,             ///< VP6 Decode Request
    VDEC_AVS         = 0x700,             ///< AVS Decode Request
    VDEC_VP8         = 0x800,             ///< VP8 Decode Request
    VDEC_JPG         = 0x900,              ///< JPG Decode Request
    VDEC_WEBP        = 0xA00
}VDEC_CODEC_T;
#endif

#if DTV_COMMON_CONFIG
/** vdec format.
*/
typedef enum
{
    VDEC_FMT_MPV,
    VDEC_FMT_MP4,
    VDEC_FMT_H264,
    VDEC_FMT_WMV,
    VDEC_FMT_H264VER,
    VDEC_FMT_MJPEG,
    VDEC_FMT_RV,
    VDEC_FMT_AVS,
    VDEC_FMT_VP6,
    VDEC_FMT_VP8,
    VDEC_FMT_H265,    
    VDEC_FMT_VP9,
    VDEC_FMT_RAW,
    VDEC_FMT_OGG,
    VDEC_FMT_JPG,
    VDEC_FMT_PNG,
    VDEC_FMT_WEBP,
    VDEC_FMT_MAX
}   ENUM_VDEC_FMT_T;
#endif


/// Enumerate read pointer align type
typedef enum
{
    BYTE_ALIGN = 0,          ///< BYTE Align Request
    WORD_ALIGN,              ///< WORD Align Request
    DWRD_ALIGN,              ///< UINT32 Align Request
} RPTR_ALIGN_TYPE;


typedef struct _VDEC_INFO_VDSCL_PRM_T_
{
    BOOL     fgDSCLEn; // 1: 709 to 601
    BOOL     fgEnColorCvt; // 1: 709 to 601
    BOOL     fgYOnly;
    BOOL     fgMbaff;
    UCHAR   ucPicStruct;
    UCHAR   ucScrAgent;  // 0:MC 2:PP 3:FGT
    UCHAR   ucSpectType;
    UCHAR   ucScanType; //0: block 1:raster
    UCHAR   ucVdoFmt; // 0:420 1:422
    UCHAR   ucAddrSwapMode;
    UINT32  u4DispW;
    UINT32  u4SrcWidth;
    UINT32  u4SrcHeight;
    UINT32  u4SrcYOffH;
    UINT32  u4SrcYOffV;
    UINT32  u4SrcCOffH;
    UINT32  u4SrcCOffV;
    UINT32  u4TrgBufLen;
    UINT32  u4TrgWidth;
    UINT32  u4TrgHeight;
    UINT32  u4TrgOffH;
    UINT32  u4TrgOffV;
    UINT32   u4TrgYAddr;
    UINT32   u4TrgCAddr;
    UINT32   u4WorkAddr;
    UINT32   u4QY;
    UINT32   u4QC;
    UINT32   u4R_norm;
    UINT32   u4OffY;
    UINT32   u4OffC;

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    UINT32   u4SclYOffH;
    UINT32   u4SclCOffH;
    UINT32   u4SclYOffV;
    UINT32   u4SclCOffV;

    BOOL     fgLumaKeyEn;
    UINT16   u2LumaKeyValue;
#endif
}VDEC_INFO_VDSCL_PRM_T;


#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
typedef struct _VDEC_INFO_LBD_PRM_T_
{
    UINT32 u4UpBoundStart;
    UINT32 u4UpBoundEnd;
    UINT32 u4LowBoundStart;
    UINT32 u4LowBoundEnd;
    UINT32 u4LeftBound;
    UINT32 u4RightBound;
    UINT32 u4YOffset;
    UINT32 u4COffset;
    UINT32 u4YValueThd;
    UINT32 u4CValueThd;
    UINT32 u4YCntThd;
    UINT32 u4CCntThd;
}VDEC_INFO_LBD_PRM_T;
#endif


typedef struct _VDEC_INFO_DEC_PRM_T_
{
    UCHAR   ucPicStruct;
    UCHAR   ucPicType;
    UCHAR   ucDecFBufIdx;
    UCHAR   ucAddrSwapMode;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)    //mtk40110 Qing.Li 2010/11/25, to fix mpeg4 DCAC Pred bug
    UCHAR   ucMpegSpecType;    //1:mpeg12, 2:mpeg4&h263, 3:divx 
#endif
	BOOL		fgIsUFOMode;
    UINT32  u4PicBW;
    UINT32  u4PicW;
    UINT32  u4PicH;
//UFO
    UINT32  u4UFO_LEN_SIZE_Y;
    UINT32  u4PIC_SIZE_BS;
    UINT32  u4PIC_SIZE_Y_BS;
    UINT32  u4BitstreamLen;
    VDEC_INFO_VDSCL_PRM_T rDownScalerPrm;
#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) && CONFIG_DRV_FTS_SUPPORT)
    VDEC_INFO_LBD_PRM_T rLBDPrm;
#endif
    void *prVDecCodecHalPrm;
    #if(CONFIG_DRV_VERIFY_SUPPORT)
    union
    {
      VDEC_INFO_H264_DEC_PRM_T rVDecH264DecPrm;
      VDEC_INFO_H265_DEC_PRM_T rVDecH265DecPrm;
      VDEC_INFO_WMV_DEC_PRM_T rVDecWMVDecPrm;
      VDEC_INFO_MPEG_DEC_PRM_T rVDecMPEGDecPrm;
      VDEC_INFO_RM_DEC_PRM_T rVDecRMDecPrm;
      VDEC_INFO_VP6_DEC_PRM_T rVDecVP6DecPrm;
      VDEC_INFO_AVS_DEC_PRM_T rVDecAVSDecPrm;
      VDEC_INFO_VP8_DEC_PRM_T rVDecVP8DecPrm;
    } SpecDecPrm;
    #endif

}VDEC_INFO_DEC_PRM_T;

#endif //#ifndef _HAL_VDEC_COMMON_IF_H_

