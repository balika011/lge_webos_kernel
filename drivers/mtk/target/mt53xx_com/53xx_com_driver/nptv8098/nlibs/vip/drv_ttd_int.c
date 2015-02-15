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
#include "drv_ttd_RandomForest_FGBG.h"


/*---------------------------------------------------------------------*
 * definition
 *---------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 * Global/Static Variables
 *----------------------------------------------------------------------*/
UINT8 g_u1SceneChangedFlag = SV_FALSE;
UINT8 g_u1SceneChangedCounter = 0;


/*----------------------------------------------------------------------*
 * Static Variables & Functions
 *----------------------------------------------------------------------*/

static void vDrvTTDCheckLocalInverseMode(void);
static void vDrvTTDAdaptiveWeight(void);
static void vDrvTTDCalImgChar(void);
static void vDrvTTDSetGMVertGain(INT32 i4Value);
static void vDrvTTDSetGMVertGain2(INT32 i4Value);
static UINT8 u1DrvTTDIsSceneChanged(void);
static UINT8 u1DrvTTDCheckCIDInverse(void);
static UINT8 u1DrvTTDCheckCIDInverseV1(void);
static UINT8 u1DrvTTDCheckCIDInverseV2(void);
static UINT8 u1DrvTTDCheckCIDInverseV3(void);


/*----------------------------------------------------------------------*
 * External Variables & Functions
 *----------------------------------------------------------------------*/


//===========================
//  Adaptive PQ Algorithm
//===========================
static void vDrvTTDSetGMVertGain(INT32 i4Value)
{
    UINT32 u4Temp;
    if (IS_ECO_IC())
    {
        u4Temp = (i4Value >> 6) & 0x1F;
        vIO32WriteFldAlign(TTDVP_20, u4Temp, GM_VERT_GAIN_MSB);
        u4Temp = i4Value & 0x3F;
        vIO32WriteFldAlign(TTDVP_07, u4Temp, GM_VERT_GAIN);
    }
    else
    {
        u4Temp = MIN(i4Value, 0x3F);
        vIO32WriteFldAlign(TTDVP_07, u4Temp, GM_VERT_GAIN);
    }
}

static void vDrvTTDSetGMVertGain2(INT32 i4Value)
{
    UINT32 u4Temp;
    if (IS_ECO_IC())
    {
        u4Temp = (i4Value >> 6) & 0x1F;
        vIO32WriteFldAlign(TTDVP_20, u4Temp, GM_VERT_GAIN2_MSB);
        u4Temp = i4Value & 0x3F;
        vIO32WriteFldAlign(TTDVP_13, u4Temp, GM_VERT_GAIN2);
    }
    else
    {
        u4Temp = MIN(i4Value, 0x3F);
        vIO32WriteFldAlign(TTDVP_13, u4Temp, GM_VERT_GAIN2);
    }
}

void vDrvTTDSetGlobalModel(void)
{
    static UINT16 u4GMHoriCtrlPt[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    static UINT32 u4GMOfst = 0;
    static UINT32 u4PreRgnMax = 100;

    UINT32 u4CurCtrlPt[9], u4SmoothWeight, u4Idx, u4Value, u4RgnMax;
    UINT32 u4EdgeCnt0, u4EdgeCnt1, u4EdgeCnt2, u4EdgeCntMax, u4Sign, u4GainA0, u4GainA1;
    UINT32 u4ModelGain, u4GainMin, u4GainMax;
    INT32 i4CurtA;
    const INT32 i4Ap = 576;
    const INT32 i4B = 960;
    static UINT8 bForceUpdateCnt = 0;    
    // Set horizontal control points
    // Read target control points
    u4SmoothWeight = IO32ReadFldAlign(TTD_FW_REG_15, TTD_HORI_SM_WEIGHT);
    u4CurCtrlPt[0] = IO32ReadFldAlign(STA_TTDVP_20, STA_GM_HORI_CTRL_PT_0);
    u4Value = u4IO32Read4B(STA_TTDVP_21);
    u4CurCtrlPt[1] = (u4Value >> 24) & 0xFF;
    u4CurCtrlPt[2] = (u4Value >> 16) & 0xFF;
    u4CurCtrlPt[3] = (u4Value >> 8) & 0xFF;
    u4CurCtrlPt[4] = u4Value & 0xFF;
    u4Value = u4IO32Read4B(STA_TTDVP_22);
    u4CurCtrlPt[5] = (u4Value >> 24) & 0xFF;
    u4CurCtrlPt[6] = (u4Value >> 16) & 0xFF;
    u4CurCtrlPt[7] = (u4Value >> 8) & 0xFF;
    u4CurCtrlPt[8] = u4Value & 0xFF;

    if(IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_FORCE_UPDATE) == SV_TRUE)
    {
        u4SmoothWeight = 1024;
        bForceUpdateCnt = IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_FORCE_SC_CNT);
        vIO32WriteFldAlign(TTD_FW_REG_00, 0, TTD_FW_FORCE_UPDATE);          
    }
    else if(bForceUpdateCnt > 0)
    {
        u4SmoothWeight = 1024;        
        bForceUpdateCnt--;   
    }
    else
    {
        u4SmoothWeight = IO32ReadFldAlign(TTD_FW_REG_15, TTD_HORI_SM_WEIGHT);
    }
    // Calculate IIR result
    for (u4Idx = 0; u4Idx < 9; u4Idx++)
    {
        u4GMHoriCtrlPt[u4Idx] = ((u4GMHoriCtrlPt[u4Idx] * (1024 - u4SmoothWeight)) + 
                                ((u4CurCtrlPt[u4Idx] << 4) * u4SmoothWeight)) / 1024;
        u4CurCtrlPt[u4Idx] = ((u4GMHoriCtrlPt[u4Idx] + 8) / 16) & 0xFF;
    }

    // Set HW control points
    u4Value = (u4CurCtrlPt[0] << 24) | (u4CurCtrlPt[1] << 16) |
              (u4CurCtrlPt[2] << 8) | (u4CurCtrlPt[3]);
    vIO32Write4B(TTDVP_17, u4Value);
    u4Value = (u4CurCtrlPt[4] << 24) | (u4CurCtrlPt[5] << 16) |
              (u4CurCtrlPt[6] << 8) | (u4CurCtrlPt[7]);
    vIO32Write4B(TTDVP_18, u4Value);
    vIO32WriteFldAlign(TTDVP_19, u4CurCtrlPt[8], GM_HORI_CTRL_PT_8);

    // Read regional histogram and perform IIR
    u4RgnMax = IO32ReadFldAlign(STA_TTDVP_18, STA_GM_MAX_HIST_DIST);
#if defined(CC_MT5398) 
    u4Value = IO32ReadFldAlign(TTD_02, TTD_IMG_WIDTH);
    if (u4Value != PANEL_GetPanelWidth())
    {
        u4RgnMax = (u4RgnMax * PANEL_GetPanelWidth()) / u4Value;
    }
#endif

    if(u4RgnMax == 0)
    {
        u4PreRgnMax = (u4PreRgnMax * (1024 - 0) + 
                        (u4RgnMax << 4) * 0 + 512) / 1024;
        u4RgnMax = (u4PreRgnMax + 8) / 16;
//        u4SmoothWeight = 0;

    }
    else
    {
        u4PreRgnMax = (u4PreRgnMax * (1024 - u4SmoothWeight) + 
                        (u4RgnMax << 4) * u4SmoothWeight + 512) / 1024;
        u4RgnMax = (u4PreRgnMax + 8) / 16;

    }

    vIO32WriteFldAlign(TTD_FW_REG_17, u4RgnMax, TTD_RGN_MAX);

    // Set vertical model coefficient
    // Set global model base due to vertical model gain change
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_ADAP_VERT_BASE))
    {
        u4SmoothWeight = IO32ReadFldAlign(TTD_FW_REG_15, TTD_VERT_SM_WEIGHT);
        u4EdgeCnt0 = IO32ReadFldAlign(STA_TTDVP_24, STA_GM_VERT_EDGE_CNT_0);
        u4EdgeCnt1 = IO32ReadFldAlign(STA_TTDVP_24, STA_GM_VERT_EDGE_CNT_1);
        u4EdgeCnt2 = IO32ReadFldAlign(STA_TTDVP_25, STA_GM_VERT_EDGE_CNT_2);
        u4EdgeCntMax = MAX(MAX(u4EdgeCnt0, u4EdgeCnt1), u4EdgeCnt2);

        u4ModelGain = IO32ReadFldAlign(TTDVP_06, GM_VERT_MODEL_GAIN) << 7;
        u4GainMin = IO32ReadFldAlign(TTDVP_06, GM_MINUS_VERT_MIN_GAIN) << 8;
        u4GainMax = IO32ReadFldAlign(TTDVP_06, GM_VERT_MAX_GAIN) << 8;

        u4Sign = 0;
        u4GainA0 = 0;
        u4GainA1 = 0;

        if (u4EdgeCntMax != 0)
        {
            if ((u4EdgeCnt0 > u4EdgeCnt1) && (u4EdgeCnt2 > u4EdgeCnt1))
            {
                u4Sign = 0;
                u4GainA0 = MIN((u4ModelGain * (u4EdgeCnt0 - u4EdgeCnt1)) / u4EdgeCntMax, u4GainMax);
                u4GainA1 = MIN((u4ModelGain * (u4EdgeCnt2 - u4EdgeCnt1)) / u4EdgeCntMax, u4GainMax);
            }
            else if ((u4EdgeCnt1 > u4EdgeCnt0) && (u4EdgeCnt1 > u4EdgeCnt2))
            {
                u4Sign = 1;
                u4GainA0 = MIN((u4ModelGain * (u4EdgeCnt1*2 - u4EdgeCnt0 - u4EdgeCnt2)/2) / u4EdgeCntMax, u4GainMin);
                u4GainA1 = u4GainA0;
            }
        }

        vIO32WriteFldAlign(TTDVP_07, u4Sign, GM_VERT_GAIN_SIGN);
        vIO32WriteFldAlign(TTDVP_13, u4Sign, GM_VERT_GAIN2_SIGN);
        vDrvTTDSetGMVertGain(u4GainA0/4);
        vDrvTTDSetGMVertGain2(u4GainA1/4);
        i4CurtA = ((u4GainA0 + u4GainA1) << 8) / 2;

        // Make range of "a" is (-576~576)
        i4CurtA = ABS((i4CurtA*9 + 16384) / 32768);
        vIO32WriteFldAlign(TTD_FW_REG_13, i4CurtA, TTD_VERT_GAIN);

        // D=((a'-a)b^2/2)/65535
        u4Value = ((i4Ap-i4CurtA)*i4B*i4B/2 + 32768) / 65536;

        // SW IIR
        u4GMOfst = ((u4GMOfst * (1024 - u4SmoothWeight)) + ((u4Value << 4) * u4SmoothWeight) + 512) / 1024;

        // 2^10(vertical weight)x 2^5(global weight)x 2^4(IIR) => 2^19
        u4Value = 128 - MIN(128, (u4GMOfst*IO32ReadFldAlign(TTDVP_14, GM_VERT_WEIGHT)+262144) / 524288);
        vIO32WriteFldAlign(TTDDG_01, u4Value, DG_GLOBAL_BASE);
    }
}

void vDrvTTDCheckTransition(void)
{
    static UINT8 bDbgCnt = 0;    

    if ((u1DrvTTDIsSceneChanged() == SV_TRUE) || 
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

static void vDrvTTDCalImgChar(void)
{
    UINT16* Histogram = gTTDInfo.ttdHist;
    UINT16* HueHistogram = gTTDInfo.ttdHueHist;
    UINT32 i, left, right, u4Diversity, u4Variance, u4HueVariance, u4HueAvg, u4Value;
    UINT32 hist_thld = IO32ReadFldAlign(TTD_FW_REG_03, TTD_HIST_THLD);
    UINT32 div_thld_h = IO32ReadFldAlign(TTD_FW_REG_03, TTD_DIVER_THLD_H);  
    UINT32 u4APL;

#ifdef CC_MT5880
    if (IO32ReadFldAlign(TTD_03, INK_DEPTH_Y) != 0) {return;}
#endif

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
 
static void vDrvTTDAdaptiveWeight(void)
{
    UINT32 u4GlobalWeight, u4LocalWeight, u4TFUWeight;
    UINT32 u4RgnMax = IO32ReadFldAlign(TTD_FW_REG_17, TTD_RGN_MAX);
    UINT32 u4LocalMax = IO32ReadFldAlign(TTD_FW_REG_12, TTD_LOCAL_MAX);
    UINT32 u4LocalMin = IO32ReadFldAlign(TTD_FW_REG_12, TTD_LOCAL_MIN);
    UINT32 u4GlobalMax = IO32ReadFldAlign(TTD_FW_REG_12, TTD_GLOBAL_MAX);
    UINT32 u4GlobalMin = IO32ReadFldAlign(TTD_FW_REG_12, TTD_GLOBAL_MIN);
    UINT32 u4SWGlobalGain =   (IO32ReadFldAlign(TTD_02, TTD_IMG_WIDTH)<720) ? 0 : IO32ReadFldAlign(TTD_FW_REG_11, TTD_GLOBAL_GAIN);
    UINT32 u4SWLocalGain;
    UINT32 u4SWTFUGain = IO32ReadFldAlign(TTD_FW_REG_17, TTD_TFU_GAIN);
    UINT32 u4Variance = IO32ReadFldAlign(TTD_FW_REG_11, TTD_VARIANCE);
    UINT32 u4HueVariance = IO32ReadFldAlign(TTD_FW_REG_11, TTD_HUE_VARIANCE);

    // Global weight
    u4GlobalWeight = (u4RgnMax > 200) ? (((u4RgnMax - 200) * 19 + 576) >> 7) : 4;
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
    
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_ADAP_GM_EN))
    {
        vDrvTTDSetGlobalWeight(u4GlobalWeight);
        vDrvTTDSetLocalWeight(u4LocalWeight);
        vDrvTTDSetTfuWeight(u4TFUWeight);
    }    
}

static UINT8 u1DrvTTDCheckCIDInverseV1(void)
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

static UINT8 u1DrvTTDCheckCIDInverseV2(void)
{
    UINT32 u4Diversity = IO32ReadFldAlign(TTD_FW_REG_09, TTD_DIVER);
    UINT32 u4APL = IO32ReadFldAlign(TTD_FW_REG_09, TTD_CUR_AVG);
    UINT32 u4RgnMax = IO32ReadFldAlign(STA_TTDVP_18, STA_GM_MAX_HIST_DIST);
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

static UINT8 u1DrvTTDCheckCIDInverseV3(void)
{
    static int datum[57];
    UINT32 u4Index, u4MaxbinIdx, u4MaxbinCnt, u4APLbinIdx, u4BinSum, u4Inv;

    SIVPRandomForest rf;
    loadSIVPRandomForest(&rf);

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

    u4Inv = predict(&rf, datum);
    freeSIVPRandomForest(&rf);

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

static UINT8 u1DrvTTDCheckCIDInverse(void)
{    
    UINT32 u4Inv;

    switch (TTD_CHECK_INV_VER)
    {
        case TTD_CHECK_INV_V1:
            u4Inv = u1DrvTTDCheckCIDInverseV1();
            break;
        case TTD_CHECK_INV_V2:
            u4Inv = u1DrvTTDCheckCIDInverseV2();
            break;
        case TTD_CHECK_INV_V3:
            u4Inv = u1DrvTTDCheckCIDInverseV3();
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

/*  Scene change condition:
 *  1. APL change exceed threshold
 *  2. Diversity change exceed Threshold
 */
static UINT8 u1DrvTTDIsSceneChanged(void) 
{
    UINT32 u4APLDiff = 0, u4DiverDiff = 0;

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
#if defined(CC_MT5398)
    gTTDInfo.ttdCurDiver = IO32ReadFldAlign(STA_TTDVP_17, STA_GM_MAX_DIVERSITY);
#else
    gTTDInfo.ttdCurDiver = IO32ReadFldAlign(STA_TTDVP_26, STA_GM_MAX_DIVERSITY);
#endif

    // 2. calculate diff with previous one.
    u4APLDiff = DIFF(gTTDInfo.ttdCurAPL, gTTDInfo.ttdPreAPL);
    u4DiverDiff = DIFF(gTTDInfo.ttdCurDiver, gTTDInfo.ttdPreDiver);
    
    // 3. Scene change Condition 1: APL change exceed threshold, 2: diversity change exceed threshold
    if((u4APLDiff > IO32ReadFldAlign(TTD_FW_REG_13, TTD_APL_SCN_CHG_TH)) || 
        (u4DiverDiff > IO32ReadFldAlign(TTD_FW_REG_13, TTD_DIVER_SCN_CHG_TH)))
    {
        g_u1SceneChangedFlag = SV_TRUE;
        g_u1SceneChangedCounter = 0;
        LOG(9,"Scene Changed:: APL from(%d) to(%d), Diff(%d)\n", 
                gTTDInfo.ttdPreAPL, gTTDInfo.ttdCurAPL, u4APLDiff);
        LOG(9,"Scene Changed:: Diversity from(%d) to(%d), Diff(%d)\n", 
                gTTDInfo.ttdPreDiver, gTTDInfo.ttdCurDiver, u4DiverDiff);
    }

    // 4. set current APL and diversity to previous
    gTTDInfo.ttdPreAPL = gTTDInfo.ttdCurAPL;
    gTTDInfo.ttdPreDiver = gTTDInfo.ttdCurDiver;

    return SV_FALSE;
}

static void vDrvTTDCheckLocalInverseMode()
{
    UINT8 fgInverse = SV_FALSE;
    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_HIST_EN))
    {
        fgInverse = u1DrvTTDCheckCIDInverse();
        vIO32WriteFldAlign(TTD_FW_REG_00, fgInverse, TTD_FW_IS_INVERSE); 
        fgInverse = IO32ReadFldAlign(TTD_FW_REG_00, TTD_INVERSE_FW_RESULT) ? (!fgInverse) : fgInverse;
    }
    gTTDInfo.ttdFgLocalInverse = fgInverse;
    vIO32WriteFldAlign(TTDVP_09, fgInverse, VP_LOCAL_INVERSE_FLAG); 
	
    LOG(9, "TTD Final Iverse = %d\n", fgInverse);
}


