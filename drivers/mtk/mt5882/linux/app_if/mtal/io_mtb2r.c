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
 * $RCSfile: io_mtstc.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtstc.c
 *  This file contains implementation of exported APIs of STC module.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------


#include "mtb2r.h"
#include "mtb2r_mwif.h"
#include "mtstc.h"
#include "stc_drvif.h"
#include "b2r_if.h"
#include "b2r_drvif.h"
#include "fbm_drvif.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>
#include "dmx_pcr.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTB2R_VIRTUAL_FB_NUM (4)
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


typedef struct
{
    UCHAR ucFbgId;
    UCHAR ucEsId;
} MTB2R_VIRTUAL_FBG;

//-----------------------------------------------------------------------------
// Define definitions
//-----------------------------------------------------------------------------

EXTERN UINT32 _u4DrvVideoGetMute(UINT32 u4Path);

#define B2R_USE_HW_DECODER 1

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(int)))                            \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_video valid argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_DATA(ArgUsr, ArgKernel, size)              \
    if (copy_to_user((void __user *)ArgUsr, (void *)ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_DATA(ArgUsr, ArgKernel, size)              \
    if (copy_from_user((void *)ArgKernel,(void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MTB2R_INVALID_ES_ID 0xFF
#define MTB2R_INVALID_FBG_ID 0xFF

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

MTB2R_VIRTUAL_FBG _arMtb2rFbg[FBG_MAX_NS] = {{0xFF}};


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTB2R_GetPts(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT32* pu4PtsValue;
    UINT32 u4PtsValue;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    pu4PtsValue = (UINT32*)(rArg.ai4Arg[1]);

    u4PtsValue = VDP_GetPts(ucVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4PtsValue,UINT32);
    COPY_TO_USER_ARG(pu4PtsValue, u4PtsValue, UINT32);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_GetDispDrvStable(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT8* pu1DispDrvStable;
    UINT8 u1DispDrvStable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    pu1DispDrvStable = (UINT8*)(rArg.ai4Arg[1]);

    u1DispDrvStable = !_u4DrvVideoGetMute(ucVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1DispDrvStable,UINT8);
    COPY_TO_USER_ARG(pu1DispDrvStable, u1DispDrvStable, UINT8);

    return MTR_OK;
}


static MT_RESULT_T _MTB2R_GetStepForward(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT8* pu1StepFroward;
    UINT8 u1StepFroward;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    pu1StepFroward = (UINT8*)(rArg.ai4Arg[1]);

    u1StepFroward = VDP_GetStepForward(ucVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1StepFroward,UINT8);
    COPY_TO_USER_ARG(pu1StepFroward, u1StepFroward, UINT8);

    return MTR_OK;
}
static MT_RESULT_T _MTB2R_GetPosInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_POS_INFO_T* prPosInfo;
    MTB2R_POS_INFO_T rPosInfo;

    x_memset(&rPosInfo,0,sizeof(MTB2R_POS_INFO_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    prPosInfo = (MTB2R_POS_INFO_T*)(rArg.ai4Arg[1]);

    VDP_GetPosInfo(ucVdpId,(VDP_POS_INTO_T*)&rPosInfo);

    USR_SPACE_ACCESS_VALIDATE_ARG(prPosInfo,MTB2R_POS_INFO_T);
    COPY_TO_USER_ARG(prPosInfo, rPosInfo, MTB2R_POS_INFO_T);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_GetCaptureInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_CAPTURE_INFO_T* prCaptureInfo;
    MTB2R_CAPTURE_INFO_T rCaptureInfo;

    x_memset(&rCaptureInfo,0,sizeof(MTB2R_CAPTURE_INFO_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    prCaptureInfo = (MTB2R_CAPTURE_INFO_T*)(rArg.ai4Arg[1]);

    VDP_GetLockFrameBufferAddr(ucVdpId,(VDP_CAPTURE_INTO_T*)&rCaptureInfo);

    USR_SPACE_ACCESS_VALIDATE_ARG(prCaptureInfo,MTB2R_CAPTURE_INFO_T);
    COPY_TO_USER_ARG(prCaptureInfo, rCaptureInfo, MTB2R_CAPTURE_INFO_T);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_GetCaptureInfo2(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_CAPTURE_INFO_T* prCaptureInfo;
    MTB2R_CAPTURE_INFO_T rCaptureInfo;

    x_memset(&rCaptureInfo,0,sizeof(MTB2R_CAPTURE_INFO_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    prCaptureInfo = (MTB2R_CAPTURE_INFO_T*)(rArg.ai4Arg[1]);

    VDP_GetDisplayFrameBufferAddr(ucVdpId,(VDP_CAPTURE_INTO_T*)&rCaptureInfo);

    USR_SPACE_ACCESS_VALIDATE_ARG(prCaptureInfo,MTB2R_CAPTURE_INFO_T);
    COPY_TO_USER_ARG(prCaptureInfo, rCaptureInfo, MTB2R_CAPTURE_INFO_T);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_GetPicInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_PIC_INFO_T* prPicInfo;
    MTB2R_PIC_INFO_T rPicInfo;
    MT_RESULT_T eRetVal = MTR_OK;

    x_memset(&rPicInfo,0,sizeof(MTB2R_PIC_INFO_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    prPicInfo = (MTB2R_PIC_INFO_T*)(rArg.ai4Arg[1]);

    if (VDP_GetPicInfo(ucVdpId,(VDP_PIC_INFO_T*)&rPicInfo) != VDP_SET_OK)
    {
        eRetVal = MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prPicInfo,MTB2R_PIC_INFO_T);
    COPY_TO_USER_ARG(prPicInfo, rPicInfo, MTB2R_POS_INFO_T);

    return eRetVal;
}

static MT_RESULT_T _MTB2R_GetPlayMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT8* pu1PlayMode;
    UINT8 u1PlayMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    pu1PlayMode = (UINT8*)(rArg.ai4Arg[1]);

    u1PlayMode = VDP_GetPlayMode(ucVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1PlayMode,UINT8);
    COPY_TO_USER_ARG(pu1PlayMode, u1PlayMode, UINT8);

    return MTR_OK;
}
static MT_RESULT_T _MTB2R_SetGameMode(unsigned long arg)
{
#ifdef CC_B2R_SUPPORT_GAME_MODE

    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    B2R_GAME_MODE_TYPE_T pu1PlayMode;
  
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    pu1PlayMode = (B2R_GAME_MODE_TYPE_T)(rArg.ai4Arg[1]);


    VdpSetGameModeOnOff(ucVdpId,pu1PlayMode);
    
 #endif  

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_GetSlowForward(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT8* pu1SlowForward;
    UINT8 u1SlowForward;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    pu1SlowForward = (UINT8*)(rArg.ai4Arg[1]);

    u1SlowForward = VDP_GetSlowForward(ucVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1SlowForward,UINT8);
    COPY_TO_USER_ARG(pu1SlowForward, u1SlowForward, UINT8);

    return MTR_OK;
}
#ifdef CC_B2R_RM_SUPPORT
static void _MTVB2R_RmNotify(UCHAR ucConflicVdpId, MTB2R_RM_COND_T eCond,UCHAR ucCurrentVdpId)
{
    MTB2R_RM_CB_T rPara;

  //  MTVDEC_PRINT("_MTPVR_RmNotify ucEsId(%d) ucCond(%d).\n", ucEsId, eCond);
    
    rPara.u1ConflictVdpId  = ucConflicVdpId;
    rPara.u1CurrentVdpId   = ucCurrentVdpId;
    rPara.eCond   = eCond;

    _CB_PutEvent(CB_MTAL_B2R_RM_NFY, sizeof(MTB2R_RM_CB_T), (void*)&rPara);
}
#endif
static MT_RESULT_T _MTVB2R_Reg_Callback(void)
{
    #ifdef CC_B2R_RM_SUPPORT
    B2R_RegRmCb((PFN_B2R_RM_CB)_MTVB2R_RmNotify);
    #endif
    return MTR_OK;
}
static MT_RESULT_T _MTB2R_RmSet(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    BOOL fgSuccess = 0;
    UCHAR ucvdpId;
   
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucvdpId=(UINT8)(rArg.ai4Arg[0]);
    fgSuccess=(BOOL)rArg.ai4Arg[1];
    #ifdef CC_B2R_RM_SUPPORT
    B2R_RmSetKickDone(ucvdpId,fgSuccess);
    #endif
    return MTR_OK;
}
//#endif
static MT_RESULT_T _MTB2R_SetABInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_AB_INFO_T* prABInfo;
    MTB2R_AB_INFO_T rABInfo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    prABInfo = (MTB2R_AB_INFO_T*)(rArg.ai4Arg[1]);

    USR_SPACE_ACCESS_VALIDATE_ARG(prABInfo, MTB2R_AB_INFO_T);
    COPY_FROM_USER_ARG(prABInfo, rABInfo, MTB2R_AB_INFO_T);

    if(VDP_SetABInfo(ucVdpId,(VDP_AB_INTO_T*)&rABInfo))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

static MT_RESULT_T _MTB2R_SetThumbnailMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgThumbnailMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgThumbnailMode = (BOOL)rArg.ai4Arg[1];

    if(VDP_SetThumbnailMode(ucVdpId,fgThumbnailMode))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }

}

static MT_RESULT_T _MTB2R_SetSeek(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgSet;
    UINT32 u4Pts;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgSet = (BOOL)rArg.ai4Arg[1];
    u4Pts = (UINT32)rArg.ai4Arg[2];

    VDP_SetSeek(ucVdpId,fgSet,u4Pts);

    return MTR_OK;
}
static MT_RESULT_T _MTB2R_SetSeekMode(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgSet;
    UINT32 u4Pts;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgSet = (BOOL)rArg.ai4Arg[1];
    u4Pts = (UINT32)rArg.ai4Arg[2];
  #if defined(CC_MT5890)
    VDP_SetSeekMode(ucVdpId,fgSet,u4Pts);
   #endif
    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetPauseMM(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgPause;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgPause = (BOOL)rArg.ai4Arg[1];

    VDP_SetPauseMm(ucVdpId,fgPause);

    return MTR_OK;
}
#if defined(CC_MAPLE_CUST_DRV)

static MT_RESULT_T _MTB2R_SetSyncMode2(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    UINT8 ucStcId,ucAdecId,ucAdec2Id,ucVdecId;
    UINT8 u4SyncMode;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    ucStcId    =  (UINT8)rArg.ai4Arg[0];
    ucAdecId   = (UINT8)rArg.ai4Arg[1];
    ucAdec2Id  = (UINT8)rArg.ai4Arg[2];
	ucVdecId   =  (UINT8)rArg.ai4Arg[3];
	u4SyncMode =(UINT8)rArg.ai4Arg[4];
    STC_SetSyncMode2( ucStcId, ucAdecId,  ucAdec2Id, ucVdecId,  u4SyncMode);

    return MTR_OK;
}
#endif
static MT_RESULT_T _MTB2R_SetStepForward(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT8 u1StepForward;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    u1StepForward = (UINT8)rArg.ai4Arg[1];

    VDP_SetStepForward(ucVdpId,u1StepForward);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetFBNoTimeout(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgFBNoTimeout;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgFBNoTimeout = (BOOL)rArg.ai4Arg[1];

    VDP_SetFBNoTimeout(ucVdpId,fgFBNoTimeout);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetSpeed(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTSTC_SPEED_TYPE_T eSpeed;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    eSpeed = (MTSTC_SPEED_TYPE_T)rArg.ai4Arg[1];

    VDP_SetSpeed(ucVdpId,(STC_SPEED_TYPE_T)eSpeed);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetTrickMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgTrickMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgTrickMode = (BOOL)rArg.ai4Arg[1];

    VDP_SetTrickMode(ucVdpId,fgTrickMode);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetInput(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 ucVdpId;
    UINT8 ucEsId;
    UINT8 ucPort;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    ucEsId = (BOOL)rArg.ai4Arg[1];
    ucPort = (UINT32)rArg.ai4Arg[2];

    VDP_SetInput(ucVdpId,ucEsId,ucPort);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetSlowForward(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    BOOL fgSlowMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    fgSlowMode = (BOOL)rArg.ai4Arg[1];

    VDP_SetSlowForward(ucVdpId,fgSlowMode);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetCbAttachedId(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    UINT8 ucAttachedId;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    ucAttachedId = (BOOL)rArg.ai4Arg[1];

    VDP_SetCbAttachedSrcId(ucVdpId,ucAttachedId);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SelectGroupType(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    UINT8 u1FbgId;
    UINT8* pu1FbgId;
    UINT8 u1EsId;
    UINT32 u4FrameWidth;
    UINT32 u4FrameHeight;
    MTB2R_DISPLAY_FORMAT_T eFrameFormat;
    //UINT8 u1FbgType;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    pu1FbgId = (UINT8*)rArg.ai4Arg[0];
    u1EsId = (UINT8)rArg.ai4Arg[1];
    u4FrameWidth = (UINT32)rArg.ai4Arg[2];
    u4FrameHeight = (UINT32)rArg.ai4Arg[3];
    eFrameFormat = (MTB2R_DISPLAY_FORMAT_T)rArg.ai4Arg[4];

    if(u1EsId >= VDP_NS)
    {
        printk("warning ! , u1EsId = %d\n", u1EsId);
    }

    // make it same with u1EsId temporally.
    u1FbgId = u1EsId;
    if(_arMtb2rFbg[u1FbgId].ucEsId != MTB2R_INVALID_ES_ID)
    {
        printk("warning ! , _arMtb2rFbg[u1FbgId].u1EsId = %d\n", _arMtb2rFbg[u1FbgId].ucEsId);
    }
    _arMtb2rFbg[u1FbgId].ucEsId = u1EsId;
    _arMtb2rFbg[u1FbgId].ucFbgId = u1EsId; // this is virtual FbgId, set to same with EsId

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1FbgId,UINT8);
    COPY_TO_USER_ARG(pu1FbgId, u1FbgId, UINT8);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetSeqHdr(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1FbgId;
    MTB2R_SEQ_HDR_T* prFrameSeqHdr;
    MTB2R_SEQ_HDR_T  rFrameSeqHdr;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1FbgId = (UINT8)rArg.ai4Arg[0];
    prFrameSeqHdr = (MTB2R_SEQ_HDR_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prFrameSeqHdr,MTB2R_SEQ_HDR_T);
    COPY_FROM_USER_ARG(prFrameSeqHdr, rFrameSeqHdr,MTB2R_SEQ_HDR_T);
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(u1FbgId);

    if(prFbmSeqHdr != NULL)
    {
        prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
        prFbmSeqHdr->ucFrmRatCod = (UINT8)rFrameSeqHdr.eFrameRate;
        prFbmSeqHdr->fgProgressiveSeq = rFrameSeqHdr.fgProgressiveSeq;

        prFbmSeqHdr->u2LineSize = (UINT16)rFrameSeqHdr.u4LineSize;

        prFbmSeqHdr->u2HSize = (UINT16)rFrameSeqHdr.u4FrameWidth;
        prFbmSeqHdr->u2VSize = (UINT16)rFrameSeqHdr.u4FrameHeight;
        prFbmSeqHdr->u2OrgHSize = (UINT16)rFrameSeqHdr.u4FrameWidth;
        prFbmSeqHdr->u2OrgVSize = (UINT16)rFrameSeqHdr.u4FrameHeight;

        prFbmSeqHdr->u4ResizeWidth = rFrameSeqHdr.u4FrameWidth;
        prFbmSeqHdr->u4ResizeHeight = rFrameSeqHdr.u4FrameHeight;

#if B2R_USE_HW_DECODER
        prFbmSeqHdr->fgSwDecoder = FALSE;
#else
        prFbmSeqHdr->fgSwDecoder = TRUE;
#endif

        if(rFrameSeqHdr.eAddrMode == MTB2R_RASTER_MODE)
        {
            prFbmSeqHdr->fgRasterOrder = TRUE;
        }
        else
        {
            prFbmSeqHdr->fgRasterOrder = FALSE;
        }
    }
    return MTR_OK;
}

static MT_RESULT_T _MTB2R_GetEmptyFrameBuffer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1FbgId;
    //MTB2R_PIC_HDR_T* prFrameHdr;
    //MTB2R_PIC_HDR_T  rFrameHdr;
    //FBM_POOL_T *prFbmPool;

    //x_memset(&rFrameHdr,0,sizeof(MTB2R_PIC_INFO_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1FbgId = (UINT8)rArg.ai4Arg[0];
    //prFrameHdr = (MTB2R_PIC_HDR_T*)rArg.ai4Arg[1];

    UNUSED(u1FbgId);
    return MTR_OK;
}


static MT_RESULT_T _MTB2R_FreeFrameBuffer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1FbgId;
    UINT8 u1RealFbgId;
    B2R_PRM_T* prVdpPrm;
    UINT8 u1DispFbgId;
    UINT8 u1DispFbId;
    UINT8 u1TempVdpId;
	MTB2R_PIC_HDR_T* prFrameHdr;
    MTB2R_PIC_HDR_T  rFrameHdr;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1FbgId = (UINT8)rArg.ai4Arg[0];
    prFrameHdr = (MTB2R_PIC_HDR_T*)rArg.ai4Arg[1];
    USR_SPACE_ACCESS_VALIDATE_ARG(prFrameHdr,MTB2R_PIC_HDR_T);
    COPY_FROM_USER_ARG(prFrameHdr, rFrameHdr,MTB2R_PIC_HDR_T);
	u1RealFbgId = rFrameHdr.u1FbgId;

    u1TempVdpId = VDP_Es2Vdp(_arMtb2rFbg[u1FbgId].ucEsId);
    prVdpPrm = (B2R_PRM_T*)VDP_GetVdpPrm(Get_VDP_2_B2R_ID(u1TempVdpId));

    if(!prVdpPrm)
    {
        printk("%s(L:%d) prVdpPrm null\n", __FUNCTION__, __LINE__);
        return MTR_OK;
    }
#ifdef CC_3D_MM_DS_SUPPORT
	if(prVdpPrm->ucFbgId != u1RealFbgId && prVdpPrm->ucSubFbgId != u1RealFbgId)
	{
		printk("warning! Free Fbgid = %d , driver Fbgid = (%d, %d)\n", 
			u1RealFbgId, prVdpPrm->ucFbgId, prVdpPrm->ucSubFbgId);
		return MTR_OK;
	}
#endif
    u1DispFbgId = u1RealFbgId; //prVdpPrm->rPortPrm.ucFbgId;

    u1DispFbId = FBM_GetFrameBufferFromDispQ(u1DispFbgId);

    if(u1DispFbId != 0xff && u1DispFbgId != 0xff)
    {
    	FBM_PIC_HDR_T* prPicHdr;
		prPicHdr  = FBM_GetFrameBufferPicHdr(u1DispFbgId, u1DispFbId);
        FBM_SetFrameBufferStatus(u1DispFbgId, u1DispFbId, FBM_FB_STATUS_LOCK);
        FBM_SetFrameBufferStatus(u1DispFbgId, u1DispFbId, FBM_FB_STATUS_EMPTY);
    }

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetDispFrame(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1VdpId;
    UINT8 u1FbgId;
    B2R_PRM_T* prVdpPrm;
    MTB2R_PIC_HDR_T* prFrameHdr;
    MTB2R_PIC_HDR_T  rFrameHdr;
    //send a message to b2r to get next frame buffer to display
    VDP_B2R_CHG_FRAME_MSG_T rMsg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1FbgId = (UINT8)rArg.ai4Arg[0];
    prFrameHdr = (MTB2R_PIC_HDR_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prFrameHdr,MTB2R_PIC_HDR_T);
    COPY_FROM_USER_ARG(prFrameHdr, rFrameHdr,MTB2R_PIC_HDR_T);

    x_memset(&rMsg, 0, sizeof(VDP_B2R_CHG_FRAME_MSG_T));

    u1VdpId = (VDP_Es2Vdp(_arMtb2rFbg[u1FbgId].ucEsId));
    prVdpPrm = (B2R_PRM_T*)VDP_GetVdpPrm(Get_VDP_2_B2R_ID(u1VdpId));

    if(!prVdpPrm)
    {
        printk("%s(L:%d) prVdpPrm null\n", __FUNCTION__, __LINE__);
        return MTR_OK;
    }

    if(prVdpPrm != NULL)
    {
        if(prVdpPrm->ucFbgId == 0xFF || prVdpPrm->ucFbgId != rFrameHdr.u1FbgId)
        {
            printk("warning! render Fbgid = %d , driver Fbgid = %d\n",rFrameHdr.u1FbgId,prVdpPrm->ucFbgId);
            return MTR_OK;
        }
    }

    if(VDP_GstEnablGstLog(u1VdpId))
    {
        printk("render fbg = %d , fbid = %d , flag = 0x%x\n",rFrameHdr.u1FbgId,rFrameHdr.u1FbId,rFrameHdr.u4Flag);
    }
    
    rMsg.u4VdpId = (UINT32)u1VdpId;
    rMsg.u4Flag = (UINT32)rFrameHdr.u4Flag;
    VDP_SendB2RChgFrameMsg(&rMsg);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_FlushFrame(unsigned long arg)
{
    UINT8 u1VdpId;
    UINT8 u1FbgId;

    u1FbgId = (UINT8)arg;
    u1VdpId = (VDP_Es2Vdp(_arMtb2rFbg[u1FbgId].ucEsId));

    if(VDP_GstEnablGstLog(u1VdpId))
    {
        printk("flush render vdpid = %d\n", u1VdpId);
    }

    VDP_FlushB2RChgFrameMsg(u1VdpId);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_ReleaseFbg(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1FbgId;
    UINT8 u1EsId;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1FbgId = (UINT8)rArg.ai4Arg[0];
    u1EsId = (UINT8)rArg.ai4Arg[1];

    u1EsId = _arMtb2rFbg[u1FbgId].ucEsId;

    _arMtb2rFbg[u1FbgId].ucEsId = MTB2R_INVALID_ES_ID;
    _arMtb2rFbg[u1FbgId].ucFbgId= MTB2R_INVALID_FBG_ID;

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_Set3DInfo(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UCHAR u1B2rId;
    UINT8 u13DType;
    BOOL fg3DEnable;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u1B2rId = (UCHAR)rArg.ai4Arg[0];
    u13DType = (UINT8)rArg.ai4Arg[1];
    fg3DEnable = (BOOL)rArg.ai4Arg[2];

    VDP_Set3DInfo(u1B2rId, u13DType, fg3DEnable);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_SetChpterEnable(unsigned long arg)
{
#if !defined(DIVX_PLUS_CER) || !defined(CC_B2R_CHAPTER_CHG) 
    return MTR_NOT_OK;

#else
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_CHAPTER_T *prChapterUsr;
    MTB2R_CHAPTER_T rChapterKer;
    VDP_CHAPTER_T    rChapter;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)rArg.ai4Arg[0];
    prChapterUsr = (MTB2R_CHAPTER_T *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prChapterUsr, MTB2R_CHAPTER_T);
    COPY_FROM_USER_ARG(prChapterUsr, rChapterKer, MTB2R_CHAPTER_T);

    x_memset(&rChapter, 0, sizeof(rChapter));

    rChapter.fgIsPending     = rChapterKer.fgIsPending;
    rChapter.u8StartPts      = rChapterKer.u8StartPts;
    rChapter.u8EndPts        = rChapterKer.u8EndPts;
    rChapter.fgFirstSetChapter = rChapterKer.fgFirstSetChapter;

    return (VDP_SetChpterEnable(&rChapter))? MTR_OK : MTR_NOT_OK;
#endif
}

static MT_RESULT_T _MTB2R_SetMirror(unsigned long arg)
{
	MTAL_IOCTL_2ARG_T rArg;
    UCHAR ucVdpId;
    BOOL fgOn;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UCHAR)rArg.ai4Arg[0];
    fgOn = (BOOL)rArg.ai4Arg[1];

    VDP_SetB2RMirror(ucVdpId, fgOn);

    return MTR_OK;
}

static MT_RESULT_T _MTB2R_AudTrigger(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 ucVdpId;
    MTB2R_TRIG_AUD_CFG_T* prAudTrgInfo;
    MTB2R_TRIG_AUD_CFG_T  rAudTrgInfo;
    MT_RESULT_T eRetVal = MTR_OK;

    x_memset(&rAudTrgInfo,0,sizeof(MTB2R_TRIG_AUD_CFG_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucVdpId = (UINT8)(rArg.ai4Arg[0]);
    prAudTrgInfo = (MTB2R_TRIG_AUD_CFG_T*)(rArg.ai4Arg[1]);

    USR_SPACE_ACCESS_VALIDATE_ARG(prAudTrgInfo, MTB2R_TRIG_AUD_CFG_T);
    COPY_FROM_USER_ARG(prAudTrgInfo, rAudTrgInfo, MTB2R_TRIG_AUD_CFG_T);
    
    if (B2R_AudTrigger(ucVdpId,(VDP_TRIG_AUD_CFG_T*)&rAudTrgInfo) != B2R_OK)
    {
        eRetVal = MTR_NOT_OK;
    }

    return eRetVal;
}

static B2R_CB_FUNC_ENUM_T _MTB2R_CBEnum_to_VDPCBEnum(MTB2R_CB_FUNC_ENUM_T eMTB2REnum)
{
    B2R_CB_FUNC_ENUM_T eReturn = B2R_CB_FUNC_NS;

    switch (eMTB2REnum)
    {
        case MTB2R_CB_FUNC_TRIG_AUD_TO_PLAY:
            eReturn = B2R_CB_FUNC_TRIG_AUD_TO_PLAY;
            break;            

        case MTB2R_CB_FUNC_EOS:
            eReturn = B2R_CB_FUNC_EOS;
            break;
            
        default:
            eReturn = B2R_CB_FUNC_NS;
            break;
    }

    return eReturn;
}

static void _B2R_TrigAudCallback(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3)
{
    MTB2R_CB_T  rB2rCbT;

    rB2rCbT.u4Arg1 = u4VdpId;
    rB2rCbT.u4Arg2 = u4Arg1;
    rB2rCbT.u4Arg3 = u4Arg2;
    rB2rCbT.u4Arg4 = u4Arg3;
    
    _CB_PutEvent(CB_MTAL_B2R_AUD_START_TO_PLAY, sizeof(MTB2R_CB_T), (void*)&rB2rCbT);
}

static void _B2R_EosCallback(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3)
{
    MTB2R_CB_T  rB2rCbT;

    rB2rCbT.u4Arg1 = u4VdpId;
    rB2rCbT.u4Arg2 = u4Arg1;
    rB2rCbT.u4Arg3 = u4Arg2;
    rB2rCbT.u4Arg4 = u4Arg3;
    
    _CB_PutEvent(CB_MTAL_B2R_EOS, sizeof(MTB2R_CB_T), (void*)&rB2rCbT);
}

static MT_RESULT_T _MTB2R_RegCbFunc(unsigned long arg)
{
    INT32  i4Ret = MTR_OK;
    MTAL_IOCTL_2ARG_T rArg;
    MTB2R_CB_FUNC_ENUM_T eFuncType;
    MTB2R_REG_CB_OBJ_T  r_reg_obj;
    MTB2R_REG_CB_OBJ_T* pr_reg_obj;

    x_memset(&r_reg_obj, 0, sizeof(MTB2R_REG_CB_OBJ_T));

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eFuncType = (MTB2R_CB_FUNC_ENUM_T)rArg.ai4Arg[0];
    pr_reg_obj = (MTB2R_REG_CB_OBJ_T*)rArg.ai4Arg[1];

    MTB2R_PRINT(" - eFuncType = %d\n",
        eFuncType);

    USR_SPACE_ACCESS_VALIDATE_ARG(pr_reg_obj, MTB2R_REG_CB_OBJ_T);
    COPY_FROM_USER_ARG(pr_reg_obj, r_reg_obj, MTB2R_REG_CB_OBJ_T);

    switch (eFuncType)
    {
        case MTB2R_CB_FUNC_TRIG_AUD_TO_PLAY:
            i4Ret = B2R_RegCbFunc(_MTB2R_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_B2R_TrigAudCallback, (UINT32)&r_reg_obj);
            break;                        

        case MTB2R_CB_FUNC_EOS:
            i4Ret = B2R_RegCbFunc(_MTB2R_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_B2R_EosCallback, (UINT32)&r_reg_obj);
            break; 
            
        default:
            i4Ret = MTR_NOT_OK;
            break;
    }
    
    return i4Ret;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



int mtal_ioctl_mtb2r(struct inode *inode, struct file *file, unsigned int cmd,
                     unsigned long arg)
{
    int i4Ret = 0;

    switch (cmd)
    {
    case MTAL_IO_B2R_GET_PTS:
        return _MTB2R_GetPts(arg);
    case MTAL_IO_B2R_GET_STEP_FORWARD:
        return _MTB2R_GetStepForward(arg);
    case MTAL_IO_B2R_GET_POS_INFO:
        return _MTB2R_GetPosInfo(arg);
    case MTAL_IO_B2R_GET_PIC_INFO:
        return _MTB2R_GetPicInfo(arg);
    case MTAL_IO_B2R_GET_PLAY_MODE:
        return _MTB2R_GetPlayMode(arg);
    case MTAL_IO_B2R_SET_GameMode:
         return _MTB2R_SetGameMode(arg);
    case MTAL_IO_B2R_GET_SLOW_FORWARD:
        return _MTB2R_GetSlowForward(arg);
    case MTAL_IO_B2R_SET_AB_INFO:
        return _MTB2R_SetABInfo(arg);
    case MTAL_IO_B2R_SET_THUMBNAIL_MODE:
        return _MTB2R_SetThumbnailMode(arg);
    case MTAL_IO_B2R_SET_SEEK:
        return _MTB2R_SetSeek(arg);
    case MTAL_IO_B2R_SET_SEEK_MODE:
         return _MTB2R_SetSeekMode(arg);
  #if defined(CC_MAPLE_CUST_DRV)
     case MTAL_IO_B2R_SET_SYC_MODE:
        return _MTB2R_SetSyncMode2(arg);
  #endif
    case MTAL_IO_B2R_SET_PAUSE_MM:
        return _MTB2R_SetPauseMM(arg);
    case MTAL_IO_B2R_SET_STEP_FORWARD:
        return _MTB2R_SetStepForward(arg);
    case MTAL_IO_B2R_SET_FB_NO_TIMEOUT:
        return _MTB2R_SetFBNoTimeout(arg);
    case MTAL_IO_B2R_SET_SPEED:
        return _MTB2R_SetSpeed(arg);
    case MTAL_IO_B2R_SET_TRICK_MODE:
        return _MTB2R_SetTrickMode(arg);
    case MTAL_IO_B2R_SET_INPUT:
        return _MTB2R_SetInput(arg);
    case MTAL_IO_B2R_SET_SLOW_FORWARD:
        return _MTB2R_SetSlowForward(arg);
    case MTAL_IO_B2R_GET_CAPTURE_INFO:
        return _MTB2R_GetCaptureInfo(arg);
    case MTAL_IO_B2R_GET_CAPTURE_INFO2:
        return _MTB2R_GetCaptureInfo2(arg);
    case MTAL_IO_B2R_SET_CB_ATTACH_ID:
        return _MTB2R_SetCbAttachedId(arg);
    case MTAL_IO_B2R_SELECT_GROUP_TYPE:
        return _MTB2R_SelectGroupType(arg);
    case MTAL_IO_B2R_GET_EMPTY_FRAME_BUFFER:
        return _MTB2R_GetEmptyFrameBuffer(arg);
    case MTAL_IO_B2R_SET_SEQ_HDR:
        return _MTB2R_SetSeqHdr(arg);
    case MTAL_IO_B2R_SET_DISP_FRAME:
        return _MTB2R_SetDispFrame(arg);
    case MTAL_IO_B2R_FLUSH_FRAME:
        return _MTB2R_FlushFrame(arg);
    case MTAL_IO_B2R_RELEASE_FBG:
        return _MTB2R_ReleaseFbg(arg);
    case MTAL_IO_B2R_FREE_FRAME_BUFFER:
        return _MTB2R_FreeFrameBuffer(arg);
    case MTAL_IO_B2R_GET_DISP_STABLE:
        return _MTB2R_GetDispDrvStable(arg);
    case MTAL_IO_B2R_SET_3D_INFO:
        return _MTB2R_Set3DInfo(arg);
    case MTAL_IO_B2R_SET_CHPTER_ENABLE:
        return _MTB2R_SetChpterEnable(arg);
	case MTAL_IO_B2R_SET_MIRROR:
        return _MTB2R_SetMirror(arg);
 //   #ifdef CC_B2R_RM_SUPPORT
    case MTAL_IO_B2R_RM_CALLBACK:
        return _MTVB2R_Reg_Callback();
    case MTAL_IO_B2R_RM_CONFLICT_DONE:
        return  _MTB2R_RmSet(arg);
  //  #endif
    case MTAL_IO_B2R_SET_AUD_TRIGGER:
        return _MTB2R_AudTrigger(arg);
    case MTAL_IO_B2R_REG_CB:
        return _MTB2R_RegCbFunc(arg);
    default:
        break;
    }
    return i4Ret;
}
