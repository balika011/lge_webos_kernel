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
 * $RCSfile: drv_nr_int.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// Inclusions
////////////////////////////////////////////////////////////////////////////////
#define _DRV_NR_NM_C_

#include "vdo_misc.h"
#include "video_def.h"
#include "nr_debug.h"
#include "hw_sw.h"
#include "drv_nr.h"
#include "drv_nr_int.h"

#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"
#include "drv_tvd.h"
#include "sv_const.h"

#define NM_CFG_FORCE_NL       (0)
#define NM_CFG_UPDATE_NL_SC   (1)
#define NM_CFG_ANTI_NL_GAIN   (2)
#define NM_CFG_ADJ_FATTAIL    (3)
#define NM_CFG_ADJ_MOFBK      (4)
#define NM_CFG_HIST_GP        (5)
#define NM_CFG_STABLIZER_IIR  (6)
#define NM_CFG_ADJ_MOFBK_HOMO (7)

#define NM_CFG(x) (1<<(x))

typedef enum
{
    E_NR_Q1B_PEAKBIN,
    E_NR_Q1B_CENTROID,
    E_NR_Q1B_UPSAMPLE_PEAK,
    E_NR_Q1B_TDFNL,
    E_NR_Q1B_NUM
} E_NR_Q1B;

typedef enum
{
    E_NR_Q2B_PEAKVAL,
    E_NR_Q2B_MOPIX,
    E_NR_Q2B_NUM
} E_NR_Q2B;

typedef enum
{
    E_NR_Q4B_TAILSUM,
    E_NR_Q4B_NUM
} E_NR_Q4B;

typedef enum
{
    E_NR_IIR_NL,
    E_NR_IIR_ERFB,
    E_NR_IIR_HOMO,
    E_NR_IIR_NUM
} E_NR_IIR;

typedef struct
{
    UINT8 u1StaMax;
    UINT8 u1SysSta;
    UINT8 u1IncOfst;
    UINT8 u1SubSample;
    UINT8 u1SlowErfb;
    INT16 i2DiffAccm;
    INT16 i2IncTh;
    INT16 i2DecTh;
} NR_ERFB_T;


typedef struct
{
    UINT32 u4Cfg;
    UINT8  u1ExtNLMode;
    UINT8  u1DefaultNoiseLvl;
    UINT8  u1ForceNoiseLvl;
    UINT8  u1SADHistIIR;
    UINT8  u1SADHistSubSample;
    UINT8  u1SADHistShowMode;
    UINT8  u1MoFbkOfst;

    NR_ERFB_T rNrErfb;
} NR_NM_PARAM_T;

EXTERN UINT8 bTvdSignalStrengthLevel(UINT8 bMode);

static NR_NM_PARAM_T _rNmPrm;
NR_NM_ANA_T _rNmAna;

static NR_Q1B_T _rNrQ1B[E_NR_Q1B_NUM];
static NR_Q2B_T _rNrQ2B[E_NR_Q2B_NUM];
static NR_Q4B_T _rNrQ4B[E_NR_Q4B_NUM];

static NR_IIR_T _rNrIIR[E_NR_IIR_NUM];

static UINT32 _u4SADHistCntFilt = 0;
static UINT16 _arSADHistFilt[SAD_HIST_ENTRY_NUM] = {0};
static UINT16 _arSADHistGp[SAD_HIST_ENTRY_NUM] = {0};

#define SADHIST _arSADHistFilt
#define SADHIST_IIR_RES (4)

static void _vDrvMCNRLogSADHist(const UINT16 arHist[SAD_HIST_ENTRY_NUM]);

////////////////////////////////////////////////////////////////////////////////
// NM functions
////////////////////////////////////////////////////////////////////////////////

static INT16 _i2Error(INT16 i2Diff)
{
    if (_rNmPrm.rNrErfb.u1SlowErfb)
        return (i2Diff > 0) ? 1 : ((i2Diff < 0) ? -1 : 0);
    else
        return i2Diff;
}

static UINT8 _u1DrvNRGetNLForQ(void)
{
    switch (_SWR(NR_NM_06, NM_SC_Q_NL_MODE))
    {
    default:
    case 0:
        return _rNmAna.u1Centroid;
    case 1:
        return _rNmAna.u1PeakBinMo;
    case 3:
        return _rNmAna.u1TdfNL;
    }
}

static UINT8 _u1DrvNRGetNLScnChgRaw(void)
{
    switch (_SWR(NR_NM_06, NM_SC_NL_MODE))
    {
    default:
    case 0:
        return _SWR(NR_NMSTA_0F, NR_NM_STA_NL_SCNCHG);
    case 1:
        return _SWR(NR_NMSTA_0F, NR_NM_STA_NL_SCNCHG_SORT);
    }
}

#if 0
static UINT8 _u1DrvNRGetNLScnChg(UINT8 u1NLScnChgCnt)
{
    if (u1NLScnChgCnt > 1)
    {
        return _u1DrvNRGetNLForQ();
    }
    else
    {
        return _u1DrvNRGetNLScnChgRaw();
    }
}
#endif

static UINT8 _u1DrvNRErrorFeedBack(UINT8 u1NLScnChgCnt, UINT8 u1Value)
{
    static UINT8 _u1Count = 0;
    UINT32 u4ForceLvlEn = _SWR(NR_NM_00, FORCE_NOISE_LVL_EN);
    UINT32 u4ForceLvl   = _SWR(NR_NM_00, FORCE_NOISE_LVL);
    UINT32 u4RstScErfb  = _SWR(NR_NM_03, RST_SC_ERFB);
    UINT32 u4ErfbMode   = _SWR(NR_NM_03, ERFB_MODE);

#if 0
    UINT32 u4Tmp = u4IO32Read4B(MJC_FBCK_38);
    
    INT16 i2DecTh = _rNrErfb.i2DecTh;
    
    if (R_Fld(u4Tmp, ERFB_DEC_TH_CURVE_EN))
    {
        UINT32 u4DecTh = ((UINT32) R_Fld(u4Tmp, ERFB_DEC_TH_SLOPE)*_rNrErfb.u1SysSta) >> 4;
        u4DecTh = u4DecTh + R_Fld(u4Tmp, ERFB_DEC_TH_MIN);
        u4DecTh = MIN(u4DecTh, R_Fld(u4Tmp, ERFB_DEC_TH_MAX));
        i2DecTh = - (INT16)u4DecTh;
    }
#endif
    if (u4ForceLvlEn)
    {
        u1Value = u4ForceLvl;
    }

    if (u4RstScErfb && u1NLScnChgCnt == 16 && (!u4ForceLvlEn))
    {
        _rNmPrm.rNrErfb.u1SysSta = _u1DrvNRGetNLForQ();
        _rNmPrm.rNrErfb.i2DiffAccm = 0;
        _u1Count = 0;
    }
    else
    {
        UINT8 u1IncOfst;
        INT16 i2Err = _i2Error((INT16)u1Value - (INT16)_rNmPrm.rNrErfb.u1SysSta);

        u1IncOfst = MIN(ABS(i2Err), _rNmPrm.rNrErfb.u1IncOfst);

        if (_u1Count == 0)
        {
            _rNmPrm.rNrErfb.i2DiffAccm += i2Err;

            if ((_rNmPrm.rNrErfb.i2DiffAccm > _rNmPrm.rNrErfb.i2IncTh) &&
                (_rNmPrm.rNrErfb.u1SysSta < _rNmPrm.rNrErfb.u1StaMax))
            {
                if (u4ErfbMode == 0)
                {
                    _rNmPrm.rNrErfb.u1SysSta = MIN((_rNmPrm.rNrErfb.u1SysSta + u1IncOfst), _rNmPrm.rNrErfb.u1StaMax);
                    _rNmPrm.rNrErfb.i2DiffAccm = 0;
                }
                else
                {
                    INT32 i4Sta;
                    _rNrIIR[E_NR_IIR_ERFB].u1Wei = (16-_rNmPrm.rNrErfb.u1IncOfst);
                    i4Sta = u4DrvNRUtilIIR(&_rNrIIR[E_NR_IIR_ERFB], 4, u1Value);
                        //LERP_TGT(_rNrErfb.u1SysSta, u1Value, (16-_rNrErfb.u1IncOfst), 4);  
                    _rNmPrm.rNrErfb.u1SysSta = MAX(MIN(i4Sta, (INT32)_rNmPrm.rNrErfb.u1StaMax), 0);
                    _rNmPrm.rNrErfb.i2DiffAccm = 0;
                }
            }
            else if ((_rNmPrm.rNrErfb.i2DiffAccm < _rNmPrm.rNrErfb.i2DecTh) && _rNmPrm.rNrErfb.u1SysSta > 0)
            {
                if (u4ErfbMode == 0)
                {
                    _rNmPrm.rNrErfb.u1SysSta = MAX(_rNmPrm.rNrErfb.u1SysSta, u1IncOfst) - u1IncOfst;
                    _rNmPrm.rNrErfb.i2DiffAccm = 0;
                }
                else
                {
                    INT32 i4Sta;
                    _rNrIIR[E_NR_IIR_ERFB].u1Wei = (16-_rNmPrm.rNrErfb.u1IncOfst);
                    i4Sta = u4DrvNRUtilIIR(&_rNrIIR[E_NR_IIR_ERFB], 4, u1Value);
                        //LERP_TGT(_rNrErfb.u1SysSta, u1Value, (16-_rNrErfb.u1IncOfst), 4);  
                    _rNmPrm.rNrErfb.u1SysSta = MAX(MIN(i4Sta, (INT32)_rNmPrm.rNrErfb.u1StaMax), 0);
                    _rNmPrm.rNrErfb.i2DiffAccm = 0;
                }
            }
        }

        if (++_u1Count >= _rNmPrm.rNrErfb.u1SubSample)
            _u1Count = 0;
        
        //LOG(2, "u1Value=%d, i2Err=%d, i2DiffAccm=%d, u1SysSta=%d\n",
        //    u1Value, i2Err, _rNrErfb.i2DiffAccm, _rNrErfb.u1SysSta);
    }
    
    return _rNmPrm.rNrErfb.u1SysSta;
}


static void _vDrvMCNRSADHistPreProc(const NR_NM_STA_T* prNmSta)
{
    UINT8 i = 0;
    UINT32 u4TmpVal;
    
    UINT32 u4RstScHistIIR = _SWR(NR_NM_03, RST_SC_HIST_IIR);
    UINT32 u4Wei = (u4RstScHistIIR&&prNmSta->u1ScnChg) ? 0 : _rNmPrm.u1SADHistIIR;

    if (u4RstScHistIIR && prNmSta->u1ScnChgCnt)
        return;
	
    for (i = 0; i < SAD_HIST_ENTRY_NUM; i++)
    {
        _arSADHistFilt[i] = LERP_TGT(_arSADHistFilt[i], prNmSta->arSADHistCurr[i], u4Wei, SADHIST_IIR_RES);
    }

    _u4SADHistCntFilt = LERP_TGT(_u4SADHistCntFilt, prNmSta->u4SADHistCntCurr, u4Wei, SADHIST_IIR_RES);
    
    if (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_HIST_GP))
    {
        _arSADHistGp[0] = ((UINT32)_arSADHistFilt[1] + _arSADHistFilt[1] + _arSADHistFilt[1]);
        _arSADHistGp[1] = ((UINT32)_arSADHistFilt[1] * 3 );
        
        for (i = 2; i < SAD_HIST_ENTRY_NUM; i++)
        {
            u4TmpVal = ((UINT32)_arSADHistFilt[MAX(1, i-1)] + _arSADHistFilt[i] + _arSADHistFilt[MIN(SAD_HIST_LAST, i+1)]);
            _arSADHistGp[i] = u4TmpVal; //((UINT32) _arSADHistGp[i]*u4Wei + u4TmpVal*(16-u4Wei)) >> 4;
        }
    }
    else
    {
        x_memcpy(_arSADHistGp, _arSADHistFilt, sizeof(UINT16)*SAD_HIST_ENTRY_NUM);
    }

    if (_rNrPrm.u4TolBlks)
    {
        _rNmAna.u1HistInvalidRto = (_arSADHistFilt[0] == 0xFFFF) ? 0 :
            (_rNrPrm.u4TolBlks - _u4SADHistCntFilt)*256 / _rNrPrm.u4TolBlks;
        _SWW(NR_NMSTA_13, _rNmAna.u1HistInvalidRto, NR_NM_INVLD_RTO);
    }
}

static UINT32 _u4EstTVDNL(void)
{
    UINT32 u4TvdSL = MIN(bTvdSignalStrengthLevel(1), 80);
    
    UINT32 u4EstNL = (UINT32) (100*u4TvdSL*u4TvdSL - 17852*u4TvdSL + 791880);

    return MIN((u4EstNL >> 12), 60);
}

static UINT32 _u4CalcTailSum(UINT32 u4PeakBin, const UINT16 arHist[SAD_HIST_ENTRY_NUM])
{
    UINT32 i         = 0;
    UINT32 u4TailSum = 0;
    UINT32 u4Rgn     = 6;
    UINT32 u4Tail    = u4PeakBin + 2 + ((u4PeakBin*u4Rgn) >> 4);

    for (i = u4Tail; i < SAD_HIST_ENTRY_NUM; i++)
        u4TailSum += arHist[i];

    return u4TailSum;
}

static UINT32 _u4CalcFatTailRto(UINT32 u4PeakVal, UINT32 u4TailSum, UINT32 u4PeakValRto)
{
    UINT32 u4FatTail;
    UINT32 u4RdcPeak = (u4PeakVal * MIN(u4PeakValRto, 16)) >> 4;

    if (u4PeakVal == 0 || u4TailSum < u4RdcPeak)
    {
        u4FatTail= 0;
    }
    else
    {
        u4FatTail = (u4TailSum - u4RdcPeak)*64 / u4PeakVal;
        u4FatTail = MIN(u4FatTail, 64);
    }

    return u4FatTail;
}

static UINT8 _u1DrvMCNRMoFbckFact(UINT32 u4MoPixCnt)
{
	// 0~16
	UINT32 u4Dft, u4Max;
	UINT8  u1MBlend;
	
	u4Dft = (_rNrPrm.u4TolBlks >> 6) >> 2;
	u4Max = (_rNrPrm.u4TolBlks >> 6);
	u4MoPixCnt *= 10;

	if (u4MoPixCnt >= u4Dft)
		u1MBlend = LERP_F_C(u4Dft, 8, u4Max, 16, u4MoPixCnt);
	else
		u1MBlend = LERP_F_C(0, 0, u4Dft, 8, u4MoPixCnt);
	
	return u1MBlend;
}

static UINT8 _u1DrvMCNRCalcSADCentroid(UINT8 u1Start, UINT8 u1End, const UINT16 arHist[SAD_HIST_ENTRY_NUM], UINT32* u4Moment)
{
	UINT32 u4MSum = 0, u4AreaSum = 0;
	UINT32 i = 0;
    UINT32 u4Ret = 0;
	
	u1End = MIN(MAX(u1Start+1, u1End), 16);
	for (i = u1Start; i < u1End; i++)
	{
		u4AreaSum += arHist[i];
		u4MSum += i*4*arHist[i];
	}

    if (u4Moment != NULL)
        *u4Moment = u4MSum;

	if (u4AreaSum != 0)
    {   
		u4Ret = u4MSum / u4AreaSum;

        // the value should be siginificant, Area >= 25% {=> min(4*Area / Total, 100%) * u4Ret}
        //u4Ret = (u4Ret*MIN(u4AreaSum*4, u4AreaSum + arHist[0])) / (u4AreaSum + arHist[0]);
        u4AreaSum += arHist[0];
        u4Ret = MIN(u4MSum*4, u4Ret*u4AreaSum) / u4AreaSum;
 
  }
	
	return u4Ret;
}

static void _vDrvMCNRFrameLevelFallback (const NR_NM_STA_T* prNmSta)
{   
    INT32 nzc = prNmSta->u2Nzc;
    INT32 hist_diff_acc = prNmSta->u4HistDiffAcc;

    INT32 nr_flfb_max_status = 16;
    INT32 nr_flfb_status_thd_l = 4;
    INT32 nr_flfb_status_thd_h = 6;

    INT32 tol_blks = _rNrPrm.u4TolBlks;
    INT32 hist_diff_acc_thd_h = 11 * tol_blks / 128;
    INT32 hist_diff_acc_thd_l = 9 * tol_blks / 256;

    INT32 nr_fallback_step = _rNmAna.u1FbkStep;

    if (( hist_diff_acc > hist_diff_acc_thd_h ) || nzc > 10 )
        nr_fallback_step --;
    else if (( hist_diff_acc < hist_diff_acc_thd_l ) && nzc <= 8 )
        nr_fallback_step ++;
    else if (nr_fallback_step > nr_flfb_status_thd_h )
        nr_fallback_step --;

    nr_fallback_step = (UINT8) MIN(MAX(0, nr_fallback_step), nr_flfb_max_status);

    if (prNmSta->u1FlfbStatus || (prNmSta->u1CompositeStatus > 8))
    {
        nr_fallback_step = 0;
    }

    if ( nr_fallback_step < nr_flfb_status_thd_l )
        _rNmAna.u1FbkSta = E_NR_FB_MANR;
    else if ( nr_fallback_step >= nr_flfb_status_thd_l && nr_fallback_step < nr_flfb_status_thd_h )
        _rNmAna.u1FbkSta = _rNmAna.u1FbkSta;
    else 
        _rNmAna.u1FbkSta = E_NR_FB_MCNR;

    _rNmAna.u1FbkStep = nr_fallback_step;
}

static UINT8 _u1CalcPeakBin(const UINT16 arHist[SAD_HIST_ENTRY_NUM], UINT32* pu4PeakBinRawVal)
{
    UINT8 i = 0;
    UINT8 u1PeakBin = 1;

    for (i = 1; i < SAD_HIST_ENTRY_NUM - 1; i ++)
    {
        if ((arHist[i+1] <= arHist[i]) &&
            (arHist[i] >= arHist[MAX(1,i-1)]))
        {
            u1PeakBin = i;

            if (pu4PeakBinRawVal)
                *pu4PeakBinRawVal = arHist[i];

            break;
        }
    }

    return u1PeakBin;
}


#if 0
static UINT8 _u1CalcPeakBin(UINT16* pu2PeakBinRawVal)
{
    UINT8 i = 0;
    UINT8 u1PeakBin = 1;
    UINT8 u1PeakBinLeft = SAD_HIST_LAST;
    UINT8 u1Found = 0;

    for (i = 1; i < SAD_HIST_ENTRY_NUM - 1; i ++)
    {
        //if ( SAD_hist[i+1] <= SAD_hist[i] && SAD_hist[i] >= SAD_hist[MAX(1,i-1)] )
        if (((u1Found & 0x1) == 0) &&
            (_arSADHistGp[i+1] <= _arSADHistGp[i]) &&
            (_arSADHistGp[i] >= _arSADHistGp[MAX(1,i-1)]))
        {
            u1PeakBin = i;
            u1Found |= 0x1;
        }

        if (((u1Found & 0x2) == 0) && pu2PeakBinRawVal &&
            (SADHIST[i+1] <= SADHIST[i]) &&
            (SADHIST[i] >= SADHIST[MAX(1,i-1)]))
        {
            (*pu2PeakBinRawVal) = SADHIST[i];
            u1Found |= 0x2;
        }  
    }

    if (_SWR(NR_NM_03, USE_RIGHTBIN_POSSIBLE))
    {
        for (i = SAD_HIST_LAST - 1; i > 1; i --)
        {
            if ( _arSADHistGp[i+1] <= _arSADHistGp[i]
                && _arSADHistGp[i] >= _arSADHistGp[MAX(1,i-1)])
            {
                u1PeakBinLeft = i;
                break;
            }      
        }

        u1PeakBin = (!_rNmSta.u1ScnChgCnt && (_arSADHistGp[u1PeakBinLeft] > _arSADHistGp[u1PeakBin])) ? 
            u1PeakBinLeft : u1PeakBin;
    }

    return u1PeakBin;
}

static UINT8 _u1CalcPeakBinNew(UINT16* pu2PeakBinRawVal)
{
    UINT8 i = 0;
    UINT8 u1PeakBin = 1;
    UINT32 u4Max = 0;

    for (i = SAD_HIST_LAST - 1; i >= 1; i --)
    {
        if (u4Max < _arSADHistGp[i])
        {
            u4Max = _arSADHistGp[i];
            u1PeakBin = i;

            if (pu2PeakBinRawVal)
                (*pu2PeakBinRawVal) = SADHIST[i];
        }      
    }

    return u1PeakBin;
}
#endif

static UINT8 _u1CalcRTrough(UINT8 u1PeakBin, const UINT16 arHist[SAD_HIST_ENTRY_NUM])
{
    UINT8 i;
    UINT8 u1RTrough = SAD_HIST_LAST;
    
    for (i = u1PeakBin + 1; i < SAD_HIST_ENTRY_NUM; i ++)
    {
        if (arHist[i] < arHist[1])
        {
            u1RTrough = i;
            break;
        }    
    }

    return u1RTrough;
}

static UINT8 _u1CalcTerminate(const UINT16 arHist[SAD_HIST_ENTRY_NUM])
{
    UINT8 i = 0;

    for (i = 1; i < SAD_HIST_ENTRY_NUM; i++)
    {
        if (arHist[i] == 0)
            break;
    }

    return i;
}

static INT32 _iCalcUpsamplePeakBin(INT32 peak_bin, const UINT16 arHist[SAD_HIST_ENTRY_NUM])
{
    INT32 max_delta, min_delta, x1;

    INT32 delta_r = arHist[peak_bin] - arHist[MIN(peak_bin+1, SAD_HIST_LAST)];
    INT32 delta_l = 0;

    if (peak_bin > 1)
        delta_l = arHist[peak_bin] - arHist[MAX(peak_bin-1, 0)];    
    else
        delta_l = delta_r;

    max_delta = MAX(delta_l,delta_r);
    min_delta = MIN(delta_l,delta_r);
    x1 =0;
    if ( 5 * min_delta > 3 * max_delta )
        x1 = 0;
    else if ( 7 * min_delta > max_delta )
        x1 = 1;
    else
        x1 = 2;

    if ( delta_l > delta_r )
        peak_bin = peak_bin*4 + x1;
    else
        peak_bin = MAX(peak_bin*4 - x1, 0);

    return peak_bin;
}

static UINT8 _u1CalcAntiNoiseLevelGain(UINT32 u4Bin0, UINT32 u4HistCnt)
{
    UINT32 u4Ret;
    UINT32 a = _SWR(NR_NM_0F, NM_ANTI_NL_GAIN_THD_LO);
    UINT32 b = _SWR(NR_NM_0F, NM_ANTI_NL_GAIN_THD_HI);
    UINT32 c = _SWR(NR_NM_0F, NM_ANTI_NL_GAIN_MAX);

    b = MAX(b, a+1);
    
    u4Bin0 = MIN(u4Bin0, u4HistCnt);

    u4Bin0 *= 64;

    if (u4HistCnt == 0)
        return 0;

    if (u4Bin0 <= u4HistCnt*a)
    {
        u4Ret = 0;
    }
    else if (u4Bin0 > u4HistCnt*b)
    {
        u4Ret = c;
    }
    else
    {
        u4Ret = (c * (u4Bin0 - a*u4HistCnt)) / ((b-a)*u4HistCnt);
    }

    u4Ret = MIN(u4Ret, 64);

    return u4Ret;
}

void _vDrvMCNRCleanDiffLvl(void)
{
    static UINT8 _u1Cnt = 0;
    static UINT32 _u4PreHomoSum = 0;
    UINT8 u1CleanDiffLvl = 0;
    UINT32 u4DiffTh;
    UINT32 u4DiffTh_max = _SWR(NR_NM_04, CLEAN_DF_LVL_PDSUM_DIFF_TH);
    UINT32 u4DiffTh_min = _SWR(NR_NM_13, CLEAN_DF_LVL_PDSUM_DIFF_MIN_TH);
    UINT32 u4EnterTh = _SWR(NR_NM_04, CLEAN_DF_LVL_ENTER_TH);
    UINT32 u4HomoSumTh = _SWR(NR_NM_0C, CLEAN_DF_LVL_PDSUM_TH);
    UINT32 u4HomoSumTh_min = _SWR(NR_NM_13, CLEAN_DF_LVL_PDSUM_MIN_TH);
    UINT32 u4AbsDiff = 0;
    UINT32 u4HomoSum = _rNmSta.u4HomoSum;//prNmSta->u4HomoSum;
    UINT32 u4TwoHomoSum;

    UINT32 u4NoiseLvl = _rNmAna.u1NoiseLevel;
//    if (u4NoiseLvl >= 4 && (_rNmSta.u1StillNoisyScene && _SWR(NR_NM_09, NM_ZOOM_TEX_EN)))
//        u4NoiseLvl -= 4;

    u4AbsDiff = AbsDiff(u4HomoSum, _u4PreHomoSum);
    u4TwoHomoSum = u4HomoSum+_u4PreHomoSum;
    u4DiffTh = LERP(u4DiffTh_min,u4DiffTh_max, _rNmAna.u1CleanDiffLvl, 3);

    if (/*_rNmSta.u1GmvValid && */(_rNmSta.u1GmvX == 0) && (_rNmSta.u1GmvY == 0)
        && (u4TwoHomoSum > u4HomoSumTh_min) && (u4TwoHomoSum < u4HomoSumTh)
        && (u4AbsDiff > u4DiffTh)
        )
    {
        //MDDI_WRITE_FLD(VDP_1, MCVP_KC_29, 1 , NUM_OSD_ON);
        if (_u1Cnt < 24)
            _u1Cnt++;
    }
    else
    {
        //MDDI_WRITE_FLD(VDP_1, MCVP_KC_29, 0 , NUM_OSD_ON);
        if (_u1Cnt)
            _u1Cnt--;
    }

    if ((_u1Cnt > u4EnterTh) 
        && (u4NoiseLvl < 12)
        && (u4NoiseLvl > 3)
        /*&& (_rNmSta.u1ValidNoiseLevel)*/
        )
    {
        u1CleanDiffLvl = (_u1Cnt - u4EnterTh);
        u1CleanDiffLvl = MIN(u1CleanDiffLvl, 8);
    }
    else
    {
        u1CleanDiffLvl = 0;
    }

    _u4PreHomoSum = u4HomoSum;

    _rNmAna.u1CleanDiffLvl = u1CleanDiffLvl;
    _SWW(NR_NMSTA_0F, u1CleanDiffLvl, NR_NM_STA_NR_CLEAN_DF_LVL);
}

#if 0
static UINT32 _u4EstNLByTdfMo(void)
{
    static UINT32 _u4PrevMoPixCnt = 0;
    static UINT32 _u4IIRMoPixCnt = 0;
    UINT32 u4MoPixCnt = (_rNmSta.u4MoPixCnt>>1);

    static UINT32 _u4PrevAvg = 0;
    static UINT32 _u4IIRAvg = 0;
    UINT32 u4TmprlDiffCnt = _rNmSta.u4TmprDiffCnt;
    
    UINT32 u4Avg = 0;
    UINT32 u4EstLvl = 0;

    if ((u4TmprlDiffCnt>>5))
    {
        u4Avg = MIN((_rNmSta.u4TmprDiffSum>>5),612953)*7007 / (u4TmprlDiffCnt>>5);
        //u4Avg = MIN(u4Avg, 56056);
    }

    if (AbsDiff(_u4PrevMoPixCnt, u4MoPixCnt) >= (MAX(_u4PrevMoPixCnt, 5000)*8))
    {
        // scene change happens, do not use the value.
        u4MoPixCnt = _u4PrevMoPixCnt;
    }
    _u4IIRMoPixCnt = LERP_TGT(_u4IIRMoPixCnt, u4MoPixCnt, 15, 4);
    _u4PrevMoPixCnt = u4MoPixCnt;

    if (AbsDiff(_u4PrevAvg, u4Avg) >= (MAX(_u4PrevAvg, 1534)*8))
    {
        u4Avg = _u4PrevAvg;
    }
    _u4IIRAvg = LERP_TGT(_u4IIRAvg, u4Avg, 15, 4);
    _u4PrevAvg = u4Avg;

#if 0
    u4EstLvl = _u4IIRAvg + 5716 - _u4IIRMoPixCnt;
    if (u4EstLvl & 0x80000000)
        u4EstLvl = 0;
    u4EstLvl /= 1534;
#endif
    u4EstLvl = ((_u4IIRAvg*271+64)>>7) - 9450 - _u4IIRMoPixCnt;
    if (u4EstLvl & 0x80000000)
        u4EstLvl = 0;
    u4EstLvl /= 1923;

    u4EstLvl = MIN(u4EstLvl, 32);

    _SWWM(NR_NMSTA_14, P_Fld(u4EstLvl, NR_NM_STA_NL_TDF_RAW)|P_Fld(_u4IIRAvg, NR_NM_STA_IIR_AVG_TMPRL));
    _SWW(NR_NMSTA_15, _u4IIRMoPixCnt, NR_NM_STA_IIR_MOPIX);

    return (UINT32) u4EstLvl;
}
#else
static UINT32 _u4EstNLByTdfMo(const NR_NM_STA_T* prNmSta)
{
    static UINT32 _u4PrevMoPixCnt = 0;
    static UINT32 _u4IIRMoPixCnt = 0;
    UINT32 u4MoPixCnt = prNmSta->u4MoPixCnt >> 6;

    static UINT32 _u4PrevTmprlDiffCnt = 0;
    static UINT32 _u4IIRTmprlDiffCnt = 0;
    UINT32 u4TmprlDiffCnt = prNmSta->u4TmprDiffCnt >> 7;

    static UINT32 _u4PrevTmprlDiffSum = 0;
    static UINT32 _u4IIRTmprlDiffSum = 0;
    UINT32 u4TmprlDiffSum = prNmSta->u4TmprDiffSum >> 7;

    UINT32 u4IIR = _SWR(NR_NM_0F, NM_TDF_STA_IIR);
    
    UINT32 u4EstLvl = 0;

    if (AbsDiff(_u4PrevMoPixCnt, u4MoPixCnt) < (MAX(_u4PrevMoPixCnt, 128)*8))
    {
        _u4IIRMoPixCnt = LERP_TGT(_u4IIRMoPixCnt, u4MoPixCnt, u4IIR, 4);
    }
    _u4PrevMoPixCnt = u4MoPixCnt;

    if (AbsDiff(_u4PrevTmprlDiffCnt, u4TmprlDiffCnt) < (MAX(_u4PrevTmprlDiffCnt, 512)*8))
    {
        _u4IIRTmprlDiffCnt = LERP_TGT(_u4IIRTmprlDiffCnt, u4TmprlDiffCnt, u4IIR, 4);
    }
    _u4PrevTmprlDiffCnt = u4TmprlDiffCnt;

    if (AbsDiff(_u4PrevTmprlDiffSum, u4TmprlDiffSum) < (MAX(_u4PrevTmprlDiffSum, 1024)*8))
    {
        _u4IIRTmprlDiffSum = LERP_TGT(_u4IIRTmprlDiffSum, u4TmprlDiffSum, u4IIR, 4);
    }
    _u4PrevTmprlDiffSum = u4TmprlDiffSum;

    // calculate estimated noise level
    #if 0
    if (_u4IIRTmprlDiffCnt)
    {
        u4EstLvl = ((61016*_u4IIRTmprlDiffSum)/_u4IIRTmprlDiffCnt)
            - (5*_u4IIRTmprlDiffCnt + 586*_u4IIRMoPixCnt + 34704);
        if (u4EstLvl & 0x80000000)
        {
            LOG(3, "u4EstLvl=%08x, _u4IIRTmprlDiffSum=%x, _u4IIRTmprlDiffCnt=%x, _u4IIRMoPixCnt=%x\n",
                u4EstLvl, _u4IIRTmprlDiffSum, _u4IIRTmprlDiffCnt, _u4IIRMoPixCnt);
            u4EstLvl = 0;
        }
        u4EstLvl = (u4EstLvl + 4096) >> 13;
    }    
    #endif
    if (_u4IIRTmprlDiffCnt)
    {
        u4EstLvl = ((63966*_u4IIRTmprlDiffSum)/_u4IIRTmprlDiffCnt) + 29151
            - (9*_u4IIRTmprlDiffCnt + 707*_u4IIRMoPixCnt);
        if (u4EstLvl & 0x80000000)
        {
            //LOG(3, "u4EstLvl=%08x, _u4IIRTmprlDiffSum=%x, _u4IIRTmprlDiffCnt=%x, _u4IIRMoPixCnt=%x\n",
            //    u4EstLvl, _u4IIRTmprlDiffSum, _u4IIRTmprlDiffCnt, _u4IIRMoPixCnt);
            u4EstLvl = 0;
        }
        u4EstLvl = (u4EstLvl + 4096) >> 13;
    }

    u4EstLvl = MIN(u4EstLvl, 32);

    if (_EN(EN_LOG_SCNCHG))
        LOG(3, "u4EstLvl=%08x, _u4IIRTmprlDiffSum=%x, _u4IIRTmprlDiffCnt=%x, _u4IIRMoPixCnt=%x\n",
            u4EstLvl, _u4IIRTmprlDiffSum, _u4IIRTmprlDiffCnt, _u4IIRMoPixCnt);

    _SWWM(NR_NMSTA_14, P_Fld(u4EstLvl, NR_NM_STA_NL_TDF_RAW)|P_Fld(_u4IIRTmprlDiffSum, NR_NM_STA_IIR_TMPRL_SUM));
    _SWW(NR_NMSTA_15, _u4IIRMoPixCnt, NR_NM_STA_IIR_MOPIX);
    _SWW(NR_NMSTA_16, _u4IIRTmprlDiffCnt, NR_NM_STA_IIR_TMPRL_CNT);

    return (UINT32) u4EstLvl;
}
#endif

static INT32 _cmpr(const void* a, const void* b)
{
    return ((*((UINT8*) a)) > (*((UINT8*) b))) ? 1 : -1;
}

static void _vCalcNoiseLevelScnChg(UINT8 u1NLScnChgCnt, const UINT8 arQueue[16])
{
    UINT8 arSortQ[16];

    UINT32 i;
    UINT32 u4ScnChgCnt = MIN(u1NLScnChgCnt, 16);

    if (u4ScnChgCnt == 1)
    {
        UINT32 u4Sum = 0;
        UINT32 u4Med = 0;

        x_memcpy(arSortQ, arQueue, 16*sizeof(UINT8));
        _vInsertSort(UINT8, arSortQ, 16, _cmpr);

        for (i = 0; i < 16; i++)
        {
            u4Sum += arSortQ[i];

            if (_EN(EN_LOG_SCNCHG))
                LOG(3, "Q[%d] = %d\n", i, arSortQ[i]);
        }

        u4Sum = (7 + u4Sum - arSortQ[0] - arSortQ[15]) / 14;
        u4Med = ((arSortQ[7]+arSortQ[8])>>1);

        if (_EN(EN_LOG_SCNCHG))
        {
            LOG(3, "AVG = %d, MED = %d\n", u4Sum, u4Med);
        }

        _SWWM(NR_NMSTA_0F,
            P_Fld(u4Sum, NR_NM_STA_NL_SCNCHG)|
            P_Fld(u4Med, NR_NM_STA_NL_SCNCHG_SORT));
    }
}

extern UINT8 _u1Is_TVDNM_Stable;
extern RTvdStatus _rTvd3dStatus;
static void _vDrvMCNRSADAnalysis (const NR_NM_STA_T* prNmSta)
{
    UINT32 u4PeakBin    = 0;
    UINT32 u4PeakBinVal = 0;
    UINT32 u4TailSum    = 0;
    UINT32 u4Centroid   = 0;
    UINT32 u4FatTailRto = 0;
    INT32 i4PeakBinMo   = 0;
    UINT8 u1NoiseLevel;

    // 1. peak bin determination
    u4PeakBin = _u1CalcPeakBin(SADHIST, &u4PeakBinVal);
    u4PeakBin = MIN(u4PeakBin, (NR_FILT_NUM_BIN-1));
    ADD_TO_Q(_rNrQ1B[E_NR_Q1B_PEAKBIN], u4PeakBin);
    ADD_TO_Q(_rNrQ2B[E_NR_Q2B_PEAKVAL], u4PeakBinVal);

    // 2. required signals
    _rNmAna.u1RTrough = _u1CalcRTrough(u4PeakBin, SADHIST);
    _rNmAna.u1Terminate = _u1CalcTerminate(SADHIST);

    i4PeakBinMo = _iCalcUpsamplePeakBin(u4PeakBin, _arSADHistGp);

    u4Centroid = _u1DrvMCNRCalcSADCentroid(1, _rNmAna.u1Terminate/*MIN(_rNmAna.u1RTrough, 2*u4PeakBin)*/, SADHIST, &_rNmAna.u4HistMoment);
    _rNmAna.u1AntiNLGain = _u1CalcAntiNoiseLevelGain(SADHIST[0], _u4SADHistCntFilt);

    u4TailSum = _u4CalcTailSum(u4PeakBin, SADHIST);
    ADD_TO_Q(_rNrQ4B[E_NR_Q4B_TAILSUM], u4TailSum);

    u4FatTailRto = _u4CalcFatTailRto(u4PeakBinVal, u4TailSum, 8);

    if (_rNrQ1B[E_NR_Q1B_PEAKBIN].u4Idx == 0 || prNmSta->u1NLScnChgCnt == 16)
    {
        _rNmAna.u1FIRPeakBin    = GET_Q_AVG(_rNrQ1B[E_NR_Q1B_PEAKBIN]);
        _rNmAna.u2FIRPeakBinVal = GET_Q_AVG(_rNrQ2B[E_NR_Q2B_PEAKVAL]);
        _rNmAna.u4TailSum       = GET_Q_AVG(_rNrQ4B[E_NR_Q4B_TAILSUM]);
        // fat tail ratio
        _rNmAna.u1FatTailRto    = _u4CalcFatTailRto(_rNmAna.u2FIRPeakBinVal, _rNmAna.u4TailSum, 8);

        //if (_EN(EN_LOG_SCNCHG))
        //    LOG(0, "u1FIRPeakBin = %d, u2FIRPeakBinVal = %d, u4TailSum = %d, u1FatTailRto = %d\n",
        //        _rNmAna.u1FIRPeakBin, _rNmAna.u2FIRPeakBinVal, _rNmAna.u4TailSum, _rNmAna.u1FatTailRto);
    }

    if (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_ANTI_NL_GAIN))
    {
        UINT32 u4Wei = MAX(u4Centroid*2, 32) - 32;
        u4Wei = MIN(u4Wei, 32);
        u4Centroid -= (((UINT32)u4Centroid*_rNmAna.u1AntiNLGain*u4Wei)>>(6+5));
    }

    if (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_ADJ_FATTAIL))
    {
        i4PeakBinMo -= ((i4PeakBinMo*MIN(u4FatTailRto,64)) >> 6);
        u4Centroid  -= ((u4Centroid*MIN(u4FatTailRto,64)) >> 6);
    }

    ADD_TO_Q(_rNrQ1B[E_NR_Q1B_CENTROID], u4Centroid);
    ADD_TO_Q(_rNrQ1B[E_NR_Q1B_UPSAMPLE_PEAK], i4PeakBinMo);
	ADD_TO_Q(_rNrQ2B[E_NR_Q2B_MOPIX], (prNmSta->u4MoPixCnt>>8));

	_rNmAna.u1MoFbckBlend = (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_ADJ_MOFBK_HOMO) ? _rNmAna.u1MoIdx :
        _u1DrvMCNRMoFbckFact(GET_Q_AVG(_rNrQ2B[E_NR_Q2B_MOPIX]))) + _rNmPrm.u1MoFbkOfst;
    _rNmAna.u1MoFbckBlend = MIN(MAX(_rNmAna.u1MoFbckBlend, 16) - 16, 16);

    //if (_rNrQ1B[E_NR_Q1B_CENTROID].u4Idx == 0)
    {
        _rNmAna.u1Centroid  = GET_Q_AVG(_rNrQ1B[E_NR_Q1B_CENTROID]);   
        _rNmAna.u1PeakBinMo = GET_Q_AVG(_rNrQ1B[E_NR_Q1B_UPSAMPLE_PEAK]);

        if (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_ADJ_MOFBK))
        {
            //UINT32 u4A;
            //u4A = MIN(MIN(_rNmAna.u1PeakBinMo, 8), _rNmAna.u1Centroid);
            //_rNmAna.u1Centroid = LERP(u4A, _rNmAna.u1Centroid, _rNmAna.u1MoFbckBlend, 4);
            _rNmAna.u1Centroid = LERP(0, _rNmAna.u1Centroid, _rNmAna.u1MoFbckBlend, 4);
        }
    }

    _rNmAna.u1MvApprove = (_rNmAna.u1FbkSta == E_NR_FB_MCNR) || _SWR(NR_NM_00, FORCE_MV_APPROVE); 

    _rNmAna.u1PeakBin = u4PeakBin;

    // 3. noise level processing
    if (_rNrPrm.u1NoiseMeterEn == NR_NM_TDF_BASE)  // firmware control
    {
        UINT8 u1TdfNL = _u4EstNLByTdfMo(prNmSta);
        ADD_TO_Q(_rNrQ1B[E_NR_Q1B_TDFNL], u1TdfNL);
        if (_rNrQ1B[E_NR_Q1B_TDFNL].u4Idx == 0)
            _rNmAna.u1TdfNL = GET_Q_AVG(_rNrQ1B[E_NR_Q1B_TDFNL]);  
    }

    _vCalcNoiseLevelScnChg(prNmSta->u1NLScnChgCnt, _rNrPrm.u1NoiseMeterEn == NR_NM_TDF_BASE ? 
        _rNrQ1B[E_NR_Q1B_TDFNL].rQueue : _rNrQ1B[E_NR_Q1B_CENTROID].rQueue);
    
    if (prNmSta->u1NLScnChgCnt == 1 && _EN(EN_LOG_SCNCHG))
    {
        _vDrvMCNRLogSADHist(SADHIST);
        LOG(4, "u4Sum = %d\n", _rNrQ1B[E_NR_Q1B_CENTROID].u4Sum);
    }
    
    if (_SWR(NR_NM_14, NM_TRANSITION_TYPE))
    {
    u1NoiseLevel = (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_UPDATE_NL_SC)) ? /*_u1DrvNRGetNLScnChgRaw()*/
        (prNmSta->u1NLScnChgCnt > 1 ?  u4Centroid : _u1DrvNRGetNLScnChgRaw()): MIN(_u1DrvNRGetNLForQ(), 60);
    }
    else
    {
        u1NoiseLevel = (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_UPDATE_NL_SC)) ? _u1DrvNRGetNLScnChgRaw()        /*_u1DrvNRGetNLScnChg(prNmSta->u1NLScnChgCnt)*/ : MIN(_u1DrvNRGetNLForQ(), 60);    
    }

    if (_rNrPrm.u1NoiseMeterEn == 0)
    {
        u1NoiseLevel = 4; //_rNmPrm.u1DefaultNoiseLvl;
    }

    //NL before IIR filter
    _SWWM(NR_NMSTA_1A,P_Fld(u1NoiseLevel, NR_NM_STA_NL_SAD)|P_Fld(_u4EstTVDNL(), NR_NM_STA_NL_TVD));

    switch (_rNmPrm.u1ExtNLMode)
    {
    case 1:
        u1NoiseLevel = (_u1Is_TVDNM_Stable ==1) ? MAX(_u4EstTVDNL(), u1NoiseLevel) : u1NoiseLevel;
        break;
    case 2:
        u1NoiseLevel = (_u1Is_TVDNM_Stable ==1) ? _u4EstTVDNL() : _rNmAna.u1TVD_NL;
        _rNmAna.u1TVD_NL = u1NoiseLevel;
        break;
    default:
        u1NoiseLevel = u1NoiseLevel;
        break;
    }

    if (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_STABLIZER_IIR))
    {
    	if (bGetVideoDecType(SV_VP_MAIN)!= SV_VD_TVD3D)
	{
		u1NoiseLevel = u4DrvNRUtilIIR(&_rNrIIR[E_NR_IIR_NL], 8, u1NoiseLevel);
	}
	else
	{
            if (_SWR(NR_NM_13,NR_NM_NOIIR_CH_CHG))
            {
                if (_u1Is_TVDNM_Stable==0)
                {
                    u1NoiseLevel = _u4EstTVDNL();
                }
                else if (_u1Is_TVDNM_Stable==1)
                {
                    u1NoiseLevel = u4DrvNRUtilIIR(&_rNrIIR[E_NR_IIR_NL], 8, u1NoiseLevel);
                }
            }
            else
            {
                if (_rTvd3dStatus.bSigStatus ==  (UINT8)SV_VDO_STABLE)
                {
                    u1NoiseLevel = u4DrvNRUtilIIR(&_rNrIIR[E_NR_IIR_NL], 8, u1NoiseLevel);
                }
                else
                {
                    u1NoiseLevel = _u4EstTVDNL();
                }
            }
        }    
    }    
    else
    {
        u1NoiseLevel = _u1DrvNRErrorFeedBack(prNmSta->u1NLScnChgCnt, u1NoiseLevel);
    }
    
    _SWW(NR_NMSTA_1A, u1NoiseLevel, NR_NM_STA_NL_FINAL);
    _SWW(NR_NMSTA_1A, _u1TVDStable, NRTVDstable);
    
    if (_rNmPrm.u4Cfg & NM_CFG(NM_CFG_FORCE_NL))
    {
        _rNmAna.u1NoiseLevel = _rNmPrm.u1ForceNoiseLvl;
    }
    else
    {
        _rNmAna.u1NoiseLevel = u1NoiseLevel;
    }
    
    _rNmAna.u1ValidNoiseLevel = 1;
	_rNmAna.u1FbCnt = 0;

    _SWWM(NR_NMSTA_0E,
        P_Fld(_rNmAna.u1NoiseLevel, NR_NM_STA_NL)|P_Fld(_rNmAna.u1ValidNoiseLevel, NR_NM_STA_NLVD)|
        P_Fld(_rNmAna.u1Centroid, NR_NM_STA_CNTD)|P_Fld(_rNmAna.u1PeakBinMo, NR_NM_STA_PEAKMO)|P_Fld(_rNmAna.u1PeakBin, NR_NM_STA_PEAK)|
        P_Fld(_rNmAna.u1FIRPeakBin, NR_NM_STA_FIRPEAK));
    _SWWM(NR_NMSTA_0F,
        P_Fld(_rNmAna.u1RTrough, NR_NM_STA_RTROUGH)|P_Fld(_rNmAna.u1Terminate, NR_NM_STA_TERMINATE));
    _SWWM(NR_NMSTA_16, P_Fld(_rNmAna.u2FIRPeakBinVal, NR_NM_STA_PEAKBIN_VAL));
    _SWWM(NR_NMSTA_17, P_Fld(_rNmAna.u1FatTailRto, NR_NM_STA_FATTAIL_RTO)|P_Fld(_rNmAna.u4TailSum, NR_NM_STA_TAIL_SUM));
    _SWWM(NR_NMSTA_18, P_Fld(_rNmAna.u1MoFbckBlend, NR_NM_STA_MOFBK_RTO));
}

static void _vDrvMCNRCalcMoIdx(const NR_NM_STA_T* prNmSta)
{
    UINT32 u4MoIdx;
    UINT32 u4HomoSum = prNmSta->u4HomoSum;
    UINT32 u4Shift = _SWR(NR_NM_11, NM_HOMO_MO_SHIFT);
    UINT32 u4MoGain = _SWR(NR_NM_11, NM_ADJ_MO_GAIN);

    // Get param
    UINT8 u1PosMid = _SWR(NR_NM_12, NM_HOMO_MO_POS);
    UINT8 u1MoMax = _SWR(NR_NM_12, NM_HOMO_MO_TH3);
    UINT8 u1MoMid = _SWR(NR_NM_12, NM_HOMO_MO_TH2);
    UINT8 u1MoMin = _SWR(NR_NM_12, NM_HOMO_MO_TH1);

    // motion index
    _rNrIIR[E_NR_IIR_HOMO].u1Wei = _SWR(NR_MO_01, MONR_IIR_FACTOR);
    u4HomoSum = u4HomoSum*_rNrPrm.u1NormFact >> 2;
    u4HomoSum = (u4HomoSum*u4MoGain) >> 6;
    u4HomoSum = u4DrvNRUtilIIR(&_rNrIIR[E_NR_IIR_HOMO], 8, MIN((u4HomoSum>>8), 0x00FFFFFF));
    u4HomoSum = (u4HomoSum >> u4Shift);
	
    if (u4HomoSum < u1MoMid)
        u4MoIdx = LERP_F_C(u1MoMin, 0, u1MoMid, u1PosMid, u4HomoSum);
    else
        u4MoIdx = LERP_F_C(u1MoMid, u1PosMid, u1MoMax, NR_MOIDX_MAX, u4HomoSum);

	//LOG(3, "Homo %d Shift %d Min %d  Mid %d  Max %d  Idx %d\n", 
	//	u4HomoSum, u4Shift, u1MoMin, u1MoMid, u1MoMax, u4MoIdx);

    _rNmAna.u1MoIdx = u4MoIdx;

    _SWW(NR_MO_STA_01, u4HomoSum, STA_MONR_HOMOSUM);
    _SWW(NR_MO_STA_01, u4MoIdx, STA_MO_IDX);
}

static void _vDrvMCNRWriteSADHistToSWReg(const UINT16 arHist[SAD_HIST_ENTRY_NUM])
{
    UINT8  i = 0;
    UINT32 u4SWAddr = NR_NMSTA_00;

    for (i = 0; i < SAD_HIST_LAST; i += 2)
    {
        _SWW_4B(u4SWAddr, ((UINT32) arHist[i]<<16) | arHist[i+1]);
        u4SWAddr += 4;
    }
    _SWW(NR_NMSTA_0C, arHist[SAD_HIST_LAST], SADHIST24);
}

static void _vDrvMCNRLogSADHist(const UINT16 arHist[SAD_HIST_ENTRY_NUM])
{
    UINT8  i = 0;

    for (i = 0; i < SAD_HIST_LAST; i ++)
    {
        LOG(4, "arHist[%d]=%d\n", i, arHist[i]);
    }
}

static void _vDrvMCNRGetNMParam(void)
{    
    _rNmPrm.u4Cfg = 
        (_SWR(NR_NM_00, FORCE_NOISE_LVL_EN) << NM_CFG_FORCE_NL)|
        (_SWR(NR_NM_03, UPDATE_NL_SC) << NM_CFG_UPDATE_NL_SC)|
        (_SWR(NR_NM_0F, NM_ANTI_NL_GAIN_EN) << NM_CFG_ANTI_NL_GAIN)|
        (_SWR(NR_NM_0F, NM_ADJ_BY_FATTAIL) << NM_CFG_ADJ_FATTAIL)|
        (_SWR(NR_NM_10, NM_ADJ_BY_MOFBK) << NM_CFG_ADJ_MOFBK)|
        (_SWR(NR_NM_10, NM_ADJ_BY_MOFBK_USE_HOMO) << NM_CFG_ADJ_MOFBK_HOMO)|
        (_EN(EN_GROUP_HIST) << NM_CFG_HIST_GP)|
        (_SWR(NR_NM_0F, NM_STABLIZER_IIR_EN) << NM_CFG_STABLIZER_IIR);

    _rNmPrm.u1SADHistIIR        = _SWR(NR_NM_00, HIST_TEMP_IIR_WEI);
    _rNmPrm.u1SADHistSubSample  = _SWR(NR_NM_00, HIST_TEMP_IIR_SUBSAMPLE);
    _rNmPrm.u1SADHistShowMode   = _SWR(NR_NM_00, HIST_DBG_MODE);
    _rNmPrm.u1ExtNLMode         = _SWR(NR_NM_06, NM_EXT_NL_MODE);
    _rNmPrm.u1ForceNoiseLvl     = _SWR(NR_NM_00, FORCE_NOISE_LVL);
    _rNmPrm.u1MoFbkOfst         = _SWR(NR_NM_10, NM_ADJ_BY_MOFBK_OFST);

    _rNmPrm.rNrErfb.i2IncTh     = _SWR(NR_NM_0D, NM_ERFB_INC);
    _rNmPrm.rNrErfb.i2DecTh     = _SWR(NR_NM_0D, NM_ERFB_DEC);
    _rNmPrm.rNrErfb.u1IncOfst   = _SWR(NR_NM_0E, NM_ERFB_INC_OFST);
    _rNmPrm.rNrErfb.u1SubSample = _SWR(NR_NM_0F, NM_ERFB_SUBSAMPLE);
    _rNmPrm.rNrErfb.u1SlowErfb  = _EN(EN_SLOW_ERFB);

    _rNrIIR[E_NR_IIR_NL].u1Wei  = _SWR(NR_NM_10, NM_STABLIZER_IIR);
}

void vDrvNRNoiseMeterProc(const NR_NM_STA_T* prNmSta, NR_NM_ANA_T* prNmAna)
{
    _vDrvMCNRGetNMParam();

    _vDrvMCNRSADHistPreProc(prNmSta);
    _vDrvMCNRFrameLevelFallback(prNmSta);
    _vDrvMCNRCalcMoIdx(prNmSta);
    _vDrvMCNRSADAnalysis(prNmSta);
    //_vDrvMCNRCleanDiffLvl(void);

    // Debug SW status
    switch (_rNmPrm.u1SADHistShowMode)
    {
    case 2:
        _vDrvMCNRWriteSADHistToSWReg(_arSADHistGp);
        break;
    case 1:
        _vDrvMCNRWriteSADHistToSWReg(prNmSta->arSADHistCurr);
        break;
    default:
    case 0:
        _vDrvMCNRWriteSADHistToSWReg(_arSADHistFilt);
        break;
    }

    if (prNmAna)
        x_memcpy(prNmAna, &_rNmAna, sizeof(NR_NM_ANA_T));
}

void vDrvNRNoiseMeterInit(void)
{
    UINT32 i;
    x_memset(&_rNmPrm, 0, sizeof(NR_NM_PARAM_T));
    x_memset(&_rNmAna, 0, sizeof(NR_NM_ANA_T));
    
    for (i = 0; i < E_NR_Q1B_NUM; i++)
        Q_INIT(_rNrQ1B[i], NR_FILT_FIFO_NUM);
    for (i = 0; i < E_NR_Q2B_NUM; i++)
        Q_INIT(_rNrQ2B[i], NR_FILT_FIFO_NUM);
    for (i = 0; i < E_NR_Q4B_NUM; i++)
        Q_INIT(_rNrQ4B[i], NR_FILT_FIFO_NUM);
    for (i = 0; i < E_NR_IIR_NUM; i++)
        x_memset(&_rNrIIR[i], 0, sizeof(NR_IIR_T));

    _rNmPrm.rNrErfb.u1StaMax    = 63;
    _rNmPrm.rNrErfb.u1SysSta    = 0;
    _rNmPrm.rNrErfb.u1IncOfst   = 1;
    _rNmPrm.rNrErfb.u1SubSample = 1;
    _rNmPrm.rNrErfb.u1SlowErfb  = 0;
    _rNmPrm.rNrErfb.i2DiffAccm  = 0;
    _rNmPrm.rNrErfb.i2IncTh     = 8;
    _rNmPrm.rNrErfb.i2DecTh     = -8;

    _vDrvMCNRGetNMParam();
}

void NR_FW_NM_Process(const NR_NM_STA_T* prNmSta, NR_NM_ANA_T* prNmAna)
{
    vDrvNRNoiseMeterProc(prNmSta, prNmAna);
}

void NR_FW_NM_Init(void)
{
    vDrvNRNoiseMeterInit();
}

