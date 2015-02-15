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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_adcset_int.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifndef CC_MTK_LOADER


#define _ADCSET_INT_C_

#ifdef CC_UP8032_ATV
#include "general_mt82.h"
#endif
#include "general.h"
#include "hw_vdoin.h"
#include "drv_adcset.h"
#include "hw_tvd.h"
#include "api_eep.h"
#include "eepdef.h"
#include "util.h"
#include "nptv_debug.h"

#include "x_printf.h"
//////////////////////////////////////////////////////////////////////
// DEBUG define
//////////////////////////////////////////////////////////////////////


UINT16  _wTargetBlk;

void vDrvDFE(UINT16 wTARGET_BLANK) 
{
	//UINT16 wBlank_temp; //rock 20060205
	//Set for DFE, Clamp on blank
	
        //0403 in MT5381 this function need to based on MT8202C and some setting are removed???
	//Set output offset as  DFE-Vref
	vIO32WriteFldAlign(DFE_0A, wTARGET_BLANK, CLAMP_TARGET_BLANK_LV);//MT5381TVDPort DFE CLAMP_TARGET_BLANK_LV
	//vWriteVDOINMsk(DFE_CP_01, 0x80|((wTARGET_BLANK>>6)&0x0F), 0xFF, 2 ); // Set DFE target level
       //vWriteVDOINMsk(DFE_CP_01, (wTARGET_BLANK<<2)&0xFC, 0xff, 1 );   //fix hardware bug, must [8:9]=0
	//vWriteVDOINMsk(DFE_CP_01, 0x00, 0xff, 0 ); //target sync high=0, let signal initial DC level higher     

	// Set blank = (256-35)  ,  to support TVD output 16~235   , from ROCK's suggestion 
	//vIO32WriteFldAlign(CTG_07, wTARGET_BLANK-35, BLKLVL);//MT5381TVDPort CTG Port BLKLVL
	//vWriteVDOINMsk(TVD3D_05, (wTARGET_BLANK-35)>>2, 0xff, 2); 	// Set blank level for hold blank   
       //vWriteVIN2BMsk(TVD3D_05, (wTARGET_BLANK-35), 0x03fc, 3, 2); //0403 from MT8202C
	//vWriteVDOINMsk(TVD3D_04, ((wTARGET_BLANK-35) & 0x01), 1, 0); //0403 from MT8202C
    _wTargetBlk = 256;//wTARGET_BLANK;
    //_wBlankLvl = wTARGET_BLANK-35;
	
      //0403 in MT5381 these value below use default and the numbers of table are shrinked???
      /*
	#if 1//(CONFIG_MT8202B)
    vWriteVINMsk(DFE_CP_01, 0x3083E800, 0xFFF003FF );		//fix hardware bug, must [0:7]=0
	vWriteVIN(DFE_06,0x20014011);
	vWriteVIN(DFE_07,0x0FE2C040);
	vWriteVINMsk(DFE_01,0xE98E6155,0x000000ff);
	vWriteVIN(DFE_CPT_00,0x0a81502a);
	vWriteVIN(DFE_CPT_01,0x1502a054);
	vWriteVIN(DFE_CPT_02,0x2a0540a8);
	vWriteVIN(DFE_CPT_03,0x540a8150);
	vWriteVIN(DFE_CPT_04,0xa81502a0);
	vWriteVIN(DFE_CPT_05,0x502a0540);
	vWriteVIN(DFE_CPT_06,0xa0540a81);
	vWriteVIN(DFE_CPT_07,0x40a81502);
	vWriteVIN(DFE_CPT_08,0x81502a05);
	vWriteVIN(DFE_CPT_09,0x02a0540a);
	vWriteVIN(DFE_CPT_0A,0x2280a815);
    vWriteVINMsk(TVD3D_00, 0x82080DDC, 0xffffC00E);     	
	#endif
	
	//Set DFE table done, Enable DFE
	vWriteVINMsk(DFE_CP_00, 0x60604781, 0xfffffffe);	
	vWriteVDOINMsk(DFE_CP_01, 0x00, 0xff, 3 );
	vWriteVDOINMsk(DFE_CP_00, 0x01, 0x01, 0 );	//Enable DFE
        */
}


#ifdef CC_CLI
void vDrvTVDBlankLevel(UINT16 wTARGET_BLANK)
{
  //vWriteVDOINMsk(TVD3D_05, (UINT8)((UINT16)((INT16)wTARGET_BLANK-35)>>2), 0xff, 2); 	// Set blank level for hold blank   
      vIO32WriteFldAlign(CTG_07,wTARGET_BLANK-35,BLKLVL);
}
#endif



/************ BEGIN OF TVD FINE-TUNE******************/
#include "drv_tvd.h"

/****************************************************
  * NonOpen TVD FineTune functions, Next IC will be DrvTvdFT.c
  * Contains  functions  with
  *	1. special trick
  *	2. going to implement as HW
  *	!!  NOT RELEASE TO CUSTOMERS  !!
  ****************************************************/


// divider use 128 instead of 100
extern UINT8 _bTvdISRCnt;
static UINT8 EnvISRCnt;
static UINT8 EnvUpdate;

static UINT16 SwingLvl_IIRx4=128*4;
static UINT8 SwingLvl_UpdateThr=4;
static UINT8 SwingLvl_UpdateCnt=0;
static UINT8 SwingLvl_CurMax=0;
static UINT8 SwingLvl_DecayLVAdj=0;
static UINT8 SwingLvl_DecayLV=70;

static UINT16 HResNRLvl=1;
static UINT16 HResNoiseLvlx4=0;
static UINT32 HResNoiseAccum=0;
static UINT8 HResNoiseQ16=0;
/* SwingLvl_IIRx4  meansure by Fluke54200
 80 < 15
 79 < 60
 75 < 60
 60 < 220
 55 < 500   
*50 < 1600  ----High
 47 < 2500
 *45 < 4500
 42 < 6500
 *40 < 9000 ---SuperHigh
 *37 == 14000
 *35 == 20000
 32 == 32000
 30 == 51000
 27 == 55000
 */
static UINT8 vTvdFT_EnvUpdate(void)
{
	UINT8 SwingLvl_Cur;
	INT16 wSyncHeight;
	INT16 wActiveHeight;
	UINT16  NR_Cur;

	if(EnvISRCnt==_bTvdISRCnt)
		return EnvUpdate;
	EnvISRCnt=_bTvdISRCnt;
	EnvUpdate=0;
	
	wSyncHeight=IO32ReadFldAlign(DFE_STA_01, BLANK_LV)-IO32ReadFldAlign(DFE_STA_01, SYNC_LV);
	wActiveHeight=IO32ReadFldAlign(DFE_STA_03, PY_LV);
	wSyncHeight=(wSyncHeight<0)?0:wSyncHeight;
	wSyncHeight=(wSyncHeight*128)/(256-32); //assume target sync height 256-32
	wActiveHeight=(128*4*wActiveHeight)/IO32ReadFldAlign(DFE_04, AGC2_PY_LIM);

	SwingLvl_Cur=(wSyncHeight>wActiveHeight)?wSyncHeight:wActiveHeight;
	if(SwingLvl_Cur>SwingLvl_CurMax) SwingLvl_CurMax=SwingLvl_Cur;

	SwingLvl_UpdateCnt++;

	/* HResNR reference swing counter */
	NR_Cur=IO32ReadFldAlign(STA_CTG_05,VAR_CVBS_CLIP);
	if(NR_Cur==0xfff)
		NR_Cur=5120; //0x1000*1.25

    	HResNoiseQ16=(SwingLvl_IIRx4+15)/(8*4);
	if(HResNoiseQ16<=2) HResNoiseQ16=2;
	if(HResNoiseQ16<15)
		NR_Cur=(NR_Cur*16)/HResNoiseQ16;
	HResNoiseAccum+=NR_Cur;

	if(SwingLvl_UpdateCnt>SwingLvl_UpdateThr)
	{		
		SwingLvl_IIRx4=(SwingLvl_IIRx4*3)/4+SwingLvl_CurMax;
		if(SwingLvl_IIRx4>128*4)
			SwingLvl_IIRx4=128*4;

		SwingLvl_UpdateCnt=0;
		SwingLvl_CurMax=0;
		EnvUpdate=1;

		/* HResNR reference swing counter */
		HResNoiseLvlx4=(HResNoiseLvlx4*3)/4+HResNoiseAccum;
		HResNoiseAccum=0;
		
		if(HResNoiseLvlx4>20000)
			HResNRLvl=6;
		else if(HResNoiseLvlx4>14000)
			HResNRLvl=5;
		else if(HResNoiseLvlx4>9000)
			HResNRLvl=4;
		else if(HResNoiseLvlx4>4500)
			HResNRLvl=3;
		else if(HResNoiseLvlx4>1600)
			HResNRLvl=2;
		else	HResNRLvl=1;

	}
	return EnvUpdate;
}

static void vTvdFT_EnvReset(void)
{
	SwingLvl_IIRx4=128*4;
	SwingLvl_UpdateCnt=0;
	SwingLvl_CurMax=0;
}

static	struct RCAGC_NoiseMap
{
	UINT8 HNRLvlMap[5];
} rRCAGC_NoiseMap[]=
{
	 {{0,1,2,3,4}}, /*0*/
        {{0,1,2,3,4}}, /*1*/
        {{0,1,2,3,4}}, /*2*/
        {{1,2,3,4,4}}, /*3*/
        {{1,2,3,4,5}}, /*4*/
        {{1,2,3,4,5}}, /*5*/
        {{1,2,3,4,5}}, /*6*/
        {{1,3,4,5,6}}, /*7*/
        {{2,3,4,5,6}}, /*8*/
        {{2,3,4,5,6}}, /*9*/
        {{2,4,5,5,6}}, /*10*/
        {{2,4,5,6,7}}, /*11*/
        {{3,4,5,6,7}}, /*12*/
        {{3,4,5,6,7}}, /*13*/
};


static UINT16 CAGC_Prev=0;
static UINT8 CAGC_Trend=0;
static UINT8 CAGC_TrendCnt=0;
static UINT8 CAGC_DecayQ16=0;

extern UINT8 _bAgcPedEn;

void vTvdFT_Chroma_SAT(UINT8 bCmd, UINT32 dArg)
{
	UINT16  CAGC_Cur;

	if(bCmd==FTCMD_RESET)
	{
		vTvdFT_EnvReset();
		CAGC_Prev=0;
		CAGC_Trend=0;
		CAGC_TrendCnt=0;
		return;
	}

	if(IO32ReadFldAlign(CDET_00,CAGC_ADAP))
	{	//CAGC_L_STEP/CAGC_S_STEP not reset by mode done
		if(IO32ReadFldAlign(CDET_08,CAGC_S_STEP)!=1)
			vIO32WriteFldMulti(CDET_08,P_Fld(1,CAGC_S_STEP)|P_Fld(5,CAGC_L_STEP));
		return;
	}

	CAGC_Cur=IO32ReadFldAlign(STA_CDET_01,CAGC_STA);
	if(CAGC_Cur>CAGC_Prev)
	{
		if(CAGC_Trend)
		{
			CAGC_TrendCnt=(CAGC_TrendCnt==0xff)?0xff:(CAGC_TrendCnt+1);
		}
		else
		{
			CAGC_Trend=1;
			CAGC_TrendCnt=0;
		}
	}
	else if(CAGC_Cur<CAGC_Prev)
	{
		if(CAGC_Trend)
		{
			CAGC_Trend=0;
			CAGC_TrendCnt=0;
		}
		else
		{
			CAGC_TrendCnt=(CAGC_TrendCnt==0xff)?0xff:(CAGC_TrendCnt+1);
		}
	}

	if((CAGC_Cur>0x100)&&(CAGC_TrendCnt>3))
	{
		vIO32WriteFldMulti(CDET_08,P_Fld(3,CAGC_S_STEP)|P_Fld(7,CAGC_L_STEP));
	}
	else if ((CAGC_Cur>0xA0)&&(CAGC_TrendCnt>3))
	{
		vIO32WriteFldMulti(CDET_08,P_Fld(2,CAGC_S_STEP)|P_Fld(7,CAGC_L_STEP));
	}
	else
	{
		vIO32WriteFldMulti(CDET_08,P_Fld(1,CAGC_S_STEP)|P_Fld(5,CAGC_L_STEP));
	}
	CAGC_Prev=CAGC_Cur;	

	if(vTvdFT_EnvUpdate())
	{
		UINT8 mid_a,mid_b,target_a,target_b;
		UINT8 decay_lvl,mid_wid;
		//NTSC_CAGC_BOUND	0x4b765968  -/14/14/14/-
		//NTSC_J_CAGC_BOUND	0x547e6270  -/14/14/14/-
		//PAL_CAGC_BOUND	0x53876575  -/18/15/18/-	
		//SECAM_CAGC_BOUND	0x3040383a  -/8/2/6/-
		//SECAM_HN_CGAC_BOUND	0x1050383a  -/40/2/22/-

		CAGC_DecayQ16=(128*4-SwingLvl_IIRx4)/(8*4);
		if(CAGC_DecayQ16<=2) CAGC_DecayQ16=0;
		if(CAGC_DecayQ16>=13) CAGC_DecayQ16=13;

		if(!SwingLvl_DecayLVAdj)
		{
			switch (_rTvd3dStatus.bTvdMode)
			{
			case SV_CS_SECAM:
				SwingLvl_DecayLV=0;
				break;
			default:	
				SwingLvl_DecayLV=70;
				break;
			}
		}

		if((HResNRLvl>1)&&(dArg&1))
			decay_lvl=(SwingLvl_DecayLV*(CAGC_DecayQ16+rRCAGC_NoiseMap[CAGC_DecayQ16].HNRLvlMap[HResNRLvl-2]))/16;
		else
			decay_lvl=(SwingLvl_DecayLV*CAGC_DecayQ16)/16;

		switch (_rTvd3dStatus.bTvdMode)
		{
		case SV_CS_SECAM:
			target_b=0x38-decay_lvl;
			target_a=0x3a-decay_lvl;
			mid_wid=7;
			break;
		case SV_CS_PAL:
		case SV_CS_PAL_60:
		case SV_CS_PAL_N:		
			target_b=0x65-decay_lvl;
			target_a=0x75-decay_lvl;
			mid_wid=18;
			break;
		default:
		case SV_CS_PAL_M:
		case SV_CS_NTSC358:
		case SV_CS_NTSC443:
			if(_bAgcPedEn)
			{
				target_b=0x59-decay_lvl;
				target_a=0x68-decay_lvl;
			}
			else
			{
				target_b=0x62-decay_lvl;
				target_a=0x70-decay_lvl;
			}
			mid_wid=14;
			break;
		}

		if(CAGC_Cur>0x100)
		{
			switch (_rTvd3dStatus.bTvdMode)
			{
			case SV_CS_SECAM:
				decay_lvl=mid_wid+(HResNRLvl-1)*3;
				break;
			default:	
				decay_lvl=mid_wid-CAGC_DecayQ16/2; //min 8
				break;
			}
		}
		else	
			decay_lvl=mid_wid;
		mid_a=((target_a+decay_lvl)<0xff)?target_a+decay_lvl:0xff;
		mid_b=(target_b>decay_lvl)?target_b-decay_lvl:0;
		vIO32WriteFldMulti(CDET_07, P_Fld(mid_b,MID_B)|P_Fld(mid_a,MID_A)|
			P_Fld(target_b,TARGET_B)|P_Fld(target_a,TARGET_A));
	}
}

#ifndef CC_UP8032_ATV
void (*rTvdFT_FnTbl[])(UINT8 bCmd, UINT32 dArg)=
{
	0,  /* TVDFT_ALL */
	vTvdFT_Chroma_SAT,
};
#endif
#define FT_TBL_SIZE   (sizeof(rTvdFT_FnTbl)/sizeof(void (*)(UINT8,UINT32)))


void vTvdFineTune(UINT8  bItem, UINT8 bCmd, UINT32 dArg)
{
#ifndef CC_UP8032_ATV
	int i;
	if(bItem==TVDFT_ALL)
	{
		for(i=1;i<FT_TBL_SIZE;i++)
		{
			rTvdFT_FnTbl[i](bCmd,dArg);
		}
			
	}	
	else if(bItem<FT_TBL_SIZE)
	{
		rTvdFT_FnTbl[bItem](bCmd,dArg);
	}	      
#endif
}
/************** END OF TVD FINE-TUNE******************/
#if TVD_NR_AVG
UINT32 wNoiselevel_new;
UINT8 bNRcnt,RF_level;
UINT8 bNRcnt_1;
UINT32 wNoiselevel_max,wNoiselevel_min;
UINT32 _bGetNRTh = 100;
UINT32 _bGetNRCnt = 100;
UINT32 wNoiselevel_new2=0;
static const UINT32 TVD_NRlevel_tbl[] =
	{75, 86, 109, 182, 394, 663, 1162, 1994, 3263, 5374, 9530, 17064, 28304, 42895, 56133, 72147, 85691 };
static const UINT32 TVD_RFlevel_tbl[] =
	{80, 75, 70, 65, 60, 57, 54, 51, 48, 45, 42, 39, 36, 33, 30, 27, 24 };

void vSetNRTable(UINT32 u1dB, UINT32 u4Val, UINT32 u4Count)
{
    int i = 0;

    _bGetNRTh = u4Count;
    for (i=0;i<17;i++)
    {
        if(TVD_RFlevel_tbl[i]==u1dB)
        {
            printf("Set NR table %ddB = %d\n", i, u4Val);
            return;
        }
    }
    
}

void vTvdMeasureNR(BOOL  fgEnable )
{
    UINT8 i;

if((_rTvd3dStatus.fgVPres == 1) && fgEnable)
{
	if (bNRcnt < 32)
	{
		bNRcnt++;
//		wNoiselevel=wNoiselevel+IO32ReadFldAlign(STA_CDET_00, NR_LEVEL);
		//TODO
		wNoiselevel_new=wNoiselevel_new+IO32ReadFldAlign(STA_CTG_05, VAR_CVBS_CLIP);
	}
	else
	{
                if(_bGetNRCnt<_bGetNRTh)
                {
                    _bGetNRCnt++;
                    wNoiselevel_new2 = wNoiselevel_new2+wNoiselevel_new;
                }
                else if(_bGetNRCnt==_bGetNRTh)
                {
                    _bGetNRCnt++;
                    printf("NR_Level_new = %d\n", wNoiselevel_new2/_bGetNRTh);
                    wNoiselevel_new2 = 0;
                }
//		wNoiselevel=wNoiselevel;
		wNoiselevel_new=wNoiselevel_new;
//printf("NR_Level = %d\n", wNoiselevel);
//printf("NR_Level_new = %d\n", wNoiselevel_new);
//		wNoiselevel=0;
		bNRcnt=0;
for(i=0;i<17;i++)//sizeof(TVD_NRlevel_tbl);i++)
{
	if(wNoiselevel_new < TVD_NRlevel_tbl[i])
	{
		if(i == 0)
		{
			RF_level=TVD_RFlevel_tbl[i];
			break;
		}
		else if(i < 5)
		{
			RF_level=TVD_RFlevel_tbl[i-1]-
				(((wNoiselevel_new-TVD_NRlevel_tbl[i-1])*5)/(TVD_NRlevel_tbl[i]-TVD_NRlevel_tbl[i-1]));
			break;
		}
		else if(i < 12)
		{
			RF_level=TVD_RFlevel_tbl[i-1]-
				(((wNoiselevel_new-TVD_NRlevel_tbl[i-1])*3)/(TVD_NRlevel_tbl[i]-TVD_NRlevel_tbl[i-1]));
			break;
		}
		else
		{
			RF_level=TVD_RFlevel_tbl[i-1]-
				((((wNoiselevel_new>>4)-(TVD_NRlevel_tbl[i-1]>>4))*3)
				/((TVD_NRlevel_tbl[i]-TVD_NRlevel_tbl[i-1])>>4));
			break;
		}
	}
	else
	{
		RF_level=TVD_RFlevel_tbl[i];
	}
}
//	LOG(3, "RF_Level_in_dB = %d\n", RF_level);
//printf("RF_Level_in_dB = %d\n", RF_level);
		wNoiselevel_new=0;
	}
}
else
{
	bNRcnt=0;
	wNoiselevel_new=0;
	RF_level=0;
}

}
#endif

#endif


