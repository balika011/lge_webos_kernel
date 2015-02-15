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

#include "drv_tdtv_feature.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "panel.h"
#include "vdo_misc.h"

#include "mute_if.h"

#ifdef CC_MT5398
#include "drv_mjc.h"
#include "hw_mjc.h"
#include "hw_tddc.h"
#endif

/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
UINT8 g_u1SupportDepthCtrl = 1;
UINT8 g_u1Input3D = 0; 	// 0:TTD, 1:3D
UINT8 g_u1DepthOfField = 0;
UINT8 g_u1DepthOfFieldSWGlobal = 0;
UINT8 g_u1DepthOfFieldSWLocal = 0;
UINT8 g_u1ProtrudeFromScreen = 0x80;
UINT8 g_u1AutoConvEnable = 0;
UINT8 g_u1AutoConvBlkBarUpdate = 0;
UINT8 g_u1AutoConvBlkBarPxl = 0;
UINT8 g_u1ImageSafetyLevel = 0; // 0: Off, 1: Low, 2: Middle, 3: High
UINT8 g_u1DistanceToTV = 10;    // 2~18
INT32 g_i4dLimMax = 255;
INT32 g_i4dLimMin = -255;
UINT32 g_u4AutoConvOutPxl = 0;
TTD_INFO gTTDInfo;

static UINT8 u1IsTTDEnabled = 0;
static UINT8 g_u1ParameterChange = 0;
static UINT8 g_u1LREyeInverse = 0; // 0, 1
static UINT16 u2W = 0, u2H= 0 ,u2HT =0;
#if defined(CC_MT5398)
static UINT8 IsEnabled = SV_FALSE;
static UINT8 IsModeChangedFlag = SV_FALSE; 
static UINT32 RstCounter = 0;
#endif
static INT32 g_i4DisparitySaclarDft = 0;
#if defined(SUPPORT_3D_OVSN_BY_SCL) && defined(SUPPORT_3D_H_OVSN_BY_PSC)
static INT32 g_i4LeftOvsnPrev = 0;
static INT32 g_i4RightOvsnPrev = 0;
#endif
static INT32 g_i4PosLeftPrev = 0;
static INT32 g_i4PosRightPrev = 0;
static UINT32 g_u4BlackFromTTD = 0;
static UINT32 g_u4BlackFromGbSft = 0;
static UINT32 g_u4BlackSingleInv = 0;
static UINT32 g_u4BlackBarCnt = 0;
static E_TD_IN e3DMode = E_TD_IN_END;

REGTBL_T TTD_INIT[] =
{
    {REGTBL_END, 0x00000000, 0x00000000}
};

/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/

void vDrvTTDSetDepthControl(UINT8 u1OnOff)
{
#ifdef CC_MT5398
    if(SV_TRUE==u4DrvTDTVModelWithoutMJC())
    {
    	g_u1SupportDepthCtrl = SV_OFF;
		vDrvTTDSet3DDepthCtrl(SV_OFF);
    }
    else
#endif
    {
    	g_u1SupportDepthCtrl = u1OnOff;
		vDrvTTDSet3DDepthCtrl(u1OnOff);
	}

    g_u1ParameterChange = 1;
}

void vDrvTTDSet444To422En(UINT8 u1OnOff)
{
    vIO32WriteFldAlign(TTDPO_00, u1OnOff, TTD_444_422_444_EN);
    vIO32WriteFldAlign(TTDPO_00, u1OnOff, TTD_444_422_LPF_EN);    
}

void vDrvTTDSetEnable(UINT8 bEnable)
{
    LOG(4,"Set TTD to %s\n",bEnable ? "ON":"OFF");

    // if Enable/Disable status is changed, set a flag to do MJC output clk reset in output vsync.

#if defined(CC_MT5398)
    if(IsEnabled != bEnable)
    {
        IsEnabled = bEnable;
        IsModeChangedFlag = SV_TRUE;
    }

    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL , SV_ON , MJC_OUTPUT_RST);
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL , SV_OFF, MJC_OUTPUT_RST);    
#endif

    u1IsTTDEnabled = bEnable;

    // set TTD enable/disable
    if(bEnable == SV_TRUE)
    {        
		vDrvTTDSetBlackEn(bEnable);
		vDrvTTDSetBlackSingleEn(bEnable);
    	vDrvTTDSet444To422En(bEnable);

#if defined(CC_MT5398)
        vIO32WriteFldAlign(MJC_SYS_DB, (!u1IsTTDEnabled)||(u2W<256)||(u2H<256) , MJC_TTD_BYPASS);        
#endif        
        vIO32WriteFldAlign(TTD_02, SV_ON, TTD_EN);
        vIO32WriteFldAlign(TTD_00, SV_ON, TTD_FSAMPLE_ENABLE);        
    }
    else
    {        
        vIO32WriteFldAlign(TTD_00, SV_OFF, TTD_FSAMPLE_ENABLE);
        vIO32WriteFldAlign(TTD_02, SV_OFF, TTD_EN);
        
#if defined(CC_MT5398)
        vIO32WriteFldAlign(MJC_SYS_DB, (!u1IsTTDEnabled)||(u2W<256)||(u2H<256) , MJC_TTD_BYPASS);        
#endif           
    	vDrvTTDSet444To422En(SV_OFF);
        vDrvTTDSetBlackEn(SV_OFF);
        vDrvTTDSetBlackSingleEn(SV_OFF);
        vDrvTTDSetFWOnOff(SV_OFF);
    }	    
}

void vDrvTTDCheckRstCtrl(void)
{
#if defined(CC_MT5398)
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
#endif    
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

#if defined(CC_MT5398)        
        vIO32WriteFldAlign(MJC_SYS_DB, (!u1IsTTDEnabled)||(u2W<256)||(u2H<256) , MJC_TTD_BYPASS);        
#endif

    }
}

void vDrvTTDSetMJCCtrl(void)
{
#if defined(CC_MT5398)
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
#endif        
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
        
        if ((eTTDIn == E_TD_IN_LI_P) && 
            ((eTTDOut == E_TD_OUT_3D_LI) || (eTTDOut == E_TD_OUT_NATIVE)))
		{
            vIO32WriteFldAlign(TTD_02, SRC_3D_TYPE_LI , SRC_3D_TYPE);            
		}
        else
        {
            vIO32WriteFldAlign(TTD_02, SRC_3D_TYPE_FS, SRC_3D_TYPE);
        }

        vIO32WriteFldAlign(TTDIR_00, (eTTDOut == E_TD_OUT_3D_LI) ||
            ((eTTDOut == E_TD_OUT_NATIVE)&&(eTTDIn == E_TD_IN_LI_P)), IR_PR_MODE);
       
#if defined(CC_MT5398)
        vIO32WriteFldAlign(MJC_FB_LR3D, 
            ((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_3D_FS)), MJC_2D_TO_3D_MODE);

        #if SUPPORT_3D_FS_DET        
        vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_ON, MJC_CLK_O_EN);        
        vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_ON, MJC_CLK_SVP_EN);         
        #else
        vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, 
            (eTTDIn != E_TD_IN_2D_P), MJC_CLK_O_EN);
        
        vIO32WriteFldAlign(MJC_SYS_CLK_CTRL , 
            !((eTTDIn == E_TD_IN_2D_P) && (eTTDOut == E_TD_OUT_NATIVE)), MJC_CLK_SVP_EN);         
        #endif
#endif

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

#if defined(CC_MT5398)
    IsEnabled = SV_FALSE;    
    IsModeChangedFlag = SV_FALSE; 
    RstCounter = 0;
#endif

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

#if defined(CC_MT5398)
    vIO32WriteFldAlign(MJC_SYS_DB, SV_ON , MJC_TTD_BYPASS);
#endif

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
    
#if !defined(CC_MT5398)
    vIO32WriteFldAlign(TTDPO_01  , 0x2 , POST_VSYNC_MJC);
#endif   

    vIO32WriteFldAlign(TTDPO_02  , 1   , POST_TTD_WIDTH_8X_EN);
    
    vDrvTTDQualityInit();

	fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_TTD,vDrvTTDCallback);
}

void vDrvTTDSuspend(void)
{
    vDrvTTDSetEnable(SV_OFF);
#if defined(CC_MT5398)
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_OFF, MJC_CLK_O_EN);        
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_OFF, MJC_CLK_SVP_EN);         
#endif   
}

void vDrvTTDResume(void)
{
    vDrvTTDInit();
}

void vDrvTTDProc(void)
{
    //Keep Config TTD Width / Height , Should be refine later
    VRM_INFO_T in;
    VRM_INFO_T out;

    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_MJC, &in, &out);
    vDrvTTDSetPicSize(out.u2Width, out.u2Height, out.u2HTotal);
    
#ifdef CC_MT5398    
    vDrvTddcSetPicSize(out.u2Width, out.u2Height);
#endif

    vDrvTTDQualityProc();
}


//===========================
//  TTD PQ System Setting
//===========================
void vDrvTTDQualityInit(void)
{
#if defined(CC_MT5398)
    vDrvTddcInit(1920, 1080);
#else    
    vDrvTTDLSVInit();
#endif

    g_i4DisparitySaclarDft = bApiGetQtyDft(QUALITY_DEPTH_TTD_SCALAR, SOURCE_TYPE_TIMING_MAX);

    vDrvTTDSetStereoComfortRange(STEREO_COMFORT_FACTOR);

    vIO32WriteFldAlign(TTDVP_05, SV_OFF, VP_TRANSITION_CTRL_EN);
    vIO32WriteFldAlign(TTDVP_08, 2, VP_TRANSIENT_STEP);    
    vIO32WriteFldAlign(TTDIR_01, 0, IR_OCCLUSION_GAIN);
    vIO32WriteFldAlign(TTDIR_03, 1, IR_BLACK_ASYM);  //for the blackNum use different setting

    // Global Model
    vIO32WriteFldAlign(TTDVP_07, IS_ECO_IC(), GM_VERT_CTRL_EN);   // use HW mode on ES, use SW mode on MP
    vIO32WriteFldAlign(TTDVP_06, 0x64, GM_VERT_MODEL_GAIN);
    vIO32WriteFldAlign(TTDVP_12, 0x0, GM_VERT_SLOPE);
    vIO32WriteFldAlign(TTDVP_19, SV_ON, GM_HORI_CTRL_EN);   // use FW to fill control point
    vIO32WriteFldAlign(TTDVP_13, SV_ON, GM_WEIGHT_CTRL_EN);
    vIO32WriteFldAlign(TTDVP_05, 0x400, GM_HORI_SMOOTH_WEIGHT);
    vIO32WriteFldAlign(TTDVP_07, 0x10, GM_VERT_SMOOTH_WEIGHT);  // use HW mode on ES
    vIO32WriteFldAlign(TTDVP_13, 0x300, GM_HORI_WEIGHT);
    vIO32WriteFldAlign(TTDVP_14, 0x100, GM_VERT_WEIGHT);
    vIO32WriteFldAlign(TTDDG_11, 0x0, GM_HORI_CTRL_PT_TOP);
    vIO32WriteFldAlign(TTDDG_11, 0xFF, GM_HORI_CTRL_PT_BTM);

    // Local Model
    vIO32WriteFldMulti(TTD_04, P_Fld(0x8, TTD_LM_V_WEIGHT) |
                               P_Fld(0x8, TTD_LM_U_WEIGHT));
    vIO32WriteFldMulti(TTDVP_09, P_Fld(0x64, CID_EDGE_GAIN) |
                                 P_Fld(SV_OFF, VP_LETTERBOX_DETECT_EN));
    vIO32WriteFldAlign(TTDVP_12, 0x20, CID_EDGE_BASE);
    vIO32WriteFldAlign(TTDVP_15, 0x10, CID_CHROMA_GAIN);
    vIO32WriteFldMulti(TTDVP_16, P_Fld(0x7, CID_Y_WEIGHT) |
                                 P_Fld(0x9, CID_C_WEIGHT) |
                                 P_Fld(200, VP_LETTERBOX_HIST_TH));

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

    vDrvTTDSetGlobalWeight(0);
    vDrvTTDSetTfuBase(0);
    vDrvTTDSetGlobalShift(0x80);
    vDrvTTDSetFWOnOff(SV_OFF);

    if (g_u1SupportDepthCtrl)
    {
#if defined(CC_MT5398)
        vDrvTTDSetLocalWeight(32);
        vDrvTTDSetDisptScalar(DISPARITY_SCALAR_TDDC);
#else
        vDrvTTDSetLocalWeight(0);
        vDrvTTDSetDisptScalar(0);
#endif
    }
    else
    {
        vDrvTTDSetDisptScalar(0);
    }
    vDrvTTDSet3DDepthCtrl(g_u1SupportDepthCtrl);

    vIO32WriteFldAlign(TTDVP_09, SV_ON, VP_LOCAL_INVERSE_CONTROL_EN);
    vIO32WriteFldAlign(TTDVP_09, SV_OFF, VP_LOCAL_INVERSE_FLAG);
    vIO32WriteFldAlign(TTDVP_10, SV_ON, LOCAL_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDVP_10, SV_ON, TFU_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDVP_10, 0x8, FW_LOCAL_GAIN);
    vIO32WriteFldAlign(TTDVP_10, 0x0, FW_TFU_GAIN);
    vIO32WriteFldAlign(TTDIR_04, 0xFF, IR_DISPT_MIN);
    vIO32WriteFldAlign(TTDIR_04, 0xFF, IR_DISPT_MAX);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_GLOBAL_BASE);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_LOCAL_BASE);
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
    vDrvTTDSetGlobalWeight(8);
    vDrvTTDSetLocalWeight(24);
    vDrvTTDSetTfuBase(0x80);
    vDrvTTDSetGlobalShift(0x80);

    vIO32WriteFldAlign(TTDVP_09, SV_ON, VP_LOCAL_INVERSE_CONTROL_EN);
    vIO32WriteFldAlign(TTDVP_09, SV_OFF, VP_LOCAL_INVERSE_FLAG);
    vIO32WriteFldAlign(TTDVP_10, SV_ON, LOCAL_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDVP_10, SV_ON, TFU_GAIN_CONTROL_EN);
    vIO32WriteFldAlign(TTDVP_10, 0x8, FW_LOCAL_GAIN);
    vIO32WriteFldAlign(TTDVP_10, 0x8, FW_TFU_GAIN);
    vIO32WriteFldAlign(TTDIR_04, 0x27, IR_DISPT_MIN);
    vIO32WriteFldAlign(TTDIR_04, 0x27, IR_DISPT_MAX);
    vIO32WriteFldAlign(TTDIR_04, SV_OFF, IR_DSPRT_BLACK_CLP_EN);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_GLOBAL_BASE);
    vIO32WriteFldAlign(TTDDG_01, 0x80, DG_LOCAL_BASE);
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
    //keep w & h in global
    gTTDInfo.ttdInputWidth = u2Width;
    gTTDInfo.ttdInputHeight = u2Height;

    //set dg window
    vDrvTTDSetDGWidnow(u2Width, u2Height);
    
    //set demo window
    vDrvTTDSetDemoWindow(u2Width, u2Height);

    //set histogram bin size
    vDrvTTDSetHistogramBinSize(u2Width, u2Height);

    //set regional histogram setting
	vDrvTTDSetRgnHistogram(u2Width, u2Height);

    //set ROI for letter box detection
    vDrvTTDSetVpRoi(0, 32, u2Width - 1, u2Height - 33);
}

void vDrvTTDSetDGWidnow(UINT16 u2Width,UINT16 u2Height)
{
    vIO32WriteFldMulti(TTDDG_06, P_Fld(0, DG_LOCAL_X_STR) | P_Fld(0, DG_LOCAL_Y_STR));
    vIO32WriteFldMulti(TTDDG_07, P_Fld(u2Width - 1, DG_LOCAL_X_END) | P_Fld(u2Height - 1, DG_LOCAL_Y_END));
}

void vDrvTTDSetDemoWindow(UINT16 u2Width,UINT16 u2Height)
{
    vIO32WriteFldMulti(TTDDG_08, P_Fld(0, DG_DEMO_X_STR) | P_Fld(0, DG_DEMO_Y_STR));
    vIO32WriteFldMulti(TTDDG_09, P_Fld(u2Width - 1, DG_DEMO_X_END) | P_Fld(u2Height - 1, DG_DEMO_Y_END));
}

void vDrvTTDSetRgnHistogram(UINT16 u2Width, UINT16 u2Height)
{
    UINT16 u2VpLastTilePxlWeight, u2Remainder;
    UINT32 u4HistWinSize, u4VpLetterBoxHistTh;

    u2Remainder = u2Height % 16;
    u2VpLastTilePxlWeight = (u2Remainder == 0) ? 1 : (16 / u2Remainder);
    vIO32WriteFldAlign(TTDVP_04, u2VpLastTilePxlWeight, VP_LAST_TILE_PXL_WEIGHT);

    u4VpLetterBoxHistTh = VP_LETTER_BOX_HIST_TH * ((UINT32)u2Width) / 1920;
    vIO32WriteFldAlign(TTDVP_16, u4VpLetterBoxHistTh, VP_LETTERBOX_HIST_TH);

    u4HistWinSize = ((u2Height / 16 ) + 7) / 8;
    vIO32WriteFldAlign(TTDVP_05, u4HistWinSize, GM_HIST_WINDOW_SIZE);
    vIO32WriteFldAlign(TTDDG_00, u4HistWinSize, DG_HIST_WINDOW_SIZE);

    vIO32WriteFldAlign(TTDVP_07, u2Width / 2, GM_VERT_CENTER);
}

void vDrvTTDSetVpRoi(UINT16 u2WidthStart, UINT16 u2HeightStart, UINT16 u2WidthEnd, UINT16 u2HeightEnd)
{
    vIO32WriteFldAlign(TTDVP_03, u2WidthStart, VP_ROI_X_STA);
    vIO32WriteFldAlign(TTDVP_04, u2HeightStart, VP_ROI_Y_STA);
    vIO32WriteFldAlign(TTDVP_15, u2WidthEnd, VP_ROI_X_END);
    vIO32WriteFldAlign(TTDVP_15, u2HeightEnd, VP_ROI_Y_END);
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
#ifdef CC_MT5398
            // Cobra TDDC
            u4UpdateItem = UI_SW_GLOBAL | UI_SW_LOCAL | UI_SW_GLOBAL_SFT | UI_BLACK_BAR;
            u4DisparitySaclar = DIFF(g_u1DepthOfFieldSWGlobal, 0x20) * 8;  //For black bar control
            u4GlobalShift = g_u1ProtrudeFromScreen + ABS(g_u1AutoConvBlkBarPxl);  //For black bar control
#else
            // Viper TDDC
            u4UpdateItem = UI_SW_GLOBAL | UI_SW_GLOBAL_SFT;
#endif
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

UINT8 u1DrvTTDGetPrInfo(void)
{
	UINT8 u1PrInfo = 0;
	u1PrInfo = IO32ReadFldAlign(TTDIR_00, IR_PR_INFO);
	return u1PrInfo;
}

void vDrvTTDSetShutterLREyeInverse(UINT8 u1OnOff)
{
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

UINT8 u1DrvTTDGetConvDepth(void)
{
	UINT8 u1ConvDepth = 0;
	u1ConvDepth = IO32ReadFldAlign(TTD_00, TTD_CONVERGENCE_DEPTH);
	
	return u1ConvDepth;
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
#ifndef CC_MT5398
    VRM_INFO_T* inRes;
#endif

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

#ifndef CC_MT5398
    inRes = VRMGetVRMInfo(SV_VP_MAIN, VRM_MODULE_MIXER);
    u4BlackNumL = u4BlackNum + inRes->u2X;
    u4BlackNumR = u4BlackNum + (PANEL_GetPanelWidth() - inRes->u2Width - inRes->u2X);
    u4BlackSingleNumL = u4BlackSingleNum + inRes->u2X;
    u4BlackSingleNumR = u4BlackSingleNum + (PANEL_GetPanelWidth() - inRes->u2Width - inRes->u2X);  
#else
    u4BlackNumL = u4BlackNum;
    u4BlackNumR = u4BlackNum;
    u4BlackSingleNumL = u4BlackSingleNum;
    u4BlackSingleNumR = u4BlackSingleNum;
#endif

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
    if (IS_ECO_IC())
    {
        u4TempL = u4ValueL & 0xFF;
        u4TempR = u4ValueR & 0xFF;
        vIO32WriteFldAlign(TTDIR_06, u4TempL, IR_BLACK_L);
        vIO32WriteFldAlign(TTDIR_06, u4TempR, IR_BLACK_R);
        u4TempL = (u4ValueL >> 8) & 0x7;
        u4TempR = (u4ValueR >> 8) & 0x7;
        vIO32WriteFldAlign(TTD_08, u4TempL, IR_BLACK_L_MSB);
        vIO32WriteFldAlign(TTD_08, u4TempR, IR_BLACK_R_MSB);
    }
    else
    {
        u4TempL = MIN(u4ValueL, 0xFF);
        u4TempR = MIN(u4ValueR, 0xFF);
        vIO32WriteFldAlign(TTDIR_06, u4TempL, IR_BLACK_L);
        vIO32WriteFldAlign(TTDIR_06, u4TempR, IR_BLACK_R);
    }

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

//===========================
//  PQ Setting
//===========================
void vDrvTTDSetGlobalWeight(UINT8 u1Value)
{
	u1Value = (u1Value > 127) ? 127 : u1Value;
    vIO32WriteFldAlign(TTDDG_01, u1Value, DG_GLOBAL_WEIGHT);
}

void vDrvTTDSetLocalWeight(UINT8 u1Value)
{
	u1Value = (u1Value > 127) ? 127 : u1Value;
    vIO32WriteFldAlign(TTDDG_01, u1Value, DG_LOCAL_WEIGHT);    
}

void vDrvTTDSetTfuWeight(UINT8 u1Value)
{
    vIO32WriteFldAlign(TTDDG_04, u1Value, DG_TEXTURE_WEIGHT);
}

void vDrvTTDSetHistogramBinSize(UINT16 u2Width,UINT16 u2Height)
{
    UINT16 binSize;
    //binsize thr for flatness statistic
    binSize = (u2Width / 8) * (u2Height / 8) / 32;    
    vIO32WriteFldAlign(TTDVP_03,binSize, HIS_AVG_BINSIZE);
}

void vDrvTTDSetQualityISR(void)
{
    if ((gTTDInfo.ttdFwStatus == SV_ON) && IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_EN))
    {
        SYS_MEASURE_TIME_START(SV_TTD_TIMER);
        vDrvTTDSetGlobalModel();
        vDrvTTDCheckTransition();
        SYS_MEASURE_TIME_END(SV_TTD_TIMER);
        SYS_STORE_TIME_INFO_TO_REG(SV_TTD_TIMER);     
    }

    if (gTTDInfo.tddcFwStatus == SV_ON)
    {        
#if defined(CC_MT5398) 
        SYS_MEASURE_TIME_START(SV_DEPTH_CTRL_TIMER);
        vDrvTddcFwTrigger();
        SYS_MEASURE_TIME_END(SV_DEPTH_CTRL_TIMER);
        SYS_STORE_TIME_INFO_TO_REG(SV_DEPTH_CTRL_TIMER);  
#endif
    }
}

void vDrvTTDQualityProc(void)
{
    if (g_u1SupportDepthCtrl) 
    {
#if defined(CC_MT5398)
        vDrvTddcProc();
#else
        vDrvTTDLSVProc();
#endif
    }

    vDrvTTDPicChgCheck();
    
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
#if defined(CC_MT5398) 
    vDrvTddcSetCtrl(u1OnOff);
#else 
    vIO32WriteFldAlign(SW_TTD_REG_00, u1OnOff, SW_LSV_EN);
#endif
    LOG(0, "Set 3D depth control enable  %d\n", u1OnOff);       
}

UINT8 u1DrvTTDGet3DDepthCtrl(void)
{    
#if defined(CC_MT5398) 
    return IO32ReadFldAlign(TDDC_00, C_TDDC_EN);
#else 
    return IO32ReadFldAlign(SW_TTD_REG_00, SW_LSV_EN); 
#endif
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



