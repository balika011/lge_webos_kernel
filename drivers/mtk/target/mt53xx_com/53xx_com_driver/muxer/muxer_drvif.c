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
 * $RCSfile: muxer_drvif.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_drvif.c
 *  Software program stream demux driver - public interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "muxer_if.h"
#include "muxer_drvif.h"
#include "muxer_mpgts.h"
#include "muxer_cmpb.h"
#include "muxer_raw.h"
#include "muxer_debug.h"
#ifdef __KERNEL__
//#include <linux/uaccess.h>
#endif
#ifdef CC_SUPPORT_VENC
#include "venc_if.h"
#endif

#ifndef CC_SUPPORT_MUXER
#error "mx module need macro CC_SUPPORT_MUXER"
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

static MUXER_INFO_T _arMuxerInfo[MUXER_MAX_INST_NS];


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#ifdef CC_SUPPORT_VENC
#define MX_IS_H264_SPS(type) \
    (VENC_H264_PIC_TYPE_SPS == (VENC_H264_PIC_TYPE)(type))

#define MX_IS_H264_FRM(type)                                \
     (VENC_H264_PIC_TYPE_IDR == (VENC_H264_PIC_TYPE)(type)) \
    || (VENC_H264_PIC_TYPE_I == (VENC_H264_PIC_TYPE)(type)) \
    || (VENC_H264_PIC_TYPE_P == (VENC_H264_PIC_TYPE)(type)) \
    || (VENC_H264_PIC_TYPE_B == (VENC_H264_PIC_TYPE)(type))
#endif


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

extern void _AUD_SetPsrStc1(UINT32 u4Val);


//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static VOID _MUXER_NfyDataReady(HANDLE_T pt_tm_handle, VOID *pv_tag)
{
    vMxNotifyDataReady((MUXER_INFO_T *)pv_tag, 0);

    UNUSED(pt_tm_handle);
}


static BOOL _MUXER_WakeUp(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    VENC_PES_INFO_T rVidPes;
    AENC_PES_INFO_T rAudPes;
    VBIENC_PES_INFO_T rVBIPes;

    switch (prMuxInfo->eCurrentWaiting)
    {
    default:
        LOG(1, "%s: unexpected type %d\n", __FUNCTION__, prMuxInfo->eCurrentWaiting);
        break;

    case eMUXER_STRMTYPE_MAX:
        break;

    case eMUXER_STRMTYPE_VIDEO:
        rVidPes.u4FrameSize = MUXER_INVALID_UINT32;
        _MUXER_SendVideoPes(ucMxId, &rVidPes);
        break;

    case eMUXER_STRMTYPE_AUDIO:
        rAudPes.u4FrameSize = MUXER_INVALID_UINT32;
        _MUXER_SendAudioPes(ucMxId, &rAudPes);
        break;

    case eMUXER_STRMTYPE_VBI:
        rVBIPes.u4FrameSize = MUXER_INVALID_UINT32;
        _MUXER_SendVBIPes(ucMxId, &rVBIPes);
        break;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MUXER_GetCmdQSize
 *  The upper layer uses this API to Get CmdQSize to Muxer main loop
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
static BOOL _MUXER_GetCmdQSize(MUXER_INFO_T *prMuxInfo, UINT16* pu2QueueSize)
{
    VERIFY(x_msg_q_num_msgs(prMuxInfo->hMuxerCmdQ, pu2QueueSize) == OSR_OK);
    LOG(1, "%s: u2QueueSize is %d\n", __FUNCTION__, *pu2QueueSize);

    if(*pu2QueueSize > 0)
    {
        LOG(0, "%s: error, QueueSize > 0\n", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_SendCmd
 *  The upper layer uses this API to send commands to Muxer main loop
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
static BOOL _MUXER_SendCmd(UCHAR ucMxId, ENUM_MUXER_STATE_T eMuxerState)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;
    UINT32 u4PlayCmd = eMuxerState;
    SIZE_T zMsgSize = sizeof(UINT32);

    i4Ret = x_msg_q_send(prMuxInfo->hMuxerCmdQ, &u4PlayCmd, zMsgSize, 255);
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        LOG(1, "Muxer Cmd Queue, OSR_TOO_MANY\n");
        i4Ret = x_msg_q_send(prMuxInfo->hMuxerCmdQ, &u4PlayCmd, zMsgSize, 255);
    }

    VERIFY(i4Ret == OSR_OK);

    return TRUE;

}


//-----------------------------------------------------------------------------
/** _MUXER_ReceiveCmd
 *  Muxer main loop uses this API to get commands from the upper layer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
static BOOL _MUXER_ReceiveCmd(UCHAR ucMxId, VOID *pvData, UINT32 u4CurState)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;

    zMsgSize = sizeof(UINT32);

    if (u4CurState != eMUXER_STATE_RUN)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(prMuxInfo->hMuxerCmdQ), 1, X_MSGQ_OPTION_WAIT);
    }
    else
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(prMuxInfo->hMuxerCmdQ), 1, X_MSGQ_OPTION_NOWAIT);
    }

    ASSERT(zMsgSize == sizeof(UINT32));

    if (i4Ret != OSR_OK)
    {
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _SWMUX_MainLoop
 *  Software muxer main loop
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID _MUXER_MainLoop(VOID *pvArg)
{
    UCHAR ucMxId;
    UINT32 u4MuxCmd;
    MUXER_INFO_T *prMuxInfo;

    if (pvArg)
    {
        ucMxId = *(UCHAR *)pvArg;
        if (ucMxId >= MUXER_MAX_INST_NS)
        {
            ASSERT(0);
            return;
        }
        prMuxInfo = _MUXER_QueryInst(ucMxId);
    }
    else
    {
        ASSERT(0);
        return;
    }

    while (1)
    {
        if (_MUXER_ReceiveCmd(ucMxId, &u4MuxCmd, prMuxInfo->eMuxerStatus))
        {
            ENUM_MUXER_STATE_T eMuxCmd = (ENUM_MUXER_STATE_T)u4MuxCmd;

            // Function needs to be implemented.
            if (prMuxInfo->eMuxerStatus == eMUXER_STATE_STOP && eMuxCmd == eMUXER_STATE_RUN)
            {
                prMuxInfo->fgKeyframe = FALSE;
                if (prMuxInfo->pfnStart != NULL)
                {
                    prMuxInfo->pfnStart(ucMxId);
                }
            }
            if (prMuxInfo->eMuxerStatus == eMUXER_STATE_RUN && eMuxCmd == eMUXER_STATE_STOP)
            {
                if (prMuxInfo->pfnStop != NULL)
                {
                    prMuxInfo->pfnStop(ucMxId);
                }
                _MUXER_FlushData(ucMxId);
            }
            if (eMuxCmd == eMUXER_STATE_FLUSH)
            {
                UINT32 u4CurWp = prMuxInfo->u4MxBufWp;
                prMuxInfo->u4MxBufWpNfy = u4CurWp;
                prMuxInfo->u4LastNfyBufWp = u4CurWp;
                prMuxInfo->u4MxBufRp = u4CurWp;
                _MUXER_FlushData(ucMxId);
            }
            else
            {
                prMuxInfo->eMuxerPrevousStatus = prMuxInfo->eMuxerStatus;
                prMuxInfo->eMuxerStatus = eMuxCmd;
            }

            switch (eMuxCmd)
            {
            default:
                break;

            case eMUXER_STATE_RUN:
            case eMUXER_STATE_STOP:
            case eMUXER_STATE_FLUSH:
                _MxNotifySyncCmdDone(prMuxInfo);
                break;
            }
        }

        if (prMuxInfo->eMuxerStatus == eMUXER_STATE_RUN)
        {
            MUXER_HDR_STATE_T eHdrStt = prMuxInfo->pfnMuxHdr(ucMxId);

            LOG(9, "state: %d\n", eHdrStt);
            switch (eHdrStt)
            {
            default:
                LOG(0, "Unexpected state: %d\n", eHdrStt);
                ASSERT(0);
                x_thread_delay(1);
                break;

            case eMUXER_HDR_SUCCEED:
                // normal state
                break;

            case eMUXER_HDR_NO_DATA:
                x_thread_delay(1);
                break;
            }
        }
        else if (prMuxInfo->eMuxerStatus == eMUXER_STATE_STOP)
        {
        }
    }
}


static VOID PUT_HALF_BYTE(MUXER_INFO_T *prMuxInfo, UINT8 u1HalfByte)
{
    UCHAR *pucMxBuf = prMuxInfo->pucMxBuf;
    UINT8 u1OriginalByte, u1NewByte;

    u1OriginalByte = *(pucMxBuf + prMuxInfo->u4MxBufWp);
    if (prMuxInfo->u4BitPos / 4)
    {
        u1NewByte = u1OriginalByte & 0xF0;
        u1NewByte |= (u1HalfByte & 0x0F);
        *(pucMxBuf + prMuxInfo->u4MxBufWp) = u1NewByte;
        prMuxInfo->u4BitPos = 0;
        if (++prMuxInfo->u4MxBufWp == prMuxInfo->u4MxBufSz)
        {
            prMuxInfo->u4MxBufWp = 0;
        }
    }
    else
    {
        u1NewByte = u1OriginalByte & 0x0F;
        u1NewByte |= (u1HalfByte << 4);
        *(pucMxBuf + prMuxInfo->u4MxBufWp) = u1NewByte;
        prMuxInfo->u4BitPos = 4;
    }
}


VOID MxFlushRing(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp)
{
    if (u4Wp > u4Rp)
    {
        HalFlushDCacheMultipleLine((UINT32)prMuxInfo->pucMxBuf + u4Rp, u4Wp - u4Rp);
    }
    else
    {
        HalFlushDCacheMultipleLine((UINT32)prMuxInfo->pucMxBuf + u4Rp, prMuxInfo->u4MxBufSz - u4Rp);
        HalFlushDCacheMultipleLine((UINT32)prMuxInfo->pucMxBuf, u4Wp);
    }
}


static VOID MxInvalidateRing(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp)
{
    if (u4Wp > u4Rp)
    {
        HalInvalidateDCacheMultipleLine((UINT32)prMuxInfo->pucMxBuf + u4Rp, u4Wp - u4Rp);
    }
    else
    {
        HalInvalidateDCacheMultipleLine((UINT32)prMuxInfo->pucMxBuf + u4Rp, prMuxInfo->u4MxBufSz - u4Rp);
        HalInvalidateDCacheMultipleLine((UINT32)prMuxInfo->pucMxBuf, u4Wp);
    }
}


VOID MxScramble(MUXER_INFO_T *prMuxInfo, UINT32 u4Rp, UINT32 u4Wp)
{
    TSDESC_PARAM_T *prParam = &prMuxInfo->rTsDescParam;
    UINT32 u4MxBuf = (UINT32)prMuxInfo->pucMxBuf;
    UINT32 u4MxBufSz = prMuxInfo->u4MxBufSz;
    INT32 i4Ret;

    if (MUXER_FMT_MPEG2_TS_192 == prMuxInfo->eCurFMT)
    {
        prParam->eForm = TSDESC_192_PACKET_FORM;
    }
    else if (MUXER_FMT_MPEG2_TS == prMuxInfo->eCurFMT)
    {
        prParam->eForm = TSDESC_188_PACKET_FORM;
    }
    else
    {
        LOG(1, "%s: unexpected scramble(%d)\n", __FUNCTION__, prMuxInfo->eCurFMT);
        return;
    }

    prParam->u4SrcStartAddr = u4MxBuf + u4Rp;
    prParam->u4SrcBufStart = u4MxBuf ;
    prParam->u4SrcBufEnd = u4MxBuf  + u4MxBufSz;
    prParam->u4DstStartAddr = u4MxBuf + u4Rp;
    prParam->u4DstBufStart = u4MxBuf ;
    prParam->u4DstBufEnd = u4MxBuf  + u4MxBufSz;
    prParam->u4DatLen  = (u4Wp > u4Rp)
        ? (u4Wp - u4Rp)
        : (u4MxBufSz - u4Rp + u4Wp);

    MxInvalidateRing(prMuxInfo, u4Rp, u4Wp);
    i4Ret = GCPU_Cmd(0, GCPU_TSSCRM, prParam);
    MxInvalidateRing(prMuxInfo, u4Rp, u4Wp);
    if (S_GCPU_OK != i4Ret)
    {
        LOG(1, "%s call GCPU_TSSCRM fail(%d!!!\n", __FUNCTION__, i4Ret);
    }
}


static BOOL _MxStrmAvailable(UCHAR ucMxId, ENUM_MUXER_STRMTYPE_T eStrm)
{
    BOOL fgRet = FALSE;
    UINT16 u2QueueSize = 0;;

    switch (eStrm)
    {
    case eMUXER_STRMTYPE_MAX:
    default:
        break;

    case eMUXER_STRMTYPE_VIDEO:
        fgRet = _MUXER_QueryVideoPesCnt(ucMxId, &u2QueueSize);
        break;

    case eMUXER_STRMTYPE_AUDIO:
        fgRet = _MUXER_QueryAudioPesCnt(ucMxId, &u2QueueSize);
        break;

    case eMUXER_STRMTYPE_VBI:
        fgRet = _MUXER_QueryVBIPesCnt(ucMxId, &u2QueueSize);
        break;
    }

    return (fgRet && u2QueueSize)? TRUE : FALSE;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** SWDMX_Init
 *  Initialize software program stream demux driver
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
VOID _MUXER_Init(VOID)
{
    static BOOL fgInited = FALSE;
    int i, j;
    char szName[] = { "        " };

    if (fgInited) return;

    for (i = 0; i < MUXER_MAX_INST_NS; i++)
    {
        if (!_arMuxerInfo[i].fgMuxerInited)
        {
            LOG(1, "Muxer Init for instance #%d\n", i);
            _arMuxerInfo[i].ucMxId = i;

            // create muxer command queue
            x_memcpy((VOID *)szName, "MX_CMDQx", 9);
            szName[7] = '0' + i;
            VERIFY(x_msg_q_create(&_arMuxerInfo[i].hMuxerCmdQ, szName, sizeof(UINT32),
                                  MUXER_CMD_Q_SIZE) == OSR_OK);

            // create muxer video data queue
            x_memcpy((VOID *)szName, "MX_VIDQx", 9);
            szName[7] = '0' + i;
            VERIFY(x_msg_q_create(&_arMuxerInfo[i].hMuxerVidDataQ, szName, sizeof(VENC_PES_INFO_T),
                                  MUXER_VID_PES_Q_SIZE) == OSR_OK);

            // create muxer audio data queue
            x_memcpy((VOID *)szName, "MX_AUDQx", 9);
            szName[7] = '0' + i;
            VERIFY(x_msg_q_create(&_arMuxerInfo[i].hMuxerAudDataQ, szName, sizeof(AENC_PES_INFO_T),
                                  MUXER_AUD_PES_Q_SIZE) == OSR_OK);

            // create muxer VBI data queue
            x_memcpy((VOID *)szName, "MX_VBIQx", 9);
            szName[7] = '0' + i;
            VERIFY(x_msg_q_create(&_arMuxerInfo[i].hMuxerVBIDataQ, szName, sizeof(VBIENC_PES_INFO_T),
                                  MUXER_VBI_PES_Q_SIZE) == OSR_OK);

            // create muxer semaphore
            VERIFY(x_sema_create(&_arMuxerInfo[i].hMxSyncSema, X_SEMA_TYPE_BINARY,
                                 X_SEMA_STATE_LOCK) == OSR_OK);

            // create muxer wait buffer semaphore
            VERIFY(x_sema_create(&_arMuxerInfo[i].hMxWaitBufSema, X_SEMA_TYPE_BINARY,
                                 X_SEMA_STATE_LOCK) == OSR_OK);

            // create muxer notification timer
            VERIFY(x_timer_create(&_arMuxerInfo[i].hMuxerNfyTimer) == OSR_OK);

            // create muxer wait buffer timer
            VERIFY(x_timer_create(&_arMuxerInfo[i].hMxWaitBufTimer) == OSR_OK);

            // create muxer main loop thread for command receiving
            x_memcpy((VOID *)szName, "MX_THDx", 8);
            szName[6] = '0' + i;
            VERIFY(x_thread_create(&_arMuxerInfo[i].hMuxerMainThread,
                                   szName, 4096, MUXER_THREAD_PRIORITY,
                                   (x_os_thread_main_fct)_MUXER_MainLoop, sizeof(UCHAR), (VOID *)&i) == OSR_OK);
#if 0
            if (_arMuxerInfo[i].pucTmpFrmBuf == NULL)
            {
                _arMuxerInfo[i].pucTmpFrmBuf = (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(MUXER_TMP_BUF_SZ, 16));
                ASSERT(_arMuxerInfo[i].pucTmpFrmBuf != NULL);
            }
#endif
            _arMuxerInfo[i].eCurrentWaiting = eMUXER_STRMTYPE_MAX;
            _arMuxerInfo[i].u4ValidStrmNs = 0;
            for (j = 0; j < MUXER_MAX_STRM_NS; j++)
            {
                _arMuxerInfo[i].arStrm[j].eStrmType = eMUXER_STRMTYPE_MAX;
                _arMuxerInfo[i].arStrm[j].fgEnabled = FALSE;
            }

            _arMuxerInfo[i].pucMxBuf = NULL;
            _arMuxerInfo[i].u4MxBufSz = 0;
            _arMuxerInfo[i].u4MxBufWp = 0;
            _arMuxerInfo[i].u4MxBufRp = 0;
            _arMuxerInfo[i].pfNotify = NULL;
            _arMuxerInfo[i].u4NfyPeriod = 0;
            _arMuxerInfo[i].u4LastNfyBufWp = 0;

            _arMuxerInfo[i].fgMuxerInited = TRUE;
            _arMuxerInfo[i].fgOccupied = FALSE;
            _arMuxerInfo[i].pfnMuxHdr = NULL;
            _arMuxerInfo[i].pfnStart = NULL;
            _arMuxerInfo[i].pfnStop = NULL;
            _arMuxerInfo[i].u4BitPos = 0;
            _arMuxerInfo[i].fgTrustZone = FALSE;
            _arMuxerInfo[i].u2PcrPid = 0;
            _arMuxerInfo[i].u2VideoPid = 0;
            _arMuxerInfo[i].u2AudioPid = 0;

            x_memset((VOID *)&_arMuxerInfo[i].rFmtInfo, 0, sizeof(_arMuxerInfo[i].rFmtInfo));
            x_memset((VOID *)&_arMuxerInfo[i].arSection, 0, sizeof(_arMuxerInfo[i].arSection));
        }
    }

    fgInited = TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_GetInst
 *  Get a Muxer instance
 *
 *  @retval MX_RET
 */
//-----------------------------------------------------------------------------
MX_RET _MUXER_GetInst(UCHAR *pucMxId)
{
    int i;

    if (NULL == pucMxId)
    {
        LOG(0, "%s: pucMxId is NULL!\n", __FUNCTION__);
        ASSERT(0);
        return MX_RET_GNRL_ERR;
    }

    for (i = 0; i < MUXER_MAX_INST_NS; i++)
    {
        if (!_arMuxerInfo[i].fgOccupied)
        {
            _arMuxerInfo[i].fgOccupied = TRUE;
            *pucMxId = i;
            return MX_RET_OK;
        }
    }

    return MX_RET_NO_AVAIL_INST;
}


//-----------------------------------------------------------------------------
/** _MUXER_ReleaseInst
 *  Release a Muxer instance
 *
 *  @retval MX_RET
 */
//-----------------------------------------------------------------------------
MX_RET _MUXER_ReleaseInst(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    if (prMuxInfo->fgOccupied)
    {
        prMuxInfo->fgOccupied = FALSE;
        return MX_RET_OK;
    }

    return MX_RET_INST_NOT_OCCUPIED;
}


//-----------------------------------------------------------------------------
/** _MUXER_QueryInst
 *  Query a Muxer instance
 *
 *  @retval MUXER_INFO_T *
 */
//-----------------------------------------------------------------------------
MUXER_INFO_T *_MUXER_QueryInst(UCHAR ucMxId)
{
    MUXER_VERIFY_ID_SET(ucMxId, 0);
    return &(_arMuxerInfo[ucMxId]);
}


//-----------------------------------------------------------------------------
/** _MUXER_Start
 *  Start to run muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_Start(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo;
    BOOL fgRet = TRUE;

    if (ucMxId >= MUXER_MAX_INST_NS)
    {
        ASSERT(0);
        return FALSE;
    }

    prMuxInfo = _MUXER_QueryInst(ucMxId);

    if (prMuxInfo->fgMuxerInited
    && prMuxInfo->eMuxerStatus != eMUXER_STATE_RUN)
    {
        if (prMuxInfo->u4NfyPeriod)
        {
            VERIFY(x_timer_start(
                prMuxInfo->hMuxerNfyTimer,
                prMuxInfo->u4NfyPeriod,
                X_TIMER_FLAG_REPEAT,
                _MUXER_NfyDataReady,
                (VOID *)prMuxInfo) == OSR_OK);
        }

        prMuxInfo->u4BitPos = 0;
        prMuxInfo->u4MxBufFrmCnt = 0;

        _AUD_SetPsrStc1(0);  // tmp solution

        _MxInitSyncCmdFlag(prMuxInfo);
        fgRet = _MUXER_SendCmd(ucMxId, eMUXER_STATE_RUN);
        _MUXER_WakeUp(ucMxId);
        _MxWaitSyncCmdDone(prMuxInfo);
        return fgRet;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** SWDMX_Pause
 *  Pause software demux
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_Pause(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo;
    BOOL fgRet = FALSE;

    if (ucMxId >= MUXER_MAX_INST_NS)
    {
        ASSERT(0);
        return FALSE;
    }

    prMuxInfo = _MUXER_QueryInst(ucMxId);

    if (prMuxInfo->fgMuxerInited && prMuxInfo->eMuxerStatus == eMUXER_STATE_RUN)
    {
        fgRet = _MUXER_SendCmd(ucMxId, eMUXER_STATE_PAUSE);

        _MUXER_WakeUp(ucMxId);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** SWDMX_Stop
 *  Stop software program stream demux
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_Stop(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo;
    BOOL fgRet = TRUE;

    if (ucMxId >= MUXER_MAX_INST_NS)
    {
        ASSERT(0);
        return FALSE;
    }

    prMuxInfo = _MUXER_QueryInst(ucMxId);

    if (prMuxInfo->fgMuxerInited && prMuxInfo->eMuxerStatus != eMUXER_STATE_STOP)
    {
        if (prMuxInfo->u4NfyPeriod)
        {
            VERIFY(x_timer_stop(prMuxInfo->hMuxerNfyTimer) == OSR_OK);
        }

        LOG(1, "init stop(%d)\n", ucMxId);
        _MxInitSyncCmdFlag(prMuxInfo);
        fgRet = _MUXER_SendCmd(ucMxId, eMUXER_STATE_STOP);
        _MUXER_WakeUp(ucMxId);
        _MxWaitSyncCmdDone(prMuxInfo);
        LOG(1, "stop ok(%d)\n", ucMxId);
    }
    else if (!prMuxInfo->fgMuxerInited)
    {
        LOG(1, "mx(%d) is not init\n", ucMxId);
        ASSERT(0);
        return FALSE;
    }
    else
    {
        LOG(1, "mx(%d) is already stop\n", ucMxId);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** SWDMX_Flush
 *  Flush A/V data
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_Flush(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo;
    BOOL fgRet = FALSE;

    if (ucMxId >= MUXER_MAX_INST_NS)
    {
        ASSERT(0);
        return FALSE;
    }

    prMuxInfo = _MUXER_QueryInst(ucMxId);

    if (prMuxInfo->fgMuxerInited)
    {
        if (prMuxInfo->eMuxerStatus != eMUXER_STATE_STOP)
        {
            return FALSE;
        }

        _MxInitSyncCmdFlag(prMuxInfo);
        fgRet = _MUXER_SendCmd(ucMxId, eMUXER_STATE_FLUSH);
        _MUXER_WakeUp(ucMxId);
        _MxWaitSyncCmdDone(prMuxInfo);
        return fgRet;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MUXER_SendVideoPes
 *  Send Video Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_SendVideoPes(UCHAR ucMxId, VENC_PES_INFO_T *prVEncPes)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;

    LOG(6, "%s(%d) pts:%x, size:%d\n", __FUNCTION__, ucMxId, prVEncPes->u4Pts, prVEncPes->u4FrameSize);

    if(prVEncPes->u4FrameSize == 0)
    {
        return FALSE;
    }
    
    i4Ret = x_msg_q_send(prMuxInfo->hMuxerVidDataQ, (void *)prVEncPes,
        sizeof(VENC_PES_INFO_T), 255);

    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        LOG(1, "Muxer Video Data Q#%d, OSR_TOO_MANY\n", ucMxId);
        i4Ret = x_msg_q_send(prMuxInfo->hMuxerVidDataQ, (void *)prVEncPes,
            sizeof(VENC_PES_INFO_T), 255);
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MUXER_ReceiveVideoPes
 *  Send Video Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_ReceiveVideoPes(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;

    zMsgSize = sizeof(VENC_PES_INFO_T);

    prMuxInfo->eCurrentWaiting = eMUXER_STRMTYPE_VIDEO;
    i4Ret = x_msg_q_receive(&u2MsgQIdx, &prMuxInfo->rVEncPesInfo, &zMsgSize,
        &(prMuxInfo->hMuxerVidDataQ), 1, X_MSGQ_OPTION_WAIT);
    prMuxInfo->eCurrentWaiting = eMUXER_STRMTYPE_MAX;

    if (i4Ret != OSR_OK)
    {
        LOG(1, "Muxer #%d: Fail to receive Vid Pes Error code =%d\n", ucMxId, i4Ret);
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_QueryVideoPes
 *  Query the number of Video Pes Count
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_QueryVideoPesCnt(UCHAR ucMxId, UINT16 *pu2QueueSize)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;

    i4Ret = x_msg_q_num_msgs(prMuxInfo->hMuxerVidDataQ, pu2QueueSize);

    if (i4Ret == OSR_OK)
    {
        LOG(9, "Muxer #%d: Video Q size = %d\n", ucMxId, *pu2QueueSize);
        return TRUE;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MUXER_SendAudioPes
 *  Send Audio Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_SendAudioPes(UCHAR ucMxId, AENC_PES_INFO_T *prAEncPes)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;

    LOG(6, "eMuxerStatus is %d\n", prMuxInfo->eMuxerStatus); 
    if(prMuxInfo->eMuxerStatus == eMUXER_STATE_STOP)
    {
        LOG(5, "Muxer is not run, so send audio pes not ok!\n"); 
        return FALSE;
    }
    if(prAEncPes->u4FrameSize == 0)
    {
        return FALSE;
    }
    LOG(6, "%s(%d) pts:%x, size:%d\n", __FUNCTION__, ucMxId, prAEncPes->u4Pts, prAEncPes->u4FrameSize);

    i4Ret = x_msg_q_send(prMuxInfo->hMuxerAudDataQ, (void *)prAEncPes,
        sizeof(AENC_PES_INFO_T), 255);

    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        LOG(1, "Muxer Audio Data Q#%d, OSR_TOO_MANY\n", ucMxId);
        i4Ret = x_msg_q_send(prMuxInfo->hMuxerAudDataQ, (void *)prAEncPes,
            sizeof(AENC_PES_INFO_T), 255);
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MUXER_ReceiveAudioPes
 *  Send Audio Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_ReceiveAudioPes(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;

    zMsgSize = sizeof(AENC_PES_INFO_T);

    prMuxInfo->eCurrentWaiting = eMUXER_STRMTYPE_AUDIO;
    i4Ret = x_msg_q_receive(&u2MsgQIdx, &prMuxInfo->rAEncPesInfo, &zMsgSize,
        &(prMuxInfo->hMuxerAudDataQ), 1, X_MSGQ_OPTION_WAIT);
    prMuxInfo->eCurrentWaiting = eMUXER_STRMTYPE_MAX;

    if (i4Ret != OSR_OK)
    {
        LOG(1, "Muxer #%d: Fail to receive Aud Pes Error code =%d\n", ucMxId, i4Ret);
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_QueryAudioPes
 *  Query the number of Audio Pes Count
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_QueryAudioPesCnt(UCHAR ucMxId, UINT16 *pu2QueueSize)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;

    i4Ret = x_msg_q_num_msgs(prMuxInfo->hMuxerAudDataQ, pu2QueueSize);

    if (i4Ret == OSR_OK)
    {
        LOG(9, "Muxer #%d: Audio Q size = %d\n", ucMxId, *pu2QueueSize);
        return TRUE;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MUXER_SendVBIPes
 *  Send VBI Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_SendVBIPes(UCHAR ucMxId, VBIENC_PES_INFO_T *prVBIEncPes)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;

    i4Ret = x_msg_q_send(prMuxInfo->hMuxerVBIDataQ, (void *)prVBIEncPes,
        sizeof(VBIENC_PES_INFO_T), 255);

    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        LOG(1, "Muxer VBI Data Q#%d, OSR_TOO_MANY\n", ucMxId);
        i4Ret = x_msg_q_send(prMuxInfo->hMuxerVBIDataQ, (void *)prVBIEncPes,
            sizeof(VBIENC_PES_INFO_T), 255);
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** _MUXER_ReceiveVBIPes
 *  Receive VBI Pes from Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_ReceiveVBIPes(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;

    zMsgSize = sizeof(VBIENC_PES_INFO_T);

    prMuxInfo->eCurrentWaiting = eMUXER_STRMTYPE_VBI;
    i4Ret = x_msg_q_receive(&u2MsgQIdx, &prMuxInfo->rVBIEncPesInfo, &zMsgSize,
        &(prMuxInfo->hMuxerVBIDataQ), 1, X_MSGQ_OPTION_WAIT);
    prMuxInfo->eCurrentWaiting = eMUXER_STRMTYPE_MAX;

    if (i4Ret != OSR_OK)
    {
        LOG(1, "Muxer #%d: Fail to receive VBI Pes Error code =%d\n", ucMxId, i4Ret);
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_QueryVBIPes
 *  Query the number of VBI Pes Count
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_QueryVBIPesCnt(UCHAR ucMxId, UINT16 *pu2QueueSize)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    INT32 i4Ret;

    i4Ret = x_msg_q_num_msgs(prMuxInfo->hMuxerVBIDataQ, pu2QueueSize);

    if (i4Ret == OSR_OK)
    {
        LOG(7, "Muxer #%d: VBI Q size = %d\n", ucMxId, *pu2QueueSize);
        return TRUE;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
/** SWDMX_Init
 *  Initialize software program stream demux driver
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
VOID _MUXER_FlushData(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo;
    char szName[] = { "        " };

    if (ucMxId >= MUXER_MAX_INST_NS)
    {
        ASSERT(0);
        return;
    }

    prMuxInfo = _MUXER_QueryInst(ucMxId);

    // re-create muxer video data queue
    VERIFY(x_msg_q_delete(prMuxInfo->hMuxerVidDataQ) == OSR_OK);
    x_memcpy((VOID *)szName, "MX_VIDQx", 9);
    szName[7] = '0' + ucMxId;
    VERIFY(x_msg_q_create(&prMuxInfo->hMuxerVidDataQ, szName, sizeof(VENC_PES_INFO_T),
                          MUXER_VID_PES_Q_SIZE) == OSR_OK);

    // re-create muxer audio data queue
    VERIFY(x_msg_q_delete(prMuxInfo->hMuxerAudDataQ) == OSR_OK);
    x_memcpy((VOID *)szName, "MX_AUDQx", 9);
    szName[7] = '0' + ucMxId;
    VERIFY(x_msg_q_create(&prMuxInfo->hMuxerAudDataQ, szName, sizeof(AENC_PES_INFO_T),
                          MUXER_AUD_PES_Q_SIZE) == OSR_OK);

    // re-create muxer VBI data queue
    VERIFY(x_msg_q_delete(prMuxInfo->hMuxerVBIDataQ) == OSR_OK);
    x_memcpy((VOID *)szName, "MX_VBIQx", 9);
    szName[7] = '0' + ucMxId;
    VERIFY(x_msg_q_create(&prMuxInfo->hMuxerVBIDataQ, szName, sizeof(VBIENC_PES_INFO_T),
                          MUXER_VBI_PES_Q_SIZE) == OSR_OK);
}

//-----------------------------------------------------------------------------
/** _MUXER_SetMuxFmt
 *  Set Muxer Format
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_SetMuxFmt(UCHAR ucMxId, ENUM_MUXER_FMT_T eMuxFmt)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    switch(eMuxFmt)
    {
    case MUXER_FMT_MPEG2_TS_192:
    case MUXER_FMT_MPEG2_TS:
        prMuxInfo->pfnMuxHdr = _MUXER_HdrMuxMpgTs;
        prMuxInfo->pfnStart = _MUXER_MpgTsStart;
        prMuxInfo->pfnStop = _MUXER_MpgTsStop;
        break;
    case MUXER_FMT_CMPB:
        prMuxInfo->pfnMuxHdr = _MUXER_HdrMuxCmpb;
        break;
    case MUXER_FMT_RAW:
        prMuxInfo->pfnMuxHdr = _MUXER_HdrMuxRaw;
        break;
    default:
        return FALSE;
    }

    prMuxInfo->eCurFMT = eMuxFmt;

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_SetVencFmt
 *  Set Venc Format
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_SetVencFmt(UCHAR ucMxId, ENUM_MUXER_VENC_T eVidFmt)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    prMuxInfo->eVidFmt = eVidFmt;
    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MUXER_SetAencFmt
 *  Set Aenc Format
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _MUXER_SetAencFmt(UCHAR ucMxId, ENUM_MUXER_AENC_T eAudFmt)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    prMuxInfo->eAudFmt = eAudFmt;
    return TRUE;
}


BOOL _MUXER_SetBuffer(UCHAR ucMxId, UINT32 u4BufSa, UINT32 u4BufSz, MX_FCT_NFY pfNotify, UINT32 u4NfyPeriod)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    ASSERT(u4BufSa + u4BufSz > 0);
    prMuxInfo->pucMxBuf = (UCHAR *)u4BufSa;
    prMuxInfo->u4MxBufSz = u4BufSz;
    prMuxInfo->u4MxBufWp = 0;
    prMuxInfo->u4MxBufRp = 0;
    prMuxInfo->pfNotify = pfNotify;
    prMuxInfo->u4NfyPeriod = u4NfyPeriod;
    prMuxInfo->u4LastNfyBufWp = 0;
    prMuxInfo->u4MxBufWpNfy = 0;

    LOG(1, "%s: %d, 0x%p, %d, 0x%p, %d\n", __FUNCTION__, ucMxId, u4BufSa, u4BufSz, pfNotify, u4NfyPeriod);

    return TRUE;
}


BOOL _MUXER_UpdateRp(UCHAR ucMxId, UINT32 u4Rp)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    if (u4Rp < (UINT32)prMuxInfo->pucMxBuf)
    {
        LOG(0, "invalid rp:0x%08x, pucMxBuf:0x%08x\n", u4Rp, (UINT32)prMuxInfo->pucMxBuf);
        ASSERT(u4Rp >= (UINT32)prMuxInfo->pucMxBuf);
        return FALSE;
    }

    u4Rp -= (UINT32)prMuxInfo->pucMxBuf;
    if (u4Rp > prMuxInfo->u4MxBufSz)
    {
        u4Rp = u4Rp - prMuxInfo->u4MxBufSz;
    }

    if (u4Rp >= prMuxInfo->u4MxBufSz)
    {
        LOG(0, "invalid rp2:0x%08x\n", u4Rp);
        ASSERT(u4Rp < prMuxInfo->u4MxBufSz);
        return FALSE;
    }

    prMuxInfo->u4MxBufRp = u4Rp;
    LOG(6, "rp:0x%08x\n", u4Rp);
    return TRUE;
}


BOOL _MUXER_AddStrm(UCHAR ucMxId, ENUM_MUXER_STRMTYPE_T eStrm, UINT32 *pu4StrmNo)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    UINT32 i;

    if (pu4StrmNo == NULL)
    {
        LOG(1, "Pointer for stream number is NULL!!!\n");
        return FALSE;
    }

    for (i = 0; i < MUXER_MAX_STRM_NS; i++)
    {
        if (prMuxInfo->arStrm[i].eStrmType == eMUXER_STRMTYPE_MAX)
            break;
    }

    if (i == MUXER_MAX_STRM_NS)
    {
        LOG(1, "No available stream number to use!!!\n");
        return FALSE;
    }

    prMuxInfo->arStrm[i].eStrmType = eStrm;
    prMuxInfo->arStrm[i].fgEnabled = TRUE;
    prMuxInfo->u4ValidStrmNs++;
    *pu4StrmNo = i;

    if (eMUXER_STRMTYPE_VIDEO == eStrm)
    {
        prMuxInfo->fgEnVideo = TRUE;
    }
    else if (eMUXER_STRMTYPE_AUDIO == eStrm)
    {
        prMuxInfo->fgEnAudio = TRUE;
    }
    else if (eMUXER_STRMTYPE_VBI == eStrm)
    {
        prMuxInfo->fgEnTeletext = TRUE;
    }

    return TRUE;
}


BOOL _MUXER_DelStrm(UCHAR ucMxId, UINT32 u4StrmNo)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    if (u4StrmNo >= MUXER_MAX_STRM_NS)
    {
        LOG(1, "Stream number overflow!!!\n");
        return FALSE;
    }

    if (prMuxInfo->arStrm[u4StrmNo].eStrmType == eMUXER_STRMTYPE_MAX)
    {
        LOG(1, "Stream number %d not yet used!!!\n", u4StrmNo);
        return FALSE;
    }

    prMuxInfo->arStrm[u4StrmNo].eStrmType = eMUXER_STRMTYPE_MAX;
    prMuxInfo->arStrm[u4StrmNo].fgEnabled = FALSE;
    prMuxInfo->u4ValidStrmNs--;

    // TODO: consider multi-stream
    prMuxInfo->fgEnVideo = FALSE;
    prMuxInfo->fgEnAudio = FALSE;
    prMuxInfo->fgEnTeletext = FALSE;

    return TRUE;
}


BOOL _MUXER_SetScramble(UCHAR ucMxId, BOOL fgScramble)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    prMuxInfo->fgScramble = fgScramble;
    LOG(1, "%s: %s\n", __FUNCTION__, fgScramble? "on" : "off");
    return TRUE;
}


BOOL _MUXER_SetScrambleKey(UCHAR ucMxId, MM_PVR_CRYPT_INFO_T *ptPvrCrypt)
{
#define MX_SET_SCRAMBLE_KEY(pau1Key, u4Key)      \
    (pau1Key)[0] = ((u4Key) & 0xFF000000) >> 24; \
    (pau1Key)[1] = ((u4Key) & 0x00FF0000) >> 16; \
    (pau1Key)[2] = ((u4Key) & 0x0000FF00) >>  8; \
    (pau1Key)[3] = ((u4Key) & 0x000000FF) >>  0;

    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    TSDESC_PARAM_T *prParam = &prMuxInfo->rTsDescParam;

    prParam->eMode = TSDESC_FIRST_PACKET_MODE;
    prParam->eCipher = TSDESC_CIPHER_ECB;
    prParam->eKeyBitLen = KEY_BIT_LEN_128; 
    prParam->fgEncryptByEvenKey = TRUE;

    // must have
    prParam->u4TsState[0] = 0;
    prParam->u4TsState[1] = 0;
    prParam->u4TsState[2] = 0;
    prParam->u4TsState[3] = 0;

    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[0][ 0], ptPvrCrypt->aui4_even_key_1[0]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[0][ 4], ptPvrCrypt->aui4_even_key_1[1]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[0][ 8], ptPvrCrypt->aui4_even_key_2[0]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[0][12], ptPvrCrypt->aui4_even_key_2[1]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[1][ 0], ptPvrCrypt->aui4_odd_key_1[0]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[1][ 4], ptPvrCrypt->aui4_odd_key_1[1]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[1][ 8], ptPvrCrypt->aui4_odd_key_2[0]);
    MX_SET_SCRAMBLE_KEY(&prParam->au1Key[1][12], ptPvrCrypt->aui4_odd_key_2[1]);

    return TRUE;
}


BOOL _MUXER_Query(UCHAR ucMxId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    LOG(0, "id: %d, fmt: %d, state: %d, %d, tick: %d\n",
        ucMxId,
        prMuxInfo->eCurFMT,
        prMuxInfo->eMuxerStatus,
        prMuxInfo->eMuxerPrevousStatus,
        prMuxInfo->u4NfyPeriod);
    LOG(0, "vid: %d, aud: %d, ttx: %d, scramble: %d\n",
        prMuxInfo->fgEnVideo,
        prMuxInfo->fgEnAudio,
        prMuxInfo->fgEnTeletext,
        prMuxInfo->fgScramble);
    LOG(0, "fifo: vir addr: 0x%x, phy addr: 0x%x, size: 0x%x\n",
        prMuxInfo->pucMxBuf,
        PHYSICAL((UINT32)prMuxInfo->pucMxBuf),
        prMuxInfo->u4MxBufSz);
    LOG(0, "Wp: 0x%x, Rp: 0x%x, WpNfy: 0x%x, LastNfyWp: 0x%x\n",
        prMuxInfo->u4MxBufWp,
        prMuxInfo->u4MxBufRp,
        prMuxInfo->u4MxBufWpNfy,
        prMuxInfo->u4LastNfyBufWp);

    return TRUE;
}


VOID vMxPutBits(MUXER_INFO_T *prMuxInfo, UINT32 u4Pattern, UINT32 u4BitLen)
{
    UINT8 u1HalfByte;
    UINT32 u4Mask;
    UINT32 i;

    ASSERT(u4BitLen <= 32);
    ASSERT(u4BitLen % 4 == 0);

    u1HalfByte = 0;
    u4Mask = 0xF0000000 >> (32 - u4BitLen);
    for (i = 0; i < u4BitLen / 4; i++)
    {
        u1HalfByte = (u4Pattern & u4Mask) >> (u4BitLen - 4);
        u4Pattern <<= 4;
        PUT_HALF_BYTE(prMuxInfo, u1HalfByte);
    }
}


VOID vMxCopyBytes(MUXER_INFO_T *prMuxInfo, UINT32 u4StartAddr, UINT32 u4FrameSize)
{
    UCHAR *pucMxBuf = prMuxInfo->pucMxBuf;

    if (0 == u4FrameSize)
    {
        return;
    }

    if (prMuxInfo->u4MxBufWp + u4FrameSize >= prMuxInfo->u4MxBufSz)
    {
        UINT32 u4Size1 = prMuxInfo->u4MxBufSz - prMuxInfo->u4MxBufWp;
        UINT32 u4Size2 = u4FrameSize - u4Size1;

        //HalInvalidateDCacheMultipleLine(u4StartAddr, u4Size1);
        x_memcpy((VOID *)(pucMxBuf + prMuxInfo->u4MxBufWp), (VOID *)u4StartAddr, u4Size1);
        //HalFlushDCacheMultipleLine((UINT32)(pucMxBuf + prMuxInfo->u4MxBufWp), u4Size1);
        if (u4Size2)
        {
            //HalInvalidateDCacheMultipleLine((u4StartAddr + u4Size1), u4Size2);
            x_memcpy((VOID *)pucMxBuf, (VOID *)(u4StartAddr + u4Size1), u4Size2);
            //HalFlushDCacheMultipleLine((UINT32)pucMxBuf, u4Size2);
        }
        prMuxInfo->u4MxBufWp = u4Size2;
    }
    else
    {
        //HalInvalidateDCacheMultipleLine(u4StartAddr, u4FrameSize);
        x_memcpy((VOID *)(pucMxBuf + prMuxInfo->u4MxBufWp), (VOID *)u4StartAddr, u4FrameSize);
        //HalFlushDCacheMultipleLine((UINT32)(pucMxBuf + prMuxInfo->u4MxBufWp), u4FrameSize);
        prMuxInfo->u4MxBufWp += u4FrameSize;
    }
}


BOOL fgMxBufferAvail(MUXER_INFO_T *prMuxInfo, UINT32 u4Len)
{
    BOOL fgRet = TRUE;
    UINT32 u4Space = (prMuxInfo->u4MxBufWp >= prMuxInfo->u4MxBufRp)
        ? (prMuxInfo->u4MxBufSz - (prMuxInfo->u4MxBufWp - prMuxInfo->u4MxBufRp))
        : (prMuxInfo->u4MxBufRp - prMuxInfo->u4MxBufWp);
    if (u4Space < u4Len + 1)
    {
        LOG(1, "%s NG: free=%d, len=%d\n", __FUNCTION__, u4Space, u4Len);
        fgRet = FALSE;
    }

    return fgRet;
}


static VOID MxWaitBufferTimerHdlr(HANDLE_T pt_tm_handle, VOID *pv_tag)
{
    HANDLE_T hMxWaitBufSema = (HANDLE_T)pv_tag;
    VERIFY(x_sema_unlock(hMxWaitBufSema) == OSR_OK);
    UNUSED(pt_tm_handle);
}


BOOL MxWaitBufferAvail(MUXER_INFO_T *prMuxInfo, UINT32 u4Len)
{
    UINT32 u4Wait = 0;
    UINT16 u2QueueSize =0;

    if (fgMxBufferAvail(prMuxInfo, u4Len))
    {
        return TRUE;
    } 

    if (prMuxInfo->pfNotify)
    {
        prMuxInfo->pfNotify(
            prMuxInfo->ucMxId,
            MX_COND_OVERFLOW,
            (VOID *)MX_OVERFLOW_REASON_RING_BUF_FULL);
    }

    do {
        VERIFY(x_timer_start(
            prMuxInfo->hMxWaitBufTimer,
            100/*ms*/,
            X_TIMER_FLAG_ONCE,
            MxWaitBufferTimerHdlr,
            (VOID *)prMuxInfo->hMxWaitBufSema) == OSR_OK);
        VERIFY(x_sema_lock(prMuxInfo->hMxWaitBufSema, X_SEMA_OPTION_WAIT) == OSR_OK);

        if(!_MUXER_GetCmdQSize(prMuxInfo, &u2QueueSize))
        {
            return TRUE;
        }

        u4Wait++;
        LOG(1, "%s: u4Wait=%d\n", __FUNCTION__, u4Wait);
        if (u4Wait > 100)
        {
            LOG(0, "%s: timeout, u4Len=%d\n", __FUNCTION__, u4Len);
            return FALSE;
        }
    } while (!fgMxBufferAvail(prMuxInfo, u4Len));

    return TRUE;
}


//#define MX_CHK_192_SYNC
#ifdef MX_CHK_192_SYNC
VOID MxChk192TsSyncByte(MUXER_INFO_T *prMuxInfo, UINT32 u4Len, UCHAR *pucMxBuf, UINT32 u4MxBufSz)
{
    UCHAR *pucPtr = pucMxBuf + prMuxInfo->u4LastNfyBufWp + 4;
    UCHAR *pucEnd = pucMxBuf + u4MxBufSz;
    UINT32 u4Times = u4Len / 192, i;

    LOG(7, "u4Len = %d\n", u4Len);
    if (u4Len % 192)
    {
        LOG(0, "u4Len = %d is invalid\n", u4Len);
        ASSERT((u4Len % 192) == 0);
    }

    for (i = 0; i < u4Times; i++, pucPtr += 192)
    {
        if (pucPtr >= pucEnd)
        {
            pucPtr -= u4MxBufSz;
        }

        LOG(9, "pucPtr=0x%x\n", pucPtr);

        if (*pucPtr != 0x47)
        {
            LOG(0, "*pucPtr = 0x%02x is invalid\n", *pucPtr);
            ASSERT(*pucPtr == 0x47);
        }
    }
}
#endif


VOID vMxNotifyDataReady(MUXER_INFO_T *prMuxInfo, UINT32 u4Pts)
{
    UINT32 u4Wp = prMuxInfo->u4MxBufWpNfy;
    UINT32 u4WpBak = prMuxInfo->u4LastNfyBufWp;
    MX_CB_DATA_T rCbData;

    if (u4Wp != u4WpBak)
    {
        MxFlushRing(prMuxInfo, u4WpBak, u4Wp);

        if (prMuxInfo->pfNotify)
        {
            UINT32 u4Len;

            if (u4Wp >= u4WpBak)
            {
                u4Len = u4Wp - u4WpBak;
            }
            else
            {
                u4Len = prMuxInfo->u4MxBufSz - (u4WpBak - u4Wp);
            }

            if (MUXER_FMT_MPEG2_TS_192 == prMuxInfo->eCurFMT)
            {
#ifdef MX_CHK_192_SYNC
                MxChk192TsSyncByte(prMuxInfo, u4Len, prMuxInfo->pucMxBuf, prMuxInfo->u4MxBufSz);
#else
                ASSERT((prMuxInfo->pucMxBuf + u4WpBak)[4] ==0x47);
                ASSERT((u4Len & 0x3f) == 0);
#endif
            }
            else if (MUXER_FMT_MPEG2_TS == prMuxInfo->eCurFMT)
            {
                ASSERT((prMuxInfo->pucMxBuf + u4WpBak)[0] ==0x47);
                ASSERT((u4Len & 0x3) == 0);
            }

            LOG(2, "mx_cb(id, buf, len, pts)=(%d, 0x%x, %d, 0x%08x): 0x%02x%02x%02x%02x%02x%02x\n",
                prMuxInfo->ucMxId, u4WpBak, u4Len, u4Pts,
                (prMuxInfo->pucMxBuf + u4WpBak)[0], (prMuxInfo->pucMxBuf + u4WpBak)[1],
                (prMuxInfo->pucMxBuf + u4WpBak)[2], (prMuxInfo->pucMxBuf + u4WpBak)[3],
                (prMuxInfo->pucMxBuf + u4WpBak)[4], (prMuxInfo->pucMxBuf + u4WpBak)[5]);
            rCbData.u4Addr = (UINT32)prMuxInfo->pucMxBuf + u4WpBak;
            rCbData.u4Len = u4Len;
            rCbData.u4Pts = u4Pts;
            rCbData.u4Flag = 0;
            if (prMuxInfo->fgKeyframe)
            {
                LOG(5, "mx_cb: has key\n");
                rCbData.u4Flag |= PVR_TICK_INDEX_FLAG_HAS_I_FRAME;
                prMuxInfo->fgKeyframe = FALSE;
            }
            prMuxInfo->pfNotify(prMuxInfo->ucMxId, MX_COND_DATA, &rCbData);
        }

        prMuxInfo->u4LastNfyBufWp = u4Wp;
    }
    else if (prMuxInfo->u4NfyPeriod)
    {
        // dummy tick data
        rCbData.u4Addr = (UINT32)prMuxInfo->pucMxBuf;
        rCbData.u4Len = 0;
        rCbData.u4Pts = 0;
        rCbData.u4Flag = PVR_TICK_INDEX_FLAG_EMPTY;
        prMuxInfo->pfNotify(prMuxInfo->ucMxId, MX_COND_DATA, &rCbData);
    }
}


VOID _vMxUpdatePts(MUXER_INFO_T *prMuxInfo, UINT32 u4StrmNo, UINT32 u4Pts)
{
    if (MUXER_INVALID_UINT32 == u4Pts)
    {
        u4Pts = 1;
    }
    prMuxInfo->arStrm[u4StrmNo].u4CurPts = u4Pts;
}


VOID _MxStrmSelect(MUXER_INFO_T *prMuxInfo, ENUM_MUXER_STRMTYPE_T *peStrm, UINT32 *pu4StrmNo)
{
    UINT32 u4CandidatePts = 0xFFFFFFFF;
    UINT32 u4CandidateNo = MUXER_MAX_STRM_NS;
    UINT32 i;

    // TODO: consider stc ring
    for (i = 0; i < MUXER_MAX_STRM_NS; i++)
    {
        if (prMuxInfo->arStrm[i].fgEnabled)
        {
            if (prMuxInfo->arStrm[i].u4CurPts < u4CandidatePts
            && _MxStrmAvailable(prMuxInfo->ucMxId, prMuxInfo->arStrm[i].eStrmType))
            {
                u4CandidatePts = prMuxInfo->arStrm[i].u4CurPts;
                u4CandidateNo = i;
            }
        }
    }

    if (u4CandidateNo != MUXER_MAX_STRM_NS)
    {
        *peStrm = prMuxInfo->arStrm[u4CandidateNo].eStrmType;
        *pu4StrmNo = u4CandidateNo;
    }
    else
    {
        *peStrm = eMUXER_STRMTYPE_MAX;
        *pu4StrmNo = MUXER_INVALID_UINT32;
    }
}


VOID _MxVideoTypeAnalyze(MUXER_INFO_T *prMuxInfo, VENC_PES_INFO_T *prVidPesInfo)
{
#ifdef CC_SUPPORT_VENC
    if (ENUM_MUXER_VENC_H264 == prMuxInfo->eVidFmt)
    {
        if (MX_IS_H264_SPS(prVidPesInfo->u4VideoType))
        {
            LOG(5, "%s: is key\n", __FUNCTION__);
            prMuxInfo->fgKeyframe = TRUE;
        }

        if (MX_IS_H264_FRM(prVidPesInfo->u4VideoType))
        {
            LOG(5, "%s: is frm\n", __FUNCTION__);
            prMuxInfo->fgFrame = TRUE;
        }
        else
        {
            LOG(5, "%s: is not frm\n", __FUNCTION__);
            prMuxInfo->fgFrame = FALSE;
        }
    }
    else
    {
        LOG(0, "%s: unknown eVidFmt %d\n", __FUNCTION__, prMuxInfo->eVidFmt);
        ASSERT(0);
    }
#endif
}

BOOL _MUXER_ConfigPid(UCHAR ucMxId, ENUM_MUXER_PID_TYPE_T ePidType, UINT16 u2Pid)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    switch(ePidType)
    {
    case eMUXER_PID_TYPE_VIDEO:
        prMuxInfo->u2VideoPid = u2Pid;
        break;
    
    case eMUXER_PID_TYPE_AUDIO:
        prMuxInfo->u2AudioPid = u2Pid;
        break;

    case eMUXER_PID_TYPE_PCR:
        prMuxInfo->u2PcrPid = u2Pid;
        break;

    default:
        LOG(0, "%s: unknown ePidType %d\n", __FUNCTION__, ePidType);
        break;
    }

    LOG(0, "%s: %d, %d\n", __FUNCTION__, ucMxId, ePidType, u2Pid);

    return TRUE;
}

BOOL _MUXER_AddSection(UCHAR ucMxId, MUXER_ADD_SECTION_INFO_T *ptAddSection)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);
    UINT32 u4Num = 0;

    ASSERT(ptAddSection->i4sectionlen <= MUXER_SECTION_BUF_SIZE);
    if(ptAddSection->pu1sectiondata == NULL)
    {
        ASSERT(0);    
    }

    for(u4Num = 0; u4Num < MUXER_MAX_SECTION_NS; u4Num++)
    {
        if(!(prMuxInfo->arSection[u4Num].fgEnabled))
        {
            prMuxInfo->arSection[u4Num].fgEnabled = TRUE;
            ptAddSection->i4Sectionid = u4Num;
            prMuxInfo->arSection[u4Num].rAddSection.u4Handle = ptAddSection->u4Handle;
            prMuxInfo->arSection[u4Num].rAddSection.u2Pid = ptAddSection->u2Pid;
            prMuxInfo->arSection[u4Num].rAddSection.i4Repetitiontime = ptAddSection->i4Repetitiontime;
            prMuxInfo->arSection[u4Num].rAddSection.i4Sectionid = ptAddSection->i4Sectionid;
            prMuxInfo->arSection[u4Num].rAddSection.i4sectionlen = ptAddSection->i4sectionlen;

            prMuxInfo->arSection[u4Num].rAddSection.pu1sectiondata = 
                (UCHAR *)x_mem_alloc(MUXER_SECTION_BUF_SIZE);
            #if 0    
            if(copy_from_user((void*)prMuxInfo->arSection[u4Num].rAddSection.pu1sectiondata, 
                (void*)ptAddSection->pu1sectiondata, ptAddSection->i4sectionlen));
            {
                LOG(0, "_MUXER_SetSI: copy_from_user() failed\n");
                return FALSE;
            }
            #endif
        }
    }
    if(u4Num == MUXER_MAX_SECTION_NS)
    {
        LOG(0, "_MUXER_AddSection: SECTION is overflow, return!\n");
        return FALSE;
    }
    
    return TRUE;
}

BOOL _MUXER_DelSection(UCHAR ucMxId, INT32 i4SectionId)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    if(!(prMuxInfo->arSection[i4SectionId].fgEnabled))
    {
        LOG(0, "_MUXER_DelSection: i4SectionId is disable, return false!\n");
        return FALSE;
    }
    else
    {
        prMuxInfo->arSection[i4SectionId].fgEnabled = FALSE;
        x_mem_free((VOID*)prMuxInfo->arSection[i4SectionId].rAddSection.pu1sectiondata);
        prMuxInfo->arSection[i4SectionId].rAddSection.pu1sectiondata = NULL;
    }
    
    return TRUE;
}

BOOL _MUXER_UseTrustZone(UCHAR ucMxId, BOOL fgTrustZone)
{
    MUXER_INFO_T *prMuxInfo = _MUXER_QueryInst(ucMxId);

    prMuxInfo->fgTrustZone = fgTrustZone;
    LOG(0, "_MUXER_UseTrustZone: fgTrustZone is %d!\n", (INT32)fgTrustZone);
    return TRUE;
}

