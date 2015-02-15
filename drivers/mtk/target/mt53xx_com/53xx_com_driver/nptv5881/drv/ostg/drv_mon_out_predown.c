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
#include "hw_mon_out.h"
#include "drv_mon_out.h"
#include "drv_mon_out_predown.h"
#include "drv_mon_out_regen.h"
#include "drv_scaler.h"
#include "scpos_debug.h"
#include "drv_tve.h"



MONOUTPREDOWN_INFO_T _mon_out_pdsInfo;
EXTERN MONOUTREGEN_INFO_T _monOutRegenInfo;

static void _vMonOutPDSUpdateReg(void);
static void _vMonOutPDSUpdateInputResolution(UINT16 inW, UINT16 inH);
static void _vMonOutPDSUpdateOutputResolution(void);
static void _vMonOutPDSUpdateHPDSFactorPrm(UINT16 inW, UINT16 outW);
static void _vMonOutPDSUpdateVPDSFactorPrm(UINT16 inH, UINT16 outH);



void vMonOutPDSSwInit(void)
{
    //sw init
    vMonOutPDSSetHPDSFactor(DFT_HPD_FACTOR);
    vMonOutPDSSetVPDSFactor(DFT_HPD_FACTOR);
}

void vMonOutPDSHwInit(void)
{
    //hw init
    vMonOutPDSSetHPSFactorReg(DFT_HPD_FACTOR);
    vMonOutPDSSetVPSFactorReg(DFT_HPD_FACTOR);
    vMonOutPDSSetVCenActAtSync(1);
    vMonOutPDSImportEn(SV_ON);
}

void vMonOutPDSImportEn(UINT8 bOnOff)
{

   	Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, (bOnOff==1)?1:0, MON_PRE_DS_05_PREDS_MAIN_SETRES_EN);
}

void vMonOutPDSImportTrigger(void)
{
   	Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, 1, MON_PRE_DS_05_PREDS_MAIN_SETRES);
}

UINT32 u4MonOutPDSGetHPDSFactor(void)
{

    return _mon_out_pdsInfo.u4HPDSFactor;
}

UINT32 u4MonOutPDSGetVPDSFactor(void)
{
    return _mon_out_pdsInfo.u4VPDSFactor;
}

void vMonOutPDSSetHPDSFactor(UINT32 factor)
{
    _mon_out_pdsInfo.u4HPDSFactor = factor;
}

void vMonOutPDSSetVPDSFactor(UINT32 factor)
{
    _mon_out_pdsInfo.u4VPDSFactor = factor;

}

/**
* @brief Get H pre-scaling down factor
* @param bPath VDP_1/VDP_2
* @return pre-scaling down factor
* @cobra porting done
*/
UINT32 u4MonOutPDSGetHPSFactorReg(void)
{
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_00, MON_PRE_DS_00_PRE_DSCALE_H_1);
}
/**
* @brief Get V pre-scaling down factor
* @param bPath VDP_1/VDP_2
* @return pre-scaling down factor
* @6896 porting done
*/
UINT32 u4MonOutPDSGetVPSFactorReg(void)
{
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_00, MON_PRE_DS_00_PRE_DSCALER_V_1);
}
/**
* @brief Set pre-scaling down factor
* @param u4VdpId VDP_1/VDP_2
*/
void vMonOutPDSSetHPSFactorReg(UINT32 u4Factor)
{        
    UINT32 u4HpsFactor; 
    UINT32 u4GetHwHps;

    u4GetHwHps = u4MonOutPDSGetHPSFactorReg();
    // defaul value:0x8000 means auto init phase, no need to set the phase again.
    //u4HInitValue = DFT_HPD_FACTOR - u4Factor;
    u4HpsFactor = u4Factor;
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_00, (UINT16)u4HpsFactor, MON_PRE_DS_00_PRE_DSCALE_H_1);
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_01, (UINT16)(0x8000-u4HpsFactor), MON_PRE_DS_01_PRE_DSCALE_H_INIT_1);
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_02 , (UINT16)(0x8000-u4HpsFactor), MON_PRE_DS_02_PRE_DSCALE_H_INIT_RIGHT_1);    

    if(u4HpsFactor != u4GetHwHps)
    {      	 	
    	//LboxSourceChange();	 
    }
}


/**
* @brief Set pre-scaling down factor
* @param u4VdpId VDP_1/VDP_2
*/
void vMonOutPDSSetVPSFactorReg(UINT32 u4Factor)
{        
    UINT32 u4VpsFactor; 

    // defaul value:0x8000 means auto init phase, no need to set the phase again.
    //u4VInitValue = DFT_HPD_FACTOR - u4Factor;
    u4VpsFactor = u4Factor;
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_00, (UINT16)u4VpsFactor, MON_PRE_DS_00_PRE_DSCALER_V_1);
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_01, (UINT16)(0x8000-u4VpsFactor), MON_PRE_DS_01_PRE_DSCALE_V_INIT_1);
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_02 , (UINT16)(0x8000-u4VpsFactor), MON_PRE_DS_02_PRE_DSCALE_V_INIT_RIGHT_1);    
 
}

/**
* @brief Set pre-scaling input resolution
* @param u4VdpId VDP_1/VDP_2
*/
void vMonOutPDSSetInputResolutionReg(UINT32 u4InW, UINT32 u4InH)
{        
	Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_04, (UINT16)u4InW, MON_PRE_DS_04_PREDS_MAIN_IN_H);
	Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_04, (UINT16)u4InH, MON_PRE_DS_04_PREDS_MAIN_IN_V);
}

/**
* @brief Set pre-scaling output resolution
* @param u4VdpId VDP_1/VDP_2
*/
void vMonOutPDSSetOutputResolutionReg(UINT32 u4InW, UINT32 u4InH)
{        
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_03, (UINT16)u4InW, MON_PRE_DS_03_PREDS_MAIN_OUT_H);
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_03, (UINT16)u4InH, MON_PRE_DS_03_PREDS_MAIN_OUT_V);
}

void vMonOutPDSSetVCenActAtSync(UINT8 value)
{
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, value, MON_PRE_DS_05_MAIN_VCEN_ACT_AT_SYNC);       
}

UINT32 u4MonOutPDSGetHActive(UINT8 bInOut)
{
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, (bInOut==1)?1:0, MON_PRE_DS_05_MAIN_SEL_HVCNT);     
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_06, MON_PRE_DS_06_MAIN_H_ACTIVE);
}

UINT32 u4MonOutPDSGetHTotal(UINT8 bInOut)
{
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, (bInOut==1)?1:0, MON_PRE_DS_05_MAIN_SEL_HVCNT);     
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_06, MON_PRE_DS_06_MAIN_H_TOTAL);
}

UINT32 u4MonOutPDSGetVActive(UINT8 bInOut)
{
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, (bInOut==1)?1:0, MON_PRE_DS_05_MAIN_SEL_HVCNT);     
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_07,MON_PRE_DS_07_MAIN_V_ACTIVE);
}

UINT32 u4MonOutPDSGetVTotal(UINT8 bInOut)
{
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, (bInOut==1)?1:0, MON_PRE_DS_05_MAIN_SEL_HVCNT);     
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_07, MON_PRE_DS_07_MAIN_V_TOTAL);
}

UINT32 u4MonOutPDSGetInputWidthReg(void)
{
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_04, MON_PRE_DS_04_PREDS_MAIN_IN_H);
}

UINT32 u4MonOutPDSGetInputHeightReg(void)
{
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_04, MON_PRE_DS_04_PREDS_MAIN_IN_V);
}

UINT32 u4MonOutPDSGetOutputWidthReg(void)
{
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_03, MON_PRE_DS_03_PREDS_MAIN_OUT_H);
}

UINT32 u4MonOutPDSGetOutputHeightReg(void)
{
    return Mon_Out_PDS_READ_FLD(MON_PRE_DS_03, MON_PRE_DS_03_PREDS_MAIN_OUT_V);
}

UINT8 fgMonOutPDSIsModeChanged(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    UINT8 ret;

    ret = SV_FALSE;
    monOutPdsInfo = getMonOutPDSInfo();
    
    if(monOutPdsInfo->bMonOutPDSModeChanged == SV_TRUE)
    {
       ret = SV_TRUE;
    }
    return ret;
}

void vMonOutPDSSetModeChanged(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    monOutPdsInfo = getMonOutPDSInfo();

    monOutPdsInfo->bMonOutPDSModeChanged = SV_TRUE;    
}

void vMonOutPDSClearModeChanged(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    monOutPdsInfo = getMonOutPDSInfo();
    
    monOutPdsInfo->bMonOutPDSModeChanged = SV_FALSE;
}

static void _vMonOutPDSUpdateInputResolution(UINT16 inW, UINT16 inH)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    monOutPdsInfo = getMonOutPDSInfo();

    monOutPdsInfo->u4InputWidth = inW;
    monOutPdsInfo->u4InputHeight = inH;
}

static void _vMonOutPDSUpdateOutputResolution(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    UINT32 ucTveFmt;
    monOutPdsInfo = getMonOutPDSInfo();
    ucTveFmt = vMonOutGetTveFmt();
    monOutPdsInfo->u4OutputWidth = 720;
    if(ucTveFmt == TVE_FMT_480I)
    {
        monOutPdsInfo->u4OutputHeight = 480;
    }
    else
    {
        monOutPdsInfo->u4OutputHeight = 576;
    }
}

static void _vMonOutPDSUpdateHPDSFactorPrm(UINT16 inW, UINT16 outW)
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

    vMonOutPDSSetHPDSFactor(u4Factor);
}

static void _vMonOutPDSUpdateVPDSFactorPrm(UINT16 inH, UINT16 outH)
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
    
    vMonOutPDSSetVPDSFactor(u4Factor);
}


void _vMonOutPDSUpdateDispPrm(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    MONOUTREGEN_INFO_T* monOutRegenInfo; 
    UINT16 u2PdsInW;
    UINT16 u2PdsInH;
    
    monOutPdsInfo = getMonOutPDSInfo(); 
    monOutRegenInfo = getMonOutRegenInfo();

    u2PdsInW = monOutRegenInfo->u4OutputWidth;
    u2PdsInH = monOutRegenInfo->u4OutputHeight;
    //1. update input resolution to global variable
    if(vMonOutGetTTTConfig() == E_TTT_PR_IN)
    {
        u2PdsInH = (u2PdsInH >> 1);
    }
    _vMonOutPDSUpdateInputResolution(u2PdsInW,u2PdsInH);
    //2. update output resolution to global variable
    _vMonOutPDSUpdateOutputResolution();
    //3. update h pds factor
    _vMonOutPDSUpdateHPDSFactorPrm(monOutPdsInfo->u4InputWidth, monOutPdsInfo->u4OutputWidth);
    //4. update v pds factor
    _vMonOutPDSUpdateVPDSFactorPrm(monOutPdsInfo->u4InputHeight, monOutPdsInfo->u4OutputHeight);
}

static void _vMonOutPDSUpdateReg(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    monOutPdsInfo = getMonOutPDSInfo();

    //1. update input resolution
    vMonOutPDSSetInputResolutionReg(monOutPdsInfo->u4InputWidth,monOutPdsInfo->u4InputHeight);
    //2. update output resolution
    vMonOutPDSSetOutputResolutionReg(monOutPdsInfo->u4OutputWidth, monOutPdsInfo->u4OutputHeight);
    //3. update HPDS
    vMonOutPDSSetHPSFactorReg(monOutPdsInfo->u4HPDSFactor);
    //4. update VPDS
    vMonOutPDSSetVPSFactorReg(monOutPdsInfo->u4VPDSFactor);
    //5. import
    vMonOutPDSImportTrigger();
}

void vMonOutPDS(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    monOutPdsInfo = getMonOutPDSInfo();

    if(fgMonOutPDSIsModeChanged()== SV_TRUE)
    {
        _vMonOutPDSUpdateDispPrm();        

        if(monOutPdsInfo->u4InputWidth == 0 || 
           monOutPdsInfo->u4InputHeight == 0 ||
           monOutPdsInfo->u4OutputWidth == 0 ||
           monOutPdsInfo->u4OutputHeight == 0 || 
           monOutPdsInfo->u4HPDSFactor == 0 ||
           monOutPdsInfo->u4VPDSFactor == 0)
        {
            goto MonOut_PREDOWN_UPDATE_FINISH;
        }
           
        _vMonOutPDSUpdateReg();
    }

MonOut_PREDOWN_UPDATE_FINISH:    

    vMonOutPDSClearModeChanged();
}

/**
 * @brief Clear Predown set resolution status
 * @param u1VdpId VDP_1/VDP_2
 */
void vDrvMonOutPDSClrSetRes(SET_RES_STATUS_TYPE_E eStatusType)
{
    if (eStatusType == SET_RES_OUTPUT|| eStatusType == SET_RES_BOTH)
    {
        Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, 1, MON_PRE_DS_05_MAIN_SET_RES_CLEAR_O);
    }
    if (eStatusType == SET_RES_INPUT || eStatusType == SET_RES_BOTH)
    {
        Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, 1, MON_PRE_DS_05_MAIN_SET_RES_CLEAR_I);
    }      
}

/**
 * @brief Get Predown set resolution status
 * @param u1VdpId VDP_1/VDP_2
 * @param eStatusType     SET_RES_INPUT, SET_RES_OUTPUT, SET_RES_BOTH,
 */
BOOL bDrvMonOutPDSIsSetResDone(SET_RES_STATUS_TYPE_E eStatusType)
{
    UINT8 bInStatus, bOutStatus;

    bInStatus  = Mon_Out_PDS_READ_FLD(MON_PRE_DS_05, MON_PRE_DS_05_MAIN_SET_RES_FINISH_I);
    bOutStatus = Mon_Out_PDS_READ_FLD(MON_PRE_DS_05, MON_PRE_DS_05_MAIN_SET_RES_FINISH_O);

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

void vDrvMonOutPDSInOutStatusDump(void)
{
    //input  Measure HW STA
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, 0, MON_PRE_DS_05_MAIN_SEL_HVCNT); 
    LOG(0,"\tMon Out PDS INPUT HT:%4d VT:%4d HA:%4d VA:%4d\n",
         IO32ReadFldAlign(MON_PRE_DS_06, MON_PRE_DS_06_MAIN_H_TOTAL),                        
         IO32ReadFldAlign(MON_PRE_DS_07, MON_PRE_DS_07_MAIN_V_TOTAL),            
         IO32ReadFldAlign(MON_PRE_DS_06, MON_PRE_DS_06_MAIN_H_ACTIVE),                
         IO32ReadFldAlign(MON_PRE_DS_07, MON_PRE_DS_07_MAIN_V_ACTIVE)
        );
    //output Measure HW STA
    Mon_Out_PDS_WRITE_FLD(MON_PRE_DS_05, 1, MON_PRE_DS_05_MAIN_SEL_HVCNT); 
    LOG(0,"\tMon Out PDS OUT   HT:%4d VT:%4d HA:%4d VA:%4d\n",
         IO32ReadFldAlign(MON_PRE_DS_06, MON_PRE_DS_06_MAIN_H_TOTAL),                        
         IO32ReadFldAlign(MON_PRE_DS_07, MON_PRE_DS_07_MAIN_V_TOTAL),            
         IO32ReadFldAlign(MON_PRE_DS_06, MON_PRE_DS_06_MAIN_H_ACTIVE),                
         IO32ReadFldAlign(MON_PRE_DS_07, MON_PRE_DS_07_MAIN_V_ACTIVE)
        );
}
