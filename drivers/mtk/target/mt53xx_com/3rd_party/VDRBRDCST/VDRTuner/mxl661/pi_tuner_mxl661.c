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
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pi_tuner_TDVWH810F.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_tuner_TDVWH810F.c
 *  Tuner control for SS TDVWH810F
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "MxL661_TunerApi.h"
#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h" 
#include "i2c_api.h"
#include "x_typedef.h"  
#include "tunerDebug.h"   
#include "x_os.h"  
#include "MxL661_OEM_Drv.h"
#include "eq_script_mxl661.h"
#include "drvcust_if.h"
#include "x_gpio.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#if !(defined(CC_MT5365) || defined(CC_MT5395))
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
	{0x7b7, 0x10},//PRA target H
	{0x7b9, 0x0e},//PRA target L
	{0x790, 0x08},//DAGC setting
	{0x794, 0x2c},//DAGC Target level H
	{0x7a4, 0x2c},//DAGC Target level L
	{0x13b, 0x80},
	{0x1b4, 0x17},
	{0x782, 0xa0},
	{0x7b2, 0x03},
	{0x7b3, 0x60},
	{0x7b8, 0x00},
	{0x7d5, 0x00},//IF_BIAS1
	{0x7d6, 0x00},//IF_BIAS2
	{0x7d7, 0xec},//RF_SLOPE
	{0x7d8, 0xf0},//IF_SLOPE1
	{0x7d9, 0xf0},//IF_SLOPE2
	{0x7da, 0x7f},//RF Max
	{0x7db, 0x7f},//IF Max
	{0x7dc, 0x80},//RF Min
	{0x7dd, 0x80},//IF Min 1
	{0x7de, 0x80},//IF Min 2
	{0x7df, 0x00},
	{0x00,0x00}//end flag
	};
#else
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
	{0x2b7, 0x10},//PRA target H
	{0x2b9, 0x0e},//PRA target L
	{0x290, 0x08},//DAGC setting
	{0x294, 0x30},//DAGC Target level H
	{0x2a4, 0x30},//DAGC Target level L
	{0x13b, 0x80},
	{0x1b4, 0x17},
	{0x282, 0xa0},
	{0x2b2, 0x03},
	{0x2b3, 0x60},
	{0x2b8, 0x00},
	{0x2d5, 0x00},//IF_BIAS1
	{0x2d6, 0x00},//IF_BIAS2
	{0x2d7, 0xec},//RF_SLOPE
	{0x2d8, 0xf0},//IF_SLOPE1
	{0x2d9, 0xf0},//IF_SLOPE2
	{0x2da, 0x7f},//RF Max
	{0x2db, 0x7f},//IF Max
	{0x2dc, 0x80},//RF Min
	{0x2dd, 0x80},//IF Min 1
	{0x2de, 0x80},//IF Min 2
	{0x2df, 0x00},
	{0x00,0x00}//end flag
	};
#endif

/*************************************************
 DVB-T tuning parameter struct
**************************************************/
enum
   {
	   SAW_BW_6M = 0,
	   SAW_BW_7M,
	   SAW_BW_8M,
	   SAW_BW_5M
   };

//Frequency boundary .Get from tuner spec
#define C_MXL661_FREQ_DBOUND_UPPER         859*1000*1000     
#define C_MXL661_FREQ_DBOUND_LOWER         48*1000*1000 
#define C_MXL661_FREQ_DBOUND_UPPER_Ana     875*1000*1000
#define C_MXL661_FREQ_DBOUND_LOWER_Ana     45*1000*1000

//PLL lock check parameters
#define C_MXL_MXL661_PLL_POLL_INTERVAL     10 //ms
#define C_MXL_MXL661_PLL_POLL_TIMETOUT     100 //ms
#define C_MXL_MXL661_PLL_POLL_CNT          C_MXL_MXL661_PLL_POLL_TIMETOUT/C_MXL_MXL661_PLL_POLL_INTERVAL //counter

#define C_MXL661_LO_ADDRESS                ((UINT8)  0xC0)  // 7 bit address is 0x60 (DEC 96)
#define C_MXL661_IF_FREQUENCY              ((UINT16)  4500)  /* kHz */   //4.5MHz for DVB-T 
#define C_MXL661_IF_FREQUENCY_ANA          ((UINT16)  3650 )  /* kHz */   //3.65MHz for M/N 

#define C_MXL_MXL661_STR                "2013-11-07 MXL_MXL661 SW_v1 \n"

#define C_MXL661_AGC_IF_SLP             0.22 
#define C_MXL661_AGC_IF_INT             -0.15
#define C_MXL661_AGC_IF_MAX             0.7
#define C_MXL661_AGC_IF_MIN            -1

#define C_MXL661_RESET_TUNER           0

#define C_U8_MXL661_AGC_IF_SLP          (INT8)(C_MXL661_AGC_IF_SLP *64 )
#define C_U8_MXL661_AGC_IF_INT          (INT8)(C_MXL661_AGC_IF_INT *16 )
#define C_U8_MXL661_AGC_IF_MAX          (INT8)(C_MXL661_AGC_IF_MAX *128)
#define C_U8_MXL661_AGC_IF_MIN          (INT8)(C_MXL661_AGC_IF_MIN *128)

/******************************************************************************
  Below macro definitions are used to define IF path selection for different 
  work standard. 1 means select IF_PATH1, 2 means select MXL661_IF_PATH2   
*******************************************************************************/
#define MXL661_DVB_T_IF_PATH_SELECTION          1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_DVB_T2_IF_PATH_SELECTION         1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_DVB_C_IF_PATH_SELECTION          1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_J83B_IF_PATH_SELECTION           1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_ATSC_IF_PATH_SELECTION           1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_ISDB_T_IF_PATH_SELECTION         1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_NTSC_MN_IF_PATH_SELECTION        1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_PAL_BG_IF_PATH_SELECTION         1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_PAL_I_IF_PATH_SELECTION          1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_PAL_DK_IF_PATH_SELECTION         1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_SECAM_L_IF_PATH_SELECTION        1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
#define MXL661_SECAM_L1_IF_PATH_SELECTION       1 // 1: IF_PATH1, 2: MXL661_IF_PATH2 
//-----------------------------------------------------------------------------
// constant or global parameter definition
//-----------------------------------------------------------------------------
MXL_IF_AGC_DEFAULT_SET_T MxL661_ModeIfAgcSet[]={
// Work mode, bandwidth, IF path selection, IF_Freq, IF_Invert, gain_SelfAgc,gain_CloseAgc, AGC_Sel, AGC_Type, AGC_SetPoint   
  {MOD_DVBT,			   MXL661_DIG_TERR_BW_6MHz,	MXL661_DVB_T_IF_PATH_SELECTION,   MXL661_IF_5_38MHz, MXL_ENABLE,5,13,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},//add by xiongfeng to support DVBT 6M
  {MOD_DVBT,         MXL661_DIG_TERR_BW_7MHz,  MXL661_DVB_T_IF_PATH_SELECTION,   MXL661_IF_4_5MHz, MXL_ENABLE,5,13,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},
  {MOD_DVBT,         MXL661_DIG_TERR_BW_8MHz,  MXL661_DVB_T_IF_PATH_SELECTION,   MXL661_IF_5MHz,   MXL_ENABLE,5,13,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},
  {MOD_DVBT2,        MXL661_DIG_TERR_BW_7MHz,  MXL661_DVB_T2_IF_PATH_SELECTION,  MXL661_IF_4_5MHz, MXL_ENABLE,11,11,MXL661_AGC2,MXL661_AGC_EXTERNAL, 66},
  {MOD_DVBT2,        MXL661_DIG_TERR_BW_8MHz,  MXL661_DVB_T2_IF_PATH_SELECTION,  MXL661_IF_5MHz,   MXL_ENABLE,11,11,MXL661_AGC2,MXL661_AGC_EXTERNAL, 66},  
  {MOD_DVBC,         MXL661_ANA_TV_DIG_CABLE_BW_8MHz,  MXL661_DVB_C_IF_PATH_SELECTION,   MXL661_IF_5MHz,   MXL_ENABLE, 3,11,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},  
  {MOD_DTMB,         MXL661_DIG_TERR_BW_8MHz,  MXL661_DVB_T_IF_PATH_SELECTION,   MXL661_IF_5MHz,   MXL_ENABLE,11,11,MXL661_AGC2,MXL661_AGC_EXTERNAL, 66},
  {MOD_J83B_64QAM,   MXL661_ANA_TV_DIG_CABLE_BW_6MHz,  MXL661_J83B_IF_PATH_SELECTION,    MXL661_IF_5_38MHz,MXL_ENABLE, 5,11,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},  
  {MOD_J83B_256QAM,  MXL661_ANA_TV_DIG_CABLE_BW_6MHz,  MXL661_J83B_IF_PATH_SELECTION,    MXL661_IF_5_38MHz,MXL_ENABLE, 5,11,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},  
  {MOD_J83B_AUTO_QAM,MXL661_ANA_TV_DIG_CABLE_BW_6MHz,  MXL661_J83B_IF_PATH_SELECTION,    MXL661_IF_5_38MHz,MXL_ENABLE, 5,11,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66}, 
  {MOD_ATSC_8VSB,    MXL661_DIG_TERR_BW_6MHz,  MXL661_ATSC_IF_PATH_SELECTION,    MXL661_IF_5_38MHz,MXL_ENABLE, 8,11,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},   
  {MOD_ISDBT,        MXL661_DIG_TERR_BW_6MHz,  MXL661_ISDB_T_IF_PATH_SELECTION,  MXL661_IF_5MHz,   MXL_ENABLE,11,11,MXL661_AGC1,MXL661_AGC_EXTERNAL, 66},
  {MOD_NTSC_M,MXL661_ANA_TV_DIG_CABLE_BW_6MHz, MXL661_NTSC_MN_IF_PATH_SELECTION, MXL661_IF_3_65MHz,MXL_ENABLE, 9,11,MXL661_AGC1,MXL661_AGC_SELF, 66},
  {MOD_PAL_BG,MXL661_ANA_TV_DIG_CABLE_BW_7MHz, MXL661_PAL_BG_IF_PATH_SELECTION,  MXL661_IF_4_15MHz,MXL_ENABLE, 9,11,MXL661_AGC1,MXL661_AGC_SELF, 66},
  {MOD_PAL_BG,MXL661_ANA_TV_DIG_CABLE_BW_8MHz, MXL661_PAL_BG_IF_PATH_SELECTION,  MXL661_IF_4MHz,   MXL_ENABLE, 9,11,MXL661_AGC1,MXL661_AGC_SELF, 66},
  {MOD_PAL_I,MXL661_ANA_TV_DIG_CABLE_BW_8MHz,  MXL661_PAL_I_IF_PATH_SELECTION,   MXL661_IF_4_5MHz, MXL_ENABLE, 9,11,MXL661_AGC1,MXL661_AGC_SELF, 66},
  {MOD_PAL_DK,MXL661_ANA_TV_DIG_CABLE_BW_8MHz, MXL661_PAL_DK_IF_PATH_SELECTION,  MXL661_IF_4_1MHz, MXL_ENABLE, 9,11,MXL661_AGC1,MXL661_AGC_SELF, 66},
  {MOD_SECAM_L,MXL661_ANA_TV_DIG_CABLE_BW_8MHz,MXL661_SECAM_L_IF_PATH_SELECTION, MXL661_IF_4MHz,   MXL_ENABLE, 2,11,MXL661_AGC1,MXL661_AGC_SELF, 64},
  {MOD_SECAM_L1,MXL661_ANA_TV_DIG_CABLE_BW_8MHz,MXL661_SECAM_L1_IF_PATH_SELECTION,MXL661_IF_4MHz,  MXL_ENABLE, 2,11,MXL661_AGC1,MXL661_AGC_SELF, 64},
  {0xFF,MXL661_ANA_TV_DIG_CABLE_BW_8MHz,MXL661_SECAM_L1_IF_PATH_SELECTION,MXL661_IF_4MHz, MXL_ENABLE,11,11,MXL661_AGC2, MXL661_AGC_SELF, 64}
  };
static UINT8* pMXL661EqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pMXL661EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8 fgTunerInit=FALSE;
BOOL MXL661_fgEnterAutoSearch=TRUE;//TRUE :fist time for ENTER AUTO-SEARCH

//----------------------------------------------------------------------------- 
/**  MXL661_ResetTuner
 *    reset tuner GPIO get high  
 */
//-----------------------------------------------------------------------------
#if C_MXL661_RESET_TUNER
static BOOL MXL661_ResetTuner(ITUNER_CTX_T *pTCtx)
{
	UINT32 u4RstPinNum=204;
       
    if (DRVCUST_OptQuery(eTunerResetGpio, &u4RstPinNum) == 0)
	   {
		GPIO_SetOut(u4RstPinNum, 0);
	    x_thread_delay((UINT32)100);
		GPIO_SetOut(u4RstPinNum, 1);
		x_thread_delay((UINT32)20); 
		mcDBG_MSG_L2(("Reset Tuner GPIO=0x%x\n",u4RstPinNum));
	    return true;
	        }
	else
		{
		mcDBG_MSG_ERR(("Reset Tuner fail"));  
		return false;
		}        
}
#endif

//-------------------------------------------------------------static  variable----------------------
//Signal Power Sample Array Size
//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------
//-------------------------------------------------------------static  variable----------------------------------------------------------------------------------------------------------------------

/** MXL661_TunerInit
 *  Tuner initialzation for MXL661.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
VOID MXL661_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{
	    	 
	    
    ITUNER_CTX_T *psTunerCtx = pTCtx;
    SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
	SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx); 

    fgTunerInit = FALSE;
    
    psTunerCtx->I2cAddress = C_MXL661_LO_ADDRESS;
    psTunerCtx->u2IF_Freq = C_MXL661_IF_FREQUENCY;
	  psTunerCtx->u2IF_Freq_A = C_MXL661_IF_FREQUENCY_ANA;//analog if frequency for ATD
    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x09;//MASK_PATCH_CR|MASK_PATCH_SENS
    psTunerCtx->fgRFAGC = FALSE; //indicate if tuner need extern RF_AGC
	psTunerCtx->fgRFTuner = FALSE;//legacy tuner
	
	#if defined(CC_MT5365)||defined(CC_MT5395)
	#if defined(DTD_INCLUDE)  // For MT5365/95 must define DTD_INCLUDE for US DTV
	//AD Target set ,for 5365/95+5112ee
    pUSCtx->m_aucPara[TUNER_FUNC_EN_TARGET_LEVEL] = TRUE;
	pUSCtx->m_aucPara[TUNER_PARA_TARGET_LEVEL_MSB] = 0x23;   
    pUSCtx->m_aucPara[TUNER_PARA_TARGET_LEVEL_LSB] = 0x00;
	#endif
	#else    
	pUSCtx->m_aucPara_SLD[2] = (UINT8)C_U8_MXL661_AGC_IF_SLP; 
    pUSCtx->m_aucPara_SLD[3] = (UINT8)C_U8_MXL661_AGC_IF_INT;
    pUSCtx->m_aucPara_SLD[4] = (UINT8)C_U8_MXL661_AGC_IF_MAX;
    pUSCtx->m_aucPara_SLD[5] = (UINT8)C_U8_MXL661_AGC_IF_MIN;
	#endif
	
	pEUCtx->m_aucPara[2]= (UINT8)C_U8_MXL661_AGC_IF_SLP; 
	pEUCtx->m_aucPara[3]= (UINT8)C_U8_MXL661_AGC_IF_INT;
    pEUCtx->m_aucPara[4]= (UINT8)C_U8_MXL661_AGC_IF_MAX;
	pEUCtx->m_aucPara[5]= (UINT8)C_U8_MXL661_AGC_IF_MIN;
	  //Tuner frequency range
    pEUCtx->m_s4FreqBoundUpper = C_MXL661_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_MXL661_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_MXL661_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_MXL661_FREQ_DBOUND_LOWER_Ana;
    
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
	#if defined(CC_MT5880)//for descrambler
	psTunerCtx->sTunerDescramblePara.u2CVBSDelay=0x0255;
	#endif
#endif	
    
    pMXL661EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQMXL661MN;
	pMXL661EqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQMXL661BG;
	pMXL661EqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQMXL661DK;
	pMXL661EqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQMXL661I;
	pMXL661EqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQMXL661L;
	pMXL661EqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQMXL661L1;

    pMXL661EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQMXL661WeakMN;
	pMXL661EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQMXL661WeakBG;
	pMXL661EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQMXL661WeakDK;
	pMXL661EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQMXL661WeakI;
	pMXL661EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQMXL661WeakL;
    pMXL661EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQMXL661WeakL1;

    #if C_MXL661_RESET_TUNER       //add by xiongfeng to reset tuner
	
	if(MXL661_ResetTuner(pTCtx))
	{
	   mcDBG_MSG_L2(("Reset Tuner Success\n"));
	}
	else
	{
	   mcDBG_MSG_L2(("Reset Tuner fail\n"));
	}
	#endif
}

static VOID MXL661_DriverInit(ITUNER_CTX_T *pTCtx)
{	  
    MXL661_COMMAND_T apiCmd;
    MXL_STATUS status; 
	  
    // These parameters below shall be passed into by input parameters 
    MXL661_IF_FREQ_E ifOutFreq;
    MXL_BOOL ifSpecInvert;
    MXL661_IF_PATH_E ifPath; 
	  
    MXL661_AGC_ID_E agcSel; 
    MXL661_AGC_TYPE_E agcType;	
    // Here we need reset tuner, set Xtal, over write default, set IF out etc.  
    // Soft Reset MxL661
  	 MXL661_fgEnterAutoSearch=TRUE;//SET FALG AS TRUE FOR NEXT CHANNEL SCAN
    apiCmd.commandId = MXL661_DEV_SOFT_RESET_CFG;
    apiCmd.MxLIf.cmdResetCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS; 
    mcDBG_MSG_INFO(("begin MXL661_TunerInit \n"));
    status = MxLWare661_API_ConfigDevice(&apiCmd);
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_ERR(("Error! MXL_DEV_SOFT_RESET_CFG\n"));
    }
	 
    // Overwrite Default
    apiCmd.commandId = MXL661_DEV_OVERWRITE_DEFAULT_CFG;
    apiCmd.MxLIf.cmdOverwriteDefault.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
    apiCmd.MxLIf.cmdOverwriteDefault.SingleSupply_3_3V = MXL_ENABLE;
    status |= MxLWare661_API_ConfigDevice(&apiCmd);
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_ERR(("Error! MXL_DEV_OVERWRITE_DEFAULT_CFG\n"));
    }
	 
    // Xtal Setting
    apiCmd.commandId = MXL661_DEV_XTAL_SET_CFG;
    apiCmd.MxLIf.cmdXtalCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
    apiCmd.MxLIf.cmdXtalCfg.XtalFreqSel = MXL661_XTAL_16MHz;
    apiCmd.MxLIf.cmdXtalCfg.XtalCap = 25;
    apiCmd.MxLIf.cmdXtalCfg.ClkOutEnable = MXL_DISABLE;
    apiCmd.MxLIf.cmdXtalCfg.ClkOutDiv = MXL_DISABLE;
    apiCmd.MxLIf.cmdXtalCfg.SingleSupply_3_3V = MXL_ENABLE;
    apiCmd.MxLIf.cmdXtalCfg.XtalSharingMode = MXL_DISABLE;
    status |= MxLWare661_API_ConfigDevice(&apiCmd); 
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_ERR(("Error! MXL_DEV_XTAL_SET_CFG\n"));
    }
	 
    // Here we need configure IF out and AGC according to signal mode 
    // But how to pass signal mode parameters into this routine? 
	 
    // Set IF out configuration 
    //Here we define some default parameters for DVB-C mode
    ifOutFreq = MXL661_IF_5MHz;
    ifSpecInvert = MXL_ENABLE;
    ifPath = MXL661_IF_PATH2; 
	   
    apiCmd.commandId = MXL661_DEV_IF_OUT_CFG;
    apiCmd.MxLIf.cmdIfOutCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
    apiCmd.MxLIf.cmdIfOutCfg.IFOutFreq = ifOutFreq;
    apiCmd.MxLIf.cmdIfOutCfg.IFInversion = ifSpecInvert;
    apiCmd.MxLIf.cmdIfOutCfg.GainLevel = 11;
    apiCmd.MxLIf.cmdIfOutCfg.PathSel = ifPath;
    apiCmd.MxLIf.cmdIfOutCfg.ManualFreqSet = MXL_DISABLE;	  
    status |= MxLWare661_API_ConfigDevice(&apiCmd); 
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_ERR(("Error! MXL661_DEV_IF_OUT_CFG\n"));
    }
	 
    // Set AGC configuration 
	//Here we define some default parameters for DVB-C mode
    agcSel = MXL661_AGC2; 
    agcType = MXL661_AGC_EXTERNAL;
	 
    apiCmd.commandId = MXL661_TUNER_AGC_CFG;
    apiCmd.MxLIf.cmdAgcSetCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
    apiCmd.MxLIf.cmdAgcSetCfg.AgcSel = agcSel;
    apiCmd.MxLIf.cmdAgcSetCfg.AgcType = agcType;
    apiCmd.MxLIf.cmdAgcSetCfg.SetPoint = 66;
    apiCmd.MxLIf.cmdAgcSetCfg.AgcPolarityInverstion = MXL_DISABLE;  
    status |= MxLWare661_API_ConfigTuner(&apiCmd);
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_ERR(("Error! MXL661_TUNER_AGC_CFG\n"));
    }
    // Set DVB-C application as default mode
    apiCmd.commandId = MXL661_TUNER_MODE_CFG;
    apiCmd.MxLIf.cmdModeCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
    apiCmd.MxLIf.cmdModeCfg.SignalMode = MXL661_DIG_DVB_C;
    apiCmd.MxLIf.cmdModeCfg.IFOutFreqinKHz = 5000;  // 5MHz
    apiCmd.MxLIf.cmdModeCfg.XtalFreqSel = MXL661_XTAL_16MHz;
    apiCmd.MxLIf.cmdModeCfg.IFOutGainLevel = 11;
    status = MxLWare661_API_ConfigTuner(&apiCmd);
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_L2(("Error! MXL661_TUNER_MODE_CFG\n"));
    }
	 // Disable Tuner AFC function
	 apiCmd.commandId = MXL661_TUNER_AFC_CFG;
	 apiCmd.MxLIf.cmdAfcCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
	 apiCmd.MxLIf.cmdAfcCfg.AfcRangeInKHz = MXL661_AFC_DISABLE;
	 status = MxLWare661_API_ConfigTuner(&apiCmd);
	 if (MXL_FAILED == status)
	 {
	   mcDBG_MSG_L2(("Error! MXL_TUNER_AFC_CFG\n"));
	 }
    // Last step is power up Tuner 
    apiCmd.commandId = MXL661_TUNER_POWER_UP_CFG;
    apiCmd.MxLIf.cmdTunerPoweUpCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
    apiCmd.MxLIf.cmdTunerPoweUpCfg.Enable = MXL_ENABLE;
    status |= MxLWare661_API_ConfigTuner(&apiCmd);
    if (MXL_FAILED == status)
    {
        mcDBG_MSG_ERR(("Error! MXL_TUNER_POWER_UP_CFG\n"));
    }
    if (MXL_SUCCESS == status){
        mcDBG_MSG_INFO(("Init " C_MXL_MXL661_STR "(Cust tuner)\n"));
    }
}

//----------------------------------------------------------------------------- 
/**  TDVWH810F_TunerGetStatus
 *  Set Tuner PLL staus forTDVWH810F/TDVWH810F
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval TRUE        PLL lock
 *  @retval FALSE       PLL unlock
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_TunerGetStatus(ITUNER_CTX_T *pTCtx)
{
	  MXL661_COMMAND_T apiCmd;
  
    // Read back Tuner lock status
    apiCmd.commandId = MXL661_TUNER_LOCK_STATUS_REQ;
    apiCmd.MxLIf.cmdTunerLockReq.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
  
    if (MXL_TRUE == MxLWare661_API_GetTunerStatus(&apiCmd))
    {
      if (MXL_LOCKED == apiCmd.MxLIf.cmdTunerLockReq.RfSynLock &&
          MXL_LOCKED == apiCmd.MxLIf.cmdTunerLockReq.RefSynLock)
      {
        mcDBG_MSG_L2(("Tuner is locked\n"));
        return TRUE;
      }
      else
      {
        mcDBG_MSG_L1(("Tuner is not lock!\n"));
        return FALSE;
      }
    }   
    
    return FALSE;
}

//----------------------------------------------------------------------------- 
/** MXL661_TVModeTranslate
 *  Translate MTK defined TV mode parameter into Maxlinear signal mode parametr 
 *  @param  Mode: MTK defined Tuner work mode.
 *  @retval (1) SignalMode:  Maxlinear defined TV mode 
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_TVModeTranslate(UINT8 Mode, UINT8 *MxLSignalMode)
{
	 BOOL retResult = TRUE ;
	 MXL661_SIGNAL_MODE_E signalMode = MXL661_ANA_NTSC_MODE; 	 
   	 	 
	 switch ((MOD_TYPE_T)Mode)
	 {
	 	 case MOD_ISDBT: 	 	
	 	 case MOD_ATSC_8VSB:     
		   signalMode = MXL661_DIG_ISDBT_ATSC; 
		   mcDBG_MSG_L2(("ATSC\n"));
		   break; 
	 	 	
	 	 case MOD_J83B_64QAM:    
	 	 case MOD_J83B_256QAM:   
	 	 case MOD_J83B_AUTO_QAM: 
		   signalMode = MXL661_DIG_J83B; 
		   mcDBG_MSG_L2(("J83B QAM\n"));
		   break; 
	 	 	
	 	 case MOD_DVBT:           
	 	 case MOD_DVBT2: 	       
		   signalMode = MXL661_DIG_DVB_T_DTMB; 
		   mcDBG_MSG_L2(("DVBT\n"));  		
		   break; 	
	 	 	
	 	 case MOD_DVBC:         
   		   signalMode = MXL661_DIG_DVB_C; 
		   mcDBG_MSG_L2(("DVBC\n"));
		   break; 	

	 	 case MOD_PAL_BG:   signalMode = MXL661_ANA_PAL_BG;   break;
  	     case MOD_PAL_DK:   signalMode = MXL661_ANA_PAL_D;    break; 
		 case MOD_PAL_I:    signalMode = MXL661_ANA_PAL_I;    break; 	 	 
		 case MOD_SECAM_L:  signalMode = MXL661_ANA_SECAM_I;  break; 	 	
	 	 case MOD_SECAM_L1: signalMode = MXL661_ANA_SECAM_L;  break; 
		 case MOD_NTSC_M:   signalMode = MXL661_ANA_NTSC_MODE; break; 
	 	 		 	 	
	 	 case MOD_DTMB:  	 	 	
			signalMode = MXL661_DIG_DVB_T_DTMB; 
  			mcDBG_MSG_L2(("DTMB mode\n"));
			break;
	 	 default: retResult = FALSE; break; 
	 }
	 
	 *MxLSignalMode = (UINT8)signalMode;
	 return (retResult);
}

//----------------------------------------------------------------------------- 
/** MXL661_GetBandWidth
 *  Get band width (Maxlinear definition) according to input signal mode (Maxlinear definition) 
 *  @param  MxLSignalMode:  tuner work mode in Maxlinear definition
 *          SAW_BW: saw band width 
 *  @retval (1) BandWidth: signal RF band width in Maxlinear definition
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_GetBandWidth(UINT8 MxLSignalMode, UINT8 SAW_BW, UINT32 FreqInHz, UINT8 *BandWidth)
{
  BOOL retResult = TRUE;
  MXL661_BW_E bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz; 
  
  switch ((MXL661_SIGNAL_MODE_E)MxLSignalMode)
  {
    case MXL661_ANA_NTSC_MODE:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_6MHz;   
	  break; 

    case MXL661_ANA_PAL_BG:
      if(FreqInHz < 300000000)
	    bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_7MHz;  // PAL_B
      else 
	    bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz;  // PAL_GH            
      break; 

    case MXL661_ANA_PAL_I:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz;
      break; 

    case MXL661_ANA_PAL_D:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz;
      break; 

    case MXL661_ANA_SECAM_I:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz;
      break; 

    case MXL661_ANA_SECAM_L:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz;
      break; 

    case MXL661_DIG_DVB_T_DTMB:
	  bandWidth = MXL661_DIG_TERR_BW_7MHz;
	  if(SAW_BW == SAW_BW_8M){
     	 bandWidth = MXL661_DIG_TERR_BW_8MHz;
		 mcDBG_MSG_L1(("MXL661_DIG_DVB_T_DTMB 8 MHZ\n"));
	  }
	  else if(SAW_BW == SAW_BW_7M){
		 bandWidth = MXL661_DIG_TERR_BW_7MHz;
		 mcDBG_MSG_L1(("MXL661_DIG_DVB_T_DTMB 7 MHZ\n"));

	  }
	  else if(SAW_BW == SAW_BW_6M){
     	 bandWidth = MXL661_DIG_TERR_BW_6MHz;
		 mcDBG_MSG_L1(("DVBT 6 MHZ\n"));
	  }
      break;

    case MXL661_DIG_DVB_C:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_8MHz;
      break;

    case MXL661_DIG_ISDBT_ATSC:
      bandWidth = MXL661_DIG_TERR_BW_6MHz;
	  mcDBG_MSG_L1(("set MXL661_DIG_ISDBT_ATSC bandWidth=%d\n",bandWidth));
      break;

    case MXL661_DIG_J83B:
      bandWidth = MXL661_ANA_TV_DIG_CABLE_BW_6MHz;       	
      break; 
     	     	 
    default: retResult = FALSE; break;
   }  
 
   *BandWidth = (UINT8)bandWidth;
   return(retResult);
}

//----------------------------------------------------------------------------- 
/** MXL661_GetTuneFreq
 *  Get tune frequency according to signal mode and input frequency. 
 *   For analog TV mode, input is picture freq, return value is the center frequency.
 *   For digital mode, return value is the input RF frequency point.  
 *  @param  MxLSignalMode:  tuner work mode in Maxlinear definition
 *          FreqInHz: RF center or picture frequency in Hz unit  
 *  @retval (1) TuneFreqInHz: center frequency in Hz unit 
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_GetTuneFreq(UINT8 MxLSignalMode, UINT32 FreqInHz, UINT32 *TuneFreqInHz)
{
  BOOL retResult = TRUE;
  UINT32 programFreqHz = FreqInHz; 
  
  switch ((MXL661_SIGNAL_MODE_E)MxLSignalMode)
  {
     case MXL661_ANA_NTSC_MODE:
       programFreqHz =  FreqInHz + PIC_FREQ_DIFF_ANA_M_N * 1000;
       break;

     case MXL661_ANA_PAL_BG:
       if(FreqInHz < 300000000) // PAL_B
	     programFreqHz =  FreqInHz + PIC_FREQ_DIFF_ANA_B * 1000;
       else // PAL_GH
         programFreqHz =  FreqInHz + PIC_FREQ_DIFF_ANA_GH * 1000;
       break;

     case MXL661_ANA_PAL_I:
       programFreqHz =  FreqInHz + PIC_FREQ_DIFF_ANA_I * 1000;
       break;

     case MXL661_ANA_PAL_D:
       programFreqHz =  FreqInHz + PIC_FREQ_DIFF_ANA_D_K_L * 1000;
       break;

     case MXL661_ANA_SECAM_I:
       programFreqHz =  FreqInHz + PIC_FREQ_DIFF_SECAM * 1000;
       break;

     case MXL661_ANA_SECAM_L:
       programFreqHz =  FreqInHz - PIC_FREQ_DIFF_SECAM2 * 1000;
       break;

     case MXL661_DIG_DVB_T_DTMB:
	   mcDBG_MSG_L2(("MXL661_DIG_DVB_T_DTMB\n"));
       break; 
     	 
     case MXL661_DIG_DVB_C: 
       mcDBG_MSG_L2(("MXL661_DIG_DVB_C\n"));
       break; 
     	 
     case MXL661_DIG_ISDBT_ATSC: 
       mcDBG_MSG_L2(("MXL661_DIG_ISDBT_ATSC\n"));
       break; 
     	 
     case MXL661_DIG_J83B:
       mcDBG_MSG_L2(("MXL661_DIG_J83B\n"));
       break; 
     	     	 
    default: retResult = FALSE; break;
   }  
 
   *TuneFreqInHz = programFreqHz;
   return(retResult);
}

//----------------------------------------------------------------------------- 
/** MXL661_GetIfFreqInKHz
 *  Get IF frequency in KHz unit according to IF out freq selection (maxlinear 
 *  definition and band width.
 *  @param  MxLSignalMode:  tuner work mode in Maxlinear definition
 *          MxLIfFreqSel: IF out Freq selection in Maxlinear definition
 *          MxLBandWidth: band width in Maxlinear definition.   
 *  @retval (1) IfFreqInKHz: IF out frequency in KHz unit 
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_GetIfFreqInKHz(UINT8 MxLSignalMode, UINT8 MxLIfFreqSel, UINT8 MxLBandWidth, UINT32 *MxLIfFreqInKHz)
{
  BOOL retResult = TRUE;
  UINT32 ifFreqInKHz=MXL_ENABLE; 
  UINT32 ifAbsFreqInKHz[] = {3650, 4000, 4100, 4150, 4500, 4570, 5000, 5380, 6000, 6280, 7200, 8250, 35250, 36000, 36150, 36650, 44000}; 
  
   if (MxLIfFreqSel < 17) 
   {
     switch ((MXL661_SIGNAL_MODE_E)MxLSignalMode)
     {
       case MXL661_ANA_NTSC_MODE:
      	 ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] + PIC_FREQ_DIFF_ANA_M_N;
	     break; 

       case MXL661_ANA_PAL_BG:
         if ((MXL661_BW_E)MxLBandWidth == MXL661_ANA_TV_DIG_CABLE_BW_7MHz)  // PAL_B
           ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] + PIC_FREQ_DIFF_ANA_B;        
         else if ((MXL661_BW_E)MxLBandWidth == MXL661_ANA_TV_DIG_CABLE_BW_8MHz)  // PAL_GH
           ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] + PIC_FREQ_DIFF_ANA_GH;        
         break; 

       case MXL661_ANA_PAL_I:
         ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] + PIC_FREQ_DIFF_ANA_I;
         break; 

       case MXL661_ANA_PAL_D:
         ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] + PIC_FREQ_DIFF_ANA_D_K_L;
         break; 

       case MXL661_ANA_SECAM_I:
         ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] + PIC_FREQ_DIFF_SECAM;
         break; 

       case MXL661_ANA_SECAM_L:
         ifFreqInKHz  =  ifAbsFreqInKHz[MxLIfFreqSel] - PIC_FREQ_DIFF_SECAM2;
         break; 
       
       case MXL661_DIG_DVB_T_DTMB:
       case MXL661_DIG_DVB_C:
       case MXL661_DIG_ISDBT_ATSC:   
       case MXL661_DIG_J83B:     	   	
     	   ifFreqInKHz = ifAbsFreqInKHz[MxLIfFreqSel];
     	   break;      	 
 
       default: ifFreqInKHz = ifAbsFreqInKHz[MxLIfFreqSel]; break; 
      }
     mcDBG_MSG_L2(("IF out frequency = %d KHz \n",ifAbsFreqInKHz[MxLIfFreqSel])); 	  
    }
 
   *MxLIfFreqInKHz = ifFreqInKHz;
   return(retResult);
}

//----------------------------------------------------------------------------- 
/** MXL661_GetIfOutAgcDefaultSet
 *  Get IF Out parameters default setting values, such as IF Freq, IF spectrum invert and IF Gain.
 *  @param  mtklMode: Tuner work mode in MTK definition.
 *          mxLBandWidth: RF band width in maxlinear definition
 *  @retval (1) IfFreq:  IF Freq
 *          (2) IfSpecInvert:  IF spectrum invert
 *          (3) IfGain:  IF Gain
 *          (4) AgcType: AGC type, close loop or self AGC  
 *          (5) AgcSel: MXL661_AGC1 or MXL661_AGC2 
 *          (6) AgcSetPoint: AGC set point 
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_GetIfOutAgcDefaultSet(UINT8 mtkMode, UINT8 mxLBandWidth, UINT8 *IfFreq, UINT8 *IfSpecInvert, UINT8 *IfPath, UINT8 *IfGain, UINT8 *AgcType, UINT8 *AgcSel, UINT8 *AgcSetPoint)
{
	 BOOL retResult = FALSE;
	 //MXL661_SIGNAL_MODE_E signalMode = MXL661_ANA_NTSC_MODE; 	 
	 MXL661_IF_FREQ_E ifOutFreq = MXL661_IF_5MHz;
	 MXL_BOOL ifSpecInvert = MXL_DISABLE;
	 MXL661_IF_PATH_E ifPath = MXL661_IF_PATH2;
	 UINT8 ifGainLevel = 11;
	 MXL661_AGC_TYPE_E agcType;
     MXL661_AGC_ID_E agcSel;
     UINT8 agcSetPoint;	 
	 MXL661_BW_E bandWidth; 	 
     UINT16 ii;
   	 	
	 //signalMode = (MXL661_SIGNAL_MODE_E)mxLSignalMode;
     bandWidth = (MXL661_BW_E)mxLBandWidth;	 
	
     ii = 0;
	 mcDBG_MSG_L1(("mtkMode=%d,mxLBandWidth=%d\n",mtkMode,mxLBandWidth));
     while(1)
     {
   	   if (MxL661_ModeIfAgcSet[ii].SignalMode == 0xFF)
   	   {
   	 	  mcDBG_MSG_L2(("Not find IF out and AGC parameters in MxL661_ModeIfAgcSet table !\n"));	
   	 	  break;  // Searched to end of the MxL661_ModeIfAgcSet table 
   	   }

   	   if ((UINT8)mtkMode == MxL661_ModeIfAgcSet[ii].SignalMode)
   	   {mcDBG_MSG_L1(("find SignalMode,bandwidth=%d,index=%d\n",bandWidth,ii));
   	 	 // Check the second parameter bandwidth 
   	 	 if (bandWidth == MxL661_ModeIfAgcSet[ii].BandWidth)
   	 	 {mcDBG_MSG_L1(("find bandwidth\n"));
   	 	 	 // Get AGC type related parameters 
   	 	 	 agcType = MxL661_ModeIfAgcSet[ii].AgcType;
			 agcSel = MxL661_ModeIfAgcSet[ii].AgcSel;
			 agcSetPoint = MxL661_ModeIfAgcSet[ii].AgcSetPoint;
   	 	 	 // Get IF out related parameters 
   	 	 	 ifPath = (MXL661_IF_PATH_E)MxL661_ModeIfAgcSet[ii].IfPathSelection;
   	 	 	 ifOutFreq = MxL661_ModeIfAgcSet[ii].IFOutFreq;
   	 	 	 ifSpecInvert = MxL661_ModeIfAgcSet[ii].IFInversion;
   	 	 	 if (agcType == MXL661_AGC_EXTERNAL)
   	 	 	   ifGainLevel = MxL661_ModeIfAgcSet[ii].IfGainCloseAgc;
   	 	 	 else if (agcType == MXL661_AGC_SELF)
   	 	 	   ifGainLevel = MxL661_ModeIfAgcSet[ii].IfGainSelfAgc;
   	 	 	   
   	 	 	 mcDBG_MSG_L2(("Find IF out and AGC parameter setting from MxL661_ModeIfAgcSet table. \n"));
			 mcDBG_MSG_L2(("IF out freq = %d, IF Path = %d. \n",ifOutFreq,ifPath)); 
   	 	 	 retResult = TRUE;
   	 	 	 break;
   	 	 }
   	   }
   	   ii++;
     }
   
     if (retResult == TRUE)
     {
   	  *IfFreq = ifOutFreq; 
   	  *IfSpecInvert = ifSpecInvert;
	  *IfPath = ifPath;
   	  *IfGain = ifGainLevel;
	  *AgcType = agcType;
	  *AgcSel = agcSel;
	  *AgcSetPoint = agcSetPoint;
     }

   return (retResult);
}

//-----------------------------------------------------------------------------
/** TDVWH810F_TunerSetFreForChannelScan
 *  Set Tuner PLL forTDVWH810F/TDVWH810F to tune RF frequency during Channel Scan.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range
                   -1 fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 MXL661_TunerSetFreqForChannelScan(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
   UINT32 programFreqHz, Freq;
   UINT8 mxLSignalMode, Mode = param->Modulation;
   ITUNER_CTX_T *psTunerCtx;
   SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
   MXL661_COMMAND_T apiCmd;
   MXL_STATUS status;
   MXL661_SIGNAL_MODE_E signalMode = MXL661_ANA_NTSC_MODE;
   UINT8 mxlIfOutFreq, mxlIfSpecInvert, mxlIfPath, mxlIfGainLevel, mxlAgcType, mxlAgcSel, mxlAgcSetPoint; 
   MXL661_IF_FREQ_E ifOutFreq;
   MXL_BOOL ifSpecInvert;
   MXL661_IF_PATH_E ifPath;
   UINT8 ifGainLevel;
   MXL661_AGC_TYPE_E agcType;
   MXL661_AGC_ID_E agcSel;
   UINT8 agcSetPoint;
   UINT8 ifoutput;
   UINT32 ifAbsFreqInKHz[] = {3650, 4000, 4100, 4150, 4500, 4570, 5000, 5380, 6000, 6280, 7200, 8250, 35250, 36000, 36150, 36650, 44000}; 

   UINT32 ii,mxLIfFreqInKHz;   // IF freq in kHz unit

   // These 3 parameters shall be defined in input parameters
   UINT8 mxLBandWidth, SAW_BW;
   MXL661_BW_E bandWidth;
   MXL661_XTAL_FREQ_E xtalFreqSel = MXL661_XTAL_16MHz;
   MXL661_NTSC_CARRIERS_E ntscCarrier = MXL661_NTSC_CARRIER_NA; 
	 
   pTCtx->u4RF_Freq = param->Freq;//Update current frequency to context, KHz unit 
   Freq = param->Freq * 1000; // transfer from KHz to video freq (Hz unit)
   mcDBG_MSG_L2((C_MXL_MXL661_STR " Set Freq %d KHz\n",Freq));
   
   SAW_BW = pEUCtx->m_SAW_BW;
   if(Mode==MOD_DTMB)
   	{
   	SAW_BW=SAW_BW_8M;
    }
   //Check if frequency out of tuner LO range			
	 if (Freq < (C_MXL661_FREQ_DBOUND_LOWER) || 
		  Freq > (C_MXL661_FREQ_DBOUND_UPPER))
	 {
		 mcDBG_MSG_L2(("Out of range for LO!\n"));
		 //return (+1);
	 }

     if (MXL661_TVModeTranslate(Mode, &mxLSignalMode) ==  FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! invalid TV mode parameter \n"));
       return FALSE;	 
	 }	 
	 signalMode = (MXL661_SIGNAL_MODE_E)mxLSignalMode;
	 
	 // Translate picture frequency to center frequency when Tuner works in Analog TV mode 
	 // Freq is picture frequency, programFreqHz is the center frequency point 	  
     if (MXL661_GetBandWidth(signalMode, SAW_BW, Freq, &mxLBandWidth) == FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! band width seek is fail \n"));
       return FALSE;	 
	 }	
     bandWidth = (MXL661_BW_E)mxLBandWidth; 
    
     if (MXL661_GetTuneFreq(signalMode, Freq, &programFreqHz) == FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! Tune frequency translate fail \n"));
       return FALSE;	 
	 }	
	 Freq = programFreqHz;
	 
	 // Here we pass bandwidth parameter from input variables for digital TV mode

     // Search IF out and AGC parameter setting value according to work mode and band width
     // Define default setting value firstly. 
     ifOutFreq = MXL661_IF_3_65MHz;
	 ifSpecInvert = MXL_ENABLE;
	 ifPath = MXL661_IF_PATH2; 
     ifGainLevel = 11;
	 agcSel = MXL661_AGC2; 
     agcType = MXL661_AGC_EXTERNAL;
     agcSetPoint = 66;
  mcDBG_MSG_L2(("Mode=%d,bandWidth=%d\n",Mode,bandWidth));
    
	 // Find default IF out and AGC parameters according to 
	 if (MXL661_GetIfOutAgcDefaultSet(Mode, bandWidth, &mxlIfOutFreq, &mxlIfSpecInvert, &mxlIfPath, &mxlIfGainLevel, &mxlAgcType, &mxlAgcSel, &mxlAgcSetPoint) == FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! IF out and AGC default searching is fail \n"));
       return FALSE;	 
	 }	
	 ifOutFreq = (MXL661_IF_FREQ_E)mxlIfOutFreq;
	 ifSpecInvert = (MXL_BOOL)mxlIfSpecInvert;
	 ifPath = (MXL661_IF_PATH_E)mxlIfPath; 
     ifGainLevel = mxlIfGainLevel;
	 agcSel = (MXL661_AGC_ID_E)mxlAgcSel ; 
     agcType = (MXL661_AGC_TYPE_E)mxlAgcType;
     agcSetPoint = mxlAgcSetPoint; 
	 mcDBG_MSG_L1(("ifOutFreq=%d,ifSpecInvert=%d,ifPath=%d,\n ifGainLevel=%d,agcSel=%d,agcType=%d, agcSetPoint=%d\n",ifOutFreq,ifSpecInvert,ifPath,ifGainLevel,agcSel,agcType,agcSetPoint));
     // Set IF frequency according to ifOutFreq 
     psTunerCtx = pTCtx;
	 
	 // Translate IF frequency selection to absolute kHz unit value according to signal mode 
	 if (MXL661_GetIfFreqInKHz(signalMode, ifOutFreq, bandWidth,  &mxLIfFreqInKHz) == FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! IF out Freq translation is fail \n"));
       return FALSE;	 
	 }
     switch (signalMode)
     {
       case MXL661_ANA_NTSC_MODE:
       case MXL661_ANA_PAL_BG:
       case MXL661_ANA_PAL_I:
       case MXL661_ANA_PAL_D:
       case MXL661_ANA_SECAM_I:
       case MXL661_ANA_SECAM_L:
         psTunerCtx->u2IF_Freq_A  =  mxLIfFreqInKHz;
         break; 
       
       case MXL661_DIG_DVB_T_DTMB:
       case MXL661_DIG_DVB_C:
       case MXL661_DIG_ISDBT_ATSC:   
       case MXL661_DIG_J83B:     	   	
     	   psTunerCtx->u2IF_Freq = mxLIfFreqInKHz;
     	   break;      	 
 
       default: psTunerCtx->u2IF_Freq = mxLIfFreqInKHz; break; 
      }
     mcDBG_MSG_L2(("IF out frequency = %d KHz \n",mxLIfFreqInKHz)); 	  
    
    
   apiCmd.commandId = MXL661_DEV_IF_OUT_CFG;
   apiCmd.MxLIf.cmdIfOutCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
   apiCmd.MxLIf.cmdIfOutCfg.IFOutFreq = ifOutFreq;
   apiCmd.MxLIf.cmdIfOutCfg.IFInversion = ifSpecInvert;
   apiCmd.MxLIf.cmdIfOutCfg.GainLevel = ifGainLevel;
   apiCmd.MxLIf.cmdIfOutCfg.PathSel = ifPath;
   apiCmd.MxLIf.cmdIfOutCfg.ManualFreqSet = MXL_DISABLE;   
   status = MxLWare661_API_ConfigDevice(&apiCmd);	
   if (MXL_FAILED == status)
   {
     mcDBG_MSG_L2(("Error! MXL661_DEV_IF_OUT_CFG\n"));
   }
  
   // Set AGC configuration
   apiCmd.commandId = MXL661_TUNER_AGC_CFG;
   apiCmd.MxLIf.cmdAgcSetCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
   apiCmd.MxLIf.cmdAgcSetCfg.AgcSel = agcSel;
   apiCmd.MxLIf.cmdAgcSetCfg.AgcType = agcType;
   apiCmd.MxLIf.cmdAgcSetCfg.SetPoint = agcSetPoint;
   apiCmd.MxLIf.cmdAgcSetCfg.AgcPolarityInverstion = MXL_DISABLE;
   status = MxLWare661_API_ConfigTuner(&apiCmd);
   if (MXL_FAILED == status)
   {
     mcDBG_MSG_L2(("Error! MXL661_TUNER_AGC_CFG\n"));
   }
   // Set DVB-C application as default mode for channel scan
 
   if(MXL661_fgEnterAutoSearch)
   {
	 apiCmd.commandId = MXL661_TUNER_MODE_CFG;
	 apiCmd.MxLIf.cmdModeCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
	 apiCmd.MxLIf.cmdModeCfg.SignalMode = MXL661_DIG_DVB_C;
	 apiCmd.MxLIf.cmdModeCfg.IFOutFreqinKHz = 5000;  // 5MHz
	 apiCmd.MxLIf.cmdModeCfg.XtalFreqSel = MXL661_XTAL_16MHz;
	 apiCmd.MxLIf.cmdModeCfg.IFOutGainLevel = 11;
	 status = MxLWare661_API_ConfigTuner(&apiCmd);
	 if (MXL_FAILED == status)
	 {
	   mcDBG_MSG_L1(("Error!set DVBC application fail\n"));
	 }
	 mcDBG_MSG_L2(("===Enter auto-search,Set DVB-C application as default mode \n"));

	 apiCmd.MxLIf.cmdModeCfg.SignalMode = signalMode;
	 apiCmd.MxLIf.cmdModeCfg.IFOutFreqinKHz = mxLIfFreqInKHz;
	 apiCmd.MxLIf.cmdModeCfg.XtalFreqSel = xtalFreqSel;
	 apiCmd.MxLIf.cmdModeCfg.IFOutGainLevel = ifGainLevel;
	 MXL661_fgEnterAutoSearch=FALSE;
   	}
	
   // Here we need pass in Bandwidth, Xtal freq and NTSC carrier mode parameters
   // Channel frequency & bandwidth setting
   apiCmd.commandId = MXL661_TUNER_CHAN_TUNE_CFG;
   apiCmd.MxLIf.cmdChanTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
   apiCmd.MxLIf.cmdChanTuneCfg.TuneType = MXL661_VIEW_MODE;
   apiCmd.MxLIf.cmdChanTuneCfg.BandWidth = bandWidth;
   apiCmd.MxLIf.cmdChanTuneCfg.FreqInHz = Freq;
   apiCmd.MxLIf.cmdChanTuneCfg.SignalMode = signalMode;
   apiCmd.MxLIf.cmdChanTuneCfg.XtalFreqSel = xtalFreqSel;
   apiCmd.MxLIf.cmdChanTuneCfg.NtscCarrier = ntscCarrier;
   apiCmd.MxLIf.cmdChanTuneCfg.IFOutFreqinKHz = mxLIfFreqInKHz;
   status = MxLWare661_API_ConfigTuner(&apiCmd);
   if (MXL_FAILED == status)
   {
     mcDBG_MSG_L2(("Error! MXL661_TUNER_CHAN_TUNE_CFG\n"));
   }
   
   // Abort Tuner firstly 
   apiCmd.commandId = MXL661_TUNER_START_TUNE_CFG;
   apiCmd.MxLIf.cmdStartTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
   apiCmd.MxLIf.cmdStartTuneCfg.StartTune = MXL_DISABLE;
   status = MxLWare661_API_ConfigTuner(&apiCmd);   
   if (MXL_FAILED == status)
   {
     mcDBG_MSG_L2(("Error! MXL661_TUNER_START_TUNE_CFG\n"));
   }
   
   // Then enable Tuner 			
   apiCmd.commandId = MXL661_TUNER_START_TUNE_CFG;
   apiCmd.MxLIf.cmdStartTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
   apiCmd.MxLIf.cmdStartTuneCfg.StartTune = MXL_ENABLE;
   status = MxLWare661_API_ConfigTuner(&apiCmd);
    if (MXL_FAILED == status)
   {
     mcDBG_MSG_L2(("Error! MXL661_TUNER_START_TUNE_CFG\n"));
   }     
 
	 for(ii=0;ii<C_MXL_MXL661_PLL_POLL_CNT;ii++)
	 {
	   MxLWare661_OEM_Sleep(C_MXL_MXL661_PLL_POLL_INTERVAL);
	   if(MXL661_TunerGetStatus(pTCtx))break;
	 }
	
	 if(ii<C_MXL_MXL661_PLL_POLL_CNT)
	 {
	   mcDBG_MSG_L2(("PLL lock at %d th time\n",ii));
	 }
	 else
	 {
	   mcDBG_MSG_ERR(("PLL unlock !(%d)",ii));
	 }
   
    return (0);
}

//----------------------------------------------------------------------------- 
/** TDVWH810F_TunerSetFreq
 *  Set Tuner PLL forTDVWH810F/TDVWH810F to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range 
                   -1 fail  I2C error 
 */
//-----------------------------------------------------------------------------
INT16 MXL661_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{	
    if(!fgTunerInit)
	{	
        MXL661_DriverInit(pTCtx);
        fgTunerInit=TRUE;
	}
    
    if(param->fgAutoSearch)
    {
	   	mcDBG_MSG_L2(("When do AutoSearch,MXL661 set freq\n"));
		MXL661_TunerSetFreqForChannelScan(pTCtx,param);	
    }
	else
    {
        UINT32 programFreqHz, Freq;      
        UINT8 mxLSignalMode, Mode = param->Modulation;
        ITUNER_CTX_T *psTunerCtx;
        SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
        MXL661_COMMAND_T apiCmd;
        MXL_STATUS status; 	
        MXL661_SIGNAL_MODE_E signalMode = MXL661_ANA_NTSC_MODE; 	 
        UINT8 mxlIfOutFreq, mxlIfSpecInvert, mxlIfPath, mxlIfGainLevel, mxlAgcType, mxlAgcSel, mxlAgcSetPoint; 	 
        MXL661_IF_FREQ_E ifOutFreq;
        MXL_BOOL ifSpecInvert;
        MXL661_IF_PATH_E ifPath;
        UINT8 ifGainLevel;
        MXL661_AGC_ID_E agcSel;
        MXL661_AGC_TYPE_E agcType;
        UINT8 agcSetPoint;

        UINT32 ii, mxLIfFreqInKHz;   // IF freq in kHz unit

        // These 3 parameters shall be defined in input parameters
        UINT8 mxLBandWidth, SAW_BW;
        MXL661_BW_E bandWidth=MXL661_ANA_TV_DIG_CABLE_BW_8MHz;
        MXL661_XTAL_FREQ_E xtalFreqSel = MXL661_XTAL_16MHz;
        MXL661_NTSC_CARRIERS_E ntscCarrier = MXL661_NTSC_CARRIER_NA;

        pTCtx->u4RF_Freq = param->Freq;//Update current frequency to context, KHz unit
        Freq = param->Freq * 1000; // transfer from KHz to video freq (Hz unit)
        mcDBG_MSG_L2((C_MXL_MXL661_STR " Set Freq %d KHz\n",Freq));
   	 MXL661_fgEnterAutoSearch=TRUE;//SET FALG AS TRUE FOR NEXT CHANNEL SCAN
        SAW_BW = pEUCtx->m_SAW_BW;
        if(Mode==MOD_DTMB)
	   	{
            SAW_BW=SAW_BW_8M;
	    }
        //Check if frequency out of tuner LO range			
        if (Freq < (C_MXL661_FREQ_DBOUND_LOWER) || 
    		  Freq > (C_MXL661_FREQ_DBOUND_UPPER))
        {
		 mcDBG_MSG_L2(("Out of range for LO!\n"));
            //return (+1);
        }

        if (MXL661_TVModeTranslate(Mode, &mxLSignalMode) ==  FALSE)
        {
            mcDBG_MSG_ERR(("Error! invalid TV mode parameter \n"));
            return FALSE;	 
        }	 
        signalMode = (MXL661_SIGNAL_MODE_E)mxLSignalMode;
	 
        // Translate picture frequency to center frequency when Tuner works in Analog TV mode 
        // Freq is picture frequency, programFreqHz is the center frequency point 	  
        if (MXL661_GetBandWidth(signalMode, SAW_BW,Freq, &mxLBandWidth) == FALSE)
        {
            mcDBG_MSG_ERR(("Error! band width seek is fail \n"));
            return FALSE;	 
	    }	
        bandWidth = (MXL661_BW_E)mxLBandWidth; 
    
        if (MXL661_GetTuneFreq(signalMode, Freq, &programFreqHz) == FALSE)
        {
            mcDBG_MSG_ERR(("Error! Tune frequency translate fail \n"));
            return FALSE;	 
        }	
	 
        Freq = programFreqHz;	 
	
        // Search IF out and AGC parameter setting value according to work mode and band width 
        // Define default setting value firstly. 
        ifOutFreq = MXL661_IF_3_65MHz;
        ifSpecInvert = MXL_ENABLE;
        ifPath = MXL661_IF_PATH2; 
        ifGainLevel = 11;  
        agcSel = MXL661_AGC2; 
        agcType = MXL661_AGC_EXTERNAL;
        agcSetPoint = 66;
   
        // Find default IF out and AGC parameters according to 
        if (MXL661_GetIfOutAgcDefaultSet(Mode, bandWidth, &mxlIfOutFreq, &mxlIfSpecInvert, &mxlIfPath, &mxlIfGainLevel, &mxlAgcType, &mxlAgcSel, &mxlAgcSetPoint) == FALSE)
        {
            mcDBG_MSG_ERR(("Error! IF out and AGC default searching is fail \n"));
            return FALSE;	 
        }	
        ifOutFreq = (MXL661_IF_FREQ_E)mxlIfOutFreq;
        ifSpecInvert = (MXL_BOOL)mxlIfSpecInvert;
        ifPath = (MXL661_IF_PATH_E)mxlIfPath; 
        ifGainLevel = mxlIfGainLevel;
        agcSel = (MXL661_AGC_ID_E)mxlAgcSel ; 
        agcType = (MXL661_AGC_TYPE_E)mxlAgcType;
        agcSetPoint = mxlAgcSetPoint;    
     
        // Set IF frequency according to ifOutFreq 
        psTunerCtx = pTCtx;
        // Translate IF frequency selection to absolute kHz unit value according to signal mode 
        if (MXL661_GetIfFreqInKHz(signalMode, ifOutFreq, bandWidth,  &mxLIfFreqInKHz) == FALSE)
        {
            mcDBG_MSG_ERR(("Error! IF out Freq translation is fail \n"));
            return FALSE;	 
        }
     
        switch (signalMode)
        {
            case MXL661_ANA_NTSC_MODE:
            case MXL661_ANA_PAL_BG:
            case MXL661_ANA_PAL_I:
            case MXL661_ANA_PAL_D:
            case MXL661_ANA_SECAM_I:
            case MXL661_ANA_SECAM_L:
                psTunerCtx->u2IF_Freq_A  =  mxLIfFreqInKHz;
                break; 
       
            case MXL661_DIG_DVB_T_DTMB:
            case MXL661_DIG_DVB_C:
            case MXL661_DIG_ISDBT_ATSC:   
            case MXL661_DIG_J83B:     	   	
                psTunerCtx->u2IF_Freq = mxLIfFreqInKHz;
                break;      	 
 
            default: psTunerCtx->u2IF_Freq = mxLIfFreqInKHz; break; 
        }
        mcDBG_MSG_L2(("IF out frequency = %d KHz \n",mxLIfFreqInKHz)); 	  
    
    	
        apiCmd.commandId = MXL661_DEV_IF_OUT_CFG;
        apiCmd.MxLIf.cmdIfOutCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
        apiCmd.MxLIf.cmdIfOutCfg.IFOutFreq = ifOutFreq;
        apiCmd.MxLIf.cmdIfOutCfg.IFInversion = ifSpecInvert;
        apiCmd.MxLIf.cmdIfOutCfg.GainLevel = ifGainLevel;
        apiCmd.MxLIf.cmdIfOutCfg.PathSel = ifPath;
        apiCmd.MxLIf.cmdIfOutCfg.ManualFreqSet = MXL_DISABLE;   
        status = MxLWare661_API_ConfigDevice(&apiCmd);	
        if (MXL_FAILED == status)
        {
            mcDBG_MSG_ERR(("Error! MXL661_DEV_IF_OUT_CFG\n"));
        }
  
        // Set AGC configuration 
        apiCmd.commandId = MXL661_TUNER_AGC_CFG;
        apiCmd.MxLIf.cmdAgcSetCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
        apiCmd.MxLIf.cmdAgcSetCfg.AgcSel = agcSel;
        apiCmd.MxLIf.cmdAgcSetCfg.AgcType = agcType;
        apiCmd.MxLIf.cmdAgcSetCfg.SetPoint = agcSetPoint;
        apiCmd.MxLIf.cmdAgcSetCfg.AgcPolarityInverstion = MXL_DISABLE;  
        status = MxLWare661_API_ConfigTuner(&apiCmd);
        if (MXL_FAILED == status)
        {
            mcDBG_MSG_ERR(("Error! MXL661_TUNER_AGC_CFG\n"));
        }
   
        // Before set frequency point, we need set tuner application mode. 
        apiCmd.commandId = MXL661_TUNER_MODE_CFG;
        apiCmd.MxLIf.cmdModeCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
        apiCmd.MxLIf.cmdModeCfg.SignalMode = signalMode;
        apiCmd.MxLIf.cmdModeCfg.IFOutFreqinKHz = mxLIfFreqInKHz;
        apiCmd.MxLIf.cmdModeCfg.XtalFreqSel = xtalFreqSel;
        apiCmd.MxLIf.cmdModeCfg.IFOutGainLevel = ifGainLevel;
        status = MxLWare661_API_ConfigTuner(&apiCmd);
        if (MXL_FAILED == status)
        {
            mcDBG_MSG_ERR(("Error! MXL661_TUNER_MODE_CFG\n"));
        }  
       
        mcDBG_MSG_L2(("Center freq = %d Hz, Band Width = %d. \n",Freq,bandWidth)); 	
  
        // Here we need pass in Bandwidth, Xtal freq and NTSC carrier mode parameters 
        // Channel frequency & bandwidth setting
        apiCmd.commandId = MXL661_TUNER_CHAN_TUNE_CFG;
        apiCmd.MxLIf.cmdChanTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
        apiCmd.MxLIf.cmdChanTuneCfg.TuneType = MXL661_VIEW_MODE;
        apiCmd.MxLIf.cmdChanTuneCfg.BandWidth = bandWidth;
        apiCmd.MxLIf.cmdChanTuneCfg.FreqInHz = Freq;
        apiCmd.MxLIf.cmdChanTuneCfg.SignalMode = signalMode;
        apiCmd.MxLIf.cmdChanTuneCfg.XtalFreqSel = xtalFreqSel;
        apiCmd.MxLIf.cmdChanTuneCfg.NtscCarrier = ntscCarrier;
        apiCmd.MxLIf.cmdChanTuneCfg.IFOutFreqinKHz = mxLIfFreqInKHz;
        status = MxLWare661_API_ConfigTuner(&apiCmd);
        if (MXL_FAILED == status)
        {
            mcDBG_MSG_ERR(("Error! MXL661_TUNER_CHAN_TUNE_CFG\n"));
        }
       
        // Abort Tuner firstly 
        apiCmd.commandId = MXL661_TUNER_START_TUNE_CFG;
        apiCmd.MxLIf.cmdStartTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
        apiCmd.MxLIf.cmdStartTuneCfg.StartTune = MXL_DISABLE;
        status = MxLWare661_API_ConfigTuner(&apiCmd);   
        if (MXL_FAILED == status)
        {
            mcDBG_MSG_ERR(("Error! MXL661_TUNER_START_TUNE_CFG\n"));
        }
   
        // Then enable Tuner 			
        apiCmd.commandId = MXL661_TUNER_START_TUNE_CFG;
        apiCmd.MxLIf.cmdStartTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
        apiCmd.MxLIf.cmdStartTuneCfg.StartTune = MXL_ENABLE;
        status = MxLWare661_API_ConfigTuner(&apiCmd);
        if (MXL_FAILED == status)
        {
            mcDBG_MSG_ERR(("Error! MXL661_TUNER_START_TUNE_CFG\n"));
        }  
   
        for(ii=0;ii<C_MXL_MXL661_PLL_POLL_CNT;ii++)
	    {
	        MxLWare661_OEM_Sleep(C_MXL_MXL661_PLL_POLL_INTERVAL);
	        if(MXL661_TunerGetStatus(pTCtx))
	            break;
	    }
	
	    if(ii<C_MXL_MXL661_PLL_POLL_CNT)
	    {
	        mcDBG_MSG_L2(("PLL lock at %d th time\n",ii));
	    }
	    else
	    {
	        mcDBG_MSG_L1(("PLL unlock !(%d)",ii));
	    }
    }
    
    return (0);
}
//----------------------------------------------------------------------------- 
/** TDVWH810F_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *MXL661_TunerGetVer(VOID)
{
    return (C_MXL_MXL661_STR);
}

#ifdef  USING_IF_OUT_API    	
//----------------------------------------------------------------------------- 
/** MXL661_GetIfOutDefaultSet
 *  Get IF Out parameters default setting values, such as IF Freq, IF spectrum invert and IF Gain.
 *  @param  Mode: Tuner work mode.in MTK definition
 *  @retval (1) IfFreq:  IF Freq
 *          (2) IfSpecInvert:  IF spectrum invert
 *          (3) IfGain:  IF Gain
 */
//-----------------------------------------------------------------------------
#ifdef  MODIFY_IF_OUT_API_INTERFACE
static BOOL MXL661_GetIfOutDefaultSet(UINT8 Mode, UINT32 FreqInHz, UINT8 *IfFreq, UINT8 *IfSpecInvert, UINT8 * IfGain)
#else
static BOOL MXL661_GetIfOutDefaultSet(UINT8 Mode, UINT8 *IfFreq, UINT8 *IfSpecInvert, UINT8 * IfGain)
#endif

{
	 BOOL retResult = FALSE;
	 MXL661_SIGNAL_MODE_E signalMode = MXL661_ANA_NTSC_MODE; 	 
	 MXL661_IF_FREQ_E ifOutFreq = MXL661_IF_5MHz;
	 MXL_BOOL ifSpecInvert = MXL_DISABLE;
	 UINT8 ifGainLevel = 11;
	 MXL661_AGC_TYPE_E agcType;
	 MXL661_BW_E bandWidth; 	 
     UINT8 mxLSignalMode, mxLBandWidth, SAW_BW = SAW_BW_7M;  // default case ! 
   	 UINT8 mxlIfOutFreq, mxlIfSpecInvert, mxlIfPath, mxlIfGainLevel, mxlAgcType, mxlAgcSel, mxlAgcSetPoint; 
	 
#ifndef  MODIFY_IF_OUT_API_INTERFACE
     UINT32 FreqInHz = 666000000;  // Assume it is PAL_GH band
#endif
     if (MXL661_TVModeTranslate(Mode, &mxLSignalMode) ==  FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! invalid TV mode parameter \n"));
       return FALSE;	 
	 }	 
	 signalMode = (MXL661_SIGNAL_MODE_E)mxLSignalMode;
	 
     if (MXL661_GetBandWidth(signalMode, SAW_BW, FreqInHz, &mxLBandWidth) == FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! band width seek is fail \n"));
       return FALSE;	 
	 }	
     bandWidth = (MXL661_BW_E)mxLBandWidth; 
	 
	 if (MXL661_GetIfOutAgcDefaultSet(Mode, bandWidth, &mxlIfOutFreq, &mxlIfSpecInvert, &mxlIfPath, &mxlIfGainLevel, &mxlAgcType, &mxlAgcSel, &mxlAgcSetPoint) == FALSE)
	 {
	   mcDBG_MSG_ERR(("Error! IF out and AGC default searching is fail \n"));
       return FALSE;	 
	 }	
	 
     ifOutFreq = (MXL661_IF_FREQ_E)mxlIfOutFreq;
	 ifSpecInvert = (MXL_BOOL)mxlIfSpecInvert;
	 ifPath = (MXL661_IF_PATH_E)mxlIfPath; 
     ifGainLevel = mxlIfGainLevel;
	 agcSel = (MXL661_AGC_ID_E)mxlAgcSel ; 
     agcType = (MXL661_AGC_TYPE_E)mxlAgcType;
     agcSetPoint = mxlAgcSetPoint; 
	 
   	 *IfFreq = ifOutFreq; 
   	 *IfSpecInvert = ifSpecInvert;
   	 *IfGain = ifGainLevel; 
   
   return (retResult);
}
#endif


//----------------------------------------------------------------------------- 
/** MXL661_SetIfSpecInvert
 *  Set IF Out parameter IF spectrum invert. 
 *  @param  I2cSlaveAddr: I2C slave adddress.
 *          ifSpecInvert:  IF spectrum invert 
 *  @retval TRUE: success; FALSE: Fail.  
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_SetIfSpecInvert(UINT8 I2cSlaveAddr, UINT8 ifSpecInvert)
{
	BOOL retResult = FALSE;
	UINT8 status = MXL_TRUE;
	UINT8 readBack, control;
	
	// Path selection is bit<7:6> 	
	status |= MxLWare661_OEM_ReadRegister(I2cSlaveAddr, IF_PATH_GAIN_REG, &readBack);
	
	// Get original spectrum invert setting. 
	control = ((readBack & 0xC0) >> 6);  //bit<7:6>
	mcDBG_MSG_L2(("Original IF spectrum invert set is %d. \n", control)); 
	 
	// Protect other bits 
	control = (readBack & 0x3F);

  // Spectrum invert indication is bit<7:6>
  if (MXL_ENABLE == ifSpecInvert)
    control += (0x3 << 6);  
  
  status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, IF_PATH_GAIN_REG, control); 
  
  mcDBG_MSG_L2(("Change IF spectrum invert setting to %d. \n", ifSpecInvert)); 
  
  if ((MXL_STATUS)status == MXL_TRUE) 
    retResult = TRUE; 
    
  return (retResult);	
}
//----------------------------------------------------------------------------- 
/** MXL661_SetIfPath
 *  Set IF Out parameter IF path. 
 *  @param  I2cSlaveAddr: I2C slave adddress.
 *          IfPath:  IF path 
 *  @retval TRUE: success; FALSE: Fail.  
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_SetIfPath(UINT8 I2cSlaveAddr, UINT8 IfPath)
{
	BOOL retResult = FALSE;
	UINT8 status = MXL_TRUE;
	UINT8 readBack, control;
	
	// Path selection is bit<5:4> 	
	status |= MxLWare661_OEM_ReadRegister(I2cSlaveAddr, IF_PATH_GAIN_REG, &readBack);
	
	// Get original IF path set value. 
	control = ((readBack & 0x30) >> 4);  //bit<5:4>
	mcDBG_MSG_L2(("Original IF path is %d. (1: Path1, 2:Path2) \n", control));	
	
	// Protect other bits 	
	control = (readBack & 0xCF);

  // Path selection is bit<5:4> 
  control += ((IfPath & 0x03) << 4);
  status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, IF_PATH_GAIN_REG, control); 

  mcDBG_MSG_L2(("Change IF path setting to %d. \n", IfPath));
  
  if ((MXL_STATUS)status == MXL_TRUE) 
    retResult = TRUE; 
    
  return (retResult);  
}
//----------------------------------------------------------------------------- 
/** MXL661_GetIfOutDefaultSet
 *  Get IF Out parameters default setting values, such as IF Freq, IF spectrum invert and IF Gain. 
 *  @param  I2cSlaveAddr: I2C slave adddress.
 *          IfGain:  IF Gain level, range 0 to 13  
 *  @retval TRUE: success; FALSE: Fail.  
 */
//-----------------------------------------------------------------------------
static BOOL MXL661_SetIfGainLevel(UINT8 I2cSlaveAddr, UINT8 IfGain)
{
	BOOL retResult = FALSE;
	UINT8 status = MXL_TRUE;
	UINT8 readBack, control;
	
  // Read back IF Path and Gain setting register 
  status |= MxLWare661_OEM_ReadRegister(I2cSlaveAddr, IF_PATH_GAIN_REG, &readBack);  
  
 	// Get original IF gain level value. 
	control = (readBack & 0x0F);  //bit<3:0>
	mcDBG_MSG_L2(("Original IF gain level is %d. \n", control));	
	 
  // IF Gain is ibt<3:0>
  control = (readBack & 0xF0);
  
  control += (IfGain & 0x0F);
  status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, IF_PATH_GAIN_REG, control); 

  // Change to page1 
  status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, PAGE_CHANGE_REG, 0x01);    

  switch(IfGain)
  {
    case IF_GAIN_SET_POINT1: 
      status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, DFE_DACIF_BYP_GAIN_REG, 0x55);    
      break;

    case IF_GAIN_SET_POINT2: 
      status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, DFE_DACIF_BYP_GAIN_REG, 0x4D);    
      break;

    case IF_GAIN_SET_POINT3: 
      status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, DFE_DACIF_BYP_GAIN_REG, 0x45);    
      break;

    default: 
      status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, DFE_DACIF_BYP_GAIN_REG, 0x1C);
     break; 
  }

  // Return to page0 
  status |= MxLWare661_OEM_WriteRegister(I2cSlaveAddr, PAGE_CHANGE_REG, 0x00);   
  
  mcDBG_MSG_L2(("Change IF gain level to %d. \n", (IfGain & 0x0F))); 
    
  if ((MXL_STATUS)status == MXL_TRUE) 
  	retResult = TRUE; 
  
  return (retResult);
}

//----------------------------------------------------------------------------- 
/** MXL661_SetSawBw
 *  Set saw bandwidth
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval saw bandwidth
 */
//-----------------------------------------------------------------------------

static VOID  MXL661_SetSawBw(ITUNER_CTX_T * pTCtx, UINT8 sawbw)
{
 pTCtx->specific_member.eu_ctx.m_SAW_BW=sawbw;
 mcDBG_MSG_L1(("MXL661_SetSawBw=%d\n",sawbw));
}

static INT32 i4AdjustPowerRec_DVBT = 2;

UINT32 MXL661_GetSSI_DVBT(ITUNER_CTX_T* pTunerCtx)
{
    INT16 Power_Ref[] =
    {
        -93,
        -91,
        -90,
        -89,
        -88,
        -87,
        -85,
        -84,
        -83,
        -82,
        -82,
        -80,
        -78,
        -77,
        -76,
    };
    INT32 i4Power_ref = 0;
    INT32 i4Power_rec_dbm = 0;
    INT32 i4Power_rel = 0;
	UINT32 u4SSI_Indicator = 0;
	SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);
	
    MXL661_COMMAND_T apiCmd;
	apiCmd.commandId = MXL661_TUNER_RX_PWR_REQ;
	apiCmd.MxLIf.cmdTunerPwrReq.I2cSlaveAddr = C_MXL661_LO_ADDRESS;

    // initial level for best mux, Ken, 100607
    pCtx->m_SigLvScan = 0;



    /* Get Power Ref Value */
    mcDBG_MSG_L2(("DVBT: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));


    /* Get Power_Rec Value */
	if (MXL_TRUE == MxLWare661_API_GetTunerStatus(&apiCmd)) 
	{
	  i4Power_rec_dbm = (SINT16)(apiCmd.MxLIf.cmdTunerPwrReq.RxPwr);
	}
	else
	{
      mcDBG_MSG_L2(("DVBT SSI Get Receive Power ERROR"));
	}
	i4Power_rec_dbm/=100;
    i4Power_rec_dbm+=i4AdjustPowerRec_DVBT;
    mcDBG_MSG_L2(("DVBT: Power_rec=%d,%d\n",i4Power_rec_dbm,i4AdjustPowerRec_DVBT));

	/*Get Ref Value-Start*/
	if(pTunerCtx->sSSIPara.i2Mod==DVBT_Qam_Mode_Unknow)
	{
		mcDBG_MSG_L1(("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec_dbm,i4Power_ref,i4Power_rel));
		return u4SSI_Indicator;
	}
	else
	{
		i4Power_ref = Power_Ref[(pTunerCtx->sSSIPara.i2Mod)*DVBT_CR_Mode_Size + pTunerCtx->sSSIPara.i2CR];
	}



    /* Get Power Rel Value */
    i4Power_rel = i4Power_rec_dbm - i4Power_ref;
	mcDBG_MSG_L2(("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec_dbm,i4Power_ref,i4Power_rel));
    /* Cal SSI_Indicator Value */
    if(i4Power_rel < -15)
    {
        u4SSI_Indicator = 0;
    }
    else if((i4Power_rel >= -15) && (i4Power_rel < 0))
    {
        u4SSI_Indicator = 2*(i4Power_rel+15)/3;
    }
    else if((i4Power_rel >= 0) && (i4Power_rel < 20))
    {
        u4SSI_Indicator = 4*i4Power_rel+10;
    }
    else if((i4Power_rel >= 20) && (i4Power_rel < 35))
    {
        u4SSI_Indicator = 2*(i4Power_rel-20)/3+90;
    }
    else if(i4Power_rel >= 35)
    {
        u4SSI_Indicator = 100;
    }

    // update signal level for best mux, Ken, 100607
    pCtx->m_SigLvScan = (INT8)u4SSI_Indicator;
    mcDBG_MSG_L2(("u4SSI_Indicator = %d",u4SSI_Indicator));
    return u4SSI_Indicator;
}

/*xiongfeng 12-07-03:DVBC&Anolog Receive Power*/
INT32 MXL661_Get_RX_POWER(ITUNER_CTX_T* pTunerCtx)
{

    INT32 i4Power_rec_dbm = 0;
	INT32 i4Power_rec_dbuv = 0;
	INT16 refIndex=0;
	
	MXL661_COMMAND_T apiCmd;
	apiCmd.commandId = MXL661_TUNER_RX_PWR_REQ;
	apiCmd.MxLIf.cmdTunerPwrReq.I2cSlaveAddr = C_MXL661_LO_ADDRESS;


	/* Get Power_Rec Value */
   	if (MXL_TRUE == MxLWare661_API_GetTunerStatus(&apiCmd)) 
	{
	  i4Power_rec_dbm = (SINT16)(apiCmd.MxLIf.cmdTunerPwrReq.RxPwr);
	}
	else
	{
      mcDBG_MSG_L2(("Get Receive Power ERROR"));
	}
	i4Power_rec_dbm/=100;
	i4Power_rec_dbuv=i4Power_rec_dbm+107;
	refIndex=pTunerCtx->sSSIPara.i2Mod;
	mcDBG_MSG_L2(("Power_rec(dbuv)=%d\n",i4Power_rec_dbuv));
    return i4Power_rec_dbuv;
}


//-----------------------------------------------------------------------------
/** MXL661_SetSSICondition
 *  Set SSI Condition for MXL661
 *  @param
 *  @retval void
 */
//-----------------------------------------------------------------------------
void MXL661_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);

}

//----------------------------------------------------------------------------- 
/** TDVWH810F_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 MXL661_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){
      MXL661_COMMAND_T apiCmd;
	  MXL_STATUS status; 
	  MXL661_AGC_ID_E AgcSel=MXL661_AGC1;
	  SINT16 rxPowerIn1_100dB = 0;
	  UINT8 regAddr, readBack = 0, agcLock = 0;
	  UINT8 *inPtr;		   
	  UINT8 *outPtr;
	  SINT16 *s16OutPtr;
	  UINT16 k; 
	  UINT8 ifSpecInvert=MXL_ENABLE;
	  UINT8 ifGainLevel=0;
	  UINT8 ifPath=1; 	
	  UINT8 u1Data;
			  	  	       
    switch(eOperation){
    case itGetVer:
        {
           CHAR ** RetStr=(CHAR **)pInOutVal;
           *RetStr=MXL661_TunerGetVer();
        }
        break;   
     case itGetTunerRelReg:
     	{
		UINT8 subSysId=SetVal;
		
		arrAtdRelReg[0].u1RegVal=0x10;//PRA target H
		arrAtdRelReg[1].u1RegVal=0x0e;//PRA target L
		arrAtdRelReg[2].u1RegVal=0x08;//DAGC setting
		arrAtdRelReg[3].u1RegVal=0x2c;//DAGC Target level H
		arrAtdRelReg[4].u1RegVal=0x2c;//DAGC Target level L
		arrAtdRelReg[5].u1RegVal=0x80;//
		arrAtdRelReg[8].u1RegVal=0x03;
		arrAtdRelReg[9].u1RegVal=0x60;
		arrAtdRelReg[10].u1RegVal=0x00;

		switch(subSysId){
		case MOD_SECAM_L:
		case MOD_SECAM_L1:
			{
			arrAtdRelReg[0].u1RegVal=0x05;//PRA target H
			arrAtdRelReg[1].u1RegVal=0x04;//PRA target L
			arrAtdRelReg[2].u1RegVal=0x88;//DAGC setting
			arrAtdRelReg[5].u1RegVal=0x81;//
			arrAtdRelReg[8].u1RegVal=0x87;
			arrAtdRelReg[9].u1RegVal=0x00;
			arrAtdRelReg[10].u1RegVal=0x03;
			}
		break;
		case MOD_PAL_I:
			{
			 arrAtdRelReg[3].u1RegVal=0x2a;//DAGC Target level H
			 arrAtdRelReg[4].u1RegVal=0x2a;//DAGC Target level L
			}
		break;
		case MOD_NTSC_M:
			{
			 arrAtdRelReg[3].u1RegVal=0x30;//DAGC Target level H
			 arrAtdRelReg[4].u1RegVal=0x30;//DAGC Target level L
			}
		break;
		default:
			break;
			}
		*(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
		}
       break; 
     case itGetEqScriptNormal: 
     	{
      *(UINT8**)pInOutVal = pMXL661EqNormal[SetVal];
      }
       break;
     case itGetEqScriptWeak:
     	{
     	*(UINT8**)pInOutVal = pMXL661EqWeak[SetVal];
     	}
     	 break;
	case itSetSawBw:
		{
		MXL661_SetSawBw(pTCtx,SetVal);
		}
		break;
    case itGetTunerRxPower://Get Tuner Rx Power 
        {
        	s16OutPtr = (SINT16 *)pInOutVal;
          apiCmd.commandId = MXL661_TUNER_RX_PWR_REQ;
          apiCmd.MxLIf.cmdTunerPwrReq.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
  
          if (MXL_TRUE == MxLWare661_API_GetTunerStatus(&apiCmd)) 
          {
            rxPowerIn1_100dB = (SINT16)(apiCmd.MxLIf.cmdTunerPwrReq.RxPwr);
          }
          *s16OutPtr = rxPowerIn1_100dB;
           mcDBG_MSG_L2(("Rx power = %d (unit = 0.01dBm) \n", rxPowerIn1_100dB));            
        }
	      break;
	/*xiongfeng 12-07-03:Add SSI function*/
	case itSetSSICond:
         MXL661_SetSSICondition(pTCtx, pInOutVal);
          break;
	case itGetSSIIndex:   /*xiongfeng 12-07-03:0 for DVBT SSI*/
	{ 
		   *((INT32*)pInOutVal) = MXL661_GetSSI_DVBT(pTCtx);       
	}
	      break;   
	case itGetSignalLevel:   /*xiongfeng 12-07-03:0 for DVBC&Anolog Receive Power*/
	{ 
		   *((INT32*)pInOutVal) = MXL661_Get_RX_POWER(pTCtx);       
	}
	      break;       
    case itGetTunerAgcLock://Get Tuner AGC lock status 
        {
        	outPtr = (UINT8 *)pInOutVal;
          apiCmd.commandId = MXL661_TUNER_AGC_LOCK_REQ;
          apiCmd.MxLIf.cmdTunerAgcLockReq.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
  
          if (MXL_TRUE == MxLWare661_API_GetTunerStatus(&apiCmd)) 
          {
            agcLock = (UINT8)apiCmd.MxLIf.cmdTunerAgcLockReq.AgcLock;
          }
          *outPtr = agcLock;   
           mcDBG_MSG_L2(("AGC Lock status = %d \n", agcLock));     	 
        }
	      break;	 
	      
    case itTunerRead:
    	  {
    	  	 // SetVal: Register address; 
    	  	 // *pInOutVal: Out, register data 
    	  	 outPtr = (UINT8 *)pInOutVal;
    	  	 status = MxLWare661_OEM_ReadRegister(C_MXL661_LO_ADDRESS, SetVal, &readBack);
    	  	 if (MXL_TRUE == status) 
    	  	 	{
    	  	 		*outPtr = readBack;
    	  	 	}
    	  }
    	  break; 
    	  
    case itTunerWrite:
    	  {
    	  	// SetVal: Register address; 
    	  	// *pInOutVal: register data 
    	  	outPtr = (UINT8 *)pInOutVal;
    	  	status = MxLWare661_OEM_WriteRegister(C_MXL661_LO_ADDRESS, SetVal, (UINT8)(*outPtr));
    	  }
    	  break;     	  
	case itSetFineRF://for mxl661 fine-tune mode , step is 62.5kHz
		{
			INT8 fineTuneScaleUp=*(INT8* )pInOutVal;
			MXL661_fgEnterAutoSearch=TRUE;//SET FALG AS TRUE FOR NEXT CHANNEL SCAN
		   	// check 0x13 bit<2> == 1, 1 means disable Tuner AFC. 
		   MxLWare661_OEM_ReadRegister(C_MXL661_LO_ADDRESS, 0x13, &u1Data);		
		   if ((u1Data & 0x04) == 0x04)  
		   {
			mcDBG_MSG_L2(("[MXL661]Tuner fine-tune mode,need disable Tuner AFC. current mode is disable AFC\n"));
		   }
		   else
		   {
			mcDBG_MSG_L2(("[MXL661]Tuner fine-tune mode,need disableTuner AFC. current mode is enable AFC\n"));
		   }
		   
		   /*call register read/write operation to enable fine tune operation	
		   MxLWare661_OEM_ReadRegister(MXL661_I2C_ADDR, 0x84, &DataByte);
		   if ((DataByte & 0x80) == 0x80)  // Fine tune is by passed 
		   {
			DataByte &= 0x7F;  // Set <bit7> = 0	 
			MxLWare661_OEM_WriteRegister(MXL661_I2C_ADDR, 0x84, DataByte);
		   }
		   */ 
		   // call MXL661_TUNER_ENABLE_FINE_TUNE_CFG  API functions
		   apiCmd.commandId = MXL661_TUNER_ENABLE_FINE_TUNE_CFG;
		   apiCmd.MxLIf.cmdEnableFineTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
		   apiCmd.MxLIf.cmdEnableFineTuneCfg.EnableFineTune = MXL_ENABLE;
		   status = MxLWare661_API_ConfigTuner(&apiCmd);
		   if (MXL_FAILED == status)
		   {
			 mcDBG_MSG_L1(("Error! MXL661_TUNER_ENABLE_FINE_TUNE_CFG\n"));
			 return ITUNER_NOT_OK;
		   }
		 	//call MXL661_TUNER_FINE_TUNE_CFG API functions
		   apiCmd.commandId = MXL661_TUNER_FINE_TUNE_CFG;
		   apiCmd.MxLIf.cmdFineTuneCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
		   apiCmd.MxLIf.cmdFineTuneCfg.ScaleUp = fineTuneScaleUp;
		   status = MxLWare661_API_ConfigTuner(&apiCmd);
		   if (MXL_FAILED == status)
		   {
			 mcDBG_MSG_L1(("Error! MXL661_TUNER_FINE_TUNE_CFG\n"));
			 return ITUNER_NOT_OK;
		   }
		}
	break; 
    case itSetAgcMode:
    	{ 
    		  // SetVal: In, AGC type, 0:Self or 1:close loop ; 
    	  	// *pInOutVal: AGC selection, 1:MXL661_AGC1 or 2:MXL661_AGC2 
    	  	inPtr = (UINT8 *)pInOutVal;
    	  	if ((UINT8)(*inPtr) == 1) AgcSel = MXL661_AGC1;
    	  	else if ((UINT8)(*inPtr) == 2) AgcSel = MXL661_AGC2;	
    	  		
    	    apiCmd.commandId = MXL661_TUNER_AGC_CFG;
          apiCmd.MxLIf.cmdAgcSetCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
          apiCmd.MxLIf.cmdAgcSetCfg.AgcSel = AgcSel ;
          apiCmd.MxLIf.cmdAgcSetCfg.AgcType = (MXL661_AGC_TYPE_E)SetVal;
          apiCmd.MxLIf.cmdAgcSetCfg.SetPoint = 66;
          status = MxLWare661_API_ConfigTuner(&apiCmd);	
          
          mcDBG_MSG_L2(("Configure AGC type = %d, AGC selection = %d (1:MXL661_AGC1, 0:MXL661_AGC2) \n", SetVal, AgcSel));	
    	}
    	break;
   
    case itSetIfPath:
    	{ 
    		  // SetVal: In, working mode  
    	  	// *pInOutVal: store IF out setting parameters 
    	  	// pInOutVal[0]: IF path 
#ifdef  USING_IF_OUT_API    	  	 
    	    UINT8 workMode = SetVal;
			
    	  	inPtr = (UINT8 *)pInOutVal;
			
#ifdef  MODIFY_IF_OUT_API_INTERFACE
            UINT32 FreqInHz;
			FreqInHz = (UINT32)(*inPtr[0]);   // NOTE: Here we have to modify it !
			ifPath = (UINT8)(*inPtr[1]);
#else
    	  	ifPath = (UINT8)(*inPtr);
#endif
    	  	// Find other IF out parameters by searching MxL661_ModeIfAgcSet table
#ifdef  MODIFY_IF_OUT_API_INTERFACE
    	  	if (MXL661_GetIfOutDefaultSet(workMode, &ifOutFreq, &ifSpecInvert, &ifGainLevel))
#else
            if (MXL661_GetIfOutDefaultSet(workMode, FreqInHz, &ifOutFreq, &ifSpecInvert, &ifGainLevel))
#endif
    	  	{
            apiCmd.commandId = MXL661_DEV_IF_OUT_CFG;
            apiCmd.MxLIf.cmdIfOutCfg.I2cSlaveAddr = C_MXL661_LO_ADDRESS;
            apiCmd.MxLIf.cmdIfOutCfg.PathSel = (MXL661_IF_PATH_E)ifPath;
            apiCmd.MxLIf.cmdIfOutCfg.IFOutFreq = (MXL661_IF_FREQ_E)ifOutFreq;
            apiCmd.MxLIf.cmdIfOutCfg.IFInversion = (MXL_BOOL)ifSpecInvert;
            apiCmd.MxLIf.cmdIfOutCfg.GainLevel = (UINT8)ifGainLevel;
            apiCmd.MxLIf.cmdIfOutCfg.ManualFreqSet = MXL_DISABLE; 
            status = MxLWare661_API_ConfigDevice(&apiCmd);	 
            
            mcDBG_MSG_L2(("IF path setting success. \n"));   		
          }
          else 
          	{
          		mcDBG_MSG_L2(("IF path setting fail! \n"));
          	}
#else         
          ifPath = (UINT8)SetVal;  	
          if (MXL661_SetIfPath(C_MXL661_LO_ADDRESS,ifPath))
          {
          	 mcDBG_MSG_L2(("IF path setting success. \n"));
          }
          else 
          {
          	mcDBG_MSG_L2(("IF path setting fail! \n")); 	
          }          
#endif     
      
    	}
    	break;
		           
	    case itSetIfGain:
	    	{
	    		 ifGainLevel = SetVal;
	    		 if (MXL661_SetIfGainLevel(C_MXL661_LO_ADDRESS, ifGainLevel)) 
    	  	 { 
    	  	  // success ! 
    	  	  mcDBG_MSG_L2(("IF gain setting success. \n"));
    	  	 }
    	  	 else 
    	  	 	{
    	  	 		mcDBG_MSG_L2(("IF gain setting fail! \n"));
    	  	 	}
	    	}
	    	break; 	
	    		       
	    case itSetIfSpecInvert:
	    	{
	    		 ifSpecInvert = SetVal;
	    		 if (MXL661_SetIfSpecInvert(C_MXL661_LO_ADDRESS, ifSpecInvert)) 
    	  	 { 
    	  	  // success ! 
    	  	  mcDBG_MSG_L2(("IF spectrum invert setting success. \n"));
    	  	 }
    	  	 else 
    	  	 	{
    	  	 		mcDBG_MSG_L2(("IF spectrum invert setting fail! \n"));
    	  	 	}
	    	}
	    	break; 
		    	    
	    case itDumpTunerReg:
    	  {
    	  	 for (k = 0; k < MXL661_MAX_REG_ADDRESS; k++)
    	  	 {
    	  	 	 regAddr = (k & 0xFF);    	  	 	
    	  	 	 if (k > 0xFF)
    	  	 	 {
    	  	 	 	 status = MxLWare661_OEM_ReadRegister(C_MXL661_LO_ADDRESS, PAGE_CHANGE_REG, &readBack);
    	  	 	 	 if (readBack != 1)  // Page 1, do not need change ! 
    	  	 	 	 {
    	  	 	     // Change to page 1 
    	  	 	     status = MxLWare661_OEM_WriteRegister(C_MXL661_LO_ADDRESS, PAGE_CHANGE_REG, 1);
    	  	 	   }
    	  	 	 }	 	 		
    	  	   status = MxLWare661_OEM_ReadRegister(C_MXL661_LO_ADDRESS, regAddr, &readBack);
    	  	 	 if (MXL_TRUE == status) 
    	  	 	 {  
    	  	 	 	 mcDBG_MSG_L2(("MxL661 Read Reg 0x%02X -> 0x%02X \n",regAddr, readBack));
    	  	 	 }
    	  	 	 else 
    	  	 	 {
    	  	 	 	 mcDBG_MSG_L2(("MxL661 Read Reg 0x%02X error ! \n",regAddr));
    	  	 	 }
    	  	 }
    	  	 
    	  	 // Resume to page 0
    	  	 status = MxLWare661_OEM_WriteRegister(C_MXL661_LO_ADDRESS, PAGE_CHANGE_REG, 0);
    	  }
    	  break; 

   #ifdef  CC_MT5890

     case itSetRSetting:

	      switch(SetVal){

            case MOD_PAL_BG: 
            case MOD_PAL_DK: 
            case MOD_PAL_I:
			case MOD_NTSC_M:
                  #if  (DEMO_BOARD==TRUE)
			             *(UINT32 *)pInOutVal=0x07 ;
				  #else
				         *(UINT32 *)pInOutVal=0x07 ;
				  #endif
		    break;
				  
	   	    case MOD_SECAM_L:
            case MOD_SECAM_L1:
                  #if  (DEMO_BOARD==TRUE)
				        
			             *(UINT32 *)pInOutVal=0x07 ;
				  
				  #else
				         *(UINT32 *)pInOutVal=0x07 ;
				  #endif
			 break;
				  
	        default:
                   *(UINT32 *)pInOutVal=0x07 ;

	  	}
	  break;
    #endif
    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}
