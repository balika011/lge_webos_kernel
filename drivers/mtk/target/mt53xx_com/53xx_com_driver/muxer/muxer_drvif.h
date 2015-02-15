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
 * $RCSfile: muxer_drvif.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_drvif.h
 *  Public interface of software program stream demux driver
 */


#ifndef MUXER_DRVIF_H
#define MUXER_DRVIF_H

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
#include "x_util.h"
#include "muxer_if.h"
#include "gcpu_if.h" // for scramble

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define MUXER_THREAD_PRIORITY  90  // ToDo: move it to drv_common.h and assign a fine value

#define MUXER_MAX_INST_NS   2

#define MUXER_MAX_STRM_NS   3

#define MUXER_MAX_SECTION_NS 9

#define MUXER_SECTION_BUF_SIZE   (4 * 1024)

#define MUXER_CMD_Q_SIZE 10
#define MUXER_VID_PES_Q_SIZE 1024
#define MUXER_AUD_PES_Q_SIZE 1024
#define MUXER_VBI_PES_Q_SIZE 1024
#define MUXER_DATA_Q_SIZE    1024

#define MUXER_VID_HDR_BUF_SIZE 128

#define MUXER_FPS_ACCURACY 1000

#define MUXER_INVALID_UINT32  0xFFFFFFFF
#define MUXER_INVALID_STRM_ID  0xFFFFFFFF

#define MUXER_TMP_BUF_SZ    (512 * 1024)

#define MUXER_VERIFY_ID_RET(id, ret)                                        \
do {                                                                        \
    if (MUXER_MAX_INST_NS <= (id))                                          \
    {                                                                       \
        LOG(3, "%s(%d): wrong id(%d)!!!\n", __FUNCTION__, __LINE__, (id));  \
        return ret;                                                         \
    }                                                                       \
} while (0)

#define MUXER_VERIFY_ID_SET(id, def)                                        \
do {                                                                        \
    if (MUXER_MAX_INST_NS <= (id))                                          \
    {                                                                       \
        LOG(3, "%s(%d): wrong id(%d)!!!\n", __FUNCTION__, __LINE__, (id));  \
        ASSERT(MUXER_MAX_INST_NS > (id));                                   \
        (id) = (def);                                                       \
    }                                                                       \
} while (0)

#define _MxInitSyncCmdFlag(obj)    x_sema_lock(obj->hMxSyncSema, X_SEMA_OPTION_NOWAIT)
#define _MxWaitSyncCmdDone(obj)    VERIFY(x_sema_lock(obj->hMxSyncSema, X_SEMA_OPTION_WAIT) == OSR_OK)
#define _MxNotifySyncCmdDone(obj)  VERIFY(x_sema_unlock(obj->hMxSyncSema) == OSR_OK)


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct
{
    UINT32 u4Pcr;
    UINT32 u4Timestamp; // 192 timestamp
    UINT32 u4CurVidPts;
    UINT32 u4CurAudPts;
    UINT32 u4CurVbiPts;
    UINT32 u4AudResidueLen;
    UINT32 u4AudResiduePts;
    UCHAR *pucAudResidueBuf;

    UINT8 u1NetContinuity;
    UINT8 u1PatContinuity;
    UINT8 u1PmtContinuity;
    UINT8 u1VidContinuity;
    UINT8 u1AudContinuity;
    UINT8 u1TtxContinuity;
    UINT8 u1SectContinuity[MUXER_MAX_SECTION_NS];

#ifdef MUXER_VID_HDR_BUF_SIZE
    UCHAR aucVidHdrBuf[MUXER_VID_HDR_BUF_SIZE];
    UINT32 u4VidHdrBufWp;
    UINT32 u4VidHdrBufSz;
#endif
} MUXER_MPG_INFO_T;

typedef struct
{
    UINT32 u4CurVidPts;
    UINT32 u4CurAudPts;
} MUXER_CMPB_INFO_T;

typedef struct
{
    UINT32 u4CurVidPts;
    UINT32 u4CurAudPts;
} MUXER_RAW_INFO_T;

typedef enum
{
    eMUXER_HDR_SUCCEED,      // header muxing succeed
    eMUXER_HDR_FAIL,         // header muxing fail
    eMUXER_HDR_NO_DATA,      // no data
    eMUXER_HDR_UNKNOWN_TYPE, // unknown muxing type
} MUXER_HDR_STATE_T;

typedef struct
{
    BOOL fgEnabled;
    UINT32 u4SecTionPts;
    MUXER_ADD_SECTION_INFO_T rAddSection;
} MUXER_SECTION_T;

typedef struct
{
    ENUM_MUXER_STRMTYPE_T eStrmType;
    BOOL fgEnabled;
    UINT32 u4CurPts;
} MUXER_STRM_T;

typedef BOOL (*PFN_START)(UCHAR ucMxId);
typedef BOOL (*PFN_STOP)(UCHAR ucMxId);
typedef MUXER_HDR_STATE_T (*PFN_MULTIPLEX_HDR)(UCHAR ucMxId);

typedef struct
{
    UCHAR   ucMxId;
    BOOL    fgMuxerInited;
    BOOL    fgOccupied;
    BOOL    fgEnVideo;
    BOOL    fgEnAudio;
    BOOL    fgEnTeletext;
    BOOL    fgScramble;
    BOOL    fgKeyframe;
    BOOL    fgFrame;
    BOOL    fgTrustZone;

    HANDLE_T hMuxerMainThread;
    HANDLE_T hMuxerCmdQ;
    HANDLE_T hMuxerVidDataQ;
    HANDLE_T hMuxerAudDataQ;
    HANDLE_T hMuxerVBIDataQ;
    HANDLE_T hMuxerNfyTimer;
    HANDLE_T hMxSyncSema;
    HANDLE_T hMxWaitBufTimer;
    HANDLE_T hMxWaitBufSema;

    UCHAR *pucMxBuf;
    UCHAR *pucTmpFrmBuf;
    MX_FCT_NFY pfNotify;
    PFN_START pfnStart;
    PFN_STOP  pfnStop;
    PFN_MULTIPLEX_HDR pfnMuxHdr;

    UINT32 u4ValidStrmNs;
    UINT32 u4VidCnt;
    UINT32 u4AudCnt;
    UINT32 u4VBICnt;
    UINT32 u4RecTotalTime;
    UINT32 u4MxBufSz;
    UINT32 u4MxBufWpNfy; // Notify until whole pes done
    UINT32 u4MxBufWp;
    UINT32 u4MxBufRp;
    UINT32 u4NfyPeriod;
    UINT32 u4LastNfyBufWp;
    UINT32 u4BitPos;
    UINT32 u4MxBufFrmCnt;
    UINT16 u2VideoPid;
    UINT16 u2AudioPid;  
    UINT16 u2PcrPid;

    ENUM_MUXER_STRMTYPE_T eCurrentWaiting;
    ENUM_MUXER_STATE_T eMuxerPrevousStatus;
    ENUM_MUXER_STATE_T eMuxerStatus;
    ENUM_MUXER_FMT_T eCurFMT;
    ENUM_MUXER_VENC_T eVidFmt;
    ENUM_MUXER_AENC_T eAudFmt;

    VENC_PES_INFO_T rVEncPesInfo;
    AENC_PES_INFO_T rAEncPesInfo;
    VBIENC_PES_INFO_T rVBIEncPesInfo;
    MUXER_STRM_T arStrm[MUXER_MAX_STRM_NS];
    MUXER_SECTION_T arSection[MUXER_MAX_SECTION_NS];
    TSDESC_PARAM_T rTsDescParam; // for scramble

    union
    {
        MUXER_MPG_INFO_T  rMuxerMpgInfo;
        MUXER_CMPB_INFO_T rMuxerCmpbInfo;
        MUXER_RAW_INFO_T  rMuxerRawInfo;
    } rFmtInfo;
} MUXER_INFO_T;


//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototype  of public functions
//-----------------------------------------------------------------------------
VOID _MUXER_Init(VOID);

MX_RET _MUXER_GetInst(UCHAR *pucMxId);

MX_RET _MUXER_ReleaseInst(UCHAR ucMxId);

MUXER_INFO_T *_MUXER_QueryInst(UCHAR ucMxId);

BOOL _MUXER_Start(UCHAR ucMxId);

BOOL _MUXER_Pause(UCHAR ucMxId);

BOOL _MUXER_Stop(UCHAR ucMxId);

BOOL _MUXER_Flush(UCHAR ucMxId);

BOOL _MUXER_SendVideoPes(UCHAR ucMxId, VENC_PES_INFO_T *prVEncPes);

BOOL _MUXER_SendAudioPes(UCHAR ucMxId, AENC_PES_INFO_T *prAEncPes);

BOOL _MUXER_SendVBIPes(UCHAR ucMxId, VBIENC_PES_INFO_T *prVBIEncPes);

BOOL _MUXER_ReceiveVideoPes(UCHAR ucMxId);

BOOL _MUXER_ReceiveAudioPes(UCHAR ucMxId);

BOOL _MUXER_ReceiveVBIPes(UCHAR ucMxId);

BOOL _MUXER_QueryVideoPesCnt(UCHAR ucMxId, UINT16 *pu2QueueSize);

BOOL _MUXER_QueryAudioPesCnt(UCHAR ucMxId, UINT16 *pu2QueueSize);

BOOL _MUXER_QueryVBIPesCnt(UCHAR ucMxId, UINT16 *pu2QueueSize);

VOID _MUXER_FlushData(UCHAR ucMxId);

BOOL _MUXER_SetMuxFmt(UCHAR ucMxId, ENUM_MUXER_FMT_T eMuxFmt);

BOOL _MUXER_SetVencFmt(UCHAR ucMxId, ENUM_MUXER_VENC_T eVidFmt);

BOOL _MUXER_SetAencFmt(UCHAR ucMxId, ENUM_MUXER_AENC_T eAudFmt);

BOOL _MUXER_SetBuffer(UCHAR ucMxId, UINT32 u4BufSa, UINT32 u4BufSz, MX_FCT_NFY pfNotify, UINT32 u4NfyPeriod);

BOOL _MUXER_UpdateRp(UCHAR ucMxId, UINT32 u4Rp);

BOOL _MUXER_AddStrm(UCHAR ucMxId, ENUM_MUXER_STRMTYPE_T eStrm, UINT32 *pu4StrmNo);

BOOL _MUXER_DelStrm(UCHAR ucMxId, UINT32 u4StrmNo);

BOOL _MUXER_SetScramble(UCHAR ucMxId, BOOL fgScramble);

BOOL _MUXER_SetScrambleKey(UCHAR ucMxId, MM_PVR_CRYPT_INFO_T *ptPvrCrypt);

BOOL _MUXER_Query(UCHAR ucMxId);

VOID vMxPutBits(MUXER_INFO_T *prMuxInfo, UINT32 u4Pattern, UINT32 u4BitLen);

VOID vMxCopyBytes(MUXER_INFO_T *prMuxInfo, UINT32 u4StartAddr, UINT32 u4FrameSize);

BOOL fgMxBufferAvail(MUXER_INFO_T *prMuxInfo, UINT32 u4Len);

BOOL MxWaitBufferAvail(MUXER_INFO_T *prMuxInfo, UINT32 u4Len);

VOID vMxNotifyDataReady(MUXER_INFO_T *prMuxInfo, UINT32 u4Pts);

VOID _vMxUpdatePts(MUXER_INFO_T *prMuxInfo, UINT32 u4StrmNo, UINT32 u4Pts);

VOID _MxStrmSelect(MUXER_INFO_T *prMuxInfo, ENUM_MUXER_STRMTYPE_T *peStrm, UINT32 *pu4StrmNo);

VOID _MxVideoTypeAnalyze(MUXER_INFO_T *prMuxInfo, VENC_PES_INFO_T *prVidPesInfo);

BOOL _MUXER_ConfigPid(UCHAR ucMxId, ENUM_MUXER_PID_TYPE_T ePidType, UINT16 u2Pid);

BOOL _MUXER_AddSection(UCHAR ucMxId, MUXER_ADD_SECTION_INFO_T *ptAddSection);

BOOL _MUXER_DelSection(UCHAR ucMxId, INT32 i4SectionId);

BOOL _MUXER_UseTrustZone(UCHAR ucMxId, BOOL fgTrustZone);

#endif  // MUXER_DRVIF_H

