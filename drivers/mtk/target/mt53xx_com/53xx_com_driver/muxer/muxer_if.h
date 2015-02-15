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
 * $RCSfile: muxer_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_if.h
 *  Public interface of software program stream demux driver
 */


#ifndef MUXER_IF_H
#define MUXER_IF_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

/*lint -save -e961 */
#include "drv_common.h"
#include "x_common.h"
#include "x_mm_common.h"
#include "x_typedef.h"
#include "x_timer.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_clksrc.h"
#include "x_hal_5381.h"
#include "x_hal_926.h"


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef enum
{
    MX_RET_OK                =  0,
    MX_RET_GNRL_ERR          = -1,
    MX_RET_NO_AVAIL_INST     = -2,
    MX_RET_INST_NOT_OCCUPIED = -3
} MX_RET;


typedef enum
{
    eMUXER_STATE_STOP,          // stop
    eMUXER_STATE_PAUSE,       // pause
    eMUXER_STATE_RUN,           // run
    eMUXER_STATE_FLUSH,
    eMUXER_STATE_MAX
} ENUM_MUXER_STATE_T;


typedef enum
{
    eMUXER_STRMTYPE_VIDEO,          // video
    eMUXER_STRMTYPE_AUDIO,          // audio
    eMUXER_STRMTYPE_VBI,            // teletext
    eMUXER_STRMTYPE_MAX
} ENUM_MUXER_STRMTYPE_T;


/** ENUM_MUXER_PID_TYPE_T
 * pid type
 */
typedef enum
{
    eMUXER_PID_TYPE_VIDEO = 0,                      ///< pid type video
    eMUXER_PID_TYPE_AUDIO,                          ///< pid type audio
    eMUXER_PID_TYPE_PCR,                            ///< pid type pcr
    eMUXER_PID_TYPE_OTHER                           ///< pid type unknown
} ENUM_MUXER_PID_TYPE_T;


typedef struct
{
    UINT32 u4FifoStart;
    UINT32 u4FifoEnd;
    UINT32 u4StartAddr;
    UINT32 u4FrameSize;
    UINT32 u4VideoType;
    UINT32 u4Pts;
    VOID*  pvTag;
} VENC_PES_INFO_T;


typedef struct
{
    UINT32 u4FifoStart;
    UINT32 u4FifoEnd;
    UINT32 u4StartAddr;
    UINT32 u4Pts;
    UINT32 u4FrameSize;
} AENC_PES_INFO_T;

typedef struct
{
    UINT32 u4Handle;
    UINT16 u2Pid;
    INT32  i4Repetitiontime;
    INT32  i4Sectionid;    
    INT32  i4sectionlen;
    UINT8* pu1sectiondata;
} MUXER_ADD_SECTION_INFO_T;

typedef struct
{
    UINT32 u4FifoStart;
    UINT32 u4FifoEnd;
    UINT32 u4StartAddr;
    UINT32 u4Pts;
    UINT32 u4FrameSize;
} VBIENC_PES_INFO_T;


typedef struct
{
    UINT32 u4FifoStart;
    UINT32 u4FifoEnd;
    UINT32 u4StartAddr;
    UINT32 u4FrameSize;
    UINT32 u4PesType;
} MUXER_PES_INFO_T;


/*
*   Container format.
*/
typedef enum
{
    MUXER_FMT_UNKNOWN,
    MUXER_FMT_AVI,
    MUXER_FMT_MPEG1_DAT,
    MUXER_FMT_MPEG2_PS,
    MUXER_FMT_MPEG2_TS,         // 188 ts
    MUXER_FMT_MPEG2_TS_192,     // 192 ts, time case
    MUXER_FMT_MP4,
    MUXER_FMT_CMPB,
    MUXER_FMT_RAW
} ENUM_MUXER_FMT_T;


/*
*   The video codec format.
*/
typedef enum
{
    ENUM_MUXER_VENC_UNKNOWN,
    ENUM_MUXER_VENC_MPEG1,
    ENUM_MUXER_VENC_MPEG2,
    ENUM_MUXER_VENC_H264,
    ENUM_MUXER_VENC_MAX
}ENUM_MUXER_VENC_T;


/*
*   The Audio codec format.
*/
typedef enum
{
    ENUM_MUXER_AENC_UNKNOWN = 0,        /* Unknown Audio Codec Type*/
    ENUM_MUXER_AENC_MPEG,               /* MPEG1 or MPEG2 audio, layer I or layer II*/
    ENUM_MUXER_AENC_MP3,                /* MPEG1 or MPEG2 audio, layer III*/
    ENUM_MUXER_AENC_AAC,                /* AAC, or HE-AAC*/
    ENUM_MUXER_AENC_DD,                 /* Dolby Digital, or Dolby Digital Plus*/
    ENUM_MUXER_AENC_TRUEHD,             /* Dolby TrueHD*/
    ENUM_MUXER_AENC_PCM,                /* PCM*/
    ENUM_MUXER_AENC_DTS,                /* DTS*/
    ENUM_MUXER_AENC_DTS_HD_HR,          /* DTS-HD High Resolution*/
    ENUM_MUXER_AENC_DTS_HD_MA,          /* DTS-HD Master Audio*/
    ENUM_MUXER_AENC_WMA,                /* WMA standard; not WMA Pro nor WMA Lossless*/
    ENUM_MUXER_AENC_COOK,               /* Cook*/
    ENUM_MUXER_AENC_VORBIS,             /* Vorbis*/
    ENUM_MUXER_AENC_FLAC,               /* Free Lossless Audio Codec*/
    ENUM_MUXER_AENC_MONKEY              /* Monkey's Audio*/
}ENUM_MUXER_AENC_T;


/*
*   Data Notification structure
*/
typedef struct _MX_CB_DATA_T
{
    UINT32 u4Addr;
    UINT32 u4Len;
    UINT32 u4Pts;
    UINT32 u4Flag;
} MX_CB_DATA_T;


/* notification conditions */
typedef enum _MX_NFY_COND_T
{
    MX_COND_ERROR    = -2, /* Error */
    MX_COND_OVERFLOW = -1, /* Buffer Overflow */
    MX_COND_DATA     =  0, /* Buffer Data Available */
} MX_NFY_COND_T;


/* Overflow reasons */
typedef enum _MX_OVERFLOW_REASON_T
{
    MX_OVERFLOW_REASON_RING_BUF_FULL    = 1,
    MX_OVERFLOW_REASON_METADATA_BUF_FULL = 2
} MX_OVERFLOW_REASON_T;


/* Error reasons */
typedef enum _MX_ERROR_REASON_T
{
    MX_ERROR_REASON_INTERNAL    = 1
} MX_ERROR_REASON_T;


/*
*   Notification function
*/
typedef VOID (*MX_FCT_NFY)(UCHAR ucMxId, MX_NFY_COND_T eType, VOID *pvData);


//-----------------------------------------------------------------------------
// Prototype of inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototype  of public functions
//-----------------------------------------------------------------------------

VOID MUXER_Init(VOID);

MX_RET MUXER_GetInst(UCHAR *pucMxId);

MX_RET MUXER_ReleaseInst(UCHAR ucMxId);

BOOL MUXER_Start(UCHAR ucMxId);

BOOL MUXER_Pause(UCHAR ucMxId);

BOOL MUXER_Stop(UCHAR ucMxId);

BOOL MUXER_Flush(UCHAR ucMxId);

BOOL MUXER_SendVideoPes(UCHAR ucMxId, VENC_PES_INFO_T *prVEncPes);

BOOL MUXER_SendAudioPes(UCHAR ucMxId, AENC_PES_INFO_T *prAEncPes);

BOOL MUXER_SendVBIPes(UCHAR ucMxId, VBIENC_PES_INFO_T *prVBIEncPes);

BOOL MUXER_SetMuxFmt(UCHAR ucMxId, ENUM_MUXER_FMT_T eMuxFmt);

BOOL MUXER_SetVencFmt(UCHAR ucMxId, ENUM_MUXER_VENC_T eVidFmt);

BOOL MUXER_SetAencFmt(UCHAR ucMxId, ENUM_MUXER_AENC_T eAudFmt);

BOOL MUXER_SetBuffer(UCHAR ucMxId, UINT32 u4BufSa, UINT32 u4BufSz, MX_FCT_NFY pfNotify, UINT32 u4NfyPeriod);

BOOL MUXER_UpdateRp(UCHAR ucMxId, UINT32 u4Rp);

BOOL MUXER_AddStrm(UCHAR ucMxId, ENUM_MUXER_STRMTYPE_T eStrm, UINT32 *pu4StrmNo);

BOOL MUXER_DelStrm(UCHAR ucMxId, UINT32 u4StrmNo);

BOOL MUXER_SetScramble(UCHAR ucMxId, BOOL fgScramble);

BOOL MUXER_SetScrambleKey(UCHAR ucMxId, MM_PVR_CRYPT_INFO_T *ptPvrCrypt);

BOOL MUXER_Query(UCHAR ucMxId);

BOOL MUXER_ConfigPid(UCHAR ucMxId, ENUM_MUXER_PID_TYPE_T ePidType, UINT16 u2Pid);

BOOL MUXER_AddSection(UCHAR ucMxId, MUXER_ADD_SECTION_INFO_T *ptAddSection);

BOOL MUXER_DelSection(UCHAR ucMxId, INT32 i4SectionId);

BOOL MUXER_UseTrustZone(UCHAR ucMxId, BOOL fgTrustZone);

#endif  // MUXER_IF_H

