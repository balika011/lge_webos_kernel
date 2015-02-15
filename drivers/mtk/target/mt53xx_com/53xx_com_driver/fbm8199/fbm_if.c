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
 * $RCSfile: fbm_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file fbm_if.c
 *  This file contains implementation of exported APIs of FBM.
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "x_lint.h"
#include "fbm_drvif.h"
#include "fbm_inst_if.h"
#include "fbm_inst_drvif.h"
#include "fbm_fb.h"
#include "fbm_pool.h"
#include "fbm_pool_config.h"
#include "fbm_cs.h"
#include "fbm_debug.h"
#include "vdp_prescale_drvif.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_5381.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_syslog.h"
#include "x_timer.h"
#include "x_bim.h"
#include "drvcust_if.h"
#include "vdec_drvif.h"
#include "b2r_drvif.h"
#include "gfx_drvif.h"
LINT_EXT_HEADER_END


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

typedef struct
{
    UINT32 u4Addr;
    UINT32 u4Size;
    BOOL fgInUse;
} FBM_SEAMLESS_T;

#if defined(CC_ENABLE_SEAMLESS_FOR_MVC)
#define MAX_SEAMLESS_BUFF_COUNT 2
#else
#define MAX_SEAMLESS_BUFF_COUNT 1
#endif

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

#define VERIFY_FBG(id)                                  \
    ((id >= FBG_MAX_NS) ||                              \
    (_arFbg[id].ucFbgType == FBM_FBG_TYPE_UNKNOWN) ||   \
    (_arFbg[id].ucFbgId != id))

#define VERIFY_FB(gid, id)                              \
    (!(((_arFbg[gid].ucFbNsBase <= id) && (id < _arFbg[gid].ucFbNs))   ||  \
    ((_arFbg[gid].ucFbNsOldBase <= id) && (id < _arFbg[gid].ucFbNsOld)))   || \
    (id >= FBM_MAX_FB_NS_PER_GROUP))

#define VERIFY_FB_NS(gid)                               \
    (_arFbg[gid].ucFbNs - _arFbg[gid].ucFbNsBase > FBM_MAX_FB_NS_PER_GROUP/2)

#define IS_REFERENCE_FB(gid, id)                        \
    ((id == _arFbg[gid].ucFbFRef) || (id == _arFbg[gid].ucFbBRef))

#define NOT_REFERENCE_FB(gid, id)                        \
    ((id != _arFbg[gid].ucFbFRef) && (id != _arFbg[gid].ucFbBRef))

#ifdef CC_VDP_FULL_ISR

#define FBM_MUTEX_LOCK(id)                              \
    (*_arFbg[id].prState = x_crit_start())

#define FBM_MUTEX_UNLOCK(id)                            \
    (x_crit_end(*_arFbg[id].prState))

#else

#define FBM_MUTEX_LOCK(id)                              \
    (VERIFY(x_sema_lock(_arFbg[id].hMutex, X_SEMA_OPTION_WAIT) == OSR_OK))

#define FBM_MUTEX_UNLOCK(id)                            \
    (VERIFY(x_sema_unlock(_arFbg[id].hMutex) == OSR_OK))

#endif


#define FBM_MEMSET(addr, value, len)                    \
    (VERIFY(x_memset((addr), value, len) == (addr)))

#define FBM_BYTE(value)                                 \
    ((UCHAR)(value & 0xFF))

#define FBM_GFX_MEMSET(u4Addr, u4Width, u4Height, u4Color)    \
    if (FBM_CHECK_CB_FUNC_VERIFY(_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_MEMSET], _arFbmCbFunc.au4CbFuncCRC[FBM_CB_FUNC_GFX_MEMSET])) \
    {\
        ((FBM_GFX_MEMSET_FUNC)_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_MEMSET])((UINT8*)u4Addr, u4Width, u4Height, u4Color);\
    }

#define FBM_GFX_MEMSET_ASYNC(u4Addr, u4Width, u4Height, u4Color)    \
    ((FBM_CHECK_CB_FUNC_VERIFY(_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET], _arFbmCbFunc.au4CbFuncCRC[FBM_CB_FUNC_GFX_TASK_MEMSET])) ?\
    (((FBM_GFX_TASK_MEMSET_FUNC)_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET])((UINT8*)u4Addr, u4Width, u4Height, u4Color)) : 0)
    

//If ns >= 10, return 10; else if ns >= 5, return 5; else return ns;
#define FBM_FLOOR_FIX(ns, a, b) (((ns)>=(a)) ? (a) : (((ns)>=(b)) ? (b) : (ns)))
#define FBM_MIN(a, b) ((a) < (b) ? (a) : (b))

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

extern UINT32 _FBM_QueryBufNum(UINT32 u4VDecFmt, UINT32 u4Width,
    UINT32 u4Height, UCHAR ucColorMode, UINT32 *pu4BufNum, UINT32 *pu4Stride,
    UINT32 *pu4SliceHeight, UINT32 *pu4UVAddrOffset, UINT32 u4ExtOption);
#ifdef FBM_CS_LOG
extern void _FbmCsLog(UINT32 u4Log, INT32 i4Count, UINT32 u4handle);
#endif

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

static void _FbgStatus(UCHAR ucFbgId);
static UINT32 FBM_PreConfigColorMode(UCHAR ucFbgId, UCHAR ucFbgCm, UINT32 u4HSize, UINT32 u4VSize, FBM_CREATE_FBG_PAR_T *prPar);
static void _DetectNewSeamless(UINT32 u4FbgId, UINT32 u4HSize, UINT32 u4VSize, FBM_CREATE_FBG_PAR_T *prPar);
static void _InitSeamlessBuffer(void);
static void _FreeSeamlessBuffer(UINT32 u4BufferAddr);
static void _GetSeamlessBuffer(UINT32 *pu4BufferAddr, UINT32 *pu4BufferSize);

void _EnableNewSeamless(UINT32 u4FbgId);




//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

// Initiated Protection
static UCHAR _FbmInitiated = 0;

#ifndef FBM_VIRTUAL_MPEG
// frame buffer group
static FBM_FBG_T _arFbg[FBG_MAX_NS];
#else
FBM_FBG_T _arFbg[FBG_MAX_NS];
#endif

static FBM_SEAMLESS_T _arSeamlessInfo[MAX_SEAMLESS_BUFF_COUNT];

static UINT32 _u4NextFbgId = 0;
static UINT8 _u1FbgColorMode = FBM_CM_420;

static BOOL _fgSetDecoderFb = FALSE;
static UINT8 _u1DecoderType = FBM_VDEC_UNKNOWN;
static UINT8 _u1NumOfFb = 4;

static FBM_POOL_T *_prPool = NULL;

static FBM_CB_FUNC_T _arFbmCbFunc;

#ifdef CC_VDP_FULL_ISR
static CRIT_STATE_T _rState;
#endif

#ifdef FBM_CS_LOG
UINT32 _u4EQHandle = 0;
#endif

static HANDLE_T _hFbgMutex = (HANDLE_T)(NULL);

EXTERN void _VdpCheckFbgReady(UCHAR ucFbgId, UCHAR ucEsId);

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** _FbgStatus
 *  Print FBM status
 *  @return NONE.
 */
//-------------------------------------------------------------------------
static void _FbgStatus(UCHAR ucFbgId)
{
    UCHAR ucIdx;

    if (ucFbgId >= FBG_MAX_NS)
    {
        return;
    }

    if ((_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_UNKNOWN) ||
            (_arFbg[ucFbgId].ucFbgId != ucFbgId))
    {
        LOG(1, "FBG(%d) is NULL!\n", ucFbgId);
        return;
    }

      LOG(1, "FBG(%d) Type(%d) CM(%d) Ns(%d) D(%d) F(%d) B(%d) Pool(0x%x, 0x%x) \n",
        _arFbg[ucFbgId].ucFbgId,
        _arFbg[ucFbgId].ucFbgType,
        _arFbg[ucFbgId].ucFbgCm,
        _arFbg[ucFbgId].ucFbNs,
        _arFbg[ucFbgId].ucFbDecode,
        _arFbg[ucFbgId].ucFbFRef,
        _arFbg[ucFbgId].ucFbBRef,
        _arFbg[ucFbgId].u4FbMemoryPool,
        _arFbg[ucFbgId].u4FbMemoryPoolSize);
      LOG(1,"Resolution(0x%x,0x%x),Pitch(0x%x),Raster(%d),422Mode(%d)\n",
        _arFbg[ucFbgId].rSeqHdr.u2HSize,
        _arFbg[ucFbgId].rSeqHdr.u2VSize,
        _arFbg[ucFbgId].rSeqHdr.u2LineSize,
        _arFbg[ucFbgId].rSeqHdr.fgRasterOrder,
        _arFbg[ucFbgId].rSeqHdr.fg422Mode);

    for(ucIdx = _arFbg[ucFbgId].ucFbNsBase; ucIdx < _arFbg[ucFbgId].ucFbNs; ucIdx++)
    {
        if(ucIdx < FBM_MAX_FB_NS_PER_GROUP)
        {
            LOG(0, "\tFB(%2d) S(%d,%d) Y(0x%x) C(0x%x) PTS(0x%8x)\n",
                ucIdx,
                _arFbg[ucFbgId].aucFbStatus[ucIdx],
                _arFbg[ucFbgId].afgRefList[ucIdx],
                _arFbg[ucFbgId].au4AddrY[ucIdx],
                _arFbg[ucFbgId].au4AddrC[ucIdx],
                _arFbg[ucFbgId].prPicHdr ? _arFbg[ucFbgId].prPicHdr[ucIdx].u4PTS : 0);
        }
    }
    for(ucIdx = _arFbg[ucFbgId].ucFbNsOldBase; ucIdx < _arFbg[ucFbgId].ucFbNsOld; ucIdx++)
    {
        if(ucIdx < FBM_MAX_FB_NS_PER_GROUP)
        {
            LOG(2, "\tFB(%2d) S(%d,%d) Y(0x%x) C(0x%x) PTS(0x%8x)\n",
                ucIdx,
                _arFbg[ucFbgId].aucFbStatus[ucIdx],
                _arFbg[ucFbgId].afgRefList[ucIdx],
                _arFbg[ucFbgId].au4AddrY[ucIdx],
                _arFbg[ucFbgId].au4AddrC[ucIdx],
                _arFbg[ucFbgId].prPicHdr ? _arFbg[ucFbgId].prPicHdr[ucIdx].u4PTS : 0);
        }
    }

    if(_arFbg[ucFbgId].fgEnableNewSeamless)
    {
        for (ucIdx = 0; ucIdx < _arFbg[ucFbgId].ucSeamlessFbNs; ucIdx++)
        {
            if(ucIdx < FBM_MAX_FB_NS_PER_GROUP)
            {
                LOG(0, "\tFB(%d) S(%d) O(%d) Y(0x%x) C(0x%x)\n",
                    ucIdx,
                    _arFbg[ucFbgId].aucResizeFbStatus[ucIdx],
                    _arFbg[ucFbgId].aucResizeSrcFbId[ucIdx],
                    _arFbg[ucFbgId].au4ResizeAddrY[ucIdx],
                    _arFbg[ucFbgId].au4ResizeAddrC[ucIdx]);
            }
        }
    }
}

UINT32 FBM_PreConfigColorMode(UCHAR ucFbgId, UCHAR ucFbgCm, UINT32 u4HSize, UINT32 u4VSize, FBM_CREATE_FBG_PAR_T *prPar)
{
    //UINT32 u4YAddr, u4CAddr, u4FbgIdx, u4FbId;
    UINT32 u4FbId ,u4BufNum, u4Stride, u4SliceHeight, u4UVAddrOffset;

    // init Q
    _arFbg[ucFbgId].rEmptyQ.ucCount = 0;
    _arFbg[ucFbgId].rEmptyQ.ucReadIdx = 0;


    _FBM_QueryBufNum(_arFbg[ucFbgId].u4VDecFmt, u4HSize, u4VSize, ucFbgCm,
        &u4BufNum, &u4Stride, &u4SliceHeight, &u4UVAddrOffset, 0);
    _arFbg[ucFbgId].ucFbNs = u4BufNum;

    if (_arFbg[ucFbgId].ucFbNs > FBM_MAX_FB_NS_PER_GROUP)
    {
        _arFbg[ucFbgId].ucFbNs = FBM_MAX_FB_NS_PER_GROUP;
    }

    for (u4FbId = 0; u4FbId < _arFbg[ucFbgId].ucFbNs; u4FbId++)
    {
        _arFbg[ucFbgId].au4AddrY[u4FbId] = 0;
        _arFbg[ucFbgId].au4AddrC[u4FbId] = 0;
    }

    _arFbg[ucFbgId].u1FbgAppMode= prPar->u1AppMode;
    _arFbg[ucFbgId].u4FbWidth = u4Stride;
    _arFbg[ucFbgId].u4FbHeight = u4SliceHeight;
    _arFbg[ucFbgId].ucFbgCm = ucFbgCm;
    _arFbg[ucFbgId].ucMvBufNs = 0;

    _arFbg[ucFbgId].u4FbMemoryPoolSize = 0;
    _arFbg[ucFbgId].u4FbMemoryPool = 0;
    
    // init working buf
    if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_MPEG2)
    {
        _arFbg[ucFbgId].au4AddrMv[0] = 0;

        if ((_arFbg[ucFbgId].ucFbNs >= FBM_DBK_FB_NS) && (_arFbg[ucFbgId].ucFbgType != FBM_FBG_TYPE_1080HD_RR))
        {
            _arFbg[ucFbgId].au4AddrY[_arFbg[ucFbgId].ucFbNs] = 0;
            _arFbg[ucFbgId].au4AddrC[_arFbg[ucFbgId].ucFbNs] = 0;
        }
    }
    else if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264)
    {
        for (u4FbId = 0; u4FbId < _arFbg[ucFbgId].ucMvBufNs && u4FbId < (FBM_MAX_FB_NS_PER_GROUP-1) ; u4FbId++)
        {
            _arFbg[ucFbgId].au4AddrMv[u4FbId] = 0;
        }
    }
    else if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RV)
    {
        _arFbg[ucFbgId].u4ExtraYBuffer = 0;
        _arFbg[ucFbgId].u4ExtraCBuffer = 0;
        _arFbg[ucFbgId].au4AddrMv[0] = 0;
    }
    else
    {
        _arFbg[ucFbgId].au4AddrMv[0] = 0;
    }

    _arFbg[ucFbgId].u4WorkBufSize = 0;

    if ((_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264) ||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_AVS) ||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RAW) ||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_JPEG)||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_VP8)||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_VP6))

    {
        // create semaphore, Empty Q
        VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, 0) == OSR_OK);
    }
    else
    {
        // create semaphore, Empty Q
        VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, 0) == OSR_OK);

        // create semaphore, Empty BQ
        VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyBQSemaphore, _arFbg[ucFbgId].hMutex, 0) == OSR_OK);

#ifdef CC_VDP_FULL_ISR
        _arFbg[ucFbgId].hEmptyQSemaphore.prState = &_rState;
        _arFbg[ucFbgId].hEmptyBQSemaphore.prState = &_rState;
#endif

    }

    _arFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbFRef = FBM_FB_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbBRef = FBM_FB_ID_UNKNOWN;

    // set default status to unknown
    for (u4FbId = 0; u4FbId < _arFbg[ucFbgId].ucFbNs; u4FbId++)
    {
        _arFbg[ucFbgId].aucFbStatus[u4FbId] = FBM_FB_STATUS_UNKNOWN;
    }

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
//    ASSERT(_arFbg[ucFbgId].hEmptyRQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    return 0;
}


void _DetectNewSeamless(UINT32 u4FbgId, UINT32 u4HSize, UINT32 u4VSize, FBM_CREATE_FBG_PAR_T *prPar)
{
#ifdef CC_FBM_ENABLE_NEW_SEAMLESS_DETECTION
    if (prPar->u1AppMode == FBM_FBG_APP_Frame_Pack_3D)
    {
        // auto detect if need enable new seamless.
        UINT32 u43DFbgId = FBM_FBG_ID_UNKNOWN;
        UINT32 u4ResizeFbgId = FBM_FBG_ID_UNKNOWN;
        UINT32 u4Addr, u4FbgIdx, u4FbIdx, u4KR3DFbNs = 0;
        UINT32 u4ReusePoolId = FBM_FBG_ID_UNKNOWN;
        FBM_POOL_T* prFbmPool = NULL;

        UINT32 u4SeamlessYSize = FBM_FBG_TYPE_1080HD_Y_SIZE;
        UINT32 u4SeamlessCSize = FBM_FBG_TYPE_1080HD_C_SIZE;
        UCHAR ucRzId = 0;
        
        _arFbg[u4FbgId].rSeqHdr.u2HSize = u4HSize;
        _arFbg[u4FbgId].rSeqHdr.u2VSize = u4VSize;
        
        for (u4FbgIdx = 0; u4FbgIdx < FBG_MAX_NS; u4FbgIdx++)
        {
            if (_arFbg[u4FbgIdx].ucFbgId != FBM_FBG_ID_UNKNOWN && _arFbg[u4FbgIdx].u1FbgAppMode == FBM_FBG_APP_Frame_Pack_3D)
            {
                if (u4FbgIdx == u4FbgId)
                {
                    // skip self.
                    continue;
                }
                u43DFbgId = u4FbgIdx;

                if (u4HSize > _arFbg[u43DFbgId].rSeqHdr.u2HSize || 
                    (u4VSize > _arFbg[u43DFbgId].rSeqHdr.u2VSize  && _arFbg[u43DFbgId].rSeqHdr.u2VSize < 1080))
                {
                    LOG(1, "Resize FbgId %d  (%d %d) to (%d %d)\n", u43DFbgId, _arFbg[u43DFbgId].rSeqHdr.u2HSize, _arFbg[u43DFbgId].rSeqHdr.u2VSize, u4HSize, u4VSize);
                    u4ResizeFbgId = u43DFbgId;
                    _arFbg[u43DFbgId].rSeqHdr.u4ResizeWidth = u4HSize;
                    _arFbg[u43DFbgId].rSeqHdr.u4ResizeHeight = u4VSize;
                }
                else if (u4HSize < _arFbg[u43DFbgId].rSeqHdr.u2HSize || 
                    (u4VSize < _arFbg[u43DFbgId].rSeqHdr.u2VSize && u4VSize < 1080))
                {
                    LOG(1, "Resize FbgId %d  (%d %d) to (%d %d)\n", u4FbgId, u4HSize, u4VSize, _arFbg[u43DFbgId].rSeqHdr.u2HSize, _arFbg[u43DFbgId].rSeqHdr.u2VSize);
                    u4ResizeFbgId = u4FbgId;
                    _arFbg[u4ResizeFbgId].rSeqHdr.u4ResizeWidth = _arFbg[u43DFbgId].rSeqHdr.u2HSize;
                    _arFbg[u4ResizeFbgId].rSeqHdr.u4ResizeHeight = _arFbg[u43DFbgId].rSeqHdr.u2VSize;
                }
                else
                {
                    break;
                }

                if (_arFbg[u4FbgId].ucFbgCm == FBM_CM_422)
                {
                    u4SeamlessCSize *= 2;
                }
                else if (_arFbg[u4FbgId].ucFbgCm == FBM_CM_444)
                {
                    u4SeamlessCSize *= 4;
                }

                ASSERT(_arFbg[u4FbgId].ucFbgCm == _arFbg[u43DFbgId].ucFbgCm);
                
                //get extra buffer for 1 frame.
                if (_arFbg[u4FbgId].u4FbMemoryPoolSize > FBM_GetRunningVdecMemSize(u4FbgId))
                {
                    UINT32 u4FreeSize = 0;
                    u4FreeSize = _arFbg[u4FbgId].u4FbMemoryPoolSize - FBM_GetRunningVdecMemSize(u4FbgId);

                    if (u4FreeSize > u4SeamlessYSize + u4SeamlessCSize)
                    {
                        u4ReusePoolId = u4FbgId;
                        LOG(1, "Share pool FbgId %d, free size %d\n",u4ReusePoolId, u4FreeSize);
                    }
                    else if (_arFbg[u43DFbgId].u4FbMemoryPoolSize > FBM_GetRunningVdecMemSize(u43DFbgId))
                    {
                        u4FreeSize = _arFbg[u43DFbgId].u4FbMemoryPoolSize - FBM_GetRunningVdecMemSize(u43DFbgId);

                        if (u4FreeSize > u4SeamlessYSize + u4SeamlessCSize)
                        {
                            u4ReusePoolId = u43DFbgId;
                            LOG(1, "Share pool FbgId %d, free size %d\n",u4ReusePoolId, u4FreeSize);
                        }
                    }
                }
                else if (_arFbg[u43DFbgId].u4FbMemoryPoolSize > FBM_GetRunningVdecMemSize(u43DFbgId))
                {
                    UINT32 u4FreeSize = 0;
                    u4FreeSize = _arFbg[u43DFbgId].u4FbMemoryPoolSize - FBM_GetRunningVdecMemSize(u43DFbgId);

                    if (u4FreeSize > u4SeamlessYSize + u4SeamlessCSize)
                    {
                        u4ReusePoolId = u43DFbgId;
                        LOG(1, "Share pool FbgId %d, free size %d\n",u4ReusePoolId, u4FreeSize);
                    }
                }

                LOG(1, "Reuse Pool ID: %d\n", u4ReusePoolId);
                ASSERT(u4ReusePoolId != FBM_FBG_ID_UNKNOWN);

                ucRzId = _VDP_Prescale_Init();
                LOG(1, "Partitioning new seamless buffer, RzId %d.\n", ucRzId);
                ASSERT(ucRzId != VDP_PRESCALE_RZ_ID_UNKNOWN);
                _arFbg[u4ResizeFbgId].ucSeamlessRzId = (ucRzId != VDP_PRESCALE_RZ_ID_UNKNOWN) ? ucRzId : VDP_PRESCALE_RZ_ID_DEFAULT;
                    
                _arFbg[u4ResizeFbgId].u4FbResizeMemoryPoolSize = 1920*1088*3; // 1 from share pool, 2 from KR3D.
                _arFbg[u4ResizeFbgId].u4FbResizeMemoryPool = 0;

                // partition seamless buffer here.
                _arFbg[u4ResizeFbgId].ucSeamlessFbNs = 1;

                u4Addr = _arFbg[u4ReusePoolId].u4FbMemoryPool + FBM_GetRunningVdecMemSize(u4ReusePoolId);
                u4Addr = (((u4Addr) + 0x7FF) & (~0x7FF));
                for (u4FbIdx = 0; ((u4FbIdx < _arFbg[u4ResizeFbgId].ucSeamlessFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
                {
                    _arFbg[u4ResizeFbgId].au4ResizeAddrY[u4FbIdx] = u4Addr;
                    FBM_GFX_MEMSET((UINT8*)u4Addr, FBM_FBG_1080HD_H, FBM_FBG_1080HD_V, 0x0);
                    u4Addr += u4SeamlessYSize;

                    _arFbg[u4ResizeFbgId].au4ResizeAddrC[u4FbIdx] = u4Addr;
                    FBM_GFX_MEMSET((UINT8*)u4Addr, FBM_FBG_1080HD_H, (FBM_FBG_1080HD_V >> 1), 0x80);
                    u4Addr += u4SeamlessCSize;
                }

                // Reuse KR3D buffer.
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_IMGRZ_3D_KR);
                u4Addr = (((prFbmPool->u4Addr) + 0x7FF) & (~0x7FF));
                
                u4KR3DFbNs = (FBM_IMGRZ_3D_KR_POOL_SIZE / (u4SeamlessYSize + u4SeamlessCSize));
                
                for (u4FbIdx = _arFbg[u4ResizeFbgId].ucSeamlessFbNs ; ((u4FbIdx < _arFbg[u4ResizeFbgId].ucSeamlessFbNs + u4KR3DFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
                {
                    _arFbg[u4ResizeFbgId].au4ResizeAddrY[u4FbIdx] = u4Addr;
                    FBM_GFX_MEMSET((UINT8*)u4Addr, FBM_FBG_1080HD_H, FBM_FBG_1080HD_V, 0x0);
                    u4Addr += u4SeamlessYSize;

                    _arFbg[u4ResizeFbgId].au4ResizeAddrC[u4FbIdx] = u4Addr;
                    FBM_GFX_MEMSET((UINT8*)u4Addr, FBM_FBG_1080HD_H, (FBM_FBG_1080HD_V >> 1), 0x80);
                    u4Addr += u4SeamlessCSize;
                }
                
                _arFbg[u4ResizeFbgId].ucSeamlessFbNs += u4KR3DFbNs;

                _arFbg[u4ResizeFbgId].rResizeEmptyQ.ucCount = 0;
                _arFbg[u4ResizeFbgId].rResizeEmptyQ.ucReadIdx = 0;

                // create semaphore, Empty Q
                VERIFY(_FBM_csema_reset(&_arFbg[u4ResizeFbgId].hEmptyResizeQSemaphore, _arFbg[u4ResizeFbgId].hMutex, (INT32)_arFbg[u4ResizeFbgId].ucSeamlessFbNs) == OSR_OK);

                // put frames into empty Q
                for (u4FbIdx = 0; u4FbIdx < _arFbg[u4ResizeFbgId].ucSeamlessFbNs; u4FbIdx++)
                {
                    _FBM_PutFrameBufferToEmptyResizeQ(u4ResizeFbgId, (UCHAR)(u4FbIdx));
                }

                if (u4ResizeFbgId == u43DFbgId)
                {
                    FBM_MUTEX_LOCK(u4ResizeFbgId);

                    _arFbg[u4ResizeFbgId].fgEnableNewSeamless = TRUE;
                    if (_arFbg[u4ResizeFbgId].rDisplayQ.ucCount > 0)
                    {
                        UCHAR ucCount = _arFbg[u4ResizeFbgId].rDisplayQ.ucCount;
                        UCHAR ucResizeFbId, ucReadIdx;
                        ucReadIdx = _arFbg[u4ResizeFbgId].rDisplayQ.ucReadIdx;

                        do
                        {
                            ucResizeFbId = _arFbg[u4ResizeFbgId].rDisplayQ.aucQueue[ucReadIdx];
                            LOG(1, "Peek and resize (%d, %d)\n",u4ResizeFbgId,ucResizeFbId);
                            _arFbg[u4ResizeFbgId].ucCurSeamlessTaskId++;
                            if (_arFbg[u4ResizeFbgId].ucCurSeamlessTaskId == 0xFF)
                            {
                                _arFbg[u4ResizeFbgId].ucCurSeamlessTaskId = 0;
                            }
                            _arFbg[u4ResizeFbgId].prPicHdr[ucResizeFbId].ucSeamlessTaskId = _arFbg[u4ResizeFbgId].ucCurSeamlessTaskId;
                            _VDP_Prescale_Send(_arFbg[u4ResizeFbgId].ucSeamlessRzId, u4ResizeFbgId, ucResizeFbId, _arFbg[u4ResizeFbgId].ucCurSeamlessTaskId);
                            ucCount--;
                            ucReadIdx = (UCHAR) (ucReadIdx+1) % FBM_MAX_FB_NS_PER_GROUP;
                        }
                        while(ucCount > 0);
                    }

                    FBM_MUTEX_UNLOCK(u4ResizeFbgId);
                }
               
                _arFbg[u4ResizeFbgId].fgEnableNewSeamless = TRUE;
                _arFbg[u4ResizeFbgId].fgAutoSeamless = TRUE;
                break;
            }
        }
    }
#endif
    return;
}

void _EnableNewSeamless(UINT32 u4FbgId)
{
    if (!_arFbg[u4FbgId].fgEnableNewSeamless)
    {
        UINT32 u4Addr, u4FbIdx;
#ifdef FBM_SUPPORT_4K2K_SEAMLESS
        UINT32 u4SeamlessYSize = FBM_FBG_TYPE_4K2K_Y_SIZE;
        UINT32 u4SeamlessCSize = FBM_FBG_TYPE_4K2K_C_SIZE;
#else
        UINT32 u4SeamlessYSize = FBM_FBG_TYPE_1080HD_Y_SIZE;
        UINT32 u4SeamlessCSize = FBM_FBG_TYPE_1080HD_C_SIZE;
#endif

        _arFbg[u4FbgId].rResizeEmptyQ.ucCount = 0;
        _arFbg[u4FbgId].rResizeEmptyQ.ucReadIdx = 0;

        if (_arFbg[u4FbgId].ucFbgCm == FBM_CM_422)
        {
            u4SeamlessCSize *= 2;
        }
        else if (_arFbg[u4FbgId].ucFbgCm == FBM_CM_444)
        {
            u4SeamlessCSize *= 4;
        }

#if 0
        // Allocate seamless buffer for small picture
        if (u4HSize < FBM_IMG_RESIZE_LIMITATION || u4VSize < FBM_IMG_RESIZE_LIMITATION)
        {
            UINT32 u4YSize;
            UINT32 u4WidthAlign = FBM_ALIGN_MASK(u4HSize, FBM_B2R_H_PITCH);
            UINT32 u4HeightAlign = FBM_ALIGN_MASK(u4VSize, FBM_MPV_V_ALIGMENT);
            //UINT32 u4HeightAlign = FBM_ALIGN_MASK((u4VSize + 31) & (~31));
            //UINT32 u4WidthAlign =  FBM_ALIGN_MASK((u4HSize + 63) & (~64));
            u4YSize = u4WidthAlign * u4HeightAlign;

            LOG(1, "Partitioning small picture resize buffer.\n");

            _arFbg[u4FbgId].ucSeamlessFbNs = 2;
            _arFbg[u4FbgId].au4ResizeAddrY[0] = ((_arFbg[u4FbgId].au4AddrY[0] + u4YSize) + 2047) & (~2047);
            _arFbg[u4FbgId].au4ResizeAddrC[0] = ((_arFbg[u4FbgId].au4AddrC[0] + (u4YSize/2)) + 2047) & (~2047);
            _arFbg[u4FbgId].au4ResizeAddrY[1] = ((_arFbg[u4FbgId].au4AddrY[1] + u4YSize) + 2047) & (~2047);
            _arFbg[u4FbgId].au4ResizeAddrC[1] = ((_arFbg[u4FbgId].au4AddrC[1] + (u4YSize/2)) + 2047) & (~2047);

            // create semaphore, Empty Q
            VERIFY(_FBM_csema_reset(&_arFbg[u4FbgId].hEmptyResizeQSemaphore, _arFbg[u4FbgId].hMutex, (INT32)_arFbg[u4FbgId].ucSeamlessFbNs) == OSR_OK);

            // put frames into empty Q
            for (u4FbIdx = 0; u4FbIdx < _arFbg[u4FbgId].ucSeamlessFbNs; u4FbIdx++)
            {
                _FBM_PutFrameBufferToEmptyResizeQ(u4FbgId, (UCHAR)(u4FbIdx));
            }

            _arFbg[u4FbgId].fgEnableNewSeamless = TRUE;
            ucRzId = _VDP_Prescale_Init();
            LOG(1, "Enable new seamless buffer, RzId %d.\n", ucRzId);
            ASSERT(ucRzId != VDP_PRESCALE_RZ_ID_UNKNOWN);
            _arFbg[u4FbgId].ucSeamlessRzId = (ucRzId != VDP_PRESCALE_RZ_ID_UNKNOWN) ? ucRzId : VDP_PRESCALE_RZ_ID_DEFAULT;
            
            //_arFbg[u4FbgId].fgAutoSeamless = TRUE;
            
            return;
        }
#endif

        _arFbg[u4FbgId].u4FbResizeMemoryPool = 0;
        _arFbg[u4FbgId].u4FbResizeMemoryPoolSize = 0;

        // Allocate seamless buffer for RV RPR.
        if (_arFbg[u4FbgId].u4VDecFmt == FBM_VDEC_RV && _arFbg[u4FbgId].fgRPRMode)
        {
            LOG(1, "Partitioning RV RPR seamless buffer.\n");
            _arFbg[u4FbgId].ucSeamlessFbNs = 2;
            _arFbg[u4FbgId].au4ResizeAddrY[0] = _arFbg[u4FbgId].u4RPRSeamlessBuffer;
            _arFbg[u4FbgId].au4ResizeAddrC[0] = _arFbg[u4FbgId].au4ResizeAddrY[0] + FBM_FBG_TYPE_PAL_Y_SIZE;
            _arFbg[u4FbgId].au4ResizeAddrY[1] = _arFbg[u4FbgId].au4ResizeAddrC[0] + FBM_FBG_TYPE_PAL_C_SIZE;
            _arFbg[u4FbgId].au4ResizeAddrC[1] = _arFbg[u4FbgId].au4ResizeAddrY[1] + FBM_FBG_TYPE_PAL_Y_SIZE;

            // create semaphore, Empty Q
            VERIFY(_FBM_csema_reset(&_arFbg[u4FbgId].hEmptyResizeQSemaphore, _arFbg[u4FbgId].hMutex, (INT32)_arFbg[u4FbgId].ucSeamlessFbNs) == OSR_OK);

            // put frames into empty Q
            for (u4FbIdx = 0; u4FbIdx < _arFbg[u4FbgId].ucSeamlessFbNs; u4FbIdx++)
            {
                _FBM_PutFrameBufferToEmptyResizeQ(u4FbgId, (UCHAR)(u4FbIdx));
            }

            return;
        }

        // Allocate seamless buffer for netflix, etc.

        LOG(1, "Partitioning seamless buffer.\n");

        _GetSeamlessBuffer(&_arFbg[u4FbgId].u4FbResizeMemoryPool, &_arFbg[u4FbgId].u4FbResizeMemoryPoolSize);
        if (_arFbg[u4FbgId].u4FbResizeMemoryPool == 0)
        {
            LOG(0, "Error! Could not allocate seamless buffer!\n");
            ASSERT(_arFbg[u4FbgId].u4FbResizeMemoryPool != 0 && _arFbg[u4FbgId].u4FbResizeMemoryPoolSize != 0);
        }

        _arFbg[u4FbgId].ucSeamlessFbNs = (_arFbg[u4FbgId].u4FbResizeMemoryPoolSize / (u4SeamlessYSize + u4SeamlessCSize));

        LOG(1, "Seamless Config: FbNs(%d) Y,C(0x%8x, 0x%8x).\n", _arFbg[u4FbgId].ucSeamlessFbNs, u4SeamlessYSize, u4SeamlessCSize);
        u4Addr = _arFbg[u4FbgId].u4FbResizeMemoryPool;
        
        for (u4FbIdx = 0; ((u4FbIdx < _arFbg[u4FbgId].ucSeamlessFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
        {
            _arFbg[u4FbgId].au4ResizeAddrY[u4FbIdx] = u4Addr;
            FBM_GFX_MEMSET((UINT8*)u4Addr, FBM_FBG_1080HD_H, FBM_FBG_1080HD_V, 0x0);
            u4Addr += u4SeamlessYSize;

            _arFbg[u4FbgId].au4ResizeAddrC[u4FbIdx] = u4Addr;
            FBM_GFX_MEMSET((UINT8*)u4Addr, FBM_FBG_1080HD_H, (FBM_FBG_1080HD_V >> 1), 0x80);
            u4Addr += u4SeamlessCSize;
        }

        // create semaphore, Empty Q
        VERIFY(_FBM_csema_reset(&_arFbg[u4FbgId].hEmptyResizeQSemaphore, _arFbg[u4FbgId].hMutex, (INT32)_arFbg[u4FbgId].ucSeamlessFbNs) == OSR_OK);

        // put frames into empty Q
        for (u4FbIdx = 0; u4FbIdx < _arFbg[u4FbgId].ucSeamlessFbNs; u4FbIdx++)
        {
            _FBM_PutFrameBufferToEmptyResizeQ(u4FbgId, (UCHAR)(u4FbIdx));
        }

        //_arFbg[u4FbgId].fgEnableNewSeamless = TRUE;
    }
    return;
}

static void _InitSeamlessBuffer(void)
{
#ifdef CC_ENABLE_SEAMLESS_FOR_2D
    FBM_POOL_T* prFbmPool = NULL;
    UCHAR ucCount;
    UINT32 u4Addr, u4ResizePoolSize;
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);

    u4Addr = 0;
#ifdef FBM_SUPPORT_4K2K_SEAMLESS
    u4ResizePoolSize = ((FBM_FBG_TYPE_4K2K_Y_SIZE * 3) >> 1) * 2;
#else
    u4ResizePoolSize = ((FBM_FBG_TYPE_1080HD_Y_SIZE * 3) >> 1) * 2;
#endif
    
    if (prFbmPool)
    {
        u4Addr = prFbmPool->u4Addr + prFbmPool->u4Size - u4ResizePoolSize - FMB_EXTRA_FOR_SINGLE_H264_LV5;
    }

    for (ucCount=0; ucCount < MAX_SEAMLESS_BUFF_COUNT; ucCount++)
    {
        _arSeamlessInfo[ucCount].u4Addr = u4Addr;
        _arSeamlessInfo[ucCount].u4Size = u4ResizePoolSize;
        _arSeamlessInfo[ucCount].fgInUse = FALSE;
        u4Addr -= u4ResizePoolSize;
    }
#endif
}
static void _FreeSeamlessBuffer(UINT32 u4BufferAddr)
{
    UCHAR ucCount;
    for (ucCount=0; ucCount < MAX_SEAMLESS_BUFF_COUNT; ucCount++)
    {
        if (_arSeamlessInfo[ucCount].u4Addr == u4BufferAddr)
        {
            ASSERT(_arSeamlessInfo[ucCount].fgInUse == TRUE);
            _arSeamlessInfo[ucCount].fgInUse = FALSE;
            break;
        }
    }
}

static void _GetSeamlessBuffer(UINT32 *pu4BufferAddr, UINT32 *pu4BufferSize)
{
    UCHAR ucCount;
    
    if(pu4BufferAddr == NULL || pu4BufferSize == NULL)
    {
        ASSERT(0);
        return;
    }

    for (ucCount=0; ucCount < MAX_SEAMLESS_BUFF_COUNT; ucCount++)
    {
        if (_arSeamlessInfo[ucCount].fgInUse == FALSE)
        {
            *pu4BufferAddr = _arSeamlessInfo[ucCount].u4Addr;
            *pu4BufferSize = _arSeamlessInfo[ucCount].u4Size;
            _arSeamlessInfo[ucCount].fgInUse = TRUE;
            break;
        }
    }
}


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** FBM_Init
 *  FBM initialize
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_Init(void)
{
    if (_FbmInitiated == 0)
    {
        UINT32 u4Idx;

        _FbmInitiated = 1;

        FBM_MEMSET((void*)_arFbg, 0, (sizeof(FBM_FBG_T) * FBG_MAX_NS));

        FBM_MEMSET((void*)(&_arFbmCbFunc), 0, sizeof(FBM_CB_FUNC_T));

        // register GFX_Memset() into FBM module
#if !defined(CC_B2R_EMULATION) && !defined(MPV_SEMI_HOST)
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_MEMSET, (UINT32)GFX_Memset);
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_TASK_MEMSET, (UINT32)GFX_Task_Memset);
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_TASK_MEMSET_DONE, (UINT32)GFX_Task_Check_Done);
#endif

        // create FBG mutex
        VERIFY(x_sema_create(&_hFbgMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);

        // init frame buffer group
        for (u4Idx = 0; u4Idx < FBG_MAX_NS; u4Idx++)
        {
            _arFbg[u4Idx].ucFbgType = FBM_FBG_TYPE_UNKNOWN;
            _arFbg[u4Idx].ucFbgId = FBM_FBG_ID_UNKNOWN;
            _arFbg[u4Idx].ucFbgCm = FBM_CM_UNKNOWN;

            _arFbg[u4Idx].ucStcSrc = STC_SRC_NS;
            _arFbg[u4Idx].ucAvSyncMode = VID_SYNC_MODE_NS;
            _arFbg[u4Idx].ucPlayMode = FBM_FBG_DTV_MODE;
            _arFbg[u4Idx].u4AvSyncStatus = FBM_FBG_AVSYNC_DISABLE;

            // create semaphore
            VERIFY(x_sema_create(&_arFbg[u4Idx].hMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);

#ifdef CC_VDP_FULL_ISR
            _arFbg[u4Idx].prState = &_rState;
#endif

            // create semaphore
            VERIFY(_FBM_csema_create(&_arFbg[u4Idx].hEmptyQSemaphore, _arFbg[u4Idx].hMutex, 0) == OSR_OK);

            // create semaphore
            VERIFY(_FBM_csema_create(&_arFbg[u4Idx].hEmptyBQSemaphore, _arFbg[u4Idx].hMutex, 0) == OSR_OK);

            // create semaphore
            VERIFY(_FBM_csema_create(&_arFbg[u4Idx].hEmptyResizeQSemaphore, _arFbg[u4Idx].hMutex, 0) == OSR_OK);


#ifdef CC_VDP_FULL_ISR
            _arFbg[u4Idx].hEmptyQSemaphore.prState = &_rState;
            _arFbg[u4Idx].hEmptyBQSemaphore.prState = &_rState;
            _arFbg[u4Idx].hEmptyResizeQSemaphore.prState = &_rState;
#endif
        }

        // initialize frame buffer pool
        _prPool = _FBM_PoolInit();
        ASSERT(_prPool != NULL);

        // initialize frame buffer
        _FBM_FbInit(_arFbg);
        FBM_Inst_Init();
        _InitSeamlessBuffer();

        _u4NextFbgId = 0;
    }
}


//-------------------------------------------------------------------------
/** FBM_GetFbgNs
 *  Get maximum FBG number
 *  @return NONE.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFbgNs(void)
{
    return FBG_MAX_NS;
}

//-------------------------------------------------------------------------
/** FBM_CalcRefNum 
 *  Calc the MV buffer Ns for H264 only
 *  @return Ns.
 */
//-------------------------------------------------------------------------

static UINT32 FBM_CalcRefNum(UCHAR ucFbgType, UINT32 u4VDecFmt, UINT32 u4Width, UINT32 u4Height, UINT8 u1AppMode)
{
    UINT32 u4ReferenceNs = 0;
    
    if ((u4VDecFmt == FBM_VDEC_H264) && (u1AppMode != FBM_FBG_APP_SKYPE))
    {
        // Calculate Reference Frame Number
        switch(ucFbgType)
        {
            case FBM_FBG_TYPE_4K2K:
                u4ReferenceNs = FBM_FBG_H264_4K2K_REFERNCE_NS;  // 4 ref max!!!
                break;
            case FBM_FBG_TYPE_SBSHD: 
            case FBM_FBG_TYPE_TNBHD:
            case FBM_FBG_TYPE_1080HD:
                u4ReferenceNs = (((FBM_FBG_1080HD_H * FBM_FBG_1080HD_V) * FBM_FBG_H264_FHD_REFERNCE_NS) / (u4Width * u4Height));//FBM_FBG_H264_FHD_REFERNCE_NS;                      
                break;
            case FBM_FBG_TYPE_720HD:
                u4ReferenceNs = (((FBM_FBG_720HD_H * FBM_FBG_720HD_V) * FBM_FBG_TYPE_720HD_H264_R_NS) / (u4Width * u4Height));//FBM_FBG_TYPE_720HD_H264_R_NS;                                              
                break;
            case FBM_FBG_TYPE_PAL:
                u4ReferenceNs = (((FBM_FBG_PAL_H * FBM_FBG_PAL_V) * FBM_FBG_TYPE_PAL_H264_R_NS) / (u4Width * u4Height));//FBM_FBG_TYPE_PAL_H264_R_NS;                                       
                break;
            default:
                u4ReferenceNs = (((FBM_FBG_1080HD_H * FBM_FBG_1080HD_V) * FBM_FBG_H264_FHD_REFERNCE_NS) / (u4Width * u4Height));
                LOG(0, "Warning! No matched fbg type!\n");
                break;
        }
    }
    else if((u4VDecFmt == FBM_VDEC_H264) && (u1AppMode == FBM_FBG_APP_SKYPE))
    {
        u4ReferenceNs = 4;
    }
    
    if(u4ReferenceNs > FBM_FBG_H264_REFERNCE_MAX_NS)
    {
        u4ReferenceNs = FBM_FBG_H264_REFERNCE_MAX_NS;
    }

    return u4ReferenceNs;
}
//-------------------------------------------------------------------------
/** FBM_CalcBufNum
 *  Calc the buffer Ns for w,h
 *  @return Ns.
 */
//-------------------------------------------------------------------------

UINT32 FBM_CalcBufNum(UCHAR ucFbgId, UCHAR ucFbgType, UINT32 u4VDecFmt, UINT32 u4Width, UINT32 u4Height,
    UINT32 u4PoolSize, UINT32 u4YSize, UINT32 u4CSize, UINT8 u1AppMode, UINT32* pu4ExtFbNs)
{
    BOOL fgEnableH264V5Support = FALSE;
    UINT32 u4FbCount = 0;
    UINT32 u4FbSize = 0;
    UINT32 u4ExtraFbNs = 0;
    FBM_POOL_T* prFbmPool = NULL;
    
    if ((ucFbgId != FBM_FBG_ID_UNKNOWN) && (ucFbgId < FBG_MAX_NS))
    {
        fgEnableH264V5Support = _arFbg[ucFbgId].fgEnableH264V5Support;
    }
    UNUSED(fgEnableH264V5Support);
    UNUSED(prFbmPool);
    UNUSED(u4ExtraFbNs);

    u4FbSize = u4YSize + u4CSize;
    u4FbCount = u4PoolSize / u4FbSize;

    if(u1AppMode == FBM_FBG_APP_MHP)
    {
        u4FbCount = 2;
        return u4FbCount;
    }

    switch (u4VDecFmt)
    {
        case FBM_VDEC_H264:
        {
            UCHAR ucMvBufNs;

            // MV Frame Buffer Number = Reference Frame Number + 1;
            // MV Frame Buffer Size = (MV Frame Buffer Number * Y Size) / 4
            ucMvBufNs = FBM_CalcRefNum(ucFbgType, u4VDecFmt, u4Width, u4Height, u1AppMode) + 1; //one decoder frame

            // Calculate Total Frame Number = (Total Size - MV Size) / (Y Size + C Size)
            u4FbCount = ((u4PoolSize - ((ucMvBufNs * u4YSize) >> 2)) / u4FbSize);
            
#if defined(CC_H264_LV5_SUPPORT_SHARE_POOL) || defined(CC_H264_LV5_SUPPORT_SINGLE_POOL)
            if (fgEnableH264V5Support)
            {
                UINT32 u4ExtraReferenceNs = 0;
#if defined(CC_H264_LV5_SUPPORT_SHARE_POOL)
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
        
                ASSERT(FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL > FBM_FBG_H264_FHD_REFERNCE_NS);
                u4ExtraReferenceNs = FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL - FBM_FBG_H264_FHD_REFERNCE_NS;
                u4ExtraFbNs = prFbmPool ? ((prFbmPool->u4Size - ((u4ExtraReferenceNs * u4YSize) >> 2))/u4FbSize) : 0;
#elif defined(CC_H264_LV5_SUPPORT_SINGLE_POOL)
                ASSERT(FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL > FBM_FBG_H264_FHD_REFERNCE_NS);
                u4ExtraReferenceNs = FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL - FBM_FBG_H264_FHD_REFERNCE_NS;
                u4ExtraFbNs = ((FMB_EXTRA_FOR_SINGLE_H264_LV5 - ((u4ExtraReferenceNs * u4YSize) >> 2))/u4FbSize);
#endif
                ASSERT(u4ExtraFbNs >= u4ExtraReferenceNs);
                if (u4ExtraFbNs > u4ExtraReferenceNs)
                {
                    u4ExtraFbNs = u4ExtraReferenceNs;
                }
            }
#endif

            if (u1AppMode == FBM_FBG_APP_SKYPE)
            {
                u4FbCount = 7;
                u4ExtraFbNs = 0;
            }
                        
            if ((ucFbgId != FBM_FBG_ID_UNKNOWN) && (ucFbgId < FBG_MAX_NS))
            {
                _arFbg[ucFbgId].ucMvBufNs = ucMvBufNs;
            }
        }
        break;
        case FBM_VDEC_JPEG:
        {
            if ((u4YSize == u4CSize) && (u1AppMode != FBM_FBG_APP_MTIMAGE))
            {   //FBM_CM_422
                u4FbCount = 2;
            }
            else
            {
                u4FbCount = 4;
            }
            if (u1AppMode == FBM_FBG_APP_OMX_DISP)
            {
                u4FbCount += 2;
            }
#if defined(FBM_4K2K_SUPPORT) || defined(FBM_4K2K_JPG_SUPPORT)// 4k2k need 5 fb! 
            u4FbCount = 5;
#endif
        }
        break;
        case FBM_VDEC_MPEG2:
        {
            u4FbCount = (u1AppMode == FBM_FBG_APP_OMX_DISP) ? FBM_FLOOR_FIX(u4FbCount, 10, 6) : FBM_FLOOR_FIX(u4FbCount, 6, 6);           
        }
        break;
        case FBM_VDEC_VC1:
        {
            u4FbCount = (u1AppMode == FBM_FBG_APP_OMX_DISP) ? FBM_FLOOR_FIX(u4FbCount, 10, 5) : FBM_FLOOR_FIX(u4FbCount, 5, 5);
        }
        break;
        case FBM_VDEC_RV:
        {
            // reserve three frame for RPR mode
            // one is for reference frame, the other two frame is for seamless display
            if(u1AppMode == FBM_FBG_APP_OMX_DISP)
            {
                LOG(0, "[%s-%d]OMX not support RV yet....\n", __FUNCTION__, __LINE__);
            }
            if (u4FbCount >= 10)
            {
                u4FbCount -= 2; // usine two frame buffer for rpr mode
            }
#ifdef CC_USE_DDI
            u4FbCount = FBM_MIN(u4FbCount, 5);
#endif
        }
        break;
        case FBM_VDEC_VP6:
        {
            u4FbCount = (u1AppMode == FBM_FBG_APP_OMX_DISP) ? FBM_FLOOR_FIX(u4FbCount, 10, 5) : FBM_FLOOR_FIX(u4FbCount, 5, 5);
        }
        break;
        case FBM_VDEC_VP8:
        {
            u4FbCount = (u1AppMode == FBM_FBG_APP_OMX_DISP) ? FBM_FLOOR_FIX(u4FbCount, 10, 6) : FBM_FLOOR_FIX(u4FbCount, 6, 6);
        }
        break;
        case FBM_VDEC_MPEG4:
        {
            u4FbCount = (u1AppMode == FBM_FBG_APP_OMX_DISP) ? FBM_FLOOR_FIX(u4FbCount, 10, 6) : FBM_FLOOR_FIX(u4FbCount, 6, 6);
        }
        break;
        default:
        {
            //do nothing
        }
        break;
    }


    if (u4FbCount >= FBM_MAX_FB_NS_PER_GROUP/2) //FB enough, not need extra FB.
    {
        u4ExtraFbNs = 0;
        u4FbCount = FBM_MAX_FB_NS_PER_GROUP/2-1;
    }
    else if (u4FbCount + u4ExtraFbNs >= FBM_MAX_FB_NS_PER_GROUP/2) //FB not enough, but not need all extra FB.
    {
        u4ExtraFbNs = FBM_MAX_FB_NS_PER_GROUP/2 - u4FbCount - 1;
        u4FbCount = FBM_MAX_FB_NS_PER_GROUP/2-1;
    }
    else
    {
        u4FbCount += u4ExtraFbNs;
    }

    ASSERT(u4FbCount <= FBM_MAX_FB_NS_PER_GROUP/2);
    
    if ((ucFbgId != FBM_FBG_ID_UNKNOWN) && (ucFbgId < FBG_MAX_NS))
    {
        *pu4ExtFbNs = u4ExtraFbNs;
    }

    if (u4VDecFmt == FBM_VDEC_H264)
    {
        if (u1AppMode == FBM_FBG_APP_OMX_DISP)
        {
            //B2r render need 4 lock frame in stage fright: 
            //User space set render will get by b2r(set register) at next vsync. 
            //B2R register will enable render at next vsync. (double buffer)
            //After render, fb will be release at next vysnc. 
            ASSERT(u4FbCount >= FBM_FBG_H264_FHD_REFERNCE_NS + 1 + 4); // 1decoder + 4 B2R render
        }
        LOG(2, "H264 FbgType(%d) H/V(%d, %d) Ns(%d) Mv(%d) ExtNs(%d) TotalSize(%d)\n",
            ucFbgType, u4Width, u4Height, u4FbCount, ((ucFbgId < FBG_MAX_NS) ? _arFbg[ucFbgId].ucMvBufNs : 0), u4ExtraFbNs, u4PoolSize);
    }
    else
    {
        LOG(2, "FbgType(%d) Codec(%d) W/H(%d, %d) APP(%d) Ns(%d)\n", ucFbgType, u4VDecFmt, u4Width, u4Height, u1AppMode, u4FbCount);
    }
    return u4FbCount;
}


//-------------------------------------------------------------------------
/** FBM_CreateGroupExt
 *  Create FBM Group
 *  @param  ucFbgType   FBG type.
 *  @return FBG index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_CreateGroupExt(UCHAR ucFbgType, UINT32 u4VDecFmt,
                         UINT32 u4HSize, UINT32 u4VSize, FBM_CREATE_FBG_PAR_T *prPar)
{
    UCHAR ucFbpListNs;

    UINT32 u4FbgTypeValid;
    UINT32 u4MallocSize;

    UINT32 u4FbgId;
    UINT32 u4FbgIdx;
    UINT32 u4TargetFbgIdx;
    UINT32 u4ListIdx;
    UINT32 u4FbIdx;

    FBP_LIST_T* prFbpList;
    FBM_POOL_T* prFbmPool = NULL;
    
    if(prPar && (prPar->ucCftFbgId == 0))
    {
        prPar->ucCftFbgId = FBM_FBG_ID_UNKNOWN;
    }
    else if (prPar)
    {
        prPar->ucCftFbgId--;
    }

    VERIFY(x_sema_lock(_hFbgMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

    LOG(1, "FBM_CreateGroupExt: FbgType(%d) Codec(%d) HV(%d, %d) Thumbnail(%d) seamlessMode(%d).\n",ucFbgType,u4VDecFmt,u4HSize,u4VSize,
		prPar ? (prPar->fgThumbnailMode) : 0,
		prPar ? (prPar->eSeamlessMode):0);
	
    _FBM_GetPoolList(&prFbpList, &ucFbpListNs);

    // Check frame buffer group type
    u4FbgTypeValid = 0;
    for (u4ListIdx = 0; u4ListIdx < ucFbpListNs; u4ListIdx++)
    {
        if (ucFbgType == prFbpList[u4ListIdx].ucType)
        {
            u4FbgTypeValid = 1;
        }
    }
    if ((u4FbgTypeValid == 0) || (u4VDecFmt == FBM_VDEC_UNKNOWN))
    {
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }
    if(prPar && (prPar->ucCftFbgId != FBM_FBG_ID_UNKNOWN) && (prPar->ucCftFbgId < FBG_MAX_NS))
    {
        for (u4FbIdx = _arFbg[prPar->ucCftFbgId].ucFbNsBase;
                ((u4FbIdx < _arFbg[prPar->ucCftFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
        {
            if (_arFbg[prPar->ucCftFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_DECODE || 
                _arFbg[prPar->ucCftFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_READY ||
                _arFbg[prPar->ucCftFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_FIELD_READY)
            {
                if (_arFbg[prPar->ucCftFbgId].afgRefList[u4FbIdx] == 1)
                {
                    _arFbg[prPar->ucCftFbgId].afgRefList[u4FbIdx] = 0;
                }
                _arFbg[prPar->ucCftFbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_EMPTY;
                _FBM_PutFrameBufferToEmptyQ(prPar->ucCftFbgId, (UCHAR)(u4FbIdx));
                LOG(3, "Flush DECODE/READY FB(%d) to emptyQ.\n", u4FbIdx);
            }
        }
        u4FbgId = prPar->ucCftFbgId;
        _arFbg[u4FbgId].u4FbMemoryPool2Size = _arFbg[u4FbgId].u4FbMemoryPoolSize;
        _arFbg[u4FbgId].u4FbMemoryPool2 = _arFbg[u4FbgId].u4FbMemoryPool;
    }
    else
    {
    // Search for empty frame buffer group
    u4FbgId = FBM_FBG_ID_UNKNOWN;
    u4TargetFbgIdx = _u4NextFbgId;
    for (u4FbgIdx = 0; u4FbgIdx < FBG_MAX_NS; u4FbgIdx++)
    {
        if (_arFbg[u4TargetFbgIdx].ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            u4FbgId = u4TargetFbgIdx;

            u4FbgIdx = FBG_MAX_NS;

            if (++u4TargetFbgIdx >= FBG_MAX_NS)
            {
                u4TargetFbgIdx = 0;
            }
            _u4NextFbgId = u4TargetFbgIdx;
        }

        if (++u4TargetFbgIdx >= FBG_MAX_NS)
        {
            u4TargetFbgIdx = 0;
        }
    }
    if (u4FbgId == FBM_FBG_ID_UNKNOWN)
    {
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }

#if defined(CC_H264_LV5_SUPPORT_SHARE_POOL)
    if (u4VDecFmt == FBM_VDEC_H264 && prPar && (prPar->u1VdecProfile & FBM_H264_LV5_PROFILE))
    {
        for (u4FbgIdx = 0; u4FbgIdx < FBG_MAX_NS; u4FbgIdx++)
        {
            if (_arFbg[u4FbgIdx].ucFbgId != FBM_FBG_ID_UNKNOWN)
            {
                LOG(0, "Create FBG error: Could not find enough MPEG pools to support H264 LV5 profile!\n");
                VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
                return FBM_FBG_ID_UNKNOWN;
            }
        }
        // enable share mpeg pool
        _arFbg[u4FbgId].fgEnableH264V5Support = TRUE;
    }
#elif defined(CC_H264_LV5_SUPPORT_SINGLE_POOL)
    if (u4VDecFmt == FBM_VDEC_H264 && prPar && (prPar->u1VdecProfile & FBM_H264_LV5_PROFILE))
    {
        for (u4FbgIdx = 0; u4FbgIdx < FBG_MAX_NS; u4FbgIdx++)
        {
            if (_arFbg[u4FbgIdx].ucFbgId != FBM_FBG_ID_UNKNOWN && _arFbg[u4FbgIdx].fgEnableH264V5Support == TRUE)
            {
                LOG(0, "Create FBG error: Extra MPEG pool is used by H264 for LV5 profile!\n");
                VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
                return FBM_FBG_ID_UNKNOWN;
            }
        }

        // enable share mpeg pool
        _arFbg[u4FbgId].fgEnableH264V5Support = TRUE;
    }
#endif

#ifdef FBM_CS_LOG
    _u4EQHandle = (UINT32)_arFbg[u4FbgId].hEmptyQSemaphore.hBinary;
#endif

    _arFbg[u4FbgId].u4VDecFmt = u4VDecFmt;


    // Clear Flag
    _arFbg[u4FbgId].u4FbgFlag = 0;

    // Determine number of frame buffer, later
    _arFbg[u4FbgId].ucFbNs = FBM_MAX_FB_NS_PER_GROUP;

    // Allocalte memory for picture header
    u4MallocSize = _arFbg[u4FbgId].ucFbNs * (sizeof(FBM_PIC_HDR_T));
    _arFbg[u4FbgId].pMallocAddr = x_mem_alloc(u4MallocSize);
    if (_arFbg[u4FbgId].pMallocAddr == NULL)
    {
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }
    FBM_MEMSET(_arFbg[u4FbgId].pMallocAddr, 0, u4MallocSize);
    _arFbg[u4FbgId].prPicHdr = (FBM_PIC_HDR_T *)_arFbg[u4FbgId].pMallocAddr;

    // Init frame buffer status
    for (u4FbIdx = 0; u4FbIdx < _arFbg[u4FbIdx].ucFbNs; u4FbIdx++)
    {
        _arFbg[u4FbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_UNKNOWN;
    }
    }

    if (prPar && prPar->fgExternalAddr)
    {
        //Not alloc
        LOG(1, "Not alloc FB Pool As FB addr will set external.\n");
    }
    else if (prPar && prPar->fgCreateFromInst)
    {
        _arFbg[u4FbgId].fgCreateFromInst = TRUE;
    }
#if (defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) || defined(CC_VOMX_TV_COEXIST)) && defined(CC_FBM_FULL_3D_SUPPORT) || defined (FBM_4K1K_VIDEO_SUPPORT)
    else if(ucFbgType == FBM_FBG_TYPE_SBSHD || ucFbgType == FBM_FBG_TYPE_TNBHD)
    {
        UINT32 u4PoolSize = 0;
        _arFbg[u4FbgId].u4FbMemoryPool = 0;
        _arFbg[u4FbgId].u4FbMemoryPool2 = 0;
        
        u4PoolSize = _FBM_PoolAllocate(FBM_FBG_TYPE_1080HD, &_arFbg[u4FbgId].u4FbMemoryPool);
        if (u4PoolSize == 0 || _arFbg[u4FbgId].u4FbMemoryPool == (UINT32)(NULL))
        {
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }
            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
        ASSERT(u4PoolSize == FBM_FBG_TYPE_1080HD_H264_SIZE);

        u4PoolSize = _FBM_PoolAllocate(FBM_FBG_TYPE_1080HD, &_arFbg[u4FbgId].u4FbMemoryPool2);
        if (u4PoolSize == 0 || _arFbg[u4FbgId].u4FbMemoryPool2 == (UINT32)(NULL))
        {
            _FBM_PoolRelease(_arFbg[u4FbgId].u4FbMemoryPool);
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }
            
            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
        ASSERT(u4PoolSize == FBM_FBG_TYPE_1080HD_H264_SIZE);
        _arFbg[u4FbgId].u4FbMemoryPoolSize = FBM_FBG_TYPE_1080HD_H264_SIZE*2;
    }
#endif
#ifdef FBM_4K2K_JPG_SUPPORT  // FBM_4K2K_SUPPORT
    else if (ucFbgType == FBM_FBG_TYPE_4K2K_JPEG)
    {
        UINT32 u4PoolSize = 0;
        _arFbg[u4FbgId].u4FbMemoryPool = 0;
        _arFbg[u4FbgId].u4FbMemoryPool2 = 0;

        u4PoolSize = _FBM_PoolAllocate(FBM_FBG_TYPE_4K2K_JPEG, &_arFbg[u4FbgId].u4FbMemoryPool);
        if (u4PoolSize == 0 || _arFbg[u4FbgId].u4FbMemoryPool == (UINT32)(NULL))
        {
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }
            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
        ASSERT(u4PoolSize == FBM_FBG_TYPE_4K2K_JPEG_SIZE);

        u4PoolSize = _FBM_PoolAllocate(FBM_FBG_TYPE_4K2K_JPEG, &_arFbg[u4FbgId].u4FbMemoryPool2);
        if (u4PoolSize == 0 || _arFbg[u4FbgId].u4FbMemoryPool2 == (UINT32)(NULL))
        {
            _FBM_PoolRelease(_arFbg[u4FbgId].u4FbMemoryPool);
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }

            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
        ASSERT(u4PoolSize == FBM_FBG_TYPE_4K2K_JPEG_SIZE);
        _arFbg[u4FbgId].u4FbMemoryPoolSize = FBM_FBG_TYPE_1080HD_H264_SIZE*2;
    }
#endif
#ifdef FBM_4K2K_SUPPORT
    else if (ucFbgType == FBM_FBG_TYPE_4K2K)
    {
        UINT32 u4PoolSize = 0;
        _arFbg[u4FbgId].u4FbMemoryPool = 0;
        _arFbg[u4FbgId].u4FbMemoryPool2 = 0;

        u4PoolSize = _FBM_PoolAllocate(FBM_FBG_TYPE_4K2K, &_arFbg[u4FbgId].u4FbMemoryPool);
        if (u4PoolSize == 0 || _arFbg[u4FbgId].u4FbMemoryPool == (UINT32)(NULL))
        {
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }
            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
        ASSERT(u4PoolSize == FBM_FBG_TYPE_4K2K_7FB_SIZE_PART1);

        u4PoolSize = _FBM_PoolAllocate(FBM_FBG_TYPE_4K2K_EXTRA, &_arFbg[u4FbgId].u4FbMemoryPool2);
        if (u4PoolSize == 0 || _arFbg[u4FbgId].u4FbMemoryPool2 == (UINT32)(NULL))
        {
            _FBM_PoolRelease(_arFbg[u4FbgId].u4FbMemoryPool);
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }

            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
        ASSERT(u4PoolSize == FBM_FBG_TYPE_4K2K_7FB_SIZE_PART2);
        _arFbg[u4FbgId].u4FbMemoryPoolSize = FBM_FBG_TYPE_4K2K_7FB_SIZE_PART1 + FBM_FBG_TYPE_4K2K_7FB_SIZE_PART2;
    }
#endif
    else
    {
        // Allocate frame buffer pool
        _arFbg[u4FbgId].u4FbMemoryPoolSize = 0;
        _arFbg[u4FbgId].u4FbMemoryPool = 0;
        //_arFbg[u4FbgId].ucFbNsBase = 0;
        if(prPar && (prPar->ucCftFbgId != FBM_FBG_ID_UNKNOWN) && (prPar->ucCftFbgId < FBG_MAX_NS))
        {
            _arFbg[u4FbgId].ucFbOldEmptyCnt = _arFbg[u4FbgId].rEmptyQ.ucCount;
            for (u4FbIdx = _arFbg[u4FbgId].ucFbNsBase; u4FbIdx < _arFbg[u4FbgId].ucFbNs; u4FbIdx++)
            {
                if (_arFbg[u4FbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_UNKNOWN)
                {
                    _arFbg[u4FbgId].ucFbOldEmptyCnt++;
                }
            }

            FBM_MEMSET((VOID *)&_arFbg[u4FbgId].rEmptyQ, 0, sizeof(_arFbg[u4FbgId].rEmptyQ));
            
            _arFbg[u4FbgId].ucFbNsOld = _arFbg[u4FbgId].ucFbNs;
            _arFbg[u4FbgId].ucFbNsOldBase = _arFbg[u4FbgId].ucFbNsBase;
            _arFbg[u4FbgId].ucFbNsBase = (_arFbg[u4FbgId].ucFbNsOldBase ? 0 : FBM_MAX_FB_NS_PER_GROUP/2);
            FBM_MEMSET((VOID *)(_arFbg[u4FbgId].aucFbStatus + _arFbg[u4FbgId].ucFbNsBase), FBM_FB_STATUS_UNKNOWN, FBM_MAX_FB_NS_PER_GROUP/2);
            
            if ((prPar->eSeamlessMode & SEAMLESS_2FBS_2POOL) == SEAMLESS_2FBS_2POOL)
            {
                _arFbg[u4FbgId].u4FbMemoryPoolSize = _FBM_PoolAllocate(ucFbgType, &_arFbg[u4FbgId].u4FbMemoryPool);

                //MPEG2 may out of range while stage fright more 3FB enable.
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
                if (prFbmPool 
                    && (prFbmPool->u4Addr == _arFbg[u4FbgId].u4FbMemoryPool) 
                    && (_arFbg[u4FbgId].u4FbMemoryPoolSize > prFbmPool->u4Size))
                {
                    _arFbg[u4FbgId].u4FbMemoryPoolSize = prFbmPool->u4Size;
                }
            }
            if (_arFbg[u4FbgId].u4FbMemoryPoolSize && _arFbg[u4FbgId].u4FbMemoryPool)//alloc success
            {
                _arFbg[u4FbgId].eDynamicMode = FBM_DYNAMIC_DIFF;
            }
            else
            {
				UINT32 u4RequestPoolSize =0;
				
                _arFbg[u4FbgId].u4FbMemoryPool = _arFbg[u4FbgId].u4FbMemoryPool2;
				
				if (_FBM_QueryCurPoolSize(ucFbgType,&u4RequestPoolSize))
				{
					_arFbg[u4FbgId].u4FbMemoryPoolSize = u4RequestPoolSize;
				}
				else
				{
					_arFbg[u4FbgId].u4FbMemoryPoolSize = _arFbg[u4FbgId].u4FbMemoryPool2Size;
				}
				
                _arFbg[u4FbgId].eDynamicMode = FBM_DYNAMIC_SAME;
            }
        }
        else
        {
            _arFbg[u4FbgId].u4FbMemoryPoolSize = _FBM_PoolAllocate(ucFbgType, &_arFbg[u4FbgId].u4FbMemoryPool);
            
            //MPEG2 may out of range while stage fright more 3FB enable.
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
            if (prFbmPool 
                && (prFbmPool->u4Addr == _arFbg[u4FbgId].u4FbMemoryPool) 
                && (_arFbg[u4FbgId].u4FbMemoryPoolSize > prFbmPool->u4Size))
            {
                _arFbg[u4FbgId].u4FbMemoryPoolSize = prFbmPool->u4Size;
            }
        }
        LOG(2, "AllocFBM = (0x%8x + 0x%x).\n", _arFbg[u4FbgId].u4FbMemoryPool, _arFbg[u4FbgId].u4FbMemoryPoolSize);
            
        if ((_arFbg[u4FbgId].u4FbMemoryPoolSize == 0) ||
                (_arFbg[u4FbgId].u4FbMemoryPool == (UINT32)(NULL)))
        {
            if (_arFbg[u4FbgId].pMallocAddr != NULL)
            {
                x_mem_free(_arFbg[u4FbgId].pMallocAddr);
                _arFbg[u4FbgId].pMallocAddr = NULL;
            }
            VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
            return FBM_FBG_ID_UNKNOWN;
        }
    }

    // Settings
    _arFbg[u4FbgId].eSeamlessMode = prPar ? prPar->eSeamlessMode : SEAMLESS_NONE;
    if (((_arFbg[u4FbgId].eSeamlessMode & SEAMLESS_2FBS_2POOL) == SEAMLESS_2FBS_2POOL)
        && (_arFbg[u4FbgId].eDynamicMode != FBM_DYNAMIC_DIFF))
    {
        _arFbg[u4FbgId].eSeamlessMode &= ~(SEAMLESS_2FBS_2POOL);
        _arFbg[u4FbgId].eSeamlessMode |= (SEAMLESS_ONE_FBS);
    }

    _arFbg[u4FbgId].ucFbgType = ucFbgType;
    _arFbg[u4FbgId].ucFbgId = (UCHAR)u4FbgId;

    if(prPar && prPar->fgSwHEVC)
        _arFbg[u4FbgId].fgCreateFromInst = TRUE;

    // Config color mode
    LOG(1, "Config FBG(%d): SmlMode(%x) FBNs(%d-%d %d-%d) Pool(0x%x + 0x%x) EmptyCnt(%d).\n", 
            u4FbgId, _arFbg[u4FbgId].eSeamlessMode, _arFbg[u4FbgId].ucFbNsOldBase, _arFbg[u4FbgId].ucFbNsOld, _arFbg[u4FbgId].ucFbNsBase, _arFbg[u4FbgId].ucFbNs,
            _arFbg[u4FbgId].u4FbMemoryPool, _arFbg[u4FbgId].u4FbMemoryPoolSize, _arFbg[u4FbgId].ucFbOldEmptyCnt);
    if (prPar && prPar->fgCreateFromInst)
    {
        FBM_PreConfigColorMode((UCHAR)(u4FbgId), _u1FbgColorMode, u4HSize, u4VSize, prPar);
    }
    else
    {
        FBM_ConfigColorMode((UCHAR)(u4FbgId), _u1FbgColorMode, u4HSize, u4VSize, prPar);
    }
    // Detect new seamless for 3D
    _DetectNewSeamless(u4FbgId, u4HSize, u4VSize, prPar);

    _u1FbgColorMode = FBM_CM_420;

    // [LOG] FBM Create
    SYSLOG(FBM_PREFIX + 99, u4FbgId, ucFbgType, 0);

    // Apply global value
    if(FBM_ChkFrameBufferGlobalFlagIsr(0xFF, FBM_FLAG_FB_DECODE_ONLY))
    {
        _arFbg[u4FbgId].u4FbgFlag |= FBM_FLAG_DISP_READY;
    }
    if (prPar && prPar->fgThumbnailMode)
    {
        _arFbg[u4FbgId].u4FbgFlag |= FBM_FLAG_THUMBNAIL_MODE;
        _arFbg[u4FbgId].u4FbgFlag |= FBM_FLAG_DISP_READY;
    }

    ASSERT(_arFbg[u4FbgId].hEmptyQSemaphore.hMutex == _arFbg[u4FbgId].hMutex);
    ASSERT(_arFbg[u4FbgId].hEmptyBQSemaphore.hMutex == _arFbg[u4FbgId].hMutex);

    VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);

    return FBM_BYTE(u4FbgId);
}

//-------------------------------------------------------------------------
/** FBM_CreateGroup
 *  Create FBM Group
 *  @param  ucFbgType   FBG type.
 *  @return FBG index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_CreateGroup(UCHAR ucFbgType, UINT32 u4VDecFmt, UINT32 u4HSize, UINT32 u4VSize)
{
    UCHAR ucFbpListNs;

    UINT32 u4FbgTypeValid;
    UINT32 u4MallocSize;

    UINT32 u4FbgId;
    UINT32 u4FbgIdx;
    UINT32 u4TargetFbgIdx;
    UINT32 u4ListIdx;
    UINT32 u4FbIdx;

    FBM_CREATE_FBG_PAR_T rPar;
    FBP_LIST_T* prFbpList;

    VERIFY(x_sema_lock(_hFbgMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

    _FBM_GetPoolList(&prFbpList, &ucFbpListNs);

    // Check frame buffer group type
    u4FbgTypeValid = 0;
    for (u4ListIdx = 0; u4ListIdx < ucFbpListNs; u4ListIdx++)
    {
        if (ucFbgType == prFbpList[u4ListIdx].ucType)
        {
            u4FbgTypeValid = 1;
        }
    }
    if ((u4FbgTypeValid == 0) || (u4VDecFmt == FBM_VDEC_UNKNOWN))
    {
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }

    // Search for empty frame buffer group
    u4FbgId = FBM_FBG_ID_UNKNOWN;
    u4TargetFbgIdx = _u4NextFbgId;
    for (u4FbgIdx = 0; u4FbgIdx < FBG_MAX_NS; u4FbgIdx++)
    {
        if (_arFbg[u4TargetFbgIdx].ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            u4FbgId = u4TargetFbgIdx;

            u4FbgIdx = FBG_MAX_NS;

            if (++u4TargetFbgIdx >= FBG_MAX_NS)
            {
                u4TargetFbgIdx = 0;
            }
            _u4NextFbgId = u4TargetFbgIdx;
        }

        if (++u4TargetFbgIdx >= FBG_MAX_NS)
        {
            u4TargetFbgIdx = 0;
        }
    }
    if (u4FbgId == FBM_FBG_ID_UNKNOWN)
    {
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }

#ifdef FBM_CS_LOG
    _u4EQHandle = (UINT32)_arFbg[u4FbgId].hEmptyQSemaphore.hBinary;
#endif

    _arFbg[u4FbgId].u4VDecFmt = u4VDecFmt;


    // Clear Flag
    _arFbg[u4FbgId].u4FbgFlag = 0;

    // Determine number of frame buffer, later
    _arFbg[u4FbgId].ucFbNs = FBM_MAX_FB_NS_PER_GROUP;

    // Allocalte memory for picture header
    u4MallocSize = _arFbg[u4FbgId].ucFbNs * (sizeof(FBM_PIC_HDR_T));
    _arFbg[u4FbgId].pMallocAddr = x_mem_alloc(u4MallocSize);
    if (_arFbg[u4FbgId].pMallocAddr == NULL)
    {
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }
    FBM_MEMSET(_arFbg[u4FbgId].pMallocAddr, 0, u4MallocSize);
    _arFbg[u4FbgId].prPicHdr = (FBM_PIC_HDR_T *)_arFbg[u4FbgId].pMallocAddr;

    // Init frame buffer status
    for (u4FbIdx = 0; u4FbIdx < _arFbg[u4FbIdx].ucFbNs; u4FbIdx++)
    {
        _arFbg[u4FbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_EMPTY;
    }

    // Allocate frame buffer pool
    _arFbg[u4FbgId].u4FbMemoryPoolSize = _FBM_PoolAllocate(ucFbgType, &_arFbg[u4FbgId].u4FbMemoryPool);

    if ((_arFbg[u4FbgId].u4FbMemoryPoolSize == 0) ||
            (_arFbg[u4FbgId].u4FbMemoryPool == 0))
    {
        if (_arFbg[u4FbgId].pMallocAddr)
        {
            x_mem_free(_arFbg[u4FbgId].pMallocAddr);
            _arFbg[u4FbgId].pMallocAddr = NULL;
        }
        VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
        return FBM_FBG_ID_UNKNOWN;
    }

    // Settings
    _arFbg[u4FbgId].ucFbgType = ucFbgType;
    _arFbg[u4FbgId].ucFbgId = (UCHAR)u4FbgId;

    // Config color mode

    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
    rPar.u1AppMode = FBM_FBG_APP_NORMAL;
    FBM_ConfigColorMode((UCHAR)(u4FbgId), _u1FbgColorMode, u4HSize, u4VSize, &rPar);

    _u1FbgColorMode = FBM_CM_420;

    // [LOG] FBM Create
    SYSLOG(FBM_PREFIX + 99, u4FbgId, ucFbgType, 0);

    // Apply global value
    if(FBM_ChkFrameBufferGlobalFlagIsr(0xFF, FBM_FLAG_FB_DECODE_ONLY))
    {
        _arFbg[u4FbgId].u4FbgFlag |= FBM_FLAG_DISP_READY;
    }

    ASSERT(_arFbg[u4FbgId].hEmptyQSemaphore.hMutex == _arFbg[u4FbgId].hMutex);
    ASSERT(_arFbg[u4FbgId].hEmptyBQSemaphore.hMutex == _arFbg[u4FbgId].hMutex);

    VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);

    return FBM_BYTE(u4FbgId);
}


//-------------------------------------------------------------------------
/** FBM_ReleaseGroup
 *  Release FBM Group
 *  @param  ucFbgType   FBG type.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_ReleaseGroup(UCHAR ucFbgId)
{
    // Notes about mutex semaphore
    // We will not destroy mutex semaphore at release
    // Because OSAI itself is NOT threadsafe
    // Semaphore release function is dangerous

    void* pMallocAddr;
    UINT32 u4FbMemoryPool;
    UINT32 u4FbMemoryPool2;    
    HANDLE_T hMutex;
    UCHAR    ucFbgType;

    COUNTING_SEMAPHORE_T         hEmptyQSemaphore;    // Empty Queue Counting Semaphore
    COUNTING_SEMAPHORE_T         hEmptyBQSemaphore;    // Empty B Queue Counting Semaphore
    COUNTING_SEMAPHORE_T         hEmptyResizeQSemaphore;
    
    UNUSED(u4FbMemoryPool2);
    UNUSED(ucFbgType);
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    LOG(2, "[%s-%d]Fbg(%d) release.\n", __func__, __LINE__, ucFbgId);
    VERIFY(x_sema_lock(_hFbgMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
    FBM_MUTEX_LOCK(ucFbgId);

    // [LOG] FBM Release
    SYSLOG(FBM_PREFIX + 98, ucFbgId, _arFbg[ucFbgId].ucFbgType, 0);

    u4FbMemoryPool = _arFbg[ucFbgId].u4FbMemoryPool;
    u4FbMemoryPool2 = _arFbg[ucFbgId].u4FbMemoryPool2; 
    pMallocAddr = _arFbg[ucFbgId].pMallocAddr;
    ucFbgType = _arFbg[ucFbgId].ucFbgType;

    // backup semaphore
    hMutex = _arFbg[ucFbgId].hMutex;

    hEmptyQSemaphore = _arFbg[ucFbgId].hEmptyQSemaphore;
    hEmptyBQSemaphore = _arFbg[ucFbgId].hEmptyBQSemaphore;
    hEmptyResizeQSemaphore = _arFbg[ucFbgId].hEmptyResizeQSemaphore;

    if (_arFbg[ucFbgId].fgEnableNewSeamless)
    {
        FBM_MUTEX_UNLOCK(ucFbgId); //unlock first for VDP Prescale thread exit normally.
        _VDP_Prescale_Close(_arFbg[ucFbgId].ucSeamlessRzId);
        FBM_MUTEX_LOCK(ucFbgId);
        _FreeSeamlessBuffer(_arFbg[ucFbgId].u4FbResizeMemoryPool);
    }
    // Clear frame buffer group
    FBM_MEMSET(((void*)&_arFbg[ucFbgId]), 0, sizeof(FBM_FBG_T));

    // Init Settings, refer to FBM_Init
    _arFbg[ucFbgId].ucFbgType = FBM_FBG_TYPE_UNKNOWN;
    _arFbg[ucFbgId].ucFbgId = FBM_FBG_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbgCm = FBM_CM_UNKNOWN;
    _arFbg[ucFbgId].ucStcSrc = STC_SRC_NS;
    _arFbg[ucFbgId].ucAvSyncMode = VID_SYNC_MODE_NS;
    _arFbg[ucFbgId].ucPlayMode = FBM_FBG_DTV_MODE;
    _arFbg[ucFbgId].u4AvSyncStatus = FBM_FBG_AVSYNC_DISABLE;

    _arFbmCbFunc.aau4CbFunc[ucFbgId][FBM_CB_FUNC_FB_READY_EX_IND] = 0;
    _arFbmCbFunc.aau4CbFunc[ucFbgId][FBM_CB_FUNC_FBG_CHG_EX_IND] = 0;

    // restore semaphore
    _arFbg[ucFbgId].hMutex = hMutex;

#ifdef CC_VDP_FULL_ISR
    _arFbg[ucFbgId].prState = &_rState;
#endif

    _arFbg[ucFbgId].hEmptyQSemaphore = hEmptyQSemaphore;
    _arFbg[ucFbgId].hEmptyBQSemaphore = hEmptyBQSemaphore;
    _arFbg[ucFbgId].hEmptyResizeQSemaphore = hEmptyResizeQSemaphore;

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyResizeQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    // Release frame buffer pool
    if (!_arFbg[ucFbgId].fgCreateFromInst)
    {
#if ((defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)  || defined(CC_VOMX_TV_COEXIST)) && defined(CC_FBM_FULL_3D_SUPPORT)) || defined(FBM_4K2K_JPG_SUPPORT) ||defined(FBM_4K2K_SUPPORT) ||defined(FBM_4K1K_VIDEO_SUPPORT)
        if (ucFbgType == FBM_FBG_TYPE_SBSHD || ucFbgType == FBM_FBG_TYPE_TNBHD ||ucFbgType == FBM_FBG_TYPE_4K2K_JPEG  || ucFbgType == FBM_FBG_TYPE_4K2K)
    {
        _FBM_PoolRelease(u4FbMemoryPool);
        _FBM_PoolRelease(u4FbMemoryPool2);        
    }
    else
#endif
    {
        _FBM_PoolRelease(u4FbMemoryPool);
    }
    }
    // Release memory
    if (pMallocAddr != NULL)
    {
        x_mem_free(pMallocAddr);
    }

    FBM_MUTEX_UNLOCK(ucFbgId);
    VERIFY(x_sema_unlock(_hFbgMutex) == OSR_OK);
    
    // TODO, to have vdec es id
#ifdef CC_53XX_SWDMX_V2
    VDEC_FbgReleaseHdlr(_arFbg[ucFbgId].u1DecoderSrcId);
#else
    VDEC_FbgReleaseHdlr();
#endif
}


//-------------------------------------------------------------------------
/** FBM_ResetGroup
 *  Reset FBM Group
 *  @param  ucFbgType   FBG type.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_ResetGroup(UCHAR ucFbgId)
{
    // Note about usage
    // When FBM_FLAG_RESET flag was set by vdp
    // 5381 Â²¤Æ, 20070403, Refer to FBM_ConfigColorMode

    UINT32 u4FbIdx;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (VERIFY_FB_NS(ucFbgId))
    {
        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (_arFbg[ucFbgId].fgCreateFromInst)
    {
        ASSERT(0);
#if 0        
        ASSERT(_arFbg[ucFbgId].rEmptyQ.ucCount != 0);
        for (u4FbIdx = 0; ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
        {
            if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] != FBM_FB_STATUS_EMPTY)
            {
                _arFbg[ucFbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_RECYCLE;
            }
        }

        // clear display queue
        _arFbg[ucFbgId].rDisplayQ.ucReadIdx = 0;
        _arFbg[ucFbgId].rDisplayQ.ucCount = 0;
        
        // Init Settings
        _arFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbFRef = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbBRef = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbFRefValid = 0;
        _arFbg[ucFbgId].ucFbBRefValid = 0;
        
        // default flag
        _arFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_NOT_READY;
#endif        
        FBM_MUTEX_UNLOCK(ucFbgId);
        return;
    }
    // put all frame buffer into empty queue
    _arFbg[ucFbgId].rEmptyQ.ucCount = 0;
    for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase;
            ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
    {
        _FBM_PutFrameBufferToEmptyQ(ucFbgId, (UCHAR)(u4FbIdx));
        _arFbg[ucFbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_EMPTY;
    }

    // put all frame buffer into empty queue
    if (_arFbg[ucFbgId].fgEnableNewSeamless)
    {
        _arFbg[ucFbgId].rResizeEmptyQ.ucCount = 0;
        for (u4FbIdx = 0; ((u4FbIdx < _arFbg[ucFbgId].ucSeamlessFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
        {
            _FBM_PutFrameBufferToEmptyResizeQ(ucFbgId, (UCHAR)(u4FbIdx));
        }
    }

    if ((_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264) ||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_AVS) ||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RAW) ||
            (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_JPEG))
    {
        // create semaphore, Empty Q
        VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)_arFbg[ucFbgId].ucFbNs) == OSR_OK);
    }
    else
    {
        // create semaphore, Empty Q
        VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)_arFbg[ucFbgId].ucFbNs) == OSR_OK);

        // create semaphore, Empty BQ
        VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyBQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)(_arFbg[ucFbgId].ucFbNs - FBM_REF_FB_NS_PER_GROUP)) == OSR_OK);
    }

    VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyResizeQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)_arFbg[ucFbgId].ucSeamlessFbNs) == OSR_OK);

    // clear display queue
    _arFbg[ucFbgId].rDisplayQ.ucReadIdx = 0;
    _arFbg[ucFbgId].rDisplayQ.ucCount = 0;

    // Init Settings
    _arFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbFRef = FBM_FB_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbBRef = FBM_FB_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbFRefValid = 0;
    _arFbg[ucFbgId].ucFbBRefValid = 0;

    // default flag
    _arFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_NOT_READY;

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyResizeQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_InvalidateRefFrameBuffer
 *  Invalidate FBM Group reference frame buffer
 *  @param  ucFbgType   FBG type.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_InvalidateRefFrameBuffer(UCHAR ucFbgId)
{
    // Note about usage
    // When mpv mode change, invalidate refernce
    // Start again at next I picture

    UINT32 u4FbIdx;
    UINT32 u4LockNs;
    UINT32 u4EmptyNs;
    UINT32 u4EmptyRefNs;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (VERIFY_FB_NS(ucFbgId))
    {
        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);
    if (_arFbg[ucFbgId].fgCreateFromInst)
    {
        for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase; ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
        {
            if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_DECODE || 
                _arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_READY ||
                _arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_FIELD_READY)
            {
                FBM_SetFrameBufferStatus(ucFbgId, u4FbIdx, FBM_FB_STATUS_EMPTY); //flush to displayQ
            }
        }

        _arFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbFRef = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbBRef = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbFRefValid = 0;
        _arFbg[ucFbgId].ucFbBRefValid = 0;
    
        FBM_MUTEX_UNLOCK(ucFbgId);
        return;
    }

    // display can lock 1~2 B picture
    u4LockNs = 0;
    for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase;
            ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
    {
        if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
        {
            u4LockNs++;
        }
    }

#if 1
    // reset counting Semaphore
    _arFbg[ucFbgId].hEmptyBQSemaphore.i4Count = 0;
    _arFbg[ucFbgId].hEmptyQSemaphore.i4Count = 0;
#endif

    // clear empty queue
    _arFbg[ucFbgId].rEmptyQ.ucReadIdx = 0;
    _arFbg[ucFbgId].rEmptyQ.ucCount = 0;

    // clear display queue
    _arFbg[ucFbgId].rDisplayQ.ucReadIdx = 0;
    _arFbg[ucFbgId].rDisplayQ.ucCount = 0;
    _arFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
//    _arFbg[ucFbgId].ucFbFRef = FBM_FB_ID_UNKNOWN;
//    _arFbg[ucFbgId].ucFbBRef = FBM_FB_ID_UNKNOWN;
    _arFbg[ucFbgId].ucFbFRefValid = 0;
    _arFbg[ucFbgId].ucFbBRefValid = 0;

    u4EmptyNs = _arFbg[ucFbgId].ucFbNs - _arFbg[ucFbgId].ucFbNsBase - u4LockNs;

    u4EmptyRefNs = 0;
    if ((_arFbg[ucFbgId].ucFbFRef != FBM_FB_ID_UNKNOWN) &&
            (_arFbg[ucFbgId].aucFbStatus[_arFbg[ucFbgId].ucFbFRef] == FBM_FB_STATUS_LOCK))
    {
    }
    else
    {
        u4EmptyRefNs++;
    }
    if ((_arFbg[ucFbgId].ucFbBRef != FBM_FB_ID_UNKNOWN) &&
            (_arFbg[ucFbgId].aucFbStatus[_arFbg[ucFbgId].ucFbBRef] == FBM_FB_STATUS_LOCK))
    {
    }
    else
    {
        u4EmptyRefNs++;
    }

    // set counting Semaphore
    // Bug fixed 20050708, reference may locked by display
    // _arFbg[ucFbgId].hEmptyBQSemaphore.i4Count = (INT32)(u4EmptyNs - FBM_REF_FB_NS_PER_GROUP) is NOT correct!

    // create semaphore, Empty Q
    VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)u4EmptyNs) == OSR_OK);

    // create semaphore, Empty BQ
    VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyBQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)(u4EmptyNs - u4EmptyRefNs)) == OSR_OK);

    for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase;
            ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
    {
        if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] != FBM_FB_STATUS_LOCK)
        {
            if (_arFbg[ucFbgId].fgEnableNewSeamless && _arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_DISPLAYQ)
            {
                if (_arFbg[ucFbgId].prPicHdr[u4FbIdx].ucSeamlessResizeFbid != FBM_FB_ID_UNKNOWN &&
                    _arFbg[ucFbgId].aucResizeFbStatus[_arFbg[ucFbgId].prPicHdr[u4FbIdx].ucSeamlessResizeFbid] != FBM_FB_STATUS_EMPTY)
                {
                    _FBM_PutFrameBufferToEmptyResizeQ(ucFbgId, _arFbg[ucFbgId].prPicHdr[u4FbIdx].ucSeamlessResizeFbid);
                    VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyResizeQSemaphore) == OSR_OK);
                }
            }
            
            _arFbg[ucFbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_EMPTY;
            _FBM_PutFrameBufferToEmptyQ(ucFbgId, (UCHAR)(u4FbIdx));
        }
    }

    LOG(7, "Invalidate G(%d) S(%d %d %d %d) R(%02x %02x) Q(%d %d %d %d)\n",
        ucFbgId,
        _arFbg[ucFbgId].aucFbStatus[0],
        _arFbg[ucFbgId].aucFbStatus[1],
        _arFbg[ucFbgId].aucFbStatus[2],
        _arFbg[ucFbgId].aucFbStatus[3],
        _arFbg[ucFbgId].ucFbFRef,
        _arFbg[ucFbgId].ucFbBRef,
        _arFbg[ucFbgId].rDisplayQ.ucCount,
        _arFbg[ucFbgId].rEmptyQ.ucCount,
        _arFbg[ucFbgId].hEmptyQSemaphore.i4Count,
        _arFbg[ucFbgId].hEmptyBQSemaphore.i4Count);

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.i4Count >= 0);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.i4Count >= 0);

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_FlushLockFrameBuffer
 *  Flush FBM Group lock frame buffer
 *  @param  ucFbgType   FBG type.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_FlushLockFrameBuffer(UCHAR ucFbgId)
{
    // Note about usage
    // When vdp mode change, flush lock frame buffer

    UINT32 u4FbIdx;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (VERIFY_FB_NS(ucFbgId))
    {
        return;
    }

    if (_arFbg[ucFbgId].fgCreateFromInst)
    {
        return;
    }
    FBM_MUTEX_LOCK(ucFbgId);

    // put all lock frame buffer into empty queue
    for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase;
            ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
    {
        if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
        {

            if ((_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264) ||
                    (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_AVS) ||
                    (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RAW) ||
                    (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_JPEG))

            {
                VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);
            }
            else
            {
                // release semaphore (Empty)
                VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);

                if (IS_REFERENCE_FB(ucFbgId, u4FbIdx))
                {   // release reference frame buffer
                }
                else
                {   // release B frame buffer

                    // release semaphore (B & Empty)
                    VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);
                }
            }

            _FBM_PutFrameBufferToEmptyQ(ucFbgId, (UCHAR)(u4FbIdx));

            if (_arFbg[ucFbgId].fgEnableNewSeamless)
            {
                if (_arFbg[ucFbgId].prPicHdr[u4FbIdx].ucSeamlessResizeFbid != FBM_FB_ID_UNKNOWN &&
                    _arFbg[ucFbgId].aucResizeFbStatus[_arFbg[ucFbgId].prPicHdr[u4FbIdx].ucSeamlessResizeFbid] != FBM_FB_STATUS_EMPTY)
                {
                    _FBM_PutFrameBufferToEmptyResizeQ(ucFbgId, _arFbg[ucFbgId].prPicHdr[u4FbIdx].ucSeamlessResizeFbid);
                    VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyResizeQSemaphore) == OSR_OK);
                }
            }
            _arFbg[ucFbgId].aucFbStatus[u4FbIdx] = FBM_FB_STATUS_EMPTY;
        }
    }

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_WaitUnlockFrameBuffer
 *  Wait FBM Group unlock frame buffer
 *  @param  ucFbgType   FBG type.
 *  @param  u4Timeout   time to wait.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_WaitUnlockFrameBuffer(UCHAR ucFbgId, UINT32 u4Timeout)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (VERIFY_FB_NS(ucFbgId))
    {
        return;
    }

    if (_arFbg[ucFbgId].fgCreateFromInst)
    {
        return;
    }
    FBM_MUTEX_LOCK(ucFbgId);

    {
        UINT32 u4FbIdx;
        UINT32 u4LockFbNs;
        UINT32 u4EmptyFbNs;

        // unlock frame buffer Ns
        u4LockFbNs = 0;
        u4EmptyFbNs = 0;
        for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase;
                ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
        {
            if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
            {
                u4LockFbNs++;
            }
            else if (_arFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_EMPTY)
            {
                u4EmptyFbNs++;
            }
        }

        // wait for lock frame buffer
        if (u4LockFbNs != 0)
        {
            u4FbIdx = 0;
            while (u4FbIdx < (u4LockFbNs + u4EmptyFbNs))
            {
                if (_FBM_csema_lock(&_arFbg[ucFbgId].hEmptyQSemaphore, X_SEMA_OPTION_WAIT, u4Timeout) == OSR_OK)
                {
                    u4FbIdx++;
                }
                else
                {
                    u4FbIdx = _arFbg[ucFbgId].ucFbNs - _arFbg[ucFbgId].ucFbNsBase;
                    LOG(1, "Wait unlock timeout! (%d, %d)\n",u4LockFbNs, u4EmptyFbNs);
                }
            }
        }
    }

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_ReleaseDispQ
 *  Release fb in DispQ to EmptyQ.
 *  @param  ucFbgId   FBG index.
 *  @return release fb number.
 */
//-------------------------------------------------------------------------
void FBM_ReleaseDispQ(UCHAR ucFbgId)
{
    UCHAR ucCnt;
    UCHAR ucFbId = FBM_FB_ID_UNKNOWN;
    UCHAR ucIdx;
	UCHAR ucVdpId = VDP_NS;
	UCHAR ucEsId = ES0;
    UCHAR ucThumbnail = 0;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (_arFbg[ucFbgId].fgCreateFromInst)
    {
        return;
    }

	FBM_MUTEX_LOCK(ucFbgId);
    ucThumbnail = FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_THUMBNAIL_MODE);

    if (!ucThumbnail)
    {
        FBM_MUTEX_UNLOCK(ucFbgId);
        ucEsId = _arFbg[ucFbgId].u1DecoderSrcId;
        ucVdpId = VDP_Es2Vdp(ucEsId);
        #if defined(CC_MT5890)
        VDP_VsyncMutexLock(ucVdpId);
        #else
        VDP_VsyncMutexLock();
        #endif
        ucFbId = VDP_GetPendingFB(ucVdpId, ucFbgId);
        VDP_SetReleaseDispQ(ucVdpId, ucFbgId);
        FBM_MUTEX_LOCK(ucFbgId);
    }

    if (_arFbg[ucFbgId].fgEnableNewSeamless)
    {
        _VDP_Prescale_Flush(_arFbg[ucFbgId].ucSeamlessRzId, FALSE);
        ucCnt = _arFbg[ucFbgId].rDisplayQ.ucCount;
    }
    else
    {
        ucCnt = FBM_CheckFrameBufferDispQ(ucFbgId);
    }

    for (ucIdx = 0; ucIdx < ucCnt; ucIdx++)
    {
        UCHAR ucDispFbId;

        if (_arFbg[ucFbgId].fgEnableNewSeamless)
        {
            if (_arFbg[ucFbgId].rDisplayQ.ucCount > 0)
            {
                ucDispFbId = _arFbg[ucFbgId].rDisplayQ.aucQueue[_arFbg[ucFbgId].rDisplayQ.ucReadIdx];
                
                _arFbg[ucFbgId].rDisplayQ.ucReadIdx = (UCHAR)(_arFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
                _arFbg[ucFbgId].rDisplayQ.ucCount--;
                
                ASSERT(_arFbg[ucFbgId].aucFbStatus[ucDispFbId] == FBM_FB_STATUS_DISPLAYQ);
                
                if ((_arFbg[ucFbgId].ucSeqChg) && (_arFbg[ucFbgId].ucNewFbId == ucDispFbId))
                {
                    _arFbg[ucFbgId].u4FbgFlag |= _arFbg[ucFbgId].u4SeqChgFlag;
                    _arFbg[ucFbgId].ucSeqChg = 0;
                }
            }
            else
            {
                ucDispFbId = FBM_FB_ID_UNKNOWN;
            }
        }
        else
        {
            ucDispFbId = FBM_GetFrameBufferFromDispQ(ucFbgId);
        }
        LOG(3, "Flush display fbid (%d ,%d)\n", ucDispFbId, _arFbg[ucFbgId].aucFbStatus[ucDispFbId]);
        FBM_SetFrameBufferStatus(ucFbgId, ucDispFbId, FBM_FB_STATUS_EMPTY);
    }

    if ((ucFbId != FBM_FB_ID_UNKNOWN) && (ucFbId < FBM_MAX_FB_NS_PER_GROUP))
    {
        if (_arFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_LOCK)
        {
            LOG(3, "Release Display Q with 1 more lock frame buffer!\n");

            if ((_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264) ||
                    (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_AVS) ||
                    (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RAW) ||
                    (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_JPEG))

            {
                VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);
            }
            else
            {
                // release semaphore (Empty)
                VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);

                if (IS_REFERENCE_FB(ucFbgId, ucFbId))
                {   // release reference frame buffer
                }
                else
                {   // release B frame buffer

                    // release semaphore (B & Empty)
                    VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);
                }
            }

            _FBM_PutFrameBufferToEmptyQ(ucFbgId, ucFbId);
            if (_arFbg[ucFbgId].afgRefList[ucFbId] ==1)
            {
                _arFbg[ucFbgId].afgRefList[ucFbId]=0;
            }

            _arFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_EMPTY;
#if 1
            if (_arFbg[ucFbgId].fgEnableNewSeamless)
            {
                if (_arFbg[ucFbgId].prPicHdr[ucFbId].ucSeamlessResizeFbid != FBM_FB_ID_UNKNOWN &&
                    _arFbg[ucFbgId].aucResizeFbStatus[_arFbg[ucFbgId].prPicHdr[ucFbId].ucSeamlessResizeFbid] != FBM_FB_STATUS_EMPTY)
                {
                    LOG(3, "Release resize fbid %d from pending fb %d!\n", _arFbg[ucFbgId].prPicHdr[ucFbId].ucSeamlessResizeFbid, ucFbId);
                    _FBM_PutFrameBufferToEmptyResizeQ(ucFbgId, _arFbg[ucFbgId].prPicHdr[ucFbId].ucSeamlessResizeFbid);
                    VERIFY(_FBM_csema_unlock(&_arFbg[ucFbgId].hEmptyResizeQSemaphore) == OSR_OK);
                }
            }
#endif
            if (!ucThumbnail)
            {
                VDP_CleanPendingFB(ucVdpId, ucFbgId);
            }
        }
    }
    FBM_MUTEX_UNLOCK(ucFbgId);
    if (!ucThumbnail)
    {
        #if defined(CC_MT5890)
        VDP_VsyncMutexUnlock(ucVdpId);
        #else
        VDP_VsyncMutexUnlock();
        #endif
    }
}


//-------------------------------------------------------------------------
/** FBM_ConfigColorMode
 *  Check FBG color mode
 *  @param  ucFbgType   FBG type.
 *  @param  ucFbgCm   color mode.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_ConfigColorMode(UCHAR ucFbgId, UCHAR ucFbgCm, UINT32 u4HSize,
                         UINT32 u4VSize, FBM_CREATE_FBG_PAR_T *prPar)
{
    // Note about color
    // Support 420/422/444/420Digest

    UCHAR ucFbpListNs;

    UINT32 u4YSize;
    UINT32 u4CSize;
    UINT32 u4MvSize;
    UINT32 u4FbIdx;
    UINT32 au4CbFunc[FBM_CB_FUNC_NS] = {0};

    UINT32 u4FbNs;
    UINT32 u4ListIdx;
    FBM_POOL_T* prFbmPool = NULL;
    
    UINT32 u4PoolSize = 0;

    FBP_LIST_T* prFbpList;
    
    UNUSED(prFbmPool);
    UNUSED(u4PoolSize);
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (!prPar)
    {
        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    _arFbg[ucFbgId].rEmptyQ.ucCount = 0;
    _arFbg[ucFbgId].rEmptyQ.ucReadIdx = 0;

    _FBM_GetPoolList(&prFbpList, &ucFbpListNs);

    _arFbg[ucFbgId].u1DecoderSrcId = prPar->u1VdecId;
    u4YSize = 0;
    u4CSize = 0;
    for (u4ListIdx = 0; u4ListIdx < ucFbpListNs; u4ListIdx++)
    {
        if (_arFbg[ucFbgId].ucFbgType == prFbpList[u4ListIdx].ucType)
        {
            u4YSize = prFbpList[u4ListIdx].u4YSize;
            u4CSize = prFbpList[u4ListIdx].u4CSize;

            _arFbg[ucFbgId].u4FbWidth = prFbpList[u4ListIdx].u4Width;
            _arFbg[ucFbgId].u4FbHeight = prFbpList[u4ListIdx].u4Height;

            u4ListIdx = ucFbpListNs;
        }
    }
    if (_arFbg[ucFbgId].eDynamicMode == FBM_DYNAMIC_SAME)
    {    //save the memset func ptr
        au4CbFunc[FBM_CB_FUNC_GFX_MEMSET] = _arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_MEMSET];
        au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET] = _arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET];
        au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET_DONE] = _arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET_DONE];
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_MEMSET, (UINT32)NULL);
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_TASK_MEMSET, (UINT32)NULL);
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_TASK_MEMSET_DONE, (UINT32)NULL);
    }

    if (prPar && prPar->fgExternalAddr)
    {
        _arFbg[ucFbgId].u1FbgAppMode= prPar->u1AppMode;
        _arFbg[ucFbgId].ucFbgCm = ucFbgCm;
        //_arFbg[ucFbgId].ucFbNsBase = 0;
        
        _arFbg[ucFbgId].ucFbNs = prPar->u1RequestFbNs;// + _arFbg[ucFbgId].ucFbNsBase;
    }
    else if (ucFbgCm == FBM_CM_420 || ucFbgCm == FBM_CM_422 || ucFbgCm == FBM_CM_444)
    {
        UINT32 u4Addr = 0;
        UINT32 u4WidthDiv = 1;
        UINT32 u4ExtraFbNs = 0;
        
        _arFbg[ucFbgId].fgRPRMode = FALSE;
        _arFbg[ucFbgId].u1FbgAppMode= prPar->u1AppMode;
        _arFbg[ucFbgId].ucFbgCm = ucFbgCm;
        
#ifdef ENABLE_MULTIMEDIA
    //for not-standard timing such as 1440*900 to support much more reference.
    {
        VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(_arFbg[ucFbgId].u1DecoderSrcId);
        if(prVdecEsInfo && (prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_NETWORK_NETFLIX)) 
        {
            if ((_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_1080HD) || (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_720HD))
            {
                _arFbg[ucFbgId].u4FbWidth = FBM_ALIGN_MASK(u4HSize, FBM_B2R_H_PITCH);
                _arFbg[ucFbgId].u4FbHeight = FBM_ALIGN_MASK(u4VSize, FBM_MPV_V_ALIGMENT);;
                u4YSize = FBM_ALIGN_MASK((_arFbg[ucFbgId].u4FbWidth * _arFbg[ucFbgId].u4FbHeight), FBM_FMG_Y_ALIGMENT);
                u4CSize = FBM_ALIGN_MASK((u4YSize >> 1), FBM_FMG_Y_ALIGMENT);
                if (prPar->fg10Bit)
                {
                    u4YSize = (u4YSize * 5) >> 2;
                    u4CSize = (u4CSize * 5) >> 2;
                            
                    LOG(1,"Compact Mode 10 bit [u4YSize 0x%x u4CSize 0x%x Pool Size 0x%x]\n",u4YSize,u4CSize,_arFbg[ucFbgId].u4FbMemoryPoolSize);
                }    
            }
        }
    }
#endif
        if (ucFbgCm == FBM_CM_422)
        {
            u4CSize *= 2;    // 420 to 422
        }
        else if(ucFbgCm == FBM_CM_444)
        {
            u4CSize *= 4;    // 420 to 444
        }

        // Calculate Frame Buffer Number
        u4FbNs = FBM_CalcBufNum(ucFbgId, _arFbg[ucFbgId].ucFbgType, _arFbg[ucFbgId].u4VDecFmt, 
                _arFbg[ucFbgId].u4FbWidth, _arFbg[ucFbgId].u4FbHeight, 
                _arFbg[ucFbgId].u4FbMemoryPoolSize, u4YSize, u4CSize, 
                _arFbg[ucFbgId].u1FbgAppMode, &u4ExtraFbNs);
        
        u4FbNs -= u4ExtraFbNs; // Remove Extra Fb first, as the extra partion maybe not continuous.

        if(_fgSetDecoderFb && (_u1DecoderType == _arFbg[ucFbgId].u4VDecFmt))
        {
            u4FbNs = _u1NumOfFb;
            _fgSetDecoderFb = FALSE;
        }

        if (u4FbNs >= FBM_MAX_FB_NS_PER_GROUP/2)
        {
            u4FbNs = FBM_MAX_FB_NS_PER_GROUP/2 - 1;
        }
        
        _arFbg[ucFbgId].ucFbNs = (UCHAR)(u4FbNs) + _arFbg[ucFbgId].ucFbNsBase;

        
        u4Addr = _arFbg[ucFbgId].u4FbMemoryPool;
        u4PoolSize = _arFbg[ucFbgId].u4FbMemoryPoolSize;

        if(_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_JPEG && ucFbgCm == FBM_CM_422 && prPar->u1AppMode != FBM_FBG_APP_MTIMAGE)
        {
            u4Addr = _arFbg[ucFbgId].u4FbMemoryPool;
#ifdef CC_FBM_3D_OSD
#if !defined(FBM_4K2K_JPG_SUPPORT) && !defined(FBM_4K2K_SUPPORT)
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
            if (prFbmPool != NULL)
            {
                if (_arFbg[ucFbgId].u4FbMemoryPool < prFbmPool->u4Addr + prFbmPool->u4Size && 
                    prFbmPool->u4Addr + prFbmPool->u4Size < _arFbg[ucFbgId].u4FbMemoryPool + _arFbg[ucFbgId].u4FbMemoryPoolSize)
                {
                    LOG(0, "Free size after overlapped with JPG_VDP %d\n", _arFbg[ucFbgId].u4FbMemoryPool + _arFbg[ucFbgId].u4FbMemoryPoolSize - (prFbmPool->u4Addr + prFbmPool->u4Size));
                    //ASSERT(_arFbg[ucFbgId].u4FbMemoryPool + _arFbg[ucFbgId].u4FbMemoryPoolSize - (prFbmPool->u4Addr + prFbmPool->u4Size) >= 0x880000);
                    u4Addr += (prFbmPool->u4Addr + prFbmPool->u4Size - _arFbg[ucFbgId].u4FbMemoryPool);

                    //1024 alignment
                    u4Addr = ((u4Addr+1023) & ~1023);
                }
            }
#endif
#endif
#if defined(FBM_4K2K_JPG_SUPPORT) || defined(FBM_4K2K_SUPPORT)
            {
                u4PoolSize = FBM_FBG_TYPE_4K2K_7FB_SIZE_PART1;
            }
#endif
        }
        else
        {
#if !defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) && !defined(CC_VOMX_TV_COEXIST) && defined(CC_FBM_TWO_FBP) //using 23.4 mb + 6mb extra buffer
            if(prPar->u1AppMode == FBM_FBG_APP_MHP)
            {
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
                if(prFbmPool != NULL)
                {
                    u4Addr = FBM_ALIGN_MASK(prFbmPool->u4Addr+ FBM_FBG_TYPE_1080HD_H264_SIZE, FBM_FMG_Y_ALIGMENT);
                }
            }
            else if(prPar->u1AppMode == FBM_FBG_APP_VP6_ALPHA)
            {
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
                if(prFbmPool != NULL)
                {
                    UINT32 u4FrameSize = u4YSize + u4CSize;
                    u4Addr = ((prFbmPool->u4Addr + prFbmPool->u4Size) - u4FbNs*u4FrameSize);
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_FMG_Y_ALIGMENT);
                }
            }
#elif defined(CC_FBM_INT_TWO_FBP) && defined(CC_FBM_TWO_FBP)
            // for android omx, vconv will use mpeg bottom part, and vdec use upper part...
            // TODO: fix it for new omx
            if(prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
            {
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
                if(prFbmPool != NULL)
                {
                    u4Addr = FBM_ALIGN_MASK(prFbmPool->u4Addr, FBM_FMG_Y_ALIGMENT);
                }
            }
#endif

            // Pool Size
            if(_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_SBSHD  || _arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_TNBHD)
            {
                u4PoolSize = FBM_FBG_TYPE_1080HD_H264_SIZE;
            }
#ifdef FBM_4K2K_SUPPORT
            else if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_4K2K)
            {
                u4PoolSize = FBM_FBG_TYPE_4K2K_7FB_SIZE_PART1 ;
            }
#endif
#ifdef FBM_4K2K_JPG_SUPPORT
            else if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_4K2K_JPEG)
            {
                u4PoolSize = FBM_FBG_TYPE_4K2K_JPEG_SIZE ;
            }
#endif
        }

#if (defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)  || defined(CC_VOMX_TV_COEXIST)) && defined(CC_FBM_FULL_3D_SUPPORT) || defined (FBM_4K1K_VIDEO_SUPPORT)
        if(_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_SBSHD  || _arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_TNBHD)
        {
            for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase; ((u4FbIdx < _arFbg[ucFbgId].ucFbNs -1)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
            {
                if(u4PoolSize < (u4YSize+u4CSize))
                {
                    u4Addr = _arFbg[ucFbgId].u4FbMemoryPool2;
                    u4PoolSize = FBM_FBG_TYPE_1080HD_H264_SIZE;
                }

                if (prPar->u1AppMode == FBM_FBG_APP_OMX || prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_Y_ALIGMENT);
                }
                
                // Y
                _arFbg[ucFbgId].au4AddrY[u4FbIdx] = u4Addr;
                _arFbg[ucFbgId].au4GfxMemsetId[u4FbIdx] = FBM_GFX_MEMSET_ASYNC(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), _arFbg[ucFbgId].u4FbHeight, 0x0);

                u4Addr += u4YSize;

                if (prPar->u1AppMode == FBM_FBG_APP_OMX || prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_C_ALIGMENT);
                }
                
                _arFbg[ucFbgId].au4AddrC[u4FbIdx] = u4Addr;
                _arFbg[ucFbgId].au4GfxMemsetId[u4FbIdx] = FBM_GFX_MEMSET_ASYNC(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), (_arFbg[ucFbgId].u4FbHeight >> 1), 0x80);

                u4Addr += u4CSize;

                if (u4PoolSize >= u4YSize + u4CSize)
                {
                    u4PoolSize -= (u4YSize + u4CSize);
                }
                else
                {
                    u4PoolSize = 0;
                }                
            }

            // use extra buffer from CC_FBM_FULL_3D_SUPPORT
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
            if(prFbmPool != NULL)
            {
                _arFbg[ucFbgId].au4AddrY[_arFbg[ucFbgId].ucFbNs -1] = FBM_ALIGN_MASK(prFbmPool->u4Addr + prFbmPool->u4Size - FBM_EXTRA_FOR_SEAMLESS - FMB_EXTRA_FOR_SINGLE_H264_LV5, FBM_FMG_Y_ALIGMENT);

                if (prPar->u1AppMode == FBM_FBG_APP_OMX || prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    _arFbg[ucFbgId].au4AddrY[_arFbg[ucFbgId].ucFbNs -1] = FBM_ALIGN_MASK(_arFbg[ucFbgId].au4AddrY[_arFbg[ucFbgId].ucFbNs -1], FBM_GFX_Y_ALIGMENT);
                }                
                _arFbg[ucFbgId].au4AddrC[_arFbg[ucFbgId].ucFbNs -1] = u4YSize + _arFbg[ucFbgId].au4AddrY[_arFbg[ucFbgId].ucFbNs -1];

                if (prPar->u1AppMode == FBM_FBG_APP_OMX || prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    _arFbg[ucFbgId].au4AddrC[_arFbg[ucFbgId].ucFbNs -1] = FBM_ALIGN_MASK(_arFbg[ucFbgId].au4AddrC[_arFbg[ucFbgId].ucFbNs -1], FBM_GFX_C_ALIGMENT);
                }  
            }
        }
        else
#endif
#ifdef FBM_4K2K_JPG_SUPPORT  //FBM_4K2K_SUPPORT
            if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_4K2K_JPEG)
            {
                for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase; ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
                {
                    if (u4PoolSize < (u4YSize + u4CSize))
                    {
                        u4Addr = _arFbg[ucFbgId].u4FbMemoryPool2;
                        u4PoolSize = FBM_FBG_TYPE_4K2K_JPEG_SIZE;
                    }
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_Y_ALIGMENT);
                    // Y
                    _arFbg[ucFbgId].au4AddrY[u4FbIdx] = u4Addr;
                    _arFbg[ucFbgId].au4GfxMemsetId[u4FbIdx] = FBM_GFX_MEMSET_ASYNC(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), _arFbg[ucFbgId].u4FbHeight, 0x0);

                    u4Addr += u4YSize;

                    // C
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_C_ALIGMENT);
                    _arFbg[ucFbgId].au4AddrC[u4FbIdx] = u4Addr;
                    _arFbg[ucFbgId].au4GfxMemsetId[u4FbIdx] = FBM_GFX_MEMSET_ASYNC(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), (_arFbg[ucFbgId].u4FbHeight >> 1), 0x80);

                    u4Addr += u4CSize;
                    LOG(3, "FBG_TYPE_4K2K(%d): AddrYC(0x%x::0x%x) SizeYC(0x%x::0x%x) Pool(%x::%x) Size(0x%x).\n",
                        u4FbIdx, _arFbg[ucFbgId].au4AddrY[u4FbIdx], _arFbg[ucFbgId].au4AddrC[u4FbIdx],
                        u4YSize, u4CSize, _arFbg[ucFbgId].u4FbMemoryPool, _arFbg[ucFbgId].u4FbMemoryPool2, u4PoolSize);

                    if (u4PoolSize >= u4YSize + u4CSize)
                    {
                        u4PoolSize -= (u4YSize + u4CSize);
                    }
                    else
                    {
                        u4PoolSize = 0;
                    }
                }
            }
            else
#endif
#ifdef FBM_4K2K_SUPPORT
            if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_4K2K)
            {
                LOG(0, "4K2K FB:Addr(0x%x) Size[Pool(0x%x) Y(0x%x) C(0x%x) FbNs(%d).\n", u4Addr, u4PoolSize, u4YSize, u4CSize, _arFbg[ucFbgId].ucFbNs);
                for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase; ((u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
                {
                    if (u4PoolSize < (u4YSize + u4CSize))
                    {
                        u4Addr = _arFbg[ucFbgId].u4FbMemoryPool2;
                        u4PoolSize = FBM_FBG_TYPE_4K2K_7FB_SIZE_PART2;
                    }
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_Y_ALIGMENT);

                    // Y
                    _arFbg[ucFbgId].au4AddrY[u4FbIdx] = u4Addr;
                    _arFbg[ucFbgId].au4GfxMemsetId[u4FbIdx] = FBM_GFX_MEMSET_ASYNC(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), _arFbg[ucFbgId].u4FbHeight, 0x0);

                    u4Addr += u4YSize;

                    // C
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_C_ALIGMENT);

                    _arFbg[ucFbgId].au4AddrC[u4FbIdx] = u4Addr;
                    _arFbg[ucFbgId].au4GfxMemsetId[u4FbIdx] = FBM_GFX_MEMSET_ASYNC(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), (_arFbg[ucFbgId].u4FbHeight >> 1), 0x80);

                    u4Addr += u4CSize;

                    if (u4PoolSize >= u4YSize + u4CSize)
                    {
                        u4PoolSize -= (u4YSize + u4CSize);
                    }
                    else
                    {
                        u4PoolSize = 0;
                    }
                }
            }
            else
#endif
        {
            for (u4FbIdx= _arFbg[ucFbgId].ucFbNsBase;(u4FbIdx < _arFbg[ucFbgId].ucFbNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP); u4FbIdx++)
            {
                if (prPar->u1AppMode == FBM_FBG_APP_OMX || prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_Y_ALIGMENT);
                }
                // Y
                _arFbg[ucFbgId].au4AddrY[u4FbIdx] = u4Addr;
                FBM_GFX_MEMSET(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), _arFbg[ucFbgId].u4FbHeight, 0x0);

                u4Addr += u4YSize;

                if (prPar->u1AppMode == FBM_FBG_APP_OMX || prPar->u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    u4Addr = FBM_ALIGN_MASK(u4Addr, FBM_GFX_C_ALIGMENT);
                }
                
                _arFbg[ucFbgId].au4AddrC[u4FbIdx] = u4Addr;
                FBM_GFX_MEMSET(u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), (_arFbg[ucFbgId].u4FbHeight >> 1), 0x80);

                u4Addr += u4CSize;
            }
        }

        //MV buffer config
        if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_MPEG2)
        {
            _arFbg[ucFbgId].au4AddrMv[0] = u4Addr;

            if ((u4FbIdx >= FBM_DBK_FB_NS) && (_arFbg[ucFbgId].ucFbgType != FBM_FBG_TYPE_1080HD_RR))
            {
                UINT32 u4ExtraYSize;

                if (u4FbIdx >= FBM_MAX_FB_NS_PER_GROUP)
                {
                    ASSERT(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP);
                    FBM_MUTEX_UNLOCK(ucFbgId);
                    return;
                }
                _arFbg[ucFbgId].au4AddrY[u4FbIdx] = u4Addr;

                switch (_arFbg[ucFbgId].ucFbgType)
                {

                case FBM_FBG_TYPE_SBSHD:
                    u4ExtraYSize = FBM_SBSHD_Y_EXTRA_SIZE;
                    break;

                case FBM_FBG_TYPE_TNBHD:
                    u4ExtraYSize = FBM_TBNHD_Y_EXTRA_SIZE;
                    break;

                case FBM_FBG_TYPE_1080HD:
                    u4ExtraYSize = FBM_1080HD_Y_EXTRA_SIZE;
                    break;

                case FBM_FBG_TYPE_720HD:
                    u4ExtraYSize = FBM_720HD_Y_EXTRA_SIZE;
                    break;

                case FBM_FBG_TYPE_PAL:
                    u4ExtraYSize = FBM_PAL_Y_EXTRA_SIZE;
                    break;

                case FBM_FBG_TYPE_NTSC:
                    u4ExtraYSize = FBM_NTSC_Y_EXTRA_SIZE;
                    break;

                default:
                    u4ExtraYSize = 0;
                    ASSERT(0);
                    break;
                }

                _arFbg[ucFbgId].au4AddrC[u4FbIdx] = FBM_ALIGN_MASK((u4Addr + u4ExtraYSize), FBM_MPV_C_ALIGMENT);
            }
        }
        else if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264)
        {
            if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_1080HD_RR)
            {
                u4MvSize = u4YSize >> 1;
            }
            else
            {
                u4MvSize = u4YSize >> 2;
            }

            if (_arFbg[ucFbgId].ucMvBufNs > FBM_MAX_FB_NS_PER_GROUP-1)
            {
                ASSERT(_arFbg[ucFbgId].ucMvBufNs <= FBM_MAX_FB_NS_PER_GROUP-1);
                FBM_MUTEX_UNLOCK(ucFbgId);
                return;
            }
            if((prPar->ucCftFbgId != FBM_FBG_ID_UNKNOWN) && (prPar->ucCftFbgId < FBG_MAX_NS))
            {
                if(_arFbg[ucFbgId].au4AddrMv[0] > u4Addr)
                {
                    u4Addr = _arFbg[ucFbgId].au4AddrMv[0];
                }
            }
            for (u4FbIdx = 0;
                    ((u4FbIdx < _arFbg[ucFbgId].ucMvBufNs)&&(u4FbIdx < (FBM_MAX_FB_NS_PER_GROUP-1))); u4FbIdx++)
            {
                _arFbg[ucFbgId].au4AddrMv[u4FbIdx] = u4Addr;
                u4Addr += u4MvSize;
            }
        }
        else if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RV)
        {
            if (_arFbg[ucFbgId].fgRPRMode)
            {
                _arFbg[ucFbgId].u4ExtraYBuffer = u4Addr;
                //u4Addr += u4YSize;
                u4Addr += FBM_FBG_TYPE_PAL_Y_SIZE;
                _arFbg[ucFbgId].u4ExtraCBuffer = u4Addr;
                //u4Addr += u4CSize;
                u4Addr += FBM_FBG_TYPE_PAL_C_SIZE;

                _arFbg[ucFbgId].u4RPRSeamlessBuffer = u4Addr;
                //for another two frame
                u4Addr += ((FBM_FBG_TYPE_PAL_Y_SIZE+FBM_FBG_TYPE_PAL_C_SIZE)<<1);
                //u4Addr += (u4YSize<<1);
                //u4Addr += (u4CSize<<1);
            }
            _arFbg[ucFbgId].au4AddrMv[0] = u4Addr;
        }
        else if (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_MPEG4)
        {
#if (defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)  || defined(CC_VOMX_TV_COEXIST)) && defined(CC_FBM_FULL_3D_SUPPORT) || defined(FBM_4K1K_VIDEO_SUPPORT)
            if(_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_SBSHD  || _arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_TNBHD)
            {
                if (u4PoolSize < FBM_WORK_BUF_SIZE)
                {
                    u4Addr = _arFbg[ucFbgId].u4FbMemoryPool2;
                    u4PoolSize = FBM_FBG_TYPE_1080HD_H264_SIZE;
                }
            }
            _arFbg[ucFbgId].au4AddrMv[0] = u4Addr;
#else
            {
                _arFbg[ucFbgId].au4AddrMv[0] = u4Addr;
            }
#endif
        }
        else    //VC-1 and RM
        {
            ASSERT(_arFbg[ucFbgId].u4VDecFmt != FBM_VDEC_UNKNOWN);
            _arFbg[ucFbgId].au4AddrMv[0] = u4Addr;
        }
        if (!_arFbg[ucFbgId].fgRPRMode)
        {
            _arFbg[ucFbgId].u4ExtraYBuffer =0;
            _arFbg[ucFbgId].u4ExtraCBuffer =0;
        }

#ifdef CC_FBM_SPLIT_MPEG_FREE_PART
        _arFbg[ucFbgId].u4WorkBufSize = FBM_WORK_BUF_SIZE;
#else

#if (defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)  || defined(CC_VOMX_TV_COEXIST)) && defined(CC_FBM_FULL_3D_SUPPORT) ||defined(FBM_4K1K_VIDEO_SUPPORT)
        if(_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_SBSHD  || _arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_TNBHD)
        {
            _arFbg[ucFbgId].u4WorkBufSize = u4PoolSize;
        }
#ifdef FBM_4K2K_SUPPORT
        else if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_4K2K)
        {
            _arFbg[ucFbgId].u4WorkBufSize = FBM_FBG_TYPE_4K2K_7FB_SIZE_PART2 - (3 * ( u4YSize + u4CSize));
        }
#endif
        else
        {
            _arFbg[ucFbgId].u4WorkBufSize = _arFbg[ucFbgId].u4FbMemoryPoolSize - ((_arFbg[ucFbgId].ucFbNs - _arFbg[ucFbgId].ucFbNsBase) * ( u4YSize + u4CSize));
        }
#else
        _arFbg[ucFbgId].u4WorkBufSize = _arFbg[ucFbgId].u4FbMemoryPoolSize - ((_arFbg[ucFbgId].ucFbNs - _arFbg[ucFbgId].ucFbNsBase) * ( u4YSize + u4CSize));
#endif
        LOG(1, "u4WorkBufSize(%d)=PoolSize(%d)-(Y(%d)+C(%d))*Ns(%d - %d).\n", _arFbg[ucFbgId].u4WorkBufSize, 
                _arFbg[ucFbgId].u4FbMemoryPoolSize, u4YSize, u4CSize, _arFbg[ucFbgId].ucFbNs, _arFbg[ucFbgId].ucFbNsBase);
#endif

    //Handle extra reference frame, update FbNs & Fb partition here.

#if defined(CC_H264_LV5_SUPPORT_SHARE_POOL) || defined(CC_H264_LV5_SUPPORT_SINGLE_POOL)

        if (u4ExtraFbNs && _arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264 && _arFbg[ucFbgId].fgEnableH264V5Support)
        {
            UINT32 u4ExtraReferenceNs;
            UINT32 u4MVSize;

#if defined(CC_H264_LV5_SUPPORT_SHARE_POOL)

            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
            u4Addr = FBM_ALIGN_MASK(prFbmPool->u4Addr , FBM_FMG_Y_ALIGMENT);
            u4PoolSize = prFbmPool->u4Size;

            ASSERT(FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL > FBM_FBG_H264_FHD_REFERNCE_NS);
            u4ExtraReferenceNs = FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL - FBM_FBG_H264_FHD_REFERNCE_NS;
            u4MVSize = ((u4ExtraReferenceNs * u4YSize) >> 2);

            u4ExtraReferenceNs = ((u4PoolSize - u4MVSize)/(u4YSize + u4CSize));;
            ASSERT(u4ExtraReferenceNs >= FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL - FBM_FBG_H264_FHD_REFERNCE_NS);

            if (u4ExtraReferenceNs > FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL - FBM_FBG_H264_FHD_REFERNCE_NS)
            {
                u4ExtraReferenceNs = FBM_FBG_H264_FHD_REFERNCE_NS_TWO_POOL - FBM_FBG_H264_FHD_REFERNCE_NS;
            }

#elif defined(CC_H264_LV5_SUPPORT_SINGLE_POOL)
           
            ASSERT(FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL > FBM_FBG_H264_FHD_REFERNCE_NS);
            u4ExtraReferenceNs = FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL - FBM_FBG_H264_FHD_REFERNCE_NS;
            u4MVSize = ((u4ExtraReferenceNs * u4YSize) >> 2);

            u4ExtraReferenceNs = ((FMB_EXTRA_FOR_SINGLE_H264_LV5 - u4MVSize)/(u4YSize + u4CSize));;
            ASSERT(u4ExtraReferenceNs >= FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL - FBM_FBG_H264_FHD_REFERNCE_NS);

            if (u4ExtraReferenceNs > FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL - FBM_FBG_H264_FHD_REFERNCE_NS)
            {
                u4ExtraReferenceNs = FBM_FBG_H264_FHD_REFERNCE_NS_ONE_POOL - FBM_FBG_H264_FHD_REFERNCE_NS;
            }
            
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
            u4Addr = FBM_ALIGN_MASK(prFbmPool->u4Addr + prFbmPool->u4Size - FMB_EXTRA_FOR_SINGLE_H264_LV5, FBM_FMG_Y_ALIGMENT);
            u4PoolSize = FMB_EXTRA_FOR_SINGLE_H264_LV5;

#endif
            u4ExtraReferenceNs = FBM_MIN(u4ExtraReferenceNs, u4ExtraFbNs);

            for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase; ((u4FbIdx < _arFbg[ucFbgId].ucFbNs + u4ExtraReferenceNs)&&(u4FbIdx < FBM_MAX_FB_NS_PER_GROUP)); u4FbIdx++)
            {
                // Y
                _arFbg[ucFbgId].au4AddrY[u4FbIdx] = u4Addr;
                FBM_GFX_MEMSET((UINT8*)u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), _arFbg[ucFbgId].u4FbHeight, 0x0);
                u4Addr += u4YSize;

                _arFbg[ucFbgId].au4AddrC[u4FbIdx] = u4Addr;
                FBM_GFX_MEMSET((UINT8*)u4Addr, (_arFbg[ucFbgId].u4FbWidth / u4WidthDiv), (_arFbg[ucFbgId].u4FbHeight >> 1), 0x80);
                u4Addr += u4CSize;
            }

            _arFbg[ucFbgId].ucFbNs += u4ExtraReferenceNs;

            if (_arFbg[ucFbgId].ucMvBufNs + u4ExtraReferenceNs > FBM_MAX_FB_NS_PER_GROUP-1)
            {
                ASSERT(_arFbg[ucFbgId].ucMvBufNs + u4ExtraReferenceNs <= FBM_MAX_FB_NS_PER_GROUP-1);
                FBM_MUTEX_UNLOCK(ucFbgId);
                return;
            }

            if (_arFbg[ucFbgId].ucFbgType == FBM_FBG_TYPE_1080HD_RR)
            {
                u4MvSize = u4YSize >> 1;
            }
            else
            {
                u4MvSize = u4YSize >> 2;
            }
            
            for (u4FbIdx = _arFbg[ucFbgId].ucMvBufNs; ((u4FbIdx < _arFbg[ucFbgId].ucMvBufNs + u4ExtraReferenceNs)&&(u4FbIdx < (FBM_MAX_FB_NS_PER_GROUP-1))); u4FbIdx++)
            {
                _arFbg[ucFbgId].au4AddrMv[u4FbIdx] = u4Addr;
                u4Addr += u4MvSize;
            }

            _arFbg[ucFbgId].ucMvBufNs += u4ExtraReferenceNs;

            // TODO: Need update _arFbg[ucFbgId].u4WorkBufSize for H.264?             
            //_arFbg[ucFbgId].u4WorkBufSize = ?
            
            if (u4ExtraFbNs >= u4ExtraReferenceNs)
            {
                u4ExtraFbNs -= u4ExtraReferenceNs;
            }
            
            ASSERT(u4ExtraFbNs == 0);
            
        }                
#endif

    }
    
    if (_arFbg[ucFbgId].eDynamicMode == FBM_DYNAMIC_SAME)
    {    //recover the memset func ptr
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_MEMSET, (UINT32)au4CbFunc[FBM_CB_FUNC_GFX_MEMSET]);
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_TASK_MEMSET, (UINT32)au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET]);
        FBM_RegCbFunc(FBM_CB_FUNC_GFX_TASK_MEMSET_DONE, (UINT32)au4CbFunc[FBM_CB_FUNC_GFX_TASK_MEMSET_DONE]);
    }

    //fbm memory error check
    {
        FBM_POOL_T* prFbmPool;
#if defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) || defined(CC_VOMX_TV_COEXIST)
        FBM_POOL_T* prFbmPool2;
#if 0//defined(CC_VOMX_TV_COEXIST) && defined(CC_VOMX_TWO_INST)
        FBM_POOL_T* prFbmPool3;
        prFbmPool3 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG3);
#endif 
        prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
#endif
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);

#if defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) || defined(CC_VOMX_TV_COEXIST)
        if(!(((_arFbg[ucFbgId].au4AddrMv[0] + _arFbg[ucFbgId].u4WorkBufSize) <= (prFbmPool->u4Addr + prFbmPool->u4Size)) ||
                ((_arFbg[ucFbgId].au4AddrMv[0] + _arFbg[ucFbgId].u4WorkBufSize) <= (prFbmPool2->u4Addr + prFbmPool2->u4Size))
#if 0//defined(CC_VOMX_TV_COEXIST) && defined(CC_VOMX_TWO_INST)
        || ((_arFbg[ucFbgId].au4AddrMv[0] + _arFbg[ucFbgId].u4WorkBufSize) <= (prFbmPool3->u4Addr + prFbmPool3->u4Size))
#endif
        ))
#else
        if((_arFbg[ucFbgId].au4AddrMv[0] + _arFbg[ucFbgId].u4WorkBufSize) > (prFbmPool->u4Addr + prFbmPool->u4Size))
#endif
        {
            LOG(1,"warning ! frame buffer allocation error\n");
            //ASSERT(0);
        }
    }


    {
        UINT32 u4FbIdx;

        if ((_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264) ||
                (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_AVS) ||
                (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_RAW) ||
                (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_JPEG)||
                (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_VP8)||
                (_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_VP6))

        {
            // create semaphore, Empty Q
            VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)_arFbg[ucFbgId].ucFbNs) == OSR_OK);
        }
        else
        {
            // create semaphore, Empty Q
            VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)_arFbg[ucFbgId].ucFbNs) == OSR_OK);

            // create semaphore, Empty BQ
            VERIFY(_FBM_csema_reset(&_arFbg[ucFbgId].hEmptyBQSemaphore, _arFbg[ucFbgId].hMutex, (INT32)(_arFbg[ucFbgId].ucFbNs - FBM_REF_FB_NS_PER_GROUP)) == OSR_OK);

#ifdef CC_VDP_FULL_ISR
            _arFbg[ucFbgId].hEmptyQSemaphore.prState = &_rState;
            _arFbg[ucFbgId].hEmptyBQSemaphore.prState = &_rState;
#endif
        }

        // Nivia and William Co-design
        _arFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbFRef = FBM_FB_ID_UNKNOWN;
        _arFbg[ucFbgId].ucFbBRef = FBM_FB_ID_UNKNOWN;

        if((prPar->ucCftFbgId != FBM_FBG_ID_UNKNOWN) && (prPar->ucCftFbgId < FBG_MAX_NS))
        {
            if (_FBM_2FBsChkDynamicDone(ucFbgId))
            {
                //Do nothing
            }
            else if (_arFbg[ucFbgId].eDynamicMode == FBM_DYNAMIC_SAME)
            {
                _FBM_2FBsConflictList(ucFbgId);
            }
        }
        for (u4FbIdx = _arFbg[ucFbgId].ucFbNsBase; u4FbIdx < _arFbg[ucFbgId].ucFbNs; u4FbIdx++)
        {
            //LOG(2, "FB[%d-%d] Try put into emptyQ.\n", ucFbgId, u4FbIdx);
            _FBM_PutFrameBufferToEmptyQ(ucFbgId, (UCHAR)(u4FbIdx));
        }
    }

    ASSERT(_arFbg[ucFbgId].hEmptyQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);
    ASSERT(_arFbg[ucFbgId].hEmptyBQSemaphore.hMutex == _arFbg[ucFbgId].hMutex);

    FBM_MUTEX_UNLOCK(ucFbgId);
}

//-------------------------------------------------------------------------
/** FBM_SetFrameBufferLayout
 *  Fill frame buffer with constant color
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  u4Color   color.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFrameBufferLayout(UCHAR ucFbgId, UCHAR ucFbId, UINT32 u4Addr, BOOL fgIsY)
{
    
    if (u4Addr == 0)
    {
        return;
    }

    if (VERIFY_FBG(ucFbgId) || VERIFY_FB(ucFbgId, ucFbId))
    {
        return;
    }

    if (fgIsY)
    {
        _arFbg[ucFbgId].au4AddrY[ucFbId] = u4Addr;
    }
    else
    {
        _arFbg[ucFbgId].au4AddrC[ucFbId] = u4Addr;
    }

}

//-------------------------------------------------------------------------
/** FBM_SetFrameBufferColor
 *  Fill frame buffer with constant color
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  u4Color   color.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFrameBufferColor(UCHAR ucFbgId, UCHAR ucFbId, UINT32 u4Color)
{
    UCHAR* pucYAddr;
    UCHAR* pucCAddr;
    UINT32 u4YSize;
    UINT32 u4CSize;
    UINT32 u4Idx;
    UCHAR ucY;
    UCHAR ucCb;
    UCHAR ucCr;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    switch (_arFbg[ucFbgId].ucFbgType)
    {
    case FBM_FBG_TYPE_1080HD:
        u4YSize = FBM_FBG_TYPE_1080HD_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_1080HD_C_SIZE;
        break;

    case FBM_FBG_TYPE_720HD:
        u4YSize = FBM_FBG_TYPE_720HD_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_720HD_C_SIZE;
        break;

    case FBM_FBG_TYPE_PAL:
        u4YSize = FBM_FBG_TYPE_PAL_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_PAL_C_SIZE;
        break;

    case FBM_FBG_TYPE_NTSC:
        u4YSize = FBM_FBG_TYPE_NTSC_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_NTSC_C_SIZE;
        break;

    default:
        u4YSize = 0;
        u4CSize = 0;
        ASSERT(0);
        break;
    }

    if (_arFbg[ucFbgId].ucFbgCm == FBM_CM_422)
    {
        u4CSize *= 2;
    }
    else if (_arFbg[ucFbgId].ucFbgCm == FBM_CM_444)
    {
        u4CSize *= 4;
    }
    else
    {
    }

    ucY = (UCHAR)((u4Color >> 16) & 0xFF);
    ucCb = (UCHAR)((u4Color >> 8) & 0xFF);
    ucCr = (UCHAR)((u4Color) & 0xFF);

    pucYAddr = (UCHAR*) _arFbg[ucFbgId].au4AddrY[ucFbId];
    pucCAddr = (UCHAR*)_arFbg[ucFbgId].au4AddrC[ucFbId];

    // use Gfx Engine in the future
    FBM_MEMSET(pucYAddr, ucY, u4YSize);

    for (u4Idx = 0; u4Idx < u4CSize; u4Idx += 2)
    {
        pucCAddr[u4Idx + 0] = ucCb;
        pucCAddr[u4Idx + 1] = ucCr;
    }
}


//-------------------------------------------------------------------------
/** FBM_SetFrameBufferZebraColor
 *  Fill frame buffer with constant color
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFrameBufferZebraColor(UCHAR ucFbgId, UCHAR ucFbId)
{
    UCHAR* pucYAddr;
    UCHAR* pucCAddr;
    UCHAR ucYColor;
    UINT32 u4YSize;
    UINT32 u4CSize;
    UINT32 u4Idx;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    switch (_arFbg[ucFbgId].ucFbgType)
    {
    case FBM_FBG_TYPE_1080HD:
        u4YSize = FBM_FBG_TYPE_1080HD_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_1080HD_C_SIZE;
        break;

    case FBM_FBG_TYPE_720HD:
        u4YSize = FBM_FBG_TYPE_720HD_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_720HD_C_SIZE;
        break;

    case FBM_FBG_TYPE_PAL:
        u4YSize = FBM_FBG_TYPE_PAL_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_PAL_C_SIZE;
        break;

    case FBM_FBG_TYPE_NTSC:
        u4YSize = FBM_FBG_TYPE_NTSC_Y_SIZE;
        u4CSize = FBM_FBG_TYPE_NTSC_C_SIZE;
        break;

    default:
        u4YSize = 0;
        u4CSize = 0;
        ASSERT(0);
        break;
    }

    if (_arFbg[ucFbgId].ucFbgCm == FBM_CM_422)
    {
        u4CSize *= 2;
    }
    else if (_arFbg[ucFbgId].ucFbgCm == FBM_CM_444)
    {
        u4CSize *= 4;
    }
    else
    {
    }

    pucYAddr = (UCHAR*) _arFbg[ucFbgId].au4AddrY[ucFbId];
    pucCAddr = (UCHAR*)_arFbg[ucFbgId].au4AddrC[ucFbId];

    ucYColor = 0;
    for (u4Idx = 0; u4Idx < u4YSize; u4Idx++)
    {
        pucYAddr[u4Idx] = ucYColor;

        if ((u4Idx & 0x3F) == 0x3F)
        {
            ucYColor = ~ucYColor;
        }
    }

    // use Gfx Engine in the future
    FBM_MEMSET(pucCAddr, 0x80, u4CSize);
}


//-------------------------------------------------------------------------
/** FBM_SetFrameBufferPlayBackNs
 *  Set frame buffer playback number
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  ucPlayBackNs   playback number.
 *  @param  ucChangeFieldNs   change field number.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFrameBufferPlayBackNs(UCHAR ucFbgId, UCHAR ucFbId, UCHAR ucPlayBackNs, UCHAR ucChangeFieldNs)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    _arFbg[ucFbgId].aucPlayBackNs[ucFbId] = ucPlayBackNs;
    _arFbg[ucFbgId].aucChangeFieldNs[ucFbId] = ucChangeFieldNs;
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferPlayBackNs
 *  Get frame buffer playback number
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return playback number.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFrameBufferPlayBackNs(UCHAR ucFbgId, UCHAR ucFbId)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return 0;
    }

    return _arFbg[ucFbgId].aucPlayBackNs[ucFbId];
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferChangeFieldNs
 *  Get frame buffer change field number
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return change field number.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFrameBufferChangeFieldNs(UCHAR ucFbgId, UCHAR ucFbId)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return 0;
    }

    return _arFbg[ucFbgId].aucChangeFieldNs[ucFbId];
}


//-------------------------------------------------------------------------
/** FBM_ChkFrameBufferLastPlayBack
 *  Check frame buffer last playback
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return 1/0 for last playback or NOT.
 */
//-------------------------------------------------------------------------
UCHAR FBM_ChkFrameBufferLastPlayBack(UCHAR ucFbgId, UCHAR ucFbId)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return 1;
    }

    if (_arFbg[ucFbgId].aucPlayBackNs[ucFbId] > 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


//-------------------------------------------------------------------------
/** FBM_QueryStatus
 *  Query FBM status
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_QueryStatus(UCHAR ucFbgId)
{
    UCHAR ucIdx;

    if ((ucFbgId < FBG_MAX_NS) &&
        (_arFbg[ucFbgId].ucFbgType != FBM_FBG_TYPE_UNKNOWN) &&
        (_arFbg[ucFbgId].ucFbgId != FBM_FBG_ID_UNKNOWN))
    {
        _FbgStatus(_arFbg[ucFbgId].ucFbgId);
        return;
    }

    for (ucIdx = 0; ucIdx < FBG_MAX_NS; ucIdx++)
    {
        if ((_arFbg[ucIdx].ucFbgType != FBM_FBG_TYPE_UNKNOWN) &&
                (_arFbg[ucIdx].ucFbgId != FBM_FBG_ID_UNKNOWN))
        {
            _FbgStatus(_arFbg[ucIdx].ucFbgId);
        }
    }

    _FBM_PoolStatus();
}

//-------------------------------------------------------------------------
/** FBM_RegCbFunc
 */
//-------------------------------------------------------------------------
void FBM_RegCbFunc(FBM_CB_FUNC_ENUM_T eFuncType, UINT32 u4FuncPtr)
{
    if (eFuncType < FBM_CB_FUNC_NS)
    {
        if (u4FuncPtr == 0)
        {
            _arFbmCbFunc.au4CbFunc[eFuncType] = u4FuncPtr;
            _arFbmCbFunc.au4CbFuncCRC[eFuncType] = u4FuncPtr;
        }
        else
        {
            _arFbmCbFunc.au4CbFunc[eFuncType] = u4FuncPtr;
            _arFbmCbFunc.au4CbFuncCRC[eFuncType] = ~u4FuncPtr;
        }
    }
    else
    {
        ASSERT(0);
    }
}

//-------------------------------------------------------------------------
/** FBM_RegFbgCbFunc
 */
//-------------------------------------------------------------------------
void FBM_RegFbgCbFunc(UCHAR ucFbgId, FBM_CB_FUNC_ENUM_T eFuncType, UINT32 u4FuncPtr, UINT32 u4Tag)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }
    if (eFuncType < FBM_CB_FUNC_NS)
    {
        if (u4FuncPtr == 0)
        {
            _arFbmCbFunc.aau4CbFunc[ucFbgId][eFuncType] = u4FuncPtr;
            _arFbmCbFunc.aau4CbFuncCRC[ucFbgId][eFuncType] = u4FuncPtr;
            _arFbmCbFunc.aau4CbFuncTag[ucFbgId][eFuncType] = u4Tag;
        }
        else
        {
            _arFbmCbFunc.aau4CbFunc[ucFbgId][eFuncType] = u4FuncPtr;
            _arFbmCbFunc.aau4CbFuncCRC[ucFbgId][eFuncType] = ~u4FuncPtr;
            _arFbmCbFunc.aau4CbFuncTag[ucFbgId][eFuncType] = u4Tag;
        }
    }
    else
    {
        ASSERT(0);
    }
}

//-------------------------------------------------------------------------
/** FBM_GetCbFuncTbl
 */
//-------------------------------------------------------------------------
FBM_CB_FUNC_T* FBM_GetCbFuncTbl(void)
{
    return (&_arFbmCbFunc);
}

//-------------------------------------------------------------------------
/** FBM_FbgChgNotify
 *  FBM notify (MPEG to VDP)
 *  @param  ucFbgId   FBG index.
 *  @param  ucEsId   ESindex.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_FbgChgNotify(UCHAR ucFbgId, UCHAR ucEsId)
{
    UCHAR ucPlayMode = FBM_FBG_MODE_NS;
    UCHAR ucPreFbgId = FBM_FBG_ID_UNKNOWN;
    
    // Note about usage
    // This function is called back by mpeg decode to indicate that
    // Frame buffer group ID is change.
    // It will by pass this notify to video plane driver

    if(ucFbgId != FBM_FBG_ID_UNKNOWN && (ucFbgId<FBG_MAX_NS))
    {
        _arFbg[ucFbgId].u1DecoderSrcId = ucEsId;
        if(_arFbg[ucFbgId].eDynamicMode != FBM_DYNAMIC_MODE_NONE)
        {
            return ;
        }
    }
    if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_THUMBNAIL_MODE))
    {
        return;
    }

    ucPreFbgId = FBM_GetFbgByEs(ucEsId);
    
    FBM_GetPlayMode(ucFbgId,&ucPlayMode);
    if (ucPlayMode == FBM_FBG_NOTDISPLAY_MODE)
    {
        LOG(5, "FBM_FBG_NOTDISPLAY_MODE\n");
        _VdpCheckFbgReady(ucFbgId, ucEsId);
        return;
    }

    if (FBM_CHECK_CB_FUNC_VERIFY(_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_FBG_CHG_IND], _arFbmCbFunc.au4CbFuncCRC[FBM_CB_FUNC_FBG_CHG_IND]))
    {
#if defined(CC_53XX_SWDMX_V2) && defined(CC_MT5365)
        VDP_CONF_T* prVdpConf = VDP_GetConf(VDP_1); // get which decoder is connect to vdp1
#endif

        ((FBM_FBG_CHG_IND_FUNC)_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_FBG_CHG_IND])(ucFbgId, ucEsId);

#if defined(CC_53XX_SWDMX_V2) && defined(CC_MT5365)
        if(prVdpConf != NULL)
        {
            if(ucFbgId == FBM_FBG_ID_UNKNOWN && prVdpConf->ucInputPort[0] == ucEsId)
            {
                UINT8 u1Idx;
                UINT8 u1TargetEsId = MAX_ES_NS;
                UINT8 u1FbgId = FBM_FBG_ID_UNKNOWN;
                UCHAR ucEs2FbgId = FBM_FBG_ID_UNKNOWN;

                for(u1Idx=0 ; u1Idx < MAX_ES_NS ; u1Idx++)
                {
                    ucEs2FbgId = FBM_GetFbgByEs(u1Idx);
                    if((ucEs2FbgId != FBM_FBG_ID_UNKNOWN) && (u1Idx != ucEsId))
                    {
                        u1FbgId = ucEs2FbgId;
                        u1TargetEsId = u1Idx;
                    }
                }

                if(u1FbgId != FBM_FBG_ID_UNKNOWN)
                {
                    ((FBM_FBG_CHG_IND_FUNC)_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_FBG_CHG_IND])(FBM_FBG_ID_UNKNOWN, u1TargetEsId);
                    VDP_SetInput(VDP_1,u1TargetEsId,0);
                    ((FBM_FBG_CHG_IND_FUNC)_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_FBG_CHG_IND])(u1FbgId, u1TargetEsId);
                }
            }
        }
#endif
    }
    else
    {
        ASSERT(_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_FBG_CHG_IND] == 0);
    }

    if  ((ucFbgId != FBM_FBG_ID_UNKNOWN && (ucFbgId<FBG_MAX_NS)) && (FBM_CHECK_CB_FUNC_VERIFY(_arFbmCbFunc.aau4CbFunc[ucFbgId][FBM_CB_FUNC_FBG_CHG_EX_IND],
                                 _arFbmCbFunc.aau4CbFuncCRC[ucFbgId][FBM_CB_FUNC_FBG_CHG_EX_IND])))
    {
        ((FBM_FB_READY_IND_FUNC_EX)_arFbmCbFunc.aau4CbFunc[ucFbgId][FBM_CB_FUNC_FBG_CHG_EX_IND])(
            _arFbmCbFunc.aau4CbFuncTag[ucFbgId][FBM_CB_FUNC_FBG_CHG_EX_IND],
            0);
    }
    if (ucFbgId == FBM_FBG_ID_UNKNOWN  && ucPreFbgId != FBM_FBG_ID_UNKNOWN)
    {
        if (_arFbg[ucPreFbgId].fgEnableNewSeamless)
        {
            _VDP_Prescale_Flush(_arFbg[ucPreFbgId].ucSeamlessRzId, TRUE);
        }
    }

    if (ucFbgId < FBG_MAX_NS)
    {
        VDEC_FbgChgHdlr(ucEsId,ucFbgId, _arFbg[ucFbgId].u4FbMemoryPool,
                        (_arFbg[ucFbgId].u4FbMemoryPool + _arFbg[ucFbgId].u4FbMemoryPoolSize));
    }
}

//-------------------------------------------------------------------------
/** FBM_PicStructChgNotify
 *  FBM notify (MPEG to VDP)
 *  @param  ucEsId   ESindex.
 *  @param  ucPicStruct   Frame/Field.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_PicStructChgNotify(UCHAR ucEsId, UCHAR ucPicStruct)
{
    // Note about usage
    // This function is called back by mpeg decode to indicate that
    // Picture structure is change. (frame/field)
    // It will by pass this notify to video plane driver

    if (FBM_CHECK_CB_FUNC_VERIFY(_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_PICSTRUCT_CHG_IND], _arFbmCbFunc.au4CbFuncCRC[FBM_CB_FUNC_PICSTRUCT_CHG_IND]))
    {
        ((FBM_PICSTRUCT_CHG_IND_FUNC)_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_PICSTRUCT_CHG_IND])(ucEsId, ucPicStruct);
    }
    else
    {
        ASSERT(_arFbmCbFunc.au4CbFunc[FBM_CB_FUNC_PICSTRUCT_CHG_IND] == 0);
    }
}

//-------------------------------------------------------------------------
/** FBM_SetSyncStc
 *  Set AV Sync Info
 *  @param  ucFbgId   FBG index
 *  @param  ucMode   AV Sync Mode
 *  @param  ucStcId   AV Sync Source
  *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetSyncStc(UCHAR ucFbgId, UCHAR ucMode, UCHAR ucStcId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    ASSERT(ucMode < VID_SYNC_MODE_NS);
    ASSERT(ucStcId < STC_SRC_NS);


    _arFbg[ucFbgId].ucStcSrc = ucStcId;
#if defined(CC_B2R_EMULATION)
    _arFbg[ucFbgId].ucAvSyncMode = VID_SYNC_MODE_NONE;
#else
    _arFbg[ucFbgId].ucAvSyncMode = ucMode;
#endif
}


//-------------------------------------------------------------------------
/** FBM_GetSyncStc
 *  GetV Sync Info
 *  @param  ucFbgId   FBG index
 *  @param  pucMode   AV Sync Mode pointer
 *  @param  pucStcId   AV Sync Source pointer
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetSyncStc(UCHAR ucFbgId, UCHAR* pucMode, UCHAR* pucStcId)
{
    if ((pucMode == NULL) || (pucStcId == NULL))
    {
        return;
    }

    if (VERIFY_FBG(ucFbgId))
    {
        *pucMode = VID_SYNC_MODE_NS;
        *pucStcId = STC_SRC_NS;
        return;
    }

    *pucMode = _arFbg[ucFbgId].ucAvSyncMode;
    *pucStcId = _arFbg[ucFbgId].ucStcSrc;
}

//-------------------------------------------------------------------------
/** FBM_SetAVSyncStatus
 *  Set AV Sync status (Repeat or Drop or ...)
 *  @param  ucFbgId   FBG index
 *  @param  u4AVSyncStatus   A/V sync mode
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetAVSyncStatus(UCHAR ucFbgId, UINT32 u4AVSyncStatus)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    _arFbg[ucFbgId].u4AvSyncStatus = u4AVSyncStatus;
}

//-------------------------------------------------------------------------
/** FBM_GetAVSyncStatus
 *  Get AV Sync status (Repeat or Drop or ...)
 *  @param  ucFbgId   FBG index
 *  @param  pu4AVSyncStatus   A/V sync mode
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetAVSyncStatus(UCHAR ucFbgId, UINT32* pu4AVSyncStatus)
{
    if (pu4AVSyncStatus == NULL)
    {
        return;
    }
    
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    *pu4AVSyncStatus = _arFbg[ucFbgId].u4AvSyncStatus;
}

//-------------------------------------------------------------------------
/** FBM_GetEmptyQCount
 *  Get Empty Queue Count
 *  @param  ucFbgId   FBG index
 *  @param  pucCount   Pointer of return results
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetEmptyQCount(UCHAR ucFbgId, UCHAR* pucCount)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    *pucCount = _arFbg[ucFbgId].rEmptyQ.ucCount;
}

UINT8 FBM_GetDecoderSrcId(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return 0xFF;
    }

    return _arFbg[ucFbgId].u1DecoderSrcId;
}

UINT8 FBM_GetFbgAppMode(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return 0xFF;
    }

    return _arFbg[ucFbgId].u1FbgAppMode;
}

//-------------------------------------------------------------------------
/** FBM_SetPlayMode
 *  Set Play Mode (DTV or MM or ...)
 *  @param  ucFbgId   FBG index
 *  @param  ucPlayMode   Play Mode
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetPlayMode(UCHAR ucFbgId, UCHAR ucPlayMode)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    _arFbg[ucFbgId].ucPlayMode = ucPlayMode;
}

void FBM_SetDecoderFB(UINT8 u1DecoderType,UINT8 u1NumOfFB)
{
    _fgSetDecoderFb = TRUE;
    _u1DecoderType = u1DecoderType;
    _u1NumOfFb = u1NumOfFB;
}

void FBM_SetColorMode(UINT8 u1ColorMode)
{
    if(u1ColorMode == FBM_CM_420 || u1ColorMode ==FBM_CM_422 )
    {
        LOG(1, "Color mode %d is selected.\n", u1ColorMode);
        _u1FbgColorMode = u1ColorMode;
    }
}

//-------------------------------------------------------------------------
/** FBM_GetPlayMode
 *  Get Play Mode (DTV or MM or ...)
 *  @param  ucFbgId   FBG index
 *  @param  pucPlayMode   Play Mode pointer
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetPlayMode(UCHAR ucFbgId, UCHAR* pucPlayMode)
{
    if ((pucPlayMode == NULL))
    {
        return;
    }

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    *pucPlayMode = _arFbg[ucFbgId].ucPlayMode;
}

//-------------------------------------------------------------------------
/** FBM_CheckPadding
 *  Check if pool memory corruption
 *  @param NONE
 *  @return NONE.
 */
//-------------------------------------------------------------------------

void FBM_CheckPadding(void)
{
#ifndef __KERNEL__
    UINT32 u4PoolIdx;
    UINT32 u4Idx;
    UINT32 u4PadAddr;
    UINT32 u4PadLen;
    UINT32 u4Addr;
    UINT32 u4Valid;

    u4Valid = 1;
    for (u4PoolIdx = 1 ; u4PoolIdx < (UINT32)FBM_POOL_TYPE_NS; u4PoolIdx++)
    {
        if ((_prPool[u4PoolIdx].u4Inherit == 0) &&
                (_prPool[u4PoolIdx].u4Addr > DRVCUST_OptGet(eFbmMemAddr)))
        {
            u4PadAddr = _prPool[u4PoolIdx].u4Addr - FBM_POOL_PADDING_SIZE;
            u4PadLen = FBM_POOL_PADDING_SIZE;

            for (u4Idx = 0; u4Idx < u4PadLen; u4Idx++)
            {
                u4Addr = u4PadAddr + u4Idx;

                if (*(UCHAR*)u4Addr != FBM_POOL_PADDING)
                {
                    LOG(1, "Pool(%s) memory corruption! addr=0x%x, val=0x%x\n", FBM_GetPoolName((UCHAR)u4PoolIdx), u4Addr, *(UCHAR*)u4Addr);

                    u4Valid = 0;

                    u4Idx = u4PadLen;
                }
            }
        }
    }

    if (u4Valid)
    {
        LOG(1, "No memory corruption\n");
    }
#else
    LOG(1, "NOT Support!\n");
#endif
}


//-------------------------------------------------------------------------
/** FBM_ResetFbChangeField
 *  Wait Frame buffer change field
 *  @param  ucFbgType   FBG type.
 *  @return lock status.
 */
//-------------------------------------------------------------------------
INT32 FBM_ResetFbChangeField(UCHAR ucFbgId) {

    if (VERIFY_FBG(ucFbgId))
    {
        return OSR_INV_ARG;
    }

    if (x_sema_lock(_arFbg[ucFbgId].hChgFieldSema, X_SEMA_OPTION_NOWAIT) == OSR_OK)
    {
    }
    else
    {
    }

    return OSR_OK;
}


//-------------------------------------------------------------------------
/** FBM_WaitFbChangeField
 *  Wait Frame buffer change field
 *  @param  ucFbgType   FBG type.
 *  @param  u4Timeout   time to wait.
 *  @return lock status.
 */
//-------------------------------------------------------------------------
INT32 FBM_WaitFbChangeField(UCHAR ucFbgId, UINT32 u4Timeout)
{
    INT32 i4Ret;

    if (VERIFY_FBG(ucFbgId))
    {
        return OSR_INV_ARG;
    }

    i4Ret = x_sema_lock_timeout(_arFbg[ucFbgId].hChgFieldSema, u4Timeout);

    return i4Ret;
}


//-------------------------------------------------------------------------
/** FBM_FbChangeField
 *  Frame buffer change field
 *  @param  ucFbgType   FBG type.
 *  @return unlock status.
 */
//-------------------------------------------------------------------------
void FBM_FbChangeField(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    if ((ucFbgId == FBM_FBG_ID_UNKNOWN) ||
            (_arFbg[ucFbgId].hChgFieldSema == 0))
    {
        return;
    }

    if (_arFbg[ucFbgId].hChgFieldSema)
    {
        VERIFY(x_sema_unlock(_arFbg[ucFbgId].hChgFieldSema) == OSR_OK);
    }
}


//-------------------------------------------------------------------------
/** FBM_SetFbNextFieldPlayBackNs
 *  Set frame buffer next field playback number
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  ucPlayBackNs   playback number.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFbNextFieldPlayBackNs(UCHAR ucFbgId, UCHAR ucFbId, UCHAR ucPlayBackNs)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    _arFbg[ucFbgId].aucChgFldPlayBackNs[ucFbId] = ucPlayBackNs;

}


//-------------------------------------------------------------------------
/** FBM_GetFbNextFieldPlayBackNs
 *  Get frame buffer next field playback number
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return playback number.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFbNextFieldPlayBackNs(UCHAR ucFbgId, UCHAR ucFbId)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return 0;
    }

    return _arFbg[ucFbgId].aucChgFldPlayBackNs[ucFbId];
}

UINT32 FBM_GetVDecMemSize(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }
    return _arFbg[ucFbgId].u4FbMemoryPoolSize;
}

UINT32 FBM_GetRunningVdecMemSize(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }
    if(_arFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264)
    {
        return _arFbg[ucFbgId].u4FbMemoryPoolSize;
    }
    else
    {
        return _arFbg[ucFbgId].ucFbNs * ( (_arFbg[ucFbgId].u4FbWidth * _arFbg[ucFbgId].u4FbHeight *3) >>1) + FBM_WORK_BUF_SIZE;
    }
}

BOOL FBM_ChkFbgCreateFromInst(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return FALSE;
    }

#ifdef CC_HWCOMP_UT
    return TRUE;
#else
    return _arFbg[ucFbgId].fgCreateFromInst;
#endif
}
#ifdef FBM_CS_LOG
void FBM_FlushFbCsLog(void)
{
    _FbmCsLog(24, 0, _u4EQHandle);

}
#endif
#if 1
void FBM_SetCodecInfo(UCHAR ucFbgId, ENUM_CODEC_INFO_T eCodecInfo)
{
    _arFbg[ucFbgId].eCodecType = eCodecInfo;
}

ENUM_CODEC_INFO_T FBM_GetCodecInfo(UCHAR ucFbgId)
{
   ENUM_CODEC_INFO_T eCodecInfo;
   if (VERIFY_FBG(ucFbgId))
       {
           return E_VDEC_CODEC_UNKNOWN;
       }
     eCodecInfo=_arFbg[ucFbgId].eCodecType;
    return  eCodecInfo;
}
BOOL FBM_GetCodecResolution(UCHAR ucFbgId,FBM_CODEC_RESOLUTION_T* rResolution)
{
   if (VERIFY_FBG(ucFbgId))
       {
       return FALSE;
       }
    rResolution->u4Width = _arFbg[ucFbgId].rSeqHdr.u2HSize;
    rResolution->u4Height = _arFbg[ucFbgId].rSeqHdr.u2VSize;
    rResolution->u4OrgWidth = _arFbg[ucFbgId].rSeqHdr.u2OrgWidth;
    rResolution->u4OrgHeight = _arFbg[ucFbgId].rSeqHdr.u2OrgHeight;
    return TRUE;
}

#endif

UCHAR FBM_GetFbgByEs(UCHAR ucEsId)
{
    UCHAR ucFbgId = FBM_FBG_ID_UNKNOWN;
    UCHAR ucFbgIdx;

    do
    {
        if(ucEsId >= MAX_ES_NS)
        {
            break;
        }

        for(ucFbgIdx = 0; ucFbgIdx < FBG_MAX_NS; ucFbgIdx ++)
        {
            if(VERIFY_FBG(ucFbgIdx))
            {
                continue;
            }
            
            if(_arFbg[ucFbgIdx].u1DecoderSrcId == ucEsId)
            {
                ucFbgId = ucFbgIdx;
                break;
            }
        }
    }while(0);

    return ucFbgId;
}
