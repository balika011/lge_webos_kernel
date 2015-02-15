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
 * $RCSfile: swdmx_wmv.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_wmv.c
 *  Software demux for wmv - internal interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_if.h"
#include "vdec_drvif.h"
#include "feeder_if.h"
#include "dmx_mm_if.h"
#include "swdmx_drvif.h"
#include "swdmx_wmv.h"
#include "swdmx_debug.h"
#include "x_util.h"
#include "u_midxbuld.h"
#include "aud_drvif.h"

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
#include "tz_if.h"
#endif


EXTERN void VDEC_SetWptr(UINT8 u1VdecId, UINT32 u4Wptr);
//EXTERN INT32 AUD_GetAudFifo(UINT32 * pu4Fifo1Start, UINT32 * pu4Fifo1SEnd,UINT32 * pu4Fifo2Start,UINT32 * pu4Fifo2End);

EXTERN void AUD_DrvSetWMAErrFunc(UINT8 u1DecId, AUD_WMAErr_NOTIFY pfClipNotify, VOID *prInst);  // megaa 20090828

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define WMV_DATA_OBJ_HEADER (50)
#define WMV_NETFLIX_HEADER_SIZE (4)
#define WMV_MAX_FEEDER_RETRY (10)
#define RETRY_TIMES (3)
#define TRICK_MODE_RETRY_TIMES (1000)
#define FEEDER_OFFSET (64*1024)
#define WMV_KEY_TABLE_THRESHOLD (3)
#define MORE_DEMUX_PACKET_COUNT 11  // ToDo: adjust according to real picture size

//since there's no key index table, we cannot get key_entry size
#define TIMEBASED_TRICK_REQ_PACKET_COUNT 16

#define WMV_VFIFO_SIZE (10*1024*1024)
#define WMV_DMX_FIFO_ALIGNMENT (64)
#define SWMDX_WMV_FEEDER_DATA_Q_SIZE (255)
#define NETFLIX_UNDERFLOW_THRESHOLD (15)
#define NETFLIX_CMMP_BUFFER_SIZE (15*1024)
#define NETFLIX_CMMP_BUFFER_SIZE_EXTRA (20*1024)
#define WMV_INVALID_STREAM_NUM_THREAHOLD (1000)
#define WMV_SC_FRAME        0x10D
#define WMV_SC_ENTRY        0x10E
#define WMV_SC_SEQ          0x10F
#define WMV_SC_FIELD        0x10C

#ifdef __KERNEL__
#define FM_BLOCK_SIZE (2048)  // tmp solution for DTV00213176, DTV00213223, DTV00213232(?)
#else
#define FM_BLOCK_SIZE (512)
#endif

//option define
#define WMV_FAST_I_FRAME_PARSING
#define WMV_MULTI_REQUEST // always turn on
#define WMV_ERROR_HANDLE_ON
#define WAIT_AUDIO_READY
#define WMV_SUPPORT_AUDIO_SWITCH
//#define WMV_ALLOCATE_VFIFO
//#define DISABLE_AUDIO_IN_BAD_INTERLEAVE
//#define ENABLE_WMV_DRM
#define WMA_TRICK_MODE
#define WMV_SEEK_OFFSET
//#define WMA_SEEK_OFFSET
#ifndef CC_DISABLE_PREBUFFER
#define WMV_AUDIO_USE_PRE_BUFFER
#endif

#ifdef WMV_AUDIO_USE_PRE_BUFFER
// mw will pass out-of-bound audio stream id to swdmx.
// swdmx will use it as index of array AudioPreBufferPid[8]
// if the audio stream id >7, something will go wrong.
// hence, swdmx will id%8, then pass to array
// BTW, mw will ensure the number(of audio stream id) <= 8
#define WMV_CHECK_AUDIO_STREAM_ID
#endif

#ifdef CC_3D_MM_DS_SUPPORT  // tmp solution
//#define WMV_BAD_INTLV_SUPPORT  // bad interleave support (defined in cfg)
#endif
#ifdef WMV_BAD_INTLV_SUPPORT
//#define WMV_BAD_INTLV_DEBUG
#endif

#ifdef WMA_TRICK_MODE
#define WMA_SENDDATA_TIMELEN (2*90000)
#define WMA_MAXSTEND_TIME (6)
#endif

#ifdef WMA_SEEK_OFFSET
#define WMA_SEEK_PACKET_NUM (6)
#define WMA_SEEK_MINCHECK_NUM (3)
#define WMA_MAX_SEEKTIME (4)
#endif

#define WMV_NETFLIX_PRE_BUFFER_SIZE  (2*1024*1024)  // DTV00086562: Netflix requires 2MB pre-buffer size anyway



#define INPUT_SRC  (_prRangeList->prDmxRangeInfo->e_mm_src_type)

#ifdef CC_NETFLIX_CMPB_SUPPORT
static UINT32 _u4RemainPacketDataSize = 0;
static UINT8* _pu1RemainPacketData = NULL;
#endif

#ifdef WMV_BAD_INTLV_SUPPORT
BOOL _fgEnBadIntlv = 1;
UINT32 _u4WmvBadIntlvThresA = 2100;
UINT32 _u4WmvBadIntlvThresV = 1800;
#endif

#ifdef WMV_BAD_INTLV_DEBUG
UINT32 _u4AFifoAccuLen = 0;
UINT32 _u4VFifoAccuLen = 0;
#endif


#if 1  // multi-instance?

#ifdef CC_DLNA_SUPPORT
#define FEEDER_SEGMENT       min(282*1024, min(FEEDER_AUDIO_BUF_SIZE, FEEDER_DEFAULT_BUF_SIZE) >> 3)
#define WMV_PRE_BUFFER_SIZE  min(2*1024*1024, min(FEEDER_AUDIO_BUF_SIZE, FEEDER_DEFAULT_BUF_SIZE) * 5 / 10)
#else
#ifdef CC_DMX_PURE_AUDIO
#define FEEDER_SEGMENT       min(240*1024, min(FEEDER_AUDIO_BUF_SIZE, FEEDER_DEFAULT_BUF_SIZE) >> 3)
#define WMV_PRE_BUFFER_SIZE  min(250*1024, min(FEEDER_AUDIO_BUF_SIZE, FEEDER_DEFAULT_BUF_SIZE) * 5 / 10)
#else
#define FEEDER_SEGMENT       min(282*1024, min(FEEDER_AUDIO_BUF_SIZE, FEEDER_DEFAULT_BUF_SIZE) >> 3)
#define WMV_PRE_BUFFER_SIZE  min(2*1024*1024, min(FEEDER_AUDIO_BUF_SIZE, FEEDER_DEFAULT_BUF_SIZE) * 5 / 10)
#endif
#endif
#define WMA_REQUEST_SIZE (192*1024)
#define WMV_REQUEST_SIZE (512*1024)

/* 1. ensure this size is smaller than feeder buffer size(2MB)
 * 2. ensure this size is enough to contain one IFrame(2MB/3)
 */
#define TIMEBASED_TRICKMODE_WMV_REQ_SIZE  (682*1024)

/* enlarge this value if video unsmooth */
#define TIMEBASED_NORMALMODE_WMV_REQ_SIZE (512*1024)

#else  // multi-instance?

#ifdef CC_DLNA_SUPPORT
//#define WMV_ALLOCATE_VFIFO
#define FEEDER_SEGMENT (282*1024)
#define WMV_PRE_BUFFER_SIZE (2*1024*1024)
//#define ENABLE_FEEDER_THRESHOLD //something wrong....
#else
#ifdef CC_DMX_PURE_AUDIO
// feeder buffer only have 512*1024
// wmv will request two feeder request.
// So wmv can't use 288*1024 or feeder will full and drop that request.
#define WMV_PRE_BUFFER_SIZE (250*1024)
#define FEEDER_SEGMENT (240*1024)
#else
// 282*1024 = 288768
#define WMV_PRE_BUFFER_SIZE (2*1024*1024)
#define FEEDER_SEGMENT (282*1024)
#endif
#endif

#endif  // multi-instance?

//#ifdef CC_DLNA_SUPPORT
//#define WMV_2X_TO_1X_PREBUFFER
//#endif


#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
  #ifdef FEEDER_SEGMENT
  #undef FEEDER_SEGMENT
  #endif

  #define FEEDER_SEGMENT  (512*1024)  // to ensure the data input performance under Linux!
#endif


#define NETFLIX_MUX (0)
#define NETFLIX_AUDIO (1)
#define NETFLIX_VIDEO (2)

#ifdef WMV_AUDIO_USE_PRE_BUFFER
// mw will pass out-of-bound audio stream id to swdmx.
// swdmx will use it as index of array AudioPreBufferPid[8]
// if the audio stream id >7, something will go wrong.
// hence, swdmx will id%8, then pass to array
// BTW, mw will ensure the number(of audio stream id) <= 8
#define WMV_CHECK_AUDIO_STREAM_ID
#endif

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
//for wmv parsing
#undef TypeTransform
#define TypeTransform(x) (SIZE_T)((x&0x2)&&(x&0x1)?4:x)
#undef min
#define min(x, y) ((x < y) ? x : y)
#undef max
#define max(x, y) ((x > y) ? x : y)
#undef abs
#define abs(x) (((x) >= 0) ? (x) : -(x))
#define IDR_PIC 5
#define IS_IDR_NALU(x) (((x & 0x1f) == IDR_PIC))

#define _rSwdmxWmvInfo  (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo)  // multi-instance

#define PTS_S(pts) (pts/90000)
#define PTS_MS(pts) ((pts%90000)/90)


#ifdef WMV_CHECK_AUDIO_STREAM_ID
#define ASID(x) (((UINT8)(x)==0xFF)?(x):(x%WMV_MAX_AUDIO_TRACK))
#else
#define ASID(x) (x)
#endif


#define SWDMX_WMV_VERIFY_NULL(ptr)                                                          \
          do {                                                                              \
            if (ptr == NULL) {                                                              \
                LOG(0, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                return; }                                                                   \
          } while (0)

#define SWDMX_WMV_VERIFY_NULL_ERROR(ptr)                                                    \
          do {                                                                              \
            if (ptr == NULL) {                                                              \
                LOG(0, "SWDMX Instance is NULL %s,%d, oops....\n", __FUNCTION__, __LINE__); \
                return FALSE; }                                                             \
          } while (0)

#ifdef SUPPORT_3D_MEDIA
#define ENABLE_NEWTRACK(prSwdmxInst) (prSwdmxInst->eSubVideoType==ENUM_SWDMX_VDEC_MULTITRACK&&(prSwdmxInst->u1VdecId!=prSwdmxInst->u1Vdec2Id))
#define IS_NEWTRACK(prSwdmxInst,u1Id) (ENABLE_NEWTRACK(prSwdmxInst)&&(u1Id==prSwdmxInst->u1Vdec2Id))
#define IS_NEWTRACKDATA(prSwdmxInst,u1StreamId) (prSwdmxInst->eSubVideoType==ENUM_SWDMX_VDEC_MULTITRACK&&u1StreamId==prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4VSubStreamId)
#define IS_SIMP_IDX(p) ((p) && (p)->ui4_size > 3)
#else
#define ENABLE_NEWTRACK(prSwdmxInst) (0)
#define IS_NEWTRACK(prSwdmxInst,u1Id)(0)
#define IS_NEWTRACKDATA(prSwdmxInst,u1StreamId) (0)
#define IS_SIMP_IDX(p) (1)
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#if 1  // multi-instance

#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
UINT32 _u4BIVideoDelay = 500;
UINT32 _u4BIVideoDelay2 = 5;
UINT32 _u4BIAThreshold = 60;
UINT32 _u4BIAThreshold2 = 25;
//UINT32 _u4BIAudioDelay = 0;

UINT32 _u4CommVFifoSize = 0;
UINT32 _u4CommAFifoSize = 0;
UINT8 _u1CommAudDmxId = 0;
UINT8 _u1CommAudPid = 0;
HANDLE_T _hCommBIQ = 0;
#endif

#define _prRangeList             (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.prRangeList)
#define _prRangeInfo             (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.prRangeInfo)

#define _rFeederCond             (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rFeederCond)
#define _rFeederReqData          (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rFeederReqData)

#define _u1AudioStreamId         (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudioStreamId)
#define _rFeederInfo             (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rFeederInfo)

#if 1  // because of complicated build error  ToDo: fix it and use instance variable
static VDEC_PES_INFO_T _rPreviousPesInfo[SWDMX_SOURCE_MAX] = {{{0}}};
static DMX_AUDIO_PES_T _rPreviousAudioPes = {0};
static BOOL _fgReceiveAudioPes = FALSE;
#else
#define _rPreviousPesInfo        (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rPreviousPesInfo)
#define _rPreviousAudioPes       (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rPreviousAudioPes)
#define _fgReceiveAudioPes       (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.fgReceiveAudioPes)
#endif

#define _fgReceiveVideoPes       (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.fgReceiveVideoPes)
//#define _u4DataWithStartCode     (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4DataWithStartCode)
#define _pu1VideoFrameData       (prSwdmxInst->pu1VideoFrameData)  //(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.pu1VideoFrameData)
#define _u4PrevFramePTS          (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4PrevFramePTS)
#define _u4DefalutReqSize        (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4DefalutReqSize)

//for netflix application
#define _prWmvGenericInfo        (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.prWmvGenericInfo)
#define _u4NetflixVideoStreamId  (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4NetflixVideoStreamId)
#define _u4NetflixAudioStreamId  (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4NetflixAudioStreamId)
#define _fgNetflixEos            (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.fgNetflixEos)
#define _fgDataExhausted         (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.fgDataExhausted)
#define _eCurFMT                 (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.eCurFMT)
#define _u4NumberOfBitstream     (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4NumberOfBitstream)
//for netflix application

#define rPayLoadInfo             (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rPayLoadInfo)
#define rMultiPayloadInfo        (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rMultiPayloadInfo)
#define rPayLoadHeader           (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rPayLoadHeader)


#define _u4VideoFrameDataAddr        (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4VideoFrameAddr)
static UINT64 _u8SampleId = 0;

#define WMV_CHECK_STARTCODE_BEFORE_MOVEDATA
//#define WMV_DEBUG_SHOW_INDEX_TABLE_ENTRY
//#define TEST_WMV_DRM

#ifdef TEST_WMV_DRM

//"bear catch fish"  video
//clear key
//static UINT8 _au1ContentKey[16] = {0x9d, 0x1c, 0xbc, 0x29, 0xc3, 0xc6, 0xf1, 0x03,
//                                   0x49, 0x70, 0x69, 0x9a, 0x50, 0xb5, 0x56, 0x6c};

//after protect key,please use this key to test driver
static UINT8 _au1ContentKey[16] = {0xb1, 0x38, 0x22, 0xf5, 0x14, 0xfb, 0x17, 0x05, 
                                   0x77, 0xb5, 0x2a, 0xe2, 0x28, 0x41, 0x06, 0xe1};

//static UINT8 _au1ContentKey[7] = {0x9D, 0xFD, 0x13, 0x72, 0x37, 0xA9, 0x1A};
//static UINT8 _au1ContentKey[16] = {0xC9, 0x3E, 0x86, 0x8C, 0x65, 0x6C, 0xA7, 0x26,
//                                   0x52, 0xA1, 0x9B, 0xE7, 0x03, 0x9C, 0x63, 0x64};

#endif

#ifdef WMV_FAST_I_FRAME_PARSING
#define _u8PreKeyFrameAddr     (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u8PreKeyFrameAddr)
#endif


//#ifdef WMV_MULTI_REQUEST
#define _hWMVFeederDataQue     (prSwdmxInst->hWMVFeederDataQue) //(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.hWMVFeederDataQue)
#define _u4RequestDataSize     (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4RequestDataSize)
//#endif

#define _u4VFifoStartAddr      (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4VFifoStartAddr)
#define _u4VFifoSize           (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4VFifoSize)
#define _u4AFifoStartAddr      (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4AFifoStartAddr)
#define _u4AFifoSize           (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4AFifoSize)

//static UINT32 _u4WmvStpRsmThres = 1;  // for stop resume

#ifdef WMV_2X_TO_1X_PREBUFFER
static INT32 _i4WmvStcAdj = -1500;
#endif

#define _u4WmaBytesPerSec      (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4WmaBytesPerSec)
#define _u4AudSampleRate      (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4AudSampleRate)
#define _u4AudChannelNs      (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4AudChannelNs)
#define _rBadIntlv             (prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.rBadIntlv)

#define AAC_FAKE_HDR_SIZE 7
#define AAC_ALLOC_SIZE   (256)

#define PKTBYTE(x) (*((UINT8*)prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4PacketStartAddr + (x)))
#else  // single-instance

static SWDMX_RANGE_LIST_T *_prRangeList = NULL;
static MM_RANGE_INFO_T* _prRangeInfo = NULL;

static FEEDER_TRANSMIT_COND _rFeederCond;
static FEEDER_REQ_DATA_T _rFeederReqData;

static UINT8 _u1AudioStreamId = 0xFF;
static FEEDER_BUF_INFO_T _rFeederInfo;

static SWDMX_WMV_INFO_T _rSwdmxWmvInfo;
static VDEC_PES_INFO_T _rPreviousPesInfo;
static DMX_AUDIO_PES_T _rPreviousAudioPes;
static BOOL _fgReceiveAudioPes;
static BOOL _fgReceiveVideoPes;
//static UINT32 _u4DataWithStartCode[10*1024];
static UINT8* _pu1VideoFrameData = NULL;
static UINT32 _u4PrevFramePTS = 0;
static UINT32 _u4DefalutReqSize = 0;

//for netflix application
static MINFO_TYPE_GENERIC_T* _prWmvGenericInfo = NULL;
static UINT32 _u4NetflixVideoStreamId;
static UINT32 _u4NetflixAudioStreamId;
static BOOL _fgNetflixEos = FALSE;
static BOOL _fgDataExhausted = TRUE;
static ENUM_SWDMX_FMT_T _eCurFMT;
static UINT32 _u4NumberOfBitstream = 1;
//for netflix application

static PayloadParseInfo rPayLoadInfo;
static MultiPayloadInfo rMultiPayloadInfo;
static PayloadHeader rPayLoadHeader;


#ifdef ENABLE_WMV_DRM
static UINT8 _u1VideoFrameData[100*1024];
static UINT64 _u8SampleId = 0;

//1.wmv drm file
static UINT8 _au1ContentKey[16] = {0xfd, 0x2b, 0xb7, 0x8c, 0x88, 0xb8 ,0xbe ,0x2f,0x75,
                                   0x15, 0xe9, 0xf6, 0x2f, 0x1a, 0xa4 ,0x3a
                                  };


//2.wmv drm file
/*static UINT8 _au1ContentKey[16] = {0xd3, 0x16, 0x1f, 0xea, 0x9e, 0x12 ,0xaf ,0x1,0x12,
                                   0x19, 0xd1, 0x29, 0x9, 0x8a, 0xe3 ,0xc5
                                  };
*/

#endif  // ENABLE_WMV_DRM

#ifdef WMV_FAST_I_FRAME_PARSING
static UINT64 _u8PreKeyFrameAddr =0;
#endif


//#ifdef WMV_MULTI_REQUEST
static HANDLE_T _hWMVFeederDataQue = 0;
static UINT32 _u4RequestDataSize = 0;
static HAL_TIME_T _rFeederReqTime;
static HAL_TIME_T _rStartTime,_rEndTime;
//#endif

#ifdef WMV_ALLOCATE_VFIFO
static UINT32 _u4VFifoStartAddr = NULL;
#endif

//static UINT32 _u4WmvStpRsmThres = 1;  // for stop resume

#ifdef WMV_2X_TO_1X_PREBUFFER
static INT32 _i4WmvStcAdj = -1500;
#endif

#endif  // multi/single-instance


#ifdef __MODEL_slt__
static UINT8 _au1VC1SltSeqHdr[] =
{
    0x21, 0x00, 0x00, 0x01, 0x0F, 0xD3, 0xEE, 0x27,
    0xF1, 0x67, 0x0A, 0x27, 0xF8, 0x59, 0xE2, 0xE9,
    0x40, 0x00, 0x40, 0x01, 0xC0, 0x00, 0x80, 0x01,
    0xA0, 0x00, 0xC0, 0x01, 0x80, 0x01, 0x00, 0x01,
    0x60, 0x01, 0x40, 0x01, 0x40, 0x01, 0x80, 0x01,
    0x20, 0x01, 0xC0, 0x01, 0x00, 0x02, 0x00, 0x00,
    0xE0, 0x02, 0x40, 0x00, 0xC0, 0x02, 0x80, 0x00,
    0xA0, 0x02, 0xC0, 0x00, 0x80, 0x03, 0x00, 0x00,
    0x60, 0x03, 0x40, 0x00, 0x40, 0x03, 0x80, 0x00,
    0x28, 0x00, 0x00, 0x01, 0x0E, 0x16, 0x93, 0xBB,
    0xD3, 0xEB, 0xF4, 0x04, 0x14, 0x24, 0x3C, 0x44,
    0x54, 0x5C, 0x64, 0x7C, 0x94, 0x9F, 0xC5, 0x9E,
    0x40
};

UINT8 _u1VC1SltLastPicType = 0;
UINT32 _u4VC1SltLastPicPts = 0;
#endif

/* DLNA: in time-based mode, seek to 0 might return file header(from byte 0).
 * If it is so, swdmx should pass correct data to driver by "u8FileOffset".
 */
/* ASF object header(GUID) */
static UINT8 _au1AsfObjectHeader[16] = {0x30, 0x26, 0xB2, 0x75,
                                        0x8E, 0x66, 0xCF, 0x11,
                                        0xA6, 0xD9, 0x00, 0xAA, 
                                        0x00, 0x62, 0xCE, 0x6C};

UINT32 u4AudDspPts[SWDMX_SOURCE_MAX]={1};
static BOOL fgTrickTrigger[SWDMX_SOURCE_MAX]={FALSE};
//static UINT32 u4StartIndex=0;
#ifdef CC_AUD_SUPPORT_DUAL_DSP
extern AUD_DRV_STATE_T AUD_DRVGetAudioState(UINT8 u1DspId, UINT8 u1DecId);
#else
extern AUD_DRV_STATE_T AUD_DRVGetAudioState(UINT8 u1DecId);
#endif
BOOL _SWDMX_WMVRequestFeederData(SWDMX_INFO_T *prSwdmxInst, UINT32 u4QrySize,UINT64 u8Offset,BOOL fgTrickMode);
#ifdef CC_3D_MM_DS_SUPPORT
static VOID _WMVChkResume1XPos(SWDMX_INFO_T* prSwdmxInst);
#endif

static VOID _SWDMX_WMVConsumeFeeder(SWDMX_INFO_T *prSwdmxInst);
static VOID _SWDMX_WMVHandleEos(SWDMX_INFO_T* prSwdmxInst, 
                                VDEC_ES_INFO_T *prVdecEsInfo, 
                                SWDMX_RANGE_LIST_T* prRangeList,
                                UINT32 i4PlaySpeed);
static VOID _SWDMX_WMVInitVDmx(UINT8 u1SrcId);
static VOID _SWDMX_WMVFlushAVFifo(UINT8 u1SrcId, UINT64 u8SeekPTS);
static VOID _SWDMX_WMVTellVDecRenderPts(UINT8 u1SrcId, INT32 i4PlaySpeed, UINT64 u8SeekPTS);
static BOOL _WMVSearchAllEntriesByPts(SWDMX_INFO_T *prSwdmxInst, 
                                   UINT64 u8SeekPts,
                                   UINT64* pu8AbsOfst, /* output param */
                                   UINT32* pu4FoundPts, /* output param */
                                   UINT32* pu4FoundIdx /* output param */);
static BOOL _WMVSearchKeyIdxTblByPts(SWDMX_INFO_T *prSwdmxInst, 
                                     MM_RANGE_ELMT_IDXTBL_T* pt_tbl,
                                     UINT64 u8SeekPts,
                                     UINT32 u4Entries,
                                     UINT64* pu8AbsOfst, /* output param */
                                     UINT32* pu4FoundPts, /* output param */
                                     UINT32* pu4FoundIdx /* output param */);
static BOOL _WMVGetEntryInfoByIdx(SWDMX_INFO_T *prSwdmxInst, 
                                   UINT32 u4Index,
                                   UINT64* pu8EntryAbsOfst, /* output param */
                                   UINT32* pu4EntryPts, /* output param */
                                   UINT32* pu4EntrySize /* output param */);
static UINT64 _WMVCalcWMADecodingPos(SWDMX_INFO_T *prSwdmxInst);
BOOL _SWDMX_WMVDeliverDataTimebased(/*SWDMX_INFO_T *prSwdmxInst, */
                                    UINT8 u1SrcId,
                                    SWDMX_RANGE_LIST_T* prRangeList);
VOID _SWDMX_WMVSeekTimebased(UINT8 u1SrcId,
                             SWDMX_RANGE_LIST_T* prRangeList,
                             SWDMX_CUR_CMD_T* prCmd);
VOID _SWDMX_WMVHandleSpeedChangeTimebased(UINT8 u1SrcId,
                                          //UINT64 u8SeekTime,
                                          SWDMX_RANGE_LIST_T* prRangeList,
                                          SWDMX_CUR_CMD_T* prCmd);
BOOL _SWDMX_WMVRequestFeederDataTimebased(SWDMX_INFO_T *prSwdmxInst, 
                                          UINT32 u4QrySize, 
                                          UINT64 u8Offset, 
                                          BOOL fgTrickMode);
static VOID _SWDMX_WMVChangeAudioTrack(UINT8 u1SrcId);

static BOOL _SWDMX_WMVDRMInitDmx(SWDMX_INFO_T *prSwdmxInst);
static VOID _SWDMX_WMVSendVEOS(SWDMX_INFO_T *prSwdmxInst);
static VOID _SWDMX_WMVSendAEOS(SWDMX_INFO_T *prSwdmxInst);
#ifdef TEST_WMV_DRM
EXTERN BOOL TZ_NDDRM_ProtectAESKey (UINT8 * u4KeyBuf, UINT32 u4KeyLen);
#endif
EXTERN BOOL TZ_NDDRM_SetAESKey (UINT8 * u4KeyBuf, UINT32 u4KeyLen, UINT8 * u4IvBuf, UINT32 u4IvLen);
//#define WMV_CHECK_KEY_INDEX_TABLE_ENTRY
#ifdef WMV_CHECK_KEY_INDEX_TABLE_ENTRY
static BOOL _SWDMX_WMVIsKeyIndexTableValid(SWDMX_INFO_T *prSwdmxInst);
#endif

static BOOL _WMVGetVideoFifo(UINT8 u1SrcId, UINT32 *pu4Addr, UINT32 *pu4Size);
static BOOL _WMVGetVideo2Fifo(UINT8 u1SrcId, UINT32 *pu4Addr, UINT32 *pu4Size);

#define GET_ENTRY_INFO_BOOL(inst, idx, absoft, pts, sz) \
        do \
        { \
            absoft = 0; \
            pts = 0; \
            sz = 0; \
            if (!_WMVGetEntryInfoByIdx(inst, idx, &absoft, &pts, &sz)) \
            { \
                LOG(0, "get entry fail(index: #%d)\n", idx); \
                return FALSE; \
            } \
        } while (0)

#define GET_ENTRY_INFO_VOID(inst, idx, absoft, pts, sz) \
        do \
        { \
            absoft = 0; \
            pts = 0; \
            sz = 0; \
            if (!_WMVGetEntryInfoByIdx(inst, idx, &absoft, &pts, &sz)) \
            { \
                LOG(0, "get entry fail(index: #%d)\n", idx); \
                return; \
            } \
        } while (0)

static BOOL _WMVGetVideoFifo(UINT8 u1SrcId, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    return _SWDMX_GetVideoFifo(u1SrcId, pu4Addr, pu4Size);
}


static BOOL _WMVGetVideo2Fifo(UINT8 u1SrcId, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    return _SWDMX_GetVideo2Fifo(u1SrcId, pu4Addr, pu4Size);
}

static BOOL _WMVIsObjectHeader(UINT8 * pu1InputData)
{
    INT32 i = 0;

    for (i = 0; i < 16; i++)
    {
        if (*(pu1InputData + i) != _au1AsfObjectHeader[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

#if 1 //DTV00454391
static BOOL _WMVWaitVDec(SWDMX_INFO_T *prSwdmxInst, UINT32 u4Flag)
{
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
    if (u4Status == VDEC_ST_STOP || u4Status == VDEC_ST_IDLE)
    {
        LOG(0, "(%d)VDec no response! count: %d, vdec-status: %d\n", u4Flag, u4Cnt, u4Status);
        return FALSE;
    }
    else
    {
        LOG(0, "(%d)ASF-SWDMX wait VDec, count: %d, vdec-status: %d\n", u4Flag, u4Cnt, u4Status);
        return TRUE;
    }
}
#define _SWDMX_WMVWaitVDec(x) do {if (FALSE == _WMVWaitVDec(prSwdmxInst, x)) \
            { \
                return eSWDMX_HDR_PRS_FAIL; \
            } } while (0)
#else
#define _SWDMX_WMVWaitVDec(x)
#endif

static UINT32 _SWDMX_WMV_u4Round(UINT32 a, UINT32 b)
{
    UINT32 ret = 0;
    UINT32 remainder = 0;
    UINT32 factor = 0;

    ret = a / b;
    remainder = a % b;
    if (remainder > 0)
    {
        factor = (b+1) / 2;
        return (remainder >= factor) ? (ret + 1) : (ret);
    }
    return ret;
}

static VOID _SWDMX_WMVFlushFeederDataQ(SWDMX_INFO_T *prSwdmxInst)
{
    UINT16 u2Cnt;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    WMV_FEEDER_REQ_DATA_T rData;

    zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);
    while (x_msg_q_receive(&u2MsgQIdx, &rData, &zMsgSize,
                           &(_hWMVFeederDataQue), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
    }

    VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
    ASSERT(u2Cnt== 0);
}


#ifdef WMV_BAD_INTLV_SUPPORT
static BOOL _WMVBeginReparse(SWDMX_INFO_T* prSwdmxInst)
{
    if (_rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_V)
    {
        _rBadIntlv.eState = BAD_INTLV_STT_REPARSE_V;
    }
    else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_A)
    {
        _rBadIntlv.eState = BAD_INTLV_STT_REPARSE_A;
    }
    else
    {
        return FALSE;
    }
    
    //_SWDMX_FlushReqMsgQ(u1SrcId);  !!!
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.u4ConsumedSize = 0;
    _SWDMX_WMVConsumeFeeder(prSwdmxInst);
    
    _rSwdmxWmvInfo.u4CurParsingPacketNo = _rBadIntlv.u4BeginSkipPktNo;
    _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset 
                + WMV_DATA_OBJ_HEADER 
                + ((UINT64)_rSwdmxWmvInfo.u4CurParsingPacketNo)*_rSwdmxWmvInfo.u4PacketSize;    

    return TRUE;
}

static BOOL _WMVBeginRePatch(SWDMX_INFO_T* prSwdmxInst)
{
    if (_rBadIntlv.u4EndPatchPktNo == 0)  // !!!1027
        return FALSE;
    
    if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V)
    {
        _rBadIntlv.eState = BAD_INTLV_STT_PATCH_V;
    }
    else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A)
    {
        _rBadIntlv.eState = BAD_INTLV_STT_PATCH_A;
    }
    else
    {
        return FALSE;
    }
    
    //_SWDMX_FlushReqMsgQ(u1SrcId);  !!!
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.u4ConsumedSize = 0;
    _SWDMX_WMVConsumeFeeder(prSwdmxInst);
    
    _rSwdmxWmvInfo.u4CurParsingPacketNo = _rBadIntlv.u4EndPatchPktNo;
    _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset 
                + WMV_DATA_OBJ_HEADER 
                + ((UINT64)_rSwdmxWmvInfo.u4CurParsingPacketNo)*_rSwdmxWmvInfo.u4PacketSize;

    return TRUE;
}

static VOID _WMVEndReparse(SWDMX_INFO_T* prSwdmxInst)
{
    _rBadIntlv.eState = BAD_INTLV_STT_NONE;
    _rBadIntlv.u4BeginSkipPktNo = 0;
    _rBadIntlv.u4BeginSkipPldNo = 0;
    _rBadIntlv.u8BeginSkipPktOfst = 0;
    _rBadIntlv.u4EndPatchPktNo = 0;
    _rBadIntlv.u4EndPatchPldNo = 0;
}

static VOID _WMVResetBadIntlv(SWDMX_INFO_T *prSwdmxInst)
{
    _WMVEndReparse(prSwdmxInst);
    _rBadIntlv.fgAudioEnd = FALSE;
    _rBadIntlv.fgVideoEnd = FALSE;
}
#endif  // WMV_BAD_INTLV_SUPPORT


static VOID _WMVMsgQInit(UCHAR u1SrcId)
{
    char szMsgQName[] = { "WMVFQn" };
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    if(!prSwdmxInst)
    {
        LOG(0,"instance is NULL");
        return;
    }
    
    if (_hWMVFeederDataQue == 0)
    {
        szMsgQName[5] = '0' + u1SrcId;
        //if (_u4SwdmxWmvMsgQID >= 10)
        //    _u4SwdmxWmvMsgQID = 0;
        // create receive feeder data queue
        VERIFY(x_msg_q_create(&_hWMVFeederDataQue, szMsgQName, sizeof(WMV_FEEDER_REQ_DATA_T),
                              SWMDX_WMV_FEEDER_DATA_Q_SIZE) == OSR_OK);
    }
        
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
}


VOID _WMVAudioErrorCallback(VOID *prInst, UINT32 u4ErrorCode)  // megaa 20090828
{
    SWDMX_INFO_T *prSwdmxInst = (SWDMX_INFO_T *)prInst;
    _rSwdmxWmvInfo.u4WmaErrorCode = u4ErrorCode;
}


static VOID _WMVInternalInit(UCHAR u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    
    if(!prSwdmxInst)
    {
        LOG(0,"instance is NULL");
        return;
    }
    
    //set audio call back function
    AUD_DrvSetWMAErrFunc(prSwdmxInst->u1AdecId,_WMVAudioErrorCallback, (VOID *)prSwdmxInst);  // megaa 20090828

#if 1  // sometimes &_eCurFMT is not 4 byte aligned so that accessing it as UINT32* will cause data abort
    {
        UINT32 u4CurFMT;

        _SWDMX_GetInfo(u1SrcId, eSWDMX_CONTAINER_TYPE,(UINT32*)(&u4CurFMT),0,0);
        _eCurFMT = (ENUM_SWDMX_FMT_T)u4CurFMT;
    }
#else
    _SWDMX_GetInfo(eSWDMX_CONTAINER_TYPE,(UINT32*)(&_eCurFMT),0,0);
#endif

    //x_memset(&(_rSwdmxWmvInfo),0,sizeof(SWDMX_WMV_INFO_T));  this has been done in common part
    
    _rSwdmxWmvInfo.u1AudioStreamId = 0xFF;
    _rSwdmxWmvInfo.fgDataExhausted = TRUE;
    _rSwdmxWmvInfo.u4NumberOfBitstream = 1;
    _rSwdmxWmvInfo.fgDemuxFinish = FALSE;
    _rSwdmxWmvInfo.fgFirstFrame = TRUE;
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.fgBeginPlay = TRUE;
    _rSwdmxWmvInfo.u8SeekPTS = 0xFFFFFFFF;
    _rSwdmxWmvInfo.u8SeekPTS <<= 32;
    _rSwdmxWmvInfo.u8SeekPTS += 0xFFFFFFFF;
    //_rSwdmxWmvInfo.u4AStreamId = 0xff;
    //_rSwdmxWmvInfo.u4VStreamId = 0xff;
    _rSwdmxWmvInfo.fgBlock = FALSE;
    _rSwdmxWmvInfo.u4CurParsingPacketNo = 0;
    _fgNetflixEos = FALSE;
    _fgDataExhausted = TRUE;
    _u1AudioStreamId = 0xFF;
    _u4PrevFramePTS = 0;
    _u4DefalutReqSize = 0;
    _fgReceiveAudioPes = FALSE;

    LOG(0, "currently4 _fgReceiveVideoPes is %s, set to FLASE!\n", 
        _fgReceiveVideoPes?"TRUE":"FALSE");
    _fgReceiveVideoPes = FALSE;


    _rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;  // !!! megaa 20100321 ToDo: check if this affects stop resume

    _rSwdmxWmvInfo.fgInternalInited = TRUE;
    prSwdmxInst->u4Multiple = 3;

    _rSwdmxWmvInfo.u4AudSampleRate = 48000;
    _rSwdmxWmvInfo.u4AudChannelNs = 2;
    if (_rSwdmxWmvInfo.eAudioType == ENUM_SWDMX_ADEC_AAC &&
        prSwdmxInst->pucAudBuf == NULL)
    {
        #if 0
        UINT32 u4RequestSize = AAC_ALLOC_SIZE;
        UINT32 u4Align = 16;
       _SWDMX_GetInfo(u1SrcId, 
                      eSWDMX_GET_INTERNAL_BUF, 
                      (UINT32*)prSwdmxInst->pucAudBuf, 
                      &u4RequestSize, 
                      &u4Align);            

        #endif
        prSwdmxInst->pucAudBuf = (UINT8 *)BSP_AllocAlignedDmaMemory(AAC_ALLOC_SIZE, 16);
        if (prSwdmxInst->pucAudBuf == NULL)
        {
            LOG(1, "[WMV] Allocate AAC buffer failed!\n");
            return;
        }
    }   

    if (_pu1VideoFrameData == NULL)
    {
#if 1
        _pu1VideoFrameData = (UINT8 *)BSP_AllocAlignedDmaMemory(1*1024,16);
#else /* need mentor's double check, follow David's mail: "Please review swdmx internal buffer use" */
		{
			UINT32 u4Addr = 0;
			UINT32 u4Size = 1024;
			UINT32 u4Align = 16;
			SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align);
			_pu1VideoFrameData = (UCHAR*)u4Addr;
		}
#endif
        if (_pu1VideoFrameData == NULL)
        {
            ASSERT(0);
        }
    }
    
#ifdef CC_NETFLIX_CMPB_SUPPORT
    if(_pu1RemainPacketData == NULL)
    {
#if 1
        _pu1RemainPacketData = (UINT8 *)BSP_AllocAlignedDmaMemory(NETFLIX_CMMP_BUFFER_SIZE_EXTRA,16);
#else /* need mentor's double check, follow David's mail: "Please review swdmx internal buffer use" */
		{
			UINT32 u4Addr = 0;
			UINT32 u4Size = NETFLIX_CMMP_BUFFER_SIZE_EXTRA;
			UINT32 u4Align = 16;
			SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align);
			_pu1RemainPacketData = (UCHAR*)u4Addr;
		}
#endif
        if(_pu1RemainPacketData == NULL)
        {
            ASSERT(0);
        }
    }
#endif     
}


#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
static BOOL _WMV_AFifo_Fullness(SWDMX_INFO_T *prSwdmxInst, UINT32 u4Percentage)
{
    UINT32 u4AFullness;
    
    u4AFullness = _u4CommAFifoSize - DMX_MUL_GetEmptySize(_u1CommAudDmxId, DMX_PID_TYPE_ES_AUDIO, _u1CommAudPid);
    
    if (_u4CommAFifoSize * u4Percentage <= u4AFullness * 100)
        return TRUE;
    else
        return FALSE;
}

static VOID vWmvBIGetMsg(BOOL fgWait)
{
    UINT16 u2QIdx;
    UINT32 u4Msg;
    SIZE_T zMsgSize;
    
    // flush the BI message Q
    zMsgSize = sizeof(UINT32);
    x_msg_q_receive(&u2QIdx, &u4Msg, &zMsgSize, &_hCommBIQ, 1, fgWait ? X_MSGQ_OPTION_WAIT : X_MSGQ_OPTION_NOWAIT);
}
#endif


#ifdef WMV_BAD_INTLV_SUPPORT
static BOOL _WMV_AFifo_Low(SWDMX_INFO_T *prSwdmxInst, UINT32 *pu4ADuration)
{
    UINT32 u4AFullness;
    
    u4AFullness = _u4AFifoSize - DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_AUDIO, _rSwdmxWmvInfo.u1AudPid);
    *pu4ADuration = 1000 * u4AFullness / _u4WmaBytesPerSec;
    
    if (*pu4ADuration < _u4WmvBadIntlvThresA)
        return TRUE;
    else
        return FALSE;
}

static BOOL _WMV_VFifo_Low(SWDMX_INFO_T *prSwdmxInst, UINT32 *pu4VDuration)
{
    UINT16 u2CurQNum, u2CurQNum2, u2MaxQNum;

    VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
    if (ENABLE_NEWTRACK(prSwdmxInst))
    {
        VDEC_GetQueueInfo(prSwdmxInst->u1Vdec2Id, &u2CurQNum2, &u2MaxQNum);
    }
    else
    {
        u2CurQNum2 = u2CurQNum;
    }
    *pu4VDuration = 1000 * min(u2CurQNum, u2CurQNum2) / _rSwdmxWmvInfo.u4FrameRate;
    
    if (*pu4VDuration < _u4WmvBadIntlvThresV)
        return TRUE;
    else
        return FALSE;
}

static VOID _WMVCheckBadIntlv(SWDMX_INFO_T *prSwdmxInst, BOOL fgVideoFull)
{
    UINT32 u4ADuration, u4VDuration;
    
    if (fgVideoFull)  // video full, check audio status
    {
        if (!_rSwdmxWmvInfo.fgEnAudio)
            return;
            
        if (!_rBadIntlv.fgEverEntered && _rSwdmxWmvInfo.u4AudioCount != 0)  // tmp solution to allow only G39, G40, G41 to enter bad interleave flow
            return;
            
        if (_rBadIntlv.eState == BAD_INTLV_STT_NONE || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V)
        {
            if (_WMV_AFifo_Low(prSwdmxInst, &u4ADuration) && !_rBadIntlv.fgAudioEnd)
            {
                LOG(3, "Audio underflow (%dms) due to bad interleave!!!\n", u4ADuration);
                _rBadIntlv.fgEverEntered = TRUE;
                _rBadIntlv.eState = BAD_INTLV_STT_TO_PATCH_A;
                _rBadIntlv.fgRecord = TRUE;
            }
        }
        else if (_rBadIntlv.eState == BAD_INTLV_STT_PATCH_V)
        {
#if defined(WAIT_AUDIO_READY)
            if (_rSwdmxWmvInfo.fgVideoPause)
            {
                _WMVStartToPlay(prSwdmxInst);
            }
#endif
            _rBadIntlv.eState = BAD_INTLV_STT_TO_REPRS_A;
            _rBadIntlv.fgRecord = TRUE;
        }
    }
    else  // audio full, check video status
    {
        if (!_rSwdmxWmvInfo.fgEnVideo)
            return;
            
        if (_rBadIntlv.eState == BAD_INTLV_STT_NONE || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
        {
            if (_WMV_VFifo_Low(prSwdmxInst, &u4VDuration) && !_rBadIntlv.fgVideoEnd)
            {
                LOG(3, "Video underflow (%dms) due to bad interleave!!!\n", u4VDuration);
                _rBadIntlv.fgEverEntered = TRUE;
                _rBadIntlv.eState = BAD_INTLV_STT_TO_PATCH_V;
                _rBadIntlv.fgRecord = TRUE;
            }
        }
        else if (_rBadIntlv.eState == BAD_INTLV_STT_PATCH_A)
        {
#if defined(WAIT_AUDIO_READY)
            if (_rSwdmxWmvInfo.fgVideoPause)
            {
                _WMVStartToPlay(prSwdmxInst);
            }
#endif
            _rBadIntlv.eState = BAD_INTLV_STT_TO_REPRS_V;
            _rBadIntlv.fgRecord = TRUE;
        }
    }
}
#endif  // WMV_BAD_INTLV_SUPPORT


VOID _WMVFindMaxPacketSize(SWDMX_INFO_T *prSwdmxInst)
{
#ifdef __KERNEL__
    UINT32 u4Idx;
    UINT32 u4MaxPacketSize = 0;
    for (u4Idx = 0 ; u4Idx < _u4NumberOfBitstream ; u4Idx++)
    {
        if (_prWmvGenericInfo[u4Idx].ui4_sz_max_acces_unt > u4MaxPacketSize)
        {
            u4MaxPacketSize = _prWmvGenericInfo[u4Idx].ui4_sz_max_acces_unt;
        }
    }
    FeederNotifyHandle(prSwdmxInst->eFeederSrc, FEEDER_NOTIFY_PACKET_SIZE, (u4MaxPacketSize+WMV_NETFLIX_HEADER_SIZE));
#else
    UNUSED(_u4NumberOfBitstream);
#endif
#ifdef WMA_SEEK_OFFSET
    prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u4SampleRate=_prWmvGenericInfo[0].ui4_sz_max_bitrate;
#endif
}

BOOL _WMVDmxMoveData(SWDMX_INFO_T *prSwdmxInst, UINT8 u1InsId,UINT8 u1Idx, DMX_MM_DATA_T *prData, UINT32 u4TimeOut)
{
    BOOL fgRet = TRUE;
    prData->u1Idx = u1Idx;
    
    if (_rSwdmxWmvInfo.fgIsDRM)
    {
        if (_rSwdmxWmvInfo.eDRMType == DRM_KEY_AES)
        {
            if ((!_rSwdmxWmvInfo.fgMoveSeqHdr) && (!_rSwdmxWmvInfo.fgMoveStartCode))
            {
                DMX_MM_DRM_AES_T rMMDrmAes;
                x_memset((VOID*)&rMMDrmAes,0,sizeof(DMX_MM_DRM_AES_T));

                DMX_MM_SetDRMMode(u1Idx,DMX_DRM_MODE_ND); //from 2223_9_001_23_002
                //u4Flag = (UINT32)(DMX_MM_DRM_FLAG_CHUNK_LEN | DMX_MM_DRM_FLAG_ENCRYLEN | DMX_MM_DRM_FLAG_SAMPLE_ID);
                prData->fgTrigger = TRUE;
                //prData->fgNDDRM = TRUE;//from 2223_9_001_23_002
                rMMDrmAes.u4ChunkLen = prData->u4FrameSize;
                rMMDrmAes.u4EncryLen = prData->u4FrameSize;
                rMMDrmAes.fgCBC = FALSE;
                rMMDrmAes.u4KeyBitLen = 128;
                x_memset((VOID*)&rMMDrmAes.au1Key[0],0,sizeof(rMMDrmAes.au1Key));
                x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],_rSwdmxWmvInfo.u4DRMKeySize);

                //x_memset((VOID*)&rMMDrmAes.au1SampleID[0],0,sizeof(rMMDrmAes.au1SampleID)); 
                //x_memcpy((VOID*)&rMMDrmAes.au1SampleID[0],&_u8SampleId,8);

                x_memset((VOID*)&rMMDrmAes.au1IV[0],0,sizeof(rMMDrmAes.au1SampleID)); 
                x_memcpy((VOID*)&rMMDrmAes.au1IV[0],&_u8SampleId,8);
                #ifdef TEST_WMV_DRM
                //TZ_NDDRM_ProtectAESKey (&rMMDrmAes.au1Key[0], _rSwdmxWmvInfo.u4DRMKeySize);
                #endif
                //DMX_MM_SetNDDRM(u1Idx,u4Flag,&rMMDrmAes); 
                TZ_NDDRM_SetAESKey (&rMMDrmAes.au1Key[0], _rSwdmxWmvInfo.u4DRMKeySize, &rMMDrmAes.au1IV[0], 8);
                if(!DMX_MM_SetSecureAES(u1Idx, (UINT32)DMX_MM_DRM_FLAG_ALL, &rMMDrmAes, TRUE))
                {
                    LOG(0,"DMX_MM_SetSecureAES set error..........\n");
                    ASSERT(0);  // !!!
                    return FALSE;
                }
            }
            else
            {
                DMX_MM_SetDRMMode(u1Idx,DMX_DRM_MODE_BYPASS); 
            }    
        }
        else if(_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4)
        {          
#ifdef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA
            prData->fgPDDRM = FALSE;
#else
            if((!_rSwdmxWmvInfo.fgMoveSeqHdr) && (!_rSwdmxWmvInfo.fgMoveStartCode))
            {               
                prData->fgPDDRM = TRUE;
            }
#endif
        }
    }
    if(prData->u4Pts==0)
    {
      LOG(2,"+++++++++++++++++++++++++PTS IS ZERO++++++++++++++++++\n"); 
    }
    LOG(9, "prData->u4Pts  = %x\n",prData->u4Pts);
    fgRet = _SWDMX_MoveData(u1InsId, prData);
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
    if (fgRet && prSwdmxInst->fgIsBadInterleaved)
    {
        if (prSwdmxInst->fgEnVideoDmx)
        {
            if (!_rSwdmxWmvInfo.fgVideoPause && !_WMV_AFifo_Fullness(prSwdmxInst, _u4BIAThreshold2))
            {
                LOG(1, "A fullness under %d%, V thread sleep for a while...\n", _u4BIAThreshold2);
                x_thread_delay(_u4BIVideoDelay2);
            }
        }
        /*
        else
        {
            x_thread_delay(_u4BIAudioDelay);
        }
        */
    }
#endif
    return fgRet;
}

UINT32 _WMVParsingStartCode(UINT32 u4FrameAddr,UINT32 u4FifoStart,UINT32 u4FifoEnd)
{
    UINT32 u4StartCode = 0;

    UINT8 u1Byte1,u1Byte2,u1Byte3,u1Byte4;
#if 0
    if (!(u4FrameAddr + 4 > u4FifoEnd))
    {
        x_memcpy(&u4StartCode, (VOID*)VIRTUAL(u4FrameAddr), 4);
        u4StartCode = ((u4StartCode&0x000000FF)<<24)
                      | ((u4StartCode&0x0000FF00)<<8) | ((u4StartCode&0x00FF0000)>>8)
                      | ((u4StartCode&0xFF000000)>>24);
    }
    else
    {
        //ASSERT(0);
    }
#else
    x_memcpy(&u1Byte1,(VOID*)VIRTUAL(u4FrameAddr),1);
    x_memcpy(&u1Byte2,(VOID*)VIRTUAL((u4FrameAddr+1)%u4FifoEnd),1);
    x_memcpy(&u1Byte3,(VOID*)VIRTUAL((u4FrameAddr+2)%u4FifoEnd),1);
    x_memcpy(&u1Byte4,(VOID*)VIRTUAL((u4FrameAddr+3)%u4FifoEnd),1);
    u4StartCode = u1Byte1<<24 |u1Byte2<<16 |u1Byte3<<8 |u1Byte4;
#endif
    return u4StartCode;
}


#ifdef WMV_BAD_INTLV_DEBUG
UINT32 _u4PrintPer = 10;
void vShowMoveStatus(SWDMX_INFO_T *prSwdmxInst, UINT32 u4FileSize, UINT32 u4Sa, BOOL fgVideo)
{
    UINT8 u1Data = *(UINT8 *)u4Sa;
    
    if ((_rSwdmxWmvInfo.u4CurParsingPacketNo % _u4PrintPer == 0)/* || _rSwdmxWmvInfo.u4CurParsingPacketNo >= 1100*/)
        printf("[swmv]%02X l=%d %ca=%08X p#%d s=%d\n",
            (UINT32)u1Data,
            u4FileSize,
            fgVideo ? 'v' : 'a', 
            fgVideo ? _u4VFifoAccuLen : _u4AFifoAccuLen,
            _rSwdmxWmvInfo.u4CurParsingPacketNo,
            (int)_rBadIntlv.eState);
}
#endif

static VOID _SWDMX_WMVHandleAacHeader(SWDMX_INFO_T *prSwdmxInst, 
                                      UINT8 u1Pidx, 
                                      UINT32 u4PayloadSz,
                                      UINT32 u4Pts)
{
    UINT8 u1SampleRateIdx = 3;
    //UINT32 u4RequestSize = AAC_ALLOC_SIZE;
    //UINT32 u4Align = 16;
    DMX_MM_DATA_T rDmxMMDataAacHdr = {0};
    
    if(prSwdmxInst->pucAudBuf == NULL)
    {
        LOG(1, "pucAudBuf should be allocated in init function\n");

        prSwdmxInst->pucAudBuf = (UINT8 *)BSP_AllocAlignedDmaMemory(AAC_ALLOC_SIZE, 16);
    }
    //LOG(1, "pucAudBuffer 0x%x\n", prSwdmxInst->pucAudBuf);
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
        switch (_u4AudSampleRate)
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
             ((_u4AudChannelNs >> 2) & 0x1);
        /* 2b: channel_configuration */
        /* 1b: original */
        /* 1b: home */
        /* 1b: copyright_id */
        /* 1b: copyright_id_start */
        /* 2b: aac_frame_length */
        prSwdmxInst->pucAudBuf[3] = ((_u4AudChannelNs & 0x3) << 6) |
                 (((u4PayloadSz + AAC_FAKE_HDR_SIZE) >> 11) & 0x3);
        /* 8b: aac_frame_length */
        prSwdmxInst->pucAudBuf[4] = (((u4PayloadSz + AAC_FAKE_HDR_SIZE) >> 3) & 0xFF);
        /* 3b: aac_frame_length */
        /* 5b: adts_buffer_fullness */
        prSwdmxInst->pucAudBuf[5] = (((u4PayloadSz + AAC_FAKE_HDR_SIZE) << 5) & 0xE0) |
                 ((0x7FF >> 6) & 0x1F);
        /* 6b: adts_buffer_fullness */
        /* 2b: number_of_raw_data_blocks_in_frame */
        prSwdmxInst->pucAudBuf[6] = ((0x7FF << 2) & 0xFC);
        rDmxMMDataAacHdr.u4BufStart = VIRTUAL((UINT32)prSwdmxInst->pucAudBuf);
        rDmxMMDataAacHdr.u4BufEnd = ((UINT32)( rDmxMMDataAacHdr.u4BufStart+AAC_ALLOC_SIZE) + 0x10+188)&0xFFFFFFF0;
            //rDmxMMDataAacHdr.u4BufStart + AAC_ALLOC_SIZE;
        rDmxMMDataAacHdr.u4StartAddr = rDmxMMDataAacHdr.u4BufStart;
        rDmxMMDataAacHdr.u4FrameSize = AAC_FAKE_HDR_SIZE;
        rDmxMMDataAacHdr.u4Pts = u4Pts; // without this, PTS is zero
        //rDmxMMDataAacHdr.u1Idx = _rSwdmxWmvInfo.u1AudPid;
        rDmxMMDataAacHdr.fgFrameHead = TRUE;
              
        /*
        LOG(1, "try to move AAC header 0x%x(%02x %02x %02x %02x %02x %02x %02x)\n", 
            rDmxMMDataAacHdr.u4BufStart,
            prSwdmxInst->pucAudBuf[0],
            prSwdmxInst->pucAudBuf[1],
            prSwdmxInst->pucAudBuf[2],
            prSwdmxInst->pucAudBuf[3],
            prSwdmxInst->pucAudBuf[4],
            prSwdmxInst->pucAudBuf[5],
            prSwdmxInst->pucAudBuf[6]);      
        */
        if (_WMVDmxMoveData(prSwdmxInst,
                            prSwdmxInst->u1SwdmxId,
                            u1Pidx,
                            &rDmxMMDataAacHdr, 
                            500) != TRUE)                       
        {
            LOG(1, "Demuxer fails to move AAC frame header.\n");
        } 
    }
    return;
}

#if 0
static BOOL _SWDMX_WMAHandleMultipayload(SWDMX_INFO_T *prSwdmxInst,
                                         UINT8 u1Pidx, 
                                         UINT32 u4PayloadSz,
                                         UINT32 u4RetryAudio,
                                         UINT16 u2WaitTimes,
                                         UINT32 u4DmxAvailSize,
                                         BOOL fgSendToDMX,
                                         DMX_MM_DATA_T *prDmxMMData)
{
    BOOL fgDmxRoomEnough = FALSE;
    u1Pidx = _rSwdmxWmvInfo.u1AudPid;

#ifdef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA
    if(_rSwdmxWmvInfo.fgIsDRM && (_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4))
    {   
        if (DMX_PDDRM_Decrypt(u1Pidx, prDmxMMData) != TRUE)
        {
           LOG(0,"DMX_PDDRM_Decrypt Audio Fail !!!!!!!!\n");
           return FALSE;
        }                     
    }
#endif
    /* if audio is AAC, send AAC frame header firstly */
    if (_rSwdmxWmvInfo.eAudioType == ENUM_SWDMX_ADEC_AAC)
    {
        _SWDMX_WMVHandleAacHeader(prSwdmxInst, u1Pidx, u4PayloadSz, prDmxMMData->u4Pts);
    }


    if (_rSwdmxWmvInfo.fgBlock && _rSwdmxWmvInfo.u2SendPayloadNo > 1)
        _rSwdmxWmvInfo.u2SendPayloadNo--;
    if (u4DmxAvailSize >= u4PayloadSz * (_rSwdmxWmvInfo.u2WmaPlCntPerPkt 
                                        - (_rSwdmxWmvInfo.u2SendPayloadNo
                                           %_rSwdmxWmvInfo.u2WmaPlCntPerPkt)))
    {
        /* DMX available size is enough */
        fgDmxRoomEnough = TRUE;
    }
    if (fgDmxRoomEnough)
    {
        //move_data()
        if (FALSE == _WMVDmxMoveData(prSwdmxInst,
                                     prSwdmxInst->u1SwdmxId,
                                     u1Pidx, 
                                     prDmxMMData, 
                                     500))
        {
            LOG(0, "Demuxer fails to move audio data.\n");
            return FALSE;
        }
        else
        {
            _rSwdmxWmvInfo.u4AudioObjSz = u4PayloadSz;
            _rSwdmxWmvInfo.u4PrevAudPts = prDmxMMData->u4Pts;
            LOG(5, "mv %08x\n", (prDmxMMData->u4Pts)/90+_rSwdmxWmvInfo.u4PreRoll);                            
        }
        _rSwdmxWmvInfo.u4CurPTS = prDmxMMData->u4Pts;//DTV00213648
        u4RetryAudio = 0;
        fgSendToDMX = TRUE;
        _rSwdmxWmvInfo.fgBlock = FALSE;
        _rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;
    }
    else
    {
        if (u4RetryAudio >= u2WaitTimes)
        {
            _rSwdmxWmvInfo.fgBlock = TRUE;
            if (_rSwdmxWmvInfo.u2PreviousSendPayloadNo > 0)
            {
                LOG(6, "fifo full old raw %08x(s:%d/%d)\n", 
                    (_rSwdmxWmvInfo.u4PrevAudPts)/90
                        +_rSwdmxWmvInfo.u4PreRoll,
                    _rSwdmxWmvInfo.u2SendPayloadNo,
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo); 
                //x_thread_delay(20);//if no delay, maybe cause watchdog timeout
                return FALSE;
            }
            LOG(5, "fifo full pts raw %08x(s:%d/%d)\n", 
                (prDmxMMData->u4Pts)/90+_rSwdmxWmvInfo.u4PreRoll,
                _rSwdmxWmvInfo.u2SendPayloadNo,
                _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo = _rSwdmxWmvInfo.u2SendPayloadNo;
            x_thread_delay(20);
            if (_rSwdmxWmvInfo.fgAudIsReady)
            {
                AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                STC_StartStc(prSwdmxInst->u1StcId);
                _rSwdmxWmvInfo.fgAudIsReady = FALSE;
            }
            prSwdmxInst->fgFifoFull=TRUE;
            return FALSE;
        }
        u4RetryAudio++;
        x_thread_delay(1);
        fgSendToDMX = FALSE;
    }
    return TRUE;
}
#endif

BOOL _WMVMoveData(SWDMX_INFO_T *prSwdmxInst, 
                  UINT32 u4StartAddress,
                  UINT32 u4CurPosition ,
                  UINT32 u4FileSize,
                  PayloadHeader* prPayLoadHeader,
                  BOOL fgHeadSegment)
{
    //UINT32 static u4AudioCount = 0;
    //UINT32 u4VideoCount =0;
    UINT32 u4FrameCount=0;
    UINT32 u4Data;
    //UINT32 static u4Count =0;
    DMX_MM_DATA_T rDmxMMData;
    DMX_MM_DATA_T rDmxMMData_StartCode;
    DMX_MM_DATA_T rDmxMMData_SequenceHeader;
    UINT32 u4DmxAvailSize;
    BOOL fgSendToDMX = FALSE;
    //UINT32 u4StartCode[10];
    UINT32 u4RetryTimes =0;
    UINT16 u2VdecQueueSize =0;
    UINT16 u2VdecMaxQueueSize=0;
    UINT16 u2DmxPictureQSize = 0;
    UINT16 u2DmxMaxQSize = 0;
    UINT16 u2WaitTimes=50;
    BOOL fgVideoSegment = FALSE;
    BOOL fgAudioSegment = FALSE;
    UINT8 u1Idx;
    
    if((prPayLoadHeader->u1StreamNum==_rSwdmxWmvInfo.u4VStreamId)||IS_NEWTRACKDATA(prSwdmxInst,prPayLoadHeader->u1StreamNum))
    {
       fgVideoSegment=TRUE;
       if(IS_NEWTRACKDATA(prSwdmxInst,prPayLoadHeader->u1StreamNum))
       {
         if (prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_IFRAME || _rSwdmxWmvInfo.i4WMVPlaySpeed==2000)
            return TRUE;
         
         u1Idx=_rSwdmxWmvInfo.u1Vid2Pid;
       }
       else
       {
         u1Idx=_rSwdmxWmvInfo.u1VidPid;
       }
    }
    else
    {
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        u1Idx=_rSwdmxWmvInfo.u1AudPreBufPid[_rSwdmxWmvInfo.u4AStreamId]; 
        if (!_rSwdmxWmvInfo.fgAlreadySet)
        {
            DMX_AudHandler_SetActivePidx(u1Idx);  // !!!
            _rSwdmxWmvInfo.fgAlreadySet = TRUE;
        }
        #else
        u1Idx=_rSwdmxWmvInfo.u1AudPid; //todo-audioprebuffer
        if (!_rSwdmxWmvInfo.fgAlreadySet)
        {
            //DMX_AudHandler_SetActivePidx(u1Idx);  // !!!
            _rSwdmxWmvInfo.fgAlreadySet = TRUE;
        }
        #endif
        if (ASID(prPayLoadHeader->u1StreamNum) == (UINT8)_rSwdmxWmvInfo.u4AStreamId)
        {
            fgAudioSegment = TRUE;
        }
    }
    
#ifdef WMV_BAD_INTLV_SUPPORT
    if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
    {
        if (fgVideoSegment)
            return TRUE;  // skip video
    }
    else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V || _rBadIntlv.eState == BAD_INTLV_STT_PATCH_V || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V)
    {
        if (fgAudioSegment)
            return TRUE;  // skip audio
    }
    
    if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_PATCH_A ||
        _rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V || _rBadIntlv.eState == BAD_INTLV_STT_PATCH_V)
    {
        if (_rSwdmxWmvInfo.u4CurParsingPacketNo < _rBadIntlv.u4EndPatchPktNo)
            return TRUE;
        if (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4EndPatchPktNo && rMultiPayloadInfo.u1CurPayloadIdx < _rBadIntlv.u4EndPatchPldNo)
            return TRUE;
    }
    
    if (_rBadIntlv.fgAudioEnd && fgAudioSegment)
        return TRUE;
    if (_rBadIntlv.fgVideoEnd && fgVideoSegment)
        return TRUE;
#else
    UNUSED(fgAudioSegment);
#endif

    x_memset(&rDmxMMData,0,sizeof(DMX_MM_DATA_T));
    x_memset(&rDmxMMData_StartCode,0,sizeof(DMX_MM_DATA_T));
    x_memset(&rDmxMMData_SequenceHeader,0,sizeof(DMX_MM_DATA_T));


    if ((_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES)&&(prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_IFRAME))
    {
        u2WaitTimes = TRICK_MODE_RETRY_TIMES;
    }
    else
    {
        u2WaitTimes = RETRY_TIMES;
    }

    //u4StartCode[1] = 0x0D010000;

    _rSwdmxWmvInfo.u2SendPayloadNo++;

    //VDEC_GetQueueInfo(prSwdmxInst->u1VdecId,&u2VdecQueueSize,&u2VdecMaxQueueSize);
    //LOG(0,"status : vdec queue size : %d/%d \n",u2VdecQueueSize,u2VdecMaxQueueSize);

    if (prPayLoadHeader->u4PTS < _rSwdmxWmvInfo.u4PreRoll)
    {
        LOG(0, "payload pts(0x%x) < preroll(0x%x), drop!\n", 
            prPayLoadHeader->u4PTS,
            _rSwdmxWmvInfo.u4PreRoll);
        return TRUE;
    }
    _rSwdmxWmvInfo.u4CurPacketPTS = (prPayLoadHeader->u4PTS-_rSwdmxWmvInfo.u4PreRoll)*90;
    if (_rSwdmxWmvInfo.u4CurPacketPTS > 0xFFFF0000)
    {
        LOG(0, "u4CurPacketPTS(0x%x) > 0xFF000000, drop!\n", _rSwdmxWmvInfo.u4CurPacketPTS);
        return TRUE;
    }
    if (prPayLoadHeader->fgKeyFrame && fgHeadSegment && fgVideoSegment && prPayLoadHeader->u1StreamNum==(UINT8)_rSwdmxWmvInfo.u4VStreamId)
    {
        _rSwdmxWmvInfo.fgGetKeyHead = TRUE;
        _rSwdmxWmvInfo.u4IFrameOffset = _rSwdmxWmvInfo.u4CurParsingPacketNo;
        
    }
    else if (!prPayLoadHeader->fgKeyFrame && fgHeadSegment && fgVideoSegment)
    {
        _rSwdmxWmvInfo.u4FrameOffset = _rSwdmxWmvInfo.u4CurParsingPacketNo;
    }

    else
    {
        //null operation
    }

    if (_rSwdmxWmvInfo.u2SendPayloadNo  < _rSwdmxWmvInfo.u2PreviousSendPayloadNo)
    {
        LOG(5, "SendNo(%d) < PrevNo(%d), ret\n",
            _rSwdmxWmvInfo.u2SendPayloadNo,
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
        return TRUE;
    }

    if (fgVideoSegment&& (prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_IFRAME))
    {

        if(!_rSwdmxWmvInfo.fgGetKeyHead)
        {
            //LOG(1, "[md] didn't getKeyHead, return\n");
            return TRUE;
        }

        if(fgHeadSegment&&prPayLoadHeader->u1StreamNum==(UINT8)_rSwdmxWmvInfo.u4VStreamId)
        {
            if(prPayLoadHeader->fgKeyFrame&&(_rSwdmxWmvInfo.u1FWKeyFrameNo==0))
            {
                _rSwdmxWmvInfo.u1FWKeyFrameNo=1;
                LOG(5, "+key frame 0x%x\n", _rSwdmxWmvInfo.u4CurPacketPTS);
            }
            else if(_rSwdmxWmvInfo.u1FWKeyFrameNo==1)
            {
               _rSwdmxWmvInfo.fgFinishDemuxIFrame = TRUE;
                LOG(5, "+finish demux I frame 0x%x\n", _rSwdmxWmvInfo.u4CurPacketPTS);
               return TRUE;
            }
        }
    }
    //LOG(0,"packet distance : %d\n",u4CurPosition - u4Address);
    //u4Address = u4CurPosition;

    rDmxMMData.u4BufStart = _rFeederInfo.u4StartAddr;
    rDmxMMData.u4BufEnd = _rFeederInfo.u4EndAddr;
    rDmxMMData.u4StartAddr = (u4StartAddress + u4CurPosition);

    if (rDmxMMData.u4StartAddr < rDmxMMData.u4BufStart)
    {
        rDmxMMData.u4BufStart -= FEEDER_OFFSET;
    }

    if (rDmxMMData.u4StartAddr >= rDmxMMData.u4BufEnd || rDmxMMData.u4StartAddr < rDmxMMData.u4BufStart)
    {
        LOG(1,"rDmxMMData.u4StartAddr:%x,rDmxMMData.u4BufEnd:%x u4StartAddress : %x,u4CurPosition:%x\n",
            rDmxMMData.u4StartAddr,rDmxMMData.u4BufEnd,u4StartAddress,u4CurPosition);
        LOG(1, "!!!!!!!!!!!!!!!!! wrong parm @ 660 !!!!!!!!!!!!!!!\n");
        return FALSE;
    }
    
    //if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1 && _rSwdmxWmvInfo.fgFileWithStartCode)
    //    rDmxMMData.u4FrameSize = (UINT32)max(6,u4FileSize);
    //else
    rDmxMMData.u4FrameSize = u4FileSize;
    rDmxMMData.u4Pts = ((prPayLoadHeader->u4PTS-_rSwdmxWmvInfo.u4PreRoll)*90);

    //rDmxMMData.fgFrameTail  = fgEndFrame;
    rDmxMMData.fgFrameHead  = fgHeadSegment;

    if (u4FileSize ==0)
    {
        LOG(5, "filesz==0, ret\n");
        return TRUE;
    }

    if (_rSwdmxWmvInfo.fgEnVideo &&
       ((prPayLoadHeader->u1StreamNum == (UINT8)_rSwdmxWmvInfo.u4VStreamId)||IS_NEWTRACKDATA(prSwdmxInst,prPayLoadHeader->u1StreamNum)))
    {
        if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1
	     || _rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_MPEG4
		 || _rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_H264
		 || _rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_H263)
        {
            if (_rSwdmxWmvInfo.u4VideoCount == 0
#ifndef __MODEL_slt__
                &&_rSwdmxWmvInfo.u4SequenceHeaderLength>0
#endif
                )
            {
                UINT8 u1BindingByte = 0;
                // DTV00213176
                //x_memcpy((VOID*)(VIRTUAL(&_pu1VideoFrameData[0])), (VOID*)VIRTUAL(_rSwdmxWmvInfo.u4SequenceHeader), (_rSwdmxWmvInfo.u4SequenceHeaderLength));
                //_rSwdmxWmvInfo.u4SequenceHeader = (UINT32)VIRTUAL(&_pu1VideoFrameData[0]);
#ifdef __MODEL_slt__
                x_memcpy((VOID *)(VIRTUAL(&_pu1VideoFrameData[0])), (VOID*)_au1VC1SltSeqHdr, 97);
                _rSwdmxWmvInfo.u4SequenceHeader = (UINT32)VIRTUAL(&_pu1VideoFrameData[0]);
                _rSwdmxWmvInfo.u4SequenceHeaderLength = 97;
#endif

                rDmxMMData_SequenceHeader.u4BufStart = (UINT32)(_rSwdmxWmvInfo.u4SequenceHeader)&0xFFFFFFF0;
                rDmxMMData_SequenceHeader.u4BufEnd = ((UINT32)(_rSwdmxWmvInfo.u4SequenceHeader) + 0x10+188)&0xFFFFFFF0;
                rDmxMMData_SequenceHeader.u4StartAddr = _rSwdmxWmvInfo.u4SequenceHeader;//(UINT32)&_rSwdmxWmvInfo.au1SequenceHeader[0];
                rDmxMMData_SequenceHeader.u4FrameSize = (UINT32)(_rSwdmxWmvInfo.u4SequenceHeaderLength);
/*#if defined(SWDMX_IS_POST_MT5368)
                if (rDmxMMData_SequenceHeader.u4FrameSize < 128 + 16)
                {
                    rDmxMMData_SequenceHeader.u4FrameSize = 128 + 16;
                }
#endif*/
                rDmxMMData_SequenceHeader.u4Pts = 0;
                rDmxMMData_SequenceHeader.fgFrameHead  = TRUE;
                _rSwdmxWmvInfo.fgMoveSeqHdr = TRUE;
                x_memcpy(&u1BindingByte,(VOID*)rDmxMMData_SequenceHeader.u4BufStart,1);
                
                 LOG(3, "%s(%d): Setup binding byte to WMV decoder1 : %02x\n", __FUNCTION__, __LINE__, u1BindingByte);
                 VDEC_SetParam(prSwdmxInst->u1VdecId, 18, (UINT32)(&u1BindingByte), 0, 0);
                 {
                     /* show some bytes of seq header*/
                     UINT32 j = 0;
                     UINT32 len = min(8, rDmxMMData_SequenceHeader.u4FrameSize);
                     printf("sequence header info(%d of %d): ", len, rDmxMMData_SequenceHeader.u4FrameSize);
                     for (j = 0; j < len; j++)
                         printf("%02x ", *((UINT8*)rDmxMMData_SequenceHeader.u4StartAddr + j));
                     printf("\n");
                 }
                if (_WMVDmxMoveData(prSwdmxInst, prSwdmxInst->u1SwdmxId,_rSwdmxWmvInfo.u1VidPid, &rDmxMMData_SequenceHeader, 500) != TRUE)
                {
                    LOG(0, "Demuxer fails to move VC1/Mpeg4 sequence header.\n");
                    ASSERT(0);
                    return FALSE;
                }
                
                if(ENABLE_NEWTRACK(prSwdmxInst))
                {
                    LOG(3, "%s(%d): Setup binding byte to WMV decoder2 : %02x\n", __FUNCTION__, __LINE__, u1BindingByte);
                    VDEC_SetParam(prSwdmxInst->u1Vdec2Id, 18, (UINT32)(&u1BindingByte), 0, 0);
                    if (_WMVDmxMoveData(prSwdmxInst, prSwdmxInst->u1SwdmxId,_rSwdmxWmvInfo.u1Vid2Pid, &rDmxMMData_SequenceHeader, 500) != TRUE)
                    {
                        LOG(0, "Demuxer fails to move VC1/Mpeg4 sequence header.\n");
                        ASSERT(0);
                        return FALSE;
                    }
                }
                if (_rSwdmxWmvInfo.fgIsDRM)
                {
                    LOG(3, "Set VDEC param encrypted!!!\n");
                    VDEC_SetParam(prSwdmxInst->u1VdecId, 19, TRUE, 0, 0);
                }
            }
            _rSwdmxWmvInfo.u4VideoCount++;
            _rSwdmxWmvInfo.fgMoveSeqHdr = FALSE;
        }
        do
        { 
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            if(u2DmxMaxQSize <= u2DmxPictureQSize)
            { 
                LOG(3,"DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
                x_thread_delay(50);
                return FALSE;
            }
            
            u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_VIDEO, u1Idx);
            
            if(IS_NEWTRACKDATA(prSwdmxInst,prPayLoadHeader->u1StreamNum))
            {
               VDEC_GetQueueInfo(prSwdmxInst->u1Vdec2Id,&u2VdecQueueSize,&u2VdecMaxQueueSize);
            }
            else
            {
               VDEC_GetQueueInfo(prSwdmxInst->u1VdecId,&u2VdecQueueSize,&u2VdecMaxQueueSize);
            }
#if 1
            if (_rSwdmxWmvInfo.fgVideoNormalPlay && fgHeadSegment && prPayLoadHeader->fgKeyFrame)
            {
                _rSwdmxWmvInfo.u4TriggerAudioPTS = rDmxMMData.u4Pts;
                _rSwdmxWmvInfo.u4DisplayPTS = rDmxMMData.u4Pts;
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, (UINT32)(rDmxMMData.u4Pts-3000));
                if (_rSwdmxWmvInfo.fgEnVideo)//protect flag
                {
                    _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;
                }
            }
            else if (_rSwdmxWmvInfo.fgVideoNormalPlay)
            {
                return TRUE;
            }
#endif

            //if((u4FileSize <= u4DmxAvailSize) && (u2VdecQueueSize <= 3*30))
            if ((u4FileSize + 4 <= u4DmxAvailSize) && // megaa: add 4 for start code's space!
                ((u2VdecMaxQueueSize - u2VdecQueueSize) >256) &&
                ((!_rSwdmxWmvInfo.fgEnADmx && u2VdecQueueSize < 30) || 
                (_rSwdmxWmvInfo.fgEnADmx))
                )
            {
#ifdef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA                
                if (_rSwdmxWmvInfo.fgIsDRM && (_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4))
                {   
                    if (DMX_PDDRM_Decrypt(u1Idx, &rDmxMMData) != TRUE)
                    {
                       LOG(0,"DMX_PDDRM_Decrypt Video Fail !!!!!!!!\n");
                       return FALSE;
                    }                     
                }
#endif
                if (prPayLoadHeader->u4OffsetIntoMediaObj == 0 && _rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1
#ifndef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA
                        && !_rSwdmxWmvInfo.fgFileWithStartCode
#endif
                    )  // megaa 20100331 ToDo: check who added the "&!_rSwdmxWmvInfo.fgFileWithStartCode"
                {
                    UINT32 u4PreStartCode = _WMVParsingStartCode(rDmxMMData.u4StartAddr,rDmxMMData.u4BufStart,rDmxMMData.u4BufEnd);
                    if (!(u4PreStartCode ==WMV_SC_FRAME || u4PreStartCode==WMV_SC_ENTRY || u4PreStartCode==WMV_SC_FIELD|| u4PreStartCode==WMV_SC_SEQ))
                    {
                        UINT32 u4Tmp = 0x0D010000;
                        x_memcpy((VOID*)(VIRTUAL((UINT32)&_pu1VideoFrameData[0])),(VOID*)&u4Tmp,4);

                        //x_memcpy((VOID*)(VIRTUAL(&_pu1VideoFrameData[4])),(VOID*)rDmxMMData.u4StartAddr,rDmxMMData.u4FrameSize);
                        rDmxMMData_StartCode.u4BufStart = VIRTUAL((UINT32)(_pu1VideoFrameData));
                        rDmxMMData_StartCode.u4BufEnd = ((UINT32)( rDmxMMData_StartCode.u4BufStart+1*1024) + 0x10+188)&0xFFFFFFF0;
                        rDmxMMData_StartCode.u4StartAddr = VIRTUAL((UINT32)(_pu1VideoFrameData));
                        rDmxMMData_StartCode.u4FrameSize = 4;
                        rDmxMMData_StartCode.u4Pts = rDmxMMData.u4Pts;
                        rDmxMMData_StartCode.fgFrameHead= TRUE;
                        _rSwdmxWmvInfo.fgMoveStartCode =TRUE;

                        //if((_rSwdmxWmvInfo.i4WMVPlaySpeed > 2000 || _rSwdmxWmvInfo.i4WMVPlaySpeed <= -2000))
                        //    LOG(0,"send addition start code : %x\n",rDmxMMData.u4Pts);
                        LOG(9,"Send VC1 frame statr_code\n");
                        if (_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,u1Idx,&rDmxMMData_StartCode, 500) != TRUE)
                        {
                            LOG(0, "Demuxer fails to move frame start_code data.\n");
                            return FALSE;
                        }
                        _rSwdmxWmvInfo.fgMoveStartCode = FALSE;
                    }
                    else
                    {
#ifdef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA
                        LOG(5, "after decrypt, video frame has start_code!!(no need send extra data)\n");
#else
                        _rSwdmxWmvInfo.fgFileWithStartCode = TRUE;
                        //LOG(0,"!!!!!!! wmv error bug\n");
#endif
                    }

                }
                //else
                if(_rSwdmxWmvInfo.fgIsDRM && (_rSwdmxWmvInfo.eDRMType == DRM_KEY_AES))
                {
                    x_memcpy((VOID*)(VIRTUAL(_u4VideoFrameDataAddr+ prPayLoadHeader->u4OffsetIntoMediaObj)),
                         (VOID*)rDmxMMData.u4StartAddr,u4FileSize);
                    HalFlushDCacheMultipleLine((UINT32)(_u4VideoFrameDataAddr+ prPayLoadHeader->u4OffsetIntoMediaObj), (UINT32)u4FileSize);
                    if ((prPayLoadHeader->u4OffsetIntoMediaObj + u4FileSize == prPayLoadHeader->u4MediaObjSize))
                    {
                        rDmxMMData.u4BufStart = VIRTUAL((UINT32)(_u4VideoFrameDataAddr)&0xFFFFFFF0);
                        rDmxMMData.u4BufEnd = VIRTUAL(((UINT32)(_u4VideoFrameDataAddr+200*1024) + 0x10+188)&0xFFFFFFF0);
                        rDmxMMData.u4StartAddr = VIRTUAL((UINT32)_u4VideoFrameDataAddr);
                        rDmxMMData.u4FrameSize = prPayLoadHeader->u4MediaObjSize;// + rDmxMMData.u4FrameSize;
                        //rDmxMMData.u4Pts = rDmxMMData.u4Pts;
                        rDmxMMData.fgFrameHead= TRUE;
                        if (_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,u1Idx,&rDmxMMData, 500) != TRUE)
                        {
                            LOG(0, "Demuxer fails to move video data.\n");
                            return FALSE;
                        }
                    }
                }              
                else//normal move data
                {
                    LOG(9, "Move the video data! pts %x(%d.%03d)\n", rDmxMMData.u4Pts,PTS_S(rDmxMMData.u4Pts),PTS_MS(rDmxMMData.u4Pts));

                    if(rDmxMMData.u4FrameSize > 18)
                    {
                        LOG(9,"Packet no(%d)[V:%d]The send data first 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            _rSwdmxWmvInfo.u4VStreamId,
                            *((UINT8*)rDmxMMData.u4StartAddr + 0),
                            *((UINT8*)rDmxMMData.u4StartAddr + 1),
                            *((UINT8*)rDmxMMData.u4StartAddr + 2),
                            *((UINT8*)rDmxMMData.u4StartAddr + 3),
                            *((UINT8*)rDmxMMData.u4StartAddr + 4),
                            *((UINT8*)rDmxMMData.u4StartAddr + 5),
                            *((UINT8*)rDmxMMData.u4StartAddr + 6),
                            *((UINT8*)rDmxMMData.u4StartAddr + 7));
                        LOG(9,"Packet no(%d)[V:%d]The send data midlle 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            _rSwdmxWmvInfo.u4VStreamId,
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 1),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 3),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 4),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 5),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 6),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 7));
                        
                        LOG(9,"Packet no(%d)[V:%d]The send data last 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            _rSwdmxWmvInfo.u4VStreamId,
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -8),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -7),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -6),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -5),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -4),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -3),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -1));
                    }
                    
                    if(_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,u1Idx, &rDmxMMData, 500) != TRUE)
                    {
                        LOG(0, "Demuxer fails to move video data.\n");
                        return FALSE;
                    }
                    else
                    {
                        if (prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_IFRAME)
                            LOG(5, "+move data(trick) 0x%x\n", rDmxMMData.u4Pts);
                    }
                    //    _rSwdmxWmvInfo.u8TimebasedLastParsedPktPts = (UINT64)rDmxMMData.u4Pts;
                    
                }
#ifdef WMV_BAD_INTLV_DEBUG
                    _u4VFifoAccuLen += u4FileSize;
                    vShowMoveStatus(prSwdmxInst, u4FileSize, rDmxMMData.u4StartAddr, TRUE);
#endif
                if (_rSwdmxWmvInfo.u2PreviousSendPayloadNo >0)
                {
                    LOG(6,"video start at packet %d , payload %d\n",_rSwdmxWmvInfo.u4CurParsingPacketNo,_rSwdmxWmvInfo.u2SendPayloadNo);
                    x_memcpy(&u4Data,(void*)(VIRTUAL(u4CurPosition+u4StartAddress)),4);
                    LOG(7,"video start again , first 4 bytes : %x\n",u4Data);
                }
                u4RetryTimes = 0;
                //_rSwdmxWmvInfo.u2SendPayloadNo++;
                _rSwdmxWmvInfo.fgBlock = FALSE;
                _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
                fgSendToDMX = TRUE;
            }
            else
            {
                //LOG(0,"dmx doesn't have available size to send payload data\n");
                if (u4RetryTimes >= u2WaitTimes || 
					(_rSwdmxWmvInfo.fgPlayAbort&&u2WaitTimes==TRICK_MODE_RETRY_TIMES))
                {
#ifdef DISABLE_AUDIO_IN_BAD_INTERLEAVE
 #if 1
                    //UINT8 u1PidIdx = prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid;
                    UINT32 u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_AUDIO, u1Idx);
 #else
                    UINT32 u4DmxAvailSize = DMX_MM_GetBufEmptySize(eSWDMX_MEDIA_AUDIO); //indicate video fifo
 #endif
                    UINT32 u4QueueSize = (288*1024 - u4DmxAvailSize);
                    UINT32 u4Ratio = (100*u4QueueSize)/(288*1024);
                    if (u4Ratio <10 && _rSwdmxWmvInfo.fgEnAudio)
                        _rSwdmxWmvInfo.fgEnAudio = FALSE;
                    //LOG(0,"audio FIFO size : %d %d\n",u4QueueSize,((100*u4QueueSize)/(288*1024)));
#endif
                    _rSwdmxWmvInfo.fgBlock = TRUE;
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo = _rSwdmxWmvInfo.u2SendPayloadNo;
                    x_memcpy(&u4Data,(void*)(VIRTUAL(u4CurPosition+u4StartAddress)),4);
                    LOG(6,"video payload size : %d/%d \n",u4FileSize,u4DmxAvailSize);
                    LOG(6,"vdec queue size : %d/%d \n",u2VdecQueueSize,u2VdecMaxQueueSize);
                    LOG(6,"video stop at packet %d , payload %d\n",_rSwdmxWmvInfo.u4CurParsingPacketNo,_rSwdmxWmvInfo.u2SendPayloadNo);
                    LOG(6,"video stop at first 4 bytes : %x\n",u4Data);
                    u4RetryTimes = 0;
#if defined(WAIT_AUDIO_READY) && !defined(WMV_BAD_INTLV_SUPPORT)
                    if (_rSwdmxWmvInfo.fgVideoPause)
                    {
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                        if (prSwdmxInst->fgIsBadInterleaved)
                        {
                            if (_WMV_AFifo_Fullness(prSwdmxInst, _u4BIAThreshold))
                            {
                                LOG(1, "V full with A %d%, start to play!!!\n", _u4BIAThreshold);
                                _WMVStartToPlay(prSwdmxInst);
                            }
                            else
                            {
                                LOG(1, "V full but A under %d%, suspend!!!\n", _u4BIAThreshold);
                                vWmvBIGetMsg(TRUE);
                                LOG(1, "V full and A over %d%, start to play!!!\n", _u4BIAThreshold);
                                _WMVStartToPlay(prSwdmxInst);
                            }
                        }
#else
                        _WMVStartToPlay(prSwdmxInst);
#endif
                    }
#endif
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                    if (prSwdmxInst->fgIsBadInterleaved && !_rSwdmxWmvInfo.fgVideoPause)
                    {
                        LOG(3, "V full, sleep for a while......................\n");
                        x_thread_delay(_u4BIVideoDelay);
                    }
#endif
#ifdef WMV_BAD_INTLV_SUPPORT
                    if (_fgEnBadIntlv)
                    {
                        _WMVCheckBadIntlv(prSwdmxInst, TRUE);
                    }
                    else
                    {
                        if (_rSwdmxWmvInfo.fgVideoPause)
                        {
                            _WMVStartToPlay(prSwdmxInst);
                        }
                    }
#endif
					prSwdmxInst->fgFifoFull=TRUE;
                    return FALSE;
                }
                u4RetryTimes++;
                x_thread_delay(1);
                fgSendToDMX = FALSE;
            }
        } while (!fgSendToDMX);
        if (fgHeadSegment)
            u4FrameCount++;
        _rSwdmxWmvInfo.u4VideoCount++;
        _rSwdmxWmvInfo.u4InvalidStreamNumCount = 0;
        //_rSwdmxWmvInfo.u4LastVideoPTS = rDmxMMData.u4Pts;
    }
    else if ((ASID(prPayLoadHeader->u1StreamNum) == (UINT8)_rSwdmxWmvInfo.u4AStreamId) &&
             _rSwdmxWmvInfo.fgEnAudio &&!_rSwdmxWmvInfo.fgChangeAudTrack &&
#ifdef WMV_SEEK_OFFSET
             !_prRangeList->fgSetRenderPosition &&
#endif
#ifndef WMA_TRICK_MODE
            (_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000)
#else
           ( (_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000)

            ||(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES) )
#endif
            )
    {
#if 1
        UINT32 u4RetryAudio =0;
        do {
            if (_rSwdmxWmvInfo.u4WmaErrorCode)
            {
                if (_rSwdmxWmvInfo.fgFirstAudioError)
                    return TRUE;
                LOG(0,"wma error code %x\n",_rSwdmxWmvInfo.u4WmaErrorCode);

                //enable video to play
#ifdef WAIT_AUDIO_READY
                if (_rSwdmxWmvInfo.fgVideoPause)
                {
                    _WMVStartToPlay(prSwdmxInst);
                }
#endif
                _rSwdmxWmvInfo.fgFirstAudioError = TRUE;
                return TRUE;
            }

            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            if(u2DmxMaxQSize <= u2DmxPictureQSize)
            { 
                LOG(3,"DMX MESSAGE QUEUE FULL,WAIT FOR A WHILE\n");
                x_thread_delay(50);
                return FALSE;
            }
            
            u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, 
                                                  DMX_PID_TYPE_ES_AUDIO, 
                                                  _rSwdmxWmvInfo.u1AudPid);//cannot use u1Idx here
            
			if (_rSwdmxWmvInfo.fgAudioNormalPlay
                && fgHeadSegment
                && (    
                        /* ensure video could be rendered before audio */
                        (rDmxMMData.u4Pts >= _rSwdmxWmvInfo.u4DisplayPTS+3000
                         &&rDmxMMData.u4Pts >= prSwdmxInst->rCurRangeReq.u8StartAudPts+3000) 
                    ||
                        /* ensure beginning data could be sent (DTV498378) */
                        _rSwdmxWmvInfo.u4DisplayPTS == 0 
                   )
               )
            {
                LOG(0,"\n---=== wmv start pts: %x(%d.%03d) ===---\n",
                    rDmxMMData.u4Pts,
                    PTS_S(rDmxMMData.u4Pts),
                    PTS_MS(rDmxMMData.u4Pts));
                //VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, rDmxMMData.u4Pts);
                _rSwdmxWmvInfo.u4TriggerAudioPTS = rDmxMMData.u4Pts;
                _rSwdmxWmvInfo.u4CurPTS = rDmxMMData.u4Pts;//DTV00213648
                prSwdmxInst->rCurRangeReq.u8StartAudPts=0;
                AUD_SetStartPts(prSwdmxInst->u1AdecId,rDmxMMData.u4Pts);
                if (_rSwdmxWmvInfo.fgEnAudio)//protect flag
                {
                    _rSwdmxWmvInfo.fgAudioNormalPlay = FALSE;
                }
            }
            else if (_rSwdmxWmvInfo.fgAudioNormalPlay)
                //else if(_rSwdmxWmvInfo.fgAudioNormalPlay && !fgHeadSegment )
            {
                LOG(5, "aud normal, ret\n");
                return TRUE;
            }

            if (_rSwdmxWmvInfo.eAudioType == ENUM_SWDMX_ADEC_AAC 
                && u4DmxAvailSize > AAC_FAKE_HDR_SIZE
                )
            {
                u4DmxAvailSize -= AAC_FAKE_HDR_SIZE;
            }

            /* WMA file may contain more than one payload in one packet */
            _rSwdmxWmvInfo.u2WmaPlCntPerPkt = _rSwdmxWmvInfo.u4PacketSize / u4FileSize;
            if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES
                && _rSwdmxWmvInfo.u2WmaPlCntPerPkt > 1)
            {
                BOOL fgDmxRoomEnough = FALSE;
                DMX_MM_DATA_T* prDmxMMData = &rDmxMMData;
                UINT8 u1Pidx = u1Idx;
                UINT32 u4PayloadSz = u4FileSize;
                UINT16 u2PlToBeMoved = 0;

                #ifdef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA
                if (_rSwdmxWmvInfo.fgIsDRM && (_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4))
                {   
                    if (DMX_PDDRM_Decrypt(u1Pidx, prDmxMMData) != TRUE)
                    {
                       LOG(0,"DMX_PDDRM_Decrypt Audio Fail !!!!!!!!\n");
                       return FALSE;
                    }                     
                }
                #endif
                /* if audio is AAC, send AAC frame header firstly */
                if (_rSwdmxWmvInfo.eAudioType == ENUM_SWDMX_ADEC_AAC)
                {
                    _SWDMX_WMVHandleAacHeader(prSwdmxInst, u1Pidx, u4PayloadSz, prDmxMMData->u4Pts);
                }

                if (_rSwdmxWmvInfo.fgBlock && _rSwdmxWmvInfo.u2SendPayloadNo > 1)
                    _rSwdmxWmvInfo.u2SendPayloadNo--;
                u2PlToBeMoved = _rSwdmxWmvInfo.u2WmaPlCntPerPkt 
                                - (_rSwdmxWmvInfo.u2SendPayloadNo
                                   %_rSwdmxWmvInfo.u2WmaPlCntPerPkt);
                if (u4DmxAvailSize >= u4PayloadSz * u2PlToBeMoved)
                {
                    /* DMX available size is enough */
                    fgDmxRoomEnough = TRUE;
                    LOG(5, "dmx ok 0x%x, PlSz %d, cnt %d\n", u4DmxAvailSize, u4PayloadSz, u2PlToBeMoved);
                }
                else
                {
                    LOG(5, "dmx full 0x%x, PlSz %d, cnt %d\n", u4DmxAvailSize, u4PayloadSz, u2PlToBeMoved);
                }
                if (fgDmxRoomEnough)
                {
                    LOG(9, "Move the wma data! pts %x(%d.%03d)\n", rDmxMMData.u4Pts,PTS_S(rDmxMMData.u4Pts),PTS_MS(rDmxMMData.u4Pts));
                    if(rDmxMMData.u4FrameSize > 18)
                    {
                        LOG(9,"Packet no(%d)[WMA]The send data first 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            *((UINT8*)rDmxMMData.u4StartAddr + 0),
                            *((UINT8*)rDmxMMData.u4StartAddr + 1),
                            *((UINT8*)rDmxMMData.u4StartAddr + 2),
                            *((UINT8*)rDmxMMData.u4StartAddr + 3),
                            *((UINT8*)rDmxMMData.u4StartAddr + 4),
                            *((UINT8*)rDmxMMData.u4StartAddr + 5),
                            *((UINT8*)rDmxMMData.u4StartAddr + 6),
                            *((UINT8*)rDmxMMData.u4StartAddr + 7));
                        LOG(9,"Packet no(%d)[WMA]The send data midlle 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 1),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 3),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 4),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 5),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 6),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 7));
                        
                        LOG(9,"Packet no(%d)[WMA]The send data last 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -8),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -7),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -6),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -5),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -4),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -3),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -1));
                    }
                    
                    if (FALSE == _WMVDmxMoveData(prSwdmxInst,
                                                 prSwdmxInst->u1SwdmxId,
                                                 u1Pidx, 
                                                 prDmxMMData, 
                                                 500))
                    {
                        LOG(0, "Demuxer fails to move audio data.\n");
                        return FALSE;
                    }
                    else
                    {
                        _rSwdmxWmvInfo.u4AudioObjSz = u4PayloadSz;
                        _rSwdmxWmvInfo.u4PrevAudPts = prDmxMMData->u4Pts;
                        LOG(5, "mv %08x\n", (prDmxMMData->u4Pts)/90+_rSwdmxWmvInfo.u4PreRoll);                            
                    }
                    _rSwdmxWmvInfo.u4CurPTS = prDmxMMData->u4Pts;//DTV00213648
                    u4RetryAudio = 0;
                    fgSendToDMX = TRUE;
                    _rSwdmxWmvInfo.fgBlock = FALSE;
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;
                }
                else
                {
                    if (u4RetryAudio >= u2WaitTimes)
                    {
                        _rSwdmxWmvInfo.fgBlock = TRUE;
                        if (_rSwdmxWmvInfo.u2PreviousSendPayloadNo > 0)
                        {
                            LOG(5, "fifo full old raw %08x(s:%d/%d)np\n", 
                                (_rSwdmxWmvInfo.u4PrevAudPts)/90
                                    +_rSwdmxWmvInfo.u4PreRoll,
                                _rSwdmxWmvInfo.u2SendPayloadNo,
                                _rSwdmxWmvInfo.u2PreviousSendPayloadNo); 
                            //x_thread_delay(20);//if no delay, maybe cause watchdog timeout
                            return FALSE;
                        }
                        LOG(5, "fifo full pts raw %08x(s:%d/%d)\n", 
                            (prDmxMMData->u4Pts)/90+_rSwdmxWmvInfo.u4PreRoll,
                            _rSwdmxWmvInfo.u2SendPayloadNo,
                            _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
                        _rSwdmxWmvInfo.u2PreviousSendPayloadNo = _rSwdmxWmvInfo.u2SendPayloadNo;
                        x_thread_delay(20);
                        if (_rSwdmxWmvInfo.fgAudIsReady)
                        {
                            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                            STC_StartStc(prSwdmxInst->u1StcId);
                            _rSwdmxWmvInfo.fgAudIsReady = FALSE;
                        }
                        prSwdmxInst->fgFifoFull=TRUE;
                        return FALSE;
                    }
                    u4RetryAudio++;
                    x_thread_delay(1);
                    fgSendToDMX = FALSE;
                }
            }
            else
            {
                if ((u4FileSize <= u4DmxAvailSize))
                {
    #ifdef WMV_CHECK_STARTCODE_BEFORE_MOVEDATA 
                    if(_rSwdmxWmvInfo.fgIsDRM && (_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4))
                    {   
                        if (DMX_PDDRM_Decrypt(u1Idx, &rDmxMMData) != TRUE)
                        {
                           LOG(0,"DMX_PDDRM_Decrypt Audio Fail !!!!!!!!\n");
                           return FALSE;
                        }                     
                    }
    #endif
                    /* if audio is AAC, send AAC frame header firstly */
                    if (_rSwdmxWmvInfo.eAudioType == ENUM_SWDMX_ADEC_AAC)
                    {
                        _SWDMX_WMVHandleAacHeader(prSwdmxInst, 
                                                  u1Idx, 
                                                  u4FileSize, 
                                                  rDmxMMData.u4Pts);
                    }

                    LOG(9, "move audio data pidx#%d 0x%x(%d.%03d)\n",
                        u1Idx,
                        rDmxMMData.u4Pts,
                        PTS_S(rDmxMMData.u4Pts),
                        PTS_MS(rDmxMMData.u4Pts));

                    #if 1
                    /* check audio PTS of WMV file */
                    {
                        if (0 != _rSwdmxWmvInfo.u4PrevAudPts 
                            && _rSwdmxWmvInfo.i4WMVPlaySpeed > 0)
                        {
                            if (rDmxMMData.u4Pts < _rSwdmxWmvInfo.u4PrevAudPts)
                            {
                                LOG(0, "!!!currpts(0x%x) < prepts(0x%x)\n",
                                    rDmxMMData.u4Pts,
                                    _rSwdmxWmvInfo.u4PrevAudPts);
                                LOG(0, "pkt#%d: %02x %02x %02x %02x %02x %02x %02x %02x\n",
                                    _rSwdmxWmvInfo.u4CurParsingPacketNo,
                                    PKTBYTE(0),PKTBYTE(1),PKTBYTE(2),PKTBYTE(3),
                                    PKTBYTE(4),PKTBYTE(5),PKTBYTE(6),PKTBYTE(7));
                                LOG(0, "pkt+ofst(%d): %02x %02x %02x %02x %02x %02x %02x %02x\n",
                                    u4CurPosition,
                                    PKTBYTE(u4CurPosition+0),PKTBYTE(u4CurPosition+1),
                                    PKTBYTE(u4CurPosition+2),PKTBYTE(u4CurPosition+3),
                                    PKTBYTE(u4CurPosition+4),PKTBYTE(u4CurPosition+5),
                                    PKTBYTE(u4CurPosition+6),PKTBYTE(u4CurPosition+7));
                                LOG(0, "drop above packet!\n");
                                return FALSE;
                            }
                        }
                    }
                    #endif

                    LOG(9, "Move the audio data! pts %x(%d.%03d)\n", rDmxMMData.u4Pts,PTS_S(rDmxMMData.u4Pts),PTS_MS(rDmxMMData.u4Pts));
                    if(rDmxMMData.u4FrameSize > 18)
                    {
                        LOG(9,"Packet no(%d)[A:%d]The send data first 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            _rSwdmxWmvInfo.u4AStreamId,
                            *((UINT8*)rDmxMMData.u4StartAddr + 0),
                            *((UINT8*)rDmxMMData.u4StartAddr + 1),
                            *((UINT8*)rDmxMMData.u4StartAddr + 2),
                            *((UINT8*)rDmxMMData.u4StartAddr + 3),
                            *((UINT8*)rDmxMMData.u4StartAddr + 4),
                            *((UINT8*)rDmxMMData.u4StartAddr + 5),
                            *((UINT8*)rDmxMMData.u4StartAddr + 6),
                            *((UINT8*)rDmxMMData.u4StartAddr + 7));
                        LOG(9,"Packet no(%d)[A:%d]The send data midlle 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            _rSwdmxWmvInfo.u4AStreamId,
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 1),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 3),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 4),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 5),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 6),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize/2 + 7));
                        
                        LOG(9,"Packet no(%d)[A:%d]The send data last 8 byte: %x %x %x %x %x %x %x %x \n",
                            _rSwdmxWmvInfo.u4CurParsingPacketNo,
                            _rSwdmxWmvInfo.u4AStreamId,
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -8),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -7),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -6),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -5),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -4),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -3),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -2),
                            *((UINT8*)rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize -1));
                    }
                    
                    if (_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,u1Idx, &rDmxMMData, 500) != TRUE)
                    {
                        LOG(0, "Demuxer fails to move audio data.\n");
                        return FALSE;
                    }
                    else
                    {
                        _rSwdmxWmvInfo.u4AudioObjSz = u4FileSize;
                        _rSwdmxWmvInfo.u4PrevAudPts = rDmxMMData.u4Pts;
                    }
                    //printf("Audio pkt pts = %d (%d sec)\n", rDmxMMData.u4Pts, rDmxMMData.u4Pts / 90000);
    #ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
                    if (prSwdmxInst->fgIsBadInterleaved)
                    {
                        if (_WMV_AFifo_Fullness(prSwdmxInst, _u4BIAThreshold))
                        {
                            UINT32 u4Msg = 0;
                            x_msg_q_send(_hCommBIQ, &u4Msg, sizeof(UINT32), 1);
                        }
                    }
    #endif
    #ifdef WMV_BAD_INTLV_DEBUG
                    _u4AFifoAccuLen += u4FileSize;
                    vShowMoveStatus(prSwdmxInst, u4FileSize, rDmxMMData.u4StartAddr, FALSE);
    #endif
                    if (_rSwdmxWmvInfo.u2PreviousSendPayloadNo >0) {
                        LOG(6,"audio start at packet %d , payload %d\n",_rSwdmxWmvInfo.u4CurParsingPacketNo,_rSwdmxWmvInfo.u2SendPayloadNo);
                        x_memcpy(&u4Data,(void*)(VIRTUAL(u4CurPosition+u4StartAddress)),4);
                        LOG(6,"audio start again , first 4 bytes : %x\n",u4Data);
                    }
                    _rSwdmxWmvInfo.u4CurPTS = rDmxMMData.u4Pts;//DTV00213648
                    u4RetryAudio =0;
                    fgSendToDMX = TRUE;
                    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
                    {
                    	_rSwdmxWmvInfo.fgBlock = FALSE;
                    	_rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;
                    }
                    //LOG(0,"swdmx send audio pes : %x\n",rDmxMMData.u4Pts);
                }
                else
                {
                    if (u4RetryAudio >= u2WaitTimes)
                    {
                        _rSwdmxWmvInfo.fgBlock = TRUE;
                        _rSwdmxWmvInfo.u2PreviousSendPayloadNo = _rSwdmxWmvInfo.u2SendPayloadNo;
                        LOG(7, "audio(pid:%d) fifo full, pts=0x%x (%d.%03d)\n",
                            _rSwdmxWmvInfo.u1AudPid,
                            rDmxMMData.u4Pts,
                            PTS_S(rDmxMMData.u4Pts),
                            PTS_MS(rDmxMMData.u4Pts));
                        x_thread_delay(20);
                        if(_rSwdmxWmvInfo.fgAudIsReady)
                        {
                            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
                            STC_StartStc(prSwdmxInst->u1StcId);
                            _rSwdmxWmvInfo.fgAudIsReady = FALSE;
                        }
                        //x_memcpy(&u4Data,(void*)(VIRTUAL(u4CurPosition+u4StartAddress)),4);
                        //LOG(6,"audio payload size : %d/%d \n",u4FileSize,u4DmxAvailSize);
                        //LOG(6,"audio stop at packet %d , payload %d\n",_rSwdmxWmvInfo.u4CurParsingPacketNo,_rSwdmxWmvInfo.u2SendPayloadNo);
                        //LOG(6,"audio stop at first 4 bytes : %x\n",u4Data);
    #ifdef WMV_BAD_INTLV_SUPPORT
                        if (_fgEnBadIntlv)
                        {
                            _WMVCheckBadIntlv(prSwdmxInst, FALSE);
                        }
    #endif
                        prSwdmxInst->fgFifoFull=TRUE;
                        return FALSE;
                    }
                    u4RetryAudio++;
                    x_thread_delay(1);
                    fgSendToDMX = FALSE;
                }
                
            }
        } while (!fgSendToDMX);
        _rSwdmxWmvInfo.u4AudioCount++;
        _rSwdmxWmvInfo.u4InvalidStreamNumCount = 0;
        //u4AudioCount++;
#endif
    }
    else if(_rSwdmxWmvInfo.fgEnAudio && !_rSwdmxWmvInfo.fgChangeAudTrack )
    {
        // for DTV00367006, if continuous invalid stream number, the file might be broken.
        if ((prPayLoadHeader->u1StreamNum == 0) || (prPayLoadHeader->u1StreamNum > WMV_MAX_AUDIO_TRACK - 1) )
        {
            _rSwdmxWmvInfo.u4InvalidStreamNumCount++;
            if (_rSwdmxWmvInfo.u4InvalidStreamNumCount > WMV_INVALID_STREAM_NUM_THREAHOLD)
            {
                LOG(0, "File is broken: invalid stream number: %d\n", prPayLoadHeader->u1StreamNum);
                _rSwdmxWmvInfo.fgBrokenFile = TRUE;
                return FALSE;
            }
        }
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        else
        {
            u1Idx = _rSwdmxWmvInfo.u1AudPreBufPid[ASID(prPayLoadHeader->u1StreamNum)];
            LOG(7, "idx(%d) = u1AudPreBufPid[asid:%d(payload:%d)]\n",
                u1Idx,
                ASID(prPayLoadHeader->u1StreamNum),
                prPayLoadHeader->u1StreamNum);
            
            //u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_AUDIO,_rSwdmxWmvInfo.u1AudPid);
            if (_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,u1Idx, &rDmxMMData, 500) != TRUE)
            {
                LOG(0, "Demuxer fails to move audio data.\n");
                return FALSE;
            }
            else
            {
                LOG(7, "move multi audio data 0x%x (%d.%03d)\n",
                    rDmxMMData.u4Pts,
                    PTS_S(rDmxMMData.u4Pts),
                    PTS_MS(rDmxMMData.u4Pts));
            }
            
            _rSwdmxWmvInfo.u4InvalidStreamNumCount = 0;
        }
        #else
        LOG(7, "got audio data aud_strm_num#%d 0x%x(%d.%03d)\n",
                    prPayLoadHeader->u1StreamNum,
                    rDmxMMData.u4Pts,
                    PTS_S(rDmxMMData.u4Pts),
                    PTS_MS(rDmxMMData.u4Pts));
        #endif
    }

    return TRUE;
}


static BOOL _WMVParrsingMultiPayload(SWDMX_INFO_T *prSwdmxInst, UINT32 u4ParsingCount,PayloadParseInfo* prPayLoadInfo,UINT32 u4PacketStartAddr,UINT32 u4CurrentPos)
{
    //UINT32 u4PayloadSize=0;
    //MultiPayloadInfo rMultiPayloadInfo;

    UINT8 u1Tmp=0;
    UINT8 index=0;
    BOOL fgRet = TRUE;

    x_memset(&rMultiPayloadInfo,0,sizeof(MultiPayloadInfo));

    //u4PayloadSize = (prPayLoadInfo->u4PacketLength) - u4ParsingCount - (prPayLoadInfo->u4PaddingLength);
    x_memcpy(&u1Tmp,((VOID*)VIRTUAL(u4PacketStartAddr+u4CurrentPos)),1);
    u4CurrentPos += 1;
    rMultiPayloadInfo.u1PayloadsNumber = u1Tmp &0x3F;
    rMultiPayloadInfo.u1PayloadLengthType = (u1Tmp>>6) & 0x3;
    rMultiPayloadInfo.u4FilePos = u4CurrentPos;
    //LOG(0,"number of payloads : %d\n",rMultiPayloadInfo.u1PayloadsNumber);
    //do {
    for (index=0 ; index<rMultiPayloadInfo.u1PayloadsNumber ; index++) {
        rMultiPayloadInfo.u1CurPayloadIdx = index;
        fgRet = _WMVParsingPayload(prSwdmxInst, u4ParsingCount,prPayLoadInfo,u4PacketStartAddr,rMultiPayloadInfo.u4FilePos,&rMultiPayloadInfo);
        //fgRet = WMVParsingPayload(rMultiPayloadInfo.u4FilePos,prPayLoadInfo,u4PacketStartAddr,rMultiPayloadInfo.u4FilePos,&rMultiPayloadInfo);
        
        if (fgRet == FALSE)
        {
#ifdef WMV_BAD_INTLV_SUPPORT
            if (_rBadIntlv.eState == BAD_INTLV_STT_NONE || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V)
            {
                return FALSE;  // return due to FIFO being blocked
            }
            else if (_rBadIntlv.fgRecord)
            {
                FEEDER_REQ_DATA_T *prFdr = &_rFeederReqData;
                
                _rBadIntlv.fgRecord = FALSE;
                if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V)
                {
                    _rBadIntlv.u4BeginSkipPktNo = _rSwdmxWmvInfo.u4CurParsingPacketNo;
                    _rBadIntlv.u4BeginSkipPldNo = index;
                    _rBadIntlv.u8BeginSkipPktOfst = prFdr->u8FilePos - prFdr->u4ReadSize + prFdr->u4AlignOffset;
                    // do not return because the latter payloads may be what we want to patch!
                }
                else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_V || _rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_A)
                {
                    _rBadIntlv.u4EndPatchPktNo = _rSwdmxWmvInfo.u4CurParsingPacketNo;
                    _rBadIntlv.u4EndPatchPldNo = index;
                    return FALSE;  // return due to FIFO being blocked and we have to reparse another FIFO immediately
                }
            }
#else
            return FALSE;
#endif
        }
    }
    return TRUE;
    //}while(rMultiPayloadInfo.u4FilePos < u4PayloadSize);
}

BOOL _WMVParsingPayload(SWDMX_INFO_T *prSwdmxInst, 
                        UINT32 u4ParsingCount,
                        PayloadParseInfo* prPayLoadInfo,
                        UINT32 u4PacketStartAddr,
                        UINT32 u4CurrentPos,
                        MultiPayloadInfo* prMultiPayloadInfo)
{
    //PayloadHeader rPayLoadHeader;
    BOOL fgMoveDataRet = FALSE;
    UINT8 u1Tmp =0;
    UINT32 u4PayloadLength =0;
    //UINT32 u4ParsingCount =0;
    x_memset(&rPayLoadHeader,0,sizeof(PayloadHeader));

    //ASSERT(((u4PacketStartAddr+u4CurrentPos)&(0xF0000000)) ==0 );


    //ASSERT((u4PacketStartAddr+u4CurrentPos) < _rFeederInfo.u4EndAddr);

    //LOG(0,"address : %x\n",u4PacketStartAddr+u4CurrentPos);
#ifdef WMV_ERROR_HANDLE_ON
    if ((u4CurrentPos& 0xFF000000) != 0)
    {
        _rSwdmxWmvInfo.fgBrokenFile = TRUE;
        LOG(0,"\n\n!!!!!!!!!!!!!!!! Inst#%d: The file is broken1 "
              "(%02x %02x %02x %02x)"
              "(%02x %02x %02x %02x)"
              "\n\n\n", 
            prSwdmxInst->u1SwdmxId,
            *((UINT8*)u4PacketStartAddr+0),
            *((UINT8*)u4PacketStartAddr+1),
            *((UINT8*)u4PacketStartAddr+2),
            *((UINT8*)u4PacketStartAddr+3),
            *((UINT8*)u4PacketStartAddr+4),
            *((UINT8*)u4PacketStartAddr+5),
            *((UINT8*)u4PacketStartAddr+6),
            *((UINT8*)u4PacketStartAddr+7));
        return FALSE;
    }
#endif
    //media stream number , there are multiple stream in packet , it represents for multi video stream or audio stream
    x_memcpy(&u1Tmp,((VOID*)VIRTUAL(u4PacketStartAddr+u4CurrentPos)),1);
    //LOG(0,"u1Tmp : %x\n",u1Tmp);
    //LOG(0,"payload index : %x\n",u4CurrentPos);
    rPayLoadHeader.u1StreamNum = u1Tmp &0x7F;
    rPayLoadHeader.fgKeyFrame = (u1Tmp>>7) & 0x1;
    u4CurrentPos += 1;
    u4ParsingCount += 1;

    //LOG(0,"u1StreamNum : %d\n",rPayLoadHeader.u1StreamNum);

    //media object number , it represent vidoe/audio frame no.
    if (prPayLoadInfo->u1MediaObjectNumType != 0) {
        x_memcpy(&rPayLoadHeader.u4MediaObjNum,(VOID*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((prPayLoadInfo->u1MediaObjectNumType)));
        u4CurrentPos += TypeTransform((prPayLoadInfo->u1MediaObjectNumType));
        u4ParsingCount += TypeTransform((prPayLoadInfo->u1MediaObjectNumType));
        //LOG(0,"u4MediaObjNum : %d\n",rPayLoadHeader.u4MediaObjNum);
    }
    else 
    {
        //ASSERT((prPayLoadInfo->u1MediaObjectNumType));
    }
    //OffsetInto , it represent the offset of corresponding position in that video/audio object
    if (prPayLoadInfo->u1OffsetIntoType != 0) {
        x_memcpy(&rPayLoadHeader.u4OffsetIntoMediaObj,(VOID*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((prPayLoadInfo->u1OffsetIntoType)));
        u4CurrentPos += TypeTransform((prPayLoadInfo->u1OffsetIntoType));
        u4ParsingCount += TypeTransform((prPayLoadInfo->u1OffsetIntoType));
        //LOG(0,"u4OffsetIntoMediaObj : %d\n",rPayLoadHeader.u4OffsetIntoMediaObj);
    }
    else 
    {
        //ASSERT((prPayLoadInfo->u1OffsetIntoType));
    }
    // , it represent the offset of corresponding position in that video/audio object
    if (prPayLoadInfo->u1ReplicatedDataType!= 0) {
        x_memcpy(&rPayLoadHeader.u4ReplicatedDataLen,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((prPayLoadInfo->u1ReplicatedDataType)));
        u4CurrentPos += TypeTransform((prPayLoadInfo->u1ReplicatedDataType));
        u4ParsingCount += TypeTransform((prPayLoadInfo->u1ReplicatedDataType));
        //LOG(0,"u4ReplicatedDataLen : %d\n",rPayLoadHeader.u4ReplicatedDataLen);
    }
    else 
    {
        //ASSERT((prPayLoadInfo->u1ReplicatedDataType));
    }
    //parsing single payload
    if (rPayLoadHeader.u4ReplicatedDataLen != 1) {
        //parsing replicated data (PTS and media object size)
        x_memcpy(&rPayLoadHeader.u4MediaObjSize,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),4);
        u4CurrentPos += 4;
        u4ParsingCount += 4;
        x_memcpy(&rPayLoadHeader.u4PTS,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),4);
        if (prSwdmxInst->fgFifoFull)
        {
            LOG(9, "raw %02x%02x%02x%02x(s:%d/%d)\n",
                PKTBYTE(u4CurrentPos+3),PKTBYTE(u4CurrentPos+2),
                PKTBYTE(u4CurrentPos+1),PKTBYTE(u4CurrentPos+0),
                _rSwdmxWmvInfo.u2SendPayloadNo+1,
                _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
        }
        u4CurrentPos += 4;
        u4ParsingCount +=4;
        //adding sample id info
        if ((rPayLoadHeader.u4ReplicatedDataLen >= 16) && (_rSwdmxWmvInfo.fgIsDRM))
        {
            UINT32 u4Offset = rPayLoadHeader.u4ReplicatedDataLen -16;
            x_memcpy(&_u8SampleId,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos+u4Offset),8);
        }
        u4CurrentPos += rPayLoadHeader.u4ReplicatedDataLen - 8;
        u4ParsingCount += rPayLoadHeader.u4ReplicatedDataLen - 8;
        //parsing payload data
        if (prMultiPayloadInfo != NULL) {
            //spork , error handling
#ifdef WMV_ERROR_HANDLE_ON
            if ( (u4CurrentPos& 0xFF000000) != 0)
            {
                _rSwdmxWmvInfo.fgBrokenFile = TRUE;
                LOG(0,"\n\n!!!!!!!!!!!!!!!! Inst#%d: The file is broken2 "
                      "(%02x %02x %02x %02x)"
                      "(%02x %02x %02x %02x)"
                      "\n\n\n", 
                    prSwdmxInst->u1SwdmxId,
                    *((UINT8*)u4PacketStartAddr+0),
                    *((UINT8*)u4PacketStartAddr+1),
                    *((UINT8*)u4PacketStartAddr+2),
                    *((UINT8*)u4PacketStartAddr+3),
                    *((UINT8*)u4PacketStartAddr+4),
                    *((UINT8*)u4PacketStartAddr+5),
                    *((UINT8*)u4PacketStartAddr+6),
                    *((UINT8*)u4PacketStartAddr+7));
                return FALSE;
            }
#endif
            x_memcpy(&u4PayloadLength,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((prMultiPayloadInfo->u1PayloadLengthType)));
            u4CurrentPos += TypeTransform((prMultiPayloadInfo->u1PayloadLengthType));
            prMultiPayloadInfo ->u4FilePos = u4CurrentPos+u4PayloadLength;
            /*if(((prMultiPayloadInfo ->u4FilePos)&(0xF0000000)) !=0 ) {
                _rSwdmxWmvInfo.u4FilePos = prMultiPayloadInfo ->u4FilePos;
                return FALSE;
            }*/
            //LOG(0,"filepos : %x ,multi payload length : %d %d\n",prMultiPayloadInfo ->u4FilePos,u4PayloadLength,(prMultiPayloadInfo->u1PayloadLengthType));
#ifdef WMV_BAD_INTLV_SUPPORT
            if (_rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
            {
                if ((_rSwdmxWmvInfo.u4CurParsingPacketNo < _rBadIntlv.u4BeginSkipPktNo) ||
                    (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4BeginSkipPktNo && prMultiPayloadInfo->u1CurPayloadIdx < _rBadIntlv.u4BeginSkipPldNo))
                {
                    return TRUE;
                }
                if (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4EndPatchPktNo &&
                    prMultiPayloadInfo->u1CurPayloadIdx == _rBadIntlv.u4EndPatchPldNo)
                {
                    _WMVEndReparse(prSwdmxInst);
                }
            }
#endif
        }
        else 
        {
            u4PayloadLength = prPayLoadInfo->u4PacketLength - u4ParsingCount - (prPayLoadInfo->u4PaddingLength);
            //LOG(0,"payload size : %x %x %x %x %x\n",u4PayloadLength,prPayLoadInfo->u4PacketLength,u4CurrentPos,u4ParsingCount,prPayLoadInfo->u4PaddingLength);
#ifdef WMV_BAD_INTLV_SUPPORT
            if (_rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
            {
                if (_rSwdmxWmvInfo.u4CurParsingPacketNo < _rBadIntlv.u4BeginSkipPktNo)
                {
                    return TRUE;
                }
                if (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4EndPatchPktNo)
                {
                    _WMVEndReparse(prSwdmxInst);
                }
            }
#endif
        }
        //u4PayloadLength = prPayLoadInfo->u4PacketLength - u4ParsingCount - (prPayLoadInfo->u4PaddingLength);
        //printf("payload size : %x %x %x %x %x\n",u4PayloadLength,prPayLoadInfo->u4PacketLength,u4CurrentPos,u4ParsingCount,prPayLoadInfo->u4PaddingLength);
        /*
        if((rPayLoadHeader.u4OffsetIntoMediaObj + u4PayloadLength) == rPayLoadHeader.u4MediaObjSize) {
            DMAMove(u4PacketStartAddr,u4CurrentPos,u4PayloadLength,&rPayLoadHeader,TRUE);
        }
        else {
            DMAMove(u4PacketStartAddr,u4CurrentPos,u4PayloadLength,&rPayLoadHeader,FALSE);
        }
        */
        /* why use following IF block?(index pts) */
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
           if(_rSwdmxWmvInfo.i4WMVPlaySpeed<0&&(!_rSwdmxWmvInfo.fgAudioNormalPlay))
           {
               if(_rSwdmxWmvInfo.u4IndexPts==0)
               {
                 _rSwdmxWmvInfo.u4IndexPts=rPayLoadHeader.u4PTS;
               }
               else
               {
                   if(_rSwdmxWmvInfo.u4IndexPts>_rSwdmxWmvInfo.u4PreRoll)
                   {
                        _rSwdmxWmvInfo.u4IndexPts--;
                   }
               }
               rPayLoadHeader.u4PTS=_rSwdmxWmvInfo.u4IndexPts;
           }
        }
        
        if (rPayLoadHeader.u4OffsetIntoMediaObj == 0)
        {
            fgMoveDataRet = _WMVMoveData(prSwdmxInst, 
                                         u4PacketStartAddr,
                                         u4CurrentPos,
                                         u4PayloadLength,
                                         &rPayLoadHeader,
                                         TRUE);
        }
        else
        {
            fgMoveDataRet = _WMVMoveData(prSwdmxInst, 
                                         u4PacketStartAddr,
                                         u4CurrentPos,
                                         u4PayloadLength,
                                         &rPayLoadHeader,
                                         FALSE);
        }
        /* why use following IF block?(index pts) */
        if (_rSwdmxWmvInfo.fgBlock)
        {
            if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
              if(_rSwdmxWmvInfo.u4IndexPts && _rSwdmxWmvInfo.i4WMVPlaySpeed<0)
              {
                 _rSwdmxWmvInfo.u4IndexPts++;
              }
            }
            return FALSE;
        }
        /*if((rPayLoadHeader.u4OffsetIntoMediaObj + u4PayloadLength) == rPayLoadHeader.u4MediaObjSize)
            SendES();
        else
            ;//for lint free
        */
    }
    //parsing single payload with compressed payload data
    else 
    {
        UINT32 u4ParsingSize =0;
        rPayLoadHeader.u4PTS = rPayLoadHeader.u4OffsetIntoMediaObj; //in compressed payload , offset into media object field represent PTS
        x_memcpy(&rPayLoadHeader.u4PTSDelta,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
        u4CurrentPos += 1;
        u4ParsingCount += 1;
        if (prMultiPayloadInfo != NULL)
        {
            x_memcpy(&u4PayloadLength,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((prMultiPayloadInfo->u1PayloadLengthType)));
            u4CurrentPos += TypeTransform((prMultiPayloadInfo->u1PayloadLengthType));
            prMultiPayloadInfo ->u4FilePos = u4CurrentPos+u4PayloadLength;
#ifdef WMV_BAD_INTLV_SUPPORT
            if (_rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
            {
                if ((_rSwdmxWmvInfo.u4CurParsingPacketNo < _rBadIntlv.u4BeginSkipPktNo) ||
                    (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4BeginSkipPktNo && prMultiPayloadInfo->u1CurPayloadIdx < _rBadIntlv.u4BeginSkipPldNo))
                {
                    return TRUE;
                }
                if (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4EndPatchPktNo &&
                    prMultiPayloadInfo->u1CurPayloadIdx == _rBadIntlv.u4EndPatchPldNo)
                {
                    _WMVEndReparse(prSwdmxInst);
                }
            }
#endif
        }
        else
        {
            u4PayloadLength = prPayLoadInfo->u4PacketLength - u4ParsingCount - (prPayLoadInfo->u4PaddingLength);
#ifdef WMV_BAD_INTLV_SUPPORT
            if (_rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
            {
                if (_rSwdmxWmvInfo.u4CurParsingPacketNo < _rBadIntlv.u4BeginSkipPktNo)
                {
                    return TRUE;
                }
                if (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rBadIntlv.u4EndPatchPktNo)
                {
                    _WMVEndReparse(prSwdmxInst);
                }
            }
#endif
        }
        do {
            UINT8 u1SubPayloadLen =0;
            x_memcpy(&u1SubPayloadLen,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
            u4CurrentPos += 1;
            fgMoveDataRet = _WMVMoveData(prSwdmxInst, u4PacketStartAddr,u4CurrentPos,u1SubPayloadLen,&rPayLoadHeader,TRUE);
            if (_rSwdmxWmvInfo.fgBlock)
            {
#ifdef WMV_BAD_INTLV_SUPPORT
                ASSERT(0);  // !!! ToDo: find a real case to trace and figure out the solution
#endif
                return FALSE;
            }
            //SendES(); //for compressed payload data, every payload represent one media object
            u4CurrentPos += u1SubPayloadLen;
            u4ParsingSize += u1SubPayloadLen + 1;
        } while (u4ParsingSize < u4PayloadLength);
    }
    return fgMoveDataRet;
}

BOOL _WMVParsingPacket(
    SWDMX_INFO_T *prSwdmxInst, 
    UINT32 u4PacketStartAddr, 
    UINT32 u4CurrentPos, 
    UINT32 u4PacketSize)
{
    //UINT32 u4CurrentPos = 0;
    //UINT64 u8TotalDataPackets=0;
    //UINT32 index=0;
    UINT32 u4ParsingCount = 0;
    BOOL fgRet;

    //x_memcpy(&u8TotalDataPackets,u4PacketStartAddr+u4CurrentPos+40,8);
    //LOG(0,"total data packet : %d \n",u8TotalDataPackets);
    //u4CurrentPos += WMV_DATA_OBJ_HEADER; //packet header size is 50 bytes

    //LOG(0,"packet start pos : %x\n",u4CurrentPos);

    //u4Address = u4CurrentPos;

    //handle other than payload paring
    //do {
    UINT8 u1Tmp =0;
    if (_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
    {
#ifndef CC_NETFLIX_CMPB_SUPPORT    
        UINT8 u1IdxOfPraingBitstream=0;
        UINT8 u1PacketType =0;
        UINT8 u1EOS =0;
        UINT64 u8Remainder =0;
        x_memcpy(&u1IdxOfPraingBitstream,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
        u4CurrentPos +=1;
        x_memcpy(&u1PacketType,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
        u4CurrentPos +=1;
        x_memcpy(&u1EOS,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
        u4CurrentPos +=1;

        //reserved bytes
        u4CurrentPos +=1;

        if (u1EOS == TRUE)
        {   
            LOG(2,"Send Eos 1\n");
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return TRUE;
        }
        _rSwdmxWmvInfo.u4VStreamId = _u4NetflixVideoStreamId;
        _rSwdmxWmvInfo.u4AStreamId = _u4NetflixAudioStreamId;
        _rSwdmxWmvInfo.u4PreRoll = (UINT32)u8Div6432(_prWmvGenericInfo[u1IdxOfPraingBitstream].ui8_preroll, 90, &u8Remainder);
        _rSwdmxWmvInfo.u4PacketSize = (UINT32)_prWmvGenericInfo[u1IdxOfPraingBitstream].ui4_sz_max_acces_unt;
        //_prRangeInfo->ui4_sz_min_acces_unt = _rSwdmxWmvInfo.u4PacketSize;

        if (u1PacketType == NETFLIX_VIDEO) //video
        {
            _rSwdmxWmvInfo.u4AStreamId = 0xFF;
        }
        else if (u1PacketType == NETFLIX_AUDIO) //audio
        {
            _rSwdmxWmvInfo.u4VStreamId = 0xFF;
        }
#else //netflix cmpb mode

        static NetflixEmbeddedCmd_T _rNetflixEmbeddedCmd;
        x_memset(&_rNetflixEmbeddedCmd,0,sizeof(NetflixEmbeddedCmd_T));
        x_memcpy(&_rNetflixEmbeddedCmd,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),sizeof(NetflixEmbeddedCmd_T));

        u4CurrentPos += sizeof(NetflixEmbeddedCmd_T);

        if (_rNetflixEmbeddedCmd.fgEos )
        {
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return TRUE;
        }
        

        _rSwdmxWmvInfo.u4VStreamId = _u4NetflixVideoStreamId;
        _rSwdmxWmvInfo.u4AStreamId = _u4NetflixAudioStreamId;
        
        _rSwdmxWmvInfo.u4PacketSize = (UINT32)_rNetflixEmbeddedCmd.u2PacketSize;
        //_prRangeInfo->ui4_sz_min_acces_unt = _rSwdmxWmvInfo.u4PacketSize;
        
        _rSwdmxWmvInfo.u4PreRoll = 0; //netflix spec , pts preroll always = 0

        if (_rNetflixEmbeddedCmd.u1PacketType == NETFLIX_VIDEO) //video
        {
            _rSwdmxWmvInfo.u4AStreamId = 0xFF;
        }
        else if (_rNetflixEmbeddedCmd.u1PacketType == NETFLIX_AUDIO) //audio
        {
            _rSwdmxWmvInfo.u4VStreamId = 0xFF;
        }          
#endif
    }

    x_memcpy(&u1Tmp,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
    //LOG(0,"u1Tmp : %x\n",u1Tmp);
    //LOG(0,"index : %x\n",u4CurrentPos);
    //u4CurrentPos += 1;
    u4ParsingCount = 0;
#ifdef WMA_SEEK_OFFSET
    {
      x_memcpy((void *)(&_rSwdmxWmvInfo.u8PacketMarker),(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),5);
    }
#endif

    if (u1Tmp&0x80) //parsing error correction data
    {
        
#ifdef WMA_SEEK_OFFSET
      _rSwdmxWmvInfo.u4PacketMarkerLen=5;
#endif
        //UINT8 u1ErrorCorrectFlag;
        //UINT8 u1ErrorCorrectionDataLen=0;
        //UINT8 u1ErrorCorrectionLenType=0;
        //UINT8 u1ErrorCorrectionPresent=0;
        //parsing Error Correction Data
        //u1ErrorCorrectionDataLen = (u1Tmp&0xF);
        //u1ErrorCorrectionLenType = (u1Tmp>>5)&0x3;
        //u1ErrorCorrectionPresent = (u1Tmp>>7)&0x1;
        u4CurrentPos += 1;
        u4CurrentPos += 2;
        u4ParsingCount += 3;
    }
    else
    {
#ifdef WMA_SEEK_OFFSET
        _rSwdmxWmvInfo.u4PacketMarkerLen=2;
#endif

    }
    //else //parsing payload information
    {
        //UINT8 u1Length=0;

        //PayloadParseInfo rPayLoadInfo;

        x_memset(&rPayLoadInfo,0,sizeof(PayloadParseInfo));
        //LOG(0,"index : %x\n",u4CurrentPos);
        x_memcpy(&u1Tmp,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
        //LOG(0,"u1Tmp : %x\n",u1Tmp);
        u4CurrentPos += 1;
        u4ParsingCount += 1;
        rPayLoadInfo.fgMultiplePayload = u1Tmp & 0x1;
        rPayLoadInfo.u1SequenceType= (u1Tmp>>1) & 0x3;
        rPayLoadInfo.u1PaddingLenghtType = (u1Tmp>>3) & 0x3;
        rPayLoadInfo.u1PacketLengthType = (u1Tmp>>5) & 0x3;
        rPayLoadInfo.fgErrorCorrectPresent = (u1Tmp>>7) & 0x1;

        x_memcpy(&u1Tmp,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),1);
        //LOG(0,"u1Tmp : %x\n",u1Tmp);
        u4CurrentPos += 1;
        u4ParsingCount += 1;
        rPayLoadInfo.u1ReplicatedDataType = (u1Tmp)&0x3;
        rPayLoadInfo.u1OffsetIntoType = (u1Tmp>>2)&0x3;
        rPayLoadInfo.u1MediaObjectNumType = (u1Tmp>>4)&0x3;
        rPayLoadInfo.u1StreamNumberType = (u1Tmp>>6)&0x3;

        if (rPayLoadInfo.u1PacketLengthType != 0)
        { //packet length
            x_memcpy(&rPayLoadInfo.u4PacketLength,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((rPayLoadInfo.u1PacketLengthType)));
            u4CurrentPos += TypeTransform((rPayLoadInfo.u1PacketLengthType));
            u4ParsingCount += TypeTransform((rPayLoadInfo.u1PacketLengthType));
            //LOG(0,"packet length : %d %d\n",rPayLoadInfo.u4PacketLength,TypeTransform((rPayLoadInfo.u1PacketLengthType)));
        }
        else 
        {
            rPayLoadInfo.u4PacketLength = _rSwdmxWmvInfo.u4PacketSize; //get form upper middleware
        }
        if (rPayLoadInfo.u1SequenceType != 0) 
        { //sequence length
            x_memcpy(&rPayLoadInfo.u4Sequence,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((rPayLoadInfo.u1SequenceType)));
            u4CurrentPos += TypeTransform((rPayLoadInfo.u1SequenceType));
            u4ParsingCount += TypeTransform((rPayLoadInfo.u1SequenceType));
            //LOG(0,"sequence length : %d %d\n",rPayLoadInfo.u4Sequence,TypeTransform((rPayLoadInfo.u1SequenceType)));
        }
        else 
        {
            rPayLoadInfo.u4Sequence = 0;
        }
        if (rPayLoadInfo.u1PaddingLenghtType != 0) 
        { //padding length
            x_memcpy(&rPayLoadInfo.u4PaddingLength,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),TypeTransform((rPayLoadInfo.u1PaddingLenghtType)));
            u4CurrentPos += TypeTransform((rPayLoadInfo.u1PaddingLenghtType));
            u4ParsingCount += TypeTransform((rPayLoadInfo.u1PaddingLenghtType));
            //LOG(0,"padding length : %d %d\n",rPayLoadInfo.u4PaddingLength,TypeTransform((rPayLoadInfo.u1PaddingLenghtType)));
        }
        else 
        {
            rPayLoadInfo.u4PaddingLength = 0;
        }
        x_memcpy(&rPayLoadInfo.u4SendTime,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),4);

        u4CurrentPos += 4;
        u4ParsingCount += 4;
        x_memcpy(&rPayLoadInfo.u2Duration,(void*)VIRTUAL(u4PacketStartAddr+u4CurrentPos),2);
        u4CurrentPos += 2;
        u4ParsingCount += 2;
        //LOG(0,"index : %x\n",u4CurrentPos);
        //LOG(0,"u4ParsingCount : %x\n",u4ParsingCount);
        if (rPayLoadInfo.fgMultiplePayload)
        {
            fgRet = _WMVParrsingMultiPayload(prSwdmxInst, u4ParsingCount,&rPayLoadInfo,u4PacketStartAddr,u4CurrentPos);
        }
        else {
            rMultiPayloadInfo.u1CurPayloadIdx = 0;
            fgRet = _WMVParsingPayload(prSwdmxInst, u4ParsingCount,&rPayLoadInfo,u4PacketStartAddr,u4CurrentPos,NULL);
#ifdef WMV_BAD_INTLV_SUPPORT
            if (!fgRet && _rBadIntlv.eState != BAD_INTLV_STT_NONE)
            {
                if (_rBadIntlv.fgRecord)
                {
                    FEEDER_REQ_DATA_T *prFdr = &_rFeederReqData;
                                        
                    _rBadIntlv.fgRecord = FALSE;
                    if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V)
                    {
                        _rBadIntlv.u4BeginSkipPktNo = _rSwdmxWmvInfo.u4CurParsingPacketNo;
                        _rBadIntlv.u4BeginSkipPldNo = 0;
                        _rBadIntlv.u8BeginSkipPktOfst = prFdr->u8FilePos - prFdr->u4ReadSize + prFdr->u4AlignOffset;
                    }
                    else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_V || _rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_A)
                    {
                        _rBadIntlv.u4EndPatchPktNo = _rSwdmxWmvInfo.u4CurParsingPacketNo;
                        _rBadIntlv.u4EndPatchPldNo = 0;
                    }
                }
            }
#endif
        }
    }

    return fgRet;
}


VOID _WMVStartToPlay(SWDMX_INFO_T *prSwdmxInst)
{
    if (_rSwdmxWmvInfo.fgEnVideo)
    {
        VDP_SetPauseMm(prSwdmxInst->u1B2rId, FALSE);
    }

    switch (_rSwdmxWmvInfo.eVideoType)
    {
    case ENUM_SWDMX_VDEC_VC1:
    case ENUM_SWDMX_VDEC_WMV7:
    case ENUM_SWDMX_VDEC_WMV8:
    case ENUM_SWDMX_VDEC_WMV9:
        VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_WMV);
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
           VDEC_Play(prSwdmxInst->u1Vdec2Id, VDEC_FMT_WMV);
        }
        break;
    case ENUM_SWDMX_VDEC_MPEG4:
    case ENUM_SWDMX_VDEC_DX311:
    case ENUM_SWDMX_VDEC_H263:
        VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_MP4);
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
           VDEC_Play(prSwdmxInst->u1Vdec2Id, VDEC_FMT_MP4);
        }       
        break;
    case ENUM_SWDMX_VDEC_H264:
        VDEC_Play(prSwdmxInst->u1VdecId, VDEC_FMT_H264);
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
           VDEC_Play(prSwdmxInst->u1Vdec2Id, VDEC_FMT_H264);
        }
        break;
    default :
        //do nothing
        break;
    }
    _rSwdmxWmvInfo.fgVideoPause = FALSE;
    LOG(0,"_WMVStartToPlay start to play video\n");
}

BOOL _SWDMX_WmvAudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet = TRUE;
    SWDMX_INFO_T *prSwdmxInst = (SWDMX_INFO_T*)prPes->pvInstanceTag;
    DMX_AUDIO_PES_T rDmxPes;
    x_memcpy(&rDmxPes,prPes,sizeof(DMX_AUDIO_PES_T));
    
#if defined(CC_MT5365) || defined(CC_MT5395) || defined(SWDMX_IS_POST_MT5368)
    if(_rSwdmxWmvInfo.i4WMVPlaySpeed>0)
    {
        rDmxPes.fgForward=TRUE;
    }
    else
    {
       rDmxPes.fgForward=FALSE;
    }
#endif

    _fgReceiveAudioPes = TRUE;

    if (_fgReceiveAudioPes)
    {
        
        x_memcpy((VOID*)&_rPreviousAudioPes,(VOID*)(&rDmxPes),sizeof(DMX_AUDIO_PES_T));
    
        _rPreviousAudioPes.u4Pts = rDmxPes.u4Pts;
        fgRet =  AUD_SendAudioPes(&_rPreviousAudioPes);
        //if (rDmxPes.u1PidIndex == _rSwdmxWmvInfo.u1AudPid)
        {
            UINT16 u2VdecQueueSize = 0;
            UINT16 u2VdecMaxQueueSize = 0;
            VDEC_GetQueueInfo(prSwdmxInst->u1VdecId,&u2VdecQueueSize,&u2VdecMaxQueueSize);
//          if (_rPreviousAudioPes.u4Pts>15*90000
//              && _rPreviousAudioPes.u4Pts<19*90000)
            {
                LOG(7, "send audio(pid:%d) es 0x%x(%d.%03d) [vdecq:%d/%d][stc:%d]\n",
                    rDmxPes.u1PidIndex, 
                    _rPreviousAudioPes.u4Pts,
                    PTS_S(_rPreviousAudioPes.u4Pts),
                    PTS_MS(_rPreviousAudioPes.u4Pts),
                    u2VdecQueueSize,
                    u2VdecMaxQueueSize,
                    PTS_S(STC_GetStcValue(prSwdmxInst->u1StcId)));
            }
        }
        if (rDmxPes.fgEOS)
        {
            fgRet =  AUD_SendAudioPes(&rDmxPes);
            LOG(6, "send audio EOS 0x%x(%d.%03d)\n", 
                _rPreviousAudioPes.u4Pts,
                PTS_S(_rPreviousAudioPes.u4Pts),
                PTS_MS(_rPreviousAudioPes.u4Pts));
            AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
        }
    }
    else
    {
        
        if (rDmxPes.fgEOS)
        {
            fgRet =  AUD_SendAudioPes(&rDmxPes);
            AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
        }
    }
    
    return fgRet;
}
#if 0
VOID _WMVCheckDataExhausted(SWDMX_INFO_T *prSwdmxInst)
{
    UINT16 u2VdecQueueSize,u2VdecMaxQueueSize;
    u2VdecMaxQueueSize =0;
    u2VdecQueueSize =0;

    if(!MPV_IsDisplay(prSwdmxInst->u1VdecId))
    {
        return; //wait sequence header
    }

    VDEC_GetQueueInfo(prSwdmxInst->u1VdecId,&u2VdecQueueSize,&u2VdecMaxQueueSize);//maiyou
    if (u2VdecQueueSize < NETFLIX_UNDERFLOW_THRESHOLD && !_fgNetflixEos && !_fgDataExhausted &&
            _prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
    {
        LOG(0,"swdmx wmv netflix data exhausted 3\n");
        _fgDataExhausted = TRUE;
        FeederNotifyHandle(prSwdmxInst->eFeederSrc, FEEDER_NOTIFY_DATA_EXHAUSTED, 1);  // megaa 20090828
        _SWDMX_RANGE_CALLBACK(prSwdmxInst->rCurRangeReq.prDmxRangeInfo->pv_tag,
                                MM_RANGE_BUFF_UNDERFLOW,
                                prSwdmxInst->rCurRangeReq.prDmxRangeInfo->ui4_range_id,
                                0xFFFFFFFF,
                                0);        
    }
    else if (u2VdecQueueSize > (NETFLIX_UNDERFLOW_THRESHOLD<<1) && _fgDataExhausted)
    {
        LOG(0,"swdmx wmv netflix data exhausted 4\n");
        _SWDMX_RANGE_CALLBACK(prSwdmxInst->rCurRangeReq.prDmxRangeInfo->pv_tag,
                                MM_RANGE_BUFF_READY,
                                prSwdmxInst->rCurRangeReq.prDmxRangeInfo->ui4_range_id,
                                0xFFFFFFFF,
                                0);        
        _fgDataExhausted = FALSE;
    }
}
#endif

BOOL _SWDMX_WmvVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    UCHAR *pucBuf;
    UINT32 u4Addr;
    UINT32 u4StartCode = 0;
    UINT32 u4RepeatTimes =0;
    SWDMX_INFO_T *prSwdmxInst;
    BOOL fgTrickMode;
    
    prSwdmxInst = (SWDMX_INFO_T*)prPes->pvInstanceTag;

    
    if (!prSwdmxInst)
    {
        LOG(1, "Swdmx Instance is NULL\n");
        return FALSE;
    }
    
    fgTrickMode = (prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_IFRAME) ? TRUE : FALSE;
    
    //UINT16 u2VdecQueueSize,u2VdecMaxQueueSize;
    //VDEC_GetQueueInfo((UCHAR)prSwdmxInst->u1VdecId,&u2VdecQueueSize,&u2VdecMaxQueueSize);

    //LOG(0,"vdec queue %d/%d\n",u2VdecQueueSize,u2VdecMaxQueueSize);

    //pu1Temp = (UINT8*)prPes->u4FrameAddr;

    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));

    //if (prPes->u4Pts>15*90000 &&prPes->u4Pts<25*90000)
    //{
        LOG(8,"wmv callback PTS: 0x%x(%d.%03d)(codec:%d)[stc:%d]\n",
            prPes->u4Pts,
            PTS_S(prPes->u4Pts),
            PTS_MS(prPes->u4Pts),
            _rSwdmxWmvInfo.eVideoType,
            PTS_S(STC_GetStcValue(prSwdmxInst->u1StcId)));
    //}
    if(ENABLE_NEWTRACK(prSwdmxInst))
    {
      rPesInfo.ucEsId = (UCHAR)prPes->u1Channel;
    }
    else
    {
      rPesInfo.ucEsId = (UCHAR)prSwdmxInst->u1VdecId;
    }
    if(_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_MJPEG)
    {
        rPesInfo.ucMpvId = VLD1; // video format all use VLD expect jpeg
    }
    else
    {        
        rPesInfo.ucMpvId = VLD0; // video format all use VLD expect jpeg
    }
    rPesInfo.fgDtsValid = TRUE;//prPes->fgPtsDts;
    rPesInfo.u4PTS = (prPes->u4Pts);
    rPesInfo.u4DTS = prPes->u4Dts;
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;//(prPes->u4FrameAddr - prPes->u4BufStart);
    rPesInfo.fgEos = prPes->fgEOS;

    rPesInfo.u8OffsetI = (UINT64)_rSwdmxWmvInfo.u4IFrameOffset;
    rPesInfo.u8Offset = (UINT64)_rSwdmxWmvInfo.u4FrameOffset;
    
     x_memcpy((void*)rPesInfo.au1PicInfo, (void*)prPes->au1PicInfo, DMX_PICINFO_SIZE);
	
    if (rPesInfo.fgEos)
    {
        rPesInfo.fgBackward= (_rSwdmxWmvInfo.i4WMVPlaySpeed<0) ? TRUE : FALSE;
        rPesInfo.u4VldReadPtr =0;
        rPesInfo.u4FifoStart =0;
        rPesInfo.u4FifoEnd =0;
        if (_rSwdmxWmvInfo.fgVideoPause)
        {
            LOG(1, "_WMVStartToPlay for fgEos==TURE\n");
            _WMVStartToPlay(prSwdmxInst);
        }
    }


    _rSwdmxWmvInfo.u4LastVideoWptr = prPes->u4FrameAddr;

    switch (_rSwdmxWmvInfo.eVideoType)
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
    case ENUM_SWDMX_VDEC_MPEG4:
    case ENUM_SWDMX_VDEC_H263:
        rPesInfo.u4VideoType = DMX_VIDEO_MPEG4;
        break;
    case ENUM_SWDMX_VDEC_H264:
        rPesInfo.u4VideoType = DMX_VIDEO_H264;
        break;
    default:
        rPesInfo.u4VideoType = DMX_VIDEO_WMV9;
        break;
    }

    if ( _rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_DX311)
    {
        #if 1//defined(CC_TRUSTZONE_SUPPORT) 
        u4Addr = (UINT32)prPes->au1PicInfo;//prPes->u4FrameAddr;
        /*if (u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }*/
        #else
        u4Addr = prPes->u4FrameAddr;
         if (u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
         #endif
        pucBuf = (UCHAR*)u4Addr;

        if ((pucBuf[0] >> 6) & 0x1)
        {
            rPesInfo.ucPicType  = P_TYPE;
        }
        else
        {
            rPesInfo.ucPicType = I_TYPE;
        }
    }
    else if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_MPEG4)
    {
        #if 1//defined(CC_TRUSTZONE_SUPPORT) 
        u4Addr = (UINT32)prPes->au1PicInfo;//prPes->u4FrameAddr + 3;
        /*if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }*/
        #else
        u4Addr = prPes->u4FrameAddr + 3;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        #endif
        pucBuf = (UCHAR*)u4Addr;

        if (pucBuf[0] == 0xB6)
        {
            #if 1//defined(CC_TRUSTZONE_SUPPORT) 
            u4Addr = (UINT32)(prPes->au1PicInfo + 1);//prPes->u4FrameAddr + 4;
            /*if(u4Addr >= prPes->u4BufEnd)
            {
                u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
            }*/
            #else
            u4Addr = prPes->u4FrameAddr + 4;
            if(u4Addr >= prPes->u4BufEnd)
            {
                u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
            }
            #endif
            pucBuf = (UCHAR*)u4Addr;
            switch (pucBuf[0] >> 6)
            {
                case 0:
                    rPesInfo.ucPicType  = I_TYPE;
                    break;
                case 1: case 3:
                    rPesInfo.ucPicType  = P_TYPE;
                    break;
                case 2:
                    rPesInfo.ucPicType = B_TYPE;
                    break;
                default:
                    rPesInfo.ucPicType = UNKNOWN_TYPE;
                    break;
            }
        }
        if(fgTrickMode)
        {
            if(rPesInfo.u4PTS)
            {
                _u4PrevFramePTS = rPesInfo.u4PTS;
            }
            else
            {
                rPesInfo.u4PTS = _u4PrevFramePTS;
            }
            if(rPesInfo.ucPicType!=I_TYPE)
            {
                fgTrickMode=FALSE;
            }
        }
    }
    else if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1)
    {
        u4Addr =(UINT32)prPes->au1PicInfo;
        pucBuf = (UCHAR*)u4Addr;
        u4StartCode = pucBuf[0];
        u4StartCode |= 0x100;
        if ((u4StartCode == WMV_SC_ENTRY|| u4StartCode ==WMV_SC_SEQ))
        {
            VDEC_SendEs((void*)&rPesInfo);
            LOG(6, "send video es(vc1) 0x%x(%d.%03d)\n", 
                rPesInfo.u4PTS,
                PTS_S(rPesInfo.u4PTS),
                PTS_MS(rPesInfo.u4PTS));
            if (rPesInfo.u4PTS!=0 || u4StartCode ==WMV_SC_SEQ)
                _u4PrevFramePTS = (rPesInfo.u4PTS);
            return TRUE;
        }
        else if ((u4StartCode == WMV_SC_FIELD|| u4StartCode ==WMV_SC_FRAME))
        {
            if (rPesInfo.u4PTS ==0)
                rPesInfo.u4PTS = _u4PrevFramePTS;
            if (fgTrickMode)
                LOG(0,"frame pts: %x(%d.%03d)\n", 
                    rPesInfo.u4PTS,
                    PTS_S(rPesInfo.u4PTS),
                    PTS_MS(rPesInfo.u4PTS));
        }
        else
        {
            LOG(0,"!!!!!!Wmv videocallback find the unknown Start Code %x\n",u4StartCode);
            //ASSERT(0);
        }

    }
    else if(_rSwdmxWmvInfo.eVideoType==ENUM_SWDMX_VDEC_WMV7 
		|| _rSwdmxWmvInfo.eVideoType==ENUM_SWDMX_VDEC_WMV8
		|| _rSwdmxWmvInfo.eVideoType==ENUM_SWDMX_VDEC_WMV9)
    {
        if(rPayLoadHeader.u4OffsetIntoMediaObj==0 && rPayLoadHeader.u4MediaObjSize==1)
        {
           rPesInfo.u2SkipVidCount=1;
        }
    }
    else if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_H264)
    {
        #if 1//defined(CC_TRUSTZONE_SUPPORT) 
        //u4Addr = (UINT32)prPes->au1PicInfo;//prPes->u4FrameAddr + 3;
        /*if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }*/
        //rPesInfo.ucPicType = *(UINT8*)VIRTUAL(u4Addr);
        rPesInfo.ucPicType = prPes->au1PicInfo[0];
        #else
        u4Addr = prPes->u4FrameAddr + 3;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.ucPicType = *(UINT8*)u4Addr;
        #endif
        u4Addr = prPes->u4FrameAddr + 4;
        if(u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;
        
        if(IS_IDR_NALU(rPesInfo.ucPicType))
        {
            rPesInfo.fgDtsValid = TRUE;
        }
        else
        {
            rPesInfo.fgDtsValid = FALSE;
        }
    }
    else if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_H263)
    {
        u4Addr =(UINT32)prPes->au1PicInfo;
        pucBuf = (UCHAR*)u4Addr;
        LOG(1, "H263 picinfo: %02x %02x %02x %02x %02x %02x\n",
            pucBuf[0], pucBuf[1], pucBuf[2], pucBuf[3],
            pucBuf[4], pucBuf[5]);

        switch (pucBuf[0] & 0x02)
        {
            case 0:
                rPesInfo.ucPicType = I_TYPE;
                break;
            case 2:
                rPesInfo.ucPicType = P_TYPE;
                break;
            default:
                rPesInfo.ucPicType = UNKNOWN_TYPE;
                break;
        }
    }
	
    if (fgTrickMode)
    {
        if (!rPesInfo.fgEos && rPesInfo.u4PTS!=0)
        {
            if ((rPesInfo.u4PTS < (UINT32)_rSwdmxWmvInfo.u4DisplayPTS))
            {
                if (_rSwdmxWmvInfo.i4WMVPlaySpeed >0)
                {
                    LOG(3,"wmv video pts is too small for FF %x %x\n",rPesInfo.u4PTS,_rSwdmxWmvInfo.u4DisplayPTS);
                    return TRUE;
                }
            }
            else
            {
                if (_rSwdmxWmvInfo.i4WMVPlaySpeed <0)
                {
                    LOG(3,"wmv video pts is too big for FR %x %x \n",rPesInfo.u4PTS,_rSwdmxWmvInfo.u4DisplayPTS);
                    return TRUE;
                }
            }
        }

        if ( _fgReceiveVideoPes 
             && (u4StartCode != WMV_SC_FIELD) 
             && (rPesInfo.ucEsId == prSwdmxInst->u1VdecId))
        {
            UINT32 u4PtsDelta = 0;
            INT32 i4PtsDelta = 0;
            UINT32 i;
            
            if(rPesInfo.ucEsId < SWDMX_SOURCE_MAX)
            {
                i4PtsDelta = (INT32)((INT32)rPesInfo.u4PTS - (INT32)_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS);
                u4PtsDelta = (UINT32)abs(i4PtsDelta);
                u4RepeatTimes = (UINT32)(_rSwdmxWmvInfo.u4FrameRate*u4PtsDelta)/(90*(abs(_rSwdmxWmvInfo.i4WMVPlaySpeed)));
                u4RepeatTimes = min(u4RepeatTimes,300);
                LOG(5, "repeat times:%d cur:%d.%03d pre:%d.%03d (delta:%d.%03d, rate:%d, speed:%d)\n",
                    u4RepeatTimes,
                    PTS_S(rPesInfo.u4PTS),
                    PTS_MS(rPesInfo.u4PTS),
                    PTS_S(_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS),
                    PTS_MS(_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS),
                    PTS_S(u4PtsDelta),
                    PTS_MS(u4PtsDelta),
                    _rSwdmxWmvInfo.u4FrameRate,
                    _rSwdmxWmvInfo.i4WMVPlaySpeed);
                _rPreviousPesInfo[rPesInfo.ucEsId].u1SliceCount = TRUE;
            }
            
            if (rPesInfo.fgEos)
                u4RepeatTimes = 5;

            //x_thread_delay(100); // to avoid dmx not finish send data to vfifo

            for (i =0 ; i<u4RepeatTimes ; i++)
            {
                if(rPesInfo.ucEsId < SWDMX_SOURCE_MAX)
                {
                	if (i == u4RepeatTimes-1)
                	{
                        _rPreviousPesInfo[rPesInfo.ucEsId].u1SliceCount = FALSE;
                    }
                    LOG(6, "send video es1 0x%x(%d.%03d) %d\n", 
                            _rPreviousPesInfo[rPesInfo.ucEsId].u4PTS,
                            PTS_S(_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS),
                            PTS_MS(_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS),
                            _rPreviousPesInfo[rPesInfo.ucEsId].ucPicType);
                    VDEC_SendEs((void*)&_rPreviousPesInfo[rPesInfo.ucEsId]);
                }
            }
            if (rPesInfo.fgEos)
            {
                LOG(6, "send video es(eos1) 0x%x(%d.%03d)\n", 
                    rPesInfo.u4PTS,
                    PTS_S(rPesInfo.u4PTS),
                    PTS_MS(rPesInfo.u4PTS));
                VDEC_SendEs((void*)&rPesInfo);
            }
        }
        else if ( _fgReceiveVideoPes && u4StartCode == WMV_SC_FIELD)
        {
            UINT32 u4idx =0;
            for (u4idx =0 ; u4idx <_rSwdmxWmvInfo.u4RepeatTimes ; u4idx++)
            {
                if(rPesInfo.ucEsId < SWDMX_SOURCE_MAX)
                {
                    LOG(6, "send video es2 0x%x(%d.%03d) %d\n", 
                        _rPreviousPesInfo[rPesInfo.ucEsId].u4PTS,
                        PTS_S(_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS),
                        PTS_MS(_rPreviousPesInfo[rPesInfo.ucEsId].u4PTS),
                        _rPreviousPesInfo[rPesInfo.ucEsId].ucPicType);
                    VDEC_SendEs((void*)&_rPreviousPesInfo[rPesInfo.ucEsId]);
                }
                LOG(6, "send video es3 0x%x(%d.%03d) %d\n", 
                    rPesInfo.u4PTS,
                    PTS_S(rPesInfo.u4PTS),
                    PTS_MS(rPesInfo.u4PTS),
                    rPesInfo.ucPicType);
                VDEC_SendEs((void*)&rPesInfo);
            }
        }
        else
        {
            LOG(5, "cannot send es for _fgReceiveVideoPes==%s, pts 0x%x\n",
                _fgReceiveVideoPes?"TRUE":"FALSE",
                rPesInfo.u4PTS);
            if (rPesInfo.fgEos)
            {
                LOG(6, "send video es(eos2) 0x%x(%d.%03d) %d\n", 
                    rPesInfo.u4PTS,
                    PTS_S(rPesInfo.u4PTS),
                    PTS_MS(rPesInfo.u4PTS),
                    rPesInfo.ucPicType);
                VDEC_SendEs((void*)&rPesInfo);
            }
        }
    }
    else
    {
#ifdef __MODEL_slt__
        if (rPesInfo.u4PTS > _u4VC1SltLastPicPts)
        {
            _u1VC1SltLastPicType = rPesInfo.ucPicType;
            _u4VC1SltLastPicPts = rPesInfo.u4PTS;
        }
#endif
        LOG(7, "send video es 0x%x(%d.%03d)\n", 
            rPesInfo.u4PTS,
            PTS_S(rPesInfo.u4PTS),
            PTS_MS(rPesInfo.u4PTS));
        VDEC_SendEs((void*)&rPesInfo);
    }

    if (fgTrickMode&&(u4RepeatTimes!=0 ||_fgReceiveVideoPes==FALSE))
    {
        if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1)
        {
            if ((u4StartCode == WMV_SC_FRAME || u4StartCode == WMV_SC_FIELD))
            {
                if(rPesInfo.ucEsId < SWDMX_SOURCE_MAX)
                {
                    x_memcpy((VOID*)&_rPreviousPesInfo[rPesInfo.ucEsId],(VOID*)&rPesInfo,sizeof(VDEC_PES_INFO_T));
                }
                _fgReceiveVideoPes = TRUE;
            }
            else
            {
                LOG(6, "send video es(vc1,trick) 0x%x(%d.%03d) %d\n", 
                    rPesInfo.u4PTS,
                    PTS_S(rPesInfo.u4PTS),
                    PTS_MS(rPesInfo.u4PTS),
                    rPesInfo.ucPicType);
                VDEC_SendEs((void*)&rPesInfo);
            }
        }
        else
        {
            if(rPesInfo.ucEsId < SWDMX_SOURCE_MAX)
            {
                LOG(5, "---save pes to prePes(0x%x), repeat:%d, RecvVPes?%s(will be yes)\n",
                    rPesInfo.u4PTS,
                    u4RepeatTimes,
                    _fgReceiveVideoPes?"yes":"no");
                x_memcpy((VOID*)&_rPreviousPesInfo,(VOID*)&rPesInfo,sizeof(VDEC_PES_INFO_T));
            }
            _fgReceiveVideoPes = TRUE;
        }
    }
    return TRUE;

}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
/**
*/
BOOL _SWDMX_WMVFeederIsReady(SWDMX_INFO_T *prSwdmxInst)
{
    if (MPV_IsDisplay(prSwdmxInst->u1VdecId))
    {
        return TRUE; //wait sequence header
    }

    //should be checked, 
    //time-based mode, feeder is always ready?
    if (prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK)
    {
        return TRUE;
    }

    if ( (_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
            || (_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DLNA))
    {
#ifdef ENABLE_FEEDER_THRESHOLD
        //with feeder threshold
#else
        //without feeder threshold
#if 1
        if (_fgNetflixEos && _prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            return TRUE;
        }
#endif
        if (!_SWDMX_WMVIsFeederBufferFull(prSwdmxInst))
        {
            UINT32 u4FeederSize;
            FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo);
            u4FeederSize = _rFeederInfo.u4BytesInBuf;
            if (_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
            {
                if (u4FeederSize < WMV_NETFLIX_PRE_BUFFER_SIZE && _u4RequestDataSize !=0/*!_rSwdmxWmvInfo.fgReqFeederDataEOS*/)
                {
                    return FALSE;
                }
            }
            else
            {
                if (u4FeederSize < WMV_PRE_BUFFER_SIZE && _u4RequestDataSize !=0/*!_rSwdmxWmvInfo.fgReqFeederDataEOS*/)
                {
                    return FALSE;
                }
            }
            return TRUE;
        }
        else
        {
            return TRUE;
        }
#endif
    }
    else
    {
        return TRUE;
    }
}


BOOL _SWDMX_WMVIsFeederBufferFull(SWDMX_INFO_T *prSwdmxInst)
{
    UINT32 u4BlockSize;
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) != FEEDER_E_OK)
    {
        LOG(7, "Feeder input buffer error.\n");
        return FALSE;
    }
    u4BlockSize=FeederGetAlignSize(prSwdmxInst->eFeederSrc);

    if (_rFeederInfo.u4FreeSize >=(_u4DefalutReqSize+u4BlockSize+u4BlockSize))
    {
        return FALSE;
    }
    else
    {
//      LOG(5, "fdr_free(0x%x)<defReqSz(0x%x)+2*block(0x%x)\n",
//          _rFeederInfo.u4FreeSize,
//          _u4DefalutReqSize,
//          u4BlockSize);
        return TRUE;
    }
}

BOOL _SWDMX_WMVGetFeederDataTimebased(SWDMX_INFO_T *prSwdmxInst, BOOL fgNoWait)
{
    BOOL fgRet = FALSE;
    INT32 i4Ret;
    WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;

    //if(_rSwdmxWmvInfo.fgReqFeederDataEOS)
    if ( _u4RequestDataSize == 0)
    {
        LOG(0, "req data size 0, return@%s(tb)\n", __func__);
        return FALSE;
    }

    x_memset(&_rFeederReqData,0,sizeof(FEEDER_REQ_DATA_T));
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) != FEEDER_E_OK)
    {
        LOG(0, "!!!!%s FeederGetBufferInfo() error.(tb)\n", __func__);
        return FALSE;
    }

    if (!fgNoWait)
    {
        do
        {
            if(_rSwdmxWmvInfo.fgPlayAbort)
            {
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                LOG(3,"Play abort 1(tb)\n");
                return FALSE;
            }
            
            fgRet = _SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, 
                                            (VOID*)&_rFeederReqData);
            if (_rFeederReqData.eDataType == FEEDER_SOURCE_INVALID)
            {
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                LOG(3,"feeder invalid 1(tb), reqid(%d/%d)\n", 
                    _rFeederReqData.u4Id,
                    _rSwdmxWmvInfo.u4FeederReqID-1);
                return FALSE;
            }
            
            if (_rFeederReqData.u4WriteAddr == 0 && _rFeederReqData.fgEof)
            {
                LOG(0, "\n\n\ngot eof && addr == 0!!\n");
            }
            if (_rFeederReqData.u4WriteAddr != 0 && _rFeederReqData.fgEof)
            {
                LOG(0, "\n\n\ngot eof && addr != 0 (expected)\n");
            }
            if (_rFeederReqData.u4WriteAddr != 0 && !_rFeederReqData.fgEof)
            {
                LOG(0, "\n\n\nnot get eof && addr != 0 (expected)\n");
            }
#if 1  // megaa 20100422: tmp solution for 1st playback error of WMA on Linux Turnkey
            if (_rFeederReqData.u4WriteAddr == 0 && !_rFeederReqData.fgEof)  // it is error case!!!
            {
                #if 0
                LOG(0, "\n\n\n_rFeederReqData.u4WriteAddr == 0, fdr_EOF?%s (ignore)\n\n\n\n",
                    _rFeederReqData.fgEof ? "yes" : "no");
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                return FALSE;
//              if (_rFeederReqData.fgEof)
//              {
//                  _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
//              }
//              return FALSE;
                //_SWDMX_WMVRequestFeederData(prSwdmxInst, _u4RequestDataSize, _rSwdmxWmvInfo.u8LastFileParsingPosition, FALSE);
                //_rFeederReqData.u4Id = 0xFFFF1234;
                #else
                UINT32 u4CurPts = 0;

                LOG(0, "\n\n\n_rFeederReqData.u4WriteAddr == 0, fdr_EOF?%s (ignore)\n\n\n\n",
                    _rFeederReqData.fgEof ? "yes" : "no");
                _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
                #if 0
                LOG(5, "+++[ACK] update feeder read pointer! (0x%x)\n", _rSwdmxWmvInfo.u4PacketStartAddr);
                FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
                #else
                if (_rFeederInfo.u4WriteAddr)
                {
                    LOG(5, "+++[ACK] update feeder read pointer! (0x%x)\n", _rFeederInfo.u4WriteAddr);
                    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rFeederInfo.u4WriteAddr);
                }
                #endif
                if(_rFeederReqData.u8FilePos != (UINT64)DLNA_TIME_SEEK_INVALID_TIME)
                {
                    _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
                    _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = _rFeederReqData.u8FilePos*90000;
                }
                _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 0, 0, FALSE);
                _rFeederReqData.u4Id = 0xFFFF1234;
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                _rSwdmxWmvInfo.u4ConsumedSize = 0;
                STC_StopStc(prSwdmxInst->u1StcId);              
                STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                #endif
            }
#endif
        } while (_rFeederReqData.u4Id+1 != _rSwdmxWmvInfo.u4FeederReqID);

        if (!fgRet)
        {
            LOG(3,"feeder timeout1(tb)\n");
            
            LOG(2,"Send Eos 2(tb)\n");
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return FALSE;
        }
    }
    else
    {
        do
        {
            if(_rSwdmxWmvInfo.fgPlayAbort)
            {
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                LOG(3,"Play abort 2(tb)\n");
                return FALSE;
            }
            i4Ret = _SWDMX_ReceiveFeederAckNoWait(prSwdmxInst->u1SwdmxId, (VOID*)&_rFeederReqData);
            if (i4Ret == OSR_NO_MSG)
            {
                LOG(0, "OSR_NO_MSG(tb)\n");
                return FALSE;
            }
            else if (i4Ret != OSR_OK)
            {
                LOG(2,"Send Eos 3(tb)\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
                return FALSE;
            }
            else
            {
                if (_rFeederReqData.eDataType == FEEDER_SOURCE_INVALID)
                {
                    _rSwdmxWmvInfo.fgFeederInvalid = TRUE;                         
                    LOG(3,"feeder invalid 2(tb), reqid(%d/%d)\n", 
                        _rFeederReqData.u4Id,
                        _rSwdmxWmvInfo.u4FeederReqID-1);
                    if (_rFeederReqData.fgEof)//feeder should handle this flag
                        _rFeederReqData.fgEof = FALSE;
                    return FALSE;
                }
            }
        } while (_rFeederReqData.u4Id+1 != _rSwdmxWmvInfo.u4FeederReqID);
    }

    LOG(7, "feeder ack(tb) %d\n",_rFeederReqData.u4Id);
    if ((_rFeederReqData.u4WriteAddr < _rFeederInfo.u4StartAddr) || 
        (_rFeederReqData.u4WriteAddr >= _rFeederInfo.u4EndAddr))
    {
        if (!_rFeederReqData.fgEof)
        {
            LOG(0,"!!!!!!!![tb]not eof! prFeederData->u4WriteAddr = %x(return)\n",_rFeederReqData.u4WriteAddr);
        _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
        return FALSE;
        }
        else
        {
            LOG(0,"!!!!!!!![tb]eof! prFeederData->u4WriteAddr = %x(ignore!)\n",_rFeederReqData.u4WriteAddr);
        }
    }

    x_memcpy((VOID*)&(rWmvFeederReqData.rFeederReqData),(VOID*)&_rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
    rWmvFeederReqData.u4RequestFeederSize = _u4RequestDataSize;

    if (_rFeederReqData.fgEof) 
    {
        LOG(0, "!!![tb] got eof from feeder msgQ! (send to wmv msgQ @id: %d)\n", _rFeederReqData.u4Id);
        _rSwdmxWmvInfo.fgNeedSetFdrReq = FALSE;
    }
    i4Ret = x_msg_q_send(_hWMVFeederDataQue, &rWmvFeederReqData, sizeof(WMV_FEEDER_REQ_DATA_T), 255);
    LOG(5, "[#%d][tb] ACK ok, msgs++\n", _rFeederReqData.u4Id);
//  if (_rFeederReqData.fgEof)
//  {
//      LOG(5, "fgEof == TRUE, reset!\n");
//      x_memset(&_rFeederReqData,0,sizeof(FEEDER_REQ_DATA_T));
//      LOG(5, "fgEof == %s now!\n", _rFeederReqData.fgEof?"TRUE":"FALSE");
//  }

    return TRUE;
}

BOOL _SWDMX_WMVGetFeederData(SWDMX_INFO_T *prSwdmxInst, BOOL fgNoWait)
{
    BOOL fgRet = FALSE;
    INT32 i4Ret;
    WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;

    //if(_rSwdmxWmvInfo.fgReqFeederDataEOS)
    if ( _u4RequestDataSize == 0)
    {
        return FALSE;
    }

    x_memset(&_rFeederReqData,0,sizeof(FEEDER_REQ_DATA_T));
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) != FEEDER_E_OK)
    {
        LOG(0, "!!!!_SWDMX_WMVGetFeederData FeederGetBufferInfo() error.\n");
        return FALSE;
    }

    if (!fgNoWait)
    {
        do
        {
            if(_rSwdmxWmvInfo.fgPlayAbort)
            {
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                LOG(3,"Play abort\n");
                return FALSE;
            }
            
            fgRet = _SWDMX_ReceiveFeederAck(prSwdmxInst->u1SwdmxId, (VOID*)&_rFeederReqData);
            if (_rFeederReqData.eDataType == FEEDER_SOURCE_INVALID)
            {
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                LOG(3,"feeder invalid 1\n");
                return FALSE;
            }
            
#if 1  // megaa 20100422: tmp solution for 1st playback error of WMA on Linux Turnkey
            if (_rFeederReqData.u4WriteAddr == 0)  // it is error case!!!
            {
                LOG(5, "ACK addr=0, update feeder read pointer\n");
                _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
                _SWDMX_WMVConsumeFeeder(prSwdmxInst);
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                _rSwdmxWmvInfo.u4ConsumedSize = 0;
                _rFeederReqData.u4Id = 0xFFFF1234;
                _SWDMX_WMVRequestFeederData(prSwdmxInst, 
                                            _u4RequestDataSize, 
                                            _rSwdmxWmvInfo.u8LastFileParsingPosition, 
                                            FALSE);
            }
#endif
        } while (_rFeederReqData.u4Id+1 != _rSwdmxWmvInfo.u4FeederReqID);

        if (!fgRet)
        {
            LOG(3,"feeder timeout1\n");
            
            LOG(2,"Send Eos 2\n");
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return FALSE;
        }
    }
    else
    {
        do
        {
            if(_rSwdmxWmvInfo.fgPlayAbort)
            {
                _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                LOG(3,"Play abort\n");
                return FALSE;
            }
            i4Ret = _SWDMX_ReceiveFeederAckNoWait(prSwdmxInst->u1SwdmxId, (VOID*)&_rFeederReqData);
            if (i4Ret == OSR_NO_MSG)
            {
                return FALSE;
            }
            else if (i4Ret != OSR_OK)
            {
                LOG(2,"Send Eos 3\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
                return FALSE;
            }
            else
            {
                if (_rFeederReqData.eDataType == FEEDER_SOURCE_INVALID)
                {
                    _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
                    LOG(3,"feeder invalid 1\n");
                    return FALSE;
                }
            }
        } while (_rFeederReqData.u4Id+1 != _rSwdmxWmvInfo.u4FeederReqID);
    }

    LOG(7,"feeder ack %d\n",_rFeederReqData.u4Id);
    if ((_rFeederReqData.u4WriteAddr < _rFeederInfo.u4StartAddr) || 
        (_rFeederReqData.u4WriteAddr >=_rFeederInfo.u4EndAddr))
    {
        LOG(0,"!!!!!!!!prFeederData->u4WriteAddr = %x\n",_rFeederReqData.u4WriteAddr);
        _rSwdmxWmvInfo.fgFeederInvalid = TRUE;
        return FALSE;
    }

    x_memcpy((VOID*)&(rWmvFeederReqData.rFeederReqData),(VOID*)&_rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
    rWmvFeederReqData.u4RequestFeederSize = _u4RequestDataSize;

    i4Ret = x_msg_q_send(_hWMVFeederDataQue, &rWmvFeederReqData, sizeof(WMV_FEEDER_REQ_DATA_T), 255);
	if(i4Ret!=OSR_OK)
    {
       LOG(0,"send _hWMVFeederDataQue error %d\n",i4Ret);
    }
    if (_rSwdmxWmvInfo.fgFirstFrame)
    {
        if (_rSwdmxWmvInfo.u8BeginParseOfst == 0)
        {
            _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER+
                    ((_u4RequestDataSize-WMV_DATA_OBJ_HEADER)/_prRangeInfo->ui4_sz_min_acces_unt)*_prRangeInfo->ui4_sz_min_acces_unt;
        }
        else
        {
            _rSwdmxWmvInfo.u8LastFileParsingPosition = _rSwdmxWmvInfo.u8BeginParseOfst + 
                    (_u4RequestDataSize/_prRangeInfo->ui4_sz_min_acces_unt)*_prRangeInfo->ui4_sz_min_acces_unt;
        }
    }
    else
    {
        _rSwdmxWmvInfo.u8LastFileParsingPosition += (_u4RequestDataSize/_prRangeInfo->ui4_sz_min_acces_unt)*_prRangeInfo->ui4_sz_min_acces_unt;
    }

    return TRUE;
}

BOOL _SWDMX_WMVRequestFeederDataTimebased(SWDMX_INFO_T *prSwdmxInst, UINT32 u4QrySize, UINT64 u8Offset, BOOL fgTrickMode)
{
    INT32 _i4Ret = -1;
    UINT64 _u8Remainder=0;
    UINT64 _u8ReqTime = 0;
    UINT32 u4DefalutReqSize,u4DefaultSize=0;

    x_memset(&_rFeederCond,0,sizeof(FEEDER_TRANSMIT_COND));

    if (!fgTrickMode)
    {
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
           u4DefaultSize = WMA_REQUEST_SIZE;
        else
           u4DefaultSize = TIMEBASED_NORMALMODE_WMV_REQ_SIZE;
        //u8Offset = _rSwdmxWmvInfo.u8LastFileParsingPosition;
    }
    u4DefalutReqSize = (u4DefaultSize/_prRangeInfo->ui4_sz_min_acces_unt)
                        *_prRangeInfo->ui4_sz_min_acces_unt;

    if (_rSwdmxWmvInfo.fgFirstFrame && _rSwdmxWmvInfo.u8BeginParseOfst == 0)
    {
        //u4DefalutReqSize += WMV_DATA_OBJ_HEADER; //time based cannot use this
    }

    if (!fgTrickMode)
    {
        u4QrySize = u4DefalutReqSize;
    }
    _u4RequestDataSize = u4QrySize;
    LOG(7, "set request(0x%x bytes) to feeder(id:%d)\n", 
        u4QrySize,
        _rSwdmxWmvInfo.u4FeederReqID);

    if (_rSwdmxWmvInfo.fgTimebasedNeedSeek)
    {
        if (0 != _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo)
        {
            _rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr = FALSE;
        }
        _u8ReqTime = _SWDMX_Div6464(_rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo, 
                                    (UINT64)90000, 
                                    &_u8Remainder);
        _i4Ret = FeederSetRequest(prSwdmxInst->eFeederSrc,
                                  u4QrySize,
                                  _u8ReqTime, 
                                  &_rFeederCond,
                                  _rSwdmxWmvInfo.u4FeederReqID++);
        LOG(3, "Timebased FeederSetRequest#seek(time:0x%llx, reqid:%d, reqsz:%d)\n",
            _u8ReqTime,
            (_rSwdmxWmvInfo.u4FeederReqID-1),
            u4QrySize);
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = FALSE;
    }
    else
    {        
        if (_rSwdmxWmvInfo.fgFirstReqFeederData)
            _u8ReqTime = 0;
        else
            _u8ReqTime = (UINT64)DLNA_TIME_SEEK_INVALID_TIME;
        _i4Ret = FeederSetRequest(prSwdmxInst->eFeederSrc,
                                  u4QrySize,
                                  _u8ReqTime, 
                                  &_rFeederCond,
                                  _rSwdmxWmvInfo.u4FeederReqID++);
        LOG(5, "Timebased FeederSetRequest#normal(time:0x%llx, reqid:%d, reqsz:%d)\n",
            _u8ReqTime,
            (_rSwdmxWmvInfo.u4FeederReqID-1),
            u4QrySize);
    }
    if (_i4Ret != FEEDER_E_OK)
    {
        LOG(0, "Timebased FeederSetRequest FAIL! ret:%d reqid:%d\n", 
            _i4Ret, 
            _rSwdmxWmvInfo.u4FeederReqID - 1);
        _u4RequestDataSize = 0;
    }
    return TRUE;
}

BOOL _SWDMX_WMVRequestFeederData(SWDMX_INFO_T *prSwdmxInst, UINT32 u4QrySize,UINT64 u8Offset,BOOL fgTrickMode)
{
    //HAL_TIME_T rStartTime,rEndTime;
    UINT32 u4RequestDataSize;
    UINT64 u8RemainedDataSize;
    UINT32 u4DefalutReqSize,u4DefaultSize=0;
    //UINT32 u4QrySize;
    //UINT64 u8Offset;
    x_memset(&_rFeederCond,0,sizeof(FEEDER_TRANSMIT_COND));

    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
       u4DefaultSize=WMA_REQUEST_SIZE;
    }
	else
	{
       u4DefaultSize=WMV_REQUEST_SIZE;
	}

    if((_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_DLNA) &&
        (_rSwdmxWmvInfo.i4WMVPlaySpeed > 1 )&&
        (_rSwdmxWmvInfo.i4WMVPlaySpeed < 1000) &&
        prSwdmxInst->fgSupportConnStalling)
    {//for dlna MCVT  3.0.3 slow forward stalling test case
        u4DefaultSize  *= prSwdmxInst->u4Multiple;
        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) == FEEDER_E_OK)
        {
            if(_rFeederInfo.u4BytesInBuf > 192*1024)
            {
                _u4RequestDataSize = 0;
                return TRUE;
            }
        }
    }
	
    if (_rSwdmxWmvInfo.fgFirstFrame)
    {
        if (_rSwdmxWmvInfo.u8BeginParseOfst == 0)
            u8RemainedDataSize = _rSwdmxWmvInfo.u8FileSize;
        else
            u8RemainedDataSize = _rSwdmxWmvInfo.u8FileSize + _prRangeInfo->ui8_fileoffset - _rSwdmxWmvInfo.u8BeginParseOfst;
        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            u4DefalutReqSize = (u4DefaultSize/_prRangeInfo->ui4_sz_min_acces_unt)*_prRangeInfo->ui4_sz_min_acces_unt;
            if (_rSwdmxWmvInfo.u8BeginParseOfst == 0)
                u4DefalutReqSize += WMV_DATA_OBJ_HEADER;
        }
        else
        {
            u4DefalutReqSize = (282*1024);//min(282*1024, FEEDER_DEFAULT_BUF_SIZE >> 3);
        }
    }
    else
    {
        u8RemainedDataSize = _rSwdmxWmvInfo.u8FileSize -
                             ((_rSwdmxWmvInfo.u8LastFileParsingPosition)-(_prRangeList->prDmxRangeInfo->ui8_fileoffset));
        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            u4DefalutReqSize = (u4DefaultSize/_prRangeInfo->ui4_sz_min_acces_unt)*_prRangeInfo->ui4_sz_min_acces_unt;
        }
        else
        {
            u4DefalutReqSize = (282*1024);//min(282*1024, FEEDER_DEFAULT_BUF_SIZE >> 3);
        }
    }

    if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
    {
        u4RequestDataSize = min(u4DefalutReqSize,u8RemainedDataSize);
    }
    else
    {
        u4RequestDataSize = u4DefalutReqSize;
    }
    
    if (!fgTrickMode)
    {
        u4QrySize = u4RequestDataSize;
        u8Offset = _rSwdmxWmvInfo.u8LastFileParsingPosition;
    }

    _u4RequestDataSize = u4QrySize;

    if (u4QrySize != 0)
    {
        INT32 i4Ret;
        LOG(7, "[wmv]req id: %d (Ofst 0x%llx, QrySz 0x%x)(remain:0x%llx, def:0x%x)\n",
            _rSwdmxWmvInfo.u4FeederReqID,
            u8Offset,
            u4QrySize,
            u8RemainedDataSize,
            u4DefalutReqSize);
        i4Ret = FeederSetRequest(prSwdmxInst->eFeederSrc,u4QrySize,u8Offset,&_rFeederCond,_rSwdmxWmvInfo.u4FeederReqID++);

        //HAL_GetTime(&rStartTime);
        //_SWDMX_WMVGetFeederData(prSwdmxInst);
        //HAL_GetTime(&rEndTime);
        //HAL_GetDeltaTime(&_rFeederReqTime, &rStartTime, &rEndTime);
        //_rSwdmxWmvInfo.fgTureReqFeederData = TRUE;
        if (i4Ret != FEEDER_E_OK)
        {
            _u4RequestDataSize = 0;
        }
        return TRUE;
    }
    else
    {
        _rSwdmxWmvInfo.fgReqFeederDataEOS = TRUE;
        FeederSetInfo(prSwdmxInst->eFeederSrc, FEEDER_SWDMX_EOS,TRUE);
        return FALSE;
    }
}

BOOL _SWDMX_WMVSetStrmID(UINT8 u1SrcId, UINT8 u1HdrType,UINT32 u4StrmID, UINT32 u4StrmSubID)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);        

    switch (u1HdrType)
    {
    case eSWDMX_STRM_TYPE_VID:
#if defined(CC_3D_MM_DS_SUPPORT)  // hard code to inverse L/R
        if (ENABLE_NEWTRACK(prSwdmxInst))
        {
            u4StrmID = 2;
        }
#endif
        _rSwdmxWmvInfo.u4VStreamId= u4StrmID;
#ifdef CC_NETFLIX_CMPB_SUPPORT
        _u4NetflixVideoStreamId = u4StrmID;
#endif
        break;
    case eSWDMX_STRM_TYPE_VID2:
#if defined(CC_3D_MM_DS_SUPPORT)  // hard code to inverse L/R
        if (ENABLE_NEWTRACK(prSwdmxInst))
        {
            u4StrmID = 3;
        }
#endif
        _rSwdmxWmvInfo.u4VSubStreamId=u4StrmID;
        break;
    case eSWDMX_STRM_TYPE_AUD:
        _u1AudioStreamId = (UINT8)_rSwdmxWmvInfo.u4AStreamId;//old audio stream id
        _rSwdmxWmvInfo.u4AStreamId = ASID(u4StrmID);//new audio stream id
        LOG(0, "setStrmID:: strm_id(old:%d new:%d)\n", _u1AudioStreamId, u4StrmID);
#ifdef CC_NETFLIX_CMPB_SUPPORT
        _u4NetflixAudioStreamId = ASID(u4StrmID);
#endif
#ifdef WMV_SUPPORT_AUDIO_SWITCH
        if ((_u1AudioStreamId != (UINT8)_rSwdmxWmvInfo.u4AStreamId) &&_u1AudioStreamId !=0xff)
        {
            _rSwdmxWmvInfo.fgChangeAudTrack = TRUE;
            LOG(0,"WMV file change audio channel\n");
        }
        else
        {
            _rSwdmxWmvInfo.u4WmaErrorCode = 0; //DTV215322
            //_rSwdmxWmvInfo.fgChangeAudTrack = FALSE;
        }
#endif
        break;
    case eSWDMX_STRM_TYPE_SUBTITLE:
        _rSwdmxWmvInfo.u4SpStreamId = u4StrmID;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

BOOL _SWDMX_WMVEnableStrm(UINT8 u1SrcId, UINT8 u1StrmType, VOID* pfnCbFunc)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);        

    switch (u1StrmType)
    {
    case eSWDMX_STRM_TYPE_VID:
        //prSwdmxInst->u1SwdmxId = u1SrcId;//maiyou  megaa: no need
        _rSwdmxWmvInfo.fgEnVideo = TRUE;
        /* double confirm DMX being initialized
         * another initializing action is in HeaderParse()
         */
         /* ensure this function is invoked after SetRange */
        if (!_rSwdmxWmvInfo.fgEnVDmx && _rSwdmxWmvInfo.fgIsSetRangeInvoked)
        {
            _SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1VdecId,TRUE,FALSE);
            _rSwdmxWmvInfo.fgEnVDmx = TRUE;
        }
        break;
    
    case eSWDMX_STRM_TYPE_VID2:
        //prSwdmxInst->u1SwdmxId = u1SrcId;//maiyou  megaa: no need
        _rSwdmxWmvInfo.fgEnVideo = TRUE;
        if (!_rSwdmxWmvInfo.fgEnV2Dmx && _rSwdmxWmvInfo.fgIsSetRangeInvoked)
        {
            _SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1Vdec2Id,TRUE,FALSE);
            _rSwdmxWmvInfo.fgEnV2Dmx = TRUE;
        }      
        break;

    case eSWDMX_STRM_TYPE_AUD:
    case eSWDMX_STRM_TYPE_AUD2:
        LOG(0, "%s::aud#%d, fgEnAudio will be set to TRUE\n", __FUNCTION__, _rSwdmxWmvInfo.u1AudPid);
        _rSwdmxWmvInfo.fgEnAudio = TRUE; 
        _rSwdmxWmvInfo.fgAudStrmDisabled = FALSE;
        break;

    case eSWDMX_STRM_TYPE_SUBTITLE:
        _rSwdmxWmvInfo.fgEnSubTitle = TRUE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


BOOL _SWDMX_WMVDisableStrm(UINT8 u1SrcId, UINT8 u1StrmType)
{
    DMX_MM_T rPid;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);        

    rPid.fgEnable = FALSE;

    switch (u1StrmType)
    {
    case eSWDMX_STRM_TYPE_VID:
        _rSwdmxWmvInfo.fgEnVideo = FALSE;
        if (_rSwdmxWmvInfo.fgEnVDmx)
        {
            VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid, DMX_MM_FLAG_VALID, &rPid));
            VERIFY(DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid));
            VERIFY(DMX_MUL_FreePidx(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid));
            _rSwdmxWmvInfo.fgEnVDmx = FALSE;
        }
        if (_rSwdmxWmvInfo.fgAudIsReady && _rSwdmxWmvInfo.fgEnAudio)
        {
            LOG(0,"disable video stream and force audio play\n");
            STC_StartStc(prSwdmxInst->u1StcId);
            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
            _rSwdmxWmvInfo.fgAudIsReady = FALSE;
        }
        break;
    case eSWDMX_STRM_TYPE_VID2:
        _rSwdmxWmvInfo.fgEnVideo = FALSE;
        if (_rSwdmxWmvInfo.fgEnV2Dmx)
        {
            VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid, DMX_MM_FLAG_VALID, &rPid));
            VERIFY(DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid));
            VERIFY(DMX_MUL_FreePidx(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid));
            _rSwdmxWmvInfo.fgEnV2Dmx = FALSE;
        }
        break;
    case eSWDMX_STRM_TYPE_AUD:
    case eSWDMX_STRM_TYPE_AUD2:
        LOG(0, "%s::aud#%d, fgEnAudio will be set to FALSE\n", __FUNCTION__, _rSwdmxWmvInfo.u1AudPid);
        _rSwdmxWmvInfo.fgEnAudio = FALSE;
        _rSwdmxWmvInfo.fgAudStrmDisabled = TRUE;
        if (_rSwdmxWmvInfo.fgVideoPause&&_rSwdmxWmvInfo.fgEnVideo)
        {
            _WMVStartToPlay(prSwdmxInst);
        }        
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        DMX_AudHandler_SetActivePidx(DMX_AUDIO_HANDLER_NULL_PIDX);
        #endif
        _rSwdmxWmvInfo.fgAlreadySet = TRUE;

        break;
    case eSWDMX_STRM_TYPE_SUBTITLE:
        _rSwdmxWmvInfo.fgEnSubTitle = FALSE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL _SWDMX_WMVDRMInitDmx(SWDMX_INFO_T* prSwdmxInst)
{
    UINT8 u1VidPid=0;
    UINT8 u1AudPid=0;
    
    if(_rSwdmxWmvInfo.fgEnVideo)
    {
        u1VidPid = _rSwdmxWmvInfo.u1VidPid;
        if(_rSwdmxWmvInfo.eDRMType == DRM_KEY_AES)
        {
            DMX_MM_DRM_AES_T rMMDrmAes;
            UINT32 u4Flag =0;
            _u4VideoFrameDataAddr = (UINT32)BSP_AllocAlignedDmaMemory(200*1024,WMV_DMX_FIFO_ALIGNMENT);
            ASSERT(_u4VideoFrameDataAddr!=0);
            LOG(0, "The AES frame FIFO Addr is %X\n",_u4VideoFrameDataAddr);            

            x_memset((VOID*)&rMMDrmAes,0,sizeof(DMX_MM_DRM_AES_T));
            if (INPUT_SRC == MM_SRC_TYPE_NETWORK_NETFLIX)
            {
                u4Flag = (UINT32)(DMX_MM_DRM_FLAG_KEY);
                x_memset((VOID*)&rMMDrmAes.au1Key[0],0,sizeof(rMMDrmAes.au1Key));
                x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],_rSwdmxWmvInfo.u4DRMKeySize);
                DMX_MM_SetDRMMode(u1VidPid,DMX_DRM_MODE_ND);  // megaa 20100314 ToDo: check if DMX has new DRM APIs
                DMX_MM_SetNDDRM(u1VidPid,u4Flag,&rMMDrmAes);  // megaa 20100314 ToDo: check if DMX has new DRM APIs
            } 
            else
            {
                /*u4Flag = (UINT32)(DMX_MM_DRM_FLAG_KEY | DMX_MM_DRM_FLAG_SAMPLE_ID);
                x_memset((VOID*)&rMMDrmAes.au1Key[0],0,sizeof(rMMDrmAes.au1Key));
                x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],_rSwdmxWmvInfo.u4DRMKeySize);
                
                //x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_au1ContentKey[0],sizeof(_au1ContentKey));
                x_memset((VOID*)&rMMDrmAes.au1SampleID[0],0,sizeof(rMMDrmAes.au1SampleID));                       
                DMX_MM_SetNDDRM(u1VidPid,u4Flag,&rMMDrmAes);  
                */
                DMX_MM_SetDRMMode(u1VidPid,DMX_DRM_MODE_ND); 
            }
        }
        else if(_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4)
        {
            DMX_MM_DRM_PD_T rMMDrmRC4;
            UINT32 u4Flag = 0;
            x_memset((VOID*)&rMMDrmRC4,0,sizeof(DMX_MM_DRM_PD_T));
            u4Flag = (UINT32)(DMX_MM_DRM_FLAG_KEY);
            rMMDrmRC4.u4ContectKeyAddr = (UINT32)(&_rSwdmxWmvInfo.u1DRMContentKey[0]);
            //rMMDrmRC4.u4ContectKeyAddr = (UINT32)(VOID*)&_au1ContentKey[0];
            rMMDrmRC4.u4ContectKeyLen = _rSwdmxWmvInfo.u4DRMKeySize;
            DMX_MM_SetPDDRM(u1VidPid,u4Flag,&rMMDrmRC4);
        }
    }
    if(_rSwdmxWmvInfo.fgEnAudio)
    {
        //u1AudPid = _rSwdmxWmvInfo.u1AudPid;
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        u1AudPid= _rSwdmxWmvInfo.u1AudPreBufPid[_rSwdmxWmvInfo.u4AStreamId];
        #else
        u1AudPid= _rSwdmxWmvInfo.u1AudPid;//todo-audioprebuffer
        #endif
        
        if(_rSwdmxWmvInfo.eDRMType == DRM_KEY_AES)
        {                        
            DMX_MM_DRM_AES_T rMMDrmAes;
            UINT32 u4Flag =0;
            
            x_memset((VOID*)&rMMDrmAes,0,sizeof(DMX_MM_DRM_AES_T));
            if (INPUT_SRC == MM_SRC_TYPE_NETWORK_NETFLIX)
            {
                u4Flag = (UINT32)(DMX_MM_DRM_FLAG_KEY);
                x_memset((VOID*)&rMMDrmAes.au1Key[0],0,sizeof(rMMDrmAes.au1Key));
                x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],_rSwdmxWmvInfo.u4DRMKeySize);
                
                DMX_MM_SetDRMMode(u1AudPid,DMX_DRM_MODE_ND);  // megaa 20100314 ToDo: check if DMX has new DRM APIs
                DMX_MM_SetNDDRM(u1AudPid,u4Flag,&rMMDrmAes);  // megaa 20100314 ToDo: check if DMX has new DRM APIs
            }
            else
            {
                /*u4Flag = (UINT32)(DMX_MM_DRM_FLAG_KEY | DMX_MM_DRM_FLAG_SAMPLE_ID);
                x_memset((VOID*)&rMMDrmAes.au1Key[0],0,sizeof(rMMDrmAes.au1Key));
                x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],_rSwdmxWmvInfo.u4DRMKeySize);
                
                //x_memcpy((VOID*)&rMMDrmAes.au1Key[0],(VOID*)&_au1ContentKey[0],sizeof(_au1ContentKey));
                x_memset((VOID*)&rMMDrmAes.au1SampleID[0],0,sizeof(rMMDrmAes.au1SampleID));

                DMX_MM_SetNDDRM(u1AudPid,u4Flag,&rMMDrmAes);
                */
                
                DMX_MM_SetDRMMode(u1AudPid,DMX_DRM_MODE_ND);  
            }
        }
        else if(_rSwdmxWmvInfo.eDRMType == DRM_KEY_RC4)
        {
            DMX_MM_DRM_PD_T rMMDrmRC4;
            UINT32 u4Flag = 0;
            
            x_memset((VOID*)&rMMDrmRC4,0,sizeof(DMX_MM_DRM_PD_T));
            u4Flag = (UINT32)(DMX_MM_DRM_FLAG_KEY);
            rMMDrmRC4.u4ContectKeyAddr = (UINT32)(&_rSwdmxWmvInfo.u1DRMContentKey[0]);
            //rMMDrmRC4.u4ContectKeyAddr = (UINT32)(VOID*)&_au1ContentKey[0];
            rMMDrmRC4.u4ContectKeyLen = _rSwdmxWmvInfo.u4DRMKeySize;
            #ifdef WMV_AUDIO_USE_PRE_BUFFER
            {
                UINT32 u1Idx = 0;
                for (u1Idx = 0;u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
                {
                    u1AudPid = _rSwdmxWmvInfo.u1AudPreBufPid[u1Idx];
                    DMX_MM_SetPDDRM(u1AudPid, u4Flag, &rMMDrmRC4);
                }
            }
            #else
            DMX_MM_SetPDDRM(u1AudPid, u4Flag, &rMMDrmRC4);
            #endif
            //DMX_MM_SetPDDRM(u1AudPid, u4Flag, &rMMDrmRC4);
        }
    }

    return TRUE;
        
}
BOOL _SWDMX_WMVInitDmx(SWDMX_INFO_T* prSwdmxInst, UINT8 u1HdrType,UINT8 u1Index,BOOL fgEnable,BOOL fgEos)
{
    UINT32 u4Flag;
    DMX_MM_T rDmxMMInfo;
    UINT8 u1VidPid=0;
    BOOL fgDmxEnabled=FALSE;
    x_memset(&rDmxMMInfo, 0, sizeof(DMX_MM_T));
    
    if (u1HdrType == eSWDMX_MEDIA_VIDEO && _rSwdmxWmvInfo.fgEnVideo)
    {
        rDmxMMInfo.fgEnable = fgEnable;
        rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_VIDEO;
        if(IS_NEWTRACK(prSwdmxInst,u1Index))
        {
           fgDmxEnabled=_rSwdmxWmvInfo.fgEnV2Dmx;
        }
        else
        {
           fgDmxEnabled=_rSwdmxWmvInfo.fgEnVDmx;
        }
#if 1  // for multi-instance
 #if 1  // v2
        DMX_SetToDecoder(TRUE);
 #else
        {
            DMX_DECODER_CALLBACKS_T rDecoderCallbacks;
            rDecoderCallbacks.pfnSendVideoPes = _WMVVideoCallback;//_VideoCallback;
            rDecoderCallbacks.pfnUpdateVideoWp = VDEC_SetWptr;
            //rDecoderCallbacks.pfnSendAudioPes = AUD_SendAudioPes;
            DMX_SetDecoderCallbacksSeparate(DMX_CB_FLAG_VIDEO, (const DMX_DECODER_CALLBACKS_T*)&rDecoderCallbacks);
            DMX_SetToDecoder(TRUE);
        }
 #endif
#endif
        if (!fgDmxEnabled)
        {
           // prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid = DMX_MUL_GetAvailablePidx(u1Index);
           
            u1VidPid=DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            LOG(0, "DMX_MUL_GetAvailablePidx video %d\n", u1VidPid);
#ifndef WMV_ALLOCATE_VFIFO
            if (!ENABLE_NEWTRACK(prSwdmxInst))
            {
                _WMVGetVideoFifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize);
            }
#else

#if 1
            _u4VFifoStartAddr = (UINT32)BSP_AllocAlignedDmaMemory(_u4WmvFifoSize,WMV_DMX_FIFO_ALIGNMENT);
#else /* need mentor's double check, follow David's mail: "Please review swdmx internal buffer use" */
			{
				UINT32 u4Addr = 0;
				UINT32 u4Size = _u4WmvFifoSize;
				UINT32 u4Align = WMV_DMX_FIFO_ALIGNMENT;
				SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align);
				_u4VFifoStartAddr = (UINT32)u4Addr;
			}
#endif

            if (_u4VFifoStartAddr != NULL)
            {
                rDmxMMInfo.u4BufAddr = _u4VFifoStartAddr;
                rDmxMMInfo.u4BufSize = _u4WmvFifoSize;
            }
            else
            {
                ASSERT(0);
                _SWDMX_GetVideoFifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize);
            }
#endif
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                if(IS_NEWTRACK(prSwdmxInst,u1Index))
                {
                    if (_WMVGetVideo2Fifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize))
                    {
                       _rSwdmxWmvInfo.u1Vid2Pid=u1VidPid;
                       rDmxMMInfo.u1ChannelId = prSwdmxInst->u1Vdec2Id;
                    }
                    else
                    {
                        LOG(0, "get video2 FIFO fail! return\n");
                        return FALSE;
                    }
                }
                else
                {
                    _WMVGetVideoFifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize);
                   _rSwdmxWmvInfo.u1VidPid=u1VidPid;
                   rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
                    //rDmxMMInfo.u4BufSize=rDmxMMInfo.u4BufSize>>1;
                }
            }
            else
            {
                _rSwdmxWmvInfo.u1VidPid=u1VidPid;
                rDmxMMInfo.u1ChannelId = prSwdmxInst->u1VdecId;
            }
            
            u4Flag = (UINT32)(DMX_MM_FLAG_TYPE | DMX_MM_FLAG_INSTANCE_TAG |
                              DMX_MM_FLAG_VALID | DMX_MM_FLAG_BUF |
                              DMX_MM_FLAG_SEARCH_START_CODE |
                              DMX_MM_FLAG_DEVICE_ID);

            rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
            rDmxMMInfo.fgAllocBuf = FALSE;
            if(_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_MJPEG)
            {
                rDmxMMInfo.u1DeviceId = VLD1; // video format all use VLD expect jpeg
            }
            else
            {        
                rDmxMMInfo.u1DeviceId = VLD0; // video format all use VLD expect jpeg
            }

            switch (_rSwdmxWmvInfo.eVideoType)
            {
            case ENUM_SWDMX_VDEC_WMV7:
            case ENUM_SWDMX_VDEC_WMV8:
            case ENUM_SWDMX_VDEC_WMV9:
            case ENUM_SWDMX_VDEC_DX311:
            case ENUM_SWDMX_VDEC_H263:
                LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
                rDmxMMInfo.fgSearchStartCode = FALSE;
                break;
            case ENUM_SWDMX_VDEC_MPEG4:
            case ENUM_SWDMX_VDEC_VC1:
            case ENUM_SWDMX_VDEC_H264:
                LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                rDmxMMInfo.fgSearchStartCode = TRUE;
                break;
            default:
                //ASSERT(0); //unknow wmv type
                //LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                //rDmxMMInfo.fgSearchStartCode = TRUE;
                break;
            }
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
            /* DTV00561903
             * if VFIFO comes from FBM, the memory may NOT continuous
             */
            if (!_rSwdmxWmvInfo.fgIs3DFile)
            {
                /* VFIFO2 will be merged with VFIFO1, VFIFO1=VFIFO1+VFIFO2 */
                UINT32 u4VfifoAddr;
                UINT32 u4VfifoSize = 0;
                UINT32 u4VfifoAddr2;
                UINT32 u4VfifoSize2 = 0;

                _WMVGetVideoFifo(prSwdmxInst->u1SwdmxId, &u4VfifoAddr, &u4VfifoSize);
                rDmxMMInfo.u4BufAddr = u4VfifoAddr;
                rDmxMMInfo.u4BufSize = u4VfifoSize;
                /* DTV00574854, DTV00575054
                 * if 5880_USE_384M, _WMVGetVideo2Fifo() will return FALSE. 
                 */
                if (_WMVGetVideo2Fifo(prSwdmxInst->u1SwdmxId, &u4VfifoAddr2, &u4VfifoSize2))
                {
                    LOG(0, "not 3D file, VFIFO2 will be merged with VFIFO1\n");
                    rDmxMMInfo.u4BufAddr = min(u4VfifoAddr, u4VfifoAddr2);
                    rDmxMMInfo.u4BufSize = u4VfifoSize + u4VfifoSize2;
                }
            }
#endif
            #if 0
            if (rDmxMMInfo.u4BufSize == 0x480000)
            {
                rDmxMMInfo.u4BufSize += 0x280000;
            }
            if (rDmxMMInfo.u4BufSize == 0x980000)
            {
                rDmxMMInfo.u4BufAddr += 0x280000;
                rDmxMMInfo.u4BufSize -= 0x280000;
            }
            #endif
            _u4VFifoStartAddr = rDmxMMInfo.u4BufAddr;
            _u4VFifoSize = rDmxMMInfo.u4BufSize;
            LOG(0,">>>>>>>>wmv vfifo size is 0x%x(pid:%d, SA:0x%x, 3D?%s, setRange?%s)\n",
                rDmxMMInfo.u4BufSize,
                u1VidPid,
                rDmxMMInfo.u4BufAddr,
                _rSwdmxWmvInfo.fgIs3DFile?"yes":"no",
                _rSwdmxWmvInfo.fgIsSetRangeInvoked?"yes":"no");
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
            _u4CommVFifoSize = rDmxMMInfo.u4BufSize;
#endif
            DMX_MM_Set(u1VidPid, u4Flag, &rDmxMMInfo);

            if (_rSwdmxWmvInfo.fgIsDRM)
            {
                _SWDMX_WMVDRMInitDmx(prSwdmxInst);
            } 
        }
        else
        {
        
            if (IS_NEWTRACK(prSwdmxInst,u1Index))
            {
                u1VidPid=_rSwdmxWmvInfo.u1Vid2Pid;
            }
            else
            {
                u1VidPid=_rSwdmxWmvInfo.u1VidPid;
            }
            
            u4Flag = DMX_MM_FLAG_SEARCH_START_CODE;
            if (fgEos)
            {
                rDmxMMInfo.fgSearchStartCode = FALSE;
            }
            else
            {
                switch (_rSwdmxWmvInfo.eVideoType)
                {
                case ENUM_SWDMX_VDEC_WMV7:
                case ENUM_SWDMX_VDEC_WMV8:
                case ENUM_SWDMX_VDEC_WMV9:
                case ENUM_SWDMX_VDEC_DX311:
                case ENUM_SWDMX_VDEC_H263:
                    LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = FALSE\n");
                    rDmxMMInfo.fgSearchStartCode = FALSE;
                    break;
                case ENUM_SWDMX_VDEC_MPEG4:
                case ENUM_SWDMX_VDEC_VC1:
                case ENUM_SWDMX_VDEC_H264:
                    LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                    rDmxMMInfo.fgSearchStartCode = TRUE;
                    break;
                default:
                    //ASSERT(0); //unknow wmv type
                    //LOG(7, "_SWDMX_SetVideoStreamID SearchStartCode = TRUE\n");
                    //rDmxMMInfo.fgSearchStartCode = TRUE;
                    break;
                }
            }
            DMX_MM_Set(u1VidPid, u4Flag, &rDmxMMInfo);
        }
    }
    else if (u1HdrType == eSWDMX_MEDIA_AUDIO && _rSwdmxWmvInfo.fgEnAudio)
    {
        UINT32 au4BufStart[2], au4BufEnd[2];
               
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        INT32 i = 0;
        UINT32 u4AudPreFifoAddr = 0;
        UINT32 u4AudPreFifoSize = 0;
        UINT32 u4AudTrackSize = 0;
        #endif

        DMX_SetToDecoder(TRUE);

        prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
        _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &rDmxMMInfo.u4BufAddr, &rDmxMMInfo.u4BufSize);

        u4Flag = (UINT32)(DMX_MM_FLAG_TYPE | DMX_MM_FLAG_VALID | DMX_MM_FLAG_INSTANCE_TAG
                          | DMX_MM_FLAG_BUF | DMX_MM_FLAG_SEARCH_START_CODE |
                          DMX_MM_FLAG_DEVICE_ID);

        rDmxMMInfo.fgEnable = fgEnable;
        #ifdef CC_DMX_PURE_AUDIO   // multi-instance
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
            rDmxMMInfo.ePidType = DMX_PID_TYPE_PURE_AUDIO;
        }
        else
        #endif
        {
            rDmxMMInfo.ePidType = DMX_PID_TYPE_ES_AUDIO;
        }

        rDmxMMInfo.fgSearchStartCode = FALSE;

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

        rDmxMMInfo.pvInstanceTag = (void*)prSwdmxInst;
        rDmxMMInfo.fgAllocBuf = FALSE;
        rDmxMMInfo.u1ChannelId = prSwdmxInst->u1AdecId;
        rDmxMMInfo.u1DeviceId = prSwdmxInst->u1AdecId;

        _u4AFifoStartAddr = rDmxMMInfo.u4BufAddr;
        _u4AFifoSize = rDmxMMInfo.u4BufSize;
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        _u4CommAFifoSize = rDmxMMInfo.u4BufSize;
        _u1CommAudDmxId = prSwdmxInst->u1DmxId;
        _u1CommAudPid = _rSwdmxWmvInfo.u1AudPid;
        if (prSwdmxInst->fgIsBadInterleaved)
        {
            if (_hCommBIQ == 0)
            {
                // create BI message Q
                VERIFY(x_msg_q_create(&_hCommBIQ, "WMVBIQ", sizeof(UINT32), 1) == OSR_OK);
            }
            else
            {
                // clear BI message Q
                vWmvBIGetMsg(FALSE);
            }
        }
#endif

#ifdef WMV_AUDIO_USE_PRE_BUFFER
        DMX_AudHandler_SetEnable(TRUE);
        DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid, u4Flag, &rDmxMMInfo);

        if (!DMX_AudHandler_Set(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid, TRUE))
        {
            LOG(0, "%s:%d, DMX_AudHandler_Set fail\n", __FUNCTION__, __LINE__);
        }

        if (_SWDMX_GetAudioPreFifo(prSwdmxInst->u1SwdmxId, &u4AudPreFifoAddr, &u4AudPreFifoSize))
        {
            //start address 16 bytes align
            if((u4AudPreFifoAddr%16))
            {
            	u4AudPreFifoAddr = 16 - (u4AudPreFifoAddr%16) + u4AudPreFifoAddr;
			    u4AudPreFifoSize = u4AudPreFifoSize - (16 - (u4AudPreFifoAddr%16));
            }
            u4AudTrackSize = u4AudPreFifoSize/WMV_MAX_AUDIO_TRACK;
            //audio fifo size 16 bytes align
            u4AudTrackSize = u4AudTrackSize - u4AudTrackSize%16;
            if(u4AudTrackSize > _u4AFifoSize)
            {
                u4AudTrackSize = _u4AFifoSize;
            }
            //u4AudTrackSize = u4AudPreFifoSize / WMV_MAX_AUDIO_TRACK;
            LOG(0,"%s:%d, _SWDMX_GetAudioPreFifo TrackSize is %x\n", __FUNCTION__, __LINE__,u4AudTrackSize);
        }
        else
        {
            LOG(0, "%s:%d, _SWDMX_GetAudioPreFifo fail\n", __FUNCTION__, __LINE__);
        }

        for (i = 0; i < WMV_MAX_AUDIO_TRACK; i++)  // !!!
        {
            rDmxMMInfo.fgAllocBuf = FALSE;
            rDmxMMInfo.u4BufSize = u4AudTrackSize;
            rDmxMMInfo.u4BufAddr = u4AudPreFifoAddr;
            _rSwdmxWmvInfo.u1AudPreBufPid[i] = DMX_MUL_GetAvailablePidx(prSwdmxInst->u1DmxId);
            if (_rSwdmxWmvInfo.u1AudPreBufPid[i] == 0xFF)
            {
                    LOG(0, "get  audio pre  pid fail\n");
                ASSERT(0);
            }
            VERIFY(DMX_MM_Set(_rSwdmxWmvInfo.u1AudPreBufPid[i], u4Flag, &rDmxMMInfo));
            if (!DMX_AudHandler_Set(_rSwdmxWmvInfo.u1AudPreBufPid[i], FALSE))
            {
                LOG(1, "%s:%d, DMX_AudHandler_Set fail\n", __FUNCTION__, __LINE__);
            }
            u4AudPreFifoAddr += u4AudTrackSize;
        }
        VERIFY(DMX_AudHandler_SetActivePidx(_rSwdmxWmvInfo.u1AudPreBufPid[0]));
        _rSwdmxWmvInfo.fgAlreadySet = FALSE;  
        
        #ifdef CC_DLNA_SUPPORT
        DMX_AudHandler_SetEnable(FALSE);
        #endif
#else
        DMX_MM_Set(_rSwdmxWmvInfo.u1AudPid, u4Flag, &rDmxMMInfo);
        LOG(0, "%s::DMX_MM_Set audPid:%d\n", __FUNCTION__, _rSwdmxWmvInfo.u1AudPid);
        _rSwdmxWmvInfo.fgAlreadySet = FALSE;  
#endif

    }
    return TRUE;
}

static VOID _SWDMX_WMVConsumeFeeder(SWDMX_INFO_T *prSwdmxInst)
{
    if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) !=
            FEEDER_E_OK)
    {
        LOG(1, "Feeder input buffer error.\n");
        //return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
    }
    LOG(5, "+++ update feeder read pointer! (0x%x)\n", _rFeederInfo.u4WriteAddr);
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rFeederInfo.u4WriteAddr);
    _rSwdmxWmvInfo.u4FeederReqID++;
}

#ifdef WMA_SEEK_OFFSET
INT32 _SWDMX_WMASeekOffset(SWDMX_INFO_T *prSwdmxInst)
{
   MM_RANGE_INFO_T* prRangeInfo=_prRangeInfo;
   SWDMX_WMV_INFO_T *prSwdmxwmvInfo=&_rSwdmxWmvInfo;
   FEEDER_TRANSMIT_COND rFeederReqcond;
   FEEDER_REQ_DATA_T rFeederReqdata;
   UINT64 u8StartSeekPos;
   UINT32 u4CheckPoint=0,u4MarkerPoint=0,u4RequestSize=0,u4CheckTime=0,u4CheckIndex=0;
   UCHAR *puData,*puMarker;
   BOOL fgFindPacket=FALSE;
   //prSwdmxwmvInfo->u4SampleRate=97353;
   
   if(prSwdmxwmvInfo->u4WMASeekTimes==0 && prSwdmxwmvInfo->fgSeekingNow==FALSE)
   {
     u8StartSeekPos=u8Div6464(prSwdmxwmvInfo->u8SeekPTS,90000,NULL)*prSwdmxwmvInfo->u4SampleRate;
     u8StartSeekPos=u8Div6464(u8StartSeekPos,8,NULL);
     u8StartSeekPos+=prRangeInfo->ui8_fileoffset+WMV_DATA_OBJ_HEADER;
   }
   else
   {
     u8StartSeekPos=prSwdmxwmvInfo->u8LastSeekPos;
   }
   u4RequestSize=prSwdmxwmvInfo->u4PacketSize*WMA_SEEK_PACKET_NUM;
   u4RequestSize=MIN(u4RequestSize,prSwdmxwmvInfo->u8FileSize-u8StartSeekPos);
   u4CheckTime=(u4RequestSize/prSwdmxwmvInfo->u4PacketSize);
   if(u4CheckTime<WMA_SEEK_MINCHECK_NUM)
   {
     prSwdmxwmvInfo->u4WMASeekTimes=0;
     prSwdmxwmvInfo->fgSeekingNow=FALSE;
     prSwdmxwmvInfo->u8LastSeekPos=0;
     return -1;
   }
   u4CheckTime--;
   if(!(prSwdmxwmvInfo->u8LastSeekPos && prSwdmxwmvInfo->u4WMASeekTimes==0))
   {
       _SWDMX_FlushReqMsgQ(prSwdmxInst->u1StcId);
       _SWDMX_ReceiveFeederAck(prSwdmxInst->u1StcId,&rFeederReqdata);
       FeederSetRing(prSwdmxInst->eFeederSrc,TRUE);
       FeederSetRequest(prSwdmxInst->eFeederSrc,u4RequestSize,u8StartSeekPos,&rFeederReqcond,prSwdmxwmvInfo->u4FeederReqID++);
   }
   _SWDMX_ReceiveFeederAck(prSwdmxInst->u1StcId,&rFeederReqdata);
   prSwdmxwmvInfo->fgSeekingNow=TRUE;
   prSwdmxwmvInfo->u8LastSeekPos=u8StartSeekPos;
   if(rFeederReqdata.eDataType==FEEDER_SOURCE_INVALID)
   {
     LOG(3,"WMASeek,Invalid data type\n");
     prSwdmxwmvInfo->fgFeederInvalid=TRUE;
     return -1;
   }
   
   if(rFeederReqdata.u4WriteAddr==0)
   {
     LOG(3,"WMASeek,Get data error\n");
     prSwdmxwmvInfo->fgSeekingNow=FALSE;
     prSwdmxwmvInfo->fgFeederInvalid=TRUE;
     return -1;
   }
   
   puData=(UCHAR *)(rFeederReqdata.u4WriteAddr+rFeederReqdata.u4AlignOffset);
   puMarker=(UCHAR *)(&prSwdmxwmvInfo->u8PacketMarker);
   u4RequestSize=rFeederReqdata.u4ReadSize-rFeederReqdata.u4AlignOffset;
   for(u4CheckPoint=0;u4CheckPoint<u4RequestSize-8;u4CheckPoint++)
   {
      u4MarkerPoint=0;
      if(!(u4CheckPoint&(1023)))
      {
         if(_SWDMX_ReceiveFeederAckNoWait(prSwdmxInst->u1StcId,&rFeederReqdata)==OSR_OK)
         {
            if(rFeederReqdata.eDataType==FEEDER_SOURCE_INVALID)
            {
                prSwdmxwmvInfo->fgFeederInvalid=TRUE;
                break;
            }
         }
         
      }
      while(puData[u4CheckPoint+u4MarkerPoint]==puMarker[u4MarkerPoint]&&u4MarkerPoint<8)u4MarkerPoint++;
      
      if(u4MarkerPoint>=prSwdmxwmvInfo->u4PacketMarkerLen)
      {  
         UINT32 u4CheckOffset;
         for(u4CheckIndex=1;u4CheckIndex<u4CheckTime;u4CheckIndex++)
         {
            u4MarkerPoint=0;
             u4CheckOffset=u4CheckPoint+u4CheckIndex*prSwdmxwmvInfo->u4PacketSize;
            while(puData[u4CheckOffset+u4MarkerPoint]==puMarker[u4MarkerPoint]&&u4MarkerPoint<prSwdmxwmvInfo->u4PacketMarkerLen) u4MarkerPoint++;
            if(u4MarkerPoint<prSwdmxwmvInfo->u4PacketMarkerLen)
            {
              break;
            }
         }

         if(u4CheckIndex==u4CheckTime)
         {
             LOG(3,"SWA Seek Find\n");
             prSwdmxwmvInfo->u8LastFileParsingPosition=u8StartSeekPos+u4CheckPoint;
             prSwdmxwmvInfo->u4CurParsingPacketNo = (UINT32)u8Div6464(prSwdmxwmvInfo->u8LastFileParsingPosition-(prRangeInfo->ui8_fileoffset+WMV_DATA_OBJ_HEADER),(UINT64)prSwdmxwmvInfo->u4PacketSize,NULL);
             prSwdmxwmvInfo->fgSeekingNow=FALSE;
             prSwdmxwmvInfo->u4WMASeekTimes=0;
             fgFindPacket=TRUE;
             break;
         }
      }
   }
   
   if(!fgFindPacket)
   {
   
      prSwdmxwmvInfo->u4WMASeekTimes++;
      if(prSwdmxwmvInfo->u4WMASeekTimes>WMA_MAX_SEEKTIME)
      {
          prSwdmxwmvInfo->fgSeekingNow=FALSE;
          prSwdmxwmvInfo->u8LastSeekPos=0; 
          prSwdmxwmvInfo->u4WMASeekTimes=0;
      }
      else
      {
          prSwdmxwmvInfo->fgSeekingNow=TRUE;
          prSwdmxwmvInfo->u8LastSeekPos=u8StartSeekPos+u4CheckPoint;
      }
      return -1;
   }
   else
   {
      return 0;
   }
}
#endif

#ifdef WMV_SEEK_OFFSET
VOID _SWDMX_WMVSeekOffset(SWDMX_INFO_T *prSwdmxInst,BOOL fgCurPos)
{

    _SWDMX_WMVConsumeFeeder(prSwdmxInst);

    if (_rSwdmxWmvInfo.u4PacketSize == 0 && _prRangeInfo !=0)
    {
        _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
    }

    if (_rSwdmxWmvInfo.u4PacketSize != 0)
    {
        if(fgCurPos)
        {
            VDP_POS_INTO_T rVdpPosInfo;
            if(VDP_GetPosInfo(prSwdmxInst->u1B2rId,&rVdpPosInfo))
            {
               _rSwdmxWmvInfo.u4CurParsingPacketNo=(UINT32)rVdpPosInfo.u8OffsetI;
               _rSwdmxWmvInfo.u8SeekPTS=rVdpPosInfo.u8Pts;
            }
        }
        else
        {
        	if(_prRangeList->fgSetRenderPosition)
        	{
                UINT64 u8Remainder=0;
                _prRangeList->fgSetRenderPosition = FALSE;
                _rSwdmxWmvInfo.u4CurParsingPacketNo = _prRangeList->u8OffsetI;//(UINT32)u8Div6464(_prRangeList->u8OffsetI,(UINT64)_rSwdmxWmvInfo.u4PacketSize,&u8Remainder);
                _rSwdmxWmvInfo.u4TotalPacketNo = (UINT32)u8Div6432((_rSwdmxWmvInfo.u8FileSize-WMV_DATA_OBJ_HEADER),
                	   _prRangeInfo->ui4_sz_min_acces_unt, &u8Remainder);
                if (_rSwdmxWmvInfo.u4TotalPacketNo == 0) 
                {
                   _rSwdmxWmvInfo.u4TotalPacketNo = 1;	 // this will cause _SWDMX_WMVSendEOS immediately
                }
                _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
                _rSwdmxWmvInfo.fgFirstFrame=FALSE;
                _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;
        	}
        	else
        	{
                _rSwdmxWmvInfo.u4CurParsingPacketNo = prSwdmxInst->rSeekOffset.ui8_i_frame_position;//(UINT32)u8Div6464(prSwdmxInst->rSeekOffset.ui8_i_frame_position,(UINT64)_rSwdmxWmvInfo.u4PacketSize,&u8Remainder);
        	}
        }

        _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset 
                + WMV_DATA_OBJ_HEADER 
                + ((UINT64)_rSwdmxWmvInfo.u4CurParsingPacketNo)*_rSwdmxWmvInfo.u4PacketSize;

        LOG(5,"[WMV]AB repeat go to seek offset!!!\n");
        _rSwdmxWmvInfo.u4PrevAudPts = 0;
    }
    else
    {
        LOG(2,"Send Eos 4\n");
        _SWDMX_WMVSendEOS(prSwdmxInst);
    }
}
#endif

#ifdef WMV_CHECK_KEY_INDEX_TABLE_ENTRY
static BOOL _SWDMX_WMVIsKeyIndexTableValid(SWDMX_INFO_T *prSwdmxInst)
{
    #if 1 //todo: check case: file size >4G
    INT32 i;
    UINT32 u4NumberOfEntry = 0;

    if (_rSwdmxWmvInfo.prWmvKeyIdxTbl != NULL)
    {
        u4NumberOfEntry = _rSwdmxWmvInfo.prWmvKeyIdxTbl->ui4_number_of_entry;
    }
    
    for (i = 0; i < u4NumberOfEntry ; i++)
    {
        UINT32 ui4Size = _rSwdmxWmvInfo.prWmvKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_size;
        if (ui4Size==0 || ui4Size > (UINT32)_rSwdmxWmvInfo.u8FileSize)
            return FALSE;       
    }
    return TRUE;
    #else
    #endif
}
#endif

VOID _SWDMX_WMVSeekTimebased(UINT8 u1SrcId,
                             SWDMX_RANGE_LIST_T* prRangeList,
                             SWDMX_CUR_CMD_T* prCmd)
{
//  UINT32 u4CurPtsTb = 0;
//  UINT16 u2DecodingOrder;
//  INT32 i4TempRef;
//  UINT64 u8FrameOffset =0;
//  UINT64 u8IFrameOffset =0;
    VDEC_ES_INFO_T *prVdecEsInfo;
    INT32 i4PlaySpeed = prCmd->i4PlaySpeed;
    //SWDMX_PLAY_MODE_T ePlayMode=prCmd->ePlayMode;
    //BOOL fgSpeedChange = prCmd->fgChange;
    //UINT32 _u4CmdSeekMode = prCmd->u4SeekMode; 
    //UINT64 u8SeekTime = prCmd->u8SeekTime; 
    UINT64 u8SeekTime = 0;
    
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    if (!prSwdmxInst)
    {
        return;
    }
    u8SeekTime = _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo;
    prVdecEsInfo = _VDEC_GetEsInfo(prSwdmxInst->u1VdecId);

    if (_rSwdmxWmvInfo.u4PacketSize == 0 && _prRangeInfo != NULL)
    {
        _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
    }

    _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;

    //todo //check if u8SeekTime is valid or not
    ////_rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u8SeekTime;
    if (_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES || i4PlaySpeed>0)
    {
        _rSwdmxWmvInfo.fgAudioNormalPlay = TRUE;
    }
    LOG(5, "tb seek 0x%llx\n", u8SeekTime);
    _rSwdmxWmvInfo.fgBlock = FALSE;
    _SWDMX_WMVInitVDmx(u1SrcId);

//          if (_rSwdmxWmvInfo.fgEnVideo)//protect func
//          {
//              VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder,
//                                  &u8FrameOffset, &u8IFrameOffset);
//          }

    if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
#ifdef WMA_SEEK_OFFSET
//      if(_rSwdmxWmvInfo.u4WMASeekTimes<WMA_MAX_SEEKTIME)
//      {
//         if(_SWDMX_WMASeekOffset(prSwdmxInst)<0)
//         {
//           return eSWDMX_HDR_PRS_SUCCEED;
//         }
//      }
//      else
//      {
//         _rSwdmxWmvInfo.fgSeekingNow=FALSE;
//         return eSWDMX_HDR_PRS_SUCCEED;
//      }
#else
        if (i4PlaySpeed < 0)
        {
            fgTrickTrigger[u1SrcId] = TRUE;
        }
//      if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
//      {
//          _SWDMX_WMVSeek(prSwdmxInst, u8SeekPos, prRangeList);
//           if(i4PlaySpeed<0)
//           {
//               fgTrickTrigger[u1SrcId]=TRUE;
//           }
//      }
//      else
//      {
//          _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER;
//          _rSwdmxWmvInfo.u4CurParsingPacketNo = 0;
//      }
#endif
    }

    _SWDMX_WMVConsumeFeeder(prSwdmxInst);
    
    _SWDMX_WMVTellVDecRenderPts(u1SrcId, i4PlaySpeed, u8SeekTime);
    _SWDMX_WMVFlushAVFifo(u1SrcId, u8SeekTime);
    if (_rSwdmxWmvInfo.fgEnVideo)//protect func
    {
        LOG(0,"VDEC_SetRenderFromPts : 0x%llx(%d.%05d)(tb)\n",
            u8SeekTime,
            (UINT32)u8SeekTime/90000,
            (UINT32)u8SeekTime%90000);
        VDEC_StartPTS(prSwdmxInst->u1VdecId,TRUE,(UINT32)u8SeekTime,NULL);
        if (ENABLE_NEWTRACK(prSwdmxInst))
        {
            VDEC_StartPTS(prSwdmxInst->u1Vdec2Id,TRUE,(UINT32)u8SeekTime,NULL);
        }
        VDP_SetSeek(prSwdmxInst->u1B2rId,1,u8SeekTime);
        LOG(0,"Set to B2r seek %lld\n",u8SeekTime);
    }
#ifdef WAIT_AUDIO_READY
    if (!_rSwdmxWmvInfo.fgVideoPause 
        && i4PlaySpeed != 1 
        && _rSwdmxWmvInfo.fgEnVideo 
        && _rSwdmxWmvInfo.fgEnAudio)
    {
        VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
        //_SWDMX_WMVWaitVDec(4);
        if (FALSE == _WMVWaitVDec(prSwdmxInst, __LINE__)) 
            return;
        VDEC_Pause(prSwdmxInst->u1VdecId);
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
            VDEC_Pause(prSwdmxInst->u1Vdec2Id);
        }
        _rSwdmxWmvInfo.fgVideoPause = TRUE;
    }
#endif
    STC_StopStc(prSwdmxInst->u1StcId);
    STC_SetStcValue(prSwdmxInst->u1StcId, (UINT32)u8SeekTime);
    
    _rSwdmxWmvInfo.u4DisplayPTS = u8SeekTime;
    _rSwdmxWmvInfo.u4PrevAudPts = 0;
    if (prVdecEsInfo != NULL)
    {
        //reset seek operation flag when speed change
        if (prVdecEsInfo->fgRenderPts)
        {
            prVdecEsInfo->fgRenderPts = FALSE;
        }
    }
#ifdef WMA_TRICK_MODE
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
        //_rSwdmxWmvInfo.fgFinishKeyTable=FALSE;
        _rSwdmxWmvInfo.u4IndexPts=0;
        _rSwdmxWmvInfo.u2SendPayloadNo = 0;
        _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
        _rSwdmxWmvInfo.fgBlock = FALSE;
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
    }
    else
#endif
    {
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid);
        if(ENABLE_NEWTRACK(prSwdmxInst))
         {
          DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid);
         }
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
    }

    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    {
        UINT8 u1Idx = 0;
        for (u1Idx = 0; u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
        {
            DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
        }
    }
    #endif

    _SWDMX_FlushReqMsgQ(u1SrcId);
    //_SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
//  _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
//  _rSwdmxWmvInfo.u4ConsumedSize = 0;
//  _rSwdmxWmvInfo.fgAlreadySet = FALSE;
        LOG(0, "currently3 _fgReceiveVideoPes is %s, set to FLASE!\n", 
            _fgReceiveVideoPes?"TRUE":"FALSE");
    _fgReceiveVideoPes = FALSE;
//  _fgReceiveAudioPes = FALSE;
//  _u4PrevFramePTS=0;
}


static VOID  _SWDMX_WMVTimebasedCalcNextTime(UINT8 u1SrcId, 
                                             UINT32 u4CurrentTime,
                                             INT32 i4PlaySpeed)
{
    //UINT64 _u8ReqTime = 0;
    //UINT64 _u8Remainder = 0;
    //UINT32 u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);

    UINT32 u4Speed;
    UINT32 u4CurPts = 0;
    UINT64 u8JumpCnt;
    UINT64 u8Remainder;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    UINT64 u8OirgVar = 0;
    UINT64 u8OirgVar_s = 0;
    UINT64 u8Ret = 0;
    UINT64 u8Ret_s = 0;

    SWDMX_WMV_VERIFY_NULL(prSwdmxInst);
    u8OirgVar = _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo;
    u8OirgVar_s = _SWDMX_Div6464(u8OirgVar, 
                                 (UINT64)90000, 
                                 &u8Remainder);

    if (u4CurrentTime == 0)
    {
        if (_rSwdmxWmvInfo.fgEnVideo)
        {
            u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
        }
        else if (_rSwdmxWmvInfo.fgEnAudio)
        {
            u4CurPts = STC_GetStcValue(prSwdmxInst->u1StcId);
        }
    }
    else
    {
        u4CurPts = u4CurrentTime;
    }

    u4Speed = abs(i4PlaySpeed);
    /* calculate jump method */
    /* -----------------------
        frequency:1/wait time (jump counts in one speed)
        step:     speed/frequency
       -----------------------
    */
    if (_rSwdmxWmvInfo.rTrickInterval.u4Seconds == 0)
    {
        u8JumpCnt = _SWDMX_Div6464((UINT64)1000000, 
                                   (UINT64)_rSwdmxWmvInfo.rTrickInterval.u4Micros, 
                                   &u8Remainder);
    }
    else
    {
        u8JumpCnt = 1;
    }
    if (u8JumpCnt > 0)
    {
        u4Speed = u4Speed / (UINT32)u8JumpCnt;
    }

    if (i4PlaySpeed < 0)
    {
        /* driver not support float operation,
        so  90* u4Speed replace (u4Speed / 1000 ) * 90000 */                
        if (u4CurPts > u4Speed * 90) 
        {
            u8Ret = u4CurPts - (u4Speed * 90); 
        }
        else
        {
            u8Ret = _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration;			
        }
    }
    else
    {
        if (u4CurPts + u4Speed * 90 < _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration)
        {
            u8Ret = u4CurPts + u4Speed * 90;                    
        }
        else
        {
            u8Ret = _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration;		
        }
    }
    if (u8Ret != _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration)
    {
        /* when slow backward, we may get same seek point */
        u8Ret_s = _SWDMX_Div6464(u8Ret, 
                                 (UINT64)90000, 
                                 &u8Remainder);
        if (u8Ret_s == u8OirgVar_s)
        {
            if (i4PlaySpeed < 0)
                u8Ret_s--;
            else
                u8Ret_s++;
        }
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u8Ret_s * 90000;
    }
    else
    {
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u8Ret;
    }
    LOG(5, "u4CurPts 0x%x(%d.%05d), next time is 0x%llx(%d.%05d)\n",
        u4CurPts,
        u4CurPts/90000,
        u4CurPts%90000,
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo,
        (UINT32)_rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo/90000,
        (UINT32)_rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo%90000);

    _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
}

VOID _SWDMX_WMVHandleSpeedChangeTimebased(UINT8 u1SrcId,
                                          //UINT64 u8SeekTime,
                                          SWDMX_RANGE_LIST_T* prRangeList, 
                                          SWDMX_CUR_CMD_T* prCmd)
{
    UINT32 u4CurPtsTb = 0;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    UINT8 u1Idx = 0;
    #endif
    
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);

    INT32 i4PlaySpeed = prCmd->i4PlaySpeed;
    SWDMX_PLAY_MODE_T ePlayMode=prCmd->ePlayMode;
    //BOOL fgSpeedChange = prCmd->fgChange;
    //UINT32 _u4CmdSeekMode = prCmd->u4SeekMode; 
    //UINT64 u8SeekTime = prCmd->u8SeekTime; 

//  UINT16 u2DecodingOrder;
//  INT32 i4TempRef;
//  UINT64 u8FrameOffset =0;
//  UINT64 u8IFrameOffset =0;

    if (!prSwdmxInst)
    {
        return;
    }

    if (    
            /* 1/(2~32)x => 2x */
            (!
                (_rSwdmxWmvInfo.i4WMVPlaySpeed > 1 
                && _rSwdmxWmvInfo.i4WMVPlaySpeed < 1000
                && i4PlaySpeed == 2000)
            ) 
        && 
            /* 1/(2~32)x => 1/(2~32)x
             * 1/(2~32)x => 2x
             */
            (_rSwdmxWmvInfo.i4WMVPlaySpeed > 1 
                && _rSwdmxWmvInfo.i4WMVPlaySpeed <= 1000
                && i4PlaySpeed != 1000
                && ePlayMode == SWDMX_PLAY_MODE_NORMAL
            )
        )
    {
        LOG(0, "speed change(tb,1) %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
        //do nothing
        if (_rSwdmxWmvInfo.fgVideoPause && i4PlaySpeed == 1)
        {
            _WMVStartToPlay(prSwdmxInst);
        }
        
        LOG(0, "flush(video:no,audio:yes) speed(%d->%d)\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);

        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        for (u1Idx = 0;u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
        {
            DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
        }
        DMX_AudHandler_SetActivePidx(DMX_AUDIO_HANDLER_NULL_PIDX);
        #endif
        
        _rSwdmxWmvInfo.fgAlreadySet = TRUE;
        return;
    }
#ifdef WMA_TRICK_MODE_TIMEBASED
    else if ((_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
             && (i4PlaySpeed >= 2000)
             && (_rSwdmxWmvInfo.i4WMVPlaySpeed > 0))
    {
        LOG(0, "speed change(tb,2) %d -> %d, do nothing\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
        //nothing to do
        return;
    }
#endif

    /* time based solution maybe have no index table */
//  if (_eCurFMT==SWDMX_FMT_WMA_AUDIO_ES && _rSwdmxWmvInfo.prWmvKeyIdxTbl == NULL)
//  {
//      return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
//  }

    /* get current PTS firstly */
#ifdef WMA_TRICK_MODE_TIMEBASED
    if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
        if((_rSwdmxWmvInfo.i4WMVPlaySpeed!=1000&&i4PlaySpeed==1000)||(i4PlaySpeed<0))
        {
            UINT32 u4DelayTime = 0;
            while (u4DelayTime < 20
                   && AUD_IsDecoderStop(prSwdmxInst->u1AdecId))
            {
               x_thread_delay(50);
               u4DelayTime++;
            }
            if (u4DelayTime != 20)
            {
                //AUD_MMQueueSyncInfo(prSwdmxInst->u1AdecId,0,0,TRUE);
            }
            else
            {
                LOG(0, "Speed change, but audio is stop now\n");
                return;
            }
            AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4CurPtsTb, FALSE);
            if (u4CurPtsTb == 0 
                && AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId))
            {
                AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4CurPtsTb, FALSE);
            }
        }
        else if(AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId))
        {
            AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4CurPtsTb, FALSE);
        }
        
        if(u4CurPtsTb == 0)
        {        
            LOG(0, "First CurPts iz 0(tb)\n");
            u4CurPtsTb = STC_GetStcValue(prSwdmxInst->u1StcId);
            LOG(0, "STC Pts iz (%d,%d)(tb)\n", u4CurPtsTb, u4CurPtsTb/90000);
        }

        if (u1SrcId < SWDMX_SOURCE_MAX)
        {
            if (i4PlaySpeed < 0)
            { 
                if (u4CurPtsTb == 0 
                   || u4CurPtsTb > _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration)
                {
                    u4CurPtsTb = u4AudDspPts[u1SrcId];
                }
                STC_StopStc(prSwdmxInst->u1StcId);
                STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPtsTb);
                u4AudDspPts[u1SrcId] = u4CurPtsTb;
                fgTrickTrigger[u1SrcId] = TRUE;
            }
            else
            {  
               u4AudDspPts[u1SrcId]=1;                    
            }
        }

        //AUD_GetDspPtsInfo(prSwdmxInst->u1AdecId, &u4CurPts, &u4PrsPos);
        LOG(0, "First CurPtsTb: 0x%x(%d.%05d)\n", 
            u4CurPtsTb, 
            u4CurPtsTb/90000,
            u4CurPtsTb%90000);
    }
    else
#endif
    {
        u4CurPtsTb = VDP_GetPts(prSwdmxInst->u1B2rId);
    }
    _rSwdmxWmvInfo.u4DisplayPTS = u4CurPtsTb;

    prVdecEsInfo = _VDEC_GetEsInfo(prSwdmxInst->u1VdecId);
    //MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
    //VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
    if (prVdecEsInfo != NULL)
    {
        //reset seek operation flag when speed change
        if (prVdecEsInfo->fgRenderPts)
        {
            prVdecEsInfo->fgRenderPts = FALSE;
        }
    }
#ifdef WMA_TRICK_MODE_TIMEBASED
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
        //_rSwdmxWmvInfo.fgFinishKeyTable=FALSE;
        _rSwdmxWmvInfo.u4IndexPts=0;
        _rSwdmxWmvInfo.u2SendPayloadNo = 0;
        _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
        _rSwdmxWmvInfo.fgBlock = FALSE;
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
    }
    else
#endif
    {
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid);
        if (ENABLE_NEWTRACK(prSwdmxInst))
        {
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid);
        }
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
    }

    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    for (u1Idx = 0; u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
    {
        DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
    }
    #endif

    _SWDMX_FlushReqMsgQ(u1SrcId);
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.u4ConsumedSize = 0;
    _rSwdmxWmvInfo.fgAlreadySet = FALSE;
    LOG(0, "currently _fgReceiveVideoPes is %s, set to FALSE\n", 
        _fgReceiveVideoPes?"TRUE":"FALSE");
    _fgReceiveVideoPes = FALSE;
    _fgReceiveAudioPes = FALSE;
    _u4PrevFramePTS=0;
//  _rSwdmxWmvInfo.u4AvgPktCountPerEntry = 0;
//  _rSwdmxWmvInfo.u4TotalPktCountPerEntry = 0;
//  _rSwdmxWmvInfo.u4TrickModeParsingActionCount = 0;
    //VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
    //////LOG(0,"VDEC_SetRenderFromPts : %x\n",u4CurPts);//

    // Things for 32/16/8/4 -> 1X are the same
    if ((_rSwdmxWmvInfo.i4WMVPlaySpeed >= 2000 || _rSwdmxWmvInfo.i4WMVPlaySpeed < 0
            || (_rSwdmxWmvInfo.i4WMVPlaySpeed >=1 && _rSwdmxWmvInfo.i4WMVPlaySpeed<1000))
            && (i4PlaySpeed == 1000 || i4PlaySpeed==2000 || i4PlaySpeed==500))
    {
        //UINT32 u4RemainedDataSize;
        LOG(0, "speed change(tb,3) %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
        
#ifdef WMA_TRICK_MODE_TIMEBASED
        if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES 
            &&   ( (_rSwdmxWmvInfo.i4WMVPlaySpeed<0 || _rSwdmxWmvInfo.i4WMVPlaySpeed>=2000)
                  &&(i4PlaySpeed==1000 || i4PlaySpeed==2000)
                  )
           )
        {
            _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
             _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
            _SWDMX_WMVSeekTimebased(u1SrcId, prRangeList, prCmd);
            ////LOG(2,"FilePos=%d,PacketNumber=%d\n",_rSwdmxWmvInfo.u8LastFileParsingPosition ,_rSwdmxWmvInfo.u4CurParsingPacketNo);
            _rSwdmxWmvInfo.fgFinishKeyTable=FALSE;
            _rSwdmxWmvInfo.u2SendPayloadNo = 0;
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
            _rSwdmxWmvInfo.fgBlock = FALSE;
            if(u1SrcId<SWDMX_SOURCE_MAX)
            {
                u4AudDspPts[u1SrcId]=1;
            }
            _SWDMX_WMVConsumeFeeder(prSwdmxInst);
            STC_StopStc(prSwdmxInst->u1StcId);
            STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPtsTb);
        }
        else
#endif
        {
//          VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder,
//                          &u8FrameOffset, &u8IFrameOffset);
            _rSwdmxWmvInfo.u2SendPayloadNo = 0;
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
            _rSwdmxWmvInfo.fgBlock = FALSE;
            //_rSwdmxWmvInfo.u4CurParsingPacketNo = (UINT32)u8IFrameOffset;
#ifdef CC_3D_MM_DS_SUPPORT
            if (ENABLE_NEWTRACK(prSwdmxInst))
            {
                _WMVChkResume1XPos(prSwdmxInst);
            }
#endif
            ////_rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER +
            ////_rSwdmxWmvInfo.u4CurParsingPacketNo*_rSwdmxWmvInfo.u4PacketSize;

            //u4RemainedDataSize = _rSwdmxWmvInfo.u4DataObjSize-
            //(_rSwdmxWmvInfo.u8LastFileParsingPosition-prRangeList->prDmxRangeInfo->ui8_fileoffset);
            
            _SWDMX_WMVConsumeFeeder(prSwdmxInst);
            //since pts is not precious, jump to next 2 frame to avoid frame flicking
            if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1 
                && _rSwdmxWmvInfo.i4WMVPlaySpeed == 2000)
            {
                u4CurPtsTb += 6000;
            }
        
            if (_rSwdmxWmvInfo.i4WMVPlaySpeed >= 1 
                && _rSwdmxWmvInfo.i4WMVPlaySpeed < 1000
                && i4PlaySpeed == 1000)
            {
                UINT32 u4CurSTC;
                u4CurSTC = STC_GetStcValue(prSwdmxInst->u1StcId);
                u4CurPtsTb = max(u4CurPtsTb,u4CurSTC);
            }
        
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPtsTb); //maiyou
            VDEC_StartPTS(prSwdmxInst->u1VdecId, TRUE, u4CurPtsTb, NULL); 
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPtsTb); 
                VDEC_StartPTS(prSwdmxInst->u1Vdec2Id, TRUE, u4CurPtsTb, NULL); 
            }
            LOG(3, "vdp get pts: 0x%x(%d.%05d,tb)\n",
                u4CurPtsTb,
                u4CurPtsTb/90000,
                u4CurPtsTb%90000);
#ifdef WAIT_AUDIO_READY
            if (!_rSwdmxWmvInfo.fgVideoPause 
                && _rSwdmxWmvInfo.fgEnVideo 
                && _rSwdmxWmvInfo.fgEnAudio)
            {
                VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                //_SWDMX_WMVWaitVDec(5);
                if (FALSE == _WMVWaitVDec(prSwdmxInst, __LINE__)) 
                    return;
                VDEC_Pause(prSwdmxInst->u1VdecId);
                if(ENABLE_NEWTRACK(prSwdmxInst))
                {
                    VDEC_Pause(prSwdmxInst->u1Vdec2Id);
                }
                _rSwdmxWmvInfo.fgVideoPause = TRUE;
            }
#endif 
            STC_StopStc(prSwdmxInst->u1StcId);
            STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPtsTb);

            _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
            _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
            #if 0
            _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 0, 0, FALSE);

            /* to bypass setFeederRequest(0) */
            if (_rSwdmxWmvInfo.fgFirstReqFeederData)
                _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
            #endif
        }
    }
    #if 1
    else /*if ((_rSwdmxWmvInfo.i4WMVPlaySpeed > 0 && i4PlaySpeed < 0)
             || (i4PlaySpeed >= 2000)
             || (i4PlaySpeed <= -2000))
        */
    {
        LOG(0, "speed change(tb,4) %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
        #if 0
        _SWDMX_FlushReqMsgQ(u1SrcId);
        _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
        _SWDMX_WMVConsumeFeeder(prSwdmxInst);
        _SWDMX_WMVFlushAVFifo(u1SrcId, u4CurPtsTb);
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId,
                        (UINT32)u4CurPtsTb);
        _rSwdmxWmvInfo.u4DisplayPTS = u4CurPtsTb; //record this for future comparison
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
        _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;
        _rSwdmxWmvInfo.fgBlock = FALSE;
        _SWDMX_WMVTimebasedCalcNextTime(u1SrcId, 
                                        u4CurPtsTb, 
                                        i4PlaySpeed);

        #else
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
        _SWDMX_WMVSeekTimebased(u1SrcId, prRangeList, prCmd);
        _SWDMX_WMVTimebasedCalcNextTime(u1SrcId, 
                                        u4CurPtsTb, 
                                        i4PlaySpeed);
        #endif
        /* trigger a feeder request */
        _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 
                                             (TIMEBASED_TRICKMODE_WMV_REQ_SIZE/_rSwdmxWmvInfo.u4PacketSize)*_rSwdmxWmvInfo.u4PacketSize,
                                             0,
                                             TRUE);
    }
    #else
    // Things for 32/16/8/4/2X/1X -> -2X/-4X/-8X/-16X/-32X are the same
    else if (_rSwdmxWmvInfo.i4WMVPlaySpeed > 0 && i4PlaySpeed < 0)
    {
        LOG(0, "(2)SpeedChange(tb) %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
        #if 0
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
        _SWDMX_WMVSeekTimebased(u1SrcId, prRangeList, prCmd);
//      if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
//      {
//          _SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
//      }
//      else
//      {
//         _SWDMX_WMVSeekOffset(prSwdmxInst,TRUE);
//      }
        
        #ifdef WMA_TRICK_MODE
        if (_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES)
        #endif
        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0);//maiyou
        if (ENABLE_NEWTRACK(prSwdmxInst))
        {
            VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, 0);
        }
        #else
        _SWDMX_FlushReqMsgQ(u1SrcId);
        _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
        _SWDMX_WMVConsumeFeeder(prSwdmxInst);
        //_SWDMX_WMVTellVDecRenderPts(u1SrcId, i4PlaySpeed, u4CurPtsTb);     
        _SWDMX_WMVFlushAVFifo(u1SrcId, u4CurPtsTb);
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId,
                        (UINT32)u4CurPtsTb);
        _rSwdmxWmvInfo.u4DisplayPTS = u4CurPtsTb; //record this for future comparison
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
        _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;
        _rSwdmxWmvInfo.fgBlock = FALSE;
        _SWDMX_WMVTimebasedCalcNextTime(u1SrcId, 
                                        u4CurPtsTb, 
                                        i4PlaySpeed);
        #endif
    }
    //else if( _rSwdmxWmvInfo.i4WMVPlaySpeed >=2000 && i4PlaySpeed>2000)
    else if (i4PlaySpeed >= 2000)
    {
        LOG(0, "(3)SpeedChange(tb) %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);

        _SWDMX_FlushReqMsgQ(u1SrcId);
        _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
        _SWDMX_WMVConsumeFeeder(prSwdmxInst);
        //_SWDMX_WMVTellVDecRenderPts(u1SrcId, i4PlaySpeed, u4CurPtsTb);     
        _SWDMX_WMVFlushAVFifo(u1SrcId, u4CurPtsTb);
        STC_StopStc(prSwdmxInst->u1StcId);
        STC_SetStcValue(prSwdmxInst->u1StcId,
                        (UINT32)u4CurPtsTb);
        _rSwdmxWmvInfo.u4DisplayPTS = u4CurPtsTb; //record this for future comparison
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
        _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;
        _rSwdmxWmvInfo.fgBlock = FALSE;
        _SWDMX_WMVTimebasedCalcNextTime(u1SrcId, 
                                        u4CurPtsTb, 
                                        i4PlaySpeed);
        /* trigger a feeder request */
        _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 
                                             (TIMEBASED_TRICKMODE_WMV_REQ_SIZE/_rSwdmxWmvInfo.u4PacketSize)*_rSwdmxWmvInfo.u4PacketSize,
                                             0,
                                             TRUE);
    }
    else if (i4PlaySpeed <= -2000)
    {
        LOG(0, "(4)SpeedChange(tb) %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
#ifdef WMA_TRICK_MODE
        _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
        _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = u4CurPtsTb;
        _SWDMX_WMVSeekTimebased(u1SrcId, prRangeList, prCmd);
//      if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
//      {
//          _SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
//      }
//      else
//      {
//         _SWDMX_WMVSeekOffset(prSwdmxInst,TRUE);
//      }
        if (_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES)
#endif
        { 
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0);
            if (ENABLE_NEWTRACK(prSwdmxInst))
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, 0);
            }
        }
    }
    else
    {
        //null operation
        LOG(0, "noop here, speed: %d -> %d\n", 
            _rSwdmxWmvInfo.i4WMVPlaySpeed, 
            i4PlaySpeed);
    }
    #endif
}

static BOOL _WMVSearchKeyIdxTblByPts(SWDMX_INFO_T *prSwdmxInst,
                                     MM_RANGE_ELMT_IDXTBL_T* pt_tbl,
                                     UINT64 u8SeekPts,
                                     UINT32 u4Entries,
                                     UINT64* pu8AbsOfst, /* output param */
                                     UINT32* pu4FoundPts, /* output param */
                                     UINT32* pu4FoundIdx /* output param */)
{
    UINT32 i = 0;
    UINT32 ui4Pts = 0;
    UINT32 ui4RelatOfst = 0;
    MM_RANGE_ELMT_IDXTBL_T* pKeyIdxTbl = pt_tbl;

    ASSERT(NULL != pKeyIdxTbl);
    ASSERT(NULL != pu8AbsOfst);
    ASSERT(NULL != pu4FoundPts);
    ASSERT(NULL != pu4FoundIdx);

    for (i = 0; i < u4Entries ; i++)
    {
        UINT32 ui4Size = pKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_size;
        ui4Pts  = pKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_pts;
        ui4RelatOfst = pKeyIdxTbl->u.pt_pb_idx_tbl_entry[i].ui4_relative_offset;
        if (ui4Pts >= (UINT32)u8SeekPts)
        {
            if (ui4Size != 0 
                && (ui4Size < _rSwdmxWmvInfo.u8FileSize))
            {
                *pu8AbsOfst = pKeyIdxTbl->ui8_base_offset + ui4RelatOfst;
                *pu4FoundPts = ui4Pts;
                *pu4FoundIdx = i;
                LOG(0, "Partial entry#%d, PTS: 0x%x, AbsOfst: 0x%llx\n",
                    *pu4FoundIdx,
                    *pu4FoundPts,
                    *pu8AbsOfst);
                return TRUE;
            }
            else
            {
                LOG(0, "meet bad entry #%d(%d), skip!!\n", i, ui4Size);  
            }
        }
    }
    *pu8AbsOfst = pKeyIdxTbl->ui8_base_offset + ui4RelatOfst;
    *pu4FoundPts = ui4Pts;
    *pu4FoundIdx = u4Entries - 1;
    return FALSE;
}

static BOOL _WMVSearchAllEntriesByPts(SWDMX_INFO_T *prSwdmxInst,
                                      UINT64 u8SeekPts,
                                      UINT64* pu8AbsOfst, /* output param */
                                      UINT32* pu4FoundPts, /* output param */
                                      UINT32* pu4FoundIdx /* output param */)
{
    UINT32 u4SearchedIdx = 0;
    MM_RANGE_ELMT_IDXTBL_T* pt_tbl = NULL;    
    UINT32 u4TotalEntries = 0;    
    MM_RANGE_ELMT_IDXTBL_T* pt_partial_tbl = NULL;
    UINT64 u8AbsOfst = 0;
    UINT32 u4Pts = 0;
    UINT32 u4Idx = 0;
    UINT32 j = 0;

    pt_tbl = _rSwdmxWmvInfo.prWmvKeyIdxTbl; //todo: consider "two key index table"
    ASSERT(pt_tbl != NULL);
    ASSERT(pu8AbsOfst != NULL);
    ASSERT(pu4FoundPts != NULL);
    ASSERT(pu4FoundIdx != NULL);

    u4TotalEntries = pt_tbl->ui4_number_of_entry;    

    if (NULL == pt_tbl->pt_next_partial) /* filesize < 4G, only ONE key index table */
    {
        return _WMVSearchKeyIdxTblByPts(prSwdmxInst,
                                        pt_tbl,
                                        u8SeekPts,
                                        u4TotalEntries,
                                        pu8AbsOfst,
                                        pu4FoundPts,
                                        pu4FoundIdx);
    }

    /* filesize > 4G, loop all partial tables.
       there're two partial tables at least. */
    pt_partial_tbl = pt_tbl->pt_next_partial;
    j = 0;
    while (pt_partial_tbl != NULL)
    {
        if (_WMVSearchKeyIdxTblByPts(prSwdmxInst,
                                     pt_partial_tbl,
                                     u8SeekPts,
                                     pt_partial_tbl->ui4_number_of_entry,
                                     &u8AbsOfst,
                                     &u4Pts,
                                     &u4Idx))
        {
            *pu8AbsOfst = u8AbsOfst;
            *pu4FoundPts = u4Pts;
            *pu4FoundIdx = u4SearchedIdx + u4Idx + j;
            LOG(0, "Final entry#%d in partial#%d, PTS: 0x%x, AbsOfst: 0x%llx\n",
                *pu4FoundIdx,
                j,
                *pu4FoundPts,
                *pu8AbsOfst);
            return TRUE;
        }
        LOG(0, "found nothing in partial#%d [%d-%d], search next partial\n",
            j,
            u4SearchedIdx + j,
            u4SearchedIdx + u4Idx + j);
        u4SearchedIdx += u4Idx;
        j++;
        pt_partial_tbl = pt_partial_tbl->pt_next_partial;
    }
    *pu8AbsOfst = u8AbsOfst;
    *pu4FoundPts = u4Pts;
    *pu4FoundIdx = u4SearchedIdx;
    return FALSE;
}


VOID _SWDMX_WMVSeek(SWDMX_INFO_T *prSwdmxInst, UINT64 u8SeekTime, SWDMX_RANGE_LIST_T* prRangeList)
{
    UINT64 u8AbsOfst = 0;
    UINT32 u4FoundPts = 0;
    UINT32 u4FoundIdx = 0;

    if (_rSwdmxWmvInfo.prWmvKeyIdxTbl == NULL)
    {
        LOG(0,"_SWDMX_WMVSeek : _rSwdmxWmvInfo.prWmvKeyIdxTbl == NULL\n");
        //ASSERT(_rSwdmxWmvInfo.prWmvKeyIdxTbl != NULL);
        return;
    }

    LOG(0, "_SWDMX_WMVSeek u8SeekTime = 0x%llx\n", u8SeekTime);

    if (_WMVSearchAllEntriesByPts(prSwdmxInst,
                                  u8SeekTime,
                                  &u8AbsOfst,
                                  &u4FoundPts,
                                  &u4FoundIdx))
    {
        if (u4FoundIdx == 0)
        {
            _rSwdmxWmvInfo.u4CurKeyIdx = 0;
        }
        else
        {
            if (u4FoundPts == (UINT32)u8SeekTime || _rSwdmxWmvInfo.i4WMVPlaySpeed > 0)
                _rSwdmxWmvInfo.u4CurKeyIdx = u4FoundIdx;
            else
                _rSwdmxWmvInfo.u4CurKeyIdx = u4FoundIdx - 1;
        }
    }
    else
    {
        LOG(0, "search entry fail(pts: 0x%x), use last entry!\n", (UINT32)u8SeekTime);
        _rSwdmxWmvInfo.u4CurKeyIdx = u4FoundIdx;
    }

    _SWDMX_WMVConsumeFeeder(prSwdmxInst);

    if (_rSwdmxWmvInfo.u4PacketSize == 0 && _prRangeInfo !=0)
    {
        _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
    }

    if (_rSwdmxWmvInfo.u4PacketSize != 0)
    {        
        UINT64 u8Remainder = 0;

        if (u8SeekTime)
        {
            _rSwdmxWmvInfo.u4CurParsingPacketNo = 
                (UINT32)u8Div6432(u8AbsOfst 
                                    - _prRangeInfo->ui8_fileoffset 
                                    - WMV_DATA_OBJ_HEADER,
                                  _rSwdmxWmvInfo.u4PacketSize, 
                                  &u8Remainder);
            _rSwdmxWmvInfo.u8SeekPTS = u4FoundPts;
        }
        else
        {
            _rSwdmxWmvInfo.u4CurParsingPacketNo = 0;
            _rSwdmxWmvInfo.u8SeekPTS = 0;
        }
        _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset 
                + WMV_DATA_OBJ_HEADER 
                + ((UINT64)_rSwdmxWmvInfo.u4CurParsingPacketNo)*_rSwdmxWmvInfo.u4PacketSize;
        LOG(0, "_SWDMX_WmvSeek LastFilePos 0x%llx, pktno %d/%d\n", 
            _rSwdmxWmvInfo.u8LastFileParsingPosition,
            _rSwdmxWmvInfo.u4CurParsingPacketNo,
            _rSwdmxWmvInfo.u4TotalPacketNo);
        
        if(_rSwdmxWmvInfo.fgFirstFrame)
        {
            _rSwdmxWmvInfo.u8BeginParseOfst=_rSwdmxWmvInfo.u8LastFileParsingPosition;
        	_rSwdmxWmvInfo.fgVideoNormalPlay=FALSE;        	
        }                              
        _rSwdmxWmvInfo.u4PrevAudPts = 0;
        _prRangeList->fgSetRenderPosition = FALSE;
        UNUSED(u8Remainder);
    }
    else
    {
    
        LOG(2,"Send Eos 5\n");
        _SWDMX_WMVSendEOS(prSwdmxInst);
    }
    _rSwdmxWmvInfo.fgVideoNormalPlay = FALSE;
    if (_rSwdmxWmvInfo.fgEnVideo)
    {
        VDP_SetSeek(prSwdmxInst->u1B2rId,1,u8SeekTime);
    }
    LOG(0, "_SWDMX_WmvSeek Key = %ld\n", _rSwdmxWmvInfo.u4CurKeyIdx);
    
}


BOOL _SWDMX_WMVTrickModeParsingTimebased(UINT8 u1SrcId)
{
#if 0
#else
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    UINT32 u4DemuxSize = 0;
    UINT32 u4DemuxPacketCount = 0;
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;
	//BOOL fgReqedData=FALSE;
    UINT32 i=0;
    //VDEC_ES_INFO_T *prVdecEsInfo = NULL;     
    BOOL fgPktCntChanged = FALSE;   
    BOOL fgGotIFrame = FALSE;
    BOOL fgRereqData = FALSE;
    BOOL fgRet = FALSE;
    UINT16 u2Cnt = 0;
    //UINT32 u4CurrentPos = 0;

    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);
    LOG(5, "---trick mode parsing begin\n");

NEXT_I_FRAME_TIMEBASED:
    //1. calculate new IFrame time
    if (fgRereqData || _fgReceiveVideoPes)
    {
        LOG(5, "---RecvVideoPes(didn't get IFrame), "
               "do trick req (again) using pts 0x%llx\n", 
            _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo+3000);
        _SWDMX_WMVTimebasedCalcNextTime(u1SrcId, 
                                        (UINT32)(_rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo+3000), 
                                        _rSwdmxWmvInfo.i4WMVPlaySpeed);        
    //}

        //1.1 check if EOS
        if (_rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo 
            == _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration)
        {
            //should EOS here
            LOG(5, "u8TimebasedTimeToBeSeekedTo == ui8_pb_duration, send eos\n");
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return TRUE;
        }

    //2. req data and get data, then parse data
    //_SWDMX_WMVFlushAVFifo(u1SrcId, 0xDEADDEAD);//cannot clear VFIFO here!!
    //
    //2.1 req data
//  if (fgRereqData || _fgReceiveVideoPes)
//  {
        if (!_SWDMX_WMVIsFeederBufferFull(prSwdmxInst))
        {
            #if 0
            u4DemuxPacketCount = TIMEBASED_TRICK_REQ_PACKET_COUNT;
            u4DemuxSize = u4DemuxPacketCount * _rSwdmxWmvInfo.u4PacketSize;
            #else
            /* currently, in time based mode, WMA trick is not support.
             * so use FEEDER_SEGMENT.
             */
            //u4DemuxPacketCount = FEEDER_SEGMENT / _rSwdmxWmvInfo.u4PacketSize;
            u4DemuxPacketCount = TIMEBASED_TRICKMODE_WMV_REQ_SIZE 
                                 / _rSwdmxWmvInfo.u4PacketSize;
            u4DemuxSize = u4DemuxPacketCount * _rSwdmxWmvInfo.u4PacketSize;
            
            #endif
            _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
            _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 
                                                 u4DemuxSize,
                                                 0,
                                                 TRUE);
        }
    }
    //2.2 get data
    fgRet = _SWDMX_WMVGetFeederDataTimebased(prSwdmxInst, FALSE);
    if (!fgRet)
    {
        if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
        {
            //LOG(5, "---goto _trick_bypass_feeder_invalid\n");
            LOG(5, "---feeder invalid or demux finished!(tb,trick) return\n");
            x_thread_delay(20);
            return FALSE;
            //goto  _trick_bypass_feeder_invalid;
        } 
    }
    else
    {
        //LOG(5, "---get feeder data OK!(tb,trick)\n");
    }
    if (!_SWDMX_WMVFeederIsReady(prSwdmxInst))
    {
        x_thread_delay(50);
        LOG(0, "!_SWDMX_WMVFeederIsReady(prSwdmxInst), sleep 50ms\n");
        //goto  _trick_bypass_feeder_invalid;
        return TRUE;
    }

//  if (!fgReqedData)
//      return FALSE;
    fgRereqData = FALSE;
    x_memset(&rWmvFeederReqData, 0, sizeof(rWmvFeederReqData));
    zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);
    VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
    if (u2Cnt == 0)  // which will cause deadlock at the x_msg_q_receive below
    {
        LOG(2, "no msg in queue, return\n");//send Eos 9(tb,trick)\n");
        _SWDMX_WMVGetFeederDataTimebased(prSwdmxInst, FALSE);
        //_SWDMX_WMVSendEOS(prSwdmxInst);
        return TRUE;
    }
    else
    {
        LOG(5, "---%d msg(s) in queue(trick)\n", u2Cnt);
    }
    i4Ret = x_msg_q_receive(&u2MsgQIdx, 
                            &rWmvFeederReqData, 
                            &zMsgSize,
                            &(_hWMVFeederDataQue), 
                            1, 
                            X_MSGQ_OPTION_WAIT);
    if (i4Ret != OSR_OK) 
        ASSERT(0);
    x_memcpy((VOID*)&_rFeederReqData,
             (VOID*)&rWmvFeederReqData.rFeederReqData,
             sizeof(FEEDER_REQ_DATA_T));
    LOG(5, "-----reqData.id: 0x%x\n", _rFeederReqData.u4Id);
    if (_rFeederReqData.u4ReadSize == 0)
    {
        LOG(0, "!!!!!!!The feeder ACK Size is 0(tb,trick,eof?)\n");
        return FALSE;
    }
    _u4DefalutReqSize = rWmvFeederReqData.u4RequestFeederSize;
    _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederReqData.u4WriteAddr 
                                       + _rFeederReqData.u4AlignOffset;
    _rSwdmxWmvInfo.u4FeederReadSize = _rFeederReqData.u4ReadSize;

    _rSwdmxWmvInfo.u1FWKeyFrameNo = 0;
    _rSwdmxWmvInfo.fgGetKeyHead = FALSE;    
    _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
    _rSwdmxWmvInfo.u2SendPayloadNo = 0;
    /* re-calculate packet count, since _rFeederReqData.u4ReadSize
       may be less than request size */
    u4DemuxPacketCount = _rSwdmxWmvInfo.u4FeederReadSize / _rSwdmxWmvInfo.u4PacketSize;
    if (u4DemuxPacketCount != TIMEBASED_TRICKMODE_WMV_REQ_SIZE / _rSwdmxWmvInfo.u4PacketSize)
    {
        fgPktCntChanged = TRUE;
        LOG(0, "demux packet count(%d) != %d, may EOF!\n",
            u4DemuxPacketCount,
            FEEDER_SEGMENT / _rSwdmxWmvInfo.u4PacketSize);
    }
    VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
    LOG(5, "---psa: 0x%x, rdsz: 0x%x msgs: %d, pktcnt %d\n",
        _rSwdmxWmvInfo.u4PacketStartAddr,
        _rSwdmxWmvInfo.u4FeederReadSize,
        u2Cnt,
        u4DemuxPacketCount);
    
    //2.4 parse data
    if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
    {
        fgTrickTrigger[prSwdmxInst->u1SwdmxId]=FALSE;
    }
    LOG(5, "---parse packet(tb,trick)(16bytes@0x%x):  "
           "%02x %02x %02x %02x %02x %02x %02x %02x "
           "%02x %02x %02x %02x %02x %02x %02x %02x\n", 
        _rSwdmxWmvInfo.u4PacketStartAddr,
        PKTBYTE(0), PKTBYTE(1), PKTBYTE(2), PKTBYTE(3), 
        PKTBYTE(4), PKTBYTE(5), PKTBYTE(6), PKTBYTE(7), 
        PKTBYTE(8), PKTBYTE(9), PKTBYTE(10), PKTBYTE(11), 
        PKTBYTE(12), PKTBYTE(13), PKTBYTE(14), PKTBYTE(15));
    for (i = 0; i < u4DemuxPacketCount; i++)
    {
        UINT32 u4AddrDelta = _rFeederInfo.u4EndAddr - _rSwdmxWmvInfo.u4PacketStartAddr;
        if (u4AddrDelta < _rSwdmxWmvInfo.u4PacketSize)
        {
            x_memcpy((VOID*)(_rFeederInfo.u4StartAddr-u4AddrDelta),
                     (VOID*)(_rSwdmxWmvInfo.u4PacketStartAddr),
                     u4AddrDelta);
            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederInfo.u4StartAddr-u4AddrDelta;
            HalFlushDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)u4AddrDelta);
        }
        LOG(5, "---trick parsing packet %d/%d @0x%x -"
               " %02x %02x %02x %02x "
               "%02x %02x %02x %02x "
               "%02x %02x %02x %02x "
               "%02x %02x %02x %02x "
               "-\n", 
            i+1, 
            u4DemuxPacketCount,
            _rSwdmxWmvInfo.u4PacketStartAddr,
            PKTBYTE(0), PKTBYTE(1), PKTBYTE(2), PKTBYTE(3), 
            PKTBYTE(4), PKTBYTE(5), PKTBYTE(6), PKTBYTE(7), 
            PKTBYTE(8), PKTBYTE(9), PKTBYTE(10), PKTBYTE(11), 
            PKTBYTE(12), PKTBYTE(13), PKTBYTE(14), PKTBYTE(15));
        fgRet = _WMVParsingPacket(prSwdmxInst, 
                                  _rSwdmxWmvInfo.u4PacketStartAddr,
                                  0,
                                  _rSwdmxWmvInfo.u4PacketSize);
        //if (fgRet)
            _rSwdmxWmvInfo.u4PacketStartAddr += _rSwdmxWmvInfo.u4PacketSize;
//      else
//      {
//          LOG(0, "---trick parsing fail!\n");
//          return FALSE;
//      }
        if (_rSwdmxWmvInfo.fgFinishDemuxIFrame)
        {
            _rSwdmxWmvInfo.fgFinishDemuxIFrame = FALSE;
            fgGotIFrame = TRUE;
            break;
        }
    }
    if (i == u4DemuxPacketCount
        && _rSwdmxWmvInfo.u1FWKeyFrameNo == 1)
    {
        LOG(0, "no key frame, _fgReceiveVideoPes is %s, set to FALSE\n", 
            _fgReceiveVideoPes?"TRUE":"FALSE");
       _fgReceiveVideoPes = FALSE;
    }
    //LOG(5, "---cur pkt pts 0x%x\n", _rSwdmxWmvInfo.u4CurPacketPTS);
    FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
    if (fgPktCntChanged)
    {                                       
        LOG(0, "Send Eos normally(tb, trick)\n");                                                
        _SWDMX_WMVSendEOS(prSwdmxInst);
        fgPktCntChanged = FALSE; 
        return TRUE;
    }  
    
    //3. check if got IFrame
//  //3.0 play video??
//  if (_rSwdmxWmvInfo.fgEnVideo
//      && _rSwdmxWmvInfo.fgVideoPause)
//  {
//      _WMVStartToPlay(prSwdmxInst);
//  }

    //3.1 if got IFrame, check pts validation
    if (fgGotIFrame)
    {
        LOG(5, "--- got IFrame, oh yeah!\n");
        fgGotIFrame = FALSE;

        /* check this IFrame pts is valid or not */
        if (_rSwdmxWmvInfo.i4WMVPlaySpeed > 0)
        {
            if (_rSwdmxWmvInfo.u4CurPacketPTS < _rSwdmxWmvInfo.u4DisplayPTS)
            {
                /* server may give us small-pts packet,
                   re-req/parse and send es to VFIFO */
                LOG(5, "--- u4CurPacketPTS(0x%x) < u4DisplayPTS(%x), re-req/parse\n");
                fgRereqData = TRUE;
                goto NEXT_I_FRAME_TIMEBASED;
            }
        }
        else
        {
            if (_rSwdmxWmvInfo.u4CurPacketPTS > _rSwdmxWmvInfo.u4DisplayPTS)
            {
                /* server may give us big-pts packet,
                   re-req/parse and send es to VFIFO */
                LOG(5, "--- u4CurPacketPTS(0x%x) > u4DisplayPTS(%x), re-req/parse\n");
                fgRereqData = TRUE;
                goto NEXT_I_FRAME_TIMEBASED;
            }
        }
        _rSwdmxWmvInfo.u4DisplayPTS = _rSwdmxWmvInfo.u4CurPacketPTS;
        LOG(5, "--- trick mode parsing end\n\n");
        return TRUE;
    }

    //3.2 if not get IFrame, use last packet pts to repeat step 2 
    fgRereqData = TRUE;
    if (_rSwdmxWmvInfo.fgBrokenFile)
    {
        LOG(5, "---got wrong data(broken file), reset it\n");
        _rSwdmxWmvInfo.fgBrokenFile = FALSE;
    }
    goto NEXT_I_FRAME_TIMEBASED; //(re-req/re-get/re-parse data)        
#endif
    //return TRUE;
}


static BOOL _WMVGetEntryInfoByIdx(SWDMX_INFO_T *prSwdmxInst, 
                                   UINT32 u4Index,
                                   UINT64* pu8EntryAbsOfst, /* output param */
                                   UINT32* pu4EntryPts, /* output param */
                                   UINT32* pu4EntrySize /* output param */)
{
    UINT32 u4SearchedEntryCount = 0;
    MM_RANGE_ELMT_IDXTBL_T* pt_tbl = NULL; 
    MM_RANGE_ELMT_IDXTBL_T* pt_partial_tbl = NULL;
    MIDXBULD_ELMT_STRM_ENTRY_T* prKeyidxEntry = NULL;

    pt_tbl = _rSwdmxWmvInfo.prWmvKeyIdxTbl;
    ASSERT(NULL != pt_tbl);
    ASSERT(NULL != pu8EntryAbsOfst);
    ASSERT(NULL != pu4EntryPts);
    ASSERT(NULL != pu4EntrySize);

    ASSERT(u4Index < pt_tbl->ui4_number_of_entry);  

    if (NULL == pt_tbl->pt_next_partial) /* filesize < 4G, only ONE key index table */
    {
        prKeyidxEntry = (MIDXBULD_ELMT_STRM_ENTRY_T*)(pt_tbl->u.pt_pb_idx_tbl_entry 
                                                      + u4Index);
        if (prKeyidxEntry == NULL)
        {
            ASSERT(0);
            return FALSE;
        }
        *pu8EntryAbsOfst = pt_tbl->ui8_base_offset + prKeyidxEntry->ui4_relative_offset;
        *pu4EntryPts     = prKeyidxEntry->ui4_pts;
        *pu4EntrySize    = prKeyidxEntry->ui4_size;
        LOG(8, "Got entry #%d, PTS: 0x%x, AbsOfst: 0x%llx, EntrySize: 0x%x\n",
            u4Index,
            *pu4EntryPts,
            *pu8EntryAbsOfst,
            *pu4EntrySize);
        return TRUE;
    }

    /* filesize > 4G, loop all partial tables.
       there're two partial tables at least. */
    pt_partial_tbl = pt_tbl->pt_next_partial;
    while (pt_partial_tbl != NULL)
    {
        if (u4Index < u4SearchedEntryCount + pt_partial_tbl->ui4_number_of_entry)
        {
            prKeyidxEntry = (MIDXBULD_ELMT_STRM_ENTRY_T*)(pt_tbl->u.pt_pb_idx_tbl_entry 
                                                          + u4Index);
            if (prKeyidxEntry == NULL)
            {
                ASSERT(0);
                return FALSE;
            }
            *pu8EntryAbsOfst = pt_partial_tbl->ui8_base_offset 
                               + prKeyidxEntry->ui4_relative_offset;
            *pu4EntryPts     = prKeyidxEntry->ui4_pts;
            *pu4EntrySize    = prKeyidxEntry->ui4_size;
            LOG(8, "Got partial entry #%d, PTS: 0x%x, AbsOfst: 0x%llx, EntrySize: 0x%x\n",
                u4Index,
                *pu4EntryPts,
                *pu8EntryAbsOfst,
                *pu4EntrySize);
            return TRUE;
        }
        u4SearchedEntryCount += pt_partial_tbl->ui4_number_of_entry;
        pt_partial_tbl = pt_partial_tbl->pt_next_partial;
    }
    return FALSE;
}

BOOL _SWDMX_WMVTrickModeParsing(SWDMX_INFO_T *prSwdmxInst)
{
    //parsing one key frame in trick mode
    UINT32 u4NumberOfEntry = 0;
    UINT64 u8address = 0;
    UINT32 u4RetryTimes =0;
    //MIDXBULD_ELMT_STRM_ENTRY_T* prKeyidxEntry = NULL;
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;
	BOOL fgReqedData=FALSE;
    UINT32 i=0;
//  UINT32 u4IdxTblCount = 0;
//  UINT32 u4FirstGoodIdx = 0;
//  UINT32 u4LastGoodIdx = 0;

    UINT64 u8EntryAbsOfst = 0; 
    UINT32 u4EntryPts = 0; 
    UINT32 u4EntrySize = 0; 
    UINT64 u8CurEntryAbsOfst = 0; 
    UINT32 u4CurEntryPts = 0; 
    UINT32 u4CurEntrySize = 0; 
    UINT64 u8Remainder = 0;
    
    LOG(6, "---===+++ trick mode parsing begin +++===---\n");
    if (_rSwdmxWmvInfo.prWmvKeyIdxTbl != NULL)
    {
        u4NumberOfEntry = _rSwdmxWmvInfo.prWmvKeyIdxTbl->ui4_number_of_entry;
    }
    else
    {
        LOG(0,"_SWDMX_WMVTrickModeParsing : _rSwdmxWmvInfo.prWmvKeyIdxTbl == NULL\n");
        ASSERT(_rSwdmxWmvInfo.prWmvKeyIdxTbl == NULL);
        return TRUE;
    }  

#ifdef WMA_TRICK_MODE
    if ((!_rSwdmxWmvInfo.fgDemuxFinish)
        && (_rSwdmxWmvInfo.fgFinishKeyTable) 
        && (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES) 
        && (_rSwdmxWmvInfo.i4WMVPlaySpeed < 0))
    {
        LOG(0,"DSP finished, Send EOS\n");
        LOG(2,"Send Eos 6\n");
        _SWDMX_WMVSendEOS(prSwdmxInst);
    }
#endif

    if (!_rSwdmxWmvInfo.fgFinishKeyTable)
    {
     //   UINT64 u8Baseoffset = _rSwdmxWmvInfo.prWmvKeyIdxTbl->ui8_base_offset;
        UINT32 u4DemuxSize = 0;
        UINT32 u4DemuxPacketCount = 0;
        UINT32 u4ParsingPacketNo = 0;
        UINT32 u4MoreParsingPacket = 0;
        
        GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                            _rSwdmxWmvInfo.u4CurKeyIdx,
                            u8CurEntryAbsOfst,
                            u4CurEntryPts, 
                            u4CurEntrySize);

		//skip bad index
        {
            if (u4CurEntrySize != 0 && u4CurEntrySize < _rSwdmxWmvInfo.u8FileSize)
            {     
                //OK case
            }
            else
            {   
                if (_rSwdmxWmvInfo.i4WMVPlaySpeed < 0 && _rSwdmxWmvInfo.u4CurKeyIdx > 0)
                {
                    UINT32 tmp = _rSwdmxWmvInfo.u4CurKeyIdx;
                    for (i = 0; i <= tmp; i++)
                    {
                        GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                            tmp - i,
                                            u8EntryAbsOfst,
                                            u4EntryPts, 
                                            u4EntrySize);
                        if (u4EntrySize != 0 && u4EntrySize < _rSwdmxWmvInfo.u8FileSize)
                            break;
                        if(_rSwdmxWmvInfo.u4CurKeyIdx > 0)
                        {
                            _rSwdmxWmvInfo.u4CurKeyIdx--;
                        }
                    }
                    if ((!_rSwdmxWmvInfo.fgFinishKeyTable) 
                        && (i == _rSwdmxWmvInfo.u4FirstGoodIdx))
                    {
                        //reach start, and it's bad, then send_eos
#ifdef WMA_TRICK_MODE
                        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
                        {
                            _rSwdmxWmvInfo.fgFinishKeyTable = TRUE;
                        }
                        else
#endif
                        {
                            LOG(0,"Send Eos 7.01\n");
                            _SWDMX_WMVSendEOS(prSwdmxInst);
                            _rSwdmxWmvInfo.fgFinishKeyTable = TRUE;
                            return TRUE;
                        }
                    }
                }
                else
                {
                    for (i = _rSwdmxWmvInfo.u4CurKeyIdx; i < u4NumberOfEntry; i++)
                    {
                        GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                            i,
                                            u8EntryAbsOfst,
                                            u4EntryPts, 
                                            u4EntrySize);
                        if (u4EntrySize != 0 && u4EntrySize < _rSwdmxWmvInfo.u8FileSize)
                            break;
                        _rSwdmxWmvInfo.u4CurKeyIdx++;
                    }
                    if ((!_rSwdmxWmvInfo.fgFinishKeyTable) 
                        && (i == _rSwdmxWmvInfo.u4LastGoodIdx))
                    {
                        //reach start, and it's bad, then send_eos
#ifdef WMA_TRICK_MODE
                        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
                        {
                            _rSwdmxWmvInfo.fgFinishKeyTable = TRUE;
                        }
                        else
#endif
                        {
                            LOG(0,"Send Eos 7.01\n");
                            _SWDMX_WMVSendEOS(prSwdmxInst);
                            _rSwdmxWmvInfo.fgFinishKeyTable = TRUE;
                            return TRUE;
                        }
                    }                    
                }                
            }        
        } // end of skip bad index


        /* after skip bad entry, re-get entry ifo */
        GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                            _rSwdmxWmvInfo.u4CurKeyIdx,
                            u8CurEntryAbsOfst,
                            u4CurEntryPts, 
                            u4CurEntrySize);
        {
            /* DTV00579084
             * key index table now is designed with absolute offset of file, 
             * so it's no need to calculate address again. use it directly. 
             *  
             * If use re-calculated address, we'll get very few I frames, it
             * leading to EOS qiuckly.
             */
            #if 0    
            /* calculate request address */         
            UINT32 u4ParsingPktNo = (UINT32)u8Div6432(u8CurEntryAbsOfst,
                                                      _rSwdmxWmvInfo.u4PacketSize, 
                                                      &u8Remainder);

            u8address = _prRangeInfo->ui8_fileoffset 
                        + WMV_DATA_OBJ_HEADER 
                        + ((UINT64)u4ParsingPktNo)*_rSwdmxWmvInfo.u4PacketSize;
            #else
            u8address = u8CurEntryAbsOfst;
            #endif
            LOG(5, "req from file address 0x%llx, idx:#%d (AbsOfst: 0x%llx, entrySz:0x%x)\n", 
                u8address, 
                _rSwdmxWmvInfo.u4CurKeyIdx,
                u8CurEntryAbsOfst,
                u4CurEntrySize);
        }

#ifdef WMA_TRICK_MODE
        if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {   
            u4DemuxSize = u4CurEntrySize;

            if (0 == _rSwdmxWmvInfo.u4AudioObjSz)
            {
                LOG(0, "audio object size ZERO! (not play yet)\n");
                return FALSE;
            }
			if (_rSwdmxWmvInfo.u2PreviousSendPayloadNo==0)
			{
                _rSwdmxWmvInfo.u1WmaDmxPktCount = u4CurEntrySize / _rSwdmxWmvInfo.u4PacketSize;
                u4DemuxPacketCount = _rSwdmxWmvInfo.u1WmaDmxPktCount;
                LOG(5, "DemuxPktCnt:%d u2PrevSendPayloadNo:%d\n", 
                    u4DemuxPacketCount,
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
				if(!_SWDMX_WMVGetFeederData(prSwdmxInst,TRUE))
				{
				   if(_rSwdmxWmvInfo.fgFeederInvalid)
				   {
                       return FALSE;
				   }
				   else
				   {
					  _SWDMX_WMVRequestFeederData(prSwdmxInst, u4DemuxSize,u8address,TRUE);
                      LOG(5, "- wma req from #%d 0x%llx/0x%x\n", 
                          _rSwdmxWmvInfo.u4CurKeyIdx, u8address, u4DemuxSize);
					  if(_SWDMX_WMVGetFeederData(prSwdmxInst, FALSE))
                      {
                        fgReqedData=TRUE;
                      }
					  if(_rSwdmxWmvInfo.fgFeederInvalid ||_rSwdmxWmvInfo.fgDemuxFinish)
					  {
						 return FALSE;
					  }
				   }
				}
                else
                {
			        fgReqedData=TRUE;
                }
			}
			else
			{
                UINT8 u1ParsedPktCnt = 0;
                if (0 == _rSwdmxWmvInfo.u2WmaPlCntPerPkt)
                {
                    return FALSE;
                }
                u1ParsedPktCnt = (_rSwdmxWmvInfo.u2PreviousSendPayloadNo - 1) 
                                            / _rSwdmxWmvInfo.u2WmaPlCntPerPkt;
                //_rSwdmxWmvInfo.u1WmaDmxPktParsed = u1ParsedPktCnt;
                u4DemuxPacketCount = _rSwdmxWmvInfo.u1WmaDmxPktCount - u1ParsedPktCnt;
                LOG(5, "DemuxPktCnt:%d u2PrevSendPayloadNo:%d\n", 
                    u4DemuxPacketCount,
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
                _rSwdmxWmvInfo.u2SendPayloadNo = _rSwdmxWmvInfo.u2PreviousSendPayloadNo-1;
                LOG(5, "---u2SendPayloadNo %d(p:%d)\n", 
                    _rSwdmxWmvInfo.u2SendPayloadNo,
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo);
			}
            LOG(5, "DemuxPktCnt:%d\n", u4DemuxPacketCount);
        }
        else
#endif
        {
            UINT64 u8Remainder = 0;

            u4ParsingPacketNo = (UINT32)u8Div6432(u8CurEntryAbsOfst
                                                    - _prRangeInfo->ui8_fileoffset 
                                                    - WMV_DATA_OBJ_HEADER,
                                                  _rSwdmxWmvInfo.u4PacketSize, 
                                                  &u8Remainder);
            UNUSED(u8Remainder);

            u4MoreParsingPacket = (UINT32)min(MORE_DEMUX_PACKET_COUNT,
                                              _rSwdmxWmvInfo.u4TotalPacketNo
                                                  - u4ParsingPacketNo
                                                  - (u4CurEntrySize / _rSwdmxWmvInfo.u4PacketSize));
            u4DemuxSize = u4CurEntrySize + (u4MoreParsingPacket*_rSwdmxWmvInfo.u4PacketSize);
            u4DemuxPacketCount = u4DemuxSize / _rSwdmxWmvInfo.u4PacketSize;
            if (u4CurEntrySize > _rSwdmxWmvInfo.u8FileSize
                || u4CurEntrySize == 0)
            {
				//should not go here!
                LOG(0, "index in table: %d\n", _rSwdmxWmvInfo.u4CurKeyIdx);
                LOG(0, "index entry: ui4_relative_offset+ui8_base_offset 0x%x\n", u8CurEntryAbsOfst);
                LOG(0, "             ui4_size 0x%x\n", u4CurEntrySize);
                LOG(0, "_rSwdmxWmvInfo: u4PacketSize 0x%x\n", _rSwdmxWmvInfo.u4PacketSize);
                LOG(0, "                u4TotalPacketNo 0x%x\n", _rSwdmxWmvInfo.u4TotalPacketNo);
                LOG(0, "-------------------\n");
                LOG(0, "u4ParsingPacketNo 0x%x\n", u4ParsingPacketNo);    
                LOG(0, "u4MoreParsingPacket 0x%x (howto: min(11, u4TotalPacketNo-u4ParsingPacketNo-(i4_size/pkt.sz)))\n", u4MoreParsingPacket);                    
                LOG(0, "u4DemuxSize 0x%x (to be req)\n", u4DemuxSize);                            
                LOG(0, "u4DemuxPacketCount 0x%x (howto: ui4_size/pkt.sz + u4MoreParsingPacket)\n", u4DemuxPacketCount);     
                LOG(0, "-------------------\n");      
                LOG(0, "ui4_size(0x%x)/pkt.sz(0x%x) 0x%x\n",
                    u4CurEntrySize,
                    _rSwdmxWmvInfo.u4PacketSize,
                    u4CurEntrySize/_rSwdmxWmvInfo.u4PacketSize);
            }            

            if (u4DemuxSize != 0)
            {
                LOG(6, "[trick]req data from 0x%llx size 0x%x\n", u8address, u4DemuxSize);
               _SWDMX_WMVRequestFeederData(prSwdmxInst, u4DemuxSize,u8address,TRUE);
               _SWDMX_WMVGetFeederData(prSwdmxInst, FALSE);
               if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
               {
                   return FALSE;
               } 
            }
			fgReqedData=TRUE;
        }

		if(fgReqedData)
		{
            x_memset(&rWmvFeederReqData,0,sizeof(rWmvFeederReqData));

            zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);
            i4Ret = x_msg_q_receive(&u2MsgQIdx, &rWmvFeederReqData, &zMsgSize,
                                    &(_hWMVFeederDataQue), 1, X_MSGQ_OPTION_WAIT);

            if (i4Ret != OSR_OK)
            {
                ASSERT(0);
            }

            x_memcpy((VOID*)&_rFeederReqData,(VOID*)&rWmvFeederReqData.rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
            _u4DefalutReqSize = rWmvFeederReqData.u4RequestFeederSize;

            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederReqData.u4WriteAddr + _rFeederReqData.u4AlignOffset;
            _rSwdmxWmvInfo.u4FeederReadSize = _rFeederReqData.u4ReadSize;

            _rSwdmxWmvInfo.u1FWKeyFrameNo =0;
			_rSwdmxWmvInfo.fgGetKeyHead = FALSE;
			
			_rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
			_rSwdmxWmvInfo.u2SendPayloadNo = 0;
		}
		
        _rSwdmxWmvInfo.fgBlock = FALSE;
        /* DTV00583149
         * if u4CurParsingPacketNo was calculated greater than actual value, 
         * the first frame might NOT be I-Frame, VDEC will drop them, we'll 
         * see video is stuck. 
         */
        _rSwdmxWmvInfo.u4CurParsingPacketNo = 
            (UINT32)u8Div6432(u8CurEntryAbsOfst 
                                - _prRangeInfo->ui8_fileoffset 
                                - WMV_DATA_OBJ_HEADER,
                              _rSwdmxWmvInfo.u4PacketSize, 
                              &u8Remainder);
        UNUSED(u8Remainder);
        if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
        {
            fgTrickTrigger[prSwdmxInst->u1SwdmxId]=FALSE;
        }
        for (i = 0; i < u4DemuxPacketCount; i++)
        {
            UINT32 u4AddrDelta = _rFeederInfo.u4EndAddr - _rSwdmxWmvInfo.u4PacketStartAddr;
            if (u4AddrDelta < _rSwdmxWmvInfo.u4PacketSize)
            {
                x_memcpy((VOID*)(_rFeederInfo.u4StartAddr-u4AddrDelta),(VOID*)(_rSwdmxWmvInfo.u4PacketStartAddr),u4AddrDelta);
                _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederInfo.u4StartAddr-u4AddrDelta;
                HalFlushDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)u4AddrDelta);
            }
            if (!(PKTBYTE(0) == 0x82
                  && PKTBYTE(1) == 0x00
                  && PKTBYTE(2) == 0x00))
            {
                LOG(1,"[trick]packet does not start with 0x820000, skip it\n");
            }
            else
            {
                _WMVParsingPacket(prSwdmxInst, 
                                  _rSwdmxWmvInfo.u4PacketStartAddr,
                                  0,
                                  _rSwdmxWmvInfo.u4PacketSize);
                LOG(7, "parsing packet #%d::0x%x\n", i, _rSwdmxWmvInfo.u4PacketStartAddr);
            }
			if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES && _rSwdmxWmvInfo.fgBlock)
			{
                //LOG(0, "wma && blocked, return!\n");
                return TRUE;
			}
			
            _rSwdmxWmvInfo.u4PacketStartAddr += _rSwdmxWmvInfo.u4PacketSize;
            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);

            if (_rSwdmxWmvInfo.fgFinishDemuxIFrame)
            {
                _rSwdmxWmvInfo.fgFinishDemuxIFrame = FALSE;
                break;
            }
        }
        _rSwdmxWmvInfo.u4TotalPktCountPerEntry += u4DemuxPacketCount;//i;
        LOG(7, "DmxPktCount(%d,%d) = (Entry->size(%d)/PktSz(%d)) + MoreParsingPkt(%d)\n",
            u4DemuxPacketCount, i, u4CurEntrySize, _rSwdmxWmvInfo.u4PacketSize,
            u4MoreParsingPacket);
        _rSwdmxWmvInfo.u4TrickModeParsingActionCount++;
        if (_rSwdmxWmvInfo.u4TrickModeParsingActionCount > 0)
        {
            _rSwdmxWmvInfo.u4AvgPktCountPerEntry = _SWDMX_WMV_u4Round(
                                            _rSwdmxWmvInfo.u4TotalPktCountPerEntry, 
                                            _rSwdmxWmvInfo.u4TrickModeParsingActionCount);
            LOG(7, "AvgPktCountPerEntry(%d) = round(%d / %d)\n",
                _rSwdmxWmvInfo.u4AvgPktCountPerEntry,
                _rSwdmxWmvInfo.u4TotalPktCountPerEntry,
                _rSwdmxWmvInfo.u4TrickModeParsingActionCount);
        }

#ifdef CC_3D_MM_DS_SUPPORT  // megaa 20110113: tmp solution for 3D multi-track trick mode
		if(i==u4DemuxPacketCount&&_rSwdmxWmvInfo.u1FWKeyFrameNo==1)
#else
		if(i==u4DemuxPacketCount&&_rSwdmxWmvInfo.u1FWKeyFrameNo==1)
#endif
		{
           _fgReceiveVideoPes=FALSE;
		}

#ifdef WMV_FAST_I_FRAME_PARSING
        _u8PreKeyFrameAddr = u8address;
#endif
    }

#ifdef WMV_FAST_I_FRAME_PARSING
NEXT_I_FRAME:
#endif

    if (    /* backward */
            (_rSwdmxWmvInfo.i4WMVPlaySpeed < 0 
             && (_rSwdmxWmvInfo.u4CurKeyIdx == 0 || _rSwdmxWmvInfo.u4CurParsingPacketNo==0 ))
        || 
            /* forward with speed > 2X */
            (_rSwdmxWmvInfo.i4WMVPlaySpeed >=4000 
             && (_rSwdmxWmvInfo.u4CurKeyIdx >= u4NumberOfEntry - 1))
       )
    {
        if (!_rSwdmxWmvInfo.fgFinishKeyTable)
        {
#ifdef WMA_TRICK_MODE
            if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
                _rSwdmxWmvInfo.fgFinishKeyTable = TRUE;
            }
            else
#endif
            {
                LOG(0, "Send Eos 7(curIdx %d, entries %d)\n", 
                    _rSwdmxWmvInfo.u4CurKeyIdx,
                    u4NumberOfEntry);
                _SWDMX_WMVSendEOS(prSwdmxInst);
                _rSwdmxWmvInfo.fgFinishKeyTable = TRUE;
                return TRUE;
            }
        }
    }
    else
    {
        /* calculate next key index via speed */
        if (_rSwdmxWmvInfo.i4WMVPlaySpeed < 0)
        {
#ifdef WMA_TRICK_MODE
            if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
                UINT32 u4JumpSec,u4SeekTime,u4PrePts;

                u4JumpSec = abs(_rSwdmxWmvInfo.i4WMVPlaySpeed)/1000+1;
                u4SeekTime = u4CurEntryPts;
                if(_rSwdmxWmvInfo.u4CurKeyIdx>0)
                {
                    _rSwdmxWmvInfo.u4CurKeyIdx--;
                }
                else
                {
                    _rSwdmxWmvInfo.fgFinishKeyTable=TRUE;
                    return TRUE;
                }
                GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                    _rSwdmxWmvInfo.u4CurKeyIdx,
                                    u8CurEntryAbsOfst,
                                    u4CurEntryPts, 
                                    u4CurEntrySize);
               
                u4PrePts = u4CurEntryPts;
                for (; u4JumpSec > 0; u4JumpSec--)
                {
                    while (u4SeekTime == u4PrePts 
                           && _rSwdmxWmvInfo.u4CurKeyIdx > 0)
                    {   
                        if (_rSwdmxWmvInfo.u4CurKeyIdx>0)
                        {
                            _rSwdmxWmvInfo.u4CurKeyIdx--;
                        }

                        GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                            _rSwdmxWmvInfo.u4CurKeyIdx,
                                            u8CurEntryAbsOfst,
                                            u4CurEntryPts, 
                                            u4CurEntrySize);
                        u4PrePts = u4CurEntryPts;
                    }
                    u4SeekTime = u4PrePts;
                }
                GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                    _rSwdmxWmvInfo.u4CurKeyIdx,
                                    u8CurEntryAbsOfst,
                                    u4CurEntryPts, 
                                    u4CurEntrySize);
                u4SeekTime = u4CurEntryPts;
                _rSwdmxWmvInfo.u4IndexPts = 0;
				_rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;
                _SWDMX_WMVSeek(prSwdmxInst, u4SeekTime, NULL);
            }
            else if (_rSwdmxWmvInfo.u4CurKeyIdx > 0)
#endif
            {
				if (_fgReceiveVideoPes)
				{
				    UINT32 u4DeltaTime=0;
				    UINT32 u4PrePts=0;
				    UINT32 ui4Size=0;
                    GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                        _rSwdmxWmvInfo.u4CurKeyIdx,
                                        u8CurEntryAbsOfst,
                                        u4CurEntryPts, 
                                        u4CurEntrySize);
                    u4PrePts = u4CurEntryPts;
                    do
                    {
                        if (_rSwdmxWmvInfo.u4CurKeyIdx == 0)
                        {
                            break;
                        }
                        _rSwdmxWmvInfo.u4CurKeyIdx--;

                        GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                            _rSwdmxWmvInfo.u4CurKeyIdx,
                                            u8CurEntryAbsOfst,
                                            u4CurEntryPts, 
                                            u4CurEntrySize);
                        ui4Size = u4CurEntrySize;
                        if (ui4Size != 0 && ui4Size < _rSwdmxWmvInfo.u8FileSize)
                        {
                            u4DeltaTime = u4PrePts - u4CurEntryPts;
                        }
                        else
                            LOG(0, "meet bad entry(#%d,0x%x), skip!!\n", 
                                _rSwdmxWmvInfo.u4CurKeyIdx,
                                ui4Size); 
				   } while (u4DeltaTime < 90 * abs(_rSwdmxWmvInfo.i4WMVPlaySpeed));
				}
				else
				{
					_rSwdmxWmvInfo.u4CurKeyIdx--;
				}
            }
        }
        else if (_rSwdmxWmvInfo.i4WMVPlaySpeed > 0)
        {
			if (_fgReceiveVideoPes)
			{
				UINT32 u4DeltaTime=0;
				UINT32 u4PrePts=0;
				UINT32 ui4Size=0;

                GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                    _rSwdmxWmvInfo.u4CurKeyIdx,
                                    u8CurEntryAbsOfst,
                                    u4CurEntryPts, 
                                    u4CurEntrySize);
                u4PrePts = u4CurEntryPts;
				do
				{
                    if (_rSwdmxWmvInfo.u4CurKeyIdx 
                        >= _rSwdmxWmvInfo.prWmvKeyIdxTbl->ui4_number_of_entry - 1)
                    {
                        break;
                    }
                    _rSwdmxWmvInfo.u4CurKeyIdx++;
                    GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                                        _rSwdmxWmvInfo.u4CurKeyIdx,
                                        u8CurEntryAbsOfst,
                                        u4CurEntryPts, 
                                        u4CurEntrySize);
                    ui4Size = u4CurEntrySize;
                    if (ui4Size != 0 && ui4Size < _rSwdmxWmvInfo.u8FileSize)
                    {
                        u4DeltaTime = u4CurEntryPts - u4PrePts;
                    }
                    else
                        LOG(0, "meet bad entry(#%d,0x%x), skip!!\n", 
                            _rSwdmxWmvInfo.u4CurKeyIdx,
                            ui4Size); 			  
				} while (u4DeltaTime < 90 * abs(_rSwdmxWmvInfo.i4WMVPlaySpeed));
                LOG(7, "speed>0, got index #%d\n", _rSwdmxWmvInfo.u4CurKeyIdx);
			}
			else
			{
				_rSwdmxWmvInfo.u4CurKeyIdx++;
                LOG(7, "[trick]speed>0, index++ #%d\n", _rSwdmxWmvInfo.u4CurKeyIdx);
			}
        }
    }
#ifdef WMA_TRICK_MODE
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
        return TRUE;
    }
#endif
#ifdef WMV_FAST_I_FRAME_PARSING
    GET_ENTRY_INFO_BOOL(prSwdmxInst, 
                        _rSwdmxWmvInfo.u4CurKeyIdx,
                        u8CurEntryAbsOfst,
                        u4CurEntryPts, 
                        u4CurEntrySize);    
    #if 0
    {
        UINT32 u4ParsingPktNo2 = (UINT32)u8Div6432(u8CurEntryAbsOfst
                                                    - _prRangeInfo->ui8_fileoffset 
                                                    - WMV_DATA_OBJ_HEADER,
                                                   _rSwdmxWmvInfo.u4PacketSize, 
                                                   &u8Remainder);
        u8address = _prRangeInfo->ui8_fileoffset 
                    + WMV_DATA_OBJ_HEADER 
                    + ((UINT64)u4ParsingPktNo2)*_rSwdmxWmvInfo.u4PacketSize;
    }
    #else
    u8address = u8CurEntryAbsOfst;
    #endif
    if (u8address == _u8PreKeyFrameAddr)
    {
        u4RetryTimes++;
        LOG(7, "[trick]address same(0x%llx), retry#%d\n", 
            u8address,
            u4RetryTimes);
        if (u4RetryTimes < 100)
            goto NEXT_I_FRAME;
    }
    else
    {
        LOG(0, "[trick]new address 0x%llx, idx:#%d (AbsOfst: 0x%llx) retry#%d\n", 
            u8address, 
            _rSwdmxWmvInfo.u4CurKeyIdx,
            u8CurEntryAbsOfst,
            u4RetryTimes);
    }
#endif
    LOG(6, "---===+++ trick mode parsing exit +++===---\n");
    return TRUE;
}

static UINT32 _g_u4PacketCountTb = 0;
BOOL _SWDMX_WMVDeliverDataTimebased(UINT8 u1SrcId, SWDMX_RANGE_LIST_T* prRangeList)
{
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    BOOL fgRet = FALSE;
    WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;
    SWDMX_INFO_T* prSwdmxInst = NULL;
    UINT16 u2Cnt = 0;
    //BOOL fgGotEofFromWMVMsgQ = FALSE;
    //BOOL fgIsAsfObjHdr = FALSE;
    BOOL fgTrickMode = FALSE;

    prSwdmxInst = _SWDMX_GetInst(u1SrcId); 
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);
    fgTrickMode = prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_NORMAL ? FALSE : TRUE;
    
#ifdef WMA_TRICK_MODE_TIMEBASED
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
      if(_rSwdmxWmvInfo.i4WMVPlaySpeed>=2000)
      {
        fgTrickMode=FALSE;
      }
    }
#endif

    if (_rSwdmxWmvInfo.fgDemuxFinish)
    {
        LOG(6, "demux_finish?%s, eof?%s, return\n",
            _rSwdmxWmvInfo.fgDemuxFinish?"yes":"no",
            _rFeederReqData.fgEof?"yes":"no");
#ifdef WMA_TRICK_MODE_TIMEBASED
        if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES && _rSwdmxWmvInfo.i4WMVPlaySpeed<0)
		{
		   if (prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
		   {
			 if ((!fgTrickTrigger[prSwdmxInst->u1SwdmxId])&&(AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId)))
			 {
			   AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, 
                                    &u4AudDspPts[prSwdmxInst->u1SwdmxId], 
                                    FALSE);
			 }
		   }
		}
#endif
        if(!_rSwdmxWmvInfo.fgDeliverVEos)
        {
            _SWDMX_WMVSendVEOS(prSwdmxInst);
        }
        if(!_rSwdmxWmvInfo.fgDeliverAEos)
        {
            _SWDMX_WMVSendAEOS(prSwdmxInst);
        }
		x_thread_delay(20);
        return FALSE;
    }
    if (_rSwdmxWmvInfo.fgFeederInvalid)
    {
        _rSwdmxWmvInfo.fgFeederInvalid = FALSE;
    }

    VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
    LOG(6, "First frame?%s, EofParsed?%s, reqData?%s, trick?%s, lastpts: 0x%x/0x%llx, msgs?%d \n",
        _rSwdmxWmvInfo.fgFirstFrame ? "yes" : "no",
        _rSwdmxWmvInfo.fgEofParsed ? "yes" : "no",
        _rSwdmxWmvInfo.fgNeedSetFdrReq ? "yes" : "no",
        fgTrickMode ? "yes" : "no",
        _rSwdmxWmvInfo.u4CurPacketPTS,
        _rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration,
        u2Cnt
        );
    if ( (!_rSwdmxWmvInfo.fgBlock) 
         && _rSwdmxWmvInfo.fgFirstFrame
         && (!fgTrickMode) )
    {        
        LOG(5, "1. First req feeder data? %s\n",
            _rSwdmxWmvInfo.fgFirstReqFeederData ? "yes" : "no");
        if (_rSwdmxWmvInfo.fgFirstReqFeederData)
        {
            _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 0, 0, FALSE);
            _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
        }

        fgRet = _SWDMX_WMVGetFeederDataTimebased(prSwdmxInst, FALSE);
        if (!fgRet)
        {
            if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
            {
                VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
                LOG(0, "feeder_invalid or demux_finished!(tb,first)\n");//, again(reset feeder)\n");
//              LOG(0, "feeder_invalid or demux_finished!(tb,first), "
//                     "return(reset feeder, %d msgs)\n",
//                  u2Cnt);
                return FALSE;
            }
        }
        else
        {
            _rSwdmxWmvInfo.fgFirstFrame = FALSE;
            //_rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
            _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 0,0,FALSE);
        }

        if(_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);
            i4Ret = x_msg_q_receive(&u2MsgQIdx, &rWmvFeederReqData, &zMsgSize,
                                &(_hWMVFeederDataQue), 1, X_MSGQ_OPTION_WAIT);

            if (i4Ret != OSR_OK)
            {
                ASSERT(0);
            }

            x_memcpy((VOID*)&_rFeederReqData,(VOID*)&rWmvFeederReqData.rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
            _u4DefalutReqSize = rWmvFeederReqData.u4RequestFeederSize;

            if (_rFeederReqData.fgEof) 
                LOG(0, "\n\n\n!!![tb] got EOF from msgQ!!!(firstFrame)\n\n\n\n");

            LOG(0, "[tb] feeder align offset 0x%x\n", _rFeederReqData.u4AlignOffset);
            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederReqData.u4WriteAddr + _rFeederReqData.u4AlignOffset; 

            _rSwdmxWmvInfo.u4FeederReadSize = _rFeederReqData.u4ReadSize-_rFeederReqData.u4AlignOffset;

            if (TRUE == _WMVIsObjectHeader((UINT8 *)_rSwdmxWmvInfo.u4PacketStartAddr))
            {
                UINT32 u4Offset = ((UINT32)prSwdmxInst->u8FileOffset
                                   + (UINT32)WMV_DATA_OBJ_HEADER);
                _rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr = TRUE;

                LOG(6, "[tb] old packet start address(normal first): 0x%x\n", _rSwdmxWmvInfo.u4PacketStartAddr);
                _rSwdmxWmvInfo.u4PacketStartAddr += u4Offset;
                _rSwdmxWmvInfo.u4ConsumedSize += u4Offset;
                //_rSwdmxWmvInfo.u4FeederReadSize -= u4Offset;
                LOG(6, "[tb] new packet start address(normal first): 0x%x\n", _rSwdmxWmvInfo.u4PacketStartAddr);
                
            }
            //get feeder buffer information
            if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) != FEEDER_E_OK)
            {
                LOG(3, "_SWDMX_WMVDeliverDataTimebased FeederGetBufferInfo() error.\n");
                return FALSE;
            }

            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
        }
        _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
        
        //LOG(5,"total data packet : %d \n",_rSwdmxWmvInfo.u4TotalPacketNo);
        LOG(5,"packet size(tb): %d(0x%x)\n",
            _rSwdmxWmvInfo.u4PacketSize,
            _rSwdmxWmvInfo.u4PacketSize);
        LOG(5,"feederReadSize(firstFrame)(tb): %d(0x%x)\n",
            _rSwdmxWmvInfo.u4FeederReadSize,
            _rSwdmxWmvInfo.u4FeederReadSize);

        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            if (_rSwdmxWmvInfo.u8BeginParseOfst == 0)
            {
                //_rSwdmxWmvInfo.u4PacketStartAddr += WMV_DATA_OBJ_HEADER;//time based
                //_rSwdmxWmvInfo.u4ConsumedSize = WMV_DATA_OBJ_HEADER;
            }
        }

        _rSwdmxWmvInfo.fgFirstFrame = FALSE;

        if (_rSwdmxWmvInfo.fgEnVideo)
        {
            VDEC_StartPTS(prSwdmxInst->u1VdecId,TRUE,0,NULL); //maiyou
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                VDEC_StartPTS(prSwdmxInst->u1Vdec2Id,TRUE,0,NULL); 
            }
        }

        if (!_SWDMX_WMVFeederIsReady(prSwdmxInst))
        {
            LOG(0, "Feeder is not ready, return! line:%d\n", __LINE__);
            return TRUE;
        }
    }

    else if ((!_rSwdmxWmvInfo.fgBlock) 
             && (!_rSwdmxWmvInfo.fgEofParsed)
             && (!fgTrickMode) )
    {
        if (!_SWDMX_WMVIsFeederBufferFull(prSwdmxInst))
        {
            //LOG(0, "_feeder buffer not full(tb)!\n");
            LOG(7, "2. First req feeder data? %s\n",
                _rSwdmxWmvInfo.fgFirstReqFeederData ? "yes" : "no");
            if ( _rSwdmxWmvInfo.fgFirstReqFeederData )
            {
                _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 0,0,FALSE);
                _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
            }

            if (_rSwdmxWmvInfo.fgNeedSetFdrReq)
            {
                fgRet = _SWDMX_WMVGetFeederDataTimebased(prSwdmxInst, FALSE);//maybe got EOF here!
                if (!fgRet)
                {
                    if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
                    {
                        LOG(0, "feeder_invalid! return! maybe first ACK data(seek etc)\n");
                        return FALSE;
                    }
                    if (_u4RequestDataSize == 0)
                    {
                        //need to request data again
                        LOG(0, "\n\n\nre-req data for last _u4RequestDataSize==0!\n\n\n\n");
                        _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst,0,0,FALSE);
                        _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                    }
                    else if (_rSwdmxWmvInfo.u4ConsumedSize ==0 && ( _rSwdmxWmvInfo.fgDemuxFinish))
                    {
                        LOG(0, "Demux_finished && consumed_size==0\n");
                        return FALSE;
                    }                 
                }
                else
                {
                    if (_rSwdmxWmvInfo.fgNeedSetFdrReq)
                    {
                        LOG(6, "ACK[%d] OK! re-req data for next parsing!\n", _rFeederReqData.u4Id);
                        _SWDMX_WMVRequestFeederDataTimebased(prSwdmxInst, 0,0,FALSE);
                        _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                    }
                    else
                    {
                        LOG(0, "[tb] already got eof from feeder, no need request data!\n");
                    }
                }
            }
            else
            {
                LOG(0, "no need to request feeder data!\n");
            }
            if (!_SWDMX_WMVFeederIsReady(prSwdmxInst))
            {
                x_thread_delay(50);
                LOG(0, "!_SWDMX_WMVFeederIsReady(prSwdmxInst), sleep 50ms\n");
                return TRUE;
            }

        }
        else
        {
            LOG(9, "_feeder buffer full(tb)!\n");
        }

        if (_rSwdmxWmvInfo.u4ConsumedSize == 0
            && _rSwdmxWmvInfo.fgGotEofFromWMVMsgQ == FALSE)
        {
            WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;
            
            zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);
            

            // tmp solution for DTV00085618
            VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
            if (u2Cnt == 0)  // which will cause deadlock at the x_msg_q_receive below
            {
                #if 0
                LOG(0, "no msg in queue, send Eos 9(tb)\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
                #else
                LOG(0, "no msg in queue, wait for a while(10ms), return\n");
                _SWDMX_WMVGetFeederDataTimebased(prSwdmxInst, FALSE);
                x_thread_delay(10);
                #endif
                return TRUE;
            }
            else
            {
                LOG(7, "%d msg(s) in queue\n", u2Cnt);
            }
            
            i4Ret = x_msg_q_receive(&u2MsgQIdx, &rWmvFeederReqData, &zMsgSize,
                                    &(_hWMVFeederDataQue), 1, X_MSGQ_OPTION_WAIT);
            if (i4Ret != OSR_OK)
            {
                ASSERT(0);
            }

            x_memcpy((VOID*)&_rFeederReqData,(VOID*)&rWmvFeederReqData.rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
            _u4DefalutReqSize = rWmvFeederReqData.u4RequestFeederSize;

            LOG(6, "Recv [#%d] ok, Sz:0x%x, pkt:#%05d, %d msgs left\n", 
                _rFeederReqData.u4Id,
                _rFeederReqData.u4ReadSize,
                _g_u4PacketCountTb,
                u2Cnt-1);
            if (_rFeederReqData.fgEof) 
            {
                LOG(0, "\n\n\n!!!got EOF from msgQ!!!(normal 0x%x)\n\n\n\n", _rFeederReqData.u4WriteAddr);
                _rSwdmxWmvInfo.fgGotEofFromWMVMsgQ = TRUE;
                if (0 == _rFeederReqData.u4WriteAddr
                    || 0 == _rFeederReqData.u4ReadSize)
                {
                    LOG(0, "eof && (addr or size ==0), don't parse data\n");
                    _rSwdmxWmvInfo.fgEofParsed = TRUE;
                    return TRUE;
                }
            }
            if (_rFeederReqData.u4ReadSize == 0)
            {
                LOG(0, "!!!!!!!The feeder ACK Size is 0(tb/EOF?)\n");
                return FALSE;
            }

            _rSwdmxWmvInfo.u4FeederReadSize = _rFeederReqData.u4ReadSize-_rFeederReqData.u4AlignOffset;
            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederReqData.u4WriteAddr + _rFeederReqData.u4AlignOffset;
            
            /* normal play or seek(!0):
             *      this flag always valid
             */
            if (_rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr)
            {
                UINT32 u4Offset = ((UINT32)prSwdmxInst->u8FileOffset
                                   + (UINT32)WMV_DATA_OBJ_HEADER);
                LOG(6, "old packet start address(normal body2): 0x%x\n", 
                    _rSwdmxWmvInfo.u4PacketStartAddr);
                _rSwdmxWmvInfo.u4PacketStartAddr += u4Offset;
                _rSwdmxWmvInfo.u4ConsumedSize += u4Offset;
                LOG(6, "new packet start address(normal body2): 0x%x\n", 
                    _rSwdmxWmvInfo.u4PacketStartAddr);
            }

            /* if seek(0), re-check first data */
            if (0 == _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo)
            {
                /* check data is obj header or not again */
                if (TRUE == _WMVIsObjectHeader((UINT8 *)_rSwdmxWmvInfo.u4PacketStartAddr))
                {
                    UINT32 u4Offset = ((UINT32)prSwdmxInst->u8FileOffset
                                       + (UINT32)WMV_DATA_OBJ_HEADER);
                    _rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr = TRUE;

                    LOG(6, "old packet start address(normal body1): 0x%x\n", 
                        _rSwdmxWmvInfo.u4PacketStartAddr);
                    _rSwdmxWmvInfo.u4PacketStartAddr += u4Offset;
                    _rSwdmxWmvInfo.u4ConsumedSize += u4Offset;
                    LOG(6, "new packet start address(normal body1): 0x%x\n", 
                        _rSwdmxWmvInfo.u4PacketStartAddr);
                }
            }
            LOG(6,"feederReadSize(normal)(tb): %d(0x%x)\n",
                _rSwdmxWmvInfo.u4FeederReadSize,
                _rSwdmxWmvInfo.u4FeederReadSize);
            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
        }
        else
        {
            LOG(7, "consumed size 0x%x (pkt/read 0x%x/0x%x)\n",
                _rSwdmxWmvInfo.u4ConsumedSize,
                _rSwdmxWmvInfo.u4PacketSize,
                _rSwdmxWmvInfo.u4FeederReadSize);
        }
    }
    else if (_rSwdmxWmvInfo.fgEofParsed)
    {
        LOG(0, "Send Eos 10(tb, EOF)\n");
        _SWDMX_WMVSendEOS(prSwdmxInst);
        if (_rSwdmxWmvInfo.fgFirstFrame)
        {
            _rSwdmxWmvInfo.fgFirstFrame = FALSE;
        }
        _rSwdmxWmvInfo.u4ConsumedSize = 0;
        _rSwdmxWmvInfo.fgEofParsed = FALSE;
        _rSwdmxWmvInfo.fgNeedSetFdrReq = TRUE;
        return TRUE;
    }
    else if (fgTrickMode)
    {
        //LOG(0, "time based trick mode!\n");
        BOOL fgRetValue=TRUE;
	   
        //x_thread_delay(1000);
        fgRetValue= _SWDMX_WMVTrickModeParsingTimebased(u1SrcId);
//      if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
//      {
//        if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
//        {
//          if((!fgTrickTrigger[prSwdmxInst->u1SwdmxId])&&(AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId)))
//          {
//            AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4AudDspPts[prSwdmxInst->u1SwdmxId], TRUE);
//          }
//        }
//      }
	   
	   return fgRetValue;
    }

    #if 1
    {
        LOG(6, "parse packet(%d of %d, remain %d, pkt:%d, AudObjSz:%d)(16bytes@0x%x):  %02x %02x %02x %02x %02x %02x %02x %02x "
               "%02x %02x %02x %02x %02x %02x %02x %02x\n", 
            _rSwdmxWmvInfo.u4ConsumedSize,
            _rSwdmxWmvInfo.u4FeederReadSize,
            _rSwdmxWmvInfo.u4FeederReadSize - _rSwdmxWmvInfo.u4ConsumedSize,
            _rSwdmxWmvInfo.u4PacketSize,
            _rSwdmxWmvInfo.u4AudioObjSz,
            _rSwdmxWmvInfo.u4PacketStartAddr,
            PKTBYTE(0), PKTBYTE(1), PKTBYTE(2), PKTBYTE(3), 
            PKTBYTE(4), PKTBYTE(5), PKTBYTE(6), PKTBYTE(7), 
            PKTBYTE(8), PKTBYTE(9), PKTBYTE(10), PKTBYTE(11), 
            PKTBYTE(12), PKTBYTE(13), PKTBYTE(14), PKTBYTE(15));

        /* 
        _rSwdmxWmvInfo.u4PacketStartAddr += ((UINT32)prSwdmxInst->u8FileOffset
                           + (UINT32)WMV_DATA_OBJ_HEADER);
        LOG(6, "(16bytes@0x%x+5677):  %02x %02x %02x %02x %02x %02x %02x %02x "
               "%02x %02x %02x %02x %02x %02x %02x %02x\n",
            _rSwdmxWmvInfo.u4PacketStartAddr,
            PKTBYTE(0), PKTBYTE(1), PKTBYTE(2), PKTBYTE(3),
            PKTBYTE(4), PKTBYTE(5), PKTBYTE(6), PKTBYTE(7),
            PKTBYTE(8), PKTBYTE(9), PKTBYTE(10), PKTBYTE(11),
            PKTBYTE(12), PKTBYTE(13), PKTBYTE(14), PKTBYTE(15));
        _rSwdmxWmvInfo.u4PacketStartAddr -= ((UINT32)prSwdmxInst->u8FileOffset
                           + (UINT32)WMV_DATA_OBJ_HEADER);
        */
    }
    #endif
    //pasrsing packet    
    //do
    {
        UINT32 u4AddrDelta;

        _rSwdmxWmvInfo.u2SendPayloadNo = 0;
        _rSwdmxWmvInfo.fgBlock = FALSE;

        u4AddrDelta = _rFeederInfo.u4EndAddr - _rSwdmxWmvInfo.u4PacketStartAddr;

        if (u4AddrDelta < _rSwdmxWmvInfo.u4PacketSize)
        {
            LOG(1,"swdmx wmv handle ring buffer(tb)\n");
            x_memcpy((VOID*)VIRTUAL(_rFeederInfo.u4StartAddr-u4AddrDelta),(VOID*)VIRTUAL(_rSwdmxWmvInfo.u4PacketStartAddr),u4AddrDelta);
            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederInfo.u4StartAddr-u4AddrDelta;
            HalFlushDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)u4AddrDelta);
        }

        if (_rSwdmxWmvInfo.fgBrokenFile)
        {
            LOG(0, "broken file flag TRUE, set to FALSE, parsing packet now\n");
            _rSwdmxWmvInfo.fgBrokenFile = FALSE;
        }
        if (!(PKTBYTE(0) == 0x82
              && PKTBYTE(1) == 0x00
              && PKTBYTE(2) == 0x00))
        {
            SWDMX_CUR_CMD_T rCmd;
            rCmd.i4PlaySpeed = _rSwdmxWmvInfo.i4WMVPlaySpeed;
            LOG(1,"packet does not start with 0x820000, skip it and seek to 0!\n");
            _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
            _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = 0;
            _rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr = FALSE;
            _SWDMX_WMVSeekTimebased(u1SrcId, prRangeList, &rCmd);
            if(_rSwdmxWmvInfo.fgEnAudio)
            {
                #ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DSPCmdStop(AUD_DSP0, prSwdmxInst->u1AdecId);
                AUD_DSPCmdPlay(AUD_DSP0, prSwdmxInst->u1AdecId);
                #else
                AUD_DSPCmdStop(prSwdmxInst->u1AdecId);
                AUD_DSPCmdPlay(prSwdmxInst->u1AdecId);
                #endif
            }
            _rSwdmxWmvInfo.u4ConsumedSize = 0;
            return TRUE;
        }
        else
        {
            _WMVParsingPacket(prSwdmxInst, 
                              _rSwdmxWmvInfo.u4PacketStartAddr,
                              0,
                              _rSwdmxWmvInfo.u4PacketSize);
        }
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
           if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
           {
             u4AudDspPts[prSwdmxInst->u1SwdmxId]=1;
           }
        }

        if (_rSwdmxWmvInfo.fgBrokenFile || _rSwdmxWmvInfo.fgDemuxFinish)
        {
            #if 1
            LOG(2, "Send Eos 11(tb)(broken:%s, dmx_finish:%s)\n",
                _rSwdmxWmvInfo.fgBrokenFile?"true":"false",
                _rSwdmxWmvInfo.fgDemuxFinish?"true":"false");
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return TRUE;
            #else
            LOG(2, "broken:%s, dmx_finish:%s, ignore\n",
                _rSwdmxWmvInfo.fgBrokenFile?"true":"false",
                _rSwdmxWmvInfo.fgDemuxFinish?"true":"false");
            _rSwdmxWmvInfo.u4PacketStartAddr += _rSwdmxWmvInfo.u4PacketSize;
            _rSwdmxWmvInfo.u4ConsumedSize += _rSwdmxWmvInfo.u4PacketSize;
            _g_u4PacketCountTb++;
            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
            _rSwdmxWmvInfo.u4FeederReqID++;
            return TRUE;
            #endif
        }

        if (_rSwdmxWmvInfo.fgBlock)
        {
            //LOG(0,"fgBlock: TRUE , leaving %s\n", __func__);
            return FALSE;
        }
        else
        {
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;
        }

        _rSwdmxWmvInfo.u4PacketStartAddr += _rSwdmxWmvInfo.u4PacketSize;
        _rSwdmxWmvInfo.u4ConsumedSize += _rSwdmxWmvInfo.u4PacketSize;
        _g_u4PacketCountTb++;
        //_rSwdmxWmvInfo.u4CurParsingPacketNo++;

        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);

        if (_rSwdmxWmvInfo.fgGotEofFromWMVMsgQ == TRUE)
        {
            LOG(0, "\n\n\n!!!eof has been parsed!!!(fdr.wp:0x%x)\n\n\n\n", _rFeederReqData.u4WriteAddr);
            _rSwdmxWmvInfo.fgEofParsed = TRUE;
        }
        #if 1
        if (_rSwdmxWmvInfo.u4ConsumedSize == _rSwdmxWmvInfo.u4FeederReadSize)
        {
            LOG(6, "parse packet(%d of %d, remain:%d, pkt:%d, AudObjSz:%d)\n", 
                _rSwdmxWmvInfo.u4ConsumedSize,
                _rSwdmxWmvInfo.u4FeederReadSize,
                _rSwdmxWmvInfo.u4FeederReadSize - _rSwdmxWmvInfo.u4ConsumedSize,
                _rSwdmxWmvInfo.u4PacketSize,
                _rSwdmxWmvInfo.u4AudioObjSz);
            _rSwdmxWmvInfo.u4ConsumedSize = 0;
            return TRUE;
        }

        if (_rSwdmxWmvInfo.u4FeederReadSize - _rSwdmxWmvInfo.u4ConsumedSize 
                < _rSwdmxWmvInfo.u4PacketSize)
        {
            LOG(0, "remainder data(0x%x) is too small to compose a packet(0x%x), drop!(tb)\n",
                _rSwdmxWmvInfo.u4FeederReadSize - _rSwdmxWmvInfo.u4ConsumedSize,
                _rSwdmxWmvInfo.u4PacketSize);
            if (!_rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr)
            {
                LOG(0, "Send Eos normally(tb)\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
            }
            _rSwdmxWmvInfo.u4ConsumedSize = 0;
            return TRUE;
        }
        #else
        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            UINT32 u4WMVPacketSize = 0;
            u4WMVPacketSize = _rSwdmxWmvInfo.u4PacketSize;
       
            if (_rSwdmxWmvInfo.u4ConsumedSize + u4WMVPacketSize > _rSwdmxWmvInfo.u4FeederReadSize)
            {
                LOG(0,"feeder read size is smaller than request data size(tb)\n");
                LOG(0,"u4ConsumedSize(0x%x) + u4WMVPacketSize(0x%x) > u4FeederReadSize(0x%x)(tb)\n",
                    _rSwdmxWmvInfo.u4ConsumedSize,
                    u4WMVPacketSize,
                    _rSwdmxWmvInfo.u4FeederReadSize);
                LOG(2,"Send Eos 14(tb)\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
                _rSwdmxWmvInfo.u4ConsumedSize = 0;
                return TRUE;
            }
        }
        #endif
        //return to main loop when finishing parsing a packet
        //LOG(0,"parsing packet number : %d/%d\n",_rSwdmxWmvInfo.u4CurParsingPacketNo,_rSwdmxWmvInfo.u4TotalPacketNo);
        return TRUE;
    }
    //} while (_rSwdmxWmvInfo.u4CurParsingPacketNo < _rSwdmxWmvInfo.u4TotalPacketNo);

    //return TRUE;
}

BOOL _SWDMX_WMVDeliverData(SWDMX_INFO_T *prSwdmxInst, SWDMX_RANGE_LIST_T* prRangeList)
{
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    BOOL fgRet = FALSE;
    WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;

    BOOL fgTrickMode = prSwdmxInst->ePlayMode==SWDMX_PLAY_MODE_NORMAL ? FALSE : TRUE;
#ifdef WMA_TRICK_MODE
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
      if(_rSwdmxWmvInfo.i4WMVPlaySpeed>=2000)
      {
        fgTrickMode=FALSE;
      }
    }
#endif

    if (_rSwdmxWmvInfo.fgDemuxFinish)
    {
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES && _rSwdmxWmvInfo.i4WMVPlaySpeed<0)
		{
		   if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
		   {
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    if((!fgTrickTrigger[prSwdmxInst->u1SwdmxId])&&(AUD_IsDecoderRealPlay(AUD_DSP0, prSwdmxInst->u1AdecId)))
#else
			 if((!fgTrickTrigger[prSwdmxInst->u1SwdmxId])&&(AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId)))
#endif			 	
			 {
			   AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, 
                                    &u4AudDspPts[prSwdmxInst->u1SwdmxId], 
                                    FALSE);
			 }
		   }
		}
        if(!_rSwdmxWmvInfo.fgDeliverVEos)
        {
            _SWDMX_WMVSendVEOS(prSwdmxInst);
        }
        if(!_rSwdmxWmvInfo.fgDeliverAEos)
        {
            _SWDMX_WMVSendAEOS(prSwdmxInst);
        }
		x_thread_delay(20);
        return FALSE;
    }
    if (_rSwdmxWmvInfo.fgFeederInvalid)
    {
        _rSwdmxWmvInfo.fgFeederInvalid = FALSE;
    }

#if 0     // move to swdmx_drvif.c common part
    if(_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
    {
        _WMVCheckDataExhausted(prSwdmxInst);
    }        
#endif

    if ( !_rSwdmxWmvInfo.fgBlock && _rSwdmxWmvInfo.fgFirstFrame && !fgTrickMode )
    {
        //VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(prSwdmxInst->u1VdecId);

        //_SWDMX_WMVConsumeFeeder(prSwdmxInst);

        if (_rSwdmxWmvInfo.fgFirstReqFeederData)
        {
            _SWDMX_WMVRequestFeederData(prSwdmxInst, 0,0,FALSE);
            _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
        }

        fgRet = _SWDMX_WMVGetFeederData(prSwdmxInst, FALSE);
        if (!fgRet)
        {
            if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
            {
                return FALSE;
            }
        }
        else
        {
            _rSwdmxWmvInfo.fgFirstFrame = FALSE;
            _SWDMX_WMVRequestFeederData(prSwdmxInst, 0,0,FALSE);
            _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
        }

        if(_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);
            i4Ret = x_msg_q_receive(&u2MsgQIdx, &rWmvFeederReqData, &zMsgSize,
                                &(_hWMVFeederDataQue), 1, X_MSGQ_OPTION_WAIT);

            if (i4Ret != OSR_OK)
            {
                ASSERT(0);
            }

            x_memcpy((VOID*)&_rFeederReqData,(VOID*)&rWmvFeederReqData.rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
            _u4DefalutReqSize = rWmvFeederReqData.u4RequestFeederSize;

            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederReqData.u4WriteAddr + _rFeederReqData.u4AlignOffset; 

            _rSwdmxWmvInfo.u4FeederReadSize = _rFeederReqData.u4ReadSize-_rFeederReqData.u4AlignOffset;
#ifdef CC_NETFLIX_CMPB_SUPPORT
            if(_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
            {
                _rSwdmxWmvInfo.u4PacketStartAddr -= _u4RemainPacketDataSize;
                _rSwdmxWmvInfo.u4FeederReadSize += _u4RemainPacketDataSize;
            }
#endif
            //get feeder buffer information
            if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) != FEEDER_E_OK)
            {
                LOG(3, "_SWDMX_WMVDeliverData FeederGetBufferInfo() error.\n");
                return FALSE;
            }

            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
        }
        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            if (_rSwdmxWmvInfo.u8BeginParseOfst == 0)
            {
                UINT64 u8Field;
                x_memcpy(&(u8Field),(VOID*)VIRTUAL(_rSwdmxWmvInfo.u4PacketStartAddr+16),8);
                _rSwdmxWmvInfo.u4DataObjSize = (UINT32)u8Field;
                x_memcpy(&(u8Field),(VOID*)VIRTUAL(_rSwdmxWmvInfo.u4PacketStartAddr+40),8);
                _rSwdmxWmvInfo.u4TotalPacketNo = (UINT32)u8Field;
            }
            else
            {
                _rSwdmxWmvInfo.u4DataObjSize = 0;
                _rSwdmxWmvInfo.u4TotalPacketNo = 0;
            }

            if (_rSwdmxWmvInfo.u4TotalPacketNo ==0 || _rSwdmxWmvInfo.u4DataObjSize==0 || _prRangeInfo->ui4_sz_min_acces_unt ==0)
            {
                UINT64 u8Remainder;
                if (_prRangeInfo->ui4_sz_min_acces_unt ==0)
                {
                    LOG(2,"Bad file, unable to be played!!! Send Eos 8\n");
                    _SWDMX_WMVSendEOS(prSwdmxInst);
                    return FALSE;
                }
                
                _rSwdmxWmvInfo.u4DataObjSize = (UINT32)_rSwdmxWmvInfo.u8FileSize;
                _rSwdmxWmvInfo.u4TotalPacketNo = (UINT32)u8Div6432((_rSwdmxWmvInfo.u8FileSize-WMV_DATA_OBJ_HEADER),
                                                 _prRangeInfo->ui4_sz_min_acces_unt, &u8Remainder);

                if (_rSwdmxWmvInfo.u4TotalPacketNo == 0)  // tmp solution for DTV00212590
                    _rSwdmxWmvInfo.u4TotalPacketNo = 1;   // this will cause _SWDMX_WMVSendEOS immediately

                //_SWDMX_WMVSendEOS(prSwdmxInst);
                //return FALSE;
            }
            _rSwdmxWmvInfo.u4PacketSize = (_rSwdmxWmvInfo.u4DataObjSize-WMV_DATA_OBJ_HEADER)/_rSwdmxWmvInfo.u4TotalPacketNo;

            if (_rSwdmxWmvInfo.u4PacketSize != _prRangeInfo->ui4_sz_min_acces_unt && _prRangeInfo->ui4_sz_min_acces_unt!=0)
            {
                _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
            }
        }
        else
        {
            _rSwdmxWmvInfo.u4DataObjSize = 0xFFFFFFF0;
            _rSwdmxWmvInfo.u4TotalPacketNo = 0xFFFFFFF0;
        }
        
        LOG(5,"total data packet : %d \n",_rSwdmxWmvInfo.u4TotalPacketNo);
        LOG(5,"packet size : %d \n",_rSwdmxWmvInfo.u4PacketSize);

        //_rSwdmxWmvInfo.u4CurPos = WMV_DATA_OBJ_HEADER; //packet header size is 50 bytes
        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            if (_rSwdmxWmvInfo.u8BeginParseOfst == 0)
            {
                _rSwdmxWmvInfo.u4PacketStartAddr += WMV_DATA_OBJ_HEADER;
                _rSwdmxWmvInfo.u4ConsumedSize = WMV_DATA_OBJ_HEADER;
            }
        }
        else
        {
            _rSwdmxWmvInfo.u4ConsumedSize = 0;
        }

        _rSwdmxWmvInfo.fgFirstFrame = FALSE;

        if (_rSwdmxWmvInfo.fgEnVideo)
        {
            VDEC_StartPTS(prSwdmxInst->u1VdecId,TRUE,0,NULL); //maiyou
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                VDEC_StartPTS(prSwdmxInst->u1Vdec2Id,TRUE,0,NULL); 
            }
        }

#if 1
        if (!_SWDMX_WMVFeederIsReady(prSwdmxInst))
        {
            return TRUE;
        }
#endif
    }

    else if (!_rSwdmxWmvInfo.fgBlock && (_rSwdmxWmvInfo.u4CurParsingPacketNo < _rSwdmxWmvInfo.u4TotalPacketNo)
             && !fgTrickMode )
    {
        if (!_SWDMX_WMVIsFeederBufferFull(prSwdmxInst))
        {
            if ( _rSwdmxWmvInfo.fgFirstReqFeederData )
            {
                _SWDMX_WMVRequestFeederData(prSwdmxInst, 0,0,FALSE);
                _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
            }

            if(_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
            {
                if(_prRangeInfo->e_mm_src_type!=MM_SRC_TYPE_HIGH_SPEED_STORAGE)
                {
                    UINT64 u8UnParsedPacket = 0;
                    UINT64 u8UnParsedData   = 0;
                    UINT64 u8Remainder      = 0;
                    u8UnParsedData = _rSwdmxWmvInfo.u8LastFileParsingPosition
                                     - _prRangeInfo->ui8_fileoffset
                                     - WMV_DATA_OBJ_HEADER
                                     - (UINT64)(_rSwdmxWmvInfo.u4CurParsingPacketNo)
                                       * _rSwdmxWmvInfo.u4PacketSize;
                    u8UnParsedPacket = u8Div6432(u8UnParsedData,
                                                 _rSwdmxWmvInfo.u4PacketSize,
                                                 &u8Remainder);
                    UNUSED(u8Remainder);
                    if(u8UnParsedPacket>=2)
                    {
                        fgRet = _SWDMX_WMVGetFeederData(prSwdmxInst, TRUE); 
                    }
                    else
                    {
                        fgRet = _SWDMX_WMVGetFeederData(prSwdmxInst, FALSE);		 
                    }

                    if (!fgRet)
                    {
                        BOOL fgReq = FALSE;
                        if (FeederGetBufferInfo(prSwdmxInst->eFeederSrc, &_rFeederInfo) == FEEDER_E_OK)
                        {
                            if(_rFeederInfo.u4BytesInBuf < 192*1024)
                            {
                                fgReq = TRUE;
                            }
                        }
                         if ( _u4RequestDataSize ==0 || fgReq)
                         {
                        	 //need to request data again
                        	 _SWDMX_WMVRequestFeederData(prSwdmxInst,0,0,FALSE);
                        	 _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                         }
                         if (_rSwdmxWmvInfo.u4ConsumedSize ==0 && (u8UnParsedPacket<2 || _rSwdmxWmvInfo.fgDemuxFinish))
                         {
                        	 return FALSE;
                         }
                    	 
                    }
                    else
                    {
                    	 //need to request data again
                    	 _SWDMX_WMVRequestFeederData(prSwdmxInst, 0,0,FALSE);
                    	 _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                    }
                }
                else
                {
                    fgRet = _SWDMX_WMVGetFeederData(prSwdmxInst, FALSE);
                    if (!fgRet)
                    {
                        if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
                        {
                        	return FALSE;
                        }
                        else if ( _u4RequestDataSize ==0 )
                        {
                        	//need to request data again
                        	_SWDMX_WMVRequestFeederData(prSwdmxInst,0,0,FALSE);
                        	_rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                        }
                    }
                    else
                    {
                    	//need to request data again
                    	_SWDMX_WMVRequestFeederData(prSwdmxInst, 0,0,FALSE);
                    	_rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                    }
                }

            }
            else //netflix mode
            {
                UINT16 u2Cnt = 0;
                VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
                
                if((MPV_IsDisplay(prSwdmxInst->u1VdecId)) && u2Cnt>=1)
                {
                    // don't receive feeder ack when data is >=1
                }
                else
                {
                    fgRet = _SWDMX_WMVGetFeederData(prSwdmxInst, FALSE);
                    if (!fgRet)
                    {
                        if (_rSwdmxWmvInfo.fgFeederInvalid || _rSwdmxWmvInfo.fgDemuxFinish)
                        {
                            return FALSE;
                        }
                        else if ( _u4RequestDataSize ==0 )
                        {
                            //need to request data again
                            _SWDMX_WMVRequestFeederData(prSwdmxInst,0,0,FALSE);
                            _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                        }
                    }
                    else
                    {
                        //need to request data again
                        _SWDMX_WMVRequestFeederData(prSwdmxInst, 0,0,FALSE);
                        _rSwdmxWmvInfo.fgFirstReqFeederData = FALSE;
                    }                
                }
            }


            if (!_SWDMX_WMVFeederIsReady(prSwdmxInst))
            {
                x_thread_delay(50);
                return TRUE;
            }

        }
        else
        {
            if (_rSwdmxWmvInfo.u4ConsumedSize == 0)
            {
                UINT16 u2Cnt = 0;
                UINT8 u1Count = 0;
                VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
                if (u2Cnt == 0)
                {
                    LOG(2,"feeder is full but do not acked to swdmx, wait\n");
                    while(_SWDMX_WMVGetFeederData(prSwdmxInst, FALSE) == FALSE)
                    {
                        u1Count++;
                        if(u1Count > 10)
                        {
                            LOG(0,"swdmx can not get data and stop play!\n");
                            break;
                        }
                    }
                }
            }
        }

        if ( _rSwdmxWmvInfo.u4ConsumedSize ==0 )
        {
            WMV_FEEDER_REQ_DATA_T rWmvFeederReqData;
            UINT16 u2Cnt = 0;
            zMsgSize = sizeof(WMV_FEEDER_REQ_DATA_T);

            // tmp solution for DTV00085618
            VERIFY(x_msg_q_num_msgs(_hWMVFeederDataQue, &u2Cnt) == OSR_OK);
            if (u2Cnt == 0)  // which will cause deadlock at the x_msg_q_receive below
            {
                LOG(2,"Send Eos 9\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
                return TRUE;
            }
            
            i4Ret = x_msg_q_receive(&u2MsgQIdx, &rWmvFeederReqData, &zMsgSize,
                                    &(_hWMVFeederDataQue), 1, X_MSGQ_OPTION_WAIT);
            if (i4Ret != OSR_OK)
            {
                ASSERT(0);
            }

            x_memcpy((VOID*)&_rFeederReqData,(VOID*)&rWmvFeederReqData.rFeederReqData,sizeof(FEEDER_REQ_DATA_T));
            _u4DefalutReqSize = rWmvFeederReqData.u4RequestFeederSize;

             if(_rFeederReqData.u4ReadSize == 0)
            {
                LOG(0,"!!!!!!!The feeder ACK  Size is 0\n");
                return FALSE;
            }

            _rSwdmxWmvInfo.u4FeederReadSize = _rFeederReqData.u4ReadSize-_rFeederReqData.u4AlignOffset;
            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederReqData.u4WriteAddr + _rFeederReqData.u4AlignOffset;
            
#ifdef CC_NETFLIX_CMPB_SUPPORT
            if(_prRangeInfo->e_mm_src_type==MM_SRC_TYPE_NETWORK_NETFLIX)
            {
                _rSwdmxWmvInfo.u4FeederReadSize += _u4RemainPacketDataSize;
                _rSwdmxWmvInfo.u4PacketStartAddr -= _u4RemainPacketDataSize;
                x_memcpy((VOID*)(_rSwdmxWmvInfo.u4PacketStartAddr),(VOID*)VIRTUAL((UINT32)_pu1RemainPacketData),_u4RemainPacketDataSize);
                HalFlushDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)_u4RemainPacketDataSize);
            }
#endif
                
            FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);
        }
    }
    else if ((_rSwdmxWmvInfo.u4CurParsingPacketNo >= _rSwdmxWmvInfo.u4TotalPacketNo)
             && (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX))
    {
        LOG(2,"Send Eos 10\n");
        _SWDMX_WMVSendEOS(prSwdmxInst);
        return TRUE;
    }

    else if (fgTrickMode)
    {
       BOOL fgRetValue=TRUE;
	   
        fgRetValue= _SWDMX_WMVTrickModeParsing(prSwdmxInst);
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
          if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
          {
#ifdef CC_AUD_SUPPORT_DUAL_DSP       
            if((!fgTrickTrigger[prSwdmxInst->u1SwdmxId])&&(AUD_IsDecoderRealPlay(AUD_DSP0, prSwdmxInst->u1AdecId)))
#else
            if((!fgTrickTrigger[prSwdmxInst->u1SwdmxId])&&(AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId)))
#endif            	
            {
              AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, 
                                   &u4AudDspPts[prSwdmxInst->u1SwdmxId], 
                                   FALSE);
            }
          }
        }
	   
	   return fgRetValue;
    }

    //pasrsing packet
    //do
    {
        UINT32 u4AddrDelta;

        _rSwdmxWmvInfo.u2SendPayloadNo = 0;
        _rSwdmxWmvInfo.fgBlock = FALSE;

        u4AddrDelta = _rFeederInfo.u4EndAddr - _rSwdmxWmvInfo.u4PacketStartAddr;

        if (u4AddrDelta < _rSwdmxWmvInfo.u4PacketSize)
        {
            LOG(1,"swdmx wmv handle ring buffer\n");
            x_memcpy((VOID*)VIRTUAL(_rFeederInfo.u4StartAddr-u4AddrDelta),(VOID*)VIRTUAL(_rSwdmxWmvInfo.u4PacketStartAddr),u4AddrDelta);
            _rSwdmxWmvInfo.u4PacketStartAddr = _rFeederInfo.u4StartAddr-u4AddrDelta;
            HalFlushDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)u4AddrDelta);
        }

        /* bypass bad packet to avoid system reboot(x_memcpy invalid pointer) */
        if (!(PKTBYTE(0) == 0x82
              && PKTBYTE(1) == 0x00
              && PKTBYTE(2) == 0x00))
        {
            LOG(1,"packet does not start with 0x820000, skip it\n");
            /* 
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return TRUE;
            */
        }
        else
        {
            _WMVParsingPacket(prSwdmxInst, _rSwdmxWmvInfo.u4PacketStartAddr,0,_rSwdmxWmvInfo.u4PacketSize);
        }
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
           if(prSwdmxInst->u1SwdmxId<SWDMX_SOURCE_MAX)
           {
             u4AudDspPts[prSwdmxInst->u1SwdmxId]=1;
           }
        }

        if (_rSwdmxWmvInfo.fgBrokenFile || _rSwdmxWmvInfo.fgDemuxFinish)
        {
            LOG(2,"Send Eos 11\n");
            _SWDMX_WMVSendEOS(prSwdmxInst);
            return TRUE;
        }

#ifdef WMV_BAD_INTLV_SUPPORT
        if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V)
        {
            _rBadIntlv.eState = (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A ? BAD_INTLV_STT_PATCH_A : BAD_INTLV_STT_PATCH_V);
            _rSwdmxWmvInfo.fgBlock = FALSE;
        }
        else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_A || _rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_V)
        {
            _rSwdmxWmvInfo.fgBlock = FALSE;
        }
#endif            
        if (_rSwdmxWmvInfo.fgBlock)
        {
            //LOG(0,"fgBlock : TRUE , leaving _SWDMX_WMVSendData\n");
            return FALSE;
        }
        else
        {
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo = 0;
        }

        _rSwdmxWmvInfo.u4PacketStartAddr += _rSwdmxWmvInfo.u4PacketSize;

        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
        {
            _rSwdmxWmvInfo.u4ConsumedSize += _rSwdmxWmvInfo.u4PacketSize;
        }
        else
        {
#ifndef CC_NETFLIX_CMPB_SUPPORT
            _rSwdmxWmvInfo.u4PacketStartAddr += WMV_NETFLIX_HEADER_SIZE;
            _rSwdmxWmvInfo.u4ConsumedSize += 1;
#else
            _rSwdmxWmvInfo.u4PacketStartAddr += sizeof(NetflixEmbeddedCmd_T); //embedded cmd
            _rSwdmxWmvInfo.u4ConsumedSize += _rSwdmxWmvInfo.u4PacketSize;
            _rSwdmxWmvInfo.u4ConsumedSize += sizeof(NetflixEmbeddedCmd_T);
#endif
        }

        _rSwdmxWmvInfo.u4CurParsingPacketNo++;

        if (_rSwdmxWmvInfo.u4CurParsingPacketNo == _rSwdmxWmvInfo.u4TotalPacketNo
#ifdef __MODEL_slt__
                || _u4VC1SltLastPicPts > 33030
#endif
           )
        {
#ifdef __MODEL_slt__
            if (_rSwdmxWmvInfo.fgVideoPause)
            {
                _WMVStartToPlay(prSwdmxInst);
            }
#endif
#ifdef WMV_BAD_INTLV_SUPPORT
            if (_rBadIntlv.eState != BAD_INTLV_STT_NONE)
            {
                if (_rBadIntlv.eState == BAD_INTLV_STT_PATCH_A)
                {
                    _rBadIntlv.fgAudioEnd = TRUE;
                    _rBadIntlv.eState = BAD_INTLV_STT_TO_REPRS_V;
                }
                else if (_rBadIntlv.eState == BAD_INTLV_STT_PATCH_V)
                {
                    _rBadIntlv.fgVideoEnd = TRUE;
                    _rBadIntlv.eState = BAD_INTLV_STT_TO_REPRS_A;
                }
                else if (_rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A)
                {
                    _rBadIntlv.fgAudioEnd = TRUE;
                    _WMVEndReparse(prSwdmxInst);
                    if (_rBadIntlv.fgAudioEnd && _rBadIntlv.fgVideoEnd)
                    {
                        LOG(2, "Send Eos 13a\n");
                        _SWDMX_WMVSendEOS(prSwdmxInst);
                        return TRUE;
                    }
                }
                else if (_rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V)
                {
                    _rBadIntlv.fgVideoEnd = TRUE;
                    _WMVEndReparse(prSwdmxInst);
                    if (_rBadIntlv.fgAudioEnd && _rBadIntlv.fgVideoEnd)
                    {
                        LOG(2, "Send Eos 13b\n");
                        _SWDMX_WMVSendEOS(prSwdmxInst);
                        return TRUE;
                    }
                }
            }
            else
#endif  // WMV_BAD_INTLV_SUPPORT
            {
                LOG(0,"Send EOS normally, PktNum %d/%d\n", 
                    _rSwdmxWmvInfo.u4CurParsingPacketNo,
                    _rSwdmxWmvInfo.u4TotalPacketNo);
                _SWDMX_WMVSendEOS(prSwdmxInst);
                return TRUE;
                //break;
            }
        }
#ifdef WMV_BAD_INTLV_SUPPORT
        else
        {
            UINT32 u4ADuration, u4VDuration;
            
            if ((_rBadIntlv.eState == BAD_INTLV_STT_PATCH_A || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_A) && !_WMV_AFifo_Low(prSwdmxInst, &u4ADuration) && _WMV_VFifo_Low(prSwdmxInst, &u4VDuration))
            {
                LOG(3, "Video underflow (%dms) due to bad interleave (%d)!!!\n", u4VDuration, __LINE__);
                _rBadIntlv.eState = (_rBadIntlv.eState == BAD_INTLV_STT_PATCH_A ? BAD_INTLV_STT_TO_REPRS_V : BAD_INTLV_STT_TO_PATCH_V);
            }
            else if ((_rBadIntlv.eState == BAD_INTLV_STT_PATCH_V || _rBadIntlv.eState == BAD_INTLV_STT_REPARSE_V) && !_WMV_VFifo_Low(prSwdmxInst, &u4VDuration) && _WMV_AFifo_Low(prSwdmxInst, &u4ADuration))
            {
                LOG(3, "Audio underflow (%dms) due to bad interleave (%d)!!!\n", u4ADuration, __LINE__);
                _rBadIntlv.eState = (_rBadIntlv.eState == BAD_INTLV_STT_PATCH_V ? BAD_INTLV_STT_TO_REPRS_A : BAD_INTLV_STT_TO_PATCH_A);
            }
            
            if (_rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_V || _rBadIntlv.eState == BAD_INTLV_STT_TO_REPRS_A)
            {
                _rBadIntlv.u4EndPatchPktNo = _rSwdmxWmvInfo.u4CurParsingPacketNo;
                _rBadIntlv.u4EndPatchPldNo = 0;
            }
            else if (_rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_V || _rBadIntlv.eState == BAD_INTLV_STT_TO_PATCH_A)
            {
                _rBadIntlv.u4BeginSkipPktNo = _rSwdmxWmvInfo.u4CurParsingPacketNo;
                _rBadIntlv.u4BeginSkipPldNo = 0;
            }
        }
#endif  // WMV_BAD_INTLV_SUPPORT

        FeederUpdateReadPointer(prSwdmxInst->eFeederSrc, _rSwdmxWmvInfo.u4PacketStartAddr);

#ifdef WMV_BAD_INTLV_SUPPORT
        if (_WMVBeginReparse(prSwdmxInst))
        {
            return TRUE;
        }
        if (_WMVBeginRePatch(prSwdmxInst))
        {
            return TRUE;
        }
#endif

#ifndef CC_NETFLIX_CMPB_SUPPORT
        if (_prRangeInfo->e_mm_src_type != MM_SRC_TYPE_NETWORK_NETFLIX)
#endif         
        {
           UINT32 u4WMVPacketSize = 0;
#ifdef CC_NETFLIX_CMPB_SUPPORT            
           if (_prRangeInfo->e_mm_src_type == MM_SRC_TYPE_NETWORK_NETFLIX)
           {
               u4WMVPacketSize = NETFLIX_CMMP_BUFFER_SIZE;
           }
           else
#endif            
           {
               u4WMVPacketSize = _rSwdmxWmvInfo.u4PacketSize;
           }
       
           if (_rSwdmxWmvInfo.u4ConsumedSize + u4WMVPacketSize > _rSwdmxWmvInfo.u4FeederReadSize)
            {
                if (_rSwdmxWmvInfo.fgFirstFrame)
                {
                    _rSwdmxWmvInfo.fgFirstFrame = FALSE;
                }
#ifdef CC_NETFLIX_CMPB_SUPPORT
                
                if(_prRangeInfo->e_mm_src_type==MM_SRC_TYPE_NETWORK_NETFLIX)
                {
                    _u4RemainPacketDataSize = _rSwdmxWmvInfo.u4FeederReadSize - _rSwdmxWmvInfo.u4ConsumedSize;

                   HalInvalidateDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)_u4RemainPacketDataSize);
                   
                   x_memcpy((VOID*)VIRTUAL((UINT32)_pu1RemainPacketData),(VOID*)_rSwdmxWmvInfo.u4PacketStartAddr,_u4RemainPacketDataSize);
                   HalFlushDCacheMultipleLine((UINT32)(_rSwdmxWmvInfo.u4PacketStartAddr), (UINT32)_u4RemainPacketDataSize);
                }
#endif
                _rSwdmxWmvInfo.u4ConsumedSize = 0;
                return TRUE;
            }

            if (_rSwdmxWmvInfo.u4ConsumedSize + _rSwdmxWmvInfo.u4PacketSize > _rSwdmxWmvInfo.u4FeederReadSize)
            {
                LOG(0,"feeder read size is smaller than request data size\n");
                LOG(2,"Send Eos 14\n");
                _SWDMX_WMVSendEOS(prSwdmxInst);
                _rSwdmxWmvInfo.u4ConsumedSize = 0;
                return TRUE;
            }
        }
#ifndef CC_NETFLIX_CMPB_SUPPORT        
        else
        {
            if (_rSwdmxWmvInfo.u4ConsumedSize + 1 > _rSwdmxWmvInfo.u4FeederReadSize)
            {
                if (_rSwdmxWmvInfo.fgFirstFrame)
                {
                    _rSwdmxWmvInfo.fgFirstFrame = FALSE;
                }
                _rSwdmxWmvInfo.u4ConsumedSize = 0;
                //LOG(0,"swdmx finish consume feeder data size = %d\n",_rSwdmxWmvInfo.u4FeederReadSize);
                return TRUE;
            }
        }
#endif        
        //return to main loop when finishing parsing a packet
        //LOG(0,"parsing packet number : %d/%d\n",_rSwdmxWmvInfo.u4CurParsingPacketNo,_rSwdmxWmvInfo.u4TotalPacketNo);
        return TRUE;
    }
    //} while (_rSwdmxWmvInfo.u4CurParsingPacketNo < _rSwdmxWmvInfo.u4TotalPacketNo);

    //return TRUE;
}

static VOID _SWDMX_WMVSendAEOS(SWDMX_INFO_T *prSwdmxInst)
{
    if (_rSwdmxWmvInfo.fgEnADmx && _rSwdmxWmvInfo.fgEnAudio)
    {
        DMX_MM_DATA_T rDmxMMData_EOS;
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        INT32 i = 0;
        #endif
        x_memset((VOID*)&rDmxMMData_EOS,0,sizeof(DMX_MM_DATA_T));
        rDmxMMData_EOS.u4BufStart = _rFeederInfo.u4StartAddr;
        rDmxMMData_EOS.u4BufEnd = _rFeederInfo.u4EndAddr;
        rDmxMMData_EOS.u4StartAddr = _rFeederInfo.u4StartAddr;
        rDmxMMData_EOS.u4FrameSize = 4;
        rDmxMMData_EOS.fgEOS = TRUE;
        rDmxMMData_EOS.fgFrameHead= TRUE;
        rDmxMMData_EOS.fgIgnoreData = TRUE;
        LOG(0,"!!!!!!!! deliever AEOS flag\n");
        if (prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK)
        {
            LOG(0,"PlaySpeed = %d\n",_rSwdmxWmvInfo.i4WMVPlaySpeed);
        }
        else
        {
            LOG(0,"PlaySpeed=%d,Index=%d\n",_rSwdmxWmvInfo.i4WMVPlaySpeed,_rSwdmxWmvInfo.u4CurKeyIdx);
        }
        _rSwdmxWmvInfo.fgDemuxFinish = TRUE;
        _prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
        
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        if(_rSwdmxWmvInfo.fgIsDRM)
        {
            if (_WMVDmxMoveData(prSwdmxInst,
                                prSwdmxInst->u1SwdmxId,
                                _rSwdmxWmvInfo.u1AudPreBufPid[_rSwdmxWmvInfo.u4AStreamId],
                                &rDmxMMData_EOS, 500) != TRUE)
            {
                LOG(0,"Swdmx WMV Drm Fail to send audio EOS!!!\n");
                return ;
            }
        }
        else
        {
            for (i = 0; i < WMV_MAX_AUDIO_TRACK; i++)
            {
                rDmxMMData_EOS.u1Idx = _rSwdmxWmvInfo.u1AudPreBufPid[i];
                if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData_EOS))
                {
                    LOG(0, "Fail to send audio eos\n");
                    return;
                }
                else
                {
                    LOG(0, "send audio eos to AFIFO\n");
                }
            }
        }
        #else
        rDmxMMData_EOS.u1Idx = prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid;
        if (_rSwdmxWmvInfo.fgIsDRM)
        {
            if (_WMVDmxMoveData(prSwdmxInst,
                                prSwdmxInst->u1SwdmxId,
                                rDmxMMData_EOS.u1Idx,
                                &rDmxMMData_EOS, 500) != TRUE)
            {
                LOG(0,"Swdmx WMV Drm Fail to send audio EOS!!!\n");
                return ;
            }
        }
        else
        {
            if (!_SWDMX_MoveData(prSwdmxInst->u1SwdmxId, &rDmxMMData_EOS))
            {
                LOG(0, "Fail to send audio eos\n");
                return;
            }
            else
            {
                LOG(0, "send audio eos to AFIFO\n");
            }
        }
        #endif
        _rSwdmxWmvInfo.fgDeliverAEos = TRUE;
    }
    else
    {
        LOG(5, "cannot send EOS for fgEnAudio is FALSE(fgEnADmx?%d)\n", _rSwdmxWmvInfo.fgEnADmx);
    }
}


static VOID _SWDMX_WMVSendVEOS(SWDMX_INFO_T *prSwdmxInst)
{
    if (_rSwdmxWmvInfo.fgEnVideo)//protect func
    {
        DMX_MM_DATA_T rDmxMMData_EOS;
        x_memset((VOID*)&rDmxMMData_EOS,0,sizeof(DMX_MM_DATA_T));
        rDmxMMData_EOS.u4BufStart = _rFeederInfo.u4StartAddr;
        rDmxMMData_EOS.u4BufEnd = _rFeederInfo.u4EndAddr;
        rDmxMMData_EOS.u4StartAddr = _rFeederInfo.u4StartAddr;
        rDmxMMData_EOS.u4FrameSize = 4;
        rDmxMMData_EOS.fgEOS = TRUE;
        rDmxMMData_EOS.fgFrameHead= TRUE;
        rDmxMMData_EOS.fgIgnoreData = TRUE;
        LOG(0,"!!!!!!!! deliever VEOS flag\n");
        if (prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK)
        {
            LOG(0,"PlaySpeed = %d\n",_rSwdmxWmvInfo.i4WMVPlaySpeed);
        }
        else
        {
            LOG(0,"PlaySpeed=%d,Index=%d\n",_rSwdmxWmvInfo.i4WMVPlaySpeed,_rSwdmxWmvInfo.u4CurKeyIdx);
        }
        //update variable to know we have sent EOS flag
        _rSwdmxWmvInfo.fgDemuxFinish = TRUE;
        _prRangeList->eRangeStats = eSWDMX_RANGE_STATE_PENDING;
        if (_SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1VdecId,TRUE, TRUE) == FALSE)
        {
            LOG(0,"set dmx error when sending EOS flag\n");
            return;
        }
        if (_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid, &rDmxMMData_EOS, 500) != TRUE)
        {
            LOG(0, "Demuxer fails to move frame start_code data.\n");
        }
        
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
            if (_SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1Vdec2Id,TRUE, TRUE) == FALSE)
            {
                LOG(0,"set dmx error when sending EOS flag\n");
                return;
            }
            
            if (_WMVDmxMoveData(prSwdmxInst,prSwdmxInst->u1SwdmxId,
                                prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid, 
                                &rDmxMMData_EOS, 500) != TRUE)
            {
                LOG(0, "Demuxer fails to move frame start_code data.\n");
            }
        }
        _rSwdmxWmvInfo.fgDeliverVEos = TRUE;
    }
}



VOID _SWDMX_WMVSendEOS(SWDMX_INFO_T *prSwdmxInst)
{
    if(!_rSwdmxWmvInfo.fgEnVideo)
    {
        LOG(0,"force Aout!!!!\n");
        STC_StartStc(prSwdmxInst->u1AdecId);
        AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
    }
    _SWDMX_WMVSendVEOS(prSwdmxInst);
    _SWDMX_WMVSendAEOS(prSwdmxInst);
    LOG(0,"!!!!!!!! Send EOS flag\n");
}

BOOL _SWDMX_WMVInit(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType)
{    
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);        
    
#if 0  // megaa 20100321
    static UINT32 _u4SwdmxWmvMsgQID = 0;
    char szMsgQName[] = { "WMVFQn" };
#endif

    LOG(5, "_SWDMX_WMVInit \n");

#ifdef WMV_BAD_INTLV_DEBUG
    _u4AFifoAccuLen = 0;
    _u4VFifoAccuLen = 0;
#endif
    
    //set audio call back function
    AUD_DrvSetWMAErrFunc(prSwdmxInst->u1AdecId, _WMVAudioErrorCallback, (VOID *)prSwdmxInst);  // megaa 20090828

#if 0  // for multi-instance, something has been done in common part
    FeederInstallCallback(prSwdmxInst->eFeederSrc, FEEDER_NOTIFY_DATA_REQ_DONE,
                          _SWDMX_FeederCallBack, (UINT32)&_rFeederInfo);
    // Initialize dmx
    fgRet = DMX_SelectInputType(DMX_IN_PLAYBACK_MM);
    if (fgRet == FALSE)
    {
        LOG(3, "DMX_SelectInputType DMX_IN_PLAYBACK_MM\n");
        return fgRet;
    }

    // moved to _SWDMX_WMVInitDmx
    DMX_SetToDecoder(TRUE);
    {
        DMX_DECODER_CALLBACKS_T rDecoderCallbacks;
        rDecoderCallbacks.pfnSendVideoPes = _WMVVideoCallback;//_VideoCallback;
        rDecoderCallbacks.pfnSendAudioPes = _WMVAudioCallback;//AUD_SendAudioPes;
        rDecoderCallbacks.pfnUpdateVideoWp = VDEC_SetWptr;
        //rDecoderCallbacks.pfnSendAudioPes = AUD_SendAudioPes;
        DMX_SetDecoderCallbacks(&rDecoderCallbacks);
    }
#endif

#if 1  // sometimes &_eCurFMT is not 4 byte aligned so that accessing it as UINT32* will cause data abort
    {
        UINT32 u4CurFMT;

        _SWDMX_GetInfo(u1SrcId, eSWDMX_CONTAINER_TYPE,(UINT32*)(&u4CurFMT),0,0);
        _eCurFMT = (ENUM_SWDMX_FMT_T)u4CurFMT;
        _rSwdmxWmvInfo.u4AudioCount = 0;
    }
#else
    _SWDMX_GetInfo(eSWDMX_CONTAINER_TYPE,(UINT32*)(&_eCurFMT),0,0);
#endif

    //x_memset(&(_rSwdmxWmvInfo),0,sizeof(SWDMX_WMV_INFO_T));  megaa 20100321: this has been done in common part
    
    _rSwdmxWmvInfo.u1AudioStreamId = 0xFF;
    _rSwdmxWmvInfo.fgDataExhausted = TRUE;
    _rSwdmxWmvInfo.u4NumberOfBitstream = 1;
    _rSwdmxWmvInfo.fgAudStrmDisabled = TRUE;

#if 1  // megaa 20100321
    _WMVMsgQInit(u1SrcId);
#else
    if ( _hWMVFeederDataQue == NULL )
    {
        szMsgQName[5] = '0' + u1SrcId;  //_u4SwdmxWmvMsgQID++;
        //if (_u4SwdmxWmvMsgQID >= 10)
        //    _u4SwdmxWmvMsgQID = 0;
        // create receive feeder data queue
        VERIFY(x_msg_q_create(&_hWMVFeederDataQue, szMsgQName, sizeof(WMV_FEEDER_REQ_DATA_T),
                              SWMDX_WMV_FEEDER_DATA_Q_SIZE) == OSR_OK);
    }
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
#endif
#ifdef CC_NETFLIX_CMPB_SUPPORT
    _u4RemainPacketDataSize = 0;
#endif
    _rSwdmxWmvInfo.fgFirstFrame = TRUE;
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.fgBeginPlay = TRUE;
    _rSwdmxWmvInfo.u8SeekPTS = 0xFFFFFFFF;
    _rSwdmxWmvInfo.u8SeekPTS <<= 32;
    _rSwdmxWmvInfo.u8SeekPTS += 0xFFFFFFFF;
    _rSwdmxWmvInfo.u4AStreamId = 0xff;
    _rSwdmxWmvInfo.u4VStreamId = 0xff;
    _rSwdmxWmvInfo.u1AudPid = 0xff;
    _rSwdmxWmvInfo.u1VidPid = 0xff;
    _rSwdmxWmvInfo.u1Vid2Pid = 0xff;
    _rSwdmxWmvInfo.u4AudioObjSz = 0;
    _fgNetflixEos = FALSE;
    _fgDataExhausted = TRUE;
    _u1AudioStreamId = 0xFF;
    _u4PrevFramePTS = 0;
    _u4DefalutReqSize = 0;
    _fgReceiveAudioPes = FALSE;
    _fgReceiveVideoPes = FALSE;
    _rSwdmxWmvInfo.fgAlreadySet = FALSE;
    _rSwdmxWmvInfo.fgEofParsed = FALSE;
    _rSwdmxWmvInfo.fgGotEofFromWMVMsgQ = FALSE;
    _rSwdmxWmvInfo.rTrickInterval.u4Seconds = 1;
    _rSwdmxWmvInfo.rTrickInterval.u4Micros = 0;
    _rFeederReqData.fgEof = FALSE;
    _rSwdmxWmvInfo.fgNeedSetFdrReq = TRUE;
    _rSwdmxWmvInfo.fgIs3DFile = FALSE;
    _rSwdmxWmvInfo.fgIsSetRangeInvoked = FALSE;
    _rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr = FALSE;
    _rSwdmxWmvInfo.u4FirstGoodIdx = 0;
    _rSwdmxWmvInfo.u4LastGoodIdx = 0;
    _rSwdmxWmvInfo.u4PrevAudPts = 0;

    /* for WMA FR/FF */
    _rSwdmxWmvInfo.u4AudioObjSz = 0;
    _rSwdmxWmvInfo.u4AvgPktCountPerEntry = 0;
    _rSwdmxWmvInfo.u4TotalPktCountPerEntry = 0;
    _rSwdmxWmvInfo.u4TrickModeParsingActionCount = 0;
    _rSwdmxWmvInfo.u8PreviousUIPos = 0;
    _rSwdmxWmvInfo.u2WmaPlCntPerPkt = 0;
    _rSwdmxWmvInfo.fgDeliverAEos = FALSE;
    _rSwdmxWmvInfo.fgDeliverVEos = FALSE;

    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    {
        UINT8 u1Idx = 0;
        for (u1Idx = 0; u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
        {
            _rSwdmxWmvInfo.u1AudPreBufPid[u1Idx] = 0xff;
        }
    }
    #endif

    if (_pu1VideoFrameData == NULL)
    {
#if 1
        _pu1VideoFrameData = (UINT8 *)BSP_AllocAlignedDmaMemory(1*1024,16);
#else /* need mentor's double check, follow David's mail: "Please review swdmx internal buffer use" */
		{
			UINT32 u4Addr = 0;
			UINT32 u4Size = 1024;
			UINT32 u4Align = 16;
			SWDMX_GetInfo(u1SrcId, eSWDMX_GET_INTERNAL_BUF, &u4Addr, &u4Size, &u4Align);
			_pu1VideoFrameData = (UCHAR*)u4Addr;
		}
#endif

        if (_pu1VideoFrameData == NULL)
        {
            ASSERT(0);
            return FALSE;
        }
    }
    
#ifdef __MODEL_slt__
    _u4VC1SltLastPicPts = 0;
#endif

    return TRUE;
}

VOID _SWDMX_WMVSetRange(UINT8 u1SrcId, SWDMX_RANGE_LIST_T *prRangeList, UINT32 u4VidStrmID,
                        UINT32 u4AudStrmID, UINT32 u4SubTitleID)
{
    UINT64 u8Remainder;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL(prSwdmxInst);  
    //UINT16 u2idx =0;
    //MIDXBULD_ELMT_STRM_ENTRY_T* prIdxEntry;

#if defined(CC_3D_MM_DS_SUPPORT)  // hard code to inverse L/R
    if (ENABLE_NEWTRACK(prSwdmxInst))
    {
        u4VidStrmID = 2;
    }
#endif
    
    LOG(5, "_SWDMX_WMV_SET_RANGE\n");

    if (prRangeList != NULL &&
            prRangeList->prDmxRangeInfo != NULL)
    {
        MM_RANGE_ELMT_IDXTBL_T* pBuldIdxTbl =
            (MM_RANGE_ELMT_IDXTBL_T*) prRangeList->prDmxRangeInfo->pt_idxtbl_lst;

        MM_RANGE_INFO_T* rRangeInfo = (prRangeList->prDmxRangeInfo);
        ASSERT(rRangeInfo != NULL);

        _prRangeInfo = rRangeInfo;
        //x_memcpy(&_rRangeInfo,prRangeList->prDmxRangeInfo,sizeof(MM_RANGE_INFO_T));
        //_prRangeInfo = &_rRangeInfo;
        _prRangeList = prRangeList;

        _rSwdmxWmvInfo.u4PreRoll = (UINT32)u8Div6432(prRangeList->prDmxRangeInfo->t_pts_offset, 90, &u8Remainder);
        _rSwdmxWmvInfo.u4FrameRate = 24;  // !!!1015

        /* set 3D flag */
        LOG(0, "_SWDMX_WMVSetRange 3D?%s\n", 
            prRangeList->prDmxRangeInfo->b_stereo_scopic?"yes":"no");
        _rSwdmxWmvInfo.fgIs3DFile = prRangeList->prDmxRangeInfo->b_stereo_scopic;
        _rSwdmxWmvInfo.fgIsSetRangeInvoked = TRUE;

#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        if (_prRangeInfo->ui4_1st_pkgnum)
        {
            _rSwdmxWmvInfo.u8BeginParseOfst = _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER +
                                       (UINT64)_prRangeInfo->ui4_sz_min_acces_unt * _prRangeInfo->ui4_1st_pkgnum;
            _rSwdmxWmvInfo.u8LastFileParsingPosition = _rSwdmxWmvInfo.u8BeginParseOfst;
        }
        else
#endif
        {
            _rSwdmxWmvInfo.u8BeginParseOfst = 0;
            _rSwdmxWmvInfo.u8LastFileParsingPosition = prRangeList->prDmxRangeInfo->ui8_fileoffset;
        }
        //_rSwdmxWmvInfo.u4PreRoll = (prRangeList->prDmxRangeInfo->t_pts_offset/90);
        _rSwdmxWmvInfo.u8FileSize = prRangeList->prDmxRangeInfo->z_range_sz;
        #ifdef TEST_WMV_DRM
        if (rRangeInfo->b_drm)
        {   
            UINT32 i = 0;
            _rSwdmxWmvInfo.eDRMType = DRM_KEY_AES;  //AES:1 RC4:2
              _rSwdmxWmvInfo.fgIsDRM = TRUE;           
              _rSwdmxWmvInfo.u4DRMKeySize = 16;           
              x_memcpy((VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],
                       (VOID*)&_au1ContentKey[0],
                       _rSwdmxWmvInfo.u4DRMKeySize);
              printf("[test]The DRM type is %d\n",_rSwdmxWmvInfo.eDRMType);
            printf("[test]The DRM contentkey size is %d. The key is: ",_rSwdmxWmvInfo.u4DRMKeySize);
            for (i=0;i<_rSwdmxWmvInfo.u4DRMKeySize;i++)
            {
                printf("%02X",_rSwdmxWmvInfo.u1DRMContentKey[i]);
            }
            printf("\n");
        }
        #else
         if (rRangeInfo->b_drm)
        {
            UINT32 i = 0;
            printf("This file type is DRM file\n");
            _rSwdmxWmvInfo.fgIsDRM = TRUE;
            x_memcpy((VOID*)&_rSwdmxWmvInfo.u1DRMContentKey[0],
                (VOID*)&rRangeInfo->t_drm_info_drv.u.t_playready_crypto_info.aui1_crypto_key_info[0],
                rRangeInfo->t_drm_info_drv.u.t_playready_crypto_info.ui4_crypto_key_size);
            _rSwdmxWmvInfo.u4DRMKeySize = rRangeInfo->t_drm_info_drv.u.t_playready_crypto_info.ui4_crypto_key_size;
            _rSwdmxWmvInfo.eDRMType = (DRM_KEY_TYPE)rRangeInfo->t_drm_info_drv.u.t_playready_crypto_info.e_crypto_key_type;
            printf("The DRM type is(SWDMX %d),(MW %d)\n",_rSwdmxWmvInfo.eDRMType
                ,(UINT32)rRangeInfo->t_drm_info_drv.u.t_playready_crypto_info.e_crypto_key_type);
            if(_rSwdmxWmvInfo.eDRMType == DRM_KEY_AES_PROTECTED)
            {
                _rSwdmxWmvInfo.eDRMType = DRM_KEY_AES;
            }
            printf("The DRM contentkey size is %d  The key is  :",_rSwdmxWmvInfo.u4DRMKeySize);
            for (i=0;i<_rSwdmxWmvInfo.u4DRMKeySize;i++)
            {
                printf("%02X",_rSwdmxWmvInfo.u1DRMContentKey[i]);
            }
            printf("\n");
        }
        #endif
//      if (_rSwdmxWmvInfo.fgIsDRM)
//      {
//          _SWDMX_WMVDRMInitDmx(prSwdmxInst);
//      }
        while (pBuldIdxTbl != NULL)
        {
            if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_KEY && pBuldIdxTbl->t_strm_id.u.t_stm_asf_id ==u4VidStrmID)
            {
#ifdef CC_3D_MM_DS_SUPPORT
                 if (_rSwdmxWmvInfo.fgIs3DFile)
                 {
                     if (IS_SIMP_IDX(pBuldIdxTbl->u.pt_pb_idx_tbl_entry))
                     {
                         if (_rSwdmxWmvInfo.prWmvKeyIdxTbl ==NULL)
                             _rSwdmxWmvInfo.prWmvKeyIdxTbl = pBuldIdxTbl;
                     }
                 }
                 else
#else
#endif
                 {
                     if (_rSwdmxWmvInfo.prWmvKeyIdxTbl ==NULL)
                         _rSwdmxWmvInfo.prWmvKeyIdxTbl = pBuldIdxTbl;
                 }
            }			
#ifdef CC_3D_MM_DS_SUPPORT
            if (pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_KEY && IS_NEWTRACKDATA(prSwdmxInst, pBuldIdxTbl->t_strm_id.u.t_stm_asf_id))
            {
                if (_rSwdmxWmvInfo.prWmvKeyIdxTbl2 == NULL)
                    _rSwdmxWmvInfo.prWmvKeyIdxTbl2 = pBuldIdxTbl;
            }
#endif
            pBuldIdxTbl = (MM_RANGE_ELMT_IDXTBL_T*)pBuldIdxTbl->pv_next_tbl;
        }
#ifdef WMA_TRICK_MODE
        pBuldIdxTbl=prRangeList->prDmxRangeInfo->pt_idxtbl_lst;
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES&&pBuldIdxTbl&&(!_rSwdmxWmvInfo.prWmvKeyIdxTbl))
        {
            if(pBuldIdxTbl->e_elem_type == MM_ELEMT_TYPE_KEY&&(pBuldIdxTbl->t_strm_id.u.t_stm_asf_id==0 || pBuldIdxTbl->t_strm_id.u.t_stm_asf_id==u4AudStrmID))
            {
               _rSwdmxWmvInfo.prWmvKeyIdxTbl = pBuldIdxTbl;
            }
        }
#endif

        {//find the first and last good index    
            UINT32 i = 0;
            UINT32 u4NumberOfEntry = 0;
            UINT64 u8EntryAbsOfst = 0; 
            UINT32 u4EntryPts = 0; 
            UINT32 u4EntrySize = 0; 

            if (_rSwdmxWmvInfo.prWmvKeyIdxTbl != NULL)
            {
                u4NumberOfEntry = _rSwdmxWmvInfo.prWmvKeyIdxTbl->ui4_number_of_entry;
                //todo: for many entries case, how to seach entry quickly?
                for (i = 0; i < u4NumberOfEntry; i++)
                {
                    GET_ENTRY_INFO_VOID(prSwdmxInst, 
                                        i,
                                        u8EntryAbsOfst,
                                        u4EntryPts, 
                                        u4EntrySize);
                    if (u4EntrySize == 0 || u4EntrySize > _rSwdmxWmvInfo.u8FileSize)
                    {
                        // bad index
                        #ifdef WMV_DEBUG_SHOW_INDEX_TABLE_ENTRY
                        LOG(0, "BAD#%d pts:0x%x sz:0x%x AbsOfst:0x%llx\n",
                            i, u4EntryPts, u4EntrySize, u8EntryAbsOfst);
                        #endif
                    }
                    else
                    {
                        #ifdef WMV_DEBUG_SHOW_INDEX_TABLE_ENTRY
                        LOG(0, "#%d pts:0x%x sz:0x%x AbsOfst:0x%llx\n",
                            i, u4EntryPts, u4EntrySize, u8EntryAbsOfst);
                        #endif
                        _rSwdmxWmvInfo.u4FirstGoodIdx = min(_rSwdmxWmvInfo.u4FirstGoodIdx, i);
                        _rSwdmxWmvInfo.u4LastGoodIdx = max(_rSwdmxWmvInfo.u4LastGoodIdx, i);
                    }        
                }
                LOG(0, "Good index(#%d, #%d)\n", 
                    _rSwdmxWmvInfo.u4FirstGoodIdx, 
                    _rSwdmxWmvInfo.u4LastGoodIdx);
            }
        }
        
        LOG(0,"pts offset: %d\n",_rSwdmxWmvInfo.u4PreRoll);

        prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
    }
    else
    {
        LOG(0, "Demuxer Range is NULL\n");
    }
}


#ifdef WMV_BAD_INTLV_DEBUG
BOOL _fgShowFifoStatus = 1;
UINT32 _u4LastVFullness = 0;
UINT32 _u4LastAFullness = 0;
UINT32 _u4Last1_10Sec = 0;
void vShowFifoStatus(SWDMX_INFO_T* prSwdmxInst)
{
    UINT32 u4AFullness, u4VFullness;
    UINT32 u4ADuration, u4VDuration;
    UINT16 u2CurQNum, u2MaxQNum;
    UINT32 u41_10Sec;
    
    u4VFullness = _u4VFifoSize - DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_VIDEO, 0);
    u4AFullness = _u4AFifoSize - DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, DMX_PID_TYPE_ES_AUDIO, 1);
    u41_10Sec = STC_GetStcValue(prSwdmxInst->u1StcId) / 9000;
    
    if (_fgShowFifoStatus &&
        /*(u4VFullness != _u4LastVFullness || u4AFullness != _u4LastAFullness)*/
        u41_10Sec != _u4Last1_10Sec)
    {
        VDEC_GetQueueInfo(prSwdmxInst->u1VdecId, &u2CurQNum, &u2MaxQNum);
        u4VDuration = 1000 * u2CurQNum / 25;
        if (_u4WmaBytesPerSec > 0)
            u4ADuration = 1000 * u4AFullness / _u4WmaBytesPerSec;
        else
            u4ADuration = 0;
        printf("[swmv] %3ds A:V = %d : %d -%df (%d ms : %d ms)\n", u41_10Sec, u4AFullness, u4VFullness, u2CurQNum, u4ADuration, u4VDuration);
    }
    _u4LastVFullness = u4VFullness;
    _u4LastAFullness = u4AFullness;
    _u4Last1_10Sec = u41_10Sec;
}
#endif  // WMV_BAD_INTLV_DEBUG


#ifdef CC_3D_MM_DS_SUPPORT
static VOID _WMVChkResume1XPos(SWDMX_INFO_T* prSwdmxInst)
{
    UINT32 i = 0;
    UINT32 u4NumberOfEntry = 0;
    //MIDXBULD_ELMT_STRM_ENTRY_T *prKeyidxEntry = NULL;
    MIDXBULD_ELMT_STRM_ENTRY_T *prKeyidxEntry2 = NULL;
    UINT64 u8EntryAbsOfst = 0;
    UINT32 u4EntryPts = 0;
    UINT32 u4EntrySize = 0;
//  UINT64 u8EntryAbsOfst2 = 0;
//  UINT32 u4EntryPts2 = 0;
//  UINT32 u4EntrySize2 = 0;
    UINT64 u8Remainder = 0;
    UINT32 u4ParsingPktNo = 0;
    BOOL fgFound = FALSE;
    
    if (_rSwdmxWmvInfo.prWmvKeyIdxTbl != NULL)
    {
        u4NumberOfEntry = _rSwdmxWmvInfo.prWmvKeyIdxTbl->ui4_number_of_entry;
    }
    else
    {
        LOG(0, "No key table for L track!!!\n");
        ASSERT(0);
        return;
    }

    if (_rSwdmxWmvInfo.prWmvKeyIdxTbl2 == NULL)
    {
        LOG(0, "No key table for R track!!!\n");
        ASSERT(0);
        return;
    }

#if 0
    i = _rSwdmxWmvInfo.u4CurKeyIdx;
    //if (i >= 1)
    //    i--;
#else
    i = 0;
#endif
    for ( ; i < u4NumberOfEntry; i++)
    {
        GET_ENTRY_INFO_VOID(prSwdmxInst, 
                            i,
                            u8EntryAbsOfst,
                            u4EntryPts, 
                            u4EntrySize);
        u4ParsingPktNo = (UINT32)u8Div6432(u8EntryAbsOfst,
                                           _rSwdmxWmvInfo.u4PacketSize, 
                                           &u8Remainder);
        if (u4ParsingPktNo == _rSwdmxWmvInfo.u4CurParsingPacketNo)
        {
            fgFound = TRUE;
            break;
        }
    }
    
    /* It's NOT ensure the prKeyidxEntry2 is designed for >4G */
    if (_rSwdmxWmvInfo.prWmvKeyIdxTbl->pt_next_partial == NULL)
    {
        /* filesize < 4G */
        if (fgFound)
        {
            LOG(3, "!!!!!!!!!!!!!!!!!!!!!!!!!!! Old resume pkt #: %d\n", _rSwdmxWmvInfo.u4CurParsingPacketNo);
            prKeyidxEntry2 = (MIDXBULD_ELMT_STRM_ENTRY_T*)(_rSwdmxWmvInfo.prWmvKeyIdxTbl2->u.pt_pb_idx_tbl_entry + i);
            _rSwdmxWmvInfo.u4CurParsingPacketNo = min(u4ParsingPktNo, 
                                                      prKeyidxEntry2->ui4_relative_offset / _rSwdmxWmvInfo.u4PacketSize);
            LOG(3, "!!!!!!!!!!!!!!!!!!!!!!!!!!! New resume pkt #: %d\n", _rSwdmxWmvInfo.u4CurParsingPacketNo);
        }
    }
    else
    {
        LOG(0, "3D file size >4G, try to check packet number by prWmvKeyIdxTbl2\n");
        if (fgFound)
        {
            //todo: get entry by index/i from table2, 
            //      then compare packet number with previous one

            //currently, use previous one directly
            _rSwdmxWmvInfo.u4CurParsingPacketNo = u4ParsingPktNo;
        }
    }
    UNUSED(u8Remainder);
}
#endif  // CC_3D_MM_DS_SUPPORT

static VOID _SWDMX_WMVHandleEos(SWDMX_INFO_T* prSwdmxInst, 
                                VDEC_ES_INFO_T *prVdecEsInfo, 
                                SWDMX_RANGE_LIST_T* prRangeList,
                                UINT32 i4PlaySpeed)
{
    if (!((_rSwdmxWmvInfo.i4WMVPlaySpeed==1000 && (i4PlaySpeed==2000 || i4PlaySpeed==1 ||i4PlaySpeed == 500))
        ||(_rSwdmxWmvInfo.i4WMVPlaySpeed > 1 && _rSwdmxWmvInfo.i4WMVPlaySpeed < 1000 && i4PlaySpeed >1 && i4PlaySpeed < 1000)))
    {
        
        if (_rSwdmxWmvInfo.fgDemuxFinish && _rSwdmxWmvInfo.u4PacketSize != 0)
        {
            if (prVdecEsInfo != NULL)
            {
                SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
            }
            AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
            LOG(0, "Clear Eos Flag\n");
            if (_SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1VdecId,TRUE, FALSE) == FALSE)
            {
                LOG(0,"set dmx error when sending EOS flag\n");
            }
            
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                if (_SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1Vdec2Id,TRUE, FALSE) == FALSE)
                {
                    LOG(0,"set dmx error when sending EOS flag\n");
                }
            }
            
            _u4PrevFramePTS = 0;//DTV00085165
            _rSwdmxWmvInfo.fgDemuxFinish = FALSE;
        }
        if (prRangeList->eRangeStats == eSWDMX_RANGE_STATE_PENDING && _rSwdmxWmvInfo.u4PacketSize != 0)
        {
            prRangeList->eRangeStats = eSWDMX_RANGE_STATE_NOT_FINISHED;
        }
    }
}

static VOID _SWDMX_WMVInitVDmx(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL(prSwdmxInst);

    if (_rSwdmxWmvInfo.fgEnVideo)  // megaa: remove the unnecessary "_rSwdmxWmvInfo.i4WMVPlaySpeed != 1000" condition for DTV00095857
    {
        //LOG(0, "WMV init video DMX! stack:\n");
        //dump_stack();
        if (_SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1VdecId,TRUE, FALSE) == FALSE)
        {
            LOG(0,"_SWDMX_WMVInitDmx(video#%d) fail\n", prSwdmxInst->u1VdecId);
        }

        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
            if (_SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1Vdec2Id,TRUE, FALSE) == FALSE)
            {
                LOG(0,"_SWDMX_WMVInitDmx(video2#%d) fail\n", prSwdmxInst->u1Vdec2Id);
            }
        }
    }
}

static VOID _SWDMX_WMVTellVDecRenderPts(UINT8 u1SrcId, INT32 i4PlaySpeed, UINT64 u8SeekPTS)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL(prSwdmxInst);
    if ((i4PlaySpeed >0)&&(_rSwdmxWmvInfo.fgEnVideo))
    {
        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,(UINT32)(u8SeekPTS+3000)); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
    //              VDEC_SetRenderPts(prSwdmxInst->u1VdecId,(UINT32)(_rSwdmxWmvInfo.u8SeekPTS+3000)); //maiyou //original, 2011/03/13
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
            VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id,(UINT32)(u8SeekPTS+3000)); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
    //                    VDEC_SetRenderPts(prSwdmxInst->u1Vdec2Id,(UINT32)(_rSwdmxWmvInfo.u8SeekPTS+3000)); //original, 2011/03/13
        }
    }
    else if (_rSwdmxWmvInfo.fgEnVideo)
    {
        VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,0); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
    //                VDEC_SetRenderPts(prSwdmxInst->u1VdecId,0); //original, 2011/03/13
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
            VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id,0); //after, 2011/03/13, from isaac's mail:2011/3/2 pm11:05
    //                    VDEC_SetRenderPts(prSwdmxInst->u1Vdec2Id,0); //original, 2011/03/13
        }
    }
}

static VOID _SWDMX_WMVFlushAVFifo(UINT8 u1SrcId, UINT64 u8SeekPTS)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId); 
    
    SWDMX_WMV_VERIFY_NULL(prSwdmxInst);
    if (_rSwdmxWmvInfo.fgEnVideo)
    {
        MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
        VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid);
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
            MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, TRUE);
            VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id);
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid);
        }
    }
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
        AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
        u4AudDspPts[u1SrcId] = u8SeekPTS;
    }
    else if(_rSwdmxWmvInfo.fgEnAudio)
    {
       DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
       _rSwdmxWmvInfo.u4PrevAudPts = 0;
    }
    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    {
        UINT8 u1Idx = 0;
        /* for DTV00459047 
         * when do "AB repeat", flow is "disable audio" -> "pause" ->
         * "seek to A" -> "enable audio" -> "restore speed" -> "play from A".
         * Hence, we should flush audio prebuffer before "seek to A"
        */
        for (u1Idx = 0; u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
        {
            DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
        }
    }
    #endif
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.fgDemuxFinish = FALSE;
    _rSwdmxWmvInfo.u4ConsumedSize = 0;
    _rSwdmxWmvInfo.fgBrokenFile = FALSE;
    _rSwdmxWmvInfo.fgAlreadySet = FALSE;
    _rSwdmxWmvInfo.fgGotEofFromWMVMsgQ = FALSE;
    _rSwdmxWmvInfo.fgNeedSetFdrReq = TRUE;
    _rSwdmxWmvInfo.fgEofParsed = FALSE;

//  LOG(0, "currently5 _fgReceiveVideoPes is %s, set to FALSE!\n",
//      _fgReceiveVideoPes?"TRUE":"FALSE");
    //////_fgReceiveVideoPes = FALSE;//could not set to FALSE here
    _fgReceiveAudioPes = FALSE;
}

static VOID _SWDMX_WMVChangeAudioTrack(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    UINT32 u4CurPts = 0;

    SWDMX_WMV_VERIFY_NULL(prSwdmxInst);
    u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
    _rSwdmxWmvInfo.fgChangeAudTrack = FALSE;
    _rSwdmxWmvInfo.u4AudioCount = 0;
    _rSwdmxWmvInfo.u4WmaErrorCode = 0;
    _rSwdmxWmvInfo.fgFirstAudioError = FALSE;
    _fgReceiveAudioPes = FALSE;
    _rSwdmxWmvInfo.fgAudIsReady = FALSE;
    _rSwdmxWmvInfo.u4TriggerAudioPTS = u4CurPts + 27000;//0.3 second
    DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPid);

#ifdef WMV_AUDIO_USE_PRE_BUFFER
    {
        UINT32 u4APts = 0;
        BOOL fgRet = FALSE;
        UINT8 u1AuPid = _rSwdmxWmvInfo.u1AudPreBufPid[_rSwdmxWmvInfo.u4AStreamId];

        fgRet = DMX_AudHandler_SeekAndUpdate(u1AuPid,
                                             _rSwdmxWmvInfo.u4TriggerAudioPTS, 
                                             &u4APts);
        if (fgRet)
        {
            LOG(5, "Find audio in the pre-buffer %d trigPts = %d \n",u4APts,_rSwdmxWmvInfo.u4TriggerAudioPTS);
            _rSwdmxWmvInfo.u4TriggerAudioPTS = u4APts;
        }
        else
        {
            LOG(5, "Can't find audio in the pre-buffer pts = %d \n",_rSwdmxWmvInfo.u4TriggerAudioPTS);
        }

        AUD_SetStartPts(prSwdmxInst->u1AdecId, _rSwdmxWmvInfo.u4TriggerAudioPTS);
        DMX_AudHandler_SetActivePidx(u1AuPid);  // !!!
    }
#else
    AUD_SetStartPts(prSwdmxInst->u1AdecId, _rSwdmxWmvInfo.u4TriggerAudioPTS);
#endif
    _rSwdmxWmvInfo.fgAlreadySet = TRUE;    
    LOG(5, "_rSwdmxWmvInfo.u4TriggerAudioPTS = 0x%08x\n", _rSwdmxWmvInfo.u4TriggerAudioPTS);
}

SWDMX_HDR_PRS_STATE_T _SWDMX_WMVHeaderParse(
    UINT8 u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    SWDMX_CUR_CMD_T* prCmd)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    UINT64 u8SeekPos = prCmd->u8SeekPos;
    INT32 i4PlaySpeed = prCmd->i4PlaySpeed;
    SWDMX_PLAY_MODE_T ePlayMode=prCmd->ePlayMode;
    BOOL fgSpeedChange = prCmd->fgChange;
    UINT32 _u4CmdSeekMode = prCmd->u4SeekMode; 
    UINT64 u8SeekTime = prCmd->u8SeekTime;  
    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    UINT8 u1Idx = 0;
    #endif 
    
    LOG(9, "_SWDMX_WMV_HEADER_PARSE\n");

#ifdef WMV_BAD_INTLV_DEBUG
    vShowFifoStatus(prSwdmxInst);
#endif

    if (!prSwdmxInst)
    {
        return eSWDMX_HDR_UNKNOWN_TYPE;
    }

    // megaa 20100321 tmp solution
    if (!_rSwdmxWmvInfo.fgInternalInited)
    {
        _WMVMsgQInit(u1SrcId);
        _WMVInternalInit(u1SrcId);
    }

    /* check if SetRange was invoked */
    if (!_rSwdmxWmvInfo.fgIsSetRangeInvoked)
    {
        LOG(0, "headerParse is return for SetRange was NOT invoked!\n");
        return eSWDMX_HDR_PRS_SUCCEED;
    }

    /* comes from EnableStrm() to ensure the instance get correct
       RangeInfo(3D info, etc) */
    if (_rSwdmxWmvInfo.fgEnVideo)
    {
        if (!_rSwdmxWmvInfo.fgEnVDmx)
        {
            _SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1VdecId,TRUE,FALSE);
            _rSwdmxWmvInfo.fgEnVDmx = TRUE;
        }
        if (!_rSwdmxWmvInfo.fgEnV2Dmx)
        {
            _SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_VIDEO,prSwdmxInst->u1Vdec2Id,TRUE,FALSE);
            _rSwdmxWmvInfo.fgEnV2Dmx = TRUE;
        }
    }
    if (_rSwdmxWmvInfo.fgEnAudio && !_rSwdmxWmvInfo.fgEnADmx && !_rSwdmxWmvInfo.fgBeginPlay)
    {
        _SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_AUDIO,prSwdmxInst->u1SwdmxId,TRUE,FALSE);
        _rSwdmxWmvInfo.fgEnADmx = TRUE;
        _SWDMX_WMVChangeAudioTrack(u1SrcId);
        LOG(0,"init dmx\n");
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(prSwdmxInst->u1VdecId);
    _rSwdmxWmvInfo.fgPlayAbort=FALSE;
    
    if (_rSwdmxWmvInfo.fgBeginPlay && i4PlaySpeed == 1000)
    {
        if(u1SrcId<SWDMX_SOURCE_MAX)
        {
            u4AudDspPts[u1SrcId]=1;
        }
        //VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, 0);
        if (_rSwdmxWmvInfo.fgEnAudio && !_rSwdmxWmvInfo.fgEnADmx)
        {
            _rSwdmxWmvInfo.fgEnADmx = TRUE;
            _SWDMX_WMVInitDmx(prSwdmxInst, eSWDMX_MEDIA_AUDIO,prSwdmxInst->u1SwdmxId,TRUE,FALSE);
            LOG(0,"begin play init aud dmx\n");
        }
        if (_rSwdmxWmvInfo.fgEnAudio)//protect func
        {
            AUD_SetStartPts(prSwdmxInst->u1AdecId,0);
            #ifdef WMV_AUDIO_USE_PRE_BUFFER
            DMX_AudHandler_SetActivePidx(_rSwdmxWmvInfo.u1AudPreBufPid[_rSwdmxWmvInfo.u4AStreamId]);
            #else
            //need to setActive? todo-audioprebuffer
            #endif
            _rSwdmxWmvInfo.fgAlreadySet = TRUE;
        }

        if (_rSwdmxWmvInfo.fgEnVideo)//protect func
        {
            VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,0); //maiyou
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id,0); 
            }
        }

        if (_rSwdmxWmvInfo.fgEnVideo  // make sure that video is prior than audio
            && _rSwdmxWmvInfo.eVideoType != ENUM_SWDMX_VDEC_UNKNOWN)  // DTV00079781
        {
            _rSwdmxWmvInfo.u4TriggerAudioPTS = 0xF0000000;
            _rSwdmxWmvInfo.u4DisplayPTS = 0xF0000000;
        }
        else
        {
            _rSwdmxWmvInfo.u4TriggerAudioPTS = 0x00000000;
            _rSwdmxWmvInfo.u4DisplayPTS = 0x00000000;
        }

#ifdef WMV_BAD_INTLV_SUPPORT
        _rBadIntlv.fgEverEntered = FALSE;
#endif

        _rSwdmxWmvInfo.fgBeginPlay = FALSE;

        if (_rSwdmxWmvInfo.fgEnAudio)//protect flag
        {
            _rSwdmxWmvInfo.fgAudioNormalPlay = TRUE;
        }
        if (_rSwdmxWmvInfo.fgEnVideo)//protect flag
        {
            _rSwdmxWmvInfo.fgVideoNormalPlay = TRUE;
        }
        //_fgGotVideoFrame = FALSE;
        //_fgVideoPause = FALSE;
#ifdef WAIT_AUDIO_READY
        if (!_rSwdmxWmvInfo.fgVideoPause && _rSwdmxWmvInfo.fgEnVideo && (
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
            prSwdmxInst->fgIsBadInterleaved ||
#endif
            _rSwdmxWmvInfo.fgEnAudio))
        {
            VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
            _SWDMX_WMVWaitVDec(1);
            VDEC_Pause(prSwdmxInst->u1VdecId);//maiyou
            if(ENABLE_NEWTRACK(prSwdmxInst))
            {
                VDEC_Pause(prSwdmxInst->u1Vdec2Id);
            }
            _rSwdmxWmvInfo.fgVideoPause = TRUE;
        }
#endif
#ifdef WMA_SEEK_OFFSET
        _rSwdmxWmvInfo.fgSeekingNow=FALSE;
        _rSwdmxWmvInfo.u4WMASeekTimes=0;
        _rSwdmxWmvInfo.u8LastSeekPos=0;
#endif
        STC_StopStc(prSwdmxInst->u1StcId);
        if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
            //UINT32 u4CurPts=0;
            STC_SetStcValue(prSwdmxInst->u1StcId, (UINT32)(0));
            //AUD_MMQueueSyncInfo(prSwdmxInst->u1AdecId,0,0,TRUE);
            AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
            //AUD_GetAudioTrickPts(255,&u4CurPts,TRUE);
        }
        else
        {
            STC_SetStcValue(prSwdmxInst->u1StcId, (UINT32)(0-3000));
        }
		
        //
        if (prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK)
        {   
            //LOG(1, "time based solution!\n");
            LOG(0, "check1: %s, %d -> %d\n", 
                prCmd->fgChange ? "Change" : "-",
                _rSwdmxWmvInfo.i4WMVPlaySpeed, 
                i4PlaySpeed);
            _rSwdmxWmvInfo.rTrickInterval.u4Seconds = 0;
            _rSwdmxWmvInfo.rTrickInterval.u4Micros = 500000;//the smaller value, the more IFrames

//          if (_u4CmdSeekMode == SWDMX_CUR_CMD_SEEK_TIME)
//          {
//              LOG(1, "time_seek, CMD_SEEK_TIME!\n");
//              fgSpeedChange = TRUE;
//              _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
//              u8SeekTime = prCmd->u8SeekTime;
//              prSwdmxInst->rCurRangeReq.u8StartAudPts = prCmd->u8SeekTime;
//          }
        }
        else
        {
            if(prSwdmxInst->rCurRangeReq.fgSetRenderPosition == TRUE 
               ||_prRangeInfo->t_start_pos.ui8_pts_info>90000)
            {
                if(_prRangeInfo->t_start_pos.ui8_pts_info>90000)
                {
                    if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
                    {
                        UINT64 u8Remainder=0;
                        fgSpeedChange = TRUE;
                        u8SeekPos = _prRangeInfo->t_start_pos.ui8_pts_info;
                        _prRangeInfo->t_vid_start_render_pts = 0;
                        _rSwdmxWmvInfo.fgFirstFrame = FALSE;
                        _rSwdmxWmvInfo.u4TotalPacketNo = (UINT32)u8Div6432((_rSwdmxWmvInfo.u8FileSize-WMV_DATA_OBJ_HEADER),
                           _prRangeInfo->ui4_sz_min_acces_unt, &u8Remainder);
                        if (_rSwdmxWmvInfo.u4TotalPacketNo == 0)  
                        {
                            _rSwdmxWmvInfo.u4TotalPacketNo = 1; 
                        }
                        _rSwdmxWmvInfo.u4PacketSize = _prRangeInfo->ui4_sz_min_acces_unt;
                    }
                    else
                    {
                        /* LOG(0,"No index table,can't start from 0x%x\n",_prRangeInfo->t_vid_start_render_pts); */
                        LOG(0,"No index table, use _WMVSeekOffset now, render from %d.%05d\n",
                            (UINT32)_prRangeInfo->t_start_pos.ui8_pts_info / 90000,
                            (UINT32)_prRangeInfo->t_start_pos.ui8_pts_info % 90000);
                        fgSpeedChange = TRUE;//DTV00457472
                        _SWDMX_WMVSeekOffset(prSwdmxInst, FALSE);
                    }
                }
                else
                {
                    fgSpeedChange = TRUE;
                    _rSwdmxWmvInfo.fgSeekingNow=TRUE;
                    u8SeekPos=prSwdmxInst->rCurRangeReq.u8StartPts;
                    prSwdmxInst->rCurRangeReq.u8StartAudPts=u8SeekPos;
                }
            }
        }
    }

    if (i4PlaySpeed == 1000 && (prSwdmxInst->ePrePlayMode==SWDMX_PLAY_MODE_IFRAME))
    {
        _rSwdmxWmvInfo.fgAudioNormalPlay = TRUE;
        //_rSwdmxWmvInfo.fgVideoNormalPlay = TRUE;
        /*for (u1Idx = 0;u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
        {
            DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
        }*/
    }
    if (i4PlaySpeed == 1000&& (_rSwdmxWmvInfo.i4WMVPlaySpeed != 1000 && prSwdmxInst->ePrePlayMode==SWDMX_PLAY_MODE_NORMAL))
    {
        _rSwdmxWmvInfo.fgAudioNormalPlay = TRUE;
        //_rSwdmxWmvInfo.fgVideoNormalPlay = TRUE;
    }

#if 0  // obsolete code (stop resume is now implemented by pause/resume)
    // stop resume handling
    if (prVdecEsInfo->fgFmtDectFinish&& _prRangeInfo->t_vid_start_render_pts >= 90000)
    {
        if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
        {
            fgSpeedChange = TRUE;
            u8SeekPos = _prRangeInfo->t_vid_start_render_pts;
            _prRangeInfo->t_vid_start_render_pts = 0;
        }
        else
        {
           LOG(0,"No index table,can't start from 0x%x\n",_prRangeInfo->t_vid_start_render_pts);
        }
    }
#else
    UNUSED(prVdecEsInfo);
#endif

    if ((prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK)
        //_u4CmdSeekMode == SWDMX_CUR_CMD_SEEK_TIME
        //||(fgSpeedChange)
        )
    {
/*
        LOG(0, "check2: %s, %d -> %d\n",     
            prCmd->fgChange ? "Change" : "-",
            _rSwdmxWmvInfo.i4WMVPlaySpeed,   
            i4PlaySpeed);                    
*/
        if (_u4CmdSeekMode == SWDMX_CUR_CMD_SEEK_TIME)
        {
            LOG(1, "time_seek, CMD_SEEK_TIME!\n");
            //fgSpeedChange = TRUE;
            _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
            u8SeekTime = prCmd->u8SeekTime;
            prSwdmxInst->rCurRangeReq.u8StartAudPts = prCmd->u8SeekTime;
        }

        if ((((UINT32)(u8SeekTime>>32) != (UINT32)0xFFFFFFFF)
                || ((UINT32)(u8SeekTime) != (UINT32)0xFFFFFFFF))
            && (prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_TIME))
        {
            _rSwdmxWmvInfo.fgTimebasedNeedSeek = TRUE;
            _rSwdmxWmvInfo.u8TimebasedTimeToBeSeekedTo = prCmd->u8SeekTime;
            LOG(0, "connection stalling support? %s\n",
                prSwdmxInst->fgSupportConnStalling ? "yes" : "no");
            _SWDMX_WMVSeekTimebased(u1SrcId, prRangeList, prCmd);
            if(_rSwdmxWmvInfo.fgEnAudio)
            {
                #ifdef CC_AUD_SUPPORT_DUAL_DSP
                AUD_DSPCmdStop(AUD_DSP0, prSwdmxInst->u1AdecId);
                AUD_DSPCmdPlay(AUD_DSP0, prSwdmxInst->u1AdecId);
                #else
                AUD_DSPCmdStop(prSwdmxInst->u1AdecId);
                AUD_DSPCmdPlay(prSwdmxInst->u1AdecId);
                #endif
            }
            //_rSwdmxWmvInfo.u8SeekPTS = 0xFFFFFFFF;
            //_rSwdmxWmvInfo.u8SeekPTS <<= 32;
            //_rSwdmxWmvInfo.u8SeekPTS += 0xFFFFFFFF;
        }
        else if (fgSpeedChange 
                 && _rSwdmxWmvInfo.i4WMVPlaySpeed != i4PlaySpeed)
        {
            _SWDMX_WMVHandleSpeedChangeTimebased(u1SrcId, prRangeList, prCmd);
            _SWDMX_WMVHandleEos(prSwdmxInst, prVdecEsInfo, prRangeList, i4PlaySpeed);
        }
        goto _header_parse_tail;
    } /* end of (prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK) */

    //handle speed change
    if (   fgSpeedChange
        || _rSwdmxWmvInfo.fgChangeAudTrack
#ifdef WMA_SEEK_OFFSET
        || _rSwdmxWmvInfo.fgSeekingNow
#endif
        )
    {
        UINT16 u2DecodingOrder;
        INT32 i4TempRef;
        UINT64 u8FrameOffset =0;
        UINT64 u8IFrameOffset =0;
        VDEC_ES_INFO_T *prVdecEsInfo;
        
        LOG(0, "***** speed %d -> %d *****\n",
            _rSwdmxWmvInfo.i4WMVPlaySpeed,
            i4PlaySpeed);
        //LOG(0, "******************i4PlaySpeed = %d\n",i4PlaySpeed);
        
        prVdecEsInfo = _VDEC_GetEsInfo((UCHAR)prSwdmxInst->u1VdecId); //maiyou

        if (_rSwdmxWmvInfo.fgEnVideo)//protect func
        {
            VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder,
                                &u8FrameOffset, &u8IFrameOffset);
        }
        _rSwdmxWmvInfo.fgFinishKeyTable = FALSE;

#ifdef WMA_SEEK_OFFSET
        if(fgSpeedChange || _rSwdmxWmvInfo.fgChangeAudTrack)
        {
            _rSwdmxWmvInfo.fgSeekingNow=FALSE;
            _rSwdmxWmvInfo.u4WMASeekTimes=0;
            _rSwdmxWmvInfo.u8LastSeekPos=0;
        }
#endif
        if (_rSwdmxWmvInfo.fgChangeAudTrack)
        {
            _SWDMX_WMVChangeAudioTrack(u1SrcId);
        }
        

        if (((UINT32)(u8SeekPos>>32) != (UINT32)0xFFFFFFFF)
                || ((UINT32)(u8SeekPos) != (UINT32)0xFFFFFFFF)
#ifdef WMV_SEEK_OFFSET
            ||(prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET)
#endif
#ifdef WMA_SEEK_OFFSET
            ||(_rSwdmxWmvInfo.fgSeekingNow)
#endif
        )
        {
            #ifdef WMV_SEEK_OFFSET
            if(prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET)
            {
                if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
                {
                    u8SeekPos = prSwdmxInst->rSeekOffset.ui8_aud_pts_info;
                }
                else
                {
                    u8SeekPos = prSwdmxInst->rSeekOffset.ui8_pts_info;
                }
            }
            #endif
#ifdef WMA_SEEK_OFFSET
            if(!_rSwdmxWmvInfo.fgSeekingNow)
            {
                _rSwdmxWmvInfo.u8SeekPTS = u8SeekPos;
                _rSwdmxWmvInfo.u4DisplayPTS = (UINT32)u8SeekPos;
                if(i4PlaySpeed==1000)
                {
                	_rSwdmxWmvInfo.fgAudioNormalPlay = TRUE;
                }
                _rSwdmxWmvInfo.u2SendPayloadNo = 0;
                _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
                _rSwdmxWmvInfo.fgBlock = FALSE;
            }
#else
            _rSwdmxWmvInfo.u8SeekPTS = u8SeekPos;
            _rSwdmxWmvInfo.u4DisplayPTS = (UINT32)u8SeekPos;
            if(_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES || i4PlaySpeed>0)
            {
            	_rSwdmxWmvInfo.fgAudioNormalPlay = TRUE;
            }
            _rSwdmxWmvInfo.u2SendPayloadNo = 0;
            _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
            _rSwdmxWmvInfo.fgBlock = FALSE;
#endif
            _SWDMX_WMVInitVDmx(u1SrcId);
            
			
            if (_rSwdmxWmvInfo.fgEnVideo)//protect func
            {
                VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder,
                                    &u8FrameOffset, &u8IFrameOffset);
            }
            
            if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
#ifdef WMA_SEEK_OFFSET
                if(_rSwdmxWmvInfo.u4WMASeekTimes<WMA_MAX_SEEKTIME)
                {
                   if(_SWDMX_WMASeekOffset(prSwdmxInst)<0)
                   {
                     return eSWDMX_HDR_PRS_SUCCEED;
                   }
                }
                else
                {
                   _rSwdmxWmvInfo.fgSeekingNow=FALSE;
                   return eSWDMX_HDR_PRS_SUCCEED;
                }
#else
                _SWDMX_WMVConsumeFeeder(prSwdmxInst);
                if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
                {
                    _SWDMX_WMVSeek(prSwdmxInst, u8SeekPos, prRangeList);
                     if(i4PlaySpeed<0)
                     {
                    	 fgTrickTrigger[u1SrcId]=TRUE;
                     }
                }
                else
                {
                	_rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER;
                	_rSwdmxWmvInfo.u4CurParsingPacketNo = 0;
                }
#endif
            }
#ifdef WMV_SEEK_OFFSET
            else if((prCmd->u4SeekMode == SWDMX_CUR_CMD_SEEK_OFFSET) ||
                ((_prRangeList->fgSetRenderPosition)
                 &&(_prRangeList->prDmxRangeInfo->t_vid_start_render_pts != 0)
                 &&(_prRangeList->prDmxRangeInfo->t_aud_start_render_pts != 0)))
            {
                _SWDMX_WMVSeekOffset(prSwdmxInst,FALSE);
                VDP_SetSeek(prSwdmxInst->u1B2rId,1,u8SeekPos);
                LOG(0,"VDP seek to pts %d\n",u8SeekPos);
            }
#endif
            else
            {
                if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
                {
                    _SWDMX_WMVSeek(prSwdmxInst, u8SeekPos, prRangeList);
                }
                else
                {
                    _SWDMX_WMVSeekOffset(prSwdmxInst,TRUE);
                }
            }
            //to be done , speed up searching time

            _SWDMX_WMVTellVDecRenderPts(u1SrcId, i4PlaySpeed, _rSwdmxWmvInfo.u8SeekPTS);
            _SWDMX_WMVFlushAVFifo(u1SrcId, _rSwdmxWmvInfo.u8SeekPTS);
            _fgReceiveVideoPes = FALSE; //DTV00579308

#ifdef WMV_BAD_INTLV_SUPPORT
            _WMVResetBadIntlv(prSwdmxInst);
#endif

            if (_rSwdmxWmvInfo.fgEnVideo)//protect func
            {
                VDEC_StartPTS(prSwdmxInst->u1VdecId,TRUE,(UINT32)_rSwdmxWmvInfo.u8SeekPTS,NULL);
                if(ENABLE_NEWTRACK(prSwdmxInst))
                {
                    VDEC_StartPTS(prSwdmxInst->u1Vdec2Id,TRUE,(UINT32)_rSwdmxWmvInfo.u8SeekPTS,NULL);
                }
            }

#ifdef WAIT_AUDIO_READY
            if (!_rSwdmxWmvInfo.fgVideoPause && i4PlaySpeed !=1 && _rSwdmxWmvInfo.fgEnVideo && _rSwdmxWmvInfo.fgEnAudio)
            {
                VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                _SWDMX_WMVWaitVDec(2);
                VDEC_Pause(prSwdmxInst->u1VdecId);
                if(ENABLE_NEWTRACK(prSwdmxInst))
                {
                    VDEC_Pause(prSwdmxInst->u1Vdec2Id);
                }
                _rSwdmxWmvInfo.fgVideoPause = TRUE;
            }
#endif
            STC_StopStc(prSwdmxInst->u1StcId);
            STC_SetStcValue(prSwdmxInst->u1StcId, (UINT32)_rSwdmxWmvInfo.u8SeekPTS);
            
            _rSwdmxWmvInfo.u4DisplayPTS=_rSwdmxWmvInfo.u8SeekPTS;
            _rSwdmxWmvInfo.u8SeekPTS = 0xFFFFFFFF;
            _rSwdmxWmvInfo.u8SeekPTS <<= 32;
            _rSwdmxWmvInfo.u8SeekPTS += 0xFFFFFFFF;

        }
        // NOT flush:   1x->2x, 1x->1/2x, 1/2x->1/4x, 
        //              1/2x->1x, 1/4x->1x, 1/8x->1x etc
        // 
        // Flush:       1/2x->2x, 1/4x->2x, 1/8x->2x etc
        // 
        // DTV00513355, VDEC could not get IFrame, screen will black, 
        //              so flush video buffer when 1/2x -> 2x
        //
        // DTV00535795, 1/2x -> 1/4x would assert in WMV decoder,
        //              so do NOT flush video buffer
        else if (    
                     (!
                       (_rSwdmxWmvInfo.i4WMVPlaySpeed > 1 
                        && _rSwdmxWmvInfo.i4WMVPlaySpeed < 1000
                        && i4PlaySpeed == 2000)
                     ) 
                 && 
                     (_rSwdmxWmvInfo.i4WMVPlaySpeed > 1 
                      && _rSwdmxWmvInfo.i4WMVPlaySpeed <= 1000
                      && i4PlaySpeed != 1000
                      && ePlayMode == SWDMX_PLAY_MODE_NORMAL
                     )
                 )
        {
            //do nothing
            if (_rSwdmxWmvInfo.fgVideoPause && i4PlaySpeed==1)
            {
                _WMVStartToPlay(prSwdmxInst);
            }
            
            LOG(0, "flush(video:no,audio:yes) speed(%d->%d)\n", 
                _rSwdmxWmvInfo.i4WMVPlaySpeed, 
                i4PlaySpeed);
            DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);

            #ifdef WMV_AUDIO_USE_PRE_BUFFER
            for (u1Idx = 0;u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
            {
                DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
            }
            DMX_AudHandler_SetActivePidx(DMX_AUDIO_HANDLER_NULL_PIDX);
            #endif
            
            _rSwdmxWmvInfo.fgAlreadySet = TRUE;
            _rSwdmxWmvInfo.u4PrevAudPts = 0;

        }
#ifdef WMA_TRICK_MODE
        else if((_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)&&(i4PlaySpeed >= 2000)&&(_rSwdmxWmvInfo.i4WMVPlaySpeed>0))
        {
            //nothing to do
        }
#endif
        else if(fgSpeedChange)
        {
            UINT32 u4CurPts=0;

            if (_eCurFMT==SWDMX_FMT_WMA_AUDIO_ES && _rSwdmxWmvInfo.prWmvKeyIdxTbl == NULL)
            {
                return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
            }
#ifdef WMA_TRICK_MODE
            if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
                if((_rSwdmxWmvInfo.i4WMVPlaySpeed!=1000&&i4PlaySpeed==1000)||(i4PlaySpeed<0))
                {
                    UINT32 u4DelayTime=0;
#ifdef CC_AUD_SUPPORT_DUAL_DSP 
                    while(u4DelayTime<20&&AUD_IsDecoderStop(AUD_DSP0, prSwdmxInst->u1AdecId))
#else
                    while(u4DelayTime<20&&AUD_IsDecoderStop(prSwdmxInst->u1AdecId))
#endif                    	
                    {
                       x_thread_delay(50);
                       u4DelayTime++;
                    }
                    if(u4DelayTime!=20)
                    {
                        //AUD_MMQueueSyncInfo(prSwdmxInst->u1AdecId,0,0,TRUE);
                    }
                    else
                    {
                        LOG(0,"Speed change, But audio is stop now\n");
                        return eSWDMX_HDR_PRS_FAIL;
                    }
                    AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4CurPts, FALSE);
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                    if(u4CurPts==0 &&AUD_IsDecoderRealPlay(AUD_DSP0, prSwdmxInst->u1AdecId))
#else
                    if(u4CurPts==0 &&AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId))
#endif                    	
                    {
                        AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4CurPts, FALSE);
                    }
                }
#ifdef CC_AUD_SUPPORT_DUAL_DSP
                else if(AUD_IsDecoderRealPlay(AUD_DSP0, prSwdmxInst->u1AdecId))
#else
                else if(AUD_IsDecoderRealPlay(prSwdmxInst->u1AdecId))
#endif                	
                {
                    AUD_GetAudioTrickPts(prSwdmxInst->u1AdecId, &u4CurPts, FALSE);
                }
                
                if(u4CurPts==0)
                {
                
                    LOG(0,"First CurPts iz 0\n");
                    u4CurPts=STC_GetStcValue(prSwdmxInst->u1StcId);
                    LOG(0,"STC Pts iz (%d,%d)\n",u4CurPts,u4CurPts/90000);
                }

                if(u1SrcId<SWDMX_SOURCE_MAX)
                {
                    if(i4PlaySpeed<0)
                    { 
                        if(u4CurPts==0||u4CurPts>_rSwdmxWmvInfo.prRangeInfo->ui8_pb_duration)
                        {
                            u4CurPts=u4AudDspPts[u1SrcId];
                        }
                        STC_StopStc(prSwdmxInst->u1StcId);
                        STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                        u4AudDspPts[u1SrcId]=u4CurPts;
                        fgTrickTrigger[u1SrcId]=TRUE;
                    }
                    else
                    {  
                       u4AudDspPts[u1SrcId]=1;                    
                    }
                }

                //AUD_GetDspPtsInfo(prSwdmxInst->u1AdecId, &u4CurPts, &u4PrsPos);
                LOG(0,"First CurPts: (%d,%d)\n",u4CurPts,u4CurPts/90000);
            }
            else
#endif
            {
                u4CurPts = VDP_GetPts(prSwdmxInst->u1B2rId);
            }
            _rSwdmxWmvInfo.u4DisplayPTS= u4CurPts;

            //MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
            //VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
            if (prVdecEsInfo != NULL)
            {
                //reset seek operation flag when speed change
                if (prVdecEsInfo->fgRenderPts)
                {
                    prVdecEsInfo->fgRenderPts = FALSE;
                }
            }
#ifdef WMA_TRICK_MODE
            if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
                if(i4PlaySpeed < 0)
                {
                    _SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
                    _rSwdmxWmvInfo.u8PreviousUIPos = _rSwdmxWmvInfo.u8LastFileParsingPosition;
                }

                _rSwdmxWmvInfo.fgFinishKeyTable=FALSE;
                _rSwdmxWmvInfo.u4IndexPts=0;
				_rSwdmxWmvInfo.u2SendPayloadNo = 0;
                _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
                _rSwdmxWmvInfo.fgBlock = FALSE;
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
            }
            else
#endif
            {
                MPV_FlushEsmQ(prSwdmxInst->u1VdecId, TRUE);
                VDEC_ReleaseDispQ(prSwdmxInst->u1VdecId);
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid);
                if (ENABLE_NEWTRACK(prSwdmxInst))
                {
                    MPV_FlushEsmQ(prSwdmxInst->u1Vdec2Id, TRUE);
                    VDEC_ReleaseDispQ(prSwdmxInst->u1Vdec2Id);
                    DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid);
                }
                DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
            }

            #ifdef WMV_AUDIO_USE_PRE_BUFFER
            for (u1Idx = 0;u1Idx < WMV_MAX_AUDIO_TRACK; u1Idx++)
            {
                DMX_MM_FlushBuffer(_rSwdmxWmvInfo.u1AudPreBufPid[u1Idx]);
            }
            #endif

            _SWDMX_FlushReqMsgQ(u1SrcId);
            _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
            _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
            _rSwdmxWmvInfo.u4ConsumedSize = 0;
            _rSwdmxWmvInfo.fgAlreadySet = FALSE;
            _fgReceiveVideoPes = FALSE;
            _fgReceiveAudioPes = FALSE;
            _u4PrevFramePTS=0;
            
            //VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
            LOG(0,"VDEC_SetRenderFromPts : %x\n",u4CurPts);
            _rSwdmxWmvInfo.u4PrevAudPts = 0;

#ifdef WMV_BAD_INTLV_SUPPORT
            _WMVResetBadIntlv(prSwdmxInst);
#endif

            // Things for 32/16/8/4/2 -> 1X are the same
            if ((_rSwdmxWmvInfo.i4WMVPlaySpeed >= 2000 || _rSwdmxWmvInfo.i4WMVPlaySpeed < 0
                    || (_rSwdmxWmvInfo.i4WMVPlaySpeed >=1 && _rSwdmxWmvInfo.i4WMVPlaySpeed<1000))
                    && (i4PlaySpeed == 1000 || i4PlaySpeed==2000 || i4PlaySpeed==500 || i4PlaySpeed ==1))
            {
                //UINT32 u4RemainedDataSize;
#ifdef WMA_TRICK_MODE
                if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES &&( (_rSwdmxWmvInfo.i4WMVPlaySpeed<0 || _rSwdmxWmvInfo.i4WMVPlaySpeed>=2000)&&(i4PlaySpeed==1000 || i4PlaySpeed==2000)))
                {
                    _SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
                    LOG(2,"WMA FilePos=%d, CurPrsPktNo=%d\n",
                        _rSwdmxWmvInfo.u8LastFileParsingPosition,
                        _rSwdmxWmvInfo.u4CurParsingPacketNo);
                    _rSwdmxWmvInfo.fgFinishKeyTable=FALSE;
                    _rSwdmxWmvInfo.u2SendPayloadNo = 0;
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
                    _rSwdmxWmvInfo.fgBlock = FALSE;
                    if(u1SrcId<SWDMX_SOURCE_MAX)
                    {
                        u4AudDspPts[u1SrcId]=1;
                    }
                    _SWDMX_WMVConsumeFeeder(prSwdmxInst);
                    STC_StopStc(prSwdmxInst->u1StcId);
                    STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                }
                else
#endif
                {
                    VDP_GetFilePosition(prSwdmxInst->u1B2rId, &i4TempRef, &u2DecodingOrder,
                                    &u8FrameOffset, &u8IFrameOffset);
                    _rSwdmxWmvInfo.u2SendPayloadNo = 0;
                    _rSwdmxWmvInfo.u2PreviousSendPayloadNo =0;
                    _rSwdmxWmvInfo.fgBlock = FALSE;
                    _rSwdmxWmvInfo.u4CurParsingPacketNo = (UINT32)u8IFrameOffset;
                    LOG(5, "got IFrameOfst %lld\n", u8IFrameOffset);
#ifdef CC_3D_MM_DS_SUPPORT
                    if (ENABLE_NEWTRACK(prSwdmxInst))
                    {
                        _WMVChkResume1XPos(prSwdmxInst);
                    }
#endif
                    _rSwdmxWmvInfo.u8LastFileParsingPosition = _prRangeInfo->ui8_fileoffset 
                            + WMV_DATA_OBJ_HEADER 
                            + ((UINT64)_rSwdmxWmvInfo.u4CurParsingPacketNo)
                                * _rSwdmxWmvInfo.u4PacketSize;

                    //u4RemainedDataSize = _rSwdmxWmvInfo.u4DataObjSize-
                    //(_rSwdmxWmvInfo.u8LastFileParsingPosition-prRangeList->prDmxRangeInfo->ui8_fileoffset);
                    _SWDMX_WMVConsumeFeeder(prSwdmxInst);
 
                    //since pts is not precious, jump to next 2 frame to avoid frame flicking
                    if (_rSwdmxWmvInfo.eVideoType == ENUM_SWDMX_VDEC_VC1 && _rSwdmxWmvInfo.i4WMVPlaySpeed ==2000)
                    {
                        u4CurPts += 6000;
                    }
                
                    if((_rSwdmxWmvInfo.i4WMVPlaySpeed >=1 && _rSwdmxWmvInfo.i4WMVPlaySpeed<1000)&&(i4PlaySpeed == 1000))
                    {
                        UINT32 u4CurSTC;
                        u4CurSTC = STC_GetStcValue(prSwdmxInst->u1StcId);
                        u4CurPts = max(u4CurPts,u4CurSTC);
                    }
                
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts); //maiyou
                    VDEC_StartPTS(prSwdmxInst->u1VdecId,TRUE,u4CurPts,NULL); 
                    if(ENABLE_NEWTRACK(prSwdmxInst))
                    {
                        VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts); 
                        VDEC_StartPTS(prSwdmxInst->u1Vdec2Id,TRUE,u4CurPts,NULL); 
                    }
                    LOG(3,"vdp get PTS: %d, req from 0x%llx\n",
                        (u4CurPts/90000), _rSwdmxWmvInfo.u8LastFileParsingPosition);
#ifdef WAIT_AUDIO_READY
                    if (!_rSwdmxWmvInfo.fgVideoPause && _rSwdmxWmvInfo.fgEnVideo && _rSwdmxWmvInfo.fgEnAudio)
                    {
                        VDP_SetPauseMm(prSwdmxInst->u1B2rId, TRUE);
                        _SWDMX_WMVWaitVDec(3);
                        VDEC_Pause(prSwdmxInst->u1VdecId);
                        if(ENABLE_NEWTRACK(prSwdmxInst))
                        {
                            VDEC_Pause(prSwdmxInst->u1Vdec2Id);
                        }
                        _rSwdmxWmvInfo.fgVideoPause = TRUE;
                    }
#endif 
                    STC_StopStc(prSwdmxInst->u1StcId);
                    STC_SetStcValue(prSwdmxInst->u1StcId, u4CurPts);
                }
            }
            // Things for 32/16/8/4/2X/1X -> -2X/-4X/-8X/-16X/-32X are the same
            else if (_rSwdmxWmvInfo.i4WMVPlaySpeed > 0 && i4PlaySpeed < 0 )
            {
                if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
                {
                    _SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
                }
                else
                {
                   _SWDMX_WMVSeekOffset(prSwdmxInst,TRUE);
                }
				
                #ifdef WMA_TRICK_MODE
                if(_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES)
                #endif
                VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId,0);//maiyou
                if(ENABLE_NEWTRACK(prSwdmxInst))
                {
                    VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id,0);
                }
            }
            //else if( _rSwdmxWmvInfo.i4WMVPlaySpeed >=2000 && i4PlaySpeed>2000)
            else if (i4PlaySpeed>=2000)
            {
                if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
                {
					_SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
					LOG(0, "idxtbl - speed: %d->%d, pts: %d.%05d\n",
					    _rSwdmxWmvInfo.i4WMVPlaySpeed,
					    i4PlaySpeed,
					    u4CurPts/90000,
					    u4CurPts%90000);
                }
				else
				{
				    _SWDMX_WMVSeekOffset(prSwdmxInst,TRUE);
					LOG(0, "no-idxtbl - speed: %d->%d\n",
					    _rSwdmxWmvInfo.i4WMVPlaySpeed,
					    i4PlaySpeed);
				}
#ifdef WMA_TRICK_MODE
                if(_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES)
#endif
                {
                    if (_rSwdmxWmvInfo.u4CurParsingPacketNo < u8IFrameOffset)
                    {
                        _rSwdmxWmvInfo.u4CurParsingPacketNo = (UINT32)u8IFrameOffset;
                    }
                    
                    VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, u4CurPts);
                    if(ENABLE_NEWTRACK(prSwdmxInst))
                    {
                        VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, u4CurPts);
                    }
                }
            }
            else if (i4PlaySpeed < 0)
            {
            
#ifdef WMA_TRICK_MODE
                if(_rSwdmxWmvInfo.prWmvKeyIdxTbl)
                {
                    _SWDMX_WMVSeek(prSwdmxInst, u4CurPts, prRangeList);
                }
                else
                {
                   _SWDMX_WMVSeekOffset(prSwdmxInst,TRUE);
                }
                if(_eCurFMT != SWDMX_FMT_WMA_AUDIO_ES)
#endif
                { 
                  VDEC_SetRenderFromPts(prSwdmxInst->u1VdecId, 0);
                  if(ENABLE_NEWTRACK(prSwdmxInst))
                  {
                      VDEC_SetRenderFromPts(prSwdmxInst->u1Vdec2Id, 0);
                  }
                }
            }
            else
            {
                //null operation
            }

        }
        _SWDMX_WMVHandleEos(prSwdmxInst, prVdecEsInfo, prRangeList, i4PlaySpeed);        
    }

_header_parse_tail:
    if (_rSwdmxWmvInfo.i4WMVPlaySpeed != i4PlaySpeed)
    {
        _rSwdmxWmvInfo.i4WMVPlaySpeed = i4PlaySpeed;
    }
    
    if(_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
    {
        if (_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000)
        {
            _rSwdmxWmvInfo.u8WMADecodingPos = _WMVCalcWMADecodingPos(prSwdmxInst);
            //_rSwdmxWmvInfo.u8PreviousUIPos = 0;
        }
#ifdef CC_AUD_SUPPORT_DUAL_DSP
        if((!AUD_IsDecoderPlay(AUD_DSP0, prSwdmxInst->u1AdecId))||AUD_IsDecoderStop(AUD_DSP0, prSwdmxInst->u1AdecId))
#else
        if((!AUD_IsDecoderPlay(prSwdmxInst->u1AdecId))||AUD_IsDecoderStop(prSwdmxInst->u1AdecId))
#endif      	
        {
            LOG(3,"WMA Wait audio ready\n");
            AUD_ClearEOSFlag(prSwdmxInst->u1AdecId);
            x_thread_delay(50);
            return eSWDMX_HDR_PRS_SUCCEED;
        }

#ifdef CC_AUD_SUPPORT_DUAL_DSP
        if((_rSwdmxWmvInfo.i4WMVPlaySpeed!=1000 || _rSwdmxWmvInfo.u4AudioCount==0)&&(AUD_DRVGetAudioState(AUD_DSP0, prSwdmxInst->u1AdecId)==AUD_WAIT_PLAY))
#else
        if((_rSwdmxWmvInfo.i4WMVPlaySpeed!=1000 || _rSwdmxWmvInfo.u4AudioCount==0)&&(AUD_DRVGetAudioState(prSwdmxInst->u1AdecId)==AUD_WAIT_PLAY))
#endif      	
        {
            AUD_MM_Set_Avsync_event(prSwdmxInst->u1AdecId);
            x_thread_delay(50);
        }
        if(!_rSwdmxWmvInfo.fgEnAudio || _rSwdmxWmvInfo.fgAudStrmDisabled)
        {
            x_thread_delay(50);
            LOG(3,"wait audio enabled \n");
            return eSWDMX_HDR_PRS_SUCCEED;
        }
    }

     
    LOG(8, "seek_type %d, seek_mode %d, src_type %d(2:DLNA)\n", 
        prSwdmxInst->eSeekType, 
        _u4CmdSeekMode,
        _prRangeInfo->e_mm_src_type);
    if (prSwdmxInst->eSeekType ==  eSWDMX_TYPE_TIME_SEEK)
    {
        LOG(7, "time based deliver data!!\n");
        if (_SWDMX_WMVDeliverDataTimebased(u1SrcId, prRangeList) == FALSE)
        {
            LOG(9, "_SWDMX_WMV_HEADER_PARSE FALSE\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
    }
    else
    {
        if (_SWDMX_WMVDeliverData(prSwdmxInst, prRangeList) == FALSE)
        {
            LOG(9, "_SWDMX_WMV_HEADER_PARSE FALSE\n");
            return eSWDMX_HDR_PRS_FED_MOVE_FAIL;
        }
    }
    return eSWDMX_HDR_PRS_SUCCEED;
}


/**
*/

BOOL _SWDMX_WMVAbort(UINT8 u1SrcId)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);
    _rSwdmxWmvInfo.fgPlayAbort=TRUE;
    return TRUE;
}

BOOL _SWDMX_WMVStop(UINT8 u1SrcId)
{
    BOOL fgEnVDmx;
    BOOL fgEnADmx;
    //DMX_MM_T rMM;        
    DMX_MM_T rPid;

    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);

    
    x_memset((void*)&rPid, 0, sizeof(DMX_MM_T));
    rPid.fgEnable = FALSE;

    fgEnVDmx = _rSwdmxWmvInfo.fgEnVDmx;
    fgEnADmx = _rSwdmxWmvInfo.fgEnADmx;

    LOG(0, "_SWDMX_WMV_STOP\n");

    if(_rSwdmxWmvInfo.fgIsDRM && (_rSwdmxWmvInfo.eDRMType == DRM_KEY_AES))
    {
        if (_u4VideoFrameDataAddr!=0)
        {
            BSP_FreeAlignedDmaMemory((UINT32)_u4VideoFrameDataAddr);
            _u4VideoFrameDataAddr =0;
        }
        
    }

#ifdef WMV_ALLOCATE_VFIFO
    if (_u4VFifoStartAddr!= NULL)
    {
        BSP_FreeAlignedDmaMemory((VOID*)_u4VFifoStartAddr);
        _u4VFifoStartAddr = NULL;
    }
#endif
    if (_rSwdmxWmvInfo.fgEnVideo)//protect func
    {
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1VidPid);
        if(ENABLE_NEWTRACK(prSwdmxInst))
        {
           DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1Vid2Pid);
        }
    }
    if (_rSwdmxWmvInfo.fgEnAudio)//protect func
    {
        DMX_MM_FlushBuffer(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
    }
    
    if (_rSwdmxWmvInfo.fgEnADmx)
    {
        VERIFY(DMX_MM_Set(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid, DMX_MM_FLAG_VALID, &rPid));
        VERIFY(DMX_MM_Free(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid));
        VERIFY(DMX_MUL_FreePidx(prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid));
        #ifdef WMV_AUDIO_USE_PRE_BUFFER
        {
            INT32 i = 0;
            for (i = 0; i < WMV_MAX_AUDIO_TRACK; i++)  // !!!
            {
                VERIFY(DMX_MM_Set(_rSwdmxWmvInfo.u1AudPreBufPid[i], DMX_MM_FLAG_VALID, &rPid));
                DMX_MM_Free(_rSwdmxWmvInfo.u1AudPreBufPid[i]);
                DMX_MUL_FreePidx(_rSwdmxWmvInfo.u1AudPreBufPid[i]);
                DMX_AudHandler_Free(_rSwdmxWmvInfo.u1AudPreBufPid[i]);
            }
        }
        #endif
        _rSwdmxWmvInfo.fgEnADmx = FALSE;
    }
    //_SWDMX_WMVFlushFeederDataQ(prSwdmxInst);

    //x_memset(&(_rSwdmxWmvInfo),0,sizeof(SWDMX_WMV_INFO_T));  for multi-instance

#ifdef CC_NETFLIX_CMPB_SUPPORT
    _u4RemainPacketDataSize = 0;
    UNUSED(_pu1RemainPacketData);
#endif
    _rSwdmxWmvInfo.fgIsDRM = FALSE;
    _rSwdmxWmvInfo.fgFirstFrame = TRUE;
    _rSwdmxWmvInfo.fgFirstReqFeederData = TRUE;
    _rSwdmxWmvInfo.fgBeginPlay = TRUE;
    _rSwdmxWmvInfo.u8SeekPTS = 0xFFFFFFFF;
    _rSwdmxWmvInfo.u8SeekPTS <<= 32;
    _rSwdmxWmvInfo.u8SeekPTS += 0xFFFFFFFF;

    _u8SampleId = 0;
    _fgNetflixEos = FALSE;
    _fgDataExhausted = TRUE;
    _u1AudioStreamId = 0xFF;
    _u4PrevFramePTS = 0;
    _u4DefalutReqSize = 0;
    _fgReceiveAudioPes = FALSE;
    _fgReceiveVideoPes = FALSE;
    _rSwdmxWmvInfo.fgIs3DFile = FALSE;
    _rSwdmxWmvInfo.fgIsSetRangeInvoked = FALSE;

    _rSwdmxWmvInfo.u4FirstGoodIdx = 0;
    _rSwdmxWmvInfo.u4LastGoodIdx = 0;

    _rSwdmxWmvInfo.u4PrevAudPts = 0;
    _SWDMX_WMVFlushFeederDataQ(prSwdmxInst);
    _rFeederReqData.fgEof = FALSE;
    _rSwdmxWmvInfo.u2WmaPlCntPerPkt = 0;


    if(u1SrcId<SWDMX_SOURCE_MAX)
    {
        u4AudDspPts[u1SrcId]=1;
    }
    _rSwdmxWmvInfo.fgEnVDmx = fgEnVDmx;
    _rSwdmxWmvInfo.fgEnADmx = fgEnADmx;
    _rSwdmxWmvInfo.u4VideoCount = 0;
    _rSwdmxWmvInfo.fgAudStrmDisabled = TRUE;

    _rSwdmxWmvInfo.u4AvgPktCountPerEntry = 0;
    _rSwdmxWmvInfo.u4TotalPktCountPerEntry = 0;
    _rSwdmxWmvInfo.u4TrickModeParsingActionCount = 0;

    #ifdef WMV_AUDIO_USE_PRE_BUFFER
    DMX_AudHandler_SetEnable(FALSE);
    #endif
    _rSwdmxWmvInfo.fgAlreadySet = FALSE;
    _rSwdmxWmvInfo.fgEofParsed = FALSE;
    _rSwdmxWmvInfo.fgGotEofFromWMVMsgQ = FALSE;
    _g_u4PacketCountTb = 0;
    _rSwdmxWmvInfo.fgFirstDataIsAsfObjHdr = FALSE;
    _rSwdmxWmvInfo.fgDeliverAEos = FALSE;
    _rSwdmxWmvInfo.fgDeliverVEos = FALSE;

#if 1
    _rSwdmxWmvInfo.fgInternalInited = FALSE;
#else
    VERIFY(x_msg_q_delete(_hWMVFeederDataQue) == OSR_OK);
    _hWMVFeederDataQue = 0;
#endif
    AUD_DrvSetWMAErrFunc(prSwdmxInst->u1AdecId,NULL, NULL);  // megaa 20090828
 
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
    if (prSwdmxInst->fgIsBadInterleaved)
    {
        // send dummy message to unlock the instance waiting if any
        UINT32 u4Msg = 0;
        x_msg_q_send(_hCommBIQ, &u4Msg, sizeof(UINT32), 1);
    }
#endif

    return TRUE;
}

BOOL _SWDMX_WMVSetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3)
{
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);        

    if (u1Type == eSWDMX_SET_VIDEOTYPE)
    {
#if defined(CC_WMV_MPEG1_DISABLE) && defined(CC_WMV_MPEG2_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MPEG1_MPEG2)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_WMV_MPEG4_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MPEG4)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_WMV_DX311_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_DX311)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_WMV_H264_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_H264)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_WMV_H263_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_H263)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_WMV_VC1_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_VC1)
        {
            return FALSE;
        }
        else
#endif
#if defined(CC_WMV_MJPEG_DISABLE)
        if (u4Para1 == (UINT32)ENUM_SWDMX_VDEC_MJPEG)
        {
            return FALSE;
        }
        else
#endif
        {
            _rSwdmxWmvInfo.eVideoType =  (ENUM_SWDMX_VDEC_T)u4Para1;
            LOG(0, "setinfo() video type %d\n", _rSwdmxWmvInfo.eVideoType);
        }
    }
    else if (u1Type == eSWDMX_SET_AUDIOTYPE)
    {
        _rSwdmxWmvInfo.eAudioType = (ENUM_SWDMX_ADEC_T)u4Para1;
        _u4WmaBytesPerSec = u4Para2;
        //printf("WMV Bytes Per Sec = %d\n", _u4WmaBytesPerSec);
    }
    else if (u1Type == eSWDMX_SET_VIDSEQHEADER)
    {
#if 0
        _rSwdmxWmvInfo.u4SequenceHeader = u4Para1;
        _rSwdmxWmvInfo.u4SequenceHeaderLength = u4Para2;
#else
        x_memcpy((VOID*)(VIRTUAL((UINT32)&_pu1VideoFrameData[0])), (VOID*)VIRTUAL(u4Para1), (u4Para2));
        _rSwdmxWmvInfo.u4SequenceHeader = (UINT32)VIRTUAL((UINT32)&_pu1VideoFrameData[0]);
        _rSwdmxWmvInfo.u4SequenceHeaderLength = u4Para2;
#endif
    }
    else if (u1Type == eSWDMX_SET_NETFLIX_STREAM_ID)
    {
        _u4NetflixVideoStreamId = (UINT32)(u4Para1);
        _u4NetflixAudioStreamId = (UINT32)(u4Para2);
    }
    else if (u1Type == eSWDMX_SET_NETFLIX_EOS)
    {
        _fgNetflixEos = (BOOL)(u4Para1);
    }
    else if (u1Type == eSWDMX_SET_SYSTEMINFO)
    {
        _prWmvGenericInfo = (MINFO_TYPE_GENERIC_T*)(u4Para1);
        _u4NumberOfBitstream = (UINT32)(u4Para2);
        _WMVFindMaxPacketSize(prSwdmxInst);
    }

    return TRUE;
}

#if 1
static UINT64 _WMVCalcWMADecodingPos(SWDMX_INFO_T *prSwdmxInst)
{
    UINT32 u4APts = 0;
    UINT32 u4FileInfoLen = 0;
    UINT64 u8FileDataLen = 0;
    UINT64 u8Remainder = 0;
    UINT64 u8Ret = 0;
//  UINT64 u8AbsOfst = 0;
//  UINT32 u4FoundPts = 0;
//  UINT32 u4FoundIdx = 0;


    AUD_GetAudioPTS(prSwdmxInst->u1AdecId, &u4APts);
    u4FileInfoLen = _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER;
    u8FileDataLen = _rSwdmxWmvInfo.u8FileSize - u4FileInfoLen;

	if (_prRangeInfo->ui8_pb_duration > 0)
	{
	    u8Ret = _SWDMX_Div6464(u8FileDataLen * u4APts, 
	                           _prRangeInfo->ui8_pb_duration, 
	                           &u8Remainder);		
	}
	
    u8Ret += u4FileInfoLen;
    UNUSED(u8Remainder);

//  if (_WMVSearchAllEntriesByPts(prSwdmxInst,
//                                (UINT64)u4APts,
//                                &u8AbsOfst,
//                                &u4FoundPts,
//                                &u4FoundIdx))
//  {
//      LOG(5, "_WMVCalcWMADecodingPos::search table by pts(0x%x) get 0x%llx(#%d)\n",
//          u4APts, u8AbsOfst, u4FoundIdx);
//  }
    return u8Ret;
}
#endif

BOOL _SWDMX_WMVGetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32* pu4Para1,UINT32* pu4Para2,UINT32* pu4Para3)
{
    UINT64 *pu8Tmp;
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);        

    LOG(7, "_SWDMX_WMVGetInfo u1Type = %d\n", u1Type);

    if (u1Type == eSWDMX_GET_CURRENT_POSITION)
    {
        pu8Tmp = (UINT64*)pu4Para1;

        if (_rSwdmxWmvInfo.fgEnVideo)
        {
            if (pu4Para1 == NULL)
            {
                LOG(1, "_SWDMX_WMVGetInfo pu4Para1 = NULL\n");
                return FALSE;
            }
            *pu8Tmp = 0;
            
            if (_prRangeInfo != NULL)
            {
                *pu8Tmp = _prRangeInfo->ui8_fileoffset
                          + ((UINT64)_rSwdmxWmvInfo.u4CurParsingPacketNo
                             * _rSwdmxWmvInfo.u4PacketSize)
                          + WMV_DATA_OBJ_HEADER;
            }
        }
        else if (prSwdmxInst->eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
        {
            
            UINT32 u4DmxAvailSizeAudio;
            UINT32 u4Addr;
            UINT32 u4BufSize;
            UINT32 u4DataSize = 0;
            UINT32 u4DecodingRemainder = 0;
            //UINT32 u4PayloadPerPkt = 0;
            UINT32 u4PacketPayloadDiff = 0;


            u4DmxAvailSizeAudio = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId,
                                                       DMX_PID_TYPE_ES_AUDIO,
                                                       _rSwdmxWmvInfo.u1AudPid);
            if(!_SWDMX_GetAudioFifo(u1SrcId, &u4Addr, &u4BufSize))
            {
                LOG(3, "_SWDMX_GetAudioFifo Fail\n");
                return FALSE;
            }
            if (0 == _rSwdmxWmvInfo.u4AudioObjSz)
            {
                LOG(0, "audio object size ZERO! (not play yet)\n");
                return FALSE;
            }

            //u4PayloadPerPkt = _rSwdmxWmvInfo.u4PacketSize / _rSwdmxWmvInfo.u4AudioObjSz;
            u4PacketPayloadDiff = _rSwdmxWmvInfo.u4PacketSize 
                                  - _rSwdmxWmvInfo.u2WmaPlCntPerPkt 
                                    * _rSwdmxWmvInfo.u4AudioObjSz;
            u4DataSize = u4BufSize - u4DmxAvailSizeAudio;

            if (prSwdmxInst->i4PlaySpeed < 0)
            {     
                UINT64 u8PreviousUIPos = _rSwdmxWmvInfo.u8PreviousUIPos;
                UINT64 u8CurMoviPos = 0;
                UINT32 u4AfifoPayloadCount = 0;
                //UINT32 i = 0;
                UINT32 u4CompletedParagraph = 0;
                UINT32 u4SliceParagraph = 0;

                UINT32 u4Trynum = 0;

                /*change Normal play to FR*/
                if(_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000)
                {
                    while((_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000) && (u4Trynum < 10))
                    {
                        /*wait 500ms the speed change done in main loop*/
                        u4Trynum++;
                        x_thread_delay(50);
                    }

                    if((_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000) && (u4Trynum >= 10))
                    {
                        LOG(0,"Wait 500ms time out!!!\n");
                    }
                    else
                    {
                        u8PreviousUIPos = 0;
                    }
                }
                
                if(_rSwdmxWmvInfo.u4AudioObjSz > 0)
                {
                    u4AfifoPayloadCount = u4DataSize / _rSwdmxWmvInfo.u4AudioObjSz;
                }
                else
                {
                    LOG(0,"_rSwdmxWmvInfo.u4AudioObjSz is ZERO !!!\n");
                }
                u4DecodingRemainder = u4DataSize % _rSwdmxWmvInfo.u4AudioObjSz;
                LOG(6, "u4AfifoPayloadCount(%d) = %d / %d\n", 
                    u4AfifoPayloadCount, u4DataSize, _rSwdmxWmvInfo.u4AudioObjSz);
                LOG(6, "u4DecodingRemainder(%d) = %d %% %d\n", 
                    u4DecodingRemainder, u4DataSize, _rSwdmxWmvInfo.u4AudioObjSz);

                if (0 == _rSwdmxWmvInfo.u4AvgPktCountPerEntry)
                {
                    *pu8Tmp = _rSwdmxWmvInfo.u8LastFileParsingPosition 
                                + u4PacketPayloadDiff 
                                + u4DecodingRemainder;
                    _rSwdmxWmvInfo.u8PreviousUIPos = *pu8Tmp;
                    return TRUE;
                }
                #if 1
                u4CompletedParagraph  = u4AfifoPayloadCount / _rSwdmxWmvInfo.u4AvgPktCountPerEntry;
                u4SliceParagraph      = u4AfifoPayloadCount % _rSwdmxWmvInfo.u4AvgPktCountPerEntry;
                if(_rSwdmxWmvInfo.u2WmaPlCntPerPkt > 0)
                {
                    u4CompletedParagraph /= _rSwdmxWmvInfo.u2WmaPlCntPerPkt;
                }
                else
                {
                    LOG(0,"_rSwdmxWmvInfo.u2WmaPlCntPerPkt is ZERO!! \n");
                }
                u4SliceParagraph     += (u4AfifoPayloadCount / _rSwdmxWmvInfo.u4AvgPktCountPerEntry) 
                                        % _rSwdmxWmvInfo.u2WmaPlCntPerPkt;
                LOG(6, "u4CompletedParagraph(%d) =  %d / %d\n", 
                    u4CompletedParagraph, 
                    u4AfifoPayloadCount, 
                    _rSwdmxWmvInfo.u4AvgPktCountPerEntry);
                LOG(6, "u4SliceParagraph(%d) =  %d %% %d\n", 
                    u4SliceParagraph, 
                    u4AfifoPayloadCount, 
                    _rSwdmxWmvInfo.u4AvgPktCountPerEntry);

                u8CurMoviPos = _rSwdmxWmvInfo.u8LastFileParsingPosition;
                u8CurMoviPos += u4CompletedParagraph
                                * (abs(_rSwdmxWmvInfo.i4WMVPlaySpeed)/1000 + 1)
                                * _rSwdmxWmvInfo.u4AvgPktCountPerEntry
                                * _rSwdmxWmvInfo.u4PacketSize;
                u8CurMoviPos += u4SliceParagraph * _rSwdmxWmvInfo.u4AudioObjSz; 

                //if ((118*4+32)!=u4DecodingRemainder)
                //    u8CurMoviPos += (_rSwdmxWmvInfo.u4PacketSize - u4DecodingRemainder);
                u8CurMoviPos += (u4PacketPayloadDiff + u4DecodingRemainder);//tiny effect
                #else
                u4CompletedParagraph = (u4AfifoPayloadCount 
                                            * _rSwdmxWmvInfo.u4TrickModeParsingActionCount)
                                        / _rSwdmxWmvInfo.u4TotalPktCountPerEntry;
                u4SliceParagraph = u4AfifoPayloadCount - u4CompletedParagraph;

                u8CurMoviPos = _rSwdmxWmvInfo.u8LastFileParsingPosition;
                u8CurMoviPos += (u4CompletedParagraph
                                    * (abs(_rSwdmxWmvInfo.i4WMVPlaySpeed)/1000 + 1)
                                    * _rSwdmxWmvInfo.u4TotalPktCountPerEntry
                                    * _rSwdmxWmvInfo.u4PacketSize)
                                / _rSwdmxWmvInfo.u4TrickModeParsingActionCount;
                u8CurMoviPos += u4SliceParagraph * _rSwdmxWmvInfo.u4PacketSize; 

                //if ((118*4+32)!=u4DecodingRemainder)
                //    u8CurMoviPos += (_rSwdmxWmvInfo.u4PacketSize - u4DecodingRemainder);
                u8CurMoviPos += (u4PacketPayloadDiff + u4DecodingRemainder);//tiny effect
                #endif
                
                #if 1
                if (0 == u8PreviousUIPos)  
                {
                    *pu8Tmp = min(_rSwdmxWmvInfo.u8WMADecodingPos, u8CurMoviPos);
                    LOG(5, "pts->pos: 0x%llx, fifo->pos: 0x%llx\n", 
                        _rSwdmxWmvInfo.u8WMADecodingPos, u8CurMoviPos);
                }
                else
                {
                    /* the more parsing action, the more accurent */
                    if (_rSwdmxWmvInfo.u4TrickModeParsingActionCount < 5)
                    {
                        *pu8Tmp = u8PreviousUIPos;
                    }
                    else
                    {
                        *pu8Tmp = min(u8PreviousUIPos, u8CurMoviPos);
                        LOG(5, "fifo->pos pre/cur 0x%llx/0x%llx\n", 
                            u8PreviousUIPos, u8CurMoviPos);
                    }
                }
                #else
                *pu8Tmp = min(u8PreviousUIPos, u8CurMoviPos);
                /* check if the previous UI position is valid or not */
                if (u8CurMoviPos > u8PreviousUIPos)
                {
                    UINT32 u4PosDiff = (UINT32)(u8CurMoviPos - u8PreviousUIPos);//risk here
                    UINT32 u4FRPayloadCountThreshold = 0;
                    UINT32 u4DiffPktCount = 0;
                    UINT32 u4DiffRemainder = 0;

                    u4DiffPktCount = u4PosDiff / _rSwdmxWmvInfo.u4PacketSize;
                    u4DiffRemainder = u4PosDiff % _rSwdmxWmvInfo.u4PacketSize;
                    if (u4DiffRemainder>0)
                    {
                        LOG(6, "\n\n\n!!!! u4DiffRemainder %d !!!!!!!!\n\n\n");
                        u4DiffPktCount++;
                    }
                    u4FRPayloadCountThreshold = u4DiffPktCount / _rSwdmxWmvInfo.u4AvgPktCountPerEntry;
                    if ((u4DiffPktCount % _rSwdmxWmvInfo.u4AvgPktCountPerEntry) > 0)
                        u4FRPayloadCountThreshold++;
                    if (u4FRPayloadCountThreshold > (abs(_rSwdmxWmvInfo.i4WMVPlaySpeed)/1000 + 1))
                    {
                        /* previous UI position is invalid! */
                        *pu8Tmp = u8CurMoviPos;
                        LOG(5, "pre(0x%llx) is invalid, use cur(0x%llx))\n",
                            u8PreviousUIPos, u8CurMoviPos);
                    }
                }
                #endif
                                      
                LOG(5, "---WMA GetInfo(FR) *pu8Tmp 0x%llx, u8CurMoviPos 0x%llx, u8PreviousUIPos 0x%llx\n",
                    *pu8Tmp, u8CurMoviPos, u8PreviousUIPos);

                LOG(6, "- u8LastFileParsingPosition(0x%llx/FS:0x%llx) offset(0x%llx)\n", 
                    _rSwdmxWmvInfo.u8LastFileParsingPosition,
                    _rSwdmxWmvInfo.u8FileSize,
                    _prRangeInfo->ui8_fileoffset + WMV_DATA_OBJ_HEADER);
                LOG(6, "- DmxDataSz(0x%x) AudObjSz(%d) u4AfifoPayloadCount(%d)\n", 
                    u4DataSize,
                    _rSwdmxWmvInfo.u4AudioObjSz, 
                    u4AfifoPayloadCount);
                /* 
                LOG(5, "- dump Rp1(0x%x/0x%x) - ", g_u4Rp, VIRTUAL(g_u4Rp));
                for (i = 0; i < 16; i++)
                    printf("%02x ", *((UINT8*)VIRTUAL(g_u4Rp) + i));
                printf("\n"); 
                */ 
                // seek end
                if (_prRangeInfo->ui8_fileoffset == _rSwdmxWmvInfo.u8LastFileParsingPosition)
                {
                    *pu8Tmp = 0;
                }

                _rSwdmxWmvInfo.u8PreviousUIPos = *pu8Tmp;
            }
            else if(prSwdmxInst->i4PlaySpeed != 1000 && prSwdmxInst->i4PlaySpeed != 0)
            {
                UINT32 u4DecodingPosition = 0;
                UINT32 u4DecodingPktNo = 0;  
                UINT32 u4AudObjSz = _rSwdmxWmvInfo.u4AudioObjSz;                                  
                UINT64 u8CurDmxMoviPos = _rSwdmxWmvInfo.u4CurParsingPacketNo 
                                         * _rSwdmxWmvInfo.u2WmaPlCntPerPkt
                                         * u4AudObjSz; /*todo: WMA filesize >4G */

                if (u8CurDmxMoviPos > u4DataSize)
                {
                    u4DecodingPosition = u8CurDmxMoviPos - u4DataSize;
                }
                else
                {
                    LOG(5, "u8CurDmxMoviPos(0x%llx,pktno:%d) < u4DataSize(0x%x)\n",
                        u8CurDmxMoviPos, _rSwdmxWmvInfo.u4CurParsingPacketNo, u4DataSize);
                    u4DecodingPosition = 0;
                }
                if(u4AudObjSz > 0 && _rSwdmxWmvInfo.u2WmaPlCntPerPkt > 0)
                {
                    u4DecodingPktNo = (u4DecodingPosition / u4AudObjSz) / _rSwdmxWmvInfo.u2WmaPlCntPerPkt;
                    u4DecodingRemainder = u4DecodingPosition % u4AudObjSz
                        + ((u4DecodingPosition / u4AudObjSz) % _rSwdmxWmvInfo.u2WmaPlCntPerPkt) 
                            * u4AudObjSz;
                }
                else
                {
                    LOG(0,"u4AudObjSz = %d,_rSwdmxWmvInfo.u2WmaPlCntPerPkt = %d\n",u4AudObjSz,_rSwdmxWmvInfo.u2WmaPlCntPerPkt);
                }
                    
                *pu8Tmp = _prRangeInfo->ui8_fileoffset
                          + (u4DecodingPktNo * _rSwdmxWmvInfo.u4PacketSize)
                          + WMV_DATA_OBJ_HEADER;
                *pu8Tmp += u4PacketPayloadDiff + u4DecodingRemainder;

                LOG(5, "---WMA GetInfo(FF) ret 0x%llx, DecodingPktNo/CurPrsPkt %d/%d, "
                       "ObjSz/PktSz %d/%d, Fifo 0x%x of 0x%x, ava 0x%x\n",
                    *pu8Tmp,u4DecodingPktNo,_rSwdmxWmvInfo.u4CurParsingPacketNo,
                    u4AudObjSz, _rSwdmxWmvInfo.u4PacketSize,
                    u4DataSize, u4BufSize, u4DmxAvailSizeAudio);

                _rSwdmxWmvInfo.u8PreviousUIPos = *pu8Tmp;
            }
            else
            {

                UINT32 u4Trynum = 0;
                
                /*change FR to normal play*/
                if(_rSwdmxWmvInfo.i4WMVPlaySpeed < 0)
                {
                    while((_rSwdmxWmvInfo.i4WMVPlaySpeed < 0) && (u4Trynum < 10))
                    {
                        /*wait 500ms the speed change done in main loop*/
                        u4Trynum++;
                        x_thread_delay(50);
                    }

                    if((_rSwdmxWmvInfo.i4WMVPlaySpeed < 0) && (u4Trynum >= 10))
                    {
                        LOG(0,"Wait 500ms time out!!!\n");
                    }

                    *pu8Tmp = _rSwdmxWmvInfo.u8PreviousUIPos;

                    _rSwdmxWmvInfo.u8PreviousUIPos = *pu8Tmp;
                 }

                /*normal play case and can not get duration, so need to use current play time to update*/
                if(_rSwdmxWmvInfo.i4WMVPlaySpeed == 1000)
                {
                    UINT32 u4DecodingPosition = 0;
                    UINT32 u4DecodingPktNo = 0;  
                    UINT32 u4AudObjSz = _rSwdmxWmvInfo.u4AudioObjSz;                                  
                    UINT64 u8CurDmxMoviPos = _rSwdmxWmvInfo.u4CurParsingPacketNo 
                                             * _rSwdmxWmvInfo.u2WmaPlCntPerPkt
                                             * u4AudObjSz; /*todo: WMA filesize >4G */

                    if (u8CurDmxMoviPos > u4DataSize)
                    {
                        u4DecodingPosition = u8CurDmxMoviPos - u4DataSize;
                    }
                    else
                    {
                        LOG(5, "u8CurDmxMoviPos(0x%llx,pktno:%d) < u4DataSize(0x%x)\n",
                            u8CurDmxMoviPos, _rSwdmxWmvInfo.u4CurParsingPacketNo, u4DataSize);
                        u4DecodingPosition = 0;
                    }
                    if(u4AudObjSz > 0 && _rSwdmxWmvInfo.u2WmaPlCntPerPkt > 0)
                    {
                        u4DecodingPktNo = (u4DecodingPosition / u4AudObjSz) / _rSwdmxWmvInfo.u2WmaPlCntPerPkt;
                        u4DecodingRemainder = u4DecodingPosition % u4AudObjSz
                            + ((u4DecodingPosition / u4AudObjSz) % _rSwdmxWmvInfo.u2WmaPlCntPerPkt) 
                                * u4AudObjSz;
                    }
                    else
                    {
                        LOG(0,"u4AudObjSz = %d,_rSwdmxWmvInfo.u2WmaPlCntPerPkt = %d\n",u4AudObjSz,_rSwdmxWmvInfo.u2WmaPlCntPerPkt);
                    }
                        
                    *pu8Tmp = _prRangeInfo->ui8_fileoffset
                              + (u4DecodingPktNo * _rSwdmxWmvInfo.u4PacketSize)
                              + WMV_DATA_OBJ_HEADER;
                    *pu8Tmp += u4PacketPayloadDiff + u4DecodingRemainder;
                }

            }
            
        }
        return TRUE;
    }
    else if (u1Type == eSWDMX_GET_IS_TRICK)
    {
        if (prSwdmxInst->eSeekType == eSWDMX_TYPE_TIME_SEEK)
        {
            if (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES)
            {
                /* DTV00573279, DTV00573759 */
                if (pu4Para1) *pu4Para1 = TRUE;
                if (pu4Para2) *pu4Para2 = TRUE;
            }
            else if (_rSwdmxWmvInfo.fgEnVideo)
            {
                if (pu4Para1) *pu4Para1 = TRUE;
                if (pu4Para2) *pu4Para2 = TRUE;
            }
            return TRUE;
        }
        else if ((_rSwdmxWmvInfo.fgEnVideo) || (_eCurFMT == SWDMX_FMT_WMA_AUDIO_ES))
        {
            if (_rSwdmxWmvInfo.prWmvKeyIdxTbl != NULL)
            {
                LOG(0, "index table NOT null 0x%x\n", _rSwdmxWmvInfo.prWmvKeyIdxTbl);
                if (_rSwdmxWmvInfo.prWmvKeyIdxTbl->ui4_number_of_entry < WMV_KEY_TABLE_THRESHOLD
                /* if index table has bad renty, we block trick operation */    
                /* || (_SWDMX_WMVIsKeyIndexTableValid(prSwdmxInst) == FALSE) */
                   )
                {
                    LOG(0, "index table entries too little(<3), GET_IS_TRICK returns FALSE;\n");
                    if (pu4Para1) *pu4Para1 = FALSE;
                    if (pu4Para2) *pu4Para2 = FALSE;
                }
                else
                {
                    LOG(0, "index table entries OK\n");
                    if (pu4Para1) *pu4Para1 = TRUE;
                    if (pu4Para2) *pu4Para2 = TRUE;
                }
            }
            else
            {
                LOG(0, "eSeekType %d(0:byte-seek 1:time-seek)\n", prSwdmxInst->eSeekType);
                #if 0
                if (prSwdmxInst->ePlayMode == SWDMX_PLAY_MODE_NORMAL)
                {
                    LOG(0, "even index table NULL, but now is normal play, return TRUE!\n");
                    if (pu4Para1) *pu4Para1 = TRUE;
                    if (pu4Para2) *pu4Para2 = TRUE;
                }
                else
                #endif
                {
                    LOG(0, "index table NULL, GET_IS_TRICK returns FALSE;\n");
                    if (pu4Para1) *pu4Para1 = FALSE;
                    if (pu4Para2) *pu4Para2 = FALSE;
                }
            }
            return TRUE;
        }
    }
    else if (u1Type == eSWDMX_GET_CURRENT_TIME)
    {
        if (pu4Para1 == NULL)
        {
            LOG(1, "_SWDMX_WMVGetInfo pu4Para1 = NULL\n");
            return FALSE;
        }

        *pu4Para1 = STC_GetStcValue(prSwdmxInst->u1StcId);
        
        return TRUE;
    }
    else if(u1Type == eSWDMX_GET_AUDIO_ACTIVE_PIDX)
    {
        UINT32 u4AudFifoAddr, u4AudFifoSize,u4DmxAvailSize;
        _SWDMX_GetAudioFifo(prSwdmxInst->u1SwdmxId, &u4AudFifoAddr, &u4AudFifoSize);
        u4DmxAvailSize = DMX_MUL_GetEmptySize(prSwdmxInst->u1DmxId, 
                                                                        (DMX_PID_TYPE_T)DMX_PID_TYPE_ES_AUDIO,
                                                                        prSwdmxInst->rFmtInfo.rSwdmxWmvInfo.u1AudPid);
        
        *pu4Para1 = u4AudFifoSize - u4DmxAvailSize;
        return TRUE;
    }
    return FALSE;
}

BOOL _SWDMX_WMVAudioCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
#if 0
    SWDMX_INFO_T *prSwdmxInst = (SWDMX_INFO_T*)pvSwdmxInst;
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);
#endif
    SWDMX_INFO_T* prSwdmxInst = _SWDMX_GetInst(u1SrcId);
    SWDMX_WMV_VERIFY_NULL_ERROR(prSwdmxInst);

#ifdef WAIT_AUDIO_READY
    LOG(7, "WMVAudioCb %d\n", eAudioNotifyType);
    if (eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
    {
        LOG(1, "ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE is %d\n", ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE);
        if (_rSwdmxWmvInfo.fgEnVideo)
        {
            VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, _rSwdmxWmvInfo.u4TriggerAudioPTS);
            LOG(0, "!!!!!! audio is ready (video_paused? %d)\n", _rSwdmxWmvInfo.fgVideoPause);
            if (_rSwdmxWmvInfo.fgVideoPause)
            {
                _WMVStartToPlay(prSwdmxInst);
                LOG(1, "_WMVStartToPlay for AudioCb\n");
            }
            _rSwdmxWmvInfo.fgAudIsReady = TRUE;
        }
#ifdef CC_MMP_BAD_INTERLEAVED_FILE_SUPPORT
        else if (prSwdmxInst->fgIsBadInterleaved)
        {
            VDP_TriggerAudReceive(prSwdmxInst->u1B2rId, _rSwdmxWmvInfo.u4TriggerAudioPTS);
            LOG(0,"!!!!!! audio is ready %d\n",_rSwdmxWmvInfo.fgVideoPause);
            if (_rSwdmxWmvInfo.fgVideoPause)
            {
                _WMVStartToPlay(prSwdmxInst);
                LOG(0, "_WMVStartToPlay for AudioCb2\n");
            }
        }
#endif
        else
        {
            LOG(0,"!!!Play WMA Start STC\n");
            STC_StartStc(prSwdmxInst->u1StcId);
            AUD_MMAoutEnable(prSwdmxInst->u1AdecId, TRUE);
        }
    }
    else
    {
        //impossible
        //ASSERT(0);
    }
#endif
    return TRUE;
}

