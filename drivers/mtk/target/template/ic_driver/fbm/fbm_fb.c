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
 * $RCSfile: fbm_fb.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file fbm_fb.c
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "fbm_fb.h"
#include "fbm_drvif.h"
#include "fbm_debug.h"
#include "x_assert.h"
#include "x_syslog.h"


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

// FBM Frame Buffer Status Log
//#define FBM_FB_LOG


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------
#define FBM_WAIT_FB_TIMEOUT 1000    //wait FB for 1000ms when the first pic is not displayed yet.

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

#define VERIFY_FBG(id)                                  \
    ((id >= FBG_MAX_NS) ||                              \
    (_prFbg[id].ucFbgType == FBM_FBG_TYPE_UNKNOWN) ||   \
    (_prFbg[id].ucFbgId != id))

#define VERIFY_FB(gid, id)                              \
    (id >= _prFbg[gid].ucFbNs)

#define IS_REFERENCE_FB(gid, id)                        \
    ((id == _prFbg[gid].ucFbFRef) || (id == _prFbg[gid].ucFbBRef))

#define NOT_REFERENCE_FB(gid, id)                        \
    ((id != _prFbg[gid].ucFbFRef) && (id != _prFbg[gid].ucFbBRef))

#ifdef CC_VDP_FULL_ISR

#define FBM_MUTEX_LOCK(id)                              \
    (*_prFbg[id].prState = x_crit_start())

#define FBM_MUTEX_UNLOCK(id)                            \
    (x_crit_end(*_prFbg[id].prState))

#else

#define FBM_MUTEX_LOCK(id)                              \
    (VERIFY(x_sema_lock(_prFbg[id].hMutex, X_SEMA_OPTION_WAIT) == OSR_OK))

#define FBM_MUTEX_UNLOCK(id)                            \
    (VERIFY(x_sema_unlock(_prFbg[id].hMutex) == OSR_OK))

#endif


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

static void _FbmPutFrameBufferToDispQ(UCHAR ucFbgId, UCHAR ucFbId);

static UCHAR _FbmGetRefFrameBufferFromEmptyQ(UCHAR ucFbgId);

static UCHAR _FbmGetBFrameBufferFromEmptyQ(UCHAR ucFbgId);

#ifdef FBM_FB_LOG
static void _FbmFbLog(UINT32 u4FbIdx, UINT32 u4FbStatus);
#else
#define _FbmFbLog(x, y)     ((void)0)
#endif

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

static FBM_FBG_T* _prFbg = NULL;

static FBM_CB_FUNC_T* _prFbmCbFunc = NULL;

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** _FbmPutFrameBufferToDispQ
 *  Put FB to display queue
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
static void _FbmPutFrameBufferToDispQ(UCHAR ucFbgId, UCHAR ucFbId)
{
    UCHAR ucNextWriteIdx;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    ASSERT(_prFbg[ucFbgId].rDisplayQ.ucCount < FBM_MAX_FB_NS_PER_GROUP);

    ucNextWriteIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + _prFbg[ucFbgId].rDisplayQ.ucCount) % FBM_MAX_FB_NS_PER_GROUP;
    _prFbg[ucFbgId].rDisplayQ.aucQueue[ucNextWriteIdx] = ucFbId;
    _prFbg[ucFbgId].rDisplayQ.ucCount++;
}


//-------------------------------------------------------------------------
/** _FbmGetRefFrameBufferFromEmptyQ
 *  Get reference FB from empty queue
 *  @param  ucFbgId   FBG index.
 *  @return FB index.
 */
//-------------------------------------------------------------------------
static UCHAR _FbmGetRefFrameBufferFromEmptyQ(UCHAR ucFbgId)
{
    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    if ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0)
    {
        while (_prFbg[ucFbgId].rEmptyQ.ucCount > 0)
        {
            ucFbId = _prFbg[ucFbgId].rEmptyQ.aucQueue[_prFbg[ucFbgId].rEmptyQ.ucReadIdx];

            if (++(_prFbg[ucFbgId].rEmptyQ.ucReadIdx) >= FBM_MAX_FB_NS_PER_GROUP)
            {
                _prFbg[ucFbgId].rEmptyQ.ucReadIdx = 0;
            }
            _prFbg[ucFbgId].rEmptyQ.ucCount--;

            ASSERT(ucFbId != FBM_FB_ID_UNKNOWN);
            return ucFbId;
        }
    }
    else
    {
        UINT32 u4Count;
        UINT32 u4ReadIdx;
        UINT32 u4TargetIdx;

        ucFbId = FBM_FB_ID_UNKNOWN;

        // search for R frame from Empty Queue
        u4Count = _prFbg[ucFbgId].rEmptyQ.ucCount;
        u4ReadIdx = _prFbg[ucFbgId].rEmptyQ.ucReadIdx;
        u4TargetIdx = FBM_MAX_FB_NS_PER_GROUP;
        while (u4Count > 0)
        {
            ucFbId = _prFbg[ucFbgId].rEmptyQ.aucQueue[u4ReadIdx];

            if (IS_REFERENCE_FB(ucFbgId, ucFbId))
            {    // found R frame
                u4TargetIdx = u4ReadIdx;
                u4Count = 0;
            }
            else
            {    // skip Reference frame
                ucFbId = FBM_FB_ID_UNKNOWN;
                if (++u4ReadIdx >= FBM_MAX_FB_NS_PER_GROUP)
                {
                    u4ReadIdx = 0;
                }
                u4Count--;
            }
        }
 
        if (u4TargetIdx != FBM_MAX_FB_NS_PER_GROUP)
        {
            // move back B frame
            while (u4TargetIdx != _prFbg[ucFbgId].rEmptyQ.ucReadIdx)
            {
                if (u4TargetIdx > 0)
                {
                    u4ReadIdx = u4TargetIdx - 1;
                }
                else
                {
                    u4ReadIdx = FBM_MAX_FB_NS_PER_GROUP - 1;
                }

                _prFbg[ucFbgId].rEmptyQ.aucQueue[u4TargetIdx] = _prFbg[ucFbgId].rEmptyQ.aucQueue[u4ReadIdx];
                    
                u4TargetIdx = u4ReadIdx;
            }
        }
        else
        {
            ASSERT(0);
        }

        _prFbg[ucFbgId].rEmptyQ.ucReadIdx = (_prFbg[ucFbgId].rEmptyQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
        _prFbg[ucFbgId].rEmptyQ.ucCount--;

        return ucFbId;
    }

    return FBM_FB_ID_UNKNOWN;
}


//-------------------------------------------------------------------------
/** _FbmGetBFrameBufferFromEmptyQ
 *  Get B FB from empty queue
 *  @param  ucFbgId   FBG index.
 *  @return FB index.
 */
//-------------------------------------------------------------------------
static UCHAR _FbmGetBFrameBufferFromEmptyQ(UCHAR ucFbgId)
{
    UCHAR ucFbId;
    UINT32 u4Count;
    UINT32 u4ReadIdx;
    UINT32 u4TargetIdx;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    ucFbId = FBM_FB_ID_UNKNOWN;

    // search for B frame from Empty Queue
    u4Count = _prFbg[ucFbgId].rEmptyQ.ucCount;
    u4ReadIdx = _prFbg[ucFbgId].rEmptyQ.ucReadIdx;
    u4TargetIdx = FBM_MAX_FB_NS_PER_GROUP;
    while (u4Count > 0)
    {
        ucFbId = _prFbg[ucFbgId].rEmptyQ.aucQueue[u4ReadIdx];
 
        if (NOT_REFERENCE_FB(ucFbgId, ucFbId))
        {    // found B frame
            u4TargetIdx = u4ReadIdx;
            u4Count = 0;
        }
        else
        {    // skip Reference frame
            ucFbId = FBM_FB_ID_UNKNOWN;
            if (++u4ReadIdx >= FBM_MAX_FB_NS_PER_GROUP)
            {
                u4ReadIdx = 0;
            }
            u4Count--;
        }
    }

    if (u4TargetIdx != FBM_MAX_FB_NS_PER_GROUP)
    {
        // move back reference frame
        while (u4TargetIdx != _prFbg[ucFbgId].rEmptyQ.ucReadIdx)
        {
            if (u4TargetIdx > 0)
            {
                u4ReadIdx = u4TargetIdx - 1;
            }
            else
            {
                u4ReadIdx = FBM_MAX_FB_NS_PER_GROUP - 1;
            }

            _prFbg[ucFbgId].rEmptyQ.aucQueue[u4TargetIdx] = _prFbg[ucFbgId].rEmptyQ.aucQueue[u4ReadIdx];
                
            u4TargetIdx = u4ReadIdx;        
        }
    }
    else
    {
        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
        {
            return FBM_FB_ID_UNKNOWN;
        }
        else
        {
            ASSERT(0);
        }
    }

    _prFbg[ucFbgId].rEmptyQ.ucReadIdx = (_prFbg[ucFbgId].rEmptyQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
    _prFbg[ucFbgId].rEmptyQ.ucCount--;
    return ucFbId;
}


//-------------------------------------------------------------------------
/** _FbmGetEmptyFrameBufferFromEmptyQ
 *  Get empty FB from empty queue. H264 ONLY.
 *  @param  ucFbgId   FBG index.
 *  @return FB index.
 */
//-------------------------------------------------------------------------
static UCHAR _FbmGetEmptyFrameBufferFromEmptyQ(UCHAR ucFbgId)
{
    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

 
    while (_prFbg[ucFbgId].rEmptyQ.ucCount > 0)
    {
        ucFbId = _prFbg[ucFbgId].rEmptyQ.aucQueue[_prFbg[ucFbgId].rEmptyQ.ucReadIdx];

        if (++(_prFbg[ucFbgId].rEmptyQ.ucReadIdx) >= FBM_MAX_FB_NS_PER_GROUP)
        {
            _prFbg[ucFbgId].rEmptyQ.ucReadIdx = 0;
        }
        _prFbg[ucFbgId].rEmptyQ.ucCount--;

        ASSERT(ucFbId != FBM_FB_ID_UNKNOWN);
        return ucFbId;
    }    

    return FBM_FB_ID_UNKNOWN;
}


#ifdef __KERNEL__
//-------------------------------------------------------------------------
/** _FbmGetFrameBufferFromDispQ
 *  Get FB from empty queue
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId    FB index.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
static void _FbmGetFrameBufferFromDispQ(UCHAR ucFbgId, UCHAR ucFbId)
{
    UINT32 u4Count;
    UINT32 u4ReadIdx;
    UINT32 u4TargetIdx;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    ASSERT(_prFbg[ucFbgId].rDisplayQ.ucCount < FBM_MAX_FB_NS_PER_GROUP);

    // search FB from Display Queue
    u4Count = _prFbg[ucFbgId].rDisplayQ.ucCount;
    u4ReadIdx = _prFbg[ucFbgId].rDisplayQ.ucReadIdx;
    u4TargetIdx = FBM_MAX_FB_NS_PER_GROUP;

    while (u4Count > 0)
    {
        if (ucFbId == _prFbg[ucFbgId].rDisplayQ.aucQueue[u4ReadIdx])
        {
            u4TargetIdx = u4ReadIdx;
            u4Count = 0;
        }
        else
        {    
            if (++u4ReadIdx >= FBM_MAX_FB_NS_PER_GROUP)
            {
                u4ReadIdx = 0;
            }
            u4Count--;
        }
    }

    if (u4TargetIdx != FBM_MAX_FB_NS_PER_GROUP)
    {
        // move back other frame buffer
        while (u4TargetIdx != _prFbg[ucFbgId].rDisplayQ.ucReadIdx)
        {
            if (u4TargetIdx > 0)
            {
                u4ReadIdx = u4TargetIdx - 1;
            }
            else
            {
                u4ReadIdx = FBM_MAX_FB_NS_PER_GROUP - 1;
            }

            _prFbg[ucFbgId].rDisplayQ.aucQueue[u4TargetIdx] = _prFbg[ucFbgId].rDisplayQ.aucQueue[u4ReadIdx];
                
            u4TargetIdx = u4ReadIdx;        
        }
    }
    else
    {
        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
        {
            return;
        }
        else
        {
            ASSERT(0);
        }
    }

    _prFbg[ucFbgId].rDisplayQ.ucReadIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
    _prFbg[ucFbgId].rDisplayQ.ucCount--;

}
#endif


#ifdef FBM_FB_LOG
//-----------------------------------------------------------------------------
/** Brief of _FbmFbLog.
 */
//-----------------------------------------------------------------------------
static void _FbmFbLog(UINT32 u4FbIdx, UINT32 u4FbStatus)
{
    #define FBM_FB_LOG_NS 64
    static CHAR szDebug1[FBM_FB_LOG_NS];
    static CHAR szDebug2[FBM_FB_LOG_NS];
    static UINT32 u4DebugIdx = 0; 
    static HANDLE_T hMutex = NULL;

    if (hMutex == NULL)
    {
        // create semaphore
        VERIFY(x_sema_create(&hMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);

        for (u4DebugIdx = 0; u4DebugIdx < FBM_FB_LOG_NS; u4DebugIdx++)
        {
            szDebug1[u4DebugIdx] = 0;
            szDebug2[u4DebugIdx] = 0;
        }

        u4DebugIdx = 0;
    }

    VERIFY(x_sema_lock(hMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

    szDebug1[u4DebugIdx] = '0' + u4FbIdx;

    switch (u4FbStatus)
    {
        case FBM_FB_STATUS_EMPTY:
            szDebug2[u4DebugIdx] = 'E';
            break;

        case FBM_FB_STATUS_DECODE:
            szDebug2[u4DebugIdx] = 'D';
            break;

        case FBM_FB_STATUS_READY:
            szDebug2[u4DebugIdx] = 'R';
            break;

        case FBM_FB_STATUS_DISPLAYQ:
            szDebug2[u4DebugIdx] = 'Q';
            break;

        case FBM_FB_STATUS_LOCK:
            szDebug2[u4DebugIdx] = 'L';
            break;

        case FBM_FB_STATUS_DIGEST:
            szDebug2[u4DebugIdx] = 'G';
            break;

        case FBM_FB_STATUS_FIELD_READY:
            szDebug2[u4DebugIdx] = 'F';
            break;
            
        default:
            szDebug2[u4DebugIdx] = '?';
            break;
    }

    if ((++u4DebugIdx >= (FBM_FB_LOG_NS - 1)) || (u4FbIdx == 10))  
    {
        LOG(1, "%s\n", szDebug1);
        LOG(1, "%s\n", szDebug2);
        LOG(1, "u4DebugIdx = %d\n", u4DebugIdx);
        u4DebugIdx = 0;
    }

    VERIFY(x_sema_unlock(hMutex) == OSR_OK);        
}
#endif

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** _FBM_FbInit
 *  Set FBG structure pointer
 *  @param  prFbg   FBG structure pointer.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void _FBM_FbInit(FBM_FBG_T* prFbg)
{
    _prFbg = prFbg;

    _prFbmCbFunc = FBM_GetCbFuncTbl();
}


//-------------------------------------------------------------------------
/** _FBM_PutFrameBufferToEmptyQ
 *  Put FB to empty queue
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void _FBM_PutFrameBufferToEmptyQ(UCHAR ucFbgId, UCHAR ucFbId)
{
    UCHAR ucNextWriteIdx;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    if (_prFbg[ucFbgId].rEmptyQ.ucCount >= _prFbg[ucFbgId].ucFbNs)
    {
        if (_prFbg[ucFbgId].u4FbgFlag & (FBM_FLAG_RESET | FBM_FLAG_CLEAR_FRC_PRM))
        {
            return;
        }
        else
        if (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY)
        {
            LOG(1, "[Warning] Put to EmptyQ Again (%d, %d)\n", ucFbgId, ucFbId);
            return;
        }
        else
        {
            ASSERT(_prFbg[ucFbgId].rEmptyQ.ucCount < _prFbg[ucFbgId].ucFbNs);
        }
    }

    ucNextWriteIdx = (_prFbg[ucFbgId].rEmptyQ.ucReadIdx + _prFbg[ucFbgId].rEmptyQ.ucCount) % FBM_MAX_FB_NS_PER_GROUP;
    _prFbg[ucFbgId].rEmptyQ.aucQueue[ucNextWriteIdx] = ucFbId;
    _prFbg[ucFbgId].rEmptyQ.ucCount++;
}


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** FBM_SetFrameBufferFlag
 *  Set FBG flag
 *  @param  ucFbgId   FBG index.
 *  @param  u4Flag   flag.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFrameBufferFlag(UCHAR ucFbgId, UINT32 u4Flag)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (u4Flag == FBM_FLAG_SEQ_CHG)
    {
        _prFbg[ucFbgId].ucRecordNextFbId = 1;
        
        FBM_MUTEX_UNLOCK(ucFbgId);
        
        return;
    }
    _prFbg[ucFbgId].u4FbgFlag |= u4Flag;

    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_ChkFrameBufferFlag
 *  Check FBG flag
 *  @param  ucFbgId   FBG index.
 *  @param  u4Flag   flag.
 *  @return 1/0 for valid/invalid.
 */
//-------------------------------------------------------------------------
UCHAR FBM_ChkFrameBufferFlag(UCHAR ucFbgId, UINT32 u4Flag)
{
    UCHAR ucChk;

    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (_prFbg[ucFbgId].u4FbgFlag & u4Flag)
    {
        ucChk = 1;    
    }
    else
    {
        ucChk = 0;    
    }

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucChk;
}


//-------------------------------------------------------------------------
/** FBM_ChkFrameBufferFlagIsr
 *  Check FBG flag at ISR
 *  @param  ucFbgId   FBG index.
 *  @param  u4Flag   flag.
 *  @return 1/0 for valid/invalid.
 */
//-------------------------------------------------------------------------
UCHAR FBM_ChkFrameBufferFlagIsr(UCHAR ucFbgId, UINT32 u4Flag)
{
    UCHAR ucChk;

    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }

    if (_prFbg[ucFbgId].u4FbgFlag & u4Flag)
    {
        ucChk = 1;    
    }
    else
    {
        ucChk = 0;    
    }

    return ucChk;
}


//-------------------------------------------------------------------------
/** FBM_ClrFrameBufferFlag
 *  Clear FBG flag
 *  @param  ucFbgId   FBG index.
 *  @param  u4Flag   flag.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_ClrFrameBufferFlag(UCHAR ucFbgId, UINT32 u4Flag)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    _prFbg[ucFbgId].u4FbgFlag &= ~u4Flag;

    if ((u4Flag == FBM_FLAG_DEINTERLACE) &&
        (_prFbg[ucFbgId].u4ResidualFb == 1))
    {
        _prFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_RESET;
        _prFbg[ucFbgId].u4ResidualFree = 1;
    }

    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferNs
 *  Get FBG frame buffer number
 *  @param  ucFbgId   FBG index.
 *  @return frame buffer number.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFrameBufferNs(UCHAR ucFbgId)
{
    UCHAR ucFbNs;

    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucFbNs = _prFbg[ucFbgId].ucFbNs;

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucFbNs;
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferSize
 *  Get FBG frame buffer size
 *  @param  ucFbgId   FBG index.
 *  @param  pu4Width   width pointer.
 *  @param  pu4Height   height pointer.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetFrameBufferSize(UCHAR ucFbgId, UINT32 *pu4Width, UINT32 *pu4Height)
{
    if ((pu4Width == NULL) || (pu4Height == NULL))
    {
        return;
    }

    *pu4Width = NULL;
    *pu4Height = NULL;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    *pu4Width = _prFbg[ucFbgId].u4FbWidth;
    *pu4Height = _prFbg[ucFbgId].u4FbHeight;
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferStatus
 *  Get FBG frame buffer status
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return frame buffer status.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFrameBufferStatus(UCHAR ucFbgId, UCHAR ucFbId)
{
    UCHAR ucFbStatus;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return FBM_FB_STATUS_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucFbStatus = _prFbg[ucFbgId].aucFbStatus[ucFbId];

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucFbStatus;
}


//-------------------------------------------------------------------------
/** FBM_CheckFrameBufferStatus
 *  Check FBG frame buffer status
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  ucFbStatus   FB status.
 *  @return 1/0 for valid/invalid.
 */
//-------------------------------------------------------------------------
UCHAR FBM_CheckFrameBufferStatus(UCHAR ucFbgId, UCHAR ucFbId, UCHAR ucFbStatus)
{
    UCHAR ucValid;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return 0;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucValid = (ucFbStatus == _prFbg[ucFbgId].aucFbStatus[ucFbId]);

    // VDP protection
    if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_NOT_READY)
    {
        ucValid = 1;
    }

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucValid;
}


//-------------------------------------------------------------------------
/** FBM_SetFrameBufferStatus
 *  Set FBG frame buffer status
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  ucFbStatus   frame buffer status.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_SetFrameBufferStatus(UCHAR ucFbgId, UCHAR ucFbId, UCHAR ucFbStatus)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    // [LOG] FBM change FB status
    SYSLOG(FBM_PREFIX + ucFbgId, ucFbId, _prFbg[ucFbgId].aucFbStatus[ucFbId], ucFbStatus);
    _FbmFbLog(ucFbId, ucFbStatus);

//    LOG(7, "Fbg(%d) Fb(%d) %d >> %d\n", ucFbgId, ucFbId, _prFbg[ucFbgId].aucFbStatus[ucFbId], ucFbStatus);
//    LOG(7, "Fb(%d, %x) %d >> %d\n", ucFbId, _prFbg[ucFbgId].au4AddrY[ucFbId], _prFbg[ucFbgId].aucFbStatus[ucFbId], ucFbStatus);

    // All FB go to EMPTY status at NOT READY mode
    if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_NOT_READY)
    {
        ucFbStatus = FBM_FB_STATUS_EMPTY;
    }

    // If Display NOT ready, don't put FB in DisplayQ (Display Q >> Empty)
    if ((ucFbStatus == FBM_FB_STATUS_DISPLAYQ) &&
        ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_DISP_READY) == 0))
    {
        ucFbStatus = FBM_FB_STATUS_EMPTY;
    }

    if (ucFbId == _prFbg[ucFbgId].ucFbDecode)
    {
        // check current decode frame buffer status change
        if ((ucFbStatus != FBM_FB_STATUS_DECODE) &&
            (ucFbStatus != FBM_FB_STATUS_FIELD_READY))
        {
            _prFbg[ucFbgId].ucFbDecode = FBM_FB_ID_UNKNOWN;
        }

        // check back reference frame buffer status change
        if (ucFbId == _prFbg[ucFbgId].ucFbBRef)        
        {
            if ((ucFbStatus == FBM_FB_STATUS_READY) ||
                (ucFbStatus == FBM_FB_STATUS_FIELD_READY))
            {
                _prFbg[ucFbgId].ucFbBRefValid = 1;
            }
            else
            if (ucFbStatus == FBM_FB_STATUS_EMPTY)  //if FBM_FB_STATUS_FIELD_READY->FBM_FB_STATUS_EMPTY, set BRef invalid.
            {
                _prFbg[ucFbgId].ucFbBRefValid = 0;
            }
            else
            {
            }
        }
    }

    switch (ucFbStatus)
    {
        case FBM_FB_STATUS_EMPTY:
            if (!FBM_ChkFrameBufferFlag(ucFbgId , FBM_FLAG_FREEZE|FBM_FLAG_RELEASE_FBG))
            {
                ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] != FBM_FB_STATUS_DISPLAYQ);
            }

            if (_prFbg[ucFbgId].aucFbStatus[ucFbId] != FBM_FB_STATUS_EMPTY)
            {
                if (_prFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264)
                {
                    if (_prFbg[ucFbgId].afgRefList[ucFbId])
                    {
                        //don't release FB when it is still in the reference list.
                        break;
                    }
                    else
                    {
                        // release semaphore (Empty)
                        VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);                
                    }
                }
                else
                {
                        
                    if ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0)
                    {
                        if (IS_REFERENCE_FB(ucFbgId, ucFbId))
                        {    // release reference frame buffer
                        }
                        else
                        {    // release B frame buffer
                        
                            // release semaphore (B & Empty)
                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);
                        }

                        // release semaphore (Empty)
                        VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);
                    }
                    else
                    {
                        if (IS_REFERENCE_FB(ucFbgId, ucFbId))
                        {    // release reference frame buffer

                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyRQSemaphore) == OSR_OK);
                        }
                        else
                        {    // release B frame buffer
                        
                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);
                        }
                    }

                }
                _FBM_PutFrameBufferToEmptyQ(ucFbgId, ucFbId);
            }
            break;

        case FBM_FB_STATUS_DECODE:
            _prFbg[ucFbgId].ucFbDecode = ucFbId;
            break;

        case FBM_FB_STATUS_DISPLAYQ:

            if ((_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DECODE) || 
                    (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_FIELD_READY)||
                    (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_READY))
            {
                _FbmPutFrameBufferToDispQ(ucFbgId, ucFbId);
            }
            else
            {
                ASSERT((_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DECODE) || 
                        (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_FIELD_READY)||
                        (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_READY));

                if (_prFbg[ucFbgId].aucFbStatus[ucFbId] != FBM_FB_STATUS_EMPTY)
                {
                    if (_prFbg[ucFbgId].u4VDecFmt == FBM_VDEC_H264)
                    {
                        if (_prFbg[ucFbgId].afgRefList[ucFbId])
                        {
                            //don't release FB when it is still in the reference list.
                            break;
                        }
                        else
                        {
                            // release semaphore (Empty)
                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);                
                        }
                    }
                    else
                    {
                
                        if ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0)
                        {
                            if (IS_REFERENCE_FB(ucFbgId, ucFbId))
                            {    // release reference frame buffer
                            }
                            else
                            {    // release B frame buffer
                            
                                // release semaphore (B & Empty)
                                VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);
                            }

                            // release semaphore (Empty)
                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);
                        }
                        else
                        {
                            if (IS_REFERENCE_FB(ucFbgId, ucFbId))
                            {    // release reference frame buffer

                                VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyRQSemaphore) == OSR_OK);
                            }
                            else
                            {    // release B frame buffer
                            
                                VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);
                            }
                        }
                    }

                    _FBM_PutFrameBufferToEmptyQ(ucFbgId, ucFbId);
                }
            }
            break;
            
        case FBM_FB_STATUS_READY:
            break;        
        case FBM_FB_STATUS_LOCK:
            if (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY)
            {
                ucFbStatus = FBM_FB_STATUS_EMPTY;
            }
            break;
        default:
            break;
    }

    _prFbg[ucFbgId].aucFbStatus[ucFbId] = ucFbStatus;

    FBM_MUTEX_UNLOCK(ucFbgId);

    if (ucFbStatus == FBM_FB_STATUS_DISPLAYQ)
    {
        if (FBM_CHECK_CB_FUNC_VERIFY(_prFbmCbFunc->au4CbFunc[FBM_CB_FUNC_FB_READY_IND], _prFbmCbFunc->au4CbFuncCRC[FBM_CB_FUNC_FB_READY_IND]))
        {
            ((FBM_FB_READY_IND_FUNC)_prFbmCbFunc->au4CbFunc[FBM_CB_FUNC_FB_READY_IND])(ucFbgId);
        }
        else
        {
            ASSERT(_prFbmCbFunc->au4CbFunc[FBM_CB_FUNC_FB_READY_IND] == 0);
        }
    }
}


//-------------------------------------------------------------------------
/** FBM_CheckFrameBufferDispQ
 *  Get FBG display queue size
 *  @param  ucFbgId   FBG index.
 *  @return display queue size.
 */
//-------------------------------------------------------------------------
UCHAR FBM_CheckFrameBufferDispQ(UCHAR ucFbgId)
{
    UCHAR ucCount;
    
    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucCount = _prFbg[ucFbgId].rDisplayQ.ucCount;

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucCount;
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferFromDispQ
 *  Get frame buffer from display queue
 *  @param  ucFbgId   FBG index.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetFrameBufferFromDispQ(UCHAR ucFbgId)
{
    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucFbId = FBM_FB_ID_UNKNOWN;
    if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
    {
        ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
        _prFbg[ucFbgId].rDisplayQ.ucReadIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
        _prFbg[ucFbgId].rDisplayQ.ucCount--;

        ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);
    }
    else
    {
        _FbmFbLog(9, FBM_FB_ID_UNKNOWN);
    }

    if ((_prFbg[ucFbgId].ucSeqChg) && (_prFbg[ucFbgId].ucNewFbId == ucFbId))
    {
        _prFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_SEQ_CHG;
        _prFbg[ucFbgId].ucSeqChg = 0;
    }

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucFbId;
}


//-------------------------------------------------------------------------
/** FBM_GetNextDispQPts
 *  Get next frame buffer's PTS from display queue
 *  @param  ucFbgId   FBG index.
 *  @return PTS value.
 */
//-------------------------------------------------------------------------
UINT32 FBM_GetNextDispQPts(UCHAR ucFbgId)
{
    UINT32 u4Pts;

    if (VERIFY_FBG(ucFbgId))
    {
        return 0;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    u4Pts = 0;        

    if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
    {
        UCHAR ucFbId;
        
        ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
        ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);

        u4Pts = _prFbg[ucFbgId].prPicHdr[ucFbId].u4PTS;
    }
    else
    {
        // return last display q item
        UCHAR ucFbId;
        UCHAR ucLastQ;

        if (_prFbg[ucFbgId].rDisplayQ.ucReadIdx == 0)
        {
            ucLastQ = FBM_MAX_FB_NS_PER_GROUP - 1;
        }
        else
        {
            ucLastQ = _prFbg[ucFbgId].rDisplayQ.ucReadIdx - 1;
        }

        ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[ucLastQ];

        if (ucFbId < FBG_MAX_NS)
        {
            u4Pts = _prFbg[ucFbgId].prPicHdr[ucFbId].u4PTS;
        }
    }

    FBM_MUTEX_UNLOCK(ucFbgId);

    return u4Pts;
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferAtIsr
 *  Get frame buffer from display queue at ISR
 *  @param  ucFbgId   FBG index.
 *  @param  pu4AddrY   Y Address.
 *  @param  pu4AddrC   C Address.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetFrameBufferAtIsr(UCHAR ucFbgId, UINT32* pu4AddrY, UINT32* pu4AddrC)
{
    if ((pu4AddrY == NULL) || (pu4AddrC == NULL))
    {
        return;
    }

    if (VERIFY_FBG(ucFbgId))
    {
        *pu4AddrY = NULL;
        *pu4AddrC = NULL;
        return;
    }

    if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
    {
        UCHAR ucFbId;
        ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
        
        *pu4AddrY = _prFbg[ucFbgId].au4AddrY[ucFbId];
        *pu4AddrC = _prFbg[ucFbgId].au4AddrC[ucFbId];

        // [LOG] FBM get fb at ISR
        SYSLOG(FBM_PREFIX + 30 + ucFbgId, ucFbId, _prFbg[ucFbgId].au4AddrY[ucFbId], _prFbg[ucFbgId].au4AddrC[ucFbId]);
    }
    else
    {
        *pu4AddrY = NULL;
        *pu4AddrC = NULL;

        // [LOG] FBM get fb at ISR
        SYSLOG(FBM_PREFIX + 30 + ucFbgId, FBM_FB_ID_UNKNOWN, 0, 0);

        LOG(7, "FBAI-F\n");
    }
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferPicHdr
 *  Get frame buffer picture header
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return picture header.
 */
//-------------------------------------------------------------------------
FBM_PIC_HDR_T *FBM_GetFrameBufferPicHdr(UCHAR ucFbgId, UCHAR ucFbId)
{
    if (ucFbId != _prFbg[ucFbgId].ucFbNs)
    {
        if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
        {
            return NULL;
        }
    }
    else if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DEBLOCKING))
    {
        if (VERIFY_FBG(ucFbgId))
        {
            return NULL;        
        }    
    }

    return &(_prFbg[ucFbgId].prPicHdr[ucFbId]);
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferSeqHdr
 *  Get frame buffer sequence header
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @return sequence header.
 */
//-------------------------------------------------------------------------
FBM_SEQ_HDR_T *FBM_GetFrameBufferSeqHdr(UCHAR ucFbgId)
{
    if (VERIFY_FBG(ucFbgId))
    {
        return NULL;
    }

    return &(_prFbg[ucFbgId].rSeqHdr);
}


//-------------------------------------------------------------------------
/** FBM_GetFrameBufferAddr
 *  Get frame buffer address
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index.
 *  @param  pu4AddrY   Y Address.
 *  @param  pu4AddrC   C Address.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetFrameBufferAddr(UCHAR ucFbgId, UCHAR ucFbId, UINT32 *pu4AddrY, UINT32 *pu4AddrC)
{
    if ((pu4AddrY == NULL) || (pu4AddrC == NULL))
    {
        return;
    }

    *pu4AddrY = NULL;
    *pu4AddrC = NULL;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    if (VERIFY_FB(ucFbgId, ucFbId))
    {
        if (ucFbId != _prFbg[ucFbgId].ucFbNs)
        {
            return;
        }
    }

    *pu4AddrY = _prFbg[ucFbgId].au4AddrY[ucFbId];
    *pu4AddrC = _prFbg[ucFbgId].au4AddrC[ucFbId];

}


void FBM_GetFrameBufferAddrMv(UCHAR ucFbgId, UCHAR ucFbId, UINT32 *pu4AddrMv)
{
    if (pu4AddrMv == NULL)
    {
        return;
    }

    *pu4AddrMv = NULL;

    if (VERIFY_FBG(ucFbgId))
    {
        return;
    }

    if (VERIFY_FB(ucFbgId, ucFbId))
    {
        return;
    }

    *pu4AddrMv = _prFbg[ucFbgId].au4AddrMv[ucFbId];    

}

//-------------------------------------------------------------------------
/** FBM_GetDecFrameBuffer
 *  Get decoding frame buffer
 *  @param  ucFbgId   FBG index.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetDecFrameBuffer(UCHAR ucFbgId)
{
    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucFbId = _prFbg[ucFbgId].ucFbDecode;

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucFbId;
}


//-------------------------------------------------------------------------
/** FBM_GetReadyFrameBuffer
 *  Get ready frame buffer
 *  @param  ucFbgId   FBG index.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetReadyFrameBuffer(UCHAR ucFbgId)
{
    // Notes about usage
    // Error Concealment background for I picture

    UCHAR ucFbId;
    UINT32 u4FbIdx;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    ucFbId = FBM_FB_ID_UNKNOWN;
    
    if ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0)
    {
        if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
        {
            // get the last FB at display Q
        
            u4FbIdx = ((_prFbg[ucFbgId].rDisplayQ.ucReadIdx + _prFbg[ucFbgId].rDisplayQ.ucCount) - 1) % FBM_MAX_FB_NS_PER_GROUP;    
            ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[u4FbIdx];

            ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);
        }
        else
        {
            // get locked frame buffer
            for(u4FbIdx = 0; u4FbIdx < _prFbg[ucFbgId].ucFbNs; u4FbIdx++)
            {
                if (_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
                {
                    ucFbId = u4FbIdx;
                    u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;
                }
            }

            // get any frame buffer which is NOT decoding 
            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                for(u4FbIdx = 0; u4FbIdx < _prFbg[ucFbgId].ucFbNs; u4FbIdx++)
                {
                    if (_prFbg[ucFbgId].ucFbDecode != u4FbIdx)
                    {
                        ucFbId = u4FbIdx;
                        u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;
                    }
                }
            }
        }
    }
    else
    {
        UINT32 u4SearchIdx;

        u4SearchIdx = 0;
        // search the Reference at display Q
        while (u4SearchIdx < _prFbg[ucFbgId].rDisplayQ.ucCount)
        {
            u4FbIdx = ((_prFbg[ucFbgId].rDisplayQ.ucReadIdx + _prFbg[ucFbgId].rDisplayQ.ucCount + u4SearchIdx) - 1) % FBM_MAX_FB_NS_PER_GROUP;
            ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[u4FbIdx];

            if (IS_REFERENCE_FB(ucFbgId, ucFbId))
            {
                u4SearchIdx = FBM_MAX_FB_NS_PER_GROUP;
            }
            else
            {
                ucFbId = FBM_FB_ID_UNKNOWN;                
            }
        }

        if (ucFbId == FBM_FB_ID_UNKNOWN)
        {
            // get locked frame buffer
            for(u4FbIdx = 0; u4FbIdx < _prFbg[ucFbgId].ucFbNs; u4FbIdx++)
            {
                if ((_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK) &&
                    (u4FbIdx < FBM_REF_FB_NS_PER_GROUP))
                {
                    ucFbId = u4FbIdx;
                    u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;
                }
            }
        }

        if (ucFbId == FBM_FB_ID_UNKNOWN)
        {
            // get ready frame buffer
            for(u4FbIdx = 0; u4FbIdx < _prFbg[ucFbgId].ucFbNs; u4FbIdx++)
            {
                if ((_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_READY) &&
                    (u4FbIdx < FBM_REF_FB_NS_PER_GROUP))
                {
                    ucFbId = u4FbIdx;
                    u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;
                }
            }
        }
        
        if (ucFbId == FBM_FB_ID_UNKNOWN)
        {
            // get any frame buffer which is NOT decoding 
            for(u4FbIdx = 0; u4FbIdx < _prFbg[ucFbgId].ucFbNs; u4FbIdx++)
            {
                if ((_prFbg[ucFbgId].ucFbDecode != u4FbIdx) &&
                    (u4FbIdx < FBM_REF_FB_NS_PER_GROUP))
                {
                    ucFbId = u4FbIdx;
                    u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;
                }
            }
        }
    }

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucFbId;
}


//-------------------------------------------------------------------------
/** FBM_GetRefFrameBuffer
 *  Get reference frame buffer
 *  @param  ucFbgId   FBG index.
 *  @param  ucFRefFbId   forward reference FB index pointer.
 *  @param  ucBRefFbId   backward reference FB index pointer.
 *  @return NONE.
 */
//-------------------------------------------------------------------------
void FBM_GetRefFrameBuffer(UCHAR ucFbgId, UCHAR* ucFRefFbId, UCHAR* ucBRefFbId)
{
    if ((ucFRefFbId == NULL) || 
        (ucBRefFbId == NULL))
    {
        return;
    }

    if (VERIFY_FBG(ucFbgId))
    {
        *ucFRefFbId = FBM_FB_ID_UNKNOWN;
        *ucBRefFbId = FBM_FB_ID_UNKNOWN;

        return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (_prFbg[ucFbgId].ucFbFRefValid)
    {
        *ucFRefFbId = _prFbg[ucFbgId].ucFbFRef;
    }
    else
    {
        *ucFRefFbId = FBM_FB_ID_UNKNOWN;        
    }

    if (_prFbg[ucFbgId].ucFbBRefValid)
    {
        *ucBRefFbId = _prFbg[ucFbgId].ucFbBRef;
    }
    else
    {
        *ucBRefFbId = FBM_FB_ID_UNKNOWN;        
    }
 
    FBM_MUTEX_UNLOCK(ucFbgId);
}


//-------------------------------------------------------------------------
/** FBM_GetEmptyRefFrameBuffer
 *  Get empty reference frame buffer
 *  @param  ucFbgId   FBG index.
 *  @param  u4Delay   time to wait.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetEmptyRefFrameBuffer(UCHAR ucFbgId, UINT32 u4Delay)
{
#ifdef CC_MT5351_SLT
    HAL_TIME_T rStart;
    HAL_TIME_T rStop;
    HAL_TIME_T rDelta;
#endif

    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

#ifdef CC_MT5351_SLT
    HAL_GetTime(&rStart);
#endif

    FBM_MUTEX_LOCK(ucFbgId);

    if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DISPLAYING))
    {
        u4Delay = FBM_WAIT_FB_TIMEOUT;
    }

    // We should not change status at NOT READY mode
    if (_prFbg[ucFbgId].u4FbgFlag & (FBM_FLAG_NOT_READY | FBM_FLAG_RESET | FBM_FLAG_CLEAR_FRC_PRM))
    {
        FBM_MUTEX_UNLOCK(ucFbgId);
        return FBM_FB_ID_UNKNOWN;
    }

    if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB)
    {
        INT32 i4ReturnValue;
        UINT32 u4GetB;

        // New design of fix reference FB (In order to handle no B bitstream at Full B Racing Mode
        // 1) Put all frame buffer at BQ first, RQ = 0
        // 2) When free frame buffer, put frame buffer to RQ if FB = F-REF or B-REF, else put frame buffer to BQ
        // 3) When get empty R frame buffer,
        //   A) If RQ not empty, get from RQ
        //   B) If BQ > 1, get from BQ
        //   C) If BQ = 1 and last decode frame is R, get from BQ
        //   D) else, wait RQ (never wait BQ)
        // It is not easy to have deadlock with (1) + (2) + (3)

        ucFbId = FBM_FB_ID_UNKNOWN;
        u4GetB = 0;
        // wait semaphore (Empty)
        if (_prFbg[ucFbgId].hEmptyRQSemaphore.i4Count != 0)
        {
            // A) If RQ not empty, get from RQ
            i4ReturnValue = _FBM_csema_lock(&_prFbg[ucFbgId].hEmptyRQSemaphore, X_SEMA_OPTION_NOWAIT, u4Delay);

            ASSERT(i4ReturnValue == OSR_OK);
        }
        else
        if (_prFbg[ucFbgId].hEmptyBQSemaphore.i4Count > 1)
        {
            // B) If BQ > 1, get from BQ
            i4ReturnValue = _FBM_csema_lock(&_prFbg[ucFbgId].hEmptyBQSemaphore, X_SEMA_OPTION_NOWAIT, u4Delay);
            u4GetB = 1;

            ASSERT(i4ReturnValue == OSR_OK);
        }
        else
        if ((_prFbg[ucFbgId].hEmptyBQSemaphore.i4Count == 1) && (_prFbg[ucFbgId].ucLastDecodeReference != 0))
        {
            // If BQ = 1 and last decode frame is R, get from BQ
            i4ReturnValue = _FBM_csema_lock(&_prFbg[ucFbgId].hEmptyBQSemaphore, X_SEMA_OPTION_NOWAIT, u4Delay);
            u4GetB = 1;

            ASSERT(i4ReturnValue == OSR_OK);
        }
        else
        {
            // else, wait RQ (never wait BQ)
            i4ReturnValue = _FBM_csema_lock(&_prFbg[ucFbgId].hEmptyRQSemaphore, X_SEMA_OPTION_WAIT, u4Delay);
        }
            
        if(i4ReturnValue == OSR_OK)
        {
            // wait semaphore success

            if (u4GetB != 0)
            {
                ucFbId = _FbmGetBFrameBufferFromEmptyQ(ucFbgId);
            }
            else
            {
                ucFbId = _FbmGetRefFrameBufferFromEmptyQ(ucFbgId);
            }            

            // Work around counting semaphore issue
            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;
            }

            if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DEBLOCKING))
            {
                if (_prFbg[ucFbgId].ucRecordNextFbId)
                {
                    _prFbg[ucFbgId].ucNewFbId = ucFbId;
                    _prFbg[ucFbgId].ucRecordNextFbId = 0;
                    _prFbg[ucFbgId].ucSeqChg = 1;
                }
            }
            
#if 0
            if (IS_REFERENCE_FB(ucFbgId, ucFbId) ||
                (FBM_FB_ID_UNKNOWN == _prFbg[ucFbgId].ucFbFRef) ||
                (FBM_FB_ID_UNKNOWN == _prFbg[ucFbgId].ucFbBRef))
            {    // we get an empty reference frame buffer
            }
            else
            {    // we get an empty B fram buffer

                ASSERT(0);
            }
#endif

            ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY);
        }
        else
        {
            // wait semaphore fail

            ASSERT(_prFbg[ucFbgId].hEmptyRQSemaphore.i4Count == 0);

            //// TODO
            LOG(1, "{FR}\n");
        }
    }
    else
    {
        ucFbId = FBM_FB_ID_UNKNOWN;
        // wait semaphore (Empty)
        if(_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyQSemaphore, X_SEMA_OPTION_WAIT, u4Delay) == OSR_OK)
        {
            // wait semaphore success
            
            ucFbId = _FbmGetRefFrameBufferFromEmptyQ(ucFbgId);

            // Work around counting semaphore issue
            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;
            }

            if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DEBLOCKING))
            {

                if (_prFbg[ucFbgId].ucRecordNextFbId)
                {
                    _prFbg[ucFbgId].ucNewFbId = ucFbId;
                    _prFbg[ucFbgId].ucRecordNextFbId = 0;            
                    _prFbg[ucFbgId].ucSeqChg = 1;
                }
            }
            
            if (IS_REFERENCE_FB(ucFbgId, ucFbId) ||
                (FBM_FB_ID_UNKNOWN == _prFbg[ucFbgId].ucFbFRef) ||
                (FBM_FB_ID_UNKNOWN == _prFbg[ucFbgId].ucFbBRef))
            {    // we get an empty reference frame buffer
            }
            else
            {    // we get an empty B fram buffer

                // In this case, old forward reference become B, Bug Fixed (20041110)
                if (_prFbg[ucFbgId].aucFbStatus[_prFbg[ucFbgId].ucFbFRef] != FBM_FB_STATUS_EMPTY)
                {
                    // wait semaphore (B & Empty)
    //                VERIFY(_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyBQSemaphore, X_SEMA_OPTION_NOWAIT, 0) == OSR_OK);
                    if (_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyBQSemaphore, X_SEMA_OPTION_NOWAIT, 0) != OSR_OK)
                    {
                        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
                        {
                            // reset, handle abnormal case

                            FBM_MUTEX_UNLOCK(ucFbgId);
                            return FBM_FB_ID_UNKNOWN;
                        }
                        else
                        {
                            ASSERT(0);
                        }
                    }

                    // London Bug
                    // Put old forward reference to Empty if it is still in ready status
                    if (_prFbg[ucFbgId].aucFbStatus[_prFbg[ucFbgId].ucFbFRef] == FBM_FB_STATUS_READY)
                    {
                        LOG(1, "[ERR] Reference FB leak!\n");

                        // release semaphore (B & Empty)
                        VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyBQSemaphore) == OSR_OK);

                        // release semaphore (Empty)
                        VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);

                        _FBM_PutFrameBufferToEmptyQ(ucFbgId, _prFbg[ucFbgId].ucFbFRef);

                        _prFbg[ucFbgId].aucFbStatus[_prFbg[ucFbgId].ucFbFRef] = FBM_FB_STATUS_EMPTY;
                    }
                }
            }

        #ifdef __KERNEL__
            if (_prFbg[ucFbgId].aucFbStatus[ucFbId] != FBM_FB_STATUS_EMPTY)
            {
                if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
                {
                    return FBM_FB_ID_UNKNOWN;
                }
                else
                {
                    switch (_prFbg[ucFbgId].aucFbStatus[ucFbId])
                    {
                        case FBM_FB_STATUS_DISPLAYQ:
                            _FbmGetFrameBufferFromDispQ(ucFbgId, ucFbId);
                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);
                            _FBM_PutFrameBufferToEmptyQ(ucFbgId, ucFbId);
                            _prFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_EMPTY;
                            break;
                        case FBM_FB_STATUS_DECODE:
                        case FBM_FB_STATUS_READY:
                        case FBM_FB_STATUS_LOCK:
                        default:    
                            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);
                            _FBM_PutFrameBufferToEmptyQ(ucFbgId, ucFbId);
                            _prFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_EMPTY;
                            break;
                    }                
                }
            }
        #else
            ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY);
        #endif
        }
        else
        {
            // wait semaphore fail
        
            ASSERT(_prFbg[ucFbgId].hEmptyQSemaphore.i4Count == 0);

            if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
            {
                // get frame buffer from display Q
            
                ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
                _prFbg[ucFbgId].rDisplayQ.ucReadIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
                _prFbg[ucFbgId].rDisplayQ.ucCount--;

                LOG(3, "{DQ}");

                ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);
            }
            else
            {
                UINT32 u4FbIdx;
            
                // it is possible. Both Ref is lock by vdp
                // 2005/02/18 William, I found this case when change channel, fbm reset is pending

                if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
                {
                    // get locked frame buffer
                    for(u4FbIdx = 0; u4FbIdx < FBM_MAX_FB_NS_PER_GROUP; u4FbIdx++)
                    {
                        if (_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
                        {
                            LOG(3, "{DL}\n");
                        
                            ucFbId = u4FbIdx;
                            u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;

                            // ask vdp to reset parameter, otherwise, assert at vdp set status to empty
                            _prFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_CLEAR_FRC_PRM;
                        }
                    }
                }

                if (ucFbId == FBM_FB_ID_UNKNOWN)
                {
                    FBM_MUTEX_UNLOCK(ucFbgId);
                    return FBM_FB_ID_UNKNOWN;
                }
            }
        }
    }

    if (ucFbId != FBM_FB_ID_UNKNOWN)
    {
        _prFbg[ucFbgId].ucLastDecodeReference = 1;
    }

    if ((ucFbId != FBM_FB_ID_UNKNOWN) && 
        (ucFbId != _prFbg[ucFbgId].ucFbBRef))
    {
#ifdef VDP_RACE_LOG
        if (ucFbId != _prFbg[ucFbgId].ucFbFRef)
        {
            VDP_FrcRacingLog('A');
            VDP_FrcRacingLog('0' + ucFbId);
        }
#endif
    
        _prFbg[ucFbgId].ucFbFRef = _prFbg[ucFbgId].ucFbBRef;
        _prFbg[ucFbgId].ucFbBRef = ucFbId;

        _prFbg[ucFbgId].ucFbFRefValid = _prFbg[ucFbgId].ucFbBRefValid;
        _prFbg[ucFbgId].ucFbBRefValid = 0;
    }

    // Auto-Change Empty Status to Decode Status
    // Bug Fixed (20041206)
    ASSERT(_prFbg[ucFbgId].ucFbDecode == FBM_FB_ID_UNKNOWN);
    if (ucFbId < FBM_MAX_FB_NS_PER_GROUP)
    {
        _FbmFbLog(ucFbId, FBM_FB_STATUS_DECODE);
        _prFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_DECODE;
         _prFbg[ucFbgId].ucFbDecode = ucFbId;
    }
    else
    {
        ucFbId = FBM_FB_ID_UNKNOWN;
        LOG(1, "[Warning] Get Empty Ref Fail (%d, %d)\n", ucFbgId, ucFbId);
//        ASSERT(0);
    }

    // [LOG] FBM get empty Ref FB
    SYSLOG(FBM_PREFIX + 10 + ucFbgId, ucFbId, _prFbg[ucFbgId].rEmptyQ.ucCount, 0);

    FBM_MUTEX_UNLOCK(ucFbgId);

#ifdef CC_MT5351_SLT
    HAL_GetTime(&rStop);

    HAL_GetDeltaTime(&rDelta, &rStart, &rStop);

    if (rDelta.u4Micros > 100000)
    {
        LOG(1, "GetRefTime %u.%6u\n", rDelta.u4Seconds, rDelta.u4Micros);
    }
#endif

    return ucFbId;
}


//-------------------------------------------------------------------------
/** FBM_GetEmptyBFrameBuffer
 *  Get empty B frame buffer
 *  @param  ucFbgId   FBG index.
 *  @param  u4Delay   time to wait.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetEmptyBFrameBuffer(UCHAR ucFbgId, UINT32 u4Delay)
{
#ifdef CC_MT5351_SLT
    HAL_TIME_T rStart;
    HAL_TIME_T rStop;
    HAL_TIME_T rDelta;
#endif

    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

#ifdef CC_MT5351_SLT
    HAL_GetTime(&rStart);
#endif

    FBM_MUTEX_LOCK(ucFbgId);

    if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DISPLAYING))
    {
        u4Delay = FBM_WAIT_FB_TIMEOUT;
    }

    // We should not change status at NOT READY mode
    if (_prFbg[ucFbgId].u4FbgFlag & (FBM_FLAG_NOT_READY | FBM_FLAG_RESET | FBM_FLAG_CLEAR_FRC_PRM))
    {
        FBM_MUTEX_UNLOCK(ucFbgId);
        return FBM_FB_ID_UNKNOWN;
    }

    ucFbId = FBM_FB_ID_UNKNOWN;
    // wait semaphore (B & Empty)
    if(_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyBQSemaphore, X_SEMA_OPTION_WAIT, u4Delay) == OSR_OK)
    {
        // wait semaphore success

        if ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0)
        {
            // wait semaphore (Empty)
            if (_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyQSemaphore, X_SEMA_OPTION_NOWAIT, 0) != OSR_OK)
            {
                if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
                {
                    // reset, handle abnormal case

                    FBM_MUTEX_UNLOCK(ucFbgId);
                    return FBM_FB_ID_UNKNOWN;
                }
                else
                {
                    ASSERT(0);
                }
            }
        }

        ucFbId = _FbmGetBFrameBufferFromEmptyQ(ucFbgId);

        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
        {
            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                LOG(1, "[Warning] Get Empty B Fail (%d, %d)\n", ucFbgId, ucFbId);
                
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;       
            }
        }

        ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY);
    }
    else
    {
        // wait semaphore fail
    
        ASSERT(_prFbg[ucFbgId].hEmptyBQSemaphore.i4Count == 0);

        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB)
        {
            //// TODO
            LOG(1, "{FB}\n");
        }
        else
        if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
        {
            ASSERT((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0);    // Handle Racing Mode
        
            ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
            _prFbg[ucFbgId].rDisplayQ.ucReadIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
            _prFbg[ucFbgId].rDisplayQ.ucCount--;

            LOG(3,"{DQ}");

            ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);
        }
        else
        {
            UINT32 u4FbIdx;
        
            // it is possible. Both Ref is lock by vdp
            // 2005/02/18 William, I found this case when change channel, fbm reset is pending

            if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
            {
                // get locked frame buffer
                for(u4FbIdx = 0; u4FbIdx < FBM_MAX_FB_NS_PER_GROUP; u4FbIdx++)
                {
                    if (_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
                    {
                        LOG(3, "{DL}\n");
                    
                        ucFbId = u4FbIdx;
                        u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;

                        // ask vdp to reset parameter, otherwise, assert at vdp set status to empty
                        _prFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_CLEAR_FRC_PRM;
                    }
                }
            }

            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;
            }
        }
    }

    if (ucFbId != FBM_FB_ID_UNKNOWN)
    {
        _prFbg[ucFbgId].ucLastDecodeReference = 0;
    }

    // Auto-Change Empty Status to Decode Status
    // Bug Fixed (20041206)
    ASSERT(_prFbg[ucFbgId].ucFbDecode == FBM_FB_ID_UNKNOWN);
    if (ucFbId < FBM_MAX_FB_NS_PER_GROUP)
    {    
        _FbmFbLog(ucFbId, FBM_FB_STATUS_DECODE);
        _prFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_DECODE;
        _prFbg[ucFbgId].ucFbDecode = ucFbId;
    }
    else
    {
        ucFbId = FBM_FB_ID_UNKNOWN;
        LOG(1, "[Warning] Get Empty B Fail (%d, %d)\n", ucFbgId, ucFbId);
//        ASSERT(0);
    }

    // [LOG] FBM get empty B FB
    SYSLOG(FBM_PREFIX + 20 + ucFbgId, ucFbId, _prFbg[ucFbgId].rEmptyQ.ucCount, 0);

    FBM_MUTEX_UNLOCK(ucFbgId);

#ifdef CC_MT5351_SLT
    HAL_GetTime(&rStop);

    HAL_GetDeltaTime(&rDelta, &rStart, &rStop);

    if (rDelta.u4Micros > 100000)
    {
        LOG(1, "GetBTime %u.%6u\n", rDelta.u4Seconds, rDelta.u4Micros);
    }
#endif
    
    return ucFbId;
}


//-------------------------------------------------------------------------
/** FBM_GetEmptyFrameBuffer
 *  Get empty frame buffer. H264 ONLY.
 *  @param  ucFbgId   FBG index.
 *  @param  u4Delay   time to wait.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------
UCHAR FBM_GetEmptyFrameBuffer(UCHAR ucFbgId, UINT32 u4Delay)
{
    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DISPLAYING))
    {
        u4Delay = FBM_WAIT_FB_TIMEOUT;
    }

    // We should not change status at NOT READY mode
    if (_prFbg[ucFbgId].u4FbgFlag & (FBM_FLAG_NOT_READY | FBM_FLAG_RESET | FBM_FLAG_CLEAR_FRC_PRM))
    {
        FBM_MUTEX_UNLOCK(ucFbgId);
        return FBM_FB_ID_UNKNOWN;
    }

    ucFbId = FBM_FB_ID_UNKNOWN;
    // wait semaphore (Empty)
    if(_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyQSemaphore, X_SEMA_OPTION_WAIT, u4Delay) == OSR_OK)
    {
        // wait semaphore success
        
        ucFbId = _FbmGetEmptyFrameBufferFromEmptyQ(ucFbgId);

        // Work around counting semaphore issue
        if (ucFbId == FBM_FB_ID_UNKNOWN)
        {
            FBM_MUTEX_UNLOCK(ucFbgId);
            return FBM_FB_ID_UNKNOWN;
        }

        if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DEBLOCKING))
        {

            if (_prFbg[ucFbgId].ucRecordNextFbId)
            {
                _prFbg[ucFbgId].ucNewFbId = ucFbId;
                _prFbg[ucFbgId].ucRecordNextFbId = 0;            
                _prFbg[ucFbgId].ucSeqChg = 1;
            }
        }
        
        ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY);
    }
    else
    {
        // wait semaphore fail
    
        ASSERT(_prFbg[ucFbgId].hEmptyQSemaphore.i4Count == 0);

        if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
        {
            // get frame buffer from display Q
        
            ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
            _prFbg[ucFbgId].rDisplayQ.ucReadIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
            _prFbg[ucFbgId].rDisplayQ.ucCount--;

            LOG(3, "{DQ}");

            ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);
        }
        else
        {
            UINT32 u4FbIdx;
        
            // it is possible. Both Ref is lock by vdp
            // 2005/02/18 William, I found this case when change channel, fbm reset is pending

            if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
            {
                // get locked frame buffer
                for(u4FbIdx = 0; u4FbIdx < FBM_MAX_FB_NS_PER_GROUP; u4FbIdx++)
                {
                    if (_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
                    {
                        LOG(3, "{DL}\n");
                    
                        ucFbId = u4FbIdx;
                        u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;

                        // ask vdp to reset parameter, otherwise, assert at vdp set status to empty
                        _prFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_CLEAR_FRC_PRM;
                    }
                }
            }

            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;
            }
        }
    }

    // Auto-Change Empty Status to Decode Status
    // Bug Fixed (20041206)
    ASSERT(_prFbg[ucFbgId].ucFbDecode == FBM_FB_ID_UNKNOWN);
    if (ucFbId < FBM_MAX_FB_NS_PER_GROUP)
    {
        _FbmFbLog(ucFbId, FBM_FB_STATUS_DECODE);
        _prFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_DECODE;
        _prFbg[ucFbgId].ucFbDecode = ucFbId;
    }
    else
    {
        ucFbId = FBM_FB_ID_UNKNOWN;
        LOG(1, "[Warning] Get Empty Ref Fail (%d, %d)\n", ucFbgId, ucFbId);
//        ASSERT(0);
    }

    // [LOG] FBM get empty Ref FB
    SYSLOG(FBM_PREFIX + 10 + ucFbgId, ucFbId, _prFbg[ucFbgId].rEmptyQ.ucCount, 0);

    FBM_MUTEX_UNLOCK(ucFbgId);

    return ucFbId;
}


//-------------------------------------------------------------------------
/** FBM_CanGetEmptyRefFrameBuffer
 *  Check empty reference frame buffer
 *  @param  ucFbgId   FBG index.
 *  @return frame buffer number.
 */
//-------------------------------------------------------------------------
UCHAR FBM_CanGetEmptyRefFrameBuffer(UCHAR ucFbgId)
{
    UCHAR rValue;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB)
    {
        rValue = (UCHAR) _prFbg[ucFbgId].hEmptyRQSemaphore.i4Count;
    }
    else
    {
        rValue = (UCHAR) _prFbg[ucFbgId].hEmptyQSemaphore.i4Count;
    }

    FBM_MUTEX_UNLOCK(ucFbgId);

    return rValue;
}


//-------------------------------------------------------------------------
/** FBM_CanGetEmptyBFrameBuffer
 *  Check empty B frame buffer
 *  @param  ucFbgId   FBG index.
 *  @return frame buffer number.
 */
//-------------------------------------------------------------------------
UCHAR FBM_CanGetEmptyBFrameBuffer(UCHAR ucFbgId)
{
    UCHAR rValue;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    rValue = (UCHAR) _prFbg[ucFbgId].hEmptyBQSemaphore.i4Count;

    FBM_MUTEX_UNLOCK(ucFbgId);

    return rValue;
}


//-------------------------------------------------------------------------
/** FBM_GetDbkEmptyRefFrameBuffer
 *  Get empty Ref frame buffer for DBK
 *  @param  ucFbgId   FBG index.
 *  @param  u4Delay   time to wait.
 *  @return frame buffer index.
 */
//-------------------------------------------------------------------------

UCHAR FBM_GetDbkEmptyRefFrameBuffer(UCHAR ucFbgId, UINT32 u4Delay)
{

#ifdef CC_MT5351_SLT
    HAL_TIME_T rStart;
    HAL_TIME_T rStop;
    HAL_TIME_T rDelta;
#endif

    UCHAR ucFbId;

    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FB_ID_UNKNOWN;
    }

#ifdef CC_MT5351_SLT
    HAL_GetTime(&rStart);
#endif

    FBM_MUTEX_LOCK(ucFbgId);

    if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DISPLAYING))
    {
        u4Delay = FBM_WAIT_FB_TIMEOUT;
    }

    // We should not change status at NOT READY mode
    if (_prFbg[ucFbgId].u4FbgFlag & (FBM_FLAG_NOT_READY | FBM_FLAG_RESET | FBM_FLAG_CLEAR_FRC_PRM))
    {
        FBM_MUTEX_UNLOCK(ucFbgId);
        return FBM_FB_ID_UNKNOWN;
    }

    ucFbId = FBM_FB_ID_UNKNOWN;
    // wait semaphore (B & Empty)
    if(_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyBQSemaphore, X_SEMA_OPTION_WAIT, u4Delay) == OSR_OK)
    {
        // wait semaphore success

        if ((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0)
        {
            // wait semaphore (Empty)
            if (_FBM_csema_lock(&_prFbg[ucFbgId].hEmptyQSemaphore, X_SEMA_OPTION_NOWAIT, 0) != OSR_OK)
            {
                if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
                {
                    // reset, handle abnormal case

                    FBM_MUTEX_UNLOCK(ucFbgId);
                    return FBM_FB_ID_UNKNOWN;
                }
                else
                {
                    ASSERT(0);
                }
            }
        }

        ucFbId = _FbmGetBFrameBufferFromEmptyQ(ucFbgId);

        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
        {
            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                LOG(1, "[Warning] Get Empty B Fail (%d, %d)\n", ucFbgId, ucFbId);
                
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;       
            }
        }

        ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY);
    }
    else
    {
        // wait semaphore fail
    
        ASSERT(_prFbg[ucFbgId].hEmptyBQSemaphore.i4Count == 0);

        if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB)
        {
            ASSERT(0);
        }
        else
        if (_prFbg[ucFbgId].rDisplayQ.ucCount > 0)
        {
            ASSERT((_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_FIX_REF_FB) == 0);    // Handle Racing Mode
        
            ucFbId = _prFbg[ucFbgId].rDisplayQ.aucQueue[_prFbg[ucFbgId].rDisplayQ.ucReadIdx];
            _prFbg[ucFbgId].rDisplayQ.ucReadIdx = (_prFbg[ucFbgId].rDisplayQ.ucReadIdx + 1) % FBM_MAX_FB_NS_PER_GROUP;
            _prFbg[ucFbgId].rDisplayQ.ucCount--;

            LOG(3,"{DQ}");

            ASSERT(_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_DISPLAYQ);
        }
        else
        {
            UINT32 u4FbIdx;
        
            // it is possible. Both Ref is lock by vdp
            // 2005/02/18 William, I found this case when change channel, fbm reset is pending

            if (_prFbg[ucFbgId].u4FbgFlag & FBM_FLAG_RESET)
            {
                // get locked frame buffer
                for(u4FbIdx = 0; u4FbIdx < FBM_MAX_FB_NS_PER_GROUP; u4FbIdx++)
                {
                    if (_prFbg[ucFbgId].aucFbStatus[u4FbIdx] == FBM_FB_STATUS_LOCK)
                    {
                        LOG(3, "{DL}\n");
                    
                        ucFbId = u4FbIdx;
                        u4FbIdx = FBM_MAX_FB_NS_PER_GROUP;

                        // ask vdp to reset parameter, otherwise, assert at vdp set status to empty
                        _prFbg[ucFbgId].u4FbgFlag |= FBM_FLAG_CLEAR_FRC_PRM;
                    }
                }
            }

            if (ucFbId == FBM_FB_ID_UNKNOWN)
            {
                FBM_MUTEX_UNLOCK(ucFbgId);
                return FBM_FB_ID_UNKNOWN;
            }
        }
    }

    // Auto-Change Empty Status to Decode Status
    // Bug Fixed (20041206)
    _FbmFbLog(ucFbId, FBM_FB_STATUS_DECODE);
    _prFbg[ucFbgId].aucFbStatus[ucFbId] = FBM_FB_STATUS_DECODE;   

    if (_prFbg[ucFbgId].ucRecordNextFbId)
    {
        _prFbg[ucFbgId].ucNewFbId = ucFbId;
        _prFbg[ucFbgId].ucRecordNextFbId = 0;
        _prFbg[ucFbgId].ucSeqChg = 1;
    }        

    
    // [LOG] FBM get empty B FB
    SYSLOG(FBM_PREFIX + 20 + ucFbgId, ucFbId, _prFbg[ucFbgId].rEmptyQ.ucCount, 0);

    FBM_MUTEX_UNLOCK(ucFbgId);

#ifdef CC_MT5351_SLT
    HAL_GetTime(&rStop);

    HAL_GetDeltaTime(&rDelta, &rStart, &rStop);

    if (rDelta.u4Micros > 100000)
    {
        LOG(1, "GetBTime %u.%6u\n", rDelta.u4Seconds, rDelta.u4Micros);
    }
#endif
    
    return ucFbId;    
}


//-------------------------------------------------------------------------
/** FBM_UpdateReferenceList
 *  Update Reference FB List. H264 ONLY.
 *  @param  ucFbgId   FBG index.
 *  @param  ucFbId   FB index. 
 *  @return NONE.
 */
//-------------------------------------------------------------------------

void FBM_UpdateReferenceList(UCHAR ucFbgId, UCHAR ucFbId, BOOL fgReference)
{
    if (_prFbg[ucFbgId].u4VDecFmt != FBM_VDEC_H264)
    {  
        return;
    }

    if (VERIFY_FBG(ucFbgId) || VERIFY_FB(ucFbgId, ucFbId))
    {
         return;
    }

    FBM_MUTEX_LOCK(ucFbgId);

    if (_prFbg[ucFbgId].afgRefList[ucFbId] && (!fgReference))
    {
        if (_prFbg[ucFbgId].aucFbStatus[ucFbId] == FBM_FB_STATUS_EMPTY)
        {
            VERIFY(_FBM_csema_unlock(&_prFbg[ucFbgId].hEmptyQSemaphore) == OSR_OK);                            
            _FBM_PutFrameBufferToEmptyQ(ucFbgId, ucFbId);            
        }
    }
    _prFbg[ucFbgId].afgRefList[ucFbId] = fgReference;

    FBM_MUTEX_UNLOCK(ucFbgId);

}


#if 0
UCHAR FBM_GetDbkSmallBFrameBuffer(UCHAR ucFbgId)
{
    UCHAR ucFbId;
    
    if (VERIFY_FBG(ucFbgId))
    {
        return FBM_FBG_ID_UNKNOWN;
    }

    ucFbId = _prFbg[ucFbgId].ucFbNs;

    return ucFbId;    
}


void FBM_SetFrameBufferFtrCnt(UCHAR ucFbgId, UCHAR ucFbId, UINT32 u4DbkOfStCnt, UINT32 u4Dbk4pCnt)
{
    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    _prFbg[ucFbgId].au4DbkOfstCnt[ucFbId] = u4DbkOfStCnt;
    _prFbg[ucFbgId].au4Dbk4pCnt[ucFbId] = u4Dbk4pCnt;

}


void FBM_GetFrameBufferFtrCnt(UCHAR ucFbgId, UCHAR ucFbId, UINT32 *pu4DbkOfStCnt, UINT32 *pu4Dbk4pCnt)
{
    if ((pu4DbkOfStCnt == NULL) || (pu4Dbk4pCnt == NULL))
    {
        return;
    }

    *pu4DbkOfStCnt = NULL;
    *pu4Dbk4pCnt = NULL;

    if ((VERIFY_FBG(ucFbgId)) || (VERIFY_FB(ucFbgId, ucFbId)))
    {
        return;
    }

    *pu4DbkOfStCnt = _prFbg[ucFbgId].au4DbkOfstCnt[ucFbId];
    *pu4Dbk4pCnt = _prFbg[ucFbgId].au4Dbk4pCnt[ucFbId];
}
#endif

#ifdef FBM_FB_LOG
void FBM_FlushFbLog(UINT32 u4FbIdx, UINT32 u4FbStatus)
{
    _FbmFbLog(u4FbIdx, u4FbStatus);
    
}

#endif


