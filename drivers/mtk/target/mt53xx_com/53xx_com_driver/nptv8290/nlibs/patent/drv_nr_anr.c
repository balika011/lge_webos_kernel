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

//#include "vdo_misc.h"
#include "video_def.h"
#include "hw_vdoin.h"
#include "hw_nr.h"
#include "hw_sw.h"

#include "hw_tdsharp.h"
#include "hw_mjc.h"

#include "nr_debug.h"
#include "drv_nr.h"
#include "drv_di_int.h"
#include "drv_bnr_int.h"
#include "drv_nr_int.h"

#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"
#include "drv_meter.h"
#include "source_select.h"

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


static NR_IIR_T _rIIRMotion;

typedef struct
{
    UINT8 u1SnrGain;
    UINT8 u1TdsCorZero;
    UINT8 u1MnrEdgeTh;
    UINT8 u1BNRGain;
    UINT8 u1LPFLevel;
    UINT8 u1BNRVAgst;
    UINT8 u1BNRVLeak;
    UINT16 u2BNRVLinesum;
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
    E_NR_MOANR_SNR   = 2,
    E_NR_MOANR_RNR   = 3,
    E_NR_MOANR_MOANR = 4,
    E_NR_MOANR_FUNC_MAX
} E_NR_MOANR_FUNC;

typedef enum
{
    E_NR_ANR_MODULE_DFT   = 0,
    E_NR_ANR_MODULE_MOANR = 1,
#ifdef DRV_SUPPORT_CUST_ANR    
    E_NR_ANR_MODULE_CUST,
#endif
    E_NR_ANR_MODULE_MAX
} E_NR_ANR_MODULE;


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
    UINT16 u2RnrTh;
    UINT16 u2RnrAggTh;
    UINT16 u2RnrConTh;
} NR_ADAP_WRITE_REG_T;

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
    {fgDrvAnrAutoOnNL,  fgDrvAnrFromSWOff, vDrvAnrSetNL,  vDrvAnrResetNL,  NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnXNR, fgDrvAnrFromSWOff, vDrvAnrSetXNR, vDrvAnrResetXNR, NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnTNR, fgDrvAnrFromSWTNR, vDrvAnrSetTNR, vDrvAnrResetTNR, NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnSNR, fgDrvAnrFromSWSNR, vDrvAnrSetSNR, vDrvAnrResetSNR, NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnRNR, fgDrvAnrFromSWRNR, vDrvAnrSetRNR, vDrvAnrResetRNR, NR_WR_ST_IDLE},
    {NULL, NULL, NULL, NULL, NR_WR_ST_NULL}
};

static NR_ADAP_WREG_BASE_T _rMoNrWRegState[E_NR_MOANR_FUNC_MAX+1] =
{
    {fgDrvAnrAutoOnNL,  fgDrvAnrFromSWOff, vDrvAnrSetNL,  vDrvAnrResetNL,  NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnXNR, fgDrvAnrFromSWOff, vDrvAnrSetXNR, vDrvAnrResetXNR, NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnSNR, fgDrvAnrFromSWSNR, vDrvAnrSetSNR, vDrvAnrResetSNR, NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnRNR, fgDrvAnrFromSWRNR, vDrvAnrSetRNR, vDrvAnrResetRNR, NR_WR_ST_IDLE},
    {fgDrvAnrAutoOnMOANR, fgDrvAnrFromSWMOANR, vDrvAnrSetMOANR, vDrvAnrResetMOANR, NR_WR_ST_IDLE},
    {NULL, NULL, NULL, NULL, NR_WR_ST_NULL}
};

static NR_ADAP_MODULE_T _rAnrModule[E_NR_ANR_MODULE_MAX] = 
{
    {_vAnrDftProc,   _rDftNrWRegState, "Default Adaptive NR"},
    {_vAnrMoAnrProc, _rMoNrWRegState,  "Motion Adaptive NR"},
#ifdef DRV_SUPPORT_CUST_ANR
    {NULL,           NULL,             "Custom Adaptive NR"}
#endif
};

static UINT8 _u1CurAnrModuleIdx = 0;
static BOOL _fgInit = 0;
UINT8 _u1BNRVAgst;
UINT8 _u1BNRVLeak;
UINT16 _u2BNRVLinesum;

////////////////////////////////////////////////////////////////////////////////
// ANR functions
////////////////////////////////////////////////////////////////////////////////
static void _vAnrResetModule(NR_ADAP_WREG_BASE_T* prModule)
{
    if (prModule == NULL)
        return;
    
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

static UINT32 _u4MoANREstRNRAggTh(UINT32 u4NoiseLvl)
{
    UINT32 a = _SWR(NR_NM_05, SNR_THM_GAIN_A);
    UINT32 b = _SWR(NR_NM_05, SNR_THM_GAIN_B);
    UINT32 c = _SWR(NR_NM_06, SNR_THM_OFST_C);
    UINT32 gain = _SWR(NR_PQ_17, RNR_AGG_THM_GAIN);
    UINT32 ofst = _SWR(NR_PQ_17, RNR_AGG_THM_OFST);
    UINT32 u4estThm;
    INT32 estThm;
    
    if (_SWR(NR_CTRL_00, EN_MOTION_ANR) ==1)
    {
        if (b & (1<<19))
            b = -(((~b)&0xfffff)+1);

        if (c & (1<<23))
            c = -(((~c)&0xffffff)+1);

        u4estThm = (UINT32) (a*u4NoiseLvl*u4NoiseLvl + b*u4NoiseLvl + c) * gain + (2048*8);

        if (u4estThm & 0x80000000)
        {
            u4estThm = 0;
        }
        else
        {
            u4estThm = (u4estThm >> (12+4));
        }

        u4estThm = MAX(MIN((u4estThm + ofst), (16*_SWR(NR_PQ_13, RNR_MAX_TH))),0);
        
        return (u4estThm*2); // consider dir_adap, 
    }
    else
    {

        estThm = 
            ((u4NoiseLvl*u4NoiseLvl*u4NoiseLvl) / 16) - ((INT32)32*u4NoiseLvl*u4NoiseLvl / 10) + 81*u4NoiseLvl + 32;

        estThm = MAX(MIN((INT32)estThm, (INT32)(16*_SWR(NR_PQ_13, RNR_MAX_TH))),0);
        return estThm;
    }

    
}

static UINT32 _u4MoANREstRNRConTh(UINT32 u4NoiseLvl)
{
    UINT32 a = _SWR(NR_NM_05, SNR_THM_GAIN_A);
    UINT32 b = _SWR(NR_NM_05, SNR_THM_GAIN_B);
    UINT32 c = _SWR(NR_NM_06, SNR_THM_OFST_C);
    UINT32 gain = _SWR(NR_PQ_18, RNR_CON_THM_GAIN);
    UINT32 ofst = _SWR(NR_PQ_18, RNR_CON_THM_OFST);
    UINT32 estThm;
    
    if (b & (1<<19))
        b = -(((~b)&0xfffff)+1);

    if (c & (1<<23))
        c = -(((~c)&0xffffff)+1);

    estThm = (UINT32) (a*u4NoiseLvl*u4NoiseLvl + b*u4NoiseLvl + c) * gain + (2048*8);

    if (estThm & 0x80000000)
    {
        estThm = 0;
    }
    else
    {
        estThm = (estThm >> (12+4));
    }
    
    estThm = MAX(MIN((estThm + ofst), (16*_SWR(NR_PQ_13, RNR_CON_TH))),0);
    return (estThm*2); // consider dir_adap, 

#if 0
    INT32 estThm = 
        ((u4NoiseLvl*u4NoiseLvl*u4NoiseLvl) / 16) - ((INT32)32*u4NoiseLvl*u4NoiseLvl / 10) + 81*u4NoiseLvl + 32;

    estThm = MAX(MIN((INT32)estThm, (INT32)(16*_SWR(NR_PQ_13, RNR_MAX_TH))),0);
    return estThm;
#endif
    
}


UINT8 u1DrvNXNRGetBlkConf(void)
{
    return (BNR_FW_IsStop() ? NR_R(NXNR_20, NX_STA_VBLK_CONFIDENCE) : NR_R(NXNR_0D, NX_MANUAL_VBLK_CONFIDENCE));
}

static void _vDrvMoNRGetParam(void)
{
     UINT32 u4TrThGain;
     
    UINT32 u4TnrThGain_ST = _SWR(NR_AUTO_00, TNR_THR_GAIN_ST);
    UINT32 u4TnrThGain_MO = _SWR(NR_AUTO_00, TNR_THR_GAIN_MO);
    UINT32 u4TnrIdx = _SWR(NR_MO_STA_00, STA_TNR_STR);
    
    u4TrThGain = LERP(u4TnrThGain_ST, u4TnrThGain_MO, u4TnrIdx, 4);
    _SWW(NR_MO_STA_02, u4TrThGain,STA_TNR_THR_GAIN);

    u4TrThGain = u4TrThGain+0x10;
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[0] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[0]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[1] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[1]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[2] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[2]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[3] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[3]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[4] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[4]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[5] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[5]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[6] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[6]*u4TrThGain)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[7] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[7]*u4TrThGain)>>4, 255);
#if 0
    UINT32 u4TnrTblGain_st;
    UINT32 u4TnrTblGain_mo;
    UINT32 u4TnrThGain_st;
    UINT32 u4TnrThGain_mo = _SWR(NR_MO_04, MONR_TNR_TH_GAIN_MO);
    UINT32 u4noiselevel = _SWR(NR_NMSTA_0E, NR_NM_STA_NL);
    
    UINT8 u1NLThMax    = _SWR(NR_AUTO_02, MONR_MAX_NL_TH);
    UINT8 u1NLThMin    = _SWR(NR_AUTO_02, MONR_MIN_NL_TH);

    //get TNR gain of TBL and TH from NL
    u4TnrTblGain_st = LERP_F_C(u1NLThMin,0x10,u1NLThMax,_SWR(NR_MO_04, MONR_TNR_TBL_GAIN_ST),u4noiselevel);
    u4TnrTblGain_mo = LERP_F_C(u1NLThMin,0x10,u1NLThMax,_SWR(NR_MO_04, MONR_TNR_TBL_GAIN_MO),u4noiselevel);
    u4TnrThGain_st = LERP_F_C(u1NLThMin,0x10,u1NLThMax,_SWR(NR_MO_04, MONR_TNR_TH_GAIN_ST),u4noiselevel);
    u4TnrThGain_mo = LERP_F_C(u1NLThMin,0x10,u1NLThMax,_SWR(NR_MO_04, MONR_TNR_TH_GAIN_MO),u4noiselevel);

    //get motion TNR table gain from motion and NL index
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[0] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[0]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[1] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[1]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[2] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[2]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[3] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[3]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[4] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[4]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[5] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[5]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[6] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[6]*u4TnrTblGain_mo)>>4, 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[7] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[7]*u4TnrTblGain_mo)>>4, 63);

    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[0] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[0]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[1] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[1]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[2] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[2]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[3] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[3]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[4] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[4]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[5] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[5]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[6] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[6]*u4TnrTblGain_st)>>4, 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[7] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[7]*u4TnrTblGain_st)>>4, 63);

    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[0] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[0]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[1] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[1]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[2] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[2]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[3] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[3]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[4] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[4]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[5] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[5]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[6] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[6]*u4TnrThGain_mo)>>4, 255);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[7] = MIN((_rNrTnr[E_NR_LV_MIN].u1MAWeiThd[7]*u4TnrThGain_mo)>>4, 255);

    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[0] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[0]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[1] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[1]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[2] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[2]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[3] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[3]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[4] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[4]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[5] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[5]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[6] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[6]*u4TnrThGain_st)>>4, 255);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[7] = MIN((_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[7]*u4TnrThGain_st)>>4, 255);
#endif
    
}

static void _vDrvTnrGetParam(void)
{
    UINT32 u4PQ00;
    UINT32 u4PQ01;
    UINT32 u4PQ03;
    UINT32 u4PQ04;
    UINT32 u4PQ05;

    UINT32 tmp;
    
	UINT8 AutoMode = _SWR(NR_PQ_1B, NR_AUTO_MODE);
	
	// get adaptive table from flash PQ
	switch (AutoMode)
	{
        default:
        case 0:
    u4PQ00 = _SWR_4B(NR_PQ_00);
    u4PQ01 = _SWR_4B(NR_PQ_01);
	u4PQ03 = _SWR_4B(NR_PQ_03);
    u4PQ04 = _SWR_4B(NR_PQ_04);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ00, NR_TBL_MA_0), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ00, NR_TBL_MA_1), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ00, NR_TBL_MA_2), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ00, NR_TBL_MA_3), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ00, NR_TBL_MA_4), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ01, NR_TBL_MA_5), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ01, NR_TBL_MA_6), 63);
    _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ01, NR_TBL_MA_7), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_0), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_1), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_2), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ03, NR_TBL_MA_E_3), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_4), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_5), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_6), 63);
    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ04, NR_TBL_MA_E_7), 63); 
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
            break;
        case 1:
            u4PQ00 = _SWR_4B(NR_PQ_1A);
    		u4PQ01 = _SWR_4B(NR_PQ_1B);
			u4PQ03 = _SWR_4B(NR_PQ_1C);
    		u4PQ04 = _SWR_4B(NR_PQ_1D);
            _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO1_MIN_0), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO1_MIN_1), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO1_MIN_2), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO1_MIN_3), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO1_MIN_4), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ01, NR_TBL_MA_AUTO1_MIN_5), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ01, NR_TBL_MA_AUTO1_MIN_6), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ01, NR_TBL_MA_AUTO1_MIN_7), 63);
			_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO1_MAX_0), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO1_MAX_1), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO1_MAX_2), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO1_MAX_3), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO1_MAX_4), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO1_MAX_5), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO1_MAX_6), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO1_MAX_7), 63); 
			tmp = _SWR_4B(NR_PQ_22);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[0] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_1);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[1] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_2);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[2] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_3);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[3] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_4);
		    tmp = _SWR_4B(NR_PQ_23);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[4] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_5);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[5] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_6);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[6] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_7);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[7] = R_Fld(tmp, NR_TBL_THD_AUTO1_MIN_8);
		    tmp = _SWR_4B(NR_PQ_24);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[0] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_1);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[1] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_2);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[2] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_3);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[3] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_4);
		    tmp = _SWR_4B(NR_PQ_25);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[4] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_5);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[5] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_6);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[6] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_7);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[7] = R_Fld(tmp, NR_TBL_THD_AUTO1_MAX_8);	
            break;
        case 2: // default: MC strong, SNR weak
            u4PQ00 = _SWR_4B(NR_PQ_1E);
    		u4PQ01 = _SWR_4B(NR_PQ_1F);
			u4PQ03 = _SWR_4B(NR_PQ_20);
    		u4PQ04 = _SWR_4B(NR_PQ_21);
            _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO2_MIN_0), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO2_MIN_1), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO2_MIN_2), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO2_MIN_3), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ00, NR_TBL_MA_AUTO2_MIN_4), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ01, NR_TBL_MA_AUTO2_MIN_5), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ01, NR_TBL_MA_AUTO2_MIN_6), 63);
    		_rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ01, NR_TBL_MA_AUTO2_MIN_7), 63);
			_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[0] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO2_MAX_0), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[1] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO2_MAX_1), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[2] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO2_MAX_2), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[3] = MIN(R_Fld(u4PQ03, NR_TBL_MA_AUTO2_MAX_3), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[4] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO2_MAX_4), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[5] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO2_MAX_5), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[6] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO2_MAX_6), 63);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[7] = MIN(R_Fld(u4PQ04, NR_TBL_MA_AUTO2_MAX_7), 63); 
			tmp = _SWR_4B(NR_PQ_26);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[0] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_1);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[1] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_2);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[2] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_3);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[3] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_4);
		    tmp = _SWR_4B(NR_PQ_27);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[4] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_5);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[5] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_6);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[6] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_7);
		    _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[7] = R_Fld(tmp, NR_TBL_THD_AUTO2_MIN_8);
		    tmp = _SWR_4B(NR_PQ_28);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[0] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_1);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[1] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_2);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[2] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_3);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[3] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_4);
		    tmp = _SWR_4B(NR_PQ_29);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[4] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_5);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[5] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_6);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[6] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_7);
		    _rNrTnr[E_NR_LV_MAX].u1MAWeiThd[7] = R_Fld(tmp, NR_TBL_THD_AUTO2_MAX_8);
            break;
	}
    
    
    u4PQ05 = _SWR_4B(NR_PQ_05);

    
    _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight = R_Fld(u4PQ05, NR_IIR_MAX_WEI);
    _rNrTnr[E_NR_LV_MIN].u1SubWei = _SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_BASE_DFT);
    _rNrTnr[E_NR_LV_MAX].u1IIRMaxWeight = R_Fld(u4PQ05, NR_IIR_MAX_WEI_E);
    _rNrTnr[E_NR_LV_MAX].u1SubWei = _SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_BASE_HI);
    //_rNrTnr[E_NR_LV_MAX].u1SubWei = 0;

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

BOOL fgDrvAnrFromSWOff(void)
{
    return FALSE;
}

// Noise Level
BOOL fgDrvAnrAutoOnNL(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_NOISELEVEL);
}

void vDrvAnrSetNL(void)
{
    NR_W(MCNR_17, _rNrWriteReg.u1NoiseLevel, MCNR_NOISE_LEVEL);
}

void vDrvAnrResetNL(void)
{
    NR_W(MCNR_17, 4, MCNR_NOISE_LEVEL);
}

// DC Alpha
BOOL fgDrvAnrAutoOnXNR(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_AUTO_XNR);
}

void vDrvAnrSetXNR(void)
{
    NR_W(MCNR_01, _rNrWriteReg.u1DcAlpha, MCNR_DCNR_ALPHA);
}

void vDrvAnrResetXNR(void)
{
    vDrvMCNRSetDftDcnrAlpha();
}

// SNR
BOOL fgDrvAnrAutoOnSNR(void)
{
    return _EN(EN_ADAP_NR) && (_EN(EN_ADAP_SNR_THM) || _EN(EN_ADAP_SNR_BLK));
}

BOOL fgDrvAnrFromSWSNR(void)
{
    return _SWR(NR_CTRL_02, EN_ADAP_SNR_THM_SW);
}

void vDrvAnrSetSNR(void)
{
    NR_WM(NXNR_11,
        P_Fld(_rNrWriteReg.u2SnrThm, NX_SNR_MANUAL_TH)|
        P_Fld(_rNrWriteReg.u1SnrK, NX_SNR_AGGRESSIVE_LV)|
        P_Fld(_rNrWriteReg.u1SnrBeta, NX_SNR_MA_RATIO));
    
    //NR_W(NXNR_04, _rNrPQ.u1SnrGain, NX_MAX_SNR_RATIO);
    if (_SWR(NR_CTRL_01, ANR_SEL) ==0)
    {
        NR_W(NXNR_04, _rNrWriteReg.u1SnrGain, NX_SNR_GAIN_Y);
    }
}

void vDrvAnrResetSNR(void)
{
    UINT32 u4SnrThm, u4K, u4ManualAutoRto, u4SnrGain;

    // 1. read SW reg
    _vDrvNXNRSelThm();
    _vDrvSnrGetParam();

    u4SnrThm = _u4EstSNRManualTh(0);
    u4K = MIN(15, _rSnrTh.u1GeneralStr);
    u4ManualAutoRto = _rSnrTh.u1MARto;
    u4SnrGain = _SWR(NR_PQ_09, NR_SNR_GAIN); //_rNrSnr[E_NR_LV_MIN].u1SnrGain;    

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
    if (_SWR(NR_CTRL_01, ANR_SEL)==0)
    {
        NR_W(NXNR_04, u4SnrGain, NX_SNR_GAIN_Y);
    }
}

// TNR
BOOL fgDrvAnrAutoOnTNR(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_ADAP_WEIGHT);
}

BOOL fgDrvAnrFromSWTNR(void)
{
    return _SWR(NR_CTRL_02, EN_ADAP_WEIGHT_SW);
}

void vDrvAnrSetTNR(void)
{
    NR_W(MCNR_08, _rNrWriteReg.u1MAMaxWei, MCNR_IIR_MAX_WEI);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_MA, _rNrWriteReg.arMATbl);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_C,  _rNrWriteReg.arMACTbl);
    vDrvMCNRSetTblThd(_rNrWriteReg.arMAThd);

    NR_WM(MCNR_03,
        P_Fld(_rNrWriteReg.u1MASubWeiBase, MCNR_MOVINGTXR_SUB_WEI_BASE)|
        P_Fld(_SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN_DFT), MCNR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN));
}

void vDrvAnrResetTNR(void)
{
    // 1. read SW reg
    _vDrvTnrGetParam();

    // 2. reset HW reg
    NR_W(MCNR_08, _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight, MCNR_IIR_MAX_WEI);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_MA, _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl);
    vDrvMCNRSetWeiTbl(MCNR_IIR_TBL_C,  _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl);
    vDrvMCNRSetTblThd(_rNrTnr[E_NR_LV_MIN].u1MAWeiThd);

    NR_WM(MCNR_03,
        P_Fld(_rNrTnr[E_NR_LV_MIN].u1SubWei, MCNR_MOVINGTXR_SUB_WEI_BASE)|
        P_Fld(_SWR(NR_PQ_08, NR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN_DFT), MCNR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN));
}

// RNR
BOOL fgDrvAnrAutoOnRNR(void)
{
    return _EN(EN_ADAP_NR) && (_EN(EN_RNR_TH_ADAP)|| _EN(EN_BNR_RNR_ADAP));
}

BOOL fgDrvAnrFromSWRNR(void)
{
    return _SWR(NR_CTRL_02, EN_RNR_TH_ADAP_SW);
}

void vDrvAnrSetRNR(void)
{  
    NR_W(NXNR_3E, _rNrWriteReg.u2RnrTh, NX_FORCE_RNR_TH_AGG);
}

void vDrvAnrResetRNR(void)
{
    NR_W(NXNR_3E, _SWR(NR_PQ_13, RNR_MAX_TH)*16, NX_FORCE_RNR_TH_AGG);
}


// MONR
BOOL fgDrvAnrAutoOnMOANR(void)
{
    return _EN(EN_ADAP_NR) && _EN(EN_MOTION_ANR);
}

BOOL fgDrvAnrFromSWMOANR(void)
{
    return _SWR(NR_CTRL_02, EN_MOTION_ANR_SW);
}

void vDrvAnrSetMOANR(void)
{
    vDrvAnrSetTNR();

    NR_WM(NXNR_11,
        P_Fld(_rNrWriteReg.u2SnrThm, NX_SNR_MANUAL_TH)|
        P_Fld(_rNrWriteReg.u1SnrK, NX_SNR_AGGRESSIVE_LV)|
        P_Fld(_rNrWriteReg.u1SnrBeta, NX_SNR_MA_RATIO));

    NR_W(NXNR_04, _rNrWRegMoAnr.u1SnrGain, NX_SNR_GAIN_Y);
    NR_W(NXNR_0E, _rNrWRegMoAnr.u1MnrEdgeTh, NX_MNR_EDGE_TH);

    //NR_W(TDSHP_27, _rNrWRegMoAnr.u1TdsCorZero, TDS_COR_ZERO);
    NR_W(CDS_04, _rNrWRegMoAnr.u1LPFLevel, CDS1_LPF_STRENGTH);

}

void vDrvAnrResetMOANR(void)
{
    UINT32 u4SnrThm, u4K, u4ManualAutoRto;

    // 1. read SW reg
    _vDrvNXNRSelThm();
    _vDrvSnrGetParam();

    u4SnrThm = _u4EstSNRManualTh(0);
    u4K = MIN(15, _rSnrTh.u1GeneralStr);
    u4ManualAutoRto = _rSnrTh.u1MARto;

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

    vDrvAnrResetTNR();

    NR_W(NXNR_04, _SWR(NR_AUTO_00, SNR_ADP_MIN), NX_SNR_GAIN_Y);
    NR_W(NXNR_0E, _rNrWRegMoAnr.u1MnrEdgeTh, NX_MNR_EDGE_TH);
    NR_W(CDS_58, _rNrWRegMoAnr.u1LPFLevel, CDS3_LPF_STRENGTH);
}


////////////////////////////////////////////////////////////////////////////////
// ANR Processing
////////////////////////////////////////////////////////////////////////////////
extern UINT16 _u2MVY[];
extern UINT16 _u2MVX[];
UINT32 _u4MVXMotion =0;
UINT32 _u4MVYMotion =0;
UINT32 _u4MVMotion =0;


void _vANRGetMotionFromMV(void)
{
    UINT8 i;
    UINT32 u4MVXStillCnt = IO32ReadFldAlign(MJC_STA_MH_14 , MJC_MV_X2_GRM4);
    UINT32 u4MVYStillCnt = IO32ReadFldAlign(MJC_STA_MH_18 , MJC_MV_Y2_GRM4);
    UINT32 u4MVMotionCnt1, u4MVMotionCnt2;
    
    _u4MVMotion =0;
    _u4MVXMotion =0;
    _u4MVYMotion =0;
      
   for (i=0 ; i<10; i++)
   {
        _u4MVXMotion += _u2MVX[i]*i;
        _u4MVMotion += _u2MVX[i]*i;
   }
    for (i=0 ; i<8; i++)
   {
        _u4MVYMotion += _u2MVY[i]*i;
        _u4MVMotion += _u2MVY[i]*i;
   }

   u4MVMotionCnt1 =(0x7E90<<1) - u4MVXStillCnt-u4MVYStillCnt;
   u4MVMotionCnt2 = (0x7E90- IO32ReadFldAlign(MJC_STA_MH_1A , MJC_STA_MV_ZERO))>>8;
   UNUSED(u4MVMotionCnt2);
    _SWW(NR_MO_STA_02, MIN((_u4MVMotion>>4) , 0xfff),STA_MO_MV_SUM);
    _SWW(NR_MO_STA_03, MIN((_u4MVXMotion>>4),0xfff),STA_MO_MVX_SUM);
    _SWW(NR_MO_STA_03, MIN((_u4MVYMotion>>4),0xfff),STA_MO_MVY_SUM);
    _SWW(NR_MO_STA_02, (u4MVMotionCnt1>>4),STA_MO_MV_SUM_1);
    //_SWW(NR_MO_STA_04, u4MVMotionCnt2, STA_MO_MV_SUM_2);
    
}
void vDrvAnrNLProc(UINT8 u1StrLvl)
{
    _rNrWriteReg.u1NoiseLevel = (UINT8) u1StrLvl;
}

void vDrvAnrDCAlphaProc(UINT8 u1StrLvl)
{
    UINT32 u4DcAlpha = LERP_F_C(20, 4, 32, 12, u1StrLvl);
        //((MIN(MAX(u1StrLvl, 20), 32) - 20) * 16) / (32-20);
    _rNrWriteReg.u1DcAlpha = (UINT8) u4DcAlpha;
    //_rNrFlfbSta.u1GmvRatio = NR_R(MCNR_18, MCNR_GMV_RATIO);    
    //INT32 gmv_de_dc_alpha = MAX( -6, MIN ( 4, (INT32) 22 - _rNmSta.u1GmvRatio ));   
    //UINT8 dc_alpha =  MAX( 0, MIN( 8, ( (INT32)u4NoiseLvl - 5 * 4 ) * 2 / 4 ));
    //NR_W(MCNR_01, dc_alpha, MCNR_DCNR_ALPHA);
}


void vDrvAnrRnrProc(UINT8 u1StrLvl)
{
    UINT32 u4RnrTh =_u4EstRNRAggTh(u1StrLvl);
    
    if (_EN(EN_BNR_RNR_ADAP))
        u4RnrTh = LERP(16*_SWR(NR_PQ_13, BNR_RNR_TH), u4RnrTh, u1DrvNXNRGetBlkConf(), 4);
    
    _rNrWriteReg.u2RnrTh = u4RnrTh;
}

void vDrvMoRnrProc(UINT8 u1StrLvl)
{
    UINT32 u4RnrAggTh;
    UINT32 u4RnrConTh;
    UINT32 u4K;

    // 1. get parameter
    _vDrvNXNRSelThm();
    _vDrvSnrGetParam();

    u4RnrAggTh = _EN(EN_RNR_TH_ADAP) ? _u4MoANREstRNRAggTh(u1StrLvl) : 16*_SWR(NR_PQ_13, RNR_MAX_TH);
    u4RnrConTh = _EN(EN_RNR_TH_ADAP) ? _u4MoANREstRNRConTh(u1StrLvl) : 16*_SWR(NR_PQ_13, RNR_CON_TH);
    u4K = MIN(15, _rSnrTh.u1GeneralStr);
    
    if (_rSnrTh.u1CostDirAdap)
    {
        u4RnrAggTh = (u4RnrAggTh + 1) >> 1;
        u4RnrConTh = (u4RnrConTh + 1) >> 1;
        u4K = (u4K + 1) >> 1;
    }
    
    if (_EN(EN_BNR_RNR_ADAP))
        u4RnrAggTh = LERP(16*_SWR(NR_PQ_13, BNR_RNR_TH), u4RnrAggTh, u1DrvNXNRGetBlkConf(), 4);
    
    _rNrWriteReg.u2RnrAggTh = u4RnrAggTh;
    _rNrWriteReg.u2RnrConTh = u4RnrConTh;
    _rNrWriteReg.u1SnrK = u4K;
}

void vDrvAnrSnrProc(UINT8 u1StrLvl)
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
    u4ManualAutoRto = _rSnrTh.u1MARto; //LERP(0, _rSnrTh.u1MARto, MIN(idx, 32), 5);
    
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

void vDrvAnrTnrProc(UINT8 u1StrLvl)
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
        _rNrWriteReg.arMATbl[i]  = LERP(_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[i], _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[i], idx, 5);
        _rNrWriteReg.arMACTbl[i] = LERP(_rNrTnr[E_NR_LV_MAX].u1MACWeiTbl[i], _rNrTnr[E_NR_LV_MIN].u1MACWeiTbl[i], idx, 5);
        _rNrWriteReg.arMAThd[i]  = LERP(_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[i], _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[i], idx, 5);
    }
    idx = idx*(5+bClarity)/5;
    
    _rNrWriteReg.u1MAMaxWei = LERP(_rNrTnr[E_NR_LV_MAX].u1IIRMaxWeight, _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight, idx, 5);        
    _rNrWriteReg.u1MASubWeiBase = u4SubWeiBase;
}

static void _vMoTnrProc(UINT8 u1TnrStr, UINT8 u1NoiseLvl)
{
    UINT32 u4SubWeiBase;
    UINT32 i,idx;
    UINT32 u4gain;
    INT8 bClarity;
    
    // 1. get parameter
    _vDrvTnrGetParam();

    //if MONR is ON, modify TNR gain/threshold by motion index
    _vDrvMoNRGetParam();    

    bClarity = _SWR(NR_AUTO_CONTROL, NR_AUTO_STRENGTH);

    // 2. calculation
    //i = 0;
    idx = LERP_F_C(_rNrTnr[E_NR_LV_MIN].u1Idx, 0, _rNrTnr[E_NR_LV_MAX].u1Idx, 32, u1NoiseLvl);
    
    //UINT32 u4NewTblTh = (u1EnCurr&_EN_ADAP_WEI) ? LERP(_rNrPQ.u1THD8_hi, _rNrPQ.u1THD8, u4NoiseLvl, 6) : _rNrPQ.u1THD8;

    // hi-noise, small subwei
    if (_SWR(NR_MO_05, MONR_SUB_WEI_EN))
    {
    u4SubWeiBase = LERP(_rNrTnr[E_NR_LV_MAX].u1SubWei, _rNrTnr[E_NR_LV_MIN].u1SubWei, u1NoiseLvl, 5); //(u4SubWeiBase*(12-MIN(u4NoiseLvl,12)) + 6) / 12;
    }
    else
    {
        u4SubWeiBase = _rNrTnr[E_NR_LV_MIN].u1SubWei;
    }

#if (!defined(CC_MT5890)) 
    u4Highfreq = IO32ReadFldAlign(TDSHP_32 , TDSHP_HISTOGRAM_BIN1) >>16;
    u4HighFreqGain = _SWR(NR_MO_04, MONR_HIGH_FREQ_GAIN);

    //decrease TNR when there is a lot of high freq 
    if (u4Highfreq >= u1HighFreqTH2)
    {
        u4gain =u4HighFreqGain;
    }
    else if (u4Highfreq > u1HighFreqTH1)
    {
        u4gain = ((u4Highfreq - u1HighFreqTH1)*u4HighFreqGain  + (u1HighFreqTH2-u4Highfreq)*0x10)/(u1HighFreqTH2-u1HighFreqTH1);
    }
    else
    {
        u4gain = 0x10;
    }
    
    // only apply at motion image
    u4gain = (0x10*( NR_MOIDX_MAX - _SWR(NR_MO_STA_01,  STA_MO_IDX)) + (u4gain * _SWR(NR_MO_STA_01,  STA_MO_IDX)))/NR_MOIDX_MAX;
#else
    u4gain =0x10;
#endif
    
    for (i = 0; i < 8; i++)
    {
        _rNrWriteReg.arMATbl[i] = LERP(_rNrTnr[E_NR_LV_MAX].u1MAWeiTbl[i], _rNrTnr[E_NR_LV_MIN].u1MAWeiTbl[i], u1TnrStr, 5);
        _rNrWriteReg.arMAThd[i] = LERP(_rNrTnr[E_NR_LV_MAX].u1MAWeiThd[i], _rNrTnr[E_NR_LV_MIN].u1MAWeiThd[i], u1NoiseLvl, 5);

        if (_SWR(NR_MO_05, MONR_HIGH_FREQ_EN))
        {
            _rNrWriteReg.arMATbl[i] =(_rNrWriteReg.arMATbl[i]*u4gain) >> 4;
        }
    }
    idx = idx*(5+bClarity)/5;
    
    _rNrWriteReg.u1MAMaxWei = LERP(_rNrTnr[E_NR_LV_MAX].u1IIRMaxWeight, _rNrTnr[E_NR_LV_MIN].u1IIRMaxWeight, idx, 5);        
    _rNrWriteReg.u1MASubWeiBase = u4SubWeiBase;

    if (_SWR(NR_CTRL_00,EN_CLEAN_DF_DCR))
    {
        UINT32 u4Tmp = _SWR_4B(NR_PQ_07);
        _rNrWriteReg.arMATbl[4] = (R_Fld(u4Tmp, NR_TBL_MA_4_CDL) > _rNrWriteReg.arMATbl[4]) ?  
                LERP(R_Fld(u4Tmp, NR_TBL_MA_4_CDL), _rNrWriteReg.arMATbl[4], _rNmAna.u1CleanDiffLvl, 3) :_rNrWriteReg.arMATbl[4];
        _rNrWriteReg.arMATbl[5] = (R_Fld(u4Tmp, NR_TBL_MA_5_CDL) > _rNrWriteReg.arMATbl[5]) ?  
                LERP(R_Fld(u4Tmp, NR_TBL_MA_5_CDL), _rNrWriteReg.arMATbl[5], _rNmAna.u1CleanDiffLvl, 3) :_rNrWriteReg.arMATbl[5];        
        _rNrWriteReg.arMATbl[6] = (R_Fld(u4Tmp, NR_TBL_MA_6_CDL) > _rNrWriteReg.arMATbl[6]) ?  
                LERP(R_Fld(u4Tmp, NR_TBL_MA_6_CDL), _rNrWriteReg.arMATbl[6], _rNmAna.u1CleanDiffLvl, 3) :_rNrWriteReg.arMATbl[6];
        _rNrWriteReg.arMATbl[7] = (R_Fld(u4Tmp, NR_TBL_MA_7_CDL) > _rNrWriteReg.arMATbl[7]) ?  
                LERP(R_Fld(u4Tmp, NR_TBL_MA_7_CDL), _rNrWriteReg.arMATbl[7], _rNmAna.u1CleanDiffLvl, 3) :_rNrWriteReg.arMATbl[7];
               
        _rNrWriteReg.arMAThd[7] = LERP(R_Fld(u4Tmp, MCNR_TBL_TH_CDL), _rNrWriteReg.arMAThd[7], _rNmAna.u1CleanDiffLvl, 3);
        _rNrWriteReg.u1MASubWeiBase = (((UINT32) _rNrWriteReg.u1MASubWeiBase * (8-_rNmAna.u1CleanDiffLvl)) + 4) >> 3;
        _rNrWriteReg.u1MASubWeiNLGain = (((UINT32) _rNrWriteReg.u1MASubWeiNLGain * (8-_rNmAna.u1CleanDiffLvl)) + 4) >> 3;


    }



    
}

void vDrvAnrMoAnr2DProc(UINT8 u1StrLvl , UINT8 u1NoiseLvl, UINT8 u4TVD_NL)
{
    UINT32 u4SmMoidx;
    UINT32 u4OriNL= _SWR(NR_NMSTA_0E, NR_NM_STA_NL);
    UINT8 u1MoSnrMax   = _SWR(NR_AUTO_00, SNR_ADP_MAX);
    UINT8 u1MoSnrMin   = _SWR(NR_AUTO_00, SNR_ADP_MIN);
    UINT8 u1MnrEgThMin = _SWR(NR_MO_00, MOMNR_EDGE_TH_MO);
    UINT8 u1MnrEgThMax = _SWR(NR_MO_00, MOMNR_EDGE_TH_ST);
    UINT8 u1CorZeroMax = _SWR(NR_MO_01, MONR_CORING_MO);
    UINT8 u1CorZeroMin = _SWR(NR_MO_01, MONR_CORING_ST);
    //UINT8 u1MoBNRMax = _SWR(NR_AUTO_04, BNR_MAX_GAIN);

    UINT8 u1LPFMin   = _SWR(NR_AUTO_03, LPF_LEVEL_MIN);
    UINT8 u1LPFMax   = _SWR(NR_AUTO_03, LPF_LEVEL_MAX);

    UINT8 u1BNRMin   = _SWR(NR_AUTO_04, BNR_STILL_GAIN);
    UINT8 u1BNRMax   = _SWR(NR_AUTO_04, BNR_MAX_GAIN);

    //UINT8 u1BNR_VAgaist_st =  _SWR(NR_MO_04, MOBNR_VAGAINST_ST);
    UINT8 u1BNR_VLeak_st =  _SWR(NR_MO_04, MOBNR_VLEAKAGE_ST);
    UINT16 u2BNR_VLinesumTh_st =_SWR(NR_MO_05, MOBNR_VLINESUMTH_ST);

    _SWW(NR_MO_STA_00, u1StrLvl, STA_2DNR_STR);
    
    _rNrWRegMoAnr.u1SnrGain = LERP(u1MoSnrMax, u1MoSnrMin, u1StrLvl, 4);
    _rNrWRegMoAnr.u1TdsCorZero = LERP(u1CorZeroMax, u1CorZeroMin, u1StrLvl, 4);
    _rNrWRegMoAnr.u1MnrEdgeTh = LERP(u1MnrEgThMin, u1MnrEgThMax, u1StrLvl, 4);
    
    //LFP only refer to noise level not motion idx
    if (_SWR(NR_NM_07,NR_NM_LPF_TVD) && (bGetVideoDecTypeNew(SV_VP_MAIN) == SV_VD_TVD3D))
    {
        _rNrWRegMoAnr.u1LPFLevel =IO32ReadFldAlign(SHARP_00,TDS_BLUR) ? LERP(u1LPFMax, u1LPFMin, u4TVD_NL, 5) : u1LPFMin;
    }
    else
    {
        _rNrWRegMoAnr.u1LPFLevel =IO32ReadFldAlign(SHARP_00,TDS_BLUR) ? LERP(u1LPFMax, u1LPFMin, u1NoiseLvl, 5) : u1LPFMin;
    }    

   // _rNrWRegMoAnr.u1LPFLevel = LERP(u1LPFMax, u1LPFMin, u1StrLvl, 4);
    _rNrWRegMoAnr.u1BNRGain = LERP(u1BNRMax, u1BNRMin, u1StrLvl, 4);
    
    

    //patch for SNR gain
    if ((u4OriNL <3) && (_SWR(NR_AUTO_02,SNR_GAIN_PATCH_EN)))
    {
        //_rNrWRegMoAnr.u1SnrGain = MIN((u4OriNL>>1),_rNrWRegMoAnr.u1SnrGain);
        _rNrWRegMoAnr.u1SnrGain =0;
    }
    if (_SWR(NR_AUTO_02,BNR_AGAINST_PATCH_EN))
    {
        #ifndef BNR_NEW_FW
        if (_rNrWRegMoAnr.u1BNRVLeak!=_SWR(NR_FWBNR_01,NR_FWBNR_VBLKLEVELLEAKAGE))
        {
            _u1BNRVLeak= _SWR(NR_FWBNR_01,NR_FWBNR_VBLKLEVELLEAKAGE);
        }

        if (_rNrWRegMoAnr.u2BNRVLinesum!=_SWR(NR_FWBNR_00,NR_FWBNR_VGRIDMINLINESUMTH))
        {
            _u2BNRVLinesum = _SWR(NR_FWBNR_00,NR_FWBNR_VGRIDMINLINESUMTH);
        }
        #endif
        u4SmMoidx = LERP_F_C((1<<4), 0, (1<<7), 16,_SWR(NR_MO_STA_01, STA_MONR_HOMOSUM));
        _rNrWRegMoAnr.u1BNRVLeak= LERP(_u1BNRVLeak, u1BNR_VLeak_st, u4SmMoidx, 4);
        //_rNrWRegMoAnr.u1BNRVAgst= LERP(_u1BNRVAgst, u1BNR_VAgaist_st, u4SmMoidx, 4);
        _rNrWRegMoAnr.u2BNRVLinesum= LERP(_u2BNRVLinesum, u2BNR_VLinesumTh_st, u4SmMoidx, 4);
    }
}

void vDrvAnrMoAnrProc(UINT32 u4NoiseLevel, UINT32 u4HomoSum)
{
   UINT32 u4MoIdx;
    UINT32 u4TnrStrIdx;
    UINT32 u4SnrStrIdx;
    UINT32 u4MVMotionCnt;
    UINT32 u4MotionLvl;
    UINT32 u4FinalMotionLvl;
    UINT32 u4Gain;
    UINT32 u4TVD_NL;

    // Get param
    UINT8 u1MoMax      = _SWR(NR_MO_00, MONR_MOTION_LEVEL_TH2);
    UINT8 u1MoMin      = _SWR(NR_MO_00, MONR_MOTION_LEVEL_TH1);
    UINT8 u1TNRNLThMax    = _SWR(NR_AUTO_02, MOTNR_MAX_NL_TH);
    UINT8 u1TNRNLThMin    = _SWR(NR_AUTO_02, MOTNR_MIN_NL_TH);
    UINT8 u1SNRNLThMax    = _SWR(NR_MO_04, MOSNR_MAX_NL_TH);
    UINT8 u1SNRNLThMin    = _SWR(NR_MO_04, MOSNR_MIN_NL_TH);
    UINT8 u1APLTH1 = _SWR(NR_MO_03, MONR_APL_TH1);
    UINT8 u1APLTH2 = _SWR(NR_MO_03, MONR_APL_TH2);
    UINT8 u1APLGain =_SWR(NR_MO_03, MONR_APL_GAIN);
    UINT8 u1APL = bDrvGetAPL();
    
    _vANRGetMotionFromMV();

    // 1. motion index, tnr index (3dnr), final index (2d) 
    _rIIRMotion.u1Wei  = _SWR(NR_MO_01, MONR_IIR_FACTOR);
    _rIIRMotion.u1Wei_dec  = _SWR(NR_MO_02, MONR_IIR_FACTOR_DEC);
    u4HomoSum = u4HomoSum*_rNrPrm.u1NormFact >> 2;
    u4HomoSum = u4DrvNRUtilIIR_1(&_rIIRMotion, 8, MIN((u4HomoSum>>8), 0x00FFFFFF));

    _SWW(NR_MO_STA_01, (u4HomoSum>>8), STA_MONR_HOMOSUM);
    //modify homosum by APL
    if (u1APL<= u1APLTH1)
    {
        u4HomoSum = (u4HomoSum*(u1APLGain+0x10))>>4,0xFF;
    }
    else if (u1APL< u1APLTH2)
    {
        if (u1APLTH2 > u1APLTH1)
        {
            u4Gain = ((u1APL-u1APLTH1)+ (((u1APLTH2-u1APL)*(u1APLGain+0x10))>>4))/(u1APLTH2-u1APLTH1);
            u4HomoSum = u4HomoSum*u4Gain;
        }
    }

    u4HomoSum = MIN(0xFF,(u4HomoSum>>8));
    u4MVMotionCnt = MIN(0x40, (0x7E90- MIN(0x7E90,IO32ReadFldAlign(MJC_STA_MH_1A , MJC_STA_MV_ZERO))+0x40)>>8);

    u4FinalMotionLvl = (u4MVMotionCnt*MIN(8,_SWR(NR_MO_03, MONR_MOTION_MV_WEI)) + 
                        MIN(0x40,u4HomoSum)*(8-MIN(8,_SWR(NR_MO_03, MONR_MOTION_MV_WEI))))>>3;
    
    u4MotionLvl= _SWR(NR_MO_02, MONR_MOTION_FROM_MV) ? u4FinalMotionLvl : u4HomoSum;

    
    
    u4MoIdx     = LERP_F_C(u1MoMin,   0, u1MoMax,   NR_MOIDX_MAX, u4MotionLvl);
    u4TnrStrIdx = LERP_F_C(u1TNRNLThMin, 0, u1TNRNLThMax, (NR_MOIDX_MAX-u4MoIdx), u4NoiseLevel);
    u4SnrStrIdx = LERP_F_C(u1SNRNLThMin, 0, u1SNRNLThMax, u4MoIdx,                u4NoiseLevel);
    u4NoiseLevel = LERP_F_C(u1TNRNLThMin, 0, u1TNRNLThMax, NR_NLIDX_MAX,                u4NoiseLevel);
    u4TVD_NL = LERP_F_C(u1TNRNLThMin, 0, u1TNRNLThMax, NR_NLIDX_MAX,                _rNmAna.u1TVD_NL);

    // 2. 2D settings
    vDrvAnrMoAnr2DProc(u4SnrStrIdx,u4NoiseLevel, u4TVD_NL);
    // 3. 3D settings
    //u4TnrStrIdx = (_SWR(NR_MO_02, MOTNR_BY_NL_EN) == 1) ? u4TnrStrIdx : (NR_MOIDX_MAX-u4MoIdx);
    u4TnrStrIdx =(NR_MOIDX_MAX-u4MoIdx);
    _vMoTnrProc(u4TnrStrIdx*2,u4NoiseLevel);

    _SWW(NR_MO_STA_04, u4MVMotionCnt, STA_MO_MV_SUM_2);
    _SWW(NR_MO_STA_04, u4FinalMotionLvl, STA_MO_MV_FINAL);
    //_SWW(NR_MO_STA_01, u4HomoSum, STA_MONR_HOMOSUM);
    _SWW(NR_MO_STA_04, u4HomoSum, STA_MONR_HOMOSUM_1);
    _SWW(NR_MO_STA_01, u4MoIdx, STA_MO_IDX);
    _SWW(NR_MO_STA_00, u4TnrStrIdx, STA_TNR_STR);
    _SWW(NR_MO_STA_00, u4SnrStrIdx, STA_SNR_STR);
    _SWW(NR_MO_STA_00, u4NoiseLevel, STA_NL_IDX);
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
        vDrvAnrNLProc(u4NoiseLvl);
    }

    if (_rDftNrWRegState[E_NR_DFT_ADAP_XNR].fgAutoOn())
    {
        vDrvAnrDCAlphaProc(u4NoiseLvl);
    }

    // adap IIR table
    if (_rDftNrWRegState[E_NR_DFT_ADAP_TNR].fgAutoOn())
    {
        vDrvAnrTnrProc(u4NoiseLvl);
    }

    // adap SNR manual threshold
    if (_rDftNrWRegState[E_NR_DFT_ADAP_SNR].fgAutoOn())
    {
        vDrvAnrSnrProc(u4NoiseLvl);
    }

    if (_rDftNrWRegState[E_NR_DFT_ADAP_RNR].fgAutoOn())
    {
        vDrvAnrRnrProc(u4NoiseLvl);
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
        vDrvAnrNLProc(u4NoiseLvl);
    }

    if (_rMoNrWRegState[E_NR_MOANR_XNR].fgAutoOn())
    {
        vDrvAnrDCAlphaProc(u4NoiseLvl);
    }

    if (_rMoNrWRegState[E_NR_MOANR_RNR].fgAutoOn())
    {
        vDrvMoRnrProc(u4NoiseLvl);
    }

    if (_rMoNrWRegState[E_NR_MOANR_MOANR].fgAutoOn())
    {
        vDrvAnrMoAnrProc(u4NoiseLvl, prNmSta->u4HomoSum);
    }
}

#ifdef DRV_SUPPORT_CUST_ANR
void NR_FW_ANR_RegisterCustFunc(ANR_FUNC_MODULE_PROC vCustModuleFunc, NR_ADAP_WREG_BASE_T* prCustSubFunc)
{
    _rAnrModule[E_NR_ANR_MODULE_CUST].vModuleProc = vCustModuleFunc;
    _rAnrModule[E_NR_ANR_MODULE_CUST].parSubFunc  = prCustSubFunc;
}
#endif

void NR_FW_ANR_Process(const NR_NM_STA_T* prNmSta, const NR_NM_ANA_T* prNmAna)
{
    UINT8 u1CurrModuleIdx = _u1CurAnrModuleIdx;
    ANR_FUNC_MODULE_PROC vModuleProc = NULL;

    if (!_fgInit)
        return;

    VERIFY(u1CurrModuleIdx < E_NR_ANR_MODULE_MAX);

    vModuleProc = _rAnrModule[u1CurrModuleIdx].vModuleProc;
    
    // call module process
    if (vModuleProc)
    {
        vModuleProc(prNmSta, prNmAna);

        if (_EN(EN_LOG_ANR))
            LOG(5, "[ACT] ANR Module (%d): %s\n", u1CurrModuleIdx, _rAnrModule[u1CurrModuleIdx].sDesc);
    }
    else
    {
        if (_EN(EN_LOG_ANR))
            LOG(5, "[ACT] NULL ANR Module (%d): %s\n", u1CurrModuleIdx, _rAnrModule[u1CurrModuleIdx].sDesc);
    }
}

void NR_FW_ANR_WriteReg(void)
{
    NR_ADAP_WREG_BASE_T * prWregSt = NULL;
    UINT8 u1CurrModule = MIN(_SWR(NR_CTRL_01, ANR_SEL), (E_NR_ANR_MODULE_MAX-1));
    UINT8 u1LogOnOff = _EN(EN_LOG_ANR);

    if (!_fgInit)
        return;

    if (_u1CurAnrModuleIdx != u1CurrModule)
    {
        _vAnrResetModule(_rAnrModule[_u1CurAnrModuleIdx].parSubFunc);
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

    _u1CurAnrModuleIdx = u1CurrModule;
}


void NR_FW_ANR_Init(void)
{
    _SWW(NR_CTRL_01, 0, ANR_SEL);
    _SWWM(NR_CTRL_02, P_Fld(1, EN_ADAP_WEIGHT_SW)|P_Fld(1, EN_ADAP_SNR_THM_SW)|P_Fld(1, EN_RNR_TH_ADAP_SW)|P_Fld(1, EN_MOTION_ANR_SW));
    _fgInit = 1;
}

