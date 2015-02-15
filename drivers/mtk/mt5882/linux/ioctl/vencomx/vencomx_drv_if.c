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
 * $RCSfile: vencomx_drv_if.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor: xiaolei li $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file vencomx_drv_if.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vencomx_mod.h"
#include "x_bim.h"
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <asm/uaccess.h>  // for access_ok VERIFY_READ


#ifdef CC_SUPPORT_VENC
#include "x_assert.h"
#include "venc_if.h"
#include "venc_drvif.h"
#include "venc_util_drvif.h"
#include "vdec_drvif.h"

#ifdef CC_SUPPORT_MUXER
#include "muxer_if.h"
#endif

// DI control related header
#include "drv_di.h"
#include "drv_video.h"
#include "vdo_misc.h"
#include "drv_vga.h"
#include "video_def.h"
#include "drv_hdmi.h"
#include "vdo_if.h"
#include "srm_drvif.h"
#include "drv_scaler.h"

#include "vdp_prescale_drvif.h"
#include "vdo_rm.h"
#include "vdp_if.h"
#include "source_select.h"
#include "source_table.h"
#include "drv_dvi.h"
#include "drv_display.h"
#include "gfx_if.h"

LINT_EXT_HEADER_BEGIN
#define DEFINE_IS_LOG	VOMX_IsLog
#include "x_debug.h"
LINT_EXT_HEADER_END

#define VENCOMX_DBG(fmt...)    {LOG(3, "%s %s(%d):","[vencomx_mod]", __FUNCTION__, __LINE__);  LOG(3, fmt);  }

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define _VENC_MAX_RES (1280 * 736)
#define _VENC_HALFFPS_THRD (1280 * 720 * 30)
#define COPYDATA_FROME_DRVIER
#define VENC_KEEPDATA_QSIZE (256)


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

VENC_BUFMGR_DECLARE(VENC_RZ_FRM, VENC_FRM_T, 10);

VENC_BUFMGR_DECLARE(VENC_FRM, VENC_FRM_T, 10);

typedef struct _VENCOMX_INFO_T
{
    BOOL                     fgIsCtrlOn;
    BOOL                     fgIsRunning;
    BOOL                     fgInterlace; // for replay
    BOOL                     fg2Usr;
    BOOL                     fgEncOSTG;
    BOOL                     fgAddAud;
    UCHAR                    ucMxId;
    UCHAR                    ucRzId;

    UINT16                   u2SrcCompId; // for vdec

    UINT32                   u4Width;
    UINT32                   u4Height;
    UINT32                   u4Pitch;
    UINT32                   u4Fps;
    UINT32                   u4Bps;
    UINT32                   u4SrcFps;    // for replay
    UINT32                   u4SrcWidth;  // for replay
    UINT32                   u4SrcHeight; // for replay
    UINT32                   u4SyncMode;
    UINT32                   u4EncType;
    UINT32                   u4Profile;
    UINT32                   u4Level;

    VENC_PROFILE_T           eProfileDef;
    VENC_PIX_FMT_T           ePixFmt;
    VENCOMX_SRC_TYPE_T       eSrcType;
    VENCOMX_FMT_TYPE_T       eEncFmt;

    VENC_HANDLE              hVEnc;
    VENC_FILTER_T*           ptFilter;
    VENC_BUFMGR_BASE_T*      ptFrmBuffMgr;
    VENC_BUFMGR_BASE_T*      ptResizeFrmBuffMgr;
    INT32 (*SetFmtInfo)      (struct _VENCOMX_INFO_T *, VOID *);
    INT32 (*ConnectInput)    (struct _VENCOMX_INFO_T *);
    INT32 (*DisconnectInput) (struct _VENCOMX_INFO_T *);
    INT32 (*SetInputParam)   (struct _VENCOMX_INFO_T *);
} VENCOMX_INFO_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//lint --e{717} do ... while (0);
#define VENCOMX_SET_VALIDATE(TYPE)         \
do                                      \
{                                       \
    if (NULL == pvData)            \
    {                                   \
        return VENCR_NOT_OK;    \
    }                                   \
                                        \
    if (sizeof(TYPE) > u4Size)  \
    {                                   \
        return VENCR_NOT_OK;    \
    }                                   \
} while (0)

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg, type)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(type)))              \
        {                                                       \
            return -1;                                          \
        }                                                       \
    
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel, type)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(type)))          \
        {                                                       \
            return -1;                                          \
        }
    
#define COPY_TO_USER_ARG(ArgUsr, ArgKernel, type)              \
        if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                           sizeof(type)))          \
        {                                                       \
            return -1;                                          \
        }
#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(VENCOMX_IOCTL_4ARG_T)))              \
        {                                                       \
            return -1;                                          \
        }                                                       \
            
#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(VENCOMX_IOCTL_4ARG_T)))          \
        {                                                       \
            return -1;                                          \
        }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(VENCOMX_IOCTL_3ARG_T)))              \
        {                                                       \
            return VENCR_ERR_INV;                                          \
        }     
#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(VENCOMX_IOCTL_3ARG_T)))          \
        {                                                       \
            return -1;                                          \
        }
#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(VENCOMX_IOCTL_2ARG_T)))              \
        {                                                       \
            return VENCR_ERR_INV;                                          \
        }
#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(VENCOMX_IOCTL_2ARG_T)))          \
        {                                                       \
            return VENCR_ERR_INV;                                          \
        } 
#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(VENCOMX_IOCTL_8ARG_T)))              \
        {                                                       \
            return VENCR_ERR_INV;                                          \
        }                                                       \
        
#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(VENCOMX_IOCTL_8ARG_T)))          \
        {                                                       \
            return VENCR_ERR_INV;                                          \
        }     \

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

extern void vDrvDISetVEncHandle(void *handle, void *ptFrmBuffMgr, void *ptResizeFrmBuffMgr, void *ptFilterPts);
extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

extern UINT32 VENC_EncSeqHdrSync(VENC_HANDLE hEnc, VENC_OUT_PIC_T *ptOut);

//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static VENCOMX_INFO_T _arVEncOMXInfo[VENCOMX_COMP_NS];


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static VENCOMX_INFO_T *_VENCGetInfoInst(UINT16 u2CompId)
{
    ASSERT((UINT16)VENCOMX_COMP_NS > u2CompId);
    return ((UINT16)VENCOMX_COMP_NS > u2CompId)? &_arVEncOMXInfo[u2CompId] : &_arVEncOMXInfo[0];
}

#if 0  //defined but not used
static VOID _VENCResetInfoInst(VENCOMX_INFO_T *prVEncOmxInfo)
{
    if (prVEncOmxInfo)
    {
        x_memset((VOID *)prVEncOmxInfo, 0, sizeof(VENCOMX_INFO_T));
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;
        prVEncOmxInfo->ucMxId = 0xFF;
        prVEncOmxInfo->eProfileDef = VENC_PROFILE_MAX;
    }
}


static BOOL _VENC_Destroy(VENCOMX_INFO_T *prVEncOmxInfo, BOOL fgDiscInput)
{
    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        UINT32 u4Ret;

        //STOP VENC
        if (prVEncOmxInfo->fgIsRunning)
        {
            if (fgDiscInput)
            {
                //disconnect  input source
                if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
                {
                    VENCOMX_DBG("- disconnect input source failed!\n");
                    return FALSE;
                }
            }

            u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                VENCOMX_DBG("- venc stop fail(%d)\n", u4Ret);
                return FALSE;
            }

            prVEncOmxInfo->fgIsRunning = FALSE;
        }

        //CLOSE VENC
        u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
            return FALSE;
        }
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;
    }

    if (prVEncOmxInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
        prVEncOmxInfo->ptFrmBuffMgr = NULL;
    }

    if (prVEncOmxInfo->ptResizeFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
        prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
    }

    return TRUE;
}

static UINT32 _VENCSetVDP(VENCOMX_INFO_T *prVEncOmxInfo, UINT8 fgOnOff)
{
#ifdef SET_VDP_BY_DRIVER
    VDP_REGION_T rSrcRegion;
    VDP_REGION_T rOutRegion;
    VDP_OVERSCAN_REGION_T rOverscan;
#endif

     
    if (fgOnOff)
    {
        vDrvVrmSetAppFlag(VRM_APP_VIDEO_ENC);

        if (prVEncOmxInfo->eProfileDef == VENC_PROFILE_MS &&
            !prVEncOmxInfo->fgEncOSTG)
        {
                bSubSrcCloneMode(SV_ON);
        }

        if (bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE)
        {
            VENCOMX_DBG(" signal not stable \n");
            return VENCR_NOT_OK;
        }

        if (prVEncOmxInfo->eProfileDef == VENC_PROFILE_PVR)
        {
            return VENCR_OK;
        }

        #if !defined(CC_MT5396)
        {
            APP_RES_T rVencRes;
            rVencRes.u4Width  = prVEncOmxInfo->u4Width;
            rVencRes.u4Height = prVEncOmxInfo->u4Height;
            vDrvVrmSetAppRes(VRM_APP_VIDEO_ENC, rVencRes);
        }    
        #else
        vScpipSetForcedHpsFactor(VDP_2,0x8000);
        #endif
        
//////////////////////////////
#ifdef SET_VDP_BY_DRIVER
        VDP_SetEnable(VDP_2, TRUE);
        VDP_SetDisplayOff_Ext(VDP_2, DRV_DISP_OFF_MODULE_VENC,TRUE);
        VDP_SetOutRegion(VDP_2, (UCHAR)TRUE, rOutRegion);
        VDP_SetSrcRegion(VDP_2, (UCHAR)TRUE, rSrcRegion);
        rOverscan.u4Top = 0;
        rOverscan.u4Bottom = 0;
        rOverscan.u4Left = 0;
        rOverscan.u4Right = 0;
        VDP_SetOverScan(VDP_2, rOverscan);
#endif
/////////////////////////////
    }
    else
    {
        vDrvVrmClrAppFlag(VRM_APP_VIDEO_ENC);

        if (prVEncOmxInfo->eProfileDef == VENC_PROFILE_MS)
        {
#if (!defined(CC_MT5396)) && defined(CC_OSD_ENCODE)
            if (prVEncOmxInfo->fgEncOSTG)
            {
                bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_MAX);
                vCCIROSDOnOff(SV_ON);
            }
            else
#endif
            {
                bSubSrcCloneMode(SV_OFF);
            }
        }
        else if (prVEncOmxInfo->eProfileDef == VENC_PROFILE_PVR)
        {
            return VENCR_OK;
        }
#if defined(CC_MT5396)
        vScpipSetForcedHpsFactor(VDP_2,0);
#endif

//////////////////////////////
#ifdef SET_VDP_BY_DRIVER
        VDP_SetDisplayOff_Ext(VDP_2, DRV_DISP_OFF_MODULE_VENC,FALSE);
        VDP_SetEnable(VDP_2, FALSE);
#endif
///////////////////
    }
    return VENCR_OK;
    // to be added for VDP by driver
    // 1. soruce change CB
    // 2. HDMI VGA color space trnasform
}

static void _VENCGetInRes(VENCOMX_INFO_T *prVEncOmxInfo, UINT32* u4InWidth, UINT32* u4InHeight)
{
    UINT32 u4Width  = (UINT32) wApiVideoGetInputWidth(VDP_2); 
    UINT32 u4Height = (UINT32) wApiVideoGetInputHeight(VDP_2); 
    
#if !defined(CC_MT5396)
    *u4InWidth  = MIN(u4Width,  prVEncOmxInfo->u4Width);
    *u4InHeight = MIN(u4Height, prVEncOmxInfo->u4Height);
#else
    *u4InWidth = u4Width;
    *u4InHeight = u4Height;
#endif
}


static INT32 _VENCConnectVDP(VENCOMX_INFO_T *prVEncOmxInfo)
{
    vDrvDISupportH264Init();

    vDrvDISetVEncHandle(
        (void *)prVEncOmxInfo->hVEnc,
        (void *)prVEncOmxInfo->ptFrmBuffMgr,
        (void *)prVEncOmxInfo->ptResizeFrmBuffMgr,
        (void *)prVEncOmxInfo->ptFilter);

#ifdef CC_ATV_PVR_SUPPORT
    if (VENC_PROFILE_PVR == prVEncOmxInfo->eProfileDef)
    {
        // trigger VBI
        VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, TRUE);
    }
#endif

    return VENCR_OK;
}


static INT32 _VENCDisconnectVDP(VENCOMX_INFO_T *prVEncOmxInfo)
{

#ifdef CC_ATV_PVR_SUPPORT
    if (VENC_PROFILE_PVR == prVEncOmxInfo->eProfileDef)
    {
        // stop VBI
        VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, FALSE);
    }
#endif

    // stop DI write mode
    vDrvDIStopSendBufToVENC(TRUE);
    return _VENCSetVDP(prVEncOmxInfo, SV_OFF);
}


static INT32 _VENCSetParamVDP(VENCOMX_INFO_T *prVEncOmxInfo)
{
    return _VENCSetVDP(prVEncOmxInfo, SV_ON);
}

static UINT32 _VENCH264CustPlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4Fps;
    UINT32 uSrcFrameRate;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth = 0;
    UINT32 u4SrcHeight = 0;
    UINT32 u4TgtPitch;
    UINT32 u4TgtWidth;
    UINT32 u4TgtHeight;
    BOOL fgAddAud;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_H264_PRAM_PL_T tProfileLevel;
    VENC_CTRL_PIXFMT_T pix;
    VENC_CTRL_RZ_T rRz;
    BOOL fgResize = FALSE;
    BOOL fgInterlace = bDrvVideoIsSrcInterlace(VDP_2)? TRUE : FALSE;

    Printf("%s start ....\n",__FUNCTION__);
    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        VENCOMX_DBG("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prVEncOmxInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_CUST, fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        VENCOMX_DBG("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prVEncOmxInfo->hVEnc = hVenc;
#if 0
    {
        UINT16 u2MsgQIdx;
		MTVENC_CB_T rCbData;
		SIZE_T zMsgSize=sizeof(MTVENC_CB_T);
		while (x_msg_q_receive(&u2MsgQIdx, &rCbData, &zMsgSize,
				&prVEncOmxInfo->hDataQ, 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK);
    }
#endif
    u4TgtWidth = prVEncOmxInfo->u4Width;
    u4TgtHeight = prVEncOmxInfo->u4Height;
    u4TgtPitch = VENC_ALIGN_MASK(u4TgtWidth, 15);
    //_MtVEncGetDramWidthHeight(&u4SrcWidth, &u4SrcHeight);
	u4SrcWidth = u4TgtWidth;
	u4SrcHeight = u4TgtHeight;

    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);

    uSrcFrameRate = bDrvVideoGetRefreshRate(VDP_2);
    
    prVEncOmxInfo->u4SrcWidth = u4SrcWidth;
    prVEncOmxInfo->u4SrcHeight = u4SrcHeight;

    VENCOMX_DBG("-Set fps=%d, src fps=%d, fgInterlace=%d\n",prVEncOmxInfo->u4Fps,uSrcFrameRate,fgInterlace);
    if(uSrcFrameRate>30)
    { 
        if(fgInterlace==FALSE)
        {
            vDrvDIVencDropFrame(1);
        }
    }
    
    if(prVEncOmxInfo->u4Fps==0)
    {
        prVEncOmxInfo->u4Fps=uSrcFrameRate;
    }
    if(prVEncOmxInfo->u4Fps>30)
    {
       prVEncOmxInfo->u4Fps=prVEncOmxInfo->u4Fps/2;
       if(fgInterlace==FALSE)
       {
           vDrvDIVencDropFrame(1);
       }
    }
    
    u4Fps = prVEncOmxInfo->u4Fps;
    prVEncOmxInfo->fgInterlace = fgInterlace;
    
    if (u4TgtWidth * u4TgtHeight * u4Fps > 1280 * 736 * 30)
    {
        vDrvDIVencDropFrame(1);
    }
    if ((u4SrcWidth != u4TgtWidth) || (u4SrcHeight != u4TgtHeight))
    {
        fgResize = TRUE;
        VENCOMX_DBG("- %dx%d resize to %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4TgtWidth, u4TgtHeight, u4Fps);
    }
    else
    {
        VENCOMX_DBG("- %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4Fps);
    }

    u4Value = u4Fps;
    VENCOMX_DBG("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }
    //##bitrate.
    u4Value = prVEncOmxInfo->u4Bps;
    VENCOMX_DBG("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4TgtWidth;
    res.u2Height = (UINT16)u4TgtHeight;
    VENCOMX_DBG("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }
    //##profile level
    tProfileLevel.u2Profile = prVEncOmxInfo->u4Profile;
    tProfileLevel.u2Level = prVEncOmxInfo->u4Level;
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_PROFILELEVEL, (VOID*)&tProfileLevel);
    fgAddAud = prVEncOmxInfo->fgAddAud;
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_ADDAUD, (VOID*)&fgAddAud);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncOmxInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4TgtPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       VENCOMX_DBG("- Set pixfmt failed(%d)!\n", u4Ret);
       return VENCR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }
	prVEncOmxInfo->fg2Usr = TRUE;

    //Output Callback
    rCb.pvData = (VOID *)prVEncOmxInfo;
    rCb.cb     = _VENCCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //_VEncSetApType(prVEncOmxInfo, (VOID *)prVEncOmxInfo->eProfileDef, 0);

//set buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prVEncOmxInfo->ptFrmBuffMgr);
    prVEncOmxInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncOmxInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(u4TgtWidth, 15) * VENC_ALIGN_MASK(u4TgtHeight, 31);

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 8; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prVEncOmxInfo->ptFrmBuffMgr, &rFrm);
    }

    if (fgResize)
    {
        ASSERT(NULL == prVEncOmxInfo->ptResizeFrmBuffMgr);
        prVEncOmxInfo->ptResizeFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_RZ_FRM);
        ASSERT(prVEncOmxInfo->ptResizeFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptResizeFrmBuffMgr);
        u4Size = 1920 * 1088;

        for (u4Idx = 0; u4Idx < VENC_RESIZE_BUF_N; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prVEncOmxInfo->ptResizeFrmBuffMgr, &rFrm);
        }
    }
    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    if (fgResize)
    {
        rRz.u4SrcPitch = u4SrcPitch;
        rRz.u4SrcWidth = u4SrcWidth;
        rRz.u4SrcHeight = u4SrcHeight;
        rRz.pvIn = (void *)prVEncOmxInfo->ptResizeFrmBuffMgr;
        rRz.pvOut = (void *)prVEncOmxInfo->ptFrmBuffMgr;
        u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_RZ, (VOID*)&rRz);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- Set resize error(%d)!\n", u4Ret);
            return -1;
        }
    }

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}

static INT32 _VENCSetEncodeOsd(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData)
{
    BOOL fgEncOsd;
    fgEncOsd = pvData? TRUE : FALSE;
#if !defined(CC_MT5396)
#ifdef CC_OSD_ENCODE
    vCCIROSDOnOff(fgEncOsd);
#endif
#endif
    VENCOMX_DBG(" - encode OSD is %s\n", fgEncOsd? "on" : "off");
    return VENCR_OK;
}

static INT32 _VENCSetMsiOSTG(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData)
{
#if !defined(CC_MT5396)
#ifdef CC_OSD_ENCODE
    BOOL fgEncOSTG;
    fgEncOSTG = pvData? TRUE : FALSE;
    prVEncOmxInfo->fgEncOSTG= fgEncOSTG;
    bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_OSDENCODE1);

    vSetPipFlg(PIP_FLG_MODE_CHG);
    vSetPipFlg(PIP_FLG_MODE_DET_DONE);
    VENCOMX_DBG(" - encode OSTG is %s\n", fgEncOSTG? "on" : "off");
    return VENCR_OK;
#else
    prVEncOmxInfo->fgEncOSTG = FALSE;
    VENCOMX_DBG(" CC_OSD_ENCODE is not defined !! ");
    return VENCR_NOT_OK;
#endif
#else
    prVEncOmxInfo->fgEncOSTG = FALSE;
    VENCOMX_DBG(" MsiOSTG not support ");
    return VENCR_OK;
#endif
}
#endif

// map from VENC_H264_PIC_TYPE to VENC_H264_PIC_TYPE
static VENC_H264_PIC_TYPE _VENC_PicTypeMap(VENC_H264_PIC_TYPE ePicType)
{
    VENCOMX_H264_PIC_TYPE eType = VENC_PIX_FMT_MAX;

    switch (ePicType)
    {
    default:
        VENCOMX_DBG("Unknown ePicType(%d)\n", ePicType);
        eType = VENCOMX_H264_PIC_TYPE_MAX;
        break;
    case VENC_H264_PIC_TYPE_SPS:
        eType = VENCOMX_H264_PIC_TYPE_SPS;
        break;
    case VENC_H264_PIC_TYPE_PPS:
        eType = VENCOMX_H264_PIC_TYPE_PPS;
        break;
    case VENC_H264_PIC_TYPE_SEI:
        eType = VENCOMX_H264_PIC_TYPE_SEI;
        break;
    case VENC_H264_PIC_TYPE_IDR:
        eType = VENCOMX_H264_PIC_TYPE_IDR;
        break;
    case VENC_H264_PIC_TYPE_I:
        eType = VENCOMX_H264_PIC_TYPE_I;
        break;
    case VENC_H264_PIC_TYPE_P:
        eType = VENCOMX_H264_PIC_TYPE_P;
        break;
    case VENC_H264_PIC_TYPE_B:
        eType = VENCOMX_H264_PIC_TYPE_B;
        break;
    }

    return eType;
}

static VOID _VENCCb(VOID *pvArg, VENC_IN_PIC_T * ptIn, VENC_OUT_PIC_T * ptOut, UINT32 type)
{
    VENCOMX_INFO_T *prVEncOmxInfo = (VENCOMX_INFO_T *)pvArg;
    if (ptIn)
    {
        VENC_FRM_T rFrm;
        rFrm.u4YAddr  = ptIn->u4YAddr;
        rFrm.u4UVAddr = ptIn->u4CAddr;
        _BUFMGR_OBJ_Free(prVEncOmxInfo->ptFrmBuffMgr, (VOID *)&rFrm);
        //Printf("_emu_demo_cb free: src(Y,UV) => (0x%08x, 0x%08x)\n", ptIn->u4YAddr, ptIn->u4CAddr);
    }

#ifdef CC_SUPPORT_MUXER
    if (ptOut && (0xFF != prVEncOmxInfo->ucMxId))
    {
        VENC_PES_INFO_T rVEncPes;
        x_memset(&rVEncPes, 0, sizeof(rVEncPes));
        rVEncPes.u4FifoStart = ptOut->u4RStart;
        rVEncPes.u4FifoEnd   = ptOut->u4REnd;
        rVEncPes.u4StartAddr = ptOut->u4Addr;
        rVEncPes.u4FrameSize = ptOut->u4Size;
        rVEncPes.u4VideoType = type;
        rVEncPes.u4Pts       = ptOut->u4Pts;
        rVEncPes.pvTag       = prVEncOmxInfo->hVEnc;
        MUXER_SendVideoPes(prVEncOmxInfo->ucMxId, &rVEncPes);
    }
#endif

    if (ptOut && prVEncOmxInfo->fg2Usr)
    {
        VENCOMX_CB_T tVencCb;
        tVencCb.eCond = VENCOMX_COND_DATA;
        tVencCb.u.tOut.eType    = _VENC_PicTypeMap(type);
        tVencCb.u.tOut.u4Pts    = ptOut->u4Pts;
        tVencCb.u.tOut.u4Addr   = ptOut->u4Addr;
        tVencCb.u.tOut.u4Size   = ptOut->u4Size;
        tVencCb.u.tOut.u4RStart = ptOut->u4RStart;
        tVencCb.u.tOut.u4REnd   = ptOut->u4REnd;

        _CB_PutEvent(CB_MTAL_VENC_DATA_NFY, sizeof(tVencCb), &tVencCb);
    }
}

//#define TMP_DRIVER_SET_SUB

static UINT32 u4DramWidth, u4DramHeight;
#ifdef TMP_DRIVER_SET_SUB

static void _VENCSetPredown(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UINT32 u4SrcWidth =  wDrvVideoInputWidth(VDP_1);
    UINT32 u4SrcHeight = wDrvVideoInputHeight(VDP_1);
    UINT32 u4Factor;

    u4DramWidth = u4SrcWidth;
    u4DramHeight = u4SrcHeight;
    if (u4SrcWidth > prVEncOmxInfo->u4Width)
    {
        u4Factor = prVEncOmxInfo->u4Width * 0x8000 / u4SrcWidth;
        u4DramWidth = prVEncOmxInfo->u4Width;
        vScpipSetForcedHpsFactor(VDP_2, u4Factor);
    }
    else
    {
        vScpipSetForcedHpsFactor(VDP_2, 0x8000);
    }
    
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)
    if (u4SrcHeight > prVEncOmxInfo->u4Height)
    {
        u4Factor = prVEncOmxInfo->u4Height * 0x8000 / u4SrcHeight;
        u4DramHeight = prVEncOmxInfo->u4Height;
        //vScpipSetForcedVpsFactor(VDP_2, u4Factor);
    }
    else
    {
        //vScpipSetForcedVpsFactor(VDP_2, 0x8000);
    }
#endif

}
#endif

static void _VENCGetDramWidthHeight(UINT32 *u4Width, UINT32 *u4Height)
{
    if (u4DramWidth == 0)
    {
        u4DramWidth =  wDrvVideoInputWidth(VDP_2);
        printf("\n u4DramWidth == 0 \n");
    }

    if (u4DramHeight == 0)
    {
        u4DramHeight = wDrvVideoInputHeight(VDP_2);
        printf("\n u4DramHeight == 0 \n");
    }
    
    *u4Width = u4DramWidth;
#if 0
    if (u1DrvDIIsVencIntrProgMode())
    {
        *u4Height = (u4DramHeight>>1);
    }
    else
#endif
    {
        *u4Height = u4DramHeight;
    }
}

//--input source settings
/// need to remove setting sub source at driver, disable this define to do this
#ifdef TMP_DRIVER_SET_SUB
static INT32 _VENCSetSubSrc(UINT8 u1OnOff)
{
    VDP_REGION_T rRegion;
    VDP_OVERSCAN_REGION_T rOvRegion;
//    UINT32 u4WaitCnt = 0;
    UINT8 u1MainSrc;
    static SRM_TV_MODE_T rOldSrmMode = SRM_TV_MODE_TYPE_NORMAL;
    static UINT8 u1OldSubSrc = SV_VS_ATV1;
    static UCHAR ucEnable;
    rRegion.u4X = 0;
    rRegion.u4Y = 0;

    if (u1OnOff)
    {
        rOldSrmMode = SRM_GetTvMode();
        u1OldSubSrc= bApiVideoGetSrcType(VDP_2);
        u1MainSrc = bApiVideoGetSrcType(VDP_1);
        SRM_SetTvMode(SRM_TV_MODE_TYPE_PIP);
        //Bypass_HalPIPClk(0);
        rRegion.u4Width = VDP_MAX_REGION_WIDTH;
        rRegion.u4Height = VDP_MAX_REGION_HEIGHT;
        VDP_SetOutRegion(VDP_2, 0, rRegion);
        VDP_GetEnable(VDP_2, &ucEnable);
        VDP_SetEnable(VDP_2, TRUE);
        VDP_SetDisplayOff(VDP_2, TRUE);
        rOvRegion.u4Top = 0;
        rOvRegion.u4Bottom = 0;
        rOvRegion.u4Left = 0;
        rOvRegion.u4Right = 0;
        VDP_SetOverScan(VDP_2, rOvRegion);
        bApiVideoSetVideoSrc(SV_VP_PIP, u1MainSrc);
        rRegion.u4Width = VDP_MAX_REGION_WIDTH;
        rRegion.u4Height = VDP_MAX_REGION_HEIGHT;
        VDP_SetSrcRegion(VDP_2,0, rRegion);
        vClrPipFlg(PIP_FLG_MODE_DET_DONE);
        vSetPipFlg(PIP_FLG_MODE_CHG);
        //u1ForceHPSDByCli = 1;
        //vScpipSetForcedHpsFactor(VDP_2, 0x8000);
#if 1
            x_thread_delay(500);
#else
        do{
            x_thread_delay(100);
            if (++u4WaitCnt > 40)
            {
                 VENCOMX_DBG("Wait for Mode Chg too long (Chg Sub Src to Main) \n");
                 return VENCR_NOT_OK;
            }
        }while( bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
                bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE ||
                _u4DrvVideoGetMute(VDP_1) == TRUE);
#endif
    }
    else
    {
        SRM_SetTvMode(rOldSrmMode);
        //vScpipSetForcedHpsFactor(VDP_2, 0);
        //u1ForceHPSDByCli = 0;
        VDP_SetDisplayOff(VDP_2, FALSE);
        bApiVideoSetVideoSrc(SV_VP_PIP, u1OldSubSrc);
        VDP_SetEnable(VDP_2, ucEnable);
        if (!ucEnable)
        {
            rRegion.u4Width = 0;
            rRegion.u4Height = 0;
            VDP_SetOutRegion(VDP_2, 0, rRegion);
        }
#if 1
            x_thread_delay(500);
#else

        do{
            x_thread_delay(100);
            if (++u4WaitCnt > 40)
            {
                 VENCOMX_DBG("Wait for Mode Chg too long (Chg Sub Src back) \n");
                 return VENCR_NOT_OK;
            }
        }while(bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
               _u4DrvVideoGetMute(VDP_1) == TRUE);
#endif
    }

    return VENCR_OK;
}

//extern UINT8 u1HdmiColorMode;
static INT32 _VENCSetYCbCr(UINT8 u1OnOff)
{
#if 0
    static UINT8 fgIsHDMIChg = 0, u1OldHdmiMode = SV_HDMI_MODE_AUTO;
    static UINT8 fgIsVGAChg = 0;
//    UINT32 u4WaitCnt = 0;

    if (u1OnOff)
    {
        if (SV_VD_DVI == bGetVideoDecType(VDP_2) && bHDMIInputType() == 1 && bIsScalerInput444(VDP_2))
        {
            u1OldHdmiMode = u1HdmiColorMode;
            u1HdmiColorMode = SV_HDMI_MODE_VIDEO ;
            vApiHdmiColorModeChg(VDP_2);
            fgIsHDMIChg = 1;
        }
        else if (SV_VD_VGA == bGetVideoDecType(VDP_2))
        {
            if(u1GetVGA422En() != SV_TRUE)
            {
                vVgaTriggerModeChange();
                vSetVGA422En(SV_TRUE);
                fgIsVGAChg = 1;
            }
        }

        do{
            x_thread_delay(100);
            if (++u4WaitCnt > 40)
            {
                 VENCOMX_DBG("Wait for Mode Chg too long (422 to 444) \n");
                 return VENCR_NOT_OK;
            }
        }while( bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
                bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE ||
                _u4DrvVideoGetMute(VDP_1) == TRUE);
    }
    else
    {
        if (fgIsHDMIChg && SV_VD_DVI == bGetVideoDecType(VDP_1))
        {
            fgIsHDMIChg = 0;
            u1HdmiColorMode = u1OldHdmiMode;
            vApiHdmiColorModeChg(VDP_1);
        }
        else if (SV_VD_VGA == bGetVideoDecType(VDP_1))
        {
            if (fgIsVGAChg)
            {
                fgIsVGAChg = 0;
                vVgaTriggerModeChange();
                vSetVGA422En(SV_FALSE);
            }
        }

        do{
            x_thread_delay(100);
            if (++u4WaitCnt > 40)
            {
                 VENCOMX_DBG("Wait for Mode Chg too long (444 to 422) \n");
                 return VENCR_NOT_OK;
            }
        }while( bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
                _u4DrvVideoGetMute(VDP_1) == TRUE);
    }

#endif
    return VENCR_OK;
}

#endif

static INT32 _VENCConnectDi(VENCOMX_INFO_T *prVEncOmxInfo)
{
    vDrvDISupportH264Init();

    vDrvDISetVEncHandle(
        (void *)prVEncOmxInfo->hVEnc,
        (void *)prVEncOmxInfo->ptFrmBuffMgr,
        (void *)prVEncOmxInfo->ptResizeFrmBuffMgr,
        (void *)prVEncOmxInfo->ptFilter);

#ifdef CC_ATV_PVR_SUPPORT
    // trigger VBI
    //VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, TRUE);
#endif

    return VENCR_OK;
}


static INT32 _VENCDisconnectDi(VENCOMX_INFO_T *prVEncOmxInfo)
{
    INT32 i4Ret = VENCR_OK;

#ifdef CC_ATV_PVR_SUPPORT
    // stop VBI
    //VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, FALSE);
#endif

    // stop DI write mode
    vDrvDIStopSendBufToVENC(TRUE);

#ifdef TMP_DRIVER_SET_SUB
    i4Ret = _VENCSetSubSrc(FALSE);
    if (i4Ret != VENCR_OK)
    {
        return i4Ret;
    }
    i4Ret = _VENCSetYCbCr(FALSE);
    if (i4Ret != VENCR_OK)
    {
        return i4Ret;
    }
#endif
    if (prVEncOmxInfo->eProfileDef == VENC_PROFILE_MS)
    {
        //bSubSrcCloneMode(SV_OFF);
    }
    return i4Ret;
}


static INT32 _VENCSetParamDi(VENCOMX_INFO_T *prVEncOmxInfo)
{
#ifdef TMP_DRIVER_SET_SUB
    INT32 i4Ret;
    _VENCSetPredown(prVEncOmxInfo);
    i4Ret = _VENCSetSubSrc(TRUE);
    if (i4Ret != VENCR_OK)
    {
        return i4Ret;
    }

    i4Ret = _VENCSetYCbCr(TRUE);
    if (i4Ret != VENCR_OK)
    {
        return i4Ret;
    }
#endif

    if (prVEncOmxInfo->eProfileDef == VENC_PROFILE_MS)
    {
        //bSubSrcCloneMode(SV_ON);
    }

#if 0
x_thread_delay(100);
#else
    if (bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE)
    {
        VENCOMX_DBG(" Signal Not Stable Cannot Continue\n");
        return VENCR_NOT_OK;
    }
#endif
    return VENCR_OK;
}







#ifdef VDEC_VENC_PATH
static INT32 _VENCConnectVdec(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UCHAR ucEsId = prVEncOmxInfo->u2SrcCompId;

    if (ucEsId >= (UINT16)VDEC_MAX_ES)
    {
        return VENCR_NOT_OK;
    }

    _VDEC_SetVencHandle(
        ucEsId,
        (VOID *)prVEncOmxInfo->hVEnc,
        (VOID *)prVEncOmxInfo->ptFrmBuffMgr,
        (VOID *)prVEncOmxInfo->ptFilter);
    _VDEC_SetVencFrmSz(ucEsId, prVEncOmxInfo->u4Width, prVEncOmxInfo->u4Height, prVEncOmxInfo->u4Width);
    _VDEC_Set2VencFlow(ucEsId, TRUE);

    return VENCR_OK;
}


static INT32 _VENCDisconnectVdec(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UCHAR ucEsId = prVEncOmxInfo->u2SrcCompId;
    _VDEC_Set2VencFlow(ucEsId, FALSE);

    return VENCR_OK;
}


#if 0
static INT32 _MtVEncSetParamVdec(MTVENC_INFO_T *prVEncOmxInfo)
{
    return VENCR_OK;
}
#endif

#endif /*VDEC_VENC_PATH*/

static INT32 _VENCSetH264Info(
    VENCOMX_INFO_T *prVEncOmxInfo,
    VOID *pvFmt)
{
    UINT32 u4Width, u4Height, u4Fps, u4Bps;
    VENCOMX_COMMON_INFO_T  *ptCmmInfo;
    VENCOMX_FRAME_RESOLUTION_T  *ptResolution;
    VENCOMX_H264_INFO_T *ptFmtH264 = (VENCOMX_H264_INFO_T *)pvFmt;


    if (NULL == prVEncOmxInfo)
    {
        VENCOMX_DBG("- prVEncOmxInfo is NULL\n");
        return VENCR_NOT_OK;
    }

    if (NULL == ptFmtH264)
    {
        VENCOMX_DBG("- ptFmtH264 is NULL\n");
        return VENCR_NOT_OK;
    }

    ptCmmInfo = &ptFmtH264->tCmmInfo;
    u4Bps           = ptCmmInfo->u4Bps;
    u4Fps           = ptCmmInfo->u4Fps;
    ptResolution   = &ptCmmInfo->tResolution;
    u4Width         = ptResolution->u4Width;
    u4Height        = ptResolution->u4Height;

    if (prVEncOmxInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_RES;
        rRTParm.u2Width     = u4Width;
        rRTParm.u2Height    = u4Height;
        rRTParm.u2Pitch     = VENC_ALIGN_MASK(u4Width, 15);

        //bitrate
        rRTParm.u4Flag     |= VENC_RT_PARM_FLAG_BITRATE;
        rRTParm.u4Bitrate   = u4Bps;

        //framterate
        rRTParm.u4Flag     |= VENC_RT_PARM_FLAG_FRAMERATE;
        rRTParm.u1Framerate = u4Fps;

        if (VENC_OK != VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm))
        {
            return VENCR_NOT_OK;
        }
    }
    else
    {
        prVEncOmxInfo->u4Profile = 0;
        prVEncOmxInfo->u4Level   = 0;
        prVEncOmxInfo->u4Width   = u4Width;
        prVEncOmxInfo->u4Height  = u4Height;
        prVEncOmxInfo->u4Fps     = u4Fps;
        prVEncOmxInfo->u4Bps     = u4Bps;
    }

    return VENCR_OK;
}

static INT32 _VENCSetEncFmt(
    VENCOMX_INFO_T *prVEncOmxInfo,
    VENCOMX_FMT_TYPE_T eEncFmt)
{
    UINT32 type = 0;
    switch (eEncFmt)
    {
    default:
        prVEncOmxInfo->SetFmtInfo = NULL;
        return VENCR_NOT_OK;

    case VENCOMX_FMT_H264:
        type = VENC_H264;
        prVEncOmxInfo->SetFmtInfo = _VENCSetH264Info;
        break;
    }
    prVEncOmxInfo->u4EncType = type;
    prVEncOmxInfo->eEncFmt = eEncFmt;
    return VENCR_OK;
}

static INT32 _VENCSetSrcType(
    VENCOMX_INFO_T *prVEncOmxInfo,
    VENCOMX_SRC_TYPE_T eSrcType)
{
    switch (eSrcType)
    {
    default:
        return VENCR_NOT_OK;

    case VENCOMX_SRC_TYPE_VDEC:
        break;

    case VENCOMX_SRC_TYPE_VDP:
        break;
    }

    prVEncOmxInfo->eSrcType = eSrcType;

    return VENCR_OK;
}

static INT32 _VENCSetFmtInfo(
    VENCOMX_INFO_T *prVEncOmxInfo,
    VOID *pvFmt)
{

    if (NULL == prVEncOmxInfo->SetFmtInfo)
    {
        return VENCR_NOT_OK;
    }

    return prVEncOmxInfo->SetFmtInfo(prVEncOmxInfo, pvFmt);
}

static INT32 _VENCH264VphonePlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth;
    UINT32 u4SrcHeight;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;

    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        VENCOMX_DBG("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prVEncOmxInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_VPHONE, FALSE);
    if (VENC_NULL_HANDLE == hVenc)
    {
        VENCOMX_DBG("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prVEncOmxInfo->hVEnc = hVenc;

    u4SrcWidth = prVEncOmxInfo->u4Width;
    u4SrcHeight = prVEncOmxInfo->u4Height;
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);

    u4Value = prVEncOmxInfo->u4Fps;
    VENCOMX_DBG("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prVEncOmxInfo->u4Bps;
    VENCOMX_DBG("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4SrcWidth;
    res.u2Height = (UINT16)u4SrcHeight;
    VENCOMX_DBG("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncOmxInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4SrcPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       VENCOMX_DBG("- Set pixfmt failed(%d)!\n", u4Ret);
       return VENCR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prVEncOmxInfo;
    rCb.cb     = _VENCCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

//set buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prVEncOmxInfo->ptFrmBuffMgr);
    prVEncOmxInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncOmxInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(u4SrcWidth, 15) * VENC_ALIGN_MASK(u4SrcHeight, 31);
    u4Size = MAX(u4Size, _VENC_MAX_RES); //for changing res in runtime
    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 4; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prVEncOmxInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}

static INT32 _VENCH264AtvPvrPlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4Fps;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth;
    UINT32 u4SrcHeight;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;
    BOOL fgInterlace = bDrvVideoIsSrcInterlace(VDP_2)? TRUE : FALSE;

    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        VENCOMX_DBG("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prVEncOmxInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_PVR, fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        VENCOMX_DBG("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prVEncOmxInfo->hVEnc = hVenc;

    u4SrcWidth = wDrvVideoInputWidth(VDP_2);//u2DrvDIGetVdpWidth(1);
    u4SrcHeight = wDrvVideoInputHeight(VDP_2); //u2DrvDIGetVdpHeight(1);
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4Fps = bDrvVideoGetRefreshRate(VDP_2);
    prVEncOmxInfo->u4SrcWidth = u4SrcWidth;
    prVEncOmxInfo->u4SrcHeight = u4SrcHeight;
    prVEncOmxInfo->u4SrcFps = u4Fps;
    prVEncOmxInfo->u4Width = u4SrcWidth;
    prVEncOmxInfo->u4Height = u4SrcHeight;
    prVEncOmxInfo->fgInterlace = fgInterlace;
    if (fgInterlace)
    {
        u4Fps >>= 1;
    }
    else if (u4SrcWidth * u4SrcHeight * u4Fps > 1280 * 736 * 30)
    {
        u4Fps >>= 1;
        //vDrvDIVencDropFrame(1);
    }
    VENCOMX_DBG("- %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4Fps);
    prVEncOmxInfo->u4Fps = u4Fps;
    u4Value = u4Fps;
    VENCOMX_DBG("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prVEncOmxInfo->u4Bps;
    VENCOMX_DBG("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4SrcWidth;
    res.u2Height = (UINT16)u4SrcHeight;
    VENCOMX_DBG("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncOmxInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4SrcPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       VENCOMX_DBG("- Set pixfmt failed(%d)!\n", u4Ret);
       return VENCR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prVEncOmxInfo;
    rCb.cb     = _VENCCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

//set buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prVEncOmxInfo->ptFrmBuffMgr);
    prVEncOmxInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncOmxInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(u4SrcWidth, 15) * VENC_ALIGN_MASK(u4SrcHeight, 31);

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 8; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prVEncOmxInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}

static UINT32 _VENCH264MsPlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4Fps;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth = 0;
    UINT32 u4SrcHeight = 0;
    UINT32 u4TgtPitch;
    UINT32 u4TgtWidth;
    UINT32 u4TgtHeight;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;
    VENC_CTRL_RZ_T rRz;
    BOOL fgResize = FALSE;
    BOOL fgInterlace = bDrvVideoIsSrcInterlace(VDP_2)? TRUE : FALSE;

    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        VENCOMX_DBG("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prVEncOmxInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_MS, fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        VENCOMX_DBG("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prVEncOmxInfo->hVEnc = hVenc;

    u4TgtWidth = prVEncOmxInfo->u4Width;
    u4TgtHeight = prVEncOmxInfo->u4Height;
    u4TgtPitch = VENC_ALIGN_MASK(u4TgtWidth, 15);
    //u4SrcWidth = wDrvVideoInputWidth(VDP_2);//u2DrvDIGetVdpWidth(1);
    //u4SrcHeight = wDrvVideoInputHeight(VDP_2); //u2DrvDIGetVdpHeight(1);
    _VENCGetDramWidthHeight(&u4SrcWidth, &u4SrcHeight);

    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4Fps = bDrvVideoGetRefreshRate(VDP_2);

    prVEncOmxInfo->u4SrcWidth = u4SrcWidth;
    prVEncOmxInfo->u4SrcHeight = u4SrcHeight;
    prVEncOmxInfo->u4SrcFps = u4Fps;
    prVEncOmxInfo->fgInterlace = fgInterlace;
    if (fgInterlace)
    {
        u4Fps >>= 1;
    }
    else if (u4TgtWidth * u4TgtHeight * u4Fps > 1280 * 736 * 30)
    {
        u4Fps >>= 1;
        //vDrvDIVencDropFrame(1);
    }
    prVEncOmxInfo->u4Fps = u4Fps;

    if ((u4SrcWidth != u4TgtWidth) || (u4SrcHeight != u4TgtHeight))
    {
        fgResize = TRUE;
        VENCOMX_DBG("- %dx%d resize to %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4TgtWidth, u4TgtHeight, u4Fps);
    }
    else
    {
        VENCOMX_DBG("- %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4Fps);
    }

    u4Value = u4Fps;
    VENCOMX_DBG("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prVEncOmxInfo->u4Bps;
    VENCOMX_DBG("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4TgtWidth;
    res.u2Height = (UINT16)u4TgtHeight;
    VENCOMX_DBG("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncOmxInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4TgtPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       VENCOMX_DBG("- Set pixfmt failed(%d)!\n", u4Ret);
       return VENCR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prVEncOmxInfo;
    rCb.cb     = _VENCCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

//set buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prVEncOmxInfo->ptFrmBuffMgr);
    prVEncOmxInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncOmxInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(u4TgtWidth, 15) * VENC_ALIGN_MASK(u4TgtHeight, 31);

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 4; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prVEncOmxInfo->ptFrmBuffMgr, &rFrm);
    }

    if (fgResize)
    {
        ASSERT(NULL == prVEncOmxInfo->ptResizeFrmBuffMgr);
        prVEncOmxInfo->ptResizeFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_RZ_FRM);
        ASSERT(prVEncOmxInfo->ptResizeFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptResizeFrmBuffMgr);
        u4Size = 1920 * 1088;

        for (u4Idx = 0; u4Idx < VENC_RESIZE_BUF_N; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prVEncOmxInfo->ptResizeFrmBuffMgr, &rFrm);
        }
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    if (fgResize)
    {
        rRz.u4SrcPitch = u4SrcPitch;
        rRz.u4SrcWidth = u4SrcWidth;
        rRz.u4SrcHeight = u4SrcHeight;
        rRz.pvIn = (void *)prVEncOmxInfo->ptResizeFrmBuffMgr;
        rRz.pvOut = (void *)prVEncOmxInfo->ptFrmBuffMgr;
        u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_RZ, (VOID*)&rRz);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- Set resize error(%d)!\n", u4Ret);
            return -1;
        }
    }

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}  


static INT32 _VENCH264Play(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UINT32 u4Ret;

    //set input source parameters
    if (prVEncOmxInfo->SetInputParam && (VENCR_OK != prVEncOmxInfo->SetInputParam(prVEncOmxInfo)))
    {
        VENCOMX_DBG("- Set input source parameters failed!\n");
        return VENCR_NOT_OK;
    }

    switch (prVEncOmxInfo->eProfileDef)
    {
    default:
        VENCOMX_DBG("- unexpected profile(%d)!\n", prVEncOmxInfo->eProfileDef);
        ASSERT(0);
        return VENCR_NOT_OK;

    case VENC_PROFILE_VPHONE:
        u4Ret = _VENCH264VphonePlay(prVEncOmxInfo);
        break;

    case VENC_PROFILE_PVR:
        u4Ret = _VENCH264AtvPvrPlay(prVEncOmxInfo);
        break;

    case VENC_PROFILE_MS:
        u4Ret = _VENCH264MsPlay(prVEncOmxInfo);
        break;
    }

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- play fail %d!\n", u4Ret);
        ASSERT(0);
        return VENCR_NOT_OK;
    }

    //##connect input source
    if (prVEncOmxInfo->ConnectInput && (VENCR_OK != prVEncOmxInfo->ConnectInput(prVEncOmxInfo)))
    {
        VENCOMX_DBG("- Connect to input source failed!\n");
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}

static INT32 _VENCH264Stop(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UINT32 u4Ret;
    INT32 i4Ret = VENCR_OK;

    //disconnect  input source
    if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
    {
        VENCOMX_DBG("- disconnect input source failed!\n");
        return VENCR_NOT_OK;
    }

    //stop
    u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- venc stop failed!\n");
        i4Ret = VENCR_NOT_OK;
    }

    //CLOSE VENC
    u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
        i4Ret = VENCR_NOT_OK;
    }
    prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

    if (prVEncOmxInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
        prVEncOmxInfo->ptFrmBuffMgr = NULL;
    }

    if (prVEncOmxInfo->ptResizeFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
        prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
    }

    return i4Ret;
}

static INT32 _VENCSetCtrl(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData, SIZE_T u4Size)
{
    BOOL fgCtrl;
    INT32 i4Ret = VENCR_OK;

    fgCtrl = (pvData)? TRUE : FALSE;

    if (prVEncOmxInfo->fgIsRunning != fgCtrl)
    {
        if (fgCtrl)
        {
            switch (prVEncOmxInfo->eEncFmt)
            {
            default:
                VENCOMX_DBG("- unexpected type(%d)!\n", prVEncOmxInfo->eEncFmt);
                ASSERT(0);
                return VENCR_NOT_OK;

            case VENCOMX_FMT_H264:
                i4Ret = _VENCH264Play(prVEncOmxInfo);
                break;
            }
        }
        else
        {
            switch (prVEncOmxInfo->eEncFmt)
            {
            default:
                VENCOMX_DBG("- unexpected type(%d)!\n", prVEncOmxInfo->eEncFmt);
                ASSERT(0);
                return VENCR_NOT_OK;

            case VENCOMX_FMT_H264:
                i4Ret = _VENCH264Stop(prVEncOmxInfo);
                break;
            }
        }
        prVEncOmxInfo->fgIsRunning = fgCtrl;
    }

    return i4Ret;
}

static UINT32 _VENCH264AtvPvrRePlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UINT32 u4Ret;

#if 0
    if ((prVEncOmxInfo->u4SrcWidth == wDrvVideoInputWidth(VDP_2))
    && (prVEncOmxInfo->u4SrcHeight == wDrvVideoInputHeight(VDP_2))
    && (prVEncOmxInfo->u4SrcFps == bDrvVideoGetRefreshRate(VDP_2))
    && (prVEncOmxInfo->fgInterlace == (bDrvVideoIsSrcInterlace(VDP_2)? TRUE : FALSE)))
    {
        if (pfgConnect)
        {
            *pfgConnect = FALSE;
        }
        return VENC_OK; // no need to replay
    }

    if (pfgConnect)
    {
        *pfgConnect = TRUE;
    }
#endif 
    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
#if 1
        // stop DI write mode
        vDrvDIStopSendBufToVENC(TRUE);
#else
        //disconnect  input source
        if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
        {
            VENCOMX_DBG("- disconnect input source failed!\n");
            return VENC_INTERNAL_ERR;
        }
#endif

        if (prVEncOmxInfo->fgIsRunning)
        {
            u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                VENCOMX_DBG("- venc stop fail(%d)\n", u4Ret);
                return VENC_INTERNAL_ERR;
            }
            prVEncOmxInfo->fgIsRunning = FALSE;
        }

        u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
            return VENC_INV_HANDLE;
        }
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

        if (prVEncOmxInfo->ptFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
            prVEncOmxInfo->ptFrmBuffMgr = NULL;
        }

        if (prVEncOmxInfo->ptResizeFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
            prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
        }
    }

    u4Ret = _VENCH264AtvPvrPlay(prVEncOmxInfo);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- _VENCH264MsPlay fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    prVEncOmxInfo->fgIsRunning = TRUE;

    return VENC_OK;
}

static UINT32 _VENCH264MsRePlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UINT32 u4Ret;

#if 0
    if ((prVEncOmxInfo->u4SrcWidth == wDrvVideoInputWidth(VDP_2))
    && (prVEncOmxInfo->u4SrcHeight == wDrvVideoInputHeight(VDP_2))
    && (prVEncOmxInfo->u4SrcFps == bDrvVideoGetRefreshRate(VDP_2))
    && (prVEncOmxInfo->fgInterlace == (bDrvVideoIsSrcInterlace(VDP_2)? TRUE : FALSE)))
    {
        if (pfgConnect)
        {
            *pfgConnect = FALSE;
        }
        return VENC_OK; // no need to replay
    }

    if (pfgConnect)
    {
        *pfgConnect = TRUE;
    }
#endif 
    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
#if 1
        // stop DI write mode
        vDrvDIStopSendBufToVENC(TRUE);
#else
        //disconnect  input source
        if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
        {
            VENCOMX_DBG("- disconnect input source failed!\n");
            return VENC_INTERNAL_ERR;
        }
#endif

        if (prVEncOmxInfo->fgIsRunning)
        {
            u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                VENCOMX_DBG("- venc stop fail(%d)\n", u4Ret);
                return VENC_INTERNAL_ERR;
            }
            prVEncOmxInfo->fgIsRunning = FALSE;
        }

        u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
            return VENC_INV_HANDLE;
        }
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

        if (prVEncOmxInfo->ptFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
            prVEncOmxInfo->ptFrmBuffMgr = NULL;
        }

        if (prVEncOmxInfo->ptResizeFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
            prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
        }
    }

    u4Ret = _VENCH264MsPlay(prVEncOmxInfo);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- _VENCH264MsPlay fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    prVEncOmxInfo->fgIsRunning = TRUE;

    return VENC_OK;
}

static INT32 _VENCH264RePlay(VENCOMX_INFO_T *prVEncOmxInfo)
{
    UINT32 u4Ret;

    //set input source parameters
    if (prVEncOmxInfo->SetInputParam && (VENCR_OK != prVEncOmxInfo->SetInputParam(prVEncOmxInfo)))
    {
        VENCOMX_DBG("- Set input source parameters failed!\n");
        return VENCR_NOT_OK;
    }

    VENCOMX_DBG("- profile(%d)\n", prVEncOmxInfo->eProfileDef);
    switch (prVEncOmxInfo->eProfileDef)
    {
    default:
        VENCOMX_DBG("- unexpected profile(%d)!\n", prVEncOmxInfo->eProfileDef);
        ASSERT(0);
        return VENCR_NOT_OK;
#if 0 // not used yet
    case VENC_PROFILE_VPHONE:
        u4Ret = _VENCH264VphonePlay(prVEncOmxInfo);
        break;
#endif
    case VENC_PROFILE_PVR:
        u4Ret = _VENCH264AtvPvrRePlay(prVEncOmxInfo);
        break;

    case VENC_PROFILE_MS:
        u4Ret = _VENCH264MsRePlay(prVEncOmxInfo);
        break;
    }

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("-play fail %d!\n", u4Ret);
        ASSERT(0);
        return VENCR_NOT_OK;
    }

    //##connect input source
    if (prVEncOmxInfo->ConnectInput && (VENCR_OK != prVEncOmxInfo->ConnectInput(prVEncOmxInfo)))
    {
        VENCOMX_DBG("-Connect to input source failed!\n");
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}

static INT32 _VENCSetRestart(VENCOMX_INFO_T *prVEncOmxInfo)
{
    INT32 i4Ret = VENCR_OK;

    if (prVEncOmxInfo->fgIsRunning)
    {
        switch (prVEncOmxInfo->eEncFmt)
        {
        default:
            VENCOMX_DBG("- unexpected type(%d)!\n", prVEncOmxInfo->eEncFmt);
            ASSERT(0);
            return VENCR_NOT_OK;

        case VENCOMX_FMT_H264:
            i4Ret = _VENCH264RePlay(prVEncOmxInfo);
            break;
        }
    }
    else
    {
        switch (prVEncOmxInfo->eEncFmt)
        {
        default:
            VENCOMX_DBG("-unexpected type(%d)!\n", prVEncOmxInfo->eEncFmt);
            ASSERT(0);
            return VENCR_NOT_OK;

        case VENCOMX_FMT_H264:
            i4Ret = _VENCH264Play(prVEncOmxInfo);
            break;
        }
        prVEncOmxInfo->fgIsRunning = TRUE;
    }

    return i4Ret;
}

static INT32 _VENCSetSettings(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData, SIZE_T u4Size)
{
    INT32 i4Ret = VENCR_NOT_OK;
    VENCOMX_SETTINGS_T rSettings;

    VENCOMX_SET_VALIDATE(VENCOMX_SETTINGS_T);

    if (prVEncOmxInfo->fgIsRunning)
    {
        VENCOMX_DBG("-venc is running\n");
        return VENCR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pvData, VENCOMX_SETTINGS_T);
    COPY_FROM_USER_ARG(pvData, rSettings, VENCOMX_SETTINGS_T);

    i4Ret = _VENCSetEncFmt(prVEncOmxInfo, rSettings.eEncFmt);
    if (i4Ret != VENCR_OK)
    {
        VENCOMX_DBG("- error enc type [%d]\n", rSettings.eEncFmt);
        return i4Ret;
    }

    i4Ret = _VENCSetSrcType(prVEncOmxInfo, rSettings.eSrcType);
    if (i4Ret != VENCR_OK)
    {
        VENCOMX_DBG("- error src type [%d]\n", rSettings.eSrcType);
        return i4Ret;
    }

    i4Ret = _VENCSetFmtInfo(prVEncOmxInfo, (VOID *)&rSettings.uFmt);
    if (i4Ret != VENCR_OK)
    {
        VENCOMX_DBG("-error fmt info\n");
        return i4Ret;
    }

    return VENCR_OK;
}

static INT32 _VENCSetFrameResolution(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData, SIZE_T u4Size)
{
    VENCOMX_FRAME_RESOLUTION_T tResolution;
    UINT32 u4Width, u4Height;
    VENCOMX_SET_VALIDATE(VENCOMX_FRAME_RESOLUTION_T);

    USR_SPACE_ACCESS_VALIDATE_ARG(pvData, VENCOMX_FRAME_RESOLUTION_T);
    COPY_FROM_USER_ARG(pvData, tResolution, VENCOMX_FRAME_RESOLUTION_T);

    u4Width  = tResolution.u4Width;
    u4Height = tResolution.u4Height;
    prVEncOmxInfo->u4Width  = u4Width;
    prVEncOmxInfo->u4Height = u4Height;

    if (prVEncOmxInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
        rRTParm.u2Width  = u4Width;
        rRTParm.u2Height = u4Height;
        rRTParm.u2Pitch  = VENC_ALIGN_MASK(u4Width, 15);

        if (VENC_OK != VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm))
        {
            return VENCR_NOT_OK;
        }
    }

    return VENCR_OK;
}

static INT32 _VENCSetFramerate(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData, SIZE_T u4Size)
{
    UINT32 u4Fps = (UINT32)pvData;

    if (!u4Fps) // fps could not be 0
    {
        return VENCR_NOT_OK;
    }

    prVEncOmxInfo->u4Fps = u4Fps;

    if (prVEncOmxInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_FRAMERATE;
        rRTParm.u1Framerate = (UINT8)u4Fps;

        if (VENC_OK != VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm))
        {
            return VENCR_NOT_OK;
        }
    }

    return VENCR_OK;
}

static INT32 _VENCSetBitrate(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData, SIZE_T u4Size)
{
    UINT32 u4Bps = (UINT32)pvData;

    if (!u4Bps) // bitrate could not be 0
    {
        return VENCR_NOT_OK;
    }

    prVEncOmxInfo->u4Bps = u4Bps;

    if (prVEncOmxInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_BITRATE;
        rRTParm.u4Bitrate   = u4Bps;

        if (VENC_OK != VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm))
        {
            return VENCR_NOT_OK;
        }
    }

    return VENCR_OK;
}

static INT32 _VENCSetApType(VENCOMX_INFO_T *prVEncOmxInfo, const VOID* pvData, SIZE_T u4Size)
{
    switch ((VENCOMX_AP_TYPE_T)pvData)
    {
    default:
        return VENCR_NOT_OK;

    case VENCOMX_AP_TYPE_SKYPE:
        VENCOMX_DBG("VENC_AP_TYPE_SKYPE\n");
        prVEncOmxInfo->eProfileDef     = VENC_PROFILE_VPHONE;
        prVEncOmxInfo->ePixFmt         = VENC_PIX_FMT_NV12;
        #ifdef VDEC_VENC_PATH
        prVEncOmxInfo->ConnectInput    = _VENCConnectVdec;
        prVEncOmxInfo->DisconnectInput = _VENCDisconnectVdec;
        prVEncOmxInfo->SetInputParam   = NULL;
        #endif /*VDEC_VENC_PATH*/
        break;

    case VENCOMX_AP_TYPE_ATVPVR:
        VENCOMX_DBG("VENC_AP_TYPE_ATVPVR\n");
        prVEncOmxInfo->eProfileDef     = VENC_PROFILE_PVR;
        prVEncOmxInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prVEncOmxInfo->ConnectInput    = _VENCConnectDi;
        prVEncOmxInfo->DisconnectInput = _VENCDisconnectDi;
        prVEncOmxInfo->SetInputParam   = _VENCSetParamDi;
        break;

    case VENCOMX_AP_TYPE_MSI:
        VENCOMX_DBG("VENC_AP_TYPE_MSI\n");
        prVEncOmxInfo->eProfileDef     = VENC_PROFILE_MS;
        prVEncOmxInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prVEncOmxInfo->ConnectInput    = _VENCConnectDi;
        prVEncOmxInfo->DisconnectInput = _VENCDisconnectDi;
        prVEncOmxInfo->SetInputParam   = _VENCSetParamDi;
        break;
    }

    return VENCR_OK;
}

static INT32 _VENCSetInsertKeyframe(VENCOMX_INFO_T *prVEncOmxInfo)
{
    if (prVEncOmxInfo->fgIsRunning)
    {
        VENC_RT_CTRL_T rRTCtrl;
        rRTCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME;
        VENC_SetRtCtrlAsync(prVEncOmxInfo->hVEnc, &rRTCtrl);
        return VENCR_OK;
    }

    return VENCR_NOT_OK;
}

static INT32 _VENCConnect(
    VENCOMX_INFO_T *prVEncOmxInfo,
    UINT16           u2CompId)
{
    if (NULL == prVEncOmxInfo)
    {
        return VENCR_NOT_OK;
    }

    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        UINT32 u4Ret;

        VENCOMX_DBG("- [%d] has already been connected\n", u2CompId);

        //STOP VENC
        if (prVEncOmxInfo->fgIsRunning)
        {
            //disconnect  input source
            if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
            {
                VENCOMX_DBG("- disconnect input source failed!\n");
                return VENCR_NOT_OK;
            }

            u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                VENCOMX_DBG("- venc stop fail(%d)\n", u4Ret);
                return VENCR_NOT_OK;
            }

            prVEncOmxInfo->fgIsRunning = FALSE;
        }

        //CLOSE VENC
        u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
            return VENCR_NOT_OK;
        }
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

        if (prVEncOmxInfo->ptFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
            prVEncOmxInfo->ptFrmBuffMgr = NULL;
        }

        if (prVEncOmxInfo->ptResizeFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
            prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
        }
    }

    //init venc MW Info
    x_memset((VOID *)prVEncOmxInfo, 0, sizeof(VENCOMX_INFO_T));
    prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;
    prVEncOmxInfo->ucMxId = 0xFF;

    prVEncOmxInfo->u2SrcCompId = u2CompId;

    VENCOMX_DBG("- video encoder connect %d successfully!\n", u2CompId);

    return VENCR_OK;
}

static INT32 _VENCDisconnect(VENCOMX_INFO_T *prVEncOmxInfo)
{
    //deinit venc mw info
    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        UINT32 u4Ret;

        //STOP VENC
        if (prVEncOmxInfo->fgIsRunning)
        {
            //disconnect  input source
            if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
            {
                VENCOMX_DBG("- disconnect input source failed!\n");
                return VENCR_NOT_OK;
            }

            u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                VENCOMX_DBG("- venc stop fail(%d)\n", u4Ret);
                return VENCR_NOT_OK;
            }

            prVEncOmxInfo->fgIsRunning = FALSE;
        }

        //CLOSE VENC
        u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
            return VENCR_NOT_OK;
        }
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

        if (prVEncOmxInfo->ptFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
            prVEncOmxInfo->ptFrmBuffMgr = NULL;
        }

        if (prVEncOmxInfo->ptResizeFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
            prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
        }
    }

    VENCOMX_DBG("-video encoder disconnect successfully!\n");
    return VENCR_OK;
}

static BOOL fg1stCalled = TRUE;
VENCOMX_RESULT_T _VENCOMX_Init(void)
{
    UINT32 u2CompId;

    VENCOMX_DBG(" - void\n");

    if (!fg1stCalled)
    {
        return VENCR_OK;
    }

    fg1stCalled = FALSE;

    for (u2CompId = 0; u2CompId < VENCOMX_COMP_NS; u2CompId++)
    {
        VENCOMX_INFO_T *prVEncOmxInfo = _VENCGetInfoInst(u2CompId);
        x_memset((VOID *)prVEncOmxInfo, 0, sizeof(VENCOMX_INFO_T));
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;
        prVEncOmxInfo->ucMxId = 0xFF;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Termint(void)
{
    VENCOMX_DBG(" - void\n");
    //add driver implementation code here
    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Reset(unsigned long arg)
{
    UINT16 u2CompId;
    VENCOMX_INFO_T *prVEncOmxInfo;
    u2CompId = (UINT16)arg;
    VENCOMX_DBG(" - u2CompId = %d\n",(int)u2CompId);

    prVEncOmxInfo = _VENCGetInfoInst(u2CompId);

    //deinit venc mw info
    if (VENC_NULL_HANDLE != prVEncOmxInfo->hVEnc)
    {
        UINT32 u4Ret;

        //STOP VENC
        if (prVEncOmxInfo->fgIsRunning)
        {
            //disconnect  input source
            if (prVEncOmxInfo->DisconnectInput && (VENCR_OK != prVEncOmxInfo->DisconnectInput(prVEncOmxInfo)))
            {
                VENCOMX_DBG("- disconnect input source failed!\n");
                return VENCR_NOT_OK;
            }

            u4Ret = VENC_Stop(prVEncOmxInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                VENCOMX_DBG("- venc stop fail(%d)\n", u4Ret);
                return VENCR_NOT_OK;
            }

            prVEncOmxInfo->fgIsRunning = FALSE;
        }

        //CLOSE VENC
        u4Ret = VENC_Close(prVEncOmxInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("- venc close fail(%d)\n", u4Ret);
            return VENCR_NOT_OK;
        }
        prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

        if (prVEncOmxInfo->ptFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
            prVEncOmxInfo->ptFrmBuffMgr = NULL;
        }

        if (prVEncOmxInfo->ptResizeFrmBuffMgr)
        {
            VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncOmxInfo->ptResizeFrmBuffMgr));
            prVEncOmxInfo->ptResizeFrmBuffMgr = NULL;
        }
    }

    x_memset((VOID *)prVEncOmxInfo, 0, sizeof(VENCOMX_INFO_T));
    prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;
    prVEncOmxInfo->ucMxId = 0xFF;

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Set(unsigned long arg)
{
    VENCOMX_IOCTL_4ARG_T rArg;
    UINT16            u2CompId;
    VENCOMX_SET_TYPE_T eSetType;
    VOID*             pvData;
    UINT32            u4Size;
    VENCOMX_INFO_T*    prVEncOmxInfo;
    VENCOMX_RESULT_T       eRet = VENCR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId = (UINT16)rArg.ai4Arg[0];
    eSetType = (VENCOMX_SET_TYPE_T)rArg.ai4Arg[1];
    pvData = (VOID*)rArg.ai4Arg[2];
    u4Size = (UINT32)rArg.ai4Arg[3];

    VENCOMX_DBG(" - u2CompId = %d, eSetType = %d, pvData = %p\n",
        (int)u2CompId, (int)eSetType, pvData);

    //add driver implementation code here

    prVEncOmxInfo = _VENCGetInfoInst(u2CompId);
    if (NULL == prVEncOmxInfo)
    {
        return VENCR_NOT_OK;
    }

    switch (eSetType)
    {
    default:
        VENCOMX_DBG("Invalid set_type - %d\n", eSetType);
        return VENCR_NOT_OK;

    case VENCOMX_SET_TYPE_CONNECT_SRC:
        eRet = _VENCConnect(prVEncOmxInfo, (UINT32)pvData);
        break;

    case VENCOMX_SET_TYPE_DISCONNECT_SRC:
        eRet = _VENCDisconnect(prVEncOmxInfo);
        break;

    case VENCOMX_SET_TYPE_CTRL:
        eRet = _VENCSetCtrl(prVEncOmxInfo, pvData, u4Size);
        break;

    case VENCOMX_SET_TYPE_RESTART:
        eRet = _VENCSetRestart(prVEncOmxInfo);
        break;

    case VENCOMX_SET_TYPE_APP_TYPE:
        eRet = _VENCSetApType(prVEncOmxInfo, pvData, u4Size);
        break;

    case VENCOMX_SET_TYPE_SETTINGS:
        eRet = _VENCSetSettings(prVEncOmxInfo, pvData, u4Size);
        break;

    //maybe used in runtime
    case VENCOMX_SET_TYPE_FRAME_RESOLUTION:
        eRet = _VENCSetFrameResolution(prVEncOmxInfo, pvData, u4Size);
        break;

    case VENCOMX_SET_TYPE_FRAME_RATE:
        eRet = _VENCSetFramerate(prVEncOmxInfo, pvData, u4Size);
        break;

    case VENCOMX_SET_TYPE_BITRATE:
        eRet = _VENCSetBitrate(prVEncOmxInfo, pvData, u4Size);
        break;

    case VENCOMX_SET_TYPE_INSERT_KEYFRAME:
        eRet = _VENCSetInsertKeyframe(prVEncOmxInfo);
        break;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Get(unsigned long arg)
{
    VENCOMX_IOCTL_4ARG_T rArg;
    UINT16            u2CompId;
    VENCOMX_GET_TYPE_T eGetType;
    VOID*             pvData;
    UINT32*           pu4Size;
    VENCOMX_INFO_T*    prVEncOmxInfo;
    VENCOMX_RESULT_T       eRet = VENCR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId = (UINT16)rArg.ai4Arg[0];
    eGetType = (VENCOMX_GET_TYPE_T)rArg.ai4Arg[1];
    pvData = (VOID*)rArg.ai4Arg[2];
    pu4Size = (UINT32 *)rArg.ai4Arg[3];

    VENCOMX_DBG(" - u2CompId = %d, eGetType = %d\n",
        (int)u2CompId, (int)eGetType);

    //add driver implementation code here
    prVEncOmxInfo = _VENCGetInfoInst(u2CompId);
    if (NULL == prVEncOmxInfo)
    {
        return VENCR_NOT_OK;
    }

    switch (eGetType)
    {
    default:
        VENCOMX_DBG("Invalid get_type - %d\n", eGetType);
        return VENCR_NOT_OK;

    case VENCOMX_GET_TYPE_CTRL:
        {
            BOOL fgCtrl = prVEncOmxInfo->fgIsRunning;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,fgCtrl);
            COPY_TO_USER_ARG(pvData,fgCtrl,fgCtrl);
            eRet = VENCR_OK;
        }
        break;
    }

    return eRet;
}


VENCOMX_RESULT_T _VENCOMX_Open(unsigned long arg)
{
    VENCOMX_IOCTL_4ARG_T rArg;
    VENCOMX_RESULT_T       eRet = VENCR_NOT_OK;
    VENCOMX_HANDLE*    phVenc;
    VENCOMX_FMT_TYPE_T eEncFmt;
    VENCOMX_AP_TYPE_T  eProfile;
    BOOL              fgInterlace;
    VENC_HANDLE hEnc = VENC_NULL_HANDLE;
    UINT32 u4Type;
    VENC_PROFILE_T eProfileSet = VENC_PROFILE_MS;
    VENCOMX_INFO_T *prVEncOmxInfo;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    phVenc = (VENCOMX_HANDLE*)rArg.ai4Arg[0];
    eEncFmt = (VENCOMX_FMT_TYPE_T)rArg.ai4Arg[1];
    eProfile = (VENCOMX_AP_TYPE_T)rArg.ai4Arg[2];
    fgInterlace = (BOOL)rArg.ai4Arg[3];
    prVEncOmxInfo = _VENCGetInfoInst(0);

    VENCOMX_DBG(" - phVenc = %d, eEncFmt = %d, eProfile = %d, fgInterlace = %d\n",
        (int)phVenc, (int)eEncFmt, (int)eProfile, (int)fgInterlace);

    //add driver implementation code here
    switch (eEncFmt)
    {
    default:
        VENCOMX_DBG("Unknown eEncFmt(%d)\n", eEncFmt);
        return VENCR_NOT_OK;

    case VENCOMX_FMT_H264:
        u4Type = VENC_H264;
        break;
    }

    switch (eProfile)
    {
    default:
        VENCOMX_DBG("Unknown eProfile(%d)\n", eProfile);
        return VENCR_NOT_OK;

    case VENCOMX_AP_TYPE_SKYPE:
        eProfileSet = VENC_PROFILE_VPHONE;
        break;

    case VENCOMX_AP_TYPE_ATVPVR:
        eProfileSet = VENC_PROFILE_PVR;
        break;

    case VENCOMX_AP_TYPE_MSI:
        eProfileSet = VENC_PROFILE_MS;
        break;

    case VENCOMX_AP_TYPE_QQ:
        eProfileSet = VENC_PROFILE_VPHONE;
        break;
    }

    _VENCOMX_Reset(0);
    hEnc = VENC_Open(u4Type, eProfileSet, fgInterlace);
    prVEncOmxInfo->hVEnc = hEnc;

    rArg.ai4Arg[0] = (INT32)hEnc;

    if (VENC_NULL_HANDLE != hEnc)
    {
        eRet = VENCR_OK;
        USR_SPACE_ACCESS_VALIDATE_ARG(phVenc,VENC_HANDLE);
        COPY_TO_USER_ARG(phVenc,hEnc,VENC_HANDLE);
    }

    return eRet;
}


VENCOMX_RESULT_T _VENCOMX_SetParam(unsigned long arg)
{
    VENCOMX_IOCTL_3ARG_T rArg;
    VENCOMX_RESULT_T       eRet = VENCR_OK;
    VENC_HANDLE hEnc;
    VENCOMX_PRAM_T eParam;
    VOID *pvArg;
    UINT32 u4Ret;
    VENCOMX_INFO_T *prVEncOmxInfo;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    eParam = (VENCOMX_PRAM_T)rArg.ai4Arg[1];
    pvArg = (VOID*)rArg.ai4Arg[2];

    VENCOMX_DBG(" - eParam = %d\n", (int)eParam);
    prVEncOmxInfo = _VENCGetInfoInst(0);

    switch (eParam)
    {
    default:
        VENCOMX_DBG("Unknown eParam(%d)\n", eParam);
        return VENCR_NOT_OK;

    case VENCOMX_PRAM_FRAMERATE:
        if (!prVEncOmxInfo->fgIsRunning)
        {
            UINT32 u4Fps = (UINT32)pvArg;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_FRAMERATE, &u4Fps);
            prVEncOmxInfo->u4Fps = u4Fps;
        }
        else
        {
            VENC_RT_PARM_T rRTParm;
            rRTParm.u4Flag      = VENC_RT_PARM_FLAG_FRAMERATE;
            rRTParm.u1Framerate = (UINT8)(UINT32)pvArg;

            u4Ret = (VENC_SMODE_SYNC == prVEncOmxInfo->u4SyncMode)
                ? VENC_SetRtParamSync(prVEncOmxInfo->hVEnc, &rRTParm)
                : VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm);
        }
        break;

    case VENCOMX_PRAM_BITRATE:
        if (!prVEncOmxInfo->fgIsRunning)
        {
            UINT32 u4Bps = (UINT32)pvArg;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_BITRATE, &u4Bps);
            prVEncOmxInfo->u4Bps = u4Bps;
        }
        else
        {
            VENC_RT_PARM_T rRTParm;
            rRTParm.u4Flag      = VENC_RT_PARM_FLAG_BITRATE;
            rRTParm.u4Bitrate   = (UINT32)pvArg;

            u4Ret = (VENC_SMODE_SYNC == prVEncOmxInfo->u4SyncMode)
                ? VENC_SetRtParamSync(prVEncOmxInfo->hVEnc, &rRTParm)
                : VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm);
        }
        break;

    case VENCOMX_PRAM_RESOLUTION:
        if (!prVEncOmxInfo->fgIsRunning)
        {
            VENCOMX_FRAME_RESOLUTION_T tResUsr;
            VENC_PRAM_RES_T res;

            x_memset(&tResUsr, 0, sizeof(tResUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,VENCOMX_FRAME_RESOLUTION_T);
            COPY_FROM_USER_ARG(pvArg,tResUsr,VENCOMX_FRAME_RESOLUTION_T);

            res.u2Height = (UINT16)tResUsr.u4Height;
            res.u2Width = (UINT16)tResUsr.u4Width;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_RESOLUTION, &res);
            prVEncOmxInfo->u4Width = tResUsr.u4Width;
            prVEncOmxInfo->u4Height = tResUsr.u4Height;
        }
        else
        {
            VENCOMX_FRAME_RESOLUTION_T tResUsr;
            VENC_RT_PARM_T rRTParm;

            x_memset(&tResUsr, 0, sizeof(tResUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,VENCOMX_FRAME_RESOLUTION_T);
            COPY_FROM_USER_ARG(pvArg,tResUsr,VENCOMX_FRAME_RESOLUTION_T);

            //RESOLUTION
            rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
            rRTParm.u2Width  = tResUsr.u4Width;
            rRTParm.u2Height = tResUsr.u4Height;
            rRTParm.u2Pitch  = VENC_ALIGN_MASK(tResUsr.u4Width, 15);

            u4Ret = (VENC_SMODE_SYNC == prVEncOmxInfo->u4SyncMode)
                ? VENC_SetRtParamSync(prVEncOmxInfo->hVEnc, &rRTParm)
                : VENC_SetRtParamAsync(prVEncOmxInfo->hVEnc, &rRTParm);
        }
        break;
    }

    if (VENC_OK != u4Ret)
    {
        eRet = VENCR_NOT_OK;
        VENCOMX_DBG(" - fail(%d)!!\n", u4Ret);
    }

    return eRet;
}


// map from VENC_SMODE_T to VENC_SMODE_T
static VENC_SMODE_T _VENC_SyncModeMap(VENCOMX_SMODE_T eSmode)
{
    VENC_SMODE_T eType;

    switch (eSmode)
    {
    default:
        VENCOMX_DBG("Unknown eSmode(%d)\n", eSmode);
        eType = VENC_SMODE_DUAL;
        break;
    case VENCOMX_SMODE_SYNC:
        eType = VENC_SMODE_SYNC;
        break;
    case VENCOMX_SMODE_ASYNC:
        eType = VENC_SMODE_ASYNC;
        break;
    case VENCOMX_SMODE_DUAL:
        eType = VENC_SMODE_DUAL;
        break;
    case VENCOMX_SMODE_MAX:
        eType = VENC_SMODE_MAX;
        break;
    }

    return eType;
}


// map from VENC_PIX_FMT_T to VENC_PIX_FMT_T
static VENC_PIX_FMT_T _VENC_PixFmtMap(VENCOMX_PIX_FMT_T ePixFmt)
{
    VENC_PIX_FMT_T eType = VENC_PIX_FMT_MAX;

    switch (ePixFmt)
    {
    default:
        VENCOMX_DBG("Unknown ePixFmt(%d)\n", ePixFmt);
        eType = VENC_PIX_FMT_NV12;
        break;
    case VENCOMX_PIX_FMT_NV12:
        eType = VENC_PIX_FMT_NV12;
        break;
    }

    return eType;
}


static VOID _Venc_GetThumbnailCb(VOID *pvTag, UINT32 u4PhyAddr)
{
    VENCOMX_INFO_T *prVEncOmxInfo = (VENCOMX_INFO_T *)pvTag;
    VENCOMX_CB_T tVencCb;

    if (!prVEncOmxInfo)
    {
        ASSERT(prVEncOmxInfo);
        VENCOMX_DBG("prVEncOmxInfo is NULL!!!\n");
        return;
    }

    tVencCb.eCond = VENCOMX_COND_THUMBNAIL;
    tVencCb.u.u4ThumbnailPhyAddr = u4PhyAddr;
    _CB_PutEvent(CB_MTAL_VENC_DATA_NFY, sizeof(tVencCb), &tVencCb);
}

VENCOMX_RESULT_T _VENCOMX_SetCtrl(unsigned long arg)
{
    VENCOMX_IOCTL_3ARG_T rArg;
    VENCOMX_RESULT_T       eRet = VENCR_OK;
    VENC_HANDLE hEnc;
    VENCOMX_CTRL_T eCtrl;
    VOID *pvArg;
    UINT32 u4Ret;
    VENCOMX_INFO_T *prVEncOmxInfo;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    eCtrl = (VENCOMX_CTRL_T)rArg.ai4Arg[1];
    pvArg = (VOID*)rArg.ai4Arg[2];

    VENCOMX_DBG(" - eCtrl = %d\n", (int)eCtrl);
    prVEncOmxInfo = _VENCGetInfoInst(0);

    switch (eCtrl)
    {
    default:
        VENCOMX_DBG("Unknown eCtrl(%d)\n", eCtrl);
        return VENCR_NOT_OK;

    case VENCOMX_CTRL_SYNC_MODE:
        {
            UINT32 u4Value = (UINT32)_VENC_SyncModeMap((VENCOMX_SMODE_T)pvArg);
            prVEncOmxInfo->u4SyncMode = u4Value;
            u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_SYNC_MODE, &u4Value);
        }
        break;

    case VENCOMX_CTRL_PIX_FMT:
        {
            VENCOMX_CTRL_PIXFMT_T tPixUsr;
            VENC_CTRL_PIXFMT_T pix;

            x_memset(&tPixUsr, 0, sizeof(tPixUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,VENCOMX_CTRL_PIXFMT_T);
            COPY_FROM_USER_ARG(pvArg,tPixUsr,VENCOMX_CTRL_PIXFMT_T);

            pix.ePixFmt = _VENC_PixFmtMap(tPixUsr.ePixFmt);
            pix.u2Pitch = (UINT16)tPixUsr.u4Pitch;

            prVEncOmxInfo->ePixFmt = pix.ePixFmt;
            prVEncOmxInfo->u4Pitch = pix.u2Pitch;

            u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_PIX_FMT, &pix);
        }
        break;

    case VENCOMX_CTRL_GET_THUMBNAIL:
        {
            VENCOMX_CTRL_THUMBNAIL_T rThumbnailUsr;
            VENC_CTRL_THUMBNAIL_T rThumbnail;

            x_memset(&rThumbnailUsr, 0, sizeof(rThumbnailUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,VENCOMX_CTRL_PIXFMT_T);
            COPY_FROM_USER_ARG(pvArg,rThumbnailUsr,VENCOMX_CTRL_PIXFMT_T);

            rThumbnail.u4Width = rThumbnailUsr.u4Width;
            rThumbnail.u4Height = rThumbnailUsr.u4Height;
            rThumbnail.pvTag = prVEncOmxInfo;
            rThumbnail.cb = _Venc_GetThumbnailCb;

            u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_GET_THUMBNAIL, &rThumbnail);
        }
        break;
    }

    if (VENC_OK != u4Ret)
    {
        eRet = VENCR_NOT_OK;
        VENCOMX_DBG(" - fail(%d)!!\n", u4Ret);
    }

    return eRet;
}


VENCOMX_RESULT_T _VENCOMX_DrvInit(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    UINT32 u4Ret;

    VENCOMX_DBG(" - hEnc = %d\n",(int)hEnc);

    u4Ret = VENC_Init(hEnc);

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_Init fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Run(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    VENCOMX_INFO_T *prVEncOmxInfo = _VENCGetInfoInst(0);
    UINT32 u4Ret;

    VENCOMX_DBG(" - hEnc = %d\n",(int)hEnc);

    u4Ret = VENC_Run(hEnc);

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_Run fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    prVEncOmxInfo->fgIsRunning = TRUE;

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Stop(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    VENCOMX_INFO_T *prVEncOmxInfo = _VENCGetInfoInst(0);
    UINT32 u4Ret;

    VENCOMX_DBG(" - hEnc = %d\n",(int)hEnc);

    if (prVEncOmxInfo->fgIsRunning)
    {
        u4Ret = VENC_Stop(hEnc);

        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("VENC_Stop fail(%d)!!\n", u4Ret);
            return VENCR_NOT_OK;
        }

        prVEncOmxInfo->fgIsRunning = FALSE;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_Close(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    VENCOMX_INFO_T *prVEncOmxInfo = _VENCGetInfoInst(0);
    UINT32 u4Ret;

    VENCOMX_DBG(" - hEnc = %d\n",(int)hEnc);

    if (prVEncOmxInfo->fgIsRunning)
    {
        u4Ret = VENC_Stop(hEnc);

        if (VENC_OK != u4Ret)
        {
            VENCOMX_DBG("VENC_Stop fail(%d)!!\n", u4Ret);
            return VENCR_NOT_OK;
        }

        prVEncOmxInfo->fgIsRunning = FALSE;
    }

    u4Ret = VENC_Close(hEnc);

    prVEncOmxInfo->hVEnc = VENC_NULL_HANDLE;

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_Close fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_EncSeqHdrSync(unsigned long arg)
{
    VENCOMX_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Ret;
    VENCOMX_OUT_PIC_T *ptOutUsr, tOutKrnl;
    VENC_OUT_PIC_T tOut;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptOutUsr = (VENCOMX_OUT_PIC_T *)rArg.ai4Arg[1];

    x_memset(&tOutKrnl, 0, sizeof(tOutKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptOutUsr,VENCOMX_OUT_PIC_T);
    COPY_FROM_USER_ARG(ptOutUsr,tOutKrnl,VENCOMX_OUT_PIC_T);
    x_memset(&tOut, 0, sizeof(tOut));
    tOut.u4Pts    = tOutKrnl.u4Pts;
    tOut.u4Addr   = tOutKrnl.u4Addr;
    tOut.u4Size   = tOutKrnl.u4Size;
    tOut.u4RStart = tOutKrnl.u4RStart;
    tOut.u4REnd   = tOutKrnl.u4REnd;

    u4Ret = VENC_EncSeqHdrSync(hEnc, &tOut);

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_EncSeqHdrSync fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    tOutKrnl.u4Pts    = tOut.u4Pts;
    tOutKrnl.u4Addr   = tOut.u4Addr;
    tOutKrnl.u4Size   = tOut.u4Size;
    tOutKrnl.u4RStart = tOut.u4RStart;
    tOutKrnl.u4REnd   = tOut.u4REnd;
    USR_SPACE_ACCESS_VALIDATE_ARG(ptOutUsr,VENCOMX_OUT_PIC_T);
    COPY_TO_USER_ARG(ptOutUsr,tOutKrnl,VENCOMX_OUT_PIC_T);

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_EncFrameSync(unsigned long arg)
{
    VENCOMX_IOCTL_3ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Ret;
    VENCOMX_IN_PIC_T *ptInUsr, tInKrnl;
    VENCOMX_OUT_PIC_T *ptOutUsr, tOutKrnl;
    VENC_IN_PIC_T tIn;
    VENC_OUT_PIC_T tOut;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptInUsr = (VENCOMX_IN_PIC_T *)rArg.ai4Arg[1];
    ptOutUsr = (VENCOMX_OUT_PIC_T *)rArg.ai4Arg[2];

    x_memset(&tInKrnl, 0, sizeof(tInKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptInUsr,VENCOMX_IN_PIC_T);
    COPY_FROM_USER_ARG(ptInUsr,tInKrnl,VENCOMX_IN_PIC_T);
    x_memset(&tIn, 0, sizeof(tIn));
    tIn.u4YAddr = tInKrnl.u4YAddr;
    tIn.u4CAddr = tInKrnl.u4CAddr;
    tIn.u4Pts   = tInKrnl.u4Pts;

    x_memset(&tOutKrnl, 0, sizeof(tOutKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptOutUsr,VENCOMX_OUT_PIC_T);
    COPY_FROM_USER_ARG(ptOutUsr,tOutKrnl,VENCOMX_OUT_PIC_T);
    x_memset(&tOut, 0, sizeof(tOut));
    tOut.u4Pts    = tOutKrnl.u4Pts;
    tOut.u4Addr   = tOutKrnl.u4Addr;
    tOut.u4Size   = tOutKrnl.u4Size;
    tOut.u4RStart = tOutKrnl.u4RStart;
    tOut.u4REnd   = tOutKrnl.u4REnd;

    u4Ret = VENC_EncFrameSync(hEnc, &tIn, &tOut);

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_EncFrameSync fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    tOutKrnl.u4Pts    = tOut.u4Pts;
    tOutKrnl.u4Addr   = tOut.u4Addr;
    tOutKrnl.u4Size   = tOut.u4Size;
    tOutKrnl.u4RStart = tOut.u4RStart;
    tOutKrnl.u4REnd   = tOut.u4REnd;
    USR_SPACE_ACCESS_VALIDATE_ARG(ptOutUsr,VENCOMX_OUT_PIC_T);
    COPY_TO_USER_ARG(ptOutUsr,tOutKrnl,VENCOMX_OUT_PIC_T);

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_EncFrameAsync(unsigned long arg)
{
    VENCOMX_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Ret;
    VENCOMX_IN_PIC_T *ptInUsr, tInKrnl;
    VENC_IN_PIC_T tIn;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptInUsr = (VENCOMX_IN_PIC_T *)rArg.ai4Arg[1];

    x_memset(&tInKrnl, 0, sizeof(tInKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptInUsr,VENCOMX_IN_PIC_T);
    COPY_FROM_USER_ARG(ptInUsr,tInKrnl,VENCOMX_IN_PIC_T);
    x_memset(&tIn, 0, sizeof(tIn));
    tIn.u4YAddr = tInKrnl.u4YAddr;
    tIn.u4CAddr = tInKrnl.u4CAddr;
    tIn.u4Pts   = tInKrnl.u4Pts;

    u4Ret = VENC_EncFrameAsync(hEnc, &tIn, NULL);

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_EncFrameAsync fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_CreateBuffMgr(unsigned long arg)
{
    VENCOMX_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    VENCOMX_HANDLE *ptFrmBuffMgrUsr;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr, u4Ret, u4Size;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;
    VENCOMX_INFO_T *prVEncOmxInfo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptFrmBuffMgrUsr = (VENCOMX_HANDLE *)rArg.ai4Arg[1];

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);
    prVEncOmxInfo = _VENCGetInfoInst(0);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prVEncOmxInfo->ptFrmBuffMgr);
    prVEncOmxInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    if (NULL == prVEncOmxInfo->ptFrmBuffMgr)
    {
        ASSERT(prVEncOmxInfo->ptFrmBuffMgr);
        return VENCR_NOT_OK;
    }

    _BUFMGR_OBJ_Reset(prVEncOmxInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(prVEncOmxInfo->u4Width, 15) * VENC_ALIGN_MASK(prVEncOmxInfo->u4Height, 31);

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 7; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prVEncOmxInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("- Set buffer error(%d)!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(ptFrmBuffMgrUsr,VENCOMX_HANDLE);
    COPY_TO_USER_ARG(ptFrmBuffMgrUsr,prVEncOmxInfo->ptFrmBuffMgr,VENCOMX_HANDLE);

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_DeleteBuffMgr(unsigned long arg)
{
    VENCOMX_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    VENC_BUFMGR_BASE_T *ptFrmBuffMgr;
    VENCOMX_INFO_T *prVEncOmxInfo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)rArg.ai4Arg[1];

    prVEncOmxInfo = _VENCGetInfoInst(0);

    ASSERT(ptFrmBuffMgr == prVEncOmxInfo->ptFrmBuffMgr);
    if (prVEncOmxInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncOmxInfo->ptFrmBuffMgr));
        prVEncOmxInfo->ptFrmBuffMgr = NULL;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_BUFMGR_OBJ_Get(unsigned long arg)
{
    VENCOMX_IOCTL_3ARG_T rArg;
    VENC_BUFMGR_BASE_T *ptFrmBuffMgr;
    VENCOMX_IN_PIC_T *ptDataUsr, tDataKrnl;
    UINT32 u4Option;
    VENCOMX_INFO_T *prVEncOmxInfo;
    VENC_FRM_T tFrm;
    UINT32 u4Ret = VENC_BUFMGR_FAIL;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)rArg.ai4Arg[0];
    ptDataUsr = (VENCOMX_IN_PIC_T *)rArg.ai4Arg[1];
    u4Option = (UINT32)rArg.ai4Arg[2];

    prVEncOmxInfo = _VENCGetInfoInst(0);

    ASSERT(ptFrmBuffMgr == prVEncOmxInfo->ptFrmBuffMgr);
    switch (u4Option)
    {
    default:
        VENCOMX_DBG("Unknown u4Option(%d)\n", u4Option);
        return VENCR_NOT_OK;
    case VENCOMX_BUFMGR_NOWAIT:
        u4Option = VENC_BUFMGR_NOWAIT;
        break;
    case VENCOMX_BUFMGR_WAIT:
        u4Option = VENC_BUFMGR_WAIT;
        break;
    }
    u4Ret = _BUFMGR_OBJ_Get(ptFrmBuffMgr, (VOID *)&tFrm, u4Option);
    if (VENC_BUFMGR_OK != u4Ret)
    {
        VENCOMX_DBG("_BUFMGR_OBJ_Get fail(%d)\n", u4Ret);
        return VENCR_NOT_OK;
    }

    tDataKrnl.u4YAddr = tFrm.u4YAddr;
    tDataKrnl.u4CAddr= tFrm.u4UVAddr;
    USR_SPACE_ACCESS_VALIDATE_ARG(ptDataUsr,VENCOMX_IN_PIC_T);
    COPY_TO_USER_ARG(ptDataUsr,tDataKrnl,VENCOMX_IN_PIC_T);

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_BUFMGR_OBJ_Free(unsigned long arg)
{
    VENCOMX_IOCTL_2ARG_T rArg;
    VENC_BUFMGR_BASE_T *ptFrmBuffMgr;
    VENCOMX_IN_PIC_T *ptDataUsr, tDataKrnl;
    VENCOMX_INFO_T *prVEncOmxInfo;
    VENC_FRM_T tFrm;
    UINT32 u4Ret = VENC_BUFMGR_FAIL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)rArg.ai4Arg[0];
    ptDataUsr = (VENCOMX_IN_PIC_T *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(ptDataUsr,VENCOMX_IN_PIC_T);
    COPY_FROM_USER_ARG(ptDataUsr,tDataKrnl,VENCOMX_IN_PIC_T);
    tFrm.u4YAddr = tDataKrnl.u4YAddr;
    tFrm.u4UVAddr = tDataKrnl.u4CAddr;

    prVEncOmxInfo = _VENCGetInfoInst(0);

    ASSERT(ptFrmBuffMgr == prVEncOmxInfo->ptFrmBuffMgr);
    u4Ret = _BUFMGR_OBJ_Free(ptFrmBuffMgr, (VOID *)&tFrm);
    if (VENC_BUFMGR_OK != u4Ret)
    {
        VENCOMX_DBG("_BUFMGR_OBJ_Free fail(%d)\n", u4Ret);
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_FlushFb(unsigned long arg)
{
    VENCOMX_IOCTL_3ARG_T rArg;
    UINT32 u4YAddr, u4CAddr, u4FrmSz;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    u4YAddr = (UINT32)rArg.ai4Arg[0];
    u4CAddr = (UINT32)rArg.ai4Arg[1];
    u4FrmSz = (UINT32)rArg.ai4Arg[2];

    HalFlushDCacheMultipleLine(VIRTUAL(u4YAddr), u4FrmSz);
    HalFlushDCacheMultipleLine(VIRTUAL(u4CAddr), u4FrmSz >> 1);

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_InsertKeyframe(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    VENC_RT_CTRL_T tCtrl;
    UINT32 u4Ret;
    VENCOMX_INFO_T *prVEncOmxInfo;

    VENCOMX_DBG(" - hEnc = %d\n",(int)hEnc);
    prVEncOmxInfo = _VENCGetInfoInst(0);

    tCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME;

    if (VENC_SMODE_SYNC == prVEncOmxInfo->u4SyncMode)
    {
        u4Ret = VENC_SetRtCtrlSync(hEnc, &tCtrl);
    }
    else
    {
        u4Ret = VENC_SetRtCtrlAsync(hEnc, &tCtrl);
    }

    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG("VENC_RT_CTRL_FLAG_INSERT_KEYFRAME fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_RegCb(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    UINT32 u4Ret;
    VENCOMX_INFO_T *prVEncOmxInfo;
    VENC_CTRL_OUTCB_T rCb;

    VENCOMX_DBG(" - hEnc = %d\n",(int)hEnc);
    prVEncOmxInfo = _VENCGetInfoInst(0);
    prVEncOmxInfo->fg2Usr = TRUE;

    //Output Callback
    rCb.pvData = (VOID *)prVEncOmxInfo;
    rCb.cb     = _VENCCb;
    u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        VENCOMX_DBG(" - fail(%d)!!\n", u4Ret);
        return VENCR_NOT_OK;
    }

    return VENCR_OK;
}


VENCOMX_RESULT_T _VENCOMX_UnlockBuf(unsigned long arg)
{
    VENCOMX_IOCTL_3ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Addr, u4Size, u4Ret;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc   = (VENC_HANDLE)rArg.ai4Arg[0];
    u4Addr = (UINT32)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];

    u4Ret = VENC_UnlockBuf(hEnc, u4Addr, u4Size);

    return (VENC_OK == u4Ret)? VENCR_OK : VENCR_NOT_OK;
}

VENCOMX_RESULT_T _VENCOMX_CopyData(unsigned long arg)
{
#ifdef COPYDATA_FROME_DRVIER
    VENCOMX_IOCTL_8ARG_T rArg;
    VENCOMX_INFO_T*    prVEncOmxInfo;
	UINT32 u4Size,u4UserAddr,u4PhyAddr,u4CopySize,u4VirAddr;
    UINT32 u4StartAddr,u4EndAddr;
     FBM_POOL_T *prFbmPool;
	INT32 i4Ret=0;
    INT8 uPort=0;
    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);
	uPort=rArg.ai4Arg[0];
	u4UserAddr=rArg.ai4Arg[1];
	u4PhyAddr=rArg.ai4Arg[2];
	u4Size=rArg.ai4Arg[3];
    u4StartAddr=rArg.ai4Arg[4];
    u4EndAddr=rArg.ai4Arg[5];

    if(u4StartAddr==0 || u4EndAddr==0)
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_VENC);
        u4StartAddr=prFbmPool->u4Addr;
        u4EndAddr=u4StartAddr+prFbmPool->u4Size;
    }
	prVEncOmxInfo = _VENCGetInfoInst(0);
    if((u4PhyAddr+u4Size)>u4EndAddr)
    {
       u4CopySize=u4EndAddr-u4PhyAddr;
       u4VirAddr=VIRTUAL(u4PhyAddr);
	   HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
       i4Ret=copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr,u4CopySize);
       if(i4Ret)
       {
          printk("%s copy_to_user error1\n", __FUNCTION__);
          return VENCR_ERR_INV;
       }
       
       u4UserAddr+=u4CopySize;
       u4CopySize=u4Size-u4CopySize;
       u4VirAddr=VIRTUAL(u4StartAddr);
	   HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
       i4Ret=copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr,u4CopySize);
       if(i4Ret)
       {
          printk("%s copy_to_user error2\n", __FUNCTION__);
          return VENCR_ERR_INV;
       }
    }
    else
    {
       u4CopySize=u4Size;
       u4VirAddr=VIRTUAL(u4PhyAddr);
	   HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
       i4Ret=copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr,u4CopySize);
       if(i4Ret)
       {
           printk("%s copy_to_user error3\n", __FUNCTION__);
           return VENCR_ERR_INV;
       }
    }
    
   return VENCR_OK;
#else
   return VENCR_NOT_OK;
#endif
}

#if defined(CC_SUPPORT_MUXER) && defined(CC_SUPPORT_VENC)
VOID VENCOMX_SetMxId(UINT16  u2Id, UCHAR ucMxId)
{
    VENCOMX_INFO_T *prVEncOmxInfo = _VENCGetInfoInst(u2Id);
    prVEncOmxInfo->ucMxId = ucMxId;
}
#endif

#endif // CC_SUPPORT_VENC




