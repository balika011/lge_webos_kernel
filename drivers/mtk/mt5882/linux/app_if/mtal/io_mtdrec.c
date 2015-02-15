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
 * $RCSfile: io_mtdmx.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor: wen han $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file io_mttsrec.c
 */

#ifdef TIME_SHIFT_SUPPORT
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "mttsrec.h"
#include "mtdrec.h"
#include "mtpvr.h"
#include "dmx_if.h"
#include "fvr_if.h"
#include "fbm_drvif.h"
#include "mpv_drvif.h"
#include "vdec_drvif.h"
#include "aud_drvif.h"
#include "stc_drvif.h"
#include "cb_data.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>

#include "x_assert.h"
#include "drvcust_if.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTDREC_THREAD_NAME                 "MTDRECTHR"
#define MTDREC_THREAD_STACK_SIZE           2048
#define MTDREC_THREAD_PRIORITY             80

#define MTDREC_MSGQ_NAME                    "MTDRECQ"
#define MTDREC_MSGQ_MSG_COUNT               (FVR_GBL_COUNT * 32)

#define MTDREC_GETSTREAM188_SIZE            (94 * 1024)
#define MTDREC_GETSTREAM192_SIZE            (96 * 1024)
#define MTDREC_GETSTREAM188_LBR_SIZE        (MTDREC_GETSTREAM188_SIZE)
#define MTDREC_GETSTREAM192_LBR_SIZE        (MTDREC_GETSTREAM192_SIZE)
#define MTDREC_GETSTREAM188_HBR_SIZE        (MTDREC_GETSTREAM188_SIZE * 2)
#define MTDREC_GETSTREAM192_HBR_SIZE        (MTDREC_GETSTREAM192_SIZE * 2)
#define MTDREC_GETSTREAM188_ALIGN_SIZE      (MTDREC_GETSTREAM188_SIZE * 8)
#define MTDREC_GETSTREAM192_ALIGN_SIZE      (MTDREC_GETSTREAM192_SIZE * 8)
#define MTDREC_HITRATE_THRESHOLD            (15 * 1024) // 15 * 1024 Kbps

#define MTDREC_PIC_INDEX_ARRAY_SIZE         (1024)
#define MTDREC_STREAM_STRUCT_MAX            (64)    // (768KB * 8)/96KB = 64, we need this size over 64
#define MTDREC_TICK_PIC_COUNT               (1024)

#define MTDREC_H264ES_BUFSIZE               (1536)  // 1.5KB

#define MTDREC_CIPHER_KEY_INDEX             (0)

#define MTDREC_FRAMERATE_SAMPLE_COUNT       (3)

#ifdef CC_DUAL_PVR_SUPPORT
#define MTDREC_NUM_FBM_BUFFER               (2)
#else
#define MTDREC_NUM_FBM_BUFFER               (1)
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT8   u1Idx;
    UINT32  u4Timeout;
} MTDREC_MSG_T;

typedef enum
{
    GETSTREAM_NORET,
    GETSTREAM_TIMEOUT,
    GETSTREAM_DATA_ENOUGH,
    GETSTREAM_FRAME_ENOUGH,
} MTDREC_GETSTREAM_RET_T;

typedef struct
{
    VDEC_AVC_SEI_PIC_STRUCT ePicStruct;
    BOOL fgFrameMbsOnlyFlag;
    UINT32 u4Timescale;
    UINT32 u4NumOfUnit;
} MTDREC_H264_T;

typedef struct
{
    /* picture data */
    UCHAR   ucEsId;
    FVR_VIDEO_TYPE_T eLastVideoPidType;
    UINT32  u4MtDRecPicWp;
    UINT32  u4MtDRecPicRp;
	UINT32  u4PrevPicOffset;
    MTDREC_PVR_INDEX_TYPE_T ePrevIdxType;
    MTDREC_PVR_INDEX_T arMtDRecPicAddr[MTDREC_TICK_PIC_COUNT];
    /* calculate frame rate */
    BOOL    fgInitTime;
    UINT32  u4PicCount;
    HAL_TIME_T rPicStartTime;
    /* stream data */
    UINT32  u4MtDRecDefineBufSize;
    UINT32  u4DRecBaseAddr;
    UINT32  u4DRecBaseSize;
    UINT32  u4DRecStreamAddr;
    UINT32  u4DRecStreamSize;
    UINT32  u4DRecStreamPktCount;
    UINT32  u4DRecIndexBufAddr;
    UINT32  u4DRecIndexBufSize;
    UINT32  u4MtDRecReadPointer;
    UINT32  u4MtDRecWritePointer;
    UINT8   u1MtDRecStreamDataWp;
    UINT8   u1MtDRecStreamDataRp;
    BOOL    fgDRecStreamStop;
    MTDREC_STREAM_INFO_T arMtDRecStreamInfo[MTDREC_STREAM_STRUCT_MAX];
    MTDREC_STREAM_DATA_T arMtDRecStreamData[MTDREC_STREAM_STRUCT_MAX];
} MTDREC_STREAM_T;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

extern MT_RESULT_T _MTPVR_ChangeThumbVdecId(UCHAR ucVdecId);
extern BOOL MTDMX_SetSecondVideoPid(UINT8 u1Pidx, UINT32 u4Flags, MTDMX_PID_T* prPid);
//extern BOOL _Mtpvr_SetPlayBackMode(ENUM_MTPVR_PLAYMODE_T ePlayMode);
//extern void _MtdmxSetDecoder(void);
extern BOOL _MTAL_DMX_IsES1Used(void);

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)       \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,   \
                       (size)))                             \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define COPY_FROM_USER_ARG_SIZE_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 3\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    } \

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 5\n");      \
        return MTR_ERR_INV;                                          \
    } \

#define PICINDEXSIZE(rp, wp, size)      \
    (((rp) <= (wp)) ? ((wp) - (rp)) : (((wp) + (size)) - (rp)))

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static HANDLE_T _hMtDrecThread = NULL_HANDLE;
static HANDLE_T _hMtDrecQueue = NULL_HANDLE;

static BOOL _fgMtDRecLocking = FALSE;
static CRIT_STATE_T _rMtDRecLock;

//static UINT32 _u4MtDRecH264ESBuf = 0;
//static UINT8 au1FrameRate[MTDREC_FRAMERATE_SAMPLE_COUNT];
//static UINT8 u1FrameRateWp = 0;
//static UINT8 u1ValidSampleCount = 0;

static UINT32 _u4MtDRecKeyLen = 128;
static FVR_KEY_TYPE_T _rMTDRecKeyType = FVR_KEY_EVEN;
static MTDREC_CIPHER_ALGORM_T _eMtDrecCipherAlgorithm = MTDREC_CIPHER_NONE;

static BOOL _fgMTDRecUseTimestamp = TRUE;
static MTDREC_TIMESTAMP_T eMtDrecTimestamp = MTDREC_TIMESTAMP_TYPE_4BYTE;

static MTDREC_STREAM_T *_prMtDrecStreamStruct[FVR_GBL_COUNT];

static MTDREC_BUFPTR_T _arMtDrecBuf[MTDREC_NUM_FBM_BUFFER];

static UINT32 u4H264ActualFrameRate = 30;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _MtDRec_Lock
 */
//-----------------------------------------------------------------------------
static void _MtDRec_Lock(void)
{
    _rMtDRecLock = x_crit_start();

    if(_fgMtDRecLocking)
    {
        MTDREC_PRINT("Already lock\n");
        ASSERT(0);
    }

    _fgMtDRecLocking = TRUE;
}


//-----------------------------------------------------------------------------
/** _MtDRec_Unlock
 */
//-----------------------------------------------------------------------------
static void _MtDRec_Unlock(void)
{
    if(!_fgMtDRecLocking)
    {
        MTDREC_PRINT("Already Unlock\n");
        ASSERT(0);
    }

    _fgMtDRecLocking = FALSE;

    x_crit_end(_rMtDRecLock);
}

//-----------------------------------------------------------------------------
/** _MtDRec_GetStreamStruct
 *  Get the STREAM structure of a given TS index
 *
 *  @param  u1Idx      TS index
 *
 *  @retval The pointer of the PID structure
 */
//-----------------------------------------------------------------------------
static MTDREC_STREAM_T *_MtDRec_GetStreamStruct(UINT8 u1Idx)
{
    ASSERT(u1Idx < FVR_GBL_COUNT);

    ASSERT(_prMtDrecStreamStruct[u1Idx]);

    return _prMtDrecStreamStruct[u1Idx];
}

//-----------------------------------------------------------------------------
/** _MtDrecGetH264VdecInfo
 */
//-----------------------------------------------------------------------------
static BOOL _MtDrecGetH264VdecInfo(UCHAR ucEsId, MTDREC_H264_T *prH264, BOOL fgRefresh)
{
    static BOOL isFirst = TRUE;
    static MTDREC_H264_T rH264Info;
    VDEC_HDR_INFO_T rVdecHdr;

    if(isFirst)
    {
        rH264Info.ePicStruct = VDEC_AVC_SEI_FRAME;
        rH264Info.fgFrameMbsOnlyFlag = TRUE;
        rH264Info.u4Timescale = 1000;
        rH264Info.u4NumOfUnit = 30;

        isFirst = FALSE;
    }

    if(fgRefresh)
    {
        // can't update vdec info in ISR
        if(!VDEC_QueryInfo(ucEsId, &rVdecHdr))
        {
            Printf("Fail to get h264 frame rate, use previous\n");
            prH264->ePicStruct = rH264Info.ePicStruct;
            prH264->fgFrameMbsOnlyFlag = rH264Info.fgFrameMbsOnlyFlag;
            prH264->u4Timescale = rH264Info.u4Timescale;
            prH264->u4NumOfUnit = rH264Info.u4NumOfUnit;
            return TRUE;
        }

        if(rVdecHdr.fgSPSValid)
        {
            rH264Info.fgFrameMbsOnlyFlag = rVdecHdr.fgSPSFrameMbsOnlyFlag;
        }

        if(rVdecHdr.fgVUIValid)
        {
            rH264Info.u4Timescale = rVdecHdr.u4VUITimeScale;
            rH264Info.u4NumOfUnit = rVdecHdr.u4VUINumUnitsInTick;
        }

        if(rVdecHdr.fgSEIValid)
        {
            rH264Info.ePicStruct = rVdecHdr.eSEIPicStruct;
        }
    }

    prH264->ePicStruct = rH264Info.ePicStruct;
    prH264->fgFrameMbsOnlyFlag = rH264Info.fgFrameMbsOnlyFlag;
    prH264->u4Timescale = rH264Info.u4Timescale;
    prH264->u4NumOfUnit = rH264Info.u4NumOfUnit;

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MtDrecSetH264FrameInfo
 */
//-----------------------------------------------------------------------------
static void _MtDrecSetH264FrameInfo(UCHAR ucEsId, UINT32 u4PicCount, HAL_TIME_T *prTime)
{
    static UINT8 u1ErrorStreamCounter = 0;
    UINT32 u4FloatFrameRate = 0;
    UINT32 u4CalculateFrameRate = 0;
    UINT32 u4ActualFrameRate = 0;
    MTDREC_H264_T rH264;

    u4FloatFrameRate = (u4PicCount * 1000) /
        ((prTime->u4Seconds * 1000) + (prTime->u4Micros/1000));

    if(!_MtDrecGetH264VdecInfo(ucEsId, &rH264, FALSE))
    {
        return;
    }

    if(rH264.fgFrameMbsOnlyFlag)
    {
        if (rH264.u4NumOfUnit != 0)
        {
            u4CalculateFrameRate = rH264.u4Timescale / (2 * rH264.u4NumOfUnit);
        }
    }
    else
    {
        if (rH264.u4NumOfUnit != 0)
        {
            u4CalculateFrameRate = rH264.u4Timescale / (rH264.u4NumOfUnit);
        }
    }

    // check error stream
    if((10 * u4FloatFrameRate) > (15 * u4CalculateFrameRate))
    {
        if(u1ErrorStreamCounter < 3)
        {
            u1ErrorStreamCounter++;
        }
    }
    else
    {
        if(u1ErrorStreamCounter > 0)
        {
            u1ErrorStreamCounter--;
        }
    }

    if(u1ErrorStreamCounter >= 3)
    {
        u4ActualFrameRate = u4CalculateFrameRate * 2;
    }
    else
    {
        u4ActualFrameRate = u4CalculateFrameRate;
    }

    MTDREC_PRINT("u4ActualFrameRate: %d %d %d %d\n", u4ActualFrameRate, u4FloatFrameRate, u4CalculateFrameRate, u1ErrorStreamCounter);

    _MtDRec_Lock();
    u4H264ActualFrameRate = u4ActualFrameRate;
    _MtDRec_Unlock();
}

//-----------------------------------------------------------------------------
/** _MtDrecGetH264FrameRate
 */
//-----------------------------------------------------------------------------
static MTDREC_PVR_FRAMERATE_T _MtDrecGetH264FrameRate(UCHAR ucEsId)
{
    UINT32 u4ActualFrameRate;
    MTDREC_PVR_FRAMERATE_T eFrameRate;
    MTDREC_H264_T rH264;
    _MtDRec_Lock();
    u4ActualFrameRate = u4H264ActualFrameRate;
    _MtDRec_Unlock();

    if(u4ActualFrameRate < 24)
    {
        eFrameRate = MTDREC_PVR_FR_23_976HZ;
    }
    else if(u4ActualFrameRate == 24)
    {
        eFrameRate = MTDREC_PVR_FR_24HZ;
    }
    else if(u4ActualFrameRate < 28)
    {
        eFrameRate = MTDREC_PVR_FR_25HZ;
    }
    else if(u4ActualFrameRate < 30)
    {
        eFrameRate = MTDREC_PVR_FR_29_97HZ;
    }
    else if(u4ActualFrameRate < 40)
    {
        eFrameRate = MTDREC_PVR_FR_30HZ;
    }
    else if(u4ActualFrameRate < 55)
    {
        eFrameRate = MTDREC_PVR_FR_50HZ;
    }
    else if(u4ActualFrameRate < 60)
    {
        eFrameRate = MTDREC_PVR_FR_59_94HZ;
    }
    else
    {
        eFrameRate = MTDREC_PVR_FR_60HZ;
    }

    if(!_MtDrecGetH264VdecInfo(ucEsId, &rH264, TRUE))
    {
        ;
    }

    return eFrameRate;
}

#if 0
//-----------------------------------------------------------------------------
/** _MtDrecCheckPicStructInSEI
 */
//-----------------------------------------------------------------------------
static UINT32 _MtDrecCheckPicStructInSEI(void)
{
    MTDREC_H264_T rH264;
    UINT32 u4IndexMultiple = 1;

    if(!_MtDrecGetH264VdecInfo(&rH264, FALSE))
    {
        return u4IndexMultiple;
    }

	switch (rH264.ePicStruct)
	{
	case VDEC_AVC_SEI_FRAME:
		if(rH264.fgFrameMbsOnlyFlag)
		{
			u4IndexMultiple += 0;
		}
		else
		{
			u4IndexMultiple += 1;
		}
		break;
	case VDEC_AVC_SEI_TOP_FIELD:
	case VDEC_AVC_SEI_BOTTOM_FIELD:
	case VDEC_AVC_SEI_TOP_BOTTOM:
	case VDEC_AVC_SEI_BOTTOM_TOP:
		u4IndexMultiple += 1;
		break;
	case VDEC_AVC_SEI_TOP_BOTTOM_TOP:
	case VDEC_AVC_SEI_BOTTOM_TOP_BOTTOM:
		u4IndexMultiple += 2;
		break;
	case VDEC_AVC_SEI_FRAME_DOUBLING:
		if(rH264.fgFrameMbsOnlyFlag)
		{
			u4IndexMultiple += 1;
		}
		else
		{
			u4IndexMultiple += 3;
		}
		break;
	case VDEC_AVC_SEI_FRAME_TRIPLING:
		if(rH264.fgFrameMbsOnlyFlag)
		{
			u4IndexMultiple += 2;
		}
		else
		{
			u4IndexMultiple += 5;
		}
		break;
    case VDEC_AVC_SEI_FRAME_UNKNOWN:
    default:
        Printf("Unknow 264 pic struct\n");
        u4IndexMultiple += 0;
        break;
	}

    MTDREC_PRINT("Index: %d %d %d\n", u4IndexMultiple, rH264.ePicStruct, rH264.fgFrameMbsOnlyFlag);

	return u4IndexMultiple;
}
#endif

//-----------------------------------------------------------------------------
/** _MtDrecAddPicCount
 */
//-----------------------------------------------------------------------------
static void _MtDrecAddPicCount(MTDREC_STREAM_T *prDRecStreamStruct, FVR_VIDEO_TYPE_T eVideoType)
{
    HAL_TIME_T rDiffTime;
    HAL_TIME_T rCurTime;

    ASSERT(prDRecStreamStruct != NULL);

    if (!prDRecStreamStruct->fgInitTime)
    {
        //init start time
        HAL_GetTime(&prDRecStreamStruct->rPicStartTime);
        prDRecStreamStruct->fgInitTime = TRUE;
    }

    HAL_GetTime(&rCurTime);
    HAL_GetDeltaTime(&rDiffTime, &prDRecStreamStruct->rPicStartTime, &rCurTime);
    if ((rDiffTime.u4Seconds >= 1) ||
        ((prDRecStreamStruct->eLastVideoPidType != FVR_VIDEO_NONE) && (prDRecStreamStruct->eLastVideoPidType != eVideoType)))
    {
        //save the current pic count, and caculate h264 frame rate
        if(eVideoType == FVR_VIDEO_H264)
        {
            _MtDrecSetH264FrameInfo(prDRecStreamStruct->ucEsId,
                                    prDRecStreamStruct->u4PicCount,
                                    &rDiffTime);
        }
        //reset pic count to 1
        prDRecStreamStruct->u4PicCount = 1;
        //store the current time to next new start time
        x_memcpy(&prDRecStreamStruct->rPicStartTime, &rCurTime, sizeof(HAL_TIME_T));
    }
    else
    {
        //accumulate pic count
        prDRecStreamStruct->u4PicCount++;
    }

    prDRecStreamStruct->eLastVideoPidType = eVideoType;
}

//-----------------------------------------------------------------------------
/** _MtDRecDetectScrambleCallback
 */
//-----------------------------------------------------------------------------
static BOOL _MtDRecDetectScrambleCallback(UINT8 u1Pidx, FVR_SCRAMBLE_STATE_T eState)
{
    DREC_MTAL_CB_T rCB;

    rCB.type = MTDREC_NOTIFY_CODE_SCRAMBLE_STATE;
    rCB.u4Data0 = u1Pidx;
    rCB.u4Data1 = eState;

    UNUSED(_CB_PutEvent(CB_MTAL_DREC_TRIGGER, sizeof(DREC_MTAL_CB_T), &rCB));

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MtDRecDetectRawScrambleCallback
 */
//-----------------------------------------------------------------------------
static BOOL _MtDRecDetectRawScrambleCallback(UINT8 u1Pidx, FVR_SCRAMBLE_STATE_T eState)
{
    DREC_MTAL_CB_T rCB;

    rCB.type = MTDREC_NOTIFY_CODE_RAW_SCRAMBLE_STATE;
    rCB.u4Data0 = u1Pidx;
    rCB.u4Data1 = eState;

    UNUSED(_CB_PutEvent(CB_MTAL_DREC_TRIGGER, sizeof(DREC_MTAL_CB_T), &rCB));

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MtTsDRecPicCallback
 */
//-----------------------------------------------------------------------------
static void _MtTsDRecPicCallback(UINT8 u1Pidx, UINT16 u2Type, UINT32 u4PktIdx,
                            UINT64 u8Pts33, const void* pvTag)
{
    UINT32 u4IndexMultiple = 1, i;
    UINT32 u4PktOffset = 0;
    FVR_VIDEO_TYPE_T eVideoType;
    MTDREC_PVR_INDEX_TYPE_T eType;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;
    UINT32 u4PicWp = 0;
    MTDREC_PID_TYPE_T eDrecPidType = DREC_TYPE_OTHERS;
    FVR_PID_T rPid;

    u4PktIdx--;

    FVR_GetPid(u1Pidx, FVR_PID_FLAG_PID | FVR_PID_FLAG_TSINDEX, &rPid);

    prDRecStreamStruct = _MtDRec_GetStreamStruct(rPid.u1TsIndex);
    eVideoType = FVR_GetVideoTypeByPidx(u1Pidx);
    eType = MTDREC_PVR_INDEX_TYPE_NONE;

    if(eVideoType == FVR_VIDEO_MPEG)
    {
        eDrecPidType = DREC_TYPE_VIDEO_MPEG2;

        if (u2Type == 0xFF)
        {
            eType = MTDREC_PVR_INDEX_TYPE_DIT;
            u4PktIdx = u4PktIdx - 2;
        }
        else if((u2Type & 0x9) == 0x9)
        {
            eType = MTDREC_PVR_INDEX_TYPE_SEQ_I_PIC;
        }
        else
        {
            switch(u2Type & 0xF)
            {
            case 0x1:
                eType = MTDREC_PVR_INDEX_TYPE_I_PIC;
                break;
            case 0x2:
                eType = MTDREC_PVR_INDEX_TYPE_P_PIC;
                break;
            case 0x4:
                eType = MTDREC_PVR_INDEX_TYPE_B_PIC;
                break;
            case 0x8:
                eType = MTDREC_PVR_INDEX_TYPE_SEQ;
                break;
            default:
                break;
            }
        }
        u4IndexMultiple = 1;
    }
    else if(eVideoType == FVR_VIDEO_H264)
    {
        eDrecPidType = DREC_TYPE_VIDEO_H264;

        if (u2Type == 0xFF)
        {
            eType = MTDREC_PVR_INDEX_TYPE_DIT;
            u4PktIdx = u4PktIdx - 2;
        }
        else if(((u2Type & 0x2) == 0x2) && ((u2Type & 0xD) != 0x0))
        {
            eType = MTDREC_PVR_INDEX_TYPE_SEQ_I_PIC;
        }
        else if((u2Type & 0xD) != 0x0)
        {
            eType = MTDREC_PVR_INDEX_TYPE_I_PIC;
        }
        else if((u2Type & 0x2) == 0x2)
        {
                eType = MTDREC_PVR_INDEX_TYPE_SEQ;
        }
        else if((u2Type & 0x30) != 0x0)
        {
                eType = MTDREC_PVR_INDEX_TYPE_P_PIC;
        }
        else if((u2Type & 0xC0) != 0x0)
        {
                eType = MTDREC_PVR_INDEX_TYPE_B_PIC;
        }
        else
        {
            Printf("Which pic type?:0x%x\n", u2Type);
        }

        //u4IndexMultiple = _MtDrecCheckPicStructInSEI();
        u4IndexMultiple = 1;
    }

    u4PktOffset = (u4PktIdx % prDRecStreamStruct->u4DRecStreamPktCount) * 192;
	// <hwachin.lee> Temporary code to avoid offset duplication. Don't remove this code
	if (u4PktOffset == prDRecStreamStruct->u4PrevPicOffset)
	{
		//printf("Index DUP : %d\n", u4OffsetBack);

		// a special handling for SEQ and I starting in the same packet,
        // but I picture pattern is end at the next packet
        if ((prDRecStreamStruct->ePrevIdxType == MTDREC_PVR_INDEX_TYPE_SEQ) &&
            (eType == MTDREC_PVR_INDEX_TYPE_I_PIC))
        {
            // merge these two type into SEQI, and update to the previous pic type
            _MtDRec_Lock();
            if(prDRecStreamStruct->u4MtDRecPicWp == 0)
            {
                u4PicWp = MTDREC_TICK_PIC_COUNT - 1;
            }
            else
            {
                u4PicWp = prDRecStreamStruct->u4MtDRecPicWp - 1;
            }
            prDRecStreamStruct->arMtDRecPicAddr[u4PicWp].eIndexType = MTDREC_PVR_INDEX_TYPE_SEQ_I_PIC;
            _MtDRec_Unlock();
        }

		return;
	}

	prDRecStreamStruct->u4PrevPicOffset = u4PktOffset;
    prDRecStreamStruct->ePrevIdxType = eType;
    u4PicWp = prDRecStreamStruct->u4MtDRecPicWp;

    for(i = 0; i < u4IndexMultiple; i++)
    {
        _MtDRec_Lock();
        prDRecStreamStruct->arMtDRecPicAddr[u4PicWp].eDrecPidType = eDrecPidType;
        prDRecStreamStruct->arMtDRecPicAddr[u4PicWp].u2VideoPid = rPid.u2Pid;
        prDRecStreamStruct->arMtDRecPicAddr[u4PicWp].eIndexType= eType;
        prDRecStreamStruct->arMtDRecPicAddr[u4PicWp].u4Offset = u4PktOffset;
        prDRecStreamStruct->arMtDRecPicAddr[u4PicWp].u8Pts33 = u8Pts33;
        u4PicWp++;
        u4PicWp %= MTDREC_TICK_PIC_COUNT;
        prDRecStreamStruct->u4MtDRecPicWp = u4PicWp;
        _MtDRec_Unlock();
        _MtDrecAddPicCount(prDRecStreamStruct, eVideoType);
    }

    _MtDRec_Lock();
    prDRecStreamStruct->u4MtDRecWritePointer = prDRecStreamStruct->u4DRecStreamAddr + u4PktOffset;
    _MtDRec_Unlock();

    UNUSED(pvTag);
}


//-----------------------------------------------------------------------------
/** _MtDRecFormatIndexArray
 */
//-----------------------------------------------------------------------------
static UINT8 _MtDRecFormatIndexArray(MTDREC_STREAM_T *prDRecStreamStruct, UINT32 u4RecordSize)
{
    UINT32 i, u4StartCodeIdx, u4PicDiff, u4PicRp;
    UINT32 u4DataSize;
    MTDREC_PVR_INDEX_T *prPVRIndex;
    BOOL fgFoundSeqSPS, fgMixedVideo = FALSE;
    UINT32 u4Offset, u4IndexAddr, u4IndexSize;
    UINT8 u1StreamDataWp = 0;
    MTDREC_PID_TYPE_T ePrevDrecPidType = DREC_TYPE_OTHERS;

    //Printf("format idx:%d\n", u1RetArrayIdx);

    if (prDRecStreamStruct == NULL)
    {
        Printf("_MtDRecFormatIndexArray: NULL stream pointer\n");
        return 0xFF;
    }

    u1StreamDataWp = prDRecStreamStruct->u1MtDRecStreamDataWp;

    ASSERT(u1StreamDataWp < MTDREC_STREAM_STRUCT_MAX);
    if(u1StreamDataWp >= MTDREC_STREAM_STRUCT_MAX)
    {
        Printf("stread data WP is wrong: %d\n", u1StreamDataWp);
        return 0xFF;
    }

    _MtDRec_Lock();
    // Calcaulate frame offset
    u4PicDiff = PICINDEXSIZE(prDRecStreamStruct->u4MtDRecPicRp, prDRecStreamStruct->u4MtDRecPicWp, MTDREC_TICK_PIC_COUNT);
    u4StartCodeIdx = prDRecStreamStruct->u4MtDRecPicRp;
    prPVRIndex = (MTDREC_PVR_INDEX_T*)prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexAddr;
    u4Offset = prDRecStreamStruct->u4MtDRecReadPointer - prDRecStreamStruct->u4DRecStreamAddr;
    u4PicRp = prDRecStreamStruct->u4MtDRecPicRp;
    _MtDRec_Unlock();

    fgFoundSeqSPS = FALSE;

    //Printf("prPVRIndex: 0x%x\n", prPVRIndex);

    ASSERT(u4PicDiff < MTDREC_PIC_INDEX_ARRAY_SIZE);
    if(u4PicDiff >= MTDREC_PIC_INDEX_ARRAY_SIZE)
    {
        Printf("wrong picdiff: %d\n", u4PicDiff);
        return 0xFF;
    }

    for(i=0; i<u4PicDiff; i++)
    {
        _MtDRec_Lock();
        u4DataSize = DATASIZE(u4Offset, prDRecStreamStruct->arMtDRecPicAddr[u4PicRp].u4Offset, prDRecStreamStruct->u4DRecStreamSize);
        _MtDRec_Unlock();

        if(u4DataSize > u4RecordSize)
        {
            // Out of record bound
            //Printf("Out of record bound\n");
            break;
        }

        prPVRIndex->u4Offset = u4DataSize;
        prPVRIndex->u8Pts33 = prDRecStreamStruct->arMtDRecPicAddr[u4PicRp].u8Pts33;
        prPVRIndex->eDrecPidType = prDRecStreamStruct->arMtDRecPicAddr[u4PicRp].eDrecPidType;
        prPVRIndex->u2VideoPid = prDRecStreamStruct->arMtDRecPicAddr[u4PicRp].u2VideoPid;
        prPVRIndex->eIndexType = prDRecStreamStruct->arMtDRecPicAddr[u4PicRp].eIndexType;

        if (i == 0)
        {
            ePrevDrecPidType = prPVRIndex->eDrecPidType;
        }

        if (ePrevDrecPidType != prPVRIndex->eDrecPidType)
        {
            fgMixedVideo = TRUE;
        }

        // Find SeqSPS
        _MtDRec_Lock();
        prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexSize++;
        if (!fgMixedVideo)
        {
            if(fgFoundSeqSPS)
            {
                // For H264 frame rate, calculate serach range
                prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SPSNextStartLen = prPVRIndex->u4Offset -
                            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SeqSPSOffset;
            }

            //Printf("got idx:%d, offset:0x%x Index: %d\n", u4PicRp, prPVRIndex->u4Offset, prPVRIndex->eIndexType);

            if(!fgFoundSeqSPS)
            {
                if((prPVRIndex->eIndexType == MTDREC_PVR_INDEX_TYPE_SEQ) ||
                   (prPVRIndex->eIndexType == MTDREC_PVR_INDEX_TYPE_SEQ_I_PIC))
                {
                    fgFoundSeqSPS = TRUE;
                    prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].fgFoundSeqSPS = TRUE;
                    prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SeqSPSOffset = prPVRIndex->u4Offset;
                    prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SPSNextStartLen = 0;

                    //Printf("Got SPS, %d\n", u4PicRp);
                }
            }
        }
        _MtDRec_Unlock();

        u4PicRp++;
        u4PicRp %= MTDREC_TICK_PIC_COUNT;

        prPVRIndex++;
        ePrevDrecPidType = prPVRIndex->eDrecPidType;
    }

    u4IndexAddr = prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexAddr;
    u4IndexSize = prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexSize;
    
    if (u4IndexSize > 0)
    {
        HalFlushInvalidateDCacheMultipleLine(u4IndexAddr, u4IndexSize*sizeof(MTDREC_PVR_INDEX_T));
    }

    _MtDRec_Lock();
    prDRecStreamStruct->u4MtDRecPicRp = u4PicRp;

    if (fgMixedVideo)
    {
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].fgFoundSeqSPS = FALSE;
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SeqSPSOffset = 0;
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SPSNextStartLen = 0;
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].eDrecPidType = DREC_TYPE_OTHERS;
    }
    else
    {
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].eDrecPidType = ePrevDrecPidType;
    }

    _MtDRec_Unlock();

    return u1StreamDataWp;
}

//-----------------------------------------------------------------------------
/** _MtDRecSearchMPEG2FrameRate
 */
//-----------------------------------------------------------------------------
static MTDREC_PVR_FRAMERATE_T _MtDRecGetMPEG2FrameRate(UCHAR ucEsId)
{
    VDEC_HDR_INFO_T rVdecHdr;
    static MTDREC_PVR_FRAMERATE_T ePreviousFrameRate = MTDREC_PVR_FR_23_976HZ;

    if(!VDEC_QueryInfo(ucEsId, &rVdecHdr))
    {
        MTDREC_PRINT("Fail to get Mpeg frame rate, previous: %d\n", ePreviousFrameRate);
        return ePreviousFrameRate;
    }

    switch(rVdecHdr.u2FrmRate)
        {
    case 2397:
        ePreviousFrameRate = MTDREC_PVR_FR_23_976HZ;
            break;
    case 2400:
        ePreviousFrameRate = MTDREC_PVR_FR_24HZ;
        break;
    case 2500:
        ePreviousFrameRate = MTDREC_PVR_FR_25HZ;
        break;
    case 2997:
        ePreviousFrameRate = MTDREC_PVR_FR_29_97HZ;
        break;
    case 3000:
        ePreviousFrameRate = MTDREC_PVR_FR_30HZ;
        break;
    case 5000:
        ePreviousFrameRate = MTDREC_PVR_FR_50HZ;
        break;
    case 5994:
        ePreviousFrameRate = MTDREC_PVR_FR_59_94HZ;
        break;
    case 6000:
        ePreviousFrameRate = MTDREC_PVR_FR_60HZ;
        break;
    default:
        Printf("Unknown frame rate %d\n", rVdecHdr.u2FrmRate);
        ePreviousFrameRate = MTDREC_PVR_FR_UNKNOWN;
        break;
    }

    MTDREC_PRINT("Get Mpeg2 frame rate: %d, id: %d\n", rVdecHdr.u2FrmRate, ePreviousFrameRate);

    return ePreviousFrameRate;
}


//-----------------------------------------------------------------------------
/** _MtDRecStreamSearchFrameRate
 */
//-----------------------------------------------------------------------------
static MTDREC_PVR_FRAMERATE_T _MtDRecStreamSearchFrameRate(MTDREC_STREAM_T *prDRecStreamStruct,
                                                            MTDREC_PID_TYPE_T eDrecPidType)
{
    MTDREC_PVR_FRAMERATE_T eFrameRate = MTDREC_PVR_FR_25HZ;

    if(eDrecPidType == DREC_TYPE_VIDEO_MPEG2)
    {
        eFrameRate = _MtDRecGetMPEG2FrameRate(prDRecStreamStruct->ucEsId);
		//printf("MPEG2 FrameRate = %d \n", eFrameRate);
    }
    else if(eDrecPidType == DREC_TYPE_VIDEO_H264)
    {
        eFrameRate = _MtDrecGetH264FrameRate(prDRecStreamStruct->ucEsId);
		//printf("H264 FrameRate = %d \n", eFrameRate);
    }
    else
    {
        eFrameRate = MTDREC_PVR_FR_UNKNOWN;
    }

    return eFrameRate;
}


//-----------------------------------------------------------------------------
/** _MtDRecStreamPostProcessing
 */
//-----------------------------------------------------------------------------
static void _MtDRecStreamPostProcessing(MTDREC_STREAM_T *prDRecStreamStruct, UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 i, u4BufStart, u4BufEnd, u4CopySize;
    UINT32 *pu4Tmp1, *pu4Tmp2;
    UINT32 u4Timestamp1, u4Timestamp2, u4TimeDiff;
    BOOL fgSeqSPS;
    UINT32 u4SeqSPSAddr, u4FRPreIndex;
    MTDREC_PVR_FRAMERATE_T eFrameRate;
    UINT8 *pu1Tmp;
    UINT8 u1StreamDataWp;
    MTDREC_PID_TYPE_T eDrecPidType;

    ASSERT(prDRecStreamStruct != NULL);

    _MtDRec_Lock();
    u4BufStart = prDRecStreamStruct->u4DRecStreamAddr;
    u4BufEnd = prDRecStreamStruct->u4DRecStreamAddr + prDRecStreamStruct->u4DRecStreamSize;
    _MtDRec_Unlock();

    ASSERT((u4Addr >= u4BufStart) && (u4Addr < u4BufEnd));

    // *******************************
    // If ring, copy to linear
    // *******************************
    if((u4Addr + u4Size) > u4BufEnd)
    {
        Printf("should not have this case!!!\n");
        u4CopySize = u4Addr + u4Size - u4BufEnd;

        ASSERT(((u4Addr%4) == 0) && ((u4Size%4) == 0));

        // Flush cache, no need
        HalInvalidateDCacheMultipleLine(u4BufStart, u4CopySize);
        //HalInvalidateDCache();

        // Memory copy
        pu4Tmp1 = (UINT32*)u4BufStart;
        pu4Tmp2 = (UINT32*)u4BufEnd;
        for(i=0; i<(u4CopySize/4); i++)
        {
            *(UINT32*)(pu4Tmp2 + i) = *(UINT32*)(pu4Tmp1 + i);
        }

        HalFlushDCacheMultipleLine(u4BufEnd, u4CopySize);
    }

    // Flush cache
    //HalFlushInvalidateDCache();
    //HalFlushInvalidateDCacheMultipleLine(u4Addr, u4Size);

    if((u4Addr + u4Size) > u4BufEnd)
    {
        Printf("should not have this case!!!\n");
        u4CopySize = u4Addr + u4Size - u4BufEnd;
        HalInvalidateDCacheMultipleLine(u4Addr, u4Size - u4CopySize);
        HalInvalidateDCacheMultipleLine(u4BufStart, u4CopySize);
    }
    else
    {
        HalInvalidateDCacheMultipleLine(u4Addr, u4Size);
    }

    // *******************************
    // Calculate bitrate
    // *******************************
    pu1Tmp = (UINT8*)u4Addr;
    u4Timestamp1 = (((UINT32)pu1Tmp[0] << 24) | ((UINT32)pu1Tmp[1] << 16) | ((UINT32)pu1Tmp[2] << 8)
                    | (UINT32)pu1Tmp[3]);
    pu1Tmp = (UINT8*)(u4Addr + u4Size - 192);
    u4Timestamp2 = (((UINT32)pu1Tmp[0] << 24) | ((UINT32)pu1Tmp[1] << 16) | ((UINT32)pu1Tmp[2] << 8)
                    | (UINT32)pu1Tmp[3]);

    if(u4Timestamp2 < u4Timestamp1)
    {
        u4TimeDiff = u4Timestamp2 + (0xFFFFFFFF - u4Timestamp1);
    }
    else
    {
        u4TimeDiff = u4Timestamp2 - u4Timestamp1;
    }
//    Printf("time:0x%x, 0x%x, 0x%x, packet:0x%x\n", u4Timestamp1, u4Timestamp2, u4TimeDiff,
//                        *(UINT32*)(u4Addr+4));

    _MtDRec_Lock();

    u1StreamDataWp = prDRecStreamStruct->u1MtDRecStreamDataWp;
    ASSERT(u1StreamDataWp < MTDREC_STREAM_STRUCT_MAX);
    if(u1StreamDataWp >= MTDREC_STREAM_STRUCT_MAX)
    {
        _MtDRec_Unlock();
        return;
    }

    prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4BitRate = u4Size * (UINT32)(27000000 / u4TimeDiff) * 8 / 1024;
    fgSeqSPS = prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].fgFoundSeqSPS;
    u4SeqSPSAddr = u4Addr + prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SeqSPSOffset;

    if(prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4BitRate >= MTDREC_HITRATE_THRESHOLD)
    {
        prDRecStreamStruct->u4MtDRecDefineBufSize = (eMtDrecTimestamp == MTDREC_TIMESTAMP_TYPE_4BYTE)?
            MTDREC_GETSTREAM192_HBR_SIZE:MTDREC_GETSTREAM188_HBR_SIZE;
    }
    else
    {
        prDRecStreamStruct->u4MtDRecDefineBufSize = (eMtDrecTimestamp == MTDREC_TIMESTAMP_TYPE_4BYTE)?
            MTDREC_GETSTREAM192_LBR_SIZE:MTDREC_GETSTREAM188_LBR_SIZE;
    }

    // *******************************
    // Search frame rate
    // *******************************
    if(u1StreamDataWp == 0)
    {
        u4FRPreIndex = MTDREC_STREAM_STRUCT_MAX - 1;
    }
    else
    {
        u4FRPreIndex = u1StreamDataWp - 1;
    }
    prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].eFrameRate =
                            prDRecStreamStruct->arMtDRecStreamInfo[u4FRPreIndex].eFrameRate;

    eDrecPidType = prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].eDrecPidType;
    _MtDRec_Unlock();

    if(fgSeqSPS)
    {
        eFrameRate = _MtDRecStreamSearchFrameRate(prDRecStreamStruct, eDrecPidType);
        _MtDRec_Lock();
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].eFrameRate = eFrameRate;
        prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate = 0;
        switch(eFrameRate)
        {
        case MTDREC_PVR_FR_23_976HZ:
        case MTDREC_PVR_FR_24HZ:
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate = 24;
            break;
        case MTDREC_PVR_FR_25HZ:
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate = 25;
            break;
        case MTDREC_PVR_FR_29_97HZ:
        case MTDREC_PVR_FR_30HZ:
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate = 30;
            break;
        case MTDREC_PVR_FR_50HZ:
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate = 50;
            break;
        case MTDREC_PVR_FR_59_94HZ:
        case MTDREC_PVR_FR_60HZ:
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate  = 60;
            break;
        default:
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate  = 30;
            break;
        }
        _MtDRec_Unlock();
        //Printf("search frame rate: %d\n", (UINT32)eFrameRate);
    }
}

//-----------------------------------------------------------------------------
/** _MtDRecThread
 */
//-----------------------------------------------------------------------------
static VOID _MtDRecThread(VOID* pvArg)
{
    MTDREC_MSG_T rMsg;
    INT32 i4Ret;
    SIZE_T zMsgSize;
    UINT16 u2QueueIndex;
    FVR_GBL_T rGBL;
    UINT32 u4RecSize, u4Timeout;
    UINT32 u4ReadPointer, u4WritePointer;
    MTDREC_GETSTREAM_RET_T eReturn = GETSTREAM_NORET;
    DREC_MTAL_CB_T rCB;
    UINT32 u4ElapseTime = 0;
    UINT8 u1Idx = 0;
    UINT8 u1StreamDataWp = 0;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    UNUSED(pvArg);

    zMsgSize = sizeof (rMsg);
    while (1)
    {
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rMsg, &zMsgSize,
            &_hMtDrecQueue, 1, X_MSGQ_OPTION_WAIT);

        // If not start, continue
        u1Idx = rMsg.u1Idx;
        u4Timeout = rMsg.u4Timeout;
        eReturn = GETSTREAM_NORET;
        u4ElapseTime = 0;
        prDRecStreamStruct = _MtDRec_GetStreamStruct(u1Idx);

        while(1)
        {
            _MtDRec_Lock();
            if(prDRecStreamStruct->fgDRecStreamStop)
            {
                _MtDRec_Unlock();
                x_memset((void*)&rCB, 0, sizeof(rCB));
                rCB.result = TRUE;
                rCB.type = MTDREC_NOTIFY_CODE_STREAM_DATA;
                // return not ok

                rCB.u4Data0 = prDRecStreamStruct->u4DRecStreamAddr;
                rCB.u4Data1 = 0;
                rCB.u4Data2 = prDRecStreamStruct->u4DRecIndexBufAddr;
                rCB.u4Data3 = 0;
                rCB.u4Data4 = u1Idx;
                UNUSED(_CB_PutEvent(CB_MTAL_DREC_TRIGGER, sizeof (DREC_MTAL_CB_T), &rCB));
                break;
            }

            u1StreamDataWp = prDRecStreamStruct->u1MtDRecStreamDataWp;

            ASSERT(u1StreamDataWp < MTDREC_STREAM_STRUCT_MAX);
            if(u1StreamDataWp >= MTDREC_STREAM_STRUCT_MAX)
            {
                _MtDRec_Unlock();
                break;
            }

            ASSERT(!prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].fgValid);
            if (prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].fgValid)
            {
                _MtDRec_Unlock();
                Printf("No available stream array\n");
                x_thread_delay(20);
                continue;
            }

            // Reset stream info
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].eFrameRate = MTDREC_PVR_FR_UNKNOWN;
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].fgFoundSeqSPS = FALSE;
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4BitRate = 0;
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4FrameRate = 0;
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SeqSPSOffset = 0;
            prDRecStreamStruct->arMtDRecStreamInfo[u1StreamDataWp].u4SPSNextStartLen = 0;

            _MtDRec_Unlock();

            x_memset((void*)&rGBL, 0, sizeof(rGBL));

            if(!FVR_GetGBL(u1Idx,FVR_GBL_FLAG_BUFFER, &rGBL))
            {
                ASSERT(0);
                //break;
            }

            /* Update write pointer by picture write pointer instead of HW write pointer.
             * It can avoid the bounded case that HW write pointer is updated,
             * but picture write pointer is not updated.
             * Normal case:
             *      |-----------|-------|
             *      PICp       HW Wp
             * Bounded case:
             *      |-------|---------|
             *      HW Wp  PICp
             */
            _MtDRec_Lock();
            u4WritePointer = prDRecStreamStruct->u4MtDRecWritePointer;
            u4ReadPointer = prDRecStreamStruct->u4MtDRecReadPointer;
            u4RecSize = DATASIZE(u4ReadPointer, u4WritePointer, rGBL.u4BufSize);
            _MtDRec_Unlock();

            if(!FVR_UpdateWritePointer(u1Idx, u4WritePointer))
            {
                ASSERT(0);
                //break;
            }

            if(u4ElapseTime > u4Timeout)
            {
                //Printf("%s, timeout\n", __FUNCTION__);
                if(u4RecSize < MTDREC_GETSTREAM192_SIZE)
                {
                    x_memset((void*)&rCB, 0, sizeof(rCB));
                    rCB.result = FALSE;
                    rCB.type = MTDREC_NOTIFY_CODE_STREAM_DATA;
                    rCB.u4Data4 = u1Idx;
                    // return not ok
                    UNUSED(_CB_PutEvent(CB_MTAL_DREC_TRIGGER, sizeof (DREC_MTAL_CB_T), &rCB));
                    break;
                }
                else
                {
                    eReturn = GETSTREAM_TIMEOUT;
                }
            }

            _MtDRec_Lock();
            if(u4RecSize >= prDRecStreamStruct->u4MtDRecDefineBufSize)
            {
                eReturn = GETSTREAM_DATA_ENOUGH;
            }
            _MtDRec_Unlock();

            if(eReturn != GETSTREAM_NORET)
            {
                UINT8 u1RetIndex;
                UINT32 u4SizeToBufEnd;

                // make the descision how much size will be notified, the data unit 96KB
                _MtDRec_Lock();
                if(u4RecSize >= prDRecStreamStruct->u4MtDRecDefineBufSize) // enough size case
                {
                    u4RecSize = prDRecStreamStruct->u4MtDRecDefineBufSize;
                }
                else
                {
                    u4RecSize = (u4RecSize / MTDREC_GETSTREAM192_SIZE) * MTDREC_GETSTREAM192_SIZE;
                }
                _MtDRec_Unlock();

                // check if u4MtDRecDefineBufSize is larger than "size to the end of buffer"
                // if yes, only notify "size to the end of buffer"
                u4SizeToBufEnd =  DATASIZE(u4ReadPointer, rGBL.u4BufStart + rGBL.u4BufSize,
                    rGBL.u4BufSize);
                u4RecSize = (u4RecSize > u4SizeToBufEnd)?u4SizeToBufEnd:u4RecSize;

                // Format index table
                u1RetIndex = _MtDRecFormatIndexArray(prDRecStreamStruct, u4RecSize);
                ASSERT(u1RetIndex != 0xFF);

                // ****************************
                // 1.Transfer ring buffer to linear buffer
                // 2.Fill stream info
                // ****************************
                _MtDRecStreamPostProcessing(prDRecStreamStruct, u4ReadPointer, u4RecSize);

                // Update new read pointer
                _MtDRec_Lock();
                prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].fgValid = TRUE;
                prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4Addr = prDRecStreamStruct->u4MtDRecReadPointer;
                prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4Size = u4RecSize;

                if(u1RetIndex == 0xFF)
                {
                    Printf("%s, index fault.\n", __FUNCTION__);
                    prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexSize = 0;
                }

                rCB.result = TRUE;
                rCB.type = MTDREC_NOTIFY_CODE_STREAM_DATA;
                rCB.u4Data0 = prDRecStreamStruct->u4MtDRecReadPointer;
                rCB.u4Data1 = u4RecSize;
                rCB.u4Data2 = prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexAddr;
                rCB.u4Data3 = prDRecStreamStruct->arMtDRecStreamData[u1StreamDataWp].u4IndexSize;
                rCB.u4Data4 = u1Idx;

                MTDREC_PRINT("Return recorded buffer addr: 0x%x, size: 0x%x\n", prDRecStreamStruct->u4MtDRecReadPointer, u4RecSize);

                // Update new value
                prDRecStreamStruct->u4MtDRecReadPointer += u4RecSize;
                if(prDRecStreamStruct->u4MtDRecReadPointer >= (rGBL.u4BufStart + rGBL.u4BufSize))
                {
                    prDRecStreamStruct->u4MtDRecReadPointer -= rGBL.u4BufSize;
                }

                u1StreamDataWp++;
                u1StreamDataWp %= MTDREC_STREAM_STRUCT_MAX;
                prDRecStreamStruct->u1MtDRecStreamDataWp = u1StreamDataWp;
                _MtDRec_Unlock();

                //HalFlushInvalidateDCache();
                UNUSED(_CB_PutEvent(CB_MTAL_DREC_TRIGGER, sizeof (DREC_MTAL_CB_T), &rCB));
                break;
            }

            x_thread_delay(20);
            u4ElapseTime += 20;
        }
    }
}



//-----------------------------------------------------------------------------
/** _MTDREC_Init
 *  Initialize
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_Init(unsigned long arg)
{
    static BOOL _fgInit = FALSE;
    FVR_GBL_T rGBL;
    UINT32 i;
    UINT32 u4FbmSize, u4FbmAddr;
    FBM_POOL_T* prFbmPool;
	UINT8 u1Idx;

    if(!_fgInit)
    {
        if (x_msg_q_create(&_hMtDrecQueue, MTDREC_MSGQ_NAME,
                        sizeof (MTDREC_MSG_T), MTDREC_MSGQ_MSG_COUNT) != OSR_OK)
        {
            return MTR_NOT_OK;
        }

        if (x_thread_create(&_hMtDrecThread, MTDREC_THREAD_NAME,
            MTDREC_THREAD_STACK_SIZE, MTDREC_THREAD_PRIORITY, _MtDRecThread, 0,
            NULL) != OSR_OK)
        {
            return MTR_NOT_OK;
        }

        _fgInit = TRUE;
    }

    if(!FVR_Init())
    {
        return MTR_NOT_OK;
    }

    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PVR);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);

    u4FbmAddr = VIRTUAL(prFbmPool->u4Addr);
    u4FbmSize = prFbmPool->u4Size;
    u4FbmSize /= MTDREC_NUM_FBM_BUFFER;

    for (i = 0; i < MTDREC_NUM_FBM_BUFFER; i++)
    {
        _arMtDrecBuf[i].fgUsed = FALSE;
        _arMtDrecBuf[i].u1TsIdx = 0xFF;
        _arMtDrecBuf[i].u4FbmAddr = VIRTUAL(prFbmPool->u4Addr);
        _arMtDrecBuf[i].u4FbmSize = prFbmPool->u4Size;
        _arMtDrecBuf[i].u4ChBaseAddr = u4FbmAddr;
        _arMtDrecBuf[i].u4ChBaseSize = u4FbmSize;
        _arMtDrecBuf[i].u4ChStreamAddr = u4FbmAddr;
        _arMtDrecBuf[i].u4ChStreamSize = u4FbmSize;

        u4FbmAddr += u4FbmSize;
    }

    for (u1Idx = 0; u1Idx < FVR_GBL_COUNT; u1Idx++)
    {
        x_memset((void*)&rGBL, 0, sizeof(rGBL));
        rGBL.u4TimeToNotify = 0;
        rGBL.pfnNotifyFunc = NULL;
        rGBL.pvNotifyTag = NULL;
        if(!FVR_SetGBL(u1Idx,FVR_GBL_FLAG_TARGET_PKT | FVR_GBL_FLAG_NOTIFY | FVR_GBL_FLAG_TIME_INFO, &rGBL))
        {
            return MTR_NOT_OK;
        }
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_AllocBuffer
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_AllocBuffer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTDREC_BUFPTR_T *prBuf, rBuf;
    UINT32 i, u4ChBaseSize = 0, u4ChBaseAddr = 0;
	UINT8 u1TsIdx, u1FbmIdx;
    BOOL fgAllocate = FALSE, fgIsFound = FALSE;
    FVR_GBL_T rGBL;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	u1TsIdx = (UINT8)rArg.ai4Arg[0];
    prBuf = (MTDREC_BUFPTR_T*)rArg.ai4Arg[1];

    if (u1TsIdx >= FVR_GBL_COUNT)
    {
        goto lbErrExit1;
    }

    _MtDRec_Lock();
    for (u1FbmIdx = 0; u1FbmIdx < MTDREC_NUM_FBM_BUFFER; u1FbmIdx++)
    {
        if ((_arMtDrecBuf[u1FbmIdx].fgUsed) &&
            (u1TsIdx == _arMtDrecBuf[u1FbmIdx].u1TsIdx))
        {
            fgIsFound = TRUE;
            break;
        }
    }

    if (!fgIsFound)
    {
        for (u1FbmIdx = 0; u1FbmIdx < MTDREC_NUM_FBM_BUFFER; u1FbmIdx++)
        {
            if (!_arMtDrecBuf[u1FbmIdx].fgUsed)
            {
                fgAllocate = TRUE;
                _arMtDrecBuf[u1FbmIdx].fgUsed = TRUE;
                _arMtDrecBuf[u1FbmIdx].u1TsIdx = u1TsIdx;
                u4ChBaseAddr = _arMtDrecBuf[u1FbmIdx].u4ChBaseAddr;
                u4ChBaseSize = _arMtDrecBuf[u1FbmIdx].u4ChBaseSize;
                break;
            }
        }
    }
    _MtDRec_Unlock();

    if (!fgIsFound && !fgAllocate)
    {
        goto lbErrExit1;
    }

    if (!_prMtDrecStreamStruct[u1TsIdx])
    {
        _prMtDrecStreamStruct[u1TsIdx] = (MTDREC_STREAM_T *)x_mem_alloc(sizeof(MTDREC_STREAM_T));
        if (!_prMtDrecStreamStruct[u1TsIdx])
        {
            goto lbErrExit2;
        }
        x_memset((void*)_prMtDrecStreamStruct[u1TsIdx], 0, sizeof(MTDREC_STREAM_T));  
    }
    
    if (fgAllocate)
    {
        prDRecStreamStruct = _MtDRec_GetStreamStruct(u1TsIdx);

        _MtDRec_Lock();
        prDRecStreamStruct->eLastVideoPidType = FVR_VIDEO_NONE;
        prDRecStreamStruct->u4MtDRecDefineBufSize = MTDREC_GETSTREAM192_LBR_SIZE;
        
        prDRecStreamStruct->u4DRecBaseAddr = u4ChBaseAddr;
        prDRecStreamStruct->u4DRecBaseSize = u4ChBaseSize;
        prDRecStreamStruct->u4DRecIndexBufAddr = (u4ChBaseAddr + (PAGE_SIZE - 1)) & PAGE_MASK;
        prDRecStreamStruct->u4DRecIndexBufSize = (MTDREC_PIC_INDEX_ARRAY_SIZE * sizeof(MTDREC_PVR_INDEX_T)) * MTDREC_STREAM_STRUCT_MAX;
        for(i=0; i<MTDREC_STREAM_STRUCT_MAX; i++)
        {
            prDRecStreamStruct->arMtDRecStreamData[i].u4IndexAddr =
              VIRTUAL((prDRecStreamStruct->u4DRecIndexBufAddr + ((MTDREC_PIC_INDEX_ARRAY_SIZE * sizeof(MTDREC_PVR_INDEX_T)) * i)));
        }

        x_memset((void*)&rGBL, 0, sizeof(rGBL));
        rGBL.fgAllocateBuffer = FALSE;
        // Linux kernel start address must page alignment
        rGBL.u4BufStart = prDRecStreamStruct->u4DRecIndexBufAddr + prDRecStreamStruct->u4DRecIndexBufSize;
        rGBL.u4BufSize = u4ChBaseSize - prDRecStreamStruct->u4DRecIndexBufSize;
        rGBL.u4BufSize -= (rGBL.u4BufSize % MTDREC_GETSTREAM192_ALIGN_SIZE);
        rGBL.fgSetAlign = TRUE;
        rGBL.u4BufAlign = 192;
        _MtDRec_Unlock();

        if(!FVR_SetGBL(u1TsIdx,FVR_GBL_FLAG_BUFFER, &rGBL))
        {
            goto lbErrExit2;
        }

        // Get stream addr and size after align handle
        if(!FVR_GetGBL(u1TsIdx,FVR_GBL_FLAG_BUFFER, &rGBL))
        {
            goto lbErrExit2;
        }

        _MtDRec_Lock();
        prDRecStreamStruct->u4DRecStreamAddr = rGBL.u4BufStart;
        prDRecStreamStruct->u4DRecStreamSize = rGBL.u4BufSize;
        prDRecStreamStruct->u4DRecStreamPktCount = prDRecStreamStruct->u4DRecStreamSize / 192;

        _arMtDrecBuf[u1FbmIdx].u4ChBaseAddr = prDRecStreamStruct->u4DRecBaseAddr;
        _arMtDrecBuf[u1FbmIdx].u4ChBaseSize = prDRecStreamStruct->u4DRecBaseSize;
        _arMtDrecBuf[u1FbmIdx].u4ChStreamAddr = prDRecStreamStruct->u4DRecStreamAddr;
        _arMtDrecBuf[u1FbmIdx].u4ChStreamSize = prDRecStreamStruct->u4DRecStreamSize;
        _MtDRec_Unlock();
    }

    rBuf = _arMtDrecBuf[u1FbmIdx];

    USR_SPACE_ACCESS_VALIDATE_ARG(prBuf, MTDREC_BUFPTR_T);
    COPY_TO_USER_ARG(prBuf, rBuf, MTDREC_BUFPTR_T);

    Printf("FBM kernel base:0x%x, size:0x%x, stream addr:0x%x, streamsize:0x%x\n",
        rBuf.u4ChBaseAddr, rBuf.u4ChBaseSize, rBuf.u4ChStreamAddr, rBuf.u4ChStreamSize);

    return MTR_OK;

lbErrExit2:
    _MtDRec_Lock();
    _arMtDrecBuf[u1FbmIdx].fgUsed = FALSE;
    _arMtDrecBuf[u1FbmIdx].u1TsIdx = 0xFF;
    _MtDRec_Unlock();

    if (_prMtDrecStreamStruct[u1TsIdx])
    {
        x_mem_free((VOID*)_prMtDrecStreamStruct[u1TsIdx]);
        _prMtDrecStreamStruct[u1TsIdx] = NULL;
    }

lbErrExit1:

    return MTR_NOT_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_FreeBuffer
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_FreeBuffer(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1TsIdx, u1FbmIdx;
    MT_RESULT_T fgRet = MTR_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1TsIdx = (UINT8)Arg;

    _MtDRec_Lock();
    for (u1FbmIdx = 0; u1FbmIdx < MTDREC_NUM_FBM_BUFFER; u1FbmIdx++)
    {
        if ((_arMtDrecBuf[u1FbmIdx].fgUsed) &&
            (u1TsIdx == _arMtDrecBuf[u1FbmIdx].u1TsIdx))
        {
            break;
        }
    }
    _MtDRec_Unlock();

    if (u1FbmIdx < MTDREC_NUM_FBM_BUFFER)
    {
        if (!FVR_FreeBuffer(u1TsIdx))
        {
            fgRet = MTR_NOT_OK;
        }
        else
        {
            _MtDRec_Lock();
            _arMtDrecBuf[u1FbmIdx].fgUsed = FALSE;
            _arMtDrecBuf[u1FbmIdx].u1TsIdx = 0xFF;
            _MtDRec_Unlock();
        }
    }

    if (_prMtDrecStreamStruct[u1TsIdx])
    {
        x_mem_free((VOID*)_prMtDrecStreamStruct[u1TsIdx]);
        _prMtDrecStreamStruct[u1TsIdx] = NULL;
    }
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MTDREC_GetBufferPtr
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_GetBufferPtr(unsigned long arg)
{
    UINT8 u1TsIdx;
    INT32 i;
    MTAL_IOCTL_2ARG_T rArg;
    MTDREC_BUFPTR_T *prBuf, rBuf;
    MT_RESULT_T fgRet = MTR_OK;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	u1TsIdx = (UINT8)rArg.ai4Arg[0];
    prBuf = (MTDREC_BUFPTR_T*)rArg.ai4Arg[1];

    _MtDRec_Lock();
    for (i = 0; i < MTDREC_NUM_FBM_BUFFER; i++)
    {
        if (u1TsIdx == _arMtDrecBuf[i].u1TsIdx)
            break;
    }
    _MtDRec_Unlock();

    if (i >= MTDREC_NUM_FBM_BUFFER)
    {
        fgRet = MTR_NOT_OK;
    }
    else
    {
        _MtDRec_Lock();
        rBuf = _arMtDrecBuf[i];
        _MtDRec_Unlock();

        USR_SPACE_ACCESS_VALIDATE_ARG(prBuf, MTDREC_BUFPTR_T);
        COPY_TO_USER_ARG(prBuf, rBuf, MTDREC_BUFPTR_T);
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SetPid
 *  Set record PID information
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SetPid(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1Pidx;
    FVR_PID_T rFvrPid;
	MTDREC_PID_T* prPid =NULL;
    MTDREC_PID_T rPid;
    MTDREC_CIPHER_ALGORM_T eCipherAlgo;

	x_memset((void *)&rPid,0,sizeof(MTDREC_PID_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    prPid = (MTDREC_PID_T*)rArg.ai4Arg[1];

	USR_SPACE_ACCESS_VALIDATE_ARG(prPid, MTDREC_PID_T);
    COPY_FROM_USER_ARG(prPid, rPid, MTDREC_PID_T);

    x_memset((void*)&rFvrPid, 0, sizeof(FVR_PID_T));

    rFvrPid.fgEnable = rPid.fgEnable;
    rFvrPid.u2Pid = rPid.u2Pid;
    rFvrPid.u1TsIndex = rPid.u1TsIndex;
    rFvrPid.eSource = DMX_SOURCE_DTV0;

    rFvrPid.fgPktSwap = FALSE;
    rFvrPid.pfnSwapNotify = NULL;
    rFvrPid.pvSwapNotifyTag = NULL;
    rFvrPid.ePidType = MTDMX_PID_TYPE_NONE; //check with jacky

    if(rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)
    {
        MTDREC_STREAM_T *prDRecStreamStruct = NULL;

        prDRecStreamStruct = _MtDRec_GetStreamStruct(rFvrPid.u1TsIndex);

        _MtDRec_Lock();
        prDRecStreamStruct->ucEsId = rPid.ucEsId;
        _MtDRec_Unlock();

        rFvrPid.fgPicSearch = TRUE;
        rFvrPid.u1PicSearchMask = 0xFF;
        rFvrPid.pfnPicSearchNotify = _MtTsDRecPicCallback;
        rFvrPid.pvPicSearchNotifyTag = NULL;
    }
    else
    {
        rFvrPid.fgPicSearch = FALSE;
        rFvrPid.u1PicSearchMask = 0;
        rFvrPid.pfnPicSearchNotify = NULL;
        rFvrPid.pvPicSearchNotifyTag = NULL;
    }

    if((rPid.ePidType == MTDMX_PID_TYPE_ES_VIDEO)||(rPid.ePidType == MTDMX_PID_TYPE_ES_AUDIO))
    {
        rFvrPid.ePidType = rPid.ePidType;
    }

    _MtDRec_Lock();
    eCipherAlgo = _eMtDrecCipherAlgorithm;
    _MtDRec_Unlock();

    if (eCipherAlgo == MTDREC_CIPHER_AES_CBC)
    {
        rFvrPid.fgScramble = TRUE;
        rFvrPid.fgCBCMode = TRUE;
    }
    else if(eCipherAlgo == MTDREC_CIPHER_AES_ECB)
    {
        rFvrPid.fgScramble = TRUE;
        rFvrPid.fgCBCMode = FALSE;
    }
    else
    {
        rFvrPid.fgScramble = FALSE;
        rFvrPid.fgCBCMode = FALSE;
    }

    rFvrPid.eKeyType = _rMTDRecKeyType;
    rFvrPid.u1KeyIdx = MTDREC_CIPHER_KEY_INDEX;
    rFvrPid.u4KeyLen = _u4MtDRecKeyLen;

    rFvrPid.fgDetectScramble = rPid.fgDetectScramble;
    if (rPid.pfnScramble)
    {
        rFvrPid.pfnDetectScrambleNotify = _MtDRecDetectScrambleCallback;
    }

    if (rPid.pfnRawScramble)
    {
        rFvrPid.pfnDetectRawScrambleNotify = _MtDRecDetectRawScrambleCallback;
    }

    // Not not set scramble detect function

    if(!FVR_SetPid(u1Pidx, FVR_PID_FLAG_ALL, &rFvrPid))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_SetPidDecoderType
 *  Set record Decoder information
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SetPidDecoderType(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1TsIdx, u1Pidx;
    MTDREC_PID_TYPE_T eType;
	FVR_VIDEO_TYPE_T eVideoType;

    eVideoType = FVR_VIDEO_NONE;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    u1Pidx = (UINT8)rArg.ai4Arg[1];
    eType = (MTDREC_PID_TYPE_T)rArg.ai4Arg[2];

    if((eType == DREC_TYPE_VIDEO_MPEG2) || (eType == DREC_TYPE_VIDEO_MPEG4) ||
       (eType == DREC_TYPE_VIDEO_H264))
    {
	    switch(eType)
        {
        case DREC_TYPE_VIDEO_MPEG2:
            eVideoType = FVR_VIDEO_MPEG;
            break;
        case DREC_TYPE_VIDEO_MPEG4:
            Printf("MPEG4 Video Type??\n");
            break;
        case DREC_TYPE_VIDEO_H264:
            eVideoType = FVR_VIDEO_H264;
            break;
        default:
            Printf("Wrong Video Type\n");
            return MTR_NOT_OK;
        }

        if(!FVR_SetVideoTypeByPidx(u1Pidx, eVideoType))
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
       Printf("Wrong Video Decoder Type: %d\n", eType);
    }

    // we need to setup this flag if PVR is started in recorded list,
    // and we need to setup thumbnail PID in this case
    //_fgDRecStreamStop = FALSE;


    return MTR_OK;
}


static MT_RESULT_T _MTDREC_GetPid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1Pidx;
    UINT32 u4Flag;
    FVR_PID_T rFvrPid;
    MTDREC_PID_T *prPid = NULL;
    MTDREC_PID_T rPid;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    u4Flag = (UINT32)rArg.ai4Arg[1];
    prPid = (MTDREC_PID_T *)rArg.ai4Arg[2];

    x_memset((void*)&rFvrPid, 0, sizeof(FVR_PID_T));

    if(!FVR_GetPid(u1Pidx, u4Flag, &rFvrPid))
    {
        return MTR_NOT_OK;
    }

    rPid.fgEnable = rFvrPid.fgEnable;
    rPid.u2Pid = rFvrPid.u2Pid;
    rPid.ePidType = rFvrPid.ePidType;
    rPid.u1TsIndex = rFvrPid.u1TsIndex;

    USR_SPACE_ACCESS_VALIDATE_ARG(prPid, MTDREC_PID_T);
    COPY_TO_USER_ARG(prPid, rPid, MTDREC_PID_T);

    MTDREC_PRINT(" -u1Pidx = %u , u4Flags = 0x%x\n ", u1Pidx, u4Flag);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_FreePid
 *  Free record PID
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_FreePid(unsigned long arg)
{
    UINT8 u1Pidx;
    FVR_PID_T rFvrPid;
    INT32 Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    x_memset((void*)&rFvrPid, 0, sizeof(FVR_PID_T));
    FVR_GetPid(u1Pidx, FVR_PID_FLAG_ALL, &rFvrPid);

    if(!FVR_FreePid(u1Pidx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_UnlockBuffer
 *  Unlock record buffer to driver
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_UnlockBuffer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 i, u4Addr;
    UINT32 u4StreamAddr, u4StreamSize;
	UINT8 u1Idx;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	u1Idx  = (UINT8)rArg.ai4Arg[0];
    u4Addr = (UINT32)rArg.ai4Arg[1];

    //Printf("kernel, unlock: 0x%x\n", u4Addr);
    prDRecStreamStruct = _MtDRec_GetStreamStruct(u1Idx);

    for(i=0; i<MTDREC_STREAM_STRUCT_MAX; i++)
    {
        _MtDRec_Lock();
        if(!prDRecStreamStruct->arMtDRecStreamData[i].fgValid)
        {
            _MtDRec_Unlock();
            continue;
        }
        u4StreamAddr = prDRecStreamStruct->arMtDRecStreamData[i].u4Addr;
        u4StreamSize = prDRecStreamStruct->arMtDRecStreamData[i].u4Size;
        _MtDRec_Unlock();

        if(u4StreamAddr == u4Addr)
        {
            UINT32 fgStreamStop;

            _MtDRec_Lock();
            prDRecStreamStruct->arMtDRecStreamData[i].fgValid = FALSE;
            prDRecStreamStruct->arMtDRecStreamData[i].u4Addr = 0;
            prDRecStreamStruct->arMtDRecStreamData[i].u4Size = 0;
            prDRecStreamStruct->arMtDRecStreamData[i].u4IndexSize = 0;
            fgStreamStop = prDRecStreamStruct->fgDRecStreamStop;
            _MtDRec_Unlock();

            if(fgStreamStop == FALSE)
            {
                if(!FVR_UnlockBuffer(u1Idx, u4Addr, u4StreamSize))
                {
                    Printf("Not unlock buffer:0x%x size: 0x%x\n", u4Addr, u4StreamSize);
                }
            }
            break;
        }
    }

    return MTR_OK;
}



//-----------------------------------------------------------------------------
/** _MTDREC_Start
 *  Start recording
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_Start(unsigned long arg)
{
    FVR_TIMESTAMP_T rTimestamp;
    UINT8 i;
	UINT8 u1Idx;
    INT32 Arg;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, Arg);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Idx = (UINT8)Arg;
    prDRecStreamStruct = _MtDRec_GetStreamStruct(u1Idx);
    _MtDRec_Lock();
    prDRecStreamStruct->fgDRecStreamStop = TRUE;
    _MtDRec_Unlock();

    if(!FVR_Stop(u1Idx))
    {
        return MTR_NOT_OK;
    }

    if(FVR_FlushBuffer(u1Idx) != TRUE)
    {
        Printf("Fail to flush buffer\n");
    }

    rTimestamp.fgEnable = _fgMTDRecUseTimestamp;
    rTimestamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    rTimestamp.u4Counter = 0;
    rTimestamp.u4Mask = 0xFFFFFFFF;

    FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimestamp);

    prDRecStreamStruct->fgInitTime = FALSE;
    prDRecStreamStruct->u4MtDRecDefineBufSize = (eMtDrecTimestamp == MTDREC_TIMESTAMP_TYPE_4BYTE)?
            MTDREC_GETSTREAM192_LBR_SIZE:MTDREC_GETSTREAM188_LBR_SIZE;
     
    _MtDRec_Lock();
    prDRecStreamStruct->u4MtDRecReadPointer = prDRecStreamStruct->u4DRecStreamAddr;   // Set read pointer to buffer start
    prDRecStreamStruct->u4MtDRecWritePointer = prDRecStreamStruct->u4DRecStreamAddr;   // Set write pointer to buffer start
    prDRecStreamStruct->u4MtDRecPicWp = 0;
    prDRecStreamStruct->u4MtDRecPicRp = 0;
    prDRecStreamStruct->u1MtDRecStreamDataWp = 0;
    prDRecStreamStruct->u1MtDRecStreamDataRp = MTDREC_STREAM_STRUCT_MAX - 1; //0;
    prDRecStreamStruct->fgDRecStreamStop = FALSE;
    _MtDRec_Unlock();

    _MtDRec_Lock();
    for(i=0; i<MTDREC_STREAM_STRUCT_MAX; i++)
    {
        prDRecStreamStruct->arMtDRecStreamData[i].fgValid = FALSE;
        prDRecStreamStruct->arMtDRecStreamData[i].u4Addr = 0;
        prDRecStreamStruct->arMtDRecStreamData[i].u4IndexSize = 0;
        prDRecStreamStruct->arMtDRecStreamData[i].u4Size = 0;
    }

    for(i=0; i<MTDREC_STREAM_STRUCT_MAX; i++)
    {
        prDRecStreamStruct->arMtDRecStreamInfo[i].u4BitRate = 0;
        prDRecStreamStruct->arMtDRecStreamInfo[i].eFrameRate = MTDREC_PVR_FR_FORBIDDEN;
        prDRecStreamStruct->arMtDRecStreamInfo[i].fgFoundSeqSPS = FALSE;
        prDRecStreamStruct->arMtDRecStreamInfo[i].u4FrameRate = 0;
        prDRecStreamStruct->arMtDRecStreamInfo[i].u4SeqSPSOffset = 0;
        prDRecStreamStruct->arMtDRecStreamInfo[i].u4SPSNextStartLen = 0;
    }
    _MtDRec_Unlock();

    if(!FVR_Start(u1Idx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_Stop
 *  Stop record
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_Stop(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1Idx;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, Arg);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Idx = (UINT8)Arg;
    prDRecStreamStruct = _MtDRec_GetStreamStruct(u1Idx);

    _MtDRec_Lock();
    prDRecStreamStruct->fgDRecStreamStop = TRUE;
    _MtDRec_Unlock();

    if(!FVR_Stop(u1Idx))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_GetDownloadStreamInfo
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_GetDownloadStreamInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTDREC_STREAM_INFO_T *prInfo;
    MTDREC_STREAM_INFO_T rInfo;
    UINT8 u1Index;
	UINT8 u1TsIdx;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	u1TsIdx = (UINT8)rArg.ai4Arg[0];
    prInfo = (MTDREC_STREAM_INFO_T*)rArg.ai4Arg[1];

    if(!FVR_IsRecording(u1TsIdx))
    {
        return MTR_NOT_OK;
    }

    prDRecStreamStruct = _MtDRec_GetStreamStruct(u1TsIdx);

    _MtDRec_Lock();
    u1Index = prDRecStreamStruct->u1MtDRecStreamDataRp;
    prDRecStreamStruct->u1MtDRecStreamDataRp++;
    prDRecStreamStruct->u1MtDRecStreamDataRp %= MTDREC_STREAM_STRUCT_MAX;

    ASSERT(u1Index < MTDREC_STREAM_STRUCT_MAX);
    if(u1Index >= MTDREC_STREAM_STRUCT_MAX)
    {
        _MtDRec_Unlock();
        return MTR_NOT_OK;
    }

    rInfo = prDRecStreamStruct->arMtDRecStreamInfo[u1Index];
    _MtDRec_Unlock();

    MTDREC_PRINT("Return SPS: 0x%x, Frate: 0x%x, bRate: 0x%x\n", rInfo.fgFoundSeqSPS, rInfo.eFrameRate, rInfo.u4BitRate);

    USR_SPACE_ACCESS_VALIDATE_ARG(prInfo, MTDREC_STREAM_INFO_T);
    COPY_TO_USER_ARG(prInfo, rInfo, MTDREC_STREAM_INFO_T);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_GetDownloadStream
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_GetDownloadStream(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTDREC_MSG_T rMsg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	rMsg.u1Idx = (UINT8)rArg.ai4Arg[0];
    rMsg.u4Timeout = (UINT32)rArg.ai4Arg[1];

    if(!FVR_IsRecording(rMsg.u1Idx))
    {
        return MTR_NOT_OK;
    }

    if (x_msg_q_send(_hMtDrecQueue, (VOID*)&rMsg, sizeof(MTDREC_MSG_T), 128) != OSR_OK)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_StopStream
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_StopStream(unsigned long arg)
{
#if 0
    _MtDRec_Lock();
    prDRecStreamStruct->fgDRecStreamStop = TRUE;
    _MtDRec_Unlock();
#endif

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_SetCipher
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SetCipher(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTDREC_CRYPTO_INFO_T rInfo, *prInfo;
    UINT8 *pu1Key, *pu1IV;
    UINT8 au1Key[32], au1IV[16];
    BOOL fgEven = FALSE, fgSecureKey;
    UINT8 i;
    FVR_PID_T rFvrPid;
    MTDREC_CIPHER_ALGORM_T eCipherAlgo;
    FVR_CRYPTO_KEY_TYPE_T eFvrKeyType;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    prInfo = (MTDREC_CRYPTO_INFO_T*)rArg.ai4Arg[0];
    pu1IV = (UINT8*)rArg.ai4Arg[1];
    pu1Key = (UINT8*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prInfo, MTDREC_CRYPTO_INFO_T);
    COPY_FROM_USER_ARG(prInfo, rInfo, MTDREC_CRYPTO_INFO_T);

    if(rInfo.u4KeyLen > 256)
    {
        return MTR_NOT_OK;
    }

    _MtDRec_Lock();
    eCipherAlgo = _eMtDrecCipherAlgorithm;
    _MtDRec_Unlock();

    if (rInfo.eAlgorithm != eCipherAlgo)
    {
        MTDREC_PRINT("CipherAlgorithm is wrong! (%u != %u)\n", rInfo.eAlgorithm, eCipherAlgo);
        return MTR_NOT_OK;
    }

    switch (rInfo.eKeyType)
    {
    case MTDREC_CIPHER_KEY_EVEN:
        _rMTDRecKeyType = FVR_KEY_EVEN;
        fgEven = TRUE;
        fgSecureKey = FALSE;
        eFvrKeyType = FVR_CRYPTO_KEY_AES;
        break;
    case MTDREC_CIPHER_KEY_ODD:
        _rMTDRecKeyType = FVR_KEY_ODD;
        fgEven = FALSE;
        fgSecureKey = FALSE;
        eFvrKeyType = FVR_CRYPTO_KEY_AES;
        break;
    case MTDREC_CIPHER_SECURE_KEY0_EVEN:
        _rMTDRecKeyType = FVR_KEY_EVEN;
        fgEven = TRUE;
        fgSecureKey = TRUE;
        eFvrKeyType = FVR_CRYPTO_KEY_SECURE_PVR_AES_0;
        break;
    case MTDREC_CIPHER_SECURE_KEY0_ODD:
        _rMTDRecKeyType = FVR_KEY_ODD;
        fgEven = FALSE;
        fgSecureKey = TRUE;
        eFvrKeyType = FVR_CRYPTO_KEY_SECURE_PVR_AES_0;
        break;
    default:
        MTDREC_PRINT("keyType=%u is not supported!\n", rInfo.eKeyType);
        return MTR_NOT_OK;
    } 
    
    // Fix klocwork
    for(i=0; i<16; i++)
    {
        au1IV[i] = 0;
    }

    if(rInfo.eAlgorithm == MTDREC_CIPHER_AES_CBC && pu1IV != NULL)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1IV, 16);
        COPY_FROM_USER_ARG_SIZE_K(pu1IV, au1IV, 16);
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1Key, rInfo.u4KeyLen/8);
    COPY_FROM_USER_ARG_SIZE_K(pu1Key, au1Key, rInfo.u4KeyLen/8);

    if (!fgSecureKey)
    {
        if (!FVR_SetAesKey(MTDREC_CIPHER_KEY_INDEX, fgEven, au1Key))
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        if (!FVR_SetCryptoKey(MTDREC_CIPHER_KEY_INDEX, fgEven, eFvrKeyType, au1Key, rInfo.u4KeyLen/8))
        {
            return MTR_NOT_OK;
        }
    }

    if(!FVR_SetAesEvenOddIV(MTDREC_CIPHER_KEY_INDEX, fgEven, au1IV))
    {
        return MTR_NOT_OK;
    }

    _u4MtDRecKeyLen = rInfo.u4KeyLen;

    x_memset((void*)&rFvrPid, 0, sizeof(FVR_PID_T));

    if(rInfo.eAlgorithm == MTDREC_CIPHER_AES_CBC)
    {
        rFvrPid.fgCBCMode = TRUE;
        rFvrPid.fgScramble = TRUE;
    }
    else
    {
        rFvrPid.fgCBCMode = FALSE;
        rFvrPid.fgScramble = TRUE;
    }

    rFvrPid.u4KeyLen = rInfo.u4KeyLen;
    rFvrPid.eKeyType = _rMTDRecKeyType;
    rFvrPid.u1KeyIdx = MTDREC_CIPHER_KEY_INDEX;

    for(i=0; i<FVR_NUM_PID_INDEX; i++)
    {
        if(!FVR_SetPid(i, FVR_PID_FLAG_SCRAMBLE_ENABLE | FVR_PID_FLAG_KEY_LEN |
            FVR_PID_FLAG_CBC_MODE | FVR_PID_FLAG_KEY_INDEX, &rFvrPid))
        {
            return MTR_NOT_OK;
        }
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_SetConfig
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SetConfig(unsigned long arg)
{
    INT32 Arg;
    MTDREC_CONFIG_T *prConfig, rConfig;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    prConfig = (MTDREC_CONFIG_T*)Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prConfig, MTDREC_CONFIG_T);
    COPY_FROM_USER_ARG(prConfig, rConfig, MTDREC_CONFIG_T);

    _MtDRec_Lock();
    eMtDrecTimestamp = rConfig.eTimestamp;
    _eMtDrecCipherAlgorithm = rConfig.eAlgorithm;

    if(rConfig.eTimestamp == MTDREC_TIMESTAMP_TYPE_4BYTE)
    {
#if 0
        MTDREC_STREAM_T *prDRecStreamStruct = NULL;
        UINT8 u1TsIdx;

        for (u1TsIdx = 0; u1TsIdx < FVR_GBL_COUNT; u1TsIdx++)
        {
            prDRecStreamStruct = _MtDRec_GetStreamStruct(u1TsIdx);
            prDRecStreamStruct->u4MtDRecDefineBufSize = MTDREC_GETSTREAM192_LBR_SIZE;
        }
#endif
        _fgMTDRecUseTimestamp = TRUE;

        //_Mtpvr_SetPlayBackMode(ENUM_MTPVR_PLAYMODE_192);
    }
    else
    {
        // we don't want to use 188 bytes for recording
        /*
        _fgMTDRecUseTimestamp = FALSE;
        _u4MtDRecDefineBufSize = MTDREC_GETSTREAM188_LBR_SIZE;
        */
        //_Mtpvr_SetPlayBackMode(ENUM_MTPVR_PLAYMODE_188);
    }
    // set pid will call it in io_mtdmx.c
    //_MtdmxSetDecoder();

    _MtDRec_Unlock();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_GetTotalDownloadBufferSize
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_GetTotalDownloadBufferSize(unsigned long arg)
{
    UINT8 u1TsIdx;
    UINT32 *pu4Size;
    MTAL_IOCTL_2ARG_T rArg;
    MTDREC_STREAM_T *prDRecStreamStruct = NULL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    pu4Size = (UINT32*)rArg.ai4Arg[1];
    prDRecStreamStruct = _MtDRec_GetStreamStruct(u1TsIdx);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
    COPY_TO_USER_ARG(pu4Size, prDRecStreamStruct->u4DRecStreamSize, UINT32);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_SetDescramblPid
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SetDescramblPid(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UINT8 u1Pidx;
    MTDMX_DESC_MODE_T eDescMode;
    UINT8 u1KeyIndex;
    FVR_PID_T rPid;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    eDescMode = (MTDMX_DESC_MODE_T)rArg.ai4Arg[1];
    u1KeyIndex = (UINT8)rArg.ai4Arg[2];

    MTDREC_PRINT(" - u1Pidx=%d, eDescMode=%d, u1KeyIndex=%d\n",
                        (int)u1Pidx, (int)eDescMode, (int)u1KeyIndex);

    rPid.fgDesc = TRUE;
    rPid.eDescMode = (DMX_DESC_MODE_T)eDescMode;
    rPid.u1DescKeyidx = u1KeyIndex;
	if(!FVR_SetPid(u1Pidx, FVR_PID_FLAG_DESC, &rPid))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get scramble state
 *
 *  @param  u1Pidx          PID index, 0 ~ 63
 *  @return     The scramble state
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_GetScrambleState(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx ;
    MTDREC_SCRAMBLE_STATE_T *peScramble_State = NULL;
    MTDREC_SCRAMBLE_STATE_T eScramble_State;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    peScramble_State = (MTDREC_SCRAMBLE_STATE_T*)rArg.ai4Arg[1];

    eScramble_State = (MTDMX_SCRAMBLE_STATE_T)FVR_GetScrambleState(u1Pidx);

    USR_SPACE_ACCESS_VALIDATE_ARG(peScramble_State, MTDREC_SCRAMBLE_STATE_T);
    COPY_TO_USER_ARG(peScramble_State, eScramble_State, MTDREC_SCRAMBLE_STATE_T);

    MTDREC_PRINT(" -u1Pidx = %d, eScramble_State = %d\n",u1Pidx,eScramble_State);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/*
 *  Get raw scramble state
 *
 *  @param  u1Pidx          PID index, 0 ~ 63
 *  @return     The scramble state
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_GetRawScrambleState(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT8 u1Pidx ;
    MTDREC_SCRAMBLE_STATE_T *peScramble_State = NULL;
    MTDREC_SCRAMBLE_STATE_T eScramble_State;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    peScramble_State = (MTDREC_SCRAMBLE_STATE_T*)rArg.ai4Arg[1];

    eScramble_State = (MTDMX_SCRAMBLE_STATE_T)FVR_GetRawScrambleState(u1Pidx);

    USR_SPACE_ACCESS_VALIDATE_ARG(peScramble_State, MTDREC_SCRAMBLE_STATE_T);
    COPY_TO_USER_ARG(peScramble_State, eScramble_State, MTDREC_SCRAMBLE_STATE_T);

    MTDREC_PRINT(" -u1Pidx = %d, RawScramble_State = %d\n",u1Pidx,eScramble_State);
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDREC_SitSetPattern
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SitSetPattern(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 u1TsIdx;
    UINT8 *pu1UsrData, *pu1Data = NULL;
    UINT16 u2Pid, u2Len;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    u2Pid = (UINT16)rArg.ai4Arg[1];
    pu1UsrData = (UINT8 *)rArg.ai4Arg[2];
    u2Len = (UINT16)rArg.ai4Arg[3];

    pu1Data = (UINT8 *)x_mem_alloc(u2Len);
    if (!pu1Data)
    {
        MTDREC_PRINT("Fail to alloc SitBuf, pData=%08x, Len=%d\n", pu1UsrData, u2Len);
        eRet = MTR_NOT_OK;
        goto lbExit1;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1UsrData, u2Len);
    COPY_FROM_USER_ARG_SIZE(pu1UsrData, pu1Data, u2Len);

    if (!FVR_SitSetPattern(u1TsIdx, u2Pid, pu1Data, u2Len))
    {
        MTDREC_PRINT("Fail to set SitPattern, TsIdx=%d, Pid=%d, pData=%08x, Len=%d\n",
                    u1TsIdx, u2Pid, pu1UsrData, u2Len);
        eRet = MTR_NOT_OK;
        goto lbExit1;
    }

lbExit1:
    if (pu1Data)
    {
        x_mem_free((VOID*)pu1Data);
    }

    return eRet;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SitFreePattern
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SitFreePattern(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1TsIdx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1TsIdx = (UINT8)Arg;

    if (!FVR_SitFreePattern(u1TsIdx))
    {
        MTDREC_PRINT("Fail to free SitPattern, TsIdx=%d\n", u1TsIdx);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SitInsertPattern
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SitInsertPattern(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1TsIdx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1TsIdx = (UINT8)Arg;

    if (!FVR_SitInsertPattern(u1TsIdx))
    {
        MTDREC_PRINT("Fail to insert pattern, TsIdx=%d\n", u1TsIdx);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SitInsertPatternByTimer
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SitInsertPatternByTimer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
	UINT8 u1TsIdx;
    UINT32 u4MsDelay;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1TsIdx = (UINT8)rArg.ai4Arg[0];
    u4MsDelay = (UINT32)rArg.ai4Arg[1];

    if (!FVR_SitInsertPatternByTimer(u1TsIdx, u4MsDelay))
    {
        MTDREC_PRINT("Fail to set up timer, TsIdx=%d, MsDelay=%u\n", u1TsIdx, u4MsDelay);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SitStopTimer
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SitStopTimer(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1TsIdx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1TsIdx = (UINT8)Arg;

    if (!FVR_SitStopTimer(u1TsIdx))
    {
        MTDREC_PRINT("Fail to stop timer, TsIdx=%d\n", u1TsIdx);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SwapSetPattern
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SwapSetPattern(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1Pidx;
    UINT8 *pu1UsrData, *pu1Data = NULL;
    UINT16 u2Len;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    pu1UsrData = (UINT8 *)rArg.ai4Arg[1];
    u2Len = (UINT16)rArg.ai4Arg[2];

    pu1Data = (UINT8 *)x_mem_alloc(u2Len);
    if (!pu1Data)
    {
        MTDREC_PRINT("Fail to alloc SwapBuf, pData=%08x, Len=%d\n", pu1UsrData, u2Len);
        eRet = MTR_NOT_OK;
        goto lbExit1;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pu1UsrData, u2Len);
    COPY_FROM_USER_ARG_SIZE(pu1UsrData, pu1Data, u2Len);

    if (!FVR_SwapSetPattern(u1Pidx, pu1Data, u2Len))
    {
        MTDREC_PRINT("Fail to set SwapPattern, Pidx=%d, pData=%08x, Len=%d\n",
                    u1Pidx, pu1UsrData, u2Len);
        eRet = MTR_NOT_OK;
        goto lbExit1;
    }

lbExit1:
    if (pu1Data)
    {
        x_mem_free((VOID*)pu1Data);
    }

    return eRet;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SwapFreePattern
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SwapFreePattern(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1Pidx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    if (!FVR_SwapFreePattern(u1Pidx))
    {
        MTDREC_PRINT("Fail to free SwapPattern, Pidx=%d\n", u1Pidx);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_SwapReplacePattern
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_SwapReplacePattern(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    FVR_PID_T rFvrPid;
    UINT8 u1Pidx;
    BOOL fgEnabled;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1Pidx = (UINT8)rArg.ai4Arg[0];
    fgEnabled = (BOOL)rArg.ai4Arg[1];

    rFvrPid.fgPktSwap = fgEnabled;
    rFvrPid.fgPktSwapOnce = FALSE;
    rFvrPid.pfnSwapNotify = NULL;
    rFvrPid.pvSwapNotifyTag = NULL;

    if (!FVR_SetPid(u1Pidx, FVR_PID_FLAG_PKTSWAP, &rFvrPid))
    {
        MTDREC_PRINT("Fail to perform swap, Pidx=%d, Enable=%s\n", u1Pidx, fgEnabled?"TRUE":"FALSE");
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_StartInsertDIT
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_StartInsertDIT(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1Pidx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    if (!FVR_StartInsertDIT(u1Pidx, 500 /*ms*/))
    {
        MTDREC_PRINT("Fail to start inserting DIT, Pidx=%d\n", u1Pidx);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTDREC_StopInsertDIT
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDREC_StopInsertDIT(unsigned long arg)
{
    INT32 Arg;
	UINT8 u1Pidx;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    u1Pidx = (UINT8)Arg;

    if (!FVR_StopInsertDIT(u1Pidx))
    {
        MTDREC_PRINT("Fail to stop inserting DIT, Pidx=%d\n", u1Pidx);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}



//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
int mtal_ioctl_mtdrec(unsigned int cmd, unsigned long arg)
{
    INT32 i4Ret = MTR_OK;

    switch (cmd)
    {
    case MTAL_IO_DMX_DREC_INIT:
        i4Ret = _MTDREC_Init(arg);
        break;

    case MTAL_IO_DMX_DREC_SETPID:
        i4Ret = _MTDREC_SetPid(arg);
        break;

    case MTAL_IO_DMX_DREC_GETPID:
        i4Ret = _MTDREC_GetPid(arg);
        break;

    case MTAL_IO_DMX_DREC_FREEPID:
        i4Ret = _MTDREC_FreePid(arg);
        break;

    case MTAL_IO_DMX_DREC_UNLOCK_BUF:
        i4Ret = _MTDREC_UnlockBuffer(arg);
        break;

    case MTAL_IO_DMX_DREC_START:
        i4Ret = _MTDREC_Start(arg);
        break;

    case MTAL_IO_DMX_DREC_STOP:
        i4Ret = _MTDREC_Stop(arg);
        break;

    case MTAL_IO_DMX_DREC_GETSTREAMINFO:
        i4Ret = _MTDREC_GetDownloadStreamInfo(arg);
        break;

    case MTAL_IO_DMX_DREC_GETSTREAM:
        i4Ret = _MTDREC_GetDownloadStream(arg);
        break;

    case MTAL_IO_DMX_DREC_STOPSTREAM:
        i4Ret = _MTDREC_StopStream(arg);
        break;

    case MTAL_IO_DMX_DREC_GETBUFFERPTR:
        i4Ret = _MTDREC_GetBufferPtr(arg);
        break;

    case MTAL_IO_DMX_DREC_ALLOCBUFFERPTR:
        i4Ret = _MTDREC_AllocBuffer(arg);
        break;

    case MTAL_IO_DMX_DREC_FREEBUFFERPTR:
        i4Ret = _MTDREC_FreeBuffer(arg);
        break;

    case MTAL_IO_DMX_DREC_SETCIPHER:
        i4Ret = _MTDREC_SetCipher(arg);
        break;

    case MTAL_IO_DMX_DREC_SETCONFIG:
        i4Ret = _MTDREC_SetConfig(arg);
        break;

    case MTAL_IO_DMX_DREC_GETTOTALDNBUFFERSIZE:
        i4Ret = _MTDREC_GetTotalDownloadBufferSize(arg);
        break;

    case MTAL_IO_DMX_DREC_SETDESCPID:
        i4Ret = _MTDREC_SetDescramblPid(arg);
        break;

    case MTAL_IO_DMX_DREC_SETPID_DECODETYPE:
		i4Ret = _MTDREC_SetPidDecoderType(arg);
		break;

    case MTAL_IO_DMX_DREC_GETSCRMSTE:
        i4Ret = _MTDREC_GetScrambleState(arg);
        break;

    case MTAL_IO_DMX_DREC_GETRAWSCRMSTE:
        i4Ret = _MTDREC_GetRawScrambleState(arg);
        break;

    case MTAL_IO_DMX_DREC_SITSETPAT:
        i4Ret = _MTDREC_SitSetPattern(arg);
        break;

    case MTAL_IO_DMX_DREC_SITFREEPAT:
        i4Ret = _MTDREC_SitFreePattern(arg);
        break;

    case MTAL_IO_DMX_DREC_SITINSPAT:
        i4Ret = _MTDREC_SitInsertPattern(arg);
        break;

    case MTAL_IO_DMX_DREC_SITINSPATBYTIME:
        i4Ret = _MTDREC_SitInsertPatternByTimer(arg);
        break;

    case MTAL_IO_DMX_DREC_SITSTOPTIMER:
        i4Ret = _MTDREC_SitStopTimer(arg);
        break;

    case MTAL_IO_DMX_DREC_SWAPSETPAT:
        i4Ret = _MTDREC_SwapSetPattern(arg);
        break;

    case MTAL_IO_DMX_DREC_SWAPFREEPAT:
        i4Ret = _MTDREC_SwapFreePattern(arg);
        break;

    case MTAL_IO_DMX_DREC_SWAPREPPAT:
        i4Ret = _MTDREC_SwapReplacePattern(arg);
        break;

    case MTAL_IO_DMX_DREC_STARTINSDIT:
        i4Ret = _MTDREC_StartInsertDIT(arg);
        break;

    case MTAL_IO_DMX_DREC_STOPINSDIT:
        i4Ret = _MTDREC_StopInsertDIT(arg);
        break;

    default:
        i4Ret = MTR_NOT_SUPPORTED;
        break;
    }

    return i4Ret;
}

#if 0
//-----------------------------------------------------------------------------
/** _MtDRecFreeThumbnailPid
 */
//-----------------------------------------------------------------------------
BOOL _MtDRecFreeThumbnailPid(UINT16 u2Pid, BOOL fgForce)
{
    DMX_PID_T rDrvPid;

    // If thumbnail PID is not used, we can't free thumbnail PID
    if (_u1DRecThumbnailPidx == DMX_MULTIPLE_NULL_PIDX)
    {
        return TRUE;
    }

    if(fgForce)
    {
        Printf("Force to free a thumbnail video PID\n");
        DMX_FreePid(_u1DRecThumbnailPidx);
        VDEC_Stop(ES1);
        _MTPVR_ChangeThumbVdecId(ES0);
        _MtDRec_Lock();
        _ucThumbVdecId = ES0;
        _MtDRec_Unlock();
    }
    else
    {
        DMX_GetPid(_u1DRecThumbnailPidx, MTDMX_PID_FLAG_ALL, &rDrvPid);
        if((rDrvPid.fgEnable == TRUE) && (rDrvPid.u2Pid == u2Pid))
        {
            Printf("Free a specific thumbnail video PID\n");
            DMX_FreePid(_u1DRecThumbnailPidx);

            // isaac
            VDEC_Stop(ES1);
            _MTPVR_ChangeThumbVdecId(ES0);
            _MtDRec_Lock();
            _ucThumbVdecId = ES0;
            _MtDRec_Unlock();
        }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MtDRecSetThumbnailPid
 */
//-----------------------------------------------------------------------------
BOOL _MtDRecSetThumbnailPid(BOOL fgEnable)
{
    MTDMX_PID_T rPid;
    BOOL fgRet = TRUE;
    ENUM_VDEC_FMT_T eVdecFmt;
    DMX_VIDEO_TYPE_T eVideoType;
	UINT8 u1Inst = 0;

    // It is not recording time
    if(_fgDRecStreamStop)
    {
        fgRet = TRUE;
        goto lbExit1;
    }

    x_memset(&rPid, 0, sizeof(MTDMX_PID_T));

    if(fgEnable)
    {
        // free the original thumbnail pidx & set the current pid
        _MtDRecFreeThumbnailPid(0, TRUE);

        if (_u1DRecThumbnailPidx == DMX_MULTIPLE_NULL_PIDX)
        {
            u1Inst = DMX_MUL_GetAvailableInst(DMX_CONN_TYPE_TUNER_0);
            if (u1Inst == DMX_MULTIPLE_NULL_INSTANCE)
            {
                fgRet = FALSE;
                goto lbExit1;
            }

            _u1DRecThumbnailPidx = DMX_MUL_GetAvailablePidx(u1Inst);
            if (_u1DRecThumbnailPidx == DMX_MULTIPLE_NULL_PIDX)
            {
                fgRet = FALSE;
                goto lbExit1;
            }
        }

        rPid.fgEnable = fgEnable;
        rPid.u2Pid = _u2MtDRecVideoPid;
        rPid.ePidType = MTDMX_PID_TYPE_ES_VIDEO;
        rPid.u1TsIndex = u4Record_Idx;

        fgRet = MTDMX_SetSecondVideoPid(_u1DRecThumbnailPidx, MTDMX_PID_FLAG_ALL, &rPid);
        if (!fgRet)
        {
            Printf("%s - fails to set thumbnail pidx %d(PID: 0x%x)",
                    __FUNCTION__, _u1DRecThumbnailPidx, _u2MtDRecVideoPid);
            _u1DRecThumbnailPidx = DMX_MULTIPLE_NULL_PIDX;
            fgRet = FALSE;
            goto lbExit1;
        }

        if(_eMtDrecVideoType == DREC_TYPE_VIDEO_MPEG2)
        {
            eVideoType = DMX_VIDEO_MPEG;
        }
        else if(_eMtDrecVideoType == DREC_TYPE_VIDEO_H264)
        {
            eVideoType = DMX_VIDEO_H264;
        }
        else if(_eMtDrecVideoType == DREC_TYPE_VIDEO_AVS)
        {
            eVideoType = DMX_VIDEO_AVS;
        }
        else
        {
            Printf("Unknow video format(%d)\n", _eMtDrecVideoType);
            goto lbExit1;
        }

        DMX_SetVideoTypeByPidx(_u1DRecThumbnailPidx, eVideoType);

        // call a Isaac function
        if(_eMtDrecVideoType == DREC_TYPE_VIDEO_MPEG2)
        {
            eVdecFmt = VDEC_FMT_MPV;
        }
        else if(_eMtDrecVideoType == DREC_TYPE_VIDEO_H264)
        {
            eVdecFmt = VDEC_FMT_H264;
        }
        else if(_eMtDrecVideoType == DREC_TYPE_VIDEO_AVS)
        {
            eVdecFmt = VDEC_FMT_AVS;
        }
        else
        {
            Printf("Unknow video format(%d)\n", _eMtDrecVideoType);
            goto lbExit1;
        }

        VDEC_PlayNotDisplay(ES1, TRUE);
        VDEC_SetMMParam(ES1, VDEC_MM_PARSING_INFO, (UINT32)TRUE, 0, 0);
        VDEC_Play(ES1, eVdecFmt);
        _MTPVR_ChangeThumbVdecId(ES1);
        _MtDRec_Lock();
        _ucThumbVdecId = ES1;
        _MtDRec_Unlock();
    }
    else
    {
        rPid.fgEnable = fgEnable;
        rPid.u2Pid = _u2MtDRecVideoPid;
        rPid.ePidType = MTDMX_PID_TYPE_ES_VIDEO;

        fgRet = MTDMX_SetSecondVideoPid(_u1DRecThumbnailPidx, MTDMX_PID_FLAG_VALID, &rPid);
        if(fgRet)
        {
            _u1DRecThumbnailPidx = DMX_MULTIPLE_NULL_PIDX;
        }
        else
        {
            Printf("%s - fails to disable thumbnail pidx %d(PID: 0x%x)",
                    __FUNCTION__, _u1DRecThumbnailPidx, _u2MtDRecVideoPid);
        }
    }

lbExit1:
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MTDREC_NotifyThumbnailChecking
 */
//-----------------------------------------------------------------------------
BOOL MTDREC_NotifyThumbnailChecking(void)
{
    UINT32 i;

    FVR_PID_T rFvrPid;
    DMX_PID_T rPid;

    // We will not control thumbnail pid if ES1 is occupied
    if(_MTAL_DMX_IsES1Used())
    {
        return TRUE;
    }

    // query record video pid
    for(i =0; i < FVR_NUM_PID_INDEX; i++)
    {
        FVR_GetPid(i, FVR_PID_FLAG_ALL, &rFvrPid);
        if((rFvrPid.fgEnable == TRUE) && (rFvrPid.fgPicSearch == TRUE))
        {
            MTDREC_PRINT("found a recorded video pid %d\n", rFvrPid.u2Pid);
            // this is a video PID
            break;
        }
    }

    // there is no video record PID, free the additional thumbnail video pid
    if(i >= FVR_NUM_PID_INDEX)
    {
        MTDREC_PRINT("there is no video record PID, free thumbnail pid\n");
        _MtDRecFreeThumbnailPid(0, TRUE);
    }

    // check if the record video pid is already playing, except ts index 2
    for(i =0; i < DMX_NUM_PID_INDEX; i++)
    {
        DMX_GetPid(i, FVR_PID_FLAG_ALL, &rPid);
        if((rPid.fgEnable == TRUE) && (rPid.u2Pid == rFvrPid.u2Pid) &&
            (rPid.u1TsIndex == u4Record_Idx) && (rPid.ePidType == DMX_PID_TYPE_ES_VIDEO))
        {
            // the video pid is already set
            MTDREC_PRINT("the playing video pid is already set\n");
            if ((_u1DRecThumbnailPidx != DMX_MULTIPLE_NULL_PIDX) && (i != _u1DRecThumbnailPidx))
            {
                // we already has another playing video pid, free thumbnail one
                _MtDRecFreeThumbnailPid(0, TRUE);
            }
            break;
        }
    }

    // set the thumbnail video pid
    if(i >= DMX_NUM_PID_INDEX)
    {
        MTDREC_PRINT("set the thumbnail video pid.\n");
        _MtDRecSetThumbnailPid(TRUE);
    }

    return TRUE;
}
#endif

BOOL _MTDREC_IsLivePidEnable(void)
{
    DMX_PID_T rPid;
    UINT8 i;

    // check if the record video pid is already playing, except ts index 2
    for(i =0; i < DMX_NUM_PID_INDEX; i++)
    {
        DMX_GetPid(i, FVR_PID_FLAG_ALL, &rPid);
        if((rPid.fgEnable == TRUE) &&
            (rPid.u1TsIndex == 2) && (rPid.ePidType == DMX_PID_TYPE_ES_VIDEO))
        {
            return FALSE;
        }
    }

    return TRUE;
}

#endif // TIME_SHIFT_SUPPORT

