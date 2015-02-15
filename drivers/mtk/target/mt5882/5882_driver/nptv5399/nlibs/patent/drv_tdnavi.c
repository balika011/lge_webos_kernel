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
 * $RCSfile: drv_mjc_int.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/

#include "x_assert.h"
#include "hw_tdnavi.h"

#include "drv_tdtv_feature.h"
#include "drv_tdnavi.h"
#include "vdo_misc.h"
#include "sv_const.h"
#include "vdp_if.h"
#include "mute_if.h"
#include "video_def.h"


#ifdef CC_MT5399
#include "drv_mjc.h"
#endif

enum
{
    E_TDNAVI_WAIT_DUMP      =0,
    E_TDNAVI_DUMPING        =1,
    E_TDNAVI_SRAM_REFRESH   =2,
    E_TDNAVI_PROCESSING     =3,
};

/*----------------------------------------------------------------------------*
 * The Result of NaviV1 + NaviV2 
 *----------------------------------------------------------------------------*/
UINT8 g_u1TdNaviDetRst = TD_NAVI_DETECT_2D;

static UINT8 u1IsInited =0;

/*----------------------------------------------------------------------------*
 * NaviV1 Global/Static variables
 *----------------------------------------------------------------------------*/
//#define TD_NAVI_DETECT_FRAME_NUM	30 // 1~255
//#define TD_NAVI_2D_TO_3D_D_TH		218 // 1~255
//#define TD_NAVI_3D_TO_2D_D_TH		160 // 1~255

UINT8 g_u1TdNaviDetInFrmNum  = 10;
UINT8 g_u1TdNaviDetOutFrmNum = 10;

UINT8 g_u1In3DSimilarityRatio = 218;
UINT8 g_u1In3DSimilarThr= 5;
UINT8 g_u1In3DDecisionType= 1;
UINT8 g_u1VldDivInVldThr= 13; // 0~255 means (0~255)/16

UINT8 g_u1Out3DSimilarityRatio = 160; // RealD Test Pattern = 136
UINT8 g_u1Out3DSimilarThr = 10;
UINT8 g_u1Out3DDecisionType = 2;


UINT8 g_u1TdNaviDetNew = 0;
UINT8 g_u1TdNaviDetCurr = 0;
UINT8 g_u1TdNaviDet2DCnt = 0;
UINT8 g_u1TdNaviDetSbsCnt = 0;
UINT8 g_u1TdNaviDetTabCnt = 0;

UINT8 g_u1TdNaviAdaptOnOff = 1;
UINT32 eUINaviPatternDetect=TDTV_TAG3D_2D;

static const REGTBL_T TD_NAVI_INIT[] =
{
    {TDNVAI_00, 0x1550C003, 0x7FFFFFFF},
    {TDNVAI_01, 0x4004DA0A, 0xFFFFFFFE},
    {TDNVAI_02, 0x03000043, 0x070000FF},
    {TDNVAI_03, 0x21004000, 0x7FFFFFFF},
    {TDNVAI_04, 0x08060000, 0x3FFFFFFF},
    {REGTBL_END, 0x00000000, 0x00000000}
};

/*----------------------------------------------------------------------*
*NaviV2:LI,CB, DA, QC default paramter
*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/
static void vDrvTdNaviDet(void);
static UINT8 u1DrvTdNaviGetFwFrameDetType(void);
static void vDrvTdNaviStaConvert(void);
static void vDrvTdNaviGetAdaptSetting(void);

UINT8 vDrvTdNaviGetSbsCnt(void)
{
	return IO32ReadFldAlign(TDNVAI_STA1, STA_SBS_CNT);
}

UINT8 vDrvTdNaviGetTabCnt(void)
{
	return IO32ReadFldAlign(TDNVAI_STA1, STA_TAB_CNT);
}

UINT8 vDrvTdNaviGetSbsVldBlk(void)
{
	return IO32ReadFldAlign(TDNVAI_STA0, STA_SBS_VLD_BLK);
}

UINT8 vDrvTdNaviGetTabVldBlk(void)
{
	return IO32ReadFldAlign(TDNVAI_STA0, STA_TAB_VLD_BLK);
}

UINT8 vDrvTdNaviGetSbsInvldBlk(void)
{
    return IO32ReadFldAlign(TDNVAI_STA1, STA_INVLD_SBS);
}

UINT8 vDrvTdNaviGetTabInvldBlk(void)
{
	return IO32ReadFldAlign(TDNVAI_STA1, STA_INVLD_TAB);
}

UINT32 u4DrvTdNaviGetBlkAPL(void)
{
   return IO32ReadFldAlign(TDNVAI_STA0, STA_BLK_APL);
}

void vDrvTdNaviSetPicSize(void)
{
	UINT16 u2Width, u2Height, u2RestX, u2RestY, u2BlkXMulBlkY;
    UINT8  u1BlkSizeX, u1BlkSizeY, u1IsIntr;
    
    u1IsIntr = bDrvVideoIsSrcInterlace(SV_VP_MAIN);	
    u2Width  = wDrvVideoInputWidth(SV_VP_MAIN);
    u2Height = wDrvVideoInputHeight(SV_VP_MAIN);
    
	u2Height = u1IsIntr ? (u2Height >> 1) : u2Height;
	
	u1BlkSizeX = (UINT8)(u2Width/30);
	u1BlkSizeY = (UINT8)(u2Height >> 4);
	u2RestX = (u2Width  >> 1) - (((UINT16)u1BlkSizeX) * 15);
	u2RestY = (u2Height >> 1) - (((UINT16)u1BlkSizeY) << 3);
	u2BlkXMulBlkY = ((UINT16)u1BlkSizeX) * ((UINT16)u1BlkSizeY);

    //for  new det setting
    vIO32WriteFldAlign(TDNVAI_05, u2Width, TDNAVI_DET_WIDTH);
    vIO32WriteFldAlign(TDNVAI_05, u2Height, TDNAVI_DET_HEIGHT);
	
	vIO32WriteFldAlign(TDNVAI_01, u1BlkSizeX, BLK_SIZE_X);
	vIO32WriteFldAlign(TDNVAI_02, u1BlkSizeY, BLK_SIZE_Y);
	vIO32WriteFldAlign(TDNVAI_03, u2RestX, BLK_X_ST);
	vIO32WriteFldAlign(TDNVAI_03, u2RestY, BLK_Y_ST);
	vIO32WriteFldAlign(TDNVAI_00, u2BlkXMulBlkY, BLK_X_MUL_BLK_Y);
}


void u1DrvTdNaviV1ModeChange(void)
{
	g_u1TdNaviDetNew = TD_NAVI_DETECT_2D;
    g_u1TdNaviDetCurr = TD_NAVI_DETECT_2D;
    g_u1TdNaviDetRst = TD_NAVI_DETECT_2D;
    g_u1TdNaviDet2DCnt = 0;
    g_u1TdNaviDetSbsCnt = 0;
    g_u1TdNaviDetTabCnt = 0;
   	vDrvTdNaviSetSimilarityRatio(g_u1In3DSimilarityRatio);
   	vDrvTdNaviSetSimilarityThr(g_u1In3DSimilarThr);
  	vDrvTdNaviSetDecisionType(g_u1In3DDecisionType);
    vDrvTdNaviSetPicSize();
}


UINT8 u1DrvTdNaviModeChange(void)
{
#if defined (CC_MT5399)
	if ((u1DrvGetMJCUniCandeceModeChangeFlg() == SV_TRUE) && (u1DrvMJCIsSupportUniCandece()))
	{
		vDrvSetMJCUniCandeceModeChangeFlg(SV_OFF);
	}
	else
	{
		u1DrvTdNaviV1ModeChange();
#if SUPPORT_3D_NAVI_V2
	    vDrvTdNaviV2ModeChange();
#endif
	}
#else
	u1DrvTdNaviV1ModeChange();
#if SUPPORT_3D_NAVI_V2
    vDrvTdNaviV2ModeChange();
#endif

#endif		
	return 1;
}    

void vDrvTdNaviInit(void)
{
    vDrvLoadRegTbl(TD_NAVI_INIT);

    vIO32WriteFldAlign(TDNVAI_00   , 0x10               , PXL_DIFF_THR); 

	vIO32WriteFldAlign(TDNAVI_FW_00, 0x0               , TDNAVI_SBS_LOG); 
	vIO32WriteFldAlign(TDNAVI_FW_00, 0x0               , TDNAVI_TAB_LOG); 
    
	vIO32WriteFldAlign(TDNAVI_FW_17, g_u1VldDivInVldThr, VLD_DIV_INVLD_THR); 
	vIO32WriteFldAlign(TDNAVI_FW_18, g_u1In3DSimilarityRatio, IN_3D_SIMILARITY_RATIO); 
	vIO32WriteFldAlign(TDNAVI_FW_18, g_u1In3DSimilarThr, IN_3D_SIMILAR_THR); 
	vIO32WriteFldAlign(TDNAVI_FW_18, g_u1In3DDecisionType, IN_3D_DECISION_TYPE); 
	vIO32WriteFldAlign(TDNAVI_FW_18, g_u1Out3DSimilarityRatio, OUT_3D_SIMILARITY_RATIO); 
	vIO32WriteFldAlign(TDNAVI_FW_18, g_u1Out3DSimilarThr, OUT_3D_SIMILAR_THR); 
	vIO32WriteFldAlign(TDNAVI_FW_18, g_u1Out3DDecisionType, OUT_3D_DECISION_TYPE); 

	vIO32WriteFldAlign(TDNVAI_05, 1, GDV_EN);
	vIO32WriteFldAlign(TDNVAI_05, 0, GDV2_EN);
	vIO32WriteFldAlign(TDNVAI_06, 5, DECISION_TYPE_EX);

    u1DrvTdNaviModeChange();
    vDrvTdNaviFWDetectionInit();
	
	
#if SUPPORT_3D_NAVI_V2
	vDrvTdNaviV2Init();
#endif

#if SUPPORT_REALD_TAG    	
	vDrvRealDInit();
#endif

    u1IsInited = SV_TRUE;

}

static void vDrvTdNaviDet(void)
{
    UINT8 u1StaSbsCnt, u1StaTabCnt, u1StaSbsVldBlk, u1StaTabVldBlk;

    vDrvTdNaviStaConvert();
    vDrvTdNaviFWDetectionProc();            

    // Get SW Adaptive Setting
    vDrvTdNaviGetAdaptSetting();

    u1StaSbsCnt = vDrvTdNaviGetSbsCnt();
    u1StaTabCnt = vDrvTdNaviGetTabCnt();
    u1StaSbsVldBlk = vDrvTdNaviGetSbsVldBlk();
    u1StaTabVldBlk = vDrvTdNaviGetTabVldBlk();
    g_u1TdNaviDetNew = u1DrvTdNaviGetFwFrameDetType();

    if(TD_NAVI_DETECT_2D==g_u1TdNaviDetNew)
    {
        g_u1TdNaviDetNew=eDrvSwNaviDetectedFmtQuery();
    }

    if(TD_NAVI_DETECT_UNKNOWN==g_u1TdNaviDetNew)
    {
        g_u1TdNaviDetNew = g_u1TdNaviDetCurr;
    }

    if (g_u1TdNaviAdaptOnOff)
    {
        if (TD_NAVI_DETECT_2D == g_u1TdNaviDetCurr)
        {
            // Current 2D, detect 3D
            vDrvTdNaviSetSimilarityRatio(g_u1In3DSimilarityRatio);
            vDrvTdNaviSetSimilarityThr(g_u1In3DSimilarThr);
            vDrvTdNaviSetDecisionType(g_u1In3DDecisionType);
        }
        else if((TD_NAVI_DETECT_SBS == g_u1TdNaviDetCurr)||(TD_NAVI_DETECT_TAB == g_u1TdNaviDetCurr))
        {
            // Current 3D, detect 2D
            vDrvTdNaviSetSimilarityRatio(g_u1Out3DSimilarityRatio);
            vDrvTdNaviSetSimilarityThr(g_u1Out3DSimilarThr);
            vDrvTdNaviSetDecisionType(g_u1Out3DDecisionType);
        }
        else
        {
            //Keep Previous State
        }
    }            

    if (TD_NAVI_DETECT_SBS == g_u1TdNaviDetCurr
	#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
	&&(eUINaviPatternDetect &TD_NAVI_DETECT_SBS)
	#endif
	)
    {

        if ((TD_NAVI_DETECT_2D == g_u1TdNaviDetNew) || (TD_NAVI_DETECT_TAB == g_u1TdNaviDetNew))
        {
            g_u1TdNaviDet2DCnt++;

            if(g_u1TdNaviDet2DCnt > g_u1TdNaviDetOutFrmNum)
            {
                g_u1TdNaviDetCurr = TD_NAVI_DETECT_2D;
                g_u1TdNaviDet2DCnt = 0;
            }
        }
        else // SBS, TOO_FEW, AMBIGUO, NEIGHBOR,Unknow
        {
            if (g_u1TdNaviDet2DCnt > 0)
            {
                g_u1TdNaviDet2DCnt--;
            }
        }
    }
 	else if(TD_NAVI_DETECT_TAB == g_u1TdNaviDetCurr
	#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
	&&(eUINaviPatternDetect &TD_NAVI_DETECT_TAB)
	#endif
	)
    {
        if ((TD_NAVI_DETECT_2D == g_u1TdNaviDetNew) || (TD_NAVI_DETECT_SBS == g_u1TdNaviDetNew))
        {
            g_u1TdNaviDet2DCnt++;

            if(g_u1TdNaviDet2DCnt > g_u1TdNaviDetOutFrmNum)
            {
                g_u1TdNaviDetCurr = TD_NAVI_DETECT_2D;
                g_u1TdNaviDet2DCnt = 0;
            }
        }
        else // TB, TOO_FEW, AMBIGUO, NEIGHBOR,Unknow
        {
            if (g_u1TdNaviDet2DCnt > 0)
            {
                g_u1TdNaviDet2DCnt--;
            }
        }
    }
    else //(TD_NAVI_DETECT_2D == g_u1TdNaviDetCurr)
    {		
        if (TD_NAVI_DETECT_SBS == g_u1TdNaviDetNew
		#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_DETECT_SBS)
		#endif
		)
        {
            g_u1TdNaviDetSbsCnt++;

            if (g_u1TdNaviDetTabCnt > 0)
            {
                g_u1TdNaviDetTabCnt--;
            }

            if(g_u1TdNaviDetSbsCnt > g_u1TdNaviDetInFrmNum)
            {
                g_u1TdNaviDetCurr = TD_NAVI_DETECT_SBS;
                g_u1TdNaviDetSbsCnt = 0;
                g_u1TdNaviDetTabCnt = 0;
            }
        }
        else if (TD_NAVI_DETECT_TAB == g_u1TdNaviDetNew
		#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_DETECT_TAB)
		#endif
		)
        {
            g_u1TdNaviDetTabCnt++;

            if (g_u1TdNaviDetSbsCnt > 0)
            {
                g_u1TdNaviDetSbsCnt--;
            }

            if(g_u1TdNaviDetTabCnt > g_u1TdNaviDetInFrmNum)
            {
                g_u1TdNaviDetCurr = TD_NAVI_DETECT_TAB;
                g_u1TdNaviDetSbsCnt = 0;
                g_u1TdNaviDetTabCnt = 0;
            }
        }
        else // 2D or unknown
        {
            if (g_u1TdNaviDetSbsCnt > 0)
            {
                g_u1TdNaviDetSbsCnt--;
            }

            if (g_u1TdNaviDetTabCnt > 0)
            {
                g_u1TdNaviDetTabCnt--;
            }
        }
    }

    if(IO32ReadFldAlign(TDNAVI_FW_00, TDNAVI_SBS_LOG)&&u1StaSbsVldBlk!=0)
    {
        LOG(0,"[NAVI SBS]Fmt =%1x SbsCnt = %2x SbsVld = %2x sbsRat = %2x,g_u1TdNaviDetCurr=%d\n", g_u1TdNaviDetNew,u1StaSbsCnt
                ,u1StaSbsVldBlk, (((UINT16)u1StaSbsCnt) << 8)/u1StaSbsVldBlk,g_u1TdNaviDetCurr);
    }

    if(IO32ReadFldAlign(TDNAVI_FW_00, TDNAVI_TAB_LOG)&&u1StaTabVldBlk!=0)
    {
        LOG(0,"[NAVI TAB]Fmt =%1x TabCnt = %2x TabVld = %2x TabRat = %2x,g_u1TdNaviDetCurr=%d\n", g_u1TdNaviDetNew,u1StaTabCnt
                ,u1StaTabVldBlk, (((UINT16)u1StaTabCnt) << 8)/u1StaTabVldBlk,g_u1TdNaviDetCurr);
    }

}

static UINT8 u1DrvTdNaviGetFwFrameDetType(void)
{
	UINT8 u1FwFrameDetType;
    
	u1FwFrameDetType = vDrvTdNaviGetNewDetType();
    
	vIO32WriteFldAlign(TDNAVI_STATUS_16, u1FwFrameDetType  , STA_FW_DET_TYPE);

    if((u1FwFrameDetType!=TD_NAVI_DETECT_2D)
        &&(u1FwFrameDetType!=TD_NAVI_DETECT_SBS)
        &&(u1FwFrameDetType!=TD_NAVI_DETECT_TAB))
    {
        u1FwFrameDetType = TD_NAVI_DETECT_UNKNOWN;
    }

	return u1FwFrameDetType;
}

static void vDrvTdNaviStaConvert(void)
{
	UINT32 u4SbsCnt, u4TabCnt, u4SbsVld, u4TabVld;
	UINT32 u4SbsPer, u4TabPer, u4CntDiff, u4VldMin;
	UINT32 u4VldMax, u4Ambiguo;
	UINT32 u4SbsNbrCnt, u4TabNbrCnt, u4MinNbrCnt;
	
	u4SbsCnt = vDrvTdNaviGetSbsCnt();
	u4TabCnt = vDrvTdNaviGetTabCnt();
	u4SbsVld = vDrvTdNaviGetSbsVldBlk();
	u4TabVld = vDrvTdNaviGetTabVldBlk();
    
	u4SbsNbrCnt = (UINT32)(IO32ReadFldAlign(TDNVAI_STA2, STA_SBS_NEIGHBOR_DIFF_CNT));
	u4TabNbrCnt = (UINT32)(IO32ReadFldAlign(TDNVAI_STA2, STA_TAB_NEIGHBOR_DIFF_CNT));

	u4VldMax = (u4SbsVld > u4TabVld) ? u4SbsVld : u4TabVld;
	u4VldMin = (u4SbsVld < u4TabVld) ? u4SbsVld : u4TabVld;
	u4CntDiff = (u4SbsCnt > u4TabCnt) ? (u4SbsCnt - u4TabCnt) : (u4TabCnt - u4SbsCnt);

	// If u4CntDiff = 0, AMBIGUO always valid
	// If u4VldMin  = 0, AMBIGUO always invalid
	u4Ambiguo = (u4CntDiff > 0) ? (u4VldMin/u4CntDiff) : 256;
	u4Ambiguo = (u4Ambiguo > 256) ? 256 : u4Ambiguo;

	u4MinNbrCnt = (u4SbsNbrCnt < u4TabNbrCnt) ? u4SbsNbrCnt : u4TabNbrCnt;

	u4SbsPer = (u4SbsVld > 0) ? ((u4SbsCnt << 8)/u4SbsVld) : 0;
	u4TabPer = (u4TabVld > 0) ? ((u4TabCnt << 8)/u4TabVld) : 0;
	
	vIO32WriteFldAlign(TDNAVI_STATUS_16, (UINT8)u4Ambiguo, STA_AMBIGUO); 
	vIO32WriteFldAlign(TDNAVI_STATUS_16, (UINT8)u4VldMax, STA_TOO_FEW); 
	vIO32WriteFldAlign(TDNAVI_STATUS_17, (UINT16)u4SbsPer, STA_SBS_RATE); 
	vIO32WriteFldAlign(TDNAVI_STATUS_17, (UINT16)u4TabPer, STA_TAB_RATE); 
	vIO32WriteFldAlign(TDNAVI_STATUS_18, (UINT16)u4MinNbrCnt, STA_NEIGHBOR); 
}


static void vDrvTdNaviGetAdaptSetting(void)
{

    if(E_TDTV_UI_NAVI_FULL_AUTO==eDrvTDTVTDNavimModeQuery())
    {
        g_u1In3DSimilarityRatio  = 0xDA;   //0xF8; add new feature, 248  setting is difficult to dect 3D
        g_u1In3DSimilarThr       = 0x5;
        
        g_u1Out3DSimilarityRatio = 0xA0;
        g_u1Out3DSimilarThr      = 0xA;
        
        vIO32WriteFldAlign(TDNVAI_01, 0x4, AMBIG_THR); //0x2; add new feature, 0x2  setting is difficult to dect 3D
        
    }
    else
    {
        g_u1In3DSimilarityRatio  = IO32ReadFldAlign(TDNAVI_FW_18, IN_3D_SIMILARITY_RATIO); 
        g_u1In3DSimilarThr       = IO32ReadFldAlign(TDNAVI_FW_18, IN_3D_SIMILAR_THR); 

        g_u1Out3DSimilarityRatio = IO32ReadFldAlign(TDNAVI_FW_18, OUT_3D_SIMILARITY_RATIO); 
        g_u1Out3DSimilarThr      = IO32ReadFldAlign(TDNAVI_FW_18, OUT_3D_SIMILAR_THR); 
        
        vIO32WriteFldAlign(TDNVAI_01, 0x4, AMBIG_THR); 
    }
    
    g_u1VldDivInVldThr       = IO32ReadFldAlign(TDNAVI_FW_17, VLD_DIV_INVLD_THR); 
    g_u1In3DDecisionType     = IO32ReadFldAlign(TDNAVI_FW_18, IN_3D_DECISION_TYPE);     
    g_u1Out3DDecisionType    = IO32ReadFldAlign(TDNAVI_FW_18, OUT_3D_DECISION_TYPE); 
}

UINT8 vDrvTdNaviGetNewDetType(void)
{
	return IO32ReadFldAlign(TDNVAI_STA2, STA_DET_TYPE);	
}

UINT8 vDrvTdNaviGetCurrDetType(void)
{
 	if((bGetVideoDecType(SV_VP_MAIN) > SV_VD_NA)
	&&(bGetVideoDecType(SV_VP_MAIN) < SV_VD_MAX)
	&&(!fgIs4k2kNativeTimingOut(SV_VP_MAIN))
	&& bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
	{
		if(IO32ReadFldAlign(TDNAVI_FW_00, TDNAVI_RST_LOG))
    	{
        	LOG(0,"[NAVI] CURRENT RST TYPE IS %d\n",g_u1TdNaviDetRst);
    	}
		return g_u1TdNaviDetRst;	
	}
	if(IO32ReadFldAlign(TDNAVI_FW_00, TDNAVI_RST_LOG))
	{
    	LOG(0,"[NAVI] CURRENT RST TYPE IS %d\n",g_u1TdNaviDetRst);
	}
	return TD_NAVI_DETECT_2D;
}

void vDrvTdNaviSetSimilarityRatio(UINT8 u1Ratio)
{
	vIO32WriteFldAlign(TDNVAI_01, u1Ratio, D_TH);
}

void vDrvTdNaviSetSimilarityThr(UINT8 u1Thr)
{
	vIO32WriteFldAlign(TDNVAI_00, u1Thr, SIMILAR_THR);
}

void vDrvTdNaviSetDecisionType(UINT8 u1Type)
{
	vIO32WriteFldAlign(TDNVAI_03, u1Type, DECISION_TYPE);
}

void vDrvTdNaviSetDbgType(UINT8 u1Type)
{
	vIO32WriteFldAlign(TDNVAI_03, u1Type, TDNAVI_DBG);
	vIO32WriteFldAlign(TDNVAI_03, (u1Type == 1), FW_DBG);
}

static void vDrvTdNaviJudgeRule(void)
{
	if((bGetVideoDecType(SV_VP_MAIN) > SV_VD_NA)
		&&(bGetVideoDecType(SV_VP_MAIN) < SV_VD_MAX)
		&&(!fgIs4k2kNativeTimingOut(SV_VP_MAIN))
		&&(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE))
	{
	    //Setup detection result
		if(TD_NAVI_DETECT_SBS==g_u1TdNaviDetCurr)
		{
			g_u1TdNaviDetRst = TD_NAVI_DETECT_SBS;	
			// Ink Light Blue
			vIO32WriteFldAlign(TDNVAI_04, 0x80, FW_DBG_INK_Y);
			vIO32WriteFldAlign(TDNVAI_04, 0xFF, FW_DBG_INK_C);			
			return;
		}

	    if(TD_NAVI_DETECT_TAB==g_u1TdNaviDetCurr)
		{
			g_u1TdNaviDetRst = TD_NAVI_DETECT_TAB;	
			// Ink Light Green
			vIO32WriteFldAlign(TDNVAI_04, 0xFF, FW_DBG_INK_Y);
			vIO32WriteFldAlign(TDNVAI_04, 0x00, FW_DBG_INK_C);		    
			return;
		}

#if SUPPORT_3D_NAVI_V2
	    if(!bDrvVideoIsSrcInterlace(SV_VP_MAIN))
	    {
	    	if(TD_NAVI_V2_DETECT_LI==g_u1TdNaviV2DetCurr)
	    	{
	    		g_u1TdNaviDetRst = TD_NAVI_DETECT_LI;
	            //Ink Dark Blue
	    		vIO32WriteFldAlign(TDNVAI_04, 0x00, FW_DBG_INK_Y);
	    		vIO32WriteFldAlign(TDNVAI_04, 0xFF, FW_DBG_INK_C);         
	    		return;
	    	}

	        if(TD_NAVI_V2_DETECT_CI==g_u1TdNaviV2DetCurr)
	    	{
	    		g_u1TdNaviDetRst = TD_NAVI_DETECT_CI;
	            //Ink Dark Blue
	    		vIO32WriteFldAlign(TDNVAI_04, 0x00, FW_DBG_INK_Y);
	    		vIO32WriteFldAlign(TDNVAI_04, 0xFF, FW_DBG_INK_C);                 
	    		return;
	    	}

	        if(TD_NAVI_V2_DETECT_CB==g_u1TdNaviV2DetCurr)
	    	{
	    		g_u1TdNaviDetRst = TD_NAVI_DETECT_CB;
	            //Ink Dark Blue
	    		vIO32WriteFldAlign(TDNVAI_04, 0x00, FW_DBG_INK_Y);
	    		vIO32WriteFldAlign(TDNVAI_04, 0xFF, FW_DBG_INK_C);         
	    		return;
	    	}
	    }

		if(TD_NAVI_V2_DETECT_QC==g_u1TdNaviV2DetCurr)
		{
			g_u1TdNaviDetRst = TD_NAVI_DETECT_QC;
	        //Ink Dark Blue
			vIO32WriteFldAlign(TDNVAI_04, 0x00, FW_DBG_INK_Y);
			vIO32WriteFldAlign(TDNVAI_04, 0xFF, FW_DBG_INK_C);          
			return;
		}    
#endif    
	}

	g_u1TdNaviDetRst = TD_NAVI_DETECT_2D;	
	// Ink Dark Green	
    vIO32WriteFldAlign(TDNVAI_04, 0x00, FW_DBG_INK_Y);
    vIO32WriteFldAlign(TDNVAI_04, 0x80, FW_DBG_INK_C);            
}


void vDrvTdNaviAtInputISR(void)
{
#ifdef CC_SUPPORT_SW_NAVI_PLUS  
    if(u1IsInited)
    {
        x_sema_unlock(_hTDNAVIVLoopCheck);
		x_sema_unlock(_hTDNAVIVCheck);        
    }
#endif
}
    
void vDrvTdNaviMainloop(void)
{
    static UINT8 u1State=E_TDNAVI_PROCESSING;

    if(u1IsInited)
    {
#if SUPPORT_REALD_TAG
        vDrvRealDMainloop(SV_VP_MAIN);
        vDrvRealDMainloop(SV_VP_PIP);
#endif

#if SUPPORT_3D_NAVI_V2
        vDrvTdNaviV2Det();
#endif

#ifdef CC_SUPPORT_SW_NAVI_PLUS 		
        if(x_sema_lock(_hTDNAVIVLoopCheck , X_SEMA_OPTION_NOWAIT)!=OSR_OK)
        {
        	goto TDNAVI_MAKE_FINAL_DECESION;
        }
        
        if(E_TDNAVI_WAIT_DUMP==u1State)
        {
            //Dummping Process Start
            x_sema_lock(_hTDNAVIDumpLock, X_SEMA_OPTION_WAIT);            
            x_sema_unlock(_hTDNAVIDumpStart);
            u1State = E_TDNAVI_DUMPING;
            goto TDNAVI_MAKE_FINAL_DECESION;
        }

        if((E_TDNAVI_DUMPING==u1State)
            &&(x_sema_lock(_hTDNAVIDumpLock , X_SEMA_OPTION_NOWAIT)==OSR_OK))
        {
             x_sema_unlock(_hTDNAVIDumpLock);
            //Dummping Process End        
            vDrvTDTVTDNAVIFillBuf();             
            u1State = E_TDNAVI_SRAM_REFRESH;
            goto TDNAVI_MAKE_FINAL_DECESION;
        }

        if(E_TDNAVI_SRAM_REFRESH==u1State)
        {
            u1State = E_TDNAVI_PROCESSING;
            goto TDNAVI_MAKE_FINAL_DECESION;
        }
        
#else
        u1State = E_TDNAVI_PROCESSING;
#endif
                
        if(E_TDNAVI_PROCESSING==u1State)
        {
            vDrvTdNaviDet();
            u1State = E_TDNAVI_WAIT_DUMP;
            goto TDNAVI_MAKE_FINAL_DECESION;            
        }

TDNAVI_MAKE_FINAL_DECESION:
        vDrvTdNaviJudgeRule();                
    }
}
