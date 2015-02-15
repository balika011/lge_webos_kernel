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
 *   $Workfile: drv_regen.c $
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
#include "drv_mon_out_regen.h"
#include "drv_regen.h"
#include "nptv_debug.h"
#include "drv_mon_out.h"
#include "drv_tve.h"

#if defined(CC_MT5881)
#ifdef MON_OUT_CLIP_BLACK_BAR
EXTERN void TVE_ScalerSetSyncTime1(void);
#endif
#endif

MONOUTREGEN_INFO_T _monOutRegenInfo;

static void _vMonOutRegenUpdateInputResolution(UINT16 inW, UINT16 inH);

static void _vMonOutRegenUpdateDispPrm(void);


void vMonOutRegenSwInit(void)
{

}

void vMonOutRegenHwInit(void)
{
    //disable regen
    vMonOutRegenSetRegenEnableReg(SV_OFF);
    vMonOutRegenSetClipEnableReg(SV_OFF);

    //disable set resolution
    vMonOutRegenSetResolutionMode(SV_OFF);
    vMonOutRegenSetResolutionInTriSel(SV_ON);
}

/**
  * @brief Set Regen input resolution
  */
void vMonOutRegenSetInputResolutionReg(UINT32 u4InW, UINT32 u4InH)
{        
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_01, (UINT16)u4InW, MON_REGEN_01_RG_M_IN_ACTIVE_WIDTH);
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_01, (UINT16)u4InH, MON_REGEN_01_RG_M_IN_ACTIVE_HEIGHT);
}

/**
  * @brief Set Regen output resolution
  */
void vMonOutRegenSetOutputResolutionReg(UINT32 u4InW, UINT32 u4InH)
{        
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_02, (UINT16)u4InW, MON_REGEN_02_RG_M_OUT_ACTIVE_WIDTH);
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_02, (UINT16)u4InH, MON_REGEN_02_RG_M_OUT_ACTIVE_HEIGHT);
}

UINT32 u4MonOutRegenGetInputWidthReg(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_01, MON_REGEN_01_RG_M_IN_ACTIVE_WIDTH);
}

UINT32 u4MonOutRegenGetInputHeightReg(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_01, MON_REGEN_01_RG_M_IN_ACTIVE_HEIGHT);
}

UINT32 u4MonOutRegenGetOutputWidthReg(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_02, MON_REGEN_02_RG_M_OUT_ACTIVE_WIDTH);
}

UINT32 u4MonOutRegenGetOutputHeightReg(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_02, MON_REGEN_02_RG_M_OUT_ACTIVE_HEIGHT);
}

UINT32 u4MonOutRegenGetInHeight(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_05, MON_REGEN_05_RG_M_IN_HEIGHT_MEASURE);
}

UINT32 u4MonOutRegenGetInWidth(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_05, MON_REGEN_05_RG_M_IN_WIDTH_MEASURE);
}

UINT32 u4MonOutRegenGetOutHeight(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_06, MON_REGEN_06_RG_M_OUT_HEIGHT_MEASURE);
}

UINT32 u4MonOutRegenGetOutWidth(void)
{
    return MON_OUT_REGEN_READ_FLD(MON_REGEN_06, MON_REGEN_06_RG_M_OUT_WIDTH_MEASURE);
}

/**
  * @brief Set Regen L Start
  */
void vMonOutRegenSetLStartReg(UINT32 u4StaH, UINT32 u4StaV)
{        
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_03, (UINT16)u4StaH, MON_REGEN_03_RG_M_H_START_L);
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_03, (UINT16)u4StaV, MON_REGEN_03_RG_M_V_START_L);
}
/**
  * @brief Set Regen R Start
  */
void vMonOutRegenSetRStartReg(UINT32 u4StaH, UINT32 u4StaV)
{        
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_04, (UINT16)u4StaH, MON_REGEN_04_RG_M_H_START_R);
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_04, (UINT16)u4StaV, MON_REGEN_04_RG_M_V_START_R);
}
 
void vMonOutRegenSetModeChanged(void)
{
    MONOUTREGEN_INFO_T* monOutRegenInfo;    
    monOutRegenInfo = getMonOutRegenInfo();

    monOutRegenInfo->bMONOUTREGENModeChanged = SV_TRUE;
}

void vMonOutRegenSetRegenEnableReg(UINT8 bOnOff)
{
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, (bOnOff==SV_ON)?1:0 , MON_REGEN_00_RG_M_REGEN_ENABLE);
}

void vMonOutRegenSetClipEnableReg(UINT8 bOnOff)
{
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, (bOnOff==SV_ON)?1:0 , MON_REGEN_00_RG_M_CLIP_EN);
}

void vMonOutRegenSetImport(void)
{
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, 1, MON_REGEN_00_RG_M_SET_RES_ACTIVE);
}

void vMonOutRegenSetResolutionMode(UINT8 bOnOff)
{
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, (bOnOff==SV_ON)?1:0, MON_REGEN_00_RG_M_SET_RES_FROM_PREV_MODULE);
}

void vMonOutRegenSetResolutionInTriSel(UINT8 bOnOff)
{
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, (bOnOff==SV_ON)?1:0, MON_REGEN_00_RG_M_SET_RES_IN_TRIG_SEL);
}

void vMonOutRegenSetResolutionBypass(UINT8 bBypass)
{
    MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, (bBypass==SV_ON)?1:0, MON_REGEN_00_RG_M_BYPASS_SET_RES_DB);
}

UINT8 fgMonOutRegenIsModeChanged(void)
{
    MONOUTREGEN_INFO_T* monOutRegenInfo= getMonOutRegenInfo();
    UINT8 ret= SV_FALSE;

    if(monOutRegenInfo->bMONOUTREGENModeChanged == SV_TRUE)
    {
       ret = SV_TRUE;
    }

    return ret;
}

void vMonOutRegenClearModeChanged(void)
{
    MONOUTREGEN_INFO_T* monOutRegenInfo;    
    monOutRegenInfo = getMonOutRegenInfo();

    monOutRegenInfo->bMONOUTREGENModeChanged = SV_FALSE;
}

static void _vMonOutRegenUpdateInputResolution(UINT16 inW, UINT16 inH)
{
    MONOUTREGEN_INFO_T* monOutRegenInfo;    
    monOutRegenInfo = getMonOutRegenInfo();

    monOutRegenInfo->u4InputWidth = inW;
    monOutRegenInfo->u4InputHeight = inH;

}
#ifdef MON_OUT_CLIP_BLACK_BAR
//clip black bar when output region is not full
void _vMonOutRegenUpdateOutputInfo(void)
{
    UINT16 outX;
    UINT16 outY;
    UINT16 outW;
    UINT16 outH;
    VRM_INFO_T* preVrmInfo;
    UINT32 u4Hfp;
    UINT32 u4Hbp;
    UINT32 u4Vfp;
    UINT32 u4Vbp;

    UINT32 u4Top;
    UINT32 u4Bottom;
    UINT32 u4Left;
    UINT32 u4Right;
    
    UINT32 u4TempBlackL;
    UINT32 u4TempBlackR;
    UINT32 u4TempBlackT;
    UINT32 u4TempBlackB;
    
    UINT32 u4NeedBlack;
    
    preVrmInfo = VRMGetVRMInfo(SV_VP_MAIN, VRM_MODULE_MIXER);

    //add black bar, for overscan issue
    u4Hfp = u4ScpipGetHsyncFp();
    u4Hbp = u4ScpipGetHsyncBp();
    u4Vfp = u4ScpipGetVsyncFp();
    u4Vbp = u4ScpipGetVsyncBp();

    _arMonOutPrm.u4BlackBarL = 0;
    _arMonOutPrm.u4BlackBarR = 0;
    _arMonOutPrm.u4BlackBarT = 0;
    _arMonOutPrm.u4BlackBarB = 0;

    //case1:H is full, add H porch for black bar
    //case2: H is not full, and output window black bar is not enough
    u4NeedBlack = (preVrmInfo->u2Width*2*MON_OUT_ADD_HBLACK)/(10000-2*MON_OUT_ADD_HBLACK);
    if((wDISPLAY_WIDTH - preVrmInfo->u2Width) < u4NeedBlack)
    {
        u4TempBlackL = u4Hbp + preVrmInfo->u2X;
        u4TempBlackR = u4Hfp + wDISPLAY_WIDTH - preVrmInfo->u2Width - preVrmInfo->u2X;
       if((u4TempBlackR + u4TempBlackL)< u4NeedBlack)
       {
         outX = 0;
         outW = preVrmInfo->u2Width +u4TempBlackL + u4TempBlackR ; 

         u4Left = u4TempBlackL;
         u4Right = u4TempBlackR;

       }
       else if(u4TempBlackL > u4NeedBlack)
       {
         outX = u4TempBlackL - u4NeedBlack;
         outW = preVrmInfo->u2Width + u4NeedBlack; 

         u4Left = u4NeedBlack;
         u4Right = 0;
       }
       else
       {
         outX = u4TempBlackL;
         outW = preVrmInfo->u2Width + u4NeedBlack; 

         u4Left = 0;
         u4Right = u4NeedBlack;
       }
       if(outW)
       {
          _arMonOutPrm.u4BlackBarL = (u4Left*_arMonOutPrm.u4OutputWidth )/outW;
          _arMonOutPrm.u4BlackBarR = (u4Right*_arMonOutPrm.u4OutputWidth )/outW;
       }
       
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case 1/2 outX=%d outW=%d u4Left=%d u4Right=%d\n",outX,outW,u4Left,u4Right);
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case1/2 u4NeedBlack=%d u4BlackBarL=%d u4BlackBarR=%d\n",u4NeedBlack,_arMonOutPrm.u4BlackBarL,_arMonOutPrm.u4BlackBarR);

       
    }
    //case3: output window black bar is enough
    else
    {
        u4TempBlackL = preVrmInfo->u2X;
        u4TempBlackR = wDISPLAY_WIDTH - preVrmInfo->u2Width - preVrmInfo->u2X;
        
        if(u4TempBlackL > u4NeedBlack)
       {
         outX = u4TempBlackL - u4NeedBlack + u4Hbp;
         outW = preVrmInfo->u2Width + u4NeedBlack; 

         u4Left = u4NeedBlack;
         u4Right = 0;
       }
       else if(u4TempBlackR > u4NeedBlack)
       {
         outX = u4TempBlackL + u4Hbp;
         outW = preVrmInfo->u2Width + u4NeedBlack; 

         u4Left = 0;
         u4Right = u4NeedBlack;
        }
        else
        {
         outX = u4Hbp;
         outW = preVrmInfo->u2Width + u4NeedBlack; 

         u4Left = u4TempBlackL;
         u4Right = u4NeedBlack - u4TempBlackL;

        }

        if(outW < _arMonOutPrm.u4OutputWidth)
        {
         outX =u4Hbp + (((wDISPLAY_WIDTH -preVrmInfo->u2X) < _arMonOutPrm.u4OutputWidth)?(wDISPLAY_WIDTH - _arMonOutPrm.u4OutputWidth):preVrmInfo->u2X);
         outW = _arMonOutPrm.u4OutputWidth;

         u4Left = (((wDISPLAY_WIDTH -preVrmInfo->u2X) < _arMonOutPrm.u4OutputWidth)?(preVrmInfo->u2X + _arMonOutPrm.u4OutputWidth - wDISPLAY_WIDTH):0);
         u4Right = (((wDISPLAY_WIDTH -preVrmInfo->u2X) < _arMonOutPrm.u4OutputWidth)?(wDISPLAY_WIDTH-preVrmInfo->u2X-preVrmInfo->u2Width):(_arMonOutPrm.u4OutputWidth - preVrmInfo->u2Width));
        }

       if(outW)
        {
              _arMonOutPrm.u4BlackBarL = (u4Left*_arMonOutPrm.u4OutputWidth )/outW;
              _arMonOutPrm.u4BlackBarR = (u4Right*_arMonOutPrm.u4OutputWidth )/outW;
        
        }     

       LOG(1,"_vMonOutRegenUpdateOutputInfo:case 3 outX=%d outW=%d u4Left=%d u4Right=%d\n",outX,outW,u4Left,u4Right);
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case 3 u4NeedBlack=%d u4BlackBarL=%d u4BlackBarR=%d\n",u4NeedBlack,_arMonOutPrm.u4BlackBarL,_arMonOutPrm.u4BlackBarR);

    }

     u4NeedBlack = (preVrmInfo->u2Height*2*MON_OUT_ADD_VBLACK)/(10000-2*MON_OUT_ADD_VBLACK);

    // add v black
    if((wDISPLAY_HEIGHT - preVrmInfo->u2Height) < u4NeedBlack)
    {
        u4TempBlackT = u4Vbp + preVrmInfo->u2Y;
        u4TempBlackB = u4Vfp + wDISPLAY_HEIGHT - preVrmInfo->u2Height - preVrmInfo->u2Y;
       if((u4TempBlackT + u4TempBlackB)< u4NeedBlack)
       {
         outY = 0;
         outH = preVrmInfo->u2Height +u4TempBlackT + u4TempBlackB ; 

         u4Top = u4TempBlackT;
         u4Bottom = u4TempBlackB;
       }
       else if(u4TempBlackT > u4NeedBlack)
       {
         outY = u4TempBlackT - u4NeedBlack;
         outH = preVrmInfo->u2Height + u4NeedBlack; 

         u4Top = u4NeedBlack;
         u4Bottom = 0;
       }
       else
       {
         outY = u4TempBlackT;
         outH = preVrmInfo->u2Height + u4NeedBlack; 

         u4Top = 0;
         u4Bottom = u4NeedBlack;
       }
        if(outH)
        {
       _arMonOutPrm.u4BlackBarT = (u4Top*_arMonOutPrm.u4OutputHeight)/outH;
       _arMonOutPrm.u4BlackBarB = (u4Bottom*_arMonOutPrm.u4OutputHeight)/outH;
       }
       
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case1/2 outY=%d outH=%d u4Top=%d u4Bottom=%d\n",outY,outH,u4Top,u4Bottom);
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case1/2 u4NeedBlack=%d u4BlackBarT=%d u4BlackBarB=%d\n",u4NeedBlack,_arMonOutPrm.u4BlackBarT,_arMonOutPrm.u4BlackBarB);

    }
    else
    {
        u4TempBlackT = preVrmInfo->u2Y;
        u4TempBlackB = wDISPLAY_HEIGHT - preVrmInfo->u2Height- preVrmInfo->u2Y;
        
        if(u4TempBlackT > u4NeedBlack)
       {
         outY = u4TempBlackT - u4NeedBlack + u4Vbp;
         outH = preVrmInfo->u2Height + u4NeedBlack; 

         u4Top = u4NeedBlack;
         u4Bottom = 0;
       }
       else if(u4TempBlackR > u4NeedBlack)
       {
         outY = u4TempBlackT + u4Vbp;
         outH = preVrmInfo->u2Height + u4NeedBlack; 

         u4Top = 0;
         u4Bottom = u4NeedBlack;
        }
        else
        {
         outY = u4Vbp;
         outH = preVrmInfo->u2Height + u4NeedBlack; 

         u4Top = u4TempBlackT;
         u4Bottom = u4NeedBlack - u4TempBlackT;
        }
        
        if(outH < _arMonOutPrm.u4OutputHeight)
        {
         outY = u4Vbp + (((wDISPLAY_HEIGHT -preVrmInfo->u2Y) < _arMonOutPrm.u4OutputHeight)?(wDISPLAY_HEIGHT - _arMonOutPrm.u4OutputHeight):preVrmInfo->u2Y);
         outH = _arMonOutPrm.u4OutputHeight;

         u4Top = (((wDISPLAY_HEIGHT -preVrmInfo->u2Y) < _arMonOutPrm.u4OutputHeight)?(preVrmInfo->u2Y + _arMonOutPrm.u4OutputHeight- wDISPLAY_HEIGHT ):0);
         u4Bottom = (((wDISPLAY_HEIGHT -preVrmInfo->u2Y) < _arMonOutPrm.u4OutputHeight)?(wDISPLAY_HEIGHT-preVrmInfo->u2Y-preVrmInfo->u2Height):(_arMonOutPrm.u4OutputHeight - preVrmInfo->u2Height));
        }
        if(outH)
        {
            _arMonOutPrm.u4BlackBarT = (u4Top*_arMonOutPrm.u4OutputHeight)/outH;
            _arMonOutPrm.u4BlackBarB = (u4Bottom*_arMonOutPrm.u4OutputHeight)/outH;
       }
       
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case 3 outY=%d outH=%d u4Top=%d u4Bottom=%d\n",outY,outH,u4Top,u4Bottom);
       LOG(1,"_vMonOutRegenUpdateOutputInfo:case 3 u4NeedBlack=%d u4BlackBarT=%d u4BlackBarB=%d\n",u4NeedBlack,_arMonOutPrm.u4BlackBarT,_arMonOutPrm.u4BlackBarB);

    }
    outX = ((outX>0)?(outX-1):outX);
    outY = ((outY>0)?(outY-1):outY);
    _vMonOutRegenUpdateOutputResolution(outX,outY,outW,outH); 
    TVE_ScalerSetSyncTime1();

}
#endif
void _vMonOutRegenUpdateOutputResolution(UINT16 outX, UINT16 outY, UINT16 outW, UINT16 outH)
{
    MONOUTREGEN_INFO_T* monOutRegenInfo;    
    monOutRegenInfo = getMonOutRegenInfo();
    if(IS_LVDS_DISP_3D_POLARIZED && (u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (outY %2))
    {
        outY += 1;  
        if(outH %2)
        {
            outH -= 1;
        }
        else
        {
            outH -= 2;
        }
    }
    monOutRegenInfo->u4OutputX = outX;
    monOutRegenInfo->u4OutputY = outY;
    monOutRegenInfo->u4OutputWidth = outW;
    monOutRegenInfo->u4OutputHeight = outH;
}

static void _vMonOutRegenUpdateDispPrm(void)
{
    //1. update input resolution to global variable
    _vMonOutRegenUpdateInputResolution(u4ScpipGetPanelWidth() + u4ScpipGetHsyncFp() + u4ScpipGetHsyncBp(), u4ScpipGetPanelHeight() + u4ScpipGetVsyncFp() + u4ScpipGetVsyncBp());
    
    //2. update output resolution to global variable
    #ifndef MON_OUT_CLIP_BLACK_BAR
    _vMonOutRegenUpdateOutputResolution(u4ScpipGetHsyncBp(), u4ScpipGetVsyncBp(), u4ScpipGetPanelWidth(), u4ScpipGetPanelHeight());  
    #else
    _vMonOutRegenUpdateOutputInfo();
    #endif
    
}
 
void _vMonOutRegenUpdateReg(void)
{
    UINT32 u4TTT;
    E_TD_OUT eTTTOut;
    MONOUTREGEN_INFO_T* monOutRegenInfo;
    monOutRegenInfo = getMonOutRegenInfo();
    u4TTT = u4DrvTDTVTTTModeQuery(SV_VP_MAIN);
    eTTTOut = TD_TTT_OUT(u4TTT);

    if(monOutRegenInfo->u4InputWidth == 0 || monOutRegenInfo->u4InputHeight == 0)
    {
        goto MON_OUT_REGEN_UPDATE_FINISH;
    }    
    //0. Set 3D format
    if((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (E_TD_OUT_3D_2_2D != eTTTOut))
    {
        MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, (IS_LVDS_DISP_3D_SHUTTER)? 0x3 : 0x0, MON_REGEN_00_RG_M_IN_FORMAT);   
    }
    //1. update input resolution
    vMonOutRegenSetInputResolutionReg(monOutRegenInfo->u4InputWidth,monOutRegenInfo->u4InputHeight);
    //2. update output resolution
    vMonOutRegenSetOutputResolutionReg(monOutRegenInfo->u4OutputWidth,monOutRegenInfo->u4OutputHeight);
    //3. update L/R overscan
    vMonOutRegenSetLStartReg(monOutRegenInfo->u4OutputX,monOutRegenInfo->u4OutputY);
    vMonOutRegenSetRStartReg(monOutRegenInfo->u4OutputX,monOutRegenInfo->u4OutputY);    
    //4. set regen enable
    vMonOutRegenSetRegenEnableReg(SV_ON);
    vMonOutRegenSetClipEnableReg(SV_ON);
    //5. import 
    vMonOutRegenSetImport();

      
  MON_OUT_REGEN_UPDATE_FINISH:  
    vMonOutRegenClearModeChanged();
  
}

/**
  * @Mou Out regen main loop
  */
void vMonOutRegen(void)
{
    if(fgMonOutRegenIsModeChanged()== SV_TRUE)
    {
        _vMonOutRegenUpdateDispPrm();
        _vMonOutRegenUpdateReg();
         vMonOutRegenClearModeChanged();
    }
}


/**
 * @brief Clear Regen set resolution status
 */
void vDrvMonOutRegenClrSetRes(SET_RES_STATUS_TYPE_E eStatusType)
{
    if (eStatusType == SET_RES_OUTPUT || eStatusType == SET_RES_BOTH)
    {
        MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, 1, MON_REGEN_00_RG_M_SET_RES_CLEAR_O);
    }
    if (eStatusType == SET_RES_INPUT || eStatusType == SET_RES_BOTH)
    {
        MON_OUT_REGEN_WRITE_FLD(MON_REGEN_00, 1, MON_REGEN_00_RG_M_SET_RES_CLEAR_I);
    }       
}

/**
 * @brief Get Regen set resolution status
 * @param eStatusType     SET_RES_INPUT, SET_RES_OUTPUT, SET_RES_BOTH,
 */
BOOL bDrvMonOutRegenIsSetResDone(SET_RES_STATUS_TYPE_E eStatusType)
{
    UINT8 bInStatus, bOutStatus;

        bInStatus  = MON_OUT_REGEN_READ_FLD(MON_REGEN_00, MON_REGEN_00_RG_M_SET_RES_FINISHED_I);
        bOutStatus = MON_OUT_REGEN_READ_FLD(MON_REGEN_00, MON_REGEN_00_RG_M_SET_RES_FINISHED_O);
    
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


void vDrvMonOutRegenInOutStatusDump(void)
{
    //Mon Out REGEN HW Measure INPUT
    LOG(0,"\tMon Out RGN  IN   HT: N/A VT: N/A HA:%4d VA:%4d\n",           
        MON_OUT_REGEN_READ_FLD(MON_REGEN_05, MON_REGEN_05_RG_M_IN_WIDTH_MEASURE),                
        MON_OUT_REGEN_READ_FLD(MON_REGEN_05, MON_REGEN_05_RG_M_IN_HEIGHT_MEASURE)
        );
    //Mon Out REGEN HW Measure OUTPUT   
    LOG(0,"\tRGN OUT RGN OUT   HT: N/A VT: N/A HA:%4d VA:%4d\n",           
        MON_OUT_REGEN_READ_FLD(MON_REGEN_06, MON_REGEN_06_RG_M_OUT_WIDTH_MEASURE),                
        MON_OUT_REGEN_READ_FLD(MON_REGEN_06, MON_REGEN_06_RG_M_OUT_HEIGHT_MEASURE)
        );        
}
