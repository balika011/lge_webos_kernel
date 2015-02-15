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
 * $RCSfile: muxer_raw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_raw.c
 *  Raw format multiplexer driver - implementation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "muxer_raw.h"
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


static BOOL _MUXER_RawHandleVideoPes(
    MUXER_INFO_T *prMuxInfo,
    VENC_PES_INFO_T *prVidPesInfo)
{
    UINT32 u4FrmSa = VIRTUAL(prVidPesInfo->u4StartAddr);
    UINT32 u4FrmSz = prVidPesInfo->u4FrameSize;
    UINT32 u4FifoSa = VIRTUAL(prVidPesInfo->u4FifoStart);
    UINT32 u4FifoEa = VIRTUAL(prVidPesInfo->u4FifoEnd);

    if (!MxWaitBufferAvail(prMuxInfo, u4FrmSz))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return FALSE;
    }

    if (u4FifoSa + u4FifoEa == 0 || u4FrmSa + u4FrmSz <= u4FifoEa)
    {
        vMxCopyBytes(prMuxInfo, u4FrmSa, u4FrmSz);
    }
    else
    {
        UINT32 u4Size1 = u4FifoEa - u4FrmSa;
        UINT32 u4Size2 = u4FrmSz - u4Size1;
        vMxCopyBytes(prMuxInfo, u4FrmSa, u4Size1);
        vMxCopyBytes(prMuxInfo, u4FifoSa, u4Size2);
    }

    return TRUE;
}


static BOOL _MUXER_RawHandleAudioPes(
    MUXER_INFO_T *prMuxInfo,
    AENC_PES_INFO_T *prAudPesInfo)
{
    if (!MxWaitBufferAvail(prMuxInfo, prAudPesInfo->u4FrameSize))
    {
        LOG(0, "%s(%d): MxWaitBufferAvail fail\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return FALSE;
    }

    vMxCopyBytes(
        prMuxInfo,
        VIRTUAL(prAudPesInfo->u4StartAddr),
        prAudPesInfo->u4FrameSize);

    return TRUE;
}


//-----------------------------------------------------------------------------
// Iner-file functions
//-----------------------------------------------------------------------------
MUXER_HDR_STATE_T _MUXER_HdrMuxRaw(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    MUXER_RAW_INFO_T *prRawMuxInfo = &prMuxInfo->rFmtInfo.rMuxerRawInfo;
    ENUM_MUXER_STRMTYPE_T eStrm;
    UINT32 u4StrmNo, u4Pts = 0;

    _MxStrmSelect(prMuxInfo, &eStrm, &u4StrmNo);

    if (eMUXER_STRMTYPE_VIDEO == eStrm)
    {
        VENC_PES_INFO_T *prVEncPesInfo = &prMuxInfo->rVEncPesInfo;
        _MUXER_ReceiveVideoPes(ucMxId);

        if (prVEncPesInfo->u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        _MxVideoTypeAnalyze(prMuxInfo, prVEncPesInfo);
        _MUXER_RawHandleVideoPes(prMuxInfo, prVEncPesInfo);
#ifdef CC_SUPPORT_VENC
        VENC_UnlockBuf(prVEncPesInfo->pvTag,
            prVEncPesInfo->u4StartAddr,
            prVEncPesInfo->u4FrameSize);
#endif

#if 1 // only send frame flow
        if (!prMuxInfo->fgFrame)
        {
            LOG(6, "MxRaw(%d): not Frame\n", ucMxId);
            return eMUXER_HDR_SUCCEED;
        }
#endif
        prMuxInfo->u4VidCnt++;
        //Printf("Video Count = (%ld) \n", prMuxInfo->u4VidCnt);

        u4Pts = prRawMuxInfo->u4CurVidPts = prVEncPesInfo->u4Pts;
        _vMxUpdatePts(prMuxInfo, u4StrmNo, prRawMuxInfo->u4CurVidPts);
    }
    else if (eMUXER_STRMTYPE_AUDIO == eStrm)
    {
        _MUXER_ReceiveAudioPes(ucMxId);

        if (prMuxInfo->rAEncPesInfo.u4FrameSize == MUXER_INVALID_UINT32)
        {
            return eMUXER_HDR_SUCCEED;
        }

        _MUXER_RawHandleAudioPes(prMuxInfo, &prMuxInfo->rAEncPesInfo);

        prMuxInfo->u4AudCnt++;
        //Printf("Audio Count = (%ld) \n", prMuxInfo->u4AudCnt);

        u4Pts = prRawMuxInfo->u4CurAudPts = prMuxInfo->rAEncPesInfo.u4Pts;
        _vMxUpdatePts(prMuxInfo, u4StrmNo, prRawMuxInfo->u4CurAudPts);
    }
    else if (eMUXER_STRMTYPE_MAX == eStrm)
    {
        return eMUXER_HDR_NO_DATA;
    }
    else
    {
        LOG(0, "MxRaw(%d): invalid stream (%d)\n", ucMxId, eStrm);
        ASSERT(0);
        x_thread_delay(1);
        return eMUXER_HDR_FAIL;
    }

#if 0
    LOG(0, "Mux Cur Vid Pts = %d Aud Pts = %d\n",
        prRawMuxInfo->u4CurVidPts,
        prRawMuxInfo->u4CurAudPts);
#endif

    prMuxInfo->u4MxBufWpNfy = prMuxInfo->u4MxBufWp;
    if (!prMuxInfo->u4NfyPeriod)
    {
        vMxNotifyDataReady(prMuxInfo, u4Pts);
    }

    return eMUXER_HDR_SUCCEED;
}


