/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: nav_mm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_mm.c
 *
 */

#include "x_os.h"
#include "x_typedef.h"
#include "x_drv_cli.h"
#include "widget_if.h"
#include "nav.h"
#include "nav_av.h"
#include "nav_event.h"
#include "nav_vquality.h"
#include "nav_aquality.h"
#include "nav_freqtbl.h"
#include "nav_config.h"
#include "nav_mm.h"
#include "nav_viewer.h"
#include "ir_if.h"
#include "gfx_if.h"
#include "musb_if.h"
#include "swdmx_if.h"
#ifdef CC_SUPPORT_PVR
#include "fvr_if.h"
#endif

#include "x_blk_dev.h"

#if 1
#if defined(CC_MT5387) && !defined(CC_MT5363) && !defined(CC_MT5301A)
#include "../5387_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5363) && !defined(CC_MT5365)
#include "../5363_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5365)
#include "../5365_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5389)
#include "../5389_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5368)
#include "../5368_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5398)
#include "../5398_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5880)
#include "../5880_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5860)||defined(CC_MT5881)
#include "../5860_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5396)
#include "../5396_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5395)
#include "../5395_driver/fm/x_fm.h"
#include "playmgr_if.h"
#elif defined(CC_MT5392B)
#include "../5392b_driver/fm/x_fm.h"
#include "../5392b_driver/playmgr/playmgr_if.h"
#elif defined(CC_MT5391)
#include "../5391_driver/fm/x_fm.h"
#include "../5391_driver/playmgr/playmgr_if.h"
#else
#include "x_fm.h"
#include "playmgr_if.h"
#endif
#else
#include "x_fm.h"
#include "playmgr_if.h"
#endif

#if 0

#if defined(CC_MT5387) && !defined(CC_MT5363)
#include "../5387_driver/fm/x_fm.h"
#ifndef DISABLE_MT5360_MM
#include "../5387_driver/playmgr/playmgr_if.h"
#endif
#endif

#if defined(CC_MT5363) && !defined(CC_MT5365)
#include "../5363_driver/fm/x_fm.h"
#ifndef DISABLE_MT5360_MM
#include "../5363_driver/playmgr/playmgr_if.h"
#endif
#endif

#if defined(CC_MT5365)
#include "../5365_driver/fm/x_fm.h"
#ifndef DISABLE_MT5360_MM
#include "../5365_driver/playmgr/playmgr_if.h"
#endif
#endif

#else

//#include "../5363_driver/fm/x_fm.h"
//#include "playmgr_if.h"

#endif

//EXTERN Funtions
EXTERN UINT64 u8Div6432(UINT64 u8Dividend, UINT64 u8Divider, UINT64 *pu8Remainder);
EXTERN INT32 fm_init(VOID);
EXTERN INT32 x_fm_fat_init(VOID *pv_data);
#ifndef __KERNEL__
EXTERN void vSetUSBDump(BOOL);
#endif

#ifdef DISABLE_MT5360_MM
/**The video codec format*/
typedef enum{
    PLAYMGR_VDEC_UNKNOWN,
    PLAYMGR_VDEC_MPEG1_MPEG2,
    PLAYMGR_VDEC_DIVX311,
    PLAYMGR_VDEC_MPEG4,
    PLAYMGR_VDEC_H264,
    PLAYMGR_VDEC_MVC,
    PLAYMGR_VDEC_AVS,
    PLAYMGR_VDEC_VC1,
    PLAYMGR_VDEC_WMV7,
    PLAYMGR_VDEC_WMV8,
    PLAYMGR_VDEC_WMV9,
    PLAYMGR_VDEC_H263,
    PLAYMGR_VDEC_MJPEG,
    PLAYMGR_VDEC_RV8,
    PLAYMGR_VDEC_RV9,
    PLAYMGR_VDEC_SORENSON,
    PLAYMGR_VDEC_VP6,
    PLAYMGR_VDEC_VP8,
    PLAYMGR_VDEC_MULTITRACK,
    PLAYMGR_VDEC_MAX
}PLAYMGR_VDEC_T;

/**Audio decoder format.*/
typedef enum{
    PLAYMGR_ADEC_UNKNOWN = 0,
    PLAYMGR_ADEC_MPEG,
    PLAYMGR_ADEC_AC3,
    PLAYMGR_ADEC_PCM,
    PLAYMGR_ADEC_MP3,
    PLAYMGR_ADEC_AAC,
    PLAYMGR_ADEC_DTS,
    PLAYMGR_ADEC_WMA,
    PLAYMGR_ADEC_RA,
    PLAYMGR_ADEC_HDCD,
    PLAYMGR_ADEC_MLP,
    PLAYMGR_ADEC_MTS,
    PLAYMGR_ADEC_A2,
    PLAYMGR_ADEC_PAL,
    PLAYMGR_ADEC_FMFM,
    PLAYMGR_ADEC_NICAM,
    PLAYMGR_ADEC_TTXAAC,
    PLAYMGR_ADEC_DETECTOR,
    PLAYMGR_ADEC_MINER,
    PLAYMGR_ADEC_ADPCM
}PLAYMGR_ADEC_T;

/**Media Type
*/
typedef enum
{
  PLAYMGR_MEDIA_TYPE_UNKNOWN, // Unknown format type
  PLAYMGR_MEDIA_TYPE_AVI, // AVI
  PLAYMGR_MEDIA_TYPE_DIVX, // Divx
  PLAYMGR_MEDIA_TYPE_MPEG2_PS, // Mpeg2 program stream
  PLAYMGR_MEDIA_TYPE_MPEG2_TS, // Mpeg2 transport stream
  PLAYMGR_MEDIA_TYPE_MPEG2_TS_192, // Mpeg2 192 transport stream
  PLAYMGR_MEDIA_TYPE_MPEG2_TS_ZERO_192, // Mpeg2 4 zero byte transport stream
  PLAYMGR_MEDIA_TYPE_MPEG2_TS_TIME_SHIFT, // Mpeg2 time shift 192 transport stream
  PLAYMGR_MEDIA_TYPE_MPEG2_ES, // Mpeg2 element stream
  PLAYMGR_MEDIA_TYPE_H264_ES, // H264 element stream
  PLAYMGR_MEDIA_TYPE_MPEG4_ES,
  PLAYMGR_MEDIA_TYPE_AVS_ES,
  PLAYMGR_MEDIA_TYPE_VC1_ES,
  PLAYMGR_MEDIA_TYPE_VC1_WMV,
  PLAYMGR_MEDIA_TYPE_VCD,
  PLAYMGR_MEDIA_TYPE_VOB,
  PLAYMGR_MEDIA_TYPE_VRO,
  PLAYMGR_MEDIA_TYPE_MP2, // MPEG Audio Layer 2
  PLAYMGR_MEDIA_TYPE_MP3, // MPEG Audio Layer 3
  PLAYMGR_MEDIA_TYPE_MPA, // MPEG Audio
  PLAYMGR_MEDIA_TYPE_JPEG,
  PLAYMGR_MEDIA_TYPE_MP4,
  PLAYMGR_MEDIA_TYPE_MKV,
  PLAYMGR_MEDIA_TYPE_FLV,
  PLAYMGR_MEDIA_TYPE_RM,
  PLAYMGR_MEDIA_TYPE_WAVE,
  PLAYMGR_MEDIA_TYPE_AIF,
  PLAYMGR_MEDIA_TYPE_AC3,
  PLAYMGR_MEDIA_TYPE_AAC,
  PLAYMGR_MEDIA_TYPE_LPCM,
  PLAYMGR_MEDIA_TYPE_OGG,
  PLAYMGR_MEDIA_TYPE_MAX
} PLAYMGR_MEDIA_TYPE_T;

/**caption format.*/
typedef enum
{
    PLAYMGR_CAPTION_UNKNOWN = 0, // Unknown format type
    PLAYMGR_CAPTION_MPEG2,
    PLAYMGR_CAPTION_AVI_TEXT,
    PLAYMGR_CAPTION_AVI_BITMAP,
    PLAYMGR_CAPTION_AVI_DXSA
}PLAYMGR_CAPTION_FMT_T;

typedef struct
{
    UINT16 u2VTrack; // Specify the video track for playback
    UINT16 u2ATrack; // Specify the audio track for playback
    UINT16 u2CapTrack; // Specify the caption track for playback
    UINT16 u2PcrTrack;

    INT32 i4Speed;                         // playback speed (+1000 = Forward 1X, +2000 = Forward 2X, -1000 = Backward 1X, ...)
    UINT32 u4OsdId;			               // the id of the OSD for display (image only)
} PLAYMGR_SETTING_T;

typedef struct
{
    PLAYMGR_MEDIA_TYPE_T   eMediaType;   // media type

    PLAYMGR_VDEC_T eVidFormat;  // video format
    PLAYMGR_ADEC_T  eAudFormat; // audio format
    PLAYMGR_CAPTION_FMT_T eCapFormat; // caption format

    UINT16 u2TotalPrmNum;    // For TS, total program may bigger then 1

    UINT16 u2VideoTracks; // number of video tracks
    UINT16 u2AudioTracks; // number of audio tracks
    UINT16 u2CaptionTracks; // number of caption tracks
    UINT16 u2PcrTracks;      // number of pcr tracks

    UINT32 u4TotalDuration;                // file's total time(millisecond)
    UINT64 u8FileSize;                     // total file size
    UINT32 u4PlayCap;                      // Play Capability
    UINT32 u4CurPos;                       // current position (millisecond)
} PLAYMGR_STATUS_T;

#define PLAYMGR_MEDIA_TYPE_UNKNOWN (0)
BOOL PLAYMGR_Init(VOID)
{
    return TRUE;
};
BOOL PLAYMGR_Open(UCHAR* pucFileName)
{
    return TRUE;
}
BOOL PLAYMGR_Close(VOID)
{
    return TRUE;
}
BOOL PLAYMGR_Reset(VOID)
{
    return TRUE;
}
BOOL PLAYMGR_Set(PLAYMGR_SETTING_T* prSettings)
{
    return TRUE;
}
BOOL PLAYMGR_Start(VOID)
{
    return TRUE;
}
BOOL PLAYMGR_Stop(VOID)
{
    return TRUE;
}
BOOL PLAYMGR_Pause(VOID)
{
    return TRUE;
}
BOOL PLAYMGR_Resume(VOID)
{
    return TRUE;
}
BOOL PLAYMGR_GetInfo(PLAYMGR_STATUS_T *prStatus)
{
    return TRUE;
}
VOID SWDMX_Init(VOID) {}
#endif  //DISABLE_MT5360_MM

VOID _MM_PlaymgrCallback(UINT8 u1SrcId,
                         PLAYMGR_CALLBACK_TYPE_T eCallbackType,
                         PLAYMGR_CALLBACKPARAM_T *prParams,
                         UINT32 u4Param);

#define MAX_LENGTH_FILE_NAME 256
#define MAX_NUM_OF_FILES 256
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)||defined(CC_MT5881))
#define NUM_OF_SUPPORT_FILE_EXTENSION 57 // add webm file support
#else
#define NUM_OF_SUPPORT_FILE_EXTENSION 55
#endif

/* Time seek by Digit Number key */
// move to nav_mm.h
//#define TIME_SEEK_BY_KEY
/* speed support */
#define MAX_SPEED (32000)

#define B 0xF237
#define W 0xFFFF
#define Y 0xFF90

#define USB_ROOT_PATH "/mnt/usb_0"
#define FILE_EXTEN_LEN (5)

typedef struct
{
    CHAR szFileName[MAX_LENGTH_FILE_NAME];
    UINT8 u1FileType;
} FILE_LIST_T;

EXTERN void VDEC_SetWptr(UINT8 u1VdecId, UINT32 u4Wptr);

static UINT16 _au2StopIcon[16 * 32] =
{
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W
};

static UINT16 _au2PlayIcon[16 * 32] =
{
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, B, W, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, B, B, B, B, B, B, B, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W
};

static UINT16 _au2ForwardIcon[16 * 32] =
{
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, W, B, B, B, B, B, W, B, B, B, B, B, W,
    W, B, B, W, B, B, B, B, B, W, B, B, B, B, B, W,
    W, B, B, W, W, B, B, B, B, W, W, B, B, B, B, W,
    W, B, B, W, W, B, B, B, B, W, W, B, B, B, B, W,
    W, B, B, W, W, W, B, B, B, W, W, W, B, B, B, W,
    W, B, B, W, W, W, B, B, B, W, W, W, B, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, W, W, W, W, W, B, W, W, W, W, W, B, W,
    W, B, B, W, W, W, W, W, B, W, W, W, W, W, B, W,
    W, B, B, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, W, W, W, W, W, B, W, W, W, W, W, B, W,
    W, B, B, W, W, W, W, W, B, W, W, W, W, W, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, W, W, W, B, B, B, W, W, W, B, B, B, W,
    W, B, B, W, W, W, B, B, B, W, W, W, B, B, B, W,
    W, B, B, W, W, B, B, B, B, W, W, B, B, B, B, W,
    W, B, B, W, W, B, B, B, B, W, W, B, B, B, B, W,
    W, B, B, W, B, B, B, B, B, W, B, B, B, B, B, W,
    W, B, B, W, B, B, B, B, B, W, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W
};


static UINT16 _au2ReverseIcon[16 * 32] =
{
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, W, B, B, B, B, B, W, B, B, W,
    W, B, B, B, B, B, W, B, B, B, B, B, W, B, B, W,
    W, B, B, B, B, W, W, B, B, B, B, W, W, B, B, W,
    W, B, B, B, B, W, W, B, B, B, B, W, W, B, B, W,
    W, B, B, B, W, W, W, B, B, B, W, W, W, B, B, W,
    W, B, B, B, W, W, W, B, B, B, W, W, W, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, W, W, W, W, W, B, W, W, W, W, W, B, B, W,
    W, B, W, W, W, W, W, B, W, W, W, W, W, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, B, B, W,
    W, B, W, W, W, W, W, B, W, W, W, W, W, B, B, W,
    W, B, W, W, W, W, W, B, W, W, W, W, W, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, W, W, W, W, B, B, W, W, W, W, B, B, W,
    W, B, B, B, W, W, W, B, B, B, W, W, W, B, B, W,
    W, B, B, B, W, W, W, B, B, B, W, W, W, B, B, W,
    W, B, B, B, B, B, W, B, B, B, B, W, W, B, B, W,
    W, B, B, B, B, B, W, B, B, B, B, W, W, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, W, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, W, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W
};

static UINT16 _au2PauseIcon[16 * 32] =
{
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, W, W, W, W, W, W, W, W, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, B, B, B, B, B, B, B, B, B, B, B, B, B, B, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W
};

static UINT16 _au2StopIconActive[16 * 32] =
{
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y
};

static UINT16 _au2PlayIconActive[16 * 32] =
{
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, B, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, B, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, B, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y
};

static UINT16 _au2ForwardIconActive[16 * 32] =
{
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B,
    B, Y, Y, B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B,
    B, Y, Y, B, B, Y, Y, Y, Y, B, B, Y, Y, Y, Y, B,
    B, Y, Y, B, B, Y, Y, Y, Y, B, B, Y, Y, Y, Y, B,
    B, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, Y, B,
    B, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, B,
    B, Y, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, B,
    B, Y, Y, B, B, B, B, B, B, B, B, B, B, B, B, B,
    B, Y, Y, B, B, B, B, B, B, B, B, B, B, B, B, B,
    B, Y, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, B,
    B, Y, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, Y, B,
    B, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, Y, B,
    B, Y, Y, B, B, Y, Y, Y, Y, B, B, Y, Y, Y, Y, B,
    B, Y, Y, B, B, Y, Y, Y, Y, B, B, Y, Y, Y, Y, B,
    B, Y, Y, B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B,
    B, Y, Y, B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y
};


static UINT16 _au2ReverseIconActive[16 * 32] =
{
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B, Y, Y, B,
    B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, Y, B, Y, Y, B,
    B, Y, Y, Y, Y, B, B, Y, Y, Y, Y, B, B, Y, Y, B,
    B, Y, Y, Y, Y, B, B, Y, Y, Y, Y, B, B, Y, Y, B,
    B, Y, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, B,
    B, Y, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, Y, B,
    B, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, Y, B,
    B, B, B, B, B, B, B, B, B, B, B, B, B, Y, Y, B,
    B, B, B, B, B, B, B, B, B, B, B, B, B, Y, Y, B,
    B, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, Y, B,
    B, Y, B, B, B, B, B, Y, B, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, B, B, B, B, Y, Y, B, B, B, B, Y, Y, B,
    B, Y, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, B,
    B, Y, Y, Y, B, B, B, Y, Y, Y, B, B, B, Y, Y, B,
    B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, B, B, Y, Y, B,
    B, Y, Y, Y, Y, Y, B, Y, Y, Y, Y, B, B, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y
};

static UINT16 _au2PauseIconActive[16 * 32] =
{
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, B, B, B, B, B, B, B, B, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y
};


static CHAR _aszMediaType2Str[][20] =
{
    "UNKNOWN",
    "AVI",
    "DIVX",
    "MPEG2_PS",
    "MPEG2_TS",
    "TS 192",
    "TS ZERO 192",
    "Time Shift",
    "MPEG2 ES",
    "H264 ES",
    "MPEG4 ES",
    "AVS ES",
    "VC1 ES",
    "VC1 WMV",
    "VCD",
    "VOB",
    "VRO",
    "MP2",
    "MP3",
    "MPA",
    "JPEG",
    "MP4",
    "MKV",
    "FLV",
    "RM",
    "WAVE",
    "AIF",
    "AC3",
    "AAC",
    "LPCM",
    "OGG"
};

static CHAR _aszVidFormat2Str[][17] =
{
    "UNKNOWN",
    "MPEG1_MPEG2",
    "DIVX311",
    "MPEG4",
    "H264",
    "MVC",
    "AVS",
    "VC1",
    "WMV7",
    "WMV8",
    "WMV9",
    "H263",
    "MJPEG",
    "RV8",
    "RV9",
    "SORENSON",
    "VP6",
    "VP8",
    "Multi-Track",
    "H265",
    "PLAYMGR_VDEC_MAX"
};

static CHAR _aszAudFormat2Str[][9] =
{
    "UNKNOWN",
    "MPEG",
    "AC3",
    "PCM",
    "MP3",
    "AAC",
    "DTS",
    "WMA",
    "RA",
    "HDCD",
    "MLP",
    "MTS",
    "A2",
    "PAL",
    "FMFM",
    "NICAM",
    "TTXAAC",
    "DETECTOR",
    "MINER",
    "ADPCM"
};

static CHAR _aszCapFormat2Str[][11] =
{
    "MPEG2",
    "AVI_TEXT",
    "AVI_BITMAP",
    "AVI_DXSA"
};

static CHAR _aszSupportedImgExtension[][FILE_EXTEN_LEN] =
{
  "jpeg",
  "JPEG",
  "jpg",
  "JPG"
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)||defined(CC_MT5881))
  ,"WEBP"
  ,"webp"
  ,"PNG"
  ,"png"
#endif
};

static CHAR _aszSupportedMMExtension[][FILE_EXTEN_LEN] =
{
    "avi",
    "AVI",
    "mpg",
    "MPG",
    "mpeg",
    "MPEG",
    "ts",
    "TS",
    "m2ts",
    "M2TS",
    "tts",
    "TTS",
    "wmv",
    "WMV",
    "wma",
    "WMA",
    "vc1",
    "VC1",
    "m2v",
    "M2V",
    "m4v",
    "M4V",
    "trp",
    "TRP",
    "vob",
    "VOB",
    "dat",
    "DAT",
    "264",
    "26l",
    "mp4",
    "mkv",
    "MKV",
    "mp3",
    "MP3",
    "rm",
    "RM",
    "rmvb",
    "RMVB",
    "mp2",
    "MP2",
    "aif",
    "AIF",
    "aiff",
    "AIFF",
    "wav",
    "WAV",
    "wave",
    "WAVE",
    "ac3",
    "AC3",
    "aac",
    "AAC",
    "avs",
    "AVS",
    "FLV",
    "flv"
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)||defined(CC_MT5881))
    ,"webm"
    ,"WEBM"
    ,"M4A"
    ,"M4a"
    ,"asf"
    ,"ASF"
    ,"MOV"
    ,"mov"
#endif
#ifdef CC_SUPPORT_PVR
    ,"pvr"
    ,"PVR"
#endif

};

static WIDGET_MENUITEM_T *_prBrowseItem;

static FILE_LIST_T _arFileList[MAX_NUM_OF_FILES] =
{
    {"jack0.avi"},
};

static UINT8 _u1SrcId = 0;
static UINT8 _u1VDPEnableCnt = 0;
static UINT8 _u1AUDEnableCnt = 0;
static BOOL _afgSrcIsOccupied[NAV_MM_MAX_SOURCE] = {FALSE};
static PLAY_INFO_T _arPlayInfo[NAV_MM_MAX_SOURCE] = {0};
/*
static UINT32 _u4MMState = E_NAV_MM_STOP;
static INT32  _i4MMSpeed = 1000;
static BOOL   _fgMMAudEn = FALSE;
static BOOL   _fgMMVidEn = FALSE;
static BOOL   _fgMMPcrEn = FALSE;
static BOOL   _fgMMEOS = FALSE;
static UINT32 _u4MMTotalTime = 0;
static UINT32 _u4MMCurTime = 0;

static UINT32 _u4AudTrackNumber = 0;
static UINT32 _u4CurrentAudTrack = 0;
static UINT32 _u4VidTrackNumber = 0;
static UINT32 _u4CurrentVidTrack = 0;
//static UINT32 _u4PcrTrackNumber = 0;
static UINT32 _u4CurrentPcrTrack = 0;
static UINT32 _u4PgmNumber = 0;
static UINT32 _u4CurrentPgm = 0;

#ifdef TIME_SEEK_BY_KEY
static UINT32 _u4SeekTime = 0;
static UCHAR _ucSeekKeyCount = 0;
static UCHAR _aucSeekKey[4] = "___\0";
#endif
*/
static UINT32 _u4OrgInputSrc = E_NAV_VS_DT1;

static HANDLE_T _hMonitorMsgq = NULL;
static HANDLE_T _hMonitorThread = NULL;

static CHAR _szMountPath[MAX_LENGTH_FILE_NAME];
static CHAR _szDevPath[MAX_LENGTH_FILE_NAME];

static CHAR _szCurrFile[MAX_LENGTH_FILE_NAME];

static  WIDGET_MENULIST_T* _prList;

static UINT32 _u4MMMode = E_NAV_MM_MODE_VIEWER;
static UINT8 _u1ActiveSrcId = 1;
static BOOL _fgMenuing = FALSE;

#ifdef CC_SUPPORT_PVR
static UINT32 _u4PvrTickStart = 0;
static UINT32 _u4PvrCurrentTick = 0;
static UINT32 _u4PvrTickEnd = 0;

static BOOL _fgPVRMode = FALSE;
#endif

BOOL _NAV_PLAYMGR_Open(UINT8 u1SrcId, UCHAR* pucFileName)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Open(u1SrcId, pucFileName);
#else
    return PLAYMGR_Open(pucFileName);
#endif
}
BOOL _NAV_PLAYMGR_Close(UINT8 u1SrcId)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Close(u1SrcId);
#else
    return PLAYMGR_Close();
#endif
}
BOOL _NAV_PLAYMGR_Set(UINT8 u1SrcId, PLAYMGR_SETTING_T* prSettings)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Set(u1SrcId, prSettings);
#else
    return PLAYMGR_Set(prSettings);
#endif
}
BOOL _NAV_PLAYMGR_Start(UINT8 u1SrcId)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Start(u1SrcId);
#else
    return PLAYMGR_Start();
#endif
}
BOOL _NAV_PLAYMGR_Stop(UINT8 u1SrcId)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Stop(u1SrcId);
#else
    return PLAYMGR_Stop();
#endif
}
BOOL _NAV_PLAYMGR_Pause(UINT8 u1SrcId)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Pause(u1SrcId);
#else
    return PLAYMGR_Pause();
#endif
}
BOOL _NAV_PLAYMGR_Resume(UINT8 u1SrcId)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Resume(u1SrcId);
#else
    return PLAYMGR_Resume();
#endif
}
BOOL _NAV_PLAYMGR_GetInfo(UINT8 u1SrcId, PLAYMGR_STATUS_T *prStatus)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_GetInfo(u1SrcId, prStatus);
#else
    return PLAYMGR_GetInfo(prStatus);
#endif
}
BOOL _NAV_PLAYMGR_ABRepeat(UINT8 u1SrcId, UINT32 u4ABRepeat)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_ABRepeat(_u1SrcId, (PLAYMGR_ABREPEAT_TYPE_T)(u4ABRepeat));
#endif
}
BOOL _NAV_PLAYMGR_Speed(UINT8 u1SrcId, INT32 i4Speed)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Speed(u1SrcId, i4Speed);
#else
    return PLAYMGR_Speed(i4Speed);
#endif
}
BOOL _NAV_PLAYMGR_SeekPos(UINT8 u1SrcId, UINT64 u8SeekBytes)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_SeekPos(u1SrcId, u8SeekBytes);
#else
    return PLAYMGR_SeekPos(u8SeekBytes);
#endif
}
BOOL _NAV_PLAYMGR_SeekTime(UINT8 u1SrcId, UINT32 u4SeekTime)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_SeekTime(u1SrcId, u4SeekTime);
#else
    return PLAYMGR_SeekTime(u4SeekTime);
#endif
}
BOOL _NAV_PLAYMGR_Step(UINT8 u1SrcId, INT32 i4StepAmount)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_Step(u1SrcId, i4StepAmount);
#else
    return PLAYMGR_Step(i4StepAmount);
#endif
}
BOOL _NAV_PLAYMGR_GetCap(UINT8 u1SrcId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_GetCap(u1SrcId, u4Type, u4Param1, u4Param2);
#else
    return TRUE;
#endif
}
BOOL _NAV_PLAYMGR_RegCb(UINT8 u1SrcId, PLAYMGR_CALLBACK_TYPE_T eCallbackType,
                        PLAYMGR_CALLBACK fnCallback,
                        UINT32 u4Param)
{
    UNUSED(u1SrcId);
#ifdef CC_53XX_SWDMX_V2
    return PLAYMGR_RegCb(u1SrcId, eCallbackType, fnCallback, u4Param);
#else
    return PLAYMGR_RegCb(eCallbackType, fnCallback, u4Param);
#endif
}

#ifdef CC_53XX_SWDMX_V2
VOID _NAV_MM_PlaymgrCallback(UINT8 u1SrcId,
                             PLAYMGR_CALLBACK_TYPE_T eCallbackType,
                             PLAYMGR_CALLBACKPARAM_T *prParams,
                             UINT32 u4Param)
{
    _MM_PlaymgrCallback(u1SrcId, eCallbackType, prParams, u4Param);
}
#else
VOID _NAV_MM_PlaymgrCallback(
    PLAYMGR_CALLBACK_TYPE_T eCallbackType,
    PLAYMGR_CALLBACKPARAM_T *prParams,
    UINT32 u4Param)
{
    _MM_PlaymgrCallback(0, eCallbackType, prParams, u4Param);
}
#endif

#ifdef TIME_SEEK_BY_KEY
static void _ClearSeekKeyStatus(UINT8 u1SrcId)
{
    if (u1SrcId >= NAV_MM_MAX_SOURCE)
    {
        return;
    }
    _arPlayInfo[u1SrcId].u4SeekTime = 0;
    _arPlayInfo[u1SrcId].ucSeekKeyCount = 0;
    x_strcpy((CHAR*)_arPlayInfo[u1SrcId].aucSeekKey, "___");
}
#endif

static BOOL _IsSupportedImgExtension(const CHAR* szFileName)
{
   UINT32 u4Index,u4Num;
   u4Num=sizeof(_aszSupportedImgExtension)/FILE_EXTEN_LEN;
   for (u4Index= 0; u4Index < u4Num; u4Index++)
   {
      if (x_strcmp(szFileName, _aszSupportedImgExtension[u4Index]) == 0)
      {
            return TRUE;
      }
   }
   return FALSE;
}

static BOOL _IsSupportedMMExtension(const CHAR* szFileName)
{
    UINT32 i,u4Num;
	u4Num=sizeof(_aszSupportedMMExtension)/FILE_EXTEN_LEN;
    for (i = 0; i < u4Num; i++)
    {
        if (x_strcmp(szFileName, _aszSupportedMMExtension[i]) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static CHAR* _FindExtension(const CHAR* szFileName)
{
    CHAR* szExt;
    szExt = x_strrchr(szFileName, '.') + 1;
    return szExt;
}

static INT32 _FileList(VOID)
{
    HANDLE_T    h_dir;
    FM_DIR_ENTRY_T at_dir_entry[1];
    UINT32  ui4_num_entries;

    UINT32 i = 0;

    if (FMR_OK != x_fm_open_dir(FM_ROOT_HANDLE, USB_ROOT_PATH, &h_dir))
    {
        printf("Open this directory fail!\n");
        return -1024;
    }

    while (x_fm_read_dir_entries(h_dir, at_dir_entry, 1, &ui4_num_entries) == FMR_OK)
    {
        CHAR* szExt;

        printf("%s\n", at_dir_entry[0].s_name);
        szExt = _FindExtension(at_dir_entry[0].s_name);
        printf("%s\n", szExt);
		
        if (_IsSupportedImgExtension(szExt))
        {
            x_strcpy(_arFileList[i].szFileName, at_dir_entry[0].s_name);
            _arFileList[i].u1FileType = E_NAV_MM_FILE_JPG;
            i++;
        }
        else if (_IsSupportedMMExtension(szExt))
        {
            x_strcpy(_arFileList[i].szFileName, at_dir_entry[0].s_name);
            _arFileList[i].u1FileType = E_NAV_MM_FILE_AVI;
            i++;
        }

    }

    if (FMR_OK != x_fm_close(h_dir))
    {
        return -1024;
    }

    return i;
}


static BOOL _MM_IRHandler(WIDGET_OSDINFO_T arOSD[], UINT32 u4Key, VOID* pvArg)
{
    BOOL fgRet;
    PLAYMGR_STATUS_T rPlaymgrStatus;
    PLAYMGR_SETTING_T rPlaymgrSetting;
    //CHAR aszTmp[MAX_LENGTH_FILE_NAME];

    x_memset(&rPlaymgrSetting, 0, sizeof(rPlaymgrSetting));
    if (u4Key == BTN_FAVORITE) // play
    {
#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif
        if (_arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_STOP)
        {
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;

            x_memset(&rPlaymgrStatus, 0, sizeof(rPlaymgrStatus));
            _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
            _arPlayInfo[_u1SrcId].u4MMTotalTime = rPlaymgrStatus.u4TotalDuration;
            _arPlayInfo[_u1SrcId].u4MMCurTime = rPlaymgrStatus.u4CurPos;
            _arPlayInfo[_u1SrcId].i4CurrentPgm = rPlaymgrStatus.u2EnPgmNum;
            if ((rPlaymgrStatus.u2VideoTracks > 0) && (!_arPlayInfo[_u1SrcId].fgMMVidEn) &&
                    (_arPlayInfo[_u1SrcId].i4CurrentVidTrack != -1))
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_VIDEO;
                rPlaymgrSetting.fgVidEnable = TRUE;
                rPlaymgrSetting.u2PlayVidTrack = _arPlayInfo[_u1SrcId].i4CurrentVidTrack;
                fgRet = _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                if (fgRet)
                {
                    _arPlayInfo[_u1SrcId].fgMMVidEn = TRUE;
                    if (_u1VDPEnableCnt == 0)
                    {
                        VDP_SetEnable(VDP_1, 1);
                        VDP_SetMode(VDP_1, VDP_MODE_NORMAL);
                        //VDP_SetInput(VDP_1, 0, 0);
                        VDP_SetInput(VDP_1, _u1SrcId, 0);
                    }
                    _u1VDPEnableCnt++;
                }

                // another path is video, use virtual display
                if (_u1VDPEnableCnt)
                {
#ifndef CC_DISABLE_TWO_B2R
                    VDP_SetEnable(VDP_2, 1);
                    VDP_SetMode(VDP_2, VDP_MODE_NORMAL);
                    //VDP_SetInput(VDP_1, 0, 0);
                    VDP_SetInput(VDP_2, _u1SrcId, 0);
#endif
                    // virtual display also consume video frame not belong to this
                    // video path.
                    /*
                    #ifdef FBM_VIRTUAL_DISPLAY
                    CLI_Parser("fbm.i");
                    CLI_Parser("fbm.vd.start 30");
                    #endif
                    */
                }
            }

            if ((rPlaymgrStatus.u2AudioTracks > 0) && (!_arPlayInfo[_u1SrcId].fgMMAudEn) &&
                    (_arPlayInfo[_u1SrcId].i4CurrentAudTrack != -1))
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
                rPlaymgrSetting.fgAudEnable = TRUE;
                rPlaymgrSetting.u2PlayAudTrack = _arPlayInfo[_u1SrcId].i4CurrentAudTrack;
                fgRet = _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                if (fgRet)
                {
                    _arPlayInfo[_u1SrcId].fgMMAudEn = TRUE;
                    if (_u1AUDEnableCnt == 0)
                    {
                        DRV_AUD_Unmute();
                    }
                    _u1AUDEnableCnt++;
                }
            }

            if ((rPlaymgrStatus.u2PcrTracks > 0) && (!_arPlayInfo[_u1SrcId].fgMMPcrEn))
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_PCR;
                rPlaymgrSetting.fgPcrEnable = TRUE;
                rPlaymgrSetting.u2PlayPcrTrack = _arPlayInfo[_u1SrcId].i4CurrentPcrTrack;
                fgRet = _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                if (fgRet)
                {
                    _arPlayInfo[_u1SrcId].fgMMPcrEn = TRUE;
                }
            }

            _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
            _arPlayInfo[_u1SrcId].fgMMEOS = FALSE;
            _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
            _NAV_PLAYMGR_Start(_u1SrcId);
        }
        else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PAUSE)
        {
            if ((_arPlayInfo[_u1SrcId].i4MMSpeed == 1) || (_arPlayInfo[_u1SrcId].i4MMSpeed == -1))
            {
                _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
                _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
            }
            else
            {
#ifdef NAV_MM_RESUME_TO_NORMAL
                _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
                _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
#endif
            }
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
            _NAV_PLAYMGR_Resume(_u1SrcId);
        }
        else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_FAST_FORWARD)
        {
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PAUSE;
            _NAV_PLAYMGR_Pause(_u1SrcId);
#ifdef NAV_MM_RESUME_TO_NORMAL
            _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
            _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
#endif
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
            _NAV_PLAYMGR_Resume(_u1SrcId);
        }
        else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_FAST_REVERSE)
        {
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PAUSE;
            _NAV_PLAYMGR_Pause(_u1SrcId);
#ifdef NAV_MM_RESUME_TO_NORMAL
            _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
            _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
#endif
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
            _NAV_PLAYMGR_Resume(_u1SrcId);
        }
        else
        {
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PAUSE;
            _NAV_PLAYMGR_Pause(_u1SrcId);
        }
    }
    else if (u4Key == BTN_FF)
    {
        // check if this file can be FF or not first
        if (!_NAV_PLAYMGR_GetCap(_u1SrcId, PLAYMGR_CAP_SEEK, 0, 0))
        {
            //this file can't be seek
            _arPlayInfo[_u1SrcId].u4SeekTime = 0;
            _arPlayInfo[_u1SrcId].aucSeekKey[_arPlayInfo[_u1SrcId].ucSeekKeyCount] = 'x';
            _arPlayInfo[_u1SrcId].ucSeekKeyCount++;

            if (_arPlayInfo[_u1SrcId].ucSeekKeyCount>=3)
            {
                _ClearSeekKeyStatus(_u1SrcId);
            }
        }
        else
        {

            if ( _arPlayInfo[_u1SrcId].fgIsWma)
            {
                _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
            }
            if (( _arPlayInfo[_u1SrcId].fgIsWma && (rPlaymgrStatus.u2VideoTracks <= 0)) ||
                    _arPlayInfo[_u1SrcId].fgIsPlayError)
            {
                if ( _arPlayInfo[_u1SrcId].fgIsPlayError)
                {
                    Printf("FF is disable if playmgr notify error!\n");
                }
                else
                {
                    Printf("FF is disable for Wma or Wmv format file which has no video track!\n");
                }
            }
            else
            {
#ifdef TIME_SEEK_BY_KEY
                _ClearSeekKeyStatus(_u1SrcId);
#endif
                if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PLAY)
                {
                    _arPlayInfo[_u1SrcId].i4MMSpeed = 2000;
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_FAST_FORWARD;
                    _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                }
                else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PAUSE)
                {
                    // Seek 5 second and step
                    PLAYMGR_STATUS_T rStatus;
                    UINT32 u4CurPos;
                    UINT64 u8CurBytesPos;
                    UINT64 u8Tmp;
                    UINT64 u8Unit;
                    UINT64 u8Remainder;
                    if (_NAV_PLAYMGR_GetInfo(_u1SrcId, &rStatus))
                    {
                        if ((rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_PS) ||
                                (rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS) ||
                                (rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_ES))
                        {
                            u8Tmp = (rStatus.u8FileSize*5000);
                            if (rStatus.u4TotalDuration)
                            {
                                u8Unit = u8Div6432(u8Tmp,
                                                   (UINT64)rStatus.u4TotalDuration,
                                                   &u8Remainder);
                            }
                            else
                            {
                                u8Unit = (2*1024*1024/8*5); // 5 seconds, 2M bps
                            }
                            if ((rStatus.u8CurBytePos + u8Unit) > rStatus.u8FileSize)
                            {
                                u8CurBytesPos = rStatus.u8FileSize;
                            }
                            else
                            {
                                u8CurBytesPos = (rStatus.u8CurBytePos + u8Unit);
                            }
                            // only 32 bits in cli
                            _NAV_PLAYMGR_SeekPos(_u1SrcId, u8CurBytesPos);
                        }
                        else
                        {
                            if ((rStatus.u4CurPos + 5000) > rStatus.u4TotalDuration)
                            {
                                u4CurPos = rStatus.u4TotalDuration;
                            }
                            else
                            {
                                u4CurPos = (rStatus.u4CurPos + 5000);
                            }
                            _NAV_PLAYMGR_SeekTime(_u1SrcId, u4CurPos);
                        }
                    }
                    _arPlayInfo[_u1SrcId].i4MMSpeed = 1;
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PAUSE;
                    _NAV_PLAYMGR_Step(_u1SrcId, 1);
                }
                else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_FAST_FORWARD)
                {
                    _arPlayInfo[_u1SrcId].i4MMSpeed *= 2;
                    if (_arPlayInfo[_u1SrcId].i4MMSpeed > MAX_SPEED)
                    {
                        _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
                        _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
                        _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                    }
                    else
                    {
                        _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                    }
                }
                else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_FAST_REVERSE)
                {
                    _arPlayInfo[_u1SrcId].i4MMSpeed = 2000;
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_FAST_FORWARD;
                    _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                }
            }
        }
    }
    else if (u4Key == BTN_FR)
    {
        // check if this file can be FR or not first
        if (!_NAV_PLAYMGR_GetCap(_u1SrcId, PLAYMGR_CAP_SEEK, 0, 0))
        {
            //this file can't be seek
            _arPlayInfo[_u1SrcId].u4SeekTime = 0;
            _arPlayInfo[_u1SrcId].aucSeekKey[_arPlayInfo[_u1SrcId].ucSeekKeyCount] = 'x';
            _arPlayInfo[_u1SrcId].ucSeekKeyCount++;

            if (_arPlayInfo[_u1SrcId].ucSeekKeyCount>=3)
            {
                _ClearSeekKeyStatus(_u1SrcId);
            }
        }
        else
        {
            if ( _arPlayInfo[_u1SrcId].fgIsWma)
            {
                _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
            }
            if (( _arPlayInfo[_u1SrcId].fgIsWma && (rPlaymgrStatus.u2VideoTracks <= 0)) ||  _arPlayInfo[_u1SrcId].fgIsPlayError)
            {
                if ( _arPlayInfo[_u1SrcId].fgIsPlayError)
                {
                    Printf("FR is disable if playmgr notify error!\n");
                }
                else
                {
                    Printf("FR is disable for Wma or Wmv format file which has no video track!\n");
                }
            }
            else
            {
#ifdef TIME_SEEK_BY_KEY
                _ClearSeekKeyStatus(_u1SrcId);
#endif
                if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PLAY)
                {
                    _arPlayInfo[_u1SrcId].i4MMSpeed = -2000;
                    _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_FAST_REVERSE;
                }
                else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PAUSE)
                {
                    // Seek 5 second and step
                    PLAYMGR_STATUS_T rStatus;
                    UINT32 u4CurPos;
                    UINT64 u8CurBytesPos;
                    UINT64 u8Unit;
                    UINT64 u8Tmp;
                    UINT64 u8Remainder;
                    if (_NAV_PLAYMGR_GetInfo(_u1SrcId, &rStatus))
                    {
                        if ((rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_PS) ||
                                (rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_TS) ||
                                (rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_MPEG2_ES))
                        {
                            u8Tmp = (rStatus.u8FileSize*5000);
                            if (rStatus.u4TotalDuration)
                            {
                                u8Unit = u8Div6432(u8Tmp,
                                                   (UINT64)rStatus.u4TotalDuration,
                                                   &u8Remainder);
                            }
                            else
                            {
                                u8Unit = (2*1024*1024/8*5); // 5 seconds, 2M bps
                            }
                            if (rStatus.u8CurBytePos < u8Unit)
                            {
                                // send eos
                                u8CurBytesPos = rStatus.u8FileSize;
                            }
                            else
                            {
                                u8CurBytesPos = (rStatus.u8CurBytePos - u8Unit);
                            }
                            _NAV_PLAYMGR_SeekPos(_u1SrcId, u8CurBytesPos);
                        }
                        else
                        {
                            if (rStatus.u4CurPos < 5000)
                            {
                                // send eos
                                u4CurPos = rStatus.u4TotalDuration;
                            }
                            else
                            {
                                u4CurPos = (rStatus.u4CurPos - 5000);
                            }
                            _NAV_PLAYMGR_SeekTime(_u1SrcId, u4CurPos);
                        }

                    }
                    _arPlayInfo[_u1SrcId].i4MMSpeed = 1;
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PAUSE;
                    _NAV_PLAYMGR_Step(_u1SrcId, 1);
                }
                else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_FAST_FORWARD)
                {
                    _arPlayInfo[_u1SrcId].i4MMSpeed = -2000;
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_FAST_REVERSE;
                    _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                }
                else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_FAST_REVERSE)
                {
                    _arPlayInfo[_u1SrcId].i4MMSpeed *= 2;
                    if (_arPlayInfo[_u1SrcId].i4MMSpeed < -MAX_SPEED)
                    {
                        _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
                        _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
                        _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                    }
                    else
                    {
                        _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
                    }
                }
            }
        }
    }
    else if (u4Key == BTN_FAV_CH) // stop
    {
#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif
        _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_STOP;
        _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;

        _NAV_PLAYMGR_Stop(_u1SrcId);

        _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);

        if (_arPlayInfo[_u1SrcId].fgMMVidEn)
        {
            rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_VIDEO;
            rPlaymgrSetting.fgVidEnable = FALSE;
            rPlaymgrSetting.u2PlayVidTrack = 0;
            _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
            _arPlayInfo[_u1SrcId].fgMMVidEn = FALSE;
            _u1VDPEnableCnt--;
        }

        if (_arPlayInfo[_u1SrcId].fgMMAudEn)
        {
            rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
            rPlaymgrSetting.fgAudEnable = FALSE;
            rPlaymgrSetting.u2PlayAudTrack = 0;
            _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
            _arPlayInfo[_u1SrcId].fgMMAudEn = FALSE;
            _u1AUDEnableCnt--;
        }

        if (_arPlayInfo[_u1SrcId].fgMMPcrEn)
        {
            rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_PCR;
            rPlaymgrSetting.fgPcrEnable = FALSE;
            rPlaymgrSetting.u2PlayPcrTrack = 0;
            _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
            _arPlayInfo[_u1SrcId].fgMMPcrEn = FALSE;
        }
    }
#ifdef NAV_MM_MULTIPLE_FILE
    else if (u4Key == BTN_MENU)
    {
        WIDGET_MENU_T* prMenu;

        _fgMenuing = TRUE;

        prMenu = WIDGET_LookupMenu(arOSD[0].prList, "MEDIA");

        VERIFY(prMenu != NULL);

        while (WIDGET_RemoveLastMenuItem(prMenu) != E_WIDGET_MSG_NULL);

        WIDGET_AddMenuItem(prMenu, _prBrowseItem);

        //VERIFY(_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);

        // Should disable and re-enable IR handling
        WIDGET_ShowMenuList(arOSD[0].prList);

        DRV_OSD_SetRegionAlpha(arOSD[0].prList->prParentOSD->u4Region, 0xD0);

        NAV_InitHandlers();
    }
#endif
    else if (u4Key == BTN_EXIT)
    {
        WIDGET_MENU_T* prMenu;

        _fgMenuing = TRUE;

#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif
        if (_arPlayInfo[_u1SrcId].u4MMState != E_NAV_MM_STOP)
        {
            _NAV_PLAYMGR_Stop(_u1SrcId);

            _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);

            if (_arPlayInfo[_u1SrcId].fgMMVidEn)
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_VIDEO;
                rPlaymgrSetting.fgVidEnable = FALSE;
                rPlaymgrSetting.u2PlayVidTrack = 0;
                _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                _arPlayInfo[_u1SrcId].fgMMVidEn = FALSE;
                _u1VDPEnableCnt--;
            }

            if (_arPlayInfo[_u1SrcId].fgMMAudEn)
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
                rPlaymgrSetting.fgAudEnable = FALSE;
                rPlaymgrSetting.u2PlayAudTrack = 0;
                _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                _arPlayInfo[_u1SrcId].fgMMAudEn = FALSE;
                _u1AUDEnableCnt--;
            }

            if (_arPlayInfo[_u1SrcId].fgMMPcrEn)
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_PCR;
                rPlaymgrSetting.fgPcrEnable = FALSE;
                rPlaymgrSetting.u2PlayPcrTrack = 0;
                _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                _arPlayInfo[_u1SrcId].fgMMPcrEn = FALSE;
            }

            _arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack = 0;
            _arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack = 0;

        }

        _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_STOP;
        _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
        _arPlayInfo[_u1SrcId].fgMMAudEn = FALSE;
        _arPlayInfo[_u1SrcId].fgMMVidEn = FALSE;
        _arPlayInfo[_u1SrcId].fgMMPcrEn = FALSE;

        _arPlayInfo[_u1SrcId].fgMMEOS = FALSE;
        _arPlayInfo[_u1SrcId].u4MMTotalTime = 0;
        _arPlayInfo[_u1SrcId].u4MMCurTime = 0;

        _arPlayInfo[_u1SrcId].u4AudTrackNumber = 0;
        _arPlayInfo[_u1SrcId].i4CurrentAudTrack = 0;
        _arPlayInfo[_u1SrcId].u4VidTrackNumber = 0;
        _arPlayInfo[_u1SrcId].i4CurrentVidTrack = 0;
        _arPlayInfo[_u1SrcId].u4PcrTrackNumber = 0;
        _arPlayInfo[_u1SrcId].i4CurrentPcrTrack = 0;
        _arPlayInfo[_u1SrcId].u4PgmNumber = 0;
        _arPlayInfo[_u1SrcId].i4CurrentPgm = 0;

#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif

        CLI_Parser("ts.play.free");
        CLI_Parser("ts.play.stop");

        _NAV_PLAYMGR_Close(_u1SrcId);
        _afgSrcIsOccupied[_u1SrcId] = FALSE;

        prMenu = WIDGET_LookupMenu(arOSD[0].prList, "MEDIA");

        VERIFY(prMenu != NULL);

        while (WIDGET_RemoveLastMenuItem(prMenu) != E_WIDGET_MSG_NULL);

        WIDGET_AddMenuItem(prMenu, _prBrowseItem);

        if (_u1VDPEnableCnt == 0)
        {
            VERIFY(DMX_Init());
        }

#ifdef CC_MT5363
        // Enable Scart out
        DRV_TVE_CTRL_Enable(TRUE);
#endif

        NAV_SetSrc(0, _u4OrgInputSrc);

        //VERIFY(_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);

        // Should disable and re-enable IR handling
        WIDGET_ShowMenuList(arOSD[0].prList);

        DRV_OSD_SetRegionAlpha(arOSD[0].prList->prParentOSD->u4Region, 0xD0);

        NAV_InitHandlers();
    }
    else if (u4Key == BTN_VOL_UP || u4Key == BTN_VOL_DOWN)
    {
        INT32 i4Vol = (INT32)NAV_AQ_GetVol();
        if (u4Key == BTN_VOL_UP)
        {
            if ( (i4Vol + 1) <= 100)
            {
                VERIFY(NAV_AQ_SetVol((void*)(i4Vol + 1)) == (INT32)E_NAV_MSG_OK);
            }
        }
        if (u4Key == BTN_VOL_DOWN)
        {
            if ( (i4Vol - 1) >= 0)
            {
                VERIFY(NAV_AQ_SetVol((void*)(i4Vol - 1)) == (INT32)E_NAV_MSG_OK);
            }
        }
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    }
    else if (u4Key == BTN_MUTE)
    {
        VERIFY(NAV_AQ_SetMute(!NAV_AQ_GetMute()) == (INT32)E_NAV_MSG_OK);
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    }
    else if (u4Key == BTN_CURSOR_RIGHT)
    {
        if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PAUSE)
        {
            _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
            _NAV_PLAYMGR_Step(_u1SrcId, 1);
        }
    }
    else if (u4Key == BTN_CURSOR_LEFT)
    {
        if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PAUSE)
        {
            _arPlayInfo[_u1SrcId].i4MMSpeed = -1;
            _NAV_PLAYMGR_Step(_u1SrcId, -1);
        }
    }
    else if (u4Key == BTN_MTS)
    {
#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif
        _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
        if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_STOP)
        {
            if (rPlaymgrStatus.u2AudioTracks > 0)
            {
#ifdef NAV_MM_MULTIPLE_FILE
                if (_arPlayInfo[_u1SrcId].i4CurrentAudTrack == (rPlaymgrStatus.u2AudioTracks - 1))
                {
                    _arPlayInfo[_u1SrcId].i4CurrentAudTrack = -1;
                }
                else
#endif
                {
                    _arPlayInfo[_u1SrcId].i4CurrentAudTrack = (_arPlayInfo[_u1SrcId].i4CurrentAudTrack+1)%rPlaymgrStatus.u2AudioTracks;
                }
            }
        }
        else if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PLAY)
        {
            /* Change Audio Track */
            if (rPlaymgrStatus.u2AudioTracks > 0)
            {
                if (_arPlayInfo[_u1SrcId].fgMMAudEn)
                {
                    rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
                    rPlaymgrSetting.fgAudEnable = FALSE;
                    rPlaymgrSetting.u2PlayAudTrack = 0;
                    _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                    _arPlayInfo[_u1SrcId].fgMMAudEn = FALSE;
                    _u1AUDEnableCnt--;
                }

#ifdef NAV_MM_MULTIPLE_FILE
                if (_arPlayInfo[_u1SrcId].i4CurrentAudTrack == (rPlaymgrStatus.u2AudioTracks - 1))
                {
                    _arPlayInfo[_u1SrcId].i4CurrentAudTrack = -1;
                }
                else
#endif
                {
                    _arPlayInfo[_u1SrcId].i4CurrentAudTrack = (_arPlayInfo[_u1SrcId].i4CurrentAudTrack+1)%rPlaymgrStatus.u2AudioTracks;
                }
                if (_arPlayInfo[_u1SrcId].i4CurrentAudTrack >= 0)
                {
                    rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
                    rPlaymgrSetting.fgAudEnable = TRUE;
                    rPlaymgrSetting.u2PlayAudTrack = _arPlayInfo[_u1SrcId].i4CurrentAudTrack;
                    fgRet = _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                    if (fgRet)
                    {
                        _arPlayInfo[_u1SrcId].fgMMAudEn = TRUE;
                        if (_u1AUDEnableCnt == 0)
                        {
                            DRV_AUD_Unmute();
                        }
                        _u1AUDEnableCnt++;
                    }
                }
            }
        }
        else
        {
            printf("Please use in other stage!!\n");
        }

    }
    else if (u4Key == 0x8F003) // ANGLE button
    {
#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif
        _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
        if ( _arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_STOP)
        {
            //if(rPlaymgrStatus.u2VideoTracks > 1)
            {
#ifdef NAV_MM_MULTIPLE_FILE
                if (_arPlayInfo[_u1SrcId].i4CurrentVidTrack == (rPlaymgrStatus.u2VideoTracks - 1))
                {
                    _arPlayInfo[_u1SrcId].i4CurrentVidTrack = -1;
                }
                else
#endif
                {
                    _arPlayInfo[_u1SrcId].i4CurrentVidTrack = (_arPlayInfo[_u1SrcId].i4CurrentVidTrack+1)%rPlaymgrStatus.u2VideoTracks;
                }
            }
        }
        else if (_arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PLAY)
        {
            /* Change Video Track */
            if (rPlaymgrStatus.u2VideoTracks > 0)
            {
                if (_arPlayInfo[_u1SrcId].fgMMVidEn)
                {
                    rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_VIDEO;
                    rPlaymgrSetting.fgVidEnable = FALSE;
                    rPlaymgrSetting.u2PlayVidTrack = 0;
                    _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                    _arPlayInfo[_u1SrcId].fgMMVidEn = FALSE;
                    _u1VDPEnableCnt--;
                }

#ifdef NAV_MM_MULTIPLE_FILE
                if (_arPlayInfo[_u1SrcId].i4CurrentVidTrack == (rPlaymgrStatus.u2VideoTracks - 1))
                {
                    _arPlayInfo[_u1SrcId].i4CurrentVidTrack = -1;
                }
                else
#endif
                {
                    _arPlayInfo[_u1SrcId].i4CurrentVidTrack = (_arPlayInfo[_u1SrcId].i4CurrentVidTrack+1)%rPlaymgrStatus.u2VideoTracks;
                }
                if (_arPlayInfo[_u1SrcId].i4CurrentVidTrack >= 0)
                {
                    rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
                    rPlaymgrSetting.fgVidEnable = TRUE;
                    rPlaymgrSetting.u2PlayVidTrack = _arPlayInfo[_u1SrcId].i4CurrentVidTrack;
                    fgRet = _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                    if (fgRet)
                    {
                        _arPlayInfo[_u1SrcId].fgMMVidEn = TRUE;
                        if (_u1VDPEnableCnt == 0)
                        {
                            VDP_SetEnable(VDP_1, 1);
                            VDP_SetMode(VDP_1, VDP_MODE_NORMAL);
                            //VDP_SetInput(VDP_1, 0, 0);
                            VDP_SetInput(VDP_1, _u1SrcId, 0);
                        }
                        _u1VDPEnableCnt++;
                    }
                }
            }
        }

        else
        {
            printf("Please use in other stage!!\n");
        }
    }
    else if (u4Key == 0xAF01F) // TITLE button
    {
#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(_u1SrcId);
#endif
        if (_arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_STOP)
        {
            _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
            if (rPlaymgrStatus.u2TotalPrmNum > 1)
            {
                rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_PROGRAM;
                rPlaymgrSetting.u2PlayPgm = (_arPlayInfo[_u1SrcId].i4CurrentPgm+1)%rPlaymgrStatus.u2TotalPrmNum;
                fgRet = _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
                if (fgRet)
                {
                    _arPlayInfo[_u1SrcId].i4CurrentPgm = (_arPlayInfo[_u1SrcId].i4CurrentPgm+1)%rPlaymgrStatus.u2TotalPrmNum;
                }
            }
            _NAV_PLAYMGR_GetInfo(_u1SrcId, &rPlaymgrStatus);
            _arPlayInfo[_u1SrcId].u4PgmNumber = rPlaymgrStatus.u2TotalPrmNum;
            _arPlayInfo[_u1SrcId].u4VidTrackNumber = rPlaymgrStatus.u2VideoTracks;
            _arPlayInfo[_u1SrcId].u4AudTrackNumber = rPlaymgrStatus.u2AudioTracks;
            _arPlayInfo[_u1SrcId].u4MMTotalTime = rPlaymgrStatus.u4TotalDuration;
            _arPlayInfo[_u1SrcId].i4CurrentVidTrack = 0;
            _arPlayInfo[_u1SrcId].i4CurrentAudTrack = 0;
            _arPlayInfo[_u1SrcId].u4MMCurTime = 0;
        }
        else
        {
            printf("Please use on Stop!!\n");
        }
    }
#ifdef TIME_SEEK_BY_KEY
    else if ((u4Key>=BTN_DIGIT_0) && (u4Key<=BTN_DIGIT_9))
    {
        if (_arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PAUSE)
        {
            // check if this file can be seek or not first
            if (!_NAV_PLAYMGR_GetCap(_u1SrcId, PLAYMGR_CAP_SEEK, 0, 0))
            {
                //this file can't be seek
                _arPlayInfo[_u1SrcId].u4SeekTime = 0;
                _arPlayInfo[_u1SrcId].aucSeekKey[_arPlayInfo[_u1SrcId].ucSeekKeyCount] = 'x';
                _arPlayInfo[_u1SrcId].ucSeekKeyCount++;

                if (_arPlayInfo[_u1SrcId].ucSeekKeyCount>=3)
                {
                    _ClearSeekKeyStatus(_u1SrcId);
                }
            }
            else
            {
                if (_arPlayInfo[_u1SrcId].ucSeekKeyCount == 0)
                {
                    _arPlayInfo[_u1SrcId].u4SeekTime = 0;
                }
                _arPlayInfo[_u1SrcId].u4SeekTime = _arPlayInfo[_u1SrcId].u4SeekTime*10 + (u4Key-BTN_DIGIT_0);
                _arPlayInfo[_u1SrcId].aucSeekKey[_arPlayInfo[_u1SrcId].ucSeekKeyCount] = '0' + (u4Key-BTN_DIGIT_0);
                _arPlayInfo[_u1SrcId].ucSeekKeyCount++;

                if (_arPlayInfo[_u1SrcId].ucSeekKeyCount>=3)
                {
                    PLAYMGR_STATUS_T rStatus;
                    UINT32 u4CurPos;

                    /* Start Seek */
                    if (_NAV_PLAYMGR_GetInfo(_u1SrcId, &rStatus))
                    {
                        if ((_arPlayInfo[_u1SrcId].u4SeekTime*1000) > rStatus.u4TotalDuration)
                        {
                            u4CurPos = rStatus.u4TotalDuration;
                        }
                        else
                        {
                            u4CurPos = (_arPlayInfo[_u1SrcId].u4SeekTime*1000);
                        }
                        _NAV_PLAYMGR_SeekTime(_u1SrcId, u4CurPos);
                    }
                    _arPlayInfo[_u1SrcId].i4MMSpeed = 1;
                    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PAUSE;
                    _NAV_PLAYMGR_Step(_u1SrcId, 1);

                    _ClearSeekKeyStatus(_u1SrcId);
                }
            }
        }
    }
#endif
    else if (u4Key == BTN_RED)
    {
        if (_arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_PLAY)
        {
            _arPlayInfo[_u1SrcId].i4MMSpeed = 500;
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_SLOW_FORWARD;
            _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
            Printf("Slow motion!!!\n");
        }
        else if (_arPlayInfo[_u1SrcId].u4MMState == E_NAV_MM_SLOW_FORWARD)
        {
            _arPlayInfo[_u1SrcId].i4MMSpeed /= 2;
            if (_arPlayInfo[_u1SrcId].i4MMSpeed < 62)
            {
                /* Max slow rate is 1/16 */
                _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
                _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
                _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
            }
            else
            {
                _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
            }
        }
    }
    else if (u4Key == BTN_GREEN) // change focus
    {
        UINT8 u1NewSrcId;
        UINT8 u1Tmp;
        u1NewSrcId = (_u1ActiveSrcId + 1)%NAV_MM_MAX_SOURCE;
        for (u1Tmp = 0;u1Tmp < NAV_MM_MAX_SOURCE;u1Tmp++)
        {
            if (_afgSrcIsOccupied[u1NewSrcId])
            {
                _u1ActiveSrcId = u1NewSrcId;
                _u1SrcId = u1NewSrcId;
                Printf("Change to SrcId(%d)!!!\n", (UINT32)u1NewSrcId);
                break;
            }
            else
            {
                if (u1NewSrcId != _u1ActiveSrcId)
                {
                    Printf("SrcId(%d) is not open!!!\n", (UINT32)u1NewSrcId);
                }
            }
            u1NewSrcId = (u1NewSrcId + 1)%NAV_MM_MAX_SOURCE;
        }
    }
    else if (u4Key == 0xAF009) // Repeat key
    {
        _arPlayInfo[_u1SrcId].fgRepeat = _arPlayInfo[_u1SrcId].fgRepeat ? FALSE : TRUE;
    }
    else if (u4Key == BTN_OSD)
    {
        fgInfoOSDOn = !fgInfoOSDOn;
        if (fgInfoOSDOn)
        {
            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
            DRV_OSD_SetRegionAlpha(arOSD[1].u4Region, 0xD0);
        }
        else
        {
            DRV_OSD_SetRegionAlpha(arOSD[1].u4Region, 0x0);
            VERIFY(WIDGET_ClearScreen(&(arOSD[1])) == (INT32)E_WIDGET_MSG_OK);
            VERIFY(WIDGET_Flush(&(arOSD[1])) == (INT32)E_WIDGET_MSG_OK);
        }
        return TRUE;
    }
    else if (u4Key == BTN_YELLOW)
    {
        _arPlayInfo[_u1SrcId].u4ABRepeat = (_arPlayInfo[_u1SrcId].u4ABRepeat + 1) % 3;
        
        if (_arPlayInfo[_u1SrcId].u4ABRepeat == 1)
        {
            _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
            _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_PLAY;
            _NAV_PLAYMGR_Speed(_u1SrcId, _arPlayInfo[_u1SrcId].i4MMSpeed);
        }
        
        _NAV_PLAYMGR_ABRepeat(_u1SrcId, _arPlayInfo[_u1SrcId].u4ABRepeat);
    }
    else
    {
        return FALSE;
    }
    NAV_DrawInfoOSD();
    return TRUE;
}


VOID _MM_PlaymgrCallback(UINT8 u1SrcId,
                         PLAYMGR_CALLBACK_TYPE_T eCallbackType,
                         PLAYMGR_CALLBACKPARAM_T *prParams,
                         UINT32 u4Param)
{
    if (u1SrcId >= NAV_MM_MAX_SOURCE)
    {
        printf("_MM_PlaymgrCallback u1SrcId %d fail\n", u1SrcId);
        return;
    }
    if (eCallbackType == PLAYMGR_CALLBACK_ELAPSEDTIME)
    {
        PLAYMGR_STATUS_T rStatus;
        x_memset(&rStatus, 0, sizeof(rStatus));
        _NAV_PLAYMGR_GetInfo(u1SrcId, &rStatus);
        _arPlayInfo[u1SrcId].u4MMTotalTime = rStatus.u4TotalDuration;
        _arPlayInfo[u1SrcId].u4MMCurTime = rStatus.u4CurPos;
        if (_arPlayInfo[u1SrcId].fgMMEOS)
        {
            PLAYMGR_SETTING_T rPlaymgrSetting;
            x_memset(&rPlaymgrSetting, 0, sizeof(rPlaymgrSetting));
            if (_arPlayInfo[u1SrcId].fgRepeat)
            {
                _arPlayInfo[u1SrcId].u4MMState = E_NAV_MM_STOP;
                _NAV_PLAYMGR_Stop(u1SrcId);

                _arPlayInfo[u1SrcId].u4MMState = E_NAV_MM_PLAY;
                _arPlayInfo[u1SrcId].i4MMSpeed = 1000;
                _arPlayInfo[u1SrcId].fgMMEOS = FALSE;
                _NAV_PLAYMGR_Speed(u1SrcId, _arPlayInfo[u1SrcId].i4MMSpeed);
                _NAV_PLAYMGR_Start(u1SrcId);
            }
            else
            {
                if (_arPlayInfo[u1SrcId].u4MMState != E_NAV_MM_STOP)
                {
                    _NAV_PLAYMGR_Stop(u1SrcId);

                    if (_arPlayInfo[u1SrcId].fgMMVidEn)
                    {
                        rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_VIDEO;
                        rPlaymgrSetting.fgVidEnable = FALSE;
                        rPlaymgrSetting.u2PlayVidTrack = 0;
                        _NAV_PLAYMGR_Set(u1SrcId, &rPlaymgrSetting);
                        _arPlayInfo[u1SrcId].fgMMVidEn = FALSE;
                        _u1VDPEnableCnt--;
                    }

                    if (_arPlayInfo[u1SrcId].fgMMAudEn)
                    {
                        rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
                        rPlaymgrSetting.fgAudEnable = FALSE;
                        rPlaymgrSetting.u2PlayAudTrack = 0;
                        _NAV_PLAYMGR_Set(u1SrcId, &rPlaymgrSetting);
                        _arPlayInfo[u1SrcId].fgMMAudEn = FALSE;
                        _u1AUDEnableCnt--;
                    }

                    if (_arPlayInfo[u1SrcId].fgMMPcrEn)
                    {
                        rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_PCR;
                        rPlaymgrSetting.fgPcrEnable = FALSE;
                        rPlaymgrSetting.u2PlayPcrTrack = 0;
                        _NAV_PLAYMGR_Set(u1SrcId, &rPlaymgrSetting);
                        _arPlayInfo[u1SrcId].fgMMPcrEn = FALSE;
                    }

                }

                _arPlayInfo[u1SrcId].u4MMState = E_NAV_MM_STOP;
                _arPlayInfo[u1SrcId].i4MMSpeed = 1000;
                _arPlayInfo[u1SrcId].fgMMAudEn = FALSE;
                _arPlayInfo[u1SrcId].fgMMVidEn = FALSE;
                _arPlayInfo[u1SrcId].fgMMPcrEn = FALSE;
                _arPlayInfo[u1SrcId].fgMMEOS = FALSE;
            }
        }
        if (!_fgMenuing)
        {
            NAV_DrawInfoOSD();
        }
    }
    else if (eCallbackType == PLAYMGR_CALLBACK_FINISH)
    {
        _arPlayInfo[u1SrcId].fgMMEOS = TRUE;
    }
    /*DTV00138388*/
    else if (eCallbackType ==  PLAYMGR_CALLBACK_ERROR)
    {
        _arPlayInfo[_u1SrcId].fgIsPlayError = TRUE;
    }
}

#ifdef CC_SUPPORT_PVR
static BOOL _PVR_IRHandler(WIDGET_OSDINFO_T arOSD[], UINT32 u4Key, VOID* pvArg)
{
    if (u4Key == BTN_FAVORITE) // play
    {
        if (_u4MMState == E_NAV_MM_STOP)
        {
            _u4MMState = E_NAV_MM_PLAY;

            //TODO: enable PVR here
            Printf("PVR enable\n");

            CLI_Parser("ts.ts.e");
        }
        else if (_u4MMState == E_NAV_MM_PAUSE)
        {
            _u4MMState = E_NAV_MM_PLAY;

            //TODO: PVR resume function
            Printf("PVR resume\n");

            CLI_Parser("ts.ts.resume");
        }
        else
        {
            _u4MMState = E_NAV_MM_PAUSE;

            //TODO: PVR pause function
            Printf("PVR pause\n");

            CLI_Parser("ts.ts.pause");
        }

    }
#if 0
    else if (u4Key == BTN_FF)
    {
        _u4MMState = E_NAV_MM_FAST_FORWARD;
    }
    else if (u4Key == BTN_FR)
    {
        _u4MMState = E_NAV_MM_FAST_REVERSE;
    }
    else if (u4Key == BTN_FAV_CH) // stop
    {
        _u4MMState = E_NAV_MM_STOP;

        PLAYMGR_Stop();
        //AUD_DSPCmdStop(0);
        //VDEC_Stop(0);

    }
#endif
    else if (u4Key == BTN_EXIT)
    {
        WIDGET_MENU_T* prMenu;

        _u4MMState = E_NAV_MM_STOP;

        //TODO: disable PVR here
        Printf("PVR disable\n");

        //PLAYMGR_Close();
        CLI_Parser("ts.ts.d");

        // reset tick value
        _u4PvrTickStart = 0;
        _u4PvrCurrentTick = 0;
        _u4PvrTickEnd = 0;

        // Should disable and re-enable IR handling
        WIDGET_ShowMenuList(arOSD[0].prList);

        DRV_OSD_SetRegionAlpha(arOSD[0].prList->prParentOSD->u4Region, 0xD0);
        //HACK: main path source for PVR!!
        arVideoPath[0].u4Src = E_NAV_VS_DT1;

        NAV_InitHandlers();
    }
    else
    {
        return FALSE;
    }
    NAV_DrawInfoOSD();
    return TRUE;
}

static VOID _PVR_Callback(UINT32 u4TickStart, UINT32 u4CurrentTick, UINT32 u4TickEnd)
{
    // assign new tick value
    _u4PvrTickStart = (u4TickStart) ? (u4TickStart % 1000) : 0;
    _u4PvrCurrentTick = (u4CurrentTick) ? (u4CurrentTick % 1000) : 0;
    _u4PvrTickEnd = (u4TickEnd) ? (u4TickEnd % 1000) : 0;
    // update tick in OSD
    NAV_DrawInfoOSD();
}
#endif

static BOOL _VIEWER_IRHandler(WIDGET_OSDINFO_T arOSD[], UINT32 u4Key, VOID* pvArg)
{
    INT32 i4Ret = 0;
    if (u4Key == BTN_EXIT) // play
    {
        WIDGET_MENU_T* prMenu;

        prMenu = WIDGET_LookupMenu(arOSD[0].prList, "MEDIA");

        VERIFY(prMenu != NULL);

        while (WIDGET_RemoveLastMenuItem(prMenu) != E_WIDGET_MSG_NULL);

        WIDGET_AddMenuItem(prMenu, _prBrowseItem);

        NAV_VIEWER_Stop();

        NAV_SetSrc(0, _u4OrgInputSrc);

        // Should disable and re-enable IR handling
        x_thread_delay(500);
        WIDGET_ShowMenuList(arOSD[0].prList);

        DRV_OSD_SetRegionAlpha(arOSD[0].prList->prParentOSD->u4Region, 0xD0);

        NAV_InitBorder();

        NAV_InitHandlers();
    }
    else if (u4Key == BTN_CURSOR_DOWN || u4Key == BTN_CURSOR_RIGHT || u4Key == BTN_FF || u4Key == BTN_NEXT)
    {
        WIDGET_MENU_T* prMenu;
        NAV_VIEWER_Stop();

        prMenu = WIDGET_LookupMenu(_prList, "MEDIA");

        VERIFY(prMenu != NULL);
        VERIFY(prMenu->prCurr != NULL);
        VERIFY(prMenu->prCurr->prNext != NULL);

        prMenu->prCurr = prMenu->prCurr->prNext;

        x_strcpy(_szCurrFile, _szMountPath);
        x_strcat(_szCurrFile, "/");
        x_strcat(_szCurrFile, prMenu->prCurr->szTitle);

        i4Ret = NAV_VIEWER_Start(_szCurrFile);
        NAV_MM_ViewerUI(&arWidgetOSD[1]);

    }
    else if (u4Key == BTN_CURSOR_UP || u4Key == BTN_CURSOR_LEFT || u4Key == BTN_FR || u4Key == BTN_PREV)
    {
        WIDGET_MENU_T* prMenu;
        NAV_VIEWER_Stop();

        prMenu = WIDGET_LookupMenu(_prList, "MEDIA");

        VERIFY(prMenu != NULL);
        VERIFY(prMenu->prCurr != NULL);
        VERIFY(prMenu->prCurr->prPrev != NULL);

        prMenu->prCurr = prMenu->prCurr->prPrev;

        x_strcpy(_szCurrFile, _szMountPath);
        x_strcat(_szCurrFile, "/");
        x_strcat(_szCurrFile, prMenu->prCurr->szTitle);

        i4Ret = NAV_VIEWER_Start(_szCurrFile);

    }
    else
    {
        return FALSE;
    }

    if (i4Ret == 0 && u4Key != BTN_EXIT)
    {
        NAV_MM_ViewerUI(&arWidgetOSD[1]);
    }
    else
    {
        if (u4Key != BTN_EXIT)
        {
            WIDGET_MENU_T* prMenu;

            prMenu = WIDGET_LookupMenu(arOSD[0].prList, "MEDIA");

            VERIFY(prMenu != NULL);

            while (WIDGET_RemoveLastMenuItem(prMenu) != E_WIDGET_MSG_NULL);

            WIDGET_AddMenuItem(prMenu, _prBrowseItem);

            NAV_VIEWER_Stop();

            NAV_SetSrc(0, _u4OrgInputSrc);

            // Should disable and re-enable IR handling
            x_thread_delay(500);
            WIDGET_ShowMenuList(arOSD[0].prList);

            DRV_OSD_SetRegionAlpha(arOSD[0].prList->prParentOSD->u4Region, 0xD0);

            NAV_InitBorder();

            NAV_InitHandlers();
        }
    }

    return TRUE;
}

static INT32 _SelectFile(void* pvArg)
{
    BOOL fgOpen = TRUE;
    WIDGET_MENU_T* prMenu;
    PLAYMGR_SETTING_T rPlaymgrSetting;
    PLAYMGR_STATUS_T rStatus;
    INT32 i4Ret = E_NAV_MSG_OK;
    UINT32 u4Tmp;
    if ((ePIPMode==E_NAV_MODE_POP)||(ePIPMode ==E_NAV_MODE_PIP))
    {
        return (INT32)E_NAV_MSG_OK;
    }

    _u4OrgInputSrc = arVideoPath[0].u4Src;
    _fgMenuing = FALSE;

    for (u4Tmp = 0;u4Tmp < NAV_MM_MAX_SOURCE;u4Tmp++)
    {
        if (_afgSrcIsOccupied[u4Tmp])
        {
            break;
        }
    }
    // no mm source is openned.
    if (u4Tmp >= NAV_MM_MAX_SOURCE)
    {
        NAV_SetSrc(0, E_NAV_VS_DT2);
    }

    DRV_OSD_SetRegionAlpha(_prList->prParentOSD->u4Region, 0x0);   // hide menu

    NAV_VIEWER_Init();

    prMenu = WIDGET_LookupMenu(_prList, "MEDIA");

    VERIFY(prMenu != NULL);
    VERIFY(prMenu->prCurr != NULL);

    x_strcpy(_szCurrFile, _szMountPath);
    x_strcat(_szCurrFile, "/");
    x_strcat(_szCurrFile, prMenu->prCurr->szTitle);

    // disable original IR event handler
    WIDGET_SetIRHandler1KeyAction(NULL);
    WIDGET_SetIRHandler2KeyAction(NULL);
    WIDGET_SetIRHandler3KeyAction(NULL);
    WIDGET_SetHandler(NULL);

    NAV_ReleaseBorder();
	if(_IsSupportedImgExtension(_FindExtension(prMenu->prCurr->szTitle)))
    {
        // Should disable and re-enable IR handling
        _u4MMMode = E_NAV_MM_MODE_VIEWER;

        WIDGET_SetIRHandler1KeyAction(_VIEWER_IRHandler);
        WIDGET_SetIRHandler2KeyAction(NULL);
        WIDGET_SetIRHandler3KeyAction(NULL);
        WIDGET_SetHandler(NULL);

        NAV_VIEWER_Start(_szCurrFile);
    }
    else
    {
        // Should disable and re-enable IR handling
        _u4MMMode = E_NAV_MM_MODE_PLAYER;

#ifdef CC_SUPPORT_PVR
        if (x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "pvr") == 0)
        {
            UINT32 i, flen;
            CHAR aszBuf[256];

            CLI_Parser("d.pvr on");
            CLI_Parser("pvr.i");
            CLI_Parser("ts.i");
            //CLI_Parser("ts.bm.sbuf file");
            //CLI_Parser("ts.p.avf /mnt/usb_0/pvrtest1.met");
            flen = x_strlen(_szCurrFile);
            x_memset((void*)aszBuf, 0, 256);
            for (i=0; i<flen; i++)
            {
                if (_szCurrFile[i] == '.')
                {
                    break;
                }
                aszBuf[i] = _szCurrFile[i];
            }
            aszBuf[i] = 0;

            CLI_Parser_Arg("ts.p.avf %s.met", aszBuf);
            x_strcpy(aszBuf, "ts.p.start ");
            x_strcat(aszBuf, _szCurrFile);
            CLI_Parser(aszBuf);
        }
#endif

        for (u4Tmp = 0; u4Tmp < NAV_MM_MAX_SOURCE; u4Tmp++)
        {
            if (!_afgSrcIsOccupied[u4Tmp])
            {
                break;
            }
        }
        if (u4Tmp < NAV_MM_MAX_SOURCE)
        {
            _u1SrcId = u4Tmp;
            _u1ActiveSrcId = u4Tmp;
            _afgSrcIsOccupied[u4Tmp] = TRUE;
        }
        /*
        if (!_afgSrcIsOccupied[0])
        {
            _u1SrcId = 0;
            _u1ActiveSrcId = 0;
            _afgSrcIsOccupied[0] = TRUE;
        }
        else if(!_afgSrcIsOccupied[1])
        {
            _u1SrcId = 1;
            _u1ActiveSrcId = 1;
            _afgSrcIsOccupied[1] = TRUE;
        }*/
        else
        {
            fgOpen = FALSE;
        }

        /*DTV00138398*/
        if ( x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "wma") == 0 ||
                x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "WMA") == 0 ||
                x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "wmv") == 0 ||
                x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "WMV") == 0)
        {
            _arPlayInfo[_u1SrcId].fgIsWma = TRUE;
        }
        else
        {
            _arPlayInfo[_u1SrcId].fgIsWma = FALSE;
        }
        _arPlayInfo[_u1SrcId].fgIsPlayError = FALSE;
        if (fgOpen)
        {
            if (!_NAV_PLAYMGR_Open(_u1SrcId, (UCHAR*)_szCurrFile))
            {
                printf("_NAV_PLAYMGR_Open fail\n");
                i4Ret = E_NAV_MSG_NOT_SUPPORT;
                goto ERROR_HANDLER;
            }
            if (!_NAV_PLAYMGR_GetInfo(_u1SrcId, &rStatus))
            {
                printf("_NAV_PLAYMGR_GetInfo fail\n");
                i4Ret = E_NAV_MSG_NOT_SUPPORT;
                goto ERROR_HANDLER;
            }
            if (rStatus.eMediaType == PLAYMGR_MEDIA_TYPE_UNKNOWN)
            {
                printf("PLAYMGR_MEDIA_TYPE_UNKNOWN\n");
                i4Ret = E_NAV_MSG_NOT_SUPPORT;
                goto ERROR_HANDLER;
            }

            _NAV_PLAYMGR_RegCb(_u1SrcId, PLAYMGR_CALLBACK_ELAPSEDTIME, _NAV_MM_PlaymgrCallback, 0);
            _NAV_PLAYMGR_RegCb(_u1SrcId, PLAYMGR_CALLBACK_FINISH, _NAV_MM_PlaymgrCallback, 0);
            /*DTV00138388*/
            _NAV_PLAYMGR_RegCb(_u1SrcId, PLAYMGR_CALLBACK_ERROR, _NAV_MM_PlaymgrCallback, 0);

            _arPlayInfo[_u1SrcId].u4PgmNumber = rStatus.u2TotalPrmNum;
            _arPlayInfo[_u1SrcId].i4CurrentPgm = rStatus.u2EnPgmNum;
            _arPlayInfo[_u1SrcId].u4VidTrackNumber = rStatus.u2VideoTracks;
            _arPlayInfo[_u1SrcId].u4AudTrackNumber = rStatus.u2AudioTracks;
            _arPlayInfo[_u1SrcId].u4MMTotalTime = rStatus.u4TotalDuration;
            _arPlayInfo[_u1SrcId].u4MMCurTime = 0;

            printf("Media Type: %s\n", _aszMediaType2Str[rStatus.eMediaType]);
            printf("Video Format: %s\n", _aszVidFormat2Str[rStatus.aeVidFormat[0]]);
            printf("Audio Format: %s\n", _aszAudFormat2Str[rStatus.aeAudFormat[0]]);
            printf("Caption Format: %s\n", _aszCapFormat2Str[rStatus.aeCapFormat[0]]);
            printf("Program Number: %d\n", rStatus.u2TotalPrmNum);
            printf("Video Tracks: %d\n", rStatus.u2VideoTracks);
            printf("Audio Tracks: %d\n", rStatus.u2AudioTracks);
            printf("Caption Tracks: %d\n", rStatus.u2CaptionTracks);
            printf("Curr Position: %d\n", rStatus.u4CurPos);
            printf("Play Capabilities: %d\n", rStatus.u4PlayCap);
            printf("Total Duration: %d\n", rStatus.u4TotalDuration);
            printf("File Size: %d\n", rStatus.u8FileSize);
        }
        else
        {
            _u1ActiveSrcId = (_u1ActiveSrcId + 1)%NAV_MM_MAX_SOURCE;
            _u1SrcId = _u1ActiveSrcId;
        }

        WIDGET_SetIRHandler1KeyAction(_MM_IRHandler);
        NAV_DrawInfoOSD();
    }

#ifdef CC_MT5363
    // Disable Scart out
    DRV_TVE_CTRL_Enable(FALSE);
#endif
    CLI_Parser("n.ms dtv1");

    return i4Ret;

ERROR_HANDLER:
    if (_arPlayInfo[_u1SrcId].u4MMState != E_NAV_MM_STOP)
    {
        _NAV_PLAYMGR_Stop(_u1SrcId);

        _NAV_PLAYMGR_GetInfo(_u1SrcId, &rStatus);

        if (_arPlayInfo[_u1SrcId].fgMMVidEn)
        {
            rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_VIDEO;
            rPlaymgrSetting.fgVidEnable = FALSE;
            rPlaymgrSetting.u2PlayVidTrack = 0;
            _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
            _arPlayInfo[_u1SrcId].fgMMVidEn = FALSE;
            _u1VDPEnableCnt--;
        }

        if (_arPlayInfo[_u1SrcId].fgMMAudEn)
        {
            rPlaymgrSetting.u4InfoMask = PLAYMGR_INFO_MASK_AUDIO;
            rPlaymgrSetting.fgAudEnable = FALSE;
            rPlaymgrSetting.u2PlayAudTrack = 0;
            _NAV_PLAYMGR_Set(_u1SrcId, &rPlaymgrSetting);
            _arPlayInfo[_u1SrcId].fgMMAudEn = FALSE;
            _u1AUDEnableCnt--;
        }
    }

    _arPlayInfo[_u1SrcId].u4MMState = E_NAV_MM_STOP;
    _arPlayInfo[_u1SrcId].i4MMSpeed = 1000;
    _arPlayInfo[_u1SrcId].fgMMAudEn = FALSE;
    _arPlayInfo[_u1SrcId].fgMMVidEn = FALSE;

    CLI_Parser("ts.play.free");
    CLI_Parser("ts.play.stop");

    _NAV_PLAYMGR_Close(_u1SrcId);
    _afgSrcIsOccupied[_u1SrcId] = FALSE;

    prMenu = WIDGET_LookupMenu(_prList, "MEDIA");

    VERIFY(prMenu != NULL);

    while (WIDGET_RemoveLastMenuItem(prMenu) != E_WIDGET_MSG_NULL);

    WIDGET_AddMenuItem(prMenu, _prBrowseItem);

    NAV_SetSrc(0, _u4OrgInputSrc);

    // Should disable and re-enable IR handling
    WIDGET_ShowMenuList(_prList);

    DRV_OSD_SetRegionAlpha(_prList->prParentOSD->u4Region, 0xD0);

    NAV_InitHandlers();

    return i4Ret;

}

static INT32 _Mount(void)
{
    INT32 i4_ret;
    UINT32 ui4_part_ns;

    UINT32 ui4_cnt;

    /*  1. dev point creation */

    i4_ret = x_fm_create_dir(FM_ROOT_HANDLE, "/dev", 0666);

    if (i4_ret == FMR_OK)
    {
        printf("/dev is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        if (i4_ret == FMR_EXIST)
        {
            printf("/dev is already existed.  That is OK.\n");
        }
        else
        {
            printf("make /dev fail\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

    /*  2. mount point creation */

    i4_ret = x_fm_create_dir(FM_ROOT_HANDLE, "/mnt", 0666);

    if (i4_ret == FMR_OK)
    {
        printf("/mnt is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        if (i4_ret == FMR_EXIST)
        {
            printf("/mnt is already existed.  That is OK.\n");
        }
        else
        {
            printf("make /mnt fail\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

    /*  3. make entry for USB */

    i4_ret = x_fm_make_entry(FM_ROOT_HANDLE,
                             "/dev/usb",
                             FM_MODE_TYPE_BLK | FM_MODE_DEV_TRUE | 0660,
                             FM_DRVT_USB_MASS_STORAGE,
                             0);

    if (i4_ret == FMR_OK)
    {
        printf("/dev/usb is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        if (i4_ret == FMR_EXIST)
        {
            printf("/dev/usb is already existed.  That is OK.\n");
        }
        else
        {
            printf("make /dev/usb fail\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

    /*  4. parse drive  */

    i4_ret = x_fm_parse_drive(FM_ROOT_HANDLE,
                              "/dev/usb",
                              (VOID *) NULL);

    if (i4_ret != FMR_OK)
    {
        printf("parse drive error.");
    }

    /*  5. get partition ns */

    i4_ret = x_fm_get_part_ns(FM_ROOT_HANDLE,
                              (const CHAR *) "/dev/usb",
                              &ui4_part_ns);

    if (i4_ret != FMR_OK)
    {
        printf("x_fm_get_part_ns fails.\n");
        printf("return value:%d\n", i4_ret);
        goto func_exit;
    }

    for (ui4_cnt = 0; ui4_cnt < ui4_part_ns; ui4_cnt++)
    {
        FM_PART_INFO_T t_part_info;

        CHAR ps_dev_name[32];
        CHAR ps_mnt_name[32];

        i4_ret = x_fm_get_part_info(FM_ROOT_HANDLE,
                                    (const CHAR *) "/dev/usb",
                                    ui4_cnt,
                                    &t_part_info);

        if (i4_ret != FMR_OK)
        {
            printf("x_fm_get_part_info fails.\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }

        x_strcpy((CHAR *) ps_dev_name, "/dev/");
        x_strcpy((CHAR *) ps_mnt_name, "/mnt/");

        x_strcat((CHAR *) ps_dev_name, (const CHAR *) t_part_info.ps_part_name);
        x_strcat((CHAR *) ps_mnt_name, (const CHAR *) t_part_info.ps_part_name);

        i4_ret = x_fm_create_dir(FM_ROOT_HANDLE, ps_mnt_name, 0666);

        if (i4_ret == FMR_OK)
        {
            printf("%s is created.\n", ps_mnt_name);
        }
        else if (i4_ret != FMR_OK)
        {
            if (i4_ret == FMR_EXIST)
            {
                printf("%s is already existed.  That is OK.\n", ps_mnt_name);
            }
            else
            {
                printf("make %s fail.\n", ps_mnt_name);
                printf("return value:%d\n", i4_ret);
                goto func_exit;
            }
        }

        i4_ret = x_fm_mount(FM_ROOT_HANDLE, ps_dev_name, FM_ROOT_HANDLE, ps_mnt_name);

        if (i4_ret == FMR_OK)
        {
            x_strncpy(_szMountPath, ps_mnt_name, x_strlen(ps_mnt_name));
            x_strncpy(_szDevPath, ps_dev_name, x_strlen(ps_dev_name));
            printf("%s is mounted as %s.\n", ps_dev_name, ps_mnt_name);
        }
        if (i4_ret != FMR_OK)
        {
            printf("ERR: mount %s as %s fail.\n", ps_dev_name, ps_mnt_name);
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

func_exit:

    return 0;
}

static INT32 _Umount(void)
{
    INT32 i4_ret;
    HANDLE_T h_dir;

    i4_ret = x_fm_umount(FM_ROOT_HANDLE, _szMountPath);

    if (i4_ret == FMR_OK)
    {
        printf("%s is unmounted.\n", _szMountPath);
        x_memset(_szMountPath, 0, MAX_LENGTH_FILE_NAME);
    }
    if (i4_ret != FMR_OK)
    {
        printf("ERR: unmount %s fail.\n", _szMountPath);
        printf("return value:%d\n", i4_ret);
        goto func_exit;
    }

    i4_ret = x_fm_set_dir_path(FM_ROOT_HANDLE, "/dev", &h_dir);

    if (i4_ret == FMR_OK)
    {
        printf("/dev is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        printf("/dev is not created.\n");
    }

    i4_ret = x_fm_release_drive(h_dir, "usb");

    if (i4_ret == FMR_OK)
    {
        printf("/dev/usb is released.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        printf("Release drive error.\n");
    }

    x_fm_close(h_dir);

#if 0
    i4_ret = x_fm_delete_entry(FM_ROOT_HANDLE,
                               "/dev/usb",
                               FM_MODE_TYPE_BLK | FM_MODE_DEV_TRUE | 0660);

    if (i4_ret == FMR_OK)
    {
        printf("/dev/usb is deleted.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        printf("delete /dev/usb fail\n");
        printf("return value:%d\n", i4_ret);
        goto func_exit;
    }
#endif

func_exit:

    return i4_ret;
}

static VOID  _MusbCallback(UINT32 u4Tag, INT32 i4Status)
{
    UINT32 u4Msg;

    if (i4Status == BLKDEV_STATUS_READY)
    {
        u4Msg = i4Status;
        x_msg_q_send(_hMonitorMsgq, &u4Msg, sizeof(UINT32), 200);
    }
    else if (i4Status == BLKDEV_STATUS_ABSENT)
    {
        u4Msg = i4Status;
        x_msg_q_send(_hMonitorMsgq, &u4Msg, sizeof(UINT32), 200);
    }
    return;
}

static VOID _MonitorThread(VOID* pvArg)
{
    UINT16 u2Idx;
    UINT32 u4Msg;
    SIZE_T rSize = sizeof(UINT32);
    while (1)
    {
        x_msg_q_receive(&u2Idx, &u4Msg, &rSize, &_hMonitorMsgq, 1, X_MSGQ_OPTION_WAIT);
        if (u4Msg == BLKDEV_STATUS_READY)
        {
            VERIFY(_Mount() == 0);
        }
        else if (u4Msg == BLKDEV_STATUS_ABSENT)
        {
            // Send virtual EXIT key event when MM is playing before umount USB disk, or system crash
            WIDGET_EVENT_T rEvent;
            rEvent.rIrEvent.rType = DRV_IR_EVENT_DOWN;
            rEvent.rIrEvent.u4Key = BTN_EXIT;
            WIDGET_SendExternalEvent(rEvent);
            rEvent.rIrEvent.rType = DRV_IR_EVENT_UP;
            WIDGET_SendExternalEvent(rEvent);
            x_thread_delay(2000);

            VERIFY(_Umount() == 0);
        }
    }
}

INT32 NAV_MM_GetMode(void)
{
    return _u4MMMode;
}

UINT16* NAV_MM_GetIcon(UINT32 u4State)
{
    switch (u4State)
    {
    case E_NAV_MM_PLAY:
        return _au2PlayIcon;
    case E_NAV_MM_STOP:
        return _au2StopIcon;
    case E_NAV_MM_PAUSE:
        return _au2PauseIcon;
    case E_NAV_MM_FAST_FORWARD:
        return _au2ForwardIcon;
    case E_NAV_MM_FAST_REVERSE:
        return _au2ReverseIcon;
    }
    return NULL;
}

INT32 NAV_MM_GetState(void)
{
    return _arPlayInfo[_u1ActiveSrcId].u4MMState;
}

INT32 NAV_MM_Browse(void* pvArg)
{
    WIDGET_MENU_T *prMenu;
    WIDGET_MENUITEM_T *prItem;
    UINT32 i = 0;
    INT32 i4NumOfFiles;

    // Read USB file list
    i4NumOfFiles = _FileList();

    if (i4NumOfFiles <= 0)
    {
        return (INT32)E_NAV_MSG_OK;
    }

    _prList = arWidgetOSD[0].prList;

    VERIFY(_prList != NULL);

    prMenu = WIDGET_LookupMenu(_prList, "MEDIA");

    VERIFY(prMenu != NULL);

    _prBrowseItem = WIDGET_LookupMenuItem(_prList, "MEDIA", "Browse");

    VERIFY(_prBrowseItem != NULL);

    VERIFY(WIDGET_RemoveLastMenuItem(prMenu) == E_NAV_MSG_OK);

    for (i = 0; i < i4NumOfFiles; i++)
    {
        prItem = (WIDGET_MENUITEM_T*)WIDGET_NewMenuItem_Action(_arFileList[i].szFileName, CM_ARGB4444_DIRECT16, NULL);

        VERIFY(WIDGET_SetAction(
                   prItem,
                   NAV_IRHandler_MenuItem_Action,
                   _SelectFile,
                   (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemAction,
                   (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemAction)
               == (INT32)E_NAV_MSG_OK);

        WIDGET_AddMenuItem(prMenu, prItem);
    }

    WIDGET_ShowMenuList(_prList);

    return (INT32)E_NAV_MSG_OK;
}

#ifdef CC_SUPPORT_PVR
// Enter PVR mode
INT32 NAV_MM_PVR_Mode(void* pvArg)
{
    WIDGET_MENU_T* prMenu;

    _prList = arWidgetOSD[0].prList;

    // check input source path
    if ((arVideoPath[0].u4Src != E_NAV_VS_DT1) ||
            ((arVideoPath[0].u4ResX * arVideoPath[0].u4ResY) >= (1280*720)))
    {
        Printf("PVR is supported only when input source is DTV SD\n");
        return E_NAV_MSG_OK;
    }

    //NAV_SetSrc(0, E_NAV_VS_DT2);

    //HACK: main path source for PVR!!
    arVideoPath[0].u4Src = E_NAV_VS_DT2;

    DRV_OSD_SetRegionAlpha(_prList->prParentOSD->u4Region, 0x0);   // hide menu

    // disable original IR event handler
    WIDGET_SetIRHandler1KeyAction(NULL);
    WIDGET_SetIRHandler2KeyAction(NULL);
    WIDGET_SetIRHandler3KeyAction(NULL);
    WIDGET_SetHandler(NULL);
    NAV_ReleaseBorder();
#if 0
    if ( x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "jpg") == 0 ||
            x_strcmp(_FindExtension(prMenu->prCurr->szTitle), "JPG") == 0)
    {

        // Should disable and re-enable IR handling
        _u4MMMode = E_NAV_MM_MODE_VIEWER;

        WIDGET_SetIRHandler1KeyAction(_VIEWER_IRHandler);
        NAV_VIEWER_Start(_szCurrFile);
    }
    else
#endif
    {
        //PLAYMGR_STATUS_T rStatus;

        // Should disable and re-enable IR handling
        _u4MMMode = E_NAV_MM_MODE_PVR;

        //TODO: enable PVR here
        CLI_Parser("pvr.i");
        CLI_Parser("ts.i");
        // PVR dram size
        CLI_Parser("ts.ts.open dram 0x1000000");

        _u4PvrTickStart = 0;
        _u4PvrCurrentTick = 0;
        _u4PvrTickEnd = 0;

        FVR_SetDrawTickCallback(_PVR_Callback);

        // TODO: register pvr tick notify here
        WIDGET_SetIRHandler1KeyAction(_PVR_IRHandler);

    }

    NAV_DrawInfoOSD();

    //CLI_Parser("n.ms dtv1");

    return E_NAV_MSG_OK;
}
#endif

INT32 NAV_MM_PlayerUI(WIDGET_OSDINFO_T* prOSD)
{
    INT32 i4Status;
    WIDGET_BITMAP_T rIcon;
    rIcon.szName = NULL;
    rIcon.u4Height = 32;
    rIcon.u4Width = 16;
    rIcon.u4PitchSize = 32;
    i4Status = NAV_MM_GetState();
    if (i4Status)
    {
        if (i4Status == (INT32)E_NAV_MM_PLAY)
        {
            rIcon.pu1Address = (UINT8 *)_au2PlayIconActive;
        }
        else if (i4Status == (INT32)E_NAV_MM_PAUSE)
        {
            rIcon.pu1Address = (UINT8 *)_au2PauseIconActive;
        }
        else if (i4Status == (INT32)E_NAV_MM_STOP)
        {
            rIcon.pu1Address = (UINT8 *)_au2PlayIcon;

        }
        else
        {
            rIcon.pu1Address = (UINT8 *)_au2PauseIcon;
        }
        WIDGET_DrawBitmapScale(&rIcon, 20, 50, 32, 32, prOSD);

        if (i4Status == (INT32)E_NAV_MM_FAST_FORWARD)
        {
            rIcon.pu1Address = (UINT8 *)_au2ForwardIconActive;
        }
        else
        {
            rIcon.pu1Address = (UINT8 *)_au2ForwardIcon;
        }
        WIDGET_DrawBitmapScale(&rIcon, 60, 50, 32, 32, prOSD);

        if (i4Status == (INT32)E_NAV_MM_FAST_REVERSE)
        {
            rIcon.pu1Address = (UINT8 *)_au2ReverseIconActive;
        }
        else
        {
            rIcon.pu1Address = (UINT8 *)_au2ReverseIcon;
        }
        WIDGET_DrawBitmapScale(&rIcon, 100, 50, 32, 32, prOSD);

        if (i4Status == (INT32)E_NAV_MM_STOP)
        {
            rIcon.pu1Address = (UINT8 *)_au2StopIconActive;
        }
        else
        {
            rIcon.pu1Address = (UINT8 *)_au2StopIcon;
        }
        WIDGET_DrawBitmapScale(&rIcon, 140, 50, 32, 32, prOSD);

        CHAR szBuf[128];
        VERIFY(WIDGET_DrawString("Media Player", 20, 10, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);
        if (_u1ActiveSrcId < NAV_MM_MAX_SOURCE)
        {
            if (_arPlayInfo[_u1ActiveSrcId].i4MMSpeed >= 1000)
            {
                x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d V %d/%d A %d/%d Time %03u / %03u  %dX",
                          _u1ActiveSrcId,
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                    _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].u4MMCurTime/1000, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000, (UINT32)(_arPlayInfo[_u1ActiveSrcId].i4MMSpeed/1000));
            }
            else if (_arPlayInfo[_u1ActiveSrcId].i4MMSpeed < -1)
            {
                x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d V %d/%d A %d/%d Time %03u / %03u %dX",
                          _u1ActiveSrcId,
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                    _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].u4MMCurTime/1000, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000, (UINT32)(_arPlayInfo[_u1ActiveSrcId].i4MMSpeed/1000));
            }
            // slow motion
            else if (_arPlayInfo[_u1ActiveSrcId].i4MMSpeed > 1)
            {
                x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d V %d/%d A %d/%d Time %03u / %03u  1/%dX",
                          _u1ActiveSrcId,
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                    _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].u4MMCurTime/1000, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000, (UINT32)(1000/_arPlayInfo[_u1ActiveSrcId].i4MMSpeed));
            }
            // forward step
            else if (_arPlayInfo[_u1ActiveSrcId].i4MMSpeed == 1)
            {
                x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d V %d/%d A %d/%d Time %03u / %03u  1X",
                          _u1ActiveSrcId,
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                    _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].u4MMCurTime/1000, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000);
            }
            // backward step
            else if (_arPlayInfo[_u1ActiveSrcId].i4MMSpeed == -1)
            {
                x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d V %d/%d A %d/%d Time %03u / %03u  1X",
                          _u1ActiveSrcId,
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                    _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].u4MMCurTime/1000, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000);
            }
            else // if(_arPlayInfo[_u1ActiveSrcId].i4MMSpeed == 0)
            {
                x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d V %d/%d A %d/%d Time %03u / %03u  0X",
                          _u1ActiveSrcId,
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                    _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].u4MMCurTime/1000, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000);
            }


#ifdef TIME_SEEK_BY_KEY
            if (_arPlayInfo[_u1ActiveSrcId].ucSeekKeyCount>0)
            {
                Printf("PlayerUI %s\n", _arPlayInfo[_u1ActiveSrcId].aucSeekKey);//prOSD) == (INT32)E_WIDGET_MSG_OK);
                    x_sprintf(szBuf, "(%d) R(%d) AB(%c) P %d/%d  V %d/%d  A %d/%d  Time %s / %03u  %dX",
                          (_u1ActiveSrcId),
                          _arPlayInfo[_u1ActiveSrcId].fgRepeat,
                        _arPlayInfo[_u1ActiveSrcId].u4ABRepeat + 'A',
                          (_arPlayInfo[_u1ActiveSrcId].u4PgmNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentPgm+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4PgmNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentVidTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4VidTrackNumber,
                          (_arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber > 0) ? (_arPlayInfo[_u1ActiveSrcId].i4CurrentAudTrack+1) : 0,
                          _arPlayInfo[_u1ActiveSrcId].u4AudTrackNumber,
                          _arPlayInfo[_u1ActiveSrcId].aucSeekKey, _arPlayInfo[_u1ActiveSrcId].u4MMTotalTime/1000, (UINT32)(_arPlayInfo[_u1ActiveSrcId].i4MMSpeed/1000));
            }
#endif
        }

        VERIFY(WIDGET_DrawString(szBuf, 240, 10, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);
    }
    else
    {
        //UNUSED(x_sprintf(szBuf, "Stop"));
    }
#ifdef CC_SUPPORT_PVR
    if (_u4MMMode == E_NAV_MM_MODE_PVR)
    {
        CHAR szBuf[50];
        VERIFY(WIDGET_DrawString("PVR", 20, 10, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);
        //x_sprintf(szBuf, "%03u:%03u:%03u", _u4PvrTickStart, _u4PvrCurrentTick, _u4PvrTickEnd);
        x_sprintf(szBuf, "%03u / %03u", _u4PvrCurrentTick, _u4PvrTickEnd);
        VERIFY(WIDGET_DrawString(szBuf, 400, 10, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);
    }
    else
#endif
    {
            //VERIFY(WIDGET_DrawString("Media Player", 20, 10, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);
    }
    //VERIFY(WIDGET_DrawString("Multimedia", 20, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    //VERIFY(WIDGET_DrawString(szBuf, 20, 50, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

    WIDGET_ClearScreen(&arWidgetOSD[0]);
    VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(prOSD) == (INT32)E_WIDGET_MSG_OK);
    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_MM_UpdateUI(WIDGET_OSDINFO_T* prOSD)
{
    if (_u4MMMode == E_NAV_MM_MODE_VIEWER)
    {
        return NAV_MM_ViewerUI(prOSD);
    }
    else if (_u4MMMode == E_NAV_MM_MODE_PLAYER || _u4MMMode == E_NAV_MM_MODE_PVR)
    {
        return NAV_MM_PlayerUI(prOSD);
    }
    return -1024;
}

INT32 NAV_MM_Init(void)
{
    INT32 i = 0;
    UINT32 u4Tag = 0x12345678;

    x_msg_q_create(&_hMonitorMsgq, "MonMsgq", sizeof(UINT32), 256);

    x_thread_create(&_hMonitorThread, "MonThd", 2048, 200, _MonitorThread, 0, NULL);

    MUSB_Init(1);

    DMX_MM_Init();

    SWDMX_Init();

    PLAYMGR_Init();

#ifdef CC_SUPPORT_PVR
    /*
        CLI_Parser("pvr.i");
        CLI_Parser("ts.i");
        CLI_Parser("ts.ts.open dram 0x1000000");

        FVR_SetDrawTickCallback(_PVR_Callback);
    */
#endif

    if (fm_init() != FMR_OK)
    {
        Printf("\nError: Init FM fail\n");
    }
    if (x_fm_fat_init(NULL) != FMR_OK)
    {
        Printf("\nError: Init FAT fail\n");
    }

    MUSB_SetHotswapCallback(u4Tag, _MusbCallback);

    for (i = 0; i < NAV_MM_MAX_SOURCE; i++)
    {
        _arPlayInfo[i].u4MMState = E_NAV_MM_STOP;
        _arPlayInfo[i].i4MMSpeed = 1000;
        _arPlayInfo[i].fgMMAudEn = FALSE;
        _arPlayInfo[i].fgMMVidEn = FALSE;
        _arPlayInfo[i].fgMMPcrEn = FALSE;
        _arPlayInfo[i].fgMMEOS = FALSE;
        _arPlayInfo[i].u4MMTotalTime = 0;
        _arPlayInfo[i].u4MMCurTime = 0;
        _arPlayInfo[i].u4ABRepeat = 2;

        _arPlayInfo[i].u4AudTrackNumber = 0;
        _arPlayInfo[i].i4CurrentAudTrack = 0;
        _arPlayInfo[i].u4VidTrackNumber = 0;
        _arPlayInfo[i].i4CurrentVidTrack = 0;
        _arPlayInfo[i].u4PcrTrackNumber = 0;
        _arPlayInfo[i].i4CurrentPcrTrack = 0;
        _arPlayInfo[i].u4PgmNumber = 0;
        _arPlayInfo[i].i4CurrentPgm = 0;

#ifdef TIME_SEEK_BY_KEY
        _ClearSeekKeyStatus(i);
#endif
    }

#ifndef __KERNEL__
    // Create USB Dram Dump Thread
    vSetUSBDump(FALSE);
#endif
    return 0;
}

