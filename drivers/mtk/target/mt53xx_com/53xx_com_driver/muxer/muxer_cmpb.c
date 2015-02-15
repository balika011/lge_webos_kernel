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
 * $RCSfile: muxer_cmpb.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_cmpb.c
 *  Private format multiplexer driver - implementation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "muxer_util.h"
#include "muxer_cmpb.h"
#include "muxer_debug.h"
#ifdef CC_SUPPORT_VENC
#include "venc_if.h"
#endif


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


static VOID _MUXER_CmpbMuxH264Es(MUXER_INFO_T *prMuxInfo, UINT32 u4StartAddr, UINT32 u4FrameSize, UINT32 u4Pts)
{
    //UINT32 u4MxBufWpBak = prMuxInfo->u4MxBufWp;

    if (!MxWaitBufferAvail(prMuxInfo, u4FrameSize + 14))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return;
    }

#if 1  // ToDo: consider endian
    u4Pts = 0;
    vMxPutBits(prMuxInfo, 0, 32);            // PTS upper part
    vMxPutBits(prMuxInfo, u4Pts, 32);        // PTS lower part
#else
    vMxPutBits(prMuxInfo, 0, 32);            // PTS upper part
    vMxPutBits(prMuxInfo, u4Pts, 32);        // PTS lower part
#endif
    vMxPutBits(prMuxInfo, ByteSwap_32(u4FrameSize), 32);  // stream size
    vMxPutBits(prMuxInfo, 1, 8);             // flag (video)
    vMxPutBits(prMuxInfo, 0, 8);             // reserved

    vMxCopyBytes(prMuxInfo, u4StartAddr, u4FrameSize);

#if 0  // notify in timer instead
    vMxNotifyDataReady(prMuxInfo, u4MxBufWpBak);
#endif

    //printf("[MX] #%d @ 0x%08X ", prMuxInfo->u4MxBufFrmCnt++, (UINT32)pucMxBuf + u4MxBufWpBak++);
    //printf("0x%08X ", (UINT32)pucMxBuf + u4MxBufWpBak++);
    //printf("0x%08X\n", (UINT32)pucMxBuf + u4MxBufWpBak);
}


static BOOL _MUXER_CmpbHandleVideoPes(
    MUXER_INFO_T *prMuxInfo,
    //MUXER_PES_INFO_T *prVidPesInfo
    VENC_PES_INFO_T *prVidPesInfo)
{
    UCHAR *pucTmpFrmBuf = prMuxInfo->pucTmpFrmBuf;
    UINT32 u4FrmSa = VIRTUAL(prVidPesInfo->u4StartAddr);
    UINT32 u4FrmSz = prVidPesInfo->u4FrameSize;
    UINT32 u4FifoSa = VIRTUAL(prVidPesInfo->u4FifoStart);
    UINT32 u4FifoEa = VIRTUAL(prVidPesInfo->u4FifoEnd);

    if (u4FifoSa + u4FifoEa == 0 || u4FrmSa + u4FrmSz <= u4FifoEa)
    {
        _MUXER_CmpbMuxH264Es(prMuxInfo, prVidPesInfo->u4StartAddr, prVidPesInfo->u4FrameSize, prVidPesInfo->u4Pts);
    }
    else
    {
        if (u4FrmSz > MUXER_TMP_BUF_SZ)
        {
            ASSERT(0);
            return FALSE;
        }
        x_memcpy((VOID *)pucTmpFrmBuf, (VOID *)u4FrmSa, u4FifoEa - u4FrmSa);
        x_memcpy((VOID *)(pucTmpFrmBuf + u4FifoEa - u4FrmSa), (VOID *)u4FifoSa, u4FrmSa + u4FrmSz - u4FifoEa);
        _MUXER_CmpbMuxH264Es(prMuxInfo, (UINT32)pucTmpFrmBuf, u4FrmSz, prVidPesInfo->u4Pts);
    }

    return TRUE;
}


static BOOL _MUXER_CmpbHandleAudioPes(
    MUXER_INFO_T *prMuxInfo,
    //MUXER_PES_INFO_T *prVidPesInfo
    AENC_PES_INFO_T *prAudPesInfo)
{
    LOG(0, "%s is not implemented yet\n", __FUNCTION__);
    ASSERT(0);
    return TRUE;
}


//-----------------------------------------------------------------------------
// Iner-file functions
//-----------------------------------------------------------------------------
MUXER_HDR_STATE_T _MUXER_HdrMuxCmpb(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    MUXER_CMPB_INFO_T *prCmpbMuxInfo = &prMuxInfo->rFmtInfo.rMuxerCmpbInfo;

    //HAL_TIME_T rTimeRstS, rTimeRstE, rTimeRstDt;
    //HAL_GetTime(&rTimeRstS);

    if (prCmpbMuxInfo->u4CurVidPts < prCmpbMuxInfo->u4CurAudPts || 1)  // video only for now
    {
        VENC_PES_INFO_T *prVEncPesInfo = &prMuxInfo->rVEncPesInfo;

#if 0
        Printf("Mux Video Cur Vid Pts = %d Aud Pts = %d\n",
            prCmpbMuxInfo->u4CurVidPts,
            prCmpbMuxInfo->u4CurAudPts);
#endif

        _MUXER_ReceiveVideoPes(ucMxId);

        //HAL_GetTime(&rTimeRstE);
        //HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
        //LOG(0, "Muxer receive video time = (%ld) \n", rTimeRstDt.u4Micros);

        if (prVEncPesInfo->u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        _MxVideoTypeAnalyze(prMuxInfo, prVEncPesInfo);
        _MUXER_CmpbHandleVideoPes(prMuxInfo, prVEncPesInfo);
#ifdef CC_SUPPORT_VENC
        VENC_UnlockBuf(prVEncPesInfo->pvTag,
            prVEncPesInfo->u4StartAddr,
            prVEncPesInfo->u4FrameSize);
#endif

        prMuxInfo->u4VidCnt++;
        //Printf("Video Count = (%ld) \n", prMuxInfo->u4VidCnt);

        prCmpbMuxInfo->u4CurVidPts = prVEncPesInfo->u4Pts;
        //HAL_GetTime(&rTimeRstE);
        //HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
        //Printf("Muxer deliver video time = (%ld) \n", rTimeRstDt.u4Micros);
    }
    else
    {
        _MUXER_ReceiveAudioPes(ucMxId);

        if (prMuxInfo->rAEncPesInfo.u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        _MUXER_CmpbHandleAudioPes(prMuxInfo, &prMuxInfo->rAEncPesInfo);

        prMuxInfo->u4AudCnt++;
        //Printf("Audio Count = (%ld) \n", prMuxInfo->u4AudCnt);

        prCmpbMuxInfo->u4CurAudPts = prMuxInfo->rAEncPesInfo.u4Pts;
    }

    //HAL_GetTime(&rTimeRstE);
    //HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
    //LOG(0, "Muxer deliver time = (%ld) \n", rTimeRstDt.u4Micros);

#if 0
    LOG(0, "Mux Cur Vid Pts = %d Aud Pts = %d\n",
        prCmpbMuxInfo->u4CurVidPts,
        prCmpbMuxInfo->u4CurAudPts);
#endif
    //prMuxInfo->u4RecTotalTime += rTimeRstDt.u4Micros;
    //Printf("Muxer deliver Total Time = (%ld)\n", prMuxInfo->u4RecTotalTime);

    //_MUXER_QueryVideoPesCnt(ucMxId);
    //_MUXER_QueryAudioPesCnt(ucMxId);

    //_MUXER_QueryPesCnt(ucMxId);

    return eMUXER_HDR_SUCCEED;
}

