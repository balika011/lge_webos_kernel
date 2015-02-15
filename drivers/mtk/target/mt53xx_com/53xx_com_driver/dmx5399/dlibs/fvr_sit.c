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
 * $RCSfile: fvr_swap.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file fvr_sit.c
 *  FVR sit driver - FVR Driver Interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "fvr.h"

#include "x_os.h"
#include "x_assert.h"
#include "x_hal_arm.h"
#include "x_timer.h"
#include "drv_common.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
//#define FVR_SIT_SUPPORT_MULTITHREAD

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define TS_PACKET_SIZE                       188
#define TS_PACKET_PAYLOAD_SIZE               184
#define PVR_PACKET_SIZE                      192

#define MAX_PACKET_NUM_PER_TIMESLOT		    3

#define MAX_SIT_PACKET_NUM                  45

#define FVR_SIT_QUEUE_NAME                  "FVRSITQ"
#define FVR_SIT_MSGQ_SIZE                   256

//#define FVR_SIT_DEBUG

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum
{
    FVR_SITCMD_KILLTHREAD,
    FVR_SITCMD_SETPATTERN,
    FVR_SITCMD_FREEPATTERN,
    FVR_SITCMD_INSERTPATTERN,
    FVR_SITCMD_NONE
} FVR_SITCMD_T;

typedef enum
{
    FVR_SIT_RESULT_OK = 0,
    FVR_SIT_RESULT_GENERIC_ERROR,
    FVR_SIT_RESULT_INSERT_TIMEOUT
} FVR_SIT_RESULT_T;

typedef struct
{
    BOOL fgValid;
    BOOL fgComplete;
    UINT8 u1Continuity;         // continuity counter
    UINT8 u1PktCount;           // packet count 
    UINT8 u1PatPktIdx;          // current index of SIT buffer
    UINT16 u2Pid;
    UINT32 u4PatBufStart;       // packet buffer carrying the pattern data
} FVR_SIT_PATTERN_T;

typedef struct
{
    FVR_SITCMD_T eSitCmd;
#ifndef FVR_SIT_SUPPORT_MULTITHREAD
    UINT8 u1TsIdx;
#endif
    UINT8 u1PktCnt;
    UINT16 u2Pid;
    UINT32 u4PatAddr;
} FVR_SIT_MSG_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define _FvrSitInitCmd(index)                                                       \
do {                                                                                \
    INT32 i4Ret = x_sema_lock(_hSitCmdDoneSema[index], X_SEMA_OPTION_NOWAIT);       \
    if ((i4Ret != OSR_OK) && (i4Ret != OSR_WOULD_BLOCK))                            \
    {                                                                               \
        LOG(1, "[FVRSIT]: cannot get command done semaphore, TsIdx=%d!\n", index);  \
        fgRet = FALSE;                                                              \
        goto lbExit1;                                                               \
    }                                                                               \
} while (0)

#define _FvrSitWaitCmdDone(index)                                                   \
do {                                                                                \
    INT32 i4Ret = x_sema_lock_timeout(_hSitCmdDoneSema[index], 500);                \
    if (i4Ret != OSR_OK)                                                            \
    {                                                                               \
        if (i4Ret == OSR_TIMEOUT)                                                   \
        {                                                                           \
            LOG(1, "[FVRSIT]: command %u Timeout, TsIdx=%d!\n", eSitCmd, index);    \
            fgRet = FALSE;                                                          \
            goto lbExit1;                                                           \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            ASSERT(0);                                                              \
        }                                                                           \
    }                                                                               \
} while (0)

#define _FvrSitNotifyCmdDone(index)                                                 \
do {                                                                                \
    VERIFY(x_sema_unlock(_hSitCmdDoneSema[index]) == OSR_OK);                       \
} while (0)


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static FVR_SIT_PATTERN_T _arSitPattern[FVR_GBL_COUNT];

static HANDLE_T _hSitCompleteSema[FVR_GBL_COUNT] = {NULL_HANDLE};

static HANDLE_T _hSitCmdThread[FVR_GBL_COUNT] = {NULL_HANDLE};
static HANDLE_T _hSitCmdMsgQ[FVR_GBL_COUNT] = {NULL_HANDLE};
static HANDLE_T _hSitCmdDoneSema[FVR_GBL_COUNT] = {NULL_HANDLE};
static HANDLE_T _hSitCmdTimer[FVR_GBL_COUNT] = {NULL_HANDLE};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _FvrSitResetInsertedPacketCount
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
static void _FvrSitResetInsertedPacketCount(UINT8 u1TsIdx)
{
    UINT32 u4Reg;

    ASSERT(u1TsIdx < FVR_GBL_COUNT);

    _FVR_Lock();
    u4Reg = FVR_GBL_ARY_W(u1TsIdx, 10);
    u4Reg &= ~(0xffu);
    FVR_GBL_ARY_W(u1TsIdx, 10) = u4Reg;
    _FVR_Unlock();
}

//-----------------------------------------------------------------------------
/** _FvrSitDisableInsert
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _FvrSitDisableInsert(UINT8 u1TsIdx)
{
    UINT32 u4Reg;

    ASSERT(u1TsIdx < FVR_GBL_COUNT);

    _FVR_Lock();
    u4Reg = FVR_GBL_BASE_W(FVR_GBL_COMMON_REGION, 11);
    u4Reg &=  ~(0xffu << 16);
    FVR_GBL_BASE_W(FVR_GBL_COMMON_REGION, 11) = u4Reg;

    // when GBL byte 10 (SIT pkt current number) is zero, ucode will stop inserting.
    // Erik: it's a workaround
    u4Reg = FVR_GBL_ARY_W(u1TsIdx, 2);
    u4Reg &= 0xFF00FFFFu;
    FVR_GBL_ARY_W(u1TsIdx, 2) = u4Reg;
    _FVR_Unlock();
    
    // wait for disable
    x_thread_delay(3);
}

//-----------------------------------------------------------------------------
/** _FvrSitFormatPacket
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
static BOOL _FvrSitFormatPacket(UINT8 u1TsIdx, UINT16 u2PidNum, 
                                const UINT8 *pu1Src, UINT16 u2Size,
                                UINT32 *pu4RetBufAddr, UINT8 *pu1RetPktCnt)
{
    UINT32 u4NewSize, u4TSAddr, u4UpdateSize;
    UINT8 *pu1Dst, u1TSCount;
    UINT32 i; 

    if (!pu1Src || !u2Size || !pu4RetBufAddr || !pu1RetPktCnt)
    {
        return FALSE;
    }

    //
    // Caculate TS count and size
    //
    u4NewSize = (UINT32)u2Size;
    u1TSCount = 0;
    if(u4NewSize < (TS_PACKET_PAYLOAD_SIZE - 1))
    {
        u4NewSize = 0;
    }
    else
    {
        u4NewSize -= (TS_PACKET_PAYLOAD_SIZE - 1);      // pointer_field
    }

    u1TSCount = 1;
    if(u4NewSize > 0)
    {
        u1TSCount += (UINT16)
                ((u4NewSize + (TS_PACKET_PAYLOAD_SIZE - 1)) / TS_PACKET_PAYLOAD_SIZE);
    }

    if (u1TSCount > MAX_SIT_PACKET_NUM)
    {
        LOG(2, "[FVRSIT]: Insert too many packets %u\n", u1TSCount);
        return FALSE;
    }

    u4TSAddr = (UINT32)x_mem_alloc((SIZE_T)(u1TSCount * TS_PACKET_SIZE));
    if(u4TSAddr == 0)
    {
        LOG(3, "[FVRSIT]: Allocate buffer fail, TSCount=%d\n", u1TSCount);
        return FALSE;
    }

    //
    // Fill pattern
    //
    u4NewSize = (UINT32)u2Size;
    pu1Dst = (UINT8*)u4TSAddr;

    pu1Dst[0] = 0x47;
    pu1Dst[1] = (UINT8)(0x40 | (UINT8)((u2PidNum >> 8) & 0x1F)); //payload unit start indicator
    pu1Dst[2] = (UINT8)(u2PidNum & 0xFF);
    pu1Dst[3] = 0x10;       // Continuity counter update later , payload only
    pu1Dst[4] = 0;          // pointer_field

    u4UpdateSize = (UINT32)MIN((TS_PACKET_PAYLOAD_SIZE - 1), u4NewSize);
    x_memset((VOID*)(pu1Dst + 5), 0xFF, (TS_PACKET_PAYLOAD_SIZE - 1));
    x_memcpy((VOID*)(pu1Dst + 5), (VOID*)pu1Src, u4UpdateSize);

    pu1Src += u4UpdateSize;
    pu1Dst += (u4UpdateSize + 5); //new 188 byte start
    u4NewSize -= u4UpdateSize;

	//the other packet
    for(i = 1; i < u1TSCount; i++)
    {
        pu1Dst[0] = 0x47;
        pu1Dst[1] = (UINT8)((u2PidNum >> 8) & 0x1F);
        pu1Dst[2] = (UINT8)(u2PidNum & 0xFF);
        pu1Dst[3] = 0x10;       // No adaptation_field, payload only

        u4UpdateSize = (UINT32)MIN(TS_PACKET_PAYLOAD_SIZE, u4NewSize);
        x_memset((VOID*)(pu1Dst + 4), 0xFF, TS_PACKET_PAYLOAD_SIZE);
        x_memcpy((VOID*)(pu1Dst + 4), (VOID*)pu1Src, u4UpdateSize);

        pu1Src += u4UpdateSize;
        pu1Dst += (u4UpdateSize + 4);
        u4NewSize -= u4UpdateSize;
    }

    *pu4RetBufAddr = u4TSAddr;
    *pu1RetPktCnt = u1TSCount;

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _FvrSitWaitInsertComplete
 *  @retval FVR_SIT_RESULT_T
 */
//-----------------------------------------------------------------------------
static FVR_SIT_RESULT_T _FvrSitWaitInsertComplete(UINT8 u1TsIdx)
{
    FVR_SIT_RESULT_T eResult = FVR_SIT_RESULT_OK;
    INT32 i4Ret;
    
    ASSERT(u1TsIdx < FVR_GBL_COUNT);

    i4Ret = x_sema_lock_timeout(_hSitCompleteSema[u1TsIdx], 500);

    if (i4Ret != OSR_OK)
    { 
        if (i4Ret == OSR_TIMEOUT)
        { 
            LOG(1, "[FVRSIT]: wait insert complete, TSIdx=%d!\n", u1TsIdx);
            eResult = FVR_SIT_RESULT_INSERT_TIMEOUT;
            goto lbExit1;
        } 
        else 
        {
            ASSERT(0);
        }
    }

lbExit1:

    return eResult;
}

//-----------------------------------------------------------------------------
/** _FvrSitInsertPattern
 *  @retval FVR_SIT_RESULT_T
 */
//-----------------------------------------------------------------------------
static FVR_SIT_RESULT_T _FvrSitInsertPattern(UINT8 u1TsIdx)
{
    FVR_SIT_RESULT_T eResult = FVR_SIT_RESULT_OK;
    INT32 i4Ret;
    UINT32 u4Reg = 0;
    UINT8 u1PktCount;

    ASSERT(u1TsIdx < FVR_GBL_COUNT);

    if (!_arSitPattern[u1TsIdx].fgValid)
    {
        LOG(1, "[FVRSIT]: Pattern not available, TSIdx=%d\n", u1TsIdx);
        eResult = FVR_SIT_RESULT_GENERIC_ERROR;
        goto lbExit1;
    }

    i4Ret = x_sema_lock(_hSitCompleteSema[u1TsIdx], X_SEMA_OPTION_NOWAIT);
    if ((i4Ret != OSR_OK) && (i4Ret != OSR_WOULD_BLOCK))
    {
        LOG(1, "[FVRSIT]: Cannot get complete semaphore, TSIdx=%d!\n", u1TsIdx);
        eResult = FVR_SIT_RESULT_GENERIC_ERROR;
        goto lbExit1;
    }

    u1PktCount = _arSitPattern[u1TsIdx].u1PktCount;

    _FVR_Lock();
    
    u4Reg = (MAX_PACKET_NUM_PER_TIMESLOT << 24) | ((UINT32)u1PktCount << 8);
    u4Reg |= u1TsIdx;
    FVR_GBL_BASE_W(FVR_GBL_COMMON_REGION, 11) = u4Reg;
   
    _FVR_Unlock();

    // enable insertion 
    _FVR_Lock();

    u4Reg |= 0x10000;
    FVR_GBL_BASE_W(FVR_GBL_COMMON_REGION, 11) = u4Reg;

    _FVR_Unlock();
   
    // wait for insertion
    i4Ret = x_sema_lock_timeout(_hSitCompleteSema[u1TsIdx], 500);
    if (i4Ret != OSR_OK)
    { 
        if (i4Ret == OSR_TIMEOUT)
        { 
            LOG(1, "[FVRSIT]: insert command timeout, TSIdx=%d!\n", u1TsIdx);
            eResult = FVR_SIT_RESULT_INSERT_TIMEOUT;
            goto lbExit1;
        } 
        else 
        {
            ASSERT(0);
        }
    }
    else
    {
        _arSitPattern[u1TsIdx].fgComplete = FALSE;
    }

lbExit1:
    
	return eResult;
}

//-----------------------------------------------------------------------------
/** _FvrSitCmdThread
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
static VOID _FvrSitCmdThread(VOID* pvArg)
{
    FVR_SIT_RESULT_T eResult = FVR_SIT_RESULT_OK;
#ifndef FVR_SIT_SUPPORT_MULTITHREAD
    UINT8 u1TsIdx = 0; 
#else
    UINT8 u1TsIdx = *(UINT8 *)pvArg;
#endif
    UINT8 u1UnfinishedTsIdx = 0;
    BOOL fgRun = TRUE;

    while (fgRun)
    {
        INT32 i4Ret;
        SIZE_T zMsgSize;
        UINT16 u2QueueIndex;
        FVR_SIT_MSG_T rMsg;

        zMsgSize = sizeof (rMsg);
#ifndef FVR_SIT_SUPPORT_MULTITHREAD
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rMsg, &zMsgSize,
            &_hSitCmdMsgQ[0], 1, X_MSGQ_OPTION_WAIT);
#else
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rMsg, &zMsgSize,
            &_hSitCmdMsgQ[u1TsIdx], 1, X_MSGQ_OPTION_WAIT);
#endif

        if (i4Ret != OSR_OK)
        {
            LOG(1, "[FVRSIT]: Error on receiving command, TsIdx=%d!\n", u1TsIdx);
            continue;
        }

        ASSERT(zMsgSize == sizeof (rMsg));
        ASSERT(u2QueueIndex == 0);

#ifndef FVR_SIT_SUPPORT_MULTITHREAD
        u1TsIdx = rMsg.u1TsIdx;
#endif
        
        switch (rMsg.eSitCmd)
        {
        case FVR_SITCMD_KILLTHREAD:
            fgRun = FALSE;
            break;

        case FVR_SITCMD_SETPATTERN:
            if  (rMsg.u2Pid != _arSitPattern[u1TsIdx].u2Pid)
            {
                _arSitPattern[u1TsIdx].u1Continuity = 0;
            }
            
            _arSitPattern[u1TsIdx].u2Pid = rMsg.u2Pid;
            _arSitPattern[u1TsIdx].fgValid = TRUE;
            _arSitPattern[u1TsIdx].fgComplete = FALSE;
            _arSitPattern[u1TsIdx].u1PktCount = rMsg.u1PktCnt;
            _arSitPattern[u1TsIdx].u1PatPktIdx = 0;
            _arSitPattern[u1TsIdx].u4PatBufStart = rMsg.u4PatAddr;
            break;

        case FVR_SITCMD_FREEPATTERN:
            _FvrSitDisableInsert(u1TsIdx);
            _FvrSitResetInsertedPacketCount(u1TsIdx);
            x_mem_free((VOID*)_arSitPattern[u1TsIdx].u4PatBufStart);
            
            _arSitPattern[u1TsIdx].fgValid = FALSE;
            _arSitPattern[u1TsIdx].fgComplete = FALSE;
            _arSitPattern[u1TsIdx].u1PktCount = 0;
            _arSitPattern[u1TsIdx].u1PatPktIdx = 0;
            _arSitPattern[u1TsIdx].u4PatBufStart = rMsg.u4PatAddr;
            break;

        case FVR_SITCMD_INSERTPATTERN:
            if (eResult == FVR_SIT_RESULT_INSERT_TIMEOUT)
            {
                LOG(1, "[FVRSIT]: ignore TS%d insert due to TS%d timeout!\n", u1TsIdx, u1UnfinishedTsIdx);
                eResult = _FvrSitWaitInsertComplete(u1UnfinishedTsIdx);
            }
            else
            {
                eResult = _FvrSitInsertPattern(u1TsIdx);
                if (eResult == FVR_SIT_RESULT_INSERT_TIMEOUT)
                {
                    u1UnfinishedTsIdx = u1TsIdx;
                }
            }
            break;

        default:
            // Unknown message type
            ASSERT(FALSE);
            break;
        }
        
        _FvrSitNotifyCmdDone(u1TsIdx);
    }

    x_thread_exit();
}

#ifndef FVR_SIT_SUPPORT_MULTITHREAD
static BOOL _FvrSitInitResources(void)
{
    CHAR ucName[64];
    UINT8 u1TsIdx = 0;

    if (!_hSitCompleteSema[u1TsIdx])
    {
        if (OSR_OK != x_sema_create(&_hSitCompleteSema[u1TsIdx], 
                                    X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK))
        {
            LOG(1, "[FVRSIT]: Fail to create complete sema, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }

    if (!_hSitCmdDoneSema[u1TsIdx])
    {
        if (OSR_OK != x_sema_create(&_hSitCmdDoneSema[u1TsIdx], 
                                    X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK))
        {
            LOG(1, "[FVRSIT]: Fail to create CmdDone sema, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }

    if (!_hSitCmdMsgQ[u1TsIdx])
    {
        x_snprintf(ucName, sizeof(ucName), "%s_%u", FVR_SIT_QUEUE_NAME, u1TsIdx);

        if (OSR_OK != x_msg_q_create(&_hSitCmdMsgQ[u1TsIdx], ucName,
            sizeof(FVR_SIT_MSG_T), FVR_SIT_MSGQ_SIZE))
        {
            LOG(1, "[FVRSIT]: Fail to create CmdMsgQ, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }

    if (!_hSitCmdThread[u1TsIdx])
    {
        x_snprintf(ucName, sizeof(ucName), "%s_%u", DMX_FVR_SIT_THREAD_NAME, u1TsIdx);
 
        if (OSR_OK != x_thread_create(&_hSitCmdThread[u1TsIdx], ucName, DMX_FVR_SIT_THREAD_STACK_SIZE, 
                    DMX_FVR_SIT_THREAD_PRIORITY, _FvrSitCmdThread, sizeof(u1TsIdx), (void*)&u1TsIdx))
        {
            LOG(1, "[FVRSIT]: Fail to create CmdThread, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }
    
    return TRUE;

lbExit1:
    
    if (_hSitCompleteSema[u1TsIdx])
    {
        VERIFY(x_sema_delete(_hSitCompleteSema[u1TsIdx]) == OSR_OK);
        _hSitCompleteSema[u1TsIdx] = NULL_HANDLE;
    }
    
    if (_hSitCmdDoneSema[u1TsIdx])
    {
        VERIFY(x_sema_delete(_hSitCmdDoneSema[u1TsIdx]) == OSR_OK);
        _hSitCmdDoneSema[u1TsIdx] = NULL_HANDLE;
    }

    if (_hSitCmdMsgQ[u1TsIdx])
    {
        VERIFY(x_msg_q_delete(_hSitCmdMsgQ[u1TsIdx]) == OSR_OK);
        _hSitCmdMsgQ[u1TsIdx] = NULL_HANDLE;
    }

    return FALSE;  
}

static BOOL _FvrSitCreateResources(UINT8 u1TsIdx)
{
    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        LOG(1, "[FVRSIT]: Fail to create resources, TsIdx=%d!\n", u1TsIdx);
        return FALSE;
    }
    
    _hSitCompleteSema[u1TsIdx] = _hSitCompleteSema[0];
    _hSitCmdDoneSema[u1TsIdx] = _hSitCmdDoneSema[0];
    _hSitCmdMsgQ[u1TsIdx] = _hSitCmdMsgQ[0];
    _hSitCmdThread[u1TsIdx] = _hSitCmdThread[0];

    return TRUE;
}
#else
static BOOL _FvrSitCreateResources(UINT8 u1TsIdx)
{
    CHAR ucName[64];

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        LOG(1, "[FVRSIT]: Fail to create resources, TsIdx=%d!\n", u1TsIdx);
        goto lbExit1;
    }

    if (!_hSitCompleteSema[u1TsIdx])
    {
        if (OSR_OK != x_sema_create(&_hSitCompleteSema[u1TsIdx], 
                                    X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK))
        {
            LOG(1, "[FVRSIT]: Fail to create complete sema, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }

    if (!_hSitCmdDoneSema[u1TsIdx])
    {
        if (OSR_OK != x_sema_create(&_hSitCmdDoneSema[u1TsIdx], 
                                    X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK))
        {
            LOG(1, "[FVRSIT]: Fail to create CmdDone sema, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }

    if (!_hSitCmdMsgQ[u1TsIdx])
    {
        x_snprintf(ucName, sizeof(ucName), "%s_%u", FVR_SIT_QUEUE_NAME, u1TsIdx);

        if (OSR_OK != x_msg_q_create(&_hSitCmdMsgQ[u1TsIdx], ucName,
            sizeof(FVR_SIT_MSG_T), FVR_SIT_MSGQ_SIZE))
        {
            LOG(1, "[FVRSIT]: Fail to create CmdMsgQ, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }

    if (!_hSitCmdThread[u1TsIdx])
    {
        x_snprintf(ucName, sizeof(ucName), "%s_%u", DMX_FVR_SIT_THREAD_NAME, u1TsIdx);
 
        if (OSR_OK != x_thread_create(&_hSitCmdThread[u1TsIdx], ucName, DMX_FVR_SIT_THREAD_STACK_SIZE, 
                    DMX_FVR_SIT_THREAD_PRIORITY, _FvrSitCmdThread, sizeof(u1TsIdx), (void*)&u1TsIdx))
        {
            LOG(1, "[FVRSIT]: Fail to create CmdThread, TsIdx=%d!\n", u1TsIdx);
            goto lbExit1;
        }
    }
    
    return TRUE;

lbExit1:
    
    if (_hSitCompleteSema[u1TsIdx])
    {
        VERIFY(x_sema_delete(_hSitCompleteSema[u1TsIdx]) == OSR_OK);
        _hSitCompleteSema[u1TsIdx] = NULL_HANDLE;
    }
    
    if (_hSitCmdDoneSema[u1TsIdx])
    {
        VERIFY(x_sema_delete(_hSitCmdDoneSema[u1TsIdx]) == OSR_OK);
        _hSitCmdDoneSema[u1TsIdx] = NULL_HANDLE;
    }

    if (_hSitCmdMsgQ[u1TsIdx])
    {
        VERIFY(x_msg_q_delete(_hSitCmdMsgQ[u1TsIdx]) == OSR_OK);
        _hSitCmdMsgQ[u1TsIdx] = NULL_HANDLE;
    }

    return FALSE;  
}
#endif

//-----------------------------------------------------------------------------
/** _FvrSitCmdThread
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
static VOID _FvrSitTimeout(HANDLE_T pt_tm_handle, VOID *pv_tag)
{
    UINT8 u1TsIdx;
    
    u1TsIdx = (UINT8)(UINT32)pv_tag;

    if (!_FVR_SitInsertPattern(u1TsIdx, FALSE))
    {
        //LOG(2, "[FVRSIT]: Fail to insert packet in timer, TsIdx=%d!\n", u1TsIdx);
    }

    UNUSED(pt_tm_handle);
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** _FVR_ReplaceDITPacket
 *  replace DIT packet
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_ReplaceDITPacket(UINT8 u1Pidx, UINT32 u4PktIdx)
{
    UINT8 au1DitPkt[2][188] = {
    { 0x47, 0x00, 0x1E, 0x2F, 0xB7, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    
    { 0x47, 0x40, 0x1E, 0x10, 0x00, 0x7E, 0x70, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};

    static UINT8 u1Continuity = 0xF;
    UINT8 u1TsIdx;
    UINT32 u4BufStart, u4BufEnd;
    UINT32 u4DitAddr, u4PktOffset, u4TotalPktCnt;
    FVR_PID_T rPid;
    FVR_GBL_T rGBL;
    

    if (!_FVR_GetPid(u1Pidx, FVR_PID_FLAG_TSINDEX, &rPid))
    {
        return FALSE;
    }
   
    u1TsIdx = rPid.u1TsIndex;
    
    // calculate address
    if(!_FVR_GetGBL(u1TsIdx,FVR_GBL_FLAG_BUFFER, &rGBL))
    {
        return FALSE;
    }
  
    u4BufStart = rGBL.u4BufStart;
    u4BufEnd = rGBL.u4BufStart + rGBL.u4BufSize;
    u4TotalPktCnt = rGBL.u4BufSize / 192;

    u4PktIdx -= 3;  
    u4PktOffset = (u4PktIdx % u4TotalPktCnt) * 192;
    u4DitAddr = u4BufStart + u4PktOffset;
    
    // modify continuity
    au1DitPkt[0][3] &= 0xF0;
    au1DitPkt[0][3] |= u1Continuity;

    u1Continuity = (u1Continuity + 1) & 0xf;

    au1DitPkt[1][3] &= 0xF0;
    au1DitPkt[1][3] |= u1Continuity;

    // put DIT packet
    x_memcpy(((UINT8 *)u4DitAddr + 4), au1DitPkt[0], 188);
    HalFlushDCacheMultipleLine(u4DitAddr, 188);
    
    u4DitAddr += 192;
    if (u4DitAddr >= u4BufEnd)
    {
        u4DitAddr = u4BufStart;
    }
    
    x_memcpy(((UINT8 *)u4DitAddr + 4), au1DitPkt[1], 188);
    HalFlushDCacheMultipleLine(u4DitAddr, 188);

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _FVR_SitQueryInsertedPacketCount
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
UINT32 _FVR_SitQueryInsertedPacketCount(UINT8 u1TsIdx)
{
    UINT32 u4PktCnt;

    _FVR_Lock();
    u4PktCnt = FVR_GBL_ARY_W(u1TsIdx, 10) & 0xffu;
    _FVR_Unlock();

    return u4PktCnt;
}

//-----------------------------------------------------------------------------
/** _FVR_SitReplacePacket
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitReplacePacket(UINT8 u1TsIdx, UINT8 u1SitPktCnt, BOOL fgComplete)
{
    FVR_GBL_T rGBL;
    UINT32 u4BufStart, u4BufEnd;
	UINT32 u4RecordAddr, u4SwapAddr, u4ConAddr;
    UINT32 u4CpLen1, u4CpLen2;
    UINT8 u1PktCount, u1PatPktIdx, u1SitPktIdx;
    UINT8 u1Continuity, u1Tmp, i;
    BOOL fgRet = TRUE;

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        LOG(2, "[FVRSIT]: Fail to replace packet, TsIdx=%d\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }

	// Check enable
    if(!_arSitPattern[u1TsIdx].fgValid)
    {
        LOG(2, "[FVRSIT]: Fail to replace packet, TSIdx=%d\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }

    // unlock complete sema here if receive complete int and no more packets
    if (fgComplete && _arSitPattern[u1TsIdx].fgComplete)
    {
        VERIFY(x_sema_unlock(_hSitCompleteSema[u1TsIdx]) == OSR_OK);
        fgRet = TRUE;
        goto lbExit1;
    }
   
    u1SitPktIdx = u1SitPktCnt - 1; 
    u1PatPktIdx = _arSitPattern[u1TsIdx].u1PatPktIdx;
    if (u1SitPktIdx < u1PatPktIdx)
    {
        fgRet = TRUE;
        goto lbExit1;
    }

    u1PktCount = _arSitPattern[u1TsIdx].u1PktCount;
    if (u1SitPktCnt > u1PktCount)
    {
        LOG(2, "[FVRSIT]: ucode gives invalid packet count %u, wanted %u\n",
            u1SitPktCnt, u1PktCount);
        fgRet = FALSE;
        goto lbExit1;
    }
	
    //prepare sit, recovery some variable
	u1Continuity = _arSitPattern[u1TsIdx].u1Continuity;
    u4SwapAddr = _arSitPattern[u1TsIdx].u4PatBufStart + (TS_PACKET_SIZE * u1PatPktIdx);
    
	//Get record buffer u4BufStart and u4BufEnd
    if(!_FVR_GetGBL(u1TsIdx, FVR_GBL_FLAG_BUFFER, &rGBL))
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    u4BufStart = rGBL.u4BufStart;
    u4BufEnd = u4BufStart + rGBL.u4BufSize;

    //begin to sit for each u1SitPkt
	for (i = u1PatPktIdx; i <= u1SitPktIdx; i++)
	{
		//recovery sit RP, and check buffer range
		u4RecordAddr = VIRTUAL(FVR_SIT_ARY_W(u1TsIdx, i));

		if((u4RecordAddr < u4BufStart) || (u4RecordAddr >= u4BufEnd))
	    {
	        LOG(2, "[FVRSIT]: RecordAddr: 0x%x out of range\n", u4RecordAddr);
            fgRet = FALSE;
            break;
	    }

		//forward one ts packet, seek to ts packet head and prepare swap 
		u4RecordAddr -= TS_PACKET_SIZE;
	    if(u4RecordAddr < u4BufStart)
	    {
	        u4RecordAddr += rGBL.u4BufSize;
	    }

		//Need to handle addr wrap    
		if((u4RecordAddr + TS_PACKET_SIZE) >= u4BufEnd)
		{
			u4CpLen1 = u4BufEnd - u4RecordAddr;
			u4CpLen2 = TS_PACKET_SIZE - u4CpLen1;
		}
		else
		{
			u4CpLen1 = TS_PACKET_SIZE;
			u4CpLen2 = 0;
		}

	    //deal with continutity counter
		u4ConAddr = u4SwapAddr + 3;

	    u1Tmp = *(UINT8*)u4ConAddr;
	    u1Tmp &= 0xF0;
	    u1Tmp |= u1Continuity;
	    *(UINT8*)u4ConAddr = u1Tmp;

		//update u1Continuity , ts index  sitbufRp and swap addr
		u1Continuity++;
    	u1Continuity %= 16;

		//real swap
		x_memcpy((VOID*)u4RecordAddr, (VOID*)u4SwapAddr, u4CpLen1);
        HalFlushDCacheMultipleLine(u4RecordAddr, u4CpLen1);

	    if(u4CpLen2 > 0)
	    {
	        x_memcpy((VOID*)u4BufStart, (VOID*)(u4SwapAddr + u4CpLen1), u4CpLen2);
            HalFlushDCacheMultipleLine(u4BufStart, u4CpLen2);
	    }

		u4SwapAddr +=  TS_PACKET_SIZE;
	}
	
	//save to global variable
	_arSitPattern[u1TsIdx].u1Continuity = u1Continuity;
    _arSitPattern[u1TsIdx].u1PatPktIdx = i;
    if (i == _arSitPattern[u1TsIdx].u1PktCount)
    {
        _FvrSitResetInsertedPacketCount(u1TsIdx);
        _arSitPattern[u1TsIdx].fgComplete = TRUE;
        _arSitPattern[u1TsIdx].u1PatPktIdx = 0;
        if (fgComplete)
        {
            VERIFY(x_sema_unlock(_hSitCompleteSema[u1TsIdx]) == OSR_OK);
        }
    }

lbExit1:

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _FVR_SitSetPattern
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitSetPattern(UINT8 u1TsIdx, UINT16 u2Pid, const UINT8 *pu1Data, UINT16 u2Len)
{
    UINT32 u4RetBufAddr;
    UINT8 u1RetPktCnt;
    FVR_SIT_MSG_T rMsg;
    BOOL fgRet = TRUE;
    FVR_SITCMD_T eSitCmd = FVR_SITCMD_SETPATTERN;

    if ((u1TsIdx >= FVR_GBL_COUNT) || !pu1Data)
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    if (!_FvrSitCreateResources(u1TsIdx))
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    // semaphore protect in fvr_if.c
    if (_arSitPattern[u1TsIdx].fgValid)
    {
        LOG(3, "[FVRSIT]: Free existed pattern before setting, TsIdx=%d!\n", u1TsIdx);
        if (!_FVR_SitFreePattern(u1TsIdx))
        {
            fgRet = FALSE;
            goto lbExit1;
        }
    }

    _FvrSitInitCmd(u1TsIdx);
    
    if(!_FvrSitFormatPacket(u1TsIdx, u2Pid, pu1Data, u2Len, &u4RetBufAddr, &u1RetPktCnt))
    {
        LOG(2, "[FVRSIT]: Format packet error, TsIdx=%d\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }

#ifndef FVR_SIT_SUPPORT_MULTITHREAD
    rMsg.u1TsIdx = u1TsIdx;
#endif

    rMsg.eSitCmd = eSitCmd;
    rMsg.u2Pid = u2Pid;
    rMsg.u1PktCnt = u1RetPktCnt;
    rMsg.u4PatAddr = u4RetBufAddr;

    if (x_msg_q_send(_hSitCmdMsgQ[u1TsIdx], (VOID*)&rMsg, sizeof (FVR_SIT_MSG_T),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        LOG(2, "[FVRSIT]: Fail to set pattern due to cmdQ full, TsIdx=%d!\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }
    
    _FvrSitWaitCmdDone(u1TsIdx);

lbExit1:
 
	return fgRet;
}

//-----------------------------------------------------------------------------
/** _FVR_SitFreePattern
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitFreePattern(UINT8 u1TsIdx)
{
    FVR_SIT_MSG_T rMsg;
    BOOL fgRet = TRUE;
    FVR_SITCMD_T eSitCmd = FVR_SITCMD_FREEPATTERN;

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    // semaphore protect in fvr_if.c
    if (!_arSitPattern[u1TsIdx].fgValid)
    {
        LOG(2, "[FVRSIT]: Fail to free invalid pattern, TsIdx=%d\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }

    _FvrSitInitCmd(u1TsIdx);

#ifndef FVR_SIT_SUPPORT_MULTITHREAD
    rMsg.u1TsIdx = u1TsIdx;
#endif
    rMsg.eSitCmd = eSitCmd;

    if (x_msg_q_send(_hSitCmdMsgQ[u1TsIdx], (VOID*)&rMsg, sizeof (FVR_SIT_MSG_T),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        LOG(2, "[FVRSIT]: Fail to free pattern due to cmdQ full, TsIdx=%d!\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }
    
    _FvrSitWaitCmdDone(u1TsIdx);

lbExit1: 
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _FVR_SitInsertPattern
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitInsertPattern(UINT8 u1TsIdx, BOOL fgSyncCmd)
{
    FVR_SIT_MSG_T rMsg;
    BOOL fgRet = TRUE;
    FVR_SITCMD_T eSitCmd = FVR_SITCMD_INSERTPATTERN;

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    if (fgSyncCmd)
    {
        _FvrSitInitCmd(u1TsIdx);
    }

#ifndef FVR_SIT_SUPPORT_MULTITHREAD
    rMsg.u1TsIdx = u1TsIdx;
#endif

    rMsg.eSitCmd = eSitCmd;

    if (x_msg_q_send(_hSitCmdMsgQ[u1TsIdx], (VOID*)&rMsg, sizeof (FVR_SIT_MSG_T),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        LOG(2, "[FVRSIT]: Fail to insert pattern due to cmdQ full, TsIdx=%d!\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }
    
    if (fgSyncCmd)
    {
        _FvrSitWaitCmdDone(u1TsIdx);
    }

lbExit1: 
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _FVR_SitInsertPatternByTimer
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitInsertPatternByTimer(UINT8 u1TsIdx, UINT32 u4MsDelay)
{
    BOOL fgRet = TRUE;

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    if (!_hSitCmdTimer[u1TsIdx])
    {
        if (OSR_OK != x_timer_create(&_hSitCmdTimer[u1TsIdx]))
        {
            LOG(2, "[FVRSIT]: Fail to create timer, TsIdx=%u, Delay=%u\n", u1TsIdx, u4MsDelay);
            fgRet = FALSE;
            goto lbExit1;
        }
    }
    
    if (OSR_OK != x_timer_start(_hSitCmdTimer[u1TsIdx], u4MsDelay, X_TIMER_FLAG_REPEAT,
                                _FvrSitTimeout, (void*)(UINT32)u1TsIdx))
    {
        LOG(2, "[FVRSIT]: Fail to start timer, TsIdx=%u, Delay=%u\n", u1TsIdx, u4MsDelay);
        fgRet = FALSE;
        goto lbExit1;
    }

lbExit1:
    
    return fgRet;
}

 //-----------------------------------------------------------------------------
/** _FVR_SitStopTimer
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitStopTimer(UINT8 u1TsIdx)
{
    BOOL fgRet = TRUE;

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        fgRet = FALSE;
        goto lbExit1;
    }

    if (OSR_OK != x_timer_stop(_hSitCmdTimer[u1TsIdx]))
    {
        LOG(2, "[FVRSIT]: Fail to start timer, TsIdx=%u\n", u1TsIdx);
        fgRet = FALSE;
        goto lbExit1;
    }

lbExit1:
        
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _FVR_SitInit
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _FVR_SitInit(void)
{
    INT32 i;
    static BOOL fgInit = FALSE;

#ifndef FVR_SIT_SUPPORT_MULTITHREAD
    if (!_FvrSitInitResources())
    {
        return FALSE;
    }
#endif

    for (i = 0; i < FVR_GBL_COUNT; i++)
    {
        _FvrSitResetInsertedPacketCount(i);

        // for error handling 
        if (fgInit & _arSitPattern[i].u4PatBufStart)
        {
            x_mem_free((VOID*)_arSitPattern[i].u4PatBufStart);
        }
        
        x_memset((VOID*)&_arSitPattern[i], 0, sizeof(FVR_SIT_PATTERN_T));
    }

    //init ucode wp
    FVR_GBL_ARY_W(0, 11) = (FVR_GBL_ARY_W(0, 11) & (~0xFFFF)) | FVR_SIT_BUFFER_OFFSET;
    FVR_GBL_ARY_W(1, 11) = (FVR_GBL_ARY_W(1, 11) & (~0xFFFF)) | FVR_SIT_BUFFER_OFFSET1;
    FVR_GBL_ARY_W(2, 11) = (FVR_GBL_ARY_W(2, 11) & (~0xFFFF)) | FVR_SIT_BUFFER_OFFSET2;
    FVR_GBL_ARY_W(3, 11) = (FVR_GBL_ARY_W(3, 11) & (~0xFFFF)) | FVR_SIT_BUFFER_OFFSET3;

    fgInit = TRUE;

    return TRUE;
}


