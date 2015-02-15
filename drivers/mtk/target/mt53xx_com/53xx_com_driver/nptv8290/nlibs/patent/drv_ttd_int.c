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
#define AbsDiff(a,b) ((a)>=(b) ? (a)-(b) : (b)-(a))

/*----------------------------------------------------------------------*
 * Global/Static Variables
 *----------------------------------------------------------------------*/
UINT8 g_u1SceneChangedFlag = SV_FALSE;
UINT8 g_u1SceneChangedCounter = 0;

/*----------------------------------------------------------------------*
 * Static Variables & Functions
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 * External Variables & Functions
 *----------------------------------------------------------------------*/

//===========================
//  Adaptive PQ Algorithm
//===========================
void vDrvTTDSetGlobalModel(void)
{
    static UINT32 u4PreRgnMax = 100;
    UINT32 u4SmoothWeight, u4Value, u4RgnMax;

    // Read regional histogram and perform IIR
    u4SmoothWeight = IO32ReadFldAlign(TTD_FW_REG_15, TTD_HORI_SM_WEIGHT);
    u4RgnMax = IO32ReadFldAlign(STA_TTDVP_18, STA_GM_MAX_HIST_DIST);
    u4Value = gTTDInfo.ttdInputWidth;
    if ((u4Value != PANEL_GetPanelWidth()) && (u4Value != 0))
    {
        u4RgnMax = (u4RgnMax * PANEL_GetPanelWidth()) / u4Value;
    }
    u4PreRgnMax = (u4PreRgnMax * (1024 - u4SmoothWeight) + 
                    (u4RgnMax << 4) * u4SmoothWeight + 512) / 1024;
    u4RgnMax = (u4PreRgnMax + 8) / 16;
    vIO32WriteFldAlign(TTD_FW_REG_17, u4RgnMax, TTD_RGN_MAX);
}

void vDrvTTDCalImgChar(void)
{
    UINT16* Histogram = gTTDInfo.ttdHist;
    UINT16* HueHistogram = gTTDInfo.ttdHueHist;
    UINT32 i, left, right, u4Diversity, u4Variance, u4HueVariance, u4HueAvg, u4Value;
    UINT32 hist_thld = IO32ReadFldAlign(TTD_FW_REG_03, TTD_HIST_THLD);
    UINT32 div_thld_h = IO32ReadFldAlign(TTD_FW_REG_03, TTD_DIVER_THLD_H);  
    UINT32 u4APL;
    static UINT32 u4PreHueVar = 255;

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
    }
    u4HueVariance = MIN(MAX(u4HueVariance >> 15, 1), 255);

    // Perform IIR
    u4PreHueVar = (u4PreHueVar * 896 +  (u4HueVariance << 4) * 128 + 512) / 1024;
    u4HueVariance = (u4PreHueVar + 8) / 16;

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
 
void vDrvTTDAdaptiveWeight(void)
{
    UINT32 u4GlobalWeight, u4LocalWeight, u4TFUWeight, u4TFUUGain;
    UINT32 u4RgnMax = IO32ReadFldAlign(TTD_FW_REG_17, TTD_RGN_MAX);
    UINT32 u4LocalMax = IO32ReadFldAlign(TTD_FW_REG_12, TTD_LOCAL_MAX);
    UINT32 u4LocalMin = IO32ReadFldAlign(TTD_FW_REG_12, TTD_LOCAL_MIN);
    UINT32 u4GlobalMax = IO32ReadFldAlign(TTD_FW_REG_12, TTD_GLOBAL_MAX);
    UINT32 u4GlobalMin = IO32ReadFldAlign(TTD_FW_REG_12, TTD_GLOBAL_MIN);
    UINT32 u4SWGlobalGain = (IO32ReadFldAlign(TTD_02, TTD_IMG_WIDTH)<720) ? 0 : IO32ReadFldAlign(TTD_FW_REG_11, TTD_GLOBAL_GAIN);
    UINT32 u4SWLocalGain;
    UINT32 u4SWTFUGain = IO32ReadFldAlign(TTD_FW_REG_17, TTD_TFU_GAIN);
    UINT32 u4Variance = IO32ReadFldAlign(TTD_FW_REG_11, TTD_VARIANCE);
    UINT32 u4HueVariance = IO32ReadFldAlign(TTD_FW_REG_11, TTD_HUE_VARIANCE);
    UINT32 u4NormFact = IO32ReadFldAlign(TTD_FW_REG_18, TTD_RGN_NORM_FACT);
    static UINT32 u4HueVarianceRec = 0;

    u4NormFact = (u4NormFact == 0) ? 1 : u4NormFact;
    u4RgnMax = u4RgnMax /u4NormFact;

    // Global weight
    u4GlobalWeight = (u4RgnMax > 270) ? ((((u4RgnMax - 270) * 12 + 40) /80) +4) : 4;
    u4GlobalWeight = (u4GlobalWeight * u4SWGlobalGain + 64) >> 7;
    u4GlobalWeight = MIN(MAX(u4GlobalWeight, u4GlobalMin), u4GlobalMax);

    // Local wieght
    u4LocalWeight = (u4RgnMax < 496) ? ((((496 - u4RgnMax) * 12 + 128) >> 8) + 4) : 4;
    u4LocalWeight = MIN(MAX(u4LocalWeight, u4LocalMin), u4LocalMax);

    u4SWLocalGain = (u4Variance < 176) ? 0x80 : ((17203 - 51 * u4Variance) >> 6);
    u4LocalWeight = (u4LocalWeight * u4SWLocalGain + 64) >> 7;

    u4SWLocalGain = IO32ReadFldAlign(TTD_FW_REG_11, TTD_LOCAL_GAIN);
    u4LocalWeight = (u4LocalWeight * u4SWLocalGain + 64) >> 7;
    u4LocalWeight = MIN(MAX(u4LocalWeight, u4LocalMin), u4LocalMax);

    // TFU weight
    u4TFUWeight = (u4HueVariance < 50) ? 2 : ((((u4HueVariance - 50) * 6 + 32) >> 6) + 2);
    u4TFUWeight = (u4TFUWeight * u4SWTFUGain + 4) >> 3;
    u4TFUWeight = MIN(MAX(u4TFUWeight, 2), 8);
    u4TFUUGain = (u4TFUWeight > 2) ? ((((u4TFUWeight - 2) * 4 + 3) /6) + 8) : 8;
    
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_ADAP_GM_EN))
    {
        vDrvTTDSetGlobalWeight(u4GlobalWeight);
        vDrvTTDSetLocalWeight(u4LocalWeight);

        if(AbsDiff(u4HueVarianceRec, u4HueVariance) > 3)
        {
            vDrvTTDSetTfuWeight(u4TFUWeight);
            vIO32WriteFldAlign(TTDDG_02, u4TFUUGain, GAINU1);
            vIO32WriteFldAlign(TTDDG_02, u4TFUUGain, GAINU2);
            u4HueVarianceRec = u4HueVariance;
        }
    }    
}

UINT32 u4DrvTTDIsSceneChanged(void) 
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
        LOG(9,"Scene Changed: APL from(%d) to(%d), Diff(%d)\n", 
                gTTDInfo.ttdPreAPL, gTTDInfo.ttdCurAPL, u4APLDiff);
    }

    // 4. set current APL and diversity to previous
    gTTDInfo.ttdPreAPL = gTTDInfo.ttdCurAPL;

    return SV_FALSE;
}


