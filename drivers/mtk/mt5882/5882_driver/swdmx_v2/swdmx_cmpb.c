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
 * $RCSfile: swdmx_cmpb.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_cmpb.c
 *  Software demux for CMPB - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "swdmx_cmpb.h"
#include "vdec_drvif.h"
#include "u_midxbuld.h"
#include "swdmx_debug.h"
#include "aud_drvif.h"
#include "x_util.h"
#include "x_timer.h"
#include "dmx_mm_if.h"
#include "gcpu_if.h"

#ifdef LINUX_TURNKEY_SOLUTION
#include "aud_drv.h"
#endif

#include "x_aud_dec.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define WRONG_FLAG_VALUE                (-1)
#define CMPB_DMX_MV_RETRY               (10)
#define H264_STARTCODE_LEN              (3)

#define CMPB_SEQHEADER_MAX_SIZE         (512)
#define CMPB_SEQHEADER_FEEDER_SIZE      (10 * 1024)

#define AAC_FAKE_HDR_SIZE               (7)
#define AAC_ALLOC_SIZE                  (256)

#define FLAG_TEST_PDCF                  (0)
#define FLAG_TO_TRIG_AUD                (1)
#define FLAG_TO_SET_STC                 (2)
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//#define CMPB_TIME_PROFILE

#define cmpb_min(x, y) (((x) < (y)) ? (x) : (y))
#define cmpb_max(x, y) (((x) > (y)) ? (x) : (y))

#define cmpb_abs(x) (((x) >= 0) ? (x) : -(x))

#define CMPB_RPOS_INC(src,len,startaddr,endaddr) (src= (src+(len)>=endaddr) ? (startaddr+((src)+(len)-(endaddr))) : ((src)+(len)))

#define CMPB_HDR_LEN    (14 + prSwdmxCmpbInfo->u4StartCodeLen)

#define IS_SLOW_FWD(s)  ((s) > 1 && (s) < 1000)

#define SWDMX_CMPB_VERIFY_NULL(ptr)                                                         \
          do {                                                                              \
            if (ptr == NULL) {                                                              \
                LOG(2, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                ASSERT(0);                                                                  \
                return; }                                                                   \
          } while (0)

#define SWDMX_CMPB_VERIFY_NULL_ERROR(ptr)                                                   \
          do {                                                                              \
            if (ptr == NULL) {                                                              \
                LOG(2, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                ASSERT(0);                                                                  \
                return FALSE; }                                                             \
          } while (0)

#define SWDMX_CMPB_VERIFY_NULL_RET(ptr, ret)                                                \
          do {                                                                              \
            if (ptr == NULL) {                                                              \
                LOG(2, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                ASSERT(0);                                                                  \
                return ret; }                                                               \
          } while (0)

#define IS_SPEC_AP(a)                   ((a) == MM_SRC_TYPE_NETWORK_SKYPE || (a) == MM_SRC_TYPE_NETWORK_MHP)
#define IS_NO_AVSYNC_AP(a)              ((a) == MM_SRC_TYPE_NETWORK_SKYPE)
#define IS_NEED_BUFFER_TO_STARTUP(a)    (((a) == MM_SRC_TYPE_NETWORK_NETFLIX) || \
                                         ((a) == MM_SRC_TYPE_NETWORK_NETFLIX_3D))

#define IS_NEW_AUDIO_CHG_FLOW(a)        ((a) == MM_SRC_TYPE_NETWORK_DASH)

#define IS_AP_WANT_TO_DO_SEEK(a)        (((a) == MM_SRC_TYPE_YOUTUBE) || \
                                         (a) == MM_SRC_TYPE_NETWORK_MSS) 

#define IS_AP_SETTED_SEEK_PTS(a)        ((a) == MM_SRC_TYPE_YOUTUBE)


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static BOOL _fgTestPdcf     = FALSE;
static BOOL _fgToTrigAud    = FALSE;
static BOOL _fgToSetStc     = FALSE;

#ifdef CMPB_TIME_PROFILE
static HAL_TIME_T _rSwdmxStart;
static HAL_TIME_T _rSwdmxEnd;
static HAL_TIME_T _rSwdmxDif;
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static SWDMX_HDR_PRS_STATE_T _SWDMX_PrsCmpbPacket(
    UCHAR u1SrcId,
    UCHAR* pucBitstream,
    SWDMX_RANGE_LIST_T* prRangeList,
    UINT32 u4AvailSize,
    UINT32* pu4ConsumeSize,
    INT32 i4PlaySpeed,
    BOOL fgRangeEnd,
    BOOL fgOneVideoFrm);

static SWDMX_HDR_PRS_STATE_T _SWDMX_CmpbHdrPrsWithoutIdx(
    UCHAR u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    INT32 i4PlaySpeed,
    UINT64 u8SeekPos,
    BOOL fgSpeedChange);


static SWDMX_HDR_PRS_STATE_T  _SWDMX_BeforeDeliverUseIdx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1HdrType);

static BOOL _SWDMX_CmpbInitDmx(UCHAR u1SrcId, UINT8 u1HdrType, VOID* pfnCbFunc, BOOL fgEos);

static BOOL _SWDMX_CmpbGetDataFromFeeder(UCHAR u1SrcId, FEEDER_REQ_DATA_T *prFeederData);

static BOOL _SWDMX_CmpbWaitFeeder(UCHAR u1SrcId, FEEDER_REQ_DATA_T *prFeederData);

static BOOL _SWDMX_CmpbResetFeeder(UCHAR u1SrcId);

static VOID _SWDMX_CmpbVideoWakeUp(UCHAR u1SrcId);

static BOOL _SWDMX_SendCmpbEos(UCHAR u1SrcId, UINT8 u1HdrType);

static ENUM_SWDMX_ADEC_T _SWDMX_CmpbAudCodecConvert(UINT8 e_aud_codec);

static BOOL _SWDMX_CmpbSetStaticFlagValue(INT32 i4FlagType, BOOL fgValue);

static BOOL _SWDMX_CmpbGetStaticFlagValue(INT32 i4FlagType);


BOOL _SWDMX_CmpbSetDecoderInfo(UCHAR u1SrcId,
    UINT8 u1Type,
    UINT32 u4Para1,
    UINT32 u4Para2,
    UINT32 u4Para3)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    if (u1Type == eSWDMX_SET_VIDEOTYPE)
    {
        prSwdmxCmpbInfo->u4VDecType =  u4Para1;
        return TRUE;
    }
    else if (u1Type == eSWDMX_SET_VIDSEQHEADER)
    {
        if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_VC1 ||
            prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV9)
        {
            UINT32 u4Align = 16;
            
            prSwdmxCmpbInfo->u4SequenceHeaderLength = u4Para2;  
            _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxCmpbInfo->u4SequenceHeader, &u4Para2, &u4Align);    

            if(prSwdmxCmpbInfo->u4SequenceHeader)    
            {          
                x_memcpy((VOID*)VIRTUAL(prSwdmxCmpbInfo->u4SequenceHeader),         
                (VOID*)VIRTUAL(u4Para1),            
                prSwdmxCmpbInfo->u4SequenceHeaderLength );   
                return TRUE;
            }      
            else         
            {           
                LOG(0,"Header Seq Buffer alloc fail\n"); 
                return FALSE;
            } 
        }
        else
        {
#if 0 //#ifndef LINUX_TURNKEY_SOLUTION
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr = u4Para1;
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHdrLength = u4Para2;
#else
            UINT32 u4Align = 16;
            UINT32 u4Size = CMPB_SEQHEADER_MAX_SIZE;
            UINT32 i = 0;
            
            LOG(0,"Set decoder info, eSWDMX_SET_VIDSEQHEADER\n");

            if (u4Para2 > u4Size)
            {
                LOG(0,"=====ERROR: seq header size > %u, not support\n", u4Size);
                return TRUE;
            }
           
            while (i < CMPB_MAX_SEQ_HEADER)
            {
                if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgVidHdrIsUsed[i] == FALSE)
                {
                    if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr[i] == 0)
                    {
                        _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr[i], &u4Size, &u4Align);
                        LOG(1, "==Get seq header %u buffer, size = %u\n", i, u4Size);
                    }
                    
                    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHdrLength[i] = u4Para2;
                    LOG(1, "==The u4VidHdrLength %u is %d==\n", i, (int)prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHdrLength[i]);
                   
                    if(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr[i])
                    {
                        x_memcpy((VOID*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr[i]), 
                                (VOID*)VIRTUAL(u4Para1), 
                                prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHdrLength[i]);

                        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgVidHdrIsUsed[i] = TRUE;
                    }
                    else
                    {
                        LOG(0,"Header Seq Buffer alloc fail\n");
                    }

                    if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderFdrPtr == 0)
                    {
                        LOG(0,"===The first play set codec info :===\n");
                        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderFdrPtr = (UINT32)BSP_AllocAlignedDmaMemory(CMPB_SEQHEADER_FEEDER_SIZE, 16);
                    }

                    break;
                }     

                i++;
           }
#endif
            return TRUE;
        }
    }
    else if (u1Type == eSWDMX_SET_AUDIOTYPE)
    {
        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4ADecType = u4Para1;
        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudSampleRate = u4Para2;
        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudChannelNs = u4Para3;

        LOG(0, "AUDIOTYPE, u4ADecType = %u, u4AudSampleRate = %u, u4AudChannelNs = %u\n", u4Para1, u4Para2, u4Para3);

        return TRUE;
    }
    else if (u1Type == eSWDMX_SET_VIDSEQHEADER)
    {       
        UINT32 u4Align = 16; 

        prSwdmxCmpbInfo->u4SequenceHeaderLength = u4Para2;  
        _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &prSwdmxCmpbInfo->u4SequenceHeader, &u4Para2, &u4Align);    

        if(prSwdmxCmpbInfo->u4SequenceHeader)    
        {          
            x_memcpy((VOID*)VIRTUAL(prSwdmxCmpbInfo->u4SequenceHeader),         
            (VOID*)VIRTUAL(u4Para1),            
            prSwdmxCmpbInfo->u4SequenceHeaderLength );   
            return TRUE;
        }      
        else         
        {           
            LOG(0,"Header Seq Buffer alloc fail\n"); 
            return FALSE;
        }  
    }
    else
    {
        return FALSE;
    }
}

BOOL _SWDMX_CmpbGetInfo(UCHAR u1SrcId,
    UINT8 u1Type,
    UINT32* pu4Para1,
    UINT32* pu4Para2,
    UINT32* pu4Para3)
{
    UINT64 *pu8Tmp;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    //LOG(5, "_SWDMX_CmpbGetInfo u1Type = %d\n", u1Type);

    if(u1Type == eSWDMX_GET_CURRENT_POSITION)
    {
        if(pu4Para1 == NULL)
        {
            LOG(1, "_SWDMX_CmpbGetInfo pu4Para1 = NULL\n");
            return FALSE;
        }
        
        pu8Tmp = (UINT64*)pu4Para1;
        *pu8Tmp = prSwdmxCmpbInfo->u8CurFilePos;

        return TRUE;
    }

    return FALSE;
}

BOOL _SWDMX_CmpbVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    UINT32 u4Addr;
    UINT32 u4StartCode;
    UCHAR *pucBuf;
    SWDMX_INFO_T *prSwdmxInst = (SWDMX_INFO_T*)prPes->pvInstanceTag;
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;

    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));

    rPesInfo.ucEsId = (UCHAR)prSwdmxInst->u1VdecId;

    if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucMpvId = VLD1; // video format all use VLD expect jpeg
    }
    else
    {
        rPesInfo.ucMpvId = VLD0; // video format all use VLD expect jpeg
    }
#if 1  // to fix video jitter problem
    rPesInfo.u4PTS = (prPes->u4Pts != 0 ? prPes->u4Pts : prSwdmxCmpbInfo->u4CurVidPts);
#else
    rPesInfo.u4PTS = prSwdmxCmpbInfo->u4CurVidPts;
#endif
    rPesInfo.u4DTS = prPes->u4Dts;
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
    rPesInfo.fgDtsValid = prPes->fgPtsDts;
    rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
    rPesInfo.fgEos = prPes->fgEOS;
    rPesInfo.u8Offset = prSwdmxCmpbInfo->u8VidOffset;
    #ifdef CC_VDEC_FMT_DETECT
    rPesInfo.u4DmxFrameType = prPes->u4FrameType;
    #endif

    if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_VC1 ||
        prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV9)
    {
        rPesInfo.fgDtsValid = TRUE;//prPes->fgPtsDts;
        rPesInfo.u4PTS = (prPes->u4Pts);

        if (rPesInfo.fgEos)
        {
            rPesInfo.u4VldReadPtr =0;
            rPesInfo.u4FifoStart =0;
            rPesInfo.u4FifoEnd =0;
#if 0
            if (_rSwdmxWmvInfo.fgVideoPause)
            {
                _WMVStartToPlay(prSwdmxInst);
            }
#endif
        }
        
        switch (prSwdmxCmpbInfo->u4VDecType)
        {
        case ENUM_SWDMX_VDEC_VC1:
            rPesInfo.u4VideoType = DMX_VIDEO_VC1;
            break;
        case ENUM_SWDMX_VDEC_WMV9:
            rPesInfo.u4VideoType = DMX_VIDEO_WMV9;
            break;
#if 0
        case ENUM_SWDMX_VDEC_WMV7:
            rPesInfo.u4VideoType = DMX_VIDEO_WMV7;
            break;
        case ENUM_SWDMX_VDEC_WMV8:
            rPesInfo.u4VideoType = DMX_VIDEO_WMV8;
            break;
#endif
        default:
            LOG(0, "Unknown WMV type: %d!!!\n", prSwdmxCmpbInfo->u4VDecType);
            ASSERT(0);
        }
    }

    if(!rPesInfo.fgEos)
    {
        ASSERT((rPesInfo.u4VldReadPtr < rPesInfo.u4FifoEnd) &&
            (rPesInfo.u4VldReadPtr >= rPesInfo.u4FifoStart));
    }   
    else
    {
        if(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.i4PrevSpeed < 0)
        {
            rPesInfo.fgBackward = TRUE;
        }  
         
        rPesInfo.u4FifoStart = 0;
        rPesInfo.u4FifoEnd = 0;
        rPesInfo.u4VldReadPtr = 0;
    }
    
    if(prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_H264)
    {
        u4Addr = (UINT32)prPes->au1PicInfo;
        rPesInfo.ucPicType = *(UINT8*)(u4Addr);

        u4Addr = prPes->u4FrameAddr + 4;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;

        if(( rPesInfo.ucPicType & 0x1f) == 5)
        {
            prSwdmxCmpbInfo->u4LastIPts = rPesInfo.u4PTS;

            if (prSwdmxCmpbInfo->fgSeekWaitFirstIFrame == TRUE)
            {
                prSwdmxCmpbInfo->fgSeekWaitFirstIFrame = FALSE;
                LOG(0, "SEEK First IFrame Pts = 0x%08x\n", rPesInfo.u4PTS);
            }
        }

        if (prSwdmxCmpbInfo->u4LastIPts == 0xFFFFFFFF)
        {
            VDEC_SetRenderFromFirstPic(prSwdmxInst->u1VdecId, TRUE, prSwdmxInst->u8FileSize);
            prSwdmxCmpbInfo->u4LastIPts = 0x0;
            LOG(0, "u4LastIPts not find!!!!!!! ucPicType: 0x%x\n", rPesInfo.ucPicType);
        }
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_H265)
    {   
        rPesInfo.u4VideoType = (UINT32)DMX_VIDEO_H265;
        rPesInfo.ucPicType = (UCHAR)prPes->u4FrameType;

        LOG(6, "rPesInfo.ucPicType: 0x%02x\n", rPesInfo.ucPicType);

        u4Addr = prPes->u4FrameAddr + 3;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;
        
        switch (prPes->u4FrameType)
        {
            case H265_SLICE_BLA_W_LP:           
            case H265_SLICE_BLA_W_RADL:             
            case H265_SLICE_BLA_N_LP:               
            case H265_SLICE_IDR_W_RADL:             
            case H265_SLICE_IDR_N_LP:               
            case H265_SLICE_TRAIL_CRA:              
            LOG(7, "Swdmx Got Iframe pts = %x \n",prPes->u4Pts);
            
            break;

            /* case H265_PIC_TYPE_P:
            u4SwdmxPicType = SWDMX_PIC_TYPE_P;
            break;*/

            /*case PIC_TYPE_B:
            break;*/

            default:
            break;
        }
        
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_H263)
    {
        if (!rPesInfo.fgEos)
        {
            UCHAR *pucBuf;
            
            u4Addr = (UINT32)(prPes->au1PicInfo + 1);
            pucBuf = (UCHAR*)(u4Addr);

            rPesInfo.fgGop = prPes->fgGop;
            rPesInfo.rExtra.fgValid = TRUE;

            switch (pucBuf[0] & 0x02) 
            {
                case 0:
                    rPesInfo.ucPicType = I_TYPE;
                    prSwdmxCmpbInfo->u4LastIPts = rPesInfo.u4PTS;
                    break;
                case 2:
                    rPesInfo.ucPicType = P_TYPE;
                    break;
                default:
                    rPesInfo.ucPicType = UNKNOWN_TYPE;
                    break;
            }
        }
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_MPEG4)
    {
        UCHAR *pucBuf;

        u4Addr = (UINT32)prPes->au1PicInfo;
        pucBuf = (UCHAR*)(u4Addr);
        
        if (pucBuf[0] == 0xB6)
        {
            u4Addr = (UINT32)(prPes->au1PicInfo + 1);
            pucBuf = (UCHAR*)VIRTUAL(u4Addr);
            
            switch (pucBuf[0] >> 6)
            {
            case 0:
                rPesInfo.ucPicType  = I_TYPE;
                rPesInfo.fgDtsValid = TRUE;
                break;
            case 1: case 3:
                rPesInfo.ucPicType  = P_TYPE;
                rPesInfo.fgDtsValid = FALSE;
                break;
            case 2:
                rPesInfo.ucPicType = B_TYPE;
                rPesInfo.fgDtsValid = FALSE;
                break;
            default:
              rPesInfo.ucPicType = UNKNOWN_TYPE;
              rPesInfo.fgDtsValid = FALSE;
              break;
            }
        }
        else if ((pucBuf[0] & 0x20) == 0x20)
        {
            rPesInfo.fgSeqHdr = TRUE;
        }
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_MPEG1_MPEG2)
    {
        // should fill pic type for VDEC !!!
        switch (prPes->u4FrameType)
        {
        case PIC_TYPE_I:
            rPesInfo.ucPicType = MPV_I_TYPE;
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
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucPicType  = I_TYPE;
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_RAW_NV12)
    {
        rPesInfo.ucPicType  = I_TYPE;
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_VP6)
    {
        // TODO: Add vp6 related parsing
    }
    else if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_VC1)   
    {        
        u4Addr = (UINT32)prPes->au1PicInfo;

        if (u4Addr >= prPes->u4BufEnd)        
        {            
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);   
        }       

        pucBuf = (UCHAR*)(u4Addr);   
        u4StartCode = pucBuf[0];     
        u4StartCode |= 0x100;     

        if ((u4StartCode == 0x10E || u4StartCode ==0x10F))  
        {            
            VDEC_SendEs((void*)&rPesInfo);           
            //if (rPesInfo.u4PTS!=0 || u4StartCode ==0x10F)          
            //    _u4PrevFramePTS = (rPesInfo.u4PTS);           
            return TRUE;       
        }       
        else if ((u4StartCode == 0x10C || u4StartCode ==0x10D))     
        {          
            // do nothing here  
        }        
        else        
        {         
            // do nothing here    
        }   
    }
    else
    {
        LOG(0, "Unknown video type: %d\n", prSwdmxCmpbInfo->u4VDecType);
        ASSERT(0);
    }

    if((prPes->u4FrameAddr == 0) && (!prPes->fgEOS))
    {
        LOG(3, "MMCallback Addr 0!\n");
    }
    else
    {
        // push out the frame for MHP single frame decoding
        if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) &&
              prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_MHP)
        {
            rPesInfo.fgNonBuffer = TRUE;
        }

        x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);

        /*Bandwidth change*/
        if ((prSwdmxCmpbInfo->fgBandWidthChg == TRUE) && ((rPesInfo.ucPicType & 0x1f) <= 0x5))
        {
            rPesInfo.fgBandwidthChanged = TRUE;
            prSwdmxCmpbInfo->fgBandWidthChg = FALSE;
            LOG(0, "VDEC_SendEs fgBandwidthChanged = TRUE\n");
        }
        /*Frame rate change*/
        if ( (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate != FEEDER_CTRL_VSH_SRC_FRAME_RATE_UNKNOWN) &&
             (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.ePrevFrameRate != FEEDER_CTRL_VSH_SRC_FRAME_RATE_UNKNOWN) &&
             (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.ePrevFrameRate != prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate))
        {
            rPesInfo.fgFrmChanged = TRUE;
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.ePrevFrameRate = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate;
        }
        VDEC_SendEs((void*)&rPesInfo);

        // push out the frame for Skype
        if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) &&
              prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_SKYPE)
        {
            // push 264 data
            VDEC_PES_INFO_T rPesInfo1;
            x_memcpy((VOID*)&rPesInfo1, (VOID*)&rPesInfo, sizeof(VDEC_PES_INFO_T));
            rPesInfo1.ucPicType = I_TYPE;
            rPesInfo1.fgSeqEnd = TRUE;
            VDEC_SendEs((void*)&rPesInfo1);
        }
    }
    
    return TRUE;
}

BOOL _SWDMX_CmpbAudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet;
    DMX_AUDIO_PES_T rPes;
    SWDMX_INFO_T *prSwdmxInst = NULL;

    if(prPes != NULL)
    {
        prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
        SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    }
    else
    {
        LOG(1, "_SWDMX_Mp4AudioCallback prPes NULL\n");
        return FALSE;
    }

    x_memset(&rPes, 0, sizeof(rPes));
    x_memcpy(&rPes, prPes, sizeof(DMX_AUDIO_PES_T));
    
    if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.i4PrevSpeed > 0)    
    {        
        rPes.fgForward = TRUE;    
    }   
    else    
    {        
        rPes.fgForward = FALSE;    
    }    
    
    fgRet = AUD_SendAudioPes(&rPes);

    return fgRet;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL _SWDMX_CmpbInit(UCHAR u1SrcId, ENUM_SWDMX_FMT_T eType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    FEEDER_BUF_INFO_T *prCmpbFeederInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    prCmpbFeederInfo= &prSwdmxCmpbInfo->rCmpbFeederInfo;

    UNUSED(eType);

    x_memset((void*)prSwdmxCmpbInfo, 0, sizeof(SWDMX_CMPB_INFO_T));
    x_memset((void*)prCmpbFeederInfo, 0, sizeof(FEEDER_BUF_INFO_T));
    
    if(prSwdmxCmpbInfo->u4ADecType == ENUM_SWDMX_ADEC_AAC &&
        prSwdmxInst->pucAudBuf == 0)
    {
        UINT32 u4RequestSize = AAC_ALLOC_SIZE;
        UINT32 u4Align = 16;
        
        _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, (UINT32*)prSwdmxInst->pucAudBuf, &u4RequestSize, &u4Align);            

        if(prSwdmxInst->pucAudBuf == 0)
        {
            LOG(1, "Allocate AAC buffer failed!\n");
            return FALSE;
        }
    }

    prSwdmxCmpbInfo->u4AudSampleRate = 48000;

    prSwdmxCmpbInfo->fgGotEosIbc = FALSE;
    prSwdmxCmpbInfo->fgBeginToPlay = TRUE;
    prSwdmxCmpbInfo->fgAudTrigVidDec = FALSE;

    prSwdmxCmpbInfo->u4CurVidPts = 0;
    prSwdmxCmpbInfo->fgIsBlock = FALSE;

    prSwdmxCmpbInfo->u8CurFilePos = 0;
    prSwdmxCmpbInfo->u8CurDmxFilePos = 0;

    prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
    prSwdmxCmpbInfo->fgFeederTimeout = FALSE;
    prSwdmxCmpbInfo->i4ReqReadTimes = 0;
    prSwdmxCmpbInfo->u4FeederReqID = 0;

    prSwdmxCmpbInfo->fgWaitFirstAudio = TRUE;
    prSwdmxCmpbInfo->fgWaitFirstVideo = TRUE;
    prSwdmxCmpbInfo->fgAdjustPTS      = FALSE;
    prSwdmxCmpbInfo->fgApFlushFeeder  = FALSE;
    prSwdmxCmpbInfo->fgSeekFlush      = FALSE;
    prSwdmxCmpbInfo->ui4SeekPos       = 0;
    prSwdmxCmpbInfo->fgAudHasTriggeredVidDec = FALSE;
    
    prSwdmxCmpbInfo->fgWaitNextAudio  = FALSE;
    prSwdmxCmpbInfo->u4LastAudioNum   = 0;
    prSwdmxCmpbInfo->fgAudioSwitchCanceled = FALSE;

    prSwdmxCmpbInfo->u4TriggerAudPts = 0xFFFFFFFF;
    prSwdmxCmpbInfo->u4PrevAStrmID   = 0xFFFFFFFF;

    prSwdmxCmpbInfo->u4StartCodeLen = 4;
    prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
    
    prSwdmxCmpbInfo->u4VidCount = 0;
    prSwdmxCmpbInfo->u4CurVidHeaderIndex = 0;
    prSwdmxCmpbInfo->u4CurAudHeaderIndex = 0;

    prSwdmxCmpbInfo->fgBufferToStartup = TRUE;
    prSwdmxCmpbInfo->u4FirstVideoPts = 0xFFFFFFFF;

    prSwdmxCmpbInfo->fgAudCodecChg = FALSE;
    prSwdmxCmpbInfo->fgWaitAudCodecChgDone = FALSE;
    prSwdmxCmpbInfo->fgSeekWaitFirstIFrame = FALSE;

    prSwdmxCmpbInfo->ePrevFrameRate = FEEDER_CTRL_VSH_SRC_FRAME_RATE_UNKNOWN;
    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate = FEEDER_CTRL_VSH_SRC_FRAME_RATE_UNKNOWN;
    prSwdmxCmpbInfo->fgBandWidthChg = FALSE;
    
    return TRUE;
}

/**
*/
VOID _SWDMX_CmpbSetRange(UCHAR u1SrcId, SWDMX_RANGE_LIST_T *prRangeList, UINT32 u4VidStrmID,
                         UINT32 u4AudStrmID, UINT32 u4SubTitleID)
{
    UINT8 u1ATrack, u1SubTrack;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    if (prRangeList == NULL)
    {
        ASSERT(0);
        LOG(1, "Demuxer Range is NULL\n");
        return;
    }
    // megaa test 20100212
    if (prRangeList->prDmxRangeInfo != NULL)
    {
        prRangeList->prDmxRangeInfo->ui8_fileoffset = 0;
        prRangeList->prDmxRangeInfo->z_range_sz = 0xFFFFFFFF;
        prSwdmxCmpbInfo->prDmxRangeInfo = prRangeList->prDmxRangeInfo;
        LOG(1, "=======_SWDMX_CmpbSetRange, e_mm_src_type = %u\n", prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type);
    }

    if (prRangeList->prDmxRangeInfo != NULL)
    {
        MM_RANGE_ELMT_IDXTBL_T *pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)prRangeList->prDmxRangeInfo->pt_idxtbl_lst;

        u1ATrack = 0;
        u1SubTrack = 0;

        while (pBuldIdxTbl != NULL)
        {
            if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_AUDIO &&
                u1ATrack < CMPB_MAX_AUDIO_TRACK)
            {
                prSwdmxCmpbInfo->prAudIdxTbl[u1ATrack] = pBuldIdxTbl;
                LOG(5, "_SWDMX_CmpbSetRange Audio Index table = 0x%08x\n",
                    (UINT32)(prSwdmxCmpbInfo->prAudIdxTbl[u1ATrack]));
                u1ATrack++;
            }
            else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_VIDEO)
            {
                prSwdmxCmpbInfo->prVidIdxTbl = pBuldIdxTbl;
                LOG(5, "_SWDMX_CmpbSetRange Video Index table = 0x%08x\n",
                    (UINT32)(prSwdmxCmpbInfo->prVidIdxTbl));
            }
            else if ((pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_SP)&&
                      (u1SubTrack < CMPB_MAX_SUB_TRACK))
            {
                prSwdmxCmpbInfo->prSubIdxTbl[u1SubTrack] = pBuldIdxTbl;
                LOG(5, "_SWDMX_CmpbSetRange subtile Index table = 0x%08x\n",
                    (UINT32)(prSwdmxCmpbInfo->prSubIdxTbl[u1SubTrack]));
                u1SubTrack++;
            }
            else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_KEY)
            {
                prSwdmxCmpbInfo->prVidKeyIdxTbl = pBuldIdxTbl;
                LOG(5, "_SWDMX_CmpbSetRange Key Index table = 0x%08x\n",
                    (UINT32)(prSwdmxCmpbInfo->prVidKeyIdxTbl));
            }

            pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)pBuldIdxTbl->pv_next_tbl;

           //prSwdmxCmpbInfo->u8_data_offset = prRangeList->prDmxRangeInfo->ui8_fileoffset;
           //prSwdmxCmpbInfo->u8_range_sz  = prRangeList->prDmxRangeInfo->z_range_sz;
        }

        if (prSwdmxCmpbInfo->prVidIdxTbl)
        {
            prSwdmxCmpbInfo->u4VidFps = 30;
            LOG(5, "prSwdmxCmpbInfo->u4VidFps = %ld\n", prSwdmxCmpbInfo->u4VidFps);
        }

        // Set the number of total audio tracks
        if (prSwdmxCmpbInfo->u1TotalATrack == 0)  // tmp solution for MW not setting this
            prSwdmxCmpbInfo->u1TotalATrack = u1ATrack;

        if (prSwdmxCmpbInfo->u1TotalSubTrack == 0)  // tmp solution for MW not setting this
            prSwdmxCmpbInfo->u1TotalSubTrack = u1SubTrack;

        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
    else
    {
        LOG(1, "Demuxer Range is NULL\n");
    }
}


/**
*/
SWDMX_HDR_PRS_STATE_T _SWDMX_CmpbHdrPrs(
    UCHAR u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    SWDMX_CUR_CMD_T* prCmd)
{
    UINT32 u4PrsState;
    INT32 i4PlaySpeed;
    UINT64 u8SeekPos;
    BOOL fgSpeedChange;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_RET(prSwdmxInst, eSWDMX_HDR_PRS_FAIL);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    if (!prCmd)
    {
        LOG(3, "prCmd is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__);
        ASSERT(0);
        return eSWDMX_HDR_PRS_FAIL;
    }
    
    i4PlaySpeed = prCmd->i4PlaySpeed;
    u8SeekPos = prCmd->u8SeekPos;
    fgSpeedChange = prCmd->fgChange;

    if (prSwdmxCmpbInfo->fgAudHasTriggeredVidDec == TRUE)
    {
        LOG(3,"_SWDMX_CmpbHdrPrs Audio has triggerd VidDec\n");
        prSwdmxCmpbInfo->fgAudTrigVidDec = FALSE;
        prSwdmxCmpbInfo->fgAudHasTriggeredVidDec = FALSE;
    }
    
    if (prSwdmxCmpbInfo->fgApFlushFeeder == TRUE)
    {       
        prSwdmxCmpbInfo->i4ReqReadTimes = 0;
        prSwdmxCmpbInfo->u4LeftBytesInFeeder = 0;
        prSwdmxCmpbInfo->u4FeederReqID += 1;        

        if (IS_AP_SETTED_SEEK_PTS(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))
        {               
            AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxCmpbInfo->ui4SeekPos);
            STC_StopStc(prSwdmxInst->u1StcId);
            STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxCmpbInfo->ui4SeekPos);
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxCmpbInfo->ui4SeekPos); 
            prSwdmxCmpbInfo->u4TriggerAudPts = prSwdmxCmpbInfo->ui4SeekPos;
            prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
            LOG(1, "%s %d, Mainloop Noticed Seek, TriggerPts:%d, SetRenderPts:%d, Stc:%d\n", 
                __FUNCTION__,__LINE__,prSwdmxCmpbInfo->ui4SeekPos, prSwdmxCmpbInfo->ui4SeekPos,STC_GetStcValue(prSwdmxInst->u1StcId));
            prSwdmxCmpbInfo->ui4SeekPos = 0;         
        }
        else 
        {
            if (FALSE == _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_TRIG_AUD , TRUE))
            {
                LOG(0, "%s %d Set Flag(%d) Value(TRUE) Fail!\n",__FUNCTION__,__LINE__,FLAG_TO_TRIG_AUD);
            }
            STC_StopStc(prSwdmxInst->u1StcId);
            _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_SET_STC,TRUE);
            LOG(1, "%s %d, Mainloop Noticed Seek, Stc:%d\n", 
                __FUNCTION__,__LINE__,STC_GetStcValue(prSwdmxInst->u1StcId));
        }
        prSwdmxCmpbInfo->fgApFlushFeeder = FALSE;
        prSwdmxCmpbInfo->fgSeekFlush = FALSE;
    }
    
    if (prSwdmxCmpbInfo->fgBeginToPlay &&
        i4PlaySpeed == 1000)
    {
        if (prSwdmxCmpbInfo->fgEnAudio && prSwdmxCmpbInfo->fgEnVideo)
        {
#if 1       /*wait vdec turn to play*/
            {
                #define _LOOP_CNT_    200
                UINT32 u4Cnt = 0;
                while (u4Cnt < _LOOP_CNT_)
                {
                    UINT32 u4Status = MPV_GetDecMode(prSwdmxInst->u1VdecId);
                    if ((u4Status == VDEC_ST_STOP) || (u4Status == VDEC_ST_IDLE))
                    {
                        x_thread_delay(10);
                        u4Cnt++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
#endif  
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            VDEC_Pause(prSwdmxInst->u1VdecId);

            prSwdmxCmpbInfo->fgVDecIsPause = TRUE;
            prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;

            if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))
            {
                prSwdmxCmpbInfo->u4TriggerAudPts = 0;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxCmpbInfo->u4TriggerAudPts);
            }

            if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxCmpbInfo->u4CurVidPts);
            }
        }
        // audio only
        else if (prSwdmxCmpbInfo->fgEnAudio)
        {
            if (prSwdmxCmpbInfo->u1CurATrack < CMPB_MAX_AUDIO_TRACK)
            {
                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxCmpbInfo->u4AudPts[prSwdmxCmpbInfo->u1CurATrack]);
            }
            else
            {
                LOG(1, "Out of Audio Track Number\n");
                AUD_SetStartPts(prSwdmxInst->u1AdecId, 0);
            }
        }
        // video only
        else if (prSwdmxCmpbInfo->fgEnVideo)
        {
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            VDEC_Pause(prSwdmxInst->u1VdecId);

            prSwdmxCmpbInfo->fgVDecIsPause = TRUE;
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxCmpbInfo->u4CurVidPts);
        }

        // set stc for starting playback
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxCmpbInfo->u4CurVidPts - 45000);

        prSwdmxCmpbInfo->fgBeginToPlay = FALSE;
    }

    // Reset some values for state change.
    if (prSwdmxCmpbInfo->fgChgATrack || fgSpeedChange)
    {
        if (prSwdmxCmpbInfo->fgDeliverVEos)
        {
            UINT32 u4Flag = 0;
            DMX_MM_T rDmxMMInfo;

            x_memset(&rDmxMMInfo, 0, sizeof(DMX_MM_T));
            rDmxMMInfo.fgEnable = TRUE;
            rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;
            u4Flag |= DMX_MM_FLAG_SEARCH_START_CODE;

            switch (prSwdmxCmpbInfo->u4VDecType)
            {
                case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
                case ENUM_SWDMX_VDEC_MPEG4:
                case ENUM_SWDMX_VDEC_H264:
                case ENUM_SWDMX_VDEC_MVC:
                case ENUM_SWDMX_VDEC_VC1:
                case ENUM_SWDMX_VDEC_AVS:
                case ENUM_SWDMX_VDEC_H265:
                    LOG(7, "%s SearchStartCode = TRUE\n", __FUNCTION__);
                    rDmxMMInfo.fgSearchStartCode = TRUE;
                    break;

                default:
                     LOG(7, "%s SearchStartCode = FALSE\n", __FUNCTION__);
                    rDmxMMInfo.fgSearchStartCode = FALSE;
                    break;
            }
            
            if (!DMX_MM_Set(prSwdmxCmpbInfo->u1VidPid, u4Flag, &rDmxMMInfo))
            {
                LOG(3, "Fail to reset demuxer\n");
            }

            prSwdmxCmpbInfo->fgDeliverVEos = FALSE;
        }
        
        if (prSwdmxCmpbInfo->fgDeliverAEos)
        {
            prSwdmxCmpbInfo->fgDeliverAEos = FALSE;
            AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
        }

#if defined(CC_DLNA_SUPPORT) && 0
        // DLNA
        FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_EOS, FALSE);
#endif
        prSwdmxCmpbInfo->fgAudIsReady = FALSE;

        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
    
    if((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && (!IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type)))
    {
        if (prSwdmxCmpbInfo->fgChgATrack && prSwdmxCmpbInfo->fgEnAudio)
        {
            prSwdmxCmpbInfo->fgChgATrack = FALSE;
        }

        if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL &&
            prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_DASH &&
            prSwdmxInst->fgSendUnderflow2MW == TRUE && prSwdmxCmpbInfo->fgWaitNextAudio == TRUE)
        {
            prSwdmxCmpbInfo->fgAudioSwitchCanceled = TRUE;
            prSwdmxCmpbInfo->u4LastAudioNum = 0;
            prSwdmxCmpbInfo->fgWaitNextAudio = FALSE;
        }
    }
    
    if (fgSpeedChange)
    {
        /*speed change*/
        if (((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && (prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_MSS)) &&
            ((i4PlaySpeed>0 && prSwdmxCmpbInfo->i4PrevSpeed<0) || (i4PlaySpeed<0 && prSwdmxCmpbInfo->i4PrevSpeed>0)))
        {
            _SWDMX_CmpbHandleFlushFeeder(u1SrcId, 0, NULL, 0, 0);
            prSwdmxCmpbInfo->i4ReqReadTimes = 0;
            prSwdmxCmpbInfo->u4LeftBytesInFeeder = 0;
            prSwdmxCmpbInfo->u4FeederReqID += 1; 
            prSwdmxCmpbInfo->fgApFlushFeeder = FALSE;
            prSwdmxCmpbInfo->fgSeekFlush = FALSE;
            _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_TRIG_AUD, TRUE);
        }

        LOG(0, "speedChanged %d, %d\n", i4PlaySpeed, prSwdmxCmpbInfo->i4PrevSpeed);
        
        if(((UINT32)(u8SeekPos>>32) != 0xFFFFFFFF) || ((UINT32)(u8SeekPos) != 0xFFFFFFFF))
        {
            LOG(0, "----------SEEK PTS: 0x%08x-----------\n", (UINT32)u8SeekPos);

            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxCmpbInfo->fgVDecIsPause = TRUE;
            prSwdmxCmpbInfo->fgBufferToStartup = TRUE;
            prSwdmxCmpbInfo->fgSeekWaitFirstIFrame = TRUE;
            prSwdmxCmpbInfo->u4LastIPts = 0xFFFFFFFF;

            /*reset eos flag*/
            prSwdmxCmpbInfo->fgGotEosIbc = FALSE;
            prSwdmxCmpbInfo->fgDeliverAEos = FALSE;
            prSwdmxCmpbInfo->fgDeliverVEos = FALSE;
            
            if (prSwdmxCmpbInfo->fgWaitNextAudio == TRUE)
            {
                prSwdmxCmpbInfo->u4LastAudioNum = 0;
                prSwdmxCmpbInfo->fgWaitNextAudio = FALSE;
            }
           
            _SWDMX_CmpbHandleFlushFeeder(u1SrcId, 0, NULL, 0, 0);
            prSwdmxCmpbInfo->i4ReqReadTimes = 0;
            prSwdmxCmpbInfo->u4LeftBytesInFeeder = 0;
            prSwdmxCmpbInfo->u4FeederReqID += 1; 
            prSwdmxCmpbInfo->fgApFlushFeeder = FALSE;
            prSwdmxCmpbInfo->fgSeekFlush = FALSE;
            _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_TRIG_AUD, TRUE);
            _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_SET_STC,FALSE); 
            
            STC_StopStc(prSwdmxInst->u1StcId);
            STC_SetStcValue(prSwdmxInst->u1StcId, (UINT32)u8SeekPos);
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, (UINT32)u8SeekPos);
            
#ifdef CC_AUD_SUPPORT_DUAL_DSP 
            AUD_DSPCmdPlay(AUD_DSP0, 0);
#else
            AUD_DSPCmdPlay(0);
#endif

       }
    }

    u4PrsState = _SWDMX_CmpbHdrPrsWithoutIdx(u1SrcId,
                                             prRangeList,
                                             i4PlaySpeed,
                                             u8SeekPos,
                                             fgSpeedChange);

    prSwdmxCmpbInfo->i4PrevSpeed = i4PlaySpeed;

    return (SWDMX_HDR_PRS_STATE_T)u4PrsState;
}

static BOOL _CheckCmpbPacketType(UCHAR u1SrcId, UINT8 *pu1Buf, UINT64 *pu8Pts, UINT8 *pu1Flag, UINT32 *pu4PacketSize, UINT8 *pu1Flag2)
{
    UINT64 u8Pts = 0;
    UINT32 u4PacketSize = 0;
    int i;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    if ((4 == prSwdmxCmpbInfo->u4StartCodeLen) &&
        ((pu1Buf[0] != 0x3E) || (pu1Buf[1] != 0xD1) || (pu1Buf[2] != 0xA7) || (pu1Buf[3] != 0xE4)))
    {
        LOG(0, "CMPB old mode\n");
        prSwdmxCmpbInfo->u4StartCodeLen = 0;
        ASSERT(0);
    }

    *pu1Flag = *(pu1Buf + 12 + prSwdmxCmpbInfo->u4StartCodeLen);

    if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
        prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
    {
        *pu1Flag2 = *(pu1Buf + 13 + prSwdmxCmpbInfo->u4StartCodeLen);
    }

    for (i = 0; i < 8; i++)
    {
        u8Pts = (u8Pts << 8) + (UINT64)(*(pu1Buf + 7 + prSwdmxCmpbInfo->u4StartCodeLen - i));
    }

    if (prSwdmxCmpbInfo->fgAdjustPTS && (u8Pts == 0))
    {
        u8Pts = 1;
    }

    if (u8Pts & ((UINT64)0x1) << 63)
    {
        prSwdmxCmpbInfo->fgAdjustPTS = TRUE;
        u8Pts = 0;
    }

    *pu8Pts = u8Pts;

    for (i = 0; i < 4; i++)
    {
        u4PacketSize = (u4PacketSize << 8) + (UINT32)(*(pu1Buf + 11 + prSwdmxCmpbInfo->u4StartCodeLen - i));
    }
    *pu4PacketSize = u4PacketSize;

    switch ((*pu1Flag) & 0x07)
    {
        case 1:  // video
            switch ((*pu1Flag) & 0x18)
            {
                case 0x08:  // I-frame
                case 0x10:  // B-frame
                case 0x18:  // P-frame
                case 0:     // tmp solution
                    return TRUE;

                default:
                    return FALSE;
            }

        case 2:  // audio
            return TRUE;

        case 3:  // subpicture
            return TRUE;
    }

    return FALSE;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_PrsCmpbPacket(
    UCHAR u1SrcId,
    UCHAR* pucBitstream,
    SWDMX_RANGE_LIST_T* prRangeList,
    UINT32 u4AvailSize,
    UINT32* pu4ConsumeSize,
    INT32 i4PlaySpeed,
    BOOL fgRangeEnd,
    BOOL fgOneVideoFrm)
{
    UINT32 i = 0;
    BOOL fgFindPkt = FALSE;
    UINT32 u4OrgAvailSize = u4AvailSize;
    UINT64 u8Pts;
    UINT8 u1Flag;
    UINT8 u1Flag2 = 0;
    UINT32 u4PacketSize;
    UINT8 u1HdrType;
    UINT32 u4DmxAvailSize = 0, u4MoveRetryCnt;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    DMX_MM_DATA_T *prCmpbDmxMMData;
    FEEDER_BUF_INFO_T *prCmpbFeederInfo;
    SWDMX_CMPB_VERIFY_NULL_RET(prSwdmxInst, eSWDMX_HDR_PRS_FAIL);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    prCmpbDmxMMData = &prSwdmxCmpbInfo->rCmpbDmxMMData;
    prCmpbFeederInfo= &prSwdmxCmpbInfo->rCmpbFeederInfo;

#if 0  // debug log code
    {
        int n;
        UINT8 *pu1Ptr = (UINT8 *)pucBitstream;
        LOG(7, "        got    : ");
        for (n = 0; n < 32; n++)
        {
            LOG(7, "%02X", (UINT32)(*pu1Ptr++));
        }
        LOG(7, " len = %d\n", u4AvailSize);
    }
#endif

    while (i < u4AvailSize)
    {
        if (_CheckCmpbPacketType(u1SrcId, pucBitstream + i, &u8Pts, &u1Flag, &u4PacketSize, &u1Flag2))
        {
            fgFindPkt = TRUE;
            break;
        }
        
        i++;
    }

    // Didn't find the packet
    if (!fgFindPkt)
    {
        *pu4ConsumeSize = u4OrgAvailSize;
        LOG(1, "unknown packet!!!!!!!!!!!!!!!!!!!Reset encrypt info.\n");
        /*Reset encrypt info*/        
        prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
        x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));
        return eSWDMX_HDR_PRS_FAIL;
    }

    switch (u1Flag & 0x7)
    {
        case 1: /*Video*/
        {
            u1HdrType = eSWDMX_MEDIA_VIDEO;
            /*tmp soluiton to trigger Vdec to update stc*/
            if (u8Pts == 0)
            {
                u8Pts = 1;
            }
            LOG(6, "video packet: %u, %u\n", (UINT32)u8Pts, STC_GetStcValue(prSwdmxInst->u1StcId));
            prSwdmxCmpbInfo->u4CurVidPts = (UINT32)u8Pts;

            if(TRUE == _SWDMX_CmpbGetStaticFlagValue(FLAG_TO_SET_STC))
            {
                STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxCmpbInfo->u4CurVidPts);
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxCmpbInfo->u4CurVidPts);
                _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_SET_STC, FALSE);
            }
            
            /*We Will Stop Stc When Ap Seeks, Should Start Stc After Recieved First Video Packet While did not send underflow!*/
            if (prSwdmxInst->fgSendUnderflow2MW == FALSE)
            {
                if(!STC_IsStartedStc(prSwdmxInst->u1StcId))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                    LOG(1, "Force to call STC_StartStc!!!!\n");
                }
            }
            
            if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
                prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
            {
                UINT16 u2MaxQNum, u2CurQNum;
                UINT8 u1PidIdx = prSwdmxCmpbInfo->u1VidPid;

                u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_VIDEO, u1PidIdx);
                VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);

                if (u2CurQNum > u2MaxQNum - 10)
                {
                    LOG(7, "Seq Header, u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);

                    // megaa 20110822: patch for video only case
                    if (!prSwdmxCmpbInfo->fgEnAudio && prSwdmxCmpbInfo->fgVDecIsPause)
                    {
                        LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up seq1\n");
                        _SWDMX_CmpbVideoWakeUp(u1SrcId);
                        prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
                    }

                    *pu4ConsumeSize = 0;
                    prSwdmxInst->fgFifoFull = TRUE;
                    x_thread_delay(10);
                    return eSWDMX_HDR_PRS_SUCCEED;
                }        

                if(u4DmxAvailSize < CMPB_SEQHEADER_MAX_SIZE)
                {
                    LOG(7, "Seq Header u4DmxAvailSize: 0x%08x\n", u4DmxAvailSize);

                    if (prSwdmxCmpbInfo->fgVDecIsPause)
                    {
                        LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up seq2\n");
                        _SWDMX_CmpbVideoWakeUp(u1SrcId);
                        prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
                    }

                    *pu4ConsumeSize = 0;
                    prSwdmxInst->fgFifoFull = TRUE;
                    x_thread_delay(10);
                    return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
                }

                // change seq header
                u1Flag2 = (u1Flag2 & 0xFF) > 0 ? ((u1Flag2 & 0xFF) - 1):0;

                if (prSwdmxCmpbInfo->u4CurVidHeaderIndex != u1Flag2 && u1Flag2 < CMPB_MAX_SEQ_HEADER)
                {
                    prSwdmxCmpbInfo->u4CurVidHeaderIndex = u1Flag2;
                    _SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);    
                }
            }

            break;
        }
        case 2: /*Audio*/
        {
            u1HdrType = eSWDMX_MEDIA_AUDIO;
            LOG(7, "audio packet: %u, %u\n", (UINT32)u8Pts, STC_GetStcValue(prSwdmxInst->u1StcId));  

            if (TRUE == _SWDMX_CmpbGetStaticFlagValue(FLAG_TO_TRIG_AUD))
            {
                LOG(1,"%s %d set trigger aud pts: %u\n",__FUNCTION__,__LINE__,(UINT32)u8Pts);
                prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
                prSwdmxCmpbInfo->u4TriggerAudPts = (UINT32)u8Pts;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxCmpbInfo->u4TriggerAudPts); 
                _SWDMX_CmpbSetStaticFlagValue(FLAG_TO_TRIG_AUD, FALSE);
            }
            
            if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
                prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
            {
                if (prSwdmxCmpbInfo->fgWaitAudCodecChgDone)
                {
                    *pu4ConsumeSize = 0;
                    x_thread_delay(10);
                    return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                }

                if (prSwdmxCmpbInfo->fgAudCodecChg)
                {
                    UINT32 u4CurPts = 0;
                    
                    u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

                    LOG(0, "Audio codec change done, triger audio.\n");
                    prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
                    prSwdmxCmpbInfo->u4TriggerAudPts = u4CurPts;
                    prSwdmxCmpbInfo->fgAudCodecChg = FALSE;
                }

                if (prSwdmxCmpbInfo->fgSeekWaitFirstIFrame)
                {
                     u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                     LOG(0, ">>>>>>>>>>>>>>>>>seek skip audio packet, Pts = 0x%08x\n", (UINT32)u8Pts);
                     break;
                }
#if 0                
                if (prSwdmxCmpbInfo->fgSeekFlush)
                {
                    LOG(0, "Dash triger audio after seek.\n");
                    prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
                    prSwdmxCmpbInfo->u4TriggerAudPts = (UINT32)u8Pts;
                    prSwdmxCmpbInfo->fgSeekFlush = FALSE;

                    AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxCmpbInfo->u4TriggerAudPts);
                }
#endif               
                // change seq header
                if (0 != (u1Flag2 & 0xF) && (u1Flag2 & 0xF) <= CMPB_MAX_SEQ_HEADER)
                {
                    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudSampleRate = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader[(u1Flag2 & 0xF) - 1].ui4_sample_rate;
                    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudChannelNs = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader[(u1Flag2 & 0xF) - 1].i2_channle;
                    
                    LOG(7, "Change AUDIOTYPE, u4ADecType = %u, u4AudSampleRate = %u, u4AudChannelNs = %u\n", 
                            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4ADecType, 
                            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudSampleRate, 
                            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudChannelNs);
                }
                else
                {
                    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudSampleRate = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader[0].ui4_sample_rate;
                    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4AudChannelNs = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader[0].i2_channle;
                }
            }

             /*new audio change flow*/
            if (((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && (IS_NEW_AUDIO_CHG_FLOW(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))))
            {
                if (prSwdmxCmpbInfo->fgWaitNextAudio)
                {   
                     UINT32 u4CurPts = 0;
                    if (prSwdmxCmpbInfo->u4LastAudioNum > 0)
                    {
                        u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                        LOG(0, ">>>>>>>>>>>>>>>>>[new] skip last audio packet, Pts = 0x%x, CurPts = 0x%x\n", (UINT32)u8Pts, u4CurPts);
                    }
                    else
                    {
                       
                        u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

                        if ((UINT32)u8Pts + 1500 < u4CurPts )
                        {
                            u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                            LOG(0, ">>>>>>>>>>>>>>>>>[new] skip new audio packet, Pts = 0x%x, CurPts = 0x%x\n", (UINT32)u8Pts, u4CurPts);
                        }
                        else
                        {
                            prSwdmxCmpbInfo->fgWaitNextAudio = FALSE;
                            prSwdmxCmpbInfo->u4LastAudioNum  = 0;
                            prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
                            prSwdmxCmpbInfo->u4TriggerAudPts = (UINT32)u8Pts;
                            
                            AUD_SetStartPts(prSwdmxInst->u1AdecId, (UINT32)u8Pts);

                            LOG(0, ">>>>>>[new] audio change, start send audio data,( 0x%x, 0x%x)\n", (UINT32)u8Pts, u4CurPts);

                            if(prSwdmxInst->rCurRangeReq.prDmxRangeInfo)
                            {
                                _SWDMX_RANGE_CALLBACK(
                                    prSwdmxInst->rCurRangeReq.prDmxRangeInfo->pv_tag,
                                    MM_AUDIO_SWITCH_DONE,
                                    prSwdmxInst->rCurRangeReq.prDmxRangeInfo->ui4_range_id,
                                    0xFFFFFFFF,
                                    (UINT32)u1SrcId);

                                LOG(0, "[new] Send MM_AUDIO_SWITCH_DONE\n");
                            }
                         }
                    }
                }
            }
            /*old flow*/
            else if((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) &&(!IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type)))
            {
                if (prSwdmxCmpbInfo->fgWaitNextAudio)
                {
                    UINT32 u4CurPts;
                    
                    u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

                    if (prSwdmxCmpbInfo->u4LastAudioNum > 0)
                    {
                        u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                        LOG(0, ">>>>>>>>>>>>>>>>>skip last audio packet, Pts = 0x%x, CurPts = 0x%x\n", (UINT32)u8Pts, u4CurPts);

                        if ((UINT32)u8Pts == 0)
                        {
                            prSwdmxCmpbInfo->u4LastAudioNum--;
                            LOG(0, "--, u4LastAudioNum = %u\n", prSwdmxCmpbInfo->u4LastAudioNum);
                        }
                    }
                    else
                    {
                        if ((UINT32)u8Pts + 1500 < u4CurPts )
                        {
                            u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                            LOG(0, ">>>>>>>>>>>>>>>>>skip new audio packet, Pts = 0x%x, CurPts = 0x%x\n", (UINT32)u8Pts, u4CurPts);
                        }
                        else
                        {
                            prSwdmxCmpbInfo->fgWaitNextAudio = FALSE;
                            prSwdmxCmpbInfo->u4LastAudioNum  = 0;
                            prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
                            prSwdmxCmpbInfo->u4TriggerAudPts = (UINT32)u8Pts;
                            
                            AUD_SetStartPts(prSwdmxInst->u1AdecId, (UINT32)u8Pts);

                            LOG(0, ">>>>>>audio change, start send audio data,( 0x%x, 0x%x)\n", (UINT32)u8Pts, u4CurPts);

                            if(prSwdmxInst->rCurRangeReq.prDmxRangeInfo)
                            {
                                _SWDMX_RANGE_CALLBACK(
                                    prSwdmxInst->rCurRangeReq.prDmxRangeInfo->pv_tag,
                                    MM_AUDIO_SWITCH_DONE,
                                    prSwdmxInst->rCurRangeReq.prDmxRangeInfo->ui4_range_id,
                                    0xFFFFFFFF,
                                    (UINT32)u1SrcId);

                                LOG(0, "Send MM_AUDIO_SWITCH_DONE\n");
                            }
                         }
                    }
                }
                else if (prSwdmxCmpbInfo->fgAudioSwitchCanceled)
                {               
                    UINT32 u4CurPts;
                    
                    u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

                    if ((UINT32)u8Pts + 1500 < u4CurPts )
                    {
                        u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                        LOG(0, ">>>>>>>>>>>>>>>>>audio change canceled, skip new audio packet, Pts = 0x%x, CurPts = 0x%x\n", (UINT32)u8Pts, u4CurPts);
                    }
                    else
                    {
                        prSwdmxCmpbInfo->fgAudioSwitchCanceled = FALSE;
                        prSwdmxCmpbInfo->fgAudTrigVidDec = TRUE;
                        prSwdmxCmpbInfo->u4TriggerAudPts = (UINT32)u8Pts;

                        AUD_SetStartPts(prSwdmxInst->u1AdecId, (UINT32)u8Pts);

                        LOG(0, ">>>>>>audio change canceled, start send audio data,( 0x%x, 0x%x)\n", (UINT32)u8Pts, u4CurPts);
                    }   
                }
            }

            prSwdmxCmpbInfo->u4CurAudPts = (UINT32)u8Pts;

            break;
        }
        case 3: /*Subtitle*/
        {
            u1HdrType = eSWDMX_MEDIA_SUBTITLE;
            LOG(5, "subtitle packet: %d, %d\n", (UINT32)u8Pts, STC_GetStcValue(prSwdmxInst->u1StcId));
            break;
        }
        default:
        {
            u1HdrType = eSWDMX_MEDIA_SKIPPED;
            LOG(5, "skipped packet\n");
            break;
        }
    }

    prCmpbDmxMMData->u4StartAddr = (UINT32)(pucBitstream + i + CMPB_HDR_LEN);
    if (prCmpbDmxMMData->u4StartAddr >= (UINT32)(VIRTUAL(prCmpbFeederInfo->u4EndAddr)))
        ASSERT(0);

    if (prSwdmxCmpbInfo->u8RecDmxMoviPos)
    {
    }

    // Error Handling
    if (u4PacketSize > CMPB_BUFFER_SIZE)
    {
        LOG(1, "u4PacketSize > CMPB_BUFFER_SIZE Chunk size = 0x%08x!!Reset encrypt info!\n", u4PacketSize);
        *pu4ConsumeSize = u4OrgAvailSize;
        /*Reset encrypt info*/
        prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
        x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));
        return eSWDMX_HDR_PRS_FAIL;
    }
    else if (u4PacketSize > u4AvailSize)
    {
        LOG(3, "Chunk size is 0x%08x larger than feeder's, the size is 0x%08x\n", u4PacketSize, u4AvailSize);

        x_thread_delay(10);

        if (!fgRangeEnd)
        {
            prSwdmxCmpbInfo->fgIsBlock = TRUE;
            //prSwdmxCmpbInfo->u4BlockChunkSize = u4PacketSize;  !!!
            *pu4ConsumeSize = 0;
        }
        else
        {
            *pu4ConsumeSize = u4AvailSize;
            /*Reset encrypt info*/
            LOG(1,"Range End, Reset encrypt info.\n");
            prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
            x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));  
            LOG(5, "Range End!!!\n");
        }

        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }
    else if (u4PacketSize == 0)
    {
        LOG(1, "\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!! Payload size is 0, AP problem??????\n\n\n");
        *pu4ConsumeSize = i + CMPB_HDR_LEN;
        /*Reset encrypt info*/
        LOG(1,"Payload size is 0, Reset encrypt info.\n");
        prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
        x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));   
        return eSWDMX_HDR_PRS_FAIL;
    }

    // Disable streams here
    if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        if ((i4PlaySpeed != SWDMX_PLAY_SPEED_1X) || (prSwdmxCmpbInfo->fgEnAudio == FALSE))
        {
            prSwdmxCmpbInfo->u4PullBackAudPts = (UINT32)u8Pts;  // record the PTS for later usage

            u1HdrType = eSWDMX_MEDIA_SKIPPED;
            if (prSwdmxCmpbInfo->fgEnAudio == FALSE)
            {
                LOG(1,"prSwdmxCmpbInfo->fgEnAudio == FALSE,Skip Audio!\n");
            }
            else 
            {
                LOG(1,"PlaySpeed is not 1000,Skip Audio!\n");
            }
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        if (prSwdmxCmpbInfo->fgEnVideo == FALSE)
        {
            u1HdrType = eSWDMX_MEDIA_SKIPPED;
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        UINT32 u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

        if((UINT32)u8Pts < u4CurPts || prSwdmxCmpbInfo->fgEnSubTitle == FALSE)
        {
             u1HdrType = eSWDMX_MEDIA_SKIPPED;
             LOG(2, "skip subtitle, pts = 0x%x, cur_pts = 0x%x, fgEnSub = %u\n", (UINT32)u8Pts, u4CurPts, prSwdmxCmpbInfo->fgEnSubTitle);
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO)
    {
    }

    if (u1HdrType == eSWDMX_MEDIA_SKIPPED ||
        u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO)
    {
        *pu4ConsumeSize = u4PacketSize + CMPB_HDR_LEN;
        /*Reset encrypt info*/
        LOG(1,"Skipped packet, Reset encrypt info.\n");
        prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
        x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));    
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    // Get available size here (we don't have to query subtitle available size here)
    //if (u1HdrType != eSWDMX_MEDIA_SUBTITLE)
    {
        if (u1HdrType == eSWDMX_MEDIA_VIDEO)
        {
            UINT8 u1PidIdx = prSwdmxCmpbInfo->u1VidPid;
            u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_VIDEO, u1PidIdx);
        }
        else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
        {
            UINT8 u1PidIdx = prSwdmxCmpbInfo->u1AudPid;
            u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_AUDIO, u1PidIdx);
        }
        else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)
        {
            UINT8 u1SubPid = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid;
            u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_OTHER, u1SubPid);
        }

        if (u4PacketSize > u4DmxAvailSize)
        {
            if (prSwdmxCmpbInfo->fgVDecIsPause)
            {
                LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 1\n");
                _SWDMX_CmpbVideoWakeUp(u1SrcId);
                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
            }

            if (u1HdrType == eSWDMX_MEDIA_AUDIO &&
                !prSwdmxCmpbInfo->fgEnVideo/* &&  megaa 20100621: no need to check this flag??
                prSwdmxCmpbInfo->fgAudIsReady*/)
            {
                AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                prSwdmxCmpbInfo->fgAudIsReady = FALSE;
            }

            LOG(7, "Chunk is blocked u1HdrType = %ld, u4PacketSize = %ld, u4DmxAvailSize = %ld\n",
                u1HdrType,
                u4PacketSize,
                u4DmxAvailSize);
            
            *pu4ConsumeSize = 0;
            prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(10);
            return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
        }

        //_SWDMX_CmpbInitDmx(u1SrcId, u1HdrType, FALSE);  megaa 20100618
    }

    prCmpbDmxMMData->u4FrameSize = u4PacketSize;

    if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
    }
    else if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        UINT16 u2MaxQNum, u2CurQNum;
        UINT16 u2DmxPictureQSize, u2DmxMaxQSize;

        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);

        if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && (IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type)))
        {
            if ((!IS_SPEC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type) && u2CurQNum > 60/*(prSwdmxCmpbInfo->u4VidFps * 3)*/)
            || ( IS_SPEC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type) && prSwdmxCmpbInfo->fgVDecIsPause))
            {
                if (prSwdmxCmpbInfo->fgVDecIsPause)
                {
                    LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 2\n");
                    _SWDMX_CmpbVideoWakeUp(u1SrcId);
                    prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
                }
                LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);
                *pu4ConsumeSize = 0;
                prSwdmxInst->fgFifoFull = TRUE;
                x_thread_delay(10);
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }

        /*buffer 1s for startup*/
        if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && IS_NEED_BUFFER_TO_STARTUP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type) && 
                (prSwdmxCmpbInfo->fgBufferToStartup == TRUE)) 
        {
            if (((prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX_3D) && 
				(u8Pts>prSwdmxCmpbInfo->u4FirstVideoPts) && (u8Pts-prSwdmxCmpbInfo->u4FirstVideoPts > 90000*3)) ||
        		((prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX_3D) &&
        		((u8Pts>prSwdmxCmpbInfo->u4FirstVideoPts) && (u8Pts-prSwdmxCmpbInfo->u4FirstVideoPts > 90000*2))))
        	{
        	
	            if (prSwdmxCmpbInfo->fgVDecIsPause)
	            {
	                LOG(0, "++++++++++++++++++++++++++++++++++++++ wake up. buffer enough data for startup.(0x%08x %08x)\n", (UINT32)u8Pts, prSwdmxCmpbInfo->u4FirstVideoPts);
	                _SWDMX_CmpbVideoWakeUp(u1SrcId);
	                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
	            }
	            
	            prSwdmxCmpbInfo->fgBufferToStartup = FALSE;
        	}
        }

        if (!prSwdmxCmpbInfo->fgEnAudio && prSwdmxCmpbInfo->fgVDecIsPause && u2CurQNum > 20)
        {
            LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up videoOnly\n");
            _SWDMX_CmpbVideoWakeUp(u1SrcId);
            prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
        }
        
        if (u2CurQNum > u2MaxQNum - 10)
        {
            LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);

            // megaa 20110822: patch for video only case
            if (!prSwdmxCmpbInfo->fgEnAudio && prSwdmxCmpbInfo->fgVDecIsPause)
            {
                LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 2A\n");
                _SWDMX_CmpbVideoWakeUp(u1SrcId);
                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
            }

            *pu4ConsumeSize = 0;
            prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(10);
            
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        else if(((u2DmxPictureQSize > 0) && (u2DmxMaxQSize > 384)) &&
            (u2DmxPictureQSize > (u2DmxMaxQSize - 384)))
        {
            LOG(7, "u2DmxPictureQSize = %d, u2DmxMaxQSize = %d .\n", u2DmxPictureQSize, u2DmxMaxQSize);

            // megaa 20110822: patch for video only case
            if (!prSwdmxCmpbInfo->fgEnAudio && prSwdmxCmpbInfo->fgVDecIsPause)
            {
                LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 2B\n");
                _SWDMX_CmpbVideoWakeUp(u1SrcId);
                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
            }

            *pu4ConsumeSize = 0;
            prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(10);
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        else if((u2DmxPictureQSize + u2CurQNum) >= u2MaxQNum) 
        {
             /*
               protect ESMQ overflow by the interrupt from dmx picture Q
               when os scheduled to dmx, it will dump all in hands picture Q to Vdec ESMQ
             */
            LOG(7, "u2DmxPictureQSize = %d, u2CurQNum = %d, u2MaxQNum = %d .\n", 
                u2DmxPictureQSize, u2CurQNum, u2MaxQNum);

            // megaa 20110822: patch for video only case
            if (!prSwdmxCmpbInfo->fgEnAudio && prSwdmxCmpbInfo->fgVDecIsPause)
            {
                LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 2C\n");
                _SWDMX_CmpbVideoWakeUp(u1SrcId);
                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
            }

            *pu4ConsumeSize = 0;
            prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(10);
            return eSWDMX_HDR_PRS_SUCCEED;
        }

        // Record every video's chunk position.
        prSwdmxCmpbInfo->u8VidOffset = prSwdmxCmpbInfo->u8CurDmxFilePos;
    }

    prCmpbDmxMMData->u4BufStart = (UINT32)(VIRTUAL(prCmpbFeederInfo->u4StartAddr));
    prCmpbDmxMMData->u4BufEnd = (UINT32)(VIRTUAL(prCmpbFeederInfo->u4EndAddr));
    prCmpbDmxMMData->u4Pts = (UINT32)u8Pts;
    prCmpbDmxMMData->u4Dts = (UINT32)u8Pts;

    if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VDecType == ENUM_SWDMX_VDEC_WMV9)
    {      
        prCmpbDmxMMData->fgFrameHead  = TRUE; 
    }
    
    if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        UINT32 u4SegIndex = 0;
        UINT32 u4Offset = 0;
        prCmpbDmxMMData->u4FrameSize = u4PacketSize;

        if (prCmpbDmxMMData->u4StartAddr < prCmpbDmxMMData->u4BufStart)
        {
            prCmpbDmxMMData->u4BufStart -= FEEDER_RESERVED_SIZE;
        }

        if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) &&
              (!IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type)) && prSwdmxCmpbInfo->fgWaitFirstAudio)
        {
            prSwdmxCmpbInfo->u4TriggerAudPts = (UINT32)u8Pts;
            LOG(3, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 1st audio pts = 0x%08X(%dms)\n", (UINT32)u8Pts, (UINT32)u8Pts / 90);
            AUD_SetStartPts(prSwdmxInst->u1AdecId, (UINT32)u8Pts);

            if (prSwdmxCmpbInfo->fgEnVideo)
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, (UINT32)u8Pts);
            }

            prSwdmxCmpbInfo->fgWaitFirstAudio = FALSE;
        }
        
        if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
            prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eDRMType == eSWDMX_DRM_TYPE_PDCF && prSwdmxCmpbInfo->fgAesCrtCrypt)
            {
                AES_PARAM_T rAesParam;
                UINT32 u4Tmp = prCmpbDmxMMData->u4StartAddr;
                UINT32 u4SubCount = 0;
                UINT32 u4ClearLen = 0;
                UINT32 u4EncryptLen = 0;
                UINT32 u4ClearLen2 = 0;
                UINT32 u4EncryptLen2 = 0;
                UINT32 u4AllLen = 0;
                UINT32 u4Align = 16;
                UINT32 u4TempPtr = 0;

                x_memcpy(rAesParam.au1Key, prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1AudioDrmKey[0], 16);

                if(TRUE == _SWDMX_CmpbGetStaticFlagValue(FLAG_TEST_PDCF))
                {
                    x_memcpy(rAesParam.au1Iv, &(prSwdmxCmpbInfo->u1AudioIV[0]), 16);
                }
                else
                {
                    x_memcpy(rAesParam.au1Iv, &(prSwdmxCmpbInfo->rAesCrtCrypt.ui1_iv), 16);           
                }

                x_memcpy(rAesParam.au1Iv, &(prSwdmxCmpbInfo->rAesCrtCrypt.ui1_iv), 16);

                if (prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.ui4_sub_sample_cnt == 0)
                {
                    u4ClearLen = 0;
                    u4EncryptLen = prCmpbDmxMMData->u4FrameSize;
                    
           #if 0
                    {
                        static UINT32 i = 0;
                        static BOOL fgFlag = TRUE;

                        if (fgFlag)
                        {
                            printf("\n*****  KEY  *******\n");
                            for ( i = 0; i < 16; i++ )
                            {
                                printf("0x%02x ", (UINT8)(rAesParam.au1Key[i]));
                            }

                            printf("\n\n*****  IV  *******\n");
                            for ( i = 0; i < 16; i++ )
                            {
                                printf("0x%02x ", (UINT8)(rAesParam.au1Iv[i]));
                            }
                            printf("\n\n");
                            //fgFlag = FALSE;
                        }
                    }
           #endif
                    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                    rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
                    rAesParam.u4SrcBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                    rAesParam.u4SrcBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                    rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
                    rAesParam.u4DstBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                    rAesParam.u4DstBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                    rAesParam.u4DatLen  = u4EncryptLen;;

                    rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                    rAesParam.fgEncrypt = FALSE;

                    LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                    GCPU_Cmd(0, GCPU_AES, &rAesParam);
                }
                else
                {
                    while (u4SubCount < prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.ui4_sub_sample_cnt && u4SubCount < 32)
                    {
                        u4ClearLen = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui2_size_clear_data;
                        u4EncryptLen = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui4_size_encrypted_data;
               #if 0
                        {
                            static UINT32 i = 0;
                            static BOOL fgFlag = TRUE;

                            if (fgFlag)
                            {
                                printf("\n*****  KEY  *******\n");
                                for ( i = 0; i < 16; i++ )
                                {
                                    printf("0x%02x ", (UINT8)(rAesParam.au1Key[i]));
                                }

                                printf("\n\n*****  IV  *******\n");
                                for ( i = 0; i < 16; i++ )
                                {
                                    printf("0x%02x ", (UINT8)(rAesParam.au1Iv[i]));
                                }
                                printf("\n\n");
                                //fgFlag = FALSE;
                            }
                        }
               #endif
                        if (u4EncryptLen == 0)
                        {
                            u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                            u4SubCount++;
                            continue;
                        }
                        else if ( u4EncryptLen % 16 != 0 )
                        {
                            LOG(0, "encrypt length % 16 is not 0  !!!!\n");

                            if ( u4SubCount < prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.ui4_sub_sample_cnt - 1 )
                            {
                                u4ClearLen2 = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui2_size_clear_data;
                                u4EncryptLen2 = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui4_size_encrypted_data;
                            }
                            else
                            {
                                LOG(0, "the last block is error!!!!!!\n");
                                break;
                            }

                            _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &u4TempPtr, &u4EncryptLen, &u4Align);

                            x_memcpy((UINT8 *)u4TempPtr, (VOID *)(u4Tmp + u4ClearLen), u4EncryptLen);
                            x_memcpy((UINT8 *)(u4TempPtr + u4EncryptLen), (VOID *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), u4EncryptLen2);

                            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                            rAesParam.u4SrcStartAddr = VIRTUAL(u4TempPtr);
                            rAesParam.u4SrcBufStart = VIRTUAL(u4TempPtr);
                            rAesParam.u4SrcBufEnd = VIRTUAL(u4TempPtr);
                            rAesParam.u4DstStartAddr = VIRTUAL(u4TempPtr);
                            rAesParam.u4DstBufStart = VIRTUAL(u4TempPtr);
                            rAesParam.u4DstBufEnd = VIRTUAL(u4TempPtr);
                            rAesParam.u4DatLen  = u4EncryptLen + u4EncryptLen2;

                            rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                            rAesParam.fgEncrypt = FALSE;

                            LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                            GCPU_Cmd(0, GCPU_AES, &rAesParam);

                            x_memcpy((UINT8 *)(u4Tmp + u4ClearLen), (VOID *)u4TempPtr, u4EncryptLen);
                            x_memcpy((UINT8 *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), (VOID *)(u4TempPtr + u4EncryptLen), u4EncryptLen2);

                            u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                            u4SubCount += 2;
                        }
                        else
                        {            
                            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                            rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                            rAesParam.u4SrcBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                            rAesParam.u4SrcBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                            rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                            rAesParam.u4DstBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                            rAesParam.u4DstBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                            rAesParam.u4DatLen  = u4EncryptLen;

                            rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                            rAesParam.fgEncrypt = FALSE;

                            LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                            GCPU_Cmd(0, GCPU_AES, &rAesParam);

                            u4AllLen += (u4ClearLen + u4EncryptLen);
                            u4SubCount++;
                            
                        }
                    }
                }
                
                prSwdmxCmpbInfo->fgAesCrtCrypt = FALSE;
            }

            if (prSwdmxCmpbInfo->u4ADecType == ENUM_SWDMX_ADEC_AAC)
            {
                UINT8 u1SampleRateIdx = 3;
                UINT32 u4RequestSize = AAC_ALLOC_SIZE;
                UINT32 u4Align = 16;
                DMX_MM_DATA_T rDmxMMData;
                x_memset(&rDmxMMData,0,sizeof(DMX_MM_DATA_T));
                if(prSwdmxInst->pucAudBuf == 0)
                {
                    LOG(1, "pucAudBuf should be allocated in init function");

                    _SWDMX_GetInfo(prSwdmxInst->u1SwdmxId,eSWDMX_GET_INTERNAL_BUF, (UINT32*)prSwdmxInst->pucAudBuf, &u4RequestSize, &u4Align); 
                }                               
                if (prSwdmxInst->pucAudBuf)
                {
                    /* 8b: syncword */
                    prSwdmxInst->pucAudBuf[0] = 0xFF;
                    /* 4b: syncword */
                    /* 1b: mpeg id = 1 */
                    /* 2b: layer = 0 */
                    /* 1b: protection absent */
                    prSwdmxInst->pucAudBuf[1] = 0xF9;
                    /* 2b: profile */
                    /* 4b: sampling_frequency_index */
                    /* 1b: private = 0 */
                    /* 1b: channel_configuration */
                    switch (prSwdmxCmpbInfo->u4AudSampleRate)
                    {
                        case 96000:
                            u1SampleRateIdx = 0;
                                break;
                        case 88200:
                            u1SampleRateIdx = 1;
                            break;
                        case 64000:
                            u1SampleRateIdx = 2;
                            break;
                        case 48000:
                            u1SampleRateIdx = 3;
                            break;
                        case 44100:
                            u1SampleRateIdx = 4;
                            break;
                        case 32000:
                            u1SampleRateIdx = 5;
                            break;
                        case 24000:
                            u1SampleRateIdx = 6;
                            break;
                        case 22050:
                            u1SampleRateIdx = 7;
                            break;
                        case 16000:
                            u1SampleRateIdx = 8;
                            break;
                        case 12000:  
                            u1SampleRateIdx = 9;
                            break;
                        case 11025:  
                            u1SampleRateIdx = 10;
                            break;
                        case 8000:  
                            u1SampleRateIdx = 11;
                            break;
                        default:
                            u1SampleRateIdx = 4;
                            break;
                    }

                    prSwdmxInst->pucAudBuf[2] = (1 << 6) |
                             ((u1SampleRateIdx<< 2) & 0x3C) |
                         ((prSwdmxCmpbInfo->u4AudChannelNs >> 2) & 0x1);
                    /* 2b: channel_configuration */
                    /* 1b: original */
                    /* 1b: home */
                    /* 1b: copyright_id */
                    /* 1b: copyright_id_start */
                    /* 2b: aac_frame_length */

                    prSwdmxInst->pucAudBuf[3] = ((prSwdmxCmpbInfo->u4AudChannelNs & 0x3) << 6) |
                             (((prCmpbDmxMMData->u4FrameSize + AAC_FAKE_HDR_SIZE) >> 11) & 0x3);
                    /* 8b: aac_frame_length */
                    prSwdmxInst->pucAudBuf[4] = (((prCmpbDmxMMData->u4FrameSize + AAC_FAKE_HDR_SIZE) >> 3) & 0xFF);
                    /* 3b: aac_frame_length */
                    /* 5b: adts_buffer_fullness */
                    prSwdmxInst->pucAudBuf[5] = (((prCmpbDmxMMData->u4FrameSize + AAC_FAKE_HDR_SIZE) << 5) & 0xE0) |
                             ((0x7FF >> 6) & 0x1F);
                    /* 6b: adts_buffer_fullness */
                    /* 2b: number_of_raw_data_blocks_in_frame */
                    prSwdmxInst->pucAudBuf[6] = ((0x7FF << 2) & 0xFC);
                    rDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucAudBuf;
                    rDmxMMData.u4BufEnd = rDmxMMData.u4BufStart + AAC_ALLOC_SIZE;
                    rDmxMMData.u4StartAddr = rDmxMMData.u4BufStart;
                    rDmxMMData.u4FrameSize = AAC_FAKE_HDR_SIZE;
                    rDmxMMData.u4Pts = prCmpbDmxMMData->u4Pts;
                    rDmxMMData.u1Idx =prSwdmxCmpbInfo->u1AudPid;

                    if(DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1AudPid)!=DMX_DRM_MODE_BYPASS)
                    {           
                        if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1AudPid,DMX_DRM_MODE_BYPASS))
                        {              
                            LOG(0,"VPush:DMX_MM_SetDRMMode set error..........\n");
                            ASSERT(0);  // !!!
                            *pu4ConsumeSize = 0;
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;            
                        }       
                    }    
                                        
                    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                    {
                        LOG(1, "Demuxer fails to move data. SrcID = %d, Header Type = %d\n",
                            prSwdmxInst->u1SwdmxId,
                            u1HdrType);
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    } 
                }
            }
        }

#if (defined(SWDMX_IS_POST_MT5398))
        if (prSwdmxCmpbInfo->fgIsEncrypt == TRUE && prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum > 0)
        {
            prCmpbDmxMMData->u4FrameSize = 0;

            while (u4SegIndex < prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum && u4SegIndex < FEEDER_CTRL_MTK_ENCRYPT_SEGMENT_MAX_NUM)
            {
                u4MoveRetryCnt = 0;

                if (prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex].fgEnc == TRUE)
                {
                    DMX_MM_DRM_AES_T rAesSetting;

                    x_memset((void*)&rAesSetting, 0, sizeof(DMX_MM_DRM_AES_T));

                    if (u4SegIndex > 0 && prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex - 1].fgEnc == FALSE)
                    {
                        u4Offset += prCmpbDmxMMData->u4FrameSize;
                    }
  
                    prCmpbDmxMMData->u4FrameSize += prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex].u4Size;

                    if (u4SegIndex < prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum - 1 && 
                        u4SegIndex + 1 < FEEDER_CTRL_MTK_ENCRYPT_SEGMENT_MAX_NUM &&
                        prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex + 1].fgEnc == TRUE)
                    {
                        u4SegIndex++;
                        continue;
                    }

                    if (DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1AudPid) != DMX_DRM_MODE_ND)
                    {
                        LOG(9, "DMX_MM_SetDRMMode DMX_DRM_MODE_ND\n");
                        if (!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1AudPid, DMX_DRM_MODE_ND))
                        {
                            LOG(0,"DMX_MM_SetDRMMode set error..........\n");
                            ASSERT(0);  // !!!
                            *pu4ConsumeSize = 0;
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;  
                        }
                    }

                    LOG(6, "%s: DMX_MM_SetAES, u1AudPid = %u, pts = 0x%x, u4Offset = %u, size = %u\n", 
                    __FUNCTION__, 
                    prSwdmxCmpbInfo->u1AudPid,
                    prCmpbDmxMMData->u4Pts,
                    u4Offset,
                    prCmpbDmxMMData->u4FrameSize);

                    // set it before each MM move
                    rAesSetting.fgCBC = FALSE;
                    rAesSetting.u4KeyBitLen = 128;
                    rAesSetting.u4Offset = u4Offset;                
                    rAesSetting.u4EncryLen = prCmpbDmxMMData->u4FrameSize - u4Offset;  
                    rAesSetting.fgEncrypt = FALSE;

					x_memcpy(rAesSetting.au1IV, (UINT8 *)prSwdmxCmpbInfo->rMtkEncryptInfo.au1IV, 16);

                    if(!DMX_MM_SetSecureAES(prSwdmxCmpbInfo->u1AudPid, (UINT32)DMX_MM_DRM_FLAG_ALL, &rAesSetting, TRUE))
                    {
                        LOG(0,"DMX_MM_SetSecureAES set error..........\n");
                        ASSERT(0);  // !!!
                        *pu4ConsumeSize = 0;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;  
                    }

                    if ( 1/*u4Offset > 0*/ )
                    {
                        prCmpbDmxMMData->fgTrigger = TRUE;
                    }
                }
                else
                {
                    prCmpbDmxMMData->fgTrigger = FALSE;
                    prCmpbDmxMMData->u4FrameSize += prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex].u4Size;

                    if (u4SegIndex < prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum - 1 && u4SegIndex + 1 < FEEDER_CTRL_MTK_ENCRYPT_SEGMENT_MAX_NUM)
                    {
                        u4SegIndex++;
                        continue;
                    }
                    
                    if(DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1AudPid) != DMX_DRM_MODE_BYPASS)
                    {           
                        LOG(1, "DMX_MM_SetDRMMode DMX_DRM_MODE_BYPASS\n");
                        if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1AudPid, DMX_DRM_MODE_BYPASS))
                        {              
                            LOG(0,"DMX_MM_SetDRMMode set error..........\n");
                            ASSERT(0);  // !!!
                            *pu4ConsumeSize = 0;
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;         
                        }       
                    }
                }

                LOG(6, "_SWDMX_MoveData, size = 0x%x ( %u ), startaddr = 0x%x,  pts = 0x%x\n", 
                        prCmpbDmxMMData->u4FrameSize, 
                        prCmpbDmxMMData->u4FrameSize,
                        prCmpbDmxMMData->u4StartAddr,
                        prCmpbDmxMMData->u4Pts);

                prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1AudPid;

                while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
                {
                    LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
                    u4MoveRetryCnt++;
                    if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
                    {
                        LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                        ASSERT(0);  // !!!
                        *pu4ConsumeSize = 0;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                }

                u4Offset = 0;
                u4SegIndex++;
                prCmpbDmxMMData->u4StartAddr += prCmpbDmxMMData->u4FrameSize;
                prCmpbDmxMMData->u4FrameSize = 0;
            };

            // reset encrypt info
            prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
            x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));
        }
        else
        {
            if(DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1AudPid)!=DMX_DRM_MODE_BYPASS)
            {           
                if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1AudPid,DMX_DRM_MODE_BYPASS))
                {              
                    LOG(0,"VPush:DMX_MM_SetDRMMode set error..........\n");
                    ASSERT(0);  // !!!
                    *pu4ConsumeSize = 0;
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;            
                }       
            }

            prCmpbDmxMMData->u4FrameSize = u4PacketSize;
            u4MoveRetryCnt = 0;
            prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1AudPid;

            while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
            {
                LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
                u4MoveRetryCnt++;

                if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
                {
                    LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                    ASSERT(0);  // !!!
                    *pu4ConsumeSize = 0;
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
            }
        }

#else
#if 1
        u4MoveRetryCnt = 0;
        prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1AudPid;

        while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
        {
            LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
            u4MoveRetryCnt++;
            if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
            {
                LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                ASSERT(0);  // !!!
                *pu4ConsumeSize = 0;
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
#else
        prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1AudPid;
        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
        {
            LOG(0, "Demuxer fails to move data.\n");
            ASSERT(0);  // !!!
            *pu4ConsumeSize = 0;
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
#endif
#endif
    }
    else if (u1HdrType == eSWDMX_MEDIA_VIDEO)  // video
    {
        UINT32 u4SegIndex = 0;
        UINT32 u4Offset = 0;

        prCmpbDmxMMData->u4FrameSize = u4PacketSize;
        prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1VidPid;

        if (prCmpbDmxMMData->u4StartAddr < prCmpbDmxMMData->u4BufStart)
            prCmpbDmxMMData->u4BufStart -= FEEDER_RESERVED_SIZE;

        if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) &&(!IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type)) && 
              prSwdmxCmpbInfo->fgWaitFirstVideo)
        {
            LOG(3, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 1st video pts = 0x%08X(%dms)\n", (UINT32)u8Pts, (UINT32)u8Pts / 90);

            prSwdmxCmpbInfo->u4FirstVideoPts = (UINT32)u8Pts;   
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            VDEC_Pause(prSwdmxInst->u1VdecId);  // tmp solution to deal with unexpected MW's calling VDEC_Play
            prSwdmxCmpbInfo->fgVDecIsPause = TRUE;

            prSwdmxCmpbInfo->fgWaitFirstVideo = FALSE;

            if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
                prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
            {
                _SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);
            }
        }
        
        if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
            prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
        {
            if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eDRMType == eSWDMX_DRM_TYPE_PDCF && prSwdmxCmpbInfo->fgAesCrtCrypt)
            {
                AES_PARAM_T rAesParam;
                UINT32 u4Tmp = prCmpbDmxMMData->u4StartAddr;
                UINT32 u4SubCount = 0;
                UINT32 u4ClearLen = 0;
                UINT32 u4EncryptLen = 0;
                UINT32 u4ClearLen2 = 0;
                UINT32 u4EncryptLen2 = 0;
                UINT32 u4AllLen = 0;
                UINT32 u4Align = 16;
                UINT32 u4TempPtr = 0;

                x_memcpy(rAesParam.au1Key, prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1VideoDrmKey, 16);

                if(TRUE == _SWDMX_CmpbGetStaticFlagValue(FLAG_TEST_PDCF))
                {
                    x_memcpy(rAesParam.au1Iv, &(prSwdmxCmpbInfo->u1VideoIV[0]), 16);
                }
                else
                {
                    x_memcpy(rAesParam.au1Iv, &(prSwdmxCmpbInfo->rAesCrtCrypt.ui1_iv), 16);          
                }

                x_memcpy(rAesParam.au1Iv, &(prSwdmxCmpbInfo->rAesCrtCrypt.ui1_iv), 16);

                if (prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.ui4_sub_sample_cnt == 0)
                {
                    u4ClearLen = 0;
                    u4EncryptLen = prCmpbDmxMMData->u4FrameSize;
                    
           #if 0
                    {
                        static UINT32 i = 0;
                        static BOOL fgFlag = TRUE;

                        if (fgFlag)
                        {
                            printf("\n*****  KEY  *******\n");
                            for ( i = 0; i < 16; i++ )
                            {
                                printf("0x%02x ", (UINT8)(rAesParam.au1Key[i]));
                            }

                            printf("\n\n*****  IV  *******\n");
                            for ( i = 0; i < 16; i++ )
                            {
                                printf("0x%02x ", (UINT8)(rAesParam.au1Iv[i]));
                            }
                            printf("\n\n");
                            //fgFlag = FALSE;
                        }
                    }
           #endif
                
                    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                    rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp);
                    rAesParam.u4SrcBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                    rAesParam.u4SrcBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                    rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp);
                    rAesParam.u4DstBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                    rAesParam.u4DstBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                    rAesParam.u4DatLen  = u4EncryptLen;;

                    rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                    rAesParam.fgEncrypt = FALSE;

                    LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                    GCPU_Cmd(0, GCPU_AES, &rAesParam);
                }
                else
                {
                    while (u4SubCount < prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.ui4_sub_sample_cnt && u4SubCount < 32)
                    {
                        u4ClearLen = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui2_size_clear_data;
                        u4EncryptLen = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui4_size_encrypted_data;
               #if 0
                        {
                            static UINT32 i = 0;
                            static BOOL fgFlag = TRUE;

                            if (fgFlag)
                            {
                                printf("\n*****  KEY  *******\n");
                                for ( i = 0; i < 16; i++ )
                                {
                                    printf("0x%02x ", (UINT8)(rAesParam.au1Key[i]));
                                }

                                printf("\n\n*****  IV  *******\n");
                                for ( i = 0; i < 16; i++ )
                                {
                                    printf("0x%02x ", (UINT8)(rAesParam.au1Iv[i]));
                                }
                                printf("\n\n");
                                //fgFlag = FALSE;
                            }
                        }
               #endif
                        if (u4EncryptLen == 0)
                        {
                            u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                            u4SubCount++;
                            continue;
                        }
                        else if ( u4EncryptLen % 16 != 0 )
                        {
                            LOG(0, "encrypt length % 16 is not 0  !!!!\n");

                            if ( u4SubCount < prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.ui4_sub_sample_cnt - 1 )
                            {
                                u4ClearLen2 = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui2_size_clear_data;
                                u4EncryptLen2 = prSwdmxCmpbInfo->rAesCrtCrypt.t_dash_senc.a_sub_sample[u4SubCount].ui4_size_encrypted_data;
                            }
                            else
                            {
                                LOG(0, "the last block is error!!!!!!\n");
                                break;
                            }

                            _SWDMX_GetInfo(u1SrcId,eSWDMX_GET_INTERNAL_BUF, &u4TempPtr, &u4EncryptLen, &u4Align);

                            x_memcpy((UINT8 *)u4TempPtr, (VOID *)(u4Tmp + u4ClearLen), u4EncryptLen);
                            x_memcpy((UINT8 *)(u4TempPtr + u4EncryptLen), (VOID *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), u4EncryptLen2);

                            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                            rAesParam.u4SrcStartAddr = VIRTUAL(u4TempPtr);
                            rAesParam.u4SrcBufStart = VIRTUAL(u4TempPtr);
                            rAesParam.u4SrcBufEnd = VIRTUAL(u4TempPtr);
                            rAesParam.u4DstStartAddr = VIRTUAL(u4TempPtr);
                            rAesParam.u4DstBufStart = VIRTUAL(u4TempPtr);
                            rAesParam.u4DstBufEnd = VIRTUAL(u4TempPtr);
                            rAesParam.u4DatLen  = u4EncryptLen + u4EncryptLen2;

                            rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                            rAesParam.fgEncrypt = FALSE;

                            LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                            GCPU_Cmd(0, GCPU_AES, &rAesParam);

                            x_memcpy((UINT8 *)(u4Tmp + u4ClearLen), (VOID *)u4TempPtr, u4EncryptLen);
                            x_memcpy((UINT8 *)(u4Tmp + u4ClearLen + u4EncryptLen + u4ClearLen2), (VOID *)(u4TempPtr + u4EncryptLen), u4EncryptLen2);

                            u4AllLen += (u4ClearLen + u4EncryptLen + u4ClearLen2 + u4EncryptLen2);
                            u4SubCount += 2;
                        }
                        else
                        {            
                            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
                            rAesParam.u4SrcStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                            rAesParam.u4SrcBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                            rAesParam.u4SrcBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                            rAesParam.u4DstStartAddr = VIRTUAL(u4Tmp + u4ClearLen + u4AllLen);
                            rAesParam.u4DstBufStart = VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                            rAesParam.u4DstBufEnd = VIRTUAL(prCmpbFeederInfo->u4EndAddr);
                            rAesParam.u4DatLen  = u4EncryptLen;

                            rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
                            rAesParam.fgEncrypt = FALSE;

                            LOG(2, "GCPU_Cmd,  size = %u\n", rAesParam.u4DatLen);
                            GCPU_Cmd(0, GCPU_AES, &rAesParam);

                            u4AllLen += (u4ClearLen + u4EncryptLen);
                            u4SubCount++;
                            
                        }
                    }
                }
                
                prSwdmxCmpbInfo->fgAesCrtCrypt = FALSE;
            }
     
            //replace length with start code            
            if (!prSwdmxCmpbInfo->fgIsEncrypt && prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_H264)
            {
                UCHAR* pucBuf = NULL;
                UCHAR *pucFdrBuf = NULL;
                UINT32 u4NaluSize = 0, u4FrmSize = 0;
                DMX_MM_DATA_T rDmxMMData;
                UINT32 u4DmxVdecAvailSize = 0;
                UINT32 u4MarlinOffset = 0;

                pucFdrBuf = (UCHAR*)prCmpbDmxMMData->u4StartAddr;
                pucBuf = pucFdrBuf;
                u4FrmSize = 0;
                
                if (prSwdmxCmpbInfo->u1StartCodeLenMinus1 <= H264_STARTCODE_LEN)
                {
                    while(u4FrmSize < u4PacketSize - u4MarlinOffset)
                    {
                        if(u4FrmSize >= u4PacketSize - u4MarlinOffset - 4)
                        {
                            LOG(3, "Skip abnormal nalu size!\n");
                            break;
                        }

                        u4NaluSize = 0;
                        //insert start code
                        for(i = 0; i < prSwdmxCmpbInfo->u1StartCodeLenMinus1; i++)
                        {
                            u4NaluSize += (UINT32)*pucFdrBuf;
                            u4NaluSize <<= 8;
                            *pucFdrBuf++ = 0;
                            
                            //check ring buffer
                            if((UINT32)pucFdrBuf == VIRTUAL(prCmpbFeederInfo->u4EndAddr))
                            {
                                pucFdrBuf = (UCHAR*)VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                            }
                        }

                        u4NaluSize += (UINT32)*pucFdrBuf;
                        if (u4NaluSize == 0)
                        {  
                            *pu4ConsumeSize = u4PacketSize + CMPB_HDR_LEN;
                            LOG(0,">>>>>u4NaluSize == 0 return fail!\n");
                            return eSWDMX_HDR_PRS_SUCCEED;
                        }
                        
                        *pucFdrBuf++ = 1;

                        if((prSwdmxCmpbInfo->u1StartCodeLenMinus1 < H264_STARTCODE_LEN) &&
                            (prSwdmxCmpbInfo->u1StartCodeLenMinus1 > 0))
                        {
                            UINT32   u4Loop = 0;
                            UINT32   u4DataSize = 0;
                            
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
                            HalFlushInvalidateDCacheMultipleLine(prCmpbFeederInfo->u4StartAddr, 
                                (UINT32)(prCmpbFeederInfo->u4EndAddr - prCmpbFeederInfo->u4StartAddr));
#else
                            HalFlushInvalidateDCache();
#endif
                            for(u4Loop=0; u4Loop<(H264_STARTCODE_LEN - prSwdmxCmpbInfo->u1StartCodeLenMinus1); u4Loop++)
                            {
                                if((UINT32)pucBuf == VIRTUAL(prCmpbFeederInfo->u4StartAddr))
                                {
                                    pucBuf = (UCHAR*)VIRTUAL(prCmpbFeederInfo->u4EndAddr - 1);
                                }
                                else
                                {
                                    pucBuf--;
                                }

                                *pucBuf = 0;
                            }

                            u4DataSize = u4NaluSize + (H264_STARTCODE_LEN-
                                                prSwdmxCmpbInfo->u1StartCodeLenMinus1)+
                                                (prSwdmxCmpbInfo->u1StartCodeLenMinus1 + 1);
                            
                            rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prCmpbFeederInfo->u4StartAddr));
                            rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prCmpbFeederInfo->u4EndAddr));
                            rDmxMMData.u4StartAddr = (UINT32)pucBuf;
                            rDmxMMData.u4FrameSize = u4DataSize;
                            rDmxMMData.u4Pts = (UINT32)u8Pts;
                            rDmxMMData.u1Idx = prSwdmxCmpbInfo->u1VidPid;

                            u4DmxVdecAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_VIDEO,
                                                                      prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1VidPid);
                            if(u4DataSize >= u4DmxVdecAvailSize)
                            {
                                x_thread_delay(10);
                                
                                if (!fgRangeEnd)
                                {
                                    prSwdmxCmpbInfo->fgIsBlock = TRUE;
                                    //prSwdmxCmpbInfo->u4BlockChunkSize = u4PacketSize;  !!!
                                    *pu4ConsumeSize = 0;
                                }
                                else
                                {
                                    *pu4ConsumeSize = u4AvailSize;
                                    LOG(5, "Range End!!!\n");
                                }

                                return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
                            }
                            
                            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                            {
                                LOG(1, "Demuxer fails to move data.\n");
                                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                            }
                        }

                        //check ring buffer
                        if((UINT32)pucFdrBuf == VIRTUAL(prCmpbFeederInfo->u4EndAddr))
                        {
                            pucFdrBuf = (UCHAR*)VIRTUAL(prCmpbFeederInfo->u4StartAddr);
                        }
                        // nalu size
                        u4FrmSize += ((prSwdmxCmpbInfo->u1StartCodeLenMinus1 + 1) + u4NaluSize);
                        pucFdrBuf += u4NaluSize;
                        //check ring buffer
                        if((UINT32)pucFdrBuf >= VIRTUAL(prCmpbFeederInfo->u4EndAddr))
                        {
                            pucFdrBuf = 
                            (UCHAR*)((UINT32)pucFdrBuf - 
                            VIRTUAL(prCmpbFeederInfo->u4EndAddr) + 
                            VIRTUAL(prCmpbFeederInfo->u4StartAddr));
                        }
                        pucBuf = pucFdrBuf;
                    }
                }
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
                HalFlushInvalidateDCacheMultipleLine(prCmpbFeederInfo->u4StartAddr, 
                    (UINT32)(prCmpbFeederInfo->u4EndAddr - prCmpbFeederInfo->u4StartAddr));
#else
                HalFlushInvalidateDCache();
#endif
            }
        }

#if (defined(SWDMX_IS_POST_MT5398))
        if (prSwdmxCmpbInfo->fgIsEncrypt == TRUE && prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum > 0)
        {
            prCmpbDmxMMData->fgTrigger = FALSE;
            prCmpbDmxMMData->u4FrameSize = 0;
            
            while (u4SegIndex < prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum && u4SegIndex < FEEDER_CTRL_MTK_ENCRYPT_SEGMENT_MAX_NUM)
            {
                u4MoveRetryCnt = 0;

                if (prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex].fgEnc == TRUE)
                {
                    DMX_MM_DRM_AES_T rAesSetting;

                    x_memset((void*)&rAesSetting, 0, sizeof(DMX_MM_DRM_AES_T));

                    if (u4SegIndex > 0 && prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex - 1].fgEnc == FALSE)
                    {
                        u4Offset += prCmpbDmxMMData->u4FrameSize;
                    }
  
                    prCmpbDmxMMData->u4FrameSize += prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex].u4Size;
                    if (u4SegIndex < prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum - 1 && 
                        u4SegIndex + 1 < FEEDER_CTRL_MTK_ENCRYPT_SEGMENT_MAX_NUM &&
                        prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex + 1].fgEnc == TRUE)
                    {
                        u4SegIndex++;
                        continue;
                    }

                    if (DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1VidPid) != DMX_DRM_MODE_ND)
                    {
                        LOG(9, "DMX_MM_SetDRMMode DMX_DRM_MODE_ND\n");
                        if (!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1VidPid, DMX_DRM_MODE_ND))
                        {
                            LOG(0,"DMX_MM_SetDRMMode set error..........\n");
                            ASSERT(0);  // !!!
                            *pu4ConsumeSize = 0;
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;  
                        }
                    }

                    LOG(6, "%s: DMX_MM_SetAES, u1VidPid = %u, pts = 0x%x, u4Offset = %u, size = %u\n", 
                    __FUNCTION__, 
                    prSwdmxCmpbInfo->u1VidPid,
                    prCmpbDmxMMData->u4Pts,
                    u4Offset,
                    prCmpbDmxMMData->u4FrameSize);

                    // set it before each MM move
                    rAesSetting.fgCBC = FALSE;
                    rAesSetting.u4KeyBitLen = 128;
                    rAesSetting.u4Offset = u4Offset;                
                    rAesSetting.u4EncryLen = prCmpbDmxMMData->u4FrameSize - u4Offset;  
                    rAesSetting.fgEncrypt = FALSE;

					x_memcpy(rAesSetting.au1IV, (UINT8 *)prSwdmxCmpbInfo->rMtkEncryptInfo.au1IV, 16);
					
                    if(!DMX_MM_SetSecureAES(prSwdmxCmpbInfo->u1VidPid, (UINT32)DMX_MM_DRM_FLAG_ALL, &rAesSetting, TRUE))
                    {
                        LOG(0,"DMX_MM_SetSecureAES set error..........\n");
                        ASSERT(0);  // !!!
                        *pu4ConsumeSize = 0;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;  
                    }

                    if (1)
                    {
                        prCmpbDmxMMData->fgTrigger = TRUE;
                    }
     
                }
                else
                {
                    //prCmpbDmxMMData->fgTrigger = FALSE;
                    prCmpbDmxMMData->u4FrameSize += prSwdmxCmpbInfo->rMtkEncryptInfo.atSegInfo[u4SegIndex].u4Size;
                    if (u4SegIndex < prSwdmxCmpbInfo->rMtkEncryptInfo.u2SegNum - 1 && u4SegIndex + 1 < FEEDER_CTRL_MTK_ENCRYPT_SEGMENT_MAX_NUM)
                    {
                        u4SegIndex++;
                        continue;
                    }
                    
                    if(DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1VidPid) != DMX_DRM_MODE_BYPASS)
                    {           
                        LOG(1, "DMX_MM_SetDRMMode DMX_DRM_MODE_BYPASS\n");
                        if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1VidPid, DMX_DRM_MODE_BYPASS))
                        {              
                            LOG(0,"DMX_MM_SetDRMMode set error..........\n");
                            ASSERT(0);  // !!!
                            *pu4ConsumeSize = 0;
                            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;         
                        }       
                    }
                }

                LOG(6, "_SWDMX_MoveData, size = 0x%x ( %u ), startaddr = 0x%x,  pts = 0x%x\n", 
                        prCmpbDmxMMData->u4FrameSize, 
                        prCmpbDmxMMData->u4FrameSize,
                        prCmpbDmxMMData->u4StartAddr,
                        prCmpbDmxMMData->u4Pts);

                while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
                {
                    LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
                    u4MoveRetryCnt++;
                    if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
                    {
                        LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                        ASSERT(0);  // !!!
                        *pu4ConsumeSize = 0;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    }
                }

                u4Offset = 0;
                u4SegIndex++;
                prCmpbDmxMMData->u4StartAddr += prCmpbDmxMMData->u4FrameSize;
                prCmpbDmxMMData->u4FrameSize = 0;
            };

            // reset encrypt info
            prSwdmxCmpbInfo->fgIsEncrypt = FALSE;
            x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));
        }
        else
        {
            if(DMX_MM_GetDRMMode(prSwdmxCmpbInfo->u1VidPid)!=DMX_DRM_MODE_BYPASS)
            {           
                if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1VidPid,DMX_DRM_MODE_BYPASS))
                {              
                    LOG(0,"VPush:DMX_MM_SetDRMMode set error..........\n");
                    ASSERT(0);  // !!!
                    *pu4ConsumeSize = 0;
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;            
                }       
            }

            prCmpbDmxMMData->u4FrameSize = u4PacketSize;
            u4MoveRetryCnt = 0;

            while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
            {
                LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
                u4MoveRetryCnt++;
                if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
                {
                    LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                    ASSERT(0);  // !!!
                    *pu4ConsumeSize = 0;
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
            }
        }
#else
        prCmpbDmxMMData->u4FrameSize = u4PacketSize;
        u4MoveRetryCnt = 0;

        while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
        {
            LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
            u4MoveRetryCnt++;
            if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
            {
                LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                ASSERT(0);  // !!!
                *pu4ConsumeSize = 0;
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
#endif
        prSwdmxCmpbInfo->u4VidCount++;
#if 0  // debug log code
        {
            int n;
            UINT8 *pu1Ptr = (UINT8 *)VIRTUAL(prCmpbDmxMMData->u4StartAddr);
            LOG(7, "        v-piece: ");
            for (n = 0; n < 16; n++)
            {
                LOG(7, "%02X", (UINT32)(*pu1Ptr++));
            }
            LOG(7, " len = %d\n", prCmpbDmxMMData->u4FrameSize);
        }
#endif
    }
    else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)  // subtitle
    {
        prCmpbDmxMMData->u4FrameSize = u4PacketSize;
        
        if (prCmpbDmxMMData->u4StartAddr < prCmpbDmxMMData->u4BufStart)
            prCmpbDmxMMData->u4BufStart -= FEEDER_RESERVED_SIZE;

        prCmpbDmxMMData->u1Idx = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid;
        u4MoveRetryCnt = 0;

        while (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
        {
            LOG(0, "%s(%d): _SWDMX_MoveData fail(%d)\n", __FUNCTION__, __LINE__, u4MoveRetryCnt);
            u4MoveRetryCnt++;
            if (u4MoveRetryCnt > CMPB_DMX_MV_RETRY)
            {
                LOG(0, "%s(%d): _SWDMX_MoveData retry fail\n", __FUNCTION__, __LINE__);
                ASSERT(0);  // !!!
                *pu4ConsumeSize = 0;
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
    }
    else
    {
        ASSERT(0);
    }

//CMPB_PKT_END:
    *pu4ConsumeSize = u4PacketSize + CMPB_HDR_LEN;

    if (prSwdmxCmpbInfo->fgIsBlock == TRUE)
    {
        prSwdmxCmpbInfo->fgIsBlock = FALSE;
    }

    return eSWDMX_HDR_PRS_SUCCEED;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_CmpbHdrPrsWithoutIdx(
    UCHAR u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    INT32 i4PlaySpeed,
    UINT64 u8SeekPos,
    BOOL fgSpeedChange)
{
    BOOL fgRangeEnd = FALSE;

    UINT32 u4ReadSize = 0;
    UINT32 u4ConsumedSize = 0;
    UINT32 u4PrsState = eSWDMX_HDR_PRS_SUCCEED;

    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;

    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    FEEDER_BUF_INFO_T *prCmpbFeederInfo;
    DMX_MM_DATA_T *prCmpbDmxMMData;
    SWDMX_CMPB_VERIFY_NULL_RET(prSwdmxInst, eSWDMX_HDR_PRS_FAIL);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    prCmpbFeederInfo= &prSwdmxCmpbInfo->rCmpbFeederInfo;
    prCmpbDmxMMData = &prSwdmxCmpbInfo->rCmpbDmxMMData;

    if (prSwdmxCmpbInfo->fgFeederInvalid)
    {
        ASSERT(prSwdmxCmpbInfo->i4ReqReadTimes == 0);
        LOG(3, "Feeder is invalid, Cur Feeder ID = %ld, req times = %ld, Cur Dmx Pos = %lld\n",
            prSwdmxCmpbInfo->u4FeederReqID,
            prSwdmxCmpbInfo->i4ReqReadTimes,
            prSwdmxCmpbInfo->u8CurDmxFilePos);

        for ( ; prSwdmxCmpbInfo->i4ReqReadTimes > 0;)
        {
            if (_SWDMX_CmpbWaitFeeder(u1SrcId, &rFeederReqData))
                prSwdmxCmpbInfo->i4ReqReadTimes--;
        }

        _SWDMX_CmpbResetFeeder(u1SrcId);

        prSwdmxCmpbInfo->fgIsSeek = TRUE;
        prSwdmxCmpbInfo->u4LeftBytesInFeeder = 0;
        prSwdmxCmpbInfo->u8CurFilePos = prSwdmxCmpbInfo->u8CurDmxFilePos;
        prSwdmxCmpbInfo->fgFeederInvalid = FALSE;
        prSwdmxCmpbInfo->i4ReqReadTimes = 0;
        prSwdmxCmpbInfo->u4FeederReqID += 1;
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    // Check if demuxing is done
    if (prSwdmxCmpbInfo->fgGotEosIbc/* ||
        prSwdmxCmpbInfo->u8CurDmxFilePos >= prRangeList->prDmxRangeInfo->z_range_sz + prRangeList->prDmxRangeInfo->ui8_fileoffset*/)
    {
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
        if (prSwdmxCmpbInfo->fgEnVideo && !prSwdmxCmpbInfo->fgDeliverVEos)
        {
            if (prSwdmxCmpbInfo->fgVDecIsPause)
            {
                LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 3\n");
                _SWDMX_CmpbVideoWakeUp(u1SrcId);
                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
            }

            LOG(5, "Cur DmxMoviPos = %ld, range size = %ld, range offset = %ld\n",
                prSwdmxCmpbInfo->u8CurDmxFilePos,
                prRangeList->prDmxRangeInfo->z_range_sz,
                prRangeList->prDmxRangeInfo->ui8_fileoffset);
            LOG(2, "_SWDMX_CmpbHdrPrsWithoutIdx Deliver Video Eos\n");
            
            if (!_SWDMX_SendCmpbEos(u1SrcId, eSWDMX_MEDIA_VIDEO))
            {
                LOG(3, "Fail to send Video Eos\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
            prSwdmxCmpbInfo->fgDeliverVEos = TRUE;
        }

        if (prSwdmxCmpbInfo->fgEnAudio && !prSwdmxCmpbInfo->fgDeliverAEos)
        {
            LOG(2, "_SWDMX_CmpbHdrPrsWithoutIdx Deliver Audio Eos\n");
            if (!_SWDMX_SendCmpbEos(u1SrcId, eSWDMX_MEDIA_AUDIO))
            {
                LOG(3, "Fail to send Audio Eos\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
            prSwdmxCmpbInfo->fgDeliverAEos = TRUE;
            AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
        }

        return eSWDMX_HDR_PRS_SUCCEED;
    }

    // Request data for the first time
    if ((UINT32)prSwdmxCmpbInfo->u8CurFilePos == 0 &&
        (UINT32)(prSwdmxCmpbInfo->u8CurFilePos >> 32) == 0 &&
        prSwdmxCmpbInfo->i4ReqReadTimes == 0)
    {
#if 1  // megaa 20100618
        if (prSwdmxCmpbInfo->fgEnVDmx == FALSE && prSwdmxCmpbInfo->fgEnVideo)
        {
            _SWDMX_CmpbInitDmx(u1SrcId, eSWDMX_MEDIA_VIDEO, NULL, FALSE);
            prSwdmxCmpbInfo->fgEnVDmx = TRUE;
        }
        
        if (prSwdmxCmpbInfo->fgEnADmx == FALSE && prSwdmxCmpbInfo->fgEnAudio)
        {
            _SWDMX_CmpbInitDmx(u1SrcId, eSWDMX_MEDIA_AUDIO, NULL, FALSE);
            prSwdmxCmpbInfo->fgEnADmx = TRUE;
        }
#endif
        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, prCmpbFeederInfo) !=
          FEEDER_E_OK)
        {
            LOG(7, "Feeder input buffer error.\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

#if 1
        u4ReadSize = 1;
#else
        u4ReadSize = prRangeList->prDmxRangeInfo->z_range_sz;
        u4ReadSize = cmpb_min(u4ReadSize, CMPB_MIN_READ_SIZE);
#endif
        prSwdmxCmpbInfo->u4LastAlignOffset = 0;

        prSwdmxCmpbInfo->u8CurFilePos = prRangeList->prDmxRangeInfo->ui8_fileoffset;
        prSwdmxCmpbInfo->u8CurDmxFilePos = prSwdmxCmpbInfo->u8CurFilePos;

        if (prSwdmxCmpbInfo->u4VDecType == ENUM_SWDMX_VDEC_VC1)
        {
            prCmpbDmxMMData->u4BufStart = (UINT32)(prSwdmxCmpbInfo->u4SequenceHeader)&0xFFFFFFF0;
            prCmpbDmxMMData->u4BufEnd = ((UINT32)(prSwdmxCmpbInfo->u4SequenceHeader) + 0x10+188)&0xFFFFFFF0;
            prCmpbDmxMMData->u4StartAddr = prSwdmxCmpbInfo->u4SequenceHeader;//(UINT32)&_rSwdmxWmvInfo.au1SequenceHeader[0];
            prCmpbDmxMMData->u4FrameSize = (UINT32)(prSwdmxCmpbInfo->u4SequenceHeaderLength);
            prCmpbDmxMMData->u4Pts = 0;
            prCmpbDmxMMData->fgFrameHead  = TRUE;

            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData) != TRUE)
            {
                LOG(0, "Demuxer fails to move VC-1 sequence header.\n");
                ASSERT(0);
                return eSWDMX_HDR_PRS_FAIL;
            }
        }
        
        while (prSwdmxCmpbInfo->i4ReqReadTimes < SWDMX_DATA_Q_SIZE - 1)  // megaa 20100304: ToDo: check if the -1 is needed
        {
            if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                    u4ReadSize,
#if 1
                    0,
#else
                    prSwdmxCmpbInfo->u8CurFilePos,
#endif
                    &rFeederCond,
                    prSwdmxCmpbInfo->u4FeederReqID++) == FEEDER_E_OK)
            {
#ifdef CMPB_TIME_PROFILE
                HAL_GetTime(&_rSwdmxStart);
#endif
                prSwdmxCmpbInfo->i4ReqReadTimes++;
                prSwdmxCmpbInfo->u4Req++;
                LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                    prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);
            }
            else
            {
                ASSERT(0);
                break;
            }
        }

        if (!_SWDMX_CmpbGetDataFromFeeder(u1SrcId, &rFeederReqData))
        {
            //prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
            return eSWDMX_HDR_PRS_FAIL;
        }
        
#ifdef CMPB_TIME_PROFILE
        HAL_GetTime(&_rSwdmxEnd);
        HAL_GetDeltaTime(&_rSwdmxDif, &_rSwdmxStart, &_rSwdmxEnd);
        printf("Duration: %d\n", _rSwdmxDif.u4Seconds * 1000000 + _rSwdmxDif.u4Micros);
#endif
        prSwdmxCmpbInfo->i4ReqReadTimes--;
        prSwdmxCmpbInfo->u4LastAlignOffset = rFeederReqData.u4AlignOffset;
        prSwdmxCmpbInfo->u8CurFilePos += rFeederReqData.u4ReadSize - prSwdmxCmpbInfo->u4LastAlignOffset;
        prSwdmxCmpbInfo->u4CurDmxFeederRPtr = VIRTUAL(rFeederReqData.u4WriteAddr + prSwdmxCmpbInfo->u4LastAlignOffset);
        prSwdmxCmpbInfo->u4LeftBytesInFeeder += rFeederReqData.u4ReadSize;
        LOG(7, "_SWDMX_ReceiveFeederAck [%ld],  u4QrySize= %ld u8Offset= 0x%x \n\n", rFeederReqData.u4Id, rFeederReqData.u4ReadSize, rFeederReqData.u8FilePos);
    }

    if (prSwdmxCmpbInfo->fgGotEosIbc)
    {
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if (prSwdmxCmpbInfo->u4LeftBytesInFeeder == 0 && prSwdmxCmpbInfo->i4ReqReadTimes > 0)
    {
        if (prSwdmxCmpbInfo->fgSeekFlush == FALSE)
        {
            if (!_SWDMX_CmpbGetDataFromFeeder(u1SrcId, &rFeederReqData))
            {
                 //prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
                return eSWDMX_HDR_PRS_FAIL;
            }
#ifdef CMPB_TIME_PROFILE
            HAL_GetTime(&_rSwdmxEnd);
            HAL_GetDeltaTime(&_rSwdmxDif, &_rSwdmxStart, &_rSwdmxEnd);
            printf("Duration: %d\n", _rSwdmxDif.u4Seconds * 1000000 + _rSwdmxDif.u4Micros);
#endif
            prSwdmxCmpbInfo->i4ReqReadTimes--;
            prSwdmxCmpbInfo->u4LastAlignOffset = rFeederReqData.u4AlignOffset;
            prSwdmxCmpbInfo->u8CurFilePos += rFeederReqData.u4ReadSize - prSwdmxCmpbInfo->u4LastAlignOffset;
            prSwdmxCmpbInfo->u4CurDmxFeederRPtr = VIRTUAL(rFeederReqData.u4WriteAddr + prSwdmxCmpbInfo->u4LastAlignOffset);
            prSwdmxCmpbInfo->u4LeftBytesInFeeder += rFeederReqData.u4ReadSize;
            LOG(7, "_SWDMX_ReceiveFeederAck [%ld],  u4QrySize= %ld u8Offset= 0x%x \n\n", rFeederReqData.u4Id, rFeederReqData.u4ReadSize, rFeederReqData.u8FilePos);
        }
        else 
        {
            LOG(1,"%s %d fgSeekFlush is TRUE",__FUNCTION__,__LINE__);
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        //return eSWDMX_HDR_PRS_FAIL;
    }

    if (prSwdmxCmpbInfo->fgGotEosIbc)
    {
        // Update read ptr to feeder
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);  // megaa 20100304: tmp solution (0 to distinguish push mode)
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if ((prSwdmxCmpbInfo->u4LeftBytesInFeeder == 0)
        || (prSwdmxCmpbInfo->i4ReqReadTimes < SWDMX_DATA_Q_SIZE / 2)
        || prSwdmxCmpbInfo->fgIsBlock)
    {
#if 1
        u4ReadSize = 1;
#else
        if (prRangeList->prDmxRangeInfo->z_range_sz + prRangeList->prDmxRangeInfo->ui8_fileoffset > prSwdmxCmpbInfo->u8CurFilePos)
        {
            u4ReadSize = prRangeList->prDmxRangeInfo->z_range_sz + prRangeList->prDmxRangeInfo->ui8_fileoffset -
                                   prSwdmxCmpbInfo->u8CurFilePos;
        }
        u4ReadSize = cmpb_min(u4ReadSize, CMPB_MIN_READ_SIZE);

        if (u4ReadSize == 0)
        {
            fgRangeEnd = TRUE;
        }
#endif

#if defined(CC_DLNA_SUPPORT) && 0
        // DLNA
        if ((!prSwdmxCmpbInfo->fgFeederIsReady) ||
            (prSwdmxCmpbInfo->u4LeftBytesInFeeder + u4ReadSize < CMPB_BUFFER_SIZE))
#else
        if (prSwdmxCmpbInfo->u4LeftBytesInFeeder + u4ReadSize < CMPB_BUFFER_SIZE)
#endif
        {
            while (prSwdmxCmpbInfo->i4ReqReadTimes < SWDMX_DATA_Q_SIZE - 1 && prSwdmxCmpbInfo->fgSeekFlush == FALSE)  // megaa 20100304: ToDo: check if the -1 is needed
            {
                LOG(6, "Left bytes = %ld offset = %lld id = %ld.\n",
                        prSwdmxCmpbInfo->u4LeftBytesInFeeder,
                        prSwdmxCmpbInfo->u8CurFilePos,
                        prSwdmxCmpbInfo->u4FeederReqID);

                if (FeederSetRequest(prSwdmxInst->eFeederSrc,
                        u4ReadSize,
#if 1
                        0,
#else
                        prSwdmxCmpbInfo->u8CurFilePos,
#endif
                        &rFeederCond,
                        prSwdmxCmpbInfo->u4FeederReqID++) == FEEDER_E_OK)
                {
#ifdef CMPB_TIME_PROFILE
                    HAL_GetTime(&_rSwdmxStart);
#endif
                    prSwdmxCmpbInfo->u4LastAlignOffset = 0;
                    prSwdmxCmpbInfo->i4ReqReadTimes++;
                    prSwdmxCmpbInfo->u4Req++;
                    LOG(8, "%s(%d): %d, ReqId=%d,ReqTime=%d,req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                        prSwdmxCmpbInfo->u4FeederReqID,prSwdmxCmpbInfo->i4ReqReadTimes,prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);
                }
                else
                {
                    LOG(3, "Feeder Buffer Full\n");
                    ASSERT(0);
                    //prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
                    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, prCmpbFeederInfo) !=
                        FEEDER_E_OK)
                    {
                        LOG(1, "Feeder input buffer error.\n");
                        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                    }
                    LOG(3, "Cur Fed's write ptr = 0x%08x\n", prCmpbFeederInfo->u4WriteAddr);
                    return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                }
            }
            if (prSwdmxCmpbInfo->fgSeekFlush == TRUE)
            {
                LOG(1,"%s %d fgSeekFlush is TRUE",__FUNCTION__,__LINE__);
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }        
    }

#if defined(CC_DLNA_SUPPORT) && 0
    if (u4ReadSize != CMPB_MIN_READ_SIZE)
    {
        // DLNA
        FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_EOS, TRUE);
    }

    // DLNA
    if (!prSwdmxCmpbInfo->fgFeederIsReady && FeederIsReady(prSwdmxInst->eFeederSrc))
    /*if ((!prSwdmxCmpbInfo->fgFeederIsReady) &&
        ((prSwdmxCmpbInfo->u4LeftBytesInFeeder >= CMPB_PREBUFFER_SIZE) || (prSwdmxCmpbInfo->u4LeftBytesInFeeder >= prRangeList->prDmxRangeInfo->z_range_sz)))*/
    {
        prSwdmxCmpbInfo->fgFeederIsReady = TRUE;
    }
#endif

#ifdef SWDMX_TIME_PROFILE
    HAL_TIME_T rTimeRstS, rTimeRstE, rTimeRstDt;
    HAL_GetTime(&rTimeRstS);
#endif

#if defined(CC_DLNA_SUPPORT) && 0
    if ((prSwdmxCmpbInfo->u4LeftBytesInFeeder > 0) && prSwdmxCmpbInfo->fgFeederIsReady)
#else
    if (prSwdmxCmpbInfo->u4LeftBytesInFeeder)
#endif
    {
        if (prSwdmxCmpbInfo->fgSpeedChange && i4PlaySpeed == 1000)
        {
            prSwdmxCmpbInfo->fgSpeedChange = FALSE;
        }

        // Start to parse data
        u4PrsState = _SWDMX_PrsCmpbPacket(u1SrcId,
            (UCHAR*)(prSwdmxCmpbInfo->u4CurDmxFeederRPtr),
            prRangeList,
            prSwdmxCmpbInfo->u4LeftBytesInFeeder,
            &u4ConsumedSize,
            i4PlaySpeed,
            fgRangeEnd,
            FALSE);
    }

#ifdef SWDMX_TIME_PROFILE
    HAL_GetTime(&rTimeRstE);
    HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
    LOG(0, "Swdmx parsing and deliver time = (%ld) \n", rTimeRstDt.u4Micros);
#endif

    switch (u4PrsState)
    {
        case eSWDMX_HDR_PRS_FAIL:
            LOG(0, "No CMPB chunks was discovered!!!! No start code?\n");
            ASSERT(0);
            return eSWDMX_HDR_PRS_FAIL;

        default:
            break;
    }

    if (prSwdmxCmpbInfo->u4LeftBytesInFeeder >= u4ConsumedSize)
    {
        prSwdmxCmpbInfo->u8CurDmxFilePos += u4ConsumedSize;
        prSwdmxCmpbInfo->u4LeftBytesInFeeder -= u4ConsumedSize;
    }
    else
    {
        prSwdmxCmpbInfo->u4LeftBytesInFeeder = 0;
    }

    if (u4ConsumedSize != 0)
    {
        LOG(6, "u4ConsumedSize = %ld u4LeftBytesInFeeder = 0x%08x.\n",
            u4ConsumedSize, prSwdmxCmpbInfo->u4LeftBytesInFeeder);
    }

    if ((prSwdmxCmpbInfo->u4CurDmxFeederRPtr + u4ConsumedSize) >=
        VIRTUAL(prCmpbFeederInfo->u4EndAddr))
    {
        prSwdmxCmpbInfo->u4CurDmxFeederRPtr =  VIRTUAL(prCmpbFeederInfo->u4StartAddr) +
            ((prSwdmxCmpbInfo->u4CurDmxFeederRPtr + u4ConsumedSize) -VIRTUAL(prCmpbFeederInfo->u4EndAddr));
    }
    else
    {
        prSwdmxCmpbInfo->u4CurDmxFeederRPtr += u4ConsumedSize;
    }

    if (u4ConsumedSize != 0)
    {
        // Update read ptr to feeder
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);  // megaa 20100304: tmp solution (0 to distinguish push mode)
        prSwdmxCmpbInfo->u4Con++;
        LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
            prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);
    }

    LOG(9, "prSwdmxCmpbInfo->u4CurDmxMoviPos = %ld.\n", prSwdmxCmpbInfo->u8CurDmxFilePos);

    return eSWDMX_HDR_PRS_SUCCEED;
}


static BOOL _SWDMX_CmpbResetFeeder(UCHAR u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    FEEDER_BUF_INFO_T *prCmpbFeederInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    prCmpbFeederInfo= &prSwdmxCmpbInfo->rCmpbFeederInfo;

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, prCmpbFeederInfo) !=
        FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return FALSE;
    }

    LOG(5, "_SWDMX_CmpbResetFeeder Cur Fed's WPtr = 0x%08x\n", prCmpbFeederInfo->u4WriteAddr);
    prSwdmxCmpbInfo->u4CurDmxFeederRPtr = prCmpbFeederInfo->u4WriteAddr ;

    // Update read ptr to feeder
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxCmpbInfo->u4CurDmxFeederRPtr));
    return TRUE;
}


BOOL _SWDMX_CmpbSetStrmID(UCHAR u1SrcId, UINT8 u1HdrType, UINT32 u4StrmID, UINT32 u4StrmSubID)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    UNUSED(u4StrmSubID);
    
    switch(u1HdrType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxCmpbInfo->u4VStrmID = u4StrmID;
            break;
            
        case eSWDMX_STRM_TYPE_AUD:
            prSwdmxCmpbInfo->u4AStrmID = u4StrmID;
            break;
            
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxCmpbInfo->u4SubTitleID = u4StrmID;
            break;
            
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_CmpbEnableStrm(UCHAR u1SrcId, UINT8 u1StrmType, VOID* pfnCbFunc)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    LOG(0,"%s %d Instance(%u) enable type %u\n",__FUNCTION__,__LINE__,u1SrcId,u1StrmType);
    
    switch (u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
        {
            prSwdmxCmpbInfo->fgEnVideo = TRUE;
#if 0  // megaa 20100618: moved to first time of header parse to avoid MW sequence problem
            if (prSwdmxCmpbInfo->fgEnVDmx == FALSE)
            {
                _SWDMX_CmpbInitDmx(u1SrcId, eSWDMX_MEDIA_VIDEO, FALSE);
                prSwdmxCmpbInfo->fgEnVDmx = TRUE;
            }
#endif
            break;
        }
        case eSWDMX_STRM_TYPE_AUD:
        {
#if 0  // megaa 20100618: moved to first time of header parse to avoid MW sequence problem
            if (prSwdmxCmpbInfo->fgEnADmx == FALSE)
            {
                _SWDMX_CmpbInitDmx(u1SrcId, eSWDMX_MEDIA_AUDIO, FALSE);
                prSwdmxCmpbInfo->fgEnADmx = TRUE;
            }
#endif
           if ( ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && (!IS_NEW_AUDIO_CHG_FLOW(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))) &&
                (prSwdmxCmpbInfo->u4PrevAStrmID != 0xFFFFFFFF) && 
                 ( FALSE == prSwdmxCmpbInfo->fgWaitNextAudio && 0 == prSwdmxCmpbInfo->u4LastAudioNum) && 
                ( !IS_AP_WANT_TO_DO_SEEK(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type)))
            {
                prSwdmxCmpbInfo->fgChgATrack = TRUE;

                LOG(0, ">>>>>>>>>>>>..audio change\n");
                prSwdmxCmpbInfo->fgWaitNextAudio = TRUE;
                prSwdmxCmpbInfo->u4LastAudioNum++;

                LOG(0, "++, u4LastAudioNum = %u\n", prSwdmxCmpbInfo->u4LastAudioNum);

                DMX_MM_FlushBuffer(prSwdmxCmpbInfo->u1AudPid); 
            } 

            prSwdmxCmpbInfo->u4PrevAStrmID = prSwdmxCmpbInfo->u4AStrmID;
            prSwdmxCmpbInfo->fgEnAudio = TRUE;
            //prSwdmxCmpbInfo->u1TotalATrack = 1;  // tmp solution for MW not setting this

            if (prSwdmxCmpbInfo->eDRMType != eSWDMX_DRM_TYPE_NONE)
            {
                UCHAR ucAKeyId;

                for (ucAKeyId=0; ucAKeyId < prSwdmxCmpbInfo->u1AKeyCount; ucAKeyId++)
                {
                    if (prSwdmxCmpbInfo->u1AudioKeyTrackId[ucAKeyId] == prSwdmxCmpbInfo->u4AStrmID)
                    {
                        prSwdmxCmpbInfo->u1CurAKeyIndex = ucAKeyId;
                    }
                }
            }
            break;
        }
        case eSWDMX_STRM_TYPE_SUBTITLE:
        {
           if (prSwdmxCmpbInfo->fgEnSDmx == FALSE)
           {
               UNUSED(_SWDMX_CmpbInitDmx(u1SrcId, eSWDMX_MEDIA_SUBTITLE, pfnCbFunc, FALSE));
               prSwdmxCmpbInfo->fgEnSDmx = TRUE;
           }
           prSwdmxCmpbInfo->fgChgSubTrack = TRUE;
           prSwdmxCmpbInfo->fgEnSubTitle = TRUE;
           
           break;
        }
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_CmpbDisableStrm(UCHAR u1SrcId, UINT8 u1StrmType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    LOG(0,"%s %d Instance(%u) disable type %u \n",__FUNCTION__,__LINE__,u1SrcId,u1StrmType);
    
    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            if(prSwdmxCmpbInfo->fgVDecIsPause)
            {
                LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 4\n");
                _SWDMX_CmpbVideoWakeUp(u1SrcId);
                prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
            }
            prSwdmxCmpbInfo->fgEnVideo = FALSE;
            break;
            
        case eSWDMX_STRM_TYPE_AUD:
            prSwdmxCmpbInfo->fgEnAudio = FALSE;
            break;
            
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxCmpbInfo->fgEnSubTitle = FALSE;
            break;
            
        default:
            return FALSE;
    }

    return TRUE;
}

static BOOL _SWDMX_CmpbInitDmx(UCHAR u1SrcId, UINT8 u1HdrType, VOID* pfnCbFunc, BOOL fgEos)
{
    BOOL fgRet = FALSE;
    UINT32 u4Flag = 0;
    FBM_POOL_T* prFbmPool;
    DMX_MM_T rDmxMMInfo;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    x_memset(&rDmxMMInfo, 0, sizeof(DMX_MM_T));

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;

#if 1  // v2
        DMX_SetToDecoder(TRUE);
#endif
        if (!prSwdmxCmpbInfo->fgEnVDmx)
        {
#if 1  // v2
            UNUSED(prFbmPool);
            prSwdmxCmpbInfo->u1VidPid =
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

            u4Flag = (UINT32)(DMX_MM_FLAG_TYPE | DMX_MM_FLAG_INSTANCE_TAG |
                              DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF);
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
#else
            // Allocate video buffer from FBM
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
            if((prFbmPool == NULL) || (prFbmPool->u4Addr == 0))
            {
                LOG(1, "_SWDMX_SetVideoStreamID (prFbmPool == NULL) || (prFbmPool->u4Addr == NULL)\n");
                return FALSE;
            }

            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_NOTIFY |
              DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF);
#endif

            rDmxMMInfo.fgAllocBuf = FALSE;
#if 1  // v2
            _SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize);
            rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
            prSwdmxCmpbInfo->u4VFifoSa = VIRTUAL(rDmxMMInfo.u4BufAddr);
            prSwdmxCmpbInfo->u4VFifoEa = VIRTUAL(rDmxMMInfo.u4BufAddr) + rDmxMMInfo.u4BufSize;
#else
            rDmxMMInfo.u4BufAddr = VIRTUAL(prFbmPool->u4Addr);
            rDmxMMInfo.u4BufSize = prFbmPool->u4Size;
            prSwdmxCmpbInfo->u4VFifoSa = VIRTUAL(prFbmPool->u4Addr);
            prSwdmxCmpbInfo->u4VFifoEa = VIRTUAL(prFbmPool->u4Addr + prFbmPool->u4Size);
#endif
            u4Flag |= DMX_MM_FLAG_SEARCH_START_CODE;

            switch (prSwdmxCmpbInfo->u4VDecType)
            {
                case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
                case ENUM_SWDMX_VDEC_MPEG4:
                case ENUM_SWDMX_VDEC_H264:
                case ENUM_SWDMX_VDEC_MVC:
                case ENUM_SWDMX_VDEC_VC1:
                case ENUM_SWDMX_VDEC_AVS:
                case ENUM_SWDMX_VDEC_H265:
                    LOG(7, "%s SearchStartCode = TRUE\n", __FUNCTION__);
                    rDmxMMInfo.fgSearchStartCode = TRUE;
                    break;

                default:
                    LOG(7, "%s SearchStartCode = FALSE\n", __FUNCTION__);
                    rDmxMMInfo.fgSearchStartCode = FALSE;
                    break;
            }
            
            fgRet = DMX_MM_Set(prSwdmxCmpbInfo->u1VidPid, u4Flag, &rDmxMMInfo);
        }
        else if (fgEos)
        {
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
            fgRet = DMX_MM_Set(prSwdmxCmpbInfo->u1VidPid, u4Flag, &rDmxMMInfo);
        }

        if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1VidPid,DMX_DRM_MODE_BYPASS))
        {
            LOG(3, "%s(%d): DMX_MM_SetDRMMode fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
        
        LOG(0,"Video DMX_MM_SetDRMMode set OK..........\n");
    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
#if 0
        UINT32 au4BufStart[2], au4BufEnd[2];
#endif
        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_AUDIO;

#if 1  // v2
        DMX_SetToDecoder(TRUE);
#endif
        if (!prSwdmxCmpbInfo->fgEnADmx)
        {
#if 1  // v2
            prSwdmxCmpbInfo->u1AudPid =
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

            _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize);

            u4Flag = (UINT32)(DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID | DMX_MM_FLAG_INSTANCE_TAG
                              | DMX_MM_FLAG_BUF | DMX_MM_FLAG_SEARCH_START_CODE);

            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
            rDmxMMInfo.fgSearchStartCode = FALSE;
#else
            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_NOTIFY |
              DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF);
#ifdef AUD_OLDFIFO_INTF
            if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1],
                 &au4BufEnd[1]) != AUD_OK)
#else
            if (AUD_GetAudFifo(prSwdmxInst->u1AdecId, &au4BufStart[0], &au4BufEnd[0]) != AUD_OK)
#endif
            {
                LOG(1, "_SWDMX_SetAudioStreamID Can't get audio buffers!\n");
                return FALSE;
            }
#endif

            rDmxMMInfo.fgAllocBuf = FALSE;
#if 0
            rDmxMMInfo.u4BufAddr = au4BufStart[0];
            rDmxMMInfo.u4BufSize = au4BufEnd[0] - au4BufStart[0];
#endif
            fgRet = DMX_MM_Set(prSwdmxCmpbInfo->u1AudPid, u4Flag, &rDmxMMInfo);
            prSwdmxCmpbInfo->u4AFifoSa = VIRTUAL(rDmxMMInfo.u4BufAddr);
            prSwdmxCmpbInfo->u4AFifoSz = rDmxMMInfo.u4BufSize;
        }
        else if (fgEos)
        {
            // necessary?
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
            fgRet = DMX_MM_Set(prSwdmxCmpbInfo->u1AudPid, u4Flag, &rDmxMMInfo);
        }

        if(!DMX_MM_SetDRMMode(prSwdmxCmpbInfo->u1AudPid,DMX_DRM_MODE_BYPASS))
        {
            LOG(3, "%s(%d): DMX_MM_SetDRMMode fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
        LOG(0,"Audio DMX_MM_SetDRMMode set OK..........\n");
    }
    else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_PES;

        if (!prSwdmxCmpbInfo->fgEnSDmx)
        {
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid = 
                DMX_MUL_GetSubtitlePidx(prSwdmxInst->u1DmxId);

            if (pfnCbFunc)
            {
                u4Flag = (DMX_MM_FLAG_TYPE | 
                    DMX_MM_FLAG_SEARCH_START_CODE | 
                    DMX_MM_FLAG_BUF | 
                  DMX_MM_FLAG_VALID);
            
                u4Flag |= DMX_MM_FLAG_NOTIFY;
                rDmxMMInfo.pfnCallback = (PFN_DMX_NOTIFY)pfnCbFunc;
                
                rDmxMMInfo.fgSearchStartCode = FALSE;
                rDmxMMInfo.fgAllocBuf = TRUE;
                rDmxMMInfo.u4BufSize = CMPB_SUBTITLE_BUFSIZE;

                fgRet = DMX_MM_Set( prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid , u4Flag, &rDmxMMInfo);
            }
        }
    }

    return fgRet;
}


static BOOL _SWDMX_SendCmpbEos(UCHAR u1SrcId, UINT8 u1HdrType)
{
    BOOL fgRet;
    UINT16 u2MaxQNum, u2CurQNum;
    //UINT32 u4DummyData[4];
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    DMX_MM_DATA_T *prCmpbDmxMMData;
    FEEDER_BUF_INFO_T *prCmpbFeederInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    prCmpbDmxMMData = &prSwdmxCmpbInfo->rCmpbDmxMMData;
    prCmpbFeederInfo= &prSwdmxCmpbInfo->rCmpbFeederInfo;

    LOG(5, "SWDMX Send CMPB Eos Hdr Type = %d\n", u1HdrType);

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, prCmpbFeederInfo) !=
      FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return FALSE;
    }

    x_memset(prCmpbDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        LOG(7, "u2CurQNum = %ld, u2MaxQNum = %ld .\n", u2CurQNum, u2MaxQNum);

        //if (u2CurQNum > (prSwdmxCmpbInfo->u4VidFps/2))
        if (u2CurQNum > (u2MaxQNum - 10))
        {
            prSwdmxInst->fgFifoFull = TRUE;
            x_thread_delay(1);
            return FALSE;
        }

        prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1VidPid;

    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        prCmpbDmxMMData->u1Idx = prSwdmxCmpbInfo->u1AudPid;
    }

    if (_SWDMX_CmpbInitDmx(u1SrcId, u1HdrType, NULL, TRUE) == FALSE)
    {
        return FALSE;
    }

    prCmpbDmxMMData->u4BufStart = prCmpbFeederInfo->u4StartAddr;
    prCmpbDmxMMData->u4BufEnd = prCmpbFeederInfo->u4EndAddr;
    prCmpbDmxMMData->u4StartAddr = prCmpbFeederInfo->u4StartAddr;
    prCmpbDmxMMData->u4FrameSize = 4;
    prCmpbDmxMMData->fgEOS = TRUE;
    prCmpbDmxMMData->fgIgnoreData = TRUE;

#if 1
    fgRet = _SWDMX_MoveData(prSwdmxInst->u1SwdmxId, prCmpbDmxMMData);
#else
    fgRet = DMX_MM_MoveData(u1HdrType, prCmpbDmxMMData, 500);
#endif
    ASSERT(fgRet);

    prCmpbDmxMMData->fgEOS = FALSE;

    return fgRet;
}

/**
*/
BOOL _SWDMX_CmpbStop(UCHAR u1SrcId)
{
    DMX_MM_T rPid;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    rPid.fgEnable = FALSE;

    if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderFdrPtr != 0)
    {
        LOG(0,"===Free the first play set codec info :===\n");
   
        BSP_FreeAlignedDmaMemory(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderFdrPtr);
        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderFdrPtr = 0;
    }

    if (prSwdmxCmpbInfo->fgEnVDmx)
    {
#if 1  // v2
        VERIFY(DMX_MM_Set(prSwdmxCmpbInfo->u1VidPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxCmpbInfo->u1VidPid);
        DMX_MUL_FreePidx(prSwdmxCmpbInfo->u1VidPid);
#endif
        prSwdmxCmpbInfo->fgEnVDmx = FALSE;
    }

    if (prSwdmxCmpbInfo->fgEnADmx)
    {
#if 1  // v2
        VERIFY(DMX_MM_Set(prSwdmxCmpbInfo->u1AudPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxCmpbInfo->u1AudPid);
        DMX_MUL_FreePidx(prSwdmxCmpbInfo->u1AudPid);
#endif
        prSwdmxCmpbInfo->fgEnADmx = FALSE;
    }

    if (prSwdmxCmpbInfo->fgEnSDmx)
    {
        VERIFY(DMX_MM_Set( prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid , DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free( prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid );
        DMX_MUL_FreePidx( prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1SubPid );

        prSwdmxCmpbInfo->fgEnSDmx = FALSE;
    }

    x_memset(&prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo, 0, sizeof(SWDMX_CMPB_INFO_T));

    return TRUE;
}


static BOOL _SWDMX_CmpbGetDataFromFeeder(UCHAR u1SrcId, FEEDER_REQ_DATA_T *prFeederData)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    FEEDER_BUF_INFO_T *prCmpbFeederInfo;
    FEEDER_TRACK_MARLIN_CRYPT_INFO_T* pTrackInfo;
    UINT32 i = 0;
    
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    prCmpbFeederInfo= &prSwdmxCmpbInfo->rCmpbFeederInfo;

    do
    {
        prFeederData->eFeederIBC = FEEDER_IBC_NONE;
        
        if (!_SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)prFeederData))
        {
            LOG(1, "_SWDMX_CmpbGetDataFromFeeder got Feeder ACK failed!!!\n");
            prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
            return FALSE;
        }

        if (prFeederData->eDataType == FEEDER_SOURCE_INVALID)
        {
            LOG(1, "_SWDMX_CmpbGetDataFromFeeder timeout\n");
            prSwdmxCmpbInfo->fgFeederTimeout = TRUE;
            return FALSE;
        }

        if ((prSwdmxCmpbInfo->i4ReqReadTimes == 0) || 
            (prFeederData->u4Id != (prSwdmxCmpbInfo->u4FeederReqID - prSwdmxCmpbInfo->i4ReqReadTimes)))
        {
            LOG(0, "Feeder ID mismatch!!! feeder id = %ld, ReqId = %d ,ReqReadTimes = %d\n", 
                prFeederData->u4Id,prSwdmxCmpbInfo->u4FeederReqID,prSwdmxCmpbInfo->i4ReqReadTimes);
            
            //prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
            return FALSE;
        }
        
        if (prSwdmxCmpbInfo->prDmxRangeInfo != NULL && \
            prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DASH)  // for DASH
        {   
            if (prFeederData->eFeederIBC == FEEDER_IBC_PARAM_STRM_INFO)
            {
                SWDMX_BUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T rTmpIBCStrmInfo;

                 x_memcpy((VOID*)&rTmpIBCStrmInfo,
                                (VOID*)prFeederData->u4WriteAddr,
                                sizeof(SWDMX_BUF_AGT_CTRL_IBC_PARAM_STRM_INFO_T));

                LOG(0, "_SWDMX_CmpbGetDataFromFeeder FEEDER_IBC_PARAM_STRM_INFO %d %d\n", rTmpIBCStrmInfo.ui4_aud_strm_id, rTmpIBCStrmInfo.e_aud_enc);

                rTmpIBCStrmInfo.e_aud_enc = (UINT8)_SWDMX_CmpbAudCodecConvert(rTmpIBCStrmInfo.e_aud_enc);        

                if (rTmpIBCStrmInfo.ui4_aud_strm_id != prSwdmxCmpbInfo->u4AStrmID)
                {
                    prSwdmxCmpbInfo->u4AStrmID = rTmpIBCStrmInfo.ui4_aud_strm_id;
                    prSwdmxCmpbInfo->u4PrevAStrmID = prSwdmxCmpbInfo->u4AStrmID;
                }
                
                if (prSwdmxInst->eAudioType != rTmpIBCStrmInfo.e_aud_enc)
                {
                    LOG(0, "Audio codec change begin!!\n");
                    prSwdmxInst->eUpdAudioType = (ENUM_SWDMX_ADEC_T)rTmpIBCStrmInfo.e_aud_enc;    
                    prSwdmxCmpbInfo->fgWaitAudCodecChgDone = TRUE;
                    prSwdmxCmpbInfo->fgAudCodecChg = TRUE;
                    #ifdef LINUX_TURNKEY_SOLUTION
                    AUD_SetAudCodecChange(TRUE, prSwdmxInst->eUpdAudioType);
                    #endif
                }

                prSwdmxCmpbInfo->u4Ack++;
                if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
                {
                    prSwdmxCmpbInfo->i4ReqReadTimes--;
                }
                
                // Update read ptr to feeder
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
                prSwdmxCmpbInfo->u4Con++;
                LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                    prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

                return FALSE;

            }
            else if(prFeederData->eFeederIBC == FEEDER_IBC_DASH_SET_INFO)
            {
                LOG(0, "FEEDER_IBC_DASH_SET_INFO\n");
                
                prSwdmxCmpbInfo->fgBandWidthChg = TRUE;
                
                prSwdmxCmpbInfo->u4Ack++;

                if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
                {
                    prSwdmxCmpbInfo->i4ReqReadTimes--;
                }
                
                // Update read ptr to feeder
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
                prSwdmxCmpbInfo->u4Con++;
                LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                    prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

                return FALSE;
            }
            else if(prFeederData->eFeederIBC == FEEDER_IBC_SEQHEADER)
            {     
                UINT32 u4Index = 0;
                
                LOG(6, "_SWDMX_CmpbGetDataFromFeeder, FEEDER_IBC_SEQHEADER, u1DmxId = %u\n", prSwdmxInst->u1DmxId);
                
                if (prFeederData->eFeederStrmType == FEEDER_CTRL_STREAM_TYPE_VIDEO)
                {
                    if (prFeederData->u.t_vid_seqheader_info.b_is_raw &&
                        prFeederData->u.t_vid_seqheader_info.u.t_seq_raw.pc_buff != NULL)
                    {
                        // clear old seq header
                        if (prFeederData->u.t_vid_seqheader_info.b_is_second_raw)
                        {
                            prSwdmxCmpbInfo->u4CurVidHeaderIndex = 0xFFFFFFFF;
                            x_memset(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgVidHdrIsUsed, 0, sizeof(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgVidHdrIsUsed));
                            x_memset(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgeFrameRateIsUsed, 0, sizeof(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgeFrameRateIsUsed));
                        }
                        
                        u4Index = ((UINT32)prFeederData->u.t_vid_seqheader_info.u.t_seq_raw.pc_buff - (UINT32)prSwdmxInst->arSeqHeaderInfo) / 512;

                        if ( u4Index < SWDMX_DATA_Q_SIZE)
                        {                            
                            if (_SWDMX_CmpbSetDecoderInfo( u1SrcId, eSWDMX_SET_VIDSEQHEADER, 
                                (UINT32)(prFeederData->u.t_vid_seqheader_info.u.t_seq_raw.pc_buff), 
                                prFeederData->u.t_vid_seqheader_info.u.t_seq_raw.z_buff_size,
                                0))
                            {
                                LOG(3, "_SWDMX_CmpbGetDataFromFeeder _SWDMX_BeforeDeliverUseIdx\n");
                                //_SWDMX_BeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);
                            }

                            /*keep Frame rate, should align with seq header*/
                            {
                               UINT8 i=0;
                               while (i < CMPB_MAX_SEQ_HEADER)
                               {
                                    if(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgeFrameRateIsUsed[i]==FALSE)
                                    {
                                        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eFrameRate[i]= prFeederData->u.t_vid_seqheader_info.u.t_seq_raw.e_frame_rate;  
                                        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.fgeFrameRateIsUsed[i]=TRUE;
                                        break;
                                    }
                                    i++;
                                }
                             }

                            prSwdmxInst->afgSeqHeaderInfo[u4Index] = FALSE;
                        }
                        else
                        {
                            LOG(0, "afgSeqHeaderInfo index error!!!!\n");
                        }
                    }   
                }
                else if (prFeederData->eFeederStrmType == FEEDER_CTRL_STREAM_TYPE_AUDIO)
                {
                    // clear old audio seq header
                    if (prFeederData->u.t_aud_seqheader_info.b_is_audio_change)
                    {
                        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4CurAudHeaderIndex = 0;
                        x_memset(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader, 0, sizeof(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader));
                    }

                    //set new audio seq header
                    i = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4CurAudHeaderIndex;
                    if (i < CMPB_MAX_SEQ_HEADER)
                    {
                        x_memcpy(&prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.rAudioSeqHeader[i], &prFeederData->u.t_aud_seqheader_info, sizeof(FEEDER_CTRL_IBC_AUDIO_SEQHEADER_INFO));
                        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4CurAudHeaderIndex++;
                    }
                    else
                    {   
                        LOG(0, "Set audio seq header error: index > CMPB_MAX_SEQ_HEADER\n");
                    }
                }
                else
                {
                    LOG(0, "afgSeqHeaderInfo eFeederStrmType error!!!!\n");
                }  
                
                prSwdmxCmpbInfo->u4Ack++;
                if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
                {
                    prSwdmxCmpbInfo->i4ReqReadTimes--;
                }
                
                // Update read ptr to feeder
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
                prSwdmxCmpbInfo->u4Con++;
                LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                    prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

                return FALSE;
            }
            else if(prFeederData->eFeederIBC == FEEDER_IBC_PDCF)
            {     
                UINT32 u4Index = 0;
                
                LOG(6, "_SWDMX_CmpbGetDataFromFeeder, FEEDER_IBC_MARLIN, u1DmxId = %u\n", prSwdmxInst->u1DmxId);

                if (prFeederData->pt_pdcf_crypt != NULL)
                {
                    // clear flag
                    u4Index = ((UINT32)prFeederData->pt_pdcf_crypt - (UINT32)prSwdmxInst->arPDCFInfo) / sizeof(FEEDER_PDCF_CRYPT_INFO_T);
                    if ( u4Index < SWDMX_DATA_Q_SIZE)
                    {
                        prSwdmxInst->afgPDCFInfo[u4Index] = FALSE;
                    }
                    else
                    {
                        LOG(0, "afgPDCFInfo index error!!!!\n");
                    }

                    // set marlin info
                    prSwdmxCmpbInfo->eDRMType = eSWDMX_DRM_TYPE_PDCF;
                    prSwdmxCmpbInfo->eCryptMode = eSWDMX_CRYPT_MODE_AES_CTR;
                    for ( i = 0; i < prFeederData->pt_pdcf_crypt->ui1_valid_cnt; i++ )
                    {
                        pTrackInfo = &(prFeederData->pt_pdcf_crypt->t_track_drm[i]);

                        if (pTrackInfo)
                        {
                            if ( FEEDER_ST_VIDEO == pTrackInfo->e_strm_type )
                            {
                                x_memcpy(prSwdmxCmpbInfo->u1VideoDrmKey, (void*)pTrackInfo->aui1_key, 16);
                                LOG(3, "Get DRM type %d mode %dn",prSwdmxCmpbInfo->eDRMType, prSwdmxCmpbInfo->eCryptMode);
                            }
                            else if ( FEEDER_ST_AUDIO == pTrackInfo->e_strm_type )
                            {
                #if 0
                                if (prSwdmxCmpbInfo->u1AKeyCount < CMPB_MAX_AUDIO_TRACK)
                                {
                                    UCHAR ucTrackId;
                                    BOOL bUpdateKey = FALSE;
                                    
                                    for (ucTrackId=0; ucTrackId < prSwdmxCmpbInfo->u1AKeyCount; ucTrackId++)
                                    {
                                        if (prSwdmxCmpbInfo->u1AudioKeyTrackId[ucTrackId] == pTrackInfo->ui4_content_id)
                                        {
                                            x_memcpy(prSwdmxCmpbInfo->u1AudioDrmKey[ucTrackId], (void*)pTrackInfo->aui1_key, 16);
                                            bUpdateKey = TRUE;
                                            break;
                                        }
                                    }
                                    if (!bUpdateKey)
                                    {
                                        x_memcpy(prSwdmxCmpbInfo->u1AudioDrmKey[ucTrackId], (void*)pTrackInfo->aui1_key, 16);
                                        prSwdmxCmpbInfo->u1AudioKeyTrackId[prSwdmxCmpbInfo->u1AKeyCount] = pTrackInfo->ui4_content_id;
                                        prSwdmxCmpbInfo->u1AKeyCount++;
                                    }
                                    LOG(3, "Get DRM type %d mode %d TrackID %d\n",prSwdmxCmpbInfo->eDRMType, prSwdmxCmpbInfo->eCryptMode, pTrackInfo->ui4_content_id);
                                }
                                else
                                {
                                    ASSERT(0);
                                    LOG(0, "Audio key count exceeds maximum track count(%d)!\n", CMPB_MAX_AUDIO_TRACK);
                                }
                    #endif
                                LOG(0,"Get the audio key!\n");
                                x_memcpy(prSwdmxCmpbInfo->u1AudioDrmKey[0], (void*)pTrackInfo->aui1_key, 16);
                                
                            }
                        }
                    }
                }    

                prSwdmxCmpbInfo->u4Ack++;
                if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
                {
                    prSwdmxCmpbInfo->i4ReqReadTimes--;
                }
                
                // Update read ptr to feeder
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
                prSwdmxCmpbInfo->u4Con++;
                LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                    prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

                return FALSE;
            }
            else if(prFeederData->eFeederIBC == FEEDER_IBC_CENC)
            {
                UINT32 u4Index = 0;
                
                // clear flag
                u4Index = ((UINT32)prFeederData->pt_aes_crt_crypt - (UINT32)prSwdmxInst->arAesCrtCrypt) / sizeof(FEEDER_AES_CTR_ENCRYPT);
                if ( u4Index < SWDMX_DATA_Q_SIZE)
                {
                    prSwdmxInst->afgAesCrtCrypt[u4Index] = FALSE;
                }
                else
                {
                    LOG(0, "afgPDCFInfo index error!!!!\n");
                }

                x_memcpy(&prSwdmxCmpbInfo->rAesCrtCrypt, (void*)prFeederData->pt_aes_crt_crypt, sizeof(FEEDER_AES_CTR_ENCRYPT));
                prSwdmxCmpbInfo->fgAesCrtCrypt = TRUE;

                prSwdmxCmpbInfo->u4Ack++;
                if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
                {
                    prSwdmxCmpbInfo->i4ReqReadTimes--;
                }
                
                // Update read ptr to feeder
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
                prSwdmxCmpbInfo->u4Con++;
                LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                    prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

                //return FALSE;
            }
        }

        if(prFeederData->eFeederIBC == FEEDER_IBC_PUSH_CHANGE_AUDIO)
        {
            LOG(0, "Audio change, got new audio\n");
            if (prSwdmxCmpbInfo->u4LastAudioNum > 0)
            {
                prSwdmxCmpbInfo->u4LastAudioNum--;
                LOG(0, "Audio change, u4LastAudioNum--\n");
            }

            prSwdmxCmpbInfo->u4Ack++;
            if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
            {
                prSwdmxCmpbInfo->i4ReqReadTimes--;
            }
            
            // Update read ptr to feeder
            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
            prSwdmxCmpbInfo->u4Con++;
            LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);
            return FALSE;
        }

        if(prFeederData->eFeederIBC == FEEDER_IBC_CMPB_MTK)
        {
            UINT32 u4Index = 0;
            
            LOG(9, "_SWDMX_CmpbGetDataFromFeeder, FEEDER_IBC_CMPB_MTK, u1DmxId = %u\n", prSwdmxInst->u1DmxId);
            
            if (prFeederData->prMtkEncryptInfo != NULL)
            {
                prSwdmxCmpbInfo->fgIsEncrypt = TRUE;
                x_memset((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, 0, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));
                x_memcpy((VOID *)&prSwdmxCmpbInfo->rMtkEncryptInfo, (VOID *)prFeederData->prMtkEncryptInfo, sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T));

                u4Index = ((UINT32)prFeederData->prMtkEncryptInfo - (UINT32)prSwdmxInst->arMtkEncryptInfo) / sizeof(FEEDER_CTRL_MTK_ENCRYPT_INFO_T);
                if ( u4Index < SWDMX_DATA_Q_SIZE)
                {
                    prSwdmxInst->afgMtkEncryptInfo[u4Index] = FALSE;
                }
                else
                {
                    LOG(0, "afgMtkEncryptInfo index error!!!!\n");
                }
            }

            prSwdmxCmpbInfo->u4Ack++;
            if ( prSwdmxCmpbInfo->i4ReqReadTimes > 0)
            {
                prSwdmxCmpbInfo->i4ReqReadTimes--;
            }
            
            // Update read ptr to feeder
            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 0);
            prSwdmxCmpbInfo->u4Con++;
            LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
                prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

            //return FALSE;
        }
    }while(prFeederData->eFeederIBC == FEEDER_IBC_CMPB_MTK || prFeederData->eFeederIBC == FEEDER_IBC_CENC);
    
    if (prFeederData->fgEof)
    {
        prSwdmxCmpbInfo->fgGotEosIbc = TRUE;
    }

    if((prFeederData->u4WriteAddr < prCmpbFeederInfo->u4StartAddr) || 
       (prFeederData->u4WriteAddr >= prCmpbFeederInfo->u4EndAddr))
    {
        LOG(0,"prFeederData->u4WriteAddr = %x\n",prFeederData->u4WriteAddr);
        prSwdmxCmpbInfo->fgFeederInvalid = TRUE;
        return FALSE;
    }

    prSwdmxCmpbInfo->fgFeederTimeout = FALSE;
    prSwdmxCmpbInfo->u4Ack++;
    LOG(8, "%s(%d): %d req,ack,con=%d,%d,%d\n", __FUNCTION__, __LINE__, u1SrcId,
        prSwdmxCmpbInfo->u4Req, prSwdmxCmpbInfo->u4Ack, prSwdmxCmpbInfo->u4Con);

    return TRUE;
}


// tmp solution
static BOOL _SWDMX_CmpbWaitFeeder(UCHAR u1SrcId, FEEDER_REQ_DATA_T *prFeederData)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);

    while (1)
    {
        if (!_SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)prFeederData))
        {
            LOG(3, "_SWDMX_CmpbWaitFeeder got Feeder ACK failed!!!\n");
            ASSERT(0);
            return FALSE;
        }
        if (prFeederData->eDataType != FEEDER_SOURCE_INVALID)
        {
            break;
        }
    }

    return TRUE;
}


BOOL _SWDMX_CmpbAudioCb(UCHAR u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    if(eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        if(prSwdmxCmpbInfo->fgEnVideo)
        {
            if(prSwdmxCmpbInfo->fgAudTrigVidDec && prSwdmxCmpbInfo->fgAudHasTriggeredVidDec == FALSE)
            {
                if ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && IS_NO_AVSYNC_AP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))
                {
                    _SWDMX_CmpbVideoWakeUp(u1SrcId);
                    LOG(1, "AudCb before feeding data!\n");
                    AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);  // for EOS, aud will not notify EOS if Aout Disable
                }
                else
                {
                    LOG(1,"_SWDMX_CmpbAudioCb, VDP_TriggerAudReceive at %u, Id: %u(%d)\n",prSwdmxCmpbInfo->u4TriggerAudPts,prSwdmxInst->u1B2rId,prSwdmxInst->u1B2rId);                    
                    VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, prSwdmxCmpbInfo->u4TriggerAudPts);
                     
                    if (prSwdmxCmpbInfo->fgBufferToStartup &&
                        ((prSwdmxCmpbInfo->prDmxRangeInfo != NULL) && (!IS_NEED_BUFFER_TO_STARTUP(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))))
                    {
                        if (prSwdmxCmpbInfo->fgVDecIsPause)
                        {
                            LOG(3, "++++++++++++++++++++++++++++++++++++++ wake up 5\n");
                            _SWDMX_CmpbVideoWakeUp(u1SrcId);
                            prSwdmxCmpbInfo->fgVDecIsPause = FALSE;
                        }
                    }
                }
                
                //prSwdmxCmpbInfo->fgAudTrigVidDec = FALSE;
                prSwdmxCmpbInfo->fgAudHasTriggeredVidDec = TRUE;
            }
        }
        else
        {
            STC_StartStc(prSwdmxInst->u1StcId);
            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
        }
    }
    else
    {
        LOG(7, "AudCb Type Unknow %d\n", (UINT32)eAudioNotifyType);
    }
    return TRUE;
}


static VOID _SWDMX_CmpbVideoWakeUp(UCHAR u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    LOG(5, "_SWDMX_CmpbVideoWakeUp\n");
    VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);

    switch (prSwdmxCmpbInfo->u4VDecType)
    {
        default:
            LOG(0, "_SWDMX_CmpbVideoWakeUp Unknown Video Type (%d)\n",
                prSwdmxCmpbInfo->u4VDecType);
            ASSERT(0);
            break;
        case ENUM_SWDMX_VDEC_H265:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H265);
            break;
        case ENUM_SWDMX_VDEC_H264:
            LOG(5, "it is H264 !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H264);
            break;
        case ENUM_SWDMX_VDEC_MJPEG:
            LOG(5, "it is MJPEG !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MJPEG);
            break;
        case ENUM_SWDMX_VDEC_RAW_NV12:
            LOG(5, "it is NV12!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_RAW);
            break;
        case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
            LOG(5, "it is MPEG1/2 !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MPV);
            break;
        case ENUM_SWDMX_VDEC_MPEG4:
            LOG(5, "it is MPEG4 !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MP4);
            break;
        case ENUM_SWDMX_VDEC_H263:
            LOG(5, "it is H263 !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MP4);
            break;
        case ENUM_SWDMX_VDEC_VP6:
            LOG(5, "it is VP6 !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_VP6);
            break;
        case ENUM_SWDMX_VDEC_VC1:
        case ENUM_SWDMX_VDEC_WMV9:      
            LOG(5, "it is VC1 !!!!!!!!!!!!!!!!!!!!\n");
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_WMV);
            break;
    }
}

static SWDMX_HDR_PRS_STATE_T  _SWDMX_H264HeaderHandle(SWDMX_INFO_T *prSwdmxInst, UCHAR *pucHdrBuf, UCHAR *pucFdrBuf, UINT8 u1HdrType) 
{
    UINT8 u1NumOfPS = 0;
    UINT8 u1PSNum = 0;
    UINT8 i = 0;
    UINT16 u2PSLength = 0;
    DMX_MM_DATA_T rDmxMMData;
    
    if(pucHdrBuf == NULL || pucFdrBuf == NULL || prSwdmxInst == NULL)
    {
        LOG(0,"header pointer is NULL\n");
        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
    }
     
    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
     
    pucHdrBuf += 4; //skip unused info
    prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1StartCodeLenMinus1 = (*pucHdrBuf++) & 0x3;
    for(u1PSNum = 0; u1PSNum < 2; u1PSNum++)    // 0 for SPS, 1 for PPS
    {
        if(u1PSNum == 0)
        {
            u1NumOfPS = (*pucHdrBuf++) & 0x1f;
        }
        else
        {
            u1NumOfPS = *pucHdrBuf++;
        }
        LOG(3,"==The u1NumOfPS is %d==\n",(int)u1NumOfPS);
        for (i = 0; i < u1NumOfPS; i++)
        {
            u2PSLength = (UINT16)(*pucHdrBuf++);
            u2PSLength <<= 8;
            u2PSLength += (UINT16)(*pucHdrBuf++);

            //handle ring buffer
            //if(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) <= (((UINT32)pucFdrBuf + (UINT32)u2PSLength) + 4))
            //{
            //    pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            //}
            //set move data parameter
            rDmxMMData.u4BufStart = VIRTUAL((UINT32)pucFdrBuf);
            rDmxMMData.u4BufEnd = VIRTUAL((UINT32)pucFdrBuf + CMPB_SEQHEADER_FEEDER_SIZE);
            rDmxMMData.u4StartAddr = (UINT32)pucFdrBuf;
            rDmxMMData.u4FrameSize = (UINT32)u2PSLength + 4;    // 4 for start code
            rDmxMMData.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1VidPid;
            LOG(3,"==The u4BufStart is 0x%x,the u4BufEnd is 0x%x,The u4StartAddr is 0x%x,The u4FrameSize is %d==\n",
                (int)rDmxMMData.u4BufStart,(int)rDmxMMData.u4BufEnd,(int)rDmxMMData.u4StartAddr,(int)rDmxMMData.u4FrameSize);
            //insert start code
            *pucFdrBuf = 0;
            *(pucFdrBuf + 1) = 0;
            *(pucFdrBuf + 2) = 0;
            *(pucFdrBuf + 3) = 1;      
            //copy SPS or PPS
            x_memcpy((VOID*)(pucFdrBuf + 4), (VOID*)pucHdrBuf, u2PSLength);
            //pucFdrBuf += u2PSLength;
            pucHdrBuf += u2PSLength;
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
            HalFlushInvalidateDCacheMultipleLine((UINT32)pucFdrBuf, 
              (UINT32)(CMPB_SEQHEADER_FEEDER_SIZE));
#else
            HalFlushInvalidateDCache();
#endif
            if(DMX_MM_GetDRMMode(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1VidPid)!=DMX_DRM_MODE_BYPASS)
            {           
                if(!DMX_MM_SetDRMMode(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u1VidPid,DMX_DRM_MODE_BYPASS))
                {              
                    LOG(0,"SEQ HEADER:DMX_MM_SetDRMMode set error..........\n");
                    ASSERT(0);  // !!!
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;            
                }       
            }
            
            //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
            {
                LOG(1, "Demuxer fails to move sequence header.\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
    }
    return eSWDMX_HDR_PRS_SUCCEED;
} 

static SWDMX_HDR_PRS_STATE_T  _SWDMX_BeforeDeliverUseIdx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1HdrType)
{
    UCHAR  *pucHdrBuf = NULL;
    UCHAR  *pucFdrBuf = NULL;
    UINT32 u4CurVidHeaderIndex = 0;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }       

    u4CurVidHeaderIndex = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4CurVidHeaderIndex;

    if (prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VDecType == ENUM_SWDMX_VDEC_H264)
    {  
        LOG(3, "==The u4CurVidHeaderIndex is %u==\n", u4CurVidHeaderIndex);
        
        if(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr[u4CurVidHeaderIndex] != (UINT32)NULL)
        {
            pucHdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderPtr[u4CurVidHeaderIndex]);
            pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4VidHeaderFdrPtr);
            LOG(3, "==The pucFdrBuf is 0x%x==\n", (int)pucFdrBuf);
            
            if(_SWDMX_H264HeaderHandle(prSwdmxInst, pucHdrBuf, pucFdrBuf,u1HdrType)!= eSWDMX_HDR_PRS_SUCCEED)
            {
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
        else
        {
            LOG(0, "==Set H264 Seq header error, no index %u header!!!\n", u4CurVidHeaderIndex);
        }
                
        prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eFrameRate[u4CurVidHeaderIndex];

        if(prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.ePrevFrameRate == FEEDER_CTRL_VSH_SRC_FRAME_RATE_UNKNOWN)
        {
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.ePrevFrameRate = prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate;
        }
                
        LOG(1,"SampleRate Change PTS: 0x%08x, Prev SampleRate: %d  Current SamepleRate: %d\n", 
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.u4CurVidPts, 
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.ePrevFrameRate, 
            prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo.eCurFrameRate);
    }
        
    return eSWDMX_HDR_PRS_SUCCEED;
}


BOOL _SWDMX_CmpbHandleFlushFeeder(UINT8 u1SrcId, UINT8 u1Type, VOID *pvMMFlushInfo, UINT32 u4Para2, UINT32 u4Para3)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo = NULL;
    UINT32 ui4SeekPts = 0;
    
    if (prSwdmxInst == NULL)
    {
        return FALSE;
    }
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
       
    LOG(0, "Flush feeder here\n");    

    if (NULL != pvMMFlushInfo)
    {
        if (IS_AP_SETTED_SEEK_PTS(prSwdmxCmpbInfo->prDmxRangeInfo->e_mm_src_type))
        {
            ui4SeekPts = (*((UINT32*)pvMMFlushInfo))*90;
            LOG(1,"%s %d ApSeekPts is %d, will set it in next loop!\n",__FUNCTION__,__LINE__,ui4SeekPts);
            prSwdmxCmpbInfo->ui4SeekPos = ui4SeekPts;
        }
    }
    else 
    {
        LOG(0,"_SWDMX_CmpbHandleFlushFeeder pvMMFlushInfo is null\n");
    }
    
    prSwdmxCmpbInfo->fgSeekFlush = TRUE;
    
    if(prSwdmxCmpbInfo->fgEnVideo)
    {
        DMX_MM_FlushBuffer(prSwdmxCmpbInfo->u1VidPid);       
    }

    if(prSwdmxCmpbInfo->fgEnAudio)
    {
        DMX_MM_FlushBuffer(prSwdmxCmpbInfo->u1AudPid);        
    }

    if(prSwdmxCmpbInfo->fgEnSubTitle)
    {
        DMX_MM_FlushBuffer(prSwdmxCmpbInfo->u1SubPid);     
    }

    MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
    VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
    _SWDMX_FlushReqMsgQ(u1SrcId);
    
    prSwdmxCmpbInfo->fgApFlushFeeder = TRUE;

    return TRUE;
}

BOOL _SWDMX_CmpbCodecChgDoneCb(UINT8 u1SrcId, UINT8 u1Type, VOID* pvCodecChg)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo = NULL;

    if (prSwdmxInst == NULL)
    {
        return FALSE;
    }
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;
    
    if (pvCodecChg == NULL)
    {
        return FALSE;
    }
    
    if (u1Type == eSWDMX_TYPE_AUD_CODEC_CHG_DONE)
    {
        LOG(0,"Audio codec change done. [%s %d] eAudioType %d\n ",__FUNCTION__,__LINE__,prSwdmxInst->eUpdAudioType);            

        prSwdmxCmpbInfo->fgWaitAudCodecChgDone = FALSE;
        prSwdmxInst->eAudioType = prSwdmxInst->eUpdAudioType;
        prSwdmxCmpbInfo->u4ADecType = prSwdmxInst->eUpdAudioType;
        prSwdmxInst->eUpdAudioType = ENUM_SWDMX_ADEC_UNKNOWN;   
    }

    return TRUE;
}

static ENUM_SWDMX_ADEC_T _SWDMX_CmpbAudCodecConvert(UINT8 e_aud_codec)
{
    ENUM_SWDMX_ADEC_T e_swdmx_aud_codec = ENUM_SWDMX_ADEC_UNKNOWN;

    switch(e_aud_codec)
    {
        case AUD_DEC_FMT_MPEG:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_MPEG;
        break;
        case AUD_DEC_FMT_MP3:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_MP3;
        break;
        case AUD_DEC_FMT_AAC:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_AAC;
        break;
        case AUD_DEC_FMT_AC3:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_AC3;
        break;   
        case AUD_DEC_FMT_PCM:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_PCM;
        break;
        case AUD_DEC_FMT_DTS:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_DTS;
        break;
        case AUD_DEC_FMT_WMA:   
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_WMA;
        break;         
        default:
        e_swdmx_aud_codec = ENUM_SWDMX_ADEC_UNKNOWN;
        break;
    }
    
    return e_swdmx_aud_codec;
}

BOOL _SWDMX_CmpbChangeAudio(UCHAR u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_CMPB_INFO_T *prSwdmxCmpbInfo;
    SWDMX_CMPB_VERIFY_NULL_ERROR(prSwdmxInst);
    prSwdmxCmpbInfo = &prSwdmxInst->rFmtInfo.rSwdmxCmpbInfo;

    prSwdmxCmpbInfo->fgChgATrack = TRUE;

    LOG(0, "Audio change\n");
    prSwdmxCmpbInfo->fgWaitNextAudio = TRUE;
    prSwdmxCmpbInfo->u4LastAudioNum++;

    LOG(0, "++, u4LastAudioNum = %u\n", prSwdmxCmpbInfo->u4LastAudioNum);

    DMX_MM_FlushBuffer(prSwdmxCmpbInfo->u1AudPid); 
    
    return TRUE;
}

static BOOL _SWDMX_CmpbSetStaticFlagValue(INT32 i4FlagType, BOOL fgValue)
{
    BOOL fgRet = FALSE;
    if (FALSE !=fgValue && TRUE != fgValue)
    {
        LOG(1, "%s %d wrong value\n",__FUNCTION__,__LINE__);
        return fgRet;
    }
    
    LOG(3, "%s %d flagType is %d, SetValue is %d\n",__FUNCTION__,__LINE__,i4FlagType,fgValue);
    
    switch (i4FlagType)
    {
        case FLAG_TEST_PDCF:
            _fgTestPdcf = fgValue;
            fgRet = TRUE;
            break;
        case FLAG_TO_TRIG_AUD:
            _fgToTrigAud = fgValue;
            fgRet = TRUE;
            break;
        case FLAG_TO_SET_STC:
            _fgToSetStc = fgValue;
            fgRet = TRUE;
            break;
        default:
            LOG(0,"%s %d Doesn't has this flag(%d)!\n",__FUNCTION__,__LINE__,i4FlagType);
            fgRet = FALSE;
            break;
    }
    return fgRet;
}

static BOOL _SWDMX_CmpbGetStaticFlagValue(INT32 i4FlagType)
{
    BOOL fgRet = FALSE;
    switch (i4FlagType)
    {
        case FLAG_TEST_PDCF:
            fgRet = _fgTestPdcf;
            break;
        case FLAG_TO_TRIG_AUD:
            fgRet = _fgToTrigAud;
            break;
        case FLAG_TO_SET_STC:
            fgRet = _fgToSetStc;
            break;
        default:
            LOG(0,"%s %d Doesn't has this flag(%d)!\n",__FUNCTION__,__LINE__,i4FlagType);
            fgRet = WRONG_FLAG_VALUE;
            break;
    }
    return fgRet;
}

