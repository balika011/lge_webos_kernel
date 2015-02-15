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
 * $RCSfile: vdec_esm.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_esm.c
 *  This file contains ES manager of MPV.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
#include "mpv_if.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "mpv_drvif.h"
#include "vdec_flw.h"
#include "vdec_type.h"
#include "vdec_debug.h"
#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#ifndef CC_VDEC_H264_DISABLE
#include "vdec_h264api.h"
#endif
#endif
#ifndef CC_VDEC_AVS_DISABLE
#include "vdec_avsapi.h"
#endif
#ifdef ENABLE_MULTIMEDIA
#ifndef CC_VDEC_VP6_DISABLE
#include "vdec_vp6api.h"
#endif
#endif

#ifndef CC_VDEC_H265_DISABLE
#include "./h265/vdec_h265_api.h"
#endif

#ifndef CC_VDEC_VP9_DISABLE
#include "./vp9/vdec_vp9_api.h"
#endif

#include "./hw_common/vdec_hw_common.h"
#if defined(CC_MT5890)
#include "./lae/vdec_lae.h"
#endif
#include "fbm_drvif.h"
#include "dmx_if.h"
#include "drv_dbase.h"
#include "drv_common.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_syslog.h"
#include "x_timer.h"
#include "x_hal_5381.h"
#if defined(VDEC_IS_POST_MT5398)
#include "x_ckgen.h"
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static DRV_DBASE_T* _prDbase = NULL;
VDEC_INFO_T _rVdecInfo;
static VDEC_PES_INFO_T _arPesInfoEsm[VDEC_MAX_ES];

#ifdef MPV_VIRTUAL_PRS
static BOOL _fgMpvVirtualPrsStart = FALSE;
static BOOL _fgMpvVirtualPrsDestroy = FALSE;
static UINT32 _ucMpvVirtualPrsDelay = 0;
static UCHAR _ucMpvVPEsNs = 0;
static UCHAR _ucMpvVpEsId[VDEC_MAX_ES];
static UCHAR _ucMpvVpMpvId[VDEC_MAX_ES];
static UINT32 _u4MpvVpPicNs[VDEC_MAX_ES];
static UINT32 _u4MpvVpPicCount[VDEC_MAX_ES];
static UINT32 _u4MpvVpStartAddr[VDEC_MAX_ES];
static UINT32* _u4MpvVpAddr[VDEC_MAX_ES];
static UCHAR* _u4MpvVpPicTp[VDEC_MAX_ES];
#endif
#ifdef CC_DUAL_CORE_DEC
static HANDLE_T _hStopSema[VDEC_MAX_ES] = {(HANDLE_T)NULL};
#endif

#define MVC_MAIN_VIDEO 0
#define MVC_SUB_VIDEO 1

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

VDEC_INFO_T* _VDEC_GetInfo(void)
{
    return (VDEC_INFO_T*)&(_rVdecInfo);
}

VDEC_ES_INFO_T* _VDEC_GetEsInfo(UCHAR ucEsId)
{
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "_VDEC_GetEsInfo ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        //ASSERT(0);
        ucEsId = (VDEC_MAX_ES - 1);
    }
    return &(_rVdecInfo.arVdecEsInfo[ucEsId]);
}

VDEC_ES_INFO_KEEP_T* _VDEC_GetEsInfoKeep(UCHAR ucEsId)
{
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "_VDEC_GetEsInfoKeep ucEsId(%d) > VDEC_MAX_ES, oops...\n", ucEsId);
        //ASSERT(0);
        ucEsId = (VDEC_MAX_ES - 1);
    }
    return &(_rVdecInfo.arVdecEsInfoKeep[ucEsId]);
}
/*
VDEC_PES_INFO_T* _VDEC_GetPesInfo()
{
    return &_rVdecInfo.rPesInfo;
}*/

void _VDEC_CreateEsmSema(UCHAR ucEsId)
{
    CHAR szBuf[16];
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        return;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((!prVdecEsInfo) || (!prVdecEsInfoKeep))
    {
        LOG(1, "prVdecEsInfo/prVdecEsInfoKeep null\n");
        return;
    }

    #ifdef MPV_VIRTUAL_PRS
    prVdecEsInfo->u2UdfNs = 0;
    #else
    prVdecEsInfo->u2UdfNs = MPV_UDF_PES_NS;
    #endif

    //ilii test
    //VDEC_PES_INFO_T rPesInfo;

    // create semaphore
    prVdecEsInfo->ucStarted = 0;
    VERIFY(x_sema_create(&prVdecEsInfoKeep->hUnderFlowSema, X_SEMA_TYPE_BINARY,
        X_SEMA_STATE_LOCK) == OSR_OK);

    // create message queue
    x_snprintf(szBuf, sizeof(szBuf),  "ESM-Q%d\n", ucEsId);
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hMsgQ, szBuf,
        sizeof(VDEC_PES_INFO_T), VDEC_MSG_Q_SIZE) == OSR_OK);

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    x_snprintf(szBuf, sizeof(szBuf),  "ESM-QSub%d\n", ucEsId);
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hMsgQSub, szBuf,
        sizeof(VDEC_PES_INFO_T), VDEC_MSG_Q_SIZE) == OSR_OK);
#endif
#ifdef CC_SUPPORT_VDEC_PREPARSE
    x_snprintf(szBuf, sizeof(szBuf),  "ESM-QParse%d\n", ucEsId);
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hMsgQPreParse, szBuf,
        sizeof(VDEC_PES_INFO_T), VDEC_MSG_Q_SIZE) == OSR_OK);
#endif
    
#if defined(CC_MT5890)
    x_snprintf(szBuf, sizeof(szBuf),  "LaeIn-Q%d\n", ucEsId);
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hMsgQLaeInput, szBuf,
            sizeof(VDEC_LAE_DecPrm), 5) == OSR_OK);
            
    x_snprintf(szBuf, sizeof(szBuf),  "LaeOut-Q%d\n", ucEsId);        
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hMsgQLaeOuput, szBuf,
            sizeof(VDEC_LAE_DecPrm), 5) == OSR_OK);
#endif
    x_snprintf(szBuf, sizeof(szBuf),  "IO-Q%d\n", ucEsId);
    VERIFY(x_sema_create(&prVdecEsInfoKeep->hMutex, X_SEMA_TYPE_MUTEX,
        X_SEMA_STATE_UNLOCK) == OSR_OK);

    VERIFY(x_sema_create(&prVdecEsInfoKeep->hFlushSema, X_SEMA_TYPE_BINARY,
        X_SEMA_STATE_LOCK) == OSR_OK);

    VERIFY(x_sema_create(&prVdecEsInfoKeep->hWaitStopSema, X_SEMA_TYPE_BINARY,
        X_SEMA_STATE_LOCK) == OSR_OK);

    VERIFY(x_sema_create(&prVdecEsInfoKeep->hSrcCtrlMutex, X_SEMA_TYPE_MUTEX,
            X_SEMA_STATE_UNLOCK) == OSR_OK);

    VERIFY(x_sema_create(&prVdecEsInfoKeep->hFlushMutex, X_SEMA_TYPE_MUTEX,
        X_SEMA_STATE_UNLOCK) == OSR_OK);

    VERIFY(x_sema_create(&prVdecEsInfoKeep->hCcSema, X_SEMA_TYPE_MUTEX,
        X_SEMA_STATE_UNLOCK) == OSR_OK);

    VERIFY(x_sema_create(&prVdecEsInfoKeep->hVdecSetPriSema, X_SEMA_TYPE_MUTEX,
        X_SEMA_STATE_UNLOCK) == OSR_OK);

#ifdef CC_REALD_3D_SUPPORT
    VERIFY(x_sema_create(&prVdecEsInfoKeep->hVdec3DModeChange, X_SEMA_TYPE_MUTEX,
        X_SEMA_STATE_UNLOCK) == OSR_OK);
#endif

#ifdef CC_SUPPORT_VDEC_PREPARSE
    VERIFY(x_sema_create(&prVdecEsInfoKeep->hWaitKickSema, X_SEMA_TYPE_BINARY,
        X_SEMA_STATE_LOCK) == OSR_OK);
#endif
    // create multi vdec thread schedule msg Q and Semaphone
    x_snprintf(szBuf, sizeof(szBuf),  "MTS-Q%d\n", ucEsId);
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hVdecScheMsg, szBuf,
    sizeof(VDEC_VLD_STATUS_T), 1) == OSR_OK);
    // create command queue
    x_snprintf(szBuf, sizeof(szBuf),  "CMDQ-Q%d\n", ucEsId);
    VERIFY(x_msg_q_create(&prVdecEsInfoKeep->hCmdQ, szBuf, VDEC_CMD_SIZE,
        VDEC_CMD_Q_SIZE) == OSR_OK);
    
#ifdef CC_DUAL_CORE_DEC
    VERIFY(x_sema_create(&_hStopSema[ucEsId], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
#endif

    VERIFY(x_timer_create(&prVdecEsInfoKeep->hDataTimer) == OSR_OK);
    VERIFY(x_timer_create(&prVdecEsInfoKeep->hCodecChkTimer) == OSR_OK);
}


void _VDEC_DeleteEsmSema(UCHAR ucEsId)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        return;
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if(!prVdecEsInfoKeep)
    {
        LOG(1, "prVdecEsInfoKeep null\n");
        return;
    }

    // delete semaphore
    VERIFY(x_sema_delete(prVdecEsInfoKeep->hUnderFlowSema)== OSR_OK);

    // delete message queue
    VERIFY(x_msg_q_delete(prVdecEsInfoKeep->hMsgQ)== OSR_OK);
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    VERIFY(x_msg_q_delete(prVdecEsInfoKeep->hMsgQSub)== OSR_OK);
    #endif
#ifdef CC_SUPPORT_VDEC_PREPARSE
    VERIFY(x_msg_q_delete(prVdecEsInfoKeep->hMsgQPreParse)== OSR_OK);
    prVdecEsInfoKeep->hMsgQPreParse = NULL_HANDLE;
#endif

    VERIFY(x_sema_delete(prVdecEsInfoKeep->hMutex) == OSR_OK);
#if 1
    VERIFY(x_sema_delete(prVdecEsInfoKeep->hMsgQLaeInput) == OSR_OK);
    VERIFY(x_sema_delete(prVdecEsInfoKeep->hMsgQLaeOuput) == OSR_OK);
#endif

    VERIFY (x_sema_delete(prVdecEsInfoKeep->hFlushSema) == OSR_OK);

    VERIFY (x_sema_delete(prVdecEsInfoKeep->hWaitStopSema) == OSR_OK);

    VERIFY(x_sema_delete(prVdecEsInfoKeep->hFlushMutex) == OSR_OK);

    VERIFY(x_sema_delete(prVdecEsInfoKeep->hCcSema) == OSR_OK);
    VERIFY(x_sema_delete(prVdecEsInfoKeep->hVdecSetPriSema) == OSR_OK);

    // delete command queue
    VERIFY(x_msg_q_delete(prVdecEsInfoKeep->hCmdQ)== OSR_OK);
    VERIFY(x_msg_q_delete(prVdecEsInfoKeep->hVdecScheMsg)== OSR_OK);

    VERIFY(x_timer_delete(prVdecEsInfoKeep->hDataTimer)== OSR_OK);
    
    #ifdef CC_SUPPORT_VDEC_PREPARSE
    VERIFY (x_sema_delete(prVdecEsInfoKeep->hWaitKickSema) == OSR_OK);
    #endif
    
#ifdef CC_DUAL_CORE_DEC
    VERIFY(x_sema_delete(_hStopSema[ucEsId]) == OSR_OK);
#endif
    VERIFY(x_timer_delete(prVdecEsInfoKeep->hCodecChkTimer)== OSR_OK);
}


void _VDEC_LockFlushSema(UCHAR ucEsId)
{
    VERIFY(x_sema_lock(_rVdecInfo.arVdecEsInfoKeep[ucEsId].hFlushSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}


void _VDEC_UnlockFlushSema(UCHAR ucEsId)
{
    VERIFY(x_sema_unlock(_rVdecInfo.arVdecEsInfoKeep[ucEsId].hFlushSema) == OSR_OK);
}


void _VDEC_LockFlushMutex(UCHAR ucEsId)
{
    VERIFY(x_sema_lock(_rVdecInfo.arVdecEsInfoKeep[ucEsId].hFlushMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
}


void _VDEC_UnlockFlushMutex(UCHAR ucEsId)
{
    VERIFY(x_sema_unlock(_rVdecInfo.arVdecEsInfoKeep[ucEsId].hFlushMutex) == OSR_OK);
}


#ifdef CC_DUAL_CORE_DEC
void _VDEC_LockStopSema(UCHAR ucEsId)
{
    VERIFY(x_sema_lock(_hStopSema[ucEsId], X_SEMA_OPTION_WAIT) == OSR_OK);
}


void _VDEC_UnlockStopSema(UCHAR ucEsId)
{
    VERIFY(x_sema_unlock(_hStopSema[ucEsId]) == OSR_OK);
}
#endif


void _VDEC_LockCcSema(UCHAR ucEsId)
{
    VERIFY(x_sema_lock(_rVdecInfo.arVdecEsInfoKeep[ucEsId].hCcSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}


void _VDEC_UnlockCcSema(UCHAR ucEsId)
{
    VERIFY(x_sema_unlock(_rVdecInfo.arVdecEsInfoKeep[ucEsId].hCcSema) == OSR_OK);
}

#ifdef TEST_ONOFF_RUNTIME
static BOOL _fgTestOnoff = TRUE;
#endif

#if VLD_LOCK_LOG
UINT32 _u4VLDLockLogIdx = 0;
VDEC_LOCK_VLD_LOG_T _arVDecLockVLDLog[VLD_LOCK_LOG_CNT];

UINT32 u4VLDLogWr(UCHAR ucEsId, UCHAR ucVldId, UINT32 u4LogValue)
{
  _arVDecLockVLDLog[_u4VLDLockLogIdx].u4LogValue = u4LogValue;
  _arVDecLockVLDLog[_u4VLDLockLogIdx].ucEsId = ucEsId;
  _arVDecLockVLDLog[_u4VLDLockLogIdx].ucVldId = ucVldId;
  _arVDecLockVLDLog[_u4VLDLockLogIdx].ucLockCnt = _VDEC_GetInfo()->arVdecVldInfo[ucVldId].u4LockCount;
  _u4VLDLockLogIdx++;
  _u4VLDLockLogIdx %= VLD_LOCK_LOG_CNT;

  return E_VDEC_OK;
}

#endif
UINT32 _VDEC_LockVld(UCHAR ucEsId, UCHAR ucUnuseVldId)
{
#define VDEC_MULTI_CORE_LOCK_TIMEOUT 33 //ms
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    UCHAR aucVldId[VDEC_MAX_VLD];
    UCHAR aucVldLockRet[VDEC_MAX_VLD];
    UCHAR ucNeedVldCnt = 0, ucWaitLockIdx = 0;
    UINT32 u4TryLockTimer = VDEC_MULTI_CORE_LOCK_TIMEOUT;// ms;
    
    CRIT_STATE_T _rState;
        
    UNUSED(ucUnuseVldId);
    ASSERT(ucEsId < VDEC_MAX_ES);
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "ucEsId %d >= VDEC_MAX_ES\n", ucEsId);
        return E_VDEC_FAIL;
    }
    

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if((!prVdecEsInfo) || (!prVdecInfo) || (!prVdecEsInfoKeep))
    {
        LOG(0, "ucEsId/ucVldId fail\n");
        return E_VDEC_FAIL;
    }
    #ifndef CC_VDEC_RM_SUPPORT
    ASSERT(prVdecEsInfo->u4VldId == ucUnuseVldId);
    #endif
    UNUSED(prVdecEsInfoKeep);
    
    #ifndef CC_VDEC_RM_SUPPORT
    if (prVdecEsInfo->u4VldId >= VDEC_MAX_VLD) 
    {
        LOG(0, "[%s-%d]ucVldId(%d) >= VDEC_MAX_VLD\n", __func__, __LINE__, prVdecEsInfo->u4VldId);
        return E_VDEC_FAIL;
    }
    #else
    if ((prVdecEsInfo->u4VldId >= VDEC_MAX_VLD) && (prVdecEsInfo->u4Vld2Id >= VDEC_MAX_VLD))
    {
        LOG(0, "[%s-%d]ucVldId(%d) && ucVld2Id(%d) >= VDEC_MAX_VLD\n", __func__, __LINE__, prVdecEsInfo->u4VldId, prVdecEsInfo->u4Vld2Id);
        return E_VDEC_FAIL;
    }
    #endif

    x_memset(aucVldId, VDEC_MAX_VLD, sizeof(aucVldId));
    x_memset(aucVldLockRet, OSR_OK, sizeof(aucVldLockRet));

    if (prVdecEsInfo->u4VldId < VDEC_MAX_VLD)
    {
        aucVldId[ucNeedVldCnt++] = prVdecEsInfo->u4VldId;
    }
    #ifdef CC_VDEC_RM_SUPPORT
    if (prVdecEsInfo->u4Vld2Id < VDEC_MAX_VLD)
    {
        aucVldId[ucNeedVldCnt++] = prVdecEsInfo->u4Vld2Id;
    }
    #endif

    if (ucNeedVldCnt == 2)
    {
        if (aucVldId[0] == aucVldId[1])
        {
            LOG(1, "[%s] ucVldId(%d, %d) error.\n", __func__, aucVldId[0], aucVldId[1]);
            ASSERT(aucVldId[0] != aucVldId[1]);
        }
        do
        {
            aucVldLockRet[ucWaitLockIdx] = x_sema_lock(prVdecInfo->arVdecVldInfo[aucVldId[ucWaitLockIdx]].hVldSemaphore, X_SEMA_OPTION_WAIT);
            _rState = x_crit_start();
            ucWaitLockIdx = (ucWaitLockIdx ? 0 : 1);  //The other
            aucVldLockRet[ucWaitLockIdx] = x_sema_lock(prVdecInfo->arVdecVldInfo[aucVldId[ucWaitLockIdx]].hVldSemaphore, X_SEMA_OPTION_NOWAIT);
            
            if ((aucVldLockRet[0] != OSR_OK) || (aucVldLockRet[1] != OSR_OK))
            {
                ucWaitLockIdx = VDEC_MAX_ES;
                if (aucVldLockRet[0] == OSR_OK)
                {
                    VERIFY(x_sema_unlock(prVdecInfo->arVdecVldInfo[aucVldId[0]].hVldSemaphore) == OSR_OK);
                    ucWaitLockIdx = 1;  //The other lock fail, lock it for block
                }
                if (aucVldLockRet[1] == OSR_OK)
                {
                    VERIFY(x_sema_unlock(prVdecInfo->arVdecVldInfo[aucVldId[1]].hVldSemaphore) == OSR_OK);
                    ucWaitLockIdx = 0;  //The other lock fail, lock it for block
                }
                x_crit_end(_rState);
                u4TryLockTimer--;
                if (!u4TryLockTimer)
                {
                    LOG(1, "Try Lock ucVldId(%d && %d) %d Times Fail.\n", aucVldId[0], aucVldId[1], u4TryLockTimer);
                    u4TryLockTimer = VDEC_MULTI_CORE_LOCK_TIMEOUT;
                    // return E_VDEC_FAIL; // If return here, this function must return value, instead of return void. 
                }
            }
            else
            {
                //lock success.
                x_crit_end(_rState);
                break;
            }
        }while(u4TryLockTimer);
        
        if (0)//!u4TryLockTimer)
        {
            LOG(0, "Try Lock ucVldId(%d) ucVld2Id(%d) %d Times, Fail.\n", aucVldId[0], aucVldId[1], u4TryLockTimer);
            return E_VDEC_FAIL; // If return here, this function must return value, instead of return void.
        }
    }
    else if (ucNeedVldCnt == 1)
    {
        VERIFY(x_sema_lock(prVdecInfo->arVdecVldInfo[aucVldId[0]].hVldSemaphore, X_SEMA_OPTION_WAIT) == OSR_OK);
    }
    else 
    {
        ASSERT((ucNeedVldCnt == 1) || (ucNeedVldCnt == 2));
    }

    do
    {
        ucNeedVldCnt--;
        if(prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].u4LockCount == 0)
        {
            prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].ucLockEsId = ucEsId;
        #ifdef TEST_ONOFF_RUNTIME
            prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].fgSwrst = FALSE;
            #ifdef CC_DYNAMIC_POWER_ONOFF  //power off
            if((aucVldId[ucNeedVldCnt] == VLD0) && (_fgTestOnoff))
            {
            #ifdef VDEC_IS_POST_MT5368
                IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_SPEC, VDEC_PDN_CON_OFF);
                IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_MODULE, VDEC_PDN_CON_OFF);
            #else
                IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_ON);
            #endif
            }
            else if ((aucVldId[ucNeedVldCnt] == VLD1) && (_fgTestOnoff))
            {
                //TBD
            }
            #endif
        #endif
        }
        else
        {
                if(prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].ucLockEsId != ucEsId)
                {
                    LOG(0, "ucLockEsId(%d) should be ucEsId(%d) ===========\n",
                            (UINT32)prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].ucLockEsId,
                            (UINT32)ucEsId);
                    ASSERT(0);
                }
        }
        prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].u4LockCount++;
        if(prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].u4LockCount > 2)
        {
            // Max is 2
            // One is in decoder.
            // One is in swdmx_mpg header parse.
            LOG(0, "u4LockCount(%d) should not happen ===========\n",
                    prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].u4LockCount);
            LOG(0, "Please review your vdec lock/unlock code\n");
            UNUSED(prVdecInfo->arVdecVldInfo[aucVldId[ucNeedVldCnt]].u4LockCount);
        
        #if VLD_LOCK_LOG
            {
            UINT32 u4Idx = 0;
            
            LOG(0, "_u4VLDLockLogIdx:%d\n", _u4VLDLockLogIdx);

            for(u4Idx = 0; u4Idx < VLD_LOCK_LOG_CNT; u4Idx++)
            { 
                LOG(0, "[%2d] ES:%2d, VLD:%2d, Lock:%2d, Log:%d\n", 
                    u4Idx,
                    _arVDecLockVLDLog[u4Idx].ucEsId,
                    _arVDecLockVLDLog[u4Idx].ucVldId,
                    _arVDecLockVLDLog[u4Idx].ucLockCnt,
                    _arVDecLockVLDLog[u4Idx].u4LogValue);
            }
            }
        #endif
            
            ASSERT(0);
        }
        _VDEC_SetIsr(ucEsId, aucVldId[ucNeedVldCnt]);
    }while(ucNeedVldCnt);
    
    UNUSED(prVdecEsInfo);

    return E_VDEC_OK;
}

void _VDEC_UnlockVld(UCHAR ucEsId, UCHAR ucUnuseVldId)
{
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    UCHAR aucVldId[VDEC_MAX_VLD];
    UCHAR ucVldId = VDEC_MAX_VLD;
    UCHAR ucTmpIdx = 0;

    UNUSED(ucUnuseVldId);
    ASSERT(ucEsId < VDEC_MAX_ES);
    //ASSERT(ucVldId < VDEC_MAX_VLD);
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(0, "ucEsId %d >= VDEC_MAX_ES\n", ucEsId);
        return;
    }

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if((!prVdecEsInfo) || (!prVdecInfo))
    {
        LOG(0, "ucEsId/ucVldId fail\n");
        return;
    }
    #ifndef CC_VDEC_RM_SUPPORT
    ASSERT(prVdecEsInfo->u4VldId == ucUnuseVldId);
    #endif
    
#ifndef CC_VDEC_RM_SUPPORT
    if (prVdecEsInfo->u4VldId >= VDEC_MAX_VLD) 
    {
        LOG(0, "[%s-%d]ucVldId(%d) >= VDEC_MAX_VLD\n", __func__, __LINE__, prVdecEsInfo->u4VldId);
        return ;
    }
#else
    if ((prVdecEsInfo->u4VldId >= VDEC_MAX_VLD) && (prVdecEsInfo->u4Vld2Id >= VDEC_MAX_VLD))
    {
        LOG(0, "[%s-%d]ucVldId(%d) && ucVld2Id(%d) >= VDEC_MAX_VLD\n", __func__, __LINE__, prVdecEsInfo->u4VldId, prVdecEsInfo->u4Vld2Id);
        return ;
    }
#endif

    x_memset(aucVldId, VDEC_MAX_VLD, sizeof(aucVldId));

    aucVldId[ucTmpIdx++] = prVdecEsInfo->u4VldId;
    #ifdef CC_VDEC_RM_SUPPORT
    aucVldId[ucTmpIdx++] = prVdecEsInfo->u4Vld2Id;
    #endif

    for (ucTmpIdx = 0; ucTmpIdx < VDEC_MAX_VLD; ucTmpIdx++)
    {
        ucVldId = aucVldId[ucTmpIdx];
        if (ucVldId >= VDEC_MAX_VLD)
        {
            continue;
        }
        if(prVdecInfo->arVdecVldInfo[ucVldId].u4LockCount == 0)
        {
            LOG(0, "u4LockCount(0) should never happen\n");
            ASSERT(0);
        }
        
        if(prVdecInfo->arVdecVldInfo[ucVldId].ucLockEsId != ucEsId)
        {
            LOG(0, "ucLockEsId(%d) should be ucEsId(%d) ===========\n",
                    (UINT32)prVdecInfo->arVdecVldInfo[ucVldId].ucLockEsId,
                    (UINT32)ucEsId);
            ASSERT(0);
        }
    
        prVdecInfo->arVdecVldInfo[ucVldId].u4LockCount--;
        if(prVdecInfo->arVdecVldInfo[ucVldId].u4LockCount == 0)
        {
            prVdecInfo->arVdecVldInfo[ucVldId].ucLockEsId = 0xFF;
#ifdef TEST_ONOFF_RUNTIME
#ifdef CC_DYNAMIC_POWER_ONOFF  //power off
            if((ucVldId == VLD0) && (_fgTestOnoff))
            {
#ifdef VDEC_IS_POST_MT5368
                //VDEC_CkgenOnOff(FALSE);
                IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_SPEC, VDEC_PDN_CON_OFF);
                IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), RW_VDEC_MISC_PDN_CON_MODULE, VDEC_PDN_CON_OFF);
#else
                IO_WRITE32(VDEC_CTRL_BASE, VDEC_PWR_CTRL, VDEC_PWR_OFF);
#endif
            }
#endif
#endif
        }
        prVdecInfo->uLastEsId=ucEsId;
        VERIFY(x_sema_unlock(prVdecInfo->arVdecVldInfo[ucVldId].hVldSemaphore) == OSR_OK);
    }

    UNUSED(prVdecEsInfo);
}


VOID _VDEC_CheckLockStatus(UCHAR ucVldId)
{
#ifdef TEST_ONOFF_RUNTIME
    VDEC_INFO_T *prVdecInfo;
    prVdecInfo = _VDEC_GetInfo();
    if(prVdecInfo->arVdecVldInfo[ucVldId].u4LockCount == 0)
    {
        LOG(0, "FixMe, can't access VLD/MC register before lock vld\n");
        ASSERT(0);
    }

    if(!prVdecInfo->arVdecVldInfo[ucVldId].fgSwrst)
    {
        LOG(0, "FIXME: can't access VDEC register before swrst\n");
        ASSERT(0);
    }
    if(!prVdecInfo->arVdecVldInfo[ucVldId].fgPowerOn)
    {
        LOG(0, "FIXME: can't access VDEC register before power on\n");
        ASSERT(0);
    }
#endif
    UNUSED(ucVldId);
}


BOOL _VDEC_CkgenSel(UCHAR ucEsId, ENUM_VDEC_FMT_T eCurFMT, UINT32 u4App)
{
    UNUSED(ucEsId);
    UNUSED(u4App);

#if defined(VDEC_IS_POST_MT5368)
    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
    #if defined(VDEC_IS_POST_MT5398)
        #if defined(CC_MT5890)
        if ((VDEC_LAE0 == ucEsId) || (VDEC_LAE1 == ucEsId))
        {
            IO_WRITE32(CKGEN_REG_LAE_SYS_CKCFG, 0, LAE_SYS_CK_SEL_FAST);
        }
        else
        {
        	if(BSP_GetIcVersion() >= IC_VER_5890_AC)
    		{
    			IO_WRITE32(CKGEN_REG_VDEC_SYS_CKCFG,0, VDEC_SYS_CK_SEL_FAST);
    		}
			else
			{
        	    IO_WRITE32(CKGEN_REG_VDEC_SYS_CKCFG,0, 0xC/*432M VDEC_SYS_CK_SEL_FAST*/);
			}
        }
        #else
        IO_WRITE32(CKGEN_REG_VDEC_SYS_CKCFG, 0 ,VDEC_SYS_CK_SEL_FAST);
        #endif
    #else
        IO_WRITE32(CKGEN_BASE, VDEC_SYS_CKCFG, VDEC_SYS_CK_SEL_FAST);
    #endif
        break;

    case VDEC_FMT_MPV:
    case VDEC_FMT_MP4:
    case VDEC_FMT_WMV:
    case VDEC_FMT_VP8:
        #if defined(VDEC_IS_POST_MT5398)
        IO_WRITE32(CKGEN_REG_VDEC_SYS_CKCFG, 0, VDEC_SYS_CK_SEL_MID);
        #else
        IO_WRITE32(CKGEN_BASE, VDEC_SYS_CKCFG, VDEC_SYS_CK_SEL_MID);
        #endif
        break;

    case VDEC_FMT_RV:
    case VDEC_FMT_AVS:
    case VDEC_FMT_VP6:
        #if defined(VDEC_IS_POST_MT5398)
        IO_WRITE32(CKGEN_REG_VDEC_SYS_CKCFG, 0, VDEC_SYS_CK_SEL_SLOW);
        #else
        IO_WRITE32(CKGEN_BASE, VDEC_SYS_CKCFG, VDEC_SYS_CK_SEL_SLOW);
        #endif
        break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        break;

    default:
        LOG(1, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break;
    }
#else
    UNUSED(eCurFMT);
#endif

    return TRUE;
}

BOOL _VDEC_SysClkSel(UCHAR ucEsId, ENUM_VDEC_FMT_T eCurFMT, UINT32 u4App)
{
    UNUSED(ucEsId);
    UNUSED(u4App);

#if defined(VDEC_IS_POST_MT5368)
    switch (eCurFMT)
    {
    case VDEC_FMT_H264:
    case VDEC_FMT_H264VER:
        #if defined(CC_MT5890)
        if ((VDEC_LAE0 == ucEsId) || (VDEC_LAE1 == ucEsId))
        {
            vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
        }
        else
        {           
            vVDecWriteMISC(ucEsId,VDEC_SYS_CLK_SEL,VDEC_SYS_FAST_CLK);
        }
        #else
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), VDEC_SYS_CLK_SEL, VDEC_SYS_FAST_CLK);
        #endif
        break;

    case VDEC_FMT_MPV:
    case VDEC_FMT_MP4:
    case VDEC_FMT_WMV:
    case VDEC_FMT_VP8:
        #if defined(CC_MT5890)
        vVDecWriteMISC(ucEsId, VDEC_SYS_CLK_SEL, VDEC_SYS_MID_CLK);
        #else
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), VDEC_SYS_CLK_SEL, VDEC_SYS_MID_CLK);
        #endif
        break;

    case VDEC_FMT_RV:
    case VDEC_FMT_AVS:
    case VDEC_FMT_VP6:
        IO_WRITE32((IO_VIRT + VDEC_MISC_BASE0), VDEC_SYS_CLK_SEL, VDEC_SYS_SLOW_CLK);
        break;

    case VDEC_FMT_MJPEG:
    case VDEC_FMT_RAW:
        break;

    default:
        LOG(1, "%s: unknown format %d\n", __FUNCTION__, eCurFMT);
        ASSERT(0);
        break;
    }
#else
    UNUSED(eCurFMT);
#endif

    return TRUE;
}


void _VDEC_ClearEsInfo(UCHAR ucEsId)
{
    CRIT_STATE_T _rCritState;
    VDEC_PES_INFO_T *prVdecPesInfo;
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        return;
    }
    if((!prVdecEsInfo) || (!prVdecEsInfoKeep))
    {
        LOG(1, "prVdecEsInfo/prVdecEsInfoKeep null\n");
        return;
    }

    x_memset((void*)prVdecEsInfo, 0, sizeof(VDEC_ES_INFO_T));

    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));

    // if other format want to use prMpvCounter
    // please move it out of this score.
    prVdecEsInfo->prMpvCounter = (PARAM_MPV_T*)&(_prDbase->rMpv[ucEsId]);
    x_memset((void*)prVdecEsInfo->prMpvCounter, 0, sizeof(PARAM_MPV_T));
    x_memset((void*)&prVdecEsInfoKeep->rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    prVdecPesInfo = _arPesInfoEsm;
    prVdecPesInfo += (UINT32)ucEsId;
    x_memset((void*)prVdecPesInfo, 0, sizeof(VDEC_PES_INFO_T));

    prVdecEsInfo->ucFbgId = FBM_FBG_ID_UNKNOWN;
    prVdecEsInfo->ucSubFbgId = FBM_FB_ID_UNKNOWN;
    prVdecEsInfo->ucFbId = FBM_FB_ID_UNKNOWN;
#ifdef CC_MPV_DEBLOCKING
    prVdecEsInfo->ucDbkFbId = FBM_FB_ID_UNKNOWN;
    prVdecEsInfo->ucDbkRefId = FBM_FB_ID_UNKNOWN;
#endif
    //default No AV Sync
    prVdecEsInfo->u4WaitDispTime = VDEC_WAIT_DISP_TIME;

    prVdecEsInfo->u4AvgPicSize = 0;//VDEC_AVG_PIC_SIZE;//the average picture size of some files is too small, the initialization is too large, it may cause FF abnormal

    prVdecEsInfo->ucVideoFmt = VDEC_DEFAULT_VIDEO_FORMAT;
    prVdecEsInfo->ucUnsupportProLvlCnt = 0;

    prVdecEsInfo->u4BigBLevel1 = VDEC_BIG_B_LEVEL1;
    prVdecEsInfo->u4BigBLevel2 = VDEC_BIG_B_LEVEL2;
    prVdecEsInfo->u4BigBLevel3 = VDEC_BIG_B_LEVEL3;
    prVdecEsInfo->u4BigBBaseCnt = VDEC_BIG_B_BASE_COUNT;

    // clean Vdec thumbnail count
    prVdecEsInfo->fgRenderPicCnt = 0;
    prVdecEsInfo->u4RenderPicCnt = 0;

    prVdecEsInfo->fgPvrPlayBack = FALSE;
    prVdecEsInfo->u1PvrPlayMode = IPBMode;

    //clean codec change variable
    prVdecEsInfo->fgCodecChange = FALSE;
    prVdecEsInfo->fgForceFlush = FALSE;
    prVdecEsInfo->fgMpvNoFlush = FALSE;

    prVdecEsInfo->u4VldId = VDEC_MAX_VLD;
    prVdecEsInfo->fgEnableUFO = FALSE;
    prVdecEsInfoKeep->fgPatternMode = FALSE;
#ifdef CC_VDEC_RM_SUPPORT
    prVdecEsInfo->u4Vld2Id = VDEC_MAX_VLD;
#endif
    prVdecEsInfo->u4RenderVDPId = 0xFF;
#ifdef VDEC_TIME_PROFILE
    UINT8 i = 0;

    for ( i=0; i<5; i++ )
    {
        prVdecEsInfo->au4DecTime[i][0] =  0x0;
        prVdecEsInfo->au4DecTime[i][1] =  0xFFFFFFFF;
        prVdecEsInfo->au4DecTime[i][2] =  0x0;
        prVdecEsInfo->au4DecTime[i][3] =  0x0;
        prVdecEsInfo->au4DecTime[i][4] =  0x0;
        prVdecEsInfo->au4DecTime[i][5] =  0x0;
        prVdecEsInfo->au4DecTime[i][6] =  0x0;
        prVdecEsInfo->au4DecTime[i][7] =  0x0;
        prVdecEsInfo->au4DecTime[i][8] =  0x0;

        prVdecEsInfo->au4DecTimeMaxPicSize[i][0] = 0;
    }
    prVdecEsInfo->u4FrmFldCnt = 0;
#endif
#if defined(CC_MAPLE_CUST_DRV) || defined(VDEC_TIME_PROFILE)
    prVdecEsInfo->fgCalEachPicSize = TRUE;
    prVdecEsInfo->u4CurrPicSize = 0;
    prVdecEsInfo->u4StatisticPicSize = 0;
#endif

    // in case swdmx control faster than mpv control, prVdecEsInfo might be clear and sync mode is lost
    prVdecEsInfo->ucSyncMode = prVdecEsInfoKeep->ucSyncMode;
    prVdecEsInfo->ucStcId = prVdecEsInfoKeep->ucStcId;

    if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV) && (ucEsId < MPV_MAX_ES))
    {
        _MPV_ClearEsInfo(ucEsId);
    }
}



void _VDEC_ClearInfo(UCHAR ucVldId)
{
#ifdef CC_VDEC_RM_SUPPORT
    VDEC_INFO_T *prVdecInfo = NULL;

    prVdecInfo = _VDEC_GetInfo();

    if (prVdecInfo)
    {
        prVdecInfo->arVdecVldInfo[ucVldId].ucConnTmpEsId = VDEC_MAX_ES;
        
        prVdecInfo->arVdecVldInfo[ucVldId].u4ConnEsIdMask = 0;
        prVdecInfo->arVdecVldInfo[ucVldId].u1TotalLoading = 0;
        x_memset(prVdecInfo->arVdecVldInfo[ucVldId].au1EsLoading, 0, sizeof(prVdecInfo->arVdecVldInfo[ucVldId].au1EsLoading));
    }
#endif
}


void _VDEC_InitInfo(void)
{
    UCHAR ucIdx;

    for (ucIdx = 0; ucIdx < VDEC_MAX_ES; ucIdx++)
    {
        _VDEC_ClearEsInfo(ucIdx);
    }
    for (ucIdx = 0; ucIdx < MPV_MAX_ES; ucIdx++)
    {
        _MPV_ClearMpvCc(ucIdx);
        _MPV_VparserReloadEs(ucIdx);
    }
    for (ucIdx = 0; ucIdx < VDEC_MAX_VLD; ucIdx++)
    {
        _VDEC_ClearInfo(ucIdx);
    }
}


void _VDEC_FlushEs(UCHAR ucEsId)
{
    UCHAR ucMpvId;
    UINT16 u2MsgQIdx;
    UINT32 u4Cnt;
    SIZE_T zMsgSize;
    UINT16 u2Count;
    
    VDEC_PES_INFO_T rPesInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
#ifdef CC_DUAL_CORE_DEC
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeepLae;
    VDEC_ES_INFO_T* prVdecEsInfoLae;
    VDEC_LAE_DecPrm rLAEDecPrm[2];
    VDEC_LAE_DecPrm *pLaeDecPrm;
    UINT32 u4LaeQCnt = 0,u4LaeCnt = 0;
    INT32 i4Ret;
#endif

    ASSERT(ucEsId < VDEC_MAX_ES);

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        return;
    }

    // prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = &_rVdecInfo.arVdecEsInfo[ucEsId];
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);

    u4Cnt = 0;

    zMsgSize = sizeof(VDEC_PES_INFO_T);
    while (x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
        &(prVdecEsInfoKeep->hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
        ucMpvId = rPesInfo.ucMpvId;

        // Handle Virtual Picture Header for Event Trigger
        if (ucMpvId == VDEC_MAX_VLD)
        {
            u4Cnt++;
        }
    }

    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    while (x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
        &(prVdecEsInfoKeep->hMsgQSub), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
    }
    #endif
    #ifdef CC_SUPPORT_VDEC_PREPARSE
    while (x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
        &(prVdecEsInfoKeep->hMsgQPreParse), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
    }
    #endif
#ifdef CC_DUAL_CORE_DEC 
    prVdecEsInfoKeepLae = _VDEC_GetEsInfoKeep(prVdecEsInfo->ucLaeEsInfoId);
    prVdecEsInfoLae = _VDEC_GetEsInfo(prVdecEsInfo->ucLaeEsInfoId);
    if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
    {
        //clear LAE thread esm Q
        u4LaeCnt = 0;
        while (x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                &(prVdecEsInfoKeepLae->hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
            ucMpvId = rPesInfo.ucMpvId;

            // Handle Virtual Picture Header for Event Trigger
            if (ucMpvId == VDEC_MAX_VLD)
            {
                u4LaeCnt++;
            }
        }
        
        LOG(2,"VDEC [%d] clear LAE ES msg Q,Rest of cmd %d\n",ucEsId,u4LaeCnt);
    }
    
    if(prVdecEsInfo->ucDualCoreMode)
    {
        LOG(1,"clear LAE Dec msg Q\n");
        //clear LAE buffer input Q
        pLaeDecPrm = &rLAEDecPrm[0];
        zMsgSize = sizeof(VDEC_LAE_DecPrm);
        while (x_msg_q_receive(&u2MsgQIdx, (void*)pLaeDecPrm, &zMsgSize,
                &(prVdecEsInfoKeepLae->hMsgQLaeInput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
            LOG(1,"Flush1 LAE[%d] 0x%x 0x%x\n",u4LaeQCnt,pLaeDecPrm->u4CabacAddr,pLaeDecPrm->u4ECAddr);
            pLaeDecPrm ++;
            u4LaeQCnt ++;
        }

        zMsgSize = sizeof(VDEC_LAE_DecPrm);
        //clear LAE buffer output Q
        while (x_msg_q_receive(&u2MsgQIdx, (void*)pLaeDecPrm, &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeOuput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
            LOG(1,"Flush2 LAE[%d] 0x%x 0x%x\n",u4LaeQCnt,pLaeDecPrm->u4CabacAddr,pLaeDecPrm->u4ECAddr);
            pLaeDecPrm ++;
            u4LaeQCnt ++;
        }

        while(u4LaeQCnt > 0)
        {
            pLaeDecPrm = &rLAEDecPrm[u4LaeQCnt - 1];
            i4Ret = x_msg_q_send(prVdecEsInfoKeepLae->hMsgQLaeInput, (void *)pLaeDecPrm,
                sizeof(VDEC_LAE_DecPrm), 255);
            if(i4Ret != OSR_OK)
            {
                LOG(0,"LAE put msgQ fail[%d]\n",i4Ret);
            }
            else
            {
                LOG(1,"Send lae Q 0x%x 0x%x\n",pLaeDecPrm->u4CabacAddr,pLaeDecPrm->u4ECAddr);
            }
            u4LaeQCnt --;
        }
    }
#endif
    // DTV00136850 Video display discontinuously after  plug supported CAM module at TV in Menu
    // video frame are not correct release.
    if(prVdecEsInfo->fgMMPlayback)
    {
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            //flush dpb
            #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
            #ifndef CC_VDEC_H264_DISABLE
            LOG(3, "Flush VDEC_FMT_H264, fgVPush:%d\n", prVdecEsInfoKeep->fgVPush);
            
            _VDEC_H264Flush(ucEsId, FALSE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);                
            #ifdef CC_DUAL_CORE_DEC
                if(prVdecEsInfo->ucDualCoreMode)
                {
                    SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_NEXT_I);
                    SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_AVC_RAP);
                }       
            #endif
            }
            if((prVdecEsInfo->e3DType == VDEC_3D_MVC) || (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
            {
                LOG(4,"Start from Base-View[%d] Org view %d\n",ucEsId,prVdecEsInfo->fgIsDependView);
                prVdecEsInfo->fgIsDependView = FALSE;
            }
            #endif
            #endif
        }
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
        {
            #ifndef CC_VDEC_H265_DISABLE
            vVDecH265FlushDecInfo(ucEsId);
            vVDecH265Flush(ucEsId,FALSE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
            }
            //Set lae thread decode from netx RAP frame
            #ifdef CC_DUAL_CORE_DEC
            if(prVdecEsInfo->ucDualCoreMode)
            {
                SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_NEXT_I);
            }       
            #endif
            #endif
        }
        else if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_AVS)
        {
#ifndef CC_VDEC_AVS_DISABLE
            _VDEC_AVSFlush(ucEsId, TRUE);
#endif
        }
        else if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_VP6)
        {
#if defined(CC_VP6_SUPPORT)
#ifndef CC_VDEC_VP6_DISABLE
            _VDEC_VP6Flush(ucEsId, TRUE);
#endif
#endif
        }
        else if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_VP8)
        {
           if(prVdecEsInfoKeep->fgVPush)
           {
             SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
           }
           prVdecEsInfoKeep->pfnVdecFlush(ucEsId,TRUE);
        }
        else if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_VP9)
        {
           if(prVdecEsInfoKeep->fgVPush)
           {
             SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
           }
#ifndef CC_VDEC_VP9_DISABLE
            _VDEC_Vp9Flush(ucEsId, TRUE);
#endif
        }
        else
        {
            UCHAR ucFRefFbId, ucBRefFbId;
            FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
            if(ucBRefFbId != FBM_FB_ID_UNKNOWN)
            {
                if((FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_READY))
                   ||
                   (FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_DECODE)))
                {
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucBRefFbId, FBM_FB_STATUS_EMPTY);
                    LOG(3, "Flush ucBRefFbId %d -> empty\n", ucBRefFbId);
                }
            }
            if(ucFRefFbId != FBM_FB_ID_UNKNOWN)
            {
                if((FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_READY))
                   ||
                   (FBM_CheckFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_DECODE)))
                {
                    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFRefFbId, FBM_FB_STATUS_EMPTY);
                    LOG(3, "Flush ucFRefFbId %d -> empty\n", ucFRefFbId);
                }
            }
            if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK))
            {
                 
                 if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                 {
                      LOG(3, "Flush prVdecEsInfo->ucDbkFbId %d -> empty\n", prVdecEsInfo->ucDbkFbId);                    
                      LOG(3, "Flush  prVdecEsInfo->ucDbkRefId %d -> empty\n",  prVdecEsInfo->ucDbkRefId);
                      LOG(3, "Flush prVdecEsInfo->ucPicType %d\n",prVdecEsInfo->ucPicType);
                                        
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
            // because we invalidate reference frame, we should restart to get a sync.
            prVdecEsInfo->fgDetectIntra = TRUE;
        }
        
        if(prVdecEsInfo->prMpvCounter)
        {
            prVdecEsInfo->prMpvCounter->u4RetrieveNs = 0;
        }
        
        if(prVdecEsInfo->u4VFifoPrevWptr)
        {
            if (!VDEC_SetRptr(ucEsId, 
                prVdecEsInfo->u4VFifoPrevWptr, prVdecEsInfo->u4VFifoPrevWptr))                
            {                    
                LOG(6, "(L:%d) _VDEC_FlushEs ES(%d) VDEC_SetRptr Fail\n",
                    __LINE__, ucEsId);
            }
        }

        //LOG(3, "Flush VDEC_FMT_H264, DEC_FLG_WAIT_NEXT_I\n");
        
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);       
    #ifdef CC_DUAL_CORE_DEC
        if(prVdecEsInfo->ucDualCoreMode)
        {
            SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_NEXT_I);
        }       
    #endif
        #if 1 // add for fix 448151 @2012/11/12
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);             
        #ifdef CC_DUAL_CORE_DEC
            if(prVdecEsInfo->ucDualCoreMode)
            {
                SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_AVC_RAP);
            }       
        #endif
        }
        #endif
    }
    else
    {
        // DTV case
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
        {
            SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
            if(prVdecEsInfo->u4VFifoPrevWptr)
            {
                prVdecEsInfo->u4VldReadPtr = prVdecEsInfo->u4VFifoPrevWptr;
                prVdecEsInfo->u4DecReadPtr = prVdecEsInfo->u4VldReadPtr;
                if(prVdecEsInfoKeep->pfnVdecUpdRptr)
                {
                    prVdecEsInfoKeep->pfnVdecUpdRptr(ucEsId);
                }
            }
        }
        #if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
        #ifndef CC_VDEC_H264_DISABLE
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            //flush dpb
            _VDEC_H264Flush(ucEsId, FALSE);
            //if(prVdecEsInfoKeep->fgVPush)
            {
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
            }
            #ifdef CC_DUAL_CORE_DEC
            if(prVdecEsInfo->ucDualCoreMode)
            {
                SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_NEXT_I);
                SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_AVC_RAP);
            }       
            #endif
            if(prVdecEsInfo->u4VFifoPrevWptr)
            {
                if (!VDEC_SetRptr(ucEsId, prVdecEsInfo->u4VFifoPrevWptr, prVdecEsInfo->u4VFifoPrevWptr))
                {
                    LOG(2,"ucEsId %d, prVdecEsInfo->u4VFifoPrevWptr %d\n", ucEsId,prVdecEsInfo->u4VFifoPrevWptr);
                }
            }
        }
        #endif
        #endif
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265)
        {
            #ifndef CC_VDEC_H265_DISABLE
            vVDecH265Flush(ucEsId,FALSE);            
            //Set lae thread decode from netx RAP frame
            #ifdef CC_DUAL_CORE_DEC
            if(prVdecEsInfo->ucDualCoreMode)
            {
                SET_DECFLAG(prVdecEsInfoLae, DEC_FLG_WAIT_NEXT_I);
            }
            #endif
            #endif
        }
    }

    prVdecEsInfo->fgRefInfo = FALSE;
    prVdecEsInfo->fgKeepPes = FALSE;
    prVdecEsInfo->fgSendedEos = FALSE;
    prVdecEsInfo->fgSetStartPts = FALSE;
    prVdecEsInfo->u4PTSLast = 0;
    prVdecEsInfo->u8PTSLast = 0;
    prVdecEsInfo->u4PTSLastNoneZero= 0;
    prVdecEsInfo->u8OffsetLast = 0;
    prVdecEsInfo->u4DisplayQPicCnt = 0;
    prVdecEsInfo->u4FirstDispPTS = 0;
    prVdecEsInfo->ucPicTypeLast = UNKNOWN_TYPE;
    prVdecEsInfoKeep->rPesInfo.fgEos = FALSE;
    prVdecEsInfo->ucDropBNs = 0;
    prVdecEsInfo->u4FrameNumInEsmQ = 0;
    prVdecEsInfo->ucUnsupportProLvlCnt = 0;

#ifdef __MODEL_slt__
    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
    {
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    }
#endif

    VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
    // one msg come from VERIFY(VDEC_SendEs(&_arMpvPesInfo[ucEsId]) == 1) in MPV_FlushEsmQ.
    if(prVdecEsInfo->u2Count == 1)
    {
        LOG(3, "After Flush u2Count = 1\n");
    }
    else
    {
        LOG(5, "After Flush u2Count = %d\n", prVdecEsInfo->u2Count);
        //ASSERT(prVdecEsInfo->u2Count == 0);
    }
    LOG(5, "Es%d Flush\n",ucEsId);
    SYSLOG(50, ucEsId, 0, 0);

    VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hCmdQ, &(u2Count)) == OSR_OK);
    if (prVdecEsInfo->prMpvCounter)
    {
        prVdecEsInfo->prMpvCounter->u4EsmQCnt = (UINT32)u2Count;
    }
    // LOG(5, "Es%d Flush, u4Cnt:%d, u2Count:%d\n", ucEsId, u4Cnt, u2Count);
        
    if (u4Cnt > u2Count)
    {
        u4Cnt = u2Count;
    }

    // rPesInfo should reset or we will have the last one's information
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    while (u4Cnt)
    {
        rPesInfo.ucEsId = ucEsId;
        rPesInfo.ucMpvId = VDEC_MAX_VLD;
        VERIFY(VDEC_SendEs(&rPesInfo) == 1);
        u4Cnt--;
    }

    #ifdef CC_DUAL_CORE_DEC
    VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeepLae->hCmdQ, &(u2Count)) == OSR_OK);  
    u4LaeCnt = (u4LaeCnt > u2Count) ? u2Count : u4LaeCnt;
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    while (u4LaeCnt)
    {
        rPesInfo.ucEsId = prVdecEsInfo->ucLaeEsInfoId;
        rPesInfo.ucMpvId = VDEC_MAX_VLD;
        VERIFY(VDEC_SendEs(&rPesInfo) == 1);
        u4LaeCnt--;
    }
    #endif
}


#ifdef MPV_VIRTUAL_PRS

static void _MpvVirutualPrsRoutine(void* pvArg)
{
    VDEC_PES_INFO_T rPesInfo;
    BOOL fgStop[VDEC_MAX_ES] = {FALSE};
    UINT32 u4OnRepeatCount = 0;
    UINT32 u4RoundCount = 0;
    //HANDLE_T hThread;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_INFO_T *prVdecInfo;

    prVdecInfo = _VDEC_GetInfo();

    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));

    while (!_fgMpvVirtualPrsDestroy)
    {
        UCHAR ucEsIdx;

        for (ucEsIdx = 0; ucEsIdx < _ucMpvVPEsNs; ucEsIdx++)
        {
            prVdecEsInfo = &_rVdecInfo.arVdecEsInfo[ucEsIdx];
            if (!fgStop[ucEsIdx])
            {
                if (prVdecEsInfo->u2Count > (MPV_MAX_PES_NS_PER_QUEUE / 2))
                {
                    x_thread_delay(_ucMpvVirtualPrsDelay);
                }
                else
                {
                    rPesInfo.fgNonBuffer = TRUE;
                    rPesInfo.u4FifoStart = _u4MpvVpStartAddr[ucEsIdx];
                    rPesInfo.u4FifoEnd = ~0;
                    rPesInfo.u4FifoEnd = 0x3ffffffe;
                    rPesInfo.u4VldReadPtr =
                        _u4MpvVpAddr[ucEsIdx][_u4MpvVpPicCount[ucEsIdx]];
                    rPesInfo.ucEsId = _ucMpvVpEsId[ucEsIdx];
                    rPesInfo.ucMpvId = _ucMpvVpMpvId[ucEsIdx];
                    rPesInfo.ucPicType =
                        _u4MpvVpPicTp[ucEsIdx][_u4MpvVpPicCount[ucEsIdx]];

                    VDEC_SendEs(&rPesInfo);

                    #ifdef MPV_SEMI_HOST
                    {
                        if (prVdecInfo->fgSave)
                        {
                            if ((_u4MpvVpPicCount[ucEsIdx] + 1) == _u4MpvVpPicNs[ucEsIdx])
                            {
                                #if 0
                                fgStop[ucEsIdx] = TRUE;
                                _fgMpvVirtualPrsDestroy = TRUE;
                                #else
                                while(1)
                                {
                                    x_thread_delay(10000);
                                    LOG(1, "Wait for decode finish\n");
                                    if(!prVdecInfo->fgSave)
                                    {
                                        break;
                                    }
                                }
                                ASSERT(0);
                                #endif
                            }
                        }
                    }
                    #endif

                    if (!fgStop[ucEsIdx])
                    {
                        _u4MpvVpPicCount[ucEsIdx] = (_u4MpvVpPicCount[ucEsIdx] + 1) % _u4MpvVpPicNs[ucEsIdx];
                        if(_u4MpvVpPicCount[ucEsIdx] == 0)
                        {
                            LOG(1, "Parse Round %d\n", u4RoundCount++);
                        }

                        #ifdef MPV_REPEAT_TEST
                        if(_u4MpvVpPicCount[ucEsIdx] > MPV_PARSE_REPEAT_END)
                        {
                            _u4MpvVpPicCount[ucEsIdx] = MPV_PARSE_REPEAT_START;
                            prVdecEsInfo->fgParseOnRepeat = 1;
                        }
                        if(prVdecEsInfo->fgParseOnRepeat)
                        {
                            LOG(1, "Parse On Repeat %d\n", u4OnRepeatCount++);
                        }
                        #endif
                        UNUSED(u4OnRepeatCount);
                    }
                }
            }
        }
    }

    _fgMpvVirtualPrsStart = FALSE;
    LOG(1, "VirtualPrs Exit\n");

    // destroy thread
    x_thread_exit();

}

#endif

//-----------------------------------------------------------------------------
// VDEC_RTSendEsInterval
// fgGet = FALSE: Caclulate the interval between get two frames from dmx.
// fgGet = TRUE: get the MaxInterval as Real Time Mode, the Min Buffer for playback smooth.
//-----------------------------------------------------------------------------
#ifdef ENABLE_MULTIMEDIA

UINT32 VDEC_RTSendEsInterval(BOOL fgGet)
{
    static UINT32 u4MaxInterval = 3000;
    static UCHAR  ucOverMaxCnt = 0;
    static UCHAR  ucOverMaxMinusCnt = 0;
    static UINT32 u4EsCnt = 0;
    static HAL_TIME_T rLastTime;

    static UINT32 u4LastGetMax = 0;
    
    HAL_TIME_T rDeltaTime;
    HAL_TIME_T rCurTime;

    if(fgGet)
    {
        if(u4LastGetMax != u4MaxInterval)
        {
            LOG(2, "RT: return Inter(%d) changed.\n", u4MaxInterval);
            u4LastGetMax = u4MaxInterval;
        }
        return u4MaxInterval;//1s=90000 --> 9/100=1us
    }
    
    HAL_GetTime(&rCurTime);
    HAL_GetDeltaTime(&rDeltaTime, &rLastTime, &rCurTime);
    //LOG(4, "ESTime Last(%03dms) Cur(%03dms) DeltaPts(0x%08x).\n", rLastTime.u4Micros/1000, rCurTime.u4Micros/1000, rLastTime.u4Seconds*90 + rDeltaTime.u4Micros*9/100);
    x_memcpy(&rLastTime, &rCurTime, sizeof(HAL_TIME_T));
    
    if (rDeltaTime.u4Seconds) //ignore delta time > 1.0s case
    {
        return 0;
    }

    if (rDeltaTime.u4Micros*9/100 > u4MaxInterval)
    {
        ucOverMaxCnt++;
        if(ucOverMaxCnt > 15)
        {
            LOG(1, "Inter(%d) OverCnt(%d, %d).\n", u4MaxInterval, ucOverMaxCnt, ucOverMaxMinusCnt);
            u4MaxInterval += 300;
            ucOverMaxCnt = ucOverMaxMinusCnt = u4EsCnt = 0;
            return 0;
        }
    }
    if (rDeltaTime.u4Micros*9/100 > u4MaxInterval - 300)
    {
        ucOverMaxMinusCnt++;
    }
    
    if (++u4EsCnt > 150)
    {
        LOG(1, "Inter(%d) OverCnt(%d, %d).\n", u4MaxInterval, ucOverMaxCnt, ucOverMaxMinusCnt);
        u4MaxInterval += ((ucOverMaxCnt > 15) ? 300 : ((ucOverMaxMinusCnt < 20) ? -300 : 0));
        ucOverMaxCnt = 0;
        ucOverMaxMinusCnt = 0;
        u4EsCnt = 0;
    }
    
    return 0;
}

#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

UCHAR VDEC_SendEs(const VDEC_PES_INFO_T*prPesInfo)
{
    BOOL fgGetIframeTime = FALSE;
    UCHAR ucValid;
    UCHAR ucEsId;
    UCHAR ucMpvId;
    INT32 i4Ret, u4OverflwThrd;
    //ESMQ_T* prEsmQ;
    //MPV_ES_INFO_T* prEsInfo;
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_STATUS_CB_T rStatusCB;
    PARAM_MPV_T *prMpvCounter;
    HANDLE_T hMsgQ;
#ifdef CC_DUAL_CORE_DEC
    INT32 i4RetLae;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeepLae;
    VDEC_ES_INFO_T* prVdecEsInfoLae;
    HANDLE_T hMsgQLae;
    VDEC_PES_INFO_T rPesInfoLae;
#endif
#ifdef CC_SUPPORT_VDEC_PREPARSE
    HANDLE_T hMsgQPreParse;
#endif
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeepMain;
    #endif
    
    ASSERT(prPesInfo != NULL);
    ucMpvId = prPesInfo->ucMpvId;
    ucEsId = prPesInfo->ucEsId;
    ASSERT(ucEsId < VDEC_MAX_ES);

    if(ucEsId >= VDEC_MAX_ES)
    {
        LOG(1, "ucEsId >= VDEC_MAX_ES\n");
        return 1;
    }

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (NULL == prVdecEsInfo)
    {
        return 1;
    }
    
#ifdef CC_DUAL_CORE_DEC
    //prVdecEsInfoKeepLae = _VDEC_GetEsInfoKeep(prVdecEsInfo->ucLaeEsInfoId);
    //prVdecEsInfoLae = _VDEC_GetEsInfo(prVdecEsInfo->ucLaeEsInfoId);
    prVdecEsInfoKeepLae = _VDEC_GetEsInfoKeep(VDEC_MAX_ES - 1 - ucEsId);
    prVdecEsInfoLae = _VDEC_GetEsInfo(VDEC_MAX_ES - 1 - ucEsId);
    hMsgQLae = prVdecEsInfoKeepLae->hMsgQ;
#endif
    prVdecInfo = &_rVdecInfo;
    prMpvCounter = (PARAM_MPV_T *)prVdecEsInfo->prMpvCounter;

    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    prVdecEsInfoKeepMain = _VDEC_GetEsInfoKeep(MVC_MAIN_VIDEO);
    #endif

    #ifdef ENABLE_MULTIMEDIA
    if ((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY) &&
        (prVdecEsInfo->u4LatestPTS != prPesInfo->u4PTS))
    {
        VDEC_RTSendEsInterval(FALSE);
    }
    #endif
    
    if(prPesInfo->u4PTS != 0)
    {
        prVdecEsInfo->u4LatestPTS = prPesInfo->u4PTS;
    }
    if(prPesInfo->u8PTS != 0)
    {
        prVdecEsInfo->u8LatestPTS = prPesInfo->u8PTS;
    }

#ifdef CC_SUPPORT_VDEC_PREPARSE
    hMsgQPreParse = prVdecEsInfoKeep->hMsgQPreParse;
#endif
    // Virtual Picture Header for Event Trigger
    if (ucMpvId >= VDEC_MAX_VLD)
    {
        VERIFY(x_msg_q_num_msgs(prVdecEsInfoKeep->hMsgQ, &(prVdecEsInfo->u2Count))
            == OSR_OK);
        //if (prEsmQ->u2Count <= prEsmQ->u2UdfNs)
        i4Ret = x_msg_q_send(prVdecEsInfoKeep->hMsgQ, (void *)prPesInfo,
            sizeof(VDEC_PES_INFO_T), 254);

        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        if(((prVdecEsInfo->e3DType == VDEC_3D_MVC) ||
            (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
            && (ucEsId == 0))
        {
            i4Ret |= x_msg_q_send(prVdecEsInfoKeepMain->hMsgQSub, (void *)prPesInfo,
                sizeof(VDEC_PES_INFO_T), 254);
        }
        #endif
        if (i4Ret != OSR_OK)
        {
            if (i4Ret == OSR_TOO_MANY)
            {
                LOG(3, "EsQ full\n");
            }
            else
            {
                ASSERT(0);
            }
        }
#ifdef CC_SUPPORT_VDEC_PREPARSE
        if (prVdecEsInfo->ePreParseStatus != VDEC_PREPS_ST_DONE)
        {                
            i4Ret = x_msg_q_send(hMsgQPreParse, (void *)prPesInfo,
                sizeof(VDEC_PES_INFO_T), 254);
        }
#endif

        VERIFY(x_sema_unlock(prVdecEsInfoKeep->hUnderFlowSema) == OSR_OK);
        return 1;
    }

    ASSERT(ucMpvId < VDEC_MAX_VLD);

    //redirect to single ESM for MVC
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if((prVdecEsInfo->e3DType == VDEC_3D_MVC) ||
        (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
    {
        if(ucEsId > 0)  // es case will not have EsId > 0, Sub Q is useless
        {
            hMsgQ = prVdecEsInfoKeepMain->hMsgQSub;
#ifdef CC_SUPPORT_VDEC_PREPARSE
            //Not send ES to Sub ES Pre-ParserQ
            prVdecEsInfo->ePreParseStatus = VDEC_PREPS_ST_DONE;
#endif
        }
        else
        {
            hMsgQ = prVdecEsInfoKeep->hMsgQ;
        }
        //error handling
    }
    else
    #endif
    {
        hMsgQ = prVdecEsInfoKeep->hMsgQ;
    }

    if(prVdecEsInfo->fgThumbMethod2)
    {
        if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
        {
            if((prPesInfo->ucPicType & 0xF) == 0x5) // IDR_NALU
            {
                fgGetIframeTime = TRUE;
            }
        }
        else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MPV)
        {
            if(prPesInfo->ucPicType == I_TYPE)
            {
                fgGetIframeTime = TRUE;
            }
        }
        else
        {
            fgGetIframeTime = TRUE;
        }
        // error handle
        if(prMpvCounter)
        {
            if((prMpvCounter->u4RetrieveNs - prVdecEsInfo->u4ThumbRetrieveNs) > 200)
            {
                fgGetIframeTime = TRUE;
            }
        }
        if(fgGetIframeTime)
        {
            if(!prVdecEsInfo->fgThumbStartTime)
            {
                HAL_GetTime(&prVdecEsInfo->rThumbStartTime);
                LOG(5, "Es(%d) Thumbnail First I Pic. Time(%d.%06d)\n", ucEsId,
                    prVdecEsInfo->rThumbStartTime.u4Seconds,
                    prVdecEsInfo->rThumbStartTime.u4Micros);
                prVdecEsInfo->fgThumbStartTime = TRUE;
            }
        }
    }

    // [LOG] Parser send message into ESM Q
    SYSLOG(ucEsId, 1, prVdecEsInfo->u2Count, prPesInfo->u4PTS);
    //SYSLOG(ucEsmQId, 1, prPesInfo->u4PTS, prPesInfo->u4DTS);

#ifdef MPV_VIRTUAL_PRS
    VERIFY(x_msg_q_send(hMsgQ, (void *)prPesInfo,
        sizeof(VDEC_PES_INFO_T), 255) == OSR_OK);
    VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
#else
    if(prVdecEsInfoKeep->pfnVdecWaitRandomAP)
    {
        if(prVdecEsInfoKeep->pfnVdecWaitRandomAP((void *)prPesInfo))
        {
            return 1;   //waiting first random access point
        }
    }

    if(prVdecEsInfo->fgMMPlayback && prVdecEsInfo->ucSequenceHdrFound)
    {
        // MM case, not allow drop
        INT32 i4Count = 0;
        while(1)
        {
            VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
            if(prMpvCounter)
            {
                prMpvCounter->u4EsmQCnt = (UINT32)prVdecEsInfo->u2Count;
            }
            if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
            {
                u4OverflwThrd = H264_MAX_PES_NS_PER_QUEUE-1;
            }
            else
            {
                u4OverflwThrd = MPV_MAX_PES_NS_PER_QUEUE-1;
            }
            if(prVdecEsInfo->u2Count >= (UINT16)u4OverflwThrd)
            {
                LOG(0,"VDEC Queue Size if full!!!!!!!!!\n");
                //x_thread_delay(33);
                i4Count++;
                if(i4Count > 30)
                {
                    UCHAR ucRet = 0;
                    LOG(7, "MM x_thread_delay(33)\n");

                    if(prVdecEsInfoKeep->pfnVdecHandleOverFlw)
                    {
                        LOG(5, "fgMMPlayback MM pfnVdecHandleOverFlw\n");
                        ucRet =  prVdecEsInfoKeep->pfnVdecHandleOverFlw(ucMpvId, ucEsId,
                            prVdecEsInfoKeep->hMutex, hMsgQ, (void*)prPesInfo);
                    }
                    return ucRet;
                }
            }
            else
            {
                break;
            }
        }
    }
    if((prPesInfo->ucMpvId < VDEC_MAX_VLD) &&
       (!prPesInfo->fgEos) &&
       (!prPesInfo->fgMoveComplete))
    {
        prVdecEsInfo->ucPicTypeLast = prPesInfo->ucPicType;
    }

    MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDEC_FRAME_FIRST);

    if(prPesInfo->fgNonBuffer)
    {
        if (prVdecEsInfo->fgKeepPes)
        {
            LOG(1, "fgNonBuffer and fgKeepPes, should not happen\n");
        }
        // direct send
        i4Ret = x_msg_q_send(hMsgQ, (void *)prPesInfo,
            sizeof(VDEC_PES_INFO_T), 255);    
        
#ifdef CC_SUPPORT_VDEC_PREPARSE
        if (prVdecEsInfo->ePreParseStatus != VDEC_PREPS_ST_DONE)
        {                
            i4Ret = x_msg_q_send(hMsgQPreParse, (void *)prPesInfo,
                sizeof(VDEC_PES_INFO_T), 255);
        }
#endif
        #ifdef CC_DUAL_CORE_DEC
        if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
        {
            x_memcpy(&rPesInfoLae,prPesInfo,sizeof(VDEC_PES_INFO_T));
            rPesInfoLae.ucMpvId = prVdecEsInfo->ucLaeId;
            
            i4RetLae = x_msg_q_send(hMsgQLae, (void *)&rPesInfoLae,
                        sizeof(VDEC_PES_INFO_T), 255);
            //Add error-handle ??? mtk40343
        }
        #endif
        if (i4Ret != OSR_OK)
        {
            UCHAR ucRet = 0;

            if(prVdecEsInfoKeep->pfnVdecHandleOverFlw)
            {
                ucRet =  prVdecEsInfoKeep->pfnVdecHandleOverFlw(ucMpvId, ucEsId,
                    prVdecEsInfoKeep->hMutex, hMsgQ, (void*)&_arPesInfoEsm[ucEsId]);
                x_memcpy(&_arPesInfoEsm[ucEsId], prPesInfo, sizeof(VDEC_PES_INFO_T));
            }
            return ucRet;
        }
    }
    else if((!prVdecEsInfo->fgKeepPes) && (prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY_I_FRAME))
    {
        if (prVdecEsInfo->fgMMPlayback && prPesInfo->fgEos)
        {
            // we don't get any frame before Eos
            x_memcpy((void *)&_arPesInfoEsm[ucEsId], (void *)prPesInfo, sizeof(VDEC_PES_INFO_T));
            _arPesInfoEsm[ucEsId].ucMpvId = VDEC_MAX_VLD;
            i4Ret = x_msg_q_send(hMsgQ, (void *)&_arPesInfoEsm[ucEsId],
                sizeof(VDEC_PES_INFO_T), 255);
            #ifdef CC_DUAL_CORE_DEC
            if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
            {
                x_memcpy(&rPesInfoLae,(void *)&_arPesInfoEsm[ucEsId],sizeof(VDEC_PES_INFO_T));
                rPesInfoLae.ucMpvId = VDEC_MAX_VLD;
                
                i4RetLae = x_msg_q_send(hMsgQLae, (void *)&rPesInfoLae,
                            sizeof(VDEC_PES_INFO_T), 255);
            }
            #endif
            #ifdef CC_SUPPORT_VDEC_PREPARSE
            if (prVdecEsInfo->ePreParseStatus != VDEC_PREPS_ST_DONE)
            {                
                i4Ret = x_msg_q_send(hMsgQPreParse, (void *)&_arPesInfoEsm[ucEsId],
                    sizeof(VDEC_PES_INFO_T), 255);
            }
            #endif
        }
        else
        {
            x_memcpy(&_arPesInfoEsm[ucEsId], prPesInfo, sizeof(VDEC_PES_INFO_T));
            prVdecEsInfo->fgKeepPes = TRUE;
        }
    }
    else
    {
        i4Ret = OSR_OK;
        if(prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY_I_FRAME)
        {
            if (prVdecEsInfo->fgMMPlayback && _arPesInfoEsm[ucEsId].fgSeqEnd)
            {
                if(prPesInfo->fgEos)
                {
                    // we don't get any frame before Eos
                    x_memcpy((void *)&_arPesInfoEsm[ucEsId], (void *)prPesInfo, sizeof(VDEC_PES_INFO_T));
                    _arPesInfoEsm[ucEsId].ucMpvId = VDEC_MAX_VLD;
                    i4Ret = x_msg_q_send(hMsgQ, (void *)&_arPesInfoEsm[ucEsId],
                        sizeof(VDEC_PES_INFO_T), 255);
                    #ifdef CC_DUAL_CORE_DEC
                    if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
                    {
                        x_memcpy(&rPesInfoLae,(void *)&_arPesInfoEsm[ucEsId],sizeof(VDEC_PES_INFO_T));
                        rPesInfoLae.ucMpvId = VDEC_MAX_VLD;
                        
                        i4RetLae = x_msg_q_send(hMsgQLae, (void *)&rPesInfoLae,
                                    sizeof(VDEC_PES_INFO_T), 255);
                    }
                    #endif
                    #ifdef CC_SUPPORT_VDEC_PREPARSE
                    if (prVdecEsInfo->ePreParseStatus != VDEC_PREPS_ST_DONE)
                    {                
                        i4Ret = x_msg_q_send(hMsgQPreParse, (void *)&_arPesInfoEsm[ucEsId],
                            sizeof(VDEC_PES_INFO_T), 255);
                    }
                    #endif
                }
                else
                {
                    x_memcpy(&_arPesInfoEsm[ucEsId], prPesInfo, sizeof(VDEC_PES_INFO_T));
                    prVdecEsInfo->fgKeepPes = TRUE;
                }
            }
            else
            {
                if(prPesInfo->fgEos)
                {
                    LOG(5, "(%d)EOS in PesInfo\n", ucEsId);
                    _arPesInfoEsm[ucEsId].fgEos = TRUE;
                }
                if(prPesInfo->fgBackward)
                {
                    LOG(5, "fgBackward in PesInfo\n");
                    _arPesInfoEsm[ucEsId].fgBackward = TRUE;
                }

                //DTV00139473
               if(prPesInfo->fgSeqEnd)
               {
                   LOG(5, "fgSeqEnd in PesInfo\n");
                   _arPesInfoEsm[ucEsId].fgSeqEnd = TRUE;
               }
               
               if(prPesInfo->fgIBCEos)
               {
                    LOG(5, "fgIBCEos in PesInfo\n");
                    _arPesInfoEsm[ucEsId].fgIBCEos = TRUE;
               }

                if(prPesInfo->u8Offset > _arPesInfoEsm[ucEsId].u8Offset)
                {
                    if((prPesInfo->u8Offset - _arPesInfoEsm[ucEsId].u8Offset) <
                       (1024*1024))
                    {
                        prVdecEsInfo->u4AvgPicSize =
                            ((prVdecEsInfo->u4AvgPicSize*
                            (VDEC_AVG_PIC_COUNT-1))/VDEC_AVG_PIC_COUNT);
                        prVdecEsInfo->u4AvgPicSize +=
                            ((UINT32)(prPesInfo->u8Offset - _arPesInfoEsm[ucEsId].u8Offset)/
                            VDEC_AVG_PIC_COUNT);
                    }
                }

                // picture end position
                if (prVdecEsInfoKeep->fgLowLatency)// && prVdecEsInfoKeep->fgVPush)
                {
                    LOG(5, "pesinfo[0x%x---0x%x]\n", prPesInfo->u4VldReadPtr, prPesInfo->u4VldReadPtrEnd);
                    x_memcpy(&_arPesInfoEsm[ucEsId], prPesInfo, sizeof(VDEC_PES_INFO_T));
                }
                else
                {
                    if (prVdecEsInfo->fgParsingInfo && prVdecEsInfoKeep->fgVPush &&
                        prPesInfo->fgMoveComplete && prPesInfo->fgSeqEnd)
                    {
                        _arPesInfoEsm[ucEsId].u4VldReadPtrEnd = prVdecEsInfo->u4VFifoPrevWptr;
                    }
                    else
                    {
                        _arPesInfoEsm[ucEsId].u4VldReadPtrEnd = prPesInfo->u4VldReadPtr;
                    }
                }
                //LOG(1, "_arPesInfoEsm[ucEsId].u4VldReadPtr 0x%x\n", _arPesInfoEsm[ucEsId].u4VldReadPtr);
                i4Ret = x_msg_q_send(hMsgQ, (void *)&_arPesInfoEsm[ucEsId],
                    sizeof(VDEC_PES_INFO_T), 255);
                #ifdef CC_DUAL_CORE_DEC
                if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
                {
                    x_memcpy(&rPesInfoLae,(void *)&_arPesInfoEsm[ucEsId],sizeof(VDEC_PES_INFO_T));
                    rPesInfoLae.ucMpvId = prVdecEsInfo->ucLaeId;
                    
                    i4RetLae = x_msg_q_send(hMsgQLae, (void *)&rPesInfoLae,
                                sizeof(VDEC_PES_INFO_T), 255);
                    if(i4RetLae != OSR_OK)
                    {
                        LOG(9,"Send LAE msgQ Faile ret %d\n",i4RetLae);
                    }
                    else
                    {
                        LOG(9,"Send LAE msgQ success MsgQ-Handle 0x%x\n",hMsgQLae);
                    }
                }
                #endif
                #ifdef CC_SUPPORT_VDEC_PREPARSE
                if (prVdecEsInfo->ePreParseStatus != VDEC_PREPS_ST_DONE)
                {                
                    i4Ret = x_msg_q_send(hMsgQPreParse, (void *)&_arPesInfoEsm[ucEsId],
                        sizeof(VDEC_PES_INFO_T), 255);
                }
                #endif
                if(i4Ret == OSR_OK)
                {
                    if(prPesInfo->fgEos)
                    {
                        prVdecEsInfo->fgKeepPes = FALSE;
                    }
                    x_memcpy(&_arPesInfoEsm[ucEsId], prPesInfo, sizeof(VDEC_PES_INFO_T));
                }
                // When we have a sequence end flag, this pes info
                // does not contain video data. We should skip it.
                if (prVdecEsInfo->fgMMPlayback && prPesInfo->fgSeqEnd)
                {
                    LOG(3, "fgSeqEnd in PesInfo\n");
                    prVdecEsInfo->fgKeepPes = FALSE;
                }
            }
        }
        else
        {
            i4Ret = x_msg_q_send(hMsgQ, (void *)prPesInfo,
                sizeof(VDEC_PES_INFO_T), 255);
            #ifdef CC_DUAL_CORE_DEC
            if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT))
            {
                x_memcpy(&rPesInfoLae,(void *)prPesInfo,sizeof(VDEC_PES_INFO_T));
                rPesInfoLae.ucMpvId = prVdecEsInfo->ucLaeId;
                
                i4RetLae = x_msg_q_send(hMsgQLae, (void *)&rPesInfoLae,
                            sizeof(VDEC_PES_INFO_T), 255);
            }
            #endif
            #ifdef CC_SUPPORT_VDEC_PREPARSE
            if (prVdecEsInfo->ePreParseStatus != VDEC_PREPS_ST_DONE)
            {                
                i4Ret = x_msg_q_send(hMsgQPreParse, (void *)prPesInfo,
                    sizeof(VDEC_PES_INFO_T), 255);
            }
            #endif
        }
        if (i4Ret != OSR_OK)
        {
            UCHAR ucRet = 0;

            if(prVdecEsInfoKeep->pfnVdecHandleOverFlw)
            {
                ucRet =  prVdecEsInfoKeep->pfnVdecHandleOverFlw(ucMpvId, ucEsId,
                    prVdecEsInfoKeep->hMutex, hMsgQ, (void*)&_arPesInfoEsm[ucEsId]);
                x_memcpy(&_arPesInfoEsm[ucEsId], prPesInfo, sizeof(VDEC_PES_INFO_T));
            }
            return ucRet;
        }
    }
    VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
    if(((UINT32)prVdecEsInfo->u2Count > prVdecEsInfo->u4MaxEsCnt))
    {
        prVdecEsInfo->u4MaxEsCnt = (UINT32)prVdecEsInfo->u2Count;
    }
    if(prMpvCounter)
    {
        prMpvCounter->u4EsmQCnt = (UINT32)prVdecEsInfo->u2Count;
    }

    #ifdef CC_DUAL_CORE_DEC
    VERIFY(x_msg_q_num_msgs(hMsgQLae, &(prVdecEsInfoLae->u2Count)) == OSR_OK);
    prVdecEsInfoLae->u4MaxEsCnt = prVdecEsInfo->u4MaxEsCnt;
    #endif
    
#endif


#if defined(ENABLE_MULTIMEDIA) || defined(CC_H264_SUPPORT)
#if (!defined(CC_VDEC_H264_DISABLE) || !defined(CC_VDEC_H265_DISABLE))

    if(((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264) && IS_H264_PIC_NALU(prVdecEsInfoKeep->rPesInfo.ucPicType))
       #ifndef CC_VDEC_H265_DISABLE
       || ((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H265) && IsH265SliceNalUnit(prVdecEsInfoKeep->rPesInfo.ucPicType))
       #endif
       )
    {
        prVdecEsInfo->u4FrameNumInEsmQ++;
        #ifdef CC_DUAL_CORE_DEC
        prVdecEsInfoLae->u4FrameNumInEsmQ ++;
        #endif
    }
#endif
#endif

    if (prVdecEsInfo->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
        if (prFbmSeqHdr != NULL)
        {
            prFbmSeqHdr->u4LastPTS = prPesInfo->u4PTS;
            prFbmSeqHdr->u8LastPTS = prPesInfo->u8PTS;
        }
    }

    ucValid = 1;
    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_H264)
    {
        u4OverflwThrd = H264_MAX_PES_NS_PER_QUEUE;
    }
    else
    {
        u4OverflwThrd = MPV_MAX_PES_NS_PER_QUEUE;
    }

    if (prVdecEsInfo->u2Count >= (UINT16)u4OverflwThrd)
    {
        LOG(1, "[Warning][%d] EsFifoCnt OverFlw !!!!!!!!!!!!!!!!!\n",ucEsId);
        if(prVdecInfo->pfnStatusCb)
        {
            rStatusCB.u1EsId = ucEsId;
            rStatusCB.u4Status = (UINT32)VDEC_BUF_OVERFLOW;
            prVdecInfo->pfnStatusCb(&rStatusCB);
        }
        if(prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY)
        {
            prVdecEsInfo->ucDropBNs++;
        }
        ucValid = 0;
    }

    if (!prVdecEsInfo->ucStarted)
    {
        if ((prVdecEsInfo->u2Count >= prVdecEsInfo->u2UdfNs) ||
            (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME) ||
            (prPesInfo->fgNonBuffer) ||
            (_arPesInfoEsm[ucEsId].fgEos) ||
            (prVdecEsInfoKeep->fgVPush))
        {
            prVdecEsInfo->ucStarted = 1;

            VERIFY(x_sema_unlock(prVdecEsInfoKeep->hUnderFlowSema) == OSR_OK);
        }
    }

    return ucValid;
}


#ifdef MPV_VIRTUAL_PRS
void MPV_VirtualPrsParse(UCHAR ucMvpId, UINT32 u4Addr, UINT32 u4PicNs, UINT32 u4FrmNs)
{
    ASSERT(ucMvpId < VDEC_MAX_VLD);

    _rVdecInfo.arVdecEsInfo[_ucMpvVPEsNs].u4CompareNs = u4FrmNs;

    if (_ucMpvVPEsNs < VDEC_MAX_ES)
    {
        UCHAR ucPicType;
        UCHAR *pucPicTp;
        UINT32 u4Temp;
        UINT32* pu4Hdr;
        UINT32 u4StartCode;
        UINT32 u4Idx;

        _ucMpvVpEsId[_ucMpvVPEsNs] = _ucMpvVPEsNs;
        _u4MpvVpStartAddr[_ucMpvVPEsNs] = u4Addr;
        _ucMpvVpMpvId[_ucMpvVPEsNs] = ucMvpId;
        _u4MpvVpPicNs[_ucMpvVPEsNs] = u4PicNs;

        pu4Hdr = (UINT32*) x_mem_alloc(u4PicNs * sizeof(UINT32));
        ASSERT(pu4Hdr != NULL);
        _u4MpvVpAddr[_ucMpvVPEsNs] = pu4Hdr;

        pucPicTp = (UCHAR*) x_mem_alloc(u4PicNs * sizeof(UINT32));
        ASSERT(pucPicTp != NULL);
        _u4MpvVpPicTp[_ucMpvVPEsNs] = pucPicTp;

        #if defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4Addr), (50*1024*1024));
        #else
        HalFlushInvalidateDCache();
        #endif

        _MPV_DecReset(ucMvpId, _ucMpvVPEsNs);
        _MPV_VldSetVFifo(ucMvpId, 0, ~0);
        _MPV_VldInitBarrelShifter(ucMvpId, u4Addr);

        for (u4Idx = 0; u4Idx < u4PicNs; u4Idx++)
        {
            u4StartCode = _MPV_VParserGetHdr(ucMvpId, _ucMpvVPEsNs, &u4Temp, &ucPicType);
            pu4Hdr[u4Idx] = u4Temp;
            pucPicTp[u4Idx] = ucPicType;
            LOG(5, "Pic %d, Type %d, Addr 0x%x\n", u4Idx, ucPicType, u4Temp);

            while (u4StartCode != 0x100)        // Picture Header Start Code
            {
                u4StartCode = _MPV_VParserGetHdr(ucMvpId, _ucMpvVPEsNs, &u4Temp, &ucPicType);
            }
        }

        _ucMpvVPEsNs++;
    }
}


void MPV_VirtualPrsStart(UINT32 u4Delay)
{
    if (_fgMpvVirtualPrsStart == 0)
    {
        HANDLE_T hThread1;

        _fgMpvVirtualPrsDestroy = FALSE;
        _fgMpvVirtualPrsStart = TRUE;

        _ucMpvVirtualPrsDelay = u4Delay;

        // create thread
        VERIFY(x_thread_create(&hThread1, "VPrs Thread", MPV_VPRS_STACK_SIZE,
            MPV_VPRS_THREAD_PRIORITY,
        _MpvVirutualPrsRoutine, 0, NULL) == OSR_OK);
    }
}


void MPV_VirtualPrsStop(void)
{
    if (_fgMpvVirtualPrsStart)
    {
        _fgMpvVirtualPrsDestroy = TRUE;
    }
}
#endif


