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
 * $RCSfile: vdp_venc_ctrl.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifdef CC_SUPPORT_VENC

#include "srm_drvif.h"
#include "drv_di.h"
#include "vdp_venc_ctrl.h"
#include "drv_di.h"
#include "drv_scaler.h"
#include "vdo_rm.h"
#include "vdp_if.h"

#include "venc_if.h"
#include "drv_video.h"
#include "vdo_misc.h"
#include "nptv_debug.h"
#include "x_assert.h"


#define _VENC_MAX_WIDTH  (1280)
#define _VENC_MAX_HEIGHT ( 720)

typedef struct VDP_VENC_INFO
{
    VENC_PROFILE_T           eProfileDef;
    E_VDP_VENC_INPUT         eEncInput; 
    E_VDP_VENC_MODULE        eEncModule;
    E_VDP_VENC_FIELD_MODE    eEncFldType;
    UINT32                   u4TrgtWidth;    //recording width
    UINT32                   u4TrgtHeight;   //recording height
}VDP_VENC_INFO_t;


VDP_VENC_INFO_t           tVencInfo;
VDP_VENC_INPUT_RES_t      tVencRes;

static UINT8          u1WaitForVRM = FALSE;

static HANDLE_T       _hVencVDPCtrl;
static HANDLE_T       _hVencVDPWaitForVRM;
///// vdp related static function /////////



// to do set default eEncInput as BEfore DI
// buffer management for APP

static void vDrvVencCtrlConfig(VENC_PROFILE_T eProfileDef)
{

    tVencInfo.eEncInput     = E_VDP_VENC_SUB_BEFORE_DI;
    tVencInfo.eEncModule    = E_VDP_VENC_SUB_DI;
        
    // temp  need to modify
    if (VENC_PROFILE_PVR  == eProfileDef)
    {
        tVencInfo.u4TrgtWidth = _VENC_MAX_WIDTH;
        tVencInfo.u4TrgtHeight= _VENC_MAX_HEIGHT;
    }
    if (VENC_PROFILE_CUST == eProfileDef)
    {
        // special case, atvpvr wants to record the input resolution  need AP to modify it.
        tVencInfo.u4TrgtWidth  = (tVencInfo.u4TrgtWidth  == 0) ? _VENC_MAX_WIDTH  : tVencInfo.u4TrgtWidth;
        tVencInfo.u4TrgtHeight = (tVencInfo.u4TrgtHeight == 0) ? _VENC_MAX_HEIGHT : tVencInfo.u4TrgtHeight;
    }

               
    LOG(1 ,"- eEncInput:%d  eEncModule:%d eEncFldType %d\n", tVencInfo.eEncInput, tVencInfo.eEncModule, tVencInfo.eEncFldType);
    
}

static void inline vDrvVencSetFldMode(void)
{

    if (!tVencRes.fgInterlace)
    {
        vDrvDIVencFldRecordMode(E_DI_VENC_NORMAL);
    }
    else
    {
        switch (tVencInfo.eEncFldType)
        {
            case E_VDP_VENC_TOP_FLD: 
                vDrvDIVencFldRecordMode(E_DI_VENC_TOP_FLD);
                break;
            case E_VDP_VENC_BOTTOM_FLD:
                vDrvDIVencFldRecordMode(E_DI_VENC_BOT_FLD);
                break;
            default: //E_VDP_VENC_ENC_BOTH_FLD
                vDrvDIVencFldRecordMode(E_DI_VENC_NORMAL);
                break;
        }
    }           

}


static void vDrvVencCalVencInRes(void)
{
    VRM_INFO_T*  vrmInfo; 

    vrmInfo = VRMGetVRMInfo(SV_VP_PIP, VRM_MODULE_MIB);

    tVencRes.u4EncInWidth     = (UINT32)vrmInfo->u2Width;
    tVencRes.u4EncInHeight    = (UINT32)vrmInfo->u2Height; 
    tVencRes.u4Fps            = (UINT32)vrmInfo->u2FrameRate;
    tVencRes.fgInterlace      = (UINT8) vrmInfo->u2ScanMode;   
    
    if (tVencRes.fgInterlace && bDrvVencVDPIsOneFldMode())
    {
        tVencRes.u4EncInHeight >>= 1;
    }  

    LOG(1 ,"- eEncInput: %d EncInHeight: %d u4Fps:%d\n", tVencRes.u4EncInWidth, tVencRes.u4EncInHeight, tVencRes.u4Fps);
    LOG(1 , "- TgtWidth:  %d TgtHeight:   %d fgInterlace:%d\n", tVencInfo.u4TrgtWidth, tVencInfo.u4TrgtHeight, tVencRes.fgInterlace);

}


void vDrvVencCtrlStopModule(void)
{
    vDrvDIStopSendBufToVENC(TRUE);
}


static void vDrvVencVDPEnableWaitForVRM(UINT8 fgEnable)
{
    CRIT_STATE_T csState;
    csState = x_crit_start();
    u1WaitForVRM = fgEnable;
    x_crit_end(csState);
}

static UINT8 u1DrvVencVDPIsWaitingForVRM(void)
{
    return u1WaitForVRM;
}

////////////////////////////////////////////// interface

UINT8 bDrvVencVDPIsOneFldMode()
{
    if (tVencInfo.eEncFldType == E_VDP_VENC_TOP_FLD||
        tVencInfo.eEncFldType == E_VDP_VENC_BOTTOM_FLD)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

UINT8 u1DrvVencVDPSignalReady()
{
    if (E_VDP_VENC_MAIN_BEFORE_PRED == tVencInfo.eEncInput)
    {
        if (bDrvVideoSignalStatus(VDP_1) != SV_VDO_STABLE)
        {
            LOG(1, " Main Path Signal not stable \n");
            return FALSE;
        }
        // Question may also need to check may's color space ???
        return TRUE;
    }
    else if (E_VDP_VENC_SUB_AFTER_DI    == tVencInfo.eEncInput ||
             E_VDP_VENC_SUB_BEFORE_DI   == tVencInfo.eEncInput ||
             E_VDP_VENC_SUB_BEFORE_PRED == tVencInfo.eEncInput)
    {
        if (bDrvVideoSignalStatus(VDP_2) != SV_VDO_STABLE)
        {
            LOG(1, " Sub Path Signal not stable \n");
            return FALSE;
        }
        // force modechange to make it always become 422
        if (bIsScalerInputRGB(VDP_2))
        {    
            vSetPipFlg(PIP_FLG_MODE_CHG);
            vSetPipFlg(PIP_FLG_MODE_DET_DONE);
            LOG(1, " 3x3 Matrix is not ready \n");
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

//not sure about this part
UINT8 u1DrvVencVDPVRMReady()
{
    if (_bPipState != VDO_STATE_IDLE)
    {
        return FALSE;
    }
    else if (VRM_EVENT_NONE == vVRMGetEventFlg(VDP_2))
    {
        return TRUE;
    }
    else
    {
        LOG(1 ," Sub VRM is not ready \n");
        return FALSE;
    }
}



void vDrvVencVDPDropFrame(UINT8 u1OnOff, UINT8 u1M, UINT8 u1N)
{
    vDrvDIVencDropFrame(u1OnOff);
    LOG(1 ,"- DI Frame Drop %d!\n",u1OnOff);
}

void vDrvVencVDPStart(void *handle, void *ptFrmBuffMgr, void *ptResizeFrmBuffMgr, void *ptFilterPts)
{
    VERIFY (x_sema_lock(_hVencVDPCtrl, X_SEMA_OPTION_WAIT) == OSR_OK);

    LOG(1," vDrvVencVDPStart \n");

    vDrvDISupportH264Init();
    vDrvDISetVEncHandle(handle, ptFrmBuffMgr, ptResizeFrmBuffMgr, ptFilterPts);

#ifdef CC_ATV_PVR_SUPPORT
    if (VENC_PROFILE_PVR == tVencInfo.eProfileDef)
    {
        // trigger VBI
        VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, TRUE);
    }
#endif
    // complete the whole start procedure unlock the semaphore
    x_sema_unlock(_hVencVDPCtrl);
}

void vDrvVencVDPStop()
{
    VERIFY (x_sema_lock(_hVencVDPCtrl, X_SEMA_OPTION_WAIT) == OSR_OK);

    LOG(1," vDrvVencVDPStop \n");
#ifdef CC_ATV_PVR_SUPPORT
    if (VENC_PROFILE_PVR == tVencInfo.eProfileDef)
    {
        // stop VBI
        VBI_PVRStartNotifyCCOnOff(SV_VP_MAIN, FALSE);
    }
#endif

    vDrvVrmClrAppFlag(VRM_APP_VIDEO_ENC);
    vDrvVencCtrlStopModule();
    
    x_sema_unlock(_hVencVDPCtrl);
}

void vDrvVencVDPSetParam(VENC_PROFILE_T eProfileDef, UINT32 u4RecWidth, UINT32 u4RecHeight)
{
    APP_RES_T rVencRes;

    LOG(1," vDrvVencVDPSetParam \n");
    VERIFY (x_sema_lock(_hVencVDPCtrl, X_SEMA_OPTION_WAIT) == OSR_OK);

    tVencInfo.eProfileDef  = eProfileDef;
    tVencInfo.u4TrgtWidth  = u4RecWidth;
    tVencInfo.u4TrgtHeight = u4RecHeight;
    
    vDrvVencCtrlConfig(tVencInfo.eProfileDef);
    vDrvVrmSetAppFlag(VRM_APP_VIDEO_ENC);

    // w/h = 0  is a special case. it means it wants to record the input size
    rVencRes.u4Width  = tVencInfo.u4TrgtWidth;
    rVencRes.u4Height = tVencInfo.u4TrgtHeight;
    vDrvVrmSetAppRes(VRM_APP_VIDEO_ENC, rVencRes);

    LOG(1,"[VDP_VENC]Start to wait for VRM\n\n");
    VERIFY (x_sema_lock(_hVencVDPWaitForVRM, X_SEMA_OPTION_WAIT) == OSR_OK);
    vDrvVencVDPEnableWaitForVRM(TRUE);    
    //wait for vrm ready
    VERIFY (x_sema_lock(_hVencVDPWaitForVRM, X_SEMA_OPTION_WAIT) == OSR_OK);
    x_sema_unlock(_hVencVDPWaitForVRM);
    LOG(1,"[VDP_VENC]Finish waiting \n\n");    
    
    vDrvVencCalVencInRes();
    vDrvVencSetFldMode();

    x_sema_unlock(_hVencVDPCtrl);
}

/**
 * @brief  Check whether the frame rate is feasible for the write engine
 * @param  u4APFR  the frame rate AP sets
 * @param  u4InFR  video input frame/field rate
 * @return feasible frame rate for AP
 */
UINT32 u4DrvVencVDPCtrlCheckFrameRate(UINT32 u4APFR, UINT32 u4InFR)
{
    if (u4APFR != u4InFR && u4APFR != u4InFR/2)
    {
        LOG(1,"[VDP_VENC]- AP FR %d/Input FR %d , AP frame rate is not feasible (DI write) \n", 
            u4APFR, u4InFR);
        return u4InFR;
    }
    else if (bDrvVencVDPIsOneFldMode() && tVencRes.fgInterlace)
    {
        LOG(1,"[VDP_VENC]- Interlace One Fld mode, doesn't support AP frame rate, use input frame rate \n");
        return u4InFR;
    }
    else 
    {
        LOG(1,"[VDP_VENC]- AP FR %d/Input FR %d, AP frame rate is feasible \n", u4APFR, u4InFR);
        return u4APFR;
    }
}

void vDrvVencVDPSetEncFldType(E_VDP_VENC_FIELD_MODE eEncFldType)
{
    tVencInfo.eEncFldType = eEncFldType;
}

void vDrvVencVDPSetEncModule(E_VDP_VENC_MODULE eEncModule)
{
    tVencInfo.eEncModule = eEncModule;
}

void vDrvVencVDPSetEncInput(E_VDP_VENC_INPUT eEncInput)
{
    tVencInfo.eEncInput = eEncInput;
}

void vDrvVencVDPGetEncInRes(VDP_VENC_INPUT_RES_t* tRes)
{
    *tRes = tVencRes;
}

E_VDP_VENC_MODULE eDrvVencVDPGetEncModule()
{
    return tVencInfo.eEncModule;
}
E_VDP_VENC_INPUT eDrvVencVDPGetEncInput()
{
    return tVencInfo.eEncInput;
}

void vDrvVencVDPInit()
{
    static UINT8 fgIsInit = FALSE;

    if (!fgIsInit)
    {
        VERIFY(x_sema_create(&_hVencVDPCtrl      , X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        VERIFY(x_sema_create(&_hVencVDPWaitForVRM, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        u1WaitForVRM = FALSE;
        tVencInfo.eProfileDef = VENC_PROFILE_MSI_OSTG;
        tVencInfo.eEncFldType = E_VDP_VENC_BOTH_FLD;
        tVencInfo.eEncModule  = E_VDP_VENC_SUB_DI;
        tVencInfo.eEncInput   = E_VDP_VENC_SUB_BEFORE_DI;
        fgIsInit = TRUE;
    }    
}

void vDrvVencVDPCtrlMLoop()
{

    if (u1DrvVencVDPIsWaitingForVRM())
    {
        LOG(2," Wait For VRM, Signal %d  Vrm %d \n\n", u1DrvVencVDPSignalReady(), u1DrvVencVDPVRMReady());
        if (u1DrvVencVDPSignalReady() && u1DrvVencVDPVRMReady())
        {
            // unlock semaphore
            vDrvVencVDPEnableWaitForVRM(FALSE);
            x_sema_unlock(_hVencVDPWaitForVRM);
        }   
    }
    
}

#endif

