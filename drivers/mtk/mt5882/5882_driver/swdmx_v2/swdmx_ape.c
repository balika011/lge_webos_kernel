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
 * $RCSfile: swdmx_ape.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_ape.c
 *  Software demux for ape - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_if.h"
#include "dmx_mm_if.h"
#include "aud_drvif.h"
#include "aud_if.h"
#include "swdmx_drvif.h"
#include "stc_drvif.h"
#include "swdmx_debug.h"
#include "swdmx_ape.h"

#ifdef SWDMX_DUMP_DATA
#include "swdmx_debug_util.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
// APE Feeder RETURN VAL
#define APE_REQ_OK              0
#define APE_REQ_END             1
#define APE_NO_INSTANSE         2
#define APE_REQ_FAIL            3
#define APE_REQ_NONE            4
#define APE_ACK_FAIL            5
#define APE_ACK_OK              6
#define APE_HAVE_DATA           7
#define APE_NO_DATA             8
// APE SEND DATA RETURN VAL
#define APE_SEND_OK             0
#define APE_SEND_FULL           1
#define APE_SEND_FAIL           2
#define APE_SEND_EOS            3
#define APE_SEND_LAST           4
#define APE_SEND_WAIT           6
        
#define APE_ATHRESHOLD_MAX      (200 * 1024)
#define APE_ATHRESHOLD_MIN      (20 * 1024)
#define APE_SYNC_WORD_SIZE      (256)

const UINT8 uc1Mod4Map[4][4] = { \
    {0, 1, 2, 3}, \
    {3, 0, 1, 2}, \
    {2, 3, 0 ,1}, \
    {1, 2, 3, 0}  \
};
#define APE_MOD4(x)                 (uc1Mod4Map[prSwdmxApe->u1AlignBase][(x) % 4])
#define APE_CUT4(x)                 ((x) - APE_MOD4(x))

#define IS_INVAILD_FILESIZE(x)      ((x) == 0xFFFFFFFFUL)


#define SWDMX_APE_VERIFY_NULL(ptr) \
    do { \
        if (ptr == NULL) { \
            LOG(0, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
            return; \
        } \
    } while (0)
          
#define SWDMX_APE_VERIFY_NULL_ERROR(ptr) \
    do { \
        if (ptr == NULL) { \
            LOG(0, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
            return FALSE; \
        } \
    } while (0)



void _SWDMX_ApePrintHex(UINT8 *pu1Addr, UINT32 u4Len)
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

BOOL _SWDMX_ApeGetRingData(UINT8 *pu1RingStartAddr, UINT8 *pu1RingEndAddr, UINT8 *pu1ReadAddr, UINT8 *pu1data, UINT32 u4ReadSize)
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

UINT32 _SWDMX_ApeGetCurFramePos(UINT8 u1SrcId, UINT32 u4FrmIdx)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    if (u4FrmIdx < prSwdmxApe->u4TotalFrame)
    {
        return prSwdmxApe->pu4SeekTable[u4FrmIdx];
    }
    else
    {
        return prSwdmxInst->u8FileSize;
    }
}

UINT32 _SWDMX_ApeGetFrameIdx(UINT8 u1SrcId, UINT32 u4Pos)
{
    UINT32 u4FrameIdx;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    for (u4FrameIdx=1; u4FrameIdx<prSwdmxApe->u4TotalFrame; u4FrameIdx++)
    {
        if (u4Pos < prSwdmxInst->rFmtInfo.rSwdmxApeInfo.pu4SeekTable[u4FrameIdx])
        {
            u4FrameIdx--;
            break;
        }
    }

    return u4FrameIdx;
}

/****reset the sended position****/
VOID _SWDMX_ApeSetSendedPos(UINT8 u1SrcId,UINT64 u8SendedPos)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    prSwdmxApe->u8SendedSize = u8SendedPos;
    prSwdmxApe->u8ReadedPos = u8SendedPos;
    prSwdmxApe->u8PlayPos = u8SendedPos;
    prSwdmxApe->u8PreviousSendedSize = u8SendedPos;
}

/****set req size and move size one time****/
void _SWDMX_ApeSetReqSize(UINT8 u1SrcId)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    prSwdmxApe->u4OneTimeSize = MPG_BATCH_SIZE;
    prSwdmxApe->u4OneTimeReadSize = MPG_BATCH_SIZE * MPG_BATCH_READ_SIZE;

    LOG(5, "set move data size %d req data size %d\n",
        prSwdmxApe->u4OneTimeSize,
        prSwdmxApe->u4OneTimeReadSize);
}

VOID _SWDMX_ApeDetectStrmChgInfo(UINT8 u1SrcId)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    if (prSwdmxApe->rRecvStreamChgInfo.fgDisableAudio)
    {
        LOG(5, "disable audio\n");
        prSwdmxApe->rRecvStreamChgInfo.fgDisableAudio = FALSE;
        if (prSwdmxApe->fgAudio)
        {
            prSwdmxApe->fgSendAudioEos = FALSE;
            prSwdmxApe->fgAudio = FALSE;
        }
    }

    if (prSwdmxApe->rRecvStreamChgInfo.fgEnableAudio)
    {
        LOG(5, "enable audio\n");
        prSwdmxApe->rRecvStreamChgInfo.fgEnableAudio = FALSE;
        if (!prSwdmxApe->fgAudio)
        {
            prSwdmxApe->fgGotAudioReady = FALSE;
            prSwdmxApe->fgSendAudioEos = FALSE;
            prSwdmxApe->fgAudio = TRUE;
        }
    }

}

BOOL _SWDMX_ApeInitDmx(UINT8 u1SrcId,UINT8 u1StrmType)
{
    BOOL fgRet = FALSE;
    UINT8 u1Pidx;
    UINT32 u4Flags;
    UINT32 u4Addr, u4Size;
    DMX_MM_T rDmxMMInfo;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    LOG(5, "%s SrcId %d StrmType %d\n", __FUNCTION__, u1SrcId, u1StrmType);
    
    switch (u1StrmType)
    {
        case eSWDMX_STRM_TYPE_AUD:
        {
            u1Pidx = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            if (u1Pidx >= DMX_NUM_PID_INDEX)
            {
                LOG(3, "DMX_MUL_GetAvailablePidx Fail\n");
                return FALSE;
            }
            if (prSwdmxApe->u1PidxAudio != 0xFF)
            {
                LOG(3, " swdmx ape enable audio es u1PidxAudio != 0xFF\n");
                return FALSE;
            }
            
            x_memset(&rDmxMMInfo, 0, sizeof(rDmxMMInfo));
            
            u4Flags = (UINT32)(DMX_MM_FLAG_TYPE | DMX_MM_FLAG_NOTIFY |
                               DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG |
                               DMX_MM_FLAG_DEVICE_ID);
            rDmxMMInfo.fgEnable = TRUE;
            rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_AUDIO;
            rDmxMMInfo.fgAllocBuf = FALSE;
            if (!_SWDMX_GetAudioFifo(u1SrcId, &u4Addr, &u4Size))
            {
                LOG(3, "_SWDMX_MpgEsSetAudioId _SWDMX_GetAudioFifo Fail\n");
                return FALSE;
            }
            rDmxMMInfo.u4BufAddr = u4Addr;
            rDmxMMInfo.u4BufSize = u4Size;
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
            rDmxMMInfo.u1ChannelId = prSwdmxInst->u1AdecId;
            rDmxMMInfo.u1DeviceId = prSwdmxInst->u1AdecId;
            fgRet = DMX_MM_Set(u1Pidx, u4Flags, &rDmxMMInfo);
            if (!fgRet)
            {
                LOG(3, "_SWDMX_MpgEsSetAudioId DMX_MM_Set Fail\n");
                return FALSE;
            }
            
            prSwdmxApe->u1PidxAudio = u1Pidx;
            prSwdmxApe->fgAudio = TRUE;
            prSwdmxApe->fgGotAudioReady = FALSE;
            prSwdmxApe->fgSendAudioEos = FALSE;
            break;
        }
        default:
            LOG(0,"not match stream type,pls check it.\n");
            ASSERT(0);
            break;
    }
    return TRUE;
}

/****this is used to flush audio fifo****/
VOID _SWDMX_ApeFlushFifo(UINT8 u1SrcId)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T * prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    if (prSwdmxApe->u1PidxAudio != 0xFF)
    {
        if (prSwdmxApe->u4LastAudioWptr != 0)
        {
            if (!(DMX_UpdateReadPointer(prSwdmxApe->u1PidxAudio,
                                        prSwdmxApe->u4LastAudioWptr,
                                        prSwdmxApe->u4LastAudioWptr)))
            {
                if (prSwdmxInst->u4LogFilter & check_data_flush)
                    LOG(5, "%s DMX_UpdateReadPointer Fail\n", __FUNCTION__);
            }
            prSwdmxApe->u4LastAudioWptr = 0;
        }
        UNUSED(DMX_MM_FlushBuffer(prSwdmxApe->u1PidxAudio));
    }
}

/****this is used to flush second video fifo****/
BOOL _SWDMX_ApeResetFeeder(UINT8 u1SrcId)
{
    UINT32 u4ReqCnt = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T * prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
            FEEDER_E_OK)
    {
        if (prSwdmxInst->u4LogFilter & check_data_flush)
            LOG(0, "%s feeder input buffer error.\n", __FUNCTION__);
        return FALSE;
    }

    prSwdmxApe->u4FeederReqID += 1000; // always a even value
    prSwdmxApe->u4BufAvailCnt = 0;
    prSwdmxApe->u4BufReadIdx = 0;
    prSwdmxApe->u4BufWriteIdx = 0;
    prSwdmxApe->u4BufTotalSize = 0;
    prSwdmxApe->u4ReqTimes = 0;
    prSwdmxApe->u4ReqReadIdx = 0;
    prSwdmxApe->u4ReqWriteIdx = 0;
    prSwdmxApe->u4ReqTotalSize = 0;
    prSwdmxApe->u4UsingBufSize = 0;
    prSwdmxApe->u8ReadedPos = prSwdmxApe->u8SendedSize;

    for (u4ReqCnt = 0;u4ReqCnt < prSwdmxApe->u4FeederReqNum;u4ReqCnt++)
    {
        // make it invalid, not a even value
        prSwdmxApe->au4ReqId[u4ReqCnt] = 1;
    }

    FeederSetReadyThreshold(prSwdmxInst->eFeederSrc, MPG_DLNA_FEEDER_UNREACH);
    FeederSetUnderFlowThreshold(prSwdmxInst->eFeederSrc, MPG_DLNA_FEEDER_UNREACH);
    FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_THRESHOLD, FALSE);
    // Update read ptr to feeder
    if (prSwdmxInst->rFeederInfo.u4WriteAddr)
    {
        if (FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, prSwdmxInst->rFeederInfo.u4WriteAddr) != FEEDER_E_OK)
        {
            if (prSwdmxInst->u4LogFilter & check_data_flush)
                LOG(0, "%s FeederUpdateReadPointer fail\n", __FUNCTION__);
        }
    }
    
    return TRUE;
}

VOID _SWDMX_ApeResetData(UINT8 u1SrcId)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    //flush afifo
    _SWDMX_ApeFlushFifo(u1SrcId);
    //flush req msg Q
    _SWDMX_FlushReqMsgQ(u1SrcId);
    //reset feeder
    _SWDMX_ApeResetFeeder(u1SrcId);
    //clear eos flag
    prSwdmxApe->fgSendEOF = FALSE;
    prSwdmxApe->fgSendAudioEos = FALSE;
    prSwdmxApe->fgGetEof = FALSE;
    //clear a/v ready flag for start to play
    prSwdmxApe->fgGotAudioReady = FALSE;
    //reset first flag
    prSwdmxApe->fgFirst = TRUE;

    //change range status
    if (prSwdmxApe->prCurRangeReq)
    {
        prSwdmxApe->prCurRangeReq->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
    return ;

}

/****this is judge the file is all sended or not****/
static BOOL _SWDMX_ApeJudgeFileEnd(UINT8 u1SrcId, UINT32 u4SendSize)
{
    BOOL fgRet = FALSE;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    if (prSwdmxApe->fgSendEOF)
    {
        fgRet = TRUE;
    }
    else if (!IS_INVAILD_FILESIZE(prSwdmxInst->u8FileSize))
    {
        if ((prSwdmxApe->i4CurSpeed >= 0) &&
                (prSwdmxApe->u8SendedSize + u4SendSize >= prSwdmxInst->u8FileSize))
        {
            LOG(1, "Found file end by file size!\n");
            fgRet = TRUE;
        }
        else if ((prSwdmxApe->i4CurSpeed < 0) &&
                 (prSwdmxApe->u8SendedSize <= prSwdmxInst->u8FileOffset))
        {
            LOG(1, "Found file start by file size!\n");
            fgRet = TRUE;
        }
    }
    // get eos from mw, and no data to move, end file
    if ((prSwdmxApe->fgGetEof) &&
            (prSwdmxApe->u4UsingBufSize <= u4SendSize) &&
            (!prSwdmxApe->u4BufAvailCnt))
    {
        LOG(1, "Found file end by feeder eof!\n");
        fgRet = TRUE;
    }
    return fgRet;
}

VOID _SWDMX_ApeHandleEos(UINT8 u1SrcId)
{
    DMX_AUDIO_PES_T rPes = {0};
    UINT32 buf_start = 0, buf_end = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    if (prSwdmxApe->fgSendAudioEos)
    {
        return;
    }
    
    if (prSwdmxApe->u4LastAudioWptr)
    {
        rPes.u4Wp = prSwdmxApe->u4LastAudioWptr;
    }
    else
    {
        VERIFY(AUD_GetAudFifo(prSwdmxInst->u1AdecId, &buf_start, &buf_end) == AUD_OK);
        rPes.u4Wp = (buf_start + 4);
        rPes.fgEOS = TRUE;
    }
    UNUSED(AUD_SendAudioPes(&rPes)); // audio driver said they need this one.

    rPes.fgForward = (prSwdmxApe->i4CurSpeed >= 0) ? TRUE : FALSE;
    rPes.fgEOS = TRUE;
    UNUSED(AUD_SendAudioPes(&rPes));
    
    prSwdmxApe->fgSendAudioEos = TRUE;
    LOG(0, "Send audio eos, fgForward(%d)\n", rPes.fgForward);
}

INT32 _SWDMX_ApeFeederReq(UINT8 u1SrcId)
{
    FEEDER_TRANSMIT_COND rFeederCond;
    UINT64 u8ReadAddr = 0;  /****this is read address of file****/
    UINT32 u4ReadSize = 0;
    INT32 i4Ret = -1;
    BOOL u4ReqTimes = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    u8ReadAddr = prSwdmxApe->u8ReadedPos + prSwdmxApe->u4ReqTotalSize + 
                 prSwdmxApe->u4BufTotalSize;
    u4ReadSize = prSwdmxApe->u4OneTimeReadSize;
    if (u8ReadAddr >= prSwdmxInst->u8FileSize)
    {
        return APE_REQ_END;
    }
    if (u4ReadSize + u8ReadAddr > prSwdmxInst->u8FileSize)
    {
        u4ReadSize = prSwdmxInst->u8FileSize - u8ReadAddr;
    }

    /****judge req or not****/
    if (u4ReadSize > 0)
    {
        if (prSwdmxApe->u4ReqTimes + prSwdmxApe->u4BufAvailCnt + 1 <=
            prSwdmxApe->u4FeederReqNum)
        {
            u4ReqTimes = prSwdmxApe->u4FeederReqNum
                         - prSwdmxApe->u4BufAvailCnt
                         - prSwdmxApe->u4ReqTimes;
        }
    }

    if (u4ReqTimes)
    {
        x_memset(&rFeederCond, 0, sizeof(FEEDER_TRANSMIT_COND));
        i4Ret = FeederSetRequest(prSwdmxInst->eFeederSrc,
                                 u4ReadSize,
                                 u8ReadAddr,
                                 &rFeederCond,
                                 prSwdmxApe->u4FeederReqID);
        if (i4Ret != FEEDER_E_OK)
        {
            LOG(0, "req id = %d failed!\n", prSwdmxApe->u4FeederReqID);

            if (i4Ret == FEEDER_E_BUFFER_FULL)
            {
                LOG(0, "found feeder full, check it!\n");
            }
            return APE_REQ_FAIL;
        }

        if (prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "req id(%d), size(0x%X)\n",
                prSwdmxApe->u4FeederReqID,
                u4ReadSize);
        }

        prSwdmxApe->au4ReqId[prSwdmxApe->u4ReqWriteIdx] = prSwdmxApe->u4FeederReqID;
        prSwdmxApe->au4ReqSize[prSwdmxApe->u4ReqWriteIdx] = u4ReadSize;
        prSwdmxApe->u4ReqTotalSize += u4ReadSize;
        prSwdmxApe->u4FeederReqID += 2;
        prSwdmxApe->u4ReqTimes++;
        prSwdmxApe->u4ReqWriteIdx++;
        if (prSwdmxApe->u4ReqWriteIdx >= prSwdmxApe->u4FeederReqNum)
        {
            prSwdmxApe->u4ReqWriteIdx = 0;
        }
    }
    else
    {
        if (prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "ReqTimes(%d) BufAvailCnt(%d) FeederReqNum(%d)\n",
                prSwdmxApe->u4ReqTimes,
                prSwdmxApe->u4BufAvailCnt,
                prSwdmxApe->u4FeederReqNum);
        }
        return APE_REQ_NONE;
    }

    return APE_REQ_OK;
}

INT32 _SWDMX_ApeFeederAck(UINT8 u1SrcId)
{
    INT32 i4Ret = -1;
    BOOL fgRet = FALSE;
    UINT32 u4TmpFeederReqID;
    FEEDER_REQ_DATA_T rFeederAckData;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    /****req exist and file not end****/
    if ((prSwdmxApe->u4ReqTimes) && (!prSwdmxApe->fgGetEof))
    {
        x_memset(&rFeederAckData, 0, sizeof(FEEDER_REQ_DATA_T));
        /****when no data in feeder, should wait.****/
        if ((prSwdmxApe->u4BufAvailCnt == 0) &&
                (prSwdmxApe->u4UsingBufSize == 0))
        {
            fgRet = _SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)&rFeederAckData);

        }
        else
        {
            i4Ret = _SWDMX_ReceiveFeederAckNoWait(prSwdmxInst->u1SwdmxId, (VOID*)&rFeederAckData);
            if (i4Ret == OSR_OK)
            {
                fgRet = TRUE;
            }
            else
            {
                if (prSwdmxInst->u4LogFilter & check_feeder_req)
                    LOG(7, "rec feeder msg fail, i4Ret = %d\n", i4Ret);
            }
        }
        if (fgRet)
        {
            /****timeout****/
            if (rFeederAckData.eDataType == FEEDER_SOURCE_INVALID)
            {
                LOG(0, "invalid data, ack id = %d\n", rFeederAckData.u4Id);
                return APE_ACK_FAIL;
            }
            /****eos from feeder****/
            else if (rFeederAckData.fgEof)
            {
                prSwdmxApe->fgGetEof = TRUE;
                LOG(0, "get eof!");
            }
            /***found feeder wp NULL, return fail directly***/
            else if (rFeederAckData.u4WriteAddr == 0)
            {
                LOG(0, "feeder ack error data, id = %d\n", rFeederAckData.u4Id);
                _SWDMX_ApeResetFeeder(u1SrcId);
                x_thread_delay(30);
                return APE_ACK_FAIL;
            }
            else if ((rFeederAckData.u4WriteAddr < prSwdmxInst->rFeederInfo.u4StartAddr) ||
                (rFeederAckData.u4WriteAddr >= prSwdmxInst->rFeederInfo.u4EndAddr))            
            {
                LOG(0, "feeder ack id(%d) WP(0x%X) outrange[0x%X,0x%X)\n", 
                    rFeederAckData.u4Id, rFeederAckData.u4WriteAddr,
                    prSwdmxInst->rFeederInfo.u4StartAddr, prSwdmxInst->rFeederInfo.u4EndAddr);
                _SWDMX_ApeResetFeeder(u1SrcId);
                x_thread_delay(30);
                return APE_ACK_FAIL;
            }

            u4TmpFeederReqID = prSwdmxApe->u4FeederReqID - prSwdmxApe->u4ReqTimes * 2;
            if (rFeederAckData.u4Id == u4TmpFeederReqID)
            {
                UINT32 req_size = prSwdmxApe->au4ReqSize[prSwdmxApe->u4ReqReadIdx];
                if (prSwdmxInst->u4LogFilter & check_feeder_req)
                {
                    LOG(1, "ack id(%d), size(0x%X)\n", rFeederAckData.u4Id, rFeederAckData.u4ReadSize);

                }
                prSwdmxApe->u4ReqTimes--;
                prSwdmxApe->u4ReqTotalSize -= req_size;
                if (rFeederAckData.u4ReadSize < req_size)
                {
                    req_size = rFeederAckData.u4ReadSize;
                    LOG(0, "ID(%d), req size(0x%X) -> ack size(0x%X)\n",
                        rFeederAckData.u4Id, req_size, rFeederAckData.u4ReadSize);
                }
                prSwdmxApe->au4BufStartAddr[prSwdmxApe->u4BufWriteIdx] =
                    rFeederAckData.u4WriteAddr + rFeederAckData.u4AlignOffset;
                prSwdmxApe->au4BufSize[prSwdmxApe->u4BufWriteIdx] = req_size;
                prSwdmxApe->u4BufTotalSize += req_size;
                prSwdmxApe->u4BufAvailCnt++;
                prSwdmxApe->u4ReqReadIdx++;
                prSwdmxApe->u4BufWriteIdx++;
                if (prSwdmxApe->u4ReqReadIdx >= prSwdmxApe->u4FeederReqNum)
                {
                    prSwdmxApe->u4ReqReadIdx = 0;
                }
                if (prSwdmxApe->u4BufWriteIdx >= prSwdmxApe->u4FeederBufNum)
                {
                    prSwdmxApe->u4BufWriteIdx = 0;
                }

                return APE_ACK_OK;
            }

            else
            {
                if (prSwdmxInst->u4LogFilter & check_feeder_req)
                {
                    LOG(0, "ack id not continuous, u4FeederReqID(%d)-2*u4ReqTimes(%d)=(%d), ack(%d) = %d\n",
                        prSwdmxApe->u4FeederReqID,
                        prSwdmxApe->u4ReqTimes*2,
                        u4TmpFeederReqID,
                        rFeederAckData.u4Id);
                }
            }
        }
    }
    else
    {
        if (prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "reqtimes = %d, fgeos = %d\n",
                prSwdmxApe->u4ReqTimes,
                prSwdmxApe->fgGetEof);
        }
    }
    return APE_ACK_FAIL;


}
VOID _SWDMX_ApeUpdateUsingBuffer(UINT8 u1SrcId)
{
    UINT32 req_size = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    while ((!prSwdmxApe->u4UsingBufSize) && (prSwdmxApe->u4BufAvailCnt))
    {
        req_size = prSwdmxApe->au4BufSize[prSwdmxApe->u4BufReadIdx];
        if (prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "add size(0x%X) to using buffer\n", req_size);
        }
        prSwdmxApe->u8ReadedPos += req_size;
        prSwdmxApe->u4UsingBufSize += req_size;
        prSwdmxApe->u4BufTotalSize -= req_size;
        prSwdmxApe->u4UsingStartAddr = prSwdmxApe->au4BufStartAddr[prSwdmxApe->u4BufReadIdx];
        prSwdmxApe->u4BufAvailCnt--;

        if (prSwdmxInst->u4LogFilter & check_move_data)
        {
            LOG(1," au4BufStartAddr[%d] 0x%x UsingBufSize 0x%x u4BufAvailCnt 0x%x \n",
                prSwdmxApe->u4BufReadIdx,
                prSwdmxApe->u4UsingStartAddr,
                prSwdmxApe->u4UsingBufSize,
                prSwdmxApe->u4BufAvailCnt);
        }
        prSwdmxApe->u4BufReadIdx++;
        if (prSwdmxApe->u4BufReadIdx >= prSwdmxApe->u4FeederBufNum)
        {
            prSwdmxApe->u4BufReadIdx = 0;
        }

    }
}

INT32 _SWDMX_ApeHandleFeeder(UINT8 u1SrcId)
{
    INT32 i4Ret = -1;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    i4Ret = _SWDMX_ApeFeederReq(u1SrcId);
    switch(i4Ret)
    {
        case APE_REQ_END:
            if(prSwdmxInst->u4LogFilter & check_feeder_req)
            {
                LOG(1, "all data requested and delay 10ms !\n");
            }
            x_thread_delay(10);
            break;
        case APE_REQ_FAIL:
            x_thread_delay(10);
            break;
        case APE_REQ_NONE:
            break;
        case APE_REQ_OK:
            break;
        default:
            ASSERT(0);            
            break;       
    }

    if (prSwdmxApe->u4ReqTimes)
    {
        i4Ret = _SWDMX_ApeFeederAck(u1SrcId);
        switch(i4Ret)
        {
            case APE_ACK_OK:
                break;
            case APE_ACK_FAIL:
                break;
            default:
                ASSERT(0);            
                break;       
        }
    }
    if ((!prSwdmxApe->u4UsingBufSize) && (prSwdmxApe->u4BufAvailCnt))
    {
        _SWDMX_ApeUpdateUsingBuffer(u1SrcId);
    }
    if (prSwdmxApe->u4UsingBufSize > 0)
    {
        return APE_HAVE_DATA;
    }
    else 
    {
        return APE_NO_DATA;
    }
}

VOID _SWDMX_ApeGetAThreshold(UINT8 u1SrcId,UINT32 * p4AudioThreshold)
{
    UINT32 u4AudioThreshold = 0;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);

    u4AudioThreshold = APE_ATHRESHOLD_MAX;
    if (90000 <= prSwdmxInst->u4TotalTime)
    {
        u4AudioThreshold = _SWDMX_Div6464(prSwdmxInst->u8FileSize,
                                          prSwdmxInst->u4TotalTime/90000, NULL);
        u4AudioThreshold *= 15;
        // Notes: the size must larger than FIFO_EMPTY_THRESHOLD  (UINT32)(3300)  or underflow may be occur
        if (APE_ATHRESHOLD_MIN > u4AudioThreshold)
        {
            u4AudioThreshold = APE_ATHRESHOLD_MIN;
        }
        else if (APE_ATHRESHOLD_MAX < u4AudioThreshold)
        {
            u4AudioThreshold = APE_ATHRESHOLD_MAX;
        }
    }

    *p4AudioThreshold = u4AudioThreshold;
}

BOOL _SWDMX_ApeFifoIsFull(UINT8 u1SrcId)
{
    BOOL fgFull = FALSE;
    UINT32 u4ThresholdAudio = 0;
    UINT32 u4AvailAudioSize = 0;
    UINT32 u4AudioDataSize = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    if (prSwdmxApe->fgAudio)
    {
        u4AvailAudioSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                                DMX_PID_TYPE_ES_AUDIO,
                                                prSwdmxApe->u1PidxAudio);
    }
    if (u4AvailAudioSize < prSwdmxApe->u4OneTimeSize)
    {
        fgFull = TRUE;
    }
    if ((!fgFull) && (prSwdmxApe->fgAudio))
    {
        UINT32 u4AudioFifoSize = 0;
        UINT32 u4Addr = 0;
        
        _SWDMX_ApeGetAThreshold(u1SrcId,&u4ThresholdAudio);
        
        if (!_SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4Addr, &u4AudioFifoSize))
        {
            if (prSwdmxInst->u4LogFilter & check_fifo_full)
                LOG(0, "line = %d, get audio fifo info fail\n", __LINE__);
            return FALSE;
        }
        u4AudioDataSize = u4AudioFifoSize - u4AvailAudioSize;
        if (prSwdmxApe->fgGotAudioReady)
        {
            if (u4AudioDataSize > u4ThresholdAudio)
            {
                fgFull = TRUE;
            }
        }
    }

    if (fgFull && (prSwdmxInst->u4LogFilter & check_fifo_full))
    {
        LOG(0, "a_size = 0x%X, a_max = 0x%X, onetimesize = 0x%X \n",
            u4AudioDataSize,
            u4ThresholdAudio,
            prSwdmxApe->u4OneTimeSize);
    }

    return fgFull;
}

VOID _SWDMX_ApeSetMoveDataInfo(UINT8 u1SrcId,UINT32 u4SendSize,DMX_MM_DATA_T* prMoveData)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    prMoveData->u1Idx = prSwdmxApe->u1PidxAudio;
    prMoveData->u4BufStart = prSwdmxInst->rFeederInfo.u4StartAddr;
    prMoveData->u4BufEnd = prSwdmxInst->rFeederInfo.u4EndAddr;
    prMoveData->u4StartAddr = prSwdmxApe->u4UsingStartAddr;
    prMoveData->u4FrameSize = u4SendSize;

    if (prSwdmxApe->fgFirst)
    {
        prMoveData->fgFrameHead = TRUE;
        prSwdmxApe->fgFirst = FALSE;
    }
    
    if (_SWDMX_ApeJudgeFileEnd(u1SrcId, u4SendSize))
    {
        prMoveData->fgEOS = TRUE;
    }
}

INT32 _SWDMX_ApeMoveData(UINT8 u1SrcId, DMX_MM_DATA_T *prMoveData)
{
    BOOL fgRet = FALSE;

    if ((prMoveData->u4StartAddr >= prMoveData->u4BufEnd) ||
        (prMoveData->u4StartAddr < prMoveData->u4BufStart) ||
        (prMoveData->u4FrameSize > (prMoveData->u4BufEnd - prMoveData->u4BufStart)))
    {
        LOG(0, "MoveData range [0x%X, 0x%X), but cur at 0x%X, length 0x%X!!!\n", 
            prMoveData->u4BufStart,
            prMoveData->u4BufEnd,
            prMoveData->u4StartAddr,
            prMoveData->u4FrameSize);
        ASSERT(0);
    }
    
    if ((prMoveData->u4StartAddr < prMoveData->u4BufEnd) &&
            ((prMoveData->u4StartAddr + prMoveData->u4FrameSize) >= prMoveData->u4BufEnd))
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prMoveData->u4StartAddr), (prMoveData->u4BufEnd - prMoveData->u4StartAddr));
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prMoveData->u4BufStart), (prMoveData->u4FrameSize - (prMoveData->u4BufEnd - prMoveData->u4StartAddr)));
    }
    else
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prMoveData->u4StartAddr), prMoveData->u4FrameSize);
    }

    fgRet = _SWDMX_MoveData(u1SrcId, prMoveData);
    if (!fgRet)
    {
        return APE_SEND_FAIL;
    }

    return APE_SEND_OK;
}

/****this to update read positon of feeder****/
BOOL _SWDMX_ApeUpdateFeederPosition(UINT8 u1SrcId, DMX_MM_DATA_T * prMoveData)
{
    UINT32 u4FeederPtr = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    if (prSwdmxApe->u4UsingBufSize >= prMoveData->u4FrameSize)
    {
        prSwdmxApe->u8SendedSize += prMoveData->u4FrameSize;
        prSwdmxApe->u4UsingStartAddr += prMoveData->u4FrameSize;
        prSwdmxApe->u4UsingBufSize -= prMoveData->u4FrameSize;
    }
    else
    {
        LOG(0, "%s, u4UsingBufSize = 0x%X, u4FrameSize = 0x%X\n",
                __FUNCTION__, 
                prSwdmxApe->u4UsingBufSize,
                prMoveData->u4FrameSize);
        ASSERT(0);
    }

    while (prSwdmxApe->u4UsingStartAddr >= 
            prSwdmxInst->rFeederInfo.u4EndAddr)
    {
        prSwdmxApe->u4UsingStartAddr -= 
            (prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr);
    }

    u4FeederPtr = prMoveData->u4StartAddr + prMoveData->u4FrameSize;
    while (u4FeederPtr >= prSwdmxInst->rFeederInfo.u4EndAddr)
    {
        u4FeederPtr -= (prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr);
    }
    if (FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, u4FeederPtr) != FEEDER_E_OK)
    {
        if (prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(0, "%s %d FeederUpdateReadPointer fail\n", __FUNCTION__, __LINE__);
        }
        return FALSE;
    }

    return TRUE;
}

VOID _SWDMX_ApeHandleSyncWord(UINT8 u1SrcId, UINT8 u1Remainder)
{
    UINT32 u4FrameLength =0;
    UINT32 u4CurFrame = 0;
    UINT8  u1Loop = 0;
    UINT8  u1ConsumeDataSize = 0;
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
    
    u4CurFrame = prSwdmxApe->u4CurFrame;

    u4FrameLength = _SWDMX_ApeGetCurFramePos(u1SrcId, u4CurFrame + 1) - 
                    _SWDMX_ApeGetCurFramePos(u1SrcId, u4CurFrame);
    
    u4FrameLength = u4FrameLength & (~3); //audio driver requite it.
    
    _SWDMX_ApeGetRingData((UINT8 *)prSwdmxInst->rFeederInfo.u4StartAddr, 
                (UINT8 *)prSwdmxInst->rFeederInfo.u4EndAddr, 
                (UINT8 *)prSwdmxApe->u4UsingStartAddr, 
                prSwdmxApe->pu1SyncWord,
                4);
    for (u1Loop=0; u1Loop<4; u1Loop++)
    {
        prSwdmxApe->pu1SyncWord[u1Loop+4] = prSwdmxApe->pu1SyncWord[u1Loop];
        prSwdmxApe->pu1SyncWord[u1Loop+8] = prSwdmxApe->pu1SyncWord[u1Loop];
    }
    
    switch (u1Remainder)
    {
        case 0:
        {
            prSwdmxApe->pu1SyncWord[0] = 'D';
            prSwdmxApe->pu1SyncWord[1] = 'E';
            prSwdmxApe->pu1SyncWord[2] = 'P';
            prSwdmxApe->pu1SyncWord[3] = 'A';
            *(UINT32*)(prSwdmxApe->pu1SyncWord+4) = u4FrameLength;
            break;
        }
        case 1:
        {
            prSwdmxApe->pu1SyncWord[2] = 'A';
            prSwdmxApe->pu1SyncWord[1] = 'P';
            prSwdmxApe->pu1SyncWord[0] = 'E';
            prSwdmxApe->pu1SyncWord[7] = 'D';
            prSwdmxApe->pu1SyncWord[6] = (u4FrameLength >> 24)&0xFF;
            prSwdmxApe->pu1SyncWord[5] = (u4FrameLength >> 16)&0xFF;
            prSwdmxApe->pu1SyncWord[4] = (u4FrameLength >> 8)&0xFF;
            prSwdmxApe->pu1SyncWord[11] = (u4FrameLength)&0xFF;
            break;
        }
        case 2:
        {
            prSwdmxApe->pu1SyncWord[1] = 'A';
            prSwdmxApe->pu1SyncWord[0] = 'P';
            prSwdmxApe->pu1SyncWord[7] = 'E';
            prSwdmxApe->pu1SyncWord[6] = 'D';
            prSwdmxApe->pu1SyncWord[5] = (u4FrameLength >> 24)&0xFF;
            prSwdmxApe->pu1SyncWord[4] = (u4FrameLength >> 16)&0xFF;
            prSwdmxApe->pu1SyncWord[11] = (u4FrameLength >> 8)&0xFF;
            prSwdmxApe->pu1SyncWord[10] = (u4FrameLength)&0xFF;
            break;
        }
        case 3:
        {
            prSwdmxApe->pu1SyncWord[0] = 'A';
            prSwdmxApe->pu1SyncWord[7] = 'P';
            prSwdmxApe->pu1SyncWord[6] = 'E';
            prSwdmxApe->pu1SyncWord[5] = 'D';
            prSwdmxApe->pu1SyncWord[4] = (u4FrameLength >> 24)&0xFF;
            prSwdmxApe->pu1SyncWord[11] = (u4FrameLength >> 16)&0xFF;
            prSwdmxApe->pu1SyncWord[10] = (u4FrameLength >> 8)&0xFF;
            prSwdmxApe->pu1SyncWord[9] = (u4FrameLength)&0xFF;
            break;
        }
        default:
            ASSERT(0);
            break;
    }

    u1ConsumeDataSize = (u1Remainder & 3) ? 4 : 0;

    rDmxMMData.u1Idx = prSwdmxApe->u1PidxAudio;
    rDmxMMData.u4BufStart = (UINT32)prSwdmxApe->pu1SyncWord;
    rDmxMMData.u4BufEnd = (UINT32)prSwdmxApe->pu1SyncWord + APE_SYNC_WORD_SIZE;
    rDmxMMData.u4StartAddr = (UINT32)prSwdmxApe->pu1SyncWord;
    rDmxMMData.u4FrameSize = 8 + u1ConsumeDataSize;
    if (prSwdmxInst->u4LogFilter & check_move_data)
    {
        LOG(0, "sync word(rem %d):\n", u1Remainder);
        _SWDMX_ApePrintHex(prSwdmxApe->pu1SyncWord, 8 + u1ConsumeDataSize);
    }

    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
    {
        LOG(0, "Demuxer fails to move data.\n");
        return;
    }

    if (u1ConsumeDataSize)
    {
        rDmxMMData.u4StartAddr = prSwdmxApe->u4UsingStartAddr;
        rDmxMMData.u4FrameSize = 4;
        _SWDMX_ApeUpdateFeederPosition(u1SrcId, &rDmxMMData);
    }
    
    return;

}

INT32 _SWDMX_ApeHandleMovedata(UINT8 u1SrcId,UINT32 u4SendSize)
{
    INT32 i4Ret = APE_SEND_FAIL;
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    
    if (u4SendSize == 0)
    {
        return APE_SEND_OK;
    }

    x_memset(&rDmxMMData, 0, sizeof(rDmxMMData));
    
    _SWDMX_ApeSetMoveDataInfo(u1SrcId,u4SendSize,&rDmxMMData);

    i4Ret = _SWDMX_ApeMoveData(u1SrcId, &rDmxMMData);
    if (i4Ret != APE_SEND_OK)
    {
        LOG(0, "move data fail! ret %d\n",i4Ret);
        return i4Ret;
    }
    
    _SWDMX_ApeUpdateFeederPosition(u1SrcId, &rDmxMMData);

    if (prSwdmxInst->u4LogFilter & check_move_data)
    {
        UINT8 au1Data[16], u1Size;
        
        LOG(0, "StartAddr=0x%x, Size=0x%x\n", rDmxMMData.u4StartAddr, u4SendSize);
        u1Size = (u4SendSize > 16) ? 16 : u4SendSize;
        _SWDMX_ApeGetRingData((UINT8 *)rDmxMMData.u4BufStart,
                                (UINT8 *)rDmxMMData.u4BufEnd,
                                (UINT8 *)rDmxMMData.u4StartAddr,
                                au1Data,
                                u1Size);
        _SWDMX_ApePrintHex(au1Data, u1Size);
    }

    return APE_SEND_OK;
}

INT32 _SWDMX_ApeDemuxer(UINT8 u1SrcId)
{
    INT32 i4Ret = APE_SEND_FAIL;
    UINT32 u4MoveSize = 0, u4SendSize = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    u4SendSize = prSwdmxApe->u4OneTimeSize;
    if (prSwdmxApe->u8SendedSize + u4SendSize > prSwdmxInst->u8FileSize)
    {
        u4SendSize = prSwdmxInst->u8FileSize - prSwdmxApe->u8SendedSize;
    }
    if (u4SendSize > prSwdmxApe->u4UsingBufSize)
    {
        u4SendSize = prSwdmxApe->u4UsingBufSize;
    }
    
    while (u4SendSize)
    {
        if ((u4SendSize >= 4) &&
            (prSwdmxApe->u8SendedSize == APE_CUT4(_SWDMX_ApeGetCurFramePos(u1SrcId, prSwdmxApe->u4CurFrame))))
        {
            _SWDMX_ApeHandleSyncWord(u1SrcId, APE_MOD4(_SWDMX_ApeGetCurFramePos(u1SrcId, prSwdmxApe->u4CurFrame)));
            if (APE_MOD4(_SWDMX_ApeGetCurFramePos(u1SrcId, prSwdmxApe->u4CurFrame)))
            {
                u4SendSize -= 4;
            }
        }
        
        if (u4SendSize < 4)
        {
            u4MoveSize = u4SendSize; // last frame
        }
        else if ((prSwdmxApe->u8SendedSize + u4SendSize) <
            APE_CUT4(_SWDMX_ApeGetCurFramePos(u1SrcId, prSwdmxApe->u4CurFrame + 1)))
        {
            u4MoveSize = u4SendSize;
        }
        else
        {
            u4MoveSize = APE_CUT4(_SWDMX_ApeGetCurFramePos(u1SrcId, prSwdmxApe->u4CurFrame + 1)) -
                         prSwdmxApe->u8SendedSize;
            prSwdmxApe->u4CurFrame++;
        }
        
        i4Ret = _SWDMX_ApeHandleMovedata(u1SrcId, u4MoveSize);
        if (i4Ret != APE_SEND_OK)
        {
            break;
        }
        u4SendSize -= u4MoveSize;
    }

    return i4Ret;
    
}


VOID _SWDMX_ApeFirstDataHeader(UINT8 u1SrcId)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    /****check the feeder state****/
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &(prSwdmxInst->rFeederInfo)) != FEEDER_E_OK)
    {
        LOG(0, "FeederGetBufferInfo Fail \n");
        return ;
    }

    LOG(1, "feeder buf [0x%x] -> [0x%x]\n",
        prSwdmxInst->rFeederInfo.u4StartAddr, prSwdmxInst->rFeederInfo.u4EndAddr);

    if (prSwdmxApe->fgSeek)
    {
        prSwdmxApe->fgSeek = FALSE;
    }

    AUD_DspAPEMuteBankNum(prSwdmxInst->u1AdecId, 0);
}

VOID _SWDMX_ApeSetFrmIdx(UINT8 u1SrcId, UINT32 u4CurFrmIdx)
{
    UINT64 u8FilePos = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    _SWDMX_ApeResetData(u1SrcId);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
    AUD_DSPCmdStop(AUD_DSP0, prSwdmxInst->u1AdecId);
    AUD_DSPCmdPlay(AUD_DSP0, prSwdmxInst->u1AdecId);
#else    
    AUD_DSPCmdStop(prSwdmxInst->u1AdecId);
    AUD_DSPCmdPlay(prSwdmxInst->u1AdecId);
#endif
    u8FilePos = _SWDMX_ApeGetCurFramePos(u1SrcId, u4CurFrmIdx);
    if (APE_MOD4(u8FilePos))
    {
        u8FilePos = APE_CUT4(u8FilePos);
        ASSERT(u4CurFrmIdx);
        if (u4CurFrmIdx)
        {
            u4CurFrmIdx--;
        }
    }
    prSwdmxApe->u4CurFrame = u4CurFrmIdx;
    prSwdmxApe->u8PlayPos = u8FilePos;
    prSwdmxApe->u8ReadedPos = u8FilePos;
    prSwdmxApe->u8SendedSize = u8FilePos;
    prSwdmxApe->u8PreviousUIPos = u8FilePos;

    LOG(0, "%s u4CurFrame = %d\n", __FUNCTION__, u4CurFrmIdx);
}

VOID _SWDMX_ApeHandleTriggerSeek(UINT8 u1SrcId, INT32 i4PlaySpeed, UINT64 u8SeekPos)
{
    UINT32 u4SeekFrmIdx;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    LOG(5, "_SWDMX_ApeHandleTriggerSeek change pos: 0x%llx -> 0x%llx ================\n",
        prSwdmxApe->u8SendedSize, u8SeekPos);
        
    u4SeekFrmIdx = _SWDMX_ApeGetFrameIdx(u1SrcId, u8SeekPos);
    
    _SWDMX_ApeSetFrmIdx(u1SrcId, u4SeekFrmIdx);
    
    prSwdmxApe->u8PreviousUIPos = u8SeekPos;
    prSwdmxApe->fgSeek = TRUE;
    
    STC_StopStc(prSwdmxInst->u1StcId);
}

VOID _SWDMX_ApeHandleTriggerTrick(UINT8 u1SrcId,INT32 i4PlaySpeed)
{
    UINT32 u4SkipFrame;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    LOG(5, "_SWDMX_ApeHandleTrick audio change speed: %d \n", i4PlaySpeed);

    if ((prSwdmxApe->fgAudio) && (prSwdmxInst->u4TotalTime > 0) && (i4PlaySpeed < 0))
    {
        LOG(5, "_SWDMX_ApeHandleChange audio change speed: %d ====================\n", i4PlaySpeed);

        prSwdmxApe->fgJumpping = TRUE;
        
        i4PlaySpeed /= -SWDMX_PLAY_SPEED_1X;
        u4SkipFrame = (UINT32)_SWDMX_Div6464((UINT64)i4PlaySpeed*90000,
                                             (UINT64)prSwdmxApe->u4TimePerFrame,
                                             NULL);
        if (u4SkipFrame == 0)
        {
            u4SkipFrame = 1;
        }
        prSwdmxApe->u4SkipFrame = u4SkipFrame;
        prSwdmxApe->u4JumpStartFrame = prSwdmxApe->u4CurFrame;
        if (prSwdmxApe->u4JumpStartFrame < prSwdmxApe->u4SkipFrame)
        {
            prSwdmxApe->fgSendEOF = TRUE;
            prSwdmxApe->u4JumpStartFrame = 0;
        }
        else
        {
            prSwdmxApe->u4JumpStartFrame -= prSwdmxApe->u4SkipFrame;
        }
        _SWDMX_ApeSetFrmIdx(u1SrcId, prSwdmxApe->u4JumpStartFrame);
        HAL_GetTime(&prSwdmxApe->rJumpStartTime);
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
        {
            LOG(1, "u4SkipFrame = %u\n", prSwdmxApe->u4SkipFrame);
        }
    }
    else if ((prSwdmxApe->fgAudio) && (i4PlaySpeed >= 0) && (prSwdmxApe->i4CurSpeed < 0))
    {
        prSwdmxApe->fgJumpping = FALSE;
        _SWDMX_ApeSetFrmIdx(u1SrcId, prSwdmxApe->u4JumpStartFrame);
    }
}

VOID _SWDMX_ApeHandleNextTrick(UINT8 u1SrcId)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    if (prSwdmxApe->u4JumpStartFrame < prSwdmxApe->u4SkipFrame)
    {
        prSwdmxApe->fgSendEOF = TRUE;
        prSwdmxApe->u4JumpStartFrame = 0;
    }
    else
    {
        prSwdmxApe->u4JumpStartFrame -= prSwdmxApe->u4SkipFrame;
    }
    _SWDMX_ApeSetFrmIdx(u1SrcId, prSwdmxApe->u4JumpStartFrame);
    HAL_GetTime(&prSwdmxApe->rJumpStartTime);

    if(prSwdmxInst->u4LogFilter & check_skip_mode)
    {
        LOG(1, "u4JumpStartFrame = %u\n", prSwdmxApe->u4JumpStartFrame);
    }
    
    return;
}

VOID _SWDMX_ApeWaitNextTrick(UINT8 u1SrcId)
{
    HAL_TIME_T rJumpEndTime, rDelta;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    HAL_GetTime(&rJumpEndTime);
    HAL_GetDeltaTime(&rDelta, &prSwdmxApe->rJumpStartTime, &rJumpEndTime);

    if (rDelta.u4Seconds < 1)
    {
        return;
    }
    
    _SWDMX_ApeHandleNextTrick(u1SrcId);
}

VOID _SWDMX_ApeHandleChange(UINT8 u1SrcId, SWDMX_CUR_CMD_T* prCmd)
{
    INT32 i4PlaySpeed;
    UINT64 u8SeekPos;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    if (!prCmd)
    {
        LOG(1, "_SWDMX_MpgHandleChange prCmd null\n");
        return;
    }
    i4PlaySpeed = prCmd->i4PlaySpeed;
    u8SeekPos = prCmd->u8SeekPos;
    /* handle seek */
    if (((UINT32)(u8SeekPos>>32) != (UINT32)0xFFFFFFFF)
            || ((UINT32)(u8SeekPos) != (UINT32)0xFFFFFFFF))
    {
        _SWDMX_ApeHandleTriggerSeek(u1SrcId,i4PlaySpeed,u8SeekPos);
    }
    else
    {
        /* handle trick */
        _SWDMX_ApeHandleTriggerTrick(u1SrcId,i4PlaySpeed);
    }
    prSwdmxApe->i4CurSpeed = i4PlaySpeed;
}

INT32 _SWDMX_ApeSendData(UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList,
                         SWDMX_CUR_CMD_T* prCmd)
{
    INT32 i4Ret;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    _SWDMX_ApeDetectStrmChgInfo((u1SrcId));

    if ((prSwdmxApe->fgAudio) && (!prSwdmxApe->fgEnADmx))
    {
        if (!_SWDMX_ApeInitDmx(u1SrcId,eSWDMX_STRM_TYPE_AUD))
        {
            LOG(0,"Mpg init audio pid index fail\n");
            return APE_SEND_FAIL;
        }
        prSwdmxApe->fgEnADmx = TRUE;
    }

    /****handle change for seek & speed change****/
    if (prCmd->fgChange)
    {
        _SWDMX_ApeHandleChange(u1SrcId, prCmd);
    }

    if (prSwdmxApe->fgJumpping)
    {
        _SWDMX_ApeWaitNextTrick(u1SrcId); /* waiting consume done and caculate next time */
    }

    /****send eos ****/
    if (_SWDMX_ApeJudgeFileEnd(u1SrcId, 0))
    {
        _SWDMX_ApeHandleEos(u1SrcId);
        return APE_SEND_EOS;
    }

    /****request data****/
    i4Ret = _SWDMX_ApeHandleFeeder(u1SrcId);
    if (i4Ret == APE_NO_DATA)
    {
        return APE_SEND_FAIL;
    }

    /****judge fifo is full or not****/
    if (_SWDMX_ApeFifoIsFull(u1SrcId))
    {
        prSwdmxInst->fgFifoFull = TRUE;
        return APE_SEND_FULL;
    }
    if (prSwdmxApe->fgFirst)
    {
        _SWDMX_ApeFirstDataHeader(u1SrcId);
    }

    /****demuxer****/
    _SWDMX_ApeDemuxer(u1SrcId);

    prSwdmxApe->u8PreviousSendedSize = prSwdmxApe->u8SendedSize;

    return APE_SEND_OK;
}

/************************************************************************/
BOOL _SWDMX_ApeFlush(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    LOG(5, "_SWDMX_ApeFlush(%d)\n", u1SrcId);
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    _SWDMX_ApeResetData(u1SrcId);
    return TRUE;
}

BOOL _SWDMX_ApeSetStrmID(UINT8 u1SrcId, UINT8 u1HdrType,
                         UINT32 u4StrmID, UINT32 u4StrmSubID)
{
    return TRUE;
}

BOOL _SWDMX_ApeEnableStrm(UINT8 u1SrcId, UINT8 u1StrmType, VOID* pfnCbFunc)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    LOG(5, "_SWDMX_ApeEnableStrm(%d), u1StrmType =%d\n", u1SrcId, u1StrmType);

    switch (u1StrmType)
    {
    case eSWDMX_STRM_TYPE_AUD:
        prSwdmxApe->rRecvStreamChgInfo.fgEnableAudio=TRUE;
        break;
    default:
        break;
    }

    LOG(3, "_SWDMX_ApeEnableStrm(%d)\n", u1SrcId);
    UNUSED(pfnCbFunc);
    return TRUE;
}

BOOL _SWDMX_ApeDisableStrm(UINT8 u1SrcId, UINT8 u1StrmType)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    LOG(5, "_SWDMX_ApeDisableStrm(%d) u1StrmType = %d\n", u1SrcId, u1StrmType);

    switch (u1StrmType)
    {
    case eSWDMX_STRM_TYPE_AUD:
        prSwdmxApe->rRecvStreamChgInfo.fgDisableAudio=TRUE;
        break;
    default:
        break;
    }

    LOG(3, "_SWDMX_ApeDisableStrm(%d) %d \n", u1SrcId);
    return TRUE;
}

BOOL _SWDMX_ApeGetStreamInfo(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType,
                             UINT32 u4InfoMask,
                             UINT64 u8FileSize,
                             SWDMX_PROGRAM_INFO_T* prPInfo)
{
    return TRUE;
}

BOOL _SWDMX_ApeInit(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    LOG(5, "_SWDMX_ApeInit(%d)\n", u1SrcId);

    if (!prSwdmxApe->pu1SyncWord)
    {
        prSwdmxApe->pu1SyncWord = (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(APE_SYNC_WORD_SIZE,16));
        x_memset(prSwdmxApe->pu1SyncWord, 0x0, APE_SYNC_WORD_SIZE);
    }

    prSwdmxApe->u1PidxAudio = 0xFF;
    prSwdmxApe->fgFirst = TRUE;

    prSwdmxApe->i4CurSpeed = SWDMX_PLAY_SPEED_1X;
    prSwdmxApe->u4FeederReqID = 1;
    prSwdmxApe->u4FeederBufNum = APE_BUF_NUM;
    prSwdmxApe->u4FeederReqNum = APE_REQ_NUM;

    prSwdmxApe->fgSeek = FALSE;
    prSwdmxApe->u4LastAudioWptr = 0;

    DMX_SetToDecoder(TRUE);

    return TRUE;
}

VOID _SWDMX_ApeSetRange(UINT8 u1SrcId, SWDMX_RANGE_LIST_T *prRangeList, UINT32 u4VidStrmID,
                        UINT32 u4AudStrmID, UINT32 u4SubTitleID)
{
    INT32 i = 0;
    MM_RANGE_INFO_T* prRangeInfo = 0;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    LOG(5, "_SWDMX_ApeSetRange(%d)\n", u1SrcId);

    if (prRangeList == NULL)
    {
        LOG(0, "%s prRangeList is NULL!\n", __FUNCTION__);
        return;
    }

    prRangeInfo = prRangeList->prDmxRangeInfo;
    prSwdmxApe->prCurRangeReq = prRangeList;

    if (prRangeInfo == NULL)
    {
        LOG(0, "%s prRangeInfo is NULL!\n", __FUNCTION__);
        return;
    }

    if ((prRangeInfo->pt_idxtbl_lst == NULL) || (prRangeInfo->pt_idxtbl_lst->u.pv_idx_tbl_entry == NULL))
    {
        LOG(0, "Index table is NULL.\n");
        ASSERT(0);
        return;
    }
    
    _SWDMX_ApeResetData(u1SrcId);
    
    prSwdmxApe->u4BlocksPerFrame = 
        prRangeInfo->pt_idxtbl_lst->t_aud_spec_info.u.t_ape_info.ui4_block_per_frame;
    prSwdmxApe->u4FinalFrameBlocks = 
        prRangeInfo->pt_idxtbl_lst->t_aud_spec_info.u.t_ape_info.ui4_final_frame_block;
    prSwdmxApe->u4TotalFrame = 
        prRangeInfo->pt_idxtbl_lst->t_aud_spec_info.u.t_ape_info.ui2_total_frame_num_high;
    prSwdmxApe->u4SampleRate = 
        prRangeInfo->pt_idxtbl_lst->t_aud_spec_info.u.t_ape_info.ui4_input_sampling_rate;
    prSwdmxApe->u4CurFrame = 0;

    prSwdmxApe->u4TimePerFrame = (UINT32)_SWDMX_Div6464((UINT64)prSwdmxApe->u4BlocksPerFrame * 90000, 
                                                        (UINT64)prSwdmxApe->u4SampleRate,
                                                        NULL);

    if (prRangeInfo->pt_idxtbl_lst->ui4_number_of_entry != prSwdmxApe->u4TotalFrame)
    {
        LOG(0, "u4TotalFrame in APE header is %u, but seek tbl size is %u.\n",
               prSwdmxApe->u4TotalFrame,
               prRangeInfo->pt_idxtbl_lst->ui4_number_of_entry);
        /* seektable index may be no match with totalframe in special file,but index overflow totalframe is 0 usually*/
    }
    
    prSwdmxApe->pu4SeekTable = prRangeInfo->pt_idxtbl_lst->u.pv_idx_tbl_entry;
    prSwdmxInst->u8FileSize = (prRangeInfo->z_range_sz + prRangeInfo->ui8_fileoffset);
    prSwdmxInst->u8FileOffset = prSwdmxApe->pu4SeekTable[0];
    prSwdmxInst->u4TotalTime = (UINT32)prRangeInfo->ui8_pb_duration;

    if (prSwdmxInst->u8FileOffset >= prSwdmxInst->u8FileSize)
    {
        LOG(3, "u8StartFileOffset >= u8FileSize\n");
        if (prSwdmxInst->u8FileSize > 0)
        {
            prSwdmxInst->u8FileOffset = (prSwdmxInst->u8FileSize - 1);
        }
        else
        {
            prSwdmxInst->u8FileOffset = 0;
        }
    }
    prSwdmxApe->u8SendedSize = prSwdmxInst->u8FileOffset;   /*the first Frame pos */
    prSwdmxApe->u1AlignBase = prSwdmxApe->pu4SeekTable[0] % 4;

    LOG(1, "u8FileSize 0x%llx, u8SendedSize 0x%llx, u4TotalTime %u\n",
           prSwdmxInst->u8FileSize,
           prSwdmxApe->u8SendedSize,
           prSwdmxInst->u4TotalTime);
    LOG(1, "u4SampleRate %u, u4TimePerFrame(90K) %u, u4TotalFrame %u\n",
           prSwdmxApe->u4SampleRate,
           prSwdmxApe->u4TimePerFrame,
           prSwdmxApe->u4TotalFrame);
    LOG(1, "u4BlocksPerFrame %u, u4FinalFrameBlocks %u, u1AlignBase %u\n",
           prSwdmxApe->u4BlocksPerFrame,
           prSwdmxApe->u4FinalFrameBlocks,
           prSwdmxApe->u1AlignBase);

    if (prSwdmxInst->u4LogFilter & check_sended_size)
    { 
        for (i=0; i<prSwdmxApe->u4TotalFrame; i++)
        {
            LOG(1, "INDEX\t%d\t\t0x%X\n", i, _SWDMX_ApeGetCurFramePos(u1SrcId, i));
        }
    }

    _SWDMX_ApeSetSendedPos(u1SrcId, prSwdmxApe->u8SendedSize);
    _SWDMX_ApeSetReqSize(u1SrcId);
    
    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    prSwdmxApe->i4CurSpeed = SWDMX_PLAY_SPEED_1X;

    UNUSED(u4VidStrmID);
    UNUSED(u4AudStrmID);
    UNUSED(u4SubTitleID);
}

BOOL _SWDMX_ApePlay(UINT8 u1SrcId)
{
    return TRUE;
}

BOOL _SWDMX_ApePause(UINT8 u1SrcId)
{
    return TRUE;
}

BOOL _SWDMX_ApeAbort(UINT8 u1SrcId)
{
    return TRUE;
}

BOOL _SWDMX_ApeStop(UINT8 u1SrcId)
{
    BOOL fgRet = FALSE;
    DMX_MM_T rDmxMMInfo;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    LOG(5, "%s(%d)\n", __FUNCTION__, u1SrcId);

    _SWDMX_ApeResetData(u1SrcId);
    
    if (prSwdmxApe->pu1SyncWord)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prSwdmxApe->pu1SyncWord);
    }
    
    if (prSwdmxApe->fgEnADmx)
    {
        UINT8 u1PidxAudio = prSwdmxApe->u1PidxAudio;
        if (u1PidxAudio == 0xFF)
        {
            LOG(3, " swdmx ape disable audio es u1PidxAudio == 0xFF\n");
            return FALSE;
        }
        x_memset(&rDmxMMInfo, 0, sizeof(rDmxMMInfo));
        rDmxMMInfo.fgEnable = FALSE;
        fgRet = DMX_MM_Set(u1PidxAudio, DMX_PID_FLAG_VALID, &rDmxMMInfo);
        if (!fgRet)
        {
            LOG(3, "swdmx ape disable DMX_MM_Set Fail\n");
            return FALSE;
        }
        fgRet = DMX_MM_Free(u1PidxAudio);
        if (!fgRet)
        {
            LOG(3, "_swdmx ape disable DMX_MM_Free Fail\n");
            return FALSE;
        }
        fgRet = DMX_MUL_FreePidx(u1PidxAudio);
        if (!fgRet)
        {
            LOG(3, "DMX_MUL_FreePidx Fail\n");
            fgRet = FALSE;
        }
        prSwdmxApe->fgEnADmx = FALSE;
    }
    
    prSwdmxApe->i4CurSpeed = SWDMX_PLAY_SPEED_1X;

    return TRUE;
}

BOOL _SWDMX_ApeSetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    LOG(5, "%s(%d) u1Type = %d, u4Para1 = %d\n", __FUNCTION__, u1SrcId, u1Type, u4Para1);

    switch (u1Type)
    {
        case eSWDMX_PRINT_INTERNAL_STATUS:
        {
            LOG(0, "Cur: u8SendedSize(0x%llX) u8ReadedPos(0x%llX)\n",
                prSwdmxApe->u8SendedSize,
                prSwdmxApe->u8ReadedPos);
            LOG(0, "rBuf: u4BufTotalSize(%d) u4ReqTotalSize(%d)\n",
                prSwdmxApe->u4BufTotalSize,
                prSwdmxApe->u4ReqTotalSize);
            LOG(0, "AvailableAFifo(%d)\n",
                DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                    DMX_PID_TYPE_ES_AUDIO,
                    prSwdmxApe->u1PidxAudio));
            LOG(0, "Speed: %d\n", prSwdmxApe->i4CurSpeed);
            return TRUE;
        }
        default:
            break;
    }

    UNUSED(u4Para2);
    UNUSED(u4Para3);
    return FALSE;
}

BOOL _SWDMX_ApeGetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32* pu4Para1,UINT32* pu4Para2,UINT32* pu4Para3)
{
    UINT64 *pu8Tmp;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;
    
    LOG(6, "%s u1Type = %d\n", __FUNCTION__, u1Type);
    if (pu4Para1 == NULL)
    {
        LOG(1, "_SWDMX_MpgGetInfo pu4Para1 = NULL\n");
        return FALSE;
    }

    switch (u1Type)
    {
        case eSWDMX_GET_CURRENT_POSITION:
        {
            pu8Tmp = (UINT64*)(VOID*)pu4Para1;

            *pu8Tmp = prSwdmxApe->u8PreviousUIPos;
            if (prSwdmxApe->fgAudio)
            {
                if ((prSwdmxApe->i4CurSpeed < 0))
                {
                    *pu8Tmp = prSwdmxApe->u8PreviousUIPos;
                    if (prSwdmxInst->u4LogFilter & check_getinfo)
                    {
                        LOG(1,"---GetInfo *pu8Tmp %d\n", (UINT32)*pu8Tmp);
                    }
                }
                else
                {
                    DMX_MM_T rDMXInfo;
                    UINT32 u4DataSize = 0;
                    x_memset(&rDMXInfo, 0, sizeof(DMX_MM_T));
                    if (0xFF != prSwdmxApe->u1PidxAudio)
                    {
                        if (DMX_MM_Get(prSwdmxApe->u1PidxAudio, DMX_MM_FLAG_BUF, &rDMXInfo))
                        {
                            u4DataSize = (rDMXInfo.u4Wp < rDMXInfo.u4Rp) ?
                                         (rDMXInfo.u4BufSize + rDMXInfo.u4Wp - rDMXInfo.u4Rp) : (rDMXInfo.u4Wp - rDMXInfo.u4Rp);
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }

                    if (prSwdmxApe->u8SendedSize > u4DataSize)
                    {
                        *pu8Tmp = prSwdmxApe->u8SendedSize - u4DataSize;
                    }
                    else
                    {
                        *pu8Tmp = 0;
                    }
                    if (*pu8Tmp < prSwdmxApe->u8PreviousUIPos)
                    {
                        *pu8Tmp = prSwdmxApe->u8PreviousUIPos;
                    }
                    prSwdmxApe->u8PreviousUIPos = *pu8Tmp;
                    if ((UINT32)*pu8Tmp == (UINT32)prSwdmxApe->u8SendedSize)
                    {
                        /* debug to reserve it. */
                        LOG(1,"GetInfo pu8Tmp=u8SendedSize 0x%llx\n",(UINT32)prSwdmxApe->u8SendedSize);
                    }
                    
                    if (prSwdmxInst->u4LogFilter & check_getinfo)
                    {
                        LOG(1, "---GetInfo %d %d %d \n",
                            (UINT32)*pu8Tmp, (UINT32)prSwdmxApe->u8SendedSize, u4DataSize);
                    }
                }
            }

            return TRUE;
        }
        case eSWDMX_GET_IS_TRICK:
        {
            *pu4Para1 = TRUE;
            return TRUE;
        }
        default:
        {
            break;
        }
    }

    return FALSE;
}

BOOL _SWDMX_ApeAudioCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    if (eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        LOG(1, "audio is ready===========================\n");
        prSwdmxApe->fgGotAudioReady = TRUE;
        AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
    }
    return TRUE;
}

BOOL _SWDMX_ApeAudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet;
    DMX_AUDIO_PES_T rPes;
    SWDMX_INFO_T *prSwdmxInst = NULL;
    SWDMX_APE_INFO_T *prSwdmxApe = NULL;
    
    if (prPes == NULL)
    {
        LOG(1, "_SWDMX_MpgAudioCallback prPes NULL\n");
        return FALSE;
    }

    prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxApe = &prSwdmxInst->rFmtInfo.rSwdmxApeInfo;

    if (prSwdmxInst->u4LogFilter & check_original_audio_pts)
    {
        LOG(1, "AO PTS=0x%X RP=0x%X WP=0x%X Pos=0x%llX\n",
            prPes->u4Pts, prPes->u4PesRp, prPes->u4Wp,
            prSwdmxApe->u8PreviousSendedSize);
    }

    x_memset(&rPes, 0, sizeof(rPes));
    x_memcpy(&rPes, prPes, sizeof(DMX_AUDIO_PES_T));

    if (prSwdmxApe->u4LastAudioWptr)
    {
        rPes.u4Wp = prSwdmxApe->u4LastAudioWptr;
    }
    prSwdmxApe->u4LastAudioWptr = prPes->u4Wp;

    rPes.fgForward = (prSwdmxApe->i4CurSpeed > 0) ? TRUE : FALSE;

    if (prPes->fgEOS)
    {
        DMX_AUDIO_PES_T rPes1Last;
        x_memset(&rPes1Last, 0, sizeof(rPes1Last));
        rPes1Last.u1DeviceId = prSwdmxInst->u1AdecId;
        rPes1Last.u4Wp = rPes.u4Wp;
        rPes1Last.fgForward = rPes.fgForward;
        
        if (rPes.u4Wp != 0)
        {
            UNUSED(AUD_SendAudioPes(&rPes1Last));
        }
        else
        {
            LOG(5,"_SWDMX_ApeAudioCallback rPes.u4Wp %lu EOS %u \n" ,
                rPes.u4Wp, prPes->fgEOS);

            return TRUE;
        }

        if (prSwdmxInst->u4LogFilter & check_audio_pts)
        {
            LOG(1, "AC PTS=0x%X RP=0x%X WP=0x%X Pos=0x%llX (Last)\n",
                rPes1Last.u4Pts, rPes1Last.u4PesRp, rPes1Last.u4Wp,
                prSwdmxApe->u8PreviousSendedSize);
        }

        LOG(3, "_SWDMX_ApeAudioCallback send EOS\n");
        rPes.u4Wp = prPes->u4Wp;
        prSwdmxApe->fgSendAudioEos = TRUE;
    }

#ifdef SWDMX_DUMP_DATA
    _SWDMX_DumpAFifo(prSwdmxInst, rPes.u4Wp);
#endif
    
    fgRet = AUD_SendAudioPes(&rPes);

    if (prSwdmxInst->u4LogFilter & check_audio_pts)
    {
        LOG(1, "AC PTS=0x%X RP=0x%X WP=0x%X Pos=0x%llX\n",
            rPes.u4Pts, rPes.u4PesRp, rPes.u4Wp,
            prSwdmxApe->u8PreviousSendedSize);
    }

    if (prPes->fgEOS)
    {
        AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
    }
    return fgRet;
}

BOOL _SWDMX_ApeGetPosByTime(UINT8 u1SrcId,UINT32 u4Time,UINT64 *pu8FilePos)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_APE_VERIFY_NULL_ERROR(prSwdmxInst);
    
    if (u4Time > prSwdmxInst->u4TotalTime)
    {
        u4Time = prSwdmxInst->u4TotalTime;
    }

    *pu8FilePos = _SWDMX_Div6464((prSwdmxInst->u8FileSize - prSwdmxInst->u8FileOffset)*(u4Time),
                            (UINT64)(prSwdmxInst->u4TotalTime), NULL);
    *pu8FilePos += prSwdmxInst->u8FileOffset;

    return TRUE;
}

SWDMX_HDR_PRS_STATE_T _SWDMX_ApeHeaderParse(UINT8 u1SrcId, 
                                            SWDMX_RANGE_LIST_T* prRangeList,
                                            SWDMX_CUR_CMD_T* prCmd)
{
    INT32 i4Ret;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    if (prSwdmxInst == NULL)
    {
        return eSWDMX_HDR_PRS_FAIL;
    }

    i4Ret = _SWDMX_ApeSendData(u1SrcId, prRangeList, prCmd);
    if (i4Ret == APE_SEND_OK)
    {
        return eSWDMX_HDR_PRS_SUCCEED;
    }
    else if (i4Ret == APE_SEND_FULL)
    {
        x_thread_delay(10);
        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }
    else if (i4Ret == APE_SEND_EOS)
    {
        x_thread_delay(1);
        LOG(7, "_SWDMX_ApeHeaderParse send data Finish.\n");
        return eSWDMX_HDR_PRS_SUCCEED;
    }
    else if (i4Ret == APE_SEND_FAIL)
    {
        x_thread_delay(1);
        LOG(7, "_SWDMX_ApeHeaderParse  send data Fail.\n");
        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }
    else if (i4Ret == MPG_SEND_SKIP)
    {
        x_thread_delay(1);
        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }
    else if (i4Ret == MPG_SEND_WAIT)
    {
        x_thread_delay(1);
        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }
    else
    {
        x_thread_delay(1);
        LOG(3, "_SWDMX_MpgHeaderParse i4Ret %d unknow.\n", i4Ret);
    }

    LOG(5, "_SWDMX_MpgHeaderParse _SWDMX_MpgSendData Unknow return.\n");
    return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
}

