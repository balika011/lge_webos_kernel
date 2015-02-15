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
 *   $Workfile: drv_monitorout.c $
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
#include "drv_mon_out_predown.h"
#include "drv_mon_out.h"
#include "drv_tve.h"
#include "drv_display.h"
#include "drv_tdtv_drvif.h"
#include "scpos_debug.h"
#include "vdo_misc.h"


EXTERN void vDrvMonOutSrmUpdate(void);

EXTERN MONOUTREGEN_INFO_T _monOutRegenInfo;

E_MON_OUT_TTT_CONFIG _MonOutTTTConfig = E_TTT_2D_IN;
#if defined(CC_MT5881)
#ifdef MON_OUT_CLIP_BLACK_BAR
EXTERN void TVE_ScalerSetDacOnOff(UCHAR ucOn);
#endif
#endif
/**
 * @brief monitor out normal procedure
 */
void vMonOutNormProc(UCHAR ucOnOff,UCHAR ucTveFmt) 
{
    //update prm
   // if((_arMonOutPrm.u4TveFmt != ucTveFmt) || (_arMonOutPrm.u4TveEnable != ucOnOff))
    {
    vMonOutRegenSetModeChanged();
    vMonOutPDSSetModeChanged();
    }
    vMonOutUpdateTveFmt(ucOnOff,ucTveFmt);
    //set register
    _vMonOutCenSel(ucOnOff);
    _vMonOutOnOff(ucOnOff);
        //11. Set Read Pointer gen.
    _vMonOutSetReadPosGen(ucTveFmt); 
    
    vMonOutTveSeting(ucTveFmt);
    vMonOutRegen();
   // vMonOutTTTConfig();
    vMonOutPDS();
    vMonOutCtrl();
#ifdef CC_FLIP_MIRROR_SUPPORT 
    _vMonOutSetMirrorFlipReg();
#endif   
    
    vDrvMonOutSrmUpdate();
    _vMonOutCheckWriteCtrl();
}
void _vMonOutCheckWriteCtrl(void)
{
    UINT8 bSigStatus;
    UINT8 bWriteOnOff= SV_OFF;
    UCHAR ucOnOff;
    UINT32 u4MIB     = u4DrvTDTVDIModeQuery();
    UINT32 u4TTD     = u4DrvTDTVTTDModeQuery();
	E_TD_OUT  eTTDOUT = TD_TTD_OUT(u4TTD);
    E_TD_OUT  eMIBOUT	 = TD_MIB_OUT(u4MIB);

    bSigStatus = bDrvVideoSignalStatus(SV_VP_MAIN);
    ucOnOff = _arMonOutPrm.u4TveEnable;

    if(ucOnOff && (bSigStatus == (UINT8)SV_VDO_STABLE))
    {
        bWriteOnOff = SV_ON;
    }
    else
    {
        bWriteOnOff = SV_OFF;
    }
    //when 3D output, not monitor out video 
    if((E_TD_OUT_3D_LI == eTTDOUT)|| (eMIBOUT ==E_TD_OUT_3D_LI))
    {
        bWriteOnOff = SV_OFF;
    }
    _vMonOutWriteCtrlOnOff(bWriteOnOff);
}

/**
 * @Remy porting done
 * @brief Turn On/Off MON OUT Write Control 
 * @param u1OnOff On/Off
 */
#define MON_READ_DELAY_CNT (6)
void _vMonOutWriteCtrlOnOff(UCHAR ucOnOff)
{
    static UINT8 bReadDlyCnt=0;
    
    if (ucOnOff == SV_ON)
    {
        _vMonOutSetDramWrite(SV_ON);

         if(!bReadDlyCnt)
        {
            _vMonOutSetDramRead(SV_ON);
        }
        else
        {
            bReadDlyCnt--;
        }
    } 
    else
    {   
        _vMonOutSetDramRead(SV_OFF); 
        _vMonOutSetDramWrite(SV_OFF);

        bReadDlyCnt = MON_READ_DELAY_CNT;
    }
}

void _vMonOutSetDramWrite(UCHAR ucOnOff)
{
    MONOUT_WRITE_FLD(DRAM_MON_00, ucOnOff, DRAM_MON_00_DA_WRITE_ENABLE);   
}

void _vMonOutSetDramRead(UCHAR ucOnOff)
{  
    MONOUT_WRITE_FLD(DRAM_MON_00, ucOnOff, DRAM_MON_00_DA_READ_ENABLE);	
}


void _vMonOutCenSel(UCHAR ucOnOff)
{  
    MONOUT_WRITE_FLD(MON_CTRL_03, ucOnOff, MON_CTRL_03_MON_CEN_SEL);	
}
void _vMonOutOnOff(UCHAR ucOnOff)
{  
    MONOUT_WRITE_FLD(MON_TVE_06, ucOnOff, MON_TVE_06_SC_PIP2TVE_ENABLE);
}
void _vMonOutSetReadPosGen(UCHAR ucTveFmt)
{
    if(ucTveFmt == TVE_FMT_480I)
    {
        MONOUT_WRITE_FLD(MON_CTRL_03, 480, MON_CTRL_03_SC_RPT_GEN_VCNT_1);	
        
    }
    else
    {
        MONOUT_WRITE_FLD(MON_CTRL_03, 576, MON_CTRL_03_SC_RPT_GEN_VCNT_1);	
    }
}

void vMonOutTveSeting(UCHAR ucTveFmt)
{
    if(ucTveFmt == TVE_FMT_480I)
    {
        vMonOutSetTVETimeGen(720,858,480,525);
        vMonOutSetTveTiming(720,858,480,525);
        
    }
    else
    {
        vMonOutSetTVETimeGen(720,864,576,625);
        vMonOutSetTveTiming(720,864,576,625);
    }
    
}
void vMonOutTTTConfig(void)
{
    UINT32 u4TTT;
    E_TD_OUT eTTTOut;
    MONOUTREGEN_INFO_T* monOutRegenInfo;
    monOutRegenInfo = getMonOutRegenInfo();
    u4TTT = u4DrvTDTVTTTModeQuery(SV_VP_MAIN);
    eTTTOut = TD_TTT_OUT(u4TTT);
    if((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (E_TD_OUT_3D_2_2D != eTTTOut))
    { 
        //1. update input resolution to global variable
        vMonOutTTTSetInputResolution(monOutRegenInfo->u4OutputWidth, monOutRegenInfo->u4OutputHeight);
        
        MONOUT_WRITE_FLD(MON_3D22D_00, 1, MON_3D22D_00_R_B3D22D_EN_M);
        if(IS_LVDS_DISP_3D_POLARIZED)
        {
            MONOUT_WRITE_FLD(MON_3D22D_00, 0x3, MON_3D22D_00_R_UI3DMODESEL_M);
            _MonOutTTTConfig = E_TTT_PR_IN;
        }
        else
        {
            MONOUT_WRITE_FLD(MON_3D22D_00, 0x0, MON_3D22D_00_R_UI3DMODESEL_M);
            _MonOutTTTConfig = E_TTT_SG_IN;
        }
    }
    else
    {
        MONOUT_WRITE_FLD(MON_3D22D_00, 0, MON_3D22D_00_R_B3D22D_EN_M);
        _MonOutTTTConfig = E_TTT_2D_IN;
    }
}

E_MON_OUT_TTT_CONFIG vMonOutGetTTTConfig(void)
{
    return _MonOutTTTConfig;
}
void vMonOutTTTSetInputResolution(UINT16 inW, UINT16 inH)
{
    MONOUT_WRITE_FLD(MON_3D22D_00, inW, MON_3D22D_00_R_UIIMGWIDTH_M);
    MONOUT_WRITE_FLD(MON_3D22D_00, inH, MON_3D22D_00_R_UIIMGHEIGHT_M);
}

void vMonOutSetTveTiming(UINT32 u4Hact, UINT32 u4Htot, UINT32 u4Vact, UINT32 u4Vtot)
{
	MONOUT_WRITE_FLD(MON_TVE_0C, u4Hact, MON_TVE_0C_SC_TVE_WIDTH);	
    MONOUT_WRITE_FLD(MON_TVE_0C, u4Vact, MON_TVE_0C_SC_TVE_HEIGHT);	
    MONOUT_WRITE_FLD(MON_TVE_0B, u4Htot, MON_TVE_0B_SC_TVE_HTOTAL);	
    MONOUT_WRITE_FLD(MON_TVE_0B, u4Vtot, MON_TVE_0B_SC_TVE_VTOTAL);	
}

void vMonOutSetTVETimeGen(UINT16 u2HActive,UINT16 u2HTotal, UINT16 u2VActive, UINT16 u2VTotal)
{    
    MONOUT_WRITE_FLD(MON_TVE_02, u2HActive, MON_TVE_02_SC_PNL_WIDTH);	
    MONOUT_WRITE_FLD(MON_TVE_02, u2VActive, MON_TVE_02_SC_PNL_HEIGHT);	
    MONOUT_WRITE_FLD(MON_TVE_03, u2HTotal, MON_TVE_03_SC_PNL_HTOTAL);	
    MONOUT_WRITE_FLD(MON_TVE_03, u2VTotal, MON_TVE_03_SC_PNL_VTOTAL);	
}
#ifdef CC_FLIP_MIRROR_SUPPORT

void _vMonOutSetMirrorFlipReg(void)
{
    UINT8 bMirrorFlipMode = SV_OFF;
    if (u1GetFlipMirrorConfig() & SYS_MIRROR_CONFIG_ON)
    {
        bMirrorFlipMode = SV_ON ;
        bMirrorFlipMode = bMirrorFlipMode << 1;
    }    

    if (u1GetFlipMirrorConfig() & SYS_FLIP_CONFIG_ON)
    {
        bMirrorFlipMode = bMirrorFlipMode | SV_ON;
    }
    
    MONOUT_WRITE_FLD(DRAM_MON_00, bMirrorFlipMode, DRAM_MON_00_DA_DISPLAY_MODE);
}
#endif

void vMonOutUpdateTveFmt(UCHAR ucOnOff,UCHAR ucTveFmt)
{
    //TVE_FMT_480I  or TVE_FMT_576I
    _arMonOutPrm.u4TveFmt = ucTveFmt;
    _arMonOutPrm.u4TveEnable = ucOnOff;	
    if(ucTveFmt == TVE_FMT_480I)
    {
        _arMonOutPrm.u4OutputWidth = 720;
        _arMonOutPrm.u4OutputHeight = 480;
    }
    else
    {
        _arMonOutPrm.u4OutputWidth = 720;
        _arMonOutPrm.u4OutputHeight = 576;
    }
}
UINT32 vMonOutGetTveFmt(void)
{
    return _arMonOutPrm.u4TveFmt;
}

void vMonOutSetDramWidthHeight(UINT32 u4DramWidth, UINT32 u4DramHeight)
{
    if(u4DramWidth != 0)
    {
	    MONOUT_WRITE_FLD(MON_CTRL_00, u4DramWidth, MON_CTRL_00_SC_DRAM_W_WIDTH_1);
    }
    
    if(u4DramHeight != 0)
    {
	    MONOUT_WRITE_FLD(MON_CTRL_00, u4DramHeight, MON_CTRL_00_SC_DRAM_W_HEIGHT_1);
    }
}
void vMonOutSetDramHVPitch(UINT32 u4DramWidth, UINT32 u4DramHeight)
{
    if(u4DramWidth != 0)
    {
	    MONOUT_WRITE_FLD(MON_CTRL_01, u4DramWidth, MON_CTRL_01_SC_WR_DRAM_H_PITCH_1);
    }
    
    if(u4DramHeight != 0)
    {
	    MONOUT_WRITE_FLD(MON_CTRL_01, u4DramHeight, MON_CTRL_01_SC_WR_DRAM_V_PITCH_1);
    }
}
void vMonOutCtrl(void)
{
  UINT32 u4DramWidth;
  UINT32 u4DramHeight;

   // need to add other ctrl ???
  if(_arMonOutPrm.u4TveFmt == TVE_FMT_480I)
  {
    u4DramWidth = 720;
    u4DramHeight = 480;
  }
  else
  {
    u4DramWidth = 720;
    u4DramHeight = 576;
  }

  vMonOutSetDramWidthHeight(u4DramWidth,u4DramHeight);
  vMonOutSetDramHVPitch(u4DramWidth,u4DramHeight);


}
void vMonOutUpdate(void)
{
    vMonOutRegenSetModeChanged();
    vMonOutPDSSetModeChanged();

    vMonOutRegen();
    vMonOutPDS();
}
#ifdef MON_OUT_CLIP_BLACK_BAR
void vMonOutGetVideoBlackBar(INT32 *pTop,INT32 *pBootom,INT32 *pLeft,INT32 *pRight)
{
    *pTop = _arMonOutPrm.u4BlackBarT;
    *pBootom = _arMonOutPrm.u4BlackBarB;
    *pLeft = _arMonOutPrm.u4BlackBarL;
    *pRight = _arMonOutPrm.u4BlackBarR;
    
    #ifdef CC_FLIP_MIRROR_SUPPORT
    if (u1GetFlipMirrorConfig() & SYS_MIRROR_CONFIG_ON)
    {
        *pLeft = _arMonOutPrm.u4BlackBarR;
        *pRight = _arMonOutPrm.u4BlackBarL;
    }    

    if (u1GetFlipMirrorConfig() & SYS_FLIP_CONFIG_ON)
    {
         *pTop = _arMonOutPrm.u4BlackBarB;
         *pBootom = _arMonOutPrm.u4BlackBarT;
    }
    #endif

}

void vMonOutSetUpdatePrmFlg(BOOL bflag)
{
    _arMonOutPrm.u4UpdateFlg = bflag;
}
BOOL  vMonOutGetUpdatePrmFlg(void)
{

    return _arMonOutPrm.u4UpdateFlg;
}
void vMonOutCheck(void)
{
    static UINT32 u4TveMuteCnt=0;
    static BOOL fgMuteStatus=SV_FALSE;
    
    if(SV_TRUE == vMonOutGetUpdatePrmFlg())
    {
        //set tve mute
        u4TveMuteCnt = MON_OUT_MUTE_COUNTER;
        fgMuteStatus = SV_TRUE;
        LOG(1,"vMonOutCheck:monitor out mute!\n");
        TVE_ScalerSetDacOnOff(SV_FALSE);
        vMonOutUpdate();
        vMonOutSetUpdatePrmFlg(SV_FALSE);
    }

    if(fgMuteStatus == SV_TRUE)
    {
        if(u4TveMuteCnt > 0)
       {
            u4TveMuteCnt--;
        }
        else
        {
             //set tve unmute
            LOG(1,"vMonOutCheck:monitor out unmute!\n");
            fgMuteStatus = SV_FALSE;
            TVE_ScalerSetDacOnOff(SV_TRUE);

        }
    }
    _vMonOutCheckWriteCtrl();

}
#endif
void vMonOutDumpModuleInfo(void)
{
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    MONOUTREGEN_INFO_T* monOutRegenInfo; 
    
    monOutPdsInfo = getMonOutPDSInfo();  
    monOutRegenInfo = getMonOutRegenInfo();
    
    if(monOutPdsInfo == NULL || monOutRegenInfo == NULL)
    {
        LOG(2, "Illegal module Prt\n");
        return;
    }
    
    LOG(2, "[%8s   X=%4d Y=%4d Width=%4d Height=%4d]\n",
            "MonOutRegen",
            0,
            0,
            monOutRegenInfo->u4InputWidth,
            monOutRegenInfo->u4InputHeight
            );
    LOG(2, "[%8s   X=%4d Y=%4d Width=%4d Height=%4d]\n",
            "TTDConfig  ",
            monOutRegenInfo->u4OutputX,
            monOutRegenInfo->u4OutputY,
            monOutRegenInfo->u4OutputWidth,
            monOutRegenInfo->u4OutputHeight
            );
    LOG(2, "[%8s   X=%4d Y=%4d Width=%4d Height=%4d]\n",
            "MonOutPds  ",
            0,
            0,
            monOutPdsInfo->u4InputWidth,
            monOutPdsInfo->u4InputHeight
            );
    LOG(2, "[%8s   X=%4d Y=%4d Width=%4d Height=%4d]\n",
            "TVE In     ",
            0,
            0,
            monOutPdsInfo->u4OutputWidth,
            monOutPdsInfo->u4OutputHeight
            );
    return;   
}

void vMonOutDumpAllModuleInfo(void)
{ 
    LOG(0, "FW STA DUMP\n");        
    LOG(0, "==============================================================================\n");        
    vMonOutDumpModuleInfo();
    //Video Module Status Dump
    LOG(0, "==============================================================================\n");  
    LOG(0, "HW STA DUMP\n");        
    LOG(0, "==============================================================================\n");         
    vDrvMonOutPDSInOutStatusDump();
    vDrvMonOutRegenInOutStatusDump();          
    LOG(0, "==============================================================================\n");   
}

void vMonOutModuleCfg(void)
{
    LOG(0, "TVE Config\n");
    LOG(0, "==============================================================================\n"); 
    LOG(0, "MonOut Tve is On/Off:(%d)\n",_arMonOutPrm.u4TveEnable);
    LOG(0, "MonOut Tve Fmt is :(%d)\n",_arMonOutPrm.u4TveFmt);
    LOG(0, "MonOut Tve TTT Config is:(%d)\n",_MonOutTTTConfig);
    LOG(0, "==============================================================================\n"); 
    
}


