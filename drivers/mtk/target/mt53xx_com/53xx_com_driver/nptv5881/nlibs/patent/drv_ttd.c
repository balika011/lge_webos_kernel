/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_ttd.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#include "general.h"
#include "x_assert.h"

#include "hw_ttd.h"
#include "hw_ospe.h"

#include "drv_ttd.h"
#include "nptv_debug.h"
#include "drv_video.h"
#include "drv_scpos.h"
#include "drv_display.h"

#include "drv_tdtv_feature.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "panel.h"
#include "vdo_misc.h"

#include "mute_if.h"

/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
EXTERN void vDrvTddcSetPicSize(UINT16 u2Width, UINT16 u2Height);

UINT8 g_u1ParameterChange = 0;
UINT8 g_u1SupportDepthCtrl = 1;
UINT8 g_u1Input3D = 0; 	// 0:TTD, 1:3D
UINT8 g_u1DepthOfField = 0;
UINT8 g_u1DepthOfFieldSWGlobal = 0;
UINT8 g_u1DepthOfFieldSWLocal = 0;
UINT8 g_u1ProtrudeFromScreen = 0x80;
UINT8 g_u1DistanceToTV = 10; // 2~18
UINT8 g_u1LREyeInverse = 0; // 0, 1
INT32 g_i4dLimMax = 255;
INT32 g_i4dLimMin = -255;
INT32 g_i4DisparitySaclarDft = 0;
UINT32 g_u4BlackFromTTD = 0;
UINT32 g_u4BlackFromGbSft = 0;
UINT32 g_u4BlackSingleInv = 0;
UINT32 g_u4BlackBarCnt = 0;
TTD_INFO gTTDInfo;

#if defined(SUPPORT_3D_OVSN_BY_SCL) && defined(SUPPORT_3D_H_OVSN_BY_PSC)
INT32 g_i4LeftOvsnPrev = 0;
INT32 g_i4RightOvsnPrev = 0;
#endif
INT32 g_i4PosLeftPrev = 0;
INT32 g_i4PosRightPrev = 0;

static UINT16 u2W = 0, u2H= 0 ,u2HT =0;

REGTBL_T TTD_INIT[] =
{
    {REGTBL_END, 0x00000000, 0x00000000}
};

/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/

void vDrvTTDSetDepthControl(UINT8 u1OnOff)
{
	g_u1SupportDepthCtrl = u1OnOff;
	vDrvTTDSet3DDepthCtrl(u1OnOff);
    g_u1ParameterChange = 1;
}

void vDrvTTDSetEnable(UINT8 bEnable)
{
    LOG(4,"Set TTD to %s\n",bEnable ? "ON":"OFF");
    // set TTD enable/disable
    if(bEnable == SV_TRUE)
    {        
		vDrvTTDSetBlackEn(bEnable);
		vDrvTTDSetBlackSingleEn(bEnable);

        vIO32WriteFldAlign(TTD_02, SV_ON, TTD_EN);
        vIO32WriteFldAlign(TTD_00, SV_ON, TTD_FSAMPLE_ENABLE);        
    }
    else
    {        
        vIO32WriteFldAlign(TTD_00, SV_OFF, TTD_FSAMPLE_ENABLE);
        vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_EN);
        
        vDrvTTDSetBlackEn(SV_OFF);
        vDrvTTDSetBlackSingleEn(SV_OFF);
        vDrvTTDSetFWOnOff(SV_OFF);
    }	    
}

void vDrvTTDCheckRstCtrl(void)
{

}

void vDrvTTDSetPicSize(UINT16 u2Width, UINT16 u2Height ,UINT16 u2HTotal)
{    
    if ((u2W != u2Width) || (u2H != u2Height) || (u2HT !=u2HTotal))
    {
        u2W = u2Width;
        u2H = u2Height;
        u2HT = u2HTotal;
        //set input size
        vIO32WriteFldMulti(TTDPO_01, P_Fld(u2HT - u2W -1, POST_HS_BNK_WIDTH)| P_Fld(u2HT -1, POST_HS_TOTAL_WIDTH));        
        vIO32WriteFldMulti(TTD_02, P_Fld(u2W, TTD_IMG_WIDTH)| P_Fld(u2H, TTD_IMG_HEIGHT));
                
        vDrvTTDSetPQWindow(u2Width,u2Height);
    }
}

void vDrvTTDSetMJCCtrl(void)
{

}

void vDrvTTDSetCtrl(void)
{    
    static UINT32 u4TTDPrev = 0xFF;

	UINT32 u4TTD = u4DrvTDTVTTDModeQuery();
    E_TD_IN eTTDIn  = TD_TTD_IN(u4TTD);
    E_TD_OUT eTTDOut = TD_TTD_OUT(u4TTD);

    if (u4TTD != u4TTDPrev)
    {
	    LOG(4,"TTDIn = %d, TTDOut = %d\n", eTTDIn, eTTDOut);

        if(eTTDIn == E_TD_IN_2D_P)
        {
            vDrvTTDSet2DInput();            

            #if SUPPORT_3D_FS_DET
            if (eTTDOut == E_TD_OUT_NATIVE)
            {
                vIO32WriteFldAlign(TTD_02, SV_ON , TTD_SRC_3D_MODE);
                vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_SRC_2D_NRPT); 
                vDrvTDDCSetFWOnOff(SV_ON);
            }
            else
            {
                vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_SRC_3D_MODE);
                vIO32WriteFldAlign(TTD_02, ((eTTDOut == E_TD_OUT_3D_LI) ||
                    ((eTTDOut == E_TD_OUT_3D_LI)|| (eTTDOut == E_TD_OUT_NATIVE)))
                    , TTD_SRC_2D_NRPT);   
                vDrvTDDCSetFWOnOff(SV_OFF);
            }
            #else            
            vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_SRC_3D_MODE);
            vIO32WriteFldAlign(TTD_02, ((eTTDOut == E_TD_OUT_3D_LI) ||
                ((eTTDOut == E_TD_OUT_3D_LI)|| (eTTDOut == E_TD_OUT_NATIVE)))
                , TTD_SRC_2D_NRPT);   
            vDrvTDDCSetFWOnOff(SV_OFF);
            #endif            
        }
        else
        {
            vDrvTTDSet3DInput();            
            vIO32WriteFldAlign(TTD_02, SV_ON , TTD_SRC_3D_MODE);
            vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_SRC_2D_NRPT);   
            vDrvTDDCSetFWOnOff(g_u1SupportDepthCtrl);
        }
        
        vIO32WriteFldAlign(TTDIR_00, (eTTDOut == E_TD_OUT_3D_LI) ||
            ((eTTDOut == E_TD_OUT_NATIVE)&&(eTTDIn == E_TD_IN_LI_P)), IR_PR_MODE);
       
        vDrvTTDSetEnable( !((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_NATIVE)));
        
	    u4TTDPrev = u4TTD;
    }
    
}

void vDrvTTDCallback(void *pvArgv) 
{   
	vDrvTTDSetMJCCtrl();
    vDrvTTDSetCtrl();
	vApiRegisterVideoEvent(PE_EVENT_SCALER_V, SV_VP_MAIN, SV_ON);
}

void vDrvTTDInit(void)
{
    UINT16 u2Width  = PANEL_GetPanelWidth();
	UINT16 u2Height = PANEL_GetPanelHeight();
	UINT16 u2HTotal = wDrvGetOutputHTotal();

	vDrvLoadRegTbl(TTD_INIT);
    vDrvTTDSetEnable(SV_OFF);
    vDrvTTDSetFWOnOff(SV_ON);

	u2Width  = ((u2Width + 7) >> 3) << 3;
    u2Height = ((u2Height+7) >> 3) << 3; // alignment 8
    
    vDrvTTDSetPicSize(u2Width, u2Height ,u2HTotal);

	// LLRR Mode always Off
    vIO32WriteFldAlign(TTD_01, SV_OFF, TTD_LLRR_MODE);

    vIO32WriteFldAlign(TTD_01, (IS_LVDS_DISP_3D_POLARIZED), TTD_OUT_LR_DLY);

#if SUPPORT_3D_DEPTH_CONTROL
	g_u1SupportDepthCtrl = 1; 
#else
	g_u1SupportDepthCtrl = 0;  
#endif

    if(u4DrvTDTVModelWithoutMJC())
    {
        vIO32WriteFldAlign(TTDPO_00  , SV_OFF , POST_TTD_BYPASS);
        vIO32WriteFldAlign(TTDPO_00  , SV_ON  , POST_TTD_EN);    
        vIO32WriteFldAlign(OSTG_00   , SV_OFF , C_BYPASS_TTD); 
        #if defined(CC_MT5881)
        #ifdef SMALL_OUTWINDOW_TTD_NEWSETTING    
        // when scaler input <= 192, the video can not dispaly,So as DE FC's suggestion
        vIO32WriteFldAlign(TTDPO_00  , 0x80 ,   SYNC_DELAY_CYCLES); 
        vIO32WriteFldAlign(TTDIR_02  , 0x5F ,   IR_FIFO_LENGTH); 
        #endif
        #endif
    }
    else
    {
        vIO32WriteFldAlign(TTDPO_00  , SV_ON  , POST_TTD_BYPASS);
        vIO32WriteFldAlign(TTDPO_00  , SV_OFF , POST_TTD_EN);    
        vIO32WriteFldAlign(OSTG_00   , SV_ON  , C_BYPASS_TTD);
    }
    
    vIO32WriteFldAlign(TTDPO_01  , 0x2 , POST_VSYNC_MJC);
    vDrvTTDQualityInit();
	fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_TTD,vDrvTTDCallback);
}

void vDrvTTDSuspend(void)
{
    vDrvTTDSetEnable(SV_OFF);
}

void vDrvTTDResume(void)
{
    u2W  = 0;    // reset input size
    u2H  = 0;
    u2HT = 0;
    vDrvTTDInit();
}


//===========================
//  System Setting
//===========================
void vDrvTTDQualityInit(void)
{
    vDrvTTDLSVInit();

    g_i4DisparitySaclarDft = bApiGetQtyDft(QUALITY_DEPTH_TTD_SCALAR, SOURCE_TYPE_TIMING_MAX);

    vDrvTTDSetStereoComfortRange(STEREO_COMFORT_FACTOR);

    // Global Model
    vDrvTTDSetGlobalModel();

    // TFU initial setting
    vIO32WriteFldMulti(TTDDG_04, P_Fld(0x80, DG_TFU_BASE) |
                                 P_Fld(0x6, DG_TEXTURE_WEIGHT) |
                                 P_Fld(0xD, DG_TEXTURE_V) |
                                 P_Fld(0xA, DG_TEXTURE_U) |
                                 P_Fld(0x9, DG_TEXTURE_Y));
    vIO32WriteFldMulti(TTDDG_05, P_Fld(0x1FFF, TFU_THR_HIGH) |
                                 P_Fld(0x3F, TFU_NBR_CLIP_Y) |
                                 P_Fld(0x3F, MAX_TUNE_THR));
    vIO32WriteFldAlign(TTDDG_01, 0xF, DG_LOCAL_WEIGHT);
    vIO32WriteFldAlign(TTDIR_03, 1, IR_BLACK_ASYM);  //for the blackNum use different setting

    // SW register init
    vIO32WriteFldMulti(TTD_FW_REG_00, P_Fld(SV_ON, TTD_FW_EN) |
                                      P_Fld(SV_ON, TTD_FW_FORCE_SC) |
                                      P_Fld(SV_ON, TTD_FW_ADAP_VERT_BASE) |
                                      P_Fld(SV_ON, TTD_FW_ADAP_GM_EN) |
                                      P_Fld(60, TTD_FW_FORCE_SC_CNT));
    vIO32WriteFldMulti(TTD_FW_REG_03, P_Fld(90, TTD_AVG_THLD) |
                                      P_Fld(0, TTD_HIST_THLD) |
                                      P_Fld(70, TTD_DIVER_THLD_H) |
                                      P_Fld(35, TTD_DIVER_THLD_L));
    vIO32WriteFldMulti(TTD_FW_REG_11, P_Fld(0x80, TTD_GLOBAL_GAIN) |
                                      P_Fld(0x80, TTD_LOCAL_GAIN));
    vIO32WriteFldMulti(TTD_FW_REG_12, P_Fld(16, TTD_LOCAL_MAX) |
                                      P_Fld(4, TTD_LOCAL_MIN) |
                                      P_Fld(16, TTD_GLOBAL_MAX) |
                                      P_Fld(4, TTD_GLOBAL_MIN));
    vIO32WriteFldMulti(TTD_FW_REG_13, P_Fld(0x28, TTD_APL_SCN_CHG_TH) |
                                      P_Fld(0xC0, TTD_DIVER_SCN_CHG_TH) |
                                      P_Fld(3, TTD_SCN_CHG_CNT_THR));
    vIO32WriteFldMulti(TTD_FW_REG_15, P_Fld(0x20, TTD_HORI_SM_WEIGHT) |
                                      P_Fld(0x20, TTD_VERT_SM_WEIGHT));
    vIO32WriteFldMulti(TTD_FW_REG_17, P_Fld(0x6, TTD_TFU_GAIN) |
                                      P_Fld(0x80, TTD_IMGSAFETY_SCALE));
}

void vDrvTTDSet3DInput(void)
{
    LOG(1, "vDrvTTDSet3DInput\n");

    g_u1Input3D = 1;
    g_u1ParameterChange = 1;

    vDrvTTDSetTfuBase(0);
    vDrvTTDSetGlobalShift(0x80);
    vDrvTTDSetFWOnOff(SV_OFF);
    vDrvTTDSetDisptScalar(0);
    vDrvTTDSet3DDepthCtrl(g_u1SupportDepthCtrl);

    vIO32WriteFldAlign(TTDIR_04, 0xFF, IR_DISPT_MIN);
    vIO32WriteFldAlign(TTDIR_04, 0xFF, IR_DISPT_MAX);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_GLOBAL_BASE);
    vIO32WriteFldAlign(TTDDG_04, SV_OFF, DG_TFU_EN);
    vIO32WriteFldAlign(TTD_FW_REG_00, SV_OFF, TTD_FW_HIST_EN); 
}

void vDrvTTDSet2DInput(void)
{
    LOG(1, "vDrvTTDSet2DInput\n");

    g_u1Input3D = 0;
    g_u1ParameterChange = 1;

    vDrvTTDSet3DDepthCtrl(SV_OFF);
    vDrvTTDSetDisptScalar(g_i4DisparitySaclarDft);
    vDrvTTDSetTfuBase(0x80);
    vDrvTTDSetGlobalShift(0x80);

    vIO32WriteFldAlign(TTDIR_04, 0x27, IR_DISPT_MIN);
    vIO32WriteFldAlign(TTDIR_04, 0x27, IR_DISPT_MAX);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_GLOBAL_BASE);
    vIO32WriteFldAlign(TTDDG_04, SV_ON, DG_TFU_EN);
    vIO32WriteFldAlign(TTD_FW_REG_00, SV_ON, TTD_FW_HIST_EN); 

    vDrvTTDSetFWOnOff(SV_ON);
}

void vDrvTTDPicChgCheck(void)
{   
	INT32 i4PosLeft, i4PosRight;
    VDP_SCALER_PRM_T* scalerInfo;
    #if (defined(SUPPORT_3D_OVSN_BY_SCL) && defined(SUPPORT_3D_H_OVSN_BY_PSC))
    RPicInfo* pscPicInfo;
    RDispInfo* pscDispInfo;    
    INT32 i4LeftOvsn,i4RightOvsn;
    pscPicInfo = getPscPicInfo();
    pscDispInfo = getPscDispInfo();
    #endif
    
    scalerInfo = getScalerInfo(SV_VP_MAIN);    
    i4PosLeft = (INT32)(scalerInfo->u4OutputX);
    i4PosRight = (INT32)(scalerInfo->u4OutputX) + (INT32)(scalerInfo->u4OutputWidth);
    
    if (g_i4PosLeftPrev != i4PosLeft)
    {
    	g_u1ParameterChange = 1;
    	g_i4PosLeftPrev = i4PosLeft;
    }

    if (g_i4PosRightPrev != i4PosRight)
    {
    	g_u1ParameterChange = 1;
    	g_i4PosRightPrev = i4PosRight;
    }

    #if (defined(SUPPORT_3D_OVSN_BY_SCL) && defined(SUPPORT_3D_H_OVSN_BY_PSC))
    i4LeftOvsn = pscPicInfo->wXOffset;
    i4RightOvsn = pscDispInfo->wWidth - pscPicInfo->wXOffset - pscPicInfo->wTargetW;

    if(g_i4LeftOvsnPrev != i4LeftOvsn)
    {
        g_u1ParameterChange = 1;
        g_i4LeftOvsnPrev = i4LeftOvsn;        
        LOG(6,"TTD Overscan updated Left(%d)\n", g_i4LeftOvsnPrev);
    }

    if(g_i4RightOvsnPrev != i4RightOvsn)
    {
        g_u1ParameterChange = 1;    
        g_i4RightOvsnPrev = i4RightOvsn;
        LOG(6,"TTD Overscan updated Right(%d)\n", g_i4RightOvsnPrev);        
    }
    #endif
}

void vDrvTTDSetLRSyncSwitch(UINT8 bEnable)
{
	vIO32WriteFldAlign(TTD_01, bEnable, TTD_OUT_LR_INV);
}

void vDrvTTDSetLRDataSwitch(UINT8 bEnable)
{
	vIO32WriteFldAlign(TTD_01, bEnable, TTD_DIBR_LR_INV);
}

void vDrvTTDSetFWOnOff(UINT8 bOnOff)
{
    gTTDInfo.ttdFwStatus = bOnOff;
    LOG(0,"TTD FW On/Off = %d\n", bOnOff);
}

void vDrvTDDCSetFWOnOff(UINT8 bOnOff)
{
    gTTDInfo.tddcFwStatus = bOnOff;
    LOG(0,"TDDC FW On/Off = %d\n", bOnOff);
}


//===========================
//  PQ Config Window
//===========================
void vDrvTTDSetPQWindow(UINT16 u2Width,UINT16 u2Height)
{
    gTTDInfo.ttdInputWidth = u2Width;
    gTTDInfo.ttdInputHeight = u2Height;
}


//===========================
//  UI Setting
//===========================
void vDrvTTDUiValueUpdate(void)
{
    UINT32 u4UpdateItem;
	UINT32 u4DepthEn, u4PtruEn;
    UINT32 u4TfuBase;
    UINT32 u4BlackFromTTD, u4BlackFromGbSft, u4BlackSingleInv;
    UINT32 u4ImgSafetySacle;
    INT32 i4Value;
    static UINT32 u4DisparitySaclar = 0, u4GlobalShift = 0x80;

    u4UpdateItem = 0;
    u4DepthEn = 0; u4PtruEn = 0;
    u4TfuBase = 0x80;
    u4BlackFromTTD = 0; u4BlackFromGbSft = 0; u4BlackSingleInv = 0;
    u4ImgSafetySacle = IO32ReadFldAlign(TTD_FW_REG_17, TTD_IMGSAFETY_SCALE);

    if (g_u1Input3D)
    {
        if (g_u1SupportDepthCtrl)
        {
            // Remy TDDC
            u4UpdateItem = UI_SW_GLOBAL | UI_SW_GLOBAL_SFT;
        }
        else
        {
            // Not Support TDDC
            u4UpdateItem = UI_GLOBAL_SFT | UI_BLACK_BAR;
            u4DisparitySaclar = 0;
        }
    }
    else
    {
        // 2D-to-3D
        u4UpdateItem = UI_DSPT_SCALAR | UI_GLOBAL_SFT | UI_BLACK_BAR;
    }

    if (u4UpdateItem & UI_DSPT_SCALAR)
    {
        // Update Disparity Scalar
        i4Value = (g_u1DepthOfField * u4ImgSafetySacle + 64) / 128;
        if (i4Value > u4DisparitySaclar)
        {
            u4DisparitySaclar++;
            u4DepthEn = 1;      
        }
        else if (i4Value < u4DisparitySaclar)
        {
            u4DisparitySaclar--;
            u4DepthEn = 1;
        }
        else
        {
            u4DepthEn = 0;
        }
        vDrvTTDSetDisptScalar(u4DisparitySaclar);

        // Update TFU Base
        u4TfuBase = (g_i4DisparitySaclarDft == 0) ? 
            0x80 : MIN((g_u1DepthOfField * 0x80) / g_i4DisparitySaclarDft, 0x80);
        vDrvTTDSetTfuBase(u4TfuBase);
    }

    if (u4UpdateItem & UI_GLOBAL_SFT)
    {
        // Update Global Shift
        i4Value = g_u1ProtrudeFromScreen;
        i4Value = 0x80 + ((i4Value-0x80)*(INT32)u4ImgSafetySacle) / 128;

        if (i4Value > u4GlobalShift)
        {
            u4GlobalShift++;
            u4PtruEn = 1;       
        }
        else if (i4Value < u4GlobalShift)
        {
            u4GlobalShift--;
            u4PtruEn = 1;
        }
        else
        {
            u4PtruEn = 0;
        }
		vDrvTTDSetGlobalShift(u4GlobalShift);
    }

    if (u4UpdateItem & UI_SW_GLOBAL)
    {
        vIO32WriteFldAlign(SW_TDDC_REG_02, g_u1DepthOfFieldSWGlobal, TDDC_GLOBAL_GAIN_UI);
    }

    if (u4UpdateItem & UI_SW_LOCAL)
    {
        vIO32WriteFldAlign(SW_TDDC_REG_02, g_u1DepthOfFieldSWLocal, TDDC_LOCAL_GAIN_UI);
    }

    if (u4UpdateItem & UI_SW_GLOBAL_SFT)
    {
        vIO32WriteFldAlign(SW_TDDC_REG_02, g_u1ProtrudeFromScreen, TDDC_GLOBAL_SFT_UI);
    }

    if (u4UpdateItem & UI_BLACK_BAR)
    {
        u4BlackFromTTD = (u4DisparitySaclar == 0) ? 0 : ((u4DisparitySaclar + 3) / 4);
        u4BlackFromGbSft = DIFF(128, u4GlobalShift);
        u4BlackFromGbSft = (u4BlackFromGbSft == 0) ? 0 : (u4BlackFromGbSft + 1);
        u4BlackSingleInv = (u4GlobalShift < 0x80) ? SV_FALSE : SV_TRUE;
        vDrvTTDSetBlackBar(u4BlackFromTTD, u4BlackFromGbSft, u4BlackSingleInv);
    }

    g_u1ParameterChange = (u4DepthEn || u4PtruEn) ? SV_TRUE : SV_FALSE;

    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_UI_DBG_EN))
    {
        LOG(0, "==== vDrvTTDUiValueUpdate ====\n");
        LOG(0, "u4DisparitySaclar = %d, u4TfuBase = %d, u4GlobalShift = %d\n", u4DisparitySaclar, u4TfuBase, u4GlobalShift);
        LOG(0, "u4BlackFromTTD = %d, u4BlackFromGbSft = %d, u4BlackSingleInv = %d\n", u4BlackFromTTD, u4BlackFromGbSft, u4BlackSingleInv);
    }
}

void vDrvTTDSetStereoComfortRange(UINT8 u1Factor)
{
	UINT32 u4PixelSize, u4MaxDisparity;
	u4PixelSize = PANEL_Get3DPixelSize();
	u4MaxDisparity = u1Factor * 1000 / u4PixelSize;

	vDrvTTDSetDsprtMax((u4MaxDisparity >> 1));
	vDrvTTDSetDsprtMin((u4MaxDisparity >> 1));
}

void vDrvTTDSetDepthOfField(UINT8 u1Depth)
{
    UNUSED(u1Depth);
    g_u1DepthOfField = wReadQualityTable(QUALITY_DEPTH_TTD_SCALAR);
    g_u1DepthOfFieldSWGlobal = wReadQualityTable(QUALITY_DEPTH_TTDC_GLOBAL);
    g_u1DepthOfFieldSWLocal = wReadQualityTable(QUALITY_DEPTH_TTDC_LOCAL);
    g_u1ParameterChange = 1;
}

void vDrvTTDSetProtrudeFromScreen(UINT8 u1Protrude)
{
    UNUSED(u1Protrude);
    g_u1ProtrudeFromScreen = wReadQualityTable(QUALITY_PROTRUDE_GLOBAL_SFT);
    g_u1ParameterChange = 1;
}

void vDrvTTDSetLREyeInverse(UINT8 u1OnOff)
{
    static UINT8 u1Pre = 0xFF;
    g_u1LREyeInverse = u1OnOff;

    if(g_u1LREyeInverse != u1Pre)
    {
        u1Pre = g_u1LREyeInverse;
        vDrvTTDSetPrInfo(g_u1LREyeInverse);
    }
}

void vDrvTTDSetPrInfo(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? 1 : 0;
    vIO32WriteFldAlign(TTDIR_00, u1OnOff, IR_PR_INFO);
}

void vDrvTTDSetShutterLREyeInverse(UINT8 u1OnOff)
{
    vIO32WriteFldAlign(TTD_01, u1OnOff, TTD_DIBR_LR_INV);
}

//===========================
//  Disparity Control
//===========================
void vDrvTTDSetTfuBase(UINT32 u4Value)
{
    u4Value = MIN(u4Value, 0xFF);
    vIO32WriteFldAlign(TTDDG_04, u4Value, DG_TFU_BASE);
}

void vDrvTTDSetDisptScalar(UINT32 u4Scalar)
{
    u4Scalar = MIN(u4Scalar, 0x3FF);
    vIO32WriteFldAlign(TTDIR_05, u4Scalar, IR_DISPT_SCALAR);
}

void vDrvTTDSetDsprtMin(UINT8 u1Value)
{
    vIO32WriteFldAlign(TTDIR_04, u1Value, IR_DISPT_MIN);
}

void vDrvTTDSetDsprtMax(UINT8 u1Value)
{
    vIO32WriteFldAlign(TTDIR_04, u1Value, IR_DISPT_MAX);
}

void vDrvTTDSetGlobalShift(UINT32 u4Shift)
{
	u4Shift = MIN(u4Shift, 0xFF);
    vIO32WriteFldAlign(TTDIR_04, u4Shift, IR_GLOBAL_SHIFT);
}

void vDrvTTDSetGlobalWeight(UINT8 u1Value)
{
	u1Value = MIN(u1Value, 0x20);
    vIO32WriteFldAlign(TTDDG_01, u1Value, DG_GLOBAL_WEIGHT);
}

void vDrvTTDSetTfuWeight(UINT8 u1Value)
{
	u1Value = MIN(u1Value, 0xF);
    vIO32WriteFldAlign(TTDDG_04, u1Value, DG_TEXTURE_WEIGHT);
}

//===========================
//  Black Bar Control
//===========================
void vDrvTTDSetBlackBar(UINT32 u4BlackFromTTD, UINT32 u4BlackFromGbSft, UINT32 u4BlackSingleInv)
{
    g_u4BlackBarCnt = 0;
    g_u4BlackFromTTD = u4BlackFromTTD;
    g_u4BlackFromGbSft = u4BlackFromGbSft;
    g_u4BlackSingleInv = u4BlackSingleInv;
    vDrvTTDSetBlackBarMainLoop();
}

void vDrvTTDSetBlackBarMainLoop(void)
{
    UINT32 u4BlackNum, u4BlackNumR, u4BlackNumL, u4BlackSingleNum, u4BlackSingleNumR, u4BlackSingleNumL, u4Index;
    static UINT32 u4PreBlackNum[TTD_BLACKBAR_BUF_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
    static UINT32 u4PreBlackSingleNum[TTD_BLACKBAR_BUF_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0}; 
    VRM_INFO_T* inRes;

#if SUPPORT_BLACK_SINGLE_MASK
    u4PreBlackNum[0] = g_u4BlackFromTTD;
    u4PreBlackSingleNum[0] = g_u4BlackFromTTD + g_u4BlackFromGbSft;
#else
    u4PreBlackNum[0] = g_u4BlackFromTTD + g_u4BlackFromGbSft;
    u4PreBlackSingleNum[0] = 0;
#endif

    u4BlackNum = u4DrvGetArrayMaxValue(u4PreBlackNum, TTD_BLACKBAR_BUF_SIZE);
    u4BlackSingleNum = u4DrvGetArrayMaxValue(u4PreBlackSingleNum, TTD_BLACKBAR_BUF_SIZE);

    for (u4Index = TTD_BLACKBAR_BUF_SIZE-1; u4Index > 0; u4Index--)
    {
        u4PreBlackNum[u4Index] = u4PreBlackNum[u4Index-1];
        u4PreBlackSingleNum[u4Index] = u4PreBlackSingleNum[u4Index-1];
    }

    g_u4BlackBarCnt++;

    inRes = VRMGetVRMInfo(SV_VP_MAIN, VRM_MODULE_MIXER);
    u4BlackNumL = u4BlackNum + inRes->u2X;
    u4BlackNumR = u4BlackNum + (PANEL_GetPanelWidth() - inRes->u2Width - inRes->u2X);
    u4BlackSingleNumL = u4BlackSingleNum + inRes->u2X;
    u4BlackSingleNumR = u4BlackSingleNum + (PANEL_GetPanelWidth() - inRes->u2Width - inRes->u2X);  

    vDrvTTDSetBlackNum(u4BlackNumL,u4BlackNumR);
    vDrvTTDSetBlackSingleNum(u4BlackSingleNumL, u4BlackSingleNumR);
    vDrvTTDSetBlackSingleInv(g_u4BlackSingleInv);

    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_UI_DBG_EN))
    {
        LOG(0, "u4BlackNum  = %d, u4BlackSingleNum  = %d, g_u4BlackSingleInv = %d\n", u4BlackNum, u4BlackSingleNum, g_u4BlackSingleInv);
        LOG(0, "u4BlackNumL = %d, u4BlackSingleNumL = %d\n", u4BlackNumL, u4BlackSingleNumL);
        LOG(0, "u4BlackNumR = %d, u4BlackSingleNumR = %d\n", u4BlackNumR, u4BlackSingleNumR);
    }
}


void vDrvTTDSetBlackEn(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? 1 : 0;
	vIO32WriteFldAlign(TTDIR_00, u1OnOff, IR_BLACK_EN);
}

void vDrvTTDSetBlackNum(UINT32 u4ValueL,UINT32 u4ValueR)
{
    UINT32 u4TempL,u4TempR;
    u4TempL = MIN(u4ValueL, 0x7FF);
    u4TempR = MIN(u4ValueR, 0x7FF);
    vIO32WriteFldAlign(TTDIR_01, u4TempL, IR_BLACK_L);
    vIO32WriteFldAlign(TTDIR_01, u4TempR, IR_BLACK_R);
}

void vDrvTTDSetBlackSingleEn(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? 1 : 0;
	vIO32WriteFldAlign(TTDIR_03, u1OnOff, IR_BLACK_SINGLE_EN);
}

void vDrvTTDSetBlackSingleInv(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? 1 : 0;
	vIO32WriteFldAlign(TTDIR_03, u1OnOff, IR_BLACK_SINGLE_INV);
}

void vDrvTTDSetBlackSingleNum(UINT8 u1ValueL,UINT8 u1ValueR)
{
	u1ValueL = MIN(u1ValueL, 0xFF);
	u1ValueR = MIN(u1ValueR, 0xFF);
    vIO32WriteFldAlign(TTDIR_03, u1ValueL, IR_BLACK_SINGLE_WIDTH_L);
    vIO32WriteFldAlign(TTDIR_03, u1ValueR, IR_BLACK_SINGLE_WIDTH_R);
}


UINT32 u4DrvGetArrayMaxValue(UINT32 u4Array[], UINT32 u4Size)
{
	INT32 u4Idx, u4Max;

	u4Max = u4Array[0];
	
	for (u4Idx = 1; u4Idx < u4Size; u4Idx++)
	{
		if (u4Array[u4Idx] > u4Max)
		{
			u4Max = u4Array[u4Idx];
		}		
	}

	return u4Max;
}

void vDrvTTDSetQualityISR(void)
{
    if ((gTTDInfo.ttdFwStatus == SV_ON) && IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_EN))
    {
        SYS_MEASURE_TIME_START(SV_TTD_TIMER);
        vDrvTTDCheckTransition();
        SYS_MEASURE_TIME_END(SV_TTD_TIMER);
        SYS_STORE_TIME_INFO_TO_REG(SV_TTD_TIMER);     
    }
}

void vDrvTTDQualityProc(void)
{
    if (g_u1SupportDepthCtrl) 
    {
        vDrvTTDLSVProc();
    }

    vDrvTTDPicChgCheck();
    
    if (g_u4BlackBarCnt != TTD_BLACKBAR_BUF_SIZE)
    {
        vDrvTTDSetBlackBarMainLoop();
    }

    if (g_u1ParameterChange)
    {
        vDrvTTDCalculateSafetyScale();
        vDrvTTDUiValueUpdate();
    }
}

void vDrvTTDProc(void)
{
    //Keep Config TTD Width / Height , Should be refine later
    VRM_INFO_T in;
    VRM_INFO_T out;

    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_MJC, &in, &out);
    vDrvTTDSetPicSize(out.u2Width, out.u2Height, out.u2HTotal);
    vDrvTTDQualityProc();
}

