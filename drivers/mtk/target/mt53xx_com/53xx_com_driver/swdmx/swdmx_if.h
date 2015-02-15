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
 * $RCSfile: swdmx_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_if.h
 *  Public interface of software program stream demux driver
 */


#ifndef SWDMX_IF_H
#define SWDMX_IF_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

/*lint -save -e961 */
#include "x_common.h"
#include "x_typedef.h"
#include "x_timer.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_clksrc.h"
#include "x_hal_5381.h"
#include "x_hal_926.h"

//#define DEFINE_IS_LOG   SWDMX_IsLog
//#include "x_debug.h"
/*lint -restore */

#include "drv_common.h"
#include "dmx_if.h"
#include "mpv_if.h"
#include "mpv_drvif.h"
#include "vdp_if.h"
#include "vdp_drvif.h"
#include "pmx_if.h"
#include "aud_if.h"
#include "feeder_if.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

#define SWDMX_DRV_OK			       ((INT32) 0)
#define SWDMX_DRV_SET_FAILED         ((INT32) -262)
#define SWDMX_DRV_INV_GET_INFO       ((INT32) -258)
#define SWDMX_DRV_INV_SET_INFO       ((INT32) -259) /**<the set operation is invalid or cannot be processed*/

// It is better to sync this with PLAYMGR_MAX_VID_TRACK
#define SWDMX_MAX_VID_TRACK    0x00000008
#define SWDMX_MAX_AUD_TRACK    0x00000010
#define SWDMX_MAX_SUB_TRACK    0x00000008
#define SWDMX_MAX_PCR_TRACK    0x00000001
#define SWDMX_MAX_PROGRAM      0x00000060

// SWDMX_STREAM_INFO
#define SWDMX_STREAM_INFO_MASK_NONE             0x00000000
#define SWDMX_STREAM_INFO_MASK_TRACK            0x00000001
#define SWDMX_STREAM_INFO_MASK_RESOLUTION       0x00000002

#define SWDMX_VID_PID_INDEX                     0x00000000
#define SWDMX_AUD_PID_INDEX                     0x00000001
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//#define CC_SWDMX_RENDER_PTS
#define CC_SWDMX_RENDER_POSITION

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

/** the notify conditions.
  This structure match to the one in mw. Can't change it's sequence.
*/
typedef enum
{
    SWDMX_COND_ERROR = -1,
    SWDMX_COND_CTRL_DONE,
    SWDMX_COND_SUBTITLE
}   SWDMX_COND_T;

/**This notification function is called by the video decoder in response to some events.
*/
typedef void (*PFN_SWDMX_NFY_FCT) (
    void*               pvNfyTag,
    SWDMX_COND_T        eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2
);

/**This structure contains a video decoders notify function and tag.
*/
typedef struct
{
    void*  pvTag;
    PFN_SWDMX_NFY_FCT  pfSwdmxNfy;
}SWDMX_NFY_INFO_T;



/** file format.
*/
typedef enum
{
    SWDMX_FMT_UNKNOWN,
    SWDMX_FMT_AVI,
    SWDMX_FMT_MPEG1_DAT,
    SWDMX_FMT_MPEG2_PS,
    SWDMX_FMT_MPEG2_TS,         // 188 ts
    SWDMX_FMT_MPEG2_TS_192,     // 192 ts, time case
    SWDMX_FMT_MPEG2_TS_ZERO_192,// 4 byte 192 ts
    SWDMX_FMT_MPEG2_TS_ENCRYPT_192,
    SWDMX_FMT_MPEG2_TS_TIME_SHIFT,// time shift 192 ts
    SWDMX_FMT_VC1_ES,
    SWDMX_FMT_MPEG2_VIDEO_ES,
    SWDMX_FMT_MPEG4_VIDEO_ES,
    SWDMX_FMT_H264_VIDEO_ES,
    SWDMX_FMT_AVS_VIDEO_ES,
    SWDMX_FMT_RAW_AUDIO_ES, // last one for ES, should not change this order.
    SWDMX_FMT_MP3_AUDIO_ES,
    SWDMX_FMT_VC1_WMV,
    SWDMX_FMT_WMA_AUDIO_ES,
    SWDMX_FMT_MP4,
    SWDMX_FMT_MKV,
    SWDMX_FMT_RM,
    SWDMX_FMT_FLV,
    SWDMX_FMT_CMPB,            // private container for CMPB
    SWDMX_FMT_ES_WITH_PTS
}   ENUM_SWDMX_FMT_T;


typedef enum
{
    eSWDMX_STRM_TYPE_UNKNOWN,
    eSWDMX_STRM_TYPE_VID,
    eSWDMX_STRM_TYPE_AUD,
    eSWDMX_STRM_TYPE_AUD2,
    eSWDMX_STRM_TYPE_SUBTITLE,
    eSWDMX_STRM_TYPE_PCR,
    eSWDMX_STRM_TYPE_MAX
} ENUM_SWDMX_STRM_TYPE_T;


/**The video codec format*/
typedef enum{
    ENUM_SWDMX_VDEC_UNKNOWN,
    ENUM_SWDMX_VDEC_MPEG1_MPEG2,
    ENUM_SWDMX_VDEC_DX311,
    ENUM_SWDMX_VDEC_MPEG4,
    ENUM_SWDMX_VDEC_H264,
    ENUM_SWDMX_VDEC_VC1,
    ENUM_SWDMX_VDEC_WMV7,
    ENUM_SWDMX_VDEC_WMV8,
    ENUM_SWDMX_VDEC_WMV9,
    ENUM_SWDMX_VDEC_MJPEG,
    ENUM_SWDMX_VDEC_RV,
    ENUM_SWDMX_VDEC_H263,
    ENUM_SWDMX_VDEC_MAX
}ENUM_SWDMX_VDEC_T;


/**Audio decoder format.*/
typedef enum{
    ENUM_SWDMX_ADEC_UNKNOWN = 0,
    ENUM_SWDMX_ADEC_MPEG,
    ENUM_SWDMX_ADEC_AC3,
    ENUM_SWDMX_ADEC_PCM,
    ENUM_SWDMX_ADEC_MP3,
    ENUM_SWDMX_ADEC_AAC,
    ENUM_SWDMX_ADEC_DTS,
    ENUM_SWDMX_ADEC_WMA,
    ENUM_SWDMX_ADEC_RA,
    ENUM_SWDMX_ADEC_HDCD,
    ENUM_SWDMX_ADEC_MLP,
    ENUM_SWDMX_ADEC_MTS,
    ENUM_SWDMX_ADEC_A2,
    ENUM_SWDMX_ADEC_PAL,
    ENUM_SWDMX_ADEC_FMFM,
    ENUM_SWDMX_ADEC_NICAM,
    ENUM_SWDMX_ADEC_TTXAAC,
    ENUM_SWDMX_ADEC_DETECTOR,
    ENUM_SWDMX_ADEC_MINER
}ENUM_SWDMX_ADEC_T;


/**Subpic format.*/
typedef enum{
    ENUM_SWDMX_SUBPIC_UNKNOWN = 0,
    ENUM_SWDMX_SUBPIC_MAX
}ENUM_SWDMX_SUBPIC_T;

typedef enum
{
    eSWDMX_STATE_STOP,			// stop
    eSWDMX_STATE_PAUSE,      	// pause
    eSWDMX_STATE_PLAY,         	// play
    eSWDMX_STATE_MAX
} ENUM_SWDMX_PLAY_STATE_T;


typedef enum _SWDMX_SET_DECODER_TYPE
{
    eSWDMX_SET_VIDSEQHEADER,
    eSWDMX_SET_VIDEOTYPE,
    eSWDMX_SET_AUDIOTYPE,
    eSWDMX_SET_SYSTEMINFO,
    eSWDMX_ENABLE_STREAM,
    eSWDMX_DISABLE_STREAM,
    eSWDMX_SET_RANGECALLBACK,
    eSWDMX_SET_CHK_PSI_BUF_SIZE, // should be eSWDMX_CHK_PSI_BUF_SIZE
    eSWDMX_TOTAL_DURATION,
    eSWDMX_SET_FEEDER_TEST_FUNCTION,
    eSWDMX_SET_SUBTITLECALLBACK,
    eSWDMX_GET_CURRENT_POSITION,
    eSWDMX_GET_FILE_OFFSET,
    eSWDMX_GET_IS_TRICK, // should be eSWDMX_GET_IS_TRICKABLE
    eSWDMX_CONTAINER_TYPE,
    eSWDMX_GET_INTERNAL_INFO,
    eSWDMX_SET_REC_BUF_RANGE,
    eSWDMX_PRINT_REC_BUF_STATUS,
    eSWDMX_PRINT_INTERNAL_STATUS,
    eSWDMX_SET_MONITOF_LBA,
    eSWDMX_GET_CURRENT_TICK,
    eSWDMX_GET_CURRENT_LBA,
    eSWDMX_SET_NETFLIX_STREAM_ID,
    eSWDMX_SET_NETFLIX_EOS,
    eSWDMX_GET_CURRENT_TIME,
    eSWDMX_PID_INDEX_START,
    eSWDMX_SET_TPT_VIDSEQHEADER,    //for divx plus
    eSWDMX_SET_AOUT_ENABLE,
    eSWDMX_SET_EOS,
    eSWDMX_SET_VIDEO_DECRYPT,
    eSWDMX_SET_AUDIO_DECRYPT,
    eSWDMX_SET_VID_PID_IDX,
    eSWDMX_SET_AUD_PID_IDX,
    eSWDMX_SET_PCR_PID_IDX,
    eSWDMX_GET_STC_OFFSET,
    eSWDMX_GET_INTERNAL_BUF,
    eSWDMX_SET_MAX
}SWDMX_SET_DECODER_TYPE;


typedef struct SWDMX_STREAM_INFO
{
    // SWDMX_STREAM_INFO_MASK_TRACK
    UINT16 u2VideoNum;
	UINT16 u2AudioNum;
	UINT16 u2SubpicNum;
	UINT16 u2PcrNum;

	UINT16 au2VideoId[SWDMX_MAX_VID_TRACK];
	UINT16 au2AudioId[SWDMX_MAX_AUD_TRACK];
	UINT16 au2AudioSubId[SWDMX_MAX_AUD_TRACK];
	UINT16 au2SubpicId[SWDMX_MAX_SUB_TRACK];
	UINT16 au2PcrId[SWDMX_MAX_PCR_TRACK];

    UINT32 u4FirstPts;
    UINT32 u4TotalTime;

    ENUM_SWDMX_VDEC_T aeVideo[SWDMX_MAX_VID_TRACK];
    ENUM_SWDMX_ADEC_T aeAudio[SWDMX_MAX_AUD_TRACK];
    ENUM_SWDMX_SUBPIC_T aeSubpic[SWDMX_MAX_SUB_TRACK]; // TS no caption
    //ENUM_SWDMX_SUBPIC_T aePcrpic[SWDMX_MAX_PCR_TRACK];

    // SWDMX_STREAM_INFO_MASK_RESOLUTION
    UINT32 u4Width[SWDMX_MAX_VID_TRACK];
    UINT32 u4Height[SWDMX_MAX_VID_TRACK];
} SWDMX_STREAM_INFO_T;


typedef struct SWDMX_PROGRAM_INFO
{
    // output
    UINT32 u4InfoMask;
    UINT32 u4PgmNum;
    SWDMX_STREAM_INFO_T rPgm[SWDMX_MAX_PROGRAM];
} SWDMX_PROGRAM_INFO_T;


typedef struct SWDMX_RANGE_INFO
{
    UINT32 u4CurRangeID;
    UINT64 u8CurRangePTS;
    UINT32 u4QRangeID;
    UINT64 u8QRangePTS;
}SWDMX_RANGE_INFO_T;

typedef struct SWDMX_READ_INFO
{
    UCHAR*  pucDest;
    UINT8   u1SerialNumber;
    UINT32  u4FrameAddr;
    UINT32  u4CopyLen;
}SWDMX_READ_INFO_T;

typedef enum
{
    ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE,
    ENUM_SWDMX_AUDIO_NOTIFY_NO_DATA,
    ENUM_SWDMX_AUDIO_NOTIFY_MAX
}ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T;


typedef struct
{
    UINT8 u1FlavorID;
    UINT8 u1ChanNum;
    UINT32 u4SamplePerFrame;
    UINT32 u4CookSampRate;
    UINT16 u2FrameSzInBytes;
    UINT16 u2RegnNum;
    UINT16 u2StartRegn;
    UINT16 u2CplQBits;
    UINT16 u2CookBlockPerSB;
    UINT16 u2CookBlockSz;
    UINT16 u2CookFramePerBlock;
    UINT16 u2CookFramePerSB;
    UINT16 u2CookSBSz;
} COOK_INFO_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototype  of public functions
//-----------------------------------------------------------------------------

EXTERN VOID SWDMX_Init(VOID);

EXTERN BOOL SWDMX_GetStreamInfo(ENUM_SWDMX_FMT_T eType,
        UINT32 u4InfoMask,
        UINT64 u8FileSize,
        SWDMX_PROGRAM_INFO_T* prPInfo);

EXTERN BOOL SWDMX_SetInfo(UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3);

EXTERN BOOL SWDMX_GetInfo(UINT8 u1Type,UINT32* pu4Para1,UINT32* pu4Para2,UINT32* pu4Para3);

EXTERN BOOL SWDMX_SetSpeed(INT32 i4Speed);

EXTERN BOOL SWDMX_SeekTime(UINT32 u4SeekTime, UINT64* pu8FilePos);

EXTERN BOOL SWDMX_SeekPos(UINT64 u8SeekPos, UINT64 u8SeekPosI);

EXTERN BOOL SWDMX_SeekPercent(UINT32 u4SeekPercent, UINT64* pu8FilePos);

EXTERN BOOL SWDMX_RenderFromPos(VOID *prPosition);

EXTERN BOOL SWDMX_SetContainerType(ENUM_SWDMX_FMT_T eContainerType);

EXTERN BOOL SWDMX_Play(VOID);

EXTERN BOOL SWDMX_Pause(VOID);

EXTERN BOOL SWDMX_Stop(VOID);

EXTERN BOOL SWDMX_Flush(VOID);

EXTERN BOOL SWDMX_SetStrmID(UINT32 u4StrmID, UINT32 u4StrmSubID, ENUM_SWDMX_STRM_TYPE_T eStrmType);

EXTERN BOOL SWDMX_AddRange(VOID* pvRangeInfo, UINT32 u4TagID);

EXTERN BOOL SWDMX_DelRange(VOID* pvRangeInfo, UINT32 u4TagID, BOOL fgDelAllRange);

EXTERN ENUM_SWDMX_PLAY_STATE_T SWDMX_QueryStatus(VOID);

EXTERN BOOL SWDMX_SetRangeCb(VOID* pfnCbFunc);

EXTERN VOID SWDMX_QueryRangeInfo(SWDMX_RANGE_INFO_T* prRangeInfo);

EXTERN INT32 SWDMX_SetDmxNfy(const SWDMX_NFY_INFO_T* prSwdmxNfyInfo);

EXTERN BOOL SWDMX_ReadBuffer(ENUM_SWDMX_STRM_TYPE_T eStrmType, SWDMX_READ_INFO_T *prReadInfo);

EXTERN BOOL SWDMX_ReleaseBuffer(ENUM_SWDMX_STRM_TYPE_T eStrmType, SWDMX_READ_INFO_T *prReadInfo);

EXTERN BOOL SWDMX_AudioNotify(ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType);

EXTERN BOOL SWDMX_PcrCallBack(UINT32 u4PcrBase);

#ifdef CC_SWDMX_RENDER_PTS
EXTERN VOID SWDMX_SetRenderPts(UINT32 u4RenderPts);
#endif

#if 0//def CC_SWDMX_RENDER_POSITION
EXTERN VOID SWDMX_GetRenderPosition(INT32 *pi4TempRef, UINT16 *pu2DecodingOrder, UINT64 *pu8Offset, UINT64 *pu8OffsetI);
EXTERN VOID SWDMX_SetRenderPosition(INT32 i4TempRef, UINT16 u2DecodingOrder, UINT64 u8Offset, UINT64 u8OffsetI);
#endif

#endif	// SWDMX_IF_H


