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
 
#include "vdo_misc.h"
#include "sv_const.h"
#include "drv_tdtv_feature.h"
#include "drv_tdnavi.h"
#include "hw_tdnavi.h"
#if SUPPORT_3D_NAVI_V2
#if 0

#include "x_assert.h"


#include "vdp_if.h"
#endif

/*----------------------------------------------------------------------*
*NaviV2:LI,CB, DA, QC default paramter
*----------------------------------------------------------------------*/

UINT8 g_u1TdLb_Thr		= 10;

UINT32 g_u4HeightSt		= 0;
UINT32 g_u4WidthSt		= 0;
UINT8  g_u1HMidBlk		= 2;
UINT8  g_u1WMidBlk		= 2;

UINT8 g_u1CbCoriThr		= 4;
UINT8 g_u1CbDiffGapThr	= 2;
UINT8 g_u1CbCntthr		= 2;
UINT8 g_u1CbStaMode		= 0;

UINT8 g_u1CiDiffGapthr	= 2;
UINT8 g_u1CiHorThr		= 4;
UINT8 g_u1CiCntThr		= 3;	
UINT8 g_u1CiStaMode		= 0;

UINT8 g_u1LiVerThr1		= 6;
UINT8 g_u1LiVerThr2		= 6;
UINT8 g_u1LiCntThr		= 4;
UINT8 g_u1LiStaMode		= 0;

UINT8 g_u1QcDiffGapThr	= 2;
UINT8 g_u1QcVdiffGainThr= 2;
UINT8 g_u1QcVdiffMinThr = 10;
UINT8 g_u1QcCntThr		= 4;
UINT8 g_u1QcStaMode		= 0;
UINT8 g_u1QcMode		= 0;

UINT8 g_u1CbFrcHi		= 22;
UINT8 g_u1CiFrcHi		= 18;
UINT8 g_u1LiFrcHi		= 22;
UINT8 g_u1QcFrcHi		= 19;
UINT8 g_u1CbFrcLo		= 13;
UINT8 g_u1CiFrcLo		= 10;
UINT8 g_u1LiFrcLo		= 13;
UINT8 g_u1QcFrcLo		= 13;

UINT8 g_u1TdNaviV2DetCurr  = TD_NAVI_V2_DETECT_2D;
UINT8 g_u1TdNaviV2DetNew   = TD_NAVI_V2_DETECT_2D;
UINT8 g_u1TdNaviV2Det2DCnt = 0;
UINT8 g_u1TdNaviV2DetLiCnt = 0;
UINT8 g_u1TdNaviV2DetCbCnt = 0;
UINT8 g_u1TdNaviV2DetCiCnt = 0;
UINT8 g_u1TdNaviV2DetQcCnt = 0;
UINT8 g_u1TdNaviV2DetUnknowCnt = 0;

UINT8 g_u1TdNaviV2DetInFrmNum 	= 15;
UINT8 g_u1TdNaviV2DetOutFrmNum 	= 5;



/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/


static void vDrvTdNaviV2PrmSet(void)
{
	vIO32WriteFldAlign(TDNVAI_08, g_u1TdLb_Thr, TD_LB_THR);	
	vIO32WriteFldAlign(TDNVAI_08, g_u1LiVerThr1, LI_VER_THR1);
	vIO32WriteFldAlign(TDNVAI_08, g_u1LiVerThr2, LI_VER_THR2);
	
	vIO32WriteFldAlign(TDNVAI_09, g_u1CiHorThr, CI_HOR_THR);
	vIO32WriteFldAlign(TDNVAI_09, g_u1CiDiffGapthr, CI_DIFF_GAP_THR);		
	vIO32WriteFldAlign(TDNVAI_09, g_u1LiCntThr, LI_CNT_THR);
	vIO32WriteFldAlign(TDNVAI_09, g_u1CiCntThr, CI_CNT_THR);
	vIO32WriteFldAlign(TDNVAI_09, g_u1LiStaMode, LI_STA_MODE);
	vIO32WriteFldAlign(TDNVAI_09, g_u1CiStaMode, CI_STA_MODE);
		
	vIO32WriteFldAlign(TDNVAI_0A, g_u1LiFrcHi, LI_FCR_HI);
	vIO32WriteFldAlign(TDNVAI_0A, g_u1LiFrcLo, LI_FCR_LO);
	vIO32WriteFldAlign(TDNVAI_0A, g_u1CiFrcHi, CI_FCR_HI);
	vIO32WriteFldAlign(TDNVAI_0A, g_u1CiFrcLo, CI_FCR_LO);

	
	vIO32WriteFldAlign(TDNVAI_0C, g_u1CbCntthr, CB_CNT_THR);
	vIO32WriteFldAlign(TDNVAI_0C, g_u1QcCntThr, QC_CNT_THR);	
	vIO32WriteFldAlign(TDNVAI_0C, g_u1CbCoriThr, CB_CORI_THR);
	vIO32WriteFldAlign(TDNVAI_0C, g_u1CbDiffGapThr, CB_DIFF_GAP_THR);	
	vIO32WriteFldAlign(TDNVAI_0C, g_u1QcVdiffMinThr, QC_VDIFF_MIN_THR);
	vIO32WriteFldAlign(TDNVAI_0C, g_u1QcVdiffGainThr, QC_VDIFF_GAIN_THR);
	vIO32WriteFldAlign(TDNVAI_0C, g_u1QcDiffGapThr, QC_DIFF_GAP_THR);
	vIO32WriteFldAlign(TDNVAI_0C, g_u1QcMode, QC_MODE);

	
	vIO32WriteFldAlign(TDNVAI_0D, g_u1CbFrcHi, CB_FCR_HI);
	vIO32WriteFldAlign(TDNVAI_0D, g_u1CbFrcLo, CB_FCR_LO);	
	vIO32WriteFldAlign(TDNVAI_0D, g_u1QcFrcHi, QC_FCR_HI);
	vIO32WriteFldAlign(TDNVAI_0D, g_u1QcFrcLo, QC_FCR_LO);	
	vIO32WriteFldAlign(TDNVAI_0D, g_u1CbStaMode, CB_STA_MODE);
	vIO32WriteFldAlign(TDNVAI_0D, g_u1QcStaMode, QC_STA_MODE);	
}

static void vDrvTdNaviV2SetPicSize(void)
{
	UINT32 u2InputWidth, u2InputHeight;
    UINT32  u1BlkWidth, u1BlkHeight, u1IsIntr;
    
    u1IsIntr = bDrvVideoIsSrcInterlace(SV_VP_MAIN);	
    u2InputWidth  = wDrvVideoInputWidth(SV_VP_MAIN);
    u2InputHeight = wDrvVideoInputHeight(SV_VP_MAIN);
    
	u2InputHeight = u1IsIntr ? (u2InputHeight >> 1) : u2InputHeight;
	
	u1BlkWidth = (u2InputWidth - 2*g_u4HeightSt ) / 3;
	u1BlkHeight = (u2InputHeight - 2*g_u4WidthSt) / 3;
	//need to set u1BlkWidth >= 140
	if(u1BlkWidth < 140)
		u1BlkWidth = 140;		
	
	vIO32WriteFldAlign(TDNVAI_07, g_u1HMidBlk, H_MID_BLK);
	vIO32WriteFldAlign(TDNVAI_07, g_u1WMidBlk, W_MID_BLK);		
	vIO32WriteFldAlign(TDNVAI_07, g_u4HeightSt, HEIGHT_ST);		
	vIO32WriteFldAlign(TDNVAI_07, g_u4WidthSt, WIDTH_ST);
	
	vIO32WriteFldAlign(TDNVAI_08, u1BlkHeight, WIN_HEIGHT);		
	vIO32WriteFldAlign(TDNVAI_08, u1BlkWidth, WIN_WIDTH);

}

static UINT8 u1DrvTdNaviV2GetDetType(void)
{
	return IO32ReadFldAlign(TDNVAI_STA5, STA_FMT);
}

void vDrvTdNaviV2Det(void)
{
	//(0--Li,1--Ci, 2--Cb, 3-Qc, 4-2D,5-Unknow)
	g_u1TdNaviV2DetNew = u1DrvTdNaviV2GetDetType();
	LOG(7,"navi2 result g_u1TdNaviV2DetNew(%d)\n",g_u1TdNaviV2DetNew);

	//Check Board
    if (TD_NAVI_V2_DETECT_CB == g_u1TdNaviV2DetCurr
	#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
	&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_CB)
	#endif
	)
	{	
		if (TD_NAVI_V2_DETECT_UNKNOWN != g_u1TdNaviV2DetNew && TD_NAVI_V2_DETECT_CB != g_u1TdNaviV2DetNew )// LI, CI QC 
		//if ( TD_NAVI_V2_DETECT_CB != g_u1TdNaviV2DetNew )// LI, CI QC ,Unknow
		{
			g_u1TdNaviV2Det2DCnt++;

			if(g_u1TdNaviV2Det2DCnt > g_u1TdNaviV2DetOutFrmNum)
			{
				g_u1TdNaviV2DetCurr = TD_NAVI_V2_DETECT_2D;
				g_u1TdNaviV2Det2DCnt = 0;
			}
		}
		else //Unknow ,CB
 		{
			if (g_u1TdNaviV2Det2DCnt > 0)
			{
				g_u1TdNaviV2Det2DCnt--;
			}
		}
	}
	//Line Interleave
	else if(TD_NAVI_V2_DETECT_LI == g_u1TdNaviV2DetCurr
	#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_LI)
	#endif
	)
	{
		if ((TD_NAVI_V2_DETECT_UNKNOWN != g_u1TdNaviV2DetNew) && (TD_NAVI_V2_DETECT_LI != g_u1TdNaviV2DetNew))// CB,CI,QC 
		//if ((TD_NAVI_V2_DETECT_LI != g_u1TdNaviV2DetNew)) // CB,CI,QC ,Unknow
		{
			g_u1TdNaviV2Det2DCnt++;

			if(g_u1TdNaviV2Det2DCnt > g_u1TdNaviV2DetOutFrmNum)
			{
				g_u1TdNaviV2DetCurr = TD_NAVI_V2_DETECT_2D;
				g_u1TdNaviV2Det2DCnt = 0;
			}
		}
		else//LI,Unknow
		{
			if (g_u1TdNaviV2Det2DCnt > 0)
			{
				g_u1TdNaviV2Det2DCnt--;
			}
		}
	}
	//Column Interleave
	else if(TD_NAVI_V2_DETECT_CI == g_u1TdNaviV2DetCurr
	#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_CI)
	#endif
	)
	{
		if ((TD_NAVI_V2_DETECT_UNKNOWN != g_u1TdNaviV2DetNew) && (TD_NAVI_V2_DETECT_CI != g_u1TdNaviV2DetNew))// LI,CB,QC
		//if ( (TD_NAVI_V2_DETECT_CI != g_u1TdNaviV2DetNew))// LI,CB,QC,Unknow
		{
			g_u1TdNaviV2Det2DCnt++;

			if(g_u1TdNaviV2Det2DCnt > g_u1TdNaviV2DetOutFrmNum)
			{
				g_u1TdNaviV2DetCurr = TD_NAVI_V2_DETECT_2D;
				g_u1TdNaviV2Det2DCnt = 0;
			}
		}
		else //CI,Unknow
		{
			if (g_u1TdNaviV2Det2DCnt > 0)
			{
				g_u1TdNaviV2Det2DCnt--;
			}
		}
	}
	//Quincunx
	else if(TD_NAVI_V2_DETECT_QC == g_u1TdNaviV2DetCurr
	#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
	&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_QC)
	#endif
	)
	{
		if ((TD_NAVI_V2_DETECT_UNKNOWN != g_u1TdNaviV2DetNew) && (TD_NAVI_V2_DETECT_QC != g_u1TdNaviV2DetNew))// LI,CB,CI
		//if ((TD_NAVI_V2_DETECT_QC != g_u1TdNaviV2DetNew))// LI,CB,CI,Unknow
		{
			g_u1TdNaviV2Det2DCnt++;

			if(g_u1TdNaviV2Det2DCnt > g_u1TdNaviV2DetOutFrmNum)
			{
				g_u1TdNaviV2DetCurr = TD_NAVI_V2_DETECT_2D;
				g_u1TdNaviV2Det2DCnt = 0;
			}
		}
		else //QC,Unknow
		{
			if (g_u1TdNaviV2Det2DCnt > 0)
			{
				g_u1TdNaviV2Det2DCnt--;
			}
		}
	}
	// 2D,Unknow
    else 
	{	
		if (TD_NAVI_V2_DETECT_CB == g_u1TdNaviV2DetNew
		#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_CB)
		#endif
		
		{
			g_u1TdNaviV2DetCbCnt++;

			if (g_u1TdNaviV2DetLiCnt > 0)
				g_u1TdNaviV2DetLiCnt--;
			if (g_u1TdNaviV2DetCiCnt > 0)
				g_u1TdNaviV2DetCiCnt--;
			if (g_u1TdNaviV2DetQcCnt > 0)
				g_u1TdNaviV2DetQcCnt--;
			if(g_u1TdNaviV2DetCbCnt > g_u1TdNaviV2DetInFrmNum)
			{
				g_u1TdNaviV2DetCurr 	= TD_NAVI_V2_DETECT_CB;
				g_u1TdNaviV2DetCbCnt 	= 0;
		 		g_u1TdNaviV2DetLiCnt 	= 0;
		 		g_u1TdNaviV2DetCiCnt 	= 0;
		 		g_u1TdNaviV2DetQcCnt 	= 0;
				
			}
		}
		else if(TD_NAVI_V2_DETECT_LI == g_u1TdNaviV2DetNew
		#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_LI)
		#endif
		)
		{
			g_u1TdNaviV2DetLiCnt++;
	
			if (g_u1TdNaviV2DetCbCnt > 0)
				g_u1TdNaviV2DetCbCnt--;
			if (g_u1TdNaviV2DetCiCnt > 0)
				g_u1TdNaviV2DetCiCnt--;
			if (g_u1TdNaviV2DetQcCnt > 0)
				g_u1TdNaviV2DetQcCnt--;
	
			if(g_u1TdNaviV2DetLiCnt > g_u1TdNaviV2DetInFrmNum)
			{
				g_u1TdNaviV2DetCurr 	= TD_NAVI_V2_DETECT_LI;
				g_u1TdNaviV2DetCbCnt 	= 0;
				g_u1TdNaviV2DetLiCnt 	= 0;
				g_u1TdNaviV2DetCiCnt 	= 0;
				g_u1TdNaviV2DetQcCnt 	= 0;
			}
		}
		else if(TD_NAVI_V2_DETECT_CI == g_u1TdNaviV2DetNew
		#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_CI)
		#endif
		)
		{
			g_u1TdNaviV2DetCiCnt++;
	
			if (g_u1TdNaviV2DetCbCnt > 0)
				g_u1TdNaviV2DetCbCnt--;
			if (g_u1TdNaviV2DetLiCnt > 0)
				g_u1TdNaviV2DetLiCnt--;
			if (g_u1TdNaviV2DetQcCnt > 0)
				g_u1TdNaviV2DetQcCnt--;
	
			if(g_u1TdNaviV2DetCiCnt > g_u1TdNaviV2DetInFrmNum)
			{
				g_u1TdNaviV2DetCurr 	= TD_NAVI_V2_DETECT_CI;
				g_u1TdNaviV2DetCbCnt 	= 0;
				g_u1TdNaviV2DetLiCnt 	= 0;
				g_u1TdNaviV2DetCiCnt 	= 0;
				g_u1TdNaviV2DetQcCnt 	= 0;
			}
		}
		else if(TD_NAVI_V2_DETECT_QC == g_u1TdNaviV2DetNew
		#if defined (CC_SUPPORT_NAVI_PATTERN_MODE)
		&&(eUINaviPatternDetect &TD_NAVI_V2_DETECT_QC)
		#endif	

		)
		{
			g_u1TdNaviV2DetQcCnt++;
	
			if (g_u1TdNaviV2DetCbCnt > 0)
				g_u1TdNaviV2DetCbCnt--;
			if (g_u1TdNaviV2DetCiCnt > 0)
				g_u1TdNaviV2DetCiCnt--;
			if (g_u1TdNaviV2DetLiCnt > 0)
				g_u1TdNaviV2DetLiCnt--;
	
			if(g_u1TdNaviV2DetQcCnt > g_u1TdNaviV2DetInFrmNum)
			{
				g_u1TdNaviV2DetCurr 	= TD_NAVI_V2_DETECT_QC;
				g_u1TdNaviV2DetCbCnt 	= 0;
				g_u1TdNaviV2DetLiCnt 	= 0;
				g_u1TdNaviV2DetCiCnt 	= 0;
				g_u1TdNaviV2DetQcCnt 	= 0;
			}
		}
		else // 2D ,Unknow
		{
			if (g_u1TdNaviV2DetCbCnt > 0)
				g_u1TdNaviV2DetCbCnt--;
			if (g_u1TdNaviV2DetCiCnt > 0)
				g_u1TdNaviV2DetCiCnt--;
			if (g_u1TdNaviV2DetLiCnt > 0)
				g_u1TdNaviV2DetLiCnt--;
			if (g_u1TdNaviV2DetQcCnt > 0)
				g_u1TdNaviV2DetQcCnt--;
		}
	}
	if(IO32ReadFldAlign(TDNAVI_FW_00, TDNAVI_V2_LOG))
	{
		LOG(0,"CbCnt(%d),CiCnt(%d),LiCnt(%d),QcCnt(%d),2DCnt(%d),DetV2New(%d),DetV2Curr(%d)\n"
		,g_u1TdNaviV2DetCbCnt,g_u1TdNaviV2DetCiCnt,g_u1TdNaviV2DetLiCnt,g_u1TdNaviV2DetQcCnt,
		g_u1TdNaviV2Det2DCnt,g_u1TdNaviV2DetNew,g_u1TdNaviV2DetCurr);
	}
}


void vDrvTdNaviV2ModeChange(void)
{
	g_u1TdNaviV2DetCurr	 = TD_NAVI_V2_DETECT_2D;
	g_u1TdNaviV2DetNew   = TD_NAVI_V2_DETECT_2D;
	g_u1TdNaviV2Det2DCnt = 0;
	g_u1TdNaviV2DetLiCnt = 0;
	g_u1TdNaviV2DetCbCnt = 0;
	g_u1TdNaviV2DetCiCnt = 0;
	g_u1TdNaviV2DetQcCnt = 0;
	vDrvTdNaviV2PrmSet();
	vDrvTdNaviV2SetPicSize();
}

void vDrvTdNaviV2Init(void)
{
	vDrvTdNaviV2PrmSet();
	vDrvTdNaviV2ModeChange();
}


#endif
