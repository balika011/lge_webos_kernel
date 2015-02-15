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
 * $RCSfile: drv_nr_anr.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// Inclusions
////////////////////////////////////////////////////////////////////////////////
#define _DRV_NR_ANR_C_

#include "vdo_misc.h"
#include "video_def.h"
#include "hw_vdoin.h"
#include "hw_nr.h"
#include "hw_sw.h"

#ifdef CC_MT5398
#include "hw_tdsharp.h"
#endif

#include "nr_debug.h"
#include "drv_nr.h"
#include "drv_di_int.h"
#include "drv_bnr_int.h"
#include "drv_nr_int.h"

#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"

#if 0
static NR_ADAP_PARAM_REG_T _rNrPrmReg[NUM_PARAM_GROUP] =
{
	{ 4, 23, {4,  8, 12, 20, 25, 30, 40, 60}, {32, 40, 40, 32, 20, 15, 10, 5}}, /*0*/
	{ 8, 20, {5,  9, 14, 23, 28, 34, 45, 65}, {33, 41, 41, 32, 20, 15, 10, 5}},//1 
	{12, 17, {5, 10, 16, 25, 31, 38, 50, 70}, {33, 42, 41, 32, 21, 15, 11, 5}}, /*2*/
	{16, 14, {6, 11, 18, 28, 34, 41, 55, 75}, {34, 43, 42, 31, 21, 15, 11, 5}},
	{18, 12, {6, 12, 20, 30, 38, 45, 60, 80}, {34, 44, 42, 31, 21, 16, 11, 5}}, /*4*/
	{20,  9, {7, 13, 22, 33, 41, 49, 65, 85}, {35, 45, 43, 31, 21, 16, 11, 5}},
	{24,  6, {7, 14, 24, 35, 44, 53, 70, 90}, {35, 46, 43, 31, 22, 16, 12, 5}}, /*6*/
	{26,  3, {8, 15, 26, 38, 47, 56, 75, 95}, {36, 47, 44, 30, 22, 16, 12, 5}},
	{28,  0, {8, 16, 28, 40, 50, 60, 80,100}, {36, 48, 44, 30, 22, 16, 12, 5}},  /*8*/
	{32,  0, {8, 16, 28, 40, 50, 60, 80,100}, {36, 48, 44, 30, 22, 16, 12, 5}}
};
#endif

#define NR_WR_ST_NULL    0
#define NR_WR_ST_IDLE    1
#define NR_WR_ST_AUTO    2
#define NR_WR_ST_FROM_SW 3
#define NR_WR_ST_RESET   4

#define NR_MOIDX_MAX 16
static NR_IIR_T _rIIRMotion;

typedef struct
{
    UINT8 u1SnrGain;
    UINT8 u1TdsCorZero;
    UINT8 u1MnrEdgeTh;
} NR_WRITE_MOANR_REG_T;

static NR_WRITE_MOANR_REG_T _rNrWRegMoAnr;

typedef enum
{
    E_NR_DFT_ADAP_NL    = 0,
    E_NR_DFT_ADAP_XNR   = 1,
    E_NR_DFT_ADAP_TNR   = 2,
    E_NR_DFT_ADAP_SNR   = 3,
    E_NR_DFT_ADAP_RNR   = 4,
    E_NR_DFT_ADAP_FUNC_MAX
} E_NR_DFT_ADAP_FUNC;

typedef enum
{
    E_NR_MOANR_NL    = 0,
    E_NR_MOANR_XNR   = 1,
    E_NR_MOANR_RNR   = 2,
    E_NR_MOANR_MOANR = 3,
    E_NR_MOANR_FUNC_MAX
} E_NR_MOANR_FUNC;

typedef enum
{
    E_NR_ANR_MODULE_DFT   = 0,
    E_NR_ANR_MODULE_MOANR = 1,
    E_NR_ANR_MODULE_MAX
} E_NR_ANR_MODULE;

typedef struct _NR_ADAP_WREG_BASE_T
{
    BOOL (*fgAutoOn)(void);
    BOOL (*fgFromSW)(void);
    void (*vSet)(void);
    void (*vReset)(void);
    UINT8 u1State;
} NR_ADAP_WREG_BASE_T;

typedef struct _NR_ADAP_MODEUL_T
{
    void (*vModuleProc)(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna);
    NR_ADAP_WREG_BASE_T* parSubFunc;
    char* sDesc;
} NR_ADAP_MODULE_T;

typedef enum
{
    E_NR_LV_MIN,
    E_NR_LV_MAX,
    E_NR_LV_NUM
} E_NR_STR_LVL;

typedef struct
{
    UINT8 u1Idx;
    UINT8 u1SubWei;
    UINT8 u1IIRMaxWeight;
    UINT8 u1MAWeiThd[8];
    UINT8 u1MAWeiTbl[8];
	UINT8 u1MACWeiTbl[8];
} NR_TNR_PARAM_REG_T;

typedef struct
{
    UINT8 u1Idx;
    UINT8 u1SnrGain;
} NR_SNR_PARAM_REG_T;

typedef struct
{
    UINT32 u4A;
    UINT32 u4B;
    UINT32 u4C;
    UINT32 u4Gain;
    UINT32 u4Offset;
    UINT16 u2SnrThm;
    UINT8 u1CostDirAdap;
    UINT8 u1MARto;      // beta
    UINT8 u1GeneralStr; // k
} NR_SNR_TH_T;
    


typedef struct
{
    UINT8 u1NoiseLevel;
    UINT8 u1SnrGain;
    UINT8 u1SnrBeta;
    UINT8 u1SnrK;
    UINT8 u1DcAlpha;
    UINT8 u1MASubWeiBase;
    UINT8 u1MASubWeiNLGain;
    UINT8 u1MAMaxWei;
    UINT8 arMATbl[8];
    UINT8 arMACTbl[8];
    UINT8 arMAThd[8];
    UINT16 u2SnrThm;
    UINT16 u2RnrThAgg;
    UINT16 u2RnrThCon;
} NR_ADAP_WRITE_REG_T;



static void _vSetNL(void);
static void _vResetNL(void);
static void _vSetXNR(void);
static void _vResetXNR(void);
static void _vSetSNR(void);
static void _vResetSNR(void);
static void _vSetTNR(void);
static void _vResetTNR(void);
static void _vSetRNR(void);
static void _vResetRNR(void);
static void _vSetMOANR(void);
static void _vResetMOANR(void);

static BOOL _fgAutoOnNL(void);
static BOOL _fgAutoOnXNR(void);
static BOOL _fgAutoOnSNR(void);
static BOOL _fgAutoOnTNR(void);
static BOOL _fgAutoOnRNR(void);
static BOOL _fgAutoOnMOANR(void);

static BOOL _fgFromSWOff(void);
static BOOL _fgFromSWTNR(void);
static BOOL _fgFromSWSNR(void);
static BOOL _fgFromSWRNR(void);
static BOOL _fgFromSWMOANR(void);

static void _vAnrDftProc(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna);
static void _vAnrMoAnrProc(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna);

static NR_TNR_PARAM_REG_T _rNrTnr[E_NR_LV_NUM] = 
{
    // u1Idx, u1SubWei, u1IIRMaxWeight, u1MAWeiThd, u1MAWeiTbl
    { 8, 32, 23, {4,  8, 12, 20, 25, 30, 40, 60}, {32, 40, 40, 32, 20, 15, 10, 5}},
    {32,  0,  0, {8, 16, 28, 40, 50, 60, 80,100}, {36, 48, 44, 30, 22, 16, 12, 5}}
};

static NR_SNR_PARAM_REG_T _rNrSnr[E_NR_LV_NUM] =
{
    // u1Idx, u1SnrGain
    { 8, 20},
    {32, 32}
};

static NR_SNR_TH_T _rSnrTh = {0};

static NR_ADAP_WRITE_REG_T _rNrWriteReg;

static NR_ADAP_WREG_BASE_T _rDftNrWRegState[E_NR_DFT_ADAP_FUNC_MAX+1] =
{
    {_fgAutoOnNL,  _fgFromSWOff, _vSetNL,  _vResetNL,  NR_WR_ST_IDLE},
    {_fgAutoOnXNR, _fgFromSWOff, _vSetXNR, _vResetXNR, NR_WR_ST_IDLE},
    {_fgAutoOnTNR, _fgFromSWTNR, _vSetTNR, _vResetTNR, NR_WR_ST_IDLE},
    {_fgAutoOnSNR, _fgFromSWSNR, _vSetSNR, _vResetSNR, NR_WR_ST_IDLE},
    {_fgAutoOnRNR, _fgFromSWRNR, _vSetRNR, _vResetRNR, NR_WR_ST_IDLE},
    {NULL, NULL, NULL, NULL, NR_WR_ST_NULL}
};

static NR_ADAP_WREG_BASE_T _rMoNrWRegState[E_NR_MOANR_FUNC_MAX+1] =
{
    {_fgAutoOnNL,  _fgFromSWOff, _vSetNL,  _vResetNL,  NR_WR_ST_IDLE},
    {_fgAutoOnXNR, _fgFromSWOff, _vSetXNR, _vResetXNR, NR_WR_ST_IDLE},
    {_fgAutoOnRNR, _fgFromSWRNR, _vSetRNR, _vResetRNR, NR_WR_ST_IDLE},
    {_fgAutoOnMOANR, _fgFromSWMOANR, _vSetMOANR, _vResetMOANR, NR_WR_ST_IDLE},
    {NULL, NULL, NULL, NULL, NR_WR_ST_NULL}
};

static NR_ADAP_MODULE_T _rAnrModule[E_NR_ANR_MODULE_MAX] = 
{
    {_vAnrDftProc,   _rDftNrWRegState, "Default Adaptive NR"},
    {_vAnrMoAnrProc, _rMoNrWRegState,  "Motion Adaptive NR"}
};

static UINT8 _u1CurAnrModuleIdx = 0;
static BOOL _fgInit = 0;

////////////////////////////////////////////////////////////////////////////////
// ANR functions
////////////////////////////////////////////////////////////////////////////////
static void _vAnrResetModule(NR_ADAP_WREG_BASE_T* prModule)
{
    while (prModule->u1State != NR_WR_ST_NULL)
    {
        prModule->vReset();
        prModule->u1State = NR_WR_ST_IDLE;
        prModule ++;
    }
}

static UINT32 _u4EstSNRManualTh(UINT32 u4NoiseLvl)
{
    UINT32 a = _rSnrTh.u4A;
    UINT32 b = _rSnrTh.u4B;
    UINT32 c = _rSnrTh.u4C;
    UINT32 gain = _rSnrTh.u4Gain;
    UINT32 ofst = _rSnrTh.u4Offset;
    
    UINT32 estThm = (UINT32) (a*u4NoiseLvl*u4NoiseLvl + b*u4NoiseLvl + c) * gain + (2048*8);

    if (estThm & 0x80000000)
    {
        estThm = 0;
    }
    else
    {
        estThm = (estThm >> (12+4));
    }

    return ((estThm + ofst)*2); // consider dir_adap, 
}

static UINT32 _u4EstBlockNL(void)
{
    UINT32 u4FnlStr = _SWR(NR_BLK_STA_0F, NR_BLK_FNL_STR);
    UINT32 u4MaxStr = _SWR(NR_BK_METER_01, NR_BK_NL_MAX_STR);
    UINT32 u4BkNL = _SWR(NR_BK_METER_01, NR_BK_NL_GAIN);

    u4BkNL = (u4BkNL * MIN(u4FnlStr, u4MaxStr)) >> 6;

    return MIN(u4BkNL, 63);
}

static UINT32 _u4EstRNRAggTh(UINT32 u4NoiseLvl)
{
    UINT32 ofst = _SWR(NR_PQ_17, RNR_AGG_THM_OFST);

    INT32 estThm = 
        ((u4NoiseLvl*u4NoiseLvl*u4NoiseLvl) / 16) - ((INT32)32*u4NoiseLvl*u4NoiseLvl / 10) + 81*u4NoiseLvl + 32;

    estThm = MAX(MIN((INT32)estThm, (INT32)(16*_SWR(NR_PQ_13, RNR_MAX_TH))),0);
	
	if(!_EN(EN_RNR_TH_ADAP))//rnr_th_adap=0,RNR_AGG_THM_OFST can tune rnraggth.
	{
		estThm=MIN(ofst, (INT32)(16*_SWR(NR_PQ_13, RNR_MAX_TH)));
	}

    return estThm;
}

static UINT8 u1DrvNXNRGetBlkConf(void)
{
    return (BNR_FW_IsStop() ? NR_R(NXNR_20, NX_STA_VBLK_CONFIDENCE) : NR_R(NXNR_0D, NX_MANUAL_VBLK_CONFIDENCE));
}


static void _vDrvTnrGetParam(void)
{
    UINT32 u4PQ00;
    UINT32 u4PQ01;
    UINT32 u4PQ03;
    UINT32 u4PQ04;
    UINT32 u4PQ05;

    UINT32 tmp;
    
    u4PQ00 = _SWR_4B(NR_PQ_00);
    u4PQ01 = _SWR_4B(NR_PQ_01);
	u4PQ03 = _SWR_4B(NR_PQ_03);
    u4PQ04 = _SWR_4B(NR_PQ_04);
    u4PQ05 = _SWR_4B(NR_PQ_05);

    // get adaptive table from flash PQ
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ00, NR_TBL_MA_0), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ00, NR_TBL_MA_1), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ00, NR_TBL_MA_2), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ00, NR_TBL_MA_3), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ00, NR_TBL_MA_4), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ01, NR_TBL_MA_5), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ01, NR_TBL_MA_6), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ01, NR_TBL_MA_7), 63);
    _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight = R_Fld(u4PQ05, NR_IIR_MAX_WEI);
    _rNrTnr[E_NR_LV_MIN].u1SubWei = _SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_BASE_DFT);

    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_0), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_1), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_2), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_3), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_4), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_5), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_6), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_7), 63); 
    _rNrTnr[E_NR_LV_MAX].u1IIRMaxWeight = R_Fld(u4PQ05, NR_IIR_MAX_WEI_E);
    _rNrTnr[E_NR_LV_MAX].u1SubWei = 0;

	// get adaptive table from flash PQ
    u4PQ00 = _SWR_4B(NR_PQ_0C);
    u4PQ01 = _SWR_4B(NR_PQ_0D);
	u4PQ03 = _SWR_4B(NR_PQ_15);
    u4PQ04 = _SWR_4B(NR_PQ_16);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[0] = MIN(R_Fld(u4PQ00, NR_TBL_C_0), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[1] = MIN(R_Fld(u4PQ00, NR_TBL_C_1), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[2] = MIN(R_Fld(u4PQ00, NR_TBL_C_2), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[3] = MIN(R_Fld(u4PQ00, NR_TBL_C_3), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[4] = MIN(R_Fld(u4PQ00, NR_TBL_C_4), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[5] = MIN(R_Fld(u4PQ01, NR_TBL_C_5), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[6] = MIN(R_Fld(u4PQ01, NR_TBL_C_6), 63);
    _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[7] = MIN(R_Fld(u4PQ01, NR_TBL_C_7), 63);

    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[0] = MIN(R_Fld(u4PQ03, NR_TBL_C_E_0), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[1] = MIN(R_Fld(u4PQ03, NR_TBL_C_E_1), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[2] = MIN(R_Fld(u4PQ03, NR_TBL_C_E_2), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[3] = MIN(R_Fld(u4PQ03, NR_TBL_C_E_3), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[4] = MIN(R_Fld(u4PQ03, NR_TBL_C_E_4), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[5] = MIN(R_Fld(u4PQ04, NR_TBL_C_E_5), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[6] = MIN(R_Fld(u4PQ04, NR_TBL_C_E_6), 63);
    _rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[7] = MIN(R_Fld(u4PQ04, NR_TBL_C_E_7), 63); 

    // table threshold
    tmp = _SWR_4B(NR_PQ_0E);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[0] = R_Fld(tmp, NR_TBL_THD_1);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[1] = R_Fld(tmp, NR_TBL_THD_2);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[2] = R_Fld(tmp, NR_TBL_THD_3);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[3] = R_Fld(tmp, NR_TBL_THD_4);
    tmp = _SWR_4B(NR_PQ_0F);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[4] = R_Fld(tmp, NR_TBL_THD_5);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[5] = R_Fld(tmp, NR_TBL_THD_6);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[6] = R_Fld(tmp, NR_TBL_THD_7);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[7] = R_Fld(tmp, NR_TBL_THD_8);
    tmp = _SWR_4B(NR_PQ_10);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[0] = R_Fld(tmp, NR_TBL_THD_E_1);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[1] = R_Fld(tmp, NR_TBL_THD_E_2);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[2] = R_Fld(tmp, NR_TBL_THD_E_3);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[3] = R_Fld(tmp, NR_TBL_THD_E_4);
    tmp = _SWR_4B(NR_PQ_11);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[4] = R_Fld(tmp, NR_TBL_THD_E_5);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[5] = R_Fld(tmp, NR_TBL_THD_E_6);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[6] = R_Fld(tmp, NR_TBL_THD_E_7);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[7] = R_Fld(tmp, NR_TBL_THD_E_8);
    tmp = _SWR_4B(NR_PQ_12);
    _rNrTnr[E_NR_LV_MIN].u1Idx = R_Fld(tmp, NR_NL_THD_MIN);
    _rNrTnr[E_NR_LV_MAX].u1Idx = R_Fld(tmp, NR_NL_THD_MAX);
    _rNrTnr[E_NR_LV_MAX].u1Idx = MAX(_rNrTnr[E_NR_LV_MIN].u1Idx+1, _rNrTnr[E_NR_LV_MAX].u1Idx);
}

static void _vDrvSnrGetParam(void)
{
    UINT32 tmp;
    UINT32 a = _SWR(NR_NM_05, SNR_THM_GAIN_A);
    UINT32 b = _SWR(NR_NM_05, SNR_THM_GAIN_B);
    UINT32 c = _SWR(NR_NM_06, SNR_THM_OFST_C);
    UINT32 g = _SWR(NR_NM_03, SNR_THM_GAIN);
    UINT32 ofst = _SWR(NR_NM_04, SNR_THM_OFST);
    UINT8 beta = _SWR(NR_PQ_08, NR_SNR_MANUAL_AUTO_RATIO);
    UINT8 k = _SWR(NR_PQ_08, NR_SNR_AUTO_TH_GENERAL_STR);
    UINT8 u1CostDirAdap = NR_R(NXNR_11, NX_SNR_COST_TH_DIR_ADAP);

    if (b & (1<<19))
        b = -(((~b)&0xfffff)+1);

    if (c & (1<<23))
        c = -(((~c)&0xffffff)+1);

    _rSnrTh.u4A = a;
    _rSnrTh.u4B = b;
    _rSnrTh.u4C = c;
    _rSnrTh.u4Gain = g;
    _rSnrTh.u4Offset = ofst;

    _rSnrTh.u1MARto = beta;
    _rSnrTh.u1GeneralStr = k;
    _rSnrTh.u1CostDirAdap = u1CostDirAdap;
      
    tmp = _SWR_4B(NR_PQ_12);

    _rNrSnr[E_NR_LV_MIN].u1Idx = R_Fld(tmp, SNR_NL_THD_MIN);
    _rNrSnr[E_NR_LV_MAX].u1Idx = R_Fld(tmp, SNR_NL_THD_MAX);

    _rNrSnr[E_NR_LV_MAX].u1Idx = MAX(_rNrSnr[E_NR_LV_MIN].u1Idx+1, _rNrSnr[E_NR_LV_MAX].u1Idx);

    tmp = _SWR_4B(NR_PQ_14);

    _rNrSnr[E_NR_LV_MIN].u1SnrGain = MIN(R_Fld(tmp, SNR_NL_GAIN_MIN), 32);
    _rNrSnr[E_NR_LV_MAX].u1SnrGain = MIN(R_Fld(tmp, SNR_NL_GAIN_MAX), 32);
}

static void _vDrvNXNRSelThm(void)
{
    static UINT8 _u4PreSel = 0xFF;
    UINT8 u4Sel = _SWR(NR_NM_07, SNR_THM_SEL);

    if (u4Sel != _u4PreSel)
    {
        switch (u4Sel)
        {
        default:
        case 0:
            _SWW(NR_NM_05, 0, SNR_THM_GAIN_A);
            _SWW(NR_NM_05, 0, SNR_THM_GAIN_B);
            _SWW(NR_NM_06, 0, SNR_THM_OFST_C);
            break;
        case 1:
            _SWW(NR_NM_05, 2300, SNR_THM_GAIN_A);
            _SWW(NR_NM_05, 75743, SNR_THM_GAIN_B);
            _SWW(NR_NM_06, (-908984), SNR_THM_OFST_C);
            break;
        case 2: // default: MC strong, SNR weak
            _SWW(NR_NM_05, 1679, SNR_THM_GAIN_A);
            _SWW(NR_NM_05, 136495, SNR_THM_GAIN_B);
            _SWW(NR_NM_06, (-1936630), SNR_THM_OFST_C);
            break;
        case 3: // MC weak, SNR strong
            _SWW(NR_NM_05, 3333, SNR_THM_GAIN_A);
            _SWW(NR_NM_05, -2287, SNR_THM_GAIN_B);
            _SWW(NR_NM_06, (125063), SNR_THM_OFST_C);
            break;
        }
    }

    _u4PreSel = u4Sel;
}

////////////////////////////////////////////////////////////////////////////////
// ANR On/Off, Set/Reset functions
////////////////////////////////////////////////////////////////////////////////

static BOOL _fgFromSWOff(void)
{
    return FALSE;
}

static BOOL _fgAutoOnNL(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_NOISELEVEL);
}

static void _vSetNL(void)
{
    NR_W(MCNR_17, _rNrWriteReg.u1NoiseLevel, MCNR_NOISE_LEVEL);
}

static void _vResetNL(void)
{
    NR_W(MCNR_17, 4, MCNR_NOISE_LEVEL);
}

static BOOL _fgAutoOnXNR(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_AUTO_XNR);
}

static void _vSetXNR(void)
{
    NR_W(MCNR_01, _rNrWriteReg.u1DcAlpha, MCNR_DCNR_ALPHA);
}

static void _vResetXNR(void)
{
    NR_W(MCNR_01, 0, MCNR_DCNR_ALPHA);
}

static BOOL _fgAutoOnSNR(void)
{
    return _EN(EN_ADAP_NR) && (_EN(EN_ADAP_SNR_THM) || _EN(EN_ADAP_SNR_BLK));
}

static BOOL _fgFromSWSNR(void)
{
    return _SWR(NR_CTRL_02, EN_ADAP_SNR_THM_SW);
}

static void _vSetSNR(void)
{
    NR_WM(NXNR_11,
        P_Fld(_rNrWriteReg.u2SnrThm, NX_SNR_MANUAL_TH)|
        P_Fld(_rNrWriteReg.u1SnrK, NX_SNR_AGGRESSIVE_LV)|
        P_Fld(_rNrWriteReg.u1SnrBeta, NX_SNR_MA_RATIO));
    
    //NR_W(NXNR_04, _rNrPQ.u1SnrGain, NX_MAX_SNR_RATIO);
    NR_W(NXNR_04, _rNrWriteReg.u1SnrGain, NX_SNR_GAIN_Y);
}

static void _vResetSNR(void)
{
    UINT32 u4SnrThm, u4K, u4ManualAutoRto, u4SnrGain;

    // 1. read SW reg
    _vDrvNXNRSelThm();
    _vDrvSnrGetParam();

    u4SnrThm = _u4EstSNRManualTh(0);
    u4K = MIN(15, _rSnrTh.u1GeneralStr);
    u4ManualAutoRto = _rSnrTh.u1MARto;
    u4SnrGain =_SWR(NR_PQ_09, NR_SNR_GAIN);// _rNrSnr[E_NR_LV_MIN].u1SnrGain;    

    if (_rSnrTh.u1CostDirAdap)
    {
        u4SnrThm = (u4SnrThm + 1) >> 1;
        u4K = (u4K + 1) >> 1;
    }

    // 2. reset HW reg
    NR_WM(NXNR_11,
        P_Fld(u4SnrThm, NX_SNR_MANUAL_TH)|
        P_Fld(u4K, NX_SNR_AGGRESSIVE_LV)|
        P_Fld(u4ManualAutoRto, NX_SNR_MA_RATIO));
    
    //NR_W(NXNR_04, _rNrPQ.u1SnrGain, NX_MAX_SNR_RATIO);
    NR_W(NXNR_04, u4SnrGain, NX_SNR_GAIN_Y);
}

static BOOL _fgAutoOnTNR(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_ADAP_WEIGHT);
}

static BOOL _fgFromSWTNR(void)
{
    return _SWR(NR_CTRL_02, EN_ADAP_WEIGHT_SW);
}

static void _vSetTNR(void)
{
    NR_W(MCNR_08, _rNrWriteReg.u1MAMaxWei, MCNR_IIR_MAX_WEI);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_MA, _rNrWriteReg.arMATbl);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_C,  _rNrWriteReg.arMACTbl);
    vDrvMCNRSetTblThd(_rNrWriteReg.arMAThd);

    NR_WM(MCNR_03,
        P_Fld(_rNrWriteReg.u1MASubWeiBase, MCNR_MOVINGTXR_SUB_WEI_BASE)|
        P_Fld(_SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN_DFT), MCNR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN));
}

static void _vResetTNR(void)
{
    // 1. read SW reg
    _vDrvTnrGetParam();

    // 2. reset HW reg
    NR_W(MCNR_08, _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight, MCNR_IIR_MAX_WEI);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_MA, _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_C,  _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl);
    vDrvMCNRSetTblThd(_rNrTnr[E_NR_LV_MIN].u1MAWeiThd);

    NR_WM(MCNR_03,
        P_Fld(_rNrTnr[E_NR_LV_MIN].u1SubWei, MCNR_MOVINGTXR_SUB_WEI_BASE)|
        P_Fld(_SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN_DFT), MCNR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN));
}

static BOOL _fgAutoOnRNR(void)
{
    return _EN(EN_ADAP_NR) && (_EN(EN_RNR_TH_ADAP)|| _EN(EN_BNR_RNR_ADAP));
}

static BOOL _fgFromSWRNR(void)
{
    return _SWR(NR_CTRL_02, EN_RNR_TH_ADAP_SW);
}

static void _vSetRNR(void)
{  
    NR_WM(NXNR_3E, P_Fld(_rNrWriteReg.u2RnrThAgg, NX_FORCE_RNR_TH_AGG)|P_Fld(_rNrWriteReg.u2RnrThCon, NX_FORCE_RNR_TH_CON));

}

static void _vResetRNR(void)
{
    NR_W(NXNR_3E, _SWR(NR_PQ_13, RNR_MAX_TH)*16, NX_FORCE_RNR_TH_AGG);
	NR_W(NXNR_3E, 0, NX_FORCE_RNR_TH_CON);
}

static BOOL _fgAutoOnMOANR(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_MOTION_ANR);
}

static BOOL _fgFromSWMOANR(void)
{
    return _SWR(NR_CTRL_02, EN_MOTION_ANR_SW);
}

static void _vSetMOANR(void)
{
    _vSetTNR();

    NR_W(NXNR_04, _rNrWRegMoAnr.u1SnrGain, NX_SNR_GAIN_Y);
    NR_W(NXNR_0E, _rNrWRegMoAnr.u1MnrEdgeTh, NX_MNR_EDGE_TH);
#ifdef CC_MT5398
    NR_W(TDSHP_27, _rNrWRegMoAnr.u1TdsCorZero, TDS_COR_ZERO);
#endif
}

static void _vResetMOANR(void)
{
    _vResetTNR();

    NR_W(NXNR_04, _SWR(NR_AUTO_00, SNR_ADP_MIN), NX_SNR_GAIN_Y);
    NR_W(NXNR_0E, _rNrWRegMoAnr.u1MnrEdgeTh, NX_MNR_EDGE_TH);
#ifdef CC_MT5398
    NR_W(TDSHP_27, _rNrWRegMoAnr.u1TdsCorZero, TDS_COR_ZERO);
#endif
}


////////////////////////////////////////////////////////////////////////////////
// ANR Processing
////////////////////////////////////////////////////////////////////////////////

static void _vAnrNLProc(UINT8 u1StrLvl)
{
    _rNrWriteReg.u1NoiseLevel = (UINT8) u1StrLvl;
}

static void _vAnrDCAlphaProc(UINT8 u1StrLvl)
{
    UINT32 u4DcAlpha = ((MIN(MAX(u1StrLvl, 20), 32) - 20) * 16) / (32-20);
    _rNrWriteReg.u1DcAlpha = (UINT8) u4DcAlpha;
    //_rNrFlfbSta.u1GmvRatio = NR_R(MCNR_18, MCNR_GMV_RATIO);    
    //INT32 gmv_de_dc_alpha = MAX( -6, MIN ( 4, (INT32) 22 - _rNmSta.u1GmvRatio ));   
    //UINT8 dc_alpha =  MAX( 0, MIN( 8, ( (INT32)u4NoiseLvl - 5 * 4 ) * 2 / 4 ));
    //NR_W(MCNR_01, dc_alpha, MCNR_DCNR_ALPHA);
}

static void _vAnrRnrProc(UINT8 u1StrLvl)
{
    UINT32 u4BlkLvl = 0;
    UINT32 u4RnrThCon = 0;
    UINT32 u4RnrTh =_u4EstRNRAggTh(u1StrLvl);
    
    if (_EN(EN_BNR_RNR_ADAP))
    {
        u4BlkLvl = _u4EstBlockNL();
        u4RnrThCon = _u4EstRNRAggTh(u4BlkLvl);
        u4RnrTh = LERP(16*_SWR(NR_PQ_13, BNR_RNR_TH), u4RnrTh, u1DrvNXNRGetBlkConf(), 4);
        u4RnrThCon = MIN(u4RnrThCon, u4RnrTh);		
    }
    
    _rNrWriteReg.u2RnrThAgg = u4RnrTh;
    _rNrWriteReg.u2RnrThCon = u4RnrThCon;

}

static void _vAnrSnrProc(UINT8 u1StrLvl)
{
    UINT32 idx, u4SnrThm, u4K, u4ManualAutoRto, u4SnrGain;
    UINT32 u4NL = _EN(EN_ADAP_SNR_THM) ? u1StrLvl : 0;
    UINT32 u4BlkLvl = _EN(EN_ADAP_SNR_BLK) ? _u4EstBlockNL() : 0;

    // 1. get parameter
    _vDrvNXNRSelThm();
    _vDrvSnrGetParam();

    // 2. calculation
    u4NL = MAX(u4NL, u4BlkLvl);
    _rSnrTh.u2SnrThm = LERP_TGT(_rSnrTh.u2SnrThm, _u4EstSNRManualTh(u4NL), _SWR(NR_NM_00, SNR_THM_IIR_WEI), 6);
    idx = LERP_F_C(_rNrSnr[E_NR_LV_MIN].u1Idx, 0, _rNrSnr[E_NR_LV_MAX].u1Idx, 32, u4NL); //MIN(((u4NL + 2 )/4), (NUM_PARAM_GROUP-1));
    u4SnrThm = _rSnrTh.u2SnrThm;
    u4K = MIN(15, _rSnrTh.u1GeneralStr);
    u4ManualAutoRto = LERP(0, _rSnrTh.u1MARto, MIN(idx, 32), 5);
    
    u4SnrGain = LERP(_rNrSnr[E_NR_LV_MAX].u1SnrGain, _rNrSnr[E_NR_LV_MIN].u1SnrGain, idx, 5);
    
    if (_rSnrTh.u1CostDirAdap)
    {
        u4SnrThm = (u4SnrThm + 1) >> 1;
        u4K = (u4K + 1) >> 1;
    }
    
    _rNrWriteReg.u2SnrThm = u4SnrThm;
    _rNrWriteReg.u1SnrK = u4K;
    _rNrWriteReg.u1SnrBeta = u4ManualAutoRto;
    _rNrWriteReg.u1SnrGain = u4SnrGain;    
}

static void _vAnrTnrProc(UINT8 u1StrLvl)
{
    UINT32 u4SubWeiBase;
    UINT32 i, idx;
    INT8 bClarity;

    // 1. get parameter
    _vDrvTnrGetParam();

    bClarity = _SWR(NR_AUTO_CONTROL, NR_AUTO_STRENGTH);

    // 2. calculation
    i = 0;
    idx = LERP_F_C(_rNrTnr[E_NR_LV_MIN].u1Idx, 0, _rNrTnr[E_NR_LV_MAX].u1Idx, 32, u1StrLvl);
    
    //UINT32 u4NewTblTh = (u1EnCurr&_EN_ADAP_WEI) ? LERP(_rNrPQ.u1THD8_hi, _rNrPQ.u1THD8, u4NoiseLvl, 6) : _rNrPQ.u1THD8;

    // hi-noise, small subwei
    u4SubWeiBase = LERP(_rNrTnr[E_NR_LV_MAX].u1SubWei, _rNrTnr[E_NR_LV_MIN].u1SubWei, idx, 5); //(u4SubWeiBase*(12-MIN(u4NoiseLvl,12)) + 6) / 12;

    for (i = 0; i < 8; i++)
    {
        _rNrWriteReg.arMATbl[i] = LERP(_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[i], _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[i], idx, 5);
        _rNrWriteReg.arMACTbl[i] = LERP(_rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[i], _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[i], idx, 5);
        _rNrWriteReg.arMAThd[i] = LERP(_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[i], _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[i], idx, 5);
    }
    idx = idx*(5+bClarity)/5;
    
    _rNrWriteReg.u1MAMaxWei = LERP(_rNrTnr[E_NR_LV_MAX].u1IIRMaxWeight, _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight, idx, 5);        
    _rNrWriteReg.u1MASubWeiBase = u4SubWeiBase;
}

static void _vMoAnr2DProc(UINT8 u1StrLvl)
{
    UINT8 u1MoSnrMax   = _SWR(NR_AUTO_00, SNR_ADP_MAX);
    UINT8 u1MoSnrMin   = _SWR(NR_AUTO_00, SNR_ADP_MIN);
    UINT8 u1MnrEgThMin = _SWR(NR_MO_00, MOMNR_EDGE_TH_MO);
    UINT8 u1MnrEgThMax = _SWR(NR_MO_00, MOMNR_EDGE_TH_ST);
    UINT8 u1CorZeroMax = _SWR(NR_MO_01, MONR_CORING_MO);
    UINT8 u1CorZeroMin = _SWR(NR_MO_01, MONR_CORING_ST);

    _rNrWRegMoAnr.u1SnrGain = LERP(u1MoSnrMax, u1MoSnrMin, u1StrLvl, 4);
    _rNrWRegMoAnr.u1TdsCorZero = LERP(u1CorZeroMax, u1CorZeroMin, u1StrLvl, 4);
    _rNrWRegMoAnr.u1MnrEdgeTh = LERP(u1MnrEgThMin, u1MnrEgThMax, u1StrLvl, 4);
}

static void _vMoAnrProc(UINT32 u4NoiseLevel, UINT32 u4HomoSum)
{
    UINT32 u4MoIdx;
    UINT32 u4TnrStrIdx;
    UINT32 u4SnrStrIdx;

    // Get param
    UINT8 u1MoMax      = _SWR(NR_MO_00, MONR_MOTION_LEVEL_TH2);
    UINT8 u1MoMin      = _SWR(NR_MO_00, MONR_MOTION_LEVEL_TH1);
    UINT8 u1NLThMax    = _SWR(NR_AUTO_02, SNR_ADP_MAX_NL_TH);
    UINT8 u1NLThMin    = _SWR(NR_AUTO_02, SNR_ADP_MIN_NL_TH);

    // 1. motion index, tnr index (3dnr), final index (2d) 
    _rIIRMotion.u1Wei  = _SWR(NR_MO_01, MONR_IIR_FACTOR);
    u4HomoSum = u4HomoSum*_rNrPrm.u1NormFact >> 2;
    u4HomoSum = u4DrvNRUtilIIR(&_rIIRMotion, 8, MIN((u4HomoSum>>8), 0x00FFFFFF));

    u4MoIdx     = LERP_F_C(u1MoMin,   0, u1MoMax,   NR_MOIDX_MAX,          (u4HomoSum >> 8));
    u4TnrStrIdx = LERP_F_C(u1NLThMin, 0, u1NLThMax, (NR_MOIDX_MAX-u4MoIdx), u4NoiseLevel);
    u4SnrStrIdx = LERP_F_C(u1NLThMin, 0, u1NLThMax, u4MoIdx,                u4NoiseLevel);

    // 2. 2D settings
    _vMoAnr2DProc(u4SnrStrIdx);
    // 3. 3D settings
    u4TnrStrIdx = (_SWR(NR_MO_02, MOTNR_BY_NL_EN) == 1) ? u4TnrStrIdx : (NR_MOIDX_MAX-u4MoIdx);
    _vAnrTnrProc(u4TnrStrIdx*2);

    _SWW(NR_MO_STA_01, u4HomoSum, STA_MONR_HOMOSUM);
    _SWW(NR_MO_STA_01, u4MoIdx, STA_MO_IDX);
    _SWW(NR_MO_STA_00, u4TnrStrIdx, STA_FINAL_STR);
}

////////////////////////////////////////////////////////////////////////////////
// ANR Entry
////////////////////////////////////////////////////////////////////////////////
static void _vAnrDftProc(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna)
{
    UINT32 u4NoiseLvl;
    UINT32 u4MaxNL;

    u4MaxNL = _SWR(NR_NM_03, MAX_NOISE_LVL);
    u4NoiseLvl = ((_SWR(NR_NM_00, FORCE_NOISE_LVL_EN)) ? _SWR(NR_NM_00, FORCE_NOISE_LVL) : prNmAna->u1NoiseLevel);
    u4NoiseLvl = MIN(u4NoiseLvl, u4MaxNL);

    if (_rDftNrWRegState[E_NR_DFT_ADAP_NL].fgAutoOn())
    {
        _vAnrNLProc(u4NoiseLvl);
    }

    if (_rDftNrWRegState[E_NR_DFT_ADAP_XNR].fgAutoOn())
    {
        _vAnrDCAlphaProc(u4NoiseLvl);
    }

    // adap IIR table
    if (_rDftNrWRegState[E_NR_DFT_ADAP_TNR].fgAutoOn())
    {
        _vAnrTnrProc(u4NoiseLvl);
    }

    // adap SNR manual threshold
    if (_rDftNrWRegState[E_NR_DFT_ADAP_SNR].fgAutoOn())
    {
        _vAnrSnrProc(u4NoiseLvl);
    }

    if (_rDftNrWRegState[E_NR_DFT_ADAP_RNR].fgAutoOn())
    {
        _vAnrRnrProc(u4NoiseLvl);
    }
}

static void _vAnrMoAnrProc(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna)
{
    UINT32 u4NoiseLvl;
    UINT32 u4MaxNL;

    u4MaxNL = _SWR(NR_NM_03, MAX_NOISE_LVL);
    u4NoiseLvl = ((_SWR(NR_NM_00, FORCE_NOISE_LVL_EN)) ? _SWR(NR_NM_00, FORCE_NOISE_LVL) : prNmAna->u1NoiseLevel);
    u4NoiseLvl = MIN(u4NoiseLvl, u4MaxNL);

    if (_rMoNrWRegState[E_NR_MOANR_NL].fgAutoOn())
    {
        _vAnrNLProc(u4NoiseLvl);
    }

    if (_rMoNrWRegState[E_NR_MOANR_XNR].fgAutoOn())
    {
        _vAnrDCAlphaProc(u4NoiseLvl);
    }

    if (_rMoNrWRegState[E_NR_MOANR_RNR].fgAutoOn())
    {
        _vAnrRnrProc(u4NoiseLvl);
    }

    if (_rMoNrWRegState[E_NR_MOANR_MOANR].fgAutoOn())
    {
        _vMoAnrProc(u4NoiseLvl, prNmSta->u4HomoSum);
    }
}


void NR_FW_ANR_Process(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna)
{
    if (!_fgInit)
        return;
    
    // call module process
    _rAnrModule[_u1CurAnrModuleIdx].vModuleProc(prNmSta, prNmAna);

    if (_EN(EN_LOG_ANR))
        LOG(5, "[ACT] ANR Module (%d): %s\n", _u1CurAnrModuleIdx, _rAnrModule[_u1CurAnrModuleIdx].sDesc);
}

void NR_FW_ANR_WriteReg(void)
{
    NR_ADAP_WREG_BASE_T * prWregSt = NULL;
    UINT8 u1CurrModule = MIN(_SWR(NR_CTRL_01, ANR_SEL), (E_NR_ANR_MODULE_MAX-1));
    UINT8 u1PreModule = MIN(_u1CurAnrModuleIdx,(E_NR_ANR_MODULE_MAX-1));
    UINT8 u1LogOnOff = _EN(EN_LOG_ANR);

    if (!_fgInit)
        return;

    if (u1PreModule != u1CurrModule)
    {
        _vAnrResetModule(_rAnrModule[u1PreModule].parSubFunc);
        _vAnrResetModule(_rAnrModule[u1CurrModule].parSubFunc);
    }
    
    if (u1LogOnOff)
        LOG(7, "[VSYNC] Curr ANR Module (%d): %s\n", u1CurrModule, _rAnrModule[u1CurrModule].sDesc);
   
    prWregSt = _rAnrModule[u1CurrModule].parSubFunc;

    while (prWregSt != NULL && prWregSt->u1State != NR_WR_ST_NULL)
    {
        if (u1LogOnOff)
            LOG(7, "[VSYNC] ANR SubFunc (%d) State: %d\n", u1CurrModule, prWregSt->u1State);
        
        switch (prWregSt->u1State)
        {
        case NR_WR_ST_IDLE:
            if (prWregSt->fgAutoOn())
                prWregSt->u1State = NR_WR_ST_AUTO;
            else if (prWregSt->fgFromSW())
                prWregSt->u1State = NR_WR_ST_FROM_SW;
            break;
        case NR_WR_ST_AUTO:
            prWregSt->vSet();
            if (!prWregSt->fgAutoOn())
                prWregSt->u1State = NR_WR_ST_RESET;
            break;
        case NR_WR_ST_FROM_SW:
            prWregSt->vReset();
            if (prWregSt->fgAutoOn())
                prWregSt->u1State = NR_WR_ST_AUTO;
            else if (!prWregSt->fgFromSW())
                prWregSt->u1State = NR_WR_ST_IDLE;
            break;
        case NR_WR_ST_RESET:
            prWregSt->vReset();
            prWregSt->u1State = NR_WR_ST_IDLE;
            break;
        }
        prWregSt ++;
    } 

    _u1CurAnrModuleIdx = MIN(u1CurrModule,(E_NR_ANR_MODULE_MAX-1));
}


void NR_FW_ANR_Init(void)
{
    _SWW(NR_CTRL_01, 0, ANR_SEL);
    _SWWM(NR_CTRL_02, P_Fld(1, EN_ADAP_WEIGHT_SW)|P_Fld(1, EN_ADAP_SNR_THM_SW)|P_Fld(1, EN_RNR_TH_ADAP_SW));
    _fgInit = 1;
}

