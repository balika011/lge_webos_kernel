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
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND   s  *
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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_if.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_if.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
#include "vdec_debug.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "vdec_flw.h"
#include "./hw_common/vdec_hw_common.h"
#if defined(CC_MT5890)
#include "./lae/vdec_lae.h"
#endif
#include "vdec_esm.h"
#include "vdec_type.h"
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
#include "vdec_h264api.h"
#endif
#endif
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VC1_DISABLE
#include "vdec_wmvapi.h"
#endif
#ifndef CC_VDEC_MPEG4_DISABLE
#include "vdec_mpeg4api.h"
#endif
#ifndef CC_VDEC_MJPEG_DISABLE
#include "vdec_img.h"
#endif
#ifndef CC_VDEC_RV_DISABLE
#include "vdec_rvapi.h"
#endif
#ifndef CC_VDEC_VP6_DISABLE
#include "vdec_vp6api.h"
#endif
#ifndef CC_VDEC_VP8_DISABLE
#include "vdec_vp8api.h"
#endif
#ifndef CC_VDEC_VP9_DISABLE
#include "./vp9/vdec_vp9_api.h"
#endif
#endif

#ifndef CC_VDEC_AVS_DISABLE
#include "vdec_avsapi.h"
#endif

#ifndef CC_VDEC_H265_DISABLE
#include "./h265/vdec_h265_api.h"
#endif

#ifdef ENABLE_MULTIMEDIA
#include "swdmx_if.h"
#endif

#include "vdp_if.h"
#include "drv_dbase.h"
#include "fbm_drvif.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "drvcust_if.h"
#include "x_assert.h"
#include "x_hal_5381.h"
#include "x_hal_5381.h"
#include "x_chip_id.h"
#include "x_mm_common.h"
#include "mpv_type.h"

#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
#include "tz_if.h"
#endif

#ifdef VDEC_DUMP_VFIFO
EXTERN HANDLE_T _hVFifoDataDumpThreadState[VDEC_MAX_ES];
EXTERN INT32 StopVFifoDataDump(UCHAR ucEsId);
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#ifndef REPEAT_FRM
#define REPEAT_FRM 5
#endif
#define DROP_FRM (6)
#define CODEC_CHK_DATASZ (0x3000 * ((UINT32)VDEC_CODEC_CHK_TIMER_THRSD/1000))    // 12kbps
#define VDEC_SCHEDULE_TIMEOUT 5000
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgVDecInitiated = FALSE;
static VDEC_PES_INFO_T _arPesInfo[VDEC_MAX_ES]; // this is for cli
UCHAR _aucVp9LogOnOff[4];

#ifdef VDEC_CRC_TEST
static BOOL _fgCRCTest = FALSE;
#endif

#if BDP_LOG_STYLE
static VDEC_MODELOG_INFO_T _arVDecModeLogInfo[8];
VDEC_LOG_PIC_INFO_T _arVDecLogPicInfo[MPV_MAX_ES];
#endif
extern void _VdecGetThumbnail2(UCHAR ucEsId, UCHAR ucFbId);
extern UCHAR VDP_FrcGetLockFrameBuffer(UINT8 u1B2rId);

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
#ifdef CC_VDEC_PRIORITY_ENABLE
void _VDEC_LockLoadCyclesMutex(UCHAR ucEsId)
{

    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    VERIFY (x_sema_lock(prVdecEsInfoKeep->hVdecSetPriSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}

void _VDEC_UnlockLoadCyclesMutex(UCHAR ucEsId)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    VERIFY (x_sema_unlock(prVdecEsInfoKeep->hVdecSetPriSema) == OSR_OK);
}
#endif

#if 0//!defined(CC_EMULATION) && !defined(CC_USE_DDI)
static VOID _DataTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    UCHAR ucEsId;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    PARAM_MPV_T *prMpvCounter;

    ucEsId = (UCHAR)(UINT32)pv_tag;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prMpvCounter = prVdecEsInfo->prMpvCounter;

    if (ucEsId >= VDEC_MAX_ES)
    {
        return;
    }
	LOG(0,"VDEC super Flush %d,%d,%d\n",prVdecEsInfo->u4NsAtTimeout,prMpvCounter->u4RetrieveNs,prVdecEsInfo->eNotifyStatus);
    if ((prVdecEsInfo->u4NsAtTimeout == prMpvCounter->u4RetrieveNs) &&
        (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_NO_DATA))
    {
#if 0
        //callback, no data receive
        if ((prVdecEsInfoKeep->pfDecNfyFct) &&
            (!prVdecEsInfo->fgMMPlayback))
        {
            _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_FLUSH);

            _arPesInfo[ucEsId].ucEsId = ucEsId;
            _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;
            VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

            LOG(3, "ES(%d) Notify Status change, no data\n", ucEsId);
            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_NO_DATA, 0);

            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_NO_DATA;
        }
#else
		LOG(0,"VDEC super Flush %d\n",ucEsId);
       UNUSED(VDEC_SuperFlush(ucEsId));
#endif
    }

    prVdecEsInfo->u4NsAtTimeout = prMpvCounter->u4RetrieveNs;

    UNUSED(pt_tm_handle);
    UNUSED(prVdecEsInfoKeep);
}
#endif

BOOL VDEC_SuperFlush(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (ucEsId >= VDEC_MAX_ES)
    {
        return FALSE;
    }
    if ((prVdecEsInfo->ucVDecStatus == REAL_DECODE) || (prVdecEsInfo->ucVDecStatus == UNKOWN_DECODE))
    {
        if (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_NO_DATA)
        {
            //callback, no data receive
            if ((prVdecEsInfoKeep->pfDecNfyFct) && (!prVdecEsInfo->fgMMPlayback))
            {
                _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_FLUSH);

                _arPesInfo[ucEsId].ucEsId = ucEsId;
                _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;
                VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

                LOG(3, "%s: ES(%d) Notify Status change, no data\n", __FUNCTION__, ucEsId);
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                    VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_NO_DATA, 0);

                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_NO_DATA;
            }
        }
    }
    return TRUE;
}

#if 0
static VOID _ThumbTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    UCHAR ucEsId;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    PARAM_MPV_T *prMpvCounter;

    ucEsId = (UCHAR)(UINT32)pv_tag;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prMpvCounter = prVdecEsInfo->prMpvCounter;

    if (ucEsId >= VDEC_MAX_ES)
    {
        return;
    }

    if((prVdecEsInfo->u4NsAtThumbTimeout == prMpvCounter->u4RetrieveNs) &&
       (prVdecEsInfo->fgThumbMethod2))
    {
        //callback, no data receive
        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            LOG(3, "Notify thumbnail timeout\n");
            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_THUMBNAIL_FAIL,  0, 0);
        }
        prVdecEsInfo->fgThumbMethod2 = FALSE;
    }

    prVdecEsInfo->u4NsAtThumbTimeout = prMpvCounter->u4RetrieveNs;

    UNUSED(pt_tm_handle);
}

static VOID _CodecChkTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    UCHAR ucEsId;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    PARAM_MPV_T *prMpvCounter;
    BOOL fgErrNotify = TRUE;

    ucEsId = (UCHAR)(UINT32)pv_tag;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prMpvCounter = prVdecEsInfo->prMpvCounter;

    if (ucEsId >= VDEC_MAX_ES)
    {
        return;
    }

    if((prMpvCounter->u4RetrieveNs > 0) && (prMpvCounter->u4DecOkNs == 0))
    {
        LOG(1, "RecvPesInfoCnt %d but can't dec any frm\n", prMpvCounter->u4RetrieveNs);
    }
    else if((prMpvCounter->u4RetrieveNs == 0) && (prVdecEsInfo->u4VFifoDataSz > CODEC_CHK_DATASZ))
    {
        LOG(1, "RecvDataSz 0x%x but can't find start code\n", prVdecEsInfo->u4VFifoDataSz);
    }
    else
    {
        fgErrNotify = FALSE;
        LOG(1, "RecvDataSz 0x%x, PesInfoCnt %d\n",
            prVdecEsInfo->u4VFifoDataSz, prMpvCounter->u4RetrieveNs);
    }

    if(fgErrNotify)
    {
        //callback, no data receive
        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            LOG(3, "ES(%d) Notify Status change, data error\n", ucEsId);
            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_ERROR, 0);
        }
    }
    prVdecEsInfoKeep->fgCodecTypeChecking = FALSE;
    VERIFY(x_timer_stop(prVdecEsInfoKeep->hCodecChkTimer) == OSR_OK);

    UNUSED(pt_tm_handle);
}
BOOL VDEC_GetFrmNumInEsmQ(UCHAR ucEsId, UINT32* pu4FrmNumInQueue)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;

    if (!_fgVDecInitiated)
    {
        return FALSE;
    }

    if(!pu4FrmNumInQueue)
    {
        LOG(3, "VDEC_GetFrmNumInEsmQ Invalid Parameter \n");
        return FALSE;
    }

    *pu4FrmNumInQueue = 0;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if(prVdecEsInfoKeep && prVdecEsInfo)
    {
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            *pu4FrmNumInQueue = prVdecEsInfo->u4FrameNumInEsmQ;
            return TRUE;
        }
    }
    return FALSE;
}
//Synchronous command
void VDEC_Reset(void)
{
    LOG(0,"VDEC_Reset\n");

    _VDEC_InitInfo();
}
void VDEC_PlayVideoClip(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfoKeep->eCurFMT = eFmt;

    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_PLAY_VIDEO_CLIP);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);
}
//Asynchronous command
void VDEC_Flush_DPB(UCHAR ucEsId)
{
    ASSERT(ucEsId < VDEC_MAX_ES);

    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_FLUSH_DPB);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);
}
//Get parameter
BOOL VDEC_IsPlayMM(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    if(ucEsId >= VDEC_MAX_ES)
    {
        ucEsId = ES0;
        LOG(0, "VDEC_IsPlayMM: (ucEsId >= VDEC_MAX_ES)\n");
        ASSERT(0);
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prVdecEsInfo)
    {
        return prVdecEsInfo->fgMMPlayback;
    }
    return FALSE;
}
//Get parameter
BOOL VDEC_IsPlayNotDisplay(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    if(ucEsId >= VDEC_MAX_ES)
    {
        ucEsId = ES0;
        LOG(0, "VDEC_IsPlayNotDisplay: (ucEsId >= VDEC_MAX_ES)\n");
        ASSERT(0);
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prVdecEsInfo)
    {
        return prVdecEsInfo->fgPlaybackNotDisplay;
    }
    return FALSE;
}
#endif
static void _VDPReapeatDropCB(UINT32 u4VdpId, UINT32 u4RepeatDrop, UCHAR ucEsId)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_STATUS_CB_T rStatusCB;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecInfo->pfnStatusCb)
    {
        rStatusCB.u1EsId = prVdecEsInfoKeep->rPesInfo.ucEsId;
        if(u4RepeatDrop == (UINT32)REPEAT_FRM)
        {
            rStatusCB.u4Status = (UINT32)VDEC_FRM_REPEAT;
        }
        else if(u4RepeatDrop == (UINT32)DROP_FRM)
        {
            rStatusCB.u4Status = (UINT32)VDEC_FRM_DROP;
        }
        else
        {
            return;
        }
        prVdecInfo->pfnStatusCb(&rStatusCB);
    }
    UNUSED(u4VdpId);
}


static void _VDEC_SetClock(void)
{
        //vdec clock src selection
#if defined(CC_MT5360) || defined(CC_MT5363)
    UINT32 u4Clk_MC, u4Clk_SYS;

    u4Clk_MC = IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_MC) & 0xFFFFFFF0;
    u4Clk_SYS = IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_SYS) & 0xFFFFFFF0;

#if defined(CC_MT5360)
    if (BSP_GetIcVersion() < IC_VER_5360_AD)
    {
        IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, (u4Clk_MC | VDEC_MC_MC_CLK));
        IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, (u4Clk_SYS | VDEC_MC_SYS_CLK));
    }
    else
#endif
    {
        IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, (u4Clk_MC | VDEC_MC_MC_CLK_ECO_B));
        IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, (u4Clk_SYS | VDEC_MC_SYS_CLK_ECO_B));
    }
#ifdef CC_DYNAMIC_POWER_ONOFF   //power off
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_MC) | VDEC_PWR_OFF);
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_SYS) | VDEC_PWR_OFF);
#endif
#elif defined(CC_MT5387)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, VDEC_MC_SYS_CLK);
#ifdef CC_DYNAMIC_POWER_ONOFF  //power off
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL) | VDEC_PWR_OFF);
#endif
#elif defined(CC_MT5365)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, VDEC_MC_MC_CLK);
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, VDEC_MC_SYS_CLK);
#ifdef CC_DYNAMIC_POWER_ONOFF  //power off
    IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_OFF);
#endif
#elif defined(CC_MT5395)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, VDEC_MC_MC_CLK);
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, VDEC_MC_SYS_CLK);
#ifdef CC_DYNAMIC_POWER_ONOFF  //power off
    IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_OFF);
#endif
#elif defined(CC_MT5396)
    // FIXME
#elif defined(CC_MT5368)
    // FIXME
#elif defined(VDEC_IS_POST_MT5389)
    // FIXME
#else //if defined(CC_MT5391)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, VDEC_MC_SYS_CLK);
#ifdef CC_DYNAMIC_POWER_ONOFF  //power off
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL) | VDEC_PWR_OFF);
#endif
#endif
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

VOID VDEC_GetQueueInfo(UCHAR ucEsId, UINT16* pu2QueueSize,UINT16* pu2MaxQueueSize)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_INFO_T *prVdecInfo;

    if (!_fgVDecInitiated)
    {
        return;
    }

    if((!pu2MaxQueueSize) || (!pu2QueueSize))
    {
        LOG(3, "VDEC_GetQueueInfo Invalid Parameter \n");
        return;
    }

    *pu2QueueSize = 0;
    *pu2MaxQueueSize = 0;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecInfo = _VDEC_GetInfo();
    if(prVdecEsInfoKeep && prVdecInfo)
    {
        *pu2MaxQueueSize = VDEC_MSG_Q_SIZE;
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            *pu2MaxQueueSize = H264_MAX_PES_NS_PER_QUEUE;
        }
        else
        {
            *pu2MaxQueueSize = MPV_MAX_PES_NS_PER_QUEUE;
        }

        VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hMsgQ, pu2QueueSize) == OSR_OK);
    }
}


#ifdef CC_FAST_INIT
void VDEC_pm_suspend(void)
{
}


void VDEC_pm_resume(void)
{
    _VDEC_SetClock();
}
#endif

#ifdef CC_SUPPORT_STR
void VDEC_pm_str_suspend(void)
{
    VDEC_HDR_INFO_T rHdrInfo;
    UINT8 u1VdecId;

    for (u1VdecId = 0;u1VdecId < VDEC_MAX_ES;u1VdecId++)
    {
        // check driver state
        if(VDEC_QueryInfo(u1VdecId,&rHdrInfo))
        {
            if(rHdrInfo.u1CurState != VDEC_ST_STOP)
            {
                LOG(0, "STR_WARNING: VDEC(%d) is not stopped", u1VdecId);
                switch (rHdrInfo.eCodecType)
                {
                    case VDEC_FMT_H264:
                    case VDEC_FMT_H264VER:
                        LOG(0, "--H.264\n");
                        break;
                    case VDEC_FMT_WMV:
                        LOG(0, "--WMV\n");
                        break;
                    case VDEC_FMT_MPV:
                        if (TRUE == rHdrInfo.fgMPEG2)
                        {
                            LOG(0, "--MPEG2\n");
                        }
                        else
                        {
                            LOG(0, "--MPEG1\n");
                        }
                        break;
                    case VDEC_FMT_MP4:
                        LOG(0, "--MPEG4\n");
                        break;
                    case VDEC_FMT_VP8:
                        LOG(0, "--VP8\n");
                        break;
                    case VDEC_FMT_VP6:
                        LOG(0, "--VP6\n");
                        break;
                    case VDEC_FMT_RV:
                        LOG(0, "--RV\n");
                        break;
                    case VDEC_FMT_AVS:
                        LOG(0,"--AVS\n");
                        break;
                    case VDEC_FMT_H265:
                        LOG(0,"--AVS\n");
                        break;
                    default:
                        LOG(0, "--UnKnown Type\n");
                        break;
                }
            }
            //Stop
            VDEC_Stop(u1VdecId);
        }
    }
    //off block clock
    VDEC_CkgenOnOff(FALSE);
}

void VDEC_pm_str_resume(void)
{
    VDEC_CkgenOnOff(TRUE);
}

#endif



void _VDEC_FakeVldIsr(UINT16 u2Vector)
{
    UNUSED(u2Vector);
}

//Synchronous command
void VDEC_Init(void)
{
    HANDLE_T hThread;
    VDEC_INFO_T *prVdecInfo;
    UCHAR ucMpvId, ucEsId;
    CHAR szBuf[16];
    DRV_DBASE_T* _prDbase;
    CRIT_STATE_T _rCritState;

    prVdecInfo = _VDEC_GetInfo();
    _prDbase = NULL;

    if (!_fgVDecInitiated)
    {
        _fgVDecInitiated = TRUE;

        _VDEC_SetClock();

        x_memset((void *)prVdecInfo, 0, sizeof(VDEC_INFO_T));    //init state = VDEC_ST_STOP

        _VDEC_InitInfo();


        #if BDP_LOG_STYLE
        VDec_InitModeLogInfo();
        #endif

        prVdecInfo->fgVDecInitiated = TRUE;
        // create vld Info semephore
        VERIFY (x_sema_create(&prVdecInfo->hVldInfoSemaphore, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);

        // create thread semephore
        for (ucMpvId = 0; ucMpvId < VDEC_MAX_VLD; ucMpvId++)
        {
            VERIFY (x_sema_create(&prVdecInfo->arVdecVldInfo[ucMpvId].hVldSemaphore, X_SEMA_TYPE_MUTEX,
                X_SEMA_STATE_UNLOCK) == OSR_OK);
            prVdecInfo->arVdecVldInfo[ucMpvId].u4PowerOnRef = 0;
            prVdecInfo->arVdecVldInfo[ucMpvId].pfnVdecIsr = _VDEC_FakeVldIsr;
        }

        for (ucEsId = 0; ucEsId < VDEC_MAX_ES; ucEsId++)
        {
            x_snprintf(szBuf, sizeof(szBuf), "VDEC%3d", ucEsId);

            prVdecInfo->arVdecEsInfoKeep[ucEsId].fgWaitIFrame = TRUE;

            prVdecInfo->arVdecEsInfo[ucEsId].eNotifyStatus
                = VDEC_DEC_DECODE_UNKNOWN;
            prVdecInfo->arVdecEsInfo[ucEsId].fgMMSeqInfoValid = FALSE;
            _prDbase = DBS_Lock(&_rCritState);
            ASSERT(_prDbase);
            VERIFY(DBS_Unlock(_prDbase, _rCritState));
            prVdecInfo->arVdecEsInfo[ucEsId].prMpvCounter = &(_prDbase->rMpv[ucEsId]);

            // create thread
            _VDEC_CreateEsmSema(ucEsId);
            VERIFY(x_thread_create(&hThread, szBuf, VDEC_STACK_SIZE,
                VDEC_THREAD_PRIORITY,  _VDEC_MainLoop, sizeof(ucEsId), (void*)&ucEsId) == OSR_OK);
        }

        // Query Op code
        #ifndef CC_VDEC_MPEG2_DISABLE
        if(!DRVCUST_OpCodeQuery(eIsSupportMP2))
        #endif
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_MPEG2;
        }
        #ifndef CC_VDEC_VC1_DISABLE
        if(!DRVCUST_OpCodeQuery(eIsSupportVC1))
        #endif
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_VC1;
        }

        #ifdef CC_VDEC_MPEG4_DISABLE
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_MPEG4;
        }
        #endif
        #ifdef CC_VDEC_H264_DISABLE
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_H264;
        }
        #endif
        #ifdef CC_VDEC_MJPEG_DISABLE
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_MJPEG;
        }
        #endif
        #ifdef CC_VDEC_RV_DISABLE
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_RV;
        }
        #endif
        #ifdef CC_VDEC_VP6_DISABLE
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_VP6;
        }
        #endif
        #ifdef CC_VDEC_VP8_DISABLE
        {
            prVdecInfo->u4OpCode |= VDEC_OP_DISABLE_VP8;
        }
        #endif

#ifdef TEST_ONOFF_RUNTIME
        _VDEC_LockVld(ES0, VLD0);
        #if VLD_LOCK_LOG
        u4VLDLogWr(ES0, VLD0, VLD_LOCK_LOG_IF_BASE+1);
        #endif
#endif
        MPV_Init();
#ifdef VDEC_SUPPORT_ISR
        _VDEC_IsrInit();
#endif
#ifdef TEST_ONOFF_RUNTIME
        #if VLD_LOCK_LOG
        u4VLDLogWr(ES0, VLD0, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_IF_BASE + 1);
        #endif
        _VDEC_UnlockVld(ES0, VLD0);
#endif

#ifdef CC_REALD_3D_SUPPORT
        fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_MM, _VDEC_TDTVModeChangeCB);
#endif
    }
}

//Synchronous command
void VDEC_Termint(void)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    if (prVdecInfo->fgVDecInitiated)
    {
        UCHAR ucEsId;

        for (ucEsId = 0; ucEsId < VDEC_MAX_ES; ucEsId++)
        {
            prVdecInfo->arVdecEsInfoKeep[ucEsId].fgThreadDestroy = TRUE;
        }
    }
}

//Asynchronous command
BOOL VDEC_Play(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt)
{
    UINT32 u4Tmp;
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    #ifdef ENABLE_MULTIMEDIA
    UCHAR ucFRefFbId,ucBRefFbId;
    #endif
    // should assign later
    //prVdecEsInfoKeep->eCurFMT = eFmt;

    ASSERT(ucEsId < VDEC_MAX_ES);

    if(prVdecEsInfoKeep->eCurState==VDEC_ST_STOP)
    {
        MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_PLAY_START);
    }
    else if(prVdecEsInfoKeep->eCurState==VDEC_ST_IDLE)
    {
        MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_RESUME_START);
    }

    if((eFmt != prVdecEsInfoKeep->eCurFMT) && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY))
    {
        return FALSE;
    }

    if((prVdecEsInfo->fgMMPlayback) &&
       (prVdecEsInfo->fgSendedEos) &&
       (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY))
    {
        return FALSE;
    }

//    #ifdef TIME_MEASUREMENT
//    TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "SET_MPV_PLAY");
//    #endif

    prVdecEsInfoKeep->eCurFMT = eFmt;
    if(!prVdecEsInfo->fgMMPlayback && !prVdecEsInfo->fgIPTVPlayback)
    {
        #if (defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)) && !defined(CC_VDEC_H264_DISABLE)
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264) // format will be mpv by default before _VDEC_DecCfg
        {
            VERIFY(STC_SetPtsDrift(prVdecEsInfo->ucStcId, H264_FIFO_CTRL_INIT) == STC_VALID);
        }
        else
        #endif
        {
            VERIFY(STC_SetPtsDrift(prVdecEsInfo->ucStcId, MPV_FIFO_CTRL_INIT) == STC_VALID);
        }
    }

    LOG(2,"VDEC_Play Esid(%d), eFmt(%d)\n",ucEsId, eFmt);

    #ifndef CC_VDEC_RM_SUPPORT
    if(eFmt == VDEC_FMT_MJPEG)
    {
        prVdecEsInfo->u4VldId = VLD_IMG;
    }  
    else
    {
        prVdecEsInfo->u4VldId = VLD0;
    }
    #endif

    #ifdef CC_DUAL_CORE_DEC
    if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT) && (ucVDecConnectLAE(ucEsId) != 0xFF))
    {
        LOG(1,"VDEC[%d] connect LAE[%d] Success\n",ucEsId,prVdecEsInfo->ucLaeEsInfoId);
    }
    #endif
    
    #ifdef CC_VDEC_PRIORITY_ENABLE
    if((prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_NOUSE)||
        (prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PAUSE)
      )
    {
       prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = VDEC_VLD_ACTIVE;
       VDEC_SetPriority(ucEsId,VDEC_PRIORITY_DEFAULT);

       //LOG(3,"VDEC_Play set Es(%d) priority =%d",ucEsId,10);
    }
    #endif
    
    if(eFmt == VDEC_FMT_H265)
    {
        #if defined(ENABLE_MULTIMEDIA)
        #ifndef CC_VDEC_H265_DISABLE
        if(!vVDECIsSupportH265())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_H264)
    {
        #ifndef CC_VDEC_FMT_DETECT
            #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
            #ifndef CC_VDEC_H264_DISABLE
            if(!_VDEC_IsSupportH264())
            #endif
            {
                LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
                return FALSE;
            }
            #else
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
            #endif
        #endif
    }
    else if(eFmt == VDEC_FMT_WMV)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VC1_DISABLE
        if(!_VDEC_IsSupportVc1())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_RV)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_RV_DISABLE
        if(!_VDEC_IsSupportRV())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MP4)
    {
        #ifdef CC_VDEC_MPEG4_DISABLE
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MJPEG)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_MJPEG_DISABLE
        if(!_VDEC_IsSupportMJPEG())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MPV)
    {
        #if defined(CC_MT5387) && !defined(CC_MT5363)
        if((!prVdecEsInfo->fgMMPlayback) && (!IS_SupportDTV()))
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #endif
    }
    else if(eFmt==VDEC_FMT_VP6)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VP6_DISABLE
        if(!_VDEC_IsSupportVP6())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt==VDEC_FMT_VP8)
    {
    #ifdef ENABLE_MULTIMEDIA
    #ifndef CC_VDEC_VP8_DISABLE
        if(!_VDEC_Vp8IsSupport())
    #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
    #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
    #endif
    }
    else if(eFmt==VDEC_FMT_VP9)
    {
    #ifdef ENABLE_MULTIMEDIA
    #ifndef CC_VDEC_VP9_DISABLE
        if(!_VDEC_Vp9IsSupport())
    #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
    #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
    #endif
    }
    else if(eFmt==VDEC_FMT_AVS)
    {
    #ifdef ENABLE_MULTIMEDIA
    #ifndef CC_VDEC_AVS_DISABLE
        if(!_VDEC_IsSupportAVS())
    #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
    #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
    #endif
    }

    else if(eFmt >= VDEC_FMT_MAX)
    {
        LOG(1, "Video codec type %d err!!\n", (UINT32)eFmt);
        return FALSE;
    }

    //DTV00094254 for network case, when buffer underflow--->ready, mw will send vdec_pause--->vdec_play
    // for PS/TS/ES in FR or FF 4x, we use VDEC_PlayOneFrm() but not vdec_play, so we should add VDEC_PlayOneFrm() 's function
    //

    UNUSED(u4Tmp);
#ifdef ENABLE_MULTIMEDIA
    SWDMX_GetInfo(SWDMX_PRIMARY_SOURCE, eSWDMX_CONTAINER_TYPE, &u4Tmp, 0, 0);
    if(((u4Tmp == (UINT32)SWDMX_FMT_H264_VIDEO_ES) ||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG2_VIDEO_ES) ||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG4_VIDEO_ES) ||
       (u4Tmp == (UINT32)SWDMX_FMT_VC1_ES)||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG1_DAT)||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG2_TS)||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG2_PS)||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG2_TS_192)||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG2_TS_ZERO_192)||
       (u4Tmp == (UINT32)SWDMX_FMT_MPEG2_TS_ENCRYPT_192))&&
       (((prVdecEsInfo->u4MMSpeed >= STC_SPEED_TYPE_FORWARD_4X)&&(prVdecEsInfo->u4MMSpeed <= STC_SPEED_TYPE_FORWARD_1024X))||
        (prVdecEsInfo->u4MMSpeed >= STC_SPEED_TYPE_REWIND_1X)))
       {

          LOG(6, "VDEC_PlayOneFrm\n");
          prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
          prVdecEsInfo->fgPlayOneFrm = TRUE;

          //Put Backward Reference pic to EMPTY
          FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
          //prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
          //if(prFbmPicHdr)
          {
              if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_READY))
              {
                 FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_EMPTY);
                 LOG(3, "VDEC_PlayOneFrm ucBRefFbId %d\n", ucBRefFbId);
              }
              if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_READY))
              {
                  FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_EMPTY);
                  LOG(3, "VDEC_PlayOneFrm ucFRefFbId %d\n", ucFRefFbId);
               }
           }

           prVdecEsInfo->fgRefInfo = FALSE;
           prVdecEsInfo->fgKeepPes = FALSE;
        }
       else
#endif
       {
          prVdecEsInfo->fgPlayOneFrm = FALSE;
       }

    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_PLAY);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);
//#if !defined(CC_EMULATION) && !defined(CC_USE_DDI)
 //   if(prVdecEsInfoKeep->hDataTimer && !prVdecEsInfoKeep->fgTimerStarted
 //       && !prVdecEsInfo->fgMMPlayback && !prVdecEsInfoKeep->fgVPush)
 //   {
 //       VERIFY(x_timer_start(prVdecEsInfoKeep->hDataTimer, VDEC_DATA_TIMER_THRSD, X_TIMER_FLAG_REPEAT,
 //           _DataTimeout, (void*)(UINT32)ucEsId) == OSR_OK);
 //       prVdecEsInfoKeep->fgTimerStarted = TRUE;
 //   }
//#endif
    return TRUE;
}


void VDEC_Play_I_Frame(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    prVdecEsInfoKeep->eCurFMT = eFmt;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo->fgPlayOneFrm = FALSE;
    
    if(eFmt == VDEC_FMT_H265)
    {
        #if defined(ENABLE_MULTIMEDIA)
        #ifndef CC_VDEC_H265_DISABLE
        if(!vVDECIsSupportH265())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return ;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return ;
        #endif
    }
    else if(eFmt == VDEC_FMT_H264)
    {
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        if(!_VDEC_IsSupportH264())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return;
        #endif
    }
    else if(eFmt >= VDEC_FMT_MAX)
    {
        LOG(1, "Video codec type %d err!!\n", (UINT32)eFmt);
        return;
    }
    VDEC_SyncStc(ucEsId, VID_SYNC_MODE_NONE, prVdecEsInfo->ucStcId);
    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_PLAY_I_FRAME);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);
}


//Asynchronous command
void VDEC_Stop(UCHAR ucEsId)
{
#if 0
    VDEC_INFO_T* prVdecInfo;
#endif
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        ASSERT(ucEsId < VDEC_MAX_ES);
        ucEsId = (VDEC_MAX_ES-1);
    }

#if 0
    prVdecInfo = _VDEC_GetInfo();
#endif
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

	MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_STOP_START);
    //power off VDEC module
    //move to main loop, need sync with VDEC thread

    if (!prVdecEsInfo->fgIPTVPlayback)
    {
        VERIFY(STC_SetPtsDrift(prVdecEsInfo->ucStcId, MPV_FIFO_CTRL_INIT) == STC_VALID);
    }
#ifdef CC_VDEC_PRIORITY_ENABLE
    // should wakeup  vdec pending thread schedule
    //
    UINT32 i4Ret;
    SIZE_T zMsgSize = sizeof(VDEC_VLD_STATUS_T);
    VDEC_VLD_STATUS_T u1SendMsg;
    u1SendMsg = VDEC_VLD_NOUSE;
    if(prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PENDING)
    {

        i4Ret = x_msg_q_send(prVdecEsInfoKeep->hVdecScheMsg,
                           &u1SendMsg, zMsgSize, 255);
        if ((i4Ret != OSR_OK))
        {
          ASSERT(0);
        }
        LOG(3,"%s  send ScheMsg %d to Es(%d)\n", __FUNCTION__,u1SendMsg,ucEsId);
    }
    else
    {
        prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]= VDEC_VLD_NOUSE;
    }
    VDEC_WakeUpPendingThread(ucEsId);    
#endif

    LOG(3,"VDEC_Stop Esid(%d)\n",ucEsId);

    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_STOP);

#ifndef CC_EMULATION
    FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
    FBM_ReleaseDispQ(prVdecEsInfo->ucFbgId);
    FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
#endif

// for AVS verification
#if 0
    // release 1 frame to empty Q to avoid waiting too long at get_frame_buffer
    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
    {
        if(prVdecEsInfoKeep->eDecStatus == VDEC_DEC_HEADER_PARSE)
        {
            FBM_ReleaseOneFB(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucFbId);
            prVdecEsInfo->ucFbId = FBM_FB_ID_UNKNOWN;
        }
    }
    else
    {
        FBM_ReleaseOneFB(prVdecEsInfo->ucFbgId, FBM_FB_ID_UNKNOWN);
    }
#endif
    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);

    MPV_StopCc(ucEsId);

     // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

#ifdef CC_VDEC_RM_SUPPORT
    _VDEC_RMKickRet(ucEsId, VDEC_RM_BLK_FORCE_CLR);
#endif
    if(prVdecEsInfoKeep->hDataTimer && prVdecEsInfoKeep->fgTimerStarted
        && !prVdecEsInfo->fgMMPlayback)
    {
        VERIFY(x_timer_stop(prVdecEsInfoKeep->hDataTimer) == OSR_OK);
        prVdecEsInfoKeep->fgTimerStarted = FALSE;
    }
#ifdef VDEC_DUMP_VFIFO
    if (_hVFifoDataDumpThreadState[ucEsId] == 1)
    {
        StopVFifoDataDump(ucEsId);
    }
#endif
}

//Asynchronous command
void VDEC_Pause(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
#ifdef CC_VDEC_PRIORITY_ENABLE
    // should wake up  vdec  pending thread schedule
    UINT32 i4Ret;
    SIZE_T zMsgSize = sizeof(VDEC_VLD_STATUS_T);
    VDEC_VLD_STATUS_T u1SendMsg;
    u1SendMsg = VDEC_VLD_PAUSE;
    if(prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PENDING)
    {

        i4Ret = x_msg_q_send(prVdecEsInfoKeep->hVdecScheMsg,&u1SendMsg, zMsgSize, 255);
        if ((i4Ret != OSR_OK))
        {
          ASSERT(0);
        }
        LOG(3,"%s  send ScheMsg %d to Es(%d)\n", __FUNCTION__,u1SendMsg,ucEsId);


    }
    else
    {
        prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = VDEC_VLD_PAUSE;
    }
    VDEC_WakeUpPendingThread(ucEsId);    
#endif

    LOG(3,"VDEC_Pause Esid(%d)\n",ucEsId);

	MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_PAUSE_START);
    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_PAUSE);
    if(!prVdecEsInfo->fgMMPlayback)
    {
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    }

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

    // for time-shift, change from DTV to MM-time-shfit
    if(prVdecEsInfoKeep->hDataTimer && prVdecEsInfoKeep->fgTimerStarted
        && prVdecEsInfo->fgMMPlayback)
    {
        VERIFY(x_timer_stop(prVdecEsInfoKeep->hDataTimer) == OSR_OK);
        prVdecEsInfoKeep->fgTimerStarted = FALSE;
    }
}


void VDEC_SyncStc(UCHAR ucEsId, UCHAR ucMode, UCHAR ucStcId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    ASSERT(ucEsId < VDEC_MAX_ES);
    ASSERT(prVdecEsInfo!=NULL);
    ASSERT(prVdecEsInfoKeep!=NULL);

    LOG(2, "%s(%d, %d, %d)\n", __FUNCTION__, ucEsId, ucMode, ucStcId);

    prVdecEsInfo->ucSyncMode = ucMode;
    prVdecEsInfo->ucStcId = ucStcId;

    // in case swdmx control faster than mpv control, prVdecEsInfo might be clear
    prVdecEsInfoKeep->ucSyncMode = ucMode;
    prVdecEsInfoKeep->ucStcId = ucStcId;

    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_AVSYNC);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);
}


//Synchronous command
void VDEC_RegEventNotifyFunc(PFN_VDEC_EVENT_NOTIFY pfFunc)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    prVdecInfo->pfErrNotify = pfFunc;
}

INT32 VDEC_SetDecNfy(UCHAR ucEsId, const VDEC_DEC_NFY_INFO_T* prDecNfyInfo)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if (prDecNfyInfo == NULL)
    {
        return (MPV_DRV_INV_SET_INFO);
    }

    LOG(2, "%s(%d, %p)\n", __FUNCTION__, ucEsId, prDecNfyInfo->pfDecNfy);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pvDecNfyTag = prDecNfyInfo->pvTag;
    prVdecEsInfoKeep->pfDecNfyFct = prDecNfyInfo->pfDecNfy;

    return (VDEC_DRV_OK);
}

INT32 VDEC_SetFrameToUser(UCHAR ucEsId, BOOL fgFrameToUser)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->fgFrameToUser = fgFrameToUser;

    return (VDEC_DRV_OK);
}

INT32 VDEC_SetVdecNotSupportNfy(UCHAR ucEsId, const PFN_VDEC_NOT_SUPPORT_NFY_FCT prVdecNtyNotSupport)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if (prVdecNtyNotSupport == NULL)
    {
        return (MPV_DRV_INV_SET_INFO);
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pfVdecNotSupporuNfyFct = prVdecNtyNotSupport;

    return (VDEC_DRV_OK);
}

BOOL VDEC_GetGnlInfo(UCHAR ucEsId, VDEC_GNL_INFO_T* prGnlInfo)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    if(prGnlInfo == NULL)
    {
        return FALSE;
    }
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        ASSERT(ucEsId < VDEC_MAX_ES);
        ucEsId = (VDEC_MAX_ES-1);
    }
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(prVdecEsInfo == NULL || prVdecEsInfoKeep == NULL)
    {
        LOG(1, "VDEC_GetGnlInfo prVdecEsInfo null\n");
        return FALSE;
    }

    prGnlInfo->u2UdfNs = prVdecEsInfo->u2UdfNs;

    prGnlInfo->u4ESBufDataSize=prVdecEsInfoKeep->u4CompressedDataSzie;
    prGnlInfo->u4DisplayQNum = FBM_CheckFrameBufferDispQ(prVdecEsInfo->ucFbgId);
    prGnlInfo->u4DecodingPTS = prVdecEsInfo->u4PTS;
    prGnlInfo->u4DisplayPTS = VDP_GetPts(VDP_Es2Vdp(ucEsId));
    LOG(3,"VDEC_GetGnlInfo(%d),fbg=%d,BufSize=0x%x,DispNum=%d\n",\
        ucEsId,prVdecEsInfo->ucFbgId,prGnlInfo->u4ESBufDataSize,prGnlInfo->u4DisplayQNum);
    return TRUE;
}

//Synchronous command
BOOL VDEC_QueryInfo(UCHAR ucEsId, VDEC_HDR_INFO_T* prHdrInfo)
{
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    FBM_PIC_HDR_T *prFbmPicHdr;
    FBM_PIC_HDR_T *prFbmPicHdr0;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    PARAM_MPV_T *prMpvCounter;
    UINT32 u4FbgId=FBM_FBG_ID_UNKNOWN, u4FbId=FBM_FBG_ID_UNKNOWN, u4VideoFmt = 5; //default value
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();

    if(prHdrInfo == NULL)
    {
        return FALSE;
    }

    prHdrInfo->fgHdrInvalid = TRUE;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((prVdecEsInfo == NULL) ||(prVdecEsInfoKeep == NULL)) 
    {
        return FALSE;        
    }

    prMpvCounter = (PARAM_MPV_T*)prVdecEsInfo->prMpvCounter;

    prHdrInfo->eNotifyStatus = prVdecEsInfo->eNotifyStatus;
    prHdrInfo->u4FailCnt = prVdecEsInfo->u4FailCnt;

    if(prMpvCounter != NULL)
    {
        prHdrInfo->u4DropFrameCnt = prMpvCounter->u4EsmSkipNs + prMpvCounter->u4VpsrDropNs;
    }

    prHdrInfo->fgSeqValid = prVdecEsInfo->fgSeqValid;
    prHdrInfo->u2SeqFrmRate = prVdecEsInfo->u2SeqFrmRate;
    prHdrInfo->eCodecType = prVdecEsInfoKeep->eCurFMT;
    prHdrInfo->u1CurState = (UINT8)prVdecEsInfoKeep->eCurState;
    prHdrInfo->u4MaxEsCnt = prVdecEsInfo->u4MaxEsCnt;
    prHdrInfo->u4CurEsCnt = (UINT32)prVdecEsInfo->u2Count;
    prHdrInfo->u4DisplayQPicCnt = prVdecEsInfo->u4DisplayQPicCnt;
    prHdrInfo->u4FirstDispPTS = prVdecEsInfo->u4FirstDispPTS;
    prHdrInfo->fgVPushFBGFromInst = prVdecEsInfoKeep->fgVPushFBGFromInst;
    
    if (!VDEC_IsLock(ucEsId))
    {
        return FALSE;
    }

    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, 0, &u4FbgId, &u4FbId, &u4VideoFmt);
        prVdecEsInfo->ucVideoFmt = (UCHAR)u4VideoFmt;
    }
    else
    {
        u4FbgId = (UINT32)prVdecEsInfo->ucFbgId;
        u4FbId = (UINT32)prVdecEsInfo->ucFbId;
    }

    if (u4FbgId == FBM_FBG_ID_UNKNOWN)
    {
        return FALSE;
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr((UCHAR)u4FbgId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr((UCHAR)u4FbgId, (UCHAR)u4FbId);
    prFbmPicHdr0 = FBM_GetFrameBufferPicHdr((UCHAR)u4FbgId, (UCHAR)0);

    if (prFbmPicHdr==NULL || prFbmPicHdr0==NULL)
    {
       LOG(9, "VDEC_QueryInfo Invalid Fbg[%d] PicHdr[0]=0x%x,PicHdr[%d]=0x%x\n",u4FbgId,prFbmPicHdr0,u4FbId,prFbmPicHdr);
    }

    #ifdef CC_REALD_3D_SUPPORT
    if(prFbmSeqHdr != NULL)
    {
        prHdrInfo->fgB2R3DEnable = prFbmSeqHdr->fgB2R3DEnable;
        prHdrInfo->fgB2RForce2D = prFbmSeqHdr->fgB2RForce2D;
        prHdrInfo->u1B2R3DType = prFbmSeqHdr->u1B2R3DType;
    }
    prHdrInfo->e3DType = prVdecEsInfo->e3DType;
    prHdrInfo->u13DUsrCtrlMode = prVdecEsInfo->u13DUsrCtrlMode;
    #endif
    if (prFbmSeqHdr == NULL)
    {
        x_memset(prHdrInfo, 0, sizeof(VDEC_HDR_INFO_T));
        prHdrInfo->fgHdrInvalid = TRUE;
        return FALSE;
    }
    else
    {
        prHdrInfo->fgResized = FALSE;
        prHdrInfo->fgHdrInvalid = FALSE;
        prHdrInfo->fgCropping = prFbmSeqHdr->fgCropping;
        prHdrInfo->u4CropX = prFbmSeqHdr->u4CropX;
        prHdrInfo->u4CropY = prFbmSeqHdr->u4CropY;
        prHdrInfo->u4CropWidth = prFbmSeqHdr->u4CropWidth;
        prHdrInfo->u4CropHeight = prFbmSeqHdr->u4CropHeight;

        if(prFbmSeqHdr->fgResizeSmallPic)
        {
            if((prFbmPicHdr) && (prFbmPicHdr->u4PicWidth != 0) && (prFbmPicHdr->u4PicHeight != 0))
            {
                prHdrInfo->u2Width = prFbmPicHdr->u4PicWidth;
                prHdrInfo->u2Height = prFbmPicHdr->u4PicHeight;
                prHdrInfo->u2DHS = prFbmPicHdr->u4PicWidth;
                prHdrInfo->u2DVS = prFbmPicHdr->u4PicHeight;
                prHdrInfo->u2OrgWidth = prFbmPicHdr->u4PicWidth;
                prHdrInfo->u2OrgHeight = prFbmPicHdr->u4PicHeight;
            }
            else if((prFbmPicHdr0) && (prFbmPicHdr0->u4PicWidth != 0) && (prFbmPicHdr0->u4PicHeight != 0))
            {
                prHdrInfo->u2Width = prFbmPicHdr0->u4PicWidth;
                prHdrInfo->u2Height = prFbmPicHdr0->u4PicHeight;
                prHdrInfo->u2DHS = prFbmPicHdr0->u4PicWidth;
                prHdrInfo->u2DVS = prFbmPicHdr0->u4PicHeight;
                prHdrInfo->u2OrgWidth = prFbmPicHdr0->u4PicWidth;
                prHdrInfo->u2OrgHeight = prFbmPicHdr0->u4PicHeight;
            }
            else
            {
                prHdrInfo->u2Width = prFbmSeqHdr->u2HSize;
                prHdrInfo->u2Height = prFbmSeqHdr->u2VSize;
                prHdrInfo->u2DHS = prFbmSeqHdr->u2DHS;
                prHdrInfo->u2DVS = prFbmSeqHdr->u2DVS;
                prHdrInfo->u2OrgWidth = prFbmSeqHdr->u2OrgHSize;
                prHdrInfo->u2OrgHeight = prFbmSeqHdr->u2OrgVSize;
            }
        }
        else if (prVdecEsInfo->fgSeamlessPlay)
        {
            // For new seamless case, the display video size is known by resize size.
            prHdrInfo->u2Width = (UINT16)prFbmSeqHdr->u4ResizeWidth;
            prHdrInfo->u2Height = (UINT16)prFbmSeqHdr->u4ResizeHeight;
            prHdrInfo->u2DHS = (UINT16)prFbmSeqHdr->u4ResizeWidth;
            prHdrInfo->u2DVS = (UINT16)prFbmSeqHdr->u4ResizeHeight;
            prHdrInfo->u2OrgWidth = (UINT16)prFbmSeqHdr->u4ResizeWidth;
            prHdrInfo->u2OrgHeight = (UINT16)prFbmSeqHdr->u4ResizeHeight;
            prHdrInfo->u2SourceWidth = (UINT16)prFbmSeqHdr->u2OrgWidth;
            prHdrInfo->u2SourceHeight = (UINT16)prFbmSeqHdr->u2OrgHeight;
            prHdrInfo->eSourceAspectRatio = (VDEC_ASPECT_RATIO_T)prFbmSeqHdr->ucOrgAsp;
            prHdrInfo->u4SourcePixelAspectRatioWidth = prFbmSeqHdr->u4OrgAspWidth;
            prHdrInfo->u4SourcePixelAspectRatioHeight = prFbmSeqHdr->u4OrgAspHeight;
            prHdrInfo->fgResized = TRUE;
        }
        else
        {
            prHdrInfo->u2Width = prFbmSeqHdr->u2HSize;
            prHdrInfo->u2Height = prFbmSeqHdr->u2VSize;
            prHdrInfo->u2DHS = prFbmSeqHdr->u2DHS;
            prHdrInfo->u2DVS = prFbmSeqHdr->u2DVS;
            if(prFbmPicHdr)
            {
                prHdrInfo->u2OrgWidth = prFbmPicHdr->u4PicWidth;
                prHdrInfo->u2OrgHeight = prFbmPicHdr->u4PicHeight;
            }
            else
            {
                prHdrInfo->u2OrgWidth = prFbmSeqHdr->u2OrgHSize;
                prHdrInfo->u2OrgHeight = prFbmSeqHdr->u2OrgVSize;
            }
        }

        prHdrInfo->ucVideoFmt = prVdecEsInfo->ucVideoFmt;
        prHdrInfo->u4BitRate = prFbmSeqHdr->u4BitRatVal;
        prHdrInfo->fgProgressiveSeq = prFbmSeqHdr->fgProgressiveSeq;
        prHdrInfo->ucAfd = prFbmSeqHdr->ucActFmt;
        prHdrInfo->fgNoOverScan = prFbmSeqHdr->fgNoOverScan;

        prHdrInfo->u4PixelAspectRatioWidth = prFbmSeqHdr->u4AspectRatioWidth;
        prHdrInfo->u4PixelAspectRatioHeight = prFbmSeqHdr->u4AspectRatioHeight;
        prHdrInfo->fgPixelAspectRatio = prFbmSeqHdr->fgPixelAspectRatio;

        prHdrInfo->fgContrained = prVdecEsInfo->fgContrained;
        prHdrInfo->fgMPEG2 = prVdecEsInfo->fgMPEG2;
        prHdrInfo->fgSeqDispExt = prVdecEsInfo->fgSeqDispExt;
        prHdrInfo->fgLowDelay = prVdecEsInfo->fgLowDelay;
        prHdrInfo->ucProfileId = prVdecEsInfo->ucProfileId;
        prHdrInfo->u4VbvSizeExt = prVdecEsInfo->u4VbvSizeExt;

        prHdrInfo->ucPicType = prVdecEsInfo->ucPicType;
        prHdrInfo->u4BitDepth = prVdecEsInfo->u4BitDepth;
        //Get sliec QP when HEVC codec.
        prHdrInfo->u4SliceQP = 0;
        if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265) && (prVdecEsInfoKeep->pfnVdecGetParam))
        {
            prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, VDEC_SLICE_QP, &prHdrInfo->u4SliceQP, NULL, NULL);
        }
        if(prFbmPicHdr != NULL)
        {
            prHdrInfo->u2TempRef = (UINT16)prFbmPicHdr->i4TemporalRef;
            prHdrInfo->ucPicStruct = prFbmPicHdr->ucPicStruct;
            prHdrInfo->fgTFF = prFbmPicHdr->fgTopFldFirst;
            prHdrInfo->fgRFF = prFbmPicHdr->fgRepFirstFld;
            prHdrInfo->fgProgressiveFrm = prFbmPicHdr->fgProgressiveFrm;
        }
        prHdrInfo->fgColorDescrip = prFbmSeqHdr->fgColorDescrip;
        prHdrInfo->ucMatrixCoefficients = prFbmSeqHdr->ucMatrixCoeff;
        prHdrInfo->ucTransferCharacteristics = prFbmSeqHdr->ucTransCharacter;
        prHdrInfo->ucColorPrimaries = prFbmSeqHdr->ucColourPrimaries;
        prHdrInfo->u4ChromaFmt = prFbmSeqHdr->ucCrmaFmt;

        switch (prFbmSeqHdr->ucFrmRatCod)
        {
            case MPEG_FRAME_RATE_24_:
                prHdrInfo->u2FrmRate = 2397;
                break;
            case MPEG_FRAME_RATE_24:
                prHdrInfo->u2FrmRate = 2400;
                break;
            case MPEG_FRAME_RATE_25:
                prHdrInfo->u2FrmRate = 2500;
                break;
            case MPEG_FRAME_RATE_30_:
                prHdrInfo->u2FrmRate = 2997;
                break;
            case MPEG_FRAME_RATE_30:
                prHdrInfo->u2FrmRate = 3000;
                break;
            case MPEG_FRAME_RATE_50:
                prHdrInfo->u2FrmRate = 5000;
                break;
            case MPEG_FRAME_RATE_60_:
                prHdrInfo->u2FrmRate = 5994;
                break;
            case MPEG_FRAME_RATE_60:
                prHdrInfo->u2FrmRate = 6000;
                break;
            case MPEG_FRAME_RATE_12:
                prHdrInfo->u2FrmRate = 1200;
                break;
            case MPEG_FRAME_RATE_15:
                prHdrInfo->u2FrmRate = 1500;
                break;
            case MPEG_FRAME_RATE_20:
                prHdrInfo->u2FrmRate = 2000;
                break;
            default:
                prHdrInfo->u2FrmRate = 0;
                break;
        }

        //frame buffer number
        if(prHdrInfo->eCodecType == VDEC_FMT_H264)
        {
            if(prVdecEsInfoKeep->pfnVdecGetParam)
            {
                prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, 1,
                    &prHdrInfo->u4Profile, &prHdrInfo->u4Level, &prHdrInfo->u4IsCABAC);
            }            
            if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfo->fgIPSeamless))//for LG only, to show IP seamless
            {
                prHdrInfo->fgProgressiveSeq = TRUE;
            }
            prHdrInfo->u4H264IdcInfo = prVdecEsInfo->u4H264IdcInfo;
        }
        else if(prHdrInfo->eCodecType == VDEC_FMT_H265)
        {
            if(prVdecEsInfoKeep->pfnVdecGetParam)
            {
                prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, BS_TYPE,
                    &prHdrInfo->u4Profile, &prHdrInfo->u4Level, NULL);
            }   
        }
        else
        {
            prHdrInfo->u4H264IdcInfo = 0;
        }

        // For new seamless display(eg. Netflix), report PAR=1:1, such that DAR = PAR * (resize size).
        if (prVdecEsInfo->fgSeamlessPlay)
        {
            prHdrInfo->eAspectRatio=_VdecDecideAspectRatio(prHdrInfo->eCodecType, MPEG_ASPECT_RATIO_1_1,
                      prFbmSeqHdr->u2OrgHSize, prFbmSeqHdr->u2OrgVSize,prVdecEsInfo->fgMMPlayback);
            prHdrInfo->fgPixelAspectRatio = TRUE;
        }
        else
        {
            prHdrInfo->eAspectRatio=_VdecDecideAspectRatio(prHdrInfo->eCodecType,prFbmSeqHdr->ucAspRatInf,
                      prFbmSeqHdr->u2OrgHSize, prFbmSeqHdr->u2OrgVSize,prVdecEsInfo->fgMMPlayback);
        }

        prHdrInfo->ucFbNum = FBM_GetFrameBufferNs((UCHAR)u4FbgId);
        prHdrInfo->fgDeblocking = prVdecEsInfo->fgDeblocking;
        prHdrInfo->u4Dbk1PixelCnt = prVdecEsInfo->u4Dbk1PixelCnt;
        prHdrInfo->u4Dbk2PixelCnt = prVdecEsInfo->u4Dbk2PixelCnt;
        prHdrInfo->u4Dbk3PixelCnt = prVdecEsInfo->u4Dbk3PixelCnt;
#ifdef CC_REALD_3D_SUPPORT
        prHdrInfo->fgB2rWidthLimitation = prVdecEsInfo->fgB2rResLimitation;
        if(prHdrInfo->fgB2rWidthLimitation)
        {
            prHdrInfo->u2B2rDispWidth = (UINT16)prFbmSeqHdr->u4B2R3DWidthOffset;
        }
#endif
        if(prVdecEsInfo->ucFbgId==FBM_FBG_ID_UNKNOWN)
        {
            prHdrInfo->fgHdrInvalid = TRUE;
            LOG(0,"VDEC_QueryInfo FBG Change to invalid\n");
            return FALSE;
        }

        prHdrInfo->eProfile = prVdecEsInfo->eProfile;
        prHdrInfo->eLevel = prVdecEsInfo->eLevel;
#ifdef VDEC_TIME_PROFILE
        x_memcpy((void*)&prHdrInfo->au4DecTime[0], (void*)&prVdecEsInfo->au4DecTime[0], sizeof(prVdecEsInfo->au4DecTime));
#endif
    }
    return TRUE;
}


BOOL VDEC_QueryChromaInfo(UCHAR ucEsId, VDEC_CHROMA_SIMPLE_INFO_T* prChromaSimpleInfo)
{
	VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	
	if(prChromaSimpleInfo == NULL)
    {
        return FALSE;
    }

	if (1 != prVdecEsInfoKeep->u4ChromaFormatIdc)
	{
		return FALSE;
	}
	else
	{
		prChromaSimpleInfo->u4ChromaFormatIdc = prVdecEsInfoKeep->u4ChromaFormatIdc;
		prChromaSimpleInfo->fgChromaLocationInfoPresentFlag = prVdecEsInfoKeep->fgChromaLocationInfoPresentFlag;
		if (prVdecEsInfoKeep->fgChromaLocationInfoPresentFlag)
		{
			prChromaSimpleInfo->u4ChromaSampleLocTypeTopField = prVdecEsInfoKeep->u4ChromaSampleLocTypeTopField;
			prChromaSimpleInfo->u4ChromaSampleLocTypeBottomField = prVdecEsInfoKeep->u4ChromaSampleLocTypeBottomField;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL VDEC_QueryWFDErrInfo(UCHAR ucEsId, VDEC_WFD_DECODE_ERROR_T* prErrInfo)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prErrInfo == NULL)
    {
        return FALSE;
    }

    prErrInfo->i4LackSeqHeaderCnt = prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt;
    prErrInfo->i4OutOfSpecCnt = prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt;
    prErrInfo->i4TimeoutCnt = prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt;
    
    LOG(2, "LackSeqHeaderCnt %d, OutOfSpecCnt %d, TimeoutCnt %d\n", 
        prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt,
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt,
        prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt);
    return TRUE;
}

void VDEC_ResetWFDInfo(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    LOG(3,"VDEC_ResetWFDInfo Esid(%d)\n",ucEsId);

    x_memset((void*)&prVdecEsInfo->rWFDDecodeErr, 0, sizeof(VDEC_WFD_DECODE_ERROR_T));
}

BOOL VDEC_GetDelayTime(UCHAR ucEsId, VDEC_HDR_INFO_T* prHdrInfo)
{  
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    if(prHdrInfo == NULL)
    {
        return FALSE;
    }
    if (VDEC_FMT_H264 == prVdecEsInfoKeep->eCurFMT)
    {
        prHdrInfo->u4DelayTime = H264_FIFO_CTRL_INIT;
    }
    else if (VDEC_FMT_MPV == prVdecEsInfoKeep->eCurFMT)
    {
        prHdrInfo->u4DelayTime = MPV_FIFO_CTRL_INIT;
    }
    else
    {
        prHdrInfo->u4DelayTime = 9000;
    }

    return TRUE;
}


//Set parameter
void VDEC_SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;

    if (ucEsId >= VDEC_MAX_ES)
    {
        LOG(5, "Id error\n");
        return ;
    }
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo  = _VDEC_GetEsInfo(ucEsId);
    
    if (VDEC_COM_PARAM_LOG_CTRL == u4Type)
    {
        prVdecEsInfoKeep->u4LOGCtrl = u4Param1;
    }

#if defined(VDEC_CRC_TEST) && !defined(CC_STRESS_TEST_CRC_CHK)
    /**RECORD_CRC**/
    if(u4Type == 16)
    {
        VDEC_SetTestCRC(ucEsId, (BOOL)u4Param1);
        if((BOOL)u4Param1)
        {
            LOG(0, "the crc record is enable!!\n");
        }
        else
        {
            LOG(0, "the crc record is disable!!\n");
        }
        return;
    }
#endif

    if(u4Type == 17)    // for IPTV
    {
        if (prVdecEsInfo)
        {
            prVdecEsInfo->fgIPTVPlayback = (BOOL)u4Param1;
            LOG(0, "Set IPTV playback = %d\n", prVdecEsInfo->fgIPTVPlayback);
        }
        return;
    }

#ifndef VDEC_TIME_PROFILE
    if(u4Type == 11)    //vdec_h264api.h
    {
        x_memset((void*)(prVdecEsInfo->au4DecTime), 0, 5*9*sizeof(UINT32));
    }
    else
#endif
    {
        if(prVdecEsInfoKeep->pfnVdecSetParam)
        {
            prVdecEsInfoKeep->pfnVdecSetParam(ucEsId , u4Type, u4Param1, u4Param2, u4Param3);
        }
    }
}

//Get parameter
void VDEC_GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;
    
    if (ucEsId >= VDEC_MAX_ES)
    {
        LOG(5, "Id error\n");
        return ;
    }
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo  = _VDEC_GetEsInfo(ucEsId);
    
    if (VDEC_COM_PARAM_LOG_CTRL == u4Type)
    {
        if (pu4Param1)
        {
            *pu4Param1 = prVdecEsInfoKeep->u4LOGCtrl;
        }
    }

    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, u4Type, pu4Param1, pu4Param2, pu4Param3);
    }

    UNUSED(prVdecEsInfo);
    
}

#ifdef CC_MAPLE_CUST_DRV
//Get bitrate
void VDEC_GetDataSizePerSecond(UCHAR ucEsId, UINT32* pu4Param1)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;
    UINT32 u4DataSize = 0;
    UINT32 i;
    if (ucEsId >= VDEC_MAX_ES)
    {
        LOG(5, "Id error\n");
        return ;
    }
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo  = _VDEC_GetEsInfo(ucEsId);

    for(i=0;i <= prVdecEsInfo->rBitRateInfo.u4FrameRate; i++)
    {
		u4DataSize += prVdecEsInfo->rBitRateInfo.u4PicSize[i];
    }

	u4DataSize += prVdecEsInfo->rBitRateInfo.u4NonePicSize;
	*pu4Param1 = u4DataSize;
}
#endif
BOOL VDEC_SetRptr(UCHAR ucEsId, UINT32 u4Rp,
    UINT32 u4FrameAddr)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep((UCHAR)ucEsId);
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo((UCHAR)ucEsId);
	//Get avariable compressed data in VFIFO
	if(prVdecEsInfo->u4VFifoPrevWptr >= u4Rp)
	{
		prVdecEsInfoKeep->u4CompressedDataSzie = prVdecEsInfo->u4VFifoPrevWptr - u4Rp;
	}
	else
	{
		prVdecEsInfoKeep->u4CompressedDataSzie =
			(prVdecEsInfo->u4VFifoPrevWptr - prVdecEsInfo->u4FifoStart)
			+ (prVdecEsInfo->u4FifoEnd - u4Rp);
	}
	
	//LOG(9,"VDEC_SetRptr %x %x %x %x %x\n",prVdecEsInfoKeep->u4CompressedDataSzie,u4Rp,prVdecEsInfo->u4VFifoPrevWptr,
	//		prVdecEsInfo->u4FifoEnd,prVdecEsInfo->u4FifoStart);
    if(prVdecEsInfoKeep->fgVPush)
    {
        return _VPUSH_SetRptr(ucEsId, u4Rp, u4FrameAddr);
    }
    return DMX_UpdateVideoReadPointer(ucEsId, u4Rp, u4FrameAddr);
}
//Set parameter
void VDEC_SetWptr(UINT8 u1VdecId, UINT32 u4Wptr)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo((UCHAR)u1VdecId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep((UCHAR)u1VdecId);

#if 0
    FBM_POOL_T* prFbmPool;

    // get video fifo size from FBM
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);

    if((prVdecEsInfo->u4VFifoPrevWptr > 0) && (prVdecEsInfo->u4VFifoDataSz <= CODEC_CHK_DATASZ))
    {
        if(u4Wptr > prVdecEsInfo->u4VFifoPrevWptr)
        {
            prVdecEsInfo->u4VFifoDataSz += (u4Wptr - prVdecEsInfo->u4VFifoPrevWptr);
        }
        else
        {
            prVdecEsInfo->u4VFifoDataSz +=
                ((u4Wptr + prFbmPool->u4Size) - prVdecEsInfo->u4VFifoPrevWptr);
        }
    }
#endif

    prVdecEsInfo->u4VFifoPrevWptr = u4Wptr;

    if(prVdecEsInfoKeep->pfnVdecSetWptr)
    {
        prVdecEsInfoKeep->pfnVdecSetWptr(u1VdecId, u4Wptr);
    }
}


// 0928 special case for H264 trick mode IN PVR
void VDEC_SetH264PVR(UCHAR ucEsId, UINT32 u4PVRMode)
{
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
     VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
     if(prVdecEsInfoKeep->pfnVdecSetParam && prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
     {
        prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, MM_PLAY_DTV, u4PVRMode, 0, 0);
     }
#endif
#endif
}


void VDEC_SetH264MMErrMask(UCHAR ucEsId, BOOL fgMMErrMask)
{
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
     VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

     if(prVdecEsInfoKeep->pfnVdecSetParam && prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
     {
        prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, USE_MM_ERR_MASK, fgMMErrMask, 0, 0);
     }

#endif
#endif
}


BOOL VDEC_IsLock(UCHAR ucEsId)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    UINT32 u4FbgId, u4FbId;
    FBM_SEQ_HDR_T *prFbmSeqHdr;

    if(ucEsId >= VDEC_MAX_ES)
    {
        return FALSE;
    }

    if((prVdecInfo == NULL) || (prVdecEsInfo == NULL))
    {
        return FALSE;
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    /* MPV_IsLock is designed to check if mpv driver receive any video frame
       in a period. Original design is used for no signal check.
       Now we have decode status notify. If customer need that, we could give a
       notification to them.
    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
    {
        return MPV_IsLock(ucEsId);
    }
    else
    */
    {
        if(prVdecEsInfoKeep->pfnVdecGetParam)
        {
            prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, 0, &u4FbgId, &u4FbId, &u4FbId);
        }
        else
        {
            u4FbgId = (UINT32)prVdecEsInfo->ucFbgId;
        }

        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr((UCHAR)u4FbgId);
        if(!prFbmSeqHdr)
        {
            return FALSE;
        }
        if((prFbmSeqHdr->u2HSize > 0) &&
            (prFbmSeqHdr->u2VSize > 0))
        {
            return TRUE;
        }
    }

    // ToDo: add other format
    return FALSE;
}


//Set parameter
void VDEC_PlayMM(UCHAR ucEsId, BOOL fgPlayMM)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    if(ucEsId >= VDEC_MAX_ES)
    {
        ucEsId = ES0;
        LOG(0, "VDEC_PlayMM: (ucEsId >= VDEC_MAX_ES)\n");
        ASSERT(0);
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prVdecEsInfo)
    {
        prVdecEsInfo->fgMMPlayback = fgPlayMM;
    }
}

void VDEC_RePlayVdec(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if(ucEsId >= VDEC_MAX_ES)
    {
        ucEsId = ES0;
        LOG(0, "VDEC_RePlayVdec: (ucEsId >= VDEC_MAX_ES)\n");
        ASSERT(0);
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(prVdecEsInfoKeep)
    {
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            VDEC_SetRenderFromPos(ucEsId, TRUE,(UINT64)0, (INT32)0, (UINT16)0);
            if(prVdecEsInfo)
            {
                VDEC_SetWaitPvrSps(ucEsId, (UINT8)1);
            }
        }
        else
        {
            LOG(0, "VDEC_RePlayVdec: the codec(%d) not implement the function \n",prVdecEsInfoKeep->eCurFMT);
        }

    }
    else
    {
        LOG(0, "VDEC_RePlayVdec: (prVdecEsInfoKeep == NULL)\n");
    }

}


//Set parameter
void VDEC_PlayNotDisplay(UCHAR ucEsId, BOOL fgNotDisplay)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    if(ucEsId >= VDEC_MAX_ES)
    {
        ucEsId = ES0;
        LOG(0, "VDEC_PlayNotDisplay: (ucEsId >= VDEC_MAX_ES)\n");
        ASSERT(0);
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prVdecEsInfo)
    {
        prVdecEsInfo->fgPlaybackNotDisplay = fgNotDisplay;
    }
}
#ifdef ENABLE_MULTIMEDIA
#ifdef USB_CLI_FORCE_SETTING_SUPPORT //Cli force usb file seamless for debug
UINT32 u4ForceSrc = (UINT32)SWDMX_SRC_TYPE_UNKNOWN; //For Cli Force set..
#endif
#endif
void VDEC_SetMMParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    UCHAR ucSkipMode;

    LOG(4,"ucEsId (%d) u4Type (%d) u4Param[%d,%d,%d] \n",ucEsId,u4Type,u4Param1,u4Param2,u4Param3);
    if(u4Type == (UINT32)VDEC_MM_PLAY_SPEED)
    {
        if(prVdecEsInfo->u4MMSpeed != u4Param1)
        {
            prVdecEsInfo->fgSpeedChg = TRUE;
        }
        prVdecEsInfo->u4MMSpeed = u4Param1;
        #ifdef CC_DUAL_CORE_DEC
        if(prVdecEsInfo->ucDualCoreMode)
        {
            VDEC_ES_INFO_T *prVdecLaeEsInfo = _VDEC_GetEsInfo(prVdecEsInfo->ucLaeEsInfoId);
            prVdecLaeEsInfo->u4MMSpeed = u4Param1;
        }
        #endif
        LOG(6, "ES(%d) VDEC_MM_PLAY_SPEED(%d)\n", ucEsId, prVdecEsInfo->u4MMSpeed);

        switch (prVdecEsInfo->u4MMSpeed)
        {
        case STC_SPEED_TYPE_FORWARD_2X:
#ifdef VDEC_TIME_SHIFT_2X_I_MODE
            if(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
            {
                ucSkipMode = IMode;
            }
            else
#endif
            {
                ucSkipMode = IPMode;
            }
            break;
        case STC_SPEED_TYPE_FORWARD_3X:
        case STC_SPEED_TYPE_FORWARD_4X:
        case STC_SPEED_TYPE_FORWARD_8X:
        case STC_SPEED_TYPE_FORWARD_16X:
        case STC_SPEED_TYPE_FORWARD_32X:
        case STC_SPEED_TYPE_FORWARD_64X:
        case STC_SPEED_TYPE_FORWARD_128X:
        case STC_SPEED_TYPE_FORWARD_256X:
        case STC_SPEED_TYPE_FORWARD_512X:
        case STC_SPEED_TYPE_FORWARD_1024X:
        case STC_SPEED_TYPE_REWIND_1X:
        case STC_SPEED_TYPE_REWIND_2X:
        case STC_SPEED_TYPE_REWIND_3X:
        case STC_SPEED_TYPE_REWIND_4X:
        case STC_SPEED_TYPE_REWIND_8X:
        case STC_SPEED_TYPE_REWIND_16X:
        case STC_SPEED_TYPE_REWIND_32X:
        case STC_SPEED_TYPE_REWIND_64X:
        case STC_SPEED_TYPE_REWIND_128X:
        case STC_SPEED_TYPE_REWIND_256X:
        case STC_SPEED_TYPE_REWIND_512X:
        case STC_SPEED_TYPE_REWIND_1024X:
            ucSkipMode = IMode;
            break;
        default:
            ucSkipMode = IPBMode;
            break;
        }

        if(prVdecEsInfo->e3DType == VDEC_3D_MVC)
        {          
            if((ucSkipMode == IPBMode)&&(prVdecEsInfo->u4MMSpeed != STC_SPEED_TYPE_FORWARD_1X))
            {
                if(prVdecEsInfo->ucSubFbgId != FBM_FBG_ID_UNKNOWN)
                {
                    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucSubFbgId);
                    if(prFbmSeqHdr)
                    {
                        prFbmSeqHdr->u1B2R3DTrickModeType = B2R_3D_TRICK_MODE_3D_VIEW;
                    }
                    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                    if(prFbmSeqHdr)
                    {
                        prFbmSeqHdr->u1B2R3DTrickModeType = B2R_3D_TRICK_MODE_3D_VIEW;
                    }
                    FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
                }
            }
            else if(ucSkipMode != IPBMode)
            {
                if(prVdecEsInfo->ucSubFbgId != FBM_FBG_ID_UNKNOWN)
                {
                    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                    if(prFbmSeqHdr)
                    {
                        prFbmSeqHdr->u1B2R3DTrickModeType = B2R_3D_TRICK_MODE_L_VIEW;
                    }
                    FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
                }
            }
        }
        else if(prVdecEsInfo->e3DType == VDEC_3D_DUAL_STREAM)
        {
            if((ucSkipMode == IPBMode)&&(prVdecEsInfo->u4MMSpeed != STC_SPEED_TYPE_FORWARD_1X))
            {
                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                if(prFbmSeqHdr)
                {
                    LOG(2, "ES(%d) B2R_3D_TRICK_MODE_3D_VIEW_2(%d)\n", ucEsId, prVdecEsInfo->u4MMSpeed);   
                    prFbmSeqHdr->u1B2R3DTrickModeType = B2R_3D_TRICK_MODE_3D_VIEW;
                }
                FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
            }            
        }
    }
    else if(u4Type==VDEC_MM_STREAM_TYPE)
    {
       prVdecEsInfo->eStreamType=(VDEC_STREAM_TYPE_T)u4Param1;
    }
    else if(u4Type == (UINT32)VDEC_MM_SEQ_INFO)
    {
        x_memcpy((void*)&prVdecEsInfo->rMMSeqInfo, (void*)u4Param1, sizeof(VDEC_SEQUENCE_DATA_T));
        prVdecEsInfo->fgMMSeqInfoValid = TRUE;
    }
    else if(u4Type == (UINT32)VDEC_MM_TPT_SEQ_INFO)
    {
        x_memcpy((void*)&prVdecEsInfo->rMMTPTSeqInfo, (void*)u4Param1, sizeof(VDEC_SEQUENCE_DATA_T));
        prVdecEsInfo->fgMMTPTSeqInfoValid = TRUE;
    }
    else if(u4Type == (UINT32)VDEC_MM_FRAME_RATE)
    {
        prVdecEsInfo->rMMSeqInfo.e_frame_rate = (ENUM_VDEC_FRAME_RATE_T)(u4Param1);
    }
#ifdef ENABLE_MULTIMEDIA
    else if(u4Type == (UINT32)VDEC_MM_CONTAINER_TYPE)
    {
        prVdecEsInfo->eContainerType = (ENUM_SWDMX_FMT_T)(u4Param1);
    }
    else if(u4Type == (UINT32)VDEC_MM_SRC_TYPE)
    {
        #ifdef USB_CLI_FORCE_SETTING_SUPPORT //Cli force usb file seamless for debug
        if (u4Param3  == 0xCF00) /*Cli Force*/
        {
            u4ForceSrc = (u4Param1 & 0xFF);
        }
        else
        #endif
        {
            prVdecEsInfo->eMMSrcType = (SWDMX_SOURCE_TYPE_T)(u4Param1);
        }
    }
#ifdef CC_B2R_SENSIO_CONV
    else if(u4Type == (UINT32)VDEC_MM_SENSIO_FORMAT)// sensio format
    {
        LOG(1,"------->>>>MW set sensio format to driver\n");
        prVdecEsInfo->fgSensio3D = TRUE;
        prVdecEsInfo->e3DType = VDEC_3D_SBS_LF;
        prVdecEsInfo->u13DUsrCtrlMode = VDEC_3D_CTRL_FORCE_3D_LR;
    }
#endif
#ifndef CC_VDEC_RV_DISABLE
    else if (u4Type == (UINT32)VDEC_MM_RV_RPR_INFO)
    {
        UINT32 au4Size[9] = {0,1,1,2,2,3,3,3,3};
        INT32 i;
        ASSERT(u4Param1 < 9);

        prVdecEsInfo->u4SzOfPicSz = au4Size[u4Param1];
        for (i = 0; i < 2 * u4Param1; i++)
        {
            prVdecEsInfo->au4RPRSizes[i] = *(UINT32 *)u4Param2;
            u4Param2 += 4;
        }
        prVdecEsInfo->fgRV9 = (BOOL)u4Param3;
    }
#endif
#endif
    else if(u4Type == (UINT32)VDEC_MM_3D_INFO)
    {
		LOG(2,"Set VDEC_MM_3D_INFO %d\n",u4Param1);
        if(u4Param1 < VDEC_3D_MAX)
        {
            prVdecEsInfo->e3DType = (VDEC_3D_TYPE_T)(u4Param1);
        }
        else
        {
            LOG(2, "Invalid VDEC_3D_TYPE %d", u4Param1);
        }
        if(u4Param2 < VDEC_3D_RESOLUTION_MAX)
        {
            prVdecEsInfo->e3DResType = (VDEC_3D_RESOLUTION_TYPE_T)(u4Param2);
        }
        else
        {
            LOG(2, "Invalid VDEC_3D_RESOLUTION_TYPE %d", u4Param2);
        }
    }
#ifdef CC_3D_MM_DS_SUPPORT
    else if(u4Type == (UINT32)VDEC_MM_3D_TRICK_TYPE)
    {
         prVdecEsInfo->u13DTrickModeType = (UINT8)u4Param1;
    }
#endif
    else if (u4Type == (UINT32)VDEC_MM_DIVX4_SIGNAL)
    {
          prVdecEsInfo->fgSetDivX4 = TRUE;
    }
    else if(u4Type == (UINT32)VDEC_MM_PARSING_INFO)
    {
        prVdecEsInfo->fgParsingInfo = (u4Param1 > 0) ? TRUE : FALSE;
    }
    else if(u4Type == (UINT32)VDEC_MM_SWDMX_SEND_EOS_DONE)
    {
        LOG(0,"[VDEC-SET %d] Swdmx send eos done\n",ucEsId);
        prVdecEsInfo->fgSwdmxSendEosDone = (u4Param1 > 0) ? TRUE : FALSE;
    }
    else if(u4Type == VDEC_MM_SWDMX_SEND_CHG)
    {
        prVdecEsInfo->fgCodecChange = (u4Param1 > 0) ? TRUE : FALSE;
        prVdecEsInfo->eNextFmt = (ENUM_VDEC_FMT_T)u4Param2;
        LOG(0,"[VDEC-SET %d] Swdmx msg vdec codec change %d\n",ucEsId,u4Param2);
    }
    else if (u4Type == VDEC_MM_SWDMX_SEND_NO_FLUSH)
    {
        prVdecEsInfo->fgMpvNoFlush = u4Param1;
        LOG(0,"[VDEC-SET %d] not mpv flush esQ %d\n",ucEsId,u4Param1);
    }
    else if(u4Type == (UINT32)VDEC_MM_XVID_SIGNAL)
    {
        prVdecEsInfo->fgXVID = TRUE;
    }    
    else if(u4Type==(UINT32)VDEC_MM_PVR_MODE)
    {
        prVdecEsInfo->fgPvrPlayBack = (BOOL)u4Param1;
        prVdecEsInfo->u1PvrPlayMode = (UINT8)u4Param2;
        LOG(2, "%s(%d) PvrPlayBack(%d) PlayMode(%d)\n", __FUNCTION__, ucEsId,
                prVdecEsInfo->fgPvrPlayBack, prVdecEsInfo->u1PvrPlayMode);
    }
    else if ((UINT32)VDEC_MM_SET_SEAMLESS_TARGET == u4Type)
    {
        prVdecEsInfo->u4SeamlessWidth = u4Param1;
        prVdecEsInfo->u4SeamlessHeight = u4Param2;		
        if (prVdecEsInfo->u4SeamlessWidth > 1920 || prVdecEsInfo->u4SeamlessHeight > 1080)
        {
            LOG(0, "Warning!!!!target w/h error");
            prVdecEsInfo->u4SeamlessWidth = 1920;
            prVdecEsInfo->u4SeamlessHeight = 1080;
        }
        LOG(0, "[VDEC-SET %d] Seamless target[%d x %d]\n", ucEsId, u4Param1, u4Param2);
    }
    else if((UINT32)VDEC_MM_UFO_MODE_ENABLE == u4Type)
    {
        prVdecEsInfo->fgEnableUFO = (BOOL)u4Param1;
    }
    else if((UINT32)VDEC_MM_HAL_PARAMETER == u4Type)
	{
		switch((HAL_VDEC_PARAM_T)u4Param1)
		{
			case HAL_VDEC_PARAM_3DMODE:
				prVdecEsInfo->i4Hal3DMode = u4Param2;
				break;
			case HAL_VDEC_PARAM_NOT_USE_STC:
				prVdecEsInfo->i4HalNotUseSTC = u4Param2;
				break;
			case HAL_VDEC_PARAM_AUDIO_CHANNEL:
				prVdecEsInfo->i4AudioChannel = u4Param2;
				break;
			case HAL_VDEC_PARAM_DISPLAY_DELAY:
				prVdecEsInfo->i4HalDisplayDelay = u4Param2;
				break;
			case HAL_VDEC_PARAM_LIPSYNC_MASTER:
				prVdecEsInfo->i4HalLipsyncMaster = u4Param2;
				break;
			default:
				break;
		}
	}
	else
    {
        LOG(3, "VDEC_SetMMParam: unknown type: %d\n", u4Type);
    }

    UNUSED(u4Param2);
    UNUSED(u4Param3);
}

void VDEC_GetHALParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32* u4Param2, UINT32 u4Param3)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	if((UINT32)VDEC_MM_HAL_PARAMETER == u4Type)
	{
		switch((HAL_VDEC_PARAM_T)u4Param1)
		{
			case HAL_VDEC_PARAM_3DMODE:
				*u4Param2 = prVdecEsInfo->i4Hal3DMode;
				break;
			case HAL_VDEC_PARAM_NOT_USE_STC:
				*u4Param2 = prVdecEsInfo->i4HalNotUseSTC;
				break;
			case HAL_VDEC_PARAM_AUDIO_CHANNEL:
				*u4Param2 = prVdecEsInfo->i4AudioChannel;
				break;
			case HAL_VDEC_PARAM_DISPLAY_DELAY:
				*u4Param2 = prVdecEsInfo->i4HalDisplayDelay;
				break;
			case HAL_VDEC_PARAM_LIPSYNC_MASTER:
				*u4Param2 = prVdecEsInfo->i4HalLipsyncMaster;
				break;
			default:
				break;
		}
	}
}



//Set parameter
void VDEC_SetBigBParameter(UCHAR ucEsId, BOOL fgForceEnable, UINT32 u4BigBLevel1,
    UINT32 u4BigBLevel2, UINT32 u4BigBLevel3, UINT32 u4BigBBaseCnt)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    prVdecEsInfo->fgBigBForceEnable = fgForceEnable;
    prVdecEsInfo->u4BigBLevel1 = u4BigBLevel1;
    prVdecEsInfo->u4BigBLevel2 = u4BigBLevel2;
    prVdecEsInfo->u4BigBLevel3 = u4BigBLevel3;
    prVdecEsInfo->u4BigBBaseCnt = u4BigBBaseCnt;
}
#if 0
//Set parameter
void VDEC_WaitSyncPoint(UCHAR ucEsId, BOOL fgTricking)
{
    //VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    #if 0
    PARAM_MPV_T *prMpvCounter;
    #endif

    //prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    #if 0
    prMpvCounter = (PARAM_MPV_T *)prVdecEsInfo->prMpvCounter;
    #endif

    MPV_FlushEsmQ(ucEsId, TRUE); // abort decoder? FixMe

    #if 0
    prVdecEsInfo->u4PicCntJump = prMpvCounter->u4DecOkNs;
    #endif
    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    prVdecEsInfo->fgRefInfo = FALSE;
    prVdecEsInfo->ucSequenceHdrFound = FALSE;
    prVdecEsInfo->fgKeepPes = FALSE;
    prVdecEsInfo->fgSendedEos = FALSE;
    prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    prVdecEsInfo->fgTricking = fgTricking;
}

//Get parameter
UINT32 VDEC_GetDecPicCount(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    PARAM_MPV_T *prMpvCounter;

    prMpvCounter = (PARAM_MPV_T *)prVdecEsInfo->prMpvCounter;

    return prMpvCounter->u4DecOkNs - prVdecEsInfo->u4PicCntJump;
}
BOOL  VDEC_SetRenderLastFrame(UCHAR ucEsId, BOOL fgRenderLastFrame)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderLastFrame = fgRenderLastFrame;

    if(prVdecEsInfo->fgRenderLastFrame)
    {
        prVdecEsInfo->u2Disp2EmptyCnt = 0;
        prVdecEsInfo->u4PTSLast = 0;
        prVdecEsInfo->u8PTSLast = 0;
        prVdecEsInfo->fgSetStartPts = FALSE;

        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
        //prVdecEsInfo->ucSequenceHdrFound = FALSE;
        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    }

    return TRUE;
}
VOID VDEC_SetResizeInfo(UCHAR ucEsId, BOOL fgResizeDispFrm, UCHAR ucRzID)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prVdecEsInfo != NULL)
    {
        LOG(2,"VDEC_SetResizeInfo , fgResizeDispFrm = %d, ucRzID = %d\n",
            fgResizeDispFrm, ucRzID);
        prVdecEsInfo->fgResizeDispFrm = fgResizeDispFrm;
        prVdecEsInfo->ucRzId = ucRzID;
    }
}

#endif

BOOL VDEC_GetVideoThumbnailDirect(UCHAR ucEsId,VDP_THUMBNAIL_INFO_T* prDstInfo,
    UINT32 u4WorkingBufAddr, UINT32 u4WorkingBufSize,BOOL fgKeepRation)
{
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    PARAM_MPV_T *prMpvCounter;
    UCHAR ucVdpId,ucFbId;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    if((prDstInfo == NULL) || (!u4WorkingBufAddr) || (!u4WorkingBufSize))
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    if((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }


    prMpvCounter = (PARAM_MPV_T*)prVdecEsInfo->prMpvCounter;
    if(prMpvCounter == NULL)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }

    x_memcpy((void*)&prVdecEsInfo->rCapDstInfo, (void*)prDstInfo, sizeof(VDP_THUMBNAIL_INFO_T));
    prVdecEsInfo->fgKeepRation = fgKeepRation;
    prVdecEsInfo->u4ThumbRetrieveNs = prMpvCounter->u4RetrieveNs;
    prVdecEsInfo->u4WorkingBufAddr = u4WorkingBufAddr;
    prVdecEsInfo->u4WorkingBufSize = u4WorkingBufSize;
    if(prVdecEsInfo->ucFbgId==FBM_FBG_TYPE_UNKNOWN || prVdecEsInfoKeep->eCurState==VDEC_ST_STOP)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    ucVdpId=(UCHAR)VDP_Es2Vdp(ucEsId);
    if(ucVdpId==255)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }

    ucFbId=VDP_FrcGetLockFrameBuffer(ucVdpId);
    if(ucFbId==FBM_FB_ID_UNKNOWN)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    
    _VdecGetThumbnail2(ucEsId,ucFbId);
    return TRUE;
}

//clear the seqinfo flags
void VDEC_ClrMMParam(UCHAR ucEsId, UINT32 u4Type)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->fgMMSeqInfoValid = FALSE;

}

//clear the seqinfo flags
void VDEC_SetTPTFlag(UCHAR ucEsId, BOOL fgIsTPT)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->fgIsTPT = fgIsTPT;
}

VOID VDEC_RegRmCb(PFN_VDEC_RM_CB pfnRmCb)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();

    if (!prVdecInfo)
    {
        return ;
    }

    prVdecInfo->pfnRmCb = pfnRmCb;
    LOG(0, "[%s] VDEC RM pfnRmCb(0x%p).\n", __func__, pfnRmCb);
}

VOID VDEC_RmSetRenderVdp(UCHAR ucEsId, UINT32 u4VDPId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "prVdecEsInfo ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (prVdecEsInfo)
    {
        prVdecEsInfo->u4RenderVDPId = u4VDPId;
        LOG(0, "[%s]ucEsId(%d) u4RenderVDPId(%d).\n", __func__, ucEsId, u4VDPId);
    }
    UNUSED(prVdecEsInfo);
}

#ifdef CC_VDEC_RM_SUPPORT
VOID VDEC_RmSetPriority(UCHAR ucEsId, UINT32 u4Priority)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "_VDEC_GetEsInfoKeep ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        return;
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if (prVdecEsInfoKeep)
    {
        prVdecEsInfoKeep->u4Priority = u4Priority;
        LOG(1, "[%s]ucEsId(%d) u4Priority(%d).\n", __func__, ucEsId, u4Priority);
    }
}

VOID VDEC_RmSetKickDone(UCHAR ucEsId, BOOL fgSuccess)
{
    _VDEC_RMKickRet(ucEsId, VDEC_RM_BLK_VDEC_DONE);
}

VOID VDEC_RmSetIsVdpRender(UCHAR ucEsId, BOOL fgIsVDPRender)
{
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "prVdecEsInfo ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (prVdecEsInfo)
    {
        prVdecEsInfo->fgIsVDPRender = fgIsVDPRender;
        LOG(1, "[%s]ucEsId(%d) fgIsVDPRender(%d).\n", __func__, ucEsId, fgIsVDPRender);
    }
    UNUSED(prVdecEsInfo);
}
VOID VDEC_RmGetImgrzIdMsk(UCHAR ucEsId, UCHAR *pucImgrzId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    if ((ucEsId >= VDEC_MAX_ES) || (pucImgrzId == NULL))
    {
        LOG(0, "prVdecEsInfo ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (prVdecEsInfo)
    {
        *pucImgrzId = prVdecEsInfo->ucImgrzId;
        LOG(1, "[%s]ucEsId(%d) ucImgrzIdMsk(%d).\n", __func__, ucEsId, *pucImgrzId);
    }
    UNUSED(prVdecEsInfo);
}

VOID VDEC_RmSetImgrzIdMsk(UCHAR ucEsId, UCHAR ucImgrzId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "prVdecEsInfo ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (prVdecEsInfo == NULL)
    {
        return;
    }
    
    prVdecEsInfo->ucImgrzId = ucImgrzId;
    LOG(1, "[%s]ucEsId(%d) ucImgrzIdMsk(%d).\n", __func__, ucEsId, ucImgrzId);
    
    _VDEC_RMKickRet(ucEsId, VDEC_RM_BLK_IMGRZ_DONE);

    UNUSED(prVdecEsInfo);
}
#ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
UCHAR VDEC_RmGetEsByImgrz(UCHAR ucImgrzId)
{
    UCHAR ucEsIdx;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;

    for (ucEsIdx = 0; ucEsIdx < VDEC_MAX_ES; ucEsIdx++)
    {
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsIdx);
        if (prVdecEsInfo && (prVdecEsInfo->ucImgrzId == ucImgrzId))
        {
            LOG(1, "[%s] ucImgrzId(%d) -- ucEsIdx(%d).\n", __func__, ucImgrzId, ucEsIdx);
            return ucEsIdx;
        }
        
    }
    return VDEC_MAX_ES;
}

VOID VDEC_RmSetTextureMode(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "prVdecEsInfo ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (prVdecEsInfo)
    {
        prVdecEsInfo->fgTextureMode = TRUE;
        LOG(1, "[%s] fgTextureMode(%d).\n", __func__, prVdecEsInfo->fgTextureMode);
    }
    UNUSED(prVdecEsInfo);
}

#endif

#endif

//Get parameter
UINT32 VDEC_GetAvgPicSize(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(prVdecEsInfo == NULL)
    {
        LOG(3, "VDEC_GetAvgPicSize prVdecEsInfo null");
        return 0;
    }

    return prVdecEsInfo->u4AvgPicSize;
}

void VDEC_GetDecErrInfo(UCHAR ucEsId, BOOL* pfgError, BOOL* pfgDisplayStatus,UINT32 *pu4ErrMbCnt)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    UINT32 u4AVSyncStatus;

    if(pfgError == NULL || pfgDisplayStatus == NULL)
    {
        LOG(1, "VDEC_GetDecErrInfo pointer null");
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if (prVdecEsInfo)
    {
        *pfgError = prVdecEsInfo->fgDecodeErr;
        *pu4ErrMbCnt=prVdecEsInfo->u4DecErrMbCnt;
        FBM_GetAVSyncStatus(prVdecEsInfo->ucFbgId, &u4AVSyncStatus);
        switch (u4AVSyncStatus)
        {
            case FBM_FBG_AVSYNC_DISABLE:
            case FBM_FBG_AVSYNC_STC_INVALID:
            case FBM_FBG_AVSYNC_REPEAT:
            case FBM_FBG_AVSYNC_DROP:
            case FBM_FBG_AVSYNC_PTS_JITTER:
            case FBM_FBG_AVSYNC_NO_PTS:
                *pfgDisplayStatus = TRUE;
                break;
            case FBM_FBG_AVSYNC_NORMAL:
            case FBM_FBG_AVSYNC_RESERVE:
            case FBM_FBG_AVSYNC_WAIT_1ST:
            case FBM_FBG_AVSYNC_UNKNOWN:
            default:
                *pfgDisplayStatus = FALSE;
                break;
        }
    }

    return;
}

void VDEC_GetDecStatus(UCHAR ucEsId, BOOL* pfgLock
    , BOOL* pfgTopFldFirst, BOOL* pfgRepFirstFld, BOOL* pfgProgressiveFrm
    , UCHAR* pucPicType, UCHAR* pucPicStruture, UCHAR* pucTemporalRef
    , UINT32* pu4ReceiveNs, UINT32* pu4DecOkNs)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    //VDEC_INFO_T *prVdecInfo;
    PARAM_MPV_T* prMpvCounter;
    FBM_PIC_HDR_T *prFbmPicHdr = NULL;
    UINT32 u4FbgId;
    UINT32 u4FbId;
    UINT32 u4VideoFmt;

    if((pfgLock == NULL) ||
       (pfgTopFldFirst == NULL) ||
       (pfgRepFirstFld == NULL) ||
       (pfgProgressiveFrm == NULL) ||
       (pucPicType == NULL) ||
       (pucPicStruture == NULL) ||
       (pucTemporalRef == NULL) ||
       (pu4ReceiveNs == NULL) ||
       (pu4DecOkNs == NULL))
    {
        LOG(1, "VDEC_GetDecStatus pointer null");
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    //prVdecInfo = _VDEC_GetInfo();
    prMpvCounter = prVdecEsInfo->prMpvCounter;

    if (prVdecEsInfo->eNotifyStatus == VDEC_DEC_DECODE_NORMAL)
    {
        *pfgLock = TRUE;
    }
    else
    {
        *pfgLock = FALSE;
    }

    *pu4ReceiveNs = prMpvCounter->u4RetrieveNs;
    *pu4DecOkNs = prMpvCounter->u4DecOkNs;
    *pucPicType = prVdecEsInfo->ucPicType;

    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, 0, &u4FbgId, &u4FbId, &u4VideoFmt);
    }
    else
    {
        u4FbgId = (UINT32)prVdecEsInfo->ucFbgId;
        u4FbId = (UINT32)prVdecEsInfo->ucFbId;
    }

    if (u4FbgId != FBM_FBG_ID_UNKNOWN)
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr((UCHAR)u4FbgId, (UCHAR)u4FbId);
    }

    if(prFbmPicHdr)
    {
        *pucPicStruture = prFbmPicHdr->ucPicStruct;
        *pfgTopFldFirst = prFbmPicHdr->fgTopFldFirst;
        *pfgRepFirstFld = prFbmPicHdr->fgRepFirstFld;
        *pfgProgressiveFrm = prFbmPicHdr->fgProgressiveFrm;
        // ToDo: pucTemporalRef -> pi4TemporalRef
        *pucTemporalRef = (UCHAR)prFbmPicHdr->i4TemporalRef;
        *pucPicStruture = prFbmPicHdr->ucPicStruct;
    }
    else
    {
        *pucPicStruture = 0;
        *pfgTopFldFirst = 0;
        *pfgRepFirstFld = 0;
        *pfgProgressiveFrm = 0;
        *pucTemporalRef = 0;
        *pucPicStruture = 0;
    }
    return;
}


BOOL  VDEC_RegDecErrCb(PFN_VDEC_DECERR_CB pfnDecErrCb, UINT32 u4ErrDuration)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();

    if (pfnDecErrCb == NULL)
    {
        return FALSE;
    }

    prVdecInfo->pfnDecErrCb = pfnDecErrCb;
    prVdecInfo->u4ErrDuration= u4ErrDuration;

    return TRUE;
}


BOOL VDEC_RegPicTypeCb(PFN_VDEC_PIC_TYPE_CB pfnPicTypeCb)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();

    if (!prVdecInfo)
    {
        return FALSE;
    }

    prVdecInfo->pfPicTypeCb = pfnPicTypeCb;

    return TRUE;
}


BOOL VDEC_RegPicInfoCb(PFN_VDEC_PICINFO_CB pfnPicInfoCb)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();

    if (!prVdecInfo)
    {
        return FALSE;
    }

    prVdecInfo->pfPicInfo = pfnPicInfoCb;

    return TRUE;
}

BOOL VDEC_RegStatusCb(PFN_VDEC_STATUS_CB pfnStatusCb)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();

    if (!prVdecInfo)
    {
        return FALSE;
    }

    prVdecInfo->pfnStatusCb = pfnStatusCb;
    VDP_RegCbFunc(VDP_CB_FUNC_REPEAT_DROP_IND, (UINT32)_VDPReapeatDropCB, 0, 0);

    return TRUE;
}


//Asynchronous command
BOOL VDEC_PlayOneFrm(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    UCHAR ucFRefFbId, ucBRefFbId;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    ASSERT(ucEsId < VDEC_MAX_ES);

    if(!prVdecEsInfoKeep)
    {
        return FALSE;
    }

    if((prVdecEsInfoKeep) &&
       ((eFmt != prVdecEsInfoKeep->eCurFMT) ||
        ((prVdecEsInfoKeep->eCurState != VDEC_ST_STOP) &&
         (prVdecEsInfoKeep->eCurState != VDEC_ST_IDLE) &&
         (prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY)))) // trick mode in play state
    {
        return FALSE;
    }

    if(eFmt == VDEC_FMT_MAX)
    {
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
    }
    else if(eFmt == VDEC_FMT_H265)
    {
        #if defined(ENABLE_MULTIMEDIA)
        #ifndef CC_VDEC_H265_DISABLE
        if(!vVDECIsSupportH265())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_H264)
    {
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        if(!_VDEC_IsSupportH264())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_WMV)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VC1_DISABLE
        if(!_VDEC_IsSupportVc1())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MP4)
    {
        #ifdef CC_VDEC_MPEG4_DISABLE
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MJPEG)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_MJPEG_DISABLE
        if(!_VDEC_IsSupportMJPEG())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_RV)
    {
#ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_RV_DISABLE
        if(!_VDEC_IsSupportRV())
        #endif
    {
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
    }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
#endif
    }
    else if(eFmt == VDEC_FMT_VP6)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VP6_DISABLE
                if(!_VDEC_IsSupportVP6())
        #endif
            {
                LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
                return FALSE;
            }
        #else
                LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
                return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_VP8)
    {
    #ifdef ENABLE_MULTIMEDIA
    #ifndef CC_VDEC_VP8_DISABLE
                if(!_VDEC_Vp8IsSupport())
    #endif
            {
                LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
                return FALSE;
            }
    #else
                LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
                return FALSE;
    #endif
    }

    {
        LOG(6, "VDEC_PlayOneFrm\n");
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        prVdecEsInfo->fgPlayOneFrm = TRUE;

        //Put Backward Reference pic to EMPTY
        FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
        //prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
        //if(prFbmPicHdr)
        {
            if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_READY))
            {
                FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_EMPTY);
                LOG(3, "VDEC_PlayOneFrm ucBRefFbId %d\n", ucBRefFbId);
            }
            if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_READY))
            {
                FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_EMPTY);
                LOG(3, "VDEC_PlayOneFrm ucFRefFbId %d\n", ucFRefFbId);
            }
        }

        /*
        FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
        FBM_ReleaseDispQ(prVdecEsInfo->ucFbgId);
        */

        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;

        _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_PLAY);
    }
    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

    return TRUE;
}


BOOL  VDEC_RegRenderPtsCb(UCHAR ucEsId, PFN_VDEC_RENDER_PTS_CB pfnRenderPtsCb, UINT8 u1AttachedSrcId)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (!prVdecEsInfoKeep)
    {
        return FALSE;
    }

    prVdecEsInfoKeep->pfnRenderPtsCb = pfnRenderPtsCb;
    prVdecEsInfoKeep->u1AttachedSrcId = u1AttachedSrcId;
    return TRUE;
}


BOOL  VDEC_RegTrickPtsCb(UCHAR ucEsId, PFN_VDEC_TRICK_PTS_CB pfnTrickPtsCb, UINT8 u1AttachedSrcId)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (!prVdecEsInfoKeep)
    {
        return FALSE;
    }

    // trick -> normal, will register VDEC_RegTrickPtsCb = NULL;
    prVdecEsInfoKeep->pfnTrickPtsCb = pfnTrickPtsCb;
    prVdecEsInfoKeep->u1AttachedSrcId = u1AttachedSrcId;

    return TRUE;
}


BOOL  VDEC_SetRenderPts(UCHAR ucEsId, UINT32 u4Pts)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderFromPts = FALSE;
    prVdecEsInfo->fgRenderPts = TRUE;
    prVdecEsInfo->u4RenderPts = u4Pts;
    prVdecEsInfo->u4PTSLast = 0;
    prVdecEsInfo->u8PTSLast = 0;
    prVdecEsInfo->fgSetStartPts = FALSE;

    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    //prVdecEsInfo->ucSequenceHdrFound = FALSE;
    prVdecEsInfo->fgRefInfo = FALSE;
    prVdecEsInfo->fgKeepPes = FALSE;
    prVdecEsInfo->fgSendedEos = FALSE;
    prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;

    return TRUE;
}

BOOL  VDEC_SetTotalTime(UCHAR ucEsId, UINT32 u4TotalTime)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->u4TotalTime = u4TotalTime;

    return TRUE;
}

BOOL  VDEC_SetFileSize(UCHAR ucEsId, UINT64 u8FileSize)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->u8FileSize = u8FileSize;

    return TRUE;
}

BOOL  VDEC_SetLastTime(UCHAR ucEsId, UINT32 u4LastTime)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->u4PTSLast = u4LastTime;

    return TRUE;
}

BOOL  VDEC_SetWaitPvrSps(UCHAR ucEsId, UINT8 u1PVRWaitSps)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->u1PVRWaitSps = u1PVRWaitSps;
 
    return TRUE;
}

BOOL  VDEC_SetRenderBackStep(UCHAR ucEsId, BOOL fgRenderBackStep, UINT64 u8Pos,
    INT32 i4TempRef, UINT16 u2DecodingOrder)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderBackStep = fgRenderBackStep;

    if(fgRenderBackStep)
    {
        prVdecEsInfo->u8RenderFromPos = u8Pos;
        prVdecEsInfo->i4RenderTempRef = i4TempRef;
        prVdecEsInfo->u2RenderDecodingOrder = u2DecodingOrder;
        prVdecEsInfo->u2Disp2EmptyCnt = 0;
        prVdecEsInfo->u4PTSLast = 0;
        prVdecEsInfo->u8PTSLast = 0;
        prVdecEsInfo->fgSetStartPts = FALSE;

        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
        //prVdecEsInfo->ucSequenceHdrFound = FALSE;
        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    }

    return TRUE;
}

BOOL  VDEC_SetRenderFromPos(UCHAR ucEsId, BOOL fgRenderFromPos,
    UINT64 u8Pos, INT32 i4TempRef, UINT16 u2DecodingOrder)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderFromPos = fgRenderFromPos;

    if(prVdecEsInfo->fgRenderFromPos)
    {
        prVdecEsInfo->u8RenderFromPos = u8Pos;
        prVdecEsInfo->i4RenderTempRef = i4TempRef;
        prVdecEsInfo->u2RenderDecodingOrder = u2DecodingOrder;
        prVdecEsInfo->u2Disp2EmptyCnt = 0;
        prVdecEsInfo->u4PTSLast = 0;
        prVdecEsInfo->u8PTSLast = 0;
        prVdecEsInfo->fgSetStartPts = FALSE;

        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
        //prVdecEsInfo->ucSequenceHdrFound = FALSE;
        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    }

    return TRUE;
}

BOOL  VDEC_SetRenderFromPts(UCHAR ucEsId, UINT32 u4Pts)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderPts = FALSE;
    prVdecEsInfo->fgRenderFromPts = TRUE;
    prVdecEsInfo->u4RenderFromPts = u4Pts;
    prVdecEsInfo->u2Disp2EmptyCnt = 0;
    prVdecEsInfo->u4PTSLast = 0;
    prVdecEsInfo->u8PTSLast = 0;
    prVdecEsInfo->fgSetStartPts = FALSE;
    prVdecEsInfo->fgAutoPause = FALSE;
    prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;

    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    //prVdecEsInfo->ucSequenceHdrFound = FALSE;

    return TRUE;
}


BOOL  VDEC_SetRenderFromPtsAutoPause(UCHAR ucEsId, UINT32 u4Pts, BOOL fgAutoPause)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderFromPts = TRUE;
    prVdecEsInfo->u4RenderFromPts = u4Pts;
    prVdecEsInfo->u2Disp2EmptyCnt = 0;
    prVdecEsInfo->u4PTSLast = 0;
    prVdecEsInfo->u8PTSLast = 0;
    prVdecEsInfo->fgSetStartPts = FALSE;
    prVdecEsInfo->fgAutoPause = fgAutoPause;
    prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;

    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    //prVdecEsInfo->ucSequenceHdrFound = FALSE;

    return TRUE;
}

BOOL  VDEC_SetRenderFromPtsAutoPauseStartPts(UCHAR ucEsId, UINT32 u4Pts, BOOL fgAutoPause, BOOL fgStartPts)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderFromPts = TRUE;
    prVdecEsInfo->u4RenderFromPts = u4Pts;
    prVdecEsInfo->u2Disp2EmptyCnt = 0;
    prVdecEsInfo->u4PTSLast = 0;
    prVdecEsInfo->u8PTSLast = 0;
    prVdecEsInfo->fgSetStartPts = fgStartPts;
    prVdecEsInfo->fgAutoPause = fgAutoPause;
    prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;

    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    //prVdecEsInfo->ucSequenceHdrFound = FALSE;

    return TRUE;
}



BOOL VDEC_SetRenderFromFirstPic(UCHAR ucEsId,
    BOOL fgRenderFromFirstPic, UINT64 u8LastRenderPos)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderFromFirstPic = fgRenderFromFirstPic;

    if(fgRenderFromFirstPic)
    {
        prVdecEsInfo->u4PTSLast = 0;
        prVdecEsInfo->u8PTSLast = 0;
        prVdecEsInfo->fgSetStartPts = FALSE;
        prVdecEsInfo->u8RenderFromPos = u8LastRenderPos;
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
        //prVdecEsInfo->ucSequenceHdrFound = FALSE;
        prVdecEsInfo->u2Disp2EmptyCnt = 0;
        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    }

    return TRUE;
}

BOOL VDEC_SetRenderPicCount(UCHAR ucEsId, BOOL fgRenderPicCnt, UINT32 u4Count)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgRenderPicCnt = fgRenderPicCnt;
    prVdecEsInfo->u4RenderPicCnt = u4Count;
    if(fgRenderPicCnt)
    {
        prVdecEsInfo->u4PTSLast = 0;
        prVdecEsInfo->u8PTSLast = 0;
        prVdecEsInfo->fgSetStartPts = FALSE;

        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        //prVdecEsInfo->ucSequenceHdrFound = FALSE;
        prVdecEsInfo->u2Disp2EmptyCnt = 0;
        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    }

    return TRUE;
}

BOOL VDEC_SetGetSeqHdrOnlyBegin(UCHAR ucEsId,
    ENUM_VDEC_FMT_T eFmt, BOOL *pfgPowerOff, UCHAR *ucMpvId)
{
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo->fgOnlyGotSeqHeader = TRUE;
    
    *ucMpvId =  VLD0;  //VDEC_GetMpvId();
    *pfgPowerOff = (!prVdecInfo->arVdecVldInfo[*ucMpvId].fgPowerOn);
    if(!prVdecInfo->arVdecVldInfo[*ucMpvId].fgPowerOn)
    {
        UNUSED(VDEC_PowerOn(ucEsId, *ucMpvId));
    }

    if((eFmt != prVdecEsInfoKeep->eCurFMT) ||
       (prVdecEsInfoKeep->eCurState == VDEC_ST_STOP))
    {
        prVdecEsInfoKeep->eCurFMT = eFmt;
        UNUSED(_VDEC_DecCfg(ucEsId));
    }

    return TRUE;
}

BOOL VDEC_SetGetSeqHdrOnlyEnd(UCHAR ucEsId, BOOL fgPowerOff, UCHAR ucMpvId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfo->fgOnlyGotSeqHeader)
    {
        if((prVdecEsInfoKeep->pfnVdecStop) && (prVdecEsInfoKeep->eCurState == VDEC_ST_STOP))
        {
            prVdecEsInfoKeep->pfnVdecStop(ucEsId);
        }
        // I am not sure if we need this.
        //MPV_FlushEsmQ(ES 0, FALSE);

        // for resume stop, we should assign this variable
        if(!prVdecEsInfo->ucSequenceHdrFound)
        {
            // copy from mpv_if.c _MPV_WaitRandomAP(...)
            prVdecEsInfo->ucSequenceHdrFound = 1;
            prVdecEsInfo->fgChgThrsd = TRUE;
            prVdecEsInfo->u4TotalPicSize = 0;
            prVdecEsInfo->u4PicCnt = 0;
            prVdecEsInfo->u4LastVldRptr = 0;
        }

        if(fgPowerOff)
        {
            UNUSED(VDEC_PowerOff(ucEsId, ucMpvId));
        }
    }
    prVdecEsInfo->fgOnlyGotSeqHeader = FALSE;

    return TRUE;
}

BOOL  VDEC_SetSuperFreeze(UCHAR ucEsId, BOOL fgSuperFreeze)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(fgSuperFreeze)
    {
        LOG(6, "VDEC_SetSuperFreeze!!\n");

        MPV_FlushEsmQ(ucEsId, FALSE);

        prVdecEsInfo->u2Disp2EmptyCnt = 0;
        prVdecEsInfo->u4PTSLast = 0;
        prVdecEsInfo->u8PTSLast = 0;
        prVdecEsInfo->fgSetStartPts = FALSE;
        prVdecEsInfo->fgSuperFreeze = TRUE;
        prVdecEsInfo->fgNormalAfterFreeze = FALSE;

        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
        //prVdecEsInfo->ucSequenceHdrFound = FALSE;
        prVdecEsInfo->fgRefInfo = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        prVdecEsInfo->fgSendedEos = FALSE;
        prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;

        FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SUPER_FREEZE);
        prVdecEsInfo->fgTricking = FALSE;
        prVdecEsInfo->fgMMPlayback = TRUE;
        VDEC_Pause(ucEsId);
        LOG(6, "VDEC_SetSuperFreeze exit!!\n");
    }
    else
    {
        prVdecEsInfo->fgSuperFreeze = FALSE;
        prVdecEsInfo->fgNormalAfterFreeze = FALSE;
    }

    return TRUE;
}


BOOL VDEC_GetDisplayFrameBufferAddr(UCHAR ucEsId, VDP_CAPTURE_INTO_T* prCapInfo)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    UCHAR ucDispFbNum;
    UCHAR ucFbgId;
    UCHAR ucFbId = FBM_FB_ID_UNKNOWN;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;

    if(ucEsId >= VDEC_MAX_ES)
    {
        ucEsId = ES0;
        LOG(0, "VDEC_GetDisplayFrameBufferAddr: (ucEsId >= VDEC_MAX_ES)\n");
        ASSERT(0);
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    ucFbgId = prVdecEsInfo->ucFbgId;
    ucDispFbNum = FBM_CheckFrameBufferDispQ(ucFbgId);

    ucFbId = FBM_PreLookNthFBFromDispQ(ucFbgId, ucDispFbNum - 1);  // -1 the last disp Q FB may be nothing
    LOG(4, "[%s-%d]Get ucFbId(%d) FbNs(%d).\n", __func__, __LINE__, ucFbId, ucDispFbNum);

    if(ucFbId != FBM_FB_ID_UNKNOWN)
    {
        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &(prCapInfo->u4AddrY), &(prCapInfo->u4AddrC));

        FBM_GetFrameBufferSize(ucFbgId, &(prCapInfo->u4MemHSize), &(prCapInfo->u4MemVSize));

        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);

        if (prFbmSeqHdr == NULL)
        {
            LOG(1, "[%s-%d]prFbmSeqHdr NULL.\n", __func__, __LINE__);
            return FALSE;
        }
        else
        {
            /* Thumbnail usage: Image resizer will use this as pitch. */
            if (prFbmPicHdr && prFbmSeqHdr->fgResizeSmallPic)
            {
                prCapInfo->u4MemHSize = (UINT32)prFbmPicHdr->u4PicWidthPitch;

                prCapInfo->u4HSize = (UINT32)prFbmPicHdr->u4PicWidth;
                prCapInfo->u4VSize = (UINT32)prFbmPicHdr->u4PicHeight;
            }
            else
            {
                prCapInfo->u4MemHSize = (UINT32)prFbmSeqHdr->u2LineSize;

                prCapInfo->u4HSize = (UINT32)prFbmSeqHdr->u2OrgHSize;
                prCapInfo->u4VSize = (UINT32)prFbmSeqHdr->u2OrgVSize;
            }            

            prCapInfo->u4RRMode = (UINT32)prFbmSeqHdr->fgRRYEn;
            #if (defined (CC_MT5890))
            if (prFbmSeqHdr->fgUfoEn || (10 == prVdecEsInfo->u4BitDepth))
            {
                FBM_GetFrameBufferAddrExt(ucFbgId, ucFbId, &(prCapInfo->u4AddrYLen), &(prCapInfo->u4AddrCLen));
            }
            #endif
        }
    }

    return TRUE;

}

BOOL VDEC_GetIFrameBufferAddr(UCHAR ucEsId, VDP_CAPTURE_INTO_T* prCapInfo)
{
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;

    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(!prVdecEsInfo)
    {
        Printf("prVdecEsInfo = 0!\n");
        return FALSE;
    }

    prCapInfo->u4AddrY = 0;
    prCapInfo->u4AddrC = 0;
    FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId,
        prVdecEsInfo->ucFbId, &(prCapInfo->u4AddrY), &(prCapInfo->u4AddrC));

    FBM_GetFrameBufferSize(prVdecEsInfo->ucFbgId,
        &(prCapInfo->u4MemHSize), &(prCapInfo->u4MemVSize));

    if((prCapInfo->u4AddrY == 0) ||
       (prCapInfo->u4AddrC == 0) ||
       (prCapInfo->u4MemHSize == 0) ||
       (prCapInfo->u4MemVSize == 0))
    {
        Printf("size = 0!\n");
        return FALSE;
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucFbId);

    if (prFbmSeqHdr == NULL)
    {
        Printf("prFbmSeqHdr = 0!\n");
        return FALSE;
    }
    else
    {
        /* Thumbnail usage: Image resizer will use this as pitch. */
        // because we do not enter b2r, we don't need to check resize case
        // but because we already assign origina information in picture header
        // and target information to sequence header, we have to use picture header in this case
        if (prFbmPicHdr && prFbmSeqHdr->fgResizeSmallPic)
        {
            prCapInfo->u4MemHSize = (UINT32)prFbmPicHdr->u4PicWidthPitch;

            prCapInfo->u4HSize = (UINT32)prFbmPicHdr->u4PicWidth;
            prCapInfo->u4VSize = (UINT32)prFbmPicHdr->u4PicHeight;
        }
        else
        {
            prCapInfo->u4MemHSize = (UINT32)prFbmSeqHdr->u2LineSize;

            prCapInfo->u4HSize = (UINT32)prFbmSeqHdr->u2OrgHSize;
            prCapInfo->u4VSize = (UINT32)prFbmSeqHdr->u2OrgVSize;
        }

        prCapInfo->u4RRMode = (UINT32)prFbmSeqHdr->fgRRYEn;
    }

    return TRUE;
}

BOOL  VDEC_GetVideoThumbnail(UCHAR ucEsId, UINT16 u2CompId, BOOL bIsOnePass,
    VDP_THUMBNAIL_INFO_T* prDstInfo, VDP_CAPTURE_INTO_T* prSrcInfo, BOOL fgKeepRation)
{
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
  
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
    
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "VDEC_GetVideoThumbnail ucEsId %d!!", (UINT32)ucEsId);
        return FALSE;
    }
    if((prDstInfo == NULL) || (prSrcInfo == NULL))
    {
        LOG(0, "VDEC_GetVideoThumbnail pointer null!!");
        return FALSE;
    }
    
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);    
    if (prFbmSeqHdr == NULL)
    {        
        LOG(0, "prFbmSeqHdr pointer null!!");
        return FALSE;
    }
    
    if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MJPEG)
    {
        #if  defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)// Force raster out for VENC
        prSrcInfo->u4IsRsIn = 1; //MJPEG for 5395 encoder input
        prSrcInfo->u4SrcSwap = 0;
        #else
        prSrcInfo->u4IsRsIn = 0;
        prSrcInfo->u4SrcSwap = 6;//block swap mode;
        #endif
    }
    else if ((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265) || prFbmSeqHdr->fgUfoEn 
        || ((!prFbmSeqHdr->fgRasterOrder) && (prFbmSeqHdr->ucBlockType)))
    {
        prSrcInfo->u4IsRsIn = 0;
        prSrcInfo->u4SrcSwap = 0;//block swap mode;
    }
    else
    {
        prSrcInfo->u4IsRsIn = 0;
        prSrcInfo->u4SrcSwap = 6;//block swap mode;
    }
    
    if (prFbmSeqHdr->fgUfoEn)
    {
        prSrcInfo->fgUFOOn = TRUE;
        if (8 == prVdecEsInfo->u4BitDepth)
        {
            prSrcInfo->u4UFOType = RZ_UFO_TYPE_8BIT;            
            LOG(3, "8 bit ufo enable\n");
        }
    }
    if (10 == prVdecEsInfo->u4BitDepth)
    {
        prSrcInfo->fgUFOOn = TRUE;
        prSrcInfo->u4UFOType = RZ_UFO_TYPE_10BIT_COMPACT;           
        LOG(3, "10 bit ufo enable\n");
    }
    prVdecEsInfo->u2CompId = u2CompId;
    x_memcpy((void*)&prVdecEsInfo->rCapDstInfo, (void*)prDstInfo, sizeof(VDP_THUMBNAIL_INFO_T));
    x_memcpy((void*)&prVdecEsInfo->rCapSrcInfo, (void*)prSrcInfo, sizeof(VDP_CAPTURE_INTO_T));
    #ifdef CC_REALD_3D_SUPPORT
    if(prVdecEsInfo->fgIsRealD3DFmt)
    {
        if(prVdecEsInfo->rCapSrcInfo.u4VSize == 1088)
        {
            prVdecEsInfo->rCapSrcInfo.u4VSize = 1080;
        }
        else
        {
            prVdecEsInfo->rCapSrcInfo.u4VSize -= 2;
        }
    }
    #endif
    prVdecEsInfo->bIsOnePass = bIsOnePass;
    prVdecEsInfo->fgKeepRation = fgKeepRation;
    
    if (prFbmSeqHdr->fgCropping)
    {
        //use cropped dimension
        prVdecEsInfo->rCapSrcInfo.u4HSize = prFbmSeqHdr->u4CropWidth;
        prVdecEsInfo->rCapSrcInfo.u4VSize = prFbmSeqHdr->u4CropHeight;
        LOG(3, "Crop W:%d H:%d\n", prFbmSeqHdr->u4CropWidth, prFbmSeqHdr->u4CropHeight);
    }
    
    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_CAPTURE);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

    UNUSED(prSrcInfo);
    UNUSED(prDstInfo);
    return TRUE;
}

BOOL  VDEC_GetVideoThumbnail2(UCHAR ucEsId,
    VDP_THUMBNAIL_INFO_T* prDstInfo,
    UINT32 u4WorkingBufAddr, UINT32 u4WorkingBufSize,
    BOOL fgKeepRation)
{
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    PARAM_MPV_T *prMpvCounter;
    //VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    if((prDstInfo == NULL) || (!u4WorkingBufAddr) || (!u4WorkingBufSize))
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    if((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }

    prMpvCounter = (PARAM_MPV_T*)prVdecEsInfo->prMpvCounter;
    if(prMpvCounter == NULL)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }

    #if 0
    if(prVdecEsInfoKeep->hThumbTimer && !prVdecEsInfoKeep->fgThumbTimerStarted)
    {
        VERIFY(x_timer_start(prVdecEsInfoKeep->hThumbTimer, VDEC_DATA_TIMER_THRSD, X_TIMER_FLAG_REPEAT,
            _ThumbTimeout, (void*)(UINT32)ucEsId) == OSR_OK);
        prVdecEsInfoKeep->fgThumbTimerStarted = TRUE;
    }
    #endif

    LOG(6, "ES(%d) %s\n", ucEsId, __FUNCTION__);

    x_memcpy((void*)&prVdecEsInfo->rCapDstInfo, (void*)prDstInfo, sizeof(VDP_THUMBNAIL_INFO_T));
    prVdecEsInfo->fgKeepRation = fgKeepRation;
    prVdecEsInfo->fgThumbStartTime = FALSE;
    prVdecEsInfo->u4ThumbRetrieveNs = prMpvCounter->u4RetrieveNs;
    prVdecEsInfo->u4WorkingBufAddr = u4WorkingBufAddr;
    prVdecEsInfo->u4WorkingBufSize = u4WorkingBufSize;
    prVdecEsInfo->fgThumbMethod2 = TRUE;

    UNUSED(prDstInfo);
    return TRUE;
}

BOOL  VDEC_StopVideoThumbnail2(UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }
    if((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
        return FALSE;
    }

    prVdecEsInfo->fgThumbMethod2 = FALSE;
    LOG(6, "ES(%d) %s\n", ucEsId, __FUNCTION__);

    #if 0
    if(prVdecEsInfoKeep->hThumbTimer && prVdecEsInfoKeep->fgThumbTimerStarted)
    {
        VERIFY(x_timer_stop(prVdecEsInfoKeep->hThumbTimer) == OSR_OK);
        prVdecEsInfoKeep->fgThumbTimerStarted = FALSE;
    }
    #endif

    return TRUE;
}

// This function get I-Frame Y,C address, and use it to transform YC-Block --> ARGB8888(one pass if no keep ratio)
BOOL  VDEC_GetVideoThumbnail3(UCHAR ucEsId, UINT16 u2CompId, BOOL bIsOnePass,
    VDP_THUMBNAIL_INFO_T* prDstInfo,  BOOL fgKeepRation)
{
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VDP_CAPTURE_INTO_T rSrcInfo;

    x_memset(&rSrcInfo, 0, sizeof(rSrcInfo));

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "VDEC_GetVideoThumbnail ucEsId %d!!", (UINT32)ucEsId);
        return FALSE;
    }

    if((prDstInfo == NULL))
    {
        LOG(0, "VDEC_GetVideoThumbnail pointer null!!\n");
        return FALSE;
    }

    if(!VDEC_GetIFrameBufferAddr(ucEsId, &rSrcInfo))
    {
        LOG(1, " VDEC_GetIFrameBufferAddr fail\n");
        return FALSE;
    }

    if((rSrcInfo.u4AddrY == 0) || (rSrcInfo.u4AddrC== 0) || (rSrcInfo.u4RRMode > 1))
    {
        LOG(1, " addr fail\n");
        return FALSE;
    }
    else
    {
        // get pitch from FB, io_mtvdec can not get line size, we fill here
        prDstInfo->u4CanvasPitch = rSrcInfo.u4MemHSize*4;
        LOG(6, " u4AddrY 0x%x, u4 AddrC 0x%x \n",rSrcInfo.u4AddrY,rSrcInfo.u4AddrC);
    }

    if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MJPEG)
    {
        #if  defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)// Force raster out for VENC
        rSrcInfo.u4IsRsIn = 1; //MJPEG for 5395 encoder input
        rSrcInfo.u4SrcSwap = 0;
        #else
        rSrcInfo.u4IsRsIn = 0;
        rSrcInfo.u4SrcSwap = 6;//block swap mode;
        #endif
    }
    else
    {
        rSrcInfo.u4IsRsIn = 0;
        rSrcInfo.u4SrcSwap = 6;//block swap mode;
    }

    prVdecEsInfo->u2CompId = u2CompId;
    x_memcpy((void*)&prVdecEsInfo->rCapDstInfo, (void*)prDstInfo, sizeof(VDP_THUMBNAIL_INFO_T));
    x_memcpy((void*)&prVdecEsInfo->rCapSrcInfo, (void*)&rSrcInfo, sizeof(VDP_CAPTURE_INTO_T));
    prVdecEsInfo->bIsOnePass = bIsOnePass;
    prVdecEsInfo->fgKeepRation = fgKeepRation;

    // trigger get thumbnail
    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_CAPTURE);

    // Virtual Picture Header for Event Trigger
    _arPesInfo[ucEsId].ucEsId = ucEsId;
    _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;

    // send virtual es
    VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);

    UNUSED(rSrcInfo);
    UNUSED(prDstInfo);
    return TRUE;
}

//Synchronous command, only called when pause/idle/stop
BOOL VDEC_ReleaseDispQ(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    UCHAR ucVdpId;

    ASSERT(ucEsId < VDEC_MAX_ES);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
    {
        ucVdpId=(UCHAR)VDP_Es2Vdp(ucEsId);
        if(ucVdpId==255)
        {
            LOG(0, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);
            return FALSE;
        }
        VDP_FlushB2RChgFrameMsg(ucVdpId);
    }
    FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
    FBM_ReleaseDispQ(prVdecEsInfo->ucFbgId);
    FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);

    prVdecEsInfo->fgFlushFB = TRUE; //for h.264
    return TRUE;
}

#ifdef CC_SUPPORT_STR
BOOL VDEC_CkgenOnOff(BOOL fgEnable)
{
#if (!defined(CC_MT5880) && !defined(CC_MT5881) && !defined(CC_MT5890))
#define BLOCK_RST_CFG0 0x270
#define VDEC_RST ((UINT32)1 << 18)
#define BLOCK_CKEN_CFG0 0x278
#define VDEC_CKEN ((UINT32)1 << 18)

    if (fgEnable)
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~VDEC_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) | VDEC_CKEN);
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) | VDEC_RST);
    }
    else
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~VDEC_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) & ~VDEC_CKEN);
    }

#else
// python
#define BLOCK_RST_CFG1 0x1C4
#define VDEC_RST ((UINT32)1 << 20)
#define BLOCK_CKEN_CFG1 0x1CC
#define VDEC_CKEN ((UINT32)1 << 20)

    if (fgEnable)
    {
        //IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG1, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG1) & ~VDEC_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG1, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG1) | VDEC_CKEN);
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG1, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG1) | VDEC_RST);
    }
    else
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG1, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG1) & ~VDEC_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG1, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG1) & ~VDEC_CKEN);
    }

#endif


    UNUSED(fgEnable);
    return TRUE;
}
#endif

BOOL VDEC_PowerOn(UCHAR ucEsId, UCHAR ucVldId)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    if(ucVldId >= VDEC_MAX_VLD)
    {
        ASSERT(ucVldId < VDEC_MAX_VLD);
        return FALSE;
    }

    _VDEC_LockVld(ucEsId, ucVldId);
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, ucVldId, VLD_LOCK_LOG_IF_BASE+2);
    #endif
    prVdecInfo->arVdecVldInfo[ucVldId].u4PowerOnRef++;
    LOG(6, "PowerUp(vld%d)(ref%d)!!\n", ucVldId,
        prVdecInfo->arVdecVldInfo[ucVldId].u4PowerOnRef);
    if(prVdecInfo->arVdecVldInfo[ucVldId].fgPowerOn)
    {
        #if VLD_LOCK_LOG
        u4VLDLogWr(ucEsId, ucVldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_IF_BASE + 2);
        #endif
        _VDEC_UnlockVld(ucEsId, ucVldId);
        return TRUE;
    }

    //power on VDEC module
    #ifdef CC_DYNAMIC_POWER_ONOFF
    #if defined(CC_MT5360) || defined(CC_MT5363)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_MC) & VDEC_PWR_ON);
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_SYS) & VDEC_PWR_ON);
    #elif defined(CC_MT5387)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL) & VDEC_PWR_ON);
    #elif defined(CC_MT5365)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_ON);
    }
    #elif defined(CC_MT5395)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_ON);
    }
    #elif defined(CC_MT5396)
    // FIXME
    #elif defined(CC_MT5368)
    // FIXME
    #elif defined(VDEC_IS_POST_MT5389)
    // FIXME
    #else //if defined(CC_MT5391)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL) & VDEC_PWR_ON);
    #endif
    #endif
    prVdecInfo->arVdecVldInfo[ucVldId].fgPowerOn = TRUE;
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, ucVldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_IF_BASE + 3);
    #endif
    _VDEC_UnlockVld(ucEsId, ucVldId);   
    return TRUE;
}

BOOL VDEC_PowerOff(UCHAR ucEsId, UCHAR ucVldId)
{
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    if(ucVldId >= VDEC_MAX_VLD)
    {
        ASSERT(ucVldId < VDEC_MAX_VLD);
        return FALSE;
    }

    if(!prVdecInfo->arVdecVldInfo[ucVldId].fgPowerOn)
    {
        return TRUE;
    }

    _VDEC_LockVld(ucEsId, ucVldId);
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, ucVldId, VLD_LOCK_LOG_IF_BASE+3);
    #endif
    prVdecInfo->arVdecVldInfo[ucVldId].u4PowerOnRef--;
    LOG(6, "PowerDown(vld%d)(ref%d)!!\n", ucVldId,
        prVdecInfo->arVdecVldInfo[ucVldId].u4PowerOnRef);
    if(prVdecInfo->arVdecVldInfo[ucVldId].u4PowerOnRef > 0)
    {
        #if VLD_LOCK_LOG
        u4VLDLogWr(ucEsId, ucVldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_IF_BASE + 4);
        #endif
        _VDEC_UnlockVld(ucEsId, ucVldId);
        return TRUE;
    }

    //power off VDEC module
    #ifdef CC_DYNAMIC_POWER_ONOFF
    #if defined(CC_MT5360) || defined(CC_MT5363)
    IMGRZ_Lock();
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_MC, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_MC) | VDEC_PWR_OFF);
    IMGRZ_Unlock();
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL_SYS, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL_SYS) | VDEC_PWR_OFF);
    #elif defined(CC_MT5387)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL) | VDEC_PWR_OFF);
    #elif defined(CC_MT5365)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_OFF);
    }
    #elif defined(CC_MT5395)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_OFF);
    }
    #elif defined(CC_MT5396)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_SPEC, VDEC_PDN_CON_OFF);
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_MODULE, VDEC_PDN_CON_OFF);
    }
    #elif defined(CC_MT5368)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_SPEC, VDEC_PDN_CON_OFF);
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_MODULE, VDEC_PDN_CON_OFF);
    }
    #elif defined(VDEC_IS_POST_MT5389)
    // jpeg use the other h/w
    if(ucVldId == VLD0)
    {
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_SPEC, VDEC_PDN_CON_OFF);
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_MODULE, VDEC_PDN_CON_OFF);
    }
    #else //if defined(CC_MT5391)
    IO_WRITE32(CKGEN_BASE, VDEC_CLK_SEL, IO_READ32(CKGEN_BASE, VDEC_CLK_SEL) | VDEC_PWR_OFF);
    #endif
    #endif

    prVdecInfo->arVdecVldInfo[ucVldId].fgPowerOn = FALSE;
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, ucVldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_IF_BASE + 5);
    #endif
    _VDEC_UnlockVld(ucEsId, ucVldId);  
    return TRUE;
}

void VDEC_StartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, VDEC_PTS_INFO_T* prPTSInfo)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    prVdecEsInfo->fgEnCalPTS = fgEnCalPTS;
    prVdecEsInfo->u4StartPTS = u4Pts;
    if(prPTSInfo)
    {
        x_memcpy((void*)&prVdecEsInfo->rPTSInfo, (void*)prPTSInfo, sizeof(VDEC_PTS_INFO_T));
    }
    if(prVdecEsInfoKeep->pfnVdecStartPts)
    {
        _VDEC_LockFlushMutex(ucEsId);    
        prVdecEsInfo->fgFlushEsmQ = TRUE;
        _VDEC_UnlockFlushMutex(ucEsId);
        
        _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_FLUSH_ESMQ);
        
        // Virtual Picture Header for Event Trigger
        _arPesInfo[ucEsId].ucEsId = ucEsId;
        _arPesInfo[ucEsId].ucMpvId = (UCHAR)VDEC_MAX_VLD;
        
        VERIFY(VDEC_SendEs(&_arPesInfo[ucEsId]) == 1);
        //_VDEC_FlushEs(ucEsId);
        UNUSED(VDEC_ReleaseDispQ(ucEsId));

        _VDEC_LockFlushSema(ucEsId);
        
        if(prVdecEsInfoKeep->pfnVdecStartPts)
        {
            prVdecEsInfoKeep->pfnVdecStartPts(ucEsId , fgEnCalPTS, u4Pts, (UINT32)prPTSInfo);
        }
    }
}

BOOL VDEC_ChkCodecCap(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt)
{
    UNUSED(ucEsId);

    if(eFmt == VDEC_FMT_H265)
    {
        #if defined(ENABLE_MULTIMEDIA)
        #ifndef CC_VDEC_H265_DISABLE
        if(!vVDECIsSupportH265())
        #endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_H264)
    {
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        if(!_VDEC_IsSupportH264())
        #endif
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_WMV)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VC1_DISABLE
        if(!_VDEC_IsSupportVc1())
        #endif
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MP4)
    {
        #ifdef CC_VDEC_MPEG4_DISABLE
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_MJPEG)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_MJPEG_DISABLE
        if(!_VDEC_IsSupportMJPEG())
        #endif
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_RV)
    {
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_RV_DISABLE
        if(!_VDEC_IsSupportRV())
        #endif
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_AVS)
    {
        #ifndef CC_VDEC_AVS_DISABLE
        if(!_VDEC_IsSupportAVS())
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
        #else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
        #endif
    }
    else if(eFmt == VDEC_FMT_VP6)
    {
    #ifdef ENABLE_MULTIMEDIA
    #ifndef CC_VDEC_VP6_DISABLE
        if(!_VDEC_IsSupportVP6())
    #endif
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
    #else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
    #endif
    }
    else if(eFmt == VDEC_FMT_VP8)
    {
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP8_DISABLE
        if(!_VDEC_Vp8IsSupport())
#endif
        {
            LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
            return FALSE;
        }
#else
        LOG(1, "Video codec format %d not support!!\n", (UINT32)eFmt);
        return FALSE;
#endif
    }
    else if(eFmt==VDEC_FMT_VP9)
    {
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP9_DISABLE
        if(!_VDEC_Vp9IsSupport())
#endif
        {
            LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
            return FALSE;
        }
#else
        LOG(1, "Video codec format %d not support!!", (UINT32)eFmt);
        return FALSE;
#endif
    }

    return TRUE;
}

#if defined(VDEC_4K_DEC_CHECK)
BOOL VDEC_Chk4K2KCap(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt)
{
    UINT32 u4Width = 0,u4Height = 0;
    UINT32 u4FrameRate = 0;
    BOOL fgRet = TRUE;

    
    #ifdef CC_DUAL_CORE_DEC
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        //LAE decode thread skip eos handle.
        UINT32 u4Mode, u4Param2 = 0, u4Param3 = 0;
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,&u4Param2,&u4Param3);
        if(u4Mode == LAE_DECODE)
        {
            return TRUE;
        }
    }
    #endif
    UNUSED(eFmt);
    VDEC_GetParam(ucEsId,SEQ_HDR_INFO,&u4Width,&u4Height,&u4FrameRate);

    //Only filter >= 3840*2160, maybe mask more resolution. TBD
    if((u4Width >= VDEC_4K2K_EFUSE_WIDTH) && (u4Height >= VDEC_4K2K_EFUSE_HEIGHT))
    {
        if(IS_Support4KDecode())
        {
            if((u4FrameRate > MPEG_FRAME_RATE_30) && (u4FrameRate <= MPEG_FRAME_RATE_60))
            {
                if(!IS_Support4K60Decode())
                {
                    fgRet = FALSE;
                }
            }
        }
        else
        {
            fgRet = FALSE;
        }
    }

    return fgRet;
}
#endif

BOOL VDEC_ChkDivXBonding(VOID)
{
    #if defined(CC_MT5391)
    return IS_DivxSupport();
    #elif defined(VDEC_IS_POST_MT5368)
    return IS_SupportDivXHD();
    #elif defined(CC_MT5360) || defined(VDEC_IS_POST_MT5363)
    return IS_SupportDivX();
    #else
    return FALSE;
    #endif
}

BOOL VDEC_SetMid(UCHAR ucEsId, BOOL fgVdecMidEnable)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfoKeep)
    {
        prVdecEsInfoKeep->fgVdecMidEnable = fgVdecMidEnable;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL VDEC_Notify(UCHAR ucEsId,VDEC_DEC_DECODE_STATUS_T notifyStatus)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(notifyStatus==VDEC_DEC_DECODE_CODEC_NOT_SUPPORT)
    {
        if(prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_CODEC_NOT_SUPPORT)
        {
            //callback, codec not support mpv data
            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
                LOG(3, "ES(%d) Notify Status change, mpeg2 codec not support\n", ucEsId);
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_CODEC_NOT_SUPPORT, 0);
            }
            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_CODEC_NOT_SUPPORT;
        }
        return TRUE;
    }
    if(notifyStatus==VDEC_DEC_DECODE_NO_DATA)
    {
        if(prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_NO_DATA)
        {
            //callback, codec not support mpv data
            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
                LOG(3, "ES(%d) Notify Status change, mpeg2 codec no data \n", ucEsId);
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_NO_DATA, 0);
            }
            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_NO_DATA;
        }
        return TRUE;
    }
    else
    {
        LOG(3, "undefine notify status--%dt\n",notifyStatus);
        return FALSE;
    }
}

void VDEC_SetSrcId(UCHAR ucEsId, UINT8 u1SwdmxInstId )
{

    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->u1SwdmxSrcId=u1SwdmxInstId;

}

VOID VDEC_SetABInfo(UCHAR ucEsId, VDEC_AB_INTO_T *prABInfo)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->rABInfo=*prABInfo;
}

#ifdef CC_VDEC_PRIORITY_ENABLE
void VDEC_WakeUpPendingThread(UCHAR ucEsId)
{
    UINT8 u1temp=0;
    INT32 i4Ret=0;
    SIZE_T zMsgSize = sizeof(VDEC_VLD_STATUS_T);
    VDEC_VLD_STATUS_T u1SendMsg = VDEC_VLD_ACTIVE;
    VDEC_INFO_T *prVdecInfo;
    //VDEC_ES_INFO_T *prVdecEsInfo;
    //VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    //prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecInfo = _VDEC_GetInfo();
    //prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    for(u1temp=0;u1temp<VDEC_MAX_ES;u1temp++)
    {
      if(prVdecInfo->arVdecEsInfoKeep[u1temp].arThreadStatus[prVdecInfo->arVdecEsInfo[u1temp].u4VldId]
       == VDEC_VLD_PENDING)
      {

          _VDEC_LockLoadCyclesMutex(ucEsId);
          if(u1temp != ucEsId)
          {
            //_arVdecThreadWorkingCycles[u1temp] = _arVdec_Thread_Priority[u1temp];
            prVdecInfo->arVdecEsInfo[u1temp].i2VdecThreadWorkingCycles=
                                   prVdecInfo->arVdecEsInfo[u1temp].i2VdecThreadPriority;

          }
          _VDEC_UnlockLoadCyclesMutex(ucEsId);

          i4Ret = x_msg_q_send(prVdecInfo->arVdecEsInfoKeep[u1temp].hVdecScheMsg,
                                 &u1SendMsg, zMsgSize, 255);
          if ((i4Ret != OSR_OK))
          {
            ASSERT(0);
          }
          LOG(3,"%s  send ScheMsg to Es(%d)\n", __FUNCTION__ ,u1temp);
      }
    }
}

BOOL VDEC_SetPriority(UCHAR ucEsId,INT16 Priority)
{
    UINT8 u1temp=0;
    INT32 i4Ret=0;
    SIZE_T zMsgSize = sizeof(VDEC_VLD_STATUS_T);
    VDEC_VLD_STATUS_T u1SendMsg;
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;


    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    ASSERT(ucEsId < VDEC_MAX_ES);

    if((Priority<1)||
        (Priority>100)||
        (prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] == VDEC_VLD_NOUSE)
      )
    {
       LOG(0,"VDEC_SetPriority fail\n");
       return FALSE;
    }

    LOG(3,"VDEC_Play set Es(%d) priority =%d",ucEsId,Priority);

    prVdecEsInfo->i2VdecThreadPriority= Priority;

    _VDEC_LockLoadCyclesMutex(ucEsId);

    for(u1temp=0;u1temp<VDEC_MAX_ES;u1temp++)
    {
       if(prVdecInfo->arVdecEsInfoKeep[u1temp].arThreadStatus[prVdecInfo->arVdecEsInfo[u1temp].u4VldId]
           != VDEC_VLD_NOUSE)
       {
         //_arVdecThreadWorkingCycles[u1temp]= _arVdec_Thread_Priority[u1temp];
         prVdecInfo->arVdecEsInfo[u1temp].i2VdecThreadWorkingCycles=
                                   prVdecInfo->arVdecEsInfo[u1temp].i2VdecThreadPriority;
       }
    }

   _VDEC_UnlockLoadCyclesMutex(ucEsId);

    u1SendMsg = VDEC_VLD_ACTIVE;
    if(prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PENDING)
    {

        i4Ret = x_msg_q_send(prVdecEsInfoKeep->hVdecScheMsg,&u1SendMsg, zMsgSize, 255);
        if ((i4Ret != OSR_OK))
        {
          ASSERT(0);
        }
        LOG(3,"%s  send ScheMsg to Es(%d)\n", __FUNCTION__ ,u1temp);

    }
    VDEC_WakeUpPendingThread(ucEsId);

   return TRUE;
}

static UINT32 u4SedFd=0;
static UINT32 u4RecFd=0;

BOOL VDEC_ThreadSchedule(UCHAR ucEsId)
{
   INT32 i4Ret=0;
   UINT16 u2MsgQIdx;
   SIZE_T zMsgSize = sizeof(VDEC_VLD_STATUS_T);
   VDEC_VLD_STATUS_T u1SendMsg = VDEC_VLD_NOUSE;
   VDEC_VLD_STATUS_T u1RecMsg = VDEC_VLD_NOUSE;
   UINT8 u1temp=0;
   UINT8 u1temp2=0;

   VDEC_INFO_T *prVdecInfo;
   VDEC_ES_INFO_T *prVdecEsInfo;
   VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

   prVdecInfo = _VDEC_GetInfo();
   prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
   prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);


   //prVdecInfo->arVdecEsInfo[ucEsId]->hThreadScheMsg
   LOG(3,"%s Esid(%d) ++++++++ Decoded one frame \n", __FUNCTION__,ucEsId);


   if((prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_FREE_RUN)||
         (prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_FINISH)||
         (prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PAUSE)
        )
   {

     LOG(6," VDEC_ThreadSchedule Esid(%d) free run \n",ucEsId);
     return TRUE;
   }

   for(u1temp=0; u1temp<VDEC_MAX_ES; u1temp++)
   {
      if(u1temp != ucEsId)
      {
         u1temp2 |= prVdecInfo->arVdecEsInfoKeep[u1temp].arThreadStatus[prVdecInfo->arVdecEsInfo[u1temp].u4VldId];
      }
   }

   if(u1temp2 == VDEC_VLD_NOUSE)                //just one vdec thread is active, so no need to schedule
   {

     return TRUE;

   }

   if(prVdecEsInfo->i2VdecThreadWorkingCycles>0)
   {
      if(prVdecEsInfo->fgSendedEos)       ////??? maybe should consider do seek after vdec send EOS
      {
         prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = VDEC_VLD_FINISH;
         _VDEC_LockLoadCyclesMutex(ucEsId);
         prVdecEsInfo->i2VdecThreadWorkingCycles=0;
        _VDEC_UnlockLoadCyclesMutex(ucEsId);

         return TRUE;
      }
      else
      {
         _VDEC_LockLoadCyclesMutex(ucEsId);
         prVdecEsInfo->i2VdecThreadWorkingCycles--;
         _VDEC_UnlockLoadCyclesMutex(ucEsId);

      }

      i4Ret=1;
      for(u1temp=0; u1temp<VDEC_MAX_ES; u1temp++)
      {
         if(u1temp != ucEsId)
         {
            if(prVdecInfo->arVdecEsInfoKeep[u1temp].arThreadStatus[prVdecInfo->arVdecEsInfo[u1temp].u4VldId]
               == VDEC_VLD_ACTIVE)
             {
               i4Ret=0;
               break;
             }
         }
      }

      if((i4Ret)&&(prVdecEsInfo->i2VdecThreadWorkingCycles ==0))
      {

            // reload Thread working cycles;
          _VDEC_LockLoadCyclesMutex(ucEsId);
          for(u1temp=0;u1temp<VDEC_MAX_ES;u1temp++)
          {
              if(prVdecInfo->arVdecEsInfoKeep[u1temp].arThreadStatus[prVdecInfo->arVdecEsInfo[u1temp].u4VldId]
                  == VDEC_VLD_PENDING)
              {
                  //_arVdecThreadWorkingCycles[u1temp]=_arVdec_Thread_Priority[u1temp];
                  prVdecInfo->arVdecEsInfo[u1temp].i2VdecThreadWorkingCycles=
                                   prVdecInfo->arVdecEsInfo[u1temp].i2VdecThreadPriority;
              }
          }
          prVdecEsInfo->i2VdecThreadWorkingCycles=prVdecEsInfo->i2VdecThreadPriority;
          _VDEC_UnlockLoadCyclesMutex(ucEsId);

          LOG(3,"%s Esid(%d) Reload working cycles \n", __FUNCTION__,ucEsId);

         u1SendMsg= VDEC_VLD_ACTIVE;
         for(u1temp=0;u1temp<VDEC_MAX_ES;u1temp++)
         {
             if((prVdecInfo->arVdecEsInfoKeep[u1temp].arThreadStatus[prVdecInfo->arVdecEsInfo[u1temp].u4VldId]
                 == VDEC_VLD_PENDING)&&
                 (u1temp != ucEsId))
            {
               i4Ret = x_msg_q_send(prVdecInfo->arVdecEsInfoKeep[u1temp].hVdecScheMsg,
                                      &u1SendMsg, zMsgSize, 255);
                if ((i4Ret != OSR_OK))
                {
                  ASSERT(0);
                }
                LOG(3,"%s Esid(%d) send ScheMsg to Es thread(%d)u4SedFd(%d)\n",
                    __FUNCTION__ ,ucEsId,u1temp,u4SedFd);
                u4SedFd ++;
            }

         }
      }
      else if(prVdecEsInfo->i2VdecThreadWorkingCycles == 0)
      {

         LOG(3,"%s Esid(%d) waiting to received ScheMsg u4RecFd(%d) \n", __FUNCTION__,ucEsId,u4RecFd);
          prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = VDEC_VLD_PENDING;

         #if 0
         i4Ret= x_msg_q_receive(&u2MsgQIdx, (VOID *)&u1RecMsg, &zMsgSize,
                                &(prVdecEsInfo->hThreadScheMsg), 1, X_MSGQ_OPTION_WAIT);
         #endif

         i4Ret = x_msg_q_receive_timeout(&u2MsgQIdx, (VOID *)&u1RecMsg, &zMsgSize,
                         &(prVdecEsInfoKeep->hVdecScheMsg), 1, VDEC_SCHEDULE_TIMEOUT);
         if(i4Ret == OSR_OK)
         {

            if( prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PENDING)
            {
                prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = u1RecMsg;
            }
            LOG(3,"%s Esid(%d) received ScheMsg u4RecFd(%d)\n", __FUNCTION__,ucEsId,u4RecFd);
            u4RecFd++;

         }
         else if (i4Ret == OSR_TIMEOUT)
         {

           LOG(0,"VDEC Schedule time out \n");
            prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = VDEC_VLD_FREE_RUN;

         }
         else
         {
             ASSERT(0);
         }
     }
     else
     {

     }
   }
   else
   {
     if(( prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] == VDEC_VLD_ACTIVE)||
         ( prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] == VDEC_VLD_PENDING))
         {
          i4Ret = x_msg_q_receive_timeout(&u2MsgQIdx, (VOID *)&u1RecMsg, &zMsgSize,
                         &(prVdecEsInfoKeep->hVdecScheMsg), 1, VDEC_SCHEDULE_TIMEOUT);
          if(i4Ret == OSR_OK)
          {

            if( prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId]== VDEC_VLD_PENDING)
            {
                prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = u1RecMsg;
            }
            LOG(3,"%s Esid(%d) received ScheMsg u4RecFd(%d)\n", __FUNCTION__,ucEsId,u4RecFd);
            u4RecFd++;

          }
          else if (i4Ret == OSR_TIMEOUT)
          {

            LOG(0,"VDEC Schedule time out \n");
            prVdecEsInfoKeep->arThreadStatus[prVdecEsInfo->u4VldId] = VDEC_VLD_FREE_RUN;

          }
          else
          {
             ASSERT(0);
          }

         }
   }

   return TRUE;
}
#endif
/************************************/
//Add for rvu STEP->STOP patch
//step->stop cmd seq: play->pause->step
//swdmx send all I frame,vdec drop all
//because of normal play, make B2R can not notify seq chg done
//so,mw can not send "pause" ...... mtk40343
/************************************/
EXTERN BOOL VDEC_SetSkipMode(UCHAR ucEsId, BOOL fgSkipMode)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if(NULL == prVdecEsInfo)
    {
        return FALSE;
    }
    else
    {
        prVdecEsInfo->fgRVUSkip = fgSkipMode;
        if(fgSkipMode)
        {
            prVdecEsInfo->ucRVUSkipMode = IMode;
        }
        else
        {
            prVdecEsInfo->ucRVUSkipMode = IPBMode;
        }
        LOG(3,"VDEC set mode (%d)\n",prVdecEsInfo->ucRVUSkipMode);
    }
    return TRUE;
}


void VDEC_SetMVCCmpare(UCHAR ucEsId, BOOL fgParam)
{
   VDEC_ES_INFO_T *prVdecEsInfo;
   prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
   prVdecEsInfo->fgIsMVCPtsCmp = fgParam;
}
#ifdef CC_REALD_3D_SUPPORT
void VDEC_Set3DUserCtrl(UCHAR ucEsId, UINT8 u1Param)
{
   VDEC_ES_INFO_T *prVdecEsInfo;

   prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

   prVdecEsInfo->u13DUsrCtrlMode = u1Param;
}

#endif

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
void  VDEC_SetCrcGolden(UCHAR ucEsId, UINT32 uCrcGolden)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    prVdecEsInfo->u4CrcGolden = uCrcGolden;
    prVdecEsInfo->fgAutoCrcCk= TRUE;

}

INT32 VDEC_SetAutoTestDecNfy(UCHAR ucEsId, UINT32 prDecNfy, void *pvTag)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if (prDecNfy == NULL)
    {
        return (MPV_DRV_INV_SET_INFO);
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pfDecAutoTestNfyFct = (PFN_VDEC_DEC_NFY_FCT)prDecNfy;
    prVdecEsInfoKeep->pvDecAutoTestNfyTag = pvTag;

    return (VDEC_DRV_OK);
}

#ifdef VDEC_TIME_PROFILE
VOID VDEC_SetAutoTestCurFileDir(UCHAR ucEsId, CHAR *ucAutoPlayCurFileDir)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    x_memset(&(prVdecEsInfo->ucAutoPlayCurFileDir[0]), 0x0, sizeof(prVdecEsInfo->ucAutoPlayCurFileDir));
    x_memcpy(&(prVdecEsInfo->ucAutoPlayCurFileDir[0]), ucAutoPlayCurFileDir, sizeof(prVdecEsInfo->ucAutoPlayCurFileDir));

    return;
}
VOID VDEC_SetAutoTestCurFileName(UCHAR ucEsId, CHAR *ucAutoPlayCurFileName)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    x_memset(&(prVdecEsInfo->ucAutoPlayCurFileName[0]), 0x0, sizeof(prVdecEsInfo->ucAutoPlayCurFileName));
    x_memcpy(&(prVdecEsInfo->ucAutoPlayCurFileName[0]), ucAutoPlayCurFileName, sizeof(prVdecEsInfo->ucAutoPlayCurFileName));

    return;
}
#endif

#endif

#ifdef VDEC_CRC_TEST
BOOL VDEC_IsTestCRC(UCHAR ucEsId)
{
    ASSERT(ucEsId < VDEC_MAX_ES);
    return _fgCRCTest;
}

VOID VDEC_SetTestCRC(UCHAR ucEsId, BOOL fgTestOrNot)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->u4CrcResult = 0;
    _fgCRCTest = fgTestOrNot;
    return;
}
#endif
#ifndef VDEC_PUSH_SUPPORT
BOOL _VPUSH_Play(VOID* prdec)
{
    UNUSED(prdec);
    return TRUE;
}
BOOL _VPUSH_Stop(VOID* prdec)
{
    UNUSED(prdec);
    return TRUE;
}
BOOL _VPUSH_StopSync(VOID* prdec)
{
    UNUSED(prdec);
    return TRUE;
}
BOOL _VPUSH_PutData(VOID* prdec, VDEC_BYTES_INTO_T *prBytesInfo)
{
    UNUSED(prdec);
    UNUSED(prBytesInfo);
    return TRUE;
}
BOOL _VPUSH_PutDataSync(VOID* prdec, VDEC_BYTES_INTO_T *prBytesInfo)
{
    UNUSED(prdec);
    UNUSED(prBytesInfo);
    return TRUE;
}
BOOL _VPUSH_PutDataDone(VOID* prdec, UINT32 u4Tag)
{
    UNUSED(prdec);
    UNUSED(u4Tag);
    return TRUE;
}
VOID* _VPUSH_AllocVideoDecoder(ENUM_VDEC_FMT_T eFmt, UCHAR ucVdecId)
{
    UNUSED(eFmt);
    return NULL;
}
VOID _VPUSH_ReleaseVideoDecoder(VOID* prdec)
{
    UNUSED(prdec);
    return;
}
VOID _VPUSH_DecodeInit(VOID)
{
    return;
}

BOOL _VPUSH_DecodeReset(VOID* prdec)
{
    UNUSED(prdec);
    return TRUE;
}

VOID _VPUSH_DecodeDone(UCHAR ucEsId, VOID *prPicNfyInfo)
{
    UNUSED(ucEsId);
    UNUSED(prPicNfyInfo);
    return;
}
BOOL _VPUSH_RegCb(VOID* prdec, VDEC_PUSH_CB_T *prVdecPushCb)
{
    UNUSED(prdec);
    UNUSED(prVdecPushCb);
    return TRUE;
}
BOOL _VPUSH_GetVFifoInfo(VDEC_VFIFO_INTO_T *prVfifoInfo)
{
    UNUSED(prVfifoInfo);
    return TRUE;
}
BOOL _VPUSH_SetInfo(VOID* prdec, VDEC_SET_INTO_T *prVdecSetInfo)
{
    UNUSED(prdec);
    UNUSED(prVdecSetInfo);
    return TRUE;
}
BOOL _VPUSH_GetInfo(VOID* prdec, VDEC_GET_INTO_T *prVdecGetInfo)
{
    UNUSED(prdec);
    UNUSED(prVdecGetInfo);
    return TRUE;
}
BOOL _VPUSH_SetPushType(VOID* prdec, ENUM_VDEC_PUSH_FMT_T ePushFmt)
{
    UNUSED(prdec);
    UNUSED(ePushFmt);
    return TRUE;
}
VOID _VPUSH_SetPicSz(VOID* prdec, UINT32 u4Width, UINT32 u4Height,UINT32 u4FrameRate)
{
    UNUSED(prdec);
    UNUSED(u4Width);
    UNUSED(prdec);
    UNUSED(u4FrameRate);
    return;
}
BOOL _VPUSH_SetRptr(UCHAR ucEsId, UINT32 u4Rp, UINT32 u4FrameAddr)
{
    UNUSED(ucEsId);
    UNUSED(u4Rp);
    UNUSED(u4FrameAddr);
    return TRUE;
}
BOOL _VPUSH_AllocVFifo(VOID* prdec,
    UINT32 *pu4BufSA, UINT32 u4Size, ENUM_VDEC_FMT_T eFmt)
{
    UNUSED(prdec);
    UNUSED(pu4BufSA);
    UNUSED(u4Size);
    UNUSED(eFmt);
    return TRUE;
}
BOOL _VPUSH_FreeVFifo(VOID* prdec, UINT32 u4BufSA)
{
    UNUSED(prdec);
    UNUSED(u4BufSA);
    return TRUE;
}
#endif
#ifdef CC_ITIMING_ALWAYS
void VDEC_SetFixFHDDisplay(UCHAR ucEsId, BOOL fgInterlace)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfoKeep->fgFixFHDInterlaceDisplay = fgInterlace;

}
#endif

BOOL  VDEC_SetRestartAudioPts(UCHAR ucEsId, BOOL fgEnable)
{
    VDEC_ES_INFO_T *prVdecEsInfo;

    ASSERT(ucEsId < VDEC_MAX_ES);

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfo->fgSetStartPts = (!fgEnable);
    
    return TRUE;
}

BOOL VDEC_TryLockValidSrc(UCHAR *pucEsId)
{
    BOOL fgRetvl = FALSE;
#ifndef CC_USE_DDI
    INT32 i4Ret = OSR_OK;
#endif
    UCHAR ucEsId;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    UNUSED(prVdecEsInfoKeep);

    if (!pucEsId)
    {
        LOG(2, "VDEC_TryLockValidSrc Null pointer\n");
        return FALSE;
    }

    if (*pucEsId < VDEC_MAX_ES)
    {
#ifndef CC_USE_DDI
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(*pucEsId);
        i4Ret = x_sema_lock(prVdecEsInfoKeep->hSrcCtrlMutex, X_SEMA_OPTION_NOWAIT); 

        if (OSR_WOULD_BLOCK == i4Ret)
        {
        }
        else if (OSR_OK == i4Ret)
        {
            fgRetvl = TRUE;
            goto EXIT_PORT;
        }
        
        *pucEsId = 0xFF;
        fgRetvl = TRUE;
        goto EXIT_PORT;
#else
        UINT32 u4DecMode = VDEC_ST_MAX;
        
        u4DecMode = MPV_GetDecMode(*pucEsId);
        if ((VDEC_ST_STOP!=u4DecMode) && (VDEC_ST_MAX!=u4DecMode))
        {
        }
        else
        {
            fgRetvl = TRUE;
            goto EXIT_PORT;
        }
#endif
    }

    *pucEsId = 0xFF;
    for (ucEsId=0; VDEC_MAX_ES>ucEsId; ucEsId++)
    {
#ifndef CC_USE_DDI
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        i4Ret = x_sema_lock(prVdecEsInfoKeep->hSrcCtrlMutex, X_SEMA_OPTION_NOWAIT); 

        if (OSR_WOULD_BLOCK == i4Ret)
        {
        }
        else if (OSR_OK == i4Ret)
        {
            *pucEsId = ucEsId;
            fgRetvl = TRUE;
            goto EXIT_PORT;
        }
#else
        UINT32 u4DecMode = VDEC_ST_MAX;

        u4DecMode = MPV_GetDecMode(ucEsId);
        if ((VDEC_ST_STOP!=u4DecMode) && (VDEC_ST_MAX!=u4DecMode))
        {
        }
        else
        {
            *pucEsId = ucEsId;
            fgRetvl = TRUE;
            goto EXIT_PORT;
        }
#endif
    }
    
EXIT_PORT:
    LOG(2, "%s:%u (0x%X 0x%X) Ret(%d)\n",
            __FUNCTION__, __LINE__, pucEsId, *pucEsId, fgRetvl);
            
    return fgRetvl;
}

BOOL VDEC_UnlockSrc(UCHAR ucEsId)
{
    BOOL fgRetvl = TRUE;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

#ifndef CC_USE_DDI
    if (ucEsId < VDEC_MAX_ES)
    {
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    }
    else
    {
        fgRetvl = FALSE;
        goto EXIT_PORT;
    }
    
    VERIFY(x_sema_unlock(prVdecEsInfoKeep->hSrcCtrlMutex) == OSR_OK);

EXIT_PORT:
    LOG(2, "%s Es%d\n", __FUNCTION__, ucEsId);
#else
    UNUSED(prVdecEsInfoKeep);
#endif

    return fgRetvl;
}

#if BDP_LOG_STYLE
void VDec_InitModeLogInfo(void)
{
    UCHAR ucIdx;
    for (ucIdx = 0; ucIdx < VDEC_MAX_ES; ucIdx++)
    {
        _arVDecModeLogInfo[ucIdx].u4LogLevel = 0;
        //_arVDecModeLogInfo[ucIdx].eDecCtrl = VID_DEC_CTRL_RESET;
        _arVDecModeLogInfo[ucIdx].fgDecModeFin = FALSE;
        _arVDecModeLogInfo[ucIdx].u2DecMode = 0xFF;     //MPV_IDLE_MODE;
        //_arVDecModeLogInfo[ucIdx].e_speed_type = VID_DEC_SPEED_TYPE_STEP_REVERSE;
        _arVDecModeLogInfo[ucIdx].u2OriginalWidth = 0;
        _arVDecModeLogInfo[ucIdx].u2OriginalHeight = 0;
        _arVDecModeLogInfo[ucIdx].u2DesiredWidth = 0;
        _arVDecModeLogInfo[ucIdx].u2DesiredHeight = 0;
    }
}

VDEC_MODELOG_INFO_T* VDec_GetModeLogInfo(UCHAR ucEsId)
{
    UCHAR ucEsIdMap;
    //ucEsIdMap = (ucEsId==4)?1:0;
    //ucEsIdMap = _arVDecLogMap[ucEsId];
    ucEsIdMap = ucEsId;
    return &_arVDecModeLogInfo[ucEsIdMap];
}

int VDecPrintf(UCHAR ucEsId, UINT32 ucLevel, CHAR *format, ...)
{
    va_list ap;
    INT32 nRet = 0;
    CHAR buffer[512];
    UCHAR ucEsIdMap;
    ucEsIdMap = ucEsId;

    if( ucLevel & _arVDecModeLogInfo[ucEsIdMap].u4LogLevel )
    {
        //x_sprintf (buffer, "Es%d ", ucEsId);
        va_start (ap, format);
        x_vsnprintf (buffer,512, format, ap);
        LOG(1,buffer);
        va_end (ap);
    }
    return nRet;
}
#endif

BOOL VDEC_ISH264_MVC(void)
{
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
    return _VDEC_ISH264_MVC();
#endif	
#else
    return 0;
#endif
}

/**
    Add for DTV MHEG5 I frame case. 
    Audio playing abnormally when VDEC in play I frm status: audio will do Av sync when rsv video mode change. In fact, we should not do it.
 */
BOOL VDEC_IsExistDtvIFrmCase(VOID)
{
    BOOL fgRet = FALSE;
    UCHAR ucIdx = 0;
    VDEC_ES_INFO_T *prVDecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    for (ucIdx=0; ucIdx<VDEC_MAX_ES; ucIdx++)
    {
        prVDecEsInfo = _VDEC_GetEsInfo(ucIdx);
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucIdx);
        

        LOG(6, "Es(%d), fgMMPlayback(%d), eCurState(%d)\n",
                ucIdx, prVDecEsInfo->fgMMPlayback, prVdecEsInfoKeep->eCurState);

        if ((FALSE==prVDecEsInfo->fgMMPlayback) &&
            (VDEC_ST_PLAY_I_FRAME==prVdecEsInfoKeep->eCurState))
        {
            fgRet = TRUE;

            break;
        }
    }

    return fgRet;
}

BOOL VDEC_NotifyStop(UCHAR ucEsId)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if ((prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY) || (!prVdecEsInfoKeep->fgVPushFBGFromInst))
    {
        LOG(1, "VDEC%d is in stop status\n", ucEsId);
        return FALSE;
    }

#ifdef CC_SUPPORT_MTK_ANDROID_RM
    LOG(0, "VDEC%d is in wrong state(%d), enter error handle\n", ucEsId, prVdecEsInfoKeep->eCurState);
    {
        UINT32 u4WaitStopCnt = 0;
        do {
            x_thread_delay(100);
            ++u4WaitStopCnt;
            if (u4WaitStopCnt > 10)
            {
                break;
            }
        } while(prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY);
    }

    if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY)
    {
        LOG(0, "VDEC%d is in wrong state(%d), force stop!!\n", ucEsId, prVdecEsInfoKeep->eCurState);
        ASSERT(prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY);
        VDEC_Stop(ucEsId);
    }
    return TRUE;    
#else

    if (!prVdecEsInfoKeep->pfDecNfyFct)
    {
        LOG(0, "pfDecNfyFct is NULL!!!\n");
        ASSERT(prVdecEsInfoKeep->pfDecNfyFct);
        VDEC_Stop(ucEsId);
        return TRUE;    
    }

    if (prVdecEsInfoKeep->hWaitStopSema)
    {
        x_sema_lock(prVdecEsInfoKeep->hWaitStopSema, X_SEMA_OPTION_NOWAIT);
    }
    else
    {
        ASSERT(prVdecEsInfoKeep->hWaitStopSema);
    }

    LOG(1, "VDEC%d need release first\n", ucEsId);
    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
        VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_FORCE_STOP, 0);
    // wait stop finish
    if (prVdecEsInfoKeep->hWaitStopSema)
    {
        VERIFY(x_sema_lock(prVdecEsInfoKeep->hWaitStopSema, X_SEMA_OPTION_WAIT) == OSR_OK);
    }
    else
    {
        // error handle...
        LOG(0, "hWaitStopSema is NULL!!!\n");
        x_thread_delay(100);
    }
    LOG(1, "VDEC%d stop done\n", ucEsId);
#endif

    return TRUE;    
}


void VDEC_YUV2YCbCr_Resize(VDEC_YUV_RESIZE_INFO_T *pYUVRezInfo)
{ 
    VDEC_YUV_RESIZE_INFO_T rPreScaleFrm;
    UINT32 u4SrcYaddr;
    UINT32 u4TgtYaddr,u4TgtCaddr;
    UINT32 u4RZWorkingBuf;
    FBM_POOL_T* pFBMPoolInfo; 
    RZ_JPG_SCL_PARAM_SET_T prImgrzParam;
    
    x_memcpy(&rPreScaleFrm,pYUVRezInfo,sizeof(VDEC_YUV_RESIZE_INFO_T));

    pFBMPoolInfo = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PVR2);
    u4RZWorkingBuf = pFBMPoolInfo->u4Addr;

    u4SrcYaddr = rPreScaleFrm.u4SrcAddr;
    u4TgtYaddr = rPreScaleFrm.u4TgtYAddr;
    u4TgtCaddr = rPreScaleFrm.u4TgtCAddr;

    {
        x_memset(&prImgrzParam,0,sizeof(RZ_JPG_SCL_PARAM_SET_T));
        prImgrzParam.u4IsRsIn = 1;  // block|raster based input
        prImgrzParam.u4YSrcBufLen = rPreScaleFrm.u4YPitchSize;
        prImgrzParam.u4CSrcBufLen = rPreScaleFrm.u4UVPitchSize;
        prImgrzParam.u4YSrcVOffset = 0;
        prImgrzParam.u4YSrcHOffset = 0;
        prImgrzParam.u4YSrcW = rPreScaleFrm.u4PWidth;//prImgrzParam->u4YSrcHOffset;
        prImgrzParam.u4YSrcH = rPreScaleFrm.u4PHeight;
        prImgrzParam.u4CbSrcVOffset = 0;
        prImgrzParam.u4CbSrcHOffset = 0;
        prImgrzParam.u4CbSrcW = (rPreScaleFrm.u4PWidth>>1);//+prImgrzParam->u4CbSrcHOffset;
        prImgrzParam.u4CbSrcH = (rPreScaleFrm.u4PHeight>>1);
        prImgrzParam.u4CrSrcVOffset = 0;
        prImgrzParam.u4CrSrcHOffset = 0;
        prImgrzParam.u4CrSrcW = (rPreScaleFrm.u4PWidth>>1);//+prImgrzParam->u4CrSrcHOffset;
        prImgrzParam.u4CrSrcH = (rPreScaleFrm.u4PHeight>>1);
        prImgrzParam.u4ColorComp = 7;
        prImgrzParam.u4IsRsOut = 1; // block|raster based output
        prImgrzParam.u4OutSwap = 0;         // 6--block out
        prImgrzParam.u4IsVdo2Osd = 0;// output in VDO format ?
        prImgrzParam.u4YTgCM = 0;
        prImgrzParam.u4OutMode = E_RZ_VDO_OUTMD_420;
        prImgrzParam.u4YTgBufLen = rPreScaleFrm.u4YPitchSize;
        prImgrzParam.u4CTgBufLen = rPreScaleFrm.u4YPitchSize;
        prImgrzParam.u4YTgW = (rPreScaleFrm.u4PWidth > 4)? rPreScaleFrm.u4PWidth : 4;
        prImgrzParam.u4YTgH = rPreScaleFrm.u4PHeight;
        prImgrzParam.u4CTgW = prImgrzParam.u4YTgW >> 1;
        prImgrzParam.u4CTgH = prImgrzParam.u4YTgH >> 1;
        prImgrzParam.u4IsFstBl = 0;
        prImgrzParam.u4IsLstBl = 0;
        prImgrzParam.u4IsJpgRzOn = 0;
        prImgrzParam.u4JpgVfacY = 1;
        prImgrzParam.u4JpgVfacCb = 0;
        prImgrzParam.u4JpgVfacCr = 0;
        prImgrzParam.fgBlassign = TRUE;
        prImgrzParam.u4YSrcBase1 = u4SrcYaddr;
        prImgrzParam.u4YSrcBase2 = u4SrcYaddr;    // y2
        prImgrzParam.u4CbSrcBase1 = u4SrcYaddr + rPreScaleFrm.u4YPitchSize*rPreScaleFrm.u4PHeight;  // cb1
        prImgrzParam.u4CbSrcBase2 = u4SrcYaddr + rPreScaleFrm.u4YPitchSize*rPreScaleFrm.u4PHeight;  // cb2
        prImgrzParam.u4CrSrcBase1 = prImgrzParam.u4CbSrcBase1 +  (rPreScaleFrm.u4UVPitchSize*rPreScaleFrm.u4PHeight)/2;  // cr1
        prImgrzParam.u4CrSrcBase2 = prImgrzParam.u4CbSrcBase2 + (rPreScaleFrm.u4UVPitchSize*rPreScaleFrm.u4PHeight)/2;  // cr2
        LOG(2,"====YSr %x U%x V%x====\n", prImgrzParam.u4YSrcBase1,prImgrzParam.u4CbSrcBase1,prImgrzParam.u4CrSrcBase1);

        prImgrzParam.u4JpgTempAddr = u4RZWorkingBuf;
        prImgrzParam.u4YTgBase = u4TgtYaddr;
        prImgrzParam.u4CTgBase = u4TgtCaddr;
        LOG(2,"====YTg %x, Cb %x ====\n", prImgrzParam.u4YTgBase,prImgrzParam.u4CTgBase );
        IMGRZ_Lock();
        IMGRZ_ReInit();
        IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
        IMGRZ_Scale((void *)(&prImgrzParam));
        IMGRZ_Flush();
        IMGRZ_Wait();
        IMGRZ_Unlock();
    }
}

void VDEC_INTER_RESIZE(VDEC_I_RESIZE_INFO_T *ResizeFRMT,BOOL fgRepeatTop)
{  

	UCHAR ucRzId;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    VDEC_I_RESIZE_INFO_T rPreScaleFrm;
    RZ_VDO_SCL_PARAM_SET_T rScaleParamV;
    UINT32 u4YAddr,u4CAddr;
	ucRzId = 0;
	x_memcpy(&rPreScaleFrm,ResizeFRMT,sizeof(VDEC_I_RESIZE_INFO_T));

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(rPreScaleFrm.ucFbgId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(rPreScaleFrm.ucFbgId, rPreScaleFrm.ucFbId);
    if(prFbmSeqHdr && prFbmPicHdr)
    {
    
		LOG(1,"Resize FbgId %d FbId %d Top(%d)\n ",rPreScaleFrm.ucFbgId,rPreScaleFrm.ucFbId,fgRepeatTop);
        x_memset(&rScaleParamV, 0, sizeof(RZ_VDO_SCL_PARAM_SET_T));
        rScaleParamV.u4VdoCbCrSwap =0;  //todo2, set center position by aspect ratio?
        rScaleParamV.u4ABlend=0x80;
        rScaleParamV.u4IsRsIn = 1;//;(prFbmSeqHdr->fgRasterOrder) ? 1 : 0;
        rScaleParamV.u4InMode = E_RZ_VDO_OUTMD_420;
		//--------------  get source target
		FBM_GetFrameBufferAddr(rPreScaleFrm.ucFbgId, rPreScaleFrm.ucFbId, &u4YAddr, &u4CAddr);
		if(fgRepeatTop)
		{
             rScaleParamV.u4YSrcBase=u4YAddr;
             rScaleParamV.u4CSrcBase=u4CAddr;
		}
		else
		{
			rScaleParamV.u4YSrcBase=u4YAddr+64;
			rScaleParamV.u4CSrcBase=u4CAddr+64;
		}
        rScaleParamV.u4YSrcBufLen = 128;//prFbmPicHdr->u4PicWidthPitch;
        rScaleParamV.u4CSrcBufLen = 128;
        rScaleParamV.u4YSrcHOffset = 0;//prFbmPicHdr->fgCropping ? prFbmPicHdr->u4CropX : 0;
        rScaleParamV.u4YSrcVOffset = 0;//prFbmPicHdr->fgCropping ? prFbmPicHdr->u4CropY : 0;
        rScaleParamV.u4YSrcW = 64;//prFbmPicHdr->fgCropping ? prFbmPicHdr->u4CropWidth : prFbmPicHdr->u4PicWidth;
        rScaleParamV.u4YSrcH = (prFbmPicHdr->u4PicHeight/2)*(prFbmPicHdr->u4PicWidthPitch/64);//prFbmPicHdr->fgCropping ? prFbmPicHdr->u4CropHeight : prFbmPicHdr->u4PicHeight;
        rScaleParamV.u4CSrcHOffset = 0;//rScaleParamV.u4YSrcHOffset >> 1;
        rScaleParamV.u4CSrcVOffset = 0;//rScaleParamV.u4YSrcVOffset >> 1;
        rScaleParamV.u4CSrcW = rScaleParamV.u4YSrcW >> 1;
        rScaleParamV.u4CSrcH = rScaleParamV.u4YSrcH >> 1;
		//--------------  get resize target
        //FBM_GetFrameBufferAddr(rPreScaleFrm.ucFbgId, rPreScaleFrm.ucFbId, &rScaleParamV.u4YTgBase, &rScaleParamV.u4CTgBase);
//		rScaleParamV.u4YTgBase = rScaleParamV.u4YSrcBase + 64;
//		rScaleParamV.u4CTgBase = rScaleParamV.u4CSrcBase + 64;

		if(fgRepeatTop)
		{
             rScaleParamV.u4YTgBase=u4YAddr+64;
             rScaleParamV.u4CTgBase=u4CAddr+64;
		}
		else
		{
			rScaleParamV.u4YTgBase=u4YAddr;
			rScaleParamV.u4CTgBase=u4CAddr;
		}

		rScaleParamV.u4YTgW = rScaleParamV.u4YSrcW;
        rScaleParamV.u4YTgH = rScaleParamV.u4YSrcH;
        //use resize width as pitch when auto seamless
        rScaleParamV.u4YTgBufLen= 128;//rScaleParamV.u4YSrcBufLen;
        rScaleParamV.u4CTgBufLen=128;
        rScaleParamV.u4IsRsOut =  1;//rScaleParamV.u4IsRsIn;

        rScaleParamV.u4OutMode = E_RZ_VDO_OUTMD_420;
        rScaleParamV.u4IsVdo2Osd = 0;
        rScaleParamV.u4YTgCM = E_RZ_VDO_OUTMD_420;
/*
        if(rScaleParamV.u4YTgW&0x1)
        {
            rScaleParamV.u4YTgW--;
        }

        if(rScaleParamV.u4YTgH&0x1)
        {
            rScaleParamV.u4YTgH--;
        }
*/
        rScaleParamV.u4CTgW=rScaleParamV.u4YTgW>>1;
        rScaleParamV.u4CTgH=rScaleParamV.u4YTgH>>1;


            //don't need to handle centralization, APP will set source region after vdp callback. 
        rScaleParamV.u4YTgHOffset = 0;//rScaleParamV.u4YSrcHOffset;//(prFbmSeqHdr->u2HSize - prFbmSeqHdr->u4ResizeWidth) >> 1;
        rScaleParamV.u4YTgVOffset = 0;//rScaleParamV.u4YSrcVOffset;//(prFbmSeqHdr->u2VSize - prFbmSeqHdr->u4ResizeHeight) >> 1;

        rScaleParamV.u4OutSwap = 0;//(rScaleParamV.u4IsRsOut) ? 0 : 6;
        rScaleParamV.u4SrcSwap = 0;//(prFbmSeqHdr->fgRasterOrder) ? 0 : 6;

//--------------  start resizing
/*
		if(!prFbmSeqHdr->fgProgressiveSeq)
        {
        	LOG(2,"Inter Resize1\n");
            rScaleParamV.u4FieldTypeEnable = 1;
            rScaleParamV.u4FieldType = E_RZ_VDO_TOP_FILED;
            rScaleParamV.u4YSrcH >>= 1;
            rScaleParamV.u4CSrcH >>= 1;
            rScaleParamV.u4YTgH >>= 1;
            rScaleParamV.u4CTgH >>= 1;
        }
		*/
		LOG(6,"##Resize Para %d, %d\n",rScaleParamV.u4IsRsIn,rScaleParamV.u4InMode);
		LOG(6,"##Sorce Para\n");
		LOG(6,"##YBase 0x%x CBase 0x%x Ybuflen 0x%x YHoff 0x%x YVoff 0x%x\n",rScaleParamV.u4YSrcBase,
			rScaleParamV.u4CSrcBase,rScaleParamV.u4YSrcBufLen,rScaleParamV.u4YSrcHOffset,rScaleParamV.u4YSrcVOffset);
		LOG(6,"##YSrcW %d YSrcH %d CSrcW %d CSrcH %d CHoff 0x%x CVoff 0x%x\n",rScaleParamV.u4YSrcW
			,rScaleParamV.u4YSrcH,rScaleParamV.u4CSrcW,rScaleParamV.u4CSrcH,rScaleParamV.u4CSrcHOffset, rScaleParamV.u4CSrcVOffset);
		LOG(6,"##Target Para\n");
		LOG(6,"##YBase 0x%x CBase 0x%x Ybuflen 0x%x YHoff 0x%x YVoff 0x%x\n",rScaleParamV.u4YTgBase,
			rScaleParamV.u4CTgBase,rScaleParamV.u4YTgBufLen,rScaleParamV.u4YTgHOffset,rScaleParamV.u4YTgVOffset);
		LOG(6,"##YTgW %d YTgH %d CTgW %d CTgH %d \n",rScaleParamV.u4YTgW
			,rScaleParamV.u4YTgH,rScaleParamV.u4CTgW,rScaleParamV.u4CTgH);

		//frame or top 
			
		IMGRZ_Lock_Ex((UINT32)ucRzId);
		IMGRZ_ReInit_Ex((UINT32)ucRzId);
		IMGRZ_SetScaleOpt_Ex((UINT32)ucRzId, E_RZ_INOUT_TYPE_VDOMD);
		IMGRZ_Scale_Ex((UINT32)ucRzId, (void *)(&rScaleParamV));
		IMGRZ_Flush_Ex((UINT32)ucRzId);
		IMGRZ_Wait_Ex((UINT32)ucRzId);
		IMGRZ_Unlock_Ex((UINT32)ucRzId);

    }
}


//EXTERN VOID MMPlaybackSpeedTestAddTimeStamp_cmd(E_TIME_STAMP_TYPE eType);
VOID MMPlaybackSpeedTestAddTimeStamp(E_TIME_STAMP_TYPE eType)
{
    //MMPlaybackSpeedTestAddTimeStamp_cmd(eType);
    return;
}

void VDEC_VP9LogOption(const CHAR* pTypeStr, const CHAR* pArg1, const CHAR* pArg2)
{
#ifndef CC_VDEC_VP9_DISABLE
    UCHAR ucVp9LogType;
    UCHAR ucOnOff;
    if (memcmp("io", pTypeStr, 2) == 0) 
    {
        ucVp9LogType = VP9_LOG_IO;        
    }
    else if(memcmp("reg", pTypeStr, 3) == 0)
    {
        ucVp9LogType = VP9_LOG_RISC;
    }
    else if(memcmp("dbg", pTypeStr, 3) == 0)
    {
        ucVp9LogType = VP9_LOG_DEBUG;
    }
    else if(memcmp("vbs", pTypeStr, 3) == 0)
    {
        ucVp9LogType = VP9_LOG_VERBOSE;
    }
    else if(memcmp("chk", pTypeStr, 3) == 0)
    {
        _VDEC_Vp9ChkStatus();
        return;
    }
    else
    {
        LOG(0, "[vp9] wrong parameters!\n");
        return;
    }


    if(memcmp("on", pArg1, 2) == 0)
    {
        ucOnOff = 1;
    }
    else if(memcmp("off", pArg1, 3) == 0)
    {
        ucOnOff = 0;
    }
    else
    {
        LOG(0, "[vp9] wrong parameters!! only support \"on\"/\"off\"\n");
        return;
    }

    LOG(0, "[vp9] %s log -> %s\n", pTypeStr, (ucOnOff ? "on" : "off"));
    _VDEC_Vp9SetParam(0, 0xFF, ucVp9LogType, ucOnOff, 0);
#else
    return;
#endif
}


