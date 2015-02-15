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
 * $RCSfile: drv_lcdim.c,v $
 *
 *---------------------------------------------------------------------------*/

/*
    1. design new local dimming panel table, add phase information, add light direction
    2. add SUPPORT_10BIT_DIMMING, to keep consistence between V1 / V2
    3. phase information : (1) phase sampling points (2) phase x/y (3) phase ratio
    4. check if temporal filter reset would reset itself (no need to toggle)
*/

// === INCLUDE =========================================================================
#include "general.h"
#include "vdo_misc.h"
#include "drv_lcdim_v2.h"
#include "hw_lcdim_v2.h"
#include "panel.h"
#include "drv_meter.h"
#include "nptv_debug.h"
#include "hw_sw.h"
#include "hw_ospe.h"
#include "drv_ldmspi.h"
#include "drv_di.h"

// === DEFINE ===========================================================================

#define LIGHTPROFILE(phase, sampleidx) u1LightProfile[(sampleidx)*LDM_LP_TOTAL_PHASE_NUM + (phase)]

// === GLOBAL VARIABLE ====================================================================
UINT32 _u4BlockPxlH, _u4BlockPxlV;
UINT32 _u4BlockPxlHLast, _u4BlockPxlVLast;

#ifndef NDEBUG    // PQ SRAM Stress Test Only Take Effect in Debug Build
UINT8 bLcDimLightProfileReadBack[LCDIM_LIGHTPROF_NUM];
#endif

// === EXTERN FUNCTION ====================================================================
EXTERN UINT8 bSupportLocalDimming;
EXTERN UINT8 u1LCDIMOnOffStatus;
EXTERN UINT16 u2BlockH; 
EXTERN UINT16 u2BlockV;
EXTERN UINT32 u4CurrentDemoBlock;
EXTERN UINT32 u4DemoFrameCount;

EXTERN UINT16 u2LcdimDimTable_OFF[LCDIM_DIMMING_NUM];

EXTERN UINT32 IS_SupportDimming (VOID);


enum{
    LCDIM_PANEL_TYPE_DIRECT=0,
    LCDIM_PANEL_TYPE_TOP_BOTTOM=1,
    LCDIM_PANEL_TYPE_LEFT_RIGHT=2,
};

enum{
    PANEL_LIGHT_DIR_DIRECT=0,
    PANEL_LIGHT_DIR_LR=1,
    PANEL_LIGHT_DIR_TB=2,
    PANEL_LIGHT_DIR_L=3,
    PANEL_LIGHT_DIR_R=4,    
    PANEL_LIGHT_DIR_T=5,
    PANEL_LIGHT_DIR_B=6,        
};

enum{
    LCDIM_BLOCK_INFO_MAX=0,
    LCDIM_BLOCK_INFO_OUT_DIM=1,
    LCDIM_BLOCK_INFO_TAR_DIM=2,
    LCDIM_BLOCK_INFO_CONF=3, 
};

void vDrvLcdimWriteLightProfile(UINT8 *u1LightProfile)
{	   
	UINT8 u1SampleIdx = 0;
	// write into sram
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_ACTIVE);
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_SEL_0);

	vIO32WriteFldAlign(LD2_SRAM_IF_00, 1, C_LD_WR_AUTO_INC_ADDR);
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0, C_LD_RD_AUTO_INC_ADDR);

	  // set initial address
	vIO32WriteFldAlign(LD2_SRAM_IF_01, 0, C_LD_ADDR);

	for ( u1SampleIdx = 0; u1SampleIdx < 128 ; u1SampleIdx++ )
	{
		vIO32WriteFldAlign(LD2_SRAM_IF_07, LIGHTPROFILE(20, u1SampleIdx), C_LD_WDATA_5);
		vIO32Write4B(LD2_SRAM_IF_06, ((UINT32)LIGHTPROFILE(19, u1SampleIdx) << 24 ) | ((UINT32)LIGHTPROFILE(18, u1SampleIdx)<< 16 ) | ((UINT32)LIGHTPROFILE(17, u1SampleIdx)<< 8 ) | ((UINT32)LIGHTPROFILE(16, u1SampleIdx)));
		vIO32Write4B(LD2_SRAM_IF_05, ((UINT32)LIGHTPROFILE(15, u1SampleIdx) << 24 ) | ((UINT32)LIGHTPROFILE(14, u1SampleIdx)<< 16 ) | ((UINT32)LIGHTPROFILE(13, u1SampleIdx)<< 8 ) | ((UINT32)LIGHTPROFILE(12, u1SampleIdx)));		
		vIO32Write4B(LD2_SRAM_IF_04, ((UINT32)LIGHTPROFILE(11, u1SampleIdx) << 24 ) | ((UINT32)LIGHTPROFILE(10, u1SampleIdx)<< 16 ) | ((UINT32)LIGHTPROFILE( 9, u1SampleIdx)<< 8 ) | ((UINT32)LIGHTPROFILE( 8, u1SampleIdx)));
		vIO32Write4B(LD2_SRAM_IF_03, ((UINT32)LIGHTPROFILE( 7, u1SampleIdx) << 24 ) | ((UINT32)LIGHTPROFILE( 6, u1SampleIdx)<< 16 ) | ((UINT32)LIGHTPROFILE( 5, u1SampleIdx)<< 8 ) | ((UINT32)LIGHTPROFILE( 4, u1SampleIdx)));
		vIO32Write4B(LD2_SRAM_IF_02, ((UINT32)LIGHTPROFILE( 3, u1SampleIdx) << 24 ) | ((UINT32)LIGHTPROFILE( 2, u1SampleIdx)<< 16 ) | ((UINT32)LIGHTPROFILE( 1, u1SampleIdx)<< 8 ) | ((UINT32)LIGHTPROFILE( 0, u1SampleIdx)));
	}

	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_ACTIVE);
}

void vDrvLcdimReadLightProfile(UINT8 *u1LightProfile)
{
	UINT8 u1SampleIdx;
	UINT32 u4Read;


	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_ACTIVE);
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_SEL_0);

	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0, C_LD_WR_AUTO_INC_ADDR);
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 1, C_LD_RD_AUTO_INC_ADDR);

	  // set initial address
	vIO32WriteFldAlign(LD2_SRAM_IF_01, 0, C_LD_ADDR);

	for ( u1SampleIdx = 0; u1SampleIdx < 128 ; u1SampleIdx++ )
	{
		u4Read = u4IO32Read4B(LD2_SRAM_IF_08);		    
			LIGHTPROFILE(3, u1SampleIdx) = (u4Read >> 24) & 0xFF;
			LIGHTPROFILE(2, u1SampleIdx) = (u4Read >> 16) & 0xFF;
			LIGHTPROFILE(1, u1SampleIdx) = (u4Read >>  8) & 0xFF;
			LIGHTPROFILE(0, u1SampleIdx) = (u4Read)       & 0xFF;

		u4Read = u4IO32Read4B(LD2_SRAM_IF_09);
			LIGHTPROFILE(7, u1SampleIdx) = (u4Read >> 24) & 0xFF;
			LIGHTPROFILE(6, u1SampleIdx) = (u4Read >> 16) & 0xFF;
			LIGHTPROFILE(5, u1SampleIdx) = (u4Read >>  8) & 0xFF;
			LIGHTPROFILE(4, u1SampleIdx) = (u4Read)       & 0xFF;

		u4Read = u4IO32Read4B(LD2_SRAM_IF_10);
			LIGHTPROFILE(11, u1SampleIdx) = (u4Read >> 24) & 0xFF;
			LIGHTPROFILE(10, u1SampleIdx) = (u4Read >> 16) & 0xFF;
			LIGHTPROFILE( 9, u1SampleIdx) = (u4Read >>  8) & 0xFF;
			LIGHTPROFILE( 8, u1SampleIdx) = (u4Read)       & 0xFF;

		u4Read = u4IO32Read4B(LD2_SRAM_IF_11);
			LIGHTPROFILE(15, u1SampleIdx) = (u4Read >> 24) & 0xFF;
			LIGHTPROFILE(14, u1SampleIdx) = (u4Read >> 16) & 0xFF;
			LIGHTPROFILE(13, u1SampleIdx) = (u4Read >>  8) & 0xFF;
			LIGHTPROFILE(12, u1SampleIdx) = (u4Read)       & 0xFF;

		u4Read = u4IO32Read4B(LD2_SRAM_IF_12);
			LIGHTPROFILE(19, u1SampleIdx) = (u4Read >> 24) & 0xFF;
			LIGHTPROFILE(18, u1SampleIdx) = (u4Read >> 16) & 0xFF;
			LIGHTPROFILE(17, u1SampleIdx) = (u4Read >>  8) & 0xFF;
			LIGHTPROFILE(16, u1SampleIdx) = (u4Read)       & 0xFF;

		u4Read = u4IO32Read4B(LD2_SRAM_IF_13);
			LIGHTPROFILE(20, u1SampleIdx) = u4Read;
	}
	
}

void vDrvLcdimWriteDimTable(UINT16 *u2DimTable)
{
	UINT8 i;

	// write data into sram
	vIO32WriteFldAlign(LD2_MODE_00, 1, C_LD_EN);
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_ACTIVE);
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_SEL_0);

	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0xFF, C_LD_MAX_WAIT_CYCLE);

	// write
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x1, C_LD_WR_AUTO_INC_ADDR);
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x0, C_LD_RD_AUTO_INC_ADDR);

	// set initial address
	vIO32WriteFldAlign(LD2_SRAM_IF_01, 0, C_LD_ADDR);

	for ( i = 0; i < (LCDIM_DIMMING_NUM - 1); i++ )
	{
		vIO32Write4B(LD2_SRAM_IF_02, (UINT32)((UINT32)u2DimTable[i] << 12 ) | (u2DimTable[i+1]));
	}

	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_ACTIVE);
}

void vDrvLcdimReadDimTable(UINT16 *u2DimTable)
{
	UINT8 i;
	UINT32 u4Read;

	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_ACTIVE);

	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x0, C_LD_WR_AUTO_INC_ADDR);
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x1, C_LD_RD_AUTO_INC_ADDR);

	// set initial address
	vIO32WriteFldAlign(LD2_SRAM_IF_01, 0, C_LD_ADDR);
	
	for ( i = 0; i < (LCDIM_DIMMING_NUM - 1) ; i++ )
	{
		u4Read = u4IO32Read4B(LD2_SRAM_IF_08);
		u2DimTable[i] = (u4Read>>12) & 0xFFF;
	}
	u2DimTable[LCDIM_DIMMING_NUM - 1] = u4Read & 0xFFF;	

	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_ACTIVE);
}

void vDrvLcdimWriteGainLimitTable(UINT16 *u2GainLimitTable)
{
    UINT8 i;

    for(i = 1; i < LCDIM_GAIN_LIMIT_NUM; i+=2)
    {
    	vIO32Write4B(LD2_GAIN_LIMIT_00 + ((i - 1)*2), (UINT32)((UINT32)u2GainLimitTable[i-1] << 16 ) | (u2GainLimitTable[i]));
    }
  	vIO32WriteFldAlign(LD2_GAIN_LIMIT_16, u2GainLimitTable[LCDIM_GAIN_LIMIT_NUM-1], C_LD_GAIN_LIMIT_32);    
}

void vDrvLcdimWriteGainTable(UINT16 *u2GainTable)
{
	UINT8 i;

	// write data into sram
	vIO32WriteFldAlign(LD2_MODE_00, 1, C_LD_EN);
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_ACTIVE);
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_SEL_0);

	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0xFF, C_LD_MAX_WAIT_CYCLE);

	// write
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x1, C_LD_WR_AUTO_INC_ADDR);
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x0, C_LD_RD_AUTO_INC_ADDR);

	// set initial address
	vIO32WriteFldAlign(LD2_SRAM_IF_01, 32, C_LD_ADDR);

	for ( i = 0; i < (LCDIM_GAIN_NUM - 1) ; i++ )
	{
		vIO32Write4B(LD2_SRAM_IF_02, (UINT32)(((UINT32)u2GainTable[i]&0x3FFF) << 14 ) | (u2GainTable[i+1] &0x3FFF));
	}

	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_ACTIVE);
}

void vDrvLcdimReadGainTable(UINT16 *u2GainTable)
{
	UINT8 i;
	UINT32 u4Read;
	vIO32WriteFldAlign(LD2_SRAM_IF_14, 1, C_LD_CPURW_ACTIVE);

	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x0, C_LD_WR_AUTO_INC_ADDR);
	vIO32WriteFldAlign(LD2_SRAM_IF_00, 0x1, C_LD_RD_AUTO_INC_ADDR);

	// set initial address
	vIO32WriteFldAlign(LD2_SRAM_IF_01, 32, C_LD_ADDR);

	for ( i = 0; i < (LCDIM_GAIN_NUM - 1) ; i++ )
	{
		u4Read = u4IO32Read4B(LD2_SRAM_IF_08);
		u2GainTable[i] = (u4Read>>14) & 0x3FFF;
	}
	u2GainTable[LCDIM_GAIN_NUM - 1] = u4Read & 0x3FFF;

	vIO32WriteFldAlign(LD2_SRAM_IF_14, 0, C_LD_CPURW_ACTIVE);

}


void vDrvLcdimWriteSpatialCfg(LCDIM_SPATIAL_CFG_T* prLDSptlCfg)
{
    vIO32WriteFldAlign(LD2_MODE_04, prLDSptlCfg->u1DimWeight0, C_LD_DIM_WET_0);
    vIO32WriteFldAlign(LD2_MODE_04, prLDSptlCfg->u1DimWeight1, C_LD_DIM_WET_1);
    vIO32WriteFldAlign(LD2_MODE_04, prLDSptlCfg->u1DimWeight2, C_LD_DIM_WET_2);
    vIO32WriteFldAlign(LD2_MODE_04, prLDSptlCfg->u1DimWeight3, C_LD_DIM_WET_3);
    vIO32WriteFldAlign(LD2_MODE_05, prLDSptlCfg->u1DimWeight4, C_LD_DIM_WET_4);  

}

void vDrvLcdimWriteTemporalCfg(LCDIM_TEMPORAL_CFG_T* prLDTplCfg)
{
    vIO32WriteFldAlign(LD2_MODE_17, prLDTplCfg->u1UpDimMax, C_LD_MAX_UP_DIM_STEP);
    vIO32WriteFldAlign(LD2_MODE_17, prLDTplCfg->u1DnDimMax, C_LD_MAX_DN_DIM_STEP);
    vIO32WriteFldAlign(LD2_MODE_06, prLDTplCfg->u1DimSlope, C_LD_DY_DIM_SLOPE);   
    vIO32WriteFldAlign(LD2_MODE_06, prLDTplCfg->u1DimStepSft, C_LD_DIM_STEP_SHIFT);
}

UINT8 u1DrvLcDimCalAvgDivider(void)
{
	UINT32 u4BlockPxlCnt = _u4BlockPxlH * _u4BlockPxlV;
	UINT32 u4BlockLumaSumMax = _u4BlockPxlHLast * _u4BlockPxlVLast * 0xFF;
	UINT32 u4ShiftA=0, u4MultiplyB=0, u4ShiftC=0, u4ShiftS=0;
	UINT32 u4Shift_Total;

	for(u4Shift_Total = 1; u4Shift_Total < 100; u4Shift_Total++)	
	{
        u4MultiplyB = (((UINT32)0x1 << u4Shift_Total) + (u4BlockPxlCnt >> 1)) / u4BlockPxlCnt;

        if((u4MultiplyB >= 512) && (u4MultiplyB <= 1023))
        {
            for(u4ShiftS = 1; u4ShiftS < 1023; u4ShiftS++)    
            {              
                if( (u4BlockLumaSumMax>>u4ShiftS) < (1<<20))
                {
                    for(u4ShiftA = 8; u4ShiftA > 0; u4ShiftA--)  
                    {
                        if( ((u4BlockLumaSumMax>>u4ShiftS)>>u4ShiftA) < (1<<12))
                        {
                            u4ShiftC = u4Shift_Total - u4ShiftS - u4ShiftA - 2;
                            
                            vIO32WriteFldAlign(LD2_MODE_16, u4ShiftS, C_LD_BLKAVG_LSUM_SHIFT);
                            vIO32WriteFldAlign(LD2_MODE_16, u4ShiftA, C_LD_BLKAVE_SHIFT_A);
                            vIO32WriteFldAlign(LD2_MODE_16, u4MultiplyB, C_LD_BLKAVE_MULT_B);
                            vIO32WriteFldAlign(LD2_MODE_16, u4ShiftC, C_LD_BLKAVE_SHIFT_C);       
                            Printf("-> u4ShiftS = %d\n-> u4ShiftA = %d\n-> u4MultiplyB = %d\n-> u4ShiftC = %d\n", 
                                u4ShiftS, u4ShiftA, u4MultiplyB, u4ShiftC);

                            return SV_TRUE;
                        }
                        /*
                        else
                        {
                            Printf("u4ShiftS calculation fail, u4BlockLumaSumMax = %d \n", u4BlockLumaSumMax);
                        }
                        */
                    }
                }
                /*
                else
                {
                    Printf("calculation fail, u4ShiftS = %d, u4BlockLumaSumMax = %d \n", u4ShiftS, u4BlockLumaSumMax);
                }
                */
            }
        }
        /*
        else
        {
            Printf("calculation fail. u4MultiplyB = %d,  %d \n", u4MultiplyB, (((UINT32)0x1 << u4Shift_Total) + (u4BlockPxlCnt >> 1)) / u4BlockPxlCnt);
        }
        */
          
    }

		Printf("LcDimCalAvgDivider fail, panel out of spec\n");

        return SV_FALSE;
}

void vDrvLcdimResetTemporalFilter(UINT8 bOnOff)
{
    vIO32WriteFldAlign(LD2_MODE_00, bOnOff,  C_LD_SCENE_CHG);
}

//UINT8 u1LcdimPhase[LDM_LP_TOTAL_PHASE_NUM * 2] = 
void vDrvLcdimSetPhase(UINT8 *u1LcdimPhase)
{
    // x index of phase 0 ~ 7
    vIO32Write4B(LD2_MODE_10, (UINT32)((UINT32)u1LcdimPhase[ 0] | ((UINT32)u1LcdimPhase[2] << 4) | 
                             ((UINT32)u1LcdimPhase[ 4] <<  8) | ((UINT32)u1LcdimPhase[ 6] << 12) | ((UINT32)u1LcdimPhase[ 8] << 16) |
                             ((UINT32)u1LcdimPhase[10] << 20) | ((UINT32)u1LcdimPhase[12] << 24) | ((UINT32)u1LcdimPhase[14] << 28)));
    // x index of phase 8 ~ 16
    vIO32Write4B(LD2_MODE_11, (UINT32)((UINT32)u1LcdimPhase[16] | ((UINT32)u1LcdimPhase[18] << 4) | 
                             ((UINT32)u1LcdimPhase[20] <<  8) | ((UINT32)u1LcdimPhase[22] << 12) | ((UINT32)u1LcdimPhase[24] << 16) |
                             ((UINT32)u1LcdimPhase[26] << 20) | ((UINT32)u1LcdimPhase[28] << 24) | ((UINT32)u1LcdimPhase[30] << 28)));
    // x index of phase 17 ~ 21
    vIO32Write4B(LD2_MODE_12, (UINT32)((UINT32)u1LcdimPhase[32] | ((UINT32)u1LcdimPhase[34] << 4) | 
                             ((UINT32)u1LcdimPhase[36] <<  8) | ((UINT32)u1LcdimPhase[38] << 12) | ((UINT32)u1LcdimPhase[40] << 16)));    

    // y index of phase 0 ~ 7
    vIO32Write4B(LD2_MODE_13, (UINT32)((UINT32)u1LcdimPhase[ 1] | ((UINT32)u1LcdimPhase[3] << 4) | 
                             ((UINT32)u1LcdimPhase[ 5] <<  8) | ((UINT32)u1LcdimPhase[ 7] << 12) | ((UINT32)u1LcdimPhase[ 9] << 16) |
                             ((UINT32)u1LcdimPhase[11] << 20) | ((UINT32)u1LcdimPhase[13] << 24) | ((UINT32)u1LcdimPhase[15] << 28)));
    // y index of phase 8 ~ 16
    vIO32Write4B(LD2_MODE_14, (UINT32)((UINT32)u1LcdimPhase[17] | ((UINT32)u1LcdimPhase[19] << 4) | 
                             ((UINT32)u1LcdimPhase[21] <<  8) | ((UINT32)u1LcdimPhase[23] << 12) | ((UINT32)u1LcdimPhase[25] << 16) |
                             ((UINT32)u1LcdimPhase[27] << 20) | ((UINT32)u1LcdimPhase[29] << 24) | ((UINT32)u1LcdimPhase[31] << 28)));
    // y index of phase 17 ~ 21
    vIO32Write4B(LD2_MODE_15, (UINT32)((UINT32)u1LcdimPhase[33] | ((UINT32)u1LcdimPhase[35] << 4) | 
                             ((UINT32)u1LcdimPhase[37] <<  8) | ((UINT32)u1LcdimPhase[39] << 12) | ((UINT32)u1LcdimPhase[41] << 16)));    

    vIO32WriteFldAlign(LD2_MODE_09, u1LcdimPhase[42],  C_LD_PHASE_VERT_RATIO);
    vIO32WriteFldAlign(LD2_MODE_09, u1LcdimPhase[43],  C_LD_PHASE_VERT);
    vIO32WriteFldAlign(LD2_MODE_09, u1LcdimPhase[44],  C_LD_PHASE_HOR_RATIO);
    vIO32WriteFldAlign(LD2_MODE_09, u1LcdimPhase[45],  C_LD_PHASE_HOR);        
}

void vDrvSetLcDimOnOff(UINT8 bOnOff)
{
    /*
    Combination of three registers: {C_LD_EN, C_LD_BYPASS, C_LD_FORCE_DIM_EN}
    0X0: disable data-stretch function with last dimming 
    0X1, 111: disable data-stretch function with fixed dimming value
    100: data-stretch function with normal dimming control (normal mode)
    101: data-stretch function with fixed dimming value
    110: disable data-stretch function with normal dimiming control
    */    
    if ((!IS_SupportDimming()) || (bSupportLocalDimming == SV_FALSE))
    {
        bOnOff = SV_OFF;
    }
    if(bOnOff)    
    {
        vIO32WriteFldAlign(LD2_MODE_00, 1,  C_LD_EN);
        vIO32WriteFldAlign(LD2_MODE_09, 0, C_LD_FORCE_DIM_EN);        
    	vIO32WriteFldAlign(LD2_MODE_00, 0, C_LD_BYPASS);          
    	vIO32WriteFldAlign(LD2_MODE_00, 0, C_LD_DIS_DS);                
//        vIO32WriteFldAlign(ADAP_LCDIM_00, 1, ADAP_LCDIM_FORCE_EN);
        vDrvLcdimResetTemporalFilter(SV_OFF);
        u1LCDIMOnOffStatus = 1;
    }
    else
    {    
//        vIO32WriteFldAlign(LD2_MODE_00, 0,  C_LD_EN);
        vIO32WriteFldAlign(LD2_MODE_00, 1,  C_LD_EN);   //always enable LCD to update dimming result
        vIO32WriteFldAlign(LD2_MODE_09, 1, C_LD_FORCE_DIM_EN);
    	vIO32WriteFldAlign(LD2_MODE_00, 1, C_LD_BYPASS);
    	vIO32WriteFldAlign(LD2_MODE_00, 1, C_LD_DIS_DS);                
        vIO32WriteFldAlign(LD2_MODE_09, 0x3FF,   C_LD_FORCE_DIM_VALUE);        
//        vIO32WriteFldAlign(ADAP_LCDIM_00, 0, ADAP_LCDIM_FORCE_EN);
        vDrvLcdimWriteDimTable(u2LcdimDimTable_OFF);
        vDrvLcdimWriteGainTable(u2LcdimGainTable);
        vDrvLcdimResetTemporalFilter(SV_ON);
        u1LCDIMOnOffStatus = 0;
    }
}

UINT8 vDrvGetLcDimOnOffStatus(void)
{
    UINT8 u1LcdimEnable = (IO32ReadFldAlign(LD2_MODE_09, C_LD_FORCE_DIM_EN) == 1) ? 0 : 1;
    
    return u1LcdimEnable;
}

BOOL bDrvLcDimSetPanelInfo(UINT32 u4BlockNumH, UINT32 u4BlockNumV, UINT8 u1LightDirection)
{
    /*
        note : u1LightDirection is used for 1-D Edge-Type Dimming
    */
	UINT32 u4PanelWidth, u4PanelHeight; 
	UINT32 u4BlockWidth, u4BlockHeight; 	
	UINT32 u4BlockPxlHLast, u4BlockPxlVLast;

    u2BlockH = (UINT16)u4BlockNumH;
    u2BlockV = (UINT16)u4BlockNumV;

	u4PanelWidth = PANEL_GetPanelWidth();
	u4PanelHeight = PANEL_GetPanelHeight();

    u4BlockWidth = u4PanelWidth / u4BlockNumH;
    u4BlockHeight = u4PanelHeight / u4BlockNumV;    
	u4BlockPxlHLast = (u4PanelWidth / u4BlockNumH) + (u4PanelWidth % u4BlockNumH);
	u4BlockPxlVLast = (u4PanelHeight / u4BlockNumV) + (u4PanelHeight % u4BlockNumV);

    _u4BlockPxlH = u4BlockWidth;
    _u4BlockPxlV = u4BlockHeight;
    _u4BlockPxlHLast = u4BlockPxlHLast;
    _u4BlockPxlVLast = u4BlockPxlVLast;   

	vIO32WriteFldAlign(LD2_MODE_01, u2BlockH * u2BlockV, C_LD_BLK_NUM);
	vIO32WriteFldAlign(LD2_MODE_00, u2BlockV, C_LD_BLK_VERT_NUM);
	vIO32WriteFldAlign(LD2_MODE_00, u2BlockH, C_LD_BLK_HOR_NUM);    
	vIO32WriteFldAlign(LD2_MODE_01, u4BlockHeight      , C_LD_BLK_HEIGHT);
	vIO32WriteFldAlign(LD2_MODE_01, u4BlockWidth       , C_LD_BLK_WIDTH);	


    if((u4BlockNumH == 1) && (u4BlockNumV == 1))        
    {
        // ---------------------------------------------
        // (1) Zero-D Dimming
        // ---------------------------------------------        
    	vIO32WriteFldAlign(LD2_MODE_02, LCDIM_PANEL_TYPE_DIRECT, C_LD_BLK_CTR_MODE);		        
    	vIO32WriteFldAlign(LD2_MODE_02, u4BlockWidth / 2 , C_LD_BLK_CTR_H1);		        
    	vIO32WriteFldAlign(LD2_MODE_02, u4BlockHeight / 2, C_LD_BLK_CTR_V1);		        
    }
    else if(u4BlockNumH == 1)                          
    {
    	vIO32WriteFldAlign(LD2_MODE_02, LCDIM_PANEL_TYPE_LEFT_RIGHT, C_LD_BLK_CTR_MODE);        

        if(u1LightDirection ==  PANEL_LIGHT_DIR_L)   
        {
        // ---------------------------------------------
        // (2) 1-D L/R Type Dimming (Light From Left)
        // ---------------------------------------------
    	    vIO32WriteFldAlign(LD2_MODE_02, 0, C_LD_BLK_CTR_H1);		        
        	vIO32WriteFldAlign(LD2_MODE_02, u4BlockHeight / 2, C_LD_BLK_CTR_V1);		        
        }
        else                                            
        {
        // ---------------------------------------------
        // (3) 1-D L/R Type Dimming (Light From Right)
        // ---------------------------------------------            
    	    vIO32WriteFldAlign(LD2_MODE_02, u4BlockWidth - 1 , C_LD_BLK_CTR_H1);		        
        	vIO32WriteFldAlign(LD2_MODE_02, u4BlockHeight / 2, C_LD_BLK_CTR_V1);		        
        }
    }
    else if(u4BlockNumV == 1)                           
    {
    	vIO32WriteFldAlign(LD2_MODE_02, LCDIM_PANEL_TYPE_TOP_BOTTOM, C_LD_BLK_CTR_MODE);        

        if(u1LightDirection ==  PANEL_LIGHT_DIR_T)    
        {
        // ---------------------------------------------
        // (4) 1-D T/B Type Dimming (Light From Top)
        // --------------------------------------------- 
    	    vIO32WriteFldAlign(LD2_MODE_02, u4BlockWidth / 2, C_LD_BLK_CTR_H1);		        
        	vIO32WriteFldAlign(LD2_MODE_02, 0, C_LD_BLK_CTR_V1);		        
        }
        else                                            
        {
        // ---------------------------------------------
        // (5) 1-D T/B Type Dimming (Light From Bottom)
        // ---------------------------------------------             
    	    vIO32WriteFldAlign(LD2_MODE_02, u4BlockWidth / 2 , C_LD_BLK_CTR_H1);		        
//        	vIO32WriteFldAlign(LD2_MODE_02, u4BlockHeight - 1, C_LD_BLK_CTR_V1);		        
            vIO32WriteFldAlign(LD2_MODE_02, 0, C_LD_BLK_CTR_V1); //Use top light profile
        }
    }
    else if(u4BlockNumH == 2)                           
    {
        // ---------------------------------------------
        // (6) 2-D L/R Edge-Type Dimming
        // ---------------------------------------------         
    	vIO32WriteFldAlign(LD2_MODE_02, LCDIM_PANEL_TYPE_LEFT_RIGHT, C_LD_BLK_CTR_MODE);                
   	    vIO32WriteFldAlign(LD2_MODE_02, 0 , C_LD_BLK_CTR_H1);		        
      	vIO32WriteFldAlign(LD2_MODE_02, u4BlockHeight / 2, C_LD_BLK_CTR_V1);	
    }
    else if(u4BlockNumV == 2)                           
    {
        // ---------------------------------------------
        // (7) 2-D T/B Edge-Type Dimming
        // ---------------------------------------------         
    	vIO32WriteFldAlign(LD2_MODE_02, LCDIM_PANEL_TYPE_TOP_BOTTOM, C_LD_BLK_CTR_MODE);                
   	    vIO32WriteFldAlign(LD2_MODE_02, u4BlockWidth / 2 , C_LD_BLK_CTR_H1);		        
      	vIO32WriteFldAlign(LD2_MODE_02, 0, C_LD_BLK_CTR_V1);	
    }    
    else
    {
        // ---------------------------------------------
        // (8) 2-D Direct-Type Dimming
        // ---------------------------------------------        
    	vIO32WriteFldAlign(LD2_MODE_02, LCDIM_PANEL_TYPE_DIRECT, C_LD_BLK_CTR_MODE);                
   	    vIO32WriteFldAlign(LD2_MODE_02, u4BlockWidth / 2 , C_LD_BLK_CTR_H1);		        
      	vIO32WriteFldAlign(LD2_MODE_02, u4BlockHeight / 2, C_LD_BLK_CTR_V1);	        
    }
    
    return SV_TRUE;
}

void vDrvLcdimSetInk(UINT8 u1InkType)
{
    /*
    0: Disable  (default)
    1: Current dimming luminance distribution
    2: Full dimming luminance distribution
    3: Pixel gain (after gain limited)
    4: Force image data (ink_r/g/b)
    5: Block dimming
    6: Image data without data stretch
    7: Debug Ink (refer to dbg_sel)
    */    
    vIO32WriteFldAlign(LD2_MODE_03, u1InkType, C_LD_INK);	     
}

void vDrvLcdimSetInkColor(UINT16 u2ColorR, UINT16 u2ColorG, UINT16 u2ColorB)
{
    vIO32WriteFldAlign(LD2_INK_DATA, u2ColorR, C_LD_INK_R);	     
    vIO32WriteFldAlign(LD2_INK_DATA, u2ColorG, C_LD_INK_G);	     
    vIO32WriteFldAlign(LD2_INK_DATA, u2ColorB, C_LD_INK_B);	     
}
/*
u1InfoType=0/3->blending, 2 -> avg,1 -> rgb max
*/
UINT32 u4DrvLcdimReadBlkInfoV3(UINT8 u1InfoType,UINT8 * pu1BLBlockDataTbl,UINT8 u1BlkStartNum)
{
    UINT8 i,j;
    UINT32 u4Info;
	vIO32WriteFldAlign(LD2_MODE_31,u1InfoType,C_LD_BLKINFO_SEL);	
    if((pu1BLBlockDataTbl==NULL)||!(IO32ReadFldAlign(LD2_MODE_32,LD_BLKINFO_READY)))
    {
        return 0;
    }
	//for(k=0;k<=u4BlkNum/32;k++)
	//{
	LOG(3,"u4DrvLcdimReadBlkInfoV3 startnum=%d \n",u1BlkStartNum*32);
	
	vIO32WriteFldAlign(LD2_MODE_31,32*u1BlkStartNum,C_LD_BLKINFO_START_NUM);
	for(i=0;i<6;i++)
	{
		u4Info=u4IO32Read4B(LD2_MODE_25+4*i);
		for(j=0;j<5;j++)
		{
			*(pu1BLBlockDataTbl+32*u1BlkStartNum+i*5+j)=(u4Info>>(j*6))&0x3f;
			LOG(3,"%d\t",(u4Info>>(j*6))&0x3f);
		}
		LOG(3,"\n");
	}
	*(pu1BLBlockDataTbl+32*u1BlkStartNum+30)=IO32ReadFldAlign(LD2_MODE_31,LD_BLKINFO_030);	
	*(pu1BLBlockDataTbl+32*u1BlkStartNum+31)=IO32ReadFldAlign(LD2_MODE_31,LD_BLKINFO_031);
	LOG(3,"%d\t %d\t",*(pu1BLBlockDataTbl+32*u1BlkStartNum+30),*(pu1BLBlockDataTbl+32*u1BlkStartNum+31));
	LOG(3,"\n");
	//}

	return 1;
}

UINT32 u4DrvLcdimReadBlockInfo(UINT8 u1H, UINT8 u1V, UINT8 u1InfoType)
{
    UINT16 u2BlockIndex = u1V * u2BlockH + u1H;
    UINT32 u4Info;

    vIO32WriteFldAlign(LD2_STATUS_00, u2BlockIndex, C_LD_READ_BLK_NUM);	

    switch (u1InfoType)
    {
        case LCDIM_BLOCK_INFO_MAX:
            u4Info = IO32ReadFldAlign(LD2_STATUS_00, LD_MAX_INFO);
            break;    
        case LCDIM_BLOCK_INFO_OUT_DIM:
            u4Info = IO32ReadFldAlign(LD2_STATUS_01, LD_OUT_DIM);            
            break;    
        case LCDIM_BLOCK_INFO_TAR_DIM:
            u4Info = IO32ReadFldAlign(LD2_STATUS_01, LD_TAR_DIM);            
            break;    
        case LCDIM_BLOCK_INFO_CONF:
            u4Info = IO32ReadFldAlign(LD2_STATUS_01, LD_CONFIDENCE);            
            break;    
 
        default:
            u4Info = 0;
            break;
    }

    return u4Info;
}

void vDrvSetLcDSOnOff(BOOL bOnOff)
{
	vIO32WriteFldAlign(LD2_MODE_00, !bOnOff, C_LD_DIS_DS);    
}

void vDrvLcdimSetDSDemo(UINT8 u1DemoType, BOOL bOnOff)
{

    if (u1DemoType == 0)     // Store Demo
    {
        if (bOnOff == SV_TRUE)
        {
            // Set ink mode
            vIO32WriteFldAlign(LD2_MODE_03, 4, C_LD_INK);        // set to flat level
            vIO32WriteFldAlign(LD2_INK_DATA, 0x3FF, C_LD_INK_B);
            vIO32WriteFldAlign(LD2_INK_DATA, 0x3FF, C_LD_INK_G);
            vIO32WriteFldAlign(LD2_INK_DATA, 0x3FF, C_LD_INK_R);


            // Set full white ink
            vIO32WriteFldAlign(LD2_DEMO_00, 0, C_LD_DEMO_XSTART);
            vIO32WriteFldAlign(LD2_DEMO_01, PANEL_GetPanelWidth(), C_LD_DEMO_XEND);
            vIO32WriteFldAlign(LD2_DEMO_00, 0, C_LD_DEMO_YSTART);
            vIO32WriteFldAlign(LD2_DEMO_01, PANEL_GetPanelHeight(), C_LD_DEMO_YEND);

        }
        else
        {
            vIO32WriteFldAlign(LD2_MODE_03, 0, C_LD_INK);        // disable ink mode

        }
    }
    else                    // Factory Line Demo
    {
        if (bOnOff == SV_TRUE)
        {
            // Set ink mode
            vIO32WriteFldAlign(LD2_MODE_03, 4, C_LD_INK);        // set to flat level
            vIO32WriteFldAlign(LD2_INK_DATA, 0x3FF, C_LD_INK_B);
            vIO32WriteFldAlign(LD2_INK_DATA, 0x3FF, C_LD_INK_G);
            vIO32WriteFldAlign(LD2_INK_DATA, 0x3FF, C_LD_INK_R);

            
            // Set full white ink
            vIO32WriteFldAlign(LD2_DEMO_00, 0, C_LD_DEMO_XSTART);
            vIO32WriteFldAlign(LD2_DEMO_01, PANEL_GetPanelWidth(), C_LD_DEMO_XEND);
            vIO32WriteFldAlign(LD2_DEMO_00, 0, C_LD_DEMO_YSTART);
            vIO32WriteFldAlign(LD2_DEMO_01, PANEL_GetPanelHeight(), C_LD_DEMO_YEND);

/*
            if (_u4BlockNumH != 0)
            {
                u4DemoBlockWidth = PANEL_GetPanelWidth()/_u4BlockNumH;
            }
*/            
            u4CurrentDemoBlock = 0;
            u4DemoFrameCount = 0;

            vIO32WriteFldAlign(LCDIM_DEMO_SETTING_0, SV_ON, LCDIM_DEMO_EN);     
            
        }
        else
        {
            vIO32WriteFldAlign(LD2_MODE_03, 0, C_LD_INK);        // disable ink mode

            vIO32WriteFldAlign(LCDIM_DEMO_SETTING_0, SV_OFF, LCDIM_DEMO_EN); 

            u4CurrentDemoBlock = 0;
            u4DemoFrameCount = 0;
        }
    }
}

void vDrvLcdimSetDimSWMode(UINT16 *u2DimmingResult)
{
    static UINT16 u2Buffer[LCDIM_SW_MODE_MAX_BLK_NUM];
    UINT8 i;
//    UINT32 u4Read;

    UINT32 u4DimSum = 0;
    UINT16 u2TotalBlock = u2BlockV * u2BlockH;    
//1. Enable FW mode
    if(u2TotalBlock > LCDIM_SW_MODE_MAX_BLK_NUM)
    {
        vIO32WriteFldAlign(LD2_MODE_33, SV_OFF, C_LD_FW_DIM_EN);
        return;
    }
    else
    {
        vIO32WriteFldAlign(LD2_MODE_33, SV_ON, C_LD_FW_DIM_EN);
    }


    for(i = 0; i < LCDIM_SW_MODE_MAX_BLK_NUM; i++)
    {
        u2Buffer[i] = (i < u2TotalBlock) ? u2DimmingResult[i] : 0;
    }

//2. Write to FW Dim


    for(i = 0; i < (LCDIM_SW_MODE_MAX_BLK_NUM/3); i++)
    {
        vIO32WriteFldAlign(LD2_MODE_33+i*4, u2Buffer[i*3], C_LD_FW_DIM_000);
        vIO32WriteFldAlign(LD2_MODE_33+i*4, u2Buffer[i*3+1], C_LD_FW_DIM_001);
        vIO32WriteFldAlign(LD2_MODE_33+i*4, u2Buffer[i*3+2], C_LD_FW_DIM_002);
    }

    for(i = 0; i < LCDIM_SW_MODE_MAX_BLK_NUM; i++)
    {
        u4DimSum += u2Buffer[i];
    }    
    
    vIO32Write4B(LD2_MODE_41, u4DimSum);
}

void vDrvGetLcDimDimmingResultHW(UINT16 *u2DimmingResult) 
{
    UINT16 u2CurPos = 3;
    UINT16 u2CurAddrOfst = 0;
    UINT32 u4Read;
    UINT16 u2TotalBlock = u2BlockV * u2BlockH;
    UINT16 u2Remainder = u2TotalBlock % 3;
    UINT32 u4DimResultSum = 0;

    // optimized code, no need to use division manipulation 
    // -------- optimization begin
    while(u2CurPos <= u2TotalBlock)
    {
        u4Read = u4IO32Read4B(LD2_DIM_RESULT_00 + u2CurAddrOfst);            
    
        u2DimmingResult[u2CurPos-1] = (UINT16)(((u4Read >> 20) & 0x3FF));
        u2DimmingResult[u2CurPos-2] = (UINT16)(((u4Read >> 10) & 0x3FF));
        u2DimmingResult[u2CurPos-3] = (UINT16)((u4Read & 0x3FF));
    
        u2CurPos      += 3;
        u2CurAddrOfst += 4;
    
        u4DimResultSum = u4DimResultSum + ((u4Read >> 20) & 0x3FF) + ((u4Read >> 10) & 0x3FF) + (u4Read & 0x3FF);
    }
    
    if(u2Remainder == 1)
    {
        u4Read = u4IO32Read4B(LD2_DIM_RESULT_00 + u2CurAddrOfst);            
    
        u2DimmingResult[u2CurPos-3] = (UINT16)((u4Read & 0x3FF));
    
        u4DimResultSum = u4DimResultSum + (u4Read & 0x3FF);
    }
    else if(u2Remainder == 2)
    {
        u4Read = u4IO32Read4B(LD2_DIM_RESULT_00 + u2CurAddrOfst);            
    
        u2DimmingResult[u2CurPos-2] = (UINT16)(((u4Read >> 10) & 0x3FF));
        u2DimmingResult[u2CurPos-3] = (UINT16)((u4Read & 0x3FF));
    
        u4DimResultSum = u4DimResultSum + ((u4Read >> 10) & 0x3FF) + (u4Read & 0x3FF);
    }
    // -------- optimization end     

}


#ifndef NDEBUG    // PQ SRAM Stress Test Only Take Effect in Debug Build
BOOL vDrvVerifyLcDimLightProfile(UINT8 *bLcDimLightProfile)
{
    UINT16 index;
    UINT8 bRet = SV_TRUE;

    vDrvLcdimReadLightProfile(bLcDimLightProfileReadBack);

    for (index = 0; index < LCDIM_LIGHTPROF_NUM; index++)
    {
        if(bLcDimLightProfileReadBack[index] != bLcDimLightProfile[index])
        {
            Printf("[FAIL] LCDIM Light Profile [%d] : [W] = %d, [R] = %d\n", index, bLcDimLightProfile[index], 
                                                               bLcDimLightProfileReadBack[index]);
            bRet = SV_FALSE;
        }
    }

    return bRet;
}

BOOL vDrvVerifyLcDimDimTBL(UINT16 *u2LcDimDimTbl)
{
    UINT16 u2LcDimDimTblReadBack[LCDIM_DIMMING_NUM];
    UINT16 index;
    UINT8 bRet = SV_TRUE;

    vDrvLcdimReadDimTable(u2LcDimDimTblReadBack);

    for (index = 0; index < LCDIM_DIMMING_NUM; index++)
    {
        if(u2LcDimDimTblReadBack[index] != u2LcDimDimTbl[index])
        {
            Printf("[FAIL] LCDIM Dim Table [%d] : [W] = %d, [R] = %d\n", index, u2LcDimDimTbl[index], 
                                                               u2LcDimDimTblReadBack[index]);
            bRet = SV_FALSE;
        }
    }

    return bRet;
}

BOOL vDrvVerifyLcDimGainTBL(UINT16 *u2LcDimGainTbl)
{
    UINT16 u2LcDimGainTblReadBack[LCDIM_GAIN_NUM];
    UINT16 index;
    UINT8 bRet = SV_TRUE;

    vDrvLcdimReadGainTable(u2LcDimGainTblReadBack);

    for (index = 0; index < LCDIM_GAIN_NUM; index++)
    {
        if(u2LcDimGainTblReadBack[index] != u2LcDimGainTbl[index])
        {
            Printf("[FAIL] LCDIM Gain Table [%d] : [W] = %d, [R] = %d\n", index, u2LcDimGainTbl[index], 
                                                               u2LcDimGainTblReadBack[index]);
            bRet = SV_FALSE;
        }
    }

    return bRet;
}

#endif // NDEBUG   

