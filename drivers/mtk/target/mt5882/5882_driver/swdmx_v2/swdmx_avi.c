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
 * $RCSfile: swdmx_avi.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_avi.c
 *  Software demux for avi - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dmx_mm_if.h"
#include "swdmx_avi.h"
#include "vdec_drvif.h"
#include "u_midxbuld.h"
#include "swdmx_debug.h"
#include "aud_drvif.h"
#include "x_util.h"
#include "x_drm_drv.h"
#if defined(CC_MT5387) || defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395) ||defined(SWDMX_IS_POST_MT5368)
#include "b2r_drvif.h"
#include "b2r_if.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define LE_LONG32(a) ((a >> 24) | ((a & 0x00FF0000) >> 8) | \
           ((a & 0x0000FF00) << 8) | (a << 24))
#undef min   
#define min(x, y) ((x < y) ? x : y)
#undef max
#define max(x, y) ((x > y) ? x : y)
#undef abs
#define abs(x)                  (((x) >= 0) ? (x) : -(x))
#define IDR_PIC 5
#define IS_IDR_NALU(x) (((x & 0x1f) == IDR_PIC))

#define SWDMX_AVI_AAC_FAKE_HDR_SIZE (7)
#define SWDMX_AVI_AAC_ALLOC_SIZE   (256)
#define _pu1VideoFrameData       (prSwdmxInst->pu1VideoFrameData)
#define WMV_SC_FRAME        0x10D
#define WMV_SC_ENTRY        0x10E
#define WMV_SC_SEQ          0x10F
#define WMV_SC_FIELD        0x10C



#define SWDMX_AVI_VERIFY_NULL(ptr)                                    \
          do {                                              \
            if (ptr == NULL) { \
                LOG(3, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                return; }      \
          } while (0)

#define SWDMX_AVI_VERIFY_NULL_ERROR(ptr)                                    \
          do {                                              \
            if (ptr == NULL) { \
                LOG(3, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                return FALSE; }      \
          } while (0)


#ifdef LINUX_TURNKEY_SOLUTION
#define SWDMX_AVI_VIDEO_HEADER_ALLOC_SIZE (512)
#endif
#define INPUT_SRC  prSwdmxInst->rFmtInfo.rSwdmxAviInfo.prDmxRangeInfo->e_mm_src_type
#ifdef SUPPORT_3D_MEDIA
#define ENABLE_NEWTRACK(prSwdmxInst) \
        ((prSwdmxInst->eVideoType==ENUM_SWDMX_VDEC_MJPEG) && \
        (prSwdmxInst->u1VdecId!=prSwdmxInst->u1Vdec2Id) && \
        (prSwdmxInst->fgEnVideo2Dmx))
#define IS_NEWTRACK(prSwdmxInst,u1Id) (ENABLE_NEWTRACK(prSwdmxInst)&&(u1Id==prSwdmxInst->u1Vdec2Id))
#else
#define ENABLE_NEWTRACK(prSwdmxInst) (0)
#define IS_NEWTRACK(prSwdmxInst,u1Id)(0)
#endif

#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
UINT32 _u4Threshold = 90000;
UINT32 _u4Delay = 1;
#define SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD _u4Threshold
#define SWDMX_AVI_BAD_INTERLEAVED_DELAY _u4Delay
#endif

//-----------------------------------------------------------------------------
// Static variables

static BOOL g_fgNotSupport[SWDMX_SOURCE_MAX] = {FALSE};


//-----------------------------------------------------------------------------

//static SWDMX_RANGE_LIST_T *_prRangeList = NULL;

#if 0
static SWDMX_AVI_INFO_T prSwdmxInst->rFmtInfo.rSwdmxAviInfo = {0};

static FEEDER_BUF_INFO_T _rFeederInfo;

static UINT32 _u4LastAlignOffset;

static UCHAR *_pucAviAacBuf = 0;
#ifdef LINUX_TURNKEY_SOLUTION
static UCHAR *_pucAviVidHeaderPtr = 0;
#endif
#endif

#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
static BOOL _fgBiAviAudioReady;
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static SWDMX_HDR_PRS_STATE_T _SWDMX_AviHdrPrsUseIdx(
         UINT8 u1SrcId,
        SWDMX_RANGE_LIST_T* prRangeList, 
        INT32 i4PlaySpeed);

static SWDMX_HDR_PRS_STATE_T _SWDMX_Avi3DHdrPrsUseIdx(
        UINT8 u1SrcId,
        SWDMX_RANGE_LIST_T* prRangeList, 
        INT32 i4PlaySpeed,
        MM_RANGE_ELMT_IDXTBL_T *prKeyTbl);


static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverAviDataUseIdx(
    UINT8 u1SrcId,
        INT32 i4PlaySpeed,
        MM_RANGE_ELMT_IDXTBL_T *prIdxTbl, 
        UINT32* pu4CurIdx, 
        SWDMX_RANGE_LIST_T* prRangeList,
       UINT8 u1VdecId);

static SWDMX_HDR_PRS_STATE_T _SWDMX_AviHdrPrsWithoutIdx(
    UINT8 u1SrcId,
        SWDMX_RANGE_LIST_T* prRangeList,
        INT32 i4PlaySpeed,
	 BOOL fgSpeedChange);

static SWDMX_HDR_PRS_STATE_T _SWDMX_PrsAviChunk(
    UINT8 u1SrcId,
    UCHAR* pucBitstream, 
    SWDMX_RANGE_LIST_T* prRangeList,
    UINT32 u4AvailSize,
    UINT32* pu4ConsumeSize, 
    INT32 i4PlaySpeed,
    BOOL fgRangeEnd);
        
static BOOL _SWDMX_AviGetDataFromFeeder(
    SWDMX_INFO_T *prSwdmxInst,
    FEEDER_REQ_DATA_T *prFeederData,
    UINT8 u1VdecId);

static BOOL _CheckAviChunkIndex(
    UCHAR *pucBuf, 
    UINT32 u4StrmID);

static BOOL _CheckAviChunkType(
    SWDMX_INFO_T *prSwdmxInst,
    UCHAR *pucBuf, 
    UINT8 *pu1HdrType, 
    UINT8 *pu1TrackIdx);

static BOOL _CheckAviOtherChunk(UCHAR *pucBuf);

static BOOL _SWDMX_InitDmx(
    SWDMX_INFO_T *prSwdmxInst,
    UINT8 u1HdrType, 
    UINT8 u1Index,
    VOID* pfnCbFunc, 
    BOOL fgEos);

static BOOL _SWDMX_AviHandleSpeedChange(
    SWDMX_INFO_T *prSwdmxInst,
    SWDMX_CUR_CMD_T* prCmd,
    SWDMX_RANGE_LIST_T* prRangeList,
    INT32 i4PlaySpeed);


static VOID AviMoniterVdecSupport(UCHAR u1SrcId, BOOL fgNotSupport);

static BOOL _SWDMX_AviFlush(SWDMX_INFO_T *prSwdmxInst);

static BOOL _SWDMX_SendAviEos(
    SWDMX_INFO_T *prSwdmxInst,
    UINT8 u1HdrType,
    UINT8 u1Index);

static BOOL _SWDMX_AviABRepeat(SWDMX_INFO_T* prSwdmxInst,
    UINT64* u8SeekPos,
    SWDMX_CUR_CMD_T* prCmd);

static BOOL _SWDMX_AviTimeSeek(SWDMX_INFO_T* prSwdmxInst,
    UINT64 u8SeekPos,
    INT32 i4PlaySpeed ,
    SWDMX_RANGE_LIST_T* prRangeList);

static BOOL _SWDMX_AviSeekCurPos(SWDMX_INFO_T* prSwdmxInst);

static BOOL _SWDMX_AviSeek(
    SWDMX_INFO_T* prSwdmxInst, 
    UINT64 u8SeekTime, 
    SWDMX_RANGE_LIST_T* prRangeList);

static BOOL _SWDMX_AviResetFeeder(SWDMX_INFO_T* prSwdmxInst);

static BOOL _SWDMX_AviCalculateRepeatFrame(
    SWDMX_INFO_T* prSwdmxInst, 
    INT32 i4PlaySpeed,  
    UINT8 u1VdecId);

static BOOL _SWDMX_Avi3DCalculateRepeatFrame(
    SWDMX_INFO_T* prSwdmxInst, 
    INT32 i4PlaySpeed,  
    UINT8 u1VdecId);

static void _SWDMX_AviCalculatePts(
    SWDMX_INFO_T* prSwdmxInst, 
    UINT8 u1HdrType, 
    UINT8 u1TrackIdx, 
    UINT32 u4ChunkSize);

static void _SWDMX_AviPrsDrmChunk(
    SWDMX_INFO_T* prSwdmxInst, 
    UINT8 u1HdrType, 
    UINT32 u4StartAddr);
static UINT32 _SWDMX_AVIPrsVC1StartCode(
    UINT32 u4FrameAddr,
    UINT32 u4FifoStart,
    UINT32 u4FifoEnd);
#if 0
static UINT32 _SWDMX_AviAsci2Decimal(
    UINT8*   aui1_ary, 
    UINT8    ui1_round);

static void _SWDMX_AviPrsSubtitle(
    SWDMX_INFO_T *prSwdmxInst,
    UCHAR* pucBuf,
    UINT32* pt_pts_start,
    UINT32* pt_pts_end);
#endif

static SWDMX_HDR_PRS_STATE_T AviHandAAC(SWDMX_INFO_T *prSwdmxInst, UINT32 u4ChunkSize, UINT32 u4Pts, UINT32 u4BufStartAddr, UINT8 u1TrackIdx);

static void _SWDMX_AviVideoWakeUp(SWDMX_INFO_T *prSwdmxInst);

static SWDMX_HDR_PRS_STATE_T  _SWDMX_AviBeforeDeliverUseIdx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1HdrType);


const UINT32 _pwMpegaSampFreq[3] = {44100, 48000, 32000};

/* bit rate table */
/*  MPEG1 - layer 3 */
const UINT32 _pdwLayer3BitRate1[14] = {
  32000,  40000,  48000,  56000,
  64000,  80000,  96000,  112000,
  128000, 160000, 192000, 224000,
  256000, 320000
};

/*  MPEG2 - layer 3 */
const UINT32 _pdwLayer3BitRate2[14] = {
  8000,   16000,  24000,  32000,
  40000,  48000,  56000,  64000,
  80000,  96000,  112000, 128000,
  144000, 160000
};
  /*  MPEG1 - layer 2 */
const UINT32 _pdwLayer2BitRate1[14] = {
  32000,  48000,  56000,  64000, 
  80000,  96000,  112000, 128000, 
  160000, 192000, 224000, 256000, 
  320000, 384000
};

/*  MPEG1 - layer 1 */
#if 0 
const DWRD _pdwLayer1BitRate1[14] = {
  32000, 64000, 96000, 128000, 
  160000, 192000, 224000, 256000, 
  288000, 320000, 352000, 384000, 
  416000, 448000
};
#endif

/*  MPEG2 - layer 1 */
const UINT32 _pdwLayer1BitRate2[14] = {
  32000, 48000, 56000, 64000,
  80000, 96000, 112000, 128000,
  144000, 160000, 176000, 192000,
  224000,256000
};

BOOL MPEGA_IS_VALID_HDR(const BYTE* hdr) 
{
  return ((hdr[0] == 0xFF) && 
         (((hdr[1] & 0xF0) == 0xF0)||((hdr[1] & 0xF0) == 0xE0)) && 
         ((hdr[1] & 0x06) != 0x0) && ((hdr[2] & 0x0C) != 0x0C) && 
         ((hdr[2] & 0xF0) != 0xF0) && ((hdr[2] & 0xF0) != 0x00));
}

BOOL MPEGA_IS_MPEG25(const BYTE* hdr)
{
   return ((hdr[0] == 0xFF) && ((hdr[1]&0xF0) == 0xE0));
}
UINT32 wMpA_GetSampFreq(BYTE bIdx, BOOL fgMpeg25)
{
  if (bIdx >= 0x3)
    return 0xffff;

  return fgMpeg25 ? _pwMpegaSampFreq[bIdx]/4 : _pwMpegaSampFreq[bIdx];
}

UINT32 dwMpA_GetBitRate(BYTE bIdx, BYTE bLayer, BYTE bID)
{
  UINT32 dwBitRate = 0;
    
  if(!((bIdx>=1)&&(bIdx<=14)))
  {
  	return dwBitRate;
  }

  switch (bLayer)
  {
  case MPEGA_LAYER_3: // - MP3
    if (bID)
      dwBitRate = _pdwLayer3BitRate1[bIdx - 1];  // - MPEG1
    else
      dwBitRate = _pdwLayer3BitRate2[bIdx - 1];  // - MPEG2
    break;
  case MPEGA_LAYER_2: // - MP2
    if (bID)
      dwBitRate = _pdwLayer2BitRate1[bIdx - 1];  // - MPEG1
    else
      dwBitRate = 2 * _pdwLayer3BitRate2[bIdx - 1];  // - MPEG2
    break;
  case MPEGA_LAYER_1: // - MP1
    if (bID)
    {    
      //dwBitRate = 4 * _pdwLayer1BitRate1[bIdx - 1];  // - MPEG1
      dwBitRate = 4 * 32000 * bIdx;
    }  
    else
      dwBitRate = 4 * _pdwLayer1BitRate2[bIdx - 1];  // - MPEG2
    break;
  default:
    // - ASST: unsupported layer
    dwBitRate = 0xffffffff;
    break;
  }

  return dwBitRate;
}

BYTE bMpA_GetBitRateFactor(BYTE bLayer, BYTE bID, BOOL fgMpeg25)
{
  if (bLayer == MPEGA_LAYER_1)
    return bID ? 12 : 24;

  //if (bLayer == MPEGA_LAYER_3)
  //  return bID ? 144 : 72;
  if (fgMpeg25)
    return 72;

  return 144;
}

UINT32 dwMpA_GetFrmSz(const BYTE *pbHdr, UINT32 *pBitRate)
{
  BYTE bID, bLayer, bBitRateIdx, bSampFreqIdx, bPaddingBit;
  UINT32 dwFrmSz;
  BOOL fgMpeg25;

  // - get related value from header
  bID = (pbHdr[1] & 0x08) >> 3;
  bLayer = (pbHdr[1] & 0x06) >> 1;
  bBitRateIdx = (pbHdr[2] & 0xF0) >> 4;
  bSampFreqIdx = (pbHdr[2] & 0x0C) >> 2;
  bPaddingBit = (pbHdr[2] & 0x02) >> 1;
  fgMpeg25 = MPEGA_IS_MPEG25(pbHdr);

  if ((bLayer == 0x0) || (bBitRateIdx == 0x0) || (bBitRateIdx == 0xFF) ||
      (bSampFreqIdx == 0x3))
  {
    // - ASST: given frame header is wrong
  } 

  *pBitRate = (UINT32)dwMpA_GetBitRate(bBitRateIdx, bLayer, bID) ;
  
  dwFrmSz = (((UINT32)bMpA_GetBitRateFactor(bLayer, bID, fgMpeg25) *
              dwMpA_GetBitRate(bBitRateIdx, bLayer, bID)) /
             (UINT32)wMpA_GetSampFreq(bSampFreqIdx, fgMpeg25)) + (UINT32)bPaddingBit;

  return dwFrmSz;
}

BOOL _SWDMX_AVISetDecoderInfo(
    UINT8 u1SrcId,
    UINT8 u1Type, 
    UINT32 u4Para1, 
    UINT32 u4Para2, 
    UINT32 u4Para3)
{

    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;

#ifdef LINUX_TURNKEY_SOLUTION
    UINT32 u4Align = 16;
#endif    

    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);        

    if(u1Type == eSWDMX_SET_VIDEOTYPE) 
    {
        #if defined(CC_AVI_MPEG1_DISABLE) && defined(CC_AVI_MPEG2_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MPEG1_MPEG2)
        {
            return FALSE;
        }
        else
        #endif
        #if defined(CC_AVI_MPEG4_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MPEG4)
        {
            return FALSE;
        }
        else
        #endif
        #if defined(CC_AVI_DX311_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_DX311)
        {
            return FALSE;
        }
        else
        #endif
        #if defined(CC_AVI_H263_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_H263)
        {
            return FALSE;
        }
        else
        #endif
        #if defined(CC_AVI_H264_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_H264)
        {
            return FALSE;
        }
        else
        #endif
        #if defined(CC_AVI_VC1_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_VC1)
        {
            return FALSE;
        }
        else
        #endif
        #if defined(CC_AVI_MJPEG_DISABLE)
        if(u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
        {
            return FALSE;
        }
        else
        #endif
        {
            prSwdmxAviInfo->u4VDecType =  u4Para1;
            return TRUE;
        }
    }
    else if (u1Type == eSWDMX_SET_AUDIOTYPE)
    {
        prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] = u4Para1;
        prSwdmxAviInfo->u4AudSampleRate[prSwdmxAviInfo->u1CurATrackIdx] = u4Para2;
        prSwdmxAviInfo->u4AudChannelNs[prSwdmxAviInfo->u1CurATrackIdx] = u4Para3;
        LOG(3, "Set Audio Info to track %d.\n", prSwdmxAviInfo->u1CurATrackIdx);
        return TRUE;
    }    
   else if (u1Type == eSWDMX_SET_VIDSEQHEADER)
   {
#ifndef LINUX_TURNKEY_SOLUTION
        prSwdmxAviInfo->u4VidHeaderPtr = u4Para1;
        prSwdmxAviInfo->u4VidHdrLength = u4Para2;
 #else
         prSwdmxAviInfo->u4VidHdrLength = u4Para2;
         
         _SWDMX_GetInfo(u1SrcId, 
                eSWDMX_GET_INTERNAL_BUF, 
                &prSwdmxAviInfo->u4VidHeaderPtr, 
                &u4Para2, 
                &u4Align);
         
         if(prSwdmxAviInfo->u4VidHeaderPtr)
         {
             x_memcpy((VOID*)VIRTUAL(prSwdmxAviInfo->u4VidHeaderPtr), 
                 (VOID*)VIRTUAL(u4Para1), 
                 prSwdmxAviInfo->u4VidHdrLength);
         }
         else
         {
             LOG(0,"Header Seq Buffer alloc fail\n");
         }

 #endif
        return TRUE;
    }    

    return FALSE;
}


BOOL _SWDMX_AVIGetInfo(UINT8 u1SrcId,
    UINT8 u1Type,
    UINT32* pu4Para1,
    UINT32* pu4Para2,
    UINT32* pu4Para3)
{
    UINT64 *pu8Tmp;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);  
    LOG(5, "_SWDMX_AVIGetInfo u1Type = %d\n", u1Type);

	if(u1Type == eSWDMX_GET_CURRENT_POSITION)
    {
        if(pu4Para1 == NULL)
        {
        	LOG(1, "_SWDMX_AVIGetInfo pu4Para1 = NULL\n");
            return FALSE;
        }
        pu8Tmp = (UINT64*)pu4Para1;
        *pu8Tmp = prSwdmxAviInfo->u8CurDmxMoviPos;
        return TRUE;
    }
    else if (u1Type == eSWDMX_GET_IS_TRICK)
    {
        if(prSwdmxAviInfo->fgEnVideo)
        {
            if ((prSwdmxAviInfo->prVidKeyIdxTbl != NULL)&&
                (prSwdmxAviInfo->prVidKeyIdxTbl->pt_next_partial != NULL))
            {
                if ((prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry < AVI_KEY_TABLE_THRESHOLD)&&
                   (prSwdmxAviInfo->prVidKeyIdxTbl->pt_next_partial->ui4_number_of_entry < AVI_KEY_TABLE_THRESHOLD))
                {
                    if(pu4Para1)
                    {
                        *pu4Para1 = FALSE;
                    }
                    if(pu4Para2)
                    {
                        *pu4Para2 = FALSE;
                    }
                }
                else
                {
                    if(pu4Para1)
                    {
                        *pu4Para1 = TRUE;
                    }
                    if(pu4Para2)
                    {
                        *pu4Para2 = TRUE;
                    }
                }
            }
            else if(prSwdmxAviInfo->prVidKeyIdxTbl != NULL)
            {
                if (prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry <
                    AVI_KEY_TABLE_THRESHOLD)
                {
                    if(pu4Para1)
                    {
                        *pu4Para1 = FALSE;
                    }
                    if(pu4Para2)
                    {
                        *pu4Para2 = FALSE;
                    }
                }
                else
                {
                    if(pu4Para1)
                    {
                        *pu4Para1 = TRUE;
                    }
                    if(pu4Para2)
                    {
                        *pu4Para2 = TRUE;
                    }
                }
            }
            
        }
        // for bad interleave invalid audio key idx tbl
        #if 0
        if(abs(prSwdmxAviInfo->i8FirstAVDel) > AVI_BAD_INTERLEAVE_THRESHOLD)
        {
            if(pu4Para1)
            {
                *pu4Para1 = FALSE;
            }
            if(pu4Para2)
            {
                *pu4Para2 = FALSE;
            }
        }
        #endif
    }
    else if(u1Type == eSWDMX_GET_AUDIO_ACTIVE_PIDX)
    {
        UINT32 u4AudFifoAddr, u4AudFifoSize,u4DmxAvailSize;
        _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);
        u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, 
                                              (DMX_PID_TYPE_T)DMX_PID_TYPE_ES_AUDIO,
                                              prSwdmxAviInfo->u1AudPid);
        
        *pu4Para1 = u4AudFifoSize - u4DmxAvailSize;
        return TRUE;
    }
        
    return FALSE;
}

BOOL _SWDMX_AviVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    UINT32 u4Addr;
    UCHAR  ucTmp;
    UINT64 u8DataSize;
    UINT64 u8Numerator = 0, u8Denominator = 0, u8Remainder = 0;
    SWDMX_INFO_T *prSwdmxInst = (SWDMX_INFO_T*)prPes->pvInstanceTag;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;

    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));

    if (!prSwdmxInst)
    {
        LOG(1, "Swdmx Instance is NULL\n");
        return FALSE;
    }
    
    rPesInfo.u4DTS = prPes->u4Dts;
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
    rPesInfo.fgDtsValid = prPes->fgPtsDts;
    rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
    rPesInfo.fgGop = prPes->fgGop;
    rPesInfo.fgEos = prPes->fgEOS;
    rPesInfo.ucEsId = prPes->u1Channel;     
    if(prSwdmxAviInfo->fgIs3DFile)  
    {
         if(prSwdmxAviInfo->fgIsLeftVideo)
         {
             rPesInfo.u4PTS = prSwdmxAviInfo->u4CurVidPts;
             rPesInfo.u8Offset = prSwdmxAviInfo->u8VidOffset;
         }
         else
         {
             rPesInfo.u4PTS = prSwdmxAviInfo->u4CurSubVidPts;
             rPesInfo.u8Offset = prSwdmxAviInfo->u8SubVidOffset;
         }
    }
    else
    {
         rPesInfo.u4PTS = prSwdmxAviInfo->u4CurVidPts;
         rPesInfo.u8Offset = prSwdmxAviInfo->u8VidOffset;
    }

    if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
    {
         rPesInfo.rExtra.u8AudPTS = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];
    }
    #ifdef CC_VDEC_FMT_DETECT
    rPesInfo.u4DmxFrameType = prPes->u4FrameType;
    #endif
    if(prSwdmxAviInfo->u4VDecType == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucMpvId = VLD1; // video format all use VLD expect jpeg
    }
    else
    {        
        rPesInfo.ucMpvId = VLD0; // video format all use VLD expect jpeg
    }
    if (rPesInfo.fgEos)
    {
         if(prSwdmxAviInfo->i4PrevSpeed < 0)
     	{
     	    LOG(3, "Send video eos, backward flag is true!!!\r\n");
     		rPesInfo.fgBackward = TRUE;
     	}
        rPesInfo.u4FifoStart = 0;
        rPesInfo.u4FifoEnd = 0;
        rPesInfo.u4VldReadPtr = 0;
    }

    if (!rPesInfo.fgEos)
    {
        if (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_MPEG1_MPEG2)
        {  
            if(prSwdmxAviInfo->fgIsFirstFrame)
            {
                UNUSED(VDEC_SetTotalTime(prSwdmxInst->u1VdecId, prSwdmxInst->u4TotalTime));
            }
            
            switch (prPes->u4FrameType)
            {
            case PIC_TYPE_I:
                rPesInfo.ucPicType = MPV_I_TYPE;
                prSwdmxAviInfo->u4LastIPts = rPesInfo.u4PTS;
                prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8VidOffset;
                break;

            case PIC_TYPE_P:
                rPesInfo.ucPicType = MPV_P_TYPE;
                break;

            case PIC_TYPE_B:
                rPesInfo.ucPicType = MPV_B_TYPE;
                break;

            default:
                #ifndef CC_VDEC_FMT_DETECT
                ASSERT(0);
                #endif
                break;
            }
        }
        else if(prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_H264)
        {
            rPesInfo.ucPicType = prPes->au1PicInfo[0];
            
            u4Addr = prPes->u4FrameAddr + 4;
            if(u4Addr >= prPes->u4BufEnd)
            {
                u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
            }
            rPesInfo.u4VldReadPtr = u4Addr;
            rPesInfo.u2SkipVidCount = prSwdmxAviInfo->u2SkipVidCount;
            prSwdmxAviInfo->u2SkipVidCount = 0;
            if(IS_IDR_NALU(rPesInfo.ucPicType))
            {
                rPesInfo.fgDtsValid = TRUE;
                prSwdmxAviInfo->u4LastIPts = rPesInfo.u4PTS;
                prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8VidOffset;
            }
            else
            {
                rPesInfo.fgDtsValid = FALSE;
            }
        }
        else if (prSwdmxAviInfo->u4VDecType == (UINT32)ENUM_SWDMX_VDEC_H265)
    	{	
    		rPesInfo.u4VideoType = (UINT32)DMX_VIDEO_H265;
    		rPesInfo.ucPicType = (UCHAR)prPes->u4FrameType;

    		LOG(5, "H265 codec, PicType = 0x%02x\n", rPesInfo.ucPicType);

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
    			LOG(2, "H265, Swdmx Got Iframe\n");

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
        else if(prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_MPEG4)
        {
            ucTmp = prPes->au1PicInfo[0];

			u8DataSize = prSwdmxInst->u8FileSize -prSwdmxInst->u8FileOffset;
            u8Numerator = u8DataSize*DEMUX_AVI_AV_PERIOD;
            u8Denominator = (UINT64)prSwdmxInst->u4TotalTime;
            if(u8Denominator != 0)
            {
                rPesInfo.u4BitRate = u8Div6432(u8Numerator, u8Denominator,&u8Remainder);
                rPesInfo.u4BitRate = rPesInfo.u4BitRate*8;
            }
			
            rPesInfo.u2SkipVidCount = prSwdmxAviInfo->u2SkipVidCount;
            rPesInfo.fgNoVdecCalPts = prSwdmxAviInfo->fgNoCalPts;
            prSwdmxAviInfo->u2SkipVidCount = 0;

            
            if (ucTmp == 0xB6)
            {
                ucTmp = prPes->au1PicInfo[1];
                prSwdmxAviInfo->fgNoCalPts = TRUE;
                
                switch (ucTmp >> 6)
                {
                    case 0:
                        rPesInfo.ucPicType  = I_TYPE;
                        prSwdmxAviInfo->u4LastIPts = rPesInfo.u4PTS;
                        prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8VidOffset;
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
            else if ((ucTmp & 0x20) == 0x20)
            {                
                rPesInfo.fgSeqHdr = TRUE;            
            }
            if(prSwdmxAviInfo->u4AbDmxPos != 0)
            {
                rPesInfo.fgDtsValid = TRUE;
            }
        }
        else if (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_DX311)
        {
            ucTmp = prPes->au1PicInfo[0];

            rPesInfo.fgDtsValid = TRUE;

            if ((ucTmp >> 6) & 0x1)
            {
                rPesInfo.ucPicType  = P_TYPE;
            }
            else
            {
                rPesInfo.ucPicType = I_TYPE;
                prSwdmxAviInfo->u4LastIPts = rPesInfo.u4PTS;
                prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8VidOffset;
            }
        }
        else if (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_H263)
        {
            ucTmp = prPes->au1PicInfo[0];
        
            switch (ucTmp & 0x02) 
            {
                case 0:
                    rPesInfo.ucPicType = I_TYPE;
                    prSwdmxAviInfo->u4LastIPts = rPesInfo.u4PTS;
                    break;
                case 2:
                    rPesInfo.ucPicType = P_TYPE;
                    break;
                default:
                    rPesInfo.ucPicType = UNKNOWN_TYPE;
                    break;
            }
        }
        else if (prSwdmxAviInfo->u4VDecType == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
        {
            rPesInfo.ucPicType = I_TYPE;
            prSwdmxAviInfo->u4LastIPts = rPesInfo.u4PTS;
            if(prSwdmxAviInfo->fgIs3DFile)  
            {
                 if(prSwdmxAviInfo->fgIsLeftVideo)
                 {
                     prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8VidOffset;
                 }
                 else
                 {
                     prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8SubVidOffset;
                 }
            }
            else
            {
                 prSwdmxAviInfo->u8LastIPos = prSwdmxAviInfo->u8VidOffset;
            }
        }
        else
        {	    	
            rPesInfo.fgDtsValid = TRUE;
            switch(prSwdmxAviInfo->u4VDecType)
            {
                case ENUM_SWDMX_VDEC_VC1:
                    rPesInfo.u4VideoType = DMX_VIDEO_VC1;
                break;
                case ENUM_SWDMX_VDEC_WMV7:
                    rPesInfo.u4VideoType = DMX_VIDEO_WMV7;
                break;
                case ENUM_SWDMX_VDEC_WMV8:
                    rPesInfo.u4VideoType = DMX_VIDEO_WMV8;
                break;
                case ENUM_SWDMX_VDEC_WMV9:
                    rPesInfo.u4VideoType = DMX_VIDEO_WMV9;
                break;
                default:
                    rPesInfo.u4VideoType = DMX_VIDEO_WMV9;
                break;
            }
        }
    }
    
    rPesInfo.u8OffsetI = prSwdmxAviInfo->u8LastIPos;
    rPesInfo.rExtra.u8PTSI = prSwdmxAviInfo->u4LastIPts;
    x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
    VDEC_SendEs((void*)&rPesInfo);
    
    return TRUE;
}


BOOL _SWDMX_AviAudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet;
    DMX_AUDIO_PES_T rPes;

    SWDMX_INFO_T *prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    if(prPes != NULL)
    {
         prSwdmxInst = (SWDMX_INFO_T*)(prPes->pvInstanceTag);
         SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    }
    else
    {
         LOG(1, "_SWDMX_AviAudioCallback prPes NULL\n");
         return FALSE;
    }

    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    x_memset(&rPes, 0, sizeof(rPes));

    x_memcpy(&rPes, prPes, sizeof(DMX_AUDIO_PES_T));
    if (prSwdmxAviInfo->i4PrevSpeed > 0)    
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

BOOL _SWDMX_AviPlay(UINT8 u1SrcId)
{
    SWDMX_INFO_T*  prSwdmxInst;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;

    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    
    if(abs(prSwdmxAviInfo->i8FirstAVDel) > AVI_BAD_INTERLEAVE_THRESHOLD)
    {
        STC_StartStc(prSwdmxInst->u1StcId);
        LOG(0, "Badinterleave file, (Pause->Play) STC_StartStc\n");
    }
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL _SWDMX_AVIInit(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType)
{
    INT32 i = 0;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);        
    UNUSED(eType);
    x_memset(&prSwdmxInst->rFmtInfo.rSwdmxAviInfo, 0, sizeof(SWDMX_AVI_INFO_T));

    if (!prSwdmxInst->pucAudBuf)
    {

        UINT32 u4Addr = 0;
        UINT32 u4Size = SWDMX_AVI_AAC_ALLOC_SIZE;
        UINT32 u4Align = 16;
        
        LOG(3, "_SWDMX_AVIInit audio buffer is NULL, Oops!\n");
        SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align);
	    prSwdmxInst->pucAudBuf = (UCHAR*)u4Addr;

        if(!prSwdmxInst->pucAudBuf)
        {
            LOG(1, "Allocate AAC buffer failed!\n");
            return FALSE;
        }
    }

    if (_pu1VideoFrameData == NULL)
    {
        _pu1VideoFrameData = (UINT8 *)BSP_AllocAlignedDmaMemory(1*1024,16);
        if (_pu1VideoFrameData == NULL)
        {
            ASSERT(0);
        }
    }
    //*********************************************************
    //FeederSetBufferSize(prSwdmxInst->eFeederSrc, 5*10*1024); 
    //FeederStart(prSwdmxInst->eFeederSrc);
#if 0
    FeederInstallCallback(prSwdmxInst->eFeederSrc, FEEDER_NOTIFY_DATA_REQ_DONE, 
    _SWDMX_FeederCallBack, (UINT32)&prSwdmxInst->rFeederInfo);
#endif
    //*********************************************************

    // Default Speed
    prSwdmxAviInfo->i4PrevSpeed = 1000;
    // Default Seek parameter
    prSwdmxAviInfo->fgIsSeek = FALSE;
    //prSwdmxAviInfo->fgPauseVidForAVSync = TRUE;

    prSwdmxAviInfo->fgRequestData = TRUE;
    prSwdmxAviInfo->fgIsFirstFrame = TRUE;
    prSwdmxAviInfo->fgGetMp3Status = FALSE;
    prSwdmxAviInfo->u4CurRepeatFrameIdx = 0xFFFFFFFF;
    //prSwdmxAviInfo->u4ModeChangPts = 0xFFFFFFFF;

    prSwdmxAviInfo->u8RecDmxMoviPos = 0;

    prSwdmxAviInfo->u8VSeekPts = 0xFFFFFFFF;
    prSwdmxAviInfo->u8VSeekPts <<= 32;
    prSwdmxAviInfo->u8VSeekPts += 0xFFFFFFFF;

    prSwdmxAviInfo->u8ASeekPts = 0xFFFFFFFF;
    prSwdmxAviInfo->u8ASeekPts <<= 32;
    prSwdmxAviInfo->u8ASeekPts += 0xFFFFFFFF;
    prSwdmxAviInfo->u1AudPid  = 0xFF;
    prSwdmxAviInfo->u1VidPid  = 0xFF;
    prSwdmxAviInfo->u1SubPid  = 0xFF;

    prSwdmxAviInfo->u8PrevSubPos = 0xFFFFFFFF;
    prSwdmxAviInfo->u8PrevSubPos <<= 32;
    prSwdmxAviInfo->u8PrevSubPos += 0xFFFFFFFF;

    prSwdmxAviInfo->u4PrevSpStrmID = 0xFFFFFFFF;
    if (prSwdmxAviInfo->fgIs3DFile)
    {
        prSwdmxAviInfo->fgSubVideoRequestData = TRUE;
        prSwdmxAviInfo->u4CurRepeatSubFrameIdx = 0xFFFFFFFF;
        prSwdmxAviInfo->u1Vid2Pid  = 0xFF;
        prSwdmxAviInfo->fgIsLeftVideo = FALSE;
    }
    
    if (u1SrcId < SWDMX_SOURCE_MAX) 
    {
        g_fgNotSupport[u1SrcId] = FALSE;
    }
    for (i=0; i < prSwdmxAviInfo->u1TotalATrack; i++)
    {
        prSwdmxInst->rFmtInfo.rSwdmxAviInfo.u1AudPreBufPid[i] = 0xFF;
    }
    return TRUE;
}

UINT8 _SWDMX_AVIAACSampFreqIdx(UINT32 u4SampFreq)
{
    switch(u4SampFreq)
    {
      case 96000:
        return(0x0);
      case 88200:
        return(0x1);
      case 64000:
        return(0x2);
      case 48000:
        return(0x3);
      case 44100:
        return(0x4);
      case 32000:
        return(0x5);
      case 24000:
        return(0x6);
      case 22050:
        return(0x7);
      case 16000:
        return(0x8);
      case 12000:
        return(0x9);
      case 11025:
        return(0xa);
      case 8000:
        return(0xb);
      case 7350:
        return(0xc);
      default:
        return(0xFF);
    }
}

UINT32 _SWDMX_AVIDecTypeCast(ASH_AUDIO_FMT_T  e_fmt)
{
    switch(e_fmt)
    {
    case ASH_AUDIO_FMT_MPEG:
        return ENUM_SWDMX_ADEC_MPEG;
    case ASH_AUDIO_FMT_AC3:
        return ENUM_SWDMX_ADEC_AC3;
    case ASH_AUDIO_FMT_PCM:
        return ENUM_SWDMX_ADEC_PCM;
    case ASH_AUDIO_FMT_MP3:
        return ENUM_SWDMX_ADEC_MP3;
    case ASH_AUDIO_FMT_AAC:
        return ENUM_SWDMX_ADEC_AAC;
    case ASH_AUDIO_FMT_DTS:
        return ENUM_SWDMX_ADEC_DTS;
    case ASH_AUDIO_FMT_WMA:
        return ENUM_SWDMX_ADEC_WMA;
    case ASH_AUDIO_FMT_RA:
        return ENUM_SWDMX_ADEC_RA;
    case ASH_AUDIO_FMT_HDCD:
        return ENUM_SWDMX_ADEC_HDCD;
    case ASH_AUDIO_FMT_MLP:
        return ENUM_SWDMX_ADEC_MLP;
    case ASH_AUDIO_FMT_MTS:
        return ENUM_SWDMX_ADEC_MTS;
    case ASH_AUDIO_FMT_VORBIS:
        return ENUM_SWDMX_ADEC_VORBIS;
    default:
        LOG(0, "Unknown fmt %d\n", e_fmt);
        return ENUM_SWDMX_ADEC_UNKNOWN;
    }
}


/**
*/
VOID _SWDMX_AVISetRange(
    UINT8 u1SrcId,
    SWDMX_RANGE_LIST_T *prRangeList, 
    UINT32 u4VidStrmID, 
    UINT32 u4AudStrmID, 
    UINT32 u4SubTitleID)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL(prSwdmxInst);   

    LOG(3, "Set range, file size = 0x%llx, offset = 0x%llx...\r\n", 
        prSwdmxInst->u8FileSize, prSwdmxInst->u8FileOffset);

    if (prRangeList != NULL &&
       prRangeList->prDmxRangeInfo != NULL)
    { 
        INT32 i = 0;
        INT32 j = 0;
        
        MM_RANGE_ELMT_IDXTBL_T *pBuldIdxTbl = 
        (MM_RANGE_ELMT_IDXTBL_T*)prRangeList->prDmxRangeInfo->pt_idxtbl_lst;

        LOG(3, "duration = 0x%llx...\n", prRangeList->prDmxRangeInfo->ui8_vid_duration);
        prSwdmxInst->prRangeList = prRangeList;
        prSwdmxAviInfo->prDmxRangeInfo = prRangeList->prDmxRangeInfo;
        prSwdmxAviInfo->fgIsNonSeekable = prRangeList->prDmxRangeInfo->b_non_seekable;
        if (prRangeList->prDmxRangeInfo->b_stereo_scopic)
        {
            prSwdmxAviInfo->fgIs3DFile = TRUE;
        }
        while (pBuldIdxTbl != NULL)
        {
            if ((pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_AUDIO ) &&
                ((pBuldIdxTbl->t_strm_id.u.t_stm_avi_id | AVI_AUDIO_FCC) == AVI_AUDIO_FCC))
            {
                prSwdmxAviInfo->prAudIdxTbl[j++] = pBuldIdxTbl;
            }
            else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_VIDEO )
            {
                prSwdmxAviInfo->fgIsSupport3DTrick = prRangeList->prDmxRangeInfo->b_support_3d_trick;
                if(prSwdmxAviInfo->fgIs3DFile)
                {
                    if(pBuldIdxTbl->t_strm_id.u.t_stm_avi_id == u4VidStrmID)
                    {
                         prSwdmxAviInfo->prVidIdxTbl = pBuldIdxTbl;
                    }  
                    else
                    {
                        prSwdmxAviInfo->prSubVidIdxTbl = pBuldIdxTbl;
                    }
                }
                else 
                {
                    if(pBuldIdxTbl->t_strm_id.u.t_stm_avi_id == u4VidStrmID)
                    {
                          prSwdmxAviInfo->prVidIdxTbl = pBuldIdxTbl;
                    }
                }   
             }       
            else if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_KEY)
            {
                prSwdmxAviInfo->fgKeyExist = TRUE;
                if(((pBuldIdxTbl->t_strm_id.u.t_stm_avi_id | AVI_VIDEO_COMPRE_FCC)   == AVI_VIDEO_COMPRE_FCC) ||
                   ((pBuldIdxTbl->t_strm_id.u.t_stm_avi_id | AVI_VIDEO_UNCOMPRE_FCC) == AVI_VIDEO_UNCOMPRE_FCC))
                {
                    if(prSwdmxAviInfo->fgIs3DFile)
                    {
                         if(pBuldIdxTbl->t_strm_id.u.t_stm_avi_id == u4VidStrmID)
                         {
                             prSwdmxAviInfo->prVidKeyIdxTbl = pBuldIdxTbl;
                             prSwdmxAviInfo->prCurVidKeyIdxTbl = pBuldIdxTbl;
                             LOG(0, "3D file last second key size:%d\n",
                                pBuldIdxTbl->u.pt_pb_idx_tbl_entry[(pBuldIdxTbl->ui4_number_of_entry-1)].ui4_size);
                             LOG(0, "3D file last key size:%d\n",
                                pBuldIdxTbl->u.pt_pb_idx_tbl_entry[pBuldIdxTbl->ui4_number_of_entry].ui4_size);
                         }
                         else 
                         {
                             prSwdmxAviInfo->prSubVidKeyIdxTbl = pBuldIdxTbl;
                             prSwdmxAviInfo->prCurSubVidKeyIdxTbl = pBuldIdxTbl;       
                             LOG(0, "3D file last second sub key size:%d\n",
                                pBuldIdxTbl->u.pt_pb_idx_tbl_entry[(pBuldIdxTbl->ui4_number_of_entry-1)].ui4_size);
                             LOG(0, "3D file last sub key size:%d\n",
                                pBuldIdxTbl->u.pt_pb_idx_tbl_entry[pBuldIdxTbl->ui4_number_of_entry].ui4_size);
                         }
                     }
                     else
                     {
                         prSwdmxAviInfo->prVidKeyIdxTbl = pBuldIdxTbl;
                         prSwdmxAviInfo->prCurVidKeyIdxTbl = pBuldIdxTbl;
                     }
                }
                else if ((pBuldIdxTbl->t_strm_id.u.t_stm_avi_id | AVI_AUDIO_FCC) == AVI_AUDIO_FCC)
                {
                    if (i < AVI_DIVX_MAX_AUD_TRACK)
                    {
                        if (pBuldIdxTbl->t_aud_spec_info.e_fmt < 0xFF)
                        {
                            prSwdmxAviInfo->u4ADecType[i] =
                                _SWDMX_AVIDecTypeCast(pBuldIdxTbl->t_aud_spec_info.e_fmt);
                        }
                        if (prSwdmxAviInfo->u4ADecType[i] == ENUM_SWDMX_ADEC_AAC)
                        {
                            prSwdmxAviInfo->u4AudSampleRate[i] =
                                pBuldIdxTbl->t_aud_spec_info.u.t_aac_info.ui4_sample_rate;
                            prSwdmxAviInfo->u4AudChannelNs[i] =
                                pBuldIdxTbl->t_aud_spec_info.u.t_aac_info.ui4_num_ch;
                        }
                        prSwdmxAviInfo->parAudKeyIdxTbl[i++] = pBuldIdxTbl;
                    }
                    else
                    {
                        LOG(3, "Too many audio track key index table\n");
                    }
                }
                else
                {
                    LOG(1, "Unknow Stream ID\n");
                }
            }

            pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)pBuldIdxTbl->pv_next_tbl;
        }

        if (prSwdmxAviInfo->prVidIdxTbl)
        {
            // The upper layer (mw or playmgr should prevent 0 case for rate and scale)
            if (prSwdmxAviInfo->prVidIdxTbl->ui4_scale)
            {
                prSwdmxAviInfo->u4VidFps = 
                    (UINT32)(prSwdmxAviInfo->prVidIdxTbl->ui4_rate/prSwdmxAviInfo->prVidIdxTbl->ui4_scale);
            }
            if (!prSwdmxAviInfo->u4VidFps || prSwdmxAviInfo->u4VidFps > 60)
            {
                prSwdmxAviInfo->u4VidFps = 30;
            }
            
            LOG(5, "prSwdmxAviInfo->u4VidFps = %ld\n", prSwdmxAviInfo->u4VidFps);
        }

        if (prSwdmxAviInfo->fgKeyExist)
        {
            prSwdmxAviInfo->u1TotalATrack = i;
        }
        else if (prSwdmxAviInfo->prAudIdxTbl)
        {
            prSwdmxAviInfo->u1TotalATrack = j;
        }
        // set current audio track idx
        for (i = 0; (i < prSwdmxAviInfo->u1TotalATrack) && (i < AVI_DIVX_MAX_AUD_TRACK); i++)
        {
            if ((prSwdmxAviInfo->fgKeyExist)&&(prSwdmxAviInfo->parAudKeyIdxTbl[i]))
            {
                if (prSwdmxAviInfo->parAudKeyIdxTbl[i]->t_strm_id.u.t_stm_avi_id == prSwdmxAviInfo->u4AStrmID)
                {
                    prSwdmxAviInfo->u1CurATrackIdx = i;
                    LOG(5, "Line %d, Cur Audio Track Idx = %ld\n", __LINE__, i);
                }
            }
            else
            {    
                if(prSwdmxAviInfo->prAudIdxTbl[i])
                {
                    if (prSwdmxAviInfo->prAudIdxTbl[i]->t_strm_id.u.t_stm_avi_id == prSwdmxAviInfo->u4AStrmID)
                    {
                        prSwdmxAviInfo->u1CurATrackIdx = i;
                        LOG(5, "Line %d, Cur Audio Track Idx = %ld\n", __LINE__, i);
                    }
                }
            }
        }
        
        //common have 
        //prSwdmxAviInfo->u4TotalTime = prRangeList->prDmxRangeInfo->ui8_pb_duration;
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(prSwdmxAviInfo->fgIsSupport3DTrick)
            {
                VDEC_SetMMParam(prSwdmxInst->u1VdecId, VDEC_MM_3D_TRICK_TYPE, VDEC_3D_TRICK_3D_VIEW, 0, 0);
                VDEC_SetMMParam(prSwdmxInst->u1Vdec2Id, VDEC_MM_3D_TRICK_TYPE, VDEC_3D_TRICK_3D_VIEW, 0, 0);
            }
            else
            {
                VDEC_SetMMParam(prSwdmxInst->u1VdecId, VDEC_MM_3D_TRICK_TYPE, VDEC_3D_TRICK_L_VIEW, 0, 0);
                VDEC_SetMMParam(prSwdmxInst->u1Vdec2Id, VDEC_MM_3D_TRICK_TYPE, VDEC_3D_TRICK_L_VIEW, 0, 0);
            }
        }
  }
  else
  {
     LOG(0, "Demuxer Range is NULL\n");
  }
}


BOOL _SWDMX_AVISetStrmID(
    UINT8 u1SrcId,
    UINT8 u1HdrType,  
    UINT32 u4StrmID, 
    UINT32 u4StrmSubID)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);        

    UNUSED(u4StrmSubID);
    
    switch(u1HdrType)
    {
        case eSWDMX_STRM_TYPE_VID:
            prSwdmxAviInfo->u4VStrmID = u4StrmID;
            LOG(5, "_SWDMX_AVISetStrmID Vid = 0x%08x\n", u4StrmID);
            break;
        #ifdef CC_3D_MM_DS_SUPPORT  
        case eSWDMX_STRM_TYPE_VID2:
            prSwdmxAviInfo->u4VSubStrmID = u4StrmID;
            LOG(5, "_SWDMX_AVISetStrmID Vid = 0x%08x\n", u4StrmID);
            break;
        #endif
        case eSWDMX_STRM_TYPE_AUD:
            prSwdmxAviInfo->u4AStrmID = u4StrmID;
            LOG(5, "_SWDMX_AVISetStrmID Aud = 0x%08x\n", u4StrmID);
            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxAviInfo->u4SpStrmID = u4StrmID;
            LOG(5, "_SWDMX_AVISetStrmID Sub = 0x%08x\n", u4StrmID);
            break;
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_AVIEnableStrm(
    UINT8 u1SrcId,
    UINT8 u1StrmType, 
    VOID* pfnCbFunc)
{
    INT32 i;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);        

    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            LOG(5, "_SWDMX_AVIEnableStrm Vid\n");
            prSwdmxAviInfo->fgEnVideo = TRUE;
            if (u1SrcId < SWDMX_SOURCE_MAX) 
            {
                g_fgNotSupport[u1SrcId] = FALSE;
            }
            break;
        case eSWDMX_STRM_TYPE_VID2:
            LOG(5, "_SWDMX_AVIEnableStrm Vid\n");
            prSwdmxAviInfo->fgEnVideo = TRUE;
            if (u1SrcId < SWDMX_SOURCE_MAX) 
            {
                g_fgNotSupport[u1SrcId] = FALSE;
            }
            break;
        case eSWDMX_STRM_TYPE_AUD:
            LOG(5, "_SWDMX_AVIEnableStrm Aud\n");
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
            _fgBiAviAudioReady = FALSE;
#endif        
#if 0
            if (prSwdmxAviInfo->u4NumSentAud != 0)
            {
                prSwdmxAviInfo->fgChgATrack = TRUE;
            }
#endif

            for (i = 0; (i < prSwdmxAviInfo->u1TotalATrack) && (i < AVI_DIVX_MAX_AUD_TRACK); i++)
            {
                if ((prSwdmxAviInfo->fgKeyExist)&&(prSwdmxAviInfo->parAudKeyIdxTbl[i]))
                {
                    if (prSwdmxAviInfo->parAudKeyIdxTbl[i]->t_strm_id.u.t_stm_avi_id == prSwdmxAviInfo->u4AStrmID)
                    {
                        prSwdmxAviInfo->u1CurATrackIdx = i;
                        LOG(5, "Line %d, Cur Audio Track Idx = %ld\n", __LINE__, i);
                    }
                }
                else
                {    
                    if(prSwdmxAviInfo->prAudIdxTbl[i])
                    {
                        if (prSwdmxAviInfo->prAudIdxTbl[i]->t_strm_id.u.t_stm_avi_id == prSwdmxAviInfo->u4AStrmID)
                        {
                            prSwdmxAviInfo->u1CurATrackIdx = i;
                            LOG(5, "Line %d, Cur Audio Track Idx = %ld\n", __LINE__, i);
                        }
                    }
                }
            }
            if (((prSwdmxAviInfo->u4PrevAStrmID != 0xFFFFFFFF)&&(prSwdmxAviInfo->u4PrevAStrmID != 0)&&
                (prSwdmxAviInfo->u4PrevAStrmID != prSwdmxAviInfo->u4AStrmID)) ||
                 prSwdmxAviInfo->fgBeginToPlay == TRUE)
            {
                prSwdmxAviInfo->fgChgATrack = TRUE;
            }
            else if (prSwdmxAviInfo->fgAudStrmDisable && (!prSwdmxAviInfo->fgPreviouIsSeek) && (!prSwdmxAviInfo->fgSpeedChange))
            {
                LOG(5, "Audio stream is disabled and seek is not performed\n");
                prSwdmxAviInfo->fgChgATrack = TRUE;
                prSwdmxAviInfo->fgAudStrmDisable = FALSE;
            }
            prSwdmxAviInfo->fgPreviouIsSeek = FALSE;
            prSwdmxAviInfo->u4PrevAStrmID = prSwdmxAviInfo->u4AStrmID;
            prSwdmxAviInfo->fgEnAudio = TRUE;
            break;
        case eSWDMX_STRM_TYPE_SUBTITLE:
            if (prSwdmxAviInfo->fgEnSubDmx == FALSE)
            {
                UNUSED(_SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_SUBTITLE, prSwdmxInst->u1SwdmxId, pfnCbFunc, FALSE));
                prSwdmxAviInfo->fgEnSubDmx = TRUE;
            }
            prSwdmxAviInfo->u4PrevSpStrmID = prSwdmxAviInfo->u4SpStrmID;
	    prSwdmxAviInfo->fgChgSbTrack = TRUE;	
            prSwdmxAviInfo->fgEnSubTitle = TRUE;
            LOG(5, "_SWDMX_AVIEnableStrm Sub\n");
           break;
        default:
            return FALSE;
    }

    return TRUE;
}


BOOL _SWDMX_AVIDisableStrm(
    UINT8 u1SrcId,
    UINT8 u1StrmType)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);        

    switch(u1StrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
            if(prSwdmxAviInfo->fgVDecIsPause)
            {
                _SWDMX_AviVideoWakeUp(prSwdmxInst);
                prSwdmxAviInfo->fgVDecIsPause = FALSE;
            }

            prSwdmxAviInfo->fgEnVideo = FALSE;
            LOG(5, "_SWDMX_AVIDisableStrm Vid\n");
            break;
            
        case eSWDMX_STRM_TYPE_VID2:
            if(prSwdmxAviInfo->fgVDecIsPause)
            {
                _SWDMX_AviVideoWakeUp(prSwdmxInst);
                prSwdmxAviInfo->fgVDecIsPause = FALSE;
            }
            prSwdmxAviInfo->fgEnVideo = FALSE;
            prSwdmxInst->u1Vdec2Id = 0;
            LOG(5, "_SWDMX_AVIDisableStrm Vid2\n");
            break;
            
        case eSWDMX_STRM_TYPE_AUD:
            prSwdmxAviInfo->fgEnAudio = FALSE;
            prSwdmxAviInfo->fgAudStrmDisable = TRUE;
            DMX_AudHandler_SetActivePidx(DMX_AUDIO_HANDLER_NULL_PIDX);
			prSwdmxAviInfo->fgSetActivePid = FALSE;
            LOG(5, "_SWDMX_AVIDisableStrm Aud\n");
            break;
            
        case eSWDMX_STRM_TYPE_SUBTITLE:
            prSwdmxAviInfo->fgEnSubTitle = FALSE;
            LOG(5, "_SWDMX_AVIDisableStrm Sub\n");
            break;
            
        default:
            return FALSE;
    }

    return TRUE;
}

/**
*/
SWDMX_HDR_PRS_STATE_T _SWDMX_AVIHdrParse(
    UINT8 u1SrcId,
	SWDMX_RANGE_LIST_T* prRangeList,
	SWDMX_CUR_CMD_T* prCmd)
{
    INT32 i4PlaySpeed = prCmd->i4PlaySpeed;
    BOOL fgSpeedChange = prCmd->fgChange;
    UINT32 u4CurPts = 0;
    UINT32 u4PrsState = 0;
    VDEC_PTS_INFO_T rPTSInfo ;
    VDEC_PTS_INFO_T rSubPTSInfo ;
     
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;

    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
    
    if (prRangeList->prDmxRangeInfo == NULL)
    {
        LOG(5, "This range is empty\n");
        return eSWDMX_HDR_PRS_EMPTY_RANGE;
    }

    if ((prSwdmxInst->eSwDmxPrevousStatus == eSWDMX_STATE_PAUSE) && (i4PlaySpeed == 1000) && (prSwdmxAviInfo->fgPreviouIsSeek))
    {
        LOG(5, "PAUSE -> SEEK -> RESUME set stc value to 0x%08x\n", prSwdmxAviInfo->u4RecSeekTime);
        
        STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxAviInfo->u4RecSeekTime);
		prSwdmxAviInfo->fgPreviouIsSeek = FALSE;
    }
    else if ((prSwdmxAviInfo->fgPreviouIsSeek) && (i4PlaySpeed != 1000) && (i4PlaySpeed != 1))
    {
        prSwdmxAviInfo->fgPreviouIsSeek = FALSE;
    }
  
    if((prSwdmxAviInfo->fgEnVideo) && (!prSwdmxAviInfo->fgEnVDmx))
    {
        UNUSED(_SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId, NULL, FALSE));
        prSwdmxAviInfo->fgEnVDmx = TRUE;
            
        if (prSwdmxAviInfo->fgIs3DFile)
        {
            if(!prSwdmxAviInfo->fgEnV2Dmx)
            {
                UNUSED(_SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1Vdec2Id, NULL, FALSE));
                prSwdmxAviInfo->fgEnV2Dmx = TRUE;
            }
        }
    }

    if ((prSwdmxAviInfo->fgEnAudio) && (!prSwdmxAviInfo->fgEnADmx))
    {
        UNUSED(_SWDMX_InitDmx(prSwdmxInst, eSWDMX_MEDIA_AUDIO, 0, NULL, FALSE));
        prSwdmxAviInfo->fgEnADmx = TRUE;
    }
    
    if ((!prSwdmxAviInfo->fgBeginToPlay) && (prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts != 0))
    {
        x_memset((void*)&rPTSInfo, 0, sizeof(VDEC_PTS_INFO_T));
        prSwdmxAviInfo->fgFirstVdChunk = TRUE;
        prSwdmxAviInfo->fgFirstAdChunk = TRUE;
        if(prSwdmxAviInfo->prVidIdxTbl)
        {
            rPTSInfo.u8Scale = (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD;
            rPTSInfo.u4Rate = prSwdmxAviInfo->prVidIdxTbl->ui4_rate;
        }
        if (prSwdmxAviInfo->fgIs3DFile)
        {
            x_memset((void*)&rSubPTSInfo, 0, sizeof(VDEC_PTS_INFO_T));
            if (prSwdmxAviInfo->prSubVidIdxTbl)
            {
                 rSubPTSInfo.u8Scale = (UINT64)prSwdmxAviInfo->prSubVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD;
                 rSubPTSInfo.u4Rate = prSwdmxAviInfo->prSubVidIdxTbl->ui4_rate;
            }
        }
       
        if(prSwdmxAviInfo->u4VidHeaderPtr != 0)
        {
            BOOL fgSeekReady = FALSE;
            
            _SWDMX_AviBeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);
            
            fgSeekReady = _SWDMX_AviSeek(prSwdmxInst, prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts, prRangeList);
            
	        LOG(5, "Video Start Render Pts = %lld\n", prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts);
	        
   	        if(fgSeekReady)
   	        {
   		        prRangeList->fgSetRenderPosition = FALSE; 
   		    }
   		    
            prSwdmxAviInfo->u8VSeekPts = prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts;
            prSwdmxAviInfo->fgIsSeek = TRUE;
        }
        
        prSwdmxAviInfo->u8ASeekPts = prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts;

        if (prSwdmxAviInfo->fgEnVideo)
        {
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, 0, &rSubPTSInfo);
                VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts);
            }
            
            VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, 0, &rPTSInfo);
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts);
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                VDEC_Pause(prSwdmxInst->u1Vdec2Id);
            }
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxAviInfo->fgVDecIsPause = TRUE;
        }
        
        VDEC_SetVdecNotSupportNfy(u1SrcId,AviMoniterVdecSupport);
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts - 45000);
        prSwdmxAviInfo->fgBeginToPlay = TRUE;
    }
    else if ((!prSwdmxAviInfo->fgBeginToPlay) && (prSwdmxAviInfo->prDmxRangeInfo->t_aud_start_render_pts != 0))
    {
        BOOL fgSeekReady = FALSE;
        prSwdmxAviInfo->fgFirstVdChunk = TRUE;
        prSwdmxAviInfo->fgFirstAdChunk = TRUE;

        fgSeekReady = _SWDMX_AviSeek(prSwdmxInst, prSwdmxAviInfo->prDmxRangeInfo->t_aud_start_render_pts, prRangeList);
        
        LOG(5, "Audio Start Render Pts = %lld\n", prSwdmxAviInfo->prDmxRangeInfo->t_aud_start_render_pts);
        
        if (fgSeekReady)
  	    {
  	        prRangeList->fgSetRenderPosition = FALSE;
  	    }
  	    
        prSwdmxAviInfo->fgIsSeek = TRUE;
        prSwdmxAviInfo->u8ASeekPts = prSwdmxAviInfo->prDmxRangeInfo->t_aud_start_render_pts;
        VDEC_SetVdecNotSupportNfy(u1SrcId,AviMoniterVdecSupport);
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxAviInfo->prDmxRangeInfo->t_aud_start_render_pts - 45000);
        prSwdmxAviInfo->fgBeginToPlay = TRUE;
    }
    else if((!prSwdmxAviInfo->fgBeginToPlay) && (prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info!=0))
    {
        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != FEEDER_E_OK)
        {
            LOG(7, "Feeder input buffer error.\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
        if (prSwdmxAviInfo->fgEnVideo)
        {
            VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, 0, &rPTSInfo);
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info);
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxAviInfo->fgVDecIsPause = TRUE;
            prRangeList->fgSetRenderPosition = TRUE;
        }
        if ( prSwdmxAviInfo->prVidKeyIdxTbl && prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry != 0)
        {
               BOOL fgSeekReady = FALSE;
               if(prSwdmxAviInfo->u4VidHeaderPtr != 0)
               {
                   _SWDMX_AviBeforeDeliverUseIdx(prSwdmxInst, eSWDMX_MEDIA_VIDEO);
               }
               
               fgSeekReady = _SWDMX_AviSeek(prSwdmxInst, prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info, prRangeList);
               
    	       if(fgSeekReady)
    		   {
                    prRangeList->fgSetRenderPosition = FALSE; 
    		   }
                
               prSwdmxAviInfo->u8VSeekPts = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info;
               prSwdmxAviInfo->u8ASeekPts = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info;
               prSwdmxAviInfo->fgIsSeek = TRUE;
        } 
        else
        {
	        if(prSwdmxAviInfo->fgEnAudio)
	        {
	            prSwdmxAviInfo->u8ASeekPts = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info;
	        }
	        
	        prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_i_frame_position;
	        
	        prSwdmxAviInfo->u4CurVidPts = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_i_pts_info;
	
	        prSwdmxAviInfo->u8CurMoviPos = prSwdmxAviInfo->u8CurDmxMoviPos;
	        prSwdmxAviInfo->u4CurDmxFeederRPtr = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
	
	        prSwdmxAviInfo->u8AbRepeatVidPts = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info;         
		    prSwdmxAviInfo->u8AbRepeatAudPts =prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_aud_pts_info;
        }
        
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info - 45000);
        if(i4PlaySpeed == 1000)
        {
            prSwdmxAviInfo->fgRecSpeedChangeForAud = TRUE;
            prSwdmxAviInfo->i4RecPrevSpeedForAud = prSwdmxAviInfo->i4PrevSpeed;
        }
    }

    if(!prSwdmxAviInfo->fgBeginToPlay && prRangeList->fgSetRenderPosition == TRUE &&
        prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info == 0)
    {
        LOG(5, "Set Render Position = 0!!!\r\n");
        prRangeList->fgSetRenderPosition = FALSE;
    }

    if (!prSwdmxAviInfo->fgBeginToPlay && i4PlaySpeed == 1000)
    {
        x_memset((void*)&rPTSInfo, 0, sizeof(VDEC_PTS_INFO_T));
        
        if(prSwdmxAviInfo->prVidIdxTbl)
        {
            rPTSInfo.u8Scale = (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD;
            rPTSInfo.u4Rate = prSwdmxAviInfo->prVidIdxTbl->ui4_rate;
        }
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            x_memset((void*)&rSubPTSInfo, 0, sizeof(VDEC_PTS_INFO_T));
            if(prSwdmxAviInfo->prSubVidIdxTbl)
            {
                rSubPTSInfo.u8Scale = (UINT64)prSwdmxAviInfo->prSubVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD;
                rSubPTSInfo.u4Rate = prSwdmxAviInfo->prSubVidIdxTbl->ui4_rate;
            }
        }
        
       if (prSwdmxAviInfo->fgEnVideo)
        {
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, 0, &rSubPTSInfo);
            }
            VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, 0, &rPTSInfo);

            if(prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info == 0)
            { 
                 if(prSwdmxAviInfo->fgIs3DFile)
                 {
                     VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, prSwdmxAviInfo->u4CurSubVidPts);
                 }
                 VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prSwdmxAviInfo->u4CurVidPts);
            }
            else
            {
                 if(prSwdmxAviInfo->fgIs3DFile)
                 {
                    VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, prRangeList->u8StartPts);
                 }
                 VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, prRangeList->u8StartPts);
            }
            
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);

#if 1
            {
                /* [6/26/2012] workaround for mw/drv timing issue, added by cong.wang */
                #define _LOOP_CNT 200
                UINT32 u4Cnt = 0;
                UINT32 u4Status;

                while(u4Cnt < _LOOP_CNT)
                {
                    u4Status = MPV_GetDecMode(prSwdmxInst->u1VdecId);
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
            
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                VDEC_Pause(prSwdmxInst->u1Vdec2Id);
            }
            VDEC_Pause(prSwdmxInst->u1VdecId);
            prSwdmxAviInfo->fgVDecIsPause = TRUE;
        }
        if (prSwdmxAviInfo->fgEnAudio && prSwdmxAviInfo->fgEnVideo)
        {
            VDEC_SetVdecNotSupportNfy(u1SrcId,AviMoniterVdecSupport);
            
             if(prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx] && 
                 prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->b_is_VBR)
             {
                 prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx] += prRangeList->prDmxRangeInfo->t_aud_start_pts;
             }
        }
        
        if (prSwdmxAviInfo->fgEnAudio)
        {
            if (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
            {
                if(!prRangeList->fgSetRenderPosition)
                {
                      AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);
                      prSwdmxAviInfo->u4TriggerAudPts = 
                                        prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];
                }
                else
                {
                      AUD_SetStartPts(prSwdmxInst->u1AdecId,prRangeList->u8StartAudPts);
                      prSwdmxAviInfo->u4TriggerAudPts = prRangeList->u8StartAudPts;
                }
            }
            else
            {
                LOG(1, "Out of Audio Track Number\n");
                if(!prRangeList->fgSetRenderPosition)
                {
                     AUD_SetStartPts(prSwdmxInst->u1AdecId, 0);
                     prSwdmxAviInfo->u4AudStartPts = 0;
                }
                else
                {
                     AUD_SetStartPts(prSwdmxInst->u1AdecId,prRangeList->u8StartAudPts);
                     prSwdmxAviInfo->u4TriggerAudPts = prRangeList->u8StartAudPts;
                }
            }
        }
        // set stc for starting playback
        STC_StopStc(prSwdmxInst->u1StcId);
        
        if(!prRangeList->fgSetRenderPosition)
        {
            if(!prSwdmxAviInfo->fgIsSeek)
            {
                STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxAviInfo->u4CurVidPts - 45000);
            }
        }
        else
        {
            STC_SetStcValue(prSwdmxInst->u1StcId, prRangeList->u8StartPts);
        }
        prSwdmxAviInfo->fgBeginToPlay = TRUE;
    }     

    if((prRangeList->fgSetRenderPosition) && (prSwdmxAviInfo->fgSendVolInfo))
    {
        UINT8  i = 0;
        prSwdmxAviInfo->fgSendVolInfo = FALSE;
        prRangeList->fgSetRenderPosition =FALSE;
       
        if( (prSwdmxAviInfo->prDmxRangeInfo->t_aud_start_render_pts == 0) &&
            (prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts == 0))
        {
            for (i =0; (i<prSwdmxAviInfo->u1TotalATrack)&&(i<AVI_DIVX_MAX_AUD_TRACK); i++)
            {
                prSwdmxAviInfo->u4AudPts[i] = 0;
            }
            prSwdmxAviInfo->fgFeederInvalid = TRUE;
            prSwdmxAviInfo->u8CurDmxMoviPos = prRangeList->u8OffsetI;
            prSwdmxAviInfo->u4CurVidPts = prRangeList->u8StartPtsI;
            prSwdmxAviInfo->u8AbRepeatVidPts = prRangeList->u8StartPts;         
            prSwdmxAviInfo->u8AbRepeatAudPts = prRangeList->u8StartAudPts;  
            LOG(5, "set render audio pts and video pts.\n");
        }
            
    }
    
    if(i4PlaySpeed != 1000)
    {
    	  if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
    	  {
    	    if( (prSwdmxAviInfo->fgKeyExist)&&(prSwdmxAviInfo->prAudIdxTbl[0] != NULL))
            {
                prRangeList->u4CurRangeAudIdx = prSwdmxAviInfo->prAudIdxTbl[0]->ui4_number_of_entry;
            }
            else if (prSwdmxAviInfo->prAudIdxTbl [prSwdmxAviInfo->u1CurATrackIdx]!= NULL)
            {
                prRangeList->u4CurRangeAudIdx = prSwdmxAviInfo->prAudIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->ui4_number_of_entry;
            }
        }
    }

    prSwdmxAviInfo->fgVdecNotSupport = g_fgNotSupport[u1SrcId];
    //SpeedChange    Reset some values for state change.
    if (prSwdmxAviInfo->fgChgATrack || fgSpeedChange)
    {
        if (prSwdmxAviInfo->fgDeliverVEos)
        {
            UINT32 u4Flag;
            DMX_MM_T rDmxMMInfo;

            rDmxMMInfo.fgEnable = TRUE;
            rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;

            switch (prSwdmxAviInfo->u4VDecType)
            {
                case ENUM_SWDMX_VDEC_DX311: case ENUM_SWDMX_VDEC_WMV7:
                case ENUM_SWDMX_VDEC_WMV8: case ENUM_SWDMX_VDEC_WMV9:
                case ENUM_SWDMX_VDEC_MJPEG:case ENUM_SWDMX_VDEC_H263:
                    LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
                    rDmxMMInfo.fgSearchStartCode = FALSE;
                    break;
                default:
                    LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                    rDmxMMInfo.fgSearchStartCode = TRUE;      
                    break;
            } 
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                if (!DMX_MM_Set(prSwdmxAviInfo->u1Vid2Pid, u4Flag, &rDmxMMInfo))
                {
                    LOG(3, "Fail to reset demuxer\n");
                }
            }
            if (!DMX_MM_Set(prSwdmxAviInfo->u1VidPid, u4Flag, &rDmxMMInfo))
            {
                LOG(3, "Fail to reset demuxer\n");
            }
            
            prSwdmxAviInfo->fgDeliverVEos = FALSE;
        }
        if (prSwdmxAviInfo->fgDeliverAEos)
        {
            prSwdmxAviInfo->fgDeliverAEos = FALSE;
            AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
        }
                 
        prSwdmxAviInfo->fgAudIsReady = FALSE;
        prSwdmxAviInfo->fgQAud = FALSE;
        prSwdmxAviInfo->fgAudIsLoaded = FALSE;
        prSwdmxAviInfo->fgBadAudio = FALSE;
        
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
    
    if(fgSpeedChange)
    {
         _SWDMX_AviHandleSpeedChange(prSwdmxInst,prCmd,prRangeList,i4PlaySpeed);
    }

    if ((prSwdmxAviInfo->fgChgATrack || prSwdmxAviInfo->fgChgSbTrack) &&
        (i4PlaySpeed>0) && (!prSwdmxAviInfo->fgPreviouIsSeek) && (!prSwdmxAviInfo->fgSpeedChange))
    {
#ifdef CC_DLNA_SUPPORT
        if (INPUT_SRC == MM_SRC_TYPE_HIGH_SPEED_STORAGE)
        {
#endif
            if(prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info != 0 &&
               prSwdmxAviInfo->fgFirstVdChunk == FALSE && prSwdmxAviInfo->fgFirstAdChunk == FALSE)
            {
                u4CurPts = prSwdmxAviInfo->prDmxRangeInfo->t_start_pos.ui8_pts_info;
            }
            else if(prSwdmxAviInfo->fgEnVideo)
            {
                u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
            }
            else
            {
                u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);
            }

            if(prSwdmxAviInfo->u4CurVidPts - u4CurPts > 60*90000)
            {
                u4CurPts = prSwdmxAviInfo->u4CurVidPts-27000;
                LOG(5, "Avi pts diff = 0x%08x, re-assign\n", 
                                    prSwdmxAviInfo->u4CurVidPts - u4CurPts);
            }

            if(!prSwdmxAviInfo->fgIsNonSeekable)
            {
                if (prSwdmxAviInfo->fgChgSbTrack)
                {
                    _SWDMX_FlushReqMsgQ(prSwdmxInst->u1SwdmxId);
                    prSwdmxAviInfo->fgFeederInvalid = TRUE;
      
                    if (prSwdmxAviInfo->u8CurDmxMoviPos > prSwdmxAviInfo->u8RecDmxMoviPos)
                    {
                        prSwdmxAviInfo->u8RecDmxMoviPos = prSwdmxAviInfo->u8CurDmxMoviPos;
                    }
        			if(prSwdmxAviInfo->fgKeyExist)
        			{
        				UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));
        			}
        			else if(prSwdmxAviInfo->fgEnVDmx)
        			{
        				_SWDMX_AviSeekCurPos(prSwdmxInst);
        			}
					
                    if (((prSwdmxAviInfo->u8PrevSubPos >> 32 != 0xFFFFFFFF) ||
                        (prSwdmxAviInfo->u8PrevSubPos != 0xFFFFFFFF)) &&
                        (prSwdmxAviInfo->u8PrevSubPos != 0) &&
                        (prSwdmxAviInfo->u8PrevSubPos < prSwdmxAviInfo->u8CurDmxMoviPos))
                    {
                        if ((prSwdmxAviInfo->u8CurDmxMoviPos > prSwdmxAviInfo->u8PrevSubPos) &&
                            ((prSwdmxAviInfo->u8CurDmxMoviPos - prSwdmxAviInfo->u8PrevSubPos) < 5 * ONE_MB))
                        {
                            INT32 i = 0;
                            prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxAviInfo->u8PrevSubPos;
                            prSwdmxAviInfo->fgIsSeek = TRUE;
                            prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
                            prSwdmxAviInfo->u8CurMoviPos = prSwdmxAviInfo->u8CurDmxMoviPos;
                            //Recover Aud/Vid pts
                            prSwdmxAviInfo->u4CurVidPts = prSwdmxAviInfo->u4RecVidPtsForSub;
                            for (i = 0; (i < prSwdmxAviInfo->u1TotalATrack) && (i < AVI_DIVX_MAX_AUD_TRACK); i++)
                            {
                                 prSwdmxAviInfo->u4AudPts[i] = 
                                    prSwdmxAviInfo->u4RecAudPtsForSub[i];
                                LOG(5, "Avi Seek Audio pts = 0x%08x\n", 
                                    prSwdmxAviInfo->u4AudPts[i]);
                            }
                        }
                    }
       
                    prSwdmxAviInfo->fgChgSbTrack = FALSE;
                    prSwdmxAviInfo->u4ChangeMode |= AVI_CHANGE_SUBTITLE_MODE;
                    LOG(5, "Change SubTrack, Rec Movi Pos = %llx\n",
                        prSwdmxAviInfo->u8RecDmxMoviPos);
                    LOG(5, "Change SubTrack, Cur Dmx Pos = %llx\n",
                        prSwdmxAviInfo->u8CurDmxMoviPos);
                    LOG(5, "VDP Pts = 0x%08x\n", u4CurPts);
                } 
            }
#ifdef CC_DLNA_SUPPORT
        }
        else
        {
            LOG(5, "Input Source is not MM_SRC_TYPE_HIGH_SPEED_STORAGE\n");

            if (prSwdmxAviInfo->fgChgSbTrack)
            {
                prSwdmxAviInfo->fgChgSbTrack = FALSE;
                LOG(5, "Do nothing for Sub track switch\n");
            }
        }
#endif
        if ((prSwdmxAviInfo->fgChgATrack) &&
            (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack))
        {   
            BOOL fgRet = FALSE;
            UINT32 u4APts = 0;
       
            DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);

            prSwdmxAviInfo->fgChgATrack = FALSE;

            if (!prSwdmxAviInfo->fgRecSpeedChangeForAud)
            {
                u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
                prSwdmxAviInfo->u4TriggerAudPts = u4CurPts + 27000;

                if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
                {
                    fgRet = DMX_AudHandler_SeekAndUpdate(prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx],
                                                         prSwdmxAviInfo->u4TriggerAudPts, &u4APts);
                }

                LOG(5, "Change Audio Track\n");

                if (fgRet)
                {
                    LOG(5, "Find audio in the pre-buffer\n");
                    prSwdmxAviInfo->u4TriggerAudPts = u4APts;
                }
                else
                {
                    LOG(5, "Can't find audio in the pre-buffer\n");
                    prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];
                }

                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4TriggerAudPts);
                
                if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
                {
                     DMX_AudHandler_SetActivePidx(prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx]);
                     prSwdmxAviInfo->fgSetActivePid = TRUE;
                }
                if(prSwdmxAviInfo->u8CurDmxMoviPos >= prSwdmxInst->u8FileSize)
                {
                    prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->prDmxRangeInfo->ui8_vid_duration;
                }
                LOG(5, "prSwdmxAviInfo->u4TriggerAudPts = 0x%08x\n", prSwdmxAviInfo->u4TriggerAudPts);
            }
        }
    }

    if(prSwdmxAviInfo->fgChgATrack)
    {
        prSwdmxAviInfo->fgChgATrack = FALSE;
    }
    
    if (prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_IFRAME)
    {
        u4PrsState = _SWDMX_AviHdrPrsUseIdx(u1SrcId,
                                                                prRangeList, 
                                                                i4PlaySpeed);
    }
    else if(prSwdmxAviInfo->fgIs3DFile && (i4PlaySpeed == 2000))
    {
       u4PrsState = _SWDMX_AviHdrPrsUseIdx(u1SrcId,
                                                               prRangeList, 
                                                               i4PlaySpeed);
    }
    else
    {
        u4PrsState = _SWDMX_AviHdrPrsWithoutIdx(u1SrcId,
                                                                       prRangeList,
                                                                       i4PlaySpeed,
                                                                       fgSpeedChange);
    }

    prSwdmxAviInfo->i4PrevSpeed = i4PlaySpeed;
    return (SWDMX_HDR_PRS_STATE_T)u4PrsState;
}

static BOOL _SWDMX_AviHandleSpeedChange(
    SWDMX_INFO_T *prSwdmxInst,
    SWDMX_CUR_CMD_T* prCmd,
    SWDMX_RANGE_LIST_T* prRangeList,
    INT32 i4PlaySpeed)
{
    UINT8 i = 0;
    UINT32 u4CurPts = 0;
    UINT64 u8SeekPos = prCmd->u8SeekPos;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    VDEC_PTS_INFO_T rPTSInfo ;
    VDEC_PTS_INFO_T rSubPTSInfo ;

    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

    prSwdmxAviInfo->fgSpeedChange = TRUE;

    u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
    LOG(5, "Speed change while VDP Pts = 0x%08x\n", u4CurPts);
    LOG(5, "Speed change %d -> %d\n",prSwdmxAviInfo->i4PrevSpeed,i4PlaySpeed);

    prSwdmxAviInfo->u4NumSentSub = 0;
    prSwdmxAviInfo->u4NumSentAud = 0;

    // 1X -> 2X
    if ((prSwdmxAviInfo->i4PrevSpeed == 1000) &&
        (i4PlaySpeed == 2000))
    {
        if (prSwdmxAviInfo->u8RecDmxMoviPos != 0)
        {
            _SWDMX_FlushReqMsgQ(prSwdmxInst->u1SwdmxId);
            prSwdmxAviInfo->i4ReqReadTimes = 0;
            prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
            prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxAviInfo->u8RecDmxMoviPos;
        }
    }

    prSwdmxAviInfo->u8RecDmxMoviPos = 0;
    //prSwdmxAviInfo->u4ModeChangPts = 0xFFFFFFFF;
    prSwdmxAviInfo->u4ChangeMode = 0;
    prSwdmxAviInfo->fgRecSpeedChangeForAud = FALSE;
    prSwdmxAviInfo->u4VidRepeatFrame = 0;
    prSwdmxAviInfo->fgRequestData = TRUE;
    prSwdmxAviInfo->fgPreviouIsSeek = FALSE;
    prSwdmxAviInfo->u8VSeekPts = 0xFFFFFFFF;
    prSwdmxAviInfo->u8VSeekPts <<= 32;
    prSwdmxAviInfo->u8VSeekPts += 0xFFFFFFFF;

    prSwdmxAviInfo->u8ASeekPts = 0xFFFFFFFF;
    prSwdmxAviInfo->u8ASeekPts <<= 32;
    prSwdmxAviInfo->u8ASeekPts += 0xFFFFFFFF;
    prSwdmxAviInfo->fgQAud = FALSE;
    prSwdmxAviInfo->u4QAudAddr = 0;
    prSwdmxAviInfo->u8VRemainder = 0;

    prSwdmxAviInfo->fgAbRepeat = FALSE; 
    prSwdmxAviInfo->u4AbChunkSize = 0;
    prSwdmxAviInfo->u4AbDmxPos = 0;
    prSwdmxAviInfo->u4AbPts = 0;

    prSwdmxAviInfo->u8PrevSubPos = 0xFFFFFFFF;
    prSwdmxAviInfo->u8PrevSubPos <<= 32;
    prSwdmxAviInfo->u8PrevSubPos += 0xFFFFFFFF;

    prSwdmxAviInfo->fgReqDataMiss = FALSE;
    prSwdmxAviInfo->i4ReqReadTimes = 0;
    prSwdmxAviInfo->u4FeederReqID += 1;
    prSwdmxAviInfo->u4FeederReqReceiveID = prSwdmxAviInfo->u4FeederReqID;
    
    if (prSwdmxAviInfo->fgIs3DFile)
    {
        prSwdmxAviInfo->u4SubVidRepeatFrame = 0;
        prSwdmxAviInfo->fgSubVideoRequestData = TRUE;
        prSwdmxAviInfo->u8VRRemainder = 0;
        prSwdmxAviInfo->fgReqSubDataMiss = FALSE;
    }
    
     // is not the case 1X < -> 2X
    if (!(((prSwdmxAviInfo->i4PrevSpeed == 1000) && (i4PlaySpeed == 2000)) ||
          ((prSwdmxAviInfo->i4PrevSpeed == 2000) && (i4PlaySpeed == 1000))))
    {
        for (i = 0; i < prSwdmxAviInfo->u1TotalATrack; i++)
        {
            prSwdmxAviInfo->u8ARemainder[i] = 0;
        }
    }

    if(prSwdmxAviInfo->fgVDecIsPause)
    {
        _SWDMX_AviVideoWakeUp(prSwdmxInst);
        prSwdmxAviInfo->fgVDecIsPause = FALSE;
    }
    x_memset((void*)&rPTSInfo, 0, sizeof(VDEC_PTS_INFO_T));
    
    if(prSwdmxAviInfo->prVidIdxTbl)
    {
        rPTSInfo.u8Scale = (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD;
        rPTSInfo.u4Rate = prSwdmxAviInfo->prVidIdxTbl->ui4_rate;
    } 
    if(prSwdmxAviInfo->fgIs3DFile)
    { 
        x_memset((void*)&rSubPTSInfo, 0, sizeof(VDEC_PTS_INFO_T));
        if(prSwdmxAviInfo->prSubVidIdxTbl)
        {
             rSubPTSInfo.u8Scale = (UINT64)prSwdmxAviInfo->prSubVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD;
             rSubPTSInfo.u4Rate = prSwdmxAviInfo->prSubVidIdxTbl->ui4_rate;
        }
    }
    
    if((prCmd->fgSeek) && (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET))
    {
        _SWDMX_AviABRepeat(prSwdmxInst,&u8SeekPos,prCmd);
    }
    // seek
    if(((UINT32)(u8SeekPos>>32) != 0xFFFFFFFF)
        || ((UINT32)(u8SeekPos) != 0xFFFFFFFF))
    {
        _SWDMX_AviTimeSeek(prSwdmxInst,u8SeekPos,i4PlaySpeed,prRangeList);
    }
    else if(prSwdmxInst->ePlayMode == SWDMX_PLAY_MODE_NORMAL &&
		    prSwdmxInst->ePrePlayMode == SWDMX_PLAY_MODE_NORMAL)
    {
        UINT8 u1TrackIdx = prSwdmxAviInfo->u1CurATrackIdx;
        UINT32 u4Pts=0,u1Idx=0;
        if (prSwdmxAviInfo->fgIs3DFile)
        {
            if ((i4PlaySpeed == 2000) ||
                ((prSwdmxAviInfo->i4PrevSpeed == 2000) && (i4PlaySpeed == 1000)))
            {
                UINT32 u4SubPts = 0;
                if(prSwdmxAviInfo->fgKeyExist)
                {
		            UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));
    			    prSwdmxAviInfo->u4PreRepeatFrameIdx = prSwdmxAviInfo->u4CurRepeatFrameIdx;
    			    prSwdmxAviInfo->u4PreRepeatSubFrameIdx = prSwdmxAviInfo->u4CurRepeatSubFrameIdx;
                    u4SubPts=prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentSubKey].ui4_pts;
    			    u4Pts=prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_pts;
		        }
		        else if(prSwdmxAviInfo->fgEnVDmx)
		        {
                    _SWDMX_AviSeekCurPos(prSwdmxInst);
			        u4Pts=prSwdmxAviInfo->u4CurVidPts;
			        u4SubPts=u4Pts;
		        }
                
                MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, TRUE);
                VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);                   
                VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id);
         
                _SWDMX_FlushReqMsgQ(prSwdmxInst->u1SwdmxId);
                
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                
                if((prSwdmxAviInfo->i4PrevSpeed == 2000) && (i4PlaySpeed == 1000))
                {
                     UINT8 u1TrackIdx;
                     UINT8 u1Idx = 0;
                     UINT32 u4APts = 0;
                                              
                     DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);
                     DMX_MM_FlushBuffer(prSwdmxAviInfo->u1VidPid);
                     DMX_MM_FlushBuffer(prSwdmxAviInfo->u1Vid2Pid);
                     
                     VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
                     VDEC_StartPTS(prSwdmxInst->u1Vdec2Id,TRUE,u4SubPts,&rPTSInfo);
                     
                     LOG(0, "######## The current pts is : %x\n", u4CurPts);
                     
                     VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                     VDEC_StartPTS(prSwdmxInst->u1VdecId,TRUE,u4Pts,&rPTSInfo);
                     
                     for ( ;(u1Idx < prSwdmxAviInfo->u1TotalATrack)&&(u1Idx<AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
                     {
                         DMX_AudHandler_SeekAndUpdate(prSwdmxAviInfo->u1AudPreBufPid[u1Idx], 0xffffffff, &u4APts);
                     }
                     u1TrackIdx = prSwdmxAviInfo->u1CurATrackIdx;
                     if(u1TrackIdx<prSwdmxAviInfo->u1TotalATrack)
         		     {
         		         prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
         		     }
                     if(!prSwdmxAviInfo->fgMp3Abr)
                     { 
                         AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);
                     }
                     else
                     {
                         AUD_SetStartPts(prSwdmxInst->u1AdecId,u4CurPts);
                     }
                     prSwdmxAviInfo->u4AudStartPts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];
                }
           }
        }
        
        u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

        if(prSwdmxAviInfo->i4PrevSpeed == 1)
        {
            STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
        }

        if ((prSwdmxAviInfo->i4PrevSpeed == 1) || 
            ((prSwdmxAviInfo->i4PrevSpeed < 1000) && (prSwdmxAviInfo->i4PrevSpeed > 1)))
        {
            STC_StartStc(prSwdmxInst->u1StcId);
        }
        
        if (i4PlaySpeed<=2000&&prSwdmxAviInfo->i4PrevSpeed>2000&&prSwdmxAviInfo->fgKeyExist==FALSE)
        {
            if(prSwdmxAviInfo->fgEnVDmx)
         	{
                _SWDMX_AviSeekCurPos(prSwdmxInst);
            }
       	 
            _SWDMX_AviFlush(prSwdmxInst);
   	  
       	    for (;(u1Idx < prSwdmxAviInfo->u1TotalATrack)&&(u1Idx<AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
       	    {
       			DMX_AudHandler_SeekAndUpdate(prSwdmxAviInfo->u1AudPreBufPid[u1Idx], 0xffffffff, &u4Pts);
       	    }
   								
       	    if(prSwdmxAviInfo->fgIs3DFile)
       	    {
       		    VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
       		    VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE,prSwdmxAviInfo->u4CurVidPts, &rPTSInfo);
       	    }
       	    
       	    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
       	    VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, prSwdmxAviInfo->u4CurVidPts,&rPTSInfo);
       	   
       	    if(prSwdmxAviInfo->fgMp3Abr)
       	    {
       		    prSwdmxAviInfo->u8VSeekPts = u4CurPts;
       	    }
       	    if(u1TrackIdx<prSwdmxAviInfo->u1TotalATrack)
       	    {
       		    AUD_SetStartPts(prSwdmxInst->u1AdecId,prSwdmxAviInfo->u4AudPts[u1TrackIdx]);
       		    DMX_AudHandler_SetActivePidx(prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx]);
       		    prSwdmxAviInfo->fgSetActivePid = TRUE;
       	    }
   			prSwdmxAviInfo->u4TriggerAudPts = u4CurPts + 27000;
      }
      else if ((u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)&&(i4PlaySpeed == 1000))
      {
          BOOL fgRet;
          UINT32 u4APts = 0;
          UINT32 u4AVDelta = 0;
          UINT32 u8SeekPos = 0;
  
          LOG(5, "Avi Speed 2X/0.5X -> 1X Cur Vid Pts = 0x%08x, Cur Aud Pts = 0x%08x\n",
          prSwdmxAviInfo->u4CurVidPts, prSwdmxAviInfo->u4AudPts[u1TrackIdx]);
  
          DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);
  
          prSwdmxAviInfo->u4TriggerAudPts = u4CurPts + 27000;
     
          fgRet = DMX_AudHandler_SeekAndUpdate(prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx],
                                               prSwdmxAviInfo->u4TriggerAudPts, &u4APts);
  
          LOG(5, "Find Audio Track Data for 2X/0.5X -> 1X\n");
          
          if (fgRet)
          {
              u4AVDelta = u4APts - u4CurPts;
              LOG(5, "Find audio in the pre-buffer\n");
                /*add this for some file audio chunk is larger ,lose audio about 2-3s after FF2X or seek after pause */    
              if(((prSwdmxAviInfo->fgPauseToSeek && (u4AVDelta > DEMUX_AVI_AV_PERIOD)) ||
                  (u4AVDelta > DEMUX_AVI_AV_PERIOD)) && 
                  prSwdmxAviInfo->fgKeyExist)
              {
                  
                  if(prSwdmxAviInfo->fgPauseToSeek && (u4AVDelta > DEMUX_AVI_AV_PERIOD) && 
                      (prSwdmxAviInfo->i4PrevSpeed == 1))
                  {
                      u8SeekPos = prSwdmxAviInfo->u8PauseToSeekPos;
                  }
                  else if(u4AVDelta > DEMUX_AVI_AV_PERIOD)
                  {
                      u8SeekPos = u4CurPts;
                  }
  
                  LOG(5, "Avi Speed 2X/0.5X -> Seek to CurPts %llu.\n", u8SeekPos);
                  UNUSED(_SWDMX_AviSeek(prSwdmxInst, u8SeekPos, prRangeList));
  
                  prSwdmxAviInfo->u8VSeekPts = u8SeekPos;
                  prSwdmxAviInfo->u8ASeekPts = u8SeekPos;
                  prSwdmxAviInfo->fgFeederInvalid = TRUE;
                  prSwdmxAviInfo->fgRecSpeedChangeForAud = TRUE;
  
                  if (prSwdmxAviInfo->fgEnVideo)
                  {
                      MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                      VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                      DMX_MM_FlushBuffer(prSwdmxAviInfo->u1VidPid);
                      
                      VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                      VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, u4CurPts, NULL);
                  
                      if (prSwdmxAviInfo->fgIs3DFile)
                      {
                          MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, TRUE);
                          VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id);
                          DMX_MM_FlushBuffer(prSwdmxAviInfo->u1Vid2Pid);
  
                          VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
                          VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, u4CurPts, NULL);
                      }
                  }                         
                  DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);
                  STC_StopStc(prSwdmxInst->u1StcId);
                  STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                  
                  prSwdmxAviInfo->fgPauseToSeek = FALSE;
              }
              
              prSwdmxAviInfo->u4TriggerAudPts = u4APts;
              
          }
          else
          {
              LOG(5, "Can't find audio in the pre-buffer\n");
              prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
          }
  
          if(u4AVDelta < DEMUX_AVI_AV_PERIOD || !prSwdmxAviInfo->fgKeyExist)
          {
              AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4TriggerAudPts);
  
              DMX_AudHandler_SetActivePidx(prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx]);
              
              prSwdmxAviInfo->fgSetActivePid = TRUE;
  
              prSwdmxAviInfo->i4RecPrevSpeedForAud = prSwdmxAviInfo->i4PrevSpeed;
  
              LOG(5, "Cur Dmx Pos = %ld, \n",
                  prSwdmxAviInfo->u8CurDmxMoviPos);
          }
          if(prSwdmxAviInfo->u8CurDmxMoviPos >= prSwdmxInst->u8FileSize)
          {
              UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));

              if(prSwdmxAviInfo->u8CurDmxMoviPos < prSwdmxInst->u8FileSize)
              {
                  _SWDMX_AviFlush(prSwdmxInst);
                  prSwdmxAviInfo->u8VSeekPts = u4CurPts;
                  prSwdmxAviInfo->u8ASeekPts = u4CurPts;
                  prSwdmxAviInfo->fgFeederInvalid = TRUE;
      
                  if (prSwdmxAviInfo->fgEnVideo)
                  {                
                      VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                      VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, u4CurPts, NULL);
                  
                      if (prSwdmxAviInfo->fgIs3DFile)
                      {
                          VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
                          VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, u4CurPts, NULL);
                      }
                  }                         
                  STC_StopStc(prSwdmxInst->u1StcId);
                  STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                  prSwdmxAviInfo->u4TriggerAudPts = u4CurPts + 27000;
              }
              else
              {
                  prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->prDmxRangeInfo->ui8_vid_duration;
              }
          }
          LOG(5, "set audio trigger pts = 0x%x.\n", prSwdmxAviInfo->u4TriggerAudPts);
      }
      else
      {
          LOG(0, "Out of Max Audio Track Number!!!\n");
      }
    }
    //is the case FB -> normal / FF2X
    else if (prSwdmxAviInfo->i4PrevSpeed < 0 &&
        (i4PlaySpeed == 2000 || i4PlaySpeed == 1000))
    {
        UINT8 u1Idx = 0;
        if(prSwdmxAviInfo->fgKeyExist)
        {
	        UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));
        }
        else if(prSwdmxAviInfo->fgEnVDmx)
        {
            _SWDMX_AviSeekCurPos(prSwdmxInst);
        }
        
        _SWDMX_AviFlush(prSwdmxInst);
         
        for ( ;(u1Idx < prSwdmxAviInfo->u1TotalATrack)&&(u1Idx<AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
        {
            DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPreBufPid[u1Idx]);
        }
          
        if (prSwdmxAviInfo->fgEnVideo)
        { 
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, 
                    prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentSubKey].ui4_pts);
            }
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 
                prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_pts);
            
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, 
                    prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentSubKey].ui4_pts, 
                    &rPTSInfo);
            }
            VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, 
                prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_pts, 
                &rPTSInfo);
            if(prSwdmxAviInfo->fgMp3Abr)
            {
    	         prSwdmxAviInfo->u8VSeekPts = u4CurPts;
            } 	      
        }

        if ((i4PlaySpeed == 1000) && 
            (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack))
        {
            prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];

            LOG(5, "VDP trigger audio = 0x%08x\n", 
                prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);

            if(!prSwdmxAviInfo->fgMp3Abr)
            {	
                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);
            }
            else
            {
                AUD_SetStartPts(prSwdmxInst->u1AdecId,u4CurPts);
            }
            
            prSwdmxAviInfo->u4AudStartPts =  prSwdmxAviInfo->u4AudPts[ prSwdmxAviInfo->u1CurATrackIdx];
            prSwdmxAviInfo->fgRecSpeedChangeForAud = TRUE;
            prSwdmxAviInfo->i4RecPrevSpeedForAud = prSwdmxAviInfo->i4PrevSpeed;

            // set stc
        }

        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
		
        if(i4PlaySpeed == 2000)
        {
            STC_StartStc(prSwdmxInst->u1StcId);
        }
        LOG(5, "Avi Speed -32X/-16X/-8X/-4X/---->2X - 1X\n");
    }
    // the case ( 2X->4X->8X->16X->32X) ||(all -> -2X...->-32X) ||(>=4X ->1X )
    else if (!(prSwdmxAviInfo->i4PrevSpeed == 1000 && (i4PlaySpeed == 2000 || i4PlaySpeed == 1)))
    {
        UINT32 u4Pts=0;
        UINT32 u4SubPts=0;
        
        if (!(((prSwdmxAviInfo->i4PrevSpeed == 1)&&(i4PlaySpeed == 2000))||
           ( (prSwdmxAviInfo->i4PrevSpeed == 2000 ||
           (prSwdmxAviInfo->i4PrevSpeed == 1) )&&
           ((i4PlaySpeed<1000)&&(i4PlaySpeed>1)))))
         {
             UINT8 u1Idx = 0;
             if(prSwdmxAviInfo->fgKeyExist)
             {
                 UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));
         	     u4Pts=prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_pts;
         	     if(prSwdmxAviInfo->fgIs3DFile)
         	     {
         	         u4SubPts = 
         	            prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentSubKey].ui4_pts;
         	     }
             }
             else if(prSwdmxAviInfo->fgEnVDmx)
             {
                   _SWDMX_AviSeekCurPos(prSwdmxInst);
         	    u4Pts=prSwdmxAviInfo->u4CurVidPts;
             }
        
              _SWDMX_AviFlush(prSwdmxInst);
              
             for ( ;(u1Idx < prSwdmxAviInfo->u1TotalATrack)&&(u1Idx<AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
             {
                  DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPreBufPid[u1Idx]);
             }
 				
	         if (i4PlaySpeed > 1000)
             {
                 if(prSwdmxAviInfo->fgIs3DFile)
                 {
                     VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
                     VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, u4SubPts, &rPTSInfo);
                 }
                 VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                 //avc test
                 VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, u4Pts, &rPTSInfo);
             }
             else if (i4PlaySpeed <= 1000 && i4PlaySpeed > 0)
             {
                 if(prSwdmxAviInfo->fgIs3DFile)
                 {
                      VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
                      VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, u4SubPts, &rPTSInfo);
                 }
                 VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                 //avc test
                 VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, u4Pts, &rPTSInfo);
                      
                 if(prSwdmxAviInfo->fgMp3Abr)
                 {
            	     prSwdmxAviInfo->u8VSeekPts = u4CurPts;
                 }	   
            }
            else
            {						  
                if(prSwdmxAviInfo->fgIs3DFile)
                {
                    VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, 
                        prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts);
                    VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, 
                        prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts, 
                        &rPTSInfo);  
                }
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 
                    prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts);
                VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, 
                    prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[0].ui4_pts, 
                    &rPTSInfo);
        	  }
      }
      else
      {
	    STC_StartStc(prSwdmxInst->u1StcId);
      }

      if (i4PlaySpeed > 2000 || i4PlaySpeed < 0)
      {
          prSwdmxAviInfo->fgStartTrick = TRUE;
      }
      // Things for 32/16/8/4/-2/-4/-8/-16/-32 -> 1X are the same
      if ((prSwdmxAviInfo->i4PrevSpeed > 2000 || prSwdmxAviInfo->i4PrevSpeed < 0) &&
          ((i4PlaySpeed == 1000)||((i4PlaySpeed<1000)&&(i4PlaySpeed>1))) &&
          (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack))
      {
          UINT8 u1TrackIdx;
          UINT8 u1Idx = 0;
          
          DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);

          for ( ;(u1Idx < prSwdmxAviInfo->u1TotalATrack)&&(u1Idx<AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
          {
               DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPreBufPid[u1Idx]);
          }
          u1TrackIdx = prSwdmxAviInfo->u1CurATrackIdx;
          prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
          if(!prSwdmxAviInfo->fgMp3Abr)
          {	
              AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);
          }
          else
          {
              AUD_SetStartPts(prSwdmxInst->u1AdecId,u4CurPts);
          }
          prSwdmxAviInfo->u4AudStartPts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];

          // set stc
          STC_StopStc(prSwdmxInst->u1StcId);
          STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
      }
      LOG(5, "prSwdmxAviInfo->i4PrevSpeed != 1000 i4PlaySpeed != 2000\n");
      // don't need to handle 1X->2X
    }
    prSwdmxAviInfo->i4PrevSpeed = i4PlaySpeed;
    return TRUE;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_AviHdrPrsUseIdx(
     UINT8 u1SrcId,
     SWDMX_RANGE_LIST_T* prRangeList,
     INT32 i4PlaySpeed)
{
    BOOL fgVRangeDone = TRUE;    
    UINT32 *pu4CurIdx = NULL;
    MM_RANGE_ELMT_IDXTBL_T *prKeyTbl = NULL;
    
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    SWDMX_HDR_PRS_STATE_T u4Prs3DState;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
        
    if (prSwdmxAviInfo->fgFeederInvalid)
    {
        _SWDMX_AviResetFeeder(prSwdmxInst);
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if (prSwdmxAviInfo->fgDeliverVEos)
    {
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
        return eSWDMX_HDR_PRS_SUCCEED;
    }
     
    if (prSwdmxAviInfo->prCurVidKeyIdxTbl == 0)
    {
        LOG(1, "There is no avi key table\n");
        return eSWDMX_HDR_PRS_EMPTY_RANGE;
    }

    prKeyTbl = prSwdmxAviInfo->prCurVidKeyIdxTbl;
    if(prSwdmxAviInfo->fgIs3DFile &&(prSwdmxAviInfo->fgIsSupport3DTrick))
    {
        u4Prs3DState = _SWDMX_Avi3DHdrPrsUseIdx(u1SrcId,prRangeList,i4PlaySpeed,prKeyTbl);
        if (eSWDMX_HDR_PRS_SUCCEED != u4Prs3DState)
        {
            return u4Prs3DState;
        }
    }
    else
    {
        if (prSwdmxAviInfo->u4CurSentKey < prSwdmxAviInfo->prCurVidKeyIdxTbl->ui4_number_of_entry)
        {
           prSwdmxAviInfo->u4CurDmxFeederRPtr = prSwdmxInst->rFeederInfo.u4WriteAddr;
           // Update read ptr to feeder
           FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));
           fgVRangeDone = FALSE;
           pu4CurIdx = &prSwdmxAviInfo->u4CurSentKey;
           prSwdmxAviInfo->u4CurVidPts = prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
   
           LOG(5, "prSwdmxAviInfo->u4CurSentKey = %ld\n", prSwdmxAviInfo->u4CurSentKey);
           LOG(5, "Key Pts = %ld\n", prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts);
   
           _SWDMX_DeliverAviDataUseIdx(u1SrcId,
                                       i4PlaySpeed, 
                                       prSwdmxAviInfo->prCurVidKeyIdxTbl, 
                                       pu4CurIdx, 
                                       prRangeList,
                                       prSwdmxInst->u1VdecId);
   
           if ((i4PlaySpeed < 0) && (prSwdmxAviInfo->u4CurSentKey == 0))
           {
               LOG(5, "Fast Backward to the first key fram\n");
           }
   
           LOG(7, "Current key frame movi position = 0x%llx\n", prSwdmxAviInfo->u8CurMoviPos);
       }
       else
       {
           prSwdmxAviInfo->u4VidRepeatFrame = 0;
       }
   
   // The last key frame in current node
       if (prSwdmxAviInfo->u4VidRepeatFrame == 0)
       {
           if (i4PlaySpeed > 2000 && 
           (prSwdmxAviInfo->u4CurSentKey == prSwdmxAviInfo->prCurVidKeyIdxTbl->ui4_number_of_entry))
           {
               if (prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial)
               {
                   prSwdmxAviInfo->prCurVidKeyIdxTbl = prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial;
                   prSwdmxAviInfo->u4CurSentKey = 0;
                   prSwdmxAviInfo->u4CurRepeatFrameIdx = 0;
               }
               else if ((prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial == NULL) &&
                   (prSwdmxAviInfo->u4CurRepeatFrameIdx == prKeyTbl->ui4_number_of_entry - 1) &&
                   !prSwdmxAviInfo->fgDeliverVEos)
               {
                   LOG(5, "Fast Forward _SWDMX_AviHdrPrsUseIdx Deliver Video Eos\n");
                       if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId))
                   {
                       LOG(3, "Fail to send Video Eos\n");
                       return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                   }
                   prSwdmxAviInfo->fgDeliverVEos = TRUE;
               }
           }
           else if (i4PlaySpeed < 0 && (prSwdmxAviInfo->u4CurSentKey == 0))
           {
               if (prSwdmxAviInfo->prCurVidKeyIdxTbl != prSwdmxAviInfo->prVidKeyIdxTbl)
               {
                   MM_RANGE_ELMT_IDXTBL_T *prPrevKeyTbl = prSwdmxAviInfo->prVidKeyIdxTbl;
   
                   while (prPrevKeyTbl->pt_next_partial != prSwdmxAviInfo->prCurVidKeyIdxTbl)
                   {
                       prPrevKeyTbl = prPrevKeyTbl->pt_next_partial;
                   }
                   
                   prSwdmxAviInfo->prCurVidKeyIdxTbl = prPrevKeyTbl;
                   prSwdmxAviInfo->u4CurSentKey = prPrevKeyTbl->ui4_number_of_entry - 1;
                   prSwdmxAviInfo->u4CurRepeatFrameIdx = prPrevKeyTbl->ui4_number_of_entry - 1;
               }
               else if ((prSwdmxAviInfo->prCurVidKeyIdxTbl == prSwdmxAviInfo->prVidKeyIdxTbl) &&
                           (prSwdmxAviInfo->u4CurRepeatFrameIdx == 0) &&
                           !prSwdmxAviInfo->fgDeliverVEos)
               {
                   LOG(5, "Fast Backward _SWDMX_AviHdrPrsUseIdx Deliver Video Eos\n");
                   if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId))
                   {
                       return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                   }
                   prSwdmxAviInfo->fgDeliverVEos = TRUE;
               }
          }
      }
   
       if (fgVRangeDone)  
       {
           prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
       }
   }
    
    return eSWDMX_HDR_PRS_SUCCEED;
}        

static SWDMX_HDR_PRS_STATE_T _SWDMX_Avi3DHdrPrsUseIdx(
        UINT8 u1SrcId,
        SWDMX_RANGE_LIST_T* prRangeList, 
        INT32 i4PlaySpeed,
        MM_RANGE_ELMT_IDXTBL_T *prKeyTbl)
{
     UINT32 *pu4CurIdx = NULL;
     SWDMX_HDR_PRS_STATE_T u4PrsState;
     BOOL fgVSubRangeDone = TRUE;
     UINT32 *pu4CurSubIdx = NULL;
     MM_RANGE_ELMT_IDXTBL_T *prSubKeyTbl = NULL;
     SWDMX_INFO_T* prSwdmxInst = NULL;
     SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;

     prSwdmxInst = _SWDMX_GetInst(u1SrcId);
     if (!prSwdmxInst)
     {
         return eSWDMX_HDR_UNKNOWN_TYPE;
     }
     prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

     prSubKeyTbl = prSwdmxAviInfo->prCurSubVidKeyIdxTbl;
     if (prSwdmxAviInfo->prCurSubVidKeyIdxTbl ==0)
     {
         LOG(1, "There is no avi key table\n");
         return eSWDMX_HDR_PRS_EMPTY_RANGE;
     }

    if ((prSwdmxAviInfo->u4CurSentKey < prSwdmxAviInfo->prCurVidKeyIdxTbl->ui4_number_of_entry) ||
         (prSwdmxAviInfo->u4CurSentSubKey < prSwdmxAviInfo->prCurSubVidKeyIdxTbl->ui4_number_of_entry))
    {
        prSwdmxAviInfo->u4CurDmxFeederRPtr = prSwdmxInst->rFeederInfo.u4WriteAddr;
        // Update read ptr to feeder
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));
        
        fgVSubRangeDone = FALSE;
        pu4CurIdx = &prSwdmxAviInfo->u4CurSentKey;
        pu4CurSubIdx = &prSwdmxAviInfo->u4CurSentSubKey;
        prSwdmxAviInfo->u4CurVidPts = prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts;
        prSwdmxAviInfo->u4CurSubVidPts = prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurSubIdx].ui4_pts;

        LOG(7, "prSwdmxAviInfo->u4CurSentKey = %ld\n", prSwdmxAviInfo->u4CurSentKey);
        LOG(7, "prSwdmxAviInfo->u4CurSentSubKey = %ld\n", prSwdmxAviInfo->u4CurSentSubKey);
        LOG(7, "Key Pts = %ld\n", prSwdmxAviInfo->prCurVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_pts);
        LOG(7, "SubKey Pts = %ld\n", prSwdmxAviInfo->prCurSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurSubIdx].ui4_pts);

        if (prSwdmxAviInfo->fgIsLeftViewDone == FALSE)
        {
            u4PrsState = _SWDMX_DeliverAviDataUseIdx(u1SrcId,
                                                     i4PlaySpeed, 
                                                     prSwdmxAviInfo->prCurVidKeyIdxTbl, 
                                                     pu4CurIdx, 
                                                     prRangeList,
                                                     prSwdmxInst->u1VdecId);
                
            if (u4PrsState != eSWDMX_HDR_PRS_SUCCEED)
            {
                return u4PrsState;
            }
            else
            {
                prSwdmxAviInfo->fgIsLeftViewDone = TRUE;
            }
        }
        else
        {
             u4PrsState = _SWDMX_DeliverAviDataUseIdx(u1SrcId,
                                                      i4PlaySpeed, 
                                                      prSwdmxAviInfo->prCurSubVidKeyIdxTbl, 
                                                      pu4CurSubIdx, 
                                                      prRangeList,
                                                      prSwdmxInst->u1Vdec2Id);
                
             if (u4PrsState != eSWDMX_HDR_PRS_SUCCEED)
             {
                 return u4PrsState;
             }
             else
             {
               prSwdmxAviInfo->fgIsLeftViewDone = FALSE;
             }
        }
        
        if ((i4PlaySpeed < 0) &&
            ((prSwdmxAviInfo->u4CurSentKey == 0) ||(prSwdmxAviInfo->u4CurSentSubKey == 0)))
        {
            LOG(5, "Fast Backward to the first key fram\n");
        }
        LOG(7, "Current key frame movi position = 0x%llx\n", prSwdmxAviInfo->u8CurMoviPos);
    }
    else
    {
        prSwdmxAviInfo->u4VidRepeatFrame = 0;
        prSwdmxAviInfo->u4SubVidRepeatFrame = 0;           
    }    
    
    if ((prSwdmxAviInfo->u4VidRepeatFrame == 0) && (prSwdmxAviInfo->u4SubVidRepeatFrame == 0))
    {
        if (i4PlaySpeed >= 2000 && 
            (prSwdmxAviInfo->u4CurSentKey == prSwdmxAviInfo->prCurVidKeyIdxTbl->ui4_number_of_entry) &&
            (prSwdmxAviInfo->u4CurSentSubKey == prSwdmxAviInfo->prCurSubVidKeyIdxTbl->ui4_number_of_entry))
        {
            if (prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial &&
               prSwdmxAviInfo->prCurSubVidKeyIdxTbl->pt_next_partial )
            {
                prSwdmxAviInfo->prCurVidKeyIdxTbl = prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial;
                prSwdmxAviInfo->u4CurSentKey = 0;
                prSwdmxAviInfo->u4CurRepeatFrameIdx = 0;
                
                prSwdmxAviInfo->prCurSubVidKeyIdxTbl = prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial;
                prSwdmxAviInfo->u4CurSentSubKey = 0;
                prSwdmxAviInfo->u4SubVidRepeatFrame = 0;
            }
            else if ((prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial == NULL) &&
                (prSwdmxAviInfo->u4CurRepeatFrameIdx == prKeyTbl->ui4_number_of_entry - 1) &&
                (prSwdmxAviInfo->prCurVidKeyIdxTbl->pt_next_partial == NULL) &&
                (prSwdmxAviInfo->u4CurRepeatSubFrameIdx == prSubKeyTbl->ui4_number_of_entry - 1) &&
                !prSwdmxAviInfo->fgDeliverVEos)
            {
                LOG(5, "Fast Forward _SWDMX_AviHdrPrsUseIdx Deliver Video Eos\n");
                if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId))
                {
                    LOG(3, "Fail to send Video Eos\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                 if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1Vdec2Id))
                {
                    LOG(3, "Fail to send Video Eos\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                prSwdmxAviInfo->fgDeliverVEos = TRUE;
            }
        }
        else if (i4PlaySpeed < 0 && (prSwdmxAviInfo->u4CurSentKey == 0) &&
            (prSwdmxAviInfo->u4CurSentSubKey == 0))
        {
            if ((prSwdmxAviInfo->prCurVidKeyIdxTbl != prSwdmxAviInfo->prVidKeyIdxTbl) ||
                (prSwdmxAviInfo->prCurSubVidKeyIdxTbl != prSwdmxAviInfo->prSubVidKeyIdxTbl))
            {
                MM_RANGE_ELMT_IDXTBL_T *prPrevKeyTbl = prSwdmxAviInfo->prVidKeyIdxTbl;
                MM_RANGE_ELMT_IDXTBL_T *prSubPrevKeyTbl = prSwdmxAviInfo->prSubVidKeyIdxTbl;
                while (prSubPrevKeyTbl->pt_next_partial != prSwdmxAviInfo->prCurSubVidKeyIdxTbl)
                {
                    prPrevKeyTbl = prPrevKeyTbl->pt_next_partial;
                }
                while (prPrevKeyTbl->pt_next_partial != prSwdmxAviInfo->prCurVidKeyIdxTbl)
                {
                    prPrevKeyTbl = prPrevKeyTbl->pt_next_partial;
                }
                prSwdmxAviInfo->prCurVidKeyIdxTbl = prPrevKeyTbl;
                prSwdmxAviInfo->u4CurSentKey = prPrevKeyTbl->ui4_number_of_entry - 1;
                prSwdmxAviInfo->u4CurRepeatFrameIdx = prPrevKeyTbl->ui4_number_of_entry - 1;
                
                prSwdmxAviInfo->prCurSubVidKeyIdxTbl = prPrevKeyTbl;
                prSwdmxAviInfo->u4CurSentSubKey = prPrevKeyTbl->ui4_number_of_entry - 1;
                prSwdmxAviInfo->u4CurRepeatSubFrameIdx = prPrevKeyTbl->ui4_number_of_entry - 1;
            }
            else if ((prSwdmxAviInfo->prCurVidKeyIdxTbl == prSwdmxAviInfo->prVidKeyIdxTbl) &&
                     (prSwdmxAviInfo->u4CurRepeatFrameIdx == 0) &&
                     (prSwdmxAviInfo->prCurSubVidKeyIdxTbl == prSwdmxAviInfo->prSubVidKeyIdxTbl) &&
                     (prSwdmxAviInfo->u4CurRepeatSubFrameIdx == 0) &&
                     !prSwdmxAviInfo->fgDeliverVEos)
            {
                LOG(5, "Fast Backward _SWDMX_AviHdrPrsUseIdx Deliver Video Eos\n");
                if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId))
                {
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1Vdec2Id))
                {
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                prSwdmxAviInfo->fgDeliverVEos = TRUE;
            }
        }  
    }

    if (fgVSubRangeDone)  
    {
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
    }        
    return eSWDMX_HDR_PRS_SUCCEED;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_DeliverAviDataUseIdx(
    UINT8 u1SrcId,
    INT32 i4PlaySpeed,
    MM_RANGE_ELMT_IDXTBL_T *prIdxTbl, 
    UINT32* pu4CurIdx, 
    SWDMX_RANGE_LIST_T* prRangeList,
    UINT8 u1VdecId)
{
    BOOL fgRangeEnd = FALSE;

    UINT8  u1PidIdx = 0;
    UINT8  u1HdrType = 0;
    UINT8  u1PidType = DMX_PID_TYPE_NONE;
    UINT8  u1ReadDrmSize = 0;
    UINT32 u4DmxAvailSize = 0;
    UINT32 u4Size = 0, u4ConsumeSize = 0, u4Ret = 0;
    UINT32 u4CurPts = 0;
    INT32  u4FbCurPts = 0;
    UINT32 u4PtsDif = 0;
    UINT64 u8VidOffset = 0;
    UCHAR  *pucBuf = NULL;

    DMX_MM_DATA_T rDmxMMData;
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;
    
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
    
    if(prSwdmxAviInfo->fgIs3DFile)
    {
        if(u1VdecId == prSwdmxInst->u1Vdec2Id)
        {
            u1PidIdx = prSwdmxAviInfo->u1Vid2Pid;
            u1PidType = DMX_PID_TYPE_ES_VIDEO;
            u1HdrType = eSWDMX_MEDIA_VIDEO;
        }
    }
    
    if(u1VdecId == prSwdmxInst->u1VdecId)
    {
        u1PidIdx = prSwdmxAviInfo->u1VidPid;
        u1PidType = DMX_PID_TYPE_ES_VIDEO;
        u1HdrType = eSWDMX_MEDIA_VIDEO;
    }
    else
    {
        u1PidIdx = prSwdmxAviInfo->u1AudPid;
        u1PidType = DMX_PID_TYPE_ES_AUDIO;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, (DMX_PID_TYPE_T)u1PidType, u1PidIdx);

    if (prRangeList->prDmxRangeInfo->b_drm)
    {
        u1ReadDrmSize = AVI_DRM_CHUNK_SIZE_HT;
    }
  
    if (prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size > u4DmxAvailSize)
    {
        LOG(7, "SWDMX Demuxer Buffer is not enough.\n");
        x_thread_delay(1);
        return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
    }

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    if ((*pu4CurIdx) < prIdxTbl->ui4_number_of_entry)
    {
        if (u1HdrType == eSWDMX_MEDIA_VIDEO)
        {
            //UCHAR *pBuf;
            UINT16 u2MaxQNum, u2CurQNum;

            VDEC_GetQueueInfo(u1VdecId, &u2CurQNum, &u2MaxQNum);
            LOG(7, "Line %d, u2CurQNum = %ld, u2MaxQNum = %ld .\n", __LINE__, u2CurQNum, u2MaxQNum);
            
            if ((UINT32)u2CurQNum > (prSwdmxAviInfo->u4VidFps/2))
            {
                 prSwdmxInst->fgFifoFull = TRUE;
                x_thread_delay(30);
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            if(u1VdecId == prSwdmxInst->u1VdecId)
            {
                if (prSwdmxAviInfo->u4VidRepeatFrame != 0)
                {
                    if (prSwdmxAviInfo->fgRequestData)
                    {
                        if ((prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset) >
                            (AVI_PARSER_OFFSET + u1ReadDrmSize))
                        {
                            u8VidOffset = 
                                (prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset) - 
                                (AVI_PARSER_OFFSET + u1ReadDrmSize);
                        }
                        else
                        {
                            LOG(3, "Video offset is smaller than (AVI_PARSER_OFFSET + u1ReadDrmSize)\n");
                            u8VidOffset = 0;
                        }
                         /*some time key table offset isn't point to four cc ,add the size that subtract in VidOffset 20101228*/
                        u4Size = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size + AVI_PARSER_OFFSET + 
                                      AVI_PARSER_OFFSET + u1ReadDrmSize;  
                                      
                        LOG(5, "Offset = %lld Request size = 0x%08x.\n", u8VidOffset,u4Size);
                        
                        if ((u8VidOffset + u4Size) > prSwdmxInst->u8FileSize)
                        {
                            prSwdmxAviInfo->u4CurRepeatFrameIdx = *pu4CurIdx;
                            LOG(5, "The information in the index table is wrong\n");
                            if (i4PlaySpeed > 2000)
                            {
                                (*pu4CurIdx)++;
                            }
                            else if ((i4PlaySpeed < 0) &&
                                (*pu4CurIdx > 0))
                            {
                                (*pu4CurIdx)--;
                            }
                            prSwdmxAviInfo->fgRequestData = TRUE;
                            return eSWDMX_HDR_PRS_FAIL;
                        }
                            
                        if (!prSwdmxAviInfo->fgReqDataMiss)
                        {
                            if (FeederSetRequest(prSwdmxInst->eFeederSrc, 
                                u4Size, 
                                u8VidOffset,
                                &rFeederCond,
                                prSwdmxAviInfo->u4FeederReqID) != FEEDER_E_BUFFER_FULL)
                            {
                                prSwdmxAviInfo->u4FeederReqID++;
                                prSwdmxAviInfo->i4ReqReadTimes++;
                            if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
                                {
                                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                                }
                            }
                            else
                            {
                                LOG(3, "Feeder Buffer is not enough\n");
                                return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                            }
                        }
                        else
                        {
                            LOG(5, "wait for missed data request\n");
                            if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
                            {
                                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                            }
                        }
                        prSwdmxAviInfo->i4ReqReadTimes--;
                        if (rFeederReqData.u4ReadSize > rFeederReqData.u4AlignOffset)
                        {
                            u4Size = rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
                        }
                        else
                        {
                            return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                        }
                        LOG(5, "rFeederReqData.u4ReadSize = 0x%08x.\n", u4Size);
                        prSwdmxAviInfo->u8VidOffset = u8VidOffset + u4Size;
                        pucBuf = (UCHAR*)(VIRTUAL(rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset));
                        prSwdmxAviInfo->u4RecFeederWPtr = (UINT32)pucBuf; 
                        prSwdmxAviInfo->u4RecFeederSize = u4Size;
                        prSwdmxAviInfo->fgRequestData = FALSE;
                    }
                    else
                    {
                        pucBuf = (UCHAR*)prSwdmxAviInfo->u4RecFeederWPtr;
                        u4Size = prSwdmxAviInfo->u4RecFeederSize;
                    }
                        //drm file
                    if (u1ReadDrmSize)
                    {
                        if (_SWDMX_PrsAviChunk(u1SrcId,
                            pucBuf, 
                            prRangeList, 
                            u4Size, 
                            &u4ConsumeSize, 
                            i4PlaySpeed, 
                            fgRangeEnd) != eSWDMX_HDR_PRS_SUCCEED)
                        {
                            LOG(3, "_SWDMX_DeliverAviDataUseIdx Parse Drm Fail\n");
                            goto _Exit;
                        }
                        u8VidOffset += (UINT64)(u4ConsumeSize);
                        prSwdmxAviInfo->u4RecFeederSize -= u4ConsumeSize;
                    }
                    pucBuf = (UCHAR*)VIRTUAL((UINT32)pucBuf + u4ConsumeSize);
                    u4Size = prSwdmxAviInfo->u4RecFeederSize;
                    u4Ret = _SWDMX_PrsAviChunk(u1SrcId,
                                    pucBuf, 
                                    prRangeList, 
                                    u4Size, 
                                    &u4ConsumeSize, 
                                    i4PlaySpeed, 
                                    fgRangeEnd);
                    if ((u4ConsumeSize == AVI_PARSER_OFFSET) &&
                        (u4Size > u4ConsumeSize) &&
                        (!prSwdmxAviInfo->fgIsBlock))
                    {
                        pucBuf = (UCHAR*)VIRTUAL((UINT32)pucBuf + u4ConsumeSize);
                        u4Size -= AVI_PARSER_OFFSET;
                        u4Ret = _SWDMX_PrsAviChunk(u1SrcId,
                                    pucBuf, 
                                    prRangeList, 
                                    u4Size, 
                                    &u4ConsumeSize, 
                                    i4PlaySpeed, 
                                    fgRangeEnd);
                    }
                    if (u4Ret == eSWDMX_HDR_FED_BUF_NOT_ENOUGH)
                    {
                        UINT32 u4RequestSize = 0;
                        u8VidOffset = prSwdmxAviInfo->u8VidOffset;
                        LOG(5, "Block Size = 0x%08x\n", prSwdmxAviInfo->u4BlockChunkSize);
                        if (prSwdmxAviInfo->fgIsBlock)
                        {
                            u4RequestSize = prSwdmxAviInfo->u4BlockChunkSize - 
                                (prSwdmxAviInfo->u4RecFeederSize - u4ConsumeSize);
                        }
                        else
                        {
                            LOG(3, "_SWDMX_DeliverAviDataUseIdx Parse Video Chunk Fail\n");
                            goto _Exit;
                        }
                        if (!prSwdmxAviInfo->fgReqDataMiss)
                        {
                            if (FeederSetRequest(prSwdmxInst->eFeederSrc, 
                                                 u4RequestSize, 
                                                 u8VidOffset,
                                                 &rFeederCond,
                                                 prSwdmxAviInfo->u4FeederReqID) != FEEDER_E_BUFFER_FULL)
                            {
                                prSwdmxAviInfo->u4FeederReqID++;
                                prSwdmxAviInfo->i4ReqReadTimes++;
                                if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
                                {
                                    return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                                }
                            }
                            else
                            {
                                LOG(3, "Feeder Buffer is not enough\n");
                                return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                            }
                        }
                        else
                        {
                            LOG(5, "wait for missed data request\n");
                            if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
                            {
                                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                            }
                        }
                        prSwdmxAviInfo->i4ReqReadTimes--;
                        if (rFeederReqData.u4AlignOffset != 0)
                        {
                            LOG(1, "Feeder's alignoffset != 0, it's not normal\n");
                        }
                        prSwdmxAviInfo->u4RecFeederSize += rFeederReqData.u4ReadSize;
                        UNUSED(_SWDMX_PrsAviChunk(u1SrcId,
                                    pucBuf, 
                                    prRangeList, 
                                    prSwdmxAviInfo->u4RecFeederSize, 
                                    &u4ConsumeSize, 
                                    i4PlaySpeed, 
                                    fgRangeEnd));
                    }
                    else if (u4Ret != eSWDMX_HDR_PRS_SUCCEED)
                    {
                        LOG(3, "_SWDMX_DeliverAviDataUseIdx Parse Video Chunk Fail\n");
                        goto _Exit;
                    }
                }
                if (!_SWDMX_AviCalculateRepeatFrame(prSwdmxInst, i4PlaySpeed, u1VdecId))
                {
                    if(prSwdmxAviInfo->fgIs3DFile &&(prSwdmxAviInfo->fgIsSupport3DTrick))
                    {                  
                        if (i4PlaySpeed >= 2000)
                        {
                            (*pu4CurIdx)++;
                            prSwdmxAviInfo->u4CurSentSubKey ++;
                            if((prSwdmxAviInfo->u4CurRepeatFrameIdx !=  prIdxTbl->ui4_number_of_entry - 1) &&
                                (prSwdmxAviInfo->u4CurRepeatSubFrameIdx !=  prIdxTbl->ui4_number_of_entry - 1))
                            {
                                u4PtsDif = prIdxTbl->u.pt_pb_idx_tbl_entry[(*pu4CurIdx)].ui4_pts -
                                            prIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurRepeatFrameIdx].ui4_pts;
                                if(u4PtsDif < 6000)
                                { 
                                    if(i4PlaySpeed == 2000)
                                    {
                                        u4CurPts = prIdxTbl->u.pt_pb_idx_tbl_entry[(*pu4CurIdx)].ui4_pts + AVI_2X_TRICK_JUMP_PTS;
                                    }
                                    else
                                    {
                                        u4CurPts = prIdxTbl->u.pt_pb_idx_tbl_entry[(*pu4CurIdx)].ui4_pts + AVI_TRICK_JUMP_PTS;
                                    }
                                    if(((*pu4CurIdx) == prSwdmxAviInfo->u4CurSentSubKey) && (prSwdmxAviInfo->u4SubVidRepeatFrame == 0))
                                    {
                                        if(u4CurPts < prIdxTbl->u.pt_pb_idx_tbl_entry[(prIdxTbl->ui4_number_of_entry - 1)].ui4_pts)
                                        {
                                            UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));
                                        }                                  
                                        else
                                        {
                                            prSwdmxAviInfo->u4CurSentKey = prIdxTbl->ui4_number_of_entry - 1;
                                            prSwdmxAviInfo->u4CurSentSubKey = prIdxTbl->ui4_number_of_entry -1;
                                        }
                                   }
                                }
                            }
                        }
                        else if ((i4PlaySpeed < 0) && (*pu4CurIdx > 0))
                        {
                             (*pu4CurIdx)--;
                             prSwdmxAviInfo->u4CurSentSubKey --;
                             u4PtsDif = prIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurRepeatFrameIdx].ui4_pts -
                                        prIdxTbl->u.pt_pb_idx_tbl_entry[(*pu4CurIdx)].ui4_pts;   
                                        
                             if(u4PtsDif < 6000)
                             {  
                                u4FbCurPts = (INT32)(prIdxTbl->u.pt_pb_idx_tbl_entry[(*pu4CurIdx)].ui4_pts) - AVI_TRICK_JUMP_PTS;
                                if(((*pu4CurIdx) == prSwdmxAviInfo->u4CurSentSubKey) &&
                                    (prSwdmxAviInfo->u4SubVidRepeatFrame == 0))
                                {
                                     if(u4FbCurPts > 0)
                                    {
                                        u4CurPts = (UINT32)u4FbCurPts;
                                        UNUSED(_SWDMX_AviSeek(prSwdmxInst, u4CurPts, prRangeList));
                                    }                                  
                                    else
                                    {
                                        prSwdmxAviInfo->u4CurSentKey = 0;
                                        prSwdmxAviInfo->u4CurSentSubKey = 0;
                                    }
                                }                                
                             }
                        }
                        prSwdmxAviInfo->fgSubVideoRequestData = TRUE;
                    }
                    else
                    {
                        if (i4PlaySpeed > 2000)
                        {
                            (*pu4CurIdx)++;
                        }
                        else if ((i4PlaySpeed < 0) &&
                            (*pu4CurIdx > 0))
                        {
                            (*pu4CurIdx)--;
                        }
                    }     
                    prSwdmxAviInfo->fgRequestData = TRUE;
                    return eSWDMX_HDR_PRS_SUCCEED;
                }        
        }     
        if(prSwdmxAviInfo->fgIs3DFile && u1VdecId == prSwdmxInst->u1Vdec2Id)      
        {
            if (prSwdmxAviInfo->u4SubVidRepeatFrame != 0)
            {
                if (prSwdmxAviInfo->fgSubVideoRequestData)
                {
                    if ((prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset) >
                        (AVI_PARSER_OFFSET + u1ReadDrmSize))
                    {
                        u8VidOffset = 
                            (prIdxTbl->ui8_base_offset + prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_relative_offset) - 
                            (AVI_PARSER_OFFSET + u1ReadDrmSize);
                    }
                    else
                    {
                        LOG(3, "Sub Video offset is smaller than (AVI_PARSER_OFFSET + u1ReadDrmSize)\n");
                        u8VidOffset = 0;
                    }
                    u4Size = prIdxTbl->u.pt_pb_idx_tbl_entry[*pu4CurIdx].ui4_size + AVI_PARSER_OFFSET + u1ReadDrmSize;
                    LOG(5, "Offset = %lld Request size = 0x%08x.\n", u8VidOffset,u4Size);
                    if ((u8VidOffset + u4Size) > prSwdmxInst->u8FileSize)
                    {
                        prSwdmxAviInfo->u4CurRepeatSubFrameIdx = *pu4CurIdx;
                        LOG(5, "The information in the sub index table is wrong\n");
                        if (i4PlaySpeed > 2000)
                        {
                            (*pu4CurIdx)++;
                        }
                        else if ((i4PlaySpeed < 0) &&
                            (*pu4CurIdx > 0))
                        {
                            (*pu4CurIdx)--;
                        }
                        prSwdmxAviInfo->fgSubVideoRequestData = TRUE;
                        return eSWDMX_HDR_PRS_FAIL;
                    }

                    if (!prSwdmxAviInfo->fgReqSubDataMiss)
                    {
                        if (FeederSetRequest(prSwdmxInst->eFeederSrc, 
                                             u4Size, 
                                             u8VidOffset,
                                             &rFeederCond,
                                             prSwdmxAviInfo->u4FeederReqID) != FEEDER_E_BUFFER_FULL)
                        {
                            prSwdmxAviInfo->u4FeederReqID++;
                            prSwdmxAviInfo->i4ReqReadTimes++;
                            if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1Vdec2Id))
                            {
                                //prSwdmxAviInfo->fgFeederInvalid = TRUE;
                                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                            }
                         }
                         else
                         {
                             LOG(3, "Feeder Buffer is not enough\n");
                             return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                         }
                    }
                    else
                    {
                        LOG(5, "wait for missed data request\n");
                        if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1Vdec2Id))
                        {
                            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                        }
                    }

                    prSwdmxAviInfo->i4ReqReadTimes--;

                    if (rFeederReqData.u4ReadSize > rFeederReqData.u4AlignOffset)
                    {
                        u4Size = rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
                    }
                    else
                    {
                        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                    }

                    LOG(5, "rFeederReqData.u4ReadSize = 0x%08x.\n", u4Size);

                    // prSwdmxAviInfo->u4RecFeederSize = u4Size;
                    prSwdmxAviInfo->u8SubVidOffset = u8VidOffset + u4Size;

                    pucBuf = (UCHAR*)(VIRTUAL(rFeederReqData.u4WriteAddr + rFeederReqData.u4AlignOffset));
                    prSwdmxAviInfo->u4RecSubVFeederWPtr = (UINT32)pucBuf; 
                    prSwdmxAviInfo->u4RecSubVFeederSize = u4Size;

                    // we don't have to request for the same data for frame repeating.
                    prSwdmxAviInfo->fgSubVideoRequestData = FALSE;
                }
                else
                {
                    pucBuf = (UCHAR*)prSwdmxAviInfo->u4RecSubVFeederWPtr;
                    u4Size =prSwdmxAviInfo->u4RecSubVFeederSize;
                }

                if (u1ReadDrmSize)
                {
                    if (_SWDMX_PrsAviChunk(u1SrcId,
                        pucBuf, 
                        prRangeList, 
                        u4Size, 
                        &u4ConsumeSize, 
                        i4PlaySpeed, 
                        fgRangeEnd) != eSWDMX_HDR_PRS_SUCCEED)
                    {
                        LOG(3, "_SWDMX_DeliverAviDataUseIdx Parse Drm Fail\n");
                        goto _Exit;
                    }

                    u8VidOffset += (UINT64)(u4ConsumeSize);
                    prSwdmxAviInfo->u4RecSubVFeederSize -= u4ConsumeSize;
                }

                pucBuf = (UCHAR*)VIRTUAL((UINT32)pucBuf + u4ConsumeSize);
                u4Size = prSwdmxAviInfo->u4RecSubVFeederSize;

                u4Ret = _SWDMX_PrsAviChunk(u1SrcId,
                                pucBuf, 
                                prRangeList, 
                                u4Size, 
                                &u4ConsumeSize, 
                                i4PlaySpeed, 
                                fgRangeEnd);

                // Prevent from empty video chunk
                if ((u4ConsumeSize == AVI_PARSER_OFFSET) &&
                    (u4Size > u4ConsumeSize) &&
                    (!prSwdmxAviInfo->fgIsBlock))
                {
                    pucBuf = (UCHAR*)VIRTUAL((UINT32)pucBuf + u4ConsumeSize);
                    u4Size -= AVI_PARSER_OFFSET;

                    u4Ret = _SWDMX_PrsAviChunk(u1SrcId,
                                pucBuf, 
                                prRangeList, 
                                u4Size, 
                                &u4ConsumeSize, 
                                i4PlaySpeed, 
                                fgRangeEnd);
                }

                if (u4Ret == eSWDMX_HDR_FED_BUF_NOT_ENOUGH)
                {
                    UINT32 u4RequestSize = 0;

                    u8VidOffset = prSwdmxAviInfo->u8SubVidOffset;

                    LOG(5, "Block Size = 0x%08x\n", prSwdmxAviInfo->u4BlockChunkSize);

                    if (prSwdmxAviInfo->fgIsBlock)
                    {
                        u4RequestSize = prSwdmxAviInfo->u4BlockChunkSize - 
                            (prSwdmxAviInfo->u4RecSubVFeederSize - u4ConsumeSize);
                    }
                    else
                    {
                        LOG(3, "_SWDMX_DeliverAviDataUseIdx Parse Video Chunk Fail\n");
                        goto _Exit;
                    }

                    if (!prSwdmxAviInfo->fgReqDataMiss)
                    {
                        if (FeederSetRequest(prSwdmxInst->eFeederSrc, 
                            u4RequestSize, 
                            u8VidOffset,
                            &rFeederCond,
                            prSwdmxAviInfo->u4FeederReqID) != FEEDER_E_BUFFER_FULL)
                        {
                            prSwdmxAviInfo->u4FeederReqID++;
                            prSwdmxAviInfo->i4ReqReadTimes++;
                            if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1Vdec2Id))
                            {
                                //prSwdmxAviInfo->fgFeederInvalid = TRUE;
                                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                            }
                        }
                        else
                        {
                            LOG(3, "Feeder Buffer is not enough\n");
                            return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                        }
                    }
                    else
                    {
                        LOG(5, "wait for missed data request\n");
                        if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1Vdec2Id))
                        {
                            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                        }
                    }

                    prSwdmxAviInfo->i4ReqReadTimes--;

                    if (rFeederReqData.u4AlignOffset != 0)
                    {
                        LOG(1, "Feeder's alignoffset != 0, it's not normal\n");
                    }

                    prSwdmxAviInfo->u4RecSubVFeederSize += rFeederReqData.u4ReadSize;

                    UNUSED(_SWDMX_PrsAviChunk(u1SrcId,
                                pucBuf, 
                                prRangeList, 
                                prSwdmxAviInfo->u4RecSubVFeederSize, 
                                &u4ConsumeSize, 
                                i4PlaySpeed, 
                                fgRangeEnd));
                }
                else if (u4Ret != eSWDMX_HDR_PRS_SUCCEED)
                {
                    LOG(3, "_SWDMX_DeliverAviDataUseIdx Parse Video Chunk Fail\n");
                    goto _Exit;
                }
            }

            if (!_SWDMX_AviCalculateRepeatFrame(prSwdmxInst, i4PlaySpeed, u1VdecId))
            {
                prSwdmxAviInfo->fgSubVideoRequestData = TRUE;
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
    }
        //update feeder RPTR
    prSwdmxAviInfo->u4CurDmxFeederRPtr = (UINT32)pucBuf + u4ConsumeSize;
    if(prSwdmxAviInfo->u4CurDmxFeederRPtr > 
        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        prSwdmxAviInfo->u4CurDmxFeederRPtr -= 
            (prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr);
    }
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 
        VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));
    }
    return eSWDMX_HDR_PRS_SUCCEED;

_Exit:
    //update feeder RPTR
    prSwdmxAviInfo->u4CurDmxFeederRPtr = (UINT32)pucBuf + u4ConsumeSize;
    if(prSwdmxAviInfo->u4CurDmxFeederRPtr > VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        prSwdmxAviInfo->u4CurDmxFeederRPtr -= 
            (prSwdmxInst->rFeederInfo.u4EndAddr - prSwdmxInst->rFeederInfo.u4StartAddr);
    }
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 
       VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));

    if (i4PlaySpeed > 2000)
    {
        (*pu4CurIdx)++;
    }
    else if ((i4PlaySpeed < 0) &&
        (*pu4CurIdx > 0))
    {
        (*pu4CurIdx)--;
    }
    prSwdmxAviInfo->fgRequestData = TRUE;
    if(prSwdmxAviInfo->fgIs3DFile &&(prSwdmxAviInfo->fgIsSupport3DTrick))
    {
        prSwdmxAviInfo->fgSubVideoRequestData = TRUE;
    }
    return eSWDMX_HDR_PRS_FAIL;

}

static SWDMX_HDR_PRS_STATE_T _SWDMX_AviHdrPrsWithoutIdx(
    UINT8 u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    INT32 i4PlaySpeed,
    BOOL fgSpeedChange)
{
    BOOL fgRangeEnd = FALSE;

    UINT32 u4ReadSize = 0;
    UINT32 u4ConsumedSize = 0;
    UINT32 u4PrsState = eSWDMX_HDR_PRS_SUCCEED;
    UINT32 u4MaxDataByte = 0;
    FEEDER_TRANSMIT_COND rFeederCond;
    FEEDER_REQ_DATA_T rFeederReqData;

    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }
    //to add a reset feeder fun
    if (prSwdmxAviInfo->fgFeederInvalid)
    {        
        _SWDMX_AviResetFeeder(prSwdmxInst);
        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }

    // Check if demuxing is done
    if (prSwdmxAviInfo->u8CurDmxMoviPos >= prSwdmxInst->u8FileSize
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        || (prSwdmxAviInfo->u4LeftBytesInFeeder == 0 && prSwdmxAviInfo->u8CurMoviPos >= prSwdmxInst->u8FileSize)
#endif
#ifdef __MODEL_slt__
        || prSwdmxAviInfo->u4NumSentVid == 10
#endif
        )
    {
    	  UINT32 u4CurPts = 0;
    	  BOOL fgRet = FALSE; 
    	  UINT32 u4APts = 0;
    	  
        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
        
        if (prSwdmxAviInfo->fgRecSpeedChangeForAud && prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
        {
            if (prSwdmxAviInfo->i4RecPrevSpeedForAud == 2000 || 
                prSwdmxAviInfo->i4RecPrevSpeedForAud == 1||
                ((prSwdmxAviInfo->i4RecPrevSpeedForAud > 1)&&
                (prSwdmxAviInfo->i4RecPrevSpeedForAud<1000)))
            {
                u4CurPts = (prSwdmxAviInfo->fgEnVideo) ? (VDP_GetPts(prSwdmxInst->u1B2rId)) : (STC_GetStcValue(prSwdmxInst->u1StcId));
                
                prSwdmxAviInfo->u4TriggerAudPts = u4CurPts + 27000;

                fgRet = DMX_AudHandler_SeekAndUpdate(
                    prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx],
                    prSwdmxAviInfo->u4TriggerAudPts, &u4APts);

                LOG(5, "Find Audio Track Data for 2X -> 1X\n");

                if (fgRet)
                {
                    LOG(5, "Find audio in the pre-buffer\n");
                    prSwdmxAviInfo->u4TriggerAudPts = u4APts;
                }
                else
                {
                    LOG(5, "Can't find audio in the pre-buffer\n");
                    prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx];
                }

                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4TriggerAudPts);
                LOG(5, "prSwdmxAviInfo->u4TriggerAudPts = 0x%08x\n", prSwdmxAviInfo->u4TriggerAudPts);
            }
            else
            {
                UNUSED(DMX_AudHandler_SeekAndUpdate(prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx],
                    0xFFFFFFFF, &u4APts));
            }

            prSwdmxAviInfo->fgRecSpeedChangeForAud = FALSE;
            prSwdmxAviInfo->i4RecPrevSpeedForAud = i4PlaySpeed;
            DMX_AudHandler_SetActivePidx(prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx]);
            prSwdmxAviInfo->fgSetActivePid = TRUE;
        }
        
        if (prSwdmxAviInfo->fgEnVideo && 
            !prSwdmxAviInfo->fgDeliverVEos)
        {
            if (prSwdmxAviInfo->fgVDecIsPause)
            {
                _SWDMX_AviVideoWakeUp(prSwdmxInst);
                prSwdmxAviInfo->fgVDecIsPause = FALSE;
            }

            LOG(5, "Cur DmxMoviPos = %llu, range size = %llu\n", 
                prSwdmxAviInfo->u8CurDmxMoviPos,
                prSwdmxInst->u8RangeSize);
            
            LOG(5, "_SWDMX_AviHdrPrsWithoutIdx Deliver Video Eos\n");
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1Vdec2Id))
                {
                    LOG(3, "Fail to send VideoR Eos\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                 if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId))
                {
                    LOG(3, "Fail to send Video Eos\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
            }
            else
            {
                if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_VIDEO, prSwdmxInst->u1VdecId))
                {
                    LOG(3, "Fail to send Video Eos\n");
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
            }
            prSwdmxAviInfo->fgDeliverVEos = TRUE;
        }

        if (prSwdmxAviInfo->fgEnAudio && 
            !prSwdmxAviInfo->fgDeliverAEos)
        {
            LOG(5, "_SWDMX_AviHdrPrsWithoutIdx Deliver Audio Eos\n");
            if (!_SWDMX_SendAviEos(prSwdmxInst, eSWDMX_MEDIA_AUDIO, 0))
            {
                LOG(3, "Fail to send Audio Eos\n");
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
            prSwdmxAviInfo->fgDeliverAEos = TRUE;
            AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
        }

        return eSWDMX_HDR_PRS_SUCCEED;
    }

    // Request data for the first time
    if (prSwdmxAviInfo->u8CurMoviPos == 0)
    {
        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != 
          FEEDER_E_OK)
        {
            LOG(7, "Feeder input buffer error.\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

        if (prSwdmxInst->u8RangeSize > prSwdmxAviInfo->u8CurMoviPos)
        {
            u4ReadSize = prSwdmxInst->u8RangeSize - 
                                   prSwdmxAviInfo->u8CurMoviPos;
        }

		if(prSwdmxInst->rFmtInfo.rSwdmxAviInfo.fgEnVideo)
		{
			u4ReadSize = min(u4ReadSize, (UINT32)MIN_READ_SIZE);
		}
		else
		{
			u4ReadSize = min(u4ReadSize, (UINT32)MIN_READ_SIZE_NO_VIDEO);
		}

        prSwdmxAviInfo->u4LastAlignOffset = 0;

        prSwdmxAviInfo->u8CurMoviPos = 
            prSwdmxInst->u8FileOffset;
        prSwdmxAviInfo->u8CurDmxMoviPos = 
            prSwdmxAviInfo->u8CurMoviPos;
        prSwdmxAviInfo->u4CurDmxFeederRPtr = 
            VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);

        if (!prSwdmxAviInfo->fgReqDataMiss)
        {
            //to add a fun
            if (FeederSetRequest(prSwdmxInst->eFeederSrc, 
                    u4ReadSize, /*MIN_READ_SIZE, */
                    prSwdmxAviInfo->u8CurMoviPos,
                    &rFeederCond,
                    prSwdmxAviInfo->u4FeederReqID) != FEEDER_E_BUFFER_FULL)
            {
                prSwdmxAviInfo->u4FeederReqID++;
                prSwdmxAviInfo->i4ReqReadTimes ++;
                if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
                {
                     //prSwdmxAviInfo->fgFeederInvalid = TRUE;

                    return eSWDMX_HDR_PRS_FAIL;
                }
                prSwdmxAviInfo->i4ReqReadTimes --;
                prSwdmxAviInfo->u4LastAlignOffset = rFeederReqData.u4AlignOffset;
                prSwdmxAviInfo->u8CurMoviPos += 
                    rFeederReqData.u4ReadSize - prSwdmxAviInfo->u4LastAlignOffset;
                prSwdmxAviInfo->u4CurDmxFeederRPtr += prSwdmxAviInfo->u4LastAlignOffset;
                prSwdmxAviInfo->u4LeftBytesInFeeder += rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
                LOG(7, "_SWDMX_ReceiveFeederAck [%ld],  u4QrySize= %ld u8Offset= 0x%llx \n\n", rFeederReqData.u4Id, rFeederReqData.u4ReadSize, rFeederReqData.u8FilePos);
            }
        }
        else
        {
            LOG(5, "wait for missed data request\n");
            if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }

            prSwdmxAviInfo->u4LastAlignOffset = rFeederReqData.u4AlignOffset;

            if ((rFeederReqData.u4ReadSize > prSwdmxAviInfo->u4LastAlignOffset) &&
                (rFeederReqData.u4ReadSize != 0))
            {
                prSwdmxAviInfo->u8CurMoviPos += 
                (rFeederReqData.u4ReadSize-prSwdmxAviInfo->u4LastAlignOffset);
            }
            else
            {
                LOG(3, "Feeder read size is smaller than align offset!!!\n");
                prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxInst->u8FileSize;
                prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
                return eSWDMX_HDR_PRS_FAIL;
            }
            
            prSwdmxAviInfo->u4CurDmxFeederRPtr += prSwdmxAviInfo->u4LastAlignOffset;      
            prSwdmxAviInfo->u4LeftBytesInFeeder += rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
            prSwdmxAviInfo->i4ReqReadTimes--;

            if(prSwdmxAviInfo->fgIsSeek)
            {
                prSwdmxAviInfo->u4CurDmxFeederRPtr = 
                VIRTUAL(rFeederReqData.u4WriteAddr + prSwdmxAviInfo->u4LastAlignOffset);
                prSwdmxAviInfo->fgIsSeek = FALSE;
            }
        }
    }

    if ((prSwdmxAviInfo->u4LeftBytesInFeeder <=  prSwdmxInst->rFeederInfo.u4Size) ||
        (prSwdmxAviInfo->fgIsBlock == TRUE))
    {
        //BOOL fgReadData = TRUE;
        UINT32 u4WriteAddr = 0;

        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != 
          FEEDER_E_OK)
        {
            LOG(7, "Feeder input buffer error.\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
        
		if(prSwdmxInst->rFmtInfo.rSwdmxAviInfo.fgEnVideo)
		{
            #if defined(ANDROID) && defined(CC_SUPPORT_4K2K)  
			u4MaxDataByte  = 2*MIN_READ_SIZE;
            #else
			u4MaxDataByte  = prSwdmxInst->rFeederInfo.u4Size - MIN_READ_SIZE;
            #endif
		}
		else
		{
            #if defined(ANDROID) && defined(CC_SUPPORT_4K2K)  
			u4MaxDataByte  = 2*MIN_READ_SIZE_NO_VIDEO;
            #else
			u4MaxDataByte  = prSwdmxInst->rFeederInfo.u4Size - MIN_READ_SIZE_NO_VIDEO;
			#endif
		}
		
        if (prSwdmxInst->u8FileSize > prSwdmxAviInfo->u8CurMoviPos)
        {
            u4ReadSize = prSwdmxInst->u8FileSize - prSwdmxAviInfo->u8CurMoviPos;
        }
		
		if(prSwdmxInst->rFmtInfo.rSwdmxAviInfo.fgEnVideo)
		{
			u4ReadSize = min(u4ReadSize, (UINT32)MIN_READ_SIZE);
		}
		else
		{
			u4ReadSize = min(u4ReadSize, (UINT32)MIN_READ_SIZE_NO_VIDEO);
		}

        if (u4ReadSize == 0)
        {
            fgRangeEnd = TRUE;
        }

        u4WriteAddr = VIRTUAL(prSwdmxInst->rFeederInfo.u4WriteAddr + u4ReadSize);
        if (u4WriteAddr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
        {
            u4WriteAddr =  VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr) + 
                (u4WriteAddr -VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
        }
#if defined(ANDROID) && defined(CC_SUPPORT_4K2K)  
        if (prSwdmxAviInfo->u4LeftBytesInFeeder < u4MaxDataByte ||
           ((prSwdmxAviInfo->u4NumSentVid+prSwdmxAviInfo->u4NumSentAud)%13 == 0 &&
            (prSwdmxAviInfo->u4LeftBytesInFeeder < prSwdmxInst->rFeederInfo.u4Size - MIN_READ_SIZE)))
#else
        if (prSwdmxAviInfo->u4LeftBytesInFeeder < u4MaxDataByte)
#endif     
        {
            if (u4ReadSize > 0)
            {
                LOG(6, "Left bytes = %ld offset = %lld id = %ld.\n", 
                        prSwdmxAviInfo->u4LeftBytesInFeeder,
                        prSwdmxAviInfo->u8CurMoviPos,
                        prSwdmxAviInfo->u4FeederReqID);

                if (!prSwdmxAviInfo->fgReqDataMiss)
                {
                    if (FeederSetRequest(prSwdmxInst->eFeederSrc, 
                            u4ReadSize, 
                            prSwdmxAviInfo->u8CurMoviPos,
                            &rFeederCond,
                            prSwdmxAviInfo->u4FeederReqID) != FEEDER_E_BUFFER_FULL)
                    {
                        prSwdmxAviInfo->u4FeederReqID++;
                        prSwdmxAviInfo->u4LastAlignOffset = 0;
                        prSwdmxAviInfo->i4ReqReadTimes++;
                    }
                    else
                    {
                        LOG(3, "Feeder Buffer Full\n");
                        //prSwdmxAviInfo->fgFeederInvalid = TRUE;
                        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != 
                            FEEDER_E_OK)
                        {
                            LOG(1, "Feeder input buffer error.\n");
                            return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                        }
                        LOG(3, "Cur Fed's write ptr = 0x%08x\n", prSwdmxInst->rFeederInfo.u4WriteAddr);
                        //return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
                    }
                }
                else
                {
                    LOG(5, "wait for missed data request\n");
                    if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData , prSwdmxInst->u1VdecId))
                    {
                        return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
                    }

                    prSwdmxAviInfo->u4LastAlignOffset = rFeederReqData.u4AlignOffset;

                    if ((rFeederReqData.u4ReadSize > prSwdmxAviInfo->u4LastAlignOffset) &&
                        (rFeederReqData.u4ReadSize != 0))
                    {
                        prSwdmxAviInfo->u8CurMoviPos += rFeederReqData.u4ReadSize - prSwdmxAviInfo->u4LastAlignOffset;
                    }
                    else
                    {
                        LOG(3, "Feeder read size is smaller than align offset!!!\n");
                        prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxInst->u8FileSize;
                        prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
                        return eSWDMX_HDR_PRS_FAIL;
                    }
                    
                    prSwdmxAviInfo->u4CurDmxFeederRPtr += prSwdmxAviInfo->u4LastAlignOffset;
                    prSwdmxAviInfo->u4LeftBytesInFeeder += rFeederReqData.u4ReadSize - rFeederReqData.u4AlignOffset;
                    prSwdmxAviInfo->i4ReqReadTimes--;

                    if(prSwdmxAviInfo->fgIsSeek)
                    {
                        prSwdmxAviInfo->u4CurDmxFeederRPtr = VIRTUAL(rFeederReqData.u4WriteAddr + prSwdmxAviInfo->u4LastAlignOffset);
                        prSwdmxAviInfo->fgIsSeek = FALSE;
                    }
                }
            }
        }
    }

#ifdef CC_DLNA_SUPPORT

	if(prSwdmxAviInfo->fgEnVideo)
	{
	   if (u4ReadSize != MIN_READ_SIZE)
	   {
		   // DLNA
		   FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_EOS, TRUE);
	   }
	}
	else
	{
		if (u4ReadSize != MIN_READ_SIZE_NO_VIDEO)
	   {
		   // DLNA
		   FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_EOS, TRUE);
	   }
	}

   

    // DLNA
    if (!prSwdmxAviInfo->fgFeederIsReady && FeederIsReady(prSwdmxInst->eFeederSrc))
    /*if ((!prSwdmxAviInfo->fgFeederIsReady) && 
        ((prSwdmxAviInfo->u4LeftBytesInFeeder >= AVI_PREBUFFER_SIZE) || (prSwdmxAviInfo->u4LeftBytesInFeeder >= prSwdmxInst->u8RangeSize)))*/
    {
        prSwdmxAviInfo->fgFeederIsReady = TRUE;
    }
#endif

#ifdef SWDMX_TIME_PROFILE
    HAL_TIME_T rTimeRstS, rTimeRstE, rTimeRstDt; 
    HAL_GetTime(&rTimeRstS);
#endif

#ifdef CC_DLNA_SUPPORT
    if ((prSwdmxAviInfo->u4LeftBytesInFeeder >= 8) && prSwdmxAviInfo->fgFeederIsReady)
#else
    if (prSwdmxAviInfo->u4LeftBytesInFeeder >= 8)
#endif
    {
        //test 
        if (prSwdmxAviInfo->fgSpeedChange && i4PlaySpeed == 1000)
        {
            if (prSwdmxAviInfo->fgEnAudio && prSwdmxAviInfo->fgEnVideo)
            {
                if(((UINT32)(prSwdmxAviInfo->u8VSeekPts>>32) != 0xFFFFFFFF)
                    || ((UINT32)(prSwdmxAviInfo->u8VSeekPts) != 0xFFFFFFFF))
                {
                    if (prRangeList->prDmxRangeInfo->ui8_vid_duration > 
                        (UINT32)prSwdmxAviInfo->u8VSeekPts)
                    {
                        LOG(5, "Pause Video for A/V Sync\n");
                        VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                        if(prSwdmxAviInfo->fgIs3DFile) 
                        {
                            VDEC_Pause(prSwdmxInst->u1Vdec2Id);
                        }
                        VDEC_Pause(prSwdmxInst->u1VdecId);
                        prSwdmxAviInfo->fgVDecIsPause = TRUE;
                    }
                }
            }
            prSwdmxAviInfo->fgSpeedChange = FALSE;
        }
    
        // Start to parse data
        u4PrsState = _SWDMX_PrsAviChunk(u1SrcId,
            (UCHAR*)(prSwdmxAviInfo->u4CurDmxFeederRPtr),
            prRangeList,
            prSwdmxAviInfo->u4LeftBytesInFeeder,
            &u4ConsumedSize,
            i4PlaySpeed,
            fgRangeEnd);
    }
    else if(fgRangeEnd)
    {
        // Send EOS
        prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxInst->u8FileSize;
    }

#ifdef SWDMX_TIME_PROFILE
    HAL_GetTime(&rTimeRstE);
    HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
    LOG(0, "Swdmx parsing and deliver time = (%ld) \n", rTimeRstDt.u4Micros);
#endif

    switch (u4PrsState)
    {
        case eSWDMX_HDR_PRS_FAIL:
            // Prevent unknow chunk in the first one mega
            // If the file size is smaller than one mega, avi demuxer
            // will check if demuxing is done in the beginning of _SWDMX_AviHdrPrsWithoutIdx
            if (!prSwdmxAviInfo->u4PrevChunkPos && !prSwdmxAviInfo->fgDataChunkGap)
            {
                prSwdmxAviInfo->u4PrevChunkPos = prSwdmxAviInfo->u8CurDmxMoviPos;
            }

            if (prSwdmxAviInfo->u8CurDmxMoviPos > prSwdmxInst->u8FileOffset + 4*ONE_MB)
            {
                LOG(3, "line %d, AVI Header Parse Fail!!!!\n", __LINE__);
                if (!prSwdmxAviInfo->fgDataChunkGap)
                {
                    if (prSwdmxAviInfo->u4PrevChunkPos)
                    {
                        prSwdmxAviInfo->u4DataChunkGap = prSwdmxAviInfo->u8CurDmxMoviPos - prSwdmxAviInfo->u4PrevChunkPos;
                        prSwdmxAviInfo->fgDataChunkGap = TRUE;
                        LOG(0, "line %d, prSwdmxAviInfo->u4DataChunkGap %u!!!\n", __LINE__, prSwdmxAviInfo->u4DataChunkGap);
                    }
                }
                if (prSwdmxAviInfo->u4DataChunkGap < 1024 || prSwdmxAviInfo->u4PrsFailCounter > 256)
                {
                    LOG(0, "No avi chunks was discovered, fail counter %u.\n", prSwdmxAviInfo->u4PrsFailCounter);
                    prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxInst->u8FileSize;
                    prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
                    return eSWDMX_HDR_PRS_FAIL;
                }
            }
            prSwdmxAviInfo->u4PrsFailCounter++;
            break;
        case eSWDMX_HDR_PRS_SUCCEED:
            
            if (prSwdmxAviInfo->u8CurDmxMoviPos < prSwdmxInst->u8FileOffset + 4*ONE_MB)
            {
                if (!prSwdmxAviInfo->fgDataChunkGap)
                {
                    if (prSwdmxAviInfo->u4PrevChunkPos)
                    {
                        prSwdmxAviInfo->u4DataChunkGap = prSwdmxAviInfo->u8CurDmxMoviPos - prSwdmxAviInfo->u4PrevChunkPos;
                        prSwdmxAviInfo->fgDataChunkGap = TRUE;
                        LOG(0, "line %d, prSwdmxAviInfo->u4DataChunkGap %u!!!\n", __LINE__, prSwdmxAviInfo->u4DataChunkGap);
                    }  
                }
            }
            prSwdmxAviInfo->u4PrsFailCounter = 0;
            break;
        default:
        {
            x_thread_delay(1);
            break;
        }
    }

    if (prSwdmxAviInfo->u4LeftBytesInFeeder >= u4ConsumedSize)
    {
        prSwdmxAviInfo->u8CurDmxMoviPos += u4ConsumedSize;
        prSwdmxAviInfo->u4LeftBytesInFeeder -= u4ConsumedSize;
    }
    else
    {
        prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
    }

    if (u4ConsumedSize != 0)
    {
        LOG(6, "u4ConsumedSize = %ld u4LeftBytesInFeeder = 0x%08x.\n", 
            u4ConsumedSize, prSwdmxAviInfo->u4LeftBytesInFeeder);
    }

    if(prSwdmxAviInfo->fgIsSkipIdxData &&
        (u4ConsumedSize > prSwdmxAviInfo->u4LeftBytesInFeeder))
    {
        prSwdmxAviInfo->u8CurDmxMoviPos += u4ConsumedSize;
        prSwdmxAviInfo->fgFeederInvalid = TRUE;
        prSwdmxAviInfo->fgIsSkipIdxData = FALSE;
    }
    else if ((prSwdmxAviInfo->u4CurDmxFeederRPtr + u4ConsumedSize) >= 
        VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        prSwdmxAviInfo->u4CurDmxFeederRPtr =  VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr) + 
            ((prSwdmxAviInfo->u4CurDmxFeederRPtr + u4ConsumedSize) -VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
    }
    else
    {
        prSwdmxAviInfo->u4CurDmxFeederRPtr += u4ConsumedSize;
    }
    
    // Update read ptr to feeder
    if(prSwdmxAviInfo->u4AbDmxPos != 0 && 
         prSwdmxAviInfo->u4QAudAddr != 0)
    {
        UINT32 u4DmxPos = MIN(prSwdmxAviInfo->u4AbDmxPos ,prSwdmxAviInfo->u4QAudAddr );
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(u4DmxPos));
    }  
    else if(prSwdmxAviInfo->u4AbDmxPos != 0 )
    {
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4AbDmxPos));
    }
    else if (prSwdmxAviInfo->u4QAudAddr)
    {
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4QAudAddr));
    }
    else
    {
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, 
           VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));
    }

     LOG(9, "prSwdmxAviInfo->u4CurDmxMoviPos = %llu.\n", 
        prSwdmxAviInfo->u8CurDmxMoviPos);

    if (prSwdmxAviInfo->i4ReqReadTimes > 0)
    {
        if (!_SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId))
        {
            //prSwdmxAviInfo->fgFeederInvalid = TRUE;
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

        LOG(6, "rFeederReqData.u4WriteAddr = 0x%08x. ReadSize = 0x%08x id = %ld\n", 
            rFeederReqData.u4WriteAddr,
            rFeederReqData.u4ReadSize,
            rFeederReqData.u4Id);

        prSwdmxAviInfo->u4LastAlignOffset = rFeederReqData.u4AlignOffset;

        if ((rFeederReqData.u4ReadSize > prSwdmxAviInfo->u4LastAlignOffset) &&
            (rFeederReqData.u4ReadSize != 0))
        {
            prSwdmxAviInfo->u8CurMoviPos += 
                rFeederReqData.u4ReadSize-prSwdmxAviInfo->u4LastAlignOffset;
        }
        else
        {
            LOG(3, "Feeder read size is smaller than align offset!!!\n");
            prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxInst->u8FileSize;
            prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
            return eSWDMX_HDR_PRS_FAIL;
        }
        
        prSwdmxAviInfo->u4CurDmxFeederRPtr += prSwdmxAviInfo->u4LastAlignOffset;
        prSwdmxAviInfo->u4LeftBytesInFeeder += (rFeederReqData.u4ReadSize-rFeederReqData.u4AlignOffset);
        prSwdmxAviInfo->i4ReqReadTimes--;

        if(prSwdmxAviInfo->fgIsSeek)
        {
            prSwdmxAviInfo->u4CurDmxFeederRPtr = 
                VIRTUAL(rFeederReqData.u4WriteAddr + prSwdmxAviInfo->u4LastAlignOffset);
            prSwdmxAviInfo->fgIsSeek = FALSE;
        }
        LOG(7, "_SWDMX_ReceiveFeederAck [%ld],  u4QrySize= %ld u8Offset= 0x%llx \n\n", rFeederReqData.u4Id, rFeederReqData.u4ReadSize, rFeederReqData.u8FilePos);
    }

    return eSWDMX_HDR_PRS_SUCCEED;
}


static SWDMX_HDR_PRS_STATE_T _SWDMX_PrsAviChunk(
    UINT8 u1SrcId,
    UCHAR* pucBitstream, 
    SWDMX_RANGE_LIST_T* prRangeList,
    UINT32 u4AvailSize,
    UINT32* pu4ConsumeSize, 
    INT32 i4PlaySpeed,
    BOOL fgRangeEnd)
{
    BOOL fgRingBuf = FALSE;
    BOOL fgFindInRing = FALSE;
    BOOL fgFindFcc = FALSE;
    
    UINT8 u1HdrType;
    UINT8 u1TrackIdx = 0;
    UINT8 u1AudPid = 0;

    UINT16 u2DmxPictureQSize = 0;
    UINT16 u2DmxMaxQSize = 0;
    UINT16 u2VdecMaxQNum = 0;

    UINT32 i = 0, j = 0;
    UINT32 u4RingSize = 0, u4RingStart = 0, u4OrgRingSize = 0;
    //UINT32 u4TotalConsumeSize = 0;
    UINT32 u4ChunkSize, u4DmxAvailSize,u4DmxAudAvailSize;
    UINT32 u4Pts = 0;
    UINT32 u4OrgAvailSize = u4AvailSize;

    //UINT64 u8Numerator, u8Denominator;
    //UINT64 u8Remainder;

    UCHAR ucRingBuf[AVI_PARSER_OFFSET];
    UCHAR ucTempBuf[AVI_CHUNK_ID_OFFSET];
    UCHAR* pucTemp;
    UCHAR* pucOriginalPtr = pucBitstream;

    DMX_MM_DATA_T rDmxMMData;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    if (u4AvailSize < AVI_PARSER_OFFSET)
    {
        *pu4ConsumeSize = 0;
    }

    while (i < u4AvailSize)
    {
        if (((pucBitstream + i) >= (UCHAR*)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr - AVI_PARSER_OFFSET))) &&
            ((u4AvailSize - i) >= AVI_PARSER_OFFSET) && (!fgRingBuf))
        {
            fgRingBuf = TRUE;
            u4RingSize = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - (UINT32)(pucBitstream + i);
            u4OrgRingSize = u4RingSize;

            LOG(5, "Ring Buffer start from 0x%08x to 0x%08x size = 0x%08x\n", pucBitstream, 
                VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr), u4RingSize);

            x_memcpy(ucRingBuf, (pucBitstream + i), u4RingSize);

            u4AvailSize -= (u4RingSize + i);
            pucBitstream = (UCHAR*)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
            i = 0;
        }

        if ((fgRingBuf) && (u4RingSize))
        {
            // Handle the ring buffer case
            if (u4RingSize > AVI_CHUNK_ID_OFFSET)
            {
                for (j = 0; j < AVI_CHUNK_ID_OFFSET; j++)
                {
                    ucTempBuf[j] = ucRingBuf[u4RingStart + j];
                }
            }
            else
            {
                for (j = 0; j < u4RingSize; j++)
                {
                    ucTempBuf[j] = ucRingBuf[u4RingStart + j];
                }
                for (; j < AVI_CHUNK_ID_OFFSET; j++)
                {
                    ucTempBuf[j] = pucBitstream[j - u4RingSize];
                }
            }
            
            if (_CheckAviChunkType(prSwdmxInst, ucTempBuf, &u1HdrType, &u1TrackIdx) == TRUE)
            {
                fgFindFcc = TRUE;
                fgFindInRing = TRUE;
                break;
            }
            u4RingStart++;
            u4RingSize--;
            continue;
        }

        if (_CheckAviChunkType(prSwdmxInst, pucBitstream + i, &u1HdrType, &u1TrackIdx) == TRUE)
        {
            fgFindFcc = TRUE;
            break;
        }
        i++;
        //u4TotalConsumeSize++;
    }

    // Didn't find the chunk
    if (!fgFindFcc)
    {
        *pu4ConsumeSize = u4OrgAvailSize;
        return eSWDMX_HDR_PRS_FAIL;
    }

    // Avoid to access violation
    if ((u4OrgAvailSize - i) < 4)
    {
        LOG(5, "Avoid to access violation while parse data chunk!\r\n");
        *pu4ConsumeSize = i - 4;
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if ((!fgRingBuf) || (!fgFindInRing))
    {
        pucTemp = pucBitstream + AVI_CHUNK_ID_OFFSET + i;
        rDmxMMData.u4StartAddr = (UINT32)(pucBitstream + AVI_PARSER_OFFSET + i);
    }
    else
    {
        if (u4RingSize == 0)
        {
            pucTemp = pucBitstream;
            rDmxMMData.u4StartAddr = (UINT32)(pucBitstream + AVI_CHUNK_ID_OFFSET);
        }
        else
        {
            LOG(5, "fgFindInRing == TRUE u4RingSize = 0x%08x\n", u4RingSize);
            if (u4RingSize <= AVI_CHUNK_ID_OFFSET)
            {
                pucTemp = (UCHAR*)(pucBitstream + (AVI_CHUNK_ID_OFFSET - u4RingSize));
            }
            else
            {
                u4RingStart += AVI_CHUNK_ID_OFFSET;
                
                for (j = 0; j < (u4OrgRingSize - u4RingStart); j++)
                {
                    ucTempBuf[j] = ucRingBuf[u4RingStart + j];
                }
                for (j = 0; j < (AVI_CHUNK_ID_OFFSET - (u4RingSize - u4RingStart)); j++)
                {
                    ucTempBuf[j + (u4OrgRingSize - u4RingStart)] = pucBitstream[j];
                }
                pucTemp = ucTempBuf;
            }

            rDmxMMData.u4StartAddr = (UINT32)(pucBitstream + (AVI_PARSER_OFFSET - u4RingSize));
        }
    }

    u4ChunkSize = (pucTemp[0] | (pucTemp[1] << 8) |
                             (pucTemp[2] <<16) | (pucTemp[3] << 24));

    LOG(7, "u4ChunkSize = %ld.\n", u4ChunkSize);

    if((u1HdrType == eSWDMX_MEDIA_VIDEO)&&(!prSwdmxAviInfo->fgFirstVdChunk))
    {
        prSwdmxAviInfo->fgFirstVdChunk = TRUE;
        prSwdmxAviInfo->u8FirstVdPos = prSwdmxAviInfo->u8CurDmxMoviPos;
    }
    
    if(((u1HdrType == eSWDMX_MEDIA_AUDIO) ||
        (u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO))&&(!prSwdmxAviInfo->fgFirstAdChunk))
    {
        prSwdmxAviInfo->fgFirstAdChunk = TRUE;
        prSwdmxAviInfo->u8FirstAdPos = prSwdmxAviInfo->u8CurDmxMoviPos;
    }
    
    if (prSwdmxAviInfo->fgFirstVdChunk && prSwdmxAviInfo->fgFirstAdChunk)
    {
        prSwdmxAviInfo->i8FirstAVDel = prSwdmxAviInfo->u8FirstVdPos - prSwdmxAviInfo->u8FirstAdPos;
    }
    else if (prSwdmxAviInfo->fgFirstVdChunk && !prSwdmxAviInfo->fgFirstAdChunk)
    {
        prSwdmxAviInfo->i8FirstAVDel = prSwdmxAviInfo->u8CurDmxMoviPos - prSwdmxAviInfo->u8FirstVdPos;
    }
    else if (!prSwdmxAviInfo->fgFirstVdChunk && prSwdmxAviInfo->fgFirstAdChunk)
    {
        prSwdmxAviInfo->i8FirstAVDel = prSwdmxAviInfo->u8CurDmxMoviPos - prSwdmxAviInfo->u8FirstAdPos;
    }
    else
    {
        prSwdmxAviInfo->i8FirstAVDel = 0;
    }
    
    // Error Handling
    if (u4ChunkSize > prSwdmxInst->rFeederInfo.u4Size)
    {
        LOG(3, "u4ChunkSize > Feeder size = 0x%08x\n", u4ChunkSize);
        //add for mutil riff avi file
        if ((u1HdrType == eSWDMX_MEDIA_SKIPPED) && (!fgRangeEnd))
        {
            *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
            prSwdmxAviInfo->fgIsSkipIdxData = TRUE;
            if (fgRingBuf)
            {
                *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                                   (UINT32)(pucOriginalPtr) - 
                                                                   u4RingSize);
            }
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        if((i + prSwdmxAviInfo->u4ErrorData)> MIN_READ_SIZE*2)
        {
            return eSWDMX_HDR_PRS_FAIL;  
        }
        else
        {
            *pu4ConsumeSize = i + AVI_PARSER_OFFSET;  
             prSwdmxAviInfo->u4ErrorData = i;  
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    } 
    else if ((u4ChunkSize + AVI_PARSER_OFFSET + i) > u4AvailSize)
    {
        LOG(3, "Chunk size is 0x%08x larger than AvailSize, the size is 0x%08x\n", u4ChunkSize, u4AvailSize);
           
        //add to fun
        if (prSwdmxAviInfo->fgQAud)
        {
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            if(u2DmxMaxQSize <= u2DmxPictureQSize)
            { 
                LOG(3,"DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
                x_thread_delay(20);
                return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
            }
            u4DmxAudAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, (DMX_PID_TYPE_T)(DMX_PID_TYPE_ES_AUDIO),prSwdmxAviInfo->u1AudPid);
		    
            if (prSwdmxAviInfo->u4QAudSize < u4DmxAudAvailSize)
            {
		    if ((prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_AAC)
          	  	    &&(prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack))
	          {
	              AviHandAAC(prSwdmxInst, prSwdmxAviInfo->u4QAudSize,prSwdmxAviInfo->u4QAudPts, rDmxMMData.u4StartAddr, prSwdmxAviInfo->u4QAudTrack);
	               
	          }
                if (prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
                {
                    u1AudPid= prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u4QAudTrack] ;
                }
                if (prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
                {
                    u4Pts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u4QAudTrack];
                }
                else
                {
                    u4Pts = 0;
                }
                
                rDmxMMData.u1Idx = u1AudPid;
                rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
                rDmxMMData.u4StartAddr = prSwdmxAviInfo->u4QAudAddr;
                rDmxMMData.u4FrameSize = prSwdmxAviInfo->u4QAudSize;
                rDmxMMData.u4Pts = prSwdmxAviInfo->u4QAudPts;
                
                LOG(5, "Send Q Audio Data Size = 0x%08x pts = 0x%08x\n", u4ChunkSize, rDmxMMData.u4Pts);
    
                //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                {
                    LOG(0, "Demuxer fails to move data.\n");
                    *pu4ConsumeSize = 0;
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }
                prSwdmxAviInfo->fgIsBlock = TRUE;
                prSwdmxAviInfo->u4BlockChunkSize = u4ChunkSize + AVI_PARSER_OFFSET;
                *pu4ConsumeSize = 0;
    
                prSwdmxAviInfo->fgQAud = FALSE;
                prSwdmxAviInfo->u4QAudAddr = 0;
                prSwdmxAviInfo->u4QAudSize = 0;
                
                return eSWDMX_HDR_PRS_SUCCEED;
            }
        }
        
        if ((u1HdrType == eSWDMX_MEDIA_SKIPPED) && (!fgRangeEnd))
        {
            *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
            prSwdmxAviInfo->fgIsSkipIdxData = TRUE;
            if (fgRingBuf)
            {
                *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                                   (UINT32)(pucOriginalPtr) - 
                                                                   u4RingSize);
            }
            return eSWDMX_HDR_PRS_SUCCEED;
        }
        
        if (!fgRangeEnd)
        {
            prSwdmxAviInfo->fgIsBlock = TRUE;
            prSwdmxAviInfo->u4BlockChunkSize = u4ChunkSize + AVI_PARSER_OFFSET;
            *pu4ConsumeSize = i;
        }
        else
        {
            *pu4ConsumeSize = u4AvailSize;
            LOG(5, "Range End!!!\n");
        }
        
        return eSWDMX_HDR_FED_BUF_NOT_ENOUGH;
    }
    else if (!u4ChunkSize)
    {
        // dummy video chunk
        // still have to calculate the pts
        if (u1HdrType == eSWDMX_MEDIA_VIDEO)
        {
            _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
            prSwdmxAviInfo->u2SkipVidCount++;
            prSwdmxAviInfo->fgNoCalPts = FALSE;        
        }
        
        *pu4ConsumeSize = AVI_PARSER_OFFSET + i;
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    if(u1HdrType != eSWDMX_MEDIA_SKIPPED)
    {
        if (prSwdmxAviInfo->u8RecDmxMoviPos)
        {
            LOG(5, "CurDmxMoviPos when mode Change = %llx\n", 
                prSwdmxAviInfo->u8CurDmxMoviPos);
            if (prSwdmxAviInfo->u8RecDmxMoviPos > prSwdmxAviInfo->u8CurDmxMoviPos)
            {
                // Find the first audio chunk since 2X -> 1X
                if (u1HdrType == eSWDMX_MEDIA_AUDIO)
                {
                    if (prSwdmxAviInfo->u4ChangeMode & AVI_CHANGE_AUDIO_MODE)
                    {
                        UINT8 u1ATrack = prSwdmxAviInfo->u1CurATrackIdx;
                        UINT32 u4ModeChangePts = 0;
                        if(prSwdmxAviInfo->fgEnVideo)
                        {
                            u4ModeChangePts = VDP_GetPts(prSwdmxInst->u1B2rId);
                        }
                        else
                        {
                            u4ModeChangePts = STC_GetStcValue(prSwdmxInst->u1StcId);
                        }
                        //prSwdmxAviInfo->u4ModeChangPts = VDP_GetPts(0);
                        
                        if (u1ATrack < prSwdmxAviInfo->u1TotalATrack)
                        {
                            //if (prSwdmxAviInfo->u4AudPts[u1ATrack] >= (prSwdmxAviInfo->u4ModeChangPts + 27000))
                            if (prSwdmxAviInfo->u4AudPts[u1ATrack] >= (u4ModeChangePts + 18000))
                            {
                                LOG(5, "Audio Pts (0x%08x) > Mode ChangePts (0x%08x)\n", 
                                    prSwdmxAviInfo->u4AudPts[u1ATrack], 
                                    u4ModeChangePts);

                                //prSwdmxAviInfo->u4ModeChangPts = 0xFFFFFFFF;
                                prSwdmxAviInfo->u4ChangeMode = 0;
                                prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[u1ATrack];

                                AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[u1ATrack]);
                                prSwdmxAviInfo->u4AudStartPts = prSwdmxAviInfo->u4AudPts[u1ATrack];
                            }
                            else
                            {
                                LOG(5, "Audio Pts (0x%08x) < Mode ChangePts (0x%08x)\n", 
                                    prSwdmxAviInfo->u4AudPts[u1ATrack], 
                                    u4ModeChangePts);

                                _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1ATrack, u4ChunkSize);
                                u1HdrType = eSWDMX_MEDIA_SKIPPED;
                            }
                        }
                    }
                    if (prSwdmxAviInfo->u4ChangeMode & AVI_CHANGE_SUBTITLE_MODE)
                    {
                         UINT8 u1ATrack = prSwdmxAviInfo->u1CurATrackIdx;
                         _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1ATrack, u4ChunkSize);
                         u1HdrType = eSWDMX_MEDIA_SKIPPED;
                    }
                }
                else if (u1HdrType == eSWDMX_MEDIA_VIDEO)
                {
                     _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
                     //Skip other chunks
                     u1HdrType = eSWDMX_MEDIA_SKIPPED;
                }
                else if (u1HdrType != eSWDMX_MEDIA_SUBTITLE)
                {
                    if ((u1HdrType == eSWDMX_MEDIA_DRM_VIDEO) ||
                    	(u1HdrType == eSWDMX_MEDIA_DRM_AUDIO))
                    {
                        _SWDMX_AviPrsDrmChunk(prSwdmxInst, u1HdrType, rDmxMMData.u4StartAddr);
                    }

                    _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);

                    // Skip other chunks
                    u1HdrType = eSWDMX_MEDIA_SKIPPED;
                }
            }
            else
            {   
                LOG(5, "prSwdmxAviInfo->u8RecDmxMoviPos == prSwdmxAviInfo->u8CurDmxMoviPos\n");
                prSwdmxAviInfo->u8RecDmxMoviPos = 0;

                if(prSwdmxAviInfo->u4ChangeMode&AVI_CHANGE_AUDIO_MODE)
                {
                     if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
                     {
                         AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);
                     }
                }
                prSwdmxAviInfo->u4ChangeMode = 0;
            }
        }

        // Disable streams here
        if (u1HdrType == eSWDMX_MEDIA_AUDIO)
        {
            if((((UINT32)(prSwdmxAviInfo->u8VSeekPts>>32) != 0xFFFFFFFF) || 
                ((UINT32)(prSwdmxAviInfo->u8VSeekPts) != 0xFFFFFFFF))&&(prSwdmxAviInfo->fgMp3Abr))
            {
            	  if(prSwdmxAviInfo->u4CurVidPts < (UINT32)(prSwdmxAviInfo->u8VSeekPts))
            	  {
            	  	   u1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
            	  }
            }
            u1TrackIdx = prSwdmxAviInfo->u1CurATrackIdx;
            if (i4PlaySpeed != SWDMX_PLAY_SPEED_1X)
            {
                _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
            }
            else if (prSwdmxAviInfo->u4ChangeMode & AVI_CHANGE_AUDIO_MODE)
            {
                UINT32 u4ModeChangePts = VDP_GetPts(prSwdmxInst->u1B2rId);
                //prSwdmxAviInfo->u4ModeChangPts = VDP_GetPts(prSwdmxInst->u1B2rId);
                
                if (u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)
                {
                    //if (prSwdmxAviInfo->u4AudPts[u1TrackIdx] >= (prSwdmxAviInfo->u4ModeChangPts + 27000))
                    if (prSwdmxAviInfo->u4AudPts[u1TrackIdx] >= (u4ModeChangePts + 18000))
                    {
                        LOG(5, "Audio Pts (0x%08x) > Mode ChangePts (0x%08x)\n", 
                            prSwdmxAviInfo->u4AudPts[u1TrackIdx], 
                            u4ModeChangePts);

                        //prSwdmxAviInfo->u4ModeChangPts = 0xFFFFFFFF;
                        prSwdmxAviInfo->u4ChangeMode = 0;
                        prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
                        AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[u1TrackIdx]);
                        prSwdmxAviInfo->u4AudStartPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
                    }
                    else
                    {
                        _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
                        u1HdrType = eSWDMX_MEDIA_SKIPPED;
                    }
                }
            }
            else if((((UINT32)(prSwdmxAviInfo->u8ASeekPts>>32) != 0xFFFFFFFF) || 
                ((UINT32)(prSwdmxAviInfo->u8ASeekPts) != 0xFFFFFFFF) )||
                (prSwdmxAviInfo->u8AbRepeatAudPts != 0))
            {
                if((u1TrackIdx < prSwdmxAviInfo->u1TotalATrack) &&
                    ((prSwdmxAviInfo->u4AudPts[u1TrackIdx] < (UINT32)prSwdmxAviInfo->u8ASeekPts)||
                    ( prSwdmxAviInfo->u8AbRepeatAudPts != 0)))
                {
                    _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
                    u1HdrType = eSWDMX_MEDIA_SKIPPED;
                }
            }
            
            if(prRangeList->fgSetRenderPosition)
            {
                u1HdrType = eSWDMX_MEDIA_SKIPPED;
            }
        }
        else if ((u1HdrType == eSWDMX_MEDIA_DRM_VIDEO) ||
        	(u1HdrType == eSWDMX_MEDIA_DRM_AUDIO))
        {
            _SWDMX_AviPrsDrmChunk(prSwdmxInst, u1HdrType, rDmxMMData.u4StartAddr);
            u1HdrType = eSWDMX_MEDIA_SKIPPED;
        }

        if(((u1HdrType == eSWDMX_MEDIA_AUDIO) || 
          (u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO))&&
          (prSwdmxAviInfo->prDmxRangeInfo->b_thumbnail_pb))
        {
            u1HdrType = eSWDMX_MEDIA_SKIPPED;
        }
    }
    
    if (u1HdrType == eSWDMX_MEDIA_SKIPPED)
    {
        *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
        if (fgRingBuf)
        {
            *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                               (UINT32)(pucOriginalPtr) - 
                                                               u4RingSize);
        }
        return eSWDMX_HDR_PRS_SUCCEED;
    }

  if (u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO)
    {
        if ((!prSwdmxAviInfo->fgEnAudio)
             &&(!prSwdmxAviInfo->fgEnADmx))
        {
            *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
            if (fgRingBuf)
            {
                *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                                   (UINT32)(pucOriginalPtr) - 
                                                                   u4RingSize);
            }
            _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }

    // Get available size here (we don't have to query subtitle available size here)
    if ((u1HdrType == eSWDMX_MEDIA_VIDEO) ||
        ((u1HdrType == eSWDMX_MEDIA_AUDIO) && (i4PlaySpeed == SWDMX_PLAY_SPEED_1X)))
    {
        UINT8 u1PidIdx;
        UINT8 u1PidType = 0;

        if(u1HdrType == eSWDMX_MEDIA_VIDEO)
        {
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                if(prSwdmxAviInfo->fgIsLeftVideo)
                {
                    u1PidIdx = prSwdmxAviInfo->u1VidPid;
                }
                else
                {
                    u1PidIdx = prSwdmxAviInfo->u1Vid2Pid;
                }
            }
            else
            {
                 u1PidIdx = prSwdmxAviInfo->u1VidPid;
            }
        }
        else
        {
            u1PidIdx = prSwdmxAviInfo->u1AudPid;
        }
        
        u1PidType = (u1HdrType == eSWDMX_MEDIA_VIDEO) ? (DMX_PID_TYPE_ES_VIDEO) : (DMX_PID_TYPE_ES_AUDIO);
        
        DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
        if(u2DmxMaxQSize <= u2DmxPictureQSize)
        { 
            LOG(3,"DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
            x_thread_delay(20);
            return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
        }

        u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, 
                                                                                (DMX_PID_TYPE_T)u1PidType,
                                                                                u1PidIdx);
        if ((u1HdrType == eSWDMX_MEDIA_AUDIO) &&
            (prSwdmxAviInfo->fgEnVideo)&&
            (!prSwdmxAviInfo->fgQAud) &&
            (u4ChunkSize > u4DmxAvailSize) &&
            (u4ChunkSize > (prSwdmxAviInfo->u4AFifoSize/2)) &&
            (u1TrackIdx < prSwdmxAviInfo->u1TotalATrack))
        {
            _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);

            prSwdmxAviInfo->fgQAud = TRUE;
            prSwdmxAviInfo->u4QAudAddr = rDmxMMData.u4StartAddr;
            prSwdmxAviInfo->u4QAudSize = u4ChunkSize;
            prSwdmxAviInfo->u4QAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
            prSwdmxAviInfo->u4QAudTrack = u1TrackIdx;
            
            *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
            if (fgRingBuf)
            {
                *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                               (UINT32)(pucOriginalPtr) - 
                                                               u4RingSize);
            }

            prSwdmxAviInfo->fgIsBlock = FALSE;
            prSwdmxAviInfo->u4BlockChunkSize = 0;

            return eSWDMX_HDR_PRS_SUCCEED;
        }

        if (prSwdmxAviInfo->fgQAud)
        {
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            if(u2DmxMaxQSize <= u2DmxPictureQSize)
            { 
                LOG(3,"DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
                x_thread_delay(20);
                return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
            }
            u4DmxAudAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, (DMX_PID_TYPE_T)(DMX_PID_TYPE_ES_AUDIO),prSwdmxAviInfo->u1AudPid);
            if (prSwdmxAviInfo->u4QAudSize < u4DmxAudAvailSize)
            {
                if (prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_AAC)
                {
                    if(prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
                    {
                   	    AviHandAAC(prSwdmxInst, prSwdmxAviInfo->u4QAudSize,prSwdmxAviInfo->u4QAudPts, rDmxMMData.u4StartAddr, prSwdmxAviInfo->u4QAudTrack);
                    }
                }
                if (prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
                {
                    u1AudPid= prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u4QAudTrack] ;
                }
                if (prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
                {
                    u4Pts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u4QAudTrack];
                }
                else
                {
                    u4Pts = 0;
                }

                rDmxMMData.u1Idx = u1AudPid;
                rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
                rDmxMMData.u4StartAddr = prSwdmxAviInfo->u4QAudAddr;
                rDmxMMData.u4FrameSize = prSwdmxAviInfo->u4QAudSize;
                rDmxMMData.u4Pts = prSwdmxAviInfo->u4QAudPts;
                
                LOG(5, "Send Q Audio Data Size = 0x%08x pts = 0x%08x\n", u4ChunkSize, rDmxMMData.u4Pts);

                //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                {
                    LOG(0, "Demuxer fails to move data.\n");
                    *pu4ConsumeSize = 0;
                    return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                }

                prSwdmxAviInfo->fgIsBlock = TRUE;
                prSwdmxAviInfo->u4BlockChunkSize = u4ChunkSize + AVI_PARSER_OFFSET;
                *pu4ConsumeSize = 0;

                prSwdmxAviInfo->fgQAud = FALSE;
                prSwdmxAviInfo->u4QAudAddr = 0;
                prSwdmxAviInfo->u4QAudSize = 0;
                
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            else if(u1HdrType == eSWDMX_MEDIA_AUDIO)
            {
                *pu4ConsumeSize = 0;
                x_thread_delay(20);
                return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
            }
        }
                
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        if (prSwdmxAviInfo->fgVDecIsPause
            && _fgBiAviAudioReady)
        {
            _SWDMX_AviVideoWakeUp(prSwdmxInst);
            prSwdmxAviInfo->fgVDecIsPause = FALSE;
        }
#endif

        if (u4ChunkSize > u4DmxAvailSize)
        {    
            if (prSwdmxAviInfo->fgVDecIsPause)
            {
                _SWDMX_AviVideoWakeUp(prSwdmxInst);
                prSwdmxAviInfo->fgVDecIsPause = FALSE;
            }
            
            if ((u1HdrType == eSWDMX_MEDIA_AUDIO) &&
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                !_fgBiAviAudioReady &&
#endif
                prSwdmxAviInfo->fgAudIsReady)
            {
                AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                STC_StartStc(prSwdmxInst->u1StcId);
                prSwdmxAviInfo->fgAudIsReady = FALSE;
            }

            prSwdmxInst->fgFifoFull = TRUE;
                        
            LOG(7, "Chunk is blocked u1HdrType = %ld, u4ChunkSize = %ld, u4DmxAvailSize = %ld\n", 
                u1HdrType, 
                u4ChunkSize,
                u4DmxAvailSize);
            *pu4ConsumeSize = 0;
            x_thread_delay(20);
            return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
        }

        _SWDMX_InitDmx(prSwdmxInst, u1HdrType, prSwdmxInst->u1SwdmxId, NULL, FALSE);
    }
    if (u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO)
    {
        if (prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_AAC)
        {
        	  if(u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)
        	  {
                  AviHandAAC(prSwdmxInst, u4ChunkSize,prSwdmxAviInfo->u4QAudPts,rDmxMMData.u4StartAddr,u1TrackIdx);
              }
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        u1TrackIdx = prSwdmxAviInfo->u1CurATrackIdx;

        if (u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)
        {
            BOOL fgRet;
            UINT32 u4CurPts, u4APts;
            
            //UCHAR *pBuf = (UCHAR*)(rDmxMMData.u4StartAddr);
            UINT32 u4FirstAdData = 0;
            UINT32  u413thBite =0;
            UINT32 u4TimeSeekAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];

            x_memcpy(&u4FirstAdData,(void*)VIRTUAL(rDmxMMData.u4StartAddr),4);
            u413thBite = (u4FirstAdData & 0x00001000)>>12; 
            u4FirstAdData &= 0xFFF;

            if ((prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_MP3) &&
                (!prSwdmxAviInfo->fgAudIsLoaded) &&
                ((u4FirstAdData == 0xFFF)||
                ((u4FirstAdData == 0xFFE)&&(u413thBite == 0))))
            {
                prSwdmxAviInfo->fgBadAudio = TRUE;
            }
            
            if((prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_MPEG)&&
                (!prSwdmxAviInfo->fgAudIsLoaded) &&
                (u4FirstAdData == 0xFFF))
            {
                prSwdmxAviInfo->fgBadAudio = TRUE;
            } 
	        prSwdmxAviInfo->u4NumSentAud++;
            u4Pts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
            
            if(prSwdmxAviInfo->fgRecSpeedChangeForAud)
            {
                if (prSwdmxAviInfo->i4RecPrevSpeedForAud == 2000 || 
                    prSwdmxAviInfo->i4RecPrevSpeedForAud == 1||
                    ((prSwdmxAviInfo->i4RecPrevSpeedForAud > 1)&&
                    (prSwdmxAviInfo->i4RecPrevSpeedForAud<1000)))
                {
                    u4CurPts = (prSwdmxAviInfo->fgEnVideo) ? (VDP_GetPts(prSwdmxInst->u1B2rId)) : (STC_GetStcValue(prSwdmxInst->u1StcId));
                    
                    prSwdmxAviInfo->u4TriggerAudPts = u4CurPts + 27000;

                    fgRet = DMX_AudHandler_SeekAndUpdate(
                        prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx],
                        prSwdmxAviInfo->u4TriggerAudPts, &u4APts);

                    LOG(5, "Find Audio Track Data for 2X -> 1X\n");

                    if (fgRet)
                    {
                        LOG(5, "Find audio in the pre-buffer\n");
                        prSwdmxAviInfo->u4TriggerAudPts = u4APts;
                    }
                    else
                    {
                        LOG(5, "Can't find audio in the pre-buffer\n");
                        prSwdmxAviInfo->u4TriggerAudPts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
                    }

                    AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4TriggerAudPts);
                    LOG(5, "prSwdmxAviInfo->u4TriggerAudPts = 0x%08x\n", prSwdmxAviInfo->u4TriggerAudPts);
                }
                else
                {
                    UNUSED(DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx]));
                }

                prSwdmxAviInfo->fgRecSpeedChangeForAud = FALSE;
                prSwdmxAviInfo->i4RecPrevSpeedForAud = i4PlaySpeed;
                DMX_AudHandler_SetActivePidx(prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx]);
                prSwdmxAviInfo->fgSetActivePid = TRUE;
            }

            if (((prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_MP3)||
                (prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_MPEG))&&
                (! prSwdmxAviInfo->fgGetMp3Status)&&!(prSwdmxAviInfo->fgMp3Abr))
            {
                 UCHAR *puMp3Hdr =  (UCHAR*)rDmxMMData.u4StartAddr;
                 UCHAR uParHdr[4]= {0};
                 UINT32  u4HeaderSize = 0;
                 UINT8 i = 0;
                 
                 prSwdmxAviInfo->u4AudNotParseSize =
                     prSwdmxAviInfo->u4AudNotParseSize +u4ChunkSize;

                 if((prSwdmxAviInfo->u4AudNotParseSize > prSwdmxAviInfo->u4AudFrameSize)&&
                    (prSwdmxAviInfo->u4AudFrameSize != 0))
                 {
                     prSwdmxAviInfo->u4AudNotParseSize -= prSwdmxAviInfo->u4AudFrameSize;
                     puMp3Hdr = (UCHAR*) ((UINT32)puMp3Hdr + u4ChunkSize -prSwdmxAviInfo->u4AudNotParseSize);
                     prSwdmxAviInfo->u4AudFrameSize = 0;
                 }
                 while(prSwdmxAviInfo->u4AudNotParseSize > prSwdmxAviInfo->u4AudFrameSize)
                 {
                     prSwdmxAviInfo->u4AudNotParseSize -= prSwdmxAviInfo->u4AudFrameSize;
                     
                      puMp3Hdr = (UCHAR*) ((UINT32)puMp3Hdr- u4HeaderSize + prSwdmxAviInfo->u4AudFrameSize);
                      u4HeaderSize = 0;
                      
                      while(*puMp3Hdr == 0)
                      {
                          puMp3Hdr++;
                          
                           if((UINT32)puMp3Hdr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                          {
                               puMp3Hdr = 
                                 (UCHAR*)((UINT32)puMp3Hdr - VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) + 
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                          }
                          prSwdmxAviInfo->u4AudNotParseSize --;
                      }

                     if((UINT32)puMp3Hdr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                      {
                           puMp3Hdr = 
                                (UCHAR*)((UINT32)puMp3Hdr - VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) + 
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                      }
                      for (i = 0; i<4; i++)
                      {
                      	  uParHdr[i] = *puMp3Hdr;
                      	  puMp3Hdr++;
                          u4HeaderSize ++;
                          
                          if((UINT32)puMp3Hdr >= VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
                          {
                               puMp3Hdr = 
                                 (UCHAR*)((UINT32)puMp3Hdr - VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) + 
                                VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                          }
                      	
                      }
                      
                      if(!(MPEGA_IS_VALID_HDR(uParHdr)))
                      {
                          LOG(3, "illegal audio header \n");
                          break;
                      }

                      prSwdmxAviInfo->u4AudFrameSize = dwMpA_GetFrmSz(uParHdr, &prSwdmxAviInfo->u4Mp3Rate);

                      if(prSwdmxAviInfo->u4AudFrameSize  == 0)
                      {
                           LOG(3, "illegal u4AudFrameSize\n");
                           prSwdmxAviInfo->fgGetMp3Status = TRUE;
                           break;
                      }
                      
                     if ((prSwdmxAviInfo->u4Mp3LastRate !=0)&&
                      (prSwdmxAviInfo->u4Mp3LastRate != prSwdmxAviInfo->u4Mp3Rate))
                     {
                        prSwdmxAviInfo->fgMp3Vbr = TRUE;
                        LOG(3, "Mp3 is vbr, prev rate = %d, curr rate = %d.\n", 
                            prSwdmxAviInfo->u4Mp3LastRate, prSwdmxAviInfo->u4Mp3Rate);
                     }    
                     prSwdmxAviInfo->u4Mp3LastRate = prSwdmxAviInfo->u4Mp3Rate;
                 }

            }

            if (prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_AAC)
            {
            	  if(u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)
            	  {
                      AviHandAAC(prSwdmxInst, u4ChunkSize,prSwdmxAviInfo->u4QAudPts,rDmxMMData.u4StartAddr,u1TrackIdx);
                }
            }

            //enable audio according to pts, need fix by aud index table
            if(((UINT32)(prSwdmxAviInfo->u8ASeekPts>>32) != 0xFFFFFFFF)
                || ((UINT32)(prSwdmxAviInfo->u8ASeekPts) != 0xFFFFFFFF))
            {
                if((prSwdmxAviInfo->u4AudPts[u1TrackIdx] >= (UINT32)prSwdmxAviInfo->u8ASeekPts))
                {

                    AUD_SetStartPts(prSwdmxInst->u1AdecId, u4TimeSeekAudPts);
                    prSwdmxAviInfo->u4AudStartPts = u4TimeSeekAudPts;

                    if (prSwdmxAviInfo->fgEnVideo)
                    {
                        VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                        prSwdmxAviInfo->u4TriggerAudPts = (UINT32)u4TimeSeekAudPts;
                
                        VDEC_Pause(prSwdmxInst->u1VdecId);
                        prSwdmxAviInfo->fgVDecIsPause = TRUE;
                    }
                    
                    LOG(7, "Got Aud T PTS 0x%x, Seek 0x%llx\n", 
                        u4Pts, (UINT32)prSwdmxAviInfo->u8ASeekPts);            
                    prSwdmxAviInfo->u8ASeekPts = 0xFFFFFFFF;
                    prSwdmxAviInfo->u8ASeekPts <<= 32;
                    prSwdmxAviInfo->u8ASeekPts += 0xFFFFFFFF;
                }
                else
                {
                    //skip audio data in seek process
                    *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
                    if (fgRingBuf)
                    {
                        *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                                   (UINT32)(pucOriginalPtr) - 
                                                                   u4RingSize);
                    }
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
            }           
            if((prSwdmxAviInfo->fgVdecNotSupport)&&
                (prSwdmxAviInfo->fgAudIsReady))
            {
                 AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                 STC_StartStc(prSwdmxInst->u1StcId);
                 prSwdmxAviInfo->fgAudIsReady = FALSE;
            }
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        UINT16 u2MaxQNum, u2CurQNum;
        if(prSwdmxAviInfo->fgQAud)
        {
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            if(u2DmxMaxQSize <= u2DmxPictureQSize)
            { 
                LOG(3,"DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
                x_thread_delay(20);
                return eSWDMX_HDR_DMX_BUF_NOT_ENOUGH;
            }
        	u4DmxAudAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, (DMX_PID_TYPE_T)(DMX_PID_TYPE_ES_AUDIO),prSwdmxAviInfo->u1AudPid);   
          if (prSwdmxAviInfo->u4QAudSize < u4DmxAudAvailSize)
          {
          	  if (prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_AAC)
	            {
	           	    if(prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
	           	    {
	              	   AviHandAAC(prSwdmxInst, prSwdmxAviInfo->u4QAudSize,prSwdmxAviInfo->u4QAudPts, rDmxMMData.u4StartAddr, prSwdmxAviInfo->u4QAudTrack);
	                }
	            }

              if (prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
              {
                  u1AudPid= 
                      prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u4QAudTrack] ;
              }
              if (prSwdmxAviInfo->u4QAudTrack < prSwdmxAviInfo->u1TotalATrack)
              {
                  u4Pts = prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u4QAudTrack];
              }
              else
              {
                  u4Pts = 0;
              }


              rDmxMMData.u1Idx = u1AudPid;
              rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
              rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
              rDmxMMData.u4StartAddr = prSwdmxAviInfo->u4QAudAddr;
              rDmxMMData.u4FrameSize = prSwdmxAviInfo->u4QAudSize;
              rDmxMMData.u4Pts = prSwdmxAviInfo->u4QAudPts;
              
              LOG(5, "Send Q Audio Data Size = 0x%08x pts = 0x%08x\n", u4ChunkSize, rDmxMMData.u4Pts);

              //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
              if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
              {
                  LOG(0, "Demuxer fails to move data.\n");
                  *pu4ConsumeSize = 0;
                  return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
              }

              prSwdmxAviInfo->fgIsBlock = TRUE;
              prSwdmxAviInfo->u4BlockChunkSize = u4ChunkSize + AVI_PARSER_OFFSET;
              *pu4ConsumeSize = 0;

              prSwdmxAviInfo->fgQAud = FALSE;
              prSwdmxAviInfo->u4QAudAddr = 0;
              prSwdmxAviInfo->u4QAudSize = 0;
              
              return eSWDMX_HDR_PRS_SUCCEED;
          }
        }
    
        
        if (prSwdmxAviInfo->fgEnVideo == FALSE)
        {
            *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
            if (fgRingBuf)
            {
                *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                                   (UINT32)(pucOriginalPtr) - 
                                                                   u4RingSize);
            }
             x_thread_delay(33);
            return eSWDMX_HDR_PRS_SUCCEED;
        }

#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        if (_fgBiAviAudioReady)
        {
            if (prSwdmxAviInfo->fgVDecIsPause)
            {
                _SWDMX_AviVideoWakeUp(prSwdmxInst);
                prSwdmxAviInfo->fgVDecIsPause = FALSE;
            }
        }
#endif

        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(prSwdmxAviInfo->fgIsLeftVideo)
            {
                VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
            }
            else
            {
                VDEC_GetQueueInfo(prSwdmxInst->u1Vdec2Id, &u2CurQNum, &u2MaxQNum);
            }
        }
        else
        {
            VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        }

        if (prSwdmxAviInfo->u4VidFps >= AVI_STANDARD_FPS)
        {
            u2VdecMaxQNum = AVI_MAX_VDEC_Q_NUM(prSwdmxAviInfo->u4VidFps);
        }
        else
        {
            u2VdecMaxQNum = AVI_MAX_VDEC_Q_NUM(AVI_STANDARD_FPS);
        }
        
        if (abs(prSwdmxAviInfo->i8FirstAVDel)<AVI_BAD_INTERLEAVE_THRESHOLD)
        {
            if( (u2CurQNum > u2VdecMaxQNum)
            && ((prSwdmxAviInfo->fgFirstVdChunk && prSwdmxAviInfo->fgFirstAdChunk) || !prSwdmxAviInfo->fgEnAudio))
            {
                if (prSwdmxAviInfo->fgVDecIsPause)
                {
                    _SWDMX_AviVideoWakeUp(prSwdmxInst);
                    prSwdmxAviInfo->fgVDecIsPause = FALSE;
                }
                if(!STC_IsStartedStc(prSwdmxInst->u1StcId))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                    LOG(1, "force to call STC_StartStc!!!!\n");
                }
                LOG(7, "Line %d, u2CurQNum = %ld, u2MaxQNum = %ld .\n", __LINE__, u2CurQNum, u2MaxQNum);
                *pu4ConsumeSize = 0;
                 prSwdmxInst->fgFifoFull = TRUE;
                x_thread_delay(30);
                
                return eSWDMX_HDR_PRS_SUCCEED;
            }
            
        }
        else
        {
            if(prSwdmxAviInfo->fgEnAudio == TRUE)
            {
                LOG(3,"The file is badinterleave!!!!!!!!!!!\n");
            }
            
            if (u2CurQNum > u2VdecMaxQNum)
            {
                if (prSwdmxAviInfo->fgVDecIsPause)
                {
                    _SWDMX_AviVideoWakeUp(prSwdmxInst);
                    prSwdmxAviInfo->fgVDecIsPause = FALSE;
                }
                if(!STC_IsStartedStc(prSwdmxInst->u1StcId))
                {
                    STC_StartStc(prSwdmxInst->u1StcId);
                    LOG(1, "badinterleave, force to call STC_StartStc!!!!\n");
                }
                LOG(7, "Line %d, u2CurQNum = %ld, u2MaxQNum = %ld .\n", __LINE__, u2CurQNum, u2MaxQNum);
                *pu4ConsumeSize = 0;            
                 prSwdmxInst->fgFifoFull = TRUE;
                x_thread_delay(30);
                
                return eSWDMX_HDR_PRS_SUCCEED;
            }
       }
        
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(prSwdmxAviInfo->fgIsLeftVideo)
           {
                u4Pts = prSwdmxAviInfo->u4CurVidPts;
            }
            else
            {
                u4Pts = prSwdmxAviInfo->u4CurSubVidPts;
            }
         }
         else
         {
             u4Pts = prSwdmxAviInfo->u4CurVidPts;
         }
        prSwdmxAviInfo->fgNoCalPts = FALSE;

        // Record every video's chunk position.
        if(prSwdmxAviInfo->fgIs3DFile)
        {
                if(prSwdmxAviInfo->fgIsLeftVideo)
                {
                    prSwdmxAviInfo->u8VidOffset = prSwdmxAviInfo->u8CurDmxMoviPos;
                }
                else
                {
                    prSwdmxAviInfo->u8SubVidOffset = prSwdmxAviInfo->u8CurDmxMoviPos;
                }
         }
        else
        {
             prSwdmxAviInfo->u8VidOffset = prSwdmxAviInfo->u8CurDmxMoviPos;
        }
        if(((UINT32)(prSwdmxAviInfo->u8VSeekPts>>32) != 0xFFFFFFFF)
            || ((UINT32)(prSwdmxAviInfo->u8VSeekPts) != 0xFFFFFFFF))
        {
            if(u4Pts > (UINT32)prSwdmxAviInfo->u8VSeekPts)
            {
                if ((prSwdmxAviInfo->fgMp3Abr) &&
                    (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack))
                {
                    prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx] = 
                        (UINT32)prSwdmxAviInfo->u8VSeekPts;
                    prSwdmxAviInfo->u8ASeekPts = prSwdmxAviInfo->u8VSeekPts;
                }
                LOG(5, "Got Vid T PTS 0x%x, Seek 0x%llx\n", 
                    u4Pts, (UINT32)prSwdmxAviInfo->u8VSeekPts); 
                
                prSwdmxAviInfo->fgAbRepeat = FALSE;
                prSwdmxAviInfo->u8VSeekPts = 0xFFFFFFFF;
                prSwdmxAviInfo->u8VSeekPts <<= 32;
                prSwdmxAviInfo->u8VSeekPts += 0xFFFFFFFF;
                if(prRangeList->prDmxRangeInfo->b_drm)
                {
                    UINT32 u4Flag;
                    DMX_MM_DRM_AES_T rDRM = {0};
            
                    UINT8 u1VidPidIdx = prSwdmxAviInfo->u1VidPid;
                    
                    rDRM.fgCBC = FALSE;
                    rDRM.u4KeyBitLen = AVI_DRM_AES_SIZE_128;
                    rDRM.u4Offset = 0;
                    rDRM.u4EncryLen = 0;
                    u4Flag = DMX_MM_DRM_FLAG_ALL;
                    LOG(5, "A <-> B Repeat, Reset DMX!!!\r\n");
                    DMX_MM_SetDRMMode(u1VidPidIdx, DMX_DRM_MODE_AES);
                    DMX_MM_SetAES(u1VidPidIdx, u4Flag , &rDRM);
                }
            }

        }
        if((prSwdmxAviInfo->u8AbRepeatVidPts!=0)&&
        (u4Pts >prSwdmxAviInfo->u8AbRepeatVidPts))
        {
            UINT32 i =0;
            for (i =0; (i<prSwdmxAviInfo->u1TotalATrack)&&(i<AVI_DIVX_MAX_AUD_TRACK); i++)
            {
                prSwdmxAviInfo->u4AudPts[i] = 
                       prSwdmxAviInfo->u8AbRepeatAudPts;
            }
            prSwdmxAviInfo->u8AbRepeatVidPts = 0;
            prSwdmxAviInfo->u8AbRepeatAudPts = 0;
        }
        
        if(prSwdmxAviInfo->fgAbRepeat || 
          (prSwdmxAviInfo->u4AbDmxPos != 0))
        {
            UINT32 u4StartAddr = 0;
            if((u4ChunkSize < AVI_MPEG4_NOT_CODEC_SIZE)&&
              (prSwdmxAviInfo->fgAbRepeat))
            {
                prSwdmxAviInfo->u4AbPts = u4Pts;
            }
            else
            {
                if((u4ChunkSize < AVI_MPEG4_NOT_CODEC_SIZE)&&
                   (!prSwdmxAviInfo->fgAbOneMore))
                {
                    prSwdmxAviInfo->u4AbPts = u4Pts;
                    if(!prSwdmxAviInfo->fgAbRepeat)
                    {
                        rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                	    rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
                	    rDmxMMData.u4FrameSize = u4ChunkSize;
                	    rDmxMMData.u4Pts = prSwdmxAviInfo->u4AbPts;
                	    rDmxMMData.u4Dts = prSwdmxAviInfo->u4AbPts;
                        _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
                	    LOG(11, "Header Type = %ld u4FrameSize = 0x%08x pts = 0x%08x\n", 
                	        u1HdrType, u4ChunkSize, u4Pts);  
                	    //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
                	    if(((prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV7) ||
					     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV8)) ||
					     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV9))
					    {
					         rDmxMMData.fgFrameHead = TRUE;
					    }
									    
                	    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                	    {
                	        LOG(0, "Demuxer fails to move data.\n");
                	        *pu4ConsumeSize = 0;
                	        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                	    }
                    }
                    *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
                    if (fgRingBuf)
                    {
                        *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                                           (UINT32)(pucOriginalPtr) - 
                                                                           u4RingSize);
                    }
                    _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
                    prSwdmxAviInfo->fgAbOneMore = TRUE;
                    return eSWDMX_HDR_PRS_SUCCEED;
                }
                u4StartAddr = rDmxMMData.u4StartAddr;
                if(prSwdmxAviInfo->u4AbDmxPos != 0)
                {
                    if(prSwdmxAviInfo->fgIs3DFile)
                    {
                        if(prSwdmxAviInfo->fgIsLeftVideo)    
                        {
                            rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
                        }
                        else
                        {
                             rDmxMMData.u1Idx = prSwdmxAviInfo->u1Vid2Pid;
                        }
                    }
                    else
                    {
                        rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
                    }
                    
                    if(((prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV7) ||
				     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV8)) ||
				     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV9))
				    {
				         rDmxMMData.fgFrameHead = TRUE;
				    }
                    rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
                    rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
                    rDmxMMData.u4StartAddr = (UINT32)(VIRTUAL(prSwdmxAviInfo->u4AbDmxPos));
                    rDmxMMData.u4FrameSize = prSwdmxAviInfo->u4AbChunkSize;
                    rDmxMMData.u4Pts = prSwdmxAviInfo->u4AbPts;
                    rDmxMMData.u4Dts = prSwdmxAviInfo->u4AbPts;

                    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
                    {
                        LOG(0, "Demuxer fails to move data.\n");
                        *pu4ConsumeSize = 0;
                        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
                    } 
                }
                prSwdmxAviInfo->u4AbChunkSize = u4ChunkSize;
        	    prSwdmxAviInfo->u4AbDmxPos = u4StartAddr;
        	    prSwdmxAviInfo->u4AbPts = u4Pts;
                rDmxMMData.u4StartAddr = u4StartAddr;

                if(!prSwdmxAviInfo->fgAbRepeat)
                {
                    prSwdmxAviInfo->u4AbChunkSize = 0;
        	        prSwdmxAviInfo->u4AbDmxPos = 0;
        	        prSwdmxAviInfo->u4AbPts = 0;
                    prSwdmxAviInfo->fgAbOneMore = FALSE;
                }
            }
        }
        
    }
    else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        prSwdmxAviInfo->u4NumSentSub++;
        prSwdmxAviInfo->u8PrevSubPos = prSwdmxAviInfo->u8CurDmxMoviPos;
        prSwdmxAviInfo->u4RecVidPtsForSub = prSwdmxAviInfo->u4CurVidPts;
        for (j = 0; (j < prSwdmxAviInfo->u1TotalATrack) && (j < AVI_DIVX_MAX_AUD_TRACK); j++)
        {
            prSwdmxAviInfo->u4RecAudPtsForSub[j] = 
                prSwdmxAviInfo->u4AudPts[j] ;
            LOG(5, "Avi Seek Audio pts = 0x%08x\n", 
                prSwdmxAviInfo->u4AudPts[j]);

        }
    }

    if ((u1HdrType == eSWDMX_MEDIA_AUDIO) ||
        (u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO))
    {
        if (u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)
        {
            u1AudPid = prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx];
        }
        if (u1TrackIdx < prSwdmxAviInfo->u1TotalATrack)
        {
            u4Pts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];
        }
        else
        {
            u4Pts = 0;
        }
    }

    if (prRangeList->prDmxRangeInfo->b_drm)
    {
        UINT32 u4Flag;
        DMX_MM_DRM_AES_T rDRM = {0};
        DIVX_DRM_CRYPTO_INFO_T *prDrmInfo;

        UINT8 u1VidPidIdx = prSwdmxAviInfo->u1VidPid;
        
        rDRM.fgCBC = FALSE;
        rDRM.u4KeyBitLen = AVI_DRM_AES_SIZE_128;

        prDrmInfo = 
            prRangeList->prDmxRangeInfo->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info;
        
        LOG(6, "Drm Type = %ld, DRM key idx = %ld\n",  
            prSwdmxAviInfo->u1DrmType, prSwdmxAviInfo->u2FrameKeyIdx);
        
        if ((u1HdrType == eSWDMX_MEDIA_VIDEO) &&
            (prSwdmxAviInfo->u1DrmType == eSWDMX_MEDIA_DRM_VIDEO) &&
            (prSwdmxAviInfo->u4EncryptLength != 0) &&
            (prSwdmxAviInfo->u2FrameKeyIdx < 128) &&
            prDrmInfo != NULL)
        {
            rDRM.u4Offset = prSwdmxAviInfo->u4EncryptOffset;
            rDRM.u4EncryLen = prSwdmxAviInfo->u4EncryptLength;
            
            x_memcpy(rDRM.au1Key, 
                prDrmInfo->aui1_frame_keys[prSwdmxAviInfo->u2FrameKeyIdx],
                (AVI_DRM_AES_SIZE_128/8));

            u4Flag = DMX_MM_DRM_FLAG_ALL;
            DMX_MM_SetDRMMode(u1VidPidIdx, DMX_DRM_MODE_AES);
            DMX_MM_SetAES(u1VidPidIdx, u4Flag , &rDRM);
        }
        else if (((u1HdrType == eSWDMX_MEDIA_AUDIO) ||(u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO)) &&
            (prDrmInfo != NULL && (prDrmInfo->ui1_flag & DIVX_DRM_AUDIO_PROTECTION)))
        {
            if (u4ChunkSize < prDrmInfo->ui1_protected_audio_offset)
            {
                LOG(1, "Audio DRM offset is larger than Audio Chunk Size Impossible\n");
            }
            else
            {

                if (u4ChunkSize < prDrmInfo->ui1_protected_audio_offset + prDrmInfo->ui1_protected_audio_crypto_size)
                {
                    rDRM.u4EncryLen = u4ChunkSize - prDrmInfo->ui1_protected_audio_offset;
                }
                else
                {
                    rDRM.u4EncryLen = prDrmInfo->ui1_protected_audio_crypto_size;
                }
                rDRM.u4Offset = prDrmInfo->ui1_protected_audio_offset;
                
                x_memcpy(rDRM.au1Key, 
                    prDrmInfo->aui1_frame_keys[0],
                    (AVI_DRM_AES_SIZE_128/8));

                u4Flag = DMX_MM_DRM_FLAG_ALL;
                DMX_MM_SetDRMMode(u1AudPid, DMX_DRM_MODE_AES);
                DMX_MM_SetAES(u1AudPid, u4Flag , &rDRM);
            }
        }
        LOG(6, "Drm u4Offset = %ld, DRM u4EncryLen = %ld\n",  
            rDRM.u4Offset, rDRM.u4EncryLen);

        prSwdmxAviInfo->u1DrmType = eSWDMX_MEDIA_NONE;
    }

    switch (u1HdrType)
    {
        case eSWDMX_MEDIA_VIDEO:
            if(prSwdmxAviInfo->fgIs3DFile)
            {
                if(prSwdmxAviInfo->fgIsLeftVideo)
                {
                    rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
                }
                else
                {
                    rDmxMMData.u1Idx = prSwdmxAviInfo->u1Vid2Pid;
                }
            }
            else
            {
                rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
            }
            break;
        case eSWDMX_MEDIA_AUDIO:
        case eSWDMX_MEDIA_SKIPPED_AUDIO:
            rDmxMMData.u1Idx = u1AudPid;
            break;
        // To Do
        case eSWDMX_MEDIA_SUBTITLE:
             rDmxMMData.u1Idx =prSwdmxAviInfo->u1SubPid;
             break;
        default:
            LOG(3, "Wrong Pid\n");
            return eSWDMX_HDR_PRS_FAIL;
    }
    if(((prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV7) ||
     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV8)) ||
     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV9))
    {
         rDmxMMData.fgFrameHead = TRUE;
    }
    if((u1HdrType == eSWDMX_MEDIA_VIDEO) &&
        (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_VC1) &&
        !prSwdmxAviInfo->fgVC1Seqheader)
    {
        DMX_MM_DATA_T rDmxMMData_SequenceHeader;
        UINT8 u1BindingByte = 0;

        x_memset(&rDmxMMData_SequenceHeader,0,sizeof(DMX_MM_DATA_T));

        rDmxMMData_SequenceHeader.u4BufStart = (UINT32)(prSwdmxAviInfo->u4VidHeaderPtr)&0xFFFFFFF0;
        rDmxMMData_SequenceHeader.u4BufEnd = ((UINT32)(prSwdmxAviInfo->u4VidHeaderPtr) + 0x10+188)&0xFFFFFFF0;
        rDmxMMData_SequenceHeader.u4StartAddr = prSwdmxAviInfo->u4VidHeaderPtr;//(UINT32)&_rSwdmxWmvInfo.au1SequenceHeader[0];
        rDmxMMData_SequenceHeader.u4FrameSize = (UINT32)(prSwdmxAviInfo->u4VidHdrLength);
        rDmxMMData_SequenceHeader.u4Pts = 0;
        rDmxMMData_SequenceHeader.fgFrameHead  = TRUE;
        x_memcpy(&u1BindingByte,(VOID*)rDmxMMData_SequenceHeader.u4BufStart,1);

         LOG(3, "%s(%d): Setup binding byte to WMV decoder1 : %02x\n", __FUNCTION__, __LINE__, u1BindingByte);
         VDEC_SetParam(prSwdmxInst->u1VdecId, 18, (UINT32)(&u1BindingByte), 0, 0);
        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData_SequenceHeader) != TRUE)
        {
            LOG(0, "Demuxer fails to move VC-1 sequence header.\n");
            ASSERT(0);
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
        prSwdmxAviInfo->fgVC1Seqheader = TRUE;
        
    }
    if((u1HdrType == eSWDMX_MEDIA_VIDEO) &&
        (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_VC1))
    {
        
        DMX_MM_DATA_T rDmxMMData_StartCode;
        UINT32 u4PreStartCode = 0;
        
        x_memset(&rDmxMMData_StartCode,0,sizeof(DMX_MM_DATA_T));
        
        rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
        rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
        u4PreStartCode = _SWDMX_AVIPrsVC1StartCode(rDmxMMData.u4StartAddr,rDmxMMData.u4BufStart,rDmxMMData.u4BufEnd);
        if (!(u4PreStartCode ==WMV_SC_FRAME || u4PreStartCode==WMV_SC_ENTRY || u4PreStartCode==WMV_SC_FIELD|| u4PreStartCode==WMV_SC_SEQ))
        {
            UINT32 u4Tmp = 0x0D010000;
            x_memcpy((VOID*)(VIRTUAL((UINT32)&_pu1VideoFrameData[0])),(VOID*)&u4Tmp,4);

            //x_memcpy((VOID*)(VIRTUAL(&_pu1VideoFrameData[4])),(VOID*)rDmxMMData.u4StartAddr,rDmxMMData.u4FrameSize);
            rDmxMMData_StartCode.u4BufStart = VIRTUAL((UINT32)(_pu1VideoFrameData));
            rDmxMMData_StartCode.u4BufEnd = ((UINT32)( rDmxMMData_StartCode.u4BufStart+1*1024) + 0x10+188)&0xFFFFFFF0;
            rDmxMMData_StartCode.u4StartAddr = VIRTUAL((UINT32)(_pu1VideoFrameData));
            rDmxMMData_StartCode.u4FrameSize = 4;
            rDmxMMData_StartCode.u4Pts = u4Pts;
            rDmxMMData_StartCode.fgFrameHead= TRUE;

            
            if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData_StartCode)  != TRUE)
            {
                LOG(0, "Demuxer fails to move VC1 StartCode data.\n");
                *pu4ConsumeSize = 0;
                return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
            }
        }
        
    }

    if((u1HdrType == eSWDMX_MEDIA_VIDEO) &&
        (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_H264))
    {
        UINT32 u4_i = 0;
        UCHAR *pucFdrBuf = (UCHAR*)rDmxMMData.u4StartAddr;
        for(; u4_i < 3; u4_i++)
        {
            if(*pucFdrBuf != 0 && *pucFdrBuf != 1)
            {
                *pucFdrBuf = 0;
            }
            pucFdrBuf++;
            if(pucFdrBuf == (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr))
            {
                pucFdrBuf = (UCHAR*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
            }
        }
        if(*(pucFdrBuf-1) == 0)
        {
            *pucFdrBuf++ = 1;
        }
    }
   
    if(!((u1HdrType == eSWDMX_MEDIA_VIDEO)&&
        (prSwdmxAviInfo->fgAbRepeat)))
    { 
        prSwdmxAviInfo->fgNoCalPts = FALSE;
	    rDmxMMData.u4BufStart = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr));
	    rDmxMMData.u4BufEnd = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr));
	    rDmxMMData.u4FrameSize = u4ChunkSize;
	    rDmxMMData.u4Pts = u4Pts;
	    rDmxMMData.u4Dts = u4Pts;
	    LOG(11, "Header Type = %ld u4FrameSize = 0x%08x pts = 0x%08x\n", 
	        u1HdrType, u4ChunkSize, u4Pts);  
	    //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
	    if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
	    {
	        LOG(0, "Demuxer fails to move data.\n");
	        *pu4ConsumeSize = 0;
	        return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
	    }
	}

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        prSwdmxAviInfo->fgSendVolInfo = TRUE;
         _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
        if( (prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts != 0)&&
            (prSwdmxAviInfo->u4VidHeaderPtr == 0)&&
            (prSwdmxAviInfo->u4NumSentVid == 0))
        {
             UNUSED(_SWDMX_AviSeek(prSwdmxInst, prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts, prRangeList));
             prSwdmxAviInfo->u8VSeekPts = 
                prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts;
             prSwdmxAviInfo->u8ASeekPts = 
                prSwdmxAviInfo->prDmxRangeInfo->t_vid_start_render_pts;
             prSwdmxAviInfo->fgFeederInvalid = TRUE;
        }
             prSwdmxAviInfo->u4NumSentVid++;
    }
    else if(u1HdrType == eSWDMX_MEDIA_AUDIO || 
                u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO)
    {
        _SWDMX_AviCalculatePts(prSwdmxInst, u1HdrType, u1TrackIdx, u4ChunkSize);
    }
        
    *pu4ConsumeSize = u4ChunkSize + AVI_PARSER_OFFSET + i;
    if (fgRingBuf)
    {
        *pu4ConsumeSize += ((UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr)) - 
                                                           (UINT32)(pucOriginalPtr) - 
                                                           u4RingSize);
    }

    prSwdmxAviInfo->u4ErrorData = 0;
    
    if (prSwdmxAviInfo->fgIsBlock == TRUE)
    {
        prSwdmxAviInfo->fgIsBlock = FALSE;
    }
    
    return eSWDMX_HDR_PRS_SUCCEED;
}

static BOOL _SWDMX_AviGetDataFromFeeder(
    SWDMX_INFO_T* prSwdmxInst, 
    FEEDER_REQ_DATA_T *prFeederData,
     UINT8 u1VdecId)
{
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
        
    if (!_SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)prFeederData))
    {
        LOG(3, "_SWDMX_AviGetDataFromFeeder Ack Fail!!!\n");

        prSwdmxAviInfo->fgFeederInvalid = TRUE;
        //ASSERT(0);
        return FALSE;        
    }

    if (prFeederData->eDataType == FEEDER_SOURCE_INVALID)
    {
        LOG(3, "_SWDMX_AviGetDataFromFeeder Timeout!!!\n");
        if ((prFeederData->u4Id + 1) == prSwdmxAviInfo->u4FeederReqID)
        {
        	  prSwdmxAviInfo->fgFeederInvalid = TRUE;
        }
        else
        {
             if(u1VdecId == prSwdmxInst->u1VdecId)
             {
   	             prSwdmxAviInfo->fgReqDataMiss = TRUE;
             }
             if(prSwdmxAviInfo->fgIs3DFile)
             {
                 if( u1VdecId == prSwdmxInst->u1Vdec2Id)
                 {
                     prSwdmxAviInfo->fgReqSubDataMiss = TRUE;
                 }
             }
        }
        return FALSE;
    }

    if (prFeederData->u4Id != prSwdmxAviInfo->u4FeederReqReceiveID ||
        prFeederData->u4Id+1 != prSwdmxAviInfo->u4FeederReqID)
    {
        LOG(3, "Feeder ID mismatch!!! feeder id = %ld\n", prFeederData->u4Id);
        prSwdmxAviInfo->fgFeederInvalid = TRUE;
        return FALSE;
    }
    if((prFeederData->u4WriteAddr < prSwdmxInst->rFeederInfo.u4StartAddr) || 
    (prFeederData->u4WriteAddr>=prSwdmxInst->rFeederInfo.u4EndAddr))
    {
        LOG(0,"!!!!!!!!prFeederData->u4WriteAddr = %x\n",prFeederData->u4WriteAddr);
        prSwdmxAviInfo->fgFeederInvalid = TRUE;
        return FALSE;
    }
    
    if(u1VdecId == prSwdmxInst->u1VdecId)
    {
         prSwdmxAviInfo->fgReqDataMiss = FALSE;
    }
     
     if(prSwdmxAviInfo->fgIs3DFile)
     {
         if( u1VdecId == prSwdmxInst->u1Vdec2Id)
          {
              prSwdmxAviInfo->fgReqSubDataMiss = FALSE;
          }
     }
    prSwdmxAviInfo->u4FeederReqReceiveID++;
    //prSwdmxAviInfo->fgFeederInvalid = FALSE;
    return TRUE;
}

static BOOL _CheckAviChunkType(SWDMX_INFO_T *prSwdmxInst,
    UCHAR *pucBuf, 
    UINT8 *pu1HdrType, 
    UINT8 *pu1TrackIdx)
{
    INT32 i;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

    if (prSwdmxAviInfo->fgEnVideo)
    {
        if(_CheckAviChunkIndex(pucBuf, prSwdmxAviInfo->u4VStrmID) == TRUE)
        {
            *pu1HdrType = eSWDMX_MEDIA_VIDEO;
            if (prSwdmxAviInfo->fgIs3DFile)
            {            
                prSwdmxAviInfo->fgIsLeftVideo = TRUE;
            }
             return TRUE;
        }
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(_CheckAviChunkIndex(pucBuf, prSwdmxAviInfo->u4VSubStrmID) == TRUE)
            {
                *pu1HdrType = eSWDMX_MEDIA_VIDEO;
                 prSwdmxAviInfo->fgIsLeftVideo = FALSE;
                 return TRUE;
             }
         }
    }
    
    if (prSwdmxAviInfo->fgEnSubTitle)
    {
        if (_CheckAviChunkIndex(pucBuf, 
             prSwdmxAviInfo->u4SpStrmID) == TRUE)
        {
            *pu1HdrType = eSWDMX_MEDIA_SUBTITLE;
            return TRUE;
        }
    }
    
    if ((_CheckAviChunkIndex(pucBuf, AVI_CHUNK_ID_JUNK) == TRUE) /*||
        (_CheckAviChunkIndex(pucBuf, ((AVI_IDX_CHUNK_ID) | (pucBuf[2] << 16) | (pucBuf[3] << 24)))== TRUE)*/)
    {
        *pu1HdrType = eSWDMX_MEDIA_SKIPPED;
        return TRUE;
    }
// three "else if" duplicate
    else if (_CheckAviChunkIndex(pucBuf, ((AVI_UNCOMPRE_DATA) | (pucBuf[0]) | (pucBuf[1] << 8)))== TRUE)
    {
        UCHAR ucStreamID[2];
  
        ucStreamID[1] = (prSwdmxAviInfo->u4VStrmID >> 8) & 0xff;
        ucStreamID[0] = (prSwdmxAviInfo->u4VStrmID >> 0) & 0xff;

        if (pucBuf[0] == ucStreamID[0] && pucBuf[1] == ucStreamID[1])
        {
            *pu1HdrType = eSWDMX_MEDIA_VIDEO;
            return TRUE;
        }

        ucStreamID[1] = (prSwdmxAviInfo->u4AStrmID >> 8) & 0xff;
        ucStreamID[0] = (prSwdmxAviInfo->u4AStrmID >> 0) & 0xff;

        if (pucBuf[0] == ucStreamID[0] && pucBuf[1] == ucStreamID[1])
        {
            *pu1HdrType = eSWDMX_MEDIA_AUDIO;
            return TRUE;
        }
    }
    else if (_CheckAviChunkIndex(pucBuf, ((AVI_VID_COMPRE_DATA) | (pucBuf[0]) | (pucBuf[1] << 8))) == TRUE)
    {
        UCHAR ucStreamID[2];

	ucStreamID[1] = (prSwdmxAviInfo->u4VStrmID >> 8) & 0xff;
	ucStreamID[0] = (prSwdmxAviInfo->u4VStrmID >> 0) & 0xff;
	
	if ((pucBuf[0] == ucStreamID[0] && pucBuf[1] == ucStreamID[1]) &&
	    (prSwdmxAviInfo->fgEnVideo))
	{
           *pu1HdrType = eSWDMX_MEDIA_VIDEO;
	   return TRUE;
	}
    }
    else if (_CheckAviChunkIndex(pucBuf, ((AVI_DRM_VID_CHUNK_ID) | (pucBuf[0]) | (pucBuf[1] << 8)))== TRUE)
    {
        UCHAR ucStreamID[2];
  
        ucStreamID[1] = (prSwdmxAviInfo->u4VStrmID >> 8) & 0xff;
        ucStreamID[0] = (prSwdmxAviInfo->u4VStrmID >> 0) & 0xff;

        if (pucBuf[0] == ucStreamID[0] && pucBuf[1] == ucStreamID[1])
        {
            *pu1HdrType = eSWDMX_MEDIA_DRM_VIDEO;
            return TRUE;
        }
        
        ucStreamID[1] = (prSwdmxAviInfo->u4AStrmID >> 8) & 0xff;
        ucStreamID[0] = (prSwdmxAviInfo->u4AStrmID >> 0) & 0xff;

        if (pucBuf[0] == ucStreamID[0] && pucBuf[1] == ucStreamID[1])
        {
            *pu1HdrType = eSWDMX_MEDIA_DRM_AUDIO;
            return TRUE;
        }
    }

    for (i = 0; (i < prSwdmxAviInfo->u1TotalATrack) && (i < AVI_DIVX_MAX_AUD_TRACK); i++)
    {
        if ((prSwdmxAviInfo->fgKeyExist)&&(prSwdmxAviInfo->parAudKeyIdxTbl[i]))
        {
            if (_CheckAviChunkIndex(pucBuf, 
                prSwdmxAviInfo->parAudKeyIdxTbl[i]->t_strm_id.u.t_stm_avi_id) == TRUE)
            {
                if (prSwdmxAviInfo->fgEnAudio && prSwdmxAviInfo->u1CurATrackIdx == i)
                {
                    *pu1HdrType = eSWDMX_MEDIA_AUDIO;
                }
                else 
                {
                    *pu1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                }

                if (pu1TrackIdx)
                {
                    *pu1TrackIdx = i;
                }

                return TRUE;
           }
        }
        else
        {   
            if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
            {
                if(prSwdmxAviInfo->prAudIdxTbl[i])
                {
                    if (_CheckAviChunkIndex(pucBuf, 
                        prSwdmxAviInfo->prAudIdxTbl[i]->t_strm_id.u.t_stm_avi_id) == TRUE)
                    {
                        if(prSwdmxAviInfo->fgEnAudio &&(prSwdmxAviInfo->u1CurATrackIdx == i))
                        {
                            *pu1HdrType = eSWDMX_MEDIA_AUDIO;
                        }
                        else
                        {
                            *pu1HdrType = eSWDMX_MEDIA_SKIPPED_AUDIO;
                        }
                        
                        if (pu1TrackIdx)
                        {
                            *pu1TrackIdx = i;
                        }
                        return TRUE;
                    }
                }
            }
        }
    }

    /* skip all other video & Audio ... */ 
    if (_CheckAviOtherChunk(pucBuf) == TRUE)
    {
        *pu1HdrType = eSWDMX_MEDIA_SKIPPED;
        return TRUE;
    }
    /*==============================*/

    return FALSE;
}

static BOOL _CheckAviOtherChunk(UCHAR *pucBuf)
{
    INT32 i4StreamNum = 0;
    
    // Skip Chunks here
    if (((pucBuf[2] == 'd') && (pucBuf[3] == 'c')) || 
        ((pucBuf[2] == 'w') && (pucBuf[3] == 'b')) ||
        ((pucBuf[2] == 's') && (pucBuf[3] == 'b'))
        )
    {
        if ((pucBuf[0] >= '0' && pucBuf[0] <= '1') && 
            (pucBuf[1] >= '0' && pucBuf[1] <= '9'))
        {
            i4StreamNum = (INT32)((pucBuf[0] - '0') * 10 + (pucBuf[1] - '0'));
            // Max 17 streams
            if (i4StreamNum < 17)
            {
                return TRUE;
            }
        }
    }
    else if ((pucBuf[0] == 'i') && (pucBuf[1] == 'x'))
    {
        if ((pucBuf[2] >= '0' && pucBuf[2] <= '1') && 
            (pucBuf[3] >= '0' && pucBuf[3] <= '9'))
        {
            i4StreamNum = (INT32)((pucBuf[2] - '0') * 10 + (pucBuf[3] - '0'));
            // Max 17 streams
            if (i4StreamNum < 17)
            {
                return TRUE;
            }
        }
    }
    else if ((pucBuf[0] == 'i') && (pucBuf[1] == 'd') &&
                (pucBuf[2] == 'x') && (pucBuf[3] == '1'))
    {
        return TRUE;
    }

    return FALSE;
}

static BOOL _CheckAviChunkIndex(UCHAR *pucBuf, UINT32 u4StrmID)
{
    UINT32 u4CmpID = 0;

    u4CmpID = ((UINT32)(pucBuf[0])) |
                       ((UINT32)(pucBuf[1]) << 8) |
                       ((UINT32)(pucBuf[2]) << 16) |
                       ((UINT32)(pucBuf[3]) << 24);
    if (u4CmpID != u4StrmID)
    {
        return FALSE;
    }
    
    return TRUE;
}


static void _SWDMX_AviPrsDrmChunk(SWDMX_INFO_T* prSwdmxInst, UINT8 u1HdrType, UINT32 u4StartAddr)
{
    UCHAR ucDrmBuf[AVI_DRM_SIZE];
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

    if (u4StartAddr <= (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) - AVI_DRM_SIZE))
    {
        x_memcpy(ucDrmBuf, (void*)u4StartAddr, AVI_DRM_SIZE);
    }
    else
    {
        UINT32 u4Temp = (UINT32)(VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr) -u4StartAddr);
        
        x_memcpy(ucDrmBuf, (void*)u4StartAddr, u4Temp);
        x_memcpy(&ucDrmBuf[u4Temp], 
                           (void*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr), 
                           (AVI_DRM_SIZE - u4Temp)); 
    }

    prSwdmxAviInfo->u1DrmType = u1HdrType;
    prSwdmxAviInfo->u2FrameKeyIdx = (UINT16)((ucDrmBuf[0]) |
                                                                         (ucDrmBuf[1] << 8));

    prSwdmxAviInfo->u4EncryptOffset = (ucDrmBuf[2] |
    								(ucDrmBuf[3] << 8) | 
    								(ucDrmBuf[4] << 16) | 
    								(ucDrmBuf[5] << 24));


    prSwdmxAviInfo->u4EncryptLength = (ucDrmBuf[6] | 
    								 (ucDrmBuf[7] << 8) |
    								 (ucDrmBuf[8] << 16) | 
    								 (ucDrmBuf[9] << 24));

    if (prSwdmxAviInfo->u2FrameKeyIdx >= AVI_DRM_MAX_NUM_KEY)
    {
        LOG(3, "DRM Error Key Index is too large\n");
        prSwdmxAviInfo->u1DrmType = eSWDMX_MEDIA_NONE;
    }

    LOG(9, "Drm Type = %ld\n", u1HdrType);
    LOG(9, "Drm Frame Key Index = %ld\n", prSwdmxAviInfo->u2FrameKeyIdx);
    LOG(9, "Drm Frame Encrypt Offset = %ld\n", prSwdmxAviInfo->u4EncryptOffset);
    LOG(9, "Drm Frame Encrypt Length = %ld\n", prSwdmxAviInfo->u4EncryptLength);
}


static BOOL _SWDMX_InitDmx(SWDMX_INFO_T *prSwdmxInst, UINT8 u1HdrType, UINT8 u1Index,VOID* pfnCbFunc, BOOL fgEos)
{
    BOOL fgRet = TRUE;
    UINT32 u4Flag = 0; 
    DMX_MM_T rDmxMMInfo;
    UINT8 u1VidPid=0;
    BOOL fgVDmxEnabled=FALSE;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    x_memset(&rDmxMMInfo, 0, sizeof(DMX_MM_T));

    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {
        UINT8 u1DmxPid = 0xFF;
        UINT32 u4VidFifoAddr, u4VidFifoSize;

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;
        if(prSwdmxAviInfo->fgIs3DFile)
        {
           fgVDmxEnabled = prSwdmxAviInfo->fgEnV2Dmx;
        }
        else
        {
           fgVDmxEnabled = prSwdmxAviInfo->fgEnVDmx;
        }
        if (!fgVDmxEnabled)
        {

            DMX_SetToDecoder(TRUE);

            u1VidPid = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

            if( u1VidPid != 0xFF)
            {
                u1DmxPid = u1VidPid;
            }
            else
            {
                LOG( 3,"get  video pid fail\n");
            }


            if(prSwdmxAviInfo->fgIs3DFile)
            {
                if(IS_NEWTRACK(prSwdmxInst,u1Index))
                {
                    _SWDMX_GetVideo2Fifo(prSwdmxInst->u1SwdmxId, &u4VidFifoAddr, &u4VidFifoSize);
                   prSwdmxAviInfo->u1Vid2Pid = u1VidPid;
                   rDmxMMInfo.u1ChannelId = prSwdmxInst->u1Vdec2Id;
                }
                else
                {
                    _SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &u4VidFifoAddr, &u4VidFifoSize);
                   prSwdmxAviInfo->u1VidPid = u1VidPid;
                   rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
                }
            }
            else
            {
                _SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &u4VidFifoAddr, &u4VidFifoSize);
                prSwdmxAviInfo->u1VidPid = u1VidPid;
                rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
            }

            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID |
                DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG |
                DMX_MM_FLAG_DEVICE_ID);

            rDmxMMInfo.fgAllocBuf = FALSE;
            //rDmxMMInfo.u4BufAddr = prFbmPool->u4Addr;
            //rDmxMMInfo.u4BufSize = prFbmPool->u4Size;
            rDmxMMInfo.u4BufAddr          = u4VidFifoAddr ;
            rDmxMMInfo.u4BufSize          = u4VidFifoSize;
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
            if(prSwdmxAviInfo->u4VDecType ==
                (UINT32)ENUM_SWDMX_VDEC_MJPEG)
            {
                rDmxMMInfo.u1DeviceId = VLD1; // video format all use VLD expect jpeg
            }
            else
            {        
                rDmxMMInfo.u1DeviceId = VLD0; // video format all use VLD expect jpeg
            }
            u4Flag |= DMX_MM_FLAG_SEARCH_START_CODE;

            switch (prSwdmxAviInfo->u4VDecType)
            {
                case ENUM_SWDMX_VDEC_DX311: case ENUM_SWDMX_VDEC_WMV7:
                case ENUM_SWDMX_VDEC_WMV8: case ENUM_SWDMX_VDEC_WMV9:
                case ENUM_SWDMX_VDEC_MJPEG: case ENUM_SWDMX_VDEC_H263:
                    LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
                    rDmxMMInfo.fgSearchStartCode = FALSE;
                    break;
                default:
                    LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                    rDmxMMInfo.fgSearchStartCode = TRUE;      
                    break;
            }
            fgRet = DMX_MM_Set(u1DmxPid, u4Flag, &rDmxMMInfo);
        }
        else if (fgEos)
        {
             if(prSwdmxAviInfo->fgIs3DFile)
            {
                u1VidPid = prSwdmxAviInfo->u1Vid2Pid;
            }
            else
            {
                u1VidPid = prSwdmxAviInfo->u1VidPid;
            }
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
            u1DmxPid = u1VidPid;
            fgRet = DMX_MM_Set(u1DmxPid, u4Flag, &rDmxMMInfo);
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO)
    {
        UINT8 u1DmxPid = 0xFF;
        INT32 i;
        UINT32 u4AudFifoAddr, u4AudFifoSize;
        UINT32 u4AudPreFifoAddr = 0;
        UINT32 u4AudPreFifoSize = 0;
        UINT32 u4AudTrickSize = 0;

        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_AUDIO;

        if (!prSwdmxAviInfo->fgEnADmx)
        {
            DMX_SetToDecoder(TRUE);
            prSwdmxAviInfo->u1AudPid = 
            DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);

            _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);

            if( prSwdmxAviInfo->u1AudPid != 0xFF)
            {
                u1DmxPid = prSwdmxAviInfo->u1AudPid;
            }
            else
            {
                LOG(3, "get  audio pid fail\n");
            }

            u4Flag = (DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID |
                DMX_MM_FLAG_BUF | DMX_MM_FLAG_INSTANCE_TAG |
                DMX_MM_FLAG_DEVICE_ID);
        
            rDmxMMInfo.fgAllocBuf = FALSE;
            
            rDmxMMInfo.u4BufAddr = u4AudFifoAddr;
            rDmxMMInfo.u4BufSize = u4AudFifoSize;
            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
            rDmxMMInfo.u1ChannelId = prSwdmxInst->u1AdecId;
            rDmxMMInfo.u1DeviceId = prSwdmxInst->u1AdecId;

            // Record the audio fifo size
            prSwdmxAviInfo->u4AFifoSize = rDmxMMInfo.u4BufSize;

            
            DMX_AudHandler_SetEnable(TRUE);
            
            fgRet = DMX_MM_Set(u1DmxPid, u4Flag, &rDmxMMInfo);
            if(!DMX_AudHandler_Set(u1DmxPid, TRUE))
            {
                LOG(1, "%s:%d, DMX_AudHandler_Set fail\n", __FUNCTION__, __LINE__);
            }
            //accodring num of aud track to alloc pre buffer
            if(_SWDMX_GetAudioPreFifo(prSwdmxInst->u1SwdmxId, &u4AudPreFifoAddr, &u4AudPreFifoSize))
            {
                //start address 16 bytes align
                if((u4AudPreFifoAddr%16))
                {
                	u4AudPreFifoAddr = 16 - (u4AudPreFifoAddr%16) + u4AudPreFifoAddr;
    			u4AudPreFifoSize = u4AudPreFifoSize - (16 - (u4AudPreFifoAddr%16));
                }
                //Dynamic allocate audio pre-buffer
                if(prSwdmxAviInfo->u1TotalATrack)
                {
                    u4AudTrickSize = u4AudPreFifoSize/prSwdmxAviInfo->u1TotalATrack;
                }
                else
                {
                    u4AudTrickSize = u4AudPreFifoSize/4;
                }
                //audio fifo size 16 bytes align
                u4AudTrickSize = u4AudTrickSize - u4AudTrickSize%16;
                if(u4AudTrickSize > u4AudFifoSize)
                {
                    u4AudTrickSize = u4AudFifoSize;
                }
            }
            else
            {
                LOG(1, "%s:%d, _SWDMX_GetAudioPreFifo fail\n", __FUNCTION__, __LINE__);
            }
            for (i = 0; i < prSwdmxAviInfo->u1TotalATrack; i++)
            {
                rDmxMMInfo.fgAllocBuf = FALSE;            
                rDmxMMInfo.u4BufSize = u4AudTrickSize; 
                rDmxMMInfo.u4BufAddr = u4AudPreFifoAddr;
                prSwdmxAviInfo->u1AudPreBufPid[i] = 
                DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
                if( prSwdmxAviInfo->u1AudPreBufPid[i]  == 0xFF)
                {
                    LOG(3, "get  audio pre  pid fail\n");
                }
                
                fgRet = DMX_MM_Set( prSwdmxAviInfo->u1AudPreBufPid[i], u4Flag, &rDmxMMInfo);
                if(!DMX_AudHandler_Set( prSwdmxAviInfo->u1AudPreBufPid[i], FALSE))
                {
                    LOG(1, "%s:%d, DMX_AudHandler_Set fail\n", __FUNCTION__, __LINE__);
                }
                
                u4AudPreFifoAddr = u4AudPreFifoAddr + u4AudTrickSize;
            }
            LOG(5, "Set Audio Active Pid Idx %d\r\n", prSwdmxAviInfo->u1CurATrackIdx);
            fgRet = DMX_AudHandler_SetActivePidx( prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx]);
            prSwdmxAviInfo->fgSetActivePid = TRUE;
        }
        else if (fgEos)
        {
            INT32 i;

            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            rDmxMMInfo.fgSearchStartCode = FALSE;
            for (i = 0; i < prSwdmxAviInfo->u1TotalATrack; i++)
            { 
                if(prSwdmxAviInfo->u1AudPreBufPid[i]!= 0xFF)
                {
                    fgRet = DMX_MM_Set(prSwdmxAviInfo->u1AudPreBufPid[i], u4Flag, &rDmxMMInfo);
                }
            }
        }

    }
    else if (u1HdrType == eSWDMX_MEDIA_SUBTITLE)
    {
        rDmxMMInfo.fgEnable = TRUE;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_PES;
        
        prSwdmxAviInfo->u1SubPid = 
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
            rDmxMMInfo.u4BufSize = AVI_SUBTITLE_BUFSIZE;

            fgRet = DMX_MM_Set( prSwdmxAviInfo->u1SubPid , u4Flag, &rDmxMMInfo);
        }
    }

    return fgRet;
}


static BOOL _SWDMX_SendAviEos(SWDMX_INFO_T* prSwdmxInst, UINT8 u1HdrType, UINT8 u1Index)
{
    UINT16 u2MaxQNum, u2CurQNum;
    //UINT32 u4DummyData[4];
    DMX_MM_DATA_T rDmxMMData;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);

    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != 
      FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return FALSE;
    }
    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

    rDmxMMData.u4BufStart = prSwdmxInst->rFeederInfo.u4StartAddr;
    rDmxMMData.u4BufEnd = prSwdmxInst->rFeederInfo.u4EndAddr;
    rDmxMMData.u4StartAddr = prSwdmxInst->rFeederInfo.u4StartAddr;
    rDmxMMData.u4FrameSize = 4;
    rDmxMMData.fgEOS = TRUE;
    rDmxMMData.fgIgnoreData = TRUE;

    if(((prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV7) ||
     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV8)) ||
     (prSwdmxAviInfo->u4VDecType == ENUM_SWDMX_VDEC_WMV9))
    {
         rDmxMMData.fgFrameHead = TRUE;
    }
    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {            
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(IS_NEWTRACK(prSwdmxInst,u1Index))
            {
                rDmxMMData.u1Idx = prSwdmxAviInfo->u1Vid2Pid;
                VDEC_GetQueueInfo(prSwdmxInst->u1Vdec2Id, &u2CurQNum, &u2MaxQNum);
                if (u2CurQNum > (u2MaxQNum - 10))
                {
                    x_thread_delay(20);
                    return FALSE;
                }
                if (_SWDMX_InitDmx(prSwdmxInst, u1HdrType, prSwdmxInst->u1Vdec2Id, NULL, TRUE) == FALSE)
                {
                    return FALSE;
                }
                return  _SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData);    
            }
            else
            {
                rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
                VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
                if (u2CurQNum > (u2MaxQNum - 10))
                {
                    x_thread_delay(20);
                    return FALSE;
                }
                if (_SWDMX_InitDmx(prSwdmxInst, u1HdrType, prSwdmxInst->u1VdecId, NULL, TRUE) == FALSE)
                {
                    return FALSE;
                }            
                return  _SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData);  
            }
        }
        else
        {
            rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
            VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
            LOG(7, "Line %d, u2CurQNum = %ld, u2MaxQNum = %ld .\n", __LINE__, u2CurQNum, u2MaxQNum);
            if (u2CurQNum > (u2MaxQNum - 10))
            {
                x_thread_delay(30);
                return FALSE;
            }
            if (_SWDMX_InitDmx(prSwdmxInst, u1HdrType, prSwdmxInst->u1VdecId, NULL, TRUE) == FALSE)
            {
                return FALSE;
            }
            return _SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData); 
        }           
    }
    else
    {
        INT32 i;

        if (prSwdmxAviInfo->fgEnAudio)
        {
            if((prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)&&
                (prSwdmxAviInfo->fgSetActivePid == FALSE))
            {
                 DMX_AudHandler_SetActivePidx(prSwdmxAviInfo->u1AudPreBufPid[prSwdmxAviInfo->u1CurATrackIdx]);
            }
        }
        for (i = 0; i < prSwdmxAviInfo->u1TotalATrack; i++)
        {
            rDmxMMData.u1Idx = prSwdmxAviInfo->u1AudPreBufPid[i];
            if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData))
            {
                LOG(3, "Fail to send audio eos\n");
                return FALSE;
            }
        }
        return TRUE;
    }
}

static BOOL _SWDMX_AviABRepeat(SWDMX_INFO_T * prSwdmxInst,
    UINT64 * u8SeekPos,
    SWDMX_CUR_CMD_T * prCmd)
{
     UINT8 u1Idx = 0;
     SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
     INT32 i4PlaySpeed = prCmd->i4PlaySpeed;
     prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
     SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
       
     LOG(3, "Do AVI  A<->B  repeat\n");

     prSwdmxAviInfo->fgAbRepeat = TRUE;
            
     if ( prSwdmxAviInfo->prVidKeyIdxTbl &&
          prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry != 0)
     {
          *u8SeekPos = prCmd->rSeekOffset.ui8_stc;
     }
     else
     {
            	
        prSwdmxAviInfo->fgFeederInvalid = TRUE;

        if(prSwdmxAviInfo->fgEnAudio)
        {
            prSwdmxAviInfo->u4TriggerAudPts = prCmd->rSeekOffset.ui8_aud_pts_info;
            DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);
            AUD_SetStartPts(prSwdmxInst->u1AdecId, prCmd->rSeekOffset.ui8_aud_pts_info);
        }
        for ( ;u1Idx < (prSwdmxAviInfo->u1TotalATrack)&&(u1Idx < AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
        {
             prSwdmxAviInfo->u4AudPts[u1Idx] = 0;
            DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPreBufPid[u1Idx]);
        }

        prSwdmxAviInfo->u8CurDmxMoviPos = prCmd->rSeekOffset.ui8_i_frame_position;
        prSwdmxAviInfo->u4CurVidPts = prCmd->rSeekOffset.ui8_i_pts_info;
        prSwdmxAviInfo->u8AbRepeatVidPts = prCmd->rSeekOffset.ui8_pts_info;         
        prSwdmxAviInfo->u8AbRepeatAudPts = prCmd->rSeekOffset.ui8_aud_pts_info;
             
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, prCmd->rSeekOffset.ui8_pts_info);
        }
        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 
                 prCmd->rSeekOffset.ui8_pts_info);
        if(i4PlaySpeed == 1000)
        {
          prSwdmxAviInfo->fgRecSpeedChangeForAud = TRUE;
          prSwdmxAviInfo->i4RecPrevSpeedForAud = prSwdmxAviInfo->i4PrevSpeed;
        }
    }

     return TRUE;
     
}

static BOOL _SWDMX_AviSeekCurPos(SWDMX_INFO_T* prSwdmxInst)
{
    VDP_POS_INTO_T rVdpPosInfo;
	SWDMX_AVI_INFO_T *prSwdmxAviInfo=&prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
	if(!prSwdmxAviInfo->fgEnVDmx)
	{
       return FALSE;
	}
	
	if(VDP_GetPosInfo(prSwdmxInst->u1B2rId,&rVdpPosInfo))
	{
        prSwdmxAviInfo->u8CurDmxMoviPos=rVdpPosInfo.u8OffsetI-8;
        LOG(5, "Seek to I frame pos %llu\r\n", prSwdmxAviInfo->u8CurDmxMoviPos);
		prSwdmxAviInfo->u4CurVidPts=rVdpPosInfo.u8Pts;
		if(prSwdmxAviInfo->u1CurATrackIdx<prSwdmxAviInfo->u1TotalATrack)
		{
			prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]=(UINT32)rVdpPosInfo.u8AudPts;
		}
		prSwdmxAviInfo->fgIsSeek = TRUE;
		prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
		prSwdmxAviInfo->u8CurMoviPos=prSwdmxAviInfo->u8CurDmxMoviPos;
        prSwdmxAviInfo->u4CurDmxFeederRPtr = prSwdmxInst->rFeederInfo.u4WriteAddr;
        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));
		return TRUE;
	}
	return FALSE;
}

static BOOL _SWDMX_AviSeek(SWDMX_INFO_T* prSwdmxInst, 
    UINT64 u8SeekTime, 
    SWDMX_RANGE_LIST_T* prRangeList)
{
    BOOL fgFindKey = FALSE;
    UINT8 u1ReadDrmSize = 0;
    INT32 i;
    MM_RANGE_ELMT_IDXTBL_T *prVidKeyIdxTbl = NULL;
    MM_RANGE_ELMT_IDXTBL_T *prAudKeyIdxTbl = 0;
    MM_RANGE_ELMT_IDXTBL_T *prVidTmpKeyIdxTbl = 0;
    MM_RANGE_ELMT_IDXTBL_T *prAudBaseKeyIdxTbl = 0;
    MM_RANGE_ELMT_IDXTBL_T *prAudTmpKeyIdxTbl = 0;
    MM_RANGE_ELMT_IDXTBL_T *prVidIdxTbl = 0;
    MM_RANGE_ELMT_IDXTBL_T *prAudIdxTbl = 0;
    UINT32* pu4AudPts;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    INT32 j;
    UINT64 u8TmpCurDmxPos = 0;
    MM_RANGE_ELMT_IDXTBL_T *prSubVidKeyIdxTbl = NULL;

    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);

    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    prVidKeyIdxTbl = prSwdmxAviInfo->prVidKeyIdxTbl;

    LOG(7, "_SWDMX_AviSeek u8SeekTime = 0x%llx\n", u8SeekTime);
    
    prSubVidKeyIdxTbl = prSwdmxAviInfo->prSubVidKeyIdxTbl;
    if(prSwdmxAviInfo->fgIs3DFile)
   {
        if(prSwdmxAviInfo->fgIsSupport3DTrick)
        {
            UINT8 u1ReadDrmSize = 0;

    LOG(7, "_SWDMX_AviSeek u8SeekTime = 0x%llx\n", u8SeekTime);

            if (prSubVidKeyIdxTbl && prSubVidKeyIdxTbl->ui4_number_of_entry != 0)
            {
                if (!prSwdmxAviInfo->fgKeyExist)
                {
                    LOG(5, "_SWDMX_Avi3DSeek doesn't have the key table\n");
                    return FALSE;
                }
                prSwdmxAviInfo->prCurSubVidKeyIdxTbl = prSubVidKeyIdxTbl;
             	for (j = 0; j < prSubVidKeyIdxTbl->ui4_number_of_entry; j++)
             	{
             	    if (prSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[j].ui4_pts > u8SeekTime)
             	    {
             	        break;
             	    }
             	}
                if (prSubVidKeyIdxTbl->pt_next_partial)
                {
                    INT32 i4NumRiffs = 2;
                    fgFindKey = FALSE;
                    LOG(5, "There are multiple riffs\n");
                    if (prSwdmxAviInfo->prSubVidKeyIdxTbl->pt_next_partial->u.pt_pb_idx_tbl_entry[0].ui4_pts < u8SeekTime)
                    {
                        prSubVidKeyIdxTbl = prSubVidKeyIdxTbl->pt_next_partial;
                        while (prSubVidKeyIdxTbl)
                        {
                            prVidTmpKeyIdxTbl = prSubVidKeyIdxTbl;
                            for (j = 0; j < prSubVidKeyIdxTbl->ui4_number_of_entry; j++)
                            {
                                if (prSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[j].ui4_pts > u8SeekTime)
                                {
                                    LOG(5, " Find key frame in the %ldth riff\n", i4NumRiffs);
                                    fgFindKey = TRUE;
                                    break;
                                }
                            }
                            if (fgFindKey)
                            {
                                break;
                            }
                            prSubVidKeyIdxTbl = prSubVidKeyIdxTbl->pt_next_partial;
                            i4NumRiffs++;
                        }
                        prSubVidKeyIdxTbl = prVidTmpKeyIdxTbl;
                        prSwdmxAviInfo->prCurSubVidKeyIdxTbl = prSubVidKeyIdxTbl;
                    }
                }

                if (prRangeList->prDmxRangeInfo->b_drm)
                {
                    // Currently for Home Theater Profile only
                    u1ReadDrmSize = AVI_DRM_CHUNK_SIZE_HT;
                }
                 if (j > 0)
                {
                    prSwdmxAviInfo->u4CurSentSubKey = j - 1;
                }
                else
                {
                    prSwdmxAviInfo->u4CurSentSubKey = 0;
                }
                prSwdmxAviInfo->u4CurRepeatSubFrameIdx = prSwdmxAviInfo->u4CurSentSubKey;
                if (prSubVidKeyIdxTbl != 0)
                {
                     u8TmpCurDmxPos = prSubVidKeyIdxTbl->ui8_base_offset + 
                        prSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentSubKey].ui4_relative_offset -
                        (AVI_PARSER_OFFSET + u1ReadDrmSize);
                    if(u8TmpCurDmxPos < prSwdmxInst->u8FileOffset)
                    {
                        u8TmpCurDmxPos = prSwdmxInst->u8FileOffset;
                    }
                    prSwdmxAviInfo->u4CurSubVidPts = 
                        prSubVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentSubKey].ui4_pts;
                    LOG(5, "Avi Seek Subkey frame number = 0x%08x Video offset = %llu\n", 
                        prSwdmxAviInfo->u4CurSentSubKey,
                        prSwdmxAviInfo->u8CurDmxMoviPos);
                    LOG(5, "Avi Seek SubVideo pts = 0x%08x\n", prSwdmxAviInfo->u4CurSubVidPts);
                }      
            }
        }
    }
    if ( prSwdmxAviInfo->prVidKeyIdxTbl &&
        prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry != 0)
    {
        if (!prSwdmxAviInfo->fgKeyExist)
        {
            LOG(5, "_SWDMX_AviSeek doesn't have the key table\n");
            return FALSE;
        }

        prSwdmxAviInfo->prCurVidKeyIdxTbl = prVidKeyIdxTbl;
     	for (i = 0; i < prVidKeyIdxTbl->ui4_number_of_entry; i++)
     	{
     	    if (prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts > u8SeekTime)
     	    {
     	        break;
     	    }
     	}

    // There r extra riffs
        if (prVidKeyIdxTbl->pt_next_partial)
        {
            INT32 i4NumRiffs = 2;
        
            LOG(5, "There are multiple riffs\n");
            if (prSwdmxAviInfo->prVidKeyIdxTbl->pt_next_partial->u.pt_pb_idx_tbl_entry[0].ui4_pts < u8SeekTime)
            {
                prVidKeyIdxTbl = prVidKeyIdxTbl->pt_next_partial;
                while (prVidKeyIdxTbl)
                {
                    prVidTmpKeyIdxTbl = prVidKeyIdxTbl;
                    for (i = 0; i < prVidKeyIdxTbl->ui4_number_of_entry; i++)
                    {
                        if (prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts > u8SeekTime)
                        {
                            LOG(5, " Find key frame in the %ldth riff\n", i4NumRiffs);
                            fgFindKey = TRUE;
                            break;
                        }
                    }
                    if (fgFindKey)
                    {
                        break;
                    }
                    prVidKeyIdxTbl = prVidKeyIdxTbl->pt_next_partial;
                    i4NumRiffs++;
                }
                prVidKeyIdxTbl = prVidTmpKeyIdxTbl;
                prSwdmxAviInfo->prCurVidKeyIdxTbl = prVidKeyIdxTbl;
            }
        }

        if (prRangeList->prDmxRangeInfo->b_drm)
        {
            // Currently for Home Theater Profile only
            u1ReadDrmSize = AVI_DRM_CHUNK_SIZE_HT;
        }


        // Error Handling 
        // i = 0 is impossible
         if ((i > 0) /* && 
            (i < (prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry - 1))*/)
        {
            prSwdmxAviInfo->u4CurSentKey = i - 1;
        }
        /*else if (i == (prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry - 1))
        {
            prSwdmxAviInfo->u4CurSentKey = i;
        }*/
        else
        {
            prSwdmxAviInfo->u4CurSentKey= 0;
        }

         prSwdmxAviInfo->u4CurRepeatFrameIdx = prSwdmxAviInfo->u4CurSentKey;

        if (prVidKeyIdxTbl != 0)
        {
            prSwdmxAviInfo->u8CurDmxMoviPos = 
                prVidKeyIdxTbl->ui8_base_offset + 
                prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_relative_offset -
                (AVI_PARSER_OFFSET + u1ReadDrmSize);

            if( prSwdmxAviInfo->u8CurDmxMoviPos < prSwdmxInst->u8FileOffset)
            {
                prSwdmxAviInfo->u8CurDmxMoviPos = prSwdmxInst->u8FileOffset;
            }
            prSwdmxAviInfo->u4CurVidPts = 
                prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_pts;

            LOG(5, "Avi Seek key frame number = 0x%08x Video offset = %llu\n", 
                prSwdmxAviInfo->u4CurSentKey,
                prSwdmxAviInfo->u8CurDmxMoviPos);

            LOG(5, "Avi Seek Video pts = 0x%08x\n", 
                prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_pts);
        }
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(prSwdmxAviInfo->fgIsSupport3DTrick)
            {
                prSwdmxAviInfo->u8CurDmxMoviPos = min(u8TmpCurDmxPos, prSwdmxAviInfo->u8CurDmxMoviPos);
                if(prSwdmxAviInfo->u4CurSentKey != prSwdmxAviInfo->u4CurSentSubKey)
                {
                    prSwdmxAviInfo->u4CurSentSubKey = prSwdmxAviInfo->u4CurSentKey;
                    prSwdmxAviInfo->u4CurRepeatSubFrameIdx = prSwdmxAviInfo->u4CurSentKey;
                }
            }
        }
        for (i = 0; (i < prSwdmxAviInfo->u1TotalATrack) && (i < AVI_DIVX_MAX_AUD_TRACK); i++)
        {
            if (prVidKeyIdxTbl == prSwdmxAviInfo->prVidKeyIdxTbl)
            {
                prAudKeyIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[i];
                pu4AudPts = (UINT32*)prAudKeyIdxTbl->u.pv_idx_tbl_entry;

                LOG(5, "prAudKeyIdxTbl = 0x%08x\n", prAudKeyIdxTbl);
            }
            else
            {
                prVidIdxTbl = prSwdmxAviInfo->prVidKeyIdxTbl->pt_next_partial;
                prAudIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[i]->pt_next_partial;

                while (prVidIdxTbl != prVidKeyIdxTbl)
                {
                    prVidIdxTbl = prVidIdxTbl->pt_next_partial;
                    prAudIdxTbl = prAudIdxTbl->pt_next_partial;
                }
                
                prAudKeyIdxTbl = prAudIdxTbl;
                pu4AudPts = (UINT32*)prAudIdxTbl->u.pv_idx_tbl_entry;
                LOG(5, "prAudKeyIdxTbl = 0x%08x\n", prAudKeyIdxTbl);
            }

            if(pu4AudPts)
            {
                prSwdmxAviInfo->u4AudPts[i] = 
                    pu4AudPts[prSwdmxAviInfo->u4CurSentKey];
                LOG(5, "Avi Seek Audio pts = 0x%08x\n", 
                    pu4AudPts[prSwdmxAviInfo->u4CurSentKey]);
            }
            else
            {
                LOG(0,"audio pts tabel is NULL\n");
            }
            
        }
    }
    else if ((prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack) &&
        prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx] &&
        prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->ui4_number_of_entry != 0)
    {
        prAudKeyIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx];
        pu4AudPts = (UINT32*)prAudKeyIdxTbl->u.pv_idx_tbl_entry;
        for (i = 0; i < prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->ui4_number_of_entry; i++)
        {
            if (pu4AudPts[i]> u8SeekTime)
            {
                break;
            }
        }

        prAudBaseKeyIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx];
        prAudTmpKeyIdxTbl = prAudKeyIdxTbl;

         // There r extra riffs
        if (prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->pt_next_partial)
        {
            INT32 i4NumRiffs = 2;
            
            LOG(5, "There are multiple riffs\n");
            if (prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->pt_next_partial->u.pt_pb_idx_tbl_entry[0].ui4_pts < u8SeekTime)
            {
                prAudKeyIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]->pt_next_partial;
                while (prAudKeyIdxTbl)
                {
                    prAudTmpKeyIdxTbl = prAudKeyIdxTbl;
                    pu4AudPts = (UINT32*)prAudKeyIdxTbl->u.pv_idx_tbl_entry;
                    for (i = 0; i < prAudKeyIdxTbl->ui4_number_of_entry; i++)
                    {
                        if (pu4AudPts[i]> u8SeekTime)
                        {
                           
                             fgFindKey = TRUE;
                             break;
                         }
                    }
                    if (fgFindKey)
                    {
                        break;
                    }
                    prAudKeyIdxTbl = prAudKeyIdxTbl->pt_next_partial;
                    i4NumRiffs++;
                }
                prAudBaseKeyIdxTbl = prAudTmpKeyIdxTbl;
            }
        }

        // Error Handling 
        // i = 0 is impossible
         if ((i > 0) /* && 
            (i < (prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry - 1))*/)
        {
            prSwdmxAviInfo->u4CurSentKey = i - 1;
        }
        /*else if (i == (prSwdmxAviInfo->prVidKeyIdxTbl->ui4_number_of_entry - 1))
        {
            prSwdmxAviInfo->u4CurSentKey = i;
        }*/
        else
        {
            prSwdmxAviInfo->u4CurSentKey= 0;
        }

        for (i = 0; (i < prSwdmxAviInfo->u1TotalATrack) && (i < AVI_DIVX_MAX_AUD_TRACK); i++)
        {
            if (prAudBaseKeyIdxTbl == prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx])
            {
                prAudKeyIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[i];
                pu4AudPts = (UINT32*)prAudKeyIdxTbl->u.pv_idx_tbl_entry;

                LOG(5, "prAudKeyIdxTbl = 0x%08x\n", prAudKeyIdxTbl);
            }
            else
            {
                prAudIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[i]->pt_next_partial;
                prAudTmpKeyIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx];
                
                while (prAudTmpKeyIdxTbl != prAudBaseKeyIdxTbl)
                {
                    prAudIdxTbl = prAudIdxTbl->pt_next_partial;
                    prAudTmpKeyIdxTbl = prAudTmpKeyIdxTbl->pt_next_partial;
                }

                prAudKeyIdxTbl = prAudIdxTbl;
                pu4AudPts = (UINT32*)prAudIdxTbl->u.pv_idx_tbl_entry;
                LOG(5, "prAudKeyIdxTbl = 0x%08x\n", prAudKeyIdxTbl);
            }
            
            if(pu4AudPts)
            {
                prSwdmxAviInfo->u4AudPts[i] = 
                    pu4AudPts[prSwdmxAviInfo->u4CurSentKey];
                LOG(5, "Avi Seek Audio pts = 0x%08x\n", 
                    pu4AudPts[prSwdmxAviInfo->u4CurSentKey]);
            }
            else
            {
                LOG(1, "2audio table is NULL\n");
            }

        }

        if(prSwdmxAviInfo->prVidKeyIdxTbl)
        {
            prSwdmxAviInfo->u8CurDmxMoviPos = 
                    prSwdmxAviInfo->prVidKeyIdxTbl->ui8_base_offset + 
                    prSwdmxAviInfo->prVidKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurSentKey].ui4_relative_offset -
                    (AVI_PARSER_OFFSET + u1ReadDrmSize); 
         }
    }
    else
    {
        LOG(5, "There is no audio and video index table for Avi seek\n");
    }

    prSwdmxAviInfo->fgIsSeek = TRUE;
    prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
    prSwdmxAviInfo->u8CurMoviPos = prSwdmxAviInfo->u8CurDmxMoviPos;
    prSwdmxAviInfo->u4CurDmxFeederRPtr = prSwdmxInst->rFeederInfo.u4WriteAddr;
    // Update read ptr to feeder
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));

    // Set STC, temp, should update by audio (audio master)
    //STC_SetStcValue(prSwdmxInst->u1StcId, prSwdmxAviInfo->u4CurVidPts);
    
    LOG(5, "_SWDMX_AviSeek Key = %x\n", prSwdmxAviInfo->u4CurSentKey);
    LOG(5,"At end of _SWDMX_AviSeek the CurDmxPos is %llx\n",prSwdmxAviInfo->u8CurDmxMoviPos);
    return TRUE;
}

static BOOL _SWDMX_AviTimeSeek(SWDMX_INFO_T * prSwdmxInst,
    UINT64 u8SeekPos,
    INT32 i4PlaySpeed ,
    SWDMX_RANGE_LIST_T * prRangeList)
{
    UINT8 u1Idx = 0;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
  
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;   
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    UNUSED(_SWDMX_AviSeek(prSwdmxInst, u8SeekPos, prRangeList));
    
    LOG(5, "u8SeekPos = %llu\n", u8SeekPos);

    prSwdmxAviInfo->u8VSeekPts = u8SeekPos;
    prSwdmxAviInfo->u8ASeekPts = u8SeekPos;
    prSwdmxAviInfo->fgFeederInvalid = TRUE;
    prSwdmxAviInfo->fgPreviouIsSeek = TRUE;
    prSwdmxAviInfo->u4RecSeekTime = u8SeekPos;
    if((prSwdmxInst->fgPreviouIsPause) && (!prSwdmxAviInfo->fgAbRepeat))
    {
        prSwdmxAviInfo->fgPauseToSeek = TRUE;
        prSwdmxAviInfo->u8PauseToSeekPos = u8SeekPos;
        prSwdmxInst->fgPreviouIsPause = FALSE;
    }
    if (prSwdmxAviInfo->fgMp3Abr)
    {
        LOG(5, "ABR Mp3\n");
        prSwdmxAviInfo->u8ASeekPts = 0xFFFFFFFF;
        prSwdmxAviInfo->u8ASeekPts <<= 32;
        prSwdmxAviInfo->u8ASeekPts += 0xFFFFFFFF;
        if(prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)
        {
            AUD_SetStartPts(prSwdmxInst->u1AdecId, prSwdmxAviInfo->u4AudPts[prSwdmxAviInfo->u1CurATrackIdx]);
        }
    }
    
    if (prSwdmxAviInfo->fgEnVideo)
    {
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, TRUE);
            VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id);
        }
        MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
        VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);

        if(i4PlaySpeed >0)
        {    
            if(prSwdmxAviInfo->fgIs3DFile)
            {
            	VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u8SeekPos); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
//                        VDEC_SetRenderPts(prSwdmxInst->u1Vdec2Id, u8SeekPos); //original, 2011/03/13
            }
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u8SeekPos); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
//                    VDEC_SetRenderPts(prSwdmxInst->u1VdecId, u8SeekPos); //original, 2011/03/13
        }
        else
        {   
            if(prSwdmxAviInfo->fgIs3DFile)
            {
            	VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, 0); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
//                        VDEC_SetRenderPts(prSwdmxInst->u1Vdec2Id, 0); //original, 2011/03/13
            }
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
//                    VDEC_SetRenderPts(prSwdmxInst->u1VdecId, 0); //original, 2011/03/13
        }
        
        if(prSwdmxAviInfo->fgIs3DFile)
        {
             VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, u8SeekPos, NULL);
        }
        VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, u8SeekPos, NULL);

        if(prSwdmxAviInfo->fgIs3DFile)
        {
            DMX_MM_FlushBuffer(prSwdmxAviInfo->u1Vid2Pid);
        }
        DMX_MM_FlushBuffer(prSwdmxAviInfo->u1VidPid);
    }
    if(prSwdmxAviInfo->fgEnADmx)
    {
        DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);
    }
    // set stc
    STC_StopStc(prSwdmxInst->u1StcId);
    STC_SetStcValue(prSwdmxInst->u1StcId, u8SeekPos);
        
    for ( ;(u1Idx < prSwdmxAviInfo->u1TotalATrack)&&(u1Idx<AVI_DIVX_MAX_AUD_TRACK); u1Idx++)
    {
            DMX_MM_FlushBuffer( prSwdmxAviInfo->u1AudPreBufPid[u1Idx]);
    }
    if(i4PlaySpeed == 1000)
    {
      prSwdmxAviInfo->fgRecSpeedChangeForAud = TRUE;
      prSwdmxAviInfo->i4RecPrevSpeedForAud = prSwdmxAviInfo->i4PrevSpeed;
    }

    return TRUE;
}


static BOOL _SWDMX_AviResetFeeder(SWDMX_INFO_T* prSwdmxInst)
{
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    FEEDER_REQ_DATA_T rFeederReqData;
     INT32 i;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
 
    LOG(3, "Feeder is invalid, Cur Feeder Req ID = %ld,Feeder Rev ID = %ld, Req times = %ld, Cur Dmx Pos = %llu\n",
        prSwdmxAviInfo->u4FeederReqID,
        prSwdmxAviInfo->u4FeederReqReceiveID,
        prSwdmxAviInfo->i4ReqReadTimes,
        prSwdmxAviInfo->u8CurDmxMoviPos);
    
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != 
      FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        return FALSE;
    }

    for (i = 0; i < prSwdmxAviInfo->i4ReqReadTimes; i++)
    {
        _SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1VdecId);
        #if 0
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            _SWDMX_AviGetDataFromFeeder(prSwdmxInst, &rFeederReqData, prSwdmxInst->u1Vdec2Id);
        }
        #endif
    }
    
    prSwdmxAviInfo->fgIsSeek = TRUE;
    prSwdmxAviInfo->u4LeftBytesInFeeder = 0;
    prSwdmxAviInfo->u8CurMoviPos = prSwdmxAviInfo->u8CurDmxMoviPos;
    prSwdmxAviInfo->fgFeederInvalid = FALSE;
    prSwdmxAviInfo->fgReqDataMiss = FALSE;
    prSwdmxAviInfo->i4ReqReadTimes = 0;
    prSwdmxAviInfo->u4FeederReqID += 1;
    prSwdmxAviInfo->u4FeederReqReceiveID = prSwdmxAviInfo->u4FeederReqID;
    if(prSwdmxAviInfo->fgIs3DFile)
    {
         prSwdmxAviInfo->fgReqSubDataMiss = FALSE;
    }
    prSwdmxAviInfo->u4CurDmxFeederRPtr = prSwdmxInst->rFeederInfo.u4WriteAddr;

    // Update read ptr to feeder
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, VIRTUAL(prSwdmxAviInfo->u4CurDmxFeederRPtr));
    return TRUE;
}


static void _SWDMX_AviCalculatePts(SWDMX_INFO_T *prSwdmxInst,
    UINT8 u1HdrType, 
    UINT8 u1TrackIdx, 
    UINT32 u4ChunkSize)
{
    UINT64 u8Numerator = 0, u8Denominator = 0;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    if (u1HdrType == eSWDMX_MEDIA_VIDEO)
    {   
        if(prSwdmxAviInfo->fgIs3DFile)
        {
            if(prSwdmxAviInfo->fgIsLeftVideo)
            {
                if(prSwdmxAviInfo->prVidIdxTbl)
                {
                    u8Numerator = 
                        (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD 
                        + prSwdmxAviInfo->u8VRemainder;
                    u8Denominator = 
                        (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_rate;

                    prSwdmxAviInfo->u4CurVidPts += u8Div6432(u8Numerator, u8Denominator, &prSwdmxAviInfo->u8VRemainder);
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                    {
                        UINT32 u4Pts = prSwdmxAviInfo->u4CurVidPts;
                    
                        if (prSwdmxInst->fgIsBadInterleaved && u4Pts > SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD)
                        {
                            UINT32 u4CurStc = STC_GetStcValue(prSwdmxInst->u1StcId);
                            if ((u4CurStc + SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD) < u4Pts)
                            {
                                LOG(1, "id,stc,pts=(%d,%d,%d)\n", prSwdmxInst->u1SwdmxId, u4CurStc, u4Pts);
                                x_thread_delay(SWDMX_AVI_BAD_INTERLEAVED_DELAY);
                            }
                        }
                    }
#endif
                }
               else
                {
                    LOG(3, "Video table is NULL\n");
                    return ;
                }            
            }
            else 
            {
                if(prSwdmxAviInfo->prSubVidIdxTbl)
                {
                    u8Numerator = 
                        (UINT64)prSwdmxAviInfo->prSubVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD 
                        + prSwdmxAviInfo->u8VRRemainder;
                    u8Denominator = 
                        (UINT64)prSwdmxAviInfo->prSubVidIdxTbl->ui4_rate;
                    prSwdmxAviInfo->u4CurSubVidPts += u8Div6432(u8Numerator, u8Denominator, &prSwdmxAviInfo->u8VRRemainder);
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                    {
                        UINT32 u4Pts = prSwdmxAviInfo->u4CurSubVidPts;
        
                        if (prSwdmxInst->fgIsBadInterleaved && u4Pts > SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD)
                        {
                            UINT32 u4CurStc = STC_GetStcValue(prSwdmxInst->u1StcId);
                            if ((u4CurStc + SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD) < u4Pts)
                            {
                                LOG(1, "id,stc,pts=(%d,%d,%d)\n", prSwdmxInst->u1SwdmxId, u4CurStc, u4Pts);
                                x_thread_delay(SWDMX_AVI_BAD_INTERLEAVED_DELAY);
                            }
                        }
                    }
#endif                    
                }
               else
                {
                    LOG(3, "Sub Video table is NULL\n");
                    return ;
                }              
            }
        }
        else
        {
             if(prSwdmxAviInfo->prVidIdxTbl)
             {
                 u8Numerator = 
                     (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_scale * DEMUX_AVI_AV_PERIOD 
                     + prSwdmxAviInfo->u8VRemainder;
                 u8Denominator = 
                     (UINT64)prSwdmxAviInfo->prVidIdxTbl->ui4_rate;
                 prSwdmxAviInfo->u4CurVidPts += u8Div6432(u8Numerator, u8Denominator, &prSwdmxAviInfo->u8VRemainder);
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                 {
                     UINT32 u4Pts = prSwdmxAviInfo->u4CurVidPts;
    
                     if (prSwdmxInst->fgIsBadInterleaved && u4Pts > SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD)
                     {
                         UINT32 u4CurStc = STC_GetStcValue(prSwdmxInst->u1StcId);
                         if ((u4CurStc + SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD) < u4Pts)
                         {
                             LOG(1, "id,stc,pts=(%d,%d,%d)\n", prSwdmxInst->u1SwdmxId, u4CurStc, u4Pts);
                             x_thread_delay(SWDMX_AVI_BAD_INTERLEAVED_DELAY);
                         }
                     }
                 }
#endif                    
             }
            else
            {
                 LOG(3, "Video table is NULL\n");
                 return ;
             }            
        }
    }
    else if ((u1HdrType == eSWDMX_MEDIA_AUDIO ||u1HdrType == eSWDMX_MEDIA_SKIPPED_AUDIO) &&
        (u1TrackIdx < prSwdmxAviInfo->u1TotalATrack))
    {
        if (prSwdmxAviInfo->fgKeyExist)
        {
            if(prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx])
            {
                if (prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx]->b_is_VBR)
                {
                    if((prSwdmxAviInfo->u4ADecType[prSwdmxAviInfo->u1CurATrackIdx] == ENUM_SWDMX_ADEC_PCM)
                        && (prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx]->ui4_scale == 1))//DTV00419690
                    {
                        u8Numerator = (UINT64)u4ChunkSize * DEMUX_AVI_AV_PERIOD
                        + prSwdmxAviInfo->u8ARemainder[u1TrackIdx];
                        u8Denominator = (UINT64)prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx]->ui4_avg_bytes_per_sec;
                    }
                    else
                    {
                        u8Numerator = 
                            (UINT64)prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx]->ui4_scale * DEMUX_AVI_AV_PERIOD
                            + prSwdmxAviInfo->u8ARemainder[u1TrackIdx];
                        u8Denominator = 
                            (UINT64)prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx]->ui4_rate;
                    }
                }
                else
                {
                    u8Numerator = (UINT64)u4ChunkSize * DEMUX_AVI_AV_PERIOD
                        + prSwdmxAviInfo->u8ARemainder[u1TrackIdx];
                    u8Denominator = 
                        (UINT64)prSwdmxAviInfo->parAudKeyIdxTbl[u1TrackIdx]->ui4_avg_bytes_per_sec;
                }
            }
            else
            {
                LOG(3, "audio key table is NULL\n");
                return ;
            }
        }
        else
        {   
            if(prSwdmxAviInfo->prAudIdxTbl[u1TrackIdx])
            {
                if (prSwdmxAviInfo->prAudIdxTbl[u1TrackIdx]->b_is_VBR)
                {
                    u8Numerator = 
                        (UINT64)prSwdmxAviInfo->prAudIdxTbl[u1TrackIdx]->ui4_scale * DEMUX_AVI_AV_PERIOD
                        + prSwdmxAviInfo->u8ARemainder[u1TrackIdx];
                    u8Denominator = 
                        (UINT64)prSwdmxAviInfo->prAudIdxTbl[u1TrackIdx]->ui4_rate;
                }
                else
                {
                    u8Numerator = (UINT64)u4ChunkSize * DEMUX_AVI_AV_PERIOD
                        + prSwdmxAviInfo->u8ARemainder[u1TrackIdx];
                    u8Denominator = 
                        (UINT64)prSwdmxAviInfo->prAudIdxTbl[u1TrackIdx]->ui4_avg_bytes_per_sec;
                }
            }
            else
            {
                LOG(3, "Audio table is NULL\n");
                return ;
            }
        }
        prSwdmxAviInfo->u4AudPts[u1TrackIdx]  += u8Div6432(u8Numerator, u8Denominator, &prSwdmxAviInfo->u8ARemainder[u1TrackIdx]);
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        {
            UINT32 u4Pts = prSwdmxAviInfo->u4AudPts[u1TrackIdx];

            if (prSwdmxInst->fgIsBadInterleaved && u4Pts > SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD)
            {
                UINT32 u4CurStc = STC_GetStcValue(prSwdmxInst->u1StcId);
                if ((u4CurStc + SWDMX_AVI_BAD_INTERLEAVED_PTS_THRD) < u4Pts)
                {
                    LOG(1, "id,stc,pts=(%d,%d,%d)\n", prSwdmxInst->u1SwdmxId, u4CurStc, u4Pts);
                    x_thread_delay(SWDMX_AVI_BAD_INTERLEAVED_DELAY);
                }
            }
        }
#endif
    }
}


static BOOL _SWDMX_AviCalculateRepeatFrame(SWDMX_INFO_T* prSwdmxInst, INT32 i4PlaySpeed, UINT8 u1VdecId)
{
    UINT32 u4CurKey = 0;
    MM_RANGE_ELMT_IDXTBL_T *prKeyIdxTbl = NULL;
    BOOL fg3DValue = FALSE;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

    
    
    u4CurKey = prSwdmxAviInfo->u4CurSentKey;
    prKeyIdxTbl = prSwdmxAviInfo->prCurVidKeyIdxTbl;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    if (prSwdmxAviInfo->fgIs3DFile)
    {
        fg3DValue = _SWDMX_Avi3DCalculateRepeatFrame(prSwdmxInst,i4PlaySpeed,u1VdecId);
        return fg3DValue;
    }
    else
    {
        if (prSwdmxAviInfo->u4VidRepeatFrame == 0)
        {
            if (prSwdmxAviInfo->u4CurRepeatFrameIdx != u4CurKey &&
                (prSwdmxAviInfo->u4CurRepeatFrameIdx < prKeyIdxTbl->ui4_number_of_entry))
            {
                UINT32 u4PtsDif = 0, u4MaxRepeatFrame;
        
                u4MaxRepeatFrame = AVI_MAX_REPEAT_FRAME;//(prSwdmxAviInfo->u4VidFps * 1000 * 2)/abs(i4PlaySpeed);

                if (i4PlaySpeed > 2000)
                {
                    if ((u4CurKey < (prKeyIdxTbl->ui4_number_of_entry)) &&
                        (prKeyIdxTbl->ui4_number_of_entry > AVI_MINIMUM_KEY_NUM))
                    {
                        u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts -
                            prKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurRepeatFrameIdx].ui4_pts;

                        prSwdmxAviInfo->u4VidRepeatFrame = 
                            (UINT32)((u4PtsDif * prSwdmxAviInfo->u4VidFps)/(90 * i4PlaySpeed));
                    }
                    else
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    }
                }
                else if (i4PlaySpeed < 0)
                {
                    if ((prKeyIdxTbl->ui4_number_of_entry > AVI_MINIMUM_KEY_NUM))
                    {
                        u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurRepeatFrameIdx].ui4_pts -
                            prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts;

                        prSwdmxAviInfo->u4VidRepeatFrame = 
                            (UINT32)((u4PtsDif * prSwdmxAviInfo->u4VidFps)/(90 * abs(i4PlaySpeed)));;
                    }
                    else
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    }
                }

                if(i4PlaySpeed > 2000)
                {
                    prSwdmxAviInfo->u4VidRepeatFrame = (u4MaxRepeatFrame > prSwdmxAviInfo->u4VidRepeatFrame) ?
                        (prSwdmxAviInfo->u4VidRepeatFrame) : u4MaxRepeatFrame;
                }

                if ((u4CurKey == prKeyIdxTbl->ui4_number_of_entry - 1) ||(u4CurKey == 0))
                {
                    prSwdmxAviInfo->u4VidRepeatFrame = (AVI_FIX_REPEAT_FRAME / (abs(i4PlaySpeed) / 1000)) + 1;

                    /*if (prSwdmxAviInfo->u4VidRepeatFrame > AVI_MAX_REPEAT_FRAME)
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = AVI_MAX_REPEAT_FRAME;
                    }
                    else if (prSwdmxAviInfo->u4VidRepeatFrame < AVI_MIN_REPEAT_FRAME)
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = AVI_MIN_REPEAT_FRAME;
                    }*/                        
                }

                LOG(5, "u4PtsDif = %ld Current Idx = %ld Repeat frame = %ld\n", 
                    u4PtsDif, u4CurKey, prSwdmxAviInfo->u4VidRepeatFrame);

                if (prSwdmxAviInfo->u4VidRepeatFrame == 0)
                {
                    return FALSE;
                }

                prSwdmxAviInfo->u4CurRepeatFrameIdx = u4CurKey;
                LOG(5, "Repeat Idx = %ld\n", prSwdmxAviInfo->u4CurRepeatFrameIdx);

                return TRUE;
            }
            else
            {
                // Force to play the first key frame
                if (prSwdmxAviInfo->fgStartTrick)
                {
                    prSwdmxAviInfo->fgStartTrick = FALSE;
                    prSwdmxAviInfo->u4VidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    return TRUE;
                }
                
                return FALSE;
            }
        }

        prSwdmxAviInfo->u4VidRepeatFrame--;
    }
    
    return TRUE;
}

static BOOL _SWDMX_Avi3DCalculateRepeatFrame(
    SWDMX_INFO_T* prSwdmxInst, 
    INT32 i4PlaySpeed,  
    UINT8 u1VdecId)
{
    UINT32 u4CurKey = 0;
    MM_RANGE_ELMT_IDXTBL_T *prKeyIdxTbl = NULL;
    MM_RANGE_ELMT_IDXTBL_T *prSubKeyIdxTbl = NULL;
    UINT32 u4CurSubKey = 0;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    u4CurKey = prSwdmxAviInfo->u4CurSentKey;
    prKeyIdxTbl = prSwdmxAviInfo->prCurVidKeyIdxTbl;
    
    prSubKeyIdxTbl = prSwdmxAviInfo->prSubVidKeyIdxTbl;
    u4CurSubKey = prSwdmxAviInfo->u4CurSentSubKey;
 
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    
    if(u1VdecId == prSwdmxInst->u1Vdec2Id)
    {
        if (prSwdmxAviInfo->u4SubVidRepeatFrame == 0)
        {
            if (prSwdmxAviInfo->u4PreRepeatSubFrameIdx != u4CurSubKey &&
                (prSwdmxAviInfo->u4CurRepeatSubFrameIdx < prSubKeyIdxTbl->ui4_number_of_entry))
            {
                UINT32 u4SubPtsDif = 0, u4SubMaxRepeatFrame;
                u4SubMaxRepeatFrame = AVI_MAX_REPEAT_FRAME;//(prSwdmxAviInfo->u4VidFps * 1000 * 2)/abs(i4PlaySpeed);
                if(i4PlaySpeed == 2000)
                {
                    prSwdmxAviInfo->u4SubVidRepeatFrame = 7;
                }
                else if (i4PlaySpeed > 2000)
                {
                    if ((u4CurSubKey < (prSubKeyIdxTbl->ui4_number_of_entry)) &&
                        (prSubKeyIdxTbl->ui4_number_of_entry > AVI_MINIMUM_KEY_NUM))
                    {
                        u4SubPtsDif = prSubKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurSubKey].ui4_pts -
                            prSubKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4PreRepeatSubFrameIdx].ui4_pts;
                        prSwdmxAviInfo->u4SubVidRepeatFrame = 
                            (UINT32)((u4SubPtsDif * (prSwdmxAviInfo->u4VidFps))/(90 * i4PlaySpeed));
                    }
                    else
                    {
                        prSwdmxAviInfo->u4SubVidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    }
                }
                else if (i4PlaySpeed < 0)
                {
                    if ((prSubKeyIdxTbl->ui4_number_of_entry > AVI_MINIMUM_KEY_NUM))
                    {
                        u4SubPtsDif = prSubKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4PreRepeatSubFrameIdx ].ui4_pts -
                            prSubKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurSubKey].ui4_pts;
                        prSwdmxAviInfo->u4SubVidRepeatFrame = 
                            (UINT32)((u4SubPtsDif * (prSwdmxAviInfo->u4VidFps))/(90 * abs(i4PlaySpeed)));
                    }
                    else
                    {
                        prSwdmxAviInfo->u4SubVidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    }
                }
                prSwdmxAviInfo->u4SubVidRepeatFrame = (u4SubMaxRepeatFrame > prSwdmxAviInfo->u4SubVidRepeatFrame) ?
                    (prSwdmxAviInfo->u4SubVidRepeatFrame) : u4SubMaxRepeatFrame;

                if ((u4CurSubKey == prSubKeyIdxTbl->ui4_number_of_entry - 1) ||
                    (u4CurSubKey == 0))
                {
                    prSwdmxAviInfo->u4SubVidRepeatFrame = 
                        (AVI_FIX_REPEAT_FRAME/(abs(i4PlaySpeed)/1000));                       
                }
                LOG(5, "u4SubPtsDif = %ld Current Sub Idx = %ld Sub Repeat frame = %ld\n", 
                    u4SubPtsDif, u4CurSubKey, prSwdmxAviInfo->u4SubVidRepeatFrame);
                if (prSwdmxAviInfo->u4SubVidRepeatFrame == 0)
                {
                    return FALSE;
                }
                
                prSwdmxAviInfo->u4CurRepeatSubFrameIdx = u4CurSubKey;
                prSwdmxAviInfo->u4PreRepeatSubFrameIdx = prSwdmxAviInfo->u4CurRepeatSubFrameIdx;
                
                LOG(5, "Sub Repeat Idx = %ld\n", prSwdmxAviInfo->u4CurRepeatSubFrameIdx);
                return TRUE;
            }
            else
            {
                if (prSwdmxAviInfo->fgStartTrick)
                {
                     if(prSwdmxAviInfo->fgIsSupport3DTrick)
                    {
                        if(prSwdmxAviInfo->u4VidRepeatFrame != 0)
                        {
                            prSwdmxAviInfo->fgStartTrick = FALSE;
                        }
                    }
                    prSwdmxAviInfo->u4SubVidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    if(prSwdmxAviInfo->u4SubVidRepeatFrame == 0)
                    {
                        prSwdmxAviInfo->u4SubVidRepeatFrame = 1;
                    }
                    return TRUE;
                }
                return FALSE;
            }
        }
        prSwdmxAviInfo->u4SubVidRepeatFrame--;
    }
    else if(u1VdecId == prSwdmxInst->u1VdecId)
    {
        if (prSwdmxAviInfo->u4VidRepeatFrame == 0)
        {
            if (prSwdmxAviInfo->u4PreRepeatFrameIdx != u4CurKey &&
                (prSwdmxAviInfo->u4CurRepeatFrameIdx < prKeyIdxTbl->ui4_number_of_entry))
            {
                UINT32 u4PtsDif = 0, u4MaxRepeatFrame;
                u4MaxRepeatFrame = AVI_MAX_REPEAT_FRAME;//(prSwdmxAviInfo->u4VidFps * 1000 * 2)/abs(i4PlaySpeed);
                if(i4PlaySpeed == 2000)
                {
                    prSwdmxAviInfo->u4VidRepeatFrame = 7;
                }
                else if (i4PlaySpeed > 2000)
                {
                    if ((u4CurKey < (prKeyIdxTbl->ui4_number_of_entry)) &&
                        (prKeyIdxTbl->ui4_number_of_entry > AVI_MINIMUM_KEY_NUM))
                    {
                        if(ENABLE_NEWTRACK(prSwdmxInst) &&(prSwdmxAviInfo->fgIsSupport3DTrick))
                        {
                            u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts -
                                prKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4PreRepeatFrameIdx].ui4_pts;
                        }
                        else
                        {
                            u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts -
                                prKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurRepeatFrameIdx].ui4_pts;
                         }
                        prSwdmxAviInfo->u4VidRepeatFrame = 
                            (UINT32)((u4PtsDif * (prSwdmxAviInfo->u4VidFps))/(90 * i4PlaySpeed));
                    }
                    else
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    }
                }
                else if (i4PlaySpeed < 0)
                {
                    if ((prKeyIdxTbl->ui4_number_of_entry > AVI_MINIMUM_KEY_NUM))
                    {
                        if(ENABLE_NEWTRACK(prSwdmxInst) &&(prSwdmxAviInfo->fgIsSupport3DTrick))    
                        {
                            u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4PreRepeatFrameIdx].ui4_pts -
                                prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts;
                        }
                        else
                        {
                            u4PtsDif = prKeyIdxTbl->u.pt_pb_idx_tbl_entry[prSwdmxAviInfo->u4CurRepeatFrameIdx].ui4_pts -
                                prKeyIdxTbl->u.pt_pb_idx_tbl_entry[u4CurKey].ui4_pts;
                        }
                        prSwdmxAviInfo->u4VidRepeatFrame = 
                            (UINT32)((u4PtsDif * (prSwdmxAviInfo->u4VidFps))/(90 * abs(i4PlaySpeed)));
                    }
                    else
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    }
                }
                prSwdmxAviInfo->u4VidRepeatFrame = (u4MaxRepeatFrame > prSwdmxAviInfo->u4VidRepeatFrame) ?
                    (prSwdmxAviInfo->u4VidRepeatFrame) : u4MaxRepeatFrame;
                if ((u4CurKey == prKeyIdxTbl->ui4_number_of_entry - 1) ||
                    (u4CurKey == 0))
                {
                    prSwdmxAviInfo->u4VidRepeatFrame = 
                        (AVI_FIX_REPEAT_FRAME/(abs(i4PlaySpeed)/1000));
                }
                LOG(5, "u4PtsDif = %ld Current Idx = %ld Repeat frame = %ld\n", 
                    u4PtsDif, u4CurKey, prSwdmxAviInfo->u4VidRepeatFrame);
                if (prSwdmxAviInfo->u4VidRepeatFrame == 0)
                {
                    return FALSE;
                }
                prSwdmxAviInfo->u4CurRepeatFrameIdx = u4CurKey;
                prSwdmxAviInfo->u4PreRepeatFrameIdx = prSwdmxAviInfo->u4CurRepeatFrameIdx;
                LOG(5, "Repeat Idx = %ld\n", prSwdmxAviInfo->u4CurRepeatFrameIdx);
                return TRUE;
            }
            else
            {
                if (prSwdmxAviInfo->fgStartTrick)
                {
                    if(ENABLE_NEWTRACK(prSwdmxInst) &&(prSwdmxAviInfo->fgIsSupport3DTrick))
                    {
                        if(prSwdmxAviInfo->u4SubVidRepeatFrame != 0)
                        {
                            prSwdmxAviInfo->fgStartTrick = FALSE;
                        }
                    }
                    else
                    {
                        prSwdmxAviInfo->fgStartTrick = FALSE;
                    }
                    prSwdmxAviInfo->u4VidRepeatFrame = (prSwdmxAviInfo->u4VidFps * 1000)/abs(i4PlaySpeed);
                    if(prSwdmxAviInfo->u4VidRepeatFrame == 0)
                    {
                        prSwdmxAviInfo->u4VidRepeatFrame = 1;
                    }                   
                    return TRUE;
                }                
                return FALSE;
            }
        }
        prSwdmxAviInfo->u4VidRepeatFrame--;
    }

    return TRUE;
}


BOOL _SWDMX_AviReadBuffer(UINT8 u1SrcId, ENUM_SWDMX_STRM_TYPE_T eStrmType, 
    SWDMX_READ_INFO_T *prReadInfo)
{
    BOOL  fgRet = FALSE;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    switch (eStrmType)
    {
        case eSWDMX_STRM_TYPE_SUBTITLE:
            LOG(3, "Avi Get Subtitle Buffer serial number = 0x%x Frame Address = 0x%08x",
                prReadInfo->u1SerialNumber,
                prReadInfo->u4FrameAddr);
                
            fgRet = DMX_MM_GetBuffer( prSwdmxAviInfo->u1SubPid , 
                prReadInfo->u1SerialNumber, 
                (UINT32)prReadInfo->u4FrameAddr,
            0, (UINT32)prReadInfo->u4CopyLen,
            prReadInfo->pucDest);

            if(!fgRet)
            {
                LOG(3, "_SWDMX_AviReadBuffer, get buffer error\n");
            }
            break;
        default:
            return FALSE;
    }
    
    return fgRet;
}


BOOL _SWDMX_AviReleaseBuffer(UINT8 u1SrcId, ENUM_SWDMX_STRM_TYPE_T eStrmType, 
    SWDMX_READ_INFO_T *prReadInfo)
{
    BOOL  fgRet = FALSE;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    
    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
    switch (eStrmType)
    {
        case eSWDMX_STRM_TYPE_SUBTITLE:
            fgRet = DMX_MM_UnlockBuffer( prSwdmxAviInfo->u1SubPid , 
                prReadInfo->u1SerialNumber, 
                (UINT32)prReadInfo->u4CopyLen,
                (UINT32)prReadInfo->u4FrameAddr);

            if(!fgRet)
            {
                LOG(3, "_SWDMX_AviReleaseBuffer, unlock buffer error\n");
            }
            break;
        default:
            return FALSE;
    }
    
    return fgRet;
}


/**
*/
BOOL _SWDMX_AVIStop(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = NULL;
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    DMX_MM_T rPid;

    prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    rPid.fgEnable = FALSE;

    SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
   
    if (prSwdmxAviInfo->fgEnVDmx)
    {
        prSwdmxAviInfo->fgEnVDmx = FALSE;

        VERIFY(DMX_MM_Set(prSwdmxAviInfo->u1VidPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxAviInfo->u1VidPid);
        DMX_MUL_FreePidx(prSwdmxAviInfo->u1VidPid);
    }
    if(prSwdmxAviInfo->fgIs3DFile)
    {
        if (prSwdmxAviInfo->fgEnV2Dmx)
        {
            prSwdmxAviInfo->fgEnV2Dmx = FALSE;
            VERIFY(DMX_MM_Set(prSwdmxAviInfo->u1Vid2Pid, DMX_MM_FLAG_VALID, &rPid));
            DMX_MM_Free(prSwdmxAviInfo->u1Vid2Pid);
            DMX_MUL_FreePidx(prSwdmxAviInfo->u1Vid2Pid);
        }
    }
    if (prSwdmxAviInfo->fgEnADmx)
    {
        INT32 i;

        prSwdmxAviInfo->fgEnADmx = FALSE;
        VERIFY(DMX_MM_Set(prSwdmxAviInfo->u1AudPid, DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free(prSwdmxAviInfo->u1AudPid);
        DMX_MUL_FreePidx( prSwdmxAviInfo->u1AudPid );
        
        for (i = 0; i < prSwdmxAviInfo->u1TotalATrack; i++)
        {
            VERIFY(DMX_MM_Set(prSwdmxAviInfo->u1AudPreBufPid[i],
                  DMX_MM_FLAG_VALID, &rPid));
            DMX_MM_Free(prSwdmxAviInfo->u1AudPreBufPid[i]);
            DMX_MUL_FreePidx( prSwdmxAviInfo->u1AudPreBufPid[i]);
            DMX_AudHandler_Free(prSwdmxAviInfo->u1AudPreBufPid[i]);
        }
        DMX_AudHandler_SetEnable(FALSE);
    }
    if (prSwdmxAviInfo->fgEnSubDmx)
    {
        prSwdmxAviInfo->fgEnSubDmx = FALSE;
        VERIFY(DMX_MM_Set( prSwdmxAviInfo->u1SubPid , DMX_MM_FLAG_VALID, &rPid));
        DMX_MM_Free( prSwdmxAviInfo->u1SubPid );
        DMX_MUL_FreePidx( prSwdmxAviInfo->u1SubPid );
    }
    
    if (u1SrcId < SWDMX_SOURCE_MAX) 
    {
         g_fgNotSupport[u1SrcId] = FALSE;
    }
    x_memset(&prSwdmxInst->rFmtInfo.rSwdmxAviInfo, 0, sizeof(SWDMX_AVI_INFO_T));
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
    prSwdmxAviInfo->i4PrevSpeed = 1000;
#endif

    return TRUE;
}

static BOOL _SWDMX_AviFlush(SWDMX_INFO_T * prSwdmxInst)
{
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    if(prSwdmxAviInfo->fgIs3DFile)
    {
        MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, TRUE);
        VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id);
    }
    MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
    VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
    
    if(prSwdmxAviInfo->fgIs3DFile)
    {
        DMX_MM_FlushBuffer(prSwdmxAviInfo->u1Vid2Pid);
        prSwdmxAviInfo->fgReqSubDataMiss = FALSE;
    }
    DMX_MM_FlushBuffer(prSwdmxAviInfo->u1VidPid);
    DMX_MM_FlushBuffer(prSwdmxAviInfo->u1AudPid);
    
    _SWDMX_FlushReqMsgQ(prSwdmxInst->u1SwdmxId);
    prSwdmxAviInfo->i4ReqReadTimes = 0;
    prSwdmxAviInfo->fgReqDataMiss = FALSE;
    prSwdmxAviInfo->fgRecSpeedChangeForAud = TRUE;//FF TO NORMAL/TIMESEEK 
    prSwdmxAviInfo->i4RecPrevSpeedForAud = prSwdmxAviInfo->i4PrevSpeed;

    return TRUE;
}

static BOOL _SWDMX_AviValidHdrBps(UINT32 u4HdrBps, UINT32 u4DspBps)
{
    // These values are defined in 11172-3 page 21.
    const UINT16 au2NormalBps[16] = {16,  24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};
    const UINT16 u2Precision = 1;
    UINT16 u2HdrBps = (UINT16)(u4HdrBps * 8 / 1000);
    UINT16 u2DspBps = (UINT16)(u4DspBps * 8 / 1000);
    UINT16 u2HdrPtn = 16;
    UINT16 u2DspPtn = 16;
    UINT32 i;

    LOG(5, "u4HdrBps %d u4DspBps %x\n", u4HdrBps,u4DspBps);

    for(i = 0; i < 16; i++)
    {
        if ((u2HdrBps >= au2NormalBps[i] - u2Precision) && (u2HdrBps <= au2NormalBps[i] + u2Precision))
        {
            u2HdrPtn = i;
        }
        if ((u2DspBps >= au2NormalBps[i] - u2Precision) && (u2DspBps <= au2NormalBps[i] + u2Precision))
        {
            u2DspPtn = i;
        }
    }
    if ((u2HdrPtn < 16) && (u2HdrPtn == u2DspPtn))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


BOOL _SWDMX_AviAudioDrvCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    if(eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        //BOOL fgMp3Vbr = FALSE;
        AUD_DEC_INFO_T rADecInfo;
        MM_RANGE_ELMT_IDXTBL_T *prAudIdxTbl = NULL;
        
        SWDMX_INFO_T* prSwdmxInst = NULL;
        SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
        
        prSwdmxInst = _SWDMX_GetInst(u1SrcId);
        prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;

        SWDMX_AVI_VERIFY_NULL_ERROR(prSwdmxInst);
        
        x_memset(&rADecInfo, 0, sizeof(AUD_DEC_INFO_T));

        //AUD_DspSetMp3PreparsingMode(TRUE);
        //fgMp3Vbr = AUD_DspIsMp3Vbr();
        AUD_DspGetMpegAudInfo(prSwdmxInst->u1AdecId, &rADecInfo);

        if ((prSwdmxAviInfo->fgKeyExist) &&
            (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)&&
            (prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx]))
        {
            prAudIdxTbl = prSwdmxAviInfo->parAudKeyIdxTbl[prSwdmxAviInfo->u1CurATrackIdx];
        }
        else if((prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack)&&
           ( prSwdmxAviInfo->prAudIdxTbl[prSwdmxAviInfo->u1CurATrackIdx] !=NULL))
        {
            prAudIdxTbl = prSwdmxAviInfo->prAudIdxTbl[prSwdmxAviInfo->u1CurATrackIdx];
        }
        else
        {
            LOG(3,"error happen: no audio table\n");
        }

        if (prAudIdxTbl)
        {
            if ((rADecInfo.e_aud_fmt == AUD_FMT_MP3) &&
                (!prSwdmxAviInfo->fgMp3Vbr) &&
                (prSwdmxAviInfo->u1CurATrackIdx < prSwdmxAviInfo->u1TotalATrack) &&
                (!prAudIdxTbl->b_is_VBR))
            {
                if (!_SWDMX_AviValidHdrBps(prAudIdxTbl->ui4_avg_bytes_per_sec, rADecInfo.ui4_data_rate/8))
                {
                    prSwdmxAviInfo->fgMp3Abr = TRUE;
                    LOG(5, "SWDMX_AVI Mp3 Abr!!!!!\n");
                }
            }
        }

        LOG(5, "SWDMX_AviAudioCb Audio is ready\n");
        if (prSwdmxAviInfo->fgEnVideo &&
            prSwdmxAviInfo->prDmxRangeInfo->ui8_vid_duration >prSwdmxAviInfo->u4TriggerAudPts)
        {
#if 1
            if ((prSwdmxAviInfo->fgBadAudio) &&
                (rADecInfo.e_aud_fmt == AUD_FMT_MP3))
            {
                UINT32 u4DspPts;
                UINT32 u4PrsPos;

                LOG(0, "Bad Audio, Lookup audio pts from audio driver\n");
                AUD_GetDspPtsInfo(prSwdmxInst->u1AdecId, &u4DspPts, &u4PrsPos);
                prSwdmxAviInfo->u4TriggerAudPts = u4DspPts;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4DspPts);
                prSwdmxAviInfo->fgBadAudio = FALSE;
            }
#endif            

            LOG(5, "Triggerred by video\n");
            VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, prSwdmxAviInfo->u4TriggerAudPts);
            LOG(5, "SWDMX_AviAudioCb VDP Trigger PTS = 0x%08x\n", 
                prSwdmxAviInfo->u4TriggerAudPts);
            if(prSwdmxAviInfo->fgVDecIsPause)
            {
                _SWDMX_AviVideoWakeUp(prSwdmxInst);
                prSwdmxAviInfo->fgVDecIsPause = FALSE;
            }

            // mp3 abr case, reset sync mode to none.(add by cong, Sep 7th/2012)
            if (prSwdmxAviInfo->fgMp3Abr)
            {
                STC_SetSyncMode(prSwdmxInst->u1StcId,
                    prSwdmxInst->u1AdecId,
                    prSwdmxInst->u1VdecId, AV_SYNC_MODE_NONE);
                AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                STC_StartStc(prSwdmxInst->u1StcId);
                LOG(5, "mp3 abr case, reset sync mode to none!\n");
            }
        }
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT  // this is copied from the above if block with some modification
        else if (prSwdmxInst->fgIsBadInterleaved)
        {
#if 1
            if ((prSwdmxAviInfo->fgBadAudio) &&
                (rADecInfo.e_aud_fmt == AUD_FMT_MP3))
            {
                UINT32 u4DspPts;
                UINT32 u4PrsPos;

                LOG(5, "Bad Audio, Lookup audio pts from audio driver\n");
                AUD_GetDspPtsInfo(prSwdmxInst->u1AdecId, &u4DspPts, &u4PrsPos);
                prSwdmxAviInfo->u4TriggerAudPts = u4DspPts;
                AUD_SetStartPts(prSwdmxInst->u1AdecId, u4DspPts);
                prSwdmxAviInfo->fgBadAudio = FALSE;
            }
#endif            

            LOG(5, "Triggerred by video\n");
            VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, prSwdmxAviInfo->u4TriggerAudPts);
            LOG(5, "SWDMX_AviAudioCb VDP Trigger PTS = 0x%08x\n", 
                prSwdmxAviInfo->u4TriggerAudPts);

            _fgBiAviAudioReady = TRUE;  // instead of original "_SWDMX_AviVideoWakeUp" if block

            // mp3 abr case, reset sync mode to none.(add by cong, Sep 7th/2012)
            if (prSwdmxAviInfo->fgMp3Abr)
            {
                STC_SetSyncMode(prSwdmxInst->u1StcId,
                    prSwdmxInst->u1AdecId,
                    prSwdmxInst->u1VdecId, AV_SYNC_MODE_NONE);
                AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                STC_StartStc(prSwdmxInst->u1StcId);
                LOG(5, "mp3 abr case, reset sync mode to none!\n");
            }
        }
#endif
        else
        {
            // Audio Only case
            LOG(5, "Trigger audio by _SWDMX_AviAudioDrvCb\n");
            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
            STC_StartStc(prSwdmxInst->u1StcId);
        }
        prSwdmxAviInfo->fgAudIsReady = TRUE;
        prSwdmxAviInfo->fgAudIsLoaded = TRUE;
        prSwdmxAviInfo->fgGetMp3Status = TRUE;
    }
    else if (eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_NO_DATA)
    {
        LOG(5, "_SWDMX_AviAudioDrvCb: no data\n");
    }
    else
    {
        // impossible
        LOG(1, "_SWDMX_AviAudioDrvCb eAudioNotifyType != ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE\n");
    }

    return TRUE;
}

static void _SWDMX_AviVideoWakeUp(SWDMX_INFO_T *prSwdmxInst)
{
    SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
    prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
    LOG(5, "_SWDMX_AviVideoWakeUp\n");
    VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);
    switch(prSwdmxAviInfo->u4VDecType)
    {
        case ENUM_SWDMX_VDEC_VC1:
        case ENUM_SWDMX_VDEC_WMV7:
        case ENUM_SWDMX_VDEC_WMV8:
        case ENUM_SWDMX_VDEC_WMV9:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_WMV);
            break;
        case ENUM_SWDMX_VDEC_DX311:
        case ENUM_SWDMX_VDEC_MPEG4:
        case ENUM_SWDMX_VDEC_H263:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MP4);
            break;
        case ENUM_SWDMX_VDEC_MJPEG:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MJPEG);
             if(prSwdmxAviInfo->fgIs3DFile)
            {
               VDEC_Play(prSwdmxInst->u1Vdec2Id, VDEC_FMT_MJPEG);
            }
            break;
        case ENUM_SWDMX_VDEC_MPEG1_MPEG2:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MPV);
            break;
        case ENUM_SWDMX_VDEC_H264:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H264);
            break;
        case ENUM_SWDMX_VDEC_H265:
            VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H265);
            break;
        default:
            LOG(1, "_SWDMX_AviVideoWakeUp Unknown Video Type\n");
            break;
    }
}

static SWDMX_HDR_PRS_STATE_T  _SWDMX_AviBeforeDeliverUseIdx(
    SWDMX_INFO_T *prSwdmxInst,
    UINT8 u1HdrType)
{
       SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
       
       DMX_MM_DATA_T rDmxMMData = {0};
       prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
    
       
       x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));

       if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &prSwdmxInst->rFeederInfo) != 
        FEEDER_E_OK)
        {
              LOG(7, "Feeder input buffer error.\n");
             return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }

        x_memcpy((VOID*)VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr), 
            (VOID*)VIRTUAL(prSwdmxAviInfo->u4VidHeaderPtr), 
            prSwdmxAviInfo->u4VidHdrLength);

        if(u1HdrType == eSWDMX_MEDIA_VIDEO)
        {
             if(prSwdmxAviInfo->fgIs3DFile)
             {
                 rDmxMMData.u1Idx = prSwdmxAviInfo->u1Vid2Pid;
             }
             else
             {
                 rDmxMMData.u1Idx = prSwdmxAviInfo->u1VidPid;
             }
        }
        else
        {
            rDmxMMData.u1Idx = prSwdmxAviInfo->u1AudPid;
        }
        rDmxMMData.u4BufStart = VIRTUAL(prSwdmxInst->rFeederInfo.u4StartAddr);
        rDmxMMData.u4BufEnd = VIRTUAL(prSwdmxInst->rFeederInfo.u4EndAddr);
        rDmxMMData.u4StartAddr = VIRTUAL(prSwdmxInst->rFeederInfo.u4WriteAddr);
        rDmxMMData.u4FrameSize = prSwdmxAviInfo->u4VidHdrLength;
        //if (DMX_MM_MoveData(u1HdrType, &rDmxMMData, 500) != TRUE)
        if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData)  != TRUE)
        {
            LOG(3, "Demuxer fails to move sequence header.\n");
            return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
        }
 
        prSwdmxAviInfo->u4NumSentVid++;
       return eSWDMX_HDR_PRS_FED_MOVE_FAIL;

}

static VOID AviMoniterVdecSupport(UCHAR u1SrcId, BOOL fgNotSupport)
{
     if (u1SrcId < SWDMX_SOURCE_MAX) 
     {
         g_fgNotSupport[u1SrcId] = fgNotSupport;
     }
}
static SWDMX_HDR_PRS_STATE_T AviHandAAC(SWDMX_INFO_T *prSwdmxInst, UINT32 u4ChunkSize, UINT32 u4Pts, UINT32 u4BufStartAddr, UINT8 u1TrackIdx)
{
  DMX_MM_DATA_T rTmpDmxMMData;
  UINT8 u1SampleRateIdx = 3;
  SWDMX_AVI_INFO_T *prSwdmxAviInfo = NULL;
  UCHAR *pucBuf = NULL;
  
  prSwdmxAviInfo = &prSwdmxInst->rFmtInfo.rSwdmxAviInfo;
  
  pucBuf = (UCHAR *)u4BufStartAddr;
  if((pucBuf[0] == 0xFF) && ((pucBuf[1] & 0xF0) == 0xF0))
  {
      return eSWDMX_HDR_PRS_SUCCEED;
  }
   
  x_memset(&rTmpDmxMMData, 0, sizeof(DMX_MM_DATA_T));
  
  if(prSwdmxInst->pucAudBuf == 0)
  {
      LOG(1, "pucAudBuf should be allocated in init function");
      prSwdmxInst->pucAudBuf = 
          (UCHAR*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(SWDMX_AVI_AAC_ALLOC_SIZE,16));                           
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
      switch (prSwdmxAviInfo->u4AudSampleRate[u1TrackIdx])
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
               ((prSwdmxAviInfo->u4AudChannelNs[u1TrackIdx] >> 2) & 0x1);
      /* 2b: channel_configuration */
      /* 1b: original */
      /* 1b: home */
      /* 1b: copyright_id */
      /* 1b: copyright_id_start */
      /* 2b: aac_frame_length */
      prSwdmxInst->pucAudBuf[3] = ((prSwdmxAviInfo->u4AudChannelNs[u1TrackIdx] & 0x3) << 6) |
               (((u4ChunkSize + SWDMX_AVI_AAC_FAKE_HDR_SIZE) >> 11) & 0x3);
      /* 8b: aac_frame_length */
      prSwdmxInst->pucAudBuf[4] = (((u4ChunkSize + SWDMX_AVI_AAC_FAKE_HDR_SIZE) >> 3) & 0xFF);
      /* 3b: aac_frame_length */
      /* 5b: adts_buffer_fullness */
      prSwdmxInst->pucAudBuf[5] = (((u4ChunkSize + SWDMX_AVI_AAC_FAKE_HDR_SIZE) << 5) & 0xE0) |
               ((0x7FF >> 6) & 0x1F);
      /* 6b: adts_buffer_fullness */
      /* 2b: number_of_raw_data_blocks_in_frame */
      prSwdmxInst->pucAudBuf[6] = ((0x7FF << 2) & 0xFC);
      rTmpDmxMMData.u4BufStart = (UINT32)prSwdmxInst->pucAudBuf;
      rTmpDmxMMData.u4BufEnd = rTmpDmxMMData.u4BufStart + SWDMX_AVI_AAC_ALLOC_SIZE;
      rTmpDmxMMData.u4StartAddr = rTmpDmxMMData.u4BufStart;
      rTmpDmxMMData.u4FrameSize = 7;
      rTmpDmxMMData.u4Pts = u4Pts;
      rTmpDmxMMData.u1Idx = prSwdmxAviInfo->u1AudPreBufPid[u1TrackIdx];
          
      if (_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rTmpDmxMMData)  != TRUE)
      {
          LOG(0, "Demuxer fails to move data.\n");
          return eSWDMX_HDR_PRS_DMX_MOVE_FAIL;
      } 
  }
  return eSWDMX_HDR_PRS_SUCCEED;
}

static UINT32 _SWDMX_AVIPrsVC1StartCode(UINT32 u4FrameAddr,UINT32 u4FifoStart,UINT32 u4FifoEnd)
{
    UINT32 u4StartCode = 0;

    UINT8 u1Byte1,u1Byte2,u1Byte3,u1Byte4;
    x_memcpy(&u1Byte1,(VOID*)VIRTUAL(u4FrameAddr),1);
    x_memcpy(&u1Byte2,(VOID*)VIRTUAL((u4FrameAddr+1)%u4FifoEnd),1);
    x_memcpy(&u1Byte3,(VOID*)VIRTUAL((u4FrameAddr+2)%u4FifoEnd),1);
    x_memcpy(&u1Byte4,(VOID*)VIRTUAL((u4FrameAddr+3)%u4FifoEnd),1);
    u4StartCode = u1Byte1<<24 |u1Byte2<<16 |u1Byte3<<8 |u1Byte4;
    return u4StartCode;
}
