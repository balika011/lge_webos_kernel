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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_flw.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_flw.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
#include "./hw_common/vdec_hw_common.h"
#include "vdec_if.h"
#include "vdec_debug.h"
#include "vdec_esm.h"
#include "vdec_type.h"
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
#include "vdec_h264api.h"
#endif
#endif
#ifndef CC_VDEC_H265_DISABLE
#include "./h265/vdec_h265_api.h"
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
#ifndef CC_VDEC_RAW_DISABLE
#include "vdec_raw.h"
#endif
#ifndef CC_VDEC_MPV_DISABLE
#include "mpv_decode.h"
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
#include "mpv_if.h"
#include "stc_drvif.h"
#include "fbm_drvif.h"
#include "b2r_drvif.h"
#include "x_timer.h"
#include "x_mid.h"
#include "x_bim.h"
#include "imgrz_if.h"
#include "gfx_if.h"
#ifdef VDEC_VENC_PATH
#include "gfx_drvif.h"
#include "imgrz_drvif.h"
#include "venc_drvif.h"
#include "venc_util_drvif.h"
#endif
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif
#include "dmx_if.h"
#ifdef CC_MT5392B
#include "vdp_drvif.h"
#endif
#ifndef CC_VDEC_AVS_DISABLE
#include "vdec_avsapi.h"
#endif
#ifdef CC_REALD_3D_SUPPORT
#include "gfx_drvif.h"
#endif
#include "x_chip_id.h"
#ifdef VDEC_TIME_PROFILE
#include <string.h>
#endif
#if defined(CC_TRUSTZONE_SUPPORT) && (defined(CC_TVP_SUPPORT)||defined(CC_SVP_SUPPORT))
#include "tz_if.h"
#endif
#ifdef CC_SUPPORT_VDEC_PREPARSE
#include "mpv_parser.h"
#endif

#ifdef CC_MAPLE_CUST_DRV
#define CC_IMGRZ_DYNAMIC_REQ
#endif
#ifdef __KERNEL__
#include <linux/time.h>
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#ifdef VDEC_TIME_PROFILE
#define DRV_PREV 0
#define HDR_PARSE 1
#define HW_DEC 2
#define MAX_SET 3
#endif
#define ON      1
#define OFF    0
#define _MAKE_32X(val) ((((val) + 0x1F) >> 5) << 5)
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
static BOOL fgPrevFbAllocateFail = FALSE;
#endif
static BOOL _fgWaitFbgChg = FALSE;
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
static BOOL fgErrorHappened = FALSE;
#endif

#ifdef VDEC_VENC_PATH
typedef struct
{
    BOOL         fgOn;
    VENC_HANDLE  hVEnc;
    VENC_BUFMGR_BASE_T * ptFrmBuffMgr;
    #ifdef VENC_FRAME_CTRL_BY_DRV
    VENC_FILTER_T *ptFilter;
    #endif
    //venc input source fmt
    UINT32 u4Pitch;
    UINT32 u4Width;
    UINT32 u4Height;

} VDEC_2_VENC_T;

static VDEC_2_VENC_T _arVdec2VencFlow[VDEC_MAX_ES];
#endif

#ifdef __MODEL_slt__
EXTERN BOOL fgSltWatchPts;
EXTERN UINT32 u4SltPts;
EXTERN BOOL fgSltNotSendPic;
#endif

#ifndef LINUX_TURNKEY_SOLUTION
//#define VDEC_THUMBNAIL_ADDR_CHECK
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void _VdecGetThumbnail(UCHAR ucEsId);
static void _VdecFlushDPB(UCHAR ucEsId);
EXTERN UCHAR VDP_FrcGetLockFrameBuffer(UCHAR ucVdpId);
#ifdef CC_MAPLE_CUST_DRV
static void _VDEC_CalEsDataSize(UCHAR ucEsId);
#endif
#ifdef VDEC_TIME_PROFILE
static void _VDEC_CalPicSize(UCHAR ucEsId);

extern INT32 T32_HostExec(INT32 i4TargetFlag, const CHAR *szFormat, ...);

enum
{
    E_T32_NOP,
    E_T32_EXEC,
    E_T32_RESTART,
    E_T32_EXIT,
    E_T32_EXEC_RELOAD_SYM,
    E_T32_DETECT_HOST,
    E_T32_END
};

#endif
UCHAR _VDecReceiveEs(UCHAR ucEsId);
void _VDecReceiveCmd(UCHAR ucEsId, BOOL bIsBlock);
void _VDEC_DeliverEos(UCHAR ucEsId);
UCHAR _VDEC_CheckSkipMode(UCHAR ucEsId);
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
UCHAR _VDEC_Set3DCtrl(UCHAR ucEsId);
#endif
#endif
#endif
#ifdef CC_VDEC_FMT_DETECT
UCHAR _VDEC_DetectVdecFMT(UCHAR ucEsId, UINT32 u4Type);
#endif
#ifdef CC_MAPLE_CUST_DRV
VOID _VDEC_CalBitrate(UCHAR ucEsId);
#endif
UCHAR _VDEC_CheckSkipES(UCHAR ucEsId, UCHAR ucSkipMode, UCHAR ucMpvId);
#ifdef CC_SUPPORT_VDEC_PREPARSE
INT32 VdecHdrPreParse(UCHAR ucMpvId, UCHAR ucEsId);
INT32 VdecCalcFRFromES(UCHAR ucEsId);
UINT32 u4VDecDurationToFrameRate(UINT32 u4Duration);
UINT32 _VDEC_AnalyzeDeltaPts(UCHAR ucEsId);
#endif
#ifdef VDEC_DUMP_VFIFO
EXTERN HANDLE_T _hVFifoDataArrived[VDEC_MAX_ES] ;
EXTERN HANDLE_T _hDataDumpDone[VDEC_MAX_ES];
EXTERN HANDLE_T _hVFifoDataDumpThreadState[VDEC_MAX_ES];
#endif
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef VDEC_TIME_PROFILE
#define INFO_BUF_SIZE       (7*1024*1024)
#define TMP_BUF_SIZE        (256)
#define WORDS_PER_LINE   (32)
#define MAX_LINE               (UINT32)(INFO_BUF_SIZE/WORDS_PER_LINE)
#define NEW_A_LINE      \
    { \
        u4CurTotalLines++;\
        if ( u4CurTotalLines >  MAX_LINE) \
        {\
            _VdecTimeProfileSaveToFile(ucEsId);\
            u4CurSaveFileCnt++;\
            u4CurTotalLines = 0x1;\
            x_memset(acProfieInfo, 0, INFO_BUF_SIZE);\
        }\
        else \
        {\
            strcat(acProfieInfo, "\n"); \
        }\
    }

static HAL_TIME_T _rTimeEsS, _rTimeEsE, _rTimeEsDt, _rTimeFrmS, _rTimeFrmE, _rTimeFrmDt, _rEsAcc;
char acProfieInfo[INFO_BUF_SIZE];
char acProfileTmp[TMP_BUF_SIZE];
char acProfileSaveDir[512];
UINT32 u4CurTotalLines;
UINT32 u4CurSaveFileCnt;


static void _VdecTimeProfileSaveToFile(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    CHAR acData[1024];
    INT32 i = 0, iLen = 0;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    x_memset(&(acData[0]), 0x0, 1024);
    x_memset(prVdecEsInfo->ucAutoPlayCurFileDir, 0x0, sizeof(prVdecEsInfo->ucAutoPlayCurFileDir));
    x_memset(prVdecEsInfo->ucAutoPlayCurFileName, 0x0, sizeof(prVdecEsInfo->ucAutoPlayCurFileName));

    HalFlushInvalidateDCache();
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"d:\\model.name\" 0x%08x /ny", &(acData[0])));
    HalFlushInvalidateDCache();

    iLen = strlen((CHAR *)acData);

    for ( i = iLen; i > 0; i-- )
    {
        if ( acData[i] == '/' )
        {
            break;
        }
    }

    x_strncpy(prVdecEsInfo->ucAutoPlayCurFileDir, acData + 16, i - 16); //Ignore the pre string "mnt/usb/Mass-000/"
    x_strncpy(prVdecEsInfo->ucAutoPlayCurFileName, acData + i + 1, iLen - i - 1);

    for ( i = 0; i < strlen((CHAR *)prVdecEsInfo->ucAutoPlayCurFileDir); i++ )
    {
        if ( prVdecEsInfo->ucAutoPlayCurFileDir[i] == '/' )
        {
            prVdecEsInfo->ucAutoPlayCurFileDir[i] = '\\';
        }
    }

#ifdef VDEC_TIME_PROFILE_SAVE_TO_CURRET
    {
        HalFlushInvalidateDCache();
        if ( prVdecEsInfo->ucAutoPlayCurFileName[0] == 0x0 )
        {
            UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%sTEST_Data_%d_%d.model\" 0x%08x--0x%08x",
                                "D:\\", prVdecEsInfo->u4FrmFldCnt, u4CurSaveFileCnt,
                                (UINT32)&(acProfieInfo[0]), (UINT32)((UINT32)(&(acProfieInfo[0])) + strlen((CHAR *)acProfieInfo) - 1)));
        }
        else
        {
            sprintf(acProfileSaveDir, "%s%s\\", "D:\\Model_Design_Bitstreams\\", (CHAR *)prVdecEsInfo->ucAutoPlayCurFileDir);
            UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s%s_%d_%d.model\" 0x%08x--0x%08x",
                                (CHAR *)acProfileSaveDir, (CHAR *)prVdecEsInfo->ucAutoPlayCurFileName, prVdecEsInfo->u4FrmFldCnt, u4CurSaveFileCnt,
                                (UINT32)&(acProfieInfo[0]), (UINT32)((UINT32)(&(acProfieInfo[0])) + strlen((CHAR *)acProfieInfo) - 1)));
        }
    }
#else
    {
        HalFlushInvalidateDCache();
        if ( prVdecEsInfo->ucAutoPlayCurFileName[0] == 0x0 )
        {
            UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%sTEST_Data_%d_%d.model\" 0x%08x--0x%08x",
                                "D:\\", prVdecEsInfo->u4FrmFldCnt, u4CurSaveFileCnt,
                                (UINT32)&(acProfieInfo[0]), (UINT32)((UINT32)(&(acProfieInfo[0])) + strlen((CHAR *)acProfieInfo) - 1)));
        }
        else
        {
            UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary \"%s%s.model\" 0x%08x--0x%08x",
                                "D:\\Model_Design_Bitstreams\\", (CHAR *)prVdecEsInfo->ucAutoPlayCurFileName,
                                (UINT32)&(acProfieInfo[0]), (UINT32)((UINT32)(&(acProfieInfo[0])) + strlen((CHAR *)acProfieInfo) - 1)));
        }
    }
#endif

    return;
}

/*static void _VdecPrintTime(UINT8 u1Idx)
{
    HAL_TIME_T rTime;

    HAL_GetTime(&rTime);
    LOG(1, "Time%d %u.%06us\n", u1Idx, rTime.u4Seconds, rTime.u4Micros);
}

static void _VdecTimeS(UINT8 u1ProfileIdx)
{
    if((_rVdecStart[u1ProfileIdx].u4Seconds == 0) && (_rVdecStart[u1ProfileIdx].u4Micros == 0))
    {
        HAL_GetTime(&_rVdecStart[u1ProfileIdx]);
    }
}

static void _VdecTimeE(UINT8 u1ProfileIdx, BOOL bShowLog)
{
    HAL_TIME_T rTimeEnd;
    HAL_GetTime(&rTimeEnd);
    HAL_GetDeltaTime(&_rVdecCurDt[u1ProfileIdx], &_rVdecStart[u1ProfileIdx], &rTimeEnd);

    if(_rVdecCurDt[u1ProfileIdx].u4Seconds > _rVdecMaxDt[u1ProfileIdx].u4Seconds)
    {
        _rVdecMaxDt[u1ProfileIdx].u4Seconds = _rVdecCurDt[u1ProfileIdx].u4Seconds;
        _rVdecMaxDt[u1ProfileIdx].u4Micros = _rVdecCurDt[u1ProfileIdx].u4Micros;
    }
    else if ((_rVdecCurDt[u1ProfileIdx].u4Seconds == _rVdecMaxDt[u1ProfileIdx].u4Seconds) &&
        (_rVdecCurDt[u1ProfileIdx].u4Micros > _rVdecMaxDt[u1ProfileIdx].u4Micros))
    {
        _rVdecMaxDt[u1ProfileIdx].u4Micros = _rVdecCurDt[u1ProfileIdx].u4Micros;
    }
    _rVdecStart[u1ProfileIdx].u4Seconds = 0;
    _rVdecStart[u1ProfileIdx].u4Micros = 0;

    if(bShowLog)
    {
        LOG(6, "Cur profileIdx%d %u.%06us\n", u1ProfileIdx,
            _rVdecCurDt[u1ProfileIdx].u4Seconds, _rVdecCurDt[u1ProfileIdx].u4Micros);
        LOG(6, "Max profileIdx%d %u.%06us\n", u1ProfileIdx,
            _rVdecMaxDt[u1ProfileIdx].u4Seconds, _rVdecMaxDt[u1ProfileIdx].u4Micros);
    }
}*/
#endif

void _VdecCheckStatus(UCHAR ucEsId, BOOL fgDecodeOK)
{
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    PARAM_MPV_T* prMpvCounter;
    static HAL_TIME_T _rErrStartTime;
    HAL_TIME_T rErrCurTime;
    HAL_TIME_T rDeltaTime;
    VDEC_DECERR_CB_T rDecErr;
    UINT32 u4FailCntThreshold, u4StartFailCntThreshold = 0;


    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prMpvCounter = prVdecEsInfo->prMpvCounter;
    
#ifdef CC_DUAL_CORE_DEC
    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        //LAE decode thread skip eos handle.
        UINT32 u4Mode, u4Param2 = 0, u4Param3 = 0;
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,&u4Param2,&u4Param3);
        if(u4Mode == LAE_DECODE)
        {
            return;
        }
    }
#endif

    if(fgDecodeOK)
    {
        //notify decoder status
        prVdecEsInfo->u1PVRWaitSps = 0;
        prVdecEsInfo->u4FailCnt = 0; // reset fail count
        prVdecEsInfo->u4FbgFailCnt = 0; // reset fail count
        if(!prVdecEsInfo->fgFmtDectFinish)
        {
            prVdecEsInfo->fgFmtDectFinish = TRUE;
        }

        if((prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_NORMAL) &&
            (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_CODEC_NOT_SUPPORT)
#ifndef DIVX_PLUS_CER
            && (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_BITSTREAM_NOT_SUPPORT)
#endif
            )
        {
            if(VDEC_ChkCodecCap(ucEsId, prVdecEsInfoKeep->eCurFMT))
            {
                LOG(2, "ES(%d) Notify Status change, decode normal\n", ucEsId);
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_NORMAL;
            }
            else
            {
                LOG(2, "ES(%d) Notify Status change, codec not support\n", ucEsId);
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_CODEC_NOT_SUPPORT;
                prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
            }
            //callback, correct mpv data
            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_DECODE_STATUS_CHG, (UINT32)prVdecEsInfo->eNotifyStatus, 0);
            }

        }

        if(prVdecEsInfo->fgSuperFreeze && (!prVdecEsInfo->fgNormalAfterFreeze) &&
                (prVdecEsInfo->eNotifyStatus == VDEC_DEC_DECODE_NORMAL))
        {
            if(VDEC_ChkCodecCap(ucEsId, prVdecEsInfoKeep->eCurFMT))
            {
                //callback, correct mpv data
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                        VDEC_COND_DECODE_STATUS_CHG, (UINT32)prVdecEsInfo->eNotifyStatus, 0);
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                        VDEC_COND_RESOLUTION_CHG, (UINT32)prVdecEsInfo->u2VSize, (UINT32)prVdecEsInfo->u2HSize);

                }
            }
            prVdecEsInfo->fgNormalAfterFreeze = TRUE;
        }
#ifdef ENABLE_MULTIMEDIA 
        if ((10 == prVdecEsInfoKeep->u4TimeoutCnt) && (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_ERROR)
            && (SWDMX_SRC_TYPE_NETWORK_DASH == prVdecEsInfo->eMMSrcType))
        {
            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
               if(prVdecEsInfo->u1PVRWaitSps != 2)
                {
                 LOG(2, "ES(%d) Notify Status change, decode error\n", ucEsId);
                 prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                  VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_ERROR, 0);
                }
            }
            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_ERROR;
            prVdecEsInfoKeep->u4TimeoutCnt = 0;
        }
        if (SWDMX_SRC_TYPE_WIFI_DISPLAY == prVdecEsInfo->eMMSrcType)
        {
            if (((prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt != 0) || 
                (prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt != 0) ||
                (prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt != 0)) && 
                ((prVdecEsInfo->rLastWFDDecodeErr.i4LackSeqHeaderCnt != prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt) ||
                (prVdecEsInfo->rLastWFDDecodeErr.i4OutOfSpecCnt != prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt) ||
                (prVdecEsInfo->rLastWFDDecodeErr.i4TimeoutCnt != prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt)))
            {
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                VDEC_COND_WFD_DECODE_ERR, 0, 0);
                    LOG(2, "Vdec send VDEC_COND_WFD_DECODE_ERR\n");
                }
            }   
            prVdecEsInfo->rLastWFDDecodeErr.i4LackSeqHeaderCnt = prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt;
            prVdecEsInfo->rLastWFDDecodeErr.i4OutOfSpecCnt = prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt;
            prVdecEsInfo->rLastWFDDecodeErr.i4TimeoutCnt = prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt;
        }
#endif
    }
    else
    {
        if(prVdecEsInfo->fgMMPlayback)
        {
            if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
            {
                //some 264 format need more frame to find the sequence header for start decoding.
                u4FailCntThreshold = VDEC_FRM_ERR_THRSD_PLAYBACK*5;
            }
            else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)
            {
                //some mp4 format need more frame to find the sequence header for start decoding.
                u4FailCntThreshold = VDEC_FRM_ERR_THRSD_PLAYBACK*4;
            }
            else
            {
                u4FailCntThreshold = VDEC_FRM_ERR_THRSD_PLAYBACK;
            }
        }
        else
        {
            u4FailCntThreshold = VDEC_FRM_ERR_THRSD_DTV;
        }
        #ifdef ENABLE_MULTIMEDIA
        if(prVdecEsInfo->fgMMPlayback && 
           (prVdecEsInfo->eMMSrcType == (SWDMX_SOURCE_TYPE_T)SWDMX_SRC_TYPE_HIGH_SPEED_STORAGE))
        {
            u4StartFailCntThreshold = VDEC_FRM_ERR_THRSD_STARTPLAY;
        }
        else
        #endif
        {
            //Not notify Start Play fail notify, Only USB case send
            u4StartFailCntThreshold =  u4FailCntThreshold*2;
        }

        prVdecEsInfo->u4FailCnt++;

        if (prVdecInfo->pfnDecErrCb)
        {
            if (prVdecEsInfo->u4FailCnt == 1)
            {
                HAL_GetTime(&_rErrStartTime);
            }
            else
            {
                x_memset(&rErrCurTime, 0, sizeof(HAL_TIME_T));
                x_memset(&rDeltaTime, 0, sizeof(HAL_TIME_T));

                HAL_GetTime(&rErrCurTime);
                HAL_GetDeltaTime(&rDeltaTime, &_rErrStartTime, &rErrCurTime);

                if (((rDeltaTime.u4Seconds * 1000) + (rDeltaTime.u4Micros / 1000)) >= prVdecInfo->u4ErrDuration)
                {
                    rDecErr.u1EsId = ucEsId;
                    rDecErr.u4DecErrNs = prVdecEsInfo->u4FailCnt;
                    prVdecInfo->pfnDecErrCb(&rDecErr);
                    x_memcpy(&_rErrStartTime, &rErrCurTime, sizeof(HAL_TIME_T));
                }
            }
        }

        if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY)
        {
            //notify decoder status
            if((prMpvCounter->u2HSize > 720) &&
                (prMpvCounter->u2VSize > 576) &&
                (prVdecEsInfo->ucFbgId == FBM_FBG_ID_UNKNOWN) &&
                (prVdecEsInfo->u4FbgFailCnt > 0) &&
                (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_HD_NOT_SUPPORT))
            {
                prVdecEsInfo->u4FailCnt = 0; // reset fail count
                prVdecEsInfo->u4FbgFailCnt = 0; // reset fail count

                //callback, non this format data
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                    LOG(2, "ES(%d) Notify Status change, HD size\n", ucEsId);
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                      VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_HD_NOT_SUPPORT, 0);
                }
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_HD_NOT_SUPPORT;
                prVdecEsInfo->fgFmtDectFinish = TRUE;
            }
            else if(prVdecEsInfo->u4FbgFailCnt > 0)
            {
                prVdecEsInfo->u4FailCnt = 0; // reset fail count
                prVdecEsInfo->u4FbgFailCnt = 0; // reset fail count
            }
            else if(prVdecEsInfo->eNotifyStatus == VDEC_DEC_DECODE_CODEC_NOT_SUPPORT)
            {
                prVdecEsInfo->u4FailCnt = 0; // reset fail count
                prVdecEsInfo->u4FbgFailCnt = 0; // reset fail count
                prVdecEsInfo->fgFmtDectFinish = TRUE;
            }
            else if(prVdecEsInfo->fgMMPlayback &&
              (prVdecEsInfo->u4FailCnt >= u4StartFailCntThreshold) &&
              (prVdecEsInfo->ucFbgId == FBM_FBG_ID_UNKNOWN) &&
              (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_HD_NOT_SUPPORT))
            {
                //callback, non mpv data
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                     LOG(2, "ES(%d) Notify HD_NOT_SUPPORT as start play fail\n", ucEsId);
                     prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                        VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_HD_NOT_SUPPORT, 0);
                }
                prVdecEsInfo->u4FailCnt = 0; // reset fail count
                prVdecEsInfo->u4FbgFailCnt = 0; // reset fail count
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_HD_NOT_SUPPORT;
                prVdecEsInfo->fgFmtDectFinish = TRUE;
            }
            else if((prVdecEsInfo->u4FailCnt >= u4FailCntThreshold) &&
              (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_ERROR))
            {
                //callback, non mpv data
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                   if(prVdecEsInfo->u1PVRWaitSps != 2)
                    {
                     LOG(2, "ES(%d) Notify Status change, decode error\n", ucEsId);
                     prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                      VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_ERROR, 0);
                    }
                }
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_ERROR;
                prVdecEsInfo->fgFmtDectFinish = TRUE;
            }
        }   
#ifdef ENABLE_MULTIMEDIA 
        if ((10 == prVdecEsInfoKeep->u4TimeoutCnt) && (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_ERROR)
                && (SWDMX_SRC_TYPE_NETWORK_DASH == prVdecEsInfo->eMMSrcType))
        {
            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
               if(prVdecEsInfo->u1PVRWaitSps != 2)
               {
                   LOG(2, "ES(%d) Notify Status change, decode error\n", ucEsId);
                   prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                   VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_ERROR, 0);
                }
            }
            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_ERROR;
            prVdecEsInfoKeep->u4TimeoutCnt = 0;
        }
        if (SWDMX_SRC_TYPE_WIFI_DISPLAY == prVdecEsInfo->eMMSrcType)
        {
            if (((prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt != 0) || 
            (prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt != 0) ||
            (prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt != 0)) && 
            ((prVdecEsInfo->rLastWFDDecodeErr.i4LackSeqHeaderCnt != prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt) ||
            (prVdecEsInfo->rLastWFDDecodeErr.i4OutOfSpecCnt != prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt) ||
            (prVdecEsInfo->rLastWFDDecodeErr.i4TimeoutCnt != prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt)))
            {
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                VDEC_COND_WFD_DECODE_ERR, 0, 0);
                    LOG(2, "Vdec send VDEC_COND_WFD_DECODE_ERR\n");
                }
            } 
            prVdecEsInfo->rLastWFDDecodeErr.i4LackSeqHeaderCnt = prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt;
            prVdecEsInfo->rLastWFDDecodeErr.i4OutOfSpecCnt = prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt;
            prVdecEsInfo->rLastWFDDecodeErr.i4TimeoutCnt = prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt;
        }
#endif
    }
}

static void _VdecTimeProfile(UCHAR ucEsId)
{
    UINT32 u4HwDecMs = 0;
    #ifndef VDEC_TIME_PROFILE

        #ifdef CC_MT5392B
        static UINT32 _u4MoniterCnt = 0;
        static BOOL _fgMJCOn = TRUE;
        #endif
        VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        #ifdef CC_H264_SUPPORT
        #ifndef CC_VDEC_H264_DISABLE
        VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
        UINT32 u4FbgId = 0, u4WaitFb = 0;
        UNUSED(prVdecInfo);
        #endif
        #endif

        UNUSED(prVdecEsInfoKeep);
        UNUSED(u4HwDecMs);
        #ifdef CC_H264_SUPPORT
        #ifndef CC_VDEC_H264_DISABLE
        if(prVdecEsInfoKeep->pfnVdecGetParam)
        {
            prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, (UINT32)TIME_PROFILE, &u4FbgId, &u4HwDecMs, &u4WaitFb);
        }
        #endif
        #endif
        #ifdef CC_MT5392B
        u4HwDecMs *= 1000;
        //dynamic switch mechanism
        if (u4HwDecMs > prVdecEsInfo->u4BigBLevel1)
        {
            if (u4HwDecMs > prVdecEsInfo->u4BigBLevel3)
            {
                _u4MoniterCnt = (3*prVdecEsInfo->u4BigBBaseCnt);
            }
            else
            if (u4HwDecMs > prVdecEsInfo->u4BigBLevel2)
            {
                _u4MoniterCnt = (2*prVdecEsInfo->u4BigBBaseCnt);
            }
            else
            {
                if (_u4MoniterCnt < prVdecEsInfo->u4BigBBaseCnt)
                {
                    _u4MoniterCnt = prVdecEsInfo->u4BigBBaseCnt;
                }
            }
        }
        else
        {
            if (_u4MoniterCnt != 0)
            {
                _u4MoniterCnt--;
            }
        }

        if((_u4MoniterCnt == 0) && !_fgMJCOn)
        {
            LOG(6, "Enable MJC\n");
            vDrvMJCDynOnOff(TRUE);
            _fgMJCOn = TRUE;
        }
        else if((_u4MoniterCnt > 0) && _fgMJCOn)
        {
            LOG(6, "Disable MJC\n");
            vDrvMJCDynOnOff(FALSE);
            _fgMJCOn = FALSE;
        }
        #else
        UNUSED(prVdecEsInfo);
        #endif

#else //#ifndef VDEC_TIME_PROFILE

    FBM_SEQ_HDR_T *prFbmSeqHdr;
    UINT32 u4FrmRate = 0, u4FbgId = 0, u4WaitFb = 0;
    UINT32 u4RstMs = 0, u4SliceType = 0, u4IsFrmReady = 0;
    UINT32 u4SliceID = 0;
    UINT32 u4SwMicro = 0, u4HwDecCycle = 0, u4DramUseCycle = 0;
    UINT8 i = 0;
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    static DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;

    char *acInfoTitle = "Dec Type,Frm Idx,Pic Type Idx,Pic Type,HW Dec MS,DramCycle,Pic Size,";

    UNUSED(i);

    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, TIME_PROFILE, &u4FbgId, &u4HwDecMs, &u4WaitFb);
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, TIME_PROFILE2, &u4RstMs, &u4SliceType, &u4IsFrmReady);
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, TIME_PROFILE3, &u4SwMicro, &u4HwDecCycle, &u4DramUseCycle);
    }

    if ( prVdecEsInfo->u4FrmFldCnt == 0 )
    {
        x_memset(acProfieInfo, 0, INFO_BUF_SIZE);
        x_memset(acProfileTmp, 0, TMP_BUF_SIZE);
        x_memset(acProfileSaveDir, 0, sizeof(acProfileSaveDir));
        u4CurTotalLines = 0x0;
        u4CurSaveFileCnt = 0x0;

        prVdecEsInfo->u4CurrPicSize = 0x0;
    }

    if((u4FbgId < 0xFF) && (u4IsFrmReady > 0))
    {
        HAL_GetTime(&_rTimeFrmE);
        HAL_GetDeltaTime(&_rTimeFrmDt, &_rTimeFrmS, &_rTimeFrmE);
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(u4FbgId);
        switch (prFbmSeqHdr->ucFrmRatCod)
        {
        case MPEG_FRAME_RATE_24_:
            u4FrmRate = 2397;
            break;
        case MPEG_FRAME_RATE_24:
            u4FrmRate = 2400;
            break;
        case MPEG_FRAME_RATE_25:
            u4FrmRate = 2500;
            break;
        case MPEG_FRAME_RATE_30_:
            u4FrmRate = 2997;
            break;
        case MPEG_FRAME_RATE_30:
            u4FrmRate = 3000;
            break;
        case MPEG_FRAME_RATE_50:
            u4FrmRate = 5000;
            break;
        case MPEG_FRAME_RATE_60_:
            u4FrmRate = 5994;
            break;
        case MPEG_FRAME_RATE_60:
            u4FrmRate = 6000;
            break;
        case MPEG_FRAME_RATE_12:
            u4FrmRate = 1200;
            break;
        case MPEG_FRAME_RATE_15:
            u4FrmRate = 1500;
            break;
        case MPEG_FRAME_RATE_20:
            u4FrmRate = 2000;
            break;
        default:
            u4FrmRate = 3000;
            break;
        }
        //LOG(0, "Slice Type \t = %ld \n", u4SliceType);
        u4SliceID = u4SliceType;
        if(u4SliceType >= 5)
        {
            u4SliceType -= 5;
        }
        ASSERT(u4SliceType < 5);
        //dec time and wait es input time > 9/10 vsync time
        if((u4HwDecMs/* + (_rEsAcc.u4Micros/1000)*/) >= (90000 / u4FrmRate))
        {
            /*LOG(1, "Frm %u.%03us/HwDec 0.%03us/WEs %u.%03us/WFb 0.%03us/Rst 0.%03us/%c\n",
            _rTimeFrmDt.u4Seconds, _rTimeFrmDt.u4Micros/1000,
            u4HwDecMs,
            _rEsAcc.u4Seconds, _rEsAcc.u4Micros/1000,
            u4WaitFb,
            u4RstMs,
            ucSliceType
            );*/
            //if(_rEsAcc.u4Micros < 1000)
            {
                prVdecEsInfo->au4DecTime[u4SliceType][4]++;
            }
        }

        _prDbase = DBS_Lock(&_rCritState);
        ASSERT(_prDbase);
        VERIFY(DBS_Unlock(_prDbase, _rCritState));
        //LOG(1, "PicType %c, HwDec 0.%03us, SwDec 0.%03us, DramCycle %d, F(%d), Q(%d)\n",
        //    ucSliceType, u4HwDecMs, u4SwMicro/1000, u4DramUseCycle, _prDbase->rVdp.arFbCounter[0][0].u4DispFail,
        //    FBM_CheckFrameBufferDispQ(u4FbgId));
        _rEsAcc.u4Seconds = 0;
        _rEsAcc.u4Micros = 0;
        if(u4HwDecMs > prVdecEsInfo->au4DecTime[u4SliceType][0])
        {
            prVdecEsInfo->au4DecTime[u4SliceType][0] = u4HwDecMs;
        }
        if (u4HwDecMs < prVdecEsInfo->au4DecTime[u4SliceType][1])
        {
            prVdecEsInfo->au4DecTime[u4SliceType][1] = u4HwDecMs;
        }

        if(u4SwMicro > prVdecEsInfo->au4DecTime[u4SliceType][5])
        {
            prVdecEsInfo->au4DecTime[u4SliceType][5] = u4SwMicro;
        }
        if(u4HwDecCycle > prVdecEsInfo->au4DecTime[u4SliceType][7])
        {
            prVdecEsInfo->au4DecTime[u4SliceType][7] = u4HwDecCycle;
        }
        if(u4DramUseCycle > prVdecEsInfo->au4DecTime[u4SliceType][8])
        {
            prVdecEsInfo->au4DecTime[u4SliceType][8] = u4DramUseCycle;
        }
        if (  prVdecEsInfo->u4CurrPicSize > prVdecEsInfo->au4DecTimeMaxPicSize[u4SliceType][0] )
        {
            prVdecEsInfo->au4DecTimeMaxPicSize[u4SliceType][0] = prVdecEsInfo->u4CurrPicSize;
        }
        prVdecEsInfo->au4DecTime[u4SliceType][2] += u4HwDecMs;
        prVdecEsInfo->au4DecTime[u4SliceType][6] += (u4HwDecCycle/10000);
        prVdecEsInfo->au4DecTime[u4SliceType][3]++;
        HAL_GetTime(&_rTimeFrmS);

        //if (prVdecEsInfo->u4FrmFldCnt < 31)
    {
            if ( prVdecEsInfo->u4FrmFldCnt == 0 )
            {
                sprintf(acProfieInfo, "%s", acInfoTitle);
                NEW_A_LINE;
            }
            sprintf(acProfileTmp, "%ld,", prVdecEsInfoKeep->eCurFMT);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->u4FrmFldCnt);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", u4SliceType);
            strcat(acProfieInfo, acProfileTmp);
            switch (u4SliceID)
            {
                case 0:
                    sprintf(acProfileTmp, "P,");
                    break;
                case 1:
                    sprintf(acProfileTmp, "B,");
                    break;
                case 2:
                    sprintf(acProfileTmp, "I,");
                    break;
                case 3:
                    sprintf(acProfileTmp, "SP,");
                    break;
                case 4:
                    sprintf(acProfileTmp, "SI,");
                    break;
                case 5:
                    sprintf(acProfileTmp, "P_ALL,");
                    break;
                case 6:
                    sprintf(acProfileTmp, "B_ALL,");
                    break;
                case 7:
                    sprintf(acProfileTmp, "I_ALL,");
                    break;
                case 8:
                    sprintf(acProfileTmp, "SP_ALL,");
                    break;
                case 9:
                    sprintf(acProfileTmp, "SI_ALL,");
                    break;
                default:
                    sprintf(acProfileTmp, "ERROR,"); //Error
                    break;
    }
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", u4HwDecMs);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", u4DramUseCycle);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->u4CurrPicSize);
            strcat(acProfieInfo, acProfileTmp);

            NEW_A_LINE;
        }

        prVdecEsInfo->u4FrmFldCnt++;
    }

    UNUSED(u4HwDecMs);

#endif

}


static void _VdecSetSyncStc(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
    #ifndef CC_VDEC_H264_DISABLE
    //VDEC_INFO_T* prVdecInfo;
    //prVdecInfo = _VDEC_GetInfo();
    #endif
    #endif

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    UNUSED(prVdecEsInfoKeep);

    // in case swdmx control faster than mpv control, prVdecEsInfo might be clear
    prVdecEsInfo->ucSyncMode = prVdecEsInfoKeep->ucSyncMode;
    prVdecEsInfo->ucStcId = prVdecEsInfoKeep->ucStcId;

    if (prVdecEsInfo->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        FBM_SetSyncStc(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
    }

    if (prVdecEsInfo->ucSyncMode== AV_SYNC_MODE_NONE)
    {
          //DTV00213023
          //if (STC_SetSyncTarget(AV_SYNC_TARGET_VDO,  0xFF, ucEsId) != STC_VALID)
          if (STC_SetSyncTarget(AV_SYNC_TARGET_VDO, prVdecEsInfo->ucStcId, ucEsId) != STC_VALID)
          {
              LOG(3, "_VdecSetSyncStc Invalid\n");
          }
          prVdecEsInfo->u2UdfNs = MPV_UDF_PES_NS;
          prVdecEsInfo->u4WaitDispTime = VDEC_WAIT_DISP_TIME;
    }
    else // need to SYNC
    {
        if (STC_SetSyncTarget(AV_SYNC_TARGET_VDO, prVdecEsInfo->ucStcId, ucEsId) != STC_VALID)
        {
            LOG(3, "_VdecSetSyncStc Invalid\n");
        }
        prVdecEsInfo->u2UdfNs = 0;
        if(prVdecEsInfo->fgMMPlayback)
        {
            prVdecEsInfo->u4WaitDispTime = VDEC_AVSYNC_WAIT_DISP_TIME;
        }
        else
        {
            prVdecEsInfo->u4WaitDispTime = VDEC_WAIT_DISP_TIME;
        }
    }
    #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
    #ifndef CC_VDEC_H264_DISABLE
    if(prVdecEsInfoKeep->pfnVdecSetParam)
    {
         prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)WAIT_FB_TIME, prVdecEsInfo->u4WaitDispTime, 0, 0);
         prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)AV_SYNC_INFO, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId, 0);
    }
    #endif
    #endif
    STC_SetVdoDeviceId(prVdecEsInfo->ucStcId, ucEsId);
    //set VFIFO initial delay, will modify PTS value of audio and video by dmx
    if(!prVdecEsInfo->fgMMPlayback && !prVdecEsInfo->fgIPTVPlayback)
    {
        #if (defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)) && !defined(CC_VDEC_H264_DISABLE)
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)    // format will be mpv by default before _VDEC_DecCfg
        {
            VERIFY(STC_SetPtsDrift(prVdecEsInfo->ucStcId, H264_FIFO_CTRL_INIT) == STC_VALID);
        }
        else
        #endif
        {
            VERIFY(STC_SetPtsDrift(prVdecEsInfo->ucStcId, MPV_FIFO_CTRL_INIT) == STC_VALID);
        }
    }
}

VDEC_ASPECT_RATIO_T _VdecDecideAspectRatio(ENUM_VDEC_FMT_T eFmt,UCHAR ucAspectRatio, UINT32 u4Width, UINT32 u4Height, BOOL fgMMPlay)
{
#if 0
    if(eFmt==VDEC_FMT_H264)
    {
        switch (ucAspectRatio)
        {
            case MPEG_ASPECT_RATIO_16_9:
                return VDEC_ASPECT_RATIO_16_9;

            case MPEG_ASPECT_RATIO_221_1:
                return VDEC_ASPECT_RATIO_221_1;

            default:
                break;
        }
         return VDEC_ASPECT_RATIO_4_3;
    }
#endif

    //if (ucAspectRatio != MPEG_ASPECT_RATIO_1_1)
    {
        switch (ucAspectRatio)
        {
            case MPEG_ASPECT_RATIO_1_1:
                return VDEC_ASPECT_RATIO_1_1;

            case MPEG_ASPECT_RATIO_4_3:
                return VDEC_ASPECT_RATIO_4_3;

            case MPEG_ASPECT_RATIO_16_9:
                return VDEC_ASPECT_RATIO_16_9;

            case MPEG_ASPECT_RATIO_221_1:
                return VDEC_ASPECT_RATIO_221_1;

            case MPEG_ASPECT_RATIO_10_11:
                return VDEC_ASPECT_RATIO_10_11;

            case MPEG_ASPECT_RATIO_40_33:
                return VDEC_ASPECT_RATIO_40_33;

            case MPEG_ASPECT_RATIO_16_11:
                return VDEC_ASPECT_RATIO_16_11;

            case MPEG_ASPECT_RATIO_12_11:
                return VDEC_ASPECT_RATIO_12_11;

            case MPEG_ASPECT_RATIO_3_2:
                return VDEC_ASPECT_RATIO_3_2;

            case MPEG_ASPECT_RATIO_24_11:
                return VDEC_ASPECT_RATIO_24_11;

            case MPEG_ASPECT_RATIO_20_11:
                return VDEC_ASPECT_RATIO_20_11;

            case MPEG_ASPECT_RATIO_32_11:
                return VDEC_ASPECT_RATIO_32_11;

            case MPEG_ASPECT_RATIO_80_33:
                return VDEC_ASPECT_RATIO_80_33;

            case MPEG_ASPECT_RATIO_18_11:
                return VDEC_ASPECT_RATIO_18_11;

            case MPEG_ASPECT_RATIO_15_11:
                return VDEC_ASPECT_RATIO_15_11;

            case MPEG_ASPECT_RATIO_64_33:
                return VDEC_ASPECT_RATIO_64_33;

            case MPEG_ASPECT_RATIO_160_99:
                return VDEC_ASPECT_RATIO_160_99;

            case MPEG_ASPECT_RATIO_TRANSMIT:
                return VDEC_ASPECT_RATIO_TRANSMIT;

            default:
                return VDEC_ASPECT_RATIO_1_1;
        }
    }
    /*else
    {
        UINT32 u4Temp;

        // 4:3 = 1.33
        // 16:9 = 1.78
        // 2.21:1 = 2.21

        // if width / height < (1.78 + 1.33) / 2 >> 4:3
        // else if width / height < (2.21 + 1.78) / 2 >> 16:9
        // else 2.21 : 1
        // (1.78 + 1.33) / 2 = 14 / 9
        // (2.21 + 1.78) / 2 ~= 2

        u4Temp = (((u4Width * 9) / 14) / u4Height);

        if (u4Temp < 1)
        {
            return VDEC_ASPECT_RATIO_4_3;
        }
        else
        {
            u4Temp = ((u4Width / 2) / u4Height);

            if (u4Temp < 1)
            {
                return VDEC_ASPECT_RATIO_16_9;
            }
            else
            {
                return VDEC_ASPECT_RATIO_221_1;
            }
        }
    }
    */
}

UINT32 VdecVdpClr2GfxClr(VDP_COLORMODE_T vdp_clr)
{
    GFX_COLOR_MODE_T gfx_clr = CM_Reserved0;
    switch (vdp_clr)
    {
        case VDP_COLORMODE_AYUV_D8888:
            gfx_clr = CM_AYCbCr8888_DIRECT32;
            break;
        case VDP_COLORMODE_RGB_D565:
            gfx_clr = CM_RGB565_DIRECT16;
            break;
        case VDP_COLORMODE_ARGB_D4444:
            gfx_clr = CM_ARGB4444_DIRECT16;
            break;
        case VDP_COLORMODE_ARGB_D8888:
            gfx_clr = CM_ARGB8888_DIRECT32;
            break;
        case VDP_COLORMODE_ARGB_D1555:
            gfx_clr = CM_ARGB1555_DIRECT16;
            break;
        default:
            LOG(0, "VdecVdpClr2GfxClr color mode %d is not supported yet!!\n", vdp_clr);
            ASSERT(0);
            break;
    }
    return gfx_clr;
}

UINT32 _VdecFrameRateMap(BOOL fgGetCode,UINT32 u4Value)
{
    UINT8 uIndex=0,uMapIndex=0;
    UINT32 rMap[][2]=
    {
      {2397,MPEG_FRAME_RATE_24_},
      {2400,MPEG_FRAME_RATE_24},
      {2500,MPEG_FRAME_RATE_25},
      {2997,MPEG_FRAME_RATE_30_},
      {3000,MPEG_FRAME_RATE_30},
      {5000,MPEG_FRAME_RATE_60_},
      {5994,MPEG_FRAME_RATE_30},
      {6000,MPEG_FRAME_RATE_60},
      {1200,MPEG_FRAME_RATE_12},
      {1500,MPEG_FRAME_RATE_15},
      {2000,MPEG_FRAME_RATE_20},
      {12000,MPEG_FRAME_RATE_120},
      {24000,MPEG_FRAME_RATE_240},
      {0,0}
    };

    if(!fgGetCode)
    {
        uIndex=1;
    }

    do
    {
        if(rMap[uMapIndex][uIndex]==u4Value)
        {
           break;
        }
        uMapIndex++;
    }while(rMap[uMapIndex][uIndex]!=0);

    if(rMap[uMapIndex][uIndex]==0)
    {
        LOG(3,"_VdecFrameRateMap(%d,%d) return unknow\n",fgGetCode,u4Value);
        //uMapIndex=4;
    }
    return rMap[uMapIndex][fgGetCode ? 1 : 0];
}

static void _VdecFillThumbnailRegion(VDP_THUMBNAIL_INFO_T *prCapDstInfo)
{
   //UINT32 u4Color=0xff000000;
    UINT32 u4ColorMode= 0xff000000;
    switch(prCapDstInfo->eCanvasColormode)
    {
    case VDP_COLORMODE_ARGB_D8888:
        u4ColorMode=0xff000000;
        break;
    case VDP_COLORMODE_AYUV_D8888:
        u4ColorMode=0xff008080;
        break;
    case VDP_COLORMODE_ARGB_D4444:
        u4ColorMode=0xf000;
        break;
    case VDP_COLORMODE_RGB_D565:
        u4ColorMode=0x0000;
        break;
    case VDP_COLORMODE_ARGB_D1555:
        u4ColorMode=0x7000;
        break;
    // todo :other format.......
    default:
        LOG(3,"unknow color format Colormode(%d).\n", prCapDstInfo->eCanvasColormode);
        break;
    }

   GFX_Lock();
   GFX_MMU_Set_Enable(FALSE);
   GFX_SetDst(prCapDstInfo->pucCanvasBuffer, VdecVdpClr2GfxClr(prCapDstInfo->eCanvasColormode), \
        prCapDstInfo->u4CanvasPitch);
   GFX_SetColor(u4ColorMode);
   GFX_Fill(prCapDstInfo->u4ThumbnailX,prCapDstInfo->u4ThumbnailY, \
        prCapDstInfo->u4ThumbnailWidth,prCapDstInfo->u4ThumbnailHeight);
   GFX_Flush();
   GFX_Task_Wait_Sync();
   GFX_Unlock();
}

VOID _Vdec_IFrameDecodeDone(UCHAR ucEsId, VOID *pPicNfyInfo)
{    
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    FBM_PIC_NTF_INFO_T *prPicNfyInfo;

    prPicNfyInfo = (FBM_PIC_NTF_INFO_T *)pPicNfyInfo;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);    
    ASSERT(prVdecEsInfoKeep != NULL);
    
    if (prVdecEsInfoKeep->pfDecNfyFct)
    {
        prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_FRAME_DONE, 
            prPicNfyInfo->ucFbgId, prPicNfyInfo->ucFbId);
    }
}

VOID _Vdec_FrameDisplayStart(UCHAR ucEsId,UCHAR ucFbgId,UCHAR ucFbId)
{
    
    VDEC_INFO_T* prVdecInfo;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    FBM_PIC_HDR_T *prFbmPicHdr;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_PICINFO_CB_T rPicInfo;
    
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prFbmSeqHdr=FBM_GetFrameBufferSeqHdr(ucFbgId);
    prFbmPicHdr=FBM_GetFrameBufferPicHdr(ucFbgId,ucFbId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(!prVdecEsInfo || !prFbmSeqHdr || !prFbmPicHdr || !prVdecEsInfoKeep)
    {
        LOG(2,"_Vdec_FrameStartDisplay fail \n");
        return;
    }
    
    if(prVdecEsInfo->fgMMPlayback==FALSE && prVdecInfo->pfPicInfo)
    {
        rPicInfo.ch=ucEsId;
        rPicInfo.nFramerate=_VdecFrameRateMap(FALSE,prFbmSeqHdr->ucFrmRatCod)/100;
        rPicInfo.aspect_ratio=prFbmSeqHdr->ucAspRatInf;
        rPicInfo.nHSize=prFbmPicHdr->u4PicWidth;
        rPicInfo.nVSize=prFbmPicHdr->u4PicHeight;
        rPicInfo.nBitrate=prFbmSeqHdr->u4BitRatVal;
        rPicInfo.afd=prFbmSeqHdr->ucActFmt;
        rPicInfo.bProgressiveSeq=prFbmSeqHdr->fgProgressiveSeq; 
        rPicInfo.bProgressiveFrame=prFbmPicHdr->fgProgressiveFrm;
        if(rPicInfo.nVSize==1088 && rPicInfo.nHSize==1920)
        {
            rPicInfo.nVSize=1080;
        }
        
        if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MPV)
        {
            rPicInfo.nActiveX=prFbmPicHdr->au2HOffset[0];
            rPicInfo.nActiveY=prFbmPicHdr->au2HOffset[1];
            rPicInfo.nActiveW=rPicInfo.nHSize-rPicInfo.nActiveX*2;
            rPicInfo.nActiveH=rPicInfo.nVSize-rPicInfo.nActiveY*2;
        }
        else
        {
            if(prFbmPicHdr->fgCropping)
            {
                rPicInfo.nActiveX=prFbmPicHdr->u4CropX;
                rPicInfo.nActiveY=prFbmPicHdr->u4CropY;
                rPicInfo.nActiveW=prFbmPicHdr->u4CropWidth;
                rPicInfo.nActiveH=prFbmPicHdr->u4CropHeight;
            }
            else
            {
                rPicInfo.nActiveX=0;
                rPicInfo.nActiveY=0;
                rPicInfo.nActiveW=rPicInfo.nHSize;
                rPicInfo.nActiveH=rPicInfo.nVSize;
            }
        }
        
        rPicInfo.display_horizontal_size=rPicInfo.nActiveW;
        rPicInfo.display_vertical_size=rPicInfo.nActiveH;
        rPicInfo.info3D=prVdecEsInfo->e3DType;
        rPicInfo.aspect_ratio_idc=prFbmSeqHdr->u4AspectRatioIdc;
        rPicInfo.sar_height=prFbmSeqHdr->u4AspectRatioHeight;
        rPicInfo.sar_width=prFbmSeqHdr->u4AspectRatioWidth;
        LOG(8,"_Vdec_FrameDisplayStart(%d,%d)\n",ucFbgId,ucFbId);
        prVdecInfo->pfPicInfo(&rPicInfo);
    }
    
    return;
}

static void _VdecGetThumbnail(UCHAR ucEsId)
{
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    UINT32 u4WorkBuf = 0, u4AlignSz = 0,u4FbNum = 0, u4Idx, u4TmpBuf;
    UINT32 u4AdjThumbWidth = 0, u4AdjThumbHeight = 0;
    UINT32 u4ThumbRatio = 0, u4VideoRatio = 0;
#if defined (CC_MT5890) || defined(CC_MT5861)
#if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
    UINT32 u4DynMapAddr = 0;
#endif
    UINT32 u4Pitch = 0;
    UINT32 u4Temp = 0;
    UCHAR ucFbId = 0xFF;
#endif

    BOOL fgFlush = FALSE;
#if 0
    UINT32 u4Line=0,u4Hpos=0,u4Offset=0;
    UCHAR *pDstBuffer;
#endif

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    ASSERT(prVdecInfo != NULL);
    ASSERT(prVdecEsInfo != NULL);
    ASSERT(prVdecEsInfoKeep != NULL);

    if((prVdecEsInfo->rCapSrcInfo.u4VSize == 0) || (prVdecEsInfo->rCapSrcInfo.u4HSize == 0))
    {
        return;
    }

#ifdef VDEC_THUMBNAIL_ADDR_CHECK
    {
       UINT32 u4OsdFbmAddr=0,u4OsdPoolIndex=0;
       UCHAR *pDstBuffer;
       BOOL fgFindOSD=FALSE;
       FBM_POOL_T *prFbmPool1;
       pDstBuffer=(UCHAR*)VIRTUAL(prVdecEsInfo->rCapDstInfo.pucCanvasBuffer);
       for(u4OsdPoolIndex=FBM_POOL_TYPE_JPEG_OSD1;u4OsdPoolIndex<=FBM_POOL_TYPE_JPEG_OSD4;u4OsdPoolIndex++)
       {
          prFbmPool1 = FBM_GetPoolInfo((UINT8)u4OsdPoolIndex);
          u4OsdFbmAddr=VIRTUAL(prFbmPool1->u4Addr);
          if(((UINT32)pDstBuffer)>=u4OsdFbmAddr&& ((UINT32)pDstBuffer)<(u4OsdFbmAddr+prFbmPool1->u4Size))
          {
             fgFindOSD=TRUE;
             break;
          }
       }

       if(!fgFindOSD)
       {
           if (prVdecInfo->pfErrNotify)
           {
              LOG(0, "Notify get thumbnail fail, address error\n");
              prVdecInfo->pfErrNotify((UINT32)prVdecEsInfo->u2CompId,
               (UINT32)VDEC_COND_ERROR, 0, 0);
           }
           return;
       }
    }
#endif

#if 0                //remove it for kernnel crash
    u4Offset=prVdecEsInfo->rCapDstInfo.u4ThumbnailX*4;
    pDstBuffer=VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer+(prVdecEsInfo->rCapDstInfo.u4ThumbnailY*prVdecEsInfo->rCapDstInfo.u4CanvasPitch));
    for(u4Line=0;u4Line<prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight;u4Line++)
    {
       for(u4Hpos=0;u4Hpos<prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth;u4Hpos++)
       {
         if(prVdecEsInfo->bIsOnePass)
         {
             *(((UINT32 *)(VIRTUAL((UINT32)pDstBuffer)+u4Offset))+u4Hpos)=0xff008080;
         }
         else
         {
             *(((UINT32 *)(VIRTUAL((UINT32)pDstBuffer)+u4Offset))+u4Hpos)=0xff000000;
         }
       }
       pDstBuffer+=prVdecEsInfo->rCapDstInfo.u4CanvasPitch;
    }
#endif
    _VdecFillThumbnailRegion(&prVdecEsInfo->rCapDstInfo);

    // keep ratio, calculate image area, if request to ARGB --> 2pass needed
 if(prVdecEsInfo->fgKeepRation)  //DTV000311949,
 {
    u4ThumbRatio = ((prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth * 1000) / prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight);
    u4VideoRatio = ((prVdecEsInfo->rCapSrcInfo.u4HSize * 1000)/ prVdecEsInfo->rCapSrcInfo.u4VSize);
    if(u4ThumbRatio < u4VideoRatio)
    {
        u4AdjThumbHeight = prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth *
                            prVdecEsInfo->rCapSrcInfo.u4VSize /
                            prVdecEsInfo->rCapSrcInfo.u4HSize ;
        prVdecEsInfo->rCapDstInfo.u4ThumbnailY += (prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight - u4AdjThumbHeight) / 2;
        prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight = u4AdjThumbHeight;
        if(0 == prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight)
        {
            prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight = 2;
        }
    }
    else
    {
        u4AdjThumbWidth  = prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight *
                            prVdecEsInfo->rCapSrcInfo.u4HSize /
                            prVdecEsInfo->rCapSrcInfo.u4VSize ;
        prVdecEsInfo->rCapDstInfo.u4ThumbnailX += (prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth - u4AdjThumbWidth) / 2;
        prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth = u4AdjThumbWidth;
        if(0 == prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth)
        {
            prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth = 2;
        }
    }

    if(!prVdecEsInfo->bIsOnePass)       //allocate working buffer for 2 pass
    {
        u4FbNum = (UINT32)FBM_GetFrameBufferNs(prVdecEsInfo->ucFbgId);
        for(u4Idx = 0; u4Idx < u4FbNum; u4Idx++)
        {
            if(FBM_GetFrameBufferStatus(prVdecEsInfo->ucFbgId, u4Idx)
                != FBM_FB_STATUS_LOCK)  //already freeze at mpv_mw.c
            {
                if((prVdecEsInfo->fgPlaybackNotDisplay) || // MHP I frame case
                   (!prVdecEsInfo->fgKeepRation))          // show logo case
                {
                    if(FBM_GetFrameBufferStatus(prVdecEsInfo->ucFbgId, u4Idx)
                       == FBM_FB_STATUS_DISPLAYQ)
                    {
                        // displaying, get other frame
                        continue;
                    }
                }
                FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, u4Idx, &u4WorkBuf, &u4TmpBuf);
                UNUSED(u4TmpBuf);
                break;
            }
        }
        if((u4WorkBuf == 0) || (u4WorkBuf == prVdecEsInfo->rCapSrcInfo.u4AddrY))
        {
            LOG(3, "WorkingBuf err in GetThumbnail\n");
            return;
        }

        //check alignment, gfx convert can't set src pitch, should be equal to width
        //set active region in the middle of Thumbnail rectangle
        u4AlignSz = prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth & 0xF;
        /*u4ThumbnailWidth < 16, need to enlarge u4ThumbnailWidth to 16*/
        if(u4AlignSz == prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth)
        {
            prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth = 16;
            if(prVdecEsInfo->rCapDstInfo.u4ThumbnailX >= ((16-prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth)>>1))
            {
                prVdecEsInfo->rCapDstInfo.u4ThumbnailX -= ((16-prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth)>>1);
            }
            else
            {
                prVdecEsInfo->rCapDstInfo.u4ThumbnailX = 0;
            }

        }
        else/*u4ThumbnailWidth > 16, align u4ThumbnailWidth*/
        {
            if(u4AlignSz >0)
            {
                prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth -= u4AlignSz;
                prVdecEsInfo->rCapDstInfo.u4ThumbnailX+=(u4AlignSz>>1);
            }
        }

        if(prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight&0x1)
        {
            prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight--;
            if(0 == prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight)
            {
                prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight = 2;
            }
        }
    }
    }

    if(prVdecEsInfo->rCapSrcInfo.u4VSize&0x1)
    {
        prVdecEsInfo->rCapSrcInfo.u4VSize--;
    }
#if defined (CC_MT5890) || defined(CC_MT5861)
    if(prVdecEsInfo->rCapSrcInfo.fgUFOOn)
    {
        ucFbId = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, prVdecEsInfo->u4WaitDispTime);        
        FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, ucFbId, &(prVdecEsInfo->rCapSrcInfo.u4DstY), &(prVdecEsInfo->rCapSrcInfo.u4DstC));   
        u4Temp = prVdecEsInfo->rCapSrcInfo.u4HSize;
        //prVdecEsInfo->rCapSrcInfo.u4HSize = (prVdecEsInfo->u4BitDepth == 10) ? prVdecEsInfo->rCapSrcInfo.u4MemHSize : u4Temp;
        prVdecEsInfo->rCapSrcInfo.u4HSize = prVdecEsInfo->rCapSrcInfo.u4MemHSize;
        LOG(0, "ufo thumbnail to Y:0x%x  C:0x%x\n", prVdecEsInfo->rCapSrcInfo.u4DstY, prVdecEsInfo->rCapSrcInfo.u4DstC);
        IMGRZ_DecompressUFO((void*)&prVdecEsInfo->rCapSrcInfo);
        prVdecEsInfo->rCapSrcInfo.u4HSize = u4Temp;
        prVdecEsInfo->rCapSrcInfo.u4AddrY = prVdecEsInfo->rCapSrcInfo.u4DstY;
        prVdecEsInfo->rCapSrcInfo.u4AddrC = prVdecEsInfo->rCapSrcInfo.u4DstC;
    }
#endif

    if(prVdecEsInfo->bIsOnePass)
    {
        LOG(1,"Thumbnnail with IMGRZ_SetDigest\n");       
#if defined(CC_MT5890)//for oryx, remove flush all cache in imgrz, and module flush by itself
        #if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
        u4DynMapAddr = (UINT32)BSP_MapReservedMem((void *)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer, 
                                    prVdecEsInfo->rCapDstInfo.u4CanvasPitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight);
        if(!u4DynMapAddr)
        {   
              LOG(0, "%s %d thumbnail map addr fail\n", __FUNCTION__,__LINE__);
              ASSERT(0);
         }
        #endif    

        HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer), 
                                    prVdecEsInfo->rCapDstInfo.u4CanvasPitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight);
            
        #if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
        if(u4DynMapAddr)
        {        
            BSP_UnMapReservedMem((void *)u4DynMapAddr);    
        }
        #endif
#endif

    IMGRZ_SetDigest((void*)&prVdecEsInfo->rCapSrcInfo, (void*)&prVdecEsInfo->rCapDstInfo,
                    prVdecEsInfo->bIsOnePass, u4WorkBuf);
    }
    else if(prVdecEsInfo->fgKeepRation) // YCbCr->YUV->ARGB8888
    {

#if defined (CC_MT5890) || defined(CC_MT5861)
        u4Pitch = ((prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth + 15)&(~15));
        #if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
        u4DynMapAddr = (UINT32)BSP_MapReservedMem((void *)u4WorkBuf, u4Pitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 2);
        if(!u4DynMapAddr)
        {   
            LOG(0, "%s %d thumbnail map addr fail\n", __FUNCTION__,__LINE__);
            ASSERT(0);
        }
        #endif

        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4WorkBuf), u4Pitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 2);
#endif

        IMGRZ_SetDigest((void*)&prVdecEsInfo->rCapSrcInfo, (void*)&prVdecEsInfo->rCapDstInfo,
                       prVdecEsInfo->bIsOnePass, u4WorkBuf);

#if defined (CC_MT5890) || defined(CC_MT5861)
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4WorkBuf), u4Pitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 2);
        #if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
        if(u4DynMapAddr)
        {        
            BSP_UnMapReservedMem((void *)u4DynMapAddr);    
        }
        #endif
#endif

        LOG(1,"Thumbnnail with GFX_ColorConvert\n");
#if 0//defined(CC_MT5890)//for oryx, remove flush all cache in imgrz, and module flush by itself                 
        x_memset((void *)(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer)), 0, 
            prVdecEsInfo->rCapDstInfo.u4CanvasPitch * prVdecEsInfo->rCapDstInfo.u4CanvasHeight);

        HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer), 
                    prVdecEsInfo->rCapDstInfo.u4CanvasPitch * prVdecEsInfo->rCapDstInfo.u4CanvasHeight);
#endif

        UNUSED(GFX_ColorConvert((void*)&prVdecEsInfo->rCapDstInfo, u4WorkBuf, VdecVdpClr2GfxClr(prVdecEsInfo->rCapDstInfo.eCanvasColormode), 0xFF));

#if 0//defined(CC_MT5890)
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer), 
                        prVdecEsInfo->rCapDstInfo.u4CanvasPitch *  prVdecEsInfo->rCapDstInfo.u4CanvasHeight);
#endif

    }
    else // YCbCr(64*32) -> ARGB8888
    {
        LOG(1,"Thumbnnail with GFX_ColorConvert_YC\n");        
         // we clean dst buffer by cpu, flush cache aviod the memory being changed by cpu automatically flush later
         // be care of virtual address translation
         #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
         if(GFX_Check_DirectFBMem((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer) == FALSE)
         {
            HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer),
                (prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight*prVdecEsInfo->rCapDstInfo.u4CanvasPitch));
         }
        #else
            HalFlushInvalidateDCache();
        #endif
        UNUSED(GFX_ColorConvert_YC((void*)&prVdecEsInfo->rCapDstInfo, prVdecEsInfo->rCapSrcInfo.u4AddrY,
                    prVdecEsInfo->rCapSrcInfo.u4AddrC, VdecVdpClr2GfxClr(prVdecEsInfo->rCapDstInfo.eCanvasColormode), 0xFF ,
                    prVdecEsInfo->rCapSrcInfo.u4MemHSize));         
        // we clean dst buffer by cpu, flush cache aviod the memory being changed by cpu automatically flush later
        #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
         if(GFX_Check_DirectFBMem((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer) == FALSE)
         {
             HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer),
                (prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight*prVdecEsInfo->rCapDstInfo.u4CanvasPitch));
         }
         else
         {
             fgFlush = TRUE;
         }
        #else
            HalFlushInvalidateDCache();
        #endif
    }


    if (prVdecInfo->pfErrNotify)
    {
        LOG(3, "Notify get thumbnail done, no data\n");
        prVdecInfo->pfErrNotify((UINT32)prVdecEsInfo->u2CompId,
            (UINT32)VDEC_COND_THUMBNAIL_DONE, 0, 0);
    }
    // TODO, modify mpv_mw.c to use this callback function.
    else if (prVdecEsInfoKeep->pfDecNfyFct) // notify io_mtvdec.c case
    {
        LOG(3, "Notify get thumbnail done\n");
        prVdecEsInfoKeep->pfDecNfyFct(
            prVdecEsInfoKeep->pvDecNfyTag,
            VDEC_COND_THUMBNAIL_DONE, fgFlush, 0);
    }

}

void _VdecGetThumbnail2(UCHAR ucEsId, UCHAR ucFbId)
{
    //VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;
    UINT32 u4WorkBuf = 0, u4AlignSz = 0;
    UINT32 u4AdjThumbWidth = 0, u4AdjThumbHeight = 0;
    UINT32 u4ThumbRatio = 0, u4VideoRatio = 0,u4OriginalBottom=0;
#if defined (CC_MT5890)
#if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
    UINT32 u4DynMapAddr = 0;
#endif
    UINT32 u4Pitch = 0;
#endif

    //prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    //ASSERT(prVdecInfo != NULL);
    ASSERT(prVdecEsInfo != NULL);

    FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, ucFbId,
        &(prVdecEsInfo->rCapSrcInfo.u4AddrY),
        &(prVdecEsInfo->rCapSrcInfo.u4AddrC));
    FBM_GetFrameBufferSize(prVdecEsInfo->ucFbgId,
        &(prVdecEsInfo->rCapSrcInfo.u4MemHSize),
        &(prVdecEsInfo->rCapSrcInfo.u4MemVSize));
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbId);

    if (prFbmSeqHdr == NULL)
    {
        LOG(3, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);

        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            prVdecEsInfoKeep->pfDecNfyFct(
                prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_THUMBNAIL_FAIL, 0, 0);
        }
        return;
    }
    else
    {
        /* Thumbnail usage: Image resizer will use this as pitch. */
        if (prFbmPicHdr && prFbmSeqHdr->fgResizeSmallPic)
        {
            prVdecEsInfo->rCapSrcInfo.u4MemHSize =
                (UINT32)prFbmPicHdr->u4PicWidthPitch;

            prVdecEsInfo->rCapSrcInfo.u4HSize = (UINT32)prFbmPicHdr->u4PicWidth;
            prVdecEsInfo->rCapSrcInfo.u4VSize = (UINT32)prFbmPicHdr->u4PicHeight;
        }
        else
        {
            prVdecEsInfo->rCapSrcInfo.u4MemHSize = (UINT32)prFbmSeqHdr->u2LineSize;

            prVdecEsInfo->rCapSrcInfo.u4HSize = (UINT32)prFbmSeqHdr->u2OrgHSize;
            prVdecEsInfo->rCapSrcInfo.u4VSize = (UINT32)prFbmSeqHdr->u2OrgVSize;
        }

        prVdecEsInfo->rCapSrcInfo.u4RRMode = (UINT32)prFbmSeqHdr->fgRRYEn;
    }

    if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MJPEG)
    {
        #if  defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)// Force raster out for VENC
        prVdecEsInfo->rCapSrcInfo.u4IsRsIn = 1; //MJPEG for 5395 encoder input
        prVdecEsInfo->rCapSrcInfo.u4SrcSwap = 0;
        #else
        prVdecEsInfo->rCapSrcInfo.u4IsRsIn = 0;
        prVdecEsInfo->rCapSrcInfo.u4SrcSwap = 6;//block swap mode;
        #endif
    }
    else
    {
        prVdecEsInfo->rCapSrcInfo.u4IsRsIn = 0;
        prVdecEsInfo->rCapSrcInfo.u4SrcSwap = 6;//block swap mode;
    }

    if((prVdecEsInfo->rCapSrcInfo.u4VSize == 0) ||
       (prVdecEsInfo->rCapSrcInfo.u4HSize == 0))
    {
        LOG(3, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);

        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            prVdecEsInfoKeep->pfDecNfyFct(
                prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_THUMBNAIL_FAIL, 0, 0);
        }
        return;
    }
    _VdecFillThumbnailRegion(&prVdecEsInfo->rCapDstInfo);

    if(prVdecEsInfo->fgKeepRation)
    {
        u4OriginalBottom = prVdecEsInfo->rCapDstInfo.u4ThumbnailY
            + prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight;
        u4ThumbRatio = ((prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth * 1000) /
            prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight);
        u4VideoRatio = ((prVdecEsInfo->rCapSrcInfo.u4HSize * 1000) /
            prVdecEsInfo->rCapSrcInfo.u4VSize);
        if(u4ThumbRatio < u4VideoRatio)
        {
            u4AdjThumbHeight = prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth *
                prVdecEsInfo->rCapSrcInfo.u4VSize /
                prVdecEsInfo->rCapSrcInfo.u4HSize ;
            prVdecEsInfo->rCapDstInfo.u4ThumbnailY +=
                (prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight - u4AdjThumbHeight) / 2;
            prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight = u4AdjThumbHeight;
        }
        else
        {
            u4AdjThumbWidth  = prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight *
                prVdecEsInfo->rCapSrcInfo.u4HSize /
                prVdecEsInfo->rCapSrcInfo.u4VSize ;
            prVdecEsInfo->rCapDstInfo.u4ThumbnailX +=
                (prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth - u4AdjThumbWidth) / 2;
            prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth = u4AdjThumbWidth;
        }
    }

    //allocate working buffer for 2 pass
    {
        //FBM_POOL_T *prFbmPool;
        //prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_FEEDER);
        u4WorkBuf = prVdecEsInfo->u4WorkingBufAddr;
        if((u4WorkBuf == 0) ||
           (prVdecEsInfo->u4WorkingBufSize <
           (prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth *
            prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 4)))
        {
            LOG(3, "ES(%d)(L:%d) %s fail\n", ucEsId, __LINE__, __FUNCTION__);

            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
                prVdecEsInfoKeep->pfDecNfyFct(
                    prVdecEsInfoKeep->pvDecNfyTag,
                    VDEC_COND_THUMBNAIL_FAIL, 0, 0);
            }
            return;
        }

        //check alignment, gfx convert can't set src pitch, should be equal to width
        //set active region in the middle of Thumbnail rectangle
        u4AlignSz = prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth & 0xF;

        if(u4AlignSz > 0)
        {
            prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth -= u4AlignSz;
            prVdecEsInfo->rCapDstInfo.u4ThumbnailX += (u4AlignSz>>1);
        }

        if(prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight&0x1)
        {
            if((prVdecEsInfo->rCapDstInfo.u4ThumbnailY +
                prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight + 1) >
                u4OriginalBottom)
            {
                prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight--;
            }
            else
            {
                prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight++;
            }
        }
    }

#if defined(CC_MT5890)
    u4Pitch = ((prVdecEsInfo->rCapDstInfo.u4ThumbnailWidth + 15)&(~15));    
    #if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
    u4DynMapAddr = (UINT32)BSP_MapReservedMem((void *)u4WorkBuf, u4Pitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 2);
    if(!u4DynMapAddr)
    {   
        LOG(0, "%s %d thumbnail map addr fail\n", __FUNCTION__,__LINE__);
        ASSERT(0);
    }
    #endif

    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4WorkBuf), u4Pitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 2);
#endif

    IMGRZ_SetDigest((void*)&prVdecEsInfo->rCapSrcInfo,
        (void*)&prVdecEsInfo->rCapDstInfo,
                    FALSE, u4WorkBuf);

#if defined(CC_MT5890)
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4WorkBuf), u4Pitch * prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight * 2);
                        
    #if defined(CC_VDEC_FBM_DYNAMIC_MAPPING)
        if(u4DynMapAddr)
        {        
            BSP_UnMapReservedMem((void *)u4DynMapAddr);    
        }
    #endif
#endif

    // second pass
    {
            
#if defined(CC_MT5890)//for oryx, remove flush all cache in imgrz, and module flush by itself                       
    x_memset((void *)(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer)), 0, 
        prVdecEsInfo->rCapDstInfo.u4CanvasPitch * prVdecEsInfo->rCapDstInfo.u4CanvasHeight);

    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer), 
        prVdecEsInfo->rCapDstInfo.u4CanvasPitch *  prVdecEsInfo->rCapDstInfo.u4CanvasHeight);
#endif

    UNUSED(GFX_ColorConvert((void*)&prVdecEsInfo->rCapDstInfo,
        u4WorkBuf, VdecVdpClr2GfxClr(prVdecEsInfo->rCapDstInfo.eCanvasColormode), 0xFF));

#if defined(CC_MT5890)//for oryx, remove flush all cache in imgrz, and module flush by itself                       
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer), 
        prVdecEsInfo->rCapDstInfo.u4CanvasPitch *  prVdecEsInfo->rCapDstInfo.u4CanvasHeight);
#endif
    }

    // notify finish
    if (prVdecEsInfoKeep->pfDecNfyFct)
    {
        UINT32 u4DeltaMs = 0;
        HAL_TIME_T rTime;
        HAL_TIME_T rDeltaTime;
        HAL_GetTime(&rTime);

        HAL_GetDeltaTime(&rDeltaTime, &prVdecEsInfo->rThumbStartTime, &rTime);
        u4DeltaMs = (rDeltaTime.u4Seconds * 1000) +
                    (rDeltaTime.u4Micros / 1000);
        LOG(6, "Es(%d) Thumbnail Delat Time(%d.%06d)\n", ucEsId,
            rDeltaTime.u4Seconds, rDeltaTime.u4Micros);

        LOG(6, "ES(%d)(L:%d) %s done\n", ucEsId, __LINE__, __FUNCTION__);
        prVdecEsInfoKeep->pfDecNfyFct(
            prVdecEsInfoKeep->pvDecNfyTag,
            VDEC_COND_THUMBNAIL_DONE, u4DeltaMs, 0);
    }
}

VOID _VDEC_JobBeforePutDispQ(UCHAR ucEsId, UCHAR ucFbId)
{
    UCHAR ucDispFbId;
    VDEC_ES_INFO_T *prVdecEsInfo;
    FBM_PIC_HDR_T * prFbmPicHdr;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);
    if(!prVdecEsInfo)
    {
        LOG(3, "_VDEC_JobBeforePutDispQ, prVdecEsInfo null\n");
        return;
    }

    if(prVdecEsInfo->fgThumbMethod2)
    {
        // remove previous data in displayQ
        if(ucEsId == ES1)
        {
            while(FBM_CheckFrameBufferDispQ(prVdecEsInfo->ucFbgId) > 0)
            {
                ucDispFbId = FBM_GetFrameBufferFromDispQ(prVdecEsInfo->ucFbgId);

                if(ucDispFbId != FBM_FB_ID_UNKNOWN)
                {
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucDispFbId, FBM_FB_STATUS_LOCK);
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucDispFbId, FBM_FB_STATUS_EMPTY);
                }
            }
        }
        if(prVdecEsInfo->fgThumbStartTime)
        {
            _VdecGetThumbnail2(ucEsId, ucFbId);
            prVdecEsInfo->fgThumbMethod2 = FALSE;
        }
    }

    //resize display frm for encoder flow
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbId);
    if(prFbmPicHdr)
    {
        prFbmPicHdr->fgResizeDispFrm = prVdecEsInfo->fgResizeDispFrm;
        prFbmPicHdr->ucRzId = prVdecEsInfo->ucRzId;
    }

}

static void _VdecFlushDPB(UCHAR ucEsId)
{
    UCHAR ucFRefFbId, ucBRefFbId;
    STC_CLOCK rStcClk;
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    FBM_PIC_HDR_T *prFbmPicHdr;

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(!prVdecInfo)
    {
        LOG(3, "Flush buffer handle null!\n");
        return;
    }

    if (((prVdecEsInfo->ucFbgId != FBM_FBG_TYPE_UNKNOWN) &&
        (prVdecEsInfoKeep->eCurState == VDEC_ST_IDLE)) ||
        (prVdecEsInfo->fgForceFlush == TRUE))
    {        
        LOG(3,"Flush DPB now ......\n");
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            //flush dpb
            #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
            #ifndef CC_VDEC_H264_DISABLE
            _VDEC_H264Flush(ucEsId, TRUE);
            #endif
            #endif
        }
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
        {
            #if defined(ENABLE_MULTIMEDIA) || defined(CC_H265_SUPPORT)
            #ifndef CC_VDEC_H265_DISABLE
            vVDecH265Flush(ucEsId, TRUE);
            #endif
            #endif
        }
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP9)
        {
            #if defined(ENABLE_MULTIMEDIA) || defined(CC_VP9_SUPPORT)
            #ifndef CC_VDEC_VP9_DISABLE
            //_VDEC_Vp9Flush(ucEsId, TRUE);
            #endif
            #endif
        }
        else
        {
            //Put Backward Reference pic to DISPQ
            FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
            prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
            if(prFbmPicHdr)
            {
                if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_READY))
                {
                    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                    {
                        UNUSED(_MPV_CalcPts(ucEsId, ucBRefFbId));
                    }
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_MPEG4_DISABLE
                    else if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)
                    {
                        UNUSED(_VDEC_MPEG4CalPts(ucEsId, ucBRefFbId));
                    }
#endif
#endif
                    if(STC_GetSrc(0, &rStcClk) == STC_VALID)
                    {
                        #if BDP_LOG_STYLE
                        VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, STC = %d, FbgID = %d, FbId = %d\n",ucEsId,
                                   prFbmPicHdr->ucPicCdTp,prFbmPicHdr->u4PTS, rStcClk.u4Base, prVdecEsInfo->ucFbgId, ucBRefFbId);
                        #endif
                    }

                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_DISPLAYQ);
                    if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
                    {
                        LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
                           prFbmPicHdr?prFbmPicHdr->ucPicCdTp:0,
                           prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
                           prVdecEsInfo->ucFbgId, ucBRefFbId,
                           prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
                           prFbmPicHdr?prFbmPicHdr->u8PTS:0x0);
                    }

                    prVdecEsInfo->u4DisplayQPicCnt++;
                }
                if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_READY))
                {
                    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                    {
                        UNUSED(_MPV_CalcPts(ucEsId, ucFRefFbId));
                    }
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_MPEG4_DISABLE
                    else if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)
                    {
                        UNUSED(_VDEC_MPEG4CalPts(ucEsId, ucBRefFbId));
                    }
#endif
#endif
                    if(STC_GetSrc(0, &rStcClk) == STC_VALID)
                    {
                        #if BDP_LOG_STYLE
                        VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, STC = %d, FbgID = %d, FbId = %d\n",ucEsId,
                                   prFbmPicHdr->ucPicCdTp,prFbmPicHdr->u4PTS, rStcClk.u4Base, prVdecEsInfo->ucFbgId, ucFRefFbId);
                        #endif
                    }

                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_DISPLAYQ);
                    if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
                    {
                        LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
                           prFbmPicHdr?prFbmPicHdr->ucPicCdTp:0,
                           prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
                           prVdecEsInfo->ucFbgId, ucFRefFbId,
                           prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
                           prFbmPicHdr?prFbmPicHdr->u8PTS:0x0);
                    }
                    prVdecEsInfo->u4DisplayQPicCnt++;
                }
            }
        }
    }
    else
    {
        LOG(3, "Flush buffer state err!\n");
    }
}


void _VDEC_DeliverEos(UCHAR ucEsId)
{
    //UCHAR ucFbgId;
    UCHAR ucFbId;
    UCHAR ucFbgType;
    UCHAR ucFbStatus;
    UCHAR ucFbIdSub = 0xFF;
    FBM_PIC_HDR_T *prFbmPicHdrSub = NULL;
    STC_CLOCK rStcClk;
    //UCHAR ucDecFbId;
    UCHAR ucFRefFbId, ucBRefFbId;
    FBM_PIC_HDR_T *prFbmPicHdr;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
    VDEC_TO_TZ_FB_OPERATION_T prVdecFbOp;
#endif

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((!prVdecInfo) || (!prVdecEsInfo))
    {
        LOG(3, "DeEos handle null\n");
        return;
    }
#ifdef CC_DUAL_CORE_DEC
    if(prVdecEsInfoKeep->pfnVdecGetParam)
    {
        //LAE decode thread skip eos handle.
        UINT32 u4Mode, u4Param2 = 0, u4Param3 = 0;
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,&u4Param2,&u4Param3);
        if(u4Mode == LAE_DECODE)
        {
            return;
        }
    }
#endif
    if((prVdecEsInfoKeep->rPesInfo.fgEos) &&
        (!prVdecEsInfo->fgSendedEos))
    {
        prVdecEsInfo->fgSendedEos = TRUE;
        LOG(3, "_VDEC_DeliverEos(%d)\n", ucEsId);
        
        // For H264, We may not found IDR frame (which will fgThumbStartTime=TRUE). 
        // But VDEC can decode I frame, in this case, we should also Ntf Thumbnali
        if (prVdecEsInfo->fgThumbMethod2 && (!prVdecEsInfo->fgThumbStartTime))
        {   
            LOG(2, "ES%d fgThumbStartTime=0 but decode finished\n", ucEsId);
            prVdecEsInfo->fgThumbStartTime = TRUE;
        }

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
        if (prVdecEsInfoKeep->pfDecAutoTestNfyFct)
        {
            LOG(0, "VDEC EOS, notify Auto test Crc result\n");
            prVdecEsInfoKeep->pfDecAutoTestNfyFct(prVdecEsInfoKeep->pvDecAutoTestNfyTag,
            VDEC_COND_CRC_CHECK_DONE, 2, prVdecEsInfo->u4CrcResult);
        }

        if(prVdecEsInfo->fgAutoCrcCk)
        {
            prVdecEsInfo->fgAutoCrcCk = FALSE;
            if(prVdecEsInfo->u4CrcGolden != prVdecEsInfo->u4CrcResult)
            {
                LOG(0," !!!crc mismatch, u4CrcResult =0x%x, u4CrcGolden =0x%x\n",
                prVdecEsInfo->u4CrcResult, prVdecEsInfo->u4CrcGolden);
            }
        }

#endif

#if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    if(VDEC_IsTestCRC(ucEsId))
    {
        LOG(0, "The CRC value of the file is %u\n", prVdecEsInfo->u4CrcResult);
        prVdecEsInfo->u4CrcResult = 0;
    }
#endif

        #ifdef VDEC_TIME_PROFILE
        UINT8 i = 0;

        NEW_A_LINE;
        strcat(acProfieInfo, "Pic Type Idx,Max HW Decode MS,Min HW Decode MS,Total HW Decode MS,Total Frm/Fld Cnt,Max Dram Cyle,Max Pic Size,");
        NEW_A_LINE;
        for ( i = 0; i < 5; i++ )
        {
            sprintf(acProfileTmp, "%ld,", i);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->au4DecTime[i][0]);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->au4DecTime[i][1]);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->au4DecTime[i][2]);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->au4DecTime[i][3]);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->au4DecTime[i][8]);
            strcat(acProfieInfo, acProfileTmp);
            sprintf(acProfileTmp, "%ld,", prVdecEsInfo->au4DecTimeMaxPicSize[i][0]);
            strcat(acProfieInfo, acProfileTmp);
            NEW_A_LINE;
        }
        _VdecTimeProfileSaveToFile(ucEsId);

        x_memset(&(prVdecEsInfo->ucAutoPlayCurFileDir[0]), 0x0, sizeof(prVdecEsInfo->ucAutoPlayCurFileDir));
        x_memset(&(prVdecEsInfo->ucAutoPlayCurFileName[0]), 0x0, sizeof(prVdecEsInfo->ucAutoPlayCurFileName));
        #endif

        // if this is the last video frame,
        // do the same thing in VDEC_IsNotDisplay and VDEC_CheckStopStatus
        if (prVdecEsInfo->fgRenderBackStep)
        {
            prVdecEsInfo->fgRenderBackStep = FALSE;
            prVdecEsInfo->u8RenderFromPos = 0;
            prVdecEsInfo->i4RenderTempRef = 0;
            prVdecEsInfo->u2RenderDecodingOrder = 0;

            if(prVdecEsInfo->u2Disp2EmptyCnt > 0)
            {
            }
            else
            {
                LOG(5, "_VDEC_DeliverEos fgRenderBackStep have no previous frame\n");
            }

            if(prVdecEsInfoKeep->pfnTrickPtsCb)
            {
                // We have put two to empty that originaly should put to display.
                // That means we have previous display frame to empty queue.
                if(prVdecEsInfo->u2Disp2EmptyCnt > 1)
                {
                    prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, FALSE);
                    LOG(5, "_VDEC_DeliverEos pfnTrickPtsCb fgRenderBackStep finish\n");
                }
                else
                {
                    // Send again with far position
                    prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, TRUE);
                    LOG(5, "_VDEC_DeliverEos pfnTrickPtsCb fgRenderBackStep again\n");
                }
                //x_thread_delay(33);
            }
            if(prVdecEsInfoKeep->pfnRenderPtsCb)
            {
                prVdecEsInfoKeep->pfnRenderPtsCb(prVdecEsInfo->u4PTSLast, prVdecEsInfoKeep->u1AttachedSrcId);
                LOG(3, "_VDEC_DeliverEos pfnRenderPtsCb fgRenderBackStep\n");
            }

            VDEC_Pause(ucEsId);
            // switch to cmd queue (send cmd)
            x_thread_delay(1);

            LOG(3, "_VDEC_DeliverEos fgRenderBackStep\n");
            return;
        }

        //maybe cmpb flow also should do it(andy.zha to confirm)
        if (prVdecEsInfoKeep->fgVPush)
        {            
            LOG(3, "Always flush DPB when eos\n");
            prVdecEsInfo->fgForceFlush = TRUE;
            _VdecFlushDPB(ucEsId);            
            prVdecEsInfo->fgForceFlush = FALSE;
        }

        if (prVdecEsInfo->ucFbgId != FBM_FBG_TYPE_UNKNOWN)
        {
            if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
            {
                //flush dpb
                #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
                #ifndef CC_VDEC_H264_DISABLE
                if (prVdecEsInfoKeep->fgVPushFBGFromInst) // TODO: please h.264 owner to check
                {
                    // TODO, enable it later
                    LOG(0, "KeepPes %d\n", prVdecEsInfo->fgKeepPes);
                    _VDEC_H264Flush(ucEsId, TRUE);
                }
                else
                {
                    if(!prVdecEsInfo->fgKeepPes)
                    {
                        _VDEC_H264Flush(ucEsId, FALSE);
                    }
                    else
                    {
                        _VDEC_H264Flush(ucEsId, TRUE);
                    }
                }
                #endif
                #endif
            }
            else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
            {
                #ifndef CC_VDEC_H265_DISABLE
                if (prVdecEsInfoKeep->fgVPushFBGFromInst) // TODO: please h.264 owner to check
                {
                    LOG(0,"[VDEC-H265] Need Implement later\n");
                }
                else
                {
                    if(!prVdecEsInfo->fgKeepPes)
                    {
                        vVDecH265Flush(ucEsId, FALSE);
                    }
                    else
                    {
                        vVDecH265Flush(ucEsId, TRUE);
                    }
                }
                #endif
            }
            else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_AVS)
            {
                //flush dpb
                #if defined(CC_AVS_SUPPORT)
                #ifndef CC_VDEC_AVS_DISABLE
                if(!prVdecEsInfo->fgKeepPes)
                {
                    _VDEC_AVSFlush(ucEsId, FALSE);
                }
                else
                {
                    _VDEC_AVSFlush(ucEsId, TRUE);
                }
                #endif
                #endif
            }
            else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP6)
            {
            #if defined(CC_VP6_SUPPORT)
            #ifndef CC_VDEC_VP6_DISABLE
                if(!prVdecEsInfo->fgKeepPes)
                {
                    _VDEC_VP6Flush(ucEsId, FALSE);
                }
                else
                {
                    _VDEC_VP6Flush(ucEsId, TRUE);
                }
            #endif
            #endif
            }
            else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP8)
            {
            #if defined(CC_VP8_SUPPORT)
            #ifndef CC_VDEC_VP8_DISABLE
                if(!prVdecEsInfo->fgKeepPes)
                {
                    _VDEC_Vp8Flush(ucEsId, FALSE);
                }
                else
                {
                    _VDEC_Vp8Flush(ucEsId, TRUE);
                }
            #endif
            #endif
            }
            else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP9)
            {
            #ifndef CC_VDEC_VP9_DISABLE
                _VDEC_Vp9Flush(ucEsId, TRUE);
            #endif
            }            
            else
            {
                // error handle for not finished frame. Ex. the last one is field.
                ucFbId = FBM_GetDecFrameBuffer(prVdecEsInfo->ucFbgId);
                ucFbStatus = FBM_GetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId);
                if ((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||
                    (ucFbStatus == FBM_FB_STATUS_READY))
                {
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
                }
                // ~error

                //Put Backward Reference pic to DISPQ
                FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
                LOG(3, "DeEos ucBRefFbId %d ucFRefFbid %d\n", ucBRefFbId,ucFRefFbId);
                prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucBRefFbId);
                if(prFbmPicHdr)
                {
                    if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_READY))
                    {
                        //UNUSED(_MPV_CalcPts(ucEsId, ucBRefFbId));
                        if(!prVdecEsInfo->fgKeepPes)
                        {
                            LOG(3, "DeEos ucBRefFbId %d -> empty\n", ucBRefFbId);
                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_EMPTY);
                            if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK))
                            {
                                // when New-OMX deblocking mode, we have to return dbk FB when eos occurred immediately
                                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                                {
                                    LOG(3, "DeEos prVdecEsInfo->ucDbkFbId %d --> empty\n",prVdecEsInfo->ucDbkFbId);                             
                                    LOG(3, "DeEos prVdecEsInfo->ucDbkRefId %d --> empty\n",prVdecEsInfo->ucDbkRefId); 
                                    LOG(3, "Flush prVdecEsInfo->ucPicType %d\n",prVdecEsInfo->ucPicType);
                                    // dbkID == fbId when Pic == B, avoid this case.
                                    if(prVdecEsInfo->ucDbkFbId != FBM_FB_ID_UNKNOWN)
                                    {
                                        if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucDbkFbId, FBM_FB_STATUS_READY))
                                        {
                                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId,  prVdecEsInfo->ucDbkFbId, FBM_FB_STATUS_EMPTY);                                
                                         }
                                    }
                                    
                                    if(prVdecEsInfo->ucDbkRefId != FBM_FB_ID_UNKNOWN && prVdecEsInfo->ucDbkRefId != prVdecEsInfo->ucDbkFbId )
                                    {                            
                                        if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucDbkRefId, FBM_FB_STATUS_READY))
                                        {
                                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId,  prVdecEsInfo->ucDbkRefId, FBM_FB_STATUS_EMPTY);                                
                                        }
                                    }                                  
                                    prVdecEsInfo->ucDbkFbId = FBM_FB_ID_UNKNOWN;                                    
                                    prVdecEsInfo->ucDbkRefId  = FBM_FB_ID_UNKNOWN;
                                }
                            }
                        }
                        else
                        {
                            if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                            {
                                UNUSED(_MPV_CalcPts(ucEsId, ucBRefFbId));
                            }
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_MPEG4_DISABLE
                            else if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)
                            {
                                UNUSED(_VDEC_MPEG4CalPts(ucEsId, ucBRefFbId));
                            }
#endif
#endif

                            #ifdef CC_REALD_3D_SUPPORT
                            _VDEC_Check2D3DSwitch(ucEsId, ucBRefFbId);
                            #endif
                            _VDEC_JobBeforePutDispQ(ucEsId, ucBRefFbId);

                            if(STC_GetSrc(0, &rStcClk) == STC_VALID)
                            {
                                #if BDP_LOG_STYLE
                                VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, STC = %d, FbgID = %d, FbId = %d\n",ucEsId,
                                       prFbmPicHdr->ucPicCdTp,prFbmPicHdr->u4PTS, rStcClk.u4Base, prVdecEsInfo->ucFbgId, ucBRefFbId);
                                #endif
                            }

                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_DISPLAYQ);

                            if(prVdecEsInfo->u4DisplayQPicCnt == 0)
                            {
                                STC_StopStc(prVdecEsInfo->ucStcId);
                                STC_SetStcValue(prVdecEsInfo->ucStcId, (UINT32)(prFbmPicHdr->u4PTS));
                                STC_StartStc(prVdecEsInfo->ucStcId);
                            }

                            prVdecEsInfo->u4DisplayQPicCnt++;
                            LOG(3, "DeEos ucBRefFbId %d -> display\n", ucBRefFbId);
                        }
                    }
                    if(FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_READY))
                    {
                        //UNUSED(_MPV_CalcPts(ucEsId, ucFRefFbId));
                        if(!prVdecEsInfo->fgKeepPes)
                        {
                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_EMPTY);
                            LOG(3, "DeEos ucFRefFbId %d -> empty\n", ucFRefFbId);
                        }
                        else
                        {
                            if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                            {
                                UNUSED(_MPV_CalcPts(ucEsId, ucFRefFbId));
                            }
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_MPEG4_DISABLE
                            else if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)
                            {
                                UNUSED(_VDEC_MPEG4CalPts(ucEsId, ucBRefFbId));
                            }
#endif
#endif
                            #ifdef CC_REALD_3D_SUPPORT
                            _VDEC_Check2D3DSwitch(ucEsId, ucFRefFbId);
                            #endif
                            _VDEC_JobBeforePutDispQ(ucEsId, ucFRefFbId);

                            if(STC_GetSrc(0, &rStcClk) == STC_VALID)
                            {
                                #if BDP_LOG_STYLE
                                VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, STC = %d, FbgID = %d, FbId = %d\n",ucEsId,
                                       prFbmPicHdr->ucPicCdTp,prFbmPicHdr->u4PTS, rStcClk.u4Base, prVdecEsInfo->ucFbgId, ucFRefFbId);
                                #endif
                            }

                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_DISPLAYQ);

                            if(prVdecEsInfo->u4DisplayQPicCnt == 0)
                            {
                                STC_StopStc(prVdecEsInfo->ucStcId);
                                STC_SetStcValue(prVdecEsInfo->ucStcId, (UINT32)(prFbmPicHdr->u4PTS));
                                STC_StartStc(prVdecEsInfo->ucStcId);
                            }

                            prVdecEsInfo->u4DisplayQPicCnt++;
                            LOG(3, "DeEos ucFRefFbId %d -> display\n", ucFRefFbId);
                        }
                    }
                }
            }

            //Deliver a black picture if we don't get any video frame            
            // error handle for not finished frame. Ex. the last one is field.
            ucFbId = FBM_GetDecFrameBuffer(prVdecEsInfo->ucFbgId);
            if (ucFbId != FBM_FB_ID_UNKNOWN)
            {
                LOG(3, "[%s] DecFb(%d) Status(%d) clear.\n", __func__,
                    ucFbId, FBM_GetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId));
                
                FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
            }
            // Also deliver EOS.
            if(prVdecEsInfo->u4DisplayQPicCnt == 0)
            {
                if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MJPEG)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_AVS)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_RAW)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP8)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP6)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP9))
                {
                    ucFbId = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, prVdecEsInfo->u4WaitDispTime);
                }
                else
                {
                    ucFbId = FBM_GetEmptyBFrameBuffer(prVdecEsInfo->ucFbgId,
                                        prVdecEsInfo->u4WaitDispTime);
                }

                prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbId);
                if(prFbmPicHdr)
                {
                    UINT32 u4AddrY;
                    UINT32 u4AddrC;
                    UINT32 u4FBWidth = 0;
                    UINT32 u4FBHeight = 0;
                    
                    STC_StopStc(prVdecEsInfo->ucStcId);
                    prFbmPicHdr->i4TemporalRef = 0;
                    prFbmPicHdr->u2DecodingOrder = 0;
                    // DTV00139938, [MM]Flash wrong progress bar after excute FR2X and FF2X several times during playback
                    // should base on direct to send dummy black frame
                    if(prVdecEsInfo->u4MMSpeed <= STC_SPEED_TYPE_FORWARD_1_DOT_5X)
                    {
                        if(prVdecEsInfo->rABInfo.fgValid)
                        {
                            prFbmPicHdr->rExtra.u8PTSI = prVdecEsInfo->rABInfo.rB.u8PtsI;
                            prFbmPicHdr->rExtra.u8AudPTS = prVdecEsInfo->rABInfo.rB.u8AudPts;
                            prFbmPicHdr->rExtra.u8AudOffset = prVdecEsInfo->rABInfo.rB.u8AudOffset;
                            prFbmPicHdr->u4PTS = (UINT32)prVdecEsInfo->rABInfo.rB.u8Pts;
                            prFbmPicHdr->u4TotlaTimeOffset = prVdecEsInfo->u4TotlaTimeOffset;
                            prFbmPicHdr->u8Offset = prVdecEsInfo->rABInfo.rB.u8Offset;
                            prFbmPicHdr->u8OffsetDisp = prVdecEsInfo->rABInfo.rB.u8OffsetDisp;
                            prFbmPicHdr->u8OffsetI = prVdecEsInfo->rABInfo.rB.u8OffsetI;
                            prFbmPicHdr->i4TemporalRef =
                                prVdecEsInfo->rABInfo.rB.i4TemporalRef;
                            prFbmPicHdr->u2DecodingOrder =
                                prVdecEsInfo->rABInfo.rB.u2DecodingOrder;
                            STC_SetStcValue(prVdecEsInfo->ucStcId,
                                (UINT32)(prFbmPicHdr->u4PTS));
                        }
                        else
                        {
                            prFbmPicHdr->rExtra.u8PTSI = prVdecEsInfo->u4TotalTime;
                            prFbmPicHdr->rExtra.u8AudPTS = prVdecEsInfo->u4TotalTime;
                            prFbmPicHdr->rExtra.u8AudOffset = prVdecEsInfo->u8FileSize;
                            prFbmPicHdr->u4PTS = prVdecEsInfo->u4TotalTime;
                            prFbmPicHdr->u4TotlaTimeOffset = prVdecEsInfo->u4TotlaTimeOffset;
                            prFbmPicHdr->u8Offset = prVdecEsInfo->u8FileSize;
                            prFbmPicHdr->u8OffsetDisp = prVdecEsInfo->u8FileSize;
                            prFbmPicHdr->u8OffsetI = prVdecEsInfo->u8FileSize;
                            // do not set stc in this case, mw will handle
                            //STC_SetStcValue(prVdecEsInfo->ucStcId,
                            //    (UINT32)(prFbmPicHdr->u4PTS));
                        }
                    }
                    else
                    {
                        if(prVdecEsInfo->rABInfo.fgValid)
                        {
                            prFbmPicHdr->rExtra.u8PTSI = prVdecEsInfo->rABInfo.rA.u8PtsI;
                            prFbmPicHdr->rExtra.u8AudPTS = prVdecEsInfo->rABInfo.rA.u8AudPts;
                            prFbmPicHdr->rExtra.u8AudOffset = prVdecEsInfo->rABInfo.rA.u8AudOffset;
                            prFbmPicHdr->u4PTS = (UINT32)prVdecEsInfo->rABInfo.rA.u8Pts;
                            prFbmPicHdr->u4TotlaTimeOffset = prVdecEsInfo->u4TotlaTimeOffset;
                            prFbmPicHdr->u8Offset = prVdecEsInfo->rABInfo.rA.u8Offset;
                            prFbmPicHdr->u8OffsetDisp = prVdecEsInfo->rABInfo.rA.u8OffsetDisp;
                            prFbmPicHdr->u8OffsetI = prVdecEsInfo->rABInfo.rA.u8OffsetI;
                            prFbmPicHdr->i4TemporalRef =
                                prVdecEsInfo->rABInfo.rA.i4TemporalRef;
                            prFbmPicHdr->u2DecodingOrder =
                                prVdecEsInfo->rABInfo.rA.u2DecodingOrder;
                            STC_SetStcValue(prVdecEsInfo->ucStcId,
                                (UINT32)(prFbmPicHdr->u4PTS));
                        }
                        else
                        {
                            prFbmPicHdr->rExtra.u8PTSI = 0;
                            prFbmPicHdr->rExtra.u8AudPTS = 0;
                            prFbmPicHdr->rExtra.u8AudOffset = 0;
                            prFbmPicHdr->u4PTS = 0;
                            prFbmPicHdr->u4TotlaTimeOffset = 0;
                            prFbmPicHdr->u8Offset = 0;
                            prFbmPicHdr->u8OffsetDisp = 0;
                            prFbmPicHdr->u8OffsetI = 0;
                            STC_SetStcValue(prVdecEsInfo->ucStcId, (UINT32)(0));
                        }
                    }
                    STC_StartStc(prVdecEsInfo->ucStcId);

                    u4AddrY = 0;
                    u4AddrC = 0;
                    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                    FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, ucFbId, &u4AddrY, &u4AddrC);
                    FBM_GetFrameBufferSize(prVdecEsInfo->ucFbgId, &u4FBWidth, &u4FBHeight);
                    if((u4AddrY > 0) && (u4AddrC > 0) && prFbmSeqHdr)
                    {
                        #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
                        x_memset(&prVdecFbOp,0x0,sizeof(VDEC_TO_TZ_FB_OPERATION_T));
                        prVdecFbOp.u4SrcAddr = u4AddrY;
                        prVdecFbOp.u4Value = 0;
                        prVdecFbOp.u4Size = (u4FBWidth*u4FBHeight);
                        TZ_VDEC_FB_Operation((VOID *)&prVdecFbOp, TZ_VDEC_FB_MEMSET);
                        x_memset(&prVdecFbOp,0x0,sizeof(VDEC_TO_TZ_FB_OPERATION_T));
                        prVdecFbOp.u4SrcAddr = u4AddrC;
                        prVdecFbOp.u4Value = 128;
                        prVdecFbOp.u4Size = (u4FBWidth*u4FBHeight)/2;
                        TZ_VDEC_FB_Operation((VOID *)&prVdecFbOp, TZ_VDEC_FB_MEMSET);
                        #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
                        x_memset(&prVdecFbOp,0x0,sizeof(VDEC_TO_TZ_FB_OPERATION_T));
                        prVdecFbOp.u4SrcAddr = u4AddrY;
                        prVdecFbOp.u4Size = (u4FBWidth*u4FBHeight);
                        TZ_VDEC_FB_Operation((VOID *)&prVdecFbOp, TZ_VDEC_FB_FLUSHMUTILINE);
                        x_memset(&prVdecFbOp,0x0,sizeof(VDEC_TO_TZ_FB_OPERATION_T));
                        prVdecFbOp.u4SrcAddr = u4AddrC;
                        prVdecFbOp.u4Size = (u4FBWidth*u4FBHeight)>>1;
                        TZ_VDEC_FB_Operation((VOID *)&prVdecFbOp, TZ_VDEC_FB_FLUSHMUTILINE);
                        #else
                        HalFlushInvalidateDCache();
                        #endif
                        #else  /* ! (CC_TRUSTZONE_SUPPORT &&  CC_TVP_SUPPORT)*/
                        
                        #ifndef CC_VDEC_FBM_DYNAMIC_MAPPING
                        x_memset((void*)VIRTUAL(u4AddrY), 0, (u4FBWidth*u4FBHeight));
                        x_memset((void*)VIRTUAL(u4AddrC), 128, (u4FBWidth*u4FBHeight)/2);
                        #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
                        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4AddrY), (u4FBWidth*u4FBHeight));
                        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4AddrC), (u4FBWidth*u4FBHeight)>>1);
                        #else
                        HalFlushInvalidateDCache();
                        #endif
                        #else /*CC_VDEC_FBM_DYNAMIC_MAPPING*/
                        GFX_Memset((void*)(u4AddrY), u4FBWidth, u4FBHeight, 0);
                        GFX_Memset((void*)(u4AddrC), u4FBWidth, u4FBHeight/2, 128);
                        #endif /*CC_VDEC_FBM_DYNAMIC_MAPPING*/
                        #endif  /* ! (CC_TRUSTZONE_SUPPORT &&  CC_TVP_SUPPORT)*/
                        

                        if(prVdecEsInfoKeep->rPesInfo.fgBackward)
                        {
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_BACKWARD_FLAG);
                        }
                        FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_PSEUDO_EOS_FLAG);
                        #ifdef CC_REALD_3D_SUPPORT
                        _VDEC_Check2D3DSwitch(ucEsId, ucFbId);
                        #endif
                        _VDEC_JobBeforePutDispQ(ucEsId, ucFbId);

                        if(STC_GetSrc(0, &rStcClk) == STC_VALID)
                        {
                            #if BDP_LOG_STYLE
                            VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, STC = %d, FbgID = %d, FbId = %d\n",ucEsId,
                                   prFbmPicHdr->ucPicCdTp,prFbmPicHdr->u4PTS, rStcClk.u4Base, prVdecEsInfo->ucFbgId, ucFbId);
                            #endif
                        }

                        FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_DISPLAYQ);

                        LOG(1, "%s(L:%d)(%d)DeEos Black Image ucFbId %d\n", __FUNCTION__, __LINE__, ucEsId, ucFbId);
                    }
                    else
                    {
                        FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_EOS_FLAG);
                        FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
                        LOG(1, "%s(L:%d)(%d)DeEos Black Image null\n", __FUNCTION__, __LINE__, ucEsId);
                    }
                }
                else
                {
                    FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_EOS_FLAG);
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
                    LOG(1, "%s(L:%d)(%d)DeEos Black Image prFbmPicHdr NULL", __FUNCTION__, __LINE__, ucEsId);
                }
                if((prVdecEsInfo->ucSubFbgId != 0xFF)&&(prVdecEsInfo->e3DType == VDEC_3D_MVC))
                {
                    ucFbIdSub = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucSubFbgId, prVdecEsInfo->u4WaitDispTime);
                    prFbmPicHdrSub = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucSubFbgId, ucFbIdSub);
                }
                else if (prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {
                    ucFbIdSub = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, prVdecEsInfo->u4WaitDispTime);
                    prFbmPicHdrSub = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbIdSub);
                }
                if(((prVdecEsInfo->ucSubFbgId != 0xFF)&&(prVdecEsInfo->e3DType == VDEC_3D_MVC)) \
                   || (prVdecEsInfo->e3DType == VDEC_3D_BIVL))
                {
                    if(prFbmPicHdrSub)
                   {
                        //ucFbStatus = FBM_GetFrameBufferStatus(prVdecEsInfo->ucSubFbgId, ucFbIdSub);
                        // forward speed EOS
                        if(prVdecEsInfo->e3DType == VDEC_3D_MVC)
                        {
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucSubFbgId, ucFbIdSub, FBM_MM_NOT_DISPLAY_FLAG);
                        }
                        else if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                        {
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbIdSub, FBM_MM_EOS_FLAG);
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbIdSub, FBM_MM_NOT_DISPLAY_FLAG);
                            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                            prFbmPicHdrSub->fgBivl3DLR = 0;//Right eye
                            if(prFbmSeqHdr && prFbmPicHdr)
                            {
                                prFbmPicHdrSub->u4PTS = prFbmPicHdr->u4PTS + prFbmSeqHdr->u4Duration;
                            }
                            else
                            {
                                prFbmPicHdrSub->u4PTS = 1502;
                            }
                        }
                        
                        #ifdef CC_REALD_3D_SUPPORT
                        _VDEC_Check2D3DSwitch(ucEsId, ucFbId);//subFbid ???
                        #endif
                        _VDEC_JobBeforePutDispQ(ucEsId, ucFbId);
                        if(prVdecEsInfo->e3DType == VDEC_3D_MVC)
                        {
                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucSubFbgId, ucFbIdSub, FBM_FB_STATUS_DISPLAYQ);
                            if (VDEC_PRINT_PQ_INFO & prVdecEsInfoKeep->u4LOGCtrl)
                            {
                                LOG(0, "PQ(%d) (%d %3d) (%d %d) %X/%llX \n", ucEsId,
                                   prFbmPicHdr?prFbmPicHdr->ucPicCdTp:0,
                                   prFbmPicHdr?prFbmPicHdr->i4TemporalRef:0xFF,
                                   prVdecEsInfo->ucSubFbgId, ucFbIdSub,
                                   prFbmPicHdr?prFbmPicHdr->u4PTS:0x0,
                                   prFbmPicHdr?prFbmPicHdr->u8PTS:0x0);
                            }

                        }
                        else if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                        {
                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbIdSub, FBM_FB_STATUS_DISPLAYQ);
                        }
                        LOG(1, "%s(L:%d)(%d)DeEos Sub ucFbId %d\n", __FUNCTION__, __LINE__, ucEsId, ucFbIdSub);
                    }
                    else
                    {
                        FBM_SetFrameBufferStatus(prVdecEsInfo->ucSubFbgId, ucFbIdSub, FBM_FB_STATUS_EMPTY);
                        LOG(1, "%s(L:%d)DeEos prFbmPicHdr NULL", __FUNCTION__, __LINE__);
                    }
                }
            }
            else
            {
                if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MJPEG)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_AVS)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_RAW)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP8)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP6)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
                    ||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP9))
                {
                    ucFbId = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, prVdecEsInfo->u4WaitDispTime);                    
                }
                else
                {
                    ucFbId = FBM_GetEmptyBFrameBuffer(prVdecEsInfo->ucFbgId,
                                        prVdecEsInfo->u4WaitDispTime);
                }
                //Deliver EOS
                prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbId);
                if(prFbmPicHdr)
                {
                    if(prVdecEsInfoKeep->rPesInfo.fgBackward)
                    {
                        FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_BACKWARD_FLAG);
                    }
                    FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_EOS_FLAG);
                    FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_NOT_DISPLAY_FLAG);
                    if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                    {
                        prFbmPicHdr->u4PTS = 0;
                        prFbmPicHdr->fgBivl3DLR = 1;
                    }
                    //UNUSED(_MPV_CalcPts(ucEsId, ucFbId));
                    #ifdef CC_REALD_3D_SUPPORT
                    _VDEC_Check2D3DSwitch(ucEsId, ucFbId);
                    #endif
                    _VDEC_JobBeforePutDispQ(ucEsId, ucFbId);

                    if(STC_GetSrc(0, &rStcClk) == STC_VALID)
                    {
                        #if BDP_LOG_STYLE
                        VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, STC = %d, FbgID = %d, FbId = %d\n",ucEsId,
                               prFbmPicHdr->ucPicCdTp,prFbmPicHdr->u4PTS, rStcClk.u4Base, prVdecEsInfo->ucFbgId, ucFbId);
                        #endif
                    }

                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_DISPLAYQ);
                    LOG(1, "%s(L:%d)(%d)DeEos ucFbId %d\n", __FUNCTION__, __LINE__, ucEsId, ucFbId);
                }
                else
                {
                    FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_EOS_FLAG);
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
                    prVdecEsInfo->fgSendedEos = FALSE;
                    x_memset((void*)&(prVdecEsInfoKeep->rPesInfo), 0, sizeof(prVdecEsInfoKeep->rPesInfo));
                    prVdecEsInfoKeep->rPesInfo.fgEos = TRUE;
                    VDEC_SendEs((void*)&(prVdecEsInfoKeep->rPesInfo));
                    LOG(1, "%s(L:%d)(%d)DeEos prFbmPicHdr NULL", __FUNCTION__, __LINE__, ucEsId);
                }
                if((prVdecEsInfo->ucSubFbgId != 0xFF)&&(prVdecEsInfo->e3DType == VDEC_3D_MVC))
                {
                    ucFbIdSub = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucSubFbgId, prVdecEsInfo->u4WaitDispTime);
                    prFbmPicHdrSub = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucSubFbgId, ucFbIdSub);
                }
                else if (prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {
                    ucFbIdSub = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, prVdecEsInfo->u4WaitDispTime);
                    prFbmPicHdrSub = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbIdSub);
                }
                //send Sub Pic
                if(((prVdecEsInfo->ucSubFbgId != 0xFF)&&(prVdecEsInfo->e3DType == VDEC_3D_MVC)) \
                    || (prVdecEsInfo->e3DType == VDEC_3D_BIVL))
                {
                    if(prFbmPicHdrSub)
                   {
                        //ucFbStatus = FBM_GetFrameBufferStatus(prVdecEsInfo->ucSubFbgId, ucFbIdSub);
                        // forward speed EOS
                        if(prVdecEsInfo->e3DType == VDEC_3D_MVC)
                        {
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucSubFbgId, ucFbIdSub, FBM_MM_NOT_DISPLAY_FLAG);
                        }
                        else if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                        {
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbIdSub, FBM_MM_EOS_FLAG);
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbIdSub, FBM_MM_NOT_DISPLAY_FLAG);
                            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                            prFbmPicHdrSub->fgBivl3DLR = 0;//Right eye
                            if(prFbmSeqHdr && prFbmPicHdr)
                            {
                                prFbmPicHdrSub->u4PTS = prFbmPicHdr->u4PTS + prFbmSeqHdr->u4Duration;
                            }
                            else
                            {
                                prFbmPicHdrSub->u4PTS = 1502;
                            }
                        }
                        #if BDP_LOG_STYLE
                        VDecPrintf(ucEsId,VDEC_LOG_VDEC2VDP, "[VDEC-VDP(%d)] PicType = %d , PTS = %d, FbgID = %d, FbId = %d\n",ucEsId,
                               prFbmPicHdrSub->ucPicCdTp,prFbmPicHdrSub->u4PTS,  prVdecEsInfo->ucSubFbgId, ucFbIdSub);
                        #endif

                        #ifdef CC_REALD_3D_SUPPORT
                        _VDEC_Check2D3DSwitch(ucEsId, ucFbId);//or subFbid ???
                        #endif
                        _VDEC_JobBeforePutDispQ(ucEsId, ucFbId);
                        if(prVdecEsInfo->e3DType == VDEC_3D_MVC)
                        {
                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucSubFbgId, ucFbIdSub, FBM_FB_STATUS_DISPLAYQ);
                        }
                        else if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                        {
                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbIdSub, FBM_FB_STATUS_DISPLAYQ);
                        }
                        LOG(1, "%s(L:%d)(%d)DeEos Sub ucFbId %d\n", __FUNCTION__, __LINE__, ucEsId, ucFbIdSub);
                    }
                    else
                    {
                        FBM_SetFrameBufferStatus(prVdecEsInfo->ucSubFbgId, ucFbIdSub, FBM_FB_STATUS_EMPTY);
                        LOG(1, "%s(L:%d)DeEos prFbmPicHdr NULL", __FUNCTION__, __LINE__);
                    }
                }
            }
        }
        else
        {
            if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV ) ||
                (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)||
                (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264))
              {
                    //TODO: FBM_CreateGroupExt
                    FBM_CREATE_FBG_PAR_T rPar;
                    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                    rPar.u1VdecId = (UINT8)ucEsId;

                    if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX;
                    }
                    else if(prVdecEsInfoKeep->fgVPush && prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL)
                    {
                        rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
                    }
                    else
                    {
                        rPar.u1AppMode = FBM_FBG_APP_NORMAL;
                    }

                    if (prVdecEsInfoKeep->fgVPushFBGFromInst)
                    {
                        LOG(3, "fgCreateFromInst == TRUE\n");
                        rPar.fgCreateFromInst = TRUE;
                    }

                    LOG(6, "Create FBG\n");
                    if (prVdecEsInfo->u4RenderPicCnt)
                    {
                        rPar.fgThumbnailMode = TRUE;
                    }
                    ucFbgType = FBM_SelectGroupType(720, 480);
                    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV )
                    {
                        prVdecEsInfo->ucFbgId =
                        FBM_CreateGroupExt(ucFbgType, FBM_VDEC_MPEG2,
                                720, 480, &rPar);
                    }
                    else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MP4)
                    {
                        prVdecEsInfo->ucFbgId =
                        FBM_CreateGroupExt(ucFbgType, FBM_VDEC_MPEG4,
                                720, 480, &rPar);
                    }
                    else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
                    {
                        prVdecEsInfo->ucFbgId =
                        FBM_CreateGroupExt(ucFbgType, FBM_VDEC_H264,
                                720, 480, &rPar);
                    }
                    if(prVdecEsInfo->fgMMPlayback)
                    {
                        FBM_SetPlayMode(prVdecEsInfo->ucFbgId,FBM_FBG_MM_MODE);
                    }
                    if(prVdecEsInfoKeep->fgVPush)
                    {
                        if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                        {
                            FBM_SetPlayMode(prVdecEsInfo->ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
                        }
                        FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                                FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                        if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                        {
                            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                                    FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                        }
                    }
                    else if (prVdecEsInfoKeep->fgFrameToUser)
                    {
                        FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                          FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_Vdec_IFrameDecodeDone, ucEsId);
                    }
                    if(prVdecEsInfo->fgPlaybackNotDisplay)
                    {
                        FBM_SetPlayMode(prVdecEsInfo->ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
                    }

                    if(prVdecEsInfo->ucFbgId != FBM_FBG_ID_UNKNOWN)
                    {
                        FBM_FbgChgNotify(prVdecEsInfo->ucFbgId, ucEsId);
                        FBM_SetSyncStc(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);

                        // Check if reset is required
                        if (FBM_ChkFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_RESET))
                        {
                            FBM_ResetGroup(prVdecEsInfo->ucFbgId);
                            FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_RESET);
                            FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_NOT_READY);
                        }


                        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
                        {
                            ucFbId = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, prVdecEsInfo->u4WaitDispTime);
                        }
                        else
                        {
                            ucFbId = FBM_GetEmptyBFrameBuffer(prVdecEsInfo->ucFbgId,
                                                prVdecEsInfo->u4WaitDispTime);
                        }

                        if(ucFbId != FBM_FB_ID_UNKNOWN)
                        {
                            if(prVdecEsInfoKeep->rPesInfo.fgBackward)
                            {
                                FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_BACKWARD_FLAG);
                            }
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_EOS_FLAG);
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_NOT_DISPLAY_FLAG);
                            #ifdef CC_REALD_3D_SUPPORT
                            _VDEC_Check2D3DSwitch(ucEsId, ucFbId);
                            #endif
                            _VDEC_JobBeforePutDispQ(ucEsId, ucFbId);

                            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_DISPLAYQ);
                            LOG(5, "%s(L:%d)(%d)DeEos ucFbId %d\n", __FUNCTION__, __LINE__, ucEsId, ucFbId);
                        }
                        else
                        {
                            LOG(1, "Get empty frame buffer fail\n");
                        }
                    }
                else
                {
                    LOG(3, "Create FBG fail\n");
                }
            }

            if ((prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_NO_DATA) &&
                (prVdecEsInfoKeep->pfDecNfyFct))    //enable after mw add this handler
            {
                LOG(3, "ES(%d) Notify Status change, decode no data\n", ucEsId);
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                    VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_NO_DATA, 0);
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_NO_DATA;
            }
        }
        // notify decode finish, in this case, we didn't get any video data, but eos.
        if (prVdecEsInfo->fgRenderFromFirstPic)
        {
            prVdecEsInfo->fgRenderFromFirstPic = FALSE;
            if(prVdecEsInfoKeep->pfnRenderPtsCb)
            {
                prVdecEsInfoKeep->pfnRenderPtsCb(
                    (prVdecEsInfo->u4TotalTime + prVdecEsInfo->u4TotlaTimeOffset),
                    prVdecEsInfoKeep->u1AttachedSrcId);
                LOG(3, "_VDEC_DeliverEos pfnRenderPtsCb fgRenderFromFirstPic\n");
            }
        }
        else if ((prVdecEsInfo->fgRenderPts) &&
            (prVdecEsInfo->u4PTS == prVdecEsInfo->u4RenderPts))
        {
            prVdecEsInfo->fgRenderPts = FALSE;
            prVdecEsInfo->u4RenderPts = 0;
            if(prVdecEsInfoKeep->pfnRenderPtsCb)
            {
                prVdecEsInfoKeep->pfnRenderPtsCb(
                    (prVdecEsInfo->u4TotalTime + prVdecEsInfo->u4TotlaTimeOffset),
                    prVdecEsInfoKeep->u1AttachedSrcId);
                LOG(3, "_VDEC_DeliverEos pfnRenderPtsCb fgRenderPts\n");
            }
        }

        if ((prVdecEsInfo->fgRenderFromPos) &&
            (prVdecEsInfo->u8Offset > prVdecEsInfo->u8RenderFromPos))
        {
            prVdecEsInfo->fgRenderFromPos = FALSE;
            prVdecEsInfo->u8RenderFromPos = 0;
            prVdecEsInfo->u2RenderDecodingOrder = 0;
            prVdecEsInfo->i4RenderTempRef = 0;
            if(prVdecEsInfoKeep->pfnRenderPtsCb)
            {
                prVdecEsInfoKeep->pfnRenderPtsCb(
                    (prVdecEsInfo->u4TotalTime + prVdecEsInfo->u4TotlaTimeOffset),
                    prVdecEsInfoKeep->u1AttachedSrcId);
                LOG(3, "_VDEC_DeliverEos pfnRenderPtsCb fgRenderFromPos\n");
            }
        }
        else if (prVdecEsInfo->fgRenderBackStep)
        {
            prVdecEsInfo->fgRenderBackStep = FALSE;
            prVdecEsInfo->u8RenderFromPos = 0;
            prVdecEsInfo->u2RenderDecodingOrder = 0;
            prVdecEsInfo->i4RenderTempRef = 0;
            if(prVdecEsInfoKeep->pfnRenderPtsCb)
            {
                prVdecEsInfoKeep->pfnRenderPtsCb(
                    (prVdecEsInfo->u4TotalTime + prVdecEsInfo->u4TotlaTimeOffset),
                    prVdecEsInfoKeep->u1AttachedSrcId);
                LOG(3, "_VDEC_DeliverEos pfnRenderPtsCb fgRenderBackStep\n");
            }
        }

        if (prVdecEsInfo->fgThumbMethod2 && prVdecEsInfoKeep->pfDecNfyFct)
        {
            LOG(2, "%s[%u] Es%d fgThumbMethod2=1 FoundKeyfrm(%d) Eos done\n",
                    __FUNCTION__, __LINE__, ucEsId, prVdecEsInfo->fgThumbStartTime);
                    
            prVdecEsInfoKeep->pfDecNfyFct(
                prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_PLAY_DONE, 0, 0);
          }
          
        LOG(3, "_VDEC_DeliverEos Done\n");
    }
}


void _VDEC_SendCmd(UCHAR ucEsId, UINT32 u4Cmd)
{
    #define VDEC_SENDCMD_RETRY_TIMES (10)
    #define VDEC_SENDCMD_RETRY_DELAY (30)
    UINT32 u4Cnt = 0;
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    UINT32 u4CmdDump;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    LOG(3, "Send Cmd(%d) 0x%x\n", (UINT32)ucEsId, (UINT32)u4Cmd);

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    i4Ret = x_msg_q_send(prVdecEsInfoKeep->hCmdQ,(void*)&u4Cmd, VDEC_CMD_SIZE, 255);
    if (i4Ret == OSR_TOO_MANY)
    {
        // we better not to lost any command.
        // retry sometimes if possible
        while(u4Cnt < VDEC_SENDCMD_RETRY_TIMES)
        {
            i4Ret = x_msg_q_send(prVdecEsInfoKeep->hCmdQ, (void *)&u4Cmd, VDEC_CMD_SIZE, 255);
            if (i4Ret == OSR_TOO_MANY)
            {
                u4Cnt++;
            }
            else
            {
                break;
            }
        }
        if(u4Cnt >= VDEC_SENDCMD_RETRY_TIMES)
        {
            LOG(1, "Send Cmd(%d) 0x%x fail!!!\n",
                (UINT32)ucEsId, (UINT32)u4Cmd);
        }
        else
        {
            LOG(3, "Send Cmd(%d) 0x%x full, retrying count(%d)\n",
                (UINT32)ucEsId, (UINT32)u4Cmd, u4Cnt);
        }
        zMsgSize = VDEC_CMD_SIZE;
        i4Ret = x_msg_q_receive(&u2MsgQIdx, &u4CmdDump, &zMsgSize,
                &(prVdecEsInfoKeep->hCmdQ), 1, X_MSGQ_OPTION_NOWAIT);
        VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_NO_MSG)); // no msg case is possible
        i4Ret = x_msg_q_send(prVdecEsInfoKeep->hCmdQ, (void *)&u4Cmd, VDEC_CMD_SIZE, 255);
    }
    else
    {
        VERIFY(i4Ret == OSR_OK);
    }

    //SYSLOG(60, ucEsId, u4Cmd, 0);

}

void _VDecReceiveCmd(UCHAR ucEsId, BOOL bIsBlock)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;
    UINT32 u4PreStatus;
    UINT32 u4Cmd;
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    #ifdef CC_REALD_3D_SUPPORT
    UCHAR ucLockFbId;
    #endif
    
    zMsgSize = VDEC_CMD_SIZE;

    if(bIsBlock)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, &u4Cmd, &zMsgSize,
                &(prVdecEsInfoKeep->hCmdQ), 1, X_MSGQ_OPTION_WAIT);
        ASSERT(i4Ret == OSR_OK);
    }
    else
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, &u4Cmd, &zMsgSize,
                &(prVdecEsInfoKeep->hCmdQ), 1, X_MSGQ_OPTION_NOWAIT);
        if (i4Ret == OSR_NO_MSG)
        {
            return;
        }
    }

    if (i4Ret == OSR_NO_MSG)
    {
        return;
    }
    else if (i4Ret == OSR_OK)
    {
        ASSERT(zMsgSize == VDEC_CMD_SIZE);
    }
    
#ifdef TIME_MEASUREMENT
    if (VDEC_CMD_PLAY == u4Cmd)
    {    
        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "Vdec Receive Play Cmd ");
        prVdecEsInfo->fgPrintReceiveEs = TRUE;
        prVdecEsInfo->fgPrintOutput = TRUE;
    }
#endif
    
    LOG(2, "Rsv Es%d u4Cmd %x\n", ucEsId, u4Cmd);

    u4PreStatus = (UINT32)prVdecEsInfoKeep->eCurState;

    //filter illegal command
    switch (u4Cmd)
    {
        case VDEC_CMD_STOP:
            prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
            prVdecEsInfoKeep->eCurState = VDEC_ST_STOP;
            prVdecEsInfoKeep->u4PreviousStopId = prVdecEsInfoKeep->u4CurStopId;
            prVdecEsInfoKeep->u4CurStopId ++;
            prVdecEsInfoKeep->fgWaitIFrame = TRUE;
            prVdecEsInfo->fgMvcMismatch = FALSE;
            prVdecEsInfo->fgNoFrameBuffer = FALSE;
            #ifdef CC_DUAL_CORE_DEC
            if((prVdecEsInfo->ucVDecStatus == REAL_DECODE) && prVdecEsInfo->ucDualCoreMode)
            {
                _VDEC_UnlockStopSema(ucEsId);
                _VDEC_LockStopSema(ucEsId);
                VDEC_Stop(prVdecEsInfo->ucLaeEsInfoId);
                LOG(2, "send lae stop cmd, esid %d\n", prVdecEsInfo->ucLaeEsInfoId);
            }
            if (prVdecEsInfo->ucVDecStatus == LAE_DECODE)
            {
                _VDEC_LockStopSema(VDEC_MAX_ES - ucEsId - 1);
                LOG(2, "mainesid %d, lockstopsema\n", (VDEC_MAX_ES - ucEsId - 1));
            }
            #endif
            break;
        case VDEC_CMD_PLAY:
        case VDEC_CMD_PLAY_I_FRAME:
            if(prVdecEsInfoKeep->eCurState == VDEC_ST_STOP)
            {           
                prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
                prVdecEsInfoKeep->eCurState = (u4Cmd == (UINT32)VDEC_CMD_PLAY) ? VDEC_ST_PLAY : VDEC_ST_PLAY_I_FRAME;
            }
            else if(prVdecEsInfoKeep->eCurState == VDEC_ST_IDLE)
            {
                if(VDEC_ChkCodecCap(ucEsId, prVdecEsInfoKeep->eCurFMT))
                {
                    prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
                    prVdecEsInfoKeep->eCurState = (u4Cmd == (UINT32)VDEC_CMD_PLAY) ? VDEC_ST_PLAY : VDEC_ST_PLAY_I_FRAME;
                    prVdecEsInfo->ucDropBNs = 0;
                    if(prVdecEsInfo->fgRenderPts)   //for seek case
                    {
                        if(prVdecEsInfoKeep->eDecStatus == VDEC_DEC_TRIGGER)
                        {
                            if((prVdecInfo->arVdecVldInfo[prVdecEsInfo->u4VldId].u4LockCount > 0) &&
                               (prVdecInfo->arVdecVldInfo[prVdecEsInfo->u4VldId].ucLockEsId == ucEsId))
                            {
                                #if VLD_LOCK_LOG
                                u4VLDLogWr(ucEsId, (UCHAR)prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_FLW_BASE + 1);
                                #endif
                                // Unlock VLD to keep lock/unlock in pairs
                                _VDEC_UnlockVld(ucEsId, (UCHAR)prVdecEsInfo->u4VldId);
                            }
                        }
                        prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                    }
                }
                prVdecEsInfo->u4IpFrameNum=0;         //reset I/P frame count
            }
            break;
        case VDEC_CMD_PAUSE:
            if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY)
            {
                prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
                prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_PAUSE_DONE);
            }
            break;
        case VDEC_CMD_AVSYNC:
            _VdecSetSyncStc(ucEsId);
            break;
        case VDEC_CMD_DEBLOCK:
            _MPV_SetDbk(ucEsId);
            break;
        case VDEC_CMD_RR:
            _MPV_SetRR(ucEsId);
            break;
        case VDEC_CMD_CAPTURE:
            _VdecGetThumbnail(ucEsId);
            break;
        case VDEC_CMD_FLUSH:
            _VDEC_FlushEs(ucEsId);
            //clear decoder internal buffer, fbg release
            if(prVdecEsInfoKeep->pfnVdecStop)
            {
                prVdecEsInfoKeep->pfnVdecStop(ucEsId);
            }
            prVdecEsInfo->ucFbgId = 0xFF;
            #ifdef CC_SUPPORT_VDEC_PREPARSE
            if (VDEC_DEC_PRE_PARSER == prVdecEsInfoKeep->eDecStatus || 
                VDEC_DEC_RES_ALLOC == prVdecEsInfoKeep->eDecStatus)
            {
            }
            else
            #endif
            {
                prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
            }
            break;
        case VDEC_CMD_FLUSH_DPB:
              _VdecFlushDPB(ucEsId);
              break;
        case VDEC_CMD_FLUSH_ESMQ:
            if (prVdecEsInfo->fgMMPlayback)
            {
                //clear queue, clear stored info, do notification
                _VDEC_LockFlushMutex(ucEsId);
                if (prVdecEsInfo->fgFlushEsmQ)
                {
                    LOG(1, "ES(%d) VDEC_CMD_FLUSH_ESMQ Flush\n", ucEsId);
                    _VDEC_FlushEs(ucEsId);
                    prVdecEsInfo->fgFlushEsmQ = FALSE;
                    _VDEC_UnlockFlushSema(ucEsId);
                }
                #ifdef CC_SUPPORT_VDEC_PREPARSE
                if (VDEC_DEC_PRE_PARSER == prVdecEsInfoKeep->eDecStatus || 
                    VDEC_DEC_RES_ALLOC == prVdecEsInfoKeep->eDecStatus)
                {
                }
                else
                #endif
                {
                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                }
                _VDEC_UnlockFlushMutex(ucEsId);
            }
            break;
        #ifdef CC_REALD_3D_SUPPORT
        case VDEC_CMD_3D_MODE_CHANGE:
            prVdecEsInfo->u13DUsrCtrlMode = prVdecEsInfo->u13DUsrCtrlModeNtfy;
            prVdecEsInfo->u1IsForceLView = prVdecEsInfo->u1IsForceLViewNtfy;

            if(prVdecEsInfoKeep->eCurState == VDEC_ST_IDLE)
            {
                ucLockFbId = VDP_FrcGetLockFrameBuffer(VDP_Es2Vdp(ucEsId));
                if(ucLockFbId == 0xFF)
                {
                    UCHAR ucVdpId = VDP_Es2Vdp(ucEsId);
                    ucLockFbId = VDP_GetPendingFB(ucVdpId, prVdecEsInfo->ucFbgId);
                }
                if(ucLockFbId == 0xFF)
                {
                    //ASSERT(ucLockFbId != 0xFF);
                    LOG(3, "ucLockFbId == 0xFF\n");
                }
                else
                {
                    _VDEC_Check2D3DSwitch(ucEsId, ucLockFbId);
                }
            }
            break;
        #endif
        default:
            break;
    }

    if ((UINT32)prVdecEsInfoKeep->eCurState != u4PreStatus)
    {
        LOG(2, "Es%d Status Change %x to %x\n", ucEsId, u4PreStatus, prVdecEsInfoKeep->eCurState);
    }
}


UCHAR _VDecReceiveEs(UCHAR ucEsId)
{
    UCHAR ucMpvId;
    UINT16 u2MsgQIdx;
    VDEC_PES_INFO_T* prPesInfo = NULL;
    SIZE_T zMsgSize;
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    PARAM_MPV_T* prMpvCounter;
    UINT32 u4Pts = 0, u4Dts = 0;
    UINT64 u8Pts = 0;
    UCHAR ucSkipMode = IPBMode;
    STC_CLOCK rStcClk;
    VDEC_STATUS_CB_T rStatusCB;
    static UINT32 u4VldReadPtr = 0;
    BOOL fgPrevDtsValid;
#ifdef CC_VDEC_FMT_DETECT
    UINT32 u4Type = 0;
#endif
    UCHAR ucPrevPicType;
    HANDLE_T hMsgQ;
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prMpvCounter = prVdecEsInfo->prMpvCounter;

    if (LAE_DECODE != prVdecEsInfo->ucVDecStatus) //tmp solution
    {
        if (!prVdecEsInfo->ucStarted)
        {
            VERIFY(x_sema_lock(prVdecEsInfoKeep->hUnderFlowSema, X_SEMA_OPTION_WAIT)
            == OSR_OK);
        }
    }

    //get msg from dmx
    if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
    {
        prVdecEsInfoKeep->pfnVdecGetPesStruct(ucEsId, (void**)&prPesInfo);
        if(prPesInfo == NULL)
        {
            return VLD0;
        }
    }


    zMsgSize = sizeof(VDEC_PES_INFO_T);
    VERIFY(x_sema_lock(prVdecEsInfoKeep->hMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

#ifdef VDEC_TIME_PROFILE
    HAL_TIME_T rTimeRstS, rTimeRstE, rTimeRstDt;
    HAL_GetTime(&rTimeRstS);
#endif

    // es case will not have EsId > 0, Sub Q is useless
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if(((prVdecEsInfo->e3DType == VDEC_3D_MVC) ||
        (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
        &&
        #ifdef ENABLE_MULTIMEDIA
        (prVdecEsInfo->eContainerType != SWDMX_FMT_H264_VIDEO_ES) &&
        #endif
        prVdecEsInfo->fgIsDependView)
    {
        hMsgQ = prVdecEsInfoKeep->hMsgQSub;
    }
    else
    #endif
    {
        hMsgQ = prVdecEsInfoKeep->hMsgQ;
    }
    
#ifdef CC_SUPPORT_VDEC_PREPARSE
    if ((prVdecEsInfo->ePreParseStatus == VDEC_PREPS_ST_BEGIN) || 
        (prVdecEsInfo->ePreParseStatus == VDEC_PREPS_ST_FPS_CALC))
    {
        hMsgQ = prVdecEsInfoKeep->hMsgQPreParse;
    }
#endif

    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    fgPrevFbAllocateFail = prVdecEsInfo->fgNoFrameBuffer;
    #endif
    
    if(prVdecEsInfoKeep->eCurFMT != VDEC_FMT_H264 || prVdecEsInfo->fgNoFrameBuffer == FALSE)
    {
        VERIFY(x_msg_q_receive(&u2MsgQIdx, &prVdecEsInfoKeep->rPesInfo, &zMsgSize,
            &(hMsgQ), 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
        
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #if (!defined(CC_VDEC_H264_DISABLE) || !defined(CC_VDEC_H265_DISABLE))
        if(((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264) && IS_H264_PIC_NALU(prVdecEsInfoKeep->rPesInfo.ucPicType))
            #ifndef CC_VDEC_H265_DISABLE
            || ((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265) && IsH265SliceNalUnit(prVdecEsInfoKeep->rPesInfo.ucPicType))
            #endif
          )
        {
            if(prVdecEsInfo->u4FrameNumInEsmQ > 0)
            {
                prVdecEsInfo->u4FrameNumInEsmQ--;
            }
            else
            {
                LOG(2, "[Warning][%d] prVdecEsInfo->u4FrameNumInEsmQ = 0\n",ucEsId);
            }
        }
        #endif
        #endif
    }
    else
    {
       prVdecEsInfo->fgNoFrameBuffer = FALSE;
       LOG(2,"[Warning][%d] Previous es not decoded yet pts(0x%x).\n",ucEsId,prVdecEsInfo->u4PTS);
    }

    #ifdef CC_DUAL_CORE_DEC
    if(((prVdecEsInfo->ucDualCoreMode == SINGLE_CORE_MODE) && (prVdecEsInfo->ucVDecStatus == REAL_DECODE)
        && VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT) && (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_HEADER_PARSE)
        #ifdef ENABLE_MULTIMEDIA
        && (!((VDEC_IS_SEAMLESS(prVdecEsInfo->eMMSrcType)) && (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)))
        #endif
        ) || (prVdecEsInfo->ePreParseStatus == VDEC_PREPS_ST_SKIP_ES))
    {       
        VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeepLae;
        VDEC_ES_INFO_T* prVdecEsInfoLae;
        VDEC_PES_INFO_T rPesInfoLae;
        HANDLE_T hMsgQLae;
        INT32 i4Ret;
        
        prVdecEsInfoKeepLae = _VDEC_GetEsInfoKeep(prVdecEsInfo->ucLaeEsInfoId);
        prVdecEsInfoLae = _VDEC_GetEsInfo(prVdecEsInfo->ucLaeEsInfoId);
        hMsgQLae = prVdecEsInfoKeepLae->hMsgQ;
        
        i4Ret = (x_msg_q_receive(&u2MsgQIdx, &rPesInfoLae, &zMsgSize,
                &(hMsgQLae), 1, X_MSGQ_OPTION_NOWAIT));
        LOG(7,"Get LAE msgQ[0x%x] PicType 0x%x rdptr 0x%x, i4Ret %d\n",hMsgQLae,rPesInfoLae.ucPicType,rPesInfoLae.u4VldReadPtr,i4Ret);
    }
    #endif

#ifdef TIME_MEASUREMENT
    if ((TRUE == prVdecEsInfo->fgPrintReceiveEs) && (0x7 == ((prVdecEsInfoKeep->rPesInfo.ucPicType)&(0x1f)))
        && (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264))
    {
        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "Vdec Receive First ES ");
        prVdecEsInfo->fgPrintReceiveEs = FALSE;
    }
#endif

#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeRstE);
    HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
    LOG(6, "VDec Receieve ES Delta T = (%ld), %d.%6d \n", rTimeRstDt.u4Micros,
        rTimeRstE.u4Seconds, rTimeRstE.u4Micros);
#endif
#ifdef TIME_MEASUREMENT
    if (TRUE == prVdecEsInfoKeep->rPesInfo.fgWFDTMSLogEnable)
    {   
        TMS_DIFF_EX(TMS_FLAG_WFD_LATENCY, "WFD_DATA:", "Vdec Receive ES");  
    }
#endif
	
#ifdef __KERNEL__
    if(prVdecEsInfoKeep->rPesInfo.u8_vid_pts != 0)    
    {        
        UINT64 u8CurTime = 0;        
        UINT64 u8Remainder = 0;        
        struct timeval tv = {0};                

        do_gettimeofday(&tv);                
        u8CurTime = (UINT64)tv.tv_sec * 1000000;                        
        u8CurTime = u8CurTime + (UINT64)tv.tv_usec;        
        u8CurTime = u8Div6432(u8CurTime, 1000, &u8Remainder);                
        LOG(1, "[VDEC][%s]PTS(0x%llx),Time[%llu],CurTime[%llu],Cost[%llu]\n", __FUNCTION__,             
            prVdecEsInfoKeep->rPesInfo.u8_vid_pts, prVdecEsInfoKeep->rPesInfo.u8_push_time, u8CurTime, u8CurTime - prVdecEsInfoKeep->rPesInfo.u8_push_time);    
    }
#endif
    prVdecEsInfo->fgDecodeErr = FALSE;
    prVdecEsInfo->u4DecErrMbCnt = 0;

#ifdef SWDMX_DBG_USB
    do
    {
        static UINT32 u4Addr = 0;
        if (USB_DUMP_ON(DUMP_DATA_DMX_SWDMX_VDEC))
        {
            static UINT32 u4EsCount = 0;
            static CHAR ucFileName[NAME_MAX_LEN] = {0};

            if (0!=u4Addr && u4Addr <= prVdecEsInfoKeep->rPesInfo.u4VldReadPtr)
            {
                x_sprintf(ucFileName, "%s/VES_%010u.raw", FILE_LOCAL, u4EsCount);
                Dbg_SaveBlock(ucFileName, VIRTUAL(u4Addr), prVdecEsInfoKeep->rPesInfo.u4VldReadPtr-u4Addr);
                if (DUMP_LOG)
                {
                    LOG(1, "VEs   KAddr:0x%08x VAddr:0x%08x Size=%u\n", u4Addr, VIRTUAL(u4Addr),
                        prVdecEsInfoKeep->rPesInfo.u4VldReadPtr-u4Addr);
                }
            }
            else if (0 != u4Addr)
            {
                x_sprintf(ucFileName, "%s/VEs_%010u_1.raw", FILE_LOCAL, u4EsCount);
                Dbg_SaveBlock(ucFileName, VIRTUAL(u4Addr), prVdecEsInfoKeep->rPesInfo.u4FifoEnd-u4Addr);
                if (DUMP_LOG)
                {
                    LOG(1, "VES_1 KAddr:0x%08x VAddr:0x%08x Size=%u\n", u4Addr, VIRTUAL(u4Addr),
                        prVdecEsInfoKeep->rPesInfo.u4FifoEnd-u4Addr);
                }

                x_sprintf(ucFileName, "%s/VEs_%010u_2.raw", FILE_LOCAL, u4EsCount);
                Dbg_SaveBlock(ucFileName, VIRTUAL(prVdecEsInfoKeep->rPesInfo.u4FifoStart),
                    prVdecEsInfoKeep->rPesInfo.u4VldReadPtr-prVdecEsInfoKeep->rPesInfo.u4FifoStart);
                if (DUMP_LOG)
                {
                    LOG(1, "VEs_2 KAddr:0x%08x VAddr:0x%08x Size=%u\n", u4Addr, VIRTUAL(prVdecEsInfoKeep->rPesInfo.u4FifoStart),
                        prVdecEsInfoKeep->rPesInfo.u4VldReadPtr-prVdecEsInfoKeep->rPesInfo.u4FifoStart);
                }
            }

            u4EsCount ++;
        }

        u4Addr = prVdecEsInfoKeep->rPesInfo.u4VldReadPtr;
    }while(FALSE);
#endif

    if(prPesInfo)    //h264
    {
        if(!prVdecEsInfoKeep->rPesInfo.fgEos)
        {
            ASSERT((prVdecEsInfoKeep->rPesInfo.u4VldReadPtr <= prVdecEsInfoKeep->rPesInfo.u4FifoEnd) &&
                (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr >= prVdecEsInfoKeep->rPesInfo.u4FifoStart));
        }

        u8Pts = prPesInfo->u8PTS;
        u4Pts = prPesInfo->u4PTS;
        u4Dts = prPesInfo->u4DTS;
        fgPrevDtsValid = prPesInfo->fgDtsValid;
        ucPrevPicType = prPesInfo->ucPicType;

#if defined(CC_USE_DDI)
        if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->rPesInfo.u4PTS==0) && (prVdecEsInfo->fgRenderPicCnt))
        {
            LOG(2, "PTS 0, Dup 0x%x/0x%x\n", u4Pts, u8Pts);
            prVdecEsInfoKeep->rPesInfo.u4PTS = u4Pts;
            prVdecEsInfoKeep->rPesInfo.u8PTS = u8Pts;
            prVdecEsInfoKeep->rPesInfo.u4DTS = u4Dts;
        }
#endif
        x_memcpy(prPesInfo, &prVdecEsInfoKeep->rPesInfo, sizeof(prVdecEsInfoKeep->rPesInfo));

        if(!prVdecEsInfoKeep->rPesInfo.fgDtsValid)
        {
            prPesInfo->u8PTS = u8Pts;
            prPesInfo->u4PTS = u4Pts;
            prPesInfo->u4DTS = u4Dts;
#if (defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)) && (!defined(CC_VDEC_H264_DISABLE))
            if(IS_MVC_DEPEND_PIC_NALU(prPesInfo->ucPicType) && IS_MVC_BASE_PIC_NALU(ucPrevPicType))
            {
                prPesInfo->fgDtsValid = fgPrevDtsValid;

                prVdecEsInfoKeep->rPesInfo.u4PTS = u4Pts;
                prVdecEsInfoKeep->rPesInfo.u8PTS = u8Pts;
                prVdecEsInfoKeep->rPesInfo.u4DTS = u4Dts;
                prVdecEsInfoKeep->rPesInfo.fgDtsValid = fgPrevDtsValid;
                LOG(2, "Dup Base view ptsvalid %d\n", prPesInfo->fgDtsValid);
            }
#else
            UNUSED(ucPrevPicType);
            UNUSED(fgPrevDtsValid);
#endif
        }
        //error check        
        #if (!defined(CC_DUAL_CORE_DEC))
        if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264VER) &&
            ((prPesInfo->ucPicType > 0) && (u4VldReadPtr == prPesInfo->u4VldReadPtr)))
        {
            LOG(6, "Rsv Dup Rptr 0x%x\n", prPesInfo->u4VldReadPtr);
            return VDEC_MAX_VLD;
        }
        #endif
        u4VldReadPtr = prPesInfo->u4VldReadPtr;

        //log receive pesInfo timing
        //if (VDEC_PRINT_RSV_INFO & prVdecEsInfoKeep->u4LOGCtrl)
        {
            if ((STC_GetSrc(prVdecEsInfo->ucStcId, &rStcClk) == STC_VALID) ||
                (prVdecEsInfoKeep->fgVPush))
            {
                VDecPrintf(ucEsId,VDEC_LOG_ESM2VDEC,"rsv%x,%d,%d,%x,%x,%x,%d,%d\n",ucEsId,prPesInfo->u4PTS, prPesInfo->fgDtsValid,rStcClk.u4Base,
                                    prPesInfo->ucPicType, prPesInfo->u4VldReadPtr,prPesInfo->ucMpvId,prPesInfo->fgEos);
                                    
//                LOG(0, "rsv%x,%x,%d,%x,%x,%x,%d,%d\n",ucEsId,prPesInfo->u4PTS, prPesInfo->fgDtsValid,rStcClk.u4Base,
//                                    prPesInfo->ucPicType, prPesInfo->u4VldReadPtr,prPesInfo->ucMpvId,prPesInfo->fgEos);
            }
            else
            {
                VDecPrintf(ucEsId,VDEC_LOG_ESM2VDEC,"rsv%x,%d,%d,%x,%x,%d,%d\n",ucEsId,prPesInfo->u4PTS, prPesInfo->fgDtsValid,prPesInfo->ucPicType, 
                                    prPesInfo->u4VldReadPtr,prPesInfo->ucMpvId,prPesInfo->fgEos);
                                    
//                LOG(0, "rsv%x,%x,%d,%x,%x,%d,%d\n",ucEsId,prPesInfo->u4PTS, prPesInfo->fgDtsValid,prPesInfo->ucPicType, 
//                                    prPesInfo->u4VldReadPtr,prPesInfo->ucMpvId,prPesInfo->fgEos);
            }
        }
        
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        if ((VDEC_FMT_H264VER==prVdecEsInfoKeep->eCurFMT) || 
            (VDEC_FMT_H264==prVdecEsInfoKeep->eCurFMT))
        {
            if(prVdecEsInfoKeep->pfnVdecSetParam)
            {
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)MM_PARAM, (UINT32)(prVdecEsInfo->fgMMPlayback?1:0), 0, 0);
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)MM_PVR_PLAY_MODE, (UINT32)prVdecEsInfo->fgPvrPlayBack, 
                    (UINT32)prVdecEsInfo->u1PvrPlayMode, 0);
            }
            
            if((prVdecEsInfo->fgMMPlayback && prPesInfo->fgEos)
                && (!IS_H264_PIC_NALU(prPesInfo->ucPicType))
                && (!IS_MVC_DEPEND_PIC_NALU(prPesInfo->ucPicType))
                && (!prVdecEsInfoKeep->fgVPushFBGFromInst))
            {
                return VDEC_MAX_VLD;
            }
        }
        #endif
        #endif
    }
    else
    {
        //log receive pesInfo timing
        if (VDEC_PRINT_RSV_INFO & prVdecEsInfoKeep->u4LOGCtrl)
        {
            if (STC_GetSrc(prVdecEsInfo->ucStcId, &rStcClk) == STC_VALID)
            {
                LOG(0, "rsv%d, %X/%llX, %d, %X, %X, %X\n", ucEsId, prVdecEsInfoKeep->rPesInfo.u4PTS, 
                        prVdecEsInfoKeep->rPesInfo.u8PTS,
                        prVdecEsInfoKeep->rPesInfo.fgDtsValid, 
                        rStcClk.u4Base, prVdecEsInfoKeep->rPesInfo.ucPicType, prVdecEsInfoKeep->rPesInfo.u4VldReadPtr);
            }
            else
            {
                LOG(0, "rsv%d, %X/%llX, %d, %X, %X\n", ucEsId, prVdecEsInfoKeep->rPesInfo.u4PTS, 
                        prVdecEsInfoKeep->rPesInfo.u8PTS,
                        prVdecEsInfoKeep->rPesInfo.fgDtsValid,
                        prVdecEsInfoKeep->rPesInfo.ucPicType, prVdecEsInfoKeep->rPesInfo.u4VldReadPtr);
            }
        }
    }
    
    VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
    VERIFY(x_sema_unlock(prVdecEsInfoKeep->hMutex) == OSR_OK);

    ASSERT(zMsgSize == sizeof(VDEC_PES_INFO_T));
    prMpvCounter->u4EsmQCnt = prVdecEsInfo->u2Count;

    #ifdef VDEC_DUMP_VFIFO
       if ((_hVFifoDataDumpThreadState[ucEsId] == 1) && (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr != 0))
       {
         //prVdecEsInfo->u4VFifoRptr = u4VldReadPtr;
           LOG(1,"VDEC Receive ES data PTS:%X,VIRTUAL VldReadPtr is %X\n",prPesInfo->u4PTS,VIRTUAL(u4VldReadPtr));
           VERIFY(x_sema_unlock(_hVFifoDataArrived[ucEsId]) == OSR_OK);
            
           
           VERIFY(x_sema_lock(_hDataDumpDone[ucEsId], X_SEMA_OPTION_WAIT) == OSR_OK);
       }
#endif
    //check if flush FB
    if(prVdecEsInfo->fgFlushFB)
    {
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            _VDEC_H264Flush(ucEsId, FALSE);
        }
        #endif
        #endif
        prVdecEsInfo->fgFlushFB = FALSE;
    }
    //dmx flush Q
    _VDEC_LockFlushMutex(ucEsId);
    if (prVdecEsInfo->fgFlushEsmQ)
    {
        LOG(3, "ES(%d) _VDecReceiveEs Flush\n", ucEsId);
        _VDEC_FlushEs(ucEsId);
        prVdecEsInfo->fgFlushEsmQ = FALSE;
        _VDEC_UnlockFlushSema(ucEsId);
        _VDEC_UnlockFlushMutex(ucEsId);
        prVdecEsInfo->fgIsDependView = FALSE;
        return VDEC_MAX_VLD;
    }
    _VDEC_UnlockFlushMutex(ucEsId);

#ifndef CC_VDEC_RM_SUPPORT
    ucMpvId = prVdecEsInfoKeep->rPesInfo.ucMpvId;
#else
    ucMpvId = prVdecEsInfo->u4VldId;
#endif
#ifdef __MODEL_slt__
    if (fgSltWatchPts)
    {
        if ( (u4SltPts == prVdecEsInfoKeep->rPesInfo.u4PTS) && (VDEC_IsLock(prVdecEsInfoKeep->rPesInfo.ucEsId)))
        {
            fgSltNotSendPic = TRUE;
        }

        if(fgSltNotSendPic)
        {
            return VDEC_MAX_VLD;
        }
    }
#endif

    if(prVdecEsInfoKeep->rPesInfo.fgIBCEos)
    {
        if(prVdecEsInfo->fgCodecChange)
        {
            LOG(0,"Codec change !!!!!!!!!!!!!!!!!\n");
            return VDEC_MAX_VLD;
        }
        else
        {
            LOG(0,"SWDMX should msg codec change before send IBCEos PES !!!!!!!!!\n");
            return VDEC_MAX_VLD;
        }
    }

    #ifdef CC_VDEC_FMT_DETECT
    u4Type = prVdecEsInfoKeep->rPesInfo.u4DmxFrameType;
    #endif

    // Handle Virtual Picture Header for Event Trigger
    if (prVdecEsInfoKeep->rPesInfo.ucMpvId >= VDEC_MAX_VLD)
    {
        return VDEC_MAX_VLD;
    }
    else if(prVdecEsInfoKeep->fgWaitIFrame)
    {
        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            LOG(3, "Notify Status change, wait i frame\n");
            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
              VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_RECEIVE_DATA, 0);
        }
        prVdecEsInfoKeep->fgWaitIFrame = FALSE;
    }

    // set this in VDEC_Play
    //prVdecEsInfo->u4VldId = ucMpvId;

    #ifdef CC_VDEC_FMT_DETECT
    // Handle start code / codec type mismatch
    else if(u4Type > 0)
    {
        if(VDEC_MAX_VLD == _VDEC_DetectVdecFMT(ucEsId, u4Type))
        {
            return VDEC_MAX_VLD;
        }
    }
    #endif
    
#ifndef CC_VDEC_RM_SUPPORT
    ASSERT(ucMpvId < VDEC_MAX_VLD);
#endif
    prVdecEsInfo->u4FifoStart = prVdecEsInfoKeep->rPesInfo.u4FifoStart;
    prVdecEsInfo->u4FifoEnd = prVdecEsInfoKeep->rPesInfo.u4FifoEnd;
    prVdecEsInfo->u4VldReadPtr = prVdecEsInfoKeep->rPesInfo.u4VldReadPtr;
    prVdecEsInfo->u4VldReadPtrEnd = prVdecEsInfoKeep->rPesInfo.u4VldReadPtrEnd;
    prVdecEsInfo->u8PTS = prVdecEsInfoKeep->rPesInfo.u8PTS;
    prVdecEsInfo->u4PTS = prVdecEsInfoKeep->rPesInfo.u4PTS;
    prVdecEsInfo->u4DTS = prVdecEsInfoKeep->rPesInfo.u4DTS;
    prVdecEsInfo->ucPicType = prVdecEsInfoKeep->rPesInfo.ucPicType;
    prVdecEsInfo->fgDtsValid = prVdecEsInfoKeep->rPesInfo.fgDtsValid;
    prVdecEsInfo->fgSeqHdr = prVdecEsInfoKeep->rPesInfo.fgSeqHdr;
    prVdecEsInfo->u8Offset = prVdecEsInfoKeep->rPesInfo.u8Offset;
    
    if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
    {
        #ifdef VDEC_TIME_PROFILE
        if (prVdecEsInfoKeep->pfnVdecIsPic)
        {
            if (prVdecEsInfoKeep->pfnVdecIsPic(ucEsId, &prVdecEsInfoKeep->rPesInfo))
            {
                _VDEC_CalPicSize(ucEsId);
            }
        }
        #endif
        #ifdef CC_MAPLE_CUST_DRV
        //Calculate bitrate
        if (prVdecEsInfo->ucVDecStatus == REAL_DECODE)
        {
            _VDEC_CalBitrate(ucEsId);
        }
        #endif
        if((prVdecEsInfoKeep->rPesInfo.ucPicType & 0x1f) <= 0x5)
        {
            prMpvCounter->u4RetrieveNs++;
        }
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        //check dependent view
        if (VDEC_MAX_VLD == _VDEC_Set3DCtrl(ucEsId))
        {
            return VDEC_MAX_VLD;
        }         
#endif
#endif
#endif
    }
    else
    {
#ifdef VDEC_TIME_PROFILE
        if (prVdecEsInfoKeep->pfnVdecIsPic)
        {
            if (prVdecEsInfoKeep->pfnVdecIsPic(ucEsId, &prVdecEsInfoKeep->rPesInfo))
            {
                _VDEC_CalPicSize(ucEsId);
            }
        }
#endif
#ifdef CC_MAPLE_CUST_DRV
        //Calculate bitrate
        if (prVdecEsInfo->ucVDecStatus == REAL_DECODE)
        {
            _VDEC_CalBitrate(ucEsId);   
        }
#endif
        prMpvCounter->u4RetrieveNs++;
    }

    if(prVdecEsInfo->fgMMPlayback)
    {
        if((prVdecEsInfo->u8OffsetLast > 0) &&// forward
            (prVdecEsInfo->u4MMSpeed <= STC_SPEED_TYPE_FORWARD_1_DOT_5X))
        {
            if(prVdecEsInfo->u8OffsetLast < prVdecEsInfo->u8Offset)
            {
                prVdecEsInfo->u8OffsetLast = prVdecEsInfo->u8Offset;
            }
        }
        else
        {
            prVdecEsInfo->u8OffsetLast = prVdecEsInfo->u8Offset;
        }
    }
    prVdecEsInfo->rExtra = prVdecEsInfoKeep->rPesInfo.rExtra;
    prVdecEsInfo->u8OffsetI = prVdecEsInfoKeep->rPesInfo.u8OffsetI;
    prVdecEsInfo->u4TickNum = prVdecEsInfoKeep->rPesInfo.u4TickNum;
    prVdecEsInfo->u2DecodingOrder = prVdecEsInfoKeep->rPesInfo.u2DecodingOrder;
    prVdecEsInfo->u4TotlaTimeOffset = prVdecEsInfoKeep->rPesInfo.u4TotalTimeOffset;
    prVdecEsInfo->u4Timestap = prVdecEsInfoKeep->rPesInfo.u4Timestap;

    if((prVdecEsInfo->fgMMPlayback) &&
        (prVdecEsInfoKeep->rPesInfo.fgEos) &&
        (prVdecEsInfo->u4FifoStart == 0) &&
        (prVdecEsInfo->u4FifoEnd == 0) &&
        (prVdecEsInfo->u4VldReadPtr == 0))
    {
        // EOS and without any accessable video frame
        LOG(1, "EOS and without any accessable video frame\n");
        _VDEC_DeliverEos(ucEsId);
        return VDEC_MAX_VLD;
    }

    if ((prVdecEsInfoKeep->fgDmxPartialPes) && (prVdecEsInfo->fgChgThrsd)
        && (prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY_I_FRAME))
    {
        if (prVdecEsInfo->u4LastVldRptr == 0)
        {
            prVdecEsInfo->fgSetDmxIntThrsd = TRUE;
            prVdecEsInfo->fgCalDmxIntThrsd = TRUE;
            prVdecEsInfo->u4LastVldRptr = prVdecEsInfo->u4VldReadPtr;
            prVdecEsInfo->u4DmxIntThrsd = 0 ;
        }
        else if (prVdecEsInfo->fgCalDmxIntThrsd)
        {
            if (prVdecEsInfo->u4VldReadPtr > prVdecEsInfo->u4LastVldRptr)
            {
                prVdecEsInfo->u4DmxIntThrsd = prVdecEsInfo->u4VldReadPtr - prVdecEsInfo->u4LastVldRptr;
            }
            else
            {
                prVdecEsInfo->u4DmxIntThrsd = (prVdecEsInfo->u4FifoEnd - prVdecEsInfo->u4FifoStart) -
                                                        (prVdecEsInfo->u4LastVldRptr - prVdecEsInfo->u4VldReadPtr);
            }

            prVdecEsInfo->u4TotalPicSize += prVdecEsInfo->u4DmxIntThrsd;
            prVdecEsInfo->u4PicCnt ++;

            if (prVdecEsInfo->u4PicCnt == 10)
            {
                prVdecEsInfo->fgCalDmxIntThrsd = FALSE;
                prVdecEsInfo->fgSetDmxIntThrsd = TRUE;
                prVdecEsInfo->u4DmxIntThrsd = prVdecEsInfo->u4TotalPicSize /prVdecEsInfo->u4PicCnt;
                prVdecEsInfo->fgChgThrsd = FALSE;
            }
            else
            {
                prVdecEsInfo->u4LastVldRptr = prVdecEsInfo->u4VldReadPtr;
            }
        }

        if ((prVdecEsInfo->fgSetDmxIntThrsd) && (prVdecEsInfo->u4DmxIntThrsd !=0))
        {
            VERIFY( DMX_SetInterruptThreshold(DMX_PID_TYPE_ES_VIDEO , prVdecEsInfo->u4DmxIntThrsd));
            prVdecEsInfo->fgSetDmxIntThrsd = FALSE;
            LOG(6, "DMX_SetInterruptThreshold 0x%x\n", prVdecEsInfo->u4DmxIntThrsd);
        }
    }

    #ifdef __MODEL_slt__
    if ((prVdecEsInfo->u4PTS == prVdecEsInfo->u4StartPts) &&
        (prVdecEsInfo->u4PTS != 0) &&
        (prVdecEsInfo->fgCountPicFinish == FALSE))
    {
        prVdecEsInfo->fgCountPicNs = TRUE;
        LOG(1, "SLT: start counting pictures\n");
    }
    #endif
    
    ucSkipMode = _VDEC_CheckSkipMode(ucEsId);
        
    #ifdef CC_B2R_3D_ERROR_SUPPORT
    if(ucSkipMode != IPBMode)
    {
        if(!FBM_ChkFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_ERROR_HANDLE_DISABLE))
        {
            FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_ERROR_HANDLE_DISABLE);
        }
    }
    #endif

    //underflow callback
    if (prVdecEsInfo->u2Count <= prVdecEsInfo->u2UdfNs)
    {
        if(prVdecInfo->pfnStatusCb)
        {
            rStatusCB.u1EsId = ucEsId;
            rStatusCB.u4Status = (UINT32)VDEC_BUF_UNDERFLOW;
            prVdecInfo->pfnStatusCb(&rStatusCB);
        }
    }
    //skip es
    if (VDEC_MAX_VLD == _VDEC_CheckSkipES(ucEsId, ucSkipMode, ucMpvId))
    {
        return VDEC_MAX_VLD;
    }    
  
    return (ucMpvId);
}



UCHAR _VDEC_HandleOverFlw(UCHAR ucVldId, UCHAR ucEsmQId, HANDLE_T hMutex,
    HANDLE_T hMsgQueue, const void* pvPesInfo)
{
    ASSERT(pvPesInfo!=NULL);
    //temp code
    do
    {
        x_thread_delay(1);
    }
    while (x_msg_q_send(hMsgQueue, pvPesInfo, sizeof(VDEC_PES_INFO_T), 255) != OSR_OK);

    UNUSED(ucVldId);
    UNUSED(ucEsmQId);
    UNUSED(hMutex);
    return 1;
}

INT32 _VDEC_DecCfg(UCHAR ucEsId)
{
    INT32 i4Ret = (INT32)E_VDEC_OK;    
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    LOG(0, "%s(%d), eCurFMT:%d\n", __FUNCTION__,ucEsId, (UINT32)prVdecEsInfoKeep->eCurFMT);

    switch(prVdecEsInfoKeep->eCurFMT)
    {
        case VDEC_FMT_H264VER:
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_H264SWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_H264HdrPrsNalu;//_VDEC_H264HdrPrsPic;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_H264DecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_H264DecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_H264Stop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_H264GetPesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_H264GetDispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_H264SetParam;
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_H264GetParam;
            prVdecEsInfoKeep->pfnVdecUpdRptr = NULL;
            prVdecEsInfoKeep->pfnVdecSetWptr = NULL;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = NULL;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_H264HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
            //init function
            i4Ret = _VDEC_H264Init(ucEsId, prVdecEsInfo->u4VldId);            
            if(prVdecEsInfoKeep->pfnVdecSetParam)
            {
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)WAIT_FB_TIME, VDEC_WAIT_DISP_TIME, 0, 0);
            }
            //set VFIFO initial delay, will modify PTS value of audio and video by dmx
            if(!prVdecEsInfo->fgMMPlayback && !prVdecEsInfo->fgIPTVPlayback)
            {
                VERIFY(STC_SetPtsDrift(prVdecEsInfo->ucStcId, H264_FIFO_CTRL_INIT) == STC_VALID);
            }
        #endif
        #endif
            break;
        case VDEC_FMT_H264:
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_H264SWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_H264HdrPrsNalu;//_VDEC_H264HdrPrsPic;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_H264DecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_H264DecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_H264Stop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_H264GetPesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_H264GetDispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_H264SetParam;
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_H264GetParam;
            prVdecEsInfoKeep->pfnVdecUpdRptr = NULL;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_H264SetDmxWptr;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_H264EsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_H264HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecStartPts = _VDEC_H264StartPTS;
            #if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_H264IsPic;
            #else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
            #endif
            //init function
            i4Ret = _VDEC_H264Init(ucEsId, prVdecEsInfo->u4VldId);
                              
            if(prVdecEsInfoKeep->pfnVdecSetParam)
            {
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)WAIT_FB_TIME, prVdecEsInfo->u4WaitDispTime, 0, 0);
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)AV_SYNC_INFO, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId, 0);
            #ifndef CC_DUAL_CORE_DEC
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId,VDEC_STATUS,REAL_DECODE,0,0);
            #endif
            }
            prVdecEsInfo->fgIsMVCPtsCmp = FALSE;
            prVdecEsInfo->fgNoFrameBuffer = FALSE;
        #endif
        #endif
            break;
        case VDEC_FMT_MPV:
            prVdecEsInfoKeep->pfnVdecSWRst = _MPV_DecReset;
            prVdecEsInfoKeep->pfnVdecDecode = _MPV_Decode;
            prVdecEsInfoKeep->pfnVdecHdrParse = _MPV_VParser;
            prVdecEsInfoKeep->pfnVdecDecStart = _MPV_DecodeProc;
            prVdecEsInfoKeep->pfnVdecDecFin = _MPV_WaitDecodeProc;
            prVdecEsInfoKeep->pfnVdecDecDrop = _MPV_DropPic;
            prVdecEsInfoKeep->pfnVdecStop = _MPV_StopDecES;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = NULL;
            prVdecEsInfoKeep->pfnVdecGetDispPic = NULL;
            prVdecEsInfoKeep->pfnVdecSetParam = NULL;
#ifdef VDEC_TIME_PROFILE
            prVdecEsInfoKeep->pfnVdecGetParam = _MPV_GetParam;
#else
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
#endif

            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = MPV_EsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = _MPV_WaitRandomAP;
            #ifndef MPV_VIRTUAL_PRS
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _MPV_HandleOverFlow;
            prVdecEsInfoKeep->pfnVdecUpdRptr = MPV_UpdateVldRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = MPV_UpdateVldWptr;
            #else
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = NULL;
            prVdecEsInfoKeep->pfnVdecUpdRptr = NULL;
            prVdecEsInfoKeep->pfnVdecSetWptr = NULL;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
            #endif
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = MPV_MpegIsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            MPV_Init();
            break;
        case VDEC_FMT_WMV:
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VC1_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_WMVSWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_WMVHdrPrs;//_VDEC_H264HdrPrsPic;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_WMVDecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_WMVDecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_WMVStop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_WMVGetPesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_WMVGetDispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_WMVSetParam;
            #ifdef VDEC_TIME_PROFILE
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_WMVGetParam;
            #else
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
            #endif
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_WMVEsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_WMVUpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_WMVSetDmxWptr;
            prVdecEsInfoKeep->pfnVdecStartPts = _VDEC_WMVStartPTS;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_WMVIsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            i4Ret = _VDEC_WMVInit(ucEsId);
        #endif
        #endif
            break;
        case VDEC_FMT_RV:
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_RV_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_RVSWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_RVHdrPrs;//_VDEC_H264HdrPrsPic;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_RVDecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_RVDecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_RVStop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_RVGetPesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_RVGetDispPic;
#ifdef VDEC_TIME_PROFILE
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_RVSetParam;
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_RVGetParam;
#else
            prVdecEsInfoKeep->pfnVdecSetParam = NULL;
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
#endif
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_RVEsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_RVUpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_RVSetDmxWptr;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_RvIsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            i4Ret = _VDEC_RVInit(ucEsId);
        #endif
        #endif
            break;
        case VDEC_FMT_MP4:
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_MPEG4_DISABLE
            //prVdecEsInfo->hVdec = _VDEC_Mpeg4GetHdlr();
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_Mpeg4SWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_Mpeg4PrsHdr;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_Mpeg4DecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_Mpeg4DecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = _VDEC_MPEG4DropFrame;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_Mpeg4Stop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_GetMpeg4PesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_GetMpeg4DispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_Mpeg4SetParam;
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_Mpeg4EsFlwCtrl;
#ifdef VDEC_TIME_PROFILE
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_MPEG4GetParam;
#else
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
#endif
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_MPEG4UpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_MPEG4SetDmxWptr;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecStartPts = _VDEC_MPEG4StartPTS;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_MPEG4IsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            i4Ret = _VDEC_Mpeg4Init(ucEsId);
        #endif
        #endif
            break;
        case VDEC_FMT_MJPEG:
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_MJPEG_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = NULL;//_VDEC_MJPEGSWRst;
            prVdecEsInfoKeep->pfnVdecDecode = _VDEC_MJPEGDecode;
            prVdecEsInfoKeep->pfnVdecHdrParse = NULL;
            prVdecEsInfoKeep->pfnVdecDecStart = NULL;
            prVdecEsInfoKeep->pfnVdecDecFin = NULL;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_MJPEGStop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_MJPEGGetPesStruct;
            prVdecEsInfoKeep->pfnVdecGetDispPic = NULL;
            prVdecEsInfoKeep->pfnVdecSetParam = NULL;
#ifdef VDEC_TIME_PROFILE
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_MJPEGGetParam;
#else
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
#endif
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_MJPEGEsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_MJPEGUpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_MJPEGSetDmxWptr;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_MJPEGIsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            i4Ret = _VDEC_MJPEGInit(ucEsId);
        #endif
        #endif
            break;
#ifndef CC_VDEC_AVS_DISABLE
        case VDEC_FMT_AVS:
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_AVSSWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_AVSHdrPrsHdr;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_AVSDecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_AVSDecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = _VDEC_AVSDropFrame;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_AVSStop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_AVSGetPesStruct;
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_AVSGetDispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_AVSSetParam;
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_AVSGetParam;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_AVSEsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_AVSUpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_AVSSetDmxWptr;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_AVSIsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            i4Ret = _VDEC_AVSInit(ucEsId);
            break;
#endif
        case VDEC_FMT_RAW:
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_RAW_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = NULL;
            prVdecEsInfoKeep->pfnVdecDecode = _VDEC_RAWDecode;
            prVdecEsInfoKeep->pfnVdecHdrParse = NULL;
            prVdecEsInfoKeep->pfnVdecDecStart = NULL;
            prVdecEsInfoKeep->pfnVdecDecFin = NULL;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_RAWStop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_RAWGetPesStruct;
            prVdecEsInfoKeep->pfnVdecGetDispPic = NULL;
            prVdecEsInfoKeep->pfnVdecSetParam = NULL;
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_RAWUpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = NULL;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_RAWEsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = _VDEC_HandleOverFlw;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
            //init function
            i4Ret = _VDEC_RAWInit(ucEsId);
        #endif
        #endif
            break;
        case VDEC_FMT_VP6:
        #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_VP6_DISABLE
            //prVdecEsInfo->hVdec = _VDEC_Mpeg4GetHdlr();
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_Vp6SWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_Vp6HdrPrs;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_Vp6DecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_Vp6DecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = _VDEC_Vp6DropFrame;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_Vp6Stop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_Vp6GetPesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_Vp6GetDispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_Vp6SetParam;
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_Vp6GetParam;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_Vp6EsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_Vp6UpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_VP6SetDmxWptr;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = NULL;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_VP6IsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif

            //init function
            i4Ret = _VDEC_Vp6Init(ucEsId);
          #endif
          #endif
            break;

        case VDEC_FMT_VP8:
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP8_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = _VDEC_Vp8SWRst;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_Vp8HdrPrs;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_Vp8DecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_Vp8DecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = _VDEC_Vp8DropFrame;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_Vp8Stop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = _VDEC_Vp8GetPesStruct; // should not need this?
            prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_Vp8GetDispPic;
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_Vp8SetParam;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = _VDEC_Vp8EsFlwCtrl;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecUpdRptr = _VDEC_Vp8UpdDmxRptr;
            prVdecEsInfoKeep->pfnVdecSetWptr = _VDEC_VP8SetDmxWptr;
            prVdecEsInfoKeep->pfnVdecFlush   = _VDEC_Vp8Flush;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = NULL;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_Vp8IsPic;
            #ifdef VDEC_TIME_PROFILE
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_Vp8GetParam;
            #endif
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
            prVdecEsInfoKeep->pfnVdecGetParam = NULL;
#endif
            //init function
            i4Ret = _VDEC_Vp8Init(ucEsId);
  #endif
  #endif
            break;
  
          case VDEC_FMT_VP9:
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP9_DISABLE
            prVdecEsInfoKeep->pfnVdecSWRst = NULL;
            prVdecEsInfoKeep->pfnVdecDecode = NULL;
            prVdecEsInfoKeep->pfnVdecHdrParse = _VDEC_Vp9HdrPrs;
            prVdecEsInfoKeep->pfnVdecDecStart = _VDEC_Vp9DecStart;
            prVdecEsInfoKeep->pfnVdecDecFin = _VDEC_Vp9DecFin;
            prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
            prVdecEsInfoKeep->pfnVdecStop = _VDEC_Vp9Stop;
            prVdecEsInfoKeep->pfnVdecGetPesStruct = NULL;
            prVdecEsInfoKeep->pfnVdecGetDispPic = NULL;
            if(prVdecEsInfoKeep->fgPatternMode && prVdecEsInfo->ucVDecStatus != LAE_DECODE)
            {
                prVdecEsInfoKeep->pfnVdecGetDispPic = _VDEC_Vp9GetDispPic;
            }
            prVdecEsInfoKeep->pfnVdecSetParam = _VDEC_Vp9SetParam;
            prVdecEsInfoKeep->pfnVdecEsFlwCtrl = NULL;
            prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
            prVdecEsInfoKeep->pfnVdecUpdRptr = NULL;
            prVdecEsInfoKeep->pfnVdecSetWptr = NULL;
            //prVdecEsInfoKeep->pfnVdecFlush   = _VDEC_Vp8Flush;
            prVdecEsInfoKeep->pfnVdecHandleOverFlw = NULL;
            prVdecEsInfoKeep->pfnVdecStartPts = NULL;
            prVdecEsInfoKeep->pfnVdecGetParam = _VDEC_Vp9GetParam;
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
            prVdecEsInfoKeep->pfnVdecIsPic = _VDEC_Vp9IsPic;
#else
            prVdecEsInfoKeep->pfnVdecIsPic = NULL;
#endif
            //init function
            i4Ret = _VDEC_Vp9Init(ucEsId);
#endif
#endif
            break;

        case VDEC_FMT_H265:
            #ifndef CC_VDEC_H265_DISABLE
            i4Ret = i4VDecH265DecCfg(ucEsId);
            
            #ifndef CC_DUAL_CORE_DEC
            prVdecEsInfoKeep->pfnVdecSetParam(ucEsId,VDEC_STATUS,REAL_DECODE,0,0);
            #endif
            #endif
            break;
        default:
            return (INT32)E_VDEC_FAIL;
            //break;
    }

    if(prVdecEsInfoKeep->pfnVdecStartPts)
    {
        prVdecEsInfoKeep->pfnVdecStartPts(ucEsId , prVdecEsInfo->fgEnCalPTS, prVdecEsInfo->u4StartPTS, (UINT32)&prVdecEsInfo->rPTSInfo);
    }
    return i4Ret;
}

/************************************************************/
/*VOID _VDEC_SetVdecStatus(UCHAR ucEsId)*/
/*Param: */
/*ucEsId : Instant ID*/
/*return : E_VDEC_OK or E_VDEC_XX*/
/*Function : Set VDEC[ucEsId] decode status*/
/*REAL_DECODE : Single core / Multi core decode*/
/*LAE_DECODE  : Lae decode                     */
/*************************************************************/
VOID _VDEC_SetVdecStatus(UCHAR ucEsId)
{

    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_T *prVdecLaeEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecLaeEsInfo = _VDEC_GetEsInfo(prVdecEsInfo->ucLaeEsInfoId);

    // HEVC need check Tile_Enable flag, if true need lae + single core (M10TILE_DUAL_CORE_MODE) decode mode
    // Add code here marked mtk40343
    // .....
    
    if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
    {
        if((prVdecEsInfo->u4VldId < VDEC_MAX_VLD) && (prVdecEsInfo->u4Vld2Id < VDEC_MAX_VLD))
        {
            #ifdef CC_DUAL_CORE_DEC
            prVdecEsInfo->ucDualCoreMode = DUAL_CORE_MODE;
            LOG(2,"VDEC Mcore enable  .... usage %d %d \n",prVdecEsInfo->u4VldId,prVdecEsInfo->u4Vld2Id);
            #else
            prVdecEsInfo->ucDualCoreMode = SINGLE_CORE_MODE;
            #endif
        }
        else
        {
            prVdecEsInfo->ucDualCoreMode = SINGLE_CORE_MODE;
        }
        
        LOG(2,"vdec %d fmt %d mcore %d status %d..%s @ line %d\n",ucEsId,prVdecEsInfoKeep->eCurFMT,prVdecEsInfo->ucDualCoreMode,prVdecEsInfo->ucVDecStatus,__FUNCTION__,__LINE__);
        #ifdef CC_DUAL_CORE_DEC
        if(prVdecEsInfo->ucDualCoreMode)
        {
            prVdecEsInfo->ucVDecStatus = REAL_DECODE;
            prVdecLaeEsInfo->ucVDecStatus = LAE_DECODE;
            if(prVdecEsInfo->fgEnableUFO)
            {
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId,VDEC_UFO_MODE,1,0,0);
            }
            //Send "Play" CMD LAE 
            SET_DECFLAG(prVdecLaeEsInfo, DEC_FLG_WAIT_NEXT_I);
            #ifdef ENABLE_MULTIMEDIA
            prVdecLaeEsInfo->eMMSrcType = prVdecEsInfo->eMMSrcType;
            if ( VDEC_IS_SEAMLESS(prVdecEsInfo->eMMSrcType) && 
                 (prVdecEsInfo->u2OrgHSize < VDEC_H265_SEAMLESS_WIDTH_THD) &&
                 (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
               )
            {
                prVdecEsInfo->ucDualCoreMode = SINGLE_CORE_MODE;
                SET_DECFLAG(prVdecEsInfo,DEC_FLAG_FORCE_SCORE);
                SET_DECFLAG(prVdecLaeEsInfo,DEC_FLAG_FORCE_SCORE);
            }
            #endif
            VDEC_PlayMM(prVdecEsInfo->ucLaeEsInfoId,prVdecEsInfo->fgMMPlayback);
            VDEC_Play(prVdecEsInfo->ucLaeEsInfoId,prVdecEsInfoKeep->eCurFMT);
            //prVdecLaeEsInfo->u4VldId = VDEC_LAE0;
            #ifdef CC_VDEC_RM_SUPPORT
            UNUSED(VDEC_VLDAllocLAE(prVdecEsInfo->ucLaeEsInfoId));
            #endif
        }
        else
        #endif
        {
            if(prVdecEsInfo->fgEnableUFO)
            {
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId,VDEC_UFO_MODE,1,0,0);
            }
            prVdecEsInfo->ucVDecStatus = REAL_DECODE;
        }
    }
    else
    {
        prVdecEsInfo->ucVDecStatus = REAL_DECODE;
    }
    
}


#ifdef CC_DUAL_CORE_DEC
VOID _VDEC_StopLae(UCHAR ucEsId)
{
    UINT16 u2LaeInputNum = 0,u2LaeEsNum = 0;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_LAE_DecPrm rLaeDecPrm;
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hMsgQLaeInput, &u2LaeInputNum) == OSR_OK);
    VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hMsgQ, &u2LaeEsNum) == OSR_OK);
    LOG(2,"Stop lae[%d] %s @ line %d \n",ucEsId,__FUNCTION__,__LINE__);
    if(u2LaeInputNum == 0)
    {
        //send one LAE to in msgq
        rLaeDecPrm.u4CabacAddr = 0;
        rLaeDecPrm.u4ECAddr = 0;
        vPutBufferToLae(ucEsId,&rLaeDecPrm);
        LOG(2, "Stop lae[%d] vPutBufferToLae\n", ucEsId);
    }
    else if(u2LaeEsNum == 0)
    {
        //send one es to ES msgq
        LOG(2,"Mustbe send one ESMsgQ \n");
    }
}
#endif

#ifdef VDEC_REFINE_ISR_CONDITIONAL_BRANCH
extern VDEC_INFO_T _rVdecInfo;
#endif
static void _VDEC_VldIsr(UINT16 u2Vector)
{
#ifdef VDEC_REFINE_ISR_CONDITIONAL_BRANCH
    ASSERT(u2Vector == VECTOR_VDEC);
    ASSERT(_rVdecInfo.arVdecVldInfo[VLD0].pfnVdecIsr);
    _rVdecInfo.arVdecVldInfo[VLD0].pfnVdecIsr(u2Vector);
    // main propose for this definition is to remove following conditional branch
#else
    VDEC_INFO_T *prVdecInfo;
    UCHAR ucVldId = VLD0;

    if(VECTOR_VDEC == u2Vector)
    {
        ucVldId = VLD0;
    }
#ifdef CC_DUAL_CORE_DEC
    else if(VECTOR_VDEC_CORE1 == u2Vector)
    {
        ucVldId = VLD_Core1;
    }
#if 0//TBD
    else if(VECTOR_VDEC_IMG == u2Vector)
    {
        ucVldId = VLD_IMG;
    }
#endif
    else if(VECTOR_VDEC_LAE0 == u2Vector)
    {
        ucVldId = VLD_LAE0;
    }
    else if(VECTOR_VDEC_LAE1 == u2Vector)
    {
        ucVldId = VLD_LAE1;
    }
#endif
    else
    {
        // may changed later
        ucVldId = VLD0;
    }
    if(ucVldId >= VDEC_MAX_VLD)
    {
        LOG(3, "%s(%d) ucVldId err\n", __FUNCTION__, __LINE__);
        return;
    }
    prVdecInfo = _VDEC_GetInfo();
    if(!prVdecInfo)
    {
        LOG(3, "%s(%d) prVdecInfo err\n", __FUNCTION__, __LINE__);
        return;
    }
    if(prVdecInfo->arVdecVldInfo[ucVldId].pfnVdecIsr)
    {
        prVdecInfo->arVdecVldInfo[ucVldId].pfnVdecIsr(u2Vector);
    }
#endif
}

VOID _VDEC_IsrInit(VOID)
{
    x_os_isr_fct pfnOldIsr;
    _VDEC_RegIsr(VECTOR_VDEC, _VDEC_FakeVldIsr);
    if (x_reg_isr(VECTOR_VDEC, _VDEC_VldIsr, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
    
#ifdef CC_DUAL_CORE_DEC
    _VDEC_RegIsr(VECTOR_VDEC_CORE1, _VDEC_FakeVldIsr);
    if (x_reg_isr(VECTOR_VDEC_CORE1, _VDEC_VldIsr, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
    _VDEC_RegIsr(VECTOR_VDEC_LAE0, _VDEC_FakeVldIsr);
    if (x_reg_isr(VECTOR_VDEC_LAE0, _VDEC_VldIsr, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
    _VDEC_RegIsr(VECTOR_VDEC_LAE1, _VDEC_FakeVldIsr);  
    if (x_reg_isr(VECTOR_VDEC_LAE1, _VDEC_VldIsr, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    } 
#endif    
}

VOID _VDEC_RegIsr(UINT16 u2Vector, PFN_VDEC_ISR pfnIsr)
{
    VDEC_INFO_T *prVdecInfo;
    UCHAR ucVldId = VLD0;

    if(VECTOR_VDEC == u2Vector)
    {
        ucVldId = VLD0;
    }
#ifdef CC_DUAL_CORE_DEC
    else if(VECTOR_VDEC_CORE1 == u2Vector)
    {
        ucVldId = VLD_Core1;
    }
    else if(VECTOR_VDEC_LAE0 == u2Vector)
    {
        ucVldId = VLD_LAE0;
    }
    else if(VECTOR_VDEC_LAE1 == u2Vector)
    {
        ucVldId = VLD_LAE1;
    }
#endif
    else
    {
        // may changed later
        ucVldId = VLD0;
    }
    if(ucVldId >= VDEC_MAX_VLD)
    {
        LOG(3, "%s(%d) ucVldId err\n", __FUNCTION__, __LINE__);
        return;
    }
    prVdecInfo = _VDEC_GetInfo();
    if(!prVdecInfo)
    {
        LOG(3, "%s(%d) prVdecInfo err\n", __FUNCTION__, __LINE__);
        return;
    }
#ifdef VDEC_REFINE_ISR_CONDITIONAL_BRANCH
    if(!pfnIsr)
    {
        LOG(0, "%s(%d) pfnIsr null\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return;
    }
#endif
    prVdecInfo->arVdecVldInfo[ucVldId].pfnVdecIsr = pfnIsr;
}


VOID _VDEC_SetIsr(UCHAR ucEsId, UCHAR ucVldId)
{
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    UNUSED(ucVldId);

    switch(prVdecEsInfoKeep->eCurFMT)
    {
        case VDEC_FMT_MPV:
            _MPV_SetIsr(ucEsId, ucVldId);
            break;
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_MPEG4_DISABLE
        case VDEC_FMT_MP4:
            _VDEC_MPEG4SetIsr(ucEsId);
            break;
#endif
#endif
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
        case VDEC_FMT_H264:
        case VDEC_FMT_H264VER:
            _VDEC_H264SetIsr(ucEsId, ucVldId);
            break;
#endif
#endif
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VC1_DISABLE
        case VDEC_FMT_WMV:
            _VDEC_WMVSetIsr(ucEsId);
            break;
#endif
#endif
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_RV_DISABLE
        case VDEC_FMT_RV:
            _VDEC_RVSetIsr(ucEsId);
            break;
#endif
#endif
        case VDEC_FMT_MJPEG:
            break;
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP6_DISABLE
        case VDEC_FMT_VP6:
          _VDEC_Vp6SetIsr(ucEsId);
          break;
#endif
#endif

#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP8_DISABLE
        case VDEC_FMT_VP8:
          _VDEC_Vp8SetIsr(ucEsId);
          break;
#endif
#endif

#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP9_DISABLE
        case VDEC_FMT_VP9:
          _VDEC_Vp9SetIsr(ucEsId, ucVldId);
          break;
#endif
#endif

#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_AVS_DISABLE
        case VDEC_FMT_AVS:
            _VDEC_AVSSetIsr(ucEsId);
            break;
#endif
#endif
        
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_H265_DISABLE
        case VDEC_FMT_H265:
            vVDECH265SetIsr(ucEsId, ucVldId);
            break;
#endif
#endif
        default:
            LOG(3, "format unknown %d\n", (UINT32)prVdecEsInfoKeep->eCurFMT);
            break;
    }
}

BOOL VDEC_IsNotDisplay(UCHAR ucEsId, UINT32 u4PTS, INT32 i4TempRef,
        UINT64 u8Offset)
{
    BOOL fgNotDisplay = FALSE;
    #if 0
    UCHAR ucFbId;
    #endif
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    ASSERT(ucEsId < MPV_MAX_ES);


    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(!prVdecEsInfo)
    {
        return TRUE;
    }

    if (!prVdecEsInfo->fgMMPlayback)
    {
        // do nothing for DTV
        return FALSE;
    }
    
    if(prVdecEsInfo->fgRVUSkip)
    {
        //Force output when rvuskip is true
        return FALSE;
    }
    else if(prVdecEsInfo->fgMMPlayback && prVdecEsInfo->fgSendedEos)
    {
        return TRUE;
    }

    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        // when this flag is set, we still keep one frame in decoder
        // and VDEC_CheckStopStatus is called because of this keepped frame.
        // we should not check status while putting this frame to display queue.
        return TRUE;
    }

    if ((prVdecEsInfo->fgRenderFromFirstPic) &&
        (((prVdecEsInfo->u8Offset >= prVdecEsInfo->u8RenderFromPos) &&
          ((prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_1X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_2X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_3X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_4X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_8X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_16X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_32X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_64X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_128X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_256X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_512X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_REWIND_1024X)))
         ||
         ((prVdecEsInfo->u8RenderFromPos != prVdecEsInfo->u8FileSize) &&
          (u8Offset <= prVdecEsInfo->u8RenderFromPos) &&
          ((prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_3X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_4X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_8X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_16X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_32X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_64X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_128X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_256X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_512X) ||
           (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_1024X)))
        )
       )
    {
        fgNotDisplay = TRUE;
    }
    else if ((prVdecEsInfo->fgRenderPts) &&
        (u4PTS < prVdecEsInfo->u4RenderPts))
    {
        fgNotDisplay = TRUE;
    }
    //for TS trick to normal, 1st frame should duplicate with current display
    //avoid b2r display 1st frame withou AV sync judgement
    else if ((prVdecEsInfo->fgRenderFromPos) &&
        ((i4TempRef != prVdecEsInfo->i4RenderTempRef) ||
         (u8Offset < prVdecEsInfo->u8RenderFromPos)))
    //else if (prVdecEsInfo->fgRenderFromPos)
    {
        fgNotDisplay = TRUE;
    }
    else if (prVdecEsInfo->fgRenderBackStep)
    {
        // Even this frame is target frame, we don't want to display it.
        fgNotDisplay = TRUE;
        if((prVdecEsInfo->u8Offset >= prVdecEsInfo->u8RenderFromPos) &&
            (i4TempRef == prVdecEsInfo->i4RenderTempRef))
        {
            if(prVdecEsInfo->u2Disp2EmptyCnt > 0)
            {
                #if 0
                // That means we have previous display frame to empty queue.
                ucFbId = FBM_GetLatestEmptyFrameBuffer(prVdecEsInfo->ucFbgId,
                                        prVdecEsInfo->u4WaitDispTime);
                if(ucFbId != FBM_FB_ID_UNKNOWN)
                {
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_DISPLAYQ);
                    LOG(5, "VDEC_IsNotDisplay fgRenderBackStep ucFbId %d Org i4TempRef %d u8Offset 0x%llx u8RenderFromPos 0x%llx\n",
                        ucFbId, i4TempRef, prVdecEsInfo->u8Offset, prVdecEsInfo->u8RenderFromPos);
                }
                else
                {
                    LOG(3, "VDEC_IsNotDisplay FBM_GetLatestEmptyFrameBuffer FBM_FB_ID_UNKNOWN\n");
                }
                #endif
            }
            else
            {
                LOG(5, "VDEC_IsNotDisplay fgRenderBackStep have no previous frame\n");
            }
        }
    }
    else if ((prVdecEsInfo->fgRenderFromPts) &&
        (u4PTS < prVdecEsInfo->u4RenderFromPts))
    {
        fgNotDisplay = TRUE;
    }
    else
    {
#ifdef ENABLE_MULTIMEDIA
        if(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
        {
            if(!prVdecEsInfo->fgSetStartPts)
            {
                prVdecEsInfo->fgSetStartPts = TRUE;
                if (STC_SetStartPts(AV_SYNC_TARGET_VDO, prVdecEsInfo->ucStcId, u4PTS) != STC_VALID)
                {
                    LOG(3, "STC_SetStartPts Invalid\n");
                }
                LOG(6, "STC_SetStartPts 0x%x\n", u4PTS);
                STC_StartStc(prVdecEsInfo->ucStcId);
            }
        }
        else
#endif
        {
            if((!prVdecEsInfo->fgSetStartPts)&&(prVdecEsInfoKeep->fgVPush==FALSE || prVdecEsInfoKeep->fgVPushDecodeOnly==TRUE))
            {
                if(u4PTS)
                {
                    LOG(6, "VDEC_IsNotDisplay fgSetStartPts, pts 0x%x\n",
                        (u4PTS - 3000));
                    //VDP_TriggerAudReceive(u4PTS);
                    STC_StopStc(prVdecEsInfo->ucStcId);
                    STC_SetStcValue(prVdecEsInfo->ucStcId, (UINT32)(u4PTS - 3000));
                    STC_StartStc(prVdecEsInfo->ucStcId);
                }
                else
                {
                    LOG(3, "VDEC_IsNotDisplay fgSetStartPts no pts or zero\n");
                    //VDP_TriggerAudReceive(0);
                    //STC_SetStcValue((UINT32)(0 - 3000));
                    STC_StartStc(prVdecEsInfo->ucStcId);
                }
                prVdecEsInfo->fgSetStartPts = TRUE;
            }
        }
    }

    if(fgNotDisplay)
    {
        prVdecEsInfo->u2Disp2EmptyCnt++;
    }
    else
    {
        if(prVdecEsInfo->u4FirstDispPTS == 0)
        {
            prVdecEsInfo->u4FirstDispPTS = u4PTS;
        }
    }
    return fgNotDisplay;
}

BOOL VDEC_CheckStopStatus(UCHAR ucEsId, UINT32 u4PTS, INT32 i4TempRef)
{
    BOOL fgSkip = FALSE;
    #ifdef CC_B2R_3D_ERROR_SUPPORT
    BOOL fgRenderFinish = FALSE;
    #endif
    UCHAR ucFbId;
    //UCHAR ucFbIdSub = 0xFF;
    //UINT32 u4Temp;

    //FBM_PIC_HDR_T *prFbmPicHdr = NULL;
    //FBM_PIC_HDR_T *prFbmPicHdrSub = NULL;
    //FBM_SEQ_HDR_T *prFbmSeqHdr;
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    ASSERT(ucEsId < MPV_MAX_ES);

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((!prVdecEsInfo) || (!prVdecInfo))
    {
        return FALSE;
    }

    if(!prVdecEsInfo->fgMMPlayback)
    {
        // do nothing for DTV
        return FALSE;
    }

    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        // when this flag is set, we still keep one frame in decoder
        // and VDEC_CheckStopStatus is called because of this keepped frame.
        // we should not check status while putting this frame to display queue.
        return FALSE;
    }


   if (prVdecEsInfo->fgRenderFromFirstPic)
    {
        prVdecEsInfo->fgRenderFromFirstPic = FALSE;
        #ifdef CC_B2R_3D_ERROR_SUPPORT
        fgRenderFinish = TRUE;
        #endif
        if(prVdecEsInfoKeep->pfnRenderPtsCb)
        {
            prVdecEsInfoKeep->pfnRenderPtsCb(u4PTS, prVdecEsInfoKeep->u1AttachedSrcId);
            LOG(3, "VDEC_CheckStopStatus pfnRenderPtsCb fgRenderFromFirstPic\n");
        }
        if (prVdecEsInfo->fgPlayOneFrm)
        {
            prVdecEsInfo->fgPlayOneFrm = FALSE;
            if(prVdecEsInfoKeep->pfnTrickPtsCb)
            {
                // We didn't found any frame before u8RenderFromPos.
                if(prVdecEsInfo->u2Disp2EmptyCnt > 0)
                {
                    // Send again with far position
                    prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, TRUE);
                    LOG(6, "VDEC_CheckStopStatus pfnTrickPtsCb fgRenderFromFirstPic again\n");
                }
                else
                {
                    prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, FALSE);
                    LOG(6, "VDEC_CheckStopStatus pfnTrickPtsCb fgRenderFromFirstPic finish\n");
                }
                //x_thread_delay(33);
            }
            VDEC_Pause(ucEsId);
            // switch to cmd queue (send cmd)
            x_thread_delay(1);
            fgSkip = TRUE;
        }
    }
    else if ((prVdecEsInfo->fgRenderPts) &&
        (u4PTS >= prVdecEsInfo->u4RenderPts))
    {
        prVdecEsInfo->fgRenderPts = FALSE;
        #ifdef CC_B2R_3D_ERROR_SUPPORT
        fgRenderFinish = TRUE;
        #endif
        prVdecEsInfo->u4RenderPts = 0;
        if(prVdecEsInfoKeep->pfnRenderPtsCb)
        {
            prVdecEsInfoKeep->pfnRenderPtsCb(u4PTS, prVdecEsInfoKeep->u1AttachedSrcId);
            LOG(3, "VDEC_CheckStopStatus pfnRenderPtsCb fgRenderPts\n");
        }
        if (prVdecEsInfo->fgPlayOneFrm)
        {
            prVdecEsInfo->fgPlayOneFrm = FALSE;
            if(prVdecEsInfoKeep->pfnTrickPtsCb)
            {
                prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, FALSE);
                //x_thread_delay(33);
                LOG(3, "VDEC_CheckStopStatus pfnTrickPtsCb fgRenderPts\n");
            }
            VDEC_Pause(ucEsId);
            // switch to cmd queue (send cmd)
            x_thread_delay(1);
            fgSkip = TRUE;
        }
    }
    else if ((prVdecEsInfo->fgRenderFromPos) &&
        ((prVdecEsInfo->u8Offset > prVdecEsInfo->u8RenderFromPos) ||
        ((prVdecEsInfo->u8Offset >= prVdecEsInfo->u8RenderFromPos) &&
        ((i4TempRef >= prVdecEsInfo->i4RenderTempRef) ||
         (prVdecEsInfo->u4MMSpeed != (UINT32)STC_SPEED_TYPE_FORWARD_1X)
#ifdef ENABLE_MULTIMEDIA
         || (((prVdecEsInfo->eContainerType >= SWDMX_FMT_MPEG1_DAT) &&
         (prVdecEsInfo->eContainerType <= SWDMX_FMT_AVS_VIDEO_ES))&&
         (i4TempRef == 0))
#endif
         ))))
    {
        prVdecEsInfo->fgRenderFromPos = FALSE;
        #ifdef CC_B2R_3D_ERROR_SUPPORT
        fgRenderFinish = TRUE;
        #endif
        prVdecEsInfo->u8RenderFromPos = 0;
        prVdecEsInfo->i4RenderTempRef = 0;
        prVdecEsInfo->u2RenderDecodingOrder = 0;

        if(prVdecEsInfoKeep->pfnRenderPtsCb)
        {
            prVdecEsInfoKeep->pfnRenderPtsCb(u4PTS, prVdecEsInfoKeep->u1AttachedSrcId);
            LOG(3, "VDEC_CheckStopStatus fgRenderFromPos pfnRenderPtsCb u4PTS %d\n", u4PTS);
        }

        LOG(3, "VDEC_CheckStopStatus fgRenderFromPos i4TempRef %d\n", i4TempRef);
    }
    else if ((prVdecEsInfo->fgRenderBackStep) &&
        (prVdecEsInfo->u8Offset >= prVdecEsInfo->u8RenderFromPos) &&
        ((i4TempRef == prVdecEsInfo->i4RenderTempRef) ||
         (prVdecEsInfo->u4MMSpeed != (UINT32)STC_SPEED_TYPE_FORWARD_1X)))
    {
        prVdecEsInfo->fgRenderBackStep = FALSE;
        #ifdef CC_B2R_3D_ERROR_SUPPORT
        fgRenderFinish = TRUE;
        #endif
        prVdecEsInfo->u8RenderFromPos = 0;
        prVdecEsInfo->i4RenderTempRef = 0;
        prVdecEsInfo->u2RenderDecodingOrder = 0;

        // if only decode one frame, we have to seek again (far)
        // notify swdmx to do it again
        // FixMe

        if(prVdecEsInfoKeep->pfnTrickPtsCb)
        {
            // We have put two to empty that originaly should put to display.
            // That means we have previous display frame to empty queue.
            if(prVdecEsInfo->u2Disp2EmptyCnt > 1)
            {
                prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, FALSE);
                LOG(6, "VDEC_CheckStopStatus pfnTrickPtsCb fgRenderBackStep finish\n");
            }
            else
            {
                // Send again with far position
                prVdecEsInfoKeep->pfnTrickPtsCb(prVdecEsInfo->u1SwdmxSrcId, 0, TRUE);
                LOG(6, "VDEC_CheckStopStatus pfnTrickPtsCb fgRenderBackStep again\n");
            }
            //x_thread_delay(33);
        }
        if(prVdecEsInfoKeep->pfnRenderPtsCb)
        {
            prVdecEsInfoKeep->pfnRenderPtsCb(u4PTS, prVdecEsInfoKeep->u1AttachedSrcId);
            LOG(6, "VDEC_CheckStopStatus pfnRenderPtsCb fgRenderBackStep\n");
        }
        VDEC_Pause(ucEsId);
        // switch to cmd queue (send cmd)
        x_thread_delay(1);

        LOG(3, "VDEC_CheckStopStatus fgRenderBackStep i4TempRef %d\n", i4TempRef);
    }
    else if ((prVdecEsInfo->fgRenderFromPts) &&
        (u4PTS >= prVdecEsInfo->u4RenderFromPts))
    {
        // Pull stc back to 0.5 sec before rendering.
        //STC_StopStc();
        //STC_SetStcValue(prVdecEsInfo->u4RenderFromPts - 45000);
        //STC_StartStc();

        prVdecEsInfo->fgRenderFromPts = FALSE;
        #ifdef CC_B2R_3D_ERROR_SUPPORT
        fgRenderFinish = TRUE;
        #endif
        prVdecEsInfo->u4RenderFromPts = 0;

        if(prVdecEsInfoKeep->pfnRenderPtsCb)
        {
            prVdecEsInfoKeep->pfnRenderPtsCb(u4PTS, prVdecEsInfoKeep->u1AttachedSrcId);
            LOG(3, "VDEC_CheckStopStatus fgRenderFromPts pfnRenderPtsCb u4PTS %d\n", u4PTS);
        }

        if (prVdecEsInfo->fgAutoPause)
        {
            prVdecEsInfo->fgAutoPause = FALSE;
            VDEC_Pause(ucEsId);
        }
    }
    else if ((prVdecEsInfo->fgRenderPicCnt) &&
        (prVdecEsInfo->u4DisplayQPicCnt >= prVdecEsInfo->u4RenderPicCnt) &&
        (!prVdecEsInfo->fgSendedEos))
    {
        UCHAR ucFbStatus;
        prVdecEsInfo->fgRenderPicCnt = FALSE;
        #ifdef CC_B2R_3D_ERROR_SUPPORT
        fgRenderFinish = TRUE;
        #endif
        prVdecEsInfo->u4RenderPicCnt = 0;

        prVdecEsInfo->fgSendedEos = TRUE;

        //Deliver EOS
        // error handle for not finished frame. Ex. the last one is field.
        ucFbId = FBM_GetDecFrameBuffer(prVdecEsInfo->ucFbgId);
        ucFbStatus = FBM_GetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId);
        if ((ucFbStatus == FBM_FB_STATUS_DECODE) || (ucFbStatus == FBM_FB_STATUS_FIELD_READY) ||
            (ucFbStatus == FBM_FB_STATUS_READY))
        {
            FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
        }
        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
            VDEC_COND_THUMBNAIL_PLAY_EOS, 1, 0);
        }
        VDEC_Pause(ucEsId);
        // switch to cmd queue (send cmd)
        x_thread_delay(1);
        //fgSkip = TRUE;
        LOG(3, "VDEC_CheckStopStatus(%d) DisplayCnt & RenderPicCnt %d,%d\n", ucEsId,prVdecEsInfo->u4DisplayQPicCnt,prVdecEsInfo->u4RenderPicCnt);
    }
    else
    {
        //LOG(7, "VDEC_CheckStopStatus do nothing\n");
    }

    #ifdef CC_B2R_3D_ERROR_SUPPORT
    if(fgRenderFinish)
    {
        if(FBM_ChkFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_ERROR_HANDLE_DISABLE))
        {
            FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_3D_ERROR_HANDLE_DISABLE);
        }
    }
    #endif

    return fgSkip;
}


BOOL VDEC_PrsSeqHdr(UCHAR ucEsId, ENUM_VDEC_FMT_T eFmt,
    VDEC_PES_INFO_T* prPesInfo, VDEC_SEQUENCE_DATA_T* prSeqInfo)
{
    //VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    VDEC_PES_INFO_T* prPes;
    UCHAR ucMpvId = VLD0;
    BOOL fgHdrParseOk = FALSE;
    #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
    #ifndef CC_VDEC_H264_DISABLE
    UINT32 u4Dummy;
    UINT32 u4W, u4H;
    #endif
    #endif
    //BOOL fgPowerOn;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((prPesInfo == NULL) || (prSeqInfo == NULL) || (prVdecEsInfo == NULL))
    {
        LOG(1, "PrsSeqHdr input arg error\n");
        return FALSE;
    }

    #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
    #ifndef CC_VDEC_H264_DISABLE
    if(eFmt == VDEC_FMT_H264)
    {
        if((prPesInfo->ucPicType & 0xF) != SPS_NALU)
        {
            LOG(1, "PrsSeqHdr pes type %d error\n", prPesInfo->ucPicType);
            return FALSE;
        }
    }
    #endif
    #endif


    #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
    #if (!defined(CC_TRUSTZONE_SUPPORT) || !defined(CC_SVP_SUPPORT))
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prPesInfo->u4FifoStart), (prPesInfo->u4FifoEnd - prPesInfo->u4FifoStart));
    #endif
    #else
    HalFlushInvalidateDCache();
    #endif
    // video es only.
    prVdecEsInfo->u4VldId = prPesInfo->ucMpvId;
    prVdecEsInfo->u4FifoStart = prPesInfo->u4FifoStart;
    prVdecEsInfo->u4FifoEnd = prPesInfo->u4FifoEnd;
    prVdecEsInfo->u4VldReadPtr = prPesInfo->u4VldReadPtr;
    prVdecEsInfo->u8PTS = prPesInfo->u8PTS;
    prVdecEsInfo->u4PTS = prPesInfo->u4PTS;
    prVdecEsInfo->u4DTS = prPesInfo->u4DTS;
    prVdecEsInfo->ucPicType = prPesInfo->ucPicType;
    prVdecEsInfo->fgDtsValid = prPesInfo->fgDtsValid;
    prVdecEsInfo->fgSeqHdr = prPesInfo->fgSeqHdr;
    prVdecEsInfo->rExtra = prPesInfo->rExtra;
    prVdecEsInfo->u8Offset = prPesInfo->u8Offset;
    prVdecEsInfo->u8OffsetLast = prPesInfo->u8Offset;
    prVdecEsInfo->u8OffsetI = prPesInfo->u8OffsetI;
    prVdecEsInfo->u2DecodingOrder = prPesInfo->u2DecodingOrder;
    prVdecEsInfo->u4TotlaTimeOffset = prPesInfo->u4TotalTimeOffset;

    // lock vld
    _VDEC_LockVld(ucEsId, prPesInfo->ucMpvId);
#if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, prPesInfo->ucMpvId, VLD_LOCK_LOG_FLW_BASE+1);
#endif

    //get pointer from internal decoder struct, or save in common part
    if(prVdecEsInfoKeep->pfnVdecGetPesStruct)
    {
        prVdecEsInfoKeep->pfnVdecGetPesStruct(ucEsId, (void**)&prPes);
        if(prPes == NULL)
        {
            #if VLD_LOCK_LOG
            u4VLDLogWr(ucEsId, prPesInfo->ucMpvId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_FLW_BASE + 2);
            #endif
            _VDEC_UnlockVld(ucEsId, prPesInfo->ucMpvId);
            return FALSE;
        }
        x_memcpy(prPes, prPesInfo, sizeof(VDEC_PES_INFO_T));

        if(eFmt == VDEC_FMT_WMV)
        {
            if(prVdecEsInfoKeep->pfnVdecEsFlwCtrl)
            {
                prVdecEsInfoKeep->pfnVdecEsFlwCtrl(ucEsId, &ucMpvId,0, IPBMode);
            }
        }

        if(prVdecEsInfoKeep->pfnVdecHdrParse)
        {
            fgHdrParseOk = prVdecEsInfoKeep->pfnVdecHdrParse(ucMpvId, ucEsId);

            if((fgHdrParseOk == E_VDEC_FAIL)&&(eFmt == VDEC_FMT_MP4))
            {
                #if VLD_LOCK_LOG
                u4VLDLogWr(ucEsId, prPesInfo->ucMpvId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_FLW_BASE + 3);
                #endif
                _VDEC_UnlockVld(ucEsId, prPesInfo->ucMpvId);
            }
        }
        prSeqInfo->u2_width = 0;
        prSeqInfo->u2_height = 0;
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        if (prVdecEsInfoKeep->pfnVdecGetParam)
        {
            prVdecEsInfoKeep->pfnVdecGetParam(ucEsId, (UINT32)SEQ_HDR_INFO, &u4W, &u4H, &u4Dummy);
            prSeqInfo->u2_width = (UINT16)u4W;
            prSeqInfo->u2_height = (UINT16)u4H;
        }
        #endif
        #endif
    }
    else if(eFmt == VDEC_FMT_MPV)
    {
        UINT32 u4HResolution;
        UINT32 u4VResolution;
        INT32 i4PrgScan;
        MPV_ALL_ASPECT_RATIO_T e4Ratio;
        UINT32 u4FrmRat;

        if(prVdecEsInfoKeep->pfnVdecSWRst)
        {
            prVdecEsInfoKeep->pfnVdecSWRst(ucMpvId, ucEsId);
        }
        if(prVdecEsInfoKeep->pfnVdecHdrParse)
        {
            UNUSED(prVdecEsInfoKeep->pfnVdecHdrParse(ucMpvId, ucEsId));
        }

        MPV_GetVideoInfo(ucEsId, &u4HResolution, &u4VResolution,
                            &i4PrgScan, &e4Ratio, &u4FrmRat);
        prSeqInfo->u2_width = (UINT16)u4HResolution;
        prSeqInfo->u2_height = (UINT16)u4VResolution;

        //VERIFY(x_sema_unlock(prVdecInfo->arVdecVldInfo[VLD0].hVldSemaphore) == OSR_OK);
    }
    else if(eFmt == VDEC_FMT_WMV)
    {
        if(prVdecEsInfoKeep->pfnVdecHdrParse)
        {
            UNUSED(prVdecEsInfoKeep->pfnVdecHdrParse(ucMpvId, ucEsId));
        }
        prSeqInfo->u2_width = 0;
        prSeqInfo->u2_height = 0;
    }

    if(eFmt == VDEC_FMT_MPV)
    {
#if defined (CC_MT5890)
        extern void PP_WRITE32(int id, int offset, int value);
        extern void VLD_TOP_WRITE32(int id, int offset, int value);

        PP_WRITE32(prPesInfo->ucMpvId, PP_45, 0);  
        PP_WRITE32(prPesInfo->ucMpvId, PP_46, 480);
        PP_WRITE32(prPesInfo->ucMpvId, PP_47, 960);
        PP_WRITE32(prPesInfo->ucMpvId, PP_48, 1232);  
        PP_WRITE32(prPesInfo->ucMpvId, PP_49, 1368);
        PP_WRITE32(prPesInfo->ucMpvId, PP_50, 1436);
        PP_WRITE32(prPesInfo->ucMpvId, PP_51, 1556);  
        PP_WRITE32(prPesInfo->ucMpvId, PP_52, 960);
        PP_WRITE32(prPesInfo->ucMpvId, PP_705, 60);
        PP_WRITE32(prPesInfo->ucMpvId, PP_706, 128);  
        PP_WRITE32(prPesInfo->ucMpvId, PP_707, 162);
        PP_WRITE32(prPesInfo->ucMpvId, PP_45, 0);
        if(prPesInfo->ucMpvId == VLD1)
        {
            VLD_TOP_WRITE32(prPesInfo->ucMpvId, VLD_TOP_52, 0x02080800);
            PP_WRITE32(prPesInfo->ucMpvId, PP_773, 0x00000800);
        }
#endif
    }

    prVdecEsInfo->fgParseredFromInput = TRUE;
#if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, prPesInfo->ucMpvId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_FLW_BASE + 4);
#endif
    _VDEC_UnlockVld(ucEsId, prPesInfo->ucMpvId);

#ifdef CC_VDEC_RM_SUPPORT
    prVdecEsInfo->u4VldId = VDEC_MAX_VLD;
#endif
    UNUSED(prPesInfo);
    return TRUE;
}

#ifdef CC_VDEC_RM_SUPPORT
VOID _VDEC_RMKickRet(UCHAR ucEsId, VDEC_RM_BLOCK_E eCond)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "[%s] ucEsId(%d) > VDEC_MAX_ES, oops...\n", __func__, ucEsId);
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    LOG(4, "ucEsId(%d) Kick Ret Cond(0x%x).\n", ucEsId, eCond);

    if (!prVdecEsInfoKeep->hWaitKickSema ||
        (prVdecEsInfo->ucBlockAsNoResource == VDEC_RM_BLK_NONE))
    {
        return;
    }
    

    if ((eCond == VDEC_RM_BLK_VDEC_DONE) && 
        (prVdecEsInfo->ucBlockAsNoResource & VDEC_RM_BLK_VDEC_WAIT))
    {
        prVdecEsInfo->ucBlockAsNoResource &= ~VDEC_RM_BLK_VDEC_WAIT;
    }
    else if ((eCond == VDEC_RM_BLK_B2R_DONE) && 
        (prVdecEsInfo->ucBlockAsNoResource & VDEC_RM_BLK_B2R_WAIT))
    {
        prVdecEsInfo->ucBlockAsNoResource &= ~VDEC_RM_BLK_B2R_WAIT;
    }
    else if ((eCond == VDEC_RM_BLK_IMGRZ_DONE) && 
        (prVdecEsInfo->ucBlockAsNoResource & VDEC_RM_BLK_IMGRZ_WAIT))
    {
        prVdecEsInfo->ucBlockAsNoResource &= ~VDEC_RM_BLK_IMGRZ_WAIT;
    }
    else if (eCond == VDEC_RM_BLK_FORCE_CLR)
    {
        prVdecEsInfo->ucBlockAsNoResource = VDEC_RM_BLK_FORCE_CLR;
    }

    if ((prVdecEsInfo->ucBlockAsNoResource == VDEC_RM_BLK_NONE) ||
        (prVdecEsInfo->ucBlockAsNoResource == VDEC_RM_BLK_FORCE_CLR))
    {
        x_sema_unlock(prVdecEsInfoKeep->hWaitKickSema);  // == OSR_OK);
    }
}


static BOOL _VDEC_RMAllocFlw(UCHAR ucEsId)
{
    INT32 i4RetValue = 0;
    UCHAR aucConflictEsId[VDEC_MAX_ES-1] = {VDEC_MAX_ES};
    BOOL fgNotifyFail = FALSE;
    BOOL fgAllocOK = FALSE;
    BOOL fgAllocVdecOK = FALSE;
    #ifdef CC_B2R_RM_SUPPORT
    BOOL fgAllocB2rOK = FALSE;
    UCHAR ucB2rId = B2R_NULL;
    #endif    
    #ifdef CC_IMGRZ_DYNAMIC_REQ
    BOOL fgAllocImgrzOK = FALSE;
    #endif
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    LOG(6, "Alloc Before Vld(%d) Block(%d).\n", prVdecEsInfo->u4VldId, prVdecEsInfo->ucBlockAsNoResource);
    prVdecEsInfo->ucBlockAsNoResource = VDEC_RM_BLK_NONE;

    do
    {
        #if defined(CC_IMGRZ_DYNAMIC_REQ)
        #ifdef ENABLE_MULTIMEDIA
        if (VDEC_IS_SEAMLESS(prVdecEsInfo->eMMSrcType))
        {
            VDEC_ChkSeamlessModeChg(ucEsId, prVdecEsInfo->u2OrgHSize, prVdecEsInfo->u2OrgVSize);
        }
        #endif
        #endif
                
        //Alloc VDEC VLD
        if (!fgAllocVdecOK)
        {            
            if (VDEC_FMT_MJPEG == prVdecEsInfoKeep->eCurFMT)                    
            {
                VDEC_VLDAllocVldImg(ucEsId);
                fgAllocVdecOK = TRUE;

                #ifdef CC_B2R_RM_SUPPORT
                if ((ucEsId != ES0) && (prVdecEsInfo->e3DType == VDEC_3D_DUAL_STREAM))
                {
                     //Tmp for 3D MJPEG: alloc b2r twice fail.
                    fgAllocB2rOK = TRUE;
                }
                #endif
            }
            else
            if (!VDEC_VLDMngrAllocVld(ucEsId, aucConflictEsId))
            {
                if ((aucConflictEsId[0] != VDEC_MAX_ES) && prVdecInfo->pfnRmCb)
                {
                    fgNotifyFail = FALSE;
                    prVdecEsInfo->ucBlockAsNoResource |= VDEC_RM_BLK_VDEC_WAIT;
                    //notify to resource manger conflict 
                    LOG(3, "RM CB: Wait Kick VDEC.\n");
                    prVdecInfo->pfnRmCb(ucEsId, VDEC_RM_ALLOC_CFT, aucConflictEsId);
                }
                else
                {
                    //Alloc Fail
                    LOG(3, "RM CB: Alloc Fail. No Cft(%d) or Kick(0x%p).\n", aucConflictEsId[0], prVdecInfo->pfnRmCb);
                    fgNotifyFail = TRUE;
                    break;
                }
            }
            else
            {
                fgAllocVdecOK = TRUE;
            }
        }
        
        #ifdef CC_B2R_RM_SUPPORT
        //Alloc B2R HW
        if ((!prVdecEsInfo->fgRenderPicCnt) && !fgAllocB2rOK && 
            (!prVdecEsInfoKeep->fgVPush || (prVdecEsInfoKeep->fgVPush && prVdecEsInfo->u4RenderVDPId != 0xFF)))
            //None push, or Push must set render VDP id.
        {
            B2R_RM_T input_Src;
            
            input_Src.eVdecType = prVdecEsInfoKeep->eCurFMT;
            input_Src.u2HActive = prVdecEsInfo->u2OrgHSize;
            input_Src.u2VActive = prVdecEsInfo->u2OrgVSize;
            input_Src.u2Fps = prVdecEsInfo->u4FrameRate/100;
            input_Src.fgUFOEnable = prVdecEsInfo->fgEnableUFO;
            input_Src.fg8Bit = (prVdecEsInfo->u4BitDepth == 8);
            input_Src.u2BlockMode = TRUE;
            input_Src.ucEsId = ucEsId;
            
            #ifdef USB_CLI_FORCE_SETTING_SUPPORT 
            if (prVdecEsInfo->u4B2rIdUsage)
            {
                VDP_SetB2RId(prVdecEsInfo->u4B2rIdUsage - 1, ucEsId, ucEsId);
            }
            else
            #endif
            if (prVdecEsInfo->u4RenderVDPId != 0xFF)
            {
                VDP_SetB2RId(B2R_NULL, prVdecEsInfo->u4RenderVDPId, ucEsId);
            }
            
            ucB2rId = B2R_Source_Connect(input_Src);
            
            if (prVdecEsInfoKeep->pfnVdecSetParam && (ucB2rId >= B2R_3) && (ucB2rId < B2R_NULL))
            {
                prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)VDEC_BLK_MODE, 0, 0, 0);
            }
            fgAllocB2rOK = (ucB2rId != B2R_NULL);
            if ((ucB2rId == B2R_NULL) && prVdecInfo->pfnRmCb)  //TBD, use B2R RmCb Func   
            {
                LOG(3, "RM CB: Wait Kick B2R CFT.\n");
                prVdecEsInfo->ucBlockAsNoResource |= VDEC_RM_BLK_B2R_WAIT;
            }
        }
        #endif
#ifdef CC_IMGRZ_DYNAMIC_REQ
        //Alloc Imgrz for seamless
        if ((!prVdecEsInfo->fgRenderPicCnt) && !fgAllocImgrzOK &&   //Not thumbnail
            (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_RESIZER) &&  //Seamless use resizer
            (!prVdecEsInfoKeep->fgVPush || (prVdecEsInfoKeep->fgVPush && prVdecEsInfo->u4RenderVDPId != 0xFF))) //except GFX path 
        {
            UINT32 u4ReqImgrzNum = 1, u4HaveImgrzNum = 0;
            INT32 i4Idx = 0;
            if(prVdecEsInfo->fgEnableUFO || (prVdecEsInfo->u4BitDepth == 10)) // UFO & 10bit
            {
                u4ReqImgrzNum = 2;
            }

            do
            {
                if (prVdecEsInfo->ucImgrzId & (0x01<<i4Idx))
                {
                    u4HaveImgrzNum++;
                }
            }while (++i4Idx < sizeof(prVdecEsInfo->ucImgrzId)*8);
            
            if (u4ReqImgrzNum > u4HaveImgrzNum)
            {
                if (prVdecInfo->pfnRmCb)
                {
                    fgNotifyFail = FALSE;
                    prVdecEsInfo->ucBlockAsNoResource |= VDEC_RM_BLK_IMGRZ_WAIT;
                    //notify to resource manger conflict 
                    u4ReqImgrzNum = u4ReqImgrzNum - u4HaveImgrzNum;
                    LOG(3, "RM CB: Wait Add more %d Imgrz. Have num(%d).\n", u4ReqImgrzNum, u4HaveImgrzNum);
                    prVdecInfo->pfnRmCb(ucEsId, VDEC_RM_IMGRZ_REQ, (void *)&u4ReqImgrzNum);
                }
                else
                {
                    fgNotifyFail = TRUE;
                    LOG(3, "RM CB: Fail Add Imgrz.\n");
                }
            }
            else
            {
                fgAllocImgrzOK = TRUE;
            }
        }
#endif

        if ((prVdecEsInfo->ucBlockAsNoResource & VDEC_RM_BLK_VDEC_WAIT) ||
            (prVdecEsInfo->ucBlockAsNoResource & VDEC_RM_BLK_B2R_WAIT) ||
            (prVdecEsInfo->ucBlockAsNoResource & VDEC_RM_BLK_IMGRZ_WAIT))  // Kick out Kick Done
        {
            if (prVdecEsInfoKeep->hWaitKickSema)
            {
                i4RetValue = x_sema_lock_timeout(prVdecEsInfoKeep->hWaitKickSema, 15000); // 15s
                if (i4RetValue == OSR_TIMEOUT)
                {
                    LOG(3, "Wait Kick Timeout Fail WaitMsk(%d).\n", prVdecEsInfo->ucBlockAsNoResource);
                    fgNotifyFail = TRUE;
                }
                else if ((i4RetValue == OSR_OK) && (prVdecEsInfo->ucBlockAsNoResource == VDEC_RM_BLK_NONE))
                {
                    //Kick Done, go next
                    LOG(3, "RM CB: Kick Done.\n");
                    continue;
                }
                else if ((i4RetValue == OSR_OK) && (prVdecEsInfo->ucBlockAsNoResource == VDEC_RM_BLK_FORCE_CLR))
                {
                    //Force stop, not notify runtime error
                    LOG(3, "RM CB: Kick Fail -> Stop.\n");
                    fgNotifyFail = FALSE;
                    //break;
                }
                else
                {
                    LOG(3, "RM CB: Kick Fail(%d).\n", prVdecEsInfo->ucBlockAsNoResource);
                    ASSERT(0);
                    fgNotifyFail = TRUE;
                }
            }
            else
            {
                ASSERT(prVdecEsInfoKeep->hWaitKickSema);
                fgNotifyFail = TRUE;
            }
        }
        else
        {
            fgAllocOK = TRUE;
        }

        prVdecEsInfo->ucBlockAsNoResource = VDEC_RM_BLK_NONE;
        
        break;
    }while(!fgAllocOK);


    //notify to resource manger runtime error
    if (fgNotifyFail)
    {
        LOG(3, "ES(%d) Notify RM Alloc Fail.\n", ucEsId);
        if (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_HD_NOT_SUPPORT)
        {
            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_HD_NOT_SUPPORT;
            if (prVdecEsInfoKeep->pfDecNfyFct) 
            {
                LOG(3, "ES(%d) Notify RM Alloc Fail VDEC_DEC_DECODE_HD_NOT_SUPPORT\n", ucEsId);
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                              VDEC_COND_DECODE_STATUS_CHG, (UINT32)prVdecEsInfo->eNotifyStatus, 0);
            }
            prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
        }
    }

    return fgAllocOK;
}
#endif


void _VDEC_MainLoop(void* pvArg)
{
    INT32 i4RetValue = 0;
    VDEC_DISP_PIC_INFO_T rDispInfo;
    UCHAR ucEsId;
    UCHAR ucMpvId = 0;
    BOOL fgNeedStopNfy = TRUE;
    VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;    

    if(pvArg == NULL)
    {
        ASSERT(!pvArg);
        ucEsId = ES0;
        LOG(1, "_VDEC_MainLoop(): (pvArg == NULL)\n");
    }
    else
    {
        ucEsId = *(UCHAR*)pvArg;
    }

    if (ucEsId >= VDEC_MAX_ES)
    {
        ASSERT(ucEsId < VDEC_MAX_ES);
        ucEsId = ES0;
        LOG(1, "_VDEC_MainLoop(): (ucEsId >= VDEC_MAX_ES)\n");
    }
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    prVdecEsInfoKeep->fgThreadActive = TRUE;
    prVdecEsInfoKeep->fgThreadDestroy = FALSE;
    #ifdef VDEC_TIME_PROFILE
    _rEsAcc.u4Seconds = 0;
    _rEsAcc.u4Micros = 0;
    #endif

    x_memset(&rDispInfo, 0,sizeof(VDEC_DISP_PIC_INFO_T));
    
    while (prVdecInfo->fgVDecInitiated)
    {
        if((prVdecEsInfoKeep->eCurState == VDEC_ST_STOP) || (prVdecEsInfoKeep->eCurState == VDEC_ST_RESTART))
        {
            if(prVdecEsInfoKeep->eCurState == VDEC_ST_STOP)
            {
                _VDecReceiveCmd(ucEsId, TRUE);
            }
            else
            {
                //Virtual play
                prVdecEsInfoKeep->eCurFMT = prVdecEsInfo->eNextFmt;
                prVdecEsInfoKeep->eCurState = VDEC_ST_PLAY;
            }
            if((prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY) || (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
            {
                LOG(6, "Config\n");
                prVdecEsInfo->u1LastSkipType = IPBMode;
                prVdecEsInfo->eStreamType = VDEC_STREAM_TYPE_UNKNOW;
                #ifdef CC_REALD_3D_SUPPORT
                prVdecEsInfo->eDetectedMode = E_TDTV_DECODER_INPUT_END;
                #endif
                //config, create es queue at first time
                if(_VDEC_DecCfg(ucEsId) != (INT32)E_VDEC_OK)
                {
                    prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
                    prVdecEsInfoKeep->eCurState = VDEC_ST_STOP;
                    continue;
                }
                prVdecEsInfo->u4MaxEsCnt = 0;

                #ifdef CC_SUPPORT_VDEC_PREPARSE
                if(prVdecEsInfo->ucVDecStatus == LAE_DECODE)
                {
                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                }
                else
                {
                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_PRE_PARSER;
                }
                //(prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME) ? VDEC_DEC_RES_ALLOC : VDEC_DEC_PRE_PARSER;
                #else
                if(prVdecEsInfo->ucVDecStatus == LAE_DECODE)
                {
                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                    //Temp fix VDEC_LAE0 
                    prVdecEsInfo->u4VldId = VDEC_LAE0;
                }
                else
                {
                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_RES_ALLOC;
                }
                #endif
            }
        }
        else if(prVdecEsInfoKeep->eCurState == VDEC_ST_IDLE)
        {
            if(prVdecEsInfo->fgMMPlayback)
            {
                // just pause, MM case
                _VDecReceiveCmd(ucEsId, TRUE);

                //clear queue, clear stored info, do notification
                _VDEC_LockFlushMutex(ucEsId);
                if (prVdecEsInfo->fgFlushEsmQ)
                {
                    LOG(1, "ES(%d) VDEC_ST_IDLE Flush\n", ucEsId);
                    _VDEC_FlushEs(ucEsId);
                    prVdecEsInfo->fgFlushEsmQ = FALSE;
                    _VDEC_UnlockFlushSema(ucEsId);
                }
                _VDEC_UnlockFlushMutex(ucEsId);

                if(prVdecEsInfoKeep->eCurState==VDEC_ST_PLAY ||prVdecEsInfoKeep->eCurState==VDEC_ST_PLAY_I_FRAME)
                {
                    MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_RESUME_DONE);
                }
            }
            else
            {
                // dtv ts, eat es data
                ucMpvId = _VDecReceiveEs(ucEsId);
                _VDecReceiveCmd(ucEsId, FALSE);
            }
        }
        else
        {
            _VDecReceiveCmd(ucEsId, FALSE);
        }

        switch(prVdecEsInfoKeep->eCurState)
        {
            case VDEC_ST_PLAY:
            case VDEC_ST_PLAY_I_FRAME:
            {                
                #ifdef CC_SUPPORT_VDEC_PREPARSE
                if (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_PRE_PARSER)
                {
                    // when last memory is on, maybe there is no seq header in es, use info from 
                    // _vdec_prsseqhdr    
                    if((prVdecEsInfo->fgParseredFromInput) && (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV))
                    {
                        prVdecEsInfo->u4FrameRate /= 10;  //MPV Codec Frame rate covert to Pre-parser      
                        prVdecEsInfo->fgEnableUFO = FALSE;       
                        prVdecEsInfo->u4BitDepth = 8;      
                        
                        prVdecEsInfoKeep->eDecStatus = VDEC_DEC_RES_ALLOC;
                        prVdecEsInfo->ePreParseStatus = VDEC_PREPS_ST_DONE;                         
                        
                        LOG(1, "Skip Pre-Parser: W/H(%dx%d %s @%dfps).\n", 
                            prVdecEsInfo->u2OrgHSize,  prVdecEsInfo->u2OrgVSize, 
                            prVdecEsInfo->fgProgressive ? "P" : "I", prVdecEsInfo->u4FrameRate);                            
                    }
                    else
                    {
                        UINT32 u4CntThreshold = VDEC_FRM_ERR_THRSD_STARTPLAY;
                        LOG(6, "VDEC Pre-parser.\n");
                        if (prVdecEsInfo->ePreParseStatus == VDEC_PREPS_ST_NONE)
                        {
                            prVdecEsInfo->ePreParseStatus = VDEC_PREPS_ST_BEGIN;
                        }
                        
                        ucMpvId = _VDecReceiveEs(ucEsId);
                        if (VDEC_MAX_VLD == prVdecEsInfoKeep->rPesInfo.ucMpvId)
                        {
                            LOG(6, "Empty Es.\n");
                            break;
                        }
                        i4RetValue = VdecHdrPreParse(ucMpvId, ucEsId);
                        if ((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264) || 
                            (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265))
                        {
                            u4CntThreshold *= 3;
                        }
                        #ifdef ENABLE_MULTIMEDIA
                        if (prVdecEsInfo->fgMMPlayback 
                            && (prVdecEsInfo->eMMSrcType == (SWDMX_SOURCE_TYPE_T)SWDMX_SRC_TYPE_HIGH_SPEED_STORAGE)
                            && (prVdecEsInfo->prMpvCounter->u4RetrieveNs > u4CntThreshold))
                        {
                            LOG(1, "ES(%d) Pre-Parser Cnt over cnt(%d).\n", ucEsId, u4CntThreshold);                
                            i4RetValue = E_VDEC_FAIL;
                        }
                        #endif
                        if(i4RetValue == (INT32)E_VDEC_PARSE_NOT_READY)
                        {
                            //Drop the ES for VFIFO not full and continue next .
                            VDEC_PREPS_ST_T  eSaveStatus = prVdecEsInfo->ePreParseStatus; 
                            UINT32 u4CurRptr = prVdecEsInfoKeep->rPesInfo.u4VldReadPtr;
                            
                            prVdecEsInfo->ePreParseStatus = VDEC_PREPS_ST_SKIP_ES;
                            _VDecReceiveEs(ucEsId);  //Drop by: Receieve Es from Es-Q, and ignore it.
                            if(!VDEC_SetRptr(ucEsId, u4CurRptr, u4CurRptr))                
                            {   
                                LOG(1, "Pre-parser ES(%d) VDEC_SetRptr(0x%08x) Fail\n", ucEsId, u4CurRptr);                
                            }

                            if(prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME)
                            {
                                if (prVdecEsInfoKeep->pfDecNfyFct)
                                {
                                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_ERROR, 0, 0);
                                }
                            }
                            
                            prVdecEsInfo->ePreParseStatus = eSaveStatus;
                            break;
                        }
                        else if(i4RetValue != (INT32)E_VDEC_OK)
                        {
                            //notify to resource manger runtime error 
                            if (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_HD_NOT_SUPPORT)
                            {
                                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_HD_NOT_SUPPORT;
                                if (prVdecEsInfoKeep->pfDecNfyFct) 
                                {
                                    LOG(2, "ES(%d) Pre-parser Notify: NOT_SUPPORT.\n", ucEsId);
                                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                                  VDEC_COND_DECODE_STATUS_CHG, (UINT32)prVdecEsInfo->eNotifyStatus, 0);
                                }
                                prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                            }
                            break;
                        }
                        else if (prVdecEsInfo->u4FrameRate == 0) //Unknow Frame rate.  //Not ready yet
                        {
                            prVdecEsInfo->ePreParseStatus = VDEC_PREPS_ST_FPS_CALC; 
                            //continue next 
                            do
                            {
                                _VDecReceiveEs(ucEsId);
                            }while(VdecCalcFRFromES(ucEsId) != (INT32)E_VDEC_OK);
                            //break;
                        }
                        prVdecEsInfoKeep->eDecStatus = VDEC_DEC_RES_ALLOC;
                        prVdecEsInfo->ePreParseStatus = VDEC_PREPS_ST_DONE; 
                        LOG(1, "Pre-Parser: W/H(%dx%d %s @%dfps).\n", 
                            prVdecEsInfo->u2OrgHSize,  prVdecEsInfo->u2OrgVSize, 
                            prVdecEsInfo->fgProgressive ? "P" : "I", prVdecEsInfo->u4FrameRate);
                    }                    
                }
                #endif

                if (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_RES_ALLOC)
                {
                    #ifdef CC_VDEC_RM_SUPPORT
                    _VDEC_RMAllocFlw(ucEsId);
                    #endif

                    if (prVdecEsInfo->u4VldId >= VDEC_MAX_VLD)
                    {
                        LOG(1, "Alloc After u4VldId(%d) >= VDEC_MAX_VLD\n", prVdecEsInfo->u4VldId);
                        #ifdef CC_VDEC_RM_SUPPORT
                        if (prVdecEsInfo->eNotifyStatus == VDEC_DEC_DECODE_HD_NOT_SUPPORT)
                        {
                            break;
                        }
                        //notify to resource manger runtime error
                        #else
                        prVdecEsInfo->u4VldId = VLD0;
                        #endif
                    }
                    _VDEC_SetVdecStatus(ucEsId);
                    UNUSED(VDEC_PowerOn(ucEsId, prVdecEsInfo->u4VldId));
                    //reset
                    if (prVdecEsInfoKeep->pfnVdecSWRst)
                    {
                        if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                        {
                            _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
                        }
                        
                        prVdecEsInfoKeep->pfnVdecSWRst(prVdecEsInfo->u4VldId, ucEsId);
    
                        if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                        {
                            _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
                        }
                    }
                    #ifdef VDEC_TIME_PROFILE
                    HAL_GetTime(&_rTimeFrmS);
                    #endif
                    MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_PLAY_DONE);

                    #ifdef CC_SUPPORT_VDEC_PREPARSE
                    //preparser can not influence header parse flow, so theses in prVdecEsInfo need to be reset
                    //or some flow in mpv are wrong
                    if ((prVdecEsInfo->fgParseredFromInput)&& (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV))
                    {
                        //u4FrameRate is parsed by vdec_prsseqhdr, and u4FrameRate/10 for VDEC_DEC_RES_ALLOC
                        // so here need to *10.
                        prVdecEsInfo->u4FrameRate *= 10;  //MPV Codec Frame rate covert from Pre-parser
                    }
                    else
                    {
                        prVdecEsInfo->u2OrgHSize = 0;
                        prVdecEsInfo->u2OrgVSize = 0;
                        prVdecEsInfo->fgProgressive = FALSE;
                        prVdecEsInfo->u4FrameRate = 0;
                    }
                    #endif         

                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                }

                if(prVdecEsInfoKeep->pfnVdecDecode)
                {                    
                    #ifdef CC_SUPPORT_VDEC_PREPARSE
                    if ((prVdecEsInfoKeep->eDecStatus == VDEC_DEC_PRE_PARSER) || (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_RES_ALLOC))
                    #else
                    if (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_RES_ALLOC)
                    #endif
                    {
                        break;
                    }
                    LOG(6, "Decode\n");
                    #ifdef VDEC_TIME_PROFILE
                    HAL_GetTime(&_rTimeEsS);
                    #endif

                    ucMpvId =  _VDecReceiveEs(ucEsId);

                    #ifdef VDEC_TIME_PROFILE
                    HAL_GetTime(&_rTimeEsE);
                    HAL_GetDeltaTime(&_rTimeEsDt, &_rTimeEsS, &_rTimeEsE);
                    //LOG(1, "WEs %u.%06us\n", _rTimeEsDt.u4Seconds, _rTimeEsDt.u4Micros);
                    #endif

                    if(ucMpvId >= VDEC_MAX_VLD)
                    {
                        _VDEC_DeliverEos(ucEsId);
                        break;
                    }
                    // lock vld

                    if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                    {
                        _VDEC_LockVld(ucEsId, ucMpvId);
                        #if VLD_LOCK_LOG
                        u4VLDLogWr(ucEsId, ucMpvId, VLD_LOCK_LOG_FLW_BASE+2);
                        #endif
                    }
                    i4RetValue = prVdecEsInfoKeep->pfnVdecDecode(ucMpvId, ucEsId);
                    #ifdef VDEC_TIME_PROFILE
                    _VdecTimeProfile(ucEsId);
                    #endif
                    // TODO:
                    // check notify status if OK and Drop

                     //update rptr
                    if(prVdecEsInfoKeep->pfnVdecUpdRptr)
                    {
                        prVdecEsInfoKeep->pfnVdecUpdRptr(ucEsId);
                    }

                    _VDEC_DeliverEos(ucEsId);
                    if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                    {
                        #if VLD_LOCK_LOG
                        u4VLDLogWr(ucEsId, ucMpvId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_FLW_BASE + 5);
                        #endif
                        _VDEC_UnlockVld(ucEsId, ucMpvId);
                    }

                    if ((i4RetValue == (INT32)E_VDEC_NOT_SUPPORT) &&
                                (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_CODEC_NOT_SUPPORT))
                    {
                        if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                        {
                            LOG(2, "ES(%d) Notify Status change, codec not support\n", ucEsId);
                            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_CODEC_NOT_SUPPORT, 0);
                            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_CODEC_NOT_SUPPORT;
                            if (prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                            {
                                prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                            }
                            prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                        }
                    }
                    else if ((i4RetValue == (INT32)E_VDEC_RES_NOT_SUPPORT) &&
                         ((prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_RES_NOT_SUPPORT)))
                    {
                        if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                        {
                            LOG(2, "ES(%d) Notify Status change, resolution not support\n", ucEsId);
                            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                          VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_RES_NOT_SUPPORT, 0);
                            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_RES_NOT_SUPPORT;
                            if (prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                                {
                                    prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                                }
                                prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                        }
                    }
                    #ifdef ENABLE_VDEC_FILE_NOT_SUPPORT
                    else if ((i4RetValue == (INT32)E_VDEC_FILE_NOT_SUPPORT) &&
                            (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_FILE_NOT_SUPPORT))
                    {
                        if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                        {
                            LOG(2, "Notify Status change, vdec set file not support\n");
                            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                          VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_FILE_NOT_SUPPORT, 0);
                            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_FILE_NOT_SUPPORT;
                            if (prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                            {
                                prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                            }
                        }
                    }
                    #endif

                #ifdef CC_VDEC_PRIORITY_ENABLE
                    VDEC_ThreadSchedule(ucEsId);
                #endif
                }
                else
                {
                    if ((prVdecEsInfoKeep->eDecStatus == VDEC_DEC_HEADER_PARSE) ||
                        (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_HEADER_REPARSE))
                    {
                        LOG(9, "%d, VDEC_DEC_HEADER_PARSE\n", ucMpvId);
                    #ifdef VDEC_TIME_PROFILE
                        HAL_GetTime(&_rTimeEsS);
                    #endif
                        if (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_HEADER_PARSE)
                        {
                            ucMpvId = _VDecReceiveEs(ucEsId);
                        }

                        #ifdef VDEC_TIME_PROFILE
                        HAL_GetTime(&_rTimeEsE);
                        HAL_GetDeltaTime(&_rTimeEsDt, &_rTimeEsS, &_rTimeEsE);
                        _rEsAcc.u4Seconds += _rTimeEsDt.u4Seconds;
                        _rEsAcc.u4Micros  += _rTimeEsDt.u4Micros;
                        if (_rEsAcc.u4Micros >= 1000000)
                        {
                            _rEsAcc.u4Seconds += _rEsAcc.u4Micros / 1000000;
                            _rEsAcc.u4Micros  %= 1000000;
                        }
                        #endif

                        if(ucMpvId >= VDEC_MAX_VLD)
                        {
                            if(prVdecEsInfo->fgCodecChange)
                            {
                                VdecCodecChangeHandle(ucEsId);
                            }
                            else
                            {
                                _VDEC_DeliverEos(ucEsId);
                            }
                            break;
                        }
                        // lock vld, TODO
                        //check if need re-config according to id in PesInfo?
                        i4RetValue = prVdecEsInfoKeep->pfnVdecHdrParse(ucMpvId, ucEsId);
                        #if defined(VDEC_4K_DEC_CHECK)
                        if(!VDEC_Chk4K2KCap(ucEsId, prVdecEsInfoKeep->eCurFMT))
                        {
                            LOG(2,"Not support 4K2K or 4K2K@60fps\n");
                            i4RetValue = E_VDEC_NOT_SUPPORT;
                        }
                        #endif
                        
                        if((i4RetValue == (INT32)E_VDEC_OK) ||
                            (i4RetValue == (INT32)E_VDEC_OK_RES_CHG))            //parse done
                        {
                            if(prVdecEsInfo->fgFmtDectFinish)
                            {
                                prVdecEsInfoKeep->eDecStatus = VDEC_DEC_TRIGGER;
                                #ifdef CC_VDEC_RM_SUPPORT
                                {
                                    UINT32 u4W, u4H ,u4FrameRate;
                                    if(i4RetValue == (INT32)E_VDEC_OK_RES_CHG)
                                    {
                                        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,SEQ_HDR_INFO,&u4W,&u4H,&u4FrameRate);
                                        LOG(2,"BS resolution change WH(%d,%d) FR %d\n",u4W,u4H,u4FrameRate);
                                        //VDEC RM handle here
                                        //.......
                                    }
                                }
                                #endif
                            }
                            else
                            {
                                if(VDEC_ChkCodecCap(ucEsId, prVdecEsInfoKeep->eCurFMT))
                                {
                                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_TRIGGER;
                                }
                                else
                                {
                                    prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                                    if(prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_CODEC_NOT_SUPPORT)
                                    {
                                        if (prVdecEsInfoKeep->pfDecNfyFct)
                                        {
                                            LOG(2, "ES(%d) Notify Status change, codec not support\n", ucEsId);
                                            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_CODEC_NOT_SUPPORT, 0);
                                            prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_CODEC_NOT_SUPPORT;
                                            if(prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                                            {
                                                prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                                            }
                                            prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                                        }
                                    }   
                                }
                                prVdecEsInfo->fgFmtDectFinish = TRUE;
                            }
                        }
                        else if (i4RetValue == (INT32)E_VDEC_REPARSE)
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_REPARSE;
                        }
                        else if(i4RetValue == (INT32)E_VDEC_FAIL)    //error
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_DROP;
                        }
                        else if(i4RetValue == (INT32)E_VDEC_BS_END)
                        {
                            _VDEC_DeliverEos(ucEsId);
                            if(prVdecInfo->pfErrNotify)
                            {
                                prVdecInfo->pfErrNotify((UINT32)VDEC_EVT_DEC_BS_END, (UINT32)&rDispInfo, 0, 0);
                            }
                            if(prVdecEsInfoKeep->pfnVdecStop)
                            {
                                prVdecEsInfoKeep->pfnVdecStop(ucEsId);
                            }
                            prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
                            prVdecEsInfoKeep->eCurState = VDEC_ST_STOP;
                            _VDEC_FlushEs(ucEsId);
                            prVdecEsInfo->fgFmtDectFinish = TRUE;
                            // TODO
                        }
                        else
                        {
                            _VDEC_DeliverEos(ucEsId);
                            if((i4RetValue == (INT32)E_VDEC_NOT_SUPPORT) &&
                                (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_CODEC_NOT_SUPPORT))
                            {
                                if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                                {
                                    LOG(2, "ES(%d) Notify Status change, codec not support\n", ucEsId);
                                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                        VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_CODEC_NOT_SUPPORT, 0);
                                    prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_CODEC_NOT_SUPPORT;

                                    if(prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                                    {
                                        prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                                    }
                                    prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                                }
                                prVdecEsInfo->fgFmtDectFinish = TRUE;
                            }
                            else if ((i4RetValue == (INT32)E_VDEC_FRAME_RATE_NOT_SUPPORT) &&
                                (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_FRAME_RATE_OVERFLOW))
                            {
                                if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                                {
                                    LOG(2, "Notify Status change, frame rate no support\n");
                                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                        VDEC_DEC_DECODE_FRAMERATE_NOT_SUPPORT, (UINT32)VDEC_DEC_DECODE_FRAME_RATE_OVERFLOW, 0);
                                    prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_FRAME_RATE_OVERFLOW;

                                    if(prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                                    {
                                        prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                                    }
                                    //if no PES data after notify, thread will hang at receieve ES
                                    //Actully upper layer will send PES continus,so vdec will skip 
                                    //those PES.
                                    prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                                }
                            }
                            else if ((i4RetValue == (INT32)E_VDEC_RES_NOT_SUPPORT) &&
                                        (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_RES_NOT_SUPPORT))
                            {
                                if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                                {
                                    LOG(2, "Notify Status change, resolution not support\n");
                                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                                  VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_RES_NOT_SUPPORT, 0);
                                    prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_RES_NOT_SUPPORT;
                                    if (prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                                    {
                                        prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                                    }
                                }
                            }
                            #ifdef ENABLE_VDEC_FILE_NOT_SUPPORT
                            else if ((i4RetValue == (INT32)E_VDEC_FILE_NOT_SUPPORT) &&
                                        (prVdecEsInfo->eNotifyStatus != VDEC_DEC_DECODE_FILE_NOT_SUPPORT))
                            {
                                if (prVdecEsInfoKeep->pfDecNfyFct)    //enable after mw add this handler
                                {
                                    LOG(2, "Notify Status change, vdec set file not support\n");
                                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                                                  VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_FILE_NOT_SUPPORT, 0);
                                    prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_FILE_NOT_SUPPORT;
                                    if (prVdecEsInfoKeep->pfVdecNotSupporuNfyFct)
                                    {
                                        prVdecEsInfoKeep->pfVdecNotSupporuNfyFct(ucEsId, TRUE);
                                    }
                                }
                            }
                            #endif
                             //update rptr
                            if((prVdecEsInfoKeep->pfnVdecUpdRptr) &&
                               (prVdecEsInfoKeep->eCurFMT != VDEC_FMT_MP4))
                            {
                                prVdecEsInfoKeep->pfnVdecUpdRptr(ucEsId);
                            }
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                        }
                        //update rptr
                        // may need another one for H264
                    }
                    //state combine(hdr parse+ trigger+ wait dec done)
                    if (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_TRIGGER)
                    {
                        //LOG(6, "Pic %d Decoding\n", u4DecCount++);
                        LOG(9, "%d, VDEC_DEC_TRIGGER\n", ucMpvId);
                        if(prVdecEsInfoKeep->fgVdecMidEnable && prVdecEsInfoKeep->pfnVdecGetDispPic)
                        {
                            if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264) ||
                                (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264VER))
                            {
                                prVdecEsInfoKeep->pfnVdecGetDispPic(ucEsId, (void*)&rDispInfo);
                                if(!_VDEC_MidSetting(ucEsId, &rDispInfo))
                                {
                                    LOG(3, "VdecMid Fail~\n");
                                }
                            }
                        }
                        i4RetValue = prVdecEsInfoKeep->pfnVdecDecStart(ucMpvId, ucEsId);
                        if(i4RetValue == (INT32)E_VDEC_OK)
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_WAIT_DEC_FINISH;
                        }
                        else if (i4RetValue == (INT32)E_VDEC_RETRIGGER)
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_TRIGGER;
                        }
                        else    //error
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_DROP;
                        }
                    }
                    //combine trigger decode and wait finish in one stage
                    /*else */
                    if (prVdecEsInfoKeep->eDecStatus == VDEC_DEC_WAIT_DEC_FINISH)
                    {
                        LOG(9, "%d, VDEC_DEC_WAIT_DEC_FINISH\n", ucMpvId);
                        i4RetValue = prVdecEsInfoKeep->pfnVdecDecFin(ucMpvId, ucEsId);

                        _VdecTimeProfile(ucEsId);

                        if(i4RetValue == (INT32)E_VDEC_OK)
                        {
                            //for emulation, get output picture and do notification
                            if(prVdecEsInfoKeep->pfnVdecGetDispPic)
                            {
                                prVdecEsInfoKeep->pfnVdecGetDispPic(ucEsId, (void*)&rDispInfo);
                                if(rDispInfo.pvYAddr != NULL)
                                {
                                    //callback, compare golden
                                    if(prVdecInfo->pfErrNotify)
                                    {
                                        prVdecInfo->pfErrNotify((UINT32)VDEC_EVT_DISP_READY, (UINT32)&rDispInfo, 0, 0);
                                    }
                                }
                            }

                            _VdecCheckStatus(ucEsId, TRUE);

                             //update rptr
                            if(prVdecEsInfoKeep->pfnVdecUpdRptr)
                            {
                                prVdecEsInfoKeep->pfnVdecUpdRptr(ucEsId);
                            }

                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                            //reset on each pic
                            if(prVdecEsInfoKeep->pfnVdecSWRst)
                            {
                                if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                                {
                                    _VDEC_LockVld(ucEsId, ucMpvId);
                                }
                                prVdecEsInfoKeep->pfnVdecSWRst(ucMpvId, ucEsId);

                                if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                                {
                                    _VDEC_UnlockVld(ucEsId, ucMpvId);
                                }
                            }

                            _VDEC_DeliverEos(ucEsId);
                            // TODO
                            //_VDEC_UnlockVld(ucEsId, ucMpvId);
                            //VERIFY(x_sema_unlock(prVdecInfo->arVdecVldInfo[ucMpvId].hVldSemaphore) == OSR_OK);
                        }
                        else if(i4RetValue == (INT32) E_VDEC_FAIL)
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_DROP;
                        }
                        else if (i4RetValue == E_VDEC_RETRIGGER)
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_TRIGGER;
                        }
                        else
                        {
                            prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                            _VDEC_DeliverEos(ucEsId);
                             //update rptr
                            if((prVdecEsInfoKeep->pfnVdecUpdRptr) &&
                               (prVdecEsInfoKeep->eCurFMT != VDEC_FMT_MP4))
                            {
                                prVdecEsInfoKeep->pfnVdecUpdRptr(ucEsId);
                            }
                        }
                    }
                    else if(prVdecEsInfoKeep->eDecStatus == VDEC_DEC_DROP)
                    {
                        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
                          fgErrorHappened = TRUE;
                        #endif
                      
                        if(prVdecEsInfoKeep->pfnVdecDecDrop)
                        {
                            prVdecEsInfoKeep->pfnVdecDecDrop(ucEsId);
                        }

                        _VdecCheckStatus(ucEsId, FALSE);

                        //change state
                        prVdecEsInfoKeep->eDecStatus = VDEC_DEC_HEADER_PARSE;
                        //reset on each pic
                        if(prVdecEsInfoKeep->pfnVdecSWRst)
                        {
                            if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                            {
                                _VDEC_LockVld(ucEsId, ucMpvId);
                            }
                            
                            prVdecEsInfoKeep->pfnVdecSWRst(ucMpvId, ucEsId);
        
                            if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
                            {
                                _VDEC_UnlockVld(ucEsId, ucMpvId);
                            }
                        }

                        _VDEC_DeliverEos(ucEsId);

                        //update rptr
                        if((prVdecEsInfoKeep->pfnVdecUpdRptr) &&
                           (prVdecEsInfoKeep->eCurFMT != VDEC_FMT_MP4))
                        {
                            prVdecEsInfoKeep->pfnVdecUpdRptr(ucEsId);
                        }
                        // TODO
                        //_VDEC_UnlockVld(ucEsId, ucMpvId);
                        //VERIFY(x_sema_unlock(prVdecInfo->arVdecVldInfo[ucMpvId].hVldSemaphore) == OSR_OK);
                    }
                 #ifdef CC_VDEC_PRIORITY_ENABLE
                    VDEC_ThreadSchedule(ucEsId);
                 #endif
                }

                break;
            }

            case VDEC_ST_STOP:
                //clear queue, clear stored info, do notification
                //dmx flush Q
                _VDEC_LockFlushMutex(ucEsId);
                if (prVdecEsInfo->fgFlushEsmQ)
                {
                    LOG(1, "ES(%d) _VDECStop Flush\n", ucEsId);
                    _VDEC_FlushEs(ucEsId);
                    prVdecEsInfo->fgFlushEsmQ = FALSE;
                    _VDEC_UnlockFlushSema(ucEsId);
                }
                _VDEC_UnlockFlushMutex(ucEsId);

                if(prVdecEsInfoKeep->ePreviousState != prVdecEsInfoKeep->eCurState)
                {
                    LOG(2,"VDEC[%d] clear info \n",ucEsId);
                    //stop lae decode thread
                    #ifdef CC_DUAL_CORE_DEC
                    if((prVdecEsInfo->ucVDecStatus == REAL_DECODE) || (prVdecEsInfo->ucVDecStatus == UNKOWN_DECODE))
                    {
                        if(prVdecEsInfo->ucDualCoreMode)
                        {
                            _VDEC_StopLae(prVdecEsInfo->ucLaeEsInfoId);
                        }
                        
                        if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
                        {
                            vLAESetFree(ucEsId);
                        }
                    }
                    else if(prVdecEsInfo->ucVDecStatus == LAE_DECODE)
                    {
                        fgNeedStopNfy = FALSE;
                    }
                    #endif
                    // to prevent clean esinfo twice. we may clean the setting
                    // from mw.
                    if(prVdecEsInfoKeep->pfnVdecStop)
                    {
                        prVdecEsInfoKeep->pfnVdecStop(ucEsId);
                    }                                                         
                    _VDEC_FlushEs(ucEsId);
                    //callback to mw after stop process finished, cr: DTV00125552
                    // move down,
                    //h.264 will set EsInfo->fbgId as well, should clear this or
                    //mpeg2 won't create new fbg, cause display not ready
                    _VDEC_LockFlushMutex(ucEsId);

                    if (prVdecEsInfo->fgFlushEsmQ)
                    {
                        _VDEC_FlushEs(ucEsId);
                        prVdecEsInfo->fgFlushEsmQ = FALSE;
                        _VDEC_UnlockFlushSema(ucEsId);
                    }
                    #ifdef CC_DUAL_CORE_DEC
                    if (prVdecEsInfo->ucVDecStatus == REAL_DECODE)
                    {
                        _VDEC_UnlockStopSema(ucEsId);
                        LOG(2, "_VDEC_UnLockStopSema, ucesid %d\n", ucEsId);
                    }
                    #endif
                    if((prVdecInfo->arVdecVldInfo[prVdecEsInfo->u4VldId].u4LockCount > 0) &&
                       (prVdecInfo->arVdecVldInfo[prVdecEsInfo->u4VldId].ucLockEsId == ucEsId))
                    {
                        #if VLD_LOCK_LOG
                        u4VLDLogWr(ucEsId, (UCHAR)prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_FLW_BASE + 6);
                        #endif
                        // Unlock VLD due to stop video decoder.
                        _VDEC_UnlockVld(ucEsId, (UCHAR)prVdecEsInfo->u4VldId);
                    }
                    
                    #ifdef CC_VDEC_RM_SUPPORT
                    VDEC_VLDMngrFreeVld(ucEsId);
                    #else
                    UNUSED(VDEC_PowerOff(ucEsId, prVdecEsInfo->u4VldId));
                    #endif
                    _VDEC_ClearEsInfo(ucEsId);
                    _VDEC_UnlockFlushMutex(ucEsId);

                }

                // DTV00212525,[Pre-Test]Always show 'Retrieving Data' after change channel from specail TS
                // MW will call stop even play is fail. even that we have to notify control done
                if((prVdecEsInfoKeep->u4PreviousStopId != prVdecEsInfoKeep->u4CurStopId)
                   #ifdef CC_DUAL_CORE_DEC
                   && (fgNeedStopNfy)
                   #endif
                   )
                {
                    //callback to mw after stop process finished, cr: DTV00125552
                    MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_STOP_DONE);
                    if (prVdecEsInfoKeep->pfDecNfyFct)
                    {
                        prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_CTRL_DONE,
                            //(UINT32)VID_DEC_CTRL_STOP, 0);
                            1, 0); //cr: DTV00126433
                    }
                    LOG(2,"VDEC[%d] Stop notify done\n",ucEsId);                    
                }
                prVdecEsInfoKeep->u4PreviousStopId = prVdecEsInfoKeep->u4CurStopId;
                prVdecEsInfoKeep->pfnVdecStartPts=NULL;
                // reset
                prVdecEsInfoKeep->fgVPushFBGFromInst = FALSE;
                prVdecEsInfo->eNotifyStatus = VDEC_DEC_DECODE_UNKNOWN;
                prVdecEsInfoKeep->ePreviousState = prVdecEsInfoKeep->eCurState;
                fgNeedStopNfy = TRUE;
                LOG(2,"VDEC[%d] Stop done\n",ucEsId);
                break;
           case VDEC_ST_VIRTUAL_STOP:
                //Please add FlushDPB op here
                _VdecFlushDPB(ucEsId);
                //handle some release op
                if(prVdecEsInfoKeep->pfnVdecStop)
                {
                    prVdecEsInfoKeep->pfnVdecStop(ucEsId);
                }
                
                UNUSED(VDEC_PowerOff(ucEsId, prVdecEsInfo->u4VldId));
                _VDEC_ClearEsInfo(ucEsId);
                prVdecEsInfoKeep->eCurState = VDEC_ST_RESTART;
                break;
            case VDEC_ST_IDLE:
                // will eat pes data
                break;
            default:
                break;
        }

        if(prVdecEsInfo->fgParsingInfo && prVdecEsInfoKeep->pfDecNfyFct && (prVdecEsInfoKeep->rPesInfo.ucMpvId != VDEC_MAX_VLD))
        {
            LOG(2, "VDEC_COND_VPUSH_IO_DONE %d\n", prVdecEsInfo->ucFbgId);
            prVdecEsInfoKeep->pfDecNfyFct(
                prVdecEsInfoKeep->pvDecNfyTag,
                VDEC_COND_VPUSH_IO_DONE,
                0, 0);
        }
    }

    {// finish thread        
        // if all threads are destroy, delete vdec semaphore
        UCHAR ucIdx;
        UCHAR ucAllDestory;
        ucAllDestory = 1;
        for (ucIdx = 0; ucIdx < VDEC_MAX_ES; ucIdx++)
        {        
            prVdecInfo->arVdecEsInfoKeep[ucEsId].fgThreadActive = FALSE;
        }
        for (ucIdx = 0; ucIdx < VDEC_MAX_ES; ucIdx++)
        {
            if (prVdecInfo->arVdecEsInfoKeep[ucIdx].fgThreadActive)
            {
                ucAllDestory = 0;
                ucIdx = VDEC_MAX_ES;
            }
        }
        if (ucAllDestory)
        {
            for (ucIdx = 0; ucIdx < VDEC_MAX_ES; ucIdx++)
            {
                _MPV_VldPower(ucIdx, OFF);
                _MPV_IsrStop();
                _MPV_DeleteDecSema(ucIdx);
           		if(prVdecInfo->arVdecEsInfoKeep[ucIdx].hDataTimer && prVdecInfo->arVdecEsInfoKeep[ucIdx].fgTimerStarted
        		)
	        	{
	        		VERIFY(x_timer_stop(prVdecInfo->arVdecEsInfoKeep[ucIdx].hDataTimer) == OSR_OK);
	           	}
                VERIFY(x_timer_stop(prVdecInfo->arVdecEsInfoKeep[ucIdx].hCodecChkTimer) == OSR_OK);
            }
            for (ucIdx = 0; ucIdx < VDEC_MAX_VLD; ucIdx++)
            {
                VERIFY (x_sema_delete(prVdecInfo->arVdecVldInfo[ucIdx].hVldSemaphore) == OSR_OK);
            }			
			VERIFY (x_sema_delete(prVdecInfo->hVldInfoSemaphore) == OSR_OK);
            prVdecInfo->fgVDecInitiated = FALSE;
        }
        _VDEC_DeleteEsmSema(ucEsId);
    }
    UNUSED(pvArg);
    x_thread_exit();
}

static void _VDEC_IntrudeHandler(UINT32 u4Region, MID_AGENT_ID_T eAgentId, UINT32 u4Addr)
{
    // Handling the intrusion event
    Printf("System halted\n");
    BSP_HaltSystem();
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL _VDEC_MidSetting(UCHAR ucEsId, VDEC_DISP_PIC_INFO_T* prDispInfo)
{
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UINT32 u4WorkingBuffer, u4WorkingSize, u4YSize;
    MID_AGENT_ID_T ePPAgentId;
    BOOL fgRet = TRUE;

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((prVdecInfo == NULL) || (prVdecEsInfo == NULL) ||
        (prVdecEsInfoKeep == NULL) ||
        (prDispInfo->pvYAddr == NULL) || (prDispInfo->pvCAddr == NULL))
    {
        return FALSE;
    }
    if(!prVdecEsInfoKeep->fgVdecMidEnable)
    {
        return TRUE;
    }

    #if defined(CC_MT5360)
    ePPAgentId = MID_AGENT_VDEC_MISC;
    #else
    ePPAgentId = MID_AGENT_JPEG;
    #endif

    switch(prVdecEsInfoKeep->eCurFMT)
    {
        case VDEC_FMT_H264VER:
        case VDEC_FMT_H264:
            if(prDispInfo->pvMVAddr == NULL)
            {
                return FALSE;
            }
            u4YSize = (UINT32)prDispInfo->pvCAddr - (UINT32)prDispInfo->pvYAddr;
            #if defined(VDEC_IS_POST_MT5365)
            MID_ResetRegion(0);
            MID_ResetRegion(1);
            fgRet &= MID_SetRegionEx2(0, MID_AGENT_CPU,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvMVAddr + (UINT32)(u4YSize >> 2),
                MID_FLAG_INVERSE,
                ~(1<<ePPAgentId));
            #else
            fgRet &= MID_SetRegionEx(0, ePPAgentId,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvMVAddr + (UINT32)(u4YSize >> 2),
                MID_FLAG_INVERSE);
            #endif
            fgRet &= MID_EnableRegionProtect(0);
            fgRet &= MID_SetRegion(1, MID_AGENT_CPU,
                (UINT32)prDispInfo->pvCAddr + (UINT32)(u4YSize>>1),
                (UINT32)prDispInfo->pvMVAddr);
            fgRet &= MID_EnableRegionProtect(1);
            break;

        case VDEC_FMT_MPV:
            #if defined(VDEC_IS_POST_MT5365)
            MID_ResetRegion(0);
            MID_ResetRegion(1);
            fgRet &= MID_SetRegionEx2(0, MID_AGENT_CPU,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvCAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE,
                ~(1<<MID_AGENT_MPEG1));
            fgRet &= MID_EnableRegionProtect(0);
            fgRet &= MID_SetRegionEx2(1, MID_AGENT_CPU,
                (UINT32)prDispInfo->pvYDbkAddr,
                (UINT32)prDispInfo->pvCDbkAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE,
                ~(1<<ePPAgentId));
            fgRet &= MID_EnableRegionProtect(1);
            #else
            fgRet &= MID_SetRegionEx(0, MID_AGENT_MPEG1,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvCAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE);
            fgRet &= MID_EnableRegionProtect(0);
            fgRet &= MID_SetRegionEx(1, ePPAgentId,
                (UINT32)prDispInfo->pvYDbkAddr,
                (UINT32)prDispInfo->pvCDbkAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE);
            fgRet &= MID_EnableRegionProtect(1);
            #endif
            break;

        case VDEC_FMT_WMV:
        case VDEC_FMT_MP4:
        case VDEC_FMT_RV:  // !!! megaa 20090516
        case VDEC_FMT_AVS:
            #if defined(VDEC_IS_POST_MT5365)
            MID_ResetRegion(0);
            MID_ResetRegion(1);
            //frame buffer
            fgRet &= MID_SetRegionEx2(0, MID_AGENT_CPU,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvCAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE,
                ~(1<<MID_AGENT_MPEG1));
            fgRet &= MID_EnableRegionProtect(0);
            //ac, dc buffer
            FBM_GetWorkingBuffer(prVdecEsInfo->ucFbgId, &u4WorkingBuffer, &u4WorkingSize);
            fgRet &= MID_SetRegionEx2(1, MID_AGENT_CPU, u4WorkingBuffer,
                (u4WorkingBuffer + u4WorkingSize), MID_FLAG_INVERSE,
                ~(1<<ePPAgentId));
            fgRet &= MID_EnableRegionProtect(1);
            #else
            //frame buffer
            fgRet &= MID_SetRegionEx(0, MID_AGENT_MPEG1,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvCAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE);
            fgRet &= MID_EnableRegionProtect(0);
            //ac, dc buffer
            FBM_GetWorkingBuffer(prVdecEsInfo->ucFbgId, &u4WorkingBuffer, &u4WorkingSize);
            fgRet &= MID_SetRegionEx(1, ePPAgentId, u4WorkingBuffer,
                (u4WorkingBuffer + u4WorkingSize), MID_FLAG_INVERSE);
            fgRet &= MID_EnableRegionProtect(1);
            #endif
            break;

        case VDEC_FMT_VP6:
        case VDEC_FMT_VP8:
        {
            UINT32 u4Region;
            UINT32 u4AgentList;

            MID_RegisterHandler(_VDEC_IntrudeHandler);

            u4Region = 0;
            MID_ResetRegion(u4Region);
            u4AgentList = ~(((UINT32)1 << MID_AGENT_MPEG1)|((UINT32)1 << MID_AGENT_JPEG));
            MID_ResetRegion(u4Region);
            fgRet &= MID_SetRegionEx2(
                u4Region,
                MID_AGENT_CPU,
                (UINT32)prDispInfo->pvYAddr,
                (UINT32)prDispInfo->pvCAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                MID_FLAG_INVERSE,
                u4AgentList);
            fgRet &= MID_EnableRegionProtect(u4Region);

            if ((prDispInfo->pvYDbkAddr != 0) && (prDispInfo->pvCDbkAddr!= 0))
            {
                u4Region = 1;
                MID_ResetRegion(u4Region);
                u4AgentList = ~((UINT32)1 << ePPAgentId);
                MID_ResetRegion(u4Region);
                fgRet &= MID_SetRegionEx2(
                    u4Region,
                    MID_AGENT_CPU,
                    (UINT32)prDispInfo->pvYDbkAddr,
                    (UINT32)prDispInfo->pvCDbkAddr + (UINT32)(prDispInfo->u4YSize >> 1),
                    MID_FLAG_INVERSE,
                    u4AgentList);
                fgRet &= MID_EnableRegionProtect(u4Region);
            }
        }
            break;

        default:
            return FALSE;
            //break;
    }

    return fgRet;
}


void VDEC_FbmMidTest(UCHAR ucEsId, UINT8 u1Mode, BOOL fgDBKDisable)
{
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfoKeep)
    {
        prVdecEsInfoKeep->u1FbmMidMode = u1Mode;
        prVdecEsInfoKeep->fgMidDBKDisable = fgDBKDisable;
    }
}


void VDEC_AVSyncTest(UCHAR ucEsId, UINT8 u1Mode)
{
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfoKeep)
    {
        prVdecEsInfoKeep->u1AvSyncTestMode = u1Mode;
        _fgWaitFbgChg = TRUE;
    }
}


void VDEC_FbgChgHdlr(UCHAR ucEsId, UCHAR ucFbgId, UINT32 u4FBStart, UINT32 u4FBEnd)
{
    UINT8 i;
    DMX_PID_T rPid;
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    MID_AGENT_ID_T eAgentId, ePPAgentId, eDmxAgentId = MID_AGENT_DEMUX;
    FBM_POOL_T* prFbmPoolDMX;
    UINT32 u4WorkingBuffer, u4WorkingSize;
    ENUM_VDEC_FMT_T eVDecFmt;
    BOOL fgRet = TRUE;

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((prVdecInfo == NULL) || (prVdecEsInfo == NULL) ||
        (prVdecEsInfoKeep == NULL))
    {
        return;
    }

    if(ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        if(prVdecEsInfoKeep->u1FbmMidMode > 0)
        {
            #if defined(CC_MT5360)
            ePPAgentId = MID_AGENT_VDEC_MISC;
            #else
            ePPAgentId = MID_AGENT_JPEG;
            #endif

            eVDecFmt = prVdecEsInfoKeep->eCurFMT;
            FBM_GetWorkingBuffer(ucFbgId, &u4WorkingBuffer, &u4WorkingSize);

            if((eVDecFmt == VDEC_FMT_H264) ||
                (eVDecFmt == VDEC_FMT_H264VER))
            {
                eAgentId = ePPAgentId;
            }
            else
            {
                eAgentId = MID_AGENT_MPEG1;
                if((eVDecFmt == VDEC_FMT_WMV) || (eVDecFmt == VDEC_FMT_MP4) || (eVDecFmt == VDEC_FMT_RV))  // !!! megaa 20090516
                {
                    u4FBEnd = u4WorkingBuffer;
                }
            }

            //protect mode
            if((prVdecEsInfoKeep->u1FbmMidMode == 2) ||
                (prVdecEsInfoKeep->u1FbmMidMode == 3))    //fbm corrupt prevention
            {
                //region 0, for dmx
                if(prVdecEsInfoKeep->u1FbmMidMode == 2)
                {
#ifdef ENABLE_MULTIMEDIA
                    if(prVdecEsInfo->fgMMPlayback)
                    {
                        #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
                        if((prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS) ||
                           (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_192) ||
                           (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_ZERO_192))
                           // dmx use demux_pvr to write when source is pvr or time-shift.
                           //(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_ENCRYPT_192) ||
                           //(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT))
                        {
                            // 5365 dmx use DDI to move in TS file
                            // for other format, use MID_AGENT_DMX
                            eDmxAgentId = MID_AGENT_DDI;
                        }
                        #else
                        eDmxAgentId = MID_AGENT_DDI;
                        #endif
                    }
#endif
                    prFbmPoolDMX = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
                    if((prFbmPoolDMX == NULL) || (prFbmPoolDMX->u4Addr == 0))
                    {
                        return;
                    }
                    fgRet &= MID_SetRegion(0, eDmxAgentId, prFbmPoolDMX->u4Addr,
                        prFbmPoolDMX->u4Addr + prFbmPoolDMX->u4Size);
                    fgRet &= MID_EnableRegionProtect(0);
                }
                //region 1, fbm
                if((eVDecFmt != VDEC_FMT_MPV) || prVdecEsInfoKeep->fgMidDBKDisable)
                {
                    fgRet &= MID_SetRegion(1, eAgentId, u4FBStart, u4FBEnd);
                    fgRet &= MID_EnableRegionProtect(1);
                }
                //region 2, ac, dc buffer
                if((eVDecFmt == VDEC_FMT_WMV) || (eVDecFmt == VDEC_FMT_MP4) || (eVDecFmt == VDEC_FMT_MP4))  // !!! megaa 20090516
                {
                    fgRet &= MID_SetRegion(2, ePPAgentId, u4WorkingBuffer,
                        u4WorkingBuffer + u4WorkingSize);
                    fgRet &= MID_EnableRegionProtect(2);
                }
            }
            if(fgRet)
            {
                LOG(2, "Vdec Memory Protect in FBM done~\n");
            }
            else
            {
                LOG(2, "Vdec Memory Protect in FBM fail !\n");
            }
        }
    }

    //=== av sync free pid mode
    if(prVdecEsInfoKeep)
    {
        // free pid
        x_memset((void*)&rPid, 0, sizeof(rPid));
        if((prVdecEsInfoKeep->u1AvSyncTestMode & 0x4) > 0)
        {
            for (i = 0; i < DMX_NUM_PID_INDEX; i++)
            {
                if (!DMX_GetPid(i, (UINT32)(DMX_PID_FLAG_PCR | DMX_PID_FLAG_VALID), &rPid))
                {
                    ASSERT(0);
                }

                if (rPid.fgEnable)
                {
                    if ((rPid.ePcrMode == DMX_PCR_MODE_OLD) || (rPid.ePcrMode == DMX_PCR_MODE_NEW))
                    {
                        DMX_FreePid(i);
                        break;
                    }
                }
            }
        }
        prVdecEsInfoKeep->u4AvSyncCnt = 0x1064;
        _fgWaitFbgChg = FALSE;
    }
}


void VDEC_FbgReleaseHdlr(UCHAR ucEsId)
{
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((prVdecEsInfoKeep != NULL) && (prVdecEsInfoKeep->u1FbmMidMode > 0))
    {
        UNUSED(MID_Reset());
    }
}


void VDEC_GenPTS(UCHAR ucEsId, UCHAR ucFbgId, UCHAR ucFbId)
{
    FBM_PIC_HDR_T* prFbmPicHdr;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    BOOL fgChgPTS = FALSE;
    UINT32 u4PrevCnt = 0;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);

    if((prVdecEsInfoKeep != NULL) && (prFbmPicHdr != NULL))
    {
        if((prVdecEsInfoKeep->u1AvSyncTestMode & 0x3) == 1)   //only apply fake pts on 1st frame
        {
            if(!_fgWaitFbgChg)
            {
                fgChgPTS = TRUE;
                _fgWaitFbgChg = TRUE;
            }
        }
        else if((prVdecEsInfoKeep->u1AvSyncTestMode & 0x3) > 0)   // 2 or 3, apply fake pts randomly
        {
            fgChgPTS = TRUE;
        }
        //Cnt == bit[15:12], 0:normal/1:fake PTS, bit[11:8], 0:pts+/1:pts-, bit[7:0] counter
        if(fgChgPTS)
        {
            if((prVdecEsInfoKeep->u4AvSyncCnt & 0xF000) > 0)    // fake pts, else original pts
            {
                if((prVdecEsInfoKeep->u4AvSyncCnt & 0xF00) > 0)
                {
                    prFbmPicHdr->u4PTS -= (90000 * 5);  // set 5 sec precede
                    prFbmPicHdr->u8PTS -= (VDEC_U8_SECOND_CLOCK * 5);  // set 5 sec precede
                }
                else
                {
                    prFbmPicHdr->u4PTS += (90000 * 5);  // set 5 sec behind
                    prFbmPicHdr->u8PTS += (VDEC_U8_SECOND_CLOCK * 5);  // set 5 sec behind
                }
            }
            prVdecEsInfoKeep->u4AvSyncCnt --;
            if((prVdecEsInfoKeep->u4AvSyncCnt & 0xFF) == 0) //switch, 0x1000->0x0100->0x1100->0x0000->0x1000
            {
                u4PrevCnt = prVdecEsInfoKeep->u4AvSyncCnt;
                prVdecEsInfoKeep->u4AvSyncCnt = 0;
                if((u4PrevCnt & 0xF000) == 0)
                {
                    prVdecEsInfoKeep->u4AvSyncCnt |= 0x1000;
                    prVdecEsInfoKeep->u4AvSyncCnt |= (u4PrevCnt & 0xF00);
                }
                else if((u4PrevCnt & 0xF00) == 0)
                {
                    prVdecEsInfoKeep->u4AvSyncCnt |= 0x100;
                }
                prVdecEsInfoKeep->u4AvSyncCnt |= 0x64;
            }
        }
    }

}


#ifdef VDEC_TIME_PROFILE
static void _VDEC_CalPicSize(UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (!prVdecEsInfo->fgCalEachPicSize)
    {
        return;
    }

    if (prVdecEsInfo->u4VldReadPtr)
    {
        if (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr > prVdecEsInfo->u4VldReadPtr)
        {
            prVdecEsInfo->u4CurrPicSize = (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr - prVdecEsInfo->u4VldReadPtr);
        }
        else
        {
            prVdecEsInfo->u4CurrPicSize = ((prVdecEsInfo->u4FifoEnd - prVdecEsInfo->u4VldReadPtr) +
                                           (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr - prVdecEsInfoKeep->rPesInfo.u4FifoStart));;
        }
        prVdecEsInfo->u4StatisticPicSize += prVdecEsInfo->u4CurrPicSize;
    }

    if (prVdecEsInfoKeep->rPesInfo.fgEos)
    {
        prVdecEsInfo->fgCalEachPicSize = FALSE;
    }

}
#endif

#ifdef CC_MAPLE_CUST_DRV
//Calculate bitrate
static void _VDEC_CalEsDataSize(UCHAR ucEsId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (!prVdecEsInfo->fgCalEachPicSize)
    {
        prVdecEsInfo->rBitRateInfo.fgValidData = FALSE;
        return;
    }

    if (prVdecEsInfo->u4VldReadPtr)
    {
        if (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr > prVdecEsInfo->u4VldReadPtr)
        {
            prVdecEsInfo->u4CurrPicSize = (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr - prVdecEsInfo->u4VldReadPtr);
        }
        else
        {
            prVdecEsInfo->u4CurrPicSize = ((prVdecEsInfo->u4FifoEnd - prVdecEsInfo->u4VldReadPtr) +
                                           (prVdecEsInfoKeep->rPesInfo.u4VldReadPtr - prVdecEsInfoKeep->rPesInfo.u4FifoStart));;
        }
        //prVdecEsInfo->u4StatisticPicSize += prVdecEsInfo->u4CurrPicSize;
        prVdecEsInfo->rBitRateInfo.fgValidData = TRUE;
    }

    if (prVdecEsInfoKeep->rPesInfo.fgEos)
    {
        prVdecEsInfo->fgCalEachPicSize = FALSE;
    }
}
#endif
#ifdef VDEC_VENC_PATH

VOID _VDEC_SetVencFrmSz(UCHAR ucEsId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
{
    if (VDEC_MAX_ES > ucEsId)
    {
        _arVdec2VencFlow[ucEsId].u4Width  = u4Width;
        _arVdec2VencFlow[ucEsId].u4Pitch  = u4Pitch;
        _arVdec2VencFlow[ucEsId].u4Height = u4Height;
        LOG(3, "%s: %dx%d\n", __FUNCTION__, u4Width, u4Height);
    }
    else
    {
        LOG(0, "%s: invalid id %d\n", __FUNCTION__, ucEsId);
    }
}

VOID _VDEC_Set2VencFlow(UCHAR ucEsId, BOOL fgOn)
{
    if (VDEC_MAX_ES > ucEsId)
    {
        _arVdec2VencFlow[ucEsId].fgOn  = fgOn;
#ifdef VENC_FRAME_CTRL_BY_DRV
        if (_arVdec2VencFlow[ucEsId].ptFilter)
        {
            _VENC_Filter_Reset(_arVdec2VencFlow[ucEsId].ptFilter);
        }
#endif

    }
    else
        {
        LOG(0, "%s: invalid id %d\n", __FUNCTION__, ucEsId);
        }
}

VOID _VDEC_SetVencHandle(UCHAR ucEsId, VOID *pvArg1, VOID *pvArg2, VOID *pvArg3)
{
    if (VDEC_MAX_ES > ucEsId)
    {
        _arVdec2VencFlow[ucEsId].hVEnc = (VENC_HANDLE)pvArg1;
        _arVdec2VencFlow[ucEsId].ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)pvArg2;
#ifdef VENC_FRAME_CTRL_BY_DRV
        _arVdec2VencFlow[ucEsId].ptFilter = (VENC_FILTER_T *)pvArg3;
#endif
    }
    else
    {
        LOG(0, "%s: invalid id %d\n", __FUNCTION__, ucEsId);
    }

    UNUSED(pvArg3);
}


BOOL _VDEC_fg2VencFlowOn(UCHAR ucEsId)
{
    return (VDEC_MAX_ES > ucEsId)? _arVdec2VencFlow[ucEsId].fgOn : FALSE;
}

BOOL _VDEC_PrepareVEncSourse(UCHAR ucEsId, VDEC_VENC_SRC_INFO_T *prSrcInfo)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VENC_FRM_T    rFrm;
    VENC_IN_PIC_T rIn;
    UINT32 u4Width, u4Height, u4HalfHeight, u4Pitch, u4Pts, u4Ret;

    if (ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "%s: invalid esid - %d\n", __FUNCTION__, ucEsId);
        return FALSE;
    }

    if (prSrcInfo == NULL)
    {
        LOG(1, "%s: prSrcInfo is NULL\n", __FUNCTION__);
        return FALSE;
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(!prVdecEsInfoKeep)
    {
        LOG(3, "_VDEC_PrepareVEncSourse prVdecEsInfoKeep null\n");
        return FALSE;
    }

    // Prepare inform VENC data
    u4Width = prSrcInfo->u4Width;
    u4Height = prSrcInfo->u4Height;
    u4HalfHeight = prSrcInfo->u4Height >> 1;
    u4Pitch = prSrcInfo->u4Pitch;
    u4Pts        = prSrcInfo->u4Pts;
    ASSERT((prSrcInfo->u4YAddr & 3) == 0);
    ASSERT((prSrcInfo->u4CAddr & 3) == 0);
    ASSERT((u4Width & 15) == 0);
    ASSERT((u4Pitch & 15) == 0);

#ifdef VENC_FRAME_CTRL_BY_DRV
    //LOG(3, "receive fram, and pts is %d\n", u4Pts);
    if (_arVdec2VencFlow[ucEsId].ptFilter && _VENC_Filter_Do(_arVdec2VencFlow[ucEsId].ptFilter, u4Pts) == FALSE)
    {
        LOG(3, "drop frame by filter, pts is %d\n", u4Pts);
        return FALSE;
    }
#endif

    // copy from VDEC frame buffer to VENC frame buffer
    u4Ret = _BUFMGR_OBJ_Get(_arVdec2VencFlow[ucEsId].ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
    if (VENC_BUFMGR_OK != u4Ret)
    {
        LOG(1, "%s: _BUFMGR_OBJ_Get fail(%d)\n", __FUNCTION__, u4Ret);
        return FALSE;
    }

    ASSERT(_arVdec2VencFlow[ucEsId].u4Width);
    ASSERT(_arVdec2VencFlow[ucEsId].u4Height);
    ASSERT(_arVdec2VencFlow[ucEsId].u4Pitch);

#if 0
    if ((_arVdec2VencFlow[ucEsId].u4Width != u4Width)
        || (_arVdec2VencFlow[ucEsId].u4Height != u4Height)
        || (_arVdec2VencFlow[ucEsId].u4Pitch != u4Pitch))
#endif
    {
        RZ_VDO_SCL_PARAM_SET_T rImgrzScaleParam;
        x_memset(&rImgrzScaleParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));
        if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MJPEG) ||
           (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_RAW))
        {
            rImgrzScaleParam.u4IsRsIn = 1;
            rImgrzScaleParam.u4SrcSwap = 0;
            rImgrzScaleParam.u4OutSwap = 0;
        }
        else
        {
            rImgrzScaleParam.u4SrcSwap = 6;
            rImgrzScaleParam.u4OutSwap = 0;
        }
        rImgrzScaleParam.u4InMode = E_RZ_VDO_OUTMD_420;
        rImgrzScaleParam.u4YSrcBase = prSrcInfo->u4YAddr;
        rImgrzScaleParam.u4YSrcBufLen = u4Pitch;
        rImgrzScaleParam.u4YSrcHOffset = 0;
        rImgrzScaleParam.u4YSrcVOffset = 0;
        rImgrzScaleParam.u4YSrcW = u4Width;
        rImgrzScaleParam.u4YSrcH = u4Height;
        rImgrzScaleParam.u4CSrcBase = prSrcInfo->u4CAddr;
        rImgrzScaleParam.u4CSrcHOffset = 0;
        rImgrzScaleParam.u4CSrcVOffset = 0;
        rImgrzScaleParam.u4CSrcW       = u4Width >> 1;
        rImgrzScaleParam.u4CSrcH = u4HalfHeight;

        rImgrzScaleParam.u4IsRsOut = 1;
        rImgrzScaleParam.u4OutMode = E_RZ_VDO_OUTMD_420;       // 420|422|444 output
        rImgrzScaleParam.u4IsVdo2Osd = 0;     // output in osd format ?
        rImgrzScaleParam.u4YTgBase     = rFrm.u4YAddr;
        rImgrzScaleParam.u4YTgCM = E_RZ_VDO_OUTMD_420;
        rImgrzScaleParam.u4YTgBufLen   = _arVdec2VencFlow[ucEsId].u4Pitch;
        rImgrzScaleParam.u4YTgHOffset = 0;
        rImgrzScaleParam.u4YTgVOffset = 0;
        rImgrzScaleParam.u4YTgW        = _arVdec2VencFlow[ucEsId].u4Width;
        rImgrzScaleParam.u4YTgH        = _arVdec2VencFlow[ucEsId].u4Height;
        rImgrzScaleParam.u4CTgBase     = rFrm.u4UVAddr;
        rImgrzScaleParam.u4CTgW        = rImgrzScaleParam.u4YTgW >> 1;
        rImgrzScaleParam.u4CTgH        = rImgrzScaleParam.u4YTgH >> 1;

        IMGRZ_Lock_Ex(E_FIRST_RESIZER);
        IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
        IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOMD);
        IMGRZ_Scale_Ex(E_FIRST_RESIZER, (void *)(&rImgrzScaleParam));
        IMGRZ_Flush_Ex(E_FIRST_RESIZER);
        IMGRZ_Wait_Ex(E_FIRST_RESIZER);
        IMGRZ_Unlock_Ex(E_FIRST_RESIZER);
    }
#if 0
    else
    {
        GFX_Lock();
        GFX_SetSrc((UINT8 *)prSrcInfo->u4YAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_SetDst((UINT8 *)rFrm.u4YAddr, (UINT32)CM_RGB_CLUT8, u4Width);
        GFX_BitBlt(0, 0, 0, 0, u4Width, u4Height);
        GFX_SetSrc((UINT8 *)prSrcInfo->u4CAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_SetDst((UINT8 *)rFrm.u4UVAddr, (UINT32)CM_RGB_CLUT8, u4Width);
        GFX_BitBlt(0, 0, 0, 0, u4Width, u4HalfHeight);
        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
    }
#endif

    x_memset(&rIn, 0, sizeof(rIn));
    rIn.u4YAddr = rFrm.u4YAddr;
    rIn.u4CAddr = rFrm.u4UVAddr;
    rIn.u4Pts   = u4Pts;

    if (VENC_OK != VENC_EncFrameAsync(_arVdec2VencFlow[ucEsId].hVEnc, &rIn, NULL))
    {
        LOG(1, "%s: enc frame fail\n", __FUNCTION__);
        return FALSE;
    }
    else
    {
#ifdef VENC_FRAME_CTRL_BY_DRV
        if (_arVdec2VencFlow[ucEsId].ptFilter)
    {
            _VENC_Filter_Update(_arVdec2VencFlow[ucEsId].ptFilter);
    }
#endif
    }

    return TRUE;
}
#endif // VDEC_VENC_PATH


#ifdef CC_REALD_3D_SUPPORT
VOID _VDEC_TDTVModeChangeCB(void *pvArgv)
{
    UCHAR ucEsId = ES0;//(UCHAR)*(UINT32*)pvArgv;
    UINT8 u1Force2D;
    UINT16 u2InFmt, u2OutFmt;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_PES_INFO_T rPesInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    E_TDTV_DECODER_INPUT eDetectedMode;
#ifdef CC_FBM_FULL_3D_SUPPORT
    FBM_SEQ_HDR_T* prFbmSeqHdr;
#endif

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if(!prVdecEsInfo)
    {
        return;
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VERIFY (x_sema_lock(prVdecEsInfoKeep->hVdec3DModeChange, X_SEMA_OPTION_WAIT) == OSR_OK);

#ifdef CC_FBM_FULL_3D_SUPPORT
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
    if(!prFbmSeqHdr)
    {
        return;
    }
    // full resolution check, already check max W: 3840, max H:2160 before create fbg
    if(!VDP_GetB2R2DSupport(prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize))
    {
        //set detected type as FS to TDTV, and force b2r doing SBS->FS
        eDetectedMode = prFbmSeqHdr->fgProgressiveSeq ? E_TDTV_DECODER_INPUT_FP_P : E_TDTV_DECODER_INPUT_FP_I;
        prVdecEsInfo->fgB2rResLimitation = TRUE;
        LOG(2, "Res Limitation %d x %d, force detected mode as FS\n", prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize);
        ASSERT(eDetectedMode != prVdecEsInfo->eDetectedMode);   //full resolution FS case, not support
    }
    else
#endif
    {
        prVdecEsInfo->fgB2rResLimitation = FALSE;
        eDetectedMode = prVdecEsInfo->eDetectedMode;
    }
    if (VDEC_3D_RESOLUTION_FULL == prVdecEsInfo->e3DResType)
    {
        eDetectedMode |= CONTENT_FULL_BIT;
    }

    /*eDetectedMode  = (prVdecEsInfo->e3DResType == VDEC_3D_RESOLUTION_FULL) ?             //mark for build warning
        (eDetectedMode | CONTENT_FULL_BIT) : eDetectedMode;*/ 

#ifdef CC_B2R_SENSIO_CONV
    if(prVdecEsInfo->fgSensio3D)
    {
        eDetectedMode=E_TDTV_DECODER_INPUT_SS_P;
        LOG(1,"=========fgSensio3D Set SS_P========\n");
    }
#endif
    prVdecEsInfo->u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
    switch(prVdecEsInfo->e3DForce2DType)
    {
        case VDEC_3D_FORCE2D_AUTO:
            vDrvTDTVSetMMFix322Region(E_TDTV_UI_3D_2_2D_OFF);
            break;
        case VDEC_3D_FORCE2D_L_VIEW:
            vDrvTDTVSetMMFix322Region(E_TDTV_UI_3D_2_2D_L);
            break;
        case VDEC_3D_FORCE2D_R_VIEW:
            vDrvTDTVSetMMFix322Region(E_TDTV_UI_3D_2_2D_R);
            break;
    }

    u2InFmt = prVdecEsInfo->u43DInOut & 0xFFFF;
    u2OutFmt = (prVdecEsInfo->u43DInOut >> 16) & 0xFFFF;
    switch(u2OutFmt)
    {
        case E_TDTV_DECODER_OUTPUT_NATIVE:
            prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_BYPASS;
            break;
        case E_TDTV_DECODER_OUTPUT_3D_FS:
            switch(u2InFmt)
            {
                 case E_TDTV_DECODER_INPUT_TB_I:
                 case E_TDTV_DECODER_INPUT_TB_P:
                    prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_FORCE_3D_TB;
                    break;
                 case E_TDTV_DECODER_INPUT_RD_I:
                 case E_TDTV_DECODER_INPUT_RD_P:
                    //prVdecEsInfo->fgIsRealD3DFmt = TRUE;
                 case E_TDTV_DECODER_INPUT_SBS_I:
                 case E_TDTV_DECODER_INPUT_SBS_P:
                 case E_TDTV_DECODER_INPUT_SS_I:
                 case E_TDTV_DECODER_INPUT_SS_P:
                    prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_FORCE_3D_LR;
                    break;
                default:
                    prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_OFF;
                    break;
            }
            #ifdef CC_FBM_FULL_3D_SUPPORT
            if(prVdecEsInfo->fgB2rResLimitation) //b2r limitation case
            {
                prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_FORCE_3D_LR;
                LOG(2, "Force b2r doing SBS(L first)->FS\n", prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize);
            }
            #endif
            break;
        case E_TDTV_DECODER_OUTPUT_3D_2_2D:
            switch(u2InFmt)
            {
                 case E_TDTV_DECODER_INPUT_TB_I:
                 case E_TDTV_DECODER_INPUT_TB_P:
                    prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_FORCE_2D_TB;
                    break;
                 case E_TDTV_DECODER_INPUT_RD_I:
                 case E_TDTV_DECODER_INPUT_RD_P:
                    //prVdecEsInfo->fgIsRealD3DFmt = TRUE;
                 case E_TDTV_DECODER_INPUT_SBS_I:
                 case E_TDTV_DECODER_INPUT_SBS_P:
                 case E_TDTV_DECODER_INPUT_SS_I:
                 case E_TDTV_DECODER_INPUT_SS_P:
                    prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_FORCE_2D_LR;
                    break;
                default:
                    prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_FORCE_2D;
                    break;
            }
            vTV3DGetForce2D(0, &u1Force2D, &prVdecEsInfo->u1IsForceLViewNtfy);
            break;
        default:
            prVdecEsInfo->u13DUsrCtrlModeNtfy = VDEC_3D_CTRL_OFF;   //follow original content
            break;
    }

    _VDEC_SendCmd(ucEsId, (UINT32)VDEC_CMD_3D_MODE_CHANGE); //ucEsId should be 0

    x_memset((void*)&rPesInfo, 0, sizeof(rPesInfo));
    rPesInfo.ucEsId = ucEsId;
    rPesInfo.ucMpvId = (UCHAR)VDEC_MAX_VLD;
    VERIFY(VDEC_SendEs(&rPesInfo) == 1);

    UNUSED(u1Force2D);

    VERIFY (x_sema_unlock(prVdecEsInfoKeep->hVdec3DModeChange) == OSR_OK);
}


//#define VDEC_3D_USE_ARGB_TO_DETECT //old flow: use GFX HW api to convert color mode to ARGB, then process realD ..

//#The ARGB <--> YCbCr   Formula use blow.
//  HD: 
//Y  =  0.183*r + 0.614*g + 0.062*b + 16;
//Cb = -0.101*r - 0.338*g + 0.439*b + 128;
//Cb =  0.439*r - 0.399*g - 0.040*b + 128;
//R  = 1.164*(Y-16) + 1.793*(Cr-128);
//G  = 1.164*(Y-16) - 0.534*(Cr-128) - 0.213(Cb-128);
//B  = 1.164*(Y-16) + 2.115*(Cb-128);

// SD: not use yet, as RealD only support HD & FullHD.
//Y  =  0.257*r + 0.504*g + 0.098*b + 16;
//Cb = -0.148*r - 0.291*g + 0.439*b + 128;
//Cb =  0.439*r - 0.368*g - 0.071*b + 128;
//R  = 1.164*(Y-16) + 1.596*(Cr-128);
//G  = 1.164*(Y-16) - 0.813*(Cr-128) - 0.391(Cb-128);
//B  = 1.164*(Y-16) + 2.018*(Cb-128);
                  
//support RealD 3D tag detection 
VOID _VDEC_Check2D3DSwitch(UCHAR ucEsId, UCHAR ucFbId)
{
    UINT32 u4UnqualifiedCnt = 0;
    UINT8 u1QuincunxCnt = 0, u13DType = 0;
    BOOL bIsCur2D, bIs3DQualify = FALSE;
    VDEC_DISP_PIC_INFO_T rDispInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    FBM_SEQ_HDR_T * prFbmSeqHdr;
    E_TDTV_DECODER_INPUT eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
	VDEC_TO_TZ_DISP_PIC_INFO_T rTzDispInfo;
    #else
    UINT32* pu4DecData;
    UINT32 u4Offset;
    UINT32 u4WIdx, u4BlkSz;
    UINT32* pu4Dst;
    static UINT32* pu4TmpBufY = NULL;
    static UINT32* pu4TmpBufC = NULL;
    //UINT8* pu1DstY = NULL;
    UINT8* pu1DstC = NULL;
    UINT32 u4PixerIdx = 0;
    INT8  i1Cb=0, i1Cr=0 ;
	UINT8 u1BlockIdx, u1PixIdx;
    BOOL bIsFHD = TRUE, bChkFilterType = FALSE;
    
#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
    UINT32 u4MapVirAddr = 0;
#endif
	#endif
    //HAL_TIME_T rTimeS, rTimeE, rTimeDt;
    //HAL_GetTime(&rTimeS);

    ASSERT(ucEsId < MPV_MAX_ES);

    if(ucEsId > 0)  //b2r only triggered by main path setting
    {
        return;
    }
    if(ucFbId == 0xFF)
    {
        LOG(2, "3D check FbId = 0xFF\n");
        return;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if((!prVdecEsInfo) || (!prVdecEsInfoKeep))
    {
        return;
    }
    if (prVdecEsInfo->fgRenderPicCnt) // FBM_ChkFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_THUMBNAIL_MODE))
    {
        // Thumbnail mode not need to check 2D3D. 
        return;
    }
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
    if(!prFbmSeqHdr)
    {
        return;
    }
    x_memset((void*)&rDispInfo, 0, sizeof(rDispInfo));
    bIsCur2D = !prVdecEsInfo->fgIsRealD3DFmt;
    	
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
	x_memset((void*)&rTzDispInfo, 0, sizeof(rTzDispInfo));
	FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, ucFbId, (UINT32*)&rTzDispInfo.pvYAddr, (UINT32*)&rTzDispInfo.pvCAddr);
	rTzDispInfo.u4W = (UINT32)prFbmSeqHdr->u2LineSize;
	rTzDispInfo.u4W_Cmp = (UINT32)prFbmSeqHdr->u2HSize;
	rTzDispInfo.u4H = ((UINT32)prFbmSeqHdr->u2VSize + 0xF) & (~0xF);
	rTzDispInfo.u4H_Cmp = ((UINT32)prFbmSeqHdr->u2VSize + 0xF) & (~0xF);
	rTzDispInfo.bIsCur2D = bIsCur2D;
#else
	if(pu4TmpBufY == NULL)
    {
        pu4TmpBufY = (UINT32*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(1920, 1024));
        ASSERT(pu4TmpBufY);
    }
    if(pu4TmpBufC == NULL)
    {
        pu4TmpBufC = (UINT32*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(1920, 1024));
        ASSERT(pu4TmpBufC);
    }
	FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, ucFbId, (UINT32*)&rDispInfo.pvYAddr, (UINT32*)&rDispInfo.pvCAddr);
	rDispInfo.u4W = (UINT32)prFbmSeqHdr->u2LineSize;
    rDispInfo.u4W_Cmp = (UINT32)prFbmSeqHdr->u2HSize;
    rDispInfo.u4H = ((UINT32)prFbmSeqHdr->u2VSize + 0xF) & (~0xF);
    rDispInfo.u4H_Cmp = ((UINT32)prFbmSeqHdr->u2VSize + 0xF) & (~0xF);
#endif
    //file recognize from MM
    //LOG(3, "3D(%d) W,H=(%d, %d) YC(%x %x) ARGB(%x).\n", prVdecEsInfo->e3DType, rDispInfo.u4W, rDispInfo.u4H_Cmp, 
      //      PHYSICAL((UINT32)pu4TmpBufY), PHYSICAL((UINT32)pu4TmpBufC), PHYSICAL((UINT32)pu1ARGBBuf));
    switch(prVdecEsInfo->e3DType)
    {
        case VDEC_3D_NONE:
            if(!IS_Support3D()) //bonding check
                break;

            // VOMX decode to OSD output
            if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
            {
                break;
            }

            if((rDispInfo.u4W_Cmp != 1920) && (rDispInfo.u4W_Cmp != 1280))
                break;    //RealD only support these two resolution

            #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
            TZ_VDEC_Check_RealD((void*)&rTzDispInfo);
            u4UnqualifiedCnt = rTzDispInfo.u4UnqualifiedCnt;
            u1QuincunxCnt = rTzDispInfo.u1QuincunxCnt;
            #else
            //Y start addr of the last line, block 2 raster
            u4Offset = rDispInfo.u4H % 32;
            u4Offset = (u4Offset > 0) ? u4Offset : 32; //the last block has raster count.
            u4BlkSz = 1 << 9; //(64x32) / 4, uint8 to uint32   block rectangle is 16*64
            pu4Dst = pu4TmpBufY;

            //64 is 422mode * 16pixel every block in line.   the last block 's start addr
            pu4DecData = (UINT32 *)((UINT32)rDispInfo.pvYAddr + rDispInfo.u4W * (rDispInfo.u4H - u4Offset));
            #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
            u4MapVirAddr = (UINT32)BSP_MapReservedMem((VOID *)(pu4DecData), rDispInfo.u4W_Cmp*32); // The last column block

            if (!u4MapVirAddr)
            {
                return;
            }
            #endif
            
            pu4DecData = (UINT32*)VIRTUAL((UINT32)pu4DecData + 64 * (u4Offset - 2));  // the last block 's last two line addr
                
            // copy the last second line Y data to dst.
            for(u4WIdx = 0; u4WIdx < (rDispInfo.u4W_Cmp - 64); u4WIdx += 64)
            {
                x_memcpy((void*)pu4Dst, (void*)pu4DecData, 64);   // 16 x4 byte  (4 byte = 422 mode)
                pu4DecData += u4BlkSz;
                pu4Dst += 16;
            }
            x_memcpy((void*)pu4Dst, (void*)pu4DecData, rDispInfo.u4W_Cmp % 64);

            #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
            if (u4MapVirAddr)
            {
                BSP_UnMapReservedMem((VOID *)u4MapVirAddr);
                u4MapVirAddr = 0;
            }
            #endif
            
            //C start addr of the last line, block 2 raster
            u4Offset = (rDispInfo.u4H>>1) % 16;
            u4Offset = (u4Offset > 0) ? u4Offset : 16;
            u4BlkSz = 1 << 8; //64x16 / 4
            pu4Dst = pu4TmpBufC;

            // 64 is 422mode * 16pixel every block in line.   The last column block start addr
            pu4DecData = (UINT32*)((UINT32)rDispInfo.pvCAddr + rDispInfo.u4W * ((rDispInfo.u4H>>1) - u4Offset));
            
            #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
            u4MapVirAddr = (UINT32)BSP_MapReservedMem((VOID *)(pu4DecData), rDispInfo.u4W_Cmp*16); // The last column block

            if (!u4MapVirAddr)
            {
                return;
            }
            #endif
            
            pu4DecData = (UINT32*)VIRTUAL((UINT32)pu4DecData + 64 * (u4Offset - 1));  // the last block 's last two line addr

            for(u4WIdx = 0; u4WIdx < (rDispInfo.u4W_Cmp - 64); u4WIdx += 64)
            {
                x_memcpy((void*)pu4Dst, (void*)pu4DecData, 64);
                pu4DecData += u4BlkSz;
                pu4Dst += 16;
            }
            x_memcpy((void*)pu4Dst, (void*)pu4DecData, rDispInfo.u4W_Cmp % 64);
            
            #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
            if (u4MapVirAddr)
            {
                BSP_UnMapReservedMem((VOID *)u4MapVirAddr);
                u4MapVirAddr = 0;
            }
            #endif
            //pu1DstY = (UINT8*)(UINT32)pu4TmpBufY;
            pu1DstC = (UINT8*)(UINT32)pu4TmpBufC;
            u4PixerIdx = 0;
            //check 3D tag
            bIsFHD = (rDispInfo.u4W_Cmp == 1920) ? TRUE : FALSE;
            u1BlockIdx = (bIsFHD) ? 0 : 1; //720p start from 1, 1080p start from 0
            u1PixIdx = 0; //start from 0
            for(u4WIdx = 0; u4WIdx < rDispInfo.u4W_Cmp; u4WIdx++)
            {
                if(bIsFHD)
                {
                    u1PixIdx = u4WIdx % 32;
                    if(u1PixIdx == 0)
                    {
                        u1BlockIdx++;
                    }
                }
                else //1280
                {
                    u1PixIdx = (u4WIdx ==0) ? 0 : (u1PixIdx + 1);
                    if((((u1BlockIdx & 0x3) == 3) && (u1PixIdx == 22)) ||
                        (((u1BlockIdx & 0x3) < 3) && (u1PixIdx == 21)))
                    {
                        u1PixIdx = 0;
                        u1BlockIdx++;
                    }
                }
                if(u4WIdx == (rDispInfo.u4W_Cmp >> 1)) //left blocks are use to distinguish filter type
                {
                    u1BlockIdx--;
                }
                if((bIsFHD && ((u4WIdx >> 5) >= 54)) ||
                    ((!bIsFHD) && (u4WIdx >= 1152)))    //start checking filter type at 54 block, u1BlockIdx type=3
                {
                    if(!bChkFilterType) //change point
                    {
                        u1BlockIdx = 2; //default: simple horizontal squeeze type
                    }
                    bChkFilterType = TRUE;
                }
                u1BlockIdx = (u1BlockIdx == 4) ? 1 : u1BlockIdx;

                if((u1PixIdx < 5) ||
                    (bIsFHD && (u1PixIdx > 26)) ||
                    ((!bIsFHD) && (u1PixIdx > 16)))
                {                  
                    u4PixerIdx +=1;
                    continue;
                }
                //pu1DstY = (UINT8*)(UINT32)pu4TmpBufY + u4PixerIdx;
                pu1DstC = (UINT8*)(UINT32)pu4TmpBufC + (u4PixerIdx/2)*2;  //422 YCbCr raster
                
                //i1Y = (INT8)*pu1DstY - 16;
                i1Cb = (INT8)*pu1DstC - 128;
                i1Cr = (INT8)*(pu1DstC+1)- 128;

                u4PixerIdx +=1;
                if(u1BlockIdx == 1)
                {
                    if((bIsCur2D && ((2327*i1Cr + 213*i1Cb) < 75000) && ((1793*i1Cr - 2115*i1Cb) < 75000)) || 
                       (!bIsCur2D && ((2327*i1Cr + 213*i1Cb) < 55000)&& ((1793*i1Cr - 2115*i1Cb) < 55000)))
                    {
                        !bChkFilterType ? u4UnqualifiedCnt++ : u1QuincunxCnt++;
                    }
                }
                else if(u1BlockIdx == 2)
                {
                    if((bIsCur2D && ((-2327*i1Cr - 213*i1Cb) < 75000) && ((-534*i1Cr - 2328*i1Cb) < 75000)) || 
                       (!bIsCur2D && ((-2327*i1Cr - 213*i1Cb) < 55000)&& ((-534*i1Cr - 2328*i1Cb) < 55000)))
                    {
                        !bChkFilterType ? u4UnqualifiedCnt++ : u1QuincunxCnt++;
                    }
                }
                else
                {
                    if((bIsCur2D && ((-1793*i1Cr + 2115*i1Cb) < 75000) && ((534*i1Cr + 2328*i1Cb) < 75000)) || 
                       (!bIsCur2D && ((-1793*i1Cr + 2115*i1Cb) < 55000)&& ((534*i1Cr + 2328*i1Cb) < 55000)))
                    {
                        !bChkFilterType ? u4UnqualifiedCnt++ : u1QuincunxCnt++;
                    }
                }
            }
            #endif
            if((bIsCur2D && (u4UnqualifiedCnt < 125)) ||
                (!bIsCur2D && (u4UnqualifiedCnt < 190)))
            {
                bIs3DQualify = TRUE;
                u13DType = B2R_3D_SIDE_BY_SIDE_REAL_D;
                prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
                prFbmSeqHdr->u4B2R3DHeightOffset = 0;
                //unqualify threshold is set by ratio 54blk: 6blk ~= 190: 21
                if((u1QuincunxCnt < 42) && (!prVdecEsInfo->fgIsSimpleFilterType))
                {
                    prVdecEsInfo->fgIsSimpleFilterType = TRUE;
                    LOG(3, "DetectFilterType SHS\n");
                }
                eDetectedMode = E_TDTV_DECODER_INPUT_RD_P;
            }
            prVdecEsInfo->fgIsRealD3DFmt = bIs3DQualify;
            break;
        case VDEC_3D_SBS_LF:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_SIDE_BY_SIDE;
            prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
            prFbmSeqHdr->u4B2R3DHeightOffset = 0;
            eDetectedMode = E_TDTV_DECODER_INPUT_SBS_P;
            break;
        case VDEC_3D_SBS_RF:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_SIDE_BY_SIDE;
            prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
            prFbmSeqHdr->u4B2R3DHeightOffset = 0;
            eDetectedMode = E_TDTV_DECODER_INPUT_SBS_INV_P;
            break;
        case VDEC_3D_TB_LF:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_TOP_N_BOTTOM;
            prFbmSeqHdr->u4B2R3DWidthOffset = 0;
            prFbmSeqHdr->u4B2R3DHeightOffset = prFbmSeqHdr->u2VSize >> 1;
            eDetectedMode = E_TDTV_DECODER_INPUT_TB_P;
            break;
        case VDEC_3D_TB_RF:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_TOP_N_BOTTOM;
            prFbmSeqHdr->u4B2R3DWidthOffset = 0;
            prFbmSeqHdr->u4B2R3DHeightOffset = prFbmSeqHdr->u2VSize >> 1;
            eDetectedMode = E_TDTV_DECODER_INPUT_TB_INV_P;
            break;
        case VDEC_3D_DUAL_STREAM:
        case VDEC_3D_MVC:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_FRAME_SEQUENTIAL;
            eDetectedMode = E_TDTV_DECODER_INPUT_FS_P;
            break;
        case VDEC_3D_REALD_DUAL_STREAM:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_FRAME_SEQUENTIAL;
            eDetectedMode = E_TDTV_DECODER_INPUT_FS_P;
            prFbmSeqHdr->fgRealDEnhance = TRUE;
            break;
		case VDEC_3D_SENSIO_LF:
			bIs3DQualify = TRUE;
			u13DType = B2R_3D_SIDE_BY_SIDE;
			prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
			prFbmSeqHdr->u4B2R3DHeightOffset = 0;
			eDetectedMode = E_TDTV_DECODER_INPUT_SS_P;
			break;
		case VDEC_3D_SENSIO_RF:
			bIs3DQualify = TRUE;
			u13DType = B2R_3D_SIDE_BY_SIDE;
			prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
			prFbmSeqHdr->u4B2R3DHeightOffset = 0;
			eDetectedMode = E_TDTV_DECODER_INPUT_SS_P;
			break;		  
        case VDEC_3D_CHECKER_BOARD:
            eDetectedMode = E_TDTV_DECODER_INPUT_CB_P;
            break;
        case VDEC_3D_BIVL:
            bIs3DQualify = TRUE;
            u13DType = B2R_3D_FRAME_SEQUENTIAL;//B2R_3D_BIVL_FRAME_SEQUENTIAL;
            eDetectedMode = E_TDTV_DECODER_INPUT_FS_P;
            break;
        default:
            break;
    }
    //LOG(2, "3DType(%d), DetectedMode(%d), bIs3D(%d).\n", u13DType, eDetectedMode, bIs3DQualify);
    if(!prVdecEsInfo->fgB2rResLimitation)
    {
    prFbmSeqHdr->u1B2R3DType = u13DType;
    }

    //interlace
    if(!prFbmSeqHdr->fgProgressiveSeq)
    {
        eDetectedMode -= 1;
    }
    if((eDetectedMode != prVdecEsInfo->eDetectedMode) &&
        (!prVdecEsInfo->fgB2rResLimitation))
    {
        prVdecEsInfo->eDetectedMode = eDetectedMode;
        _VDEC_TDTVModeChangeCB((void*)&ucEsId);

        //content format change -> use new mode directly, without waiting vdec command
        prVdecEsInfo->u13DUsrCtrlMode = prVdecEsInfo->u13DUsrCtrlModeNtfy;
        prVdecEsInfo->u1IsForceLView = prVdecEsInfo->u1IsForceLViewNtfy;
    }

    //HAL_GetTime(&rTimeE);
    //HAL_GetDeltaTime(&rTimeDt, &rTimeS, &rTimeE);
    //LOG(5, "bIs3DQualify %d, u4UnqualifiedCnt %d, %u.%06us\n", bIs3DQualify, u4UnqualifiedCnt,
    //    rTimeDt.u4Seconds, rTimeDt.u4Micros);

    // 3d display setting by central control module
    if(prVdecEsInfo->u13DUsrCtrlMode == VDEC_3D_CTRL_FORCE_2D)
    {
        if((prFbmSeqHdr->fgB2R3DEnable != bIs3DQualify) ||
            (!prFbmSeqHdr->fgB2RForce2D) ||
            (prFbmSeqHdr->u1B2RForce2DType != prVdecEsInfo->u1IsForceLView))
        {
            prFbmSeqHdr->fgB2R3DEnable = bIs3DQualify;
            prFbmSeqHdr->u1B2R3DType = u13DType;
            prFbmSeqHdr->fgB2RForce2D = TRUE;
            prFbmSeqHdr->u1B2RForce2DType = prVdecEsInfo->u1IsForceLView;    // 0:L, 1:R
            LOG(2, "[VDEC_3D]Force 2D, view %d\n",  prFbmSeqHdr->u1B2RForce2DType);
            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
    }
    else if(prVdecEsInfo->u13DUsrCtrlMode == VDEC_3D_CTRL_FORCE_2D_LR)
    {
        if((!prFbmSeqHdr->fgB2R3DEnable) ||
            (!prFbmSeqHdr->fgB2RForce2D) ||
            ((prFbmSeqHdr->u1B2R3DType != B2R_3D_SIDE_BY_SIDE_REAL_D) &&
            (prFbmSeqHdr->u1B2R3DType != B2R_3D_SIDE_BY_SIDE)) ||
            (prVdecEsInfo->fgIsRealD3DFmt && (prFbmSeqHdr->u1B2R3DType != B2R_3D_SIDE_BY_SIDE_REAL_D)) ||
            (!prVdecEsInfo->fgIsRealD3DFmt && (prFbmSeqHdr->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D)) ||
            (prFbmSeqHdr->u1B2RForce2DType != prVdecEsInfo->u1IsForceLView))
        {
            prFbmSeqHdr->fgB2R3DEnable = TRUE;
            prFbmSeqHdr->fgB2RForce2D = TRUE;
            prFbmSeqHdr->u1B2RForce2DType = prVdecEsInfo->u1IsForceLView;    // 0:L, 1:R
            prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
            prFbmSeqHdr->u4B2R3DHeightOffset = 0;
            prFbmSeqHdr->u1B2R3DType =
                prVdecEsInfo->fgIsRealD3DFmt ? B2R_3D_SIDE_BY_SIDE_REAL_D : B2R_3D_SIDE_BY_SIDE;
            LOG(2, "[VDEC_3D]Force 2D LF, IsRealD %d, view %d\n",
                prVdecEsInfo->fgIsRealD3DFmt, prFbmSeqHdr->u1B2RForce2DType);
            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
    }
    else if(prVdecEsInfo->u13DUsrCtrlMode == VDEC_3D_CTRL_FORCE_2D_TB)
    {
        if(!prFbmSeqHdr->fgB2RForce2D ||
            (!prFbmSeqHdr->fgB2R3DEnable) ||
            (prFbmSeqHdr->u1B2R3DType != B2R_3D_TOP_N_BOTTOM) ||
            (prFbmSeqHdr->u1B2RForce2DType != prVdecEsInfo->u1IsForceLView))
        {
            prFbmSeqHdr->fgB2R3DEnable = TRUE;
            prFbmSeqHdr->fgB2RForce2D = TRUE;
            prFbmSeqHdr->u1B2RForce2DType = prVdecEsInfo->u1IsForceLView;    // 0:L, 1:R
            prFbmSeqHdr->u4B2R3DWidthOffset = 0;
            prFbmSeqHdr->u4B2R3DHeightOffset = prFbmSeqHdr->u2VSize >> 1;
            prFbmSeqHdr->u1B2R3DType = B2R_3D_TOP_N_BOTTOM;
            LOG(2, "[VDEC_3D]Force 2D TB, view %d\n", prFbmSeqHdr->u1B2RForce2DType);
            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
    }
    else if(prVdecEsInfo->u13DUsrCtrlMode == VDEC_3D_CTRL_FORCE_3D_LR)
    {
        if(prFbmSeqHdr->fgB2RForce2D ||
            (!prFbmSeqHdr->fgB2R3DEnable) ||
            ((prFbmSeqHdr->u1B2R3DType != B2R_3D_SIDE_BY_SIDE_REAL_D) &&
            (prFbmSeqHdr->u1B2R3DType != B2R_3D_SIDE_BY_SIDE)) ||
            (prVdecEsInfo->fgIsRealD3DFmt && (prFbmSeqHdr->u1B2R3DType != B2R_3D_SIDE_BY_SIDE_REAL_D)) ||
            (!prVdecEsInfo->fgIsRealD3DFmt && (prFbmSeqHdr->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D)))
        {
            prFbmSeqHdr->fgB2R3DEnable = TRUE;
            prFbmSeqHdr->fgB2RForce2D = FALSE;
            prFbmSeqHdr->u4B2R3DWidthOffset = rDispInfo.u4W_Cmp >> 1;
            prFbmSeqHdr->u4B2R3DHeightOffset = 0;
            prFbmSeqHdr->u1B2R3DType =
                prVdecEsInfo->fgIsRealD3DFmt ? B2R_3D_SIDE_BY_SIDE_REAL_D : B2R_3D_SIDE_BY_SIDE;
            LOG(2, "[VDEC_3D]Force 3D LR, IsRealD %d\n", prVdecEsInfo->fgIsRealD3DFmt);
            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
    }
    else if(prVdecEsInfo->u13DUsrCtrlMode == VDEC_3D_CTRL_FORCE_3D_TB)
    {
        if(prFbmSeqHdr->fgB2RForce2D ||
            (!prFbmSeqHdr->fgB2R3DEnable) ||
            (prFbmSeqHdr->u1B2R3DType != B2R_3D_TOP_N_BOTTOM))
        {
            prFbmSeqHdr->fgB2R3DEnable = TRUE;
            prFbmSeqHdr->fgB2RForce2D = FALSE;
            prFbmSeqHdr->u4B2R3DWidthOffset = 0;
            prFbmSeqHdr->u4B2R3DHeightOffset = prFbmSeqHdr->u2VSize >> 1;
            prFbmSeqHdr->u1B2R3DType = B2R_3D_TOP_N_BOTTOM;
            LOG(2, "[VDEC_3D]Force 3D TB\n");
            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
    }
    else if(prVdecEsInfo->u13DUsrCtrlMode == VDEC_3D_CTRL_BYPASS)
    {
        if(prFbmSeqHdr->fgB2R3DEnable)
        {
            prFbmSeqHdr->fgB2R3DEnable = FALSE;
            prFbmSeqHdr->fgB2RForce2D = FALSE;
            prFbmSeqHdr->u4B2R3DWidthOffset = 0;
            prFbmSeqHdr->u4B2R3DHeightOffset = 0;
            LOG(2, "[VDEC_3D]Bypass 3D handling\n");
            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
    }
    else
    {
        if(prFbmSeqHdr->fgB2RForce2D ||
            (bIs3DQualify !=prFbmSeqHdr->fgB2R3DEnable) ||
            (bIs3DQualify && (prFbmSeqHdr->u1B2R3DType != u13DType)))
        {
            prFbmSeqHdr->fgB2R3DEnable = bIs3DQualify;
            if(bIs3DQualify)
            {
                prFbmSeqHdr->u1B2R3DType = u13DType;
            }
            if(!(prFbmSeqHdr->fgB2RForce2D && (!bIs3DQualify))) // 2D to 2D
            {
                LOG(2, "[VDEC_3D]SetSeqChange Is3D=%d, e3DType %d\n", bIs3DQualify, prVdecEsInfo->e3DType);
                FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
            }
            prFbmSeqHdr->fgB2RForce2D = FALSE;
        }
    }
}
#endif

UINT64 _VDEC_Div6464(UINT64 u8Dividend, UINT64 u8Divider)
{
    UINT64 u8Remainder;
    if(u8Dividend == 0)
    {
        return 0;
    }
    if (u8Divider >> 32)
    {
        return u8Div6464(u8Dividend, u8Divider, &u8Remainder);
    }
    return u8Div6432(u8Dividend, u8Divider, &u8Remainder);
}
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

#if 1 // put here due to aud_if.c is open to customer
extern UINT32 u4GetDspImgAddr(void);
extern UINT32 u4GetFlashData(UINT32 u4Addr);
VOID DRV_DIVX_GetHWSecretKeyEx(UINT8 *pu1KeyBuf)
{
    INT32 i;
    UINT32 tmp;
    UINT32 start = 32;
    UINT32 dwFlashOfst;
    UINT32 dwOfst;
    UINT32* puSrc;

    dwFlashOfst = u4GetDspImgAddr ();   // set to dsp image addr
    dwOfst = u4GetFlashData (dwFlashOfst + (150 << 2));
    puSrc= ((UINT32*) (dwFlashOfst + ((dwOfst)*4)));
    for (i = 32 / 2; i != 0; i --)
    {
        tmp = puSrc[start ++];
        *pu1KeyBuf ++ = tmp & 0xFF;
        *pu1KeyBuf ++ = (tmp >> 8) & 0xFF;
    }
}
#else
extern UINT16 AUD_ReadDROM (UINT32 u4Addr);
VOID DRV_DIVX_GetHWSecretKeyEx(UINT8 *pu1KeyBuf)
{
    INT32 i;
    UINT32 tmp;
    UINT32 start = 32;

    for (i = 32 / 2; i != 0; i --)
    {
        tmp = AUD_ReadDROM (start ++);
        *pu1KeyBuf ++ = tmp & 0xFF;
        *pu1KeyBuf ++ = (tmp >> 8) & 0xFF;
    }
}
#endif

#ifdef VDEC_DEC_REG_QUERY
static UINT32 _gu4Cmd = 0;
inline VOID VDEC_SetLogReg(UINT32 u4Cmd)
{
    _gu4Cmd = u4Cmd&0x03;

    return ;
}

inline UINT32 VDEC_GetLogReg(VOID)
{
    return _gu4Cmd;
}

void VDEC_PrintDecReg(UCHAR u1VdecId, E_REG_DUMP_T eCondition)
{
    if(((_gu4Cmd&0x01==0x01) && (eCondition==e_DECODE_START_REG_DUMP)) ||
       ((_gu4Cmd&0x02==0x02) && (eCondition==e_DECODE_DONE_REG_DUMP)) ||
       (eCondition==e_FORCE_REG_DUMP))
    {
        VDEC_HDR_INFO_T rHdrInfo;
        UINT32 u4Cnt = 0;

        LOG(0, "VDEC (%u)\n", u1VdecId);
        if (!VDEC_QueryInfo(u1VdecId, &rHdrInfo))
        {
            return ;
        }

        if (!rHdrInfo.fgHdrInvalid)
        {
            switch (rHdrInfo.eCodecType)
            {
                case VDEC_FMT_H264:
                case VDEC_FMT_H264VER:
                    if (rHdrInfo.eCodecType == VDEC_FMT_H264VER)
                    {
                        Printf("VDEC FMT: H264VER\n");
                    }
                    else
                    {
                        Printf("VDEC FMT: H264\n");
                    }

                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        //_VDEC_H264DumpInputWnd();
                    }
                    //_VDEC_DumpReg();
                    break;
                case VDEC_FMT_WMV:
                    Printf("VDEC FMT: WMV\n");
                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        vDumpWmvInputWnd(u1VdecId);
                    }
                    vDumpWmvMcReg(u1VdecId);
                    u4DumpWmvReg(NULL);
                    break;
                case VDEC_FMT_MPV:
                    if (TRUE == rHdrInfo.fgMPEG2)
                    {
                        Printf("VDEC FMT: MPEG2\n");
                    }
                    else
                    {
                        Printf("VDEC FMT: MPEG1\n");
                    }
                    // Tmp solution
                    for (u4Cnt=0; u4Cnt<MPV_MAX_VLD; u4Cnt++)
                    {
                        if (eCondition==e_DECODE_START_REG_DUMP ||
                            eCondition==e_FORCE_REG_DUMP)
                        {
                            _MPV_DumpInputWnd((UCHAR)u4Cnt);
                        }
                        _MPV_PrintReg((UCHAR)u4Cnt);
                    }
                    break;
                case VDEC_FMT_MP4:
                    LOG(0, "VDEC FMT: MPEG4\n");
                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        _VDEC_MPEG4DumpInputWnd();
                    }
                    _VDEC_MPEG4DumpReg();
                    break;
                case VDEC_FMT_VP8:
                    LOG(0, "VDEC FMT: VP8\n");
                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        VDEC_VP8DumpInputWnd();
                    }
                    VDEC_VP8DumpReg();
                    break;
                case VDEC_FMT_VP6:
                    Printf("VDEC FMT: VP6\n");
                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        _VDEC_VP6DumpInputWnd();
                    }
                    _VDEC_VP6DumpChksumReg();
                    break;
                case VDEC_FMT_RV:
                    Printf("VDEC FMT: RV\n");
                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        vDumpRvInputWnd();
                    }
                    vDumpRmChkSum();
                    break;
                case VDEC_FMT_AVS:
                    LOG(0, "VDEC FMT: AVS\n");
                    if (eCondition==e_DECODE_START_REG_DUMP ||
                        eCondition==e_FORCE_REG_DUMP)
                    {
                        _VDEC_AVSDumpInputWnd();
                    }
                    _VDEC_DumpAvsReg();
                    break;
                default:
                    LOG(0, "VDEC FMT: UnKnown\n");
                    break;
            }
        }
    }

    return ;
}
#endif // VDEC_DEC_REG_QUERY

UINT32 VdecCodecChangeHandle(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    if (ucEsId >= VDEC_MAX_ES)
    {
        ASSERT(ucEsId < VDEC_MAX_ES);
        ucEsId = ES0;
        LOG(1, "_VDEC_MainLoop(): (ucEsId >= VDEC_MAX_ES)\n");
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                VDEC_COND_DECODE_STATUS_CHG, (UINT32)VDEC_DEC_DECODE_CODEC_TYPE_CHG, 0);
    LOG(0,"Notify codec change!\n"); 
    prVdecEsInfo->fgForceFlush = TRUE;
    prVdecEsInfoKeep->eCurState = VDEC_ST_VIRTUAL_STOP;

    return 1;
}

#ifdef CC_SUPPORT_VDEC_PREPARSE
#define ANALYZE_PTS_CNT 3
INT32 VdecHdrPreParse(UCHAR ucMpvId, UCHAR ucEsId)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    VDEC_ES_INFO_T *prVdecEsInfo;
    INT32   i4RetValue = E_VDEC_OK;
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    LOG(6, "%s, eCurFMT:%d\n", __FUNCTION__, (UINT32)prVdecEsInfoKeep->eCurFMT);
    
    /*if enalbe trustzone, should use the function:
       TZ_DMX_GetPicHeader(UINT32 u4Addr, UINT32 u4BufStart, UINT32 u4BufEnd,UINT8 *pu1Buf, UINT32 u4Size)
        read data first*/
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY_I_FRAME)
    {
        TZ_DMX_GetPicHeader(prVdecEsInfoKeep->rPesInfo.u4VldReadPtr,
                        prVdecEsInfoKeep->rPesInfo.u4FifoStart,
                        prVdecEsInfoKeep->rPesInfo.u4FifoEnd,
                        prVdecEsInfoKeep->au1SeqInfo,
                        VDEC_SEQINFO_SIZE);
    }
#endif
    prVdecEsInfo->u4BitDepth = 8;
    prVdecEsInfoKeep->u4PreparseRcvCnt = 0;
    switch(prVdecEsInfoKeep->eCurFMT)
    {
        case VDEC_FMT_H265:
            ucMpvId = VDEC_VLD0;
            prVdecEsInfo->u4VldId = VDEC_VLD0;
            i4RetValue = _VDEC_H265PrePrsHdr(ucMpvId, ucEsId);
            prVdecEsInfo->u4VldId = VDEC_MAX_VLD;
            break;        
        case VDEC_FMT_H264:
            //ucMpvId = VDEC_VLD0;
            ucMpvId = VDEC_VLD0;
            prVdecEsInfo->u4VldId = VDEC_VLD0;
            i4RetValue = _VDEC_H264PrePrsHdr(ucMpvId, ucEsId);
            prVdecEsInfo->u4VldId = VDEC_MAX_VLD;
            break;
        case VDEC_FMT_AVS:
            i4RetValue = _VDEC_AvsPrePrsHdr(ucMpvId, ucEsId);
            break; 
        case VDEC_FMT_MPV:
            i4RetValue = _VDEC_MpvPrePrsHdr(ucMpvId, ucEsId);
            break; 
    #ifdef ENABLE_MULTIMEDIA
        #ifndef CC_VDEC_MPEG4_DISABLE
        case VDEC_FMT_MP4:
            i4RetValue = _VDEC_Mpeg4PrePrsHdr(ucMpvId, ucEsId);
            break; 
        #endif
        #ifndef CC_VDEC_RV_DISABLE
        case VDEC_FMT_RV:
            i4RetValue = _VDEC_RvPrePrsHdr(ucMpvId, ucEsId);
            break;
        #endif
        #ifndef CC_VDEC_VP6_DISABLE
        case VDEC_FMT_VP6:
            i4RetValue = _VDEC_Vp6PrePrsHdr(ucMpvId, ucEsId);
            break;  
        #endif
        #ifndef CC_VDEC_VP8_DISABLE
        case VDEC_FMT_VP8:
            i4RetValue = _VDEC_Vp8PrePrsHdr(ucMpvId, ucEsId);
            break;
        #endif
        #ifndef CC_VDEC_VP9_DISABLE
        case VDEC_FMT_VP9:
            i4RetValue = _VDEC_Vp9PrePrsHdr(ucMpvId, ucEsId);
            break; 
        #endif       
        #ifndef CC_VDEC_WMV_DISABLE
        case VDEC_FMT_WMV:
            i4RetValue = _VDEC_WmvPrePrsHdr(ucMpvId, ucEsId);
            break; 
        #endif
    #endif
        case VDEC_FMT_MJPEG:  //Virtual info for MJPEG alloc B2R
            prVdecEsInfo->u2OrgHSize = 1920;
            prVdecEsInfo->u2OrgVSize = 1080;
            prVdecEsInfo->fgProgressive = TRUE;
            prVdecEsInfo->u4FrameRate = 30;
            prVdecEsInfo->u4BitDepth = 8;
            prVdecEsInfo->fgEnableUFO = FALSE; 
            i4RetValue = E_VDEC_OK;
            break;
        default:
            i4RetValue = E_VDEC_FAIL;
            break;
    }
    return i4RetValue;
}


UINT32 u4VDecDurationToFrameRate(UINT32 u4Duration)
{
    UINT32 u4RetVal = 0;

    if (u4Duration != 0)
    {
        u4RetVal = (90000*100)/u4Duration;
        LOG(2,"Duration %d ret %d\n",u4Duration,u4RetVal);
    }
    else
    {
        u4RetVal = 3000;    
        LOG(2,"Default: Duration %d ret %d\n",u4Duration,u4RetVal);
    }

    return u4RetVal;
}


void vBubbleSort(UINT32 *pu4Src,INT32 i4Cnt)
{
    UINT32 i,j,temp;
    for(j = 0; j < i4Cnt - 1; j++)
    {
        for(i = 0; i < (i4Cnt - j - 1); i++)
        {
            if(pu4Src[i] > pu4Src[i + 1])
            {
                temp = pu4Src[i];
                pu4Src[i] = pu4Src[i + 1];
                pu4Src[i + 1] = temp;
            }
        }
    }
}


INT32 VdecCalcFRFromES(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UINT32 u4DeltaPTS;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if ((VDEC_FMT_H264 == prVdecEsInfoKeep->eCurFMT) && (!IS_H264_PIC_NALU(prVdecEsInfoKeep->rPesInfo.ucPicType)))
    {
        return E_VDEC_PARSE_NOT_READY;
    }
    else if ((VDEC_FMT_H265 == prVdecEsInfoKeep->eCurFMT) && (!IsH265SliceNalUnit(prVdecEsInfoKeep->rPesInfo.ucPicType)))
    {
        return E_VDEC_PARSE_NOT_READY;
    }
    else if (prVdecEsInfoKeep->rPesInfo.fgDtsValid)
    {
        prVdecEsInfoKeep->u4EsPTS[prVdecEsInfoKeep->u4PreparseRcvCnt] = prVdecEsInfoKeep->rPesInfo.u4PTS;
        prVdecEsInfoKeep->u4PreparseRcvCnt++;
        //LOG(0, "pts %x\n", prVdecEsInfoKeep->u4EsPTS[prVdecEsInfoKeep->u4PreparseRcvCnt]);
        //LOG(0, "u4PreparseRcvCnt %d\n", prVdecEsInfoKeep->u4PreparseRcvCnt);
    }
    else if (!prVdecEsInfoKeep->rPesInfo.fgDtsValid)
    {
        return E_VDEC_OK;
    }
    if (prVdecEsInfoKeep->u4PreparseRcvCnt >= ANALYZE_PTS_CNT)
    {
        vBubbleSort(prVdecEsInfoKeep->u4EsPTS, prVdecEsInfoKeep->u4PreparseRcvCnt);
        
        u4DeltaPTS = _VDEC_AnalyzeDeltaPts(ucEsId);
        prVdecEsInfo->u4FrameRate = u4VDecDurationToFrameRate(u4DeltaPTS);
        return E_VDEC_OK;
    }
    return E_VDEC_PARSE_NOT_READY;
}


UINT32 _VDEC_AnalyzeDeltaPts(UCHAR ucEsId)
{
    UINT32 u4DeltaPts[ANALYZE_PTS_CNT] = {0},u4DeltaPtsCnt[ANALYZE_PTS_CNT] = {0};
    UINT32 i,j,idx = 0,u4Max = 0,u4TempDelta;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    for(i = 0; i < ANALYZE_PTS_CNT - 1; i++)
    {
        u4DeltaPts[idx] = prVdecEsInfoKeep->u4EsPTS[i + 1] - prVdecEsInfoKeep->u4EsPTS[i];
        u4TempDelta = u4DeltaPts[idx];
        idx ++;

        for(j = 0; j < idx; j++)
        {
            if((u4TempDelta >= u4DeltaPts[j] - 1) && (u4TempDelta <= u4DeltaPts[j] + 1))
            {
                u4DeltaPtsCnt[j] ++;
                LOG(2,"Delta[%d] %d  Delat[%d] %d,cnt %d\n",idx - 1,u4DeltaPts[idx - 1],j,u4DeltaPts[j],u4DeltaPtsCnt[j]);
            }
        }
    }

    for(i = 0; i < ANALYZE_PTS_CNT - 1; i++)
    {
        if(u4Max < u4DeltaPtsCnt[i])
        {
            u4Max = u4DeltaPtsCnt[i];
            idx = i;
            LOG(2,"Max %d delta[%d] %d\n",u4Max,i,u4DeltaPtsCnt[i]);
        }
    }

    return u4DeltaPts[idx];
}
#endif

UCHAR _VDEC_CheckSkipMode(UCHAR ucEsId)
{
    UCHAR ucSkipMode = 0;
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    
    if(prVdecEsInfo->fgMMPlayback)
    {
        switch (prVdecEsInfo->u4MMSpeed)
        {
        case STC_SPEED_TYPE_FORWARD_2X:
#ifdef VDEC_TIME_SHIFT_2X_I_MODE
            if(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
            {
                LOG(7, "Skip mode 2x -> I only\n");
                ucSkipMode = IMode;
            }
            else
#endif
            {
                LOG(7, "Skip mode -> I/P\n");
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
        case STC_SPEED_TYPE_REWIND_1_DIV_2X:
        case STC_SPEED_TYPE_REWIND_1_DIV_4X:
        case STC_SPEED_TYPE_REWIND_1_DIV_8X:
        case STC_SPEED_TYPE_REWIND_1_DIV_16X:
        case STC_SPEED_TYPE_REWIND_1_DIV_32X:
        
            LOG(7, "(%d) Skip mode -> I only\n", ucEsId);
            ucSkipMode = IMode;
            break;
        default:
            LOG(7, "(%d) Skip mode -> I/P/B\n", ucEsId);
            ucSkipMode = IPBMode;
            break;
        }
    }
#if defined(CC_USE_DDI)
    else
    {
        switch (prVdecEsInfo->u4MMSpeed)
        {
        case STC_SPEED_TYPE_FORWARD_2X:
            LOG(7, "(%d) Skip mode -> I/P\n", ucEsId);
            ucSkipMode = IPMode;
            break;
        case STC_SPEED_TYPE_FORWARD_4X:
            LOG(7, "(%d) Skip mode -> I only\n", ucEsId);
            ucSkipMode = IMode;
            break;
        default:
            LOG(7, "(%d) Skip mode -> I/P/B\n", ucEsId);
            ucSkipMode = IPBMode;
            break;
        }
    }
#endif
    if(prVdecEsInfo->eDecodeMode != VDEC_DECODE_MODE_UNKNOWN)
    {
        switch (prVdecEsInfo->eDecodeMode)
        {
        case VDEC_DECODE_MODE_NORMAL:
        case VDEC_DECODE_MODE_NO_REORDER:
            ucSkipMode = IPBMode;
            break;
        case VDEC_DECODE_MODE_I_ONLY:
            ucSkipMode = IMode;
            break;
        case VDEC_DECODE_MODE_B_SKIP:
            ucSkipMode = IPMode;
            break;
        default:
            ucSkipMode = IPBMode;
            break;
        }
    }
    
    if (prVdecEsInfo->fgRVUSkip)
    {
        ucSkipMode = prVdecEsInfo->ucRVUSkipMode;
        LOG(7,"Skip mode special case RVU %d\n",ucSkipMode);
    }
    return ucSkipMode;
}


#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
UCHAR _VDEC_Set3DCtrl(UCHAR ucEsId)
{       
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);       
    VDEC_ES_INFO_T* prVdecEsInfoSub;
    
    //check dependent view
    if((prVdecEsInfo->e3DType == VDEC_3D_MVC) ||
            (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
    {
        prVdecEsInfoSub = _VDEC_GetEsInfo(ES1);    //dependent view
        if((prVdecEsInfoSub->e3DType == VDEC_3D_MVC) ||
            (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
        {
            if((prVdecEsInfoSub->e3DType == VDEC_3D_MVC) && (FALSE == fgPrevFbAllocateFail))
            {
                if(prVdecEsInfo->fgIsDependView)
                {
                    _VDEC_MvcSetViewIdx(prVdecEsInfoKeep->rPesInfo.ucEsId,1);
                }
                else
                {
                    _VDEC_MvcSetViewIdx(prVdecEsInfoKeep->rPesInfo.ucEsId,0);
                }
                if(IS_MVC_BASE_PIC_NALU(prVdecEsInfoKeep->rPesInfo.ucPicType))
                {
                    prVdecEsInfo->fgIsDependView = TRUE;
                    prVdecEsInfo->u4BasePTS = prVdecEsInfoKeep->rPesInfo.u4PTS;

                    if((!prVdecEsInfo->fgRsvSpsMvc)||(!prVdecEsInfo->fgRsvSps))
                    {
                        if(!((!prVdecEsInfo->fgRsvData)&&(prVdecEsInfo->fgRsvSps)))
                        {
                            return VDEC_MAX_VLD;
                        }

                    }
                }
                else if(IS_MVC_DEPEND_PIC_NALU(prVdecEsInfoKeep->rPesInfo.ucPicType))
                {
                    prVdecEsInfo->fgIsDependView = FALSE;
                    if(prVdecEsInfo->fgMMPlayback &&
                        (prVdecEsInfo->u4MMSpeed == STC_SPEED_TYPE_FORWARD_1X))
                    {

                        if((!prVdecEsInfo->fgRsvSpsMvc)||(!prVdecEsInfo->fgRsvSps))
                        {
                            return VDEC_MAX_VLD;
                        }
                        else if((prVdecEsInfo->u4BasePTS > 0) &&
                            (prVdecEsInfoKeep->rPesInfo.u4PTS != prVdecEsInfo->u4BasePTS))
                        {
                            LOG(3, "PTS mismatch, L 0x%x/R 0x%x\n", prVdecEsInfo->u4BasePTS, prVdecEsInfoKeep->rPesInfo.u4PTS);
                            _VDEC_MvcStartptsBoth(0, TRUE);
                            if(prVdecEsInfo->fgIsMVCPtsCmp)
                            {
                                if(_VDEC_MVCFindLRPair(prVdecEsInfo->u4VldId, ucEsId))
                                {
                                    LOG(3, "Auto correction done.\n");
                                    _VDEC_MvcStartptsBoth(0, FALSE);
                                        //VDEC_ES_INFO_T *prVdecEsInfo;
                                   SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                                   SET_DECFLAG(prVdecEsInfoSub, DEC_FLG_WAIT_NEXT_I);
                                }
                                //if(prVdecEsInfoKeep->rPesInfo.u4PTS != prVdecEsInfo->u4BasePTS) //possible to fix in _VDEC_MVCFindLRPair
                                //{
                                    LOG(3, "Skip\n");
                                    prVdecEsInfo->fgIsDependView = FALSE;
                                    return VDEC_MAX_VLD;
                               // }
                            }
                        }
                        else if(fgErrorHappened)
                        {
                            _VDEC_MvcStartptsBoth(0, FALSE);
                            fgErrorHappened = FALSE;
                        }

                    }                 
                        //LOG(2, "Force Base View. Dep DispQ %d > %d\n", ucDepDispQNum, ucBaseDispQNum);
                }
            }
            if(prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM)
            {
                UCHAR ucBaseDispQNum = 0, ucDepDispQNum = 0;
                ucBaseDispQNum = FBM_CheckFrameBufferDispQ(prVdecEsInfo->ucFbgId);
                ucDepDispQNum = FBM_CheckFrameBufferDispQ(prVdecEsInfo->ucSubFbgId);

                if(ucBaseDispQNum > ucDepDispQNum)
                {
                    prVdecEsInfo->fgIsDependView = TRUE;
                    //LOG(2, "Force Depend View. Base DispQ %d > %d\n", ucBaseDispQNum, ucDepDispQNum);
                }
                else if(ucBaseDispQNum < ucDepDispQNum)
                {
                    prVdecEsInfo->fgIsDependView = FALSE;
                    //LOG(2, "Force Base View. Dep DispQ %d > %d\n", ucDepDispQNum, ucBaseDispQNum);
                }
            }
        }
    }  
    if((prVdecEsInfo->e3DType == VDEC_3D_MVC) ||(prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
    {
        prVdecEsInfoSub = _VDEC_GetEsInfo(ES1);    //dependent view
        if(((prVdecEsInfoSub->e3DType == VDEC_3D_MVC) ||
            (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM)) && (IS_MVC_DEPEND_PIC_NALU(prVdecEsInfoKeep->rPesInfo.ucPicType)))
        {
            prVdecEsInfoSub->u8Offset = prVdecEsInfoKeep->rPesInfo.u8Offset;
        }
    }   
    return prVdecEsInfoKeep->rPesInfo.ucMpvId;
}
#endif
#endif
#endif

#ifdef CC_VDEC_FMT_DETECT
UCHAR _VDEC_DetectVdecFMT(UCHAR ucEsId, UINT32 u4Type)
{   
    UINT32 u4DmxType = (UINT32)VDEC_FMT_MAX, u4NtfyCodecType, u4Thrd;
    VDEC_DEC_DECODE_STATUS_T eNfyStatus;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    PARAM_MPV_T* prMpvCounter;
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prMpvCounter = prVdecEsInfo->prMpvCounter;
    
    if(H264_PIC_VALID_TYPE(u4Type))
    {
        u4DmxType = (UINT32)VDEC_FMT_H264;
        if((u4Type != H264_PIC_TYPE_SEQ) &&
            (prVdecEsInfo->arRcvPesCnt[u4DmxType] == 0) &&
            (u4DmxType != (UINT32)prVdecEsInfoKeep->eCurFMT))
        {
            return VDEC_MAX_VLD;    //start counting from seq header
        }
    }
    else if(MPV_PIC_VALID_TYPE(u4Type))
    {
        u4DmxType = (UINT32)VDEC_FMT_MPV;
    }
    else if(VC1_PIC_VALID_TYPE(u4Type))
    {
        u4DmxType = (UINT32)VDEC_FMT_WMV;
    }
    else if(MPEG4_PIC_VALID_TYPE(u4Type))
    {
        u4DmxType = (UINT32)VDEC_FMT_MP4;
    }
    else
    {
        return VDEC_MAX_VLD;
    }

    if((u4DmxType < (UINT32)VDEC_FMT_MAX) && (prVdecEsInfoKeep->eCurFMT < VDEC_FMT_MAX))
    {
        prVdecEsInfo->arRcvPesCnt[u4DmxType]++;
        if(prVdecEsInfo->arRcvPesCnt[u4DmxType] == 0xFF)
        {
            x_memset(prVdecEsInfo->arRcvPesCnt, 0, sizeof(prVdecEsInfo->arRcvPesCnt));
        }

        if(u4DmxType != (UINT32)prVdecEsInfoKeep->eCurFMT)
        {
            LOG(6, "detec (type,num)=(%d,%d), cur(%d,%d)\n",
                u4DmxType, prVdecEsInfo->arRcvPesCnt[u4DmxType],
                prVdecEsInfoKeep->eCurFMT, prVdecEsInfo->arRcvPesCnt[prVdecEsInfoKeep->eCurFMT]);
            //if(!prVdecEsInfo->fgFmtDectFinish)
            {
                u4Thrd = (prMpvCounter->u4DecOkNs > CODEC_DETECT_THRD) ? prMpvCounter->u4DecOkNs : CODEC_DETECT_THRD;
                if((prVdecEsInfo->arRcvPesCnt[u4DmxType] > prVdecEsInfo->arRcvPesCnt[(UINT32)prVdecEsInfoKeep->eCurFMT]) &&
                    ((prVdecEsInfo->arRcvPesCnt[u4DmxType] - prVdecEsInfo->arRcvPesCnt[(UINT32)prVdecEsInfoKeep->eCurFMT])
                    >= u4Thrd) /*&&
                    (prMpvCounter->u4DecOkNs <= CODEC_DETECT_THRD)*/)
                {
                    switch(u4DmxType)
                    {
                        case VDEC_FMT_MPV:
                            u4NtfyCodecType = VDEC_CODEC_MPEG_2;
                            break;
                        case VDEC_FMT_MP4:
                            u4NtfyCodecType = VDEC_CODEC_MPEG_4;
                            break;
                        case VDEC_FMT_H264:
                            u4NtfyCodecType = VDEC_CODEC_H264;
                            break;
                        case VDEC_FMT_WMV:
                            u4NtfyCodecType = VDEC_CODEC_WVC1;
                            break;
                        case VDEC_FMT_MJPEG:
                            u4NtfyCodecType = VDEC_CODEC_MJPEG;
                            break;
                        case VDEC_FMT_VP6:
                            u4NtfyCodecType = VDEC_CODEC_VP6;
                            break;
                        case VDEC_FMT_VP8:
                            u4NtfyCodecType = VDEC_CODEC_VP8;
                            break;
                        default:
                            u4NtfyCodecType = VDEC_CODEC_UNKNOWN;
                            break;
                    }
                    prVdecEsInfo->fgFmtDectFinish = TRUE;
                    //notify MW
                    if(prVdecEsInfoKeep->pfDecNfyFct && (!prVdecEsInfo->fgMMPlayback))
                    {
                        if(VDEC_ChkCodecCap(ucEsId, (ENUM_VDEC_FMT_T)u4DmxType))
                        {
                            LOG(3, "ES(%d) Notify Status change, codec type change, type %d\n", ucEsId, u4DmxType);
                            eNfyStatus = VDEC_DEC_DECODE_CODEC_TYPE_CHG;
                        }
                        else
                        {
                            LOG(3, "ES(%d) Notify Status change, codec not support\n", ucEsId);
                            eNfyStatus = VDEC_DEC_DECODE_CODEC_NOT_SUPPORT;
                            prVdecEsInfoKeep->eCurState = VDEC_ST_IDLE;
                        }
                        if(prVdecEsInfo->eNotifyStatus != eNfyStatus)
                        {
                            prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag,
                                VDEC_COND_DECODE_STATUS_CHG, (UINT32)eNfyStatus, u4NtfyCodecType);
                            prVdecEsInfo->eNotifyStatus = eNfyStatus;
                        }
                        x_memset(prVdecEsInfo->arRcvPesCnt, 0, sizeof(prVdecEsInfo->arRcvPesCnt));
                    }
                }
            }
            return VDEC_MAX_VLD;
        }
    }
    return prVdecEsInfoKeep->rPesInfo.ucMpvId;
}
#endif

#ifdef CC_MAPLE_CUST_DRV
VOID _VDEC_CalBitrate(UCHAR ucEsId)
{      
    UINT32 u4FbgId = FBM_FBG_ID_UNKNOWN;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    u4FbgId = (UINT32)prVdecEsInfo->ucFbgId;
    
    u4FbgId = (UINT32)prVdecEsInfo->ucFbgId;

    if (u4FbgId == FBM_FBG_ID_UNKNOWN)
    {
        LOG(1,"FBM FBG ID UNKNOWN!\n");
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr((UCHAR)u4FbgId);

    if(prFbmSeqHdr != NULL)
    {
        switch(prFbmSeqHdr->ucFrmRatCod)
        {
            case MPEG_FRAME_RATE_24_: /* 24000/1001 (23.976...) */
            case MPEG_FRAME_RATE_24:
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 24;
                break;
            case MPEG_FRAME_RATE_25:
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 25;
                break;
            case MPEG_FRAME_RATE_30_:
            case MPEG_FRAME_RATE_30:
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 30;
                break;
            case MPEG_FRAME_RATE_50: 
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 50;
                break;
            case MPEG_FRAME_RATE_60_:
            case MPEG_FRAME_RATE_60:
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 60;
                break;
                break;
            case MPEG_FRAME_RATE_12: 
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 12;
                break;
            case MPEG_FRAME_RATE_15:
               prVdecEsInfo->rBitRateInfo.u4FrameRate = 15;
               break;
             case MPEG_FRAME_RATE_20:
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 20;
                break;
            default:
                prVdecEsInfo->rBitRateInfo.u4FrameRate = 30;
                break;
        }
    }
    //Calculate bitrate
    if (prVdecEsInfoKeep->pfnVdecIsPic)
    {
        if (prVdecEsInfoKeep->pfnVdecIsPic(ucEsId, &prVdecEsInfoKeep->rPesInfo))
        {
            _VDEC_CalEsDataSize(ucEsId);
            
            if(prVdecEsInfo->rBitRateInfo.fgValidData)
            {
                prVdecEsInfo->rBitRateInfo.u4PicSize[prVdecEsInfo->rBitRateInfo.u1CurIdx]= prVdecEsInfo->u4CurrPicSize;
                prVdecEsInfo->rBitRateInfo.u1CurIdx ++;
                if ((prVdecEsInfo->rBitRateInfo.u1CurIdx >= prVdecEsInfo->rBitRateInfo.u4FrameRate) && 
                    (prVdecEsInfo->rBitRateInfo.u4FrameRate != 0))
                {
                    prVdecEsInfo->rBitRateInfo.u1CurIdx =  prVdecEsInfo->rBitRateInfo.u1CurIdx % prVdecEsInfo->rBitRateInfo.u4FrameRate;
                    prVdecEsInfo->rBitRateInfo.u4NonePicSize = 0;
                }
                prVdecEsInfo->rBitRateInfo.fgValidData = FALSE;
            }
        }
        else
        {
            _VDEC_CalEsDataSize(ucEsId);
            
            if(prVdecEsInfo->rBitRateInfo.fgValidData)
            {
                prVdecEsInfo->rBitRateInfo.u4NonePicSize += prVdecEsInfo->u4CurrPicSize;
                prVdecEsInfo->rBitRateInfo.fgValidData = FALSE;
            }
        }
    }
}
#endif

#ifdef USB_CLI_FORCE_SETTING_SUPPORT //Cli force usb file seamless for debug
EXTERN UINT32 u4ForceSrc;
#endif

#if defined(CC_FLIP_MIRROR_SUPPORT) && defined(CC_SUPPORT_NPTV_SEAMLESS)
#include "drv_display.h"
#endif

BOOL VDEC_ChkSeamlessModeChg(UCHAR ucEsId, UINT32 u4W, UINT32 u4H)
{
#ifdef ENABLE_MULTIMEDIA
    UINT32 eSeamlessMode;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    
    if(ucEsId >= VDEC_MAX_ES)
    {
        return FALSE;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    ASSERT(prVdecEsInfo!=NULL);
    ASSERT(prVdecEsInfoKeep!=NULL);
    if((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        return FALSE;
    }
#ifdef USB_CLI_FORCE_SETTING_SUPPORT //Cli force usb file seamless for debug
    if (u4ForceSrc != (UINT32)SWDMX_SRC_TYPE_UNKNOWN)
    {
        prVdecEsInfo->eMMSrcType = (SWDMX_SOURCE_TYPE_T)u4ForceSrc;
        
        switch (prVdecEsInfo->eMMSrcType)
        {
            case SWDMX_SRC_TYPE_NETWORK_LIVE_STREAMING:   //=15
                eSeamlessMode = SEAMLESS_ONE_FBS | SEAMLESS_ONE_RESIZER;
                break;
            case SWDMX_SRC_TYPE_NETWORK_NETFLIX:   //=3
                eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_ONE_RESIZER;
                break;
            case SWDMX_SRC_TYPE_NETWORK_HLS:   //=17
                eSeamlessMode = SEAMLESS_ONE_FBS | SEAMLESS_BY_NPTV;
                break;
            case SWDMX_SRC_TYPE_YOUTUBE:   //=21
                eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_BY_NPTV;
                break;
            default:
                eSeamlessMode = SEAMLESS_NONE;
                break;
        }
    }
    else
#endif
    switch (prVdecEsInfo->eMMSrcType)
    {
        case SWDMX_SRC_TYPE_NETWORK_NETFLIX_3D:  //TBD: SEAMLESS_ONE_FBS | SEAMLESS_TWO_RESIZER;
        #if defined(CC_MT5398)
            eSeamlessMode = SEAMLESS_ONE_FBS | SEAMLESS_ONE_RESIZER;
            break;
        #endif
        case SWDMX_SRC_TYPE_NETWORK_NETFLIX: // =3 //Netflix stream spec refer that: all ref <= 3
        #ifdef CC_SUPPORT_NPTV_SEAMLESS 
        {
            #ifdef CC_FLIP_MIRROR_SUPPORT 
            {
                UINT8 u1Conf = u1GetFlipMirrorConfig();
                if ((u1Conf & SYS_FLIP_CONFIG_ON) || (u1Conf & SYS_MIRROR_CONFIG_ON))
                {
                    // Disable NPTV Seamless while flip mirror on
                    eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_ONE_RESIZER;
                }
                else
                {
                    // Enable NPTV Seamless only without flip mirror
                    eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_BY_NPTV;
                }
            }
            #else  //Enable NPTV Seamless while Flip& Mirror disable
                eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_BY_NPTV;
            #endif
            
            if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)  //HEVC not ready yet for NPTV Seamless
            {
                eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_ONE_RESIZER;
            }
        }
            break;
        #endif
        case SWDMX_SRC_TYPE_NETWORK_LIVE_STREAMING:   //=15
        case SWDMX_SRC_TYPE_NETWORK_HLS:  //=18
        case SWDMX_SRC_TYPE_NETWORK_DASH:
        case SWDMX_SRC_TYPE_YOUTUBE:
        case SWDMX_SRC_TYPE_NETWORK_MSS:
        case SWDMX_SRC_TYPE_NETWORK_VUDU:
            eSeamlessMode = SEAMLESS_TWO_FBS | SEAMLESS_ONE_RESIZER;
            break;
        //eSeamlessMode = SEAMLESS_ONE_FBS | SEAMLESS_ONE_RESIZER;
        //eSeamlessMode = SEAMLESS_2FBS_2POOL | SEAMLESS_ONE_RESIZER;
        //eSeamlessMode = SEAMLESS_2FBS_1POOL | SEAMLESS_ONE_RESIZER;
        default:
            eSeamlessMode = SEAMLESS_NONE;
            break;
    }

    // One FBs limitation: NumRefFrames <= FHD_REFERENCE_NS.
    #if 0  //Not use
    if ((eSeamlessMode & SEAMLESS_ONE_FBS) && (u4RefNum > FBM_FBG_H264_FHD_REFERNCE_NS))
    {
        eSeamlessMode = SEAMLESS_NONE;
        if (prVdecEsInfo->eSeamlessMode != eSeamlessMode)
        {
            LOG(1, "NumRefFrames(%d) > %d, Seamless None.\n", prDecPrm->prSPS->u4NumRefFrames, FBM_FBG_H264_FHD_REFERNCE_NS);
        }
    }
    #endif

    //TBD: DIVX TPT 
        
    //4K2K with two resizer
    #if 0//TBD
    #ifdef CC_SUPPORT_NPTV_SEAMLESS 
    if (eSeamlessMode & SEAMLESS_BY_NPTV)
    {
        eSeamlessMode &= ~(SEAMLESS_BY_NPTV);
        eSeamlessMode |= SEAMLESS_BY_RESIZER;
    }
    #endif
    if ((eSeamlessMode & SEAMLESS_BY_RESIZER)&& 
        ((u4W > 1920) || (u4H > 1088) || 
         (prVdecEsInfo->u4SeamlessWidth > 1920) || (prVdecEsInfo->u4SeamlessHeight > 1088)))
    {
        eSeamlessMode &= ~(SEAMLESS_ONE_RESIZER);
        eSeamlessMode |= SEAMLESS_TWO_RESIZER;
    }
    #endif

    if((prVdecEsInfo->u4SeamlessWidth == 0) || (prVdecEsInfo->u4SeamlessHeight == 0))
    {
        prVdecEsInfo->u4SeamlessWidth = 1920;
        prVdecEsInfo->u4SeamlessHeight = 1080;
    }
    else if ((u4W > prVdecEsInfo->u4SeamlessWidth) 
        ||(u4H > (_MAKE_32X(prVdecEsInfo->u4SeamlessHeight))))
    {
        LOG(9, "seamless w/h[%dx%d] < source w/h[%dx%d]\n",
            prVdecEsInfo->u4SeamlessWidth, prVdecEsInfo->u4SeamlessHeight, u4W, u4H);
        
        if(prVdecEsInfo->fgEnableUFO)
        {
            LOG(9, "Down scaling seamless with UFO mode not support\n");
        }
        eSeamlessMode = SEAMLESS_NONE;
    }
    
    if ((eSeamlessMode & SEAMLESS_BY_RESIZER) && (prVdecEsInfo->u4SeamlessWidth > 1920))
    {
        eSeamlessMode &= ~(SEAMLESS_ONE_RESIZER);
        eSeamlessMode |= SEAMLESS_TWO_RESIZER;
    }

    if (prVdecEsInfo->eSeamlessMode == eSeamlessMode)
    {
        return FALSE;
    }

    LOG(3, "Seamless Mode Chg: 0x%x -> 0x%x.\n", prVdecEsInfo->eSeamlessMode, eSeamlessMode);
    prVdecEsInfo->eSeamlessMode = eSeamlessMode;
    prVdecEsInfo->fgSeamlessPlay = (eSeamlessMode != SEAMLESS_NONE);
    
    return TRUE;
#else
    UNUSED(ucEsId);
    UNUSED(u4W);
    UNUSED(u4H);
    return FALSE;
#endif
}

UCHAR _VDEC_CheckSkipES(UCHAR ucEsId, UCHAR ucSkipMode, UCHAR ucMpvId)
{
    INT8 i1EsBufStatus = 0;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    if(prVdecEsInfoKeep->pfnVdecEsFlwCtrl && (ucMpvId < VDEC_MAX_VLD))
    {
        if (prVdecEsInfo->u2Count <= prVdecEsInfo->u2UdfNs)
        {
            i1EsBufStatus = -1;
        }
        else if (prVdecEsInfo->ucDropBNs > 0)
        {
            i1EsBufStatus = 1;
        }

        if(prVdecEsInfo->u1LastSkipType==IMode&&ucSkipMode!=IMode)
        {
            SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        }

        prVdecEsInfo->u1LastSkipType=ucSkipMode;
        if(prVdecEsInfoKeep->pfnVdecEsFlwCtrl(ucEsId, &ucMpvId, i1EsBufStatus, ucSkipMode))
        {
            if((i1EsBufStatus > 0) && (prVdecEsInfo->ucDropBNs > 0))
            {
                prVdecEsInfo->ucDropBNs--;
            }
            return VDEC_MAX_VLD;//_VDecReceiveEs(ucEsId);
        }
        else
        {
            if(ucSkipMode==IMode
                &&prVdecEsInfo->u4MMSpeed<STC_SPEED_TYPE_REWIND_1X
                &&prVdecEsInfo->eStreamType==VDEC_STREAM_TYPE_IPB)
            {
               UINT32 u4DeltaPts=0;
               UINT8 u1B2rId = (UINT8)(VDP_Es2Vdp(ucEsId));
               u4DeltaPts=(90000/VDP_GetFrameRate(u1B2rId))*VDP_TrickModeSkipFrameNo(u1B2rId);
               if((prVdecEsInfo->u4PTSLast!=prVdecEsInfoKeep->rPesInfo.u4PTS)
                     &&(prVdecEsInfo->u4PTSLast+u4DeltaPts>prVdecEsInfoKeep->rPesInfo.u4PTS)&&
                     (!((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
                     #ifdef ENABLE_MULTIMEDIA
                     &&(prVdecEsInfo->eContainerType>= SWDMX_FMT_MPEG2_TS) && (prVdecEsInfo->eContainerType< SWDMX_FMT_VC1_ES)
                     #endif
                     )))
               {
                   if((i1EsBufStatus > 0) && (prVdecEsInfo->ucDropBNs > 0))
                   {
                       prVdecEsInfo->ucDropBNs--;
                   }
                   return VDEC_MAX_VLD;//_VDecReceiveEs(ucEsId);
               }
               else
               {
                  prVdecEsInfo->u4PTSLast=prVdecEsInfoKeep->rPesInfo.u4PTS;
                  prVdecEsInfo->u8PTSLast=prVdecEsInfoKeep->rPesInfo.u8PTS;
               }
            }
        }
    }
    return ucMpvId;
}
