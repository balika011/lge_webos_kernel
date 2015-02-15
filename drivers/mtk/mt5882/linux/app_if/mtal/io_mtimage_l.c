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
#include "osd_if.h"
#include "gfx_drvif.h"
#include "srm_drvif.h"
#include "drv_tdtv_drvif.h"
#include <linux/mtal_ioctl.h>
#include "x_img_slideshow.h"
#include "vdp_if.h"
#include "drv_tdtv_drvif.h"

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
//pitch transformation for AP, e.g not-16 pixels alignment.
#define MT_THUMBNAIL_WORKING_BUFFER_OFFSET	0x40000     
//for thumbnail decoding output buffer, 1800KB at the buffer and, 900KB for Y, 900KB for CbCr, max 1280x720
#define MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET   (900*1024)
#define MT_THUMBNAIL_VDO_DECODING_BUFFER_OFFSET	(0xa00000-MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET*2)     

#define MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET (0xa00000-MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET*4)
#define MT_IMAGE_EMP_TIMEOUT	5000
#define MT_IMAGE_GPU_TIMEOUT	200

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
    UINT8 u1CurView;
    UINT8 u1SlideShowType;
    UINT8 u1SlideShowEvent;
    UINT32 u4SlideShowTime;
    HANDLE_T hSlideShowSema;
    HANDLE_T hHwResouceSema;    
    ENUM_IMG_FORMAT	imgFormat;
    ENUM_IMG_ROTATE_PHASE eRotateDegree;
    ENUM_IMG_DISP_COLOR_MODE eImgDispCM;
    IMG_REGION_T rDecOut;
    IMG_REGION_T rDecOut2;
    IMG_CALLBACK_INFO_T rCallbackInfo;
    UCHAR aucFbgId[MAX_VIEW_NUM];
    UCHAR aucCurImgFbId[MAX_VIEW_NUM];
    UCHAR aucPrevImgFbId[MAX_VIEW_NUM];
    UCHAR aucCurDispFbId[MAX_VIEW_NUM];
    E_TDTV_DECODER_INPUT eDetectedMode;
    UINT32 u4PlaneId;
    UINT32 u4DisplayRegionId;
    UINT16 u2ThumbnailSetDecWidth;
    UINT16 u2ThumbnailSetDecHeight;    
    UINT32 u4SrcYaddr;
    UINT32 u4SrcCaddr;
    UINT32 u4CurYaddr;
    UINT32 u4CurCaddr;    
    UINT32 u4DesYaddr;
    UINT32 u4DesCaddr;    
    UINT32 u4PanelWidth;
    UINT32 u4PanelHeight;   
    UINT32 u4ThumbnailType;  
    BOOL fgFBGCreated;
    UINT8 decoder_id;
    UINT8 video_id;    
    UINT32  u4UserCallerPid;    /*For multiple process debug*/
    BOOL fgDrvFeedData;
} MTIMAGE_INFO_T;


/********************************************************************
  VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/
static MTIMAGE_INFO_T _rImageInfo[2] = {{0}, {0}};
static MTVDO_REGION_T _rMt3DCropinfo = {0};
static bool fgFBGStructInit = 0;
static bool fgNewFlushMode = 0;
static MTVDO_REGION_T _rCurImgRegion = {0};
static HANDLE_T hMtinageDbgInsertSema = 0; 
UINT32 _mtimageDebugStep = 1;
UINT32 u4MtimageDBGCnt = 0;
UINT32 u4MtimageDBG[256][2] = {{0}, {0}};

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
            MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");     \
            return MTR_ERR_INV;                                          \
        }                                                       \
 
#define COPY_FROM_USER_12ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(MTAL_IOCTL_12ARG_T)))          \
        {                                                       \
            MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mtimage argument error\n");      \
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
static void _MTIMAGE_vdp_stretchblit(SLIDESHOW_FADE_T t_fade);
static void _MTIMAGE_Thumbnail_rotation(UCHAR ucImgId, ENUM_IMG_ROTATE_PHASE eRotateDegree_thumbnail);

void u4MtimageDBGInsert(UINT32 type, UINT32 value)
{	
	if(hMtinageDbgInsertSema == 0)
	{
		MTIMAGE_LOG(TRUE, "MTIMAGE", "\n_MTIMAGE create hMtinageDbgInsertSema............\n");	
		VERIFY(x_sema_create(&hMtinageDbgInsertSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);		
	}

	VERIFY (x_sema_lock(hMtinageDbgInsertSema, X_SEMA_OPTION_WAIT) == OSR_OK);
    u4MtimageDBG[u4MtimageDBGCnt][0] = type;
    u4MtimageDBG[u4MtimageDBGCnt][1] = value;  
    u4MtimageDBGCnt++;
    if(u4MtimageDBGCnt >= 256)
    {
        //x_memset(u4MtimageDBG,0x0, sizeof(u4MtimageDBG)); 	
        u4MtimageDBGCnt =0;
    }	
	VERIFY (x_sema_unlock(hMtinageDbgInsertSema) == OSR_OK);
}

MTIMAGE_INFO_T * _MTIMAGE_GetImgIdInfo(UCHAR ucImgId)
{
    return &(_rImageInfo[ucImgId]);
}

static BOOL _MTIMAGE_3DCropFuncCB(TDTV_3D_CROP_INTO_T* prCropInfo)
{
    MTIMAGE_INFO_T * rImageInfo ;
	UCHAR ucImgId = 0; //only 0 will play MPO files.
	
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }
    
    prCropInfo->fgIsFixSrcAspectRatio = 1;
    if(rImageInfo->fgIsMPF)
    {        
        //printf("#@# MPO Half\n");
        prCropInfo->fgIsFullResolution = 0;    //vertical will be 1/2, and scaler will enlarge it to 2 multiple ratio.
    }
    else
    {
        //printf("#@# 2D/SBS/TB FULL\n");
        prCropInfo->fgIsFullResolution = 0;
    }
    prCropInfo->u4CropBottomOffset = _rMt3DCropinfo.u4Y;
    prCropInfo->u4CropTopOffset = _rMt3DCropinfo.u4Y;    
    prCropInfo->u4CropLeftOffset = _rMt3DCropinfo.u4X;        
    prCropInfo->u4CropRightOffset = _rMt3DCropinfo.u4X;       

    //printf("#@# _MTIMAGE_3DCropFuncCB [FULL=%d][%d][%d][%d][%d]/n", prCropInfo->fgIsFullResolution, _rMt3DCropinfo.u4X, _rMt3DCropinfo.u4Y, _rMt3DCropinfo.u4Width, _rMt3DCropinfo.u4Height);

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
    MTIMAGE_INFO_T* rImageInfo = prImageInfo;
    FBM_CREATE_FBG_PAR_T rPar;

    MTIMAGE_KERNEL_DEBUG("_FBGHandler, Here is panelW =%d, panelH =%d\n", prImageInfo->u4PanelWidth, prImageInfo->u4PanelHeight);
    //prepare VDP frame buffer, create FBG
    prImageInfo->u1CurView = 0;
    for (u4ViewIdx = 0; u4ViewIdx < MAX_VIEW_NUM; u4ViewIdx++)
    {
        if (fgConnect)
        {
            //reset
            if(prImageInfo->aucFbgId[u4ViewIdx] != 0xFF)
            {                
                FBM_ReleaseGroup(prImageInfo->aucFbgId[u4ViewIdx]);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, MTIMAGE_DECODER_ID);
            }

            prImageInfo->aucFbgId[u4ViewIdx] = 0xFF;
            prImageInfo->aucCurImgFbId[u4ViewIdx] = 0xFF;
            prImageInfo->aucPrevImgFbId[u4ViewIdx] = 0xFF;
            prImageInfo->aucCurDispFbId[u4ViewIdx] = 0xFF;
            #ifdef MAIN_SUB_UT_TEST
            if (prImageInfo->fgConnect)
            {
                FBM_ReleaseGroup(prImageInfo->aucFbgId[u4ViewIdx]);
            }
            #endif
            //todo
            //SRM_SetMmMode(SRM_MM_MODE_TYPE_JPEG);

            //ucFbgType = FBM_SelectGroupType(MAX_WIDTH, MAX_HEIGHT);
            ucFbgType = FBM_SelectGroupType(prImageInfo->u4PanelWidth, prImageInfo->u4PanelHeight);
            FBM_SetColorMode(FBM_CM_422);
            //prImageInfo->aucFbgId[u4ViewIdx] = FBM_CreateGroup(ucFbgType, FBM_VDEC_JPEG, MAX_WIDTH, MAX_HEIGHT);
            //prImageInfo->aucFbgId[u4ViewIdx] = FBM_CreateGroupExt(ucFbgType, FBM_VDEC_JPEG, MAX_WIDTH, MAX_HEIGHT, FBM_FBG_APP_MTIMAGE);
            x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
            rPar.u1AppMode = FBM_FBG_APP_MTIMAGE; 
            prImageInfo->aucFbgId[u4ViewIdx] = FBM_CreateGroupExt(ucFbgType, FBM_VDEC_JPEG, prImageInfo->u4PanelWidth, prImageInfo->u4PanelHeight, &rPar);
            if (prImageInfo->aucFbgId[u4ViewIdx] == FBM_FBG_ID_UNKNOWN)
            {
                MTIMAGE_FBM_DEBUG("Create View %d Fbg Fail!!\n", u4ViewIdx);
                //SRM_SetMmMode(SRM_MM_MODE_TYPE_OFF);
                return FALSE;
            }
            FBM_SetPlayMode(prImageInfo->aucFbgId[u4ViewIdx], FBM_FBG_MM_MODE);
            FBM_SetSyncStc(prImageInfo->aucFbgId[u4ViewIdx], 0, 0); //none
            //FBM_FbgChgNotify(prImageInfo->aucFbgId[u4ViewIdx], 0);

            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prImageInfo->aucFbgId[u4ViewIdx]);
            if(prFbmSeqHdr == NULL)
            {
                ASSERT(prFbmSeqHdr != NULL);
                return FALSE;
            }
            //prFbmSeqHdr->u2HSize = MAX_WIDTH;
            //prFbmSeqHdr->u2VSize = MAX_HEIGHT;
            prFbmSeqHdr->u2HSize = prImageInfo->u4PanelWidth;
            prFbmSeqHdr->u2VSize = prImageInfo->u4PanelHeight;            
            prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
            //prFbmSeqHdr->u2LineSize = MAX_WIDTH;
            prFbmSeqHdr->u2LineSize = ((prImageInfo->u4PanelWidth+15)&(~0xf));
            prFbmSeqHdr->fgProgressiveSeq = TRUE;
            prFbmSeqHdr->fgRasterOrder = TRUE;
            prFbmSeqHdr->fg422Mode = TRUE;
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;

            FBM_SetFrameBufferFlag(prImageInfo->aucFbgId[u4ViewIdx], FBM_FLAG_SEQ_CHG);
        }
        else
        {
            //if ever connected and fbgid isn't 0xff, then release FBG
            //if((prImageInfo->fgConnect)&&(prImageInfo->aucFbgId[u4ViewIdx] != 0xFF))
            if(prImageInfo->aucFbgId[u4ViewIdx] != 0xFF)
            {                
                FBM_ReleaseGroup(prImageInfo->aucFbgId[u4ViewIdx]);
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, MTIMAGE_DECODER_ID);
            }
            //reset
            prImageInfo->aucFbgId[u4ViewIdx] = 0xFF;
            prImageInfo->aucCurImgFbId[u4ViewIdx] = 0xFF;
            prImageInfo->aucPrevImgFbId[u4ViewIdx] = 0xFF;
            prImageInfo->aucCurDispFbId[u4ViewIdx] = 0xFF;

            //SRM_SetMmMode(SRM_MM_MODE_TYPE_OFF);
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
    if ((pucFbId == NULL) || (pu4YAddr == NULL) || (pu4CAddr == NULL) || (rImageInfo == NULL))
    {
        MTIMAGE_FBM_DEBUG("AllocFrame input parameter error\n");
        return FALSE;
    }
    //get frame buffer
    *pucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
    if (*pucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
    {
        MTIMAGE_FBM_DEBUG("Get empty frame buffer failed.\n");
        return FALSE;
    }
    FBM_GetFrameBufferAddr(ucFbgId, *pucFbId, pu4YAddr, pu4CAddr);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, *pucFbId);
    if ((prFbmPicHdr == NULL) || (pu4YAddr == NULL) || (pu4YAddr == NULL))
    {
        MTIMAGE_FBM_DEBUG("Alloc frame buffer failed.\n");
        return FALSE;
    }
    prFbmPicHdr->ucPicStruct = (UCHAR)3;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    FBM_UpdateReferenceList(ucFbgId, *pucFbId, FALSE);    //default
    FBM_SetFrameBufferStatus(ucFbgId, *pucFbId, (UCHAR)FBM_FB_STATUS_READY);
    MTIMAGE_FBM_DEBUG("<_AllocFrame FBM_SetFrameBufferStatus[%d, %d] READY>\n", ucFbgId, *pucFbId);

    u4FrameSize = (UINT32)(*pu4CAddr - *pu4YAddr);
    //MTIMAGE_FBM_DEBUG("Alloc Y 0x%x, C 0x%x, Sz 0x%x\n", *pu4YAddr, *pu4CAddr, u4FrameSize);

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

#if 1
static IMG_SLIDE_SHOW_EFFECT_T _ConvertSlideShowType(UINT8 u1Type)
{
    //check data structure "IMG_SLIDE_SHOW_EFFECT_T" and "MTIMAGE_SLIDETYPE"
    return (IMG_SLIDE_SHOW_EFFECT_T)u1Type;
}
#endif

void _MTIMAGE_Notify_0(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
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
    ucFbgId = rImageInfo->aucFbgId[rImageInfo->u1CurView];
    ucFbId = rImageInfo->aucCurImgFbId[rImageInfo->u1CurView];

    if (u4State == JPG_HW_NFY_FINISHED)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_FINISH;
        rImageInfo->rCallbackInfo.u4Param = (UINT32)pv_tag;
        if((rImageInfo->fgIsVdoMode)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, TRUE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);
            MTIMAGE_FBM_DEBUG("<_MTIMAGE_Notify_0 FBM_SetFrameBufferStatus[%d, %d] READY>\n", ucFbgId, ucFbId);
        }
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
        if((rImageInfo->fgIsVdoMode)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            MTIMAGE_FBM_DEBUG("<_MTIMAGE_Notify_0 FBM_SetFrameBufferStatus[%d, %d] EMPTY>\n", ucFbgId, ucFbId);
            rImageInfo->aucCurImgFbId[rImageInfo->u1CurView] = 0xFF;
        }
    }
    else if (u4State == JPG_HW_NFY_FILL_BUF)
    {
        if ((rImageInfo->imgFormat == IMAGE_JPEG)||(rImageInfo->imgFormat == IMAGE_MPO)||(rImageInfo->imgFormat == IMAGE_PROGRESSIVE_JPEG))
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
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("<_MTIMAGE_Notify_0> u4State=%x, wired\n", u4State);
        return;
    }
    rImageInfo->rCallbackInfo.u4ImgId = u4ImgID;
    _CB_PutEvent(CB_MTIMAGE_TRIGGER, sizeof(IMG_CALLBACK_INFO_T), &rImageInfo->rCallbackInfo);
}

void _MTIMAGE_Notify_1(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{
    IMG_FILL_BUF_MW_T* prFillBuf;
    UCHAR ucFbgId, ucFbId;
    FBM_POOL_T *prFbmPool;
    UCHAR ucImgId = (UCHAR)u4ImgID;
    MTIMAGE_INFO_T * rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("<_MTIMAGE_Notify_1> fail\n");
        return ;
    }
    ucFbgId = rImageInfo->aucFbgId[rImageInfo->u1CurView];
    ucFbId = rImageInfo->aucCurImgFbId[rImageInfo->u1CurView];

    if (u4State == JPG_HW_NFY_FINISHED)
    {
        rImageInfo->rCallbackInfo.u4Condition = MT_IMAGE_CALLBACK_FINISH;
        rImageInfo->rCallbackInfo.u4Param = (UINT32)pv_tag;
        if((rImageInfo->fgIsVdoMode)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, TRUE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);
            MTIMAGE_FBM_DEBUG("<_MTIMAGE_Notify_1 FBM_SetFrameBufferStatus[%d, %d] READY>\n", ucFbgId, ucFbId);
        }
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
        if((rImageInfo->fgIsVdoMode)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            MTIMAGE_FBM_DEBUG("<_MTIMAGE_Notify_1 FBM_SetFrameBufferStatus[%d, %d] EMPTY>\n", ucFbgId, ucFbId);
            rImageInfo->aucCurImgFbId[rImageInfo->u1CurView] = 0xFF;
        }
    }
    else if (u4State == JPG_HW_NFY_FILL_BUF)
    {        
        if ((rImageInfo->imgFormat == IMAGE_JPEG)||(rImageInfo->imgFormat == IMAGE_MPO)||(rImageInfo->imgFormat == IMAGE_PROGRESSIVE_JPEG))            
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
    }
    else
    {
        MTIMAGE_KERNEL_DEBUG("<_MTIMAGE_Notify_1> u4State=%x, wired\n", u4State);
        return;
    }
    rImageInfo->rCallbackInfo.u4ImgId = u4ImgID;
    MTIMAGE_KERNEL_DEBUG("caller_pid=%d  u4ImgId %d, confition=%d\n", rImageInfo->u4UserCallerPid, u4ImgID, rImageInfo->rCallbackInfo.u4Condition);

    _CB_PutEvent(CB_MTIMAGE_TRIGGER_1, sizeof(IMG_CALLBACK_INFO_T), &rImageInfo->rCallbackInfo);
}

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
    MTIMAGE_SLIDESHOW_DEBUG("Notify SlideShowEvent %d\n", (UINT8)e_event);
}


/**set image connect to vdp
*@param connect or disconnect
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
#include "vdec_if.h"
#include "vdec_drvif.h"

static MT_RESULT_T _MTIMAGE_Connect(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T rArg;
    BOOL fgConnect, fgIsVdoMode;
    MTIMAGE_IMGCOLORFORMAT_T eColorFMT;
    UCHAR ucImgId;
    //UINT32 u4Idx;
    INT32 i4Ret;
    MTIMAGE_INFO_T * rImageInfo ;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    fgConnect = (BOOL)rArg.ai4Arg[0];
    eColorFMT = (MTIMAGE_IMGCOLORFORMAT_T)rArg.ai4Arg[1];
    ucImgId = (UCHAR)rArg.ai4Arg[2];
	//set to false
    fgIsVdoMode = FALSE;//(eColorFMT == MT_IMAGE_COLOR_FORMAT_Y_CBCR422) ? TRUE : FALSE;
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

    rImageInfo->u4UserCallerPid = rArg.ai4Arg[5];
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Connect imgid=%d, pid=%d, eColorFMT=%d\n", ucImgId, rImageInfo->u4UserCallerPid, eColorFMT);
    
    if(fgConnect==true)
    {    
        u4MtimageDBGInsert(_mtimageDebugStep, 0xff - ucImgId);    //start code for debug
    }
    u4MtimageDBGInsert(_mtimageDebugStep, 1);

    //if(ucImgId==0)
    {        
        rImageInfo->u4PanelWidth = (UINT32)rArg.ai4Arg[3];
        rImageInfo->u4PanelHeight = (UINT32)rArg.ai4Arg[4];    
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Connect,id=%d Here is panelW =%d, panelH =%d\n", ucImgId, rImageInfo->u4PanelWidth, rImageInfo->u4PanelHeight);
    }
    
    if (rImageInfo == NULL)
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Connect rImageInfo=NULL[%d]\n", ucImgId);
        return MTR_NOT_OK;
    }

    rImageInfo->fgDrvFeedData = 1;
    IMG_Lib_Init();

    //if OSD mode, do nothing
    //else if VDP mode, prepare frame buffer
    if (fgIsVdoMode)
    {
        if((rImageInfo->fgFBGCreated==1)&&fgConnect==0)    //when exit => close FBG
        {
        	//first connect, and exit, init variables,
        	rImageInfo->eDetectedMode = 0xff;
					rImageInfo->fgFBGCreated = 0;
        		
	        if (!_FBGHandler(rImageInfo, fgConnect))
	        {
	            rImageInfo->fgConnect = FALSE;
	            return MTR_NOT_OK;
	        }
        }
        //slide show init/stop
        if (fgConnect)
        {
   #if 1
            x_img_slideshow_init_for_fade(_MTIMAGE_vdp_stretchblit);
            
            if ((i4Ret = x_img_init_slideshow(_MTIMAGE_SlideShow_Notify)) != SLIDESHOW_OK)
            {
                MTIMAGE_SLIDESHOW_DEBUG("InitSlideShowLib fail %d\n", i4Ret);
            }
#endif
            if (rImageInfo->hSlideShowSema == (HANDLE_T)NULL)
            {
                VERIFY(x_sema_create(&rImageInfo->hSlideShowSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
            }
            //rImageInfo->eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
        }
        else
        {
            fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, NULL);   //restore callback to NULL when exit jpg
#if 1   //cannt re-init?
            if ((i4Ret = x_img_stop_slideshow()) != SLIDESHOW_OK)
            {
                MTIMAGE_KERNEL_DEBUG("InitSlideShowStop fail %d\n", i4Ret);
            }
#endif
        }
    }
    rImageInfo->fgConnect = fgConnect;
    rImageInfo->fgIsVdoMode = fgIsVdoMode;

	//_rImageInfo.eImgDispCM = (eColorFMT == MT_IMAGE_COLOR_FORMAT_Y_CBCR422)? Y_CBCR422_SEP16 : AYCBCR8888_DIRECT32;

    switch(eColorFMT)
    {
	case MT_IMAGE_COLOR_FORMAT_Y_CBCR422:
        rImageInfo->eImgDispCM = Y_CBCR422_SEP16;
		MTIMAGE_KERNEL_DEBUG("-- Y_CBCR422_SEP16\n");
		
		break;
	case MT_IMAGE_COLOR_FORMAT_Y_CBCR420:
        rImageInfo->eImgDispCM = Y_CBCR420_SEP16;
		MTIMAGE_KERNEL_DEBUG("-- Y_CBCR420_SEP16\n");
		
		break;		
	case MT_IMAGE_COLOR_FORMAT_ARGB8888:
        rImageInfo->eImgDispCM = ARGB8888_DIRECT32;
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
	case MT_IMAGE_COLOR_FORMAT_LUT8:
		rImageInfo->eImgDispCM = ARGB_LUT8_CM;
		break;					
	default:		
		break;		
    }       
    u4MtimageDBGInsert(_mtimageDebugStep, 9);
    return MTR_OK;
}


//reset FBG related variables
static void _MTIMAGE_FBGStructInit(void)
{
    MTIMAGE_INFO_T * rImageInfo_0 ;
    MTIMAGE_INFO_T * rImageInfo_1 ;
    
    rImageInfo_0 = _MTIMAGE_GetImgIdInfo(0);
    rImageInfo_1 = _MTIMAGE_GetImgIdInfo(1);
    
    rImageInfo_0->aucFbgId[0] = 0xFF;
    rImageInfo_0->aucCurImgFbId[0] = 0xFF;
    rImageInfo_0->aucPrevImgFbId[0] = 0xFF;
    rImageInfo_0->aucCurDispFbId[0] = 0xFF;

    rImageInfo_1->aucFbgId[0] = 0xFF;
    rImageInfo_1->aucCurImgFbId[0] = 0xFF;
    rImageInfo_1->aucPrevImgFbId[0] = 0xFF;
    rImageInfo_1->aucCurDispFbId[0] = 0xFF;    
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

u4MtimageDBGInsert(_mtimageDebugStep, 11);
    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

	MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Open ucImgId %d, hHandle %x, rImageInfo %x, Cur %d\n", 
						ucImgId, hHandle, rImageInfo, u4CurState);	

    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }

    if(fgFBGStructInit==0)
    {
        fgFBGStructInit =1;
        _MTIMAGE_FBGStructInit();
    }        


    eImgBufType = (rImageInfo->eImgDispCM == AYCBCR8888_DIRECT32)? RING_IN_SINGLE_OUT: RING_IN_DOUBLE_OUT;

    //for jpg ARGB8888
    if (((rImageInfo->imgFormat==IMAGE_JPEG)||(rImageInfo->imgFormat==IMAGE_MPO)||(rImageInfo->imgFormat==IMAGE_PROGRESSIVE_JPEG))&&(rImageInfo->eImgDispCM == ARGB8888_DIRECT32))
    {
       eImgBufType = RING_IN_SINGLE_OUT;
    }
    
    u4CurState = IMG_GetDecState(ucImgId);
    if (u4CurState == IMG_RELEASE)
    {
        if (ucImgId == 0)
        {
            if (IMG_Init(eImgBufType, hHandle,(UINT32)ucImgId, _MTIMAGE_Notify_0) != E_IMG_OK)
            {
                return MTR_NOT_OK;
            }
        }
        else
        {
            if (IMG_Init(eImgBufType, hHandle,(UINT32)ucImgId, _MTIMAGE_Notify_1) != E_IMG_OK)
        {
            return MTR_NOT_OK;
        }
        }
        IMG_SetUseMw(ucImgId,TRUE);
        rImageInfo->eRotateDegree = CLOCKWISE_0;    //reset setting
        if (!(rImageInfo->u1PicIdx == 1))   //in the middle of MPF decoding
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

static MT_RESULT_T _MTIMAGE_SetDisplayPath(unsigned long arg)
{
    MTIMAGE_INFO_T * rImageInfo ;    
    MTAL_IOCTL_4ARG_T rArg;
    UCHAR ucImgId;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    ucImgId   = (UCHAR)rArg.ai4Arg[0];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    
    rImageInfo->decoder_id  = (UCHAR)rArg.ai4Arg[1];
    rImageInfo->video_id    = (UCHAR)rArg.ai4Arg[2];
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
    IMG_PARAM_T rInfo;
    UINT32 u4YAddr = 0, u4CAddr = 0, u4CurState, u4DecW, u4DecH, u4Pitch;
    UCHAR ucFbId;
    UCHAR ucFbgId;
    UINT32 u4MPFPicIdx; //0: 2D pic, 1: MPF 1st pic, 2: MPF 2nd pic
    UINT32 u4DecAddr = 0;
    UINT32 u4SrcWidth,u4SrcHeight,u4ImgSize;
    UCHAR ucImgId;
    UINT32 u4ThumbType; 
    MTIMAGE_INFO_T * rImageInfo ;
    FBM_POOL_T *prFbmPoolJPG;    

    USR_SPACE_ACCESS_VALIDATE_12ARG(arg);
    COPY_FROM_USER_12ARG(arg, rArg);
u4MtimageDBGInsert(_mtimageDebugStep, 21);
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
    u4ThumbType = (BOOL)rArg.ai4Arg[11];
    
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    
    rImageInfo->u4ThumbnailType = u4ThumbType;
    
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }

    if (rImageInfo->fgIsVdoMode)
    {
    		//when first connect => creat FBG
        if((rImageInfo->fgFBGCreated==0)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))    
        {
        	//first connect, and exit, init variables,
        	rImageInfo->eDetectedMode = 0xff;
					rImageInfo->fgFBGCreated = 1;
					
	        if (!_FBGHandler(rImageInfo, TRUE))	
	        {
	        		rImageInfo->fgFBGCreated = FALSE;
	            rImageInfo->fgConnect = FALSE;
	            return MTR_NOT_OK;
	        }
        }
	}

    ucFbgId = rImageInfo->aucFbgId[rImageInfo->u1CurView];

    //decode
    u4CurState = IMG_GetDecState(ucImgId);
    if (u4CurState == IMG_INIT)
    {
        if ((rImageInfo->fgIsVdoMode)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))
        {
            //FBM_FbgChgNotify(rImageInfo->aucFbgId[0], 0);   
            #ifdef MAIN_SUB_UT_TEST
            FBM_FbgChgNotify(rImageInfo->aucFbgId[0], MTIMAGE_DECODER_ID);
            #else            
            FBM_FbgChgNotify(rImageInfo->aucFbgId[rImageInfo->u1CurView], MTIMAGE_DECODER_ID);
            #endif
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
                    return MTR_NOT_OK;
                }
                MTIMAGE_FBM_DEBUG("_MTIMAGE_Decode alloc decode buffer [id=%d][%x, %x]\n", ucFbId, u4YAddr, u4CAddr);
                rImageInfo->aucPrevImgFbId[rImageInfo->u1CurView] = rImageInfo->aucCurImgFbId[rImageInfo->u1CurView];
                rImageInfo->aucCurImgFbId[rImageInfo->u1CurView] = ucFbId;
            }
            else
            {
                FBM_GetFrameBufferAddr(ucFbgId, rImageInfo->aucCurImgFbId[rImageInfo->u1CurView], &u4YAddr, &u4CAddr);
                MTIMAGE_FBM_DEBUG("_MTIMAGE_Decode alloc 2 decode buffer [id=%d][%x, %x]\n", rImageInfo->aucCurImgFbId[rImageInfo->u1CurView], u4YAddr, u4CAddr);
            }
        }

        x_memset((void*)&rInfo, 0, sizeof(IMG_PARAM_T));
        rInfo.eFormat = imgFormat;
        rInfo.pu1InBuffer = NULL;    //use internal ring buffer
        rInfo.u8BufSize = 0;            //use internal ring buffer
        rInfo.rDstRegion.u4StartX = 0;
        rInfo.rDstRegion.u4StartY = 0;

        if (rImageInfo->fgIsVdoMode)
        {
            if ((u4DecW > 0) && (u4DecH > 0) && (u4DecW < 1920) && (u4DecH < 1080))
            {
                MTIMAGE_KERNEL_DEBUG("Vdo Specify dec resolution %dx%d\n", u4DecW, u4DecH);
                rInfo.rDstRegion.u4Width = rImageInfo->u2ThumbnailSetDecWidth = u4DecW;
                rInfo.rDstRegion.u4Height = rImageInfo->u2ThumbnailSetDecHeight = u4DecH;
            }
            else
            {
                rImageInfo->u1PicIdx = u4MPFPicIdx;
                if (u4MPFPicIdx > 0)
                {
                    rImageInfo->fgIsMPF = TRUE;
                    //rInfo.rDstRegion.u4Width = MPF_HALF_W;    //half size
                    //rInfo.rDstRegion.u4Height = MPF_HALF_H*2;  //height will be reduce to 1/2 in decoder, but use 1920x1080 output window
                    rInfo.rDstRegion.u4Width = rImageInfo->u4PanelWidth;    //half size
                    rInfo.rDstRegion.u4Height = rImageInfo->u4PanelHeight;  //height will be reduce to 1/2 in decoder, but use 1920x1080 output window                    
                }
                else
                {
                    rImageInfo->fgIsMPF = FALSE;
                    rInfo.rDstRegion.u4Width = 0;    //keep original width if < 1920
                    rInfo.rDstRegion.u4Height = 0;   //keep original width if < 1080
                }
            }
            rInfo.rDstRegion.u4Pitch = u4Pitch;    //rVpInfo.u4Width;

            rInfo.eDispColorMode = rImageInfo->eImgDispCM;
            rInfo.eRotateDegree = CLOCKWISE_0;

            if(rImageInfo->u4ThumbnailType==MT_IMAGE_FILL_WITH_BOX_THUMBNAIL)
            {                
                rInfo.bKeepRatio = false;
            }
            else
            {
                rInfo.bKeepRatio = TRUE;                    
            }                
                
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
            
            if(rImageInfo->u4ThumbnailType!=MT_IMAGE_NOT_THUMBNAIL)
            {
                  prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);                    
                  
                  rInfo.rDstRegion.pu1Addr = (UINT8 *)(prFbmPoolJPG->u4Addr + MT_THUMBNAIL_VDO_DECODING_BUFFER_OFFSET);
                  rInfo.rDstRegion.u4CbCrOffset = MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET;
                
                //clean thumbnail buffer 900KB
                GFX_Lock();
                
                GFX_SetDst((UINT8 *)rInfo.rDstRegion.pu1Addr,CM_RGB_CLUT8, 1024);
                GFX_SetColor(0x0);
                GFX_Fill(0, 0, 1024, 900);
                
                GFX_SetDst((UINT8 *)(rInfo.rDstRegion.pu1Addr+rInfo.rDstRegion.u4CbCrOffset),CM_RGB_CLUT8, 1024);
                GFX_SetColor(0x80);
                GFX_Fill(0, 0, 1024, 900);
                //clean buffer for MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET, buffer which is rotated 
                GFX_SetDst((UINT8 *)(prFbmPoolJPG->u4Addr + MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET),CM_RGB_CLUT8, 1024);
                GFX_SetColor(0x0);
                GFX_Fill(0, 0, 1024, 900);
                
                GFX_SetDst((UINT8 *)(prFbmPoolJPG->u4Addr + MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET + MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET),CM_RGB_CLUT8, 1024);
                GFX_SetColor(0x80);
                GFX_Fill(0, 0, 1024, 900);
                
                
                GFX_Flush();
                GFX_Wait();
                GFX_Unlock();                  
                  
            }                
        }
        else
        {        
            if((rImageInfo->eImgDispCM == Y_CBCR420_SEP16) || (rImageInfo->eImgDispCM ==Y_CBCR422_SEP16))
            {
				MTIMAGE_KERNEL_DEBUG("Video DispCM color mode = %d\n", rImageInfo->eImgDispCM);
				
				if ((u4DecW > 0) && (u4DecH > 0) && (u4DecW <= 3840) && (u4DecH <= 2160))
				{
					MTIMAGE_KERNEL_DEBUG("Vdo Specify dec resolution %dx%d\n", u4DecW, u4DecH);
					rInfo.rDstRegion.u4Width = u4DecW;
					rInfo.rDstRegion.u4Height = u4DecH;
				}
				else
				{
					rImageInfo->u1PicIdx = u4MPFPicIdx;
					if (u4MPFPicIdx > 0)
					{
						rImageInfo->fgIsMPF = TRUE;
						//rInfo.rDstRegion.u4Width = MPF_HALF_W;	//half size
						//rInfo.rDstRegion.u4Height = MPF_HALF_H*2;  //height will be reduce to 1/2 in decoder, but use 1920x1080 output window
						rInfo.rDstRegion.u4Width = rImageInfo->u4PanelWidth;	//half size
						rInfo.rDstRegion.u4Height = rImageInfo->u4PanelHeight;	//height will be reduce to 1/2 in decoder, but use 1920x1080 output window					  
					}
					else
					{
						rImageInfo->fgIsMPF = FALSE;
						rInfo.rDstRegion.u4Width = 0;	 //keep original width if < 1920
						rInfo.rDstRegion.u4Height = 0;	 //keep original width if < 1080
					}
				}
				
				if(u4Pitch == 0)
				{
					rInfo.rDstRegion.u4Pitch = ((rInfo.rDstRegion.u4Width+0xf)&(~0xf));
					
				}
				else
				{
					rInfo.rDstRegion.u4Pitch = u4Pitch;    //rVpInfo.u4Width;
				}
				
				rInfo.eDispColorMode = rImageInfo->eImgDispCM;
				rInfo.eRotateDegree = CLOCKWISE_0;
				
				if(rImageInfo->u4ThumbnailType==MT_IMAGE_FILL_WITH_BOX_THUMBNAIL)
				{				 
					rInfo.bKeepRatio = false;
				}
				else
				{
					rInfo.bKeepRatio = TRUE;
				}
				
				if(u4DecAddr == 0)
				{
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
					
				    GFX_Memset((UINT8*)u4DecAddr, rImageInfo->u4PanelWidth, rImageInfo->u4PanelHeight, 0);
				    GFX_Memset((UINT8*)(u4DecAddr+rImageInfo->u4PanelWidth*rImageInfo->u4PanelHeight), 
						rImageInfo->u4PanelWidth, rImageInfo->u4PanelHeight, 0x80);
				}
				
				if (u4MPFPicIdx > 1)
				{
					rInfo.prDecOut = &rImageInfo->rDecOut2;
					rInfo.rDstRegion.pu1Addr =
						(UINT8 *)(u4DecAddr + rImageInfo->rDecOut.u4Pitch * rImageInfo->rDecOut.u4TotalLine);
				}
				else
				{
					rInfo.prDecOut = &rImageInfo->rDecOut;
					rInfo.rDstRegion.pu1Addr = (UINT8 *)u4DecAddr;
				}
				
                rInfo.rDstRegion.u4CbCrOffset = rInfo.rDstRegion.u4Pitch*rInfo.rDstRegion.u4Height;

				MTIMAGE_KERNEL_DEBUG("## Dst width=%d, height=%d, pitch=%d, offset=%d\n", 
					rInfo.rDstRegion.u4Width, rInfo.rDstRegion.u4Height, rInfo.rDstRegion.u4Pitch, rInfo.rDstRegion.u4CbCrOffset);
				
			}
			else
            {
                if(((u4DecW > 0) && (u4DecH > 0) && (u4DecW < 1920) && (u4DecH <1080))||(!rImageInfo->fgIsVdoMode))
                {
                    MTIMAGE_KERNEL_DEBUG(" OSD Specify dec resolution %dx%d\n", u4DecW, u4DecH);
                    rInfo.rDstRegion.u4Width = u4DecW;
                    rInfo.rDstRegion.u4Height = u4DecH;
                }
                else
                {
                    rInfo.rDstRegion.u4Width = 0;
                    rInfo.rDstRegion.u4Height = 0;
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
		
        if(imgFormat == IMAGE_PNG)
        {
            rInfo.rSrcRegion.u4SrcStartX = 0;
            rInfo.rSrcRegion.u4SrcStartY = 0;
            rInfo.rSrcRegion.u4SrcWidth = u4SrcWidth;
            rInfo.rSrcRegion.u4SrcHeight = u4SrcHeight;
			//LUT8, set LUT8 display mode
			if(rInfo.eDispColorMode == ARGB_LUT8_CM)
			{
				rInfo.u4DecColorMode = 8;//GFX_COLORMODE_ARGB_CLUT8
			}
			else
			{
            	rInfo.u4DecColorMode = 12;//GFX_COLORMODE_ARGB_D8888
			}
        }
		
        IMG_Set_ImgSize(ucImgId,u4ImgSize);
        rInfo.fgDrvFeedData = rImageInfo->fgDrvFeedData;
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
    UINT32 u4YAddr, u4CAddr, u4CurState, u4DispYAddr, u4DispCAddr, u4DispYAddr_tmp, u4DispCAddr_tmp, u4CurYAddr, u4CurCAddr, u4FrmNum;
    UINT32 u4DstYAddr, u4DstCAddr;
    UCHAR ucFbId, ucSlideShowFbId;
    INT32 i4Ret;
    // 3d related parameter
    UINT32 u43DInOut;
    UINT16 u2InFmt, u2OutFmt;
    E_TDTV_DECODER_INPUT eDetectedMode;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    DRV_OSD_T rOsd;
     FBM_POOL_T *prFbmPoolJPG;
     UINT32 u4List;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;
    TDTV_3D_CROP_INTO_T cropinfo;
    UINT32 u4LogYAddr, u4LogCAddr;
     
u4MtimageDBGInsert(_mtimageDebugStep, 31);
    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prImgSetting=(MTIMAGE_PARAM_T *)rArg.ai4Arg[1];
    fgPartialSrc = (BOOL)rArg.ai4Arg[2];
    prRegion = (MTVDO_REGION_T *)rArg.ai4Arg[3];
    ucImgId = (UCHAR)rArg.ai4Arg[4];

    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_REGION_T);
    USR_SPACE_ACCESS_VALIDATE_ARG(prImgSetting, MTIMAGE_PARAM_T);
    COPY_FROM_USER_ARG(prImgSetting, rImgSetting, MTIMAGE_PARAM_T);

    ucFbgId = rImageInfo->aucFbgId[rImageInfo->u1CurView];
    rImageInfo->u4PlaneId = prImgSetting->u1PlaneId;
    u4CurState = IMG_GetDecState(ucImgId);
	MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display Id[%d]State[%d]Dec[%d %d %d %d][%d]Dst[%d %d %d %d]\n", 
		    ucImgId, u4CurState,
		    rImageInfo->rDecOut.u4StartX, rImageInfo->rDecOut.u4StartY, rImageInfo->rDecOut.u4Width, rImageInfo->rDecOut.u4Height,
		    rImageInfo->rDecOut.u4Pitch,
		    rImgSetting.rDstRegion.u4X, rImgSetting.rDstRegion.u4Y, rImgSetting.rDstRegion.u4Width, rImgSetting.rDstRegion.u4Height); 	
    if (u4CurState == IMG_IDLE) // will inter idle after decode finish in MW mode
    {
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

        //rCmd.bKeepOriResolution = rImageInfo->fgIsMPF ? FALSE : true;    
        //new flush mode
		if(!rImgSetting.fgIsSlideShow && (rImgSetting.eRotation == MT_IMAGE_ROTATE_0))
		{		
			rCmd.bShiftImage2StartPosition = TRUE;	
			fgNewFlushMode = TRUE;	
			//send seq chg flag before display to avoid delay
			FBM_SetFrameBufferFlag(rImageInfo->aucFbgId[rImageInfo->u1CurView], FBM_FLAG_SEQ_CHG);						
		}
		else
		{
			rCmd.bShiftImage2StartPosition = FALSE;
			if(fgNewFlushMode)
			{	
				//slide show off mode, seq change for set back panel resolution
				FBM_SetFrameBufferFlag(rImageInfo->aucFbgId[rImageInfo->u1CurView], FBM_FLAG_SEQ_CHG);					
			}
			
		}

        //when picture is big enough, we don't keep original resolution and resize it fit to panelrImageInfo->u4PanelHeight
        if((rImageInfo->fgIsMPF)||(rImageInfo->rDecOut.u4Width>=rImageInfo->u4PanelWidth)||(rImageInfo->rDecOut.u4Height>=rImageInfo->u4PanelHeight)
            ||(((rImgSetting.eRotation==MT_IMAGE_ROTATE_90)||(rImgSetting.eRotation==MT_IMAGE_ROTATE_270))&&(rImageInfo->rDecOut.u4Width>MAX_HEIGHT))
            )
        {
            rCmd.bKeepOriResolution = FALSE;
        }
        else
        {
            rCmd.bKeepOriResolution = TRUE;
        }

        if(rImageInfo->fgIsMPF)
            rCmd.bKeepOriResolution = TRUE;

        //rCmd.bKeepRatio = FALSE;        

        if (rImageInfo->fgIsVdoMode)
        {
            //if cpu modified framebuffer, we flush mpeg buffer 422 format. becuase b2r node mapping buffer has been changed to cachable region.
            HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)rCmd.rSrcRegion.pu1Addr), (UINT32)(rCmd.rSrcRegion.u4CbCrOffset*2));
                
            if (FBM_GetFrameBufferStatus(ucFbgId, rImageInfo->aucCurImgFbId[rImageInfo->u1CurView]) != FBM_FB_STATUS_READY)
            {
                MTIMAGE_KERNEL_DEBUG("Decoded frame not ready!\n");
                return MTR_NOT_OK;
            }
            if (!_AllocFrame(ucImgId,ucFbgId, &rImageInfo->aucCurDispFbId[rImageInfo->u1CurView], &u4YAddr, &u4CAddr))
            {
                return MTR_NOT_OK;
            }
            MTIMAGE_FBM_DEBUG("_MTIMAGE_Display alloc display buffer [id=%d][%x, %x]\n", rImageInfo->aucCurDispFbId[rImageInfo->u1CurView], u4YAddr, u4CAddr);
            MTIMAGE_SLIDESHOW_DEBUG("SlideShow src [fbgid=%d][fbid=%d] Y 0x%x, C 0x%x\n",ucFbgId,rImageInfo->aucCurDispFbId[rImageInfo->u1CurView],u4YAddr, u4CAddr);
            //FBM_GetFrameBufferSize(ucFbgId, &rCmd.rDstRegion.u4Pitch, &rCmd.rDstRegion.u4TotalLine);
            //rCmd.rDstRegion.u4Pitch = MAX_WIDTH;
            //rCmd.rDstRegion.u4TotalLine = MAX_HEIGHT;
            rCmd.rDstRegion.u4Pitch = ((rImageInfo->u4PanelWidth+15)&(~0xf));
            rCmd.rDstRegion.u4TotalLine = rImageInfo->u4PanelHeight;            
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
            rOsd.u4Left = rCmd.rDstRegion.u4StartX / 2;
            rOsd.u4Top = rCmd.rDstRegion.u4StartY / 2;

            if ((rCmd.rDstRegion.u4StartX*2>0xffffffff)||(rCmd.rDstRegion.u4StartY*2>0xffffffff))   //shut warning-generator down
            {
                return FALSE;
            }    

            if((rCmd.rDstRegion.u4Width == rCmd.rSrcRegion.u4Width) &&
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
    	     prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);             
         #endif
            if (prFbmPoolJPG == NULL)
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
            x_memset((void*)VIRTUAL((UINT32)rOsd.pu1FrameBaseAddr),0, rOsd.u4Width* rOsd.u4Height*4);
            OSD_RGN_Create(&rImageInfo->u4DisplayRegionId, rOsd.u4Width, rOsd.u4Height, (void*)rOsd.pu1FrameBaseAddr,
                           OSD_CM_AYCBCR8888_DIRECT32, rCmd.rDstRegion.u4Pitch, rOsd.u4Left, rOsd.u4Top,
                           rOsd.u4Width, rOsd.u4Height);
            OSD_RGN_LIST_Create(&u4List);
            OSD_RGN_Insert(rImageInfo->u4DisplayRegionId, u4List);
            
            if(OSD_PLA_FlipTo(rImageInfo->u4PlaneId, u4List)!=0)
            {                
                return false;
            }                
            
            OSD_RGN_Set(rImageInfo->u4DisplayRegionId, (INT32)OSD_RGN_MIX_SEL,
                     (UINT32)OSD_BM_PIXEL);

            rCmd.rDstRegion.pu1Addr = rOsd.pu1FrameBaseAddr;//_rDisplayOSD.pu1FrameBaseAddr;
            
        }

        if (rImageInfo->fgIsMPF)
        {
            rCmd.rSrcRegion.u4TotalLine <<= 1;
            rCmd.rSrcRegion.u4Height <<= 1;
        }
        //resize or rotation from decoded frame to display frame
	
	 	MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display NewFlush bShiftImage2StartPosition Flag = %d\n", rCmd.bShiftImage2StartPosition);
        IMG_Set(rCmd, &rImageInfo->rDecOut);	

		//new flush mode
		if (!rImgSetting.fgIsSlideShow && (rImgSetting.eRotation == MT_IMAGE_ROTATE_0))
	 	{
			//save out region, align to the real panel resolution
			MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display NewFlush panel %d %d\n", PANEL_GetPanelWidth(), PANEL_GetPanelHeight());
			
			_rCurImgRegion.u4Width = rImageInfo->rDecOut.u4Width * PANEL_GetPanelWidth() / rImageInfo->u4PanelWidth;
			_rCurImgRegion.u4Height = rImageInfo->rDecOut.u4Height * PANEL_GetPanelHeight() / rImageInfo->u4PanelHeight;
			_rCurImgRegion.u4X = (PANEL_GetPanelWidth() - _rCurImgRegion.u4Width) >> 1;
			_rCurImgRegion.u4Y = (PANEL_GetPanelHeight()- _rCurImgRegion.u4Height) >> 1;

			MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display NewFlush: OUTR(%d, %d, %d, %d), ImgDecOut(%d, %d, %d, %d)\n",	
				_rCurImgRegion.u4X, _rCurImgRegion.u4Y, _rCurImgRegion.u4Width, _rCurImgRegion.u4Height,
				rImageInfo->rDecOut.u4StartX, rImageInfo->rDecOut.u4StartY, rImageInfo->rDecOut.u4Width, rImageInfo->rDecOut.u4Height);				
		 	
			//Reset SeqHdr H/V size, then make Seq chg
			prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(rImageInfo->aucFbgId[rImageInfo->u1CurView]);
			if(prFbmSeqHdr == NULL)
			{
				ASSERT(prFbmSeqHdr != NULL);
				return FALSE;
			}				
			prFbmSeqHdr->u2HSize = rImageInfo->rDecOut.u4Width;
			prFbmSeqHdr->u2VSize = rImageInfo->rDecOut.u4Height;			
			prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
			prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
	 	}
		else
		{	
			if(fgNewFlushMode)
			{	
				//Reset SeqHdr H/V size to panel W/H size at non new flush mode
				prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(rImageInfo->aucFbgId[rImageInfo->u1CurView]);			
				MTIMAGE_KERNEL_DEBUG("_MTIMAGE_Display Reset SeqHdr H,V size to Panel W,H (%d,%d) size \n", 
					rImageInfo->u4PanelWidth, rImageInfo->u4PanelHeight);
				if(prFbmSeqHdr == NULL)
				{
					ASSERT(prFbmSeqHdr != NULL);
					return FALSE;
				}				
				prFbmSeqHdr->u2HSize = rImageInfo->u4PanelWidth;
				prFbmSeqHdr->u2VSize = rImageInfo->u4PanelHeight;	
				prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
				prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
				//reset new flush mode flag to false
				fgNewFlushMode = FALSE;	
			}						
		}
		
        _rMt3DCropinfo.u4X = rImageInfo->rDecOut.u4StartX;
        _rMt3DCropinfo.u4Y = rImageInfo->rDecOut.u4StartY;
        _rMt3DCropinfo.u4Width = rImageInfo->rDecOut.u4Width;
        _rMt3DCropinfo.u4Height = rImageInfo->rDecOut.u4Height;

        if (rImageInfo->fgIsVdoMode && rImgSetting.fgDisplay)
        {                       	
            if (rImageInfo->fgIsMPF)
            {
                eDetectedMode = E_TDTV_DECODER_INPUT_TB_P;
                MTIMAGE_MPO_DEBUG("MPO set 3D E_TDTV_DECODER_INPUT_TB_P[%d]\n", rImageInfo->eDetectedMode);
            }
            else
            {
                eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
                MTIMAGE_MPO_DEBUG("MPO set 2D E_TDTV_DECODER_INPUT_2D_P[%d]\n", rImageInfo->eDetectedMode);
            }
            
            #ifdef MAIN_SUB_UT_TEST
            #else
                    _MTIMAGE_3DCropFuncCB(&cropinfo);
                    fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, _MTIMAGE_3DCropFuncCB);            
            #endif
                    u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
            
            if (eDetectedMode != rImageInfo->eDetectedMode)
            {
                u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
                u2InFmt = u43DInOut & 0xFFFF;
                u2OutFmt = (u43DInOut >> 16) & 0xFFFF;
                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);

                if(prFbmSeqHdr == NULL)
                {
                    ASSERT(prFbmSeqHdr != NULL);
                    return MTR_NOT_OK;
                }
                switch (u2OutFmt)
                {
                case E_TDTV_DECODER_OUTPUT_NATIVE:
                    prFbmSeqHdr->fgB2R3DEnable = false;
                    MTIMAGE_MPO_DEBUG("MPO set 2D/3D E_TDTV_DECODER_OUTPUT_NATIVE[%d][%d]\n", eDetectedMode, rImageInfo->eDetectedMode);
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
                FBM_SetFrameBufferPicFlag(ucFbgId, rImageInfo->aucCurDispFbId[rImageInfo->u1CurView], FBM_MM_3D_CHG_FLAG);
            }
            
            //slide show
            if ((rImgSetting.fgIsSlideShow)&&(!rImageInfo->fgIsMPF))
            {
                rImageInfo->u1SlideShowEvent = SLIDESHOW_NFY_STOP;
                u4FrmNum = 0;
								
								MTIMAGE_SLIDESHOW_DEBUG("SlideShow type=%d, time=%d\n",rImageInfo->u1SlideShowType, rImageInfo->u4SlideShowTime);
   
                if((rImageInfo->u1SlideShowType==MT_IMAGE_EFFECT_SLIDE_LEFT2RIGHT)||
                    (rImageInfo->u1SlideShowType==MT_IMAGE_EFFECT_SLIDE_RIGHT2LEFT)||
                    (rImageInfo->u1SlideShowType==MT_IMAGE_EFFECT_FADE_IN))
                {

                    //get currect buffer data    
                    ucFbId = VDP_FrcGetLockFrameBuffer((UCHAR)0);
                    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4DispYAddr_tmp, &u4DispCAddr_tmp);                    
                    
                    //change decoded buffer to currect buffer usage, it will destory original decoded data
                    FBM_GetFrameBufferAddr(ucFbgId, rImageInfo->aucCurImgFbId[rImageInfo->u1CurView], &u4CurYAddr, &u4CurCAddr);

                    MTIMAGE_SLIDESHOW_DEBUG("SlideShow Disp 1 [fbgid=%d][fbid=%d] Y 0x%x, C 0x%x\n",ucFbgId,rImageInfo->aucCurImgFbId[rImageInfo->u1CurView],u4CurYAddr, u4CurCAddr);
                    
                    GFX_Lock();
                    GFX_SetSrc((UINT8 *)u4DispYAddr_tmp, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_SetDst((UINT8 *)u4CurYAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine);
                    GFX_SetSrc((UINT8 *)u4DispCAddr_tmp, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    GFX_SetDst((UINT8 *)u4CurCAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                    //GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine >> 1);
                    GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine);
                    GFX_Flush();
                    GFX_Wait();
                    GFX_Unlock();
                    
                }                    

                do
                {                                                                        
                    if((rImageInfo->u1SlideShowType!=MT_IMAGE_EFFECT_SLIDE_LEFT2RIGHT)&&
                        (rImageInfo->u1SlideShowType!=MT_IMAGE_EFFECT_SLIDE_RIGHT2LEFT)&&
                        (rImageInfo->u1SlideShowType!=MT_IMAGE_EFFECT_FADE_IN))
                    {                
                        ucFbId = VDP_FrcGetLockFrameBuffer((UCHAR)0);
                        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4DispYAddr, &u4DispCAddr);                                                    
                        u4CurYAddr = u4DispYAddr;
                        u4CurCAddr = u4DispCAddr;                                                
                        MTIMAGE_SLIDESHOW_DEBUG("SlideShow Disp 2 [fbgid=%d][fbid=%d] Y 0x%x, C 0x%x\n",ucFbgId,ucFbId,u4DispYAddr, u4DispCAddr);
                    }                                                            
                                               
                    if (!_AllocFrame(ucImgId,ucFbgId, &ucSlideShowFbId, &u4DstYAddr, &u4DstCAddr))
                    {
                        MTIMAGE_FBM_DEBUG("SlideShow Dest _AllocFrame fail\n");
                        return MTR_NOT_OK;
                    }
                    //printf("SlideShow Src Y 0x%x, C 0x%x, Disp Y 0x%x, C 0x%x\n",
                    //       VIRTUAL(u4YAddr), VIRTUAL(u4CAddr), VIRTUAL(u4DispYAddr), VIRTUAL(u4DispCAddr));

                    MTIMAGE_FBM_DEBUG("SlideShow Dest [fbgid=%d][fbid=%d] Y 0x%x, C 0x%x\n",ucFbgId,ucSlideShowFbId,u4DstYAddr, u4DstCAddr);
                    MTIMAGE_SLIDESHOW_DEBUG("SlideShow Dest [fbgid=%d][fbid=%d] Y 0x%x, C 0x%x\n",ucFbgId,ucSlideShowFbId,u4DstYAddr, u4DstCAddr);
                    

                    if((rImageInfo->u1SlideShowType!=MT_IMAGE_EFFECT_SLIDE_LEFT2RIGHT)&&
                        (rImageInfo->u1SlideShowType!=MT_IMAGE_EFFECT_SLIDE_RIGHT2LEFT)&&
                        (rImageInfo->u1SlideShowType!=MT_IMAGE_EFFECT_FADE_IN))
                    {                                            
                        MTIMAGE_SLIDESHOW_DEBUG("Copy Disp[%x] to Dst[%x]\n", u4DispYAddr, u4DstYAddr);
                        GFX_Lock();
                        GFX_SetSrc((UINT8 *)u4DispYAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                        GFX_SetDst((UINT8 *)u4DstYAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                        GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine);
                        GFX_SetSrc((UINT8 *)u4DispCAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                        GFX_SetDst((UINT8 *)u4DstCAddr, (UINT32)CM_RGB_CLUT8, rCmd.rDstRegion.u4Pitch);
                        //GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine >> 1);
                        GFX_BitBlt(0, 0, 0, 0, rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine);
                        GFX_Flush();
                        GFX_Wait();
                        GFX_Unlock();
                        
						HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DstYAddr), (UINT32)(rCmd.rDstRegion.u4Pitch * rCmd.rDstRegion.u4Height));
						HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DstCAddr), (UINT32)(rCmd.rDstRegion.u4Pitch * rCmd.rDstRegion.u4Height));
					}
                    
                    rImageInfo->u4SrcYaddr = u4YAddr;
                    rImageInfo->u4SrcCaddr = u4CAddr;                    
                    rImageInfo->u4CurYaddr = u4CurYAddr;
                    rImageInfo->u4CurCaddr = u4CurCAddr;                    
                    rImageInfo->u4DesYaddr = u4DstYAddr;
                    rImageInfo->u4DesCaddr = u4DstCAddr;                                                            
#if 1
                    x_img_async_slideshow(_ConvertSlideShowType(rImageInfo->u1SlideShowType),
                                          (UCHAR *)VIRTUAL(u4YAddr), (UCHAR *)VIRTUAL(u4CAddr), (UCHAR *)VIRTUAL(u4CurYAddr), (UCHAR *)VIRTUAL(u4CurCAddr),
                                          (UCHAR *)VIRTUAL(u4DstYAddr), (UCHAR *)VIRTUAL(u4DstCAddr),
                                          rCmd.rDstRegion.u4CbCrOffset, rCmd.rDstRegion.u4CbCrOffset,
                                          rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine, rImageInfo->u4SlideShowTime);
#endif
                    i4Ret = x_sema_lock_timeout(rImageInfo->hSlideShowSema, MAX_SLIDE_SHOW_FRM_PERIOD);

                    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TIMEOUT));
                    if (i4Ret == OSR_TIMEOUT)
                    {
                        MTIMAGE_FBM_DEBUG("WaitSlideShow timeout\n");
                        MTIMAGE_SLIDESHOW_DEBUG("WaitSlideShow timeout\n");
                    }
                    u4FrmNum++;
					//flush invalidate, make sure the b2r display data is right.
                    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DstYAddr), (UINT32)(rCmd.rDstRegion.u4Pitch * rCmd.rDstRegion.u4Height));
                    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DstCAddr), (UINT32)(rCmd.rDstRegion.u4Pitch * rCmd.rDstRegion.u4Height));

                    if (rImageInfo->u1SlideShowEvent == SLIDESHOW_NFY_COMPLETE_ONE_PIC)
                    {
                        FBM_SetFrameBufferStatus(ucFbgId, ucSlideShowFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
                        FBM_GetFrameBufferAddr(ucFbgId, ucSlideShowFbId, &u4LogYAddr, &u4LogCAddr);                        
                        MTIMAGE_FBM_DEBUG("<_MTIMAGE_Display FBM_SetFrameBufferStatus[%d, %d] EMPTY>\n", ucFbgId, ucSlideShowFbId);
                        MTIMAGE_SLIDESHOW_DEBUG("$$$ set empty[%d][%d][%d][%x][%x]\n", rImageInfo->u1SlideShowEvent, ucFbgId, ucSlideShowFbId, u4LogYAddr, u4LogCAddr);
                    }
                    else
                    {
                        FBM_SetFrameBufferStatus(ucFbgId, ucSlideShowFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
                        FBM_GetFrameBufferAddr(ucFbgId, ucSlideShowFbId, &u4LogYAddr, &u4LogCAddr);                        
                        MTIMAGE_FBM_DEBUG("<_MTIMAGE_Display FBM_SetFrameBufferStatus[%d, %d] DISPLAY>\n", ucFbgId, ucSlideShowFbId);
                        MTIMAGE_SLIDESHOW_DEBUG("$$$ set display[%d][%d][%d][%x][%x]\n----------\n", rImageInfo->u1SlideShowEvent, ucFbgId, ucSlideShowFbId, u4LogYAddr, u4LogCAddr);
                    }
                    
                    x_thread_delay(30);
                }
                //while(rImageInfo->u1SlideShowEvent != SLIDESHOW_NFY_COMPLETE_ONE_PIC);
                while((rImageInfo->u1SlideShowEvent != SLIDESHOW_NFY_COMPLETE_ONE_PIC) &&
                        (u4FrmNum <= MAX_SLIDE_SHOW_FRM_NUM));
                if (rImageInfo->u1SlideShowEvent != SLIDESHOW_NFY_COMPLETE_ONE_PIC)
                {
                    MTIMAGE_FBM_DEBUG("_MTIMAGE_Display SlideShow Fail, event %d~ ", rImageInfo->u1SlideShowEvent);
                    MTIMAGE_SLIDESHOW_DEBUG("_MTIMAGE_Display SlideShow Fail, event %d~ ", rImageInfo->u1SlideShowEvent);
                }

                FBM_SetFrameBufferStatus(ucFbgId, rImageInfo->aucCurDispFbId[rImageInfo->u1CurView], (UCHAR)FBM_FB_STATUS_EMPTY);
                MTIMAGE_FBM_DEBUG("<_MTIMAGE_Display 1 FBM_SetFrameBufferStatus[%d, %d] EMPTY>\n", ucFbgId, rImageInfo->aucCurDispFbId[rImageInfo->u1CurView]);
            }
            else
            {
                FBM_SetFrameBufferStatus(ucFbgId, rImageInfo->aucCurDispFbId[rImageInfo->u1CurView], (UCHAR)FBM_FB_STATUS_DISPLAYQ);
                MTIMAGE_FBM_DEBUG("<_MTIMAGE_Display 1 FBM_SetFrameBufferStatus[%d, %d] DISPLAY>\n", ucFbgId, rImageInfo->aucCurDispFbId[rImageInfo->u1CurView]);
            }
        }
        else
        {
            OSD_PLA_Enable(rImageInfo->u4PlaneId, TRUE);
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
    MTAL_IOCTL_4ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    UCHAR ucFbgId ,ucFbId, ucImgId;
	BOOL fgNeedDoHalFlushInvalidateDCache = FALSE;
    MTIMAGE_INFO_T * rImageInfo ;

u4MtimageDBGInsert(_mtimageDebugStep, 41);

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];
	fgNeedDoHalFlushInvalidateDCache = (BOOL)rArg.ai4Arg[2];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }

    ucFbgId = rImageInfo->aucFbgId[rImageInfo->u1CurView];
    ucFbId = rImageInfo->aucCurImgFbId[rImageInfo->u1CurView];

    hHandle=(MTIMAGE_HANDLE)arg;
    if (rImageInfo->fgIsVdoMode &&
            !(rImageInfo->u1PicIdx == 1)&&(rImageInfo->u4ThumbnailType==MT_IMAGE_NOT_THUMBNAIL))   //MPF 1st picture
    {
        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
        MTIMAGE_FBM_DEBUG("<_MTIMAGE_Close FBM_SetFrameBufferStatus[%d, %d] EMPTY>\n", ucFbgId, ucFbId);
        MTIMAGE_FBM_DEBUG("====================\n");
    }
    else
    {
            //remove the operation of turning off osd plane             
          //OSD_PLA_Enable(_rImageInfo.u4PlaneId, FALSE);
          //OSD_RGN_Delete(_rImageInfo.u4DisplayRegionId);
    }
    IMG_Release(ucImgId);
u4MtimageDBGInsert(_mtimageDebugStep, 49);
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
		UINT32 timeout=0, threshold=0;

u4MtimageDBGInsert(_mtimageDebugStep, 51);
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];

		if(ucImgId==0)
		{
			threshold = MT_IMAGE_EMP_TIMEOUT;
		}
		else
		{
			threshold = MT_IMAGE_GPU_TIMEOUT;					
		}

    //re-write to abort decoding
    while ((IMG_GetDecState(ucImgId) == IMG_DECODE)&&(timeout<threshold))
    {
        x_thread_delay(1);
        timeout++;        
    }

    if(timeout>=threshold)
    {
        MTIMAGE_KERNEL_DEBUG("###_MTIMAGE_Stop timeout[%d] state[%d]\n", timeout,IMG_GetDecState(ucImgId));
    }

    if (IMG_GetDecState(ucImgId) !=IMG_RELEASE)
    {
        IMG_Stop(ucImgId);
        IMG_SetFillBsBufEnd(ucImgId,0, 0, TRUE);
        IMG_Release(ucImgId);
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
    UCHAR ucImgId;
    //MTIMAGE_INFO_T * rImageInfo ;
    //MTIMAGE_INFO_T type;
    MTVDO_REGION_T region;
    MTVDO_REGION_T *pregion;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    ucImgId = (UCHAR)rArg.ai4Arg[0];
    pregion = (MTVDO_REGION_T *)rArg.ai4Arg[1];
    
    //rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);    

    //get active window
    region.u4X =_rMt3DCropinfo.u4X;
    region.u4Y =_rMt3DCropinfo.u4Y;
    region.u4Width =_rMt3DCropinfo.u4Width;
    region.u4Height =_rMt3DCropinfo.u4Height;

    USR_SPACE_ACCESS_VALIDATE_ARG(pregion, MTVDO_REGION_T);
    COPY_TO_USER_ARG(pregion, region, MTVDO_REGION_T);
    
    return MTR_OK;
}


//MT_IMAGE_COLOR_FORMAT_ARGB8888
static void  _MTIMAGE_IMGYUV2RGB(UCHAR ucImgId,unsigned long arg)
{
    MTIMAGE_HANDLE hHandle;
    UINT32 u4YSrcLen=0;
    //UINT32 u4ARGBSize = (u4YSrcLen*_rImageInfo.rDecOut.u4Height*4);

    FBM_POOL_T *prFbmPoolJPG;

    MTIMAGE_INFO_T * rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        return  ;
    }
    u4YSrcLen=(rImageInfo->rDecOut.u4Pitch + 0xF) & (~0xF);
    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
    //prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);   
    ASSERT(prFbmPoolJPG != (FBM_POOL_T *)NULL);
    ASSERT(prFbmPoolJPG->u4Addr != 0);

    hHandle=(MTIMAGE_HANDLE)arg;
#if 1

    GFX_Lock();
    GFX_SetColCnvFmt(E_YCFMT_422LINEAR,E_SWAP_0,E_VSTD_BT709,E_VSYS_COMP);
    //GFX_SetColCnvSrc((rImageInfo->rDecOut.pu1Addr),u4YSrcLen, (rImageInfo->rDecOut.pu1Addr+rImageInfo->rDecOut.u4CbCrOffset),u4YSrcLen,0);
    GFX_SetColCnvSrc((UINT8 *)(prFbmPoolJPG->u4Addr+MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET),u4YSrcLen, (UINT8 *)(prFbmPoolJPG->u4Addr+MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET+MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET),u4YSrcLen,0);
    GFX_SetDst((UINT8 *)(prFbmPoolJPG->u4Addr+MT_THUMBNAIL_WORKING_BUFFER_OFFSET), CM_ARGB8888_DIRECT32,u4YSrcLen<<2);
    GFX_SetAlpha((UINT32)0xff);
    GFX_ColConv(0,0,rImageInfo->rDecOut.u4Pitch,rImageInfo->rDecOut.u4TotalLine);
    GFX_Flush();
    GFX_Wait();

    GFX_Unlock();

#else
    GFX_Lock();
    //CM_CbYCrY422_DIRECT16, CM_YCbCr_CLUT8
    GFX_SetDst(PHYSICAL(_rDecOut.pu1Addr-u4ARGBSize), CM_ARGB8888_DIRECT32,u4YSrcLen<<2);
    // GFX_SetDst(PHYSICAL(dwJpgTgtPtr), CM_ARGB8888_DIRECT32, _rDecOut.u4Width<<2);

    GFX_SetColCnvSrc(PHYSICAL(_rDecOut.pu1Addr),u4YSrcLen,PHYSICAL(_rDecOut.pu1Addr+_rDecOut.u4CbCrOffset),u4YSrcLen,0);
    GFX_SetColCnvFmt(E_YCFMT_422LINEAR,E_SWAP_0,E_VSTD_BT709,E_VSYS_COMP);
    GFX_SetAlpha((UINT32)0xff);
    GFX_ColConv(0,0,_rDecOut.u4Width,_rDecOut.u4Height);
    GFX_Flush();
    GFX_Wait();

    GFX_SetDst(PHYSICAL(_rDecOut.pu1Addr-u4ARGBSize), CM_ARGB8888_DIRECT32,u4YSrcLen<<2);
    GFX_SetColCnvSrc(PHYSICAL(_rDecOut.pu1Addr),u4YSrcLen,PHYSICAL(_rDecOut.pu1Addr+_rDecOut.u4CbCrOffset),u4YSrcLen,0);
    GFX_SetColCnvFmt(E_YCFMT_422LINEAR,E_SWAP_0,E_VSTD_BT709,E_VSYS_COMP);
    GFX_SetAlpha((UINT32)0xff);
    GFX_ColConv(0,0,_rDecOut.u4Width,_rDecOut.u4Height);
    GFX_Flush();
    GFX_Wait();


    GFX_Unlock();

    copy_to_user( (void __user *)dwJpgTgtPtr ,VIRTUAL(_rDecOut.pu1Addr-u4ARGBSize),u4ARGBSize);
#endif
}

static MT_RESULT_T _MTIMAGE_GetImg(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTIMAGE_HANDLE hHandle;
    FBM_POOL_T *prFbmPool, *prFbmPool2;
    UINT32 u4YAddr, u4CAddr;
    BOOL fgGetARGBThumbail;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;
    ENUM_IMG_ROTATE_PHASE eRotateDegree_thumbnail;

u4MtimageDBGInsert(_mtimageDebugStep, 71);
    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);
    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId = (UCHAR)rArg.ai4Arg[1];
    fgGetARGBThumbail = (BOOL)rArg.ai4Arg[2];
    eRotateDegree_thumbnail = (ENUM_IMG_ROTATE_PHASE)rArg.ai4Arg[3];

    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }

	MTIMAGE_KERNEL_DEBUG("IOO _MTIMAGE_GetImg, fgGetARGBThumbail=[%d]\n", fgGetARGBThumbail);

    if (rImageInfo->eImgDispCM == AYCBCR8888_DIRECT32)//jpglib format.
    {

#if 1//gfx hw yuv=>RGB 888
        _MTIMAGE_IMGYUV2RGB(ucImgId,(unsigned long)hHandle);
#else
        if (!access_ok(VERIFY_WRITE, (void __user *)dwJpgTgtPtr, _rDecOut.u4CbCrOffset))
        {
            MTIMAGE_KERNEL_DEBUG("mtal_ioctl_mplayer argument error\n");
            return MTR_ERR_INV;
        }

        copy_to_user( (void __user *)dwJpgTgtPtr ,VIRTUAL(_rDecOut.pu1Addr),_rDecOut.u4CbCrOffset);
#endif
        rArg.ai4Arg[1] = (INT32)rImageInfo->rDecOut.pu1Addr;
        rArg.ai4Arg[2] = (INT32)rImageInfo->rDecOut.u4CbCrOffset;
        rArg.ai4Arg[3] = (INT32)rImageInfo->rDecOut.u4Pitch;
//        rArg.ai4Arg[4] = (INT32)_rDecOut.u4StartX;
//        rArg.ai4Arg[5] = (INT32)_rDecOut.u4StartY;
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);

        rArg.ai4Arg[4] = (INT32)PAGE_ALIGN(prFbmPool->u4Addr);
        //rArg.ai4Arg[5] = (INT32)(prFbmPoolJPG->u4Size & PAGE_MASK);
        rArg.ai4Arg[5] = (INT32)(0x780000 & PAGE_MASK);


        rArg.ai4Arg[6] = (INT32)rImageInfo->rDecOut.u4Width;
        rArg.ai4Arg[7] = (INT32)rImageInfo->rDecOut.u4Height;

        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_8ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
    }
    else if (rImageInfo->eImgDispCM == Y_CBCR420_SEP16 || rImageInfo->eImgDispCM == Y_CBCR422_SEP16)
	{
		MTIMAGE_DEBUG(" _MTIMAGE_GetImg, DispCM = %d\n", rImageInfo->eImgDispCM);		
		if(rImageInfo->imgFormat == IMAGE_JPEG || rImageInfo->imgFormat == IMAGE_PNG)
		{
			#ifdef CC_FBM_3D_OSD
			prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
			#else
			prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
			#endif
			
			if(ucImgId == 0)
			{
				prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG); 
			}
			else
			{
				prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG); 
			}
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
		
		HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prFbmPool2->u4Addr), rImageInfo->rDecOut.u4Pitch*rImageInfo->rDecOut.u4Height*2);

		rArg.ai4Arg[1] = (INT32)0;
		rArg.ai4Arg[2] = rImageInfo->rDecOut.u4CbCrOffset; //size
		rArg.ai4Arg[3] = (INT32)rImageInfo->rDecOut.u4StartX;
		rArg.ai4Arg[4] = (INT32)rImageInfo->rDecOut.u4StartY;
		rArg.ai4Arg[5] = (INT32)rImageInfo->rDecOut.u4Width;
		rArg.ai4Arg[6] = (INT32)rImageInfo->rDecOut.u4Height;
		//rArg.ai4Arg[7] = (INT32)rImageInfo->rDecOut.u4Pitch;
		
        MTIMAGE_KERNEL_DEBUG(" - GetImg YOffset 0x%c, Sz %x \n", rArg.ai4Arg[1], rArg.ai4Arg[2]);
        MTIMAGE_KERNEL_DEBUG(" - GetImg x y w h [%d, %d, %d, %d]\n", rArg.ai4Arg[3], rArg.ai4Arg[4], rArg.ai4Arg[5], rArg.ai4Arg[6]);
        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_8ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);		
	}
    else
	{
        if(fgGetARGBThumbail)
        {            
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
            //prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
            //prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
    
            FBM_GetFrameBufferAddr(rImageInfo->aucFbgId[rImageInfo->u1CurView],
                                   rImageInfo->aucCurImgFbId[rImageInfo->u1CurView], &u4YAddr, &u4CAddr);
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_GetImg, thumbnail addr[Y=%x, C=%x]\n", u4YAddr, u4CAddr);            
            _MTIMAGE_Thumbnail_rotation(ucImgId, eRotateDegree_thumbnail);
            _MTIMAGE_IMGYUV2RGB(ucImgId, (unsigned long)hHandle);
    
            MTIMAGE_KERNEL_DEBUG("### Get image [src_pitch=%d][des_pitch=%d][des_height=%d]\n",rImageInfo->rDecOut.u4Pitch,rImageInfo->u2ThumbnailSetDecWidth,rImageInfo->u2ThumbnailSetDecHeight);
            GFX_Lock();
            GFX_SetSrc((UINT8 *)(prFbmPool->u4Addr+MT_THUMBNAIL_WORKING_BUFFER_OFFSET), (UINT32)CM_RGB_CLUT8, rImageInfo->rDecOut.u4Pitch*4);
            GFX_SetDst((UINT8 *)prFbmPool->u4Addr, (UINT32)CM_RGB_CLUT8, rImageInfo->u2ThumbnailSetDecWidth*4);
            GFX_BitBlt(0, 0, 0, 0, rImageInfo->u2ThumbnailSetDecWidth*4, rImageInfo->u2ThumbnailSetDecHeight);
            GFX_Flush();
            GFX_Wait();
            GFX_Unlock();
            
			HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prFbmPool->u4Addr), (UINT32)(rImageInfo->u2ThumbnailSetDecWidth*rImageInfo->u2ThumbnailSetDecHeight*4));
			    
            rArg.ai4Arg[1] = prFbmPool->u4Addr - prFbmPool->u4Addr; //addr offset, YUV2RGB to FBM_POOL_TYPE_JPEG buffer
            //rArg.ai4Arg[2] = u4CAddr - u4YAddr; //size, only for ycbcr422 usage
            rArg.ai4Arg[2] = rImageInfo->rDecOut.u4CbCrOffset; //size, only for ycbcr422 usage
        }
        else
		{
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
            //prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
    
            FBM_GetFrameBufferAddr(rImageInfo->aucFbgId[rImageInfo->u1CurView],
                                   rImageInfo->aucCurImgFbId[rImageInfo->u1CurView], &u4YAddr, &u4CAddr);   
    
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_GetImg, video frame addr[Y=%x, C=%x]\n", u4YAddr, u4CAddr);
            rArg.ai4Arg[1] = u4YAddr - prFbmPool->u4Addr; //addr offset to FBM_POOL_TYPE_MPEG buffer
            rArg.ai4Arg[2] = u4CAddr - u4YAddr; //size, only for ycbcr422 usage            
        }            
        
     
        rArg.ai4Arg[3] = (INT32)rImageInfo->rDecOut.u4StartX;
        rArg.ai4Arg[4] = (INT32)rImageInfo->rDecOut.u4StartY;
        //rArg.ai4Arg[5] = (INT32)rImageInfo->rDecOut.u4Width;
        //rArg.ai4Arg[6] = (INT32)rImageInfo->rDecOut.u4Height;
        rArg.ai4Arg[5] = (INT32)_rMt3DCropinfo.u4Width;
        rArg.ai4Arg[6] = (INT32)_rMt3DCropinfo.u4Height;
        MTIMAGE_KERNEL_DEBUG(" - GetImg YOffset 0x%c, Sz %x \n", rArg.ai4Arg[1], rArg.ai4Arg[2]);
        MTIMAGE_KERNEL_DEBUG(" - GetImg x y w h [%d, %d, %d, %d] \n", rArg.ai4Arg[3], rArg.ai4Arg[4], rArg.ai4Arg[5], rArg.ai4Arg[6]);
        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_8ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
    }
u4MtimageDBGInsert(_mtimageDebugStep, 79);    
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

u4MtimageDBGInsert(_mtimageDebugStep, 81);
    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    ucImgId =  (UCHAR)rArg.ai4Arg[1];

    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }
    rImageInfo->u1SlideShowType = (UINT8)rArg.ai4Arg[2];
    rImageInfo->u4SlideShowTime = (UINT32)rArg.ai4Arg[3];

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

u4MtimageDBGInsert(_mtimageDebugStep, 89);
    return MTR_OK;
}



/**clean the img frame buffer
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTIMAGE_CleanFB(unsigned long arg)
{
    MTIMAGE_INFO_T * rImageInfo;
    UINT32 u4GFXYAddr, u4GFXCAddr;
    UCHAR ucFbId;
    
u4MtimageDBGInsert(_mtimageDebugStep, 91);
    rImageInfo = _MTIMAGE_GetImgIdInfo(0);

		if(rImageInfo->fgFBGCreated == 0)
			return MTR_OK;

    ucFbId = VDP_FrcGetLockFrameBuffer((UCHAR)0);
    FBM_GetFrameBufferAddr(rImageInfo->aucFbgId[rImageInfo->u1CurView], ucFbId, &u4GFXYAddr, &u4GFXCAddr);
        
    //FBM_GetFrameBufferAddr(rImageInfo->aucFbgId[rImageInfo->u1CurView], rImageInfo->aucCurDispFbId[rImageInfo->u1CurView], &u4GFXYAddr, &u4GFXCAddr);
    
	if((u4GFXYAddr==0)||(u4GFXCAddr==0))
    {
        MTIMAGE_KERNEL_DEBUG("_MTIMAGE_CleanFB return, Y=%x, C=%x[FBG=%d][FB=%d]\n", u4GFXYAddr, u4GFXCAddr, rImageInfo->aucFbgId[rImageInfo->u1CurView],ucFbId);	    
		return MTR_OK;
    }			

    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_CleanFB, Y=%x, C=%x[FBG=%d][FB=%d]\n", u4GFXYAddr, u4GFXCAddr, rImageInfo->aucFbgId[rImageInfo->u1CurView],ucFbId);
    
    GFX_Lock();
    
    GFX_SetDst((UINT8 *)u4GFXYAddr,CM_RGB_CLUT8, rImageInfo->u4PanelWidth);
    GFX_SetColor(0x0);
    GFX_Fill(0, 0, rImageInfo->u4PanelWidth, rImageInfo->u4PanelHeight);
    
    GFX_SetDst((UINT8 *)u4GFXCAddr,CM_RGB_CLUT8, rImageInfo->u4PanelWidth);
    GFX_SetColor(0x80);
    GFX_Fill(0, 0, rImageInfo->u4PanelWidth, rImageInfo->u4PanelHeight);
        
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();
u4MtimageDBGInsert(_mtimageDebugStep, 99);
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_CleanFB is finished\n=================\n");
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

u4MtimageDBGInsert(_mtimageDebugStep, 101);
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle = (MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prBufFillInfo = (MTIMAGE_BUF_FILLED_T*)rArg.ai4Arg[1];
    ucImgId = (UCHAR) rArg.ai4Arg[2];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prBufFillInfo, MTIMAGE_BUF_FILLED_T);
    COPY_FROM_USER_ARG(prBufFillInfo, rBufFillInfo, MTIMAGE_BUF_FILLED_T);

    if (rBufFillInfo.u4_trsn_id == 1)
    {
        if (u4FillLen1[ucImgId] == 0)
        {
	     //MTIMAGE_KERNEL_DEBUG("@@@@_MTIMAGE_SetBufFilled 1 len=[%x] eof=[%x]\n",rBufFillInfo.u4_filled_len, rBufFillInfo.i4_ret);
            IMG_SetFillBsBufEnd(ucImgId,rBufFillInfo.u4_filled_len, 0,
                                (rBufFillInfo.i4_ret==0)?FALSE:TRUE);   //IMG_MW_FILL_OK
        }
        else
        {
            //MTIMAGE_KERNEL_DEBUG("@@@@_MTIMAGE_SetBufFilled 2 len=[%x, %x] eof=[%x]\n",u4FillLen1[ucImgId], rBufFillInfo.u4_filled_len, rBufFillInfo.i4_ret);
            IMG_SetFillBsBufEnd(ucImgId,u4FillLen1[ucImgId], rBufFillInfo.u4_filled_len,
                                (rBufFillInfo.i4_ret==0)?FALSE:TRUE);   //IMG_MW_FILL_OK
        }
        u4FillLen1[ucImgId] = 0;
    }
    else
    {
        u4FillLen1[ucImgId] = rBufFillInfo.u4_filled_len;
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
    ENUM_IMG_FORMAT	imgFormat;
    UCHAR ucImgId;
    MTIMAGE_INFO_T * rImageInfo ;

u4MtimageDBGInsert(_mtimageDebugStep, 111);
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    imgFormat=(ENUM_IMG_FORMAT)rArg.ai4Arg[0];
    ucImgId=(UCHAR)rArg.ai4Arg[1];
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    if (rImageInfo == NULL)
    {
        return MTR_NOT_OK;
    }
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_ConfigImagedecoder %d\n",ucImgId);

    rImageInfo->imgFormat = imgFormat;  // set format for _MTIMAGE_Open user later



    if (IMG_Config(ucImgId,imgFormat) != E_IMG_OK)
    {
        MTIMAGE_KERNEL_DEBUG("IMG_Config NG\n");
        return MTR_NOT_OK;
    }
u4MtimageDBGInsert(_mtimageDebugStep, 119);        
    return MTR_OK;
}

static MT_RESULT_T _MTIMAGE_SetEnlargement(unsigned long arg)
{
    MTIMAGE_HANDLE hHandle;
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_REGION_T *prEnlargeFactor;
    MTVDO_REGION_T rEnlargeFactor;
	VDP_REGION_T	rRegion;
	VDP_REGION_T 	rCurOutRegion = {0};
	UCHAR rFullRegion;			
	UCHAR ucVdpId = 0;
	UINT32 u4TmpPosX, u4TmpPosY;	
    MTIMAGE_INFO_T * rImageInfo ;	
u4MtimageDBGInsert(_mtimageDebugStep, 121);
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle=(MTIMAGE_HANDLE)rArg.ai4Arg[0];
    prEnlargeFactor = (MTVDO_REGION_T *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prEnlargeFactor, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prEnlargeFactor, rEnlargeFactor, MTVDO_REGION_T);		
    rImageInfo = _MTIMAGE_GetImgIdInfo(0);
	
    if(VDP_GetScalerPixelBased())
    {
        rRegion.u4Width = rEnlargeFactor.u4Width;
        rRegion.u4Height = rEnlargeFactor.u4Height;
        //rRegion.u4X = (MTIMAGE_ENLARGE_OUTPUT_RES_W - rEnlargeFactor.u4Width) >> 1;
        //rRegion.u4Y = (MTIMAGE_ENLARGE_OUTPUT_RES_H - rEnlargeFactor.u4Height) >> 1;
        rRegion.u4X = rEnlargeFactor.u4X;
        rRegion.u4Y = rEnlargeFactor.u4Y;
    }
    else
    {
        rRegion.u4Width = (10000 * rEnlargeFactor.u4Width)/MTIMAGE_ENLARGE_OUTPUT_RES_W;
        rRegion.u4Height = (10000 * rEnlargeFactor.u4Height)/MTIMAGE_ENLARGE_OUTPUT_RES_H;
        //rRegion.u4X = (10000*((MTIMAGE_ENLARGE_OUTPUT_RES_W - rEnlargeFactor.u4Width) >> 1))/MTIMAGE_ENLARGE_OUTPUT_RES_W;
        //rRegion.u4Y = (10000*((MTIMAGE_ENLARGE_OUTPUT_RES_H - rEnlargeFactor.u4Height) >> 1))/MTIMAGE_ENLARGE_OUTPUT_RES_H;
        rRegion.u4X = (10000 * rEnlargeFactor.u4X)/MTIMAGE_ENLARGE_OUTPUT_RES_W;
        rRegion.u4Y = (10000 * rEnlargeFactor.u4Y)/MTIMAGE_ENLARGE_OUTPUT_RES_H;
    }

    if((rArg.ai4Arg[2]==0xffff)&&(rArg.ai4Arg[3]==0xffff))
    {    
        _rImageInfo[0].rDecOut.u4StartX = rRegion.u4X;
        _rImageInfo[0].rDecOut.u4StartY = rRegion.u4Y;         
    }
    else
    {   
        //from MTIMAGE_SetPictureMovePosition 
        _rImageInfo[0].rDecOut.u4StartX = rArg.ai4Arg[2];
        _rImageInfo[0].rDecOut.u4StartY = rArg.ai4Arg[3];
    }

    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_SetEnlargement [x=%d][y=%d][w=%d][h=%d]\n", rRegion.u4X,rRegion.u4Y, rRegion.u4Width, rRegion.u4Height);
    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_SetEnlargement keep X Y for move [x=%d][y=%d]\n",_rImageInfo[0].rDecOut.u4StartX, _rImageInfo[0].rDecOut.u4StartY);

	//new flush mode, reset outregion and recalculate src region
	if(fgNewFlushMode)
	{			
		VDP_GetOutRegion(0, &rFullRegion, &rCurOutRegion);				
		MTIMAGE_KERNEL_DEBUG("_MTIMAGE_SetEnlargement Vdp OUTR [x=%d][y=%d][w=%d][h=%d] ORI OUTR [x=%d][y=%d][w=%d][h=%d]\n",
			rCurOutRegion.u4X, rCurOutRegion.u4Y, rCurOutRegion.u4Width, rCurOutRegion.u4Height,
			_rCurImgRegion.u4X, _rCurImgRegion.u4Y, _rCurImgRegion.u4Width, _rCurImgRegion.u4Height);

		//calculate out region w h
		rCurOutRegion.u4Width = _rCurImgRegion.u4Width *  rImageInfo->u4PanelWidth / rRegion.u4Width; 
		rCurOutRegion.u4Height= _rCurImgRegion.u4Height *  rImageInfo->u4PanelHeight / rRegion.u4Height;				
		MTIMAGE_KERNEL_DEBUG("_MTIMAGE_SetEnlargement w = %d, h = %d\n", rCurOutRegion.u4Width, rCurOutRegion.u4Height);
		//crop to panel width
		rCurOutRegion.u4Width = (rCurOutRegion.u4Width > PANEL_GetPanelWidth() ) ? PANEL_GetPanelWidth()  : rCurOutRegion.u4Width ;
		rCurOutRegion.u4Height = (rCurOutRegion.u4Height > PANEL_GetPanelHeight()) ? PANEL_GetPanelHeight() : rCurOutRegion.u4Height;
		//set region to center
		rCurOutRegion.u4X = (PANEL_GetPanelWidth() - rCurOutRegion.u4Width) >> 1;
		rCurOutRegion.u4Y = (PANEL_GetPanelHeight() - rCurOutRegion.u4Height) >> 1;								
		MTIMAGE_KERNEL_DEBUG("_MTIMAGE_SetEnlargement LAST OUTR [x=%d][y=%d][w=%d][h=%d]\n", 
			rCurOutRegion.u4X, rCurOutRegion.u4Y, rCurOutRegion.u4Width, rCurOutRegion.u4Height);

		//special for PDP panel resolution 1024x1536, vertical / 2
		if((1024 == PANEL_GetPanelWidth()) && (1536 == PANEL_GetPanelHeight()))
		{
			rCurOutRegion.u4Y = rCurOutRegion.u4Y >> 1;
			rCurOutRegion.u4Height = rCurOutRegion.u4Height >> 1;			
		}
		
		VDP_SetOutRegion(ucVdpId, 0, rCurOutRegion);

		//new flush reset x y, set to center position
		u4TmpPosX = (rImageInfo->u4PanelWidth - _rMt3DCropinfo.u4Width) >> 1;				
		u4TmpPosY = (rImageInfo->u4PanelHeight - _rMt3DCropinfo.u4Height) >> 1;		
		rRegion.u4X = (rRegion.u4X >= u4TmpPosX) ? (rRegion.u4X - u4TmpPosX) : 0;			
		rRegion.u4Y = (rRegion.u4Y >= u4TmpPosY) ? (rRegion.u4Y - u4TmpPosY) : 0;
		//KEEP  the value within source width height
		rRegion.u4Width = (rRegion.u4Width > _rMt3DCropinfo.u4Width) ? _rMt3DCropinfo.u4Width : rRegion.u4Width;
		rRegion.u4Height = (rRegion.u4Height > _rMt3DCropinfo.u4Height) ? _rMt3DCropinfo.u4Height : rRegion.u4Height;
		//KEEP x y width height within source SIZE
		u4TmpPosX = _rMt3DCropinfo.u4Width - rRegion.u4Width; 
		u4TmpPosY = _rMt3DCropinfo.u4Height - rRegion.u4Height;
		rRegion.u4X = (rRegion.u4X < u4TmpPosX) ? rRegion.u4X : u4TmpPosX;
		rRegion.u4Y = (rRegion.u4Y < u4TmpPosY) ? rRegion.u4Y : u4TmpPosY;
		
		MTIMAGE_KERNEL_DEBUG("_MTIMAGE_SetEnlargement last src region [x=%d][y=%d][w=%d][h=%d]\n", 
			rRegion.u4X,rRegion.u4Y, rRegion.u4Width, rRegion.u4Height);
	}

    VDP_SetSrcRegion(ucVdpId, 0, rRegion);
u4MtimageDBGInsert(_mtimageDebugStep, 129);
	return MTR_OK;
}

/**Get the active window
*@param region
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
MT_RESULT_T _MTIMAGE_GetImgActiveWindow(MTVDO_REGION_T *region)
{
    
    region->u4X =_rMt3DCropinfo.u4X;
    region->u4Y =_rMt3DCropinfo.u4Y;
    region->u4Width =_rMt3DCropinfo.u4Width;
    region->u4Height =_rMt3DCropinfo.u4Height;

    //printf("@#@_MTIMAGE_GetImgActiveWindow [%d, %d, %d, %d]\n", region->u4X, region->u4Y, region->u4Width, region->u4Height);

    return MTR_OK;
}


static void _MTIMAGE_vdp_stretchblit(SLIDESHOW_FADE_T t_fade)
{
    //INT32 i4Ret = 0;
    RZ_VDO_SCL_PARAM_SET_T rSclParam;
    E_RZ_INOUT_TYPE_T eRzType;
    
    //x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);

    x_memset(&rSclParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));
    

    rSclParam.u4SrcSwap     = 0;
    rSclParam.u4IsRsIn      = 1;
    //1st source vdp Y buffer address
    rSclParam.u4YSrcBase    = PHYSICAL(t_fade.ui4_src2_Y_addr); 
    //1st source vdp Y buffer pitch
    rSclParam.u4YSrcBufLen  = t_fade.ui4_pitch;    
    rSclParam.u4YSrcHOffset = 0;
    rSclParam.u4YSrcVOffset = 0;
    //1st source vdp Y buffer width
    rSclParam.u4YSrcW       = t_fade.ui4_width; 
    //1st source vdp Y buffer height
    rSclParam.u4YSrcH       = t_fade.ui4_height;       
    //1st source vdp C buffer address
    rSclParam.u4CSrcBase    = PHYSICAL(t_fade.ui4_src2_C_addr);       
    rSclParam.u4CSrcHOffset = 0;
    rSclParam.u4CSrcVOffset = 0;
    //1st source vdp C buffer width (422 may be equals to Y)
    rSclParam.u4CSrcW       = t_fade.ui4_width;       
    //1st source vdp C buffer height (422 may be equals to Y)
    rSclParam.u4CSrcH       = t_fade.ui4_height;       

    rSclParam.u4OutSwap     = 0;
    rSclParam.u4IsRsOut     = 1;
    rSclParam.u4OutMode     = 1;    //422 video format output
    rSclParam.u4IsVdo2Osd   = 0;
    //destination Y buffer address
    rSclParam.u4YTgBase     = PHYSICAL(t_fade.ui4_dest_Y_addr);       
    rSclParam.u4YTgCM       = 0;    //useful when u4IsVdo2Osd==1
    //destination Y buffer pitch
    rSclParam.u4YTgBufLen   = t_fade.ui4_pitch;      
    rSclParam.u4YTgHOffset  = 0;
    rSclParam.u4YTgVOffset  = 0;
    //destination Y buffer width
    rSclParam.u4YTgW        = t_fade.ui4_width;
    //destination Y buffer height
    rSclParam.u4YTgH        = t_fade.ui4_height;     
    //destination C buffer address
    rSclParam.u4CTgBase     = PHYSICAL(t_fade.ui4_dest_C_addr); 
    //destination C buffer width    (422 may be equals to Y)
    rSclParam.u4CTgW        = t_fade.ui4_width;     
    //destination C buffer height   (422 may be equals to Y)
    rSclParam.u4CTgH        = t_fade.ui4_height;       
    rSclParam.u4VdoCbCrSwap = 0;  
    
    /* blending level, 0~127, pixel percetage of 1nd cource VDP buffer
         (1st buffer pixel)*rSclParam.u4ABlend + 
         (2nd buffer pixel)*(0x80-rSclParam.u4ABlend)  */
    rSclParam.u4ABlend      = t_fade.ui4_fade_level;      
    //2nd source vdp Y buffer                                
    rSclParam.u4PreLoadYAddr = PHYSICAL(t_fade.ui4_src1_Y_addr);  
    //2nd source vdp C buffer
    rSclParam.u4PreLoadCAddr = PHYSICAL(t_fade.ui4_src1_C_addr);     
    
    eRzType = E_RZ_INOUT_TYPE_VDOMD;

    IMGRZ_Lock_Ex(E_FIRST_RESIZER);
    IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
    IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, eRzType);
    IMGRZ_Scale_Ex(E_FIRST_RESIZER, &rSclParam);
    IMGRZ_Flush_Ex(E_FIRST_RESIZER);
    IMGRZ_Wait_Ex(E_FIRST_RESIZER);
    IMGRZ_Unlock_Ex(E_FIRST_RESIZER);

    //x_sema_unlock(hMt53fbMutex);        

    //return i4Ret;

}


static void _MTIMAGE_Thumbnail_rotation(UCHAR ucImgId, ENUM_IMG_ROTATE_PHASE eRotateDegree_thumbnail)
{
    IMG_BUF_PARAM_T rCmd={0};
    MTIMAGE_INFO_T * rImageInfo ;
    FBM_POOL_T *prFbmPool;
    
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
    
    rCmd.eDispColorMode = rImageInfo->eImgDispCM;    
    rCmd.rSrcRegion.pu1Addr = rImageInfo->rDecOut.pu1Addr;
    rCmd.rSrcRegion.u4CbCrOffset = rImageInfo->rDecOut.u4CbCrOffset;
    rCmd.rSrcRegion.u4Depth = rImageInfo->rDecOut.u4Depth;
    rCmd.rSrcRegion.u4Pitch = rImageInfo->rDecOut.u4Pitch;
    rCmd.rSrcRegion.u4TotalLine = rImageInfo->rDecOut.u4TotalLine;
    rCmd.rSrcRegion.u4StartX = rImageInfo->rDecOut.u4StartX;
    rCmd.rSrcRegion.u4StartY = rImageInfo->rDecOut.u4StartY;
    rCmd.rSrcRegion.u4Width = rImageInfo->rDecOut.u4Width;
    rCmd.rSrcRegion.u4Height = rImageInfo->rDecOut.u4Height;
    
    MTIMAGE_KERNEL_DEBUG("[%d][%x][%d][%d][%d][%d][%d][%d][%d][%d]\n", 
                rCmd.eDispColorMode, rCmd.rSrcRegion.pu1Addr, rCmd.rSrcRegion.u4CbCrOffset, rCmd.rSrcRegion.u4Depth, rCmd.rSrcRegion.u4Pitch,
                rCmd.rSrcRegion.u4TotalLine, rCmd.rSrcRegion.u4StartX, rCmd.rSrcRegion.u4StartY, rCmd.rSrcRegion.u4Width, rCmd.rSrcRegion.u4Height);

    rCmd.eRotateDegree = eRotateDegree_thumbnail;
    rCmd.rDstRegion.u4StartX = 0;
    rCmd.rDstRegion.u4StartY = 0;
    rCmd.rDstRegion.u4Width = rImageInfo->u2ThumbnailSetDecWidth;   //generally equals to 176x176
    rCmd.rDstRegion.u4Height = rImageInfo->u2ThumbnailSetDecHeight;
    rCmd.rDstRegion.u4Depth = rImageInfo->rDecOut.u4Depth;
    rCmd.bKeepRatio = TRUE;
    rCmd.bKeepOriResolution = FALSE;

    rCmd.rDstRegion.u4Pitch = rImageInfo->rDecOut.u4Pitch;
    rCmd.rDstRegion.u4TotalLine = rImageInfo->rDecOut.u4Height;
    rCmd.rDstRegion.pu1Addr = (UINT8*)(prFbmPool->u4Addr+MT_THUMBNAIL_VDO_ROTATION_BUFFER_OFFSET);
    rCmd.rDstRegion.u4CbCrOffset = MT_THUMBNAIL_VDO_DECODING_CBCR_OFFSET;


    MTIMAGE_KERNEL_DEBUG("[%d][%d][%d][%d][%d][%d][%x]\n", 
                rCmd.eRotateDegree, rCmd.rDstRegion.u4Width, rCmd.rDstRegion.u4Height, rCmd.rDstRegion.u4Depth, 
                rCmd.rDstRegion.u4Pitch, rCmd.rDstRegion.u4TotalLine, rCmd.rDstRegion.pu1Addr);
    
    IMG_Set(rCmd, &rImageInfo->rDecOut);    
}

#define HW_RESOURCE_CTRL_LOCK       0
#define HW_RESOURCE_CTRL_UNLOCK     1
#define HW_RESOURCE_CTRL_INIT       2
static MT_RESULT_T _MTIMAGE_HWResouceCtrl(UCHAR ucImgId, UCHAR action)
{
    MTIMAGE_INFO_T * rImageInfo = NULL;
    static char *action_str[] = {"lock", "unlock", "init"};
    rImageInfo = _MTIMAGE_GetImgIdInfo(ucImgId);

    MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: ucImgId=%d, action=%s, sema=0x%x\n", ucImgId, action_str[action], rImageInfo->hHwResouceSema);
    /*Unlock*/
    if(HW_RESOURCE_CTRL_UNLOCK == action)
    {
        if(x_sema_unlock(rImageInfo->hHwResouceSema)== OSR_OK)
        {
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: un_lock OK!!\n");
            return MTR_OK;
        }
    }
    /*lock*/
    else if(HW_RESOURCE_CTRL_LOCK == action)
    {
        if(x_sema_lock(rImageInfo->hHwResouceSema, X_SEMA_OPTION_WAIT)== OSR_OK)
        {
            MTIMAGE_KERNEL_DEBUG("_MTIMAGE_HWResouceCtrl: lock OK!!\n");
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
                return MTR_OK;
            }
        }
        else
        {
            printk("_MTIMAGE_HWResouceCtrl: already inited!!\n");
            return MTR_OK;			
        }
    }
    else
    {
        printk("_MTIMAGE_HWResouceCtrl unknow action: ucImgId=%d, action=%d, sema=0x%x\n", ucImgId, action, rImageInfo->hHwResouceSema);
        return MTR_NOT_OK;
    }    
    printk("_MTIMAGE_HWResouceCtrl return ERROR: ucImgId=%d, action=%s, sema=0x%x\n", ucImgId, action_str[action], rImageInfo->hHwResouceSema);
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

/*This init fuction is called from file:\vm_linux\chiling\driver\linux\native\mt53xx_fb\mt53_gal.c function:MT_GAL_GALInit*/
MT_RESULT_T _MTIMAGE_InitHWResouce(void)
{
    MT_RESULT_T ret = MTR_OK;
    if(MTR_OK != (ret = _MTIMAGE_HWResouceCtrl(0, HW_RESOURCE_CTRL_INIT)))
    {
        printk("_MTIMAGE_InitHWResouce 0 return %d\n", ret);
        return ret;
    }
    if(MTR_OK != (ret = _MTIMAGE_HWResouceCtrl(1, HW_RESOURCE_CTRL_INIT)))
    {
        printk("_MTIMAGE_InitHWResouce 1 return %d\n", ret);
        return ret;
    }
    return MTR_OK;
}

#ifdef IMG_DUMP
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/param.h>

static INT32 _IMG_DumpData(const UINT8 *buf, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
{
	mm_segment_t oldfs;
	struct file *filep;
	size_t ret, count;
	char fileName[256];

	memset(fileName, 0, 256 * sizeof(char));

	sprintf(fileName, "/tmp/IMG_K_%ld_%ld_%ld.bin", u4Width, u4Height, u4Pitch);
	count = u4Pitch * u4Height;
	
	printk("[DUMP]filename=%s, buf=0x%x,count=0x%x\n", fileName, (UINT32)buf, (UINT32)count);

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	filep = filp_open(fileName, (O_CREAT | O_WRONLY | O_APPEND), 0);
	if (IS_ERR(filep))
	{
		set_fs(oldfs);
		printk("[DUMP] filp_open error %x!\n", filep);
		return -1;
	}
	ret = filep->f_op->write(filep, (VOID*)buf, count, &filep->f_pos);
	if (ret != count)
	{
		printk("[DUMP] f_op->write error ret=%x!\n", ret);
		return -1;
	}
	printk("[DUMP]buf=0x%x,size=0x%x, ret=0x%x\n", (UINT32)buf, (UINT32)count, (UINT32)ret);
//	x_thread_delay(300);	
	filp_close(filep, 0);
	set_fs(oldfs);

	return 0;
}
#endif

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
    case MTAL_IO_IMAGE_ENLARGEMENT:
        return _MTIMAGE_SetEnlargement(arg);        
    case MTAL_IO_IMAGE_SET_DISP_PATH:
        return _MTIMAGE_SetDisplayPath(arg);
    case MTAL_IO_IMAGE_RESOUCE_CTL:
        return _MTIMAGE_HWResouceCtrl_Wrapper(arg);        
    default:
        return  MTR_NOT_OK;
    }
    return MTR_OK;
}

