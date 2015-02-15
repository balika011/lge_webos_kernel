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
#include "hw_di.h"

#include "drv_ttd.h"
#include "nptv_debug.h"
#include "drv_video.h"
#include "drv_scpos.h"
#include "drv_display.h"
#include "drv_meter.h"

#include "drv_tdtv_feature.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "drv_ttd_RandomForest_FGBG.h"

#include "panel.h"
#include "vdo_misc.h"
#include "mute_if.h"
#include "drv_mjc.h"
#include "hw_mjc.h"
#include "hw_tddc.h"
#include "drv_tdtv.h"

/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
UINT8 g_u1SupportDepthCtrl = 1;
UINT8 g_u1Input3D = 0; 	// 0:TTD, 1:3D
UINT8 g_u1DepthOfField = 0x20;
UINT8 g_u1DepthOfFieldSWGlobal = 0x20;
UINT8 g_u1DepthOfFieldSWLocal = 0x20;
UINT8 g_u1AutoConvEnable = 0;
UINT8 g_u1AutoConvBlkBarUpdate = 0;
UINT8 g_u1AutoConvBlkBarPxl = 0;
UINT8 g_u1ImageSafetyLevel = 0; // 0: Off, 1: Low, 2: Middle, 3: High
UINT8 g_u1DistanceToTV = 10;    // 2~18
INT32 g_i4dLimMax = 255;
INT32 g_i4dLimMin = -255;
UINT32 g_u4ProtrudeFromScreen = 0x200;
UINT32 g_u4AutoConvOutPxl = 0;
TTD_INFO gTTDInfo;

static UINT8 u1IsTTDEnabled = 0;
static UINT8 g_u1ParameterChange = 0;
static UINT8 g_u1LREyeInverse = 0; // 0, 1
static UINT16 u2W = 0, u2H= 0 ,u2HT =0;
static UINT8 IsEnabled = SV_FALSE;
static UINT8 IsModeChangedFlag = SV_FALSE; 
static UINT32 RstCounter = 0;

static INT32 g_i4DisparitySaclarDft = 0;
#if defined(SUPPORT_3D_OVSN_BY_SCL) && defined(SUPPORT_3D_H_OVSN_BY_PSC)
static INT32 g_i4LeftOvsnPrev = 0;
static INT32 g_i4RightOvsnPrev = 0;
#endif
static UINT32 g_u4BlackFromTTD = 0;
static UINT32 g_u4BlackFromGbSft = 0;
static UINT32 g_u4BlackSingleInv = 0;
static UINT32 g_u4BlackBarCnt = 0;
static UINT32 g_u4InvChkV3Sta = SV_OFF;
static E_TD_IN e3DMode = E_TD_IN_END;
static SIVPRandomForest rf;

REGTBL_T TTD_INIT[] =
{
    {REGTBL_END, 0x00000000, 0x00000000}
};

/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/

void vDrvTTDSetDepthControl(UINT8 u1OnOff)
{
    if(SV_TRUE==u4DrvTDTVModelWithoutMJC())
    {
    	g_u1SupportDepthCtrl = SV_OFF;
		vDrvTTDSet3DDepthCtrl(SV_OFF);
    }
    else
    {
    	g_u1SupportDepthCtrl = u1OnOff;
		vDrvTTDSet3DDepthCtrl(u1OnOff);
	}

    g_u1ParameterChange = 1;
}

void vDrvTTDSet444To422En(UINT8 u1OnOff)
{

}

void vDrvTTDSetEnable(UINT8 bEnable)
{
    LOG(4,"Set TTD to %s\n",bEnable ? "ON":"OFF");

    // if Enable/Disable status is changed, set a flag to do MJC output clk reset in output vsync.

    if(IsEnabled != bEnable)
    {
        IsEnabled = bEnable;
        IsModeChangedFlag = SV_TRUE;
    }
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL , SV_ON , MJC_OUTPUT_RST);
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL , SV_OFF, MJC_OUTPUT_RST);    
    u1IsTTDEnabled = bEnable;

    // set TTD enable/disable
    if(bEnable == SV_TRUE)
    {        
        vDrvTTDSetBlackEn(bEnable);
        vDrvTTDSetBlackSingleEn(bEnable);
        vDrvTTDSet444To422En(bEnable);
        vIO32WriteFldAlign(MJC_SYS_DB, (!u1IsTTDEnabled)||(u2W<256)||(u2H<256) , MJC_TTD_BYPASS);        
		vIO32WriteFldAlign(TTD_02, SV_ON, TTD_EN);
        vIO32WriteFldAlign(TTD_00, SV_ON, TTD_FSAMPLE_ENABLE);        
    }
    else
    {        
        vIO32WriteFldAlign(TTD_00, SV_OFF, TTD_FSAMPLE_ENABLE);
        vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_EN);
        vIO32WriteFldAlign(MJC_SYS_DB, (!u1IsTTDEnabled)||(u2W<256)||(u2H<256) , MJC_TTD_BYPASS);        
        vDrvTTDSet444To422En(SV_OFF);
        vDrvTTDSetBlackEn(SV_OFF);
        vDrvTTDSetBlackSingleEn(SV_OFF);
        vDrvTTDSetFWOnOff(SV_OFF);
    }	    
}

void vDrvTTDCheckRstCtrl(void)
{
   if(IsModeChangedFlag == SV_TRUE)    
    {
        if(RstCounter == 0)
        {
            LOG(6,"MJC Clock Rst\n",RstCounter);
            vIO32WriteFldAlign(IO_VIRT + MJC_SYS_00, 4, MJC_RST_CTRL);
        }
        else if(RstCounter > MAX_TTD_RST_COUNTER)        
        {
            vIO32WriteFldAlign(IO_VIRT + MJC_SYS_00, 0, MJC_RST_CTRL);
            IsModeChangedFlag = SV_FALSE;
            RstCounter = 0;
            LOG(6,"Stop MJC Clock Rst\n");
            return;
        }
        RstCounter++;
    }
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
		vIO32WriteFldAlign(MJC_SYS_DB, (!u1IsTTDEnabled)||(u2W<256)||(u2H<256) , MJC_TTD_BYPASS);        
	}
}
void vDrvTTDSetMJCCtrl(void)
{
    static UINT32 u4TTDPrev=0xFF;
    UINT32 u4TTD = u4DrvTDTVTTDModeQuery();
    E_TD_IN   eTTDIn  = TD_TTD_IN(u4TTD);
    E_TD_OUT  eTTDOut = TD_TTD_OUT(u4TTD);    
    
    if (u4TTD != u4TTDPrev)
    {
        _vDrvVideoSetMute(MUTE_MODULE_MJC, SV_VP_MAIN, IO32ReadFldAlign(MJC_MISC_00, MODE_CHG_MUTE_COUNT), SV_TRUE);

        if ((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_3D_FS))
        {
            vIO32WriteFldAlign(MJC_FB_LR3D, SV_ON, MJC_2D_TO_3D_MODE);
        }
        else
        {
            vIO32WriteFldAlign(MJC_FB_LR3D, SV_OFF, MJC_2D_TO_3D_MODE);
        }
        u4TTDPrev = u4TTD;
    }
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

            if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_FS_DET))
            {
                if (eTTDOut == E_TD_OUT_NATIVE)
                {
                    vIO32WriteFldAlign(TTD_02, SV_ON, TTD_SRC_3D_MODE);
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
            }
            else
            {                
                vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_SRC_3D_MODE);
                vIO32WriteFldAlign(TTD_02, ((eTTDOut == E_TD_OUT_3D_LI) ||
                    ((eTTDOut == E_TD_OUT_3D_LI)|| (eTTDOut == E_TD_OUT_NATIVE)))
                    , TTD_SRC_2D_NRPT);   
                vDrvTDDCSetFWOnOff(SV_OFF);
            }
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
       
        vIO32WriteFldAlign(MJC_FB_LR3D, 
            ((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_3D_FS)), MJC_2D_TO_3D_MODE);
        if ((IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_FS_DET)) 
            && ((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_NATIVE)))
        {              
            vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_ON, MJC_CLK_TDDC_EN);        
            vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_ON, MJC_CLK_SVP_EN); 
            vIO32WriteFldAlign(CKGEN_DISP_CKCFG, (IO32ReadFldAlign(CKGEN_DISP_CKCFG, FLD_POCLK_SEL)), FLD_SVPCLK_SEL); 
        }
        else
        {
            vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, (eTTDIn != E_TD_IN_2D_P), MJC_CLK_TDDC_EN);        
            vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, 
                !((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_NATIVE)), MJC_CLK_SVP_EN);       
			vIO32WriteFldAlign(CKGEN_DISP_CKCFG, 0, FLD_SVPCLK_SEL); 
        }
        
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

    IsEnabled = SV_FALSE;    
    IsModeChangedFlag = SV_FALSE; 
    RstCounter = 0;

    vDrvTTDSetEnable(SV_OFF);
    vDrvTTDSetFWOnOff(SV_ON);

	u2Width  = ((u2Width + 7) >> 3) << 3;
    u2Height = ((u2Height+7) >> 3) << 3; // alignment 8
    
    vDrvTTDSetPicSize(u2Width, u2Height ,u2HTotal);

	// LLRR Mode always Off
    vIO32WriteFldAlign(TTD_01,SV_OFF ,TTD_LLRR_MODE);

    vIO32WriteFldAlign(TTD_01,(IS_LVDS_DISP_3D_POLARIZED), TTD_OUT_LR_DLY);

#if SUPPORT_3D_DEPTH_CONTROL
	g_u1SupportDepthCtrl = 1; 
#else
	g_u1SupportDepthCtrl = 0;  
#endif
    vIO32WriteFldAlign(MJC_SYS_DB, SV_ON , MJC_TTD_BYPASS);
    if(u4DrvTDTVModelWithoutMJC())
    {
        vIO32WriteFldAlign(TTDPO_00  , SV_OFF , POST_TTD_BYPASS);
        vIO32WriteFldAlign(TTDPO_00  , SV_ON  , POST_TTD_EN);    
        vIO32WriteFldAlign(OSTG_00   , SV_OFF , C_BYPASS_TTD);        
    }
    else
    {
        vIO32WriteFldAlign(TTDPO_00  , SV_ON  , POST_TTD_BYPASS);
        vIO32WriteFldAlign(TTDPO_00  , SV_OFF , POST_TTD_EN);    
        vIO32WriteFldAlign(OSTG_00   , SV_ON  , C_BYPASS_TTD);
    }
    
#if defined(SUPPORT_MJC)
    if(fgDrvMJCIsNotSupport())
    {
        vIO32WriteFldAlign(TTDPO_01  , 0x2 , POST_VSYNC_MJC);
    }
#else
    vIO32WriteFldAlign(TTDPO_01  , 0x2 , POST_VSYNC_MJC);
#endif

    vDrvTTDQualityInit();

	fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_TTD,vDrvTTDCallback);
}

void vDrvTTDSuspend(void)
{
    vDrvTTDSetEnable(SV_OFF);	
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_OFF, MJC_CLK_TDDC_EN);        
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_OFF, MJC_CLK_SVP_EN);         
}

void vDrvTTDResume(void)
{
    u2W  =0;
    u2H  =0;
    u2HT =0;
    vDrvTTDInit();
}

void vDrvTTDProc(void)
{
    //Keep Config TTD Width / Height , Should be refine later
    VRM_INFO_T in;
    VRM_INFO_T out;

    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_MJC, &in, &out);
	if(u1DrvTDTVCurIs3DMode())
	{
	 	out.u2Width = ALIGN_32_CEIL(out.u2Width);
	}
    vDrvTTDSetPicSize(out.u2Width, out.u2Height, out.u2HTotal);
    vDrvTddcSetPicSize(out.u2Width, out.u2Height);
    vDrvTTDQualityProc();
}


//===========================
//  TTD PQ System Setting
//===========================
void vDrvTTDQualityInit(void)
{
    //vDrvTddcInit(1920, 1080);

    // General setting
    g_i4DisparitySaclarDft = bApiGetQtyDft(QUALITY_DEPTH_TTD_SCALAR, SOURCE_TYPE_TIMING_MAX);
    vIO32WriteFldAlign(TTDVP_04, 2, VP_TRANSIENT_STEP);
    vIO32WriteFldAlign(TTDIR_00, SV_ON, IR_INTP_MODE);

    // Global Model
    vIO32WriteFldAlign(TTDVP_08, 0x4, GM_HORI_SMOOTH_WEIGHT);
    vIO32WriteFldAlign(TTDVP_09, 0x64, GM_VERT_MODEL_GAIN);
    vIO32WriteFldAlign(TTDVP_09, 0x24, GM_VERT_MAX_GAIN);
    vIO32WriteFldAlign(TTDVP_09, 0x24, GM_MINUS_VERT_MIN_GAIN);
    vIO32WriteFldAlign(TTDVP_10, 0x2, GM_VERT_SMOOTH_WEIGHT);
    vIO32WriteFldAlign(TTDVP_12, 0x0, GM_VERT_SLOPE);
    vIO32WriteFldAlign(TTDVP_13, SV_ON, GM_WEIGHT_CTRL_EN);
    vIO32WriteFldAlign(TTDVP_13, 0x300, GM_HORI_WEIGHT);
    vIO32WriteFldAlign(TTDVP_14, 0x100, GM_VERT_WEIGHT);
    vIO32WriteFldAlign(TTDVP_19, SV_OFF, GM_HORI_CTRL_EN);
    vIO32WriteFldAlign(TTDVP_19, 0x0, GM_HORI_CTRL_PT_TOP);
    vIO32WriteFldAlign(TTDVP_19, 0xFF, GM_HORI_CTRL_PT_BTM);
    vIO32WriteFldAlign(TTDDG_11, SV_OFF, DG_1ST_V_GLOBAL_EN);

    // Local Model
    vIO32WriteFldMulti(TTD_04, P_Fld(0x8, TTD_LM_V_WEIGHT) |
                               P_Fld(0x8, TTD_LM_U_WEIGHT));
    vIO32WriteFldMulti(TTDVP_00, P_Fld(0x70, CID_Y_WEIGHT) |
                                 P_Fld(0x90, CID_C_WEIGHT) |
                                 P_Fld(0x40, CID_TEXTURE_GAIN) |
                                 P_Fld(0x40, CID_CHROMA_GAIN));
    vIO32WriteFldAlign(TTDVP_01, 0x20, CID_EDGE_BASE);
    vIO32WriteFldMulti(TTDVP_02, P_Fld(0x1, CID_TEXTURE_RATIO) |
                                 P_Fld(0x16, CID_TEXTURE_SAT));
    vIO32WriteFldAlign(TTDVP_03, SV_OFF, VP_TRANSITION_CTRL_EN);

    // TFU initial setting
    vIO32WriteFldMulti(TTDDG_04, P_Fld(0x80, DG_TFU_BASE) |
                                 P_Fld(0x6, DG_TEXTURE_WEIGHT) |
                                 P_Fld(0xD, DG_TEXTURE_V) |
                                 P_Fld(0xA, DG_TEXTURE_U) |
                                 P_Fld(0x9, DG_TEXTURE_Y));
    vIO32WriteFldMulti(TTDDG_05, P_Fld(0x1FFF, TFU_THR_HIGH) |
                                 P_Fld(0x3F, TFU_NBR_CLIP_Y) |
                                 P_Fld(0x3F, MAX_TUNE_THR));

    // SW register init
    vIO32WriteFldMulti(TTD_FW_REG_00, P_Fld(SV_ON, TTD_FW_ISR_EN) |
                                      P_Fld(SV_ON, TTD_FW_MLOOP_EN) |
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
    vIO32WriteFldAlign(TTD_FW_REG_18, 10, TTD_RGN_NORM_FACT);
}

void vDrvTTDSet3DInput(void)
{
    UINT32 u4TTD = u4DrvTDTVTTDModeQuery();
    E_TD_IN eTTDIn  = TD_TTD_IN(u4TTD);
    E_TD_OUT eTTDOut = TD_TTD_OUT(u4TTD);

    LOG(1, "vDrvTTDSet3DInput\n");

    g_u1Input3D = 1;
    g_u1ParameterChange = 1;

#if SUPPORT_3D_DEPTH_CONTROL
    g_u1SupportDepthCtrl = IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_ECO_IC); 
#else
    g_u1SupportDepthCtrl = 0;  
#endif

    vDrvTTDSetGlobalWeight(0);
    vDrvTTDSetTfuBase(0);
    vDrvTTDSetGlobalShift(0x200);
    vDrvTTDSetFWOnOff(SV_OFF);

    if ((eTTDIn == E_TD_IN_LI_P) && 
        ((eTTDOut == E_TD_OUT_3D_LI) || (eTTDOut == E_TD_OUT_NATIVE)))
    {
        vIO32WriteFldAlign(TTD_02, SRC_3D_TYPE_LI , SRC_3D_TYPE);     
        vIO32WriteFldAlign(TTDDG_00, (IS_PANEL_L12R12) ? 1 : 2, DG_INTP_V_MODE); // 0:32x  1:16x   2:8x
    }
    else
    {
        vIO32WriteFldAlign(TTD_02, SRC_3D_TYPE_FS, SRC_3D_TYPE);
        vIO32WriteFldAlign(TTDDG_00, 1, DG_INTP_V_MODE); // 0:32x  1:16x   2:8x
    }
    vIO32WriteFldAlign(TTDDG_00, 1, DG_INTP_H_MODE); // 0:32x  1:16x   2:8x
    vIO32WriteFldMulti(TTDVP_20, P_Fld(1, CID_V_DOWN2) | P_Fld(1, CID_H_DOWN2));    

    if ((g_u1SupportDepthCtrl) && IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_ECO_IC))
    {   
        vDrvTTDSetLocalWeight(32);
        vDrvTTDSetDisptScalar(DISPARITY_SCALAR_TDDC);
    }
    else
    {
        vDrvTTDSetDisptScalar(0);
    }
    vDrvTTDSet3DDepthCtrl(g_u1SupportDepthCtrl);
    vDrvTTDSetFullScreenMode(SUPPORT_TDDC_FULL_SCREEN_MODE);

    vIO32WriteFldAlign(TTDDG_11, SV_ON, LOCAL_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDDG_11, SV_ON, TFU_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDDG_11, 0x8, FW_LOCAL_GAIN);
    vIO32WriteFldAlign(TTDDG_11, 0x0, FW_TFU_GAIN);
    vIO32WriteFldAlign(TTDIR_04, 0xFF, IR_DISPT_MIN);
    vIO32WriteFldAlign(TTDIR_04, 0xFF, IR_DISPT_MAX);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_GLOBAL_BASE);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_LOCAL_BASE);
    vIO32WriteFldAlign(TTDDG_04, SV_OFF, DG_TFU_EN);
}

void vDrvTTDSet2DInput(void)
{
    LOG(1, "vDrvTTDSet2DInput\n");

    g_u1Input3D = 0;
    g_u1ParameterChange = 1;

    vDrvTTDSet3DDepthCtrl(SV_OFF);
    vDrvTTDSetDisptScalar(g_i4DisparitySaclarDft);
    vDrvTTDSetGlobalWeight(8);
    vDrvTTDSetLocalWeight(24);
    vDrvTTDSetTfuBase(0x80);
    vDrvTTDSetGlobalShift(0x200);
    vDrvTTDSetFullScreenMode(SUPPORT_TTD_FULL_SCREEN_MODE);

    vIO32WriteFldMulti(TTDVP_20, P_Fld(0, CID_V_DOWN2) | P_Fld(0, CID_H_DOWN2));    
    vIO32WriteFldAlign(TTDDG_00, 0, DG_INTP_H_MODE);
    vIO32WriteFldAlign(TTDDG_00, 0, DG_INTP_V_MODE);
    vIO32WriteFldAlign(TTDDG_11, SV_ON, LOCAL_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDDG_11, SV_ON, TFU_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDDG_11, 0x8, FW_LOCAL_GAIN);
    vIO32WriteFldAlign(TTDDG_11, 0x8, FW_TFU_GAIN);
    vIO32WriteFldAlign(TTDIR_04, 0x27, IR_DISPT_MIN);
    vIO32WriteFldAlign(TTDIR_04, 0x27, IR_DISPT_MAX);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_GLOBAL_BASE);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_LOCAL_BASE);
    vIO32WriteFldAlign(TTDDG_04, SV_ON, DG_TFU_EN);

    vDrvTTDSetFWOnOff(SV_ON);
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
    LOG(2,"TTD FW On/Off = %d\n", bOnOff);

    if (TTD_CHECK_INV_VER == TTD_CHECK_INV_V3)
    {
        if ((bOnOff == SV_ON) && (g_u4InvChkV3Sta == SV_OFF))
        {
            loadSIVPRandomForest(&rf);

            if (&rf == NULL)
            {
                LOG(2,"TTD_CHECK_INV_V3 loadSIVPRandomForest Fail\n");
                g_u4InvChkV3Sta = SV_OFF;
            }
            else
            {
                g_u4InvChkV3Sta = SV_ON;
            }
        }
        else if ((bOnOff == SV_OFF) && (g_u4InvChkV3Sta == SV_ON))
        {
            freeSIVPRandomForest(&rf);
            g_u4InvChkV3Sta = SV_OFF;
        }
    }
}

void vDrvTDDCSetFWOnOff(UINT8 bOnOff)
{
    gTTDInfo.tddcFwStatus = bOnOff;
    LOG(2,"TDDC FW On/Off = %d\n", bOnOff);
}


//===========================
//  PQ Config Window
//===========================
void vDrvTTDSetPQWindow(UINT16 u2Width,UINT16 u2Height)
{
    //keep w & h in global
    gTTDInfo.ttdInputWidth = u2Width;
    gTTDInfo.ttdInputHeight = u2Height;

    //set regional histogram setting
	vDrvTTDSetRgnHistogram(u2Width, u2Height);

    //set ROI
    vIO32WriteFldMulti(TTDDG_09, P_Fld((u2Width - 1), DG_ROI_X_END) |
                               P_Fld((u2Height - 1), DG_ROI_Y_END));
}

void vDrvTTDSetRgnHistogram(UINT16 u2Width, UINT16 u2Height)
{
    UINT16 u2VpLastTilePxlWeight, u2Remainder;
    UINT32 u4HistWinSize;

    u2Remainder = u2Height % 16;
    u2VpLastTilePxlWeight = (u2Remainder == 0) ? 1 : (16 / u2Remainder);
    vIO32WriteFldAlign(TTDVP_07, u2VpLastTilePxlWeight, VP_LAST_TILE_PXL_WEIGHT);

    u4HistWinSize = ((u2Height / 16 ) + 7) / 8;
    vIO32WriteFldAlign(TTDDG_00, u4HistWinSize, DG_HIST_WINDOW_SIZE);

    vIO32WriteFldAlign(TTDVP_10, u2Width / 2, GM_VERT_CENTER);
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
    static UINT32 u4DisparitySaclar = 0, u4GlobalShift = 0x200;

    u4UpdateItem = 0;
    u4DepthEn = 0; u4PtruEn = 0;
    u4TfuBase = 0x80;
    u4BlackFromTTD = 0; u4BlackFromGbSft = 0; u4BlackSingleInv = 0;
    u4ImgSafetySacle = IO32ReadFldAlign(TTD_FW_REG_17, TTD_IMGSAFETY_SCALE);

    if (g_u1Input3D)
    {
        if (g_u1SupportDepthCtrl)
        {
            // TDDC
            u4UpdateItem = UI_SW_GLOBAL | UI_SW_LOCAL | UI_SW_GLOBAL_SFT | UI_BLACK_BAR;
            u4DisparitySaclar = (IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH) < TDDC_WIDTH_LIMIT) ?
                                   0 : DIFF(g_u1DepthOfFieldSWGlobal, 0x20) * 8;  //For black bar control
            u4GlobalShift = g_u4ProtrudeFromScreen + g_u1AutoConvBlkBarPxl;
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
        i4Value = g_u4ProtrudeFromScreen;
        i4Value = 0x200 + ((i4Value-0x200)*(INT32)u4ImgSafetySacle) / 128;

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
        vIO32WriteFldAlign(SW_TDDC_REG_02, g_u4ProtrudeFromScreen, TDDC_GLOBAL_SFT_UI);
    }

    if (u4UpdateItem & UI_BLACK_BAR)
    {
        u4BlackFromTTD = (u4DisparitySaclar == 0) ? 0 : ((u4DisparitySaclar + 3) / 4);
        u4BlackFromGbSft = DIFF(0x200, u4GlobalShift);
        u4BlackFromGbSft = (u4BlackFromGbSft == 0) ? 0 : (u4BlackFromGbSft + 1);
        u4BlackSingleInv = (u4GlobalShift < 0x200) ? SV_FALSE : SV_TRUE;
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

void vDrvTTDSetDepthOfField(UINT8 u1Depth)
{
    UNUSED(u1Depth);
    g_u1DepthOfField = wReadQualityTable(QUALITY_DEPTH_TTD_SCALAR);
    g_u1DepthOfFieldSWGlobal = wReadQualityTable(QUALITY_DEPTH_TDDC_GLOBAL);
    g_u1DepthOfFieldSWLocal = wReadQualityTable(QUALITY_DEPTH_TDDC_LOCAL);
    g_u1ParameterChange = 1;
}

void vDrvTTDSetProtrudeFromScreen(UINT8 u1Protrude)
{
    UNUSED(u1Protrude);
    g_u4ProtrudeFromScreen = wReadQualityTable(QUALITY_PROTRUDE_GLOBAL_SFT) << 2;
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
	u1OnOff = (u1OnOff > 0) ? SV_ON : SV_OFF;
    vIO32WriteFldAlign(TTDIR_00, u1OnOff, IR_PR_INFO);
}

UINT32 u4DrvTTDGetPrInfo(void)
{
	return IO32ReadFldAlign(TTDIR_00, IR_PR_INFO);
}

void vDrvTTDSetShutterLREyeInverse(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? SV_ON : SV_OFF;
    vIO32WriteFldAlign(TTD_01, u1OnOff, TTD_DIBR_LR_INV);
}

void vDrvTTDSetDistanceToTV(UINT8 u1Distance)
{
    g_u1DistanceToTV = u1Distance;
    g_u1ParameterChange = 1;
}

void vDrvTTDSetImageSafetyLevel(UINT8 u1Level)
{
    g_u1ImageSafetyLevel = u1Level;
    g_u1ParameterChange = 1;
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

void vDrvTTDSetGlobalShift(UINT32 u4Shift)
{
    u4Shift = MIN(u4Shift, 0x3FF);

    if (LVDS_DISP_3D == LVDS_DISP_3D_POLARIZED_RLRL)
    {
        u4Shift = 0x400 - u4Shift;
    }

    vIO32WriteFldAlign(TTDIR_05, u4Shift, IR_DISPT_POSTOFFSET);
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
    UINT32 u4BlackNum, u4BlackSingleNum, u4Index;
    static UINT32 u4PreBlackNum[TTD_BLACKBAR_BUF_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
    static UINT32 u4PreBlackSingleNum[TTD_BLACKBAR_BUF_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

    if (u4DrvTTDGetFullScreenMode() == SV_TRUE)
    {
        u4PreBlackNum[0] = 0;
        u4PreBlackSingleNum[0] = 0;
    }
    else
    {
#if SUPPORT_BLACK_SINGLE_MASK
        u4PreBlackNum[0] = g_u4BlackFromTTD;
        u4PreBlackSingleNum[0] = g_u4BlackFromTTD + g_u4BlackFromGbSft;
#else
        u4PreBlackNum[0] = g_u4BlackFromTTD + g_u4BlackFromGbSft;
        u4PreBlackSingleNum[0] = 0;
#endif
    }

    u4BlackNum = u4DrvGetArrayMaxValue(u4PreBlackNum, TTD_BLACKBAR_BUF_SIZE);
    u4BlackSingleNum = u4DrvGetArrayMaxValue(u4PreBlackSingleNum, TTD_BLACKBAR_BUF_SIZE);

    for (u4Index = TTD_BLACKBAR_BUF_SIZE-1; u4Index > 0; u4Index--)
    {
        u4PreBlackNum[u4Index] = u4PreBlackNum[u4Index-1];
        u4PreBlackSingleNum[u4Index] = u4PreBlackSingleNum[u4Index-1];
    }

    g_u4BlackBarCnt++;

    vDrvTTDSetBlackNum(u4BlackNum);
    vDrvTTDSetBlackSingleNum(u4BlackSingleNum);
    vDrvTTDSetBlackSingleInv(g_u4BlackSingleInv);

    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_UI_DBG_EN))
    {
        LOG(0, "u4BlackNum = %d, u4BlackSingleNum = %d, g_u4BlackSingleInv = %d\n", u4BlackNum, u4BlackSingleNum, g_u4BlackSingleInv);
    }
}

void vDrvTTDSetBlackEn(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? SV_ON : SV_OFF;
	vIO32WriteFldAlign(TTDIR_00, u1OnOff, IR_BLACK_EN);
}

void vDrvTTDSetBlackNum(UINT32 u4Value)
{
    u4Value = MIN(u4Value, 0x7FF);
    vIO32WriteFldAlign(TTDIR_07, u4Value, IR_BLACK_L);
    vIO32WriteFldAlign(TTDIR_07, u4Value, IR_BLACK_R);
}

void vDrvTTDSetBlackSingleEn(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? SV_ON : SV_OFF;
	vIO32WriteFldAlign(TTDIR_03, u1OnOff, IR_BLACK_SINGLE_EN);
}

void vDrvTTDSetBlackSingleInv(UINT8 u1OnOff)
{
	u1OnOff = (u1OnOff > 0) ? SV_ON : SV_OFF;
	vIO32WriteFldAlign(TTDIR_03, u1OnOff, IR_BLACK_SINGLE_INV);
}

void vDrvTTDSetBlackSingleNum(UINT8 u1Value)
{
    u1Value = MIN(u1Value, 0xFF);
    vIO32WriteFldAlign(TTDIR_03, u1Value, IR_BLACK_SINGLE_WIDTH_L);
    vIO32WriteFldAlign(TTDIR_03, u1Value, IR_BLACK_SINGLE_WIDTH_R);
}

void vDrvTTDSetFullScreenMode(UINT32 u4OnOff)
{
    u4OnOff = (u4OnOff > 0) ? SV_ON : SV_OFF;
    vIO32WriteFldAlign(TTDIR_01, u4OnOff, IR_FSCRN_EN);
}

UINT32 u4DrvTTDGetFullScreenMode(void)
{
    return IO32ReadFldAlign(TTDIR_01, IR_FSCRN_EN);
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

//===========================
//  TTD PQ Check Inverse
//===========================
void vDrvTTDCheckLocalInverseMode(void)
{
    UINT32 u4Inverse, u4ManualInv;

    switch (TTD_CHECK_INV_VER)
    {
        case TTD_CHECK_INV_V1:
            u4Inverse = u4DrvTTDCheckCIDInverseV1();
            break;
        case TTD_CHECK_INV_V2:
            u4Inverse = u4DrvTTDCheckCIDInverseV2();
            break;
        case TTD_CHECK_INV_V3:
            u4Inverse = u4DrvTTDCheckCIDInverseV3();
            break;
        default:
            u4Inverse = SV_FALSE;
            break;
    }

    u4ManualInv = IO32ReadFldAlign(TTD_FW_REG_00, TTD_INVERSE_FW_RESULT);
    gTTDInfo.ttdFgLocalInverse = u4ManualInv ? (!u4Inverse) : u4Inverse;
    
    vIO32WriteFldAlign(TTD_FW_REG_00, u4Inverse, TTD_FW_IS_INVERSE); 
    vIO32WriteFldAlign(TTDVP_05, gTTDInfo.ttdFgLocalInverse, CID_INVERSE_FLAG);

    // Debug message
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_INV_DBG_EN))
    {
        LOG(0, "Check func V%d = %d, Manual inverse = %d, Final inverse = %d\n",
            TTD_CHECK_INV_VER, u4Inverse, u4ManualInv, gTTDInfo.ttdFgLocalInverse);
    }
}

UINT32 u4DrvTTDCheckCIDInverseV1(void)
{
    UINT32 u4Diversity = IO32ReadFldAlign(TTD_FW_REG_09, TTD_DIVER);
    UINT32 avg_thld = IO32ReadFldAlign(TTD_FW_REG_03, TTD_AVG_THLD);
    UINT32 div_thld_h = IO32ReadFldAlign(TTD_FW_REG_03, TTD_DIVER_THLD_H);    
    UINT32 div_thld_l = IO32ReadFldAlign(TTD_FW_REG_03, TTD_DIVER_THLD_L);
    UINT32 avg_cur = IO32ReadFldAlign(TTD_FW_REG_09, TTD_CUR_AVG);
    UINT32 u4Inv, u4Case;

    // V1 is used as default method for customers
    // try to do the decision according to the following rules
    // Check the diversity
    // 1. diversity is small --> no inverse is required
    // 2. diversity is moderate --> use apl method + checking two indices are separated far apart
    // 3. diveristy is large --> use max_index as the background index + check two indices are slight separated?
    // Finally change the control register to control the inverse status
    if (u4Diversity <= div_thld_l)
    {        
        u4Case = 0;
        u4Inv = SV_FALSE;
    }
    else if (u4Diversity <= div_thld_h)
    {
        if (((gTTDInfo.max_index_sec != -1) && (ABS(gTTDInfo.max_index - gTTDInfo.max_index_sec) >= 10)) 
            || ((gTTDInfo.max_index != -1) && (gTTDInfo.max_index_sec == -1)))  // 1. two max 2. only max, no second max
        {            
            u4Case = 1;
            u4Inv = SV_FALSE;
        }
        else
        {            
            u4Case = 2;
            u4Inv = (avg_cur > avg_thld) ? SV_TRUE : SV_FALSE;
        }
    }
    else  // Large diversity
    {
        if ((gTTDInfo.max_index_sec != -1) && (ABS(gTTDInfo.max_index - gTTDInfo.max_index_sec) >= 6))   // 1. two max 
        {            
            u4Case = 3;
            u4Inv = SV_FALSE;
        }
        else
        {            
            u4Case = 4;
            u4Inv = (gTTDInfo.max_index > (avg_thld >> 3)) ? SV_TRUE : SV_FALSE;
        }
    } 
    
    // Debug message
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_INV_DBG_EN))
    {
        LOG(0, "CID case = %d  \n", u4Case);
    }

    return u4Inv;
}

UINT32 u4DrvTTDCheckCIDInverseV2(void)
{
    UINT32 u4Diversity = IO32ReadFldAlign(TTD_FW_REG_09, TTD_DIVER);
    UINT32 u4APL = IO32ReadFldAlign(TTD_FW_REG_09, TTD_CUR_AVG);
    UINT32 u4RgnMax = IO32ReadFldAlign(STA_TTDVP_18, STA_GM_MAX_HIST_DIST)/10;
    UINT32 u4Inv, u4Case, u4Value;

    if (u4Diversity <= 26)
    {
        u4Case = 0;
        u4Inv = (u4APL >= 105) ? SV_TRUE : SV_FALSE;
    }
    else if (u4Diversity <= 43)
    {
        u4Value = DIFF(gTTDInfo.max_index, gTTDInfo.max_index_sec);
        if ((u4Value >= 8) && (u4Value <= 11) && (gTTDInfo.max_index > gTTDInfo.max_index_sec))
        {
            u4Case = 1;
            u4Inv = (u4APL >= 115) ? SV_TRUE : SV_FALSE;
        }
        else
        {
            u4Case = 2;
            u4Inv = (u4APL >= 95) ? SV_TRUE : SV_FALSE;
        }
    }
    else if (u4Diversity <= 50)
    {
        if (u4RgnMax >= 260)
        {
            u4Case = 3;
            u4Inv = (u4APL >= 109) ? SV_TRUE : SV_FALSE;
        }
        else
        {
            u4Case = 4;
            u4Inv = (u4APL >= 117) ? SV_TRUE : SV_FALSE;
        }
    }
    else if (u4Diversity <= 70)
    {
        u4Case = 5;
        u4Inv = (u4APL >= 110) ? SV_TRUE : SV_FALSE;
    }
    else if (u4Diversity <= 105)
    {
        if (u4RgnMax >= 500)
        {
            u4Case = 6;
            u4Inv = (u4APL >= 125) ? SV_TRUE : SV_FALSE;
        }
        else
        {
            u4Case = 7;
            u4Inv = (u4APL >= 118) ? SV_TRUE : SV_FALSE;
        }
    }
    else
    {
        u4Case = 8;
        u4Inv = (u4APL >= 110) ? SV_TRUE : SV_FALSE;
    }

    // Debug message
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_INV_DBG_EN))
    {
        LOG(0, "CID case = %d  \n", u4Case);
    }

    return u4Inv;
}

UINT32 u4DrvTTDCheckCIDInverseV3(void)
{
    static int datum[57];
    UINT32 u4Index, u4MaxbinIdx, u4MaxbinCnt, u4APLbinIdx, u4BinSum, u4Inv;

    // index 0~31: luma histogram
    for (u4Index = 0; u4Index < 32; u4Index++)
    {
        datum[u4Index] = gTTDInfo.ttdHist[u4Index];
    }

    // index 32~39: hue histogram
    for (u4Index = 0; u4Index < 8; u4Index++)
    {
        datum[32+u4Index] = gTTDInfo.ttdHueHist[u4Index];
    }

    // index 40: max histogram bin index
    // index 41: max histogram bin count
    u4MaxbinIdx = 0;
    u4MaxbinCnt = 0;
    for (u4Index = 0; u4Index < 32; u4Index++)
    {
        if (gTTDInfo.ttdHist[u4Index] > u4MaxbinCnt)
        {
            u4MaxbinIdx = u4Index;
            u4MaxbinCnt = gTTDInfo.ttdHist[u4Index];
        }
    }
    datum[40] = u4MaxbinIdx;
    datum[41] = u4MaxbinCnt;

    // index 42: APL
    datum[42] = (int)bDrvGetAPL();

    // index 43: Sum of bin count from bin index 0 to (u4APLbinIdx-1)
    u4APLbinIdx = datum[42]/8;
    u4BinSum = 0;
    for (u4Index = 0; u4Index < u4APLbinIdx; u4Index++)
    {
        u4BinSum += gTTDInfo.ttdHist[u4Index];
    }
    datum[43] = u4BinSum;

    // index 44: Sum of bin count from bin index u4APLbinIdx to 31
    u4BinSum = 0;
    for (u4Index = u4APLbinIdx; u4Index < 32; u4Index++)
    {
        u4BinSum += gTTDInfo.ttdHist[u4Index];
    }
    datum[44] = u4BinSum;

    // index 45
    datum[45] = datum[43] - datum[44];

    // index 46
    datum[46] = datum[43] - (int)gTTDInfo.ttdHist[u4APLbinIdx];

    // index 47
    datum[47] = datum[44] - (int)gTTDInfo.ttdHist[u4APLbinIdx];

    // index 48
    datum[48] = datum[43] - (int)gTTDInfo.ttdHist[u4MaxbinIdx];

    // index 49
    datum[49] = datum[44] - (int)gTTDInfo.ttdHist[u4MaxbinIdx];


    // index 50: Sum of bin count from bin index 0 to (u4MaxbinIdx-1)
    u4APLbinIdx = datum[42]/8;
    u4BinSum = 0;
    for (u4Index = 0; u4Index < u4MaxbinIdx; u4Index++)
    {
        u4BinSum += gTTDInfo.ttdHist[u4Index];
    }
    datum[50] = u4BinSum;

    // index 51: Sum of bin count from bin index u4MaxbinIdx to 31
    u4BinSum = 0;
    for (u4Index = u4MaxbinIdx; u4Index < 32; u4Index++)
    {
        u4BinSum += gTTDInfo.ttdHist[u4Index];
    }
    datum[51] = u4BinSum;

    // index 52
    datum[52] = datum[50] - datum[51];

    // index 53
    datum[53] = datum[50] - (int)gTTDInfo.ttdHist[u4MaxbinIdx];

    // index 54
    datum[54] = datum[51] - (int)gTTDInfo.ttdHist[u4MaxbinIdx];

    // index 55
    datum[55] = datum[50] - (int)gTTDInfo.ttdHist[u4APLbinIdx];

    // index 56
    datum[56] = datum[51] - (int)gTTDInfo.ttdHist[u4APLbinIdx];

    if (g_u4InvChkV3Sta == SV_ON)
    {
        u4Inv = predict(&rf, datum);
    }
    else
    {
        u4Inv = SV_FALSE;
        Printf("u1DrvTTDCheckCIDInverseV3 randam forest is not loaded!!\n");
    }

    if (IO32ReadFldAlign(TTD_FW_REG_18, TTD_FGBG_LOG_DATA))
    {
        Printf("%d ", IO32ReadFldAlign(TTD_FW_REG_18, TTD_FGBG_DESIRE));
        for (u4Index = 0 ; u4Index < 57; u4Index++)
        {
            Printf("%d ", datum[u4Index]);
        }
        Printf("\n");
    }

    return u4Inv;

}


//===========================
//  PQ Setting
//===========================
void vDrvTTDSetGlobalWeight(UINT8 u1Value)
{
    u1Value = MIN(u1Value, 0x20);
    vIO32WriteFldAlign(TTDDG_01, u1Value, DG_GLOBAL_WEIGHT);
}

void vDrvTTDSetLocalWeight(UINT8 u1Value)
{
    u1Value = MIN(u1Value, 0x20);
    vIO32WriteFldAlign(TTDDG_01, u1Value, DG_LOCAL_WEIGHT);    
}

void vDrvTTDSetTfuWeight(UINT8 u1Value)
{
    u1Value = MIN(u1Value, 0xF);
    vIO32WriteFldAlign(TTDDG_04, u1Value, DG_TEXTURE_WEIGHT);
}

void vDrvTTDCheckQtyUpdate(void)
{
    static UINT32 u4LoopCnt = 0;    

    if ((u4DrvTTDIsSceneChanged() == SV_TRUE) || 
        (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_FORCE_SC) && (u4LoopCnt >= IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_FORCE_SC_CNT))))
    {
        gTTDInfo.ttdQtyUpdate = SV_TRUE;
        u4LoopCnt = 0;
    }
    else
    {
        gTTDInfo.ttdQtyUpdate = SV_FALSE;
        u4LoopCnt++;
    }
}

void vDrvTTDSetQualityISR(void)
{
    if ((gTTDInfo.ttdFwStatus == SV_ON) && IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_ISR_EN))
    {
        SYS_MEASURE_TIME_START(SV_TTD_TIMER);

        vDrvTTDCheckQtyUpdate();
        vDrvTTDSetGlobalModel();

        if (gTTDInfo.ttdQtyUpdate)
        {
            vDrvTTDCalImgChar();
            vDrvTTDAdaptiveWeight();
        }

        SYS_MEASURE_TIME_END(SV_TTD_TIMER);
        SYS_STORE_TIME_INFO_TO_REG(SV_TTD_TIMER);     
    }

    if (gTTDInfo.tddcFwStatus == SV_ON)
    {        
        SYS_MEASURE_TIME_START(SV_DEPTH_CTRL_TIMER);
        vDrvTddcFwTrigger();
        SYS_MEASURE_TIME_END(SV_DEPTH_CTRL_TIMER);
        SYS_STORE_TIME_INFO_TO_REG(SV_DEPTH_CTRL_TIMER);  
    }
}

void vDrvTTDQualityProc(void)
{
    if ((gTTDInfo.ttdFwStatus == SV_ON) && IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_MLOOP_EN))
    {
        if (gTTDInfo.ttdQtyUpdate)
        {
            vDrvTTDCheckLocalInverseMode();
        }
    }

    if (gTTDInfo.tddcFwStatus == SV_ON)
    {        
        if (g_u1SupportDepthCtrl) 
        {
            if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_IN_ISR) == SV_FALSE)   
            {
                vDrvTddcProc();
            }
        }
    }

    if (g_u4BlackBarCnt != TTD_BLACKBAR_BUF_SIZE)
    {
        vDrvTTDSetBlackBarMainLoop();
    }

    if (g_u1ParameterChange)
    {
        vDrvTTDCalculateSafetyScale();
    }

    if (g_u1ParameterChange || g_u1AutoConvBlkBarUpdate)
    {
        vDrvTTDUiValueUpdate();
    }
}


//===========================
//  TDDC
//===========================
void vDrvTTDSet3DDepthCtrl(UINT8 u1OnOff)
{  
    vDrvTddcSetCtrl(u1OnOff);
    LOG(2, "Set 3D depth control enable  %d\n", u1OnOff);       
}

UINT8 u1DrvTTDGet3DDepthCtrl(void)
{    
    return IO32ReadFldAlign(TDDC_00, C_TDDC_EN);
}

static void vDrvTTDLSVUpdateHW(void)
{
    UINT32 u4GlobalShift, u4BlackFromGbSft, u4BlackSingleInv;
    INT32 i4GlobalShiftUI, i4GlobalGainUI, i4Value, i4NewDV;

    i4GlobalShiftUI = IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_GLOBAL_SFT_UI);
    i4GlobalGainUI = IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_GLOBAL_GAIN_UI);

    u4GlobalShift = i4GlobalShiftUI + (cur_convg)*(i4GlobalGainUI-0x20)/0x40;   // divide by 0x40=0x20*2
    i4Value = u4GlobalShift;
    i4NewDV = cur_convg + (i4Value - 128)*2;

    if (i4NewDV > 2*g_i4dLimMax)
    {
        i4Value = (g_i4dLimMax + 128) - cur_convg/2;
    }
    else if (i4NewDV < 2*g_i4dLimMin)
    {
        i4Value = (g_i4dLimMin + 128) - cur_convg/2;
    }

    u4BlackSingleInv = (i4Value < 0x80) ? SV_FALSE : SV_TRUE;
    u4BlackFromGbSft = DIFF(i4Value, 128);

    vDrvTTDSetBlackBar(0, u4BlackFromGbSft, u4BlackSingleInv);
    vDrvTTDSetGlobalShift((UINT32)i4Value);
}

UINT8 u1DrvGetDumpDramInfo(DUMP_3D_INFO_T *pDumpInfo)
{       
    UINT32 u4BufSize;
    static UINT32 u4DesAddrL = 0;
    static UINT32 u4DesAddrR = 0;

#if DRAM_DUMP_DBG  
    FBM_POOL_T* prPool;
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_TOOL, NULL);

    // in case there is no suitable space allocated in FBM, add new way to force allocate (known) unused space for dump usage
    if(u4IO32Read4B(SWRW_08) != 0)
    {
        prPool->u4Addr = u4IO32Read4B(SWRW_08);
    }

    if (prPool->u4Size > pDumpInfo->u4DumpSize)
    {
       	u4DesAddrL = VIRTUAL(prPool->u4Addr);
        u4DesAddrR = u4DesAddrL + (pDumpInfo->u4DumpSize/2);       
        u4BufSize = prPool->u4Size;
        
        vIO32Write4B(SWRW_01, prPool->u4Addr);
        vIO32Write4B(SWRW_02, u4BufSize);
    }    
    else
#endif        
    {
        if ((u4DesAddrL == 0) && (u4DesAddrR == 0))
        {
            u4DesAddrL = (UINT32)x_mem_alloc(LSV_DUMP_MAX_SIZE);          
            u4DesAddrR = (UINT32)x_mem_alloc(LSV_DUMP_MAX_SIZE);  
        }     
        u4BufSize = LSV_DUMP_MAX_SIZE*2;
    }
    
    if ((u4BufSize < pDumpInfo->u4DumpSize) 
        || (u4DesAddrL == 0) || (u4DesAddrR == 0))
    {            
        LOG(5,"Scaler Dump Dram Error  %d   \n", pDumpInfo->u4DumpSize);    
        return SV_FALSE; 
    }          
    
    pDumpInfo->u4DesAddrL = u4DesAddrL;
    pDumpInfo->u4DesAddrR = u4DesAddrR;  
    
    return SV_TRUE;
}

UINT8 u1DrvGetDumpInfo(DUMP_3D_INFO_T *pDumpInfo)
{       
    INT32 iStep_h;

    if (IS_MIB_FRC(VDP_1))
    {       
        pDumpInfo->u4SrcWidth = IO32ReadFldAlign(MCVP_KC_0A, HDEW);
        pDumpInfo->u4SrcHeight = IO32ReadFldAlign(MCVP_KC_0A, VDEW);   
        pDumpInfo->u4SrcAddr = VIRTUAL(IO32ReadFldAlign(MCVP_DRAM_0A, DA_ADDR_BASE_MSB_Y)<<11);
        pDumpInfo->u4Index = (vDrvDIGetDADebugStatus(0)>>9)&7; // Bit[9:11]
        pDumpInfo->u4Pitch = IO32ReadFldAlign(MCVP_DRAM_09, DA_DRAM_LINE_LENGTH)*256;
        pDumpInfo->u4YFrmNum = (IO32ReadFldAlign(MCVP_DRAM_08, DA_FRAME_NUM_Y)+1)*2;
    }
    else
    {
        pDumpInfo->u4SrcWidth = IO32ReadFldAlign(SCPIP_SCCTRL1_1C, SCCTRL1_1C_SC_DRAM_W_WIDTH_1);
        pDumpInfo->u4SrcHeight = IO32ReadFldAlign(SCPIP_SCCTRL1_1C, SCCTRL1_1C_SC_DRAM_W_HEIGHT_1);   
        pDumpInfo->u4SrcAddr = VIRTUAL(IO32ReadFldAlign(SCPIP_DRAM_M_04, DRAM_M_04_DA_ADDR_BASE_MSB_Y)<<11);
        pDumpInfo->u4Index = IO32ReadFldAlign(SCPIP_SCCTRL1_1B, SCCTRL1_1B_SC_STA_DRAM_WPTR_1);          
        pDumpInfo->u4Pitch = (IO32ReadFldAlign(SCPIP_DRAM_M_00, DRAM_M_00_DA_DRAM_LINE_PITCH)<<4);
        pDumpInfo->u4YFrmNum = (IO32ReadFldAlign(SCPIP_DRAM_M_00, DRAM_M_00_DA_FRAME_NUM)+1)*2;
    }
    
    if (pDumpInfo->u4SrcWidth >= ((LSV_DES_WIDTH+128)*2)) 
    {
        pDumpInfo->u4DesWidth = LSV_DES_WIDTH*2;     
        iStep_h = 8;    //1024
    }
    else if (pDumpInfo->u4SrcWidth >= (LSV_DES_WIDTH+128)) 
    {        
        pDumpInfo->u4DesWidth = LSV_DES_WIDTH;    
        iStep_h = 4;    //512
    }
    else if (pDumpInfo->u4SrcWidth >= (512+64))
    {        
        pDumpInfo->u4DesWidth = 512;             
        iStep_h = 3;    //384
    }
    else
    {        
        pDumpInfo->u4DesWidth = 256;             
        iStep_h = 1;    //128
    }
    
    if (pDumpInfo->u4SrcHeight >= (512+256)) 
    {
        pDumpInfo->u4DesHeight = LSV_DES_HEIGHT;                            
        pDumpInfo->u4Subsample = 32;    //512
    }
    else if (pDumpInfo->u4SrcHeight >= (256+64)) 
    {        
        pDumpInfo->u4DesHeight = LSV_DES_HEIGHT;                            
        pDumpInfo->u4Subsample = 16;    //256
    }        
    else
    {        
        pDumpInfo->u4DesHeight = LSV_DES_HEIGHT/2;                            
        pDumpInfo->u4Subsample = 16;     //128
    }
        
    vDrvLSVModeChange(pDumpInfo->u4DesWidth, pDumpInfo->u4DesHeight, iStep_h);

    // Error check
    if (pDumpInfo->u4SrcHeight < pDumpInfo->u4DesHeight*pDumpInfo->u4Subsample) 
    {
        LOG(5, "LSV Dump H Error  %d   \n", pDumpInfo->u4DesHeight);    
        pDumpInfo->u4DesHeight = pDumpInfo->u4SrcHeight/pDumpInfo->u4Subsample;
    }    

    if (pDumpInfo->u4SrcWidth < pDumpInfo->u4DesWidth)
    {
        LOG(5, "LSV Dump W Error  %d   \n", pDumpInfo->u4DesWidth);    
        pDumpInfo->u4DesWidth = pDumpInfo->u4SrcWidth;
    }    
  
    pDumpInfo->u4SrcStartX = 
        (((pDumpInfo->u4SrcWidth - pDumpInfo->u4DesWidth)/2)>>2)<<2;        // 4 byte align
    pDumpInfo->u4SrcStartY = 
        (((pDumpInfo->u4SrcHeight - pDumpInfo->u4DesHeight*pDumpInfo->u4Subsample)/2)>>4)<<4;

    // Only dump Y 8bit data
    pDumpInfo->u4DumpSize = pDumpInfo->u4DesWidth*pDumpInfo->u4DesHeight*2;

    if (u1DrvGetDumpDramInfo(pDumpInfo) == SV_TRUE)
    {
        LOG(5, "==== LSV Dump mode ==== \n");
        LOG(5, "Src    %d   %d   \t0x%08x   \tPitch   %d   FrameNum  %d\n", 
            pDumpInfo->u4SrcWidth, pDumpInfo->u4SrcHeight, pDumpInfo->u4SrcAddr,
            pDumpInfo->u4Pitch, pDumpInfo->u4YFrmNum);       
        LOG(5, "Des    %d   %d   \t0x%08x   0x%08x\n", 
            pDumpInfo->u4DesWidth, pDumpInfo->u4DesHeight, 
            pDumpInfo->u4DesAddrL, pDumpInfo->u4DesAddrR);       
        LOG(5, "Start    X %d  Y %d   Subsample  %d  DumpSize  0x%08x  Scaler Ratio %d \n", 
            pDumpInfo->u4SrcStartX, pDumpInfo->u4SrcStartY, 
            pDumpInfo->u4Subsample, pDumpInfo->u4DumpSize, iSclRatio);        
        LOG(5, "depth_width %d   depth_height %d  \n", 
            m_depth_width, m_depth_height); 

        return SV_TRUE;
    }
    else
    {
        LOG(5, "LSV Get Dump Info Fail\n");    
        return SV_FALSE;
    }
}

void vDrvDumpNormal(DUMP_3D_INFO_T *pInfo)
{
    UINT32 u4SrcL, u4SrcR, u4DesL, u4DesR;
    UINT32 u4HPage;  	
    UINT32 i, j, temp, u4Pos;

    u4HPage = (pInfo->u4Pitch+SCL_H_PXL_MASK)>>SCL_H_PXL_BIT;
    
    for (j=0; j < pInfo->u4DesHeight; j++)
    {
        u4DesL = pInfo->u4DesAddrL+pInfo->u4DesWidth*j;
        u4DesR = pInfo->u4DesAddrR+pInfo->u4DesWidth*j;
        
        temp = (pInfo->u4SrcStartY+j*pInfo->u4Subsample)*pInfo->u4YFrmNum;        
 
        u4SrcL = pInfo->u4SrcAddr + ((((temp+0)>>SCL_V_PXL_BIT)*u4HPage)<<SCL_BLK_BIT) 
                    + (((temp+0)&SCL_V_PXL_MASK)<<SCL_H_PXL_BIT);
        u4SrcR = pInfo->u4SrcAddr + ((((temp+1)>>SCL_V_PXL_BIT)*u4HPage)<<SCL_BLK_BIT) 
                    + (((temp+1)&SCL_V_PXL_MASK)<<SCL_H_PXL_BIT);        

        temp = pInfo->u4SrcStartX;
        
        for (i=0; i < pInfo->u4DesWidth; i+=4)
        {           
            u4Pos = ((temp>>SCL_H_PXL_BIT)<<SCL_BLK_BIT)+(temp&SCL_H_PXL_MASK);
            *(UINT32*)(u4DesL+i) = *(UINT32*)(u4SrcL+u4Pos);
            *(UINT32*)(u4DesR+i) = *(UINT32*)(u4SrcR+u4Pos);    
            temp += 4;
        }
    }
}

UINT8 u1DrvLSVGetDumpInfo(DUMP_3D_INFO_T *pDumpInfo)
{        
    E_TD_IN  eSclIn, eMibIn;
    
    eMibIn = TD_MIB_IN(u4DrvTDTVDIModeQuery());
    eSclIn = TD_SCL_IN(u4DrvTDTVScalerModeQuery());

    if (IS_MIB_FRC(VDP_1)) 
    {             
        if ((eSclIn != (E_TD_IN)E_TD_IN_LI_P)
            && (eSclIn != (E_TD_IN)E_TD_IN_FS_P))
        {
            LOG(2, "LSV Not support 3D mode : %d\n", eSclIn);
            return SV_FALSE;
        }

        if (e3DMode != eMibIn)
        {
            e3DMode = eMibIn;
            LOG(4, "LSV 3D mode : MIB IN %d  \n", eMibIn);
        }
    }
    else
    {
        return SV_FALSE;
    }
    
    return u1DrvGetDumpInfo(pDumpInfo);
}

void vDrvLSVDump(DUMP_3D_INFO_T *pInfo)
{
    if (u1DrvTTDGet3DDepthCtrl())
    {
        HalFlushInvalidateDCache(); 
        vDrvDumpNormal(pInfo);

    #if DRAM_DUMP_DBG
        vIO32WriteFldAlign(SWRW_03, pInfo->u4DesHeight*2, DUMP_CTRL_HEIGHT);
        vIO32WriteFldAlign(SWRW_03, pInfo->u4DesWidth, DUMP_CTRL_WIDTH);
        vIO32WriteFldAlign(SWRW_03, DUMP_FMT_Y_8BIT, DUMP_CTRL_FORMAT);    
        vIO32WriteFldAlign(SWRW_04, pInfo->u4DumpSize, DUMP_CTRL_SIZE); 
    #endif
    }
}


void vDrvTTDLSVProc(void)
{    
    HAL_TIME_T TimeStart, TimeNow, TimeDelta;
    DUMP_3D_INFO_T stInfo;
    static UINT8 u1CalCount = 0;

    if (u1DrvTTDGet3DDepthCtrl() == SV_FALSE)
    {
        return;
    }
    
    if (u1Lsvinit == SV_FALSE) 
    {        
        vDrvTTDSet3DDepthCtrl(SV_OFF);
        LOG(0, "LSV Init failed, Stop LSV\n");
        return;
    }

    if (u1DrvLSVGetDumpInfo(&stInfo) == SV_FALSE) 
    {        
        LOG(6, "LSV Get Dump Info failed, Stop LSV\n");
        return;
    }
          
    LOG(5, "==== LSV Proc ====  %d\n", stInfo.u4Index);
    
    if (stInfo.u4Index == 1)
    {            
        if (u1CalCount >= u4CalLoop)
        {      
            u1CalCount = 0;
        }
        else
        {
            u1CalCount++;
            return;
        }
        
        HAL_GetTime(&TimeStart);
        
        vDrvLSVDump(&stInfo);
 
        HAL_GetTime(&TimeNow);
        HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
        LOG(4, "Dump time  %d.%06d sec.\n", TimeDelta.u4Seconds, TimeDelta.u4Micros);
    }         
    else if (stInfo.u4Index == 0) 
    {           
        if (u1CalCount != 0)
        {
            return;
        }
        
        HAL_GetTime(&TimeStart);
        
        vDrvTTDLumaSum(stInfo.u4DesWidth*stInfo.u4DesHeight, (UINT8 *)stInfo.u4DesAddrL, (UINT8 *)stInfo.u4DesAddrR);
        
        HAL_GetTime(&TimeNow);
        HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
        LOG(4, "LumaSum time  %d.%06d sec.\n", TimeDelta.u4Seconds, TimeDelta.u4Micros);       

        HAL_GetTime(&TimeStart);
        
        vDrvTTDLSVCalculate((UINT8 *)stInfo.u4DesAddrL, (UINT8 *)stInfo.u4DesAddrR);
        
        HAL_GetTime(&TimeNow);
        HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
        LOG(4, "Calculate time  %d.%06d sec.\n", TimeDelta.u4Seconds, TimeDelta.u4Micros);       
    }

    vDrvTTDLSVUpdateHW();
}

#ifdef CC_SUPPORT_STR
void vDrvTddcResume(void)
{        
    extern void vDrvTddcInitHw(void);
    vDrvTddcInitHw();
}
#endif


