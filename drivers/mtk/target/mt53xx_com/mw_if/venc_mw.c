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
 * $RCSfile: venc_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_mw.c
 *  This file defines the interfaces for middleware to access VENC driver
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------


#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "venc_if.h"
#include "venc_drvif.h"
#include "venc_util_drvif.h"
#include "vdec_drvif.h"
#include "drv_di.h"
#include "drv_video.h"

#include "drv_comp_id.h"
#include "drv_name.h"

#define DEFINE_IS_LOG    VENC_IsLog
#include "x_debug.h"
#include "x_assert.h"

#include "x_rm.h"
#ifndef LINUX_TURNKEY_SOLUTION
#include "x_rm_dev_types.h"
#else
#include "u_rm_dev_types.h"
#endif
#include "x_vid_enc.h"
#ifndef LINUX_TURNKEY_SOLUTION
#include "u_mm_common.h"
#endif

#ifdef LINUX_TURNKEY_SOLUTION
#include "cb_data_rm.h"
#include "cb_param.h"
#endif

#ifdef CC_SUPPORT_MUXER
#include "muxer_if.h"
#endif

#include "drvcust_if.h"
#include "u_drv_cust.h"
LINT_EXT_HEADER_END

#include "vdo_misc.h"
#include "drv_vga.h"
#include "video_def.h"
#include "drv_hdmi.h"
#include "vdo_if.h"
#include "srm_drvif.h"

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

VENC_BUFMGR_DECLARE(VENC_RZ_FRM, VENC_FRM_T, 10);

VENC_BUFMGR_DECLARE(VENC_FRM, VENC_FRM_T, 10);

typedef struct _VENC_MW_INFO_T
{
    BOOL                     fgIsCtrlOn;
    BOOL                     fgIsRunning;
    BOOL                     fgInterlace; // for replay
    UCHAR                    ucMxId;

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
    VID_SRC_TYPE_T           eSrcType;
    VID_ENC_FMT_TYPE_T       eEncFmt;

    VENC_HANDLE              hVEnc;
    VENC_FILTER_T*           ptFilter;
    VENC_BUFMGR_BASE_T*      ptFrmBuffMgr;
    VENC_BUFMGR_BASE_T*      ptResizeFrmBuffMgr;
    INT32 (*SetFmtInfo)      (struct _VENC_MW_INFO_T *, VOID *);
    INT32 (*ConnectInput)    (struct _VENC_MW_INFO_T *);
    INT32 (*DisconnectInput) (struct _VENC_MW_INFO_T *);
    INT32 (*SetInputParam)   (struct _VENC_MW_INFO_T *);
} VENC_MW_INFO_T;

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define _VENC_MAX_RES (1280 * 736)
#define _VENC_HALFFPS_THRD (1280 * 720 * 30)


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//lint --e{717} do ... while (0);
#define VENC_GET_VALIDATE(TYPE)          \
do                                       \
{                                        \
    if (NULL == prVEncMwInfo)            \
    {                                    \
        return RMR_DRV_GET_FAILED;       \
    }                                    \
                                         \
    if (NULL == pv_get_info)             \
    {                                    \
        return RMR_DRV_INV_GET_INFO;     \
    }                                    \
                                         \
    if (sizeof(TYPE) > *pz_get_info_len) \
    {                                    \
        *pz_get_info_len = sizeof(TYPE); \
        return RMR_DRV_NOT_ENOUGH_SPACE; \
    }                                    \
    *pz_get_info_len = sizeof(TYPE);     \
} while (0)


#define VENC_SET_VALIDATE(TYPE)          \
do                                       \
{                                        \
    if (NULL == pv_set_info)             \
    {                                    \
        return RMR_DRV_INV_SET_INFO;     \
    }                                    \
                                         \
    if (sizeof(TYPE) > z_set_info_len)   \
    {                                    \
        return RMR_DRV_INV_SET_INFO;     \
    }                                    \
} while (0)


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

extern UCHAR _CompIdtoEsId(UINT16 u2CompId);
extern void vDrvDISetVEncHandle(void *handle, void *ptFrmBuffMgr, void *ptResizeFrmBuffMgr, void *ptFilterPts);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static VENC_MW_INFO_T _arVEncMwInfo[VENC_COMP_NS];
static HANDLE_T _hVEncMwApiMutex = 0; // Thread safe protection


//-----------------------------------------------------------------------------
// Static functions declare
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


static VENC_MW_INFO_T *_VEncMwGetInfoInst(UINT16 u2CompId)
{
    ASSERT((UINT16)VENC_COMP_NS > u2CompId);
    return ((UINT16)VENC_COMP_NS > u2CompId)? &_arVEncMwInfo[u2CompId] : &_arVEncMwInfo[0];
}


static BOOL MTVENC_LockApiMutex(VOID)
{
    INT32 i4Ret = x_sema_lock(_hVEncMwApiMutex, X_SEMA_OPTION_WAIT);
    ASSERT(i4Ret == OSR_OK);
    return (i4Ret == OSR_OK)? TRUE : FALSE;
}


static BOOL MTVENC_UnLockApiMutex(VOID)
{
    INT32 i4Ret = x_sema_unlock(_hVEncMwApiMutex);
    ASSERT(i4Ret == OSR_OK);
    return (i4Ret == OSR_OK)? TRUE : FALSE;
}


static VOID _VEncMwResetInfoInst(VENC_MW_INFO_T *prVEncMwInfo)
{
    if (prVEncMwInfo)
    {
        x_memset((VOID *)prVEncMwInfo, 0, sizeof(VENC_MW_INFO_T));
        prVEncMwInfo->hVEnc = VENC_NULL_HANDLE;
        prVEncMwInfo->ucMxId = 0xFF;
        prVEncMwInfo->eProfileDef = VENC_PROFILE_MAX;
    }
}


static BOOL _VEncMwDestroy(VENC_MW_INFO_T *prVEncMwInfo, BOOL fgDiscInput)
{
    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        UINT32 u4Ret;

        //STOP VENC
        if (prVEncMwInfo->fgIsRunning)
        {
            if (fgDiscInput)
            {
                //disconnect  input source
                if (prVEncMwInfo->DisconnectInput && (RMR_OK != prVEncMwInfo->DisconnectInput(prVEncMwInfo)))
                {
                    LOG(1, "%s: disconnect input source failed!\n", __FUNCTION__);
                    return FALSE;
                }
            }

            u4Ret = VENC_Stop(prVEncMwInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                LOG(1, "%s: venc stop fail(%d)\n", __FUNCTION__, u4Ret);
                return FALSE;
            }

            prVEncMwInfo->fgIsRunning = FALSE;
        }

        //CLOSE VENC
        u4Ret = VENC_Close(prVEncMwInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            LOG(1, "%s: venc close fail(%d)\n", __FUNCTION__, u4Ret);
            return FALSE;
        }
        prVEncMwInfo->hVEnc = VENC_NULL_HANDLE;
    }

    if (prVEncMwInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncMwInfo->ptFrmBuffMgr));
        prVEncMwInfo->ptFrmBuffMgr = NULL;
    }

    if (prVEncMwInfo->ptResizeFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncMwInfo->ptResizeFrmBuffMgr));
        prVEncMwInfo->ptResizeFrmBuffMgr = NULL;
    }

    return TRUE;
}


static VOID _VEncMwCb(VOID *pvArg, VENC_IN_PIC_T * ptIn, VENC_OUT_PIC_T * ptOut, UINT32 type)
{
    VENC_MW_INFO_T *prVEncMwInfo = (VENC_MW_INFO_T *)pvArg;
    if (ptIn)
    {
        VENC_FRM_T rFrm;
        rFrm.u4YAddr  = ptIn->u4YAddr;
        rFrm.u4UVAddr = ptIn->u4CAddr;
        _BUFMGR_OBJ_Free(prVEncMwInfo->ptFrmBuffMgr, (VOID *)&rFrm);
        //Printf("_emu_demo_cb free: src(Y,UV) => (0x%08x, 0x%08x)\n", ptIn->u4YAddr, ptIn->u4CAddr);
    }

#ifdef CC_SUPPORT_MUXER
    if (ptOut && (0xFF != prVEncMwInfo->ucMxId))
    {
        VENC_PES_INFO_T rVEncPes;
        x_memset(&rVEncPes, 0, sizeof(rVEncPes));
        rVEncPes.u4FifoStart      = ptOut->u4RStart;
        rVEncPes.u4FifoEnd        = ptOut->u4REnd;
        rVEncPes.u4StartAddr      = ptOut->u4Addr;
        rVEncPes.u4FrameSize      = ptOut->u4Size;
        rVEncPes.u4VideoType = type;
        rVEncPes.u4Pts            = ptOut->u4Pts;
        rVEncPes.pvTag       = prVEncMwInfo->hVEnc;
        MUXER_SendVideoPes(prVEncMwInfo->ucMxId, &rVEncPes);
    }
#endif

    UNUSED(ptOut);
    UNUSED(type);
}


//--input source settings
//#define TMP_DRIVER_SET_SUB
/// need to remove setting sub source at driver, disable this define to do this




#ifdef TMP_DRIVER_SET_SUB
static INT32 _VEncMwSetSubSrc(UINT8 u1OnOff)
{
    VDP_REGION_T rRegion;
    VDP_OVERSCAN_REGION_T rOvRegion;
    UINT32 u4WaitCnt = 0;
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
        Bypass_HalPIPClk(0);
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
        u1ForceHPSDByCli = 1;
        vScpipSetForcedHpsFactor(VDP_2, 0x8000);

        do{
            x_thread_delay(100);
            if (++u4WaitCnt > 40)
            {
                 LOG(0, "Wait for Mode Chg too long (Chg Sub Src to Main) \n");
                 return RMR_DRV_SET_FAILED;
            }
        }while( bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
                bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE ||
                _u4DrvVideoGetMute(VDP_1) == TRUE);
    }
    else
    {
        SRM_SetTvMode(rOldSrmMode);
        vScpipSetForcedHpsFactor(VDP_2, 0);
        u1ForceHPSDByCli = 0;
        VDP_SetDisplayOff(VDP_2, FALSE);
        bApiVideoSetVideoSrc(SV_VP_PIP, u1OldSubSrc);
        VDP_SetEnable(VDP_2, ucEnable);
        if (!ucEnable)
        {
            rRegion.u4Width = 0;
            rRegion.u4Height = 0;
            VDP_SetOutRegion(VDP_2, 0, rRegion);
        }

        do{
            x_thread_delay(100);
            if (++u4WaitCnt > 40)
            {
                 LOG(0, "Wait for Mode Chg too long (Chg Sub Src back) \n");
                 return RMR_DRV_SET_FAILED;
            }
        }while(bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
               _u4DrvVideoGetMute(VDP_1) == TRUE);
    }

    return RMR_OK;
}

extern UINT8 u1HdmiColorMode;
static INT32 _VEncMwSetYCbCr(UINT8 u1OnOff)
{
    static UINT8 fgIsHDMIChg = 0, u1OldHdmiMode = SV_HDMI_MODE_AUTO;
    static UINT8 fgIsVGAChg = 0;
    UINT32 u4WaitCnt = 0;

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
                 LOG(0, "Wait for Mode Chg too long (422 to 444) \n");
                 return RMR_DRV_SET_FAILED;
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
                 LOG(0, "Wait for Mode Chg too long (444 to 422) \n");
                 return RMR_DRV_SET_FAILED;
            }
        }while( bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE ||
                _u4DrvVideoGetMute(VDP_1) == TRUE);
    }

    return RMR_OK;
}

#endif



static INT32 _VEncMwConnectDi(VENC_MW_INFO_T *prVEncMwInfo)
{
    vDrvDISupportH264Init();

    vDrvDISetVEncHandle(
        (void *)prVEncMwInfo->hVEnc,
        (void *)prVEncMwInfo->ptFrmBuffMgr,
        (void *)prVEncMwInfo->ptResizeFrmBuffMgr,
        (void *)prVEncMwInfo->ptFilter);


#ifdef CC_ATV_PVR_SUPPORT
    // trigger VBI
    //VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, TRUE);
#endif

    return RMR_OK;
}


static INT32 _VEncMwDisconnectDi(VENC_MW_INFO_T *prVEncMwInfo)
{
    INT32 i4Ret = RMR_OK;

#ifdef CC_ATV_PVR_SUPPORT
    // stop VBI
    //VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, FALSE);
#endif

    // stop DI write mode
    vDrvDIStopSendBufToVENC(TRUE);

#ifdef TMP_DRIVER_SET_SUB
    i4Ret = _VEncMwSetSubSrc(FALSE);
    if (i4Ret != RMR_OK)
    {
        return i4Ret;
    }
    i4Ret = _VEncMwSetYCbCr(FALSE);
    if (i4Ret != RMR_OK)
    {
        return i4Ret;
    }
#endif
    if (prVEncMwInfo->eProfileDef == VENC_PROFILE_MS)
    {
        bSubSrcCloneMode(SV_OFF);
    }
    return i4Ret;
}


static INT32 _VEncMwSetParamDi(VENC_MW_INFO_T *prVEncMwInfo)
{
#ifdef TMP_DRIVER_SET_SUB
    INT32 i4Ret;

    i4Ret = _VEncMwSetSubSrc(TRUE);
    if (i4Ret != RMR_OK)
    {
        return i4Ret;
    }

    i4Ret = _VEncMwSetYCbCr(TRUE);
    if (i4Ret != RMR_OK)
    {
        return i4Ret;
    }
#endif

    if (prVEncMwInfo->eProfileDef == VENC_PROFILE_MS)
    {
        bSubSrcCloneMode(SV_ON);
    }

    if (bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE)
    {
        LOG(3," Signal Not Stable Cannot Continue\n");
        return RMR_INV_CONNECT;
    }

    
    return RMR_OK;
}


#ifdef VDEC_VENC_PATH
static INT32 _VEncMwConnectVdec(VENC_MW_INFO_T *prVEncMwInfo)
{
    UCHAR ucEsId;
    if (prVEncMwInfo->u2SrcCompId >= (UINT16)MPV_COMP_NS)
    {
        return RMR_DRV_SET_FAILED;
    }

    ucEsId = _CompIdtoEsId(prVEncMwInfo->u2SrcCompId);
    _VDEC_SetVencHandle(
        ucEsId,
        (VOID *)prVEncMwInfo->hVEnc,
        (VOID *)prVEncMwInfo->ptFrmBuffMgr,
        (VOID *)prVEncMwInfo->ptFilter);
    _VDEC_SetVencFrmSz(ucEsId, prVEncMwInfo->u4Width, prVEncMwInfo->u4Height, prVEncMwInfo->u4Width);
    _VDEC_Set2VencFlow(ucEsId, TRUE);

    return RMR_OK;
}


static INT32 _VEncMwDisconnectVdec(VENC_MW_INFO_T *prVEncMwInfo)
{
    UCHAR ucEsId;
    ucEsId = _CompIdtoEsId(prVEncMwInfo->u2SrcCompId);
    _VDEC_Set2VencFlow(ucEsId, FALSE);

    return RMR_OK;
}


#if 0
static INT32 _VEncMwSetParamVdec(VENC_MW_INFO_T *prVEncMwInfo)
{
    return RMR_OK;
}
#endif

#endif /*VDEC_VENC_PATH*/


static INT32 _VEncMwSetH264Info(
    VENC_MW_INFO_T *prVEncMwInfo,
    VOID *pvFmt)
{
    UINT32 u4Width, u4Height, u4Fps, u4Bps;
    VID_ENC_COMMON_INFO_T  *pt_vid_enc_info;
    VID_FRAME_RESOLUTION_T  *pt_resolution;
    VID_ENC_H264_INFO_T *pt_fmt_h264 = (VID_ENC_H264_INFO_T *)pvFmt;


    if (NULL == prVEncMwInfo)
    {
        LOG(1, "%s: prVEncMwInfo is NULL\n", __FUNCTION__);
        return RMR_DRV_INV_SET_INFO;
    }

    if (NULL == pt_fmt_h264)
    {
        LOG(1, "%s: pt_fmt_h264 is NULL\n", __FUNCTION__);
        return RMR_DRV_INV_SET_INFO;
    }

    pt_vid_enc_info = &pt_fmt_h264->t_vid_enc_info;
    u4Bps           = pt_vid_enc_info->ui4_bitrate;
    u4Fps           = pt_vid_enc_info->ui4_fps;
    pt_resolution   = &pt_vid_enc_info->t_resolution;
    u4Width         = pt_resolution->ui4_width;
    u4Height        = pt_resolution->ui4_height;

    if (prVEncMwInfo->fgIsRunning)
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

        if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
        {
            return RMR_DRV_SET_FAILED;
        }
    }
    else
    {
        prVEncMwInfo->u4Profile = 0;
        prVEncMwInfo->u4Level   = 0;
        prVEncMwInfo->u4Width   = u4Width;
        prVEncMwInfo->u4Height  = u4Height;
        prVEncMwInfo->u4Fps     = u4Fps;
        prVEncMwInfo->u4Bps     = u4Bps;
    }

    return RMR_OK;
}


static INT32 _VEncMwSetEncFmt(
    VENC_MW_INFO_T *prVEncMwInfo,
    VID_ENC_FMT_TYPE_T e_enc_fmt)
{
    UINT32 type = 0;
    switch (e_enc_fmt)
    {
    default:
        prVEncMwInfo->SetFmtInfo = NULL;
        return RMR_DRV_INV_SET_INFO;

    case VID_ENC_FMT_H264:
        type = VENC_H264;
        prVEncMwInfo->SetFmtInfo = _VEncMwSetH264Info;
        break;
    }
    prVEncMwInfo->u4EncType = type;
    prVEncMwInfo->eEncFmt = e_enc_fmt;
    return RMR_OK;
}


static INT32 _VEncMwSetSrcType(
    VENC_MW_INFO_T *prVEncMwInfo,
    VID_SRC_TYPE_T e_src_type)
{
    switch (e_src_type)
    {
    default:
        return RMR_DRV_INV_SET_INFO;

    case VID_ENC_SRC_TYPE_VDEC:
        break;

    case VID_ENC_SRC_TYPE_VDP:
        break;
    }

    prVEncMwInfo->eSrcType = e_src_type;

    return RMR_OK;
}


static INT32 _VEncMwSetFmtInfo(
    VENC_MW_INFO_T *prVEncMwInfo,
    VOID *pvFmt)
{

    if (NULL == prVEncMwInfo->SetFmtInfo)
    {
        return RMR_NOT_INIT;
    }

    return prVEncMwInfo->SetFmtInfo(prVEncMwInfo, pvFmt);
}


static INT32 _VEncMwGetCtrl(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_get_info, SIZE_T *pz_get_info_len)
{
    BOOL *pfgCtrl;
    VENC_GET_VALIDATE(BOOL);

    pfgCtrl = (BOOL *)pv_get_info;
    *pfgCtrl = prVEncMwInfo->fgIsCtrlOn;

    return RMR_OK;
}


static INT32 _VEncMwH264VphonePlay(VENC_MW_INFO_T *prVEncMwInfo)
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

    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        LOG(0, "%s(%d): handle of venc is not release yet!\n", __FUNCTION__, __LINE__);
        ASSERT(VENC_NULL_HANDLE == prVEncMwInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_VPHONE, FALSE);
    if (VENC_NULL_HANDLE == hVenc)
    {
        LOG(0, "%s(%d): Open venc fail!\n", __FUNCTION__, __LINE__);
        return VENC_INV_HANDLE;
    }
    prVEncMwInfo->hVEnc = hVenc;

    u4SrcWidth = prVEncMwInfo->u4Width;
    u4SrcHeight = prVEncMwInfo->u4Height;
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);

    u4Value = prVEncMwInfo->u4Fps;
    LOG(2, "%s: set %d fps\n", __FUNCTION__, u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set framerate failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prVEncMwInfo->u4Bps;
    LOG(2, "%s: set %d kbps\n", __FUNCTION__, u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s(%d): Set bitrate failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4SrcWidth;
    res.u2Height = (UINT16)u4SrcHeight;
    LOG(2, "%s: set %dx%d\n", __FUNCTION__, res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s(%d): Set resolution failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncMwInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4SrcPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       LOG(1, "%s(%d): Set pixfmt failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
       return RMR_DRV_SET_FAILED;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set sync mode error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prVEncMwInfo;
    rCb.cb     = _VEncMwCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set callback error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
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
    ASSERT(NULL == prVEncMwInfo->ptFrmBuffMgr);
    prVEncMwInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncMwInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncMwInfo->ptFrmBuffMgr);
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
        _BUFMGR_OBJ_Add(prVEncMwInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set buffer error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }
}

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Init fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Run fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    prVEncMwInfo->fgIsRunning = TRUE;

    return VENC_OK;
}


static INT32 _VEncMwH264AtvPvrPlay(VENC_MW_INFO_T *prVEncMwInfo)
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

    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        LOG(0, "%s(%d): handle of venc is not release yet!\n", __FUNCTION__, __LINE__);
        ASSERT(VENC_NULL_HANDLE == prVEncMwInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_PVR, fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        LOG(0, "%s(%d): Open venc fail!\n", __FUNCTION__, __LINE__);
        return VENC_INV_HANDLE;
    }
    prVEncMwInfo->hVEnc = hVenc;

    u4SrcWidth = wDrvVideoInputWidth(VDP_2);//u2DrvDIGetVdpWidth(1);
    u4SrcHeight = wDrvVideoInputHeight(VDP_2); //u2DrvDIGetVdpHeight(1);
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4Fps = bDrvVideoGetRefreshRate(VDP_2);
    prVEncMwInfo->u4SrcWidth = u4SrcWidth;
    prVEncMwInfo->u4SrcHeight = u4SrcHeight;
    prVEncMwInfo->u4SrcFps = u4Fps;
    prVEncMwInfo->u4Width = u4SrcWidth;
    prVEncMwInfo->u4Height = u4SrcHeight;
    prVEncMwInfo->fgInterlace = fgInterlace;
    if (fgInterlace)
    {
        u4Fps >>= 1;
    }
    else if (u4SrcWidth * u4SrcHeight * u4Fps > 1280 * 736 * 30)
    {
        u4Fps >>= 1;
        vDrvDIVencDropFrame(1);
    }
    LOG(1, "%s: %dx%d @ %d fps\n", __FUNCTION__, u4SrcWidth, u4SrcHeight, u4Fps);
    prVEncMwInfo->u4Fps = u4Fps;
    u4Value = u4Fps;
    LOG(2, "%s: set %d fps\n", __FUNCTION__, u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set framerate failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prVEncMwInfo->u4Bps;
    LOG(2, "%s: set %d kbps\n", __FUNCTION__, u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s(%d): Set bitrate failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4SrcWidth;
    res.u2Height = (UINT16)u4SrcHeight;
    LOG(2, "%s: set %dx%d\n", __FUNCTION__, res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s(%d): Set resolution failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncMwInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4SrcPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       LOG(1, "%s(%d): Set pixfmt failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
       return RMR_DRV_SET_FAILED;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set sync mode error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prVEncMwInfo;
    rCb.cb     = _VEncMwCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set callback error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
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
    ASSERT(NULL == prVEncMwInfo->ptFrmBuffMgr);
    prVEncMwInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncMwInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncMwInfo->ptFrmBuffMgr);
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
        _BUFMGR_OBJ_Add(prVEncMwInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set buffer error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }
}

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Init fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Run fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    prVEncMwInfo->fgIsRunning = TRUE;

    return VENC_OK;
}


static UINT32 _VEncMwH264MsPlay(VENC_MW_INFO_T *prVEncMwInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4Fps;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth;
    UINT32 u4SrcHeight;
    UINT32 u4TgtPitch;
    UINT32 u4TgtWidth;
    UINT32 u4TgtHeight;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;
    VENC_CTRL_RZ_T rRz;
    BOOL fgResize = FALSE;
    BOOL fgInterlace = bDrvVideoIsSrcInterlace(VDP_2)? TRUE : FALSE;

    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        LOG(0, "%s(%d): handle of venc is not release yet!\n", __FUNCTION__, __LINE__);
        ASSERT(VENC_NULL_HANDLE == prVEncMwInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_MS, fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        LOG(0, "%s(%d): Open venc fail!\n", __FUNCTION__, __LINE__);
        return VENC_INV_HANDLE;
    }
    prVEncMwInfo->hVEnc = hVenc;

    u4TgtWidth = prVEncMwInfo->u4Width;
    u4TgtHeight = prVEncMwInfo->u4Height;
    u4TgtPitch = VENC_ALIGN_MASK(u4TgtWidth, 15);
    u4SrcWidth = wDrvVideoInputWidth(VDP_2);//u2DrvDIGetVdpWidth(1);
    u4SrcHeight = wDrvVideoInputHeight(VDP_2); //u2DrvDIGetVdpHeight(1);
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4Fps = bDrvVideoGetRefreshRate(VDP_2);
    prVEncMwInfo->u4SrcWidth = u4SrcWidth;
    prVEncMwInfo->u4SrcHeight = u4SrcHeight;
    prVEncMwInfo->u4SrcFps = u4Fps;
    prVEncMwInfo->fgInterlace = fgInterlace;
    if (fgInterlace)
    {
        u4Fps >>= 1;
    }
    else if (u4TgtWidth * u4TgtHeight * u4Fps > _VENC_HALFFPS_THRD)
    {
        u4Fps >>= 1;
        vDrvDIVencDropFrame(1);
    }
    prVEncMwInfo->u4Fps = u4Fps;

    if ((u4SrcWidth != u4TgtWidth) || (u4SrcHeight != u4TgtHeight))
    {
        fgResize = TRUE;
        LOG(0, "\n\n %s: %dx%d resize to %dx%d @ %d fps Interlace %d \n\n", __FUNCTION__, u4SrcWidth, u4SrcHeight, u4TgtWidth, u4TgtHeight, u4Fps, fgInterlace);
    }
    else
    {
        LOG(1, "%s: %dx%d @ %d fps\n", __FUNCTION__, u4SrcWidth, u4SrcHeight, u4Fps);
    }

    u4Value = u4Fps;
    LOG(2, "%s: set %d fps\n", __FUNCTION__, u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set framerate failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prVEncMwInfo->u4Bps;
    LOG(2, "%s: set %d kbps\n", __FUNCTION__, u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s(%d): Set bitrate failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4TgtWidth;
    res.u2Height = (UINT16)u4TgtHeight;
    LOG(2, "%s: set %dx%d\n", __FUNCTION__, res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s(%d): Set resolution failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prVEncMwInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4TgtPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       LOG(1, "%s(%d): Set pixfmt failed(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
       return RMR_DRV_SET_FAILED;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set sync mode error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prVEncMwInfo;
    rCb.cb     = _VEncMwCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set callback error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
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
    ASSERT(NULL == prVEncMwInfo->ptFrmBuffMgr);
    prVEncMwInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prVEncMwInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prVEncMwInfo->ptFrmBuffMgr);
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
        _BUFMGR_OBJ_Add(prVEncMwInfo->ptFrmBuffMgr, &rFrm);
    }

    if (fgResize)
    {
        ASSERT(NULL == prVEncMwInfo->ptResizeFrmBuffMgr);
        prVEncMwInfo->ptResizeFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_RZ_FRM);
        ASSERT(prVEncMwInfo->ptResizeFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prVEncMwInfo->ptResizeFrmBuffMgr);
        u4Size = u4SrcPitch * u4SrcHeight;

        for (u4Idx = 0; u4Idx < VENC_RESIZE_BUF_N; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prVEncMwInfo->ptResizeFrmBuffMgr, &rFrm);
        }
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Set buffer error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }
}

    if (fgResize)
    {
        rRz.u4SrcPitch = u4SrcPitch;
        rRz.u4SrcWidth = u4SrcWidth;
        rRz.u4SrcHeight = u4SrcHeight;
        rRz.pvIn = (void *)prVEncMwInfo->ptResizeFrmBuffMgr;
        rRz.pvOut = (void *)prVEncMwInfo->ptFrmBuffMgr;
        u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_RZ, (VOID*)&rRz);
        if (VENC_OK != u4Ret)
        {
            LOG(0, "%s(%d): Set resize error(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
            return -1;
        }
    }

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Init fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): Run fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    prVEncMwInfo->fgIsRunning = TRUE;

    return VENC_OK;
}


static INT32 _VEncMwH264Play(VENC_MW_INFO_T *prVEncMwInfo)
{
    UINT32 u4Ret;

    //set input source parameters
    if (prVEncMwInfo->SetInputParam && (RMR_OK != prVEncMwInfo->SetInputParam(prVEncMwInfo)))
    {
        //This means signal not stable and should not do restart
        // MW can't accept returning Fail, so it returns OK.
        LOG(1, "%s: Set input source parameters failed! Signal Not stable\n", __FUNCTION__);
        return RMR_OK;
    }

    switch (prVEncMwInfo->eProfileDef)
    {
    default:
        LOG(0, "%s: unexpected profile(%d)!\n", __FUNCTION__, prVEncMwInfo->eProfileDef);
        ASSERT(0);
        return RMR_DRV_SET_FAILED;

    case VENC_PROFILE_VPHONE:
        u4Ret = _VEncMwH264VphonePlay(prVEncMwInfo);
        break;

    case VENC_PROFILE_PVR:
        u4Ret = _VEncMwH264AtvPvrPlay(prVEncMwInfo);
        break;

    case VENC_PROFILE_MS:
        u4Ret = _VEncMwH264MsPlay(prVEncMwInfo);
        break;
    }

    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s: play fail %d!\n", __FUNCTION__, u4Ret);
        ASSERT(0);
        return RMR_DRV_SET_FAILED;
    }

    //##connect input source
    if (prVEncMwInfo->ConnectInput && (RMR_OK != prVEncMwInfo->ConnectInput(prVEncMwInfo)))
    {
        LOG(1, "%s: Connect to input source failed!\n", __FUNCTION__);
        return RMR_DRV_SET_FAILED;
    }

    return RMR_OK;
}


static INT32 _VEncMwH264Stop(VENC_MW_INFO_T *prVEncMwInfo)
{
    UINT32 u4Ret;
    INT32 i4Ret = RMR_OK;

    //disconnect  input source
    if (prVEncMwInfo->DisconnectInput && (RMR_OK != prVEncMwInfo->DisconnectInput(prVEncMwInfo)))
    {
        LOG(1, "%s: disconnect input source failed!\n", __FUNCTION__);
        return RMR_DRV_SET_FAILED;
    }

    //stop
    u4Ret = VENC_Stop(prVEncMwInfo->hVEnc);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s: venc stop failed!\n", __FUNCTION__);
        i4Ret = RMR_DRV_SET_FAILED;
    }

    //CLOSE VENC
    u4Ret = VENC_Close(prVEncMwInfo->hVEnc);
    if (VENC_OK != u4Ret)
    {
        LOG(1, "%s: venc close fail(%d)\n", __FUNCTION__, u4Ret);
        i4Ret = RMR_DRV_SET_FAILED;
    }
    prVEncMwInfo->hVEnc = VENC_NULL_HANDLE;

    if (prVEncMwInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prVEncMwInfo->ptFrmBuffMgr));
        prVEncMwInfo->ptFrmBuffMgr = NULL;
    }

    if (prVEncMwInfo->ptResizeFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prVEncMwInfo->ptResizeFrmBuffMgr));
        prVEncMwInfo->ptResizeFrmBuffMgr = NULL;
    }

    return i4Ret;
}


static INT32 _VEncMwSetCtrl(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_set_info)
{
    BOOL fgCtrl;
    INT32 i4Ret = RMR_OK;

    fgCtrl = (pv_set_info)? TRUE : FALSE;

    if (prVEncMwInfo->fgIsCtrlOn != fgCtrl)
    {
        if (fgCtrl)
        {
            switch (prVEncMwInfo->eEncFmt)
            {
            default:
                LOG(0, "%s: unexpected type(%d)!\n", __FUNCTION__, prVEncMwInfo->eEncFmt);
                ASSERT(0);
                return RMR_DRV_SET_FAILED;

            case VID_ENC_FMT_H264:
                i4Ret = _VEncMwH264Play(prVEncMwInfo);
                break;
            }
        }
        else
        {
            switch (prVEncMwInfo->eEncFmt)
            {
            default:
                LOG(0, "%s: unexpected type(%d)!\n", __FUNCTION__, prVEncMwInfo->eEncFmt);
                ASSERT(0);
                return RMR_DRV_SET_FAILED;

            case VID_ENC_FMT_H264:
                i4Ret = _VEncMwH264Stop(prVEncMwInfo);
                break;
            }
        }
        prVEncMwInfo->fgIsCtrlOn = fgCtrl;
    }

    return i4Ret;
}


static UINT32 _VEncMwH264AtvPvrRePlay(VENC_MW_INFO_T *prVEncMwInfo)
{
    UINT32 u4Ret;

    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        // stop DI write mode
        vDrvDIStopSendBufToVENC(TRUE);

        if (!_VEncMwDestroy(prVEncMwInfo, FALSE))
        {
            return VENC_INTERNAL_ERR;
        }
    }

    u4Ret = _VEncMwH264AtvPvrPlay(prVEncMwInfo);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): _VEncMwH264MsPlay fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}


static UINT32 _VEncMwH264MsRePlay(VENC_MW_INFO_T *prVEncMwInfo)
{
    UINT32 u4Ret;

    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        // stop DI write mode
        vDrvDIStopSendBufToVENC(TRUE);

        if (!_VEncMwDestroy(prVEncMwInfo, FALSE))
        {
            return VENC_INTERNAL_ERR;
        }
    }

    u4Ret = _VEncMwH264MsPlay(prVEncMwInfo);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s(%d): _VEncMwH264MsPlay fail(%d)!\n", __FUNCTION__, __LINE__, u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}


static INT32 _VEncMwH264RePlay(VENC_MW_INFO_T *prVEncMwInfo)
{
    UINT32 u4Ret;

    //set input source parameters
    if (prVEncMwInfo->SetInputParam && (RMR_OK != prVEncMwInfo->SetInputParam(prVEncMwInfo)))
    {
        //This means signal not stable and should not do restart
        // MW can't accept returning Fail, so it returns OK.
        LOG(1, "%s: Set input source parameters failed! Signal Not stable\n", __FUNCTION__);
        return RMR_OK;
    }
        
    LOG(2, "%s: profile(%d)\n", __FUNCTION__, prVEncMwInfo->eProfileDef);
    switch (prVEncMwInfo->eProfileDef)
    {
    default:
        LOG(0, "%s: unexpected profile(%d)!\n", __FUNCTION__, prVEncMwInfo->eProfileDef);
        ASSERT(0);
        return RMR_DRV_SET_FAILED;
#if 0 // not used yet
    case VENC_PROFILE_VPHONE:
        u4Ret = _VEncMwH264VphonePlay(prVEncMwInfo);
        break;
#endif
    case VENC_PROFILE_PVR:
        u4Ret = _VEncMwH264AtvPvrRePlay(prVEncMwInfo);
        break;

    case VENC_PROFILE_MS:
        u4Ret = _VEncMwH264MsRePlay(prVEncMwInfo);
        break;
    }

    if (VENC_OK != u4Ret)
    {
        LOG(0, "%s: play fail %d!\n", __FUNCTION__, u4Ret);
        ASSERT(0);
        return RMR_DRV_SET_FAILED;
    }

    //##connect input source
    if (prVEncMwInfo->ConnectInput && (RMR_OK != prVEncMwInfo->ConnectInput(prVEncMwInfo)))
    {
        LOG(1, "%s: Connect to input source failed!\n", __FUNCTION__);
        return RMR_DRV_SET_FAILED;
    }

    return RMR_OK;
}


static INT32 _VEncMwSetRestart(VENC_MW_INFO_T *prVEncMwInfo)
{
    INT32 i4Ret = RMR_OK;

    if (!prVEncMwInfo->fgIsCtrlOn)
    {
        LOG(1, "%s: ctrl is off\n", __FUNCTION__);
        return RMR_DRV_SET_FAILED;
    }

    switch (prVEncMwInfo->eEncFmt)
    {
    default:
        LOG(0, "%s: unexpected type(%d)!\n", __FUNCTION__, prVEncMwInfo->eEncFmt);
        ASSERT(0);
        return RMR_DRV_SET_FAILED;

    case VID_ENC_FMT_H264:
        i4Ret = _VEncMwH264RePlay(prVEncMwInfo);
        break;
    }

    return i4Ret;
}


static INT32 _VEncMwSetSettings(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_set_info, SIZE_T z_set_info_len)
{
    INT32 i4Ret = RMR_DRV_SET_FAILED;
    VID_ENC_SETTINGS_T *prSettings;

    VENC_SET_VALIDATE(VID_ENC_SETTINGS_T);

    if (prVEncMwInfo->fgIsRunning)
    {
        LOG(1, "%s: venc is running\n", __FUNCTION__);
        return RMR_DRV_SET_FAILED;
    }

    prSettings = (VID_ENC_SETTINGS_T *)pv_set_info;

    i4Ret = _VEncMwSetEncFmt(prVEncMwInfo, prSettings->e_enc_fmt);
    if (i4Ret != RMR_OK)
    {
        LOG(1, "%s: error enc type [%d]\n", __FUNCTION__, prSettings->e_enc_fmt);
        return i4Ret;
    }

    i4Ret = _VEncMwSetSrcType(prVEncMwInfo, prSettings->e_src_type);
    if (i4Ret != RMR_OK)
    {
        LOG(1, "%s: error src type [%d]\n", __FUNCTION__, prSettings->e_src_type);
        return i4Ret;
    }

    i4Ret = _VEncMwSetFmtInfo(prVEncMwInfo, (VOID *)&prSettings->u_fmt);
    if (i4Ret != RMR_OK)
    {
        LOG(1, "%s: error fmt info\n", __FUNCTION__);
        return i4Ret;
    }

    return RMR_OK;
}


static INT32 _VEncMwSetFrameResolution(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_set_info, SIZE_T z_set_info_len)
{
    VID_FRAME_RESOLUTION_T *pt_resolution;
    UINT32 u4Width, u4Height;
    VENC_SET_VALIDATE(VID_FRAME_RESOLUTION_T);

    pt_resolution = (VID_FRAME_RESOLUTION_T *)pv_set_info;

    u4Width  = pt_resolution->ui4_width;
    u4Height = pt_resolution->ui4_height;
    prVEncMwInfo->u4Width  = u4Width;
    prVEncMwInfo->u4Height = u4Height;

    if (prVEncMwInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
        rRTParm.u2Width  = u4Width;
        rRTParm.u2Height = u4Height;
        rRTParm.u2Pitch  = VENC_ALIGN_MASK(u4Width, 15);

        if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
        {
            return RMR_DRV_SET_FAILED;
        }
    }

    return RMR_OK;
}


static INT32 _VEncMwSetFramerate(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_set_info)
{
    UINT32 u4Fps = (UINT32)pv_set_info;

    if (!u4Fps) // fps could not be 0
    {
        return RMR_DRV_INV_SET_INFO;
    }

    prVEncMwInfo->u4Fps = u4Fps;

    if (prVEncMwInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_FRAMERATE;
        rRTParm.u1Framerate = (UINT8)u4Fps;

        if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
        {
            return RMR_DRV_SET_FAILED;
        }
    }

    return RMR_OK;
}


static INT32 _VEncMwSetBitrate(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_set_info)
{
    UINT32 u4Bps = (UINT32)pv_set_info;

    if (!u4Bps) // bitrate could not be 0
    {
        return RMR_DRV_INV_SET_INFO;
    }

    prVEncMwInfo->u4Bps = u4Bps;

    if (prVEncMwInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_BITRATE;
        rRTParm.u4Bitrate   = u4Bps;

        if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
        {
            return RMR_DRV_SET_FAILED;
        }
    }

    return RMR_OK;
}


static INT32 _VEncMwSetApType(VENC_MW_INFO_T *prVEncMwInfo, const VOID* pv_set_info)
{
    switch ((VID_AP_TYPE_T)pv_set_info)
    {
    default:
        LOG(1, "%s: unknown type %d\n", __FUNCTION__, pv_set_info);
        return RMR_DRV_INV_SET_INFO;

    case VID_ENC_AP_TYPE_SKYPE:
        LOG(1, "VID_ENC_AP_TYPE_SKYPE\n");
        prVEncMwInfo->eProfileDef     = VENC_PROFILE_VPHONE;
        prVEncMwInfo->ePixFmt         = VENC_PIX_FMT_NV12;
        #ifdef VDEC_VENC_PATH
        prVEncMwInfo->ConnectInput    = _VEncMwConnectVdec;
        prVEncMwInfo->DisconnectInput = _VEncMwDisconnectVdec;
        prVEncMwInfo->SetInputParam   = NULL;
        #endif /*VDEC_VENC_PATH*/
        break;

    case VID_ENC_AP_TYPE_BGPVR:
        LOG(1, "VID_ENC_AP_TYPE_BGPVR\n");
        prVEncMwInfo->eProfileDef     = VENC_PROFILE_PVR;
        prVEncMwInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prVEncMwInfo->ConnectInput    = _VEncMwConnectDi;
        prVEncMwInfo->DisconnectInput = _VEncMwDisconnectDi;
        prVEncMwInfo->SetInputParam   = _VEncMwSetParamDi;
        break;

    case VID_ENC_AP_TYPE_ATVPVR:
        LOG(1, "VID_ENC_AP_TYPE_ATVPVR\n");
        prVEncMwInfo->eProfileDef     = VENC_PROFILE_PVR;
        prVEncMwInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prVEncMwInfo->ConnectInput    = _VEncMwConnectDi;
        prVEncMwInfo->DisconnectInput = _VEncMwDisconnectDi;
        prVEncMwInfo->SetInputParam   = _VEncMwSetParamDi;
        break;

    case VID_ENC_AP_TYPE_MSI:
        LOG(1, "VENC_PROFILE_MS\n");
        prVEncMwInfo->eProfileDef     = VENC_PROFILE_MS;
        prVEncMwInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prVEncMwInfo->ConnectInput    = _VEncMwConnectDi;
        prVEncMwInfo->DisconnectInput = _VEncMwDisconnectDi;
        prVEncMwInfo->SetInputParam   = _VEncMwSetParamDi;
        break;
    }

    return RMR_OK;
}


static INT32 _VEncMwSetInsertKeyframe(VENC_MW_INFO_T *prVEncMwInfo)
{
    if (prVEncMwInfo->fgIsRunning)
    {
        VENC_RT_CTRL_T rRTCtrl;
        rRTCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME; 
        VENC_SetRtCtrlAsync(prVEncMwInfo->hVEnc, &rRTCtrl);
        return RMR_OK;
    }

    return RMR_DRV_INV_SET_INFO;
}


/* component functions */
static INT32 _VEncMwConnect(
                            DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    DRV_COMP_ID_T* prCompReg;
    VENC_MW_INFO_T *prVEncMwInfo;
    UINT16 u2CompId;

    MTVENC_LockApiMutex();
    LOG(1, "%s: begin\n", __FUNCTION__);

    UNUSED(z_conn_info_len);
    UNUSED(pv_tag);
    UNUSED(pf_nfy);

    // Check component type and id
    if (NULL == pt_comp_id)
    {
        goto VENC_MW_INV_CONN_INFO;
    }

    if (DRVT_VID_ENC != pt_comp_id->e_type)
    {
       goto VENC_MW_INV_CONN_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n",
            __FUNCTION__, u2CompId);
       goto VENC_MW_INV_CONN_INFO;
    }

    if (pt_comp_id->b_sel_out_port)
    {
       goto VENC_MW_INV_CONN_INFO;
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
       goto VENC_MW_INV_CONN_INFO;
    }

    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
       goto VENC_MW_INV_CONN_INFO;
    }

    if (NULL == pv_conn_info)
    {
        goto VENC_MW_INV_CONN_INFO;
    }

    if (RM_CONN_TYPE_COMP_ID != e_conn_type)
    {
        LOG(1, "%s: unexpected conn type [%d]\n", __FUNCTION__, e_conn_type);
        goto VENC_MW_INV_CONN_INFO;
    }

    prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
    prVEncMwInfo = _VEncMwGetInfoInst(u2CompId);
    if (NULL == prVEncMwInfo)
    {
        goto VENC_MW_INV_CONN_INFO;
    }

    if (VENC_NULL_HANDLE != prVEncMwInfo->hVEnc)
    {
        LOG(1, "%s, [id]%d has already been connected\n", __FUNCTION__, u2CompId);

        if (!_VEncMwDestroy(prVEncMwInfo, FALSE))
        {
            goto VENC_MW_INV_CONN_INFO;
        }
    }

    if ((prCompReg->e_type != DRVT_VID_PLANE)
#ifdef VDEC_VENC_PATH
    && (prCompReg->e_type != DRVT_VID_DEC)
#endif
    )
    {
        LOG(1, "%s: unexpected comp type [%d]\n", __FUNCTION__, prCompReg->e_type);
        goto VENC_MW_INV_CONN_INFO;
    }

    //init venc MW Info
    _VEncMwResetInfoInst(prVEncMwInfo);
    prVEncMwInfo->u2SrcCompId = prCompReg->ui2_id;

    LOG(1, "%s: video encoder(%d) connect successfully!\n", __FUNCTION__, u2CompId, prCompReg->ui2_id);

    MTVENC_UnLockApiMutex();
    return RMR_OK;

VENC_MW_INV_CONN_INFO:
    MTVENC_UnLockApiMutex();
    return RMR_DRV_INV_CONN_INFO;
}


static INT32 _VEncMwDisconnect(
                            DRV_COMP_ID_T*   pt_comp_id,
                            DRV_DISC_TYPE_T  e_disc_type,
                            const VOID*      pv_disc_info,
                            SIZE_T           z_disc_info_len)
{
    DRV_COMP_ID_T* prCompReg;
    VENC_MW_INFO_T *prVEncMwInfo;
    UINT16 u2CompId;

    MTVENC_LockApiMutex();
    LOG(1, "%s: begin\n", __FUNCTION__);

    // Check component type and id
    if (pt_comp_id == NULL)
    {
        goto VENC_MW_INV_DISC_INFO;
    }

    if (pt_comp_id->e_type != DRVT_VID_ENC)
    {
        goto VENC_MW_INV_DISC_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n",
            __FUNCTION__, u2CompId);
        goto VENC_MW_INV_DISC_INFO;
    }

    if (pt_comp_id->b_sel_out_port)
    {
       goto VENC_MW_INV_DISC_INFO;
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
       goto VENC_MW_INV_DISC_INFO;
    }

    if (e_disc_type != RM_CONN_TYPE_COMP_ID)
    {
       goto VENC_MW_INV_DISC_INFO;
    }

    if (pv_disc_info == NULL)
    {
        goto VENC_MW_INV_DISC_INFO;
    }

    prCompReg = (DRV_COMP_ID_T*)pv_disc_info;
    prVEncMwInfo = _VEncMwGetInfoInst(u2CompId);

    if ((prCompReg->e_type != DRVT_VID_PLANE)
#ifdef VDEC_VENC_PATH
    && (prCompReg->e_type != DRVT_VID_DEC)
#endif
    )
    {
        LOG(1, "%s: unexpected comp type [%d]\n", __FUNCTION__, prCompReg->e_type);
        goto VENC_MW_INV_DISC_INFO;
    }

    //deinit venc mw info
    if (!_VEncMwDestroy(prVEncMwInfo, FALSE))
    {
        return RMR_DRV_CONN_FAILED;
    }

    UNUSED(z_disc_info_len);

    LOG(1, "%s: video encoder disconnect successfully!\n", __FUNCTION__);
    MTVENC_UnLockApiMutex();
    return RMR_OK;

VENC_MW_INV_DISC_INFO:
    MTVENC_UnLockApiMutex();
    return RMR_DRV_INV_DISC_INFO;

VENC_MW_DISC_FAILED:
    MTVENC_UnLockApiMutex();
    return RMR_DRV_DISC_FAILED;
}


static INT32 _VEncMwGet(
    DRV_COMP_ID_T*        pt_comp_id,
    DRV_GET_TYPE_T        e_get_type,
    VOID*                pv_get_info,
    SIZE_T*                pz_get_info_len)
{
    INT32 i4RetCode = RMR_DRV_INV_GET_INFO;
    VENC_MW_INFO_T *prVEncMwInfo;
    UINT16 u2CompId;

    // Check component type and id
    if (NULL == pt_comp_id)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    if (DRVT_VID_ENC != pt_comp_id->e_type)
    {
        LOG(1, "%s: error component type or id type [%d]\n",
            __FUNCTION__, pt_comp_id->e_type);
        return RMR_DRV_INV_GET_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n",
            __FUNCTION__, u2CompId);
        return RMR_DRV_INV_GET_INFO;
    }

    if (NULL == pv_get_info)
    {
        LOG(1, "%s: pv_get_info is NULL\n", __FUNCTION__);
        return RMR_DRV_INV_GET_INFO;
    }

    if (NULL == pz_get_info_len)
    {
        LOG(1, "%s: pz_get_info_len is NULL\n", __FUNCTION__);
        return RMR_DRV_INV_GET_INFO;
    }

    prVEncMwInfo = _VEncMwGetInfoInst(u2CompId);
    if (NULL == prVEncMwInfo)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    MTVENC_LockApiMutex();
    switch (e_get_type)
    {
    default:
        LOG(1, "%s: Invalid get_type - %d\n", __FUNCTION__, e_get_type);
        MTVENC_UnLockApiMutex();
        return RMR_DRV_INV_GET_INFO;

    case VID_ENC_GET_TYPE_CTRL:
        i4RetCode = _VEncMwGetCtrl(prVEncMwInfo, pv_get_info, pz_get_info_len);
        break;
    }

    MTVENC_UnLockApiMutex();
    return i4RetCode;
}


static INT32 _VEncMwSet(
                    DRV_COMP_ID_T*        pt_comp_id,
                    DRV_SET_TYPE_T         e_set_type,
                    const VOID*            pv_set_info,
                    SIZE_T                z_set_info_len)
{
    INT32 i4RetCode = RMR_DRV_INV_SET_INFO;
    VENC_MW_INFO_T *prVEncMwInfo;
    UINT16 u2CompId;

    // Check component type and id
    if (NULL == pt_comp_id)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    if (DRVT_VID_ENC != pt_comp_id->e_type)
    {
        LOG(1, "%s: error component type or id type [%d]\n",
            __FUNCTION__, pt_comp_id->e_type);
        return RMR_DRV_INV_SET_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n",
            __FUNCTION__, u2CompId);
        return RMR_DRV_INV_SET_INFO;
    }

    prVEncMwInfo = _VEncMwGetInfoInst(u2CompId);
    if (NULL == prVEncMwInfo)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    MTVENC_LockApiMutex();
    switch (e_set_type)
    {
    default:
        LOG(1, "%s: Invalid set_type - %d\n", __FUNCTION__, e_set_type);
        MTVENC_UnLockApiMutex();
        return RMR_DRV_INV_SET_INFO;

    case VID_ENC_SET_TYPE_CTRL:
        i4RetCode = _VEncMwSetCtrl(prVEncMwInfo, pv_set_info);
        break;

    case VID_ENC_SET_TYPE_RESTART:
        i4RetCode = _VEncMwSetRestart(prVEncMwInfo);
        break;

    case VID_ENC_SET_TYPE_APP_TYPE:
        i4RetCode = _VEncMwSetApType(prVEncMwInfo, pv_set_info);
        break;

    case VID_ENC_SET_TYPE_SETTINGS:
        i4RetCode = _VEncMwSetSettings(prVEncMwInfo, pv_set_info, z_set_info_len);
        break;

    //maybe used in runtime
    case VID_ENC_SET_TYPE_FRAME_RESOLUTION:
        i4RetCode = _VEncMwSetFrameResolution(prVEncMwInfo, pv_set_info, z_set_info_len);
        break;

    case VID_ENC_SET_TYPE_FRAME_RATE:
        i4RetCode = _VEncMwSetFramerate(prVEncMwInfo, pv_set_info);
        break;

    case VID_ENC_SET_TYPE_BITRATE:
        i4RetCode = _VEncMwSetBitrate(prVEncMwInfo, pv_set_info);
        break;

    case VID_ENC_SET_TYPE_INSERT_KEYFRAME:
        i4RetCode = _VEncMwSetInsertKeyframe(prVEncMwInfo);
        break;

    case VID_ENC_SET_TYPE_ENABLE_OSTG:
        // VID_ENC_SET_TYPE_ENABLE_OSTG return ok for backward compatible to 96
        i4RetCode = RMR_OK;
        break;

    case VID_ENC_SET_TYPE_ENCODE_OSD:
        // VID_ENC_SET_TYPE_ENCODE_OSD return ok for backward compatible to 96
        i4RetCode = RMR_OK;
        break;

    case VID_ENC_SET_TYPE_INPUT_SOURCE:
        // TODO: Please Victor FIXME
        i4RetCode = RMR_OK;
        break;
    }

    MTVENC_UnLockApiMutex();
    return i4RetCode;
}


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

INT32 MW_VENC_Init(void)
{
    INT32 i4Return;
    DRV_COMP_REG_T tVEncCompReg;
    DRV_COMP_FCT_TBL_T tVEncFctTbl;
    UINT32 i;

    if (!_hVEncMwApiMutex)
    {
        // create semaphore
        VERIFY(x_sema_create(&_hVEncMwApiMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
    }

    /* Init VENC MW Info */
    for (i = 0; i < VENC_COMP_NS; i++)
    {
        VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(i);
        _VEncMwResetInfoInst(prVEncMwInfo);
    }

    /* register venc component */
    tVEncCompReg.e_type           = DRVT_VID_ENC;
    tVEncCompReg.e_id_type        = ID_TYPE_IND;
    tVEncCompReg.u.t_ind.ui2_id   = (UINT16)VENC_COMP_1;
    tVEncCompReg.u.t_ind.ui1_port = ALL_PORTS;
    tVEncCompReg.u.t_ind.pv_tag   = NULL;

    /* Setup component function table */
    tVEncFctTbl.pf_rm_connect     = _VEncMwConnect;
    tVEncFctTbl.pf_rm_disconnect  = _VEncMwDisconnect;
    tVEncFctTbl.pf_rm_get         = _VEncMwGet;
    tVEncFctTbl.pf_rm_set         = _VEncMwSet;

    /* Register component with the Resource Manager database */
    i4Return = x_rm_reg_comp(&tVEncCompReg, 1, 1, NULL, 0, &tVEncFctTbl, NULL, 0);

    if (i4Return != RMR_OK)
    {
        LOG(1, "%s call x_rm_reg_comp NG [%d]\n", __FUNCTION__, i4Return);
        return i4Return;
    }

    return RMR_OK;
}


#ifdef CC_SUPPORT_MUXER
VOID MW_VENC_SetMxId(UINT16  u2Id, UCHAR ucMxId)
{
    VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(u2Id);
    prVEncMwInfo->ucMxId = ucMxId;
}


#if 0
ENUM_MUXER_VENC_T MW_VENC_GetMxVencType(UINT16  u2Id)
{
    VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(u2Id);
    ENUM_MUXER_VENC_T eVidFmt = ENUM_MUXER_VENC_UNKNOWN;

    switch (prVEncMwInfo->eEncFmt)
    {
    default:
    case VID_ENC_FMT_H264:
        eVidFmt = ENUM_MUXER_VENC_H264;
        break;
    }

    return eVidFmt;
}
#endif
#endif


#if 0 // for cli test
VOID MW_VENC_SetRes(UINT32 u4Width, UINT32 u4Height)
{
    VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(0);
    VENC_RT_PARM_T rRTParm;
    //RESOLUTION
    rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
    rRTParm.u2Width  = u4Width;
    rRTParm.u2Height = u4Height;
    rRTParm.u2Pitch  = VENC_ALIGN_MASK(u4Width, 15);

    if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
    {
        ASSERT(0);
    }
}


VOID MW_VENC_SetBps(UINT32 u4Bps)
{
    VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(0);

    VENC_RT_PARM_T rRTParm;
    rRTParm.u4Flag      = VENC_RT_PARM_FLAG_BITRATE;
    rRTParm.u4Bitrate   = u4Bps;

    if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
    {
        ASSERT(0);
    }
}


VOID MW_VENC_SetFps(UINT32 u4Fps)
{
    VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(0);
    VENC_RT_PARM_T rRTParm;
    rRTParm.u4Flag      = VENC_RT_PARM_FLAG_FRAMERATE;
    rRTParm.u1Framerate = (UINT8)u4Fps;

    if (VENC_OK != VENC_SetRtParamAsync(prVEncMwInfo->hVEnc, &rRTParm))
    {
        ASSERT(0);
    }
}
#endif


