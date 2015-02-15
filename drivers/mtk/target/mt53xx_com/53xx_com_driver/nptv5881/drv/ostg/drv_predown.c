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
 /*************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: drv_predown.c $
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *
 *
 * Author:
 * -------
 *
 *
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Date: 2015/02/15 $
 *
 * $Revision: #1 $
*************************************************************************/
#include "hw_predown.h"
#include "drv_predown.h"
#include "drv_scaler_drvif.h"
#include "scpos_debug.h"
#include "drv_lbox.h"
#include "mute_if.h"
#include "hw_sw.h"
#include "srm_drvif.h"
#include "fbm_drvif.h"
#include "drv_display.h"


PREDOWN_INFO_T _pdsInfo[VDP_MAX_NS];
static void _vPDSUpdateReg(UINT8 bPath);
static void _vPDSUpdateInOutPrm(UINT8 bPath);
static void _vPDSUpdateInputResolution(UINT8 bPath, UINT16 inW, UINT16 inH);
static void _vPDSUpdateOutputResolution(UINT8 bPath, UINT16 outW, UINT16 outH);
static void _vPDSUpdateHPDSFactorPrm(UINT8 bPath, UINT16 inW, UINT16 outW);
static void _vPDSUpdateVPDSFactorPrm(UINT8 bPath, UINT16 inH, UINT16 outH);
static void _vPDSUpdate3DModePrm(UINT8 bPath);
static void _vPDSUpdateVInitPhasePrm(UINT8 bPath);

void vPDSSwInit(UINT8 bPath)
{
    //sw init
    vPDSSetHPDSFactor(bPath, DFT_HPD_FACTOR);
    vPDSSetVPDSFactor(bPath, DFT_HPD_FACTOR);
}

void vPDSHwInit(UINT8 bPath)
{
    //hw init
    vPDSSetHPSFactorReg(getScalerVDP(bPath), DFT_HPD_FACTOR);
    vPDSSetVPSFactorReg(getScalerVDP(bPath), DFT_HPD_FACTOR);
    if (bPath == VDP_1)
    {
        vPDSSetVCenActAtSync(bPath, 0);
    }
    else
    {
        vPDSSetVCenActAtSync(bPath, 1);
    }

    vPDSImportEn(bPath, SV_ON);
}

void vPDSImportEn(UINT8 bPath, UINT8 bOnOff)
{
	if(bPath==SV_VP_MAIN)
	{
   	 	PDS_WRITE_FLD(PRE_DS_MAIN_05, (bOnOff==1)?1:0, PRE_DS_MAIN_05_PREDS_MAIN_SETRES_EN);
	}
	else
	{
   	 	PDS_WRITE_FLD(PRE_DS_PIP_04, (bOnOff==1)?1:0, PRE_DS_PIP_04_PREDS_PIP_SETRES_EN);

	}
}

void vPDSImportTrigger(UINT8 bPath)
{
	if(bPath==SV_VP_MAIN)
	{
   		PDS_WRITE_FLD(PRE_DS_MAIN_05, 1, PRE_DS_MAIN_05_PREDS_MAIN_SETRES);
	}
	else
	{
		PDS_WRITE_FLD(PRE_DS_PIP_04, 1, PRE_DS_PIP_04_PREDS_PIP_SETRES);
	}
}

void vPDSUpdateMddiFifo(UINT8 bPath)
{
    UINT32 u4MddiFifoMin;
    UINT32 u4MddiFifoMax;
    UINT32 u4VdpId;
    PREDOWN_INFO_T* pdsInfo;
    SCALER_FBM_POOL_T* fbmInfo;
    
    u4VdpId = getScalerVDP(bPath);
    pdsInfo = getPDSInfo(bPath);
    fbmInfo = getFbmPoolInfo(u4VdpId);

    // Check NR/PSCAN fifo limit
    // u4MddiDramWidth was updated in vDrvScpipSrmDispCb()
    if (fbmInfo->u4MddiDramWidth != 0)
    {
        u4MddiFifoMin = u4DrvDIMiniInputWidth(u4VdpId);
        u4MddiFifoMax = u4DrvDIMaxInputWidth(u4VdpId);
        u4MddiFifoMax = MIN(MIN(u4DrvDIMaxInputWidth(u4VdpId), fbmInfo->u4MddiDramWidth), u4VrmGetDIMaxOutWidth(bPath));       
        LOG(3, "DI Max Input Width (%d) DI DRAM Limit (%d)\n",u4DrvDIMaxInputWidth(u4VdpId), fbmInfo->u4MddiDramWidth);
    }
    else
    {                
        u4MddiFifoMin = 0;
        u4MddiFifoMax = 2000;
    }      
    LOG(3, "DI Fifo(%d) range (%d - %d)\n", u4VdpId, u4MddiFifoMin, u4MddiFifoMax);

    pdsInfo->u4MddiFifoMax = u4MddiFifoMax;
    pdsInfo->u4MddiFifoMin = u4MddiFifoMin;
}

UINT32 u4PDSUpdatePDSWidth(UINT8 bPath)
{
    UINT32 ret;
    RDispInfo* dispInfo;
    RPicInfo* picInfo;    
    VRM_INFO_T* inRes;
    UINT16 u2TargetWInPDS;
    UINT32 u4MddiFifoMin;
    UINT32 u4MddiFifoMax;
    UINT32 u4Bound;
    UINT32 u4Factor;
    UINT32 u4VdpId;
    PREDOWN_INFO_T* pdsInfo;
    SCALER_FBM_POOL_T* fbmInfo= getFbmPoolInfo(bPath);
    UINT8 bIsFifoDramLimit;
    picInfo = getPicInfo(bPath);
    if((bPath == SV_VP_PIP) && _u4ForceDispOff[bPath])
    {
        dispInfo = getTveDispInfo();
    }
    else
    {
        dispInfo = getDispInfo(bPath);
    }
    pdsInfo = getPDSInfo(bPath);
    u4VdpId = getScalerVDP(bPath);
    
    u4MddiFifoMin = pdsInfo->u4MddiFifoMin;
    u4MddiFifoMax = pdsInfo->u4MddiFifoMax;    
    inRes = VRMGetVRMInfo(bPath, VRM_MODULE_PDS);
    ret = 0;    
    //error handle
    if(inRes == NULL)
    {
        return ret;
    }
    //error handle
    if(picInfo->wSrcW ==0 || picInfo->wTargetW == 0 || dispInfo->wWidth == 0 || inRes->u2Width == 0)
    {
        return ret;
    }

    // whether there is fifo dram limit
    bIsFifoDramLimit = bDrvVrmIsAppFifoDramLimit(bPath);
    
    //give default value
    ret = inRes->u2Width;

    // 1. forced pre-down factor
    if (pdsInfo->u4ForcedHPDS != 0)
    {
        ret = GET_PDS_SIZE(inRes->u2Width, pdsInfo->u4ForcedHPDS);
        LOG(3, "Force HPDS to (0x%x), Width after predown(%d)\n", pdsInfo->u4ForcedHPDS,ret);
        return ret;
    }
    #ifdef CC_SUPPORT_PRESCALE_DOWN
    // 2. oversample case, pre-down to half size
    else if (( bGetVideoDecType(bPath) == (UINT8) SV_VD_YPBPR || bGetVideoDecType(bPath) == (UINT8) SV_VD_DVI) && 
            bDrvVideoGetWidthOverSample(u4VdpId))
    {
        // 1440 >> 720
        ret = inRes->u2Width >> 1;
        LOG(3,"Oversample case, set predown width to %d\n", ret);
        return ret;
    }
    #endif
        
    //if don't do predown, the width after overscan would be like this.
    u2TargetWInPDS = (inRes->u2Width * picInfo->wTargetW) / picInfo->wSrcW;
    if (bIsFifoDramLimit)
    {
        u4Bound = MIN(dispInfo->wWidth, u4MddiFifoMax);
    }
    else
    {
        u4Bound = dispInfo->wWidth;    
    }    
    LOG(3,"dispInfo->wWidth(%d) u4MddiFifoMax(%d) u4Bound(%d)\n", dispInfo->wWidth, u4MddiFifoMax, u4Bound);
    if(IS_COBRA_E() || IS_VIPER_E())
    {
        #if defined(SCALER_WA1_DRAM_WIDTH)
        if(u4FixedWidth[bPath] != 0 && getScalerInfo(bPath)->u4IsDramFrontMode == SV_TRUE)
        {
            u4Bound = u4Bound - u4FixedWidth[bPath];        
            LOG(3,"fixed u4Bound(%d) u4FixedWidth(%d)\n",u4Bound, u4FixedWidth[bPath]);
        }
        #endif
    }   
    //predown will be bounded to MIN(dispInfo->wWidth, u4MddiFifoMax)
    if(u4Bound >= u2TargetWInPDS)   //this means both MIB fifo length and output width is larger than regen's output width.No need to do predown.
    {
        LOG(3,"u4Bound(%d) >= u2TargetWInPDS(%d). No H Predown.\n", u4Bound ,u2TargetWInPDS);
        ret = inRes->u2Width;
    }
    else
    {
        u4Factor = (DFT_HPD_FACTOR * MAX(u4Bound,u4MddiFifoMin)) / u2TargetWInPDS;
        ret = GET_PDS_SIZE(inRes->u2Width, u4Factor);
        LOG(3,"u4Bound(%d), u4MddiFifoMin(%d).\n", u4Bound , u4MddiFifoMin);
        LOG(3,"u4HPDSFactor=(0x%x*%d/%d)=(%x)\n",DFT_HPD_FACTOR, MAX(u4Bound,u4MddiFifoMin),u2TargetWInPDS,u4Factor);
    }       
        //when scalre  enter Dram mode , FBM allocation size(width) < fact size (width),   need to down to FBM allcocation size
    if((SRM_GetScposMode(bPath) & FBM_POOL_MODE_SCPOS_FULL) && bIsFifoDramLimit &&
        ((SRM_IsMainVideoStrictStable() && (bPath == VDP_1)) || (SRM_IsSubVideoStrictStable() && (bPath == VDP_2))))
    {
        ret =  MIN(ret, fbmInfo->u4Width);
     }
    return ret;   
}

UINT32 u4PDSUpdatePDSHeight(UINT8 bPath)
{
    UINT32 ret;
    RDispInfo* dispInfo;
    RPicInfo* picInfo;    
    VRM_INFO_T* inRes;
    UINT16 u2TargetHInPDS;
    UINT32 u4Bound;
    UINT32 u4Factor;    
    PREDOWN_INFO_T* pdsInfo;
    UINT32 u4VDownTime;

    UINT32 u4MIB     = u4DrvTDTVDIModeQuery();
    E_TD_IN eMIBIn   = TD_MIB_IN(u4MIB);
    E_TD_OUT eMIBOUT = TD_MIB_OUT(u4MIB);
    pdsInfo = getPDSInfo(bPath);
    picInfo = getPicInfo(bPath);
    if((bPath == SV_VP_PIP) && _u4ForceDispOff[bPath])
    {
        dispInfo = getTveDispInfo();
    }
    else
    {
        dispInfo = getDispInfo(bPath);
    }
    inRes = VRMGetVRMInfo(bPath, VRM_MODULE_PDS);
    u4Factor = DFT_HPD_FACTOR;
    ret = 0;    
    //error handle
    if(inRes == NULL)
    {
        return ret;
    }
    
    if(picInfo->wSrcH ==0 || picInfo->wTargetH == 0 || dispInfo->wHeight == 0 || inRes->u2Height == 0)
    {
        return ret;
    }

    //give default return value
    ret = inRes->u2Height;

    // 1. forced pre-down factor
    if (pdsInfo->u4ForcedVPDS != 0)
    {
        ret = GET_PDS_SIZE(inRes->u2Height, pdsInfo->u4ForcedVPDS);
        LOG(1, "Force VPDS to 0x%x, height after predown(%d)\n", pdsInfo->u4ForcedVPDS,ret);
        return ret;
    }
    
    //if don't do predown, the height after overscan would be like this.
    u2TargetHInPDS = (inRes->u2Height * picInfo->wTargetH) / picInfo->wSrcH;
    u4Bound = dispInfo->wHeight;
    LOG(3,"dispInfo->wHeight(%d) u4Bound(%d)\n", dispInfo->wHeight, u4Bound);
    
    //predown will be bounded to MIN(dispInfo->wWidth, u4MddiFifoMax)
    if(u4Bound >= u2TargetHInPDS)   //this means both MIB fifo length and output width is larger than regen's output width.No need to do predown.
    {
        LOG(3,"u4Bound(%d) >= u2TargetHInPDS(%d). No V Predown.\n", u4Bound ,u2TargetHInPDS);
        //u4Factor = DFT_HPD_FACTOR;
        ret = inRes->u2Height;
    }
    else
    {
        if(bDrvVideoIsSrcInterlace(bPath) && 
            !(bDrvVrmIsAppFlagSet(VRM_APP_VIDEO_ENC) || bDrvVrmIsAppFlagSet(VRM_APP_GRAPHIC_DUMP))) //only venc alone can do predown + graphic dump
        {
            u4VDownTime = u2TargetHInPDS / u4Bound;
            if(u4VDownTime < 2)
            {
                //no predown
                ret = inRes->u2Height;
            }
            else if(u4VDownTime < 4)
            {
                //down to 1/2
                ret = inRes->u2Height >> 1;
            }
            else
            {
                //down to 1/4
                ret = inRes->u2Height >> 2;
            }
        }
        else if((E_TD_IN_LI_P == eMIBIn )&& (E_TD_OUT_3D_FS==eMIBOUT))
        {
            ret = inRes->u2Height;
            LOG(3,"MIB input LI and output FS, v pre-down done in FSC\n");
        }
        else
        {
            u4Factor = (DFT_HPD_FACTOR * u4Bound) / u2TargetHInPDS;
            ret = GET_PDS_SIZE(inRes->u2Height, u4Factor);
        }
        LOG(3,"u4Bound(%d).\n", u4Bound);
        LOG(3,"u4VPDSFactor=(0x%x*%d/%d)=(%x)\n",DFT_HPD_FACTOR, u4Bound,u2TargetHInPDS,u4Factor);
    }

    return ret;
}

#if 0
//update H predown parameter
void vPDSUpdateHPDSInfo(UINT8 bPath)
{
    RDispInfo* dispInfo;
    RPicInfo* picInfo;    
    VRM_INFO_T* inRes;
    UINT16 u2TargetWInPDS;
    UINT32 u4MddiFifoMin;
    UINT32 u4MddiFifoMax;
    UINT32 u4Bound;
    UINT32 u4Factor;
    UINT32 u4VdpId;
    PREDOWN_INFO_T* pdsInfo;

    picInfo = getPicInfo(bPath);
    dispInfo = getDispInfo(bPath);
    pdsInfo = getPDSInfo(bPath);
    u4VdpId = getScalerVDP(bPath);
    
    u4MddiFifoMin = pdsInfo->u4MddiFifoMin;
    u4MddiFifoMax = pdsInfo->u4MddiFifoMax;    
    inRes = VRMGetVRMInfo(bPath, VRM_MODULE_PDS);
    u4Factor = DFT_HPD_FACTOR;

    //error handle
    if(inRes == NULL)
    {
        return;
    }
    
    if(picInfo->wSrcW ==0 || picInfo->wTargetW == 0 || dispInfo->wWidth == 0 || inRes->u2Width == 0)
    {
        return;
    }

    // 1. forced pre-down factor
    if (pdsInfo->u4ForcedHPDS != 0)
    {
        pdsInfo->u4HPDSFactor = pdsInfo->u4ForcedHPDS;
        LOG(1, "Force HPDS to 0x%x\n", pdsInfo->u4HPDSFactor);
        return;
    }
    #ifdef CC_SUPPORT_PRESCALE_DOWN
    // 2. oversample case, pre-down to half size
    else if (( bGetVideoDecType(bPath) == (UINT8) SV_VD_YPBPR || bGetVideoDecType(bPath) == (UINT8) SV_VD_DVI) && 
            bDrvVideoGetWidthOverSample(u4VdpId))
    {
        // 1440 >> 720
        pdsInfo->u4HPDSFactor = (DFT_HPD_FACTOR >> 1);
        LOG(1,"Oversample case, set HPDS to 0x%x\n", pdsInfo->u4HPDSFactor);
        return;
    }
    #endif    
        
    //if don't do predown, the width after overscan would be like this.
    u2TargetWInPDS = (inRes->u2Width * picInfo->wTargetW) / picInfo->wSrcW; 
    u4Bound = MIN(dispInfo->wWidth, u4MddiFifoMax);
    LOG(3,"dispInfo->wWidth(%d) u4MddiFifoMax(%d) u4Bound(%d)\n", dispInfo->wWidth, u4MddiFifoMax, u4Bound);
    #if defined(SCALER_WA1_DRAM_WIDTH)
    if(u4FixedWidth[bPath] != 0 && getScalerInfo(bPath)->u4IsDramFrontMode == SV_TRUE)
    {
        u4Bound = u4Bound - u4FixedWidth[bPath];        
        LOG(3,"fixed u4Bound(%d) u4FixedWidth(%d)\n",u4Bound, u4FixedWidth[bPath]);
    }
    #endif
        
    //predown will be bounded to MIN(dispInfo->wWidth, u4MddiFifoMax)
    if(u4Bound >= u2TargetWInPDS)   //this means both MIB fifo length and output width is larger than regen's output width.No need to do predown.
    {
        LOG(3,"u4Bound(%d) >= u2TargetWInPDS(%d). No H Predown.\n", u4Bound ,u2TargetWInPDS);
        u4Factor = DFT_HPD_FACTOR;
    }
    else
    {
        u4Factor = (DFT_HPD_FACTOR * MAX(u4Bound,u4MddiFifoMin)) / u2TargetWInPDS;
        LOG(3,"u4Bound(%d), u4MddiFifoMin(%d).\n", u4Bound , u4MddiFifoMin);
        LOG(3,"u4HPDSFactor=(0x%x*%d/%d)=(%x)\n",DFT_HPD_FACTOR, MAX(u4Bound,u4MddiFifoMin),u2TargetWInPDS,u4Factor);
    }       

    vPDSSetHPDSFactor(bPath, u4Factor);
}

//update V predown parameter
void vPDSUpdateVPDSInfo(UINT8 bPath)
{
    RDispInfo* dispInfo;
    RPicInfo* picInfo;    
    VRM_INFO_T* inRes;
    UINT16 u2TargetHInPDS;
    UINT32 u4Bound;
    UINT32 u4Factor;    
    PREDOWN_INFO_T* pdsInfo;
    UINT32 u4VDownTime;

    pdsInfo = getPDSInfo(bPath);
    picInfo = getPicInfo(bPath);
    dispInfo = getDispInfo(bPath);    
    inRes = VRMGetVRMInfo(bPath, VRM_MODULE_PDS);
    u4Factor = DFT_HPD_FACTOR;
    
    //error handle
    if(inRes == NULL)
    {
        return;
    }
    
    if(picInfo->wSrcH ==0 || picInfo->wTargetH == 0 || dispInfo->wHeight == 0 || inRes->u2Height == 0)
    {
        return;
    }

    // 1. forced pre-down factor
    if (pdsInfo->u4ForcedVPDS != 0)
    {
        pdsInfo->u4VPDSFactor = pdsInfo->u4ForcedVPDS;
        LOG(1, "Force VPDS to 0x%x\n", pdsInfo->u4VPDSFactor);
        return;
    }
    
    //if don't do predown, the height after overscan would be like this.
    u2TargetHInPDS = (inRes->u2Height * picInfo->wTargetH) / picInfo->wSrcH; 
    u4Bound = dispInfo->wHeight;
    LOG(3,"dispInfo->wHeight(%d) u4Bound(%d)\n", dispInfo->wHeight, u4Bound);
    
    //predown will be bounded to MIN(dispInfo->wWidth, u4MddiFifoMax)
    if(u4Bound >= u2TargetHInPDS)   //this means both MIB fifo length and output width is larger than regen's output width.No need to do predown.
    {
        LOG(3,"u4Bound(%d) >= u2TargetHInPDS(%d). No V Predown.\n", u4Bound ,u2TargetHInPDS);
        u4Factor = DFT_HPD_FACTOR;
    }
    else
    {
        if(bDrvVideoIsSrcInterlace(bPath))
        {
            u4VDownTime = u2TargetHInPDS / u4Bound;
            if(u4VDownTime < 2)
            {
                u4Factor = DFT_HPD_FACTOR ;
            }
            else if(u4VDownTime < 4)
            {
                u4Factor = DFT_HPD_FACTOR >> 1;
            }
            else
            {
                u4Factor = DFT_HPD_FACTOR >> 2;
            }
        }
        else
        {
            u4Factor = (DFT_HPD_FACTOR * u4Bound) / u2TargetHInPDS;
        }
        LOG(3,"u4Bound(%d).\n", u4Bound);
        LOG(3,"u4VPDSFactor=(0x%x*%d/%d)=(%x)\n",DFT_HPD_FACTOR, u4Bound,u2TargetHInPDS,u4Factor);
    }

    vPDSSetVPDSFactor(bPath, u4Factor);
}
#endif

UINT32 u4PDSGetHPDSFactor(UINT8 bPath)
{
    if(bPath > SV_VP_PIP)
    {
        return DFT_HPD_FACTOR;
    }

    return _pdsInfo[bPath].u4HPDSFactor;
}

UINT32 u4PDSGetVPDSFactor(UINT8 bPath)
{
    if(bPath > SV_VP_PIP)
    {
        return DFT_HPD_FACTOR;
    }

    return _pdsInfo[bPath].u4VPDSFactor;
}

void vPDSSetHPDSFactor(UINT8 bPath, UINT32 factor)
{
    if(bPath > SV_VP_PIP)
    {
        return;
    }

    _pdsInfo[bPath].u4HPDSFactor = factor;
}

void vPDSSetVPDSFactor(UINT8 bPath, UINT32 factor)
{
    if(bPath > SV_VP_PIP)
    {
        return;
    }

    _pdsInfo[bPath].u4VPDSFactor = factor;

}

/**
* @brief Get H pre-scaling down factor
* @param bPath VDP_1/VDP_2
* @return pre-scaling down factor
* @cobra porting done
*/
UINT32 u4PDSGetHPSFactorReg(UINT8 bPath)
{
    return PDS_READ_FLD(PRE_DS_MAIN_00 + getPDSOffset(bPath), PRE_DS_MAIN_00_PRE_DSCALE_H_1);
}
/**
* @brief Get V pre-scaling down factor
* @param bPath VDP_1/VDP_2
* @return pre-scaling down factor
* @6896 porting done
*/
UINT32 u4PDSGetVPSFactorReg(UINT8 bPath)
{
    return PDS_READ_FLD(PRE_DS_MAIN_00 + getPDSOffset(bPath), PRE_DS_MAIN_00_PRE_DSCALER_V_1);
}
/**
* @brief Set pre-scaling down factor
* @param u4VdpId VDP_1/VDP_2
*/
void vPDSSetHPSFactorReg(UINT32 u4VdpId, UINT32 u4Factor)
{        
    UINT32 u4HpsFactor; 
    UINT32 u4GetHwHps;

    u4GetHwHps = u4PDSGetHPSFactorReg(VDP_1);
    // defaul value:0x8000 means auto init phase, no need to set the phase again.
    //u4HInitValue = DFT_HPD_FACTOR - u4Factor;
    u4HpsFactor = u4Factor;
    PDS_WRITE_FLD(PRE_DS_MAIN_00 + getPDSOffset(getScalerPath(u4VdpId)), (UINT16)u4HpsFactor, PRE_DS_MAIN_00_PRE_DSCALE_H_1);
    PDS_WRITE_FLD(PRE_DS_MAIN_01 + getPDSOffset(getScalerPath(u4VdpId)), (UINT16)(0x8000-u4HpsFactor), PRE_DS_MAIN_01_PRE_DSCALE_H_INIT_1);
    if(u4VdpId == VDP_1)
    {
        PDS_WRITE_FLD(PRE_DS_MAIN_02 , (UINT16)(0x8000-u4HpsFactor), PRE_DS_MAIN_02_PRE_DSCALE_H_INIT_RIGHT_1);    
    }
    if(u4HpsFactor != u4GetHwHps)
    {      	 	
    	LboxSourceChange();	 
    }
}


/**
* @brief Set pre-scaling down factor
* @param u4VdpId VDP_1/VDP_2
*/
void vPDSSetVPSFactorReg(UINT32 u4VdpId, UINT32 u4Factor)
{        
    UINT32 u4VpsFactor; 

    // defaul value:0x8000 means auto init phase, no need to set the phase again.
    //u4VInitValue = DFT_HPD_FACTOR - u4Factor;
    u4VpsFactor = u4Factor;
    PDS_WRITE_FLD(PRE_DS_MAIN_00 + getPDSOffset(getScalerPath(u4VdpId)), (UINT16)u4VpsFactor, PRE_DS_MAIN_00_PRE_DSCALER_V_1);
    PDS_WRITE_FLD(PRE_DS_MAIN_01 + getPDSOffset(getScalerPath(u4VdpId)), (UINT16)(0x8000-u4VpsFactor), PRE_DS_MAIN_01_PRE_DSCALE_V_INIT_1);
    if(u4VdpId == VDP_1)
    {
        PDS_WRITE_FLD(PRE_DS_MAIN_02 , (UINT16)(0x8000-u4VpsFactor), PRE_DS_MAIN_02_PRE_DSCALE_V_INIT_RIGHT_1);    
    }
}

void vPDSSet3DModeReg(UINT32 u4VdpId, UINT32 u4Value)
{
    if(u4VdpId != VDP_1)
    {
        return;
    }

    PDS_WRITE_FLD(PRE_DS_MAIN_05, u4Value, PRE_DS_MAIN_05_PREDS_3DTV_MODE_1);    
}

/**
* @brief Set pre-scaling input resolution
* @param u4VdpId VDP_1/VDP_2
*/
void vPDSSetInputResolutionReg(UINT32 u4VdpId, UINT32 u4InW, UINT32 u4InH)
{        
	UINT8 bPath;
	bPath = getScalerPath(u4VdpId);
	if(bPath == SV_VP_MAIN)
	{
	    PDS_WRITE_FLD(PRE_DS_MAIN_04, (UINT16)u4InW, PRE_DS_MAIN_04_PREDS_MAIN_IN_H);
	    PDS_WRITE_FLD(PRE_DS_MAIN_04, (UINT16)u4InH, PRE_DS_MAIN_04_PREDS_MAIN_IN_V);
	}
	else
	{
	    PDS_WRITE_FLD(PRE_DS_PIP_03, (UINT16)u4InW, PRE_DS_PIP_03_PREDS_PIP_IN_H);
	    PDS_WRITE_FLD(PRE_DS_PIP_03, (UINT16)u4InH, PRE_DS_PIP_03_PREDS_PIP_IN_V);
    }
}

/**
* @brief Set pre-scaling output resolution
* @param u4VdpId VDP_1/VDP_2
*/
void vPDSSetOutputResolutionReg(UINT32 u4VdpId, UINT32 u4InW, UINT32 u4InH)
{        
    UINT8 bPath;
	bPath = getScalerPath(u4VdpId);
	if(bPath == SV_VP_MAIN)
	{
        PDS_WRITE_FLD(PRE_DS_MAIN_03, (UINT16)u4InW, PRE_DS_MAIN_03_PREDS_MAIN_OUT_H);
        PDS_WRITE_FLD(PRE_DS_MAIN_03, (UINT16)u4InH, PRE_DS_MAIN_03_PREDS_MAIN_OUT_V);
	}
	else
	{
	    PDS_WRITE_FLD(PRE_DS_PIP_02, (UINT16)u4InW, PRE_DS_PIP_02_PREDS_PIP_OUT_H);
	    PDS_WRITE_FLD(PRE_DS_PIP_02, (UINT16)u4InH, PRE_DS_PIP_02_PREDS_PIP_OUT_V);
	}
}

void vPDSSetVCenActAtSync(UINT8 bPath, UINT8 value)
{
    (bPath==SV_VP_MAIN)?
        PDS_WRITE_FLD(PRE_DS_MAIN_05, value, PRE_DS_MAIN_05_MAIN_VCEN_ACT_AT_SYNC):
        PDS_WRITE_FLD(PRE_DS_PIP_04, value, PRE_DS_PIP_04_PIP_VCEN_ACT_AT_SYNC);
}

UINT32 u4PDSGetHActive(UINT8 bPath, UINT8 bInOut)
{
    (bPath==SV_VP_MAIN)?
        PDS_WRITE_FLD(PRE_DS_MAIN_05, (bInOut==1)?1:0, PRE_DS_MAIN_05_MAIN_SEL_HVCNT):
        PDS_WRITE_FLD(PRE_DS_PIP_04,  (bInOut==1)?1:0, PRE_DS_PIP_04_PIP_SEL_HVCNT);
    return PDS_READ_FLD(PRE_DS_MAIN_06 + getPDSOffset(bPath), PRE_DS_MAIN_06_MAIN_H_ACTIVE);
}

UINT32 u4PDSGetHTotal(UINT8 bPath, UINT8 bInOut)
{
    (bPath==SV_VP_MAIN)?
        PDS_WRITE_FLD(PRE_DS_MAIN_05, (bInOut==1)?1:0, PRE_DS_MAIN_05_MAIN_SEL_HVCNT):
        PDS_WRITE_FLD(PRE_DS_PIP_04,  (bInOut==1)?1:0, PRE_DS_PIP_04_PIP_SEL_HVCNT);
    return PDS_READ_FLD(PRE_DS_MAIN_06 + getPDSOffset(bPath), PRE_DS_MAIN_06_MAIN_H_TOTAL);
}

UINT32 u4PDSGetVActive(UINT8 bPath, UINT8 bInOut)
{
    (bPath==SV_VP_MAIN)?
        PDS_WRITE_FLD(PRE_DS_MAIN_05, (bInOut==1)?1:0, PRE_DS_MAIN_05_MAIN_SEL_HVCNT):
        PDS_WRITE_FLD(PRE_DS_PIP_04,  (bInOut==1)?1:0, PRE_DS_PIP_04_PIP_SEL_HVCNT);
    return PDS_READ_FLD(PRE_DS_MAIN_07 + getPDSOffset(bPath), PRE_DS_MAIN_07_MAIN_V_ACTIVE);
}

UINT32 u4PDSGetVTotal(UINT8 bPath, UINT8 bInOut)
{
    (bPath==SV_VP_MAIN)?
        PDS_WRITE_FLD(PRE_DS_MAIN_05, (bInOut==1)?1:0, PRE_DS_MAIN_05_MAIN_SEL_HVCNT):
        PDS_WRITE_FLD(PRE_DS_PIP_04,  (bInOut==1)?1:0, PRE_DS_PIP_04_PIP_SEL_HVCNT);
    return PDS_READ_FLD(PRE_DS_MAIN_07 + getPDSOffset(bPath), PRE_DS_MAIN_07_MAIN_V_TOTAL);
}

UINT32 u4PDSGetInputWidthReg(UINT8 bPath)
{
    return ((bPath==SV_VP_MAIN)?
            PDS_READ_FLD(PRE_DS_MAIN_04, PRE_DS_MAIN_04_PREDS_MAIN_IN_H):
            PDS_READ_FLD(PRE_DS_PIP_03, PRE_DS_PIP_03_PREDS_PIP_IN_H));
}

UINT32 u4PDSGetInputHeightReg(UINT8 bPath)
{
    return ((bPath==SV_VP_MAIN)?
           PDS_READ_FLD(PRE_DS_MAIN_04, PRE_DS_MAIN_04_PREDS_MAIN_IN_V):
           PDS_READ_FLD(PRE_DS_PIP_03, PRE_DS_PIP_03_PREDS_PIP_IN_V));
}

UINT32 u4PDSGetOutputWidthReg(UINT8 bPath)
{
    return ((bPath==SV_VP_MAIN)?
           PDS_READ_FLD(PRE_DS_MAIN_03, PRE_DS_MAIN_03_PREDS_MAIN_OUT_H):
           PDS_READ_FLD(PRE_DS_PIP_02, PRE_DS_PIP_02_PREDS_PIP_OUT_H));
}

UINT32 u4PDSGetOutputHeightReg(UINT8 bPath)
{
    return ((bPath==SV_VP_MAIN)?
           PDS_READ_FLD(PRE_DS_MAIN_03, PRE_DS_MAIN_03_PREDS_MAIN_OUT_V):
           PDS_READ_FLD(PRE_DS_PIP_02, PRE_DS_PIP_02_PREDS_PIP_OUT_V));
}

UINT8 fgPDSIsModeChanged(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    UINT8 ret;

    ret = SV_FALSE;
    pdsInfo = getPDSInfo(bPath);
    
    if(pdsInfo->bPDSModeChanged == SV_TRUE)
    {
        ret = SV_TRUE;
    }
    return ret;
}

void vPDSSetModeChanged(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    pdsInfo->bPDSModeChanged = SV_TRUE;    
}

void vPDSClearModeChanged(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    pdsInfo->bPDSModeChanged = SV_FALSE;
}

static void _vPDSUpdateInOutPrm(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    VRMGetModuleVRMInfo(bPath, VRM_MODULE_PDS, &(pdsInfo->pdsIn), &(pdsInfo->pdsOut));  
    if(vDrvGetImportTriggerModule(bPath) == IMPORT_MODULE_NONE)
    {
        if((pdsInfo->u4InputWidth!= pdsInfo->pdsIn.u2Width) || (pdsInfo->u4InputHeight!= pdsInfo->pdsIn.u2Height) || 
            (pdsInfo->u4OutputWidth!= pdsInfo->pdsOut.u2Width) || (pdsInfo->u4OutputHeight!= pdsInfo->pdsOut.u2Height))
            {
                LOG(1,"Predown change mute when no import\n");
                 _vDrvVideoSetMute(MUTE_MODULE_SCPOS_PRE_DOWN, bPath, 5, TRUE);
            }
    }
}

static void _vPDSUpdateInputResolution(UINT8 bPath, UINT16 inW, UINT16 inH)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    pdsInfo->u4InputWidth = inW;
    pdsInfo->u4InputHeight = inH;
}

static void _vPDSUpdateOutputResolution(UINT8 bPath, UINT16 outW, UINT16 outH)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    pdsInfo->u4OutputWidth = outW;
    pdsInfo->u4OutputHeight = outH;
}

static void _vPDSUpdateHPDSFactorPrm(UINT8 bPath, UINT16 inW, UINT16 outW)
{
    UINT32 u4Factor;
    
    if(inW <= outW)
    {
        u4Factor = DFT_PDS_FACTOR;
    }
    else
    {
        u4Factor = ((DFT_PDS_FACTOR * outW) / inW);
    }

    vPDSSetHPDSFactor(bPath, u4Factor);
}

static void _vPDSUpdateVPDSFactorPrm(UINT8 bPath, UINT16 inH, UINT16 outH)
{
    UINT32 u4Factor;
    
    if(inH <= outH)
    {
        u4Factor = DFT_PDS_FACTOR;
    }
    else
    {
        u4Factor = ((DFT_PDS_FACTOR * outH) / inH);
    }

    vPDSSetVPDSFactor(bPath, u4Factor);
}


static void _vPDSUpdate3DModePrm(UINT8 bPath)
{
    UINT32 u4TTTMode;
    E_TD_IN eTTDIn;
    UINT32 u4Value;
    PREDOWN_INFO_T* pdsInfo;
    
    pdsInfo = getPDSInfo(bPath);        
    u4TTTMode = u4DrvTDTVTTTModeQuery(bPath);
    eTTDIn=TD_TTT_IN(u4TTTMode);
    eTTDIn = (E_TD_IN)((UINT32)eTTDIn & ~0x1);  //make I/P to same value
    
    if(bPath != SV_VP_MAIN)
    {
        return;
    }

    switch(eTTDIn)
    {
        //SBS mode predown is not supported @ Cobra/Viper/Python
        //case E_TD_IN_SBS_I:
        //    u4Value = 0x8;
        //    break;            
        //TB mode predown is cause last line broken @ Cobra/Viper/Python        
        //case E_TD_IN_TB_I:
        //    u4Value = 0x4;
        //    break;
        case E_TD_IN_LI_I:
            u4Value = (u4PDSGetVPDSFactor(bPath) == DFT_HPD_FACTOR)? 0 : 0xC;
            break;
        default:
            u4Value = 0x0;
            break;    
    }

    pdsInfo->u43DMode = u4Value;
}

/**
 * @brief Set predown v init phase parameter by top/bottom field
 * @param bPath VDP_1/VDP_2  
 * @the parameters will set to register in DI input vsync ISR.
 */
static UINT32 u4Phase[2][2]={{0,0},{0,0}};
static void _vPDSUpdateVInitPhasePrm(UINT8 bPath)
{
    UINT32 u4PreDscaleV = 0;
    UINT32 i4PhaseShift = 0;
    PREDOWN_INFO_T* pdsInfo = getPDSInfo(bPath);
    if((bPath >= VDP_NS) || !bDrvVideoIsSrcInterlace(bPath) ||
        (pdsInfo->u4OutputHeight == 0) || (pdsInfo->u4InputHeight == 0))
    {
        return ;
    }
    u4PreDscaleV = (pdsInfo->u4OutputHeight<<15)/pdsInfo->u4InputHeight;
    i4PhaseShift = u4PreDscaleV  * ((pdsInfo->u4InputHeight - pdsInfo->u4OutputHeight)*100 /pdsInfo->u4OutputHeight)/400;

    #ifdef CC_FLIP_MIRROR_SUPPORT   
    if((u4GetFlipMirrorModule(VDP_1) == FLIP_BY_PSCAN)&&(u1GetFlipMirrorConfig() & SYS_FLIP_CONFIG_ON))
    {
        u4Phase[bPath][0] = (DFT_PDS_FACTOR - u4PreDscaleV - i4PhaseShift)%DFT_PDS_FACTOR;    
        u4Phase[bPath][1] = (DFT_PDS_FACTOR - u4PreDscaleV + i4PhaseShift)%DFT_PDS_FACTOR;
    }
    else
    #endif
    {
        u4Phase[bPath][1] = (DFT_PDS_FACTOR - u4PreDscaleV - i4PhaseShift)%DFT_PDS_FACTOR;    
        u4Phase[bPath][0] = (DFT_PDS_FACTOR - u4PreDscaleV + i4PhaseShift)%DFT_PDS_FACTOR;
    }

}

/**
 * @brief Set predown v init phase by top/bottom field
 * @param bPath VDP_1/VDP_2  
 * @param bIsFSInterlace current signal format is frame sequencial interlace or not
 * @param bIsLeftEye current disp_r is left 
 * @param bIsTopField current is top field
 * @it is called in DI input vsync ISR.
 */
void vPDSUpdateVInitPhaseReg(UINT8 bPath, UINT8 bIsFSInterlace , UINT8 bIsLeftEye , UINT8 bIsBottomField)
{
    //Do not update init phase in 2D condition to avoid conflicting with dynamic resolution.
    if((bPath >= VDP_NS) || !bDrvVideoIsSrcInterlace(bPath) || (TD_TTD_IN(u4DrvTDTVTTDModeQuery())  == E_TD_IN_2D_P))
    {
        return ;
    }

    //FS interlace sequence like :LT RT LB RB , if we set registers like: LB RT LT RB, then registers take effect like: LB RB LT RT  because of vsync latch.
    //By the quality of video, solution is  u4Phase[bPath][0] should apply in bottom field ,and u4Phase[bPath][1] should apply in top filed.
    if(bIsFSInterlace && bIsLeftEye)
    {   
        bIsBottomField = (~bIsBottomField)&0x1;
    }
    
    if((IO32ReadFldAlign(VPREDOWN_INIT_PHASE, INIT_PHASE_B) != 0) || (IO32ReadFldAlign(VPREDOWN_INIT_PHASE, INIT_PHASE_T) != 0))
    {
        PDS_WRITE_FLD(PRE_DS_MAIN_01 + getPDSOffset(getScalerPath(bPath)),(bIsBottomField & 0x1)? IO32ReadFldAlign(VPREDOWN_INIT_PHASE, INIT_PHASE_B):
                                                                IO32ReadFldAlign(VPREDOWN_INIT_PHASE, INIT_PHASE_T),PRE_DS_MAIN_01_PRE_DSCALE_V_INIT_1);
    }
    else
    {
        PDS_WRITE_FLD(PRE_DS_MAIN_01 + getPDSOffset(getScalerPath(bPath)),u4Phase[bPath][(bIsBottomField & 0x1)],PRE_DS_MAIN_01_PRE_DSCALE_V_INIT_1);
    }

}

#if 0
static BOOL bCheckHPDSError(UINT8 bPath)
{
    UINT32 u4GoodWidth, u4OutputWidth;
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    if(bDrvVideoGetRefreshRate(bPath) == 0) // no signal?
    {
        return FALSE;
    }

    u4GoodWidth   = (pdsInfo->u4InputWidth * pdsInfo->u4HPDSFactor) / DFT_PDS_FACTOR;
    u4OutputWidth = u4PDSGetHActive(bPath, 1);
    if ((u4GoodWidth > u4OutputWidth + 10) ||
        (u4GoodWidth + 10 < u4OutputWidth))
    {
        LOG(5, "Predown(%d) Width Error %d != %d\n", bPath, u4GoodWidth, u4OutputWidth);
        return TRUE;
    }
    else
    {
         return FALSE;
    }
}
#endif

void _vPDSUpdateDispPrm(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    //1. update in/out resolution from VRM
    _vPDSUpdateInOutPrm(bPath);
    //2. update input resolution to global variable
    _vPDSUpdateInputResolution(bPath,pdsInfo->pdsIn.u2Width, pdsInfo->pdsIn.u2Height);
    //3. update output resolution to global variable
    _vPDSUpdateOutputResolution(bPath, pdsInfo->pdsOut.u2Width, pdsInfo->pdsOut.u2Height);
    //4. update h pds factor
    _vPDSUpdateHPDSFactorPrm(bPath,pdsInfo->u4InputWidth, pdsInfo->u4OutputWidth);
    //5. update v pds factor
    _vPDSUpdateVPDSFactorPrm(bPath,pdsInfo->u4InputHeight, pdsInfo->u4OutputHeight);
    //6. update 3D mode
    _vPDSUpdate3DModePrm(bPath);
    //7.update vpredown init phase for top/bottom field
    _vPDSUpdateVInitPhasePrm(bPath);
}

static void _vPDSUpdateReg(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    //1. update input resolution
    vPDSSetInputResolutionReg(bPath,pdsInfo->u4InputWidth,pdsInfo->u4InputHeight);
    //2. update output resolution
    vPDSSetOutputResolutionReg(bPath,pdsInfo->u4OutputWidth, pdsInfo->u4OutputHeight);
    //3. update HPDS
    vPDSSetHPSFactorReg(bPath,pdsInfo->u4HPDSFactor);
    //4. update VPDS
    vPDSSetVPSFactorReg(bPath,pdsInfo->u4VPDSFactor);
    //5. update 3D mode
    vPDSSet3DModeReg(bPath, pdsInfo->u43DMode);
    //6. import
    if(vDrvGetImportTriggerModule(bPath) == IMPORT_MODULE_NONE)
    {
        vPDSImportTrigger(bPath);
    }    
}

void vPDSLoop(UINT8 bPath)
{
    PREDOWN_INFO_T* pdsInfo;
    pdsInfo = getPDSInfo(bPath);

    if(fgPDSIsModeChanged(bPath)== SV_TRUE)
    {
        _vPDSUpdateDispPrm(bPath);        

        if(pdsInfo->u4InputWidth == 0 || 
           pdsInfo->u4InputHeight == 0 ||
           pdsInfo->u4OutputWidth == 0 ||
           pdsInfo->u4OutputHeight == 0 || 
           pdsInfo->u4HPDSFactor == 0 ||
           pdsInfo->u4VPDSFactor == 0)
        {
            goto PREDOWN_UPDATE_FINISH;
        }
           
        _vPDSUpdateReg(bPath);
    }
    /*
    else if (bCheckHPDSError(bPath)) // error handling
    {
        vPDSImportTrigger(bPath);
    }
    */
PREDOWN_UPDATE_FINISH:    

    vDrvClrImportWaitFlag(bPath,VRM_IMPORT_WAIT_FLAG_PDS);
    vPDSClearModeChanged(bPath);
}

/**
 * @brief Clear Predown set resolution status
 * @param u1VdpId VDP_1/VDP_2
 */
void vDrvPDSClrSetRes(UINT8 u1VdpId, SET_RES_STATUS_TYPE_E eStatusType)
{

    if (u1VdpId == VDP_1)
    {
        if (eStatusType == SET_RES_OUTPUT|| eStatusType == SET_RES_BOTH)
        {
            PDS_WRITE_FLD(PRE_DS_MAIN_05, 1, PRE_DS_MAIN_05_MAIN_SET_RES_CLEAR_O);
        }
        if (eStatusType == SET_RES_INPUT || eStatusType == SET_RES_BOTH)
        {
            PDS_WRITE_FLD(PRE_DS_MAIN_05, 1, PRE_DS_MAIN_05_MAIN_SET_RES_CLEAR_I);
        }    
    }
    else
    {
        if (eStatusType == SET_RES_OUTPUT|| eStatusType == SET_RES_BOTH)
        {    
            PDS_WRITE_FLD(PRE_DS_PIP_04, 1, PRE_DS_PIP_04_PIP_SET_RES_CLEAR_O);
        }
        if (eStatusType == SET_RES_INPUT|| eStatusType == SET_RES_BOTH)
        {
            PDS_WRITE_FLD(PRE_DS_PIP_04, 1, PRE_DS_PIP_04_PIP_SET_RES_CLEAR_I);
        }    
    }
}

/**
 * @brief Get Predown set resolution status
 * @param u1VdpId VDP_1/VDP_2
 * @param eStatusType     SET_RES_INPUT, SET_RES_OUTPUT, SET_RES_BOTH,
 */
BOOL bDrvPDSIsSetResDone(UINT8 u1VdpId, SET_RES_STATUS_TYPE_E eStatusType)
{
    UINT8 bInStatus, bOutStatus;
    if (u1VdpId == VDP_1)
    {
        bInStatus  = PDS_READ_FLD(PRE_DS_MAIN_05, PRE_DS_MAIN_05_MAIN_SET_RES_FINISH_I);
        bOutStatus = PDS_READ_FLD(PRE_DS_MAIN_05, PRE_DS_MAIN_05_MAIN_SET_RES_FINISH_O);
    }
    else
    {
        bInStatus  = PDS_READ_FLD(PRE_DS_PIP_04, PRE_DS_PIP_04_PIP_SET_RES_FINISH_I);
        bOutStatus = PDS_READ_FLD(PRE_DS_PIP_04, PRE_DS_PIP_04_PIP_SET_RES_FINISH_O);
    }
    
    if (eStatusType == SET_RES_INPUT)
    {
        return bInStatus;
    }
    else if (eStatusType == SET_RES_OUTPUT)
    {
        return bOutStatus;
    }
    else
    {
        return (bInStatus & bOutStatus);
    }
    
}

void vDrvPDSInOutStatusDump(UINT8 bPath)
{
    if(SV_VP_MAIN==bPath)
    {
        LOG(0,"\tPDS OUT HT:%4d VT:%4d HA:%4d VA:%4d\n",
            IO32ReadFldAlign(PRE_DS_MAIN_06, PRE_DS_MAIN_06_MAIN_H_TOTAL),                        
            IO32ReadFldAlign(PRE_DS_MAIN_07, PRE_DS_MAIN_07_MAIN_V_TOTAL),            
            IO32ReadFldAlign(PRE_DS_MAIN_06, PRE_DS_MAIN_06_MAIN_H_ACTIVE),                
            IO32ReadFldAlign(PRE_DS_MAIN_07, PRE_DS_MAIN_07_MAIN_V_ACTIVE)
            );
    }
    else
    {
        LOG(0,"\tPDS OUT HT:%4d VT:%4d HA:%4d VA:%4d\n",           
            IO32ReadFldAlign(PRE_DS_PIP_06, PRE_DS_PIP_06_PIP_H_TOTAL),                        
            IO32ReadFldAlign(PRE_DS_PIP_07, PRE_DS_PIP_07_PIP_V_TOTAL),            
            IO32ReadFldAlign(PRE_DS_PIP_06, PRE_DS_PIP_06_PIP_H_ACTIVE),                
            IO32ReadFldAlign(PRE_DS_PIP_07, PRE_DS_PIP_07_PIP_V_ACTIVE)
            );
    }   
}

