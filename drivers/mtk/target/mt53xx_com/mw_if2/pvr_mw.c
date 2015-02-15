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
 * $RCSfile: pvr_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_mw.c
 *  PVR driver - middleware interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "mtdmx_mwif.h"
#include "dmx_mw.h"

#include "dmx_if.h"
#include "fvr_if.h"
//#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "drv_comp_id.h"
#include "x_assert.h"
#include "x_hal_5381.h"
#include "x_pvr.h"
#include "x_rm_dev_types.h"
#include "x_os.h"
#include "x_mm_common.h"
#include "x_debug.h"

LINT_EXT_HEADER_END

#undef LOG
#define LOG(a, ...)

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MW_FVR_CTRL_NUM                   (2)

#define MW_FVR_META_BUF_NUM               (5)

#define PVRMW_TICK_PIC_MAX                (128)

#define PVRMW_FIFO_ALIGNMENT              (64)

#define PVRMW_BUFFER_SIZE                 (0x300000)

#define PVRMW_TICK_METABUF_SIZE           (8 * 1024)//(MW_FVR_META_BUF_NUM * PVRMW_TICK_PIC_MAX * 4)

#define PVRMW_THREAD_NAME                 "PVRMWT"
#define PVRMW_THREAD_STACK_SIZE           1024
#define PVRMW_THREAD_PRIORITY             100

#define PVRMW_QUEUE_NAME                  "PVRISRQ"
#define PVRMW_QUEUE_SIZE                  32

#define PVRMW_PRIORITY_NORMAL             128

#ifdef CC_DUAL_TUNER_SUPPORT
#define MW_FVR_INPUT_NUM                  (2)
#else
#define MW_FVR_INPUT_NUM                  (1)
#endif


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    BOOL fgValid;
    UINT32 u4SerialNum;
    UINT32 u4BufAddr;
} MWPVR_META_BUF_T;

typedef struct
{
    BOOL fgEnable;
    UINT8 u1Pidx;
} MWPVR_PIDX_USAGE_T;

typedef enum
{
    MWPVRCMD_TIME,
    MWPVRCMD_OVERFLOW,
    MWPVRCMD_SCRAMBLE
} MWPVRCMDTYPE;

typedef struct
{
    MWPVRCMDTYPE eCmd;
    PVR_TICK_DATA_T rTickData;
    UINT32 u4Data1;
    UINT8 u1Pidx;
} MWPVR_MSG_T;

typedef struct
{
    BOOL fgValid;
    UINT32 u4TsIdx;
} MWPVR_FVR_INPUT_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Import functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static PVR_NFY_INFO_T _arMwPVRNotifyInfo[MW_FVR_INPUT_NUM];

static BOOL _afgIsOverflow[MW_FVR_INPUT_NUM] = {};
static UINT32 _au4MwPVRTickNum[MW_FVR_INPUT_NUM] = {};
static UINT32 _au4MwPVRPktCount[MW_FVR_INPUT_NUM] = {};
static UINT32 _au4MwPVRPicIdx[MW_FVR_INPUT_NUM] = {};
static UINT32 *_apu4MwPVRPicData[MW_FVR_INPUT_NUM] = {};


static MWPVR_META_BUF_T _arMwPVRMetaBuf[MW_FVR_INPUT_NUM][MW_FVR_META_BUF_NUM];


static CRIT_STATE_T _rMwPvrLock;
static BOOL _fgMwPvrLocking = FALSE;

static BOOL _afgMwPvrEnable[MW_FVR_INPUT_NUM] = {};


static MWPVR_PIDX_USAGE_T _arMwPvrPerPidxUsage[FVR_SCRAMBLE_PID_INDEX];

static UINT32 _au4MwPvrStreamPhyAddr[MW_FVR_INPUT_NUM];
static UINT32 _au4MwPvrMetaPhyAddr[MW_FVR_INPUT_NUM];
static UINT32 _au4MwPvrStreamMwAddr[MW_FVR_INPUT_NUM];
static UINT32 _au4MwPvrKelTotalSize[MW_FVR_INPUT_NUM];

static MW_DMX_PID_INFO_T _arFvrPidInfo[MTDMX_NUM_FVR_PID];

static MWPVR_FVR_INPUT_T _arFvrInput[MW_FVR_INPUT_NUM];

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

#ifdef LINUX_TURNKEY_SOLUTION
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
UINT32 _MwPvrKerlToUserAddr(UINT8 u1InputId, UINT32 u4KerAddr)
{
    UNUSED(_au4MwPvrKelTotalSize[u1InputId]);
    ASSERT(u4KerAddr >= _au4MwPvrStreamPhyAddr[u1InputId]);
    ASSERT(u4KerAddr < (_au4MwPvrStreamPhyAddr[u1InputId] + _au4MwPvrKelTotalSize[u1InputId]));

    return (u4KerAddr - _au4MwPvrStreamPhyAddr[u1InputId] + _au4MwPvrStreamMwAddr[u1InputId]);
}

UINT32 _MwPvrUserToKerlAddr(UINT8 u1InputId, UINT32 u4UserAddr)
{
    ASSERT(u4UserAddr >= _au4MwPvrStreamMwAddr[u1InputId]);
    ASSERT(u4UserAddr < (_au4MwPvrStreamMwAddr[u1InputId] + _au4MwPvrKelTotalSize[u1InputId]));

    return u4UserAddr - _au4MwPvrStreamMwAddr[u1InputId] + _au4MwPvrStreamPhyAddr[u1InputId];
}


#endif // LINUX_TURNKEY_SOLUTION

//-----------------------------------------------------------------------------
/** _MwPvrLock
 *  Enter demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _MwPvrLock(void)
{
    _rMwPvrLock = x_crit_start();

    ASSERT(!_fgMwPvrLocking);
    _fgMwPvrLocking = TRUE;

    UNUSED(_fgMwPvrLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
/** _MwPvrUnlock
 *  Leave demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _MwPvrUnlock(void)
{
    ASSERT(_fgMwPvrLocking);
    _fgMwPvrLocking = FALSE;

    x_crit_end(_rMwPvrLock);

    UNUSED(_fgMwPvrLocking);          // Make Lint happy
}

static UINT8 _MWPvrGetInputIdByTsIdx(UINT8 u1TsIdx)
{
    UINT8 i;
    for(i=0; i<MW_FVR_INPUT_NUM;i++)
    {
        if(((UINT8)_arFvrInput[i].u4TsIdx) == u1TsIdx)
        {
            return i;
        }
    }
    return MW_FVR_INPUT_NUM; //Invalid input id index
}

//-----------------------------------------------------------------------------
/** _MW_FVR_ConnectInfo
 */
//-----------------------------------------------------------------------------
BOOL _MW_FVR_ConnectInfo(UINT8 u1Pidx, UINT32 u4RMType, DRV_TYPE_T eType , UINT16 u2Id)
{
    MW_DMX_PID_CONTYPE_T eConType;
    DMX_MW_PVRPID_INFO_T *prPidInfo;

    if(u1Pidx >= MTDMX_NUM_FVR_PID)
    {
        LOG(1, "Set the wrong PID index!\n");
        return FALSE;
    }

    _MwPvrLock();
    ASSERT(!_arFvrPidInfo[u1Pidx].fgValid);
    _MwPvrUnlock();

    prPidInfo = _MW_Rec_GetPidInfo(u1Pidx);
    if(prPidInfo == NULL)
    {
        LOG(1, "%s error\n", __FUNCTION__);
        return FALSE;
    }
    eConType = prPidInfo->eConType;

    _MwPvrLock();
    _arFvrPidInfo[u1Pidx].fgValid = TRUE;
    _arFvrPidInfo[u1Pidx].u4RMType = u4RMType;
    _arFvrPidInfo[u1Pidx].eConType = eConType;
    _arFvrPidInfo[u1Pidx].u1TsIndex = _MwDmxContypeToTsIdx(eConType);
    _MwPvrUnlock();

    LOG(5, "[_MW_FVR_ConnectInfo] ConnectInfo, VIRPidx: %d, Type: %d.\n",u1Pidx, eConType);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MW_FVR_IsPidConnedted
 */
//-----------------------------------------------------------------------------
BOOL _MW_FVR_IsPidConnedted(UINT8 u1Pidx)
{
    BOOL fgValid;

    if(u1Pidx >= MTDMX_NUM_FVR_PID)
    {
        return FALSE;
    }

    _MwPvrLock();
    fgValid = _arFvrPidInfo[u1Pidx].fgValid;
    _MwPvrUnlock();

    return fgValid;
}


//-----------------------------------------------------------------------------
/** _MW_FVR_DisConnectInfo
 */
//-----------------------------------------------------------------------------
BOOL _MW_FVR_DisConnectInfo(UINT8 u1Pidx)
{
    if(u1Pidx >= MTDMX_NUM_FVR_PID)
    {
        return FALSE;
    }

    _MwPvrLock();
    _arFvrPidInfo[u1Pidx].fgValid = FALSE;
    _arFvrPidInfo[u1Pidx].fgCached = FALSE;
    _arFvrPidInfo[u1Pidx].u4RMType = 0;
    _arFvrPidInfo[u1Pidx].eConType = MW_DMX_CONN_TYPE_NONE;
    _arFvrPidInfo[u1Pidx].u1TsIndex = 0;  /*set default to 0*/
    _MwPvrUnlock();

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwPvrAllocPerPidx
 */
//-----------------------------------------------------------------------------
static UINT8 _MwPvrAllocPerPidx(UINT8 u1Pidx)
{
    UINT8 i;

   
    for(i=0; i<FVR_SCRAMBLE_PID_INDEX; i++)
    {
        if(u1Pidx != 0xFF)
        {
            if(_arMwPvrPerPidxUsage[i].fgEnable && (u1Pidx == _arMwPvrPerPidxUsage[i].u1Pidx))
            {
                break;
            }
        }
    }

    if(i == FVR_SCRAMBLE_PID_INDEX)
    {
        for(i=0; i<FVR_SCRAMBLE_PID_INDEX; i++)
        {
            if(!_arMwPvrPerPidxUsage[i].fgEnable)
            {
                break;
            }
        }
    }

    if(i == FVR_SCRAMBLE_PID_INDEX)
    {
        return 0xFF;
    }

    _arMwPvrPerPidxUsage[i].fgEnable = TRUE;
    _arMwPvrPerPidxUsage[i].u1Pidx = u1Pidx;

    return i;
}


//-----------------------------------------------------------------------------
/** _MwPvrSetPid
 */
//-----------------------------------------------------------------------------
static BOOL _MwPvrSetPid(UINT8 u1Pidx, UINT32 u4Flags, MTFVR_PID_T* prPid)
{
    #ifdef CI_PLUS_SUPPORT
    UINT8 u1TsIdx;
	#endif 
	
    if(prPid == NULL)
    {
        return FALSE;
    }
	
    #ifdef CI_PLUS_SUPPORT
    u1TsIdx = _arFvrPidInfo[u1Pidx].u1TsIndex;
    #endif 
	
    if(MTFVR_SetPid(u1Pidx, u4Flags, prPid) != MTR_OK)
    {
        return FALSE;
    }

    #ifdef CI_PLUS_SUPPORT
    // Enable FVR descramble mode if neccessary
    if(((u4Flags & FVR_PID_FLAG_ENABLE) != 0) && (prPid->fgEnable == TRUE))
    {
        if(!_MwDmxTraverseFvrDesc(u1TsIdx))
        {
            return FALSE;
        }
    }
    #endif

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwPVRFreeMetaBuf
 */
//-----------------------------------------------------------------------------
static BOOL _MwPVRFreeMetaBuf(UINT8 u1InputId, UINT8 u1Idx)
{
    if(u1Idx >= MW_FVR_META_BUF_NUM)
    {
        return FALSE;
    }

    _arMwPVRMetaBuf[u1InputId][u1Idx].fgValid = FALSE;

    return TRUE;
}



//-----------------------------------------------------------------------------
/** _MwPVRAllocMetaBuf
 */
//-----------------------------------------------------------------------------
static UINT8 _MwPVRAllocMetaBuf(UINT8 u1InputId)
{
    UINT8 i;
#if 0       // debug purpose
    UINT8 u1Count = 0;

    for(i=0; i<MW_FVR_META_BUF_NUM; i++)
    {
        if(!_arMwPVRMetaBuf[i].fgValid)
        {
            u1Count++;
        }
    }
    LOG(1, "Available meta buf: %d\n", u1Count);
#endif

    for(i=0; i<MW_FVR_META_BUF_NUM; i++)
    {
         if(!_arMwPVRMetaBuf[u1InputId][i].fgValid)
        {
            break;
        }
    }

    if(i == MW_FVR_META_BUF_NUM)
    {
        return 0xFF;
    }

    return i;
}


//-----------------------------------------------------------------------------
/** _MwPVRPicHandler
 */
//-----------------------------------------------------------------------------
static void _MwPVRPicHandler(UINT8 u1Pidx, UINT8 u1Type, UINT32 u4PktIdx,
                                     const void* pvTag)
{
    MTFVR_VIDEO_TYPE_T eVideoType = MTFVR_VIDEO_NONE;
    UINT32 u4PicIdx;
    UINT8 u1Pattern, u1TsIdx, u1InputId;

    // printf("%s 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, u1Pidx, u1Type, u4PktIdx, pvTag);
    u1TsIdx = _arFvrPidInfo[u1Pidx].u1TsIndex;
    u1InputId = _MWPvrGetInputIdByTsIdx(u1TsIdx);

    ASSERT(u1InputId < MW_FVR_INPUT_NUM);
    ASSERT(_apu4MwPVRPicData[u1InputId] != NULL);

    if(MTFVR_GetVideoType(0, &eVideoType) != MTR_OK)
    {
        return;
    }

    u1Pattern = 0;
    if(eVideoType == MTFVR_VIDEO_MPEG)
    {
        switch(u1Type)
        {
        case FVR_PIC_I_TYPE:
            u1Pattern = 3;
            break;

        case FVR_PIC_P_TYPE:
            u1Pattern = 2;
            break;

        case FVR_PIC_B_TYPE:
            u1Pattern = 1;
            break;

        default:
            break;
        }
    }
    else if(eVideoType == MTFVR_VIDEO_H264)
    {
        switch(u1Type)
        {
        case FVR_PIC_H264_IDR_TYPE:
            u1Pattern = 3;
            break;

        case FVR_PIC_H264_NIDR_TYPE:
            u1Pattern = 2;
            break;

        default:
            break;
        }
    }

    _MwPvrLock();
     _apu4MwPVRPicData[u1InputId][_au4MwPVRPicIdx[u1InputId]] = ((u1Pattern & 0x3) << 30) | (u4PktIdx & 0x3FFFFFFF);
    _au4MwPVRPicIdx[u1InputId]++;
    u4PicIdx = _au4MwPVRPicIdx[u1InputId];
    _MwPvrUnlock();

    if(u4PicIdx >= PVRMW_TICK_PIC_MAX)
    {
        //Printf("Picture data exceed 0x%x\n", TICK_PIC_MAX);
        ASSERT(0);
    }

    UNUSED(pvTag);
}

//-----------------------------------------------------------------------------
/** _MwPVRDectectScrambleHandler
 */
//-----------------------------------------------------------------------------
static BOOL _MwPVRDectectScrambleHandler(UINT8 u1Pidx, MTFVR_SCRAMBLE_STATE_T eState)
{
    DMX_MW_PVRPID_INFO_T *prPidInfo;
    RECDEMUX_SCRS_T eScrs;

    prPidInfo = _MW_Rec_GetPidInfo(u1Pidx);
    if(prPidInfo == NULL)
    {
        LOG(1, "%s error\n", __FUNCTION__);
        return FALSE;
    }

    //Printf("%s, Pidx %u scrambling state: %s\n", __FUNCTION__, u1Pidx,
    //            (eState == FVR_SCRAMBLE_STATE_SCRAMBLED) ? "SCRAMBLED" : "CLEAR");

    if(eState == MTFVR_SCRAMBLE_STATE_CLEAR)
    {
        eScrs = RECDEMUX_SCRS_CLEAR;
    }
    else if(eState == MTFVR_SCRAMBLE_STATE_SCRAMBLED)
    {
        eScrs = RECDEMUX_SCRS_SCRAMBLED;
    }
    else
    {
        eScrs = RECDEMUX_SCRS_UNKNOWN;
    }

    if(prPidInfo->rNfyInfo.pf_recdemux_nfy != NULL)
    {
        if(!prPidInfo->rNfyInfo.pf_recdemux_nfy(prPidInfo->rNfyInfo.pv_tag,
                                RECDEMUX_COND_SCRS, (UINT32)eScrs))
        {
            return FALSE;
        }
    }

    UNUSED(eScrs);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MwPVRTimeHandler
 */
//-----------------------------------------------------------------------------
static void _MwPVRTimeHandler(UINT32 u4Data, const void* pvTag)
{
    MTFVR_GBL_TIMENOTIFY_INFO_T* prTimeInfo;
    UINT32 i, u4PicIdx, u4PicSize, u4EntryNum;
    UINT32 u4PacketNum, u4PrePktNum;
    INT32 i4PktDiff;
    UINT8 u1MetaBufIdx, u1TsIdx, u1InputId;
    PVR_TICK_DETAIL_T *prPvrTick;
    PVR_PIC_INFO_T *prPicInfo;

    ASSERT(_pu4MwPVRPicData != NULL);

    prTimeInfo = (MTFVR_GBL_TIMENOTIFY_INFO_T*)u4Data;
    u1TsIdx = prTimeInfo->u1TsIdx;
    u1InputId = _MWPvrGetInputIdByTsIdx(u1TsIdx);
    if(u1InputId >= MW_FVR_INPUT_NUM)
    {
        return;
    }
    prTimeInfo->u4Addr = _MwPvrKerlToUserAddr(u1InputId, prTimeInfo->u4Addr);

    u1MetaBufIdx = _MwPVRAllocMetaBuf(u1InputId);
    if(u1MetaBufIdx == 0xFF)
    {
        if(MTFVR_Stop(u1TsIdx) != MTR_OK)
        {
            return;
        }

        _MwPvrLock();
        _afgIsOverflow[u1InputId] = TRUE;
        _MwPvrUnlock();

        LOG(1, "Meta overflow\n");

        if(_arMwPVRNotifyInfo[u1InputId].pf_nfy != NULL)
        {
            LOG(3, "%s meta buf overflow\n", __FUNCTION__);

            if(!_arMwPVRNotifyInfo[u1InputId].pf_nfy(_arMwPVRNotifyInfo[u1InputId].pv_tag, PVR_COND_OVERFLOW,
                                         PVR_OVERFLOW_REASON_METADATA_BUF_FULL))
            {
                LOG(3, "%s MW notify return fail\n", __FUNCTION__);
            }
        }
        return;
    }

    if(u1MetaBufIdx >= MW_FVR_META_BUF_NUM)
    {
        return;
    }

    _MwPvrLock();
    _arMwPVRMetaBuf[u1InputId][u1MetaBufIdx].fgValid = TRUE;
    u4PrePktNum = _au4MwPVRPktCount[u1InputId];
    _MwPvrUnlock();

    if(prTimeInfo->u4PktCount >= u4PrePktNum)
    {
        u4PacketNum = prTimeInfo->u4PktCount - u4PrePktNum;
    }
    else
    {
        // Packet count overflow
        u4PacketNum = 0xFFFFFFFF - (u4PrePktNum - prTimeInfo->u4PktCount - 1);
    }

    ASSERT((u4PacketNum * 192) == prTimeInfo->u4Size);
//      LOG(5, "%s pkt:%d, size:0x%x\n", __FUNCTION__, u4PacketNum, prTimeInfo->u4Size);

    _MwPvrLock();
    _au4MwPVRPktCount[u1InputId] = prTimeInfo->u4PktCount;

    // Compose meta buf
    _arMwPVRMetaBuf[u1InputId][u1MetaBufIdx].u4SerialNum = _au4MwPVRTickNum[u1InputId];
    prPvrTick = (PVR_TICK_DETAIL_T*)(_arMwPVRMetaBuf[u1InputId][u1MetaBufIdx].u4BufAddr);
    prPicInfo = (PVR_PIC_INFO_T*)(_arMwPVRMetaBuf[u1InputId][u1MetaBufIdx].u4BufAddr + sizeof(PVR_TICK_DETAIL_T));

    prPvrTick->ui4_tick_num = _au4MwPVRTickNum[u1InputId]++;            // Field 0: Tick num
    PVR_TICK_DETAIL_SET_BLKS(prPvrTick, u4PacketNum);

    u4EntryNum = 0;

    // Default value
    PVR_TICK_DETAIL_SET_ENTRIES(prPvrTick, 0);

    if(u4PacketNum == 0)
    {
        _au4MwPVRPicIdx[u1InputId] = 0;
    }
    else
    {
        if(_au4MwPVRPicIdx[u1InputId] > 1)
        {
            for(i=0; i<(_au4MwPVRPicIdx[u1InputId] - 1); i++)
            {
                if(((_apu4MwPVRPicData[u1InputId][i] >> 30) & 0x3) == 3)
                {
                    u4PicIdx = _apu4MwPVRPicData[u1InputId][i] & 0x3FFFFFFF;
                    u4PicSize = (_apu4MwPVRPicData[u1InputId][i+1] & 0x3FFFFFFF) -
                                (_apu4MwPVRPicData[u1InputId][i] & 0x3FFFFFFF);

                    i4PktDiff = (INT32)u4PicIdx - (INT32)u4PrePktNum;
                    PVR_PIC_INFO_SET_LBA_OFFSET(prPicInfo, i4PktDiff);
                    PVR_PIC_INFO_SET_PIC_TYPE(prPicInfo, 3);
                    PVR_PIC_INFO_SET_PIC_BLKS(prPicInfo, u4PicSize);

                    u4EntryNum++;
                }
            }

            PVR_TICK_DETAIL_SET_ENTRIES(prPvrTick, u4EntryNum);

            _apu4MwPVRPicData[u1InputId][0] = _apu4MwPVRPicData[u1InputId][_au4MwPVRPicIdx[u1InputId] - 1];
            _au4MwPVRPicIdx[u1InputId] = 1;
        }
        else
        {
            // Do nothing
        }
    }

    _MwPvrUnlock();

    // Notify MW
    if(_arMwPVRNotifyInfo[u1InputId].pf_nfy != NULL)
    {
        PVR_TICK_DATA_T rTickData;
        rTickData.pv_metadata_tag = (VOID*)(UINT32)u1MetaBufIdx;
        rTickData.ui4_pkt_count = u4PacketNum;
        rTickData.pui1_data = (UINT8*)prTimeInfo->u4Addr;
        rTickData.pt_metadata = (PVR_METADATA_T*)prPvrTick;

        if(((rTickData.pt_metadata->t_tick_detail.ui4_blks_entries) & 0xFFFF) != rTickData.ui4_pkt_count)
        {
            ASSERT(0);
        }

        if(!_arMwPVRNotifyInfo[u1InputId].pf_nfy(_arMwPVRNotifyInfo[u1InputId].pv_tag, PVR_COND_TICK, (UINT32)&rTickData))
        {
            LOG(3, "%s MW notify return fail\n", __FUNCTION__);
        }
    }

    UNUSED(pvTag);
}


//-----------------------------------------------------------------------------
/** _MwPVRHandler
 */
//-----------------------------------------------------------------------------
static VOID _MwPVRHandler(MTFVR_GBL_NOTIFY_CODE_T eCode,
                          UINT32 u4Data, const void* pvTag)
{
    BOOL fgEnable;
    UINT8 u1TsIdx = 0;
    UINT8 u1InputId;

    if(eCode == MTFVR_GBL_NFY_OVERFLOW)
    {
        u1TsIdx = (UINT8)u4Data;
    }
    else if(eCode == MTFVR_GBL_NFY_TIME)
    {
        MTFVR_GBL_TIMENOTIFY_INFO_T* prTimeInfo;

        prTimeInfo = (MTFVR_GBL_TIMENOTIFY_INFO_T*)u4Data;
        u1TsIdx = prTimeInfo->u1TsIdx;
    }
    u1InputId = _MWPvrGetInputIdByTsIdx(u1TsIdx);
    if(u1InputId >= MW_FVR_INPUT_NUM)
    {
        return;
    }
    _MwPvrLock();
    fgEnable = _afgMwPvrEnable[u1InputId];
    _MwPvrUnlock();

    if(!fgEnable)
    {
        return;
    }

    if(eCode == MTFVR_GBL_NFY_OVERFLOW)
    {
        _MwPvrLock();
        _afgIsOverflow[u1InputId] = TRUE;
        _MwPvrUnlock();

        LOG(1, "Stream overflow\n");

        if(_arMwPVRNotifyInfo[u1InputId].pf_nfy != NULL)
        {
            if(!_arMwPVRNotifyInfo[u1InputId].pf_nfy(_arMwPVRNotifyInfo[u1InputId].pv_tag, PVR_COND_OVERFLOW,
                                         PVR_OVERFLOW_REASON_TS_BUF_FULL))
            {
                LOG(3, "%s MW notify return fail\n", __FUNCTION__);
            }
        }
    }
    else if(eCode == MTFVR_GBL_NFY_TIME)
    {
        _MwPVRTimeHandler(u4Data, pvTag);
    }
    else if(eCode == MTFVR_GBL_NFY_PKTCOUNT)
    {
        ASSERT(0);
    }

    UNUSED(pvTag);
}



//-----------------------------------------------------------------------------
/** _MwPVRSetCallback
 */
//-----------------------------------------------------------------------------
static BOOL _MwPVRSetCallback(UINT8 u1InputId, const PVR_NFY_INFO_T* prNfyInfo)
{
    MTFVR_GBL_T rGBL;
    UINT8 u1TsIdx;

    if(prNfyInfo == NULL)
    {
        return FALSE;
    }

    u1TsIdx = (UINT8)_arFvrInput[u1InputId].u4TsIdx;

    _arMwPVRNotifyInfo[u1InputId].pf_nfy = prNfyInfo->pf_nfy;
    _arMwPVRNotifyInfo[u1InputId].pv_tag = prNfyInfo->pv_tag;

    x_memset((void*)&rGBL, 0, sizeof(MTFVR_GBL_T));
    rGBL.pfnNotifyFunc = _MwPVRHandler;
    rGBL.pvNotifyTag = NULL;

    return (MTFVR_SetGBL(u1TsIdx, MTFVR_GBL_FLAG_NOTIFY, &rGBL) == MTR_OK)?TRUE:FALSE;
}


//-----------------------------------------------------------------------------
/** _MwPVREnablePid
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVREnablePid(UINT8 u1Pidx, const VOID* pv_set_info)
{
    DMX_MW_PVRPID_INFO_T *prPidInfo;
    MTFVR_PID_T rPid;
    BOOL fgEnable;
    UINT32 u4Flags;

    if (u1Pidx >= (PVR_STREAM_COMP_NUM + PVR_STREAM_PCR_COMP_NUM))
    {
        return RMR_INV_SET;
    }

    x_memset(&rPid, 0, sizeof(rPid));

    fgEnable = (BOOL)(UINT32)pv_set_info;

    rPid.fgEnable = fgEnable;
    u4Flags = MTFVR_PID_FLAG_ENABLE;

    prPidInfo = _MW_Rec_GetPidInfo(u1Pidx);
    if(prPidInfo == NULL)
    {
        return RMR_INV_SET;
    }

    if(fgEnable)
    {
        if(!prPidInfo->fgValid)
        {
            return RMR_INV_SET;
        }

        rPid.u2Pid = prPidInfo->u2PidNum;
        u4Flags |= MTFVR_PID_FLAG_PID;

        if(prPidInfo->rNfyInfo.pf_recdemux_nfy != NULL)
        {
            rPid.fgDetectScramble = TRUE;
            rPid.pfnDetectScrambleNotify = _MwPVRDectectScrambleHandler;
            if(((prPidInfo->u4TypeMask & ST_MASK_AUDIO) != 0) ||
               ((prPidInfo->u4TypeMask & ST_MASK_VIDEO) != 0))
            {
                //rPid.fgIsAVScramble = TRUE;
            }
            else
            {
                //rPid.fgIsAVScramble = FALSE;
            }
        }
        else
        {
            rPid.fgDetectScramble = FALSE;
            rPid.pfnDetectScrambleNotify = NULL;
            //rPid.fgIsAVScramble = FALSE;
            //rPid.u1PerPidIdx = 0x0;
        }

        u4Flags |= MTFVR_PID_FLAG_DETECT_SCRAMBLE;
    }

    LOG(6,"_MwPVREnablePid, Pidx:%d, enable:%d\n", u1Pidx, fgEnable);

    if(!_MwPvrSetPid(u1Pidx, u4Flags, &rPid))
    {
        return RMR_INV_SET;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVROutputPid
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVROutputPid(UINT8 u1Pidx, const VOID* pv_set_info)
{
    MTFVR_PID_T rPid;
    BOOL fgEnable;
    UINT32 u4Flags;

    if (u1Pidx >= (PVR_STREAM_COMP_NUM + PVR_STREAM_PCR_COMP_NUM))
    {
        return RMR_INV_SET;
    }

    x_memset(&rPid, 0, sizeof(rPid));

    fgEnable = (BOOL)(UINT32)pv_set_info;

    rPid.fgFreeze = !fgEnable;
    u4Flags = MTFVR_PID_FLAG_FREEZE;

    if(!_MwPvrSetPid(u1Pidx, u4Flags, &rPid))
    {
        return RMR_INV_SET;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVREnPicSearch
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVREnPicSearch(UINT8 u1Pidx, const VOID* pv_set_info)
{
    MTFVR_PID_T rPid;
    BOOL fgEnable;
    UINT8 u1PerPidx = 0xFF;

    if (u1Pidx >= (PVR_STREAM_COMP_NUM + PVR_STREAM_PCR_COMP_NUM))
    {
        return RMR_INV_SET;
    }

    x_memset(&rPid, 0, sizeof(rPid));

    fgEnable = (BOOL)(UINT32)pv_set_info;

    rPid.fgPicSearch = fgEnable;
    rPid.u1PicSearchMask = FVR_PIC_I_MASK | FVR_PIC_P_MASK | FVR_PIC_B_MASK;
    rPid.pfnPicSearchNotify = _MwPVRPicHandler;
    rPid.pvPicSearchNotifyTag = NULL;

    u1PerPidx = _MwPvrAllocPerPidx(u1Pidx);
    if(u1PerPidx == 0xFF)
    {
        return RMR_INV_SET;
    }
    //rPid.u1PerPidIdx = u1PerPidx;

    if(!_MwPvrSetPid(u1Pidx, MTFVR_PID_FLAG_PICSEARCH | MTFVR_PID_FLAG_DETECT_SCRAMBLE, &rPid))
    {
        return RMR_INV_SET;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRSetSwap
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRSetSwap(UINT8 u1Pidx, const PVR_SWAP_INFO_T* prSwapInfo)
{
    MTFVR_PID_T rPid;
    const PVR_SECT_DATA_T* pSecData;

    if (u1Pidx >= (PVR_STREAM_COMP_NUM + PVR_STREAM_PCR_COMP_NUM))
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if(prSwapInfo == NULL)
    {
        return RMR_INV_SET;
    }

    x_memset(&rPid, 0, sizeof(rPid));

    if(prSwapInfo->e_swap_type == PVR_SWAP_TYPE_DISABLE)
    {
        rPid.fgPktSwap = FALSE;
        if(!_MwPvrSetPid(u1Pidx, MTFVR_PID_FLAG_PKTSWAP, &rPid))
        {
            return RMR_INV_SET;
        }

        if(MTFVR_SwapFreePattern(u1Pidx) != MTR_OK)
        {
            return RMR_INV_SET;
        }

        return RMR_OK;
    }
    else if(prSwapInfo->e_swap_type == PVR_SWAP_TYPE_NULL_PKT)
    {
        if(MTFVR_SwapSetNullPattern(u1Pidx) != MTR_OK)
        {
            return RMR_INV_SET;
        }
    }
    else if(prSwapInfo->e_swap_type == PVR_SWAP_TYPE_SECT_LIST)
    {
        pSecData = prSwapInfo->u.t_sect_list.at_sects;

        if(MTFVR_SwapSetPattern(u1Pidx, (UINT8*)pSecData->pv_sect_data,
                           (UINT16)pSecData->z_sect_len) != MTR_OK)
        {
            return RMR_INV_SET;
        }
    }

    rPid.fgPktSwap = TRUE;
    rPid.pfnSwapNotify = NULL;
    rPid.pvSwapNotifyTag = NULL;
    if(!_MwPvrSetPid(u1Pidx, MTFVR_PID_FLAG_PKTSWAP, &rPid))
    {
        return RMR_INV_SET;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRSetEncryption
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRSetEncryption(DRV_COMP_ID_T* pt_comp_id,
    const VOID* pv_set_info, UINT8 u1Pidx)
{
    MM_CRYPT_INFO_T *ptEncryptInfo;
    MW_DMX_CRYPT_KEY_UNION_T uCryptKey;
    MW_DMX_CRYPT_KEY_INFO_T rKeyInfo;
    MTDMX_CRYPTO_KEY_TYPE_T eKeyType;
    MTFVR_PID_T rPid;
    UINT32 i, u4Flags, u4KeyLen;
    UINT8 u1KeyIdx, au1OddKey[16], au1EvenKey[16];

    ptEncryptInfo = (MM_CRYPT_INFO_T*)pv_set_info;

    if(ptEncryptInfo == NULL)
    {
        return RMR_INV_ARG;
    }

    if((ptEncryptInfo->u.t_pvr_crypt.e_mode == MM_CRYPT_MODE_DES_CBC) ||
       (ptEncryptInfo->u.t_pvr_crypt.e_mode == MM_CRYPT_MODE_3_DES_CBC))
    {
        LOG(3, "%s,%d not support CBC mode\n", __FUNCTION__, __LINE__);
        return RMR_INV_ARG;
    }

    x_memset(&rPid, 0, sizeof(rPid));

    //-------------------------------------------
    // Set key
    //-------------------------------------------
    u4KeyLen = 128;
    x_memset((void*)au1OddKey, 0, sizeof(au1OddKey));
    x_memset((void*)au1EvenKey, 0, sizeof(au1EvenKey));

    switch(ptEncryptInfo->e_mm_crypt_type)
    {
        case MM_CRYPT_TYPE_PVR:
            default:
            eKeyType = MTDMX_CRYPTO_KEY_SECURE_PVR_AES_0;
        break;
    }

    if((ptEncryptInfo->u.t_pvr_crypt.e_key_idx == MM_CRYPT_KEY_ALTERNATE) ||
       (ptEncryptInfo->u.t_pvr_crypt.e_key_idx == MM_CRYPT_KEY_ODD))
    {
        for(i=0; i<4; i++)
        {
            if(ptEncryptInfo->u.t_pvr_crypt.b_is_encrypt) 
            {
                au1OddKey[i] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_1[0], i);
                au1OddKey[i+4] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_1[1], i);
            }
            else
            {
                au1OddKey[i] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_1[0], 3 - i);
                au1OddKey[i+4] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_1[1], 3 - i);
            }
        }

        if(ptEncryptInfo->u.t_pvr_crypt.e_mode == MM_CRYPT_MODE_AES_ECB ||ptEncryptInfo->u.t_pvr_crypt.e_mode ==  MM_CRYPT_MODE_AES_CBC)
        {
            for(i=0; i<4; i++)
            {
                if(ptEncryptInfo->u.t_pvr_crypt.b_is_encrypt)
                {
                    au1OddKey[i+8] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_2[0], i);
                    au1OddKey[i+12] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_2[1], i);
                }
                else
                {
                    au1OddKey[i+8] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_2[0], 3 - i);
                    au1OddKey[i+12] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_odd_key_2[1], 3 - i);
                }
            }
        }

        x_memset((void*)&uCryptKey, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));

        for (i = 0; i < 4; i++)
        {
            uCryptKey.rAESKeySet.au4Key[i] = au1OddKey[(i*4) + 3] | (au1OddKey[(i*4) + 2] << 8) |
                                        (au1OddKey[(i*4) + 1] << 16) | (au1OddKey[i*4] << 24);
        }

        rKeyInfo.fgEven = FALSE;
        rKeyInfo.eDescAlg = DESC_ALG_AES_128_ECB;
        rKeyInfo.puCryptKey = &uCryptKey;
        u1KeyIdx = MW_DMX_CRYPT_INVALID_KEYX;

        if (!_MW_DMX_CRYPT_AllocateFvrKeySlot(&rKeyInfo, &u1KeyIdx))
        {
            LOG(3, "%s - fails to allocate key index\n", __FUNCTION__);
            return RMR_INV_ARG;
        }

        if(ptEncryptInfo->u.t_pvr_crypt.b_is_encrypt)
        {
            if(MTR_OK != MTFVR_SetCryptoKey(u1KeyIdx, FALSE, eKeyType, au1OddKey, sizeof(au1OddKey)))
            {
                return RMR_INV_ARG;
            }
        }
        else
        {
            if(MTFVR_SetAesKey(u1KeyIdx, FALSE, au1OddKey) != MTR_OK)
            {
                return RMR_INV_ARG;
            }
        }
    }

    if((ptEncryptInfo->u.t_pvr_crypt.e_key_idx == MM_CRYPT_KEY_ALTERNATE) ||
       (ptEncryptInfo->u.t_pvr_crypt.e_key_idx == MM_CRYPT_KEY_EVEN))
    {
        for(i=0; i<4; i++)
        {
            if(ptEncryptInfo->u.t_pvr_crypt.b_is_encrypt)
            {
                au1EvenKey[i] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_1[0], i);
                au1EvenKey[i+4] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_1[1], i);
            }
            else
            {
                au1EvenKey[i] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_1[0], 3 - i);
                au1EvenKey[i+4] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_1[1], 3 - i);
            }
        }

        if(ptEncryptInfo->u.t_pvr_crypt.e_mode == MM_CRYPT_MODE_AES_ECB ||ptEncryptInfo->u.t_pvr_crypt.e_mode ==  MM_CRYPT_MODE_AES_CBC)
        {
            for(i=0; i<4; i++)
            {
                if(ptEncryptInfo->u.t_pvr_crypt.b_is_encrypt)
                {
                    au1EvenKey[i+8] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_2[0], i);
                    au1EvenKey[i+12] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_2[1], i);
                }
                else
                {
                    au1EvenKey[i+8] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_2[0], 3 - i);
                    au1EvenKey[i+12] = GET_BYTE(ptEncryptInfo->u.t_pvr_crypt.aui4_even_key_2[1], 3 - i);
                }
            }
        }

        x_memset((void*)&uCryptKey, 0, sizeof(MW_DMX_CRYPT_KEY_UNION_T));

        for (i = 0; i < 4; i++)
        {
            uCryptKey.rAESKeySet.au4Key[i] = au1EvenKey[(i*4) + 3] | (au1EvenKey[(i*4) + 2] << 8) |
                                        (au1EvenKey[(i*4) + 1] << 16) | (au1EvenKey[i*4] << 24);
        }

        rKeyInfo.fgEven = TRUE;
        rKeyInfo.eDescAlg = DESC_ALG_AES_128_ECB;
        rKeyInfo.puCryptKey = &uCryptKey;
        u1KeyIdx = MW_DMX_CRYPT_INVALID_KEYX;

        if (!_MW_DMX_CRYPT_AllocateFvrKeySlot(&rKeyInfo, &u1KeyIdx))
        {
            LOG(3, "%s - fails to allocate key index\n", __FUNCTION__);
            return RMR_INV_ARG;
        }

        if(ptEncryptInfo->u.t_pvr_crypt.b_is_encrypt)
        {
            if(MTR_OK != MTFVR_SetCryptoKey(u1KeyIdx, TRUE, eKeyType, au1EvenKey, sizeof(au1EvenKey)))
            {
                return RMR_INV_ARG;
            }
        }
        else
        {
            if(MTFVR_SetAesKey(u1KeyIdx, TRUE, au1EvenKey) != MTR_OK)
            {
                return RMR_INV_ARG;
            }
        }
    }

    switch(ptEncryptInfo->u.t_pvr_crypt.e_key_idx)
    {
    case MM_CRYPT_KEY_ODD:
        rPid.eKeyType = MTFVR_KEY_ODD;
        break;

    case MM_CRYPT_KEY_EVEN:
        rPid.eKeyType = MTFVR_KEY_EVEN;
        break;

    case MM_CRYPT_KEY_ALTERNATE:
        rPid.eKeyType = MTFVR_KEY_BOTH;
        break;

    default:
        return RMR_INV_ARG;
    }

    rPid.fgCBCMode = FALSE;
    rPid.u4KeyLen = u4KeyLen;
    rPid.u1KeyIdx = u1KeyIdx;
    rPid.fgScramble = TRUE;
    //rPid.fgScramble = FALSE;

    u4Flags = (MTFVR_PID_FLAG_CBC_MODE | MTFVR_PID_FLAG_KEY_TYPE |
               MTFVR_PID_FLAG_SCRAMBLE_ENABLE | MTFVR_PID_FLAG_KEY_LEN);

    if(!_MwPvrSetPid(u1Pidx, u4Flags, &rPid))
    {
        return RMR_INV_SET;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRUnlockBuffer
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRUnlockBuffer(UINT8 u1InputId,
                              PVR_UNLOCK_BUFFER_INFO_T* prBuffer)
{
    UINT32 u4Addr, u4Size, u4KernelAddr;
    UINT8 u1TsIdx;

    if((prBuffer == NULL))
    {
        return RMR_INV_SET;
    }

    u1TsIdx = (UINT8)_arFvrInput[u1InputId].u4TsIdx;

    u4Addr = (UINT32)prBuffer->pui1_data_addr;
    u4Size = (UINT32)prBuffer->z_data_len;

    u4KernelAddr = _MwPvrUserToKerlAddr(u1InputId, u4Addr);

    LOG(5, "PVR unlock buffer, addr:0x%x, size:0x%x\n", u4KernelAddr, u4Size);


    if(MTFVR_UnlockBuffer(u1TsIdx, u4KernelAddr, u4Size) != MTR_OK)
    {
        return RMR_INV_SET;
    }

    return RMR_OK;
}



//-----------------------------------------------------------------------------
/** _MwPVRStream_Connect
 *  Middleware interface - connect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStream_Connect(
                            const DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    MTFVR_PID_T rPid;
    UINT32 u4Flags;
    UINT8 u1Pidx;
    DRV_COMP_ID_T* prConnectComp = 0;
    
    if ((NULL == pt_comp_id) || (NULL == pv_conn_info) )
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_COMP_NUM)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

     // Check component type
    prConnectComp = (DRV_COMP_ID_T*)pv_conn_info;
    if (!_MW_FVR_ConnectInfo(u1Pidx, (UINT32)pt_comp_id->e_type, prConnectComp->e_type, prConnectComp->ui2_id))
    {
        return RMR_DRV_CONN_FAILED;
    }
    
    x_memset((VOID*)&rPid, 0, sizeof(MTFVR_PID_T));
    rPid.fgEnable = FALSE;
    rPid.u2Pid = 0;
    rPid.fgScramble = FALSE;
    rPid.u1KeyIdx = 0;

    u4Flags = MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_SCRAMBLE_ENABLE;

    if(!_MwPvrSetPid(u1Pidx, u4Flags, &rPid))
    {
        return RMR_DRV_CONN_FAILED;
    }

    x_memset((VOID*)&rPid, 0, sizeof(MTFVR_PID_T));
    rPid.fgEnable = FALSE;
    rPid.u2Pid = 0;
    rPid.fgScramble = FALSE;
    rPid.fgPktSwap = FALSE;

    rPid.fgPicSearch = FALSE;
    rPid.u1PicSearchMask = 0;
    rPid.pfnPicSearchNotify = NULL;
    rPid.pvPicSearchNotifyTag = NULL;

    rPid.u1TsIndex = _arFvrPidInfo[u1Pidx].u1TsIndex;

    u4Flags = MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_PICSEARCH | MTFVR_PID_FLAG_PKTSWAP |
              MTFVR_PID_FLAG_SCRAMBLE_ENABLE | MTFVR_PID_FLAG_TSINDEX;

    if(!_MwPvrSetPid(u1Pidx, u4Flags, &rPid))
    {
        return RMR_DRV_CONN_FAILED;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRStream_Disconnect
 *  Middleware interface - disconnect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStream_Disconnect(
                            const DRV_COMP_ID_T*    pt_comp_id,
                            DRV_DISC_TYPE_T         e_disc_type,
                            const VOID*             pv_disc_info,
                            SIZE_T                  z_disc_info_len)
{
    UINT8 u1Pidx, i;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_disc_info) )
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_COMP_NUM)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (!_MW_FVR_DisConnectInfo(u1Pidx))
    {
        return RMR_DRV_DISC_FAILED;
    }
    
    if(MTFVR_FreePid(u1Pidx) != MTR_OK)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    for(i=0; i<FVR_SCRAMBLE_PID_INDEX; i++)
    {
        if(_arMwPvrPerPidxUsage[i].fgEnable && (_arMwPvrPerPidxUsage[i].u1Pidx == u1Pidx))
        {
            _arMwPvrPerPidxUsage[i].fgEnable = FALSE;
            _arMwPvrPerPidxUsage[i].u1Pidx = 0xFF;
        }
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRStream_Get
 *  Middleware interface - get
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStream_Get(
                        const DRV_COMP_ID_T*    pt_comp_id,
                        DRV_GET_TYPE_T          e_get_type,
                        VOID*                   pv_get_info,
                        SIZE_T*                 pz_get_info_len)
{
    MTFVR_GBL_T rGBL;
    PVR_BUFFER_STATUS_T *prBufStatus;
    MTFVR_PID_T rPid;
    UINT8 u1Pidx, u1TsIdx, u1InputId;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) )
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    // Check index range
    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_COMP_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }
    
    u1TsIdx = _arFvrPidInfo[u1Pidx].u1TsIndex;
    u1InputId = _MWPvrGetInputIdByTsIdx(u1TsIdx);
    if(u1InputId >= MW_FVR_INPUT_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }
    
    x_memset(&rGBL, 0, sizeof(rGBL));
    x_memset(&rPid, 0, sizeof(rPid));

    switch (e_get_type)
    {
    case PVR_GET_TYPE_CTRL:
        LOG(6,"%s PVR_GET_TYPE_CTRL\n", __FUNCTION__);
        if(MTFVR_GetPid(u1Pidx, MTFVR_PID_FLAG_ENABLE, &rPid) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        *(BOOL*)pv_get_info = rPid.fgEnable;
        *pz_get_info_len = sizeof(BOOL);
        break;

    case PVR_GET_TYPE_BUFFER_STATUS:
        LOG(6,"%s PVR_GET_TYPE_BUFFER_STATUS\n", __FUNCTION__);
        if(MTFVR_GetGBL(u1TsIdx, MTFVR_GBL_FLAG_BUFFER, &rGBL) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        *pz_get_info_len = sizeof(PVR_BUFFER_STATUS_T);
        LOG(6,"xxxxxxxxxxxx %s BUFFER_STATUS 0x%x, 0x%x\n", __FUNCTION__, rGBL.u4Rp, rGBL.u4Wp);
        prBufStatus = (PVR_BUFFER_STATUS_T*)pv_get_info;
        prBufStatus->pui1_read_ptr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4Rp);
        prBufStatus->pui1_write_ptr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4Wp);
        break;

    default:
        break;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRStream_Set
 *  Middleware interface - set
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStream_Set(
                    DRV_COMP_ID_T*      pt_comp_id,
                    DRV_SET_TYPE_T      e_set_type,
                    const VOID*         pv_set_info,
                    SIZE_T              z_set_info_len)
{
    INT32 i4Ret = RMR_OK;
    UINT8 u1Pidx;

    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_COMP_NUM)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    switch (e_set_type)
    {
    case PVR_SET_TYPE_CTRL:
        LOG(6,"%s PVR_SET_TYPE_CTRL\n", __FUNCTION__);
        i4Ret = _MwPVREnablePid(u1Pidx, pv_set_info);
        break;

    case PVR_SET_TYPE_CTRL_OUTPUT:
        LOG(6,"%s PVR_SET_TYPE_CTRL_OUTPUT\n", __FUNCTION__);
        i4Ret = _MwPVROutputPid(u1Pidx, pv_set_info);
        break;

    case PVR_SET_TYPE_EN_PIC_SEARCH:
        LOG(6,"%s PVR_SET_TYPE_EN_PIC_SEARCH\n", __FUNCTION__);
        i4Ret = _MwPVREnPicSearch(u1Pidx, pv_set_info);
        break;

    case PVR_SET_TYPE_SWAP:
        LOG(6,"%s PVR_SET_TYPE_EN_PIC_SEARCH\n", __FUNCTION__);
        i4Ret = _MwPVRSetSwap(u1Pidx, (PVR_SWAP_INFO_T*)pv_set_info);
        break;

    case PVR_SET_TYPE_ENCRYPTION:
        LOG(6,"%s PVR_SET_TYPE_EN_PIC_SEARCH\n", __FUNCTION__);
        i4Ret = _MwPVRSetEncryption(pt_comp_id, pv_set_info, u1Pidx);
        break;

    default:
        break;
    }

    LOG(6,"%s %d\n", __FUNCTION__, i4Ret);

    return i4Ret;
}


//-----------------------------------------------------------------------------
/** _MwPVRStreamPCR_Connect
 *  Middleware interface - connect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStreamPCR_Connect(
                            const DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    MTFVR_PID_T rPid;
    UINT32 u4Flags;
    UINT8 u1Pidx;
    DRV_COMP_ID_T* prConnectComp = 0;

    if ((NULL == pt_comp_id) || (NULL == pv_conn_info) )
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_PCR_COMP_NUM)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    u1Pidx += PVR_STREAM_COMP_NUM;
    // Check component type
    prConnectComp = (DRV_COMP_ID_T*)pv_conn_info;
    if (!_MW_FVR_ConnectInfo(u1Pidx, (UINT32)pt_comp_id->e_type, prConnectComp->e_type, prConnectComp->ui2_id))
    {
        return RMR_DRV_CONN_FAILED;
    }

    x_memset((VOID*)&rPid, 0, sizeof(MTFVR_PID_T));
    rPid.fgEnable = FALSE;
    rPid.u2Pid = 0;
    rPid.fgScramble = FALSE;
    rPid.u1KeyIdx = 0;

    rPid.u1TsIndex = _arFvrPidInfo[u1Pidx].u1TsIndex;
    u4Flags = MTFVR_PID_FLAG_PID | MTFVR_PID_FLAG_SCRAMBLE_ENABLE | MTFVR_PID_FLAG_TSINDEX;

    if(!_MwPvrSetPid(u1Pidx, u4Flags, &rPid))
    {
        return RMR_DRV_CONN_FAILED;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRStreamPCR_Disconnect
 *  Middleware interface - disconnect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStreamPCR_Disconnect(
                            const DRV_COMP_ID_T*    pt_comp_id,
                            DRV_DISC_TYPE_T         e_disc_type,
                            const VOID*             pv_disc_info,
                            SIZE_T                  z_disc_info_len)
{
    UINT8 u1Pidx;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_disc_info) )
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_PCR_COMP_NUM)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    u1Pidx += PVR_STREAM_COMP_NUM;

    if (!_MW_FVR_DisConnectInfo(u1Pidx))
    {
        return RMR_DRV_DISC_FAILED;
    }

    if(MTFVR_FreePid(u1Pidx) != MTR_OK)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRStreamPCR_Get
 *  Middleware interface - get
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStreamPCR_Get(
                        const DRV_COMP_ID_T*    pt_comp_id,
                        DRV_GET_TYPE_T          e_get_type,
                        VOID*                   pv_get_info,
                        SIZE_T*                 pz_get_info_len)
{
    MTFVR_GBL_T rGBL;
    PVR_BUFFER_STATUS_T *prBufStatus;
    MTFVR_PID_T rPid;
    UINT8 u1Pidx, u1TsIdx, u1InputId;
    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) )
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    // Check index range
    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_PCR_COMP_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }
    u1TsIdx = _arFvrPidInfo[u1Pidx].u1TsIndex;
    u1InputId = _MWPvrGetInputIdByTsIdx(u1TsIdx);
    if(u1InputId >= MW_FVR_INPUT_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }
    
    x_memset(&rGBL, 0, sizeof(rGBL));
    x_memset(&rPid, 0, sizeof(rPid));

    u1Pidx += PVR_STREAM_COMP_NUM;

    switch (e_get_type)
    {
    case PVR_GET_TYPE_CTRL:
        LOG(6,"%s PVR_GET_TYPE_CTRL\n", __FUNCTION__);
        if(MTFVR_GetPid(u1Pidx, MTFVR_PID_FLAG_ENABLE, &rPid) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        *(BOOL*)pv_get_info = rPid.fgEnable;
        *pz_get_info_len = sizeof(BOOL);
        break;

    case PVR_GET_TYPE_BUFFER_STATUS:
        LOG(6,"%s PVR_GET_TYPE_BUFFER_STATUS\n", __FUNCTION__);
        if(MTFVR_GetGBL(u1TsIdx, MTFVR_GBL_FLAG_BUFFER, &rGBL) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        prBufStatus = (PVR_BUFFER_STATUS_T*)pv_get_info;
        *pz_get_info_len = sizeof(PVR_BUFFER_STATUS_T);
        LOG(6,"xxxxxxxxxxxx %s BUFFER_STATUS 0x%x, 0x%x\n", __FUNCTION__, rGBL.u4Rp, rGBL.u4Wp);
        prBufStatus = (PVR_BUFFER_STATUS_T*)pv_get_info;
        prBufStatus->pui1_read_ptr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4Rp);
        prBufStatus->pui1_write_ptr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4Wp);
        break;

    default:
        break;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRStreamPCR_Set
 *  Middleware interface - set
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRStreamPCR_Set(
                    DRV_COMP_ID_T*      pt_comp_id,
                    DRV_SET_TYPE_T      e_set_type,
                    const VOID*         pv_set_info,
                    SIZE_T              z_set_info_len)
{
    INT32 i4Ret = RMR_OK;
    UINT8 u1Pidx;

    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    u1Pidx = (UINT8)pt_comp_id->ui2_id;
    if (u1Pidx >= PVR_STREAM_PCR_COMP_NUM)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    u1Pidx += PVR_STREAM_COMP_NUM;

    switch (e_set_type)
    {
    case PVR_SET_TYPE_CTRL:
        LOG(6,"%s PVR_SET_TYPE_CTRL\n", __FUNCTION__);
        i4Ret = _MwPVREnablePid(u1Pidx, pv_set_info);
        break;

    case PVR_SET_TYPE_CTRL_OUTPUT:
        LOG(6,"%s PVR_SET_TYPE_CTRL_OUTPUT\n", __FUNCTION__);
        i4Ret = _MwPVROutputPid(u1Pidx, pv_set_info);
        break;

    case PVR_SET_TYPE_EN_PIC_SEARCH:
        //i4Ret = RMR_DRV_INV_SET_INFO;
        break;

    case PVR_SET_TYPE_SWAP:
        //i4Ret = RMR_DRV_INV_SET_INFO;
        break;

    case PVR_SET_TYPE_ENCRYPTION:
        //i4Ret = RMR_DRV_INV_SET_INFO;
        break;

    default:
        break;
    }

    LOG(6,"%s %d\n", __FUNCTION__, i4Ret);

    return i4Ret;
}


//-----------------------------------------------------------------------------
/** _MwPVRCtrl_Connect
 *  Middleware interface - connect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRCtrl_Connect(
                            const DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    UINT8 u1InputId, u2Id;
    DRV_COMP_ID_T* prConnectComp = 0;

    if ((NULL == pt_comp_id) || (NULL == pv_conn_info) )
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    // Check index range
    u1InputId = (UINT8)pt_comp_id->ui2_id;
    if (u1InputId >= MW_FVR_INPUT_NUM)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    // Check connection type
    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
       return RMR_DRV_INV_CONN_INFO;
    }

    // Check component type
    prConnectComp = (DRV_COMP_ID_T*)pv_conn_info;
    u2Id = prConnectComp->ui2_id - 1;
    if(u2Id >= MW_DMX_CONN_TYPE_TUNER_MAX - MW_DMX_CONN_TYPE_TUNER_0)
    {
        LOG(1, "Connet Id is over the max number.\n");
        return RMR_DRV_INV_CONN_INFO;
    }

    _arFvrInput[u1InputId].fgValid = TRUE;
    _arFvrInput[u1InputId].u4TsIdx = _MwDmxContypeToTsIdx(MW_DMX_CONN_TYPE_TUNER_0 + u2Id);

    _au4MwPVRPktCount[u1InputId] = 0;

    return RMR_OK;
}



//-----------------------------------------------------------------------------
/** _MwPVRCtrl_Disconnect
 *  Middleware interface - disconnect
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRCtrl_Disconnect(
                            const DRV_COMP_ID_T*    pt_comp_id,
                            DRV_DISC_TYPE_T         e_disc_type,
                            const VOID*             pv_disc_info,
                            SIZE_T                  z_disc_info_len)
{
    UINT8 u1TsIdx, u1InputId;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_disc_info) )
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    // Check index range
    u1InputId = (UINT8)pt_comp_id->ui2_id;
    if (u1InputId >= MW_FVR_INPUT_NUM)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    u1TsIdx = _arFvrInput[u1InputId].u4TsIdx;

    //debug
    LOG(6, "_MwPVRCtrl_Disconnect TsIdx: %d\n", u1TsIdx);
    if(MTFVR_Stop(u1TsIdx) != MTR_OK)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if(MTFVR_FreeGBL(u1TsIdx) != MTR_OK)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    _arFvrInput[u1InputId].fgValid = FALSE;

    _arMwPVRNotifyInfo[u1InputId].pf_nfy = NULL;
    _arMwPVRNotifyInfo[u1InputId].pv_tag = NULL;

    return RMR_OK;
}



//-----------------------------------------------------------------------------
/** _MwPVRCtrl_Get
 *  Middleware interface - get
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRCtrl_Get(
                        const DRV_COMP_ID_T*    pt_comp_id,
                        DRV_GET_TYPE_T          e_get_type,
                        VOID*                   pv_get_info,
                        SIZE_T*                 pz_get_info_len)
{
    MTFVR_GBL_T rGBL;
    PVR_BUFFER_INFO_T* prBufInfo;
    PVR_BUFFER_STATUS_T *prBufStatus;
    UINT8 u1TsIdx, u1InputId;
//    UINT32 u4Addr, u4Size, u4Index;
//    PVR_RESIDUAL_DATA_INFO_T* prResidual;
    PVR_CAP_INFO_T *prCapInfo;

    // Check parameters
    if ((NULL == pt_comp_id) || (NULL == pv_get_info) )
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    // Check index range
    u1InputId = (UINT8)pt_comp_id->ui2_id;
    if(u1InputId >= MW_FVR_INPUT_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }
    u1TsIdx = (UINT8)_arFvrInput[u1InputId].u4TsIdx;
    if (u1TsIdx >= MTDMX_NUM_TS_INDEX)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    x_memset(&rGBL, 0, sizeof(rGBL));

    switch (e_get_type)
    {
    case PVR_GET_TYPE_CTRL:
        LOG(6,"%s PVR_GET_TYPE_CTRL\n", __FUNCTION__);
        _MwPvrLock();
        *(BOOL*)pv_get_info = _afgMwPvrEnable[u1InputId];
        _MwPvrUnlock();
        *pz_get_info_len = sizeof(BOOL);
        break;

    case PVR_GET_TYPE_BUFFER_INFO:
        if(MTFVR_GetGBL(u1TsIdx, MTFVR_GBL_FLAG_BUFFER, &rGBL) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        LOG(6,"%s PVR_GET_TYPE_BUFFER_INFO\n", __FUNCTION__);
        prBufInfo = (PVR_BUFFER_INFO_T*)pv_get_info;
        prBufInfo->pui1_buf_addr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4BufStart);
        prBufInfo->z_buf_len = (SIZE_T) (SIZE_T)rGBL.u4BufSize;
        LOG(6,"xxxxxxxxxxxx %s BUFFER_INFO 0x%x, 0x%x\n", __FUNCTION__, prBufInfo->pui1_buf_addr, prBufInfo->z_buf_len);
        *pz_get_info_len = sizeof(PVR_BUFFER_INFO_T);
        break;

    case PVR_GET_TYPE_BUFFER_STATUS:
        LOG(6,"%s PVR_GET_TYPE_BUFFER_STATUS\n", __FUNCTION__);
        if(MTFVR_GetGBL(u1TsIdx, MTFVR_GBL_FLAG_BUFFER, &rGBL) != MTR_OK)
        {
            return RMR_DRV_INV_GET_INFO;
        }
        prBufStatus = (PVR_BUFFER_STATUS_T*)pv_get_info;
        LOG(6,"xxxxxxxxxxxx %s BUFFER_STATUS 0x%x, 0x%x\n", __FUNCTION__, rGBL.u4Rp, rGBL.u4Wp);
        prBufStatus->pui1_read_ptr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4Rp);
        prBufStatus->pui1_write_ptr = (UINT8*)_MwPvrKerlToUserAddr(u1InputId, rGBL.u4Wp);
        *pz_get_info_len = sizeof(PVR_BUFFER_STATUS_T);
        break;

    case PVR_GET_TYPE_RESIDUAL_DATA:
        ASSERT(0);
        #if 0
        if(!FVR_GetResidual(&u4Addr, &u4Size, &u4Index))
        {
            return RMR_DRV_INV_GET_INFO;
        }
        prResidual = (PVR_RESIDUAL_DATA_INFO_T*)pv_get_info;
        *pz_get_info_len = sizeof(PVR_RESIDUAL_DATA_INFO_T);

#ifdef LINUX_TURNKEY_SOLUTION
        prResidual->pui1_addr = _MwPvrKerlToUserAddr(u4Addr);
#else
        prResidual->pui1_addr = (UINT8*)u4Addr;
#endif // LINUX_TURNKEY_SOLUTION

        prResidual->z_len = (SIZE_T)u4Size;
        UNUSED(u4Index);
        #endif
        break;

    case PVR_GET_TYPE_CAP_INFO:
        LOG(6,"%s PVR_GET_TYPE_CAP_INFO\n", __FUNCTION__);
        prCapInfo = (PVR_CAP_INFO_T*)pv_get_info;
        *pz_get_info_len = sizeof(PVR_CAP_INFO_T);
        prCapInfo->ui4_align_buf_start = FVR_FIFO_ALIGNMENT;
        prCapInfo->ui4_align_buf_size = FVR_FIFO_ALIGNMENT;
        prCapInfo->ui4_max_encrypt_key_num = FVR_SCRAMBLE_PID_INDEX;
        prCapInfo->ui4_max_encrypt_pid_num = FVR_NUM_PID_INDEX;
        prCapInfo->ui4_max_pid_num = FVR_NUM_PID_INDEX;
        break;

    default:
        break;
    }

    return RMR_OK;
}


//-----------------------------------------------------------------------------
/** _MwPVRInitCtrl
 */
//-----------------------------------------------------------------------------
static BOOL _MwPVRInitCtrl(UINT8 u1InputId, PVR_CTRL_CONFIG_T *prConfig)
{
    MTFVR_PID_T rFvrPid;
    MTFVR_GBL_T rGBL;
    UINT32 i;
    UINT8 u1TsIdx;

    if(prConfig == NULL)
    {
        return FALSE;
    }
    u1TsIdx = (UINT8)_arFvrInput[u1InputId].u4TsIdx;

    x_memset(&rFvrPid, 0, sizeof(rFvrPid));
    x_memset(&rGBL, 0, sizeof(rGBL));

    _afgIsOverflow[u1InputId]=FALSE;
     _au4MwPVRTickNum[u1InputId]=0;
     _au4MwPVRPktCount[u1InputId] = 0;
     _au4MwPVRPicIdx[u1InputId]=0;
     _afgMwPvrEnable[u1InputId]=FALSE;

    
    // we should not check this if dual tuner are recording two channels at the same time
    /*
        for(i = 0; i < FVR_NUM_PID_INDEX; i++)
        {
            if(MTFVR_GetPid(i, MTFVR_PID_FLAG_ENABLE, &rFvrPid) != MTR_OK)
            {
                return FALSE;
            }
            if(rFvrPid.fgEnable)
            {
                LOG(3, "There is PID enable\n");
                ASSERT(0);
            }
        }
        */
    

    for(i=0; i<FVR_SCRAMBLE_PID_INDEX; i++)
    {
        _arMwPvrPerPidxUsage[i].fgEnable = FALSE;
        _arMwPvrPerPidxUsage[i].u1Pidx = 0xFF;
    }

    //------------------------------------------
    // Buffer
    //------------------------------------------
    _au4MwPvrStreamPhyAddr[u1InputId] = (UINT32)prConfig->pv_buf_phy;
    _au4MwPvrStreamMwAddr[u1InputId] = (UINT32)prConfig->pv_buf_mw;
    _au4MwPvrMetaPhyAddr[u1InputId] = (UINT32)(prConfig->pv_buf_phy + (prConfig->z_buf_size - PVRMW_TICK_METABUF_SIZE));
    _au4MwPvrKelTotalSize[u1InputId] = prConfig->z_buf_size;

    LOG(6,"_MwPVRInitCtrl PHY: 0x%x MW: 0x%x Size: 0x%x\n", _au4MwPvrStreamPhyAddr[u1InputId], _au4MwPvrStreamMwAddr[u1InputId], _au4MwPvrKelTotalSize[u1InputId]);
    rGBL.fgSetAlign = TRUE;
    rGBL.u4BufAlign = 12288;//FVR_FIFO_ALIGNMENT; 192 and 4K align.4K is HDD sector size
    rGBL.u4BufStart = (UINT32)prConfig->pv_buf_phy;
    rGBL.u4BufSize = prConfig->z_buf_size - PVRMW_TICK_METABUF_SIZE;
    rGBL.u4TimeToNotify = prConfig->ui4_tick_period;
    rGBL.fgAllocateBuffer = FALSE;

    if(((rGBL.u4BufStart % FVR_FIFO_ALIGNMENT) != 0) || ((rGBL.u4BufSize % FVR_FIFO_ALIGNMENT) != 0))
    {
        return FALSE;
    }

    if(MTFVR_SetGBL(u1TsIdx, MTFVR_GBL_FLAG_BUFFER | MTFVR_GBL_FLAG_TIME_INFO, &rGBL) != MTR_OK)
    {
        return FALSE;
    }

    if((UINT8)prConfig->ui1_nfy_frm_mask != (UINT8)PVR_FRAME_TYPE_MASK_I)
    {
        // Current only support I mask
        return FALSE;
    }

#ifdef LINUX_TURNKEY_SOLUTION
    for(i=0; i<MW_FVR_META_BUF_NUM; i++)
    {
         _arMwPVRMetaBuf[u1InputId][i].u4BufAddr =
             _MwPvrKerlToUserAddr(u1InputId, _au4MwPvrMetaPhyAddr[u1InputId] + (i * (PVRMW_TICK_PIC_MAX * 4)));
    }
#endif //  LINUX_TURNKEY_SOLUTION

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MwPVRCtrl_Set
 *  Middleware interface - set
 *
 *  @retval RMR_OK if succeeded, failed otherwise
 */
//-----------------------------------------------------------------------------
static INT32 _MwPVRCtrl_Set(
                    DRV_COMP_ID_T*      pt_comp_id,
                    DRV_SET_TYPE_T      e_set_type,
                    const VOID*         pv_set_info,
                    SIZE_T              z_set_info_len)
{
    INT32 i4Ret = RMR_OK;
    BOOL fgEnable, fgRet, fgIsOverflow, fgIsRecording;
//    FVR_GBL_T rGBL;
//    MM_CRYPT_IV_T *pEncIV;
    UINT32 i;
    UINT8 u1TsIdx, u1InputId;

    if ((NULL == pt_comp_id))
    {
        return RMR_DRV_INV_SET_INFO;
    }

    // Check index range
    u1InputId = (UINT8)pt_comp_id->ui2_id;
    if(u1InputId >= MW_FVR_INPUT_NUM)
    {
        return RMR_DRV_INV_GET_INFO;
    }
    u1TsIdx = (UINT8)_arFvrInput[u1InputId].u4TsIdx;
    if (u1TsIdx >= MTDMX_NUM_TS_INDEX)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    //debug
    LOG(6, "_MwPVRCtrl_Set TsIdx: %d\n", u1TsIdx);

    switch (e_set_type)
    {
    case PVR_SET_TYPE_NFY_FCT:
        LOG(6,"%s PVR_SET_TYPE_NFY_FCT\n", __FUNCTION__);
        fgRet = _MwPVRSetCallback(u1InputId, (PVR_NFY_INFO_T*)pv_set_info);
        i4Ret = fgRet ? RMR_OK : RMR_INV_SET;
        break;

    case PVR_SET_TYPE_IV:
        ASSERT(0);
//        pEncIV = (MM_CRYPT_IV_T*)pv_set_info;
//        rGBL.u4CBCIVLow = pEncIV->aui4_init_vector[0];
//        rGBL.u4CBCIVHigh = pEncIV->aui4_init_vector[1];
//        fgRet = FVR_SetGBL(FVR_GBL_FLAG_CBC_IV, &rGBL);
//        i4Ret = fgRet ? RMR_OK : RMR_INV_SET;
        break;

    case PVR_SET_TYPE_CTRL:
        LOG(6,"%s PVR_SET_TYPE_CTRL\n", __FUNCTION__);
        fgEnable = (BOOL)(UINT32)pv_set_info;
        LOG(3, "%s, PVR_SET_TYPE_CTRL: %d\n", __FUNCTION__, (INT32)fgEnable);
        if(fgEnable)
        {
            for(i=0; i<MW_FVR_META_BUF_NUM; i++)
            {
                _arMwPVRMetaBuf[u1InputId][i].fgValid = FALSE;
                _arMwPVRMetaBuf[u1InputId][i].u4SerialNum = 0;

            }

             if ( (fgRet = ((MTFVR_Start(u1TsIdx) == MTR_OK)?TRUE:FALSE)) )
            {
                _MwPvrLock();
                _afgMwPvrEnable[u1InputId] = TRUE;
                _afgIsOverflow[u1InputId] = FALSE;
                _MwPvrUnlock();
            }
        }
        else
        {
             if ( (fgRet = ((MTFVR_Stop(u1TsIdx) == MTR_OK)?TRUE:FALSE)) )
            {
                _MwPvrLock();
                _afgMwPvrEnable[u1InputId] = FALSE;
                _MwPvrUnlock();
            }
        }
        i4Ret = fgRet ? RMR_OK : RMR_INV_SET;
        break;

    case PVR_SET_TYPE_UNLOCK_BUFFER:
        LOG(6,"%s PVR_SET_TYPE_UNLOCK_BUFFER\n", __FUNCTION__);
        i4Ret = _MwPVRUnlockBuffer(u1InputId, (PVR_UNLOCK_BUFFER_INFO_T*)pv_set_info);
        break;

    case PVR_SET_TYPE_FLUSH_BUFFER:
        LOG(6,"%s PVR_SET_TYPE_FLUSH_BUFFER\n", __FUNCTION__);
         if(MTFVR_IsRecording(u1TsIdx, &fgIsRecording) != MTR_OK)
        {
            return RMR_INV_SET;
        }

        if(fgIsRecording)
        {
            LOG(1, "%s, PVR_SET_TYPE_FLUSH_BUFFER still recording\n", __FUNCTION__);
            return RMR_INV_SET;
        }

        if(MTFVR_FlushBuffer(u1TsIdx) != MTR_OK)
        {
            return RMR_INV_SET;
        }
        _MwPvrLock();
        fgEnable = _afgMwPvrEnable[u1InputId];
        fgIsOverflow = _afgIsOverflow[u1InputId];
        _MwPvrUnlock();
        if(fgEnable)
        {
            if(!fgIsOverflow)
            {
                LOG(3, "Oops, should overflow\n");
            }
            LOG(3, "PVR flush buffer start...\n");
             if(MTFVR_Start(u1TsIdx) != MTR_OK)
            {
                return RMR_INV_SET;
            }
        }
        _MwPvrLock();
         _au4MwPVRPktCount[u1InputId] = 0;
        _afgIsOverflow[u1InputId] = FALSE;
        _MwPvrUnlock();

        i4Ret = RMR_OK;
        break;

    case PVR_SET_TYPE_UNLOCK_METADATA:
        LOG(6,"%s PVR_SET_TYPE_UNLOCK_METADATA\n", __FUNCTION__);
        LOG(6, "free meta: %d\n", (UINT32)pv_set_info);
        fgRet = _MwPVRFreeMetaBuf(u1InputId, (UINT8)(UINT32)pv_set_info);
        i4Ret = fgRet ? RMR_OK : RMR_INV_SET;
        break;

    case PVR_SET_TYPE_INIT_CTRL:
        LOG(6,"%s PVR_SET_TYPE_INIT_CTRL\n", __FUNCTION__);
        fgRet = _MwPVRInitCtrl(u1InputId, (PVR_CTRL_CONFIG_T*)pv_set_info);
        i4Ret = fgRet ? RMR_OK : RMR_INV_SET;
        break;

    case PVR_SET_TYPE_DEINIT_CTRL:
        LOG(6,"%s PVR_SET_TYPE_DEINIT_CTRL\n", __FUNCTION__);
        if(!_MW_DMX_CRYPT_ResetAllFvrKeySlots())
        {
            return RMR_INV_SET;
        }
        if(MTFVR_FreeGBL(u1TsIdx) != MTR_OK)
        {
            return RMR_INV_SET;
        }

        for(i=0; i<FVR_SCRAMBLE_PID_INDEX; i++)
        {
            _arMwPvrPerPidxUsage[i].fgEnable = FALSE;
            _arMwPvrPerPidxUsage[i].u1Pidx = 0xFF;
        }
        break;

    default:
        break;
    }

    LOG(6,"%s %d\n", __FUNCTION__, i4Ret);

    return i4Ret;
}

//-----------------------------------------------------------------------------
/** _MwPVRInit
 */
//-----------------------------------------------------------------------------
static BOOL _MwPVRInit(void)
{
    static BOOL _fgInit = FALSE;
    UINT32 i, j, u4Addr;

    LOG(3, "%s\n", __FUNCTION__);

    for(i = 0; i < MW_FVR_INPUT_NUM; i++)
    {
        if(MTFVR_Init(i) != MTR_OK)
        {
            return FALSE;
        }
    }

    //------------------------------------------------
    // Init global variables
    //------------------------------------------------
    if(!_MW_DMX_CRYPT_ResetAllFvrKeySlots())
    {
        return FALSE;
    }

    for(i = 0; i < MW_FVR_INPUT_NUM; i++)
    {
        _afgIsOverflow[i] = FALSE;
        _afgMwPvrEnable[i] = FALSE;
        _arMwPVRNotifyInfo[i].pf_nfy = NULL;
        _arMwPVRNotifyInfo[i].pv_tag = NULL;

        for(j=0; j<MW_FVR_META_BUF_NUM; j++)
        {
            _arMwPVRMetaBuf[i][j].fgValid = FALSE;
            _arMwPVRMetaBuf[i][j].u4SerialNum = 0;
        }
    }

    if(!_fgInit)
    {
        for(i = 0; i < MW_FVR_INPUT_NUM; i++)
        {
            _apu4MwPVRPicData[i] = (UINT32*)x_mem_alloc(PVRMW_TICK_PIC_MAX * 4);
            if(_apu4MwPVRPicData[i] == NULL)
            {
                return FALSE;
            }

            for(j=0; j<MW_FVR_META_BUF_NUM; j++)
            {
                u4Addr = (UINT32)x_mem_alloc(PVRMW_TICK_PIC_MAX * 4);
                if(u4Addr == 0)
                {
                    x_mem_free((void*)_apu4MwPVRPicData[i]);
                    return FALSE;
                }
                _arMwPVRMetaBuf[i][j].u4BufAddr = u4Addr;
            }
        }
        _fgInit = TRUE;
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** MW_PVR_Init
 *  Initialize pvr driver for middleware interface
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL MW_PVR_Init(void)
{
    ID_IND_T at_id_ind[PVR_STREAM_COMP_NUM];
    ID_IND_T at_ctrl_ind[MW_FVR_CTRL_NUM];
    DRV_COMP_REG_T t_comp_id;
    DRV_COMP_FCT_TBL_T t_comp_fct_tbl;
    UINT32 i, u4Flags;
    INT32 i4Ret;

    if(!_MwPVRInit())
    {
        return FALSE;
    }

    //-----------------------------------------------
    // Register PVR Stream components
    //-----------------------------------------------
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwPVRStream_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwPVRStream_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwPVRStream_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwPVRStream_Set;
    t_comp_id.e_type = DRVT_PVR_STREAM;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = PVR_STREAM_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind;

    for (i = 0; i < PVR_STREAM_COMP_NUM; i++)
    {
        t_comp_id.u.t_list.pt_list[i].ui2_id = i;
        t_comp_id.u.t_list.pt_list[i].pv_tag = NULL;
        t_comp_id.u.t_list.pt_list[i].ui1_port = ALL_PORTS;
    }

    u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

    i4Ret = x_rm_reg_comp(&t_comp_id,
                         1,                     // in port num
                         1,                     // out port num
                         NULL,                  // component name
                         u4Flags,               // flags
                         &t_comp_fct_tbl,
                         NULL,
                         0);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }

    //-----------------------------------------------
    // Register PVR PCR Stream components
    //-----------------------------------------------
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwPVRStreamPCR_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwPVRStreamPCR_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwPVRStreamPCR_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwPVRStreamPCR_Set;
    t_comp_id.e_type = DRVT_PVR_STREAM_PCR;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = PVR_STREAM_PCR_COMP_NUM;
    t_comp_id.u.t_list.pt_list = at_id_ind;

    for (i = 0; i < PVR_STREAM_COMP_NUM; i++)
    {
        t_comp_id.u.t_list.pt_list[i].ui2_id = i;
        t_comp_id.u.t_list.pt_list[i].pv_tag = NULL;
        t_comp_id.u.t_list.pt_list[i].ui1_port = ALL_PORTS;
    }

    u4Flags = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

    i4Ret = x_rm_reg_comp(&t_comp_id,
                         1,                     // in port num
                         1,                     // out port num
                         NULL,                  // component name
                         u4Flags,               // flags
                         &t_comp_fct_tbl,
                         NULL,
                         0);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }

    //-----------------------------------------------
    // Register PVR Control components
    //-----------------------------------------------
    t_comp_fct_tbl.pf_rm_connect = (x_rm_connect_fct)_MwPVRCtrl_Connect;
    t_comp_fct_tbl.pf_rm_disconnect = (x_rm_disconnect_fct)_MwPVRCtrl_Disconnect;
    t_comp_fct_tbl.pf_rm_get = (x_rm_get_fct)_MwPVRCtrl_Get;
    t_comp_fct_tbl.pf_rm_set = (x_rm_set_fct)_MwPVRCtrl_Set;
    t_comp_id.e_type = DRVT_PVR_CTRL;
    t_comp_id.e_id_type = ID_TYPE_LIST;
    t_comp_id.u.t_list.ui2_num_of_ids = MW_FVR_CTRL_NUM;
    t_comp_id.u.t_list.pt_list = at_ctrl_ind;

    for (i = 0; i < MW_FVR_CTRL_NUM; i++)
    {
        t_comp_id.u.t_list.pt_list[i].ui2_id = i;
        t_comp_id.u.t_list.pt_list[i].pv_tag = NULL;
        t_comp_id.u.t_list.pt_list[i].ui1_port = ALL_PORTS;
    }
    i4Ret = x_rm_reg_comp(&t_comp_id,
                         1,                     // in port num
                         1,                     // out port num
                         NULL,                  // component name
                         u4Flags,               // flags
                         &t_comp_fct_tbl,
                         NULL,
                         0);
    if (i4Ret != RMR_OK)
    {
        return FALSE;
    }

    return TRUE;
}

