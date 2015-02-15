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
 * $RCSfile: io_MTIMAGE.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/
/** @file io_MTIMAGE.c
 */

/********************************************************************
  INCLUDE FILES
********************************************************************/
#include "mtimage.h"
#include "img_lib_if.h"
#include "fbm_drvif.h"
#include "cb_data.h"
#include "jpg_if.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"
//#include "osd_if.h"
#include "osd_drvif.h"
#include "gfx_drvif.h"
#include "srm_drvif.h"
#include "drv_tdtv_drvif.h"
#include <linux/mtal_ioctl.h>
#include "x_img_slideshow.h"
#include "vdp_if.h"
#include "drv_tdtv_drvif.h"
#include "drv_common.h"
#include "x_img_dec.h"
#include "x_rm.h"
#include "b2r_drvif.h"


#include "fbm_drvif.h"
/********************************************************************
  MACRO CONSTANT DEFINITIONS
********************************************************************/
#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
#define MPF_HALF_W 1920
#define MPF_HALF_H 540

#define MAX_VIEW_NUM 1
#define ALIGN16(X) (((X) + 0xF) & (~0xF))    ///< for imgrz pitch alignment

#define MAX_SLIDE_SHOW_PIC_PERIOD 1500 // ms
#define MAX_SLIDE_SHOW_FRM_PERIOD 800 // ms
#define MAX_SLIDE_SHOW_FRM_NUM 16

//#define MTIMAGE_PERFORMANCE_TEST

/********************************************************************
  TYPE DEFINITION
********************************************************************/
typedef struct _MTIMAGE_INFO_T
{
    BOOL fgConnect;
    BOOL fgIsPushMode;
    BOOL fgIsVdoMode;
    BOOL fgIsMPF;
    UINT8 u1PicIdx;
    //UINT8 u1CurView;
    UINT8 u1SlideShowType;
    UINT8 u1SlideShowEvent;
    HANDLE_T hSlideShowSema;
    HANDLE_T hHwResouceSema;    
    ENUM_IMG_FORMAT imgFormat;
    ENUM_IMG_ROTATE_PHASE eRotateDegree;
    ENUM_IMG_DISP_COLOR_MODE eImgDispCM;
    MTIMAGE_IMGCOLORFORMAT_T eImgColorFmt;
    IMG_REGION_T rDecOut;
    IMG_REGION_T rDecOut2;
    IMG_CALLBACK_INFO_T rCallbackInfo;
    //UCHAR aucFbgId[MAX_VIEW_NUM];
    //UCHAR aucCurImgFbId[MAX_VIEW_NUM];
    //UCHAR aucPrevImgFbId[MAX_VIEW_NUM];
    //UCHAR aucCurDispFbId[MAX_VIEW_NUM];
    E_TDTV_DECODER_INPUT eDetectedMode;
    UINT32 u4PlaneId;
    UINT32 u4DisplayRegionId;

    BOOL fgLibjpeg;
    BOOL fgDrvFeedData;
    UINT32 u4TgtUsrAddr;
#ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT
    BOOL fgIsMJPEGVdoMode;
#endif
    UINT32 u4FbmPNGSize;
} MTIMAGE_INFO_T;
typedef struct _MTIMAGE_FBG_T
{
    UINT8 u1CurView;
    UCHAR aucFbgId[MAX_VIEW_NUM];
    UCHAR aucCurImgFbId[MAX_VIEW_NUM];
    UCHAR aucPrevImgFbId[MAX_VIEW_NUM];
    UCHAR aucCurDispFbId[MAX_VIEW_NUM];
    BOOL fgFBGCreated;    
}MTIMAGE_FBG_T;


/********************************************************************
  VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/
static MTIMAGE_INFO_T _rImageInfo[2] = {{0}, {0}};
static MTIMAGE_SET_NFY_PARM _rMTImage_set_nfy_parm;

static HANDLE_T hMtinageDbgInsertSema = 0;
UINT32 _mtimageDebugStep = 1;

UINT32 u4MtimageDBGCnt = 0;
UINT32 u4MtimageDBG[256][2]={{0}, {0}};

void u4MtimageDBGInsert(UINT32 type, UINT32 value)
{   
    if(hMtinageDbgInsertSema==0)
    {
        MTIMAGE_KERNEL_DEBUG("MTIMAGE", "\n_MTIMAGE create hMtinageDbgInsertSema............\n");   
        VERIFY(x_sema_create(&hMtinageDbgInsertSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);       
    }

    if(hMtinageDbgInsertSema!=0)
    {
        VERIFY (x_sema_lock(hMtinageDbgInsertSema, X_SEMA_OPTION_WAIT) == OSR_OK);
    }       
    
    u4MtimageDBG[u4MtimageDBGCnt][0] = type;
    u4MtimageDBG[u4MtimageDBGCnt][1] = value;  
    u4MtimageDBGCnt++;
    if(u4MtimageDBGCnt>=256)
    {
        //x_memset(u4MtimageDBG,0x0, sizeof(u4MtimageDBG));     
        u4MtimageDBGCnt =0;
    }   

    if(hMtinageDbgInsertSema!=0)
    {
        VERIFY (x_sema_unlock(hMtinageDbgInsertSema) == OSR_OK);    
    }       
}
static MTIMAGE_FBG_T _rImageFbg ={0};

MTIMAGE_INFO_T * _MTIMAGE_GetImgIdInfo(UCHAR ucImgId)
{
    return &(_rImageInfo[ucImgId]);
}

static BOOL _ucImgId_3D = 0;
static MTVDO_REGION_T _rMt3DCropinfo[2] = {{0},{0}};


#ifdef MTIMAGE_PERFORMANCE_TEST

static HAL_TIME_T _rMTImageTimeStart;
static HAL_TIME_T _rMTImageTimeEnd;
static HAL_TIME_T _rMTImageTimeDiff;

#define _MtImageMeasureTimeBegin()                             \
{                                                       \
    HAL_GetTime(&_rMTImageTimeStart);                   \
}
#define _MtImageMeasureTimeEnd(arg)                                                \
{                                                                           \
    HAL_GetTime(&_rMTImageTimeEnd);                                     \
    HAL_GetDeltaTime(&_rMTImageTimeDiff, &_rMTImageTimeStart, &_rMTImageTimeEnd);       \
    MTIMAGE_KERNEL_TIME_DEBUG("%s time %u.%06u \n", arg, _rMTImageTimeDiff.u4Seconds,_rMTImageTimeDiff.u4Micros);   \
}

#endif

#if defined(CC_MT5368)||defined(CC_MT5396) ||defined(CC_MT5389)||defined(CC_MT5398)||defined(CC_MT5880)||defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#define IO_MTIMAGE_IS_POST_MT5368
#endif


#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
   {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))                            \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage 3 arguments error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))                        \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage 3 arguments error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_10ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_10ARG_T)))              \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_FROM_USER_10ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_10ARG_T)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

    
#define USR_SPACE_ACCESS_VALIDATE_12ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(MTAL_IOCTL_12ARG_T)))              \
        {                                                       \
            MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
            return MTR_ERR_INV;                                          \
        }                                                       \
     
#define COPY_FROM_USER_12ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(MTAL_IOCTL_12ARG_T)))          \
        {                                                       \
            MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");       \
            return MTR_ERR_INV;                                          \
        }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_WRITE, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
 
#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);


//#define _IOMTIMAGE_JPG_MID_PROTECTION

#ifdef _IOMTIMAGE_JPG_MID_PROTECTION
#include "x_mid.h"
#include "x_hal_5381.h"
static BOOL _MTIMAGE_Set_MID_Protection(UINT32 addr, UINT32 size, UINT32 region)
{
    MID_AGENT_ID_T ePPAgentId;
    FBM_POOL_T* prFbmPool=NULL;
    FBM_POOL_T* prFbmPool2=NULL;
    BOOL fgRet = TRUE;

    //mid setting.
    ePPAgentId = MID_AGENT_JPEG;

    //protect malloc pysical buffer

    MID_ResetRegion(region);    
    fgRet &= MID_SetRegionEx(region, ePPAgentId,
        (UINT32)addr,
        (UINT32)addr+ (UINT32)size,
        MID_FLAG_WRITEABLE);

    fgRet &= MID_EnableRegionProtect(region);

    return fgRet;


}

static BOOL _MTIMAGE_Disable_MID_Protection(UINT32 region)
{
    BOOL fgRet = TRUE;

    fgRet = MID_EnableRegionProtect(region);
    
    return fgRet;
}
#endif

static BOOL _MTIMAGE_3DCropFuncCB(TDTV_3D_CROP_INTO_T* prCropInfo)
{
    prCropInfo->fgIsFixSrcAspectRatio = 1;
    if (_rImageInfo[0].fgIsMPF)
    {
        prCropInfo->fgIsFullResolution = 0;    //vertical will be 1/2, and scaler will enlarge it to 2 multiple ratio.
    }
    else
    {
        prCropInfo->fgIsFullResolution = 1;
    }
    prCropInfo->u4CropBottomOffset = _rMt3DCropinfo[_ucImgId_3D].u4Y;
    prCropInfo->u4CropTopOffset = _rMt3DCropinfo[_ucImgId_3D].u4Y;
    prCropInfo->u4CropLeftOffset = _rMt3DCropinfo[_ucImgId_3D].u4X;
    prCropInfo->u4CropRightOffset = _rMt3DCropinfo[_ucImgId_3D].u4X;

    MTIMAGE_KERNEL_DEBUG("#@# _MTIMAGE_3DCropFuncCB [%d][%d][%d][%d]/n", _rMt3DCropinfo[_ucImgId_3D].u4X, _rMt3DCropinfo[_ucImgId_3D].u4Y, _rMt3DCropinfo[_ucImgId_3D].u4Width, _rMt3DCropinfo[_ucImgId_3D].u4Height);

    return TRUE;
}


/*
Assign FBG ID, and set SRM mode.
*/
static BOOL _FBGHandler(MTIMAGE_INFO_T* prImageInfo, BOOL fgConnect)
{
    UINT32 u4ViewIdx;
    UCHAR ucFbgType;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_CREATE_FBG_PAR_T rPar;
    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
    rPar.u1VdecId = (UINT8)0;
    rPar.u1AppMode = FBM_FBG_APP_MTIMAGE;


    //prepare VDP frame buffer, create FBG
    _rImageFbg.u1CurView = 0;
    for (u4ViewIdx = 0; u4ViewIdx < MAX_VIEW_NUM; u4ViewIdx++)
    {
        if (fgConnect)
        {
            //reset
            MTIMAGE_KERNEL_DEBUG("$$$ TRUE _FBGHandler fgConnect =%d, rImageInfo->fgConnect=%d, format=%d\n", fgConnect, prImageInfo->fgConnect, prImageInfo->imgFormat);        

            if(_rImageFbg.aucFbgId[u4ViewIdx] != 0xFF)
            {                
                FBM_ReleaseGroup(_rImageFbg.aucFbgId[u4ViewIdx]);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
            }
            
            _rImageFbg.aucFbgId[u4ViewIdx] = 0xFF;
            _rImageFbg.aucCurImgFbId[u4ViewIdx] = 0xFF;
            _rImageFbg.aucPrevImgFbId[u4ViewIdx] = 0xFF;
            _rImageFbg.aucCurDispFbId[u4ViewIdx] = 0xFF;

            if (prImageInfo->fgConnect)
            {
                FBM_ReleaseGroup(_rImageFbg.aucFbgId[u4ViewIdx]);
            }
            //todo
            //SRM_SetMmMode(SRM_MM_MODE_TYPE_JPEG);

            ucFbgType = FBM_SelectGroupType(MAX_WIDTH, MAX_HEIGHT);
            FBM_SetColorMode(FBM_CM_422);
            //prImageInfo->aucFbgId[u4ViewIdx] = FBM_CreateGroup(ucFbgType, FBM_VDEC_JPEG, MAX_WIDTH, MAX_HEIGHT);
            _rImageFbg.aucFbgId[u4ViewIdx] = FBM_CreateGroupExt(ucFbgType, FBM_VDEC_JPEG, MAX_WIDTH, MAX_HEIGHT, &rPar);
            if (_rImageFbg.aucFbgId[u4ViewIdx] == FBM_FBG_ID_UNKNOWN)
            {
                MTIMAGE_KERNEL_DEBUG("Create View %d Fbg Fail!!\n", u4ViewIdx);
                //SRM_SetMmMode(SRM_MM_MODE_TYPE_OFF);
                return FALSE;
            }
            FBM_SetPlayMode(_rImageFbg.aucFbgId[u4ViewIdx], FBM_FBG_MM_MODE);
            FBM_SetSyncStc(_rImageFbg.aucFbgId[u4ViewIdx], 0, 0); //none
            FBM_FbgChgNotify(_rImageFbg.aucFbgId[u4ViewIdx], 0);

            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(_rImageFbg.aucFbgId[u4ViewIdx]);
            if (prFbmSeqHdr == NULL)
            {
                ASSERT(prFbmSeqHdr != NULL);
                return FALSE;
            }
            prFbmSeqHdr->u2HSize = MAX_WIDTH;
            prFbmSeqHdr->u2VSize = MAX_HEIGHT;
            prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
            prFbmSeqHdr->u2LineSize = MAX_WIDTH;
            prFbmSeqHdr->fgProgressiveSeq = TRUE;
            prFbmSeqHdr->fgRasterOrder = TRUE;
            prFbmSeqHdr->fg422Mode = TRUE;
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;

            FBM_SetFrameBufferFlag(_rImageFbg.aucFbgId[u4ViewIdx], FBM_FLAG_SEQ_CHG);

            VDP_SetEnable(0, 1);
            VDP_B2rSwitch(0, B2R_1);
            _rImageFbg.fgFBGCreated = TRUE;
        }
        else
        {
            MTIMAGE_KERNEL_DEBUG("$$$ FALSE _FBGHandler fgConnect =%d, rImageInfo->fgConnect=%d, format=%d\n", fgConnect, prImageInfo->fgConnect, prImageInfo->imgFormat);                
            //if ever connected and fbgid isn't 0xff, then release FBG
            //if ((prImageInfo->fgConnect)&&(_rImageFbg.aucFbgId[u4ViewIdx] != 0xFF))
            VDP_SetEnable(0, 0);
            if ((_rImageFbg.fgFBGCreated==TRUE)&&(_rImageFbg.aucFbgId[u4ViewIdx] != 0xFF))
            {
                FBM_ReleaseGroup(_rImageFbg.aucFbgId[u4ViewIdx]);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
            }
            //reset
            _rImageFbg.aucFbgId[u4ViewIdx] = 0xFF;
            _rImageFbg.aucCurImgFbId[u4ViewIdx] = 0xFF;
            _rImageFbg.aucPrevImgFbId[u4ViewIdx] = 0xFF;
            _rImageFbg.aucCurDispFbId[u4ViewIdx] = 0xFF;

            //VDP_B2rSwitch(0, B2R_NS);
            //SRM_SetMmMode(SRM_MM_MODE_TYPE_OFF);
            _rImageFbg.fgFBGCreated = FALSE;            
        }
    }
    return TRUE;
}

/*
ucFbgId [In]
pucFbId [Out]
pu4YAddr [Out]
pu4CAddr [Out]
*/
static BOOL _AllocFrame(UCHAR ucImgId,UCHAR ucFbgId, UCHAR* pucFbId, UINT32* pu4YAddr, UINT32* pu4CAddr)
{
    FBM_PIC_HDR_T* prFbmPicHdr;
    UINT32 u4FrameSize, u4W, u4H;
    MTIMAGE_INFO_T * rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

    if ((pucFbId == NULL) || (pu4YAddr == NULL) || (pu4CAddr == NULL))
    {
        MTIMAGE_KERNEL_DEBUG("AllocFrame input parameter error\n");
        return FALSE;
    }
    //get frame buffer
    *pucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
    if (*pucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
    {
        MTIMAGE_KERNEL_DEBUG("Get empty frame buffer failed.\n");
        return FALSE;
    }
    FBM_GetFrameBufferAddr(ucFbgId, *pucFbId, pu4YAddr, pu4CAddr);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, *pucFbId);
    if ((prFbmPicHdr == NULL) || (pu4YAddr == NULL) || (pu4YAddr == NULL))
    {
        MTIMAGE_KERNEL_DEBUG("Alloc frame buffer failed.\n");
        return FALSE;
    }
    prFbmPicHdr->ucPicStruct = (UCHAR)3;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    FBM_UpdateReferenceList(ucFbgId, *pucFbId, FALSE);    //default
    FBM_SetFrameBufferStatus(ucFbgId, *pucFbId, (UCHAR)FBM_FB_STATUS_READY);

    u4FrameSize = (UINT32)(*pu4CAddr - *pu4YAddr);
    MTIMAGE_KERNEL_DEBUG("Alloc Y 0x%x, C 0x%x, Sz 0x%x\n", *pu4YAddr, *pu4CAddr, u4FrameSize);

    //clean buffer content
    FBM_GetFrameBufferSize(ucFbgId, &u4W, &u4H);

    GFX_Memset((UINT8*)*pu4YAddr, u4W, u4H, 0);
    if (rImageInfo->eImgDispCM == Y_CBCR420_SEP16)
    {
        GFX_Memset((UINT8*)*pu4CAddr, u4W>>1, u4H, 0x80);
    }
    else    // 422
    {
        GFX_Memset((UINT8*)*pu4CAddr, u4W, u4H, 0x80);
    }

    return TRUE;
}

#if 0
static IMG_SLIDE_SHOW_EFFECT_T _ConvertSlideShowType(UINT8 u1Type)
{
    //check data structure "IMG_SLIDE_SHOW_EFFECT_T" and "MTIMAGE_SLIDETYPE"
    return (IMG_SLIDE_SHOW_EFFECT_T)u1Type;
}
#endif

void _MTIMAGE_Notify(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{
    IMG_FILL_BUF_MW_T* prFillBuf;
    UCHAR ucFbgId, ucFbId;
    FBM_POOL_T *prFbmPool;

    UCHAR ucImgId = (UCHAR)u4ImgID;

    MTIMAGE_INFO_T * rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("<_MTIMAGE_Notify> fail\n");
        return ;
    }
    ucFbgId = _rImageFbg.aucFbgId[_rImageFbg.u1CurView];
    ucFbId = _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView];

    if (u4State == JPG_HW_NFY_FINISHED)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_FINISH;
        rImageInfo->rCallbackInfo.u4Param = (UINT32)pv_tag;
        if (rImageInfo->fgIsVdoMode)
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, TRUE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);
        }

        #ifdef MTIMAGE_PERFORMANCE_TEST
        _MtImageMeasureTimeEnd("JPG_HW_NFY_FINISHED");
        #endif
    }
    else if (u4State == JPG_HW_NFY_ERROR)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_ERROR;
        if ((UINT32)pv_data == IMG_DRV_NOT_SUPPORT)
        {
            rImageInfo->rCallbackInfo.u4Param = MT_IMAGE_FILE_ERROR;
        }
        else if ((UINT32)pv_data == IMG_DRV_ERR_COMPONENT)
        {
            rImageInfo->rCallbackInfo.u4Param = MT_IMAGE_JPEG_COLOR_FORMAT_ERROR;
        }
        else    //eState==IMG_DRV_HW_ERROR
        {
            rImageInfo->rCallbackInfo.u4Param = MT_IMAGE_IMG_ERROR;
        }
        if (rImageInfo->fgIsVdoMode)
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView] = 0xFF;
        }
    }
    else if (u4State == JPG_HW_NFY_FILL_BUF)
    {
        if (rImageInfo->imgFormat == IMAGE_JPEG)
        {
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
        }
        else
        {
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        }
        ASSERT(prFbmPool != (FBM_POOL_T *)NULL);
        ASSERT(prFbmPool->u4Addr != 0);
    
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_NEEDDATA;
        prFillBuf = (IMG_FILL_BUF_MW_T *)pv_data;
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_trsn_id = prFillBuf->ui4_trsn_id;
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_start_add = (UINT32)prFillBuf->pv_start_add - VIRTUAL(prFbmPool->u4Addr);
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_required_len = prFillBuf->ui4_required_len;
        rImageInfo->rCallbackInfo.rReqDataInfo.u1_reset_pos = (UCHAR)prFillBuf->b_reset_pos;
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_position = prFillBuf->ui4_position;

        #ifdef MTIMAGE_PERFORMANCE_TEST
        _MtImageMeasureTimeEnd("JPG_HW_NFY_FILL_BUF");
        #endif
    }
    else if (u4State == JPG_HW_NFY_STOP_DONE)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_STOPDONE;
        rImageInfo->rCallbackInfo.u4Param = (UINT32)pv_tag;
        if (rImageInfo->fgIsVdoMode)
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView] = 0xFF;
        }
    }
    else if (u4State == JPG_HW_NFY_ALREADY_STOPPED)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_ALREADYSTOPED;
        rImageInfo->rCallbackInfo.u4Param = (UINT32)pv_tag;
        if (rImageInfo->fgIsVdoMode)
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView] = 0xFF;
        }
    }
    else
    {
        return;
    }
    rImageInfo->rCallbackInfo.u4ImgId = u4ImgID;
    if(u4ImgID == 0)
    {
        MTIMAGE_KERNEL_DEBUG("CB_MTIMAGE_TRIGGER[%d][u4State=%d]\n", u4ImgID,u4State);  
        _CB_PutEvent(CB_MTIMAGE_TRIGGER, sizeof(IMG_CALLBACK_INFO_T), &rImageInfo->rCallbackInfo);
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("CB_MTIMAGE_TRIGGER_1[%d][u4State=%d]\n", u4ImgID,u4State);        
        _CB_PutEvent(CB_MTIMAGE_TRIGGER_1, sizeof(IMG_CALLBACK_INFO_T), &rImageInfo->rCallbackInfo);
    }
}


#ifdef MTIMAGE_ANDROID_SUPPORT
void _MTIMAGE_Notify_VDO(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{
    IMG_FILL_BUF_MW_T* prFillBuf;
    UCHAR ucFbgId, ucFbId;
    FBM_POOL_T *prFbmPool;

    UCHAR ucImgId = (UCHAR)u4ImgID;

    MTIMAGE_INFO_T * rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("<_MTIMAGE_Notify_0> fail\n");
        return ;
    }
    ucFbgId = _rImageFbg.aucFbgId[_rImageFbg.u1CurView];
    ucFbId = _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView];

    if (u4State == JPG_HW_NFY_FINISHED)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_FINISH;
        rImageInfo->rCallbackInfo.u4Param = (UINT32)pv_tag;
        if (rImageInfo->fgIsVdoMode)
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, TRUE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);
        }
        #ifdef MTIMAGE_PERFORMANCE_TEST
        _MtImageMeasureTimeEnd("VDO JPG_HW_NFY_FINISHED");
        #endif
    }
    else if (u4State == JPG_HW_NFY_ERROR)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_ERROR;
        if ((UINT32)pv_data == IMG_DRV_NOT_SUPPORT)
        {
            rImageInfo->rCallbackInfo.u4Param = MT_IMAGE_FILE_ERROR;
        }
        else if ((UINT32)pv_data == IMG_DRV_ERR_COMPONENT)
        {
            rImageInfo->rCallbackInfo.u4Param = MT_IMAGE_JPEG_COLOR_FORMAT_ERROR;
        }
        else    //eState==IMG_DRV_HW_ERROR
        {
            rImageInfo->rCallbackInfo.u4Param = MT_IMAGE_IMG_ERROR;
        }
        if (rImageInfo->fgIsVdoMode)
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView] = 0xFF;
        }
    }
    else if (u4State == JPG_HW_NFY_FILL_BUF)
    {
        if (rImageInfo->imgFormat == IMAGE_JPEG)
        {
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
        }
        else
        {
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        }
        ASSERT(prFbmPool != (FBM_POOL_T *)NULL);
        ASSERT(prFbmPool->u4Addr != 0);
    
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_NEEDDATA;
        prFillBuf = (IMG_FILL_BUF_MW_T *)pv_data;
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_trsn_id = prFillBuf->ui4_trsn_id;
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_start_add = (UINT32)prFillBuf->pv_start_add - VIRTUAL(prFbmPool->u4Addr);
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_required_len = prFillBuf->ui4_required_len;
        rImageInfo->rCallbackInfo.rReqDataInfo.u1_reset_pos = (UCHAR)prFillBuf->b_reset_pos;
        rImageInfo->rCallbackInfo.rReqDataInfo.u4_position = prFillBuf->ui4_position;

        #ifdef MTIMAGE_PERFORMANCE_TEST
        _MtImageMeasureTimeEnd("VDO JPG_HW_NFY_FILL_BUF");
        #endif
    }
    else
    {
        return;
    }
    rImageInfo->rCallbackInfo.u4ImgId = u4ImgID;

    MTIMAGE_KERNEL_DEBUG("CB_MTIMAGE_TRIGGER_VDO[%d][u4State=%d]\n", u4ImgID, u4State); 
    _CB_PutEvent(CB_MTIMAGE_TRIGGER_VDO, sizeof(IMG_CALLBACK_INFO_T), &rImageInfo->rCallbackInfo);
}

#endif

void  _MTIMAGE_SlideShow_Notify(SLIDESHOW_NFY_EVENT_TYPE_T e_event)
{
    _rImageInfo[0].u1SlideShowEvent = (UINT8)e_event;
    switch (e_event)
    {
    case SLIDESHOW_NFY_COMPLETE_ONE_FRAME:
    case SLIDESHOW_NFY_COMPLETE_ONE_PIC:
        VERIFY(x_sema_unlock(_rImageInfo[0].hSlideShowSema)== OSR_OK);
        break;
     default:
        break;
    }
    MTIMAGE_KERNEL_DEBUG("SlideShowEvent %d\n", (UINT8)e_event);
}


/*
    just for jpeg linux demo
    *set decode address for decoding
*/
static MT_RESULT_T _MTIMAGE_GetDefaultDecAddr(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_HANDLE hHandle;    
    UCHAR ucImgId;
    FBM_POOL_T *prFbmPoolJPG;
    MT_RESULT_T eResult = MTR_OK;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId = rArg.ai4Arg[1];
    
    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD2);

    rArg.ai4Arg[0] = (UINT32)ucImgId;
    rArg.ai4Arg[1] = prFbmPoolJPG->u4Addr;
    MTIMAGE_KERNEL_DEBUG("dec addr:0x%x\n", prFbmPoolJPG->u4Addr);
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_2ARG_T);
    return eResult;
}

/**set image connect to vdp
*@param connect or disconnect
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_Connect(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    BOOL fgConnect, fgIsVdoMode;
    MTIMAGE_IMGCOLORFORMAT_T eColorFMT;
    UCHAR ucImgId;

    MTIMAGE_INFO_T * rImageInfo ;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    fgConnect = (BOOL)rArg.ai4Arg[0];
    eColorFMT = (MTIMAGE_IMGCOLORFORMAT_T)rArg.ai4Arg[1];
    ucImgId = (UCHAR)rArg.ai4Arg[2];

if(fgConnect==true)
{    
    u4MtimageDBGInsert(_mtimageDebugStep, 0xff-ucImgId);    //start code for debug
}    

u4MtimageDBGInsert(_mtimageDebugStep, 1+ucImgId);

    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Connect rImageInfo=NULL[%d]\n", ucImgId);
        return MTR_NOT_OK;
    }
    
#if defined(IMGDRV_ANDROID)    
    rImageInfo->fgDrvFeedData = 0;
#else
    rImageInfo->fgDrvFeedData = 1;
#endif

    rImageInfo->eImgColorFmt = eColorFMT;
    fgIsVdoMode = (eColorFMT == MT_IMAGE_COLOR_FORMAT_Y_CBCR422) ? TRUE : FALSE;

#ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT
    rImageInfo->fgIsMJPEGVdoMode = ((eColorFMT == MT_IMAGE_COLOR_FORMAT_MJPEG_YCBCR420) || (eColorFMT == MT_IMAGE_COLOR_FORMAT_MJPEG_YCBCR422)) ? TRUE : FALSE;
#endif

    IMG_Lib_Init();
    switch (eColorFMT)
    {
    case MT_IMAGE_COLOR_FORMAT_Y_CBCR422:
        rImageInfo->eImgDispCM = Y_CBCR422_SEP16;
#if defined(IMGDRV_ANDROID)    
        rImageInfo->fgDrvFeedData = 1;
#endif
        break;
    case MT_IMAGE_COLOR_FORMAT_ARGB8888:
        rImageInfo->eImgDispCM = ARGB8888_DIRECT32;
#ifdef MTIMAGE_ANDROID_SUPPORT
        rImageInfo->fgLibjpeg = TRUE;
#endif
        break;
    case MT_IMAGE_COLOR_FORMAT_ARGB1555:
        rImageInfo->eImgDispCM = ARGB1555_DIRECT16;
        break;
    case MT_IMAGE_COLOR_FORMAT_ARGB4444:
        rImageInfo->eImgDispCM = ARGB4444_DIRECT16;
        break;
    case MT_IMAGE_COLOR_FORMAT_ARGB565:
        rImageInfo->eImgDispCM = RGB565_DIRECT16;
        break;
    case MT_IMAGE_COLOR_FORMAT_AYCBCR8888:
        rImageInfo->eImgDispCM = AYCBCR8888_DIRECT32;
        break;
#ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT        
    case MT_IMAGE_COLOR_FORMAT_MJPEG_YCBCR420:
        rImageInfo->eImgDispCM = Y_CBCR420_SEP16;
        rImageInfo->fgDrvFeedData = 1;
        break;
    case MT_IMAGE_COLOR_FORMAT_MJPEG_YCBCR422:
        rImageInfo->eImgDispCM = Y_CBCR422_SEP16;
        rImageInfo->fgDrvFeedData = 1;
        break;
#endif        
    case MT_IMAGE_COLOR_FORMAT_YV16:
        rImageInfo->eImgDispCM = Y_CBCR422_SEP16;
        rImageInfo->fgLibjpeg = TRUE;
        break;
    default:
        break;
    }
#ifndef MTIMAGE_ANDROID_SUPPORT
    rImageInfo->fgLibjpeg = FALSE;
#endif

    VDP_SetThumbnailMode(0, 0);
    SRM_SetMmModeByPath(0, SRM_MM_MODE_TYPE_OFF);

    //if OSD mode, do nothing
    //else if VDP mode, prepare frame buffer
    if (!rImageInfo->fgLibjpeg)
    {
        if (fgIsVdoMode)
        {

            MTIMAGE_KERNEL_DEBUG("$$$Connect =%d, [id=%d], rImageInfo->fgConnect=%d, _rImageFbg.fgFBGCreated=%d\n", fgConnect, ucImgId, rImageInfo->fgConnect, _rImageFbg.fgFBGCreated);        
            
            //if ((rImageInfo->fgConnect==0)||fgConnect==0)   //when first connect => creat FBG, when exit => close FBG
            if (((fgConnect==TRUE)&&(_rImageFbg.fgFBGCreated==FALSE))
                ||((fgConnect==FALSE)&&(_rImageFbg.fgFBGCreated==TRUE)))   //when first connect => creat FBG, when exit => close FBG
            {
                //rImageInfo->eDetectedMode = 0xff;

                //we share FBG for jpg&png decoding, so only connect once or exit once, not twice
                _rImageInfo[0].eDetectedMode = 0xff;
                _rImageInfo[1].eDetectedMode = 0xff;            
                if (!_FBGHandler(rImageInfo, fgConnect))
                {
                    rImageInfo->fgConnect = FALSE;
                    MTIMAGE_KERNEL_DEBUG("_FBGHandler is failed[id=%d]\n", ucImgId);
                    return MTR_NOT_OK;
                }
            }
            //slide show init/stop
            if (fgConnect)
            {
#if 0
                if ((i4Ret = x_img_init_slideshow(_MTIMAGE_SlideShow_Notify)) != SLIDESHOW_OK)
                {
                    MTIMAGE_KERNEL_DEBUG("InitSlideShowLib fail %d\n", i4Ret);
                }
                if (rImageInfo->hSlideShowSema == (HANDLE_T)NULL)
                {
                    VERIFY(x_sema_create(&rImageInfo->hSlideShowSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
                }

#endif
                //rImageInfo->eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
            }
            else
            {
                fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, NULL);
#if 0   //cannt re-init?
                if ((i4Ret = x_img_stop_slideshow()) != SLIDESHOW_OK)
                {
                    MTIMAGE_KERNEL_DEBUG("InitSlideShowStop fail %d\n", i4Ret);
                }
#endif
            }
        }
        rImageInfo->fgConnect = fgConnect;
        rImageInfo->fgIsVdoMode = fgIsVdoMode;
    }
    else
    {
        rImageInfo->fgConnect = fgConnect;
        rImageInfo->fgIsVdoMode = 0;
    }
u4MtimageDBGInsert(_mtimageDebugStep, 9);    
    return MTR_OK;
}


/**Initialize jpeg hw for decoding.
*@param eColorFMT            display color format
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_Open(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    ENUM_IMG_BUFFER_TYPE eImgBufType;
    UINT32 u4CurState;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);


    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    
u4MtimageDBGInsert(_mtimageDebugStep, 11+ucImgId);

    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");        
        return MTR_NOT_OK;
    }

    eImgBufType = (rImageInfo->eImgDispCM == Y_CBCR422_SEP16)?RING_IN_DOUBLE_OUT: RING_IN_SINGLE_OUT;

    //for jpg ARGB8888
    if (((rImageInfo->imgFormat==IMAGE_JPEG)||(rImageInfo->imgFormat==IMAGE_MPO))&&(rImageInfo->eImgDispCM == ARGB8888_DIRECT32))
    {
        eImgBufType = RING_IN_SINGLE_OUT;
    }
#ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT
	if(rImageInfo->fgIsMJPEGVdoMode)
	{
		//eImgBufType = RING_IN_SINGLE_OUT; 
		switch(rImageInfo->eImgDispCM)
		{
		case Y_CBCR422_SEP16:
			eImgBufType = RING_IN_SINGLE_OUT;	
			break;
		case Y_CBCR420_SEP16:
			//for camera MJPEG 420, not use imgrz.
			eImgBufType = RING_IN_DOUBLE_OUT_FOR_CAMERA;			
			break;
		default:
			printk("[_MTIMAGE]: ##Error! Unknown this MJPEG color mode.\n");
			break;
		}
	}
#endif
	MTIMAGE_KERNEL_DEBUG("MTIMAGE_Open eImgBufType=%d, DisCM=%d\n", eImgBufType, rImageInfo->eImgDispCM);

    u4CurState = IMG_GetDecState(ucImgId);
    if (u4CurState == IMG_RELEASE)
    {
#ifdef MTIMAGE_ANDROID_SUPPORT
        if (eImgBufType == RING_IN_SINGLE_OUT)
        {
#endif
            if (IMG_Init(eImgBufType, hHandle, ucImgId, _MTIMAGE_Notify) != E_IMG_OK)
            {
                MTIMAGE_KERNEL_DEBUG("IMG_Init   is failed \n");                
                return MTR_NOT_OK;
            }
#ifdef MTIMAGE_ANDROID_SUPPORT
        }
        //special case NV16
        else if (rImageInfo->fgDrvFeedData == FALSE)
        {
            if (IMG_Init(eImgBufType, hHandle, ucImgId, _MTIMAGE_Notify) != E_IMG_OK)
            {
                MTIMAGE_KERNEL_DEBUG("IMG_Init   is failed \n");                
                return MTR_NOT_OK;
            }
        }
        else
        {
            if (IMG_Init(eImgBufType, hHandle, ucImgId, _MTIMAGE_Notify_VDO) != E_IMG_OK)
            {
                MTIMAGE_KERNEL_DEBUG("IMG_Init   is failed vdo\n");              
                return MTR_NOT_OK;
            }
        }
#endif
        IMG_SetUseMw(ucImgId,TRUE);
        rImageInfo->eRotateDegree = CLOCKWISE_0;     //reset setting
        if (!(rImageInfo->u1PicIdx == 1))    //in the middle of MPF decoding
        {
            x_memset((void *)&rImageInfo->rDecOut, 0, sizeof(IMG_REGION_T));
            x_memset((void *)&rImageInfo->rDecOut2, 0, sizeof(IMG_REGION_T));
        }
        x_memset((void *)&rImageInfo->rCallbackInfo, 0, sizeof(IMG_CALLBACK_INFO_T));
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Open only available when IMG_RELEASE, Cur %d\n", u4CurState);
        return MTR_NOT_OK;
    }
u4MtimageDBGInsert(_mtimageDebugStep, 19);     
    return MTR_OK;
}


/**Start jpeg decoding
*@param pu1Buf                Input Buffer address of jpeg
*@param u4ReadSize            Size of the current jpeg segment
*@param fgEOF                 Eof is in this segment
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_Decode(unsigned long arg)
{
    MTAL_IOCTL_12ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    ENUM_IMG_FORMAT imgFormat;
    IMG_PARAM_T rInfo = {0};
    UINT32 u4YAddr = 0, u4CAddr = 0, u4CurState, u4DecW, u4DecH, u4Pitch,u4DecRatio;
    UCHAR ucFbId;
    UCHAR ucFbgId;
    UINT32 u4MPFPicIdx; //0: 2D pic, 1: MPF 1st pic, 2: MPF 2nd pic
    UINT32 u4DecAddr = 0;
    UINT32 u4SrcWidth,u4SrcHeight,u4ImgSize;
    FBM_POOL_T *prFbmPoolJPG;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;
#ifdef CC_MAPLE_CUST_DRV
    ENUM_CODEC_INFO_T eCodecType;
#endif
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
	UINT32 u4DynMapAddr = 0;
#endif
    USR_SPACE_ACCESS_VALIDATE_12ARG(arg);
    COPY_FROM_USER_12ARG(arg, rArg);


    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    u4DecW = (UINT32)rArg.ai4Arg[1];
    u4DecH = (UINT32)rArg.ai4Arg[2];
    imgFormat = (ENUM_IMG_FORMAT)rArg.ai4Arg[3];
    u4MPFPicIdx = (UINT32)rArg.ai4Arg[4];
    u4DecAddr = (UINT32)rArg.ai4Arg[5];
    u4SrcWidth = (UINT32)rArg.ai4Arg[6];
    u4SrcHeight = (UINT32)rArg.ai4Arg[7];
    u4ImgSize = (UINT32)rArg.ai4Arg[8];
    u4Pitch = (UINT32)rArg.ai4Arg[9];
    ucImgId = (UCHAR)rArg.ai4Arg[10];
    u4DecRatio = (UINT32)rArg.ai4Arg[11];

    u4MtimageDBGInsert(_mtimageDebugStep, 21+ucImgId);
    
    MTIMAGE_KERNEL_DEBUG("Decode [%d][%d][%d][%d][%d][%x] \n", u4SrcWidth, u4SrcHeight,  u4DecW, u4DecH, u4DecRatio, u4DecAddr);   
    
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");        
        return MTR_NOT_OK;
    }

    ucFbgId = _rImageFbg.aucFbgId[_rImageFbg.u1CurView];
    //decode
    u4CurState = IMG_GetDecState(ucImgId);
    
    if (u4CurState == IMG_INIT)
    {
        if (rImageInfo->fgIsVdoMode)
        {
            //check color mode
            if ((rImageInfo->eImgDispCM != Y_CBCR420_SEP16) &&
                    (rImageInfo->eImgDispCM !=Y_CBCR422_SEP16))
            {
                MTIMAGE_KERNEL_DEBUG("Color mode %d, should be YCbCr420 or YCbCr422 in VdoMode.\n", rImageInfo->eImgDispCM);
                return MTR_NOT_OK;
            }
            //alloc frame buffer
            if (u4MPFPicIdx <= 1)
            {
                if (!_AllocFrame(ucImgId,ucFbgId, &ucFbId, &u4YAddr, &u4CAddr))
                {
                    MTIMAGE_KERNEL_DEBUG("_AllocFrame is failed!\n");                               
                    return MTR_NOT_OK;
                }
                _rImageFbg.aucPrevImgFbId[_rImageFbg.u1CurView] = _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView];
                _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView] = ucFbId;
            }
            else
            {
                FBM_GetFrameBufferAddr(ucFbgId, _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView], &u4YAddr, &u4CAddr);
            }
        }

        x_memset((void*)&rInfo, 0, sizeof(IMG_PARAM_T));
        
        rInfo.eFormat = imgFormat;
        rInfo.pu1InBuffer = NULL;    //use internal ring buffer
        rInfo.u8BufSize = 0;            //use internal ring buffer
        rInfo.rDstRegion.u4StartX = 0;
        rInfo.rDstRegion.u4StartY = 0;

        if (!rImageInfo->fgLibjpeg)
        {
            if (rImageInfo->fgIsVdoMode)
            {
                rImageInfo->u1PicIdx = u4MPFPicIdx;
                if (u4MPFPicIdx > 0)
                {
                    rImageInfo->fgIsMPF = TRUE;
                    rInfo.rDstRegion.u4Width = MPF_HALF_W;    //half size
                    rInfo.rDstRegion.u4Height = MPF_HALF_H*2;  //height will be reduce to 1/2 in decoder, but use 1404x1080 output window
                }
                else
                {
                    rImageInfo->fgIsMPF = FALSE;
                    if ((u4DecW > 0) && (u4DecH > 0) && (u4DecW <= 1920) && (u4DecH <= 1080))
                    {
                        MTIMAGE_KERNEL_DEBUG("Specify dec resolution %dx%d\n", u4DecW, u4DecH);
                        rInfo.rDstRegion.u4Width = u4DecW;
                        rInfo.rDstRegion.u4Height = u4DecH;
                    }
                    else
                    {  
                        rInfo.rDstRegion.u4Width = 0;    //keep original width if < 1920
                        rInfo.rDstRegion.u4Height = 0;   //keep original width if < 1080
                    }
                }
                rInfo.rDstRegion.u4Pitch = 0;    //rVpInfo.u4Width;

                rInfo.eDispColorMode = rImageInfo->eImgDispCM;
                rInfo.eRotateDegree = CLOCKWISE_0;
                rInfo.bKeepRatio = TRUE;
                if (u4MPFPicIdx > 1)
                {
                    rInfo.prDecOut = &rImageInfo->rDecOut2;
                    rInfo.rDstRegion.pu1Addr =
                        (UINT8 *)(u4YAddr + rImageInfo->rDecOut.u4Pitch * rImageInfo->rDecOut.u4TotalLine);
                }
                else
                {
                    rInfo.prDecOut = &rImageInfo->rDecOut;
                    rInfo.rDstRegion.pu1Addr = (UINT8 *)u4YAddr;
                }
                rInfo.rDstRegion.u4CbCrOffset = u4CAddr - u4YAddr;
            }
#ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT            
            else if(rImageInfo->fgIsMJPEGVdoMode)
            {
                if ((u4DecW > 0) && (u4DecH > 0) && (u4DecW*u4DecH <= 1920*1080))
                {
                    MTIMAGE_DEBUG("Specify dec resolution %dx%d\n", u4DecW, u4DecH);
                    rInfo.rDstRegion.u4Width = u4DecW;
                    rInfo.rDstRegion.u4Height = u4DecH;
                }
                else
                {
                    rImageInfo->fgIsMPF = FALSE;
                    rInfo.rDstRegion.u4Width = 0;    //keep original width if < 1920
                    rInfo.rDstRegion.u4Height = 0;   //keep original width if < 1080

                }
                rInfo.rDstRegion.u4Pitch = (u4DecW+0xf)&(~0xf);    //rVpInfo.u4Width;

                rInfo.eDispColorMode = rImageInfo->eImgDispCM;
                rInfo.eRotateDegree = CLOCKWISE_0;
                rInfo.bKeepRatio = TRUE;
                rInfo.prDecOut = &rImageInfo->rDecOut;

                #ifdef CC_FBM_3D_OSD
                prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
                #else
                prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
                #endif

                if (prFbmPoolJPG == NULL)
                {
                    return FALSE;
                }
                
                u4DecAddr = (UINT32)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
                
                rInfo.rDstRegion.pu1Addr = (UINT8 *)u4DecAddr;
                rInfo.rDstRegion.u4CbCrOffset = rInfo.rDstRegion.u4Pitch*rInfo.rDstRegion.u4Height;
            }
#endif 
            else
            {

#ifdef IO_MTIMAGE_IS_POST_MT5368
                if (imgFormat == IMAGE_WEBP)
                {
                    if ((u4DecW > 0) && (u4DecH > 0) && (u4DecW <= 1920) && (u4DecH <=1080))
                    {
                        rInfo.rDstRegion.u4Width = u4DecW;
                        rInfo.rDstRegion.u4Height = u4DecH;
                    }
                    else
                    {
                        rInfo.rDstRegion.u4Width = 0;
                        rInfo.rDstRegion.u4Height = 0;
                    }
                }
                else
#endif
                {
                    if ((u4DecW > 0) && (u4DecH > 0) && (u4DecW < 1920) && (u4DecH <1080))
                    {
                        rInfo.rDstRegion.u4Width = u4DecW;
                        rInfo.rDstRegion.u4Height = u4DecH;
                    }
                    else
                    {
                        rInfo.rDstRegion.u4Width = 0;
                        rInfo.rDstRegion.u4Height = 0;
                    }

                }
                rInfo.rDstRegion.u4Pitch = u4Pitch;    //rVpInfo.u4Width;
                MTIMAGE_KERNEL_DEBUG("Specify dec addr 0x%08x\n", u4DecAddr);
                rInfo.eDispColorMode = rImageInfo->eImgDispCM;
                rInfo.eRotateDegree = CLOCKWISE_0;
                rInfo.bKeepRatio = TRUE;
                rInfo.prDecOut = &rImageInfo->rDecOut;
                rInfo.rDstRegion.pu1Addr = (UINT8 *)u4DecAddr;
                rInfo.rDstRegion.u4CbCrOffset = 0;
            }
        }
        else
        {
            rInfo.rDstRegion.u4Width = u4DecW;
            rInfo.rDstRegion.u4Height = u4DecH;
            rInfo.rDstRegion.u4Pitch = 0;
            rInfo.rDstRegion.u4DecRatio = u4DecRatio;
            
            rInfo.eDispColorMode = rImageInfo->eImgDispCM;
            rInfo.eRotateDegree = CLOCKWISE_0;
            rInfo.bKeepRatio = TRUE;
            rInfo.prDecOut = &rImageInfo->rDecOut;
            if (rImageInfo->eImgColorFmt == MT_IMAGE_COLOR_FORMAT_YV16)
            {
                rInfo.bKeepRatio = FALSE;
            }
            if(u4DecAddr == 0)
            {
            #ifdef IO_MTIMAGE_IS_POST_MT5368
                if (imgFormat == IMAGE_PNG)
                {
                    #ifdef CC_FBM_3D_OSD
                    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
                    #else
                    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD2);
                    #endif
                    
                    if((prFbmPoolJPG == NULL)||(prFbmPoolJPG->u4Addr == 0))
                    {
                        return FALSE;
                    }

                    //for png and jpg decoding parallel, upper buffer for jpg, lower buffer for png
                    u4DecAddr = (UINT32)(PAGE_ALIGN(prFbmPoolJPG->u4Addr) + (prFbmPoolJPG->u4Size/2));
                    
                    MTIMAGE_KERNEL_DEBUG("png u4DecAddr=%x[Fbm addr=%x]\n", u4DecAddr, (prFbmPoolJPG->u4Addr));                 
                }
                else
            #endif
                {
                    #ifdef CC_FBM_3D_OSD
                    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
                    #else
                    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
                    #endif


                    if((prFbmPoolJPG == NULL)||(prFbmPoolJPG->u4Addr == 0))
                    {
                        return FALSE;
                    }
                    u4DecAddr = (UINT32)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
                
                    MTIMAGE_KERNEL_DEBUG("jpg u4DecAddr=%x\n", u4DecAddr);
                }   

                if (rImageInfo->eImgColorFmt != MT_IMAGE_COLOR_FORMAT_YV16)
                {
                    if((((rInfo.rDstRegion.u4Width + 0xf)&(~0xf)) * rInfo.rDstRegion.u4Height * 4) > 0x7f9000)
                    {
                        rInfo.rDstRegion.u4Width = 0;
                        rInfo.rDstRegion.u4Height = 0;
                        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Decode Dst size error [%d][%d]\n", rInfo.rDstRegion.u4Width, rInfo.rDstRegion.u4Height);
                    }
                }
            }
            else
            {
                if (rImageInfo->eImgColorFmt == MT_IMAGE_COLOR_FORMAT_YV16)
                {
                    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX2);
                    
                    if((prFbmPoolJPG == NULL)||(prFbmPoolJPG->u4Addr == 0))
                    {
                        return FALSE;
                    }
                    u4DecAddr = (UINT32)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
                }  
            }
            rInfo.rDstRegion.pu1Addr = (UINT8*)u4DecAddr;
            rInfo.rDstRegion.u4CbCrOffset = ((rInfo.rDstRegion.u4Width + 0xf)&(~0xf)) * rInfo.rDstRegion.u4Height;
            rImageInfo->u4TgtUsrAddr = u4DecAddr;
        }
        
        if (rImageInfo->eImgColorFmt != MT_IMAGE_COLOR_FORMAT_YV16)
        {

#ifdef JPG_DECODING_USING_RZ_IOMMU
            if((ucImgId==0)&&(rImageInfo->fgLibjpeg))
            {
                rInfo.u4JpgUsingRzIOMMU_MMUTableAddr = HalGetMMUTableAddress();
                MTIMAGE_KERNEL_DEBUG("_MTIMAGE_MMUTableAddr = %x\n",rInfo.u4JpgUsingRzIOMMU_MMUTableAddr);       
                HalInvalidateDCacheMultipleLine((UINT32)rInfo.rDstRegion.pu1Addr,  MAX_WIDTH * MAX_HEIGHT*4); 
            }           
#endif
#ifdef PNG_DECODING_USING_RZ_IOMMU
            if((ucImgId==1)&&(rImageInfo->fgLibjpeg))
            {
                rInfo.u4JpgUsingRzIOMMU_MMUTableAddr = HalGetMMUTableAddress();
                MTIMAGE_KERNEL_DEBUG("PNG _MTIMAGE_MMUTableAddr = %x\n",rInfo.u4JpgUsingRzIOMMU_MMUTableAddr);
                HalInvalidateDCacheMultipleLine((UINT32)rInfo.rDstRegion.pu1Addr,  MAX_WIDTH * MAX_HEIGHT*4); 
            }           
#endif
            if (rInfo.u4JpgUsingRzIOMMU_MMUTableAddr == 0)
            {
                if (rImageInfo->eImgColorFmt == MT_IMAGE_COLOR_FORMAT_Y_CBCR422)
                {
                	#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                	u4DynMapAddr = (UINT32)BSP_MapReservedMem((void*)rInfo.rDstRegion.pu1Addr, MAX_WIDTH * MAX_HEIGHT);
                	if(!u4DynMapAddr)
                    {
                        printk("%s %d dynamic map fail\n", __FUNCTION__,__LINE__);
                        ASSERT(0);
                    }
                	u4DynMapAddr = (UINT32)BSP_MapReservedMem((void*)rInfo.rDstRegion.pu1Addr + rInfo.rDstRegion.u4CbCrOffset, MAX_WIDTH * MAX_HEIGHT);
                	if(!u4DynMapAddr)
                    {
                        printk("%s %d dynamic map fail\n", __FUNCTION__,__LINE__);
                        ASSERT(0);
                    }
                	#endif
                		
                	HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)rInfo.rDstRegion.pu1Addr), MAX_WIDTH * MAX_HEIGHT);
                	HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)(rInfo.rDstRegion.pu1Addr + rInfo.rDstRegion.u4CbCrOffset)), MAX_WIDTH * MAX_HEIGHT);
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                    BSP_UnMapReservedMem((void *)VIRTUAL((UINT32)rInfo.rDstRegion.pu1Addr));
                    BSP_UnMapReservedMem((void *)VIRTUAL((UINT32)(rInfo.rDstRegion.pu1Addr + rInfo.rDstRegion.u4CbCrOffset)));
#endif
                }
                else if (rImageInfo->eImgColorFmt == MT_IMAGE_COLOR_FORMAT_MJPEG_YCBCR420 ||
                	rImageInfo->eImgColorFmt == MT_IMAGE_COLOR_FORMAT_MJPEG_YCBCR422)
                {
                	#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                	u4DynMapAddr = (UINT32)BSP_MapReservedMem((void*)rInfo.rDstRegion.pu1Addr, rInfo.rDstRegion.u4CbCrOffset * 2);
                	if(!u4DynMapAddr)
                    {
                        printk("%s %d dynamic map fail\n", __FUNCTION__,__LINE__);
                        ASSERT(0);
                    }                    
                	#endif
                		
                	HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)rInfo.rDstRegion.pu1Addr), rInfo.rDstRegion.u4CbCrOffset * 2);
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                    BSP_UnMapReservedMem((void *)VIRTUAL((UINT32)rInfo.rDstRegion.pu1Addr));
#endif
                }
                else
                {
                	#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                	u4DynMapAddr = (UINT32)BSP_MapReservedMem((void*)rInfo.rDstRegion.pu1Addr, MAX_WIDTH * MAX_HEIGHT * 4);
                	if(!u4DynMapAddr)
                    {
                        printk("%s %d dynamic map fail\n", __FUNCTION__,__LINE__);
                        ASSERT(0);
                    }
                	#endif
                	HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)rInfo.rDstRegion.pu1Addr), MAX_WIDTH * MAX_HEIGHT*4);
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                    BSP_UnMapReservedMem((void *)VIRTUAL((UINT32)rInfo.rDstRegion.pu1Addr));
#endif
                }
            }
        }

         
#ifdef IO_MTIMAGE_IS_POST_MT5368
        if (imgFormat == IMAGE_PNG)
        {
            rInfo.rSrcRegion.u4SrcStartX = 0;
            rInfo.rSrcRegion.u4SrcStartY = 0;
            rInfo.rSrcRegion.u4SrcWidth = u4SrcWidth;
            rInfo.rSrcRegion.u4SrcHeight = u4SrcHeight;
            rInfo.u4DecColorMode = 12;//GFX_COLORMODE_ARGB_D8888
        }
#endif

        IMG_Set_ImgSize(ucImgId,u4ImgSize);

#ifdef _IOMTIMAGE_JPG_MID_PROTECTION
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE physical decode addr= %x\n",PHYSICAL(u4DecAddr));
        _MTIMAGE_Set_MID_Protection(PHYSICAL(u4DecAddr), 4*1024, 4);
#endif

        #ifdef MTIMAGE_PERFORMANCE_TEST
        _MtImageMeasureTimeBegin();
        #endif
        rInfo.fgDrvFeedData = rImageInfo->fgDrvFeedData;

#ifdef CC_MAPLE_CUST_DRV
        switch(rInfo.eFormat)
        {
    	case IMAGE_JPEG:
    		eCodecType = E_VDEC_CODEC_JPEG;
    		break;     		
    	case IMAGE_MPO:
    		eCodecType = E_VDEC_CODEC_MPO;
    		break;        		
		default:
			eCodecType = E_VDEC_CODEC_UNKNOWN;
    		break;			
        }
        FBM_SetCodecInfo(ucFbgId, eCodecType);
#endif
		
        if (IMG_Decode(ucImgId,rInfo) != E_IMG_OK)
        {
            MTIMAGE_KERNEL_DEBUG("IMG_Decode NG\n");
            return MTR_NOT_OK;
        }
        else
        {
            MTIMAGE_KERNEL_DEBUG("Decode output Y 0x%x, C 0x%x\n", (UINT32)rInfo.rDstRegion.pu1Addr,
                          (UINT32)rInfo.rDstRegion.pu1Addr + rInfo.rDstRegion.u4CbCrOffset);
        }
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Decode only available when IMG_INIT, Cur %d\n", u4CurState);
        return MTR_NOT_OK;
    }

    u4MtimageDBGInsert(_mtimageDebugStep, 29);
    MTIMAGE_KERNEL_DEBUG("=========================rInfo.rDstRegion.u4Pitch=%d \n", rInfo.rDstRegion.u4Pitch);

    return MTR_OK;
}

EXTERN UCHAR VDP_FrcGetLockFrameBuffer(UCHAR ucVdpId); // from /b2r/vdp_frc.h:
/**Set image parameters for display.
*@param prImgSetting          Image setting for display
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_Display(unsigned long arg)
{
    MTIMAGE_HANDLE hHandle;
    MTAL_IOCTL_6ARG_T rArg;
    MTIMAGE_PARAM_T *prImgSetting;
    MTVDO_REGION_T *prRegion;
    MTIMAGE_PARAM_T rImgSetting;
    MTVDO_REGION_T rRegion;
    BOOL fgPartialSrc;
    UCHAR ucFbgId;
    IMG_BUF_PARAM_T rCmd={0};
    UINT32 u4YAddr, u4CAddr, u4CurState, u4DispYAddr, u4DispCAddr, u4FrmNum;
    UINT32 u4DstYAddr, u4DstCAddr;
    UCHAR ucFbId, ucSlideShowFbId;
    //INT32 i4Ret;
    // 3d related parameter
    UINT32 u43DInOut;
    UINT16 u2InFmt, u2OutFmt;
    E_TDTV_DECODER_INPUT eDetectedMode;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    DRV_OSD_T rOsd;
    FBM_POOL_T *prFbmPoolJPG;
    UINT32 u4List;
    TDTV_3D_CROP_INTO_T cropinfo;

    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;
     
    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);


    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prImgSetting=(MTIMAGE_PARAM_T *)rArg.ai4Arg[1];
    fgPartialSrc = (BOOL)rArg.ai4Arg[2];
    ucImgId = (UCHAR)rArg.ai4Arg[4];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

    u4MtimageDBGInsert(_mtimageDebugStep, 31+ucImgId);
    
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return MTR_NOT_OK;
    }

    if (fgPartialSrc)
    {
        prRegion = (MTVDO_REGION_T *)rArg.ai4Arg[3];
        USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
        COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_REGION_T);
    }
    else
    {
        UNUSED(prRegion);
        x_memset((void*)&rRegion,0,sizeof(MTVDO_REGION_T));
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(prImgSetting, MTIMAGE_PARAM_T);
    COPY_FROM_USER_ARG(prImgSetting, rImgSetting, MTIMAGE_PARAM_T);

    ucFbgId = _rImageFbg.aucFbgId[_rImageFbg.u1CurView];
    rImageInfo->u4PlaneId = rImgSetting.u1PlaneId;
    u4CurState = IMG_GetDecState(ucImgId);
    if (u4CurState == IMG_IDLE) // will inter idle after decode finish in MW mode
    { 
        #ifdef CC_MTK_AUTO_TEST
        {
            UINT32 ui4_width, ui4_height, ui4_pitch, i, j, checkSum = 0;
            UCHAR* bitmap;

            ui4_width = rImageInfo->rDecOut.u4Width;
            ui4_height = rImageInfo->rDecOut.u4Height;
            ui4_pitch = rImageInfo->rDecOut.u4Pitch;
            bitmap = (UCHAR*)VIRTUAL((UINT32)rImageInfo->rDecOut.pu1Addr);

            //Y
            for (i = 0; i < ui4_height; i++)
            {
                for (j = 0; j < ui4_width; j++)
                {
                    checkSum += bitmap[i*ui4_pitch+j];
                }
            }
            //CbCr
            bitmap = bitmap + rImageInfo->rDecOut.u4CbCrOffset;
            for (i = 0; i < ui4_height; i++)
            {
                for (j = 0; j < ui4_width; j++)
                {
                    checkSum += bitmap[i*ui4_pitch+j];
                }
            }

            printk("3d_photo_display checksum:0x%x.\n", checkSum);
        }
        #endif
        rCmd.eDispColorMode = rImageInfo->eImgDispCM;
        rCmd.eRotateDegree = rImgSetting.eRotation;
        rCmd.rSrcRegion.pu1Addr = rImageInfo->rDecOut.pu1Addr;
        rCmd.rSrcRegion.u4CbCrOffset = rImageInfo->rDecOut.u4CbCrOffset;
        rCmd.rSrcRegion.u4Depth = rImageInfo->rDecOut.u4Depth;
        rCmd.rSrcRegion.u4Pitch = rImageInfo->rDecOut.u4Pitch;
        rCmd.rSrcRegion.u4TotalLine = rImageInfo->rDecOut.u4TotalLine;
        rCmd.rSrcRegion.u4StartX = (fgPartialSrc) ? rRegion.u4X : rImageInfo->rDecOut.u4StartX;
        rCmd.rSrcRegion.u4StartY = (fgPartialSrc) ? rRegion.u4Y : rImageInfo->rDecOut.u4StartY;
        rCmd.rSrcRegion.u4Width = (fgPartialSrc) ? rRegion.u4Width : rImageInfo->rDecOut.u4Width;
        rCmd.rSrcRegion.u4Height = (fgPartialSrc) ? rRegion.u4Height : rImageInfo->rDecOut.u4Height;

        rCmd.rDstRegion.u4StartX = rImgSetting.rDstRegion.u4X;
        rCmd.rDstRegion.u4StartY = rImgSetting.rDstRegion.u4Y;
        rCmd.rDstRegion.u4Width = rImgSetting.rDstRegion.u4Width;
        rCmd.rDstRegion.u4Height = rImgSetting.rDstRegion.u4Height;
        rCmd.rDstRegion.u4Depth = rImageInfo->rDecOut.u4Depth;
        rCmd.bKeepRatio = rImageInfo->fgIsMPF ? FALSE : TRUE;
        rCmd.bKeepOriResolution = rImageInfo->fgIsMPF ? FALSE : TRUE;
        //when picture is big enough, we don't keep original resolution and resize it fit to panel
        if ((rImageInfo->fgIsMPF)||(rImageInfo->rDecOut.u4Width==MAX_WIDTH)||(rImageInfo->rDecOut.u4Height==MAX_HEIGHT)
                ||(((rImgSetting.eRotation==MT_IMAGE_ROTATE_90)||(rImgSetting.eRotation==MT_IMAGE_ROTATE_270))&&(rImageInfo->rDecOut.u4Width>MAX_HEIGHT))
           )
        {
            rCmd.bKeepOriResolution = FALSE;
        }
        else
        {
            rCmd.bKeepOriResolution = TRUE;
        }

        if (rImageInfo->fgIsMPF)
            rCmd.bKeepOriResolution = TRUE;

        if (rImageInfo->fgIsVdoMode)
        {
            //HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)rCmd.rSrcRegion.pu1Addr), (UINT32)(rCmd.rSrcRegion.u4CbCrOffset*2));
            if (FBM_GetFrameBufferStatus(ucFbgId, _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView]) != FBM_FB_STATUS_READY)
            {
                MTIMAGE_KERNEL_DEBUG("Decoded frame not ready!\n");
                return MTR_NOT_OK;
            }
            if (!_AllocFrame(ucImgId,ucFbgId, &_rImageFbg.aucCurDispFbId[_rImageFbg.u1CurView], &u4YAddr, &u4CAddr))
            {
                MTIMAGE_KERNEL_DEBUG("_AllocFrame is failed!\n");               
                return MTR_NOT_OK;
            }
            FBM_GetFrameBufferSize(ucFbgId, &rCmd.rDstRegion.u4Pitch, &rCmd.rDstRegion.u4TotalLine);
            rCmd.rDstRegion.pu1Addr = (UINT8*)u4YAddr;
            rCmd.rDstRegion.u4CbCrOffset = u4CAddr - u4YAddr;
            if (rImgSetting.fgClean)
            {   //todo?
            }
        }
        else //osd
        {
            rCmd.rDstRegion.u4Pitch = ALIGN16(rCmd.rDstRegion.u4Width * rImageInfo->rDecOut.u4Depth);
            rCmd.rDstRegion.u4TotalLine = rImgSetting.rDstRegion.u4Height;
            //  rCmd.rDstRegion.pu1Addr = rCmd.rDstRegion.pu1Addr;
            rCmd.rDstRegion.u4CbCrOffset = 0;
            rOsd.u4Width = rCmd.rDstRegion.u4Width;
            rOsd.u4Height = rCmd.rDstRegion.u4Height;
            rOsd.u4ScaleWidth = rCmd.rDstRegion.u4Width;
            rOsd.u4ScaleHeight = rCmd.rDstRegion.u4Height;
            #if 0
            rOsd.u4Left = rCmd.rDstRegion.u4StartX / 2;
            rOsd.u4Top = rCmd.rDstRegion.u4StartY / 2;
            #endif
            rOsd.u4Left = rCmd.rDstRegion.u4StartX;
            rOsd.u4Top = rCmd.rDstRegion.u4StartY;

           if ((rCmd.rDstRegion.u4StartX*2>0xffffffff)||(rCmd.rDstRegion.u4StartY*2>0xffffffff))   //shut warning-generator down
            {
                return FALSE;
            }

            if ((rCmd.rDstRegion.u4Width == rCmd.rSrcRegion.u4Width) &&
                    (rCmd.rDstRegion.u4Height == rCmd.rSrcRegion.u4Height))
            {
                rOsd.u4Left = 0;
                rOsd.u4Top = 0;
            }
            else
            {
                rOsd.u4Width = rCmd.rDstRegion.u4Width - rCmd.rDstRegion.u4StartX*2;
                rOsd.u4Height = rCmd.rDstRegion.u4Height - rCmd.rDstRegion.u4StartY*2;
            }
            rCmd.rDstRegion.u4StartX = 0;
            rCmd.rDstRegion.u4StartY = 0;
#if (defined(CC_MT5389))
            prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_B2R);
#else
            #ifdef CC_FBM_3D_OSD
            prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
            #else
            prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
            #endif
#endif
            if((prFbmPoolJPG == NULL)||(prFbmPoolJPG->u4Addr == 0))
            {
                return FALSE;
            }
            //rOsd.pu1FrameBaseAddr = BSP_AllocAlignedDmaMemory(rCmd.rDstRegion.u4Pitch * rOsd.u4Height, 16);
            rOsd.pu1FrameBaseAddr = (UINT8 *)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
            //OSD_Init();
            if (rOsd.u4Width* rOsd.u4Height*4>0xffffffff)   //shut warning-generator down
            {
                return FALSE;
            }
            //x_memset((void*)VIRTUAL((UINT32)rOsd.pu1FrameBaseAddr),0, rOsd.u4Width* rOsd.u4Height*4);
            //this fbm address is not mapped, not support virtual operation.
            GFX_Memset(rOsd.pu1FrameBaseAddr, ALIGN16(rOsd.u4Width << 2), rOsd.u4Height, 0);
			
            OSD_RGN_Create(&rImageInfo->u4DisplayRegionId, rOsd.u4Width, rOsd.u4Height, (void*)rOsd.pu1FrameBaseAddr,
                           OSD_CM_AYCBCR8888_DIRECT32, rCmd.rDstRegion.u4Pitch, rOsd.u4Left, rOsd.u4Top,
                           rOsd.u4Width, rOsd.u4Height);
            OSD_RGN_LIST_Create(&u4List);
            OSD_RGN_Insert(rImageInfo->u4DisplayRegionId, u4List);
            OSD_PLA_FlipTo(rImageInfo->u4PlaneId, u4List);
            OSD_RGN_Set(rImageInfo->u4DisplayRegionId, (INT32)OSD_RGN_MIX_SEL,
                        (UINT32)OSD_BM_PIXEL);
            OSD_SC_Scale(OSD_PLA_GetScaler(rImageInfo->u4PlaneId),
                                    TRUE,
                                    rCmd.rDstRegion.u4Width,
                                    rCmd.rDstRegion.u4Height,
                                    PANEL_GetPanelWidth(),
                                    PANEL_GetPanelHeight());

            rCmd.rDstRegion.pu1Addr = rOsd.pu1FrameBaseAddr;//_rDisplayOSD.pu1FrameBaseAddr;

        }

        if (rImageInfo->fgIsMPF)
        {
            rCmd.rSrcRegion.u4TotalLine <<= 1;
            rCmd.rSrcRegion.u4Height <<= 1;
        }
        //resize or rotation from decoded frame to display frame
        IMG_Set(rCmd, &rImageInfo->rDecOut);

        _ucImgId_3D = ucImgId;
        _rMt3DCropinfo[_ucImgId_3D].u4X = rImageInfo->rDecOut.u4StartX;
        _rMt3DCropinfo[_ucImgId_3D].u4Y = rImageInfo->rDecOut.u4StartY;
        _rMt3DCropinfo[_ucImgId_3D].u4Width = rImageInfo->rDecOut.u4Width;
        _rMt3DCropinfo[_ucImgId_3D].u4Height = rImageInfo->rDecOut.u4Height;

        if (rImageInfo->fgIsVdoMode && rImgSetting.fgDisplay)
        {
            VDP_SetThumbnailMode(0, 0);
            SRM_SetMmModeByPath(0, SRM_MM_MODE_TYPE_VIDEO);
            
            if (rImageInfo->fgIsMPF)
            {
                eDetectedMode = E_TDTV_DECODER_INPUT_TB_P;
                MTIMAGE_KERNEL_DEBUG("MPO set 3D E_TDTV_DECODER_INPUT_TB_P[%d]\n", rImageInfo->eDetectedMode);
            }
            else
            {
                eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
                MTIMAGE_KERNEL_DEBUG("MPO set 2D E_TDTV_DECODER_INPUT_2D_P[%d]\n", rImageInfo->eDetectedMode);
            }

            _MTIMAGE_3DCropFuncCB(&cropinfo);
            fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, _MTIMAGE_3DCropFuncCB);
            u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);

            if (eDetectedMode != rImageInfo->eDetectedMode)
            {
                u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
                u2InFmt = u43DInOut & 0xFFFF;
                u2OutFmt = (u43DInOut >> 16) & 0xFFFF;
                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);

                if (prFbmSeqHdr == NULL)
                {
                    MTIMAGE_KERNEL_DEBUG("prFbmSeqHdr is NULL\n", rImageInfo->eDetectedMode);                  
                    ASSERT(prFbmSeqHdr != NULL);
                    return MTR_NOT_OK;
                }
                switch (u2OutFmt)
                {
                case E_TDTV_DECODER_OUTPUT_NATIVE:
                    prFbmSeqHdr->fgB2R3DEnable = false;
                    //MTIMAGE_MPO_DEBUG("MPO set 2D/3D E_TDTV_DECODER_OUTPUT_NATIVE[%d][%d]\n", eDetectedMode, _rImageInfo.eDetectedMode);
                    break;
                case E_TDTV_DECODER_OUTPUT_3D_FS:
                    prFbmSeqHdr->fgB2R3DEnable = TRUE;
                    prFbmSeqHdr->fgB2RForce2D = FALSE;
                    prFbmSeqHdr->u4B2R3DWidthOffset = 0;
                    prFbmSeqHdr->u4B2R3DHeightOffset = rImgSetting.rDstRegion.u4Height >> 1;
                    prFbmSeqHdr->u1B2R3DType = B2R_3D_TOP_N_BOTTOM;
                    break;
                default:
                    break;
                }
                rImageInfo->eDetectedMode = eDetectedMode;
                FBM_SetFrameBufferPicFlag(ucFbgId, _rImageFbg.aucCurDispFbId[_rImageFbg.u1CurView], FBM_MM_3D_CHG_FLAG);
            }

            //slide show
            if (rImgSetting.fgIsSlideShow)
            {
                rImageInfo->u1SlideShowEvent = SLIDESHOW_NFY_STOP;
                u4FrmNum = 0;

                do
                {
                    ucFbId = VDP_FrcGetLockFrameBuffer((UCHAR)0);
                    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4DispYAddr, &u4DispCAddr);
                    if (!_AllocFrame(ucImgId,ucFbgId, &ucSlideShowFbId, &u4DstYAddr, &u4DstCAddr))
                    {
                        MTIMAGE_KERNEL_DEBUG("_AllocFrame is failed!\n");                                       
                        return MTR_NOT_OK;
                    }
                    MTIMAGE_KERNEL_DEBUG("SlideShow Src Y 0x%x, C 0x%x, Disp Y 0x%x, C 0x%x",
                                  (u4YAddr), (u4CAddr), (u4DispYAddr), (u4DispCAddr));

                    GFX_Lock();
                    GFX_MMU_Set_Enable(FALSE);                    
                    GFX_SetSrc((UINT8 *)u4DispYAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_SetDst((UINT8 *)u4DstYAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine);
                    GFX_SetSrc((UINT8 *)u4DispCAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_SetDst((UINT8 *)u4DstCAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine >> 1);
                    GFX_Flush();
                    GFX_Wait();
                    GFX_Unlock();
#if 0
                    x_img_async_slideshow(_ConvertSlideShowType(rImageInfo->u1SlideShowType),
                                          VIRTUAL(u4YAddr), VIRTUAL(u4CAddr), VIRTUAL(u4DispYAddr), VIRTUAL(u4DispCAddr),
                                          VIRTUAL(u4DstYAddr), VIRTUAL(u4DstCAddr),
                                          rCmd.rDstRegion.u4CbCrOffset, rCmd.rDstRegion.u4CbCrOffset,
                                          rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine, MAX_SLIDE_SHOW_PIC_PERIOD);
                    i4Ret = x_sema_lock_timeout(rImageInfo->hSlideShowSema, MAX_SLIDE_SHOW_FRM_PERIOD);

                    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TIMEOUT));
                    if (i4Ret == OSR_TIMEOUT)
                    {
                        MTIMAGE_KERNEL_DEBUG("WaitSlideShow timeout\n");
                    }
                    
#endif
                    u4FrmNum++;

                    if (rImageInfo->u1SlideShowEvent == SLIDESHOW_NFY_COMPLETE_ONE_PIC)
                    {
                        FBM_SetFrameBufferStatus(ucFbgId, ucSlideShowFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
                    }
                    else
                    {
                        FBM_SetFrameBufferStatus(ucFbgId, ucSlideShowFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
                    }
                }
                while ((rImageInfo->u1SlideShowEvent != SLIDESHOW_NFY_COMPLETE_ONE_PIC) &&
                        (u4FrmNum <= MAX_SLIDE_SHOW_FRM_NUM));
                if (rImageInfo->u1SlideShowEvent != SLIDESHOW_NFY_COMPLETE_ONE_PIC)
                {
                    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display SlideShow Fail, event %d~ ", rImageInfo->u1SlideShowEvent);
                }

                FBM_SetFrameBufferStatus(ucFbgId, _rImageFbg.aucCurDispFbId[_rImageFbg.u1CurView], (UCHAR)FBM_FB_STATUS_EMPTY);
            }
            else
            {
                FBM_SetFrameBufferStatus(ucFbgId, _rImageFbg.aucCurDispFbId[_rImageFbg.u1CurView], (UCHAR)FBM_FB_STATUS_DISPLAYQ);
            }
        }
        else
        {
            #ifndef ANDROID_TWO_WORLDS
            OSD_PLA_Enable(rImageInfo->u4PlaneId, TRUE);
            #endif
        }

    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display only available when IMG_IDLE, Cur %d\n", u4CurState);
        return MTR_NOT_OK;
    }

u4MtimageDBGInsert(_mtimageDebugStep, 39);

    return MTR_OK;
}


/**Close jpeg hw
*@param
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_Close(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    UCHAR ucFbgId ,ucFbId, ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

u4MtimageDBGInsert(_mtimageDebugStep, 41+ucImgId);
    
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return MTR_NOT_OK;
    }

#ifdef _IOMTIMAGE_JPG_MID_PROTECTION
    _MTIMAGE_Disable_MID_Protection(4);
#endif

    ucFbgId = _rImageFbg.aucFbgId[_rImageFbg.u1CurView];
    ucFbId = _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView];

    if (!rImageInfo->fgLibjpeg)
    {
        if (rImageInfo->fgIsVdoMode &&
                !(rImageInfo->u1PicIdx == 1))   //MPF 1st picture
        {   
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
        }
        else
        {
#ifndef ANDROID_TWO_WORLDS
            OSD_PLA_Enable(rImageInfo->u4PlaneId, FALSE);
#endif
            OSD_RGN_Delete(rImageInfo->u4DisplayRegionId);
        }
    }
    else
    {
        rImageInfo->fgConnect = FALSE;
        rImageInfo->fgLibjpeg = FALSE;
    }
    IMG_Release(ucImgId);

u4MtimageDBGInsert(_mtimageDebugStep, 49);

MTIMAGE_KERNEL_DEBUG("Decode close\n"); 
MTIMAGE_KERNEL_DEBUG("============ \n"); 
    return MTR_OK;
}

/**Stop jpeg processing
*@param
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_Stop(unsigned long arg)
{
    MTIMAGE_HANDLE hHandle;
    MTAL_IOCTL_2ARG_T rArg;
    UCHAR ucImgId;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];

u4MtimageDBGInsert(_mtimageDebugStep, 51+ucImgId);

    //re-write to abort decoding
    //while (IMG_GetDecState() == IMG_DECODE)
    {
      //  x_thread_delay(1);
    }

    if (IMG_GetDecState(ucImgId) !=IMG_RELEASE)
    {
        IMG_Stop(ucImgId);
        //IMG_SetFillBsBufEnd(0, 0, TRUE);
        //IMG_Release();
    }
    
u4MtimageDBGInsert(_mtimageDebugStep, 59);

    return MTR_OK;
}

/**Set the stream settings
*@param Image Handle
*@param pSettings
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_Set(unsigned long arg)
{
#if 0
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    MTIMAGE_SETTING_T* prSettings = NULL, rSettings;
    PLAYMGR_SETTING_T rPlaySetting;
    MTIMAGE_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prSettings=(MTIMAGE_SETTING_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prSettings, MTIMAGE_SETTING_T);
    COPY_FROM_USER_ARG(prSettings, rSettings, MTIMAGE_SETTING_T);

    MTIMAGE_KERNEL_DEBUG(" - void\n");
    //MTIMAGE_SET_PRINT("prSettings",prSettings);

    //add driver implementation code here
    eInstance = _MTIMAGE_GetInstance(hHandle);

    if (eInstance == MTIMAGE_IMAGE)
    {
        _MTIMAGE_OpenImg(hHandle, rSettings.eColorFormat);
        //MT_IMAGE_COLOR_FORMAT_ARGB8888
        dwJpgTgtWidth = rSettings.u2VTrack;
        dwJpgTgtHeight = rSettings.u2ATrack;
        dwJpgTgtPtr = rSettings.pu1SubtitleBuffer;
        uJpgColorFmt = rSettings.eColorFormat;
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG(" - Instance Error \n");
        return MTR_NOT_OK;
    }
#endif

    return MTR_OK;
}

#ifdef PNG_GETHDR_SUPPORT
extern UINT32 _u4PngHdr_clr_type;
extern UINT32 _u4PngHdr_tRNS_exist;
extern UINT32 _u4PngHdr_trans_num;
extern UINT32 _u4PngHdr_bDepth;
extern UINT32 _u4is9PatchPng;
extern UINT32 _u49PatchOffset;
/**Get the png header
*@param handle
*@param prStatus
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_GetPngHdr(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    MT_RESULT_T eResult = MTR_OK;

u4MtimageDBGInsert(_mtimageDebugStep, 62);

    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];

    rArg.ai4Arg[0] = _u4PngHdr_clr_type;
    rArg.ai4Arg[1] = _u4PngHdr_bDepth;
    rArg.ai4Arg[2] = _u4PngHdr_tRNS_exist;
    rArg.ai4Arg[3] = _u4PngHdr_trans_num;
#ifdef NINE_PATCH_PNG_SUPPORT
    rArg.ai4Arg[4] = _u4is9PatchPng;
    rArg.ai4Arg[5] = _u49PatchOffset;    
#else
    rArg.ai4Arg[4] = 0;
    rArg.ai4Arg[5] = 0;
#endif

    //MTIMAGE_KERNEL_DEBUG("_MTIMAGE_GetPngHdr %d %d %d %d %d 0x%x\n", rArg.ai4Arg[0], rArg.ai4Arg[1], rArg.ai4Arg[2], rArg.ai4Arg[3], rArg.ai4Arg[4], rArg.ai4Arg[5]);
    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
u4MtimageDBGInsert(_mtimageDebugStep, 69);
    return eResult;
}
#endif /*PNG_GETHDR_SUPPORT*/

/**Get the stream settings and status
*@param handle
*@param prSettings
*@param prStatus
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_GetInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    UINT32 u4CurState;
    MT_RESULT_T eResult = MTR_NOT_OK;   
    UCHAR ucImgId;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId = (UCHAR)rArg.ai4Arg[1];

    u4MtimageDBGInsert(_mtimageDebugStep, 71+ucImgId);  

    u4CurState = IMG_GetDecState(ucImgId);
    rArg.ai4Arg[1] = u4CurState;

    if (u4CurState == IMG_RELEASE)
    {
        eResult = MTR_OK;
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_GetInfo [%d] u4CurState = %d\n", ucImgId, u4CurState);
        if(u4CurState==0)
        {
            //0 means that image module isn't be initiated.
            IMG_Lib_Init();
            u4CurState = IMG_GetDecState(ucImgId);
            rArg.ai4Arg[1] = u4CurState;
            if (u4CurState == IMG_RELEASE)
            {
                eResult = MTR_OK;
            }
        }
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_2ARG_T);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_2ARG_T);
    
u4MtimageDBGInsert(_mtimageDebugStep, 79);

    return eResult;
}

/**Get the imgrz numbers
*@param prStatus
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_GetImgrzNum(unsigned long arg)
{    
    MTAL_IOCTL_2ARG_T rArg;
    MT_RESULT_T eResult = MTR_OK;

	rArg.ai4Arg[0] = IMGRZ_NUM;
	rArg.ai4Arg[1] = 0;
	
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_2ARG_T);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_2ARG_T);
	
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_GetImgrzNum rArg.ai4Arg[0] = %d\n", rArg.ai4Arg[0]);
	
    return eResult;
}

//MT_IMAGE_COLOR_FORMAT_ARGB8888
static void  _MTIMAGE_IMGYUV2RGB(UCHAR ucImgId,unsigned long arg)
{
    MTIMAGE_HANDLE hHandle;
    UINT32 u4YSrcLen=0;
    UINT32 u4ARGBSize = 0;
    FBM_POOL_T *prFbmPoolJPG;
    UINT32  u4Colorfmt,u4Pitch,u4Addr,u4Size;

    MTIMAGE_INFO_T * rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return  ;
    }
    u4YSrcLen=(rImageInfo->rDecOut.u4Width + 0xF) & (~0xF);
    u4ARGBSize = (UINT32)(u4YSrcLen*(rImageInfo->rDecOut.u4Height)*4);
//    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_IMGYUV2RGB\n");
    switch (rImageInfo->eImgColorFmt)
    {
    case MT_IMAGE_COLOR_FORMAT_RGB565:
        u4Colorfmt = CM_RGB565_DIRECT16;
        u4Pitch = u4YSrcLen<<1;
        break;
    case MT_IMAGE_COLOR_FORMAT_RGB888:
    default:
        u4Colorfmt = CM_ARGB8888_DIRECT32;
        u4Pitch = u4YSrcLen<<2;
        break;
    }

    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
    ASSERT(prFbmPoolJPG != NULL);
    ASSERT(prFbmPoolJPG->u4Addr != 0);
    u4Addr = (UINT32)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
    u4Size = u4Pitch*rImageInfo->rDecOut.u4Height;

    hHandle=(MTIMAGE_HANDLE)arg;

    GFX_Lock();
#ifdef PNG_DECODING_USING_RZ_IOMMU
	GFX_MMU_Set_Enable(TRUE); 
	GFX_SetDst((rImageInfo->rDecOut.pu1Addr), u4Colorfmt,u4Pitch);
#else
    GFX_MMU_Set_Enable(FALSE); 
	GFX_SetDst((UINT8*)u4Addr, u4Colorfmt,u4Pitch);
#endif
    GFX_SetColCnvSrc((rImageInfo->rDecOut.pu1Addr),u4YSrcLen,(rImageInfo->rDecOut.pu1Addr+rImageInfo->rDecOut.u4CbCrOffset),u4YSrcLen,0);
    GFX_SetColCnvFmt(E_YCFMT_422LINEAR,E_SWAP_0,E_VSTD_BT709,E_VSYS_COMP);
    GFX_SetAlpha((UINT32)0xff);
    GFX_ColConv(0,0,rImageInfo->rDecOut.u4Width,rImageInfo->rDecOut.u4Height);
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();

    //copy_to_user( (void __user *)rImageInfo->u4TgtUsrAddr ,VIRTUAL(u4Addr),u4Size);
}

extern void GFX_SetCharCM(BOOL flag,UINT32 u4SrcCM);
static void _MIMAGE_Alpha_Compositon(UINT32 u4SrcAddr,UINT32 u4DstAddr, UINT32 u4Pitch,UINT32 u4Width, UINT32 u4Height)
{
    GFX_Lock();
#ifdef PNG_DECODING_USING_RZ_IOMMU
    GFX_MMU_Set_Enable(TRUE);
#else
    GFX_MMU_Set_Enable(FALSE);    
#endif
    GFX_SetSrc((UINT8 *)u4SrcAddr, CM_ARGB8888_DIRECT32, u4Pitch);
    GFX_SetDst((UINT8 *)u4DstAddr, CM_ARGB8888_DIRECT32, u4Pitch);
    
    //GFX_SetWriteArgbOrder(G_ORDER, R_ORDER, A_ORDER, B_ORDER);
    GFX_SetAlpha(0xff);     
    GFX_SetCharCM(TRUE,E_BMP_CM_8BIT);
    GFX_SetCharCM(FALSE,E_BMP_CM_8BIT);
    
    
    GFX_SetPremult(0, 0, 1, 1);
    GFX_GeneralAlphaCom(u4Width, u4Height,0xff, 
        4, 0, 0, 1,FALSE);  
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();
}

static VOID _MTIMAGE_ClrConvert(UINT32 u4SrcAddr,UINT32 u4DstAddr, UINT32 u4Pitch, UINT32 u4Height,GFX_COLOR_MODE_T fromClrMode,GFX_COLOR_MODE_T toClrMode)
{
    GFX_Lock();
    #ifdef PNG_DECODING_USING_RZ_IOMMU
    GFX_MMU_Set_Enable(TRUE);
    #else
    GFX_MMU_Set_Enable(FALSE);
    #endif
    GFX_SetSrc((UINT8 *)u4SrcAddr, (UINT32)fromClrMode, u4Pitch);
    GFX_SetDst((UINT8 *)u4DstAddr, (UINT32)toClrMode, u4Pitch/2);
    GFX_SetAlpha((UINT32)0xff);
    GFX_BitBlt(0, 0, 0, 0, u4Pitch/4, u4Height);
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();
}
static MT_RESULT_T _MTIMAGE_GetImg(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    FBM_POOL_T *prFbmPool=NULL;
    #ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT
    FBM_POOL_T *prFbmPool2=NULL;
    #endif
    UINT32 u4YAddr, u4CAddr,u4ARGBAddr, osdMode, which_resizer;
    UCHAR ucImgId;
    BOOL fgDoDither,fgPNG3M;
    MTIMAGE_INFO_T * rImageInfo ;    

    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);
    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId = (UCHAR)rArg.ai4Arg[1];
    osdMode = (UINT32)rArg.ai4Arg[2];
    fgDoDither = (BOOL)rArg.ai4Arg[3];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

u4MtimageDBGInsert(_mtimageDebugStep, 81+ucImgId);

    which_resizer = (IMGRZ_NUM - 1);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return MTR_NOT_OK;
    }

    if((rImageInfo->u4FbmPNGSize == 0x300000)&&(rImageInfo->imgFormat == IMAGE_PNG))
    {
        fgPNG3M = 1;
    }
    else 
    {
        fgPNG3M = 0;
    }
//    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_GetImg\n");
#ifdef MTIMAGE_ANDROID_SUPPORT
    if (rImageInfo->fgLibjpeg)//jpglib format.
#else
    if (rImageInfo->eImgDispCM == AYCBCR8888_DIRECT32)//jpglib format.
#endif
    {
        UINT32 u4YSrcLen=0;
        UINT32 u4ARGBSize = 0;
        u4YSrcLen=(rImageInfo->rDecOut.u4Width + 0xF) & (~0xF);
        u4ARGBSize = (UINT32)(u4YSrcLen*(rImageInfo->rDecOut.u4Height)*4);

        if(rImageInfo->imgFormat == IMAGE_PNG)
        {
            if(fgPNG3M)
            {
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
            }
            else
            {
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
            }
        }
        else
        {
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
        }

        if((prFbmPool == NULL)||(prFbmPool->u4Addr == 0))
        {
            MTIMAGE_KERNEL_DEBUG("prFbmPool= NULL \n");     
            ASSERT(prFbmPool != NULL);              
            return MTR_NOT_OK;
        }
        
        if(fgPNG3M)
        {
            u4ARGBAddr = prFbmPool->u4Addr + prFbmPool->u4Size/2;
            u4ARGBAddr = (UINT32)PAGE_ALIGN(u4ARGBAddr);
        }
        else
        {
        u4ARGBAddr = (UINT32)PAGE_ALIGN(prFbmPool->u4Addr);
        }
        /*to 565*/
        if(1 == osdMode)
        {
#ifdef PNG_DECODING_USING_RZ_IOMMU
            _MTIMAGE_ClrConvert((UINT32)rImageInfo->rDecOut.pu1Addr, (UINT32)rImageInfo->rDecOut.pu1Addr, u4YSrcLen*4, rImageInfo->rDecOut.u4Height,CM_ARGB8888_DIRECT32,CM_RGB565_DIRECT16);
#else
            if(fgPNG3M)
            {
                _MTIMAGE_ClrConvert((UINT32)rImageInfo->rDecOut.pu1Addr, u4ARGBAddr, u4YSrcLen*4, rImageInfo->rDecOut.u4Height,CM_ARGB8888_DIRECT32,CM_RGB565_DIRECT16);
            }
            else
            {
                IMGRZ_OSD_CLR_MODE_CONV(which_resizer, (UINT32)rImageInfo->rDecOut.pu1Addr, u4ARGBAddr,u4YSrcLen*4, rImageInfo->rDecOut.u4Height,
                    OSD_CLR_MODE_CONV_OSD_8888, OSD_CLR_MODE_CONV_OSD_565, fgDoDither);
            }
#endif
        }
        else if(2 == osdMode) /*ARGB4444*/
    	{
    	#ifdef PNG_DECODING_USING_RZ_IOMMU
            _MTIMAGE_ClrConvert((UINT32)rImageInfo->rDecOut.pu1Addr, (UINT32)rImageInfo->rDecOut.pu1Addr, u4YSrcLen*4, rImageInfo->rDecOut.u4Height,CM_ARGB8888_DIRECT32,CM_ARGB4444_DIRECT16);
		#else
            _MTIMAGE_ClrConvert((UINT32)rImageInfo->rDecOut.pu1Addr, u4ARGBAddr, u4YSrcLen*4, rImageInfo->rDecOut.u4Height,CM_ARGB8888_DIRECT32,CM_ARGB4444_DIRECT16);
		#endif
    	}
	else
        {
            if (rImageInfo->eImgDispCM == ARGB8888_DIRECT32)
            {
               if(rImageInfo->imgFormat == IMAGE_PNG)
                {
                    #ifdef PNG_DECODING_USING_RZ_IOMMU
                    _MIMAGE_Alpha_Compositon((UINT32)rImageInfo->rDecOut.pu1Addr, (UINT32)rImageInfo->rDecOut.pu1Addr,u4YSrcLen<<2,rImageInfo->rDecOut.u4Width,rImageInfo->rDecOut.u4Height);
                    IMGRZ_DMA_Ex(which_resizer, (UINT32)rImageInfo->rDecOut.pu1Addr, (UINT32)rImageInfo->rDecOut.pu1Addr,u4YSrcLen<<2,rImageInfo->rDecOut.u4Height, HalGetMMUTableAddress());
                    #else
                    _MIMAGE_Alpha_Compositon((UINT32)rImageInfo->rDecOut.pu1Addr,u4ARGBAddr,u4YSrcLen<<2,rImageInfo->rDecOut.u4Width,rImageInfo->rDecOut.u4Height);
                    IMGRZ_DMA_Ex(which_resizer,u4ARGBAddr,u4ARGBAddr,u4YSrcLen<<2,rImageInfo->rDecOut.u4Height, 0);
                    #endif
               }
               else
                {
                    #ifdef JPG_DECODING_USING_RZ_IOMMU
                    //do nothing
                    #else
                    IMGRZ_DMA_Ex(which_resizer, (UINT32)rImageInfo->rDecOut.pu1Addr,u4ARGBAddr,u4YSrcLen<<2,rImageInfo->rDecOut.u4Height, NULL);
                    #endif
                    //x_memcpy((void*)VIRTUAL((UINT32)u4ARGBAddr),(void*)VIRTUAL((UINT32)rImageInfo->rDecOut.pu1Addr),u4ARGBSize);
                    //HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)u4ARGBAddr), u4ARGBSize);
                }
            }
            else if (rImageInfo->eImgDispCM == Y_CBCR422_SEP16)
            {
                //do nothing
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX2);
                u4ARGBAddr = (UINT32)PAGE_ALIGN(prFbmPool->u4Addr);
            }
            else
            {
                _MTIMAGE_IMGYUV2RGB(ucImgId,(unsigned long)hHandle);
            }
        }

        rArg.ai4Arg[1] = (INT32)PAGE_ALIGN(u4ARGBAddr - prFbmPool->u4Addr);
        rArg.ai4Arg[2] = rImageInfo->rDecOut.u4CbCrOffset; //size
        rArg.ai4Arg[3] = (INT32)rImageInfo->rDecOut.u4StartX;
        rArg.ai4Arg[4] = (INT32)rImageInfo->rDecOut.u4StartY;
        rArg.ai4Arg[5] = (INT32)rImageInfo->rDecOut.u4Width;
        rArg.ai4Arg[6] = (INT32)rImageInfo->rDecOut.u4Height;

        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_8ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
    }
    else
    {
#ifdef MTIMAGE_MJPEG_CAMERA_SUPPORT
        if (rImageInfo->eImgDispCM == Y_CBCR420_SEP16 || rImageInfo->eImgDispCM == Y_CBCR422_SEP16)
        {
            MTIMAGE_DEBUG(" _MTIMAGE_GetImg, DispCM = %d\n", rImageInfo->eImgDispCM);       
            if(rImageInfo->imgFormat == IMAGE_JPEG)
            {
                #ifdef CC_FBM_3D_OSD
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
                #else
                prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
                #endif
                
                prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG); 
            }           

            if((prFbmPool == NULL)||(prFbmPool2 == NULL))
            {
                MTIMAGE_KERNEL_DEBUG("prFbmPool= NULL \n");     
                ASSERT(prFbmPool != NULL);          
                return MTR_NOT_OK;
            }

            //x_memcpy((void *)VIRTUAL((UINT32)prFbmPool2->u4Addr), (void *)VIRTUAL((UINT32)prFbmPool->u4Addr), rImageInfo->rDecOut.u4Pitch*rImageInfo->rDecOut.u4Height*2);
            //JPG_VDP/OSD1 memory is not mapped, not support VIRTUAL  memcpy, use hw to copy
            GFX_Lock();
            GFX_MMU_Set_Enable(FALSE);
            GFX_SetSrc((UINT8 *)(prFbmPool->u4Addr), (UINT32)CM_RGB_CLUT8, rImageInfo->rDecOut.u4Pitch);
            GFX_SetDst((UINT8 *)(prFbmPool2->u4Addr), (UINT32)CM_RGB_CLUT8, rImageInfo->rDecOut.u4Pitch);
            GFX_BitBlt(0, 0, 0, 0, rImageInfo->rDecOut.u4Pitch, rImageInfo->rDecOut.u4Height*2); //YCbCR 1.5
            GFX_Flush();
            GFX_Wait();
            GFX_Unlock();

            rArg.ai4Arg[1] = (INT32)0;
            rArg.ai4Arg[2] = rImageInfo->rDecOut.u4CbCrOffset; //size
            rArg.ai4Arg[3] = (INT32)rImageInfo->rDecOut.u4StartX;
            rArg.ai4Arg[4] = (INT32)rImageInfo->rDecOut.u4StartY;
            rArg.ai4Arg[5] = (INT32)rImageInfo->rDecOut.u4Width;
            rArg.ai4Arg[6] = (INT32)rImageInfo->rDecOut.u4Height;
            
        }
        else
#endif            
        {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);

        if((prFbmPool == NULL)||(prFbmPool->u4Addr == 0))
        {
            MTIMAGE_KERNEL_DEBUG("prFbmPool= NULL \n");     
            ASSERT(prFbmPool != NULL);          
            return MTR_NOT_OK;
        }
        
        FBM_GetFrameBufferAddr(_rImageFbg.aucFbgId[_rImageFbg.u1CurView],
                               _rImageFbg.aucCurImgFbId[_rImageFbg.u1CurView], &u4YAddr, &u4CAddr);

        rArg.ai4Arg[1] = u4YAddr - prFbmPool->u4Addr; //addr offset
        rArg.ai4Arg[2] = u4CAddr - u4YAddr; //size
        rArg.ai4Arg[3] = (INT32)rImageInfo->rDecOut.u4StartX;
        rArg.ai4Arg[4] = (INT32)rImageInfo->rDecOut.u4StartY;
        rArg.ai4Arg[5] = (INT32)rImageInfo->rDecOut.u4Width;
        rArg.ai4Arg[6] = (INT32)rImageInfo->rDecOut.u4Height;
        }
        MTIMAGE_KERNEL_DEBUG(" - GetImg YOffset 0x%c, Sz %x \n", rArg.ai4Arg[1], rArg.ai4Arg[2]);
        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_8ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
    }

u4MtimageDBGInsert(_mtimageDebugStep, 89);   

    return MTR_OK;
}


/**Get a/v meta data
*@param handle
*@param prMeta
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_GetMetaData(unsigned long arg)
{
    MT_RESULT_T eResult = MTR_OK;
#if 0
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    MTIMAGE_AVMETA_T *prMeta=NULL;
    MTIMAGE_AVMETA_T   rMeta;
    PLAYMGR_AVMETA_T rPlaymgrMeta;
    MTIMAGE_INSTANCE_T eInstance;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prMeta = (MTIMAGE_STATUS_T *)rArg.ai4Arg[1];

//add driver implementation code here
    eInstance = _MTIMAGE_GetInstance(hHandle);

    if ((eInstance == MTIMAGE_PLAYMGR) || (eInstance == MTIMAGE_AUDIO))
    {
        if (prMeta == NULL)
        {
            MTIMAGE_KERNEL_DEBUG(" - meta data Error \n");
            return MTR_NOT_OK;
        }
        x_memset((void *)&rPlaymgrMeta, 0, sizeof(PLAYMGR_AVMETA_T));
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_GetMetaData(MTIMAGE_PLAYMGR0, &rPlaymgrMeta);
#else
        PLAYMGR_GetMetaData(&rPlaymgrMeta);
#endif
        x_memcpy((void *)rMeta.au2Title, (void *)rPlaymgrMeta.pu2Title, rPlaymgrMeta.u4TitleLen);
        x_memcpy((void *)rMeta.au2Album, (void *)rPlaymgrMeta.pu2Album, rPlaymgrMeta.u4AlbumLen);
        x_memcpy((void *)rMeta.au2Artist, (void *)rPlaymgrMeta.pu2Artist, rPlaymgrMeta.u4ArtistLen);
        x_memcpy((void *)rMeta.au2Genre, (void *)rPlaymgrMeta.pu2Genre, rPlaymgrMeta.u4GenreLen);
        rMeta.u4SampleFreq = rPlaymgrMeta.u4SampleFreq;
        rMeta.u4BitRate = rPlaymgrMeta.u4BitRate;
        rMeta.u4FrameSize = rPlaymgrMeta.u4FrameSize;
        rMeta.u4FrameRate = rPlaymgrMeta.u4FrameRate;
        rMeta.u4PicPos = rPlaymgrMeta.u4ImgPos;
        rMeta.u4PicSize = rPlaymgrMeta.u4ImgSize;
        rMeta.u4Width = rPlaymgrMeta.u4Width;
        rMeta.u4Height = rPlaymgrMeta.u4Height;
        rMeta.u4ParWidth = rPlaymgrMeta.u4ParWidth;
        rMeta.u4ParHeight = rPlaymgrMeta.u4ParHeight;
        rMeta.u8MediaSize = rPlaymgrMeta.u8MediaSize;
        rMeta.u8PlayTime = rPlaymgrMeta.u8PlayTime;
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG(" - Instance Error \n");
        return MTR_NOT_OK;
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(prMeta,MTIMAGE_AVMETA_T);
    COPY_TO_USER_ARG(prMeta,rMeta,MTIMAGE_AVMETA_T);

    MTIMAGE_KERNEL_DEBUG(" - void\n");
    //MTIMAGE_STATUS_PRINT("rStatus",&rStatus);
#endif
    return eResult;
}


/**Start slide show
*@param slide type
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_SlideShow(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId =  (UCHAR)rArg.ai4Arg[1];

u4MtimageDBGInsert(_mtimageDebugStep, 91+ucImgId);

    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return MTR_NOT_OK;
    }
    rImageInfo->u1SlideShowType = (UINT8)rArg.ai4Arg[2];

#if 0
    eInstance = _MTIMAGE_GetInstance(hHandle);
    if (eInstance != MTIMAGE_IMAGE)
    {
        MTIMAGE_KERNEL_DEBUG(" - Instance Error \n");
        return MTR_NOT_OK;
    }

    if (IMG_GetVideoPlaneInfo(&rVpInfo) != E_IMG_OK)
    {
        return MTR_NOT_OK;
    }
#if 0
    if (!_fgVdpConnect)
    {
        if (IMG_VDP_Connect(_u4PlaneId) != E_IMG_OK)
        {
            return FALSE;
        }
        else
        {
            _fgVdpConnect = TRUE;
            IMG_Flip();
            IMG_Flip();
        }
    }
#endif

    switch (eSlideType)
    {
    case MT_MPLAYER_EFFECT_BARWIPE_LR:
        MTIMAGE_BarWipeH(rVpInfo, TRUE);
        break;
    case MT_MPLAYER_EFFECT_BARWIPE_RL:
        MTIMAGE_BarWipeH(rVpInfo, FALSE);
        break;
    case MT_MPLAYER_EFFECT_BARWIPE_TB:
        MTIMAGE_BarWipeV(rVpInfo, TRUE);
        break;
    case MT_MPLAYER_EFFECT_BARWIPE_BT:
        MTIMAGE_BarWipeV(rVpInfo, FALSE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_TL:
        MTIMAGE_BoxWipe(rVpInfo, TRUE, TRUE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_TR:
        MTIMAGE_BoxWipe(rVpInfo, TRUE, FALSE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_BL:
        MTIMAGE_BoxWipe(rVpInfo, FALSE, TRUE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_BR:
        MTIMAGE_BoxWipe(rVpInfo, FALSE, FALSE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_HC:
        MTIMAGE_DoorWipeH(rVpInfo, TRUE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_HO:
        MTIMAGE_DoorWipeH(rVpInfo, FALSE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_VC:
        MTIMAGE_DoorWipeV(rVpInfo, TRUE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_VO:
        MTIMAGE_DoorWipeV(rVpInfo, FALSE);
        break;
    case MT_MPLAYER_EFFECT_IRISWIPE:
        MTIMAGE_IrisWipe(rVpInfo);
        break;
    case MT_MPLAYER_EFFECT_NONE:
        IMG_Flip();
        break;
    default:
        return MTR_NOT_OK;
    }
#endif

u4MtimageDBGInsert(_mtimageDebugStep, 99);

    return MTR_OK;
}



/**clean the img frame buffer
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_CleanFB(unsigned long arg)
{
    return MTR_OK;
}


/**Get capability.
*@param type
*@param parameter 1
*@param parameter 2
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_GetCap(unsigned long arg)
{
#if 0
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4Type;
    UINT32 u4Param1;
    UINT32 u4Param2;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u4Type = (UINT32)rArg.ai4Arg[0];
    u4Param1 = (UINT32)rArg.ai4Arg[1];
    u4Param2 = (UINT32)rArg.ai4Arg[2];

    MTIMAGE_KERNEL_DEBUG(" - u4Type = %u  u4Param1 = %u  u4Param2 = %u\n", u4Type, u4Param1, u4Param2);

#ifdef CC_53XX_SWDMX_V2
    if (!PLAYMGR_GetCap(MTIMAGE_PLAYMGR0, u4Type, u4Param1, u4Param2))
#else
    if (1/*!PLAYMGR_GetCap(u4Type, u4Param1, u4Param2)*/) // V1 does not have this API yet!!!
#endif
    {
        UNUSED(u4Type);
        UNUSED(u4Param1);
        UNUSED(u4Param2);
        MTIMAGE_KERNEL_DEBUG(" - PLAYMGR_GetCap return FALSE\n");
        return MTR_NOT_OK;
    }

    UNUSED(u4Type);
    UNUSED(u4Param1);
    UNUSED(u4Param2);
#endif
    return MTR_OK;
}


static MT_RESULT_T _MTIMAGE_SetBufFilled(unsigned long arg)
{
    MTIMAGE_HANDLE hHandle;
    static UINT32 u4FillLen1[2] = {0};
    MTAL_IOCTL_4ARG_T rArg;
    MTIMAGE_BUF_FILLED_T* prBufFillInfo;
    MTIMAGE_BUF_FILLED_T rBufFillInfo;

    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);


    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prBufFillInfo = (MTIMAGE_BUF_FILLED_T*)rArg.ai4Arg[1];
    ucImgId = (UCHAR) rArg.ai4Arg[2];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

u4MtimageDBGInsert(_mtimageDebugStep, 101+ucImgId);
    
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prBufFillInfo, MTIMAGE_BUF_FILLED_T);
    COPY_FROM_USER_ARG(prBufFillInfo, rBufFillInfo, MTIMAGE_BUF_FILLED_T);

    if (rBufFillInfo.u4_trsn_id == 1)
    {
        if (u4FillLen1[ucImgId] == 0)
        {
            IMG_SetFillBsBufEnd(ucImgId,rBufFillInfo.u4_filled_len, 0,
                                (rBufFillInfo.i4_ret==0)?FALSE:TRUE);   //IMG_MW_FILL_OK
        }
        else
        {
            IMG_SetFillBsBufEnd(ucImgId,u4FillLen1[ucImgId] , rBufFillInfo.u4_filled_len,
                                (rBufFillInfo.i4_ret==0)?FALSE:TRUE);   //IMG_MW_FILL_OK
        }
        u4FillLen1[ucImgId]  = 0;
    }
    else
    {
        u4FillLen1[ucImgId]  = rBufFillInfo.u4_filled_len;
    }
    
u4MtimageDBGInsert(_mtimageDebugStep, 109);

    return MTR_OK;
}


/**Initialize jpeg hw for decoding.
*@param eColorFMT            display color format
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_ConfigImagedecoder(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    ENUM_IMG_FORMAT imgFormat;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    imgFormat=(ENUM_IMG_FORMAT)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

u4MtimageDBGInsert(_mtimageDebugStep, 111+ucImgId);
    
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("rImageInfo= NULL \n");     
        return MTR_NOT_OK;
    }
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_ConfigImagedecoder %d\n",ucImgId);

    rImageInfo->imgFormat = imgFormat;  // set format for _MTIMAGE_Open user later

    if (IMG_Config(ucImgId, imgFormat) != E_IMG_OK)
    {
        MTIMAGE_KERNEL_DEBUG("IMG_Config NG\n");
        return MTR_NOT_OK;
    }
    
u4MtimageDBGInsert(_mtimageDebugStep, 119);

    return MTR_OK;
}



/**Get the stream settings and status
*@param handle
*@param prSettings
*@param prStatus
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_MW_GetInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_IMG_INFO_TYPE etype;
    VOID *prStatus;
    UINT32 rStatus;
    IMG_LIMIT_T *prLimit;
    IMG_LIMIT_T rLimit; 

u4MtimageDBGInsert(_mtimageDebugStep, 121);

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    etype = (DRV_GET_TYPE_T)rArg.ai4Arg[0];

    //MTIMAGE_PRINT(" - u4Type = %u\n", etype);

    if(etype == MT_IMAGE_TYPE_PROGRESS)
    {
        prStatus = (VOID *)rArg.ai4Arg[1];  
        IMG_GetProg(0, (UINT32 *)&rStatus);
        USR_SPACE_ACCESS_VALIDATE_ARG(prStatus,UINT32);
        COPY_TO_USER_ARG(prStatus,rStatus,UINT32);      
    }
    else if(etype == MT_IMAGE_TYPE_LIMITATION)
    {
        prLimit = (VOID *)rArg.ai4Arg[1];   
        rLimit.u4Type = prLimit->u4Type;
        IMG_GetCap((void *)&rLimit);
        USR_SPACE_ACCESS_VALIDATE_ARG(prLimit,IMG_LIMIT_T);
        COPY_TO_USER_ARG(prLimit,rLimit,IMG_LIMIT_T);   
    }

u4MtimageDBGInsert(_mtimageDebugStep, 129);

    return MTR_OK;
}

/**Set jpeg decode
*@param 
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_MW_SetDecode(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTIMAGE_INFO_T * rImageInfo;

    MTIMAGE_HANDLE hHandle;
    IMG_PARAM_T *prInfo;
    IMG_PARAM_T rInfo;
    UCHAR ucImgId;

u4MtimageDBGInsert(_mtimageDebugStep, 131);

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prInfo = (IMG_PARAM_T *)rArg.ai4Arg[1];
    ucImgId = (UCHAR)rArg.ai4Arg[2];

    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    x_memset((void *)&rImageInfo->rDecOut, 0, sizeof(IMG_REGION_T));    
    prInfo->prDecOut = &rImageInfo->rDecOut;
    USR_SPACE_ACCESS_VALIDATE_ARG(prInfo, IMG_PARAM_T);
    COPY_FROM_USER_ARG(prInfo, rInfo, IMG_PARAM_T);

        if (rInfo.bIsVitualOutputAddr == FALSE)
        {
        	if (rInfo.eDispColorMode == Y_CBCR422_SEP16)
        	{
        		HalInvalidateDCacheMultipleLine((UINT32)rInfo.rDstRegion.pu1Addr ,  rInfo.rDstRegion.u4CbCrOffset * 2);
        	}
                else
        	{
        		HalInvalidateDCacheMultipleLine((UINT32)rInfo.rDstRegion.pu1Addr , rInfo.rDstRegion.u4Pitch * rInfo.rDstRegion.u4Height);
        	}
            rInfo.rDstRegion.pu1Addr = (UINT8*)gfx_va_to_pa((unsigned int)rInfo.rDstRegion.pu1Addr);
        }
	MTIMAGE_KERNEL_DEBUG("====_MTIMAGE_MW_SetDecode rInfo.bIsVitualOutputAddr = %d\n",rInfo.bIsVitualOutputAddr);
	#ifdef JPG_DECODING_USING_RZ_IOMMU
        if((ucImgId==0)&&(rInfo.bIsVitualOutputAddr))
        {
            rInfo.u4JpgUsingRzIOMMU_MMUTableAddr = HalGetMMUTableAddress();
            MTIMAGE_KERNEL_DEBUG("--[_MTIMAGE_MW_SetDecode][JPEG] _MTIMAGE_MMUTableAddr = %x\n",rInfo.u4JpgUsingRzIOMMU_MMUTableAddr);
            HalInvalidateDCacheMultipleLine((UINT32)rInfo.rDstRegion.pu1Addr , rInfo.rDstRegion.u4Pitch * rInfo.rDstRegion.u4Height);
        }           
	#endif
	#ifdef PNG_DECODING_USING_RZ_IOMMU
        if((ucImgId==1)&&(rInfo.bIsVitualOutputAddr))
        {
            rInfo.u4JpgUsingRzIOMMU_MMUTableAddr = HalGetMMUTableAddress();
            MTIMAGE_KERNEL_DEBUG("--[_MTIMAGE_MW_SetDecode][PNG] _MTIMAGE_MMUTableAddr = %x\n",rInfo.u4JpgUsingRzIOMMU_MMUTableAddr);
            HalInvalidateDCacheMultipleLine((UINT32)rInfo.rDstRegion.pu1Addr , rInfo.rDstRegion.u4Pitch * rInfo.rDstRegion.u4Height);
        }           
	#endif
    
    rInfo.fgDrvFeedData = rImageInfo->fgDrvFeedData;

    if(IMG_Decode(ucImgId, rInfo) != E_IMG_OK)
    {
        MTIMAGE_KERNEL_DEBUG("IMG_Decode NG\n");
        return MTR_NOT_OK;
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("Decode output Y 0x%x, C 0x%x", (UINT32)rInfo.rDstRegion.pu1Addr, 
            (UINT32)rInfo.rDstRegion.pu1Addr + rInfo.rDstRegion.u4CbCrOffset);
    }
u4MtimageDBGInsert(_mtimageDebugStep, 139);    
    return MTR_OK;
}

void _MTIMAGE_MW_Callback_1(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{
    //IMG_FILL_BUF_MW_T* prFillBuf;
    FBM_POOL_T* prFbmPool;

u4MtimageDBGInsert(_mtimageDebugStep, 141);    

    if (JPG_HW_NFY_FILL_BUF == u4State)
    {
        MTIMAGE_FILL_BUF_T rFillBuf;

        rFillBuf = *(MTIMAGE_FILL_BUF_T *)pv_data;
        
        //overmtal, kernel to user
        //rFillBuf.pv_start_add = adapt_fbm_knl_to_usr(rFillBuf.pv_start_add);
        
        _rMTImage_set_nfy_parm.ui4_img_id = u4ImgID;
        _rMTImage_set_nfy_parm.pv_tag = pv_tag;
        //_rMTImage_set_nfy_parm.t_attach_buf = rFillBuf;
        _rMTImage_set_nfy_parm.ui4_trsn_id = rFillBuf.ui4_trsn_id;
        _rMTImage_set_nfy_parm.pv_start_add = rFillBuf.pv_start_add;
        _rMTImage_set_nfy_parm.ui4_required_len = rFillBuf.ui4_required_len;
        _rMTImage_set_nfy_parm.b_reset_pos = rFillBuf.b_reset_pos;
        _rMTImage_set_nfy_parm.ui4_position = rFillBuf.ui4_position;

        _rMTImage_set_nfy_parm.e_state = u4State;

        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        if(prFbmPool)
        {
            _rMTImage_set_nfy_parm.u4JpgInputBufBase = VIRTUAL(prFbmPool->u4Addr);
        }
        _CB_PutEvent(CB_MTIMAGE_TRIGGER_1, sizeof(MTIMAGE_SET_NFY_PARM), &_rMTImage_set_nfy_parm);
    }
    else
    {
        _rMTImage_set_nfy_parm.ui4_img_id = u4ImgID;
        _rMTImage_set_nfy_parm.pv_tag = pv_tag;
        _rMTImage_set_nfy_parm.pv_data = pv_data;
        _rMTImage_set_nfy_parm.e_state = u4State;

        _CB_PutEvent(CB_MTIMAGE_TRIGGER_1, sizeof(MTIMAGE_SET_NFY_PARM), &_rMTImage_set_nfy_parm);
    }
u4MtimageDBGInsert(_mtimageDebugStep, 149);        
}


void _MTIMAGE_MW_Callback_0(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{
    //IMG_FILL_BUF_MW_T* prFillBuf;
    FBM_POOL_T* prFbmPool;

u4MtimageDBGInsert(_mtimageDebugStep, 151);    

    if (JPG_HW_NFY_FILL_BUF == u4State)
    {
        MTIMAGE_FILL_BUF_T rFillBuf;

        rFillBuf = *(MTIMAGE_FILL_BUF_T *)pv_data;
        
        //overmtal, kernel to user
        //rFillBuf.pv_start_add = adapt_fbm_knl_to_usr(rFillBuf.pv_start_add);
        
        _rMTImage_set_nfy_parm.ui4_img_id = u4ImgID;
        _rMTImage_set_nfy_parm.pv_tag = pv_tag;
        //_rMTImage_set_nfy_parm.t_attach_buf = rFillBuf;
        _rMTImage_set_nfy_parm.ui4_trsn_id = rFillBuf.ui4_trsn_id;
        _rMTImage_set_nfy_parm.pv_start_add = rFillBuf.pv_start_add;
        _rMTImage_set_nfy_parm.ui4_required_len = rFillBuf.ui4_required_len;
        _rMTImage_set_nfy_parm.b_reset_pos = rFillBuf.b_reset_pos;
        _rMTImage_set_nfy_parm.ui4_position = rFillBuf.ui4_position;

        _rMTImage_set_nfy_parm.e_state = u4State;
        
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
        if(prFbmPool)
        {
            _rMTImage_set_nfy_parm.u4JpgInputBufBase = VIRTUAL(prFbmPool->u4Addr);
        }
        _CB_PutEvent(CB_MTIMAGE_TRIGGER, sizeof(MTIMAGE_SET_NFY_PARM), &_rMTImage_set_nfy_parm);
    }
    else
    {
        _rMTImage_set_nfy_parm.ui4_img_id = u4ImgID;
        _rMTImage_set_nfy_parm.pv_tag = pv_tag;
        _rMTImage_set_nfy_parm.pv_data = pv_data;
        _rMTImage_set_nfy_parm.e_state = u4State;

        _CB_PutEvent(CB_MTIMAGE_TRIGGER, sizeof(MTIMAGE_SET_NFY_PARM), &_rMTImage_set_nfy_parm);
    }

u4MtimageDBGInsert(_mtimageDebugStep, 159);    

}

/**Initialize jpeg hw for initiation.
*@param eColorFMT            display color format
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTIMAGE_MW_SetInit(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    //MTIMAGE_HANDLE hHandle;
    UCHAR ucImgId;    
    MTIMAGE_FRM_START_T *prInitInfo;
	MTIMAGE_FRM_START_T rInitInfo;
    UINT32 u4CurState;
    MTIMAGE_IMG_FORMAT  eFormat;
    //INT32 i4Ret;

u4MtimageDBGInsert(_mtimageDebugStep, 161);    
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    ucImgId=(UCHAR)rArg.ai4Arg[0];
    prInitInfo = (MTIMAGE_FRM_START_T *)rArg.ai4Arg[1];
    eFormat = rArg.ai4Arg[2];

	USR_SPACE_ACCESS_VALIDATE_ARG(prInitInfo, MTIMAGE_FRM_START_T);
    COPY_FROM_USER_ARG(prInitInfo, rInitInfo, MTIMAGE_FRM_START_T);
    
    u4CurState = IMG_GetDecState(ucImgId);
    if(u4CurState == IMG_RELEASE)
    {
        _rMTImage_set_nfy_parm.pf_nfy = rInitInfo.pf_func;

        if(1 == ucImgId)        
        {
            if(IMG_Init_Ex(RING_IN_SINGLE_OUT, rInitInfo.pv_tag, rInitInfo.ui4_img_id, ucImgId, _MTIMAGE_MW_Callback_1) != E_IMG_OK)
            {
                MTIMAGE_KERNEL_DEBUG("IMG_Init_Ex fail\n");
                return MTR_NOT_OK;
            }
        }
        else
        {
            if (eFormat == MT_IMAGE_WEBP_NEW)
            {
                if(IMG_Init_Ex(RING_IN_SINGLE_OUT, rInitInfo.pv_tag, rInitInfo.ui4_img_id, ucImgId, _MTIMAGE_MW_Callback_1) != E_IMG_OK)
                {
                    MTIMAGE_KERNEL_DEBUG("IMG_Init_Ex fail\n");
                    return MTR_NOT_OK;
                }
            }
            else
            {
                if(IMG_Init_Ex(RING_IN_SINGLE_OUT, rInitInfo.pv_tag, rInitInfo.ui4_img_id, ucImgId, _MTIMAGE_MW_Callback_0) != E_IMG_OK)
                {
                    MTIMAGE_KERNEL_DEBUG("IMG_Init_Ex fail\n");
                    return MTR_NOT_OK;
                }
            }
        }
        if(IMG_SetUseMw(ucImgId, TRUE)!=E_IMG_OK)
        {
            MTIMAGE_KERNEL_DEBUG("IMG_SetUseMw fail\n");
            return MTR_NOT_OK;
        }

        IMG_Set_ImgSize(ucImgId, rInitInfo.ui4_img_size);        
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Open only available when IMG_RELEASE, Cur %d\n", u4CurState);
        return MTR_NOT_OK;
    }

u4MtimageDBGInsert(_mtimageDebugStep, 169);    

    return MTR_OK;
}

/**connect to image module
*@param connect or disconnect
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_MW_Connect(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTIMAGE_INFO_T * rImageInfo ;
    UCHAR  ucImgId;

    u4MtimageDBGInsert(_mtimageDebugStep, 171); 
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    ucImgId = (UCHAR)rArg.ai4Arg[1];
    
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    rImageInfo->fgDrvFeedData = 1;
    if(IMG_Lib_Init()!= E_IMG_OK)
    {
        return MTR_NOT_OK;
    }
u4MtimageDBGInsert(_mtimageDebugStep, 179);     
    return MTR_OK;
}

static MT_RESULT_T _MTIMAGE_GetPNGBufferSize(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTIMAGE_INFO_T * rImageInfo;
    FBM_POOL_T* prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);    
    
u4MtimageDBGInsert(_mtimageDebugStep, 182);
    
    rImageInfo = _MTIMAGE_GetImgIdInfo(1);
    if(prFbmPool && prFbmPool->u4Addr && prFbmPool->u4Size)
    {
        rArg.ai4Arg[0] = (INT32)prFbmPool->u4Size;
        rImageInfo->u4FbmPNGSize = rArg.ai4Arg[0];
    }
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_4ARG_T);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_4ARG_T); 

u4MtimageDBGInsert(_mtimageDebugStep, 189);
    return MTR_OK;
}


static MT_RESULT_T _MTIMAGE_HWResouceCtrl(UCHAR ucImgId, UCHAR action)
{
    MTIMAGE_INFO_T * rImageInfo = NULL;
    static char *action_str[] = {"lock", "unlock", "init"};
    
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

u4MtimageDBGInsert(_mtimageDebugStep, 191+ucImgId);

    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: ucImgId=%d, action=%s, sema=0x%x\n", ucImgId, action_str[action], rImageInfo->hHwResouceSema);
    /*Unlock*/
    if(HW_RESOURCE_CTRL_UNLOCK == action)
    {
        if(x_sema_unlock(rImageInfo->hHwResouceSema)== OSR_OK)
        {
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: un_lock OK!!\n");
            u4MtimageDBGInsert(_mtimageDebugStep, 194);
            return MTR_OK;
        }
    }
    /*lock*/
    else if(HW_RESOURCE_CTRL_LOCK == action)
    {
        if(x_sema_lock(rImageInfo->hHwResouceSema, X_SEMA_OPTION_WAIT)== OSR_OK)
        {
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: lock OK!!\n");
            u4MtimageDBGInsert(_mtimageDebugStep, 195);            
            return MTR_OK;
        }
    }
    /*Init*/
    else if(HW_RESOURCE_CTRL_INIT == action)
    {
        if (rImageInfo->hHwResouceSema == (HANDLE_T)NULL)
        {
            if(x_sema_create(&rImageInfo->hHwResouceSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK)
            {
                MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: create OK 0x%x!!\n", rImageInfo->hHwResouceSema);
                u4MtimageDBGInsert(_mtimageDebugStep, 196);                
                return MTR_OK;
            }
        }
        else
        {
            u4MtimageDBGInsert(_mtimageDebugStep, 197);         
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: already inited!!\n");
            return MTR_OK;
        }
    }
    else
    {
        u4MtimageDBGInsert(_mtimageDebugStep, 198);     
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl unknow action: ucImgId=%d, action=%d, sema=0x%x\n", ucImgId, action, rImageInfo->hHwResouceSema);
        return MTR_NOT_OK;
    }    
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl return ERROR: ucImgId=%d, action=%s, sema=0x%x\n", ucImgId, action_str[action], rImageInfo->hHwResouceSema);
    u4MtimageDBGInsert(_mtimageDebugStep, 199);    
    return MTR_NOT_OK;
}

static MT_RESULT_T _MTIMAGE_HWResouceCtrl_Wrapper(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UCHAR ucImgId = 0;
    UCHAR action = 0;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    ucImgId = (UCHAR)rArg.ai4Arg[0];
    action    = (UCHAR)rArg.ai4Arg[1];

    return _MTIMAGE_HWResouceCtrl(ucImgId, action);
}

/*
MT_RESULT_T _MTIMAGE_InitHWResouce(void)
{
    MT_RESULT_T ret = MTR_OK;
    if(MTR_OK != (ret = _MTIMAGE_HWResouceCtrl(0, HW_RESOURCE_CTRL_INIT)))
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_InitHWResouce 0 return %d\n", ret);
        return ret;
    }
    if(MTR_OK != (ret = _MTIMAGE_HWResouceCtrl(1, HW_RESOURCE_CTRL_INIT)))
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_InitHWResouce 1 return %d\n", ret);
        return ret;
    }
    return MTR_OK;
}
*/
int mtal_ioctl_mtimage(struct inode *inode, struct file *file, unsigned int cmd,
                       unsigned long arg)
{
    switch (cmd)
    {
    case MTAL_IO_IMAGE_CONNECT:
        return  _MTIMAGE_Connect(arg);
    case MTAL_IO_IMAGE_OPEN:
        return  _MTIMAGE_Open(arg);
    case MTAL_IO_IMAGE_DECODE:
        return  _MTIMAGE_Decode(arg);
    case MTAL_IO_IMAGE_DISPLAY:
        return  _MTIMAGE_Display(arg);
    case MTAL_IO_IMAGE_CLOSE:
        return  _MTIMAGE_Close(arg);
    case MTAL_IO_IMAGE_STOP:
        return  _MTIMAGE_Stop(arg);
    case MTAL_IO_IMAGE_SET:
        return  _MTIMAGE_Set(arg);
    case MTAL_IO_IMAGE_GETINFO:
        return  _MTIMAGE_GetInfo(arg);
    case MTAL_IO_IMAGE_GETIMG:
        return  _MTIMAGE_GetImg(arg);
    case MTAL_IO_IMAGE_GETMETA:
        return  _MTIMAGE_GetMetaData(arg);
    case MTAL_IO_IMAGE_SLIDESHOW:
        return  _MTIMAGE_SlideShow(arg);
    case MTAL_IO_IMAGE_CLEANFB:
        return _MTIMAGE_CleanFB(arg);
    case MTAL_IO_IMAGE_GETCAP:
        return _MTIMAGE_GetCap(arg);
    case MTAL_IO_IMAGE_FILLBUF:
        return _MTIMAGE_SetBufFilled(arg);
    case MTAL_IO_IMAGE_CONFIGIMGDECODER:
        return _MTIMAGE_ConfigImagedecoder(arg);
#ifdef  PNG_GETHDR_SUPPORT
    case MTAL_IO_IMAGE_GETPNGHDR:
        return _MTIMAGE_GetPngHdr(arg);        
#endif  /*PNG_GETHDR_SUPPORT*/      
    case MTAL_IO_IMAGE_SET_DECODE:
        return  _MTIMAGE_MW_SetDecode(arg);                    
    case MTAL_IO_IMAGE_SET_INIT:
        return  _MTIMAGE_MW_SetInit(arg);                      
    case MTAL_IO_IMAGE_MW_CONNECT:
        return  _MTIMAGE_MW_Connect(arg);  
    case MTAL_IO_IMAGE_MW_GETINFO:
        return  _MTIMAGE_MW_GetInfo(arg);          
    case MTAL_IO_IMAGE_QRY_PNG_BUFFER_SIZE:
        return _MTIMAGE_GetPNGBufferSize(arg);
    case MTAL_IO_IMAGE_RESOUCE_CTL:
        return _MTIMAGE_HWResouceCtrl_Wrapper(arg); 
	case MTAL_IO_IMAGE_GET_IMGRZ_NUM:
		return _MTIMAGE_GetImgrzNum(arg);
        case MTAL_IO_IMAGE_GET_DEFDECADDR:
                return _MTIMAGE_GetDefaultDecAddr(arg);
    default:
        return  MTR_NOT_OK;
    }
    return MTR_OK;
}

