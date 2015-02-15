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
 * $RCSfile: pi_tuner_TDSY_G230D.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/* Silabs version 0C build 5  4/2012 */
// changes tagged with Silabs_120425
/** @file pi_tuner_TDSY_G230D.c
 *  Tuner control for SS TDSY_G230D
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

#include "TDSY_G230D_L1_API.h"
#include "TDSY_G230D_firmware_0_C_build_8.h"  //Silabs_120425 - new firmware version 0Cb5
#include "TDSY_G230D_firmware_0_E_build_15.h"  //Silabs_120820 - new firmware version 0Eb15 (TDSY_G230D-A0E)
// Currently TDSY_G230D-A20 boots from NVM with no patch requirement.   A dummy patch (no download) is implemented for easy replacement in the future.
#include "TDSY_G230D_firmware_2_1_build_8.h"   //Silabs_121115 patch for ROM33 to resolve Air plane flutter issue

#include "TDSY_G230D_i2c_api.h"
#include "eq_script_TDSY_G230D.h"

#define TDSY_G230D_BYTES_PER_LINE 8

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

#define TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL 1//for TDSY_G230D ATV AGC source selection, 0 for internal, 1 for external
#define TDSY_G230D_ENABLE_EXTERNAL_DTV_AGC_CONTROL 1//for TDSY_G230D DTV AGC source selection, 0 for internal, 1 for external

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Frequency boundary .Get from tuner spec
#define C_TDSY_G230D_FREQ_DBOUND_UPPER   859*1000*1000
#define C_TDSY_G230D_FREQ_DBOUND_LOWER    48*1000*1000
#define C_TDSY_G230D_FREQ_DBOUND_UPPER_Ana     875*1000*1000
#define C_TDSY_G230D_FREQ_DBOUND_LOWER_Ana    45*1000*1000

//PLL lock check parameters
#define C_TDSY_G230D_POLL_INTERVAL      5 //ms
#define C_TDSY_G230D_PLL_POLL_TIMETOUT      100 //ms
#define C_TDSY_G230D_PLL_POLL_CNT           C_TDSY_G230D_PLL_POLL_TIMETOUT/C_TDSY_G230D_POLL_INTERVAL //counter

#define C_TDSY_G230D_PF_TO_CF_SHIFT_BG_7M 2250 /*KHZ*/  //PF to CF is 2250 when 7M B
#define C_TDSY_G230D_PF_TO_CF_SHIFT_BG_8M 2750 /*KHZ*/  //PF to CF is 2750 when 8M G
#define C_TDSY_G230D_PF_TO_CF_SHIFT_DK 2750 /*KHZ*/
#define C_TDSY_G230D_PF_TO_CF_SHIFT_I  2750 /*KHZ*/
#define C_TDSY_G230D_PF_TO_CF_SHIFT_L  2750 /*KHZ*/
#define C_TDSY_G230D_PF_TO_CF_SHIFT_M  1750 /*KHZ*/
#define C_TDSY_G230D_PF_TO_CF_SHIFT_L1 2750 /*KHZ*/
#define C_TDSY_G230D_IF_CENTER_BG 5000 /*KHZ*/
#define C_TDSY_G230D_IF_CENTER_DK 5000 /*KHZ*/
#define C_TDSY_G230D_IF_CENTER_I  5000 /*KHZ*/
#define C_TDSY_G230D_IF_CENTER_L  5000 /*KHZ*/
#define C_TDSY_G230D_IF_CENTER_L1 5000 /*KHZ*/
#define C_TDSY_G230D_IF_CENTER_M  5000 /*KHZ*/

#define C_TDSY_G230D_IF_FREQUENCY          ((UINT16)  5000)  /* kHz */
#define C_TDSY_G230D_LO_ADDRESS           ((UINT8)  0xC0)

#define C_TDSY_G230D_LO_DIVIDER_STEP       ((UINT32) 1000)  /* Hz */
#define C_TDSY_G230D_LO_DIVIDER_STEP_ANA   ((UINT16)  1000)  /* Hz */
#define C_SILABS_TDSY_G230D_STR            "SILABS_TDSY_G230D-A20 21B8\n"

#define C_TDSY_G230D_TOP_SET             ((UINT8)   0x02) 
#define C_TDSY_G230D_TOP_SET_DVBC        ((UINT8)   0x04) 
#define C_TDSY_G230D_TOP_SET_ANA         ((UINT8)   0x0A) 
#define C_TDSY_G230D_SAW_BW              SAW_BW_8M 

#if TDSY_G230D_ENABLE_EXTERNAL_DTV_AGC_CONTROL
#define C_TDSY_G230D_AGC_IF_SLP           0.19
#define C_TDSY_G230D_AGC_IF_INT           -0.24
#define C_TDSY_G230D_AGC_IF_MAX           0.52
#define C_TDSY_G230D_AGC_IF_MIN           -1
#else
#define C_TDSY_G230D_AGC_IF_SLP           0.125	
#define C_TDSY_G230D_AGC_IF_INT           0.5   
#define C_TDSY_G230D_AGC_IF_MAX           0.499  // 0.5 will overflow  //0v
#define C_TDSY_G230D_AGC_IF_MIN          0.0  
#endif
#define C_TDSY_G230D_AGC_IF_SLP_SGN  (INT8)((C_TDSY_G230D_AGC_IF_SLP>0)?1:-1) 
#define C_TDSY_G230D_AGC_IF_LVL_MAX      0.4774    //-90dBm    127/256
#define C_TDSY_G230D_AGC_IF_LVL_MIN      0.08203125   //-60dBm  21/256

#define cALC_ADC_INPUT_SWING             2  //Volt
#define cALC_ADC_COMM_VOLT               1.2
#define cALC_ADC_BIAS                    (cALC_ADC_COMM_VOLT - cALC_ADC_INPUT_SWING/2)

#define C_TDSY_G230D_MAX_IF_GAIN_SET       -0.12   
#define C_TDSY_G230D_MIN_IF_GAIN_SET       -0.38   
#define C_TDSY_G230D_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_TDSY_G230D_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_TDSY_G230D_MAX_IF_GAIN_POW       -100    // dBm
#define C_TDSY_G230D_MIN_IF_GAIN_POW		-50     // dBm
#define C_TDSY_G230D_MAX_RF_GAIN_POW		 C_TDSY_G230D_MIN_IF_GAIN_POW
#define C_TDSY_G230D_MIN_RF_GAIN_POW		-20     // dBm
#define C_TDSY_G230D_POWER_CALIBRATE        26     // dBm

#define C_TDSY_G230D_AGC_COEF			((UINT8)   0xBD)
#define C_TDSY_G230D_DEMOD_INPUT_POWER	cIF_TARGET_LEVEL_0CCC_0D36

// ********************************************* //

#define C_U8_TDSY_G230D_AGC_IF_SLP         (INT8)(C_TDSY_G230D_AGC_IF_SLP *64 )  // MT5135(S(1, -6)), -0.125*64 = -8 (0xF8), -8/64 = -1/8
#define C_U8_TDSY_G230D_AGC_IF_INT         (INT8)(C_TDSY_G230D_AGC_IF_INT *16 ) // MT5135(S(3, -4))  0.5*16 = 8, 8/16 =0.5
#define C_U8_TDSY_G230D_AGC_IF_MAX         (INT8)(C_TDSY_G230D_AGC_IF_MAX *128)    //0.499*256 = 0x7F
#define C_U8_TDSY_G230D_AGC_IF_MIN         (INT8)(C_TDSY_G230D_AGC_IF_MIN *128)    //0
#define C_U8_TDSY_G230D_AGC_IF_LVL_MAX        (INT8)(C_TDSY_G230D_AGC_IF_LVL_MAX *256)
#define C_U8_TDSY_G230D_AGC_IF_LVL_MIN         (INT8)(C_TDSY_G230D_AGC_IF_LVL_MIN *256)

#define C_U8_TDSY_G230D_MAX_IF_GAIN_SET	(INT8)(C_TDSY_G230D_MAX_IF_GAIN_SET *256)
#define C_U8_TDSY_G230D_MIN_IF_GAIN_SET	(INT8)(C_TDSY_G230D_MIN_IF_GAIN_SET *256)
#define C_U8_TDSY_G230D_MAX_RF_GAIN_SET	(INT8)((C_TDSY_G230D_MAX_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_TDSY_G230D_MIN_RF_GAIN_SET	(INT8)((C_TDSY_G230D_MIN_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_U8_TDSY_G230D_IF_GAIN_RANGE      (INT8)(C_TDSY_G230D_MIN_IF_GAIN_POW - C_TDSY_G230D_MAX_IF_GAIN_POW)
#define C_U8_TDSY_G230D_RF_GAIN_RANGE      (INT8)(C_TDSY_G230D_MIN_RF_GAIN_POW - C_TDSY_G230D_MAX_RF_GAIN_POW)

static UINT8* pTDSY_G230D[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pTDSY_G230DEqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};

static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
    #if TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL
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
	{0x7d5,0xf9},
	{0x7d6,0xf9},
	{0x7d7,0xEC},
	{0x7d8,0xfc},
	{0x7d9,0xfc},
	{0x7da,0x7F},
	{0x7db,0xed},
	{0x7dc,0x80},
	{0x7dd,0x9f},
	{0x7de,0x9f},
	{0x7df,0x00},
	{0x7b7,0x10},
	{0x7b9,0x0e},
	{0x13b,0x80},
	{0x790,0x08},
	{0x1b4,0x17},   // Pilot estimator confg.
	#endif

#else

	#if TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL
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
	#if TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL
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
	{0x13b,0x81}, /*wenming 11-11-24:bit 0 set 1 for positive modulation*/
	{0x790,0x88},
	{0x1b4,0x17},	// Pilot estimator confg.
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
	{0x1b4,0x17},	// Pilot estimator confg.
	#endif

#else

#if TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL
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
#if TDSY_G230D_ENABLE_EXTERNAL_DTV_AGC_CONTROL
static unsigned char DVBT_IF_AMP=55;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBT2_IF_AMP=55;
static unsigned char DVBC_IF_AMP=60;//Si2156_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char ATSC_8VSB_IF_AMP = 70;
static unsigned char ATSC_QAM_IF_AMP = 70;

#else
static unsigned char DVBT_IF_AMP=21;//TDSY_G230D_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char DVBT2_IF_AMP=21;
static unsigned char DVBC_IF_AMP=26;//TDSY_G230D_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static unsigned char ATSC_8VSB_IF_AMP = 21;
static unsigned char ATSC_QAM_IF_AMP = 21;

#endif

static unsigned char NO_L_ANALOG_IF_AMP=160;//TDSY_G230D_ATV_LIF_OUT_PROP_AMP_DEFAULT;  //100 -->1Vpp RMS
static unsigned char L_ANALOG_IF_AMP=100;//SECAML/L1


static unsigned char NO_L_DIGITAL_IF_AMP=TDSY_G230D_DTV_LIF_OUT_PROP_AMP_DEFAULT;
static BOOL TunerInited = FALSE;

#ifdef    TDSY_G230D_GET_PROPERTY_STRING
static unsigned int aui2TDSY_G230DProperties[] = {
	TDSY_G230D_ATV_AFC_RANGE_PROP_CODE,
	TDSY_G230D_ATV_AGC_SPEED_PROP_CODE,
	TDSY_G230D_ATV_CONFIG_IF_PORT_PROP_CODE,
	TDSY_G230D_ATV_EXT_AGC_PROP_CODE,
	TDSY_G230D_ATV_IEN_PROP_CODE,
	TDSY_G230D_ATV_INT_SENSE_PROP_CODE,
	TDSY_G230D_ATV_LIF_FREQ_PROP_CODE,
	TDSY_G230D_ATV_LIF_OUT_PROP_CODE,
	TDSY_G230D_ATV_RF_TOP_PROP_CODE,
	TDSY_G230D_ATV_RSQ_RSSI_THRESHOLD_PROP_CODE,
	TDSY_G230D_ATV_VIDEO_MODE_PROP_CODE,
	TDSY_G230D_ATV_VSNR_CAP_PROP_CODE,
	TDSY_G230D_CRYSTAL_TRIM_PROP_CODE,
	TDSY_G230D_DTV_AGC_SPEED_PROP_CODE,
	TDSY_G230D_DTV_CONFIG_IF_PORT_PROP_CODE,
	TDSY_G230D_DTV_EXT_AGC_PROP_CODE,
	TDSY_G230D_DTV_FILTER_SELECT_PROP_CODE,
	TDSY_G230D_DTV_IEN_PROP_CODE,
	TDSY_G230D_DTV_INITIAL_AGC_SPEED_PROP_CODE,
	TDSY_G230D_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE,
	TDSY_G230D_DTV_INT_SENSE_PROP_CODE,
	TDSY_G230D_DTV_LIF_FREQ_PROP_CODE,
	TDSY_G230D_DTV_LIF_OUT_PROP_CODE,
	TDSY_G230D_DTV_MODE_PROP_CODE,
	TDSY_G230D_DTV_RF_TOP_PROP_CODE,
	TDSY_G230D_DTV_RSQ_RSSI_THRESHOLD_PROP_CODE,
	TDSY_G230D_MASTER_IEN_PROP_CODE,
	TDSY_G230D_TUNER_BLOCKED_VCO_PROP_CODE,
	TDSY_G230D_TUNER_IEN_PROP_CODE,
	TDSY_G230D_TUNER_INT_SENSE_PROP_CODE,
	TDSY_G230D_TUNER_LO_INJECTION_PROP_CODE
	};
static char acTDSY_G230DProMsg[100];
#endif

static INT8 Adjust_G230d_i4Power_rec = 0;

	
//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------
//-------------------------------------------------------------static  variable----------------------
//Signal Power Sample Array Size
//-----------------------------------------------------------------------------
// pi  constant definition
//-----------------------------------------------------------------------------
//-------------------------------------------------------------static  variable----------------------------------------------------------------------------------------------------------------------
static L1_TDSY_G230D_Context api_context={0};
static L1_TDSY_G230D_Context *api;
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

/************************************************************************************************************************
  NAME: PCFreq_To_CenterFreq
  DESCRIPTION: Convert Picture Carrier frequency to Center Frequency
  Parameter:  System
  Parameter:  PC Frequency
  Returns:    I2C transaction error code, 0 if successful
************************************************************************************************************************/
static UINT32 PCFreq_To_CenterFreq(UINT8 u1SubSysIdx,UINT32 pc,UINT8 fgAutoSearch)
{
	UINT32 cf=0;
	switch (u1SubSysIdx)
	{
		case MOD_PAL_BG:  /*wenming 11-12-2: BG PC to Center is by 7M or 8M BW*/
			{
				if(ATV_CONN_MODE_CHAN_SCAN == fgAutoSearch) /*wenming 12-4-16: fgAutoSearch get from MW, ignore driver fgAutoSearch setting*/
				{
					cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_BG_8M*1000;
					mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER in PC when scan mode\n"));
				}
				else
				{
					if(pc<300000000)  //pal B system
					{
						cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_BG_7M*1000;
						mcDBG_MSG_L2(("TestLog:Now set 7M PC-CENTER in PC"));
					}
					else    //pal G system
					{
						cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_BG_8M*1000;
						mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER in PC"));
					}
				}
			}
			break;
		case MOD_PAL_DK:
			cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_DK*1000;
			break;
		case MOD_PAL_I:
			cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_I*1000;
			break;
		case MOD_NTSC_M:
            cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_M*1000;
            break;
        case MOD_SECAM_L:
            cf = pc + C_TDSY_G230D_PF_TO_CF_SHIFT_L*1000;
            break;
        case MOD_SECAM_L1:
            cf = pc - C_TDSY_G230D_PF_TO_CF_SHIFT_L1*1000;
            break;
	}
	return cf;
}
/************************************************************************************************************************
  NAME: TDSY_G230D_LoadFirmware_16
  DESCRIPTON: Load firmware from firmware_struct array in TDSY_G230D_Firmware_x_y_build_z.h file into TDSY_G230D
              Requires TDSY_G230D to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  TDSY_G230D Context (I2C address)
  Parameter:  pointer to firmware_struct array
  Parameter:  number of lines in firmware table array (size in bytes / firmware_struct)
  Returns:    TDSY_G230D/I2C transaction error code, NO_TDSY_G230D_ERROR if successful
************************************************************************************************************************/
int TDSY_G230D_LoadFirmware_16        (L1_TDSY_G230D_Context *api, firmware_struct fw_table[], int nbLines)
{
    int return_code;
    int line;
    return_code = NO_TDSY_G230D_ERROR;

    mcDBG_MSG_L2 (("TDSY_G230D_LoadFirmware_16 starting...\n"));
    mcDBG_MSG_L2 (("TDSY_G230D_LoadFirmware_16 nbLines %d\n", nbLines));

    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++)
    {
		if (fw_table[line].firmware_len > 0)  /* don't download if length is 0 , e.g. dummy firmware */
		{
			/* send firmware_len bytes (up to 16) to TDSY_G230D */
			if ((return_code = TDSY_G230D_L1_API_Patch(api, fw_table[line].firmware_len, fw_table[line].firmware_table)) != NO_TDSY_G230D_ERROR)
			{
			  mcDBG_MSG_ERR(("TDSY_G230D_LoadFirmware_16 error 0x%02x patching line %d: %s\n", return_code, line, TDSY_G230D_L1_API_ERROR_TEXT(return_code) ));
			  if (line == 0) {
			  mcDBG_MSG_L2(("The firmware is incompatible with the part!\n"));
			  }
			  return ERROR_TDSY_G230D_LOADING_FIRMWARE;
			}
			if (line==3) mcDBG_MSG_L2(("traces suspend"));
		}
    }
    mcDBG_MSG_L2(("traces resume"));
    mcDBG_MSG_L2 (("TDSY_G230D_LoadFirmware_16 complete...\n"));
    return NO_TDSY_G230D_ERROR;
}

/************************************************************************************************************************
  NAME: TDSY_G230D_LoadFirmware
  DESCRIPTON: Load firmware from FIRMWARE_TABLE array in TDSY_G230D_Firmware_x_y_build_z.h file into TDSY_G230D
              Requires TDSY_G230D to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  TDSY_G230D Context (I2C address)
  Parameter:  pointer to firmware table array
  Parameter:  number of lines in firmware table array (size in bytes / BYTES_PER_LINE)
  Returns:    TDSY_G230D/I2C transaction error code, NO_TDSY_G230D_ERROR if successful
************************************************************************************************************************/
int TDSY_G230D_LoadFirmware        (L1_TDSY_G230D_Context *api, unsigned char fw_table[], int nbLines)
{
    int return_code;
    int line;
    return_code = NO_TDSY_G230D_ERROR;

   mcDBG_MSG_L2(("TDSY_G230D_LoadFirmware starting...\n"));
    mcDBG_MSG_L2 (("TDSY_G230D_LoadFirmware nbLines %d\n", nbLines));

    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++)
    {
        /* send TDSY_G230D_BYTES_PER_LINE fw bytes to TDSY_G230D */
        if ((return_code = TDSY_G230D_L1_API_Patch(api, TDSY_G230D_BYTES_PER_LINE, fw_table + TDSY_G230D_BYTES_PER_LINE*line)) != NO_TDSY_G230D_ERROR)
        {
          mcDBG_MSG_ERR(("TDSY_G230D_LoadFirmware error 0x%02x patching line %d: %s\n", return_code, line, TDSY_G230D_L1_API_ERROR_TEXT(return_code) ));
          if (line == 0) {
          mcDBG_MSG_L2(("The firmware is incompatible with the part!\n"));
          }
          return ERROR_TDSY_G230D_LOADING_FIRMWARE;
        }
        if (line==3) mcDBG_MSG_L2(("traces suspend"));
    }
    mcDBG_MSG_L2(("traces resume"));
    mcDBG_MSG_L2 (("TDSY_G230D_LoadFirmware complete...\n"));
    return NO_TDSY_G230D_ERROR;
}
//Silabs_120425 added TDSY_G230D_LoadVideofilter() for future use of video filter option.
/************************************************************************************************************************
  NAME: TDSY_G230D_LoadVideofilter
  DESCRIPTION:        Load video filters from vidfiltTable in TDSY_G230D_write_xTV_video_coeffs.h file into TDSY_G230D
  Programming Guide Reference:    Flowchart A.4 (Download Video Filters flowchart)

  Parameter:  TDSY_G230D Context (I2C address)
  Parameter:  pointer to video filter table array
  Parameter:  number of lines in video filter table array (size in bytes / atv_vid_filt_struct)
  Returns:    TDSY_G230D/I2C transaction error code, NO_Si2178_ERROR if successful
************************************************************************************************************************/
int TDSY_G230D_LoadVideofilter     (L1_TDSY_G230D_Context *api, vid_filt_struct vidFiltTable[], int lines)
{
    int line;
     /* for each line in VIDFILT_TABLE (max 16 bytes)  */
    for (line = 0; line < lines ; line++)
    {
        /* send up to 16 byte I2C command to Si2178 */
        if (TDSY_G230D_L1_API_Patch(api, vidFiltTable[line].vid_filt_len, vidFiltTable[line].vid_filt_table) != NO_TDSY_G230D_ERROR)
        {
            return ERROR_TDSY_G230D_SENDING_COMMAND;
        }
    }
    return NO_TDSY_G230D_ERROR;
}
/************************************************************************************************************************
  NAME: configureTDSY_G230D
  DESCRIPTION: Setup TDSY_G230D video filters, GPIOs/clocks, Common Properties startup, Tuner startup, ATV startup, and DTV startup.
  Parameter:  Pointer to TDSY_G230D Context (I2C address)
  Returns:    I2C transaction error code, 0 if successful
************************************************************************************************************************/
int TDSY_G230D_Configure           (L1_TDSY_G230D_Context *api)
{
    int return_code;
    return_code=0;

    /* Set All Properties startup configuration */
    api->prop->tuner_ien.tcien = TDSY_G230D_TUNER_IEN_PROP_TCIEN_ENABLE;
    api->prop->tuner_ien.rssilien = TDSY_G230D_TUNER_IEN_PROP_RSSILIEN_DISABLE;
    api->prop->tuner_ien.rssihien = TDSY_G230D_TUNER_IEN_PROP_RSSIHIEN_DISABLE;
    if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_TUNER_IEN_PROP)) != 0)
    {
       SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
    api->prop->dtv_ien.chlien = TDSY_G230D_DTV_IEN_PROP_CHLIEN_ENABLE;
    if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_DTV_IEN_PROP)) != 0)
    {
       SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
     api->prop->atv_ien.chlien = TDSY_G230D_ATV_IEN_PROP_CHLIEN_ENABLE;
     api->prop->atv_ien.pclien = TDSY_G230D_ATV_IEN_PROP_PCLIEN_DISABLE;
    if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_IEN_PROP)) != 0)
    {
       SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
    api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_8MHZ;
    api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_DVBT;
    api->prop->dtv_mode.invert_spectrum = TDSY_G230D_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL;
    if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_DTV_MODE_PROP)) != 0)
    {
       SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }

    api->prop->atv_video_mode.color= TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
    api->prop->atv_video_mode.invert_spectrum= TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL;
     // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_TERRESTRIAL;
    api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
   if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_VIDEO_MODE_PROP)) != 0)
    {
       SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
       return return_code;
    }
	api->prop->atv_agc_speed.if_agc_speed= TDSY_G230D_ATV_AGC_SPEED_PROP_IF_AGC_SPEED_185; 
	   if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_AGC_SPEED_PROP)) != 0)
		{
		   SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
		   return return_code;
		}
	#if TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL
    // Silabs_120425 was ATV_AGC_3DB	(renamed to AGC2_3DB)
	api->prop->atv_config_if_port.atv_agc_source=TDSY_G230D_ATV_CONFIG_IF_PORT_PROP_ATV_AGC_SOURCE_AGC1_3DB; //change to AGC1 as HW different
	api->prop->atv_ext_agc.max_10mv = 200;
	api->prop->atv_ext_agc.min_10mv = 50;
    if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_EXT_AGC_PROP)) != 0)
		{
		   SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
		   return return_code;
		}	
	
	#else
	api->prop->atv_config_if_port.atv_agc_source = TDSY_G230D_ATV_CONFIG_IF_PORT_PROP_ATV_AGC_SOURCE_INTERNAL;
	#endif
	api->prop->atv_config_if_port.atv_out_type=TDSY_G230D_ATV_CONFIG_IF_PORT_PROP_ATV_OUT_TYPE_LIF_DIFF_IF1;
	   if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_CONFIG_IF_PORT_PROP)) != 0)
		{
		   SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
		   return return_code;
		}
	#if TDSY_G230D_ENABLE_EXTERNAL_DTV_AGC_CONTROL
    // Silabs_120425 was DTV_AGC_3DB	(renamed to AGC2_3DB)
	   api->prop->dtv_config_if_port.dtv_agc_source=TDSY_G230D_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB; //change to AGC1 as HW different
		api->prop->dtv_ext_agc.max_10mv = 200;
	api->prop->dtv_ext_agc.min_10mv = 50;
	 if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_DTV_EXT_AGC_PROP)) != 0)
		{
		   SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
		   return return_code;
		}	
	#else
	api->prop->dtv_config_if_port.dtv_agc_source = TDSY_G230D_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_INTERNAL;
	#endif
	   api->prop->dtv_config_if_port.dtv_out_type=TDSY_G230D_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
		  if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_DTV_CONFIG_IF_PORT_PROP)) != 0)
		   {
			  SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
			  return return_code;
		   }
	//Silabs_120425 added set dtv_pga_limits min=24, max=56 for all DTV standards
	api->prop->dtv_pga_limits.min = 24;
    api->prop->dtv_pga_limits.max = 56;
	if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_PGA_LIMITS_PROP) != 0)
	{
	  SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
	  return return_code;
	}

	//wenming 2012-07-31: FEF config
	if((return_code = TDSY_G230D_L1_CONFIG_PINS (api,             
	   TDSY_G230D_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE,
	   TDSY_G230D_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ,
	   TDSY_G230D_CONFIG_PINS_CMD_GPIO2_MODE_DRIVE_0,
	   TDSY_G230D_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ,
	   TDSY_G230D_CONFIG_PINS_CMD_RESERVED1_RESERVED,
	   TDSY_G230D_CONFIG_PINS_CMD_RESERVED2_RESERVED,
	   TDSY_G230D_CONFIG_PINS_CMD_RESERVED3_RESERVED)) != 0)
	{
		SiTRACE ("TDSY_G230D_L1_CONFIG_PINS error 0x%02x\n", return_code);
		return return_code;
	}
	//end FEF config
	   
    return 0;
}
/************************************************************************************************************************
  NAME: TDSY_G230D_PowerUpWithPatch
  DESCRIPTION: Send TDSY_G230D API PowerUp Command with PowerUp to bootloader,
  Check the Chip rev and part, and ROMID are compared to expected values.
  Load the Firmware Patch then Start the Firmware.
  Programming Guide Reference:    Flowchart A.2 (POWER_UP with patch flowchart)

  Parameter:  pointer to TDSY_G230D Context
  Returns:    TDSY_G230D/I2C transaction error code, NO_TDSY_G230D_ERROR if successful
************************************************************************************************************************/
int TDSY_G230D_PowerUpWithPatch    (L1_TDSY_G230D_Context *api)
{
    int return_code;
    return_code = NO_TDSY_G230D_ERROR;

    /* always wait for CTS prior to POWER_UP command */
    if ((return_code = TDSY_G230D_pollForCTS  (api)) != NO_TDSY_G230D_ERROR) {
        SiTRACE ("TDSY_G230D_pollForCTS error 0x%02x\n", return_code);
        return return_code;
    }

   if ((return_code = TDSY_G230D_L1_POWER_UP (api,
                            TDSY_G230D_POWER_UP_CMD_SUBCODE_CODE,
                            TDSY_G230D_POWER_UP_CMD_CLOCK_MODE_XTAL,
                            TDSY_G230D_POWER_UP_CMD_EN_XOUT_DIS_XOUT,
	   						TDSY_G230D_POWER_UP_CMD_PD_LDO_LDO_POWER_UP,
                            TDSY_G230D_POWER_UP_CMD_RESERVED2_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_RESERVED3_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_RESERVED4_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_RESERVED5_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_RESERVED6_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_RESERVED7_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_RESET_RESET,
                            TDSY_G230D_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ,
                            TDSY_G230D_POWER_UP_CMD_RESERVED8_RESERVED,
                            TDSY_G230D_POWER_UP_CMD_FUNC_BOOTLOADER,
                            TDSY_G230D_POWER_UP_CMD_CTSIEN_DISABLE,
                            TDSY_G230D_POWER_UP_CMD_WAKE_UP_WAKE_UP
                            )) != NO_TDSY_G230D_ERROR)
    {
        SiTRACE ("TDSY_G230D_L1_POWER_UP error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }

    /* Get the Part Info from the chip.   This command is only valid in Bootloader mode */
    if ((return_code = TDSY_G230D_L1_PART_INFO(api)) != NO_TDSY_G230D_ERROR) {
        SiTRACE ("TDSY_G230D_L1_PART_INFO error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
    SiTRACE("chiprev %d\n",        api->rsp->part_info.chiprev);
    SiTRACE("part    Si21%d\n",    api->rsp->part_info.part   );
    SiTRACE("pmajor  %d\n",        api->rsp->part_info.pmajor );
    if (api->rsp->part_info.pmajor >= 0x30) {
    SiTRACE("pmajor '%c'\n",       api->rsp->part_info.pmajor );
    }
    SiTRACE("pminor  %d\n",        api->rsp->part_info.pminor );
    if (api->rsp->part_info.pminor >= 0x30) {
    SiTRACE("pminor '%c'\n",       api->rsp->part_info.pminor );
    }
    SiTRACE("romid %3d/0x%02x\n",  api->rsp->part_info.romid,  api->rsp->part_info.romid );
#ifdef    PART_INTEGRITY_CHECKS
    /* Check the Chip revision, part and ROMID against expected values and report an error if incompatible */
    if (api->rsp->part_info.chiprev != api->chiprev) {
        SiTRACE ("INCOMPATIBLE PART error chiprev %d (expected %d)\n", api->rsp->part_info.chiprev, api->chiprev);
        return_code = ERROR_TDSY_G230D_INCOMPATIBLE_PART;
    }
    /* Part Number is represented as the last 2 digits */
    if (api->rsp->part_info.part != api->part) {
        SiTRACE ("INCOMPATIBLE PART error part   %d (expected %d)\n", api->rsp->part_info.part, api->part);
        return_code = ERROR_TDSY_G230D_INCOMPATIBLE_PART;
    }
    /* Part Major Version in ASCII */
    if (api->rsp->part_info.pmajor != api->partMajorVersion) {
        SiTRACE ("INCOMPATIBLE PART error pmajor %d (expected %d)\n", api->rsp->part_info.pmajor, api->partMajorVersion);
        return_code = ERROR_TDSY_G230D_INCOMPATIBLE_PART;
    }
    /* Part Minor Version in ASCII */
    if (api->rsp->part_info.pminor != api->partMinorVersion) {
        SiTRACE ("INCOMPATIBLE PART error pminor %d (expected %d)\n", api->rsp->part_info.pminor, api->partMinorVersion);
        return_code = ERROR_TDSY_G230D_INCOMPATIBLE_PART;
    }
    /* ROMID in byte representation */
    if (api->rsp->part_info.romid != api->partRomid) {
        SiTRACE ("INCOMPATIBLE PART error romid %3d (expected %2d)\n", api->rsp->part_info.romid, api->partRomid);
        return_code = ERROR_TDSY_G230D_INCOMPATIBLE_PART;
    }
    if (return_code != NO_TDSY_G230D_ERROR) return return_code;
#endif /* PART_INTEGRITY_CHECKS */
      /* Load the Firmware */
	if (api->rsp->part_info.romid == 0x31) {
		/* Load the Firmware */
		//Silabs_120425 changed to version 0Cb5
		if ((return_code = TDSY_G230D_LoadFirmware(api, TDSY_G230D_FW_0_Cb8, FIRMWARE_LINES_0_Cb8)) != NO_TDSY_G230D_ERROR) {
		 SiTRACE ("TDSY_G230D_LoadFirmware error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
		 return return_code;
		}
		SiTRACE("Testlog:Now is loading the A2 firmware!\n");
    }
	 /* Load the Firmware based on ROMID Silabs_120820 */
    else if (api->rsp->part_info.romid == 0x32)
    {
      /* Load the Firmware */
        if ((return_code = TDSY_G230D_LoadFirmware(api, TDSY_G230D_FW_0_Eb15, FIRMWARE_LINES_0_Eb15)) != NO_TDSY_G230D_ERROR) 
		{
         SiTRACE ("TDSY_G230D_LoadFirmware error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
         return return_code;
       }
		SiTRACE("Testlog:Now is loading the A3 firmware!\n");
    }
    else if (api->rsp->part_info.romid == 0x33) /* if TDSY_G230D-A20 part load firmware patch (currently a dummy patch , 20bx) */
    {
        /* This dummy patch (20bx) will skip the loadFirmware function and boot from NVM only.
           When a new patch is available for the TDSY_G230D-A20, include the patch file and replace the firmware array and size in the function below */
        if ((return_code = TDSY_G230D_LoadFirmware_16(api, TDSY_G230D_FW_2_1b8, FIRMWARE_LINES_2_1b8)) != NO_TDSY_G230D_ERROR)
        {
          SiTRACE ("TDSY_G230D_LoadFirmware_16 error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
		SiTRACE("Testlog:Now is loading the A4 patch firmware!\n");
    }
    else
    {
        SiTRACE ("INCOMPATIBLE PART error ROMID 0x%02x (expected 0x%02x)\n", api->rsp->part_info.romid, api->partRomid);
        return ERROR_TDSY_G230D_INCOMPATIBLE_PART;
    }

    /*Start the Firmware */
   if (TDSY_G230D_L1_EXIT_BOOTLOADER(api, TDSY_G230D_EXIT_BOOTLOADER_CMD_FUNC_TUNER, TDSY_G230D_EXIT_BOOTLOADER_CMD_CTSIEN_OFF) != NO_TDSY_G230D_ERROR)
    {
        return ERROR_TDSY_G230D_STARTING_FIRMWARE;
    }	

    return NO_TDSY_G230D_ERROR;
}

/** TDSY_G230D_TunerInit
 *  Tuner initialzation forTDSY_G230D.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */

//-----------------------------------------------------------------------------
VOID TDSY_G230D_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{    ITUNER_CTX_T *psTunerCtx = pTCtx;
    
	//#ifdef DTD_INCLUDE
	SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);
   // #else
	SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
	//#endif
	psTunerCtx->u2LO_Step=1000;  //*1Khz
	
    psTunerCtx->I2cAddress = C_TDSY_G230D_LO_ADDRESS;
    psTunerCtx->u2IF_Freq = C_TDSY_G230D_IF_FREQUENCY;
	psTunerCtx->u2IF_Freq_A = 0; // C_TDSY_G230D_IF_FREQUENCY_ANA;//analog if frequency for ATD
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
    #if TDSY_G230D_ENABLE_EXTERNAL_DTV_AGC_CONTROL
	psTunerCtx->fgRFTuner =FALSE;//Digital tuner
	#else
	psTunerCtx->fgRFTuner =TRUE;//d120210_Haibo:PGA shound be close loop
	#endif
	psTunerCtx->u1EqIndex=0;//EQ index select
	
	pEUCtx->m_SAW_BW  = C_TDSY_G230D_SAW_BW;
    pEUCtx->m_Ana_Top = C_TDSY_G230D_TOP_SET_ANA;	
	//#ifdef DTD_INCLUDE
		pUSCtx->u2LO_Step = 1000; //*1KHZ
		pUSCtx->m_aucPara_SLD[2] = (U8)C_U8_TDSY_G230D_AGC_IF_SLP;
		pUSCtx->m_aucPara_SLD[3] = (U8)C_U8_TDSY_G230D_AGC_IF_INT;
		pUSCtx->m_aucPara_SLD[4] = (U8)C_U8_TDSY_G230D_AGC_IF_MAX;
		pUSCtx->m_aucPara_SLD[5] = (U8)C_U8_TDSY_G230D_AGC_IF_MIN;
	//#else
	
	//Tuner frequency range
		pEUCtx->m_s4FreqBoundUpper = C_TDSY_G230D_FREQ_DBOUND_UPPER;
		pEUCtx->m_s4FreqBoundLower = C_TDSY_G230D_FREQ_DBOUND_LOWER;
		pEUCtx->m_s4AnaFreqBoundUpper = C_TDSY_G230D_FREQ_DBOUND_UPPER_Ana;
		pEUCtx->m_s4AnaFreqBoundLower = C_TDSY_G230D_FREQ_DBOUND_LOWER_Ana;
	// for Signal Level display
		pEUCtx->m_ifAgc_lvl_max = (UINT8)C_U8_TDSY_G230D_AGC_IF_LVL_MAX;
		pEUCtx->m_ifAgc_lvl_min = (UINT8)C_U8_TDSY_G230D_AGC_IF_LVL_MIN;
		
		pEUCtx->m_SigLvTh = 0;
		pEUCtx->m_SigLvScan = 0;
		
		pEUCtx->m_aucPara[0] = C_TDSY_G230D_TOP_SET;
		pEUCtx->m_aucPara[1] = 0;
		
		pEUCtx->m_aucPara[2] = (UINT8)C_U8_TDSY_G230D_AGC_IF_SLP;
		pEUCtx->m_aucPara[3] = (UINT8)C_U8_TDSY_G230D_AGC_IF_INT;
		pEUCtx->m_aucPara[4] = (UINT8)C_U8_TDSY_G230D_AGC_IF_MAX;
		pEUCtx->m_aucPara[5] = (UINT8)C_U8_TDSY_G230D_AGC_IF_MIN;
		
		pEUCtx->m_aucPara[6] = C_TDSY_G230D_AGC_COEF;
		pEUCtx->m_aucPara[7] = C_TDSY_G230D_DEMOD_INPUT_POWER;
		
		pEUCtx->m_aucPara[ 8] = (UINT8)C_U8_TDSY_G230D_MAX_IF_GAIN_SET;
		pEUCtx->m_aucPara[ 9] = (UINT8)C_U8_TDSY_G230D_MIN_IF_GAIN_SET;
		pEUCtx->m_aucPara[10] = (UINT8)C_U8_TDSY_G230D_MAX_RF_GAIN_SET;
		pEUCtx->m_aucPara[11] = (UINT8)C_U8_TDSY_G230D_MIN_RF_GAIN_SET;
		pEUCtx->m_aucPara[12] = (UINT8)C_U8_TDSY_G230D_IF_GAIN_RANGE;
		pEUCtx->m_aucPara[13] = (UINT8)C_U8_TDSY_G230D_RF_GAIN_RANGE;
		pEUCtx->m_aucPara[14] = C_TDSY_G230D_POWER_CALIBRATE;
	//#endif
		
	//IF MAX setting!
		pEUCtx->m_aucPara[21] = 0x7F;
		pTDSY_G230D[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQTDSY_G230DBG;
		pTDSY_G230D[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQTDSY_G230DDK;
		pTDSY_G230D[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQTDSY_G230DI;
		pTDSY_G230D[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQTDSY_G230DL;	
		pTDSY_G230D[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQTDSY_G230DL1;
		pTDSY_G230D[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]    = EQTDSY_G230DM;

		pTDSY_G230DEqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]   = EQTDSY_G230DBG;
		pTDSY_G230DEqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]   = EQTDSY_G230DDK;
		pTDSY_G230DEqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]    = EQTDSY_G230DI;
		pTDSY_G230DEqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQTDSY_G230DL;	
		pTDSY_G230DEqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQTDSY_G230DL1;
		pTDSY_G230DEqWeak[MOD_NTSC_M- MOD_ANA_TYPE_BEGIN]	= EQTDSY_G230DM;
		if(!TunerInited)
	 	{
			mcDBG_MSG_L1(("Init " C_SILABS_TDSY_G230D_STR "(Cust tuner)\n"));
		 	api = &api_context;
			
			/* Software Init */
		 	TDSY_G230D_L1_API_Init(api,C_TDSY_G230D_LO_ADDRESS);
		}	
		
}
/* Setup properties to switch standards. */
static BOOL SetIfDemod(ITUNER_CTX_T *pTCtx,UINT8 subSysIdx,UINT32 Freq,UINT8 fgAutoSearch)
{
    // set a global here to save the video standard,  because it's not always available from the caller.
   	VideoStandard =subSysIdx;
	//Default disable the AGC freeze function
	api->prop->dtv_agc_freeze_input.level=TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
	api->prop->dtv_agc_freeze_input.pin=TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
	 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP) != 0)
	{
		mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP\n"));
		return FALSE;
	}
    switch (subSysIdx)
    {	   
        case MOD_PAL_BG:
			/*wenming 11-12-05:always set sys as G when scan mode to fix DK detected as M/N issue*/
			if(ATV_CONN_MODE_CHAN_SCAN == fgAutoSearch)
			{
				api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
				mcDBG_MSG_L2(("TestLog:Now set as GH sys when autosearch\n"));
			}
			else
			{
				if(Freq<300000)  //pal B system
				{
		            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_B;
					mcDBG_MSG_L2(("TestLog:Now set as B sys\n"));
				}
		        else
				{        //pal G system
		            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_GH;
					mcDBG_MSG_L2(("TestLog:Now set as GH sys\n"));
        		}
			}

			api->prop->atv_video_mode.color = TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_VIDEO_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->atv_lif_freq.offset = C_TDSY_G230D_IF_CENTER_BG;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));	
				return FALSE;
            }   
              api->prop->atv_lif_out.offset = TDSY_G230D_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
            break;
        case MOD_PAL_DK:
            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_DK;
            api->prop->atv_video_mode.color = TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_CABLE;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_VIDEO_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->atv_lif_freq.offset = C_TDSY_G230D_IF_CENTER_DK;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			 api->prop->atv_lif_out.offset = TDSY_G230D_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }

            break;
        case MOD_PAL_I:
            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_I;
            api->prop->atv_video_mode.color = TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
          // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_CABLE;			
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_VIDEO_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->atv_lif_freq.offset = C_TDSY_G230D_IF_CENTER_I;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			 api->prop->atv_lif_out.offset = TDSY_G230D_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }

            break;
        case MOD_NTSC_M:
            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_M;
            api->prop->atv_video_mode.color = TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
            api->prop->atv_video_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_CABLE;   			
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_VIDEO_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->atv_lif_freq.offset = C_TDSY_G230D_IF_CENTER_M;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			 api->prop->atv_lif_out.offset = TDSY_G230D_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
             api->prop->atv_lif_out.amp = NO_L_ANALOG_IF_AMP; // mark to check cli
           
           mcDBG_MSG_L2((" =====NTSC IF AMP=%d \n",api->prop->atv_lif_out.amp));
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
            break;
        case MOD_SECAM_L:
            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_L;
            api->prop->atv_video_mode.color = TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_SECAM;
            api->prop->atv_video_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;
            // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_CABLE;			
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_VIDEO_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->atv_lif_freq.offset = C_TDSY_G230D_IF_CENTER_L;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			 api->prop->atv_lif_out.offset = TDSY_G230D_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = L_ANALOG_IF_AMP;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
            break;
        case MOD_SECAM_L1:
            api->prop->atv_video_mode.video_sys = TDSY_G230D_ATV_VIDEO_MODE_PROP_VIDEO_SYS_LP;
            api->prop->atv_video_mode.color = TDSY_G230D_ATV_VIDEO_MODE_PROP_COLOR_SECAM;
            api->prop->atv_video_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_NORMAL;
            // Silabs_120425 Removed. api->prop->atv_video_mode.trans = TDSY_G230D_ATV_VIDEO_MODE_PROP_TRANS_CABLE;			
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_VIDEO_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_VIDEO_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->atv_lif_freq.offset = C_TDSY_G230D_IF_CENTER_L1;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			 api->prop->atv_lif_out.offset = TDSY_G230D_ATV_LIF_OUT_PROP_OFFSET_DEFAULT;
              api->prop->atv_lif_out.amp = L_ANALOG_IF_AMP;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
            break;
       case MOD_DVBT:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_DVBT;
            if(pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_6M)
				api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_6MHZ;
			else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_7M)
				api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_7MHZ;
			else
                api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_8MHZ;
			
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =DVBT_IF_AMP;
			mcDBG_MSG_L2(("DVBT IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top= TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 was M2DB
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}




            break;
       case MOD_DVBT2:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_DVBT;
           if(pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_6M)
				api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_6MHZ;
			else if (pTCtx->specific_member.eu_ctx.m_SAW_BW == SAW_BW_7M)
				api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_7MHZ;
			else
                api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =DVBT2_IF_AMP;
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top = TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 was M2DB
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}
			 api->prop->dtv_agc_freeze_input.level=TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
			 api->prop->dtv_agc_freeze_input.pin=TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_AGC_FREEZE_INPUT_PROP\n"));
				return FALSE;
			}
            break;
       case MOD_DVBC:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_DVBC;
            api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_8MHZ;
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
           api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =DVBC_IF_AMP;
			mcDBG_MSG_L2(("XXXXXXXXXX DVBC IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
			//api->prop->dtv_lif_out.amp =32;
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top = TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 was M2DB
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}
            break;
      case MOD_ATSC_8VSB:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_ATSC;
            api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =ATSC_8VSB_IF_AMP; 
			mcDBG_MSG_L2(("XXXXXXXXXX 8VSB IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top = TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 added
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}			
            break;
		case MOD_J83B_AUTO_QAM:
		case MOD_J83B_256QAM:
		case MOD_J83B_64QAM:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_QAM_US;
            api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =ATSC_QAM_IF_AMP;
			mcDBG_MSG_L2(("XXXXXXXXXX QAM IF AMP =%d \n",api->prop->dtv_lif_out.amp ));
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top = TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 added
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}						
            break;
        case MOD_DTMB:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_DTMB;
            api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =TDSY_G230D_DTV_LIF_OUT_PROP_AMP_DEFAULT;
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top = TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 added
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}						
            break;
       case MOD_ISDBT:
            api->prop->dtv_mode.modulation = TDSY_G230D_DTV_MODE_PROP_MODULATION_ISDBT;
            api->prop->dtv_mode.bw = TDSY_G230D_DTV_MODE_PROP_BW_BW_6MHZ;
            api->prop->dtv_mode.invert_spectrum = TDSY_G230D_ATV_VIDEO_MODE_PROP_INVERT_SPECTRUM_INVERTED;

            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_MODE_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_MODE_PROP\n"));
				return FALSE;
            }
            api->prop->dtv_lif_freq.offset=C_TDSY_G230D_IF_FREQUENCY;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_FREQ_PROP) != 0)
            {
                mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
            }
			api->prop->dtv_lif_out.offset = TDSY_G230D_DTV_LIF_OUT_PROP_OFFSET_DEFAULT; 
			api->prop->dtv_lif_out.amp =TDSY_G230D_DTV_LIF_OUT_PROP_AMP_DEFAULT;
	        if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				return FALSE;
			}
			api->prop->dtv_rf_top.dtv_rf_top = TDSY_G230D_DTV_RF_TOP_PROP_DTV_RF_TOP_P6DB; //Silabs_120425 added
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_RF_TOP_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_RF_TOP_PROP\n"));
				return FALSE;
			}						
            break;
    }

	return TRUE;
}
//-----------------------------------------------------------------------------
/** TDSY_G230D_TunerSetFreq
 *  Set Tuner PLL forTDSY_G230D/TDSY_G230D to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range
                   -1 fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 TDSY_G230D_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
  UINT32 Freq = param->Freq; // transfer to video freq(KHz) digital center freq ,analog picture freq
  UINT8 Mode = param->Modulation;    //tv system
  UINT8 AutoSearch=param->fgAutoSearch;
  UINT8 timeout =30; 
  UINT32 freqHz =0;
 UINT32 u2TickStart_eu;
  UINT32 u2TickTUNINTStart_eu;
  UINT32 u2TickxTVINTStart_eu;
  UINT32 TUNINT_duration =timeout;
  //UINT32 xTVINT_duration =150;
  UINT32 xTVINT_duration =155;//yuan add 5ms for cover "mcGET_SYS_TICK()" resolution
  TV_MODES tvMode;
 u2TickStart_eu = mcGET_SYS_TICK();
 pTCtx->u4RF_Freq = Freq;

  mcDBG_MSG_L2(("Tuner_Setfreq:" C_SILABS_TDSY_G230D_STR "(Cust tuner)\n"));
 // SPECIFIC_MEMBER_EU_CTX* pEUCtx= &( pTunerCtx->specific_member.eu_ctx);
  mcDBG_MSG_L2 (("TDSY_G230D_TunerSetFreq freq = %d;Mode=%d\n", Freq,Mode ));
#if TDSY_G230D_ENABLE_EXTERNAL_ATV_AGC_CONTROL
 mcDBG_MSG_L2(("TDSY_G230D:Now is external ATV AGC control!!!\n"));
#endif
#if TDSY_G230D_ENABLE_EXTERNAL_DTV_AGC_CONTROL
 mcDBG_MSG_L2(("TDSY_G230D:Now is external DTV AGC control!!!\n"));
#endif
  //load FW when tuner setfreq at the first time
  if(!TunerInited)
  {
	  TunerInited=TRUE;
	  
	  TDSY_G230D_PowerUpWithPatch (api);
	  TDSY_G230D_Configure (api);
   	  
  }

  if(SetIfDemod(pTCtx,Mode,Freq,AutoSearch)==FALSE)
  {
		
	  mcDBG_MSG_ERR(("TDSY_G230D SetIfDemod fail,init tuner\n"));
	  TDSY_G230D_PowerUpWithPatch(api);
	  TDSY_G230D_Configure(api);
	  SetIfDemod(pTCtx,Mode,Freq,AutoSearch);
  }
  
  if(Mode == MOD_PAL_BG) /*wenming 11-12-2: BG PC to Center is by 7M or 8M BW*/  
	{
		if(ATV_CONN_MODE_CHAN_SCAN == AutoSearch)
		{
			pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_BG+C_TDSY_G230D_PF_TO_CF_SHIFT_BG_8M;
			mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER when scan mode\n"));
		}
		else
		{
			if(Freq<300000)  //pal B system
			{
				pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_BG+C_TDSY_G230D_PF_TO_CF_SHIFT_BG_7M;
				mcDBG_MSG_L2(("TestLog:Now set 7M PC-CENTER"));
			}
			else            //pag G system
			{
				pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_BG+C_TDSY_G230D_PF_TO_CF_SHIFT_BG_8M;
				mcDBG_MSG_L2(("TestLog:Now set 8M PC-CENTER"));
			}
		}
	}
  if(Mode == MOD_PAL_DK)
  pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_DK+C_TDSY_G230D_PF_TO_CF_SHIFT_DK;
  if(Mode == MOD_PAL_I)
  pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_I+C_TDSY_G230D_PF_TO_CF_SHIFT_I;
  if(Mode == MOD_NTSC_M)
  pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_M+C_TDSY_G230D_PF_TO_CF_SHIFT_M;
  if(Mode == MOD_SECAM_L)
  pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_L+C_TDSY_G230D_PF_TO_CF_SHIFT_L;
  if(Mode == MOD_SECAM_L1)
  pTCtx->u2IF_Freq_A = C_TDSY_G230D_IF_CENTER_L1-C_TDSY_G230D_PF_TO_CF_SHIFT_L1;

  
  if (Mode < MOD_TYPE_END)
      tvMode = DIGITAL_MODE;
  else
     { tvMode=ANALOG_MODE;
       if(AutoSearch) //wenming 2012-7-19 to do: fine tune will be none-zero value, so AFC range will be 2000, should keep it or not?
           {api->prop->atv_afc_range.range_khz= 1000;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_AFC_RANGE_PROP) != 0)
            {mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_AFC_RANGE_PROP\n"));
            }
			else
				{mcDBG_MSG_L2(("[Silabs]:-->>> TDSY_G230D_L1_SetProperty2 set TDSY_G230D_ATV_AFC_RANGE_PROP OK\n"));}
			}
			else
			{api->prop->atv_afc_range.range_khz= 1000;
            if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_AFC_RANGE_PROP) != 0)
            {mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_AFC_RANGE_PROP\n"));
            }
			else
				{mcDBG_MSG_L2(("[Silabs]:-->>> TDSY_G230D_L1_SetProperty2 set TDSY_G230D_ATV_AFC_RANGE_PROP OK\n"));}
			}
				
  	}
 

/* Use Picture Carrier Frequency if ATV and Center Frequency if DTV */
  if (tvMode==ANALOG_MODE)
    freqHz = PCFreq_To_CenterFreq( Mode,Freq * 1000,AutoSearch);
  else
    freqHz = Freq * 1000;
 

  if (freqHz < TDSY_G230D_TUNER_TUNE_FREQ_CMD_FREQ_MIN || freqHz > TDSY_G230D_TUNER_TUNE_FREQ_CMD_FREQ_MAX)
	{
		mcDBG_MSG_ERR(("[Silabs]:<= Response out of Range\n"));
		return ERROR_TDSY_G230D_PARAMETER_OUT_OF_RANGE; // =1
	}
if (tvMode==ANALOG_MODE)
	{
	 /* set TDSY_G230D_TUNER_LO_INJECTION_PROP property for ATV */
	 api->prop->tuner_lo_injection.band_1	= TDSY_G230D_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_2	= TDSY_G230D_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_3	= TDSY_G230D_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;

	 if ((TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_TUNER_LO_INJECTION_PROP)) != 0)
	    {
			 SiTRACE ("TDSY_G230D_L1_SetProperty2 TDSY_G230D_TUNER_LO_INJECTION_PROP error \n");
		}
	 else
	 	{mcDBG_MSG_L2(("[Silabs]:-->>> TDSY_G230D_L1_SetProperty2 set TDSY_G230D_TUNER_LO_INJECTION_PROP OK"));}
	 
	}
	else
	{
	 /* set TDSY_G230D_TUNER_LO_INJECTION_PROP property for DTV */
     api->prop->tuner_lo_injection.band_1  = TDSY_G230D_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
	 api->prop->tuner_lo_injection.band_2  = TDSY_G230D_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
	 api->prop->tuner_lo_injection.band_3  = TDSY_G230D_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
	 if ((TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_TUNER_LO_INJECTION_PROP)) != 0)
	    {
			 SiTRACE ("TDSY_G230D_L1_SetProperty2 TDSY_G230D_TUNER_LO_INJECTION_PROP error \n");
		}
	}
  
   if (TDSY_G230D_L1_TUNER_TUNE_FREQ (api,(tvMode==ANALOG_MODE) ? TDSY_G230D_TUNER_TUNE_FREQ_CMD_MODE_ATV : TDSY_G230D_TUNER_TUNE_FREQ_CMD_MODE_DTV,  freqHz) != 0)
	{
	    mcDBG_MSG_ERR(("[Silabs]:Error Si2170_L1_TUNER_TUNE_FREQ\n"));
        return -1;
    }
   else
		   {mcDBG_MSG_L2(("[Silabs]:TDSY_G230D_L1_TUNER_TUNE_FREQ OK\n"));}

     /* wait for TUNINT, timeout is 40ms */
	 u2TickTUNINTStart_eu = mcGET_SYS_TICK();
	 while (( mcGET_TICK_PERIOD() * (mcGET_SYS_TICK() - u2TickTUNINTStart_eu)) < (timeout+5) ) //yuan add 5ms for cover "mcGET_SYS_TICK()" resolution
		 {
			 if (( TDSY_G230D_L1_CheckStatus(api)) != 0)
			 {
				 mcDBG_MSG_ERR(("[Silabs]:Error TDSY_G230D_L1_CheckStatus\n"));
				 return ERROR_TDSY_G230D_SENDING_COMMAND;
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
		/* Silabs_121115 if in SCAN mode then issue the ATV_CW_TEST command which will freeze the AFC and pass the IF */
		/* This is not to be used for picture viewing just for channel scan detection */
			if (AutoSearch)
			{
				if (TDSY_G230D_L1_ATV_CW_TEST(api,TDSY_G230D_ATV_CW_TEST_CMD_PC_LOCK_UNLOCK) != NO_TDSY_G230D_ERROR)
				{
					mcDBG_MSG_ERR(("[Silabs]:Error TDSY_G230D_L1_ATV_CW_TEST\n"));
					return -1;
				}
				else
					return NO_TDSY_G230D_ERROR;
			}
			timeout = 130; //Silabs_120803 Modified per AN654
		}
		 else
		{
	        timeout = 10; 
		}
	  u2TickxTVINTStart_eu = mcGET_SYS_TICK();
	 while (( mcGET_TICK_PERIOD() *( mcGET_SYS_TICK() - u2TickxTVINTStart_eu)) < (timeout+5) ) //yuan add 5ms for cover "mcGET_SYS_TICK()" resolution
		 {
			 if (( TDSY_G230D_L1_CheckStatus(api)) != 0)
			 {
				 mcDBG_MSG_ERR(("[Silabs]:Error TDSY_G230D_L1_CheckStatus\n"));
				 return ERROR_TDSY_G230D_SENDING_COMMAND;
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
	 
	mcDBG_MSG_L2(("[Silabs]:TDSY_G230DTunerSetFreq use time = (%3u ms) tun=%3u atv=%3u\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD(),TUNINT_duration,xTVINT_duration));
    return (0);
}


/* Get Status information from part */
static UCHAR GetStatus( L1_TDSY_G230D_Context *api,TUNER_STATUSES status)
{
    int return_code;
    if (status==CHL)
    {
        if ((return_code=TDSY_G230D_L1_ATV_STATUS(api,TDSY_G230D_ATV_STATUS_CMD_INTACK_CLEAR))!=0) {
             SiTRACE ("TDSY_G230D_L1_ATV_STATUS error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
        }
        return api->rsp->atv_status.chl;
    }
    else if (status == DTV_CHL)
    {
        if ((return_code=TDSY_G230D_L1_DTV_STATUS(api,TDSY_G230D_DTV_STATUS_CMD_INTACK_CLEAR))!=0) {
         SiTRACE ("TDSY_G230D_L1_DTV_STATUS error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
        }
        return api->rsp->dtv_status.chl;
    }
    else
        return 0;


}

/* Get RSSI  from part */
static CHAR GetSignalLevel(L1_TDSY_G230D_Context *api)
{
    int return_code;
    if ((return_code=TDSY_G230D_L1_TUNER_STATUS(api,TDSY_G230D_TUNER_STATUS_CMD_INTACK_CLEAR))!=0) {
     SiTRACE ("TDSY_G230D_L1_TUNER_STATUS error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );

    }

    return api->rsp->tuner_status.rssi;

}
//-----------------------------------------------------------------------------
/** TDSY_G230D_SetSawBw
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval Saw bandwidth
 */
//-----------------------------------------------------------------------------
static VOID TDSY_G230D_SetSawBw(ITUNER_CTX_T * pTCtx, UINT8 sawbw)
{
  pTCtx->specific_member.eu_ctx.m_SAW_BW=sawbw;
  mcDBG_MSG_L2(("TDSY_G230D_SetSawBw=%d\n",sawbw));
}

//-----------------------------------------------------------------------------
/** TDSY_G230D_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *TDSY_G230D_TunerGetVer(VOID)
{
    return ((CHAR*)C_SILABS_TDSY_G230D_STR);
}



UINT32 TDSY_G230D_GetSSI(ITUNER_CTX_T* pTunerCtx) 
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
		//mcDBG_MSG_L21(("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
		//printf("DVBT: QAM mode unknown; Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel);
		return u4SSI_Indicator;
	}
	else
	{
		i4Power_ref = Power_Ref[(pTunerCtx->sSSIPara.i2Mod)*DVBT_CR_Mode_Size + pTunerCtx->sSSIPara.i2CR];
	}


    
    /* Get Power Rel Value */
	//i4Power_rec = i4Power_rec - 2;  //adjust bu test result;
	if(pTunerCtx->sSSIPara.i2Bw == SAW_BW_8M)
	{
        mcDBG_MSG_L2(("DVBT:8M TDSY_G230D Real_i4Power_rec = %d\n",i4Power_rec));
        //i4Power_rec = i4Power_rec - Adjust_G230d_i4Power_rec - 4;  
        i4Power_rec = i4Power_rec - Adjust_G230d_i4Power_rec - 3;
    }
    else
    {
        mcDBG_MSG_L2(("DVBT:7M TDSY_G230D Real_i4Power_rec = %d\n",i4Power_rec));
        i4Power_rec = i4Power_rec - Adjust_G230d_i4Power_rec ;
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

UINT32 TDSY_G230D_GetSSI_T2(ITUNER_CTX_T* pTunerCtx) 
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
    mcDBG_MSG_L2(("DVBT2: Mod=%d, Code Rate=%d\n",pTunerCtx->sSSIPara.i2Mod,pTunerCtx->sSSIPara.i2CR));

   	
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
	i4Power_rec = i4Power_rec - 2570;  //adjust bu test result;
    i4Power_rel = i4Power_rec - i4Power_ref;
	mcDBG_MSG_L2(("DVBT2: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
    
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
    mcDBG_MSG_L2(("u4SSI_Indicator = %d",u4SSI_Indicator));
    return u4SSI_Indicator;
}

static INT32 fgParaA_DVBC = -257;  /*wenming 11-10-14:adjust power_rec for dvbc*/
/*wenming 11-10-13:DVBC SSI*/
UINT32 TDSY_G230D_GetSSI_DVBC(ITUNER_CTX_T* pTunerCtx)
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
	i4Power_rec+=fgParaA_DVBC;
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
/** TDSY_G230D_SetSSICondition
 *  Set SSI Condition for TH2603
 *  @param  
 *  @retval void
 */
//-----------------------------------------------------------------------------
void TDSY_G230D_SetSSICondition(ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);
    pTunerCtx->sSSIPara.i2Bw = *((INT16*) pInOutVal+5);
    
}

//-----------------------------------------------------------------------------
/** TDSY_G230D_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 TDSY_G230D_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){

    //L1_TDSY_G230D_Context *api;
    int return_code =0;
	
	//api->prop->dtv_mode.bw = 8;

    switch(eOperation){
    case itSetSSICond:
        TDSY_G230D_SetSSICondition(pTCtx, pInOutVal);
        break;
    case itGetEqScriptNormal: 
        *(UINT8**)pInOutVal = pTDSY_G230D[SetVal];;
        break;
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pTDSY_G230DEqWeak[SetVal];
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
        {CHAR ** RetStr=(CHAR **)pInOutVal;
        *RetStr=TDSY_G230D_TunerGetVer();

    	}
        break;
	case itGetTop:
		
          if (SetVal >= MOD_ANA_TYPE_BEGIN)
             {
              if ((return_code=TDSY_G230D_L1_GetProperty2(api,TDSY_G230D_ATV_RF_TOP_PROP))!=0)
               {
                    SiTRACE ("TDSY_G230D_L1_GetProperty2 error 0x%02x\n", return_code);
                    return ITUNER_NOT_OK;
               }
                *(UINT8 *)pInOutVal= api->prop->atv_rf_top.atv_rf_top;
             }
             else
             {
              if ((return_code=TDSY_G230D_L1_GetProperty2(api,TDSY_G230D_DTV_RF_TOP_PROP))!=0)
               {
                    SiTRACE ("TDSY_G230D_L1_GetProperty2 error 0x%02x\n", return_code);
                    return ITUNER_NOT_OK;
               }
                *(UINT8 *)pInOutVal= api->prop->dtv_rf_top.dtv_rf_top;
             }

     break;
	 case itGetDTVTunerStatus:
		{
		int return_code;
	 	 if ((return_code=TDSY_G230D_L1_DTV_STATUS(api,TDSY_G230D_DTV_STATUS_CMD_INTACK_CLEAR))!=0) 
	 	{
    	 SiTRACE ("TDSY_G230D_L1_DTV_STATUS error 0x%02x: %s\n", return_code, TDSY_G230D_L1_API_ERROR_TEXT(return_code) );
		 return ITUNER_NOT_OK;
    	}
	 	break;
	 	}
     case itSetIfDemod:
        //    SetIfDemod(SetVal);
     break;
    case itSetTop:
     {
          if(VideoStandard >= MOD_ANA_TYPE_BEGIN)
          {
            api->prop->atv_rf_top.atv_rf_top=SetVal;
            if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_RF_TOP_PROP)) != 0)
            {
               SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
               return ITUNER_NOT_OK;
            }
          }
          else
          {
            api->prop->dtv_rf_top.dtv_rf_top=SetVal;
            if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_DTV_RF_TOP_PROP)) != 0)
            {
               SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
               return ITUNER_NOT_OK;
            }

          }

     }
	 break;
	 case itSetSawBw://Set Channel Bandwidth
		{
		TDSY_G230D_SetSawBw(pTCtx,SetVal);
	 	}
	  break;
	  case itGetStatus: // return channel locked status.
          if (SetVal >= MOD_ANA_TYPE_BEGIN)
            *(UINT8 *)pInOutVal = GetStatus( api,CHL);
          else
            *(UINT8 *)pInOutVal = GetStatus( api,DTV_CHL);
	  break;
      case itGetSSIIndex: // Get RSSI
	    { 
            INT32 *RetVal= NULL;

            RetVal = (INT32 *)pInOutVal;
            if (SetVal==0)
            {
    		    *RetVal = (INT32)TDSY_G230D_GetSSI(pTCtx);
            }
            else
            {
    		    *RetVal = (INT32)TDSY_G230D_GetSSI_DVBC(pTCtx);
            }
        }
        //SiTRACE("TDSY_G230D_GetSSI= %d\n",(GetSignalLevel(api)));
        break;
	  case itGetSSIIndex_T2: // Get RSSI
            *(INT32 *)pInOutVal = TDSY_G230D_GetSSI_T2(pTCtx);//(INT16)GetSignalLevel(api);
			//SiTRACE("TDSY_G230D_GetSignalLevel= %d\n",(GetSignalLevel(api)));
	  break;
	  case itGetSignalLevel: // Get RSSI
            *(INT32 *)pInOutVal = (108+(INT8)GetSignalLevel(api));// use dBuV to show signal level
			//SiTRACE("TDSY_G230D_GetSignalLevel= %d\n",(108 +(INT8) GetSignalLevel(api)));
	  break;	
	  case itSetIF: // set ATV/DTV LIF FREQ
          if (VideoStandard >= MOD_ANA_TYPE_BEGIN)
             {
                 api->prop->atv_lif_freq.offset = SetVal;
                if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_ATV_LIF_FREQ_PROP)) != 0)
                {
                   SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
                   return ITUNER_NOT_OK;
                }

             }
             else
            {
                 api->prop->dtv_lif_freq.offset = SetVal;
                if ((return_code=TDSY_G230D_L1_SetProperty2(api,TDSY_G230D_DTV_LIF_FREQ_PROP)) != 0)
                {
                   SiTRACE ("TDSY_G230D_L1_SetProperty2 error 0x%02x\n", return_code);
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
			{DVBT_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =DVBT_IF_AMP;	    
			if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
			}	
		    mcDBG_MSG_L1(("DVBT_IF_AMP set as %d \n",DVBT_IF_AMP));}
	    else if(VideoStandard== MOD_DVBT2)
			{DVBT2_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =DVBT2_IF_AMP;
			if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
			}	
		    mcDBG_MSG_L2(("DVBT2_IF_AMP set as %d \n",DVBT2_IF_AMP));}
		else if(VideoStandard== MOD_DVBC)
			{DVBC_IF_AMP=SetVal;
			api->prop->dtv_lif_out.amp =DVBC_IF_AMP;
			if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
			}	
			mcDBG_MSG_L1(("DVBC_IF_AMP set as %d \n",DVBC_IF_AMP));}
		else if((VideoStandard== MOD_SECAM_L)||(VideoStandard==MOD_SECAM_L1))
			{L_ANALOG_IF_AMP=SetVal;
			api->prop->atv_lif_out.amp =L_ANALOG_IF_AMP;
			if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
			}	
			mcDBG_MSG_L2(("L_ANALOG_IF_AMP set as %d \n",NO_L_ANALOG_IF_AMP));}
		else if((VideoStandard==MOD_J83B_64QAM)||(VideoStandard==MOD_J83B_256QAM)||(VideoStandard==MOD_J83B_AUTO_QAM))
			{ATSC_QAM_IF_AMP=SetVal;
			 api->prop->dtv_lif_out.amp =ATSC_QAM_IF_AMP;
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
				{
					mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				}	
				mcDBG_MSG_L2(("QAM NO_L_DIGITAL_IF_AMP set as %d \n",api->prop->dtv_lif_out.amp));
			}
		else if((VideoStandard==MOD_ATSC_8VSB))
			{NO_L_DIGITAL_IF_AMP=SetVal;
			 api->prop->dtv_lif_out.amp =NO_L_DIGITAL_IF_AMP;
			 if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_DTV_LIF_OUT_PROP) != 0)
				{
					mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_DTV_LIF_FREQ_PROP\n"));
				}	
				mcDBG_MSG_L2(("8VSB NO_L_DIGITAL_IF_AMP set as %d \n",api->prop->dtv_lif_out.amp));
			}		
		else if((VideoStandard>=MOD_ANA_TYPE_BEGIN)||(VideoStandard<MOD_ANA_TYPE_END))
			{NO_L_ANALOG_IF_AMP=SetVal;
			api->prop->atv_lif_out.amp =NO_L_ANALOG_IF_AMP;
			if (TDSY_G230D_L1_SetProperty2(api, TDSY_G230D_ATV_LIF_OUT_PROP) != 0)
			{
				mcDBG_MSG_ERR((" [Silabs]: ERROR_SETTING_PROPERTY TDSY_G230D_ATV_LIF_FREQ_PROP\n"));
			}			
			mcDBG_MSG_L2(("NO_L_ANALOG_IF_AMP set as %d \n",NO_L_ANALOG_IF_AMP));}
		else
			mcDBG_MSG_L2(("TVStandard unknown pls set a channel at first"));
		  	}
		}
        break;
		
	case itGetTunerAFCKhz:
	  	{	  		
	  		 /* unsigned char intack=Si2158_ATV_STATUS_CMD_INTACK_OK;
              S32 TunerAFCKhz = 5000;
            if ((Si2158_L1_ATV_STATUS(api,intack) != 0))
        	{
        	     mcDBG_MSG_ERR(("[Silabs]:Error reading STATUS\n"));
        	}
			else
			{
				//if(api->status->atvint)
				if(api->rsp->atv_status.pcl)  //wenming 11-11-25: for weak signal loss issue, side effect: scan may slow down
                {
                  TunerAFCKhz = (S32)api->rsp->atv_status.afc_freq;
				  mcDBG_MSG_L2(("[Silabs]:Tuner ATV lock and TunerAFCKhz = %d\n",TunerAFCKhz));
				}
				else
			    {
				  mcDBG_MSG_L2(("[Silabs]:Tuner ATV unlock set TunerAFCKhz = %d,to let demod go ,,\n",TunerAFCKhz));
				}
			}*/
			S32 TunerAFCKhz = 0;
			*(S32*)pInOutVal=TunerAFCKhz;

		}
        break;
		#ifdef    TDSY_G230D_GET_PROPERTY_STRING
		case itGetReg:
		{
			UINT8 ucTemp = 0;
			UINT8 ucDumpSettingCount = 0;
			
			SiTRACE ("TDSY_G230D_itGetReg ...........\n");
			
			ucDumpSettingCount = sizeof(aui2TDSY_G230DProperties)/sizeof(unsigned int);
			mcDBG_MSG_L1(("\n"));
			for(ucTemp=0;ucTemp<ucDumpSettingCount;ucTemp++)
			{
				TDSY_G230D_L1_GetPropertyString(api,aui2TDSY_G230DProperties[ucTemp],(char*)"\n",acTDSY_G230DProMsg);
				mcDBG_MSG_L1(("%s\n",acTDSY_G230DProMsg));
			}
		}
	    break;
		#endif

    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}
