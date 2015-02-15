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
#include "hw_sw.h"
#include "hw_scpos.h"
#include "hw_ycproc.h"
#include "nptv_debug.h"
#include "drv_ttd.h"
#include "drv_display.h"
#include "drv_video.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "drv_meter.h"
#include "drv_di.h"
#include "panel.h"
#include "fbm_drvif.h"

/*---------------------------------------------------------------------*
 * definition
 *---------------------------------------------------------------------*/
#define TTD_CHECK_INV_VER TTD_CHECK_INV_V1
#define INK_DEPTH_Y Fld(2,6,AC_MSKB0)//[7:6]

/*----------------------------------------------------------------------*
 * Global/Static Variables
 *----------------------------------------------------------------------*/
UINT8 g_u1SceneChangedFlag = SV_FALSE;
UINT8 g_u1SceneChangedCounter = 0;
UINT8 g_u1ImageSafetyLevel = 0; // 0: Off, 1: Low, 2: Middle, 3: High


/*----------------------------------------------------------------------*
 * Static Variables & Functions
 *----------------------------------------------------------------------*/
static void vDrvTTDCheckLocalInverseMode(void);
static void vDrvTTDAdaptiveWeight(void);
static UINT32 u4DrvTTDIsSceneChanged(void);


/*----------------------------------------------------------------------*
 * External Variables & Functions
 *----------------------------------------------------------------------*/


//===========================
//  PQ Setting
//===========================
void vDrvTTDSetGlobalModel(void)
{
    UINT32 u4Index, u4Addr;
    UINT32 u4GMHoriCtrlPt[9] = {26, 51, 77, 102, 128, 153, 179, 204, 230};

    for (u4Index = 0; u4Index < 8; u4Index++)
    {
        u4Addr = TTDVP_17 + u4Index;
        vIO32Write1B(u4Addr, u4GMHoriCtrlPt[u4Index]);
    }
    vIO32WriteFldAlign(TTDVP_19, u4GMHoriCtrlPt[8], GM_HORI_CTRL_PT_8);
    vIO32WriteFldAlign(TTDVP_19, 0x0, GM_HORI_CTRL_PT_TOP);
    vIO32WriteFldAlign(TTDVP_19, 0xFF, GM_HORI_CTRL_PT_BTM);
}

static void vDrvTTDCalImgChar(void)
{
    UINT16* Histogram = gTTDInfo.ttdHist;
    UINT16* HueHistogram = gTTDInfo.ttdHueHist;
    UINT32 i, left, right, u4Diversity, u4Variance, u4HueVariance, u4HueAvg, u4Value;
    UINT32 hist_thld = IO32ReadFldAlign(TTD_FW_REG_03, TTD_HIST_THLD);
    UINT32 div_thld_h = IO32ReadFldAlign(TTD_FW_REG_03, TTD_DIVER_THLD_H);  
    UINT32 u4APL;

    if (IO32ReadFldAlign(TTD_03, INK_DEPTH_Y) != 0) {return;}

    gTTDInfo.max_index = -1;
    gTTDInfo.hist_max = 0;
    gTTDInfo.max_index_sec = -1;
    gTTDInfo.hist_max_sec = 0;
    u4APL = (UINT32)bDrvGetAPL();
    u4Diversity = 0;
    u4Variance = 0;
    u4HueVariance = 0;

    u1DrvGetLumaHist(Histogram, 16320);
    u1DrvGetHueHist(HueHistogram, &u4HueAvg, 4096);
    
    for (i = 0; i < 32; i++)
    {
        // Find max/sec bin in luma histogram
        left = (i > 0) ? (i - 1) : i;
        right = (i < 31) ? (i + 1) : i;

        if (((Histogram[left] + hist_thld) <= Histogram[i]) && 
            ((Histogram[right] + hist_thld) <= Histogram[i]))
        {    
            if (Histogram[i] > gTTDInfo.hist_max)
            {                         
                gTTDInfo.hist_max_sec = gTTDInfo.hist_max;
                gTTDInfo.max_index_sec = gTTDInfo.max_index;
                gTTDInfo.hist_max = Histogram[i];
                gTTDInfo.max_index = i;
            }

            if ((Histogram[i] > gTTDInfo.hist_max_sec) && (i != gTTDInfo.max_index))
            {            
                gTTDInfo.hist_max_sec = Histogram[i];
                gTTDInfo.max_index_sec = i;
            }
        }
        
        // Calcualter for variance
        u4Variance += Histogram[i] * (ABS((INT32)(u4APL-(i*8+4))));
    }
    u4Variance = MIN(MAX(u4Variance >> 13, 1), 255);

    // Calculate for diversity
    if ((gTTDInfo.max_index == -1) && (gTTDInfo.max_index_sec == -1))
    {
        u4Diversity = div_thld_h;   // force to case2 : use APL
        LOG(9, "No Local Max\n");
    }
    else
    {
        // Calculate the diversity between max index and other bins in the histogram
        for (i = 0; i < 32; i++)
        {
            u4Diversity += DIFF(Histogram[gTTDInfo.max_index], Histogram[i]) * DIFF(gTTDInfo.max_index, i);
        }
        u4Diversity = MIN(MAX(u4Diversity >> 13, 1), 255);
    }

    // Calcualter for hue variance
    u4HueAvg = (u4HueAvg + 4) / 8;
    for (i = 0; i < 8; i++)
    {
        u4Value = DIFF(HueHistogram[i], u4HueAvg);
        u4HueVariance += (u4Value * u4Value);

        if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_HIST_DBG_EN))
        {
            LOG(0, "HueHistogram[%d] = %d\n", i, HueHistogram[i]);
        }
    }
    u4HueVariance = MIN(MAX(u4HueVariance >> 15, 1), 255);

    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_HIST_DBG_EN))
    {
        LOG(0, "Average = %d, Diversity = %d, Variance = %d, HueVariance = %d, HueSum = %d\n", u4APL, u4Diversity, u4Variance, u4HueVariance, u4HueAvg*8);
        LOG(0, "Max Ind   %5d     %5d \n", gTTDInfo.max_index, gTTDInfo.max_index_sec);
        LOG(0, "Max Val   %5d     %5d \n", gTTDInfo.hist_max, gTTDInfo.hist_max_sec);
    }

    vIO32WriteFldAlign(TTD_FW_REG_09, u4APL, TTD_CUR_AVG);
    vIO32WriteFldAlign(TTD_FW_REG_09, u4Diversity, TTD_DIVER);
    vIO32WriteFldAlign(TTD_FW_REG_11, u4Variance, TTD_VARIANCE);
    vIO32WriteFldAlign(TTD_FW_REG_11, u4HueVariance, TTD_HUE_VARIANCE);    
    vIO32WriteFldAlign(TTD_FW_REG_09, gTTDInfo.max_index, TTD_MAX_FIR);
    vIO32WriteFldAlign(TTD_FW_REG_09, gTTDInfo.max_index_sec, TTD_MAX_SEC);
}


//===========================
//  Adaptive PQ Algorithm
//===========================
static void vDrvTTDAdaptiveWeight(void)
{
    UINT32 u4GlobalWeight, u4TFUWeight, u4Temp;
    UINT32 u4GlobalMax = IO32ReadFldAlign(TTD_FW_REG_12, TTD_GLOBAL_MAX);
    UINT32 u4GlobalMin = IO32ReadFldAlign(TTD_FW_REG_12, TTD_GLOBAL_MIN);
    UINT32 u4SWGlobalGain = IO32ReadFldAlign(TTD_FW_REG_11, TTD_GLOBAL_GAIN);
    UINT32 u4SWTFUGain = IO32ReadFldAlign(TTD_FW_REG_17, TTD_TFU_GAIN);
    UINT32 u4Variance = IO32ReadFldAlign(TTD_FW_REG_11, TTD_VARIANCE);
    UINT32 u4HueVariance = IO32ReadFldAlign(TTD_FW_REG_11, TTD_HUE_VARIANCE);

    // Global weight
    u4Temp = (u4Variance < 176) ? 0x80 : ((17203 - 51 * u4Variance) >> 6);
    u4GlobalWeight = (128 - u4Temp) / 2;
    u4GlobalWeight = (u4GlobalWeight * u4SWGlobalGain + 64) >> 7;
    u4GlobalWeight = MIN(MAX(u4GlobalWeight, u4GlobalMin), u4GlobalMax);

    // TFU weight
    u4TFUWeight = (u4HueVariance < 50) ? 2 : ((((u4HueVariance - 50) * 6 + 32) >> 6) + 2);
    u4TFUWeight = (u4TFUWeight * u4SWTFUGain + 4) >> 3;
    u4TFUWeight = MIN(MAX(u4TFUWeight, 2), 8);
    
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_ADAP_GM_EN))
    {
        vDrvTTDSetGlobalWeight(u4GlobalWeight);
        vDrvTTDSetTfuWeight(u4TFUWeight);
    }    
}

void vDrvTTDCheckTransition(void)
{
    static UINT8 bDbgCnt = 0;    

    if ((u4DrvTTDIsSceneChanged() == SV_TRUE) || 
        (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_FORCE_SC) && (bDbgCnt >= IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_FORCE_SC_CNT))))
    {
        vDrvTTDCalImgChar();
        vDrvTTDCheckLocalInverseMode();
        vDrvTTDAdaptiveWeight();
        bDbgCnt = 0;
    }
    else
    {
        bDbgCnt++;
    }
}

static UINT8 bDrvTTDCheckCIDInverseV1(void)
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

static UINT8 bDrvTTDCheckCIDInverseV2(void)
{
    UINT32 u4Diversity = IO32ReadFldAlign(TTD_FW_REG_09, TTD_DIVER);
    UINT32 u4APL = IO32ReadFldAlign(TTD_FW_REG_09, TTD_CUR_AVG);
    UINT32 u4Inv, u4Case, u4Value;

    // V2 is used for L-company

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
        u4Case = 3;
        u4Inv = (u4APL >= 109) ? SV_TRUE : SV_FALSE;
    }
    else if (u4Diversity <= 70)
    {
        u4Case = 5;
        u4Inv = (u4APL >= 110) ? SV_TRUE : SV_FALSE;
    }
    else if (u4Diversity <= 105)
    {
        u4Case = 6;
        u4Inv = (u4APL >= 125) ? SV_TRUE : SV_FALSE;
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

static UINT8 bDrvTTDCheckCIDInverse(void)
{    
    UINT32 u4Inv;

    switch (TTD_CHECK_INV_VER)
    {
        case TTD_CHECK_INV_V1:
            u4Inv = bDrvTTDCheckCIDInverseV1();
            break;
        case TTD_CHECK_INV_V2:
            u4Inv = bDrvTTDCheckCIDInverseV2();
            break;
        default:
            u4Inv = SV_FALSE;
            break;
    }

    // Debug message
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_INV_DBG_EN))
    {
        LOG(0, "CID inv = %d  \n", u4Inv);
    }

    return u4Inv;
}

static UINT32 u4DrvTTDIsSceneChanged(void) 
{
    UINT32 u4APLDiff = 0;

    // wait several vsync and return SV_TRUE, we do this to make sure image is stable.
    if(g_u1SceneChangedFlag == SV_TRUE)
    {
        if(g_u1SceneChangedCounter >= IO32ReadFldAlign(TTD_FW_REG_13, TTD_SCN_CHG_CNT_THR))
        {
            g_u1SceneChangedFlag = SV_FALSE;
            g_u1SceneChangedCounter = 0;
            return SV_TRUE;
        }
        else
        {
            g_u1SceneChangedCounter++;
            return SV_FALSE;
        }
    }
   
    // 1. Get APL from SCE, Get diversity from TTD HW
    gTTDInfo.ttdCurAPL = bDrvGetAPL();

    // 2. calculate diff with previous one.
    u4APLDiff = DIFF(gTTDInfo.ttdCurAPL, gTTDInfo.ttdPreAPL);
    
    // 3. Scene change Condition 1: APL change exceed threshold, 2: diversity change exceed threshold
    if(u4APLDiff > IO32ReadFldAlign(TTD_FW_REG_13, TTD_APL_SCN_CHG_TH))
    {
        g_u1SceneChangedFlag = SV_TRUE;
        g_u1SceneChangedCounter = 0;
        LOG(9,"Scene Changed:: APL from(%d) to(%d), Diff(%d)\n", 
                gTTDInfo.ttdPreAPL, gTTDInfo.ttdCurAPL, u4APLDiff);
    }

    // 4. set current APL and diversity to previous
    gTTDInfo.ttdPreAPL = gTTDInfo.ttdCurAPL;

    return SV_FALSE;
}

static void vDrvTTDCheckLocalInverseMode()
{
    UINT8 fgInverse = SV_FALSE;
    static UINT32 u4InvState = 0x10;

    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_HIST_EN))
    {
        fgInverse = bDrvTTDCheckCIDInverse();
        vIO32WriteFldAlign(TTD_FW_REG_00, fgInverse, TTD_FW_IS_INVERSE); 
        fgInverse = IO32ReadFldAlign(TTD_FW_REG_00, TTD_INVERSE_FW_RESULT) ? (!fgInverse) : fgInverse;
    }
    gTTDInfo.ttdFgLocalInverse = fgInverse;

    if (fgInverse)
    {
        if (u4InvState != 0)
            u4InvState--;
    }
    else
    {
        if (u4InvState < 0x20)
            u4InvState++;
    }

    vIO32WriteFldAlign(TTDDG_02, u4InvState, DG_TFU_INV_STAT); 
	
    LOG(9, "TTD Final Iverse = %d, Inverse State = %d\n", fgInverse, u4InvState);
}


