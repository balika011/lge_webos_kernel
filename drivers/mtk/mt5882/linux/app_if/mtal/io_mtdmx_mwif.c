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

/** @file io_mtdmx.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "mtdmx_mwif.h"
#include "dmx_if.h"
#include "dmx_mm_if.h"
#include "fvr_if.h"
#include "fbm_drvif.h"
#include "mpv_drvif.h"
#include "vdec_drvif.h"
#include "aud_drvif.h"
#include "stc_drvif.h"
#include "cb_data.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>
#include "tz_if.h"
#include "x_assert.h"
#include "mtskb.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX     0xFF

#define PRINTK(a, ...)
//#define PRINTK printk
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    BOOL fgValid;
    DMX_CONN_TYPE_T eConType;
    UINT8 u1VirtualPidx;
} MTAL_DMX_PID_INFO_T;

//-----------------------------------------------------------------------------
// Extern
//-----------------------------------------------------------------------------
extern MTAL_DMX_PID_INFO_T _arDmxInfo[DMX_NUM_PID_INDEX];


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
extern UINT8 _MTAL_DMX_PhysicalPidx(UINT8 u1VirtualPidx);

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)       \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,   \
                       (size)))                             \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 1\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 2\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 3\n");      \
        return MTR_ERR_INV;                                          \
    }     \

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 4\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, (void *)ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error 5\n");      \
        return MTR_ERR_INV;                                          \
    } \


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _u4TsRecPktCount = 0;
static UINT32 _au4PatterBufAddr[FVR_NUM_PID_INDEX];

#ifdef CC_ENABLE_HCI
#if 0
static binstr_t *hci_BinstrAlloc(np_size_t in_size, const u_int8_t *in_buf)
{
    binstr_t *bs = NULL;

    /* check */
    if ( in_size > (UINT_MAX - sizeof(binstr_t)) )
        return NULL;

    /* alloc */
    bs = x_mem_alloc(in_size + sizeof(binstr_t));
    if ( NULL != bs )
    {
        bs->len = in_size;
        bs->buf = (u_int8_t*)bs + sizeof(binstr_t);

        /* copy */
        if ( in_buf != NULL )
        {
            x_memcpy( bs->buf, in_buf, in_size );
        }
        else
        {
            x_memset( bs->buf, 0, in_size );
        }
    }

    return bs;
}


static void hci_BinstrFree(binstr_t *io_str)
{
    if (io_str)
    {
        x_mem_free(io_str);
    }
}
#endif

#endif // CC_ENABLE_HCI

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static BOOL _MtDmxMwifCallback(UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
                               UINT32 u4Data, const void* pvNotifyTag)
{
    DMX_NOTIFY_INFO_PSI_T* prPSIInfo;
    DMX_NOTIFY_INFO_PES_T* prPESInfo;
    DMX_NOTIFY_INFO_PES_TIME_T* prPESTimeInfo;
    DMX_MTAL_CB_T rCB;
    UINT8 u1VirPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    // Check if PID is really active
    ASSERT(u1Pidx < DMX_NUM_PID_INDEX);

    u1VirPidx = _arDmxInfo[u1Pidx].u1VirtualPidx;
    if (u1VirPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1VirPidx error\n");
        return FALSE;
    }
    else
    {
        rCB.u1Pidx = u1VirPidx;
    }

    rCB.type = eCode;

    // Check notification code
    switch (eCode)
    {
    case DMX_NOTIFY_CODE_OVERFLOW:
        rCB.u4Data0 = u4Data;
        break;

    case DMX_NOTIFY_CODE_SCRAMBLE_STATE:
        rCB.u4Data0 = u4Data;
        break;

    case DMX_NOTIFY_CODE_PSI:
        prPSIInfo = (DMX_NOTIFY_INFO_PSI_T*)u4Data;
        ASSERT(prPSIInfo != NULL);

        rCB.u4Data0 = prPSIInfo->u1SerialNumber;
        rCB.u4Data1 = prPSIInfo->u4SecAddr;
        rCB.u4Data2 = prPSIInfo->u4SecLen;
        // make klocwork happy
        //x_memcpy(&rCB.u4Data3, prPSIInfo->au4MatchBitmap, SEC_CNT * sizeof(UINT32));
        if (SEC_CNT >= 1)
        {
            rCB.u4Data3 = prPSIInfo->au4MatchBitmap[0];
        }
        if (SEC_CNT >= 2)
        {
            rCB.u4Data4 = prPSIInfo->au4MatchBitmap[1];
        }
        if (SEC_CNT >= 3)
        {
            rCB.u4Data5 = prPSIInfo->au4MatchBitmap[2];
        }
        break;

    case DMX_NOTIFY_CODE_PES:
        prPESInfo = (DMX_NOTIFY_INFO_PES_T*)u4Data;
        ASSERT(prPESInfo != NULL);

        rCB.u4Data0 = prPESInfo->u1SerialNumber;
        rCB.u4Data1 = prPESInfo->u4DataAddr;
        rCB.u4Data2 = prPESInfo->u4DataSize;
        break;

    case DMX_NOTIFY_CODE_PES_TIME:
        prPESTimeInfo = (DMX_NOTIFY_INFO_PES_TIME_T*)u4Data;
        ASSERT(prPESTimeInfo != NULL);

        rCB.u4Data0 = prPESTimeInfo->u1SerialNumber;
        rCB.u4Data1 = prPESTimeInfo->u4DataAddr;
        rCB.u4Data2 = prPESTimeInfo->u4DataSize;
        rCB.u4Data3 = prPESTimeInfo->u4Pts;
        rCB.u4Data4 = prPESTimeInfo->u4Dts;
        break;

    default:
        // Unexpected notification
        ASSERT(0);
        break;
    }

    UNUSED(_CB_PutEvent(CB_MTAL_DMX_TRIGGER, sizeof (DMX_MTAL_CB_T), &rCB));

    return TRUE;
}

static void _MtFvrMwifPktSwap(UINT8 u1Pidx, UINT32 u4Addr)
{
    // don't use
}

static void _MtFvrMwifPicSearch(UINT8 u1Pidx, UINT16 u2Type, UINT32 u4PktCount,
                                UINT64 u8Pts33, const void* pvTag)
{
    FVR_MTAL_CB_T rCB;

    rCB.type = MTFVR_GBL_NFY_OTHER;
    rCB.u4Data0 = u1Pidx;
    rCB.u4Data1 = u2Type;
    rCB.u4Data2 = u4PktCount;

    UNUSED(_CB_PutEvent(CB_MTAL_DMX_MWIF_TRIGGER, sizeof (FVR_MTAL_CB_T), &rCB));
}

static BOOL _MtFvrMwifDetectScramble(UINT8 u1Pidx, FVR_SCRAMBLE_STATE_T eState)
{
    FVR_MTAL_CB_T rCB;

    rCB.type = MTFVR_GBL_NFY_SCRAMBLE;
    rCB.u4Data0 = u1Pidx;
    rCB.u4Data1 = eState;

    UNUSED(_CB_PutEvent(CB_MTAL_DMX_MWIF_TRIGGER, sizeof (FVR_MTAL_CB_T), &rCB));

    return TRUE;
}

static BOOL _MtFvrMwifDetectRawScramble(UINT8 u1Pidx, FVR_SCRAMBLE_STATE_T eState)
{
    FVR_MTAL_CB_T rCB;

    rCB.type = MTFVR_GBL_NFY_RAW_SCRAMBLE;
    rCB.u4Data0 = u1Pidx;
    rCB.u4Data1 = eState;

    UNUSED(_CB_PutEvent(CB_MTAL_DMX_MWIF_TRIGGER, sizeof (FVR_MTAL_CB_T), &rCB));

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _mtTsRecTimerCallback
 */
//-----------------------------------------------------------------------------
static void _MtfvrTimerCallback(FVR_MTAL_CB_T *prCB, FVR_GBL_TIMENOTIFY_INFO_T *prTimeInfo)
{
    prCB->u4Data0 = PHYSICAL(prTimeInfo->u4Addr);
    prCB->u4Data1 = prTimeInfo->u4Size;
    prCB->u4Data2 = prTimeInfo->u4PktCount;
	prCB->u4Data3 = prTimeInfo->u1TsIndex;
}


//-----------------------------------------------------------------------------
/** _mtTsRecOverflowCallback
 */
//-----------------------------------------------------------------------------
static void _MtfvrRecOverflowCallback(FVR_MTAL_CB_T *prCB, UINT8 u1TsIdx)
{
    if (!FVR_Stop(u1TsIdx))
    {
        MTDMX_PRINT("%s, FVR stop error\n", __FUNCTION__);
    }

    FVR_SetIsOverflow(u1TsIdx, TRUE);    // DEBUG purpose

    prCB->u4Data3 = u1TsIdx;
}

static void _MtfvrGBLCallback(FVR_GBL_NOTIFY_CODE_T eCode,
                              UINT32 u4Data, const void* pvTag)
{
    FVR_MTAL_CB_T rCB;

    switch (eCode)
    {
    case FVR_GBL_NFY_TIME:
        rCB.type = MTFVR_GBL_NFY_TIME;
        _MtfvrTimerCallback(&rCB, (FVR_GBL_TIMENOTIFY_INFO_T*)u4Data);
        break;

    case FVR_GBL_NFY_PKTCOUNT:
        // Don't use
        ASSERT(0);
        break;

    case FVR_GBL_NFY_OVERFLOW:
        rCB.type = MTFVR_GBL_NFY_OVERFLOW;
        _MtfvrRecOverflowCallback(&rCB, (UINT8)u4Data);
        break;

    default:
        // Unexpected
        ASSERT(0);
        break;
    }

    UNUSED(_CB_PutEvent(CB_MTAL_DMX_MWIF_TRIGGER, sizeof (FVR_MTAL_CB_T), &rCB));
}

static BOOL _MTDMX_MUL_FreePidx(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet;

    fgRet = DMX_MUL_FreePidx(prKernParam->u1Pidx);

    return fgRet;
}

static BOOL _MTDMX_MUL_SetPidxInstance(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet;

    fgRet = DMX_MUL_SetPidxInstance(prKernParam->u1Inst, prKernParam->u1Pidx);

    return fgRet;
}

static BOOL _MTDMX_MUL_GetAvailableInst(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet = TRUE;

    prKernParam->u1Inst = DMX_MUL_GetAvailableInst(prKernParam->eConnType);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u1Inst, sizeof(prUserParam->u1Inst));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u1Inst, &prKernParam->u1Inst, sizeof(prUserParam->u1Inst));

    return fgRet;
}

static BOOL _MTDMX_MUL_GetTsIndex(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet = TRUE;

    prKernParam->u1TsIdx = DMX_MUL_GetTSIdx(prKernParam->u1Inst);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u1TsIdx, sizeof(prUserParam->u1TsIdx));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u1TsIdx, &prKernParam->u1TsIdx, sizeof(prUserParam->u1TsIdx));

    return fgRet;
}

static BOOL _MTDMX_MUL_FreeInst(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet;

    fgRet = DMX_MUL_FreeInst(prKernParam->u1Inst);

    return fgRet;
}

static BOOL _MTDMX_MUL_GetPidxInstId(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet = TRUE;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    prKernParam->u1Inst = DMX_MUL_GetPidxInstId(u1Pidx);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u1Inst,sizeof(prUserParam->u1Inst));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u1Inst,&prKernParam->u1Inst,sizeof(prUserParam->u1Inst));

    return fgRet;
}

static BOOL _MTDMX_MUL_SetSubtitlePidx(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    fgRet = DMX_MUL_SetSubtitlePidx(prKernParam->u1Inst,u1Pidx);

    return fgRet;
}

static BOOL _MTDMX_MM_Set(MTDMX_MWIF_MM_T *prKernParam, MTDMX_MWIF_MM_T *prUserParam)
{
    DMX_MM_T rMM;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->prMMStruct, sizeof(rMM));
    COPY_FROM_USER_ARG_SIZE(prKernParam->prMMStruct, &rMM, sizeof(rMM));

    if ((rMM.pfnCallback != NULL) && (prKernParam->u4Flags & MTDMX_MM_FLAG_NOTIFY))
    {
        rMM.pfnCallback = _MtDmxMwifCallback;
    }
    else
    {
        rMM.pfnCallback = NULL;
    }

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    return DMX_MM_Set(u1Pidx, prKernParam->u4Flags, &rMM);
}

static BOOL _MTDMX_MM_Get(MTDMX_MWIF_MM_T *prKernParam, MTDMX_MWIF_MM_T *prUserParam)
{
    DMX_MM_T rMM;
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    fgRet = DMX_MM_Get(u1Pidx, prKernParam->u4Flags, &rMM);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prMMStruct, sizeof(rMM));
    COPY_TO_USER_ARG_SIZE(prUserParam->prMMStruct, &rMM, sizeof(rMM));

    return fgRet;
}


static BOOL _MTDMX_MM_Free(MTDMX_MWIF_MM_T *prKernParam, MTDMX_MWIF_MM_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }


    fgRet = DMX_MM_Free(u1Pidx);

    return fgRet;
}

static BOOL _MTDMX_MM_FlushBuffer(MTDMX_MWIF_MM_T *prKernParam, MTDMX_MWIF_MM_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    fgRet = DMX_MM_FlushBuffer(u1Pidx);

    return fgRet;
}

static BOOL _MTDMX_MM_AllocateBuffer(MTDMX_MWIF_MM_T *prKernParam, MTDMX_MWIF_MM_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    fgRet = DMX_MM_AllocateBuffer(u1Pidx, prKernParam->u4Size);

    return fgRet;
}

static BOOL _MTDMX_MM_FreeBuffer(MTDMX_MWIF_MM_T *prKernParam, MTDMX_MWIF_MM_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    fgRet = DMX_MM_FreeBuffer(u1Pidx);

    return fgRet;
}

static BOOL _MTDMX_PS_SetSTreamId(MTDMX_MWIF_PS_T *prKernParam, MTDMX_MWIF_PS_T *prUserParam)
{
    DMX_PS_T rPs;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    rPs.u1StreamID = prKernParam->u1StreamId;
    rPs.u1SubstreamID = prKernParam->u1SubStreamId;
    rPs.fgEnableSsidFilter = TRUE;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return MTR_NOT_OK;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    return DMX_PS_SetStream(u1Pidx, DMX_PID_FLAG_STREAM_ID | DMX_PID_FLAG_SUBSTREAM_ID, &rPs);
}

static BOOL _MTDMX_GetVideoChannel(MTDMX_MWIF_VIDEO_CHANNEL_T *prKernParam, MTDMX_MWIF_VIDEO_CHANNEL_T *prUserParam)
{
    UINT8 u1Channel;
    BOOL fgRet;

    fgRet = DMX_GetVideoChannel(prKernParam->u1Pidx, &u1Channel);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pu1ChannelId, sizeof(u1Channel));
    COPY_TO_USER_ARG_SIZE(prUserParam->pu1ChannelId, &u1Channel, sizeof(u1Channel));

    return fgRet;
}

static BOOL _MTFVR_GetVideoType(MTDMX_MWIF_FVR_PID_T* prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    FVR_VIDEO_TYPE_T eType = FVR_GetVideoTypeEx(prKernParam->u1TsIdx);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->peType, sizeof(FVR_VIDEO_TYPE_T));
    COPY_TO_USER_ARG_SIZE(prUserParam->peType, &eType, sizeof(FVR_VIDEO_TYPE_T));

    return TRUE;
}

static BOOL _MTFVR_SetPid(MTDMX_MWIF_FVR_PID_T* prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    FVR_PID_T rPid;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->prPid, sizeof(rPid));
    COPY_FROM_USER_ARG_SIZE(prKernParam->prPid, &rPid, sizeof(rPid));

    // register callback
    if ((prKernParam->prPid->pfnSwapNotify != NULL) && (prKernParam->u4Flags & MTFVR_PID_FLAG_PKTSWAP))
    {
        rPid.pfnSwapNotify = _MtFvrMwifPktSwap;
    }
    else
    {
        rPid.pfnSwapNotify = NULL;
    }

    if ((prKernParam->prPid->pfnPicSearchNotify != NULL) && (prKernParam->u4Flags & MTFVR_PID_FLAG_PICSEARCH))
    {
        rPid.pfnPicSearchNotify = _MtFvrMwifPicSearch;
    }
    else
    {
        rPid.pfnPicSearchNotify = NULL;
    }

    if (prKernParam->u4Flags & FVR_PID_FLAG_DETECT_SCRAMBLE)
    {
        if (prKernParam->prPid->pfnDetectScrambleNotify)
        {
            rPid.pfnDetectScrambleNotify = _MtFvrMwifDetectScramble;
        }
        else
        {
            rPid.pfnDetectScrambleNotify = NULL;
        }

        if (prKernParam->prPid->pfnDetectRawScrambleNotify)
        {
            rPid.pfnDetectRawScrambleNotify = _MtFvrMwifDetectRawScramble;
        }
        else
        {
            rPid.pfnDetectRawScrambleNotify = NULL;
        }
    }

    return FVR_SetPid(prKernParam->u1Pidx, prKernParam->u4Flags, &rPid);
}

static BOOL _MTFVR_GetPid(MTDMX_MWIF_FVR_PID_T* prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    FVR_PID_T rPid;
    BOOL fgRet;

    fgRet = FVR_GetPid(prKernParam->u1Pidx, prKernParam->u4Flags, &rPid);

    // get callback funtion in user mode

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prPid, sizeof(rPid));
    COPY_TO_USER_ARG_SIZE(prUserParam->prPid, &rPid, sizeof(rPid));

    return fgRet;
}

static BOOL _MTFVR_SwapFreePattern(MTDMX_MWIF_FVR_PID_T* prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_SwapFreePattern(prKernParam->u1Pidx);

    if (prKernParam->u1Pidx < FVR_NUM_PID_INDEX)
    {
        x_mem_free((void *)_au4PatterBufAddr[prKernParam->u1Pidx]);
    }

    return fgRet;
}

static BOOL _MTFVR_SwapSetNullPattern(MTDMX_MWIF_FVR_PID_T* prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_SwapSetNullPattern(prKernParam->u1Pidx);

    return fgRet;
}

static BOOL _MTFVR_SwapSetPattern(MTDMX_MWIF_FVR_PID_T* prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    UINT16 u2Len;
    UINT8 *pu1PatternBuf;
    BOOL fgRet;

    u2Len = prKernParam->u2Len;
    pu1PatternBuf = x_mem_alloc(u2Len);

    // store pattern buffer address
    if (prKernParam->u1Pidx < FVR_NUM_PID_INDEX)
    {
        _au4PatterBufAddr[prKernParam->u1Pidx] = (UINT32)pu1PatternBuf;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->pu1Pattern, u2Len);
    COPY_FROM_USER_ARG_SIZE(prKernParam->pu1Pattern, pu1PatternBuf, u2Len);

    fgRet = FVR_SwapSetPattern(prKernParam->u1Pidx, pu1PatternBuf, u2Len);

    return fgRet;
}

static BOOL _MTFVR_SetAesKey(MTDMX_MWIF_FVR_KEY_T *prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    UINT8 au1Key[32];

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->pu1Key, 32);
    COPY_FROM_USER_ARG_SIZE(prKernParam->pu1Key, au1Key, 32);

    return FVR_SetAesKey(prKernParam->u1KeyIdx, prKernParam->fgEven, au1Key);
}

static BOOL _MTFVR_SetCryptoKey(MTDMX_MWIF_FVR_CRYPTO_KEY_T *prKernParam, MTDMX_MWIF_FVR_PID_T* prUserParam)
{
    UINT8 *pu1Key;
    BOOL fgRet;

    if (!prKernParam->pu1Key)
    {
        return FALSE;
    }

    pu1Key = (UINT8 *)x_mem_alloc(prKernParam->u4KeyLen);
    if (!pu1Key)
    {
        MTDMX_PRINT("alloc key buffer error, size=%u\n", prKernParam->u4KeyLen);
        return FALSE;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->pu1Key, prKernParam->u4KeyLen);
    COPY_FROM_USER_ARG_SIZE(prKernParam->pu1Key, pu1Key, prKernParam->u4KeyLen);

    fgRet = FVR_SetCryptoKey(prKernParam->u1KeyIdx, prKernParam->fgEven, prKernParam->eKeyType,
                            pu1Key, prKernParam->u4KeyLen);
    
    x_mem_free((VOID*)pu1Key);

    return fgRet;
}

static BOOL _MTFVR_GetGBL(MTDMX_MWIF_FVR_GBL_T *prKernParam, MTDMX_MWIF_FVR_GBL_T *prUserParam)
{
    FVR_GBL_T rGBL;
    BOOL fgRet;

    fgRet = FVR_GetGBL(prKernParam->u1TsIdx, prKernParam->u4Flags, &rGBL);

    // get callback funtion in user mode
    rGBL.u4BufStart = PHYSICAL(rGBL.u4BufStart);
    rGBL.u4Wp = PHYSICAL(rGBL.u4Wp);
    rGBL.u4Rp = PHYSICAL(rGBL.u4Rp);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prGBL, sizeof(FVR_GBL_T));
    COPY_TO_USER_ARG_SIZE(prUserParam->prGBL, &rGBL, sizeof(FVR_GBL_T));

    return fgRet;
}

static BOOL _MTFVR_SetGBL(MTDMX_MWIF_FVR_GBL_T *prKernParam, MTDMX_MWIF_FVR_GBL_T *prUserParam)
{
    FVR_GBL_T rGBL;
    BOOL fgRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->prGBL, sizeof(FVR_GBL_T));
    COPY_FROM_USER_ARG_SIZE(prKernParam->prGBL, &rGBL, sizeof(FVR_GBL_T));

    if ((prKernParam->u4Flags & MTFVR_GBL_FLAG_NOTIFY) && (rGBL.pfnNotifyFunc != NULL))
    {
        rGBL.pfnNotifyFunc = _MtfvrGBLCallback;
        rGBL.pvNotifyTag = NULL;
    }

    fgRet = FVR_SetGBL(prKernParam->u1TsIdx, prKernParam->u4Flags, &rGBL);

    return fgRet;
}

static BOOL _MTFVR_Init(MTDMX_MWIF_FVR_OP_T *prKernParam, MTDMX_MWIF_FVR_OP_T *prUserParam)
{
    BOOL fgRet;

    _u4TsRecPktCount = 0;

    fgRet = FVR_Init();

    return fgRet;
}

static BOOL _MTFVR_Start(MTDMX_MWIF_FVR_OP_T *prKernParam, MTDMX_MWIF_FVR_OP_T *prUserParam)
{
    BOOL fgRet;
    FVR_TIMESTAMP_T rTimestamp;

    _u4TsRecPktCount = 0;

    rTimestamp.fgEnable = TRUE;
    ///TODO: change to div definition
    rTimestamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    rTimestamp.u4Counter = 0;
    rTimestamp.u4Mask = 0xFFFFFFFF;

    FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimestamp);

    fgRet = FVR_Start(prKernParam->u1Idx);

    return fgRet;
}

static BOOL _MTFVR_Stop(MTDMX_MWIF_FVR_OP_T *prKernParam, MTDMX_MWIF_FVR_OP_T *prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_Stop(prKernParam->u1Idx);

    return fgRet;
}

static BOOL _MTFVR_IsRecording(MTDMX_MWIF_FVR_OP_T *prKernParam, MTDMX_MWIF_FVR_OP_T *prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_IsRecording(prKernParam->u1Idx);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&(prUserParam->fgIsRecording), sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&(prUserParam->fgIsRecording), &fgRet, sizeof(UINT32));

    return TRUE;
}

static BOOL _MTFVR_UnlockBuffer(MTDMX_MWIF_FVR_BUFFER_T *prKernParam, MTDMX_MWIF_FVR_BUFFER_T *prUserParam)
{
    BOOL fgRet;

    prKernParam->u4KernelAddr = VIRTUAL(prKernParam->u4KernelAddr);

	fgRet = FVR_UnlockBuffer(prKernParam->u1TsIdx, prKernParam->u4KernelAddr, prKernParam->u4Size);

    return fgRet;
}

static BOOL _MTFVR_FlushBuffer(MTDMX_MWIF_FVR_BUFFER_T *prKernParam, MTDMX_MWIF_FVR_BUFFER_T *prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_FlushBuffer(prKernParam->u1TsIdx);

    _u4TsRecPktCount = 0;

    return fgRet;
}

static BOOL _MTFVR_FreePid(MTDMX_MWIF_FVR_PID_T *prKernParam, MTDMX_MWIF_FVR_PID_T *prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_FreePid(prKernParam->u1Pidx);

    return fgRet;
}

static BOOL _MTFVR_FreeGBL(MTDMX_MWIF_FVR_GBL_T *prKernParam, MTDMX_MWIF_FVR_GBL_T *prUserParam)
{
    BOOL fgRet;

    fgRet = FVR_FreeGBL(prKernParam->u1TsIdx);

    return fgRet;
}

static BOOL _MTDMX_SetPacketSize(MTDMX_MWIF_SIZE_T *prKernParam, MTDMX_MWIF_SIZE_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Inst = 0;
    UINT8 u1TsIndex = 0;

    u1Inst = DMX_MUL_GetAvailableInst(DMX_CONN_TYPE_TUNER_0);
    u1TsIndex = DMX_MUL_GetTSIdx(u1Inst);
    fgRet = DMX_SetPacketSizeEx(u1TsIndex,prKernParam->u1Size);

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MTDMX_SetCustFrontEnd
 *  Set cust frontend
 *
 */
//-----------------------------------------------------------------------------

static BOOL _MTDMX_SetCustFrontEnd(MTDMX_MWIF_SIZE_T *prKernParam, MTDMX_MWIF_SIZE_T *prUserParam)
{
    BOOL fgRet;

    fgRet = DMX_CustFrontEnd();

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MTDMX_PES_FreeBufferEx
 *  Free Pes pid buffer
 *
 */
//-----------------------------------------------------------------------------
static BOOL _MTDMX_PES_FreeBufferEx(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return FALSE;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    //free pid buffer
    fgRet = DMX_PES_FreeBuffer(u1Pidx);

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _MTDMX_PSI_FreeBufferEx
 *  Free Psi section buffer
 *
 */
//-----------------------------------------------------------------------------
static BOOL _MTDMX_PSI_FreeBufferEx(MTDMX_MWIF_MUL_PID_T *prKernParam, MTDMX_MWIF_MUL_PID_T *prUserParam)
{
    BOOL fgRet;
    UINT8 u1Pidx;
    UINT8 u1PhyPidx = MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX;

    u1PhyPidx = _MTAL_DMX_PhysicalPidx(prKernParam->u1Pidx);

    if (u1PhyPidx == MTAL_DMX_MWIF_MULTIPLE_INVALID_PIDX)
    {
        MTDMX_PRINT("Get u1PhyPidx error\n");
        return FALSE;
    }
    else
    {
        u1Pidx = u1PhyPidx ;
    }

    //free section buffer
    fgRet = DMX_PSI_FreeBuffer(u1Pidx);

    return fgRet;
}



#ifdef CC_ENABLE_SKB
static BOOL _SKB_GetInstance(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T *prKernParam,
                             MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T *prUserParam)
{
    BOOL fgRet;
    MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T *pParam = NULL, *pParamPA = NULL;
//    SKB_Engine *pEngine = NULL, *pEnginePA = NULL;
    UINT32 u4ParamSize;

    PRINTK("+%s.....\n", __FUNCTION__);


//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T));
    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T));

    /*
        u4ParamSize = TZ_CACHE_ALIGN(sizeof(SKB_Engine *));
        pEnginePA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
        pEngine = (SKB_Engine *)VIRTUAL((UINT32)(pEnginePA));
        pParam->pEngine = pEnginePA;
        TZ_NWD_BufferStart((UINT32)pEngine, TZ_CACHE_ALIGN(sizeof(SKB_Engine *)));
    */
    fgRet = TZ_SKB_GetInstance(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T));
//    TZ_NWD_BufferEnd((UINT32)pEngine, TZ_CACHE_ALIGN(sizeof(SKB_Engine *)));

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->pEngine, sizeof(SKB_Engine *));
    COPY_TO_USER_ARG_SIZE(&prUserParam->pEngine, &pParam->pEngine, sizeof(SKB_Engine *));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &pParam->rRet, sizeof(SKB_Result));

    /*
        if (pEnginePA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)pEnginePA);
        }
    */
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s......................%d, engine=%x\n", __FUNCTION__, prUserParam->rRet, prUserParam->pEngine);

    return TRUE;
}

static BOOL _SKB_Engine_Release(MTDMX_MWIF_SKB_ENGINE_RELEASE_T *prKernParam,
                                MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_RELEASE_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_RELEASE_T));
    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_RELEASE_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_RELEASE_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_RELEASE_T));

    fgRet = TZ_SKB_Release(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_RELEASE_T));

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &pParam->rRet, sizeof(SKB_Result));

    PRINTK("-%s......................%d\n", __FUNCTION__, prUserParam->rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }


    return TRUE;
}

static BOOL _SKB_GetInfo(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *prKernParam,
                         MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_EngineInfo *prInfo = NULL, *prInfoPA = NULL;
//    SKB_EngineInfo rInfo;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo));
    prInfoPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    prInfo = (SKB_EngineInfo *)VIRTUAL((UINT32)(prInfoPA));
    pParam->pInfo = prInfoPA;
    TZ_NWD_BufferStart((UINT32)prInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

    /*
        prKernParam->pInfo = (SKB_EngineInfo *)PHYSICAL((UINT32)&rInfo);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rInfo, sizeof(rInfo));
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    */

    fgRet = TZ_SKB_GetInfo(pParam, TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T)));
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    /*
        HalInvalidateDCacheMultipleLine((UINT32)&rInfo, sizeof(rInfo));
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    */
    TZ_NWD_BufferEnd((UINT32)prInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pInfo, sizeof(SKB_EngineInfo));
    COPY_TO_USER_ARG_SIZE(prUserParam->pInfo, prInfo, sizeof(SKB_EngineInfo));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &pParam->rRet, sizeof(SKB_Result));

    if (prInfoPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prInfoPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    return TRUE;
}

static BOOL _SKB_GetProertiesSize(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *prKernParam,
                                  MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;
    SKB_EngineInfo *prInfo = NULL, *prInfoPA = NULL;
    SKB_Result rRet;
    UINT32 u4ValueSize;
    UINT32 u4NameSize;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));

    prInfoPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)), TZ_CACHE_LINE_SIZE);
    prInfo = (SKB_EngineInfo *)VIRTUAL((UINT32)(prInfoPA));
    pParam->pInfo = prInfoPA;
    TZ_NWD_BufferStart((UINT32)prInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

    /*
        prKernParam->pInfo = (SKB_EngineInfo *)PHYSICAL((UINT32)&rInfo);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rInfo, sizeof(rInfo));
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    */
    fgRet = TZ_SKB_GetProertiesSize(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    /*
        HalInvalidateDCacheMultipleLine((UINT32)&rInfo, sizeof(rInfo));
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    */
    TZ_NWD_BufferEnd((UINT32)prInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

    rRet = pParam->rRet;
    u4NameSize = pParam->u4PropertiesNameSize;
    u4ValueSize = pParam->u4PropertiesValueSize;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4PropertiesNameSize, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4PropertiesNameSize, &u4NameSize, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4PropertiesValueSize, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4PropertiesValueSize, &u4ValueSize, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    PRINTK("-%s...........................%d, u4PropertiesNameSize=%d, u4PropertiesValueSize=%d\n", __FUNCTION__, rRet, prUserParam->u4PropertiesNameSize, prUserParam->u4PropertiesValueSize);

    if (prInfoPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prInfoPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _SKB_GetProertiesData(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *prKernParam,
                                  MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_Result rRet;
    UINT32 u4ValueSize;
    UINT32 u4NameSize;
    BOOL fgRet;
    UINT8 *pName = NULL, *pNamePA = NULL;
    UINT8 *pValue = NULL, *pValuePA = NULL;
    SKB_EngineProperty *prProperties = NULL, *prPropertiesPA = NULL;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));

    pParam->pInfo = NULL;

    prPropertiesPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(SKB_EngineProperty)), TZ_CACHE_LINE_SIZE);
    prProperties = (SKB_EngineProperty *)VIRTUAL((UINT32)(prPropertiesPA));
    pParam->pProperties = prPropertiesPA;

    pNamePA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(pParam->u4PropertiesNameSize), TZ_CACHE_LINE_SIZE);
    pName = (UINT8 *)VIRTUAL((UINT32)(pNamePA));
    prProperties->name = pNamePA;

    pValuePA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(pParam->u4PropertiesValueSize), TZ_CACHE_LINE_SIZE);
    pValue = (UINT8 *)VIRTUAL((UINT32)(pValuePA));
    prProperties->value = pValuePA;


    TZ_NWD_BufferStart((UINT32)prProperties, TZ_CACHE_ALIGN(sizeof(SKB_EngineProperty)));
    TZ_NWD_BufferStart((UINT32)pName, TZ_CACHE_ALIGN(pParam->u4PropertiesNameSize));
    TZ_NWD_BufferStart((UINT32)pValue, TZ_CACHE_ALIGN(pParam->u4PropertiesValueSize));

    /*
        pName = x_mem_alloc(prKernParam->u4PropertiesNameSize);
        rProperties.name = (const char *)PHYSICAL((UINT32)pName);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pName, prKernParam->u4PropertiesNameSize);

        pValue = x_mem_alloc(prKernParam->u4PropertiesValueSize);
        rProperties.value = (const char *)PHYSICAL((UINT32)pValue);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pValue, prKernParam->u4PropertiesValueSize);

        prKernParam->pProperties = (SKB_EngineProperty *)PHYSICAL((UINT32)&rProperties);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rProperties, sizeof(rProperties));
        prKernParam->pInfo = (SKB_EngineInfo *)PHYSICAL((UINT32)&rInfo);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rInfo, sizeof(rInfo));

        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
    */
    fgRet = TZ_SKB_GetProertiesData(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    TZ_NWD_BufferEnd((UINT32)prProperties, TZ_CACHE_ALIGN(sizeof(SKB_EngineProperty)));
    TZ_NWD_BufferEnd((UINT32)pName, TZ_CACHE_ALIGN(pParam->u4PropertiesNameSize));
    TZ_NWD_BufferEnd((UINT32)pValue, TZ_CACHE_ALIGN(pParam->u4PropertiesValueSize));

    /*
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
        HalInvalidateDCacheMultipleLine((UINT32)&rInfo, sizeof(rInfo));
        HalInvalidateDCacheMultipleLine((UINT32)&rProperties, sizeof(rProperties));
        HalInvalidateDCacheMultipleLine((UINT32)pValue, prKernParam->u4PropertiesValueSize);
        HalInvalidateDCacheMultipleLine((UINT32)pName, prKernParam->u4PropertiesNameSize);
    */

    rRet = pParam->rRet;
    u4NameSize = pParam->u4PropertiesNameSize;
    u4ValueSize = pParam->u4PropertiesValueSize;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pProperties->name, u4NameSize);
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pProperties->value, u4ValueSize);
    COPY_TO_USER_ARG_SIZE(prUserParam->pProperties->name,
                          pName, u4NameSize);
    COPY_TO_USER_ARG_SIZE(prUserParam->pProperties->value,
                          pValue, u4ValueSize);
//    x_mem_free(pName);
//    x_mem_free(pValue);
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    if (prPropertiesPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prPropertiesPA);
    }
    if (pValuePA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pValuePA);
    }
    if (pNamePA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pNamePA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    return TRUE;
}


static BOOL _SKB_DataFromWrapped(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T *prKernParam,
                                 MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_SecureData* data;
    SKB_Byte *pwrapped = NULL, *pwrappedPA = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s..... %d\n", __FUNCTION__, (UINT32)prKernParam->wrapped_size);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T));

    u4Size = TZ_CACHE_ALIGN(pParam->wrapped_size);
    pwrappedPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    pwrapped = (SKB_Byte *)VIRTUAL((UINT32)(pwrappedPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->wrapped, u4Size);
    COPY_FROM_USER_ARG_SIZE(pParam->wrapped, pwrapped, u4Size);
    pParam->wrapped = pwrappedPA;
    TZ_NWD_BufferStart((UINT32)pwrapped, u4Size);

    /*
        wrapped = x_mem_alloc((UINT32)prKernParam->wrapped_size);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->wrapped, prKernParam->wrapped_size);
        COPY_FROM_USER_ARG_SIZE(prKernParam->wrapped, wrapped, prKernParam->wrapped_size);

        ///NOTE: prWrapped->wrapping_parameters should not be used in kernel,
        // we don't know how to handle this pointer and size
        HalFlushInvalidateDCacheMultipleLine((UINT32)wrapped, (UINT32)prKernParam->wrapped_size);
        prKernParam->wrapped = (const SKB_Byte* )PHYSICAL((UINT32)wrapped);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T));
        PRINTK("TZ-%s.....0x%x\n", __FUNCTION__, prKernParam);
        HalFlushInvalidateDCache();
    */
    fgRet = TZ_SKB_DataFromWrapped(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalFlushInvalidateDCache();
    TZ_NWD_BufferEnd((UINT32)pwrapped, u4Size);

    rRet = pParam->rRet;
    data = pParam->data;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->data, sizeof(SKB_SecureData *));
    COPY_TO_USER_ARG_SIZE(&prUserParam->data, &data, sizeof(SKB_SecureData *));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    if (pwrappedPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pwrappedPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    return TRUE;
}


static BOOL _SKB_DataFromExported(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T *prKernParam,
                                  MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_SecureData* data;
    SKB_Byte *exported = NULL, *exportedPA = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....%d\n", __FUNCTION__, (UINT32)prKernParam->exported_size);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T));

    u4Size = TZ_CACHE_ALIGN(pParam->exported_size);
    exportedPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    exported = (SKB_Byte *)VIRTUAL((UINT32)(exportedPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->exported, u4Size);
    COPY_FROM_USER_ARG_SIZE(pParam->exported, exported, u4Size);
    pParam->exported = exportedPA;
    TZ_NWD_BufferStart((UINT32)exported, u4Size);
    /*
        exported = x_mem_alloc((UINT32)prKernParam->exported_size);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->exported, prKernParam->exported_size);
        COPY_FROM_USER_ARG_SIZE(prKernParam->exported, exported, prKernParam->exported_size);

        HalFlushInvalidateDCacheMultipleLine((UINT32)exported, (UINT32)prKernParam->exported_size);
        prKernParam->exported = (const SKB_Byte*)PHYSICAL((UINT32)exported);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T));
    */

    fgRet = TZ_SKB_DataFromExported(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    TZ_NWD_BufferEnd((UINT32)exported, u4Size);

    /*
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T));
        HalInvalidateDCacheMultipleLine((UINT32)exported, (UINT32)prKernParam->exported_size);
    */

    rRet = pParam->rRet;
    data = pParam->data;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->data, sizeof(SKB_SecureData *));
    COPY_TO_USER_ARG_SIZE(&prUserParam->data, &data, sizeof(SKB_SecureData *));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

//    x_mem_free(exported);

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (exportedPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)exportedPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _SKB_CreateTransform(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T  *prKernParam,
                                 MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T  *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_Transform* pTransform = NULL;
    SKB_DigestTransformParameters *prDigestParam = NULL, *prDigestParamPA = NULL;
    SKB_VerifyTransformParameters *prVerifyParam = NULL, *prVerifyParamPA = NULL;
    SKB_SignTransformParameters *pSignParam = NULL, *pSignParamPA = NULL;
    SKB_Byte *signature = NULL, *signaturePA = NULL;
//    const void* transform_parameters = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T));

    switch (pParam->transform_type)
    {
    case SKB_TRANSFORM_TYPE_DIGEST:
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_DigestTransformParameters));
//        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->transform_parameters, u4Size);
        prDigestParamPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        prDigestParam = (void *)VIRTUAL((UINT32)prDigestParamPA);
        COPY_FROM_USER_ARG_SIZE(pParam->transform_parameters, prDigestParam, u4Size);
        pParam->transform_parameters = prDigestParamPA;
        TZ_NWD_BufferStart((UINT32)prDigestParam, TZ_CACHE_ALIGN(sizeof(SKB_DigestTransformParameters)));

        /*
                prDigestParam = x_mem_alloc(sizeof(SKB_DigestTransformParameters));
                COPY_FROM_USER_ARG_SIZE(prKernParam->transform_parameters, prDigestParam,
                    sizeof(SKB_DigestTransformParameters));
                transform_parameters = prDigestParam;
                HalFlushInvalidateDCacheMultipleLine((UINT32)prDigestParam, (UINT32)sizeof(SKB_DigestTransformParameters));
                prKernParam->transform_parameters = (const void* )PHYSICAL((UINT32)prDigestParam);
        */
        break;
    case SKB_TRANSFORM_TYPE_VERIFY:
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_VerifyTransformParameters));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->transform_parameters, u4Size);
        prVerifyParamPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        prVerifyParam = (void *)VIRTUAL((UINT32)prVerifyParamPA);
        COPY_FROM_USER_ARG_SIZE(pParam->transform_parameters, prVerifyParam, u4Size);
        pParam->transform_parameters = prVerifyParamPA;

        u4Size = TZ_CACHE_ALIGN(prVerifyParam->signature_size);
        signaturePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        signature = (void *)VIRTUAL((UINT32)signaturePA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prVerifyParam->signature, u4Size);
        COPY_FROM_USER_ARG_SIZE(prVerifyParam->signature, signature, u4Size);
        prVerifyParam->signature = signaturePA;

        TZ_NWD_BufferStart((UINT32)signature, TZ_CACHE_ALIGN(prVerifyParam->signature_size));
        TZ_NWD_BufferStart((UINT32)prVerifyParam, TZ_CACHE_ALIGN(sizeof(SKB_VerifyTransformParameters)));

        /*
                USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->transform_parameters,
                    sizeof(SKB_VerifyTransformParameters));
                prVerifyParam = x_mem_alloc(sizeof(SKB_VerifyTransformParameters));
                COPY_FROM_USER_ARG_SIZE(prKernParam->transform_parameters, prVerifyParam,
                    sizeof(SKB_VerifyTransformParameters));
                transform_parameters = prVerifyParam;
                signature = x_mem_alloc(prVerifyParam->signature_size);
                USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prVerifyParam->signature,
                    prVerifyParam->signature_size);
                COPY_FROM_USER_ARG_SIZE(prVerifyParam->signature, signature,
                    prVerifyParam->signature_size);
                prVerifyParam->signature = signature;
                HalFlushInvalidateDCacheMultipleLine((UINT32)signature, (UINT32)prVerifyParam->signature_size);
                prVerifyParam->signature = (const SKB_Byte*)PHYSICAL((UINT32)signature);
                HalFlushInvalidateDCacheMultipleLine((UINT32)prVerifyParam, (UINT32)sizeof(SKB_VerifyTransformParameters));
                prKernParam->transform_parameters = (const void*)PHYSICAL((UINT32)prVerifyParam);
        */
        break;
    case SKB_TRANSFORM_TYPE_SIGN:
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_SignTransformParameters));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->transform_parameters, u4Size);
        pSignParamPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pSignParam = (void *)VIRTUAL((UINT32)pSignParamPA);
        COPY_FROM_USER_ARG_SIZE(pParam->transform_parameters, pSignParam, u4Size);
        pParam->transform_parameters = pSignParamPA;
        TZ_NWD_BufferStart((UINT32)pSignParam, TZ_CACHE_ALIGN(sizeof(SKB_SignTransformParameters)));

        /*
                USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->transform_parameters,
                    sizeof(SKB_SignTransformParameters));
                pSignParam = x_mem_alloc(sizeof(SKB_SignTransformParameters));
                COPY_FROM_USER_ARG_SIZE(prKernParam->transform_parameters, pSignParam,
                    sizeof(SKB_SignTransformParameters));
                transform_parameters = pSignParam;
                HalFlushInvalidateDCacheMultipleLine((UINT32)pSignParam, (UINT32)sizeof(SKB_SignTransformParameters));
                prKernParam->transform_parameters = (const void*)PHYSICAL((UINT32)pSignParam);
        */
        break;
    }

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T));
    fgRet = TZ_SKB_CreateTransform(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    switch (pParam->transform_type)
    {
    case SKB_TRANSFORM_TYPE_DIGEST:
        TZ_NWD_BufferEnd((UINT32)prDigestParam, TZ_CACHE_ALIGN(sizeof(SKB_DigestTransformParameters)));
        break;
    case SKB_TRANSFORM_TYPE_VERIFY:
        TZ_NWD_BufferEnd((UINT32)signature, TZ_CACHE_ALIGN(prVerifyParam->signature_size));
        TZ_NWD_BufferEnd((UINT32)prVerifyParam, TZ_CACHE_ALIGN(sizeof(SKB_VerifyTransformParameters)));
        break;
    case SKB_TRANSFORM_TYPE_SIGN:
        TZ_NWD_BufferEnd((UINT32)pSignParam, TZ_CACHE_ALIGN(sizeof(SKB_SignTransformParameters)));
        break;
    }

//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T));
    rRet = pParam->rRet;
    pTransform = pParam->transform;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->transform, sizeof(SKB_Transform*));
    COPY_TO_USER_ARG_SIZE(&prUserParam->transform, &pTransform, sizeof(SKB_Transform*));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    switch (pParam->transform_type)
    {
    case SKB_TRANSFORM_TYPE_DIGEST:
        BSP_FreeAlignedDmaMemory((UINT32)prDigestParamPA);
//        x_mem_free(prDigestParam);
        break;
    case SKB_TRANSFORM_TYPE_VERIFY:
        BSP_FreeAlignedDmaMemory((UINT32)prVerifyParamPA);
        BSP_FreeAlignedDmaMemory((UINT32)signaturePA);
//        x_mem_free(prVerifyParam);
//        x_mem_free(signature);
        break;
    case SKB_TRANSFORM_TYPE_SIGN:
        BSP_FreeAlignedDmaMemory((UINT32)pSignParamPA);
//        x_mem_free(pSignParam);
        break;
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    return TRUE;
}


static BOOL _SKB_CreateCipher(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T *prKernParam,
                              MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T *prUserParam)
{
    MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_Cipher* pCipher;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T));

    ///NOTE: prCipher->cipher_parameters should not be used in kernel,
    // we don't know how to handle this pointer and size

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T));
    fgRet = TZ_SKB_CreateCipher(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T));
    rRet = pParam->rRet;
    pCipher = pParam->cipher;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->cipher, sizeof(SKB_Cipher*));
    COPY_TO_USER_ARG_SIZE(&prUserParam->cipher, &pCipher, sizeof(SKB_Cipher*));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _SKB_SecureData_Release(MTDMX_MWIF_SKB_SECUREDATA_T *prKernParam,
                                    MTDMX_MWIF_SKB_SECUREDATA_T *prUserParam)
{
    MTDMX_MWIF_SKB_SECUREDATA_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_SECUREDATA_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    fgRet = TZ_SKB_SecureData_Release(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    return TRUE;
}

static BOOL _SKB_SecureDataGetInfo(MTDMX_MWIF_SKB_SECUREDATA_T *prKernParam,
                                   MTDMX_MWIF_SKB_SECUREDATA_T *prUserParam)
{
    MTDMX_MWIF_SKB_SECUREDATA_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_DataInfo *prDataInfo = NULL, *prDataInfoPA = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_SECUREDATA_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));

    u4Size = TZ_CACHE_ALIGN(sizeof(SKB_DataInfo));
    prDataInfoPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    prDataInfo = (SKB_DataInfo *)VIRTUAL((UINT32)(prDataInfoPA));
    pParam->info = prDataInfoPA;
    TZ_NWD_BufferStart((UINT32)prDataInfo, TZ_CACHE_ALIGN(sizeof(SKB_DataInfo)));

    /*
        prKernParam->info = (SKB_DataInfo*)PHYSICAL((UINT32)&rDataInfo);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rDataInfo, sizeof(rDataInfo));
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    */
    fgRet = TZ_SKB_SecureDataGetInfo(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    TZ_NWD_BufferEnd((UINT32)prDataInfo, TZ_CACHE_ALIGN(sizeof(SKB_DataInfo)));
    /*
        HalInvalidateDCacheMultipleLine((UINT32)&rDataInfo, sizeof(rDataInfo));
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    */
    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->info, sizeof(SKB_DataInfo));
    COPY_TO_USER_ARG_SIZE(prUserParam->info, prDataInfo, sizeof(SKB_DataInfo));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    if (prDataInfoPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prDataInfoPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    return TRUE;
}


static BOOL _SKB_SecureData_Export(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T *prKernParam,
                                   MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T *prUserParam)
{
    MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_Size *prSize = NULL, *prSizePA = NULL;
    SKB_Byte *pByte = NULL, *pBytePA = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....%d\n", __FUNCTION__, *(SKB_Size *)prKernParam->buffer_size);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));

    if (pParam->buffer == NULL)
    {
        // get size first
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_Size));
        prSizePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        prSize = (SKB_Size *)VIRTUAL((UINT32)(prSizePA));
        pParam->buffer_size = prSizePA;
        TZ_NWD_BufferStart((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        /*
                prKernParam->buffer_size = (SKB_Size*)PHYSICAL((UINT32)&rSize);
                HalFlushInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
        */
        fgRet = TZ_SKB_SecureData_Export(pParam, u4ParamSize);
        if (!fgRet)
        {
            PRINTK("TZ-%s.....\n", __FUNCTION__);
        }
        TZ_NWD_BufferEnd((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        rRet = pParam->rRet;
        /*
                HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
                rRet = prKernParam->rRet;
                HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalInvalidateDCacheMultipleLine((UINT32)pByte, rSize);
                HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
        */


        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->buffer_size, sizeof(SKB_Size));
        COPY_TO_USER_ARG_SIZE(prUserParam->buffer_size, prSize, sizeof(SKB_Size));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
        COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

        PRINTK("-%s...........................%d, user size = %d, param size = %d\n", __FUNCTION__, rRet, *(SKB_Size*)prUserParam->buffer_size, *(SKB_Size*)prSize);

        if (prSizePA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)prSizePA);
        }
        if (pParamPA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
        }

        return TRUE;
    }

    /*
        // check if buffer size is enough
        if(rSize > *(SKB_Size *)prKernParam->buffer_size)
        {
            return FALSE;
        }
    */

//    pByte = x_mem_alloc(*(SKB_Size *)pParamPA->buffer_size);
    u4Size = TZ_CACHE_ALIGN(*(SKB_Size *)pParam->buffer_size);
    pBytePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    pByte = (SKB_Byte *)VIRTUAL((UINT32)(pBytePA));
    pParam->buffer = pBytePA;
    TZ_NWD_BufferStart((UINT32)pByte, u4Size);

    u4Size = TZ_CACHE_ALIGN(sizeof(SKB_Size));
    prSizePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    prSize = (SKB_Size *)VIRTUAL((UINT32)(prSizePA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->buffer_size, sizeof(SKB_Size));
    COPY_FROM_USER_ARG_SIZE(pParam->buffer_size, prSize, sizeof(SKB_Size));
    pParam->buffer_size = prSizePA;
    TZ_NWD_BufferStart((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

    /*
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->buffer_size, sizeof(SKB_Size));
        COPY_FROM_USER_ARG_SIZE(prUserParam->buffer_size, &rSize, sizeof(SKB_Size));
        prKernParam->buffer = (SKB_Byte*)PHYSICAL((UINT32)pByte);
        prKernParam->buffer_size = (SKB_Size*)PHYSICAL((UINT32)&rSize);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
        HalFlushInvalidateDCacheMultipleLine((UINT32)pByte, rSize);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
    */
    fgRet = TZ_SKB_SecureData_Export(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    TZ_NWD_BufferEnd((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));
    TZ_NWD_BufferEnd((UINT32)pByte, TZ_CACHE_ALIGN(*(SKB_Size*)prSize));

    /*
        HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
        HalInvalidateDCacheMultipleLine((UINT32)pByte, rSize);
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
    */

    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->buffer, *(SKB_Size*)prSize);
    COPY_TO_USER_ARG_SIZE(prUserParam->buffer, pByte, *(SKB_Size*)prSize);
//    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->buffer_size, sizeof(rSize));
//    COPY_TO_USER_ARG_SIZE(prUserParam->buffer_size, &rSize, sizeof(rSize));

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->buffer_size, sizeof(SKB_Size));
    COPY_TO_USER_ARG_SIZE(prUserParam->buffer_size, prSize, sizeof(SKB_Size));

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

//    x_mem_free(pByte);

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (pBytePA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pBytePA);
    }
    if (prSizePA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prSizePA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _SKB_SecureData_Derive(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T *prKernParam,
                                   MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T *prUserParam)
{
    MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_SecureData* data;
    SKB_Result rRet;
    BOOL fgRet;
    SKB_SliceDerivationParameters *prParameters = NULL, *prParametersPA = NULL;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T));

    u4Size = TZ_CACHE_ALIGN(sizeof(SKB_SliceDerivationParameters));
    prParametersPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    prParameters = (SKB_SliceDerivationParameters *)VIRTUAL((UINT32)(prParametersPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->parameters, u4Size);
    COPY_FROM_USER_ARG_SIZE(pParam->parameters, prParameters, u4Size);
    pParam->parameters = prParametersPA;
    TZ_NWD_BufferStart((UINT32)prParameters, u4Size);

    /*
        // parameter is a data from user space, we need to copy it to kernel space. its size is 8.
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->parameters, sizeof(SKB_SliceDerivationParameters));
        COPY_FROM_USER_ARG_SIZE(prKernParam->parameters, &rParameters, sizeof(SKB_SliceDerivationParameters));
        prKernParam->parameters = (const void*)PHYSICAL((UINT32)&rParameters);
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rParameters, sizeof(SKB_SliceDerivationParameters));
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T));
    */

    fgRet = TZ_SKB_SecureData_Derive(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    TZ_NWD_BufferEnd((UINT32)prParameters, u4Size);

    /*
        HalInvalidateDCacheMultipleLine((UINT32)&rParameters, sizeof(SKB_SliceDerivationParameters));
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T));
    */
    rRet = pParam->rRet;
    data = pParam->data;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->data, sizeof(SKB_SecureData *));
    COPY_TO_USER_ARG_SIZE(&prUserParam->data, &data, sizeof(SKB_SecureData *));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (prParametersPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prParametersPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _SKB_Transform_Release(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *prKernParam,
                                   MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *prUserParam)
{
    MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    fgRet = TZ_SKB_Transform_Release(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    rRet = pParam->rRet;
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _SKB_Transform_AddBytes(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *prKernParam,
                                    MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *prUserParam)
{
    MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_Byte *data = NULL, *dataPA = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....%d\n", __FUNCTION__, (UINT32)prKernParam->data_size);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));

    u4Size = TZ_CACHE_ALIGN(pParam->data_size);
    dataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    data = (SKB_Byte *)VIRTUAL((UINT32)(dataPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->data, u4Size);
    COPY_FROM_USER_ARG_SIZE(pParam->data, data, u4Size);
    pParam->data = dataPA;
    TZ_NWD_BufferStart((UINT32)data, u4Size);
    /*
        data = x_mem_alloc((UINT32)prKernParam->data_size);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->data, prKernParam->data_size);
        COPY_FROM_USER_ARG_SIZE(prKernParam->data, data, prKernParam->data_size);
        prKernParam->data = (const SKB_Byte*)PHYSICAL((UINT32)data);
        HalFlushInvalidateDCacheMultipleLine((UINT32)data, (UINT32)prKernParam->data_size);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    */
    fgRet = TZ_SKB_Transform_AddBytes(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    TZ_NWD_BufferEnd((UINT32)data, u4Size);

    /*
        HalInvalidateDCacheMultipleLine((UINT32)data, (UINT32)prKernParam->data_size);
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    */
    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

//    x_mem_free(data);

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (dataPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)dataPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _SKB_Transform_AddSecureData(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T *prKernParam,
        MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T *prUserParam)
{
    MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T));
    fgRet = TZ_SKB_Transform_AddSecureData(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T));
    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _SKB_Transform_GetOutput(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T *prKernParam,
                                     MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T *prUserParam)
{
    MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_Size *prSize = NULL, *prSizePA = NULL;
    SKB_Byte *data = NULL, *dataPA = NULL;
    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));

    // if buffer is NULL, get size.
    if (pParam->output == NULL)
    {
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_Size));
        prSizePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        prSize = (SKB_Size *)VIRTUAL((UINT32)(prSizePA));
        pParam->output_size = prSizePA;
        TZ_NWD_BufferStart((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        /*
                prKernParam->output_size = (SKB_Size*)PHYSICAL((UINT32)&rSize);
                HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
                HalFlushInvalidateDCacheMultipleLine((UINT32)rSize, sizeof(SKB_Size));
        */
        fgRet = TZ_SKB_Transform_GetOutput(pParam, u4ParamSize);
        if (!fgRet)
        {
            PRINTK("TZ-%s.....\n", __FUNCTION__);
        }
        TZ_NWD_BufferEnd((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        /*
                HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
                HalInvalidateDCacheMultipleLine((UINT32)rSize, sizeof(SKB_Size));
        */
        rRet = pParam->rRet;


        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->output_size, sizeof(SKB_Size));
        COPY_TO_USER_ARG_SIZE(prUserParam->output_size, prSize, sizeof(SKB_Size));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
        COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

        PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

        if (prSizePA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)prSizePA);
        }
        if (pParamPA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
        }

        return TRUE;
    }
    else
    {
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_Size));
        prSizePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        prSize = (SKB_Size *)VIRTUAL((UINT32)(prSizePA));
        pParam->output_size = prSizePA;
        TZ_NWD_BufferStart((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));
        pParam->output = NULL;
        /*
                prKernParam->output_size = (SKB_Size*)PHYSICAL((UINT32)&rSize);
                prKernParam->output = NULL;
                HalFlushInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
        */
        fgRet = TZ_SKB_Transform_GetOutput(pParam, u4ParamSize);
        if (!fgRet)
        {
            PRINTK("TZ-%s.....\n", __FUNCTION__);
        }
        TZ_NWD_BufferEnd((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        /*
                HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
        */
        rRet = pParam->rRet;

        //if(rRet != SKB_SUCCESS || *(SKB_Size *)prKernParam->output_size < rSize)
        //{
        //    return FALSE;
        //}

        u4Size = TZ_CACHE_ALIGN(*(SKB_Size*)prSize);
        dataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        data = (SKB_Byte *)VIRTUAL((UINT32)(dataPA));
        pParam->output = dataPA;
        TZ_NWD_BufferStart((UINT32)data, u4Size);

        /*
                data = x_mem_alloc((UINT32)rSize);

                prKernParam->output = (SKB_Byte*)PHYSICAL((UINT32)data);
                HalFlushInvalidateDCacheMultipleLine((UINT32)data, rSize);
                HalFlushInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
        */
        fgRet = TZ_SKB_Transform_GetOutput(pParam, u4ParamSize);
        if (!fgRet)
        {
            PRINTK("TZ-%s.....\n", __FUNCTION__);
        }

        TZ_NWD_BufferEnd((UINT32)data, u4Size);

        /*
                HalInvalidateDCacheMultipleLine((UINT32)data, rSize);
                HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
        */

        rRet = pParam->rRet;

        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->output, *(SKB_Size*)prSize);
        COPY_TO_USER_ARG_SIZE(prUserParam->output, data, *(SKB_Size*)prSize);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
        COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

//        x_mem_free(data);

        PRINTK("-%s...........................%d %d\n", __FUNCTION__, rRet, *(SKB_Size*)prSize);

        if (dataPA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)dataPA);
        }
        if (prSizePA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)prSizePA);
        }
        if (pParamPA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
        }

        return TRUE;
    }
}


static BOOL _SKB_Cipher_ProcessBuffer(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *prKernParam,
                                      MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *prUserParam)
{
    MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    SKB_Byte *in_buffer = NULL, *in_bufferPA = NULL;
    SKB_Byte *out_buffer = NULL, *out_bufferPA = NULL;
    SKB_Byte *iv = NULL, *ivPA = NULL;
    SKB_Result rRet;
    SKB_Size *prSize = NULL, *prSizePA = NULL;
    BOOL fgRet;

    PRINTK("+%s..... in: %d out: %d\n", __FUNCTION__, prKernParam->in_buffer_size, *(SKB_Size *)prKernParam->out_buffer_size);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));

    // Assume in buffer is equal to out buffer
    if (pParam->out_buffer == NULL)
    {
        u4Size = TZ_CACHE_ALIGN(sizeof(SKB_Size));
        prSizePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        prSize = (SKB_Size *)VIRTUAL((UINT32)(prSizePA));
        pParam->out_buffer_size = prSizePA;
        TZ_NWD_BufferStart((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        /*
                prKernParam->out_buffer_size = (SKB_Size*)PHYSICAL((UINT32)&rSize);
                HalFlushInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
        */

        fgRet = TZ_SKB_Cipher_ProcessBuffer(pParam, u4ParamSize);

        TZ_NWD_BufferEnd((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

        /*
                HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
                HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
        */
        rRet = pParam->rRet;

        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->out_buffer_size, sizeof(SKB_Size));
        COPY_TO_USER_ARG_SIZE(prUserParam->out_buffer_size, prSize, sizeof(SKB_Size));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
        COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

        PRINTK("-%s........................... out_buffer=NULL\n", __FUNCTION__);

        if (prSizePA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)prSizePA);
        }
        if (pParamPA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
        }

        return TRUE;
    }

    u4Size = TZ_CACHE_ALIGN(sizeof(SKB_Size));
    prSizePA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    prSize = (SKB_Size *)VIRTUAL((UINT32)(prSizePA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->out_buffer_size, sizeof(SKB_Size));
    COPY_FROM_USER_ARG_SIZE(pParam->out_buffer_size, prSize, sizeof(SKB_Size));
    pParam->out_buffer_size = prSizePA;
    TZ_NWD_BufferStart((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

    u4Size = TZ_CACHE_ALIGN(pParam->in_buffer_size);
    in_bufferPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    in_buffer = (SKB_Byte *)VIRTUAL((UINT32)(in_bufferPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->in_buffer, u4Size);
    COPY_FROM_USER_ARG_SIZE(pParam->in_buffer, in_buffer, u4Size);
    pParam->in_buffer = in_bufferPA;
    TZ_NWD_BufferStart((UINT32)in_buffer, TZ_CACHE_ALIGN(pParam->in_buffer_size));

    u4Size = TZ_CACHE_ALIGN(*(SKB_Size *)prSize);
    out_bufferPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    out_buffer = (SKB_Byte *)VIRTUAL((UINT32)(out_bufferPA));
    pParam->out_buffer = out_bufferPA;
    TZ_NWD_BufferStart((UINT32)out_buffer, TZ_CACHE_ALIGN(*(SKB_Size *)prSize));

    if (pParam->iv_size)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->iv_size);
        ivPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        iv = (SKB_Byte *)VIRTUAL((UINT32)(ivPA));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->iv, pParam->iv_size);
        COPY_FROM_USER_ARG_SIZE(pParam->iv, iv, pParam->iv_size);
        pParam->iv = ivPA;
        TZ_NWD_BufferStart((UINT32)iv, TZ_CACHE_ALIGN(pParam->iv_size));
    }
    else
    {
        iv = NULL;
    }


    /*
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->out_buffer_size, sizeof(SKB_Size));
        COPY_FROM_USER_ARG_SIZE(prUserParam->out_buffer_size, &rSize, sizeof(SKB_Size));


        out_buffer = x_mem_alloc(*(SKB_Size *)prKernParam->out_buffer_size);

        in_buffer = x_mem_alloc((UINT32)prKernParam->in_buffer_size);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->in_buffer, prKernParam->in_buffer_size);
        COPY_FROM_USER_ARG_SIZE(prKernParam->in_buffer, in_buffer, prKernParam->in_buffer_size);

        if(pParam->iv_size)
        {
            iv = x_mem_alloc((UINT32)prKernParam->iv_size);
            USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prKernParam->iv, prKernParam->iv_size);
            COPY_FROM_USER_ARG_SIZE(prKernParam->iv, iv, prKernParam->iv_size);
        }
        else
        {
            iv = NULL;
        }

        prKernParam->in_buffer = (const SKB_Byte*)PHYSICAL((UINT32)in_buffer);
        prKernParam->out_buffer = (SKB_Byte*)PHYSICAL((UINT32)out_buffer);
        prKernParam->iv = (const SKB_Byte*)PHYSICAL((UINT32)iv);
        prKernParam->out_buffer_size = (SKB_Size*)PHYSICAL((UINT32)&rSize);

        HalFlushInvalidateDCacheMultipleLine((UINT32)in_buffer, prKernParam->in_buffer_size);
        HalFlushInvalidateDCacheMultipleLine((UINT32)out_buffer, rSize);
        if(iv != NULL)
        {
            HalFlushInvalidateDCacheMultipleLine((UINT32)iv, prKernParam->iv_size);
        }
        HalFlushInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    */

    fgRet = TZ_SKB_Cipher_ProcessBuffer(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    TZ_NWD_BufferEnd((UINT32)prSize, TZ_CACHE_ALIGN(sizeof(SKB_Size)));
    TZ_NWD_BufferEnd((UINT32)in_buffer, TZ_CACHE_ALIGN(pParam->in_buffer_size));
    TZ_NWD_BufferEnd((UINT32)out_buffer, TZ_CACHE_ALIGN(*(SKB_Size *)prSize));
    TZ_NWD_BufferEnd((UINT32)iv, TZ_CACHE_ALIGN(pParam->iv_size));

    /*
        HalInvalidateDCacheMultipleLine((UINT32)in_buffer, prKernParam->in_buffer_size);
        HalInvalidateDCacheMultipleLine((UINT32)out_buffer, rSize);
        if(iv != NULL)
        {
            HalInvalidateDCacheMultipleLine((UINT32)iv, prKernParam->iv_size);
        }
        HalInvalidateDCacheMultipleLine((UINT32)&rSize, sizeof(rSize));
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    */

    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->out_buffer, *(SKB_Size *)prSize);
    COPY_TO_USER_ARG_SIZE(prUserParam->out_buffer, out_buffer, *(SKB_Size *)prSize);
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->out_buffer_size, sizeof(SKB_Size));
    COPY_TO_USER_ARG_SIZE(prUserParam->out_buffer_size, prSize, sizeof(SKB_Size));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    /*
        x_mem_free(in_buffer);
        x_mem_free(out_buffer);
        if(prKernParam->iv_size)
        {
            x_mem_free(iv);
        }
    */

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (prSizePA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prSizePA);
    }
    if (in_bufferPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)in_bufferPA);
    }
    if (out_bufferPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)out_bufferPA);
    }

    if (pParam->iv_size)
    {
        if (ivPA != NULL)
        {
            BSP_FreeAlignedDmaMemory((UINT32)ivPA);
        }
    }

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _SKB_Cipher_Release(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *prKernParam,
                                MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *prUserParam)
{
    MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    SKB_Result rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    fgRet = TZ_SKB_Cipher_Release(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    rRet = pParam->rRet;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->rRet, sizeof(SKB_Result));
    COPY_TO_USER_ARG_SIZE(&prUserParam->rRet, &rRet, sizeof(SKB_Result));

    PRINTK("-%s...........................%d\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}
#endif  // CC_ENABLE_SKB

#ifdef CC_ENABLE_HCI
static BOOL _HCI_Init(MTDMX_MWIF_HCI_INIT_T *prKernParam,
                      MTDMX_MWIF_HCI_INIT_T *prUserParam)
{
    MTDMX_MWIF_HCI_INIT_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_INIT_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_INIT_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));
    fgRet = TZ_HCI_Init(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));
    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....0x%x\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _HCI_Fin(MTDMX_MWIF_HCI_INIT_T *prKernParam,
                     MTDMX_MWIF_HCI_INIT_T *prUserParam)
{
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    fgRet = TZ_HCI_Fin(prKernParam, 0);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    PRINTK("-%s.....\n", __FUNCTION__);

    return TRUE;
}

static BOOL _HCI_Lock(MTDMX_MWIF_HCI_LOCK_T *prKernParam,
                      MTDMX_MWIF_HCI_LOCK_T *prUserParam)
{
    MTDMX_MWIF_HCI_LOCK_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_LOCK_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_LOCK_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_LOCK_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));
    fgRet = TZ_HCI_Lock(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));
    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->pout_lock, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->pout_lock, &pParam->pout_lock, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....0x%x\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_Unlock(MTDMX_MWIF_HCI_UNLOCK_T *prKernParam,
                        MTDMX_MWIF_HCI_UNLOCK_T *prUserParam)
{
    MTDMX_MWIF_HCI_UNLOCK_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_UNLOCK_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_UNLOCK_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_UNLOCK_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));
    fgRet = TZ_HCI_Unlock(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_INIT_T));
    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....0x%x\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_PersonalityKeyLoad(MTDMX_MWIF_HCI_PERSONALITY_KEY_T *prKernParam,
                                    MTDMX_MWIF_HCI_PERSONALITY_KEY_T *prUserParam)
{
    MTDMX_MWIF_HCI_PERSONALITY_KEY_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;
    UINT32 u4Size = 0;

    UINT32 rRet;
    UINT8 *pInData = NULL, *pInDataPA = NULL;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_PERSONALITY_KEY_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));

    if (pParam->pInData != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4InSize);
        pInDataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pInData = (UINT8 *)VIRTUAL((UINT32)(pInDataPA));
        x_memcpy(pInData, pParam->pInData, u4Size);
        pParam->pInData = pInDataPA;
        TZ_NWD_BufferStart((UINT32)pInData, u4Size);
    }

    /*
        pInData = x_mem_alloc(prKernParam->u4InSize);
        x_memcpy(pInData, prKernParam->pInData, prKernParam->u4InSize);
        prKernParam->pInData = (void*)PHYSICAL((UINT32)pInData);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pInData,
            prKernParam->u4InSize);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam,
            sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));
    */
    fgRet = TZ_HCI_PersonalityKeyLoad(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    if (pParam->pInData != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pInData, u4Size);
    }

    /*
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam,
            sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));
    */

    rRet = pParam->u4Ret;

//    x_mem_free(pInData);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....0x%x\n", __FUNCTION__, rRet);

    if (pInDataPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pInDataPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_KeyRefCreate(MTDMX_MWIF_HCI_KEY_REF_T *prKernParam,
                              MTDMX_MWIF_HCI_KEY_REF_T *prUserParam)
{
    MTDMX_MWIF_HCI_KEY_REF_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    BOOL fgRet;

    PRINTK("+%s.....0x%x\n", __FUNCTION__, (UINT32)prKernParam->pKeyRef);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_KEY_REF_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    fgRet = TZ_HCI_KeyRefCreate(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->pKeyRef, sizeof(hci_key_ref_t *));
    COPY_TO_USER_ARG_SIZE(&prUserParam->pKeyRef, &pParam->pKeyRef, sizeof(hci_key_ref_t *));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....0x%x 0x%x\n", __FUNCTION__, rRet, (UINT32)pParam->pKeyRef);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_KeyRefDestroy(MTDMX_MWIF_HCI_KEY_REF_T *prKernParam,
                               MTDMX_MWIF_HCI_KEY_REF_T *prUserParam)
{
    MTDMX_MWIF_HCI_KEY_REF_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    BOOL rRet;
    BOOL fgRet;

    PRINTK("+%s.....0x%x\n", __FUNCTION__, (UINT32)prKernParam->pKeyRef);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_KEY_REF_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    fgRet = TZ_HCI_KeyRefDestroy(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....%d 0x%x\n", __FUNCTION__, rRet, (UINT32)pParam->pKeyRef);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_KeyRefCheck(MTDMX_MWIF_HCI_KEY_REF_T *prKernParam,
                             MTDMX_MWIF_HCI_KEY_REF_T *prUserParam)
{
    MTDMX_MWIF_HCI_KEY_REF_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    BOOL fgRet;

    PRINTK("+%s.....%d\n", __FUNCTION__, (UINT32)prKernParam->pKeyRef);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_KEY_REF_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    fgRet = TZ_HCI_KeyRefCheck(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->bresult, sizeof(BOOL));
    COPY_TO_USER_ARG_SIZE(&prUserParam->bresult, &pParam->bresult, sizeof(BOOL));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....%d result %d 0x%x\n", __FUNCTION__, rRet, pParam->bresult, (UINT32)pParam->pKeyRef);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_ImportKey(MTDMX_MWIF_HCI_IMPORT_KEY_T *prKernParam,
                           MTDMX_MWIF_HCI_IMPORT_KEY_T *prUserParam)
{
    MTDMX_MWIF_HCI_IMPORT_KEY_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    UINT32 rRet;
    hci_key_ref_t rKeyRef;
    BOOL fgRet;
    void *pOAEPParam = NULL, *pOAEPParamPA = NULL;
    void *pIv = NULL, *pIvPA = NULL;
    void *pKeyData = NULL, *pKeyDataPA = NULL;

    PRINTK("+%s.....0x%x\n", __FUNCTION__, prKernParam->pKeyInRef);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_IMPORT_KEY_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_IMPORT_KEY_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_IMPORT_KEY_T));

    if (pParam->pOAEPParam != NULL)
    {

        u4Size = TZ_CACHE_ALIGN(pParam->u4OAEPLen);
        pOAEPParamPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pOAEPParam = (void *)VIRTUAL((UINT32)(pOAEPParamPA));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pOAEPParam, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pOAEPParam, pOAEPParam, u4Size);
        pParam->pOAEPParam = pOAEPParamPA;
        TZ_NWD_BufferStart((UINT32)pOAEPParam, u4Size);
        /*
                pOAEPParam = hci_BinstrAlloc(((binstr_t *)prKernParam->pOAEPParam)->len,
                    ((binstr_t *)prKernParam->pOAEPParam)->buf);
                if(pOAEPParam == NULL)
                {
                    goto _err_exit;
                }
                pOAEPParam->buf = (u_int8_t*)PHYSICAL((UINT32)pOAEPParam->buf);
                prKernParam->pOAEPParam = (void*)PHYSICAL((UINT32)pOAEPParam);
                HalFlushInvalidateDCacheMultipleLine((UINT32)pOAEPParam,
                    pOAEPParam->len + sizeof(binstr_t));
        */
    }
    if (pParam->pIv != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4IvLen);
        pIvPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pIv = (void *)VIRTUAL((UINT32)(pIvPA));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pIv, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pIv, pIv, u4Size);
        pParam->pIv = pIvPA;
        TZ_NWD_BufferStart((UINT32)pIv, u4Size);
    }
    if (pParam->pKeyData != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4KeyLen);
        pKeyDataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pKeyData = (void *)VIRTUAL((UINT32)(pKeyDataPA));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pKeyData, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pKeyData, pKeyData, u4Size);
        pParam->pKeyData = pKeyDataPA;
        TZ_NWD_BufferStart((UINT32)pKeyData, u4Size);
        /*
                pKeyData = hci_BinstrAlloc(((binstr_t *)prKernParam->pKeyData)->len,
                    ((binstr_t *)prKernParam->pKeyData)->buf);
                if(pKeyData == NULL)
                {
                    goto _err_exit;
                }
                pKeyData->buf = (u_int8_t*)PHYSICAL((UINT32)pKeyData->buf);
                prKernParam->pKeyData = (void*)PHYSICAL((UINT32)pKeyData);
                HalFlushInvalidateDCacheMultipleLine((UINT32)pKeyData,
                    pKeyData->len + sizeof(binstr_t));
        */
    }
//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_IMPORT_KEY_T));
    fgRet = TZ_HCI_ImportKey(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_IMPORT_KEY_T));

    if (pParam->pOAEPParam != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pOAEPParam, TZ_CACHE_ALIGN(pParam->u4OAEPLen));
    }
    if (pParam->pIv != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pIv, TZ_CACHE_ALIGN(pParam->u4IvLen));
    }
    if (pParam->pKeyData != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pKeyData, TZ_CACHE_ALIGN(pParam->u4KeyLen));
    }

    rRet = pParam->u4Ret;
    rKeyRef = pParam->pKeyOutRef;
    /*
        if(pOAEPParam != NULL)
        {
            hci_BinstrFree(pOAEPParam);
        }
        if(pKeyData != NULL)
        {
            hci_BinstrFree(pKeyData);
        }
    */
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->pKeyOutRef, sizeof(hci_key_ref_t));
    COPY_TO_USER_ARG_SIZE(&prUserParam->pKeyOutRef, &rKeyRef, sizeof(hci_key_ref_t));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....%d 0x%x\n", __FUNCTION__, rRet, pParam->pKeyOutRef);

    if (pOAEPParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pOAEPParamPA);
    }
    if (pIvPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pIvPA);
    }
    if (pKeyDataPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pKeyDataPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_MoveKeySize(MTDMX_MWIF_HCI_MOVEKEY_T *prKernParam,
                             MTDMX_MWIF_HCI_MOVEKEY_T *prUserParam)
{
    MTDMX_MWIF_HCI_MOVEKEY_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    UINT32 rRet;
    void *pInData = NULL, *pInDataPA = NULL;
    BOOL fgRet;

//    PRINTK("+%s.....%x\n", __FUNCTION__, prKernParam->pKeyRef);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_MOVEKEY_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));

    if (pParam->pInData != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4InDataLen);
        pInDataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pInData = (void *)VIRTUAL((UINT32)(pInDataPA));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pInData, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pInData, pInData, u4Size);
        pParam->pInData = pInDataPA;
        TZ_NWD_BufferStart((UINT32)pInData, u4Size);

        /*
                pInData = hci_BinstrAlloc(((binstr_t *)prKernParam->pInData)->len,
                    ((binstr_t *)prKernParam->pInData)->buf);
                if(pInData == NULL)
                {
                    goto _err_exit;
                }
                pInData->buf = (u_int8_t*)PHYSICAL((UINT32)pInData->buf);
                prKernParam->pInData = (void*)PHYSICAL((UINT32)pInData);
                HalFlushInvalidateDCacheMultipleLine((UINT32)pInData,
                    pInData->len + sizeof(binstr_t));
        */
    }

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam,
//        sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
    fgRet = TZ_HCI_MoveKeySize(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    TZ_NWD_BufferEnd((UINT32)pInData, TZ_CACHE_ALIGN(pParam->u4InDataLen));
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam,
//        sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
    rRet = pParam->u4Ret;
    u4Size = pParam->u4Size;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Size, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Size, &u4Size, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

//    PRINTK("-%s.....%d %d\n", __FUNCTION__, rRet, u4Size);

    if (pInDataPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pInDataPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _HCI_MoveKey(MTDMX_MWIF_HCI_MOVEKEY_T *prKernParam,
                         MTDMX_MWIF_HCI_MOVEKEY_T *prUserParam)
{
    MTDMX_MWIF_HCI_MOVEKEY_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4Size, u4ParamSize;

    UINT32 rRet;
    void *out_data = NULL, *out_dataPA = NULL;
    void *pInData = NULL, *pInDataPA = NULL;
    BOOL fgRet;

//    PRINTK("+%s.....0x%x\n", __FUNCTION__, prKernParam->pKeyRef);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_MOVEKEY_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));

    if (pParam->pInData != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4InDataLen);
        pInDataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pInData = (void *)VIRTUAL((UINT32)(pInDataPA));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pInData, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pInData, pInData, u4Size);
        pParam->pInData = pInDataPA;
        TZ_NWD_BufferStart((UINT32)pInData, u4Size);
        /*
                pInData = hci_BinstrAlloc(((binstr_t *)prKernParam->pInData)->len,
                    ((binstr_t *)prKernParam->pInData)->buf);
                if(pInData == NULL)
                {
                    goto _err_exit;
                }
                pInData->buf = (u_int8_t*)PHYSICAL((UINT32)pInData->buf);
                prKernParam->pInData = (void*)PHYSICAL((UINT32)pInData);
                HalFlushInvalidateDCacheMultipleLine((UINT32)pInData,
                    pInData->len + sizeof(binstr_t));
        */
    }

    u4Size = TZ_CACHE_ALIGN(pParam->u4Size);
    out_dataPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
    out_data = (void *)VIRTUAL((UINT32)(out_dataPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pOutData, u4Size);
    COPY_FROM_USER_ARG_SIZE(pParam->pOutData, out_data, u4Size);
    pParam->pOutData = out_dataPA;
    TZ_NWD_BufferStart((UINT32)out_data, u4Size);

    /*
        out_data = hci_BinstrAlloc(prKernParam->u4Size, NULL);
        if(out_data == NULL)
        {
            goto _err_exit;
        }
        prKernParam->pOutData = (void*)PHYSICAL((UINT32)out_data->buf);
        HalFlushInvalidateDCacheMultipleLine((UINT32)out_data->buf,
            prKernParam->u4Size);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam,
            sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
    */
    fgRet = TZ_HCI_MoveKey(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
    TZ_NWD_BufferEnd((UINT32)pInData, TZ_CACHE_ALIGN(pParam->u4InDataLen));
    TZ_NWD_BufferEnd((UINT32)out_data, TZ_CACHE_ALIGN(pParam->u4Size));

    /*
        HalInvalidateDCacheMultipleLine((UINT32)out_data->buf,
            prKernParam->u4Size);
        HalInvalidateDCacheMultipleLine((UINT32)prKernParam,
            sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
    */
    rRet = pParam->u4Ret;
    u4Size = pParam->u4Size;
    /*
        if(pInData != NULL)
        {
            hci_BinstrFree(pInData);
        }
    */
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->pKeyRef, sizeof(void *));
    COPY_TO_USER_ARG_SIZE(&prUserParam->pKeyRef, &pParam->pKeyRef, sizeof(void *));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pOutData, u4Size);
    COPY_TO_USER_ARG_SIZE(prUserParam->pOutData, out_data, u4Size);
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));
//    hci_BinstrFree(out_data);

//    PRINTK("-%s.....%d %d\n", __FUNCTION__, rRet, u4Size);

    if (pInDataPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pInDataPA);
    }
    if (out_dataPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)out_dataPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _HCI_FreeMem(MTDMX_MWIF_HCI_FREEMEM_T *prKernParam,
                         MTDMX_MWIF_HCI_FREEMEM_T *prUserParam)
{
    MTDMX_MWIF_HCI_FREEMEM_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    BOOL fgRet;

    PRINTK("+%s.....0x%x\n", __FUNCTION__, prKernParam->pPtr);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_FREEMEM_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_FREEMEM_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_FREEMEM_T));

    fgRet = TZ_HCI_FreeMem(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    rRet = pParam->u4Ret;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....0x%x\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }
    return TRUE;
}

static BOOL _HCI_DevIdGetSize(MTDMX_MWIF_HCI_ID_T *prKernParam,
                              MTDMX_MWIF_HCI_ID_T *prUserParam)
{
    MTDMX_MWIF_HCI_ID_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    UINT32 u4IdSize;
    BOOL fgRet;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_ID_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_ID_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_ID_T));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_ID_T));
    fgRet = TZ_HCI_DevIdsGetSize(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_ID_T));
    rRet = pParam->u4Ret;
    u4IdSize = pParam->u4IdSize;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4IdSize, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4IdSize, &u4IdSize, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....%d\n", __FUNCTION__, rRet);

    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _HCI_DevIdGetName(MTDMX_MWIF_HCI_ID_T *prKernParam,
                              MTDMX_MWIF_HCI_ID_T *prUserParam)
{
    MTDMX_MWIF_HCI_ID_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    UINT32 rRet;
    UINT32 u4IdSize;
    BOOL fgRet;
    void *pBuf = NULL, *pBufPA = NULL;

    PRINTK("+%s.....\n", __FUNCTION__);

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_HCI_ID_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_HCI_ID_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_HCI_ID_T));

    /*
        pSubBinstr = hci_BinstrAlloc(prKernParam->u4IdSize, NULL);
        if(pSubBinstr == NULL)
        {
            goto _err_exit;
        }
        HalFlushInvalidateDCacheMultipleLine((UINT32)pSubBinstr->buf, prKernParam->u4IdSize);
        u4IdSize = prKernParam->u4IdSize;
        prKernParam->pBuf = (void*)PHYSICAL((UINT32)pSubBinstr->buf);
        HalFlushInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_ID_T));
    */

    u4IdSize = TZ_CACHE_ALIGN(pParam->u4IdSize);
    pBufPA = (void *)BSP_AllocAlignedDmaMemory(u4IdSize, TZ_CACHE_LINE_SIZE);
    pBuf = (void *)VIRTUAL((UINT32)(pBufPA));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pBuf, u4IdSize);
    COPY_FROM_USER_ARG_SIZE(pParam->pBuf, pBuf, u4IdSize);
    pParam->pBuf = pBufPA;
    TZ_NWD_BufferStart((UINT32)pBuf, u4IdSize);

    fgRet = TZ_HCI_DevIdsGetName(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    TZ_NWD_BufferEnd((UINT32)pBuf, u4IdSize);

//    HalInvalidateDCacheMultipleLine((UINT32)pSubBinstr->buf, prKernParam->u4IdSize);
//    HalInvalidateDCacheMultipleLine((UINT32)prKernParam, sizeof(MTDMX_MWIF_HCI_ID_T));

    rRet = pParam->u4Ret;
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pBuf, u4IdSize);
    COPY_TO_USER_ARG_SIZE(prUserParam->pBuf, pBuf, pParam->u4IdSize);

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4Ret, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4Ret, &rRet, sizeof(UINT32));

    PRINTK("-%s.....%d\n", __FUNCTION__, rRet);

    if (pBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pBufPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


#endif // #ifdef CC_ENABLE_HCI

#ifdef CC_ENABLE_FUJ_MARLIN
static BOOL _DRM_TZ_LoadSecretDataType(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *prKernParam,
                                       MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *prUserParam)
{
    BOOL fgRet;
    DRM_ErrorInf *prErrorInf = NULL, *prErrorInfPA = NULL;
    MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *pParam = NULL, *pParamPA = NULL;
    void *pRootCertBuf = NULL, *pRootCertBufPA = NULL;
    void *pCaCertBuf = NULL, *pCaCertBufPA = NULL;
    void *pClientCertBuf = NULL, *pClientCertBufPA = NULL;
    void *pClientKeyBuf = NULL, *pClientKeyBufPA = NULL;
    UINT32 u4Size, u4ParamSize;

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T));

    if (pParam->prErrorInf != NULL)
    {
        prErrorInfPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)), TZ_CACHE_LINE_SIZE);
        prErrorInf = (DRM_ErrorInf *)VIRTUAL((UINT32)(prErrorInfPA));
        pParam->prErrorInf = prErrorInfPA;
        TZ_NWD_BufferStart((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if (pParam->pRootCertBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4RootCertLen);
        pRootCertBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pRootCertBuf = (void *)VIRTUAL((UINT32)pRootCertBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pRootCertBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pRootCertBuf, pRootCertBuf, u4Size);
        pParam->pRootCertBuf = pRootCertBufPA;
        TZ_NWD_BufferStart((UINT32)pRootCertBuf, u4Size);
    }
    if (pParam->pCaCertBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4CaCertLen);
        pCaCertBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pCaCertBuf = (void *)VIRTUAL((UINT32)pCaCertBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pCaCertBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pCaCertBuf, pCaCertBuf, u4Size);
        pParam->pCaCertBuf = pCaCertBufPA;
        TZ_NWD_BufferStart((UINT32)pCaCertBuf, u4Size);
    }
    if (pParam->pClientCertBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4ClientCertLen);
        pClientCertBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pClientCertBuf = (void *)VIRTUAL((UINT32)pClientCertBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pClientCertBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pClientCertBuf, pClientCertBuf, u4Size);
        pParam->pClientCertBuf = pClientCertBufPA;
        TZ_NWD_BufferStart((UINT32)pClientCertBuf, u4Size);
    }
    if (pParam->pClientKeyBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4ClientKeyLen);
        pClientKeyBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pClientKeyBuf = (void *)VIRTUAL((UINT32)pClientKeyBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pClientKeyBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pClientKeyBuf, pClientKeyBuf, u4Size);
        pParam->pClientKeyBuf = pClientKeyBufPA;
        TZ_NWD_BufferStart((UINT32)pClientKeyBuf, u4Size);
    }

    fgRet = TZ_DRM_TZ_LoadSecretDataType(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    if (pParam->prErrorInf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prErrorInf, sizeof(DRM_ErrorInf));
        COPY_TO_USER_ARG_SIZE(prUserParam->prErrorInf, prErrorInf, sizeof(DRM_ErrorInf));
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->i4Ret, sizeof(INT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->i4Ret, &pParam->i4Ret, sizeof(INT32));

    if (pRootCertBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pRootCertBufPA);
    }
    if (pCaCertBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pCaCertBufPA);
    }
    if (pClientCertBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pClientCertBufPA);
    }
    if (pClientKeyBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pClientKeyBufPA);
    }
    if (prErrorInfPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prErrorInfPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}


static BOOL _DRM_TZ_SetSecretDataType(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *prKernParam,
                                      MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *prUserParam)
{
    BOOL fgRet;
    DRM_ErrorInf *prErrorInf = NULL, *prErrorInfPA = NULL;
    MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *pParam = NULL, *pParamPA = NULL;
    void *pRootCertBuf = NULL, *pRootCertBufPA = NULL;
    void *pCaCertBuf = NULL, *pCaCertBufPA = NULL;
    void *pClientCertBuf = NULL, *pClientCertBufPA = NULL;
    void *pClientKeyBuf = NULL, *pClientKeyBufPA = NULL;
    UINT32 u4Size, u4ParamSize;

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T));

    if (pParam->prErrorInf != NULL)
    {
        prErrorInfPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)), TZ_CACHE_LINE_SIZE);
        prErrorInf = (DRM_ErrorInf *)VIRTUAL((UINT32)(prErrorInfPA));
        pParam->prErrorInf = prErrorInfPA;
        TZ_NWD_BufferStart((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if (pParam->pRootCertBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4RootCertBufLen);
        pRootCertBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pRootCertBuf = (void *)VIRTUAL((UINT32)pRootCertBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pRootCertBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pRootCertBuf, pRootCertBuf, u4Size);
        pParam->pRootCertBuf = pRootCertBufPA;
        TZ_NWD_BufferStart((UINT32)pRootCertBuf, u4Size);
    }
    if (pParam->pCaCertBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4CaCertBufLen);
        pCaCertBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pCaCertBuf = (void *)VIRTUAL((UINT32)pCaCertBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pCaCertBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pCaCertBuf, pCaCertBuf, u4Size);
        pParam->pCaCertBuf = pCaCertBufPA;
        TZ_NWD_BufferStart((UINT32)pCaCertBuf, u4Size);
    }
    if (pParam->pClientCertBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4ClientCertBufLen);
        pClientCertBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pClientCertBuf = (void *)VIRTUAL((UINT32)pClientCertBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pClientCertBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pClientCertBuf, pClientCertBuf, u4Size);
        pParam->pClientCertBuf = pClientCertBufPA;
        TZ_NWD_BufferStart((UINT32)pClientCertBuf, u4Size);
    }
    if (pParam->pClientKeyBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4ClientKeyBufLen);
        pClientKeyBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pClientKeyBuf = (void *)VIRTUAL((UINT32)pClientKeyBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pClientKeyBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pClientKeyBuf, pClientKeyBuf, u4Size);
        pParam->pClientKeyBuf = pClientKeyBufPA;
        TZ_NWD_BufferStart((UINT32)pClientKeyBuf, u4Size);
    }

    fgRet = TZ_DRM_TZ_SetSecretDataType(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    if (pParam->prErrorInf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prErrorInf, sizeof(DRM_ErrorInf));
        COPY_TO_USER_ARG_SIZE(prUserParam->prErrorInf, prErrorInf, sizeof(DRM_ErrorInf));
    }
    if (pParam->pRootCertBuf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pRootCertBuf, TZ_CACHE_ALIGN(pParam->u4RootCertLen));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pRootCertBuf, pParam->u4RootCertLen);
        COPY_TO_USER_ARG_SIZE(prUserParam->pRootCertBuf, pRootCertBuf, pParam->u4RootCertLen);
    }
    if (pParam->pCaCertBuf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pCaCertBuf, TZ_CACHE_ALIGN(pParam->u4CaCertLen));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pCaCertBuf, pParam->u4CaCertLen);
        COPY_TO_USER_ARG_SIZE(prUserParam->pCaCertBuf, pCaCertBuf, pParam->u4CaCertLen);
    }
    if (pParam->pClientCertBuf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pClientCertBuf, TZ_CACHE_ALIGN(pParam->u4ClientCertLen));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pClientCertBuf, pParam->u4ClientCertLen);
        COPY_TO_USER_ARG_SIZE(prUserParam->pClientCertBuf, pClientCertBuf, pParam->u4ClientCertLen);
    }
    if (pParam->pClientKeyBuf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)pClientKeyBuf, TZ_CACHE_ALIGN(pParam->u4ClientKeyLen));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pClientKeyBuf, pParam->u4ClientKeyLen);
        COPY_TO_USER_ARG_SIZE(prUserParam->pClientKeyBuf, pClientKeyBuf, pParam->u4ClientKeyLen);
    }
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4RootCertLen, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4RootCertLen, &pParam->u4RootCertLen, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4CaCertLen, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4CaCertLen, &pParam->u4CaCertLen, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4ClientCertLen, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4ClientCertLen, &pParam->u4ClientCertLen, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4ClientKeyLen, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4ClientKeyLen, &pParam->u4ClientKeyLen, sizeof(UINT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->i4Ret, sizeof(INT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->i4Ret, &pParam->i4Ret, sizeof(INT32));

    if (pRootCertBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pRootCertBufPA);
    }
    if (pCaCertBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pCaCertBufPA);
    }
    if (pClientCertBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pClientCertBufPA);
    }
    if (pClientKeyBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pClientKeyBufPA);
    }
    if (prErrorInfPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prErrorInfPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _DRM_TZ_Open(MTDMX_MWIF_DRM_TZ_OPEN_T *prKernParam,
                         MTDMX_MWIF_DRM_TZ_OPEN_T *prUserParam)
{
    BOOL fgRet;
    DRM_ErrorInf *prErrorInf = NULL, *prErrorInfPA = NULL;
    MTDMX_MWIF_DRM_TZ_OPEN_T *pParam = NULL, *pParamPA = NULL;
    DRM_ExtractInfo *prExtractInfo, *prExtractInfoPA = NULL;
    void *pLicenseObj = NULL, *pLicenseObjPA = NULL;
    void *pLid = NULL, *pLidPA = NULL;
    UINT32 u4Size, u4ParamSize;

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_DRM_TZ_OPEN_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_DRM_TZ_OPEN_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_DRM_TZ_OPEN_T));

    if (pParam->prErrorInf != NULL)
    {
        prErrorInfPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)), TZ_CACHE_LINE_SIZE);
        prErrorInf = (DRM_ErrorInf *)VIRTUAL((UINT32)(prErrorInfPA));
        pParam->prErrorInf = prErrorInfPA;
        TZ_NWD_BufferStart((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if (pParam->pExtractInfo != NULL)
    {
        prExtractInfoPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(DRM_ExtractInfo)), TZ_CACHE_LINE_SIZE);
        prExtractInfo = (DRM_ExtractInfo *)VIRTUAL((UINT32)(prExtractInfoPA));
        pParam->pExtractInfo = prExtractInfoPA;
        TZ_NWD_BufferStart((UINT32)prExtractInfo, TZ_CACHE_ALIGN(sizeof(DRM_ExtractInfo)));
    }
    if (pParam->pLicenseObj != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4LicenseObjLen);
        pLicenseObjPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pLicenseObj = (void *)VIRTUAL((UINT32)pLicenseObjPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pLicenseObj, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pLicenseObj, pLicenseObj, u4Size);
        pParam->pLicenseObj = pLicenseObjPA;
        TZ_NWD_BufferStart((UINT32)pLicenseObj, u4Size);
    }
    if (pParam->pLid != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4LidLen);
        pLidPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pLid = (void *)VIRTUAL((UINT32)pLidPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pLid, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pLid, pLid, u4Size);
        pParam->pLid = pLidPA;
        TZ_NWD_BufferStart((UINT32)pLid, u4Size);
    }

    fgRet = TZ_DRM_TZ_Open(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    if (pParam->prErrorInf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prErrorInf, sizeof(DRM_ErrorInf));
        COPY_TO_USER_ARG_SIZE(prUserParam->prErrorInf, prErrorInf, sizeof(DRM_ErrorInf));
    }
    if (pParam->pExtractInfo != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)prExtractInfo, TZ_CACHE_ALIGN(sizeof(DRM_ExtractInfo)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pExtractInfo, sizeof(DRM_ExtractInfo));
        COPY_TO_USER_ARG_SIZE(prUserParam->pExtractInfo, prExtractInfo, sizeof(DRM_ExtractInfo));
    }
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->i4Ret, sizeof(INT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->i4Ret, &pParam->i4Ret, sizeof(INT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->i4DecSlot, sizeof(INT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->i4DecSlot, &pParam->i4DecSlot, sizeof(INT32));

    if (pLicenseObjPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pLicenseObjPA);
    }
    if (pLidPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pLidPA);
    }
    if (prErrorInfPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prErrorInfPA);
    }
    if (prExtractInfoPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prExtractInfoPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _DRM_TZ_Close(MTDMX_MWIF_DRM_TZ_OPEN_T *prKernParam,
                          MTDMX_MWIF_DRM_TZ_OPEN_T *prUserParam)
{
    BOOL fgRet;
    DRM_ErrorInf *prErrorInf = NULL, *prErrorInfPA = NULL;
    MTDMX_MWIF_DRM_TZ_OPEN_T *pParam = NULL, *pParamPA = NULL;
    UINT32 u4ParamSize;

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_DRM_TZ_OPEN_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_DRM_TZ_OPEN_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_DRM_TZ_OPEN_T));

    if (pParam->prErrorInf != NULL)
    {
        prErrorInfPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)), TZ_CACHE_LINE_SIZE);
        prErrorInf = (DRM_ErrorInf *)VIRTUAL((UINT32)(prErrorInfPA));
        pParam->prErrorInf = prErrorInfPA;
        TZ_NWD_BufferStart((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }

    fgRet = TZ_DRM_TZ_Close(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    if (pParam->prErrorInf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prErrorInf, sizeof(DRM_ErrorInf));
        COPY_TO_USER_ARG_SIZE(prUserParam->prErrorInf, prErrorInf, sizeof(DRM_ErrorInf));
    }
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->i4Ret, sizeof(INT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->i4Ret, &pParam->i4Ret, sizeof(INT32));

    if (prErrorInfPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prErrorInfPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}

static BOOL _DRM_TZ_DecodePacketsVOD(MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T *prKernParam,
                                     MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T *prUserParam)
{
    BOOL fgRet;
    DRM_ErrorInf *prErrorInf = NULL, *prErrorInfPA = NULL;
    MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T *pParam = NULL, *pParamPA = NULL;
    DecoderDRM_Offset *pOffsets = NULL, *pOffsetsPA = NULL;
    void *pInBuf = NULL, *pInBufPA = NULL;
    void *pOutBuf = NULL, *pOutBufPA = NULL;
    UINT32 u4Size, u4ParamSize;

    u4ParamSize = TZ_CACHE_ALIGN(sizeof(MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T));
    pParamPA = (void *)BSP_AllocAlignedDmaMemory(u4ParamSize, TZ_CACHE_LINE_SIZE);
    pParam = (MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T *)VIRTUAL((UINT32)(pParamPA));
    x_memcpy(pParam, prKernParam, sizeof(MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T));

    if (pParam->prErrorInf != NULL)
    {
        prErrorInfPA = (void *)BSP_AllocAlignedDmaMemory(TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)), TZ_CACHE_LINE_SIZE);
        prErrorInf = (DRM_ErrorInf *)VIRTUAL((UINT32)(prErrorInfPA));
        pParam->prErrorInf = prErrorInfPA;
        TZ_NWD_BufferStart((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }

    if (pParam->pInBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN((pParam->u4PacketSize) * (pParam->u4InNumOfPackets));
        pInBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pInBuf = (void *)VIRTUAL((UINT32)pInBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pInBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pInBuf, pInBuf, u4Size);
        pParam->pInBuf = pInBufPA;
        TZ_NWD_BufferStart((UINT32)pInBuf, u4Size);
    }
    if (pParam->pOutBuf != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(pParam->u4OutBufLen);
        pOutBufPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pOutBuf = (void *)VIRTUAL((UINT32)pOutBufPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pOutBuf, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pOutBuf, pOutBuf, u4Size);
        pParam->pOutBuf = pOutBufPA;
        TZ_NWD_BufferStart((UINT32)pOutBuf, u4Size);
    }
    if (pParam->pOffsets != NULL)
    {
        u4Size = TZ_CACHE_ALIGN(sizeof(DecoderDRM_Offset) * (pParam->u4InNumOfPackets));
        pOffsetsPA = (void *)BSP_AllocAlignedDmaMemory(u4Size, TZ_CACHE_LINE_SIZE);
        pOffsets = (void *)VIRTUAL((UINT32)pOffsetsPA);
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pParam->pOffsets, u4Size);
        COPY_FROM_USER_ARG_SIZE(pParam->pOffsets, pOffsets, u4Size);
        pParam->pOffsets = pOffsetsPA;
        TZ_NWD_BufferStart((UINT32)pOffsets, u4Size);
    }

    fgRet = TZ_DRM_TZ_DecodePacketsVOD(pParam, u4ParamSize);
    if (!fgRet)
    {
        PRINTK("TZ-%s.....\n", __FUNCTION__);
    }

    if (pParam->prErrorInf != NULL)
    {
        TZ_NWD_BufferEnd((UINT32)prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->prErrorInf, sizeof(DRM_ErrorInf));
        COPY_TO_USER_ARG_SIZE(prUserParam->prErrorInf, prErrorInf, sizeof(DRM_ErrorInf));
    }
    if (pParam->pOutBuf != NULL)
    {
        // copy data to pOutBuf
        TZ_NWD_BufferEnd((UINT32)pOutBuf,
                         TZ_CACHE_ALIGN((pParam->u4PacketSize) * (pParam->u4OutNumOfPackets)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pOutBuf,
                                           (pParam->u4PacketSize) * (pParam->u4OutNumOfPackets));
        COPY_TO_USER_ARG_SIZE(prUserParam->pOutBuf, pOutBuf,
                              (pParam->u4PacketSize) * (pParam->u4OutNumOfPackets));
    }
    else
    {
        // copy data to pInBuf
        TZ_NWD_BufferEnd((UINT32)pInBuf,
                         TZ_CACHE_ALIGN((pParam->u4PacketSize) * (pParam->u4OutNumOfPackets)));
        USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(prUserParam->pInBuf,
                                           (pParam->u4PacketSize) * (pParam->u4OutNumOfPackets));
        COPY_TO_USER_ARG_SIZE(prUserParam->pInBuf, pInBuf,
                              (pParam->u4PacketSize) * (pParam->u4OutNumOfPackets));
    }
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->i4Ret, sizeof(INT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->i4Ret, &pParam->i4Ret, sizeof(INT32));
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(&prUserParam->u4OutNumOfPackets, sizeof(UINT32));
    COPY_TO_USER_ARG_SIZE(&prUserParam->u4OutNumOfPackets, &pParam->u4OutNumOfPackets,
                          sizeof(UINT32));

    if (pInBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pInBufPA);
    }
    if (pOutBufPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pOutBufPA);
    }
    if (pOffsetsPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pOffsetsPA);
    }
    if (prErrorInfPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)prErrorInfPA);
    }
    if (pParamPA != NULL)
    {
        BSP_FreeAlignedDmaMemory((UINT32)pParamPA);
    }

    return TRUE;
}
#endif // CC_ENABLE_FUJ_MARLIN
//-----------------------------------------------------------------------------
/*
 *  Find PID index for a given PID value
 *
 *  @param  u2Pid           PID
 *  @param  pu1PidIndex [out]
 *                          The PID index if found
 */
//-----------------------------------------------------------------------------
MT_RESULT_T MTDMX_MWIF_Call(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    void *pvUserParam;
    void *pvKernParam;
    UINT32 u4Size;
    MT_RESULT_T rRet;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pvUserParam = (void *)rArg.ai4Arg[0];
    u4Size = (UINT32)rArg.ai4Arg[1];

    pvKernParam = x_mem_alloc(u4Size);

    if (pvKernParam == 0)
    {
        PRINTK("Alloc pvKernParam buffer failed.....\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pvUserParam, u4Size);
    COPY_FROM_USER_ARG_SIZE(pvUserParam, pvKernParam, u4Size);

    switch (*(UINT32 *)pvKernParam) // the first 4 bytes is interface ID
    {
    case MTDMX_MWIF_MUL_FREEPID_ID:
        rRet = _MTDMX_MUL_FreePidx(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MUL_SETPIDXINSTANCE_ID:
        rRet = _MTDMX_MUL_SetPidxInstance(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MUL_GETAVAILINST_ID:
        rRet = _MTDMX_MUL_GetAvailableInst(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MUL_GETTSINDEX_ID:
        rRet = _MTDMX_MUL_GetTsIndex(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MUL_FREEINST_ID:
        rRet = _MTDMX_MUL_FreeInst(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MUL_GETPIDXINSTID_ID:
        rRet = _MTDMX_MUL_GetPidxInstId(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MUL_SETSUBTITLEPIDX_ID:
        rRet = _MTDMX_MUL_SetSubtitlePidx(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MM_SET_ID:
        rRet = _MTDMX_MM_Set(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MM_GET_ID:
        rRet = _MTDMX_MM_Get(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MM_FREE_ID:
        rRet = _MTDMX_MM_Free(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MM_FLUSHBUFFER_ID:
        rRet = _MTDMX_MM_FlushBuffer(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MM_ALLOCATEBUFFER_ID:
        rRet = _MTDMX_MM_AllocateBuffer(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_MM_FREEBUFFER_ID:
        rRet = _MTDMX_MM_FreeBuffer(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_PS_SETSTREAM_ID:
        rRet = _MTDMX_PS_SetSTreamId(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_GETVIDEOCHANNEL_ID:
        rRet = _MTDMX_GetVideoChannel(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_GETVIDEOTYPE_ID:
        rRet = _MTFVR_GetVideoType(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SETPID_ID:
        rRet = _MTFVR_SetPid(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_GETPID_ID:
        rRet = _MTFVR_GetPid(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SWAPFREE_ID:
        rRet = _MTFVR_SwapFreePattern(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SWAPSETNULL_ID:
        rRet = _MTFVR_SwapSetNullPattern(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SWAPSET_ID:
        rRet = _MTFVR_SwapSetPattern(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SETAESKEY_ID:
        rRet = _MTFVR_SetAesKey(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SETCRYPTOKEY_ID:
        rRet = _MTFVR_SetCryptoKey(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_GETGBL_ID:
        rRet = _MTFVR_GetGBL(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_SETGBL_ID:
        rRet = _MTFVR_SetGBL(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_INIT_ID:
        rRet = _MTFVR_Init(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_START_ID:
        rRet = _MTFVR_Start(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_STOP_ID:
        rRet = _MTFVR_Stop(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_ISREC_ID:
        rRet = _MTFVR_IsRecording(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_UNLOCK_BUFFER_ID:
        rRet = _MTFVR_UnlockBuffer(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_FLUSH_BUFFER_ID:
        rRet = _MTFVR_FlushBuffer(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_FREEPID_ID:
        rRet = _MTFVR_FreePid(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_FVR_FREEGBL_ID:
        rRet = _MTFVR_FreeGBL(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SETPACKETSIZE_ID:
        rRet = _MTDMX_SetPacketSize(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SETCUSTFRONTEND_ID:
        rRet = _MTDMX_SetCustFrontEnd(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_PES_FREEBFR:
        rRet = _MTDMX_PES_FreeBufferEx(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_PSI_FREEBFR:
        rRet = _MTDMX_PSI_FreeBufferEx(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;

#ifdef CC_ENABLE_SKB
    case MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE:
        rRet = _SKB_GetInstance(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_RELEASE:
        rRet = _SKB_Engine_Release(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_GET_INFO:
        rRet = _SKB_GetInfo(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_GET_PROPERTIES_SIZE:
        rRet = _SKB_GetProertiesSize(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_GET_PROPERTIES_DATA:
        rRet = _SKB_GetProertiesData(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED:
        rRet = _SKB_DataFromWrapped(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED:
        rRet = _SKB_DataFromExported(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM:
        rRet = _SKB_CreateTransform(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER:
        rRet = _SKB_CreateCipher(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_SECUREDATA_RELEASE:
        rRet = _SKB_SecureData_Release(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_SECUREDATA_GETINFO:
        rRet = _SKB_SecureDataGetInfo(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_SECUREDATA_EXPORT:
        rRet = _SKB_SecureData_Export(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_SECUREDATA_DERIVE:
        rRet = _SKB_SecureData_Derive(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_TRANSFORM_RELEASE:
        rRet = _SKB_Transform_Release(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES:
        rRet = _SKB_Transform_AddBytes(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA:
        rRet = _SKB_Transform_AddSecureData(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT:
        rRet = _SKB_Transform_GetOutput(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER:
        rRet = _SKB_Cipher_ProcessBuffer(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_SKB_CIPHER_RELEASE:
        rRet = _SKB_Cipher_Release(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
#endif // CC_ENABLE_SKB
#ifdef CC_ENABLE_HCI
    case MTDMX_MWIF_HCI_INIT:
        rRet = _HCI_Init(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_FIN:
        rRet = _HCI_Fin(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_LOCK:
        rRet = _HCI_Lock(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_UNLOCK:
        rRet = _HCI_Unlock(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_PERSONALITYKEY_LOAD:
        rRet = _HCI_PersonalityKeyLoad(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_KEYREF_CREATE:
        rRet = _HCI_KeyRefCreate(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_KEYREF_DESTROY:
        rRet = _HCI_KeyRefDestroy(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_KEYREF_CHECK:
        rRet = _HCI_KeyRefCheck(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_IMPORT_KEY:
        rRet = _HCI_ImportKey(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_MOVE_KEY_SIZE:
        rRet = _HCI_MoveKeySize(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_MOVE_KEY:
        rRet = _HCI_MoveKey(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_FREE_MEM:
        rRet = _HCI_FreeMem(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_DEVICE_GET_SIZE:
        rRet = _HCI_DevIdGetSize(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_HCI_DEVICE_GET_NAME:
        rRet = _HCI_DevIdGetName(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
#endif // CC_ENABLE_HCI
#ifdef CC_ENABLE_FUJ_MARLIN
    case MTDMX_MWIF_DRM_TZ_LOADSECRETDATATYPE:
        rRet = _DRM_TZ_LoadSecretDataType(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE:
        rRet = _DRM_TZ_SetSecretDataType(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_DRM_TZ_OPEN:
        rRet = _DRM_TZ_Open(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_DRM_TZ_CLOSE:
        rRet = _DRM_TZ_Close(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
    case MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD:
        rRet = _DRM_TZ_DecodePacketsVOD(pvKernParam, pvUserParam)?MTR_OK:MTR_NOT_OK;
        break;
#endif // CC_ENABLE_FUJ_MARLIN
    default:
        rRet = MTR_NOT_OK;
        break;
    }

    if (rRet == MTR_NOT_OK)
    {
        PRINTK("mtal_ioctl_mtdmx failed.....\n");
    }

    x_mem_free(pvKernParam);

    return rRet;
}


