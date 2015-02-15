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
 * $RCSfile: swdmx_mpg.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_ps.c
 *  Software demux for ps - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dmx_if.h"
#include "dmx_mm_if.h"
#include "vdec_drvif.h"
#include "aud_drvif.h"
#include "aud_if.h"
#include "swdmx_drvif.h"
#include "swdmx_ps.h"
#include "swdmx_mpg.h"
#include "swdmx_mpg_dmx.h"
#include "swdmx_mpg_pvr.h"
#include "swdmx_mpg_psipsr_if.h"
#include "fbm_drvif.h"
#include "x_util.h"
#include "swdmx_debug.h"
#include "swdmx_mpg_idx.h"
#include "stc_drvif.h"

#if defined(CC_MT5387) || defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
#include "b2r_drvif.h"
#include "b2r_if.h"
#endif

#ifdef SWDMX_DUMP_DATA
#include "swdmx_debug_util.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define SWDMX_PS_DMX_20
//#define HIGH_SPEED_STORAGE_USE_ACK_SIZE
#define NETWORK_USE_ACK_SIZE
//#define ENABLE_AUDIO_CONTROL

#define SEND_EOS_TIMES         2
#define PS_REQ_SIZE_20M        256 * 1024
#define PS_REQ_MOVE_RATE       8
#define MAX_ALLOW_FRAME_NUM    512
#define MAX_ALLOW_ADATA_SIZE   128 * 1024
#define MAX_EMSG_NUM           600
#define ONE_SECOND             90000

#define INVALID_VALUE(x)       (((UINT32)(x >> 32) == (UINT32)0xFFFFFFFF) \
                                && ((UINT32)(x) == (UINT32)0xFFFFFFFF))

#define IS_PUSH_MODE(x)        ((x == MM_SRC_TYPE_PUSH))

#define NEED_QUICK_START(x)    ((0))

#define USB_SOURCE(x)          ((x == MM_SRC_TYPE_HIGH_SPEED_STORAGE))

#define AUDIO_NOT_MPEG(x)      ((x == 0xBD))

/****define variables****/
VDEC_SEQUENCE_DATA_T psSeqInfo;

/****init variable****/
static VOID _SWDMX_PsVariableInit(SWDMX_MPG_INFO_T * prSwdmxPsInfo)
{
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    prSwdmxPsInfo->rVar.u1PidxVideo = 0xFF;
    prSwdmxPsInfo->rVar.u1PidxVideo2 = 0xFF;
    prSwdmxPsInfo->rVar.u1PidxAudio = 0xFF;
    prSwdmxPsInfo->rVar.fgFirstData = TRUE;
    prSwdmxPsInfo->rVar.fgFirst = TRUE;

    prSwdmxPsInfo->rFmt.u2AudioId = 0xFFFF;
    prSwdmxPsInfo->rFmt.u2AudioSubId = 0xFFFF;
    prSwdmxPsInfo->rVar.u2PrevAStrmID = 0xFFFF;
    prSwdmxPsInfo->rVar.u2PrevSubAStrmID = 0xFFFF;
    prSwdmxPsInfo->rVar.fgChgATrack = FALSE;
    prSwdmxPsInfo->rVar.u1TotalATrack = 0;

    prSwdmxPsInfo->rVar.i4CurSpeed = SWDMX_PLAY_SPEED_1X;
    prSwdmxPsInfo->rVar.u4OneTimeReadSize = PS_REQ_SIZE_20M;
    prSwdmxPsInfo->rVar.u4OneTimeSize = PS_REQ_SIZE_20M / PS_REQ_MOVE_RATE;
    prSwdmxPsInfo->rVar.u4AlignBase = 4;
    prSwdmxPsInfo->rVar.u4FeederReqID = 1;
    prSwdmxPsInfo->rVar.u4FeederBufNum = MPG_BUF_NUM;
    prSwdmxPsInfo->rVar.u4FeederReqNum = MPG_REQ_NUM;
    
    prSwdmxPsInfo->rVar.u4FirstVideoPTS = 0xFFFFFFFF;
    prSwdmxPsInfo->rVar.u4GetFirstAudioPTS = 0;
    
    prSwdmxPsInfo->rFmt.fgSeek = FALSE;
    prSwdmxPsInfo->rVar.u8SeekSkipPos = 0;
    prSwdmxPsInfo->rVar.u4LastAudioWptr = 0;
    prSwdmxPsInfo->rVar.fgBeginToPlay = FALSE;

    prSwdmxPsInfo->rVar.u8JumpStartTime = DLNA_TIME_SEEK_INVALID_TIME;
}

/****judge use req size or ack size****/
static inline BOOL _SWDMX_PsUseAckSize(MM_SRC_TYPE_T e_mm_src_type)
{
    if(IS_PUSH_MODE(e_mm_src_type))
    {
        return TRUE;
    }
    else if(e_mm_src_type == MM_SRC_TYPE_HIGH_SPEED_STORAGE)
    {
#ifdef HIGH_SPEED_STORAGE_USE_ACK_SIZE
        return TRUE;
#endif
        
    }
    else
    {        
#ifdef NETWORK_USE_ACK_SIZE
        return TRUE;
#endif
    }
    return FALSE;
}

/****reset the sended position****/
static inline VOID _SWDMX_PsSetSendedPos(SWDMX_MPG_INFO_T * prSwdmxPsInfo, 
    UINT64 u8SendedPos)
{
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    prSwdmxPsInfo->rVar.u8SendedSize = u8SendedPos;
    prSwdmxPsInfo->rVar.u8OrgSendedSize = u8SendedPos;
    prSwdmxPsInfo->rVar.u8ReadedPos = u8SendedPos;
    prSwdmxPsInfo->rVar.u8PreviousIPos = u8SendedPos;
    prSwdmxPsInfo->rVar.u8PreviousSendedSize = u8SendedPos;
}

/****get the size of audio fifo which not used****/
static inline UINT32 _SWDMX_PsGetFifoAvailSize(UINT8 u1DmxId, DMX_PID_TYPE_T e_type, UINT8 u1Pidx)
{
    UINT32 u4AvailSize = 0;    
#ifdef SWDMX_PS_DMX_20
    u4AvailSize = DMX_MUL_GetEmptySize(u1DmxId, e_type, u1Pidx);
#else
    u4AvailSize = DMX_GetMinFreeBufferSize(e_type);
#endif
    return u4AvailSize;
}

/****check vdec state****/
static BOOL _SWDMX_PsVdecIsStop(SWDMX_INFO_T *prSwdmxInst)
{
    UINT32 u4Status;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    u4Status = MPV_GetDecMode(prSwdmxInst->u1VdecId);
    if ((u4Status == VDEC_ST_STOP) || (u4Status == VDEC_ST_IDLE))
    {
        return TRUE;
    }
    if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
        (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
    {
        u4Status = MPV_GetDecMode(prSwdmxInst->u1Vdec2Id);
        if ((u4Status == VDEC_ST_STOP) || (u4Status == VDEC_ST_IDLE))
        {
            return TRUE;
        }
    }
    return FALSE;
}
    
static BOOL _SWDMX_PsWaitVdecReady(SWDMX_INFO_T *prSwdmxInst)
{
#define _LOOP_CNT    200
    UINT32 u4Cnt = 0;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    while (u4Cnt++ < _LOOP_CNT)
    {
        if (_SWDMX_PsVdecIsStop(prSwdmxInst))
        {
            x_thread_delay(10);
            LOG(1, "Vdec is stop state, waiting...\n");
        }
        else
        {
            return TRUE;
        }
    }
    
    LOG(0, "Vdec always is not ready, timeout.\n");
    return FALSE;
}

/****parse the video, mainly to wait audio info for sync****/
static BOOL _SWDMX_PsVideoPause(SWDMX_INFO_T * prSwdmxInst)
{   
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    /****pause vdec****/
    VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
    VDEC_Pause(prSwdmxInst->u1VdecId);
    if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
        (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
    {
        VDEC_Pause(prSwdmxInst->u1Vdec2Id);
    }    
    return TRUE;
}

/****get the video decoder type****/
static ENUM_VDEC_FMT_T _SWDMX_PsGetVideoType(ENUM_SWDMX_VDEC_T eVideoType)
{
    ENUM_VDEC_FMT_T e_vdec_frm = VDEC_FMT_MAX;
    
    switch(eVideoType)
    {        
        case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
            e_vdec_frm = VDEC_FMT_MPV;
            break;
        case ENUM_SWDMX_VDEC_H264:
            e_vdec_frm = VDEC_FMT_H264;
            break;
        default:
            LOG(0, "%s not support vdec type %d!\n", __FUNCTION__, eVideoType);
            break;
    } 
    return e_vdec_frm;
}

/****play the video, mainly to enable video decoder can to decode****/
static BOOL _SWDMX_PsVideoPlay(SWDMX_INFO_T * prSwdmxInst)
{
    ENUM_VDEC_FMT_T e_vdec_frm = VDEC_FMT_MAX;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    e_vdec_frm = _SWDMX_PsGetVideoType(prSwdmxInst->eVideoType);     
    LOG(1, "%s, vdec type = 0x%x\n", __FUNCTION__, e_vdec_frm);
    VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);
    if(!VDEC_Play(prSwdmxInst->u1VdecId, e_vdec_frm))
    {
        LOG(0, "vdec play fail 1\n");
        return FALSE; 
    }         
    if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
       (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
    {
        if(!VDEC_Play(prSwdmxInst->u1Vdec2Id, e_vdec_frm))
        {
            LOG(0, "vdec play fail 2\n");
            return FALSE;
        }
    }
    return TRUE; 
}

static BOOL _SWDMX_PsAudioPlay(SWDMX_INFO_T *prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if (!prSwdmxPsInfo->rVar.fgEnableAudio && prSwdmxPsInfo->rVar.fgGotAudioReady)
    {
        if (prSwdmxPsInfo->rVar.fgChgATrack && 
            (prSwdmxPsInfo->rVar.u4GetFirstAudioPTS > STC_GetStcValue(prSwdmxInst->u1StcId)))
        {
            LOG(1, "%s, Apts(0x%X) > STC(0x%X)\n", __FUNCTION__,
                    prSwdmxPsInfo->rVar.u4GetFirstAudioPTS,
                    STC_GetStcValue(prSwdmxInst->u1StcId));
            return FALSE;
        }
        
        STC_StartStc(prSwdmxInst->u1AdecId);
        AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
        prSwdmxPsInfo->rVar.fgEnableAudio = TRUE;
        prSwdmxPsInfo->rVar.u4GetFirstAudioPTS = 0;
        LOG(0, "%s enable audio output\n", __FUNCTION__);
        return TRUE;
    }
    return FALSE;
}

static BOOL _SWDMX_PsIsVFifoEmpty(SWDMX_INFO_T *prSwdmxInst)
{
    UINT16 u2QueueSize = 0, u2MaxQueueSize = 0;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if (prSwdmxPsInfo->rVar.fgGotVideo) 
    {
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2QueueSize, &u2MaxQueueSize);
    }
    if (u2QueueSize == 0)
    {
        return TRUE;
    }
    return FALSE; 
}

/****this is used to enable file can be play****/
static VOID _SWDMX_PsEnablePlay(SWDMX_INFO_T *prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    /****for video****/
    if (prSwdmxPsInfo->rFmt.fgVideo)
    {
        if (prSwdmxPsInfo->rVar.fgGotVideo && prSwdmxPsInfo->rVar.fgVideoPause)
        {
            if (_SWDMX_PsVideoPlay(prSwdmxInst))
            {
                prSwdmxPsInfo->rVar.fgVideoPause = FALSE;
                prSwdmxPsInfo->rVar.fgEnableVideo = TRUE;            
                LOG(1, "%s, cancel video pause\n", __FUNCTION__);
            }
            else
            {
                LOG(0, "%s, video play fail!\n", __FUNCTION__);
            }
        }
         
        if (!prSwdmxPsInfo->rVar.fgEnableAudio &&
            prSwdmxPsInfo->rVar.fgGotAudioReady &&
            _SWDMX_PsIsVFifoEmpty(prSwdmxInst))
        {
            LOG(1, "%s VFIFO is empty\n", __FUNCTION__);
            _SWDMX_PsAudioPlay(prSwdmxInst);
        }            
    }
    /****for audio only case****/
    else if (prSwdmxPsInfo->rFmt.fgAudio)
    {
        _SWDMX_PsAudioPlay(prSwdmxInst);
    }
}

/****get current play position****/
static BOOL _SWDMX_PsGetCurrentPlayPos(UINT8 u1SrcId, UINT64 * pu8PlayPos)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    UINT64 u8CurPos = 0;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    if((prSwdmxPsInfo->rVar.u4TotalTime == 0) ||
        (prSwdmxPsInfo->rVar.u8FileSize == 0xFFFFFFFF))
    {
        if(prSwdmxInst->u4LogFilter & check_getinfo)
        {
            LOG(1, "invalid filesize(0x%llX) or totaltime(0x%X)\n",
                prSwdmxPsInfo->rVar.u8FileSize,
                prSwdmxPsInfo->rVar.u4TotalTime);
        }
        return FALSE;
    }
    if(prSwdmxPsInfo->rFmt.fgVideo)
    {
        UINT64 offset = 0;
        UINT64 offset_i = 0;
        INT32 temp_ref = 0;
        UINT16 decode_order = 0;
        if(VDP_GetFilePosition(prSwdmxInst->u1B2rId, &temp_ref, 
            &decode_order, &offset, &offset_i))
        {
            if(offset > prSwdmxPsInfo->rVar.u8FileSize)
            {
                if(prSwdmxInst->u4LogFilter & check_getinfo)
                {
                    LOG(1, "offset(0x%X) > filesize(0x%X)\n",
                        offset, prSwdmxPsInfo->rVar.u8FileSize);
                }
                u8CurPos = prSwdmxPsInfo->rVar.u8FileSize;            
            }
            else
            {
                u8CurPos = offset; 
            }
        }
        else
        {
            if(prSwdmxInst->u4LogFilter & check_getinfo)
                LOG(1, "get position from vdp fail!\n");
            return FALSE;
        }
    }
    else if(prSwdmxPsInfo->rFmt.fgAudio)
    {
        UINT32 u4Addr = 0;
        UINT32 u4Size = 0;
        UINT32 u4DataSize = 0;

        if(!_SWDMX_GetAudioFifo(u1SrcId, &u4Addr, &u4Size))
        {
            if(prSwdmxInst->u4LogFilter & check_getinfo)
                LOG(0, "line = %d, _SWDMX_GetAudioFifo fail\n", __LINE__);
            return FALSE;
        }
        u4DataSize = _SWDMX_PsGetFifoAvailSize(prSwdmxInst->u1DmxId, 
                DMX_PID_TYPE_ES_AUDIO, 
                prSwdmxPsInfo->rVar.u1PidxVideo);
        if(u4Size > u4DataSize)
            u4DataSize = u4Size - u4DataSize;
        else
            u4DataSize = 0;
        if(prSwdmxPsInfo->rVar.u8SendedSize > u4DataSize)
        {
            u8CurPos = prSwdmxPsInfo->rVar.u8SendedSize - u4DataSize;
        }                
    }
    if(u8CurPos < prSwdmxPsInfo->rVar.u8StartFileOffset)
    {
        u8CurPos = prSwdmxPsInfo->rVar.u8StartFileOffset;
    }
    *pu8PlayPos = u8CurPos;
    return TRUE;
}

/****this is used for send eos to vdec to stop vdec****/
static BOOL _SWDMX_PsSendVEos(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    VDEC_PES_INFO_T pes_info;
    INT32 i4SendTimes = 0;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    prSwdmxPsInfo = &(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo);
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    // imit pes_info
    x_memset(&pes_info, 0, sizeof(VDEC_PES_INFO_T));
    
    pes_info.fgEos = TRUE;
    pes_info.ucMpvId = VLD0;
    pes_info.ucEsId = prSwdmxInst->u1VdecId;
    pes_info.fgSeqHdr = TRUE;
    pes_info.u4FifoStart = (UINT32)NULL;
    pes_info.u4FifoEnd = (UINT32)NULL;
    pes_info.u4VldReadPtr = (UINT32)NULL;
    if(prSwdmxPsInfo->rVar.i4CurSpeed < 0)
    {
        pes_info.fgBackward = TRUE;
    }
    else
    {
        pes_info.fgBackward = FALSE;
    }
    do
    {
        VDEC_SendEs((void*)&pes_info);
    } while(++i4SendTimes < SEND_EOS_TIMES);
    prSwdmxPsInfo->rVar.fgSendVideoEos = TRUE;
    LOG(1, "Send video EOS, fgBackward(%d)\n", pes_info.fgBackward);
    if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
        (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
    {
        pes_info.ucEsId = prSwdmxInst->u1Vdec2Id;
        do
        {
            VDEC_SendEs((void*)&pes_info);
        } while(++i4SendTimes < SEND_EOS_TIMES);
        prSwdmxPsInfo->rVar.fgSendVideo2Eos = TRUE;
        LOG(1, "Send video 2 EOS, fgBackward(%d)\n", pes_info.fgBackward);
    }
    return TRUE;
}

/****this is used for send eos to vdec to stop adec****/
static BOOL _SWDMX_PsSendAEos(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    DMX_AUDIO_PES_T pes_info;
    UINT32 buf_start[2], buf_end[2];
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    prSwdmxPsInfo = &(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo);
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    // init pes_info    
    x_memset(&pes_info, 0, sizeof(DMX_AUDIO_PES_T));

    if (prSwdmxPsInfo->rVar.u4LastAudioWptr)
    {
        pes_info.u4Wp = prSwdmxPsInfo->rVar.u4LastAudioWptr;
    }
    else
    {
#ifdef AUD_OLDFIFO_INTF
        VERIFY(AUD_GetAudFifo(&buf_start[0], &buf_end[0],
            &buf_start[1], &buf_end[1]) == AUD_OK);
#else
        VERIFY(AUD_GetAudFifo(prSwdmxInst->u1AdecId, &buf_start[0], &buf_end[0]) == AUD_OK);
#endif    
        pes_info.u4Wp = (buf_start[0] + 4); 
        pes_info.fgEOS = TRUE;
    }
    
    UNUSED(AUD_SendAudioPes(&pes_info));
    if(prSwdmxPsInfo->rVar.i4CurSpeed >= 0)
    {
        pes_info.fgForward = TRUE;
    }
    else
    {
        pes_info.fgForward = FALSE;
    }

    pes_info.fgEOS = TRUE;
    UNUSED(AUD_SendAudioPes(&pes_info));
    prSwdmxPsInfo->rVar.fgSendAudioEos = TRUE;    
    LOG(0, "Send audio eos, fgForward(%d)\n", pes_info.fgForward);
    return TRUE;
}

/****recieve vdec callback****/
VOID _SWDMX_PsSeekFinNotify(UINT8 u1SrcId, UINT32 u4PTS, BOOL fgAgain)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    prSwdmxPsInfo->rVar.fgJumpNext = TRUE;    
    if(prSwdmxInst->u4LogFilter & check_skip_mode)
    {
        UINT16 u2DecodingOrder = 0;    
        INT32 i4TempRef = 0;
        UINT64 u8Offset = 0;
        UINT64 u8OffsetI = 0;
        VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder, 
                    &u8Offset, &u8OffsetI);
        LOG(0, "Play pos(0x%llX) I pos(0x%llX)\n", u8Offset, u8OffsetI);
    }
    UNUSED(u4PTS);
    UNUSED(fgAgain);
}

/****this is judge the file is all sended or not****/
static BOOL _SWDMX_PsFileEnd(SWDMX_INFO_T * prSwdmxInst, UINT32 u4SendSize)
{
    BOOL fgRet = FALSE;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    if(prSwdmxPsInfo->rVar.fgSendEOF)
    {
        fgRet = TRUE;
    }
    else if((prSwdmxInst->eSeekType == eSWDMX_TYPE_BYTE_SEEK) &&
        ((UINT32)(prSwdmxPsInfo->rVar.u8FileSize >> 32) != (UINT32)0xFFFFFFFF) &&
        ((UINT32)(prSwdmxPsInfo->rVar.u8FileSize) != (UINT32)0xFFFFFFFF))
    {
        if((prSwdmxPsInfo->rVar.i4CurSpeed >= 0) &&
            (prSwdmxPsInfo->rVar.u8SendedSize + u4SendSize >= prSwdmxPsInfo->rVar.u8FileSize))
        {
            LOG(0, "Found file end by file size!\n");
            fgRet = TRUE;
        }
        else if((prSwdmxPsInfo->rVar.i4CurSpeed < 0) &&
            (prSwdmxPsInfo->rVar.u8SendedSize <= prSwdmxPsInfo->rVar.u8StartFileOffset))
        {
            LOG(0, "Found file end 2 by file size!\n");
            fgRet = TRUE;
        }
    }
    // get eos from mw, and no data to move, end file
    if((prSwdmxPsInfo->rBuf.fgGetEof) &&
        (prSwdmxPsInfo->rBuf.u4UsingBufSize <= u4SendSize) &&
        (!prSwdmxPsInfo->rBuf.u4BufAvailCnt))
    {
        LOG(0, "Found file end by feeder eof!\n");
        fgRet = TRUE;
    }
    return fgRet;
}

/****this is used to flush audio fifo****/
static VOID _SWDMX_PsFlushAudio(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(prSwdmxPsInfo->rFmt.fgAudio && (prSwdmxPsInfo->rVar.u1PidxAudio != 0xFF))
    {
        if(prSwdmxPsInfo->rVar.u4LastAudioWptr != 0)
        {            
            if(!(DMX_UpdateReadPointer(prSwdmxPsInfo->rVar.u1PidxAudio, 
                prSwdmxPsInfo->rVar.u4LastAudioWptr, 
                prSwdmxPsInfo->rVar.u4LastAudioWptr)))
            {
                if(prSwdmxInst->u4LogFilter & check_data_flush)
                    LOG(5, "%s DMX_UpdateReadPointer Fail\n", __FUNCTION__);
            }
            prSwdmxPsInfo->rVar.u4LastAudioWptr = 0;
        }
        UNUSED(DMX_MM_FlushBuffer(prSwdmxPsInfo->rVar.u1PidxAudio));
    }
}

/****this is used to flush first video fifo****/
static VOID _SWDMX_PsFlushVideo(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(prSwdmxPsInfo->rFmt.fgVideo && (prSwdmxPsInfo->rVar.u1PidxVideo != 0xFF))
    {
        if(prSwdmxPsInfo->rVar.u4LastVideoWptr != 0)
        {            
            if(!(DMX_UpdateReadPointer(prSwdmxPsInfo->rVar.u1PidxVideo, 
                prSwdmxPsInfo->rVar.u4LastVideoWptr, 
                prSwdmxPsInfo->rVar.u4LastVideoWptr)))
            {                
                if(prSwdmxInst->u4LogFilter & check_data_flush)
                    LOG(5, "%s DMX_UpdateReadPointer Fail\n", __FUNCTION__);
            }
            prSwdmxPsInfo->rVar.u4LastVideoWptr = 0;
        }
        UNUSED(DMX_MM_FlushBuffer(prSwdmxPsInfo->rVar.u1PidxVideo));
        MPV_FlushEsmQ(prSwdmxInst->u1VdecId, FALSE);
        UNUSED(VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId));
    }
}

/****this is used to flush second video fifo****/
static VOID _SWDMX_PsFlushVideo2(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(prSwdmxPsInfo->rFmt.fgVideo2 && (prSwdmxPsInfo->rVar.u1PidxVideo2 != 0xFF))
    {
        if(prSwdmxPsInfo->rVar.u4LastVideo2Wptr != 0)
        {            
            if(!(DMX_UpdateReadPointer(prSwdmxPsInfo->rVar.u1PidxVideo2, 
                prSwdmxPsInfo->rVar.u4LastVideo2Wptr, 
                prSwdmxPsInfo->rVar.u4LastVideo2Wptr)))
            {                
                if(prSwdmxInst->u4LogFilter & check_data_flush)
                    LOG(5, "%s DMX_UpdateReadPointer Fail\n", __FUNCTION__);
            }
            prSwdmxPsInfo->rVar.u4LastVideo2Wptr = 0;
        }
        UNUSED(DMX_MM_FlushBuffer(prSwdmxPsInfo->rVar.u1PidxVideo2));
        MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, FALSE);
        UNUSED(VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id));
    }
}

/****this is used to flush second video fifo****/
static BOOL _SWDMX_PsResetFeeder(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    UINT32 u4ReqCnt = 0;
    UINT32 u4AlignSize = 0;
    FEEDER_TRANSMIT_COND rFeederCond;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) !=
      FEEDER_E_OK)
    {
        if(prSwdmxInst->u4LogFilter & check_data_flush)
            LOG(0, "%s feeder input buffer error.\n", __FUNCTION__);
        return FALSE;
    }
    
    prSwdmxPsInfo->rVar.u4FeederReqID += 1000; // always a even value
    prSwdmxPsInfo->rBuf.u4BufAvailCnt = 0;
    prSwdmxPsInfo->rBuf.u4BufReadIdx = 0;
    prSwdmxPsInfo->rBuf.u4BufWriteIdx = 0;
    prSwdmxPsInfo->rBuf.u4BufTotalSize = 0;
    prSwdmxPsInfo->rBuf.u4ReqTimes = 0;
    prSwdmxPsInfo->rBuf.u4ReqReadIdx = 0;
    prSwdmxPsInfo->rBuf.u4ReqWriteIdx = 0;
    prSwdmxPsInfo->rBuf.u4ReqTotalSize = 0;
    prSwdmxPsInfo->rBuf.u4ReqTimeoutCount = 0;
    prSwdmxPsInfo->rBuf.u4ReqBufFullCount = 0;
    prSwdmxPsInfo->rBuf.u4UsingBufSize = 0;
    prSwdmxPsInfo->rVar.u8ReadedPos = prSwdmxPsInfo->rVar.u8SendedSize;

    for(u4ReqCnt = 0;u4ReqCnt < prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.u4FeederReqNum;u4ReqCnt++)
    {
        // make it invalid, not a even value
        prSwdmxPsInfo->rBuf.au4ReqId[u4ReqCnt] = 1;
    }

    #if 1//def CC_DLNA_SUPPORT
    FeederSetReadyThreshold(prSwdmxInst->eFeederSrc, MPG_DLNA_FEEDER_UNREACH);
    FeederSetUnderFlowThreshold(prSwdmxInst->eFeederSrc, MPG_DLNA_FEEDER_UNREACH);
    FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_THRESHOLD, FALSE);
    #endif
    // Update read ptr to feeder
    if(!IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type))
    {
        if(prSwdmxInst->rFeederInfo.u4WriteAddr)
        {
            if(FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, prSwdmxInst->rFeederInfo.u4WriteAddr) != FEEDER_E_OK)
            {
                if(prSwdmxInst->u4LogFilter & check_data_flush)
                    LOG(0, "%s FeederUpdateReadPointer fail\n", __FUNCTION__);
            }
        }
    }
    /**** DDI is 4 byte alignment****/
    u4AlignSize = (prSwdmxInst->rFeederInfo.u4WriteAddr % 4);
    if(u4AlignSize > 0)
    {
       if(prSwdmxInst->u4LogFilter & check_data_flush)
        {
            LOG(5, "%s Fd align u4WriteAddr %d\n",
                prSwdmxInst->rFeederInfo.u4WriteAddr, 
                __FUNCTION__);
        }
        if(FeederSetRequest(prSwdmxInst->eFeederSrc,
            (4 - u4AlignSize),
            (UINT64)(0),
            &rFeederCond,
            (prSwdmxPsInfo->rVar.u4FeederReqID - 25)) != FEEDER_E_OK)
        {
            if(prSwdmxInst->u4LogFilter & check_data_flush)
                LOG(0, "%s Fd align FeederSetRequest Fail\n", __FUNCTION__);
            return FALSE;
        }
    }
    return TRUE;
}

/****this is used to reset feeder & a/v fifo****/
static VOID _SWDMX_PsResetData(SWDMX_INFO_T * prSwdmxInst, BOOL fgResetFlag)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);
    LOG(1, "%s\n", __FUNCTION__);
    prSwdmxPsInfo = &(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo);
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    //reset video
    if(prSwdmxPsInfo->rFmt.fgVideo && (prSwdmxPsInfo->rVar.u1PidxVideo != 0xFF))
    {
        _SWDMX_PsFlushVideo(prSwdmxInst);
    }
    if(prSwdmxPsInfo->rFmt.fgVideo2 && (prSwdmxPsInfo->rVar.u1PidxVideo2 != 0xFF))
    {
        _SWDMX_PsFlushVideo2(prSwdmxInst);
    }
    //reset audio
    if(prSwdmxPsInfo->rFmt.fgAudio && (prSwdmxPsInfo->rVar.u1PidxAudio != 0xFF))
    {
        _SWDMX_PsFlushAudio(prSwdmxInst);
    }
    //flush req msg Q
    _SWDMX_FlushReqMsgQ(prSwdmxInst->u1SwdmxId);
    //reset feeder
    _SWDMX_PsResetFeeder(prSwdmxInst);
    //reset flag
    if(fgResetFlag)
    {
        //clear eos flag
        prSwdmxPsInfo->rVar.fgSendEOF = FALSE;
        prSwdmxPsInfo->rVar.fgSendAudioEos = FALSE;
        prSwdmxPsInfo->rVar.fgSendVideoEos = FALSE;
        prSwdmxPsInfo->rVar.fgSendVideo2Eos = FALSE;
        prSwdmxPsInfo->rBuf.fgGetEof = FALSE;
        //clear video first pts
        prSwdmxPsInfo->rVar.u4FirstVideoPTS = 0xFFFFFFFF;
        //clear first audio modify pts for trigger pts
        prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS = 0xFFFFFFFF;
        //clear a/v ready flag for start to play
        prSwdmxPsInfo->rVar.fgGotAudio = FALSE;
        prSwdmxPsInfo->rVar.fgGotVideo = FALSE;
        prSwdmxPsInfo->rVar.fgGotAudioReady = FALSE;
        prSwdmxPsInfo->rVar.fgVideoPause = FALSE;
        prSwdmxPsInfo->rVar.fgEnableAudio = FALSE;
        prSwdmxPsInfo->rVar.fgEnableVideo = FALSE;        
        //reset first flag
        prSwdmxPsInfo->rVar.fgFirst = TRUE;
        x_thread_delay(10);
    }    
    //change range status
    if(prSwdmxPsInfo->rVar.prCurRangeReq)
    {
        prSwdmxPsInfo->rVar.prCurRangeReq->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
}

/****do flush****/
BOOL _SWDMX_PsFlush(UINT8 u1SrcId)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    LOG(1, "%s\n", __FUNCTION__);
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    //reset video
    if(prSwdmxPsInfo->rFmt.fgVideo && (prSwdmxPsInfo->rVar.u1PidxVideo != 0xFF))
    {
        _SWDMX_PsFlushVideo(prSwdmxInst);
    }
    if(prSwdmxPsInfo->rFmt.fgVideo2 && (prSwdmxPsInfo->rVar.u1PidxVideo2 != 0xFF))
    {
        _SWDMX_PsFlushVideo2(prSwdmxInst);
    }
    //reset audio
    if(prSwdmxPsInfo->rFmt.fgAudio && (prSwdmxPsInfo->rVar.u1PidxAudio != 0xFF))
    {
        _SWDMX_PsFlushAudio(prSwdmxInst);
    }
    //flush req msg Q
    _SWDMX_FlushReqMsgQ(prSwdmxInst->u1SwdmxId);
    //reset feeder
    _SWDMX_PsResetFeeder(prSwdmxInst);
    return TRUE;
}

/****this is to handle eos when file end playing****/
static VOID _SWDMX_PsHandleEos(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);  
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    _SWDMX_PsEnablePlay(prSwdmxInst);
    
    if (prSwdmxPsInfo->rFmt.fgVideo && !prSwdmxPsInfo->rVar.fgSendVideoEos)
    {
        _SWDMX_PsSendVEos(prSwdmxInst);
    }
    
    if (prSwdmxPsInfo->rFmt.fgAudio && !prSwdmxPsInfo->rVar.fgSendAudioEos)
    {
        _SWDMX_PsSendAEos(prSwdmxInst);
    }
    
    if (!prSwdmxPsInfo->rVar.fgSendEOF)
    {
        prSwdmxPsInfo->rVar.fgSendEOF = TRUE;
        
        if (prSwdmxInst->rCurRangeReq.prDmxRangeInfo &&
            IS_NETWORK_SRC(prSwdmxInst->rCurRangeReq.prDmxRangeInfo->e_mm_src_type))
        {
            prSwdmxInst->fgRcvEOS = TRUE;
        }
    }

    return ;
}


/****this is used to set render position to vdec****/
static VOID _SWDMX_PsRenderPos(SWDMX_INFO_T * prSwdmxInst, UINT64 frame_pos, 
    UINT64 iframe_pos, UINT32 tmp_ref, UINT16 dec_order, UINT32 pts)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    BOOL fgI = TRUE;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(iframe_pos == (UINT64)(-1))
    {
        fgI = FALSE;            
        /****set render position****/
        {
            UNUSED(VDEC_SetRenderFromPos(prSwdmxInst->u1VdecId, FALSE, 0, 0, 0));
            UNUSED(VDEC_SetRenderBackStep(prSwdmxInst->u1VdecId, FALSE, 0, 0, 0));
            UNUSED(VDEC_SetRenderFromFirstPic(prSwdmxInst->u1VdecId, TRUE,
                frame_pos));
            if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
               (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
            {
                UNUSED(VDEC_SetRenderFromPos(prSwdmxInst->u1Vdec2Id, FALSE, 0, 0, 0));
                UNUSED(VDEC_SetRenderBackStep(prSwdmxInst->u1Vdec2Id, FALSE, 0, 0, 0));
                UNUSED(VDEC_SetRenderFromFirstPic(prSwdmxInst->u1Vdec2Id, TRUE,
                    frame_pos));
            }
        }
    }
    else
    {
        /****set render position****/
        {
            UNUSED(VDEC_SetRenderFromPos(prSwdmxInst->u1VdecId, TRUE,
                frame_pos, tmp_ref, dec_order));
            UNUSED(VDEC_SetRenderBackStep(prSwdmxInst->u1VdecId, FALSE, 0, 0, 0));
            UNUSED(VDEC_SetRenderFromFirstPic(prSwdmxInst->u1VdecId, FALSE,
                prSwdmxPsInfo->rVar.u8FileSize));
            if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
               (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
            {
                UNUSED(VDEC_SetRenderFromPos(prSwdmxInst->u1Vdec2Id, TRUE,
                    frame_pos, tmp_ref, dec_order));
                UNUSED(VDEC_SetRenderBackStep(prSwdmxInst->u1Vdec2Id, FALSE, 0, 0, 0));
                UNUSED(VDEC_SetRenderFromFirstPic(prSwdmxInst->u1Vdec2Id, FALSE,
                    prSwdmxPsInfo->rVar.u8FileSize));
            }            
            prSwdmxPsInfo->rVar.u4PreviousPTS = pts
                + prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.u4FirstPTS
                - MPG_STC_BASE;
        }
    }   
    LOG(1, "%s set rend position(0x%llX) pts(0x%X) tmp_ref(%d) dec_order(%d), fgI(%d)\n",
        __FUNCTION__, frame_pos, pts, tmp_ref, dec_order, fgI);
}
        
static BOOL _SWDMX_PsHandleLastMemory(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo->rVar.prCurRangeReq, FALSE);

    if ((prSwdmxPsInfo->rVar.prCurRangeReq->fgSetRenderPosition) &&
        (((UINT32)((prSwdmxPsInfo->rVar.prCurRangeReq->u8Offset) >> 32) != (UINT32)0xFFFFFFFF) ||
         ((UINT32)(prSwdmxPsInfo->rVar.prCurRangeReq->u8Offset) != (UINT32)0xFFFFFFFF)) &&
        (prSwdmxPsInfo->rVar.prCurRangeReq->u8Offset > prSwdmxPsInfo->rVar.u8StartFileOffset))
    {
        LOG(0, "[%s](%d) Pos case, u8Offset(%llX) u8OffsetI(%llX)!\n",
                __FUNCTION__, __LINE__,
                prSwdmxPsInfo->rVar.prCurRangeReq->u8Offset,
                prSwdmxPsInfo->rVar.prCurRangeReq->u8OffsetI); 

        if (prSwdmxPsInfo->rVar.prCurRangeReq->u8OffsetI == (UINT64)(-1))
        {
            _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.prCurRangeReq->u8Offset);
        }
        else
        {
            _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.prCurRangeReq->u8OffsetI);
        }

        _SWDMX_PsRenderPos(prSwdmxInst,
                prSwdmxPsInfo->rVar.prCurRangeReq->u8Offset,
                prSwdmxPsInfo->rVar.prCurRangeReq->u8OffsetI,
                prSwdmxPsInfo->rVar.prCurRangeReq->i4TempRef,
                prSwdmxPsInfo->rVar.prCurRangeReq->u2DecodingOrder,
                prSwdmxPsInfo->rVar.prCurRangeReq->u8StartPts);

        return TRUE;
    }
    else if ((prSwdmxPsInfo->rVar.prCurRangeReq->u4CurPTS) &&
             (prSwdmxPsInfo->rVar.u8FileSize != 0xFFFFFFFF) &&
             (prSwdmxPsInfo->rVar.u4TotalTime != 0))
    {
        UINT64 u8SeekPos = 0; 

        LOG(0, "[%s](%d) Pts case, u4CurPTS(%X)!\n", __FUNCTION__, __LINE__,
                prSwdmxPsInfo->rVar.prCurRangeReq->u4CurPTS); 

        u8SeekPos = prSwdmxPsInfo->rVar.u8FileSize - prSwdmxPsInfo->rVar.u8StartFileOffset;
        u8SeekPos = _SWDMX_Div6464(u8SeekPos * prSwdmxPsInfo->rVar.prCurRangeReq->u4CurPTS,
                                   prSwdmxPsInfo->rVar.u4TotalTime,
                                   NULL);
        u8SeekPos += prSwdmxPsInfo->rVar.u8StartFileOffset;

        _SWDMX_PsSetSendedPos(prSwdmxPsInfo, u8SeekPos);
        _SWDMX_PsRenderPos(prSwdmxInst, prSwdmxPsInfo->rVar.u8FileSize, (UINT64)(-1), 0, 0, 0);

        return TRUE;
    }
    
    return FALSE;
}

/****handle speed change to 1x****/
static VOID _SWDMX_PsJumpNormal(SWDMX_INFO_T * prSwdmxInst, BOOL fgSeek)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    prSwdmxPsInfo->rVar.fgJumpNext = FALSE;
    prSwdmxPsInfo->rVar.fgJumpping = FALSE;
    LOG(0, "%s, fgSeek(%d), fgVideo(%d)\n",
        __FUNCTION__,
        fgSeek, 
        prSwdmxPsInfo->rFmt.fgVideo);
    if(fgSeek) return;    
    STC_StopStc(prSwdmxInst->u1StcId);
    if(!prSwdmxPsInfo->rFmt.fgVideo)
    {        
        prSwdmxPsInfo->rVar.fgGotAudio = FALSE;
        prSwdmxPsInfo->rVar.fgGotAudioReady = FALSE;
        prSwdmxPsInfo->rVar.fgEnableAudio = FALSE;            
    }    
    else 
    {
        if(prSwdmxInst->eSeekType == eSWDMX_TYPE_BYTE_SEEK)
        {
            UINT64 frame_pos = 0;
            UINT64 iframe_pos = 0;
            INT32 tmp_ref = 0;
            UINT16 dec_order = 0; 
            VDP_PIC_INFO_T rPicInfo;
            
            x_memset(&rPicInfo, 0, sizeof(VDP_PIC_INFO_T));
            
            if(!VDP_GetFilePosition(prSwdmxInst->u1B2rId, &tmp_ref, 
                &dec_order, &frame_pos, &iframe_pos))
            {
                LOG(0, "%s, get pos from vdp fail!\n", __FUNCTION__);
                iframe_pos = frame_pos = prSwdmxPsInfo->rVar.u8SendedSize;                               
            }
           
            if (VDP_GetPicInfo(prSwdmxInst->u1B2rId, &rPicInfo))
            {
                STC_SetStcValue(prSwdmxInst->u1StcId, rPicInfo.u4Pts);
                STC_StartStc(prSwdmxInst->u1StcId);

                LOG(1, "%s: set STC value (0x%x)\n", __FUNCTION__, rPicInfo.u4Pts);
            }
            
            _SWDMX_PsResetData(prSwdmxInst, 1);    
            LOG(1, "%s, frame_pos(0x%X), iframe_pos(0x%X)!\n",
                __FUNCTION__,
                frame_pos,
                iframe_pos);
            _SWDMX_PsRenderPos(prSwdmxInst, frame_pos, iframe_pos, 
                tmp_ref, dec_order, STC_GetStcValue(prSwdmxInst->u1StcId));
            if(iframe_pos == (UINT64)(-1))
            {
                iframe_pos = frame_pos;
            }
            _SWDMX_PsSetSendedPos(prSwdmxPsInfo, iframe_pos);
        }
        else if(prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
        {
            UINT32 u4CurPts = 0;
            
            u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
            _SWDMX_PsResetData(prSwdmxInst, 1);
            prSwdmxPsInfo->rVar.u8JumpStartTime = u4CurPts;
            LOG(1, "%s, u4CurPts(0x%X)\n", __FUNCTION__, u4CurPts);
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
            _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8StartFileOffset);
        }
        else
        {
            LOG(0, "%s cannot support this eSeekType(%d)\n",
                __FUNCTION__,
                prSwdmxInst->eSeekType);
            ASSERT(0);
        }
    }
}

/****get a pic size****/
static UINT32 _SWDMX_PsGetSkipSize(SWDMX_INFO_T * prSwdmxInst, UINT32 u4SkipTime, 
    UINT32 u4Speed)
{
    UINT64 u8Tmp = 0;
    UINT64 u8Size = 0;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxInst, 0); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, 0);

    /****one pic size****/
    if((prSwdmxPsInfo->rVar.u4TotalTime > 90000) && 
        !INVALID_VALUE(prSwdmxPsInfo->rVar.u8FileSize))
    {
        /****total frames****/
        u8Tmp = (UINT64)((prSwdmxPsInfo->rVar.u4TotalTime + prSwdmxPsInfo->rVar.u4TotalTimeOffset) / 3000);
        if(u8Tmp > 0) // prevent div zero
        {
            u8Tmp = _SWDMX_Div6464((prSwdmxPsInfo->rVar.u8FileSize - 
                        prSwdmxPsInfo->rVar.u8StartFileOffset), u8Tmp, NULL);
        }
        else
        {
            u8Tmp = 1;
        }
    }
    else
    {
        u8Tmp = (UINT64)VDEC_GetAvgPicSize(prSwdmxInst->u1VdecId);
    }
    u8Size = u8Tmp; // average size pre frame
    u8Size *= u4Speed;
    if(u4SkipTime > 10000) // 10 ms
    {       
        if(prSwdmxPsInfo->rVar.u4TotalTime > 0)
        {
            u8Tmp = (u4SkipTime/100);
            u8Tmp *= 9;
            u8Tmp = (u8Tmp * (prSwdmxPsInfo->rVar.u8FileSize - prSwdmxPsInfo->rVar.u8StartFileOffset)); //1000000 / 90000 -> 100/9
            u8Tmp = _SWDMX_Div6464(u8Tmp,
                ((UINT64)prSwdmxPsInfo->rVar.u4TotalTime + (UINT64)prSwdmxPsInfo->rVar.u4TotalTimeOffset),
                NULL);
            u8Tmp *= u4Speed; // we should also consider the speed.
            u8Size += u8Tmp;
        }
        else //DTV00212070
        {
            u8Tmp *= 30;
            u8Tmp *= u4SkipTime;
            u8Tmp = _SWDMX_Div6464(u8Tmp, (UINT64)1000000, NULL);
            u8Tmp *= u4Speed; // we should also consider the speed.
            u8Size += u8Tmp;
        }
    }
    return (UINT32)_SWDMX_Div6464(u8Size, (UINT64)1000, NULL);
}

/****play one frame****/
static VOID _SWDMX_PsPlayOneFrame(SWDMX_INFO_T * prSwdmxInst)
{
    ENUM_VDEC_FMT_T eVdecFmt = VDEC_FMT_MPV;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    UNUSED(VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE));
    
    eVdecFmt = _SWDMX_PsGetVideoType(prSwdmxInst->eVideoType);  
    
    if(!VDEC_PlayOneFrm(prSwdmxInst->u1VdecId, eVdecFmt))
    {    
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
            LOG(0, "%s %d fail\n", __FUNCTION__, eVdecFmt);
    }
    if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
       (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
    {
        if(!VDEC_PlayOneFrm(prSwdmxInst->u1Vdec2Id, eVdecFmt))
        {           
           if(prSwdmxInst->u4LogFilter & check_skip_mode)
               LOG(0, "%s %d fail 2\n", __FUNCTION__, eVdecFmt);
        }
    }
}

/****do skip by pos****/
static VOID _SWDMX_PsDoSkipByPos(SWDMX_INFO_T * prSwdmxInst, INT32 i4PlaySpeed, 
    BOOL fgSeek)
{    
    BOOL fgEos = FALSE;
    UINT16 u2DecodingOrder = 0;    
    INT32 i4TempRef = 0;
    UINT32 u4SkipTime = 0;
    UINT32 u4SkipSize = 0;
    UINT32 u4Speed = 0;
    UINT64 u8Offset = 0;
    UINT64 u8OffsetI = 0;
    HAL_TIME_T rDelta;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);

    prSwdmxPsInfo->rVar.fgJumpNext = FALSE;
    /****clean data****/
    _SWDMX_PsResetData(prSwdmxInst, TRUE);
    /****one frame change time****/
    {
        x_memset((VOID*)(&rDelta), 0, sizeof(rDelta));
        HAL_GetTime(&prSwdmxPsInfo->rVar.rJumpEndTime);
        HAL_GetDeltaTime(&rDelta, &prSwdmxPsInfo->rVar.rJumpStartTime, 
            &prSwdmxPsInfo->rVar.rJumpEndTime);
        if(rDelta.u4Seconds > 3)
        {
            rDelta.u4Seconds = 3;
        }
        u4SkipTime = rDelta.u4Seconds * 1000000 + rDelta.u4Micros;
        x_memcpy((VOID*)(&prSwdmxPsInfo->rVar.rJumpStartTime), 
            (VOID*)(&prSwdmxPsInfo->rVar.rJumpEndTime), sizeof(HAL_TIME_T));
    }
    /****speed x****/
    {
        u4Speed = i4PlaySpeed > 0 ? i4PlaySpeed : -i4PlaySpeed;
    }
    /****set jump position****/
    {        
        {
            if(fgSeek)
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr = prSwdmxPsInfo->rVar.u8SendedSize;
            }
            else
            {
                VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder, 
                    &u8Offset, &u8OffsetI);
            }
            /****when first time, not check u4SkipTime****/
            if(prSwdmxPsInfo->rVar.u8JumpStartAddr == ((UINT64)(-1)))
            {
                u4SkipTime = 0;
                prSwdmxPsInfo->rVar.u8JumpStartAddr = u8OffsetI;
            }
        }
        u4SkipSize = _SWDMX_PsGetSkipSize(prSwdmxInst, u4SkipTime, u4Speed);        
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
        {
            LOG(0, "start pos(0x%llX), skipsize(0x%X), skiptime(0x%X), i4PlaySpeed(%d)\n", 
                prSwdmxPsInfo->rVar.u8JumpStartAddr,
                u4SkipSize, u4SkipTime, i4PlaySpeed);
        }
        if(i4PlaySpeed > 0)
        {            
            if(prSwdmxPsInfo->rVar.u8JumpStartAddr + u4SkipSize > prSwdmxPsInfo->rVar.u8FileSize)
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr = prSwdmxPsInfo->rVar.u8FileSize;
                fgEos = TRUE;
            }  
            else
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr += u4SkipSize;
                u8Offset += prSwdmxPsInfo->rVar.u4OneTimeSize;
            }
        }
        else
        {
            if(u4SkipSize + prSwdmxPsInfo->rVar.u8StartFileOffset > prSwdmxPsInfo->rVar.u8JumpStartAddr)
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr = prSwdmxPsInfo->rVar.u8StartFileOffset;
                fgEos = TRUE;
            }
            else
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr -= u4SkipSize;
            }
        }       
    }
    if(fgEos)
    {
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
        {
            LOG(0, "skip mode enter eos!\n");
        }
        prSwdmxPsInfo->rVar.fgJumpping = FALSE;
        /****v decoder may be pause, enable play****/
        _SWDMX_PsVideoPlay(prSwdmxInst);
        /****video display finished****/
        prSwdmxPsInfo->rVar.fgSendEOF = TRUE;
    }
    else if(prSwdmxPsInfo->rFmt.fgVideo)
    {
        /****stop stc****/
        STC_StopStc(prSwdmxInst->u1StcId);
        _SWDMX_PsRenderPos(prSwdmxInst, prSwdmxPsInfo->rVar.u8FileSize, (UINT64)(-1), 0, 0, 0);
        _SWDMX_PsPlayOneFrame(prSwdmxInst);
    }
    _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8JumpStartAddr);    
}

/****do skip by time****/
static VOID _SWDMX_PsDoSkipByTime(SWDMX_INFO_T * prSwdmxInst, INT32 i4PlaySpeed, 
    BOOL fgSeek)
{
    BOOL fgEos = FALSE;
    UINT64 u8SkipTime = 0;
    UINT32 u4Speed = 0;
    HAL_TIME_T rDelta;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    prSwdmxPsInfo->rVar.fgJumpNext = FALSE;
    /****clean data****/
    _SWDMX_PsResetData(prSwdmxInst, TRUE);
    /****one frame change time****/
    {
        x_memset((VOID*)(&rDelta), 0, sizeof(rDelta));
        HAL_GetTime(&prSwdmxPsInfo->rVar.rJumpEndTime);
        HAL_GetDeltaTime(&rDelta, &prSwdmxPsInfo->rVar.rJumpStartTime, 
            &prSwdmxPsInfo->rVar.rJumpEndTime);
        if(rDelta.u4Seconds > 3)
        {
            rDelta.u4Seconds = 3;
        }
        x_memcpy((VOID*)(&prSwdmxPsInfo->rVar.rJumpStartTime), 
            (VOID*)(&prSwdmxPsInfo->rVar.rJumpEndTime), sizeof(HAL_TIME_T));
        u8SkipTime = (rDelta.u4Seconds * 1000000 + rDelta.u4Micros) * 9 / 100;
    }
    /****set jump time, using u8JumpStartAddr to denote last skip time(PTS)****/
    {
        {
            if(fgSeek)
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr = prSwdmxPsInfo->rVar.u8JumpStartTime;
            }
            /****when first time, not check u4SkipTime****/
            if(prSwdmxPsInfo->rVar.u8JumpStartAddr == ((UINT64)(-1)))
            {
                u8SkipTime = 0;
                prSwdmxPsInfo->rVar.u8JumpStartAddr = VDP_GetPts(prSwdmxInst->u1B2rId);
            }
        }

        u4Speed = i4PlaySpeed > 0 ? i4PlaySpeed : -i4PlaySpeed;
        u8SkipTime = _SWDMX_Div6464((UINT64)u8SkipTime * u4Speed, (UINT64)1000, NULL);
        
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
        {
            LOG(0, "start time(%lld), skiptime(%lld), i4PlaySpeed(%d)\n", 
                prSwdmxPsInfo->rVar.u8JumpStartAddr,
                u8SkipTime, i4PlaySpeed);
        }
        
        if(i4PlaySpeed > 0)
        {
            if((prSwdmxPsInfo->rVar.u4TotalTime != 0) &&
                (prSwdmxPsInfo->rVar.u8JumpStartAddr + u8SkipTime > prSwdmxPsInfo->rVar.u4TotalTime))
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr = prSwdmxPsInfo->rVar.u4TotalTime;
                fgEos = TRUE;
            }  
            else
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr += u8SkipTime;
            }
        }
        else
        {
            if(u8SkipTime > prSwdmxPsInfo->rVar.u8JumpStartAddr)
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr = 0;
                fgEos = TRUE;
            }
            else
            {
                prSwdmxPsInfo->rVar.u8JumpStartAddr -= u8SkipTime;
            }
        }       
    }
    if(fgEos)
    {
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
        {
            LOG(0, "skip mode enter eos!\n");
        }
        prSwdmxPsInfo->rVar.fgJumpping = FALSE;
        /****v decoder may be pause, enable play****/
        _SWDMX_PsVideoPlay(prSwdmxInst);
        /****video display finished****/
        prSwdmxPsInfo->rVar.fgSendEOF = TRUE;
    }
    else if(prSwdmxPsInfo->rFmt.fgVideo)
    {
        /****stop stc****/
        STC_StopStc(prSwdmxInst->u1StcId);
        _SWDMX_PsRenderPos(prSwdmxInst, prSwdmxPsInfo->rVar.u8FileSize, (UINT64)(-1), 0, 0, 0);
        _SWDMX_PsPlayOneFrame(prSwdmxInst);
    }
    
    prSwdmxPsInfo->rVar.u8JumpStartTime = prSwdmxPsInfo->rVar.u8JumpStartAddr;
    _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8StartFileOffset);    
}

/****do skip****/
static VOID _SWDMX_PsDoSkip(SWDMX_INFO_T * prSwdmxInst, INT32 i4PlaySpeed, 
    BOOL fgSeek)
{
    switch (prSwdmxInst->eSeekType)
    {
        case eSWDMX_TYPE_BYTE_SEEK:
            _SWDMX_PsDoSkipByPos(prSwdmxInst, i4PlaySpeed, fgSeek);
            break;
        case eSWDMX_TYPE_TIME_SEEK:
            _SWDMX_PsDoSkipByTime(prSwdmxInst, i4PlaySpeed, fgSeek);
            break;
        default:
            LOG(0, "%s can not support support this eSeekType(%d)!\n", 
                __FUNCTION__,
                prSwdmxInst->eSeekType);
            ASSERT(0);
            break;
    }
}

/****handle skip mode****/
static VOID _SWDMX_PsHandleSkipMode(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);

    /**** do trick when arrived the end of file ****/
    if((prSwdmxPsInfo->rVar.u8SendedSize >= prSwdmxPsInfo->rVar.u8FileSize) &&
        (prSwdmxPsInfo->rVar.i4CurSpeed < 0))
    {
        prSwdmxPsInfo->rVar.fgJumpNext = TRUE;
        if(prSwdmxInst->u4LogFilter & check_skip_mode)
            LOG(0, "%s, do trick when arrived file end!\n", __FUNCTION__);
    }
    
    if(!prSwdmxPsInfo->rVar.fgJumpNext)
        return;

    /**** do seek when skip mode, cancel?****/
    if((prSwdmxPsInfo->rFmt.fgSeek) && (prSwdmxInst->eSeekType == eSWDMX_TYPE_BYTE_SEEK))
    {
        _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8SeekSkipPos);
    }

    /**** do trick ****/
    _SWDMX_PsDoSkip(prSwdmxInst, prSwdmxPsInfo->rVar.i4CurSpeed, prSwdmxPsInfo->rFmt.fgSeek);
    prSwdmxPsInfo->rFmt.fgSeek = FALSE;
}


/****handle trick****/
static BOOL _SWDMX_PsChangeSpeed(SWDMX_INFO_T * prSwdmxInst, INT32 i4PlaySpeed, BOOL fgSeek)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if(i4PlaySpeed == prSwdmxPsInfo->rVar.i4CurSpeed)
    {
        return FALSE;
    }

    LOG(1, "change speed %d -> %d\n", prSwdmxPsInfo->rVar.i4CurSpeed, i4PlaySpeed);

    if((i4PlaySpeed == SWDMX_PLAY_SPEED_1X) ||
        (SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed) && !SKIP_MODE(i4PlaySpeed)))
    {
        _SWDMX_PsJumpNormal(prSwdmxInst, fgSeek);            
    }
    else if(!SKIP_MODE(i4PlaySpeed))
    {
        LOG(1, "%s, enter no skip mode\n", __FUNCTION__);
        prSwdmxPsInfo->rVar.fgJumpNext = FALSE;
        prSwdmxPsInfo->rVar.fgJumpping = FALSE;        
        //STC_StopStc(prSwdmxInst->u1StcId);
    }
    else if(!SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed) && 
        SKIP_MODE(i4PlaySpeed))
    {  
        LOG(1, "%s, enter skip mode\n", __FUNCTION__);
        prSwdmxPsInfo->rVar.fgJumpping = TRUE;
        prSwdmxPsInfo->rVar.u8JumpStartAddr = (UINT64)(-1);
        VDEC_RegTrickPtsCb(prSwdmxInst->u1VdecId,
            _SWDMX_PsSeekFinNotify, prSwdmxInst->u1SwdmxId);
        _SWDMX_PsDoSkip(prSwdmxInst, i4PlaySpeed, fgSeek);
    }
    else
    {
        _SWDMX_PsResetData(prSwdmxInst, TRUE);
        _SWDMX_PsDoSkip(prSwdmxInst, i4PlaySpeed, fgSeek);
        prSwdmxPsInfo->rVar.fgJumpping = TRUE;
    }
    
    prSwdmxPsInfo->rVar.i4CurSpeed = i4PlaySpeed;
    return TRUE;
}

/****handle seek****/
static BOOL _SWDMX_PsHandleSeek(SWDMX_INFO_T * prSwdmxInst, SWDMX_CUR_CMD_T * prCmd)
{
    UINT64 u8SeekPos = prCmd->u8SeekPos;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if(prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_TIME)
    {
        /****  check range  ****/
        if (prCmd->u8SeekTime > prSwdmxPsInfo->rVar.u4TotalTime)
        {
            prCmd->u8SeekTime = prSwdmxPsInfo->rVar.u4TotalTime;
            LOG(0, "%s, invalid seek time = %d, duration = %d, change seek time to duration!\n",
                __FUNCTION__,
                prCmd->u8SeekTime, 
                prSwdmxPsInfo->rVar.u4TotalTime);
        }
        else
        {
            LOG(0, "seek time = %d\n", prCmd->u8SeekTime);
        }
        /****  reset data  ****/
        _SWDMX_PsResetData(prSwdmxInst, 1);
        STC_StopStc(prSwdmxInst->u1StcId);
        
        prSwdmxPsInfo->rVar.u8JumpStartTime = prCmd->u8SeekTime;
        _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8StartFileOffset);
        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxPsInfo->rVar.u8JumpStartTime);

        VDP_SetSeek(prSwdmxInst->u1B2rId,
                    1, 
                    (prSwdmxPsInfo->rVar.i4CurSpeed >= 0) ? 
                     prSwdmxPsInfo->rVar.u8JumpStartTime : (UINT32)-1);
        LOG(5,"Set to B2r seek \n");
        
        LOG(1,"fgAudio %d\n",prSwdmxPsInfo->rFmt.fgAudio);
        
        if(prSwdmxPsInfo->rFmt.fgAudio)
        {
            #ifdef CC_AUD_SUPPORT_DUAL_DSP
            AUD_DSPCmdStop(AUD_DSP0, prSwdmxInst->u1AdecId);
            AUD_DSPCmdPlay(AUD_DSP0, prSwdmxInst->u1AdecId);
            #else
            AUD_DSPCmdStop(prSwdmxInst->u1AdecId);
            AUD_DSPCmdPlay(prSwdmxInst->u1AdecId);
            #endif
            LOG(1,"Time-based Seek ,Stop/play audio dsp\n");
        }                
        
        return TRUE;
    }
    else if(((UINT32)(u8SeekPos>>32) != (UINT32)0xFFFFFFFF) ||
        ((UINT32)(u8SeekPos) != (UINT32)0xFFFFFFFF))
    {        
        /****change sended position****/
        {
            /****check the position****/
            if((u8SeekPos < prSwdmxPsInfo->rVar.u8StartFileOffset) ||
                (u8SeekPos > prSwdmxPsInfo->rVar.u8FileSize))
            {
                LOG(0, "%s, invalid seek position = 0x%llX, start Offset = 0x%llX, file size = 0x%llX \n",
                    __FUNCTION__,
                    u8SeekPos, 
                    prSwdmxPsInfo->rVar.u8StartFileOffset,
                    prSwdmxPsInfo->rVar.u8FileSize);
                ASSERT(0);
            }
            else
            {
                LOG(0, "seek pos = 0x%X\n", u8SeekPos);
            }
        }

        if (SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed))
        {
            /****handle SKIP MODE seek****/
            prSwdmxPsInfo->rFmt.fgSeek = TRUE;
            prSwdmxPsInfo->rVar.u8SeekSkipPos = u8SeekPos;
        }
        else
        {
            _SWDMX_PsSetSendedPos(prSwdmxPsInfo, u8SeekPos);
            /**** reset data(feeder & v/a fifo)****/
            _SWDMX_PsResetData(prSwdmxInst, 1);
            _SWDMX_PsRenderPos(prSwdmxInst, prSwdmxPsInfo->rVar.u8FileSize, (UINT64)(-1), 0, 0, 0);
            STC_StopStc(prSwdmxInst->u1StcId);
        }
        
        VDP_SetSeek(prSwdmxInst->u1B2rId,1,(prSwdmxPsInfo->rVar.i4CurSpeed >= 0) ? 0 : (UINT32)-1);
        LOG(5,"Set to B2r seek \n"); 
        
        return TRUE;
    }
    else if((prCmd->fgSeek) &&
            (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
    {
        /****  reset data(feeder & v/a fifo)  ****/
        _SWDMX_PsResetData(prSwdmxInst, 1);
        
        /****  stop stc  ****/
        STC_StopStc(prSwdmxInst->u1StcId);

        /****  change sended position  ****/
        {
            if (prCmd->rSeekOffset.ui8_i_frame_position == (UINT64)(-1))
            {
                _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prCmd->rSeekOffset.ui8_frame_position);
            }
            else
            {
                _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prCmd->rSeekOffset.ui8_i_frame_position);
            }
            
            _SWDMX_PsRenderPos(prSwdmxInst,
                prCmd->rSeekOffset.ui8_frame_position,
                prCmd->rSeekOffset.ui8_i_frame_position,
                prCmd->rSeekOffset.i4_temporal_reference,
                prCmd->rSeekOffset.ui2_decoding_order,
                (UINT32)(prCmd->rSeekOffset.ui8_pts_info));
        }

        VDP_SetSeek(prSwdmxInst->u1B2rId,1,(prSwdmxPsInfo->rVar.i4CurSpeed >= 0) ? 0 : (UINT32)-1);
        LOG(5,"Set to B2r seek \n"); 
		
        return TRUE;
    }
    
    return FALSE;
}

/****this is used to handle speed change and seek****/
static VOID _SWDMX_PsHandleChange(SWDMX_INFO_T * prSwdmxInst, SWDMX_CUR_CMD_T * prCmd)
{
    BOOL fgSeek = FALSE;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);

    /****handle seek****/
    fgSeek = _SWDMX_PsHandleSeek(prSwdmxInst, prCmd);

    /****handle trick start****/
    _SWDMX_PsChangeSpeed(prSwdmxInst, prCmd->i4PlaySpeed, fgSeek);
}

static BOOL _SWDMX_PsGetSeqHeader(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if(!prSwdmxPsInfo->rFmt.fgVideo)
    {
        LOG(0, "%s, no video\n", __FUNCTION__);
        return TRUE;
    }
    
    if((!prSwdmxPsInfo->rVar.fgSendSeqHdr) && (!prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq))
    {
        /****record sended size, we will found seq header form file start.****/
        prSwdmxPsInfo->rVar.u8OrgSendedSize = prSwdmxPsInfo->rVar.u8SendedSize;
        /****this mean need to move to u8OrgSendedSize after found seq header****/
        prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq = TRUE;
        /****go to file start for finding seq header****/
        prSwdmxPsInfo->rVar.u8SendedSize = prSwdmxPsInfo->rVar.u8StartFileOffset;
        prSwdmxPsInfo->rVar.u8ReadedPos = prSwdmxPsInfo->rVar.u8SendedSize;        
#if 1/****def CC_DLNA_SUPPORT****/
        FeederSetReadyThreshold(prSwdmxInst->eFeederSrc, MPG_DLNA_FEEDER_UNREACH);
        FeederSetUnderFlowThreshold(prSwdmxInst->eFeederSrc, MPG_DLNA_FEEDER_UNREACH);
        FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_THRESHOLD, FALSE);
#endif
        _SWDMX_PsResetData(prSwdmxInst, 1);
        LOG(1, "%s, begin to find first seq header\n", __FUNCTION__);
    }
    else if(!prSwdmxPsInfo->rVar.fgSendSeqHdr)
    {
        LOG(1, "finding seq header...\n");
    }
    else if(prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq)
    {
        prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq = FALSE;        
        _SWDMX_PsResetData(prSwdmxInst, 1);
        
        if (!_SWDMX_PsHandleLastMemory(prSwdmxInst))
        {
            LOG(0, "[%s](%d) Normal start play.\n", __FUNCTION__, __LINE__);
            _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8OrgSendedSize);
        }
        
        return TRUE;
    }
    
    return FALSE;
}

static inline VOID _SWDMX_PsGetAVThreshold(SWDMX_MPG_INFO_T * prSwdmxPsInfo, UINT16 * p2VideoThreshold, 
    UINT32 * p4AudioThreshold)
{
    UINT16 u2VideoThreshold = 0;
    UINT32 u4AudioThreshold = 0;
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(prSwdmxPsInfo->rFmt.fgVideo)
    {
        if(SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed) && prSwdmxPsInfo->rVar.fgVideoPause)
        {
            u2VideoThreshold = 0;
        }
        else if(USB_SOURCE(prSwdmxPsInfo->rVar.e_mm_src_type))
        {
            u2VideoThreshold = 30;
        }
        else
        {
            u2VideoThreshold = 60;
        }
    }
    
    if(prSwdmxPsInfo->rFmt.fgAudio)
    {
        if(AUDIO_NOT_MPEG(prSwdmxPsInfo->rFmt.u2AudioId))
        {
            u4AudioThreshold = 50 * 1024;
        }
        else
        {
            u4AudioThreshold = 30 * 1024;
        }
        if(!prSwdmxPsInfo->rFmt.fgVideo)
        {
            u4AudioThreshold *= 2;
        }
    }
    *p2VideoThreshold = u2VideoThreshold;
    *p4AudioThreshold = u4AudioThreshold;
} 

/****this used to judge a/v fifo is full or not****/
static BOOL _SWDMX_PsFifoIsFull(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    BOOL fgFull = FALSE;
    BOOL fgAudioSmaller = FALSE;    
    UINT16 u2QueueSize = 0;
    UINT16 u2MaxQueueSize = 0;
    UINT16 u4ThresholdPic = 0;
#ifdef CHECK_DMX_MSG_Q
    UINT16 u2DmxPictureQSize = 0;
    UINT16 u2DmxMaxQSize = 0;
#endif
    UINT32 u4ThresholdAudio = 0;
    UINT32 u4AvailSize = 0;
    UINT32 u4AvailAudioSize = 0;
    UINT32 u4AudioDataSize = 0;    
        
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    /****init available size to be 1M****/
    u4AvailSize = 1024 * 1024;

    /****get avail video size****/
    if(prSwdmxPsInfo->rFmt.fgVideo)
    {
        u4AvailSize = _SWDMX_PsGetFifoAvailSize(prSwdmxInst->u1DmxId, 
            DMX_PID_TYPE_ES_VIDEO, 
            prSwdmxPsInfo->rVar.u1PidxVideo);               
    }
    if(prSwdmxPsInfo->rFmt.fgAudio)
    {
        u4AvailAudioSize = _SWDMX_PsGetFifoAvailSize(prSwdmxInst->u1DmxId, 
            DMX_PID_TYPE_ES_AUDIO, 
            prSwdmxPsInfo->rVar.u1PidxAudio);
        if(u4AvailAudioSize < u4AvailSize)
        {        
            u4AvailSize = u4AvailAudioSize;
            fgAudioSmaller = TRUE;
        }
    }    
    if(u4AvailSize < prSwdmxPsInfo->rVar.u4OneTimeSize)
    {        
        fgFull = TRUE;
        if(fgAudioSmaller)
        {
            if(prSwdmxInst->u4LogFilter & check_fifo_full)
            {
                LOG(0, "A fifo full, asize = 0x%X\n", u4AvailSize);
            }

            if (!prSwdmxPsInfo->rVar.fgEnableAudio &&
                prSwdmxPsInfo->rVar.fgGotAudioReady &&
                _SWDMX_PsIsVFifoEmpty(prSwdmxInst))
            {
                LOG(1, "%s VFIFO is empty\n", __FUNCTION__);
                _SWDMX_PsAudioPlay(prSwdmxInst);
            }            
        }
        else
        {
            if(prSwdmxInst->u4LogFilter & check_fifo_full)
            {
                LOG(0, "V fifo full, vsize = 0x%X\n", u4AvailSize);
            }
        }
    }
    if(!fgFull)
    {                 
        BOOL fgVideoFull = FALSE;
        _SWDMX_PsGetAVThreshold(prSwdmxPsInfo, &u4ThresholdPic, &u4ThresholdAudio);
        
        if(prSwdmxPsInfo->rFmt.fgVideo && prSwdmxPsInfo->rVar.fgGotVideo)
        {            
            VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2QueueSize, &u2MaxQueueSize);
#ifdef CHECK_DMX_MSG_Q
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
        
            if(((u2DmxPictureQSize > 0) && (u2DmxMaxQSize > 384)) &&
                (u2DmxPictureQSize > (u2DmxMaxQSize - 384)))
            {
                fgFull = TRUE;
            }
            else if((u2DmxPictureQSize + u2QueueSize) >= u2MaxQueueSize) 
            {
                /*
                protect ESMQ overflow by the interrupt from dmx picture Q
                when os scheduled to dmx, it will dump all in hands picture Q to Vdec ESMQ
                */
                fgFull = TRUE;
            }
            else 
#endif
            if(u2QueueSize >= u4ThresholdPic)
            {
                if(u2QueueSize >= MAX_EMSG_NUM)
                {
                    /****video max, no need to check audio****/
                    fgFull = TRUE;
                }
                else
                {
                    /****video full****/
                    fgVideoFull = TRUE;
                }
            }                        
            /****ap control network****/
            else if((!prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.fgEnableVideo) 
                &&
                ((prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.e_mm_src_type ==
                MM_SRC_TYPE_NETWORK_VUDU) ||
                (prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.e_mm_src_type ==
                MM_SRC_TYPE_PUSH) ||
                (prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.e_mm_src_type ==
                MM_SRC_TYPE_NETWORK_CMPB)) 
                &&
                (((prSwdmxInst->ePlaybackBufType == eSWDMX_BUF_MODE_TIME) &&
                /**** ms -> frame (fps = 30)****/
                (u2QueueSize >= (prSwdmxInst->u4PlaybackBufThreshold / 33))) ||
                ((prSwdmxInst->ePlaybackBufType == eSWDMX_BUF_MODE_BYTE) &&
                ((prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.u8SendedSize -
                prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.u8StartFileOffset) >=
                prSwdmxInst->u4PlaybackBufThreshold))))
            {
                fgVideoFull = TRUE;
            }            
        }
        if(prSwdmxPsInfo->rFmt.fgAudio)
        {
            if(!fgFull)
            {
                UINT32 u4AudioFifoSize = 0;
                UINT32 u4Addr = 0;
                /****get audio fifo size****/
                if(!_SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4Addr, &u4AudioFifoSize))
                {
                    if(prSwdmxInst->u4LogFilter & check_fifo_full)
                        LOG(0, "line = %d, get audio fifo info fail\n", __LINE__);
                    return FALSE;
                }
                u4AudioDataSize = u4AudioFifoSize - u4AvailAudioSize;
                if(prSwdmxPsInfo->rVar.fgGotAudioReady)
                {
                    if(u4AudioDataSize > u4ThresholdAudio) fgFull = TRUE;
                }
                if((prSwdmxPsInfo->rVar.u1TotalATrack == 1) &&
                    prSwdmxPsInfo->rFmt.fgVideo && !fgVideoFull && fgFull)
                {
                    fgFull = FALSE;
                }
            }
        }
        else if(fgVideoFull)
        {
            fgFull = TRUE;
        }
    }

    if(fgFull && (prSwdmxInst->u4LogFilter & check_fifo_full))
    {
        LOG(0, "a_size = 0x%X, a_max = 0x%X, vf_num = %d, vf_max = %d\n", 
                u4AudioDataSize, 
                u4ThresholdAudio,
                u2QueueSize,
                u4ThresholdPic);
    }
    
    return fgFull;
}

/****this is used to set info for move data****/
static VOID _SWDMX_PsSetMoveDataInfo(UINT8 u1SrcId, DMX_MM_DATA_T * prMoveData)
{
    UINT32 u4SendSize = 0;
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    /****set move size****/
    u4SendSize = prSwdmxPsInfo->rVar.u4OneTimeSize;
    if((UINT64)u4SendSize + prSwdmxPsInfo->rVar.u8SendedSize > prSwdmxPsInfo->rVar.u8FileSize)
    {
        /****not over the file end****/
        u4SendSize = (prSwdmxPsInfo->rVar.u8FileSize - prSwdmxPsInfo->rVar.u8SendedSize);
    }    
    if(u4SendSize > prSwdmxPsInfo->rBuf.u4UsingBufSize)
    {
        /****not over now buffer****/
        u4SendSize = prSwdmxPsInfo->rBuf.u4UsingBufSize;
    }
    /****must not be zero****/
    ASSERT(u4SendSize);
    prMoveData->u4FrameSize = u4SendSize;
    if(prSwdmxPsInfo->rVar.fgFirstData)
    {
        prMoveData->fgFrameHead = TRUE;
    }
    /****judge is the last data or not****/
    //need modfy
    if(_SWDMX_PsFileEnd(prSwdmxInst, u4SendSize))
    {
        prMoveData->fgEOS = TRUE;
    }
    prMoveData->u4BufStart = prSwdmxInst->rFeederInfo.u4StartAddr;
    prMoveData->u4BufEnd = prSwdmxInst->rFeederInfo.u4EndAddr;
    while(prSwdmxPsInfo->rBuf.u4UsingStartAddr >= prMoveData->u4BufEnd)
    {
        if(prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "u4UsingStartAddr(0x%X) >= u4BufEnd(0x%X)\n", 
                prSwdmxPsInfo->rBuf.u4UsingStartAddr,
                prMoveData->u4BufEnd);
        }
        prSwdmxPsInfo->rBuf.u4UsingStartAddr -= (prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr);
    }
    /****set move start address****/
    prMoveData->u4StartAddr = prSwdmxPsInfo->rBuf.u4UsingStartAddr;
}

static INT32 _SWDMX_PsMoveData(UINT8 u1SrcId, DMX_MM_DATA_T * prMoveData)
{
    BOOL fgRet = FALSE;
    
    /****flush cache, no need, dmx will do it****/
    if((prMoveData->u4StartAddr < prMoveData->u4BufEnd) &&
       ((prMoveData->u4StartAddr + prMoveData->u4FrameSize) >= prMoveData->u4BufEnd))
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prMoveData->u4StartAddr), (prMoveData->u4BufEnd - prMoveData->u4StartAddr));
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prMoveData->u4BufStart), (prMoveData->u4FrameSize - (prMoveData->u4BufEnd - prMoveData->u4StartAddr)));
    }
    else
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prMoveData->u4StartAddr), prMoveData->u4FrameSize);
    }

#ifdef SWDMX_PS_DMX_20
    fgRet = _SWDMX_MoveData(u1SrcId, prMoveData);    
#else
    fgRet = DMX_DDI_MoveData(prMoveData);
#endif

    if(!fgRet)
    {
        return MPG_SEND_FAIL;
    }
    
    return MPG_SEND_OK;
}

/****this to update read positon of feeder****/
static BOOL _SWDMX_PsUpdateFeederPosition(UINT8 u1SrcId, DMX_MM_DATA_T * prMoveData)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;    
    UINT32 u4FeederPtr = 0;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if(prSwdmxPsInfo->rBuf.u4UsingBufSize >= prMoveData->u4FrameSize)
    {
        prSwdmxPsInfo->rVar.u8SendedSize += prMoveData->u4FrameSize;
        prSwdmxPsInfo->rBuf.u4UsingStartAddr += prMoveData->u4FrameSize;
        prSwdmxPsInfo->rBuf.u4UsingBufSize -= prMoveData->u4FrameSize;
    }
    else
    {
    
        if(prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(0, "%s, u4UsingBufSize = 0x%X, u4FrameSize = 0x%X\n", __FUNCTION__, prMoveData->u4FrameSize);
        }
        ASSERT(0);
    }
    if(!IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type))
    {
        u4FeederPtr = prMoveData->u4StartAddr + prMoveData->u4FrameSize;
        while(u4FeederPtr >= prSwdmxInst->rFeederInfo.u4EndAddr)
        {
            u4FeederPtr -= (prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr);
        }
        if(FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, u4FeederPtr) != FEEDER_E_OK)
        {        
            if(prSwdmxInst->u4LogFilter & check_feeder_req)
            {
                LOG(0, "%s %d FeederUpdateReadPointer fail\n", __FUNCTION__, __LINE__);
            }
            return FALSE;
        }
    }
    else if(prSwdmxPsInfo->rBuf.u4UsingBufSize == 0) /****prSwdmxPsInfo->rVar.e_mm_src_type != MM_SRC_TYPE_PUSH****/
    {
        if(FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0) != FEEDER_E_OK)
        {        
            if(prSwdmxInst->u4LogFilter & check_feeder_req)
            {
                LOG(0, "%s %d FeederUpdateReadPointer fail\n", __FUNCTION__, __LINE__);
            }
            return FALSE;
        }
    }
    return TRUE;
}

/****set req size and move size one time****/
static void _SWDMX_PsSetReqSize(SWDMX_MPG_INFO_T * prSwdmxPsInfo)
{    
    UINT32 time = 0;
    UINT32 size = 0;
    
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);

    /****u4OneTimeReadSize must < feeder size / (4 + 1)****/
    if((prSwdmxPsInfo->rVar.u8FileSize != 0xFFFFFFFF) &&
        (prSwdmxPsInfo->rVar.u4TotalTime))
    {
        time = prSwdmxPsInfo->rVar.u4TotalTime / 90000; /**second**/
        /****Mbyte****/
        size = (UINT32)_SWDMX_Div6464(prSwdmxPsInfo->rVar.u8FileSize,
            1024 * 1024,
            NULL);
        
        if(time > size * 3)
            prSwdmxPsInfo->rVar.u4OneTimeReadSize = PS_REQ_SIZE_20M / 2;
        else if(4 * time > size) 
            prSwdmxPsInfo->rVar.u4OneTimeReadSize = PS_REQ_SIZE_20M;
        else
            prSwdmxPsInfo->rVar.u4OneTimeReadSize = PS_REQ_SIZE_20M * 3 / 2;        
    }
    else
    {
        prSwdmxPsInfo->rVar.u4OneTimeReadSize = PS_REQ_SIZE_20M;
    }
    prSwdmxPsInfo->rVar.u4OneTimeSize = prSwdmxPsInfo->rVar.u4OneTimeReadSize / PS_REQ_MOVE_RATE;

    LOG(3, "%s u4OneTimeReadSize(0x%X) u4OneTimeSize(0x%X)\n", __FUNCTION__,
            prSwdmxPsInfo->rVar.u4OneTimeReadSize,
            prSwdmxPsInfo->rVar.u4OneTimeSize);
}

/****this is to send req data msg****/
static INT32 _SWDMX_PsFeederReq(SWDMX_INFO_T * prSwdmxInst)
{    
    FEEDER_TRANSMIT_COND rFeederCond;
    UINT64 u8ReadPos = 0;  /****this is read position of file****/
    UINT32 u4ReadSize = 0;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    INT32 i4Ret = -1;
    BOOL u4ReqTimes = 0;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, MPG_NO_INSTANSE);

    if(prSwdmxPsInfo->rBuf.fgGetEof)
    {
        /****no need to req data****/
        return MPG_REQ_END;
    }

    /****push mode req address is zero****/
    if(IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type))
    {
        u8ReadPos = 0;
        u4ReadSize = 1;
    }
    /****time seek mode****/
    else if(prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
    {
        u8ReadPos = prSwdmxPsInfo->rVar.u8JumpStartTime;
        // turn PTS to seconds.
        if (u8ReadPos != DLNA_TIME_SEEK_INVALID_TIME)
        {
            u8ReadPos = _SWDMX_Div6464(u8ReadPos, (UINT64)90000, NULL);
            LOG(1, "feeder req time seek to %lld(S)\n", u8ReadPos);
        }
        u4ReadSize = prSwdmxPsInfo->rVar.u4OneTimeReadSize;
    }
    else /****get req size****/
    {
        u8ReadPos = prSwdmxPsInfo->rVar.u8ReadedPos +
                    prSwdmxPsInfo->rBuf.u4ReqTotalSize +
                    prSwdmxPsInfo->rBuf.u4BufTotalSize;
        if(prSwdmxPsInfo->rVar.u8FileSize <= u8ReadPos)
        {
            /****no need to req data****/
            return MPG_REQ_END;
        }
        u4ReadSize = prSwdmxPsInfo->rVar.u4OneTimeReadSize;
        if(u4ReadSize > (prSwdmxPsInfo->rVar.u8FileSize - u8ReadPos))
        {
            u4ReadSize = (prSwdmxPsInfo->rVar.u8FileSize - u8ReadPos);
        }
    }
    /****judge req or not****/
    if(u4ReadSize > 0)
    {        
        if(((!prSwdmxPsInfo->rVar.fgSendSeqHdr) && (prSwdmxPsInfo->rFmt.fgVideo)) ||
            (SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed)))
        {
            if((prSwdmxPsInfo->rBuf.u4UsingBufSize == 0) &&
                (prSwdmxPsInfo->rBuf.u4ReqTimes == 0) &&
                (prSwdmxPsInfo->rBuf.u4BufAvailCnt == 0))
            {
                u4ReqTimes = 1;
            }
        }
        else
        {                          
            if(prSwdmxPsInfo->rBuf.u4ReqTimes + prSwdmxPsInfo->rBuf.u4BufAvailCnt 
                + 1 <= prSwdmxPsInfo->rVar.u4FeederReqNum)
            {
                u4ReqTimes = prSwdmxPsInfo->rVar.u4FeederReqNum
                    - prSwdmxPsInfo->rBuf.u4BufAvailCnt
                    - prSwdmxPsInfo->rBuf.u4ReqTimes;                 
            }
        }
    }

    if ((prSwdmxPsInfo->rVar.e_mm_src_type == MM_SRC_TYPE_NETWORK_DLNA) &&
        (prSwdmxPsInfo->rVar.i4CurSpeed > 0) &&
        (prSwdmxPsInfo->rVar.i4CurSpeed < 1000) &&
        prSwdmxInst->fgSupportConnStalling)
    {
        u4ReadSize *= prSwdmxInst->u4Multiple;
        LOG(6,"u4Multiple %d u4ReadSize 0x%x\n",prSwdmxInst->u4Multiple,u4ReadSize);
    }

    if(u4ReqTimes)
    {
        UINT32 i;
        for(i = 0; i < u4ReqTimes; i++)
        {
            x_memset(&rFeederCond, 0, sizeof(FEEDER_TRANSMIT_COND));
            i4Ret = FeederSetRequest(prSwdmxInst->eFeederSrc,
                        u4ReadSize,
                        u8ReadPos,
                        &rFeederCond,
                        prSwdmxPsInfo->rVar.u4FeederReqID);
            if(i4Ret != FEEDER_E_OK)
            {
                LOG(1, "req id = %d failed!\n", prSwdmxPsInfo->rVar.u4FeederReqID);
                if(i4Ret == FEEDER_E_BUFFER_FULL)
                {
                    LOG(1, "found feeder full, check it!\n");
                }
                return MPG_REQ_FAIL;
            }
            {
                if(prSwdmxInst->u4LogFilter & check_feeder_req)
                {
                    LOG(1, "req id(%d), size(0x%X)\n",
                        prSwdmxPsInfo->rVar.u4FeederReqID,
                        u4ReadSize);
                }
                prSwdmxPsInfo->rBuf.au4ReqId[prSwdmxPsInfo->rBuf.u4ReqWriteIdx] = prSwdmxInst->rFmtInfo.rSwdmxMpgInfo.rVar.u4FeederReqID;
                prSwdmxPsInfo->rBuf.au4ReqSize[prSwdmxPsInfo->rBuf.u4ReqWriteIdx] = u4ReadSize;                
                prSwdmxPsInfo->rBuf.u4ReqTotalSize += u4ReadSize;
                prSwdmxPsInfo->rVar.u4FeederReqID += 2;
                prSwdmxPsInfo->rBuf.u4ReqTimes++;
                prSwdmxPsInfo->rBuf.u4ReqWriteIdx++;
                if(prSwdmxPsInfo->rBuf.u4ReqWriteIdx >= prSwdmxPsInfo->rVar.u4FeederReqNum)
                {
                    prSwdmxPsInfo->rBuf.u4ReqWriteIdx = 0;
                }
                prSwdmxPsInfo->rVar.u8JumpStartTime = DLNA_TIME_SEEK_INVALID_TIME;
                break;

            }
        }
    }
    else
    {
        
        if(prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "ReqTimes(%d) BufAvailCnt(%d) FeederReqNum(%d)\n", 
                prSwdmxPsInfo->rBuf.u4ReqTimes,
                prSwdmxPsInfo->rBuf.u4BufAvailCnt,
                prSwdmxPsInfo->rVar.u4FeederReqNum);
        }
        return MPG_REQ_NONE;
    }
    
    return MPG_REQ_OK;
}

/****this is handle feeder ack****/
static INT32 _SWDMX_PsFeederAck(SWDMX_INFO_T * prSwdmxInst)
{
    FEEDER_REQ_DATA_T rFeederAckData;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    INT32 i4Ret = -1;
    BOOL fgRet = FALSE;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    /****req exist and file not end****/
    if(prSwdmxPsInfo->rBuf.u4ReqTimes && (!prSwdmxPsInfo->rBuf.fgGetEof))
    {
        x_memset(&rFeederAckData, 0, sizeof(FEEDER_REQ_DATA_T));
        /****when no data in feeder, should wait.****/
        if((prSwdmxPsInfo->rBuf.u4BufAvailCnt == 0) && (prSwdmxPsInfo->rBuf.u4UsingBufSize == 0))
        {
            fgRet = _SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)&rFeederAckData);
            
        }
        else
        {
            i4Ret = _SWDMX_ReceiveFeederAckNoWait(prSwdmxInst->u1SwdmxId, (VOID*)&rFeederAckData);
            if (i4Ret == OSR_OK)
            {
                fgRet = TRUE;
            }
            else
            {
                if(prSwdmxInst->u4LogFilter & check_feeder_req)
                    LOG(7, "rec feeder msg fail, i4Ret = %d\n", i4Ret);
            }
        }        
        if(fgRet)
        {   
            /****timeout****/
            if(rFeederAckData.eDataType == FEEDER_SOURCE_INVALID)
            {
                if (SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed))
                {
                    _SWDMX_PsResetFeeder(prSwdmxInst);
                }
                
                if(prSwdmxInst->u4LogFilter & check_feeder_req)
                    LOG(0, "timeout ack, ack id = %d\n", rFeederAckData.u4Id);

                return MPG_ACK_FAIL;
            }
            /****eos from feeder****/
            else if(rFeederAckData.fgEof)
            {
                prSwdmxPsInfo->rBuf.fgGetEof = TRUE;
                if(prSwdmxPsInfo->rVar.e_mm_src_type != MM_SRC_TYPE_HIGH_SPEED_STORAGE)
                {
                    FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_EOS, TRUE);
                }
                LOG(1, "get eof!");
            }
            /***found error data, should reset data***/
            else if(rFeederAckData.u4WriteAddr == 0)
            {
                LOG(0, "ack error data, id = %d\n", rFeederAckData.u4Id);
                if(IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type))
                {
                    rFeederAckData.u4ReadSize = 0;
                }
                else
                {
                    _SWDMX_PsResetFeeder(prSwdmxInst);
                    x_thread_delay(30);
                    return MPG_ACK_FAIL;
                }            
            }            
            {   
                if(rFeederAckData.u4Id == prSwdmxPsInfo->rVar.u4FeederReqID - (2 * prSwdmxPsInfo->rBuf.u4ReqTimes))
                {   
                    UINT32 req_size = prSwdmxPsInfo->rBuf.au4ReqSize[prSwdmxPsInfo->rBuf.u4ReqReadIdx];
                    if(prSwdmxInst->u4LogFilter & check_feeder_req)
                        LOG(1, "ack id(%d), size(0x%X)\n", rFeederAckData.u4Id, rFeederAckData.u4ReadSize);
                    (prSwdmxPsInfo->rBuf.u4ReqTimes)--;
                    prSwdmxPsInfo->rBuf.u4ReqTotalSize -= req_size;
                    if(rFeederAckData.u4ReadSize != req_size)
                    {
                        if(prSwdmxInst->u4LogFilter & check_feeder_req)
                        {
                            LOG(0, "ID(%d), req size(0x%X) -> ack size(0x%X)\n",
                                rFeederAckData.u4Id, req_size, rFeederAckData.u4ReadSize);
                        }
                        if(_SWDMX_PsUseAckSize(prSwdmxPsInfo->rVar.e_mm_src_type))
                        {
                            req_size = rFeederAckData.u4ReadSize - rFeederAckData.u4AlignOffset;
                        }
                    }
                    prSwdmxPsInfo->rBuf.au4BufStartAddr[prSwdmxPsInfo->rBuf.u4BufWriteIdx] =
                        rFeederAckData.u4WriteAddr + rFeederAckData.u4AlignOffset;
                    prSwdmxPsInfo->rBuf.afgBufFeederIBCType[prSwdmxPsInfo->rBuf.u4BufWriteIdx] =
                            (UINT32)rFeederAckData.eFeederIBC;
                    #if 0 /****need check****/
                    if(rFeederAckData.eFeederIBC == FEEDER_IBC_DMX_VUDU_KEY)
                    {
                        x_memcpy((VOID*)&_arVuduKey[u1SrcId][prSwdmxPsInfo->rBuf.u4BufWriteIdx],
                            (VOID*)rFeederAckData.u4WriteAddr,
                            sizeof(DMX_VUDU_KEY_T));
                        if(sizeof(DMX_VUDU_KEY_T) != rFeederAckData.u4ReadSize)
                        {
                            LOG(1, "(sizeof(DMX_VUDU_KEY_T)(%d) != rFeederReqData.u4ReadSize(%d))",
                                sizeof(DMX_VUDU_KEY_T),
                                rFeederAckData.u4ReadSize);
                        }
                    }
                    #endif
                    prSwdmxPsInfo->rBuf.au4BufSize[prSwdmxPsInfo->rBuf.u4BufWriteIdx] = req_size;
                    prSwdmxPsInfo->rBuf.u4BufTotalSize += req_size;
                    prSwdmxPsInfo->rBuf.u4BufAvailCnt++;
                    prSwdmxPsInfo->rBuf.u4ReqReadIdx++;
                    prSwdmxPsInfo->rBuf.u4BufWriteIdx++;
                    if(prSwdmxPsInfo->rBuf.u4ReqReadIdx >= prSwdmxPsInfo->rVar.u4FeederReqNum)
                    {
                        prSwdmxPsInfo->rBuf.u4ReqReadIdx = 0;
                    }                        
                    if(prSwdmxPsInfo->rBuf.u4BufWriteIdx >= prSwdmxPsInfo->rVar.u4FeederBufNum)
                    {
                        prSwdmxPsInfo->rBuf.u4BufWriteIdx = 0;
                    }
#ifdef SWDMX_DUMP_DATA
                    _SWDMX_DumpFeeder(prSwdmxInst, 
                        (INT8 *)(rFeederAckData.u4WriteAddr + rFeederAckData.u4AlignOffset),
                        req_size);
#endif
                    return MPG_ACK_OK;
                }
                else
                {                    
                    if(prSwdmxInst->u4LogFilter & check_feeder_req)
                    {
                        LOG(0, "ack id not continuous, need(%d), ack(%d) = %d\n",
                            prSwdmxPsInfo->rVar.u4FeederReqID - (2 * prSwdmxPsInfo->rBuf.u4ReqTimes),
                            rFeederAckData.u4Id);
                    }
                }
            }
        }
    }
    else
    {
        if(prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(1, "reqtimes = %d, fgeos = %d\n", prSwdmxPsInfo->rBuf.u4ReqTimes,
                prSwdmxPsInfo->rBuf.fgGetEof);
        }
    }
    return MPG_ACK_FAIL;    
}

/****this is update the using buffer****/
static VOID _SWDMX_PsUpdateUsingBuffer(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    UINT32 req_size = 0;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);

    while((!prSwdmxPsInfo->rBuf.u4UsingBufSize) && (prSwdmxPsInfo->rBuf.u4BufAvailCnt))
    {
        req_size = prSwdmxPsInfo->rBuf.au4BufSize[prSwdmxPsInfo->rBuf.u4BufReadIdx];        
        if(prSwdmxInst->u4LogFilter & check_feeder_req)
        {
            LOG(3, "add size(0x%X) to using buffer\n", req_size);
        }
        prSwdmxPsInfo->rVar.u8ReadedPos += req_size;
        prSwdmxPsInfo->rBuf.u4UsingBufSize += req_size;
        prSwdmxPsInfo->rBuf.u4BufTotalSize -= req_size;        
        prSwdmxPsInfo->rBuf.u4UsingStartAddr = prSwdmxPsInfo->rBuf.au4BufStartAddr[prSwdmxPsInfo->rBuf.u4BufReadIdx];
        prSwdmxPsInfo->rBuf.u4BufAvailCnt--;
        prSwdmxPsInfo->rBuf.u4BufReadIdx++;
        if(prSwdmxPsInfo->rBuf.u4BufReadIdx >= prSwdmxPsInfo->rVar.u4FeederBufNum)
        {
            prSwdmxPsInfo->rBuf.u4BufReadIdx = 0;
        }
        #if 0
        /****release the buffer the size = 0****/
        if(!prSwdmxPsInfo->rBuf.u4UsingBufSize &&
            IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type))
        {
            if(FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0) != FEEDER_E_OK)
            {        
                if(prSwdmxInst->u4LogFilter & check_feeder_req)
                {
                    LOG(0, "%s %d FeederUpdateReadPointer fail\n", __FUNCTION__, __LINE__);
                }
            }
        }
        #endif
    }    
}

/****this is to handle feeder req data & ack****/
static INT32 _SWDMX_PsHandleFeeder(SWDMX_INFO_T * prSwdmxInst)
{
    INT32 i4Ret = -1;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, MPG_NO_INSTANSE);

    i4Ret = _SWDMX_PsFeederReq(prSwdmxInst);
    switch(i4Ret)
    {
        case MPG_REQ_END:
            if(prSwdmxInst->u4LogFilter & check_feeder_req)
            {
                LOG(1, "all data requested and delay 10ms !\n");
            }
            x_thread_delay(10);
            break;
        case MPG_REQ_FAIL:
            x_thread_delay(10);
            break;
        case MPG_REQ_NONE:
            break;
        case MPG_REQ_OK:
            break;
        default:
            ASSERT(0);            
            break;       
    }
    if(prSwdmxPsInfo->rBuf.u4ReqTimes)
    {
        i4Ret = _SWDMX_PsFeederAck(prSwdmxInst);
        switch(i4Ret)
        {
            case MPG_ACK_OK:
                break;
            case MPG_ACK_FAIL:
                break;
            default:
                ASSERT(0);            
                break;       
        }
    }
    if((!prSwdmxPsInfo->rBuf.u4UsingBufSize) && (prSwdmxPsInfo->rBuf.u4BufAvailCnt))
    {
        _SWDMX_PsUpdateUsingBuffer(prSwdmxInst);
    }

    if(prSwdmxPsInfo->rBuf.u4UsingBufSize)
        return MPG_HAVE_DATA;
    
    return MPG_NO_DATA;    
}


/****this is used to handle to move data to dmx****/
static INT32 _SWDMX_PsDemuxer(UINT8 u1SrcId)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    INT32 i4Ret = MPG_SEND_FAIL;
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    x_memset(&rDmxMMData, 0, sizeof(rDmxMMData));
    _SWDMX_PsSetMoveDataInfo(u1SrcId, &rDmxMMData);
    i4Ret = _SWDMX_PsMoveData(u1SrcId, &rDmxMMData);
    if(i4Ret == MPG_SEND_OK)
    {
        /****update feeder read pointor****/
        _SWDMX_PsUpdateFeederPosition(u1SrcId, &rDmxMMData);        
    }
    else
    {        
        LOG(0, "%s, send data fail!\n", __FUNCTION__);
    }
    return i4Ret;
}

/****set seq hdr to vdec****/
BOOL _SWDMX_PsSetMPEGSeqHdr(SWDMX_INFO_T * prSwdmxInst, const DMX_PES_MSG_T * prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    BOOL fgRet = FALSE;
    UCHAR ucMpvId;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    if(prSwdmxPsInfo->rVar.u4SeqHdrChk > 256)         // define the check size is  "256"  more than  temporal reference
    {
        prSwdmxPsInfo->rVar.u4SeqHdrChk = 0;
        VDEC_Notify(prSwdmxInst->u1VdecId, VDEC_DEC_DECODE_CODEC_NOT_SUPPORT);
        LOG(1,"no seq hdr, dmx or decoder!\n");
        return FALSE;
    }
    else
    {
        prSwdmxPsInfo->rVar.u4SeqHdrChk++;
        /****set seq hdr****/
        if(prPes->fgSeqHeader)
        {
            prSwdmxPsInfo->rVar.fgVdecPowerOff = FALSE;
            prSwdmxPsInfo->rVar.u4SeqHdrChk = 0;
            UNUSED(VDEC_SetGetSeqHdrOnlyBegin(prSwdmxInst->u1VdecId, VDEC_FMT_MPV, &prSwdmxPsInfo->rVar.fgVdecPowerOff, &ucMpvId));
            x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
            // in getting sequence, we should not send pts to vdec
            // or we may get wrong start pts on vdec.
            rPesInfo.ucMpvId = ucMpvId;
            rPesInfo.ucEsId = prSwdmxInst->u1VdecId;
            rPesInfo.ucPicType = (UCHAR)prPes->u4FrameType;
            rPesInfo.u4FifoStart = prPes->u4BufStart;
            rPesInfo.u4FifoEnd = prPes->u4BufEnd;
            rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
            rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
            rPesInfo.fgGop = prPes->fgGop;
            x_memset(&psSeqInfo, 0, sizeof(psSeqInfo));
#if 1 // defined(CC_TRUSTZONE_SUPPORT)       
            x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
#endif
            if(VDEC_PrsSeqHdr(prSwdmxInst->u1VdecId, VDEC_FMT_MPV, &rPesInfo, &psSeqInfo))
            {
                LOG(1, "send seq sucess!\n");
                fgRet = TRUE;
            }
            UNUSED(VDEC_SetGetSeqHdrOnlyEnd(prSwdmxInst->u1VdecId, prSwdmxPsInfo->rVar.fgVdecPowerOff, ucMpvId));
            prSwdmxPsInfo->rVar.fgVdecPowerOff = FALSE;
        }
    }

    if(prSwdmxPsInfo->rVar.u1PidxVideo != 0xFF)
    {
        if(DMX_UpdateReadPointer(prSwdmxPsInfo->rVar.u1PidxVideo, prPes->u4Wp, prPes->u4Wp))
        {
            LOG(5, "line = %d, DMX_UpdateReadPointer fail\n");
        }
    }
    
    return fgRet;
}

/****set seq hdr to vdec****/
BOOL _SWDMX_PsSetH264SeqHdr(SWDMX_INFO_T * prSwdmxInst, const DMX_PES_MSG_T * prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    BOOL fgRet = FALSE;
    UCHAR ucMpvId;
    UINT32 u4Addr;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    x_memset(&rPesInfo, 0, sizeof(rPesInfo));
#if 1 // defined(CC_TRUSTZONE_SUPPORT)    
    rPesInfo.ucPicType = prPes->au1PicInfo[0];    
#else
    u4Addr = prPes->u4FrameAddr + 3;
    if(u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rPesInfo.ucPicType = *(UINT8*)VIRTUAL(u4Addr);
#endif 

    u4Addr = prPes->u4FrameAddr + 4;
    if(u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rPesInfo.u4VldReadPtr = u4Addr;
    if((rPesInfo.ucPicType & 0xF) == 0x7) // SPS_NALU
    {        
        prSwdmxPsInfo->rVar.fgVdecPowerOff = FALSE;
        UNUSED(VDEC_SetGetSeqHdrOnlyBegin(prSwdmxInst->u1VdecId, VDEC_FMT_H264, &prSwdmxPsInfo->rVar.fgVdecPowerOff, &ucMpvId));
        // in getting sequence, we should not send pts to vdec
        // or we may get wrong start pts on vdec.
        rPesInfo.ucMpvId = ucMpvId;
        rPesInfo.ucEsId = prSwdmxInst->u1VdecId;
        rPesInfo.u4FifoStart = prPes->u4BufStart;
        rPesInfo.u4FifoEnd = prPes->u4BufEnd;
        rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
        rPesInfo.fgGop = prPes->fgGop;
        x_memset(&psSeqInfo, 0, sizeof(psSeqInfo));
#if 1 // defined(CC_TRUSTZONE_SUPPORT)       
        x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
#endif
        if(VDEC_PrsSeqHdr(prSwdmxInst->u1VdecId, VDEC_FMT_H264, &rPesInfo, &psSeqInfo))
        {
            LOG(1, "send seq sucess!\n");
            fgRet = TRUE;
        }
        UNUSED(VDEC_SetGetSeqHdrOnlyEnd(prSwdmxInst->u1VdecId, prSwdmxPsInfo->rVar.fgVdecPowerOff, ucMpvId));
        prSwdmxPsInfo->rVar.fgVdecPowerOff = FALSE;
    }

    if(prSwdmxPsInfo->rVar.u1PidxVideo != 0xFF)
    {
        if(DMX_UpdateReadPointer(prSwdmxPsInfo->rVar.u1PidxVideo, prPes->u4Wp, prPes->u4Wp))
        {
            LOG(5, "line = %d, DMX_UpdateReadPointer fail\n");
        }
    }
    
    return fgRet;
}

static inline VOID _SWDMX_PsWatchFirstPts(SWDMX_MPG_INFO_T * prSwdmxPsInfo, UINT32 u4Pts)
{
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type))
    {
        if(!prSwdmxPsInfo->rVar.u4FirstPTS)
            prSwdmxPsInfo->rVar.u4FirstPTS = MPG_STC_BASE;
        return;
    }
    if(u4Pts > 0)
    {
        if(u4Pts + 30000 < prSwdmxPsInfo->rVar.u4FirstPTS)
        {
            LOG(1, "warning, first pts 0x%X -> 0x%X!",
                prSwdmxPsInfo->rVar.u4FirstPTS, u4Pts);
            prSwdmxPsInfo->rVar.u4FirstPTS = u4Pts;
        }
        else if(!prSwdmxPsInfo->rVar.u4FirstPTS)
        {
            if(u4Pts < 90000)
            {
                prSwdmxPsInfo->rVar.u4FirstPTS = MPG_STC_BASE;
            }
            else
            {
                prSwdmxPsInfo->rVar.u4FirstPTS = u4Pts;
            }
            LOG(1, "found first pts 0x%X!",
                prSwdmxPsInfo->rVar.u4FirstPTS);
        }
    }
}

static VOID _SWDMX_PsWatchAudioPts(SWDMX_INFO_T * prSwdmxInst, UINT32 u4Pts)
{
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);
    
    if(prSwdmxPsInfo->rVar.u4SeekAudPTS + prSwdmxPsInfo->rVar.u4FirstPTS > u4Pts)
    {
        return;
    }
    else if(!prSwdmxPsInfo->rVar.u4SeekAudPTS)
    {
        prSwdmxPsInfo->rVar.u4SeekAudPTS = 0;
    }
    if(prSwdmxPsInfo->rFmt.fgVideo &&
        prSwdmxPsInfo->rVar.fgEnableVideo &&
        (u4Pts < ONE_SECOND + STC_GetStcValue(prSwdmxInst->u1StcId)))
    {
        return;
    }
    else if(prSwdmxPsInfo->rFmt.fgVideo &&
        (u4Pts < prSwdmxPsInfo->rVar.u4FirstVideoPTS))
    {
        return;
    }    
    {   
        prSwdmxPsInfo->rVar.fgGotAudio = TRUE;
        prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS = u4Pts - prSwdmxPsInfo->rVar.u4FirstPTS;
        prSwdmxPsInfo->rVar.u4GetFirstAudioPTS = prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS;
        LOG(1,"first aud pts = 0x%X, first video pts = 0x%X, pts = 0x%X\n", u4Pts,
            prSwdmxPsInfo->rVar.u4FirstVideoPTS,
            prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS);
        AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS);
    }
}


/****this control audio pts to audio decoder****/
BOOL _SWDMX_PsAudioCallback(const DMX_AUDIO_PES_T * prPes)
{
    DMX_AUDIO_PES_T rPes;
    BOOL fgRet = FALSE;
    SWDMX_INFO_T *prSwdmxInst = NULL;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    
    if(!prPes)
    {
        LOG(0, "Null audio pes, dmx need check!\n");
        return FALSE;
    }
    
    prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxPsInfo = &(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo);
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if(!prSwdmxPsInfo->rFmt.fgAudio)
    {
        LOG(0, "line = %d, no audio pid, mw need check!", __LINE__);
        return FALSE;
    }
    
    if(prSwdmxInst->u4LogFilter & check_original_audio_pts)
    {
        LOG(0, "original apts = 0x%X\n", prPes->u4Pts);
    }

    x_memset(&rPes, 0, sizeof(rPes));
    x_memcpy(&rPes, prPes, sizeof(DMX_AUDIO_PES_T));
    
    if(prSwdmxPsInfo->rVar.u4LastAudioWptr)
    {
        rPes.u4Wp = prSwdmxPsInfo->rVar.u4LastAudioWptr;
    }
    prSwdmxPsInfo->rVar.u4LastAudioWptr = prPes->u4Wp;
    
    _SWDMX_PsWatchFirstPts(prSwdmxPsInfo, prPes->u4Pts);

    if(!prSwdmxPsInfo->rVar.fgGotAudio)
        _SWDMX_PsWatchAudioPts(prSwdmxInst, prPes->u4Pts);
        
    if(!prSwdmxPsInfo->rVar.fgGotAudio)
    {
        prSwdmxPsInfo->rVar.fgCleanAudio = TRUE;
        return TRUE;
    }
       
    if(rPes.u4Dts > 0)
    {
        rPes.u4Dts -= prSwdmxPsInfo->rVar.u4FirstPTS;
        rPes.u4Dts += (MPG_STC_BASE);
    }
    if(rPes.u4Pts > 0)
    {
        rPes.u4Pts -= prSwdmxPsInfo->rVar.u4FirstPTS;
        rPes.u4Pts += (MPG_STC_BASE);
    }
        
    if(prSwdmxInst->u4LogFilter & check_audio_pts)
    {
        LOG(0, "a pts = 0x%X\n", rPes.u4Pts);
    }
    if(prPes->fgEOS)
    {
        // DTV00065338, [Acceptance test] Playback Time length NOT equal in Playback Screen and File Information.
        // because audio driver does not get the lastest Wptr,
        // add this to set the last one.
        // if(prSwdmxPsInfo->rFmt.eFmt == SWDMX_FMT_RAW_AUDIO_ES)
        {
            DMX_AUDIO_PES_T rPes1Last;
            x_memset(&rPes1Last, 0, sizeof(rPes1Last));
            rPes1Last.u1DeviceId = prSwdmxInst->u1AdecId;
            if(prSwdmxPsInfo->rVar.u4TotalTime)
            {
                rPes1Last.u4Dts =  prSwdmxPsInfo->rVar.u4TotalTime + (MPG_STC_BASE);
                rPes1Last.u4Pts =  prSwdmxPsInfo->rVar.u4TotalTime + (MPG_STC_BASE);
            }
            rPes1Last.u4Wp =  rPes.u4Wp;
            if(prSwdmxPsInfo->rVar.i4CurSpeed < 0)
            {
                //rPes1Last.fgForward = TRUE;        //for trick mode 
                rPes.fgForward = FALSE;
            }
            else
            {
                //rPes1Last.fgForward = FALSE;
                rPes.fgForward = TRUE;
            }

            if (rPes.u4Wp != 0)
            {
                UNUSED(AUD_SendAudioPes(&rPes1Last));
            }
            else
            {
                LOG(5,"%s u4Wp=0, fgEOS=1, wait _SWDMX_PsHandleEos handle.\n", __FUNCTION__);
                return TRUE;
            }
        }
        rPes.u4Wp = prPes->u4Wp;
        prSwdmxPsInfo->rVar.fgSendAudioEos = TRUE;
        LOG(1, "%s Send AUD EOS. fgForward(%d)\n", __FUNCTION__, rPes.fgForward);
    }    
#ifdef SWDMX_DUMP_DATA
    _SWDMX_DumpAFifo(prSwdmxInst, rPes.u4Wp);
#endif
    fgRet = AUD_SendAudioPes(&rPes);
    if(prPes->fgEOS)
    {
        AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
    }
    return fgRet;
}

/****handle video pes, send info to decoder****/
BOOL _SWDMX_PsVideoCallBack(const DMX_PES_MSG_T * prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    SWDMX_INFO_T *prSwdmxInst = NULL;
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    BOOL fgI = FALSE;
    
    if(!prPes)
    {
        LOG(0, "Null video pes, dmx need check!\n");
        return FALSE;
    }
    
    prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxPsInfo = &(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo);
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    /****check video enable or not****/
    if(!prSwdmxPsInfo->rFmt.fgVideo)
    {
        LOG(1, "no video stream, mw need check!");
        return FALSE;
    }
        
    if(prSwdmxInst->u4LogFilter & check_original_video_pts)
    {
        LOG(0, "original vpts = 0x%X\n", prPes->u4Pts);
    }
    /****need to set seq header info to vdec****/
    if(!prSwdmxPsInfo->rVar.fgSendSeqHdr)
    {
        switch(prSwdmxInst->eVideoType)
        {
            case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
                if(_SWDMX_PsSetMPEGSeqHdr(prSwdmxInst, prPes))
                    prSwdmxPsInfo->rVar.fgSendSeqHdr = TRUE;
                break;
            case ENUM_SWDMX_VDEC_H264:
                if(_SWDMX_PsSetH264SeqHdr(prSwdmxInst, prPes))
                    prSwdmxPsInfo->rVar.fgSendSeqHdr = TRUE;
                break;
            default:
                break;
        }
        return TRUE;
    }
    if(prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq)
    {
        LOG(1, "need reset pos after found seq header!\n");
        return TRUE;
    }
    /****set the first pts****/
    _SWDMX_PsWatchFirstPts(prSwdmxPsInfo, prPes->u4Pts);       
    
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));
    
    rPesInfo.ucEsId = prSwdmxInst->u1VdecId;
    if((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
       (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK))
    {
        rPesInfo.ucEsId        = (prPes->u1Channel);
    }
    rPesInfo.ucMpvId = VLD0; // video format all use VLD expect jpeg
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
    rPesInfo.fgDtsValid = prPes->fgPtsDts;
    rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
    rPesInfo.fgGop = prPes->fgGop;
    rPesInfo.fgEos = prPes->fgEOS;
    rPesInfo.u4TotalTimeOffset = prSwdmxPsInfo->rVar.u4TotalTimeOffset;
    rPesInfo.u8Offset = prSwdmxPsInfo->rVar.u8PreviousSendedSize;
    rPesInfo.u8OffsetI = prSwdmxPsInfo->rVar.u8PreviousIPos;
    rPesInfo.u2DecodingOrder = prSwdmxPsInfo->rVar.u2OrderFromIPos;
    rPesInfo.u4Timestap = prSwdmxPsInfo->rVar.u4FirstPTS;    
#ifdef CC_VDEC_FMT_DETECT
    rPesInfo.u4DmxFrameType = prPes->u4FrameType;
#endif

    if (prSwdmxInst->eVideoType == ENUM_SWDMX_VDEC_MPEG1_MPEG2)
    {
        switch (prPes->u4FrameType)
        {
        case PIC_TYPE_I:
            rPesInfo.ucPicType = MPV_I_TYPE;            
            fgI = TRUE;
            break;

        case PIC_TYPE_P:
            rPesInfo.ucPicType = MPV_P_TYPE;
            break;

        case PIC_TYPE_B:
            rPesInfo.ucPicType = MPV_B_TYPE;
            break;

        case PIC_TYPE_SEQ_END:
            rPesInfo.ucPicType = MPV_I_TYPE;
            rPesInfo.fgSeqEnd = TRUE;
            break;

        default:
            rPesInfo.ucPicType = 0;
        #ifndef CC_VDEC_FMT_DETECT
            ASSERT(0);
            break;
        #else
            return TRUE;
        #endif
        }        
    }
    /****add h.264 support****/
    else if (prSwdmxInst->eVideoType == ENUM_SWDMX_VDEC_H264)
    {
        UINT32 u4Addr;
#if 1 //defined(CC_TRUSTZONE_SUPPORT)
        rPesInfo.ucPicType = prPes->au1PicInfo[0];        
#else
        u4Addr = prPes->u4FrameAddr + 3;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.ucPicType = *(UINT8*)VIRTUAL(u4Addr);        
#endif

        u4Addr = prPes->u4FrameAddr + 4;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;

        if((rPesInfo.ucPicType & 0xF) == 0x5) // IDR_NALU
        {          
            fgI = TRUE;
        }
    }
    if(!fgI)
    {
        // Sample a key frame is there is no IDR
        // Currently, key is 3 second
        UINT32 u4Tmp;
        UINT64 u8Size;
        if((prSwdmxPsInfo->rVar.u4TotalTime) > ONE_SECOND)
        {
            u4Tmp = (prSwdmxPsInfo->rVar.u4TotalTime / ONE_SECOND); // total times, seconds
            /****1 sec data size****/
            u8Size = _SWDMX_Div6464((prSwdmxPsInfo->rVar.u8FileSize - prSwdmxPsInfo->rVar.u8StartFileOffset), 
                (UINT64)u4Tmp, NULL);
            u8Size *= 3;
        }
        else
        {
            u8Size = (UINT64)VDEC_GetAvgPicSize(prSwdmxInst->u1VdecId);
            u8Size *= 90;
        }
        if((prSwdmxPsInfo->rVar.u8PreviousSendedSize
            - prSwdmxPsInfo->rVar.u8PreviousIPos) > u8Size)
        {
            fgI = TRUE;
        }
    }
    if(fgI)
    {
        prSwdmxPsInfo->rVar.u8PreviousIPos = 
            prSwdmxPsInfo->rVar.u8PreviousSendedSize;
        if(prSwdmxInst->u4LogFilter & check_original_video_pts)
        {
            LOG(1, "I, pos = %llx\n", prSwdmxPsInfo->rVar.u8PreviousIPos);
        }
    }
    if(prPes->u4Pts)
    {
        rPesInfo.u4PTS += (MPG_STC_BASE + prPes->u4Pts);
        rPesInfo.u4PTS -= prSwdmxPsInfo->rVar.u4FirstPTS;
    }
    if(prPes->u4Dts)
    {
        rPesInfo.u4DTS += (MPG_STC_BASE + prPes->u4Dts);
        rPesInfo.u4DTS -= prSwdmxPsInfo->rVar.u4FirstPTS;
    }    
    if(prPes->fgEOS)
    {
        if(prSwdmxPsInfo->rVar.i4CurSpeed < 0)
        {
            rPesInfo.fgBackward = TRUE;
        }        
        if(((prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MVC) ||
            (prSwdmxInst->eSubVideoType == ENUM_SWDMX_VDEC_MULTITRACK)) &&
           (prPes->u1Channel == prSwdmxInst->u1Vdec2Id))
        {
            LOG(3, "video2 eos!\n");
            prSwdmxPsInfo->rVar.fgSendVideo2Eos = TRUE;
        }
        else //(prPes->u1Channel == ES0)
        {
            LOG(3, "video eos!\n");
            prSwdmxPsInfo->rVar.fgSendVideoEos = TRUE;
        }
    }   
    /****when video callback, pause video****/
    if((!prSwdmxPsInfo->rVar.fgGotVideo) && 
        (!prSwdmxPsInfo->rVar.fgVideoPause) &&
        !SKIP_MODE(prSwdmxPsInfo->rVar.i4CurSpeed))
    {
        LOG(1, "pause video for start play\n");
        if(_SWDMX_PsVideoPause(prSwdmxInst))
        {
            prSwdmxPsInfo->rVar.fgVideoPause = TRUE;
        }        
        /****set the file size and total play time to vdec****/
        VDEC_SetTotalTime(prSwdmxInst->u1VdecId, prSwdmxPsInfo->rVar.u4TotalTime);
        VDEC_SetFileSize(prSwdmxInst->u1VdecId, prSwdmxPsInfo->rVar.u8FileSize);
    }
    /****set fgGotVideo****/
    if(!prSwdmxPsInfo->rVar.fgGotVideo)
    {
        /****found I or after 90 P/B****/
        if(fgI)
        {
            prSwdmxPsInfo->rVar.fgGotVideo = TRUE;            
            LOG(1, "got video, video pts = 0x%X, pts = 0x%X\n", 
                prPes->u4Pts, rPesInfo.u4PTS);
        }
    }
    /****u4FirstVideoPTS used to control how to send audio data****/
    if(prSwdmxPsInfo->rVar.fgGotVideo)
    {
        if(prSwdmxPsInfo->rVar.u4FirstVideoPTS == 0xFFFFFFFF)
        {
            prSwdmxPsInfo->rVar.u4FirstVideoPTS = prPes->u4Pts;
        }
        else
        {
            if((prPes->u4Pts < prSwdmxPsInfo->rVar.u4FirstVideoPTS) &&
                ((prSwdmxPsInfo->rVar.u4FirstVideoPTS - prPes->u4Pts) > (10000)))
            {
                prSwdmxPsInfo->rVar.u4FirstVideoPTS = prPes->u4Pts;
            }
        }
    }
    if(prSwdmxInst->u4LogFilter & check_video_pts)
    {
        LOG(0, "v pts = 0x%X ucPicType 0x%X \n", rPesInfo.u4PTS,rPesInfo.ucPicType);
    }           
#if 1 // defined(CC_TRUSTZONE_SUPPORT)   
    x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
#endif

#ifdef SWDMX_DUMP_DATA
    _SWDMX_DumpVFifo(prSwdmxInst, rPesInfo.u4VldReadPtr);
#endif
    UNUSED(VDEC_SendEs((void*)&rPesInfo));
    return TRUE;
}

/****set stream id info****/
BOOL _SWDMX_PsSetStreamId(UINT8 u1SrcId, UINT8 u1StrmType,
    UINT32 u4StrmID, UINT32 u4StrmSubID)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    LOG(1, "Set strm(%d) id = 0x%X, sub id = 0x%X\n", u1StrmType, u4StrmID, u4StrmSubID);
    
    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxPsInfo->rFmt.u2VideoId = (UINT16)u4StrmID;
            break;
        case eSWDMX_STRM_TYPE_VID2:
            prSwdmxPsInfo->rFmt.u2Video2Id = (UINT16)u4StrmID;
            break;
        case eSWDMX_STRM_TYPE_AUD:
            if ((prSwdmxPsInfo->rFmt.u2AudioId != (UINT16)u4StrmID) ||
                (prSwdmxPsInfo->rFmt.u2AudioSubId != (UINT16)u4StrmSubID))
            {
                prSwdmxPsInfo->rVar.u2PrevAStrmID = prSwdmxPsInfo->rFmt.u2AudioId;
                prSwdmxPsInfo->rVar.u2PrevSubAStrmID = prSwdmxPsInfo->rFmt.u2AudioSubId;
                prSwdmxPsInfo->rFmt.u2AudioId = (UINT16)u4StrmID;
                prSwdmxPsInfo->rFmt.u2AudioSubId = (UINT16)u4StrmSubID;
            }
            break;
        case eSWDMX_STRM_TYPE_AUD2:
            prSwdmxPsInfo->rFmt.u2Audio2Id = (UINT16)u4StrmID;
            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxPsInfo->rFmt.u2SubTitleId = (UINT16)u4StrmID;
            break;
        default:
            LOG(1, "line = %d, error set type!\n", __LINE__);
            break;
    }
    return TRUE;
}

/****enable video stream to dmx****/
static BOOL _SWDMX_PsEnableVideoStrm(SWDMX_INFO_T * prSwdmxInst, UINT8 u1Pidx, UINT16 u2VideoId)
{
    DMX_PS_T rDmxPS;
    UINT32 u4Flags = (UINT32)(DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_VALID
        | DMX_PID_FLAG_STREAM_ID | DMX_PID_FLAG_INSTANCE_TAG 
        | DMX_PID_FLAG_DEVICE_ID);

    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(1, "Can not get pid from dmx, dmx need check!\n");
        return FALSE;
    }
    
    x_memset(&rDmxPS, 0, sizeof(rDmxPS));
    rDmxPS.u1StreamID = (UINT8)u2VideoId;
    {
        UINT32 addr, size;
        if(!_SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &addr, &size))
        {
            LOG(3, "line = %d, get audio fifo info fail!\n", __LINE__);
            return FALSE;
        }
        rDmxPS.fgAllocateBuffer   = FALSE;
        rDmxPS.u4BufAddr          = addr;
        rDmxPS.u4BufSize          = size;
    }
    rDmxPS.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rDmxPS.fgEnable = TRUE;
    rDmxPS.u1DeviceId = VLD0;
    rDmxPS.u1ChannelId = (prSwdmxInst->u1VdecId);
    rDmxPS.pvInstanceTag = (void*)prSwdmxInst;
    if(!DMX_PS_SetStream(u1Pidx, u4Flags, &rDmxPS))
    {
        LOG(1, "set dmx stream fail, dmx need check!\n");
        return FALSE;
    }    
    return TRUE;
}

/****enable audio id to dmx****/
static BOOL _SWDMX_PsEnableAudioStrm(SWDMX_INFO_T * prSwdmxInst, UINT8 u1Pidx, 
    UINT16 u2AudioId, UINT16 u2AudioSubId)
{
    DMX_PS_T rDmxPS;
    DMX_AUDIO_TYPE_T eAudType = DMX_AUDIO_MPEG;
    UINT32 u4Flags = (UINT32)(DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_STREAM_ID
        | DMX_PID_FLAG_INSTANCE_TAG | DMX_PID_FLAG_DEVICE_ID);

    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);
    
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(1, "Can not get pid from dmx, dmx need check!\n");
        return FALSE;
    }
    
    x_memset(&rDmxPS, 0, sizeof(rDmxPS));
    rDmxPS.u1StreamID = (UINT8)u2AudioId;
    if(u2AudioSubId)
    {
        rDmxPS.u1SubstreamID = (UINT8)u2AudioSubId;
        rDmxPS.fgEnableSsidFilter = TRUE;
        u4Flags |= DMX_PID_FLAG_SUBSTREAM_ID;
    }
    {
        UINT32 addr, size;
        if(!_SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &addr, &size))
        {
            LOG(3, "line = %d, get audio fifo info fail!\n", __LINE__);
            return FALSE;
        }
        rDmxPS.fgAllocateBuffer   = FALSE;
        rDmxPS.u4BufAddr          = addr;
        rDmxPS.u4BufSize          = size;
    }
    rDmxPS.pvInstanceTag = (void*)prSwdmxInst;
    rDmxPS.u1DeviceId = (prSwdmxInst->u1AdecId);
    rDmxPS.u1ChannelId = (prSwdmxInst->u1AdecId);
    rDmxPS.ePidType = DMX_PID_TYPE_ES_AUDIO;
    /****set stream****/
    if(!DMX_PS_SetStream(u1Pidx, u4Flags, &rDmxPS))
    {
        LOG(1, "line = %d, DMX_PS_SetStream fail!\n", __LINE__);
        return FALSE;
    }
    if(u2AudioId == 0xBD)
    {
        if(u2AudioSubId >= 0x80 && u2AudioSubId <= 0x88)
        {
            LOG(1, "audio type is DMX_AUDIO_AC3\n");
            eAudType = DMX_AUDIO_AC3;
        }
        else if ((u2AudioSubId & 0xF8) == 0x88)
        {
            LOG(1, "audio type is DMX_AUDIO_DTS\n");
            eAudType = DMX_AUDIO_DTS;
        }
        else if(u2AudioSubId >= 0xA0 && u2AudioSubId <= 0xA8)
        {
            LOG(1, "audio type is DMX_AUDIO_LPCM\n");
            eAudType = DMX_AUDIO_LPCM;
        }
    }
    else if(u2AudioId >= 0xC0 && u2AudioId <= 0xDF)
    {
        LOG(1, "audio type is DMX_AUDIO_MPEG\n");
    }
    else
    {
        LOG(1, "u2AudioId 0x%X Unknown -> set MPEG\n", u2AudioId);
        eAudType = DMX_AUDIO_MPEG;
    }
    /****set stream type****/
    if(!DMX_PS_SetAudioType(u1Pidx, eAudType))
    {
        LOG(1, "line = %d, DMX_PS_SetAudioType fail!\n");
        return FALSE;
    }
    /****set stream enable****/
    u4Flags = (UINT32)(DMX_PID_FLAG_VALID);
    rDmxPS.fgEnable = TRUE;    
    if(!DMX_PS_SetStream(u1Pidx, u4Flags, &rDmxPS))
    {
        LOG(1, "line = %d, DMX_PS_SetStream Fail\n", __LINE__);
        return FALSE;
    }
    return TRUE;
}

/****disable video/audio stream to dmx****/
static BOOL _SWDMX_PsFreePidx(UINT8 u1Pidx)
{
    DMX_PS_T rDmxPS;
    
    if(u1Pidx != 0xFF)
    {
        x_memset(&rDmxPS, 0, sizeof(rDmxPS));
        rDmxPS.fgEnable = FALSE;
        if(!DMX_PS_SetStream(u1Pidx, DMX_PID_FLAG_VALID, &rDmxPS))
        {
            LOG(1, "line = %d, DMX_PS_SetStream fail, dmx need check!\n", __LINE__);
            return FALSE;
        }
        if(!DMX_PS_FreeStream(u1Pidx))
        {
            LOG(1, "line = %d, DMX_PS_FreeStream fail, dmx need check!\n", __LINE__);
            return FALSE;
        }
        if(!DMX_MUL_FreePidx(u1Pidx))
        {
            LOG(1, "line = %d, DMX_MUL_FreePidx fail, dmx need check!\n", __LINE__);
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/****set info****/
BOOL _SWDMX_PsSetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    LOG(1, "%s, type = %d, value = %d\n", __FUNCTION__, u1Type, u4Para1);

    switch(u1Type)
    {
        case eSWDMX_PID_INDEX_START:
            prSwdmxPsInfo->rFmt.u2PidIdxStart = (UINT16)(u4Para1);
            break;
        case eSWDMX_SET_AOUT_ENABLE:
            prSwdmxPsInfo->rVar.fgEnableAudio = TRUE;
            LOG(1, "audio out enable by vdp\n");
            break;
        default:
            break;
    }
    return TRUE;
}

/****mw/swdmx get info from ps****/
BOOL _SWDMX_PsGetInfo(UINT8 u1SrcId, UINT8 u1Type,
    UINT32 * pu4Para1,UINT32 * pu4Para2, UINT32 * pu4Para3)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    UINT64 u8CurPos = 0;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    switch(u1Type)
    {
        case eSWDMX_GET_CURRENT_POSITION:
            if(_SWDMX_PsGetCurrentPlayPos(u1SrcId, &u8CurPos))
            {                
                if(prSwdmxInst->u4LogFilter & check_getinfo)
                    LOG(1, "now playpos = 0x%llX\n", u8CurPos);
                *pu4Para1 = u8CurPos;
                return TRUE;
            }          
            break;
        case eSWDMX_GET_CURRENT_TIME:
            if(_SWDMX_PsGetCurrentPlayPos(u1SrcId, &u8CurPos))
            {                
                if(prSwdmxInst->u4LogFilter & check_getinfo)
                    LOG(1, "now playpos = 0x%llX\n", u8CurPos);
                {
                    UINT64 tmp = 0;
                    tmp = _SWDMX_Div6464((prSwdmxPsInfo->rVar.u8FileSize - 
                        prSwdmxPsInfo->rVar.u8StartFileOffset) * (UINT64)90000,
                                      (UINT64)prSwdmxPsInfo->rVar.u4TotalTime,
                                      NULL);
                    if(tmp)
                    {
                        u8CurPos -= prSwdmxPsInfo->rVar.u8StartFileOffset;
                        if(u8CurPos)
                        {
                            *pu4Para1 = (UINT32)_SWDMX_Div6464(u8CurPos * (UINT64)90000,
                                                       tmp, NULL);
                        }
                        else
                        {
                            *pu4Para1 = 0;
                        }
                    }
                    else
                    {
                        *pu4Para1 = 0;
                    }
                }
                return TRUE;
            }           
            break;
        case eSWDMX_GET_IS_TRICK:
            *pu4Para1 = TRUE;
            break;
        case eSWDMX_GET_HDMVC_INFO:
            *pu4Para1 = (UINT32)prSwdmxPsInfo->rVar.fgHdMvc;
            break;
        default:
            break;
            
    }
    return TRUE;
}

/****audio cb from audio, tell audio triger audio pts****/
BOOL _SWDMX_PsAudioCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);

    if(eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        LOG(1, "Audio is ready===========================\n");
        
        if(prSwdmxPsInfo->rFmt.fgAudio)
        {
            prSwdmxPsInfo->rVar.fgGotAudioReady = TRUE;
        }
        
        if((prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS != 0xFFFFFFFF) &&
            (prSwdmxPsInfo->rFmt.fgVideo))
        {
            LOG(1, "%s, triger audio pts = 0x%X, stc = 0x%X\n",
                __FUNCTION__,
                prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS,
                STC_GetStcValue(prSwdmxInst->u1StcId));
            VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS);
            prSwdmxPsInfo->rVar.u4FirstAudioModifyPTS = 0xFFFFFFFF;
        }
        if(prSwdmxPsInfo->rVar.fgEnableAudio)
        {
            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
            LOG(1, "re-enable audio\n");
        }
    }
    
    return TRUE;
}


/****enable stream****/
BOOL _SWDMX_PsEnableStrm(UINT8 u1SrcId, UINT8 u1StrmType, VOID * pfnCbFunc)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    UINT8 u1Pidx = 0xFF;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    LOG(1, "Begin %s\n", __FUNCTION__);
    
    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            if(prSwdmxPsInfo->rFmt.fgVideo)
            {
                LOG(1, "Enable video 1 again, mw need check!\n");
                return FALSE;
            }
            if(prSwdmxPsInfo->rVar.u1PidxVideo != 0xFF)
            {
                LOG(1, "Video id is not 0xFF before init, need check!\n");
                return FALSE;
            }
            u1Pidx = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            if(_SWDMX_PsEnableVideoStrm(prSwdmxInst, u1Pidx, prSwdmxPsInfo->rFmt.u2VideoId))
            {
                prSwdmxPsInfo->rFmt.fgVideo = TRUE;
                prSwdmxPsInfo->rVar.u1PidxVideo = u1Pidx;
                prSwdmxPsInfo->rVar.fgGotVideo = FALSE;
                prSwdmxPsInfo->rVar.fgGotVideoReady = FALSE;
                prSwdmxPsInfo->rVar.fgEnableVideo = FALSE;
                prSwdmxPsInfo->rVar.fgSendVideoEos = FALSE;
                prSwdmxPsInfo->rVar.fgSendVideo2Eos = FALSE;
            }
            else
            {
                LOG(1, "Enable video 1 fail\n");
            }
            break;
        case eSWDMX_STRM_TYPE_VID2:
            if(prSwdmxPsInfo->rFmt.fgVideo2)
            {
                LOG(1, "Enable video 2 again, mw need check!\n");
                return FALSE;
            }
            if(prSwdmxPsInfo->rVar.u1PidxVideo2 != 0xFF)
            {
                LOG(1, "Video 2 id is not 0xFF before init, need check!\n");
                return FALSE;
            }
            u1Pidx = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            if(_SWDMX_PsEnableVideoStrm(prSwdmxInst, u1Pidx, prSwdmxPsInfo->rFmt.u2Video2Id))
            {
                prSwdmxPsInfo->rFmt.fgVideo2= TRUE;
                prSwdmxPsInfo->rVar.u1PidxVideo2 = u1Pidx;
            }
            else
            {
                LOG(2, "enable video 2 fail\n");
            }
            break;
        case eSWDMX_STRM_TYPE_AUD:
            if(prSwdmxPsInfo->rFmt.fgAudio)
            {
                LOG(1, "enable aud 1 again, mw need check!\n");
                return FALSE;
            }
            if(prSwdmxPsInfo->rVar.u1PidxAudio != 0xFF)
            {
                LOG(1, "aud 1 id is not 0xFF before init, need check!\n");
                return FALSE;
            }
            u1Pidx = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            if(_SWDMX_PsEnableAudioStrm(prSwdmxInst, u1Pidx,
                prSwdmxPsInfo->rFmt.u2AudioId, prSwdmxPsInfo->rFmt.u2AudioSubId))
            {                
                prSwdmxPsInfo->rFmt.fgAudio = TRUE;
                prSwdmxPsInfo->rVar.u1PidxAudio = u1Pidx;
                prSwdmxPsInfo->rVar.fgGotAudio = FALSE;
                prSwdmxPsInfo->rVar.fgGotAudioReady = FALSE;
                prSwdmxPsInfo->rVar.fgEnableAudio = FALSE;
                prSwdmxPsInfo->rVar.fgSendAudioEos = FALSE;

                if ((prSwdmxPsInfo->rVar.fgBeginToPlay) &&
                    (prSwdmxPsInfo->rVar.u2PrevAStrmID != 0xFFFF) &&
                    ((prSwdmxPsInfo->rVar.u2PrevAStrmID != prSwdmxPsInfo->rFmt.u2AudioId) ||
                     (prSwdmxPsInfo->rVar.u2PrevSubAStrmID != prSwdmxPsInfo->rFmt.u2AudioSubId)))
                {
                    prSwdmxPsInfo->rVar.fgChgATrack = TRUE;
                    LOG(0, "Enable audio, change audio track\n");
                }
                prSwdmxPsInfo->rVar.u2PrevAStrmID = prSwdmxPsInfo->rFmt.u2AudioId;
                prSwdmxPsInfo->rVar.u2PrevSubAStrmID = prSwdmxPsInfo->rFmt.u2AudioSubId;
            }
            else
            {
                LOG(2, "enable audio 1 fail\n");
            }
            break;
        case eSWDMX_STRM_TYPE_AUD2:
            LOG(1, "enable second video stream, need check!\n");
            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            break;
        default:
            LOG(1, "line = %d, error set type!\n", __LINE__);
            break;
    }
    LOG(1, "Enable %d sucess!\n", u1StrmType);
    
    return TRUE;
}

/****free stream****/
BOOL _SWDMX_PsDisableStrm(UINT8 u1SrcId, UINT8 u1StrmType)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    LOG(1, "End %s\n", __FUNCTION__);
    
    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            if(!prSwdmxPsInfo->rFmt.fgVideo)
            {
                LOG(1, "disable first video 0 again, mw need check!\n");
                return TRUE;
            }
            if(prSwdmxPsInfo->rVar.u1PidxVideo == 0xFF)
            {
                LOG(1, "invalid video stream id, need check!\n");
                return FALSE;
            }
            if(_SWDMX_PsFreePidx(prSwdmxPsInfo->rVar.u1PidxVideo))
            {
                prSwdmxPsInfo->rVar.fgEnableVideo = FALSE;
                prSwdmxPsInfo->rVar.fgGotVideo = FALSE;
                prSwdmxPsInfo->rFmt.fgVideo = FALSE;
                prSwdmxPsInfo->rVar.u1PidxVideo = 0xFF;
            }
            else
            {
                LOG(1, "disable video fail\n");
            }
            break;
        case eSWDMX_STRM_TYPE_VID2:
            if(!prSwdmxPsInfo->rFmt.fgVideo2)
            {
                LOG(1, "disable second video again, mw need check!\n");
                return TRUE;
            }
            if(prSwdmxPsInfo->rVar.u1PidxVideo2 == 0xFF)
            {
                LOG(1, "invalid video 2 stream id, need check!\n");
                return FALSE;
            }
            if(_SWDMX_PsFreePidx(prSwdmxPsInfo->rVar.u1PidxVideo2))
            {
                prSwdmxPsInfo->rFmt.fgVideo2 = FALSE;
                prSwdmxPsInfo->rVar.u1PidxVideo2 = 0xFF;
            }
            else
            {
                LOG(1, "disable video2 fail\n");
            }
            break;
        case eSWDMX_STRM_TYPE_AUD:
            if(!prSwdmxPsInfo->rFmt.fgAudio)
            {
                LOG(1, "disable audio again, mw need check!\n");
                return TRUE;
            }
            if(prSwdmxPsInfo->rVar.u1PidxAudio == 0xFF)
            {
                LOG(1, "invalid audio stream id, need check!\n");
                return FALSE;
            }
            if(_SWDMX_PsFreePidx(prSwdmxPsInfo->rVar.u1PidxAudio))
            {
                prSwdmxPsInfo->rFmt.fgAudio = FALSE;
                prSwdmxPsInfo->rVar.fgGotAudio = FALSE;
                prSwdmxPsInfo->rVar.fgGotAudioReady = FALSE;
                prSwdmxPsInfo->rVar.fgEnableAudio = FALSE;
                prSwdmxPsInfo->rVar.u1PidxAudio = 0xFF;
            }
            else
            {
                LOG(1, "disable audio fail\n");
            }
            break;
        case eSWDMX_STRM_TYPE_AUD2:
            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            break;
        default:
            LOG(1, "line = %d, error set type!\n", __LINE__);
            break;
    }
    LOG(1, "disable %d sucess!\n", u1StrmType);
    return TRUE;
}

/****init for ps, set info to dmx****/
BOOL _SWDMX_PsInit(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_MPG_VERIFY_NULL_ERROR(prSwdmxInst); 
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, FALSE);
    
    _SWDMX_PsVariableInit(prSwdmxPsInfo);
    prSwdmxPsInfo->rFmt.eFmt = eType;
    prSwdmxInst->u4Multiple = 5; // for DLNA-cert to change feeder request size
    
    LOG(1, "%s, file format = %d\n", __FUNCTION__, eType);
    
    switch(eType)
    {
        case SWDMX_FMT_MPEG1_DAT:
            if(DMX_PS_SetDataFormat(prSwdmxInst->u1DmxId, DMX_DATA_FORMAT_DAT))
            {
                LOG(1, "set DMX_DATA_FORMAT_DAT to dmx\n");                
            }
            else
            {
                LOG(1, "set ps dat fail, check dmx!\n");
            }
            break;
        case SWDMX_FMT_MPEG2_PS:
            if(DMX_PS_SetDataFormat(prSwdmxInst->u1DmxId, DMX_DATA_FORMAT_RAW))
            {
                LOG(1, "set DMX_DATA_FORMAT_RAW to dmx\n");
            }
            else
            {
                LOG(1, "set ps raw fail, check dmx!\n");
            }
            break;
        default:
            LOG(1, "line = %d, unknow file format!", __LINE__);
            break;
    }
    DMX_SetToDecoder(TRUE);
    return TRUE;
}

/****set range****/
VOID _SWDMX_PsSetRange(UINT8 u1SrcId, SWDMX_RANGE_LIST_T * prRangeList, UINT32 u4VidStrmID,
                                      UINT32 u4AudStrmID, UINT32 u4SubTitleID)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    MM_RANGE_INFO_T *prRangeInfo = NULL;
    MM_RANGE_DECODER_T *prRangeDecoder = NULL;
    SWDMX_MPG_VERIFY_NULL(prSwdmxInst);
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL(prSwdmxPsInfo);

    if (!prRangeList)
    {
        LOG(1, "%s prRangeList is NULL.\n", __FUNCTION__);
        return ;
    }
    
    prSwdmxPsInfo->rVar.prCurRangeReq = prRangeList;
    prRangeInfo = prRangeList->prDmxRangeInfo;
    if (!prRangeInfo)
    {
        LOG(0, "range info is null, mw need check!\n");
        prSwdmxPsInfo->rVar.e_mm_src_type = MM_SRC_TYPE_HIGH_SPEED_STORAGE;
        return;        
    }

    prSwdmxPsInfo->rVar.e_mm_src_type = prRangeInfo->e_mm_src_type;
    prSwdmxPsInfo->rVar.u8StartFileOffset = prRangeInfo->ui8_fileoffset;
    prSwdmxPsInfo->rVar.u8FileSize = prRangeInfo->z_range_sz + prRangeInfo->ui8_fileoffset;
    prSwdmxPsInfo->rVar.u4TotalTime = (UINT32)prRangeInfo->ui8_pb_duration;
    prSwdmxPsInfo->rVar.u4FirstPTS = (UINT32)prRangeInfo->t_pts_offset;        
    prSwdmxPsInfo->rVar.i4CurSpeed = 1000;
    //prSwdmxPsInfo->rVar.u4TotalTimeOffset = 0;
    if(prSwdmxPsInfo->rVar.u8FileSize == 0xFFFFFFFF)
    {            
        LOG(0, "invalid file size, mw need check!\n");
    }
    else if(prSwdmxPsInfo->rVar.u8StartFileOffset > prSwdmxPsInfo->rVar.u8FileSize)
    {
        LOG(0, "error start file position, mw need check!\n");
        prSwdmxPsInfo->rVar.u8StartFileOffset = 0;
    }
    {
        LOG(1, "f_size(0x%llX), s_pos(0x%llX), t_time(0x%X), s_pts(0x%X), s_type(%d)\n",
                prSwdmxPsInfo->rVar.u8FileSize,
                prSwdmxPsInfo->rVar.u8StartFileOffset,
                prSwdmxPsInfo->rVar.u4TotalTime,
                prSwdmxPsInfo->rVar.u4FirstPTS,
                prSwdmxPsInfo->rVar.e_mm_src_type);
    }

    prRangeDecoder = prRangeInfo->pt_aud_decoder_info;
    while (prRangeDecoder)
    {
        prRangeDecoder = prRangeDecoder->pt_next;
        prSwdmxPsInfo->rVar.u1TotalATrack++;
    }
    LOG(1, "audio track is %d.\n", prSwdmxPsInfo->rVar.u1TotalATrack);

    if (IS_PUSH_MODE(prSwdmxPsInfo->rVar.e_mm_src_type) ||
        (prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK))
    {
        LOG(1, "push/timeSeek mode, no need found sequence header\n");
        prSwdmxPsInfo->rVar.fgSendSeqHdr = TRUE;
    }
    _SWDMX_PsSetSendedPos(prSwdmxPsInfo, prSwdmxPsInfo->rVar.u8StartFileOffset);
    prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    prRangeList->u4CurPTS = prRangeInfo->t_vid_start_render_pts;
    _SWDMX_PsSetReqSize(prSwdmxPsInfo);
    _SWDMX_PsResetData(prSwdmxInst, FALSE);
    STC_StopStc(prSwdmxInst->u1StcId);
}


/****handle control flow****/
SWDMX_HDR_PRS_STATE_T _SWDMX_PsHdrPrs(UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList,
    SWDMX_CUR_CMD_T* prCmd)
{
    SWDMX_INFO_T *prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_MPG_INFO_T *prSwdmxPsInfo = NULL;
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxInst, eSWDMX_HDR_UNKNOWN_TYPE);
    
    prSwdmxPsInfo = (SWDMX_MPG_INFO_T*)(&(prSwdmxInst->rFmtInfo.rSwdmxMpgInfo));
    SWDMX_PS_POINTER_VERIFY_NULL_ERROR(prSwdmxPsInfo, eSWDMX_HDR_UNKNOWN_TYPE);
    
    if(!prCmd)
    {
        LOG(0, "%s, prCmd is NULL\n", __FUNCTION__);
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    if((!prSwdmxPsInfo->rFmt.fgAudio) && (!prSwdmxPsInfo->rFmt.fgVideo))
    {
        {
            /****check enable stream****/
            LOG(0, "%s, no video and no audio!\n", __FUNCTION__);
        }
        //ASSERT(0);
    }

    if ((!prSwdmxPsInfo->rVar.fgBeginToPlay) &&
        (prCmd->i4PlaySpeed == 1000))
    {
        if (prSwdmxPsInfo->rFmt.fgVideo)
        {
            _SWDMX_PsWaitVdecReady(prSwdmxInst);
        }
        prSwdmxPsInfo->rVar.fgBeginToPlay = TRUE;
    }

    /****before req & send data, do some for the first time****/
    if(prSwdmxPsInfo->rVar.fgFirst)
    {
        /****check the feeder state****/
        if(FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &(prSwdmxInst->rFeederInfo)) != FEEDER_E_OK)
        {
            LOG(1, "%s FeederGetBufferInfo Fail [0x%x] -> [0x%x]\n", __FUNCTION__,
                prSwdmxInst->rFeederInfo.u4StartAddr, prSwdmxInst->rFeederInfo.u4EndAddr);
            x_thread_delay(50);
            return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
        }
        LOG(1, "%s feeder buf [0x%x] -> [0x%x]\n", __FUNCTION__,
                prSwdmxInst->rFeederInfo.u4StartAddr, prSwdmxInst->rFeederInfo.u4EndAddr);        
        prSwdmxPsInfo->rVar.fgFirst = FALSE;
    }

    if(prSwdmxPsInfo->rVar.fgEnableAudio && prSwdmxPsInfo->rVar.fgChgATrack)
    {
        prSwdmxPsInfo->rVar.fgChgATrack = FALSE;
        LOG(0, "[%s][%d] Set fgChgAtrack Ture, when audio enabled\n", __FUNCTION__, __LINE__); 
    }
    
    /****handle change for seek & speed change****/
    if(prCmd->fgChange)
    {
        _SWDMX_PsHandleChange(prSwdmxInst, prCmd);
    }      
    
    /****found first seq header****/
    if(((!prSwdmxPsInfo->rVar.fgSendSeqHdr) || 
        (prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq)) && 
        (prSwdmxPsInfo->rFmt.fgVideo))
    {
        if(_SWDMX_PsGetSeqHeader(prSwdmxInst))
        {
            LOG(1, "have found seq header!!!\n");
        }
    }

    /****handle seek postion for FF & FB****/
    if (prSwdmxPsInfo->rVar.fgJumpping)
    {
        _SWDMX_PsHandleSkipMode(prSwdmxInst);
    }

    /****send eos and free range****/
    if(_SWDMX_PsFileEnd(prSwdmxInst, 0))
    {
        x_thread_delay(5);        
        _SWDMX_PsHandleEos(prSwdmxInst);        
        if(prRangeList)
        {
            prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
            if(!prSwdmxPsInfo->rFmt.fgVideo && !prSwdmxPsInfo->rFmt.fgAudio)
            {
                prRangeList->eRangeStats = eSWDMX_RANGE_STATE_FINISHED;
                _SWDMX_PsResetData(prSwdmxInst, 1);
                LOG(1, "play end!\n");
            }
            else
            {
                LOG(1, "waiting play end!\n");
            }
        }        
        return eSWDMX_HDR_PRS_SUCCEED;
    }    
    if(prSwdmxInst->u4LogFilter & check_sended_size)
    {
        LOG(0, "sended size = 0x%llX\n", prSwdmxPsInfo->rVar.u8SendedSize);
    }
    
    /****request data****/
    if(_SWDMX_PsHandleFeeder(prSwdmxInst) == MPG_NO_DATA)
    {
        if(prSwdmxInst->u4LogFilter & check_feeder_req)
            LOG(0, "no data, need check why!\n");
        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }

    /****judge fifo is full or not****/
    if(_SWDMX_PsFifoIsFull(prSwdmxInst))
    {
        /**** start to play****/
        if((prSwdmxPsInfo->rFmt.fgAudio && !prSwdmxPsInfo->rVar.fgEnableAudio) ||
            (prSwdmxPsInfo->rFmt.fgVideo && !prSwdmxPsInfo->rVar.fgEnableVideo &&
            prSwdmxPsInfo->rVar.fgSendSeqHdr && !prSwdmxPsInfo->rVar.fgResetPosAfterFoundSeq))
        {
            _SWDMX_PsEnablePlay(prSwdmxInst);
        }
        /****this only for network source****/
        prSwdmxInst->fgFifoFull = TRUE;
        x_thread_delay(5);
        return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
    }

    /****demuxer****/
    if(_SWDMX_PsDemuxer(u1SrcId) == MPG_SEND_OK)
    {
        if(prSwdmxPsInfo->rVar.fgFirstData)
            prSwdmxPsInfo->rVar.fgFirstData = FALSE;
    }

    /****clean audio fifo****/
    if(prSwdmxPsInfo->rFmt.fgAudio && prSwdmxPsInfo->rVar.fgCleanAudio)
    {
        /****flush audio fifo****/
        _SWDMX_PsFlushAudio(prSwdmxInst);
        prSwdmxPsInfo->rVar.fgCleanAudio = FALSE;
    }

    /****modify u8PreviousSendedSize****/
    if(prSwdmxPsInfo->rVar.u8PreviousSendedSize != prSwdmxPsInfo->rVar.u8SendedSize)
    {
        prSwdmxPsInfo->rVar.u8PreviousSendedSize = prSwdmxPsInfo->rVar.u8SendedSize;
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}


