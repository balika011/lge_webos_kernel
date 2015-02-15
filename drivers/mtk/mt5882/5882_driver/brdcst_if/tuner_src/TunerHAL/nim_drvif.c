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
 * $RCSfile: nim_drvif.c,v $
 *
 *---------------------------------------------------------------------------*/

/** @file nim_drvif.c
 *  This C file implements the NIM interface functions. It will
 *  call the tuner exported APIs.
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "nim_drvif.h"

#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "drv_common.h"

#include "TDIS.h"
#include "tuner_if.h"

#include "x_typedef.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_pinmux.h"
#include "x_timer.h"
#include "x_tuner.h"
#include "ctrl_bus.h"
/*wenming 12-2-15: add include file for range search command*/
#include "x_rm_dev_types.h"  
#include "queue.h"   
#include "aud_if.h"
#include "drv_tvd.h"  
#include "fe_tuner_common_if.h"
#include "sv_const.h"


#ifdef __MW_CLI_DEF__
#include "x_mid_cli.h"
#else
#include "x_drv_cli.h"
#endif
//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------
#define DEFINE_IS_LOG       DMX_IsLog
#include "x_debug.h"

LINT_EXT_HEADER_END

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------
#define D_NIM_MORNITOR_GPIO         18   // GPIO_18

#define D_NIM_ERR_REPORT_PERIOD      1000

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
typedef struct _RS_CH_FREQ_BW_MAP_T {
    CHAR*                   ps_rf_channel;
    UINT8                   ui1_scan_idx;
    UINT32                  ui4_freq;
    TUNER_BANDWIDTH_T       e_bandwidth;
} RS_CH_FREQ_BW_MAP_T;

RS_CH_FREQ_BW_MAP_T at_twn_ch_map[] = 
{
    {    "2",      2,    57000,     BW_6_MHz},
    {    "3",      3,    63000,     BW_6_MHz},
    {    "4",      4,    69000,     BW_6_MHz},
    {    "5",      5,    79000,     BW_6_MHz},
    {    "6",      6,    85000,     BW_6_MHz},
    {    "7",      7,   177000,     BW_6_MHz},
    {    "8",      8,   183000,     BW_6_MHz},
    {    "9",      9,   189000,     BW_6_MHz},
    {   "10",     10,   195000,     BW_6_MHz},
    {   "11",     11,   201000,     BW_6_MHz},
    {   "12",     12,   207000,     BW_6_MHz},
    {   "13",     13,   213000,     BW_6_MHz},

    {   "14",     14,   473000,     BW_6_MHz},
    {   "15",     15,   479000,     BW_6_MHz},
    {   "16",     16,   485000,     BW_6_MHz},
    {   "17",     17,   491000,     BW_6_MHz},
    {   "18",     18,   497000,     BW_6_MHz},
    {   "19",     19,   503000,     BW_6_MHz},
    {   "20",     20,   509000,     BW_6_MHz},
    {   "21",     21,   515000,     BW_6_MHz},
    {   "22",     22,   521000,     BW_6_MHz},
    {   "23",     23,   527000,     BW_6_MHz},
    {   "24",     24,   533000,     BW_6_MHz},
    {   "25",     25,   539000,     BW_6_MHz},
    {   "26",     26,   545000,     BW_6_MHz},
    {   "27",     27,   551000,     BW_6_MHz},
    {   "28",     28,   557000,     BW_6_MHz},
    {   "29",     29,   563000,     BW_6_MHz},
    {   "30",     30,   569000,     BW_6_MHz},
    {   "31",     31,   575000,     BW_6_MHz},
    {   "32",     32,   581000,     BW_6_MHz},
    {   "33",     33,   587000,     BW_6_MHz},
    {   "34",     34,   593000,     BW_6_MHz},
    {   "35",     35,   599000,     BW_6_MHz},
    {   "36",     36,   605000,     BW_6_MHz},
    {   "37",     37,   611000,     BW_6_MHz},
    {   "38",     38,   617000,     BW_6_MHz},
    {   "39",     39,   623000,     BW_6_MHz},
    {   "40",     40,   629000,     BW_6_MHz},
    {   "41",     41,   635000,     BW_6_MHz},
    {   "42",     42,   641000,     BW_6_MHz},
    {   "43",     43,   647000,     BW_6_MHz},
    {   "44",     44,   653000,     BW_6_MHz},
    {   "45",     45,   659000,     BW_6_MHz},
    {   "46",     46,   665000,     BW_6_MHz},
    {   "47",     47,   671000,     BW_6_MHz},
    {   "48",     48,   677000,     BW_6_MHz},
    {   "49",     49,   683000,     BW_6_MHz},
    {   "50",     50,   689000,     BW_6_MHz},
    {   "51",     51,   695000,     BW_6_MHz},
    {   "52",     52,   701000,     BW_6_MHz},
    {   "53",     53,   707000,     BW_6_MHz},
    {   "54",     54,   713000,     BW_6_MHz},
    {   "55",     55,   719000,     BW_6_MHz},
    {   "56",     56,   725000,     BW_6_MHz},
    {   "57",     57,   731000,     BW_6_MHz},
    {   "58",     58,   737000,     BW_6_MHz},
    {   "59",     59,   743000,     BW_6_MHz},
    {   "60",     60,   749000,     BW_6_MHz},
    {   "61",     61,   755000,     BW_6_MHz},
    {   "62",     62,   761000,     BW_6_MHz},
    {   "63",     63,   767000,     BW_6_MHz},
    {   "64",     64,   773000,     BW_6_MHz},
    {   "65",     65,   779000,     BW_6_MHz},
    {   "66",     66,   785000,     BW_6_MHz},
    {   "67",     67,   791000,     BW_6_MHz},
    {   "68",     68,   797000,     BW_6_MHz},
    {   "69",     69,   803000,     BW_6_MHz},
    {   "70",     70,   809000,     BW_6_MHz},
    {   "71",     71,   815000,     BW_6_MHz},
    {   "72",     72,   821000,     BW_6_MHz},
    {   "73",     73,   827000,     BW_6_MHz},
    {   "74",     74,   833000,     BW_6_MHz},
    {   "75",     75,   839000,     BW_6_MHz},
    {   "76",     76,   845000,     BW_6_MHz},
    {   "77",     77,   851000,     BW_6_MHz},
    {   "78",     78,   857000,     BW_6_MHz},
    {   "79",     79,   863000,     BW_6_MHz},
};

RS_CH_FREQ_BW_MAP_T at_eu_ch_map[] = 
{
    {    "5",     25,   177500,     BW_7_MHz},
    {    "6",     27,   184500,     BW_7_MHz},
    {    "7",     30,   191500,     BW_7_MHz},
    {    "8",     33,   198500,     BW_7_MHz},
    {    "9",     36,   205500,     BW_7_MHz},
    {   "10",     38,   212500,     BW_7_MHz},
    {   "11",     40,   219500,     BW_7_MHz},
    {   "12",     42,   226500,     BW_7_MHz},

    {   "21",    121,   474000,     BW_8_MHz},
    {   "22",    122,   482000,     BW_8_MHz},
    {   "23",    123,   490000,     BW_8_MHz},
    {   "24",    124,   498000,     BW_8_MHz},
    {   "25",    125,   506000,     BW_8_MHz},
    {   "26",    126,   514000,     BW_8_MHz},
    {   "27",    127,   522000,     BW_8_MHz},
    {   "28",    128,   530000,     BW_8_MHz},
    {   "29",    129,   538000,     BW_8_MHz},
    {   "30",    130,   546000,     BW_8_MHz},
    {   "31",    131,   554000,     BW_8_MHz},
    {   "32",    132,   562000,     BW_8_MHz},
    {   "33",    133,   570000,     BW_8_MHz},
    {   "34",    134,   578000,     BW_8_MHz},
    {   "35",    135,   586000,     BW_8_MHz},
    {   "36",    136,   594000,     BW_8_MHz},
    {   "37",    137,   602000,     BW_8_MHz},
    {   "38",    138,   610000,     BW_8_MHz},
    {   "39",    139,   618000,     BW_8_MHz},
    {   "40",    140,   626000,     BW_8_MHz},
    {   "41",    141,   634000,     BW_8_MHz},
    {   "42",    142,   642000,     BW_8_MHz},
    {   "43",    143,   650000,     BW_8_MHz},
    {   "44",    144,   658000,     BW_8_MHz},
    {   "45",    145,   666000,     BW_8_MHz},
    {   "46",    146,   674000,     BW_8_MHz},
    {   "47",    147,   682000,     BW_8_MHz},
    {   "48",    148,   690000,     BW_8_MHz},
    {   "49",    149,   698000,     BW_8_MHz},
    {   "50",    150,   706000,     BW_8_MHz},
    {   "51",    151,   714000,     BW_8_MHz},
    {   "52",    152,   722000,     BW_8_MHz},
    {   "53",    153,   730000,     BW_8_MHz},
    {   "54",    154,   738000,     BW_8_MHz},
    {   "55",    155,   746000,     BW_8_MHz},
    {   "56",    156,   754000,     BW_8_MHz},
    {   "57",    157,   762000,     BW_8_MHz},
    {   "58",    158,   770000,     BW_8_MHz},
    {   "59",    159,   778000,     BW_8_MHz},
    {   "60",    160,   786000,     BW_8_MHz},
    {   "61",    161,   794000,     BW_8_MHz},
    {   "62",    162,   802000,     BW_8_MHz},
    {   "63",    163,   810000,     BW_8_MHz},
    {   "64",    164,   818000,     BW_8_MHz},
    {   "65",    165,   826000,     BW_8_MHz},
    {   "66",    166,   834000,     BW_8_MHz},
    {   "67",    167,   842000,     BW_8_MHz},
    {   "68",    168,   850000,     BW_8_MHz},
    {   "69",    169,   858000,     BW_8_MHz},
};

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------
extern UINT32 os_get_sys_tick(void);
extern void DSP_GetAtvTvSys(MW_TV_AUD_SYS_T * prTvAudSys);
extern AUD_FMT_T _AudAtvFmtDetection(void);
//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
/* global frontend lib variable, set to 1 will dump debug information */

UCHAR u1DbgLevel = 0;
UCHAR u1Rsetting = 0;

#if PFXMSG_SUPPORT
UCHAR u1DbgPfxMsgEnable=(PFXMSG_OFF_BY_DEFAULT?FALSE:TRUE);
#endif
static BOOL _fgNimTaskInited = FALSE;
#ifndef CC_MT5880_CODE_SIZE_CTRL 
static UINT32 _u4NimTsErrCount = 0;
#endif
static UINT16 _u1TunerId = MTK_MT5111_0;
static DRV_CONN_TYPE_T _e_conn_type;
static TUNER_MODULATION_T _Mod;

// Added by rocky_huang 2006/06/19
static TUNER_BANDWIDTH_T _eBandwidth = BW_8_MHz;
TUNER_BANDWIDTH_T _eAdvBandwidth = BW_8_MHz;
BOOL _fgAdvBW = FALSE;
extern UINT8 DynamicSwitch_DTD_DVBT_Mode;

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
LINT_EXT_HEADER_BEGIN
static UINT32 NIM_ParseSysMsk(INT32 i4SubSysIdx)
{
    UINT32 u4SubSysMsk=TV_SYS_MASK_M | TV_SYS_MASK_N;
    
	
    switch(i4SubSysIdx+MOD_ANA_TYPE_BEGIN)
	{
	  case MOD_PAL_BG:
	  u4SubSysMsk=TV_SYS_MASK_B | TV_SYS_MASK_G;
	  break;
	  case MOD_PAL_DK:
	  u4SubSysMsk=TV_SYS_MASK_D | TV_SYS_MASK_K;
	  break;
	  case MOD_PAL_I:
	  u4SubSysMsk=TV_SYS_MASK_I;
	  break;
	  case MOD_SECAM_L:
	  u4SubSysMsk=TV_SYS_MASK_L;
	  break;
	  case MOD_SECAM_L1:
	  u4SubSysMsk=TV_SYS_MASK_L_PRIME;
	  break;
	}
	return u4SubSysMsk;
}
static void _nim_FormatConnectInfo(DRV_CONN_TYPE_T e_conn_type, void *ptConnInfo,
    UINT32 u4FreqK, UINT32 u4SymbolK, INT32 i4QamSize, BOOL fgSpectrumInversion)
{
    switch (e_conn_type)
    {
    case TUNER_CONN_TYPE_TER_DIG:
        {
            TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info;

            pt_tuner_info = (TUNER_TER_DIG_TUNE_INFO_T *)ptConnInfo;
            pt_tuner_info->ui4_freq = u4FreqK * 1000;
            pt_tuner_info->e_mod = _Mod;
            pt_tuner_info->e_dvb_t_t2_mode = TUNER_DVB_AUTO_T_T2;
            break;
        }

    case TUNER_CONN_TYPE_CAB_DIG:
        {
            TUNER_CAB_DIG_TUNE_INFO_T *pt_tuner_info;

            pt_tuner_info = (TUNER_CAB_DIG_TUNE_INFO_T *)ptConnInfo;
            pt_tuner_info->ui4_freq = u4FreqK * 1000;
            pt_tuner_info->ui4_sym_rate = u4SymbolK * 1000;
            pt_tuner_info->e_mod = _Mod;
            break;
        }

    case TUNER_CONN_TYPE_SAT_DIG:
        {
            TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info;

            pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *)ptConnInfo;
            pt_tuner_info->ui4_freq = u4FreqK;//MHz
            pt_tuner_info->ui4_sym_rate = u4SymbolK ;
			if(999==i4QamSize)//blind scan case
			{
			  pt_tuner_info->e_conn_mode=CONN_MODE_SCAN;
			  pt_tuner_info->ui4_start_freq=u4FreqK;//MHz
			  pt_tuner_info->ui4_end_freq=u4SymbolK;//MHz
			}
			else
			{
			   pt_tuner_info->e_conn_mode=CONN_MODE_SURF;
			}
            break;
        }

    case TUNER_CONN_TYPE_TER_ANA:
        {
            TUNER_TER_ANA_TUNE_INFO_T *pt_tuner_info;

            pt_tuner_info = (TUNER_TER_ANA_TUNE_INFO_T *)ptConnInfo;
            
			pt_tuner_info->ui4_freq = u4FreqK * 1000;
			pt_tuner_info->ui4_freq_bound=u4FreqK * 1000;
			pt_tuner_info->ui4_tv_sys_mask=NIM_ParseSysMsk(i4QamSize);
			pt_tuner_info->b_fine_tune=TRUE;
			pt_tuner_info->e_mod = _Mod;
			vSetAutoSearchType(SV_CHANGE_CHANNEL_MODE); //channel change
            break;
        }

    case TUNER_CONN_TYPE_CAB_ANA:
        {
            TUNER_CAB_ANA_TUNE_INFO_T *pt_tuner_info;

            pt_tuner_info = (TUNER_CAB_ANA_TUNE_INFO_T *)ptConnInfo;
            pt_tuner_info->ui4_freq = u4FreqK * 1000;
			pt_tuner_info->ui4_freq_bound=u4FreqK * 1000;
			pt_tuner_info->ui4_tv_sys_mask=NIM_ParseSysMsk(i4QamSize);//QAMsize used for sys sub index
			pt_tuner_info->b_fine_tune=TRUE;
			pt_tuner_info->e_mod = _Mod;
			vSetAutoSearchType(SV_CHANGE_CHANNEL_MODE);//channel change
            break;
        }

    case TUNER_CONN_TYPE_SAT_ANA:
        {
            TUNER_SAT_ANA_TUNE_INFO_T *pt_tuner_info;

            pt_tuner_info = (TUNER_SAT_ANA_TUNE_INFO_T *)ptConnInfo;
            pt_tuner_info->ui4_freq = u4FreqK * 1000;
            break;
        }
    

    default:
        mcSHOW_USER_CRI_MSG(("Invalid CONN_TYPE!\n"));
        return;
    }

    return;
}
LINT_EXT_HEADER_END

#if 0
static void _nim_TsErrGpio(INT32 i4Gpio, BOOL fgStatus)
{
    UNUSED(fgStatus);
    if (i4Gpio == D_NIM_MORNITOR_GPIO)
    {
        _u4NimTsErrCount++;
    }
}
#endif
//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

static UINT32 _u4NimTimeoutMs = 0;
static HAL_TIME_T _rHalTimeStart;

INT32 _nim_SetTimeout2(UINT32 u4TimeoutMs)
{
    HAL_RAW_TIME_T rRawTime;

    HAL_GetRawTime(&rRawTime);
    HAL_RawToTime(&rRawTime, &_rHalTimeStart);
    _u4NimTimeoutMs = u4TimeoutMs;
    mcSHOW_USER_CRI_MSG(("u4TimeoutMs=%d\n",_u4NimTimeoutMs));
    return 0;
}
//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// NIM_SetConnType
//
/** Tuner Primitive API: Set connection type.
 *
 *
 *  @param[in]     e_conn_type   connection type.
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------
void NIM_SetConnType(DRV_CONN_TYPE_T e_conn_type)
{
    _e_conn_type = e_conn_type;
}

DRV_CONN_TYPE_T NIM_GetConnType(void)
{
    return _e_conn_type;
}

//-----------------------------------------------------------------------------
// NIM_SetMod
//
/** Tuner Primitive API: Set modulation type.
 *
 *
 *  @param[in]     Mod   modulation type.
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------
void NIM_SetMod(TUNER_MODULATION_T Mod)
{
    _Mod = Mod;
}

//-----------------------------------------------------------------------------
// NIM_SetTunerID
//
/** Tuner Primitive API: Set Tuner ID.
 *
 *
 *  @param[in]     TunerID   Tuner ID.
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------
void NIM_SetTunerID(UINT16 TunerID)
{
    _u1TunerId = TunerID;
}
//-----------------------------------------------------------------------------
// NIM_SetSltMode
//
/** Tuner Primitive API: Set SLT Mode.
 *
 *
 *  @param[in]     SltMode   DVB/DTD.
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------
void NIM_SetSltMode(UINT8 SltMode)
{
    DynamicSwitch_DTD_DVBT_Mode = SltMode;
}

//-----------------------------------------------------------------------------
// NIM_SetBW
//
/** Tuner Primitive API: Set Bandwidth.
 *
 *
 *  @param[in]     u4Bandwidth   bandwidth.
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------
// Added by rocky_huang 2006/06/19
void NIM_SetBW(UINT32 u4Bandwidth)
{
    _eBandwidth = (TUNER_BANDWIDTH_T)u4Bandwidth;
}
//-----------------------------------------------------------------------------
// NIM_AdvSetBW
//
/** Tuner Primitive API: Set Bandwidth.
 *
 *
 *  @param[in]     u4Bandwidth   bandwidth.
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------

void NIM_AdvSetBW(UINT32 u4Bandwidth)
{
	_fgAdvBW = TRUE;

    _eAdvBandwidth = (TUNER_BANDWIDTH_T)u4Bandwidth;
}


//-----------------------------------------------------------------------------
// NIM_CmdVer
//
/** Tuner Primitive API: Get version.
 *
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------

void NIM_CmdSetConnType(UINT32 e_conn_type)
{
	mcSHOW_USER_CRI_MSG(("e_conn_type = %d\n", e_conn_type));
#ifndef DTD_INCLUDE
	CLISetRemapConnType(e_conn_type);
#endif
}

void NIM_CmdVer(void)
{
    mcSHOW_USER_CRI_MSG(("%s\n", TunerGetVer(_u1TunerId)));
}

//**********Set R-setting Value***********//
void NIM_SetRSetting(UCHAR u1RSv)
{
  u1Rsetting = u1RSv;
  mcSHOW_DRVERR_MSG(("Set R-setting = %d",u1Rsetting));
}

LINT_EXT_HEADER_BEGIN
void NIM_CmdHostCmd(INT32 i4Argc, const CHAR **szArgv)
{
    TunerCommand(_u1TunerId, i4Argc - 1, szArgv + 1);
}

void NIM_CmdDtdCmd(INT32 i4Argc, const CHAR **szArgv)
{
    if(_u1TunerId == MTK_ANALOG_0){
        mcSHOW_USER_CRI_MSG(("TunerId=%d, do \"nim.id 1\" first.\n",_u1TunerId));
        return;
    }
    TunerDtdCommand(_u1TunerId, i4Argc - 1, szArgv + 1);
}

void NIM_CmdAtdCmd(INT32 i4Argc, const CHAR **szArgv)
{
    if(_u1TunerId != MTK_ANALOG_0){
        mcSHOW_USER_CRI_MSG(("TunerId=%d, do \"nim.id 0\" first.\n",_u1TunerId));
        return;
    }
    TunerAtdCommand(_u1TunerId, i4Argc - 1, szArgv + 1);
}
LINT_EXT_HEADER_END

//-----------------------------------------------------------------------------
// NIM_GetTunerID
//
/** Tuner Primitive API: Get Tuner ID.
 *
 *
 *
 *  @return UINT16.\n
 *
 */
//-----------------------------------------------------------------------------
UINT16 NIM_GetTunerID(void)
{
    return (_u1TunerId);
}

UCHAR NIM_GetSR(void)
{
    return u1Rsetting;
}

//-----------------------------------------------------------------------------
// NIM_GetDbgLvl
//
/** Tuner Primitive API: Get debug level.
 *
 *
 *  @return UCHAR.\n
 *
 */
//-----------------------------------------------------------------------------
UCHAR NIM_GetDbgLvl(void)
{
    return u1DbgLevel;
}

//-----------------------------------------------------------------------------
// NIM_SetDbgLvl
//
/** Tuner Primitive API: Set debug level.
 *
 *
 *  @return void.\n
 *
 */
//-----------------------------------------------------------------------------
void NIM_SetDbgLvl(UCHAR dbg_lvl)
{
    u1DbgLevel = dbg_lvl;
}

/*wenming 12-2-15: add functions for range search*/
INT32 RS_TunerBreak(void* pArg)
{
	return FALSE;
}
typedef struct _RS_SIGNAL_ENTITY_T{
	GENERIC_LIST_ENTRY  link;
	ANA_SIGNAL_INFO     anasig_t;
}RS_SIGNAL_ENTITY_T;

typedef struct _RS_DVB_SIGNAL_ENTITY_T{
	GENERIC_LIST_ENTRY  link;
	S32                 s4Freq;
    UINT32              e_bandwidth;
	INT32               i4Mod;
    UINT8               u1Hier;
    UINT16              u2SNR;
    INT32               i4PER;
    UINT8               u1SSI;
    UINT8               u1SQI;
    UINT32              u4SymbolRate;
}RS_DVB_SIGNAL_ENTITY_T;

static HANDLE_T             _hPrintfIconCtrlTimer ;
static GENERIC_LIST_ENTRY rsSignalList;
#if defined(ATD_NTSC443_DETECT)
static UINT32 au4RsScanMode[] = {TV_SYS_MASK_M | TV_SYS_MASK_N,TV_SYS_MASK_B | TV_SYS_MASK_G};
#else
static UINT32 au4RsScanMode[] = {TV_SYS_MASK_B | TV_SYS_MASK_G,TV_SYS_MASK_L};
#endif

static UINT8 u1_PrintfIconIndex = 0;

static UINT8 NIM_ParseSysSubIdx(UINT32  u4TvSubSysMask)
{
    UINT8   u1SubSysIdx = MOD_NTSC_M;
    if (u4TvSubSysMask & (TV_SYS_MASK_B | TV_SYS_MASK_G))
    {
        u1SubSysIdx = MOD_PAL_BG;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_D | TV_SYS_MASK_K))
    {
        u1SubSysIdx = MOD_PAL_DK;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_I))
    {
        u1SubSysIdx = MOD_PAL_I;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_L))
    {
        u1SubSysIdx = MOD_SECAM_L;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_L_PRIME))
    {
        u1SubSysIdx = MOD_SECAM_L1;
    }
    return (u1SubSysIdx-MOD_ANA_TYPE_BEGIN);
}

void NIM_RangeSearchATV(UINT32 u4FreqStartKHz,UINT32 u4FreqEndKHz)
{
	UINT32 u4InFreqKHz,u4BdFreqKHz;  //Local freq para
	BOOL fgRsVifLock = FALSE;  //TunerAcq result flag
	BOOL fgRsTvdLock = FALSE;  //TVD Lock flag
	RS_SIGNAL_ENTITY_T * rsSignalEntity = NULL; //To get entity from queue
	MW_TV_AUD_SYS_T rsTvAudSys; //Used when get tv system
	UINT32 u4NowTryTvMask;
	UINT32 u4DetectedSys;
	//Used when call TunerAcq
	DRV_COMP_ID_T rsTunerCompId;
	DRV_CONN_TYPE_T rsConnType = TUNER_CONN_TYPE_TER_ANA;
	TUNER_ALL_INFO_T rsConnInfo;
	RS_SIGNAL_ENTITY_T * ptRsSignal=NULL;
	//Used when get next stepsize
	TUNER_STEP_SIZE_T rsNextStepSize;
	SIZE_T rsStepLen;
	//For timming
    UINT32 u4TickRs = 0;
	//for loop counter
	INT32 i4_i = 0;
	//signal status
	UINT16 u2SigSts=0;
	//for get carrier freq
	SIGNAL rsCarrierFreq;
	//for M/N detected
	BOOL isSysMN = FALSE;
	//for burst freq detected
//	UINT8 ucBurstFreq = TVD_BLOCK_IS358;
    UINT8 u1TrySysIndex=0;    
	UINT8 u1ScanMapLen=sizeof(au4RsScanMode)/sizeof(au4RsScanMode[0]);
	GENERIC_LIST_ENTRY list_backup;
	
	QueueInitList(&rsSignalList);
	
	rsTunerCompId.ui2_id = _u1TunerId;
	rsTunerCompId.e_type = DRVT_TUNER_TER_ANA;
	
	rsConnInfo.t_tana_tuner_info.i2_fine_tune_offset = 0;
	rsConnInfo.t_tana_tuner_info.e_mod = MOD_UNKNOWN;
	rsConnInfo.t_tana_tuner_info.b_fine_tune = FALSE;
	rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
	
	u4TickRs =  os_get_sys_tick();
	
	u4InFreqKHz = u4FreqStartKHz;
	u4BdFreqKHz = u4FreqStartKHz;
	while(u4InFreqKHz <=u4FreqEndKHz)
	{	

		u4NowTryTvMask = rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask;
		
		if(u1TrySysIndex==u1ScanMapLen)//if scan map is eof
		{
		  	TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_NEXT_STEP_SIZE,(void *)&rsNextStepSize,&rsStepLen); //get nextstepsize
			u1TrySysIndex=0;
			rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
			rsConnInfo.t_tana_tuner_info.b_fine_tune = TRUE;
			u4InFreqKHz = rsCarrierFreq.Frequency/1000 + rsNextStepSize.i4_step_size/1000;
			mcSHOW_DBG_MSG(("no more try.Next step jump %dKHz!\n",rsNextStepSize.i4_step_size/1000));
			if(u4InFreqKHz>u4FreqEndKHz)break;	
			if(u4InFreqKHz >u4BdFreqKHz)
			{
			 u4BdFreqKHz=(u4BdFreqKHz +8210)>u4FreqEndKHz?u4FreqEndKHz:(u4BdFreqKHz +8210);
			}
		}
        i4_i++;		
	    mcSHOW_USER_MSG(("Do tunerAcq %d:(Freq %dKHz~%dKHz)\n", i4_i,u4InFreqKHz,u4BdFreqKHz));
		rsConnInfo.t_tana_tuner_info.ui4_freq = u4InFreqKHz * 1000;       //KHz to Hz
		rsConnInfo.t_tana_tuner_info.ui4_freq_bound = u4BdFreqKHz * 1000;
		

		
		/*when first try fine_tune=false,else fine_tune=true
		if(u4NowTryTvMask != au4RsScanMode[0])
		{
			rsConnInfo.t_tana_tuner_info.b_fine_tune = FALSE;
		}
		else
		{
			rsConnInfo.t_tana_tuner_info.b_fine_tune = TRUE;
		}*/
		
		vSetAutoSearchType(SV_SCAN_MODE);    //set scan mode
		fgRsVifLock= TunerAcq(_u1TunerId,rsConnType,(void *)&rsConnInfo,
			      sizeof(TUNER_ALL_INFO_T),RS_TunerBreak,&rsTunerCompId);
		if(!fgRsVifLock)
		{
		    u1TrySysIndex=0;
			rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
			rsConnInfo.t_tana_tuner_info.b_fine_tune = TRUE;
			u4InFreqKHz = u4BdFreqKHz + 50;
			u4BdFreqKHz=(u4BdFreqKHz +8210)>u4FreqEndKHz?u4FreqEndKHz:(u4BdFreqKHz +8210);
			mcSHOW_DBG_MSG(("VIF unlock!\n"));
		}
		else
		{
#if 0		
		    fgRsTvdLock = fgDrvTvdCheckTVDLock(0x15);
#endif		
            //d120320_Haibo:Get TVD lock status
            fgRsTvdLock=FALSE;//d120320_Haibo:No need to recheck TVD
            TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_SIGNAL_STATUS,&u2SigSts,&rsStepLen); //get signal status
			if(u2SigSts&0xff)
			{
			  fgRsTvdLock=TRUE;//d120320_Haibo:LSB is TVD lock status
			}

			
			mcSHOW_DBG_MSG(("TVD lock status=%d\n",fgRsTvdLock));
			
			TunerGetSignal(_u1TunerId,rsConnType,&rsCarrierFreq);
			//VIF Lock&Tvd unlock
			if(!fgRsTvdLock)
			{
				mcSHOW_DBG_MSG(("VIF lock,but TVD unlock!\n"));
				
				//if now first try,retry next scanmode,fine_tune=false
				//if(u4NowTryTvMask== au4RsScanMode[0])
				{
				    u1TrySysIndex++;
					if(u1TrySysIndex<u1ScanMapLen)
					{
					rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
					}
					rsConnInfo.t_tana_tuner_info.b_fine_tune = FALSE;
					u4InFreqKHz = rsCarrierFreq.Frequency/1000; 
				}
				#if 0
				else
				{
					TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_NEXT_STEP_SIZE,(void *)&rsNextStepSize,&rsStepLen); //get nextstepsize
					u1TrySysIndex++;
					rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
					rsConnInfo.t_tana_tuner_info.b_fine_tune = TRUE;
					u4InFreqKHz = rsCarrierFreq.Frequency/1000 + rsNextStepSize.i4_step_size/1000;
					mcSHOW_DBG_MSG(("Next step jump %dKHz!\n",rsNextStepSize.i4_step_size/1000));
				}
				#endif
			}
		    //VIF Lock&Tvd lock
			else
			{
				ptRsSignal = (RS_SIGNAL_ENTITY_T *)x_mem_calloc(1,sizeof(RS_SIGNAL_ENTITY_T));
				if(NULL==ptRsSignal)
				{
				  continue;
				}
				ptRsSignal->anasig_t.s4Freq = rsCarrierFreq.Frequency;
				/*Tv System detect*/
				//d120320_Haibo:Check current sub system 
				/*Detect M/N by colorburst when first try*/
				isSysMN = FALSE;
				if(u4NowTryTvMask== (TV_SYS_MASK_M | TV_SYS_MASK_N))
				{
				#if 0
					mcDELAY_MS(50);
            		ucBurstFreq = bDrvTvdCheckBurstFreq();
					if(ucBurstFreq == TVD_BLOCK_IS358)
					{
						ptRsSignal->u4TvSysMask = TV_SYS_MASK_M | TV_SYS_MASK_N;
						isSysMN = TRUE;
					}
				#endif	
                    if(MOD_NTSC_M==((u2SigSts>>8)&0xff))
					   {
					      ptRsSignal->anasig_t.u1SubSysIdx=MOD_NTSC_M;
					      isSysMN=TRUE;//d120320_Haibo:if now is try M/N,and driver does not try other sub system,should be M
					   }
				}
				if(!isSysMN)
				{
					_AudAtvFmtDetection();
					DSP_GetAtvTvSys(&rsTvAudSys);
					u4DetectedSys=(rsTvAudSys.u4TvSysMask&0xFFFF);//only take lower 16bits
					ptRsSignal->anasig_t.u1SubSysIdx= NIM_ParseSysSubIdx(u4DetectedSys); 
					//if now first try,retry next scanmode,fine_tune=false
					if(u4NowTryTvMask != u4DetectedSys)
					{
					  mcSHOW_DBG_MSG(("Retry next sub system\n"));
					}
					
					if(u4NowTryTvMask == (TV_SYS_MASK_M | TV_SYS_MASK_N))//d120315_Haibo:if now is try M/N,but not M/N,then try next sub system
					{
					    u1TrySysIndex++;
						if(u1TrySysIndex<u1ScanMapLen)
						{
						  rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
						}
						rsConnInfo.t_tana_tuner_info.b_fine_tune = FALSE;
						u4InFreqKHz = rsCarrierFreq.Frequency/1000;
						continue;
					}
					//if now try tvsys != detect tvsys,then retry detect tvsys
					if(u4NowTryTvMask != u4DetectedSys)
					{
						rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = u4DetectedSys;
						rsConnInfo.t_tana_tuner_info.b_fine_tune = FALSE;
						u4InFreqKHz = rsCarrierFreq.Frequency/1000;
						continue;
					}
				}
				QueuePutTail(&rsSignalList, &ptRsSignal->link);
				mcSHOW_DBG_MSG(("Channel Found:%dKHz\n",rsCarrierFreq.Frequency/1000));

				
				//nextstepsize jump
				TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_NEXT_STEP_SIZE,(void *)&rsNextStepSize,&rsStepLen);
				u1TrySysIndex=0;
				rsConnInfo.t_tana_tuner_info.ui4_tv_sys_mask = au4RsScanMode[u1TrySysIndex];
				rsConnInfo.t_tana_tuner_info.b_fine_tune = TRUE;
				u4InFreqKHz = rsCarrierFreq.Frequency/1000 + rsNextStepSize.i4_step_size/1000;
				mcSHOW_DBG_MSG(("Next step jump %dKHz!\n",rsNextStepSize.i4_step_size/1000));
				//delay 2000ms to show
				#if 0
				x_thread_delay(2000);
				#endif
			}
			if(u4InFreqKHz >u4BdFreqKHz)
			{
			u4BdFreqKHz=(u4BdFreqKHz +8210)>u4FreqEndKHz?u4FreqEndKHz:(u4BdFreqKHz +8210);
			}
		}
	}
	u4TickRs = os_get_sys_tick() - u4TickRs;
	mcSHOW_USER_MSG((" NIM_RangeSearch():End RangeSearch(), Search time = %d ms\n", (UINT32)(u4TickRs*x_os_drv_get_tick_period())));
    if(rsSignalList.Count == 0)
    {
    	mcSHOW_USER_MSG(("No channel found between %dKHz and %dKHz\n",u4FreqStartKHz,u4FreqEndKHz));
		return;
    }
	mcSHOW_USER_MSG(("%d channel found between %dKHz and %dKHz\n",rsSignalList.Count,u4FreqStartKHz,u4FreqEndKHz));
	rsSignalEntity = (RS_SIGNAL_ENTITY_T *)QueueGetHead(&rsSignalList);
	while(rsSignalEntity)
	{
		mcSHOW_USER_MSG(("Channel %d:Freq=%dKHz,TvSubSysIdx=%d\n",i4_i+1,
		rsSignalEntity->anasig_t.s4Freq/1000,rsSignalEntity->anasig_t.u1SubSysIdx));
		x_memcpy(&list_backup,&rsSignalEntity->link,sizeof(GENERIC_LIST_ENTRY));
		x_mem_free(rsSignalEntity);	//destory the memory allocated
		rsSignalEntity = (RS_SIGNAL_ENTITY_T *)QueueGetNext(&rsSignalList,&list_backup);
	}
	mcSHOW_USER_MSG(("Rangesearch Log End---------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n\n"));

}

BOOL NIM_RangeSearchFreqMap(const CHAR **szArgv, UINT32 *pu4FreqStartKHz,UINT32 *pu4FreqEndKHz, RS_CH_FREQ_BW_MAP_T **p_chFreqBwMap)
{
    UINT8   u1ChIndex, u1ChMaxCount;
    CHAR*   pszCmdStr;
    UINT32  u4StrLen, u4CmdLen;

    u1ChMaxCount = 0;
    //Check Country Code
    pszCmdStr = "EU";   
    u4StrLen = x_strlen(pszCmdStr);
    u4CmdLen = x_strlen(szArgv[4]);
    if((u4StrLen == u4CmdLen)&&(x_strncmp(pszCmdStr, szArgv[4], u4StrLen) == 0))
    {
        *p_chFreqBwMap = at_eu_ch_map;
        u1ChMaxCount = sizeof(at_eu_ch_map)/sizeof(RS_CH_FREQ_BW_MAP_T);
    }

    pszCmdStr = "TWN";
    u4StrLen = x_strlen(pszCmdStr);
    u4CmdLen = x_strlen(szArgv[4]);
    if((u4StrLen == u4CmdLen)&&(x_strncmp(pszCmdStr, szArgv[4], u4StrLen) == 0))
    {
        *p_chFreqBwMap = at_twn_ch_map;        
        u1ChMaxCount = sizeof(at_twn_ch_map)/sizeof(RS_CH_FREQ_BW_MAP_T);
    } 

    //Map Start Freq to Scan Map
    for(u1ChIndex = 0; u1ChIndex < u1ChMaxCount; u1ChIndex++)
    {
        if((*pu4FreqStartKHz < (*p_chFreqBwMap)[u1ChIndex].ui4_freq)||(*pu4FreqStartKHz == (*p_chFreqBwMap)[u1ChIndex].ui4_freq))
        {
            *pu4FreqStartKHz = (UINT32)u1ChIndex;
            break;
        }        
    }
    if(u1ChIndex == u1ChMaxCount)
    {
        return FALSE;
    }

    //Map End Freq to Scan Map
    for( ; u1ChIndex < u1ChMaxCount; u1ChIndex++)
    {
        if(*pu4FreqEndKHz < (*p_chFreqBwMap)[u1ChIndex].ui4_freq)
        {
            *pu4FreqEndKHz = (UINT32)(u1ChIndex - 1);
            break;
        }
        else if(*pu4FreqEndKHz == (*p_chFreqBwMap)[u1ChIndex].ui4_freq)
        {
            *pu4FreqEndKHz = (UINT32)u1ChIndex;
            break;
        }
        else if(u1ChIndex == (u1ChMaxCount - 1))
        {
            *pu4FreqEndKHz = (UINT32)u1ChIndex;
            break;
        }
    }

    if((u1ChIndex == u1ChMaxCount)||(*pu4FreqStartKHz > *pu4FreqEndKHz))
    {
        return FALSE;
    }
    return TRUE;
    
}

void NIM_RangeSearchDVBTStoreCH(void)
{
    //for get carrier freq
	SIGNAL              rsCarrierFreq;
    DRV_CONN_TYPE_T     rsConnType = TUNER_CONN_TYPE_TER_DIG;
    INT32               i4SQI;
    SIZE_T              ewsInfoLen;
    RS_DVB_SIGNAL_ENTITY_T * ptRsSignal = (RS_DVB_SIGNAL_ENTITY_T *)x_mem_calloc(1,sizeof(RS_DVB_SIGNAL_ENTITY_T));

    if(NULL == ptRsSignal)
    {
        return;
    }
    
    TunerGetSignal(_u1TunerId,rsConnType,&rsCarrierFreq);

    TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_SQI,(void *)&i4SQI,(SIZE_T*)&ewsInfoLen);
    
    ptRsSignal->s4Freq      = rsCarrierFreq.Frequency;
    ptRsSignal->e_bandwidth = rsCarrierFreq.e_bandwidth;
    ptRsSignal->u1Hier      = rsCarrierFreq.e_hierarchy_priority;
    ptRsSignal->u2SNR       = TunerGetSignalSNR(_u1TunerId,rsConnType);
    ptRsSignal->i4PER       = TunerGetSignalPER(_u1TunerId,rsConnType);
    ptRsSignal->u1SSI       = TunerGetSignalLevel(_u1TunerId,rsConnType);
    ptRsSignal->u1SQI       = (UINT8)i4SQI;
    QueuePutTail(&rsSignalList, &ptRsSignal->link);    
}

void NIM_RangeSearchDVBTPrintf(UINT32 u4FreqStartKHz, UINT32 u4FreqEndKHz)
{
    UINT16                  ui2_i;
    RS_DVB_SIGNAL_ENTITY_T  * rsSignalEntity = NULL; //To get entity from queue
    GENERIC_LIST_ENTRY      list_backup;
    
    if(rsSignalList.Count == 0)
    {
        mcSHOW_USER_CRI_MSG(("\rNo channel found between %dKHz and %dKHz\n",u4FreqStartKHz,u4FreqEndKHz));
        return;
    }
    
    mcSHOW_USER_CRI_MSG(("\r\n===================Range Search DVBT=====================\n"));
    mcSHOW_USER_CRI_MSG(("%d channel found between %dKHz and %dKHz\n",rsSignalList.Count,u4FreqStartKHz,u4FreqEndKHz));
    ui2_i = 1;
    rsSignalEntity = (RS_DVB_SIGNAL_ENTITY_T *)QueueGetHead(&rsSignalList);
	while(rsSignalEntity)
	{
        //Freq
        mcSHOW_USER_CRI_MSG(("Channel %3d: [Freq %6dkHz], ",
              ui2_i, rsSignalEntity->s4Freq));
        ui2_i++;
        
        //Band width
        if(rsSignalEntity->e_bandwidth == BW_6_MHz)
        {
            mcSHOW_USER_CRI_MSG(("[BW_6_MHz], "));
        }
        else if(rsSignalEntity->e_bandwidth == BW_7_MHz)
        {
            mcSHOW_USER_CRI_MSG(("[BW_7_MHz], "));
        }
        else if(rsSignalEntity->e_bandwidth == BW_8_MHz)
        {
            mcSHOW_USER_CRI_MSG(("[BW_8_MHz], "));
        }

        //u1Hier
        switch (rsSignalEntity->u1Hier)
        {
            case 0: mcSHOW_USER_CRI_MSG(("[Hier=UNKNOWN], ")); break;
            case 1: mcSHOW_USER_CRI_MSG(("[Hier=None], "));    break;
            case 2: mcSHOW_USER_CRI_MSG(("[Hier=High], "));    break;
            case 3: mcSHOW_USER_CRI_MSG(("[Hier=Low], "));     break;
            default: mcSHOW_USER_CRI_MSG(("[Hier=UNKNOWN], "));break;
        }

        mcSHOW_USER_CRI_MSG(("[SNR=%3d/10dB], [PER=%3d*10^-5], [SSI=%3d], [SQI=%3d]",
              rsSignalEntity->u2SNR, rsSignalEntity->i4PER,
              rsSignalEntity->u1SSI, rsSignalEntity->u1SQI));

        mcSHOW_USER_CRI_MSG(("\n"));

        x_memcpy(&list_backup,&rsSignalEntity->link,sizeof(GENERIC_LIST_ENTRY));
		x_mem_free(rsSignalEntity);	//destory the memory allocated
		rsSignalEntity = (RS_DVB_SIGNAL_ENTITY_T *)QueueGetNext(&rsSignalList,&list_backup);
	}
}

void NIM_RangeSearchPrintfCtrlFunc(HANDLE_T hHandle, VOID* pTag)
{
    UNUSED(hHandle);
    
    if(u1_PrintfIconIndex == 0)
    {
        mcSHOW_USER_CRI_MSG((".\\\r"));
        u1_PrintfIconIndex++;
    }
    else if(u1_PrintfIconIndex == 1)
    {
        mcSHOW_USER_CRI_MSG((".|\r"));
        u1_PrintfIconIndex++;
    }
    else if(u1_PrintfIconIndex == 2)
    {
        mcSHOW_USER_CRI_MSG(("./\r"));
        u1_PrintfIconIndex++;
    }
    else if(u1_PrintfIconIndex == 3)
    {
        mcSHOW_USER_CRI_MSG((".-\r"));
        u1_PrintfIconIndex = 0;
    }
}

void NIM_RangeSearchPrintfTimerInit(void)
{
    x_timer_create(&_hPrintfIconCtrlTimer);
    UNUSED(x_timer_start(_hPrintfIconCtrlTimer, 300,
                                     X_TIMER_FLAG_REPEAT, NIM_RangeSearchPrintfCtrlFunc,
                                     NULL));
}

void NIM_RangeSearchPrintfTimerDelete(void)
{
    if ((HANDLE_T *)_hPrintfIconCtrlTimer != NULL)
    {
        x_timer_stop(_hPrintfIconCtrlTimer);
        x_timer_delete(_hPrintfIconCtrlTimer);
    }
}

void NIM_RangeSearchDVBT(UINT32 u4FreqStart,UINT32 u4FreqEnd, const CHAR **szArgv)
{
    UINT32                  u4FreqStartKHz, u4FreqEndKHz;
    UINT8                   u1FreqStartIndex, u1FreqEndIndex, u1FreqCurIndex;
    
    RS_CH_FREQ_BW_MAP_T     *pchFreqBwMap;
    BOOL                    fgRetSts = FALSE;   
    
    SIGNAL                  rsCarrierFreq;
    //Used when call TunerAcq
	DRV_COMP_ID_T           rsTunerCompId;
	DRV_CONN_TYPE_T         rsConnType = TUNER_CONN_TYPE_TER_ANA;
	TUNER_ALL_INFO_T        rsConnInfo;
    
    //Map CH
    u4FreqStartKHz = u4FreqStart;
    u4FreqEndKHz   = u4FreqEnd;
    if(NIM_RangeSearchFreqMap(szArgv, &u4FreqStart, &u4FreqEnd, &pchFreqBwMap) == FALSE)
    {
        mcSHOW_USER_CRI_MSG(("Freq Range Error, Stop Search!\n")); 
        return;
    }

    u1FreqStartIndex = (UINT8)u4FreqStart;
    u1FreqEndIndex   = (UINT8)u4FreqEnd;

    //Set Conn Type
    NIM_SetConnType(TUNER_CONN_TYPE_TER_DIG);

    TunerDetachMW(_u1TunerId, TRUE);
    
    //Init for TunerAcq
    QueueInitList(&rsSignalList);
	rsTunerCompId.ui2_id = _u1TunerId;
	rsTunerCompId.e_type = DRVT_TUNER_TER_DIG;    
    u1FreqCurIndex = u1FreqStartIndex;

    //Printf Icon
    NIM_RangeSearchPrintfTimerInit();
    
    //Start Scan
    while(1)
    {
        if(u1FreqCurIndex > u1FreqEndIndex)
        {
            break;
        }
        
        rsConnInfo.t_ter_tuner_info.e_mod = MOD_UNKNOWN;
        rsConnInfo.t_ter_tuner_info.ui4_freq = (pchFreqBwMap[u1FreqCurIndex].ui4_freq) * 1000;
        rsConnInfo.t_ter_tuner_info.e_bandwidth = pchFreqBwMap[u1FreqCurIndex].e_bandwidth;
        rsConnInfo.t_ter_tuner_info.e_hierarchy_priority = HIR_PRIORITY_NONE;

        fgRetSts = TunerAcq(_u1TunerId,rsConnType,(void *)&rsConnInfo,
			      sizeof(TUNER_ALL_INFO_T),RS_TunerBreak,&rsTunerCompId);
        
        if(fgRetSts == TRUE)
        {
            NIM_RangeSearchDVBTStoreCH(); 
            TunerGetSignal(_u1TunerId,rsConnType,&rsCarrierFreq);

            if(rsCarrierFreq.e_hierarchy_priority > HIR_PRIORITY_NONE)
            {
                rsConnInfo.t_ter_tuner_info.e_hierarchy_priority = HIR_PRIORITY_LOW;

                fgRetSts = TunerAcq(_u1TunerId,rsConnType,(void *)&rsConnInfo,
			            sizeof(TUNER_ALL_INFO_T),RS_TunerBreak,&rsTunerCompId);

                if(fgRetSts == TRUE)
                {
                    NIM_RangeSearchDVBTStoreCH(); 
                }
            }
        }

        //Search Next channel
        u1FreqCurIndex ++;
    }

    NIM_RangeSearchPrintfTimerDelete();

    //Printf Icon
    NIM_RangeSearchDVBTPrintf(u4FreqStartKHz, u4FreqEndKHz);
    
    TunerDetachMW(_u1TunerId, FALSE);
}

BOOL NIM_RangeSearchDVBCCheck(UINT32 *pu4FreqStartKHz,UINT32 *pu4FreqEndKHz)
{
    if(*pu4FreqStartKHz > *pu4FreqEndKHz)
    {
        return FALSE;  
    }
    else
    {
        if(*pu4FreqEndKHz < 51000)
        {
            return FALSE;
        }
        else
        {
            if(*pu4FreqStartKHz < 51000)
            {
                *pu4FreqStartKHz = 51000;
            }
            if(*pu4FreqEndKHz > 858000)
            {
                *pu4FreqEndKHz = 858000;
            }
        }
    }
    return TRUE;
}

void NIM_RangeSearchDVBCStoreCH(void)
{
    //for get carrier freq
	SIGNAL              rsCarrierFreq;
    DRV_CONN_TYPE_T     rsConnType = TUNER_CONN_TYPE_CAB_DIG;
    INT32               i4SQI;
    SIZE_T              ewsInfoLen;
    RS_DVB_SIGNAL_ENTITY_T * ptRsSignal = (RS_DVB_SIGNAL_ENTITY_T *)x_mem_calloc(1,sizeof(RS_DVB_SIGNAL_ENTITY_T));

    if(NULL == ptRsSignal)
    {
        return;
    }
    
    TunerGetSignal(_u1TunerId,rsConnType,&rsCarrierFreq);

    TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_SQI,(void *)&i4SQI,(SIZE_T*)&ewsInfoLen);
    
    ptRsSignal->s4Freq          = rsCarrierFreq.Frequency;
    ptRsSignal->i4Mod           = rsCarrierFreq.QAMSize;
    ptRsSignal->u4SymbolRate    = rsCarrierFreq.SymbolRate;
    ptRsSignal->u2SNR           = TunerGetSignalSNR(_u1TunerId,rsConnType);
    ptRsSignal->i4PER           = TunerGetSignalPER(_u1TunerId,rsConnType);
    ptRsSignal->u1SSI           = TunerGetSignalLevel(_u1TunerId,rsConnType);
    ptRsSignal->u1SQI           = (UINT8)i4SQI;
    QueuePutTail(&rsSignalList, &ptRsSignal->link);    
}

void NIM_RangeSearchDVBCPrintf(UINT32 u4FreqStartKHz, UINT32 u4FreqEndKHz)
{
    UINT16                  ui2_i;
    RS_DVB_SIGNAL_ENTITY_T  * rsSignalEntity = NULL; //To get entity from queue
    GENERIC_LIST_ENTRY      list_backup;
    
    if(rsSignalList.Count == 0)
    {
        mcSHOW_USER_CRI_MSG(("\rNo channel found between %dKHz and %dKHz\n",u4FreqStartKHz,u4FreqEndKHz));
        return;
    }
    
    mcSHOW_USER_CRI_MSG(("\r\n=====================Range Search DVBC=======================\n"));
    mcSHOW_USER_CRI_MSG(("%d channel found between %dKHz and %dKHz\n",rsSignalList.Count,u4FreqStartKHz,u4FreqEndKHz));
    ui2_i = 1;
    rsSignalEntity = (RS_DVB_SIGNAL_ENTITY_T *)QueueGetHead(&rsSignalList);
	while(rsSignalEntity)
	{
        //Freq
        mcSHOW_USER_CRI_MSG(("Channel %3d: [Freq %6dkHz], ",
              ui2_i, rsSignalEntity->s4Freq));
        ui2_i++;
        
        //u1mod
        switch (rsSignalEntity->i4Mod)
        {
            case 16:    mcSHOW_USER_CRI_MSG(("[QamMode= 16QAM], "));    break;
            case 32:    mcSHOW_USER_CRI_MSG(("[QamMode= 32QAM], "));    break;
            case 64:    mcSHOW_USER_CRI_MSG(("[QamMode= 64QAM], "));    break;
            case 128:   mcSHOW_USER_CRI_MSG(("[QamMode=128QAM], "));    break;
            case 256:   mcSHOW_USER_CRI_MSG(("[QamMode=256QAM], "));    break;
            default:    mcSHOW_USER_CRI_MSG(("[QamMode=N/A QAM], "));   break;
        }

        //SymbolRate
        mcSHOW_USER_CRI_MSG(("[SymbolRate %6d], ",
              rsSignalEntity->u4SymbolRate));

        mcSHOW_USER_CRI_MSG(("[SNR=%3d/10dB], [PER=%5d*10^-5], [SSI=%3d], [SQI=%3d]",
              rsSignalEntity->u2SNR, rsSignalEntity->i4PER,
              rsSignalEntity->u1SSI, rsSignalEntity->u1SQI));

        mcSHOW_USER_CRI_MSG(("\n"));

        x_memcpy(&list_backup,&rsSignalEntity->link,sizeof(GENERIC_LIST_ENTRY));
		x_mem_free(rsSignalEntity);	//destory the memory allocated
		rsSignalEntity = (RS_DVB_SIGNAL_ENTITY_T *)QueueGetNext(&rsSignalList,&list_backup);
	}
}

void NIM_RangeSearchDVBC(UINT32 u4FreqStartKHz,UINT32 u4FreqEndKHz)
{
    BOOL                    fgRetSts = FALSE;     
    SIZE_T                  ewsInfoLen; 
    UINT32                  u4_dfreq, u4_CurFreq;
    TUNER_DIG_ATTRIBUTE_T   DigAttribute;

    //Used when call TunerAcq
	DRV_COMP_ID_T           rsTunerCompId;
	DRV_CONN_TYPE_T         rsConnType = TUNER_CONN_TYPE_CAB_ANA;
	TUNER_ALL_INFO_T        rsConnInfo;
    
    //Check Freq    
    if(NIM_RangeSearchDVBCCheck(&u4FreqStartKHz, &u4FreqEndKHz) == FALSE)
    {
        mcSHOW_USER_CRI_MSG(("Freq Range Error, Stop Search!\n")); 
        return;
    }

    TunerGetAttribute(_u1TunerId,TUNER_GET_TYPE_DIG_ATTRIBUTE,(void *)&DigAttribute,(SIZE_T*)&ewsInfoLen);
    u4_dfreq = (DigAttribute.ui4_fine_tune_delta_freq / 1000)*2;

    //Set Conn Type
    NIM_SetConnType(TUNER_CONN_TYPE_CAB_DIG);

    TunerDetachMW(_u1TunerId, TRUE);
    
    //Init for TunerAcq
    QueueInitList(&rsSignalList);
	rsTunerCompId.ui2_id = _u1TunerId;
	rsTunerCompId.e_type = DRVT_TUNER_CAB_DIG;    

    //Printf Icon
    NIM_RangeSearchPrintfTimerInit();

    u4_CurFreq = u4FreqStartKHz;
    //Start Scan
    while(1)
    {
        if(u4_CurFreq > u4FreqEndKHz)
        {
            break;
        }
        
        rsConnInfo.t_cab_tuner_info.e_mod           = MOD_UNKNOWN;
        rsConnInfo.t_cab_tuner_info.ui4_freq        = u4_CurFreq * 1000;
        rsConnInfo.t_cab_tuner_info.e_conn_mode     = CONN_MODE_SCAN; 
        rsConnInfo.t_cab_tuner_info.ui4_sym_rate    = 0;

        fgRetSts = TunerAcq(_u1TunerId,rsConnType,(void *)&rsConnInfo,
			      sizeof(TUNER_ALL_INFO_T),RS_TunerBreak,&rsTunerCompId);
        
        if(fgRetSts == TRUE)
        {
            NIM_RangeSearchDVBCStoreCH();
            u4_CurFreq = u4_CurFreq + 8000 + u4_dfreq;
        }
        else
        {
            u4_CurFreq = u4_CurFreq + u4_dfreq;
        }
    }

    NIM_RangeSearchPrintfTimerDelete();

    //Printf Icon
    NIM_RangeSearchDVBCPrintf(u4FreqStartKHz, u4FreqEndKHz);
    
    TunerDetachMW(_u1TunerId, FALSE);
}
void NIM_RangeSearch(DRV_CONN_TYPE_T e_conn_type, UINT32 u4FreqStartKHz,UINT32 u4FreqEndKHz, const CHAR **szArgv)
{
    if(_u1TunerId == MTK_ANALOG_0)
    {
        mcSHOW_USER_CRI_MSG(("Rangesearch ATV Start------->>>>>>>>>>>>>\n"));
        NIM_RangeSearchATV(u4FreqStartKHz, u4FreqEndKHz);
    }
    else
    {
        if(e_conn_type == TUNER_CONN_TYPE_TER_DIG)
        {
            mcSHOW_USER_CRI_MSG(("Rangesearch DVBT Start------->>>>>>>>>>>>>\n"));
            NIM_RangeSearchDVBT(u4FreqStartKHz, u4FreqEndKHz, szArgv);  
        }
        else if(e_conn_type == TUNER_CONN_TYPE_CAB_DIG) 
        {
            mcSHOW_USER_CRI_MSG(("Rangesearch DVBC Start------->>>>>>>>>>>>>\n"));
            NIM_RangeSearchDVBC(u4FreqStartKHz, u4FreqEndKHz);
        }
    }
}

static ANA_SIGNAL_INFO signalList[10];
void NIM_SetAnaChList(INT32 s4Freq,UINT8 u1SubSysIdx)
{
   static UINT8 u1Index=0;
   UINT8 ii=0;
   UINT8 u1len=sizeof(signalList)/sizeof(signalList[0]);
   if(u1Index<u1len)
   {
     signalList[u1Index].s4Freq=s4Freq;
	 signalList[u1Index++].u1SubSysIdx=u1SubSysIdx;
   }
   if(s4Freq==0)
   { 
    if(u1Index!=1)
	{
	   //print all information
       while(ii<u1len)
       {
	   if(signalList[ii].s4Freq==0)break;
        printf("[%d]:Freq:%dKHz,SubSys=%d\n",ii,signalList[ii].s4Freq,signalList[ii].u1SubSysIdx);
        ii++;		
       }
	}
	else//clear array
	{
	  x_memset(signalList, 0, u1len*sizeof(ANA_SIGNAL_INFO));
	}
      u1Index=0; 
   }   
}
ANA_SIGNAL_INFO * NIM_GetAnaChInfo(UINT32 *pu4ChannelIdx)
{
    UINT8 u1len=sizeof(signalList)/sizeof(signalList[0]);
    ANA_SIGNAL_INFO * pAnaChInfo=signalList;
	
	if(*pu4ChannelIdx<u1len)
   {
	 if(0==pAnaChInfo[*pu4ChannelIdx].s4Freq)
     {
	  *pu4ChannelIdx=0;//reset to the beginning
	 }
   }
   else
   {
      *pu4ChannelIdx=0;//reset to the beginning.Loop
   }
    return pAnaChInfo+(*pu4ChannelIdx);
}
//-----------------------------------------------------------------------------
// NIM_MonitorStart
//
/** Tuner Primitive API: start monitor.
 *
 *
 *  @return INT32.\n
 *      always return 0
 */
//-----------------------------------------------------------------------------
INT32 NIM_MonitorStart(void)
{
    if (!_fgNimTaskInited)
    {
        _fgNimTaskInited = TRUE;

        // monitor TSERR pin status.
        //VERIFY(0 <= GPIO_Reg(D_NIM_MORNITOR_GPIO, GPIO_TYPE_INTR_RISE, _nim_TsErrGpio));
    }

    return 0;
}

//-----------------------------------------------------------------------------
// NIM_MonitorStop
//
/** Tuner Primitive API: stop monitor.
 *
 *
 *  @return INT32.\n
 *    always return 0
 */
//-----------------------------------------------------------------------------
INT32 NIM_MonitorStop(void)
{
    return 0;
}

//-----------------------------------------------------------------------------
// NIM_Open
//
/** Tuner Primitive API: Open tuner.
 *
 *
 *  @return INT32.\n
 *          If success, return DRVAPI_TUNER_OK( = 0).
 *          If fail, return error code ( < 0).
 */
//-----------------------------------------------------------------------------
INT32 NIM_Open(void)
{
    INT32 i4Ret;

    i4Ret = TunerOpen();
    mcSHOW_USER_CRI_MSG(("TunerOpen() return %d\n", (INT16)i4Ret));
    return i4Ret;
}

//-----------------------------------------------------------------------------
// NIM_Close
//
/** Tuner Primitive API: close tuner.
 *
 *
 *  @return INT32.\n
 *          If success, return DRVAPI_TUNER_OK( = 0).
 *          If fail, return error code ( < 0).
 */
//-----------------------------------------------------------------------------
INT32 NIM_Close(void)
{
    return TunerClose();
}

//-----------------------------------------------------------------------------
// NIM_SetCable
//
/** Tuner Primitive API: tuner set cable parameter.
 *
 *  @param[in]     u4FreqK      frequency khz.
 *  @param[in]     u4SymbolK    k_symbol_sec.
 *  @param[in]     i4QamSize    QAM size.
 *  @param[in]     fgSpectrumInversion    SpectrumInversion.
 *  @param[in]     pfNimBreak    break function.
 *
 *  @return INT32.\n
 *          If success, return DRVAPI_TUNER_OK( = 0).
 *          If fail, return error code ( < 0).
 */
//-----------------------------------------------------------------------------
BOOL NIM_SetCable(UINT32 u4FreqK, UINT32 u4SymbolK, INT32 i4QamSize,
    BOOL fgSpectrumInversion, PF_NIM_BREAK_CB_T pfNimBreak, TUNER_ACQ_ACTION_T uAcqType, UINT32 uLoopCounts, UINT32 u4DelayMs)
{
//UINT32  fFreqM;
TUNER_ACQ_ACTION_T  eActionType;
TUNER_ALL_INFO_T    tConnInfo;
UINT32  u4TickCnt, i=0;
BOOL    fgRetSts = FALSE;

    tConnInfo.t_ter_tuner_info.e_bandwidth = _eBandwidth;

//    fFreqM = (UINT32)u4FreqK / 1000; // convert to MHz
//    _nim_FormatConnectInfo(_e_conn_type, &tConnInfo, fFreqM, u4SymbolK, i4QamSize,
//        fgSpectrumInversion);
    _nim_FormatConnectInfo(_e_conn_type, &tConnInfo, u4FreqK, u4SymbolK, i4QamSize,
        fgSpectrumInversion);

#if 1 // Temp removed due to other driver not ready at drv_build initial stage
    if(_u1TunerId != MTK_ANALOG_0){ /*acq digital, disconnect analog*/
        TunerDisc(MTK_ANALOG_0, TUNER_CONN_TYPE_TER_ANA);
//        CLI_Parser_Arg("nptv.ms dtv1");
    }
    else{       /*acq analog, disconnect digital*/
           TunerDisc(MTK_MT5111_0, TUNER_CONN_TYPE_TER_DIG);
//           CLI_Parser_Arg("nptv.ms tv");
          u4DelayMs=2000;//delay 2s when analog acquisition
    }
#endif

    u4TickCnt = os_get_sys_tick();

    eActionType = uAcqType;
    TunerSetAttribute(_u1TunerId, TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS, (void *) &eActionType, sizeof(TUNER_ACQ_ACTION_T));

    for(i=0; i<uLoopCounts; i++)
    {
        u4TickCnt = os_get_sys_tick();

        mcSHOW_USER_CRI_MSG(("NIM_SetCable(): Start TunerAcq uloopCounts=%d\n",(INT16)i));

        _nim_SetTimeout2(20000); // timeout after 20 seconds
        //mcDELAY_MS(20000); //EC_debug_dB608
        fgRetSts = TunerAcq(_u1TunerId, _e_conn_type, &tConnInfo, sizeof(TUNER_ALL_INFO_T), (x_break_fct)pfNimBreak, NULL);
        u4TickCnt = os_get_sys_tick()-u4TickCnt;
        mcSHOW_USER_CRI_MSG((" NIM_SetCable():End TunerAcq, Connection time = %d ms, delayMs=%d ms Status=%d\n", (INT16)(u4TickCnt*x_os_drv_get_tick_period()), (INT16)u4DelayMs,fgRetSts));
        mcDELAY_MS(u4DelayMs);
    }
    return fgRetSts;
}

//-----------------------------------------------------------------------------
// NIM_DetachMW
//
/** Tuner Primitive API: tuner detach middleware.
 *
 *  @param[in]     bDetach     1: detach , 0: not detach.
 *
 *  @return void.\n
 */
//-----------------------------------------------------------------------------
void    NIM_DetachMW(BOOL bDetach)
{
    TunerDetachMW(_u1TunerId, bDetach);
}

LINT_EXT_HEADER_BEGIN
//-----------------------------------------------------------------------------
// NIM_GetCableSignal
//
/** Tuner Primitive API: tuner get cable signal parameter.
 *
 *  @param[out]     prCableSignal     pointer to cable signal parameter structure.
 *
 *  @return void.\n
 */
//-----------------------------------------------------------------------------
void NIM_GetCableSignal(SIGNAL *prCableSignal)
{
    TunerGetSignal(_u1TunerId, _e_conn_type, prCableSignal);
}
LINT_EXT_HEADER_END

//-----------------------------------------------------------------------------
// NIM_GetCableSignalLevel
//
/** Tuner Primitive API: tuner get cable signal level.
 *
 *
 *  @return UINT8.\n
 *          return signal level
 */
//-----------------------------------------------------------------------------
UINT8 NIM_GetCableSignalLevel(void)
{
    return TunerGetSignalLevel(_u1TunerId, _e_conn_type);
}

//-----------------------------------------------------------------------------
// NIM_GetCableSync
//
/** Tuner Primitive API: tuner get cable sync.
 *
 *
 *  @return UINT16.\n
 *      success =1, fail <=0
 */
//-----------------------------------------------------------------------------
INT16 NIM_GetCableSync(void)
{
    return TunerGetSync(_u1TunerId, _e_conn_type);
}

//-----------------------------------------------------------------------------
// NIM_GetErrorCnt
//
/** Tuner Primitive API: tuner get error count.
 *
 *
 *  @return UINT32.\n
 *      return error count
 */
//-----------------------------------------------------------------------------
#ifndef CC_MT5880_CODE_SIZE_CTRL
UINT32 NIM_GetErrorCnt(void)
{
    return _u4NimTsErrCount;
}
#endif
//-----------------------------------------------------------------------------
// NIM_GetCableSignalSNR
//
/** Tuner Primitive API: tuner get cable signal SNR.
 *
 *
 *  @return UINT16.\n
 *      return signal SNR
 */
//-----------------------------------------------------------------------------
UINT16 NIM_GetCableSignalSNR(void)
{
    return TunerGetSignalSNR(_u1TunerId, _e_conn_type);
}
#if 1
//-----------------------------------------------------------------------------
// NIM_TunerNimTest
//
/** Tuner Primitive API: NIM_TunerNimTest.
 *
 *
 *  @return UINT16.\n
 *      return 0    ok
 *      return !=0  fail
 */
//-----------------------------------------------------------------------------
INT32 NIM_TunerNimTest(void)
{
    INT32 i4Ret;

    i4Ret = TunerNimTest(_u1TunerId);
    if(i4Ret == 0){
        mcSHOW_USER_CRI_MSG(("TunerNimTest(TunerId=%d) ok(return=%d)\r\n",_u1TunerId,i4Ret));
    }
    else{
        mcSHOW_USER_CRI_MSG(("TunerNimTest(TunerId=%d) fail(return=%d)\r\n",_u1TunerId,i4Ret));
    }
    return i4Ret;
}
#endif
//-----------------------------------------------------------------------------
// NIM_GetCableSignalPER
//
/** Tuner Primitive API: tuner get cable signal PER.
 *
 *
 *  @return UINT16.\n
 *      return signal PER
 */
//-----------------------------------------------------------------------------
UINT16 NIM_GetCableSignalPER(void)
{
    return TunerGetSignalPER(_u1TunerId, _e_conn_type);
}

//-----------------------------------------------------------------------------
// NIM_DetachI2C
//
/** Tuner Primitive API: tuner detach I2C.
 *
 *
 *  @return UINT32.\n
 *      always return 0
 */
//-----------------------------------------------------------------------------
UINT32 NIM_DetachI2C(UINT32 u4Param)
{
    TunerDetachI2C(_u1TunerId, u4Param);
    return 0;
}

UINT32 NIM_SetFineTune(BOOL fgFineTune)
{
//static BOOL _fgFineTune = FALSE;

//    _fgFineTune = fgFineTune;
    return 0;
}

void NIM_SetTsSP(UCHAR ucSerorPal)
{
    SIG_SP_T e_ts_sp = (SIG_SP_T)ucSerorPal;

    TunerSetAttribute(_u1TunerId, TUNER_SET_TYPE_TS_SP, (void *)&e_ts_sp, sizeof(SIG_SP_T));
}

void NIM_SetTsTristate(UCHAR ucTristate)
{ 
    BOOL_T e_ts_tristate = (BOOL_T)ucTristate;

    TunerSetAttribute(_u1TunerId, TUNER_SET_TYPE_TS_TRISTATE, (void *)&e_ts_tristate, sizeof(BOOL_T));
}

void NIM_GetTsFmt(UCHAR *pucSerorPal, UCHAR *pucClockPol, UINT32 *pu4TsClock, BOOL *pbGatedClock, BOOL *pbExternalDemod)
{
    TS_FMT_T rTsFmt = {SP_UNKNOWN, POL_UNKNOWN_ACTIVE};
    SIZE_T u4TsFmtLen;

    TunerGetAttribute(_u1TunerId, TUNER_GET_TYPE_TS_FMT, (void *)&rTsFmt, &u4TsFmtLen);
    *pucSerorPal = (UCHAR)rTsFmt.SerOrPar;
    *pucClockPol = (UCHAR)rTsFmt.ClockPolarity;
    *pu4TsClock =  rTsFmt.u4TsClockInKHz;
    *pbGatedClock = rTsFmt.fgGatedClock;
    *pbExternalDemod = rTsFmt.fgExternalDemod;
}

void NIM_SetTuner(UCHAR ucAddr, UCHAR *pucData, UCHAR ucLen)
{
    DigTunerBypassI2C(TRUE);
    ICtrlBus_I2cTunerWrite(270, ucAddr, pucData, ucLen);
    DigTunerBypassI2C(FALSE);
}

void NIM_GetTuner(UCHAR ucAddr, UCHAR *pucData, UCHAR ucLen)
{
    DigTunerBypassI2C(TRUE);
    ICtrlBus_I2cTunerRead(270, ucAddr, pucData, ucLen);
    DigTunerBypassI2C(FALSE);
}

void NIM_GetTunerEx(UCHAR ucAddr, UCHAR ucReg, UCHAR *pucData, UCHAR ucLen)
{
    DigTunerBypassI2C(TRUE);
    ICtrlBus_I2cTunerRead_Ext(270, ucAddr, ucReg, pucData, ucLen);
    DigTunerBypassI2C(FALSE);
}
void NimCmdSetBSContinue(void)
{
    TunerSetAttribute(_u1TunerId, TUNER_SET_TYPE_SCAN_CONTINUE,NULL,0);//set blind scan continue
}
void NimCmdSetBSCStop(void)
{
     TunerDisc(_u1TunerId,TUNER_CONN_TYPE_SAT_DIG);//set blind scan stop
}

LINT_EXT_HEADER_BEGIN
#ifndef CC_MT5880_CODE_SIZE_CTRL
void NIM_RegErrNotifyFunc(PFN_NIM_ERR_NOTIFY pfFunc)
{
}
#endif
LINT_EXT_HEADER_END
