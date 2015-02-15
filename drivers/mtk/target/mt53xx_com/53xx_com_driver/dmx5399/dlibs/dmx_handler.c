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
 * $RCSfile: dmx_handler.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_handler.c
 *  Demux driver - decoder interfaces
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_mm.h"
#include "dmx_debug.h"
#include "dmx_if.h"
#include "fvr.h"
#include "tz_if.h"

LINT_EXT_HEADER_BEGIN

#include "vdec_if.h"
#include "mpv_drvif.h"
#include "aud_drvif.h"
#include "drv_common.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_hal_arm.h"

LINT_EXT_HEADER_END


#ifdef TIME_MEASUREMENT
LINT_EXT_HEADER_BEGIN
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
LINT_EXT_HEADER_END
extern BOOL _fgWaitForFirstI;   // Measure the time from "Enabling PID" to the
                                // moment "the first I frame is received".
#else
extern BOOL _fgWaitForFirstI;
#endif  // TIME_MEASUREMENT


#if defined(CC_DMX_FRAMER_MONITOR) && !defined(CC_DMX_ENABLE_MONTHREAD)
#error "Defein CC_DMX_FRAMER_MONITOR but not define CC_DMX_ENABLE_MONTHREAD.\n"
#endif // CC_DMX_FRAMER_MONITOR

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

// Demux queue and thread paramenters
//
#define DMX_QUEUE_NAME                  "DMX"
#define DMX_PURGE_QUEUE_NAME            "PURGE"
#define DMX_PURGE_THREAD_NAME           "PURGETHREAD"
#define DMX_QUEUE_SIZE                  1024//32

#ifdef CC_DMX_ENABLE_MONTHREAD
#define DMX_MON_THREAD_NAME             "DMXMONTH"
#define DMX_MON_THREAD_STACK_SIZE       1024
#define DMX_MON_THREAD_PRIORITY         100
#define DMX_MON_TIME_PERIOD             100         //  Monitor period
#define DMX_MON_RCV_THRESHOLD           30
#define DMX_MON_DROP_THRESHOLD          100
#endif

//
// Picture header attribytes
//

// MPEG Picture start codes
#define PIC_START_CODE_1                    0
#define PIC_START_CODE_2                    0
#define PIC_START_CODE_3                    1
#define PIC_START_CODE_PIC                  0
#define PIC_START_CODE_SEQ_START            0xb3
#define PIC_START_CODE_SEQ_END              0xb7
#define PIC_START_CODE_GOP                  0xb8

// H264 Picture start codes
#define H264_PIC_START_CODE_1               0x00
#define H264_PIC_START_CODE_2               0x00
#define H264_PIC_START_CODE_3               0x01
#define H264_PIC_START_NONIDR               0x01
#define H264_PIC_START_IDR                  0x05
#define H264_PIC_START_SEI                  0x06
#define H264_PIC_START_SEQ                  0x07
#define H264_PIC_START_PIC                  0x08
#define H264_PIC_START_AU                   0x09
#define H264_PIC_START_END_OF_SEQ           0x0a
#define H264_PIC_START_PREFIX               0x0e
#define H264_PIC_START_SUB_SPS              0x0f
#define H264_PIC_START_CODED_SLICE_EXT      0x14

//special for CMI 4K 2K
#define H264_PIC_START_CMI                  0x1F


// VC1 Picture start codes
#define VC1_PIC_START_CODE_1               0x00
#define VC1_PIC_START_CODE_2               0x00
#define VC1_PIC_START_CODE_3               0x01
#define VC1_PIC_START_CODE_FIELD           0x0C
#define VC1_PIC_START_CODE_FRAME           0x0D
#define VC1_PIC_START_CODE_ENTRY           0x0E
#define VC1_PIC_START_CODE_SEQ             0x0F

// MPEG4 Picture start codes
#define MPEG4_PIC_START_CODE_1              0x00
#define MPEG4_PIC_START_CODE_2              0x00
#define MPEG4_PIC_START_CODE_3_1            0x01
#define MPEG4_PIC_START_CODE_3_2            0x80
#define MPEG4_PIC_START_CODE_PIC            0xB6

// AVS Picture start codes
#define AVS_PIC_START_CODE_1                0x00
#define AVS_PIC_START_CODE_2                0x00
#define AVS_PIC_START_CODE_3                0x01
#define AVS_PIC_START_CODE_SEQ_START        0xB0
#define AVS_PIC_START_CODE_SEQ_END          0xB1
#define AVS_PIC_START_CODE_SEQ_USR_DATA     0xB2
#define AVS_PIC_START_CODE_SEQ_PIC_I        0xB3
#define AVS_PIC_START_CODE_SEQ_EXT          0xB5
#define AVS_PIC_START_CODE_SEQ_PIC_PB       0xB6
#define AVS_PIC_START_CODE_SEQ_VID_EDIT     0xB7

// Hardware picture types
#define PIC_HW_TYPE_PIC                     0
#define PIC_HW_TYPE_SEQ_START               1
#define PIC_HW_TYPE_SEQ_END                 2
#define PIC_HW_TYPE_GOP                     3
#define PIC_HW_TYPE_UNKNOWN                 7

#if defined(CC_MVC_SUPPORT) || defined(CC_SW_MVC_ENABLE)
#define PIC_HEADER_LEN                      13
#else
#define PIC_HEADER_LEN                      10
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
enum tagFrameCodingMode
{
  PROGRESSIVE = 0,
  INTERLACEFRAME,
  INTERLACEFIELD
};


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

/// Picture queue for MPEG decoder
static HANDLE_T _hDmxQueue = NULL_HANDLE;
static HANDLE_T _hPurgeQueue = NULL_HANDLE;

/// Picture thread
static HANDLE_T _hDmxThread = NULL_HANDLE;
static HANDLE_T _hPurgeThread = NULL_HANDLE;

#ifdef CC_DMX_ENABLE_MONTHREAD
/// Monitor thread
static HANDLE_T _hDmxMonThread = NULL_HANDLE;
#endif  // CC_DMX_ENABLE_MONTHREAD

/// Sequence header and GOP flags
static BOOL _afgSeqHeader[DMX_NUM_PID_INDEX];
static UINT32 _au4SeqHeaderAddr[DMX_NUM_PID_INDEX];
static BOOL _afgSeqHeaderPtsDts[DMX_NUM_PID_INDEX];
static UINT32 _au4SeqHeaderPts[DMX_NUM_PID_INDEX];
static UINT32 _au4SeqHeaderDts[DMX_NUM_PID_INDEX];
static BOOL _afgGop[DMX_NUM_PID_INDEX];
static UINT32 _au4GopAddr[DMX_NUM_PID_INDEX];
static BOOL _afgGopPtsDts[DMX_NUM_PID_INDEX];
static UINT32 _au4GopPts[DMX_NUM_PID_INDEX];
static UINT32 _au4GopDts[DMX_NUM_PID_INDEX];
static BOOL _afgSeqEnd[DMX_NUM_PID_INDEX];
static BOOL _afgSeqEndPtsDts[DMX_NUM_PID_INDEX];
static UINT32 _au4SeqEndPts[DMX_NUM_PID_INDEX];
static UINT32 _au4SeqEndDts[DMX_NUM_PID_INDEX];

#ifdef TRACK_LAST_FRAME
/// Last sent frame (video picture or audio frame)
static UINT32 _au4LastFrame[DMX_NUM_PID_INDEX];
#endif

static UINT8 _u1H264SliceNum = 0;

#ifdef ENABLE_MULTIMEDIA
static UINT8 _u1vopFirstFieldType[8] =
{ VC1_PIC_TYPE_I, VC1_PIC_TYPE_I, VC1_PIC_TYPE_P, VC1_PIC_TYPE_P,
  VC1_PIC_TYPE_B, VC1_PIC_TYPE_B, VC1_PIC_TYPE_BI, VC1_PIC_TYPE_BI };
static UINT8 _u1vopSecondFieldType[8] =
{ VC1_PIC_TYPE_I, VC1_PIC_TYPE_P, VC1_PIC_TYPE_I, VC1_PIC_TYPE_P,
  VC1_PIC_TYPE_B, VC1_PIC_TYPE_BI, VC1_PIC_TYPE_B, VC1_PIC_TYPE_BI };
#endif  // ENABLE_MULTIMEDIA

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

#ifdef TRACK_LAST_FRAME

//-----------------------------------------------------------------------------
/** _DmxGetLastFrame
 *  Get last sent frame
 *
 *  @param  u1Pidx          PID index
 *
 *  @retval The last frame
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxGetLastFrame(UINT8 u1Pidx)
{
    CRIT_STATE_T rState;
    UINT32 u4LastFrame;

    ASSERT(u1Pidx < DMX_NUM_PID_INDEX);

    rState = x_crit_start();
    u4LastFrame = _au4LastFrame[u1Pidx];
    x_crit_end(rState);

    return u4LastFrame;
}


//-----------------------------------------------------------------------------
/** _DmxSetLastFrame
 *  Set last frame of a given PID
 *
 *  @param  u1Pidx          PID index
 *  @param  u4LastFrame     The new last frame
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _DmxSetLastFrame(UINT8 u1Pidx, UINT32 u4LastFrame)
{
    CRIT_STATE_T rState;

    ASSERT(u1Pidx < DMX_NUM_PID_INDEX);

    rState = x_crit_start();
    _au4LastFrame[u1Pidx] = u4LastFrame;
    x_crit_end(rState);
}

#endif


//-----------------------------------------------------------------------------
/** _DmxDispatchPesMessage
 *  Dispatch PES message to corresponding decoders
 *
 *  @param  prPes           PES message
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _DmxDispatchPesMessage(const DMX_PES_MSG_T* prPes)
{
    BOOL fgRet = FALSE;
    PID_STRUCT_T* prPidStruct;
    DMX_DECODER_CALLBACKS_T *prCallback;

    ASSERT(prPes != NULL);

    prPidStruct = _DMX_GetPidStruct(prPes->u1Pidx);
    if(prPidStruct == NULL)
    {
        return FALSE;
    }

    #ifndef CC_DMX_FLUSH_ALL
    if(prPes->eVideoType != DMX_VIDEO_H264)
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prPes->u4BufStart), prPes->u4BufEnd - prPes->u4BufStart);
    }
    #endif

    if ((_fgWaitForFirstI == TRUE) && (prPes->u4FrameType == PIC_TYPE_I))
    {
    #ifdef TIME_MEASUREMENT
        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "DMX_First_I");
		TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "DMX_First_I");
	#endif
	    DMXPIDLOG(DMX_LOG_AV 1, prPes->u1Pidx, "(%u) Find I-Framer For V-PID.\r\n",  prPes->u1Pidx);
        _fgWaitForFirstI = FALSE;
    }

    if ((_fgWaitForFirstI == TRUE) && (prPes->u4FrameType == H264_PIC_TYPE_SEQ))
    {
        // Normally, the I frame appears right after SEQ.
     #ifdef TIME_MEASUREMENT
        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "DMX_First_SEQ");
		TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "DMX_First_SEQ");
	 #endif
	   DMXPIDLOG(DMX_LOG_AV 1, prPes->u1Pidx, "(%u) Find H264 SEQ For V-PID.\r\n",  prPes->u1Pidx);
        _fgWaitForFirstI = FALSE;
    }



    switch (prPes->ePidType)
    {
    case DMX_PID_TYPE_ES_VIDEO:
        prCallback = _DMX_MUL_GetPidxDecoderCallback(prPes->u1Pidx);
        if((prCallback != NULL) && (prCallback->pfnSendVideoPes != NULL))
        {
            _DMX_TraceLog((UINT32)_DmxDispatchPesMessage, "_DmxDispatchPesMessage",
                        (UINT32)(prCallback->pfnSendVideoPes), "(prCallback->pfnSendVideoPes)",
                        DMX_TRACE_USE_LOCK | DMX_TRACE_FUNCTION_CALL);

            if (prCallback->pfnSendVideoPes(prPes))
            {
                // Send successful
                fgRet = TRUE;
            }
            else
            {
                // Queue full
                LOG(2, "Failed to send video PES!\n");
            }

            _DMX_TraceLog((UINT32)_DmxDispatchPesMessage, "_DmxDispatchPesMessage",
                        (UINT32)(prCallback->pfnSendVideoPes), "(prCallback->pfnSendVideoPes)",
                        DMX_TRACE_USE_LOCK | DMX_TRACE_FUNCTION_RETURN);
        }
        else
        {
            LOG(2, "%s - NULL callback!\n", __FUNCTION__);
        }
        break;

    case DMX_PID_TYPE_ES_AUDIO:
        // TBD
        break;

    default:
        // Unknown type
        ASSERT(FALSE);
        break;
    }

#ifdef TRACK_LAST_FRAME

    // Record last sent frame
    if (fgRet)
    {
        _DmxSetLastFrame(prPes->u1Pidx, prPes->uFrameAddr);
    }

#endif

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DmxDiscardFifoData
 *  Discard all data in FIFO
 *
 *  @param  u1Pidx          PID index
 *  @param  fgReenable      Re-enable PID
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _DmxDiscardFifoData(UINT8 u1Pidx, BOOL fgReenable)
{
    PID_STRUCT_T* prPidStruct;
    UINT8 u1Channel;

    DMXPIDLOG(DMX_LOG_PID DMX_LOG_BUFFER 6, u1Pidx, "Pidx %u: discard fifo data!\n", u1Pidx);

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    if (prPidStruct->fgEnable)
    {
        LOG(5, "Ignore discarding FIFO data of an active pidx %u\n", u1Pidx);
        return FALSE;
    }

    // Flush decoder
    switch (prPidStruct->ePidType)
    {
    case DMX_PID_TYPE_ES_VIDEO:
        if (_DMX_GetVideoChannel(u1Pidx, &u1Channel))
        {
            _DMX_TraceLog((UINT32)_DmxDiscardFifoData, "_DmxDiscardFifoData",
                        (UINT32)MPV_FlushEsmQ, "MPV_FlushEsmQ",
                        DMX_TRACE_USE_LOCK | DMX_TRACE_FUNCTION_CALL);

            if (prPidStruct->eScrambleStatus == DMX_SCRAMBLE_STATE_SCRAMBLED &&
                prPidStruct->fgClearToScrm)
            {
                VDEC_SuperFlush(u1Channel);
                prPidStruct->fgClearToScrm = FALSE;
            }
            else
            {
                MPV_FlushEsmQ(u1Channel, FALSE);
            }

            _DMX_TraceLog((UINT32)_DmxDiscardFifoData, "_DmxDiscardFifoData",
                        (UINT32)MPV_FlushEsmQ, "MPV_FlushEsmQ",
                        DMX_TRACE_USE_LOCK | DMX_TRACE_FUNCTION_RETURN);
        }
        break;

    case DMX_PID_TYPE_ES_AUDIO:
        _DMX_TraceLog((UINT32)_DmxDiscardFifoData, "_DmxDiscardFifoData",
                    (UINT32)AUD_WaitDspFlush, "AUD_WaitDspFlush",
                    DMX_TRACE_USE_LOCK | DMX_TRACE_FUNCTION_CALL);

        AUD_WaitDspFlush(prPidStruct->u1DeviceId);

        _DMX_TraceLog((UINT32)_DmxDiscardFifoData, "_DmxDiscardFifoData",
                    (UINT32)AUD_WaitDspFlush, "AUD_WaitDspFlush",
                    DMX_TRACE_USE_LOCK | DMX_TRACE_FUNCTION_RETURN);
        break;

    default:
        LOG(5, "Pidx %u discard fifo data: unknown pid type %u\n",
            u1Pidx, (UINT32)prPidStruct->ePidType);
        break;
    }

#ifdef TRACK_LAST_FRAME

    // Reset last-sent frame record
    _DmxSetLastFrame(u1Pidx, 0);

#endif

    if (fgReenable)
    {
        DMX_PID_T rPid;

        // Reset FIFO by simply re-enable PID
        rPid.fgEnable = TRUE;
        VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, TRUE));
    }
    else
    {
        if (!_DMX_ResetPidBuffer(u1Pidx, prPidStruct, FALSE))
        {
            return FALSE;
        }
    }

    return TRUE;
}

#if !defined(CC_TRUSTZONE_SUPPORT) || !defined(CC_SVP_SUPPORT)
//-----------------------------------------------------------------------------
/** _DmxGetPicHeader
 *  Get picture header and modify start code from 0x000001 to 0xFF0001 
 *  for specific video type.
 *
 *  @param  u4Addr          picture address
 *  @param  u4BufStart      buffer start address
 *  @param  u4BufEnd        buffer end address
 *  @param  pu1Buf          the dest buffer
 *  @param  u4Size          picture header size
 *  @param  eVideoType      video type 
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _DmxGetPicHeader(UINT32 u4Addr, UINT32 u4BufStart, UINT32 u4BufEnd,
                            UINT8 *pu1Buf, UINT32 u4Size, DMX_VIDEO_TYPE_T eVideoType)
{
    static DMX_VIDEO_STARTCODE_T _arVideoStartCode[] =
    {
        {DMX_VIDEO_AVS, {0x1, 0xB3, 0x0, 0x0}, {0xFF, 0xFF, 0x0, 0x0}},
        {DMX_VIDEO_AVS, {0x1, 0xB6, 0x0, 0x0}, {0xFF, 0xFF, 0x0, 0x0}}
    };
    
    UINT32 u4PatternSize, u4PatternNum, i, j;
    UINT8 au1DataBuf[DMX_STARTCODE_BUFFER_SIZE];

    u4PatternSize = DMX_STARTCODE_PATTERN_SIZE;
    u4PatternNum = sizeof(_arVideoStartCode)/sizeof(DMX_VIDEO_STARTCODE_T);
    
    // Check if the header is across FIFO boundary
    if ((u4Addr + DMX_STARTCODE_BUFFER_SIZE) > u4BufEnd)
    {
        UINT32 u4Len = u4BufEnd - u4Addr;

        HalInvalidateDCacheMultipleLine(u4Addr, u4Len);
        HalInvalidateDCacheMultipleLine(u4BufStart, (DMX_STARTCODE_BUFFER_SIZE - u4Len));
        memcpy((void*)au1DataBuf, (void*)u4Addr, u4Len);
        memcpy((void*)(au1DataBuf + u4Len), (void*)u4BufStart, (DMX_STARTCODE_BUFFER_SIZE - u4Len));

        if ((au1DataBuf[0] == 0x0) && (au1DataBuf[1] == 0x0))
        {
            UINT8 *pu1Pattern, *pu1Mask;
        
            for (i = 0; i < u4PatternNum; i++)
            {
                if (eVideoType != _arVideoStartCode[i].eVideoType)
                {
                    continue;
                }
                
                pu1Pattern = _arVideoStartCode[i].au1Pattern;
                pu1Mask = _arVideoStartCode[i].au1Mask;

                for (j = 0; j < u4PatternSize; j++)
                {
                    if ((au1DataBuf[j+2] & pu1Mask[j]) != (pu1Pattern[j] & pu1Mask[j]))
                    {
                        break;
                    }
                }
                
                if (j == u4PatternSize)
                {
                    *((UINT8 *)u4Addr) = 0xFF;
                    break;
                }
            }
        }
        
        HalFlushInvalidateDCacheMultipleLine(u4Addr, u4Len);
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, DMX_STARTCODE_BUFFER_SIZE);
        memcpy((void*)au1DataBuf, (void*)u4Addr, DMX_STARTCODE_BUFFER_SIZE);
        
        if ((au1DataBuf[0] == 0x0) && (au1DataBuf[1] == 0x0))
        {
            UINT8 *pu1Pattern, *pu1Mask;
        
            for (i = 0; i < u4PatternNum; i++)
            {
                if (eVideoType != _arVideoStartCode[i].eVideoType)
                {
                    continue;
                }
                
                pu1Pattern = _arVideoStartCode[i].au1Pattern;
                pu1Mask = _arVideoStartCode[i].au1Mask;

                for (j = 0; j < u4PatternSize; j++)
                {
                    if ((au1DataBuf[j+2] & pu1Mask[j]) != (pu1Pattern[j] & pu1Mask[j]))
                    {
                        break;
                    }
                }
                
                if (j == u4PatternSize)
                {
                    *((UINT8 *)u4Addr) = 0xFF;
                    break;
                }
            }
        }

        HalFlushInvalidateDCacheMultipleLine(u4Addr, DMX_STARTCODE_BUFFER_SIZE);
    }
   
    if (u4Size <= DMX_STARTCODE_BUFFER_SIZE)
    {
        memcpy((void*)pu1Buf, (void*)au1DataBuf, u4Size);
    }
    else
    {
        // Check if the header is across FIFO boundary
        if ((u4Addr + u4Size) > u4BufEnd)
        {
            UINT32 u4Len = u4BufEnd - u4Addr;
            
            HalInvalidateDCacheMultipleLine(u4Addr, u4Len);
            HalInvalidateDCacheMultipleLine(u4BufStart, (u4Size - u4Len));
            memcpy((void*)pu1Buf, (void*)u4Addr, u4Len);
            memcpy((void*)(pu1Buf + u4Len), (void*)u4BufStart, (u4Size - u4Len));
        }
        else
        {
            HalInvalidateDCacheMultipleLine(u4Addr, u4Size);
            memcpy((void*)pu1Buf, (void*)u4Addr, u4Size);
        }
    }   

    return TRUE;
}
#endif

//-----------------------------------------------------------------------------
/** _DmxDetectPictureType
 *  Detect picture type at a given address
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 *
 *  @retval Picture type:
 *                          PIC_TYPE_UNKNOWN
 *                          PIC_TYPE_I
 *                          PIC_TYPE_P
 *                          PIC_TYPE_B
 *                          PIC_TYPE_SEQ_START
 *                          PIC_TYPE_GOP
 *                          PIC_TYPE_SEQ_END
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectMPEGPictureType(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT32 u4PicType;
    UINT8* p = NULL;
    UINT8 u1Type, h[PIC_HEADER_LEN], i;

    if ((u4Addr < u4BufStart) || (u4Addr >= u4BufEnd))
    {
        LOG(3, "Invalid picture address 0x%08x, pidx: %u\n");
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(!TZ_DMX_GetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, PIC_HEADER_LEN))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }
    
    p = h;
    #else
    // Check if the header is across FIFO boundary
    p = (UINT8*)u4Addr;
    if (((u4Addr + PIC_HEADER_LEN) - 1) >= u4BufEnd)
    {
        UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
        if(u1Len >= PIC_HEADER_LEN)
        {
            u4PicType = PIC_TYPE_UNKNOWN;
            goto _exit;
        }
        HalInvalidateDCacheMultipleLine(u4Addr, u1Len);
        HalInvalidateDCacheMultipleLine(u4BufStart, (PIC_HEADER_LEN - u1Len));
        x_memcpy((void*)h, (void*)p, u1Len);
        x_memcpy((void*)(h + u1Len), (void*)u4BufStart,
            (SIZE_T)(PIC_HEADER_LEN - u1Len));
        p = h;
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, PIC_HEADER_LEN);
    }
    #endif

    // Check start code
    if ((p[0] != PIC_START_CODE_1) || (p[1] != PIC_START_CODE_2) ||
        (p[2] != PIC_START_CODE_3))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    // Check picture type
    u1Type = p[3];
    u4PicType = PIC_TYPE_UNKNOWN;

    for(i = 0; i < DMX_PICINFO_SIZE; i++)
    {
        pu1PicInfo[i] = p[DMX_PICINFO_OFFSET + i];
    }

#ifdef CC_VDEC_FMT_DETECT
    // H264
    if((u1Type & 0x1F) == H264_PIC_START_NONIDR)
    {
        if((p[4] & 0x80) == 0x80)
        {
            u4PicType = H264_PIC_TYPE_NONIDR;
        }
        goto _exit;
    }

    if((u1Type & 0x1F) == H264_PIC_START_SEQ)
    {
        if((p[5] & 0xF) == 0)
        {
            u4PicType = H264_PIC_TYPE_SEQ;
        }
        goto _exit;
    }
#endif  // CC_VDEC_FMT_DETECT

    // MPEG
    switch (u1Type)
    {
    case PIC_START_CODE_SEQ_START:
        u4PicType = PIC_TYPE_SEQ_START;
        break;

    case PIC_START_CODE_GOP:
        u4PicType = PIC_TYPE_GOP;
        break;

    case PIC_START_CODE_SEQ_END:
        u4PicType = PIC_TYPE_SEQ_END;
        break;

    case PIC_START_CODE_PIC:
        u4PicType = (p[5] >> 3) & 0x7;
        if ((u4PicType != PIC_TYPE_I) && (u4PicType != PIC_TYPE_P) &&
            (u4PicType != PIC_TYPE_B))
        {
            u4PicType = PIC_TYPE_UNKNOWN;
        }
        break;

    default:
        break;
    }

_exit:

    return u4PicType;
}


//-----------------------------------------------------------------------------
/** _DmxDetectH264PictureType
 *  Detect picture type at a given address
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectH264PictureType(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT32 u4PicType;
    UINT8* p = NULL;
    UINT8 u1Type, h[PIC_HEADER_LEN], i;

    // static UINT32 u4Count = 0;

    if ((u4Addr < u4BufStart) || (u4Addr >= u4BufEnd))
    {
        LOG(3, "Invalid picture address 0x%08x, pidx: %u\n");

        return PIC_TYPE_UNKNOWN;
    }

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(!TZ_DMX_GetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, PIC_HEADER_LEN))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    p = h;
    #else
    // Check if the header is across FIFO boundary
    p = (UINT8*)u4Addr;
    if (((u4Addr + PIC_HEADER_LEN) - 1) >= u4BufEnd)
    {
        UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
        if(u1Len >= PIC_HEADER_LEN)
        {
            u4PicType = H264_PIC_TYPE_UNKNOWN;
            goto _exit;
        }
        HalInvalidateDCacheMultipleLine(u4Addr, u1Len);
        HalInvalidateDCacheMultipleLine(u4BufStart, (PIC_HEADER_LEN - u1Len));
        x_memcpy((void*)h, (void*)p, u1Len);
        x_memcpy((void*)(h + u1Len), (void*)u4BufStart,
            (SIZE_T)(PIC_HEADER_LEN - u1Len));
        p = h;
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, PIC_HEADER_LEN);
    }
    #endif

    // Check start code
    if ((p[0] != H264_PIC_START_CODE_1) || (p[1] != H264_PIC_START_CODE_2) ||
        (p[2] != H264_PIC_START_CODE_3))
    {
        u4PicType = H264_PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    // Check picture type
    u1Type = p[3];
    u4PicType = H264_PIC_TYPE_UNKNOWN;

    for(i = 0; i < DMX_PICINFO_SIZE; i++)
    {
        pu1PicInfo[i] = p[DMX_PICINFO_OFFSET + i];
    }

#ifdef CC_VDEC_FMT_DETECT
    if(u1Type == PIC_START_CODE_PIC)
    {
        u4PicType = (p[5] >> 3) & 0x7;
        if ((u4PicType != PIC_TYPE_I) && (u4PicType != PIC_TYPE_P))
        {
            u4PicType = H264_PIC_TYPE_UNKNOWN;
        }

        goto _exit;
    }
#endif  // CC_VDEC_FMT_DETECT

    switch (u1Type & 0x1F)
    {
    case H264_PIC_START_NONIDR:
        if((p[4] & 0x80) == 0x80)
        {
            u4PicType = H264_PIC_TYPE_NONIDR;
        }
        else
        {
            _u1H264SliceNum++;
        }
        break;

    case H264_PIC_START_IDR:
        if((p[4] & 0x80) == 0x80)
        {
            u4PicType = H264_PIC_TYPE_IDR;
        }
        else
        {
            _u1H264SliceNum++;
        }
        break;

    case H264_PIC_START_SEI:
        u4PicType = H264_PIC_TYPE_SEI;
        break;

    case H264_PIC_START_SEQ:
        u4PicType = H264_PIC_TYPE_SEQ;
        break;

    case H264_PIC_START_PIC:
        u4PicType = H264_PIC_TYPE_PIC;
        break;

    case H264_PIC_START_AU:
        u4PicType = H264_PIC_TYPE_AU;
        break;
#if 0 //we don't need to return this type now
    case H264_PIC_START_END_OF_SEQ:
        u4PicType = H264_PIC_START_END_OF_SEQ;
        break;
#endif

#if defined(CC_MVC_SUPPORT) || defined(CC_SW_MVC_ENABLE)
    case H264_PIC_START_PREFIX:
        if((p[12] & 0x80) == 0x80)
        {
            u4PicType = H264_PIC_TYPE_PREFIX;
        }
        break;

    case H264_PIC_START_SUB_SPS:
        u4PicType = H264_PIC_TYPE_SUB_SPS;
        break;

    case H264_PIC_START_CODED_SLICE_EXT:
        // we detect this picture type by  { 0x011400000080, 0xff1f00000080 }
        if((p[7] & 0x80) == 0x80)
        {
            u4PicType = H264_PIC_TYPE_CODED_SLICE_EXT;
        }
        break;

	case H264_PIC_START_CMI:
		u4PicType = H264_PIC_TYPE_CMI;
		break;
		
#endif // CC_MVC_SUPPORT

    default:
        //Printf("Unknown #3\n");
        break;
    }

_exit:
    //Printf("H264 pic type: %d, idx:%d\n", u4PicType, u4Count++);

    return u4PicType;
}

#ifdef ENABLE_MULTIMEDIA
//-----------------------------------------------------------------------------
/** _DmxDetectMPEG4PictureType
 *  Detect picture type at a given address
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectMPEG4PictureType(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT32 u4PicType;
    UINT8* p = NULL;
    UINT8 u1Type, h[PIC_HEADER_LEN], i;

    // static UINT32 u4Count = 0;

    if ((u4Addr < u4BufStart) || (u4Addr >= u4BufEnd))
    {
        LOG(3, "Invalid picture address 0x%08x, pidx: %u\n");

        return PIC_TYPE_UNKNOWN;
    }

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(!TZ_DMX_GetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, PIC_HEADER_LEN))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    p = h;
    #else
    // Check if the header is across FIFO boundary
    p = (UINT8*)u4Addr;
    if (((u4Addr + PIC_HEADER_LEN) - 1) >= u4BufEnd)
    {
        UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
        if(u1Len >= PIC_HEADER_LEN)
        {
            u4PicType = MPEG4_PIC_TYPE_UNKNOWN;
            goto _exit;
        }
        HalInvalidateDCacheMultipleLine(u4Addr, u1Len);
        HalInvalidateDCacheMultipleLine(u4BufStart, (PIC_HEADER_LEN - u1Len));
        x_memcpy((void*)h, (void*)p, u1Len);
        x_memcpy((void*)(h + u1Len), (void*)u4BufStart,
            (SIZE_T)(PIC_HEADER_LEN - u1Len));
        p = h;
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, PIC_HEADER_LEN);
    }
    #endif

    // Check start code
    if ((p[0] != MPEG4_PIC_START_CODE_1) || (p[1] != MPEG4_PIC_START_CODE_2) ||
        ((p[2] != MPEG4_PIC_START_CODE_3_1) && (p[2] != MPEG4_PIC_START_CODE_3_2)))
    {
        u4PicType = MPEG4_PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    // Check picture type
    u1Type = p[3];
    u4PicType = MPEG4_PIC_TYPE_OTHERS;

    for(i = 0; i < DMX_PICINFO_SIZE; i++)
    {
        pu1PicInfo[i] = p[DMX_PICINFO_OFFSET + i];
    }

    switch (u1Type & 0xFF)
    {
    case MPEG4_PIC_START_CODE_PIC:
        u4PicType = MPEG4_PIC_TYPE_PIC;
        break;

    default:
        // Don't care others
        break;
    }

_exit:
    
    return u4PicType;
}

//-----------------------------------------------------------------------------
/** GetHiBit
 *  @param  pu1data          The data for getting one bit
 */
//-----------------------------------------------------------------------------
static UINT8 _DmxGetHiBit(UINT8 *pu1data)
{
    UINT8 ret = 0;

    DMX_PANIC(pu1data != NULL);
    DMX_PANIC_RET(0);

    if(pu1data == NULL)     // klocwork
    {
        return 0;
    }

    ret = (*pu1data >> 7) & 0x1;
    *pu1data = *pu1data << 1;
    return ret;
}


//-----------------------------------------------------------------------------
/** _DmxDetectVC1PictureType
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectVC1PictureType(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT32 u4PicType;
    UINT8* p = NULL;
    UINT8 u1Type, h[PIC_HEADER_LEN] = {0}, u1BitField, i;
    UINT8 u1FrameCodingMode;
    static BOOL fgInterlacedSource = 0;
    static INT32 i4CurrentTemporalField = 0; // 0:1st field or frame picture, 1: 2nd field
    static UINT8 u1SecondFieldType = 0;

    if ((u4Addr < u4BufStart) || (u4Addr >= u4BufEnd))
    {
        LOG(3, "Invalid picture address 0x%08x, pidx: %u\n");

        return PIC_TYPE_UNKNOWN;
    }

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(!TZ_DMX_GetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, PIC_HEADER_LEN))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }    

    p = h;
    #else
    // Check if the header is across FIFO boundary
    p = (UINT8*)u4Addr;
    if (((u4Addr + PIC_HEADER_LEN) - 1) >= u4BufEnd)
    {
        UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
        if(u1Len >= PIC_HEADER_LEN)
        {
            u4PicType = VC1_PIC_TYPE_UNKNOWN;
            goto _exit;
        }
        HalInvalidateDCacheMultipleLine(u4Addr, u1Len);
        HalInvalidateDCacheMultipleLine(u4BufStart, (PIC_HEADER_LEN - u1Len));
        x_memcpy((void*)h, (void*)p, u1Len);
        x_memcpy((void*)(h + u1Len), (void*)u4BufStart,
            (SIZE_T)(PIC_HEADER_LEN - u1Len));
        p = h;
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, PIC_HEADER_LEN);
    }
    #endif

    // Check start code
    if ((p[0] != VC1_PIC_START_CODE_1) || (p[1] != VC1_PIC_START_CODE_2) ||
        (p[2] != VC1_PIC_START_CODE_3))
    {
        u4PicType = VC1_PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    u1Type = p[3];
    u1BitField = p[4];

    for(i = 0; i < DMX_PICINFO_SIZE; i++)
    {
        pu1PicInfo[i] = p[DMX_PICINFO_OFFSET + i];
    }

    u4PicType = VC1_PIC_TYPE_UNKNOWN;
    switch (u1Type & 0xFF)
    {
    case VC1_PIC_START_CODE_SEQ:
        fgInterlacedSource = ((p[9] & 0x40) == 0x40) ? 1 : 0;
        u4PicType = VC1_PIC_TYPE_SEQ_START;
        break;

    case VC1_PIC_START_CODE_ENTRY:
        u4PicType = VC1_PIC_TYPE_ENTRY;
        break;

    case VC1_PIC_START_CODE_FIELD:
    case VC1_PIC_START_CODE_FRAME:
        if( i4CurrentTemporalField == 1 )
        {
            u1FrameCodingMode = (UINT8)INTERLACEFIELD;
        }
        else
        {
            if (fgInterlacedSource)
            {
                if (0 == _DmxGetHiBit(&u1BitField))
                {
                    u1FrameCodingMode = (UINT8)PROGRESSIVE;
                }
                else
                {
                    if (0 == _DmxGetHiBit(&u1BitField))
                    {
                        u1FrameCodingMode = (UINT8)INTERLACEFRAME;
                    }
                    else
                    {
                        u1FrameCodingMode = (UINT8)INTERLACEFIELD;
                    }
                }
            }
            else
            {
                u1FrameCodingMode = (UINT8)PROGRESSIVE;
            }
        }
        if ((u1FrameCodingMode == (UINT8)PROGRESSIVE) ||
            (u1FrameCodingMode == (UINT8)INTERLACEFRAME))
        {
            if (0 == _DmxGetHiBit(&u1BitField))
            {
                u4PicType = VC1_PIC_TYPE_P;
            }
            else if (0 == _DmxGetHiBit(&u1BitField))
            {
                u4PicType = VC1_PIC_TYPE_B;
            }
            else if (0 == _DmxGetHiBit(&u1BitField))
            {
                u4PicType = VC1_PIC_TYPE_I;
            }
            else if (0 == _DmxGetHiBit(&u1BitField))
            {
                u4PicType = VC1_PIC_TYPE_BI;
            }
            else
            {
                u4PicType = VC1_PIC_TYPE_SKIP;
            }
        }
        else if(u1FrameCodingMode == (UINT8)INTERLACEFIELD)
        {
            if(i4CurrentTemporalField == 0)
            {
                UINT32 u4Code = (_DmxGetHiBit(&u1BitField) << 2) |
                                (_DmxGetHiBit(&u1BitField) << 1) |
                                (_DmxGetHiBit(&u1BitField) << 0);
                u4PicType = _u1vopFirstFieldType[u4Code];
                u1SecondFieldType = _u1vopSecondFieldType[u4Code];
            }
            else
            {
                u4PicType = u1SecondFieldType;
            }
            i4CurrentTemporalField ^= 1;
        }
        break;

    default:
        // Don't care others
        break;
    }

_exit:
    
    return u4PicType;
}


//-----------------------------------------------------------------------------
/** _DmxDetectWMVPictureType
 *  Detect the picture type at the given address.  For now, the downstream
 *  decoder do no ask Demux to find the picture type of WMV.  Therefore, we
 *  just pick a type at will, in this case the I frame, as the picture type.
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectWMVPictureType(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UNUSED(u4Addr);
    UNUSED(u4BufStart);
    UNUSED(u4BufEnd);
    return PIC_TYPE_I;          // Pick a picture type at will.
}
#endif  // ENABLE_MULTIMEDIA


//-----------------------------------------------------------------------------
/** _DmxDetectAVSPictureType
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectAVSPictureType(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT32 u4PicType;
    UINT8* p = NULL;
    UINT8 u1Type, h[PIC_HEADER_LEN], i;

    u4PicType = AVS_PIC_TYPE_UNKNOWN;

    if ((u4Addr < u4BufStart) || (u4Addr >= u4BufEnd))
    {
        LOG(3, "Invalid picture address 0x%08x, pidx: %u\n");

        return u4PicType;
    }

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(!TZ_DMX_GetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, PIC_HEADER_LEN))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }

    p = h;
    #else
    if(!_DmxGetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, PIC_HEADER_LEN, DMX_VIDEO_AVS))
    {
        u4PicType = PIC_TYPE_UNKNOWN;
        goto _exit;
    }
    
    p = h;
    #endif

    // Check start code
    // 00 00 01
    if ((p[0] != AVS_PIC_START_CODE_1) || (p[1] != AVS_PIC_START_CODE_2) ||
        (p[2] != AVS_PIC_START_CODE_3))
    {
        goto _exit;
    }

    // Check picture type
    u1Type = p[3];

    for(i = 0; i < DMX_PICINFO_SIZE; i++)
    {
        pu1PicInfo[i] = p[DMX_PICINFO_OFFSET + i];
    }

    switch (u1Type)
    {
    case AVS_PIC_START_CODE_SEQ_START:
        u4PicType = AVS_PIC_TYPE_SEQ_START;
        break;

    case AVS_PIC_START_CODE_SEQ_END:
        u4PicType = AVS_PIC_TYPE_SEQ_END;
        break;

    case AVS_PIC_START_CODE_SEQ_USR_DATA:
        u4PicType = AVS_PIC_TYPE_USR_DATA;
        break;

    case AVS_PIC_START_CODE_SEQ_PIC_I:
        u4PicType = AVS_PIC_TYPE_PIC_I;
        break;

    case AVS_PIC_START_CODE_SEQ_EXT:
        u4PicType = AVS_PIC_TYPE_EXT;
        break;

    case AVS_PIC_START_CODE_SEQ_PIC_PB:
        u4PicType = AVS_PIC_TYPE_PIC_PB;
        break;

    case AVS_PIC_START_CODE_SEQ_VID_EDIT:
        u4PicType = AVS_PIC_TYPE_VID_EDIT_CODE;
        break;

    default:
        break;
    }

_exit:
    
    return u4PicType;
}


//-----------------------------------------------------------------------------
/** _DmxDetectPictureType
 *  Detect picture type at a given address
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxDetectPictureType(DMX_VIDEO_TYPE_T eVideoType, UINT32 u4Addr, UINT32 u4BufStart, UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT32 u4Type = PIC_TYPE_UNKNOWN;

    switch(eVideoType)
    {
    case DMX_VIDEO_MPEG:
        u4Type = _DmxDetectMPEGPictureType(u4Addr, u4BufStart, u4BufEnd, pu1PicInfo);
        break;

    case DMX_VIDEO_H264:
        u4Type = _DmxDetectH264PictureType(u4Addr, u4BufStart, u4BufEnd, pu1PicInfo);
        break;

#ifdef ENABLE_MULTIMEDIA
    case DMX_VIDEO_MPEG4:
        u4Type = _DmxDetectMPEG4PictureType(u4Addr, u4BufStart, u4BufEnd, pu1PicInfo);
        break;

    case DMX_VIDEO_VC1:
        u4Type = _DmxDetectVC1PictureType(u4Addr, u4BufStart, u4BufEnd, pu1PicInfo);
        break;

    case DMX_VIDEO_WMV7:
    case DMX_VIDEO_WMV8:
    case DMX_VIDEO_WMV9:
        u4Type = _DmxDetectWMVPictureType(u4Addr, u4BufStart, u4BufEnd, pu1PicInfo);
        break;
#endif  // ENABLE_MULTIMEDIA

    case DMX_VIDEO_AVS:
        u4Type = _DmxDetectAVSPictureType(u4Addr, u4BufStart, u4BufEnd, pu1PicInfo);
        break;

    default:
        //LOG(3, "Unknown video type!\n");
        break;
    }

    return u4Type;
}

#ifdef ENABLE_MULTIMEDIA
//-----------------------------------------------------------------------------
/** _DmxFillPicInfo
 *
 *  @param  u4Addr          The picture address
 *  @param  u4BufStart      Buffer start address
 *  @param  u4BufEnd        Buffer end address
 */
//-----------------------------------------------------------------------------
static BOOL _DmxFillPicInfo(UINT32 u4Addr, UINT32 u4BufStart,
    UINT32 u4BufEnd, UINT8 *pu1PicInfo)
{
    UINT8* p = NULL;
    UINT8 h[DMX_PICINFO_SIZE], i;
    BOOL fgRet = TRUE;

    if ((u4Addr < u4BufStart) || (u4Addr >= u4BufEnd))
    {
        LOG(3, "Invalid Pic Buffer\n");
        return FALSE;
    }

    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    if(!TZ_DMX_GetPicHeader(u4Addr, u4BufStart, u4BufEnd, h, DMX_PICINFO_SIZE))
    {
        fgRet = FALSE;
        goto _exit;
    }

    p = h;
    #else
    // Check if the header is across FIFO boundary
    p = (UINT8*)u4Addr;
    if (((u4Addr + DMX_PICINFO_SIZE) - 1) >= u4BufEnd)
    {
        UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
        if(u1Len >= DMX_PICINFO_SIZE)
        {
            fgRet = FALSE;
            goto _exit;
        }
        HalInvalidateDCacheMultipleLine(u4Addr, u1Len);
        HalInvalidateDCacheMultipleLine(u4BufStart, (DMX_PICINFO_SIZE - u1Len));
        x_memcpy((void*)h, (void*)p, u1Len);
        x_memcpy((void*)(h + u1Len), (void*)u4BufStart,
            (SIZE_T)(DMX_PICINFO_SIZE - u1Len));
        p = h;
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, DMX_PICINFO_SIZE);
    }
    #endif

    for(i = 0; i < DMX_PICINFO_SIZE; i++)
    {
        pu1PicInfo[i] = p[i];
    }

    //LOG(1, "The first 4 bytes of callback: 0x%x 0x%x 0x%x 0x%x\n", pu1PicInfo[0], pu1PicInfo[1], pu1PicInfo[2], pu1PicInfo[3]);
_exit:
    
    return fgRet;
}
#endif // ENABLE_MULTIMEDIA


//-----------------------------------------------------------------------------
static BOOL _DMX_SendMPEGPictureHeader(DMX_PES_MSG_T* prPes)
{
    UINT8 u1Pidx;
    BOOL fgRet = FALSE;

    ASSERT(prPes != NULL);

    u1Pidx = prPes->u1Pidx;
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

/*
#ifdef TIME_MEASUREMENT
    if ((_fgWaitForFirstI == TRUE) && (rPes.u4FrameType == PIC_TYPE_I))
    {
        TMS_END_EX(TMS_FLAG_CHG_CHL, "DMX_FIRST_I", "DMX_First_I");
        _fgWaitForFirstI = FALSE;
    }
#endif  // TIME_MEASUREMENT
*/
    // Queue the picture header
    switch (prPes->u4FrameType)
    {
#ifdef CC_VDEC_FMT_DETECT
    case H264_PIC_TYPE_NONIDR:
    case H264_PIC_TYPE_SEQ:
        fgRet = TRUE;
        break;
#endif  // CC_VDEC_FMT_DETECT

    case PIC_TYPE_SEQ_START:
        _afgSeqHeader[u1Pidx] = TRUE;
        _au4SeqHeaderAddr[u1Pidx] = prPes->u4FrameAddr;
        if (_afgSeqEnd[u1Pidx])
        {
            _afgSeqHeaderPtsDts[u1Pidx] = _afgSeqEndPtsDts[u1Pidx];
            _au4SeqHeaderPts[u1Pidx] = _au4SeqEndPts[u1Pidx];
            _au4SeqHeaderDts[u1Pidx] = _au4SeqEndDts[u1Pidx];

            _afgSeqEnd[u1Pidx] = FALSE;
            _afgSeqEndPtsDts[u1Pidx] = FALSE;
        }
        else
        {
            _afgSeqHeaderPtsDts[u1Pidx] = prPes->fgPtsDts;
            _au4SeqHeaderPts[u1Pidx] = prPes->u4Pts;
            _au4SeqHeaderDts[u1Pidx] = prPes->u4Dts;
        }
        break;

    case PIC_TYPE_GOP:
        _afgGop[u1Pidx] = TRUE;
        _au4GopAddr[u1Pidx] = prPes->u4FrameAddr;
        if (_afgSeqEnd[u1Pidx])
        {
            _afgGopPtsDts[u1Pidx] = _afgSeqEndPtsDts[u1Pidx];
            _au4GopPts[u1Pidx] = _au4SeqEndPts[u1Pidx];
            _au4GopDts[u1Pidx] = _au4SeqEndDts[u1Pidx];

            _afgSeqEnd[u1Pidx] = FALSE;
            _afgSeqEndPtsDts[u1Pidx] = FALSE;
        }
        else
        {
            _afgGopPtsDts[u1Pidx] = prPes->fgPtsDts;
            _au4GopPts[u1Pidx] = prPes->u4Pts;
            _au4GopDts[u1Pidx] = prPes->u4Dts;
        }
        break;

    case PIC_TYPE_I:
    case PIC_TYPE_P:
    case PIC_TYPE_B:
        prPes->fgSeqHeader = _afgSeqHeader[u1Pidx];
        prPes->fgGop = _afgGop[u1Pidx];
        if (prPes->fgSeqHeader)
        {
            prPes->u4FrameAddr = _au4SeqHeaderAddr[u1Pidx];
            if (!prPes->fgPtsDts)
            {
                prPes->fgPtsDts = _afgSeqHeaderPtsDts[u1Pidx];
                prPes->u4Pts = _au4SeqHeaderPts[u1Pidx];
                prPes->u4Dts = _au4SeqHeaderDts[u1Pidx];
            }
        }
        else if (prPes->fgGop)
        {
            prPes->u4FrameAddr = _au4GopAddr[u1Pidx];
            if (!prPes->fgPtsDts)
            {
                prPes->fgPtsDts = _afgGopPtsDts[u1Pidx];
                prPes->u4Pts = _au4GopPts[u1Pidx];
                prPes->u4Dts = _au4GopDts[u1Pidx];
            }
        }
        else if (_afgSeqEnd[u1Pidx] && (!prPes->fgPtsDts))
        {
            prPes->fgPtsDts = _afgSeqEndPtsDts[u1Pidx];
            prPes->u4Pts = _au4SeqEndPts[u1Pidx];
            prPes->u4Dts = _au4SeqEndDts[u1Pidx];
        }
        fgRet = TRUE;
        _afgSeqHeader[u1Pidx] = FALSE;
        _afgGop[u1Pidx] = FALSE;
        _afgSeqEnd[u1Pidx] = FALSE;
        break;

    case PIC_TYPE_SEQ_END:
        // This case is for supporting bit streams full of audio PES but
        // with scarce and concentrated (not scattered) video PES. It also deals
        // with cases in which the SEQUENCE_END is at the very end of a PES.
        if (prPes->fgPtsDts)
        {
            _afgSeqEnd[u1Pidx] = TRUE;
            _afgSeqEndPtsDts[u1Pidx] = TRUE;
            _au4SeqEndPts[u1Pidx] = prPes->u4Pts;
            _au4SeqEndDts[u1Pidx] = prPes->u4Dts;
        }
        fgRet = TRUE;
        break;

    default:
        // Todo: count invalid picture headers
        break;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
static BOOL _DMX_SendH264PictureHeader(DMX_PES_MSG_T* prPes)
{
    UINT8 u1Pidx;
    BOOL fgRet = FALSE;

    ASSERT(prPes != NULL);

    u1Pidx = prPes->u1Pidx;
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    prPes->eVideoType = DMX_VIDEO_H264;
/*
#ifdef TIME_MEASUREMENT
    if ((_fgWaitForFirstI == TRUE) && (rPes.u4FrameType == H264_PIC_TYPE_SEQ))
    {
        // Normally, the I frame appears right after SEQ.
        TMS_END_EX(TMS_FLAG_CHG_CHL, "DMX_FIRST_I", "DMX_First_SEQ");
        _fgWaitForFirstI = FALSE;
    }
#endif  // TIME_MEASUREMENT
*/
    // Store PTS/DTS if they exist.
    if(prPes->fgPtsDts)
    {
        _afgSeqHeaderPtsDts[u1Pidx] = prPes->fgPtsDts;
        _au4SeqHeaderPts[u1Pidx] = prPes->u4Pts;
        _au4SeqHeaderDts[u1Pidx] = prPes->u4Dts;
    }
    else if (_afgSeqHeaderPtsDts[u1Pidx])
    {
        // Make use the previous PTS/DTS values if rPes does not have PTS/DTS.
        prPes->fgPtsDts = _afgSeqHeaderPtsDts[u1Pidx];
        prPes->u4Pts = _au4SeqHeaderPts[u1Pidx];
        prPes->u4Dts = _au4SeqHeaderDts[u1Pidx];
    }

    // Clear the PTS/DTS values when a picture is found.
    if ((prPes->u4FrameType == H264_PIC_TYPE_IDR) ||
    	(prPes->u4FrameType == H264_PIC_TYPE_NONIDR))
    {
        _afgSeqHeaderPtsDts[u1Pidx] = FALSE;
        _au4SeqHeaderPts[u1Pidx] = 0;
        _au4SeqHeaderDts[u1Pidx]= 0;
    }

    if(prPes->u4FrameType == H264_PIC_TYPE_SEQ)
    {
        prPes->fgSeqHeader = TRUE;
    }

    // Queue the picture header only when certain frame types are found.
    // Two adjacent switch-case statements are used to avoid LINT warning when
    // fall-through is used.
    switch (prPes->u4FrameType)
    {
    case H264_PIC_TYPE_NONIDR:
    case H264_PIC_TYPE_IDR:
    case H264_PIC_TYPE_SEI:
    case H264_PIC_TYPE_SEQ:
    case H264_PIC_TYPE_PIC:
    case H264_PIC_TYPE_AU:
#ifdef CC_VDEC_FMT_DETECT
    case PIC_TYPE_I:
    case PIC_TYPE_P:
#endif  // CC_VDEC_FMT_DETECT
#if defined(CC_MVC_SUPPORT) || defined(CC_SW_MVC_ENABLE)
    case H264_PIC_TYPE_PREFIX:
    case H264_PIC_TYPE_SUB_SPS:
    case H264_PIC_TYPE_CODED_SLICE_EXT:
    case H264_PIC_TYPE_CMI:
#endif
        fgRet = TRUE;
        break;
    default:
        // Todo: count invalid picture headers
        break;
    }

    return fgRet;
}


#ifdef ENABLE_MULTIMEDIA
//-----------------------------------------------------------------------------
static BOOL _DMX_SendMPEG4PictureHeader(DMX_PES_MSG_T* prPes)
{
    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _DMX_SendRVPictureHeader(DMX_PES_MSG_T* prPes)
{
    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _DMX_SendVPPictureHeader(DMX_PES_MSG_T* prPes)
{
    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _DMX_SendVC1PictureHeader(DMX_PES_MSG_T* prPes)
{
    UINT8 u1Pidx;
    BOOL fgRet = FALSE;

    ASSERT(prPes != NULL);

    u1Pidx = prPes->u1Pidx;
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    prPes->eVideoType = DMX_VIDEO_VC1;

    // Store PTS/DTS if they exist.
    if(prPes->fgPtsDts)
    {
        _afgSeqHeaderPtsDts[u1Pidx] = prPes->fgPtsDts;
        _au4SeqHeaderPts[u1Pidx] = prPes->u4Pts;
        _au4SeqHeaderDts[u1Pidx] = prPes->u4Dts;
    }
    else if (_afgSeqHeaderPtsDts[u1Pidx])
    {
        // Make use the previous PTS/DTS values if rPes does not have PTS/DTS.
        prPes->fgPtsDts = _afgSeqHeaderPtsDts[u1Pidx];
        prPes->u4Pts = _au4SeqHeaderPts[u1Pidx];
        prPes->u4Dts = _au4SeqHeaderDts[u1Pidx];
    }

    // Clear the PTS/DTS values when a picture is found.
    switch (prPes->u4FrameType)
    {
    case VC1_PIC_TYPE_I:
    case VC1_PIC_TYPE_P:
    case VC1_PIC_TYPE_B:
    case VC1_PIC_TYPE_BI:
    case VC1_PIC_TYPE_SKIP:
        _afgSeqHeaderPtsDts[u1Pidx] = FALSE;
        _au4SeqHeaderPts[u1Pidx] = 0;
        _au4SeqHeaderDts[u1Pidx]= 0;
        break;
    default:
        break;
    }

    // Queue the picture header only when certain frame types are found.
    // Two adjacent switch-case statements are used to avoid LINT warning when
    // fall-through is used.
    switch (prPes->u4FrameType)
    {
    case VC1_PIC_TYPE_I:
    case VC1_PIC_TYPE_P:
    case VC1_PIC_TYPE_B:
    case VC1_PIC_TYPE_BI:
    case VC1_PIC_TYPE_SKIP:
    case VC1_PIC_TYPE_ENTRY:
    case VC1_PIC_TYPE_SEQ_START:
        fgRet = TRUE;
        break;
    default:
        // Todo: count invalid picture headers
        break;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
static BOOL _DMX_SendWMVPictureHeader(DMX_PES_MSG_T* prPes)
{
    ASSERT(prPes != NULL);

    if (! _DMX_MM_IsFrameHead())
    {
        return FALSE;        // To avoid the LOG message in _DmxProcessVideo*().
    }

    if ((prPes->u4FrameAddr <  prPes->u4BufStart) ||
        (prPes->u4FrameAddr >= prPes->u4BufEnd ))
    {
        LOG(3, "The frame address is out of boundary!\n");
        return FALSE;
    }

    return TRUE;
}
#endif  // ENABLE_MULTIMEDIA


//-----------------------------------------------------------------------------
static BOOL _DMX_SendAVSPictureHeader(DMX_PES_MSG_T* prPes)
{
    return TRUE;
}

static BOOL _DmxProcessPictureHeader(DMX_PES_MSG_T* prPes)
{
    BOOL fgRet;
    DMX_VIDEO_TYPE_T eVideoType;

    if(prPes == NULL)
    {
        return FALSE;
    }

    fgRet = FALSE;
    eVideoType = _DMX_GetPidxVideoType(prPes->u1Pidx);
    prPes->eVideoType = eVideoType;

    if(eVideoType == DMX_VIDEO_H264)
    {
        if((prPes->u4FrameType == H264_PIC_TYPE_NONIDR) ||
           (prPes->u4FrameType == H264_PIC_TYPE_IDR))
        {
            prPes->u1SliceCount = _u1H264SliceNum;
            _u1H264SliceNum = 0;
        }
    }

    switch(eVideoType)
    {
    case DMX_VIDEO_MPEG:
        fgRet = _DMX_SendMPEGPictureHeader(prPes);
        break;

    case DMX_VIDEO_H264:
        fgRet = _DMX_SendH264PictureHeader(prPes);
        break;

#ifdef ENABLE_MULTIMEDIA
    case DMX_VIDEO_MPEG4:
        fgRet = _DMX_SendMPEG4PictureHeader(prPes);
        break;

    case DMX_VIDEO_RV:
        fgRet = _DMX_SendRVPictureHeader(prPes);
        break;

    case DMX_VIDEO_VC1:
        fgRet = _DMX_SendVC1PictureHeader(prPes);
        break;

    case DMX_VIDEO_WMV7:
    case DMX_VIDEO_WMV8:
    case DMX_VIDEO_WMV9:
    case DMX_VIDEO_MP4_IN_WMV:
        fgRet = _DMX_SendWMVPictureHeader(prPes);
        break;

    case DMX_VIDEO_VP6:
    case DMX_VIDEO_VP8:
        fgRet = _DMX_SendVPPictureHeader(prPes);
        break;

#endif  // ENABLE_MULTIMEDIA

    case DMX_VIDEO_AVS:
        fgRet = _DMX_SendAVSPictureHeader(prPes);
        break;

    default:
        LOG(3, "Unknown video type\n");
        ASSERT(0);
    }

    return fgRet;
}

static BOOL fgDMXLockApi = FALSE;

static void _DmxThreadTryToLock(void)
{
    if(!fgDMXLockApi)
    {
        fgDMXLockApi = TRUE;
        _DMX_LockApi();
    }
}

static void _DmxThreadTryToUnlock(UINT8 u1Pidx)
{
    PID_STRUCT_T* prPidStruct;
    UINT32 u4PicCount;

    _DMX_Lock();
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    _DMX_Unlock();
    if ((prPidStruct->eInputType == DMX_IN_PLAYBACK_PS) ||
        (prPidStruct->eInputType == DMX_IN_PLAYBACK_MM) ||
        (prPidStruct->eInputType == DMX_IN_PLAYBACK_ES))
    {
        u4PicCount = _DMX_MUL_GetPicCount(u1Pidx);
        // check if thread already send out all PES message
        if(u4PicCount == 0)
        {
            _DMX_UnlockApi();
            fgDMXLockApi = FALSE;
            _DMX_MUL_UnlockPicSemWhenFinishedCallback();
        }
    }
    else
    {
        _DMX_UnlockApi();
        fgDMXLockApi = FALSE;
    }
}


//-----------------------------------------------------------------------------
/** _DmxThread
 *  The picture thread
 *
 *  @param  prArg           The thread argument
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID _DmxThread(VOID* pvArg)
{
    UNUSED(pvArg);

    while (1)
    {
        static DMX_PES_MSG_T rPes;
        PID_STRUCT_T* prPidStruct;
        INT32 i4Ret;
        SIZE_T zMsgSize;
        UINT16 u2QueueIndex;
#if 0
        // Debug
        while (1)
        {
            UINT16 u2MsgNum;

            VERIFY(x_msg_q_num_msgs(_hDmxQueue, &u2MsgNum) == OSR_OK);
            if (u2MsgNum > 4)
            {
                break;
            }

            x_thread_delay(10);
        }
#endif

        zMsgSize = sizeof (rPes);
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rPes, &zMsgSize,
            &_hDmxQueue, 1, X_MSGQ_OPTION_WAIT);

        // Inhibit API function calls during processing message
        _DmxThreadTryToLock();

        prPidStruct = _DMX_GetPidStruct(rPes.u1Pidx);

        if (i4Ret != OSR_OK)
        {
            _DmxThreadTryToUnlock(rPes.u1Pidx);
            LOG(1, "Error on receiving picture message!\n");
            continue;
        }

        prPidStruct = _DMX_GetPidStruct(rPes.u1Pidx);
        if (rPes.u1SerialNumber != prPidStruct->u1SerialNumber)
        {
            // Overdue message, just discard it
            _DMX_MUL_ModifyPicCount(rPes.u1Pidx, FALSE);
            _DmxThreadTryToUnlock(rPes.u1Pidx);
            LOG(3, "Overdue message of pidx %u!\n", rPes.u1Pidx);
            continue;
        }

        ASSERT(zMsgSize == sizeof (rPes));
        ASSERT(u2QueueIndex == 0);

        switch (rPes.eMsgType)
        {
        case DMX_PES_MSG_TYPE_PES:
            if (!rPes.fgEOS && !rPes.fgMoveComplete && (prPidStruct->ePidType == DMX_PID_TYPE_ES_VIDEO))
            {
                #ifdef ENABLE_MULTIMEDIA
                if((_DMX_GetPidInputType(rPes.u1Pidx) == DMX_IN_PLAYBACK_MM) &&
                   (!_DMX_MM_GetSearchStartCode(rPes.u1Pidx)))
                {
                    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
                    UNUSED(_DmxFillPicInfo(rPes.u4FrameAddr,rPes.u4BufStart, rPes.u4BufEnd, rPes.au1PicInfo));
                    #else
                    UNUSED(_DmxFillPicInfo(VIRTUAL(rPes.u4FrameAddr),
                                       VIRTUAL(rPes.u4BufStart), VIRTUAL(rPes.u4BufEnd), rPes.au1PicInfo));
                    #endif
                }
                else
                #endif  // ENABLE_MULTIMEDIA
                {
                    #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
                    rPes.u4FrameType = _DmxDetectPictureType(_DMX_GetPidxVideoType(rPes.u1Pidx),
                        rPes.u4FrameAddr, rPes.u4BufStart, rPes.u4BufEnd,
                        rPes.au1PicInfo);
                    #else
                    rPes.u4FrameType = _DmxDetectPictureType(_DMX_GetPidxVideoType(rPes.u1Pidx),
                        VIRTUAL(rPes.u4FrameAddr), VIRTUAL(rPes.u4BufStart), VIRTUAL(rPes.u4BufEnd),
                        rPes.au1PicInfo);
                    #endif

                    //LOG(5, "u4FrameType: %d\n", rPes.u4FrameType);
                    //LOG(5, "0x%x 0x%x 0x%x 0x%x\n", rPes.au1PicInfo[0], rPes.au1PicInfo[1], rPes.au1PicInfo[2], rPes.au1PicInfo[3]);
                }

                if(_DmxProcessPictureHeader(&rPes))
                {
                    UNUSED(_DmxDispatchPesMessage(&rPes));
                }

                _DMX_MUL_ModifyPicCount(rPes.u1Pidx, FALSE);
            }
            else
            {
                UNUSED(_DmxDispatchPesMessage(&rPes));
                _DMX_MUL_ModifyPicCount(rPes.u1Pidx, FALSE);
                if ((rPes.fgEOS) && (prPidStruct->ePidType == DMX_PID_TYPE_ES_VIDEO))
                {
                    _afgSeqHeader[rPes.u1Pidx] = FALSE;
                    _afgGop[rPes.u1Pidx] = FALSE;
                    _afgSeqEnd[rPes.u1Pidx] = FALSE;
                }
            }
            break;

        default:
            // Unknown message type
            ASSERT(FALSE);
            break;
        }   // switch (rPes.eMsgType)

        // Resume API calls
        _DmxThreadTryToUnlock(rPes.u1Pidx);

    }   // while (1)
}

//-----------------------------------------------------------------------------
/** _PurgeThread
 *  The purge thread
 *
 *  @param  prArg           The thread argument
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID _PurgeThread(VOID* pvArg)
{
    UNUSED(pvArg);

    while (1)
    {
        static DMX_PES_MSG_T rPes;
        PID_STRUCT_T* prPidStruct;
        INT32 i4Ret;
        SIZE_T zMsgSize;
        UINT16 u2QueueIndex;

        zMsgSize = sizeof (rPes);
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rPes, &zMsgSize,
            &_hPurgeQueue, 1, X_MSGQ_OPTION_WAIT);

        // Inhibit API function calls during processing message
        _DMX_LockApi();

        if (i4Ret != OSR_OK)
        {
            _DMX_UnlockApi();
            LOG(1, "Error on receiving picture message!\n");
            continue;
        }

        // Use Dmxthread to handle error case
        if(rPes.eMsgType == DMX_PES_MSG_TYPE_ERR_HANDLE)
        {
            _DMX_PanicReset();
            _DMX_UnlockApi();
            continue;
        }

        prPidStruct = _DMX_GetPidStruct(rPes.u1Pidx);
        if (rPes.u1SerialNumber != prPidStruct->u1SerialNumber)
        {
            // Overdue message, just discard it
            _DMX_UnlockApi();
            LOG(3, "Overdue message of pidx %u!\n", rPes.u1Pidx);
            continue;
        }

        ASSERT(zMsgSize == sizeof (rPes));
        ASSERT(u2QueueIndex == 0);

        switch (rPes.eMsgType)
        {
        case DMX_PES_MSG_TYPE_PURGE:
            UNUSED(_DmxDiscardFifoData(rPes.u1Pidx, FALSE));
            break;

        case DMX_PES_MSG_TYPE_PURGE_REENABLE:
            UNUSED(_DmxDiscardFifoData(rPes.u1Pidx, TRUE));
            break;

        default:
            // Unknown message type
            ASSERT(FALSE);
            break;
        }   // switch (rPes.eMsgType)

        // Resume API calls
        _DMX_UnlockApi();

    }   // while (1)
}

#ifdef CC_DMX_ENABLE_MONTHREAD
//-----------------------------------------------------------------------------
/** _DmxMonPktDiff
 */
//-----------------------------------------------------------------------------
static UINT32 _DmxMonPktDiff(UINT32 u4New, UINT32 u4Old)
{
    UINT32 u4Diff;

    if(u4New < u4Old)
    {
        u4Diff = (u4New + 0x10000) - u4Old;
    }
    else
    {
        u4Diff = u4New - u4Old;
    }

    return u4Diff;
}


//-----------------------------------------------------------------------------
/** _DmxMonFramer
 */
//-----------------------------------------------------------------------------
static VOID _DmxMonFramer(void)
{
    // only check framer 0 and framer 1 for general case
    static UINT32 _u4PreRcvPtkCount[2] = {};
    static UINT32 _u4PreDropPktCount[2] = {};
    static UINT32 _u4FramerCount[2] = {};
    UINT32 u4RcvPtkCount, u4DropPktCount;
    UINT32 u4PcvPktDiff, u4DropPktDiff;
    UINT32 u4Control;
    UINT8 u1FramerIdx;

    for(u1FramerIdx = 0; u1FramerIdx < 2; u1FramerIdx++)
    {
        if (_DMX_IsMicroProcessorStopped())
        {
            _u4FramerCount[u1FramerIdx] = 0;
            continue;
        }

        // check if any enable PID in playback or record
        if(!(_DMX_GetIsTSEnabled(u1FramerIdx) || _FVR_GetIsTSEnabled(u1FramerIdx)))
        {
            _u4FramerCount[u1FramerIdx] = 0;
            continue;
        }

        u4Control = DMXCMD_READ32(DMX_REG_FRAMER0_STATUS + u1FramerIdx);
        u4RcvPtkCount = u4Control & 0xFFFF;
        u4DropPktCount = (u4Control & 0xFFFF0000) >> 16;

        u4PcvPktDiff = _DmxMonPktDiff(u4RcvPtkCount, _u4PreRcvPtkCount[u1FramerIdx]);
        u4DropPktDiff = _DmxMonPktDiff(u4DropPktCount, _u4PreDropPktCount[u1FramerIdx]);

        //LOG(5, "%d, rcv_diff:%d, drop_diff:%d\n", u1FramerIdx, u4PcvPktDiff, u4DropPktDiff);

        UNUSED(u4DropPktDiff);

        if(u4PcvPktDiff < DMX_MON_RCV_THRESHOLD)
        {
            _u4FramerCount[u1FramerIdx]++;
        }
        else
        {
            _u4FramerCount[u1FramerIdx] = 0;
        }

        _u4PreRcvPtkCount[u1FramerIdx] = u4RcvPtkCount;
        _u4PreDropPktCount[u1FramerIdx] = u4DropPktCount;

        if(_u4FramerCount[u1FramerIdx] > 3)
        {
            LOG(0, "Framer %d is wrong, reset it\n", u1FramerIdx);
            _DMX_ResetFramer(u1FramerIdx);
            _u4FramerCount[u1FramerIdx] = 0;
        }
    }
}


#if 0
//-----------------------------------------------------------------------------
/** _DmxMonMicroCode
 */
//-----------------------------------------------------------------------------
static VOID _DmxMonMicroCode(void)
{
    static UINT32 _u4StopCount = 0;
    static UINT32 _u4PreVal = 0xFFFFFFFF;
    UINT32 u4Val;

    // Pull mode
    if((DMXCMD_READ32(DMX_REG_CONFIG2) & 0xC0000000) == 0xC0000000)
    {
        _u4StopCount = 0;
        return;
    }

    if (_DMX_IsMicroProcessorStopped())
    {
        _u4StopCount = 0;
        return;
    }

    // Need to as PS
    if(!_DMX_GetIsTSEnabled())
    {
    	_u4StopCount = 0;
        return;
    }

    u4Val = MICROCODE_WATCHDOG;
    if(u4Val == _u4PreVal)
    {
        _u4StopCount++;
    }
    else
    {
        _u4StopCount = 0;
    }

    _u4PreVal = u4Val;

    if(_u4StopCount >= 5)
    {
        _u4StopCount = 0;
        _DMX_PanicMicroCode();
    }

}
#endif

//-----------------------------------------------------------------------------
/** _DmxMonThread
 *  The monitor thread
 *
 *  @param  prArg           The thread argument
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID _DmxMonThread(VOID* pvArg)
{
    UNUSED(pvArg);
    UNUSED(_DmxMonFramer);

    while(1)
    {
        x_thread_delay(DMX_MON_TIME_PERIOD);

        _DmxMonFramer();
        #if 0
        _DmxMonMicroCode();
        #endif
    }
}
#endif  // CC_DMX_ENABLE_MONTHREAD


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


#ifdef CC_DMX_PES_EXT_BUFFER
//-----------------------------------------------------------------------------
/** _DMX_UpdateExtBufferReadPointer
 *  Update read pointer of PES extension buffer.
 *
 *  @param  u1Pidx          PID index
 *  @param  u4NewRp         The new read pointer
 *  @param  u4DataAddr      The address of the PES_extension_field data
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_UpdateExtBufferReadPointer(UINT8 u1Pidx, UINT32 u4NewRp, UINT32 u4DataAddr)
{
    UINT32 u4ExtBufStart, u4ExtBufEnd, u4ExtBufLen, u4DataSize, u4UpdateSize;
    PID_STRUCT_T* prPidStruct;
    PID_STRUCT_T rPidStruct;

    // Check if pid index is valid
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(3, "Update Ext_buf RP: invalid PID index %u\n", u1Pidx);
        return FALSE;
    }

    _DMX_Lock();
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    rPidStruct = *prPidStruct;
    _DMX_Unlock();

    // Check if RP is out of range.
    u4ExtBufStart = rPidStruct.u4ExtBufStart;
    u4ExtBufEnd = rPidStruct.u4ExtBufEnd + 1;  // !!!
    u4ExtBufLen = u4ExtBufEnd - u4ExtBufStart;
    u4NewRp = VIRTUAL(u4NewRp);
    if ((u4NewRp < u4ExtBufStart) || (u4NewRp >= u4ExtBufEnd))
    {
        // RP is out of range.
        LOG(3, "Pidx: %u, Invalid new RP: 0x%08x\n", u1Pidx, u4NewRp);
        return FALSE;
    }

    // Check update size
    u4DataSize = DATASIZE(rPidStruct.u4ExtBufRp, rPidStruct.u4ExtBufWp, u4ExtBufLen);
    u4UpdateSize = DATASIZE(rPidStruct.u4ExtBufRp, u4NewRp, u4ExtBufLen);
    if (u4UpdateSize > u4DataSize)
    {
        // Update size is larger than the data size. Something is wrong...
        LOG(3, "Pidx: %u, Invalid new RP: 0x%08x, dataSize: %u, updateSize: %u\n",
            u1Pidx, u4NewRp, u4DataSize, u4UpdateSize);
        return FALSE;
    }

    _DMX_Lock();
    prPidStruct->u4ExtBufRp = u4NewRp;
    _DMX_Unlock();

    // Debug
    DMXPIDLOG(DMX_LOG_BUFFER 7, u1Pidx, "Update Ext RP - pidx: %u, new RP: 0x%08x, dataAddr: 0x%08x\n",
        u1Pidx, u4NewRp, u4DataAddr);

    return TRUE;
}
#endif  // CC_DMX_PES_EXT_BUFFER

//-----------------------------------------------------------------------------
/** _DMX_UpdateWritePointer
 *  Update write pointer of PID buffer
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Wp            The new write pointer pointing to a physical address
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_UpdateWritePointer(UINT8 u1Pidx, UINT32 u4Wp)
{
    UINT32 u4BufStart, u4BufEnd;
    PID_STRUCT_T* prPidStruct;
    PID_STRUCT_T rPidStruct;

    // Check if pid index is valid
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(3, "Update WP: invalid PID index %u\n", u1Pidx);
        return FALSE;
    }

    _DMX_Lock();

    // Check if RP out of range
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    rPidStruct = *prPidStruct;

    _DMX_Unlock();

    u4BufStart = rPidStruct.u4BufStart;
    u4BufEnd = rPidStruct.u4BufEnd + 1;
    u4Wp = VIRTUAL(u4Wp);
    if ((u4Wp < u4BufStart) || (u4Wp >= u4BufEnd))
    {
        // WP out of range
        LOG(3, "Pidx: %u, Invalid WP: 0x%08x\n", u1Pidx, u4Wp);
        return FALSE;
    }

    _DMX_Lock();
    prPidStruct->u4Wp = u4Wp;
    _DMX_Unlock();

    // Debug
    DMXPIDLOG(DMX_LOG_BUFFER 7, u1Pidx, "Update WP - pidx: %u, WP: 0x%08x\n", u1Pidx, u4Wp);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_UpdateReadPointer
 *  Update read pointer of PID buffer
 *
 *  @param  u1Pidx          PID index
 *  @param  u4Rp            The new read pointer pointing to a physical address
 *  @param  u4FrameAddr     The frame (picture) physical address
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_UpdateReadPointer(UINT8 u1Pidx, UINT32 u4Rp, UINT32 u4FrameAddr)
{
    UINT32 u4BufStart, u4BufEnd, u4DataSize, u4UpdateSize;
    PID_STRUCT_T* prPidStruct;
    PID_STRUCT_T rPidStruct;

    // Check if pid index is valid
    if (u1Pidx >= DMX_NUM_PID_INDEX)
    {
        LOG(3, "Update RP: invalid PID index %u\n", u1Pidx);
        return FALSE;
    }

    _DMX_Lock();

    // Check if RP out of range
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    rPidStruct = *prPidStruct;

    _DMX_Unlock();

    u4BufStart = rPidStruct.u4BufStart;
    u4BufEnd = rPidStruct.u4BufEnd + 1;
    u4Rp = VIRTUAL(u4Rp);
    if ((u4Rp < u4BufStart) || (u4Rp >= u4BufEnd))
    {
        // RP out of range
        LOG(3, "Pidx: %u, Invalid RP: 0x%08x\n", u1Pidx, u4Rp);
        return FALSE;
    }

    // Check update size
    u4DataSize = DATASIZE(rPidStruct.u4Rp, rPidStruct.u4Wp,
        rPidStruct.u4BufLen);
    u4UpdateSize = DATASIZE(rPidStruct.u4Rp, u4Rp, rPidStruct.u4BufLen);
    if (u4UpdateSize > u4DataSize)
    {
        // Update size is larger than data size, something wrong
        LOG(3, "Pidx: %u, Invalid RP 2: 0x%08x, data size: %u, update size: %u\n",
            u1Pidx, u4Rp, u4DataSize, u4UpdateSize);
        return FALSE;
    }

    _DMX_Lock();

    PID_S_W(u1Pidx, 9) = PHYSICAL(u4Rp);
    prPidStruct->u4Rp = u4Rp;

    _DMX_Unlock();

#ifdef TRACK_LAST_FRAME

    // Clear last-sent frame record if it is
    if (_DmxGetLastFrame(u1Pidx) == u4FrameAddr)
    {
        _DmxSetLastFrame(u1Pidx, 0);
    }

#endif

    // Debug
    DMXPIDLOG(DMX_LOG_BUFFER 7, u1Pidx, "Update RP - pidx: %u, RP: 0x%08x, frame: 0x%08x\n", u1Pidx, u4Rp,
        u4FrameAddr);

    return TRUE;
}


BOOL _DMX_UpdateReadPointer2(UINT8 u1Pidx, UINT32 u4Rp, UINT32 u4FrameAddr)
{
    PID_STRUCT_T* prPidStruct;

    prPidStruct = _DMX_GetPidStruct(u1Pidx);

    _DMX_Lock();

    prPidStruct->u4Rp = u4Rp;
    PID_S_W(u1Pidx, 9) = PHYSICAL(u4Rp);

    _DMX_Unlock();

    UNUSED(u4FrameAddr);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_UpdateVideoReadPointer
 *  Update read pointer of video PID buffer, called by video decoder
 *
 *  @param  u1Channel       The channel
 *  @param  u4Rp            The new read pointer
 *  @param  u4FrameAddr     The frame (picture) address
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_UpdateVideoReadPointer(UINT8 u1Channel, UINT32 u4Rp,
    UINT32 u4FrameAddr)
{
    UINT8 u1Pidx;

    // Get pidx from channel
    if (!_DMX_GetVideoPidx(u1Channel, &u1Pidx))
    {
        return FALSE;
    }

    return _DMX_UpdateReadPointer(u1Pidx, u4Rp, u4FrameAddr);
}


//-----------------------------------------------------------------------------
/** _DMX_UpdateVideoWritePointer
 *  Update write pointer of video buffer to decoder
 *
 *  @param  u1Channel       The channel
 *  @param  u4Wp            The new write pointer
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_UpdateVideoWritePointer(UINT8 u1Pidx, UINT8 u1Channel, UINT32 u4Wp)
{
    PID_STRUCT_T *prPidStruct;
    DMX_DECODER_CALLBACKS_T *prCallback;

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    if(prPidStruct == NULL)
    {
        return FALSE;
    }

    prCallback = _DMX_MUL_GetPidxDecoderCallback(u1Pidx);
    if((prCallback != NULL) && (prCallback->pfnUpdateVideoWp != NULL))
    {
        _DMX_TraceLog((UINT32)_DMX_UpdateVideoWritePointer, "_DMX_UpdateVideoWritePointer",
                    (UINT32)(prCallback->pfnUpdateVideoWp), "(prCallback->pfnUpdateVideoWp)",
                    DMX_TRACE_FUNCTION_CALL);

        prCallback->pfnUpdateVideoWp(u1Channel, u4Wp);

        _DMX_TraceLog((UINT32)_DMX_UpdateVideoWritePointer, "_DMX_UpdateVideoWritePointer",
                (UINT32)(prCallback->pfnUpdateVideoWp), "(prCallback->pfnUpdateVideoWp)",
                DMX_TRACE_FUNCTION_RETURN);
    }
    else
    {
        LOG(3, "%s - NULL callback!\n", __FUNCTION__);
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_SendPictureHeader
 *  Send picture header to MPEG decoder
 *
 *  @param  prPes           Video PES structure
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_SendPictureHeader(DMX_PES_MSG_T* prPes)
{
    PID_STRUCT_T* prPidStruct;
    DMX_VIDEO_TYPE_T eVideoType;

    ASSERT(prPes != NULL);

    // special handling for wmv
    eVideoType = _DMX_GetPidxVideoType(prPes->u1Pidx);
    switch(eVideoType)
    {
    case DMX_VIDEO_WMV7:
    case DMX_VIDEO_WMV8:
    case DMX_VIDEO_WMV9:
    case DMX_VIDEO_MP4_IN_WMV:
        if (! _DMX_MM_IsFrameHead())
        {
            return TRUE;        // To avoid the LOG message in _DmxProcessVideo*().
        }
        break;

    default:
        break;
    }

    prPidStruct = _DMX_GetPidStruct(prPes->u1Pidx);
    prPes->u1SerialNumber = prPidStruct->u1SerialNumber;
    prPes->pvInstanceTag = prPidStruct->pvInstanceTag;
    _DMX_MUL_ModifyPicCount(prPes->u1Pidx, TRUE);

    if (x_msg_q_send(_hDmxQueue, (VOID*)prPes, sizeof (DMX_PES_MSG_T),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        _DMX_MUL_ModifyPicCount(prPes->u1Pidx, FALSE);
        LOG(3, "Demux queue full!\n");
        return FALSE;
    }

    // Update picture counter
    prPidStruct->rCounters.u4PicCount++;

    // Debug
    DMXPIDLOG(DMX_LOG_AV 7, prPes->u1Pidx, "Send picture: 0x%08x, type: %u\n", prPes->u4FrameAddr,
        prPes->u4FrameType);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_SendAudioPes
 *  Send audio PES to audio decoder
 *
 *  @param  prPes           The audio PES structure
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_SendAudioPes(DMX_AUDIO_PES_T* prPes)
{
    BOOL fgRet = TRUE;
    PID_STRUCT_T *prPidStruct;
    DMX_DECODER_CALLBACKS_T *prCallback;

    if(prPes == NULL)
    {
        return FALSE;
    }

    prPidStruct = _DMX_GetPidStruct(prPes->u1PidIndex);
    if(prPidStruct == NULL)
    {
        return FALSE;
    }
    prPes->pvInstanceTag = prPidStruct->pvInstanceTag;

    prCallback = _DMX_MUL_GetPidxDecoderCallback(prPes->u1PidIndex);
    if ((prCallback != NULL) && (prCallback->pfnSendAudioPes != NULL))
    {
        _DMX_TraceLog((UINT32)_DMX_SendAudioPes, "_DMX_SendAudioPes",
                    (UINT32)(prCallback->pfnSendAudioPes), "(prCallback->pfnSendAudioPes)",
                    DMX_TRACE_FUNCTION_CALL);

        fgRet = prCallback->pfnSendAudioPes(prPes);

        _DMX_TraceLog((UINT32)_DMX_SendAudioPes, "_DMX_SendAudioPes",
                    (UINT32)(prCallback->pfnSendAudioPes), "(prCallback->pfnSendAudioPes)",
                    DMX_TRACE_FUNCTION_RETURN);
    }
    else
    {
        LOG(3, "%s - NULL callback!\n", __FUNCTION__);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_SendPurgeMessage
 *  Send full message to demux thread
 *
 *  @param  u1Pidx          PID index
 *  @param  fgReenablePid   Re-enable PID or not
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 *
 *  @context                ISR or thread
 */
//-----------------------------------------------------------------------------
BOOL _DMX_SendPurgeMessage(UINT8 u1Pidx, BOOL fgReenablePid)
{
    DMX_PES_MSG_T rPes;
    PID_STRUCT_T* prPidStruct;

    ASSERT(u1Pidx < DMX_NUM_PID_INDEX);

    prPidStruct = _DMX_GetPidStruct(u1Pidx);

    rPes.u1Pidx = u1Pidx;
    rPes.eMsgType = (fgReenablePid ? DMX_PES_MSG_TYPE_PURGE_REENABLE :
        DMX_PES_MSG_TYPE_PURGE);
    rPes.u1SerialNumber = prPidStruct->u1SerialNumber;
    if (x_msg_q_send(_hPurgeQueue, (VOID*)&rPes, sizeof (DMX_PES_MSG_T),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        LOG(3, "Demux queue full!\n");
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_NotifyEnablePid
 *  Be informed that a PID is enabling or disabling
 *
 *  @param  u1Pidx          PID index
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
void _DMX_NotifyEnablePid(UINT8 u1Pidx, BOOL fgEnable)
{
    ASSERT(u1Pidx < DMX_NUM_PID_INDEX);

    if (fgEnable)
    {

#ifdef TRACK_LAST_FRAME

        // Reset last-sent frame
        _DmxSetLastFrame(u1Pidx, 0);

#endif

        // Reset flags of sequence header and GOP
        _afgSeqHeader[u1Pidx] = FALSE;
        _afgGop[u1Pidx] = FALSE;
        _afgSeqEnd[u1Pidx] = FALSE;
    }
}


//-----------------------------------------------------------------------------
/** _DMX_InitHandler
 *  Initialize decoder interfaces
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_InitHandler(void)
{
    UINT32 i;

    static BOOL _fgInit = FALSE;

    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        _afgSeqHeader[i] = FALSE;
        _afgGop[i] = FALSE;
        _afgSeqEnd[i] = FALSE;

#ifdef TRACK_LAST_FRAME
        _au4LastFrame[i] = 0;
#endif
    }

    if (!_fgInit)
    {
        // Create picture queues
        if (x_msg_q_create(&_hDmxQueue, DMX_QUEUE_NAME,
            sizeof (DMX_PES_MSG_T), DMX_QUEUE_SIZE) != OSR_OK)
        {
            LOG(1, "Fail to create picture queue!\n");
            return FALSE;
        }

        // Create purge queues
        if (x_msg_q_create(&_hPurgeQueue, DMX_PURGE_QUEUE_NAME,
            sizeof (DMX_PES_MSG_T), DMX_QUEUE_SIZE) != OSR_OK)
        {
            LOG(1, "Fail to create purge queue!\n");
            return FALSE;
        }

        // Create picture thread
        if (x_thread_create(&_hDmxThread, DMX_THREAD_NAME,
            DMX_THREAD_STACK_SIZE, DMX_THREAD_PRIORITY, _DmxThread, 0,
            NULL) != OSR_OK)
        {
            LOG(1, "Fail to create DMX thread!\n");
            return FALSE;
        }

        // Create purge thread
        if (x_thread_create(&_hPurgeThread, DMX_PURGE_THREAD_NAME,
            DMX_THREAD_STACK_SIZE, DMX_THREAD_PRIORITY, _PurgeThread, 0,
            NULL) != OSR_OK)
        {
            LOG(1, "Fail to create purge thread!\n");
            return FALSE;
        }

        x_thread_set_affinity(_hDmxThread, 1);

        _fgInit = TRUE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_ClearPicFlag
 *  Clear Picture Global variables
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
BOOL _DMX_ClearPicFlag(UINT8 u1Pidx)
{
    if(u1Pidx >= DMX_NUM_PID_INDEX)
    {
        return FALSE;
    }

    _afgSeqHeader[u1Pidx] = FALSE;
    _au4SeqHeaderAddr[u1Pidx] = 0;
    _afgSeqHeaderPtsDts[u1Pidx] = FALSE;
    _au4SeqHeaderPts[u1Pidx] = 0;
    _au4SeqHeaderDts[u1Pidx] = 0;
    _afgGop[u1Pidx] = FALSE;
    _au4GopAddr[u1Pidx] = 0;
    _afgGopPtsDts[u1Pidx] = FALSE;
    _au4GopPts[u1Pidx] = 0;
    _au4GopDts[u1Pidx] = 0;
    _afgSeqEnd[u1Pidx] = FALSE;
    _afgSeqEndPtsDts[u1Pidx] = FALSE;
    _au4SeqEndPts[u1Pidx] = 0;
    _au4SeqEndDts[u1Pidx] = 0;

    #ifdef TRACK_LAST_FRAME
    _au4LastFrame[u1Pidx] = 0;
    #endif

    return TRUE;
}


BOOL _DMX_InitMonThread(void)
{
    static BOOL _fgInit = FALSE;

    if (!_fgInit)
    {
        #ifdef CC_DMX_ENABLE_MONTHREAD
        // Create picture thread
        if (x_thread_create(&_hDmxMonThread, DMX_MON_THREAD_NAME,
            DMX_MON_THREAD_STACK_SIZE, DMX_MON_THREAD_PRIORITY, _DmxMonThread, 0,
            NULL) != OSR_OK)
        {
            LOG(1, "Fail to create DMX monitor thread!\n");
            return FALSE;
        }
        #endif  // CC_DMX_ENABLE_MONTHREAD
        _fgInit = TRUE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
BOOL _DMX_SendEOSMessage(UINT8 u1Pidx)
{
    DMX_PES_MSG_T rPes;
    PID_STRUCT_T *prPidStruct;
    UINT8 u1Channel;

    if (!_DMX_IsToDecoder())
    {
        return TRUE;
    }

    // Most of the values are of no use except for fgEOS.
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    if ((prPidStruct->ePidType != DMX_PID_TYPE_ES_VIDEO) &&
        (prPidStruct->ePidType != DMX_PID_TYPE_ES_AUDIO))
    {
        return FALSE;
    }
    VERIFY(_DMX_GetVideoChannel(u1Pidx, &u1Channel));

    x_memset((void*)&rPes, 0, sizeof(rPes));
    rPes.eMsgType = DMX_PES_MSG_TYPE_PES;
    rPes.ePidType = prPidStruct->ePidType;
    rPes.u4FrameType = PIC_TYPE_I;
    rPes.u1Pidx = u1Pidx;
    rPes.u1Channel = u1Channel;
    rPes.u1DeviceId = prPidStruct->u1DeviceId;          // map to ucMpvId
    rPes.u1SerialNumber = prPidStruct->u1SerialNumber;
    rPes.fgEOS = TRUE;
    rPes.pvInstanceTag = prPidStruct->pvInstanceTag;

    // Update picture counter
    _DMX_MUL_ModifyPicCount(u1Pidx, TRUE);

    if (x_msg_q_send(_hDmxQueue, (VOID*)&rPes, sizeof(rPes),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        _DMX_MUL_ModifyPicCount(u1Pidx, FALSE);
        LOG(3, "The Demux message queue is full!\n");
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
BOOL _DMX_SendMoveCompleteMessage(UINT8 u1Pidx)
{
    DMX_PES_MSG_T rPes;
    PID_STRUCT_T *prPidStruct;
    UINT8 u1Channel;

    if (!_DMX_IsToDecoder())
    {
        return TRUE;
    }

    // Most of the values are of no use except for fgMoveComplete.
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    if ((prPidStruct->ePidType != DMX_PID_TYPE_ES_VIDEO) ||
        (prPidStruct->fgSendCompleteMsg != TRUE))
    {
        return FALSE;
    }
    VERIFY(_DMX_GetVideoChannel(u1Pidx, &u1Channel));

    x_memset((void*)&rPes, 0, sizeof(rPes));
    rPes.eMsgType = DMX_PES_MSG_TYPE_PES;
    rPes.ePidType = prPidStruct->ePidType;
    rPes.u4FrameType = PIC_TYPE_I;
    rPes.u1Pidx = u1Pidx;
    rPes.u1Channel = u1Channel;
    rPes.u1DeviceId = prPidStruct->u1DeviceId;          // map to ucMpvId
    rPes.u1SerialNumber = prPidStruct->u1SerialNumber;
    rPes.fgMoveComplete = TRUE;
    rPes.pvInstanceTag = prPidStruct->pvInstanceTag;

    // Update picture counter
    _DMX_MUL_ModifyPicCount(u1Pidx, TRUE);

    if (x_msg_q_send(_hDmxQueue, (VOID*)&rPes, sizeof(rPes),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        _DMX_MUL_ModifyPicCount(u1Pidx, FALSE);
        LOG(3, "The Demux message queue is full!\n");
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
void _DMX_ErrHanlde(void)
{
    DMX_PES_MSG_T rPes;

    x_memset((void*)&rPes, 0, sizeof(rPes));
    rPes.eMsgType = DMX_PES_MSG_TYPE_ERR_HANDLE;

#ifdef ENABLE_MULTIMEDIA
    // Important!!!  To avoid a potential deadlock...
    if (!_DMX_DDI_Unlock())
    {
        LOG(3, "Cannot unlock DDI!\n");
    }
/*
        if(eInputType == DMX_IN_PLAYBACK_PS)
        {
            return;  // Auto-reset is not performed for TS/PS file playback for the time being.
                     // Remember to remove the "return" statement if auto-reset is implemented.
        }
*/
#endif  // ENABLE_MULTIMEDIA

    if (x_msg_q_send(_hPurgeQueue, (VOID*)&rPes, sizeof(rPes),
        DMX_MSG_PRIORITY_NORMAL) != OSR_OK)
    {
        LOG(3, "The Demux message queue is full!\n");
    }
}

// for vdec buffer control
void _DMX_GetPicQSize(UINT16* pu2DmxPictureQSize, UINT16* pu2DmxMaxQSize)
{
    if(_hDmxQueue != NULL_HANDLE && pu2DmxPictureQSize != NULL && pu2DmxMaxQSize != NULL)
    {
        VERIFY(x_msg_q_num_msgs(_hDmxQueue, pu2DmxPictureQSize) == OSR_OK);
        *pu2DmxMaxQSize = DMX_QUEUE_SIZE;
    }
    else // error
    {
        if(pu2DmxPictureQSize != NULL && pu2DmxMaxQSize != NULL)
        {
            *pu2DmxPictureQSize = 0;
            *pu2DmxMaxQSize  = 0;
        }
    }
    return;
}
