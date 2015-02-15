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
 * $RCSfile: pi_tuner_Si2151.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/* Silabs version 2.0 build 4 for Si2151-A20  8/20/2012 */
// changes tagged with Silabs_120820
/** @file pi_tuner_Si2151.c
 *  Tuner control for SS Si2151
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "vdr_PD_Def.h"
#include "vdr_pi_def_dvbt.h"
#include "eq_script_Si2151.h"

/*please add Si2151 head file here*/
#include "Si2151_L1_API.h"
#include "Si2151_L2_API.h"

#if 0
 enum
    {
        SAW_BW_6M = 0,
        SAW_BW_7M,
        SAW_BW_8M,
        SAW_BW_5M
    };

enum
{
    cIF_TARGET_LEVEL_0CCC = 0,
    cIF_TARGET_LEVEL_0A97,
    cIF_TARGET_LEVEL_09CC,
    cIF_TARGET_LEVEL_07CC,
    cIF_TARGET_LEVEL_0CCC_0D36
};

enum
{
    DVBT_CR_12 = 0,
    DVBT_CR_23,
    DVBT_CR_34,
    DVBT_CR_56,
    DVBT_CR_78,
    DVBT_CR_Mode_Size
};
#endif
#define DAGC_TARGET_LEVEL_H_INDEX 0
#define DAGC_TARGET_LEVEL_L_INDEX 1

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Frequency boundary .Get from tuner spec
#define C_Si2151_FREQ_DBOUND_UPPER       1002*1000*1000
#define C_Si2151_FREQ_DBOUND_LOWER       42*1000*1000
#define C_Si2151_FREQ_DBOUND_UPPER_Ana   870*1000*1000
#define C_Si2151_FREQ_DBOUND_LOWER_Ana   42*1000*1000

//PLL lock check parameters
#define C_Si2151_POLL_INTERVAL         5 //ms
#define C_Si2151_PLL_POLL_TIMETOUT     100 //ms
#define C_Si2151_PLL_POLL_CNT          C_Si2151_PLL_POLL_TIMETOUT/C_Si2151_POLL_INTERVAL //counter

#define C_Si2151_PF_TO_CF_SHIFT_BG_7M  2250 /*KHZ*/  //PF to CF is 2250 when 7M B
#define C_Si2151_PF_TO_CF_SHIFT_BG_8M  2750 /*KHZ*/  //PF to CF is 2750 when 8M G
#define C_Si2151_PF_TO_CF_SHIFT_DK     2750 /*KHZ*/
#define C_Si2151_PF_TO_CF_SHIFT_I      2750 /*KHZ*/
#define C_Si2151_PF_TO_CF_SHIFT_L      2750 /*KHZ*/
#define C_Si2151_PF_TO_CF_SHIFT_M      1750 /*KHZ*/
#define C_Si2151_PF_TO_CF_SHIFT_L1     2750 /*KHZ*/
#define C_Si2151_IF_CENTER_BG          5000 /*KHZ*/
#define C_Si2151_IF_CENTER_DK          5000 /*KHZ*/
#define C_Si2151_IF_CENTER_I           5000 /*KHZ*/
#define C_Si2151_IF_CENTER_L           5000 /*KHZ*/
#define C_Si2151_IF_CENTER_L1          5000 /*KHZ*/
#define C_Si2151_IF_CENTER_M           5000 /*KHZ*/

#define C_Si2151_IF_FREQUENCY          ((UINT16)  5000)  /* kHz */
#define C_Si2151_LO_ADDRESS            ((UINT8)  0xC0)

#define C_Si2151_LO_DIVIDER_STEP       ((UINT32) 1000)  /* Hz */
#define C_Si2151_LO_DIVIDER_STEP_ANA   ((UINT16)  1000)  /* Hz */
#define C_SILABS_Si2151_STR            "SILABS_Si2151_FW_0_A_Build_18\n 2014-04-10\n"

#define C_Si2151_TOP_SET               ((UINT8)   0x02)
#define C_Si2151_TOP_SET_DVBC          ((UINT8)   0x04)
#define C_Si2151_TOP_SET_ANA           ((UINT8)   0x0A)
#define C_Si2151_SAW_BW                SAW_BW_8M

#if SI2151_ENABLE_EXTERNAL_DTV_AGC_CONTROL
#define C_Si2151_AGC_IF_SLP            0.19
#define C_Si2151_AGC_IF_INT            -0.24
#define C_Si2151_AGC_IF_MAX            0.52
#define C_Si2151_AGC_IF_MIN            -1
#else
#define C_Si2151_AGC_IF_SLP            0.125
#define C_Si2151_AGC_IF_INT            0.5
#define C_Si2151_AGC_IF_MAX            0.499  // 0.5 will overflow  //0v
#define C_Si2151_AGC_IF_MIN            0.0
#endif
#define C_Si2151_AGC_IF_SLP_SGN        (INT8)((C_Si2151_AGC_IF_SLP>0)?1:-1)
#define C_Si2151_AGC_IF_LVL_MAX        0.4774    //-90dBm    127/256
#define C_Si2151_AGC_IF_LVL_MIN        0.08203125   //-60dBm  21/256

#define cALC_ADC_INPUT_SWING           2  //Volt
#define cALC_ADC_COMM_VOLT             1.2
#define cALC_ADC_BIAS                  (cALC_ADC_COMM_VOLT - cALC_ADC_INPUT_SWING/2)

#define C_Si2151_MAX_IF_GAIN_SET       -0.12
#define C_Si2151_MIN_IF_GAIN_SET       -0.38
#define C_Si2151_MAX_RF_GAIN_SET       cALC_ADC_BIAS *2   // Volt
#define C_Si2151_MIN_RF_GAIN_SET       cALC_ADC_BIAS *2   // Volt
#define C_Si2151_MAX_IF_GAIN_POW       -100    // dBm
#define C_Si2151_MIN_IF_GAIN_POW       -50     // dBm
#define C_Si2151_MAX_RF_GAIN_POW       C_Si2151_MIN_IF_GAIN_POW
#define C_Si2151_MIN_RF_GAIN_POW       -20     // dBm
#define C_Si2151_POWER_CALIBRATE       26     // dBm

#define C_Si2151_AGC_COEF              ((UINT8)   0xBD)
#define C_Si2151_DEMOD_INPUT_POWER     cIF_TARGET_LEVEL_0CCC_0D36

// ********************************************* //

#define C_U8_Si2151_AGC_IF_SLP         (INT8)(C_Si2151_AGC_IF_SLP *64 )  // MT5135(S(1, -6)), -0.125*64 = -8 (0xF8), -8/64 = -1/8
#define C_U8_Si2151_AGC_IF_INT         (INT8)(C_Si2151_AGC_IF_INT *16 ) // MT5135(S(3, -4))  0.5*16 = 8, 8/16 =0.5
#define C_U8_Si2151_AGC_IF_MAX         (INT8)(C_Si2151_AGC_IF_MAX *128)    //0.499*256 = 0x7F
#define C_U8_Si2151_AGC_IF_MIN         (INT8)(C_Si2151_AGC_IF_MIN *128)    //0
#define C_U8_Si2151_AGC_IF_LVL_MAX     (INT8)(C_Si2151_AGC_IF_LVL_MAX *256)
#define C_U8_Si2151_AGC_IF_LVL_MIN     (INT8)(C_Si2151_AGC_IF_LVL_MIN *256)

#define C_U8_Si2151_MAX_IF_GAIN_SET    (INT8)(C_Si2151_MAX_IF_GAIN_SET *256)
#define C_U8_Si2151_MIN_IF_GAIN_SET    (INT8)(C_Si2151_MIN_IF_GAIN_SET *256)
#define C_U8_Si2151_MAX_RF_GAIN_SET    (INT8)((C_Si2151_MAX_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_Si2151_MIN_RF_GAIN_SET    (INT8)((C_Si2151_MIN_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_Si2151_IF_GAIN_RANGE      (INT8)(C_Si2151_MIN_IF_GAIN_POW - C_Si2151_MAX_IF_GAIN_POW)
#define C_U8_Si2151_RF_GAIN_RANGE      (INT8)(C_Si2151_MIN_RF_GAIN_POW - C_Si2151_MAX_RF_GAIN_POW)

//--------------For AD internal attenuator-----------------------------------------------------------
#define AD_R_setting 0      // 1:have IF attenuator, 0:No IF attenuator
//-------------------------------------------------------------static  variable------------------------------------------------------------------------------------------------------------------
typedef enum TV_MODES
{
    ANALOG_MODE,
    DIGITAL_MODE
} TV_MODES;

typedef enum TUNER_STATUSES
{
    CHL,
    PCL,
    DTV_CHL,
    RSSI,
} TUNER_STATUSES;

/* Tuner Init variable */
L1_Si2151_Context api_context={0};
L1_Si2151_Context *api;
static BOOL TunerInited = FALSE;

/* Global variable */
UINT8 VideoStandard;

/* Tuner IF AMP variable */
#if SI2151_ENABLE_EXTERNAL_DTV_AGC_CONTROL
static unsigned char DVBT_IF_AMP       =55;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBT2_IF_AMP      =55;
static unsigned char DVBC_IF_AMP       =60;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char ATSC_8VSB_IF_AMP  = 70;
static unsigned char ATSC_QAM_IF_AMP   = 70;

#else
static unsigned char DVBT_IF_AMP       =21;//Si2151_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBT2_IF_AMP      =21;
static unsigned char DVBC_IF_AMP       =26;//Si2151_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char ATSC_8VSB_IF_AMP  = 21;
static unsigned char ATSC_QAM_IF_AMP   = 21;

#endif
//static unsigned char NO_L_DIGITAL_IF_AMP=Si2151_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char NO_L_ANALOG_IF_AMP=160;//Si2151_ATV_LIF_OUT_PROP_AMP_DEFAULT;  //100 -->1Vpp RMS
//static unsigned char L_ANALOG_IF_AMP   =100;//SECAML/L1
#if AD_R_setting  
static unsigned char L_ANALOG_IF_AMP=120;//SECAML/L1
#else
static unsigned char L_ANALOG_IF_AMP=100;//SECAML/L1
#endif


/* Tuner IF Spectrum Inverted variable */
static unsigned char TUNER_IF_INVERTED = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;

/* Signal Power related variable */
static INT8 Adjust_Si2151_i4Power_rec_val = 2;
static INT32 fgParaA_DVBC                 = 2;
static INT32 fgParaA_DVBT2                = 2;

/* Mediatek internal ATV demod variable */
static UINT8* pSi2151[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pSi2151EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
    #if SI2151_ENABLE_EXTERNAL_ATV_AGC_CONTROL
    {0x794,0x2a},
    {0x7a4,0x2a},
    {0x7d4,0x04},
    {0x7d5,0xfb},
    {0x7d6,0xfb},
    {0x7d7,0xEC},
    {0x7d8,0xf5},
    {0x7d9,0xf5},
    {0x7da,0x7F},
    {0x7db,0x30},
    {0x7dc,0x80},
    {0x7dd,0x80},
    {0x7de,0x9f},
    {0x7df,0x11},
    {0x7b7,0x10},//Target level
    {0x7b9,0x10},//Target level
    {0x13b,0x80},
    {0x790,0x08},
    {0x1b4,0x17},   // Pilot estimator confg.
    {0x7e0,0x00}, //PGA Index
    #else
    {0x794,0x2a},
    {0x7a4,0x2a},
    {0x7d4,0x04},
    {0x7d5,0x00},
    {0x7d6,0x00},
    {0x7d7,0xEC},
    {0x7d8,0xEF},
    {0x7d9,0xEF},
    {0x7da,0x7F},
    {0x7db,0x7F},
    {0x7dc,0x80},
    {0x7dd,0x80},
    {0x7de,0x80},
    {0x7df,0x00},
    {0x7b7,0x10},
    {0x7b9,0x0e},
    {0x13b,0x80},
    {0x790,0x08},
    {0x1b4,0x17},   // Pilot estimator confg.
    #endif

#else

    #if SI2151_ENABLE_EXTERNAL_ATV_AGC_CONTROL
    {0x294,0x38},
    {0x2a4,0x28},
    {0x2d4,0x04},
    {0x2d5,0xfb},
    {0x2d6,0xfb},
    {0x2d7,0xEC},
    {0x2d8,0xf5},
    {0x2d9,0xf5},
    {0x2da,0x7F},
    {0x2db,0x30},
    {0x2dc,0x80},
    {0x2dd,0x80},
    {0x2de,0x9f},
    {0x2df,0x11},
    {0x2b7,0x10},
    {0x2b9,0x0e},
    {0x13b,0x80},
    {0x290,0x08},
    #else
    {0x294,0x38},
    {0x2a4,0x28},
    {0x2d4,0x04},
    {0x2d5,0x00},
    {0x2d6,0x00},
    {0x2d7,0xEC},
    {0x2d8,0xEF},
    {0x2d9,0xEF},
    {0x2da,0x7F},
    {0x2db,0x7F},
    {0x2dc,0x80},
    {0x2dd,0x80},
    {0x2de,0x80},
    {0x2df,0x00},
    {0x2b7,0x10},
    {0x2b9,0x0e},
    {0x13b,0x80},
    {0x290,0x08},
    #endif

#endif
{0x00,0x00}
};
static ATD_TUNER_REL_REG_T arrAtdRelRegSecam[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
    #if SI2151_ENABLE_EXTERNAL_ATV_AGC_CONTROL
    {0x7d4,0x04},
    {0x7d5,0xfb},
    {0x7d6,0xfb},
    {0x7d7,0xEC},
    {0x7d8,0xf5},
    {0x7d9,0xf5},
    {0x7da,0x7F},
    {0x7db,0x30},
    {0x7dc,0x80},
    {0x7dd,0x80},
    {0x7de,0x80},
    {0x7df,0x11},
    {0x7b7,0x05},
    {0x7b9,0x05},
    {0x13b,0x81},
    {0x790,0x88},
    {0x1b4,0x17},    // Pilot estimator confg.
    {0x7e0,0x00},//PGA Index
    #else
    {0x7d4,0x04},
    {0x7d5,0x00},
    {0x7d6,0x00},
    {0x7d7,0xEC},
    {0x7d8,0xEF},
    {0x7d9,0xEF},
    {0x7da,0x7F},
    {0x7db,0x7F},
    {0x7dc,0x80},
    {0x7dd,0x80},
    {0x7de,0x80},
    {0x7df,0x00},
    {0x7b7,0x05},
    {0x7b9,0x03},
    {0x13b,0x81}, /*wenming 11-11-24:bit 0 set 1 for positive modulation*/
    {0x790,0x88},
    {0x1b4,0x17},    // Pilot estimator confg.
    #endif

#else

#if SI2151_ENABLE_EXTERNAL_ATV_AGC_CONTROL
    {0x2d4,0x04},
    {0x2d5,0xfb},
    {0x2d6,0xfb},
    {0x2d7,0xEC},
    {0x2d8,0xf5},
    {0x2d9,0xf5},
    {0x2da,0x7F},
    {0x2db,0x30},
    {0x2dc,0x80},
    {0x2dd,0x80},
    {0x2de,0x9f},
    {0x2df,0x11},
    {0x2b7,0x05},
    {0x2b9,0x03},
    {0x13b,0x81}, /*wenming 11-11-24:bit 0 set 1 for positive modulation*/
    {0x290,0x88},
    #else
    {0x2d4,0x04},
    {0x2d5,0x00},
    {0x2d6,0x00},
    {0x2d7,0xEC},
    {0x2d8,0xEF},
    {0x2d9,0xEF},
    {0x2da,0x7F},
    {0x2db,0x7F},
    {0x2dc,0x80},
    {0x2dd,0x80},
    {0x2de,0x80},
    {0x2df,0x00},
    {0x2b7,0x05},
    {0x2b9,0x03},
    {0x13b,0x81}, /*wenming 11-11-24:bit 0 set 1 for positive modulation*/
    {0x290,0x88},
    #endif

#endif
{0x00,0x00}
};
/************************************************************************************************************************
  NAME: PCFreq_To_CenterFreq
  DESCRIPTION: Convert Picture Carrier frequency to Center Frequency
  Parameter:  System
  Parameter:  PC Frequency
  Returns:    I2C transaction error code, 0 if successful
************************************************************************************************************************/
UINT32 PCFreq_To_CenterFreq(UINT8 u1SubSysIdx,UINT32 pc,UINT8 fgAutoSearch)
{
    UINT32 cf=0;
    switch (u1SubSysIdx)
    {
        case MOD_PAL_BG:  /*wenming 11-12-2: BG PC to Center is by 7M or 8M BW*/
            {
                if(ATV_CONN_MODE_CHAN_SCAN == fgAutoSearch) /*wenming 12-4-16: fgAutoSearch get from MW, ignore driver fgAutoSearch setting*/
                {
                    cf = pc + C_Si2151_PF_TO_CF_SHIFT_BG_8M*1000;
                    mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER in PC when scan mode\n"));
                }
                else
                {
                    if(pc<300000000)  //pal B system
                    {
                        cf = pc + C_Si2151_PF_TO_CF_SHIFT_BG_7M*1000;
                        mcDBG_MSG_L2(("TestLog:Now set 7M PC-CENTER in PC\n"));
                    }
                    else    //pal G system
                    {
                        cf = pc + C_Si2151_PF_TO_CF_SHIFT_BG_8M*1000;
                        mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER in PC\n"));
                    }
                }
            }
            break;
        case MOD_PAL_DK:
            cf = pc + C_Si2151_PF_TO_CF_SHIFT_DK*1000;
            break;
        case MOD_PAL_I:
            cf = pc + C_Si2151_PF_TO_CF_SHIFT_I*1000;
            break;
        case MOD_NTSC_M:
            cf = pc + C_Si2151_PF_TO_CF_SHIFT_M*1000;
            break;
        case MOD_SECAM_L:
            cf = pc + C_Si2151_PF_TO_CF_SHIFT_L*1000;
            break;
        case MOD_SECAM_L1:
            cf = pc - C_Si2151_PF_TO_CF_SHIFT_L1*1000;
            break;
    }
    return cf;
}

/** Si2151_TunerInit
 *  Tuner initialzation forSi2151.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */

//-----------------------------------------------------------------------------
VOID Si2151_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{
    ITUNER_CTX_T *psTunerCtx = pTCtx;
    SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);
    SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);

    TunerInited = FALSE;
    
    psTunerCtx->u2LO_Step=1000;  //*1Khz

    psTunerCtx->I2cAddress = C_Si2151_LO_ADDRESS;
    psTunerCtx->u2IF_Freq = C_Si2151_IF_FREQUENCY;
    psTunerCtx->u2IF_Freq_A = 0; // C_Si2151_IF_FREQUENCY_ANA;//analog if frequency for ATD
    psTunerCtx->u4RF_Freq = 0;

     // AGC para
    psTunerCtx->sAtdAgcPara.AgcRfBias = 0x00;
    psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x00;
    psTunerCtx->sAtdAgcPara.AgcIfMin  = 0x00;
    psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B;

    psTunerCtx->u2RetryTVDTm = 100;  /*wenming 11-11-16:retry TVD lock for secam loss issue*/

    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x8D;//MASK_PATCH_CR|MASK_PATCH_SENS|MASK_PATCH_CCI|MASK_PATCH_SELECTIVITY
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
    #if SI2151_ENABLE_EXTERNAL_DTV_AGC_CONTROL
    psTunerCtx->fgRFTuner =FALSE;//Digital tuner
    #else
    psTunerCtx->fgRFTuner =TRUE;//d120210_Haibo:PGA shound be close loop
    #endif
    psTunerCtx->u1EqIndex=0;//EQ index select

    pEUCtx->m_SAW_BW  = C_Si2151_SAW_BW;
    pEUCtx->m_Ana_Top = C_Si2151_TOP_SET_ANA;
    pUSCtx->u2LO_Step = 1000; //*1KHZ
    pUSCtx->m_aucPara_SLD[2] = (U8)C_U8_Si2151_AGC_IF_SLP;
    pUSCtx->m_aucPara_SLD[3] = (U8)C_U8_Si2151_AGC_IF_INT;
    pUSCtx->m_aucPara_SLD[4] = (U8)C_U8_Si2151_AGC_IF_MAX;
    pUSCtx->m_aucPara_SLD[5] = (U8)C_U8_Si2151_AGC_IF_MIN;

    //Tuner frequency range
    pEUCtx->m_s4FreqBoundUpper = C_Si2151_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_Si2151_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_Si2151_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_Si2151_FREQ_DBOUND_LOWER_Ana;
    // for Signal Level display
    pEUCtx->m_ifAgc_lvl_max = (UINT8)C_U8_Si2151_AGC_IF_LVL_MAX;
    pEUCtx->m_ifAgc_lvl_min = (UINT8)C_U8_Si2151_AGC_IF_LVL_MIN;

    pEUCtx->m_SigLvTh = 0;
    pEUCtx->m_SigLvScan = 0;

    pEUCtx->m_aucPara[0] = C_Si2151_TOP_SET;
    pEUCtx->m_aucPara[4] = (UINT8)C_U8_Si2151_AGC_IF_MAX;
    pEUCtx->m_aucPara[1] = 0;

    pEUCtx->m_aucPara[2] = (UINT8)C_U8_Si2151_AGC_IF_SLP;
    pEUCtx->m_aucPara[3] = (UINT8)C_U8_Si2151_AGC_IF_INT;
    pEUCtx->m_aucPara[5] = (UINT8)C_U8_Si2151_AGC_IF_MIN;

    pEUCtx->m_aucPara[6] = C_Si2151_AGC_COEF;
    pEUCtx->m_aucPara[7] = C_Si2151_DEMOD_INPUT_POWER;

    pEUCtx->m_aucPara[ 8] = (UINT8)C_U8_Si2151_MAX_IF_GAIN_SET;
    pEUCtx->m_aucPara[ 9] = (UINT8)C_U8_Si2151_MIN_IF_GAIN_SET;
    pEUCtx->m_aucPara[10] = (UINT8)C_U8_Si2151_MAX_RF_GAIN_SET;
    pEUCtx->m_aucPara[11] = (UINT8)C_U8_Si2151_MIN_RF_GAIN_SET;
    pEUCtx->m_aucPara[12] = (UINT8)C_U8_Si2151_IF_GAIN_RANGE;
    pEUCtx->m_aucPara[13] = (UINT8)C_U8_Si2151_RF_GAIN_RANGE;
    pEUCtx->m_aucPara[14] = C_Si2151_POWER_CALIBRATE;

    //IF MAX setting!
    pEUCtx->m_aucPara[21] = 0x7F;
    pSi2151[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2151BG;
    pSi2151[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2151DK;
    pSi2151[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2151I;
    pSi2151[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2151L;
    pSi2151[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2151L1;
    pSi2151[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]    = EQSi2151M;

    pSi2151EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2151BG;
    pSi2151EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2151DK;
    pSi2151EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2151I;
    pSi2151EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2151L;
    pSi2151EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2151L1;
    pSi2151EqWeak[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]    = EQSi2151M;



    /* Silabs tuner Init here */
    if(!TunerInited)
    {
        mcDBG_MSG_L1(("Init " C_SILABS_Si2151_STR "(Cust tuner)\n"));
        api = &api_context;
        /* Software Init */
        Si2151_L1_API_Init(api,C_Si2151_LO_ADDRESS);
		
		/* Hardware Init */
        api->cmd->power_up.clock_mode = Si2151_POWER_UP_CMD_CLOCK_MODE_XTAL;
        api->cmd->power_up.en_xout = Si2151_POWER_UP_CMD_EN_XOUT_DIS_XOUT;
		
		if ((Si2151_Init(api)) != 0)
		{
		    TunerInited = FALSE;
		    mcDBG_MSG_ERR(("[Silabs]:Error Si2151_Init!!!\n"));
		}
		else
		{
			TunerInited = TRUE;
		    mcDBG_MSG_L1(("Si2151_Init successfully.\n"));
		}
    }
}

/* Setup properties to switch standards. */
void SetIfDemod(ITUNER_CTX_T *pTCtx,UINT8 subSysIdx,UINT32 Freq,UINT8 fgAutoSearch)
{
    // set a global here to save the video standard,  because it's not always available from the caller.
    VideoStandard =subSysIdx;
	
	if (subSysIdx == MOD_DVBT2) /* Configure GPIO pin for FEF feature */
	{
        if((Si2151_L1_CONFIG_PINS (api,
                                   Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DTV_FREEZE_HIGH,
								   Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
								   Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE,
								   Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE,
								   Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != 0)
	    {
		    mcDBG_MSG_ERR((" [Silabs]: ERROR_Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DTV_FREEZE_HIGH\n"));
        }
	}
	else
	{
	    if((Si2151_L1_CONFIG_PINS (api,
                                   Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE,
								   Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE,
								   Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE,
								   Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE,
								   Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ,
								   Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE)) != 0)
	    {
		    mcDBG_MSG_ERR((" [Silabs]: ERROR_Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE\n"));
        }
	}
	
    switch (subSysIdx)
    {
        case MOD_PAL_BG:
        /*wenming 11-12-05:always set sys as G when scan mode to fix DK detected as M/N issue*/
            if(ATV_CONN_MODE_CHAN_SCAN == fgAutoSearch)
            {
                api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
                mcDBG_MSG_L2(("TestLog:Now set as GH sys when autosearch\n"));
            }
            else
            {
                if(Freq<300000)  //pal B system
                {
                    api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_B;
                    mcDBG_MSG_L2(("TestLog:Now set as B sys\n"));
                }
                else
                {        //pal G system
                    api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
                    mcDBG_MSG_L2(("TestLog:Now set as GH sys\n"));
                }
            }
            api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_Si2151_IF_CENTER_BG;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
            }
              api->prop->atv_lif_out.offset = Si2151_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp    = NO_L_ANALOG_IF_AMP;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_OUT_PROP\n"));
            }
            api->prop->atv_rf_top.atv_rf_top= Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_M6DB; 
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_PAL_DK:
            api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_DK;
            api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_Si2151_IF_CENTER_DK;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
            }
            api->prop->atv_lif_out.offset = Si2151_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->atv_lif_out.amp    = NO_L_ANALOG_IF_AMP;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_OUT_PROP\n"));
            }
            api->prop->atv_rf_top.atv_rf_top= Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_M6DB; 
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_PAL_I:
            api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_I;
            api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_Si2151_IF_CENTER_I;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
            }
            api->prop->atv_lif_out.offset = Si2151_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->atv_lif_out.amp    = NO_L_ANALOG_IF_AMP;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_OUT_PROP\n"));
            }
            api->prop->atv_rf_top.atv_rf_top= Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_M6DB; 
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_NTSC_M:
            api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_M;
            api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_Si2151_IF_CENTER_M;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
            }
            api->prop->atv_lif_out.offset = Si2151_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->atv_lif_out.amp    = NO_L_ANALOG_IF_AMP; // mark to check cli
            mcDBG_MSG_L2((" =====NTSC IF AMP=%d \n",api->prop->atv_lif_out.amp));
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_OUT_PROP\n"));
            }
            api->prop->atv_rf_top.atv_rf_top= Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_M6DB; 
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_SECAM_L:
            api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_L;
            api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_Si2151_IF_CENTER_L;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
            }
            api->prop->atv_lif_out.offset = Si2151_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->atv_lif_out.amp    = L_ANALOG_IF_AMP;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_OUT_PROP\n"));
            }
            api->prop->atv_rf_top.atv_rf_top= Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_M6DB; 
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_SECAM_L1:
            if(fgAutoSearch)
            {
                api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_L;
                api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            }
            else
            {
                api->prop->atv_video_mode.video_sys = Si2151_ATV_VIDEO_MODE_PROP_VIDEO_SYS_LP;
                api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL;
            }
            //api->prop->atv_video_mode.invert_spectrum = Si2151_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_Si2151_IF_CENTER_L1;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
            }
            api->prop->atv_lif_out.offset = Si2151_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->atv_lif_out.amp    = L_ANALOG_IF_AMP;
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_OUT_PROP\n"));
            }
            api->prop->atv_rf_top.atv_rf_top= Si2151_ATV_RF_TOP_PROP_ATV_RF_TOP_M6DB; 
            if (Si2151_L1_SetProperty2(api, Si2151_ATV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_RF_TOP_PROP\n"));
			}
            break;
       case MOD_DVBT:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_DVBT;
            if(pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_6M)
                api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_6MHZ;
            else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_7M)
                api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_7MHZ;
            else
                api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            mcDBG_MSG_L2((" [Silabs]: MOD_DVBT_MODULATION %d, BW %d, SPECTRUM_INVERTED %d\n", api->prop->dtv_mode.modulation, api->prop->dtv_mode.bw, api->prop->dtv_mode.invert_spectrum));
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp    = DVBT_IF_AMP;
            mcDBG_MSG_L2(("DVBT IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
            api->prop->dtv_rf_top.dtv_rf_top= Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_P4DB; //default 'P5DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
       case MOD_DVBT2:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_DVBT;
            if(pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_6M)
                api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_6MHZ;
            else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_7M)
                api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_7MHZ;
            else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_8M)
                api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_8MHZ;
            else
            api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            mcDBG_MSG_L2((" [Silabs]: MOD_DVBT2_MODULATION %d, BW %d, SPECTRUM_INVERTED %d\n", api->prop->dtv_mode.modulation, api->prop->dtv_mode.bw, api->prop->dtv_mode.invert_spectrum));
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp    = DVBT2_IF_AMP;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
            api->prop->dtv_rf_top.dtv_rf_top = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_P4DB; //default 'P5DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
       case MOD_DVBC:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_DVBC;
            api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = TUNER_IF_INVERTED;
            mcDBG_MSG_L2((" [Silabs]: MOD_DVBC_MODULATION %d, BW %d, SPECTRUM_INVERTED %d\n", api->prop->dtv_mode.modulation, api->prop->dtv_mode.bw, api->prop->dtv_mode.invert_spectrum));
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp    = DVBC_IF_AMP;
			//api->prop->dtv_lif_out.amp  = 32;
            mcDBG_MSG_L2(("XXXXXXXXXX DVBC IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
            
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
            api->prop->dtv_rf_top.dtv_rf_top = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_P4DB; //default 'M8DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
      case MOD_ATSC_8VSB:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_ATSC;
            api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp    = ATSC_8VSB_IF_AMP;
            mcDBG_MSG_L2(("XXXXXXXXXX 8VSB IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
			api->prop->dtv_rf_top.dtv_rf_top = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_AUTO; //default 'P4DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_J83B_AUTO_QAM:
        case MOD_J83B_256QAM:
        case MOD_J83B_64QAM:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_QAM_US;
            api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp    = ATSC_QAM_IF_AMP;
            mcDBG_MSG_L2(("XXXXXXXXXX QAM IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
			api->prop->dtv_rf_top.dtv_rf_top = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_AUTO; //default 'M8DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
        case MOD_DTMB:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_DTMB;
            api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp =Si2151_DTV_LIF_OUT_PROP_AMP_DEFAULT;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
			api->prop->dtv_rf_top.dtv_rf_top = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_AUTO; //default 'P4DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
       case MOD_ISDBT:
            api->prop->dtv_mode.modulation = Si2151_DTV_MODE_PROP_MODULATION_ISDBT;
            api->prop->dtv_mode.bw = Si2151_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2151_L1_SetProperty2(api, Si2151_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset = C_Si2151_IF_FREQUENCY;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
            }
            api->prop->dtv_lif_out.offset = Si2151_DTV_LIF_OUT_PROP_OFFSET_DEFAULT;
            api->prop->dtv_lif_out.amp    = Si2151_DTV_LIF_OUT_PROP_AMP_DEFAULT;
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_OUT_PROP\n"));
            }
			api->prop->dtv_rf_top.dtv_rf_top = Si2151_DTV_RF_TOP_PROP_DTV_RF_TOP_AUTO; //default 'P4DB'
            if (Si2151_L1_SetProperty2(api, Si2151_DTV_RF_TOP_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_RF_TOP_PROP\n"));
            }
            break;
    }
}
//-----------------------------------------------------------------------------
/** Si2151_TunerSetFreq
 *  Set Tuner PLL forSi2151/Si2151 to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param     param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range
                   -1 fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 Si2151_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
    UINT32 Freq = param->Freq; // transfer to video freq(KHz) digital center freq ,analog picture freq
    UINT8 Mode = param->Modulation;    //tv system
    UINT8 AutoSearch=param->fgAutoSearch;
    UINT32 freqHz =0;
    UINT32 u2TickStart_eu;
    TV_MODES tvMode;
	int return_code = 0;
	
    u2TickStart_eu = x_os_get_sys_tick();
	
    pTCtx->u4RF_Freq = Freq;
    mcDBG_MSG_L2 (("Si2151_TunerSetFreq freq = %d;Mode=%d\n", Freq,Mode ));
#if SI2151_ENABLE_EXTERNAL_ATV_AGC_CONTROL
    mcDBG_MSG_L2(("Si2151:Now is external ATV AGC control!!!\n"));
#endif
#if SI2151_ENABLE_EXTERNAL_DTV_AGC_CONTROL
    mcDBG_MSG_L2(("Si2151:Now is external DTV AGC control!!!\n"));
#endif

    SetIfDemod(pTCtx,Mode,Freq,AutoSearch);

    if(Mode == MOD_PAL_BG) /*wenming 11-12-2: BG PC to Center is by 7M or 8M BW*/
    {
        if(ATV_CONN_MODE_CHAN_SCAN == AutoSearch)
        {
            pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_BG+C_Si2151_PF_TO_CF_SHIFT_BG_8M;
            mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER when scan mode\n"));
        }
        else
        {
            if(Freq<300000)  //pal B system
            {
                pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_BG+C_Si2151_PF_TO_CF_SHIFT_BG_7M;
                mcDBG_MSG_L2(("TestLog:Now set 7M PC-CENTER\n"));
            }
            else            //pag G system
            {
                pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_BG+C_Si2151_PF_TO_CF_SHIFT_BG_8M;
                mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER\n"));
            }
        }
    }
    if(Mode == MOD_PAL_DK)
        pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_DK+C_Si2151_PF_TO_CF_SHIFT_DK;
    if(Mode == MOD_PAL_I)
        pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_I+C_Si2151_PF_TO_CF_SHIFT_I;
    if(Mode == MOD_NTSC_M)
        pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_M+C_Si2151_PF_TO_CF_SHIFT_M;
    if(Mode == MOD_SECAM_L)
        pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_L+C_Si2151_PF_TO_CF_SHIFT_L;
    if(Mode == MOD_SECAM_L1)
        pTCtx->u2IF_Freq_A = C_Si2151_IF_CENTER_L1-C_Si2151_PF_TO_CF_SHIFT_L1;

    if (Mode < MOD_TYPE_END)
        tvMode = DIGITAL_MODE;
    else
        tvMode = ANALOG_MODE;
	
	if (tvMode==ANALOG_MODE)
    {
        /* set Si2151_TUNER_LO_INJECTION_PROP property for ATV */
        api->prop->tuner_lo_injection.band_1    = Si2151_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
        api->prop->tuner_lo_injection.band_2    = Si2151_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
        api->prop->tuner_lo_injection.band_3    = Si2151_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;

        if ((Si2151_L1_SetProperty2(api,Si2151_TUNER_LO_INJECTION_PROP)) != 0)
        {
            mcDBG_MSG_ERR(("[Silabs]: ERROR_SETTING_PROPERTY Si2151_TUNER_LO_INJECTION_PROP\n"));
        }
    }
    else
    {
        /* set Si2151_TUNER_LO_INJECTION_PROP property for DTV */
        api->prop->tuner_lo_injection.band_1  = Si2151_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
        api->prop->tuner_lo_injection.band_2  = Si2151_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
        api->prop->tuner_lo_injection.band_3  = Si2151_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
        if ((Si2151_L1_SetProperty2(api,Si2151_TUNER_LO_INJECTION_PROP)) != 0)
        {
            mcDBG_MSG_ERR(("[Silabs]: ERROR_SETTING_PROPERTY Si2151_TUNER_LO_INJECTION_PROP\n"));
        }
    }
	
    /* Use Picture Carrier Frequency if ATV and Center Frequency if DTV */
    if (tvMode==ANALOG_MODE)
        freqHz = PCFreq_To_CenterFreq( Mode,Freq * 1000, AutoSearch);
    else
        freqHz = Freq * 1000;

    if (freqHz < Si2151_TUNER_TUNE_FREQ_CMD_FREQ_MIN || freqHz > Si2151_TUNER_TUNE_FREQ_CMD_FREQ_MAX)
    {
		mcDBG_MSG_ERR(("[Silabs]: freqHz is out of range!\n"));
        return ERROR_Si2151_PARAMETER_OUT_OF_RANGE; // =1
    }

	
	if ((tvMode == ANALOG_MODE) && (AutoSearch))
	{
	    if (Si2151_L1_ATV_SCAN_TUNE(api, freqHz) != 0)
	    {
	        mcDBG_MSG_ERR(("[Silabs]: Si2151_L1_ATV_SCAN_TUNE Fails!\n"));
            return -1;
	    }
	    else
	    {
		    x_thread_delay(40); /* wait 40ms */
			mcDBG_MSG_L2(("[Silabs]: Si2151_L1_ATV_SCAN_TUNE Successful\n"));
		}
	}
	else
	{
	    return_code = Si2151_Tune(api,(tvMode==ANALOG_MODE) ? Si2151_TUNER_TUNE_FREQ_CMD_MODE_ATV : Si2151_TUNER_TUNE_FREQ_CMD_MODE_DTV,  freqHz);
	    if ( return_code == 0)
		{
		    mcDBG_MSG_L2(("[Silabs]: Si2151_Tune Successful\n"));
		}
		else if ( (return_code == 0xd) || (return_code == 0xe)) /* Si2151_TUNINT_TIMEOUT or Si2151_xTVINT_TIMEOUT */
		{
		    mcDBG_MSG_L2(("[Silabs]: Si2151_Tune ... \n"));
		}
		else
		{ 
		   mcDBG_MSG_ERR(("[Silabs]: Si2151_Tune Fails!\n"));
           return -1;
		}
	}

    mcDBG_MSG_L2(("[Silabs]:Si2151TunerSetFreq use time = (%3u ms)\n", (x_os_get_sys_tick() - u2TickStart_eu) * x_os_get_sys_tick_period()));

    return 0;
}


/* Get Status information from part */
UCHAR GetStatus( L1_Si2151_Context *api,TUNER_STATUSES status)
{
    int return_code;
    if (status==CHL)
    {
        if ((return_code=Si2151_L1_TUNER_STATUS(api))!=0) {
             mcDBG_MSG_ERR(("Si2151_L1_TUNER_STATUS ATV error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
        }
        return api->rsp->tuner_status.STATUS->atvint;
    }
    else if (status == DTV_CHL)
    {
        if ((return_code=Si2151_L1_TUNER_STATUS(api))!=0) {
		    mcDBG_MSG_ERR(("Si2151_L1_TUNER_STATUS DTV error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));
        }
        return api->rsp->tuner_status.STATUS->dtvint;
    }
    else
        return 0;
}

/* Get RSSI  from part */
CHAR GetSignalLevel(L1_Si2151_Context *api)
{
    int return_code;
    if ((return_code=Si2151_L1_TUNER_STATUS(api))!=0) {
        mcDBG_MSG_ERR(("Si2151_L1_TUNER_STATUS RSSI error 0x%02x: %s\n", return_code, Si2151_L1_API_ERROR_TEXT(return_code)));

    }

    return api->rsp->tuner_status.rssi;
}
static VOID Si2151_SetSawBw(ITUNER_CTX_T * pTCtx, UINT8 sawbw)
{
    pTCtx->specific_member.eu_ctx.m_SAW_BW=sawbw;
    mcDBG_MSG_L2(("Si2151_SetSawBw=%d\n",sawbw));
}

//-----------------------------------------------------------------------------
/** Si2151_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *Si2151_TunerGetVer(VOID)
{
    return ((CHAR*)C_SILABS_Si2151_STR);
}



UINT32 Si2151_GetSSI(ITUNER_CTX_T* pTunerCtx)
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
    INT8 i4Power_rec = 0;
    INT32 i4Power_rel = 0;

    UINT32 u4SSI_Indicator = 0;
    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);

    // initial level for best mux, Ken, 100607
    pCtx->m_SigLvScan = 0;

    /* Get Power Ref Value */
    mcDBG_MSG_L2(("DVBT: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

    /* Get Power_Rec Value */
    i4Power_rec=GetSignalLevel(api);

    /*Get Ref Value-Start*/
    if(pTunerCtx->sSSIPara.i2Mod==DVBT_Qam_Mode_Unknow)
    {
        //mcSHOW_DBG_MSG1(("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
        //printf("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel);
        return u4SSI_Indicator;
    }
    else
    {
        i4Power_ref = Power_Ref[(pTunerCtx->sSSIPara.i2Mod)*DVBT_CR_Mode_Size + pTunerCtx->sSSIPara.i2CR];
    }

    /* Get Power Rel Value */
    if(pTunerCtx->sSSIPara.i2Bw == SAW_BW_8M)
    {
        mcDBG_MSG_L2(("DVBT:8M Real_i4Power_rec = %d\n",i4Power_rec));
        i4Power_rec = i4Power_rec - Adjust_Si2151_i4Power_rec_val;
    }
    else
    {
        mcDBG_MSG_L2(("DVBT:7M Real_i4Power_rec = %d\n",i4Power_rec));
        i4Power_rec = i4Power_rec - Adjust_Si2151_i4Power_rec_val;
    }
    i4Power_rel = i4Power_rec - i4Power_ref;
    mcDBG_MSG_L2(("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));

    //printf("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel);
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
    pCtx->m_SigLvScan = (INT8) u4SSI_Indicator;
    mcDBG_MSG_L2(("u4SSI_Indicator = %d",u4SSI_Indicator));
    return u4SSI_Indicator;
}


UINT32 Si2151_GetSSI_T2(ITUNER_CTX_T* pTunerCtx)
{
    INT16 Power_Ref[] =
    {
        -930,
        -923,
        -916,
        -909,
        -902,
        -895,
        -888,
        -881,
        -874,
        -867,
        -860,
        -853,
        -846,
        -839,
        -832,
        -825,
        -818,
        -811,
        -794,
        -787,
        -780,
        -772,
        -765,
        -758,
    };
    INT32 i4Power_ref = 0;
    INT32 i4Power_rec = 0;
    INT32 i4Power_rel = 0;

    UINT32 u4SSI_Indicator = 0;
    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);

    // initial level for best mux, Ken, 100607
    pCtx->m_SigLvScan = 0;

    /* Get Power Ref Value */
    mcDBG_MSG_L2(("DVBT: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

    /* Get Power_Rec Value */
    i4Power_rec=((INT32)GetSignalLevel(api))*10;

    /*Get Ref Value-Start*/
    if(pTunerCtx->sSSIPara.i2Mod==DVBT2_Qam_Mode_Unknow)
    {
        //mcDBG_MSG_L21(("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
        //printf("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel);
        return u4SSI_Indicator;
    }
    else
    {
        i4Power_ref = Power_Ref[(pTunerCtx->sSSIPara.i2Mod)*DVBT2_CR_Mode_Size + pTunerCtx->sSSIPara.i2CR];
    }

    /* Get Power Rel Value */
    i4Power_rec = i4Power_rec - 20;  //adjust bu test result;
    i4Power_rel = i4Power_rec - i4Power_ref;
    mcDBG_MSG_L2(("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));

    //printf("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel);
    /* Cal SSI_Indicator Value */
    if(i4Power_rel < -150)
    {
        u4SSI_Indicator = 0;
    }
    else if((i4Power_rel >= -150) && (i4Power_rel < 0))
    {
        u4SSI_Indicator = (i4Power_rel+150)/15;
    }
    else if((i4Power_rel >= 0) && (i4Power_rel < 200))
    {
        u4SSI_Indicator = 4*i4Power_rel/10+10;
    }
    else if((i4Power_rel >= 200) && (i4Power_rel < 350))
    {
        u4SSI_Indicator = (i4Power_rel-200)/15+90;
    }
    else if(i4Power_rel >= 350)
    {
        u4SSI_Indicator = 100;
    }

    // update signal level for best mux, Ken, 100607
    pCtx->m_SigLvScan = (INT8) u4SSI_Indicator;
    mcDBG_MSG_L2(("u4SSI_Indicator = %d\n",u4SSI_Indicator));
    return u4SSI_Indicator;
}

/*wenming 11-10-13:DVBC SSI*/
UINT32 Si2151_GetSSI_DVBC(ITUNER_CTX_T* pTunerCtx)
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
    INT32 i4Power_rec = 0;
    INT32 i4Power_rel = 0;
    INT16 refIndex=0;
    UINT32 u4SSI_Indicator = 0;

    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);
    pCtx->m_SigLvScan = 0;
    mcDBG_MSG_L2(("DVBC: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

    i4Power_rec=GetSignalLevel(api);;
    i4Power_rec = i4Power_rec - fgParaA_DVBC;
     //i4Power_rec+=6;//compensate
        refIndex=pTunerCtx->sSSIPara.i2Mod;
        mcDBG_MSG_L2(("refIndex=%d",refIndex));
    if(refIndex>4)
    {
        mcDBG_MSG_L2(("DVBC: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
        return u4SSI_Indicator;
    }
    else
    {
        i4Power_ref = Power_Ref[refIndex];
        mcDBG_MSG_L2(("i4Power_ref=%d\n",i4Power_ref));
    }
    i4Power_rel = i4Power_rec - i4Power_ref;
    mcDBG_MSG_L2(("DVBC: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
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
    pCtx->m_SigLvScan = (S8) u4SSI_Indicator;
    return u4SSI_Indicator;
}
//-----------------------------------------------------------------------------
/** Si2151_SetSSICondition
 *  Set SSI Condition for TH2603
 *  @param
 *  @retval void
 */
//-----------------------------------------------------------------------------
void Si2151_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);
    pTunerCtx->sSSIPara.i2Bw = *((INT16*) pInOutVal+5);
}

//-----------------------------------------------------------------------------
/** Si2151_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 Si2151_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){

    //L1_Si2151_Context *api;
    int return_code =0;

    //api->prop->dtv_mode.bw = 8;
    switch(eOperation){
    case itSetSSICond:
        Si2151_SetSSICondition(pTCtx, pInOutVal);
        break;
    case itGetEqScriptNormal:
        *(UINT8**)pInOutVal = pSi2151[SetVal];;
        break;
    case itGetEqScriptWeak:
        *(UINT8**)pInOutVal = pSi2151EqWeak[SetVal];
        break;
    case itGetTunerRelReg://Get ATD Tuner Rel register setting
        {
            if((SetVal==MOD_SECAM_L)||(SetVal==MOD_SECAM_L1)){
                *(ATD_TUNER_REL_REG_T **)pInOutVal = arrAtdRelRegSecam;}
            else{
                *(ATD_TUNER_REL_REG_T **)pInOutVal = arrAtdRelReg;
            }
        }
    break;

    case itGetVer:
        {
            CHAR ** RetStr=(CHAR **)pInOutVal;
            *RetStr=Si2151_TunerGetVer();
        }
        break;
    case itGetTop:
        if (SetVal >= MOD_ANA_TYPE_BEGIN)
        {
            if ((return_code=Si2151_L1_GetProperty2(api,Si2151_ATV_RF_TOP_PROP))!=0)
            {
                mcDBG_MSG_ERR(("Si2151_L1_GetProperty2 error 0x%02x\n", return_code));
                return ITUNER_NOT_OK;
            }
            *(UINT8 *)pInOutVal= api->prop->atv_rf_top.atv_rf_top;
        }
        else
        {
            if ((return_code=Si2151_L1_GetProperty2(api,Si2151_DTV_RF_TOP_PROP))!=0)
            {
                mcDBG_MSG_ERR(("Si2151_L1_GetProperty2 error 0x%02x\n", return_code));
                return ITUNER_NOT_OK;
            }
            *(UINT8 *)pInOutVal= api->prop->dtv_rf_top.dtv_rf_top;
        }
        break;
    case itSetIfDemod:
        //    SetIfDemod(SetVal);
        break;
    case itSetTop:
        {
            if(VideoStandard >= MOD_ANA_TYPE_BEGIN)
            {
                api->prop->atv_rf_top.atv_rf_top=SetVal;
                if ((return_code=Si2151_L1_SetProperty2(api,Si2151_ATV_RF_TOP_PROP)) != 0)
                {
                    mcDBG_MSG_ERR(("Si2151_L1_SetProperty2 error 0x%02x\n", return_code));
                    return ITUNER_NOT_OK;
                }
            }
            else
            {
                api->prop->dtv_rf_top.dtv_rf_top=SetVal;
                if ((return_code=Si2151_L1_SetProperty2(api,Si2151_DTV_RF_TOP_PROP)) != 0)
                {
                    mcDBG_MSG_ERR(("Si2151_L1_SetProperty2 error 0x%02x\n", return_code));
                    return ITUNER_NOT_OK;
                }
            }
        }
        break;
    case itSetSawBw://Set Channel Bandwidth
        {
            Si2151_SetSawBw(pTCtx,SetVal);
        }
        break;
        case itGetStatus: // return channel locked status.
        if (SetVal >= MOD_ANA_TYPE_BEGIN)
            *(UINT8 *)pInOutVal = GetStatus( api,CHL);
        else
            *(UINT8 *)pInOutVal = GetStatus( api,DTV_CHL);
        break;
    case itGetSSIIndex: // Get RSSI
        //    INT32 *RetVal= NULL;
        //   RetVal = (UINT32 *)pInOutVal;
        if (SetVal==0)
        {
            *(INT32 *)pInOutVal = Si2151_GetSSI(pTCtx);
        }
        else
        {
            *(INT32 *)pInOutVal = Si2151_GetSSI_DVBC(pTCtx);
        }
        break;
    case itGetSSIIndex_T2: // Get RSSI
        *(INT32 *)pInOutVal = Si2151_GetSSI_T2(pTCtx);//(INT16)GetSignalLevel(api);
        //mcDBG_MSG_L2(("Si2151_GetSignalLevel= %d\n",(GetSignalLevel(api))));
        break;
    case itGetRSSI:
        {
            INT32 * Retval = (INT32*)pInOutVal;
            *Retval = GetSignalLevel(api) - fgParaA_DVBT2;
        }
        break;    
    case itGetSignalLevel: // Get RSSI
        *(INT32 *)pInOutVal = (108+(INT8)GetSignalLevel(api));// use dBuV to show signal level
        //mcDBG_MSG_L2(("Si2151_GetSignalLevel= %d\n",(108 +(INT8) GetSignalLevel(api))));
        break;
    case itSetIF: // set ATV/DTV LIF FREQ
        if (VideoStandard >= MOD_ANA_TYPE_BEGIN)
        {
            api->prop->atv_lif_freq.offset = SetVal;
            if ((return_code=Si2151_L1_SetProperty2(api,Si2151_ATV_LIF_FREQ_PROP)) != 0)
            {
                mcDBG_MSG_ERR(("Si2151_L1_SetProperty2 error 0x%02x\n", return_code));
                return ITUNER_NOT_OK;
            }
        }
        else
        {
            api->prop->dtv_lif_freq.offset = SetVal;
            if ((return_code=Si2151_L1_SetProperty2(api,Si2151_DTV_LIF_FREQ_PROP)) != 0)
            {
                mcDBG_MSG_ERR(("Si2151_L1_SetProperty2 error 0x%02x\n", return_code));
                return ITUNER_NOT_OK;
            }
        }
        break;
    case itSetIFAMP:
        {
            mcDBG_MSG_L2(("---------IF_AMP set as %d \n",SetVal));
			mcDBG_MSG_L2(("VideoStandard=%d\n",VideoStandard));

            if(SetVal<=255)
            {
                if(VideoStandard== MOD_DVBT)
                {
                    DVBT_IF_AMP=SetVal;
                    api->prop->dtv_lif_out.amp =DVBT_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("DVBT_IF_AMP set as %d \n",DVBT_IF_AMP));
                }
                else if(VideoStandard== MOD_DVBT2)
                {
                    DVBT2_IF_AMP=SetVal;
                    api->prop->dtv_lif_out.amp =DVBT2_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("DVBT2_IF_AMP set as %d \n",DVBT2_IF_AMP));
                }
                else if(VideoStandard== MOD_DVBC)
                {
                    DVBC_IF_AMP=SetVal;
                    api->prop->dtv_lif_out.amp =DVBC_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("DVBC_IF_AMP set as %d \n",DVBC_IF_AMP));
                }
                else if((VideoStandard== MOD_SECAM_L)||(VideoStandard==MOD_SECAM_L1))
                {
                    L_ANALOG_IF_AMP=SetVal;
                    api->prop->atv_lif_out.amp =L_ANALOG_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("L_ANALOG_IF_AMP set as %d \n",NO_L_ANALOG_IF_AMP));
                }
                else if((VideoStandard==MOD_J83B_64QAM)||(VideoStandard==MOD_J83B_256QAM)||(VideoStandard==MOD_J83B_AUTO_QAM))
                {
                    ATSC_QAM_IF_AMP=SetVal;
                    api->prop->dtv_lif_out.amp =ATSC_QAM_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("QAM NO_L_DIGITAL_IF_AMP set as %d \n",api->prop->dtv_lif_out.amp));
                }
                else if((VideoStandard==MOD_ATSC_8VSB))
                {
                    ATSC_8VSB_IF_AMP=SetVal;
                    api->prop->dtv_lif_out.amp =ATSC_8VSB_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_DTV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_DTV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("8VSB NO_L_DIGITAL_IF_AMP set as %d \n",api->prop->dtv_lif_out.amp));
                }
                else if((VideoStandard>=MOD_ANA_TYPE_BEGIN)||(VideoStandard<MOD_ANA_TYPE_END))
                {
                    NO_L_ANALOG_IF_AMP=SetVal;
                    api->prop->atv_lif_out.amp =NO_L_ANALOG_IF_AMP;
                    if (Si2151_L1_SetProperty2(api, Si2151_ATV_LIF_OUT_PROP) != 0)
                    {
                        mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY Si2151_ATV_LIF_FREQ_PROP\n"));
                    }
                    mcDBG_MSG_L2(("NO_L_ANALOG_IF_AMP set as %d \n",NO_L_ANALOG_IF_AMP));
                }
                else
                    mcDBG_MSG_L2(("TVStandard unknown pls set a channel at first"));
            }
        }
        break;
	case itSetIfSpecInvert:
    {
        mcDBG_MSG_L2(("itSetIfSpecInvert %d, 0->normal, 1->invert\n",SetVal));

        if (SetVal == 1)
        {
            TUNER_IF_INVERTED = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_INVERTED;

        }
        else if (SetVal == 0)
        {
            TUNER_IF_INVERTED = Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL;
        }
        else
        {
            mcDBG_MSG_L2(("itSetIfSpecInvert, do nothing here!\n"));
        }
        break;
    }
#if 0
	case itSetClockState:
    {
        mcDBG_MSG_L2(("itSetClockState %d\n",SetVal));
        if (SetVal == 1)
        {
            *(S32*)pInOutVal= Si2151_L1_CONFIG_CLOCKS(api,
                                                      Si2151_CONFIG_CLOCKS_CMD_SUBCODE_CODE,
                                                      api->cmd->config_clocks.clock_mode,
                                                      Si2151_CONFIG_CLOCKS_CMD_EN_XOUT_EN_XOUT);
        }
        else if (SetVal == 0)
        {
            *(S32*)pInOutVal= Si2151_L1_CONFIG_CLOCKS(api,
                                                      Si2151_CONFIG_CLOCKS_CMD_SUBCODE_CODE,
                                                      api->cmd->config_clocks.clock_mode,
                                                      Si2151_CONFIG_CLOCKS_CMD_EN_XOUT_DIS_XOUT);
        }
        else{
            mcDBG_MSG_L2(("itSetClockState, do nothing here!\n"));
        }
        break;
    }
#endif
    case itSetRSetting:
    {
        switch(SetVal)
            {
        #if AD_R_setting    //have IF attenuator
                case MOD_PAL_BG: 
                case MOD_PAL_DK: 
                case MOD_PAL_I:
                case MOD_NTSC_M:
                    *(UINT32 *)pInOutVal=0x06 ;
                    break;
                case MOD_SECAM_L:
                case MOD_SECAM_L1:
                    *(UINT32 *)pInOutVal=0x07 ;
                    break;
                case MOD_ATSC_8VSB:
                case MOD_J83B_64QAM:
                case MOD_J83B_256QAM:
                case MOD_J83B_AUTO_QAM:
                    *(UINT32 *)pInOutVal=0x06 ;
                    break;
                case MOD_DVBT:
                case MOD_DVBC:
                    *(UINT32 *)pInOutVal=0x06 ;
                    break;
        #else               //No IF attenuator
                case MOD_PAL_BG: 
                case MOD_PAL_DK: 
                case MOD_PAL_I:
                case MOD_NTSC_M:
                    *(UINT32 *)pInOutVal=0x00 ;
                    break;
                case MOD_SECAM_L:
                case MOD_SECAM_L1:
                    *(UINT32 *)pInOutVal=0x00 ;
                    break;
                case MOD_ATSC_8VSB:
                case MOD_J83B_64QAM:
                case MOD_J83B_256QAM:
                case MOD_J83B_AUTO_QAM:
                    *(UINT32 *)pInOutVal=0x00 ;
                    break;
                case MOD_DVBT:
                case MOD_DVBC:
                    *(UINT32 *)pInOutVal=0x00 ;
                    break;
        #endif
                default:
                    *(UINT32 *)pInOutVal=0x06 ;
                    break;
    
        }
         mcDBG_MSG_L1(("Tuner R_setting Value=%d\n", *(UINT32 *)pInOutVal));
    }
    break;

    default:
        return ITUNER_NOT_SUPPORTED;
    }
    return ITUNER_OK;
}
