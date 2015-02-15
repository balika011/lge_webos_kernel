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
 * $RCSfile: pi_tuner_TDA18257.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_tuner_TDA18257.c
 *  Tuner control for NXP TDA18257
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#include "demod_tuner_interface.h"
//#include "pd_i2capi.h"


//*--------------------------------------------------------------------------------------
//* Include NXP Driver files
//*--------------------------------------------------------------------------------------
#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmbslTDA18257.h"
#include "tmbslTDA18257_Advanced.h"
#include "pd_atd_if.h" //wenming 2012-4-16: add for ATV_CONN_MODE enum

#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "i2c_api.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "tuner_if.h"

#include "eq_script_tda18257.h"
//#include "pi_def_dvbt.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Frequency boundary .Get from tuner spec
#define C_TDA18257_FREQ_DBOUND_UPPER   870*1000*1000     
#define C_TDA18257_FREQ_DBOUND_LOWER    42*1000*1000 
#define C_TDA18257_FREQ_DBOUND_UPPER_Ana     870*1000*1000
#define C_TDA18257_FREQ_DBOUND_LOWER_Ana    42*1000*1000

//PLL lock check parameters
#define C_NXP_TDA18257_PLL_POLL_INTERVAL      10 //ms
#define C_NXP_TDA18257_PLL_POLL_TIMETOUT      100 //ms
#define C_NXP_TDA18257_PLL_POLL_CNT           C_NXP_TDA18257_PLL_POLL_TIMETOUT/C_NXP_TDA18257_PLL_POLL_INTERVAL //counter
#define C_TDA18257_I2C_CLK_DIV         0x100
#define C_TDA18257_PLL_LOCK_BIT        6

#define NXP_TDA18257_ADDRESS           ((UINT8)  0xC0)


#define C_TDA18257_IF_FREQUENCY          ((UINT16)  3250)  /* kHz */
//#define C_TDA18257_IF_FREQUENCY_ANA      ((UINT16)  38900)  /* kHz */
//#define C_TDA18257_IF_FREQUENCY_L1       ((UINT16)  33900)  /* kHz */
#define C_TDA18257_LO_DIVIDER_STEP       ((UINT32) 1000)  /* Hz */
#define C_TDA18257_LO_DIVIDER_STEP_ANA   ((UINT16)  1000)  /* Hz */
#define C_NXP_TDA18257_STR                        "NXP_TDA18257 2013-03-20\n 2013-06-06\n v1.1"

#define C_TDA18257_IF_FREQUENCY_PAL_B    ((UINT16)  6400)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_PAL_G    ((UINT16)  6750)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_PAL_DK   ((UINT16)  6850)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_PAL_I    ((UINT16)  7250)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_NTSC_M   ((UINT16)  5400)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_SECAM_L  ((UINT16)  6750)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_SECAM_L1 ((UINT16)  1250)  /* kHz */
#define C_TDA18257_IF_FREQUENCY_DVBT_6M        ((UINT16)  3250)  /* kHz */  //JUST for 3.25M
#define C_TDA18257_IF_FREQUENCY_DVBT_7M        ((UINT16)  3500)  /* kHz */  //JUST for 3.5M
#define C_TDA18257_IF_FREQUENCY_DVBT_8M        ((UINT16)  4000)  /* kHz */  //JUST for 4M
#define C_TDA18257_IF_FREQUENCY_ATSC        ((UINT16)  3250)  /* kHz */  //JUST for 3.25M
#define C_TDA18257_IF_FREQUENCY_DVBC        ((UINT16)  5000)  /* kHz */  //JUST for 5M
#define C_TDA18257_IF_FREQUENCY_ATSC_64QAM  ((UINT16)  3250) /*kHz*/  
#define C_TDA18257_IF_FREQUENCY_ISDBT    	((UINT16)  3250) /*kHz*/  
#define C_TDA18257_IF_FREQUENCY_DTMB 		((UINT16)  4000) /*kHz*/  

#define C_TDA18257_SAW_BW              SAW_BW_8M
#define C_TDA18257_TOP_SET_ANA         ((UINT8)   0x0A)

//for display signal level
#define C_U8_TDA18257_AGC_IF_LVL_MAX     0xc2  //no signal
#define C_U8_TDA18257_AGC_IF_LVL_MIN     0xa8  //-70dbm input

#define C_TDA18257_CHANNEL_SCAN_JUMP_SMALL_STEP ((UINT16) 250)
#define C_TDA18257_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP ((UINT16) 1500)
#define C_TDA18257_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN ((UINT16) 500)

//MT5135 PRA 
#define C_TDA18257_AGC_IF_SLP             0.125 
#define C_TDA18257_AGC_IF_INT             -0.5
#define C_TDA18257_AGC_IF_MAX             0
#define C_TDA18257_AGC_IF_MIN            -1


#define C_U8_TDA18257_AGC_IF_SLP          (INT8)(C_TDA18257_AGC_IF_SLP *64 )
#define C_U8_TDA18257_AGC_IF_INT          (INT8)(C_TDA18257_AGC_IF_INT *16 )
#define C_U8_TDA18257_AGC_IF_MAX          (INT8)(C_TDA18257_AGC_IF_MAX *128)
#define C_U8_TDA18257_AGC_IF_MIN          (INT8)(C_TDA18257_AGC_IF_MIN *128)




static UINT8* pTDA18257EqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pTDA18257EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN]= {NULL};



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

enum
{
	DVBT_QPSK =0,
	DVBT_Q16,
	DVBT_Q64,
	DVBT_Qam_Mode_Size 
};

#define DVBT_Qam_Mode_Unknow	DVBT_Qam_Mode_Size
enum
{
	DVBT_CR_12 = 0,
	DVBT_CR_23,
	DVBT_CR_34,
	DVBT_CR_56,
	DVBT_CR_78,
	DVBT_CR_Mode_Size
};

#define CC_SUPPORT_SCANNING_MODE 1

#define PRA_TARGET_H_INDEX 0
#define PRA_TARGET_L_INDEX 1
#define SLD_DAGC_00_INDEX 2
#define PRA_LOOP_CONF_INDEX  3
#define DAGC_TARGET_LEVEL_H_INDEX 4   /*wenming 120104: DAGC target level is by modulation when 89&96*/
#define DAGC_TARGET_LEVEL_L_INDEX 5

#define PGA_INDEX  0x00


#if !(defined(CC_MT5365) || defined(CC_MT5395))
/*wenming 11-10-21:don't change top 6 items' order!!*/
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
 
 {0x7b7,0x10}, //PRA target H
 {0x7b9,0x10}, //PRA target L
 {0x790,0x08}, //DAGC setting
 {0x7ca,0x88}, //PRA speed
 {0x794,0x30}, //DAGC Target level H
 {0x7a4,0x30}, //DAGC Target level L
 {0x1b4,0x17},
 {0x7d4,0x04},//RF_BIAS  /*wenming 11-12-07: new PRA parameter*/
 {0x7d5,0xf9},//IF_BIAS1
 {0x7d6,0xf8},//IF_BIAS2
 {0x7d7,0xec},//RF_SLOPE
 {0x7d8,0xfc},//IF_SLOPE1
 {0x7d9,0xf8},//IF_SLOPE2
 {0x7da,0x7f},//RF Max
 {0x7db,0xe5},//IF Max
 {0x7dc,0x80},//RF Min
 {0x7dd,0x9f},//IF Min 1
 {0x7de,0x80},//IF Min 2
 {0x00,0x00}//end flag
 };
static CN_TUNER_REL_REG_T arrCNDTMBRelReg[]=
{
	{0xfd4,0x04},
    {0xfd5,0xf8},
    {0xfd6,0xf8},
    {0xfd7,0xec},
    {0xfd8,0xf8},
    {0xfd9,0xf8},
    {0xfda,0x7f},
    {0xfdb,0x00},
    {0xfdc,0x80},
    {0xfdd,0x80},
    {0xfde,0x80},
    {0xfdf,0x11},
    {0xfe0,0x00},
    //{0x39d,0x3a},//cold power on setting only for dtmb , the setting different with tuner 
    {0x0,0x0}//end flag
 };
static CN_TUNER_REL_REG_T arrCNDVBCRelReg[]=
{
	{0xfd4,0x04},
    {0xfd5,0xf8},
    {0xfd6,0xf8},
    {0xfd7,0xec},
    {0xfd8,0xf8},
    {0xfd9,0xf8},
    {0xfda,0x7f},
    {0xfdb,0x00},
    {0xfdc,0x80},
    {0xfdd,0x80},
    {0xfde,0x80},
    {0xfdf,0x11},
    {0xfe0,0x00},
    {0x0,0x0}//end flag

 };
#else
 /*wenming 11-10-21:don't change top 4 items' order!!*/
 static ATD_TUNER_REL_REG_T arrAtdRelReg[]={

 {0x2b7,0x10}, //PRA target H
 {0x2b9,0x10}, //PRA target L
 {0x290,0x08}, //DAGC setting
 {0x2ca,0x88}, //PRA speed
 {0x294,0x38}, //DAGC Target H
 {0x2a4,0x28}, //DAGC Target L
 {0x2d4,0x04},//RF_BIAS    /*wenming 11-12-07: new PRA parameter*/
 {0x2d5,0xf8},//IF_BIAS1
 {0x2d6,0xf8},//IF_BIAS2
 {0x2d7,0xec},//RF_SLOPE
 {0x2d8,0xf8},//IF_SLOPE1
 {0x2d9,0xf8},//IF_SLOPE2
 {0x2da,0x7f},//RF Max
 {0x2db,0x00},//IF Max
 {0x2dc,0x80},//RF Min
 {0x2dd,0x80},//IF Min 1
 {0x2de,0x80},//IF Min 2
 {0x13b,0x80},
 {0x00,0x00}//end flag
};
#endif


static tmErrorCode_t UserWrittenI2CRead(tmUnitSelect_t tUnit,	UInt32 AddrSize, UInt8* pAddr,
UInt32 ReadLen, UInt8* pData);
static tmErrorCode_t UserWrittenI2CWrite (tmUnitSelect_t tUnit, 	UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData);
static tmErrorCode_t UserWrittenWait(tmUnitSelect_t tUnit, UInt32 tms);
static tmErrorCode_t UserWrittenPrint(UInt32 level, const char* format, ...);

static UINT8 fgTunerInit=FALSE;

#if CC_SUPPORT_SCANNING_MODE
static UINT8 fgSetScanningMode = FALSE;
#endif

/** TDA18257_TunerInit
 *  Tuner initialzation forTDA18257.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */

//-----------------------------------------------------------------------------
VOID TDA18257_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{
    ITUNER_CTX_T *psTunerCtx = pTCtx;
    SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);
	SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);

  	//tmErrorCode_t err = TM_OK;	 
    psTunerCtx->I2cAddress = NXP_TDA18257_ADDRESS;
    psTunerCtx->u2IF_Freq = C_TDA18257_IF_FREQUENCY;
	//psTunerCtx->u2IF_Freq_A = C_TDA18257_IF_FREQUENCY_ANA;//analog if frequency for ATD
	psTunerCtx->u4RF_Freq = 0;
	psTunerCtx->u2LO_Step = 100;  //*10 = 1KHz

	 // AGC para
    psTunerCtx->sAtdAgcPara.AgcRfBias = 0x2f; 
    psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x80;
    psTunerCtx->sAtdAgcPara.AgcIfMin  = 0xe3; 
    psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B; 

	 //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch = 0x09; //(MASK_PATCH_CR|MASK_PATCH_SENS)
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
	psTunerCtx->fgRFTuner =FALSE;//Digital tuner
	//psTunerCtx->u1EqIndex=0;//EQ index select

	pEUCtx->m_SAW_BW  = C_TDA18257_SAW_BW;
    pEUCtx->m_Ana_Top = C_TDA18257_TOP_SET_ANA;

	//for display signal level
	pEUCtx->m_ifAgc_lvl_max = (UINT8)C_U8_TDA18257_AGC_IF_LVL_MAX;
	pEUCtx->m_ifAgc_lvl_min = (UINT8)C_U8_TDA18257_AGC_IF_LVL_MIN;

	//Tuner frequency range
    pEUCtx->m_s4FreqBoundUpper = C_TDA18257_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_TDA18257_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_TDA18257_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_TDA18257_FREQ_DBOUND_LOWER_Ana;
	
	pEUCtx->m_Small_Step = C_TDA18257_CHANNEL_SCAN_JUMP_SMALL_STEP;
    pEUCtx->m_Middle_Step_Up = C_TDA18257_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP;
    pEUCtx->m_Middle_Step_Down = C_TDA18257_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN;

	pEUCtx->m_aucPara[2]= (UINT8)C_U8_TDA18257_AGC_IF_SLP; 
	pEUCtx->m_aucPara[3]= (UINT8)C_U8_TDA18257_AGC_IF_INT;
    pEUCtx->m_aucPara[4]= (UINT8)C_U8_TDA18257_AGC_IF_MAX;
	pEUCtx->m_aucPara[5]= (UINT8)C_U8_TDA18257_AGC_IF_MIN;
	
	#if defined(CC_MT5365)||defined(CC_MT5395)
	#if defined(DTD_INCLUDE)  //Warning: For MT5365/95 must define DTD_INCLUDE for US DTV
	//AD Target set ,for 5365/95+5112ee
    pUSCtx->m_aucPara[TUNER_FUNC_EN_TARGET_LEVEL] = TRUE;
	pUSCtx->m_aucPara[TUNER_PARA_TARGET_LEVEL_MSB] = 0x23;   
    pUSCtx->m_aucPara[TUNER_PARA_TARGET_LEVEL_LSB] = 0x00;
	#endif
	#else
	//5396
	/*wenming 12-01-09 comment: pUSCtx->m_aucPara_SLD may override pEUCtx->m_aucPara in some old branches*/
	pUSCtx->m_aucPara_SLD[2] = (UINT8)C_U8_TDA18257_AGC_IF_SLP; 
    pUSCtx->m_aucPara_SLD[3] = (UINT8)C_U8_TDA18257_AGC_IF_INT;
    pUSCtx->m_aucPara_SLD[4] = (UINT8)C_U8_TDA18257_AGC_IF_MAX;
    pUSCtx->m_aucPara_SLD[5] = (UINT8)C_U8_TDA18257_AGC_IF_MIN;
	#endif
	pUSCtx->u2LO_Step = 100;  //*10 = 1KHz
    
    #ifdef CC_SUPPORT_ATV_DESCRAMBLE
    #if defined(CC_MT5880)//for descrambler
	psTunerCtx->sTunerDescramblePara.u2CVBSDelay=0x02d0;
    #endif
    #endif

  	pTDA18257EqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]=EQTDA18257B;
	pTDA18257EqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]=EQTDA18257DK;
	pTDA18257EqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]=EQTDA18257I;
	pTDA18257EqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]=EQTDA18257L;
	pTDA18257EqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]=EQTDA18257L1;
	pTDA18257EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]=EQTDA18257M;

	pTDA18257EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]=EQTDA18257BGWeak;
	pTDA18257EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]=EQTDA18257DKWeak;
	pTDA18257EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]=EQTDA18257IWeak;
	pTDA18257EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]=EQTDA18257LWeak;
	pTDA18257EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]=EQTDA18257L1Weak;
	pTDA18257EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]=EQTDA18257MWeak;
	
	mcDBG_MSG_L2(("TDA18257_TunerInit\n"));

 } 


//----------------------------------------------------------------------------- 
/** TDA18257_TunerSetFreq
 *  Set Tuner PLL forTDA18257/TDA18257 to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range 
                   -1 fail  I2C error 
 */
//-----------------------------------------------------------------------------
INT16 TDA18257_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
   UINT32 Freq = param->Freq*1000; // transfer to video freq(Hz)
   UINT8 Mode = param->Modulation;
   int ii;
   tmbslFrontEndState_t PLLLock = tmbslFrontEndStateUnknown;
   TDA18257StandardMode_t stdMode = TDA18257_ANLG_DK;
   tmErrorCode_t err = TM_OK;	
   tmbslFrontEndDependency_t sSrvTunerFunc;
   SPECIFIC_MEMBER_EU_CTX * pTunerCtx= &(pTCtx->specific_member.eu_ctx);
   #if CC_SUPPORT_SCANNING_MODE
   UINT8 AutoSearch=param->fgAutoSearch;
   #endif
   
  // TuningParam TuningParam;	
 //  TuningParam.m_BandWidth = pTCtx->specific_member.eu_ctx.m_SAW_BW;
   pTCtx->u4RF_Freq = Freq/1000;//Update current frequency to context
   mcDBG_MSG_L2((C_NXP_TDA18257_STR " Set Freq %d Hz\n",Freq));

     
   /* Driver layer struct set-up to link with user written functions */
	sSrvTunerFunc.sIo.Write 			= UserWrittenI2CWrite;
	sSrvTunerFunc.sIo.Read				= UserWrittenI2CRead;
	sSrvTunerFunc.sTime.Get 			= NULL;
	sSrvTunerFunc.sTime.Wait			= UserWrittenWait;
	sSrvTunerFunc.sDebug.Print			= UserWrittenPrint;
	sSrvTunerFunc.sMutex.Init			= NULL;
	sSrvTunerFunc.sMutex.DeInit 		= NULL;
	sSrvTunerFunc.sMutex.Acquire		= NULL;
	sSrvTunerFunc.sMutex.Release		= NULL;
	sSrvTunerFunc.dwAdditionalDataSize	= 0;
	sSrvTunerFunc.pAdditionalData		= NULL;

  if(!fgTunerInit)
  {

   /* TDA18257 Driver initialization  */
  	 	 err = tmbslTDA18257_Open(0, &sSrvTunerFunc);
    if(err == TM_OK)
   		{   
   		mcDBG_MSG_INFO(("tmbslTDA18257_Open OK\n"));
		/* TDA18257 Hardware initialization */
		err = tmbslTDA18257_HwInit(0);
   		}
   	 else
		mcDBG_MSG_INFO(("tmbslTDA18257_Open failed\n"));
	  if(err == TM_OK)
	  	{
	  	err=tmbslTDA18257_SetPowerState(0, tmPowerOn);
	  	}
	  else
	  	{
	  	mcDBG_MSG_INFO(("tmbslTDA18257_HwInit failed\n"));
	  	}
   	
   	 if(err == TM_OK)
   		{
		/* TDA18257 standard mode selection */
		err = tmbslTDA18257_SetStandardMode(0, TDA18257_ANLG_DK);
   		}   
  	 else
		mcDBG_MSG_INFO(("tmbslTDA18257_SetPowerState failed\n"));
	 
	 if(err == TM_OK)
	 	{ 	
		mcDBG_MSG_INFO(("tmbslTDA18257_SetStandardMode OK\n"));
	 	}
	 else
	   mcDBG_MSG_INFO(("tmbslTDA18257_SetStandardMode fail\n"));
	 fgTunerInit=TRUE;

  }
   
    //Check if frequency out of tuner LO range			
if (Freq < (C_TDA18257_FREQ_DBOUND_LOWER) || 
				Freq > (C_TDA18257_FREQ_DBOUND_UPPER))
	{
		mcDBG_MSG_L2(("Out of range for LO!\n"));
		//return (+1);
	}

 if( Mode == MOD_ATSC_8VSB) //digital reception
   {
     stdMode = TDA18257_ATSC_6MHz;
	  pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_ATSC;
	 
   }		
 else if((Mode==MOD_J83B_64QAM)||(Mode==MOD_J83B_256QAM)||(Mode==MOD_J83B_AUTO_QAM))
	{ stdMode=TDA18257_QAM_6MHz;
	  pTCtx->u2IF_Freq=C_TDA18257_IF_FREQUENCY_ATSC_64QAM;   //Set the Demod IF freq (KHz)
	}
 else if(Mode==MOD_DVBT)
   { 
 	//if(TuningParam.m_BandWidth == SAW_BW_7M)
 	if(pTunerCtx->m_SAW_BW==SAW_BW_6M)
	{ stdMode =TDA18257_DVBT_6MHz;
	 pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBT_6M;}
    else if(pTunerCtx->m_SAW_BW==SAW_BW_7M)
	{ stdMode =TDA18257_DVBT_7MHz;
	  pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBT_7M;}
	else 
	{ stdMode =TDA18257_DVBT_8MHz;
	  pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBT_8M;}
   }
 else if(Mode==MOD_DVBT2)
   { 
 	//if(TuningParam.m_BandWidth == SAW_BW_7M)
 	if(pTunerCtx->m_SAW_BW==SAW_BW_6M)
	{ stdMode =TDA18257_DVBT_6MHz;
	 pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBT_6M;}
    else if(pTunerCtx->m_SAW_BW==SAW_BW_7M)
	{ stdMode =TDA18257_DVBT_7MHz;
	  pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBT_7M;}
	else 
	{ stdMode =TDA18257_DVBT_8MHz;
	  pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBT_8M;}
   }
 
 else if(Mode ==MOD_DVBC)
   {
	stdMode = TDA18257_QAM_8MHz;
	pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DVBC;
   }
 else if(Mode ==MOD_ISDBT)
   {
	stdMode = TDA18257_ISDBT_6MHz;
	pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_ISDBT;
   }
 else if(Mode ==MOD_DTMB)
   {
	stdMode = TDA18257_DTMB_8MHz;
	pTCtx->u2IF_Freq =C_TDA18257_IF_FREQUENCY_DTMB;
   }

 else  if( Mode == MOD_PAL_BG) 
   {
    if(Freq<300000000)
	 {
	  stdMode = TDA18257_ANLG_B;
      pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_PAL_B;
 	 }
    else
	 {
	  stdMode = TDA18257_ANLG_GH;
	  pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_PAL_G;
	 }
   }
 else  if( Mode == MOD_PAL_DK) 
   {
	stdMode = TDA18257_ANLG_DK;
	pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_PAL_DK;

   }
 else  if( Mode == MOD_PAL_I) 
   {
	stdMode = TDA18257_ANLG_I;
	pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_PAL_I;
   }
 else  if( Mode == MOD_SECAM_L) 
   {
	stdMode = TDA18257_ANLG_L;
	pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_SECAM_L;
   }
 else  if( Mode == MOD_SECAM_L1) 
   {
	stdMode = TDA18257_ANLG_LL;
	pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_SECAM_L1;
   }
 else  if( Mode == MOD_NTSC_M) 
   {
	stdMode = TDA18257_ANLG_MN;
	pTCtx->u2IF_Freq_A =C_TDA18257_IF_FREQUENCY_NTSC_M;
   }

 #if CC_SUPPORT_SCANNING_MODE
 if(ATV_CONN_MODE_CHAN_SCAN == AutoSearch)
 {
	if(FALSE == fgSetScanningMode)
	{
		tmbslTDA18257_SetScanningMode(0,TDA18257_Scanning_Mode_Enabled);
		fgSetScanningMode = TRUE;
		mcDBG_MSG_L2(("Enable scanning mode for TDA18257>>>>>>!\n"));
	}
 }
 else
 {
 	if(TRUE == fgSetScanningMode)
 	{
	 	tmbslTDA18257_SetScanningMode(0,TDA18257_Scanning_Mode_Disabled);
		fgSetScanningMode = FALSE;
		mcDBG_MSG_L2(("Disable scanning mode for TDA18257>>>>>!\n"));
 	}
 }
 #endif
 
	if(tmbslTDA18257_SetStandardMode(0, stdMode) != TM_OK)
		{
		mcDBG_MSG_ERR(("SetStandardMode fail!\n"));
		return -1;
		}
   	if(tmbslTDA18257_SetRF(0, Freq) != TM_OK)
	{
		mcDBG_MSG_ERR(("Set tuner PLL 0x%02X fail!\n",pTCtx->I2cAddress));
		return -1;
	}
	else
	{
	 mcDBG_MSG_L2(("LO_%02X: %dHz\n",pTCtx->I2cAddress, param->Freq));
	 
	 //Loop check PLL lock
	 //polling PLL lock status
	for(ii=0;ii<C_NXP_TDA18257_PLL_POLL_CNT;ii++)
	{
		x_thread_delay(C_NXP_TDA18257_PLL_POLL_INTERVAL);
		tmbslTDA18257_GetPLLState(0, &PLLLock);
		if(PLLLock == tmbslFrontEndStateLocked)
			break;
	}
	
	if(ii<C_NXP_TDA18257_PLL_POLL_CNT)
	{
	 mcDBG_MSG_L2(("PLL lock at %d th time\n",ii));
	}
	else
	{
	 mcDBG_MSG_ERR(("PLL unlock !(%d)",ii));
	}
	}
   
    x_thread_delay((UINT32)(10));
    return (0);
}
//----------------------------------------------------------------------------- 
/** TDA18257_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *TDA18257_TunerGetVer(VOID)
{
    return (C_NXP_TDA18257_STR);
	
}

//----------------------------------------------------------------------------- 
/** TDA18257_TunerGetVer
 *  Set saw bandwidth
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval saw bandwidth
 */
//-----------------------------------------------------------------------------
static VOID  TDA18257_SetSawBw(ITUNER_CTX_T * pTCtx, UINT8 sawbw)
{
 pTCtx->specific_member.eu_ctx.m_SAW_BW=sawbw;
 mcDBG_MSG_L2(("TDA18257_SetSawBw=%d\n",sawbw));

}

/*wenming 11-12-9:Add DVBT & C SSI function*/
static INT32 i4AdjustPowerRec_DVBT = 1;
static INT32 i4AdjustPowerRec_DVBC = 2;  

UINT32 TDA18257_GetSSI_DVBT(ITUNER_CTX_T* pTunerCtx)
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
	Int32 i4Power_rec_dbuv = 0; //The Int32 is used by NXP driver API, to avoid build warning_to_error, should use NXP define types
    INT32 i4Power_rec_dbm = 0;
    INT32 i4Power_rel = 0;

    UINT32 u4SSI_Indicator = 0;
    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);

    // initial level for best mux, Ken, 100607
    pCtx->m_SigLvScan = 0;



    /* Get Power Ref Value */
    mcDBG_MSG_L2(("DVBT: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));


    /* Get Power_Rec Value */
	tmbslTDA18257_GetPowerLevel(0, &i4Power_rec_dbuv); /*wenming 11-12-9:api return in 0.5 dbuv term*/
	i4Power_rec_dbuv = i4Power_rec_dbuv/2;
	i4Power_rec_dbm = i4Power_rec_dbuv - 107;
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

/*wenming 11-10-13:DVBC SSI*/
UINT32 TDA18257_GetSSI_DVBC(ITUNER_CTX_T* pTunerCtx)
{
    INT16 Power_Ref[] =
    {
        -82,//16QAM
        -79,//32QAM
        -76,//64
        -73,//128QAM
        -70,//256QAM
    };
    INT32 i4Power_ref = 0;
    Int32 i4Power_rec_dbuv = 0;
    INT32 i4Power_rec_dbm = 0;
    INT32 i4Power_rel = 0;
	INT16 refIndex=0;
    UINT32 u4SSI_Indicator = 0;

    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);
    pCtx->m_SigLvScan = 0;
    mcDBG_MSG_L2(("DVBC: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

	/* Get Power_Rec Value */
	tmbslTDA18257_GetPowerLevel(0, &i4Power_rec_dbuv); /*wenming 11-12-9:api return in 0.5 dbuv term*/
	i4Power_rec_dbuv = i4Power_rec_dbuv/2;
	i4Power_rec_dbm = i4Power_rec_dbuv - 107;
    i4Power_rec_dbm+=i4AdjustPowerRec_DVBC;
	refIndex=pTunerCtx->sSSIPara.i2Mod;
	mcDBG_MSG_L1(("refIndex=%d",refIndex));
	if(refIndex>4)
	{
		mcDBG_MSG_L1(("DVBC: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec_dbm,i4Power_ref,i4Power_rel));
		return u4SSI_Indicator;
	}
	else
	{
		i4Power_ref = Power_Ref[refIndex];
		mcDBG_MSG_L1(("i4Power_ref=%d\n",i4Power_ref));
	}
    i4Power_rel = i4Power_rec_dbm - i4Power_ref;
	mcDBG_MSG_L2(("DVBC: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec_dbm,i4Power_ref,i4Power_rel));
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
    pCtx->m_SigLvScan = (INT8) u4SSI_Indicator;
    return u4SSI_Indicator;
}


//-----------------------------------------------------------------------------
/** TDA18257_SetSSICondition
 *  Set SSI Condition for TH2603
 *  @param
 *  @retval void
 */
//-----------------------------------------------------------------------------
void TDA18257_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);

}

//----------------------------------------------------------------------------- 
/** TDA18257_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 TDA18257_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){
         
    switch(eOperation){
    case itGetVer:
        {
        CHAR ** RetStr=(CHAR **)pInOutVal;
        *RetStr=TDA18257_TunerGetVer();
        }
        break;
	case itGetTunerRelReg://Get ATD Tuner Rel register setting
		{
			/*wenming 11-10-21:for PRA set*/
			UINT8 subSysId = SetVal;
			UINT8 u1PRATar,u1Dagc,u1PRAConf,u1DagcTargetLevel_H,u1DagcTargetLevel_L;
			
			u1PRATar =0x10;
			u1Dagc = 0x08;
			u1PRAConf=0x88;
			u1DagcTargetLevel_H = 0x38;
			u1DagcTargetLevel_L = 0x28;
			
			switch(subSysId)
			{
            #if !(defined(CC_MT5365) || defined(CC_MT5395))
				case MOD_PAL_BG:
				case MOD_PAL_DK:
					u1DagcTargetLevel_H = 0x2c;
					u1DagcTargetLevel_L = 0x2c;
				break;
				case MOD_PAL_I:
					u1DagcTargetLevel_H = 0x2a;
					u1DagcTargetLevel_L = 0x2a;
				break;
				case MOD_NTSC_M:
					u1DagcTargetLevel_H = 0x30;
					u1DagcTargetLevel_L = 0x30;
			    break;
			#endif
				case MOD_SECAM_L:
				case MOD_SECAM_L1: /*Lower PRA Target level & Fix DAGC & Lower PRA Speed*/
			    u1PRATar =0x05;
			    u1Dagc=0x88;
			    u1PRAConf=0x99;  /*wenming 11-12-07: speed up PRA for secam resume slowly issue*/
				break;
			default://PAL & NTSC
			    break;
			}
			//PRA target	
			  arrAtdRelReg[PRA_TARGET_H_INDEX].u1RegVal=u1PRATar;
			  arrAtdRelReg[PRA_TARGET_L_INDEX].u1RegVal=u1PRATar;
			  arrAtdRelReg[SLD_DAGC_00_INDEX].u1RegVal=u1Dagc;
			  arrAtdRelReg[PRA_LOOP_CONF_INDEX].u1RegVal=u1PRAConf;
			  arrAtdRelReg[DAGC_TARGET_LEVEL_H_INDEX].u1RegVal=u1DagcTargetLevel_H;
			  arrAtdRelReg[DAGC_TARGET_LEVEL_L_INDEX].u1RegVal=u1DagcTargetLevel_L;
			/*wenming 11-10-20:for PRA set end*/
		  *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
		 
		}
		break;
    case itGetEqScriptNormal:
		/*wenming 11-12-14: EQ will be diffrent by B or G*/
		if((MOD_PAL_BG - MOD_ANA_TYPE_BEGIN) == SetVal)
		{
			if(pTCtx->u4RF_Freq < 300000) /*PAL B system*/
			{ 
				pTDA18257EqNormal[SetVal] = EQTDA18257B;
			}
			else /*PAL G system*/
			{
				pTDA18257EqNormal[SetVal] = EQTDA18257G;
			}
		}
        *(UINT8**)pInOutVal = pTDA18257EqNormal[SetVal];
        break;
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pTDA18257EqWeak[SetVal];
        break;
	case itSetSawBw:
		{
		TDA18257_SetSawBw(pTCtx,SetVal);
		}
		break;
    case itSetIF:
    	break;
	/*wenming 11-12-9:Add SSI function*/
	case itSetSSICond:
    TDA18257_SetSSICondition(pTCtx, pInOutVal);
    break;
	case itGetSSIIndex:   /*wenming 11-12-9:0 for DVBT SSI, 1 for DVBC SSI*/
	{ if (SetVal==0)
		*((INT32*)pInOutVal) = TDA18257_GetSSI_DVBT(pTCtx);
     else
		 *((INT32*)pInOutVal) = TDA18257_GetSSI_DVBC(pTCtx);
	}
	 break;
	case itGetSignalLevel:
	{
	 Int32 i4Power_rec_dbuv = 0;
	 UINT8 u1signallevl=0;
	 tmbslTDA18257_GetPowerLevel(0, &i4Power_rec_dbuv);
	 u1signallevl=i4Power_rec_dbuv/2+3;	 
	 *(UINT16 *)pInOutVal=(UINT16)u1signallevl;
    }
	break;
    #if !(defined(CC_MT5365) || defined(CC_MT5395))
   case itGetCNDTMBTunerRelReg://Get CNDTMB Tuner Rel register setting
   		{
        	*(CN_TUNER_REL_REG_T **)pInOutVal=arrCNDTMBRelReg;
    	}
    break;
   case itGetCNDVBCTunerRelReg://Get CNDVBC Tuner Rel register setting
		{
			*(CN_TUNER_REL_REG_T **)pInOutVal=arrCNDVBCRelReg;
		}
	break;
   #endif
   case itGetPGAIdx://Get PGA register setting
   	{
   		*(UINT8*)pInOutVal = PGA_INDEX;
   	}
   	break;
    default:
        mcDBG_MSG_L2(("NO case !Not implement in NXP18257 Tuner.\n"));
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : UserWrittenI2CRead
//* Object              : 
//* Input Parameters    : 	tmUnitSelect_t tUnit
//* 						UInt32 AddrSize,
//* 						UInt8* pAddr,
//* 						UInt32 ReadLen,
//* 						UInt8* pData
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
static tmErrorCode_t UserWrittenI2CRead(tmUnitSelect_t tUnit,	UInt32 AddrSize, UInt8* pAddr,
UInt32 ReadLen, UInt8* pData)
{
    /* Variable declarations */


    /* Customer code here */
    /* ...*/
#ifdef TUNER_USE_SPI //spi-i2c control tuner 	
		if(ucSPIRead(NXP_TDA18257_ADDRESS, *pAddr, pData, ReadLen) == 0)   /*wenming 110920*/
#else// tuner use i2c 
		if(ucI2cRead(NXP_TDA18257_ADDRESS, *pAddr, pData, ReadLen) == 0)   /*wenming 110920*/
#endif
		{
			return TM_OK;
		}
	else
		{
			mcDBG_MSG_ERR(("ICtrlBus_I2cTunerRead_Ext: %02X-%02X,ReadLen:%02X! TestLog:Read Fail!\n", NXP_TDA18257_ADDRESS, *pAddr, ReadLen));
			return !TM_OK;
		}
    /* ...*/
    /* End of Customer code here */


}

//*--------------------------------------------------------------------------------------
//* Function Name       : UserWrittenI2CWrite
//* Object              : 
//* Input Parameters    : 	tmUnitSelect_t tUnit
//* 						UInt32 AddrSize,
//* 						UInt8* pAddr,
//* 						UInt32 WriteLen,
//* 						UInt8* pData
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
#define I2C_MAX_WRITE 15
static tmErrorCode_t UserWrittenI2CWrite (tmUnitSelect_t tUnit, 	UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
    /* Variable declarations */

    /* Customer code here */
    /* ...*/
	UINT8 ucI = 0;
	UINT8 acBuffer[I2C_MAX_WRITE+1] = {0};
	acBuffer[0] = *pAddr;
	if(WriteLen > I2C_MAX_WRITE)   /*wenming 110920*/
	{
		mcDBG_MSG_ERR(("WriteLen > I2C_MAX_WRITE!!!\n"));
		return !TM_OK;
	}
	for(ucI=0;ucI < WriteLen;ucI++)
	{
		acBuffer[ucI+1] = pData[ucI];
	}
#ifdef TUNER_USE_SPI //spi-i2c control tuner 		
	if(ucSPIWriteOnly(NXP_TDA18257_ADDRESS,acBuffer,WriteLen+1)==0)
#else
	if(ucI2cWrite(NXP_TDA18257_ADDRESS,acBuffer[0],acBuffer+1,WriteLen)==0) 
#endif

	{
			return TM_OK;
		}
	else
		{
			mcDBG_MSG_ERR(("ICtrlBus_I2cTunerWrite_Ext: %02X-%02X,WriteLen:%02X! TestLog:Write Fail!\n", NXP_TDA18257_ADDRESS, *pAddr, WriteLen));
			return !TM_OK;
		}
    /* ...*/
    /* End of Customer code here */

}

//*--------------------------------------------------------------------------------------
//* Function Name       : UserWrittenWait
//* Object              : 
//* Input Parameters    : 	tmUnitSelect_t tUnit
//* 						UInt32 tms
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
static tmErrorCode_t UserWrittenWait(tmUnitSelect_t tUnit, UInt32 tms)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;

    /* Customer code here */
    /* ...*/
	x_thread_delay(tms);
    /* ...*/
    /* End of Customer code here */

    return err;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : UserWrittenPrint
//* Object              : 
//* Input Parameters    : 	UInt32 level, const char* format, ...
//* 						
//* Output Parameters   : tmErrorCode_t.
//*--------------------------------------------------------------------------------------
static tmErrorCode_t UserWrittenPrint(UInt32 level, const char* format, ...)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;

    /* Customer code here */
    /* ...*/
	mcDBG_MSG_L1(("%s\n",format));
    /* ...*/
    /* End of Customer code here */

    return err;
}


