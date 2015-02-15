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
 * $RCSfile: pi_tuner_SI2156.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/* Silabs version 30b3-4.0.0  17Aug2011 */

/** @file pi_tuner_SI2156.c
 *  Tuner control for SS SI2156
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#include "demod_tuner_interface.h"
//#include "pd_i2capi.h"
#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "si2156_i2c_api.h"
#include "x_typedef.h"
//#include "tunerDebug.h"
#include "x_os.h"
#include "PD_Def.h"
#include "pi_def_dvbt.h"
#include "pd_atd_if.h" //wenming 2012-4-16: add for ATV_CONN_MODE enum


// add your messaging command here for debugging.

#include "si2156_L1_API.h"
#include "firmware_3_0_build_3.h"
#include "firmware_3_1_build_6.h"

#include "1Eb6_patch.h"

#include "eq_script_si2156.h"


#define SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL 1//for SI2156 ATV AGC source selection, 0 for internal, 1 for external
#define SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL 1//for SI2156 DTV AGC source selection, 0 for internal, 1 for external

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Frequency boundary .Get from tuner spec
#define C_SI2156_FREQ_DBOUND_UPPER   859*1000*1000
#define C_SI2156_FREQ_DBOUND_LOWER    48*1000*1000
#define C_SI2156_FREQ_DBOUND_UPPER_Ana     875*1000*1000
#define C_SI2156_FREQ_DBOUND_LOWER_Ana    45*1000*1000

//PLL lock check parameters
#define C_SI2156_POLL_INTERVAL      5 //ms
#define C_SI2156_PLL_POLL_TIMETOUT      100 //ms
#define C_SI2156_PLL_POLL_CNT           C_SI2156_PLL_POLL_TIMETOUT/C_SI2156_POLL_INTERVAL //counter

#define C_SI2156_PF_TO_CF_SHIFT_BG_7M 2250 /*KHZ*/  //wenming 11-12-2: PF to CF is 2250 when 7M B
#define C_SI2156_PF_TO_CF_SHIFT_BG_8M 2750 /*KHZ*/  //wenming 11-12-2:PF to CF is 2750 when 8M G
#define C_SI2156_PF_TO_CF_SHIFT_DK 2750 /*KHZ*/
#define C_SI2156_PF_TO_CF_SHIFT_I  2750 /*KHZ*/
#define C_SI2156_PF_TO_CF_SHIFT_L  2750 /*KHZ*/
#define C_SI2156_PF_TO_CF_SHIFT_M  1750 /*KHZ*/
#define C_SI2156_PF_TO_CF_SHIFT_L1 2750 /*KHZ*/
#define C_SI2156_IF_CENTER_BG 5000 /*KHZ*/
#define C_SI2156_IF_CENTER_DK 5000 /*KHZ*/
#define C_SI2156_IF_CENTER_I  5000 /*KHZ*/
#define C_SI2156_IF_CENTER_L  5000 /*KHZ*/
#define C_SI2156_IF_CENTER_L1 5000 /*KHZ*/
#define C_SI2156_IF_CENTER_M  5000 /*KHZ*/

#define C_SI2156_IF_FREQUENCY          ((UINT16)  5000)  /* kHz */


#define C_SI2156_LO_ADDRESS           ((UINT8)  0xC0)
//#define C_SI2156_IF_FREQUENCY          ((UINT16)  36130)  /* kHz */
//#define C_SI2156_IF_FREQUENCY_ANA      ((UINT16)  38900)  /* kHz */
//#define C_SI2156_IF_FREQUENCY_L1       ((UINT16)  33900)  /* kHz */
#define C_SI2156_LO_DIVIDER_STEP       ((UINT32) 1000)  /* Hz */
#define C_SI2156_LO_DIVIDER_STEP_ANA   ((UINT16)  1000)  /* Hz */
#define C_SILABS_SI2156_STR                        "SILABS_SI2156"

#define C_SI2156_TOP_SET             ((U8)   0x02) 
#define C_SI2156_TOP_SET_DVBC        ((U8)   0x04) 
#define C_SI2156_TOP_SET_ANA         ((U8)   0x0A) 
#define C_SI2156_SAW_BW              SAW_BW_8M 

#if SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL
#define C_SI2156_AGC_IF_SLP           0.15
#define C_SI2156_AGC_IF_INT           -0.085
#define C_SI2156_AGC_IF_MAX           0.515 //2.5V
#define C_SI2156_AGC_IF_MIN           -0.697 //0.5V 
#else
#define C_SI2156_AGC_IF_SLP           0.25
#define C_SI2156_AGC_IF_INT           0
#define C_SI2156_AGC_IF_MAX           0.998
#define C_SI2156_AGC_IF_MIN           -1
#endif
#define C_SI2156_AGC_IF_SLP_SGN  (S8)((C_SI2156_AGC_IF_SLP>0)?1:-1) 
#define C_SI2156_AGC_IF_LVL_MAX      0.4774    //-90dBm    127/256
#define C_SI2156_AGC_IF_LVL_MIN      0.08203125   //-60dBm  21/256

#define C_SI2156_MAX_IF_GAIN_SET       -0.12   
#define C_SI2156_MIN_IF_GAIN_SET       -0.38   
#define C_SI2156_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_SI2156_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_SI2156_MAX_IF_GAIN_POW       -100    // dBm
#define C_SI2156_MIN_IF_GAIN_POW		-50     // dBm
#define C_SI2156_MAX_RF_GAIN_POW		 C_SI2156_MIN_IF_GAIN_POW
#define C_SI2156_MIN_RF_GAIN_POW		-20     // dBm
#define C_SI2156_POWER_CALIBRATE        26     // dBm


#define C_SI2156_AGC_COEF			((U8)   0xBD)
#define C_SI2156_DEMOD_INPUT_POWER	cIF_TARGET_LEVEL_0CCC_0D36

// ********************************************* //

#define C_U8_SI2156_AGC_IF_SLP         (S8)(C_SI2156_AGC_IF_SLP *64 )  // MT5135(S(1, -6)), -0.125*64 = -8 (0xF8), -8/64 = -1/8
#define C_U8_SI2156_AGC_IF_INT         (S8)(C_SI2156_AGC_IF_INT *16 ) // MT5135(S(3, -4))  0.5*16 = 8, 8/16 =0.5
#define C_U8_SI2156_AGC_IF_MAX         (S8)(C_SI2156_AGC_IF_MAX *128)    //0.499*256 = 0x7F
#define C_U8_SI2156_AGC_IF_MIN         (S8)(C_SI2156_AGC_IF_MIN *128)    //0
#define C_U8_SI2156_AGC_IF_LVL_MAX        (S8)(C_SI2156_AGC_IF_LVL_MAX *256)
#define C_U8_SI2156_AGC_IF_LVL_MIN         (S8)(C_SI2156_AGC_IF_LVL_MIN *256)

#define C_U8_SI2156_MAX_IF_GAIN_SET	(S8)(C_SI2156_MAX_IF_GAIN_SET *256)
#define C_U8_SI2156_MIN_IF_GAIN_SET	(S8)(C_SI2156_MIN_IF_GAIN_SET *256)
#define C_U8_SI2156_MAX_RF_GAIN_SET	(S8)((C_SI2156_MAX_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_SI2156_MIN_RF_GAIN_SET	(S8)((C_SI2156_MIN_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_SI2156_IF_GAIN_RANGE      (S8)(C_SI2156_MIN_IF_GAIN_POW - C_SI2156_MAX_IF_GAIN_POW)
#define C_U8_SI2156_RF_GAIN_RANGE      (S8)(C_SI2156_MIN_RF_GAIN_POW - C_SI2156_MAX_RF_GAIN_POW)

STATIC UINT8* pSI2156[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
STATIC UINT8* pSI2156EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};

#define DAGC_TARGET_LEVEL_H_INDEX 0
#define DAGC_TARGET_LEVEL_L_INDEX 1
/*wenming 12-01-05: don't change top 2 item order*/
STATIC ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if defined(CC_MT5389) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	#if SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL
	{0x794,0x30},
	{0x7a4,0x30},
	{0x7d4,0x04},
	{0x7d5,0xFF},
	{0x7d6,0xFF},
	{0x7d7,0xEC},
	{0x7d8,0xF7},
	{0x7d9,0xF7},
	{0x7da,0x7F},
	{0x7db,0x41},
	{0x7dc,0x80},
	{0x7dd,0xA7},
	{0x7de,0xA7},
	{0x7df,0x11},
	{0x7b7,0x10},
	{0x7b9,0x10},
	{0x13b,0x80},
	{0x790,0x08},
	{0x1b4,0x17},   // Pilot estimator confg.
	#else
	{0x794,0x30},
	{0x7a4,0x30},
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
	#if SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL
	{0x294,0x38},
	{0x2a4,0x28},
	{0x2d4,0x04},
	{0x7d5,0xFF},
	{0x7d6,0xFF},
	{0x7d7,0xEC},
	{0x7d8,0xF7},
	{0x7d9,0xF7},
	{0x7da,0x7F},
	{0x7db,0x41},
	{0x7dc,0x80},
	{0x7dd,0xA7},
	{0x7de,0xA7},
	{0x2df,0x11},
	{0x2b7,0x10},
	{0x2b9,0x10},
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
STATIC ATD_TUNER_REL_REG_T arrAtdRelRegSecam[]={
#if defined(CC_MT5389) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	#if SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL
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
	{0x7df,0x11},
	{0x7b7,0x05},
	{0x7b9,0x05},
	{0x13b,0x81}, /*wenming 11-11-24:bit 0 set 1 for positive modulation*/
	{0x790,0x88},
	{0x1b4,0x17},	// Pilot estimator confg.
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
	{0x1b4,0x17},	// Pilot estimator confg.
	#endif
#else
	#if SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL
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
	{0x2df,0x11},
	{0x2b7,0x05},
	{0x2b9,0x05},
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


#if SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL
static unsigned char DVBT_IF_AMP=45;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBC_IF_AMP=60;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
#else
static unsigned char DVBT_IF_AMP=21;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBC_IF_AMP=26;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
#endif
//static unsigned char DVBT2_IF_AMP=Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBT2_IF_AMP= 45;

static unsigned char NO_L_ANALOG_IF_AMP=200;//Si2156_ATV_LIF_OUT_PROP_AMP_DEFAULT;  //100 -->1Vpp RMS
static unsigned char L_ANALOG_IF_AMP=100;//Si2156_ATV_LIF_OUT_PROP_AMP_DEFAULT;
#if SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL
static unsigned char ATSC_8VSB_IF_AMP = 45;
static unsigned char ATSC_QAM_IF_AMP = 45;
#else
static unsigned char ATSC_8VSB_IF_AMP = 21;
static unsigned char ATSC_QAM_IF_AMP = 21;
#endif
static BOOL TunerInited = FALSE;

#ifdef    Si2156_GET_PROPERTY_STRING
static unsigned int aui2SI2156Properties[] = {
	Si2156_ATV_AFC_RANGE_PROP_CODE,
	Si2156_ATV_AGC_SPEED_PROP_CODE,
	Si2156_ATV_CONFIG_IF_PORT_PROP_CODE,
	Si2156_ATV_EXT_AGC_PROP_CODE,
	Si2156_ATV_IEN_PROP_CODE,
	Si2156_ATV_INT_SENSE_PROP_CODE,
	Si2156_ATV_LIF_FREQ_PROP_CODE,
	Si2156_ATV_LIF_OUT_PROP_CODE,
	Si2156_ATV_RF_TOP_PROP_CODE,
	Si2156_ATV_RSQ_RSSI_THRESHOLD_PROP_CODE,
	Si2156_ATV_VIDEO_MODE_PROP_CODE,
	Si2156_ATV_VSNR_CAP_PROP_CODE,
	Si2156_CRYSTAL_TRIM_PROP_CODE,
	Si2156_DTV_AGC_SPEED_PROP_CODE,
	Si2156_DTV_CONFIG_IF_PORT_PROP_CODE,
	Si2156_DTV_EXT_AGC_PROP_CODE,
	Si2156_DTV_FILTER_SELECT_PROP_CODE,
	Si2156_DTV_IEN_PROP_CODE,
	Si2156_DTV_INITIAL_AGC_SPEED_PROP_CODE,
	Si2156_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE,
	Si2156_DTV_INT_SENSE_PROP_CODE,
	Si2156_DTV_LIF_FREQ_PROP_CODE,
	Si2156_DTV_LIF_OUT_PROP_CODE,
	Si2156_DTV_MODE_PROP_CODE,
	Si2156_DTV_RF_TOP_PROP_CODE,
	Si2156_DTV_RSQ_RSSI_THRESHOLD_PROP_CODE,
	Si2156_MASTER_IEN_PROP_CODE,
	Si2156_TUNER_BLOCKED_VCO_PROP_CODE,
	Si2156_TUNER_IEN_PROP_CODE,
	Si2156_TUNER_INT_SENSE_PROP_CODE,
	Si2156_TUNER_LO_INJECTION_PROP_CODE
	};
static char acSI2156ProMsg[100];
#endif
	
//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------
//-------------------------------------------------------------static  variable----------------------
//Signal Power Sample Array Size
//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------
//-------------------------------------------------------------static  variable----------------------------------------------------------------------------------------------------------------------
//static L1_Si2156_Context api_context={0};
static L1_Si2156_Context api_context;
static L1_Si2156_Context *api;
static UINT8 VideoStandard;

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

//u1PLP0MOD
enum
{
	DVBT2_QPSK =0,
	DVBT2_Q16,
	DVBT2_Q64,
	DVBT2_Q256,
	DVBT2_Qam_Mode_Size 
};

#define DVBT2_Qam_Mode_Unknow	DVBT2_Qam_Mode_Size
//u1PLP0CR
enum
{
	DVBT2_CR_12 = 0,
	DVBT2_CR_35,
	DVBT2_CR_23,
	DVBT2_CR_34,
	DVBT2_CR_45,
	DVBT2_CR_56,
	DVBT2_CR_Mode_Size
};

static INT8 Adjust_i4Power_rec = 6;
//-----------------------------------------------------------------------------
/** Si2158_SetSawBw
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval Saw bandwidth
 */
//-----------------------------------------------------------------------------
static VOID Si2156_SetSawBw(ITUNER_CTX_T * pTCtx, UINT8 sawbw)
{
  pTCtx->specific_member.eu_ctx.m_SAW_BW= sawbw;
  mcSHOW_DBG_MSG(("Si2156_SetSawBw=%d\n", sawbw));
}


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
					cf = pc + C_SI2156_PF_TO_CF_SHIFT_BG_8M*1000;
					mcSHOW_DBG_MSG(("TestLog:Now set 8M PC-CENTER in PC when scan mode\n"));
				}
				else
				{
					if(pc<300000000)  //pal B system
					{
						cf = pc + C_SI2156_PF_TO_CF_SHIFT_BG_7M*1000;
						mcSHOW_DBG_MSG(("TestLog:Now set 7M PC-CENTER in PC\n"));
					}
					else	//pal G system
					{
						cf = pc + C_SI2156_PF_TO_CF_SHIFT_BG_8M*1000;
						mcSHOW_DBG_MSG(("TestLog:Now set 8M PC-CENTER in PC\n"));
					}
				}
			}
			break;
		case MOD_PAL_DK:
			cf = pc + C_SI2156_PF_TO_CF_SHIFT_DK*1000;
			break;
		case MOD_PAL_I:
			cf = pc + C_SI2156_PF_TO_CF_SHIFT_I*1000;
			break;
		case MOD_NTSC_M:
			cf = pc + C_SI2156_PF_TO_CF_SHIFT_M*1000;
			break;
		case MOD_SECAM_L:
			cf = pc + C_SI2156_PF_TO_CF_SHIFT_L*1000;
			break;
		case MOD_SECAM_L1:
			cf = pc - C_SI2156_PF_TO_CF_SHIFT_L1*1000;
			break;
		}
		return cf;
	}

/************************************************************************************************************************
  NAME: Si2156_LoadFirmware
  DESCRIPTON: Load firmware from FIRMWARE_TABLE array in Si2156_Firmware_x_y_build_z.h file into Si2156
              Requires Si2156 to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  Si2156 Context (I2C address)
  Returns:    Si2156/I2C transaction error code, 0 if successful
************************************************************************************************************************/
int Si2156_LoadFirmware        (L1_Si2156_Context *api, unsigned char fw_table[], int nbLines)
{
    int return_code = 0;
    int line;

    //SiTRACE("Si2156_LoadFirmware nbLines %d\n", nbLines);
    mcPRINTF("Si2156_LoadFirmware nbLines %d Begin.......\n", nbLines);
    /* for each 8 bytes in fw_table */
    for (line = 0; line < nbLines; line++)
    {
        /* send that 8 byte I2C command to Si2156 */
        if ((return_code = Si2156_L1_API_Patch(api, 8, fw_table+8*line)) != 0)
        {
          SiTRACE("Si2156_LoadFirmware error 0x%02x patching line %d: %s\n", return_code, line, Si2156_L1_API_ERROR_TEXT(return_code) );
          if (line == 0) {
          SiTRACE("The firmware is incompatible with the part!\n");
          }
          return ERROR_Si2156_LOADING_FIRMWARE;
        }

    }
    mcPRINTF("Si2156_LoadFirmware complete...\n");
    return 0;
}
/************************************************************************************************************************
  NAME: configureSi2156
  DESCRIPTION: Setup Si2156 video filters, GPIOs/clocks, Common Properties startup, Tuner startup, ATV startup, and DTV startup.
  Parameter:  Pointer to Si2156 Context (I2C address)
  Returns:    I2C transaction error code, 0 if successful
************************************************************************************************************************/
int Si2156_Configure           (L1_Si2156_Context *api)
{
    int return_code;
    return_code=0;

    /* Set All Properties startup configuration */
    api->prop->tuner_ien.tcien = Si2156_TUNER_IEN_PROP_TCIEN_ENABLE;
    api->prop->tuner_ien.rssilien = Si2156_TUNER_IEN_PROP_RSSILIEN_DISABLE;
    api->prop->tuner_ien.rssihien = Si2156_TUNER_IEN_PROP_RSSIHIEN_DISABLE;
    if ((return_code=Si2156_L1_SetProperty2(api,Si2156_TUNER_IEN_PROP)) != 0)
    {
       SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
    api->prop->dtv_ien.chlien = Si2156_DTV_IEN_PROP_CHLIEN_ENABLE;
    if ((return_code=Si2156_L1_SetProperty2(api,Si2156_DTV_IEN_PROP)) != 0)
    {
       SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
     api->prop->atv_ien.chlien = Si2156_ATV_IEN_PROP_CHLIEN_ENABLE;
     api->prop->atv_ien.pclien = Si2156_ATV_IEN_PROP_PCLIEN_DISABLE;
    if ((return_code=Si2156_L1_SetProperty2(api,Si2156_ATV_IEN_PROP)) != 0)
    {
       SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
    api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_8MHZ;
    api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_DVBT;
    api->prop->dtv_mode.invert_spectrum = Si2156_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL;
    if ((return_code=Si2156_L1_SetProperty2(api,Si2156_DTV_MODE_PROP)) != 0)
    {
       SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }

	/*wenming 11-11-21: IF2 to main IC, and IF1 to 5135, and use tuner internal AGC control*/
	api->prop->atv_config_if_port.atv_out_type = Si2156_ATV_CONFIG_IF_PORT_PROP_ATV_OUT_TYPE_LIF_DIFF_IF1;
	#if SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL
	api->prop->atv_config_if_port.atv_agc_source = Si2156_ATV_CONFIG_IF_PORT_PROP_ATV_AGC_SOURCE_ALIF_AGC_3DB;
	api->prop->atv_ext_agc.max_10mv = 250;
	api->prop->atv_ext_agc.min_10mv = 50;
	#else
	api->prop->atv_config_if_port.atv_agc_source = Si2156_ATV_CONFIG_IF_PORT_PROP_ATV_AGC_SOURCE_INTERNAL; //Si2156_ATV_CONFIG_IF_PORT_PROP_ATV_AGC_SOURCE_DLIF_AGC_3DB;
	#endif
	if ((return_code=Si2156_L1_SetProperty2(api,Si2156_ATV_CONFIG_IF_PORT_PROP)) != 0)
	{
	   SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
	   return return_code;
	}
	// DTV use IF1 port
	api->prop->dtv_config_if_port.dtv_out_type = Si2156_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
	#if SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL
	api->prop->dtv_config_if_port.dtv_agc_source = Si2156_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_DLIF_AGC_3DB; 
	api->prop->dtv_ext_agc.max_10mv = 250;
	api->prop->dtv_ext_agc.min_10mv = 50;
	#else
	api->prop->dtv_config_if_port.dtv_agc_source = Si2156_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_INTERNAL; //Si2156_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_ALIF_AGC_3DB;
	#endif
	
	if ((return_code=Si2156_L1_SetProperty2(api,Si2156_DTV_CONFIG_IF_PORT_PROP)) != 0)
    {
       SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }

    api->prop->atv_video_mode.color= Si2156_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
    api->prop->atv_video_mode.invert_spectrum= Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL;
    api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_TERRESTRIAL;
    api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
   if ((return_code=Si2156_L1_SetProperty2(api,Si2156_ATV_VIDEO_MODE_PROP)) != 0)
    {
       SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
	api->prop->atv_agc_speed.if_agc_speed= Si2156_ATV_AGC_SPEED_PROP_IF_AGC_SPEED_185; /*wenming 11-12-26: change to 185*/
	   if ((return_code=Si2156_L1_SetProperty2(api,Si2156_ATV_AGC_SPEED_PROP)) != 0)
		{
		   SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
		   return return_code;
		}
    return 0;
}

/** SI2156_TunerInit
 *  Tuner initialzation forSI2156.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */

//-----------------------------------------------------------------------------
VOID SI2156_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{    ITUNER_CTX_T *psTunerCtx = pTCtx;
	#ifdef DTD_INCLUDE
	SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);
	#else
    SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
	#endif

	mcSHOW_DBG_MSG(("SI2156_TunerInit ......\n"));
	if(!TunerInited)
  	{
  		/* Software Init */
		api = &api_context;
		
    	Si2156_L1_API_Init(api,C_SI2156_LO_ADDRESS);
	}

	mcSHOW_DBG_MSG(("The address of api = 0x%x\n", api));
	mcSHOW_DBG_MSG(("api_context.prop.dtv_mode.bw = 0x%x\n", api_context.prop->dtv_mode.bw));

	psTunerCtx->u2LO_Step=100;  //*10 = 1Khz
    psTunerCtx->I2cAddress = C_SI2156_LO_ADDRESS;

    psTunerCtx->u2IF_Freq = C_SI2156_IF_FREQUENCY;//C_SI2156_IF_FREQUENCY;

	psTunerCtx->u2IF_Freq_A = 0; // C_SI2156_IF_FREQUENCY_ANA;//analog if frequency for ATD
	psTunerCtx->u4RF_Freq = 0;

	 // AGC para
    psTunerCtx->sAtdAgcPara.AgcRfBias = 0x00;
    psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x00;
    psTunerCtx->sAtdAgcPara.AgcIfMin  = 0x00; 
    psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B;

	psTunerCtx->u2RetryTVDTm = 100;  /*wenming 11-11-16:retry TVD lock for secam loss issue*/

    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x09; //MASK_PATCH_CR|MASK_PATCH_SENS
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
    #if SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL
	psTunerCtx->fgRFTuner =FALSE;
	#else
	psTunerCtx->fgRFTuner =TRUE;//d120210_Haibo:PGA shound be close loop
	#endif
	psTunerCtx->u1EqIndex=0;//EQ index select
	#ifdef DTD_INCLUDE
	pUSCtx->u2LO_Step = 100; //*10 = 1KHZ
	pUSCtx->m_aucPara_SLD[2] = (U8)C_U8_SI2156_AGC_IF_SLP;
	pUSCtx->m_aucPara_SLD[3] = (U8)C_U8_SI2156_AGC_IF_INT;
	pUSCtx->m_aucPara_SLD[4] = (U8)C_U8_SI2156_AGC_IF_MAX;
	pUSCtx->m_aucPara_SLD[5] = (U8)C_U8_SI2156_AGC_IF_MIN;
	mcSHOW_DBG_MSG(("pUSCtx->m_aucPara_SLD[5]=0x%02X\n",pUSCtx->m_aucPara_SLD[5]));
	#else
	//Tuner frequency range
    pEUCtx->m_s4FreqBoundUpper = C_SI2156_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_SI2156_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_SI2156_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_SI2156_FREQ_DBOUND_LOWER_Ana;

	pEUCtx->m_SAW_BW  = C_SI2156_SAW_BW;
    pEUCtx->m_Ana_Top = C_SI2156_TOP_SET_ANA;	
	// for Signal Level display
	pEUCtx->m_ifAgc_lvl_max = (U8)C_U8_SI2156_AGC_IF_LVL_MAX;
	pEUCtx->m_ifAgc_lvl_min = (U8)C_U8_SI2156_AGC_IF_LVL_MIN;
		
	pEUCtx->m_SigLvTh = 0;
	pEUCtx->m_SigLvScan = 0;
		
	pEUCtx->m_aucPara[0] = C_SI2156_TOP_SET;
	pEUCtx->m_aucPara[1] = 0;
		
	pEUCtx->m_aucPara[2] = (U8)C_U8_SI2156_AGC_IF_SLP;
	pEUCtx->m_aucPara[3] = (U8)C_U8_SI2156_AGC_IF_INT;
	pEUCtx->m_aucPara[4] = (U8)C_U8_SI2156_AGC_IF_MAX;
	pEUCtx->m_aucPara[5] = (U8)C_U8_SI2156_AGC_IF_MIN;
		
	pEUCtx->m_aucPara[6] = C_SI2156_AGC_COEF;
	pEUCtx->m_aucPara[7] = C_SI2156_DEMOD_INPUT_POWER;
		
	pEUCtx->m_aucPara[ 8] = (U8)C_U8_SI2156_MAX_IF_GAIN_SET;
	pEUCtx->m_aucPara[ 9] = (U8)C_U8_SI2156_MIN_IF_GAIN_SET;
	pEUCtx->m_aucPara[10] = (U8)C_U8_SI2156_MAX_RF_GAIN_SET;
	pEUCtx->m_aucPara[11] = (U8)C_U8_SI2156_MIN_RF_GAIN_SET;
	pEUCtx->m_aucPara[12] = (U8)C_U8_SI2156_IF_GAIN_RANGE;
	pEUCtx->m_aucPara[13] = (U8)C_U8_SI2156_RF_GAIN_RANGE;
	pEUCtx->m_aucPara[14] = C_SI2156_POWER_CALIBRATE;
	//IF MAX setting!
	pEUCtx->m_aucPara[21] = 0x7F;
	#endif
		#if defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
		if(IS_IC_5860())
		{
			pSI2156[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BG_5860;
			pSI2156[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DK_5860;
			pSI2156[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156I_5860;
			pSI2156[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156L_5860;	
			pSI2156[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1_5860;
			pSI2156[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]    = EQSi2156M_5860;
		}
		else
		{
			pSI2156[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BG;
			pSI2156[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DK;
			pSI2156[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156I;
			pSI2156[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156L;	
			pSI2156[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1;
			pSI2156[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]    = EQSi2156M;
		}
		#else
			pSI2156[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BG;
			pSI2156[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DK;
			pSI2156[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156I;
			pSI2156[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156L;	
			pSI2156[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1;
			pSI2156[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]    = EQSi2156M;
		#endif
		
        #if defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
			#if defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
			if(IS_IC_5860())
			{
			   	pSI2156EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BGWeak_5860;
				pSI2156EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DKWeak_5860;
				pSI2156EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156IWeak_5860;
				pSI2156EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156LWeak_5860;	
				pSI2156EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1Weak_5860;
				pSI2156EqWeak[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]	= EQSi2156MWeak_5860;
			}
			else
			{
				pSI2156EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BGWeak;
				pSI2156EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DKWeak;
				pSI2156EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156IWeak;
				pSI2156EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156LWeak;	
				pSI2156EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1Weak;
				pSI2156EqWeak[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]	= EQSi2156MWeak;
			}
			#else
				pSI2156EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BGWeak;
				pSI2156EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DKWeak;
				pSI2156EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156IWeak;
				pSI2156EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156LWeak;	
				pSI2156EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1Weak;
				pSI2156EqWeak[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]	= EQSi2156MWeak;
			#endif
		#else
		pSI2156EqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQSi2156BG;
		pSI2156EqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQSi2156DK;
		pSI2156EqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQSi2156I;
		pSI2156EqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQSi2156L;	
		pSI2156EqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQSi2156L1;
		pSI2156EqWeak[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]	= EQSi2156M;
		#endif
			
}
/* Setup properties to switch standards. */
void SetIfDemod(ITUNER_CTX_T *pTCtx,UINT8 subSysIdx,UINT32 Freq,UINT8 fgAutoSearch)
{
    // set a global here to save the video standard,  because it's not always available from the caller.
   	VideoStandard =subSysIdx;
    switch (subSysIdx)
    {
        case MOD_PAL_BG:
			/*wenming 11-12-05:always set sys as G when scan mode to fix DK detected as M/N issue*/
			if(ATV_CONN_MODE_CHAN_SCAN == fgAutoSearch)
			{
				api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
				mcSHOW_DBG_MSG(("TestLog:Now set as GH sys when autosearch\n"));
			}
			else
			{
				if(Freq<300000)  //pal B system
				{
		    		api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_B;
					mcSHOW_DBG_MSG(("TestLog:Now set as B sys\n"));
				}
				else  //pal G system
				{
					api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
					mcSHOW_DBG_MSG(("TestLog:Now set as GH sys\n"));
				}
			}
			api->prop->atv_video_mode.color = Si2156_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_SI2156_IF_CENTER_BG;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }   
              api->prop->atv_lif_out.offset = Si2156_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 4;
            break;
        case MOD_PAL_DK:
            api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_DK;
            api->prop->atv_video_mode.color = Si2156_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_SI2156_IF_CENTER_DK;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }
			 api->prop->atv_lif_out.offset = Si2156_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 4;
            break;
        case MOD_PAL_I:
            api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_I;
            api->prop->atv_video_mode.color = Si2156_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_SI2156_IF_CENTER_I;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }
			 api->prop->atv_lif_out.offset = Si2156_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 4;
            break;
        case MOD_NTSC_M:
            api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_M;
            api->prop->atv_video_mode.color = Si2156_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_SI2156_IF_CENTER_M;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }
			 api->prop->atv_lif_out.offset = Si2156_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 4;
            break;
        case MOD_SECAM_L:
            api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_L;
            api->prop->atv_video_mode.color = Si2156_ATV_VIDEO_MODE_PROP_COLOR_SECAM;
            api->prop->atv_video_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_SI2156_IF_CENTER_L;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }
			 api->prop->atv_lif_out.offset = Si2156_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = L_ANALOG_IF_AMP;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 8;
            break;
        case MOD_SECAM_L1:
            api->prop->atv_video_mode.video_sys = Si2156_ATV_VIDEO_MODE_PROP_VIDEO_SYS_LP;
            api->prop->atv_video_mode.color = Si2156_ATV_VIDEO_MODE_PROP_COLOR_SECAM;
            api->prop->atv_video_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL;
            api->prop->atv_video_mode.trans = Si2156_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_VIDEO_MODE_PROP\n"));
            }
            api->prop->atv_lif_freq.offset = C_SI2156_IF_CENTER_L1;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }
			 api->prop->atv_lif_out.offset = Si2156_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = L_ANALOG_IF_AMP;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
            }

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 8;
            break;
       case MOD_DVBT:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_DVBT;
            if(pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_6M)
				api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
			else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_7M)
				api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_7MHZ;
			else
                api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_8MHZ;
			
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =DVBT_IF_AMP;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
			api->prop->dtv_rf_top.dtv_rf_top= Si2156_DTV_RF_TOP_PROP_DTV_RF_TOP_M2DB;
			 if (Si2156_L1_SetProperty2(api, Si2156_DTV_RF_TOP_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_RF_TOP_PROP\n"));
			}

			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 5;
            break;
       case MOD_DVBT2:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_DVBT;
            //api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_8MHZ;
            /*************************************************************/
            if(pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_6M)
				api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
			else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_7M)
				api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_7MHZ;
			else
                api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_8MHZ;
            /*************************************************************/
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =DVBT2_IF_AMP;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
			api->prop->dtv_rf_top.dtv_rf_top = Si2156_DTV_RF_TOP_PROP_DTV_RF_TOP_M2DB;
			 if (Si2156_L1_SetProperty2(api, Si2156_DTV_RF_TOP_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_RF_TOP_PROP\n"));
			}

			 //wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 5;
            break;
       case MOD_DVBC:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_DVBC;
            api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
           api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =DVBC_IF_AMP;
			//api->prop->dtv_lif_out.amp =32;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
			api->prop->dtv_rf_top.dtv_rf_top = Si2156_DTV_RF_TOP_PROP_DTV_RF_TOP_AUTO;
			 if (Si2156_L1_SetProperty2(api, Si2156_DTV_RF_TOP_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_RF_TOP_PROP\n"));
			}

			 //wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 2;
            break;
      case MOD_ATSC_8VSB:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_ATSC;
            api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =ATSC_8VSB_IF_AMP;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 6;
			//wenming 2012-3-29: for polarity if setting for NA reference
    		pTCtx->bSpecPolarity = 0;
            break;
		case MOD_J83B_AUTO_QAM:
		case MOD_J83B_256QAM:
		case MOD_J83B_64QAM:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_QAM_US;
            api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =ATSC_QAM_IF_AMP;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
			//wenming 2012-3-29: for legacy default DTV PGA index setting
    		pTCtx->uSLD_DefaultPGAIndex = 6;
			//wenming 2012-3-29: for polarity if setting for NA reference
   			pTCtx->bSpecPolarity = 0;
            break;
        case MOD_DTMB:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_DTMB;
            api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
            break;
       case MOD_ISDBT:
            api->prop->dtv_mode.modulation = Si2156_DTV_MODE_PROP_MODULATION_ISDBT;
            api->prop->dtv_mode.bw = Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = Si2156_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (Si2156_L1_SetProperty2(api, Si2156_DTV_MODE_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_MODE_PROP\n"));
            }
            api->prop->dtv_lif_freq.offset=C_SI2156_IF_FREQUENCY;
            if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_FREQ_PROP) != 0)
            {
                mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
            }
			api->prop->dtv_lif_out.offset = Si2156_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
	        if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}
            break;
    }


}
//-----------------------------------------------------------------------------
/** SI2156_TunerSetFreq
 *  Set Tuner PLL forSI2156/SI2156 to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range
                   -1 fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 SI2156_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
  UINT32 Freq = param->Freq; // transfer to video freq(KHz) digital center freq ,analog picture freq
  UINT8 Mode = param->Modulation;    //tv system
  UINT8 AutoSearch=param->fgAutoSearch;
  int timeout =30;
  UINT32 freqHz =0;
  UINT32 u2TickStart_eu;
  UINT32 u2TickTUNINTStart_eu;
  UINT32 u2TickxTVINTStart_eu;
  UINT32 TUNINT_duration =timeout;
  //UINT32 xTVINT_duration =150;
  UINT32 xTVINT_duration =155;//yuan add 5ms for cover "mcGET_SYS_TICK()" resolution
  TV_MODES tvMode;
  int return_code =0;
  
  u2TickStart_eu = mcGET_SYS_TICK();

  if(!TunerInited)
  {
	TunerInited=TRUE;

	//ADD YOU TUNER DRIVER CODE IN HERE
	Printf ("si2156 Tuner Set Freq beging......\n");

	mcSHOW_DBG_MSG(("si2156 Tuner Set Freq check ptr0 \n"));
    if ((return_code=Si2156_pollForCTS(api)) != 0) {
        SiTRACE ("Si2156_pollForCTS error 0x%02x\n", return_code);

    }

	mcSHOW_DBG_MSG(("si2156 Tuner Set Freq check ptr1 \n"));
    if ((return_code = Si2156_L1_POWER_UP (api,
                            Si2156_POWER_UP_CMD_SUBCODE_CODE,
                            Si2156_POWER_UP_CMD_RESERVED1_RESERVED,
                            Si2156_POWER_UP_CMD_RESERVED2_RESERVED,
                            Si2156_POWER_UP_CMD_RESERVED3_RESERVED,
                            Si2156_POWER_UP_CMD_CLOCK_MODE_XTAL,
                            Si2156_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ,
                            Si2156_POWER_UP_CMD_ADDR_MODE_CURRENT,
                            Si2156_POWER_UP_CMD_FUNC_BOOTLOADER,        /* start in bootloader mode */
                            Si2156_POWER_UP_CMD_CTSIEN_DISABLE,
                            Si2156_POWER_UP_CMD_WAKE_UP_WAKE_UP)) != 0)
    {
        SiTRACE ("Si2156_L1_POWER_UP error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );

    }

	mcSHOW_DBG_MSG(("si2156 Tuner Set Freq check ptr2 \n"));
   	/* Get the Part Info from the chip.   This command is only valid in Bootloader mode */
    if ((return_code = Si2156_L1_PART_INFO(api)) != 0) {
        SiTRACE ("Si2156_L1_PART_INFO error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );

    }

	//printf ("Part Number = Si21%d Rev %c%c%c , ROMID= 0x%02x,  %s\n", api->rsp->part_info.part, api->rsp->part_info.chiprev+0x40,api->rsp->part_info.pmajor,api->rsp->part_info.pminor,api->rsp->part_info.romid );

    SiTRACE ("si2156 loadFirmware beginning......\n");

	//if((api->rsp->part_info.pmajor =='3')&&(api->rsp->part_info.pminor =='0'))
	if (api->rsp->part_info.romid == 0x13)
	{
		/*ROM ID = 0X13*/
       	/* Load the firmware patch for B30 */
       	/* Currently the Firmware version is 3.0b3 */
		/* update Firmware version is 3.1b6 for spur and ALIF issues */
		if ((return_code = Si2156_LoadFirmware(api, Si2156_FW_31b6, FIRMWARE_LINES_3_1b6)) != 0) {
         SiTRACE ("Si2156_LoadFirmware error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );
    	}
  	}
  	else
  	{
  		/*ROM ID = 0X12*/
		/* Load the firmware patch for B1A*/
		/* Currently the Firmware version is 1.eb6 */
		if ((return_code = Si2156_LoadFirmware(api, Si2156_FW_1_eb6, FIRMWARE_LINES_1_eb6)) != 0) {
			SiTRACE ("Si2156_LoadFirmware error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );
		}
  	}

	SiTRACE ("si2156 loadeFirmware finished!\n");
    if ((return_code = Si2156_L1_EXIT_BOOTLOADER(api, Si2156_EXIT_BOOTLOADER_CMD_FUNC_TUNER, Si2156_EXIT_BOOTLOADER_CMD_CTSIEN_OFF)) != 0)
    {
        SiTRACE ("Si2156_StartFirmware error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );

    }
    Si2156_Configure (api);
	//SiTRACE ("si2156 init beging......\n");

    //mcSHOW_DBG_MSG(("Init " C_SILABS_SI2156_STR "(Cust tuner)\n"));
  }
  
  pTCtx->u4RF_Freq = Freq;

 #if SI2156_ENABLE_EXTERNAL_ATV_AGC_CONTROL
  mcSHOW_DBG_MSG3(("SI2156:Now is external ATV AGC control!!!\n"));
 #endif
 #if SI2156_ENABLE_EXTERNAL_DTV_AGC_CONTROL
  mcSHOW_DBG_MSG3(("SI2156:Now is external DTV AGC control!!!\n"));
 #endif

 // SPECIFIC_MEMBER_EU_CTX* pEUCtx= &( pTunerCtx->specific_member.eu_ctx);
  SiTRACE ("SI2156_TunerSetFreq freq = %d;Mode=%d\n", Freq,Mode );

  SetIfDemod(pTCtx,Mode,Freq,AutoSearch);
  
  if(Mode == MOD_PAL_BG) /*wenming 11-12-2: BG PC to Center is by 7M or 8M BW*/  
  {
		if(ATV_CONN_MODE_CHAN_SCAN == AutoSearch)
		{
			pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_BG+C_SI2156_PF_TO_CF_SHIFT_BG_8M;
			mcSHOW_DBG_MSG(("TestLog:Now set 8M PC-CENTER when scan mode\n"));
		}
		else
		{
			if(Freq<300000)  //pal B system
			{
				pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_BG+C_SI2156_PF_TO_CF_SHIFT_BG_7M;
				mcSHOW_DBG_MSG(("TestLog:Now set 7M PC-CENTER\n"));
			}
			else            //pal G system
			{
				pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_BG+C_SI2156_PF_TO_CF_SHIFT_BG_8M;
				mcSHOW_DBG_MSG(("TestLog:Now set 8M PC-CENTER\n"));
			}
		}
  }
  
  if(Mode == MOD_PAL_DK)
  pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_DK+C_SI2156_PF_TO_CF_SHIFT_DK;
  if(Mode == MOD_PAL_I)
  pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_I+C_SI2156_PF_TO_CF_SHIFT_I;
  if(Mode == MOD_NTSC_M)
  pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_M+C_SI2156_PF_TO_CF_SHIFT_M;
  if(Mode == MOD_SECAM_L)
  pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_L+C_SI2156_PF_TO_CF_SHIFT_L;
  if(Mode == MOD_SECAM_L1)
  pTCtx->u2IF_Freq_A = C_SI2156_IF_CENTER_L1-C_SI2156_PF_TO_CF_SHIFT_L1;

  
  if (Mode < MOD_TYPE_END)
      tvMode = DIGITAL_MODE;
  else
     { tvMode=ANALOG_MODE;
       if(ATV_CONN_MODE_CHAN_SCAN == AutoSearch)
           {api->prop->atv_afc_range.range_khz= 1000;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_AFC_RANGE_PROP) != 0)
            {mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_AFC_RANGE_PROP\n"));
            }}
			else
			{api->prop->atv_afc_range.range_khz= 1000;
            if (Si2156_L1_SetProperty2(api, Si2156_ATV_AFC_RANGE_PROP) != 0)
            {mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_AFC_RANGE_PROP\n"));
            }}
				
  	}

/* Use Picture Carrier Frequency if ATV and Center Frequency if DTV */
  if (tvMode==ANALOG_MODE)
    freqHz = PCFreq_To_CenterFreq( Mode,Freq * 1000,AutoSearch);
  else
    freqHz = Freq * 1000;

  if (freqHz < Si2156_TUNER_TUNE_FREQ_CMD_FREQ_MIN || freqHz > Si2156_TUNER_TUNE_FREQ_CMD_FREQ_MAX)
	{
		mcSHOW_HW_MSG(("[Silabs]:<= Response out of Range\n"));
		return ERROR_Si2156_PARAMETER_OUT_OF_RANGE; // =1
	}
if (tvMode==ANALOG_MODE)
	{
	 /* set Si2156_TUNER_LO_INJECTION_PROP property for ATV */
	 api->prop->tuner_lo_injection.band_1		= Si2156_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_2	  = Si2156_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_3	   = Si2156_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_4	   = Si2156_TUNER_LO_INJECTION_PROP_BAND_4_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_5	   = Si2156_TUNER_LO_INJECTION_PROP_BAND_5_HIGH_SIDE;
	 if ((Si2156_L1_SetProperty2(api,Si2156_TUNER_LO_INJECTION_PROP)) != 0)
	    {
			 SiTRACE ("Si2156_L1_SetProperty2 Si2156_TUNER_LO_INJECTION_PROP error \n");

		}
	}
	else
	{
	 /* set Si2156_TUNER_LO_INJECTION_PROP property for DTV */
	  api->prop->tuner_lo_injection.band_1		= Si2156_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_2	  = Si2156_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
	 api->prop->tuner_lo_injection.band_3	   = Si2156_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
	 api->prop->tuner_lo_injection.band_4	   = Si2156_TUNER_LO_INJECTION_PROP_BAND_4_LOW_SIDE;
	 api->prop->tuner_lo_injection.band_5	   = Si2156_TUNER_LO_INJECTION_PROP_BAND_5_LOW_SIDE;
	 if ((Si2156_L1_SetProperty2(api,Si2156_TUNER_LO_INJECTION_PROP)) != 0)
	    {
			 SiTRACE ("Si2156_L1_SetProperty2 Si2156_TUNER_LO_INJECTION_PROP error \n");
		}
	}
  
   if (Si2156_L1_TUNER_TUNE_FREQ (api,(tvMode==ANALOG_MODE) ? Si2156_TUNER_TUNE_FREQ_CMD_MODE_ATV : Si2156_TUNER_TUNE_FREQ_CMD_MODE_DTV,  freqHz) != 0)
	{
	    mcSHOW_DBG_MSG(("[Silabs]:Error Si2170_L1_TUNER_TUNE_FREQ\n"));
        return -1;
    }

     /* wait for TUNINT, timeout is 30ms */
	 u2TickTUNINTStart_eu = mcGET_SYS_TICK();
	 while (( mcGET_TICK_PERIOD() * (mcGET_SYS_TICK() - u2TickTUNINTStart_eu)) < (timeout+5) ) //yuan add 5ms for cover "mcGET_SYS_TICK()" resolution
		 {
			 if (( Si2156_L1_CheckStatus(api)) != 0)
			 {
				 mcSHOW_DBG_MSG(("[Silabs]:Error Si2156_L1_CheckStatus\n"));
				 return ERROR_Si2156_SENDING_COMMAND;
			 }
			 if (api->status->tunint)
			 {
				 /* below line for debug only, should be commented in normal mode */
				 TUNINT_duration = (mcGET_SYS_TICK() - u2TickTUNINTStart_eu) * mcGET_TICK_PERIOD();
				 break;
			} 
	 	 }

    /*wait for ATVINT, timeout is 150ms for ATVINT and 10 ms for DTVINT */
		 if (tvMode==ANALOG_MODE)
				{
	timeout = 150; 
			}
		 else
				{
	timeout = 10; 
			}
	  u2TickxTVINTStart_eu = mcGET_SYS_TICK();
	 while (( mcGET_TICK_PERIOD() *( mcGET_SYS_TICK() - u2TickxTVINTStart_eu)) < (timeout+5) ) //yuan add 5ms for cover "mcGET_SYS_TICK()" resolution
		 {
			 if (( Si2156_L1_CheckStatus(api)) != 0)
			 {
				 mcSHOW_DBG_MSG(("[Silabs]:Error Si2156_L1_CheckStatus\n"));
				 return ERROR_Si2156_SENDING_COMMAND;
			 }

        	if (tvMode==ANALOG_MODE)
        	{
                if (api->status->atvint)
					{
						 /* below line for debug only, should be commented in normal mode */
						 xTVINT_duration = (mcGET_SYS_TICK() - u2TickxTVINTStart_eu) * mcGET_TICK_PERIOD();
						 break;
					} 
        	}
        	else
        	{
                if (api->status->dtvint)
					{
						 /* below line for debug only, should be commented in normal mode */
						 xTVINT_duration = (mcGET_SYS_TICK() - u2TickxTVINTStart_eu) * mcGET_TICK_PERIOD();
						 break;
					} 
        	}
	 	}
	 
	 mcSHOW_DBG_MSG(("[Silabs]:SI2156TunerSetFreq use time = (%3u ms) tun=%3u atv=%3u\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD(),TUNINT_duration,xTVINT_duration));
    return (0);
}


/* Get Status information from part */
UCHAR GetStatus( L1_Si2156_Context *api,TUNER_STATUSES status)
{
    int return_code;
    if (status==CHL)
    {
        if ((return_code=Si2156_L1_ATV_STATUS(api,Si2156_ATV_STATUS_CMD_INTACK_CLEAR))!=0) {
             SiTRACE ("Si2156_L1_ATV_STATUS error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );
        }
        return api->rsp->atv_status.chl;
    }
    else if (status == DTV_CHL)
    {
        if ((return_code=Si2156_L1_DTV_STATUS(api,Si2156_DTV_STATUS_CMD_INTACK_CLEAR))!=0) {
         SiTRACE ("Si2156_L1_DTV_STATUS error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );
        }
        return api->rsp->dtv_status.chl;
    }
    else
        return 0;


}

/* Get RSSI  from part */
CHAR GetSignalLevel(L1_Si2156_Context *api)
{
    int return_code;
    if ((return_code=Si2156_L1_TUNER_STATUS(api,Si2156_TUNER_STATUS_CMD_INTACK_CLEAR))!=0) {
     SiTRACE ("Si2156_L1_TUNER_STATUS error 0x%02x: %s\n", return_code, Si2156_L1_API_ERROR_TEXT(return_code) );

    }

    return api->rsp->tuner_status.rssi;

}

//-----------------------------------------------------------------------------
/** SI2156_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *SI2156_TunerGetVer(VOID)
{
    return ((CHAR*)C_SILABS_SI2156_STR);
}



UINT32 SI2156_GetSSI(ITUNER_CTX_T* pTunerCtx) 
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
    mcSHOW_DBG_MSG(("DVBT: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

   	
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
	//i4Power_rec = i4Power_rec - 6;  //adjust bu test result;
	if(pTunerCtx->sSSIPara.i2Bw == SAW_BW_8M)
	{
        i4Power_rec = i4Power_rec - Adjust_i4Power_rec;
    }
    else
    {
        i4Power_rec = i4Power_rec - Adjust_i4Power_rec;
    }
    i4Power_rel = i4Power_rec - i4Power_ref;
	mcSHOW_DBG_MSG(("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
    
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
    pCtx->m_SigLvScan = (S8) u4SSI_Indicator;
    mcSHOW_DBG_MSG(("u4SSI_Indicator = %d",u4SSI_Indicator));
    return u4SSI_Indicator;
}

UINT32 SI2156_GetSSI_DVBC(ITUNER_CTX_T* pTunerCtx)
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
    mcSHOW_DBG_MSG(("DVBC: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));


	i4Power_rec=GetSignalLevel(api);;
	//i4Power_rec+=fgParaA_DVBC;
	//i4Power_rec+=6;//compensate
	refIndex=pTunerCtx->sSSIPara.i2Mod;
	mcSHOW_DBG_MSG(("refIndex=%d",refIndex));
	if(refIndex>4)
	{
		mcSHOW_DBG_MSG(("DVBC: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
		return u4SSI_Indicator;
	}
	else
	{
		i4Power_ref = Power_Ref[refIndex];
		mcSHOW_DBG_MSG(("i4Power_ref=%d\n",i4Power_ref));
	}
    i4Power_rel = i4Power_rec - i4Power_ref;
	mcSHOW_DBG_MSG(("DVBC: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
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


UINT32 Si2156_GetSSI_T2(ITUNER_CTX_T* pTunerCtx) 
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
    mcSHOW_DBG_MSG(("DVBT: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

   	
    /* Get Power_Rec Value */
	i4Power_rec=((INT32)GetSignalLevel(api))*10;
   

	/*Get Ref Value-Start*/
	if(pTunerCtx->sSSIPara.i2Mod==DVBT2_Qam_Mode_Unknow)
	{
		//mcSHOW_DBG_MSG1(("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
		//printf("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel);
		return u4SSI_Indicator;
	}
	else
	{
		i4Power_ref = Power_Ref[(pTunerCtx->sSSIPara.i2Mod)*DVBT2_CR_Mode_Size + pTunerCtx->sSSIPara.i2CR];
	}


    
    /* Get Power Rel Value */
	i4Power_rec = i4Power_rec - 2610;  //adjust bu test result;
    i4Power_rel = i4Power_rec - i4Power_ref;
	mcSHOW_DBG_MSG(("DVBT: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
    
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
    pCtx->m_SigLvScan = (S8) u4SSI_Indicator;
    mcSHOW_DBG_MSG(("u4SSI_Indicator = %d",u4SSI_Indicator));
    return u4SSI_Indicator;
}

//----------------------------------------------------------------------------- 
/** SI2156_SetSSICondition
 *  Set SSI Condition for TH2603
 *  @param  
 *  @retval void
 */
//-----------------------------------------------------------------------------
void SI2156_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);
    pTunerCtx->sSSIPara.i2Bw = *((INT16*) pInOutVal+5);
    
}

//-----------------------------------------------------------------------------
/** SI2156_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 SI2156_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){

    //L1_Si2156_Context *api;
    int return_code =0;
	
	//api->prop->dtv_mode.bw = 8;

    switch(eOperation){
    case itSetSSICond:
		SiTRACE ("Si2156_itSetSSICond ...........\n");
        SI2156_SetSSICondition(pTCtx, pInOutVal);
        break;
    case itGetEqScriptNormal: 
		SiTRACE ("Si2156_itGetEqScriptNormal ...........\n");
        *(UINT8**)pInOutVal = pSI2156[SetVal];;
        break;
    case itGetEqScriptWeak: 
		SiTRACE ("Si2156_itGetEqScriptWeak ...........\n");
        *(UINT8**)pInOutVal = pSI2156EqWeak[SetVal];
        break;
	case itGetTunerRelReg://Get ATD Tuner Rel register setting
        {
			SiTRACE ("Si2156_itGetTunerRelReg ...........\n");
			
            if((SetVal==MOD_SECAM_L)||(SetVal==MOD_SECAM_L1))
            {
                *(ATD_TUNER_REL_REG_T **)pInOutVal = arrAtdRelRegSecam;
            }
            else
            {
            	#if defined(CC_MT5389) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
				/*wenming 12-1-5:for DAGC setting*/
				UINT8 subSysId = SetVal;
				UINT8 u1DagcTargetLevel_H,u1DagcTargetLevel_L;
				u1DagcTargetLevel_H = 0x30;
				u1DagcTargetLevel_L = 0x30;
				
				switch(subSysId)
				{
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
				}
				arrAtdRelReg[DAGC_TARGET_LEVEL_H_INDEX].u1RegVal=u1DagcTargetLevel_H;
			    arrAtdRelReg[DAGC_TARGET_LEVEL_L_INDEX].u1RegVal=u1DagcTargetLevel_L;
				#endif
                *(ATD_TUNER_REL_REG_T **)pInOutVal = arrAtdRelReg;
            }
        }
        break;
	/*
	case itTagetLevel:  //set if input target level
				{
					if((SetVal==MOD_SECAM_L)||(SetVal==MOD_SECAM_L1))
						pInOutVal = 0x05;
					else
						pInOutVal = 0x10;}
				break;
				*/
    case itGetVer:
		SiTRACE ("Si2156_itGetVer ...........\n");
        {CHAR ** RetStr=(CHAR **)pInOutVal;
        *RetStr=SI2156_TunerGetVer();
		//CHAR ** RetStr = (CHAR **)pInOutVal;
        //*RetStr = SI2156_GetSwVer();
    	}
        break;
	case itGetTop:
		SiTRACE ("Si2156_itGetTop ...........\n");
		
          if (SetVal >= MOD_ANA_TYPE_BEGIN)
             {
              if ((return_code=Si2156_L1_GetProperty2(api,Si2156_ATV_RF_TOP_PROP))!=0)
               {
                    SiTRACE ("Si2156_L1_GetProperty2 error 0x%02x\n", return_code);
                    return ITUNER_NOT_OK;
               }
                *(UINT8 *)pInOutVal= api->prop->atv_rf_top.atv_rf_top;
             }
             else
             {
              if ((return_code=Si2156_L1_GetProperty2(api,Si2156_DTV_RF_TOP_PROP))!=0)
               {
                    SiTRACE ("Si2156_L1_GetProperty2 error 0x%02x\n", return_code);
                    return ITUNER_NOT_OK;
               }
                *(UINT8 *)pInOutVal= api->prop->dtv_rf_top.dtv_rf_top;
             }

     break;
     case itSetIfDemod:
	 	SiTRACE ("Si2156_itSetIfDemod ...........\n");
        //    SetIfDemod(SetVal);
     break;
    case itSetTop:
     {
	 	SiTRACE ("Si2156_itSetTop ...........\n");
          if(VideoStandard >= MOD_ANA_TYPE_BEGIN)
          {
            api->prop->atv_rf_top.atv_rf_top=SetVal;
            if ((return_code=Si2156_L1_SetProperty2(api,Si2156_ATV_RF_TOP_PROP)) != 0)
            {
               SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
               return ITUNER_NOT_OK;
            }
          }
          else
          {
            api->prop->dtv_rf_top.dtv_rf_top=SetVal;
            if ((return_code=Si2156_L1_SetProperty2(api,Si2156_DTV_RF_TOP_PROP)) != 0)
            {
               SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
               return ITUNER_NOT_OK;
            }
          }
     }
	 break;
	 case itSetSawBw://Set Channel Bandwidth
	 	 SiTRACE ("Si2156_itSetSawBw ...........\n");

		 /*
         pTCtx->specific_member.eu_ctx.m_SAW_BW = SetVal;

		 SiTRACE ("pTCtx->specific_member.eu_ctx.m_SAW_BW = %d\n", pTCtx->specific_member.eu_ctx.m_SAW_BW);
		 SiTRACE ("api->prop->dtv_mode.bw = %d\n", api->prop->dtv_mode.bw);
		 
		 if(SetVal== SAW_BW_6M)
			api->prop->dtv_mode.bw =Si2156_DTV_MODE_PROP_BW_BW_6MHZ;
		 else if(SetVal== SAW_BW_7M)
			api->prop->dtv_mode.bw =Si2156_DTV_MODE_PROP_BW_BW_7MHZ;
         else
	      	api->prop->dtv_mode.bw =Si2156_DTV_MODE_PROP_BW_BW_8MHZ;

		 SiTRACE ("[MS]api->prop->dtv_mode.bw = %d\n", api->prop->dtv_mode.bw); 
		 SiTRACE ("[MS]api->prop->dtv_mode.modulation = %d\n", api->prop->dtv_mode.modulation);
		 //SiTRACE ("api->prop->dtv_mode.bw = %d\n", api->prop->dtv_mode.bw);
		 
         if ((return_code=Si2156_L1_SetProperty2(api,Si2156_DTV_MODE_PROP)) != 0)
         {
            SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
            return ITUNER_NOT_OK;
         }*/
        /****************************************************************/
		{
			Si2156_SetSawBw(pTCtx,SetVal);
	 	}
		 
		SiTRACE ("Si2156_itSetSawBw bw is %d ...........\n", SetVal);
	  break;
	  case itGetStatus: // return channel locked status.
	  	SiTRACE ("Si2156_itGetStatus ...........\n");
		
          if (SetVal >= MOD_ANA_TYPE_BEGIN)
            *(UINT8 *)pInOutVal = GetStatus( api,CHL);
          else
            *(UINT8 *)pInOutVal = GetStatus( api,DTV_CHL);
	  break;
	  case itGetSSIIndex: // Get RSSI
	  	{
	  	SiTRACE ("Si2156_itGetSSIIndex ...........\n");
		 if (SetVal==0)
             *((INT32*)pInOutVal) =SI2156_GetSSI(pTCtx);
         else
             *((INT32*)pInOutVal) =SI2156_GetSSI_DVBC(pTCtx);
	  	}
	  break;
	  case itGetSSIIndex_T2: // Get RSSI
            *(INT32 *)pInOutVal = Si2156_GetSSI_T2(pTCtx);//(INT16)GetSignalLevel(api);
	  break;
	  case itGetSignalLevel: // Get RSSI
	  	SiTRACE ("Si2156_itGetSignalLevel ...........\n");
		
        *(INT32 *)pInOutVal = (108+(INT8)GetSignalLevel(api));// use dBuV to show signal level
		SiTRACE("Si2156_GetSignalLevel= %d\n",(108 +(INT8) GetSignalLevel(api)));
	  break;	
	  case itSetIF: // set ATV/DTV LIF FREQ
	  	  SiTRACE ("Si2156_itSetIF ...........\n");
		
          if (VideoStandard >= MOD_ANA_TYPE_BEGIN)
             {
                 api->prop->atv_lif_freq.offset = SetVal;
                if ((return_code=Si2156_L1_SetProperty2(api,Si2156_ATV_LIF_FREQ_PROP)) != 0)
                {
                   SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
                   return ITUNER_NOT_OK;
                }

             }
             else
            {
                 api->prop->dtv_lif_freq.offset = SetVal;
                if ((return_code=Si2156_L1_SetProperty2(api,Si2156_DTV_LIF_FREQ_PROP)) != 0)
                {
                   SiTRACE ("Si2156_L1_SetProperty2 error 0x%02x\n", return_code);
                   return ITUNER_NOT_OK;
                }

            }
	  break;
	  case itSetIFAMP:	  
	  	{
			mcSHOW_DBG_MSG(("---------IF_AMP set as %d \n",SetVal));


			if(SetVal<=255)
	  		{
	  	if(VideoStandard== MOD_DVBT)
			{DVBT_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =DVBT_IF_AMP;	    
			if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}	
		    mcSHOW_DBG_MSG(("DVBT_IF_AMP set as %d \n",DVBT_IF_AMP));}
	    else if(VideoStandard== MOD_DVBT2)
			{DVBT2_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =DVBT2_IF_AMP;
			if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}	
		    mcSHOW_DBG_MSG(("DVBT2_IF_AMP set as %d \n",DVBT2_IF_AMP));}
		else if(VideoStandard== MOD_DVBC)
			{DVBC_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =DVBC_IF_AMP;
			if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}	
			mcSHOW_DBG_MSG(("DVBC_IF_AMP set as %d \n",DVBC_IF_AMP));}
		else if((VideoStandard== MOD_SECAM_L)||(VideoStandard==MOD_SECAM_L1))
			{L_ANALOG_IF_AMP=SetVal;
			api->prop->atv_lif_out.amp =L_ANALOG_IF_AMP;
			if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
			}	
			mcSHOW_DBG_MSG(("L_ANALOG_IF_AMP set as %d \n",NO_L_ANALOG_IF_AMP));}
		else if((VideoStandard>=MOD_ANA_TYPE_BEGIN) && (VideoStandard<MOD_TYPE_UNKNOWN))
			{NO_L_ANALOG_IF_AMP=SetVal;
			api->prop->atv_lif_out.amp =NO_L_ANALOG_IF_AMP;
			if (Si2156_L1_SetProperty2(api, Si2156_ATV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_ATV_LIF_FREQ_PROP\n"));
			}			
			mcSHOW_DBG_MSG(("NO_L_ANALOG_IF_AMP set as %d \n",NO_L_ANALOG_IF_AMP));}
	    else if(MOD_ATSC_8VSB==VideoStandard)
			{
			ATSC_8VSB_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =ATSC_8VSB_IF_AMP;
			if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}	
		    mcSHOW_DBG_MSG(("ATSC_8VSB_IF_AMP set as %d \n",ATSC_8VSB_IF_AMP));
			}
		else if((VideoStandard==MOD_J83B_64QAM) || (VideoStandard==MOD_J83B_AUTO_QAM) || (VideoStandard==MOD_J83B_256QAM))
		   {
		     ATSC_QAM_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =ATSC_QAM_IF_AMP;
			if (Si2156_L1_SetProperty2(api, Si2156_DTV_LIF_OUT_PROP) != 0)
			{
				mcSHOW_HW_MSG((" [Silabs]: ERROR_SETTING_PROPERTY Si2156_DTV_LIF_FREQ_PROP\n"));
			}	
		    mcSHOW_DBG_MSG(("ATSC_QAM_IF_AMP set as %d \n",ATSC_QAM_IF_AMP));
			}
		else
			mcSHOW_DBG_MSG(("TVStandard unknown pls set a channel at first"));
		  	}
		}
        break;
		
	case itGetTunerAFCKhz:
	  	{	  		
	  		unsigned char intack=Si2156_ATV_STATUS_CMD_INTACK_OK;
            S32 TunerAFCKhz = 5000;

			SiTRACE ("Si2156_itGetTunerAFCKhz ...........\n");
			  
            if ((Si2156_L1_ATV_STATUS(api,intack) != 0))
        	   {
        	           mcSHOW_DBG_MSG(("[Silabs]:Error reading STATUS\n"));
        	    }
			else{
				//if(api->status->atvint)
				if(api->rsp->atv_status.pcl)  /*wenming 11-11-25: for weak signal loss issue, side effect: scan may slow down*/
                { 
                  TunerAFCKhz = (S32)api->rsp->atv_status.afc_freq;
				  mcSHOW_DBG_MSG(("[Silabs]:Tuner ATV lock and TunerAFCKhz = %d\n",TunerAFCKhz));
				}
				else
					{
					mcSHOW_DBG_MSG(("[Silabs]:Tuner ATV unlock set TunerAFCKhz = %d,to let demod go ,,\n",TunerAFCKhz));
					}
			   }
			*(S32*)pInOutVal=TunerAFCKhz;
			
		}
        break;
		#ifdef    Si2156_GET_PROPERTY_STRING
	case itGetReg:
		{
			UINT8 ucTemp = 0;
			UINT8 ucDumpSettingCount = 0;
			
			SiTRACE ("Si2156_itGetReg ...........\n");
			
			ucDumpSettingCount = sizeof(aui2SI2156Properties)/sizeof(unsigned int);
			mcSHOW_DBG_MSG(("\n"));
			for(ucTemp=0;ucTemp<ucDumpSettingCount;ucTemp++)
			{
				Si2156_L1_GetPropertyString(api,aui2SI2156Properties[ucTemp],(char*)"\n",acSI2156ProMsg);
				mcSHOW_DBG_MSG1(("%s\n",acSI2156ProMsg));
			}
		}
	    break;
		#endif

     case itSetSSIAdjust:
        {
            mcSHOW_DBG_MSG1(("---------Adjust_i4Power_rec set as %d \n",SetVal));
            Adjust_i4Power_rec = SetVal;
        }
        break;

#ifdef CC_SUPPORT_STR
    case itSetTunerInit:
        //TunerInited = ((SetVal > 0) ?: TRUE,FALSE);
		TunerInited = ((SetVal > 0) ? TRUE:FALSE);// build warning 
        break;
#endif
            
    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}
