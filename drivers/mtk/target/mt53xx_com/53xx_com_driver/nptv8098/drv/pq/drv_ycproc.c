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
 */
 /*------------------------------------------------------------------------------------------------ 
 * Owner: menghu.lin
 * 
 * Description: 
 * This file contains Luma/Chroma processing functions. 
 *-----------------------------------------------------------------------------------------------
 */ 

// === INCLUDE =============================================================================
#include "x_os.h"	
#include "x_rand.h"
#include "sv_const.h"
#include "hw_vdoin.h"
#include "vdo_misc.h"
#include "nptv_debug.h"
#include "video_def.h"
#include "drv_scaler.h"
#if defined(CC_MT5398)
#include "drv_lcdim_v2.h"
#else
#include "drv_lcdim.h"
#endif
#include "drv_od.h"
#include "drv_meter.h"
#include "drv_pqstress.h"
#include "x_assert.h"
#include "b2r_drvif.h"
#include "drv_upscaler.h"
#include "drv_predown.h"
#include "drv_scaler_drvif.h"
#include "drv_di.h"
#include "drv_contrast.h"
#include "drv_nr.h"

// === HW INCLUDE =============================================================================
#include "hw_ycproc.h"
#include "hw_sw.h"
#include "hw_ospe.h"
#include "hw_scpos.h"
#include "drv_tdtv_drvif.h"
#include "hw_od.h"
#include "drv_scaler_gfx.h"
// === DEFINE =============================================================================
#define  vDrvPoclkSetClock(x)      (vIO32WriteFldAlign(OSTG_00, x, R_POST_POCLK_EN))
#define  vDrvOclkSetClock(x)      (vIO32WriteFldAlign(OSTG_00, x, R_POST_OCLK_EN))

// === FUNCTION PROTOTYPE =================================================================

// === EXTERN FUNCTION ====================================================================
extern void vDrvSharpProc(void);
extern void vDrvADLProc(void);
extern void vDrvADLProcMainLoop(void);
extern void vDrvGammaProc(void);
extern void vDrvDIForce3DModeWithDelay(void);
extern void vDrvNRSet2DNRDemo(UINT8 bOnOff, UINT8 bRegion, UINT16 wXCenter);
extern void vDrvNRSet3DNRDemo(UINT8 bOnOff, UINT8 bRegion, UINT16 wXCenter);
#ifdef CUST_SOEM_DRV
extern void vDrvBrightPlusProc(void);
#endif
// === GLOBAL VARIABLE ====================================================================
MMAppInfo sMMAppInfo; 
UINT8 u1SupportHdmiYCbCr444 = SV_FALSE;
UINT8 u1SupportVgaYCbCr444 = SV_FALSE;

// === EXTERN VARIABLE ====================================================================
extern UINT16 u2PQItemMatrix[SV_VP_NA][PQ_MAX];
extern UINT8 bSceGain[3];
extern UINT8 bSkinTable[3];
extern UINT8 bSce3dGain[2];
extern UINT8 _bMAIN_COLOR_SYS;
extern UINT8 _bPIP_COLOR_SYS;

// === STATIC VARIABLE ====================================================================
static Region DemoRec; //, b4ScalerDemoRec;
static INT8 bHMove=1, bVMove=1;

// === BODY ===============================================================================
UINT8 bDrvCheckPqReg(UINT32* pu4Table, UINT32 u4Size)
{
    UINT32 i, u4RegCheck = 0, u4MskCheck = 0;
    UINT32 uCnt = u4Size/12-1;

    for (i=0; i<uCnt; i++)
    {
        u4RegCheck += (pu4Table[i*3]>>(i&0xF));
        u4MskCheck += (pu4Table[i*3+2]>>(i&0xF));   
    }

    if ((u4RegCheck == pu4Table[i*3]) && (u4MskCheck == pu4Table[i*3+2]))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void vDrvSetPqReg(UINT32* pu4Table, UINT32 u4Size)
{
    UINT32 i;
    UINT32 uCnt = u4Size/12-1;

    for (i=0; i<uCnt; i++)
    {
        if (pu4Table[i*3] < VDOIN_BASE)
        {
            vIO32Write4BMsk(u4SWReg(pu4Table[i*3]), pu4Table[i*3+1], pu4Table[i*3+2]);
        }
        else
        {
        	vIO32Write4BMsk(pu4Table[i*3], pu4Table[i*3+1], pu4Table[i*3+2]);
    	}
    }
    
    return;
}

void vDrvGetPqReg(UINT32* pu4Table, UINT32 u4Size)
{
    UINT32 i;
    UINT32 uCnt = u4Size/12-1;

    for (i=0; i<uCnt; i++)
    {
        if (pu4Table[i*3] < VDOIN_BASE)
        {
            pu4Table[i*3+1] = u4IO32Read4B(u4SWReg(pu4Table[i*3]))&pu4Table[i*3+2];
        }
        else
        {
        	pu4Table[i*3+1] = u4IO32Read4B(pu4Table[i*3])&pu4Table[i*3+2];
    	}
    }
    
    return;
}

void vDrvPreprocHWInit(void)
{
    vIO32WriteFldAlign(PAT_PIP_00,0,C_MAIN_SELFGEN_EN);
    vIO32WriteFldAlign(TRANS_MAIN_07, 0, CONTROL_SEL);
    vIO32WriteFldAlign(TRANS_MAIN_07, 6, INPUT_SELP);
    vIO32WriteFldAlign(TRANS_PIP_07, 0, CONTROL_SEL);
    vIO32WriteFldAlign(TRANS_PIP_07, 6, INPUT_SELP);
    vIO32WriteFldAlign(YCBCR2YC_MAIN_00, 0x0, C_MAIN_HSYNC_BLACK);
    vIO32WriteFldAlign(YCBCR2YC_PIP_00, 0x0, C_MAIN_HSYNC_BLACK);
    
#ifdef CC_MT5398
    vDrvPreSharpInit();
    vDrvCTIFInit();
#endif
}

void vDrvPreprocSWInit(void)
{
    u1SupportHdmiYCbCr444 = DRVCUST_OptGet(eSupportHdmiYCbCr444);
    u1SupportVgaYCbCr444 = DRVCUST_OptGet(eSupportVgaYCbCr444);
    vIO32WriteFldAlign(MATRIX_00, 0x0, IN_CSC_BLACK_LVL_M);
    vIO32WriteFldAlign(MATRIX_00, 0x0, IN_CSC_BLACK_LVL_P);
    vIO32WriteFldAlign(MATRIX_00, DRVCUST_OptGet(eMatrixHDYCbCr709), MATRIX_HD_709);
    vIO32WriteFldAlign(MATRIX_01, 0x200, IN_Y_GAIN);
    vIO32WriteFldAlign(MATRIX_01, 0x200, IN_Y_OFST);
    vIO32WriteFldAlign(MATRIX_01, 0x200, IN_C_GAIN);
    vIO32WriteFldAlign(MATRIX_03, u1SupportHdmiYCbCr444, YCBCR444_HDMI);
    vIO32WriteFldAlign(MATRIX_03, u1SupportVgaYCbCr444, YCBCR444_VGA);
}

void vDrvPreprocInit(void)
{
    vDrvPreprocHWInit();
    vDrvPreprocSWInit();
}

void vDrvOstgHWInit(void)
{
    vDrvxvYCCSetClip(DRVCUST_OptGet(eFlagxvYCCClip));

		// loader logo not exist, option sel will be set during driver init
    if(DRVCUST_InitGet(eLoaderLogoNumber)==0)
    {
        vDrvSetOstgOptPos(DRVCUST_OptGet(eOSOptionPos));
    }

    vIO32WriteFldAlign(YCBCR2RGB_00, 0x0, YUV2RGB_BYPASS_MAIN);
    vIO32WriteFldAlign(YCBCR2RGB_00, 0x0, YUV2RGB_BYPASS_PIP);
    vIO32WriteFldAlign(GAINOSET_00, 0x0, RGB_GAIN_SEL);      // Select RGB gain range 0~2
    vIO32WriteFldAlign(GAMMA_00, 0, C_GM_SWAP_POST);         // default use pre-gamma (before local dimming)
    vIO32WriteFldAlign(OSTG_00, 1, C_DBUF_POCLK_ON);         // 5398 enable OSTG double buffer
    vIO32WriteFldAlign(OSTG_00, 1, C_DBUF_OCLK_ON);         // 5398 enable OSTG double buffer    
//    vDrvGammaOnOff(SV_ON);    // gamma ON step need to be done after SRAM initialized
    vDrvDBLHWInit();
}

void vDrvOstgSWInit(void)
{
    vDrvDBLSWInit();

    // Initial color filter value
    vIO32WriteFldAlign(MATRIX_02, 0x200, COLOR_FILTER_R_GAIN);
    vIO32WriteFldAlign(MATRIX_02, 0x200, COLOR_FILTER_G_GAIN);
    vIO32WriteFldAlign(MATRIX_02, 0x200, COLOR_FILTER_B_GAIN);

    // Initial OS 3x3 matrix
    vDrvCalVideoColorMatrix(SV_VP_MAIN, 0x80, 0x80, 0x80, 0x80);
    vDrvCalVideoColorMatrix(SV_VP_PIP, 0x80, 0x80, 0x80, 0x80);
}

void vDrvGlobalPQSWInit(void)
{
    // initial PQ setting
    vDrvSetBrightness(SV_VP_MAIN, 0x80);
    vDrvSetContrast(SV_VP_MAIN, 0x80);
    vDrvSetSaturation(SV_VP_MAIN, 0x80);
    vDrvSetHue(SV_VP_MAIN, 0x80);
    vDrvSetBrightness(SV_VP_PIP, 0x80);
    vDrvSetContrast(SV_VP_PIP, 0x80);
    vDrvSetSaturation(SV_VP_PIP, 0x80);
    vDrvSetHue(SV_VP_PIP, 0x80);
}

void vDrvOstgInit(void)
{
    vDrvOstgHWInit();
    vDrvOstgSWInit();
    vDrvGlobalPQSWInit();
}

void vDrvPreprocResume(void)
{
    vDrvPreprocHWInit();
}

void vDrvOSTGReset(UINT8 bOnOff)
{
    vIO32WriteFldAlign(OSTG_00, bOnOff, CLK2_RST_CTRL);
    vIO32WriteFldAlign(OSTG_00, bOnOff, CLK_RST_CTRL);
    vIO32WriteFldAlign(OSTG_00, bOnOff, REG_RST_CTRL);
}

void vDrvPostprocSuspend(void)
{
    vDrvPoclkSetClock(SV_OFF);
    vDrvOclkSetClock(SV_OFF);
}

void vDrvPostprocResume(void)
{
    vDrvPoclkSetClock(SV_ON);
    vDrvOclkSetClock(SV_ON);
    //fix PP resume OSD offset wrong menghu 0626
//    vDrvOSTGReset(SV_ON);
//    vDrvOSTGReset(SV_OFF);
    #ifdef CUST_SOEM_DRV
	vDrvNRInit(); // add to initialize the NR related register for from standby to power up
	#endif
    vDrvPostSharpInit();
    vDrvCTIRInit();   
    vDrvSCEHWInit();
    vDrvADLInit();
    vDrvOstgHWInit();
    vDrvGammaInitSRAM();    
    #ifdef CUST_SOEM_DRV
    vDrvGamma_Resume();    //Masoem,fix gamma disable issue after standby
    #endif
    vDrvMeterInit();
#ifdef CC_MT5398
    vDrvCBEInit();
#endif

}

void vDrvYcprocInit(void)
{
    //Pre-Proc initialize
    vDrvPreprocInit();

    //Post-Proc initialize
    vDrvPostSharpInit();
    vDrvCTIFInit();
    vDrvCTIRInit();    
    vDrvSCEInit();
    vDrvADLInit();

    // Output-Stage initialize
    vDrvOstgInit();
    vDrvMeterInit();
#ifdef CC_MT5398
	vDrvCBEInit();
#endif

}

// ============================================================================
/**
 *  @brief Set filed signal resync with vsync when input is interlace.
 *  @param bPath SV_VP_MAIN/SV_VP_PIP
 */
void vDrvPreprocSetFieldResync(UINT8 bPath)
{
	UINT8 bFieldResync;

	if ((bGetVideoDecType(bPath) == SV_VD_MPEGHD) ||
	        (bDrvVideoIsSrcInterlace(bPath) == SV_FALSE)) // DTV or progressive timing, disable field prediction
	{
		bFieldResync = SV_FALSE;
	}
	else
	{
		bFieldResync = SV_TRUE;
	}

	LOG(3,"Set Field Resync to (%d)\n",bFieldResync);

	if (bPath == SV_VP_MAIN)
	{
        // TODO
		//vIO32WriteFldAlign(PAT_MAIN_00, bFieldResync, C_MAIN_F_V_SHK);    //Field resync by Vsync when vsize is reduced.
	}
	else
	{
        // TODO
        //vIO32WriteFldAlign(PAT_PIP_00, bFieldResync, C_PIP_F_V_SHK);    //Field resync by Vsync when vsize is reduced.
	}
}


/*****************************************************************************************/
/**********************************   DEMO MODE   ****************************************/
/*****************************************************************************************/
void vDrvDemoSetRegion(Region rDemoRegion)
{
    if((rDemoRegion.wHEnd<=rDemoRegion.wHStart)||(rDemoRegion.wVEnd<=rDemoRegion.wVStart))
    {
        printf("Invalid Demo Width or Demo Height Set !!\n");
    }
    vIO32WriteFldAlign(DEMO_00, rDemoRegion.wHStart, DEMOMODE_HSTART);
    vIO32WriteFldAlign(DEMO_00, rDemoRegion.wVStart, DEMOMODE_VSTART);
    vIO32WriteFldAlign(DEMO_01, (rDemoRegion.wHEnd-rDemoRegion.wHStart), DEMOMODE_WIDTH);
    vIO32WriteFldAlign(DEMO_01, (rDemoRegion.wVEnd-rDemoRegion.wVStart), DEMOMODE_HEIGHT); 
    vIO32WriteFldAlign(DEMO_00, rDemoRegion.u1OnOff, DEMOMODE_ENABLE);
}

void vDrvGetb4ScalerRec(void)
{
    #if 0  
    RDispPrm* dispPrm;    
    UINT16 wHPorch, wVPorch;
    UINT32 u4PreDownScale = u4PDSGetHPSFactorReg(SV_VP_MAIN);
    dispPrm = getDispPrm(SV_VP_MAIN); 
    
    if((dispPrm==NULL) || 
       (DemoRec.wHEnd<DemoRec.wHStart) || 
       (DemoRec.wVEnd<DemoRec.wVStart))
    {
        printf("Error parameters of demo region ! \n");
	}

    ASSERT(dispPrm->u4DSclH && dispPrm->u4DSclV);

    //In display mode, overscan is clipped by Regen, others by Scaler.
    wHPorch = (u1ScpipGetDispMode(SV_VP_MAIN)) ? 0 : getMixedHPorch(SV_VP_MAIN);
    wVPorch = (u1ScpipGetDispMode(SV_VP_MAIN)) ? 0 : getMixedVPorch(SV_VP_MAIN);
     
    //Transform demo region in panel size base to demo region in input image size base.

    b4ScalerDemoRec.wHStart = (UINT32)DemoRec.wHStart*dispPrm->u4USclH/0x8000*
                               0x8000/dispPrm->u4DSclH + wHPorch*u4PreDownScale/0x8000;
    b4ScalerDemoRec.wVStart = (UINT32)DemoRec.wVStart*dispPrm->u4USclV/0x8000*
                               0x8000/dispPrm->u4DSclV + wVPorch;
    b4ScalerDemoRec.wHEnd = b4ScalerDemoRec.wHStart + (DemoRec.wHEnd-DemoRec.wHStart)*
                               dispPrm->u4USclH/0x8000*0x8000/dispPrm->u4DSclH;
    b4ScalerDemoRec.wVEnd = b4ScalerDemoRec.wVStart + (DemoRec.wVEnd-DemoRec.wVStart)*
                               dispPrm->u4USclV/0x8000*0x8000/dispPrm->u4DSclV;
    #endif
}

void vDrvDemoRecValidation(void)
{
    UINT16 wWidth = u2ScpipGetDispWidth(SV_VP_MAIN);
    UINT16 wHeight = u2ScpipGetDispHeight(SV_VP_MAIN);

    // region Validation.
    if(IO32ReadFldAlign(DEMO_01, DEMOMODE_WIDTH)>wWidth)
    {
        vIO32WriteFldAlign(DEMO_01, wWidth, DEMOMODE_WIDTH);
    }
    if(IO32ReadFldAlign(DEMO_01, DEMOMODE_HEIGHT)>wHeight)
    {
        vIO32WriteFldAlign(DEMO_01, wHeight, DEMOMODE_HEIGHT);
    }
    if(DemoRec.wHEnd>=wWidth)
    {
        DemoRec.wHStart = wWidth-IO32ReadFldAlign(DEMO_01, DEMOMODE_WIDTH);
        DemoRec.wHEnd = wWidth;
    }
    if(DemoRec.wVEnd>=wHeight)
    {
        DemoRec.wVStart = wHeight-IO32ReadFldAlign(DEMO_01, DEMOMODE_HEIGHT);
        DemoRec.wVEnd = wHeight;
    }    
}

void vDrvDemoRecMoving(void)
{
    UINT16 wWidth = u2ScpipGetDispWidth(SV_VP_MAIN);
    UINT16 wHeight = u2ScpipGetDispHeight(SV_VP_MAIN);
    
    bHMove = (DemoRec.wHStart==0) ? 1 : (DemoRec.wHEnd==wWidth) ? -1 : bHMove;
    bVMove = (DemoRec.wVStart==0) ? 1 : (DemoRec.wVEnd==wHeight) ? -1 : bVMove;
    vIO32WriteFldAlign(DEMO_00, DemoRec.wHStart+bHMove, DEMOMODE_HSTART);
    vIO32WriteFldAlign(DEMO_00, DemoRec.wVStart+bVMove, DEMOMODE_VSTART);
}

void vDrvDemoProc(void)
{
    UINT8 bOnOff = IO32ReadFldAlign(DEMO_00, DEMOMODE_ENABLE);
    UINT8 bSplitOnOff = IO32ReadFldAlign(DEMO_00, SPLIT_ONOFF);    
    UINT8 bRegion = IO32ReadFldAlign(DEMO_00, REGION_LEFT_RIGHT);
    UINT8 bMovingOnOff = IO32ReadFldAlign(DEMO_00, MOVINGEMO_ENABLE);    
    static UINT8 bDEMOModules = 0;
    static UINT8 btiming = 0xFF;
    static UINT8 u1FlipModule = 0xFF;
    static UINT16 u2DIWidth = 0xFFFF;
    static UINT8 bpreOnOff = SV_OFF;
    UINT8 bDemoToggle = (bOnOff != bpreOnOff) ? SV_ON : SV_OFF;

    bpreOnOff = bOnOff;

    if((bOnOff == SV_OFF)&&(bDemoToggle == SV_OFF))
        return;
    
    if((u2DIWidth !=u2DrvDIGetWidth())||(u1FlipModule != (UINT8)u4QueryFlipModule(SV_VP_MAIN))||
        (btiming != bDrvVideoGetSourceTypeTiming(VDP_1))||
        (DemoRec.u1OnOff!=bOnOff)|| (bDEMOModules != IO32ReadFldAlign(DEMO_02, DEMO_MODULES))||
        (DemoRec.wHStart!=IO32ReadFldAlign(DEMO_00, DEMOMODE_HSTART))||
        (DemoRec.wVStart!=IO32ReadFldAlign(DEMO_00, DEMOMODE_VSTART))||
        (DemoRec.wHEnd!=(IO32ReadFldAlign(DEMO_00, DEMOMODE_HSTART)+IO32ReadFldAlign(DEMO_01, DEMOMODE_WIDTH)))||
        (DemoRec.wVEnd!=(IO32ReadFldAlign(DEMO_00, DEMOMODE_VSTART)+IO32ReadFldAlign(DEMO_01, DEMOMODE_HEIGHT))))
    {
        u2DIWidth = u2DrvDIGetWidth();
        u1FlipModule = (UINT8)u4QueryFlipModule(SV_VP_MAIN);
        btiming = bDrvVideoGetSourceTypeTiming(VDP_1);
        bDEMOModules = IO32ReadFldAlign(DEMO_02, DEMO_MODULES);
        DemoRec.u1OnOff = bOnOff;
        DemoRec.wHStart = IO32ReadFldAlign(DEMO_00, DEMOMODE_HSTART);
        DemoRec.wVStart = IO32ReadFldAlign(DEMO_00, DEMOMODE_VSTART);
        DemoRec.wHEnd = IO32ReadFldAlign(DEMO_00, DEMOMODE_HSTART)+IO32ReadFldAlign(DEMO_01, DEMOMODE_WIDTH);
        DemoRec.wVEnd = IO32ReadFldAlign(DEMO_00, DEMOMODE_VSTART)+IO32ReadFldAlign(DEMO_01, DEMOMODE_HEIGHT);
        vDrvDemoRecValidation();
        vDrvGetb4ScalerRec();

        //Split demo
        //vDrvDIDemo(bSplitOnOff&&(IO32ReadFldAlign(DEMO_02, DEMO_MDDI_EN)), bRegion,(u2DIWidth/2)+0x44);   // 0x44 is for MEMA Demo position alignment. why 0x44? just test the position, and got this difference value.
        vDrvNRSet2DNRDemo(bSplitOnOff&&(IO32ReadFldAlign(DEMO_02, DEMO_2DNR_EN)), bRegion, (u2DIWidth/2));
        vDrvNRSet3DNRDemo(bSplitOnOff&&(IO32ReadFldAlign(DEMO_02, DEMO_3DNR_EN)), bRegion, (u2DIWidth/2));
        //vDrv3DNRDemo(bOnOff, b4ScalerDemoRec);
        //vDrvSetPostSharpDemoRange(DemoRec);    
        vDrv2DSharpDemo(bSplitOnOff&&(IO32ReadFldAlign(DEMO_02, DEMO_TDSHARP_EN)), DemoRec);
        vDrvECTIDemo(bSplitOnOff&&(IO32ReadFldAlign(DEMO_02, DEMO_ECTI_EN)), DemoRec);          

        #ifndef CUST_SOEM_DRV
        vDrvColorDemo(bOnOff&&(IO32ReadFldAlign(DEMO_02, DEMO_COLOR_EN)), DemoRec); 
        #endif
    }
    
    if(bMovingOnOff==SV_ON)
    {    
        vDrvDemoRecMoving();
    }    
}

/*****************************************************************************************/
/*************************************** General  *****************************************/
/*****************************************************************************************/
UINT8 bDrvGetPeUiDebugMode(void)
{
    return IO32ReadFldAlign(PEUI_00, PEUI_DEBUG_EN);
}

void vDrvSetPeUiDebugMode(UINT8 bOnOff)
{
    vIO32WriteFldAlign(PEUI_00, bOnOff, PEUI_DEBUG_EN);
}

void vDrvPreprocEnablePattern(UINT8 bPath,UINT8 bPattern)
{
    UINT32 u4Grid;
    UINT32 u4BypassMatrix = (bPattern == 0) ? SV_OFF : SV_ON;

    vIO32WriteFldAlign(PAT_MAIN_04,   0x3   , C_MAIN_HV_MODE);
    vIO32WriteFldAlign(PAT_MAIN_04, 0x300   , C_MAIN_FC_Y);
    vIO32WriteFldAlign(PAT_MAIN_04, 0x300   , C_MAIN_FC_CB);
    vIO32WriteFldAlign(PAT_MAIN_04, 0x300   , C_MAIN_FC_CR);
    vIO32WriteFldAlign(PAT_MAIN_05,   0x0   , C_MAIN_BC_Y);
    vIO32WriteFldAlign(PAT_MAIN_05, 0x100   , C_MAIN_BC_CB);
    vIO32WriteFldAlign(PAT_MAIN_05, 0x100   , C_MAIN_BC_CR);

    vIO32WriteFldAlign(PAT_PIP_04,   0x3    , C_MAIN_HV_MODE);
    vIO32WriteFldAlign(PAT_PIP_04, 0x300    , C_MAIN_FC_Y);
    vIO32WriteFldAlign(PAT_PIP_04, 0x300    , C_MAIN_FC_CB);
    vIO32WriteFldAlign(PAT_PIP_04, 0x300    , C_MAIN_FC_CR);
    vIO32WriteFldAlign(PAT_PIP_05,   0x0    , C_MAIN_BC_Y);
    vIO32WriteFldAlign(PAT_PIP_05, 0x100    , C_MAIN_BC_CB);
    vIO32WriteFldAlign(PAT_PIP_05, 0x100    , C_MAIN_BC_CR);

    switch(bPattern)
    {
        default:
        case 1:
        case 2:
            u4Grid = (wDrvVideoInputWidth(bPath) / 8) - 1;
            break;
        case 4:
            u4Grid = wDrvVideoInputWidth(bPath) / 11;
            break;
    }

    if (bPath == SV_VP_MAIN)
    {
        vIO32WriteFldAlign(PAT_MAIN_02, bPattern, C_MAIN_PAT_TYPE);   
        vIO32WriteFldAlign(PAT_MAIN_02, 0x0, C_MAIN_NOISE_EN);
        vIO32WriteFldAlign(PAT_MAIN_02, 0x8, C_MAIN_NOISE_LEVEL);
        vIO32WriteFldAlign(PAT_MAIN_03, u4Grid, C_MAIN_H_GRID);
        vIO32WriteFldAlign(TRANS_MAIN_00, u4BypassMatrix, MAIN_EASY_BP3X3);
    }
    else
    {
        vIO32WriteFldAlign(PAT_PIP_02, bPattern, C_MAIN_PAT_TYPE);
        vIO32WriteFldAlign(PAT_PIP_02, 0x0, C_MAIN_NOISE_EN);
        vIO32WriteFldAlign(PAT_PIP_02, 0x8, C_MAIN_NOISE_LEVEL);
        vIO32WriteFldAlign(PAT_PIP_03, u4Grid, C_MAIN_H_GRID);
        vIO32WriteFldAlign(TRANS_PIP_00, u4BypassMatrix, MAIN_EASY_BP3X3);
    }
}

/*****************************************************************************************/
/******************************   YCPROC MAIN LOOP   ***************************************/
/*****************************************************************************************/

static void _vDrvSCEParialLoadProc(void)
{
    UINT32 u4Temp = u4IO32Read4B(SCE_SW_LOAD);
    
    if (u4Temp&Fld2Msk32(SCE_SW_LOAD_TRIG))
    {
        vDrvSCELoadIdx(
            (u4Temp&Fld2Msk32(SCE_SW_LOAD_FUNC_IDX))>>Fld_shft(SCE_SW_LOAD_FUNC_IDX),
            (u4Temp&Fld2Msk32(SCE_SW_LOAD_HUE_IDX))>>Fld_shft(SCE_SW_LOAD_HUE_IDX),
            (u4Temp&Fld2Msk32(SCE_SW_LOAD_VALUE))>>Fld_shft(SCE_SW_LOAD_VALUE));
        vIO32WriteFldAlign(SCE_SW_LOAD, 0, SCE_SW_LOAD_TRIG);
    }
}

void vDrvBitPlusProc(void)
{
    UINT8 u1Index = 0;
    UINT8 u1NoiseIdx = 3;
    UINT8 u1NoiselevelTh[4] = {0, 8, 16, 24};
    UINT8 u1BitPlusYRange[4] = {12, 8, 4, 0};
    UINT8 u1BitPlusSRange[4] = {16, 12, 8, 0};
    UINT8 u1Noiselevel = (UINT8)IO32ReadFldAlign(NR_NMSTA_0E, NR_NM_STA_NL);

    for(u1Index = 0; u1Index < 4; u1Index ++)
    {
        if(u1Noiselevel >= u1NoiselevelTh[u1Index])
        {
            u1NoiseIdx = u1Index;
        }
    }

    vDrvBitPlusSetYRange(u1BitPlusYRange[u1NoiseIdx]);
    vDrvBitPlusSetSRange(u1BitPlusSRange[u1NoiseIdx]);    
    
}

void vDrvGlobalPQAdj(void)
{
    // Global PQ adjustment
    UINT8 bPath, bOffset, bDemoModeOnOff = SV_OFF, bDemoToggle = SV_OFF;
    static UINT8 bpreDemoModeOnOff = 0xFF;
    UINT32 u4MatrixPQAdj = IO32ReadFldAlign(MATRIX_00, MATRIX_PQ_ADJ);

    for (bPath = SV_VP_MAIN; bPath < SV_VP_NA; bPath++)
    {
        bOffset = (bPath ==SV_VP_MAIN) ? 0 : 0x4;
        if (bPath == SV_VP_MAIN)
        {
            bDemoModeOnOff = (IO32ReadFldAlign(DEMO_00, DEMOMODE_ENABLE)&&IO32ReadFldAlign(DEMO_02, DEMO_COLOR_EN)) ? SV_ON : SV_OFF;
            bDemoToggle = (bDemoModeOnOff != bpreDemoModeOnOff) ? SV_ON : SV_OFF;
            bpreDemoModeOnOff = bDemoModeOnOff ;
        }
        
		if ((bPath == SV_VP_MAIN) && IO32ReadFldAlign(PEUI_00, PEUI_OS_CSC_MODE_CHANGE_M)) 
		{
            vDrvCalVideoColorMatrix(SV_VP_MAIN, 0x80, 0x80, 0x80, 0x80);
			vIO32WriteFldAlign(PEUI_00, 0, PEUI_OS_CSC_MODE_CHANGE_M);
		}

		if ((bPath == SV_VP_PIP) && IO32ReadFldAlign(PEUI_00, PEUI_OS_CSC_MODE_CHANGE_P)) 
		{
            vDrvCalVideoColorMatrix(SV_VP_PIP, 0x80, 0x80, 0x80, 0x80);
			vIO32WriteFldAlign(PEUI_00, 0, PEUI_OS_CSC_MODE_CHANGE_P);
		}			
		
        if ((u4MatrixPQAdj & !bDemoModeOnOff) ||
            (bPath == SV_VP_PIP) || (ISSCALERRGB(bPath)))
        {
            //bDemoToggle = (IO32ReadFldAlign(DEMO_00, DEMOMODE_ENABLE) != bDemoModeOnOff) ? 1 : 0;
            //bOffset = (bPath ==SV_VP_MAIN) ? 0 : 0x4;
			
            if ((IO32ReadFldAlign(GLOBAL_ADJ_00 + bOffset, BRIGHT_DUMMY) != u2PQItemMatrix[bPath][PQ_BRIGHTNESS]) ||
                (IO32ReadFldAlign(GLOBAL_ADJ_02 + bOffset, CONT_DUMMY) != u2PQItemMatrix[bPath][PQ_CONTRAST]) ||
                (IO32ReadFldAlign(GLOBAL_ADJ_02 + bOffset, SAT_DUMMY) != u2PQItemMatrix[bPath][PQ_SATURATION]) ||
                (IO32ReadFldAlign(GLOBAL_ADJ_00 + bOffset, HUE_DUMMY) != u2PQItemMatrix[bPath][PQ_HUE]) ||
                bDemoToggle)
            {
                vDrvVideoColorMatrix(bPath);
                //bDemoModeOnOff = IO32ReadFldAlign(DEMO_00, DEMOMODE_ENABLE);

                // Blue stretch
                if (bPath == SV_VP_MAIN)
                {
                    vRegWriteFldAlign(G_PIC_ADJ_MAIN_1, 0x400, BRIGHTNESS);
                    vRegWriteFldAlign(G_PIC_ADJ_MAIN_2, 0x200, HUE);
                    vRegWriteFldAlign(G_PIC_ADJ_MAIN_2, 0x80, SAT);
                    vRegWriteFldAlign(G_PIC_ADJ_MAIN_1, 0x80, CONTRAST);
                    vDrvSetBlueStretchBase();
                }
            }
        }
        else
        {
            if ((IO32ReadFldAlign(GLOBAL_ADJ_00 + bOffset, BRIGHT_DUMMY) != u2PQItemMatrix[bPath][PQ_BRIGHTNESS]) ||
                (IO32ReadFldAlign(GLOBAL_ADJ_02 + bOffset, CONT_DUMMY) != u2PQItemMatrix[bPath][PQ_CONTRAST]) ||
                (IO32ReadFldAlign(GLOBAL_ADJ_02 + bOffset, SAT_DUMMY) != u2PQItemMatrix[bPath][PQ_SATURATION]) ||
                (IO32ReadFldAlign(GLOBAL_ADJ_00 + bOffset, HUE_DUMMY) != u2PQItemMatrix[bPath][PQ_HUE]) ||
                bDemoToggle)
            {                
                vIO32WriteFldAlign(G_PIC_ADJ_MAIN_1, 
                    0x400 + ((IO32ReadFldAlign(GLOBAL_ADJ_00, BRIGHT_DUMMY) - 0x80) << IO32ReadFldAlign(SCE_REG_00, SCE_BRI_INTERVAL)), 
                    BRIGHTNESS);
                vIO32WriteFldAlign(G_PIC_ADJ_MAIN_2, 
                    0x200 + ((IO32ReadFldAlign(GLOBAL_ADJ_00, HUE_DUMMY) - 0x80) << IO32ReadFldAlign(SCE_REG_00, SCE_HUE_INTERVAL)), 
                    HUE);
                
                vIO32WriteFldAlign(G_PIC_ADJ_MAIN_2, (IO32ReadFldAlign(GLOBAL_ADJ_02, SAT_DUMMY)*IO32ReadFldAlign(METER_INFO_17, METER_CC_RATIO))>>4, SAT);
                vIO32WriteFldAlign(G_PIC_ADJ_MAIN_1, IO32ReadFldAlign(GLOBAL_ADJ_02, CONT_DUMMY), CONTRAST);

                vDrvCalVideoColorMatrix(bPath, 0x80, 0x80, 0x80, 0x80);
                // Blue stretch
                if (bPath == SV_VP_MAIN)
                {
                    vDrvSetBlueStretchBase();
                }
            }
        }

    	if (IO32ReadFldAlign(PEUI_00, PEUI_IN_CSC_MODE_CHANGE_M))
    	{
    	    vDrvDIForce3DModeWithDelay();   // Avoid 3x3 matrix change then DI flicker       
    		vVdoSetColorTransform(SV_VP_MAIN);
    		vIO32WriteFldAlign(PEUI_00, SV_OFF, PEUI_IN_CSC_MODE_CHANGE_M);	
    	}

    	if (IO32ReadFldAlign(PEUI_00, PEUI_IN_CSC_MODE_CHANGE_P))
    	{
    		vVdoSetColorTransform(SV_VP_PIP);
    		vIO32WriteFldAlign(PEUI_00, SV_OFF, PEUI_IN_CSC_MODE_CHANGE_P);	
    	}
	}
}

void vDrvSCEProc(void)
{
    static UINT8 bPreSceGain[3] = {0x80, 0x80, 0x80};
    static UINT8 bPreSkinTable[3] = {0x80, 0x80, 0x80};
    static UINT8 bPreSce3dGain[2] = {0x80, 0x80};

    vDrvGlobalPQAdj();

    //SCE cannot be enabled under VGA signal
    if (ISSCALERRGB(SV_VP_MAIN))
    {
    	return;
    }

    bSceGain[0] = IO32ReadFldAlign(ADAPTIVE_SCE_REG, ADAPTIVE_SCE_L);
    bSceGain[1] = IO32ReadFldAlign(ADAPTIVE_SCE_REG, ADAPTIVE_SCE_S);
    bSceGain[2] = IO32ReadFldAlign(ADAPTIVE_SCE_REG, ADAPTIVE_SCE_H);
    bSkinTable[0] = IO32ReadFldAlign(ADAPTIVE_SKIN_REG, ADAPTIVE_SKIN_L);
    bSkinTable[1] = IO32ReadFldAlign(ADAPTIVE_SKIN_REG, ADAPTIVE_SKIN_S);
    bSkinTable[2] = IO32ReadFldAlign(ADAPTIVE_SKIN_REG, ADAPTIVE_SKIN_H);
    bSce3dGain[0] = IO32ReadFldAlign(TDTV_FW_00, SCE_3D_GAIN_00);
    bSce3dGain[1] = IO32ReadFldAlign(TDTV_FW_00, SCE_3D_GAIN_01);
        
    if ((IO32ReadFldAlign(ADAPTIVE_SCE_REG, ADAPTIVE_SCE_FORCE_LOAD) == SV_ON) 
        || (bPreSceGain[0] != bSceGain[0]) || (bPreSceGain[1] != bSceGain[1]) || (bPreSceGain[2] != bSceGain[2])
        || (bPreSkinTable[0] != bSkinTable[0]) || (bPreSkinTable[1] != bSkinTable[1]) || (bPreSkinTable[2] != bSkinTable[2])
        || (((bPreSce3dGain[0] != bSce3dGain[0]) || (bPreSce3dGain[1] != bSce3dGain[1])) && (u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF)))
    {
        bPreSceGain[0] = bSceGain[0]; 
        bPreSceGain[1] = bSceGain[1]; 
        bPreSceGain[2] = bSceGain[2]; 
        bPreSkinTable[0] = bSkinTable[0]; 
        bPreSkinTable[1] = bSkinTable[1]; 
        bPreSkinTable[2] = bSkinTable[2]; 
        bPreSce3dGain[0] = bSce3dGain[0];
        bPreSce3dGain[1] = bSce3dGain[1];
        
        vDrvSCECalculate();
        vIO32WriteFldAlign(ADAPTIVE_SCE_REG,SV_OFF, ADAPTIVE_SCE_FORCE_LOAD);
        vApiRegisterVideoEvent(PE_EVENT_SCE, SV_VP_MAIN, SV_ON);
    }

    _vDrvSCEParialLoadProc();

    if(IO32ReadFldAlign(ADAP_LUMA2_00, ADAP_BITPLUS_EN) == SV_ON)
    {
        vDrvBitPlusProc();
    }
}

void vDrvMMAPPorc(void)
{
    if (IO32ReadFldAlign(MMAPP_04, APP_CHANGE_NOTIFY) == SV_ON)
    {
        vIO32WriteFldAlign(MMAPP_04, SV_OFF, APP_CHANGE_NOTIFY);

        sMMAppInfo.u1AppType = VDP_GetPlayMode(SV_VP_MAIN);
        //sMMAppInfo.u1AppType = IO32ReadFldAlign(MMAPP_00, APP_TYPE);
        
        if (sMMAppInfo.u1AppType == 0xFF) /*Pretend for Netflix App */
        {
            sMMAppInfo.u2SrcWidth = IO32ReadFldAlign(MMAPP_01, APP_SRC_WIDTH);
            sMMAppInfo.u2SrcHeight = IO32ReadFldAlign(MMAPP_01, APP_SRC_HEIGHT);
            sMMAppInfo.u2OutWidth = IO32ReadFldAlign(MMAPP_02, APP_OUT_WIDTH);
            sMMAppInfo.u2OutHeight = IO32ReadFldAlign(MMAPP_02, APP_OUT_HEIGHT);
            sMMAppInfo.u4BitRate = IO32ReadFldAlign(MMAPP_03, APP_BITRATE);
            DRVCUST_MMAPProc();
        }
    }
}


#define OD_STATE_CNT 3
#define DI_STATE_CNT 3
#define GFX_STATE_CNT 3
EXTERN void HalFlushInvalidateDCache(void);

void vDrvDumpShrink(void)
{   
    UINT32 i, j;
    UINT32 pos_x, pos_y;
    UINT32 ratio_x, ratio_y;
    UINT32* pBuf = (UINT32 *)VIRTUAL(u4IO32Read4B(SWRW_01));
    UINT32 u4SrcWidth = IO32ReadFldAlign(SWRW_03, DUMP_CTRL_WIDTH);
    UINT32 u4SrcHeight = IO32ReadFldAlign(SWRW_03, DUMP_CTRL_HEIGHT);
    UINT32 u4DesWidth = IO32ReadFldAlign(SWRW_05, DUMP_CTRL_SHRINK_W);
    UINT32 u4DesHeight = IO32ReadFldAlign(SWRW_05, DUMP_CTRL_SHRINK_H);
    
    LOG(4, "OD Dump shrink W %d  H %d", u4DesWidth, u4DesHeight);

    
    if (pBuf == NULL)
    {        
        LOG(0, "Dump Shrink error");        
        return;
    }

    if ((u4DesWidth == 0) || (u4DesHeight == 0))
    {        
        LOG(4, "No Shrink", u4DesWidth, u4DesHeight);        
        return;
    }

    if ((u4DesWidth >= u4SrcWidth) && (u4DesHeight >= u4SrcHeight))
    {
        LOG(4, "Not support enlarge function");
        return;
    }

    ratio_x = (u4SrcWidth<<8)/u4DesWidth;
    ratio_y = (u4SrcHeight<<8)/u4DesHeight;
    
    for (j=0; j<u4DesHeight; j++)
    {
        pos_y = (j*ratio_y)>>8;
        
        for (i=0; i<u4DesWidth; i++)
        {            
            pos_x = (i*ratio_x)>>8;
            
            pBuf[j*u4DesWidth+i] = pBuf[pos_y*u4SrcWidth+pos_x];
        }
    }           
    
    vIO32WriteFldAlign(SWRW_03, u4DesWidth, DUMP_CTRL_WIDTH);
    vIO32WriteFldAlign(SWRW_03, u4DesHeight, DUMP_CTRL_HEIGHT);
    vIO32WriteFldAlign(SWRW_04, u4DesWidth*u4DesHeight*4, DUMP_CTRL_SIZE); 
}

void vDrvDumpPostProc(void)
{     
    HalFlushInvalidateDCache();  
    vDrvDumpShrink();      
    HalFlushInvalidateDCache();

    // finish dump
    vIO32WriteFldAlign(SWRW_04, 0, DUMP_CTRL_STATE);
    vIO32WriteFldAlign(SWRW_04, DUMP_ENG_NONE, DUMP_CTRL_ENG_SEL);  
}


void vDrvDumpProc(void)
{
    UINT32 u4State = IO32ReadFldAlign(SWRW_04, DUMP_CTRL_STATE);
    UINT32 u4EngSel = IO32ReadFldAlign(SWRW_04, DUMP_CTRL_ENG_SEL);
    UINT32 u4ODReady = IO32ReadFldAlign(OD_STA00, OD_RDY);
    static UINT32 u4DumpWaitCnt = 0;
    static UINT8 u1ODWaitCnt = 0;

    if (u4State == DUMP_STATE_INIT)
    {
        if (u4EngSel == DUMP_ENG_OD)
        {            
            vDrvOdDumpInit();
            u4DumpWaitCnt = 0;  //OD_STATE_CNT;            
        }
        else if (u4EngSel == DUMP_ENG_PSCAN)        
        {            
            vDrvDIDumpInit(VDP_1);
            u4DumpWaitCnt = DI_STATE_CNT;            
        }
#ifdef CC_CDC_SUPPORT
		else if(u4EngSel == DUMP_ENG_GRAPHIC)
		{
			vScpipDumpInit();
            u4DumpWaitCnt = GFX_STATE_CNT;            
		}
#endif
        else
        {            
            u4DumpWaitCnt = 0;
            vIO32WriteFldAlign(SWRW_04, DUMP_STATE_INIT, DUMP_CTRL_STATE);
            return;
        }       
        
        vIO32WriteFldAlign(SWRW_04, DUMP_STATE_START, DUMP_CTRL_STATE);
    }   
    else if (u4State == DUMP_STATE_START)
    {        
        if (u4DumpWaitCnt == 0)
        {
            if (u4EngSel == DUMP_ENG_OD)
            {            
                if (u4ODReady == 1)
                {
                    vDrvOdDumpStart();
                    u4DumpWaitCnt = OD_STATE_CNT;
                    u1ODWaitCnt = 0;
                    vIO32WriteFldAlign(SWRW_04, DUMP_STATE_END, DUMP_CTRL_STATE);
                    LOG(2, "OD Dump Start test OK\n");
                }
                else if (u1ODWaitCnt < 10)
                {
                    u1ODWaitCnt++;
                    LOG(2, "OD Dump NOT ready\n");
                }
                else
                {
                    //Printf("OD not ready:return \n");
                    u4DumpWaitCnt = 0;
                    u1ODWaitCnt = 0;
                    vIO32WriteFldAlign(SWRW_04, DUMP_ENG_NONE, DUMP_CTRL_ENG_SEL);
                    vIO32WriteFldAlign(SWRW_04, DUMP_STATE_INIT, DUMP_CTRL_STATE);
                    return;
                }
            }        
            else if (u4EngSel == DUMP_ENG_PSCAN)        
            {            
                vDrvDIDumpStart(VDP_1);
                u4DumpWaitCnt = DI_STATE_CNT;
                vIO32WriteFldAlign(SWRW_04, DUMP_STATE_END, DUMP_CTRL_STATE);
            }        
#ifdef CC_CDC_SUPPORT
			else if (u4EngSel == DUMP_ENG_GRAPHIC)        
            {            
                vScposGfxDumpStart(SV_VP_MAIN);
                u4DumpWaitCnt = GFX_STATE_CNT;
                vIO32WriteFldAlign(SWRW_04, DUMP_STATE_END, DUMP_CTRL_STATE);
            }
#endif
            else
            {                           
                u4DumpWaitCnt = 0;                
                vIO32WriteFldAlign(SWRW_04, DUMP_ENG_NONE, DUMP_CTRL_ENG_SEL);
                vIO32WriteFldAlign(SWRW_04, DUMP_STATE_INIT, DUMP_CTRL_STATE);
                return;
            }
        
            //vIO32WriteFldAlign(SWRW_04, DUMP_STATE_END, DUMP_CTRL_STATE);
        }
        else
        {
            //Printf("Start wait count %d \n", u4DumpWaitCnt);
            u4DumpWaitCnt = (u4DumpWaitCnt > 0) ? (u4DumpWaitCnt-1) : 0;
        }
    }
    else if (u4State == DUMP_STATE_END)
    {           
        if (u4DumpWaitCnt == 0)
        {
            if (u4EngSel == DUMP_ENG_OD)
            {
                vDrvOdDumpEnd();
                vDrvDumpPostProc();
            }
            else if (u4EngSel == DUMP_ENG_PSCAN)
            {
                vDrvDIDumpEnd(VDP_1);
                vDrvDumpPostProc();
            }

#ifdef CC_CDC_SUPPORT
            else if (u4EngSel == DUMP_ENG_GRAPHIC)
            {
                vScposGfxDumpEnd(VDP_1);
                //vScposGfxDumpPostProc();
            }
#endif
            
            vIO32WriteFldAlign(SWRW_04, DUMP_ENG_NONE, DUMP_CTRL_ENG_SEL);
            vIO32WriteFldAlign(SWRW_04, DUMP_STATE_INIT, DUMP_CTRL_STATE);
        }
        else
        {
            //Printf("End wait count %d \n", u4DumpWaitCnt);
            u4DumpWaitCnt = (u4DumpWaitCnt > 0) ? (u4DumpWaitCnt-1) : 0;
        }            
    }
}

void vDrv3DPQProc(void)
{
    /*Reset PQ items when : 
        * a. Mode change done
        * b. 3D format change
        * c. Scaler resolution change
        */
    if(fgGetPEUIResetFlg(SV_VP_MAIN))
    {
    	#if SUPPORT_MIB_3D_CONVERT_444_TO_422
		vDrvSet422(SV_VP_MAIN, !(bIsScalerInput444(SV_VP_MAIN)));
		#endif
        vApiApplyPQSetting(SV_VP_MAIN);
        vDrvSetPEUIResetFlg(SV_VP_MAIN, SV_OFF);
    }
    if(fgGetPEUIResetFlg(SV_VP_PIP))
    {
        vApiApplyPQSetting(SV_VP_PIP);
        vDrvSetPEUIResetFlg(SV_VP_PIP, SV_OFF);
    }
}
void vDrvYcprocProc(void)
{
    // PQ SRAM Stress Test Procedure
#ifndef NDEBUG
    vDrvPQStressProc();
#endif // NDEBUG
    
    vDrvMeterProc();
    
    vDrvSCEProc();

    vDrvSharpProc();

    //vDrvOPCProc();	
    #ifdef CUST_SOEM_DRV
	vDrvBrightPlusProc();
	#endif

    vDrvDemoProc();

    vDrvMMAPPorc();

    //vDrvADLProc();    
    vDrvADLProcMainLoop();

    //vDrvLcdimProc();
#ifdef CC_MT5398    
    vDrvLcdimProcMainLoop();
#endif
    //vDrvDumpProc();

    vDrv3DPQProc(); 
}

