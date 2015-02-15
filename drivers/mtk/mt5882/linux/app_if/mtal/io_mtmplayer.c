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
 * $RCSfile: io_mtmplayer.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/
/** @file io_mtmplayer.c
 */

/********************************************************************
  INCLUDE FILES
********************************************************************/
#include "mtmplayer.h"
#include "mttsrec.h"
#include "mtdmx.h"
#include "playmgr_if.h"
#include "swdmx_if.h"
#include "swdmx_drvif.h"
#include "img_lib_if.h"
#include "cb_data.h"
#include "feeder_if.h"
//#include "osd_if.h"
#include "osd_drvif.h"
#include "jpg_if.h"
#include "fbm_drvif.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"
#include "srm_drvif.h"
#include "b2r_drvif.h"
#include "mtal_mtmplayer_slide.h"
#ifdef _EN_SBTL_
#include "mtal_sbtl_if.h"
#endif
#include <linux/mtal_ioctl.h>
#if defined(CC_MT5387) || defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395) || defined(SWDMX_IS_POST_MT5368)
#include "b2r_drvif.h"
#include "b2r_if.h"
#endif
/********************************************************************
  MACRO CONSTANT DEFINITIONS
********************************************************************/

/********************************************************************
  TYPE DEFINITION
********************************************************************/
/*
typedef struct _MTMPLAYER_BUF_INFO_T
{
    UINT32  u4StartAddr;
    UINT32  u4EndAddr;
    UINT32  u4ReadAddr;
    UINT32  u4WriteAddr;
    UINT32  u4LastReadAddr;
    UINT32  u4LastReadSize;
    UINT32  u4FreeSize;
    UINT32  u4BytesInBuf;
    UINT32  u4Size;
    BOOL    fgRingBuf;
} MTMPLAYER_BUF_INFO_T;
*/
typedef struct _MTMPLAYER_SP_DISP_T
{
    MTMPLAYER_IMGPARAM_T rImgSetting;
    MTVDO_REGION_T rRegion;
    MTMPLAYER_PIXFORMAT_T eFormatType;
    UINT32 u4Buffer;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4Stride;    
    BOOL fgSpDisp;    
    BOOL fgPartial;
} MTMPLAYER_SP_DISP_T;

/********************************************************************
  VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/
#define ALIGN16(X) (((X) + 0xF) & (~0xF))	///< for imgrz pitch alignment
#define MTMPLAYER_SYNC2SDIMG    
#define MTMPLAYER_PLAYMGR0 (0)
#define MTMPLAYER_SWDMX0 (0)
#define MTMPLAYER_COPY_WITH_LIMIT(dest, src, src_size, dest_size) \
    if ((dest == NULL) || (src == NULL)) \
        printk("MTMPLAYER_COPY_WITH_LIMIT with null dest/src\n"); \
    else if(src_size < dest_size) \
    	x_memcpy((void *)dest, (void *)src, src_size); \
    else \
    	x_memcpy((void *)dest, (void *)src, dest_size); \

#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
   {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))                            \
    {                                                       \
        printk("mtal_ioctl_mplayer 3 arguments error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))                        \
    {                                                       \
        printk("mtal_ioctl_mplayer 3 arguments error\n");      \
        return MTR_ERR_INV;                                          \
    }        

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");      \
        return MTR_ERR_INV;                                          \
    }        

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_WRITE, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mplayer argument error\n");      \
        return MTR_ERR_INV;                                          \
    }    

 #ifdef MTMPLAYER_SET_PRINT
 #undef MTMPLAYER_SET_PRINT
 #endif
 #define MTMPLAYER_SET_PRINT(func, setting)    	do {\
                    										if(IS_MTMPLAYER_LOG_ENABLE)	\
                    										{printf("%s->u2VTrack=0x%x\n",func,(setting)->u2VTrack);	\
                    										printf("%s->u2ATrack=0x%x\n",func,(setting)->u2ATrack);	\
                    										printf("%s->u2CapTrack=0x%x\n",func,(setting)->u2CapTrack);	\
                    										printf("%s->i4Speed=%d\n",func,(setting)->i4Speed);	\
                    										printf("\n");}\
                    									} while (0)

 #ifdef MTMPLAYER_STATUS_PRINT
 #undef MTMPLAYER_STATUS_PRINT
 #endif
#define MTMPLAYER_STATUS_PRINT(func, status)    	do {\
                    										if(IS_MTMPLAYER_LOG_ENABLE)	\
                    										{printf("%s->eMediaType=%d\n",func,(status)->eMediaType);	\
                    										printf("%s->eVidFormat=%d\n",func,(status)->eVidFormat);	\
                    										printf("%s->eAudFormat=%d\n",func,(status)->eAudFormat);	\
                    										printf("%s->eCapFormat=%d\n",func,(status)->eCapFormat);	\
                    										printf("%s->u2VideoTrackNum=0x%x\n",func,(status)->u2VideoTrackNum);	\
                    										printf("%s->u2AudioTrackNum=0x%x\n",func,(status)->u2AudioTrackNum);	\
                    										printf("%s->u2CaptionTrackNum=0x%x\n",func,(status)->u2CaptionTrackNum);	\
                    										printf("%s->u4TotalDuration=0x%x\n",func,(status)->u4TotalDuration);	\
                    										printf("%s->u8FileSize=0x%08x08%x\n",func,(INT32)((status)->u8FileSize >> 32),(INT32)((status)->u8FileSize));	\
                    										printf("%s->u4PlayCap=0x%x\n",func,(status)->u4PlayCap);	\
                    										printf("%s->u4CurPos=0x%x\n",func,(status)->u4CurPos);	\
                    										printf("\n");}\
                    									} while (0)
 #ifdef MTVDO_REGION_PRINT
 #undef MTVDO_REGION_PRINT
 #endif
#define MTVDO_REGION_PRINT(func,region)            do {\
                    										if(IS_MTMPLAYER_LOG_ENABLE)	\
                    										{printf("%s.u4X=0x%x\n",func,(region).u4X);	\
                    										printf("%s.u4Y=0x%x\n",func,(region).u4Y);	\
                    										printf("%s.u4Width=0x%x\n",func,(region).u4Width);	\
                    										printf("%s.u4Height=0x%x\n",func,(region).u4Height);	\
                    										printf("\n");}\
                    									} while (0)
 #ifdef MTMPLAYER_IMGPARAM_PRINT
 #undef MTMPLAYER_IMGPARAM_PRINT
 #endif
#define MTMPLAYER_IMGPARAM_PRINT(func, Imgsetting)    	do {\
                    										if(IS_MTMPLAYER_LOG_ENABLE)	\
                    										{MTVDO_REGION_PRINT("rDstRegion",(Imgsetting)->rDstRegion); \
                    										printf("%s->eRotation=%d\n",func,(Imgsetting)->eRotation);	\
                    										printf("%s->u1PlaneID=%d\n",func,(Imgsetting)->u1PlaneId);	\
                    										printf("\n");}\
                   									} while (0)


#define MTMPLAYER_CHKINSTANCE(instance)         if (((UINT32)instance >= (UINT32)MTMPLAYER_MAX) ||  \
                                                    (ahHandle[instance] != NULL))   \
                                                {                                      \
                                                    MTMPLAYER_PRINT(" - instance BUSY\n"); \
                                                    return MTR_NOT_OK;                 \
                                                }                                      \
                                                ahHandle[instance] = *phHandle
static MTMPLAYER_SP_DISP_T rSpDisp;                    										
static MTMPLAYER_HANDLE ahHandle[MTMPLAYER_MAX] = {NULL, NULL};  
static ENUM_IMG_DISP_COLOR_MODE _eImgDispCM;
static ENUM_IMG_ROTATE_PHASE _eRotateDegree;
static IMG_REGION_T _rDecOut;
static UINT32 _u4DisplayRegionId, _u4PlaneId;
static UINT32 _u4ImgOutput;

static UINT8 _u1DecErr = 0;	// 1: error, 2: not support, 3: stop
static BOOL _fgStartDec = FALSE;
static BOOL _fgVdpConnect = FALSE;;

#ifdef ENABLE_MULTIMEDIA
static BOOL _fgPlaymgrOpen = FALSE;
static CHAR _acFileName[1024];
static MTMPLAYER_SRCTYPE _eSrcType = MTMPLAYER_SRCTYPE_HIGH_SPEED_STORAGE;
#endif
//MT_MPLAYER_COLOR_FORMAT_ARGB8888
static UINT16       dwJpgTgtWidth =0;
static UINT16       dwJpgTgtHeight =0;
static UINT8*       dwJpgTgtPtr ;
static MTMPLAYER_IMGCOLORFORMAT_T uJpgColorFmt=0;

extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

static BOOL _MTMPLAYER_DisplayImg(MTMPLAYER_IMGPARAM_T *prImgSetting, 
                                            MTVDO_REGION_T *prRegion, BOOL fgPartialSrc);
static MT_RESULT_T _MTMPLAYER_ConvertSP(UINT32 u4Buffer, UINT32 u4Width, 
                                                   UINT32 u4Height, UINT32 u4Stride, 
                                                   MTMPLAYER_PIXFORMAT_T eFormatType);
void _MTMPLAYER_IMGNotify(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{
    MM_CALLBACK_INFO_T rCallbackInfo;

    rCallbackInfo.rMMNotify.u4SrcType = (UINT32)FEEDER_SOURCE_TOTAL;
    
	if(u4State == JPG_HW_NFY_FINISHED)
	{
        _fgStartDec = FALSE;
		//_InjectVideoPlane();
		rCallbackInfo.rMMNotify.u4Condition = MT_MPLAYER_CALLBACK_FINISH;
		rCallbackInfo.rMMNotify.u4Param = (UINT32)pv_tag;
		if (rSpDisp.fgSpDisp)
		{
		   _MTMPLAYER_DisplayImg(&rSpDisp.rImgSetting, &rSpDisp.rRegion, rSpDisp.fgPartial); 
		   _MTMPLAYER_ConvertSP(rSpDisp.u4Buffer, rSpDisp.u4Width, rSpDisp.u4Height,
		                        rSpDisp.u4Stride, rSpDisp.eFormatType);
		}
	}
	else if(u4State == JPG_HW_NFY_ERROR)
	{
        _fgStartDec = FALSE;	
        if((UINT32)pv_data == IMG_DRV_NOT_SUPPORT)
        {
            _u1DecErr = 2;
            rCallbackInfo.rMMNotify.u4Param = MT_MPLAYER_FILE_ERROR;
        }
        else if ((UINT32)pv_data == IMG_DRV_ERR_COMPONENT)
		{
			//code sync
			_u1DecErr = 2;
            rCallbackInfo.rMMNotify.u4Param = MT_MPLAYER_JPEG_COLOR_FORMAT_ERROR;
		}        
        else	//eState==IMG_DRV_HW_ERROR
        {
            _u1DecErr = 1;
            rCallbackInfo.rMMNotify.u4Param = MT_MPLAYER_IMG_ERROR;
        }
        rCallbackInfo.rMMNotify.u4Condition = MT_MPLAYER_CALLBACK_ERROR;
    }	
	else
	{
	    return;
    }
	        
    _CB_PutEvent(CB_MTMPLAYER_TRIGGER, sizeof(MM_CALLBACK_INFO_T), &rCallbackInfo);	
}

/**Initialize jpeg hw for decoding.
*@param eColorFMT            display color format
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static BOOL _MTMPLAYER_OpenImg(MTMPLAYER_HANDLE hHandle, MTMPLAYER_IMGCOLORFORMAT_T eColorFMT)
{    
    ENUM_IMG_BUFFER_TYPE eImgBufType;
    eImgBufType = (eColorFMT == MT_MPLAYER_COLOR_FORMAT_AYCBCR8888)? RING_IN_SINGLE_OUT: RING_IN_DOUBLE_OUT;
    _eImgDispCM = (eColorFMT == MT_MPLAYER_COLOR_FORMAT_AYCBCR8888)? AYCBCR8888_DIRECT32: Y_CBCR422_SEP16;


    IMG_Lib_Init();

    if(IMG_GetDecState(0) == IMG_RELEASE)
    {
        if(IMG_Init(eImgBufType, hHandle, 0, _MTMPLAYER_IMGNotify) != E_IMG_OK)
        {
            return FALSE;
        }
        _eRotateDegree = CLOCKWISE_0;	//reset setting
        x_memset((void *)&_rDecOut, 0, sizeof(IMG_REGION_T));
        _u1DecErr = 0;
    }
    SRM_SetMmMode(SRM_MM_MODE_TYPE_JPEG);
    return TRUE;
}

/**Close jpeg hw
*@param
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static BOOL _MTMPLAYER_CloseImg(VOID)
{
    if (_eImgDispCM == Y_CBCR422_SEP16)
    {/*
        if (_fgVdpConnect)
        {
            //VDO
            VDP_Image_Disconnect();
            _fgVdpConnect = FALSE;
        }*/
    }
    else
    {
        OSD_PLA_Enable(_u4PlaneId, FALSE);
        OSD_RGN_Delete(_u4DisplayRegionId);
    }
    
#ifndef MTMPLAYER_SYNC2SDIMG    
    IMG_Release(0);
#endif
    SRM_SetMmMode(SRM_MM_MODE_TYPE_OFF);
    _u1DecErr = 0;
//MT_MPLAYER_COLOR_FORMAT_ARGB8888
     dwJpgTgtWidth =0;
    dwJpgTgtHeight =0;
    dwJpgTgtPtr =NULL;
    uJpgColorFmt=0;

    return TRUE;
}

/**Stop jpeg processing
*@param 
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static BOOL _MTMPLAYER_StopImg(VOID)
{
    _fgStartDec = FALSE;
    
//#ifndef MTMPLAYER_SYNC2SDIMG  
  //  _u1DecErr = 3;
    while(IMG_GetDecState(0) == IMG_DECODE)
	{
		 x_thread_delay(1);
	}

	if(IMG_GetDecState(0) !=IMG_RELEASE)
	{
		IMG_Stop(0);
		IMG_SetFillBsBufEnd(0,0, 0, TRUE);  
		IMG_Release(0);
	}
//#endif    

    _u1DecErr = 0;
    return TRUE;
}

/**Set image parameters for display.
*@param prImgSetting          Image setting for display
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static BOOL _MTMPLAYER_DisplayImg(MTMPLAYER_IMGPARAM_T *prImgSetting, 
                                            MTVDO_REGION_T *prRegion, BOOL fgPartialSrc)
{
    IMG_BUF_PARAM_T rCmd={0};
    IMG_VP_INFO_T rVpInfo;
    DRV_OSD_T rOsd;
    FBM_POOL_T *prFbmPoolJPG;
    UINT32 u4List;
    IMG_REGION_T    rDstRegion;

    x_memset(&rCmd, 0, sizeof(IMG_BUF_PARAM_T));
    rCmd.eDispColorMode = _eImgDispCM;
	rCmd.eRotateDegree = prImgSetting->eRotation;
	rCmd.rSrcRegion.pu1Addr = _rDecOut.pu1Addr;
	rCmd.rSrcRegion.u4CbCrOffset = _rDecOut.u4CbCrOffset;
	rCmd.rSrcRegion.u4Depth = _rDecOut.u4Depth;
	rCmd.rSrcRegion.u4Pitch = _rDecOut.u4Pitch;
	rCmd.rSrcRegion.u4TotalLine = _rDecOut.u4TotalLine;
	rCmd.rSrcRegion.u4StartX = (fgPartialSrc) ? prRegion->u4X : _rDecOut.u4StartX;
	rCmd.rSrcRegion.u4StartY = (fgPartialSrc) ? prRegion->u4Y : _rDecOut.u4StartY;
	rCmd.rSrcRegion.u4Width = (fgPartialSrc) ? prRegion->u4Width : _rDecOut.u4Width;
	rCmd.rSrcRegion.u4Height = (fgPartialSrc) ? prRegion->u4Height : _rDecOut.u4Height;
    
	rCmd.rDstRegion.u4StartX = prImgSetting->rDstRegion.u4X;
	rCmd.rDstRegion.u4StartY = prImgSetting->rDstRegion.u4Y;
	rCmd.rDstRegion.u4Width = prImgSetting->rDstRegion.u4Width;
	rCmd.rDstRegion.u4Height = prImgSetting->rDstRegion.u4Height;
	rCmd.rDstRegion.u4Depth = _rDecOut.u4Depth;		
	rCmd.bKeepRatio = TRUE;

    _u4PlaneId = (UINT32)prImgSetting->u1PlaneId;
	
    if (_eImgDispCM == Y_CBCR422_SEP16)
    {
        if (prImgSetting->fgDisplay && (!_fgVdpConnect))
        {
            if (IMG_VDP_Connect(_u4PlaneId) != E_IMG_OK)
            {
	        	return FALSE;
    	    } 
            else
            {
                _fgVdpConnect = TRUE;
            }
        }
        if(IMG_GetVideoPlaneInfo(&rVpInfo) != E_IMG_OK)
    	{
	    	return FALSE;
    	}
        rCmd.rDstRegion.u4Pitch = ALIGN16(rVpInfo.u4Width);	
        rCmd.rDstRegion.u4TotalLine = rVpInfo.u4Height;
    	rCmd.rDstRegion.pu1Addr = rVpInfo.pu1BackFrameAddr[0];
	    rCmd.rDstRegion.u4CbCrOffset = (UINT32)rVpInfo.pu1BackFrameAddr[1] - 
		    (UINT32)rVpInfo.pu1BackFrameAddr[0];
	    if (!prImgSetting->fgClean)
	    {
	        x_memcpy((void*)VIRTUAL((UINT32)rVpInfo.pu1BackFrameAddr[0]), 
		        (void*)VIRTUAL((UINT32)rVpInfo.pu1CurFrameAddr[0]),
        		((UINT32)rVpInfo.pu1CurFrameAddr[0] < (UINT32)rVpInfo.pu1BackFrameAddr[0])?
		        ((UINT32)rVpInfo.pu1BackFrameAddr[0] - (UINT32)rVpInfo.pu1CurFrameAddr[0]):
        		((UINT32)rVpInfo.pu1CurFrameAddr[0] - (UINT32)rVpInfo.pu1BackFrameAddr[0]));
	    }
    }
    else                            //osd
    {
        rOsd.u4Width = rCmd.rDstRegion.u4Width;
        rOsd.u4Height = rCmd.rDstRegion.u4Height;
        rOsd.u4ScaleWidth = rCmd.rDstRegion.u4Width;
        rOsd.u4ScaleHeight = rCmd.rDstRegion.u4Height;
        rOsd.u4Left = rCmd.rDstRegion.u4StartX;
        rOsd.u4Top = rCmd.rDstRegion.u4StartY;
        
        rCmd.rDstRegion.u4Pitch = ALIGN16(rCmd.rDstRegion.u4Width * _rDecOut.u4Depth);	        


	//prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_B2R);   //use b2r buffer for display on osd  
	prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);


	    if (prFbmPoolJPG == NULL)
	    {
	        return FALSE;
	    }
        //rOsd.pu1FrameBaseAddr = BSP_AllocAlignedDmaMemory(rCmd.rDstRegion.u4Pitch * rOsd.u4Height, 16);
        rOsd.pu1FrameBaseAddr = (UINT8 *)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
       // OSD_Init();
        OSD_RGN_Create(&_u4DisplayRegionId, rOsd.u4Width, rOsd.u4Height, (void*)rOsd.pu1FrameBaseAddr,
                       OSD_CM_AYCBCR8888_DIRECT32, rCmd.rDstRegion.u4Pitch, rOsd.u4Left, rOsd.u4Top,
                       rOsd.u4Width, rOsd.u4Height);
        OSD_RGN_LIST_Create(&u4List);
        OSD_RGN_Insert(_u4DisplayRegionId, u4List);
        OSD_PLA_FlipTo(_u4PlaneId, u4List);  

        rCmd.rDstRegion.u4TotalLine = prImgSetting->rDstRegion.u4Height;
        rCmd.rDstRegion.pu1Addr = rOsd.pu1FrameBaseAddr;//_rDisplayOSD.pu1FrameBaseAddr;
	    rCmd.rDstRegion.u4CbCrOffset = 0;

    }
    _u4ImgOutput = (UINT32)rCmd.rDstRegion.pu1Addr;
	
    IMG_Set(rCmd, &rDstRegion);

    if (_eImgDispCM == Y_CBCR422_SEP16)
    {
        if (prImgSetting->fgDisplay)
        {
	        if(IMG_Flip() != E_IMG_OK)
        	{
	        	return FALSE;
    	    }
        }
    }
    else                //OSD
    {
        OSD_PLA_Enable(_u4PlaneId, TRUE);
    }
    return TRUE;
}

/**Start jpeg decoding
*@param pu1Buf                Input Buffer address of jpeg
*@param u4ReadSize            Size of the current jpeg segment
*@param fgEOF                 Eof is in this segment
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static BOOL _MTMPLAYER_DecImg(UINT8 *pu1Buf, UINT32 u4ReadSize, BOOL fgEOF)
{
    IMG_PARAM_T rInfo={0};
    IMG_LIB_FILL_RING_BUF_T rStreamBufInfo;
    IMG_VP_INFO_T rVpInfo;
    UINT32 u4Entry1FillSz, u4Entry2FillSz;
    UINT32 u4BufEmpty;

    //return if decoding error occur
	if(_u1DecErr == 1)
	{
		return FALSE;
	}
	else if(_u1DecErr == 2)
	{	
		return FALSE;
	}
    //fill buffer
    //query available buffer size and info
    
    x_memset((void *)&rStreamBufInfo, 0, sizeof(IMG_LIB_FILL_RING_BUF_T));
    #if 0
    while((IMG_GetBSBufInfo(&rStreamBufInfo) < u4ReadSize) && (_u1DecErr==0));	//wait for enough space
    #else
    do {

		if (IMG_GetDecState(0)==IMG_RELEASE) //may be finished, //code sync
		{
			if (_u1DecErr == 0)
			{
				return TRUE;
			}
			return FALSE;
		}

        u4BufEmpty = IMG_GetBSBufInfo(0,&rStreamBufInfo);
		if(u4BufEmpty<u4ReadSize)
		{
			x_thread_delay(5);
		}
    }while((u4BufEmpty < u4ReadSize) && (_u1DecErr==0));	//wait for enough space
    #endif
    
    if(rStreamBufInfo.rEntry1.bValid)
    {
        if(rStreamBufInfo.rEntry1.u4RequiredLen < u4ReadSize)
        {
            u4Entry1FillSz = rStreamBufInfo.rEntry1.u4RequiredLen;
            u4Entry2FillSz = u4ReadSize - rStreamBufInfo.rEntry1.u4RequiredLen;
        }
        else
        {
            u4Entry1FillSz = u4ReadSize;
            u4Entry2FillSz = 0;
        }
    }
    else
    {
        u4Entry1FillSz = 0;
        u4Entry2FillSz = u4ReadSize;
    }
    // Copy user space argument
    if(rStreamBufInfo.rEntry1.pvStartAddr)
    {		
        if(copy_from_user((void *)VIRTUAL((UINT32)rStreamBufInfo.rEntry1.pvStartAddr), 
    					(void __user *)pu1Buf, u4Entry1FillSz))
        {
            printk("PLAYMGR_DecImg copy_from_user error\n");
            return FALSE;
        }
    }
    if(rStreamBufInfo.rEntry2.pvStartAddr)
    {
        if(copy_from_user((void *)VIRTUAL((UINT32)rStreamBufInfo.rEntry2.pvStartAddr), 
                           (void __user *)(pu1Buf + u4Entry1FillSz), u4Entry2FillSz))
        {
            printk("PLAYMGR_DecImg copy_from_user error\n");
            return FALSE;
        }
    }
    IMG_SetFillBsBufEnd(0,u4Entry1FillSz, u4Entry2FillSz, fgEOF/*(u4ReadSize < BS_PKT_SIZE)?TRUE:FALSE*/);

	
    //decode
    if((IMG_GetDecState(0)==IMG_INIT) && (_fgStartDec == FALSE))
    {
        if((IMG_GetBSBufInfo(0,&rStreamBufInfo) < u4ReadSize) || fgEOF/*(u4ReadSize < BS_PKT_SIZE)*/)
        {
            _fgStartDec = TRUE;
            if (_eImgDispCM == Y_CBCR422_SEP16)
            {
				ENUM_IMG_RET ret;
			//MT_MPLAYER_COLOR_FORMAT_ARGB8888
	         if (uJpgColorFmt != MT_MPLAYER_COLOR_FORMAT_ARGB8888)//jpglib format.
	         {
			 	ret = IMG_GetVideoPlaneInfo(&rVpInfo);
	                if(ret!= E_IMG_OK)
	                {
            			printk("PLAYMGR_DecImg VideoPlaneInfo error=%d\n", ret);
	                    return FALSE;
	                }	
	         }	
            }
            else
            {
                //DRV_OSD_RGN_Create(&_rDisplayOSD, &_u4DisplayRegionId);
            }
            rInfo.eFormat = IMAGE_JPEG;
            rInfo.pu1InBuffer = NULL;		//use internal ring buffer
            rInfo.u8BufSize = 0;			//use internal ring buffer
            rInfo.rDstRegion.u4StartX = 0;
            rInfo.rDstRegion.u4StartY = 0;
            #ifdef CC_JPG_USE_VIDEO_PLANE_DEMO_PROGRAM
            	rInfo.rDstRegion.u4Width = 1366;		//rVpInfo.u4Width;
            	rInfo.rDstRegion.u4Height = 768;	//rVpInfo.u4Height;	            
            #else
            	rInfo.rDstRegion.u4Width = dwJpgTgtWidth;	//rVpInfo.u4Width;
            	rInfo.rDstRegion.u4Height = dwJpgTgtHeight;	//rVpInfo.u4Height;            
            #endif
            
            rInfo.rDstRegion.u4Pitch = 0;	//rVpInfo.u4Width;

            rInfo.eDispColorMode = _eImgDispCM;
            rInfo.eRotateDegree = CLOCKWISE_0;
            rInfo.bKeepRatio = TRUE;
            rInfo.prDecOut = &_rDecOut;

            rInfo.rDstRegion.pu1Addr = NULL;
            rInfo.rDstRegion.u4CbCrOffset = 0;

            if(IMG_Decode(0,rInfo) != E_IMG_OK)
            {
		printk("PLAYMGR_DecImg NG\n");
                return FALSE;
            }
        }
    }

    if (!_u1DecErr)
    {
        return TRUE;
    }
    else
    {
	printk("PLAYMGR_DecImg NG.2\n");
        return FALSE;
    }
}


static VOID _MTMPLAYER_GetPlaymgrInfo(MTMPLAYER_STATUS_T *pMplayer, PLAYMGR_STATUS_T *pPlaymgr)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(pMplayer);
    UNUSED(pPlaymgr);
    
    return;
#else
    // ToDo: need to specify track id.
    if((pMplayer == NULL) || (pPlaymgr == NULL))
    {
        return;
    }
        
#ifdef _EN_DRM_
    pMplayer->eDRMType = PLAYMGR_DRMTYPE_NONE;
    pMplayer->u4DRMRentalLimit = 0;
    pMplayer->u4DRMRentalCount = 0;
#endif
    
    switch (pPlaymgr->eMediaType)
    {        
    case PLAYMGR_MEDIA_TYPE_AVI:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_AVI;
#ifdef _EN_DRM_
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_GetDivxDRMStatus(MTMPLAYER_PLAYMGR0, (PLAYMGR_DRMTYPE *)&pMplayer->eDRMType, &pMplayer->u4DRMRentalLimit, &pMplayer->u4DRMRentalCount);
#else
        PLAYMGR_GetDivxDRMStatus((PLAYMGR_DRMTYPE *)&pMplayer->eDRMType, &pMplayer->u4DRMRentalLimit, &pMplayer->u4DRMRentalCount);
#endif
#endif
        break;
    case PLAYMGR_MEDIA_TYPE_DIVX:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_DIVX;
#ifdef _EN_DRM_
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_GetDivxDRMStatus(MTMPLAYER_PLAYMGR0, (PLAYMGR_DRMTYPE *)&pMplayer->eDRMType, &pMplayer->u4DRMRentalLimit, &pMplayer->u4DRMRentalCount);
#else
        PLAYMGR_GetDivxDRMStatus((PLAYMGR_DRMTYPE *)&pMplayer->eDRMType, &pMplayer->u4DRMRentalLimit, &pMplayer->u4DRMRentalCount);
#endif
#endif
        break;
    case PLAYMGR_MEDIA_TYPE_MPEG2_PS:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MPEG2_PS;
        break;
    case PLAYMGR_MEDIA_TYPE_MPEG2_TS:
    case PLAYMGR_MEDIA_TYPE_MPEG2_TS_192:
    case PLAYMGR_MEDIA_TYPE_MPEG2_TS_ZERO_192:
    case PLAYMGR_MEDIA_TYPE_MPEG2_TS_TIME_SHIFT:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MPEG2_TS;
        break;
    case PLAYMGR_MEDIA_TYPE_MPEG2_ES:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MPEG2_ES;
        break;
    case PLAYMGR_MEDIA_TYPE_H264_ES:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_H264_ES;
        break;
    case PLAYMGR_MEDIA_TYPE_MPEG4_ES:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MPEG4_ES;
        break;
    case PLAYMGR_MEDIA_TYPE_VC1_ES:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_VC1_ES;
            break;
    case PLAYMGR_MEDIA_TYPE_VC1_WMV:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_ASF;
        break;
    case PLAYMGR_MEDIA_TYPE_VCD:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_VCD;
        break;
    case PLAYMGR_MEDIA_TYPE_VOB:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_VOB;
        break;
    case PLAYMGR_MEDIA_TYPE_MP3:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MP3;
        break;
    case PLAYMGR_MEDIA_TYPE_WAVE:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_WAVE;
        break;
    case PLAYMGR_MEDIA_TYPE_AIF:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_AIF;
        break;
    case PLAYMGR_MEDIA_TYPE_AC3:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_AC3;
        break;
    case PLAYMGR_MEDIA_TYPE_AAC:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_AAC;
        break;
    case PLAYMGR_MEDIA_TYPE_MPA:
    case PLAYMGR_MEDIA_TYPE_MP2:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MPA;
        break;	
    case PLAYMGR_MEDIA_TYPE_MP4:
		pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MP4;
		break;
    case PLAYMGR_MEDIA_TYPE_MKV:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_MKV;
        break;
    case PLAYMGR_MEDIA_TYPE_RM:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_RM;
        break;
	case PLAYMGR_MEDIA_TYPE_FLV:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_FLV;
		break;
    default:
        pMplayer->eMediaType = MT_MPLAYER_MEDIA_TYPE_UNKNOWN;
        break;
    }

    switch (pPlaymgr->aeVidFormat[0])
    {
    case PLAYMGR_VDEC_MPEG1_MPEG2:
        pMplayer->eVidFormat = MTVDEC_FMT_MPEG1_MPEG2;
        break;
    case PLAYMGR_VDEC_MPEG4: 
	case PLAYMGR_VDEC_DIVX311:
	case PLAYMGR_VDEC_H263:	
        pMplayer->eVidFormat = MTVDEC_FMT_MPEG4;
        break;
    case PLAYMGR_VDEC_H264:
        pMplayer->eVidFormat = MTVDEC_FMT_H264;
        break;
    case PLAYMGR_VDEC_VC1:
	case PLAYMGR_VDEC_WMV7:
	case PLAYMGR_VDEC_WMV8:
	case PLAYMGR_VDEC_WMV9:
        pMplayer->eVidFormat = MTVDEC_FMT_VC1;
        break;	
    case PLAYMGR_VDEC_RV8:
    case PLAYMGR_VDEC_RV9:
        pMplayer->eVidFormat = MTVDEC_FMT_RV;
        break;	
	case PLAYMGR_VDEC_VP8:
        pMplayer->eVidFormat = MTVDEC_FMT_VP8;
		break;
	case PLAYMGR_VDEC_VP6:
        pMplayer->eVidFormat = MTVDEC_FMT_VP6;
		break;
    case PLAYMGR_VDEC_MJPEG:
        pMplayer->eVidFormat = MTVDEC_FMT_MJPEG;
		break;
    case PLAYMGR_VDEC_H265:
        pMplayer->eVidFormat = MTVDEC_FMT_H265;
		break;
    default:   
        pMplayer->eVidFormat = MTVDEC_FMT_UNKNOWN;
        break;
    }

    switch (pPlaymgr->aeAudFormat[0])
    {        
    case PLAYMGR_ADEC_MPEG:
        pMplayer->eAudFormat = MTAUDDEC_FMT_MPEG;
        break;
    case PLAYMGR_ADEC_AC3: 
        pMplayer->eAudFormat = MTAUDDEC_FMT_AC3;
        break;
    case PLAYMGR_ADEC_PCM: 
    case PLAYMGR_ADEC_ADPCM:
        pMplayer->eAudFormat = MTAUDDEC_FMT_PCM;
        break;
    case PLAYMGR_ADEC_MP3:    
        pMplayer->eAudFormat = MTAUDDEC_FMT_MP3;
        break;
    case PLAYMGR_ADEC_AAC:  
        pMplayer->eAudFormat = MTAUDDEC_FMT_AAC;
        break;
    case PLAYMGR_ADEC_DTS: 
        pMplayer->eAudFormat = MTAUDDEC_FMT_DTS;
        break;
    case PLAYMGR_ADEC_WMA:    
        pMplayer->eAudFormat = MTAUDDEC_FMT_WMA;
        break;
	case PLAYMGR_ADEC_RA: 
		pMplayer->eAudFormat = MTAUDDEC_FMT_RA;
        break;
    case PLAYMGR_ADEC_HDCD:   
		pMplayer->eAudFormat = MTAUDDEC_FMT_HDCD;
        break;
    case PLAYMGR_ADEC_MLP:    
		pMplayer->eAudFormat = MTAUDDEC_FMT_MLP;
        break;
    case PLAYMGR_ADEC_MTS:    
		pMplayer->eAudFormat = MTAUDDEC_FMT_MTS;
        break;
    case PLAYMGR_ADEC_A2:    
		pMplayer->eAudFormat = MTAUDDEC_FMT_A2;
        break;
    case PLAYMGR_ADEC_NICAM: 
		pMplayer->eAudFormat = MTAUDDEC_FMT_NICAM;
        break;
    default:
        pMplayer->eAudFormat = MTAUDDEC_FMT_UNKNOWN;
        break;
    }

	switch (pPlaymgr->aeCapFormat[0])
    {
    case PLAYMGR_CAPTION_MPEG2:
		pMplayer->eCapFormat = MTMPLAYER_CAPTION_MPEG2;
		break;
	case PLAYMGR_CAPTION_AVI_TEXT:
		pMplayer->eCapFormat = MTMPLAYER_CAPTION_AVI_TEXT;
		break;
	case PLAYMGR_CAPTION_AVI_BITMAP:
		pMplayer->eCapFormat = MTMPLAYER_CAPTION_AVI_BITMAP;
		break;
	default:
		break;
	}
    
    pMplayer->u2VideoTrackNum = pPlaymgr->u2VideoTracks;
    pMplayer->u2AudioTrackNum = pPlaymgr->u2AudioTracks;
    pMplayer->u2PcrTrackNum = pPlaymgr->u2PcrTracks;
	pMplayer->u2CaptionTrackNum = pPlaymgr->u2CaptionTracks;
	pMplayer->u2VideoId= pPlaymgr->au4VideoId[0];
	pMplayer->u2AudioId= pPlaymgr->au4AudioId[0];
	pMplayer->u2CapTrackId = pPlaymgr->au4CapTrackId[0];
	pMplayer->u4TotalDuration = pPlaymgr->u4TotalDuration;
	pMplayer->u8FileSize= pPlaymgr->u8FileSize;
	pMplayer->u4PlayCap = pPlaymgr->u4PlayCap;
	pMplayer->u4CurPos = pPlaymgr->u4CurPos;
	pMplayer->u4CurTick = pPlaymgr->u4CurTick;
	pMplayer->u8CurBytePos = pPlaymgr->u8CurBytePos;
	pMplayer->i4Speed = pPlaymgr->i4Speed;

#ifdef _EN_SBTL_
	pMplayer->u2ExtSbtlNum = u2MtalSbtlGetTrackNs();
#else
	pMplayer->u2ExtSbtlNum = 0;
#endif
#endif  // ENABLE_MULTIMEDIA
}


static MTMPLAYER_INSTANCE_T _MTMPLAYER_GetInstance(MTMPLAYER_HANDLE hHandle)
{

        
    if (hHandle == ahHandle[MTMPLAYER_IMAGE])
    {
        return MTMPLAYER_IMAGE;
    }
    else if (hHandle == ahHandle[MTMPLAYER_PLAYMGR])
    {
        return MTMPLAYER_PLAYMGR;
    }
    else if (hHandle == ahHandle[MTMPLAYER_AUDIO])
    {
        return MTMPLAYER_AUDIO;
    }

    return MTMPLAYER_MAX;
}

                    										
/**Initialize MPlayer
*@param MPlayer Handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Open(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_3ARG_T rArg;
    MTMPLAYER_HANDLE* phHandle=NULL;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;
    CHAR* pcFileName = NULL;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    phHandle=(MTMPLAYER_HANDLE*)rArg.ai4Arg[0];
    eInstance = (MTMPLAYER_INSTANCE_T)rArg.ai4Arg[1];
    pcFileName = (UCHAR*)rArg.ai4Arg[2];

    MTMPLAYER_PRINT(" - void _MTMPLAYER_Open[%x][%x]\n", *phHandle, eInstance);
   //add driver implementation code here
    if (eInstance == MTMPLAYER_IMAGE)
    {
        MTMPLAYER_CHKINSTANCE(eInstance);
#ifdef CC_53XX_SWDMX_V2
        FeederSetInfo(FEEDER_VIDEO_SOURCE, FEEDER_MM_USE, (UINT32)FALSE);
#else
        FeederSetInfo(FEEDER_MM_USE, (UINT32)FALSE);
#endif
        i4Ret = FeederOpen(FEEDER_VIDEO_SOURCE);
        x_memset((VOID *)&rSpDisp, 0x0, sizeof(MTMPLAYER_SP_DISP_T));
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        MTMPLAYER_CHKINSTANCE(eInstance);
#ifdef CC_53XX_SWDMX_V2
        FeederSetInfo(FEEDER_AUDIO_SOURCE, FEEDER_MM_USE, (UINT32)FALSE);
#else
        FeederSetInfo(FEEDER_MM_USE, (UINT32)FALSE);
#endif
        i4Ret = FeederOpen(FEEDER_AUDIO_SOURCE);
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_SetAudTime(MTMPLAYER_PLAYMGR0, PLAYMGR_AUDTIME_STOP, 0);
#else
        PLAYMGR_SetAudTime(PLAYMGR_AUDTIME_STOP, 0);
#endif
    }
    else
    {
        MTMPLAYER_CHKINSTANCE(eInstance);
        if (!PLAYMGR_Init())
        {
            i4Ret = 1;
        }
#ifdef CC_53XX_SWDMX_V2
		PLAYMGR_FeederReset(MTMPLAYER_PLAYMGR0);
#endif
		_fgPlaymgrOpen = FALSE;
    }    

    if (i4Ret)
    {
        if((UINT32)eInstance < (UINT32)MTMPLAYER_MAX)
        {
            ahHandle[eInstance] = NULL;
        }
        return MTR_NOT_OK;
    }

    if ((_eSrcType == MTMPLAYER_SRCTYPE_HIGH_SPEED_STORAGE)||
		(_eSrcType == MTMPLAYER_SRCTYPE_NETWORK_FM)||
		(_eSrcType == MTMPLAYER_SRCTYPE_NETWORK_RHAPSODY)||
		(_eSrcType == MTMPLAYER_SRCTYPE_NETWORK_XUNLEI))

    {
        if (pcFileName != NULL)
        {
            if (copy_from_user((void *)_acFileName, (void __user *)pcFileName, 100))
            {
                printk("copy from user error!!!!!!!!\n");
                return MTR_NOT_OK;
            }
        }
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Terminate Mplayer
*@param MPlayer Handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Close(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    UNUSED(_MTMPLAYER_CloseImg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void _MTMPLAYER_Close\n");

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);    

    if (eInstance == MTMPLAYER_IMAGE)
    {
        _MTMPLAYER_CloseImg();
        i4Ret = FeederClose(FEEDER_VIDEO_SOURCE);
    }
    else if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Close(MTMPLAYER_PLAYMGR0))
#else
        if (!PLAYMGR_Close())
#endif
        {
            i4Ret = 1;
        }    
		_fgPlaymgrOpen = FALSE;		
#ifdef _EN_SBTL_
        if (fgIsMtalSbtlExisted())
        {
            fgMtalSbtlTextClose();
        }
#endif
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        i4Ret = FeederClose(FEEDER_AUDIO_SOURCE);
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }

    ahHandle[eInstance] = NULL;
	_eSrcType = MTMPLAYER_SRCTYPE_HIGH_SPEED_STORAGE;
    
    if (i4Ret)
    {
        return MTR_NOT_OK;
    }    

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Reset Mplayer
*@param MPlayer Handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Reset(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void\n");

 //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_Reset(MTMPLAYER_PLAYMGR0);
#else
        PLAYMGR_Reset();
#endif
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Set the stream settings to MPlayer
*@param MPlayer Handle
*@param pSettings
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Set(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    UNUSED(_MTMPLAYER_OpenImg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_SETTING_T* prSettings = NULL, rSettings;
    PLAYMGR_SETTING_T rPlaySetting;
    MTMPLAYER_INSTANCE_T eInstance;
    //INT32 i4Ret = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    prSettings=(MTMPLAYER_SETTING_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prSettings, MTMPLAYER_SETTING_T);
    COPY_FROM_USER_ARG(prSettings, rSettings, MTMPLAYER_SETTING_T);
    
    MTMPLAYER_PRINT(" - void\n");
    //MTMPLAYER_SET_PRINT("prSettings",prSettings);

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);    

    if (eInstance == MTMPLAYER_IMAGE)
    {
        _MTMPLAYER_OpenImg(hHandle, rSettings.eColorFormat);
        //MT_MPLAYER_COLOR_FORMAT_ARGB8888
        dwJpgTgtWidth = rSettings.u2VTrack;
        dwJpgTgtHeight = rSettings.u2ATrack;
        dwJpgTgtPtr = rSettings.pu1SubtitleBuffer;
        uJpgColorFmt = rSettings.eColorFormat;
    }
    else if (eInstance == MTMPLAYER_PLAYMGR)
    {
        x_memset((VOID*)&rPlaySetting, 0, sizeof(PLAYMGR_SETTING_T));
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_VIDEO)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_VIDEO;
            rPlaySetting.u2PlayVidTrack = rSettings.u2VTrack;
            rPlaySetting.fgVidEnable= rSettings.fgVidEn;
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_AUDIO)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_AUDIO;
            rPlaySetting.u2PlayAudTrack = rSettings.u2ATrack;
            rPlaySetting.fgAudEnable= rSettings.fgAudEn;
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_NETFLIX_STREAM)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_NETFLIX_STREAM;
            rPlaySetting.u1NetflixVideoIdx = rSettings.u1NetflixVideoIdx;
            rPlaySetting.u1NetflixAudioIdx = rSettings.u1NetflixAudioIdx;
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_CAPTRACK)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_CAPTRACK;
            rPlaySetting.pu1SubtitleBuffer = rSettings.pu1SubtitleBuffer;
            rPlaySetting.u2PlaySubTrack = rSettings.u2CapTrack;
            rPlaySetting.fgSubEnable = rSettings.fgCapEn;
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_EXTSBTL)
        {
#ifdef _EN_SBTL_
            vMtalSbtlTextBufInit(rSettings.pu1SubtitleBuffer);
            vMtalSbtlTextSet(MST_MASK_ENABLE | MST_MASK_SYNC | MST_MASK_TRK_NO, rSettings.fgExtSbtlEn, rSettings.u2ExtSbtlTrack, rSettings.i1ExtSbtlSync);
#endif
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_SPEED)
        {
#ifdef CC_53XX_SWDMX_V2
            if (!PLAYMGR_Speed(MTMPLAYER_PLAYMGR0, rSettings.i4Speed))
#else
            if (!PLAYMGR_Speed(rSettings.i4Speed))
#endif
            {
                return MTR_NOT_OK;
            }
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_PVR_VIDEO)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_PVR_VIDEO;
            rPlaySetting.u2PlayVidTrack = 0;
            rPlaySetting.fgVidEnable= rSettings.fgVidEn;
            rPlaySetting.u2VideoPid = rSettings.u2VPid;
            rPlaySetting.u2VideoPidIdx = rSettings.u2VPidIdx;
            rPlaySetting.fgVidDecrypt = rSettings.fgVidDecrypt;
            rPlaySetting.u1VideoKeyIdx = rSettings.u1VideoKeyIdx;
            if(rSettings.eVdecFmt == MTVDEC_FMT_MPEG1_MPEG2)
            {
                rPlaySetting.eVdecFmt = PLAYMGR_VDEC_MPEG1_MPEG2;
            }
            else if(rSettings.eVdecFmt == MTVDEC_FMT_H264)
            {
                rPlaySetting.eVdecFmt = PLAYMGR_VDEC_H264;
            }
            else
            {
                MTMPLAYER_PRINT(" - unknow eVdecFmt %d\n", (UINT32)rSettings.eAdecFmt);
            }
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_PVR_AUDIO)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_PVR_AUDIO;
            rPlaySetting.u2PlayAudTrack = 0;
            rPlaySetting.fgAudEnable= rSettings.fgAudEn;
            rPlaySetting.u2AudioPid = rSettings.u2APid;
            rPlaySetting.u2AudioPidIdx = rSettings.u2APidIdx;
            rPlaySetting.fgAudDecrypt = rSettings.fgAudDecrypt;
            rPlaySetting.u1AudioKeyIdx = rSettings.u1AudioKeyIdx;
            if(rSettings.eAdecFmt == MTAUDDEC_FMT_MPEG)
            {
                rPlaySetting.eAdecFmt = PLAYMGR_ADEC_MPEG;
            }
            else if(rSettings.eAdecFmt == MTAUDDEC_FMT_AC3)
            {
                rPlaySetting.eAdecFmt = PLAYMGR_ADEC_AC3;
            }
            else if(rSettings.eAdecFmt == MTAUDDEC_FMT_AAC)
            {
                rPlaySetting.eAdecFmt = PLAYMGR_ADEC_AAC;
            }
            else if(rSettings.eAdecFmt == MTAUDDEC_FMT_DTS)
            {
                rPlaySetting.eAdecFmt = PLAYMGR_ADEC_DTS;
            }
            else if(rSettings.eAdecFmt == MTAUDDEC_FMT_MP3)
            {
                rPlaySetting.eAdecFmt = PLAYMGR_ADEC_MP3;
            }
            else
            {
                MTMPLAYER_PRINT(" - unknow eAdecFmt %d\n", (UINT32)rSettings.eAdecFmt);
            }
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_PVR_PCR)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_PVR_PCR;
            rPlaySetting.u2PlayPcrTrack = 0;
            rPlaySetting.fgPcrEnable= rSettings.fgPcrEn;
            rPlaySetting.u2PcrPid = rSettings.u2PcrPid;
            rPlaySetting.u2PcrPidIdx = rSettings.u2PcrPidIdx;
            #ifdef MTDMX_PID_INSRC_TIMESHIFT_PLAY
            if(!MTDMX_SetPCR(rSettings.u2PcrPidIdx,
                rSettings.u2PcrPid,
                MTDMX_PID_INSRC_TIMESHIFT_PLAY,
                rSettings.fgPcrEn))
            {
                MTMPLAYER_PRINT(" - MTDMX_SetPCR Error \n");
                return MTR_NOT_OK;
            }
            #else
            printf(" - MTDMX_SetPCR undefine\n");
            #endif
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_PVR_CTRL_BLK)
        {
            #ifdef TIME_SHIFT_SUPPORT
            UCHAR *pvStart = rSettings.rTickCtl.pv_start_address;
            UCHAR *pvEnd = rSettings.rTickCtl.pv_end_address;
            #endif
            //FEEDER_BUF_INFO_T rBufInfo;

            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_PVR_CTRL_BLK;
            x_memset((VOID*)&rPlaySetting.rTickCtl, 0, sizeof(PLAYMGR_TICK_CTRL_BLK_T));
        
            #if 0
            // ToDo
            // wait for Eddie's new device API
            FeederGetBufferInfo(FEEDER_PARSING_SOURCE, (FEEDER_BUF_INFO_T *)&rBufInfo);
        
            //rPlaySetting.rTickCtl.pv_start_address = rSettings.rTickCtl.pv_start_address;
            //rPlaySetting.rTickCtl.pv_end_address = rSettings.rTickCtl.pv_end_address;

            rPlaySetting.rTickCtl.pv_start_address =
                rBufInfo.u4StartAddr;
            rPlaySetting.rTickCtl.pv_end_address = 
                rBufInfo.u4StartAddr + rBufInfo.u4Size;
            // ~wait
            #else
            #ifdef TIME_SHIFT_SUPPORT
            {
                extern void MTTSREC_GetBufferAddr(MTTSREC_BUFFERPTR_T *prBufAddr);
                MTTSREC_BUFFERPTR_T rBuf = {0};
                MTTSREC_GetBufferAddr(&rBuf);
                if(rBuf.u4MetaAddr == 0)
                {
                    MTMPLAYER_PRINT(" - u4MetaAddr = 0\n");
                    return MTR_NOT_OK;
                }
                rPlaySetting.rTickCtl.pv_start_address =
                    (void *)rBuf.u4MetaAddr;
                rPlaySetting.rTickCtl.pv_end_address =
                    (void *)(rBuf.u4MetaAddr + (UINT32)(pvEnd - pvStart));
            }
            #else
            printf("undefine MTTSREC_BUFFERPTR_T fail\n");
            #endif
            #endif
    
            rPlaySetting.rTickCtl.ui4_entry_num = rSettings.rTickCtl.ui4_entry_num;
            rPlaySetting.rTickCtl.ui4_entry_size = rSettings.rTickCtl.ui4_entry_size;
            rPlaySetting.rTickCtl.ui4_tick_period = rSettings.rTickCtl.ui4_tick_period;
            rPlaySetting.rTickCtl.ui4_lba_init = rSettings.rTickCtl.ui4_lba_init;
            rPlaySetting.rTickCtl.ui8_fifo_offset = rSettings.rTickCtl.ui8_fifo_offset;
            rPlaySetting.rTickCtl.ui4_fifo_pkt_num = rSettings.rTickCtl.ui4_fifo_pkt_num;
        }
        if(rSettings.u4InfoMask & MTMPLAYER_MASK_PVR_REC_RANGE)
        {
            rPlaySetting.u4InfoMask |= PLAYMGR_INFO_MASK_PVR_REC_RANGE;
            x_memset((VOID*)&rPlaySetting.rRangeInfo, 0, sizeof(PLAYMGR_REC_BUF_RANGE_INFO_T));
            rPlaySetting.rRangeInfo.t_valid.ui4_start = rSettings.rRangeInfo.t_valid.ui4_start;
            rPlaySetting.rRangeInfo.t_valid.ui4_end = rSettings.rRangeInfo.t_valid.ui4_end;
            rPlaySetting.rRangeInfo.t_safe.ui4_start = rSettings.rRangeInfo.t_safe.ui4_start;
            rPlaySetting.rRangeInfo.t_safe.ui4_end = rSettings.rRangeInfo.t_safe.ui4_end;
        }
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Set(MTMPLAYER_PLAYMGR0, &rPlaySetting))
#else
        if (!PLAYMGR_Set(&rPlaySetting))
#endif
        {
            return MTR_NOT_OK;
        }
    }
	else if  (eInstance == MTMPLAYER_AUDIO)
	{
	}
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }    

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Start Play.
*@param MPlayer Handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Start(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void\n");

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Start(MTMPLAYER_PLAYMGR0))
#else
        if (!PLAYMGR_Start())
#endif
        {
            i4Ret = 1;
        }
#ifdef _EN_SBTL_
        if (fgIsMtalSbtlEnabled())  // automatically play the external subtitle
        {
            fgMtalSbtlTextOpen();
        }
#endif
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        FeederSetAppQueryMode(FEEDER_AUDIO_SOURCE, FEEDER_PUSH_MODE);        
        i4Ret = FeederStart(FEEDER_AUDIO_SOURCE);
        FeederSetRing(FEEDER_AUDIO_SOURCE, TRUE);
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_SetAudTime(MTMPLAYER_PLAYMGR0, PLAYMGR_AUDTIME_PLAY, 0);
#else
        PLAYMGR_SetAudTime(PLAYMGR_AUDTIME_PLAY, 0);
#endif
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }    

    if (i4Ret)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Stop MPlayer.
*@param handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Stop(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    UNUSED(_MTMPLAYER_StopImg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void\n");

    //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);    

    if (eInstance == MTMPLAYER_IMAGE)
    {
        _MTMPLAYER_StopImg();
        i4Ret = 0;
    }
    else if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Stop(MTMPLAYER_PLAYMGR0))
#else
        if (!PLAYMGR_Stop())
#endif
        {
            i4Ret = 1;
        }
#ifdef _EN_SBTL_
        if (fgIsMtalSbtlEnabled())
        {
            fgMtalSbtlTextStop();
        }
#endif
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        i4Ret = FeederStop(FEEDER_AUDIO_SOURCE);
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_SetAudTime(MTMPLAYER_PLAYMGR0, PLAYMGR_AUDTIME_STOP, 0);
#else
        PLAYMGR_SetAudTime(PLAYMGR_AUDTIME_STOP, 0);
#endif
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }       

    if (i4Ret)
    {
        return MTR_NOT_OK;
    }    

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Abort MPlayer.
*@param handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Abort(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void\n");

    //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);    

    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Abort(MTMPLAYER_PLAYMGR0))
#else
        if (!PLAYMGR_Abort())
#endif
        {
            i4Ret = 1;
        }
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }       

    if (i4Ret)
    {
        return MTR_NOT_OK;
    }    

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}

/**Pause MPlayer.
*@param handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Pause(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void\n");

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Pause(MTMPLAYER_PLAYMGR0))
#else
        if (!PLAYMGR_Pause())
#endif
        {
            i4Ret = 1;
        }
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        i4Ret = FeederPause(FEEDER_AUDIO_SOURCE);
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_SetAudTime(MTMPLAYER_PLAYMGR0, PLAYMGR_AUDTIME_STOP, 0);
#else
        PLAYMGR_SetAudTime(PLAYMGR_AUDTIME_STOP, 0);
#endif
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }        

    if (i4Ret)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Resume MPlayer.
*@param handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Resume(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4Ret = 0;

    hHandle=(MTMPLAYER_HANDLE)arg;
    MTMPLAYER_PRINT(" - void\n");

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Resume(MTMPLAYER_PLAYMGR0))
#else
        if (!PLAYMGR_Resume())
#endif
        {
            i4Ret = 1;
        }
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        i4Ret = FeederResume(FEEDER_AUDIO_SOURCE);
#ifdef CC_53XX_SWDMX_V2
        PLAYMGR_SetAudTime(MTMPLAYER_PLAYMGR0, PLAYMGR_AUDTIME_PLAY, 0);
#else
        PLAYMGR_SetAudTime(PLAYMGR_AUDTIME_PLAY, 0);
#endif
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }            

    if (i4Ret)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Get the stream settings and status from MPlayer
*@param handle
*@param prSettings
*@param prStatus
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_GetInfo(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    UNUSED(_MTMPLAYER_GetPlaymgrInfo);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_STATUS_T *prStatus=NULL;
    MTMPLAYER_STATUS_T   rStatus;
    PLAYMGR_STATUS_T rPlaymgrStatus;
    MT_RESULT_T eResult = MTR_OK;
    MTMPLAYER_INSTANCE_T eInstance;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    prStatus = (MTMPLAYER_STATUS_T *)rArg.ai4Arg[1];

 //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_IMAGE)
    {
        rStatus.eMediaType = MT_MPLAYER_MEDIA_TYPE_JPEG;
        eResult = MTR_OK;
    }
    else if (eInstance == MTMPLAYER_PLAYMGR)
    {
    	if (!_fgPlaymgrOpen)
    	{
#ifdef CC_53XX_SWDMX_V2
        	if (!PLAYMGR_Open(MTMPLAYER_PLAYMGR0, _acFileName))
#else
        	if (!PLAYMGR_Open(_acFileName))
#endif
        	{
                MTMPLAYER_PRINT(" - PLAYMGR_Open fail \n");
                return MTR_NOT_OK;
        	} 
			else
			{
				_fgPlaymgrOpen = TRUE;
			}
    	}
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_GetInfo(MTMPLAYER_PLAYMGR0, (PLAYMGR_STATUS_T *)&rPlaymgrStatus))
#else
        if (!PLAYMGR_GetInfo((PLAYMGR_STATUS_T *)&rPlaymgrStatus))
#endif
        {
            MTMPLAYER_PRINT(" - PLAYMGR_GetInfo fail \n");
            return MTR_NOT_OK;
        }
		_MTMPLAYER_GetPlaymgrInfo(&rStatus, &rPlaymgrStatus);
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_GetAudInfo(MTMPLAYER_PLAYMGR0, (PLAYMGR_STATUS_T *)&rPlaymgrStatus))
#else
        if (!PLAYMGR_GetAudInfo((PLAYMGR_STATUS_T *)&rPlaymgrStatus))
#endif
        {
            eResult = MTR_NOT_OK;
            MTMPLAYER_PRINT(" - PLAYMGR_GetAudInfo fail \n");
            return MTR_NOT_OK;
        }
		_MTMPLAYER_GetPlaymgrInfo(&rStatus, &rPlaymgrStatus);
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    } 
    USR_SPACE_ACCESS_VALIDATE_ARG(prStatus,MTMPLAYER_STATUS_T);
    COPY_TO_USER_ARG(prStatus,rStatus,MTMPLAYER_STATUS_T);

    MTMPLAYER_PRINT(" - void\n");
    //MTMPLAYER_STATUS_PRINT("rStatus",&rStatus);

    return eResult;
#endif  // ENABLE_MULTIMEDIA
}


/**Register the callback function of MPlayer
 MPlayer can uses the registered callback function to notify the upper layer.
*@param handle
*@param eCallbackType
*@param callback funtion pointer
*@param reserved
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_RegCb(unsigned long arg)
{
#if 0
    MTAL_IOCTL_4ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_CALLBACK_TYPE_T eCallbackType;
    MTMPLAYER_CALLBACK fnCallback;
    UINT32 u4Param;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle=(MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    eCallbackType=(MTMPLAYER_CALLBACK_TYPE_T)rArg.ai4Arg[1];
    fnCallback=(MTMPLAYER_CALLBACK)rArg.ai4Arg[2];
    u4Param=(UINT32)rArg.ai4Arg[3];

    MTMPLAYER_PRINT(" - eCallbackType = %d, u4Param = %d\n",
		eCallbackType,
		u4Param);

 //add driver implementation code here
#endif
    return MTR_OK;
}


/**Set the seek position by time.
*@param handle
*@param seek time(millisecond)
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Seek(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    UINT32 u4SeekTime;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    u4SeekTime=(UINT32)rArg.ai4Arg[1];

    MTMPLAYER_PRINT(" - u4SeekTime = %u\n", (int)u4SeekTime);

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if(!PLAYMGR_SeekTime(MTMPLAYER_PLAYMGR0, u4SeekTime))
#else
        if(!PLAYMGR_SeekTime(u4SeekTime))
#endif
        {
            MTMPLAYER_PRINT(" - PLAYMGR_SeekTime %d Error \n", u4SeekTime);
            return MTR_NOT_OK;
        }
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
#ifdef CC_53XX_SWDMX_V2
        if(!PLAYMGR_SetAudTime(MTMPLAYER_PLAYMGR0, PLAYMGR_AUDTIME_SEEK, u4SeekTime))
#else
        if(!PLAYMGR_SetAudTime(PLAYMGR_AUDTIME_SEEK, u4SeekTime))
#endif
        {
            MTMPLAYER_PRINT(" - PLAYMGR_SetAudTime %d Error \n", u4SeekTime);
            return MTR_NOT_OK;
        }
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }               

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Set the seek position by time.
*@param handle
*@param seek position (byte)
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SeekPos(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_3ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    UINT64 u8SeekPosition;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    u8SeekPosition = (UINT32)rArg.ai4Arg[1];
    u8SeekPosition = (u8SeekPosition<<32) + (UINT64)(UINT32)rArg.ai4Arg[2];

    MTMPLAYER_PRINT(" - u4SeekPosition = %u\n", (int)u8SeekPosition);

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if(!PLAYMGR_SeekPos(MTMPLAYER_PLAYMGR0, u8SeekPosition))
#else
        if(!PLAYMGR_SeekPos(u8SeekPosition))
#endif
        {
            MTMPLAYER_PRINT(" - PLAYMGR_SeekPos %lld Error \n", u8SeekPosition);
            return MTR_NOT_OK;
        }
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Set image parameters for display.
*@param prImgSetting               Image setting for display
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_DisplayImage(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_IMGPARAM_T *prImgSetting=NULL, rImgSetting;
    MTMPLAYER_INSTANCE_T eInstance;
    MTVDO_REGION_T *prRegion, rRegion;
    BOOL fgPartial;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    hHandle=(MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    prImgSetting=(MTMPLAYER_IMGPARAM_T *)rArg.ai4Arg[1];
    fgPartial = (BOOL)rArg.ai4Arg[2];
    prRegion = (MTVDO_REGION_T *)rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_REGION_T);
    USR_SPACE_ACCESS_VALIDATE_ARG(prImgSetting, MTMPLAYER_IMGPARAM_T);
    COPY_FROM_USER_ARG(prImgSetting, rImgSetting, MTMPLAYER_IMGPARAM_T);

    MTMPLAYER_PRINT(" -void\n");
    //MTMPLAYER_IMGPARAM_PRINT("prImgSetting",prImgSetting);

 //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_IMAGE)
    {
        _MTMPLAYER_DisplayImg(&rImgSetting, &rRegion, fgPartial);
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }                   

    return MTR_OK;
}


static MT_RESULT_T _MTMPLAYER_GetBufInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    FEEDER_BUF_INFO_T *prBufInfo;
    FEEDER_BUF_INFO_T rBufInfo;
    MTMPLAYER_INSTANCE_T eInstance;
    FeederSourceType eDataType;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);    
    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    prBufInfo = (FEEDER_BUF_INFO_T *)rArg.ai4Arg[1];
    
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance <= MTMPLAYER_MAX)
    {
        if (eInstance == MTMPLAYER_AUDIO)
        {
            eDataType = FEEDER_AUDIO_SOURCE;
        }
        else if (eInstance == MTMPLAYER_PLAYMGR)
        {
            eDataType = FEEDER_PROGRAM_SOURCE;           
        }
        else
        {
            eDataType = FEEDER_VIDEO_SOURCE;
        }
        FeederGetBufferInfo(eDataType, (FEEDER_BUF_INFO_T *)&rBufInfo);
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }              
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prBufInfo, FEEDER_BUF_INFO_T);
    COPY_TO_USER_ARG(prBufInfo, rBufInfo, FEEDER_BUF_INFO_T);

    return MTR_OK;
}


static MT_RESULT_T _MTMPLAYER_DecImage(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 *pu1Buf;
    UINT32 u4ReadSize;
    BOOL   fgEOF;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    pu1Buf = (UINT8 *)rArg.ai4Arg[0];
    u4ReadSize = (UINT32)rArg.ai4Arg[1];
    fgEOF = (BOOL)rArg.ai4Arg[2];
    //_MTMPLAYER_DecImg(pu1Buf, u4ReadSize, fgEOF);
    //code sync
    if (!_MTMPLAYER_DecImg(pu1Buf, u4ReadSize, fgEOF))
    {
        return MTR_NOT_OK;
    }    
    
    return MTR_OK;
}


static MT_RESULT_T _MTMPLAYER_SubmitData(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_4ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    FEEDER_BUF_INFO_T rBufferInfo;
    MTMPLAYER_INSTANCE_T eInstance;
    FeederSourceType eDataType;
    UINT64 u8Pos;
    UINT32 u4Size;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);    
    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    u4Size = (UINT32)rArg.ai4Arg[1];
    u8Pos = ((((UINT64)(UINT32)rArg.ai4Arg[2]) << 32) | (UINT64)(UINT32)rArg.ai4Arg[3]);

    eInstance = _MTMPLAYER_GetInstance(hHandle);
    eDataType = (eInstance == MTMPLAYER_AUDIO)? FEEDER_AUDIO_SOURCE : FEEDER_PROGRAM_SOURCE;
    // Get the buffer information
    if (FeederGetBufferInfo(eDataType, (FEEDER_BUF_INFO_T *)&rBufferInfo) != FEEDER_E_OK)
    {
        return (MTR_NOT_OK);
    }

    if (eInstance == MTMPLAYER_AUDIO)                           //push mode
    {
        if (FeederUpdateWritePointer(eDataType, u4Size) != FEEDER_E_OK)
        {
            return (MTR_NOT_OK);
        }
    }
    else                                                        //pull mode
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_ReceiveData(MTMPLAYER_PLAYMGR0, u4Size, u8Pos))
#else
        if (!PLAYMGR_ReceiveData(u4Size, u8Pos))
#endif  	
        {
            return (MTR_NOT_OK);
        }
		if (rBufferInfo.u4FreeSize < u4Size)
        {
            return (MTR_NOT_OK);
        }
    }
        
    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}
//MT_MPLAYER_COLOR_FORMAT_ARGB8888
static void  _MTMPLAYER_IMGYUV2RGB(void)
{
    FBM_POOL_T *prFbmPoolJPG;
   UINT32 u4YSrcLen=0;
   //UINT32 u4ARGBSize = (u4YSrcLen*_rDecOut.u4Height*4);
   
	prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
	ASSERT(prFbmPoolJPG != NULL);
	ASSERT(prFbmPoolJPG->u4Addr != 0);
   u4YSrcLen=(_rDecOut.u4Width + 0xF) & (~0xF);

	//printf("_MTMPLAYER_IMGYUV2RGB[destination_addr=%x]\n", (_rDecOut.pu1Addr-u4ARGBSize));
	//printf("_MTMPLAYER_IMGYUV2RGB[src_y_addr.pu1Addr=%x][src_cbcr_addr.pu1Addr=%x]\n", _rDecOut.pu1Addr, (_rDecOut.pu1Addr+_rDecOut.u4CbCrOffset));
	//printf("_MTMPLAYER_IMGYUV2RGB[_rDecOut.pu1Addr=%x][u4ARGBSize=%x][_rDecOut.u4Width=%x][_rDecOut.u4Height=%x]\n", _rDecOut.pu1Addr, u4ARGBSize, _rDecOut.u4Width, _rDecOut.u4Height);
	//printf("_MTMPLAYER_IMGYUV2RGB[dwJpgTgtPtr=%x]\n", dwJpgTgtPtr);
	
  #if 1
  
   GFX_Lock();
   GFX_MMU_Set_Enable(FALSE);
   GFX_SetColCnvFmt(E_YCFMT_422LINEAR,E_SWAP_0,E_VSTD_BT709,E_VSYS_COMP);
   GFX_SetColCnvSrc((UINT8 *)PHYSICAL((UINT32)_rDecOut.pu1Addr),u4YSrcLen,(UINT8 *)PHYSICAL((UINT32)_rDecOut.pu1Addr+_rDecOut.u4CbCrOffset),u4YSrcLen,0);
   GFX_SetDst((UINT8 *)PHYSICAL(prFbmPoolJPG->u4Addr), CM_ARGB8888_DIRECT32,u4YSrcLen<<2);
//   GFX_SetAlpha((UINT32)0xff);
   GFX_ColConv(0,0,_rDecOut.u4Width,_rDecOut.u4Height);
   GFX_Flush();
   GFX_Wait();
   
   GFX_Unlock();

  #else
   GFX_Lock();
   GFX_MMU_Set_Enable(FALSE);  
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
   /*
   printf("_MTMPLAYER_IMGYUV2RGB user cpy[%x][%x]\n", (void __user *)dwJpgTgtPtr, VIRTUAL(_rDecOut.pu1Addr-u4ARGBSize));
   p = VIRTUAL(_rDecOut.pu1Addr-u4ARGBSize);
   for(i=0;i<32;i++)
   	{
		printf("[%x]", *(p+i));
   }
   printf("\n\n");
   */
   copy_to_user( (void __user *)dwJpgTgtPtr ,VIRTUAL(_rDecOut.pu1Addr-u4ARGBSize),u4ARGBSize);

	/*
   p = (void __user *)dwJpgTgtPtr;
   for(i=0;i<32;i++)
   	{
		printf("[%x]", *(p+i));
   }
   printf("\n");
   */
   
 #endif
}

static MT_RESULT_T _MTMPLAYER_GetImg(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    FBM_POOL_T *prFbmPoolJPG;
    //UINT32 *pu4Addr, u4Addr, *pu4Size;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);    
    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];

    eInstance = _MTMPLAYER_GetInstance(hHandle);

    if (eInstance != MTMPLAYER_IMAGE)
    {
        return (MTR_NOT_OK);
    }
    
    if (uJpgColorFmt == MT_MPLAYER_COLOR_FORMAT_ARGB8888)//jpglib format.
    {
        
#if 1//gfx hw yuv=>RGB 888
         _MTMPLAYER_IMGYUV2RGB();
#else
        if (!access_ok(VERIFY_WRITE, (void __user *)dwJpgTgtPtr, _rDecOut.u4CbCrOffset))       
        {                                                       
            printk("mtal_ioctl_mplayer argument error\n");     
            return MTR_ERR_INV;                                          
        }                                                       

        copy_to_user( (void __user *)dwJpgTgtPtr ,VIRTUAL(_rDecOut.pu1Addr),_rDecOut.u4CbCrOffset);
#endif
        rArg.ai4Arg[1] = (INT32)_rDecOut.pu1Addr;
        rArg.ai4Arg[2] = (INT32)_rDecOut.u4CbCrOffset;
        rArg.ai4Arg[3] = (INT32)_rDecOut.u4Pitch;
//        rArg.ai4Arg[4] = (INT32)_rDecOut.u4StartX;
//        rArg.ai4Arg[5] = (INT32)_rDecOut.u4StartY;
	    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);

	    rArg.ai4Arg[4] = (INT32)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
	    //rArg.ai4Arg[5] = (INT32)(prFbmPoolJPG->u4Size & PAGE_MASK);
	    rArg.ai4Arg[5] = (INT32)(0x780000 & PAGE_MASK);
	    

        rArg.ai4Arg[6] = (INT32)_rDecOut.u4Width;
        rArg.ai4Arg[7] = (INT32)_rDecOut.u4Height;

        USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_8ARG_T);
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_8ARG_T);
    }
    else
    {
    
	    #ifdef CC_JPG_USE_VIDEO_PLANE_DEMO_PROGRAM
	    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_B2R);
	    #else
	    prFbmPoolJPG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
	    #endif


	    rArg.ai4Arg[1] = (INT32)PAGE_ALIGN(prFbmPoolJPG->u4Addr);
	    rArg.ai4Arg[2] = (INT32)(prFbmPoolJPG->u4Size & PAGE_MASK);
	    
	    USR_SPACE_ACCESS_VALIDATE_ARG(arg, MTAL_IOCTL_3ARG_T);
	    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_3ARG_T);
    }
    return MTR_OK;
}


/**Get a/v meta data
*@param handle
*@param prMeta
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_GetMetaData(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_AVMETA_T *prMeta=NULL;
    MTMPLAYER_AVMETA_T   rMeta;
	PLAYMGR_AVMETA_T rPlaymgrMeta;
    MT_RESULT_T eResult = MTR_OK;
    MTMPLAYER_INSTANCE_T eInstance;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    prMeta = (MTMPLAYER_AVMETA_T *)rArg.ai4Arg[1];

 //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);

    if ((eInstance == MTMPLAYER_PLAYMGR) || (eInstance == MTMPLAYER_AUDIO))
    {
    	if (prMeta == NULL)
    	{
    	    MTMPLAYER_PRINT(" - meta data Error \n");
            return MTR_NOT_OK;
    	}
    	x_memset((void *)&rPlaymgrMeta, 0, sizeof(PLAYMGR_AVMETA_T));
#ifdef CC_53XX_SWDMX_V2
    	PLAYMGR_GetMetaData(MTMPLAYER_PLAYMGR0, &rPlaymgrMeta);  
#else
    	PLAYMGR_GetMetaData(&rPlaymgrMeta);    	
#endif

        USR_SPACE_ACCESS_VALIDATE_ARG(prMeta, MTMPLAYER_AVMETA_T);
        COPY_FROM_USER_ARG(prMeta, rMeta, MTMPLAYER_AVMETA_T);
		if (rPlaymgrMeta.pu2Title != NULL)
		{
	        MTMPLAYER_COPY_WITH_LIMIT((void *)rMeta.au2Title, \
	            (void *)rPlaymgrMeta.pu2Title, rPlaymgrMeta.u4TitleLen, \
	            MTMPLAYER_META_SIZE);		
		}
		if (rPlaymgrMeta.pu2Album != NULL)
		{
	        MTMPLAYER_COPY_WITH_LIMIT((void *)rMeta.au2Album, \
	            (void *)rPlaymgrMeta.pu2Album, rPlaymgrMeta.u4AlbumLen, \
	            MTMPLAYER_META_SIZE);			
		}
		if (rPlaymgrMeta.pu2Artist != NULL)
		{
	        MTMPLAYER_COPY_WITH_LIMIT((void *)rMeta.au2Artist, \
	            (void *)rPlaymgrMeta.pu2Artist, rPlaymgrMeta.u4ArtistLen, \
	            MTMPLAYER_META_SIZE);			
		}
		if (rPlaymgrMeta.pu2Genre != NULL)
		{
	        MTMPLAYER_COPY_WITH_LIMIT((void *)rMeta.au2Genre, \
	            (void *)rPlaymgrMeta.pu2Genre, rPlaymgrMeta.u4GenreLen, \
	            MTMPLAYER_META_SIZE);			
		}
		
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
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    } 
    USR_SPACE_ACCESS_VALIDATE_ARG(prMeta,MTMPLAYER_AVMETA_T);
    COPY_TO_USER_ARG(prMeta,rMeta,MTMPLAYER_AVMETA_T);

    MTMPLAYER_PRINT(" - void\n");
    //MTMPLAYER_STATUS_PRINT("rStatus",&rStatus);

    return eResult;
#endif  // ENABLE_MULTIMEDIA
}


/**Step forward/backward MPlayer.
*@param handle
*@param step amount (1 : step forward 1 frame a time  -1 : step backward 1 frame a time)
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_Step(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    INT32 i4StepAmount;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    i4StepAmount=(INT32)rArg.ai4Arg[1];

    MTMPLAYER_PRINT(" - i4StepAmount = %u\n", i4StepAmount);

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_Step(MTMPLAYER_PLAYMGR0, i4StepAmount))
#else
        if (!PLAYMGR_Step(i4StepAmount))
#endif
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }               

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}

/**Set FIFO to MPlayer
*@param MPlayer Handle
*@param prFifo
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SetFifo(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_FIFOPARM_T* prFifo = NULL, rFifo;
    MTMPLAYER_INSTANCE_T eInstance;
    PLAYMGR_FIFO_T rPlayFifo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle=(MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    prFifo=(MTMPLAYER_FIFOPARM_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prFifo, MTMPLAYER_FIFOPARM_T);
    COPY_FROM_USER_ARG(prFifo, rFifo, MTMPLAYER_FIFOPARM_T);

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);    
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
        rPlayFifo.eFifoType = (PLAYMGR_FIFOTYPE_T)rFifo.eFifoType;
        rPlayFifo.u4Sa = rFifo.u4Sa;
        rPlayFifo.u4Ea = rFifo.u4Ea;
        rPlayFifo.u4NtfySz = rFifo.u4NtfySz;
#ifdef CC_53XX_SWDMX_V2
        if (!PLAYMGR_SetFifo(MTMPLAYER_PLAYMGR0, &rPlayFifo))
#else
        if (!PLAYMGR_SetFifo(&rPlayFifo))
#endif
        {
            return MTR_NOT_OK;
        }
    }
    else if (eInstance == MTMPLAYER_IMAGE)
    {
        // ???
    }
    else if (eInstance == MTMPLAYER_AUDIO)
    {
        // ???
    }

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


static MT_RESULT_T _MTMPLAYER_ConvertSP(UINT32 u4Buffer, UINT32 u4Width, 
                                                   UINT32 u4Height, UINT32 u4Stride, 
                                                   MTMPLAYER_PIXFORMAT_T eFormatType)
{
    IMG_VP_INFO_T rVpInfo;
    RZ_VDO_SCL_PARAM_SET_T rScaleParamV;
    UINT32 u4ColorMode; 

    if(IMG_GetVideoPlaneInfo(&rVpInfo) != E_IMG_OK)
    {
	   	return MTR_NOT_OK;
    }

    if ((eFormatType == MT_MPLAYER_PIX_FORMAT_UYVY) || (eFormatType == MT_MPLAYER_PIX_FORMAT_VYUY))
    {
        rScaleParamV.u4IsRsIn = 1;        
        rScaleParamV.u4InMode = E_RZ_VDO_OUTMD_422;
        rScaleParamV.u4YSrcBase = (UINT32)rVpInfo.pu1BackFrameAddr[0];
        rScaleParamV.u4YSrcBufLen = ALIGN16(rVpInfo.u4Width);
        rScaleParamV.u4YSrcHOffset = 0;
        rScaleParamV.u4YSrcVOffset = 0;
        rScaleParamV.u4YSrcW = u4Width;
        rScaleParamV.u4YSrcH = u4Height;
        rScaleParamV.u4CSrcBase = (UINT32)rVpInfo.pu1BackFrameAddr[1];
        rScaleParamV.u4CSrcHOffset = 0;
        rScaleParamV.u4CSrcVOffset = 0;
        rScaleParamV.u4IsRsOut = 1;
        rScaleParamV.u4OutMode = E_RZ_VDO_OUTMD_422;
        rScaleParamV.u4IsVdo2Osd = 1;     
        rScaleParamV.u4YTgBase = (UINT32)u4Buffer;       
        rScaleParamV.u4YTgCM = (eFormatType == MT_MPLAYER_PIX_FORMAT_UYVY) ? E_RZ_OSD_DIR_CM_CbYCrY : 
                                E_RZ_OSD_DIR_CM_YCbYCr;
        rScaleParamV.u4YTgBufLen = u4Stride;
        rScaleParamV.u4YTgHOffset = 0;
        rScaleParamV.u4YTgVOffset = 0;
        rScaleParamV.u4YTgW = u4Width;
        rScaleParamV.u4YTgH = u4Height;
        rScaleParamV.u4CTgBase = (UINT32)u4Buffer;
        rScaleParamV.u4CSrcW = u4Width >> 1;
        rScaleParamV.u4CSrcH = u4Height;
        rScaleParamV.u4CTgW = u4Width >> 1;
        rScaleParamV.u4CTgH = u4Height;        
        rScaleParamV.u4SrcSwap = 0;    
        IMGRZ_Lock_Ex(E_FIRST_RESIZER);
        IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
        IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOMD);
        IMGRZ_Scale_Ex(E_FIRST_RESIZER, (void *)(&rScaleParamV));           
        IMGRZ_Flush_Ex(E_FIRST_RESIZER);
        IMGRZ_Wait_Ex(E_FIRST_RESIZER);
        IMGRZ_Unlock_Ex(E_FIRST_RESIZER);
    }
    else
    {
        switch (eFormatType)
        {
        case MT_MPLAYER_PIX_FORMAT_1555:	
            u4ColorMode = CM_ARGB1555_DIRECT16;
            break;
        case MT_MPLAYER_PIX_FORMAT_565:		
            u4ColorMode = CM_RGB565_DIRECT16;
            break;
        case MT_MPLAYER_PIX_FORMAT_4444:
            u4ColorMode = CM_ARGB4444_DIRECT16;
            break;
        case MT_MPLAYER_PIX_FORMAT_8888:	
            u4ColorMode = CM_ARGB8888_DIRECT32;
            break;
        default:
            return MTR_NOT_OK;
        }
        GFX_Lock();
        GFX_MMU_Set_Enable(FALSE);        
        GFX_SetColCnvFmt((UINT32)E_YCFMT_422LINEAR, (UINT32)E_SWAP_BLOCK, 
            (UINT32)E_VSTD_BT709, (UINT32)E_VSYS_COMP);
        GFX_SetColCnvSrc((UINT8 *)rVpInfo.pu1BackFrameAddr[0], ALIGN16(rVpInfo.u4Width), 
                         (UINT8 *)rVpInfo.pu1BackFrameAddr[1], ALIGN16(rVpInfo.u4Width), 0);        
        GFX_SetDst((UINT8 *)u4Buffer, u4ColorMode, u4Stride);
        GFX_ColConv(0, 0, u4Width, u4Height);
        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
    }

    return MTR_OK;
}

static MT_RESULT_T _MTMPLAYER_SetSpDisp(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTMPLAYER_IMGPARAM_T *prImg;
    MTMPLAYER_IMGPARAM_T *prRegion;

    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);

    prImg = (MTMPLAYER_IMGPARAM_T *)rArg.ai4Arg[0];
    prRegion = (MTMPLAYER_IMGPARAM_T *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prImg, MTMPLAYER_IMGPARAM_T);
    COPY_FROM_USER_ARG(prImg, rSpDisp.rImgSetting, MTMPLAYER_IMGPARAM_T);
    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rSpDisp.rRegion, MTVDO_REGION_T);

    //x_memcpy((VOID *)&rSpDisp.rImgSetting, (VOID *)rArg.ai4Arg[0], sizeof(MTMPLAYER_IMGPARAM_T));
    //x_memcpy((VOID *)&rSpDisp.rRegion, (VOID *)rArg.ai4Arg[1], sizeof(MTVDO_REGION_T));
    rSpDisp.fgPartial = (BOOL)rArg.ai4Arg[2];
    rSpDisp.u4Buffer = (UINT32)rArg.ai4Arg[3];
    rSpDisp.u4Width = (UINT32)rArg.ai4Arg[4];
    rSpDisp.u4Height = (UINT32)rArg.ai4Arg[5];
    rSpDisp.u4Stride = ALIGN16((UINT32)rArg.ai4Arg[6]);
    rSpDisp.eFormatType = (MTMPLAYER_PIXFORMAT_T)rArg.ai4Arg[7];   
    rSpDisp.fgSpDisp = TRUE;

    return MTR_OK;
}


/**Start slide show
*@param slide type
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SlideShow(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    MTMPLAYER_SLIDETYPE eSlideType;
    IMG_VP_INFO_T rVpInfo; 

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];
    eSlideType = (MTMPLAYER_SLIDETYPE)rArg.ai4Arg[1];
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance != MTMPLAYER_IMAGE)        
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }

    if(IMG_GetVideoPlaneInfo(&rVpInfo) != E_IMG_OK)
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
        MTMPLAYER_BarWipeH(rVpInfo, TRUE);
        break;   
    case MT_MPLAYER_EFFECT_BARWIPE_RL:
        MTMPLAYER_BarWipeH(rVpInfo, FALSE);
        break;
    case MT_MPLAYER_EFFECT_BARWIPE_TB:
        MTMPLAYER_BarWipeV(rVpInfo, TRUE);
        break;        
    case MT_MPLAYER_EFFECT_BARWIPE_BT:
        MTMPLAYER_BarWipeV(rVpInfo, FALSE);
        break;        
    case MT_MPLAYER_EFFECT_BOXWIPE_TL:
        MTMPLAYER_BoxWipe(rVpInfo, TRUE, TRUE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_TR:
        MTMPLAYER_BoxWipe(rVpInfo, TRUE, FALSE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_BL:
        MTMPLAYER_BoxWipe(rVpInfo, FALSE, TRUE);
        break;
    case MT_MPLAYER_EFFECT_BOXWIPE_BR:
        MTMPLAYER_BoxWipe(rVpInfo, FALSE, FALSE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_HC:
        MTMPLAYER_DoorWipeH(rVpInfo, TRUE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_HO:
        MTMPLAYER_DoorWipeH(rVpInfo, FALSE);
        break;    
    case MT_MPLAYER_EFFECT_DOORWIPE_VC:
        MTMPLAYER_DoorWipeV(rVpInfo, TRUE);
        break;
    case MT_MPLAYER_EFFECT_DOORWIPE_VO:
        MTMPLAYER_DoorWipeV(rVpInfo, FALSE);
        break;  
    case MT_MPLAYER_EFFECT_IRISWIPE:
        MTMPLAYER_IrisWipe(rVpInfo);
        break;
    case MT_MPLAYER_EFFECT_NONE:
        IMG_Flip();
        break;
    default:
        return MTR_NOT_OK;
    }
    
    return MTR_OK;
}


/**Check pending subtitle.
*@param MPlayer Handle
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_ChkSubtitle(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_3ARG_T rArg;
    MTMPLAYER_HANDLE hHandle;
    MTMPLAYER_INSTANCE_T eInstance;
    BOOL fgDrawAck;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    hHandle = (MTMPLAYER_HANDLE)rArg.ai4Arg[0];

     //add driver implementation code here
    eInstance = _MTMPLAYER_GetInstance(hHandle);
    if (eInstance == MTMPLAYER_PLAYMGR)
    {
#ifdef _EN_SBTL_
        if (fgIsMtalSbtlExisted())  // check for external subtitle first
        {
            vMtalSbtlTextCheck();
            return MTR_OK;
        }
#endif
        // if external subtitle has been checked, no need to check internal subtitle since they are mutex

        if (rArg.ai4Arg[2] == 0)
          fgDrawAck = FALSE;
        else
          fgDrawAck = TRUE;
          
#ifdef CC_53XX_SWDMX_V2
        if (PLAYMGR_ChkSubtitle(MTMPLAYER_PLAYMGR0, fgDrawAck))
#else
        if (PLAYMGR_ChkSubtitle(fgDrawAck))
#endif
          rArg.ai4Arg[1] = (INT32)TRUE;
        else
          rArg.ai4Arg[1] = (INT32)FALSE;
        COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_3ARG_T);
        
        return MTR_OK;
    }
    else
    {
        MTMPLAYER_PRINT(" - Instance Error \n");
        return MTR_NOT_OK;
    }    
#endif  // ENABLE_MULTIMEDIA
}


/**set image connect to vdp
*@param connect or disconnect
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_ConnectImg(unsigned long arg)
{
    //INT32 i4Ret = 0;
    BOOL fgConnect;

    fgConnect = (BOOL)arg;

    if (fgConnect)
    {
        if (!_fgVdpConnect)
        {
            if (IMG_VDP_Connect(_u4PlaneId) != E_IMG_OK)
            {
    	      	return MTR_NOT_OK;
        	} 
            else
            {
                _fgVdpConnect = TRUE;
                IMG_Flip();
            }
        }
    }
    else
    {
        if (_fgVdpConnect)
        {
            //VDO
            VDP_Image_Disconnect();
            _fgVdpConnect = FALSE;
        }
    }

    return MTR_OK;
}

/**clean the img frame buffer
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_CleanFB(unsigned long arg)
{
    IMG_VP_INFO_T rVpInfo; 
    UINT32 u4Size;

    if(IMG_GetVideoPlaneInfo(&rVpInfo) != E_IMG_OK)
    {
	   	return MTR_NOT_OK;
    }

    u4Size = (UINT32)(rVpInfo.pu1CurFrameAddr[1] - rVpInfo.pu1CurFrameAddr[0]);
    x_memset((VOID *)VIRTUAL((UINT32)rVpInfo.pu1CurFrameAddr[0]), 0x0, u4Size);
    x_memset((VOID *)VIRTUAL((UINT32)rVpInfo.pu1CurFrameAddr[1]), 0x80, u4Size);
    x_memset((VOID *)VIRTUAL((UINT32)rVpInfo.pu1BackFrameAddr[0]), 0x0, u4Size);
    x_memset((VOID *)VIRTUAL((UINT32)rVpInfo.pu1BackFrameAddr[1]), 0x80, u4Size);

    return MTR_OK;
}

/**Assign external subtitle parameter.
*@param type
*@param size
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_AssignExtSubtitle(unsigned long arg)
{
#ifdef _EN_SBTL_
    MTAL_IOCTL_2ARG_T rArg;
    MM_SBTL_TYPE_T eType;
    UINT32 u4Size;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eType = (MM_SBTL_TYPE_T)rArg.ai4Arg[0];
    u4Size = (UINT32)rArg.ai4Arg[1];

    MTMPLAYER_PRINT(" - eType = %d, u4Size = %d\n", (int)eType, (int)u4Size);

    if (u4Size != 0)
    {
        vMtalSbtlTextAssign(TRUE, eType, u4Size);
    }
    else
    {
        vMtalSbtlTextAssign(FALSE, 0, 0);
    }

    return MTR_OK;
#else
    arg = arg;
    return MTR_OK;
#endif
}

/**Get DRM registration code.
*@param code
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_GetDRMRegCode(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
#ifdef _EN_DRM_
    char acCode[11];
    MTAL_IOCTL_3ARG_T rArg;
    
#ifdef CC_53XX_SWDMX_V2
    if (!PLAYMGR_GetDivxDRMRegCode(MTMPLAYER_PLAYMGR0, acCode))
#else
    if (!PLAYMGR_GetDivxDRMRegCode(acCode))
#endif
        return MTR_NOT_OK;
    
    rArg.ai4Arg[0] = acCode[0] | (acCode[1] << 8) | (acCode[2] << 16) | (acCode[3] << 24);
    rArg.ai4Arg[1] = acCode[4] | (acCode[5] << 8) | (acCode[6] << 16) | (acCode[7] << 24);
    rArg.ai4Arg[2] = acCode[8] | (acCode[9] << 8);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_3ARG_T);
    
    return MTR_OK;
#else
    arg = arg;
    return MTR_OK;
#endif
#endif  // ENABLE_MULTIMEDIA
}

/**Get DRM UI help info.
*@param info
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_GetDRMUIHelpInfo(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
#ifdef _EN_DRM_
    UINT32 u4Info, *pu4Info;
    
    pu4Info = (UINT32 *)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Info, UINT32);

#ifdef CC_53XX_SWDMX_V2
    if (!PLAYMGR_GetDivxDRMUIHelpInfo(MTMPLAYER_PLAYMGR0, &u4Info))
#else
    if (!PLAYMGR_GetDivxDRMUIHelpInfo(&u4Info))
#endif
        return MTR_NOT_OK;
    
    COPY_TO_USER_ARG(pu4Info, u4Info, UINT32);
    
    return MTR_OK;
#else
    arg = arg;
    return MTR_OK;
#endif
#endif  // ENABLE_MULTIMEDIA
}

/**DRM deactivation.
*@param code
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_DRMDeactivation(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
#ifdef _EN_DRM_
    char acCode[9];
    MTAL_IOCTL_2ARG_T rArg;
    
#ifdef CC_53XX_SWDMX_V2
    if (!PLAYMGR_DivxDRMDeactivation(MTMPLAYER_PLAYMGR0, acCode))
#else
    if (!PLAYMGR_DivxDRMDeactivation(acCode))
#endif
        return MTR_NOT_OK;
    
    rArg.ai4Arg[0] = acCode[0] | (acCode[1] << 8) | (acCode[2] << 16) | (acCode[3] << 24);
    rArg.ai4Arg[1] = acCode[4] | (acCode[5] << 8) | (acCode[6] << 16) | (acCode[7] << 24);
    COPY_TO_USER_ARG(arg, rArg, MTAL_IOCTL_2ARG_T);
    
    return MTR_OK;
#else
    arg = arg;
    return MTR_OK;
#endif
#endif  // ENABLE_MULTIMEDIA
}

/**Set source type.
*@param Source Type
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SetSrcType(unsigned long arg)
{    
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTMPLAYER_SRCTYPE eMTType = (MTMPLAYER_SRCTYPE)arg;
    PLAYMGR_SourceType_T ePlayType = PLAYMGR_SRC_TYPE_HIGH_SPEED_STORAGE;
    switch(eMTType)
    {
        case MTMPLAYER_SRCTYPE_UNKNOWN:
            ePlayType = PLAYMGR_SRC_TYPE_UNKNOWN;
            break;
        case MTMPLAYER_SRCTYPE_HIGH_SPEED_STORAGE:
            ePlayType = PLAYMGR_SRC_TYPE_HIGH_SPEED_STORAGE;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_DLNA:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_DLNA;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_NETFLIX:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_NETFLIX;
            break;
        case MTMPLAYER_SRCTYPE_TIME_SHIFT:
            ePlayType = PLAYMGR_SRC_TYPE_TIME_SHIFT;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_VUDU:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_VUDU;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_FM:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_FM;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_DLNA_LPCM:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_DLNA_LPCM;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_RHAPSODY:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_RHAPSODY;
            break;
        case MTMPLAYER_SRCTYPE_NETWORK_XUNLEI:
            ePlayType = PLAYMGR_SRC_TYPE_NETWORK_XUNLEI;
            break;
        default:
            printf(" - _MTMPLAYER_SetSrcType unknown type(%d)\n", (UINT32)eMTType);
            break;
    }
#ifdef CC_53XX_SWDMX_V2
    PLAYMGR_SetSrcType(MTMPLAYER_PLAYMGR0, ePlayType);
#else
    PLAYMGR_SetSrcType(ePlayType);
#endif

    _eSrcType = eMTType;
    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}

/**Set number of bitstream.
*@param Number of Bitstream
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SetNumOfBitstream(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
#ifdef CC_53XX_SWDMX_V2
    PLAYMGR_SetNumberOfBitstream(MTMPLAYER_PLAYMGR0, (UINT32)arg);
#else
    PLAYMGR_SetNumberOfBitstream((UINT32)arg);
#endif

    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}

/**Set netflix eos.
*@param eos flag
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SetNetflixEos(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
#ifdef CC_53XX_SWDMX_V2
    SWDMX_SetInfo(MTMPLAYER_SWDMX0, eSWDMX_SET_NETFLIX_EOS,(UINT32)arg,0,0);
#else
    SWDMX_SetInfo(eSWDMX_SET_NETFLIX_EOS,(UINT32)arg,0,0);
#endif
    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}

/**Set netflix eos.
*@param eos flag
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_SetEos(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
#ifdef CC_53XX_SWDMX_V2
    SWDMX_SetInfo(MTMPLAYER_SWDMX0, eSWDMX_SET_EOS,(UINT32)arg,0,0);
#else
    SWDMX_SetInfo(eSWDMX_SET_EOS,(UINT32)arg,0,0);
#endif
    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Get capability.
*@param type
*@param parameter 1
*@param parameter 2
*@retval MTR_OK                    Success.
*@retval MTR_NOT_OK                Fail.
*/
static MT_RESULT_T _MTMPLAYER_GetCap(unsigned long arg)
{
#ifndef ENABLE_MULTIMEDIA
    UNUSED(arg);
    
    return MTR_NOT_OK;
#else
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4Type;
    UINT32 u4Param1;
    UINT32 u4Param2;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u4Type = (UINT32)rArg.ai4Arg[0];
    u4Param1 = (UINT32)rArg.ai4Arg[1];
    u4Param2 = (UINT32)rArg.ai4Arg[2];

    MTMPLAYER_PRINT(" - u4Type = %u  u4Param1 = %u  u4Param2 = %u\n", u4Type, u4Param1, u4Param2);

#ifdef CC_53XX_SWDMX_V2
    if(!PLAYMGR_GetCap(MTMPLAYER_PLAYMGR0, u4Type, u4Param1, u4Param2))
#else
    if(1/*!PLAYMGR_GetCap(u4Type, u4Param1, u4Param2)*/)  // V1 does not have this API yet!!!
#endif
    {
        UNUSED(u4Type);
        UNUSED(u4Param1);
        UNUSED(u4Param2);
        MTMPLAYER_PRINT(" - PLAYMGR_GetCap return FALSE\n");
        return MTR_NOT_OK;
    }

    UNUSED(u4Type);
    UNUSED(u4Param1);
    UNUSED(u4Param2);
    return MTR_OK;
#endif  // ENABLE_MULTIMEDIA
}


/**Initialize jpeg hw for decoding.
*@param eColorFMT            display color format
*@retval TRUE                 Success.
*@retval FALSE                Fail.
*/
static MT_RESULT_T _MTMPLAYER_ConfigImagedecoder(unsigned long arg)
{
    ENUM_IMG_FORMAT	imgFormat;
    
    imgFormat=(ENUM_IMG_FORMAT)arg;

	MTMPLAYER_PRINT("imgFormat =[%d]\n", imgFormat);

    if(IMG_Config(0,imgFormat) != E_IMG_OK)
    {
        MTMPLAYER_PRINT("IMG_Config NG\n");
        return MTR_NOT_OK;
    }    

    return MTR_OK;
}

#ifdef SWDMX_DBG_USB
static VOID *_pvAddr = NULL;
static UINT32 _u4Size = 0;
static volatile BOOL _fgGo = TRUE;
static volatile BOOL _fgGoObjectExist = FALSE;
static UINT32 _u4SleepCnt = 0;

static inline void  _CreateGoObject( BOOL fgInitGo )
{
    _fgGo = fgInitGo;
    _fgGoObjectExist = TRUE;
    
    return ;
}

static inline void _DeleteGoObject(VOID)
{
    _fgGo = TRUE;
    _fgGoObjectExist = FALSE;
    
    return ;
}

static BOOL _WaitForGoObject(VOID)
{
    _u4SleepCnt = 0;
    while (TRUE)
    {   
        if (TRUE==_fgGoObjectExist && FALSE==_fgGo)
        {   
            _u4SleepCnt++;
            x_thread_delay( 1 );
        }
        else if (FALSE==_fgGoObjectExist )
        {
            _fgGo = FALSE;
            return FALSE;
        }
        else if (TRUE==_fgGoObjectExist && TRUE==_fgGo)       // Do not del "TRUE == _fgGoObjectExist"
        {
            _fgGo = FALSE;
            break;
        }
        else if (2000 < _u4SleepCnt)
        {
            Printf("Wait for GoObj time out. Event ignored.\n");
            return FALSE;
        }
    }

    return TRUE;
}

static inline void _ReleaseGoObeject(VOID)
{
    _fgGo = TRUE;

    return;
}

// Please add your code here
static inline VOID _ReleaseSource(VOID)
{
    SWDMX_Release_source();

    return ;
}

void _MTMPLAYER_SWDMX_PutEvent(SWDMX_DBG_T *prCmd)
{  
    switch (prCmd->eCmd)
    {
    case SWDMX_DBG_DO_OPEN_FILE:        // open file CMD
    case SWDMX_DBG_DO_CLOSE_FILE:
        if (NULL==prCmd->u.rFileInfo.au1FilePath ||
            FALSE==_WaitForGoObject())
        {
            return ;
        }
        break;
        
    case SWDMX_DBG_DO_SAVE_DATA:
        Printf("SAVE Data, ignored!\n");
        return ;
    case SWDMX_DBG_DO_SAVE_BLOCK:
        // tmp resolution for thread ACK
        if (0==prCmd->u.rBufInfo.u4BlockSize ||
            FALSE==_WaitForGoObject())
        {
            return ;
        }

        _pvAddr = prCmd->u.rBufInfo.pvBufStart;
        _u4Size = prCmd->u.rBufInfo.u4BlockSize;
        break;

    case SWDMX_DBG_DO_LOAD_DATA:
        // Not use yet
        return ;
        #if 0
        if (0==prCmd->u.rBufInfo.u4BlockSize ||
            FALSE==_WaitForGoObject())
        {
            return ;
        }
        #endif
        break;

    case SWDMX_DBG_DO_SAVE_CRC:
    case SWDMX_DBG_DO_LOAD_CRC:
        if (0==prCmd->u.rCrcInfo.u4BlockSize ||
            FALSE==_WaitForGoObject())
        {
            return ;
        }
        break;

    case SWDMX_DBG_DO_DEBUG_EXIT:
        // Nothing To do, Only tell DEMO to exit
        if(FALSE==_WaitForGoObject())
        {
            Printf("Wait for GoObj time out. Quit faild.");
            return;
        }
        else
        {
            _DeleteGoObject();
        }
        break;
        
    default:
        Printf("%s:%d Unknown cmd.", __FUNCTION__, __LINE__);
        return ;
    }

    _CB_PutEvent(CB_SWDMX_DBG_TRIGGER, sizeof(SWDMX_DBG_T), prCmd);
    
    return ;
}

static MT_RESULT_T _MTMPLAYER_SWDMX_GetCmdReport( unsigned long arg )
{   // Can use the function in swdmx_drvif
    MTAL_IOCTL_5ARG_T *prArray = (MTAL_IOCTL_5ARG_T *)arg;
    SWDMX_DBG_CMD_T  rCmd = (SWDMX_DBG_CMD_T)prArray->ai4Arg[0];

    switch ( rCmd )
    {
    case SWDMX_DBG_REQ_OPEN_FILE:                       // open file CMD
    case SWDMX_DBG_REQ_CLOSE_FILE:                      // 
        _ReleaseGoObeject();
        if (TRUE == (BOOL)prArray->ai4Arg[1])
        {
            *(UINT32 *)prArray->ai4Arg[3] = (UINT32)prArray->ai4Arg[2];
        }
        break;
        
    case SWDMX_DBG_REQ_SAVE_DATA:                       // 
    case SWDMX_DBG_REQ_SAVE_BLOCK:
    {
        UINT32 u4Size = (UINT32)prArray->ai4Arg[3];
        VOID *pvAddr = (VOID __user*)prArray->ai4Arg[2];
        UINT32 u4Retvl = 0;
        u4Retvl = copy_to_user(pvAddr, _pvAddr, _u4Size>u4Size ? u4Size : _u4Size );
        UNUSED(u4Retvl);
        _ReleaseGoObeject();
        break;
    }
    
    case SWDMX_DBG_REQ_LOAD_DATA:                       // 
        // Not use now
        return MTR_OK;
        #if 0
        _ReleaseGoObeject();
        #endif
        break;
        
    case SWDMX_DBG_REQ_SAVE_CRC:                        // 
        _ReleaseGoObeject();
        // nothing to do
        break;
        
    case SWDMX_DBG_REQ_LOAD_CRC:                        // 
        if ( (UINT32)prArray->ai4Arg[2] != (UINT32)prArray->ai4Arg[1] )
        {
            Printf("Crc Miss match. GetCrc=%u, CalcCrc=%u, Offset=%u\n", (UINT32)prArray->ai4Arg[1],
                     (UINT32)prArray->ai4Arg[2], (UINT32)prArray->ai4Arg[3]);
        }
        _ReleaseGoObeject();
        break;
        
    case SWDMX_DBG_REQ_INIT:
        _CreateGoObject( TRUE );
        Dbg_Register_DBG_CB(0, (VOID *)_MTMPLAYER_SWDMX_PutEvent);
        _ReleaseSource();
        break;
        
    case SWDMX_DBG_REQ_EXIT:
        _DeleteGoObject();
        Dbg_Register_DBG_CB(0, NULL);
        _ReleaseSource();
        break;
        
    default:
        Printf("%s:%d: Cmd %d unknown.\n", __FUNCTION__, __LINE__, rCmd);
    }
    
    return MTR_OK;
}
#endif // SWDMX_DBG_USB

int mtal_ioctl_mtmplayer(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
	switch (cmd)
	{
		case MTAL_IO_MPLAYER_OPEN:
                     return  _MTMPLAYER_Open(arg);
		case MTAL_IO_MPLAYER_CLOSE:
                     return  _MTMPLAYER_Close(arg);
		case MTAL_IO_MPLAYER_RESET:
                     return  _MTMPLAYER_Reset(arg);
		case MTAL_IO_MPLAYER_SET:
                     return  _MTMPLAYER_Set(arg);
		case MTAL_IO_MPLAYER_START:
                     return  _MTMPLAYER_Start(arg);
		case MTAL_IO_MPLAYER_STOP:
                     return  _MTMPLAYER_Stop(arg);
		case MTAL_IO_MPLAYER_ABORT:
                     return  _MTMPLAYER_Abort(arg);
		case MTAL_IO_MPLAYER_PAUSE:
                     return  _MTMPLAYER_Pause(arg);
		case MTAL_IO_MPLAYER_RESUME:
                     return  _MTMPLAYER_Resume(arg);
  		case MTAL_IO_MPLAYER_GETINFO:
                     return  _MTMPLAYER_GetInfo(arg);
		case MTAL_IO_MPLAYER_REGCB:
                     return  _MTMPLAYER_RegCb(arg);
		case MTAL_IO_MPLAYER_SEEK:
                     return  _MTMPLAYER_Seek(arg);
        case MTAL_IO_MPLAYER_SEEK_POS:
                     return  _MTMPLAYER_SeekPos(arg);
		case MTAL_IO_MPLAYER_DISPLAYIMG:
                     return  _MTMPLAYER_DisplayImage(arg);
        case MTAL_IO_MPLAYER_GETBUFINFO:
                     return  _MTMPLAYER_GetBufInfo(arg);
        case MTAL_IO_MPLAYER_DECIMG:
                     return  _MTMPLAYER_DecImage(arg);
        case MTAL_IO_MPLAYER_SUBMIT:
                     return  _MTMPLAYER_SubmitData(arg);
        case MTAL_IO_MPLAYER_GETIMG:
                    return  _MTMPLAYER_GetImg(arg);
        case MTAL_IO_MPLAYER_GETMETA:            
                    return  _MTMPLAYER_GetMetaData(arg);
        case MTAL_IO_MPLAYER_STEP:
                    return  _MTMPLAYER_Step(arg);
	    case MTAL_IO_MPLAYER_SETFIFO:
                    return  _MTMPLAYER_SetFifo(arg);
        case MTAL_IO_MPLAYER_SETSPDISP:
                    return  _MTMPLAYER_SetSpDisp(arg);
                case MTAL_IO_MPLAYER_SLIDESHOW:
                    return  _MTMPLAYER_SlideShow(arg);
        case MTAL_IO_MPLAYER_CHKSUBTITLE:
                    return  _MTMPLAYER_ChkSubtitle(arg);
        case MTAL_IO_MPLAYER_CONNECTIMG:
                    return  _MTMPLAYER_ConnectImg(arg);
        case MTAL_IO_MPLAYER_CLEANFB:
                    return _MTMPLAYER_CleanFB(arg);
        case MTAL_IO_MPLAYER_ASSIGNEXTSUBTITLE:
                    return  _MTMPLAYER_AssignExtSubtitle(arg);
        case MTAL_IO_MPLAYER_GETDRMREGCODE:
                    return  _MTMPLAYER_GetDRMRegCode(arg);
        case MTAL_IO_MPLAYER_DRMDEACTIVATION:
                    return  _MTMPLAYER_DRMDeactivation(arg);
        case MTAL_IO_MPLAYER_GETDRMUIHELPINFO:
                    return  _MTMPLAYER_GetDRMUIHelpInfo(arg);
        case MTAL_IO_MPLAYER_SETSRCTYPE:
                    return  _MTMPLAYER_SetSrcType(arg);
        case MTAL_IO_MPLAYER_SETNUMBSTM:
                    return  _MTMPLAYER_SetNumOfBitstream(arg);
        case MTAL_IO_MPLAYER_NETFLIXEOS:
                    return _MTMPLAYER_SetNetflixEos(arg);
        case MTAL_IO_MPLAYER_SETEOS:
                    return _MTMPLAYER_SetEos(arg);                    
        case MTAL_IO_MPLAYER_GETCAP:
                    return _MTMPLAYER_GetCap(arg);                    
        case MTAL_IO_MPLAYER_CONFIGIMGDECODER:
                    return _MTMPLAYER_ConfigImagedecoder(arg);          
#ifdef SWDMX_DBG_USB
        case MTAL_IO_MPLAYER_SWDMX:
                    return _MTMPLAYER_SWDMX_GetCmdReport( arg );
#endif // SWDMX_DBG_USB
		default:
		     return  MTR_NOT_OK;
	}
	return MTR_OK;
}
