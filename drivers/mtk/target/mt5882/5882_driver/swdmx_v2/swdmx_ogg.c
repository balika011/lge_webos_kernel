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
 * $RCSfile: swdmx_Ogg.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_Ogg.c
 *  Software demux for Ogg - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "swdmx_ogg.h"
#include "vdec_drvif.h"
#include "u_midxbuld.h"
#include "swdmx_debug.h"
#include "aud_drvif.h"
#include "b2r_drvif.h"
#include "b2r_if.h"

#ifdef SWDMX_DUMP_DATA
#include "swdmx_debug_util.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define AAC_FAKE_HDR_SIZE               7
#define AAC_ALLOC_SIZE                  (256)

#define MIN_READ_SIZE                   (512 * 1024)
#define MIN_READ_SIZE_NO_VIDEO          (192 * 1024)
#define OGG_BUFFER_SIZE                 (235 * 7 * 1024)
#define OGG_TEN_SEC_DURATION            (90000 * 10)
#define OGG_THIRTY_SEC_DURATION         (90000 * 30)
#define OGG_ONE_MB                      (1024 * 1024)
#define SWDMX_OGG_AAC_ALLOC_SIZE        (256)
#define SWDMX_OGG_AAC_FAKE_HDR_SIZE     (7)
#define OGG_INVAILD_AUD_IDX             (255)
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define get_min(x, y)   (((x) < (y)) ? (x) : (y))
#define get_abs(x)      (((x) >= 0) ? (x) : -(x))

#define SWDMX_OGG_VERIFY_NULL(ptr)                                    \
    do {                                              \
        if (ptr == NULL) { \
            LOG(3, "SWDMX pointer is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
            return; }      \
    } while (0)

#define SWDMX_OGG_VERIFY_NULL_ERROR(ptr)                                    \
    do {                                              \
        if (ptr == NULL) { \
            LOG(3, "SWDMX pointer is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
            return FALSE; }      \
    } while (0)

#define SWDMX_OGG_NOT_NEED_SEARCH_START_CODE(vdec_type)     ( \
    ((vdec_type) == ENUM_SWDMX_VDEC_DX311) || \
    ((vdec_type) == ENUM_SWDMX_VDEC_WMV7) || \
    ((vdec_type) == ENUM_SWDMX_VDEC_WMV8) || \
    ((vdec_type) == ENUM_SWDMX_VDEC_WMV9) || \
    ((vdec_type) == ENUM_SWDMX_VDEC_MJPEG) || \
    ((vdec_type) == ENUM_SWDMX_VDEC_H263))

#define SWDMX_OGG_PIC_STARTCODE        (0x00000001)
#define SWDMX_OGG_STARTCODE_LEN        (4)
#define SWDMX_OGG_PIC_HDR_BUF_LEN      (16)
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static KEY_INFO stKeyinfo;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static SWDMX_HDR_PRS_STATE_T _SWDMX_OggRequestData(SWDMX_INFO_T* prSwdmxInst);

static SWDMX_HDR_PRS_STATE_T _SWDMX_OggParsePage(  UINT8 u1SrcId,
        SWDMX_RANGE_LIST_T* prRangeList,
        UCHAR* pucBitstream,
        UINT32 u4AvailSize,
        UINT32* pu4ConsumeSize);

static SWDMX_HDR_PRS_STATE_T _SWDMX_OggParse(  UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList);

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverKey(UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList);

static SWDMX_HDR_PRS_STATE_T    _SWDMX_OggMoveKey(  SWDMX_INFO_T *prSwdmxInst,
        SWDMX_RANGE_LIST_T* prRangeList,
        UCHAR* pucBitstream);


static SWDMX_HDR_PRS_STATE_T _SWDMX_OggMoveData(  SWDMX_INFO_T *prSwdmxInst,
        SWDMX_RANGE_LIST_T* prRangeList,
        UCHAR* pucBitstream ,
        UINT8 u1PageSegment,
        UINT8 u1StreamType,
        UINT8 u1TrackIdx,
        BOOL fgFreshPacket,
        UINT64 u8GranulAverg);


static SWDMX_HDR_PRS_STATE_T _SWDMX_OggAACHandle(  SWDMX_INFO_T *prSwdmxInst,
        SWDMX_RANGE_LIST_T* prRangeList,
        UINT8 u1AudTrick, UINT32 u4SampleSize,
        UINT32 u4Pts);



static BOOL _SWDMX_OggInitDmx(
    SWDMX_INFO_T* prSwdmxInst,
    UINT8 u1HdrType,
    BOOL fgEos);

static void _SWDMX_OggCalculatePts(SWDMX_INFO_T *prSwdmxInst,
                                   UINT8 u1StreamType,
                                   UINT8 u1TrackIdx,
                                   UINT64 u4GranulPos,
                                   UINT32*  pu4Pts);

static BOOL _SWDMX_OggHandleSpeedChange(SWDMX_INFO_T *prSwdmxInst, SWDMX_CUR_CMD_T* prCmd, SWDMX_RANGE_LIST_T* prRangeList);
static BOOL _SWDMX_SendOggAVEos(SWDMX_INFO_T* prSwdmxInst, SWDMX_RANGE_LIST_T* prRangeList);

static BOOL _SWDMX_SendOggEos(SWDMX_INFO_T* prSwdmxInst,
                              UINT8 u1HdrType);

static BOOL _SWDMX_OggGetSomeBytes(SWDMX_INFO_T *prSwdmxInst, UCHAR *pucBitstream,
                                   UCHAR **pucStream, UCHAR *pucData, UINT8 u1ByteNum);

static BOOL _SWDMX_OggSeek(SWDMX_INFO_T* prSwdmxInst,
                           SWDMX_RANGE_LIST_T* prRangeList,
                           UCHAR *pucBitstream,
                           UINT64 u8SeekTime,
                           UINT32 u4AvailSize, UINT32 *pu4ConsumedSize);

static BOOL _SWDMX_OggPageHdrPrs(SWDMX_INFO_T* prSwdmxInst,
                                 UCHAR *pucBitstream, UINT64 *pu8GranulePos,
                                 UINT8 *pu1StreamType, UINT8 *pu1Segment,
                                 UINT8 *pu1TrackIdx);

static BOOL _SWDMX_OggGetDataFromFeeder(SWDMX_INFO_T* prSwdmxInst,
                                        FEEDER_REQ_DATA_T *prFeederData);

static BOOL _SWDMX_OggCalculateRepeatFrame(SWDMX_INFO_T* prSwdmxInst, UINT32 u4Pts, UINT32 *pu4TimeCount);

static BOOL _SWDMX_OggResetFeeder(SWDMX_INFO_T* prSwdmxInst);

static void _SWDMX_OggVideoWakeUp(SWDMX_INFO_T* prSwdmxInst);

static BOOL _SWDMX_OggCheckHeader(UCHAR *pucBuf);

static BOOL _SWDMX_OggCheckOggType(SWDMX_INFO_T *prSwdmxInst ,
                                   UCHAR *pucBuf, UINT8 *pu1StreamType,
                                   UINT8 *pu1TrackIdx);

static void _SWDMX_OggConvertCharToUint(UCHAR *pucBuf, UINT32* pu4Data);

static UINT32 _SWDMX_OggSeekLen(SWDMX_INFO_T*prSwdmxInst, SEEK_DERECTION eSeekDerection,
                                 MM_RANGE_ELMT_IDXTBL_T * prIdxTbl);

static BOOL _SWDMX_OggFindHeader(SWDMX_INFO_T* prSwdmxInst,
                                 UCHAR *pucBitstream, UINT32* pu4ConsumedSize, UINT32 u4AvailSize);

static UINT32 _SWDMX_OggGetPageSizeAndFrameNum(SWDMX_INFO_T* prSwdmxInst, UCHAR *pucBitstream,
        UINT8 u1SegNum, UINT8 *u1FrameNum);

static BOOL _SWDMX_OggFindVideoPage(SWDMX_INFO_T* prSwdmxInst,
                                    UCHAR *pucBitstream, UINT32* pu4ConsumedSize, UINT32 u4AvailSize);


static BOOL _SWDMX_OggJudgeKey(SWDMX_INFO_T* prSwdmxInst,
                               UCHAR *pucBitstream, UINT8 u1SegNum, UINT8 *u1KeyNum,
                               BOOL fgFreshPage);

static BOOL _SWDMX_OggSeekKeyPage(SWDMX_INFO_T* prSwdmxInst,
                                  SWDMX_RANGE_LIST_T* prRangeList,
                                  UCHAR *pucBitstream, UINT32 u4AvailSize,
                                  UINT32 *pu4ConsumeSize, SEEK_DERECTION eSeekDerection,
                                  UINT32 u4SeekTime);
static BOOL _SWDMX_OggGetVorbisTblSize(SWDMX_INFO_T* prSwdmxInst, UINT32 *pu4VorbisTblSize);
static BOOL _SWDMX_OggSaveVorbisTbl(UINT8 u1SrcId,UINT8 u1TrackIdx, UCHAR* pucBitstream);
static UINT32 _SWDMX_OggGetAudFifoData(SWDMX_INFO_T *prSwdmxInst);
static BOOL _SWDMX_OggWaitVDecPlay(SWDMX_INFO_T *prSwdmxInst, UINT32 u4Flag);
static BOOL _SWDMX_OggGetLerpPos(SWDMX_INFO_T *prSwdmxInst, UINT64 *pu8CurPos);
static void _SWDMX_OggResetLerpPos(SWDMX_INFO_T *prSwdmxInst);


static void _SWDMX_OggPrintHex(UINT8 *pu1Addr, UINT32 u4Len)
{
    UINT32 u4Cnt = 0;

    while (u4Len--)
    {
        if (!(u4Cnt % 16))
        {
            if (u4Cnt)
            {
                Printf("\n");
            }
            Printf("%08X |", (UINT32)pu1Addr + u4Cnt);
        }
        Printf(" %02X", *pu1Addr++);
        u4Cnt++;
    }
    Printf("\n");
}

static BOOL _SWDMX_OggGetRingData(UINT8 *pu1RingStartAddr, UINT8 *pu1RingEndAddr, 
                                  UINT8 *pu1ReadAddr, UINT8 *pu1data, UINT32 u4ReadSize)
{
    if ((pu1ReadAddr < pu1RingStartAddr) ||
        (pu1ReadAddr >= pu1RingEndAddr) ||
        (u4ReadSize > pu1RingEndAddr - pu1RingStartAddr))
    {
        LOG(0, "%s [0x%X, 0x%X) @0x%X,L=0x%X!!!\n",
            pu1RingStartAddr, pu1RingEndAddr, pu1ReadAddr, u4ReadSize);
        ASSERT(0);
        return FALSE;
    }

    while ((pu1ReadAddr < pu1RingEndAddr) && (u4ReadSize))
    {
        *pu1data++ = *pu1ReadAddr++;
        u4ReadSize--;
    }

    while (u4ReadSize)
    {
        *pu1data++ = *pu1RingStartAddr++;
        u4ReadSize--;
    }
    return TRUE;
}

static UCHAR *_SWDMX_OggMemcpy(UCHAR *pucDest, UCHAR *pucSrc, UINT32 u4Len)
{
    if ((pucDest == NULL) || (pucSrc == NULL))
    {
        return NULL;
    }

    x_memcpy(pucDest, pucSrc, u4Len);
    pucDest += u4Len;
    
    return pucDest;
}


static BOOL _SWDMX_OggNoPreBufGetPidx(UINT8 u1SrcId, UINT8 u1AudIdx, UINT8 *pu1AudPid)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(pu1AudPid);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    if ((!prSwdmxOggInfo->fgEnAudio) ||
        (u1AudIdx != prSwdmxOggInfo->u1CurATrackIdx))
    {
        *pu1AudPid = DMX_AUDIO_HANDLER_NULL_PIDX;
        return FALSE;
    }
        
    *pu1AudPid = prSwdmxOggInfo->u1AudPid;

    return TRUE;
}

static BOOL _SWDMX_OggNoPreBufSetActivePidx(UINT8 u1SrcId, UINT8 u1AudIdx)
{
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    if (u1AudIdx == OGG_INVAILD_AUD_IDX)
    {
        DMX_MM_FlushBuffer(prSwdmxOggInfo->u1AudPid);
        return TRUE;
    }

    if (u1AudIdx != prSwdmxOggInfo->u1CurATrackIdx)
    {
        LOG(0, "%s:%d, Audio index is wrong!\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if (prSwdmxOggInfo->u4ADecType != ENUM_SWDMX_ADEC_VORBIS)
    {
        return TRUE;
    }
	
    if (!prSwdmxOggInfo->fgSaveVorbisTable)
    {
		LOG(0, "%s:%d, Vorbis table is not saved!\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    rDmxMMData.u1Idx = prSwdmxOggInfo->u1AudPid;
    rDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucSwdmxInternalBuf;
    rDmxMMData.u4BufEnd = rDmxMMData.u4BufStart + SWDMX_INTERNAL_BUF_SIZE;
    rDmxMMData.u4StartAddr = prSwdmxOggInfo->u4AudHeaderPtr[u1AudIdx];
    rDmxMMData.u4FrameSize = prSwdmxOggInfo->u4AudHdrLength[u1AudIdx];

    LOG(2, "%s:%d, move vorbis header to dmx 0x%X(0x%X)\n", __FUNCTION__, __LINE__,
        rDmxMMData.u4StartAddr, rDmxMMData.u4FrameSize);

    if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData))
    {
        LOG(0, "%s:%d, move data fail!\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    return TRUE;
}

static SWDMX_OGG_ABUF_OPTS rNoPreBufOpts = {
    .GetPidx               = _SWDMX_OggNoPreBufGetPidx,
    .SetActivePidx         = _SWDMX_OggNoPreBufSetActivePidx,
};

#ifndef CC_DISABLE_PREBUFFER
static BOOL _SWDMX_OggPreBufGetPidx(UINT8 u1SrcId, UINT8 u1AudIdx, UINT8 *pu1AudPid)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(pu1AudPid);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    if ((u1AudIdx < OGG_MAX_AUDIO_TRACK) && (u1AudIdx < prSwdmxOggInfo->u1TotalATrack))
    {
        *pu1AudPid = prSwdmxOggInfo->u1AudPreBufPid[u1AudIdx];
        return TRUE;
    }
    else if (u1AudIdx == OGG_INVAILD_AUD_IDX)
    {
        *pu1AudPid = DMX_AUDIO_HANDLER_NULL_PIDX;
        return TRUE;
    }

    return FALSE;
}

static BOOL _SWDMX_OggPreBufSetActivePidx(UINT8 u1SrcId, UINT8 u1AudIdx)
{
    BOOL fgRet = FALSE;
    UINT8 u1ActivePid = DMX_AUDIO_HANDLER_NULL_PIDX;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    if (!_SWDMX_OggPreBufGetPidx(prSwdmxInst->u1SwdmxId, u1AudIdx, &u1ActivePid))
    {
        LOG(0, "%s:%d, Audio index is wrong!\n", __FUNCTION__, __LINE__);
        return FALSE; 
    }

    if ((prSwdmxOggInfo->u4ADecType == ENUM_SWDMX_ADEC_VORBIS) &&
        (u1AudIdx < prSwdmxOggInfo->u1TotalATrack) &&
        (u1AudIdx < OGG_MAX_AUDIO_TRACK))
    {
	    if (!prSwdmxOggInfo->fgSaveVorbisTable)
		{
			LOG(0, "%s:%d, Vorbis table is not saved!\n", __FUNCTION__, __LINE__);
			return FALSE;
		}
	
        if (!prSwdmxOggInfo->u4AudHdrLength[u1AudIdx])
        {
            LOG(0, "%s:%d, Vorbis header length is 0\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
		
        fgRet = DMX_AudHandler_SetActivePidxEx(u1ActivePid, 
                    (UINT8 *)prSwdmxOggInfo->u4AudHeaderPtr[u1AudIdx],
                    prSwdmxOggInfo->u4AudHdrLength[u1AudIdx]);
    }
    else 
    {
        fgRet = DMX_AudHandler_SetActivePidx(u1ActivePid);
    }
 
    if (fgRet)
    {
        prSwdmxOggInfo->u1ActivePid = u1ActivePid;
    }

    return fgRet;
}

static BOOL _SWDMX_OggPreBufSeekUpdate(UINT8 u1SrcId, UINT8 u1AudIdx, UINT32 *pu4PTS)
{
    UINT8 u1AudPid = 0;
    UINT32 u4SeekPTS = 0;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(pu4PTS);

    if (!_SWDMX_OggPreBufGetPidx(prSwdmxInst->u1SwdmxId, u1AudIdx, &u1AudPid))
    {
        LOG(0, "%s:%d, Audio index is wrong!\n", __FUNCTION__, __LINE__);
        return FALSE; 
    }

    u4SeekPTS = *pu4PTS;

    return DMX_AudHandler_SeekAndUpdate(u1AudPid, u4SeekPTS, pu4PTS);
}

static VOID _SWDMX_OggPreBufFlush(UINT8 u1SrcId)
{
    UINT8 u1Idx = 0;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    for (u1Idx = 0; (u1Idx < prSwdmxOggInfo->u1TotalATrack) &&
         (u1Idx < OGG_MAX_AUDIO_TRACK); u1Idx++)
    {
        DMX_MM_FlushBuffer(prSwdmxOggInfo->u1AudPreBufPid[u1Idx]);
    }
}

static BOOL _SWDMX_OggPreBufInit(UINT8 u1SrcId, DMX_MM_T *prDmxMMInfo) 
{
    UINT32 i = 0;
    UINT32 u4AudTrickSize = 0;
    UINT32 u4AudPreFifoAddr = 0;
    UINT32 u4AudPreFifoSize = 0;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    UINT32 u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(prDmxMMInfo);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    DMX_AudHandler_SetEnable(TRUE);

    if (!DMX_AudHandler_Set(prSwdmxOggInfo->u1AudPid, TRUE))
    {
        LOG(0, "%s:%d, DMX_AudHandler_Set fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if (!_SWDMX_GetAudioPreFifo(prSwdmxInst->u1SwdmxId, &u4AudPreFifoAddr, &u4AudPreFifoSize))
    {
        LOG(0, "%s:%d, _SWDMX_GetAudioPreFifo fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    //start address 16 bytes align
    if ((u4AudPreFifoAddr % 16))
    {
        u4AudPreFifoAddr = 16 - (u4AudPreFifoAddr % 16) + u4AudPreFifoAddr;
        u4AudPreFifoSize = u4AudPreFifoSize - (16 - (u4AudPreFifoAddr % 16));
    }
    //Dynamic allocate audio pre-buffer
    if (prSwdmxOggInfo->u1TotalATrack)
    {
        u4AudTrickSize = u4AudPreFifoSize / prSwdmxOggInfo->u1TotalATrack;
    }
    else
    {
        u4AudTrickSize = u4AudPreFifoSize / 4;
    }
    //audio fifo size 16 bytes align
    u4AudTrickSize = u4AudTrickSize - u4AudTrickSize % 16;
    u4AudTrickSize = (u4AudTrickSize > prDmxMMInfo->u4BufSize) ? prDmxMMInfo->u4BufSize : u4AudTrickSize;

    for (i = 0; i < prSwdmxOggInfo->u1TotalATrack; i++)
    {
        prDmxMMInfo->fgAllocBuf = FALSE;
        prDmxMMInfo->u4BufSize = u4AudTrickSize;
        prDmxMMInfo->u4BufAddr = u4AudPreFifoAddr;
        prSwdmxOggInfo->u1AudPreBufPid[i] =
            DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

        DMX_AudHandler_SetQSize(prSwdmxOggInfo->u1AudPreBufPid[i], 480);
        if (!DMX_MM_Set(prSwdmxOggInfo->u1AudPreBufPid[i], u4Flag, prDmxMMInfo))
        {
            LOG(0, "%s:%d, DMX_MM_Set fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
        if (!DMX_AudHandler_Set(prSwdmxOggInfo->u1AudPreBufPid[i], FALSE))
        {
            LOG(0, "%s:%d, DMX_AudHandler_Set fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
        u4AudPreFifoAddr = u4AudPreFifoAddr + u4AudTrickSize;
    }
    _SWDMX_OggPreBufSetActivePidx(prSwdmxInst->u1SwdmxId, OGG_INVAILD_AUD_IDX);

    return TRUE;
}

static BOOL _SWDMX_OggPreBufDeinit(UINT8 u1SrcId, DMX_MM_T *prDmxMMInfo) 
{
    UINT32 i = 0;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(prDmxMMInfo);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    for (i = 0; i < prSwdmxOggInfo->u1TotalATrack; i++)
    {
        if (!DMX_MM_Set(prSwdmxOggInfo->u1AudPreBufPid[i],
                DMX_MM_FLAG_VALID, prDmxMMInfo))
        {
            LOG(0, "%s:%d, DMX_MM_Set fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
        DMX_MM_Free(prSwdmxOggInfo->u1AudPreBufPid[i]);
        DMX_MUL_FreePidx(prSwdmxOggInfo->u1AudPreBufPid[i]);
        DMX_AudHandler_Free(prSwdmxOggInfo->u1AudPreBufPid[i]);
    }
    DMX_AudHandler_SetEnable(FALSE);

    return TRUE;
}

static BOOL _SWDMX_OggPreBufCancelSearchStartCode(UINT8 u1SrcId, DMX_MM_T *prDmxMMInfo) 
{
    UINT32 i = 0;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    UINT32 u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(prDmxMMInfo);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    prDmxMMInfo->fgSearchStartCode = FALSE;
    for (i = 0; i < prSwdmxOggInfo->u1TotalATrack; i++)
    {
        if (!DMX_MM_Set(prSwdmxOggInfo->u1AudPreBufPid[i], 
                    u4Flag, prDmxMMInfo))
        {
            LOG(0, "%s:%d, DMX_MM_Set fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    }

    return TRUE;
}

static SWDMX_OGG_ABUF_OPTS rPreBufOpts = {
    .GetPidx               = _SWDMX_OggPreBufGetPidx,
    .SetActivePidx         = _SWDMX_OggPreBufSetActivePidx,
    .SeekUpdate            = _SWDMX_OggPreBufSeekUpdate,
    .Flush                 = _SWDMX_OggPreBufFlush,
    .Init                  = _SWDMX_OggPreBufInit,
    .Deinit                = _SWDMX_OggPreBufDeinit,
    .CancelSearchStartCode = _SWDMX_OggPreBufCancelSearchStartCode,
};
#endif

static BOOL _SWDMX_OggABufGetPidx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1AudIdx, UINT8 *pu1AudPid)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.GetPidx)
    {
        return TRUE;
    }

    return prSwdmxOggInfo->rABufOpts.GetPidx(prSwdmxInst->u1SwdmxId, u1AudIdx, pu1AudPid);
}

static BOOL _SWDMX_OggABufSetActivePidx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1AudIdx)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.SetActivePidx)
    {
        return TRUE;
    }

    return prSwdmxOggInfo->rABufOpts.SetActivePidx(prSwdmxInst->u1SwdmxId, u1AudIdx);
}

static BOOL _SWDMX_OggABufSeekUpdate(SWDMX_INFO_T *prSwdmxInst, UINT8 u1AudIdx, UINT32 *pu4PTS)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.SeekUpdate)
    {
        return FALSE;
    }

    return prSwdmxOggInfo->rABufOpts.SeekUpdate(prSwdmxInst->u1SwdmxId, u1AudIdx, pu4PTS);
}

static VOID _SWDMX_OggABufFlush(SWDMX_INFO_T *prSwdmxInst)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.Flush)
    {
        return;
    }

    prSwdmxOggInfo->rABufOpts.Flush(prSwdmxInst->u1SwdmxId);
}

static BOOL _SWDMX_OggABufInit(SWDMX_INFO_T *prSwdmxInst, DMX_MM_T *prDmxMMInfo) 
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.Init)
    {
        return TRUE;
    }

    return prSwdmxOggInfo->rABufOpts.Init(prSwdmxInst->u1SwdmxId, prDmxMMInfo);
}

static BOOL _SWDMX_OggABufDeinit(SWDMX_INFO_T *prSwdmxInst, DMX_MM_T *prDmxMMInfo) 
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.Deinit)
    {
        return TRUE;
    }

    return prSwdmxOggInfo->rABufOpts.Deinit(prSwdmxInst->u1SwdmxId, prDmxMMInfo);
}

static BOOL _SWDMX_OggABufCancelSearchStartCode(SWDMX_INFO_T *prSwdmxInst, DMX_MM_T *prDmxMMInfo) 
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    if (!prSwdmxOggInfo->rABufOpts.CancelSearchStartCode)
    {
        return TRUE;
    }

    return prSwdmxOggInfo->rABufOpts.CancelSearchStartCode(prSwdmxInst->u1SwdmxId, prDmxMMInfo);
}

static BOOL _SWDMX_OggABufOptsRegister(SWDMX_INFO_T *prSwdmxInst, SWDMX_OGG_ABUF_OPTS *prABufOpts) 
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    SWDMX_OGG_VERIFY_NULL_ERROR(prABufOpts);

    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    prSwdmxOggInfo->rABufOpts = *prABufOpts;

    return TRUE;
}



BOOL _SWDMX_OggSetDecoderInfo(UINT8 u1SrcId,
                              UINT8 u1Type,
                              UINT32 u4Para1,
                              UINT32 u4Para2,
                              UINT32 u4Para3)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_INFO_T *prSwdmxOggInfo;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);

    if (u1Type == eSWDMX_SET_VIDEOTYPE)
    {
        switch (u4Para1)
        {
#if defined(CC_Ogg_MPEG1_DISABLE) && defined(CC_Ogg_MPEG2_DISABLE)
            case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
                return FALSE;
#endif
#if defined(CC_Ogg_MPEG4_DISABLE)
            case ENUM_SWDMX_VDEC_MPEG4: 
                return FALSE;
#endif
#if defined(CC_Ogg_DX311_DISABLE)
            case ENUM_SWDMX_VDEC_DX311: 
                return FALSE;
#endif
#if defined(CC_Ogg_H264_DISABLE)
            case ENUM_SWDMX_VDEC_H264: 
                return FALSE;
#endif
#if defined(CC_Ogg_VC1_DISABLE)
            case ENUM_SWDMX_VDEC_VC1: 
                return FALSE;
#endif
#if defined(CC_Ogg_MJPEG_DISABLE)
            case ENUM_SWDMX_VDEC_MJPEG: 
                return FALSE;
#endif
            default:
                prSwdmxOggInfo->u4VDecType = u4Para1;
                return TRUE;
        }
    }
    else if (u1Type == eSWDMX_SET_AUDIOTYPE)
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType = u4Para1;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudSampleRate = u4Para2;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudChannelNs = u4Para3;
        return TRUE;
    }
    else if (u1Type == eSWDMX_PRINT_INTERNAL_STATUS)
    {
        LOG(0, "Speed: %d\n", prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed);
        return TRUE; 
    }

    return FALSE;
}

BOOL _SWDMX_OggGetInfo(UINT8 u1SrcId,
                       UINT8 u1Type,
                       UINT32* pu4Para1,
                       UINT32* pu4Para2,
                       UINT32* pu4Para3)
{
    UINT64 *pu8Tmp;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(6, "%s source id = %d, u1Type = %d\n", __FUNCTION__, u1SrcId, u1Type);

    if (u1Type == eSWDMX_GET_CURRENT_POSITION)
    {
        if (pu4Para1 == NULL)
        {
            LOG(1, "_SWDMX_OggGetInfo pu4Para1 = NULL\n");
            return FALSE;
        }
        if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            pu8Tmp = (UINT64*)pu4Para1;
            *pu8Tmp = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos;
            if (prSwdmxInst->u4LogFilter & check_getinfo)
            {
                LOG(1, "_SWDMX_OggGetInfo Get current position: 0x%x\n", 
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos);
            }
        }
        else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
        {
            pu8Tmp = (UINT64*)pu4Para1;
            if((prSwdmxInst->eCurFMT == SWDMX_FMT_OGG_AUDIO_ES) && (prSwdmxInst->i4PlaySpeed < 0))
            {
                UINT64 u8PreviousUIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos;
                UINT64 u8CurMoviPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos;
                
                if (u8PreviousUIPos < u8CurMoviPos )
                {
                    *pu8Tmp = u8PreviousUIPos;
                }
                else
                {
                    *pu8Tmp = u8CurMoviPos;
                }
                if (prSwdmxInst->u4LogFilter & check_getinfo)
                {
                    LOG(1, "---GetInfo *pu8Tmp 0x%llx u8CurMoviPos 0x%llx u8PreviousUIPos 0x%llx\n",
                            *pu8Tmp, u8CurMoviPos, u8PreviousUIPos);
                }
                // seek end
                if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FileOffset== prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos)
                {
                    *pu8Tmp = 0;
                }
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos = *pu8Tmp;
            }
            else
            {
                UINT32 u4DataSize = 0;

                u4DataSize = _SWDMX_OggGetAudFifoData(prSwdmxInst);
                u4DataSize *= prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack;
                
                if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos > u4DataSize)
                {
                    *pu8Tmp = (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos - u4DataSize);
                }
                else
                {
                    *pu8Tmp = 0;
                }
                _SWDMX_OggGetLerpPos(prSwdmxInst, pu8Tmp);
                if (*pu8Tmp < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos)
                {
                    *pu8Tmp = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos = *pu8Tmp;
                }
                if (prSwdmxInst->u4LogFilter & check_getinfo)
                {
                    LOG(1, "---GetInfo *pu8Tmp 0x%llx u8CurMoviPos 0x%llx\n",
                        *pu8Tmp, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos);
                }
            }
        }
        return TRUE;
    }
    else if (u1Type == eSWDMX_GET_AUDIO_ACTIVE_PIDX)
    {
        *pu4Para1 = _SWDMX_OggGetAudFifoData(prSwdmxInst);
        return TRUE;
    }
    else if (u1Type == eSWDMX_GET_IS_TRICK)
    {
        *pu4Para1 = TRUE;
		if (prSwdmxInst->eCurFMT == SWDMX_FMT_OGM)
		{
			if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
			{
				*pu4Para1 = FALSE;
				LOG(0, "OGM no video cannot support seek/trick\n");
			}
		}
        return TRUE;
    }
    return FALSE;
}

BOOL _SWDMX_OggVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    UINT32 u4Addr;
    SWDMX_INFO_T *prSwdmxInst = NULL;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;

    if (prPes == NULL)
    {
        LOG(1, "%s prPes = NULL\n", __FUNCTION__);
        return FALSE;
    }
        
    prSwdmxInst = (SWDMX_INFO_T*)prPes->pvInstanceTag;
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    
    if (prSwdmxInst->u4LogFilter & check_original_video_pts)
    {
        LOG(1, "VO PTS=0x%x RP=0x%x WP=0x%x Pos=0x%llx\n",
                prPes->u4Pts, prPes->u4FrameAddr, prPes->u4Wp,
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos);
    }
    
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    rPesInfo.u4PTS = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastVidPts;
    rPesInfo.u4DTS = prPes->u4Dts;
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
    rPesInfo.fgDtsValid = prPes->fgPtsDts;
    rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
    rPesInfo.fgGop = prPes->fgGop;
    rPesInfo.fgEos = prPes->fgEOS;
    rPesInfo.ucEsId = prSwdmxInst->u1SwdmxId;
    rPesInfo.u8Offset = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;

    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastVidPts == 0) &&
            (!rPesInfo.fgEos))
    {
        LOG(5, "this is not a frame\n");
        return TRUE;
    }
    if (prSwdmxOggInfo->u4VDecType == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucMpvId = VLD1; // video format all use VLD expect jpeg
    }
    else
    {
        rPesInfo.ucMpvId = VLD0; // video format all use VLD expect jpeg
    }
    // Pts is valid all the time
    rPesInfo.fgDtsValid = TRUE;

#ifdef CC_VDEC_FMT_DETECT
    rPesInfo.u4DmxFrameType = prPes->u4FrameType;
#endif
    
    if (rPesInfo.fgEos)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0)
        {
            rPesInfo.fgBackward = TRUE;
        }
        rPesInfo.u4FifoStart = 0;
        rPesInfo.u4FifoEnd = 0;
        rPesInfo.u4VldReadPtr = 0;
    }
    else if (SWDMX_OGG_NOT_NEED_SEARCH_START_CODE(prSwdmxOggInfo->u4VDecType))
    {
        rPesInfo.u4VldReadPtr += 4;
        if (rPesInfo.u4VldReadPtr >= rPesInfo.u4FifoEnd)
        {
            rPesInfo.u4VldReadPtr -= rPesInfo.u4FifoEnd - rPesInfo.u4FifoStart;
        }
    }

    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType == ENUM_SWDMX_VDEC_H264) &&
            !rPesInfo.fgEos)
    {
        u4Addr = (UINT32)(prPes->au1PicInfo);
        rPesInfo.ucPicType = *(UINT8*)u4Addr;

        u4Addr = prPes->u4FrameAddr + 4;
        if (u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;

        if (( rPesInfo.ucPicType & 0x1f) == 5)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts = rPesInfo.u4PTS;

        }
    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType == ENUM_SWDMX_VDEC_MPEG4) &&
             !rPesInfo.fgEos)
    {
        UCHAR *pucBuf;

        u4Addr = (UINT32)(prPes->au1PicInfo);
        pucBuf = (UCHAR*)u4Addr;

        if (pucBuf[0] == 0xB6)
        {
            u4Addr = (UINT32)(prPes->au1PicInfo + 1);

            pucBuf = (UCHAR*)u4Addr;
            switch (pucBuf[0] >> 6)
            {
                case 0:
                    rPesInfo.ucPicType  = I_TYPE;
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts = rPesInfo.u4PTS;
                    break;
                case 1: case 3:
                    rPesInfo.ucPicType  = P_TYPE;
                    break;
                case 2:
                    rPesInfo.ucPicType = B_TYPE;
                    break;
                default:
                    rPesInfo.ucPicType = UNKNOWN_TYPE;
                    break;
            }
        }
        else if ((pucBuf[0] >= 0x20) &&
                 (pucBuf[0] <= 0x2f))
        {
            rPesInfo.fgSeqHdr = TRUE;
        }
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucPicType = I_TYPE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts = rPesInfo.u4PTS;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType == ENUM_SWDMX_VDEC_MPEG1_MPEG2)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsFirstFrame)
        {
            UNUSED(VDEC_SetTotalTime(prSwdmxInst->u1VdecId, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TotalTime));
        }
        switch (prPes->u4FrameType)
        {
            case PIC_TYPE_I:
                rPesInfo.ucPicType = MPV_I_TYPE;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts = rPesInfo.u4PTS;
                break;

            case PIC_TYPE_P:
                rPesInfo.ucPicType = MPV_P_TYPE;
                break;

            case PIC_TYPE_B:
                rPesInfo.ucPicType = MPV_B_TYPE;
                break;

            default:
#ifndef CC_VDEC_FMT_DETECT
                ASSERT(0);
#endif
                break;
        }
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType == (UINT32)ENUM_SWDMX_VDEC_H263)
    {
        UCHAR *pucBuf;

        u4Addr = (UINT32)(prPes->au1PicInfo + 1);
        pucBuf = (UCHAR*)u4Addr;

        switch (pucBuf[0] & 0x02)
        {
            case 0:
                rPesInfo.ucPicType = I_TYPE;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts = rPesInfo.u4PTS;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                break;
            case 2:
                rPesInfo.ucPicType = P_TYPE;
                break;
            default:
                rPesInfo.ucPicType = UNKNOWN_TYPE;
                break;
        }
    }
    else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType == ENUM_SWDMX_VDEC_DX311)
    {
        UCHAR *pucBuf;

        u4Addr = (UINT32)(prPes->au1PicInfo);
        pucBuf = (UCHAR*)u4Addr;

        rPesInfo.fgDtsValid = TRUE;
        
        if ((pucBuf[0] >> 6) & 0x1)
        {
            rPesInfo.ucPicType  = P_TYPE;
        }
        else
        {
            rPesInfo.ucPicType = I_TYPE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts = rPesInfo.u4PTS;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
        }
    }

    rPesInfo.u8OffsetI = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LastIPos;
    rPesInfo.rExtra.u8PTSI = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastIPts;
     x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
    VDEC_SendEs((void*)&rPesInfo);
    
    if (prSwdmxInst->u4LogFilter & check_video_pts)
    {
        LOG(1, "VC PTS=0x%X RP=0x%X PicType(0x%x) Pos=0x%llX\n",
                rPesInfo.u4PTS, rPesInfo.u4VldReadPtr, rPesInfo.ucPicType,
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos);
    }

    return TRUE;
}


BOOL _SWDMX_OggAudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet;
    DMX_AUDIO_PES_T rPes;
    SWDMX_INFO_T *prSwdmxInst = NULL;
    
    if (prPes == NULL)
    {
        LOG(1, "%s prPes = NULL\n", __FUNCTION__);
        return FALSE;
    }
    
    prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    if (prSwdmxInst->u4LogFilter & check_original_audio_pts)
    {
        LOG(1, "AO PTS=0x%x RP=0x%x WP=0x%x Pos=0x%llx\n",
                prPes->u4Pts, prPes->u4PesRp, prPes->u4Wp,
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos);
    }
   
    x_memset(&rPes, 0, sizeof(rPes));
    x_memcpy(&rPes, prPes, sizeof(DMX_AUDIO_PES_T));
    rPes.fgForward = (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 0) ? TRUE : FALSE;
#ifdef SWDMX_DUMP_DATA
    _SWDMX_DumpAFifo(prSwdmxInst, rPes.u4Wp);
#endif
    fgRet = AUD_SendAudioPes(&rPes);

    if (prSwdmxInst->u4LogFilter & check_audio_pts)
    {
        LOG(1, "AC PTS=0x%X RP=0x%X WP=0x%X Pos=0x%llX EOS=%u\n",
               rPes.u4Pts, rPes.u4PesRp, rPes.u4Wp,
               prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos,
               rPes.fgEOS);
        _SWDMX_OggPrintHex((UINT8 *)VIRTUAL(rPes.u4PesRp), 16);
    }
    
    return fgRet;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL _SWDMX_OggInit(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType)
{
    // Initialize feeder
    //FeederOpen(FEEDER_PROGRAM_SOURCE);
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    UNUSED(eType);
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);
    // Default some values
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4PrevAStrmID = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AStrmID = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudSampleRate = 48000;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = 0xFFFFFFFF;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts = 0xFFFFFFFF;

    x_memset((void*)&stKeyinfo, 0, sizeof(KEY_INFO));
    
    //DDI_BUF_ALIGNMENT == 16 and
    //for _dmx_mm_movedata  Meet the alignment and packet size (188 bytes) requirements.
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_AAC &&
            prSwdmxInst->pucAudBuf == 0)
    {
        prSwdmxInst->pucAudBuf =
            (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(AAC_ALLOC_SIZE, 16));
        if (prSwdmxInst->pucAudBuf == 0)
        {
            LOG(1, "Allocate AAC buffer failed!\n");
            ASSERT(0);
            return FALSE;
        }
    }

    DMX_SetToDecoder(TRUE);

    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgRepeatRequest = TRUE;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsFirstFrame = TRUE;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable = FALSE;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid = 0xFF;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid  = 0xFF;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgNotSendPacket = FALSE;
    _SWDMX_OggResetLerpPos(prSwdmxInst);

#ifndef CC_DISABLE_PREBUFFER
    if (prSwdmxInst->ui8ForceEnablePrebuffer != PREBUFFER_FORCE_DISABLE)
    {
        _SWDMX_OggABufOptsRegister(prSwdmxInst, &rPreBufOpts);
        LOG(0, "Prebuf.\n");
    }
    else
#endif
    {
        _SWDMX_OggABufOptsRegister(prSwdmxInst, &rNoPreBufOpts);
        LOG(0, "No prebuf.\n");
    }

    return TRUE;
}

BOOL _SWDMX_OggSetStrmID(
    UINT8 u1SrcId,
    UINT8 u1HdrType,
    UINT32 u4StrmID,
    UINT32 u4StrmSubID)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_INFO_T *prSwdmxOggInfo;
    
    UNUSED(u4StrmSubID);
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    LOG(3, "%s source id = %d, type = %d\n", __FUNCTION__, u1SrcId, u1HdrType);
    
    switch (u1HdrType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxOggInfo->u4VStrmID = u4StrmID;
            break;
        case eSWDMX_STRM_TYPE_AUD:
            if (prSwdmxOggInfo->u4AStrmID != u4StrmID)
            {
                prSwdmxOggInfo->u4PrevAStrmID = prSwdmxOggInfo->u4AStrmID;
                prSwdmxOggInfo->u4AStrmID = u4StrmID;
            }
            break;
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_OggEnableStrm(
    UINT8 u1SrcId,
    UINT8 u1StrmType,
    VOID* pfnCbFunc)
{
    UINT32 i = 0;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_INFO_T *prSwdmxOggInfo;
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    LOG(3, "%s source id = %d, type = %d\n", __FUNCTION__, u1SrcId, u1StrmType);

    switch (u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxOggInfo->fgEnVideo = TRUE;
            break;
        case eSWDMX_STRM_TYPE_AUD:
            for (i=0; (i<prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) && (i<OGG_MAX_AUDIO_TRACK); i++)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[i])
                {
                    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[i]->t_strm_id.u.t_stm_ogg_id ==
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AStrmID)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx = i;
                        LOG(5, "Cur Track Idx = %ld\n", i);
                    }
                }
            }

            if ((prSwdmxOggInfo->u4PrevAStrmID != 0xFFFFFFFF)&&(prSwdmxOggInfo->u4PrevAStrmID != 0)&&
                 (prSwdmxOggInfo->u4PrevAStrmID != prSwdmxOggInfo->u4AStrmID) && prSwdmxOggInfo->fgBeginToPlay)
            {
                prSwdmxOggInfo->fgChgATrack = TRUE;
                LOG(5, "Enable audio ,change audio track\n");
            }
            prSwdmxOggInfo->u4PrevAStrmID = prSwdmxOggInfo->u4AStrmID;
            prSwdmxOggInfo->fgEnAudio = TRUE;
            break;
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_OggDisableStrm(
    UINT8 u1SrcId,
    UINT8 u1StrmType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(3, "%s source id = %d, type = %d\n", __FUNCTION__, u1SrcId, u1StrmType);

    switch (u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo = FALSE;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
            {
                _SWDMX_OggVideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
            }
            break;
        case eSWDMX_STRM_TYPE_AUD:
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio = FALSE;
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, OGG_INVAILD_AUD_IDX);
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

static BOOL _SWDMX_OggInitDmx(
    SWDMX_INFO_T* prSwdmxInst,
    UINT8 u1HdrType,
    BOOL fgEos)
{
    BOOL fgRet = FALSE;
    UINT32 u4Flag = 0;
    DMX_MM_T rDmxMMInfo;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, prSwdmxInst->u1SwdmxId);

    x_memset((void*)&rDmxMMInfo, 0, sizeof(DMX_MM_T));

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVDmx)
        {
            UINT32 u4VidFifoAddr, u4VidFifoSize;

            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid =
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            
            _SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &u4VidFifoAddr, &u4VidFifoSize);

            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG);

            rDmxMMInfo.fgAllocBuf = FALSE;
            rDmxMMInfo.u4BufAddr = u4VidFifoAddr;
            rDmxMMInfo.u4BufSize = u4VidFifoSize;
            rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
        }

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;

        u4Flag |= DMX_MM_FLAG_SEARCH_START_CODE;

        if (SWDMX_OGG_NOT_NEED_SEARCH_START_CODE(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType))
        {
            UINT32 u4Addr = 0, u4Size = SWDMX_OGG_PIC_HDR_BUF_LEN, u4Align = 16;
            
            if (!SWDMX_GetInfo(prSwdmxInst->u1SwdmxId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align) ||
                    u4Addr == 0)
            {
                LOG(0, "%s(%d): eSWDMX_GET_INTERNAL_BUF NG\n", __FUNCTION__, __LINE__);
                return FALSE;
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.pu1PicHdrBuf = (UINT8 *)u4Addr;
            LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
            rDmxMMInfo.fgSearchStartCode = FALSE;
        }
        else
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.pu1PicHdrBuf = NULL;
            LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
            rDmxMMInfo.fgSearchStartCode = TRUE;
        }

        if (fgEos)
        {
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
        }

        fgRet = DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid, u4Flag, &rDmxMMInfo);
    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        UINT8 u1DmxPid;
        UINT32 u4AudFifoAddr, u4AudFifoSize;

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_AUDIO;

        if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnADmx)
        {
            DMX_SetToDecoder(TRUE);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid =
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

            _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);

            u1DmxPid = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid;

            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG);

            rDmxMMInfo.fgAllocBuf = FALSE;
            rDmxMMInfo.u4BufAddr = u4AudFifoAddr;
            rDmxMMInfo.u4BufSize = u4AudFifoSize;
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;

            if (!DMX_MM_Set(u1DmxPid, u4Flag, &rDmxMMInfo))
            {
                LOG(0, "%s:%d, DMX_MM_Set fail\n", __FUNCTION__, __LINE__);
                return FALSE;
            }
            
            fgRet = _SWDMX_OggABufInit(prSwdmxInst, &rDmxMMInfo);
        }
        else if (fgEos)
        {
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
            if (!DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid, u4Flag, &rDmxMMInfo))
            {
                LOG(0, "%s:%d, DMX_MM_Set fail\n", __FUNCTION__, __LINE__);
                return FALSE;
            }

            fgRet = _SWDMX_OggABufCancelSearchStartCode(prSwdmxInst, &rDmxMMInfo);
        }
    }

    return fgRet;
}

VOID _SWDMX_OggSetRange(UINT8 u1SrcId,
                        SWDMX_RANGE_LIST_T *prRangeList,
                        UINT32 u4VidStrmID,
                        UINT32 u4AudStrmID,
                        UINT32 u4SubTitleID)
{
    UINT8 u1ATrack = 0;
    UINT64 u8TempValue = 0;
    UINT64 u8Remander = 0;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    MM_RANGE_ELMT_IDXTBL_T *pBuldIdxTbl;
    MM_RANGE_DECODER_T* pDecodrtInfo;
    
    SWDMX_OGG_VERIFY_NULL(prSwdmxInst);

    LOG(5, "%s source id = %d\n", __FUNCTION__, u1SrcId);

    if ((prRangeList == NULL) || (prRangeList->prDmxRangeInfo == NULL))
    {
        LOG(0, "Demuxer Range is NULL\n");
        return;
    }

    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prDmxRangeInfo = prRangeList->prDmxRangeInfo;
    pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)prRangeList->prDmxRangeInfo->pt_idxtbl_lst;
    while (pBuldIdxTbl != NULL)
    {
        if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_AUDIO && 
            u1ATrack < OGG_MAX_AUDIO_TRACK)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[u1ATrack] = pBuldIdxTbl;
            LOG(5, "_SWDMX_OggSetRange Audio Index table = 0x%08x\n",
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[u1ATrack]));
            u1ATrack++;
        }
        else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_VIDEO)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl = pBuldIdxTbl;
            LOG(5, "_SWDMX_OggSetRange Video Index table = 0x%04x\n",
                    (UINT32)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl));
            LOG(5, "_SWDMX_OggSetRange, avg_frame_num:0x%x, avg_packet_size:0x%x\n",
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui4_avg_frame_num,
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui4_avg_packet_size);
            LOG(5, "_SWDMX_OggSetRange, timeunit:0x%llx\n",
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui8_timeunit);
        }
        pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)pBuldIdxTbl->pv_next_tbl;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.ui8_data_offset = prRangeList->prDmxRangeInfo->ui8_fileoffset;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz = prRangeList->prDmxRangeInfo->z_range_sz;
    }
    // Set the number of total audio tracks
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack = u1ATrack;
    if (prRangeList->prDmxRangeInfo->pt_aud_decoder_info &&
        prRangeList->prDmxRangeInfo->pt_aud_decoder_info->b_has_vorbis_info)
    {
        pDecodrtInfo = (MM_RANGE_DECODER_T*)prRangeList->prDmxRangeInfo->pt_aud_decoder_info;
        u1ATrack = 0;
        while (pDecodrtInfo != NULL)
        {
            if (u1ATrack < OGG_MAX_AUDIO_TRACK)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.AudVorbisOffset[u1ATrack] = pDecodrtInfo->t_audio_vorbis_offset;
                u1ATrack++;
            }
            pDecodrtInfo = (MM_RANGE_DECODER_T*)pDecodrtInfo->pt_next;
        }
    }
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TotalTime = prRangeList->prDmxRangeInfo->ui8_pb_duration;
    u8TempValue = (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz -
                  prSwdmxInst->rFmtInfo.rSwdmxOggInfo.ui8_data_offset) * 90000; 
    if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TotalTime)
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond = 
            u8Div6432(u8TempValue, (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TotalTime), &u8Remander);
    }
    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    LOG(5, "_SWDMX_OggSetRange range size:0x%llx, file offset:0x%llx, duration:0x%llx\n",
            prRangeList->prDmxRangeInfo->z_range_sz,
            prRangeList->prDmxRangeInfo->ui8_fileoffset, 
            prRangeList->prDmxRangeInfo->ui8_pb_duration);  
}

static BOOL _SWDMX_OggPreparePlay(SWDMX_INFO_T *prSwdmxInst)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);
    
    LOG(5, "%s source id = %d\n", __FUNCTION__, prSwdmxInst->u1SwdmxId);

    // get feeder info
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return FALSE;
    }

    if ((prSwdmxOggInfo->u4ADecType == ENUM_SWDMX_ADEC_VORBIS) &&
        (!prSwdmxOggInfo->fgSaveVorbisTable))
    {
        LOG(7, "Vorbis table is not saved.\n");
        return TRUE;
    }

    // last memory
    if ((prSwdmxOggInfo->prDmxRangeInfo != NULL) &&
        ((prSwdmxOggInfo->prDmxRangeInfo->t_vid_start_render_pts != 0) ||
        (prSwdmxOggInfo->prDmxRangeInfo->t_aud_start_render_pts != 0)))
    {
        UINT32  u4SeekPts = 0;
        
        LOG(5,"Last memory case 1!\n");

        prSwdmxOggInfo->u8FileOffset = prSwdmxOggInfo->prDmxRangeInfo->ui8_fileoffset;
        prSwdmxOggInfo->u4CurDmxFeederRPtr = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);

        if (prSwdmxOggInfo->fgEnVideo)
        {
            u4SeekPts = (UINT32)prSwdmxOggInfo->prDmxRangeInfo->t_vid_start_render_pts;
            prSwdmxOggInfo->eActStatus = eSWDMX_OGG_FIND_HEADER;
            prSwdmxOggInfo->u8FilePosBeforeSeek = prSwdmxOggInfo->u8CurDmxMoviPos;
            prSwdmxOggInfo->fgIsSeek = TRUE;
            prSwdmxOggInfo->u4ASeekPts = u4SeekPts;
            prSwdmxOggInfo->u4VSeekPts = u4SeekPts;
            prSwdmxOggInfo->u4ChangeAudPts = u4SeekPts;
            prSwdmxOggInfo->fgSpeedChange = TRUE;
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4SeekPts);
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            _SWDMX_OggWaitVDecPlay(prSwdmxInst, 0);
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxOggInfo->fgVDecIsPause = TRUE;
            prSwdmxOggInfo->fgSpeedChange = TRUE;
            LOG(5,"Last memory, t_vid_start_render_pts=0x%x\n", u4SeekPts);
        }
        else if (prSwdmxOggInfo->fgEnAudio)
        {
            UINT64  u8SeekPos = 0;
            UINT64  u8Remainder = 0;

            u4SeekPts = (UINT32)prSwdmxOggInfo->prDmxRangeInfo->t_aud_start_render_pts;
            if (prSwdmxOggInfo->u4TotalTime > 0)
            {
                u8SeekPos = _SWDMX_Div6464((prSwdmxOggInfo->prDmxRangeInfo->z_range_sz - 
                                prSwdmxOggInfo->prDmxRangeInfo->ui8_fileoffset)*(u4SeekPts),
                                (UINT64)(prSwdmxOggInfo->u4TotalTime), &u8Remainder);
            }
            else
            {
                u8SeekPos = 0;
            }
            u8SeekPos += prSwdmxOggInfo->prDmxRangeInfo->ui8_fileoffset;
            prSwdmxOggInfo->fgIsSeek = FALSE;
            prSwdmxOggInfo->u8CurDmxMoviPos = u8SeekPos;
            prSwdmxOggInfo->u8CurMoviPos = prSwdmxOggInfo->u8CurDmxMoviPos;
            prSwdmxOggInfo->u4ASeekPts = u4SeekPts;
            LOG(5,"Last memory, t_aud_start_render_pts=0x%x\n", u4SeekPts);
        }

        if (prSwdmxOggInfo->fgEnAudio)
        {
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxOggInfo->u1CurATrackIdx);
			prSwdmxOggInfo->fgSetActivePid = TRUE;
        }
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, u4SeekPts - 45000);
        prSwdmxOggInfo->fgBeginToPlay = TRUE;
    }
    else if ((prSwdmxOggInfo->prDmxRangeInfo != NULL) &&
             (prSwdmxOggInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info != 0))
    {
        LOG(5,"Last memory case 2!\n");
        prSwdmxOggInfo->u8CurDmxMoviPos =
            prSwdmxOggInfo->prDmxRangeInfo->t_start_pos.ui8_i_frame_position;
        prSwdmxOggInfo->u8CurMoviPos =
            prSwdmxOggInfo->prDmxRangeInfo->t_start_pos.ui8_i_frame_position;
        prSwdmxOggInfo->u8FileOffset =
            prSwdmxOggInfo->prDmxRangeInfo->ui8_fileoffset;
        prSwdmxOggInfo->u4CurDmxFeederRPtr =
            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);

        if (prSwdmxOggInfo->fgEnVideo)
        {
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 
                prSwdmxOggInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info);
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            _SWDMX_OggWaitVDecPlay(prSwdmxInst, 1);
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxOggInfo->fgVDecIsPause = TRUE;
            prSwdmxOggInfo->fgSpeedChange = TRUE;
        }

        if (prSwdmxOggInfo->fgEnAudio)
        {
            prSwdmxOggInfo->u4ASeekPts =
                prSwdmxOggInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info;
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxOggInfo->u1CurATrackIdx);
			prSwdmxOggInfo->fgSetActivePid = TRUE;
        }
        
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, 
            prSwdmxOggInfo->prDmxRangeInfo->t_vid_start_render_pts - 45000);
        prSwdmxOggInfo->fgBeginToPlay = TRUE;
    }
    else if (prSwdmxInst->i4PlaySpeed == 1000)
    {
        if (prSwdmxOggInfo->fgEnVideo)
        {
            DMX_MM_FlushBuffer(prSwdmxOggInfo->u1VidPid);
            VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
            MPV_FlushEsmQ(prSwdmxInst->u1VdecId, FALSE);
            VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
            VDP_ResetPts(prSwdmxInst->u1B2rId);
        }

        if ((prSwdmxOggInfo->fgEnVideo) &&
                (prSwdmxOggInfo->fgEnAudio) &&
                (prSwdmxOggInfo->prVidIdxTbl) &&
                (prSwdmxOggInfo->u1CurATrackIdx < prSwdmxOggInfo->u1TotalATrack ) &&
                (prSwdmxOggInfo->prAudIdxTbl[prSwdmxOggInfo->u1CurATrackIdx]))
        {
            AUD_SetStartPts(prSwdmxInst->u1AdecId, 0);

            prSwdmxOggInfo->u4TriggerAudPts = 0;
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            _SWDMX_OggWaitVDecPlay(prSwdmxInst, 2);
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxOggInfo->fgVDecIsPause = TRUE;
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxOggInfo->u1CurATrackIdx);
			prSwdmxOggInfo->fgSetActivePid = TRUE;
        }
        // Video only
        else if (prSwdmxOggInfo->fgEnVideo && prSwdmxOggInfo->prVidIdxTbl)
        {
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            _SWDMX_OggWaitVDecPlay(prSwdmxInst, 3);
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxOggInfo->fgVDecIsPause = TRUE;
        }
        // Audio only
        else if ((prSwdmxOggInfo->fgEnAudio) &&
                 (prSwdmxOggInfo->u1CurATrackIdx < prSwdmxOggInfo->u1TotalATrack) &&
                 (prSwdmxOggInfo->prAudIdxTbl[prSwdmxOggInfo->u1CurATrackIdx]))
        {
            AUD_SetStartPts(prSwdmxInst->u1AdecId, 0);
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxOggInfo->u1CurATrackIdx);
			prSwdmxOggInfo->fgSetActivePid = TRUE;
        }
        else
        {
            LOG(1, "SWDMX_Ogg Unknow state for starting playback\n");
            return FALSE;
        }
        STC_StopStc(prSwdmxInst->u1StcId);

        STC_SetStcValue(prSwdmxInst->u1StcId, 0);
        prSwdmxOggInfo->fgBeginToPlay = TRUE;
        prSwdmxOggInfo->i4PrevSpeed  = 1000;
    }
    else
    {
        LOG(0, "Abnormal start play case!\n");
    }

    return TRUE;
}
 
SWDMX_HDR_PRS_STATE_T _SWDMX_OggHdrPrs(UINT8 u1SrcId,
                                       SWDMX_RANGE_LIST_T* prRangeList,
                                       SWDMX_CUR_CMD_T* prCmd)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    //init dmx after add range
    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo) && 
        (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVDmx))
    {
        LOG(3, "### _SWDMX_OggHdrPrs Init video dmx\n");
        _SWDMX_OggInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO, FALSE);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVDmx = TRUE;
    }
    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio) && 
        (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnADmx))
    {
        LOG(3, "### _SWDMX_OggHdrPrs Init audio dmx\n");
        _SWDMX_OggInitDmx(prSwdmxInst, eSWDMX_MEDIA_AUDIO, FALSE);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnADmx = TRUE;
    }

    //Prepare env for last memory or normal start play
    if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgBeginToPlay)
    {
        if (!_SWDMX_OggPreparePlay(prSwdmxInst))
        {
            return eSWDMX_HDR_PRS_FAIL;
        }
    }
    
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgPlayAbort = FALSE;
    if (prSwdmxInst->fgPlayChange)
    {
        _SWDMX_OggResetLerpPos(prSwdmxInst);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFindKey = FALSE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFirstFrame = FALSE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurFilePos = 0;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = 0xFFFFFFFF;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep = 0;
        _SWDMX_OggHandleSpeedChange(prSwdmxInst, prCmd, prRangeList);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed = prSwdmxInst->i4PlaySpeed;
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
    
    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgChgATrack) &&
         (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx < AVI_DIVX_MAX_AUD_TRACK))
    {
        BOOL fgRet = FALSE;
        UINT32 u4APts = 0;
        UINT32 u4CurPts = 0;
        
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgChgATrack = FALSE;

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos = FALSE;
            AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
            LOG(5,"Clear audio eos\n");
        }
        
        u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4CurPts + 27000;

        u4APts = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts;
        fgRet = _SWDMX_OggABufSeekUpdate(prSwdmxInst, 
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx, &u4APts);

        LOG(5, "_SWDMX_OggParsePage Change Audio Track\n");

        if (fgRet)
        {
            LOG(5, "Find audio in the pre-buffer\n");
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4APts;
            AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts);
        }
        else
        {
            LOG(5, "Can't find audio in the pre-buffer\n");
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ChangeAudPts = 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastAudPts[prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx];
        }

        _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx);
        LOG(5, "prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = 0x%08x\n", prSwdmxInst->rFmtInfo.rSwdmxAviInfo.u4TriggerAudPts);
       
    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio) && 
             (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSetActivePid) &&
			 (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack))
	{
		if (!((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS) && 
			  (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable)))
		{
			LOG(0, "%s trigger audio resume\n", __FUNCTION__);
			DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
			prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = VDP_GetPts(prSwdmxInst->u1B2rId);
			AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts);
			_SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx);
			prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSetActivePid = TRUE;
		}
	}
	
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek)
    {
        BOOL fgFindKey = FALSE;
        UINT32 u4SeekTime = 0;
        UINT32 u4ConsumedSize = 0;

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts != 0xFFFFFFFF)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts > 10*90000)
            {
                u4SeekTime = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts - 10 * 90000;
            }
        }
        fgFindKey = _SWDMX_OggSeekKeyPage(prSwdmxInst, prRangeList,
                                          (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr),
                                          prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder,
                                          &u4ConsumedSize, SEEK_KEY, u4SeekTime);

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid)
        {
            _SWDMX_OggResetFeeder(prSwdmxInst);
            u4ConsumedSize = 0;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4FeederReqID++;
        }

        if (u4ConsumedSize != 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4ConsumedSize;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder -=  u4ConsumedSize;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr += u4ConsumedSize;
            if ((UINT32) prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr >=
                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr =
                    (UINT32)prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }
            FeederUpdateReadPointer(FEEDER_PROGRAM_SOURCE, 
                VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder < OGG_NORMAL_PAGE_SIZE)
        {
            if (_SWDMX_OggRequestData(prSwdmxInst) == eSWDMX_HDR_PRS_FED_MOVE_FAIL)
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
        }

        if (!fgFindKey && prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekDone)
        {
            prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NO_INDEX;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FilePosBeforeSeek;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekError = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekDone = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek = FALSE;

            STC_StartStc(prSwdmxInst->u1StcId);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = 0;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts = 0xFFFFFFFF;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
            {
                AUD_WaitDspFlush(INTERNAL_DAC_ID_MAIN);
            }
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        else if (fgFindKey)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVDmx)
            {
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
                VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                MPV_FlushEsmQ(prSwdmxInst->u1VdecId, FALSE);
                VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                VDP_ResetPts(prSwdmxInst->u1B2rId);
            }

            VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);

            if (prSwdmxInst->i4PlaySpeed > 0)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxMp4Info.fgEnVideo)
                {
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts + 2000);
                }
            }
            else
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0);
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts = 0xFFFFFFFF;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekDone = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek = FALSE;
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        else
        {
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }
    if (((prSwdmxInst->i4PlaySpeed > 2000 || prSwdmxInst->i4PlaySpeed < 0) &&
         prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo) ||
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio &&
         !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
         (prSwdmxInst->i4PlaySpeed < 0) && 
         prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady))
    {
        return _SWDMX_DeliverKey(u1SrcId, prRangeList);
    }
    else
    {
        if (prRangeList->eRangeStats == eSWDMX_RANGE_STATE_NO_INDEX)
        {
            prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
        }
        return  _SWDMX_OggParse(u1SrcId, prRangeList);
    }
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverKey(UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList)
{
    BOOL    fgFindKey = FALSE;
    UINT32  u4ConsumedSize = 0;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid)
    {
        LOG(3, "Feeder invalid, reset feeder!\n");
        _SWDMX_OggResetFeeder(prSwdmxInst);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = FALSE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4FeederReqID++;
    }

    if (_SWDMX_OggRequestData(prSwdmxInst) !=  eSWDMX_HDR_PRS_SUCCEED)
    {
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if ((prSwdmxInst->i4PlaySpeed > 2000) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount == 0) &&
        !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos)
    {

        fgFindKey = _SWDMX_OggSeekKeyPage(prSwdmxInst, prRangeList,
                                          (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr),
                                          prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder,
                                          &u4ConsumedSize, SEEK_FORWARD, 0);

        if (u4ConsumedSize != 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4ConsumedSize;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder -= u4ConsumedSize;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr += u4ConsumedSize;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr >=
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr =
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }
            FeederUpdateReadPointer(FEEDER_PROGRAM_SOURCE, 
                VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos >=
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz)
        {
            _SWDMX_SendOggAVEos(prSwdmxInst, prRangeList);
        }

        if (!fgFindKey)
        {
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

    }
    if ((prSwdmxInst->i4PlaySpeed < 0) &&
        (((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount == 0) && 
         !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos) ||
        (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo && 
         !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos)))
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver)
        {
        
            _SWDMX_SendOggAVEos(prSwdmxInst, prRangeList);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFirstFrame = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = FALSE;
        }
        if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            fgFindKey = _SWDMX_OggSeekKeyPage(prSwdmxInst, prRangeList,
                            (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr),
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder,
                            &u4ConsumedSize, SEEK_BACKWARD, 0);
        }
        else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
        {
            if((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos - 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos) >= 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond)
            {
                UINT32 u4DmxAvailSizeAudio;
                UINT32 u4Addr, u4Size;
                UINT32 u4BufSize;

                u4DmxAvailSizeAudio = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                                      DMX_PID_TYPE_ES_AUDIO,
                                                      prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
                
                if(!_SWDMX_GetAudioFifo(u1SrcId, &u4Addr, &u4Size))
                {
                    LOG(3, "_SWDMX_GetAudioFifo Fail\n");
                    return eSWDMX_HDR_PRS_FAIL;
                }
                
                u4BufSize = u4Size;
                if((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS) &&
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendVorbisTable)
                {
                    if(u4DmxAvailSizeAudio >= 
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHdrLength[prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx])
                    {
                        u4DmxAvailSizeAudio -= 
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHdrLength[prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx];
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendVorbisTable = FALSE;
                }
                if(((u4BufSize - u4DmxAvailSizeAudio) < 100000) &&
                   ((u4BufSize - u4DmxAvailSizeAudio) == prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastAudioFifoDataSize))
                {
                    fgFindKey = _SWDMX_OggSeekKeyPage(prSwdmxInst, prRangeList,
                                      (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr),
                                      prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder,
                                      &u4ConsumedSize, SEEK_BACKWARD, 0);
                    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver)
                    {
                        _SWDMX_SendOggAVEos(prSwdmxInst, prRangeList);
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = FALSE;
                    }
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastAudioFifoDataSize =
                        u4BufSize - u4DmxAvailSizeAudio;
                    // wait audio consume
                    if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond*20)  // DTV00089946 for too low bitrate file,
                    {
                        x_thread_delay(200); // last 20 second
                    }
                    else
                    {
                        x_thread_delay(100);
                    }
                    return eSWDMX_HDR_PRS_SUCCEED;
                }   
            }
        }
        if (u4ConsumedSize != 0)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4ConsumedSize;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder -= u4ConsumedSize;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr += u4ConsumedSize;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr >=
                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr =
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }
            FeederUpdateReadPointer(FEEDER_PROGRAM_SOURCE, 
                VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
        }

        if (!fgFindKey && prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
    }
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo && 
        (fgFindKey || (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount > 0)))
    {
        return _SWDMX_OggMoveKey(prSwdmxInst, prRangeList,
                                  (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
    }
    else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio &&
            !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
            !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos)
    {
        return _SWDMX_OggParse(u1SrcId, prRangeList);
    }
    else
    {
        return eSWDMX_HDR_PRS_SUCCEED;
    }
}

static BOOL _SWDMX_OggMovePicHdr(SWDMX_INFO_T *prSwdmxInst, DMX_MM_DATA_T *prDmxMMData)
{
    UINT8 *pu1Buf = NULL;
    UINT32 u4Size = SWDMX_OGG_PIC_HDR_BUF_LEN;
    DMX_MM_DATA_T rDmxMMData = {0};
    UINT32 i;
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    if (!prDmxMMData)
    {
        LOG(0, "[%s][%d] prDmxMMData = NULL!\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    
    pu1Buf = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.pu1PicHdrBuf;
    if (!pu1Buf)
    {
        LOG(0, "[%s][%d] there is no buf to make start code!\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    
    // Add pic header(00 00 00 01) in front of video pkt
    for (i=0; i<SWDMX_OGG_STARTCODE_LEN; i++)
    {
        *pu1Buf++ = (UINT8)(SWDMX_OGG_PIC_STARTCODE >> (8 * (SWDMX_OGG_STARTCODE_LEN - i - 1)));
    }
    u4Size -= SWDMX_OGG_STARTCODE_LEN;
    
    u4Size = (u4Size > prDmxMMData->u4FrameSize) ? prDmxMMData->u4FrameSize : u4Size;
    
    _SWDMX_OggMemcpy((UCHAR *)&rDmxMMData, (UCHAR *)prDmxMMData, sizeof(DMX_MM_DATA_T));
    for (i=0; i<u4Size; i++)
    {
        *pu1Buf++ = *(UINT8 *)prDmxMMData->u4StartAddr;
        prDmxMMData->u4FrameSize--;
        prDmxMMData->u4StartAddr++;
        if (prDmxMMData->u4StartAddr >= prDmxMMData->u4BufEnd)
        {
            prDmxMMData->u4StartAddr -= prDmxMMData->u4BufEnd - prDmxMMData->u4BufStart;
        }
    }
    rDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucSwdmxInternalBuf;
    rDmxMMData.u4BufEnd = rDmxMMData.u4BufStart + SWDMX_INTERNAL_BUF_SIZE;
    rDmxMMData.u4StartAddr = (UINT32)prSwdmxInst->rFmtInfo.rSwdmxOggInfo.pu1PicHdrBuf;
    rDmxMMData.u4FrameSize = u4Size + SWDMX_OGG_STARTCODE_LEN;

    if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData))
    {
        LOG(0, "%s:%d, move data fail!\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    
    if (prSwdmxInst->u4LogFilter & check_move_data)
    {
        UINT8 au1Data[16];

        LOG(0, "move pic hdr, StartAddr=0x%X, Size=0x%X, Pts=0x%X\n",
                rDmxMMData.u4StartAddr, rDmxMMData.u4FrameSize,
                rDmxMMData.u4Pts);
        _SWDMX_OggGetRingData((UINT8 *)rDmxMMData.u4BufStart,
                (UINT8 *)rDmxMMData.u4BufEnd,
                (UINT8 *)rDmxMMData.u4StartAddr,
                au1Data,
                16);
        _SWDMX_OggPrintHex(au1Data, 16);
    }               
    
    return TRUE;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_OggMoveKey(SWDMX_INFO_T *prSwdmxInst,
                                               SWDMX_RANGE_LIST_T* prRangeList,
                                               UCHAR* pucBitstream)
{
    BOOL fgFrechPage = FALSE;
    BOOL fgIsKeyFrame = FALSE;
    BOOL fgSendDone = FALSE;

    UCHAR *pucStream = pucBitstream;
    UCHAR *pucSegBits = NULL;
    UCHAR *pucBitNext = NULL;
    UCHAR ucData[4] = {0};

    UINT64 u8GranulePos = 0;

    UINT8 u1StreamType = 0;
    UINT8 u1PageSegment = 0;
    UINT8 u1TrackIdx = 0;
    UINT8 u1FrameCount = 0;
    UINT8 u1FrameKey = 0;

    UINT32 u4PayLoadSize = 0;
    UINT32 u4PageSize = 0;
    UINT32 u4Pts = 0;
    UINT32 u4RepeatCount = 0;
    UINT32 u4KeyFrameSize = 0;
    UINT32 u4AvailByte = 0;
    UINT32 u4ConsumeSize = 0;
    UINT32 u4DmxAvailSize = 0;

    DMX_MM_DATA_T rDmxMMData;

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucBitstream, &pucStream, ucData, 4);

    if (!_SWDMX_OggCheckHeader(ucData) )
    {
        return eSWDMX_HDR_PRS_FAIL;
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
        (u1StreamType == eSWDMX_MEDIA_VIDEO))
    {
        UINT16 u2CurQNum = 0;
        UINT16 u2MaxQNum = 0;
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        if (u2CurQNum > (25*3))
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
            {
                _SWDMX_OggVideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
            }
            LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);
            x_thread_delay(20);
		    prSwdmxInst->fgFifoFull = TRUE;
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }

    fgFrechPage = _SWDMX_OggPageHdrPrs(prSwdmxInst, pucStream,  &u8GranulePos,
                                       &u1StreamType, &u1PageSegment, &u1TrackIdx);
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos = u8GranulePos;
    
    u4PageSize = OGG_HEADER_SIZE + u1PageSegment;
    pucStream = pucBitstream + OGG_HEADER_SIZE;
    pucSegBits = pucStream + u1PageSegment;
    if ((UINT32)(pucSegBits) >=
        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        pucSegBits = (UCHAR*)((UINT32)pucSegBits -
                     VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                     VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
    }
    
    if (fgFrechPage)
    {
        if (*pucSegBits == 0x08)
        {
            fgIsKeyFrame = TRUE;
        }
    }

    while (u1PageSegment > 0)
    {
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 1);
        u1PageSegment--;
        if (((UINT8)ucData[0] == 0xff) && (u1PageSegment != 0))
        {
            u4PayLoadSize = u4PayLoadSize + (UINT8)ucData[0];
            u4PageSize += (UINT8)ucData[0];
        }
        else
        {
            u4PayLoadSize = u4PayLoadSize + (UINT8)ucData[0];
            u4PageSize += (UINT8)ucData[0];
            u1FrameCount ++;
            if (!fgIsKeyFrame)
            {
                pucSegBits += u4PayLoadSize;
                if ((UINT32)(pucSegBits) >=
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    pucSegBits = (UCHAR*)((UINT32)pucSegBits -
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                }
                if (*pucSegBits == 0x08)
                {
                    u1FrameKey = u1FrameCount;
                    fgIsKeyFrame = TRUE;
                }
            }
            else
            {
                if (u4KeyFrameSize == 0)
                {
                    u4KeyFrameSize = u4PayLoadSize;
                }
            }

            u4PayLoadSize = 0;
        }
    }
    if (u4KeyFrameSize == 0)
    {
        u4KeyFrameSize = u4PayLoadSize;
    }

    if (((UINT8)ucData[0] == 0xff) && (u1PageSegment == 0))
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendKeyDone = FALSE;
    }
    else
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendKeyDone = TRUE;
    }

    u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                          (DMX_PID_TYPE_T)eSWDMX_MEDIA_VIDEO,
                                          prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos > u1FrameCount)
    {
        u8GranulePos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos - u1FrameCount + u1FrameKey;
    }

    u8GranulePos++;
    _SWDMX_OggCalculatePts(prSwdmxInst, u1StreamType, u1TrackIdx, u8GranulePos, &u4Pts);

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount == 0)
    {
        _SWDMX_OggCalculateRepeatFrame(prSwdmxInst, u4Pts, &u4RepeatCount);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount = u4RepeatCount;
    }

    FeederUpdateReadPointer(FEEDER_PROGRAM_SOURCE, 
        VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));

    if (u4DmxAvailSize <= (u4KeyFrameSize + OGG_ONE_MB / 2))
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
        {
            _SWDMX_OggVideoWakeUp(prSwdmxInst);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
        }
        x_thread_delay(20);
		prSwdmxInst->fgFifoFull = TRUE;
        return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
    }

    rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid;
    rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
    rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
    rDmxMMData.u4StartAddr = (UINT32)pucSegBits + 1;
    rDmxMMData.u4FrameSize = u4KeyFrameSize - 1;
    rDmxMMData.u4Pts = u4Pts;
    rDmxMMData.u4Dts = u4Pts;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastVidPts = u4Pts;

    if (u1StreamType == eSWDMX_MEDIA_VIDEO &&
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastVidPts &&
        SWDMX_OGG_NOT_NEED_SEARCH_START_CODE(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType))
    {
        if (_SWDMX_OggMovePicHdr(prSwdmxInst, &rDmxMMData) != TRUE)
        {
            LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastVidPts = 0;
    }

    LOG(7, "Type = %ld Size = 0x%08x pts = 0x%08x\n",
        u1StreamType, u4KeyFrameSize, u4Pts);
    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData) != TRUE)
    {
        LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
    }
    
    if (prSwdmxInst->u4LogFilter & check_move_data)
    {
        UINT8 au1Data[16];

        LOG(0, "[%d]move data, Type=%d, StartAddr=0x%X, Size=0x%X, Pts=0x%X\n",
                __LINE__, u1StreamType, rDmxMMData.u4StartAddr, rDmxMMData.u4FrameSize,
                rDmxMMData.u4Pts);
        _SWDMX_OggGetRingData((UINT8 *)rDmxMMData.u4BufStart,
                (UINT8 *)rDmxMMData.u4BufEnd,
                (UINT8 *)rDmxMMData.u4StartAddr,
                au1Data,
                16);
        _SWDMX_OggPrintHex(au1Data, 16);
    }

    u4AvailByte = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder;
    u4AvailByte -= u4PageSize;
    u4ConsumeSize += u4PageSize;
    u4KeyFrameSize = 0;

    if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendKeyDone)
    {
        pucBitNext = pucBitstream;

        while (TRUE)
        {
            pucBitNext += u4PageSize;
            if ((UINT32)pucBitNext >=
                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                pucBitNext = (UCHAR*)((UINT32)pucBitNext -
                             VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                             VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
            }
            pucStream = pucBitNext;
            if (u4AvailByte < OGG_HEADER_SIZE + 0xFF)
            {
                _SWDMX_OggRequestData(prSwdmxInst);
                u4AvailByte += MIN_READ_SIZE;
            }
            _SWDMX_OggGetSomeBytes(prSwdmxInst, pucBitNext, &pucStream, ucData, 4);
            if (!_SWDMX_OggCheckHeader(ucData) )
            {
                return eSWDMX_HDR_PRS_FAIL;
            }
            fgFrechPage = _SWDMX_OggPageHdrPrs(prSwdmxInst, pucStream,  &u8GranulePos,
                                                &u1StreamType, &u1PageSegment, &u1TrackIdx);

            u4PageSize = OGG_HEADER_SIZE + u1PageSegment;
            pucStream = pucBitNext + OGG_HEADER_SIZE;
            if (u1StreamType != eSWDMX_MEDIA_VIDEO)
            {
                while (u1PageSegment > 0)
                {
                    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 1);
                    u4PageSize += (UINT32)(ucData[0]);
                    u1PageSegment--;
                }
                if (u4AvailByte < u4PageSize )
                {
                    _SWDMX_OggRequestData(prSwdmxInst);
                    u4AvailByte += MIN_READ_SIZE;
                }
                u4AvailByte -= u4PageSize;
                u4ConsumeSize += u4PageSize;

                continue;
            }
            else
            {
                if (fgFrechPage)
                {
                    break;
                }
                else
                {
                    pucStream = pucBitNext + OGG_HEADER_SIZE;
                    pucSegBits = pucStream + u1PageSegment;
                    u4PageSize = u1PageSegment + OGG_HEADER_SIZE;
                    if ((UINT32)(pucSegBits) >=
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                    {
                        pucSegBits = (UCHAR*)((UINT32)pucSegBits -
                                     VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                     VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                    }

                    while (u1PageSegment > 0)
                    {
                        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 1);
                        u1PageSegment--;
                        if ((UINT8)ucData[0] == 0xff)
                        {
                            u4PayLoadSize = u4PayLoadSize + (UINT8)ucData[0];
                            u4PageSize += (UINT8)ucData[0];
                        }
                        else
                        {
                            u4PayLoadSize = u4PayLoadSize + (UINT8)ucData[0];
                            u4PageSize += (UINT8)ucData[0];
                            fgSendDone = TRUE;
                            if (u4KeyFrameSize == 0)
                            {
                                u4KeyFrameSize = u4PayLoadSize;
                            }
                            u4PayLoadSize = 0;
                        }
                    }
                    if (u4KeyFrameSize == 0)
                    {
                        u4KeyFrameSize = u4PayLoadSize;
                    }

                    if (u4AvailByte < u4PageSize )
                    {
                        _SWDMX_OggRequestData(prSwdmxInst);
                        u4AvailByte += MIN_READ_SIZE;
                    }

                    rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                    rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
                    rDmxMMData.u4StartAddr = (UINT32)pucSegBits;
                    rDmxMMData.u4FrameSize = u4KeyFrameSize;
                    rDmxMMData.u4Pts = u4Pts;
                    rDmxMMData.u4Dts = u4Pts;
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastVidPts =  0;

                    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                    {
                        LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                    if (prSwdmxInst->u4LogFilter & check_move_data)
                    {
                        UINT8 au1Data[16];

                        LOG(0, "[%d]move data, Type=%d, StartAddr=0x%X, Size=0x%X, Pts=0x%X\n",
                                __LINE__, u1StreamType, rDmxMMData.u4StartAddr, rDmxMMData.u4FrameSize,
                                rDmxMMData.u4Pts);
                        _SWDMX_OggGetRingData((UINT8 *)rDmxMMData.u4BufStart,
                                (UINT8 *)rDmxMMData.u4BufEnd,
                                (UINT8 *)rDmxMMData.u4StartAddr,
                                au1Data,
                                16);
                        _SWDMX_OggPrintHex(au1Data, 16);
                    }
                }

                if (ucData[0] != 0xFF)
                {
                    fgSendDone = TRUE;
                }

                if (fgSendDone)
                {
                    break;
                }
                if (u4AvailByte < u4PageSize )
                {
                    _SWDMX_OggRequestData(prSwdmxInst);
                    u4AvailByte += MIN_READ_SIZE;
                }
                u4AvailByte -= u4PageSize;
                u4ConsumeSize += u4PageSize;
            }
        }
    }

    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount --;

    if ( prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount == 0)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFirstFrame)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = TRUE;
        }

        if (pucBitNext != NULL)
        {
            pucBitNext = (UCHAR*)((UINT32)pucBitNext + u4PageSize);
        }
        else
        {
            pucBitNext = (UCHAR*)((UINT32)pucBitstream + u4PageSize);
        }
        if ((UINT32)(pucBitNext) >=
                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
        {
            pucBitNext = (UCHAR*)((UINT32)pucBitNext -
                         VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                         VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
        }

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder -= u4ConsumeSize;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4ConsumeSize;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr = (UINT32)pucBitNext;
        FeederUpdateReadPointer(FEEDER_PROGRAM_SOURCE, 
            VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
    }
    else
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr = (UINT32)pucBitstream;
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_OggParse(UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList)
{
    UINT32 u4ConsumedSize = 0;
    UINT32 u4PrsState = 0;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos == 0)
    {
        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
                FEEDER_E_OK)
        {
            LOG(7, "Feeder input buffer error.\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
            prRangeList->prDmxRangeInfo->ui8_fileoffset;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
            prRangeList->prDmxRangeInfo->ui8_fileoffset;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FileOffset =
            prRangeList->prDmxRangeInfo->ui8_fileoffset;
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = 0;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos = 0;
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr =
            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
    }
    if (((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS) &&
         !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prDmxRangeInfo->t_start_pos.ui8_pts_info != 0))
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = 0;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos = 0;
    }

    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos >= 
         prRangeList->prDmxRangeInfo->z_range_sz) && 
        (prRangeList->prDmxRangeInfo->z_range_sz != 0xffffffff))
    {
        _SWDMX_SendOggAVEos(prSwdmxInst, prRangeList);
    }
    if ((prRangeList->prDmxRangeInfo->z_range_sz > prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos) || 
        ((prRangeList->prDmxRangeInfo->z_range_sz == 0xffffffff) && 
         (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder >= u4ConsumedSize)))
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid)
        {
            _SWDMX_OggResetFeeder(prSwdmxInst);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4FeederReqID++;
        }
        if (_SWDMX_OggRequestData(prSwdmxInst) == eSWDMX_HDR_PRS_FED_MOVE_FAIL)
        {
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

		if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder >= 
		    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FileOffset)
        {
            if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS) &&
                !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable)
            {
                UINT32 i = 0;
                UINT32 u4Size = 0;
                BOOL fgSaveSuc = FALSE;
                
                _SWDMX_OggGetVorbisTblSize(prSwdmxInst, &u4Size);
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHeaderPtr)
                {
                    UINT32 u4Addr, u4Align;
                    u4Addr = 0;
                    u4Align = 16;
                    if (!SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align) ||
                        u4Addr == 0)
                    {
                        LOG(0, "%s(%d): eSWDMX_GET_INTERNAL_BUF NG\n", __FUNCTION__, __LINE__);
                        ASSERT(0);
                        return eSWDMX_HDR_PRS_FAIL;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHeaderPtr[0] = u4Addr;
                }
                for (i=0; i<prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack; i++)
                {
                    fgSaveSuc = _SWDMX_OggSaveVorbisTbl(u1SrcId, i,
                                    (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
                    if (fgSaveSuc)
                    {
                        if(((i + 1) < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) && 
                           ((i + 1) < OGG_MAX_AUDIO_TRACK))
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHeaderPtr[i + 1] = 
                                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHeaderPtr[i] +
                                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudHdrLength[i];
                        }
                    }
                }
                if (fgSaveSuc)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable = TRUE;
        		    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = FALSE;
                    //last memory case
                    if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prDmxRangeInfo->t_start_pos.ui8_pts_info != 0)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prDmxRangeInfo->t_start_pos.ui8_i_frame_position;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prDmxRangeInfo->t_start_pos.ui8_i_frame_position;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FileOffset =
                            prRangeList->prDmxRangeInfo->ui8_fileoffset;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr =
                            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
                    }
                    else
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos = prSwdmxInst->u8FileOffset;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = 
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
                    LOG(5,"Save vorbis table success!\n");
                }
                return eSWDMX_HDR_PRS_SUCCEED;
            }
         }
         else if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable)
         {
             if (_SWDMX_OggRequestData(prSwdmxInst) == eSWDMX_HDR_PRS_FED_MOVE_FAIL)
             {
                 return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
             }
         }
    }
	

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder > 0)
    {
        u4PrsState = _SWDMX_OggParsePage(u1SrcId, prRangeList,
                         (UCHAR*)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr),
                         prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder, &u4ConsumedSize);

        switch (u4PrsState)
        {
            case eSWDMX_HDR_PRS_FAIL:
                if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos >=
                    (prRangeList->prDmxRangeInfo->z_range_sz + prRangeList->prDmxRangeInfo->ui8_fileoffset)) ||
                    (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos > 
                     prRangeList->prDmxRangeInfo->ui8_fileoffset + OGG_ONE_MB))
                {
                    LOG(3, "No dts chunks was discovered\n");
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                        prRangeList->prDmxRangeInfo->z_range_sz + 
                        prRangeList->prDmxRangeInfo->ui8_fileoffset;
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder = 0;
                    return eSWDMX_HDR_PRS_FAIL;
                }
                break;
            default:
                break;
        }
    }

	//add cts
	if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver) &&
	    (u4ConsumedSize >= prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder)) 
    {
        _SWDMX_SendOggAVEos(prSwdmxInst, prRangeList);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFirstFrame = FALSE;
        LOG(3,"_SWDMX_OggParse send over, send eos!\n");
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder >= u4ConsumedSize)
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4ConsumedSize;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder -= u4ConsumedSize;
    }
    else
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder = 0;
    }

    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr += u4ConsumedSize;
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr >=
        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr -=
            VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) -
            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
    }

    FeederUpdateReadPointer(FEEDER_PROGRAM_SOURCE, 
        VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr));
    LOG(7,"Feeder Update Read Pointer,u4CurDmxFeederRPtr:0x%x, left in buffer:0x%x\n",
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr,
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder);
    return eSWDMX_HDR_PRS_SUCCEED;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_OggParsePage(UINT8 u1SrcId,
                                                 SWDMX_RANGE_LIST_T* prRangeList,
                                                 UCHAR* pucBitstream,
                                                 UINT32 u4AvailSize,
                                                 UINT32* pu4ConsumeSize)
{
    BOOL fgFindPage     = FALSE;
    BOOL fgLostPage     = FALSE;
    BOOL fgFreshPacket = FALSE;

    UINT8 u1TempSegment = 0;
    UINT8 u1PageSegment = 0;
    UINT8 u1StreamType = 0;
    UINT8 u1TrackIdx = 0;
    UINT8 u1PidIdx = 0;
    UINT8 u1PidType = 0;
    UINT8 u1FrameCount = 0;
    UINT16 u2CurQNum = 0;
    UINT16 u2MaxQNum = 0;
	UINT16 u2DmxPictureQSize = 0;
    UINT16 u2DmxMaxQSize = 0;

    UINT32 u4PageSize = 0;
    UINT32 i = 0;
    UINT32 u4PageSeqNum = 0;
    UINT32 u4PrsState = 0;
    UINT32 u4DmxAvailSize = 0;
    UINT64 u8GranulAverg = 0;

    UINT64 u8GranulePos = 0;

    UCHAR *pucStream = pucBitstream;
    UCHAR *pucSegBits = NULL;

    UCHAR ucData[4] = {0};
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    while (i < u4AvailSize)
    {
        pucStream = pucBitstream;

        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucBitstream, &pucStream, ucData, 4);
        if (_SWDMX_OggCheckHeader(ucData) == TRUE)
        {
            fgFindPage = TRUE;
            break;
        }

        i++;
        pucBitstream++;
        if ((UINT32)pucBitstream >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
        {
            pucBitstream = (UCHAR*)((UINT32)pucBitstream -
                           VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                           VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
        }
    }

    // Didn't find the Page
    if (!fgFindPage)
    {
        x_thread_delay(10);
        *pu4ConsumeSize = i;
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if ((u4AvailSize -i) < OGG_HEADER_SIZE)
    {
        LOG(3, "left in Feeder byte number is small than header size\n");
        *pu4ConsumeSize = i;
        return eSWDMX_HDR_PRS_SUCCEED;
    }
    
    fgFreshPacket = _SWDMX_OggPageHdrPrs(prSwdmxInst, pucStream,  &u8GranulePos,
                                         &u1StreamType, &u1PageSegment, &u1TrackIdx);
    pucSegBits = pucBitstream + OGG_HEADER_SIZE;

    fgLostPage =
        (++prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4PageSeqNum) == u4PageSeqNum ? FALSE : TRUE ;

    if (fgLostPage)
    {
        //LOG(3, "there is a packet lost %d\n",(1+u4PageSeqNum - prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4PageSeqNum));
    }

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
        (u1StreamType == eSWDMX_MEDIA_VIDEO))
    {
        BOOL fgVideoFull = FALSE;
    
    	DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        if ((u2CurQNum > (25*3)) &&
            !(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio && 
            !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady))
        {
            LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld\n", u2CurQNum, u2MaxQNum);
            fgVideoFull = TRUE;
        }
		else if(u2DmxMaxQSize <= (u2DmxPictureQSize + 100))
        { 
            LOG(6, "DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
			fgVideoFull = TRUE;
        }
		else if((u2DmxPictureQSize + u2CurQNum)>= (u2MaxQNum -10))
        { 
            LOG(6, "DMX MESSAGE QUEUE VDEC Q FULL,WAIT FOR A WHILE\n");   
			fgVideoFull = TRUE;
        }

        if (fgVideoFull)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
            {
                _SWDMX_OggVideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
            }
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
            {
                if (_SWDMX_OggGetAudFifoData(prSwdmxInst) <= 1*1024 &&
                    !STC_IsStartedStc(prSwdmxInst->u1StcId))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                    AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                    LOG(0, "[%d]VFIFO full, AFIFO empty, force AoutEnable, force start stc at(0x%x)!\n", __LINE__,
                            STC_GetStcValue(prSwdmxInst->u1StcId));
                }
            }
            
            *pu4ConsumeSize = 0;
		    prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(20);
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }

    if ((!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo) &&
        (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS))
    {
        LOG(3, "Video & Audio both disable!\n");
        *pu4ConsumeSize = 0;
        x_thread_delay(20);
        return eSWDMX_HDR_PRS_SUCCEED;
    }
    
    if (u4AvailSize < u1PageSegment)
    {
        LOG(3, "left in Feeder byte number is small than header segment size\n");
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    u1TempSegment = u1PageSegment;

    pucStream = pucBitstream + OGG_HEADER_SIZE;

    if ((UINT32)pucStream >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        pucStream = (UCHAR*)((UINT32)pucStream -
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
    }

    while (u1TempSegment > 0)
    {
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 1);

        u4PageSize = u4PageSize + (UINT8)ucData[0];
        if ((UINT8)ucData[0] != 0xFF)
        {
            u1FrameCount++;
        }
        u1TempSegment--;
    }

    if (ucData[0] == 0xFF)
    {
        u1FrameCount++;
    }

    u4PageSize = u4PageSize + u1PageSegment + OGG_HEADER_SIZE;
    LOG(7,"_SWDMX_OggParsePage PageSize:0x%x\n",u4PageSize);
    if (u4AvailSize < u4PageSize + i)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos + u4AvailSize >=
            prRangeList->prDmxRangeInfo->z_range_sz)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                prRangeList->prDmxRangeInfo->z_range_sz;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder = 0;
        }
        if(prSwdmxInst->i4PlaySpeed < 0)
        {
            *pu4ConsumeSize = u4AvailSize;
        }
        LOG(3, "left in Feeder byte number is small than page size\n");
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if ((!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnADmx) &&
        (eSWDMX_MEDIA_SKIPPED_AUDIO == u1StreamType))
    {
         u1StreamType = eSWDMX_MEDIA_SKIPPED;
    }

    if ((eSWDMX_MEDIA_AUDIO == u1StreamType) ||
        (eSWDMX_MEDIA_SKIPPED_AUDIO == u1StreamType))
    {
        UINT32 u4Pts = 0;

        if (u1FrameCount > 0)
        {
            if (u1TrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack)
            {
                u8GranulAverg = u8Div6432((u8GranulePos - 
                                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx]), 
                                    u1FrameCount, NULL);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx] += u8GranulAverg;
            }
        }
        else
        {
            if (u1TrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx] = u8GranulePos;
            }
        }
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts != 0xFFFFFFFF)
        {
            if (u1TrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack)
            {
                _SWDMX_OggCalculatePts(prSwdmxInst, 
                                       u1StreamType, 
                                       u1TrackIdx, 
                                       prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx], 
                                       &u4Pts);
            }
            if ((u4Pts < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts) ||
                (u1TrackIdx != prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastAudPts[u1TrackIdx] = u4Pts;
                u1StreamType = eSWDMX_MEDIA_SKIPPED;
                if (u1TrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx] = u8GranulePos;
                }
            }
            else
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
                {
#ifdef CC_AUD_SUPPORT_DUAL_DSP       
                    AUD_DSPCmdPlay(AUD_DSP0, 0);
#else
                    AUD_DSPCmdPlay(0);
#endif
                }
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4Pts;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = 0xFFFFFFFF;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4Pts);
            }
        }
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgChgATrack && (eSWDMX_MEDIA_AUDIO == u1StreamType))
        {
            if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgChgATrack) &&
                (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx < AVI_DIVX_MAX_AUD_TRACK))
            {
                BOOL fgRet = FALSE;
                UINT32 u4APts = 0;
                UINT32 u4CurPts = 0;
                
                if (u4Pts == 0)
                {
                    _SWDMX_OggCalculatePts(prSwdmxInst, u1StreamType, u1TrackIdx, 
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx], &u4Pts);
                }
                
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgChgATrack = FALSE;

                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos = FALSE;
                    AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
                    LOG(5,"Clear audio eos\n");
                }
                
                u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4CurPts + 27000;

                u4APts = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts;
                fgRet = _SWDMX_OggABufSeekUpdate(prSwdmxInst,
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx, &u4APts);

                LOG(5, "_SWDMX_OggParsePage Change Audio Track\n");

                if (fgRet)
                {
                    LOG(5, "Find audio in the pre-buffer\n");
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4APts;
                    AUD_SetStartPts(prSwdmxInst->u1AdecId, 
                                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts);
                }
                else
                {
                    LOG(5, "Can't find audio in the pre-buffer\n");
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ChangeAudPts = u4Pts;
                }

                _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx);
                LOG(5, "prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = 0x%08x\n",
                        prSwdmxInst->rFmtInfo.rSwdmxAviInfo.u4TriggerAudPts);
            }
        }
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekError)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
            {
#ifdef CC_AUD_SUPPORT_DUAL_DSP     
                AUD_DSPCmdPlay(AUD_DSP0, 0);
#else
                AUD_DSPCmdPlay(0);
#endif
            }
            _SWDMX_OggCalculatePts(prSwdmxInst, u1StreamType, u1TrackIdx, u8GranulePos, &u4Pts);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4Pts;
            AUD_SetStartPts(prSwdmxInst->u1AdecId, u4Pts);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekError = FALSE;
        }
    }
    else
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSpeedChange)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos = u8GranulePos;
        }
    }

    if (eSWDMX_MEDIA_SKIPPED == u1StreamType)
    {
        *pu4ConsumeSize = u4PageSize + i;
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if ((u1StreamType == eSWDMX_MEDIA_VIDEO) ||
       (u1StreamType == eSWDMX_MEDIA_AUDIO))
    {
        u1PidIdx = (u1StreamType == eSWDMX_MEDIA_VIDEO) ? 
                   (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid) :
                   (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
        u1PidType = (u1StreamType == eSWDMX_MEDIA_VIDEO) ? 
                    (DMX_PID_TYPE_ES_VIDEO) : (DMX_PID_TYPE_ES_AUDIO);

        u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                              (DMX_PID_TYPE_T)u1PidType,
                                              u1PidIdx);

        if (u4DmxAvailSize <= u4PageSize)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
            {
                _SWDMX_OggVideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
            }

            if ((u1StreamType == eSWDMX_MEDIA_AUDIO) &&
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady)
            {
                AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                STC_StartStc(prSwdmxInst->u1StcId);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady = FALSE;
            }
            else if (u1StreamType == eSWDMX_MEDIA_VIDEO)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
                {
                    if (_SWDMX_OggGetAudFifoData(prSwdmxInst) <= 1*1024)
                    {
                        if (!STC_IsStartedStc(prSwdmxInst->u1StcId))
                        {
                            STC_StartStc(prSwdmxInst->u1StcId);
                            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                            LOG(0, "VFIFO full, AFIFO empty, force AoutEnable, force start stc at(0x%x)!\n",
                                    STC_GetStcValue(prSwdmxInst->u1StcId));
                        }
                    }
                }
            }

            LOG(6, "blocked u1StreamType = %ld, u4PageSize = %ld, u4DmxAvailSize = %ld\n",
                    u1StreamType,
                    u4PageSize,
                    u4DmxAvailSize);
            *pu4ConsumeSize = i;
            x_thread_delay(5);
			prSwdmxInst->fgFifoFull = TRUE;
            return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
        }
    }
    //Send Ogg page header to audio fifo
    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS)&&
        ((u1StreamType == eSWDMX_MEDIA_AUDIO) ||
         (u1StreamType == eSWDMX_MEDIA_SKIPPED_AUDIO)))
    {
        DMX_MM_DATA_T rDmxMMData;
        UINT8 u1AudPid = OGG_INVAILD_AUD_IDX;

        if (_SWDMX_OggABufGetPidx(prSwdmxInst, u1TrackIdx, &u1AudPid))
        {
            x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

            rDmxMMData.u1Idx = u1AudPid;
            rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
            rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
            rDmxMMData.u4StartAddr = (UINT32)pucBitstream;
            rDmxMMData.u4FrameSize = u1PageSegment + OGG_HEADER_SIZE;
            rDmxMMData.u4Pts = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastAudPts[u1TrackIdx];
            rDmxMMData.u4Dts = 1;

            LOG(6, "_SWDMX_OggParsePage move ogg header, size:0x%08x, PTS: 0x%08x\n",
                    rDmxMMData.u4FrameSize,rDmxMMData.u4Pts);
            if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData))
            {
                LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
    }

    u4PrsState = _SWDMX_OggMoveData(prSwdmxInst, prRangeList, pucSegBits, 
                                    u1PageSegment, u1StreamType, u1TrackIdx, 
                                    fgFreshPacket, u8GranulAverg);

    if (u4PrsState == eSWDMX_HDR_PRS_SUCCEED)
    {
        if (u1StreamType == eSWDMX_MEDIA_VIDEO)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos = u8GranulePos;
        }
        else
        {
            if (u1TrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1TrackIdx] = u8GranulePos;
            }
        }
        *pu4ConsumeSize = u4PageSize + i;
        LOG(7,"_SWDMX_OggParsePage pu4ConsumeSize:0x%x\n",*pu4ConsumeSize);
    }
    else
    {
        return eSWDMX_HDR_PRS_FAIL;
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_OggMoveData(SWDMX_INFO_T *prSwdmxInst,
                                                SWDMX_RANGE_LIST_T *prRangeList,
                                                UCHAR *pucBitstream,
                                                UINT8 u1PageSegment,
                                                UINT8 u1StreamType,
                                                UINT8 u1TrackIdx,
                                                BOOL fgFreshPacket,
                                                UINT64 u8GranulAverg)
{
    BOOL fgVaildPid = TRUE;
    UCHAR ucData[4] = {0};
    UCHAR *pucStream = pucBitstream;
    UCHAR *pucBitsSeg = NULL;
    UINT32 u4Pts = 0;
    UINT32 u4PacketSize = 0;
    UINT32 u4ConsumedSize    = 0;
    UINT32 u4LenBytes = 0;
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    
    if (!prSwdmxInst)
    {
        LOG(0, "[%s][%d] prSwdmxInst = NULL!\n", __FUNCTION__, __LINE__); 
        return eSWDMX_HDR_PRS_FAIL;
    }
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    pucBitsSeg = pucStream + u1PageSegment;

    if ((UINT32)pucBitsSeg >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        pucBitsSeg = (UCHAR*)((UINT32)pucBitsSeg -
                     VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                     VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
    }

    while (u1PageSegment > 0)
    {
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 1);
        u1PageSegment--;
        if (((UINT8)ucData[0] == 0xff) && (u1PageSegment != 0))
        {
            u4PacketSize = u4PacketSize + (UINT8)ucData[0];
        }
        else
        {
            u4PacketSize = u4PacketSize + (UINT8)ucData[0];

            if (fgFreshPacket == FALSE)
            {
                pucBitsSeg = pucBitsSeg + u4ConsumedSize ;
            }
            else
            {
                pucBitsSeg = pucBitsSeg + u4ConsumedSize;

                if ((UINT32)pucBitsSeg >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    pucBitsSeg = (UCHAR*)((UINT32)pucBitsSeg -
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                }
                u4LenBytes = (((*pucBitsSeg) & (0x3)) << 1) | (((*pucBitsSeg) & 0x40) >> 6);
                if (u4LenBytes != 0)
                {
                    LOG(7, "***********************%d**\n", u4LenBytes);
                }

                if (((prSwdmxOggInfo->u4ADecType != ENUM_SWDMX_ADEC_VORBIS) &&
                     ((u1StreamType == eSWDMX_MEDIA_AUDIO) || 
                      (u1StreamType == eSWDMX_MEDIA_SKIPPED_AUDIO))) ||
                    (u1StreamType == eSWDMX_MEDIA_VIDEO))
                {
                    if (u4PacketSize > (u4LenBytes + 1))
                    {
                       pucBitsSeg = pucBitsSeg + u4LenBytes + 1;
                        u4PacketSize = u4PacketSize - (u4LenBytes + 1);
                    }
                    else
                    {
                        if(u4LenBytes == 0)
                        {
                            prSwdmxOggInfo->fgNotSendPacket = TRUE;
                            LOG(0,"skip this packet!\n");
                        }
                    }
                }
            }

            if (u1StreamType != eSWDMX_MEDIA_SKIPPED)
            {
                UINT32 u8GranulPos = 0;

                if ((u1StreamType == eSWDMX_MEDIA_AUDIO) ||
                    (u1StreamType == eSWDMX_MEDIA_SKIPPED_AUDIO))
                {
                    if (fgFreshPacket)
                    {
                        if (u1TrackIdx < prSwdmxOggInfo->u1TotalATrack)
                        {
                            u8GranulPos = prSwdmxOggInfo->u8AudGranulePos[u1TrackIdx] ;
                            prSwdmxOggInfo->u8AudGranulePos[u1TrackIdx] += u8GranulAverg;
                        }
                        _SWDMX_OggCalculatePts(prSwdmxInst, u1StreamType, u1TrackIdx, u8GranulPos, &u4Pts);
                        if( prSwdmxOggInfo->fgPlayChange && prSwdmxInst->i4PlaySpeed == 1000)
                        {
                            LOG(5,"Play change, set audio start pts:0x%x\n",u4Pts);
                            AUD_SetStartPts(prSwdmxInst->u1AdecId, u4Pts);
                            prSwdmxOggInfo->fgPlayChange = FALSE;
                        }
                        prSwdmxOggInfo->u4LastAudPts[u1TrackIdx] = u4Pts;
                        if(prSwdmxOggInfo->u4ADecType != ENUM_SWDMX_ADEC_VORBIS)
                        {
                            if (u4PacketSize > 2)
                            {
                                pucBitsSeg = pucBitsSeg + 2;
                                u4PacketSize -= 2;
                            }
                        }
                        if (prSwdmxOggInfo->u4ADecType == ENUM_SWDMX_ADEC_AAC)
                        {
                            _SWDMX_OggAACHandle(prSwdmxInst, prRangeList, u1TrackIdx,
                                                u4PacketSize, u4Pts);
                        }
                    }
                    if (fgFreshPacket == FALSE)
                    {
                        u4Pts = prSwdmxOggInfo->u4LastAudPts[u1TrackIdx];
                        if (u4PacketSize == 0)
                        {
                            fgFreshPacket = TRUE;
                            continue;
                        }
                    }

                    if (prSwdmxOggInfo->u4ChangeAudPts != 0)
                    {
                        if (!fgFreshPacket)
                        {
                            fgFreshPacket = TRUE;
                            u4ConsumedSize = u4PacketSize;
                            u4PacketSize = 0;
                            continue;
                        }
                        else
                        {
                            if (u4Pts > prSwdmxOggInfo->u4ChangeAudPts)
                            {
                                prSwdmxOggInfo->u4TriggerAudPts = u4Pts;
                                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4Pts);
                                prSwdmxOggInfo->u4ChangeAudPts = 0;
                            }
                            else
                            {
                                continue;
                            }
                        }
                    }
                }
                else
                {
                    if (fgFreshPacket)
                    {
                        u8GranulPos = ++prSwdmxOggInfo->u8VidGranulePos;
                        _SWDMX_OggCalculatePts(prSwdmxInst, u1StreamType, u1TrackIdx, u8GranulPos, &u4Pts);
                        prSwdmxOggInfo->u4LastVidPts = u4Pts;
                    }
                    if (fgFreshPacket == FALSE)
                    {
                        prSwdmxOggInfo->u4LastVidPts = 0;
                        prSwdmxOggInfo->u8VidGranulePos++;
                        if (u4PacketSize == 0)
                        {
                            fgFreshPacket = TRUE;
                            continue;
                        }
                    }
                }

                if ((UINT32)pucBitsSeg >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    pucBitsSeg = (UCHAR*)((UINT32)pucBitsSeg -
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                }
                if ((prSwdmxOggInfo->fgSpeedChange) && (fgFreshPacket == FALSE))
                {
                    fgFreshPacket = TRUE;
                    u4ConsumedSize = u4PacketSize;
                    u4PacketSize = 0;
                    continue;
                }

                if (u1StreamType == eSWDMX_MEDIA_VIDEO)
                {
                    if ((u4PacketSize < 0x20) &&
                        (prSwdmxOggInfo->u8CurDmxMoviPos ==
                         prRangeList->prDmxRangeInfo->ui8_fileoffset))
                    {
                        u4ConsumedSize = u4PacketSize;
                        u4PacketSize = 0;
                        continue;
                    }
                }

                prSwdmxOggInfo->fgSpeedChange = FALSE;

                switch (u1StreamType)
                {
                    case eSWDMX_MEDIA_VIDEO:
                        rDmxMMData.u1Idx = prSwdmxOggInfo->u1VidPid;
                        fgVaildPid = TRUE;
                        break;
                    case eSWDMX_MEDIA_AUDIO:
                    case eSWDMX_MEDIA_SKIPPED_AUDIO:
                        fgVaildPid = _SWDMX_OggABufGetPidx(prSwdmxInst, u1TrackIdx, &rDmxMMData.u1Idx);
                        break;
                    default:
                        LOG(3, "Wrong Pid\n");
                        fgVaildPid = FALSE;
                        break;
                }
                rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
                rDmxMMData.u4StartAddr = (UINT32)pucBitsSeg;
                rDmxMMData.u4FrameSize = u4PacketSize;
                rDmxMMData.u4Pts = u4Pts;
                rDmxMMData.u4Dts = u4Pts;
                
                if (u4PacketSize == 0)
                {
                    LOG(0, "meet error\n");
                    u4ConsumedSize = 0;
                    continue;
                }
                if (!fgVaildPid)
                {
                    // do nothing  
                }
                else if (!prSwdmxOggInfo->fgNotSendPacket)
                {
                    if (u1StreamType == eSWDMX_MEDIA_VIDEO &&
                        prSwdmxOggInfo->u4LastVidPts &&
                        SWDMX_OGG_NOT_NEED_SEARCH_START_CODE(prSwdmxOggInfo->u4VDecType))
                    {
                        if (_SWDMX_OggMovePicHdr(prSwdmxInst, &rDmxMMData) != TRUE)
                        {
                            LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                        }
                        prSwdmxOggInfo->u4LastVidPts = 0;
                    }

                    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData) != TRUE)
                    {
                        LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                    
                    if (prSwdmxInst->u4LogFilter & check_move_data)
                    {
                        UINT8 au1Data[16], u1Size;

                        LOG(0, "move data, Type=%d, StartAddr=0x%X, Size=0x%X 0x%X, Pts=0x%X, Pos=0x%llX\n",
                            u1StreamType, rDmxMMData.u4StartAddr, u4PacketSize, rDmxMMData.u4FrameSize,
                            rDmxMMData.u4Pts,
                            prSwdmxOggInfo->u8CurDmxMoviPos);
                        u1Size = (u4PacketSize > 16) ? 16 : u4PacketSize;
                        _SWDMX_OggGetRingData((UINT8 *)rDmxMMData.u4BufStart,
                                              (UINT8 *)rDmxMMData.u4BufEnd,
                                              (UINT8 *)rDmxMMData.u4StartAddr,
                                              au1Data,
                                              u1Size);
                        _SWDMX_OggPrintHex(au1Data, u1Size);
                    }               
                }
                else
                {
                    prSwdmxOggInfo->fgNotSendPacket = FALSE;
                }
                fgFreshPacket = TRUE;
                u4ConsumedSize = u4PacketSize;
                u4PacketSize = 0;
            }
            else
            {
                continue;
            }
        }
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}

static BOOL _SWDMX_OggGetSomeBytes(SWDMX_INFO_T *prSwdmxInst, UCHAR *pucBitstream,
                                   UCHAR **pucStream, UCHAR *pucData, UINT8 u1ByteNum)
{
    UINT32 u4Length = 0;

    if (((UINT32)(pucBitstream) + u1ByteNum) < prSwdmxInst->rFeederInfo.u4EndAddr)
    {
        x_memcpy(pucData, pucBitstream, u1ByteNum);
        *pucStream = (UCHAR*)((UINT32) * pucStream + u1ByteNum);
    }
    else
    {
        if ((UINT32)*pucStream >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
        {
            *pucStream = (UCHAR*)((UINT32) * pucStream -
                         VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                         VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
        }
        else
        {
            u4Length = prSwdmxInst->rFeederInfo.u4EndAddr - (UINT32)pucBitstream;
            x_memcpy(pucData, pucBitstream, u4Length);
            *pucStream = (UCHAR*)prSwdmxInst->rFeederInfo.u4StartAddr;
        }

        u4Length = u1ByteNum - u4Length;
        x_memcpy(pucData, pucStream, u4Length);
        *pucStream = (UCHAR*)((UINT32) * pucStream + u4Length);
    }
    return TRUE;
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_OggRequestData(SWDMX_INFO_T* prSwdmxInst)
{
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;
    UINT32 u4ReadSize = 0;
    UINT32 u4MaxDataByte = 0;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
	if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver && 
	    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSaveVorbisTable)
	{
        return eSWDMX_HDR_PRS_SUCCEED;
	}
	
    x_memset(&rFeederReqData, 0, sizeof(FEEDER_REQ_DATA_T));

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
    {
        u4ReadSize = MIN_READ_SIZE;
    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS) && 
             (prSwdmxInst->i4PlaySpeed < 0))
    {
        u4ReadSize = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond;
    }
    else
    {
        u4ReadSize = MIN_READ_SIZE_NO_VIDEO;
    }

    u4MaxDataByte = prSwdmxInst->rFeederInfo.u4Size - u4ReadSize;

    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder < u4MaxDataByte) &&
        ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos) < 
         prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz))
    {
		if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz == 0xffffffff) ||
		    (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz <= 0))
		{
			u4ReadSize = MIN_READ_SIZE_NO_VIDEO;
		}
        else
		{
        	u4ReadSize = get_min((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz -
        	                     prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos),
        	                     u4ReadSize);
		}

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4ReqReadTimes == 0)
        {
            if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                                 u4ReadSize,
                                 (UINT64)(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos),
                                 &rFeederCond,
                                 prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4FeederReqID++) == FEEDER_E_BUFFER_FULL)
            {
                LOG(3, "Feeder buffer is not enough\n");
                return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4ReqReadTimes++;
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4ReqReadTimes > 0)
        {
            if (!_SWDMX_OggGetDataFromFeeder(prSwdmxInst, &rFeederReqData))
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4ReqReadTimes--;
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgRepeatRequest)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr = 
                rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AlignOffset = 
                rFeederReqData.u4AlignOffset;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr > 
                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr = 
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr +
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr) - 
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgRepeatRequest = FALSE;
        }

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos += 
            rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder += 
            rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset ;
    }
    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos >= 
         prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8_range_sz))
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos = 0;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = 0;
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}

static BOOL _SWDMX_OggCheckHeader(UCHAR *pucBuf)
{
    UINT32 u4CmpID = 0;

    _SWDMX_OggConvertCharToUint(pucBuf, &u4CmpID);
    if (u4CmpID != OGG_PAGE_HEADER)
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL _SWDMX_OggCheckOggType(SWDMX_INFO_T *prSwdmxInst ,
                                   UCHAR *pucBuf, 
                                   UINT8 *pu1StreamType,
                                   UINT8 *pu1TrackIdx)
{
    UINT32 u4CmpID = 0;
    UINT32 i = 0;

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
    {
        _SWDMX_OggConvertCharToUint(pucBuf, &u4CmpID);
        if (u4CmpID == prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VStrmID)
        {
            *pu1StreamType = eSWDMX_MEDIA_VIDEO;
            return TRUE;
        }
    }

    for (i = 0; (i < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) && (i < OGG_MAX_AUDIO_TRACK); i++)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[i])
        {
            _SWDMX_OggConvertCharToUint(pucBuf, &u4CmpID);
            if (u4CmpID == prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[i]->t_strm_id.u.t_stm_ogg_id)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio && 
                    (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx == i))
                {
                    *pu1StreamType = eSWDMX_MEDIA_AUDIO;
                }
                else
                {
                    *pu1StreamType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                }

                if (pu1TrackIdx)
                {
                    *pu1TrackIdx = i;
                }
                return TRUE;
            }
        }
    }
    *pu1StreamType = eSWDMX_MEDIA_SKIPPED;
    return TRUE;
}

static BOOL _SWDMX_OggResetFeeder(SWDMX_INFO_T* prSwdmxInst)
{
    FEEDER_REQ_DATA_T rFeederReqData;
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LeftBytesInFeeder = 0;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgRepeatRequest = TRUE;
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr = 
        prSwdmxInst->rFeederInfo.u4StartAddr;
    while (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4ReqReadTimes > 0)
    {
        _SWDMX_OggGetDataFromFeeder(prSwdmxInst, &rFeederReqData);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4ReqReadTimes--;
    }

    FeederSetRing(prSwdmxInst->eFeederSrc, TRUE);
    return TRUE;
}

static BOOL _SWDMX_SendOggAVEos(SWDMX_INFO_T* prSwdmxInst, SWDMX_RANGE_LIST_T* prRangeList)
{
    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
        !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
        {
            _SWDMX_OggVideoWakeUp(prSwdmxInst);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
        }

        LOG(5, "_SWDMX_SendOggAVEos Deliver Video Eos\n");
        if (!_SWDMX_SendOggEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
        {
            LOG(3, "Fail to send Video Eos\n");
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos = TRUE;
    }
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio &&
        !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos)
    {
        LOG(5, "_SWDMX_SendOggAVEos Deliver Audio Eos\n");
        if (!_SWDMX_SendOggEos(prSwdmxInst, eSWDMX_MEDIA_AUDIO))
        {
            LOG(3, "Fail to send Audio Eos\n");
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos = TRUE;
        AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
    }
    return TRUE;
}

static BOOL _SWDMX_SendOggEos(SWDMX_INFO_T* prSwdmxInst, UINT8 u1HdrType)
{
    UINT8 u1PidIdx = 0;
    UINT16 u2MaxQNum, u2CurQNum;
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    LOG(5, "%s source id = %d, Hdr Type = %d\n", 
            __FUNCTION__, prSwdmxInst->u1SwdmxId, u1HdrType);

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld\n", u2CurQNum, u2MaxQNum);
        if (u2CurQNum > (u2MaxQNum - 10))
        {
		    prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(1);
            return FALSE;
        }
        u1PidIdx = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    if (_SWDMX_OggInitDmx(prSwdmxInst, u1HdrType, TRUE) == FALSE)
    {
        return FALSE;
    }

    rDmxMMData.u4BufStart = prSwdmxInst->rFeederInfo.u4StartAddr;
    rDmxMMData.u4BufEnd = prSwdmxInst->rFeederInfo.u4EndAddr;
    rDmxMMData.u4StartAddr = prSwdmxInst->rFeederInfo.u4StartAddr;
    rDmxMMData.u4FrameSize = 4;
    rDmxMMData.fgEOS = TRUE;
    rDmxMMData.u1Idx = u1PidIdx;
    rDmxMMData.fgIgnoreData = TRUE;
    prSwdmxInst->fgRcvEOS = TRUE;
    
    if (u1HdrType != eSWDMX_MEDIA_AUDIO)
    {
        return _SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData);
    }
    else
    {
        INT32 i;

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio && 
            (prSwdmxOggInfo->u1ActivePid == DMX_AUDIO_HANDLER_NULL_PIDX))
        {
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx);
        }
        for (i = 0; i < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack; i++)
        {
            if (!_SWDMX_OggABufGetPidx(prSwdmxInst, i, &rDmxMMData.u1Idx))
            {
                continue;
            }
            if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData))
            {
                LOG(3, "Fail to send audio eos\n");
                return FALSE;
            }
        }
        return TRUE;
    }
}

BOOL _SWDMX_OggStop(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    DMX_MM_T rPid;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    rPid.fgEnable = FALSE;

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVDmx)
    {
        LOG(1, "*****stop video\n");
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVDmx = FALSE;
        VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
        DMX_MUL_FreePidx(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
    }
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnADmx)
    {
        LOG(1, "*****stop audio\n");
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnADmx = FALSE;
        VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
        DMX_MUL_FreePidx(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);

        _SWDMX_OggABufDeinit(prSwdmxInst, &rPid);
    }
    x_memset(&prSwdmxInst->rFmtInfo.rSwdmxOggInfo, 0, sizeof(SWDMX_OGG_INFO_T));

    return TRUE;
}

static BOOL _SWDMX_OggSeek(SWDMX_INFO_T* prSwdmxInst,
                           SWDMX_RANGE_LIST_T* prRangeList,
                           UCHAR *pucBitstream,
                           UINT64 u8SeekTime,
                           UINT32 u4AvailSize,
                           UINT32 *pu4ConsumedSize)
{
    BOOL fgSeekForward = FALSE;
    BOOL fgRequestData = FALSE;

    UINT32 u4CurPageTime = 0;

    UINT64 u8DiffPos = 0;
    UINT64 u8DiffGranule = 0;
    UINT64 u8AverageGran = 0;
    UINT64 u8AverageLen = 0;
    UINT64 u8DiffPts = 0;
    UINT64 u8TempValue = 0;

    if (!prSwdmxInst)
    {
        return FALSE;
    }
    
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl)
    {
        u8AverageGran = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui4_avg_frame_num;
        u8AverageLen = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui4_avg_packet_size;
        _SWDMX_OggCalculatePts(prSwdmxInst, 
                               eSWDMX_MEDIA_VIDEO, 
                               0, 
                               prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos, 
                               &u4CurPageTime);

        if ((u8SeekTime <= OGG_TEN_SEC_DURATION) ||
            (prRangeList->prDmxRangeInfo->z_range_sz < OGG_ONE_MB))
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                prRangeList->prDmxRangeInfo->ui8_fileoffset;
            *pu4ConsumedSize = 0;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
            return TRUE;
        }
        if (u8SeekTime > u4CurPageTime)
        {
            u8DiffPts = u8SeekTime - (UINT64)u4CurPageTime;
            fgSeekForward = TRUE;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos >=
                 (prRangeList->prDmxRangeInfo->z_range_sz - OGG_ONE_MB))
            {
                LOG(1, "u8CurDmxMoviPos is too large!\n");
                return TRUE;
            }
            if ((u8DiffPts >= (OGG_TEN_SEC_DURATION * prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep)) &&
                (u8DiffPts < (OGG_THIRTY_SEC_DURATION + 
                 (OGG_TEN_SEC_DURATION * prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep))))
            {
                LOG(1, "u4ReSeekStep=%u u8DiffPts=%llu u8CurDmxMoviPos=%llX\n", 
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep, 
                        u8DiffPts, prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos);
                return TRUE;
            }
        }
        else
        {
            fgSeekForward = FALSE;
            u8DiffPts = (UINT64)u4CurPageTime - u8SeekTime;
            if (u8DiffPts < OGG_TEN_SEC_DURATION / 2)
            {
                u8DiffPts = OGG_TEN_SEC_DURATION / 2;
            }
        }
        u8TempValue = u8Div6432(u8DiffPts, 90, NULL) * 10000;
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui8_timeunit > 0)
        {
            u8DiffGranule =  u8Div6432(u8TempValue, 
                                       prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui8_timeunit, 
                                       NULL);
        }
        if (u8AverageGran > 0)
        {
            u8DiffPos = u8Div6432(u8DiffGranule * 100, u8AverageGran, NULL) * u8AverageLen;
        }
        if (fgSeekForward)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u8DiffPos;
            if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos >
                 prRangeList->prDmxRangeInfo->z_range_sz) &&
                (prRangeList->prDmxRangeInfo->z_range_sz > OGG_ONE_MB))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                    prRangeList->prDmxRangeInfo->z_range_sz - OGG_ONE_MB;
            }

            if ((UINT32)u8DiffPos <  u4AvailSize)
            {
                u4AvailSize -= (UINT32)u8DiffPos;
                fgRequestData = FALSE;
                *pu4ConsumedSize += (UINT32)u8DiffPos;
            }
            else
            {
                fgRequestData = TRUE;
            }
        }
        else
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos < u8DiffPos)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                    prRangeList->prDmxRangeInfo->ui8_fileoffset;
            }
            else
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos -= u8DiffPos;
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos <= 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos < u8DiffPos)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                        prRangeList->prDmxRangeInfo->ui8_fileoffset;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos - u8DiffPos;
                }
            }
            fgRequestData = TRUE;
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos = 
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;

        if (fgRequestData)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
        }
    }
    return FALSE;
}

static BOOL _SWDMX_OggCalculateRepeatFrame(SWDMX_INFO_T* prSwdmxInst, UINT32 u4Pts, UINT32 *pu4TimeCount)
{
    UINT32 u4PtsDif = 0;
    UINT32 u4CurPts = 0;
    INT16  u1PreIdx = 0;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

    if (prSwdmxInst->i4PlaySpeed > 2000)
    {
        u1PreIdx = stKeyinfo.u1CurIdx  - 2;
        if (u1PreIdx < 0)
        {
            u1PreIdx = (u1PreIdx + KEY_LEN) % KEY_LEN ;
        }
        if (u1PreIdx < KEY_LEN)
        {
            if (stKeyinfo.stKeyData[u1PreIdx].u4KeyPts != 0)
            {
                u4CurPts = stKeyinfo.stKeyData[u1PreIdx].u4KeyPts;
            }
        }

        if (u4Pts > u4CurPts)
        {
            u4PtsDif = u4Pts - u4CurPts;
            *pu4TimeCount =  (UINT32)((u4PtsDif * 25) / (90 * prSwdmxInst->i4PlaySpeed));
        }
        if (*pu4TimeCount == 0)
        {
            *pu4TimeCount = 1;
        }

        if (*pu4TimeCount > MAX_REPEAT_TIME)
        {
            *pu4TimeCount = MAX_REPEAT_TIME;
        }
    }
    else if (prSwdmxInst->i4PlaySpeed < 0)
    {
        u1PreIdx = stKeyinfo.u1CurIdx  - 2;
        if (u1PreIdx < 0)
        {
            u1PreIdx = (u1PreIdx + KEY_LEN) % KEY_LEN ;
        }
        if (u1PreIdx < KEY_LEN)
        {
            if (stKeyinfo.stKeyData[u1PreIdx].u4KeyPts != 0)
            {
                u4CurPts = stKeyinfo.stKeyData[u1PreIdx].u4KeyPts;
            }
        }
        if (u4CurPts > u4Pts)
        {
            u4PtsDif = u4CurPts - u4Pts ;
            *pu4TimeCount =  (UINT32)((u4PtsDif * 25) / (90 * get_abs(prSwdmxInst->i4PlaySpeed) * 2));
        }
        if (*pu4TimeCount == 0)
        {
            *pu4TimeCount = 1;
        }

        if (*pu4TimeCount > MAX_REPEAT_TIME)
        {
            *pu4TimeCount = MAX_REPEAT_TIME;
        }
    }
    LOG(1, "pu4TimeCount = %d\n", *pu4TimeCount );
    return TRUE;
}

static BOOL _SWDMX_OggGetDataFromFeeder(SWDMX_INFO_T* prSwdmxInst,
                                        FEEDER_REQ_DATA_T *prFeederData)
{
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgPlayAbort)
    {
        return FALSE;
    }

    if (!_SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)prFeederData))
    {
        LOG(3, "_SWDMX_OggGetDataFromFeeder FeederTimeOut!!!\n");
        return FALSE;
    }

    if (prFeederData->eDataType == FEEDER_SOURCE_INVALID)
    {
        LOG(3, "_SWDMX_OggGetDataFromFeeder invalid\n");
        return FALSE;
    }

    if ((prFeederData->u4Id + 1) != prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4FeederReqID)
    {
        LOG(3, "Feeder ID mismatch!!! feeder id = %ld\n", prFeederData->u4Id);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
        return FALSE;
    }

    if((prFeederData->u4WriteAddr < prSwdmxInst->rFeederInfo.u4StartAddr) || 
       (prFeederData->u4WriteAddr>=prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        LOG(0,"prFeederData->u4WriteAddr = %x\n",prFeederData->u4WriteAddr);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
        return FALSE;
    }

    HalFlushInvalidateDCacheMultipleLine(prSwdmxInst->rFeederInfo.u4StartAddr, 
                                         prSwdmxInst->rFeederInfo.u4Size);
   
	//add end of file
	if(prFeederData->fgEof)
    {
        LOG(5,"---------------------end of file--\n\n\n");
		prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = TRUE;
        return TRUE;
    }

    return TRUE;
}

BOOL _SWDMX_OggAudioCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    if (eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            LOG(5, "SWDMX_OggAudioCb Audio is ready\n");
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts != 0xFFFFFFFF)
            {
            	VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, 
            	                      prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts);
            	LOG(5, "SWDMX_OggAudioCb VDP Trigger PTS = 0x%08x\n", 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts);
            	prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = 0xFFFFFFFF;
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
            {
                _SWDMX_OggVideoWakeUp(prSwdmxInst);
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
            }
        }
        else 
        {
            // Audio Only case
            LOG(5, "Trigger audio by _SWDMX_OggAudioCb\n");
            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
            STC_StartStc(prSwdmxInst->u1StcId);
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady = TRUE;
    }
    else
    {
        // impossible
        LOG(7, "_SWDMX_OggAudioCb eAudioNotifyType != ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE\n");
    }

    return TRUE;
}

static void _SWDMX_OggVideoWakeUp(SWDMX_INFO_T* prSwdmxInst)
{
    SWDMX_OGG_INFO_T *prSwdmxOggInfo;

    SWDMX_OGG_VERIFY_NULL(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&(prSwdmxInst->rFmtInfo.rSwdmxOggInfo);

    VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);
    
    LOG(5, "_SWDMX_OggVideoWakeUp\n");

    switch (prSwdmxOggInfo->u4VDecType)
    {
        case ENUM_SWDMX_VDEC_VC1:
        case ENUM_SWDMX_VDEC_WMV7:
        case ENUM_SWDMX_VDEC_WMV8:
        case ENUM_SWDMX_VDEC_WMV9:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_WMV);
            break;
        case ENUM_SWDMX_VDEC_DX311:
        case ENUM_SWDMX_VDEC_MPEG4:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MP4);
            break;
        case ENUM_SWDMX_VDEC_MJPEG:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MJPEG);
            break;
        case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MPV);
            break;
        case ENUM_SWDMX_VDEC_H264:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H264);
            break;
        default:
            LOG(1, "SWDMX_OggAudioCb Unknown Video Type\n");
            break;
    }
}

static void _SWDMX_OggConvertCharToUint(UCHAR *pucBuf, UINT32* pu4Data)
{
    *pu4Data = ((UINT32)(pucBuf[0])) |
               ((UINT32)(pucBuf[1]) << 8) |
               ((UINT32)(pucBuf[2]) << 16) |
               ((UINT32)(pucBuf[3]) << 24);
}

static SWDMX_HDR_PRS_STATE_T _SWDMX_OggAACHandle(SWDMX_INFO_T *prSwdmxInst,
                                                 SWDMX_RANGE_LIST_T* prRangeList,
                                                 UINT8 u1AudTrick, 
                                                 UINT32 u4SampleSize,
                                                 UINT32 u4Pts)
{
    DMX_MM_DATA_T rTmpDmxMMData;
    UINT8 u1SampleRateIdx = 3;
    
    if (prSwdmxInst->pucAudBuf == 0)
    {
        LOG(1, "pucAudBuf should be allocated in init function");
        prSwdmxInst->pucAudBuf =
            (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(SWDMX_OGG_AAC_ALLOC_SIZE, 16));
        if (!prSwdmxInst->pucAudBuf)
        {
            LOG(1, "pucAudBuf allocated fail");
            ASSERT(0);
        }
    }

    if (prSwdmxInst->pucAudBuf)
    {
        /* 8b: syncword */
        prSwdmxInst->pucAudBuf[0] = 0xFF;
        /* 4b: syncword */
        /* 1b: mpeg id = 1 */
        /* 2b: layer = 0 */
        /* 1b: protection absent */
        prSwdmxInst->pucAudBuf[1] = 0xF9;
        /* 2b: profile */
        /* 4b: sampling_frequency_index */
        /* 1b: private = 0 */
        /* 1b: channel_configuration */
        switch (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudSampleRate)
        {
            case 48000:
                u1SampleRateIdx = 3;
                break;
            case 44100:
                u1SampleRateIdx = 4;
                break;
            case 32000:
                u1SampleRateIdx = 5;
                break;
            case 24000:
                u1SampleRateIdx = 6;
                break;
            case 22050:
                u1SampleRateIdx = 7;
                break;
            case 16000:
                u1SampleRateIdx = 8;
                break;
            case 12000:
                u1SampleRateIdx = 9;
                break;
            case 11025:
                u1SampleRateIdx = 10;
                break;
            case 8000:
                u1SampleRateIdx = 11;
                break;
            default:
                u1SampleRateIdx = 4;
                break;
        }

        prSwdmxInst->pucAudBuf[2] = (1 << 6) |
                                    ((u1SampleRateIdx << 2) & 0x3C) |
                                    ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudChannelNs >> 2) & 0x1);
        /* 2b: channel_configuration */
        /* 1b: original */
        /* 1b: home */
        /* 1b: copyright_id */
        /* 1b: copyright_id_start */
        /* 2b: aac_frame_length */
        prSwdmxInst->pucAudBuf[3] = ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4AudChannelNs & 0x3) << 6) |
                                    (((u4SampleSize + SWDMX_OGG_AAC_FAKE_HDR_SIZE) >> 11) & 0x3);
        /* 8b: aac_frame_length */
        prSwdmxInst->pucAudBuf[4] = (((u4SampleSize + SWDMX_OGG_AAC_FAKE_HDR_SIZE) >> 3) & 0xFF);
        /* 3b: aac_frame_length */
        /* 5b: adts_buffer_fullness */
        prSwdmxInst->pucAudBuf[5] = (((u4SampleSize + SWDMX_OGG_AAC_FAKE_HDR_SIZE) << 5) & 0xE0) |
                                    ((0x7FF >> 6) & 0x1F);
        /* 6b: adts_buffer_fullness */
        /* 2b: number_of_raw_data_blocks_in_frame */
        prSwdmxInst->pucAudBuf[6] = ((0x7FF << 2) & 0xFC);

        x_memset(&rTmpDmxMMData, 0, sizeof(DMX_MM_DATA_T));
        rTmpDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucAudBuf;
        rTmpDmxMMData.u4BufEnd = rTmpDmxMMData.u4BufStart + SWDMX_OGG_AAC_ALLOC_SIZE;
        rTmpDmxMMData.u4StartAddr = rTmpDmxMMData.u4BufStart;
        rTmpDmxMMData.u4FrameSize = 7;
        rTmpDmxMMData.u4Pts = u4Pts;

        if (!_SWDMX_OggABufGetPidx(prSwdmxInst, u1AudTrick, &rTmpDmxMMData.u1Idx))
        {
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }

        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rTmpDmxMMData)  != TRUE)
        {
            LOG(0, "[%d]Demuxer fails to move data.\n", __LINE__);
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
    }
    return  eSWDMX_HDR_PRS_SUCCEED;
}

static void _SWDMX_OggCalculatePts(SWDMX_INFO_T *prSwdmxInst,
                                   UINT8 u1StreamType,
                                   UINT8 u1TrackIdx,
                                   UINT64 u8GranulPos,
                                   UINT32 *pu4Pts)
{
    UINT64 ui8TempVal = 0;
    
    if (u1StreamType == eSWDMX_MEDIA_VIDEO)
    {
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl)
        {
            ui8TempVal = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui8_timeunit * u8GranulPos;
            *pu4Pts = (UINT32)u8Div6432(ui8TempVal, 10000, NULL);
            *pu4Pts = *pu4Pts * 90;
        }
    }
    else if ((u1StreamType == eSWDMX_MEDIA_AUDIO || 
              u1StreamType == eSWDMX_MEDIA_SKIPPED_AUDIO) &&
             (u1TrackIdx < AVI_DIVX_MAX_AUD_TRACK) && 
             (u1TrackIdx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack))
    {
        if ( prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[u1TrackIdx])
        {
            ui8TempVal = u8GranulPos * 1000;
            ui8TempVal = ui8TempVal * 90;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[u1TrackIdx]->ui4_rate > 0)
            {
                *pu4Pts = (UINT32)u8Div6432(ui8TempVal, 
                                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[u1TrackIdx]->ui4_rate, 
                                            NULL);
            }
        }
    }
}

static BOOL _SWDMX_OggHandleSpeedChange(SWDMX_INFO_T *prSwdmxInst, 
                                        SWDMX_CUR_CMD_T *prCmd, 
                                        SWDMX_RANGE_LIST_T *prRangeList)
{
    #if 0
    UINT32 u4SeekLen = 0;
    #endif
    UINT32 u4AudAvailData = 0;
    UINT32 u4CurPts = 0;
    UINT64 u8CurPos = 0;
    UINT8 u1AudIdx = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx;

    if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
    {
        UINT64 frame_pos = 0;
        UINT64 iframe_pos = 0;
        INT32 tmp_ref = 0;
        UINT16 dec_order = 0; 

        u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

        if(!VDP_GetFilePosition(prSwdmxInst->u1B2rId, &tmp_ref, 
                    &dec_order, &frame_pos, &iframe_pos))
        {
            LOG(0, "%s, get pos from vdp fail!\n", __FUNCTION__);
            iframe_pos = frame_pos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
        }
        u8CurPos = frame_pos;
    }
    else
    {
        u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);
        u8CurPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
    }
    LOG(5, "%s, u4CurPts=0x%X u8CurPos=0x%llX\n", __FUNCTION__, u4CurPts, u8CurPos);
    
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos)
    {
        UINT32 u4Flag;
        DMX_MM_T rDmxMMInfo;

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;
        u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
        
        if (SWDMX_OGG_NOT_NEED_SEARCH_START_CODE(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VDecType))
        {
            LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
            rDmxMMInfo.fgSearchStartCode = FALSE;
        }
        else
        {
            LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
            rDmxMMInfo.fgSearchStartCode = TRUE;
        }
        if (!DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid, u4Flag, &rDmxMMInfo))
        {
            LOG(3, "Fail to reset demuxer\n");
        }

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos = FALSE;
    }
    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos)
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverAEos = FALSE;
        AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
    }

    // Audio only case don't care follow speed change
    if((!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 0) &&
        (prSwdmxInst->i4PlaySpeed > 0) &&
        (!prCmd->fgSeek) &&
        ((UINT32)(prSwdmxInst->u8SeekPos >> 32) == 0xFFFFFFFF) && 
        ((UINT32)(prSwdmxInst->u8SeekPos) == 0xFFFFFFFF))
    {
        LOG(1, "SWDMX ogg audio only case don't care speed change [%d -> %d]\n",
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed, prSwdmxInst->i4PlaySpeed);
        return TRUE;
    }
    
    if ((prSwdmxInst->i4PlaySpeed > 0) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos != 0))
    {
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos = 0;
    }

    if ((prCmd->fgSeek) &&
        (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
    {
        UINT8 u1Idx = 0;
        LOG(1, "seek offset, ipos(0x%llX), pts(0x%llX)\n",
            prCmd->rSeekOffset.ui8_i_frame_position,
            prCmd->rSeekOffset.ui8_pts_info);

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSpeedChange = TRUE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount = 0;
        
        for ( ;(u1Idx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) && (u1Idx < OGG_MAX_AUDIO_TRACK); u1Idx++)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1Idx] = 0;
        }
        _SWDMX_OggABufFlush(prSwdmxInst);

        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
            prCmd->rSeekOffset.ui8_i_frame_position;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts =
            prCmd->rSeekOffset.ui8_pts_info;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ChangeAudPts = 
            prCmd->rSeekOffset.ui8_pts_info;
        MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
        VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,
                              prCmd->rSeekOffset.ui8_pts_info);
        stKeyinfo.stKeyData[0].u8KeyOffset =
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
        stKeyinfo.stKeyData[0].u4KeyPts = prCmd->rSeekOffset.ui8_i_pts_info;

        if (prSwdmxInst->i4PlaySpeed == 1000)
        {
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, u1AudIdx);
        }
    }
    else if (((UINT32)(prSwdmxInst->u8SeekPos >> 32) != 0xFFFFFFFF) || 
             ((UINT32)(prSwdmxInst->u8SeekPos) != 0xFFFFFFFF))
    {
        UINT32 u1Idx = 0;
        
        if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            LOG(1, "seek time, time(0x%llX)\n", prSwdmxInst->u8SeekPos);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_HEADER;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FilePosBeforeSeek = 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = (UINT32)prSwdmxInst->u8SeekPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts = (UINT32)prSwdmxInst->u8SeekPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ChangeAudPts = (UINT32)prSwdmxInst->u8SeekPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady = FALSE;
            MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
            VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
        }
        else if (!prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            LOG(1, "seek time, pos(0x%llX)\n", prSwdmxInst->u8SeekPos);
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek = FALSE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos = prSwdmxInst->u8SeekPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = prSwdmxInst->u8SeekPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos = 
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = 0;
        }
        
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSpeedChange = TRUE;

        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
        for ( ;(u1Idx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) && (u1Idx < OGG_MAX_AUDIO_TRACK); u1Idx++)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1Idx] = 0;
        }
        _SWDMX_OggABufFlush(prSwdmxInst);

        if ((prSwdmxInst->i4PlaySpeed == 1000) && 
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
        {
            _SWDMX_OggABufSetActivePidx(prSwdmxInst, u1AudIdx);
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = FALSE;
    }
    else if (((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed <= 2000) &&
              (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 0)) &&
             (prSwdmxInst->i4PlaySpeed <= 2000 && prSwdmxInst->i4PlaySpeed > 0))
    {
        UINT32 u4Pts = 0;
        BOOL fgFind = FALSE;

        LOG(1, "[%s][%d]Speed change: %d -> %d\n", __FUNCTION__, __LINE__,
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed,
            prSwdmxInst->i4PlaySpeed);

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos = 0;

        if (prSwdmxInst->i4PlaySpeed == 1000)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed == 1)
                {
                    STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                }

                if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed == 1) ||
                    ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 1000) &&
                     (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 1)))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                }

                u4Pts = u4CurPts + 45000;
                fgFind = _SWDMX_OggABufSeekUpdate(prSwdmxInst, 
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx, &u4Pts);
                LOG(1, "Find Apts = 0x%X from PreAFifo, fgFind=%d\n", u4Pts, fgFind);
            }

            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
        
            if (fgFind)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4Pts;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4Pts);
            }
            else
            {
                u4Pts = u4CurPts + 45000;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4TriggerAudPts = u4Pts;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ChangeAudPts = u4Pts;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4Pts);
            }

            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
            {
                _SWDMX_OggABufSetActivePidx(prSwdmxInst, u1AudIdx);
                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4CurPts);
            }
        }
    }
    else if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 2000 ||
              prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0) &&
             ((prSwdmxInst->i4PlaySpeed == 2000) ||
              (prSwdmxInst->i4PlaySpeed <= 1000 && 
               prSwdmxInst->i4PlaySpeed > 0)))
    {
        UINT32 i = 0;
        UINT8 u1Idx = 0;

        LOG(1, "[%s][%d]Speed change: %d -> %d\n", __FUNCTION__, __LINE__,
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed,
            prSwdmxInst->i4PlaySpeed);

        if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 2000) &&
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            for (i=0; i<KEY_LEN; i++)
            {
                if (stKeyinfo.u1CurIdx < KEY_LEN)
                {
                    LOG(1, "u4KeyPts = %x\n", stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u4KeyPts);
                    if (stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u4KeyPts >= u4CurPts)
                    {
                        break;
                    }
                    stKeyinfo.u1CurIdx++;
                    if (stKeyinfo.u1CurIdx >= KEY_LEN)
                    {
                        stKeyinfo.u1CurIdx = stKeyinfo.u1CurIdx % KEY_LEN;
                    }
                }
            }
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
                 prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0)
        {
            for (i=0; i<KEY_LEN; i++)
            {
                stKeyinfo.u1CurIdx--;
                if (stKeyinfo.u1CurIdx <= 0)
                {
                    stKeyinfo.u1CurIdx = KEY_LEN - 1;
                }
                if (stKeyinfo.u1CurIdx < KEY_LEN)
                {
                    LOG(1, "u4KeyPts = %x\n", stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u4KeyPts);
                    if (stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u4KeyPts >= u4CurPts)
                    {
                        break;
                    }
                }
            }
        }
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount = 0;
            MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
            VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
            for ( ;u1Idx < (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) &&
                 (u1Idx < AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1Idx] = 0;
            }

            if ((stKeyinfo.u1CurIdx < KEY_LEN) && (i < KEY_LEN) )
            {
                #if 0
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0)
                {
                u4SeekLen = _SWDMX_OggSeekLen(prSwdmxInst, 
                                              SEEK_BACKWARD,
                                              prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl);
                }
                if ((u4SeekLen > prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos))
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                        prRangeList->prDmxRangeInfo->ui8_fileoffset;
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                }
                else
                #endif
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                        stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u8KeyOffset;
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                }
            }
            _SWDMX_OggResetFeeder(prSwdmxInst);
        
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
            {
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
            }
            for (u1Idx=0; (u1Idx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) &&
                 (u1Idx < OGG_MAX_AUDIO_TRACK); u1Idx++)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1Idx] = 0;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4LastAudPts[u1Idx] = 0;
            }
            _SWDMX_OggABufFlush(prSwdmxInst);
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
            
            if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio) && 
                (prSwdmxInst->i4PlaySpeed == 1000))
            {
                _SWDMX_OggABufSetActivePidx(prSwdmxInst, u1AudIdx);
            }
        }
        else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
        {
            if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS) &&
                    prSwdmxInst->i4PlaySpeed == 1000)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgPlayChange = TRUE;
                u4AudAvailData = _SWDMX_OggGetAudFifoData(prSwdmxInst);
                if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 2000)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos -= u4AudAvailData;
                }
                else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4AudAvailData;
                    u4CurPts = 0;
                }
                STC_StopStc(prSwdmxInst->u1StcId);
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);
                for (u1Idx = 0 ;(u1Idx < prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack) &&
                     (u1Idx < OGG_MAX_AUDIO_TRACK); u1Idx++)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AudGranulePos[u1Idx] = 0;
                }

                _SWDMX_OggABufFlush(prSwdmxInst);
                _SWDMX_OggABufSetActivePidx(prSwdmxInst, u1AudIdx);
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos =
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
        }

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSpeedChange = TRUE;

        if (prSwdmxInst->i4PlaySpeed == 1000)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos ==
                prRangeList->prDmxRangeInfo->ui8_fileoffset)
            {
                u4CurPts = 0;
            }
            if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ASeekPts = u4CurPts;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ChangeAudPts = u4CurPts;
            }
        }

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            if (i >= KEY_LEN)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_HEADER;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FilePosBeforeSeek = 
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgIsSeek = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts = (UINT32)u4CurPts;
            }
            x_memset(&stKeyinfo, 0, sizeof(KEY_INFO));
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
        }
        STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
        STC_StopStc(prSwdmxInst->u1StcId);
        
    }
    else
    {
        LOG(1, "[%s][%d]Speed change: %d -> %d\n", __FUNCTION__, __LINE__,
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed,
            prSwdmxInst->i4PlaySpeed);

        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
            (((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 2000) &&
              (prSwdmxInst->i4PlaySpeed < 0)) ||
             ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0) &&
              (prSwdmxInst->i4PlaySpeed > 2000))))
        {
            x_memset(&stKeyinfo, 0, sizeof(KEY_INFO));
        }

        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_SEEK_LEN;
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {            
            MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
            VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
            if (prSwdmxInst->i4PlaySpeed < 0)
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0);
            }
            else
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
            }
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = u8CurPos;
        }
        else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
        {
            u4AudAvailData = _SWDMX_OggGetAudFifoData(prSwdmxInst); 
            if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed >= 0)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos -= u4AudAvailData;
            }
            else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed < 0)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4AudAvailData;
            }
            if((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 0) &&
               (prSwdmxInst->i4PlaySpeed < 0))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = FALSE;
            }
            STC_StopStc(prSwdmxInst->u1StcId);
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos =
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
        
        _SWDMX_OggABufFlush(prSwdmxInst);
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid);

        if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
        {
            if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ADecType == ENUM_SWDMX_ADEC_VORBIS)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendVorbisTable = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgAudIsReady = TRUE;
            }

            _SWDMX_OggABufSetActivePidx(prSwdmxInst, u1AudIdx);
        }
        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
        {
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1VidPid);
        }
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4RepeatCount = 0;
    }
    
    return TRUE;
}

static BOOL _SWDMX_OggPageHdrPrs(SWDMX_INFO_T* prSwdmxInst,
                                 UCHAR *pucBitstream,
                                 UINT64 *pu8GranulePos,
                                 UINT8 *pu1StreamType,
                                 UINT8 *pu1Segment,
                                 UINT8 *pu1TrackIdx)
{
    BOOL fgFreshPacket = FALSE;
    BOOL fgBeginPage = FALSE;
    BOOL fgEosPage = FALSE;

    UCHAR *pucStream = pucBitstream;
    UCHAR ucData[4] = {0};

    UINT32 u4TempData = 0;
    UINT32 u4PageSeqNum = 0;
    UINT32 u4CrcCheck = 0;

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 2);

    fgFreshPacket = ucData[1] & 0x1 ? FALSE : TRUE;
    fgBeginPage = ucData[1] & 0x2;
    fgEosPage = ucData[1] & 0x4;

    if (fgBeginPage)
    {
        LOG(5, "enter BOS page, maybe system\n");
    }

    if (fgEosPage)
    {
        LOG(5, "enter EOS page\n");
    }

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 4);
    _SWDMX_OggConvertCharToUint(ucData, &u4TempData);
    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 4);
    _SWDMX_OggConvertCharToUint(ucData, (UINT32*)pu8GranulePos);
    *pu8GranulePos = (*pu8GranulePos << 32) | u4TempData ;

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 4);

    _SWDMX_OggCheckOggType(prSwdmxInst, ucData, pu1StreamType, pu1TrackIdx);

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 4);
    _SWDMX_OggConvertCharToUint(ucData, &u4PageSeqNum);

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 4);
    _SWDMX_OggConvertCharToUint(ucData, &u4CrcCheck);

    _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 1);
    *pu1Segment = (UINT8)ucData[0];

    if (fgEosPage)
    {
        LOG(5, "enter EOS page\n");
        if (*pu1StreamType == eSWDMX_MEDIA_VIDEO)
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo &&
                !prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause)
                {
                    _SWDMX_OggVideoWakeUp(prSwdmxInst);
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgVDecIsPause = FALSE;
                }

                LOG(5, "_SWDMX_AviHdrPrsWithoutIdx Deliver Video Eos\n");
                if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.i4PrevSpeed > 0)
                {
                    if (!_SWDMX_SendOggEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO))
                    {
                        LOG(3, "Fail to send Video Eos\n");
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgDeliverVEos = TRUE;
                }
            }
        }
    }
    return fgFreshPacket;
}

BOOL _SWDMX_OggPlay(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    _SWDMX_OggResetLerpPos(prSwdmxInst);
    return TRUE;
}

BOOL _SWDMX_OggAbort(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = &prSwdmxInst->rFmtInfo.rSwdmxOggInfo;
    if (prSwdmxOggInfo)
    {
        LOG(3, "OGG Play abort\n");
        prSwdmxOggInfo->fgPlayAbort = TRUE;
    }
    return TRUE;
}

static UINT32 _SWDMX_OggSeekLen(SWDMX_INFO_T*prSwdmxInst, SEEK_DERECTION eSeekDerection,MM_RANGE_ELMT_IDXTBL_T * prIdxTbl)
{
    UINT32 u4SeekLenth = 0;
    UINT32 u4SeekTime = 0;
    UINT64 u8DiffGranule = 0;
    UINT64 u8TempValue = 0;
    UINT64 u8AverageGran = 0;
    UINT64 u8AverageLen = 0;
    UINT32 u4CurPts =0;

    if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
    {
        if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl != NULL)
        {
            u8AverageGran = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui4_avg_frame_num;
            u8AverageLen = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui4_avg_packet_size;
        }
        if (eSeekDerection == SEEK_FORWARD)
        {
            u8TempValue = u8Div6432((UINT64)(OGG_FF_SEEK_TIME * 90000), 90, NULL) * 10000;
        }
        else if (eSeekDerection == SEEK_BACKWARD)
        {
            u8TempValue = u8Div6432((UINT64)(OGG_FB_SEEK_TIME * 90000), 90, NULL) * 10000;
        }

        if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl != NULL) &&
            (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui8_timeunit > 0))
        {
            u8DiffGranule =  u8Div6432(u8TempValue, 
                                       prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl->ui8_timeunit,
                                       NULL);
        }

        if (u8AverageGran > 0)
        {
            u4SeekLenth = u8Div6432(u8DiffGranule * 100, u8AverageGran, NULL) * u8AverageLen;
        }
    }
    else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
    {
        //data total size
        u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);
        u4SeekTime = get_abs(prSwdmxInst->i4PlaySpeed) / SWDMX_OGG_BASE_SPEED;
        u4SeekLenth = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond * u4SeekTime;
        if(u4CurPts - u4SeekTime > 0)
         {
            u4CurPts -= u4SeekTime;
        }
        else
        {
            u4CurPts = 0;
        }
    }
    return u4SeekLenth;
}

static BOOL _SWDMX_OggFindHeader(SWDMX_INFO_T* prSwdmxInst,
                                 UCHAR *pucBitstream, 
                                 UINT32* pu4ConsumedSize, 
                                 UINT32 u4AvailSize)
{
    UCHAR ucData[4] = {0};
    UCHAR *pucStream = NULL;

    while (u4AvailSize > (OGG_HEADER_SIZE + 0xFF))
    {
        pucStream = pucBitstream;
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucBitstream, &pucStream, ucData, 4);
        if (_SWDMX_OggCheckHeader(ucData) == TRUE)
        {
            return TRUE;
        }
        (*pu4ConsumedSize)++;
        pucBitstream++;
        u4AvailSize--;
        if ((UINT32)pucBitstream >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
        {
            pucBitstream = (UCHAR*)((UINT32)pucBitstream -
                           VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                           VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
        }
    }
    return FALSE;
}

static UINT32 _SWDMX_OggGetPageSizeAndFrameNum(SWDMX_INFO_T* prSwdmxInst, 
                                               UCHAR *pucBitstream,
                                               UINT8 u1SegNum,
                                               UINT8 *u1FrameNum)
{
    UCHAR ucData[4] = {0};
    UINT32 u4PayloadSize = u1SegNum;

    while (u1SegNum > 0)
    {
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucBitstream, &pucBitstream, ucData, 1);
        u4PayloadSize = u4PayloadSize +  (UINT8)ucData[0];
        if ((UINT8)ucData[0] < 0xFF)
        {
            (*u1FrameNum)++;
        }
        u1SegNum --;
    }
    u4PayloadSize = u4PayloadSize + OGG_HEADER_SIZE;
    return u4PayloadSize;
}

static BOOL _SWDMX_OggJudgeKey(SWDMX_INFO_T* prSwdmxInst,
                               UCHAR *pucBitstream, 
                               UINT8 u1SegNum, 
                               UINT8 *u1KeyNum,
                               BOOL fgFreshPage)
{
    BOOL fgFindKey = FALSE;
    UCHAR ucData[4] = {0};
    UINT8 u1PayDate = 0;
    UINT32 u4PayloadSize = 0;
    UCHAR *pucPayLoadPos = pucBitstream + u1SegNum;

    if ((UINT32)pucPayLoadPos >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        pucPayLoadPos = (UCHAR*)((UINT32)prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4CurDmxFeederRPtr -
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                 VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
    }

    if (fgFreshPage)
    {
        u1PayDate = (UINT8) * pucPayLoadPos;
        if (u1PayDate == 0x08)
        {
            fgFindKey = TRUE;
        }
    }

    while (u1SegNum > 0)
    {
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucBitstream, &pucBitstream, ucData, 1);
        u1SegNum--;

        if ((UINT8)ucData[0] == 0xff)
        {
            u4PayloadSize = u4PayloadSize + (UINT8)ucData[0];
        }
        else
        {
            u4PayloadSize = u4PayloadSize + (UINT8)ucData[0];
            pucPayLoadPos = pucPayLoadPos + u4PayloadSize;
            u4PayloadSize = 0;
            (*u1KeyNum)++;
            if ((UINT32)pucPayLoadPos >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                pucPayLoadPos = pucPayLoadPos - 
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }

            u1PayDate = (UINT8) * pucPayLoadPos;
            if (u1PayDate == 0x08)
            {
                fgFindKey = TRUE;
                break;
            }
        }
    }
    return fgFindKey;
}

static BOOL _SWDMX_OggSeekKeyPage(SWDMX_INFO_T* prSwdmxInst, 
                                  SWDMX_RANGE_LIST_T* prRangeList,
                                  UCHAR *pucBitstream, 
                                  UINT32 u4AvailSize,
                                  UINT32 *pu4ConsumeSize, 
                                  SEEK_DERECTION eSeekDerection,
                                  UINT32 u4SeekTime)
{
    BOOL    fgFindKey = FALSE;
    BOOL    fgPage = FALSE;
    BOOL    fgSeekDone = FALSE;

    switch (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus)
    {
        case eSWDMX_OGG_TIME_SEEK:
        {
            fgPage = _SWDMX_OggFindVideoPage(prSwdmxInst, 
                                             pucBitstream, 
                                             pu4ConsumeSize, 
                                             u4AvailSize);

            if (fgPage)
            {
                u4AvailSize -= *pu4ConsumeSize;
                pucBitstream = (UCHAR*)((UINT32)pucBitstream + *pu4ConsumeSize);
                if ((UINT32)pucBitstream >=
                    VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    pucBitstream = (UCHAR*)((UINT32)pucBitstream -
                                            VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                }
                
                fgSeekDone = _SWDMX_OggSeek(prSwdmxInst, 
                                            prRangeList, 
                                            pucBitstream, 
                                            u4SeekTime,
                                            u4AvailSize,
                                            pu4ConsumeSize);
                if (fgSeekDone)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_KEY;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_HEADER;
                }
            }
        }
        break;
        case eSWDMX_OGG_SEEK_LEN:
        {
            UINT32 u4SeekLen = 0;
            if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
            {
                u4SeekLen = _SWDMX_OggSeekLen(prSwdmxInst,
                                              eSeekDerection,
                                              prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prVidIdxTbl);
            }
            else if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
            {
                u4SeekLen = _SWDMX_OggSeekLen(prSwdmxInst, 
                                              eSeekDerection,
                                              prSwdmxInst->rFmtInfo.rSwdmxOggInfo.prAudIdxTbl[prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1CurATrackIdx]);
            }
            if (eSeekDerection == SEEK_FORWARD)
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos += u4SeekLen;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_HEADER;
            }
            else if (eSeekDerection == SEEK_BACKWARD)
            {
                if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnVideo)
                {
                    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos > u4SeekLen)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FilePosBeforeSeek =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos - u4SeekLen;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                    }
                    else
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                            prRangeList->prDmxRangeInfo->ui8_fileoffset;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFirstFrame = TRUE;
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_HEADER;
                }
                else if(prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgEnAudio)
                {
                    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos > prRangeList->prDmxRangeInfo->ui8_fileoffset) &&
                        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos - prRangeList->prDmxRangeInfo->ui8_fileoffset > u4SeekLen))
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos - u4SeekLen;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos;
                    }
                    else  
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = TRUE;
                    }
                    if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos > prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos)
                    {
                        LOG(1, "_SWDMX_OggSeekKeyPage current UI pos[0x%llx] < u8PlayPos[0x%llx] ,should reset u8PlayPos \n",
                                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos,
                                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos);
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreviousUIPos;
                    }    
                }
            }
            _SWDMX_OggResetFeeder(prSwdmxInst);
        }
        break;
        case eSWDMX_OGG_FIND_HEADER:
        {
            fgPage = _SWDMX_OggFindHeader(prSwdmxInst, (UCHAR*)pucBitstream,
                                          pu4ConsumeSize, u4AvailSize);
            if (fgPage)
            {
                if (eSeekDerection == SEEK_KEY)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_TIME_SEEK;
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_FIND_KEY;
                }
            }
        }
        break;
        case eSWDMX_OGG_FIND_KEY:
        {
            BOOL   fgFreshPage = FALSE;

            UINT8  u1StreamType = 0;
            UINT8  u1PageSegment = 0;
            UINT8  u1TrackIdx = 0;
            UINT8  u1FrameNum = 0;

            UINT32 u4PageSize = 0;
            UINT32 u4Pts = 0;

            UINT64 u8GranulePos = 0;

            while (u4AvailSize > OGG_HEADER_SIZE + 0xFF)
            {
                fgFreshPage = _SWDMX_OggPageHdrPrs(prSwdmxInst,
                                                   (pucBitstream + 4),
                                                   &u8GranulePos, &u1StreamType, 
                                                   &u1PageSegment, &u1TrackIdx);

                u4PageSize = _SWDMX_OggGetPageSizeAndFrameNum(prSwdmxInst,
                                                              (pucBitstream + OGG_HEADER_SIZE),
                                                              u1PageSegment, &u1FrameNum);

                if (u4AvailSize >= u4PageSize)
                {
                    if (u1StreamType == eSWDMX_MEDIA_VIDEO)
                    {
                        UINT8 u1KeyNum = 0;

                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos = u8GranulePos;
                        _SWDMX_OggCalculatePts(prSwdmxInst, u1StreamType, 0, u8GranulePos, &u4Pts);

                        fgFindKey = _SWDMX_OggJudgeKey(prSwdmxInst,
                                                       (pucBitstream + 27),
                                                       u1PageSegment, &u1KeyNum, fgFreshPage);
                        if (fgFindKey)
                        {
                            if (stKeyinfo.u1CurIdx < KEY_LEN)
                            {
                                stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u8KeyOffset =
                                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos + *pu4ConsumeSize;
                                stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u4KeyPts = u4Pts;
                                stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u8GranulPos =
                                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos ;
                                LOG(1, " key pts add:%x offset = %x\n", 
                                        stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u4KeyPts, 
                                        (int)stKeyinfo.stKeyData[stKeyinfo.u1CurIdx].u8KeyOffset);
                                stKeyinfo.u1CurIdx++;
                                if (stKeyinfo.u1CurIdx >= KEY_LEN)
                                {
                                    stKeyinfo.u1CurIdx = stKeyinfo.u1CurIdx % KEY_LEN;
                                    stKeyinfo.fgRing = TRUE;
                                }
                            }
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep = 0;
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_SEEK_LEN;
                            break;
                        }
                    }
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4PageCount++;

                    if (u4Pts > prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep++;
                        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep * OGG_TEN_SEC_DURATION < 
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts)
                        {
                            LOG(1, "Cannot find Kframe, u4Pts=0x%X, u4VSeekPts=0x%X\n", u4Pts, 
                                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4VSeekPts);
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_TIME_SEEK;
                        }
                        else
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSeekDone = TRUE;
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u4ReSeekStep = 0;
                        }
                        break;
                    }

                    pucBitstream += u4PageSize;
                    u4AvailSize -= u4PageSize;
                    *pu4ConsumeSize += u4PageSize;
                    if ((UINT32)pucBitstream >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                    {
                        pucBitstream = (UCHAR*)((UINT32)pucBitstream -
                                                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                    }

                    if ((eSeekDerection == SEEK_BACKWARD) &&
                        ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos + *pu4ConsumeSize) >
                         prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8FilePosBeforeSeek))
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_SEEK_LEN;
                        if (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFirstFrame)
                        {
                            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgSendOver = TRUE;
                        }
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            if (!fgFindKey && (eSeekDerection == SEEK_BACKWARD) &&
                (u4AvailSize <= OGG_HEADER_SIZE + 0xFF))
            {
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos =
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurMoviPos =
                    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8PreDmxPos;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.fgFeederInvalid = TRUE;
                prSwdmxInst->rFmtInfo.rSwdmxOggInfo.eActStatus = eSWDMX_OGG_SEEK_LEN;
                LOG(1, "[%s][%d] Not find Key frame, set u8CurDmxMoviPos 0x%llX\n", __FUNCTION__, __LINE__,
                        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8CurDmxMoviPos); 
            }
        }
        break;
        default:
            break;
    }
    return fgFindKey;
}

static BOOL _SWDMX_OggFindVideoPage(SWDMX_INFO_T* prSwdmxInst,
                                    UCHAR *pucBitstream, 
                                    UINT32* pu4ConsumedSize, 
                                    UINT32 u4AvailSize)
{
    UCHAR *pucStream = pucBitstream;
    UINT8 u1StreamType = 0;
    UINT8 u1PageSegment = 0;
    UINT8 u1TrackIdx = 0;
    UINT8 u1FrameNum = 0;
    UINT32 u4PageSize = 0;
    UINT64 u8GranulePos = 0;
    UCHAR ucData[4] = {0};

    while (u4AvailSize > OGG_HEADER_SIZE + 0xFF)
    {
        _SWDMX_OggGetSomeBytes(prSwdmxInst, pucStream, &pucStream, ucData, 4);

        if (_SWDMX_OggCheckHeader(ucData) != TRUE)
        {
            return FALSE;
        }
        _SWDMX_OggPageHdrPrs(prSwdmxInst, pucStream,  &u8GranulePos,
                             &u1StreamType, &u1PageSegment, &u1TrackIdx);

        if (u1StreamType == eSWDMX_MEDIA_VIDEO)
        {
            prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8VidGranulePos = u8GranulePos;
            return TRUE;
        }

        else
        {
            u4PageSize = _SWDMX_OggGetPageSizeAndFrameNum(prSwdmxInst,
                         (pucStream + (OGG_HEADER_SIZE - 4)),
                         u1PageSegment, &u1FrameNum);
            if (u4PageSize <= u4AvailSize)
            {
                u4AvailSize -= u4PageSize;
                *pu4ConsumedSize += u4PageSize;
                pucStream = (UCHAR*)((UINT32)pucStream + u4PageSize);
                if ((UINT32)pucStream >=
                        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                {
                    pucStream = (UCHAR*)((UINT32)pucStream -
                                         VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) +
                                         VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                }
            }
            else
            {
                break;
            }
        }
    }
    return FALSE;
}

static BOOL _SWDMX_OggGetVorbisTblSize(SWDMX_INFO_T *prSwdmxInst, UINT32 *pu4VorbisTblSize)
{
    UINT8 u1TrackIdx = 0;
    UINT32 u4TrackTblSize = 0;
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T*)&prSwdmxInst->rFmtInfo.rSwdmxOggInfo;
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxOggInfo);

    if (pu4VorbisTblSize == NULL)
    {
        return FALSE;
    }

    *pu4VorbisTblSize = 0;
    for (u1TrackIdx=0; u1TrackIdx<prSwdmxOggInfo->u1TotalATrack; u1TrackIdx++)
    {
        u4TrackTblSize = prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_bos_header_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_vorbis_header_offset_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_comment_header_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_comment_offset_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_setup_header_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_setup_offset_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_continue_header_size +
                          prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_continue_size;
        if (prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_comment_header_offset ==
            prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_setup_header_offset)
        {
            u4TrackTblSize -= prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx].ui4_packet_setup_header_size;
        }
        prSwdmxOggInfo->u4AudHdrLength[u1TrackIdx] = u4TrackTblSize;
        *pu4VorbisTblSize += u4TrackTblSize;
    }
    LOG(1, "Get vorbis table size(0x%x)\n", *pu4VorbisTblSize);
    
    return TRUE;
}

BOOL _SWDMX_OggSaveVorbisTbl(UINT8 u1SrcId, UINT8 u1TrackIdx, UCHAR *pucBitstream)
{
    UCHAR *pucTgtBuf = NULL, *pucBitStreamBase = NULL;
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_OGG_INFO_T *prSwdmxOggInfo = NULL;
    MM_AUDIO_VORBIS_OFFSET_T *prAudVorbisOffset = NULL;

    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxOggInfo = (SWDMX_OGG_INFO_T *)&prSwdmxInst->rFmtInfo.rSwdmxOggInfo;
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxOggInfo);
    prAudVorbisOffset = (MM_AUDIO_VORBIS_OFFSET_T *)&prSwdmxOggInfo->AudVorbisOffset[u1TrackIdx];
    SWDMX_OGG_VERIFY_NULL_ERROR(prAudVorbisOffset);

    pucBitStreamBase = pucBitstream - prSwdmxOggInfo->u8CurDmxMoviPos;
    if ((UINT32)pucBitStreamBase < prSwdmxInst->rFeederInfo.u4StartAddr)
    {
        LOG(0, "%s: vorbis table pos exceed feeder length!", __FUNCTION__);
        return FALSE;
    }

    pucTgtBuf = (UINT8 *)prSwdmxOggInfo->u4AudHeaderPtr[u1TrackIdx];

    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_packet_bos_header_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_packet_bos_header_size);

    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_vorbis_header_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_vorbis_header_offset_size);
    
    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_packet_comment_header_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_packet_comment_header_size);

    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_comment_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_comment_offset_size);

    if (prAudVorbisOffset->ui4_packet_setup_header_offset !=
        prAudVorbisOffset->ui4_packet_comment_header_offset)
    {
        pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_packet_setup_header_offset;
        pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                     pucBitstream, 
                                     prAudVorbisOffset->ui4_packet_setup_header_size);
    }

    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_setup_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_setup_offset_size);

    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_packet_continue_header_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_packet_continue_header_size);
    
    pucBitstream = pucBitStreamBase + prAudVorbisOffset->ui4_continue_offset;
    pucTgtBuf = _SWDMX_OggMemcpy(pucTgtBuf, 
                                 pucBitstream, 
                                 prAudVorbisOffset->ui4_continue_size);
     
    return TRUE;
}

static UINT32 _SWDMX_OggGetAudFifoData(SWDMX_INFO_T *prSwdmxInst)
{
    UINT32 u4AudFifoAddr, u4AudFifoSize;
    UINT8 u1PidIdx = 0;
    UINT8 u1PidType = 0;
    UINT32 u4DmxAvailSize = 0;
    UINT32 u4AudFifoAvailData = 0;
    
    u1PidIdx = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1AudPid;
    u1PidType = DMX_PID_TYPE_ES_AUDIO;

    u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                          (DMX_PID_TYPE_T)u1PidType,
                                          u1PidIdx);

    _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);
    u4AudFifoAvailData = u4AudFifoSize -u4DmxAvailSize;
    return u4AudFifoAvailData;
}

static BOOL _SWDMX_OggWaitVDecPlay(SWDMX_INFO_T *prSwdmxInst, UINT32 u4Flag)
{
#define _LOOP_CNT 200
    UINT32 u4Cnt = 0;
    UINT32 u4Status;

    while(u4Cnt < _LOOP_CNT)
    {
        u4Status = MPV_GetDecMode(prSwdmxInst->u1VdecId);
        if ((u4Status == VDEC_ST_STOP) || (u4Status == VDEC_ST_IDLE))
        {
            x_thread_delay(10);
            u4Cnt++;
        }
        else
        {
            break;
        }
    }
    if (u4Status == VDEC_ST_STOP || u4Status == VDEC_ST_IDLE)
    {
        LOG(0, "(%d)SWDMX_Ogg: VDec no response! count: %d, vdec-status: %d\n",
                u4Flag, u4Cnt, u4Status);
        return FALSE;
    }
    else
    {
        LOG(1, "(%d)SWDMX_Ogg: wait VDec, count: %d, vdec-status: %d\n", 
                u4Flag, u4Cnt, u4Status);
        return TRUE;
    }
}

/*
 * init position linear interpolation calculation
*/
static void _SWDMX_OggResetLerpPos(SWDMX_INFO_T *prSwdmxInst)
{
    SWDMX_OGG_VERIFY_NULL(prSwdmxInst);
    prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LerpLastPos = (UINT64)-1;
}

/*
 * get position linear interpolation value
*/
static BOOL _SWDMX_OggGetLerpPos(SWDMX_INFO_T *prSwdmxInst, UINT64 *pu8CurPos)
{
    HAL_TIME_T rCurTime;
    static HAL_TIME_T rLastChgPosTime;
    
    SWDMX_OGG_VERIFY_NULL_ERROR(prSwdmxInst);

    // When ByteRate <= 3000, use it to estimate position
    if ((pu8CurPos == NULL) ||
        (prSwdmxInst->i4PlaySpeed != 1000) ||
        (prSwdmxInst->eCurFMT != SWDMX_FMT_OGG_AUDIO_ES) ||
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond == 0))
    {
        return FALSE;
    }
    if ((prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u1TotalATrack == 1) &&
        (prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond > 3000))
    {
        return FALSE;
    }
    
    HAL_GetTime(&rCurTime);
    if (*pu8CurPos == prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LerpLastPos)
    {
        HAL_TIME_T rDelta;
        UINT64 u8Remainder = 0;
        UINT64 u8DeltaPos = prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8AvgDataSizePerSecond;

        HAL_GetDeltaTime(&rDelta, &rLastChgPosTime, &rCurTime);
        u8DeltaPos *= (UINT64)rDelta.u4Seconds * 1000000 + rDelta.u4Micros;
        u8DeltaPos = _SWDMX_Div6464(u8DeltaPos, (UINT64)1000000, &u8Remainder);
        *pu8CurPos += u8DeltaPos;
    }
    else
    {
        rLastChgPosTime = rCurTime;
        prSwdmxInst->rFmtInfo.rSwdmxOggInfo.u8LerpLastPos = *pu8CurPos;
    }
    
    return TRUE;
}

