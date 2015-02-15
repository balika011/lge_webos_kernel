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
 * $RCSfile: drv_upscaler.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_ospe.c
 *  Brief of file drv_ospe.c.
 *  Details of file drv_ospe.c (optional).
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "general.h"
#include "hw_scpip_pq.h"
#include "hw_psc.h"
#include "drv_upscaler.h"
#include "drv_scaler.h"
#include "drv_scaler_psc.h"
#include "hw_tdsharp.h"
#include "vdo_misc.h"
#include "x_rand.h"
#include "nptv_debug.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

/**************************************************************************
 * PEI and Dering and Initial
 *************************************************************************/
#ifdef CC_MT5398
REGTBL_T const CODE REGTBL_PEI[] =
{  
	{SC_PEI_00, 0x00000005, 0xE7F00005},    
	{SC_PEI_01, 0x00F04912, 0x01F0FF1F},    
	{SC_PEI_02, 0x76007040, 0xFF81FFFF},    
	{SC_PEI_03, 0x00054444, 0x000FFFFF},    
	{SC_PEI_04, 0x00064864, 0x000FFFFF},    
	{SC_PEI_05, 0x04244842, 0x0FFFFFFF},    
	{SC_PEI_08, 0x101002BF, 0x77FF07FF},    
	{SC_PEI_09, 0x001001CF, 0x07FF07FF},    
	{SC_PEI_10, 0x101002BF, 0x77FF07FF},    
	{SC_PEI_11, 0x001001CF, 0x07FF07FF},    
	{SC_PEI_12, 0x28B2CC8C, 0x7FFFFFBF},
	{SC_PEI_13, 0x7A141014, 0xFFFFFFFF},
	{REGTBL_END, 0x00000000, 0x00000000},  
};
#endif

REGTBL_T const CODE REGTBL_DE_RING[] =
{
	{SCFIR_09, 0x30310000, 0xb0fff000},		// VER_DERING
	{SCFIR_0A, 0x303013FF, 0xb0ffffff},		// HOR_Y_DERING
	{SCFIR_0B, 0x0206FF60, 0x0ffffff0},		// HOR_UV_DERING
	{REGTBL_END, 0x00000000, 0x00000000},
};

REGTBL_T const CODE REGTBL_Tap_Adap[] =
{
    {SCFIR_0F, 0x8429E531, 0xFFFFFFFF},   
    {SCFIR_10, 0x88084215, 0xFFFFFFFF}, 
    {SCFIR_11, 0x00440000, 0x00FF0000},     
    {REGTBL_END, 0x00000000, 0x00000000},
};

/**************************************************************************
 * Filter Coefficients Definition
 *************************************************************************/

UINT32 u4SCHYCoeff[][SCPQ_COEF_TBLSIZE] =
{
    #include "Coeff/HY_up422.txt"     // upscale coeff       
    #include "Coeff/HY_ds422_1_5.txt" // down scale 1.5 coeff
    #include "Coeff/HY_ds422_2.txt"   // down scale 2 coeff
    #include "Coeff/HY_ds422_3.txt"   // down scale 3 coeff
    #include "Coeff/HY_ds422_4.txt"   // down scale 4 coeff
    #include "Coeff/HY_ds422_5.txt"   // down scale 5 coeff
};
UINT32 u4SCHCCoeff[][SCPQ_COEF_TBLSIZE] =
{
    #include "Coeff/HC_up422.txt"     // upscale coeff       
    #include "Coeff/HC_ds422_1_5.txt" // down scale 1.5 coeff        
    #include "Coeff/HC_ds422_2.txt"   // down scale 2 coeff
    #include "Coeff/HC_ds422_3.txt"   // down scale 3 coeff
    #include "Coeff/HC_ds422_4.txt"   // down scale 4 coeff
    #include "Coeff/HC_ds422_5.txt"   // down scale 5 coeff
};
UINT32 u4SCVYCCoeff[][SCPQ_COEF_TBLSIZE] =
{
    #include "Coeff/VYC_up.txt"     // upscale coeff       
    #include "Coeff/VYC_ds_1_5.txt" // down scale 1.5 coeff        
    #include "Coeff/VYC_ds_2.txt"   // down scale 2 coeff
    #include "Coeff/VYC_ds_3.txt"   // down scale 3 coeff
    #include "Coeff/VYC_ds_4.txt"   // down scale 4 coeff
    #include "Coeff/VYC_ds_5.txt"   // down scale 5 coeff
	#include "Coeff/VYC_4Tapsup.txt"	  // upscaler coeff 4-taps for 3D
};

UINT32 dwHCOEFF_MSBVALID_MASK[3]={0x3FFFFFFF,0xFFFFFFFF,0xFFFFFFFF}; //LSB 94 bit valid in 3 dwords
UINT32 dwVCOEFF_MSBVALID_MASK[3]={0x00000000,0x0FFFFFFF,0xFFFFFFFF}; //LSB 60 bit valid in 3 dwords

EXTERN UINT8 bForceAllPQOff;

//UINT32 u4SCVYCCoeff[][SCPQ_COEF_TBLSIZE] =
//{
//    #include "Coeff/VYC_up422.txt"        
//    #include "Coeff/VYC_ds422_1_5.txt"
//    #include "Coeff/VYC_ds422_2.txt"
//    #include "Coeff/VYC_ds422_3.txt"
//    #include "Coeff/VYC_ds422_4.txt"
//    #include "Coeff//VYC_ds422_5.txt"
//};
//-----------------------------------------------------------------------------
/**
 * @brief vDrvUpScalerInit

 * @param void
 * @retval void
 */
//-----------------------------------------------------------------------------
void vDrvSCPQInit(void)
{  
    // Set UpScaler Coefficient from tbl -> Use Hw default.
    //vDrvSetUpScalerFilter(SV_VP_MAIN, UPSCALER_HORI, 0);
    //vDrvSetUpScalerFilter(SV_VP_MAIN, UPSCALER_VERT, 0);

    // Disable SRAM coefficient and use Hw default tbl.
    vIO32WriteFldAlign(VDP_SCPQ_01, 0x0, SCPQ_CUST_UPCOEFF);
    vDrvSCPQSetSramCoeff(COEFTYP_HOR_Y,u4SCHYCoeff[0]);
    vDrvSCPQSetSramCoeff(COEFTYP_HOR_C,u4SCHCCoeff[0]);
    vDrvSCPQSetSramCoeff(COEFTYP_VER_Y,u4SCVYCCoeff[0]);
    vDrvSCPQSetSramCoeff(COEFTYP_VER_C,u4SCVYCCoeff[0]);
    vDrvSCPQSramCoeffOnOff(SV_ON);

    //De-ring    
    vDrvDeRingInit();
    vDrvDeRingOnOff(SV_VP_MAIN, SV_ON);

    //Tap-adap
    vDrvSCPQTapAdapInit(); 
    vDrvSCPQTapAdapOnOff(SV_ON);
    
    //VY, C Sram tbl separate
    vIO32WriteFldAlign(SCPIP_SCCTRL1_05, 0x1, SCCTRL1_05_SC_COEF_PP_RD_A);
    
#ifdef CC_MT5398
    // Disable SRAM coefficient and use Hw default tbl.
    //vDrvSetPostUpScalerFilter(SV_VP_MAIN, UPSCALER_HORI, 0);
    //vDrvSetPostUpScalerFilter(SV_VP_MAIN, UPSCALER_VERT, 0); 
    vIO32WriteFldAlign(PSCFIR_0C, SV_OFF, PSC_H_SRAM_COEFF_EN);
    vIO32WriteFldAlign(PSCFIR_0C, SV_OFF, PSC_V_SRAM_COEFF_EN);

    // SCPIP_PQ Initialize    
    vDrvPostDeRingInit();

	// Post de-ring off
    vDrvPostDeRingOnOff(SV_VP_MAIN, SV_ON);

	//PEI off
    vDrvUsPeiInit();	
    vDrvUsPeiOnOff(SV_ON);
#else
	vIO32WriteFldAlign(SCPIP_SCCTRL1_0C, SV_OFF, SCCTRL1_0C_SC_PE_ENABLE);
#endif


}

//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------  FRONT UPSCALER ------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
void vDrvSCPQSramCoeffOnOff(UINT8 bOnOff)
{
    vIO32WriteFldAlign(SCFIR_0C, bOnOff, H_SRAM_COEFF_EN);
    vIO32WriteFldAlign(SCFIR_0C, bOnOff, V_SRAM_COEFF_EN);
}

void vDrvSCPQSetSramCoeff(UINT8 bType, UINT32 dwCoeff[SCPQ_COEF_TBLSIZE])
{

    UINT16 i;

	if(bType>=COEFTYP_MAX)
	{
        Printf("Set Scaler PQ SRAM Coeff Type: %d,  Wrong !!\n", bType);
	}

	//initial setting before writing coefficient
	//vDrvSCPQSramCoeffOnOff(SV_OFF);
    
    vIO32WriteFldMulti(SCFIR_00, P_Fld(0x1, RD_AUTO_INC_ADDR)|
		  						 P_Fld(0x1, WR_AUTO_INC_ADDR)|
    							 P_Fld(0x0, PIO_MODE)|
		  						 P_Fld(0x0, FORCE_WR_NO_WAIT)|
		  						 P_Fld(0xFF, MAX_WAIT_CYCLE)); 	
	vIO32WriteFldAlign(SCFIR_01, (bType!=3), CPURW_ACTIVE);
    vIO32WriteFldAlign(SCFIR_01, (bType==3), CPURW_ACTIVE_VPP);	
	vIO32WriteFldMulti(SCFIR_02, P_Fld(bType, COE_SEL)|
		  						 P_Fld(0x0, CPU_SRAM_ADDR)); 
    
	
	//writing sram coeff
	for (i = 0; i < SCPQ_PHASE_NUM; i++)
	{
		vIO32Write4B(SCFIR_05, dwCoeff[i*SCPQ_COEF_LEN+0]);
		vIO32Write4B(SCFIR_04, dwCoeff[i*SCPQ_COEF_LEN+1]);
		vIO32Write4B(SCFIR_03, dwCoeff[i*SCPQ_COEF_LEN+2]);
	}

	//finish writing sram coeff
	vIO32WriteFldAlign(SCFIR_01, 0, CPURW_ACTIVE);
	vIO32WriteFldAlign(SCFIR_01, 0, CPURW_ACTIVE_VPP);
}

void vDrvSCPQCoeffRead(UINT8 bType, UINT32 dwResult[SCPQ_COEF_TBLSIZE])
{
    UINT16 i;
    
	if(bType>=COEFTYP_MAX)
	{
        Printf("Scaler PQ Coeff Type: %d,  Wrong !!\n", bType);
	}
    
    //vDrvSCPQSramCoeffOnOff(SV_OFF);
    
	//initial setting before reading coefficient
    vIO32WriteFldMulti(SCFIR_00, P_Fld(0x1, RD_AUTO_INC_ADDR)|
		  						 P_Fld(0x1, WR_AUTO_INC_ADDR)|
    							 P_Fld(0x0, PIO_MODE)|
		  						 P_Fld(0x0, FORCE_WR_NO_WAIT)); 
	
	vIO32WriteFldAlign(SCFIR_01, (bType!=3), CPURW_ACTIVE);
	vIO32WriteFldAlign(SCFIR_01, (bType==3), CPURW_ACTIVE_VPP);
	vIO32WriteFldMulti(SCFIR_02, P_Fld(bType, COE_SEL)|
		  						 P_Fld(0x0, CPU_SRAM_ADDR)); 
    
    LOG(2, "Scaler PQ Coeff Type: %d !!\n", bType);
	//reading sram coeff
	for (i = 0; i < SCPQ_PHASE_NUM; i++)
	{
        dwResult[i*SCPQ_COEF_LEN+2]=IO32ReadFldAlign(SCFIR_06, RDATA_00);
        dwResult[i*SCPQ_COEF_LEN+1]=IO32ReadFldAlign(SCFIR_07, RDATA_01);
        dwResult[i*SCPQ_COEF_LEN+0]=IO32ReadFldAlign(SCFIR_08, RDATA_02);
		LOG(2, "0x%08x, 0x%08x, 0x%08x,\n", dwResult[i*SCPQ_COEF_LEN+0], dwResult[i*SCPQ_COEF_LEN+1], dwResult[i*SCPQ_COEF_LEN+2]);
	}
	
	//finish reading sram coeff
	vIO32WriteFldAlign(SCFIR_01, 0, CPURW_ACTIVE);
	vIO32WriteFldAlign(SCFIR_01, 0, CPURW_ACTIVE_VPP);

}


UINT8 bDrvSCPQCoeffValidate(UINT8 bType, UINT32 dwCoeffTblR[SCPQ_COEF_TBLSIZE], UINT32 dwCoeffTblW[SCPQ_COEF_TBLSIZE])
{
    UINT16 i, wIdx;
    UINT32* dwCoeffMsk;
    UINT8 bRet = SV_TRUE;

	if(bType>=COEFTYP_MAX)
	{
        Printf("Scaler PQ Coeff Type: %d,  Wrong !!\n", bType);
	}
    
    dwCoeffMsk=(bType<=COEFTYP_HOR_C)?dwHCOEFF_MSBVALID_MASK:dwVCOEFF_MSBVALID_MASK;
    
    for(i=0; i<SCPQ_PHASE_NUM; i++)
    {
        wIdx=i*SCPQ_COEF_LEN;
	    if (dwCoeffTblR[wIdx+0]!=(dwCoeffTblW[wIdx+0]&dwCoeffMsk[0])||
	        dwCoeffTblR[wIdx+1]!=(dwCoeffTblW[wIdx+1]&dwCoeffMsk[1])||
	        dwCoeffTblR[wIdx+2]!=(dwCoeffTblW[wIdx+2]&dwCoeffMsk[2]))
	    {
	    	Printf("Scaler PQ Coeff Type: %d,  Wrong !!\n", bType);
			Printf("[0] Read [%d]  Write [%d]\n", dwCoeffTblR[wIdx+0], dwCoeffTblW[wIdx+0]);
			Printf("[1] Read [%d]  Write [%d]\n", dwCoeffTblR[wIdx+1], dwCoeffTblW[wIdx+1]);
			Printf("[2] Read [%d]  Write [%d]\n", dwCoeffTblR[wIdx+2], dwCoeffTblW[wIdx+2]); 
            bRet = SV_FALSE;
	    }
    }
    
    return bRet;
}


//Set Coeff According to Scaling Ratio

UINT8 vDrvSCPQSetHCoeff(UINT8 bPath)
{
    UINT8 bHCoeffIdx, IsScalerInput444;
	UINT8 bCustUPCoeff = IO32ReadFldAlign(VDP_SCPQ_01, SCPQ_CUST_UPCOEFF);
	UINT32 dwScHIn = u4IO32Read4B(SCALER_COEF_MAIN_H_IN);
	UINT32 dwScHOut= u4IO32Read4B(SCALER_COEF_MAIN_H_OUT);
	
    static UINT8 bPreHCoeffIdx = 0xFF;
	static UINT8 bPreIsScalerInput444 = 0xFF;
	
    if(bPath!=SV_VP_MAIN)
        return SV_FALSE;

	/*	bHCoeffIdx
			0: up scaling
			1: down scaling 1.5
			2: down scaling 2
			3: down scaling 3
			4: down scaling 4
			5: down scaling 5
		*/ 
	bHCoeffIdx = (dwScHOut>=dwScHIn)?0: // upscaling only 1 hw default tbl
			  (dwScHOut*9<dwScHIn*2)?5: // DS ratio > 4.5 use 5
			  (dwScHOut*7<dwScHIn*2)?4: // DS ratio > 3.5 use 4
			  (dwScHOut*5<dwScHIn*2)?3: // DS ratio > 2.5 use 3
			  (dwScHOut*3<dwScHIn*2)?2: // DS ratio > 1.75 use 2
			  (1);						// DS ratio < 1.75 use 1.5
	IsScalerInput444 = bIsScalerInput444(bPath);
    
	//Use Sram coeff tbl
	if((!bCustUPCoeff))
	{ 
		if((bPreHCoeffIdx!=bHCoeffIdx)||(IsScalerInput444!=bPreIsScalerInput444))
		{
			bPreHCoeffIdx = bHCoeffIdx;
			bPreIsScalerInput444 = IsScalerInput444;
			
	        vDrvSCPQSetSramCoeff(COEFTYP_HOR_Y,u4SCHYCoeff[bHCoeffIdx]);
			if(!IsScalerInput444) 
			{
				// 422 mode Y/C needs different coeff
        		vDrvSCPQSetSramCoeff(COEFTYP_HOR_C,u4SCHCCoeff[bHCoeffIdx]);
			}
			else
			{
				vDrvSCPQSetSramCoeff(COEFTYP_HOR_C,u4SCHYCoeff[bHCoeffIdx]);
			}
		}
    }
	else
	{
		DRVCUST_HUPSCaleCoeff(bPath);
	}

	//Turn off dering when down-scaling
    vDrvDeRingHOnOff((!bHCoeffIdx));
    return SV_TRUE;
}

UINT8 vDrvSCPQSetVCoeff(UINT8 bPath)
{
    UINT8 bVCoeffIdx;
    UINT8 bLineInterleave;
	UINT32 dwScVIn = u4IO32Read4B(SCALER_COEF_MAIN_V_IN);
	UINT32 dwScVOut= u4IO32Read4B(SCALER_COEF_MAIN_V_OUT);	
    static UINT8 bPreVCoeffIdx = 0xFF;
	UINT8 bPingPongMode = IO32ReadFldAlign(SCPIP_SCCTRL1_04, SCCTRL1_04_SC_COEF_PP_ENABLE);
    UINT8 bPingPongWRTVC = IO32ReadFldAlign(SCPIP_SCCTRL1_05, SCCTRL1_05_SC_COEF_PP_RD_A);
    UINT32   u4SCL  = u4DrvTDTVScalerModeQuery();
    E_TD_OUT eSCLOut= TD_SCL_OUT(u4SCL);
    E_TD_IN  eSCLIn = TD_SCL_IN(u4SCL);
	UINT8 bCustUPCoeff = IO32ReadFldAlign(VDP_SCPQ_01, SCPQ_CUST_UPCOEFF);
	UINT8 bTTD_Coeff_Blur = ((dwScVIn>=720)&&
								(u4DrvTDTV3DModeQuery()==E_TDTV_UI_3D_MODE_TTD)&&
								IS_LVDS_DISP_3D_POLARIZED);
    if(bPath!=SV_VP_MAIN)
        return SV_FALSE;

	//scaler out line interleave or not
	bLineInterleave =((E_TD_OUT_3D_LI==eSCLOut)||
					 ((E_TD_IN_LI_P==eSCLIn)&&(E_TD_OUT_NATIVE==eSCLOut))); 

     /*  bVCoeffIdx
        0: up scaling
        1: down scaling 1.5
        2: down scaling 2
        3: down scaling 3
        4: down scaling 4
        5: down scaling 5
    */
    bVCoeffIdx = ((dwScVOut>=dwScVIn) && (!bLineInterleave) && (!bTTD_Coeff_Blur))?0: // upscaling hw default tbl
#ifndef CC_MT5398
	//Viper TTD needs differenct coeff to remove aliasing effect
    			 ((dwScVOut>=dwScVIn) && (!bLineInterleave))?2:
#endif    			 
    		  	  (dwScVOut>=dwScVIn)?6: // upscaling use 4 taps tbl
	              (dwScVOut*9<dwScVIn*2)?5: // DS ratio > 4.5 use 5
	              (dwScVOut*7<dwScVIn*2)?4: // DS ratio > 3.5 use 4
	              (dwScVOut*5<dwScVIn*2)?3: // DS ratio > 2.5 use 3
	              (dwScVOut*3<dwScVIn*2)?2: // DS ratio > 1.75 use 2
	              (1);                      // DS ratio < 1.75 use 1.5	
	//printf("Scaler Output Coeff Type %d %d %d\n", bVCoeffIdx, (dwScVIn>=720), (u4DrvTDTV3DModeQuery()==E_TDTV_UI_3D_MODE_TTD));	              
    //Use Sram coeff tbl
	if((!bCustUPCoeff))
	{       

        //Use Hw default coeff tbl          
        //vIO32WriteFldAlign(SCPIP_SCCTRL1_07, bVCoeffIdx, SCCTRL1_07_SC_V_COEF_TABLE_SEL);
        if(bPingPongMode)
        {
            //Ping-pong R/W VYC same tbl         
            vDrvSCPQSetSramCoeff(((bPingPongWRTVC)?COEFTYP_VER_C:COEFTYP_VER_Y), 
                                                          u4SCVYCCoeff[bVCoeffIdx]);
            vIO32WriteFldAlign(SCPIP_SCCTRL1_05, (!bPingPongWRTVC), SCCTRL1_05_SC_COEF_PP_RD_A);
        }
        else
        {
				if(bPreVCoeffIdx!=bVCoeffIdx)
				{
					bPreVCoeffIdx = bVCoeffIdx;
					vDrvSCPQSetSramCoeff(COEFTYP_VER_Y, u4SCVYCCoeff[bVCoeffIdx]);
					vDrvSCPQSetSramCoeff(COEFTYP_VER_C, u4SCVYCCoeff[bVCoeffIdx]);					
				}
		}
    }
	else
	{
		DRVCUST_VUPSCaleCoeff(bPath);
	}

    //Turn off dering when down-scaling
    vDrvDeRingVOnOff((!bVCoeffIdx));
    vDrvSCPQTapAdapOnOff((!bVCoeffIdx));
    return SV_TRUE;

}
/*****************************************************************************************/
/*************************************   Tap-adap   *******************************************/
/*****************************************************************************************/
void vDrvSCPQTapAdapInit(void)
{
    vDrvLoadRegTbl(REGTBL_Tap_Adap);
}
void vDrvSCPQTapAdapOnOff(UINT8 bOnOff)
{
    vIO32WriteFldMulti(SCPIP_SCCTRL1_0C,
            		  P_Fld(bOnOff, SCCTRL1_0C_SC_SS_V_EN)|
            		  P_Fld(bOnOff, SCCTRL1_0C_SC_SS_H_EN));

    //2D Tap-adap off , ES not work.
    //line-interleave needs off -> flow needs implented
#ifdef CC_MT5398     
    if (IC_VER_5398_AA == BSP_GetIcVersion())//5398A
    {    
	    vIO32WriteFldMulti(SCFIR_11, P_Fld(0, ADAPT_VERT_RATIO_0)|
	                                 P_Fld(0, ADAPT_VERT_RATIO_1));
    }
#endif

}

/*****************************************************************************************/
/*************************************   Dering   *******************************************/
/*****************************************************************************************/

void vDrvDeRingInit(void)
{
   	 vDrvLoadRegTbl(REGTBL_DE_RING);
}


void vDrvDeRingHOnOff(UINT8 bOnOff)
{
    UINT8 bModuleOnOff = bOnOff && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_DERING);        
    vIO32WriteFldAlign(SCPIP_SCCTRL1_0C, bModuleOnOff, SCCTRL1_0C_SC_DERING_H_EN);	

}

void vDrvDeRingVOnOff(UINT8 bOnOff)    
{
    UINT8 bModuleOnOff = bOnOff && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_DERING);        
    vIO32WriteFldAlign(SCPIP_SCCTRL1_0C, bModuleOnOff, SCCTRL1_0C_SC_DERING_V_EN);	
}

void vDrvDeRingOnOff(UINT8 bPath, UINT8 bOnOff)
{
    if (bPath == SV_VP_MAIN)
    {
        vDrvDeRingHOnOff(bOnOff);
        vDrvDeRingVOnOff(bOnOff);        
        vDrvSetImportWaitFlag(bPath, VRM_IMPORT_WAIT_FLAG_TRIG_START);
    }
}

/*****************************************************************************************/
/*****************************    PureEdgeInterpolation   ********************************/
/*****************************************************************************************/
#ifdef CC_MT5398

void vDrvUsPeiInit(void)
{
	vDrvLoadRegTbl(REGTBL_PEI);
}

void vDrvUsPeiOnOff(UINT8 bOnOff)
{
    VRM_INFO_T in,out;
    UINT8 bLineInterleave;

    UINT32   u4SCL  = u4DrvTDTVScalerModeQuery();
    E_TD_OUT eSCLOut= TD_SCL_OUT(u4SCL);
    E_TD_IN  eSCLIn = TD_SCL_IN(u4SCL);

    //scaler out line interleave or not
    bLineInterleave =((E_TD_OUT_3D_LI==eSCLOut)||
                     ((E_TD_IN_LI_P==eSCLIn)&&(E_TD_OUT_NATIVE==eSCLOut))); 

    //get fsc in & fsc out info
    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_FSC, &in, &out);

    //Turn off this function when  front scaler out is
    //a. line interleave
    //b. width > 1920
    //c. height > 1080
    bOnOff &= (!(out.u2Width > 1920 || 
                 out.u2Height> 1080 || 
                 in.u2Height > 2000 ||
                 bLineInterleave||
                 (out.u2Height<= in.u2Height) ) );  
    
    vIO32WriteFldAlign(SCPIP_SCCTRL1_0C, bOnOff, SCCTRL1_0C_SC_PE_ENABLE);
    vDrvSetImportWaitFlag(SV_VP_MAIN, VRM_IMPORT_WAIT_FLAG_TRIG_START);	 	
    printf("add debug log at %d %s\n",__LINE__,__FUNCTION__);
}
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------  POST UPSCALER -----------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
void vDrvPSCPQSramCoeffOnOff(UINT8 bOnOff)
{
    vIO32WriteFldAlign(PSCFIR_0C, bOnOff, PSC_H_SRAM_COEFF_EN);
    //Post scaler only H
    //vIO32WriteFldAlign(PSCFIR_0C, bOnOff, PSC_V_SRAM_COEFF_EN);
}

void vDrvPSCPQSetSramCoeff(UINT8 bType, UINT32 dwCoeff[SCPQ_COEF_TBLSIZE])
{

    UINT16 i;

	//initial setting before writing coefficient
    vIO32WriteFldMulti(PSCFIR_00, P_Fld(0x1, PSC_RD_AUTO_INC_ADDR)|
		  						  P_Fld(0x1, PSC_WR_AUTO_INC_ADDR)|
    							  P_Fld(0x0, PSC_PIO_MODE)|
		  						  P_Fld(0x0, PSC_FORCE_WR_NO_WAIT)|
		  						  P_Fld(0xFF, PSC_MAX_WAIT_CYCLE));  
	
	vIO32WriteFldAlign(PSCFIR_01, 1, PSC_CPURW_ACTIVE);
	vIO32WriteFldMulti(PSCFIR_02, P_Fld(bType, PSC_COE_SEL)|
		  						  P_Fld(0x0, PSC_CPU_SRAM_ADDR));
	
	//writing sram coeff
	for (i = 0; i < SCPQ_PHASE_NUM; i++)
	{
		vIO32WriteFldAlign(PSCFIR_05, dwCoeff[i*SCPQ_COEF_LEN+0], PSC_WDATA_02);
		vIO32WriteFldAlign(PSCFIR_04, dwCoeff[i*SCPQ_COEF_LEN+1], PSC_WDATA_01);
		vIO32WriteFldAlign(PSCFIR_03, dwCoeff[i*SCPQ_COEF_LEN+2], PSC_WDATA_00);
	}

	//finish writing sram coeff
	vIO32WriteFldAlign(PSCFIR_01, 0, PSC_CPURW_ACTIVE);

}

void vDrvPSCPQCoeffRead(UINT8 bType, UINT32 dwResult[SCPQ_COEF_TBLSIZE])
{
    UINT16 i;
    
	if(bType>=COEFTYP_MAX)
	{
        Printf("Post Scaler PQ Coeff Type: %d,  Wrong !!\n", bType);
	}
    
	//initial setting before reading coefficient
	vDrvPSCPQSramCoeffOnOff(SV_OFF);

    
	//initial setting before reading coefficient
    vIO32WriteFldMulti(PSCFIR_00, P_Fld(0x1, PSC_RD_AUTO_INC_ADDR)|
		  						  P_Fld(0x1, PSC_WR_AUTO_INC_ADDR)|
    							  P_Fld(0x0, PSC_PIO_MODE)|
		  						  P_Fld(0x0, PSC_FORCE_WR_NO_WAIT)); 
	
	vIO32WriteFldAlign(PSCFIR_01, 1, PSC_CPURW_ACTIVE);
	vIO32WriteFldMulti(PSCFIR_02, P_Fld(bType, PSC_COE_SEL)|
		  						  P_Fld(0x0, PSC_CPU_SRAM_ADDR));  
    
    LOG(2, "Post Scaler PQ Coeff Type: %d !!\n", bType);
	//reading sram coeff
	for (i = 0; i < SCPQ_PHASE_NUM; i++)
	{
        dwResult[i*SCPQ_COEF_LEN+2]=IO32ReadFldAlign(PSCFIR_06, PSC_RDATA_00);
        dwResult[i*SCPQ_COEF_LEN+1]=IO32ReadFldAlign(PSCFIR_07, PSC_RDATA_01);
        dwResult[i*SCPQ_COEF_LEN+0]=IO32ReadFldAlign(PSCFIR_08, PSC_RDATA_02);

		LOG(2, "0x%08x, 0x%08x, 0x%08x,\n", dwResult[i*SCPQ_COEF_LEN+0], dwResult[i*SCPQ_COEF_LEN+1], dwResult[i*SCPQ_COEF_LEN+2]);
	}
	
	//finish reading sram coeff
	vIO32WriteFldAlign(PSCFIR_01, 0, PSC_CPURW_ACTIVE);	    
}

UINT8 bDrvPSCPQCoeffValidate(UINT8 bType, UINT32 dwCoeffTblR[SCPQ_COEF_TBLSIZE], UINT32 dwCoeffTblW[SCPQ_COEF_TBLSIZE])
{
    UINT16 i, wIdx;
    UINT32* dwCoeffMsk;
    UINT8 bRet = SV_TRUE;
    
	if(bType>=COEFTYP_MAX)
	{
	    Printf("Post Scaler PQ Coeff Type: %d,  Wrong !!\n", bType);
	}
    
    // Post scaler Only H Coeff
    dwCoeffMsk=dwHCOEFF_MSBVALID_MASK;
    
    for(i=0; i<SCPQ_PHASE_NUM; i++)
    {
        wIdx=i*SCPQ_COEF_LEN;
	    if (dwCoeffTblR[wIdx+0]!=(dwCoeffTblW[wIdx+0]&dwCoeffMsk[0])||
	        dwCoeffTblR[wIdx+1]!=(dwCoeffTblW[wIdx+1]&dwCoeffMsk[1])||
	        dwCoeffTblR[wIdx+2]!=(dwCoeffTblW[wIdx+2]&dwCoeffMsk[2]))
	    {
	    	Printf("Scaler PQ Coeff Type: %d,  Wrong !!\n", bType);
			Printf("[0] Read [%d]  Write [%d]\n", dwCoeffTblR[wIdx+0], dwCoeffTblW[wIdx+0]);
			Printf("[1] Read [%d]  Write [%d]\n", dwCoeffTblR[wIdx+1], dwCoeffTblW[wIdx+1]);
			Printf("[2] Read [%d]  Write [%d]\n", dwCoeffTblR[wIdx+2], dwCoeffTblW[wIdx+2]); 
            bRet = SV_FALSE;
	    }
    }
    
    return bRet;
}

/*****************************************************************************************/
/*********************************   Post Dering   *******************************************/
/*****************************************************************************************/

void vDrvPostDeRingInit(void)
{
    //Post Scaler No DeringV
	vIO32WriteFldMulti(PSCFIR_0A,
		  P_Fld(0x01, PSC_DE_RING_EN_H_YUV)|
		  P_Fld(0x03, PSC_DE_RING_SEL_H_YUV)|
		  P_Fld(0x03, PSC_DE_RING_SLOPE_H_Y)|
		  P_Fld(0x10, PSC_DE_RING_MAX_THRESHOLD_H_Y)|
		  P_Fld(0x03, PSC_DE_RING_FIRST_SLOPE_H_Y)|
		  P_Fld(0x00, PSC_DE_RING_FIRST_THRESHOLD_H_Y));
    
	vIO32WriteFldMulti(PSCFIR_0B,
		  P_Fld(0x20, PSC_RING_THR_H_UV)|
		  P_Fld(0x06, PSC_SLOPE_SEL_H_UV)|
		  P_Fld(0x00, PSC_RING_1ST_THR_H_UV)|
		  P_Fld(0x03, PSC_SLOPE_1ST_SEL_H_UV));    
}

void vDrvPostDeRingOnOff(UINT8 bPath, UINT8 bOnOff)
{
    if (bPath == SV_VP_MAIN)
    {	
        if(IS_COBRA_E())
        {
            vPscSetResToggle(SV_OFF);
            vIO32WriteFldAlign(PSCFIR_0A, bOnOff, PSC_DE_RING_EN_H_YUV);
            vPscSetResToggle(SV_ON);
        }
        else
        {
            vIO32WriteFldAlign(PSCFIR_0A, bOnOff, PSC_DE_RING_EN_H_YUV);
            vDrvSetImportWaitFlag(bPath, VRM_IMPORT_WAIT_FLAG_TRIG_START);        
        }
    }
}
#endif

