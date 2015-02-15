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
 * $RCSfile: io_mtpvr.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor:  $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file io_mtpvr.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtpvr.h"
#include "aud_if.h"
#include "aud_drvif.h"
#include "dmx_if.h"
#include "dmx_mm_if.h"
#include "fvr_if.h"
#include "vdec_drvif.h"
#include "vdec_if.h"
#include "feeder_if.h"
#include "stc_drvif.h"
#include "mtauddec.h"
#include "b2r_if.h"

#include "cb_data.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>

#include "x_assert.h"
#include "swdmx_drvif.h"

#include <linux/fs.h>           //three include file for dump feeder data
#include <asm/uaccess.h>
#include <linux/param.h>
#include "x_fm.h"

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
#include "tz_if.h"
#endif

LINT_EXT_HEADER_BEGIN
#define DEFINE_IS_LOG    SWDMX_IsLog
#include "x_debug.h"
LINT_EXT_HEADER_END

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MHEG_AUDIO_MASTER           // MHEG5 use audio mater instead of system master
#define PVR0                        (0)
#define PVR_MAX_ID                  (1)
#define OUTPUT_SIZE_188             (188 * 128)
#define OUTPUT_SIZE_192             (192 * 128)
#define OUTPUT_SIZE_204             (204 * 128)
#define INPUT_SIZE_188              (282 * 1024)
#define INPUT_SIZE_204              (282 * 1024)
#define KERNEL_BUFFER_SIZE          (282 * 1024)

// This part is modified by LGE ( hwachin.lee ) Please don't remove it
#define INPUT_SIZE_192              (282 * 1024)     // Change the input size to DASY DMA unisze.
#define MAX_PIC_SIZE                (384 * 1024)    // To prevent picture data loss we need sufficient buffer margin
#define PRELOAD_BUFFER_SIZE_192_EX  (4 * 192 * 1024)

#define AUD_START_COND              (4 * 1024)
#define VID_START_COND              (30)
#define RESERVED_BUFFER_SIZE_188    ((INPUT_SIZE_188) + (OUTPUT_SIZE_188) + (1024 * 240))
#define RESERVED_BUFFER_SIZE_192    ((INPUT_SIZE_192) + (OUTPUT_SIZE_192) + (1024 * 240))
#define RESERVED_BUFFER_SIZE_204    ((INPUT_SIZE_204) + (OUTPUT_SIZE_204) + (1024 * 240))
#define RESERVED_ESMQ_THRESHOLD     (384)
#define RESERVED_PICQ_THRESHOLD     (512)
#define PTS_DRIFT                   (45000)

// INPUT_SIZE is for MTPVR write, OUTPUT_SIZE is for swdmx_mpg_pvr
#define PVRBUF_NUM                  ((MTPVR_PVRDATASIZE/INPUT_SIZE) + 2)
#define MTPVR_DATA_TIMER_THRSD      (16000)           // For extract thumbnail, should longer

#define MTPVR_THREAD_NAME            "MTPVR"
#define MTPVR_ES_THREAD_NAME         "MTPVR_ES"
#define MTPVR_THREAD_STACK_SIZE      (4096)
#define MTPVR_THREAD_PRIORITY        (100)
#define MTPVR_CMD_NAME               "MTPVR_CMD"
#define MTPVR_CMD_NAME_ES            "PVR_CMD"
#define MTPVR_FRAME_SIZE_NAME        "PVR_FRAME"
#define MTPVR_CMD_Q_SIZE             (16)
#define MTPVR_FRAME_Q_SIZE           (1024)

#define MTPVR_STAMP_UNDERRUN_DELAY      810
#define MTPVR_STAMP_DEFAULT_DELTA       3000
#define MTPVR_STAMP_FIFO_FULL_THRESHOLD (6 * 192 * 1024)

#define MTVDEC_NOTIFY_IFRAME         (1<<0)
#define MTVDEC_NOTIFY_THUMBNAIL      (1<<1)
#define MTVDEC_NOTIFY_DECODE_IFRAME  (1<<2)  // 20110801

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum
{
    ePVR_STATE_STOP,                // stop
    ePVR_STATE_PAUSE,               // pause
    ePVR_STATE_PLAY,                // play
    ePVR_CMD_FLUSH,                 // flush
    ePVR_STATE_MAX
} ENUM_MTPVR_PLAY_STATE_T;

typedef enum
{
    ePVR_CHECK_STREAM, 
    ePVR_NO_STREAM,
    ePVR_HAS_STREAM, 
    ePVR_STREAM_MAX
} ENUM_MTPVR_STREAM_STATE_T;

typedef enum
{
    ePVR_STEP_DONTCARE,             // not in step
    ePVR_STEP_START,                // in step
    ePVR_STEP_DONE,                 // step end
    ePVR_STEP_END,                  // step end
    ePVR_STEP_PREPARE,              // step open vdp
    ePVR_STEP_MAX
} ENUM_MTPVR_STEP_STATE_T;

typedef enum
{
    ePVR_BUFFER_UNDERFLOW,
    ePVR_BUFFER_READY,    
    ePVR_BUFFER_MAX
} ENUM_MTPVR_UNDERFLOW_STATE_T;

typedef struct PVR_STATUS
{
    BOOL        fgStarted;
    BOOL        fgH264ESMode;
    BOOL        fgFirst;
    BOOL        fgSpeedChange;
    BOOL        fgThumbTimerStarted;
    UCHAR       ucEsId;
    UCHAR       ucTsIdx;
    UINT8       u1AudId;
    UINT8       u1StcId;
    UINT8       u1B2rId;
    UINT8       u1DmxId;
    UINT8       u1DmxPid;
    
    UINT32      u4PvrBaseAddr;
    UINT32      u4PvrBufSize;
    UINT32      u4StartAddr;
    UINT32      u4EndAddr;
    UINT32      u4ReadAddr;
    UINT32      u4WriteAddr;
    UINT32      u4QueuedSize;
    
    // set secure buffer
    BOOL        fgSecure;
   
    // StampQ 
    BOOL    fgFoundPcr;
    UINT16  u2PcrPid;
    UINT32  u4LastTimeStamp;
    UINT32  u4LastTimeDelta;
    UINT32  u4PacketCountbwPcrs;
    INT64   i8LastPcr;
    UINT32  u4StampReadAddr;
    UINT32  u4StampWriteAddr;
    UINT32  u4StampQSize;
    UINT32  u4PreloadSize;

    UINT32      u4FrameSize;
    UINT32      u4FeederAddrPhy;
    UINT32      u4FeederAddrVir;    
    
    INT32       i4Speed;
    INT32       i4SpeedNew;
    
    HANDLE_T    hPVRMainThread;
    HANDLE_T    hPVRPlayESThread;
    HANDLE_T    hPVRPlayCmdQ;
    HANDLE_T    hPVRPlayESCmdQ;
    HANDLE_T    hPVRPlayFrameSizeQ;
    HANDLE_T    hThumbTimer;            // timer to keep watching input data
    
    DMX_PVR_PLAY_T                  rPvrPlay;
    ENUM_MTPVR_STREAM_STATE_T       ePVRVideoStatus;
    ENUM_MTPVR_STREAM_STATE_T       ePVRAudioStatus;
    ENUM_MTPVR_PLAY_STATE_T         ePVRPrevousStatus;
    ENUM_MTPVR_PLAY_STATE_T         ePVRStatus;
    ENUM_MTPVR_STEP_STATE_T         ePVRStep;
    ENUM_MTPVR_UNDERFLOW_STATE_T    eUnderFlowStatus;
    ENUM_MTPVR_PLAYMODE_T           ePlayMode;
} MTPVR_STATUS_T;


typedef struct
{
    BOOL   fgTriggerAudio;
    BOOL   fgAudioMaster;
    BOOL   fgGotVideo;
    BOOL   fgGotAudio;
    BOOL   fgGotAudioReady;
    BOOL   fgEnableAudio;
    BOOL   fgGotVideoReady;
    BOOL   fgVideoPause;
    BOOL   fgEnableVideo;
    BOOL   fgGetFirstAudPts;
    BOOL   fgCleanAudio;   
    UINT8  u1PidxAudio;    
    
    UINT32 u4FirstPTS;
    UINT32 u4FirstVideoPTS;
    UINT32 u4FirstAudioPTS;   
    UINT32 u4LastVideoPTS;
    UINT32 u4LastAudioPTS;
    UINT32 u4LastAudioWptr;
}MTPVR_MPG_VARIABLE_T;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
extern MT_RESULT_T  _MTAUD_SetAvSynMode (UINT8 u1DecId,MTAV_SYNC_MODE_T eSynMode);
extern MT_RESULT_T _MTAUD_SetSoureFrom(UINT8 u1DecId,MTAUDDEC_DEC_STREAM_FROM_T eStreamFrom);
extern INT32 AUD_GetAudFifo(UINT8 u1DecId, UINT32 * pu4FifoStart, UINT32 * pu4FifoEnd);
extern BOOL _MTVDEC_QueryVdecStatus(UCHAR ucEsId);
extern UINT32 AUD_AudioDecoderState(UINT8 u1DecId);
extern void _MTDMX_SetDecoderEx(UINT8 u1InstId);
extern UINT32 _MtdmxGetAudioPidx(VOID);
extern void AUD_MMAoutEnable(UINT8 u1DecId, BOOL fgEnable);


#ifdef VDEC_IS_POST_MT5368
extern VOID _DMX_SetTimeShiftMode(BOOL fgEnable);
#endif

extern void _MTVDEC_Notify_Lock(void);
extern void _MTVDEC_Notify_Unlock(void);

extern void _MTVDEC_VDEC_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2);
    
extern MT_RESULT_T  _MTAUD_Play (UINT8 u1DecId);
extern MT_RESULT_T  _MTAUD_Stop (UINT8 u1DecId);
extern MT_RESULT_T  _MTAUD_CheckPvrPause (INT32 i4Speed);

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)       \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       (size)))                             \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    } \

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtpvr argument error\n");      \
        return MTR_ERR_INV;                                          \
    } \

#define DATASIZE(rp, wp, size)      \
    (((rp) <= (wp)) ? ((wp) - (rp)) : (((wp) + (size)) - (rp)))
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static BOOL _fgPVRInit = FALSE;
static MTPVR_STATUS_T _rPVRStatus = {0};
static UINT32 _u4ThumbnailId = 0;
static BOOL _fgThumbnail = FALSE;
static BOOL _fgThumbnail2 = FALSE;
static HANDLE_T _hQueueSema = 0;
static BOOL _fgPVRLocking = FALSE;
//static BOOL _fgPVRThumbnailLocking = FALSE;
static CRIT_STATE_T _rPVRLock;
static VDP_THUMBNAIL_INFO_T _rThumbDstInfo;
static UCHAR _ucThumbVdecId = ES0;
static UCHAR _ucExtractThumbVdecId = ES0;
extern UINT32 _u4NotifyMask;
extern UCHAR _aucMTVDECEsId[VDEC_MAX_ES];
static UINT32 _u4OutputSize = OUTPUT_SIZE_192;
static UINT32 _u4InputSize = INPUT_SIZE_192;
static UINT32 _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_192;
static BOOL _fgContainTimeStamp = TRUE;
//static BOOL _fgIs4BytesPcrMode = FALSE;
static BOOL _fgEOS = FALSE;
static MTPVR_MPG_VARIABLE_T _rMtPvrMpgInfo = {0};
static HANDLE_T _hStopSema = 0;
static HANDLE_T _hResetSema = 0;
static FBM_POOL_T *_prDmxFbmPool = NULL;
static UINT32 _u4PvrDumpFlag = 0;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/** _MTPVR_Lock
 *  Enter demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _MTPVR_Lock(void)
{
    _rPVRLock = x_crit_start();

    ASSERT(!_fgPVRLocking);
    _fgPVRLocking = TRUE;

    UNUSED(_fgPVRLocking);          // Make Lint happy
}

//-----------------------------------------------------------------------------
/** _MTPVR_Unlock
 *  Leave demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _MTPVR_Unlock(void)
{
    ASSERT(_fgPVRLocking);
    _fgPVRLocking = FALSE;

    x_crit_end(_rPVRLock);

    UNUSED(_fgPVRLocking);          // Make Lint happy
}

#if 0
static void _MTPVR_ThumbnailLock(void)
{
    if (_hQueueSema)
    {
        VERIFY(x_sema_lock(_hQueueSema, X_SEMA_OPTION_WAIT) == OSR_OK);
        _fgPVRThumbnailLocking = TRUE;
    }
}

static void _MTPVR_ThumbnailUnlock(void)
{
    if (_hQueueSema)
    {
        _fgPVRThumbnailLocking = FALSE;
        VERIFY(x_sema_unlock(_hQueueSema) == OSR_OK);
    }
}
#endif

static VOID _MTPVR_ClearAudio(VOID)
{
    INT32 i = 0;
    DMX_PID_T rDrvPid;
    
    x_memset(&rDrvPid, 0, sizeof(rDrvPid));
    for (i=0; i<DMX_NUM_PID_INDEX; i++)
    {
        if (!DMX_GetPid(i, DMX_PID_FLAG_TS_INDEX|DMX_PID_FLAG_BUFFER|DMX_PID_FLAG_VALID, &rDrvPid))
        {
            LOG(1, "Get Pid ePidtype fail in io_mtpvr %d\n",i);
            
            return;
        }
        
        if ((rDrvPid.ePidType==DMX_PID_TYPE_ES_AUDIO) && (rDrvPid.fgEnable==TRUE) && (rDrvPid.u1TsIndex==2))
        {
            LOG(3, "flush auido buffer pid %d\n",i);
            DMX_FlushPidBuffer(i);
            
            break;
        }
    }
}

static VOID _MTPVR_CleanBuf(BOOL fgCleanStampQ)
{
    if (fgCleanStampQ)
    {
        _rPVRStatus.u4StampReadAddr = _rPVRStatus.u4StampWriteAddr;
        _rPVRStatus.fgFoundPcr = FALSE;
        _rPVRStatus.u2PcrPid = 0;
        _rPVRStatus.u4LastTimeStamp = 0;
        _rPVRStatus.u4LastTimeDelta = 0;
        _rPVRStatus.u4PacketCountbwPcrs = 0;
        _rPVRStatus.i8LastPcr = 0;
        _rPVRStatus.u4PreloadSize = PRELOAD_BUFFER_SIZE_192_EX;
        _MTPVR_Lock();
        _rPVRStatus.u4StampQSize = 0;
        _MTPVR_Unlock();
    }

    _rPVRStatus.fgFirst = TRUE;
    _rPVRStatus.u4ReadAddr = _rPVRStatus.u4WriteAddr;
    _MTPVR_Lock();
    _rPVRStatus.u4QueuedSize = 0;
    _rPVRStatus.u4FrameSize = 0;
    _MTPVR_Unlock();

    MTPVR_PRINT("%s done\n", __FUNCTION__);
}

void _MTPVR_VDEC_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2)
{
    MTPVR_THUMBNAIL_CB_T rThumbCb;
    MTVDEC_PRINT("_MTPVR_VDEC_Nfy\n");

    x_memset(&rThumbCb, 0, sizeof(MTPVR_THUMBNAIL_CB_T));
    
    // _MTPVR_ThumbnailLock();
    if (eNfyCond == VDEC_COND_THUMBNAIL_DONE)
    {
        MTVDEC_PRINT(" - Vdec thumbnail done\n");
        if(_fgThumbnail)
        {
            LOG(5, "%s(L:%d) DONE\n", __FUNCTION__, __LINE__);
            // notify
            rThumbCb.fgOK = TRUE;
            rThumbCb.u4Id = _u4ThumbnailId;
            rThumbCb.u4DeltaMs = u4Data1;
            rThumbCb.eResult    = E_MTPVR_THUMBNAIL_RESULT_DONE;
            _CB_PutEvent(CB_MTAL_PVR_THUMBNAIL_TRIGGER,
                sizeof(MTPVR_THUMBNAIL_CB_T),
                (void*)&rThumbCb);
            _fgThumbnail = FALSE;
            _fgThumbnail2 = FALSE;
        }
    }
    else if (eNfyCond == VDEC_COND_THUMBNAIL_FAIL)
    {
        MTVDEC_PRINT(" - Vdec thumbnail error(%d)\n", u4Data1);
        if (_fgThumbnail)
        {
            LOG(3, "%s(L:%d) FAIL\n", __FUNCTION__, __LINE__);
            // notify a black image
            rThumbCb.fgOK = TRUE;
            rThumbCb.u4Id = _u4ThumbnailId;
            rThumbCb.eResult    = E_MTPVR_THUMBNAIL_RESULT_FAIL;
            _CB_PutEvent(CB_MTAL_PVR_THUMBNAIL_TRIGGER,
                sizeof(MTPVR_THUMBNAIL_CB_T),
                (void*)&rThumbCb);
            _fgThumbnail = FALSE;
            _fgThumbnail2 = FALSE;
        }
    }
    else if (VDEC_COND_PLAY_DONE == eNfyCond)       // Play done was sent in DeliveEos, _fgThumbnail=TRUE means no frame was decoded
    {
         MTVDEC_PRINT(" - Vdec thumbnail No data(%d)\n", u4Data1);
        if (_fgThumbnail)
        {
            LOG(3, "%s(L:%d) no data.\n", __FUNCTION__, __LINE__);
            // notify a black image
            rThumbCb.fgOK = FALSE;
            rThumbCb.u4Id = _u4ThumbnailId;
            rThumbCb.eResult    = E_MTPVR_THUMBNAIL_RESULT_NO_DATA;
            _CB_PutEvent(CB_MTAL_PVR_THUMBNAIL_TRIGGER,
                sizeof(MTPVR_THUMBNAIL_CB_T),
                (void*)&rThumbCb);
            _fgThumbnail = FALSE;
            _fgThumbnail2 = FALSE;
        }
    }
    else
    {
        // Do not care
    }
    
    //_MTPVR_ThumbnailUnlock();

    UNUSED(pvNfyTag);
    UNUSED(eNfyCond);
    UNUSED(u4Data1);
    UNUSED(u4Data2);
}


static VOID _MTPVR_ThumbTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    if (_fgThumbnail)
    {
        LOG(3, "Notify thumbnail timeout\n");
        _MTPVR_VDEC_Nfy((void*)pv_tag, VDEC_COND_THUMBNAIL_FAIL, 0, 0);
    }

    UNUSED(pt_tm_handle);
}

static VOID _MTPVR_StartTimer(UCHAR ucPvrId, UCHAR ucVdecId)
{
    if (ucVdecId >= VDEC_MAX_ES)
    {
        LOG(1, "%s(L:%d) ucVdecId(%d) error\n", __FUNCTION__, __LINE__, ucVdecId);
        return;
    }
    
    if (_rPVRStatus.hThumbTimer && !_rPVRStatus.fgThumbTimerStarted)
    {
        VERIFY(x_timer_start(_rPVRStatus.hThumbTimer, MTPVR_DATA_TIMER_THRSD, X_TIMER_FLAG_ONCE,
            _MTPVR_ThumbTimeout, (void*)&_aucMTVDECEsId[ucVdecId]) == OSR_OK);
        _rPVRStatus.fgThumbTimerStarted = TRUE;
    }
    
    UNUSED(ucPvrId);
}

static VOID _MTPVR_StopTimer(UCHAR ucPvrId)
{
    if (_rPVRStatus.hThumbTimer && _rPVRStatus.fgThumbTimerStarted)
    {
        VERIFY(x_timer_stop(_rPVRStatus.hThumbTimer) == OSR_OK);
        _rPVRStatus.fgThumbTimerStarted = FALSE;
    }
    
    UNUSED(ucPvrId);
}

static VOID _MTPVR_StopFinished(VOID)
{
    if (_fgContainTimeStamp)
    {
        MTPVR_PRINT("Stop Finished\n");
        VERIFY (x_sema_unlock(_hStopSema) == OSR_OK);
    }
}

static VOID _MTPVR_ResetBufferFinished(VOID)
{
    MTPVR_PRINT("Reset Finished\n");
    VERIFY (x_sema_unlock(_hResetSema) == OSR_OK);
}

static BOOL _MTPVR_ReceiveFrameSize(UCHAR ucPvrId, VOID *pvData)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("_MTPVR_ReceiveFrameSize (ucPvrId >= PVR_SOURCE_MAX)\n");
    }

    zMsgSize = sizeof(UINT32);

    i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                            &(_rPVRStatus.hPVRPlayFrameSizeQ), 1, X_MSGQ_OPTION_NOWAIT);

    ASSERT(zMsgSize == sizeof(UINT32));

    if (i4Ret != OSR_OK)
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL _MTPVR_ReceivePlayESCmd(UCHAR ucPvrId, VOID *pvData, UINT32 u4CurState)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("_PVR_MainLoop (ucPvrId >= PVR_SOURCE_MAX)\n");
    }

    zMsgSize = sizeof(UINT32);
    if (u4CurState != ePVR_STATE_PLAY)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(_rPVRStatus.hPVRPlayESCmdQ), 1, X_MSGQ_OPTION_WAIT);
    }
    else
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(_rPVRStatus.hPVRPlayESCmdQ), 1, X_MSGQ_OPTION_NOWAIT);
    }

    ASSERT(zMsgSize == sizeof(UINT32));

    if (i4Ret != OSR_OK)
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL _MTPVR_ReceivePlayCmd(UCHAR ucPvrId, VOID *pvData, UINT32 u4CurState)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;

    // ENUM_MTPVR_PLAY_STATE_T eStatus;
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("_PVR_MainLoop (ucPvrId >= PVR_SOURCE_MAX)\n");
    }

    zMsgSize = sizeof(UINT32);
    if (u4CurState != ePVR_STATE_PLAY)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(_rPVRStatus.hPVRPlayCmdQ), 1, X_MSGQ_OPTION_WAIT);
    }
    else
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(_rPVRStatus.hPVRPlayCmdQ), 1, X_MSGQ_OPTION_NOWAIT);
    }

    ASSERT(zMsgSize == sizeof(UINT32));

    if (i4Ret != OSR_OK)
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL _MTPVR_SendPlayESCmd(UCHAR ucPvrId, ENUM_MTPVR_PLAY_STATE_T ePlayState, UCHAR ucPriority)
{
    UINT32 u4TryCnt = 0;
    INT32 i4Ret;
    UINT32 u4PlayCmd = (UINT32)ePlayState;
    SIZE_T zMsgSize = sizeof(UINT32);

    i4Ret = x_msg_q_send(_rPVRStatus.hPVRPlayESCmdQ, &u4PlayCmd, zMsgSize, ucPriority);
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(100);
        LOG(2, "PVR Cmd(%d) Queue, OSR_TOO_MANY\n", u4PlayCmd);
        i4Ret = x_msg_q_send(_rPVRStatus.hPVRPlayESCmdQ, &u4PlayCmd, zMsgSize, ucPriority);
        u4TryCnt ++;
        
        if (u4TryCnt > 50)
        {
            LOG(0, "PVR Cmd(%d) Queue, OSR_TOO_MANY -> exit\n", u4PlayCmd);
            break;
        }
    }

    VERIFY(i4Ret == OSR_OK);

    return TRUE;
}

static BOOL _MTPVR_SendPlayCmd(UCHAR ucPvrId, ENUM_MTPVR_PLAY_STATE_T ePlayState, UCHAR ucPriority)
{
    UINT32 u4TryCnt = 0;
    INT32 i4Ret;
    UINT32 u4PlayCmd = (UINT32)ePlayState;
    SIZE_T zMsgSize = sizeof(UINT32);

    i4Ret = x_msg_q_send(_rPVRStatus.hPVRPlayCmdQ, &u4PlayCmd, zMsgSize, ucPriority);
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(100);
        
        LOG(2, "PVR Cmd(%d) Queue, OSR_TOO_MANY\n", u4PlayCmd);
        i4Ret = x_msg_q_send(_rPVRStatus.hPVRPlayCmdQ, &u4PlayCmd, zMsgSize, ucPriority);
        u4TryCnt ++;
        
        if (u4TryCnt > 50)
        {
            LOG(0, "PVR Cmd(%d) Queue, OSR_TOO_MANY -> exit\n", u4PlayCmd);
            break;
        }
    }

    VERIFY(i4Ret == OSR_OK);

    return TRUE;
}

#if 0
BOOL _MTPVR_SetPcrPid(UINT16 u2PcrPid)
{
    if (_fgIs4BytesPcrMode)
    {
        Printf("MTPVR - old PcrPid: %d, new PcrPid: %d\n", _rPVRStatus.u2PcrPid, u2PcrPid);
        _rPVRStatus.u2PcrPid = u2PcrPid;
    }

    return TRUE;
}
#endif

#if 0
static BOOL _MTPVR_IsStampQFull(void)
{
    UINT32 u4BufStart, u4BufEnd;
    UINT32 u4BufSize, u4WorkSize;
    UINT32 u4StampReadAddr, u4StampWriteAddr;
    BOOL fgIsFull = FALSE;

    u4BufStart = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StartAddr;
    u4BufEnd = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4EndAddr;
    u4StampReadAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StampReadAddr;
    u4StampWriteAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StampWriteAddr;
    u4BufSize = u4BufEnd - u4BufStart;
    u4WorkSize = DATASIZE(u4StampReadAddr, u4StampWriteAddr, u4BufSize); 
    fgIsFull = (u4WorkSize >= MTPVR_STAMP_FIFO_FULL_THRESHOLD) ? TRUE : FALSE;

    return fgIsFull;
}

static BOOL _MTPVR_SetTimeStamp(UINT32 u4TimeDelta)
{
    UINT32 u4BufStart, u4BufEnd;
    UINT8 *puCurPkt;
    UINT32 u4Offset;
    UINT32 u4BufSize, u4WorkSize;
    UINT32 u4LocalTimeStamp = 0, u4LastTimeStamp;
    UINT32 u4StampReadAddr, u4StampWriteAddr;
    DMX_PVRPLAY_BUFPTR_T rPVRPlayBuf;
    BOOL fgIsStampAdjusted = FALSE;
        
    u4LastTimeStamp = _rPVRStatus.u4LastTimeStamp;
    u4BufStart = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StartAddr;
    u4BufEnd = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4EndAddr;
    u4StampReadAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StampReadAddr;
    u4StampWriteAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StampWriteAddr;
    u4BufSize = u4BufEnd - u4BufStart;
    u4WorkSize = DATASIZE(u4StampReadAddr, u4StampWriteAddr, u4BufSize); 

    DMX_PVRPlay_GetBufPointer(&rPVRPlayBuf);
    if (rPVRPlayBuf.u4HwWp == rPVRPlayBuf.u4HwRp)
    {
        /* buffer underrun */
        DMX_PVRPlay_GetTimeStamp(0, &u4LocalTimeStamp);
        if ((u4LastTimeStamp + u4TimeDelta - MTPVR_STAMP_UNDERRUN_DELAY) < u4LocalTimeStamp)
        {
            u4LastTimeStamp = u4LocalTimeStamp + MTPVR_STAMP_UNDERRUN_DELAY;
            fgIsStampAdjusted = TRUE;
        }
    }

    for (u4Offset = 0; u4Offset < u4WorkSize; u4Offset+=192)
    {
        puCurPkt = (UINT8 *)u4StampReadAddr + u4Offset;
        if ((UINT32)puCurPkt >= u4BufEnd)
        {
            puCurPkt -= u4BufSize;
        }
        
        if (fgIsStampAdjusted)
        {
            fgIsStampAdjusted = FALSE;
        }
        else
        {
            u4LastTimeStamp += u4TimeDelta;
        }
        puCurPkt[0] = (u4LastTimeStamp >> 24) & 0xFF;
        puCurPkt[1] = (u4LastTimeStamp >> 16) & 0xFF;
        puCurPkt[2] = (u4LastTimeStamp >> 8) & 0xFF;
        puCurPkt[3] = u4LastTimeStamp & 0xFF;
    }

    _rPVRStatus.u4LastTimeStamp = u4LastTimeStamp;
    _rPVRStatus.u4StampReadAddr += u4WorkSize;
    if(_rPVRStatus.u4StampReadAddr >= _rPVRStatus.u4EndAddr)
    {
        _rPVRStatus.u4StampReadAddr -= u4BufSize;
    }

    _MTPVR_Lock();
    _rPVRStatus.u4StampQSize += u4WorkSize;
    _MTPVR_Unlock();

    return TRUE;
}

static BOOL _MTPVR_GetTimeStampDelta(UINT32 *pu4RetDelta)
{
#define MAX_PCR_27MHZ           2576980377600ll
#define MAX_PCR_DELTA           (500 * 27000)

    UINT8 *puCurPkt;
    UINT32 u4StampWriteAddr, u4ReadAddr, u4BufStart, u4BufEnd;
    UINT32 u4BufSize, u4QueuedSize, u4WorkSize;
    UINT32 u4Offset;
    UINT16 u2Pid;
    BOOL fgResult = TRUE;

    if (!pu4RetDelta)
    {
        fgResult = FALSE;
        goto lbExit1;
    }
    
    *pu4RetDelta = 0;
    u4QueuedSize = _rPVRStatus.u4QueuedSize;
    u4BufStart = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StartAddr;
    u4BufEnd = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4EndAddr;
    u4BufSize = u4BufEnd - u4BufStart;
    u4ReadAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4ReadAddr + u4QueuedSize;
    u4StampWriteAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StampWriteAddr;

    if (u4ReadAddr >= u4BufEnd)
    {
        u4ReadAddr -= u4BufSize;
    }
 
    u4WorkSize = DATASIZE(u4StampWriteAddr, u4ReadAddr, u4BufSize); 
    ASSERT((u4WorkSize % 192) == 0);
    for (u4Offset = 0; u4Offset < u4WorkSize; u4Offset+=192)
    {
        puCurPkt = (UINT8 *)(u4StampWriteAddr + u4Offset);
        if ((UINT32)puCurPkt >= u4BufEnd)
        {
            puCurPkt -= u4BufSize;
        }
       
        if (_rPVRStatus.fgFoundPcr)
        {
            _rPVRStatus.u4PacketCountbwPcrs++;
        }
 
        puCurPkt += 4;
        if (puCurPkt[0] != 0x47)
        {
            //Printf("_MTPVR_GetTimeStampDelta - ERROR not sync\n");
            continue;
        }

        /* adaptation field present? */
        if (puCurPkt[3] & 0x20)
        {
            /* if adaptation field length is non-zero */
            if (puCurPkt[4] != 0)
            {
                /* if PCR bit on adaptation field is set */
                if (puCurPkt[5] & 0x10)
                {
                    INT64 i8Pcr1, i8Pcr2, i8Pcr3, i8CurrPcr;
            
                    u2Pid = (UINT16)(puCurPkt[1] & 0x1f) << 8 | (UINT16)puCurPkt[2];
                    if (_rPVRStatus.u2PcrPid == 0)
                    {
                        _rPVRStatus.u2PcrPid = u2Pid;
                    }

                    if (_rPVRStatus.u2PcrPid != u2Pid)
                    {
                        continue;
                    }

                    i8Pcr1 =  ((INT64)puCurPkt[6]<<8)+puCurPkt[7];
                    i8Pcr2 =  ((INT64)puCurPkt[8]<<8)+puCurPkt[9];
                    i8Pcr3 =  ((INT64)puCurPkt[10]<<8)+puCurPkt[11];
                    i8CurrPcr = ((((INT64)i8Pcr1)<<17) + (i8Pcr2<<1) + (i8Pcr3>>15)) * 300 + (i8Pcr3 & 0x1FF);
                    //Printf("PCR: %llu Count: %u Size: %u\n", i8CurrPcr, _u4PCRCount, u4ProcessSize);
                    if (_rPVRStatus.fgFoundPcr)
                    {
                        INT64 i8PcrDelta;

                        i8PcrDelta = i8CurrPcr - _rPVRStatus.i8LastPcr;
    
                        if (i8PcrDelta < 0) 
                        {
                            i8PcrDelta += MAX_PCR_27MHZ;
                        }

                        if (i8PcrDelta >= MAX_PCR_DELTA)
                        {
                            /* PCR might be changed, re-check PCR */
                            Printf("**_MTPVR_GetTimeStampDelta error - a big PCR drift**\n");
                            fgResult = FALSE;
                        }

                        *pu4RetDelta = ((UINT32)i8PcrDelta/_rPVRStatus.u4PacketCountbwPcrs);
                        _rPVRStatus.fgFoundPcr = FALSE;
                        _rPVRStatus.u4PacketCountbwPcrs = 0;
                        break;
                    }

                    _rPVRStatus.fgFoundPcr = TRUE;
                    _rPVRStatus.i8LastPcr = i8CurrPcr;
                    _rPVRStatus.u4PacketCountbwPcrs = 0;
                }
            }
        }
    }

    _rPVRStatus.u4StampWriteAddr += u4Offset;
    if(_rPVRStatus.u4StampWriteAddr >= _rPVRStatus.u4EndAddr)
    {
        _rPVRStatus.u4StampWriteAddr -= u4BufSize;
    }

lbExit1:

    return fgResult;
}
#endif

BOOL _Mtpvr_SetPlayBackMode(ENUM_MTPVR_PLAYMODE_T ePlayMode)
{
    if (ePlayMode == ENUM_MTPVR_PLAYMODE_188)
    {
        _MTPVR_Lock();
        _u4OutputSize = OUTPUT_SIZE_188;
        _u4InputSize = INPUT_SIZE_188;
        _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_188;
        _fgContainTimeStamp = FALSE;
        _rPVRStatus.ePlayMode = ENUM_MTPVR_PLAYMODE_188;
        _MTPVR_Unlock();
#ifdef MHEG_AUDIO_MASTER
        _rMtPvrMpgInfo.fgAudioMaster = TRUE;
        STC_SetSyncMode(_rPVRStatus.u1StcId, _rPVRStatus.u1AudId, PVR0, AV_SYNC_MODE_AUDIO_MASTER);        
#else
        _rMtPvrMpgInfo.fgAudioMaster = FALSE;
        STC_SetSyncMode(_rPVRStatus.u1StcId, _rPVRStatus.u1AudId, PVR0, AV_SYNC_MODE_SYSTEM_MASTER);        
#endif
        VDEC_SetH264MMErrMask(_rPVRStatus.ucEsId, TRUE);
    }
    else if (ePlayMode == ENUM_MTPVR_PLAYMODE_192)
    {
        _MTPVR_Lock();
        _u4OutputSize = OUTPUT_SIZE_192;
        _u4InputSize = INPUT_SIZE_192;
        _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_192;
        _fgContainTimeStamp = TRUE;
        _rMtPvrMpgInfo.fgAudioMaster = FALSE;
        _rPVRStatus.ePlayMode = ENUM_MTPVR_PLAYMODE_192;
        _MTPVR_Unlock();
    }
    else if (ENUM_MTPVR_PLAYMODE_188_SYSMASTER == ePlayMode)
    {
        _MTPVR_Lock();
        _u4OutputSize = OUTPUT_SIZE_188;
        _u4InputSize = INPUT_SIZE_188;
        _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_188;
        _fgContainTimeStamp = FALSE;
        _rPVRStatus.fgH264ESMode = FALSE;
        _rMtPvrMpgInfo.fgAudioMaster = FALSE;
        _rPVRStatus.ePlayMode = ENUM_MTPVR_PLAYMODE_188_SYSMASTER;
        _MTPVR_Unlock();

        STC_SetSyncMode(_rPVRStatus.u1StcId, _rPVRStatus.u1AudId, PVR0, AV_SYNC_MODE_SYSTEM_MASTER);
    }
    else if (ENUM_MTPVR_PLAYMODE_192_AUDIO_MASTER == ePlayMode)
    {
        _MTPVR_Lock();
        _u4OutputSize = OUTPUT_SIZE_192;
        _u4InputSize = INPUT_SIZE_192;
        _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_192;
        _fgContainTimeStamp = FALSE;
        _rPVRStatus.fgH264ESMode = FALSE;
        _rMtPvrMpgInfo.fgAudioMaster = TRUE;
        _rPVRStatus.ePlayMode = ENUM_MTPVR_PLAYMODE_192_AUDIO_MASTER;
        _MTPVR_Unlock();

        STC_SetSyncMode(_rPVRStatus.u1StcId, _rPVRStatus.u1AudId, PVR0, AV_SYNC_MODE_AUDIO_MASTER);
    }
    else if (ENUM_MTPVR_PLAYMODE_188_AUDIO_MASTER == ePlayMode)
    {
        _MTPVR_Lock();
        _u4OutputSize = OUTPUT_SIZE_188;
        _u4InputSize = INPUT_SIZE_188;
        _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_188;
        _fgContainTimeStamp = FALSE;
        _rPVRStatus.fgH264ESMode = FALSE;
        _rMtPvrMpgInfo.fgAudioMaster = TRUE;
        _rPVRStatus.ePlayMode = ENUM_MTPVR_PLAYMODE_188_AUDIO_MASTER;
        _MTPVR_Unlock();
        
        STC_SetSyncMode(_rPVRStatus.u1StcId, _rPVRStatus.u1AudId, PVR0, AV_SYNC_MODE_AUDIO_MASTER);
    }
    else if (ENUM_MTPVR_PLAYMODE_204 == ePlayMode)
    {
        _MTPVR_Lock();
        _u4OutputSize = OUTPUT_SIZE_204;
        _u4InputSize = INPUT_SIZE_204;
        _u4ReservedBufferSize = RESERVED_BUFFER_SIZE_204;
        _fgContainTimeStamp = FALSE;
        _rPVRStatus.fgH264ESMode = FALSE;
        _rMtPvrMpgInfo.fgAudioMaster = TRUE;
        _rPVRStatus.ePlayMode = ENUM_MTPVR_PLAYMODE_204;
        _MTPVR_Unlock();
        
        STC_SetSyncMode(_rPVRStatus.u1StcId, _rPVRStatus.u1AudId, PVR0, AV_SYNC_MODE_AUDIO_MASTER);
    }
    else
    {
        LOG(5, "Not support packet size: %d\n", ePlayMode);
        return FALSE;
    }

    LOG(5, "%s Play mode(%d)\n", __FUNCTION__, ePlayMode);

    return TRUE;
}


/*
    play mode 
*/
ENUM_MTPVR_PLAYMODE_T _MTPVR_QueryPlayMode(UCHAR ucPvrId)
{
    UNUSED(ucPvrId);
    return _rPVRStatus.ePlayMode;
}

/*
    reset variable of mpg info
*/
VOID _MTPVR_ResetMpgInfo(UCHAR ucPvrId)
{
    UNUSED(ucPvrId);
    
    LOG(1,"Reset MPG INFO.. \n");
    
    _rMtPvrMpgInfo.u4FirstPTS = 0xFFFFFFFF;
    _rMtPvrMpgInfo.u4FirstVideoPTS = 0xFFFFFFFF;
    _rMtPvrMpgInfo.u4FirstAudioPTS = 0xFFFFFFFF;
    _rMtPvrMpgInfo.u4LastAudioPTS  = 0xFFFFFFFF;
    _rMtPvrMpgInfo.u4LastAudioWptr = 0;
    _rMtPvrMpgInfo.u4LastVideoPTS = 0;
    _rMtPvrMpgInfo.u1PidxAudio = 0xFF;
    _rMtPvrMpgInfo.fgGotVideo = FALSE;
    _rMtPvrMpgInfo.fgGotAudio = FALSE;
    _rMtPvrMpgInfo.fgGotAudioReady = FALSE;
    _rMtPvrMpgInfo.fgGotVideoReady = FALSE;
    _rMtPvrMpgInfo.fgGetFirstAudPts = FALSE;
    _rMtPvrMpgInfo.fgCleanAudio = FALSE;
    _rMtPvrMpgInfo.fgVideoPause = FALSE;
    _rMtPvrMpgInfo.fgEnableAudio = FALSE;
    _rMtPvrMpgInfo.fgEnableVideo = FALSE;
    _rMtPvrMpgInfo.fgAudioMaster = FALSE;
    _rMtPvrMpgInfo.fgTriggerAudio = FALSE;
    _rPVRStatus.eUnderFlowStatus = ePVR_BUFFER_MAX;

    return ;
}

/*
    _MTPVR_ResetMpgAudioInfo, When change audio channel, call it;
*/
static MT_RESULT_T _MTPVR_ResetMpgAudioInfo(unsigned long arg)
{  
    UCHAR ucPvrId;

    ucPvrId = (UCHAR)arg;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    LOG(0,"Reset MPG AUDIO INFO.. \n");
    
    _rMtPvrMpgInfo.u4FirstAudioPTS = 0xFFFFFFFF;
    _rMtPvrMpgInfo.u4LastAudioPTS  = 0xFFFFFFFF;
    _rMtPvrMpgInfo.u4LastAudioWptr = 0;
    _rMtPvrMpgInfo.u1PidxAudio = 0xFF;
    _rMtPvrMpgInfo.fgGotAudio = FALSE;
    _rMtPvrMpgInfo.fgGotAudioReady = FALSE;
    _rMtPvrMpgInfo.fgGetFirstAudPts = FALSE;
    _rMtPvrMpgInfo.fgCleanAudio = FALSE;
    _rMtPvrMpgInfo.fgEnableAudio = FALSE;
    _rMtPvrMpgInfo.fgTriggerAudio = FALSE;
    
    return MTR_OK;
}

/*
    take care of video pts
*/
MT_RESULT_T _MTPVR_MonitorVideoPTS(UCHAR ucPvrId, const DMX_PES_MSG_T* prPes)
{
    UNUSED(ucPvrId);
    
    if (prPes == NULL)
    {
        LOG(1, "prPes NULL\n");

        return MTR_NOT_OK;
    }
    
    if (!_rMtPvrMpgInfo.fgGotVideo)
    {
        if (_rMtPvrMpgInfo.u4FirstPTS == (UINT32)0xFFFFFFFF)
        {
            if (prPes->u4Pts != 0)
            {
                _rMtPvrMpgInfo.fgGotVideo = TRUE;
                _rMtPvrMpgInfo.u4FirstPTS  = (prPes->u4Pts);
                
                if (_rMtPvrMpgInfo.fgEnableAudio)
                {
                   LOG(1, "Audio already started, no need ctrl STC.\n"); 
                }
                else
                {
                    STC_StopStc(_rPVRStatus.u1StcId);
                    if (_rMtPvrMpgInfo.fgAudioMaster)
                    {
                        STC_SetStcValue(_rPVRStatus.u1StcId, (UINT32)(_rMtPvrMpgInfo.u4FirstPTS - PTS_DRIFT * 6));
                        LOG(1, "Audio master, Video set STC(0x%X)\n",(UINT32)(_rMtPvrMpgInfo.u4FirstPTS-PTS_DRIFT*6));
                    }
                    else
                    {
                        STC_SetStcValue(_rPVRStatus.u1StcId, (UINT32)(_rMtPvrMpgInfo.u4FirstPTS - PTS_DRIFT));
                        LOG(1, "Sys master, Video set STC(0x%X)\n",(UINT32)(_rMtPvrMpgInfo.u4FirstPTS-PTS_DRIFT));
                    }
                    STC_StartStc(_rPVRStatus.u1StcId);
                }
            }
            else
            {
            #if 0
                _rMtPvrMpgInfo.u4FirstVideoPTS = u4LastVideoPTS;
                LOG(5,"set FirstVideoPTS=u4LastPTS=0x%x STC(0x%x)\n",
                        u4LastVideoPTS, STC_GetStcValue(_rPVRStatus.u1StcId));                
            #endif
            }
        }
    }
    
    if (0 != prPes->u4Pts)
    {
        _rMtPvrMpgInfo.u4LastVideoPTS = prPes->u4Pts;
    }

#if 0
    if ((prPes->fgPtsDts) && (prPes->u4Pts > 0))
    {      
        if (_rMtPvrMpgInfo.u4FirstPTS == 0)
        {
            _rMtPvrMpgInfo.u4FirstPTS = (prPes->u4Pts);
        }
        else
        {
            if ((prPes->u4Pts < _rMtPvrMpgInfo.u4FirstPTS) &&
                ((_rMtPvrMpgInfo.u4FirstPTS - prPes->u4Pts) < (8*90000)))
            {
                _rMtPvrMpgInfo.u4FirstPTS = prPes->u4Pts;
            }
        }
        
#if 0
        if (_rMtPvrMpgInfo.fgSetStc)
        {
            _rMtPvrMpgInfo.fgSetStc = TRUE;
        }
#endif
    }
#endif

    return MTR_OK;
}

/*
    handle video callback 
*/
BOOL _MTPVR_VideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rPesInfo;
    UINT32 u4Addr = 0;
    UINT32 u4SwdmxPicType = SWDMX_PIC_TYPE_OTHER;
   
   if (prPes == NULL)
   {
       LOG(1, "prPes NULL\n");
       return FALSE;
   }  
    x_memset(&rPesInfo, 0, sizeof(VDEC_PES_INFO_T));

    if (!_rPVRStatus.fgH264ESMode)
    {
        _MTPVR_MonitorVideoPTS(PVR0, prPes);
    }
    
    rPesInfo.u4PTS = prPes->u4Pts;
    rPesInfo.u4DTS = prPes->u4Dts;
    rPesInfo.rTime = prPes->rTimeHwS;

#if 0
    if (rPesInfo.u4PTS > 0)
    {
        if (_rMtPvrMpgInfo.u4FirstPTS != (UINT32)0xFFFFFFFF)
        {
            if ((prPes->u4Pts < _rMtPvrMpgInfo.u4FirstPTS ) &&
                ((_rMtPvrMpgInfo.u4FirstPTS  - prPes->u4Pts) > (10000))) // 1 sec
            {
                _rMtPvrMpgInfo.u4FirstPTS  = prPes->u4Pts;
            }
        }
    }
#endif 

    // filling data
    rPesInfo.ucMpvId = prPes->u1DeviceId;
    rPesInfo.ucEsId = prPes->u1Channel;
    rPesInfo.u4FifoStart = prPes->u4BufStart;
    rPesInfo.u4FifoEnd = prPes->u4BufEnd;
    rPesInfo.u4VldReadPtr = prPes->u4FrameAddr;
    rPesInfo.fgDtsValid = prPes->fgPtsDts;
    rPesInfo.fgSeqHdr = prPes->fgSeqHeader;
    rPesInfo.fgGop = prPes->fgGop;
    rPesInfo.fgEos = prPes->fgEOS;
    rPesInfo.u4DmxFrameType = prPes->u4FrameType;

    // determine pic type
    if (prPes->eVideoType == DMX_VIDEO_MPEG)
    {
        switch (prPes->u4FrameType)
        {       
        case PIC_TYPE_I:
           rPesInfo.ucPicType = MPV_I_TYPE;
           u4SwdmxPicType = SWDMX_PIC_TYPE_I;
           break;
       
       case PIC_TYPE_P:
           rPesInfo.ucPicType = MPV_P_TYPE;
           break;
       
       case PIC_TYPE_B:
           rPesInfo.ucPicType = MPV_B_TYPE;
           break;
        }
    }
    else if (prPes->eVideoType == DMX_VIDEO_H264)
    {              
        rPesInfo.ucPicType = prPes->au1PicInfo[0];
        u4Addr = prPes->u4FrameAddr + 4;
        if (u4Addr >= prPes->u4BufEnd)
        {
            u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
        }
        rPesInfo.u4VldReadPtr = u4Addr;
        
        if (((rPesInfo.ucPicType & 0xF) == 0x5) ||
            ((rPesInfo.ucPicType & 0xF) == 0x7)) // IDR_NALU
        {
            u4SwdmxPicType = SWDMX_PIC_TYPE_I;
        }
        else // TBD
        {
            if (!_rMtPvrMpgInfo.fgGotVideo)
            {
                LOG(6, "%s(L:%d) no idr\n", __FUNCTION__, __LINE__);
            }            
        }
        rPesInfo.u1SliceCount = prPes->u1SliceCount;
        
    }
    else if (prPes->eVideoType == DMX_VIDEO_AVS)
    {
        rPesInfo.ucPicType = prPes->au1PicInfo[3];
        
        if (rPesInfo.ucPicType == 0xB3)
        {
            u4SwdmxPicType = SWDMX_PIC_TYPE_I;
        }
        else if (rPesInfo.ucPicType == 0xB0)
        {
            rPesInfo.fgSeqHdr = TRUE;
        }
    }
    
    if (_rMtPvrMpgInfo.fgAudioMaster)
    {
        if (_rPVRStatus.ePVRAudioStatus == ePVR_CHECK_STREAM)
        {
            _rPVRStatus.ePVRAudioStatus = ePVR_NO_STREAM;
            LOG(1, "No audio stream\n");                    // The AV sync mode, need change
        }
        
        // this is first frame we can display
        if (u4SwdmxPicType == SWDMX_PIC_TYPE_I)
        {
            if (!_rMtPvrMpgInfo.fgGotVideoReady)
            {
                _rMtPvrMpgInfo.fgGotVideoReady = TRUE;
                STC_StopStc(_rPVRStatus.u1StcId);
            }
        }
        
        if (_rMtPvrMpgInfo.fgGotVideoReady)
        {
            if (_rMtPvrMpgInfo.u4FirstVideoPTS == 0xFFFFFFFF)
            {
                if (rPesInfo.u4PTS)
                {
                    _rMtPvrMpgInfo.u4FirstVideoPTS = rPesInfo.u4PTS;                    
                }
                else if (_rMtPvrMpgInfo.u4LastVideoPTS)
                {
                    _rMtPvrMpgInfo.u4FirstVideoPTS = _rMtPvrMpgInfo.u4LastVideoPTS;
                }
                
                if (_rMtPvrMpgInfo.u4FirstVideoPTS != 0xFFFFFFFF)
                {
                    if (!_rMtPvrMpgInfo.fgEnableAudio)
                    {
                        STC_SetStcValue(_rPVRStatus.u1StcId, _rMtPvrMpgInfo.u4FirstVideoPTS - PTS_DRIFT);
                        
                        LOG(2, "First video, audio still not output. SetSTC(0x%X)\n",
                                _rMtPvrMpgInfo.u4FirstVideoPTS - PTS_DRIFT);
                    }

                    // Sync Video Sync mode. For prevous Setting has no effect for H264 codec (VDEC cfg later than mtpvr)
                    VDEC_SyncStc(_rPVRStatus.ucEsId, VID_SYNC_MODE_SLAVE_MM, _rPVRStatus.u1StcId);
                    STC_StartStc(_rPVRStatus.u1StcId);
                    
                    LOG(1, "First video PTS 0x%x 0x%X\n", _rMtPvrMpgInfo.u4FirstVideoPTS, STC_GetStcValue(_rPVRStatus.u1StcId));
                }
            }
        }
    }

    // need or not?
    // rPesInfo.u4Timestap = _rMtPvrMpgInfo.u4FirstPTS;

    // TODO: send PES to squeeze
#if 1
    if (_rPVRStatus.fgH264ESMode &&  prPes->fgEOS)    
    {
        VDEC_PES_INFO_T rPesInfo1;
        x_memcpy((VOID*)&rPesInfo1, (VOID*)&rPesInfo, sizeof(VDEC_PES_INFO_T));
        rPesInfo1.ucPicType = I_TYPE;
        rPesInfo1.fgSeqEnd = TRUE;
        VDEC_SendEs((void*)&rPesInfo1);
        
        return TRUE;
    } 
#endif
    
    if (rPesInfo.u4PTS && (_rMtPvrMpgInfo.u4LastVideoPTS != rPesInfo.u4PTS))
    {
        _rMtPvrMpgInfo.u4LastVideoPTS = rPesInfo.u4PTS;
    }
    
    //if (!prPes->fgEOS)
    {
        UNUSED(VDEC_SendEs((void*)&rPesInfo));
    }  

    if (prPes->fgEOS)
    {
        LOG(0, "MTPVR Send Video EOS.\n");
    }
    
    return TRUE;        
}

/*
    take care of audio pts
*/
MT_RESULT_T _MTPVR_MonitorAudioPTS(UINT8 ucPvrId, const DMX_AUDIO_PES_T* prPes)
{    
    if (prPes == NULL)
    {
        LOG(1, "prPes NULL\n");
        return MTR_NOT_OK;
    }

    UNUSED(ucPvrId);
   
    if (!_rMtPvrMpgInfo.fgGotAudio)
    {
        LOG(5, "===== get audio data ===\n");
        _rMtPvrMpgInfo.fgGotAudio = TRUE;
        
        // vdec playing
        if (_MTVDEC_QueryVdecStatus(_rPVRStatus.ucEsId))
        {
            return MTR_OK;
        }
        
        if (_rMtPvrMpgInfo.u4FirstPTS == 0xFFFFFFFF)
        {  
            if (prPes->u4Pts != 0)
            {   
                _rMtPvrMpgInfo.u4FirstPTS  = (prPes->u4Pts);
                STC_StopStc(_rPVRStatus.u1StcId);
                STC_SetStcValue(_rPVRStatus.u1StcId, (UINT32)(_rMtPvrMpgInfo.u4FirstPTS - PTS_DRIFT));
                STC_StartStc(_rPVRStatus.u1StcId);
                LOG(1, "=====AUDIO SET STC to 0x%x ===\n",(UINT32)(_rMtPvrMpgInfo.u4FirstPTS - PTS_DRIFT));
            }
            else
            {
                #if 0
                _rMtPvrMpgInfo.u4FirstVideoPTS = u4LastVideoPTS;
                LOG(5,"set FirstVideoPTS=u4LastPTS=0x%x STC(0x%x)\n",
                        u4LastVideoPTS, STC_GetStcValue(_rPVRStatus.u1StcId));                
                #endif
            }
        }
    }
    
#if 0
    if (prPes->u4Pts > 0)
    {
        if (_rMtPvrMpgInfo.u4FirstPTS == 0xFFFFFFFF)
        {
            _rMtPvrMpgInfo.u4FirstPTS = prPes->u4Pts;
            _rMtPvrMpgInfo.fgGotAudio = TRUE;
        }
        else
        {
            #if 0
            if ((u4PTS < _rMtPvrMpgInfo.u4FirstPTS) &&
                ((_rMtPvrMpgInfo.u4FirstPTS - prPes->u4Pts) < (8*90000)))
            {
                _rMtPvrMpgInfo.u4FirstPTS = prPes->u4Pts;
            }
            #endif
        }
        
        if (!_rMtPvrMpgInfo.fgGetFirstAudPts)
        {
            _rMtPvrMpgInfo.fgGetFirstAudPts = TRUE;
            _rMtPvrMpgInfo.u4FirstAudioPTS = prPes->u4Pts;
            #if 0
            _rMtPvrMpgInfo.fgGotAudio = TRUE;
            #endif

            LOG(1, "GetFirstAudioPTS:0x%x\n",
                _rMtPvrMpgInfo.u4FirstAudioPTS);
        }
    }
    else
    {
        LOG(5,"[Play 188] Aduio Pts == 0 \n");
    }
#endif

    return MTR_OK;
}


/*
    handle audio callback
*/
BOOL _MTPVR_AudioCallback(const DMX_AUDIO_PES_T* prPes)
{
    DMX_AUDIO_PES_T rPesInfo;

    if (prPes == NULL)
    {
        LOG(1, "prPes NULL\n");
        return FALSE;
    }
    
    x_memset(&rPesInfo, 0, sizeof(DMX_AUDIO_PES_T));
    x_memcpy(&rPesInfo, prPes, sizeof(DMX_AUDIO_PES_T));

    _rMtPvrMpgInfo.u4LastAudioWptr = prPes->u4Wp;

    if (_rMtPvrMpgInfo.fgAudioMaster)
    {
        if (!_rMtPvrMpgInfo.fgGotAudio)
        {
            if (_rMtPvrMpgInfo.fgGotVideoReady && 
                (prPes->u4Pts >_rMtPvrMpgInfo.u4FirstVideoPTS))
            {
#if 1
                if(prPes->u4Wp > prPes->u4PesRp + 16)
                {
                    LOG(1, "first valid audio 1 pts 0x%x\n", prPes->u4Pts);
                    _rMtPvrMpgInfo.u4FirstAudioPTS = prPes->u4Pts;
                    AUD_SetStartPts(_rPVRStatus.u1AudId, prPes->u4Pts);
                }
                else
#endif
                {
                    LOG(1, "no clean wp 0x%x lwp 0x%x, pts 0x%x\n", rPesInfo.u4Wp, rPesInfo.u4PesRp, rPesInfo.u4Pts);
                }
            }
            else if (_rPVRStatus.ePVRVideoStatus == ePVR_CHECK_STREAM)
            {
                LOG(1, "no video stream\n");
                _rMtPvrMpgInfo.u4FirstAudioPTS = prPes->u4Pts;
                _rPVRStatus.ePVRVideoStatus = ePVR_NO_STREAM;
                AUD_SetStartPts(_rPVRStatus.u1AudId, prPes->u4Pts);
            }
            else if (_rPVRStatus.ePVRVideoStatus != ePVR_NO_STREAM)
            {
                _rMtPvrMpgInfo.fgCleanAudio = TRUE;
            }
            
            // if(_rMtPvrMpgInfo.fgCleanAudio) return TRUE;
        }
        else if ((_rMtPvrMpgInfo.u4FirstAudioPTS == 0xFFFFFFFF) && (_rMtPvrMpgInfo.u4LastAudioPTS != rPesInfo.u4Pts))
        {
            LOG(1, "first valid audio 2 pts 0x%x\n", prPes->u4Pts);
            _rMtPvrMpgInfo.u4FirstAudioPTS = prPes->u4Pts;
            AUD_SetStartPts(_rPVRStatus.u1AudId, prPes->u4Pts);
        }

        if (_rMtPvrMpgInfo.fgCleanAudio && !_rMtPvrMpgInfo.fgGotAudio &&
            _rPVRStatus.ePVRVideoStatus != ePVR_NO_STREAM)
        {
            if (0xFFFFFFFF == _rMtPvrMpgInfo.u4FirstAudioPTS)
            {
                _rMtPvrMpgInfo.u4FirstAudioPTS = prPes->u4Pts;
                LOG(5, "Need clean audio (First) 0x%x\n", prPes->u4Pts);
                return TRUE;
            }
            else if ((0==prPes->u4Pts) || (prPes->u4Pts<=_rMtPvrMpgInfo.u4FirstAudioPTS) ||
                     ((prPes->u4Pts>_rMtPvrMpgInfo.u4FirstAudioPTS) &&
                      (90000>(prPes->u4Pts-_rMtPvrMpgInfo.u4FirstAudioPTS))))
            {
                LOG(5, "Need clean audio (No video) 0x%x\n", prPes->u4Pts);
                return TRUE;
            }

            if (prPes->u4Wp > prPes->u4PesRp + 16)
            {
                LOG(1, "First valid audio 3 pts 0x%x\n", prPes->u4Pts);
                _rMtPvrMpgInfo.u4FirstAudioPTS = prPes->u4Pts;
                AUD_SetStartPts(_rPVRStatus.u1AudId, prPes->u4Pts);
            }
            else
            {
                LOG(1, "No clean wp 0x%x lwp 0x%x, pts 0x%x\n", rPesInfo.u4Wp, rPesInfo.u4PesRp, rPesInfo.u4Pts);
            }
        }
        
        if ((!_rMtPvrMpgInfo.fgEnableAudio) && 
            (_rPVRStatus.ePVRVideoStatus == ePVR_NO_STREAM) && 
            (prPes->u4Pts < _rMtPvrMpgInfo.u4FirstAudioPTS + 90000))
        {
            LOG(1, "not send 0x%x, 0x%x\n", prPes->u4Pts, _rMtPvrMpgInfo.u4FirstAudioPTS);
            return TRUE;
        }
    }

    // get first pts to adec
    _MTPVR_MonitorAudioPTS(PVR0, prPes);

    rPesInfo.fgForward = FALSE;
    if ((ePVR_NO_STREAM != _rPVRStatus.ePVRVideoStatus) ||
        (rPesInfo.u4Pts && (_rMtPvrMpgInfo.u4LastAudioPTS != rPesInfo.u4Pts)))
    {
        LOG(6, "wp 0x%x lwp 0x%x, pts 0x%x\n", rPesInfo.u4Wp, rPesInfo.u4PesRp, rPesInfo.u4Pts);
        _rMtPvrMpgInfo.u4LastAudioPTS = rPesInfo.u4Pts;
    }

    if ((TRUE==rPesInfo.fgEOS) && 
        ((0==rPesInfo.u4Wp) || (0==rPesInfo.u4PesRp)))
    {
        UINT32 u4BufStart, u4BufEnd;
        
        VERIFY(AUD_GetAudFifo(_rPVRStatus.u1AudId, &u4BufStart, &u4BufEnd) == AUD_OK);
        rPesInfo.u4Wp = u4BufStart;
        rPesInfo.u4PesRp = u4BufStart;
    }

    if (rPesInfo.fgEOS)
    {
        LOG(5, "MTPVR Send audio EOS..\n");
    }
    
    AUD_SendAudioPes(&rPesInfo);

    return TRUE;
}

static VOID _MTPVR_TrigAudOutCb(void *pvTag)
{
    UCHAR ucPvrId = (UCHAR)((UINT32)pvTag);
    UNUSED(ucPvrId);
    
    _rMtPvrMpgInfo.fgTriggerAudio = FALSE;
    _rMtPvrMpgInfo.fgEnableAudio = TRUE;

    LOG(5, "%s:%d Aout(0&1) Enable by B2r\n", __FUNCTION__, __LINE__);
    AUD_MMAoutEnable(_rPVRStatus.u1AudId, TRUE);
    AUD_MMAoutEnable(0, TRUE);
    
    return ;
}

/*
    handle audio ready callback
*/
BOOL _MTPVR_AudioReadyCb(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{  
    if((eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE) ||
       (eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_NO_DATA))
    {
       if ((eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE))
       {
           LOG(1, "=================  Audio is ready  ======================\n");
       }
       else if (eAudioNotifyType == ENUM_SWDMX_AUDIO_NOTIFY_DECODE_DONE)
       {
           LOG(1, "=================  Audio is no data  ======================\n");
       }
       
       if (_rMtPvrMpgInfo.fgGotVideoReady)
       {
           if (!_rMtPvrMpgInfo.fgGotAudioReady)
           {
                #if 0
                // TODO, use correct B2R ID.
                VDP_TriggerAudReceive(0, _rMtPvrMpgInfo.u4FirstAudioPTS);
                #else
                VDP_TRIG_AUD_ARRIVE_CB_T rCbInfo;
                rCbInfo.pfnTrigAud = _MTPVR_TrigAudOutCb;
                rCbInfo.pvTag = (void *)((UINT32)u1SrcId);
                // In fact, _MTPVR_AudioReadyCb not used now
                LOG(1, "Triger audio pts 0x%x\n", _rMtPvrMpgInfo.u4FirstAudioPTS);
               // UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
               //UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
               #if defined(CC_MT5890)||defined(CC_MT5882)
                UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS));
               #else
               UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
               #endif
                #endif
           }
       }
       #if 0
       else
       {
           LOG(1, "%s(L:%d) error\n", __FUNCTION__, __LINE__);
       }
       #endif
       
       _rMtPvrMpgInfo.fgGotAudioReady = TRUE;
       
    }

    return TRUE;
}

BOOL _MTPVR_GetAudioFifo(UINT32 *pu4Addr, UINT32 *pu4Size)
{
    UINT32 au4BufStart[2], au4BufEnd[2];

    if ((!pu4Addr) || (!pu4Size))
    {
        LOG(1, "_SWDMX_GetVideoBuffer pu4Addr or pu4Size null\n");
        return FALSE;
    }

    // is here MAIN forever ?
    if (AUD_GetAudFifo(0 , &au4BufStart[0], &au4BufEnd[0]) != AUD_OK)
    {
        LOG(1, "_SWDMX_GetAudioFifo Can't get audio buffers!\n");
        return FALSE;
    }

    *pu4Addr          = au4BufStart[0];
    *pu4Size          = au4BufEnd[0] - au4BufStart[0];
    
    return TRUE;
}

MT_RESULT_T _MTPVR_CheckStartUp(UCHAR ucPvrId)
{
    BOOL fgVReady = FALSE;
    BOOL fgAReady = FALSE;
    UINT32 u4DmxAvailSizeAudio = 0;
    UINT16 u2QueueSize = 0;
    UINT16 u2MaxQueueSize = 0;
    UINT32 u4Addr, u4Size;
    UNUSED(ucPvrId);

    // already play
    if (!_rMtPvrMpgInfo.fgVideoPause)
    {
        //LOG(3, "%s(L:%d) error\n", __FUNCTION__, __LINE__);
        return MTR_OK;
    }
    
    u4DmxAvailSizeAudio = DMX_GetMinFreeBufferSizeEx(2, DMX_PID_TYPE_ES_AUDIO);
    if (!_MTPVR_GetAudioFifo(&u4Addr, &u4Size))
    {
        LOG(3, "_SWDMX_GetAudioFifo Fail\n");
        return FALSE;
    }
    
    if ((u4Size - u4DmxAvailSizeAudio) > (AUD_START_COND))
    {
        fgAReady = TRUE;   
    }
    else
    {
       // fgAReady = TRUE;  
        LOG(6, "%s(L:%d) audio fifo size(%d)\n", __FUNCTION__, __LINE__, (u4Size - u4DmxAvailSizeAudio));
    }
    
    VDEC_GetQueueInfo(_rPVRStatus.ucEsId, &u2QueueSize, &u2MaxQueueSize);
                  
    if((u2QueueSize > 0) && (u2MaxQueueSize > 0) &&
       (u2QueueSize >= VID_START_COND))
    {
        fgVReady = TRUE;
    }
    else
    {
        //fgVReady = TRUE;
        LOG(6, "%s(L:%d) video queue size(%d)\n", __FUNCTION__, __LINE__, u2QueueSize);
    }

    if (_rMtPvrMpgInfo.fgVideoPause && fgVReady && fgAReady && _rMtPvrMpgInfo.fgGotAudioReady)
    {
        // play
        VDP_SetPauseMm(B2R_1, FALSE);
        _rMtPvrMpgInfo.fgVideoPause = FALSE;  
        LOG(1, "fgVideoPause off\n"); 
    }
    else
    {
        LOG(1,"StartUp condition is not meet fgVReady %d, fgAReady %d, fgGotAudioReady %d, fgVideoPause %d\n" ,
            fgVReady,fgAReady,_rMtPvrMpgInfo.fgGotAudioReady,_rMtPvrMpgInfo.fgVideoPause);
    }

    return MTR_OK;
}

#if 0
BOOL _MTPVR_PauseAV(UCHAR ucPvrId)
{
#if 0
    VDEC_Pause(ES0);
    AUD_DSPCmdPause(1);
#endif
    STC_StopStc(STC_SRC_A1);
}

BOOL _MTPVR_ResumeAV(UCHAR ucPvrId)
{
#if 0
    VDEC_Play(ES0, VDEC_FMT_H264);
    AUD_DSPCmdResume(1);
#endif
    STC_StartStc(STC_SRC_A1);    
}
#endif

VOID _MTPVR_HandleUnderFlow(UCHAR ucPvrId, BOOL fgFull, UINT16 u2QueueSize, UINT32 u4DmxAvailSizeAudio)
{
    UINT32 u4VPTS = 0, u4APTS = 0, u4STC = 0;
    
    if (!MPV_IsDisplay(ES0)) return;
    
    
    u4STC = STC_GetStcValue(_rPVRStatus.u1StcId);
    u4VPTS = _rMtPvrMpgInfo.u4LastVideoPTS;
    u4APTS = _rMtPvrMpgInfo.u4LastAudioPTS;
    
    if ((u4VPTS + 20 * 90000 < u4APTS) || (u4APTS + 20 * 90000 < u4VPTS) )
    {
        LOG(2, "vpts = 0x%x, apts = 0x%x, stc = 0x%x\n", u4VPTS, u4APTS, u4STC);
        return;
    }
    
#if 0
    if (u4STC > 15 * 90000)
    {
        LOG(1, "set stc to 900 from 0x%x\n", STC_GetStcValue(STC_SRC_A1));
        STC_SetStcValue(0, 900);        
    } 
    
    if ((_rPVRStatus.eUnderFlowStatus == ePVR_BUFFER_MAX) && u4VPTS && u4APTS && (!fgFull) && (((u4VPTS < u4APTS) ? u4VPTS : u4APTS) < u4STC))
    {
        LOG(1, "max->underflow %d %d, 0x%x 0x%x 0x%x, %d, 0x%x\n", _rPVRStatus.eUnderFlowStatus, fgFull, u4VPTS, u4APTS, u4STC, u2QueueSize, u4DmxAvailSizeAudio);
        _rPVRStatus.eUnderFlowStatus = ePVR_BUFFER_UNDERFLOW;
        _MTPVR_PauseAV(ucPvrId);
    }    
    else if ((_rPVRStatus.eUnderFlowStatus == ePVR_BUFFER_READY) && u4VPTS && u4APTS && (!fgFull) && (((u4VPTS < u4APTS) ? u4VPTS : u4APTS) < u4STC + 45000))
    {
        LOG(1, "ready->underflow %d, 0x%x 0x%x 0x%x, %d, 0x%x\n", fgFull, u4VPTS, u4APTS, u4STC, u2QueueSize, u4DmxAvailSizeAudio);
        _rPVRStatus.eUnderFlowStatus = ePVR_BUFFER_UNDERFLOW;
        _MTPVR_PauseAV(ucPvrId);
    }
    else if ((_rPVRStatus.eUnderFlowStatus == ePVR_BUFFER_UNDERFLOW) && (fgFull || _fgEOS || (((u4VPTS < u4APTS) ? u4VPTS : u4APTS) > u4STC + 90000 * 3)))
    {
        LOG(1, "underflow->ready %d, 0x%x 0x%x 0x%x, %d, 0x%x\n", fgFull, u4VPTS, u4APTS, u4STC, u2QueueSize, u4DmxAvailSizeAudio);
        _rPVRStatus.eUnderFlowStatus = ePVR_BUFFER_READY;
        _MTPVR_ResumeAV(ucPvrId);
    }
#endif

    return ;
}

VOID _MTPVR_PlayAV(UCHAR ucPvrId, BOOL fgFull, BOOL fgAudioFull, UINT32 u4AudioState)
{
    if (ucPvrId > 1)
    {
        return;
    }
    
#if 0
    if(_rMtPvrMpgInfo.fgVideoPause && !_rMtPvrMpgInfo.fgEnableVideo)
    {
        LOG(1, "%s enable video\n");
        VDEC_Play(ES0, VDEC_FMT_H264);
        _rMtPvrMpgInfo.fgEnableVideo = TRUE;
        _rMtPvrMpgInfo.fgVideoPause = FALSE;
    }

    if(_rMtPvrMpgInfo.u4LastAudioPTS && !_rMtPvrMpgInfo.fgEnableAudio)
    {
        LOG(1, "%s enable audio\n", __FUNCTION__);
        AUD_MMAoutEnable(AUD_DEC_MAIN, TRUE);
        AUD_MMAoutEnable(_rPVRStatus.u1StcId, TRUE);
        if(_fgAuxAudioReady)
        {
            _rMtPvrMpgInfo.fgEnableAudio = TRUE;
            LOG(1, "aux audio is ready\n");
        }      
    }
#else

    if (u4AudioState== 0x49200)
    {
        if ((_rPVRStatus.ePVRVideoStatus == ePVR_NO_STREAM) || fgAudioFull)
        {
            if (!_rMtPvrMpgInfo.fgGotVideoReady)
            {
                STC_SetStcValue(_rPVRStatus.u1StcId, (UINT32)(_rMtPvrMpgInfo.u4FirstAudioPTS - PTS_DRIFT));
                STC_StartStc(_rPVRStatus.u1StcId);
            }
            
            LOG(1, "Audio output, %d %d 0x%x 0x%x\n", fgFull, _rMtPvrMpgInfo.fgEnableAudio, 
                    _rMtPvrMpgInfo.u4FirstAudioPTS, _rMtPvrMpgInfo.u4LastAudioPTS);
                    
            AUD_MMAoutEnable(_rPVRStatus.u1AudId, TRUE);
            AUD_MMAoutEnable(0, TRUE);
        }
        else if((_rMtPvrMpgInfo.u4FirstAudioPTS != 0xFFFFFFFF) && (!_rMtPvrMpgInfo.fgTriggerAudio))
        {
            VDP_TRIG_AUD_ARRIVE_CB_T rCbInfo;
            rCbInfo.pfnTrigAud = _MTPVR_TrigAudOutCb;
            rCbInfo.pvTag = (void *)((UINT32)ucPvrId);
            
            LOG(1, "Triger audio pts 0x%x\n", _rMtPvrMpgInfo.u4FirstAudioPTS);
            //UNUSED(VDP_TriggerAudReceiveEx(ucPvrId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
             #if defined(CC_MT5890) || defined(CC_MT5882)
                UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS));
             #else
               UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
             #endif
               /// UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS));
            _rMtPvrMpgInfo.fgTriggerAudio = TRUE;
        }
    }
    else if((u4AudioState == 0x9200) && (!_rMtPvrMpgInfo.fgEnableAudio))
    {
        LOG(1, "Audio has output\n");
        
        if (ePVR_HAS_STREAM==_rPVRStatus.ePVRVideoStatus)
        {
            // LWB: cancel Trig action
           // UNUSED(VDP_TriggerAudReceiveEx(ucPvrId, FALSE, _rMtPvrMpgInfo.u4FirstAudioPTS, NULL));
            //UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
               #if defined(CC_MT5890) || defined(CC_MT5882)
                UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS));
               #else
               UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, NULL));
               #endif
            // LWB: This avoid VDEC to update STC
            UNUSED(VDEC_SetRestartAudioPts(_rPVRStatus.ucEsId, FALSE));

            // LWB: Let VDP pending for audio
            UNUSED(VDP_SetPendingForStc(ucPvrId, TRUE));
            LOG(1, "No video data, audio out. Reset VDEC/VDP related info. \n");
        }
        
        _rMtPvrMpgInfo.fgTriggerAudio = FALSE;
        _rMtPvrMpgInfo.fgEnableAudio = TRUE;
    }   
    
    if (!_rMtPvrMpgInfo.fgEnableVideo && _rMtPvrMpgInfo.u4LastVideoPTS)
    {
        _rMtPvrMpgInfo.fgEnableVideo = TRUE;
    }
#endif

    UNUSED(ucPvrId);
}

BOOL _MTPVR_IsFifoFull(UCHAR ucPvrId)
{
    UINT32 u4DmxAvailSizeVideo = 0;
    UINT32 u4DmxAvailSizeAudio = 0;
    UINT16 u2DmxPictureQSize = 0;
    UINT16 u2DmxMaxQSize = 0;
    UINT16 u2QueueSize = 0;
    UINT16 u2MaxQueueSize = 0;
    BOOL fgFull = FALSE;
    BOOL fgAudioFull = FALSE;
    UINT32 u4AudioState;

    do
    {
        // audio free buffer
        if (ePVR_HAS_STREAM == _rPVRStatus.ePVRAudioStatus)
        {
            u4DmxAvailSizeAudio = DMX_GetMinFreeBufferSizeEx(_rPVRStatus.ucTsIdx, DMX_PID_TYPE_ES_AUDIO);
            if ((u4DmxAvailSizeAudio < _u4OutputSize) &&
                (u4DmxAvailSizeAudio != 0))
            {
                x_thread_delay(5);
                fgAudioFull = TRUE;
                fgFull = TRUE;
                break;
            }
        }

        if (ePVR_HAS_STREAM == _rPVRStatus.ePVRVideoStatus)
        {
            // video free buffer.
            u4DmxAvailSizeVideo = DMX_GetMinFreeBufferSizeEx(_rPVRStatus.ucTsIdx, DMX_PID_TYPE_ES_VIDEO);
            if((u4DmxAvailSizeVideo < _u4OutputSize) &&
                    (u4DmxAvailSizeVideo != 0))
            {
                x_thread_delay(5);
                fgFull = TRUE;
                break;
            }
            
            // video esmq num
            VDEC_GetQueueInfo(_rPVRStatus.ucEsId, &u2QueueSize, &u2MaxQueueSize);
            if ((u2QueueSize > 0) && (u2MaxQueueSize > 0) &&
                (u2QueueSize >(u2MaxQueueSize - RESERVED_ESMQ_THRESHOLD)))
            {
            x_thread_delay(5);
            fgFull = TRUE;
            break;
            }

            // dmx mesq num
            DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
            if ((u2DmxPictureQSize >0) && (u2DmxMaxQSize > 0) &&
                (u2DmxPictureQSize > (u2DmxMaxQSize - RESERVED_PICQ_THRESHOLD)))
            {
                    x_thread_delay(5);
                fgFull = TRUE;
                break;
            }
        }

        if ((ePVR_HAS_STREAM==_rPVRStatus.ePVRVideoStatus) && (FALSE==_rMtPvrMpgInfo.fgGotVideoReady))
        {
            UINT32 au4BufStart[2]={0}, au4BufEnd[2]={0};
            UINT32 u4BufSize=0, u4DataSize=0;

#ifdef AUD_OLDFIFO_INTF
            if (AUD_OK != AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0], &au4BufStart[1], &au4BufEnd[1]))
#else
            if (AUD_OK != AUD_GetAudFifo(_rPVRStatus.u1AudId, &au4BufStart[0], &au4BufEnd[0]))
#endif
            {
                LOG(1, "Can't get audio buffers!\n");
                return FALSE;
            }

            u4BufSize = au4BufEnd[0] - au4BufStart[0];
            u4DataSize = u4BufSize - u4DmxAvailSizeAudio;

            if ((u4DataSize && u4BufSize) && ((32*1024<=u4DataSize) ||
                (_rMtPvrMpgInfo.u4LastAudioPTS && (0xFFFFFFFF!=_rMtPvrMpgInfo.u4LastAudioPTS) &&
                 _rMtPvrMpgInfo.u4FirstAudioPTS && (0xFFFFFFFF!=_rMtPvrMpgInfo.u4FirstAudioPTS) &&
                (4*90000<_rMtPvrMpgInfo.u4LastAudioPTS-_rMtPvrMpgInfo.u4FirstAudioPTS))))
            {
                LOG(5, "AFIFO full due to no video. Size(0x%X)\n", u4DataSize);         
                    x_thread_delay(5);
                fgAudioFull = TRUE;

                break;
            }
        }
    } while(0);

    if (_rMtPvrMpgInfo.fgAudioMaster && (!_rMtPvrMpgInfo.fgEnableVideo || !_rMtPvrMpgInfo.fgEnableAudio))
    {
        u4AudioState = AUD_AudioDecoderState(_rPVRStatus.u1AudId);

        if (fgFull || (u4AudioState == 0x49200) || (u4AudioState == 0x9200))
        {
            LOG(5, "fgFull %d, u4AudioState 0x%x\n", fgFull, u4AudioState);
            _MTPVR_PlayAV(ucPvrId, fgFull, fgAudioFull, u4AudioState);
        }
    }

    return fgFull;
}


static INT32 _DumpData(CHAR *pcFileName, const INT8 *buf, size_t count )
{
    mm_segment_t old_fs;
    struct file *filep;
    size_t ret;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    filep = filp_open(pcFileName, (O_CREAT | O_RDWR | O_APPEND), 0);
    if ( IS_ERR(filep) || NULL == filep)
    {
        set_fs( old_fs );
        LOG(0, "_DumpData, unable to open file: %s\n", pcFileName);

        return -1;
    }

    ret = filep->f_op->write(filep, buf, count, &filep->f_pos);
    
    filp_close( filep, 0 );
    set_fs( old_fs );
    
    return 0;
}


MT_RESULT_T _MTPVR_SendData(UCHAR ucPvrId)
{
    MT_RESULT_T eRet = MTR_OK;
    SWDMX_PVR_SENDDATA_RET_T ePVRRet;
    DMX_MM_DATA_T rDmxMMData;
    UINT32 u4SendSize = 0;
    BOOL fgPvrEOS = FALSE;
    BOOL fgFast = FALSE;

    MTPVR_PRINT("Entry _MTPVR_SendData!)\n");
    u4SendSize = _rPVRStatus.u4QueuedSize;
    
    if (u4SendSize > _u4OutputSize)
    {
        u4SendSize = _u4OutputSize;
    }

    if (u4SendSize == 0)
    {
        if (_fgEOS)
        {
            fgPvrEOS = TRUE;
            _fgEOS = FALSE;
            x_thread_delay(10);
            LOG(2, "Send Eos\n");
        }
        else
        {
            x_thread_delay(5);
            return MTR_OK;
        }
    }

    if (_rMtPvrMpgInfo.fgAudioMaster) 
    {          
        if ((_rPVRStatus.ePVRVideoStatus == ePVR_CHECK_STREAM) ||
            (_rPVRStatus.ePVRAudioStatus == ePVR_CHECK_STREAM))
        {
            INT32 i = 0;
            DMX_PID_T rDrvPid;
            
            x_memset(&rDrvPid, 0, sizeof(rDrvPid));
            for (i=0; i<DMX_NUM_PID_INDEX; i++)
            {
                if (!DMX_GetPid(i, DMX_PID_FLAG_TS_INDEX|DMX_PID_FLAG_BUFFER|DMX_PID_FLAG_VALID, &rDrvPid))
                {
                    LOG(5, "Get Pid ePidtype fail in io_mtpvr %d\n",i);
                    return MTR_NOT_OK;
                }
                
                if ((rDrvPid.ePidType==DMX_PID_TYPE_ES_VIDEO) && (rDrvPid.fgEnable==TRUE) && 
                    (rDrvPid.u1TsIndex==_rPVRStatus.ucTsIdx))
                {
                    LOG(5, "found video stream pid %d\n", i);
                    _rPVRStatus.ePVRVideoStatus = ePVR_HAS_STREAM;
                }
                
                if ((rDrvPid.ePidType==DMX_PID_TYPE_ES_AUDIO) && (rDrvPid.fgEnable==TRUE) && 
                    (rDrvPid.u1TsIndex==_rPVRStatus.ucTsIdx))
                {
                    LOG(5, "found audio stream pid %d\n", i);
                    _rPVRStatus.ePVRAudioStatus = ePVR_HAS_STREAM;
                }
            }
        }
                    
        if ((_rPVRStatus.ePVRVideoStatus != ePVR_HAS_STREAM) && 
            (_rPVRStatus.ePVRAudioStatus != ePVR_HAS_STREAM))
        {
            LOG(1, "no video & audio\n");
            x_thread_delay(5);
            return MTR_OK;
        }
    }
                    
    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
    if (_rPVRStatus.fgFirst)
    {
        rDmxMMData.fgFrameHead = TRUE;
        LOG(5, "%s(L:%d) First Move\n", __FUNCTION__, __LINE__);
    }
    rDmxMMData.u4BufStart = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StartAddr;
    rDmxMMData.u4BufEnd = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4EndAddr;
    rDmxMMData.u4StartAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4ReadAddr;
    rDmxMMData.u4FrameSize = u4SendSize;
    rDmxMMData.fgEOS = fgPvrEOS;
    rDmxMMData.u1Idx = _rPVRStatus.ucTsIdx;

    // play 188
    if (TRUE==_rMtPvrMpgInfo.fgAudioMaster)
    {
        if (_MTPVR_IsFifoFull(ucPvrId))
        {
            x_thread_delay(5);
            return MTR_OK;
        }
    }

    if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
		(ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
    {
        if(!((_rPVRStatus.i4Speed == 25) ||
             (_rPVRStatus.i4Speed == 50) ||
             (_rPVRStatus.i4Speed == 100) ||
             (_rPVRStatus.i4Speed == 200)))
        {
            fgFast = TRUE;

        #ifdef VDEC_IS_POST_MT5368
            {
                UINT32 u4DmxAvailSizeVideo;
                UINT16 u2DmxPictureQSize;
                UINT16 u2DmxMaxQSize;            
                UINT16 u2QueueSize;
                UINT16 u2MaxQueueSize;
                
                // 2 stands for record path.
                u4DmxAvailSizeVideo = DMX_GetMinFreeBufferSizeEx(
                    2, DMX_PID_TYPE_ES_VIDEO);
                
                if((u4DmxAvailSizeVideo < _u4OutputSize) &&
                            (u4DmxAvailSizeVideo != 0))
                {
                    x_thread_delay(5);
                    return MTR_OK;
                }

            #if 0
                u4DmxAvailSizeAudio = DMX_GetMinFreeBufferSizeEx(
                                    2, DMX_PID_TYPE_ES_AUDIO);
                
                 if((u4DmxAvailSizeAudio < _u4OutputSize) &&
                            (u4DmxAvailSizeAudio != 0))
                 {
                     LOG(7,"return audio size delayed - 192 ff\n");
                     x_thread_delay(5);
                     return MTR_OK;
                 }  
             #endif
                 
                 // watch esmq size 
                 u2QueueSize = 0;
                 u2MaxQueueSize = 0;
                     VDEC_GetQueueInfo(_rPVRStatus.ucEsId, &u2QueueSize, &u2MaxQueueSize);
                 
                 if((u2QueueSize > 0) && (u2MaxQueueSize > 0) &&
                         (u2QueueSize >(u2MaxQueueSize - RESERVED_ESMQ_THRESHOLD)))
                 {
                     LOG(7,"return Q size delayed u2QueueSize %d, u2MaxQueueSize %d - 192 ff\n",
                             u2QueueSize,u2MaxQueueSize );
                     x_thread_delay(5);
                     return MTR_OK;
                 }

                 // watch pic Q size
                 u2DmxPictureQSize = 0;
                 u2DmxMaxQSize = 0;
                 DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
                    
                 if((u2DmxPictureQSize >0) && (u2DmxMaxQSize > 0) &&
                    (u2DmxPictureQSize > (u2DmxMaxQSize - RESERVED_PICQ_THRESHOLD)))
                 {
                     LOG(7,"return Dmx PicQ Size %d - 192 ff\n",u2DmxPictureQSize);
                     x_thread_delay(5);
                     return MTR_OK;
                 }
            }
        #endif
        }
            
        ePVRRet = SWDMX_PVR_SendData(ucPvrId, &rDmxMMData, _u4InputSize, fgFast);

        if(ePVRRet == SWDMX_PVR_SENDDATA_FULL)
        {
            LOG(8,"SWDMX_PVR_SEND_DATA_FULL\n");
            x_thread_delay(5);
            return MTR_OK;
        }
        else if(ePVRRet != SWDMX_PVR_SENDDATA_OK)
        {
            //printf("_SWDMX_PVR_SendData -> not ok\n");
            LOG(5, "%s(L:%d) (ePVRRet(%d) != PVR_PVR_SENDDATA_OK) error\n", __FUNCTION__, __LINE__,
                (UINT32)ePVRRet);
            MTPVR_PRINT("%s (ePVRRet(%d) != PVR_PVR_SENDDATA_OK) error\n",
                __FUNCTION__, (UINT32)ePVRRet);
            x_thread_delay(10);
            return MTR_NOT_OK;
        }
    }
    else
    {
        LOG(9, "czg u1DmxId(%d), address(%x), size(%d)\n", 
            _rPVRStatus.u1DmxId, rDmxMMData.u4StartAddr, rDmxMMData.u4FrameSize);

        if(_u4PvrDumpFlag == 1)     
        {
            static CHAR ucFileName[255] = "/usbdisk/dmx.ts";             
            //x_sprintf(ucFileName, "%s/dmx.ts", "/usbdisk");            
            
            if(rDmxMMData.u4StartAddr + rDmxMMData.u4FrameSize <= rDmxMMData.u4BufEnd)
            {
                _DumpData(ucFileName, (INT8*)VIRTUAL(rDmxMMData.u4StartAddr), (size_t)rDmxMMData.u4FrameSize);        
            }
            else
            {
                _DumpData(ucFileName, (INT8*)VIRTUAL(rDmxMMData.u4StartAddr), (size_t)rDmxMMData.u4BufEnd - rDmxMMData.u4StartAddr);                
                _DumpData(ucFileName, (INT8*)VIRTUAL(rDmxMMData.u4BufStart), (size_t)rDmxMMData.u4FrameSize - ((size_t)rDmxMMData.u4BufEnd - rDmxMMData.u4StartAddr));            
            }
        }
        if (!DMX_MUL_MoveData(_rPVRStatus.u1DmxId, &rDmxMMData))
        {
            LOG(1, "%s DMX_MUL_MoveData fail\n", __FUNCTION__);
            x_thread_delay(10);
            
            return MTR_NOT_OK;
        }
    }

    if (_rMtPvrMpgInfo.fgAudioMaster && 
        (!_rMtPvrMpgInfo.fgEnableVideo || !_rMtPvrMpgInfo.fgEnableAudio))
    {
        if (_rMtPvrMpgInfo.fgCleanAudio)
        {
            _MTPVR_ClearAudio();
            _rMtPvrMpgInfo.fgCleanAudio = FALSE;
        }
    }
        
    if(_rPVRStatus.fgFirst)
    {
        _rPVRStatus.fgFirst = FALSE;
    }
    _rPVRStatus.u4ReadAddr += u4SendSize;
    _MTPVR_Lock();
    _rPVRStatus.u4QueuedSize -= u4SendSize;
    _MTPVR_Unlock();
    if(_rPVRStatus.u4ReadAddr >= _rPVRStatus.u4EndAddr)
    {
        _rPVRStatus.u4ReadAddr -= (_rPVRStatus.u4EndAddr - _rPVRStatus.u4StartAddr);
    }
    
#if 0
    else if(_rPVRStatus.u4ReadAddr > _rPVRStatus.u4EndAddr)
    {
        MTPVR_PRINT("%s u4ReadAddr(%d) > u4EndAddr(%d)\n",
            __FUNCTION__, _rPVRStatus.u4ReadAddr, _rPVRStatus.u4EndAddr);
        ASSERT(0);
        return MTR_NOT_OK;
    }
#endif

    LOG(7, "SendData, Size(0x%06x) Addr(0x%08x) Q(0x%06x) R(0x%08x) W(0x%08x)\n",
        u4SendSize, _rPVRStatus.u4ReadAddr, _rPVRStatus.u4QueuedSize,
        _rPVRStatus.u4ReadAddr, _rPVRStatus.u4WriteAddr);
    
    return eRet;
}


MT_RESULT_T _MTPVR_SendESData(UCHAR ucPvrId)
{
    MT_RESULT_T eRet = MTR_OK;
    SWDMX_PVR_SENDDATA_RET_T ePVRRet;
    DMX_MM_DATA_T rDmxMMData;
    DMX_MM_DATA_T rDmxDummyData;
    UINT32 u4SendSize = 0;
    UINT32 u4FrameSize = 0;
    HAL_TIME_T rTime; 

    u4SendSize = _rPVRStatus.u4QueuedSize;
    ePVRRet = SWDMX_PVR_SENDDATA_OK;

    if (u4SendSize == 0)
    {
        x_thread_delay(5);
        return MTR_OK;
    }
    
    // data already consumed.
    if (_rPVRStatus.u4FrameSize == 0)
    {
        if (_MTPVR_ReceiveFrameSize(ucPvrId, &u4FrameSize) == TRUE)
        {
           ASSERT(u4FrameSize != 0);
            _rPVRStatus.u4FrameSize = u4FrameSize;
        }
        else
        {
            x_thread_delay(5);
            LOG(3,"Get Frame Size Failed....\n");
            return MTR_OK;
        }
    }

    if (u4SendSize < _rPVRStatus.u4FrameSize)
    {
        x_thread_delay(5);
        LOG(5,"frame size not enough\n");
        return MTR_OK;
    }
    else
    {
        LOG(6,"Frame size ready... frame size = %d, send size = %d\n",_rPVRStatus.u4FrameSize, u4SendSize);
        u4SendSize = _rPVRStatus.u4FrameSize;
    }

    x_memset(&rDmxMMData, 0, sizeof(DMX_MM_DATA_T));
    x_memset(&rDmxDummyData, 0, sizeof(DMX_MM_DATA_T));
    
    if (_rPVRStatus.fgFirst)
    {
        rDmxMMData.fgFrameHead = TRUE;
        LOG(5, "%s(L:%d) First Move\n", __FUNCTION__, __LINE__);
    }
    
    HAL_GetTime(&rTime);
    LOG(3,"before Move Data %d micros\n",rTime.u4Micros);
    rDmxMMData.u4BufStart = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StartAddr;
    rDmxMMData.u4BufEnd = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4EndAddr;
    rDmxMMData.u4StartAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4ReadAddr;
    rDmxMMData.u4FrameSize = u4SendSize;
    rDmxMMData.rTimeHwS = rTime;
    rDmxMMData.u1Idx = _rPVRStatus.u1DmxPid;

    rDmxDummyData.fgEOS = TRUE;
    rDmxDummyData.fgIgnoreData = TRUE;
    rDmxDummyData.u4FrameSize = 4;
    rDmxDummyData.u4BufStart = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4StartAddr;
    rDmxDummyData.u4BufEnd = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4EndAddr;
    rDmxDummyData.u4StartAddr = _rPVRStatus.u4PvrBaseAddr + _rPVRStatus.u4ReadAddr;
    rDmxDummyData.u1Idx = _rPVRStatus.u1DmxPid;

#if 0
    {
        UINT32 u4DmxAvailSizeVideo;
        UINT16 u2DmxPictureQSize;
        UINT16 u2DmxMaxQSize;
        UINT16 u2QueueSize;
        UINT16 u2MaxQueueSize;
        UINT8 u1TsIdx;
        
        u1TsIdx = _DMX_MUL_GetTsIdx(_rPVRStatus.u1DmxId);

        u4DmxAvailSizeVideo = DMX_GetMinFreeBufferSizeEx(
                    u1TsIdx, DMX_PID_TYPE_ES_VIDEO);
        
         if((u4DmxAvailSizeVideo < _u4OutputSize) &&
            (u4DmxAvailSizeVideo != 0))
        {
            LOG(7,"return video size delayed\n");
            x_thread_delay(5);
            return MTR_OK;
        }
        else
        {
            LOG(8,"u4DmxAvailSizeVideo %d\n",u4DmxAvailSizeVideo);
        }

       // watch esmq size 
       u2QueueSize = 0;
       u2MaxQueueSize = 0;
       VDEC_GetQueueInfo(_rPVRStatus.ucEsId, &u2QueueSize, &u2MaxQueueSize);
                
        if((u2QueueSize > 0) && (u2MaxQueueSize > 0) &&
            (u2QueueSize >(u2MaxQueueSize - RESERVED_ESMQ_THRESHOLD)))
        {
            LOG(7,"return Q size delayed u2QueueSize %d, u2MaxQueueSize %d\n",
                    u2QueueSize,u2MaxQueueSize );
            x_thread_delay(5);
            return MTR_OK;
        }
        else
        {
            LOG(8,"u2QueueSize %d, u2MaxQueueSize %d\n",u2QueueSize,u2MaxQueueSize );
        }

        // watch pic Q size
        u2DmxPictureQSize = 0;
        u2DmxMaxQSize = 0;
        DMX_GetPicQSize(&u2DmxPictureQSize, &u2DmxMaxQSize);
                
        if((u2DmxPictureQSize >0) && (u2DmxMaxQSize > 0) &&
            (u2DmxPictureQSize > (u2DmxMaxQSize - RESERVED_PICQ_THRESHOLD)))
        {
            LOG(7,"return Dmx PicQ Size %d\n",u2DmxPictureQSize);
            x_thread_delay(5);
            return MTR_OK;
        }
        else
        {
            LOG(8,"==== Dmx Pic Q = %d ====\n",u2DmxPictureQSize);
        }
    }
#endif

    if (!DMX_MUL_MoveData( _rPVRStatus.u1DmxId, &rDmxMMData))
    {
        LOG(3,"DMX_MUL_MOVE_DATA FAIL\n");
    }
    else
    {
        LOG(7,"DMX_MUL_MOVE_DATA OK...\n");
    }
  
    if (!DMX_MUL_MoveData( _rPVRStatus.u1DmxId, &rDmxDummyData))
    {
        LOG(3,"DMX_MUL_MOVE_DATA FAIL DUMMY\n");
    }
    else
    {
        LOG(7,"DMX_MUL_MOVE_DATA OK DUMMY...\n");
    }
    
#if 0
    if (ePVRRet == SWDMX_PVR_SENDDATA_FULL)
    {
        LOG(8,"SWDMX_PVR_SEND_DATA_FULL\n");
        x_thread_delay(5);
        return MTR_OK;
    }
    else if (ePVRRet != SWDMX_PVR_SENDDATA_OK)
    {
        //printf("_SWDMX_PVR_SendData -> not ok\n");
        LOG(5, "%s(L:%d) (ePVRRet(%d) != PVR_PVR_SENDDATA_OK) error\n", __FUNCTION__, __LINE__,
            (UINT32)ePVRRet);
        MTPVR_PRINT("%s (ePVRRet(%d) != PVR_PVR_SENDDATA_OK) error\n",
            __FUNCTION__, (UINT32)ePVRRet);
        x_thread_delay(10);
        return MTR_NOT_OK;
    }
#endif
    
    if (_rPVRStatus.fgFirst)
    {
        _rPVRStatus.fgFirst = FALSE;
    }
    _rPVRStatus.u4ReadAddr += u4SendSize;
    _MTPVR_Lock();
    _rPVRStatus.u4QueuedSize -= u4SendSize;
    _rPVRStatus.u4FrameSize = 0;
    _MTPVR_Unlock();
    if (_rPVRStatus.u4ReadAddr >= _rPVRStatus.u4EndAddr)
    {
        _rPVRStatus.u4ReadAddr -= (_rPVRStatus.u4EndAddr - _rPVRStatus.u4StartAddr);
    }
    #if 0
    else if(_rPVRStatus.u4ReadAddr > _rPVRStatus.u4EndAddr)
    {
        MTPVR_PRINT("%s u4ReadAddr(%d) > u4EndAddr(%d)\n",
            __FUNCTION__, _rPVRStatus.u4ReadAddr, _rPVRStatus.u4EndAddr);
        ASSERT(0);
        return MTR_NOT_OK;
    }
    #endif

    LOG(6, "SendData, Size(0x%06x) Addr(0x%08x) Q(0x%06x) R(0x%08x) W(0x%08x)\n",
            u4SendSize, _rPVRStatus.u4ReadAddr, _rPVRStatus.u4QueuedSize,
            _rPVRStatus.u4ReadAddr, _rPVRStatus.u4WriteAddr);
    
    return eRet;
}

static VOID _MTPVR_ES_PushLoop(VOID *pvArg)
{
    UCHAR ucPvrId;
    UINT32 u4PlayCmd;

    ucPvrId = (UCHAR)(*((UINT8*)pvArg));
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("_PVR_MainLoop (ucPvrId >= PVR_SOURCE_MAX)\n");
    }

    while (1)
    {
        // block if SwDmx status is not ePVR_STATE_PLAY
        u4PlayCmd = (UINT32)ePVR_STATE_MAX;
        
        if (_MTPVR_ReceivePlayESCmd(ucPvrId, &u4PlayCmd, _rPVRStatus.ePVRStatus) == TRUE)
        {
            #if 0
            if(_rPVRStatus.ePVRStatus != u4PlayCmd)
            {
                printf(" _rPVRStatus.ePVRStatus = %d , u4PlayCmd = %d \n", _rPVRStatus.ePVRStatus, u4PlayCmd);
            }
            #endif
        
            if ((_rPVRStatus.ePVRStatus == ePVR_STATE_STOP) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_PLAY))
            {
                LOG(3,"%s received Play CMD\n",__FUNCTION__);
            }

            if ((_rPVRStatus.ePVRStatus == ePVR_STATE_PAUSE) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_PLAY))
            {
                LOG(3,"%s received Play CMD\n",__FUNCTION__);
            }

            if ((_rPVRStatus.ePVRStatus == ePVR_STATE_PLAY) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_PAUSE))
            {
                LOG(3,"%s received Pause CMD\n",__FUNCTION__);
            }

            if ((_rPVRStatus.ePVRStatus != ePVR_STATE_STOP) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_STOP))
            {
                LOG(3,"%s received Stop CMD\n",__FUNCTION__);
                VDEC_SetMMParam(_rPVRStatus.ucEsId, VDEC_MM_CONTAINER_TYPE, SWDMX_FMT_UNKNOWN, 0, 0);
                _MTPVR_StopFinished();
            }
            else if (_rPVRStatus.ePVRStatus == ePVR_STATE_STOP &&
                       (u4PlayCmd == (UINT32)ePVR_STATE_STOP))
            {
                // prevent dead lock.
                _MTPVR_StopFinished();
            }

            if (u4PlayCmd == (UINT32)ePVR_CMD_FLUSH)
            {
                _MTPVR_CleanBuf(FALSE);
                _MTPVR_ResetBufferFinished();
                continue;
            }

            _rPVRStatus.ePVRPrevousStatus = _rPVRStatus.ePVRStatus;
            UNUSED(_rPVRStatus.ePVRPrevousStatus);
            _rPVRStatus.ePVRStatus = (ENUM_MTPVR_PLAY_STATE_T)u4PlayCmd;
            
            continue;
        }

        if (_rPVRStatus.ePVRStatus == ePVR_STATE_PLAY)
        {
            _MTPVR_SendESData(ucPvrId);
        }
        else if (_rPVRStatus.ePVRStatus == ePVR_STATE_PAUSE)
        {
            x_thread_delay(50);
        }
    }
    
}


static VOID _MTPVR_MainLoop(VOID *pvArg)
{
    UCHAR ucPvrId;
    UINT32 u4PlayCmd;
    DMX_PVR_PLAY_T rPvrPlay;

    ucPvrId = (UCHAR)(*((UINT8*)pvArg));
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("_PVR_MainLoop (ucPvrId >= PVR_SOURCE_MAX)\n");
    }

    MTPVR_PRINT("Entry _MTPVR_MainLoop!)\n");
    while (1)
    {
        // block if SwDmx status is not ePVR_STATE_PLAY
        u4PlayCmd = (UINT32)ePVR_STATE_MAX;
        if (_MTPVR_ReceivePlayCmd(ucPvrId, &u4PlayCmd, _rPVRStatus.ePVRStatus) == TRUE)
        {
        #if 0
            if(_rPVRStatus.ePVRStatus != u4PlayCmd)
            {
                printf(" _rPVRStatus.ePVRStatus = %d , u4PlayCmd = %d \n", _rPVRStatus.ePVRStatus, u4PlayCmd);
            }
        #endif
        
            if ((_rPVRStatus.ePVRStatus == ePVR_STATE_STOP) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_PLAY))
            {
                // start play
                #ifdef VDEC_IS_POST_MT5368
                if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
                    (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
                {
                    VDP_SetTimeShiftMode(0,1);
                    _DMX_SetTimeShiftMode(1);
                }
                #endif
            }

            if ((_rPVRStatus.ePVRStatus == ePVR_STATE_PAUSE) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_PLAY))
            {
                // pause to play
               
                LOG(5, "pause to play stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
                if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
                    (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
                {
                    SWDMX_PVR_Resume(ucPvrId);
                }
                STC_StartStc(_rPVRStatus.u1StcId);
            }

            if ((_rPVRStatus.ePVRStatus == ePVR_STATE_PLAY) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_PAUSE))
            {
                // play to pause
                STC_StopStc(_rPVRStatus.u1StcId);
                if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
                    (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
                {
                    SWDMX_PVR_Pause(ucPvrId);
                }
                LOG(5, "play to pause stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
            }

            if ((_rPVRStatus.ePVRStatus != ePVR_STATE_STOP) &&
                    (u4PlayCmd == (UINT32)ePVR_STATE_STOP))
            {
                if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
                    (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
                {
                    #ifdef VDEC_IS_POST_MT5368
                    VDP_SetTimeShiftSpeed(0, 100);
                    VDP_SetTimeShiftMode(0,0);
                    _DMX_SetTimeShiftMode(0);
                    #endif
                    
                    // stop, clear data
                    if (!SWDMX_PVR_Stop(ucPvrId))
                    {
                        MTPVR_PRINT("%s SWDMX_PVR_Stop fail\n",
                            __FUNCTION__);
                    }

                    if (!SWDMX_PVR_Close(ucPvrId))
                    {
                        MTPVR_PRINT("%s SWDMX_PVR_Close fail\n",
                            __FUNCTION__);
                    }

                    UNUSED(DMX_SetTSMode(ucPvrId, DMX_TSFMT_188));       // Default
                }

                if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
                    (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
                {
                    VDEC_SetMMParam(_rPVRStatus.ucEsId, VDEC_MM_CONTAINER_TYPE, SWDMX_FMT_UNKNOWN, 0, 0);
                }

                VDEC_SetMMParam(_rPVRStatus.ucEsId, VDEC_MM_CONTAINER_TYPE, SWDMX_FMT_UNKNOWN, 0, 0);
                _MTPVR_StopFinished();
            }
            else if (_rPVRStatus.ePVRStatus == ePVR_STATE_STOP &&
                     (u4PlayCmd == (UINT32)ePVR_STATE_STOP))
            {
                // prevent dead lock.
                _MTPVR_StopFinished();
            }

            if (u4PlayCmd == (UINT32)ePVR_CMD_FLUSH)
            {
                if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
                    (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
                {
                    if (!SWDMX_PVR_Stop(ucPvrId))
                    {
                        MTPVR_PRINT("%s _SWDMX_PVR_Stop fail\n", __FUNCTION__);
                    }

                    if (!DMX_PVRPlay_RequestReset())
                    {
                        MTPVR_PRINT("%s DMX_PVRPlay_RequestReset fail\n", __FUNCTION__);
                    }
                }
                _MTPVR_CleanBuf(TRUE);
                _MTPVR_ResetBufferFinished();
                continue;
            }

            _rPVRStatus.ePVRPrevousStatus = _rPVRStatus.ePVRStatus;
            UNUSED(_rPVRStatus.ePVRPrevousStatus);
            _rPVRStatus.ePVRStatus = (ENUM_MTPVR_PLAY_STATE_T)u4PlayCmd;
            continue;
        }

        if (_rPVRStatus.ePVRStatus == ePVR_STATE_PLAY)
        {
            if (_rPVRStatus.fgSpeedChange)
            {
                if (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode)
                {
                    INT32 i4PreSpeed = _rPVRStatus.i4Speed;
                    _rPVRStatus.i4Speed = _rPVRStatus.i4SpeedNew;
                    _rPVRStatus.fgSpeedChange = FALSE;

                    // DMX
         
                    // Set Speed to swdmx 

                    //_MTPVR_Lock();
                    
                    //set speed pvr speed to display module
                    #ifdef VDEC_IS_POST_MT5368
                    VDP_SetTimeShiftSpeed(0,_rPVRStatus.i4Speed);
                    #endif

                    if (((_rPVRStatus.i4Speed == 25) ||
                        (_rPVRStatus.i4Speed == 50) ||
                        (_rPVRStatus.i4Speed == 100) ||
                        (_rPVRStatus.i4Speed == 200)))
                    {
                        STC_StopStc(_rPVRStatus.u1StcId);
                        SWDMX_PVR_Pause(ucPvrId);                       
                        _rPVRStatus.rPvrPlay.fgContainTimeStamp = _fgContainTimeStamp;

                        // take care of playing 188 ts
                        if (_fgContainTimeStamp)
                        {
                            _rPVRStatus.rPvrPlay.fgIgnoreTimeStamp = FALSE;
                        }
                        else
                        {
                            _rPVRStatus.rPvrPlay.fgIgnoreTimeStamp = TRUE;
                        }
                        
                        _rPVRStatus.rPvrPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE * 100 / _rPVRStatus.i4Speed;

                        //if(_rPVRStatus.i4Speed == 100)
                        if(!((i4PreSpeed == 25) ||
                            (i4PreSpeed == 50) ||
                            (i4PreSpeed == 100) ||
                            (i4PreSpeed == 200)))
                        {
                            SWDMX_PVR_FlushBuf(ucPvrId);
                        }

                        rPvrPlay.eMode = DMX_PVR_PLAY_STREAM;
                        rPvrPlay.fgContainTimeStamp = _rPVRStatus.rPvrPlay.fgContainTimeStamp;
                        rPvrPlay.fgIgnoreTimeStamp = _rPVRStatus.rPvrPlay.fgIgnoreTimeStamp;
                        rPvrPlay.u2TimeStampFreqDiv = _rPVRStatus.rPvrPlay.u2TimeStampFreqDiv;
                        rPvrPlay.u4ThresholdSize = 0x800000;
                        
                        LOG(6,"Enter Stream mode rPlay.u2TimeStampFreqDiv = %d;\n",DMX_PVRPLAY_TIMESTAMP_DIV_BASE);

                        if (!DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_MODE| PVRPLAY_FLAGS_TIMESTAMP | PVRPLAY_FLAGS_THRESHOLD), &rPvrPlay))
                        {
                            MTPVR_PRINT("%s fail to set timestamp.\n", __FUNCTION__);
                        }
                        
                        LOG(6, "fgContainTimeStamp %d, fgIgnoreTimeStamp %d, u2TimeStampFreqDiv %d\n",_rPVRStatus.rPvrPlay.fgContainTimeStamp,
                               _rPVRStatus.rPvrPlay.fgIgnoreTimeStamp,_rPVRStatus.rPvrPlay.u2TimeStampFreqDiv);
                        
                        SWDMX_PVR_Resume(ucPvrId);
                        STC_StartStc(_rPVRStatus.u1StcId);
                    }
                    else if ((_rPVRStatus.i4Speed == 0) || (_rPVRStatus.i4Speed == 1))
                    {
                    }
                    else
                    {
                        if (!DMX_PVRPlay_Get((UINT32)(PVRPLAY_FLAGS_TIMESTAMP), &rPvrPlay))
                        {
                            MTPVR_PRINT("%s fail to set timestamp.\n", __FUNCTION__);
                        }

                        if (rPvrPlay.fgIgnoreTimeStamp == FALSE)
                        {
                            SWDMX_PVR_Pause(ucPvrId);                       
                            _rPVRStatus.rPvrPlay.fgContainTimeStamp = _fgContainTimeStamp;
                            _rPVRStatus.rPvrPlay.fgIgnoreTimeStamp = TRUE;
                            _rPVRStatus.rPvrPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;

                            //SWDMX_PVR_FlushBuf(ucPvrId);

                            rPvrPlay.fgContainTimeStamp = _rPVRStatus.rPvrPlay.fgContainTimeStamp;
                            rPvrPlay.fgIgnoreTimeStamp = _rPVRStatus.rPvrPlay.fgIgnoreTimeStamp;
                            rPvrPlay.u2TimeStampFreqDiv = _rPVRStatus.rPvrPlay.u2TimeStampFreqDiv;

                            #if 0
                            rPvrPlay.eMode = DMX_PVR_PLAY_SINGLEMOVE;
                            if(!DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_MODE | PVRPLAY_FLAGS_TIMESTAMP), &rPvrPlay))
                            #endif
                            
                            if (!DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_TIMESTAMP), &rPvrPlay))
                            {
                                MTPVR_PRINT("%s fail to set timestamp.\n", __FUNCTION__);
                            }
                            
                            SWDMX_PVR_Resume(ucPvrId);       
                        } 
                    }
                }
                else
                {
                    printk("Do nothing for speed change.\n");
                }
            }

            _MTPVR_SendData(ucPvrId);
        }
        else if (_rPVRStatus.ePVRStatus == ePVR_STATE_PAUSE)
        {
            x_thread_delay(50);
        }
    }
}

//-----------------------------------------------------------------------------
/*
 *  Init demux driver
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_Init(unsigned long arg)
{
    UINT32 i = 0;
    FBM_POOL_T *prFbmPool;

    if (!_fgPVRInit)
    {
        _fgPVRInit = TRUE;
        
        // create play command queue
        VERIFY(x_msg_q_create(&_rPVRStatus.hPVRPlayCmdQ,
            MTPVR_CMD_NAME,
            sizeof(UINT32),
            MTPVR_CMD_Q_SIZE) == OSR_OK);        
        
        // create play ES command queue
        VERIFY(x_msg_q_create(&_rPVRStatus.hPVRPlayESCmdQ,
              MTPVR_CMD_NAME_ES,
              sizeof(UINT32),
              MTPVR_CMD_Q_SIZE) == OSR_OK);
        
        VERIFY(x_thread_create(&_rPVRStatus.hPVRMainThread,
            MTPVR_THREAD_NAME,
            MTPVR_THREAD_STACK_SIZE,
            MTPVR_THREAD_PRIORITY,
            (x_os_thread_main_fct)_MTPVR_MainLoop, sizeof(UINT8), (VOID*)&i) == OSR_OK);

        //create es push thread
        VERIFY(x_thread_create(&_rPVRStatus.hPVRPlayESThread,
            MTPVR_ES_THREAD_NAME,
            MTPVR_THREAD_STACK_SIZE,
            MTPVR_THREAD_PRIORITY,
            (x_os_thread_main_fct)_MTPVR_ES_PushLoop, sizeof(UINT8), (VOID*)&i) == OSR_OK);

        VERIFY(x_timer_create(&_rPVRStatus.hThumbTimer) == OSR_OK);

        // create play Frame size Q
         VERIFY(x_msg_q_create(&_rPVRStatus.hPVRPlayFrameSizeQ,
               MTPVR_FRAME_SIZE_NAME,
               sizeof(UINT32),
               MTPVR_FRAME_Q_SIZE) == OSR_OK);

        VERIFY(x_sema_create(&_hQueueSema, X_SEMA_TYPE_BINARY,
            X_SEMA_STATE_UNLOCK) == OSR_OK);
    
        VERIFY(x_sema_create(&_hStopSema, X_SEMA_TYPE_BINARY,
            X_SEMA_STATE_LOCK) == OSR_OK);
        VERIFY(x_sema_create(&_hResetSema, X_SEMA_TYPE_BINARY,
            X_SEMA_STATE_LOCK) == OSR_OK);
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_FEEDER);
        if(!prFbmPool)
        {
            MTPVR_PRINT("%s prFbmPool null\n",
                __FUNCTION__);
            ASSERT(0);
            return MTR_NOT_OK;
        }
        
        _rPVRStatus.u4FeederAddrPhy = (prFbmPool->u4Addr);
        if ((prFbmPool->u4Size == 0) || (prFbmPool->u4Addr == 0))
        {
            MTPVR_PRINT("%s prFbmPool buffer null\n",
                __FUNCTION__);
            ASSERT(0);
            return MTR_NOT_OK;
        }
        
        _rPVRStatus.u4FeederAddrVir = (VIRTUAL(_rPVRStatus.u4FeederAddrPhy));
        if (_rPVRStatus.u4FeederAddrVir == 0)
        {
            MTPVR_PRINT("%s u4FeederAddrVir null\n",
                __FUNCTION__);
            ASSERT(0);
            return MTR_NOT_OK;
        }
        
        if (prFbmPool->u4Size <
            (MTPVR_THUMBOUTSIZE + MTPVR_THUMBWORKSIZE + MTPVR_PVRDATASIZE))
        {
            MTPVR_PRINT("%s buffer not enough\n",
                __FUNCTION__);
            ASSERT(0);
            return MTR_NOT_OK;
        }
        
        _rPVRStatus.u4PvrBaseAddr = (_rPVRStatus.u4FeederAddrVir + MTPVR_PVRDATAOFST);
        _rPVRStatus.u4PvrBufSize = (MTPVR_PVRDATASIZE - _u4InputSize); // the last one is the buffer for wrap around temp buffer
        _rPVRStatus.fgH264ESMode = FALSE;
        _rPVRStatus.u1DmxId = 0xff;
        _rPVRStatus.u1DmxPid = 0xff;
        _rPVRStatus.u4FrameSize = 0;
        _rPVRStatus.fgStarted = FALSE;
        _rPVRStatus.fgSpeedChange = FALSE;
        _rPVRStatus.i4Speed = 100;
        _rPVRStatus.i4SpeedNew = 100;
        _rPVRStatus.u1AudId = MT_AUD_DEC_AUX;
        _rPVRStatus.u1StcId = STC_SRC_A1;
        _rPVRStatus.u1B2rId = 0;
        _rPVRStatus.ucTsIdx = 2;
        _rPVRStatus.ePVRVideoStatus = ePVR_STREAM_MAX;
        _rPVRStatus.ePVRAudioStatus = ePVR_STREAM_MAX;

        _rPVRStatus.fgSecure = FALSE;     

        _MTPVR_CleanBuf(TRUE);
        _MTPVR_ResetMpgInfo(PVR0);
    
        MTPVR_PRINT(" - _MTPVR_Init\n");
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Start secure, must call it front of MTPVR_Start() and back of MTPVR_Init();
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_SetSecure(unsigned long arg)
{  
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucPvrId;
    BOOL fgSecure;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    fgSecure = (BOOL)rArg.ai4Arg[1];

    _rPVRStatus.fgSecure = fgSecure;
    
    return eRet;
}

//-----------------------------------------------------------------------------
/*
 *  Start demux
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_Start (unsigned long arg)
{
    UCHAR ucPvrId;
    DMX_PVR_PLAY_T rPlay;
    UINT8 u1Inst = 0;
    //UINT8 i;
    
    ucPvrId = (UCHAR)arg;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if (_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s, already started\n", __FUNCTION__);
        return MTR_OK;
    }

    LOG(1, "%s\n", __FUNCTION__);

    if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) &&
        (ENUM_MTPVR_PLAYMODE_192 == _rPVRStatus.ePlayMode))
    {
        u1Inst = DMX_MUL_GetAvailableInst(DMX_CONN_TYPE_TUNER_2);
        if (u1Inst == DMX_MULTIPLE_NULL_INSTANCE)
        {
            return FALSE;
        }

        if (!DMX_MUL_SetInstType(u1Inst, DMX_IN_PLAYBACK_TS))
        {
            MTPVR_PRINT("%s, DMX_MUL_SetInstType fail\n", __FUNCTION__);
            return FALSE;
        }
        
        _MTDMX_SetDecoderEx(u1Inst);
        
    /*
        for (i = 0; i < DMX_NUM_PID_INDEX; i++)
        {
            // change each pid index to PVR
            DMX_PID_T rPid;
            DMX_GetPid(i, DMX_PID_FLAG_VALID, &rPid);
            if (rPid.fgEnable == TRUE)
            {
                rPid.eInputType = DMX_IN_PLAYBACK_TS;
                rPid.u1TsIndex = 2;
                DMX_SetPid(i, DMX_PID_FLAG_INPUTTYPE | DMX_PID_FLAG_TS_INDEX, &rPid);
            }
        }
    */

        if (!SWDMX_PVR_Init(ucPvrId))
        {
            MTPVR_PRINT("%s, SWDMX_PVR_Init fail\n", __FUNCTION__);
            return FALSE;
        }
        
        if (!SWDMX_PVR_OPEN2(ucPvrId, _rPVRStatus.u4PvrBaseAddr, _rPVRStatus.u4PvrBufSize))
        {
            MTPVR_PRINT("%s, SWDMX_PVR_OPEN2 fail\n", __FUNCTION__);
            return FALSE;
        }

        // Make it single move in the begining
        if (!SWDMX_PVR_SetDataMode(ucPvrId, 1))
        {
            MTPVR_PRINT("%s, SWDMX_PVR_SetDataMode fail\n", __FUNCTION__);
        }
        
        UNUSED(DMX_SetTSMode(u1Inst, DMX_TSFMT_TIMESHIFT));

        // change to 188 bytes
        if (_fgContainTimeStamp == FALSE)
        {
            // reset STC
            STC_StopStc(_rPVRStatus.u1StcId);
            STC_SetStcValue(_rPVRStatus.u1StcId, 0);
            STC_StartStc(_rPVRStatus.u1StcId);
            rPlay.eMode = DMX_PVR_PLAY_SINGLEMOVE;
            rPlay.fgContainTimeStamp = _fgContainTimeStamp;
            rPlay.fgIgnoreTimeStamp = TRUE;
            rPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
            if (_rMtPvrMpgInfo.fgAudioMaster)
            {
                VDEC_PlayMM(ucPvrId, TRUE);
            }

            //if(!DMX_MUL_ForceToFreePCR(TRUE))
            //{
            //     LOG(0, "%s(%d): DMX_MUL_ForceToFreeVdec0 fail\n", __FUNCTION__, __LINE__);
            //}
            
            if (!DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_MODE | PVRPLAY_FLAGS_TIMESTAMP), &rPlay))
            {
                MTPVR_PRINT("%s fail to set timestamp.\n", __FUNCTION__);
            }
        }

        // set pid will call it in io_mtdmx.c
        // _MtdmxSetDecoder();
    }
    else
    {
        DMX_DECODER_CALLBACKS_T rDecoderCallbacks;
        
        x_memset(&rDecoderCallbacks, 0, sizeof(rDecoderCallbacks));
        rDecoderCallbacks.pfnSendVideoPes = _MTPVR_VideoCallback;
        rDecoderCallbacks.pfnUpdateVideoWp = VDEC_SetWptr;
        rDecoderCallbacks.pfnSendAudioPes = _MTPVR_AudioCallback;
        
        LOG(5, "%s:%d Set Dmx Es callback info. Play mode(%d)\n", __FUNCTION__, __LINE__, _rPVRStatus.ePlayMode);
        DMX_MUL_SetDecoderCallbacks(_rPVRStatus.u1DmxId, &rDecoderCallbacks);
    }

    if ((ENUM_MTPVR_PLAYMODE_192_AUDIO_MASTER == _rPVRStatus.ePlayMode) ||
        (ENUM_MTPVR_PLAYMODE_192_SYSMASTER==_rPVRStatus.ePlayMode) ||
        (ENUM_MTPVR_PLAYMODE_192==_rPVRStatus.ePlayMode))
    {
        UNUSED(DMX_SetTSMode(_rPVRStatus.u1DmxId, DMX_TSFMT_192));
    }
    else if ((ENUM_MTPVR_PLAYMODE_188 == _rPVRStatus.ePlayMode) ||
             (ENUM_MTPVR_PLAYMODE_188_AUDIO_MASTER == _rPVRStatus.ePlayMode) ||
             (ENUM_MTPVR_PLAYMODE_188_SYSMASTER == _rPVRStatus.ePlayMode))
    {
        UNUSED(DMX_SetTSMode(_rPVRStatus.u1DmxId, DMX_TSFMT_188));
    }
    else if (ENUM_MTPVR_PLAYMODE_204 == _rPVRStatus.ePlayMode)
    {
        UNUSED(DMX_SetTSMode(_rPVRStatus.u1DmxId, DMX_TSFMT_204));
    }
    
    _rPVRStatus.u4StartAddr = 0;
    _rPVRStatus.u4ReadAddr = 0;
    _rPVRStatus.u4WriteAddr = 0;
    _rPVRStatus.u4QueuedSize = 0;

    _rPVRStatus.u4StampReadAddr = 0;
    _rPVRStatus.u4StampWriteAddr = 0;
    _rPVRStatus.u4StampQSize = 0;
    _rPVRStatus.fgFoundPcr = FALSE;
    _rPVRStatus.u2PcrPid = 0;
    _rPVRStatus.u4LastTimeStamp = 0;
    _rPVRStatus.u4LastTimeDelta = 0;
    _rPVRStatus.u4PacketCountbwPcrs = 0;
    _rPVRStatus.i8LastPcr = 0;
    _rPVRStatus.u4PreloadSize = PRELOAD_BUFFER_SIZE_192_EX;

    _rPVRStatus.ePVRVideoStatus = ePVR_CHECK_STREAM;
    _rPVRStatus.ePVRAudioStatus = ePVR_CHECK_STREAM;
    _rPVRStatus.u4PvrBufSize = (MTPVR_PVRDATASIZE - _u4InputSize);
    _rPVRStatus.u4EndAddr = _rPVRStatus.u4PvrBufSize;
    LOG(5, "Start, total(0x%x)\n", _rPVRStatus.u4PvrBufSize);

    _rPVRStatus.fgFirst = TRUE;

    if(_rPVRStatus.fgSecure)
    {   
        _rPVRStatus.u4StartAddr = 0;
        _rPVRStatus.u4ReadAddr = 0;
        _rPVRStatus.u4WriteAddr = 0;        
        _rPVRStatus.u4QueuedSize = 0;

        //TZ_DMX_GetSecurePvr(&_rPVRStatus.u4PvrBaseAddr, &_rPVRStatus.u4PvrBufSize);
        _rPVRStatus.u4PvrBaseAddr = _rPVRStatus.u4PvrBaseAddr;
        _rPVRStatus.u4PvrBufSize  = _rPVRStatus.u4PvrBufSize;
        
        if(ucPvrId == 0)
        {
            _rPVRStatus.u4PvrBaseAddr = _rPVRStatus.u4PvrBaseAddr;
        }
        else if(ucPvrId == 1)
        {
            _rPVRStatus.u4PvrBaseAddr = _rPVRStatus.u4PvrBaseAddr + 0x300000;  //secure buffer size is 6M;
        }
        else
        {
            MTPVR_PRINT("%s ucPvrId(%d) > 1\n", __FUNCTION__, ucPvrId);
            return MTR_NOT_OK;
        }

        _rPVRStatus.u4PvrBufSize = _rPVRStatus.u4PvrBufSize - _u4InputSize;
        _rPVRStatus.u4EndAddr = _rPVRStatus.u4PvrBufSize;
    }

    MTPVR_PRINT("%s - OK\n", __FUNCTION__);

    _rPVRStatus.fgStarted = TRUE;
    _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_PLAY, 255);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Start demux for ES Playback 
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_StartPlayES (unsigned long arg)
{
    UCHAR ucPvrId;
    UCHAR ucEsId;
    UINT32 u4Flag;
    UINT32 u4VfifoAddr=0, u4VfifoSize=0;
    MTVDEC_FMT_T eMtVdecFmt;
    DMX_VIDEO_TYPE_T eDmxFmt;
    DMX_MM_T rDmxMM;
    MTAL_IOCTL_3ARG_T    rArg;
    DMX_DECODER_CALLBACKS_T rDmxCallback;
    
    x_memset(&rDmxMM, 0, sizeof(DMX_MM_T));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
  
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    ucPvrId = (UINT32)rArg.ai4Arg[1];
    eMtVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[2];
    
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_SUPPORTED;
    }
    
    if (ucEsId >= VDEC_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if (_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s, already started\n", __FUNCTION__);
        return MTR_OK;
    }

    MTPVR_PRINT("%s\n", __FUNCTION__);

    switch (eMtVdecFmt)
    {
        case MTVDEC_FMT_MPEG1_MPEG2:
            eDmxFmt = DMX_VIDEO_MPEG;
        break;
        
        case MTVDEC_FMT_H264:
            eDmxFmt = DMX_VIDEO_H264;        
        break;
        
        case MTVDEC_FMT_VC1:
            eDmxFmt = DMX_VIDEO_VC1;        
        break;
        
        case MTVDEC_FMT_RV:
            eDmxFmt = DMX_VIDEO_RV;
        break;
        
        case MTVDEC_FMT_AVS:
            eDmxFmt = DMX_VIDEO_AVS;        
        break;    
        
        case MTVDEC_FMT_VP8:
            eDmxFmt = DMX_VIDEO_VP8;        
        break;
        
        case MTVDEC_FMT_VP6:
            eDmxFmt = DMX_VIDEO_VP6;        
        break;
        
        case MTVDEC_FMT_UNKNOWN:
            eDmxFmt = DMX_VIDEO_NONE;        
        break;
        
        default:
            eDmxFmt = DMX_VIDEO_NONE;        
        break; 
    }

    if (eDmxFmt == DMX_VIDEO_NONE)
    {
        LOG(3, "%s(%d): DMX Format Not Supported\n", __FUNCTION__, __LINE__);
        return MTR_NOT_OK;
    }

    DMX_MUL_EnableMultipleInst(TRUE);

    // TODO: we need to take more concern here.
    _rPVRStatus.u1DmxId = DMX_MUL_GetAvailableInst((DMX_CONN_TYPE_T)((UINT32)DMX_CONN_TYPE_DIRECT_PLAY_1 + ucPvrId));
    
    if (_rPVRStatus.u1DmxId  == 0xFF)
    {
        LOG(3, "%s(%d): DMX_MUL_GetAvailableInst fail\n", __FUNCTION__, __LINE__);
        return MTR_NOT_OK;
    }
    else
    {
        LOG(3, "DMX ID = %d\n",_rPVRStatus.u1DmxId);
    }

    if (!DMX_MUL_SetInstType(_rPVRStatus.u1DmxId , DMX_IN_PLAYBACK_ES))
    {
        LOG(3, "%s(%d): DMX_MUL_SetInstTypefail\n", __FUNCTION__, __LINE__);
        return MTR_NOT_OK;
    }


    if (!DMX_MUL_SetVideoType(_rPVRStatus.u1DmxId, eDmxFmt))
    {
        LOG(3, "%s(%d): DMX_MUL_SetVideoType\n", __FUNCTION__, __LINE__);
        return MTR_NOT_OK;  
    }

    DMX_SetToDecoder(TRUE);

    // for video callback
    x_memset(&rDmxCallback, 0, sizeof(rDmxCallback));
    rDmxCallback.pfnSendVideoPes = _MTPVR_VideoCallback;
    rDmxCallback.pfnUpdateVideoWp = VDEC_SetWptr;
    DMX_MUL_SetDecoderCallbacks(_rPVRStatus.u1DmxId,&rDmxCallback);

    // get AvailablePidx of dmx
    _rPVRStatus.u1DmxPid = DMX_MUL_GetAvailablePidx(_rPVRStatus.u1DmxId);
    _rPVRStatus.ucEsId = ucEsId;
    
    if (_rPVRStatus.u1DmxPid  >= DMX_NUM_PID_INDEX)
    {
        LOG(3, "%s(%d): (prVdec->u1DmxPid == DMX_MULTIPLE_NULL_PIDX)\n", __FUNCTION__, __LINE__);
        return MTR_NOT_OK;
    }

    x_memset(&rDmxMM, 0, sizeof(DMX_MM_T));
    
    u4Flag = (UINT32)(DMX_MM_FLAG_TYPE
            | DMX_MM_FLAG_INSTANCE_TAG
            | DMX_MM_FLAG_VALID
            | DMX_MM_FLAG_BUF
            | DMX_MM_FLAG_SEARCH_START_CODE);

    if (!_prDmxFbmPool)
    {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
        _prDmxFbmPool = FBM_Alloc(FBM_TYPE_TZ_DMX);
#else
        _prDmxFbmPool = FBM_Alloc(FBM_TYPE_DMX);
#endif
    }
    
    if (!_prDmxFbmPool || (_prDmxFbmPool->u4Addr == 0) || (_prDmxFbmPool->u4Size == 0))
    {
        LOG(1, "%s:%d  prFbmPool->u4Addr/u4Size 0\n", __FUNCTION__, __LINE__);
        return MTR_NOT_OK;
    }
    
    u4VfifoAddr = _prDmxFbmPool->u4Addr;
    u4VfifoSize = _prDmxFbmPool->u4Size;
    
    rDmxMM.fgSearchStartCode = TRUE;
    rDmxMM.fgEnable = TRUE;
    rDmxMM.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rDmxMM.pvInstanceTag = (void*)(&_rPVRStatus);
    rDmxMM.fgAllocBuf = FALSE;
     
    // same with VDEC id
    rDmxMM.u4BufAddr = u4VfifoAddr;
    rDmxMM.u4BufSize = (u4VfifoSize >> 1);
    rDmxMM.u1ChannelId = ucEsId;
    DMX_MM_Set(_rPVRStatus.u1DmxPid, u4Flag, &rDmxMM);
    
    VDEC_PlayMM(ucEsId, TRUE);
    VDEC_SetMMParam(ucEsId, VDEC_MM_SRC_TYPE, (UINT32)SWDMX_SRC_TYPE_HW_DIRECT_PLAY, 0, 0);
    VDEC_SyncStc(ucEsId, VID_SYNC_MODE_NONE, _rPVRStatus.u1StcId);
    
    // TODO: Here concern
    _rPVRStatus.u4StartAddr = 0;
    _rPVRStatus.u4ReadAddr = 0;
    _rPVRStatus.u4WriteAddr = 0;
    _rPVRStatus.u4QueuedSize = 0;
    _rPVRStatus.u4EndAddr = _rPVRStatus.u4PvrBufSize;
    LOG(5, "Start, total(0x%x)\n", _rPVRStatus.u4PvrBufSize);

    _rPVRStatus.fgFirst = TRUE;

    MTPVR_PRINT("%s - OK\n", __FUNCTION__);

    _rPVRStatus.fgStarted = TRUE;
    _rPVRStatus.fgH264ESMode = TRUE;
    _MTPVR_SendPlayESCmd(ucPvrId, ePVR_STATE_PLAY, 255);

    return MTR_OK;    
}

static MT_RESULT_T _MTPVR_StopESPlay(unsigned long arg)
{
    UCHAR ucPvrId;
    INT32 i4Ret;
    DMX_MM_T rDmxMM;
    
    ucPvrId = (UCHAR)arg;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if (!_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s not started\n", __FUNCTION__);
        return MTR_OK;
    }

    MTPVR_PRINT("%s\n", __FUNCTION__);
    
    _rPVRStatus.fgStarted = FALSE;
    _rPVRStatus.fgSpeedChange = FALSE;
    _rPVRStatus.fgH264ESMode = FALSE;
    _rPVRStatus.i4Speed = 100;
    _rPVRStatus.i4SpeedNew = 100;
    _MTPVR_ResetMpgInfo(PVR0);

    if (_fgContainTimeStamp)
    {
        // clear lock
        x_sema_lock(_hStopSema, X_SEMA_OPTION_NOWAIT);
    }
    
    _MTPVR_SendPlayESCmd(ucPvrId, ePVR_STATE_STOP, 255);
        
    if (_fgContainTimeStamp)
    {
        // wait ctrl done
        i4Ret = x_sema_lock_timeout(_hStopSema, 5000);
    
        if (i4Ret != OSR_OK)
        {
            printf("_MTPVR stop timeout\n");
        }
    }
    
    if (_rPVRStatus.u1DmxId != 0xff)
    {
        x_memset(&rDmxMM, 0, sizeof(DMX_MM_T));
    
        rDmxMM.fgEnable = FALSE;
        VERIFY(DMX_MM_Set(_rPVRStatus.u1DmxPid, DMX_MM_FLAG_VALID, &rDmxMM));
        DMX_MM_Free(_rPVRStatus.u1DmxPid);
        DMX_MUL_FreePidx(_rPVRStatus.u1DmxPid);
        DMX_MUL_FreeInst(_rPVRStatus.u1DmxId);
        _rPVRStatus.u1DmxId = 0xFF;
        _rPVRStatus.u1DmxPid = 0xFF;
    }

    FBM_Free(_prDmxFbmPool);
    _prDmxFbmPool = NULL;
    
    return MTR_OK;
}

static MT_RESULT_T _MTPVR_SetFrameSize(unsigned long arg)
{
    UCHAR ucPvrId;
    UINT32 u4FrameSize = 0;
    UINT32 u4TryCnt = 0;
    INT32 i4Ret;
    SIZE_T zMsgSize = sizeof(UINT32);
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
  
    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    u4FrameSize = (UINT32)rArg.ai4Arg[1];

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if (!_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s not started\n", __FUNCTION__);
        return MTR_OK;
    }

    i4Ret = x_msg_q_send(_rPVRStatus.hPVRPlayFrameSizeQ, &u4FrameSize, zMsgSize, 255);
    
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(100);
        LOG(2, "PVR Frame Size Q(%d) Queue, OSR_TOO_MANY\n", u4FrameSize);
        i4Ret = x_msg_q_send(_rPVRStatus.hPVRPlayFrameSizeQ, &u4FrameSize, zMsgSize, 255);
        u4TryCnt ++;
        if(u4TryCnt > 50)
        {
            LOG(0, "PVR Frame Size(%d) Queue, OSR_TOO_MANY -> exit\n", u4FrameSize);
            break;
        }
    }

    VERIFY(i4Ret == OSR_OK);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Stop demux
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_Stop(unsigned long arg)
{
    UCHAR ucPvrId;
    INT32 i4Ret;
    
    ucPvrId = (UCHAR)arg;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if (!_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s not started\n", __FUNCTION__);
        return MTR_OK;
    }

    LOG (1, "%s\n", __FUNCTION__);

    if (!_fgContainTimeStamp)
    {
        #if 0
        _MTAUD_SetAvSynMode (MT_AUD_DEC_MAIN, MTAV_SYNC_MASTER);
        _MTAUD_SetSoureFrom(MT_AUD_DEC_MAIN, MTAUDDEC_STREAM_FROM_NUM);
        #endif
        
        if(_rMtPvrMpgInfo.fgAudioMaster)
        {
            VDEC_PlayMM(ucPvrId, FALSE);
            if (ePVR_HAS_STREAM == _rPVRStatus.ePVRVideoStatus)
            {
               // UNUSED(VDP_TriggerAudReceiveEx(ucPvrId, FALSE, _rMtPvrMpgInfo.u4FirstAudioPTS, NULL));
               //UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, &rCbInfo));
               
#if defined(CC_MT5890) || defined(CC_MT5882)
                UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS));
#else
               UNUSED(VDP_TriggerAudReceiveEx(_rPVRStatus.u1B2rId, TRUE, _rMtPvrMpgInfo.u4FirstAudioPTS, NULL));
#endif
               UNUSED(VDP_SetPendingForStc(ucPvrId, FALSE));
            }
        }

        if (!DMX_MUL_ForceToFreePCR(FALSE))
        {
             LOG(0, "%s(%d): DMX_MUL_ForceToFreeVdec0 fail\n", __FUNCTION__, __LINE__);
        }
    }

    if (_fgContainTimeStamp)
    {
         // clear lock
        x_sema_lock(_hStopSema, X_SEMA_OPTION_NOWAIT);
    }

    _rPVRStatus.fgStarted = FALSE;
    _rPVRStatus.fgSpeedChange = FALSE;
    _rPVRStatus.i4Speed = 100;
    _rPVRStatus.i4SpeedNew = 100;
    _rPVRStatus.u1AudId = MT_AUD_DEC_AUX;
    _rPVRStatus.u1StcId = STC_SRC_A1;
    _rPVRStatus.u1B2rId = 0;
    _rPVRStatus.ucTsIdx = 2;
    _rPVRStatus.ePVRVideoStatus = ePVR_STREAM_MAX;
    _rPVRStatus.ePVRAudioStatus = ePVR_STREAM_MAX;
    _fgEOS = FALSE;
    _MTPVR_ResetMpgInfo(PVR0);
    
    _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_STOP, 255);
        
    if (_fgContainTimeStamp)
    {
        // wait ctrl done
        i4Ret = x_sema_lock_timeout(_hStopSema, 5000);
    
        if (i4Ret != OSR_OK)
        {
            printf("_MTPVR stop timeout\n");
        }
    }
    
    return MTR_OK;
}

MT_RESULT_T _MTPVR_GetAvailSize(UCHAR ucPvrId, UINT32* pu4Addr, UINT32* pu4Size)
{
    if ((!pu4Size) || (!pu4Addr))
    {
        MTPVR_PRINT("%s parameter error\n", __FUNCTION__);
        return MTR_NOT_OK;
    }
    
    if ((_rPVRStatus.u4PvrBufSize - _rPVRStatus.u4QueuedSize) > _u4ReservedBufferSize)
    {
        *pu4Addr = _rPVRStatus.u4WriteAddr;

        #if 0
        if((_rPVRStatus.u4EndAddr - _rPVRStatus.u4WriteAddr) <= _u4InputSize)
        {
            *pu4Size = (_rPVRStatus.u4EndAddr - _rPVRStatus.u4WriteAddr);
        }
        else
        #endif
        if ((_rPVRStatus.u4PvrBufSize - _rPVRStatus.u4QueuedSize) >= _u4InputSize)
        {
            *pu4Size = _u4InputSize;
        }
        else
        {
            return MTR_BUFFER_FULL;
        }
    }
    else
    {
        return MTR_BUFFER_FULL;
    }
    
    return MTR_OK;
}

MT_RESULT_T _MTPVR_GetPvrBufferStatus(void)
{
    // This part is modified by LGE ( hwachin.lee ) Please don't remove it
    // To prevent picture data loss we need sufficient buffer margin
    if ((_rPVRStatus.u4PvrBufSize - _rPVRStatus.u4QueuedSize) > _u4ReservedBufferSize + MAX_PIC_SIZE )
    {
        if ((_rPVRStatus.u4PvrBufSize - _rPVRStatus.u4QueuedSize) >= _u4InputSize)
        {
        }
        else
        {
            return MTR_BUFFER_FULL;
        }
    }
    else
    {
        return MTR_BUFFER_FULL;
    }
    
    return MTR_OK;

}

//-----------------------------------------------------------------------------
/*
 *  Get buffer
 *
 *  @param  pu4Addr         Return buffer size
 *  @param  pu4Addr         Return allocated buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_GetBuffer(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_NOT_OK;
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucPvrId;
    UINT32 *pu4Size;
    UINT32 *pu4Addr = NULL;
    UINT32 u4Size;
    UINT32 u4Addr;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    pu4Addr = (UINT32*)rArg.ai4Arg[1];
    pu4Size = (UINT32*)rArg.ai4Arg[2];

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if ((!pu4Size) || (!pu4Addr))
    {
        MTPVR_PRINT("%s pu4Size or pu4Addr null\n", __FUNCTION__);
        return MTR_NOT_OK;
    }

    u4Size = 0;
    u4Addr = 0;
    eRet = _MTPVR_GetAvailSize(ucPvrId, &u4Addr, &u4Size);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
    COPY_TO_USER_ARG(pu4Size, u4Size, UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
    COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);

    if (eRet == MTR_OK)
    {
        LOG(7, "GetBuf, u4Size(0x%06x) u4Addr(0x%08x)\n",
            u4Size, u4Addr);                    
    }
    else
    {
        LOG(7, "GetBuf Failed....\n");                    
    }

    return eRet;
}

//-----------------------------------------------------------------------------
/*
 *  Get Secure buffer
 *
 *  @param  pu4Addr         Return buffer size
 *  @param  pu4Addr         Return allocated buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_GetSecureBuffer(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_NOT_OK;
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucPvrId;
    UINT32 *pu4Size;
    UINT32 *pu4Addr = NULL;
    UINT32 u4Size = 0;
    UINT32 u4Addr;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    pu4Addr = (UINT32*)rArg.ai4Arg[1];
    pu4Size = (UINT32*)rArg.ai4Arg[2];

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if ((!pu4Size) || (!pu4Addr))
    {
        MTPVR_PRINT("%s pu4Size or pu4Addr null\n", __FUNCTION__);
        return MTR_NOT_OK;
    }

    if ((_rPVRStatus.u4PvrBufSize - _rPVRStatus.u4QueuedSize) > 0)
    {
        u4Addr  = _rPVRStatus.u4WriteAddr;
        u4Size = KERNEL_BUFFER_SIZE;
    }
    else
    {
        return MTR_BUFFER_FULL;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
    COPY_TO_USER_ARG(pu4Size, u4Size, UINT32); //get buffer size is 288*1024
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
    COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);

    if (eRet == MTR_OK)
    {
        LOG(7, "GetBuf, u4Size(0x%06x) u4Addr(0x%08x)\n",
            KERNEL_BUFFER_SIZE, _rPVRStatus.u4WriteAddr);                    
    }
    else
    {
        LOG(1, "_MTPVR_GetSecureBuffer GetBuf Failed....\n");                    
    }

    return eRet;
}

//-----------------------------------------------------------------------------
/*
 *  Put buffer
 *
 *  @param  u4PutSize          Buffer size
 *  @param  pu4PutAddr         Buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_PutBuffer(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucPvrId;
    UINT32 u4Size;
    UINT32 u4Addr;
    UINT32 u4PutSize;
    UINT32 u4PutAddr;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);


    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    u4PutAddr = (UINT32)rArg.ai4Arg[1];
    u4PutSize = (UINT32)rArg.ai4Arg[2];

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }

    u4Addr = 0;
    eRet = _MTPVR_GetAvailSize(ucPvrId, &u4Addr, &u4Size);
    if (eRet == MTR_OK)
    {
        if (((UINT32)u4PutAddr) != u4Addr)
        {
            MTPVR_PRINT("%s u4PutAddr(%d) != u4Addr(%d)\n",
                __FUNCTION__, u4PutAddr, u4Addr);
            return MTR_NOT_OK;
        }
        
        if (u4PutSize > _u4InputSize)
        {
            MTPVR_PRINT("%s u4PutSize(%d) > buffer size(%d)\n",
                __FUNCTION__, u4PutSize, _u4InputSize);
            ASSERT(0);
            return MTR_NOT_OK;
        }
        
        if (((UINT64)_rPVRStatus.u4WriteAddr + (UINT64)u4PutSize) >= (UINT64)0xFFFFFFFF)
        {
            MTPVR_PRINT("%s address incorrect(%d)\n",
                __FUNCTION__, _rPVRStatus.u4WriteAddr);
            ASSERT(0);
            return MTR_NOT_OK;
        }
        
        if (u4PutSize > u4Size)
        {
            MTPVR_PRINT("%s u4PutSize(%d) > u4Size(%d)n",
                __FUNCTION__, u4PutSize, u4Size);
            return MTR_NOT_OK;
        }
    }
    else
    {
        MTPVR_PRINT("%s _MTPVR_GetAvailSize failn",
            __FUNCTION__);
        return MTR_NOT_OK;
    }
    
    HalFlushInvalidateDCacheMultipleLine((UINT32)_rPVRStatus.u4PvrBaseAddr+u4Addr, u4PutSize);
    if ((_rPVRStatus.u4WriteAddr + u4PutSize) >= _rPVRStatus.u4EndAddr)
    {
        x_memcpy((VOID*)(_rPVRStatus.u4StartAddr + _rPVRStatus.u4PvrBaseAddr),
                 (VOID*)(_rPVRStatus.u4EndAddr + _rPVRStatus.u4PvrBaseAddr),
                 ((_rPVRStatus.u4WriteAddr + u4PutSize) - _rPVRStatus.u4EndAddr));
        HalFlushInvalidateDCacheMultipleLine((UINT32)_rPVRStatus.u4PvrBaseAddr+_rPVRStatus.u4StartAddr,  ((_rPVRStatus.u4WriteAddr + u4PutSize) - _rPVRStatus.u4EndAddr));
     }
     
/*
    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        // change each pid index to PVR
        DMX_PID_T rPid;
        DMX_GetPid(i, DMX_PID_FLAG_VALID, &rPid);
        if (rPid.fgEnable == TRUE)
        {
            rPid.eInputType = DMX_IN_PLAYBACK_TS;
            rPid.u1TsIndex = 2;
            DMX_SetPid(i, DMX_PID_FLAG_INPUTTYPE | DMX_PID_FLAG_TS_INDEX, &rPid);
        }
    }
*/
    
    if (!_rPVRStatus.fgH264ESMode)
    {
        VDEC_SetMMParam(_rPVRStatus.ucEsId, VDEC_MM_CONTAINER_TYPE, SWDMX_FMT_MPEG2_TS_TIME_SHIFT, 0, 0);
    }

#if 0 // check for error data
    {
        UINT32 u4Value;
        static UINT32 u4PreValue = 0;
        UINT32 u4Parsedsize = 0;
        UINT32 u4Pos = _rPVRStatus.u4WriteAddr;
        static UINT32 u4FilePos = 0;
        UCHAR *pUploadBuffer = (UCHAR*)_rPVRStatus.u4PvrBaseAddr;
        while (u4Parsedsize < u4PutSize)
        {
            if (u4Pos >= _rPVRStatus.u4PvrBufSize)
            {
                 u4Pos -= _rPVRStatus.u4PvrBufSize;
            }
            u4Value = pUploadBuffer[u4Pos];
            u4Value <<= 8;
            u4Value += pUploadBuffer[u4Pos+1];
            u4Value <<= 8;
            u4Value += pUploadBuffer[u4Pos+2];
            u4Value <<= 8;
            u4Value += pUploadBuffer[u4Pos+3];
            if ((u4Value - u4PreValue) > 0x10000)
            {
                while (1)
                {
                    if (!u4PreValue)
                        break;
                    Printf("%s Cur(0x%x) Pre(0x%x) u4FilePos(%d)\n", __FUNCTION__, u4Value, u4PreValue, u4FilePos);
                    x_thread_delay(1000);//(1000000);
                    UNUSED(1000);
                }
            }
            u4PreValue = u4Value;
            u4Parsedsize += 192;
            u4FilePos += 192;
            u4Pos += 192;
        }
        Printf("%s Put byte Cur(0x%x) Pre(0x%x) u4FilePos(%d) u4PutSize(%d)\n",  __FUNCTION__, u4Value, u4PreValue, u4FilePos, u4PutSize);
    }
#endif
        
    _rPVRStatus.u4WriteAddr += u4PutSize;
    if (_rPVRStatus.u4WriteAddr >= _rPVRStatus.u4EndAddr)
    {
        _rPVRStatus.u4WriteAddr -= (_rPVRStatus.u4EndAddr - _rPVRStatus.u4StartAddr);
    }

    _MTPVR_Lock();
    _rPVRStatus.u4QueuedSize += u4PutSize;
    _MTPVR_Unlock();
    
    if (_rPVRStatus.fgFirst)
    {
        //LOG(3, "%s(L:%d) PutBuffer\n", __FUNCTION__, __LINE__);
    }
    #if 0
    else if(_rPVRStatus.u4WriteAddr > _rPVRStatus.u4EndAddr)
    {
        MTPVR_PRINT("%s u4WriteAddr(%d) > u4EndAddr(%d)\n",
            __FUNCTION__, _rPVRStatus.u4WriteAddr, _rPVRStatus.u4EndAddr);
        ASSERT(0);
        return MTR_NOT_OK;
    }
    #endif

    LOG(7, "PutBuf, Size(0x%06x) Addr(0x%08x) Q(0x%06x) R(0x%08x) W(0x%08x)\n",
            u4PutSize, u4Addr, _rPVRStatus.u4QueuedSize,
            _rPVRStatus.u4ReadAddr, _rPVRStatus.u4WriteAddr);

    return eRet;
}

//-----------------------------------------------------------------------------
/*
 *  Get Secure buffer
 *
 *  @param  pu4Addr         Return buffer size
 *  @param  pu4Addr         Return allocated buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_PutSecureBuffer(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_NOT_OK;
    MTAL_IOCTL_3ARG_T rArg;
    UCHAR ucPvrId;
    UINT32 u4Size;
    UINT32 u4Addr;    
    static UCHAR  *pu4KernalAddr = NULL;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    u4Addr = (UINT32)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    
    if ((!u4Size) || (!u4Addr))
    {
        MTPVR_PRINT("%s u4Size or u4Addr null\n", __FUNCTION__);
        return MTR_NOT_OK;
    }

    if(pu4KernalAddr == NULL)  //only entry once
    {
        pu4KernalAddr = (UCHAR *)x_mem_alloc(KERNEL_BUFFER_SIZE);
    }

    if(u4Size > KERNEL_BUFFER_SIZE)
    {
        //ASSERT(u4Size <= KERNEL_BUFFER_SIZE);
        LOG(0, "%s u4Size(%d) is bigger than max\n", __FUNCTION__, (int)u4Size);
        return MTR_NOT_OK;
    }

    if(copy_from_user((void*)pu4KernalAddr, (void*)u4Addr, u4Size));
    {
        LOG(0, "_MUXER_SetSI: copy_from_user() failed\n");
        return FALSE;
    }

#if 0
    if(_rPVRStatus.u4WriteAddr + u4Size <= _rPVRStatus.u4EndAddr)
    {
        memcpy_n2n_chk((void *)(_rPVRStatus.u4WriteAddr + _rPVRStatus.u4PvrBaseAddr),(void *)pu4KernalAddr, u4Size, NULL);
    }
    else
    {
        memcpy_n2n_chk((void *)(_rPVRStatus.u4WriteAddr + _rPVRStatus.u4PvrBaseAddr),
            (void *)pu4KernalAddr, (_rPVRStatus.u4EndAddr - _rPVRStatus.u4WriteAddr) , NULL);   
        memcpy_n2n_chk((void *)(_rPVRStatus.u4PvrBaseAddr),
            (void *)pu4KernalAddr + (_rPVRStatus.u4EndAddr - _rPVRStatus.u4WriteAddr), 
            u4Size - (_rPVRStatus.u4EndAddr - _rPVRStatus.u4WriteAddr), NULL);
    }
#endif
    _rPVRStatus.u4WriteAddr = _rPVRStatus.u4WriteAddr + u4Size;

    if(_rPVRStatus.u4WriteAddr > _rPVRStatus.u4EndAddr)
    {
        _rPVRStatus.u4WriteAddr = _rPVRStatus.u4WriteAddr - _rPVRStatus.u4EndAddr;
    }
    
    _rPVRStatus.u4QueuedSize += u4Size;

    return eRet;
}

MT_RESULT_T _MTPVR_ChangeThumbVdecId(UCHAR ucVdecId)
{
    UINT32 u4WorkingBufAddr;
    UINT32 u4WorkingBufSize;
    VDEC_DEC_NFY_INFO_T rNfyInfo;
    
    if (_fgThumbnail)
    {
        // stop
        x_memset(&rNfyInfo, 0, sizeof(VDEC_DEC_NFY_INFO_T));
        _MTVDEC_Notify_Lock();
        _u4NotifyMask &= ~MTVDEC_NOTIFY_THUMBNAIL;
        _MTVDEC_Notify_Unlock();
        VDEC_StopVideoThumbnail2(_ucThumbVdecId);

        if(ucVdecId < VDEC_MAX_ES)
        {
            _MTPVR_StopTimer(PVR0);
            // change vdec id
            _ucThumbVdecId = ucVdecId;
        
            // start
            u4WorkingBufAddr = (_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBWORKOFST);
            u4WorkingBufSize = MTPVR_THUMBWORKSIZE;
            rNfyInfo.pvTag = (void*)&_aucMTVDECEsId[_ucThumbVdecId];
            rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;
            _MTVDEC_Notify_Lock();
            _u4NotifyMask |= MTVDEC_NOTIFY_THUMBNAIL;
            _MTVDEC_Notify_Unlock();
            VDEC_SetDecNfy(_ucThumbVdecId, &rNfyInfo);
            VDEC_GetVideoThumbnail2(_ucThumbVdecId, &_rThumbDstInfo,
                u4WorkingBufAddr, u4WorkingBufSize, FALSE);
            _MTPVR_StartTimer(PVR0, _ucThumbVdecId);
        }
        else
        {
            MTPVR_PRINT("%s ucVdecId(%d) >= VDEC_MAX_ES\n", __FUNCTION__, ucVdecId);
        }
    }
    
    if (ucVdecId < VDEC_MAX_ES)
    {
        _ucThumbVdecId = ucVdecId;
    }
    else
    {
        MTPVR_PRINT("%s ucVdecId(%d) >= VDEC_MAX_ES\n", __FUNCTION__, ucVdecId);
        LOG(1, "%s ucVdecId(%d) >= VDEC_MAX_ES\n", __FUNCTION__, ucVdecId);
    }
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get thumbnail
 *
 *  @param  u4PutSize          Buffer size
 *  @param  pu4PutAddr         Buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_GetThumbnail(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_6ARG_T    rArg;
    UCHAR ucPvrId;
    UCHAR *pDstBuffer;
    //UINT32 u4Size;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4Pitch;
    UINT32 u4AddrOffset;
    UINT32 u4WorkingBufAddr;
    UINT32 u4WorkingBufSize;
    UINT32 u4Line, u4Hpos;
    VDEC_DEC_NFY_INFO_T rNfyInfo;
    //VDP_CAPTURE_INTO_T rCapInfo;
    
    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    _u4ThumbnailId ++;

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    u4Width = (UINT32)rArg.ai4Arg[1];
    u4Height = (UINT32)rArg.ai4Arg[2];
    u4Pitch = (UINT32)rArg.ai4Arg[3];
    u4AddrOffset = (UINT32)rArg.ai4Arg[4];

    LOG(5, "%s(L:%d)\n", __FUNCTION__, __LINE__);
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }

    if ((u4Width == 0) || (u4Height == 0) || (u4Pitch == 0))
    {
        MTPVR_PRINT("%s width/height/pitch = 0\n", __FUNCTION__);
        return MTR_NOT_OK;
    }
    
    if (u4AddrOffset != MTPVR_THUMBOUTOFST)
    {
        MTPVR_PRINT("%s (u4AddrOffset != MTPVR_THUMBOUTOFST)\n", __FUNCTION__);
        return MTR_NOT_OK;
    }
    
    if ((u4Pitch*u4Height*4) > MTPVR_THUMBOUTSIZE)
    {
        MTPVR_PRINT("%s out buffer size not enought, u4Pitch(%d) u4Height(%d)\n",
            __FUNCTION__, u4Pitch, u4Height);
        return MTR_NOT_OK;
    }
    
    u4WorkingBufAddr = (_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBWORKOFST);
    u4WorkingBufSize = MTPVR_THUMBWORKSIZE;
    if ((u4Pitch*u4Height*4) > MTPVR_THUMBWORKSIZE)
    {
        MTPVR_PRINT("%s working buffer size not enought, u4Pitch(%d) u4Height(%d)\n",
            __FUNCTION__, u4Pitch, u4Height);
        return MTR_NOT_OK;
    }

    {
        x_memset((VOID*)VIRTUAL(_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBOUTOFST),
            0, MTPVR_THUMBOUTSIZE);
        x_memset(&_rThumbDstInfo, 0, sizeof(VDP_THUMBNAIL_INFO_T));
        _rThumbDstInfo.eCanvasColormode = VDP_COLORMODE_ARGB_D8888;
        _rThumbDstInfo.pucCanvasBuffer = (UCHAR*)(_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBOUTOFST);
        _rThumbDstInfo.u4CanvasHeight = u4Height;
        _rThumbDstInfo.u4CanvasPitch = u4Pitch * 4;
        _rThumbDstInfo.u4CanvasWidth = u4Width * 4;
        _rThumbDstInfo.u4ThumbnailHeight = u4Height;
        _rThumbDstInfo.u4ThumbnailWidth = u4Width;
        _rThumbDstInfo.u4ThumbnailX = 0; // offset
        _rThumbDstInfo.u4ThumbnailY = 0;

        // clean thumbanil
        pDstBuffer = _rThumbDstInfo.pucCanvasBuffer;
        for (u4Line=0;u4Line<_rThumbDstInfo.u4ThumbnailHeight;u4Line++)
        {
            for(u4Hpos=0;u4Hpos<_rThumbDstInfo.u4ThumbnailWidth;u4Hpos++)
            {
                *(((UINT32 *)((VOID*)VIRTUAL((UINT32)pDstBuffer)+0))+u4Hpos)=0xff000000;
            }
            pDstBuffer+=_rThumbDstInfo.u4CanvasPitch;
        }
        
        _MTPVR_StopTimer(ucPvrId);
        
        _fgThumbnail = TRUE;
        rNfyInfo.pvTag = (void*)&_ucThumbVdecId;
        rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;
        _MTVDEC_Notify_Lock();
        _u4NotifyMask |= MTVDEC_NOTIFY_THUMBNAIL;
        _MTVDEC_Notify_Unlock();
        VDEC_SetDecNfy(_ucThumbVdecId, &rNfyInfo);
        VDEC_GetVideoThumbnail2(_ucThumbVdecId, &_rThumbDstInfo,
            u4WorkingBufAddr, u4WorkingBufSize, FALSE);
        
        _MTPVR_StartTimer(ucPvrId, _ucThumbVdecId);
    }

    MTPVR_PRINT(" - Wait VDEC_GetVideoThumbnail2\n");
    LOG(5, "%s(L:%d) return(MTR_OK)\n", __FUNCTION__, __LINE__);

    //MTPVR_PRINT(" - u4Size = 0x%lx, u4Addr = 0x%lx\n", u4Size, u4Addr);
    return eRet;
}

//-----------------------------------------------------------------------------
/*
 *  Stop thumbnail
 *
 *  @param  u4PutSize          Buffer size
 *  @param  pu4PutAddr         Buffer address (Kernel address)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_StopThumbnail(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucPvrId;
    //UINT32 u4Size;
    //UINT32 u4Addr;
    //UINT32 u4PutSize;
    //UINT32 *pu4PutAddr = NULL;
    //VDEC_DEC_NFY_INFO_T rNfyInfo;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    LOG(5, "%s(L:%d)\n", __FUNCTION__, __LINE__);
    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    if(ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        LOG(3, "%s(L:%d) (ucPvrId(%d) >= PVR_MAX_ID)\n", __FUNCTION__, __LINE__, ucPvrId);
        return MTR_NOT_OK;
    }

    _MTPVR_StopTimer(ucPvrId);
        
    _MTVDEC_Notify_Lock();
    _u4NotifyMask &= ~MTVDEC_NOTIFY_THUMBNAIL;
    _MTVDEC_Notify_Unlock();
    VDEC_StopVideoThumbnail2(_ucThumbVdecId);
    if(_fgThumbnail)
    {
        _MTPVR_VDEC_Nfy((void*)&_ucThumbVdecId, VDEC_COND_THUMBNAIL_FAIL, 0, 0);
        _fgThumbnail = FALSE;
    }

    LOG(5, "%s(L:%d) return(MTR_OK)\n", __FUNCTION__, __LINE__);
    //MTPVR_PRINT(" - u4Size = 0x%lx, u4Addr = 0x%lx\n", u4Size, u4Addr);
    return eRet;
}


MT_RESULT_T _MTPVR_SetSpeedExt(UCHAR ucPvrId, INT32 i4Speed, UINT32 u4StepCmd)
{
    MT_RESULT_T eRet = MTR_OK;
    UINT16 u2QueueSize;
    UINT16 u2MaxQueueSize;
    //MTAL_IOCTL_3ARG_T    rArg;
    //DMX_PVR_PLAY_T rPvrPlay;

    ucPvrId = (UCHAR)ucPvrId;
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }

    if (u4StepCmd == (UINT32)ePVR_STEP_PREPARE)
    {
        LOG(5, "ePVR_STEP_PREPARE, stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
        _rPVRStatus.ePVRStep = ePVR_STEP_PREPARE;
    }
    else if (u4StepCmd == (UINT32)ePVR_STEP_START)     // step start
    {
        LOG(5, "ePVR_STEP_START, stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));

        // check the vdec Q size.
        u2QueueSize = 0;
        u2MaxQueueSize = 0;
        VDEC_GetQueueInfo(_rPVRStatus.ucEsId, &u2QueueSize, &u2MaxQueueSize);

        // picture in q is not enough, sending speed adjust to normal play
        if (u2QueueSize < 10)
        {
            if(100 != _rPVRStatus.i4Speed)
            {
                _rPVRStatus.i4SpeedNew = 100;
                _rPVRStatus.fgSpeedChange = TRUE;
            }
        }
        else
        {
            if(25 != _rPVRStatus.i4Speed)
            {
                _rPVRStatus.i4SpeedNew = 25;
                _rPVRStatus.fgSpeedChange = TRUE;
            }            
        }
        
        if (_rPVRStatus.ePVRStep == ePVR_STEP_PREPARE && _rPVRStatus.ePVRStatus != ePVR_STATE_PLAY)
        {
            _rPVRStatus.ePVRStep = ePVR_STEP_START;
            _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_PLAY, 255);
        }
    }
    else if (u4StepCmd == (UINT32)ePVR_STEP_DONE) // step done
    {
        if (_rPVRStatus.ePVRStep != ePVR_STEP_START && _rPVRStatus.ePVRStep != ePVR_STEP_PREPARE)
        {
            // do nothing
            return eRet;
        }
        LOG(5, "ePVR_STEP_DONE, stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
        _rPVRStatus.ePVRStep = ePVR_STEP_DONE;
        _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_PAUSE, 254);
        // higher priority, so AP speed would be handle later.
        // our final speed would be the one AP set.
    }
    else if (u4StepCmd == (UINT32)ePVR_STEP_END)  // step end
    {
        LOG(5, "ePVR_STEP_END, stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
        _rPVRStatus.ePVRStep = ePVR_STEP_DONTCARE;
        /*
            _rPVRStatus.fgSpeedChange = TRUE;
            _rPVRStatus.i4SpeedNew = 100;
        */
        _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_PLAY, 254);
        // higher priority, so AP speed would be handle later.
        // our final speed would be the one AP set.
    }
    else if ((i4Speed == 0) || (i4Speed == 1)) // 0 pause, 1 step
    {
        LOG(5, "speed 0 or 1, stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
        _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_PAUSE, 255);
    }
    else if((_rPVRStatus.ePVRStatus == ePVR_STATE_PAUSE) ||
            (_rPVRStatus.ePVRStatus == ePVR_STATE_PLAY))
    {
        LOG(5, "play, stc(0x%x)\n", STC_GetStcValue(_rPVRStatus.u1StcId));
        
        if (_rPVRStatus.i4Speed != i4Speed)
        {
            LOG(3, "========== Play, i4Speed = %d\n", i4Speed);
            if (200==_rPVRStatus.i4Speed && 100==i4Speed)
            {
                LOG(2, "Re-set audio PTS. Due to video seamless play from 2X->1X\n");
                VDEC_SetRestartAudioPts(_rPVRStatus.ucEsId, TRUE);
            }
            
            _rPVRStatus.fgSpeedChange = TRUE;
            _rPVRStatus.i4SpeedNew = i4Speed;
        }
        
        _MTPVR_SendPlayCmd(ucPvrId, ePVR_STATE_PLAY, 255);
    }
    else
    {
        LOG(5, "speed change(%d), stc(0x%x)\n", i4Speed, STC_GetStcValue(_rPVRStatus.u1StcId));
        _rPVRStatus.fgSpeedChange = TRUE;
        _rPVRStatus.i4SpeedNew = i4Speed;
    }

    return eRet;
}

static MT_RESULT_T _MTPVR_ExtractThumbnail(unsigned long arg)
{
    MT_RESULT_T eRetvl = MTR_OK;
    UCHAR   ucPvrId = PVR0;
    UCHAR   *pDstBuffer;
    UINT32  u4WorkingBufAddr, u4WorkingBufSize;
    UINT32  u4Width, u4Height, u4Pitch, u4Line, u4Hpos;
    VDEC_DEC_NFY_INFO_T     rNfyInfo;
    MTAL_IOCTL_3ARG_T       rArg;
    MTPVR_THUMBNAIL_PARAM_T rParamInfo;
    MTPVR_THUMBNAIL_PARAM_T *prParamInfo;
    MTPVR_THUMBNAIL_ARGB_T  rThumData;
    MTPVR_THUMBNAIL_ARGB_T  *pThumData;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    prParamInfo = (MTPVR_THUMBNAIL_PARAM_T *)rArg.ai4Arg[1];
    pThumData = (MTPVR_THUMBNAIL_ARGB_T *)rArg.ai4Arg[2];

    if ((!prParamInfo) || (!pThumData))
    {
        MTPVR_PRINT("%s:%u Param Error.\n", __FUNCTION__, __LINE__);
        return MTR_ERR_INV;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prParamInfo, MTPVR_THUMBNAIL_PARAM_T);
    COPY_FROM_USER_ARG(prParamInfo, rParamInfo, MTPVR_THUMBNAIL_PARAM_T);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pThumData, MTPVR_THUMBNAIL_ARGB_T);
    COPY_FROM_USER_ARG(pThumData, rThumData, MTPVR_THUMBNAIL_ARGB_T);

    if ((rThumData.u4Height==0) || (rThumData.u4Width==0) || (rThumData.pucAddr==NULL))
    {
        MTPVR_PRINT("%s:%u Param Error.\n", __FUNCTION__, __LINE__);
        return MTR_ERR_INV;
    }

    u4Width = rThumData.u4Width;
    u4Height= rThumData.u4Height;
    u4Pitch = ((u4Width+0xF)&(~0xF));
    if ((u4Pitch*u4Height*4) > MTPVR_THUMBWORKSIZE)
    {
        MTPVR_PRINT("%s working buffer size not enought, u4Pitch(%d) u4Height(%d)\n",
            __FUNCTION__, u4Pitch, u4Height);
        return MTR_NOT_OK;
    }

    u4WorkingBufAddr = (_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBWORKOFST);
    u4WorkingBufSize = MTPVR_THUMBWORKSIZE;

    x_memset((VOID*)VIRTUAL(_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBOUTOFST),  0, MTPVR_THUMBOUTSIZE);
    x_memset(&_rThumbDstInfo, 0, sizeof(VDP_THUMBNAIL_INFO_T));
    _rThumbDstInfo.eCanvasColormode = VDP_COLORMODE_ARGB_D8888;
    _rThumbDstInfo.pucCanvasBuffer = (UCHAR*)(_rPVRStatus.u4FeederAddrPhy + MTPVR_THUMBOUTOFST);
    _rThumbDstInfo.u4CanvasHeight = u4Height;
    _rThumbDstInfo.u4CanvasPitch = u4Pitch * 4;
    _rThumbDstInfo.u4CanvasWidth = u4Width * 4;
    _rThumbDstInfo.u4ThumbnailHeight = u4Height;
    _rThumbDstInfo.u4ThumbnailWidth = u4Width;
    _rThumbDstInfo.u4ThumbnailX = 0;                    // offset
    _rThumbDstInfo.u4ThumbnailY = 0;

    // clean thumbanil
    pDstBuffer = _rThumbDstInfo.pucCanvasBuffer;
    
    // need check.
    for (u4Line=0; u4Line<_rThumbDstInfo.u4ThumbnailHeight; u4Line++)
    {
        for (u4Hpos=0; u4Hpos<_rThumbDstInfo.u4ThumbnailWidth; u4Hpos++)
        {
            *(((UINT32 *)((VOID*)VIRTUAL((UINT32)pDstBuffer)+0))+u4Hpos) = 0xFF000000;
        }
        pDstBuffer += _rThumbDstInfo.u4CanvasPitch;
    }
    
    _fgThumbnail = TRUE;
    _fgThumbnail2 = TRUE;
    _ucExtractThumbVdecId = prParamInfo->ucVdecId;

    _MTPVR_StopTimer(ucPvrId);
    
    rNfyInfo.pvTag = (void*)&_ucExtractThumbVdecId;
    rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;
    _MTVDEC_Notify_Lock();
    _u4NotifyMask |= MTVDEC_NOTIFY_THUMBNAIL;
    _MTVDEC_Notify_Unlock();
    VDEC_SetDecNfy(_ucExtractThumbVdecId, &rNfyInfo);
    VDEC_GetVideoThumbnail2(_ucExtractThumbVdecId, &_rThumbDstInfo,
        u4WorkingBufAddr, u4WorkingBufSize, FALSE);
    _MTPVR_StartTimer(ucPvrId, _ucExtractThumbVdecId);
    
    return eRetvl;
}

static MT_RESULT_T _MTPVR_StopExtractThumbnail(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    UCHAR ucPvrId;

    ucPvrId = (UCHAR)arg;
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }

    _MTPVR_StopTimer(ucPvrId);
        
    _MTVDEC_Notify_Lock();
    _u4NotifyMask &= ~MTVDEC_NOTIFY_THUMBNAIL;
    _MTVDEC_Notify_Unlock();
    VDEC_StopVideoThumbnail2(_ucExtractThumbVdecId);
    if (_fgThumbnail)
    {
        _MTPVR_VDEC_Nfy((void*)&_ucExtractThumbVdecId, VDEC_COND_THUMBNAIL_FAIL, 0, 0);
        _fgThumbnail = FALSE;
        _fgThumbnail2 = FALSE;
    }

    _ucExtractThumbVdecId = 0;
    LOG(5, "%s(L:%d) return(MTR_OK)\n", __FUNCTION__, __LINE__);

    return eRet;
}

//-----------------------------------------------------------------------------
/*
 *  Set speed
 *
 *  @param  u4Speed            Speed
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_SetSpeed(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucPvrId;
    INT32 i4Speed;
    INT32 i=0;
    DMX_PID_T rDrvPid;
    //DMX_PVR_PLAY_T rPlay;
    static UINT8 u1StopAudio=0;
    x_memset(&rDrvPid, 0, sizeof(rDrvPid));

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }
    i4Speed = (INT32)rArg.ai4Arg[1];

    _MTAUD_CheckPvrPause(i4Speed);
    if (i4Speed != 100 && u1StopAudio != 1)
    {
        //_MTAUD_Stop(1);    // remove driver control audio stop/play when PVR FF mode, DDI will control it.
        u1StopAudio = 1;
    }
    else if (i4Speed == 100 && u1StopAudio==1)
    {
        for (i=0;i<DMX_NUM_PID_INDEX;i++)
        {
            if (!DMX_GetPid(i, DMX_PID_FLAG_TS_INDEX|DMX_PID_FLAG_BUFFER|DMX_PID_FLAG_VALID, &rDrvPid))
            {
                MTPVR_PRINT("Get Pid ePidtype fail in io_mtpvr %d\n",i);
                return MTR_NOT_OK;
            }
            
            if ((rDrvPid.ePidType==DMX_PID_TYPE_ES_AUDIO) && (rDrvPid.fgEnable==TRUE) && (rDrvPid.u1TsIndex==_rPVRStatus.ucTsIdx))
            {
                LOG(1, "flush audio id = %d", i);
                AUD_SetStartPts(1, (UINT32)(-1));
                DMX_FlushPidBuffer(i);
                break;
            }
        }
        //_MTAUD_Play(1);
        u1StopAudio = 0;
    }

    return _MTPVR_SetSpeedExt(ucPvrId, i4Speed, (UINT32)ePVR_STEP_DONTCARE);
}


MT_RESULT_T _MTPVR_ResetBufferEx(UCHAR ucPvrId)
{
    INT32 i4Ret;
    
    // clear lock
    x_sema_lock(_hResetSema, X_SEMA_OPTION_NOWAIT);
    
    if (!_rPVRStatus.fgH264ESMode)
    {
        _MTPVR_SendPlayCmd(ucPvrId, ePVR_CMD_FLUSH, 255);
    }
    else
    {
        _MTPVR_SendPlayESCmd(ucPvrId, ePVR_CMD_FLUSH, 255);
    }
       
    // wait ctrl done
   i4Ret = x_sema_lock_timeout(_hResetSema, 5000);
    
   if (i4Ret != OSR_OK)
   {
       Printf("_MTPVR stop timeout\n");
   }
    
   if (!_fgContainTimeStamp)
   {
        if(_rMtPvrMpgInfo.fgAudioMaster)
        {
            _MTPVR_ClearAudio();
        }
        
        LOG(1, "%s %d\n", __FUNCTION__, __LINE__);
        _MTPVR_ResetMpgInfo(ucPvrId);
   }
   else
   {
       _MTPVR_ClearAudio();
   }
    
    LOG(1,"Reset Buffer Done\n");

    return MTR_OK;
}



//-----------------------------------------------------------------------------
/*
 *  _MTPVR_ResetBuffer
 *
 *  @param  u4Speed            Speed
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_ResetBuffer(unsigned long arg)
{
    UCHAR ucPvrId;

    ucPvrId = (UCHAR)arg;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }

    if (!_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s not started\n", __FUNCTION__);
        return MTR_OK;
    }

    MTPVR_PRINT("%s\n", __FUNCTION__);

    return _MTPVR_ResetBufferEx(ucPvrId);
}

//-----------------------------------------------------------------------------
/*
 *  _MTPVR_Deliver_EOS
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPVR_Deliver_EOS(unsigned long arg)
{
    UCHAR ucPvrId;

    ucPvrId = (UCHAR)arg;

    if (ucPvrId >= PVR_MAX_ID)
    {
        MTPVR_PRINT("%s ucPvrId(%d) > PVR_MAX_ID\n", __FUNCTION__, ucPvrId);
        return MTR_NOT_OK;
    }

    if (!_rPVRStatus.fgStarted)
    {
        MTPVR_PRINT("%s not started\n", __FUNCTION__);
        return MTR_OK;
    }

    #if 0
    if (_fgContainTimeStamp)
    {
        MTPVR_PRINT("%s 192 TS Do Not need receive EOS.\n", __FUNCTION__);
        return MTR_OK;
    }
    #endif
    
    MTPVR_PRINT("%s\n", __FUNCTION__);

    _fgEOS = TRUE;

    return MTR_OK;
}

static MT_RESULT_T _MTPVR_PutDataChData(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_2ARG_T    rArg;
    UINT32 u4Addr;
    UINT32 u4Size;
    UINT8 u1Inst;
    DMX_MM_DATA_T rMm;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4Addr = (UINT32)rArg.ai4Arg[0];
    u4Size = (UINT32)rArg.ai4Arg[1];

    u1Inst = DMX_MUL_GetAvailableInst(DMX_CONN_TYPE_BUFAGENT_0);
    if (u1Inst == 0xff)
    {
        Printf("Fail to allocate Inst.\n");
        return MTR_NOT_OK;
    }

    if (!DMX_MUL_SetInstType(u1Inst, DMX_IN_PLAYBACK_TS))
    {
        Printf("Fail to set input type.\n");
        return MTR_NOT_OK;
    }

    if (!DMX_SetTSMode(u1Inst, DMX_TSFMT_188))
    {
        Printf("Fail to set ts mode.\n");
        return MTR_NOT_OK;
    }

    x_memset(&rMm, 0, sizeof(rMm));
    rMm.u4BufStart = u4Addr;
    rMm.u4BufEnd = u4Addr + 0x80000;
    rMm.u4StartAddr = u4Addr;
    rMm.u4FrameSize = u4Size;
    rMm.fgFrameHead = TRUE;
    //rMm.fgEOS = TRUE;

    if (!DMX_MUL_MoveData(u1Inst, &rMm))
    {
        Printf("Fail to move data\n");
        return MTR_NOT_OK;
    }

    return eRet;
}

static MT_RESULT_T _MTPVR_SetMode(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    UCHAR ucPvrId;
    MTAL_IOCTL_2ARG_T rArg;
    ENUM_MTPVR_PLAYMODE_T ePlayMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    ePlayMode = (ENUM_MTPVR_PLAYMODE_T)rArg.ai4Arg[1];
    
    if (!_Mtpvr_SetPlayBackMode(ePlayMode))
    {
        eRet = MTR_NOT_OK;
    }

    UNUSED(ucPvrId);
    
    return eRet;
}

static MT_RESULT_T _MTPVR_SetInstID(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_3ARG_T rArg;
    UCHAR ucPvrId;
    UCHAR ucInstId;
    ENUM_MTPVR_SET_INST_ID_T eSetTyp;
    
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucPvrId = (UCHAR)rArg.ai4Arg[0];
    eSetTyp = (ENUM_MTPVR_SET_INST_ID_T)rArg.ai4Arg[1];
    ucInstId = (UCHAR)rArg.ai4Arg[2];

    LOG(3, "%s Type(%d) Id(%d)\n", __FUNCTION__, eSetTyp, ucInstId);
    
    switch (eSetTyp)
    {
    case ENUM_MTPVR_SET_DMX_ID:
        _rPVRStatus.u1DmxId = ucInstId;
        _rPVRStatus.ucTsIdx = DMX_MUL_GetTSIdx(ucInstId);
        break;

    case ENUM_MTPVR_SET_VDEC_ID:
        _rPVRStatus.ucEsId =  ucInstId;
        break;

    case ENUM_MTPVR_SET_AUD_ID:
        _rPVRStatus.u1AudId = ucInstId;
        break;

    case ENUM_MTPVR_SET_STC_ID:
        _rPVRStatus.u1StcId = ucInstId;
        break;

    case ENUM_MTPVR_SET_B2R_ID:
        _rPVRStatus.u1B2rId = ucInstId;
        break;
    case ENUM_MTPVR_SET_TS_IDX:
        _rPVRStatus.ucTsIdx = ucInstId;
        break;
    default:
        LOG(0, "Err: Set type OK.\n");
        eRet = MTR_ERR_INV;
    }
    
    return eRet;
}



//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
int mtal_ioctl_mtpvr(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    INT32 i4Ret = MTR_OK;

    switch (cmd)
    {
    case MTAL_IO_PVR_INIT:
        i4Ret = _MTPVR_Init(arg);
        break;

    case MTAL_IO_SET_SECURE:
        i4Ret = _MTPVR_SetSecure(arg);
        break;

    case MTAL_IO_PVR_RESET_MPG_AUDIO_INFO:
        i4Ret = _MTPVR_ResetMpgAudioInfo(arg);
        break;

    case MTAL_IO_PVR_START:
        i4Ret = _MTPVR_Start(arg);
        break;

    case MTAL_IO_PVR_ES_START:
        i4Ret = _MTPVR_StartPlayES(arg);
        break;

    case MTAL_IO_PVR_STOP:
        i4Ret = _MTPVR_Stop(arg);
        break;

   case MTAL_IO_PVR_ES_STOP:
        i4Ret = _MTPVR_StopESPlay(arg);
        break;
        
    case MTAL_IO_SET_FRAME_SIZE:
        i4Ret = _MTPVR_SetFrameSize(arg);
        break;
            
    case MTAL_IO_PVR_GETBUFFER:
        i4Ret = _MTPVR_GetBuffer(arg);
        break;

    case MTAL_IO_PVR_GET_SECURE_BUFFER:
        i4Ret = _MTPVR_GetSecureBuffer(arg);
        break;

    case MTAL_IO_PVR_PUTBUFFER:
        i4Ret = _MTPVR_PutBuffer(arg);
        break;
        
    case MTAL_IO_PVR_PUT_SECURE_BUFFER:
        i4Ret = _MTPVR_PutSecureBuffer(arg);
        break;

    case MTAL_IO_PVR_GETTHUMBNAIL:
        i4Ret = _MTPVR_GetThumbnail(arg);
        break;
        
    case MTAL_IO_PVR_STOPTHUMBNAIL:
        i4Ret = _MTPVR_StopThumbnail(arg);
        break;
        
    case MTAL_IO_PVR_SETSPEED:
        i4Ret = _MTPVR_SetSpeed(arg);
        break;

    case MTAL_IO_PVR_RESETBUFFER:
        i4Ret = _MTPVR_ResetBuffer(arg);
        break;

    case MTAL_IO_PVR_DELIVER_EOS:
        i4Ret = _MTPVR_Deliver_EOS(arg);
        break;

    case MTAL_IO_PVR_GETBUFFERSTATUS:
        i4Ret = _MTPVR_GetPvrBufferStatus();
        break;

    case MTAL_IO_PVR_PUTDATACHDATA:
        i4Ret = _MTPVR_PutDataChData(arg);
        break;
        
    case MTAL_IO_PVR_EXTRACT_THUMBNAIL:
        i4Ret = _MTPVR_ExtractThumbnail(arg);
        break;

    case MTAL_IO_PVR_STOP_EXTRACT_THUMBNAIL:
        i4Ret = _MTPVR_StopExtractThumbnail(arg);
        break;
        
    case MTAL_IO_PVR_SETMODE:
        i4Ret = _MTPVR_SetMode(arg);
        break;
    
    case MTAL_IO_PVR_SET_INST_ID:
        i4Ret = _MTPVR_SetInstID(arg);
        break;
        
    default:
        break;
    }

    //printf("pvr: 0x%x ret: %d\n", cmd, i4Ret);

    return i4Ret;
}

