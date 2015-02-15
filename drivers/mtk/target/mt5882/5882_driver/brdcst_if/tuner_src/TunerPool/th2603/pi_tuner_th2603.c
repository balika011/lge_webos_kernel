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
 * $RCSfile: pi_tuner_th2603.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_tuner_th2603.c
 *  Tuner control for NuTune TH2603.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "tuner_interface_if.h"
#ifndef CC_MT5175
#include "pi_def_dvbt.h"
#endif
#include "PI_Def.h"
#include "ctrl_bus.h"
#include "x_gpio.h"
#ifndef CC_MT5175
#include "pi_dvbt_if.h" 
#endif
#include "fe_tuner_common_if.h"
#include "pi_demod_atd.h"
#include "drvcust_if.h"
#include "eq_script_th2603.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define C_TH2603_VERSION                "TH2603 2010-01-01\n 2011-05-17\n v1.2"

#define C_TH2603_BASE                  ((UINT16) 0xC2)
#define C_TH2603_IF_FREQUENCY          ((UINT16) 36000)  /* kHz */
#define C_TH2603_IF_FREQUENCY_LO_STEP  ((UINT16) 900)    /* 100Hz */
#define C_TH2603_IF_FREQUENCY_ANA      ((UINT16) 38875)  /* kHz */
#define C_TH2603_IF_FREQUENCY_L1       ((UINT16) 33400)  /* kHz */
#define C_TH2603_IF_FREQUENCY_I        ((UINT16) 39500)  /* kHz */
#define C_TH2603_LO_DIVIDER_STEP       ((UINT32) 166670) /* Hz */
#define C_TH2603_LO_DIVIDER_STEP_ANA   ((UINT16) 50000)  /* Hz */
#define C_TH2603_LO_DIVIDER_STEP_C     ((UINT16) 50000)  /* Hz */

// DVB-T+DVB-C D-only Tuner
#define ccTUNER_LOCK_CONF_THRESHOLD     0
#define cALC_ADC_COMM_VOLT              1.2
#define cALC_ADC_INPUT_SWING            2  //Volt
#define cALC_ADC_BIAS                   (cALC_ADC_COMM_VOLT - cALC_ADC_INPUT_SWING/2)

#define C_TH2603_TOP_SET_T              ((U8)   0x05)  //100dBuV, from Benson
#define C_TH2603_TOP_SET_C              ((U8)   0x05)  //100dBuV
//#define C_TH2603_TOP_SET                ((U8)   0x06)  //external AGC
//#define C_TH2603_TOP_SET_ANA           ((U8)   0x03)  //106dBuV
//#define C_TH2603_TOP_SET               ((U8)   0x06)  //EXTERNAL
#define C_TH2603_TOP_SET_ANA            ((U8)   0x06)  //EXTERNAL
#ifndef CC_MT5175
#define C_TH2603_SAW_BW                 SAW_BW_8M 
#endif
#define C_TH2603_AGC_IF_SLP             0.75
#define C_TH2603_AGC_IF_INT             -2
#define C_TH2603_AGC_IF_MAX             0.998
#define C_TH2603_AGC_IF_MIN             -1

#define C_TH2603_MAX_IF_GAIN_SET        -0.12   
#define C_TH2603_MIN_IF_GAIN_SET        -0.38   
#define C_TH2603_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_TH2603_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_TH2603_MAX_IF_GAIN_POW        -100    // dBm
#define C_TH2603_MIN_IF_GAIN_POW        -50     // dBm
#define C_TH2603_MAX_RF_GAIN_POW        C_TH2603_MIN_IF_GAIN_POW
#define C_TH2603_MIN_RF_GAIN_POW        -20     // dBm
#define C_TH2603_POWER_CALIBRATE        26     // dBm

#define C_TH2603_AGC_COEF               ((U8)   0xBD)
#ifndef CC_MT5175
#define C_TH2603_DEMOD_INPUT_POWER      cIF_TARGET_LEVEL_0CCC
#endif
#define C_TH2603_I2C_DIVIDER       ((U16) 0x100)
// ********************************************* //

#define C_U8_TH2603_AGC_IF_SLP          (S8)(C_TH2603_AGC_IF_SLP *64 )
#define C_U8_TH2603_AGC_IF_INT          (S8)(C_TH2603_AGC_IF_INT *16 )
#define C_U8_TH2603_AGC_IF_MAX          (S8)(C_TH2603_AGC_IF_MAX *128)
#define C_U8_TH2603_AGC_IF_MIN          (S8)(C_TH2603_AGC_IF_MIN *128)

#define C_U8_TH2603_MAX_IF_GAIN_SET     (S8)(C_TH2603_MAX_IF_GAIN_SET *256)
#define C_U8_TH2603_MIN_IF_GAIN_SET     (S8)(C_TH2603_MIN_IF_GAIN_SET *256)
#define C_U8_TH2603_MAX_RF_GAIN_SET     (S8)((C_TH2603_MAX_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_TH2603_MIN_RF_GAIN_SET     (S8)((C_TH2603_MIN_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_TH2603_IF_GAIN_RANGE       (S8)(C_TH2603_MIN_IF_GAIN_POW - C_TH2603_MAX_IF_GAIN_POW)
#define C_U8_TH2603_RF_GAIN_RANGE       (S8)(C_TH2603_MIN_RF_GAIN_POW - C_TH2603_MAX_RF_GAIN_POW)

#define C_TH2603_FREQ_DBOUND_UPPER   858*1000*1000     //modified for DVB-C application, Menghau, 091020
#define C_TH2603_FREQ_DBOUND_LOWER    51*1000*1000
#define C_TH2603_FREQ_DBOUND_UPPER_Ana   865*1000*1000
#define C_TH2603_FREQ_DBOUND_LOWER_Ana    44*1000*1000
#define C_TH2603_CHANNEL_SCAN_JUMP_SMALL_STEP ((U16) 250)
#define C_TH2603_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP ((U16) 1500)
#define C_TH2603_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN ((U16) 500)
#define C_U8_TH2603_AGC_IF_LVL_MAX     0x12//18  //no signal
#define C_U8_TH2603_AGC_IF_LVL_MIN     0x08 //8  //-70dbm input

STATIC UINT8* pTh2603EqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
STATIC UINT8* pTh2603EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
STATIC UINT8* pTh2603EqStrong[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};

//Tuner releated ATD register
#if defined(CC_MT5396)  || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398)
STATIC ATD_TUNER_REL_REG_T arrAtdRelReg[]=
{
    {0x7d4,0x02},
    {0x7d5,0xc6},
    {0x7d6,0xc6},
    {0x7d7,0xee},
    {0x7d8,0xb6},
    {0x7d9,0xb6},
    {0x7da,0x7f},
    {0x7db,0x7f},
    {0x7dc,0x80},
    {0x7dd,0xf0},
    {0x7de,0xf0},
    {0x7ca,0x99},
    {0x0,0x0}//end flag
 };
STATIC CN_TUNER_REL_REG_T arrCNDTMBRelReg[]=
{
	{0xfd4,0x04},
    {0xfd5,0x00},
    {0xfd6,0x00},
    {0xfd7,0xec},
    {0xfd8,0xef},
    {0xfd9,0xef},
    {0xfda,0x7f},
    {0xfdb,0x7f},
    {0xfdc,0x80},
    {0xfdd,0x80},
    {0xfde,0x80},
    {0xfdf,0x00},
    {0x39d,0x3b},//for code power on
    {0x0,0x0}//end flag
 };

STATIC CN_TUNER_REL_REG_T arrCNDVBCRelReg[]=
{
    {0xfd4,0x04},
    {0xfd5,0x00},
    {0xfd6,0x00},
    {0xfd7,0xec},
    {0xfd8,0xef},
    {0xfd9,0xef},
    {0xfda,0x7f},
    {0xfdb,0x7f},
    {0xfdc,0x80},
    {0xfdd,0x80},
    {0xfde,0x80},
    {0xfdf,0x00},
    {0x0,0x0}//end flag
 };

#else
STATIC ATD_TUNER_REL_REG_T arrAtdRelReg[]=
{
	{0x2ca,0x99},
	{0x0,0x0}//end flag
};
#endif

//----------------------------------------------------------------------------- 
/** TH2603_TunerInit
 *  Tuner initialzation for TH2603.
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
void TH2603_TunerInit(ITUNER_CTX_T *pTCtx)
{
    ITUNER_CTX_T *psTunerCtx = pTCtx;
    SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
    SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);    

    psTunerCtx->I2cAddress = C_TH2603_BASE;
    psTunerCtx->u2IF_Freq = C_TH2603_IF_FREQUENCY;
    psTunerCtx->u2IF_Freq_A = C_TH2603_IF_FREQUENCY_ANA; //EC_code_dA402: Add for MT5365 SAWLess ATD IF Freq. Info. 
    psTunerCtx->u4RF_Freq = 0;
    psTunerCtx->u1AtdPatchSwitch = (MASK_PATCH_CR|MASK_PATCH_SENS|MASK_PATCH_CCI);
    psTunerCtx->fgRFTuner=TRUE;//Sawless tuner

    // iven 20110706, add for polarity if setting for NA reference
    psTunerCtx->bSpecPolarity = 0;
    // iven 20110706, add for legacy tuner as default fixed PGA gain index. Don care for Sawless
    psTunerCtx->uSLD_DefaultPGAIndex = 0;		    

    // AGC para
    psTunerCtx->sAtdAgcPara.AgcRfBias = 0x2f; //EC_code_d9930: Change 0x151 value for EU new BPF
    psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x80;
    psTunerCtx->sAtdAgcPara.AgcIfMin  = 0xe3; //EC_code_d9930: Change 0x158 value for EU new BPF
    psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B; //EC_code_d9930: Change 0x15F value for EU new BPF

	psTunerCtx->u2LO_Step=C_TH2603_LO_DIVIDER_STEP_ANA/10;//PLL stepsize(unit is 10Hz) 
    pEUCtx->u2LO_Step=psTunerCtx->u2LO_Step;//for single-system
	
	#ifndef CC_MT5175
    pEUCtx->m_SAW_BW  = C_TH2603_SAW_BW;
	#endif
    //pEUCtx->m_Ana_Top = C_TH2603_TOP_SET_ANA;
    pEUCtx->m_s4FreqBoundUpper = C_TH2603_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_TH2603_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_TH2603_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_TH2603_FREQ_DBOUND_LOWER_Ana;
    
    pEUCtx->m_Small_Step = C_TH2603_CHANNEL_SCAN_JUMP_SMALL_STEP;
    pEUCtx->m_Middle_Step_Up = C_TH2603_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP;
    pEUCtx->m_Middle_Step_Down = C_TH2603_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN;
    
    // add by JC, 081215, for Signal Level display
    pEUCtx->m_ifAgc_lvl_max = (U8)C_U8_TH2603_AGC_IF_LVL_MAX;
    pEUCtx->m_ifAgc_lvl_min = (U8)C_U8_TH2603_AGC_IF_LVL_MIN;

    pEUCtx->m_SigLvTh = 0;//C_TDTWS710D_SIGNAL_LVL_TH;
    pEUCtx->m_SigLvScan = 0;

    pEUCtx->m_aucPara[0] = C_TH2603_TOP_SET_T;
    pEUCtx->m_aucPara[1] = 0;

    pEUCtx->m_aucPara[2] = (U8)C_U8_TH2603_AGC_IF_SLP;
    pEUCtx->m_aucPara[3] = (U8)C_U8_TH2603_AGC_IF_INT;
    pEUCtx->m_aucPara[4] = (U8)C_U8_TH2603_AGC_IF_MAX;
    pEUCtx->m_aucPara[5] = (U8)C_U8_TH2603_AGC_IF_MIN;

// Add for US demod in SLD PRA setting, iven 20110602
    pUSCtx->m_aucPara_SLD[2] = (U8)C_U8_TH2603_AGC_IF_SLP;
    pUSCtx->m_aucPara_SLD[3] = (U8)C_U8_TH2603_AGC_IF_INT;    
    pUSCtx->m_aucPara_SLD[4] = (U8)C_U8_TH2603_AGC_IF_MAX;    
    pUSCtx->m_aucPara_SLD[5] = (U8)C_U8_TH2603_AGC_IF_MIN;        
  

    pEUCtx->m_aucPara[6] = C_TH2603_AGC_COEF;
	#ifndef CC_MT5175
    pEUCtx->m_aucPara[7] = C_TH2603_DEMOD_INPUT_POWER;
	#endif

    pEUCtx->m_aucPara[ 8] = (U8)C_U8_TH2603_MAX_IF_GAIN_SET;
    pEUCtx->m_aucPara[ 9] = (U8)C_U8_TH2603_MIN_IF_GAIN_SET;
    pEUCtx->m_aucPara[10] = (U8)C_U8_TH2603_MAX_RF_GAIN_SET;
    pEUCtx->m_aucPara[11] = (U8)C_U8_TH2603_MIN_RF_GAIN_SET;
    pEUCtx->m_aucPara[12] = (U8)C_U8_TH2603_IF_GAIN_RANGE;
    pEUCtx->m_aucPara[13] = (U8)C_U8_TH2603_RF_GAIN_RANGE;
    pEUCtx->m_aucPara[14] = C_TH2603_POWER_CALIBRATE;

    pTh2603EqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQTh2603BG;
    pTh2603EqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQTh2603DK;
    pTh2603EqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQTh2603I;
    pTh2603EqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQTh2603L;
    pTh2603EqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQTh2603L1; 
    pTh2603EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN] = EQTh2603M;
    
    pTh2603EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQTh2603BGWeak;//EQTH2603BG;
    pTh2603EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQTh2603DKWeak;//EQTH2603DK;
    pTh2603EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQTh2603IWeak;//EQTH2603I;
    pTh2603EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQTh2603LWeak;//EQTH2603L;
    pTh2603EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQTh2603L1Weak;//EQTH2603L1;
    pTh2603EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN] = EQTh2603MWeak;//EQTH2603M;

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398)
   pTh2603EqStrong[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQTh2603BGStrong;//EQTH2603BG;
    pTh2603EqStrong[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQTh2603DKStrong;//EQTH2603DK;
    pTh2603EqStrong[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQTh2603IStrong;//EQTH2603I;
    pTh2603EqStrong[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQTh2603LStrong;//EQTH2603L;
    pTh2603EqStrong[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQTh2603L1Strong;//EQTH2603L1;
    pTh2603EqStrong[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN] = EQTh2603MStrong;//EQTH2603M;
#endif

    mcSHOW_USER_MSG(("Tuner TH2603 init done!\n"));     
}


//----------------------------------------------------------------------------- 
/** TH2603_TunerSetFreq
 *  Set Tuner PLL for TH2603 to tune RF frequency.
 *  @param  pTCtx     Pointer of pi tuner driver context.
 *  @param  freq        RF center frequency in KHz.
 *  @param  mode                    
 *  @param  autoSearch  
 *  @param  step            
 *  @retval 
 */
//-----------------------------------------------------------------------------
INT16 TH2603_TunerSetFreq(
    ITUNER_CTX_T *pTunerCtx,
    PARAM_SETFREQ_T* param
  )
{
    UCHAR uc_data;
    UCHAR TunerS[6];
    S16 ucTunerLockConfCnt;
    UINT32 Lo;
    UINT16 Ndivider,CB11,CB12,CB2;
    S16 ii, jj;
    UINT8 Mode;
    UINT8 autoSearch;
    UINT32 Freq;
    SPECIFIC_MEMBER_EU_CTX* pEUCtx; //= &( pTunerCtx->specific_member.eu_ctx);    

    autoSearch=param->fgAutoSearch;
    Freq = param->Freq;
    pEUCtx= &( pTunerCtx->specific_member.eu_ctx);    

    mcSHOW_DBG_MSG(("TH2603_TunerSetFreq\n"));
      
    //GPIO_AGC_SWITCH = DRVCUST_OptGet(eTunerCtrlAna);
    
    Mode = param->Modulation;
    pTunerCtx->u4RF_Freq = Freq;
    pTunerCtx->I2cAddress = C_TH2603_BASE;

    if((Mode <= MOD_DVBT) ||(Mode == MOD_DTMB)) //DVB-T reception
    {
        Lo = Freq + C_TH2603_IF_FREQUENCY; // kHz
        Ndivider = (U16) ( (Lo*1000+C_TH2603_LO_DIVIDER_STEP/2) / C_TH2603_LO_DIVIDER_STEP);

        if ((Freq < (C_TH2603_FREQ_DBOUND_LOWER/1000)) || (Freq > (C_TH2603_FREQ_DBOUND_UPPER/1000)))
        {
            mcSHOW_DRVERR_MSG(("Out of range for LO!\n"));
            //return (+1);
        }

        //CB11 = 0xCA;
        CB11 = 0xC2;   // b[3]=0, XT OFF for 4M spur from CJ
        if (autoSearch)
            CB12 = 0x88;
        else
            CB12 = 0x80;

        //CB12 = CB12 | 0x06  ;  //USE EXTERNAL AGC CONTROL FOR DVB       /BENSON REQUEST
        //CB12 = CB12|(pEUCtx->m_aucPara[0]) ; //  TOP setting 00000xxx
        CB12 = CB12|C_TH2603_TOP_SET_T; //  TOP setting 00000xxx

        if (Lo < 127000)
        {
            CB2 = 0x41;
        }
        else if (Lo < 151000)
        {
            CB2 = 0x61;
        }
        else if (Lo < 176000)
        {
            CB2 = 0x81;
        }
        else if (Lo < 193000)
        {
            CB2 = 0xA1;
        }
        else if (Lo < 337000)
        {
            CB2 = 0x42;
        }
        else if (Lo < 394000)
        {
            CB2 = 0x62;
        }
        else if (Lo < 426000)
        {
            CB2 = 0x82;
        }
        else if (Lo < 457000)
        {
            CB2 = 0xA2;
        }
        else if (Lo < 480000)
        {
            CB2 = 0xc2;
        }
        else if (Lo < 692000)
        {
            CB2 = 0x64;
        }
        else if (Lo < 812000)
        {
            CB2 = 0x84;
        }
        else if (Lo < 890000)
        {
            CB2 = 0xA4;
        }
        else
        {
            CB2 = 0xA4;   //for th2603
        }
        //CB12=0x81;  //ATC=0 is normal 
        //CB12 = (CB12&0xF8)|(( pEUCtx->m_aucPara[0]&0x07));  // 0x ****XXXX     

        //if(!autoSearch)
     //   {
       //     CB12 = CB12 & 0xF7; // Be sure ATC = 0  if it is not autosearch
       // }
    }
    else if(Mode == MOD_DVBC) //DVB-C reception
    {
        Lo = Freq + C_TH2603_IF_FREQUENCY; // kHz
        Ndivider = (U16) ( (Lo*1000+C_TH2603_LO_DIVIDER_STEP_C/2) / C_TH2603_LO_DIVIDER_STEP_C);

        if ((Freq < (C_TH2603_FREQ_DBOUND_LOWER/1000)) || (Freq > 
(C_TH2603_FREQ_DBOUND_UPPER/1000)))
        {
            mcSHOW_DRVERR_MSG(("Out of range for LO!\n"));
            //return (+1);
        }
        //CB11 = 0xCB;
        CB11 = 0xC3;   // b[3]=0, XT OFF for 4M spur from CJ
        if (autoSearch)
            CB12 = 0x88;
        else
            CB12 = 0x80;

        //CB12 = CB12 | 0x06  ;  //USE EXTERNAL AGC CONTROL FOR DVB       /BENSON REQUEST
        //CB12 = CB12|(pEUCtx->m_aucPara[0]) ; //  TOP setting 00000xxx
        CB12 = CB12|C_TH2603_TOP_SET_C; //  TOP setting 00000xxx

        if (Lo < 127000)
        {
            CB2 = 0x81;
        }
        else if (Lo < 167000)
        {
            CB2 = 0xA1;
        }
        else if (Lo < 177000)
        {
            CB2 = 0xC1;
        }
        else if (Lo < 196875)
        {
            CB2 = 0xE1;
        }
        else if (Lo < 307000)
        {
            CB2 = 0x82;
        }
        else if (Lo < 387000)
        {
            CB2 = 0xA2;
        }
        else if (Lo < 436000)
        {
            CB2 = 0xC2;
        }
        else if (Lo < 482875)
        {
            CB2 = 0xE2;
        }
        else if (Lo < 637000)
        {
            CB2 = 0x84;
        }
        else if (Lo < 737000)
        {
            CB2 = 0xA4;
        }
        else if (Lo < 837000)
        {
            CB2 = 0xC4;
        }
        else if (Lo < 903000)
        {
            CB2 = 0xE4;
        }
        else
        {
            CB2 = 0xE4;
        }
    }
    else //analog reception
    {
        if (Mode == MOD_SECAM_L1)
        {
            Lo = Freq + C_TH2603_IF_FREQUENCY_L1; // kHz
        }
        else if (Mode == MOD_PAL_I)
        {
            Lo = Freq + C_TH2603_IF_FREQUENCY_I; // kHz
        }
        else
        {
            Lo = Freq + C_TH2603_IF_FREQUENCY_ANA; // kHz
        }

        pTunerCtx->u2IF_Freq_A = Lo - Freq;
        Ndivider = (U16) ( (Lo*1000+C_TH2603_LO_DIVIDER_STEP_ANA/2) / C_TH2603_LO_DIVIDER_STEP_ANA);
        mcSHOW_DBG_MSG(("Freq = %d, Divider ratio = %04X PLL set freq=%d kHz\n",pTunerCtx->u4RF_Freq, Ndivider,(Ndivider*50000/1000-38900)));

        if (Freq < C_TH2603_FREQ_DBOUND_LOWER_Ana/1000 || Freq > C_TH2603_FREQ_DBOUND_UPPER_Ana/1000)
        {
            mcSHOW_DBG_MSG(("Out of range for LO!\n"));
            //return (+1);
        }

        //CB11 = 0xCB;
        CB11 = 0xC3;   // b[3]=0, XT OFF for 4M spur from CJ
        if (autoSearch)
            CB12 = 0x8E;
        else
            CB12 = 0x86;

        //CB12 = CB12|(pTunerCtx->m_aucPara[0]) ; //  TOP setting 00000xxx
        CB12 = CB12|C_TH2603_TOP_SET_ANA;
        if (Lo < 169000)
        {
            CB2 = 0x01;
        }
        else if (Lo < 179000)
        {
            CB2 = 0x21;
        }
        else if (Lo < 189000)
        {
            CB2 = 0x41;
        }
        else if (Lo < 196150)
        {
            CB2 = 0x61;
        }
        else if (Lo < 406000)
        {
            CB2 = 0x02;
        }
        else if (Lo < 438000)
        {
            CB2 = 0x22;
        }
        else if (Lo < 461000)
        {
            CB2 = 0x42;
        }
        else if (Lo < 482150)
        {
            CB2 = 0x62;
        }
        else if (Lo < 750000)
        {
            CB2 = 0x04;
        }
        else if (Lo < 817000)
        {
            CB2 = 0x24;
        }
        else if (Lo < 862000)
        {
            CB2 = 0x44;
        }
        else if (Lo < 882000)
        {
            CB2 = 0x64;
        }
        else if (Lo < 895000)
        {
            CB2 = 0x84;
        }
        else
        {
            CB2 = 0xA4;
        }
    }

    // LO settings
    TunerS[0] = (UCHAR) 0x7F & (Ndivider >> 8);
    TunerS[1] = (UCHAR) 0xFF & Ndivider;
    TunerS[2] = (UCHAR) CB11;
    TunerS[3] = (UCHAR) CB2;
    TunerS[4] = (UCHAR) CB12;
    TunerS[5] = (UCHAR) CB2;

    //pTunerCtx->u4LO_Freq = Lo;

    for (jj=0; jj<2; jj++)
    {        
        if (ICtrlBus_I2cTunerWrite(C_TH2603_I2C_DIVIDER,pTunerCtx->I2cAddress, TunerS,6) > 0)
        {
            return (-1);
        }

        ucTunerLockConfCnt = 0;
        for (ii=0; ii<20; ii++)
        {
            mcDELAY_MS(5);
            ICtrlBus_I2cTunerRead(C_TH2603_I2C_DIVIDER,pTunerCtx->I2cAddress, &uc_data,1);

            if (mcGET_FIELD(uc_data, 0x40, 6))  // if PLL lock
                ucTunerLockConfCnt++;
            else
                ucTunerLockConfCnt = 0;

            if (ucTunerLockConfCnt>ccTUNER_LOCK_CONF_THRESHOLD)
                break;
        }
        ucTunerLockConfCnt = ii;

        if(ii < 20)
        {
            mcSHOW_DBG_MSG(("Freq = %d MHz, the %d-th time setting PLL success, cnt = %d\n" , Freq, jj+1, ucTunerLockConfCnt)); 
            break;
        }
        else
        {
            mcSHOW_DBG_MSG(("Freq = %d MHz, the %d-th time setting PLL fail\n" , Freq, jj+1));
        }
    }

    mcSHOW_DBG_MSG(("LO_%02X: %02X-%02X-%02X-%02X-%02X-%02X\n",pTunerCtx->I2cAddress, TunerS[0], TunerS[1], TunerS[2], TunerS[3],TunerS[4],TunerS[5]));

    pEUCtx->m_aucPara[1]  = TunerS[0];   //send register value to UI, LC 070102
    pEUCtx->m_aucPara[15] = TunerS[1];
    pEUCtx->m_aucPara[16] = TunerS[2];
    pEUCtx->m_aucPara[17] = TunerS[3];
    pEUCtx->m_aucPara[18] = TunerS[4];
    pEUCtx->m_aucPara[19] = TunerS[5];

    return (0);
}
//----------------------------------------------------------------------------- 
/** TH2603_TunerGetStatus
 *  Set Tuner PLL staus for TH2603
 *  @param  
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
char *TH2603_TunerGetVer(void)
{
    return ((CHAR*)C_TH2603_VERSION);
}
//----------------------------------------------------------------------------- 
/** TH2603_SetSawBw
 *  Set Tuner PLL staus for TH2603
 *  @param  
 *  @retval tuner Saw BandWidth
 */
//-----------------------------------------------------------------------------
void TH2603_SetSawBw(ITUNER_CTX_T* pTunerCtx, UINT8 sawbw)
{
    pTunerCtx->specific_member.eu_ctx.m_SAW_BW = sawbw;
}
#ifndef CC_MT5175
//----------------------------------------------------------------------------- 
/** TH2603_SetSSICondition
 *  Set SSI Condition for TH2603
 *  @param  
 *  @retval void
 */
//-----------------------------------------------------------------------------
void TH2603_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);
    pTunerCtx->sSSIPara.i2Ssi_RF= *((INT16 *) pInOutVal+2);
    pTunerCtx->sSSIPara.i2Ssi_IF= *((INT16 *) pInOutVal+3);
    pTunerCtx->sSSIPara.i2Ssi_BB= *((INT16 *) pInOutVal+4);
}

//----------------------------------------------------------------------------- 
/*
 *  TH2603_GetSSI
 *  calculate Signal Strength Indicator.
 *  @param  psDemodCtx  Pointer of context created by DVBT_DemodCtxCreate.
 *  @retval   
 */
//-----------------------------------------------------------------------------
UINT32 TH2603_GetSSI(ITUNER_CTX_T* pTunerCtx) 
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
    INT32 i4Power_rec = 0;
    INT32 i4Power_rel = 0;
    INT16 i2Ssi_RF = 0;
    INT16 i2Ssi_IF = 0;
    INT16 i2Ssi_BB = 0;
    UINT32 u4SSI_Indicator = 0;
    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);

    // initial level for best mux, Ken, 100607
    pCtx->m_SigLvScan = 0;
    
    /* Get Power Ref Value */  
    mcSHOW_DBG_MSG(("DVBT: Mod=%d, Code Rate=%d, RFAGC=%d, IFAGC=%d, BBAGC=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR,
	                                                              pTunerCtx->sSSIPara.i2Ssi_RF,pTunerCtx->sSSIPara.i2Ssi_IF,pTunerCtx->sSSIPara.i2Ssi_BB));

    /* Get Pre Value */    
    i2Ssi_RF = pTunerCtx->sSSIPara.i2Ssi_RF;    
    i2Ssi_IF = pTunerCtx->sSSIPara.i2Ssi_IF;    
    i2Ssi_BB = pTunerCtx->sSSIPara.i2Ssi_BB;	
    /* Get Power_Rec Value */
    if(i2Ssi_RF <= 115)
    {
        i4Power_rec = -5*(i2Ssi_RF-115)/12 -10; 
    }
    else if((i2Ssi_RF > 115) && (i2Ssi_RF <= 135))
    {
        i4Power_rec = -5*(i2Ssi_RF-135)/11 - 20;
    }
    else if((i2Ssi_RF > 135) && (i2Ssi_RF <= 199))
    {
        i4Power_rec = -5*(i2Ssi_RF-199)/41 - 30;
    }
	else if((i2Ssi_RF > 199) && (i2Ssi_RF <= 318))
    {
        i4Power_rec = -5*(i2Ssi_RF-318)/63 - 40;
    }
	else if((i2Ssi_RF > 318) && (i2Ssi_RF <= 384))
    {
        i4Power_rec = -5*(i2Ssi_RF-384)/32 - 50;
    }
	else if((i2Ssi_RF > 384) && (i2Ssi_RF <= 423))
    {
        i4Power_rec = -5*(i2Ssi_RF-423)/39 - 55;
    }
//    else if((i2Ssi_RF >= 540) && (i2Ssi_IF >= 8) && (i2Ssi_IF < 18))
    else if((i2Ssi_RF > 423) && (i2Ssi_IF < 18))
    {
        i4Power_rec = -5*(i2Ssi_IF-18)/4 - 65;
    }
    else if((i2Ssi_RF >= 515) && (i2Ssi_IF >= 18) && (i2Ssi_BB > 52) && (i2Ssi_BB <= 240))
    {
//        i4Power_rec = 5*(i2Ssi_BB-52)/37 - 75;
        i4Power_rec = 5*(i2Ssi_BB-52)/43 - 75;
    }
    else if((i2Ssi_RF >= 515) && (i2Ssi_IF >= 18) && (i2Ssi_BB > 21) && (i2Ssi_BB <= 52))
    {
        i4Power_rec = 5*(i2Ssi_BB-21)/11 - 85;
    }
    else if((i2Ssi_RF >= 515) && (i2Ssi_IF >= 18) && (i2Ssi_BB > 0) && (i2Ssi_BB <= 21))
    {
        i4Power_rec = 5*(i2Ssi_BB-16)/5 - 90;
    }
    else
    {
        i4Power_rec = -120;
    }

	/*Get Ref Value-Start*/
	if(pTunerCtx->sSSIPara.i2Mod==DVBT_Qam_Mode_Unknow)
	{
		mcSHOW_DBG_MSG1(("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
		return u4SSI_Indicator;
	}
	else
	{
		i4Power_ref = Power_Ref[(pTunerCtx->sSSIPara.i2Mod)*DVBT_CR_Mode_Size + pTunerCtx->sSSIPara.i2CR];
	}


    
    /* Get Power Rel Value */
    i4Power_rel = i4Power_rec - i4Power_ref;
	mcSHOW_DBG_MSG(("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
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
    pCtx->m_SigLvScan = (S8) u4SSI_Indicator;
    
    return u4SSI_Indicator;
}
#endif
//----------------------------------------------------------------------------- 
/** TH2603_TunerOP
 *  Set Tuner PLL staus for TH2603
 *  @param  
 *  @retval tuner OP
 */
//-----------------------------------------------------------------------------
INT16 TH2603_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    switch (eOperation)
    {
    case itGetVer:
        {
            CHAR ** RetStr=(CHAR **)pInOutVal;
          *RetStr=TH2603_TunerGetVer();
        }
        break;    

    case itSetTop:
    {
          SPECIFIC_MEMBER_EU_CTX * pTunerCtx= &(pTCtx->specific_member.eu_ctx);
          pTunerCtx->m_aucPara[0] = SetVal;
    }
        break;

    case itSetReg:
         {
        
         UCHAR TunerS[5];
         UCHAR uc_data;
         UINT8 *ptrWord = (UINT8 *)pInOutVal;
         int ii,jj;
         int ucTunerLockConfCnt;
    
         SPECIFIC_MEMBER_EU_CTX * pTunerCtx= &(pTCtx->specific_member.eu_ctx);
         
         for(ii = 0; ii < 5; ii++)
            {
               TunerS[ii]=(UCHAR)(*ptrWord);
               ptrWord++;
               //mcSHOW_DRVERR_MSG(("The register word is: 0x%x\n",TunerS[ii]));
               // mcDELAY(C_ENV57S_PLL_POLL_INTERVAL);
            }
           mcSHOW_DRVERR_MSG(("The register words are: 0x%x-0x%x-0x%x-0x%x-0x%x\n",TunerS[0],TunerS[1],TunerS[2],TunerS[3],TunerS[4]));
        
          
          for (jj=0; jj<2; jj++)
              {
                if (ICtrlBus_I2cTunerWrite(C_TH2603_BASE,pTCtx->I2cAddress, TunerS,5) > 0)
                {
                    return (-1);
                }

                  pTunerCtx->m_aucPara[1] = (UCHAR)TunerS[0]; //send register value to UI, LC 070102
          pTunerCtx->m_aucPara[15] = (UCHAR)TunerS[1];
          pTunerCtx->m_aucPara[16] = (UCHAR)TunerS[2];
          pTunerCtx->m_aucPara[17] = (UCHAR)TunerS[3];
          pTunerCtx->m_aucPara[18] = (UCHAR)TunerS[4]; 
                ucTunerLockConfCnt = 0;
              for (ii=0; ii<20; ii++)
              {
              	mcDELAY_MS(5);;// wait PLL lock
                ICtrlBus_I2cTunerRead(C_TH2603_BASE,pTCtx->I2cAddress, &uc_data,1);
                if (mcGET_FIELD(uc_data, 0x40, 6))  // if PLL lock
                  ucTunerLockConfCnt++;
                else
                  ucTunerLockConfCnt = 0;
                if (ucTunerLockConfCnt>ccTUNER_LOCK_CONF_THRESHOLD)
                  break;
              }
              ucTunerLockConfCnt = ii;
              if(ii < 20)
              {
                mcSHOW_DRVAPI_MSG(("the %d-th time setting PLL success, cnt = %d\n" , jj+1, ucTunerLockConfCnt));
                break;
              }
              else
              {
                mcSHOW_DRVAPI_MSG(("the %d-th time setting PLL fail\n" , jj+1));
              }
          
            }
          
        }
        break;
        
    case itGetReg:
        {            
                int i = 0;
            SPECIFIC_MEMBER_EU_CTX * pTunerCtx= &(pTCtx->specific_member.eu_ctx);
                *((INT8 *) pInOutVal)   = pTunerCtx->m_aucPara[1]; 
          *((INT8 *) pInOutVal+1) = pTunerCtx->m_aucPara[15];
          *((INT8 *) pInOutVal+2) = pTunerCtx->m_aucPara[16];
                *((INT8 *) pInOutVal+3) = pTunerCtx->m_aucPara[17];
          *((INT8 *) pInOutVal+4) = pTunerCtx->m_aucPara[18];
          for (i = 0; i < 5; i++)
                  mcSHOW_DRVERR_MSG(("The register word is: 0x%x\n",*((INT8 *)pInOutVal+i)));              
        }
        break;

    case itSetIfDemod:
        break;
    case itSetSawBw:
        TH2603_SetSawBw(pTCtx, SetVal);
        break;
    case itSetLNA:
        break;
    case itSetIfMin:
        {

        UINT8 subSysId = SetVal;
        UINT8 *ifMinPtr = (UINT8*)pInOutVal;
            
        mcSHOW_DBG_MSG(("TH2603 TOP Setting\n"));
        switch(subSysId)
            {
            case MOD_SECAM_L:
            case MOD_SECAM_L1:
                *ifMinPtr = 0x00;
                break;
            default:
                *ifMinPtr = 0xF0;
                break;
            }
        }
        break;

    case itGetTHC:
        {

        UINT8 subSysId = SetVal;
        UINT8 *ifThcPtr = (UINT8*)pInOutVal;
            
        mcSHOW_DBG_MSG(("TH2603 TOP Setting\n"));
        switch(subSysId){
            case MOD_SECAM_L:
            case MOD_SECAM_L1:
                *ifThcPtr = 0x20;
                break;
            default:
                *ifThcPtr = 0x1A;
                break;
        }
        }
        break;

    case itGetEqScriptNormal: 
        *(UINT8**)pInOutVal = pTh2603EqNormal[SetVal];
        break;
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pTh2603EqWeak[SetVal];
        break;
    case itGetEqScriptStrongChroma: 
        *(UINT8**)pInOutVal = pTh2603EqStrong[SetVal];
        break; 
#ifndef CC_MT5175
    case itSetSSICond:
        TH2603_SetSSICondition(pTCtx, pInOutVal);
        break;
    case itGetSSIIndex:
        *((UINT32*)pInOutVal) = TH2603_GetSSI(pTCtx);
        break;  
#endif
    case itGetAnaFreqBound:
        {
        TUNER_ANA_ATTRIBUTE_T   *psTunerAnaAttribute;

        psTunerAnaAttribute = (TUNER_ANA_ATTRIBUTE_T *) pInOutVal;
        psTunerAnaAttribute->ui4_lower_bound_freq = C_TH2603_FREQ_DBOUND_LOWER_Ana;
        psTunerAnaAttribute->ui4_upper_bound_freq = C_TH2603_FREQ_DBOUND_UPPER_Ana;
        }
        break;
   case itGetTunerRelReg://Get ATD Tuner Rel register setting
       {
            *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
        }
        break;
#if defined(CC_MT5396)  || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398)
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
    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}

