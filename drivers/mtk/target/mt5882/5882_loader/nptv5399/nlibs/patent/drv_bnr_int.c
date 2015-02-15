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
 * $RCSfile: drv_bnr_int.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifdef _C_MODEL_
#include "drv_bnr_int.h"
#include "drv_common.h"
#include "nxnr.h"
#include "console.h"

#include <string.h>
#else
#include "drv_nr.h"
#include "drv_nr_int.h"
#include "drv_bnr_int.h"
#include "hw_nr.h"
#include "nr_debug.h"
#include "hw_sw.h"

#include "x_os.h"
#include "x_assert.h"

//#define BNR_MUTEX_LOCK                  (VERIFY(x_sema_lock(_hBNRMutex, X_SEMA_OPTION_WAIT) == OSR_OK))
//#define BNR_MUTEX_UNLOCK                (VERIFY(x_sema_unlock(_hBNRMutex) == OSR_OK))
#endif

#ifndef BNR_NEW_FW

/*
 * control points
 */
typedef struct
{
    INT32 iHGridMinLineSumTh;
    INT32 iHGridActiveTh;
    INT32 iHGridInactiveTh;
    INT32 iHBlkLevelActiveTh;
    INT32 iHBlkLevelConfirmTh;
    INT32 iHBlkLevelLeakage;
    INT32 iHBlkInsideOutsideRatio;
    INT32 iHBlkActiveTrialTh;

    INT32 iVGridMinLineSumTh;
    INT32 iVGridActiveTh;
    INT32 iVGridInactiveTh;
    INT32 iVBlkLevelActiveTh;
    INT32 iVBlkLevelConfirmTh;
    INT32 iVBlkLevelLeakage;
    INT32 iVBlkInsideOutsideRatio;
    INT32 iVBlkActiveTrialTh;
    INT32 iVBlkGaugeIncTh;

    INT32 iVBlkWindowTrialCountTh;
    INT32 iVBlkScoreTh;

    INT32 iSizeFactor;
    INT32 iSizeFactorH;

#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    UINT8 u1InitRtoNum;
    UINT8 u1InitRtoDen;
#endif

    BOOL  fgIgnoreNW;
    BOOL  fgUseR;
    BOOL  fgEnLocWrite;
    BOOL  fgPatchCG;
} NXNR_BNR_CTRL_PT_T;

typedef struct
{
    UINT8  u1LogMode;
    UINT32 u4LogCnt;
} BNR_LOG_CFG_T;

/*
 * global variables
 */
static UINT8 _u1Init = 0;

static INT32 _iHeight;
static INT32 _iWidth;

static INT32 _iBnrFwStop = 0;
static INT32 _iBnrFwStateOnStop;
static INT32 _iBnrFwState;

#ifdef _C_MODEL_
static INT32 _arVBlkLineSum[_NXNR_BNR_FW_MAX_WIDTH];
static INT32 _arHBlkLineSum[_NXNR_BNR_FW_MAX_HEIGHT];
static INT32 iVBlkPos[_NXNR_BNR_FW_MAX_WIDTH];
static INT32 iHBlkPos[_NXNR_BNR_FW_MAX_HEIGHT];
#else
static UINT16 _arVBlkLineSum[_NXNR_BNR_FW_MAX_WIDTH];
static UINT16 _arHBlkLineSum[_NXNR_BNR_FW_MAX_HEIGHT];
#endif
static UINT8  _arVBlkLineSumSlice[_NXNR_BNR_FW_MAX_WIDTH];

static UINT16 _u2VBlkPixCnt;
static UINT16 _u2HBlkPixCnt;

static UINT8 _u1MoIdx;
static UINT8 _u1VBlkAvgLS;
static UINT8 _u1HBlkAvgLS;

static UINT8 _au1VBlkPosGauge[_NXNR_BNR_FW_MAX_WIDTH];
static UINT8 _au1HBlkPosGauge[_NXNR_BNR_FW_MAX_HEIGHT];

static UINT8 _au1VBlkPeriodArray[_NXNR_BNR_FW_MAX_WIDTH];
static UINT8 _au1HBlkPeriodArray[_NXNR_BNR_FW_MAX_HEIGHT];

static INT32 _iHBlkPeriodHist[_NXNR_BNR_FW_MAX_PERIOD+2];
static INT32 _iVBlkPeriodHist[_NXNR_BNR_FW_MAX_PERIOD+2];

static INT32 _iHBlkLevel;
static INT32 _iVBlkLevel;

static INT32 _iHBlkConfidence;
static INT32 _iVBlkConfidence;

static INT32 _iHBlkActiveTrialCount;
static INT32 _iVBlkActiveTrialCount;

static INT32 _iHBlkSavedPeriod;
static INT32 _iVBlkSavedPeriod;

static INT32 _iHBlkSavedGroup;
static INT32 _iVBlkSavedGroup;

static INT32 _iHBlkBestPeriod;
static INT32 _iVBlkBestPeriod;
static INT32 _iHBlkBestPeriodCnt;
static INT32 _iVBlkBestPeriodCnt;
static INT32 _iVGridMinLsTh = 32;

#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
static UINT8 _u1FoundSizePhs;
static BNR_DESC_T _rBnrDesc;
#endif

#ifndef _C_MODEL_
//static HANDLE_T _hBNRMutex = (HANDLE_T)NULL;
#endif

static void _BNR_FW_FindGroup ( INT32 _iBestPeriod, INT32 * _iGroup, INT32 * _iMember );
static void _BNR_FW_SetControlPoints(void);
static void _BNR_FW_ResetHGridDet(void);
static void _BNR_FW_ResetVGridDet(void);
static void _BNR_FW_HGridDet(void);
static void _BNR_FW_VGridDet(void);
static void _BNR_FW_ReadLineSum(void);
static void _BNR_FW_CopyLineSumTool(void);
static void _BNR_FW_WriteBlkPos(void);
static void _BNR_FW_WriteReg(void);
static void _BNR_FW_WriteRegOnce(void);

static BNR_LOG_CFG_T _rBnrLog = {0, 0};

NXNR_BNR_CTRL_PT_T rBnrCpt;

static const UINT8 _iVBlkGroup[_NXNR_VBLK_GROUP_COUNT][_NXNR_VBLK_GROUP_MEMBER_COUNT] =
{
    { 7,  8,  9, 15, 16, 17, 31, 32, 33}, // group 1
    {10, 21, 32,  0,  0,  0,  0,  0,  0}  // group 2
};

/*
 * for C-model only variables
 */
#ifdef _C_MODEL_
CNXNR * pNXNR;
static char szPatternName[256];
#else
EXTERN void vDrvNXNRCopyLineSumTool(void);
EXTERN UINT8 u1DrvNXNRWriteBlkPos(void);
EXTERN UINT8 u1DrvNXNRReadBlkLineSum(void);
EXTERN void vDrvNXNRWritePos(BOOL u1IsH, UINT32 u4Pos, UINT8 u1OnOff);
#endif

/*
 * functions
 */
static INT32 clip ( const INT32 lb, const INT32 ub, const INT32 val )
{
    if ( val < lb )
        return lb;
    else if ( val > ub )
        return ub;
    else
        return val;
}

static void _BNR_FW_FindGroup ( INT32 _iBestPeriod, INT32 * _iGroup, INT32 * _iMember )
{
    INT32 g, m;

    if ( _iBestPeriod == 0 )
    {
        *_iGroup = 0;
        *_iMember = 0;
        return;
    }

    for ( g = 0; g < _NXNR_VBLK_GROUP_COUNT; g++ )
    {
        for ( m = 0; m < _NXNR_VBLK_GROUP_MEMBER_COUNT; m++ )
        {
            if ( _iBestPeriod == _iVBlkGroup[g][m] )
            {
                *_iGroup = g + 1;
                *_iMember = m;
                return;
            }
        }
    }
}

static void _BNR_FW_ResetHGridDet(void)
{
    x_memset(_au1HBlkPosGauge, 0, sizeof(UINT8) * _NXNR_BNR_FW_MAX_HEIGHT);
    _iHBlkLevel = 0;
    _iHBlkConfidence = 0;
    _iHBlkActiveTrialCount = 0;
    _iHBlkSavedPeriod = 0;
}

static void _BNR_FW_ResetVGridDet(void)
{
    x_memset(_au1VBlkPosGauge, 0, sizeof(UINT8) * _NXNR_BNR_FW_MAX_WIDTH);
    _iVBlkLevel = 0;
    _iVBlkConfidence = 0;
    _iVBlkActiveTrialCount = 0;
    _iVBlkSavedPeriod = 0;
}

static void _BNR_FW_HGridDet(void)
{
    // slice and get period histogram
    UINT32 u4HGridMinLineSumTh = rBnrCpt.iHGridMinLineSumTh;
    UINT32 u4SliceSum = 0;
    INT32 iArtiRej = 0;
    INT32 iPrevHGridPos = 0;
    INT32 iBestPeriod = 0;
    INT32 iBestPeriodCnt = 0;
    INT32 iBestPeriodPct = 0;
    INT32 iGroup = 0;
    INT32 iMember = 0;
    INT32 iLineSumInside = 0;
    INT32 iLineSumOutside = 0;
    INT32 iLineSumDiff = 0;
    INT32 iDeltaLevel = 0;
    INT32 iBlkConf = 0;
    INT32 y, h, i, j, p;
    BOOL fgMatchPeriod;

    x_memset( _iHBlkPeriodHist, 0, sizeof(INT32) * 35 );
    x_memset( _au1HBlkPeriodArray, 0, sizeof(UINT8) * _NXNR_BNR_FW_MAX_HEIGHT );

    h = _iHeight;

    // for _arHBlkLineSum[-30:+30]
    for (i = 1; i <= 30; i++ )
    {
        u4SliceSum += MAX ( (UINT32) _arHBlkLineSum[i], u4HGridMinLineSumTh );
    }

    u4SliceSum = 
        (u4SliceSum << 1)  /*for _arHBlkLineSum[-30:+30]*/
        + MAX ( (UINT32) _arHBlkLineSum[0], u4HGridMinLineSumTh )  /*_arHBlkLineSum[0]*/
        + MAX ( (UINT32) _arHBlkLineSum[31], u4HGridMinLineSumTh ) /*_arHBlkLineSum[-31]*/
        + MAX ( (UINT32) _arHBlkLineSum[32], u4HGridMinLineSumTh ) /*_arHBlkLineSum[-32]*/
        + MAX ( (UINT32) _arHBlkLineSum[33], u4HGridMinLineSumTh ) /*_arHBlkLineSum[-33]*/;

    for (y = 0; y < h; y++ )
    {
        INT32 u4SliceLv;

        // on-the-fly find slicing level by average of _arHBlkLineSum[-32:+31]
        // use mirror for boundary padding
        // update slicing window
        i = y + 31;

        if ( i >= h )
        {
            i = 2 * ( h - 1 ) - i;
        }

        u4SliceSum += MAX ( (UINT32) _arHBlkLineSum[i], u4HGridMinLineSumTh );
        j = y - 33;

        if ( j < 0 )
        {
            j = -j;
        }

        u4SliceSum -= MAX ( (UINT32) _arHBlkLineSum[j], u4HGridMinLineSumTh );
        u4SliceLv = ( u4SliceSum >> 6 );

        // slicing, update period and period histogram
        if ( _arHBlkLineSum[y] > u4SliceLv && 
             _arHBlkLineSum[y] > MAX(_arHBlkLineSum[MAX(y,1)-1], _arHBlkLineSum[MIN(y+1, h-1)]))
        {
            INT32 iPeriod = y - iPrevHGridPos;
            iPrevHGridPos = y;

            if ( iPeriod < _NXNR_BNR_FW_MIN_PERIOD )
            {
                // period is too small
                _iHBlkPeriodHist[0]++;
            }
            else if ( iPeriod > _NXNR_BNR_FW_MAX_PERIOD + 1 )
            {
                // period is too large
                _iHBlkPeriodHist[1]++;
            }
            else
            {
                _iHBlkPeriodHist[iPeriod]++;
                _au1HBlkPeriodArray[y] = (UINT8) iPeriod;
            }
        }
    }


    // find initial period
    for (p = _NXNR_BNR_FW_MIN_PERIOD; p <= _NXNR_BNR_FW_MAX_PERIOD; p++)
    {
        INT32 iTestPeriodCnt = _iHBlkPeriodHist[p] + _iHBlkPeriodHist[p+1];
        INT32 iTestPeriodPct = iTestPeriodCnt * p; //iTestPeriodCnt * 100 / ( h / p );

        if ( iTestPeriodPct > iBestPeriodPct )
        {
            iBestPeriod = p;
            iBestPeriodCnt = iTestPeriodCnt;
            iBestPeriodPct = iTestPeriodPct;
        }
    }

    _iHBlkBestPeriod = iBestPeriod;
    _iHBlkBestPeriodCnt = iBestPeriodCnt;
    _BNR_FW_FindGroup ( iBestPeriod, & iGroup, & iMember );
    fgMatchPeriod = ( iBestPeriod != 0 ) && ( iGroup == _iHBlkSavedGroup || iBestPeriod == _iHBlkSavedPeriod );

    // update grid position gauge
    if ( fgMatchPeriod )
    {
        for (y = 0; y < h; y++)
        {
            BOOL fgCond0 = ( _au1HBlkPeriodArray[y] == iBestPeriod );
            BOOL fgCond1 = ( _au1HBlkPeriodArray[y] == ( iBestPeriod + 1 ) );
            BOOL fgCond2 = ( ( y+iBestPeriod < h ) && ( _au1HBlkPeriodArray[y+iBestPeriod] == iBestPeriod ) );
            BOOL fgCond3 = ( ( y+iBestPeriod+1 < h ) && ( _au1HBlkPeriodArray[y+iBestPeriod+1] == ( iBestPeriod + 1 ) ) );

            if ( fgCond0 || fgCond1 || fgCond2 || fgCond3 )
            {
                if ( _au1HBlkPosGauge[y] < 15 )
                {
                    _au1HBlkPosGauge[y]++;
                }

                iLineSumInside += _arHBlkLineSum[y];
            }
            else
            {
                if ( _au1HBlkPosGauge[y] > 0 )
                {
                    _au1HBlkPosGauge[y]--;
                }

                iLineSumOutside += _arHBlkLineSum[y];
            }
        }
    }
    else
    {
        for (y = 0; y < h; y++ )
        {
            if ( _au1HBlkPosGauge[y] > 0 )
            {
                _au1HBlkPosGauge[y]--;
            }
        }
    }

    // update block level
    iArtiRej = (_iHBlkPeriodHist[0]*_NXNR_BNR_FW_MIN_PERIOD + _iHBlkPeriodHist[1]*_NXNR_BNR_FW_MAX_PERIOD);
    iArtiRej = ((iBestPeriodPct - iArtiRej) * rBnrCpt.iSizeFactorH) / 16;
#if 0
    iLineSumDiff = iLineSumInside * rBnrCpt.iHBlkInsideOutsideRatio * iBestPeriod / 8 - iLineSumOutside;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactorH ) >> ( 8 + 4 ) ); // iLineSumDiff could be negative
#else
    iLineSumDiff = (iBestPeriodCnt*rBnrCpt.iHBlkInsideOutsideRatio*2 - (h / MAX(iBestPeriod, 1)))/4;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactorH ) /16 ); // iLineSumDiff could be negative
#endif

    if ( fgMatchPeriod )
    {
        _iHBlkLevel += ((iDeltaLevel*_u1MoIdx)/4); // delta could be positive or negative
    }
    else if ( _iHBlkLevel < rBnrCpt.iHBlkLevelConfirmTh )
    {
        _iHBlkLevel = 0;
    }
    else
    {
        _iHBlkLevel -= MAX(0, iDeltaLevel);
    }

    _iHBlkLevel -= rBnrCpt.iHBlkLevelLeakage;
    _iHBlkLevel += (rBnrCpt.fgPatchCG && iArtiRej < 0 ? iArtiRej*iBestPeriod : 0);
    _iHBlkLevel = clip ( 0, 2047, _iHBlkLevel );

    if ( _iHBlkLevel == 0 )
    {
        _iHBlkSavedPeriod = iBestPeriod;
        _iHBlkSavedGroup = iGroup;
    }

    // must active within limited time period
    if ( _iHBlkLevel < rBnrCpt.iHBlkLevelActiveTh )
    {
        _iHBlkActiveTrialCount++;
    }
    else
    {
        _iHBlkActiveTrialCount = 0;
    }

    if ( _iHBlkActiveTrialCount >= rBnrCpt.iHBlkActiveTrialTh )
    {
        _BNR_FW_ResetHGridDet ();
    }

    iBlkConf = ( clip ( 0, 1023, _iHBlkLevel - rBnrCpt.iHBlkLevelActiveTh ) + 63 ) / 64;
    
    if (iBlkConf < _iHBlkConfidence)
    {
        _iHBlkConfidence --;
    }
    else if (iBlkConf > _iHBlkConfidence)
    {
        _iHBlkConfidence ++;
    }

    // set grid position
    for (y = 0; y < h; y++ )
    {
        if ( _au1HBlkPosGauge[y] > rBnrCpt.iHGridActiveTh )
        {
        #ifdef _C_MODEL_
            iHBlkPos[y] = 0x3;
        #else
            vDrvNXNRWritePos(1, y, 1);
        #endif    
        }
        else if ( _au1HBlkPosGauge[y] < rBnrCpt.iHGridInactiveTh )
        {
        #ifdef _C_MODEL_
            iHBlkPos[y] = 0x0;
        #else
            vDrvNXNRWritePos(1, y, 0);
        #endif    
        }
    }
}

#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT

typedef struct
{
    UINT32 u4Idx;
    UINT32 u4Val;
} BNR_SP_HIST_T;

typedef struct
{
    UINT32 u4MaxGrp;
    UINT32 u4MinGrp;
    //UINT32 u4Inside;
    //UINT32 u4Outside;
} BNR_SG_NODE_ANA_T;

// size = period * number_of_grid + residul = p*n+r
// size / n = p, size % n = r
// GCD(n, r) = 1
static UINT8 _rBNRSP[NUM_NODE] =
{
    BNR_NODE(1,0),
    BNR_NODE(2,1),
    BNR_NODE(3,1), BNR_NODE(3,2),
    BNR_NODE(4,1), BNR_NODE(4,3),
    BNR_NODE(5,1), BNR_NODE(5,2), BNR_NODE(5,3), BNR_NODE(5,4),
    BNR_NODE(6,1), BNR_NODE(6,5),
    BNR_NODE(7,1), BNR_NODE(7,2), BNR_NODE(7,3), BNR_NODE(7,4), BNR_NODE(7,5), BNR_NODE(7,6),
    BNR_NODE(8,1), BNR_NODE(8,3), BNR_NODE(8,5), BNR_NODE(8,7),
    BNR_NODE(9,1), BNR_NODE(9,2), BNR_NODE(9,4), BNR_NODE(9,5), BNR_NODE(9,7), BNR_NODE(9,8)
};

static UINT32 _rLsHist[BNR_MAX_SIZE];
static BNR_SP_HIST_T _rHist[BNR_MAX_SIZE];
static BNR_SP_HIST_T _rBestHist[BNR_MAX_SIZE];

static INT32 _vCmpBnrSpHistVal(const void* a, const void* b)
{
    return (((BNR_SP_HIST_T*)a)->u4Val) < (((BNR_SP_HIST_T*)b)->u4Val) ? 1 : -1;
}

static INT32 _vCmpBnrSpHistIdx(const void* a, const void* b)
{
    return (((BNR_SP_HIST_T*)a)->u4Idx) > (((BNR_SP_HIST_T*)b)->u4Idx) ? 1 : -1;
}

static INT32 _BNR_FW_VGridForceLocAssist(const BNR_DESC_T* pBnrDesc, UINT32 u4Width, UINT8 u1IncTh, UINT8* pau1PosGauge)
{
    UINT32 s   = pBnrDesc->u4S; 
    UINT32 idx = pBnrDesc->u4StartIdx;
    UINT32 p   = pBnrDesc->u4P;

    UINT32 u4CurP = 0, n, i, j;
    UINT8  u1RefSeq[MAX_NUM_GRID];

    if (p == 0 || s == 0 || (p > s) || (u4Width <= pBnrDesc->u4Start))
    {
        LOG(3, "BNR VGridForce Wrong Argument!\n");
        return 1;
    }

    n = s / p;
    if (n > MAX_NUM_GRID)
    {
        LOG(3, "BNR VGridForce Wrong Argument!\n");
        return 1;
    }

    for (i = 0; i < n; i++)
        u1RefSeq[i] = pBnrDesc->u1PSeq[i];

    for (i = 0; i < pBnrDesc->u4Start; i++)
    {
        pau1PosGauge[i] -= MIN(pau1PosGauge[i], 1);
    }

    j = u4CurP;
    for (i = pBnrDesc->u4Start; i < u4Width; i++)
    {
        if (j == u4CurP)
        {
            pau1PosGauge[i] = (UINT8) MIN(15, (pau1PosGauge[i]+u1IncTh));
            j = 0;
            u4CurP = u1RefSeq[idx];
            idx = (idx+1);
            if (idx == n)
                idx = 0;
        }
        else
        {
            pau1PosGauge[i] -= MIN(pau1PosGauge[i], 1);
        }
        j++;
    }

    return 0;
}

static UINT8 _BNR_FW_VGridSizePhase(UINT32 u4BestP, BOOL fgIsBestPIntSize, BNR_DESC_T* pDesc)
{
    UINT8 u1Found = 0;
    UINT32 u4BestSize = 0, u4BestNode = 0;
    UINT32 u4VGridMinLineSumTh = _iVGridMinLsTh;
    INT32 node, w = _iWidth;
    BNR_SG_NODE_ANA_T rBnrBestAna = {2, 1};

    rBnrBestAna.u4MaxGrp = rBnrCpt.u1InitRtoNum;
    rBnrBestAna.u4MinGrp = rBnrCpt.u1InitRtoDen;

    // mod operation
    for (node = NUM_NODE-1; node >= 0; node--)
    {
        UINT8 u1CurNode;
        INT32 i, numOfGrid, s, numOfSort;
        UINT8 *prVBlkLs;
        UINT32 u4Min;
        BNR_SP_HIST_T *prHist, *prHistCur, *prHistEnd;
        BNR_SG_NODE_ANA_T rBnrCurAna = {0,0};

        u1CurNode = _rBNRSP[node];
        numOfGrid = BNR_NODE_N(u1CurNode);
        s = BNR_SIZE(u4BestP, u1CurNode);
        numOfSort = MIN(s, numOfGrid*2);

        if (s > BNR_MAX_SIZE || (fgIsBestPIntSize && (numOfGrid != 1) && ((u4BestP & 0x7) == 0)))
            continue;

        prVBlkLs = _arVBlkLineSumSlice; //_arVBlkLineSum;
        prHist = _rHist;
        prHistCur = prHist;
        prHistEnd = prHist + s;

        // init idx
        for (i = 0; i < s; i++)
        {
            prHist[i].u4Idx = i;
            prHist[i].u4Val = 0;
        }

        // mod operation
        //SYS_MEASURE_TIME_START(SV_BNR_ACTIVE_TIMER);
        for (i = w-1; i >= 0; i--)
        {
            UINT32 u4Ls = *prVBlkLs++;
            prHistCur->u4Val += u4Ls; //(u4Ls > u4VGridMinLineSumTh ? (u4Ls-u4VGridMinLineSumTh) : 0);
            prHistCur ++;
            if (prHistCur == prHistEnd)
                prHistCur = prHist;
        }
        //SYS_MEASURE_TIME_END(SV_BNR_ACTIVE_TIMER);
        //SYS_STORE_TIME_INFO_TO_REG(SV_BNR_ACTIVE_TIMER);

        // search max bins, sorting
        _vSelectSortK(BNR_SP_HIST_T, prHist, (UINT32)s, _vCmpBnrSpHistVal, (UINT32)numOfSort);
        
        for (i = 0; i < numOfGrid; i++)
            rBnrCurAna.u4MaxGrp += prHist[i].u4Val;
        for (i = numOfGrid; i < numOfSort; i++)
            rBnrCurAna.u4MinGrp += prHist[i].u4Val;

        //rBnrCurAna.u4Inside  = u4VGridMinLineSumTh*rBnrCurAna.u4MaxGrp; // rBnrCurAna.u4MaxGrp;
        //rBnrCurAna.u4Outside = u4VGridMinLineSumTh*rBnrCurAna.u4MinGrp; // rBnrCurAna.u4MinGrp;
        
        u4Min = MAX(rBnrCurAna.u4MinGrp, (UINT32)1);
        if (rBnrCurAna.u4MaxGrp*rBnrBestAna.u4MinGrp >= rBnrBestAna.u4MaxGrp*u4Min)
        {
            u4BestSize = s;
            u4BestNode = u1CurNode;
            rBnrBestAna.u4MinGrp = u4Min;
            rBnrBestAna.u4MaxGrp = rBnrCurAna.u4MaxGrp;
            //rBnrBestAna.u4Inside = rBnrCurAna.u4Inside;
            //rBnrBestAna.u4Outside = rBnrCurAna.u4Outside;
            x_memcpy(_rBestHist, prHist, sizeof(BNR_SP_HIST_T)*s);
        }
    }

    // calculate LS inside/outside
    if (u4BestSize != 0)
    {   
        UINT32 u4LsIn = 0, u4LsOut = 0;
        INT32 i, numOfGrid, numOfSort;
        UINT32 *prLsHist, *prLsHistEnd, *prLsHistCur;
        UINT16 *prVBlkLsum;

        numOfGrid = BNR_NODE_N(u4BestNode);
        numOfSort = MIN((INT32)u4BestSize, numOfGrid*2);
        prVBlkLsum = _arVBlkLineSum;
        prLsHist = _rLsHist;
        prLsHistEnd = prLsHist + u4BestSize;

        prLsHistCur = prLsHist;
        for (i = u4BestSize-1; i >= 0; i--)
        {
            *prLsHistCur++ = 0;
        }

        prLsHistCur = prLsHist;
        for (i = w-1; i >= 0; i--)
        {
            UINT32 u4Ls = *prVBlkLsum++;
            *prLsHistCur += (u4Ls > u4VGridMinLineSumTh ? (u4Ls-u4VGridMinLineSumTh) : 0);
            prLsHistCur ++;
            if (prLsHistCur == prLsHistEnd)
                prLsHistCur = prLsHist;
        }
        
        for (i = 0; i < numOfGrid; i++)
            u4LsIn += prLsHist[_rBestHist[i].u4Idx];
        for (i = numOfGrid; i < numOfSort; i++)
            u4LsOut += prLsHist[_rBestHist[i].u4Idx];

        pDesc->u4LsInside = u4LsIn;
        pDesc->u4LsOutside = u4LsOut;
    }


    // search sequence order
    if (u4BestSize == 0 || (u4BestSize == u4BestP && !fgIsBestPIntSize))
    {
        u1Found = 0;
    }
    else
    {
        INT32 r, i, j, pos_int, pos_fr, pos;
        INT32 numOfGrid = BNR_NODE_N(u4BestNode);
        BNR_SP_HIST_T* prHist = _rBestHist;
        UINT32 u4RefSeq[MAX_NUM_GRID];
        UINT32 u4CurSeq[MAX_NUM_GRID];

        // ASSERT(numOfGrid <= MAX_NUM_GRID);
        
        _vInsertSort(BNR_SP_HIST_T, prHist, (UINT32)numOfGrid, _vCmpBnrSpHistIdx);

        for (i = 0; i < numOfGrid; i++)
        {
            pDesc->u1GridPos[i] = prHist[i].u4Idx;
        }        
        pDesc->u4P = u4BestP;
        pDesc->u4S = u4BestSize;
        pDesc->u4Start = prHist[0].u4Idx;
        pDesc->u4StartIdx = 0;
        u1Found = 2;
        
        r = BNR_NODE_R(u4BestNode);
        pos_int = 0;
        pos_fr = 0;
        pos = 0;
        for (i = 0; i < numOfGrid; i++)
        {
            pos_int += u4BestP;
            pos_fr += r;
            if (pos_fr >= numOfGrid)
            {
                pos_fr -= numOfGrid;
                pos_int += 1;
            }
            if (2*pos_fr >= numOfGrid)
            {
                u4RefSeq[i] = (pos_int+1) - pos;
                pos = (pos_int+1);
            }
            else
            {
                u4RefSeq[i] = (pos_int) - pos;
                pos = pos_int;
            }
        }

        u4CurSeq[numOfGrid-1] = (prHist[0].u4Idx+u4BestSize - prHist[numOfGrid-1].u4Idx);
        for (i = 0; i < numOfGrid-1; i++)
        {
            u4CurSeq[i] = (prHist[i+1].u4Idx - prHist[i].u4Idx);
        }

        // matching
        for (i = 0; i < numOfGrid; i++)
        {
            INT32 iCntMatch = 0;
            for (j = 0; j < numOfGrid; j++)
            {
                INT32 k = i+j;
                if (k >= numOfGrid)
                    k -= numOfGrid;
                if (u4RefSeq[k] == u4CurSeq[j])
                {
                    iCntMatch++;
                }
                else
                {
                    break;
                }
            }
            if (iCntMatch == numOfGrid)
            {
                pDesc->u4StartIdx = i;
                for (j = 0; j < numOfGrid; j++)
                    pDesc->u1PSeq[j] = u4RefSeq[j];
                u1Found = 1;
                break;
            }
        }
    }

    return u1Found;
}
#endif //SUPPORT_BNR_SIZE_PHASE_PREDICT

static void _BNR_FW_VGridDet (void)
{ 
    // slice and get period histogram
    UINT32 u4VGridMinLineSumTh = _iVGridMinLsTh;
    UINT32 u4SliceSum = 0;
    INT32 iArtiRej = 0;
    INT32 iPrevVGridPos = 0;
    INT32 iBestPeriod = 0;
    INT32 iBestPeriodCnt = 0;
    INT32 iBestPeriodPct = 0;
    INT32 iGroup = 0;
    INT32 iMember = 0;
    INT32 iLineSumInside = 0;
    INT32 iLineSumOutside = 0;
    INT32 iLineSumDiff = 0;
    INT32 iDeltaLevel = 0;
    INT32 iBlkConf = 0;
    INT32 x, w, i, j, p;
    BOOL fgMatchPeriod;
#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    UINT8 u1FoundSizePhase = 0;
    BNR_DESC_T bnrDesc;

    x_memset ( &bnrDesc, 0, sizeof(BNR_DESC_T));
#endif //SUPPORT_BNR_SIZE_PHASE_PREDICT

    x_memset ( _iVBlkPeriodHist, 0, sizeof(INT32) * 35 );
    x_memset ( _au1VBlkPeriodArray, 0, sizeof(UINT8) * _NXNR_BNR_FW_MAX_WIDTH);

    w = _iWidth;
    iBestPeriodPct = w / 8;

    // for _arVBlkLineSum[-30:+30]
    for (i = 1; i <= 30; i++ )
    {
        u4SliceSum += MAX ( (UINT32) _arVBlkLineSum[i], u4VGridMinLineSumTh );
    }

    u4SliceSum = 
        (u4SliceSum << 1)  /*for _arVBlkLineSum[-30:+30]*/
        + MAX ( (UINT32) _arVBlkLineSum[0], u4VGridMinLineSumTh )  /*_arVBlkLineSum[0]*/
        + MAX ( (UINT32) _arVBlkLineSum[31], u4VGridMinLineSumTh ) /*_arVBlkLineSum[-31]*/
        + MAX ( (UINT32) _arVBlkLineSum[32], u4VGridMinLineSumTh ) /*_arVBlkLineSum[-32]*/
        + MAX ( (UINT32) _arVBlkLineSum[33], u4VGridMinLineSumTh ) /*_arVBlkLineSum[-33]*/;

    for (x = 0; x < w; x++ )
    {
        INT32 u4SliceLv;

        // on-the-fly find slicing level by average of _arVBlkLineSum[-32:+31]
        // use mirror for boundary padding
        // update slicing window
        i = x + 31;

        if ( i >= w )
        {
            i = 2 * ( w - 1 ) - i;
        }

        u4SliceSum += MAX ( (UINT32) _arVBlkLineSum[i], u4VGridMinLineSumTh );
        j = x - 33;

        if ( j < 0 )
        {
            j = -j;
        }

        u4SliceSum -= MAX ( (UINT32) _arVBlkLineSum[j], u4VGridMinLineSumTh );
        u4SliceLv = ( u4SliceSum >> 6 ) + 16;

        _arVBlkLineSumSlice[x] = _arVBlkLineSum[x] > u4SliceLv ? 1 : 0;

        // slicing, update period and period histogram
        if ( _arVBlkLineSum[x] > u4SliceLv && 
             _arVBlkLineSum[x] > MAX(_arVBlkLineSum[MAX(x,1)-1], _arVBlkLineSum[MIN(x+1, w-1)]))
        {
            INT32 iPeriod = x - iPrevVGridPos;
            iPrevVGridPos = x;

            if ( iPeriod < _NXNR_BNR_FW_MIN_PERIOD )
            {
                // period is too small
                _iVBlkPeriodHist[0]++;
            }
            else if ( iPeriod > _NXNR_BNR_FW_MAX_PERIOD + 1 )
            {
                // period is too large
                _iVBlkPeriodHist[1]++;
            }
            else
            {
                _iVBlkPeriodHist[iPeriod]++;
                _au1VBlkPeriodArray[x] = (UINT8) iPeriod;
            }
        }
    }

    // find initial period
    for (p = _NXNR_BNR_FW_MIN_PERIOD; p <= _NXNR_BNR_FW_MAX_PERIOD; p++)
    {
        INT32 iTestPeriodCnt = _iVBlkPeriodHist[p] + _iVBlkPeriodHist[p+1];
        INT32 iTestPeriodPct = iTestPeriodCnt * p; //iTestPeriodCnt * 100 / ( w / p );

        if ( iTestPeriodPct > iBestPeriodPct )
        {
            iBestPeriod = p;
            iBestPeriodCnt = iTestPeriodCnt;
            iBestPeriodPct = iTestPeriodPct;
        }
    }

    _iVBlkBestPeriod = iBestPeriod;
    _iVBlkBestPeriodCnt = iBestPeriodCnt;
    _BNR_FW_FindGroup ( iBestPeriod, & iGroup, & iMember );

#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    if (iBestPeriod != 0)
    {
        BOOL fgIsBestPIntegerSize = (_iVBlkPeriodHist[iBestPeriod] >= (_iVBlkPeriodHist[iBestPeriod+1]*8));

        //SYS_MEASURE_TIME_START(SV_BNR_ACTIVE_TIMER);
        u1FoundSizePhase = _BNR_FW_VGridSizePhase(iBestPeriod, fgIsBestPIntegerSize, &bnrDesc);
        //SYS_MEASURE_TIME_END(SV_BNR_ACTIVE_TIMER);
        //SYS_STORE_TIME_INFO_TO_REG(SV_BNR_ACTIVE_TIMER);
    }
#endif //SUPPORT_BNR_SIZE_PHASE_PREDICT

    fgMatchPeriod = ( iBestPeriod != 0 ) &&
        ((iGroup != 0 && iGroup == _iVBlkSavedGroup) || (iBestPeriod == _iVBlkSavedPeriod) );

    // update grid position gauge
    if ( fgMatchPeriod )
    {
    #ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
        if (u1FoundSizePhase == 1)
        {
            iLineSumInside = bnrDesc.u4LsInside;
            iLineSumOutside = bnrDesc.u4LsOutside;
            _BNR_FW_VGridForceLocAssist(&bnrDesc, w, rBnrCpt.iVBlkGaugeIncTh*2, _au1VBlkPosGauge);
            LOG(6, "=>1:ST(%2d)S(%2d)P(%2d)SI(%d)I(%06d)O(%06d)\n", bnrDesc.u4Start, bnrDesc.u4S, bnrDesc.u4P, bnrDesc.u4StartIdx, iLineSumInside, iLineSumOutside);
            LOG(6, "=>1:%3d, %3d, %3d, %3d, %3d\n", bnrDesc.u1GridPos[0], bnrDesc.u1GridPos[1], bnrDesc.u1GridPos[2], bnrDesc.u1GridPos[3], bnrDesc.u1GridPos[4]);
        }
        else
        {
            if (u1FoundSizePhase == 2)
            {
                //iLineSumInside = bnrDesc.u4LsInside;
                //iLineSumOutside = bnrDesc.u4LsOutside;
                LOG(6, "=>2:ST(%2d)S(%2d)P(%2d)SI(%d)I(%06d)O(%06d)\n", bnrDesc.u4Start, bnrDesc.u4S, bnrDesc.u4P, bnrDesc.u4StartIdx, iLineSumInside, iLineSumOutside);
                LOG(6, "=>2:%3d, %3d, %3d, %3d, %3d\n", bnrDesc.u1GridPos[0], bnrDesc.u1GridPos[1], bnrDesc.u1GridPos[2], bnrDesc.u1GridPos[3], bnrDesc.u1GridPos[4]);
            }
    #endif // SUPPORT_BNR_SIZE_PHASE_PREDICT
            for (x = 0; x < w; x++)
            {
                BOOL f0 = ( _au1VBlkPeriodArray[x] == iBestPeriod );
                BOOL f1 = ( _au1VBlkPeriodArray[x] == ( iBestPeriod + 1 ) );
                BOOL f2 = ( ( x+iBestPeriod < _iWidth ) && ( _au1VBlkPeriodArray[x+iBestPeriod] == iBestPeriod ) );
                BOOL f3 = ( ( x+iBestPeriod+1 < _iWidth ) && ( _au1VBlkPeriodArray[x+iBestPeriod+1] == ( iBestPeriod + 1 ) ) );

                if ( f0 || f1 || f2 || f3 )
            {
                _au1VBlkPosGauge[x] = (UINT8) MIN(15, (_au1VBlkPosGauge[x]+rBnrCpt.iVBlkGaugeIncTh));
                iLineSumInside += _arVBlkLineSum[x];
            }
            else
            {
                if ( _au1VBlkPosGauge[x] > 0 )
                {
                    _au1VBlkPosGauge[x]--;
                }

                    iLineSumOutside += _arVBlkLineSum[x]; //MAX ( (UINT32) _arVBlkLineSum[x], u4VGridMinLineSumTh );
                }
            }
    #ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
        }
    #endif //SUPPORT_BNR_SIZE_PHASE_PREDICT
    }
    else
    {
        for (x = 0; x < w; x++ )
        {
            if ( _au1VBlkPosGauge[x] > 0 )
            {
                _au1VBlkPosGauge[x]--;
            }
        }
    }

    // update block level
    iArtiRej = (_iVBlkPeriodHist[0]*_NXNR_BNR_FW_MIN_PERIOD + _iVBlkPeriodHist[1]*_NXNR_BNR_FW_MAX_PERIOD);
    iArtiRej = ((iBestPeriodPct - iArtiRej) * rBnrCpt.iSizeFactor) / 16;

#if 0    
    iLineSumDiff = iLineSumInside * rBnrCpt.iVBlkInsideOutsideRatio * iBestPeriod / 8 - iLineSumOutside;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactor ) >> ( 8 + 4 ) ); // iLineSumDiff could be negative
#else
    //iLineSumDiff = (iBestPeriodCnt*rBnrCpt.iVBlkInsideOutsideRatio*2 - (w / MAX(iBestPeriod, 1))) / 4;
    iLineSumDiff = (iBestPeriodPct*rBnrCpt.iVBlkInsideOutsideRatio*2 - w) / 16;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactor ) / 16 ); // iLineSumDiff could be negative
#endif

    //LOG(6, "WM(%2d)P(%2d)PC(%4d)I(%08d)O(%08d)RS(%08d)LSD(%d)DL(%d)\n", _iVBlkWindowMode, iBestPeriod, iBestPeriodCnt, iLineSumInside, iLineSumOutside, iVBlkRawStr, iLineSumDiff, iDeltaLevel);

    if ( fgMatchPeriod )
    {
    #ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
        if (u1FoundSizePhase == 1)
        {
            iDeltaLevel += ((MAX(bnrDesc.u4S, 8) - 8) * 64);
        }
    #endif
        _iVBlkLevel += ((iDeltaLevel*_u1MoIdx)/4); // delta could be positive or negative
    }
    else if ( _iVBlkLevel < rBnrCpt.iVBlkLevelConfirmTh )
    {
        _iVBlkLevel = 0;
    }
    else
    {
        _iVBlkLevel -= MAX(0, iDeltaLevel);
    }

    // try to decrease block level (leakage=128) if it is still scene.
    _iVBlkLevel -= rBnrCpt.iVBlkLevelLeakage;
    _iVBlkLevel += (rBnrCpt.fgPatchCG && iArtiRej < 0 ? iArtiRej*iBestPeriod : 0);
    _iVBlkLevel = clip ( 0, 2047, _iVBlkLevel );

    if ( _iVBlkLevel == 0 )
    {
        _iVBlkSavedPeriod = iBestPeriod;
        _iVBlkSavedGroup = iGroup;
    }

    // must active within limited time period
    if ( _iVBlkLevel < rBnrCpt.iVBlkLevelActiveTh )
    {
        _iVBlkActiveTrialCount++;
    }
    else
    {
        _iVBlkActiveTrialCount = 0;
    }

    if ( _iVBlkActiveTrialCount >= rBnrCpt.iVBlkActiveTrialTh )
    {
        _BNR_FW_ResetVGridDet ();
    }

    iBlkConf = ( clip ( 0, 1023, _iVBlkLevel - rBnrCpt.iVBlkLevelActiveTh ) + 63 ) / 64;

    if (iBlkConf < _iVBlkConfidence)
    {
        _iVBlkConfidence --;
    }
    else if (iBlkConf > _iVBlkConfidence)
    {
        _iVBlkConfidence ++;
    }

    // set grid position
    for (x = 0; x < w; x++ )
    {
        if ( _au1VBlkPosGauge[x] > rBnrCpt.iVGridActiveTh )
        {
        #ifdef _C_MODEL_
            iVBlkPos[x] = 0x3;
        #else
            vDrvNXNRWritePos(0, x, 1);
        #endif
        }
        else if ( _au1VBlkPosGauge[x] < rBnrCpt.iVGridInactiveTh )
        {
        #ifdef _C_MODEL_
            iVBlkPos[x] = 0x0;
        #else
            vDrvNXNRWritePos(0, x, 0);
        #endif
        }
    }

#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    _u1FoundSizePhs = u1FoundSizePhase;
    _rBnrDesc = bnrDesc;
#endif

#ifdef _C_MODEL_
#if 0
    LogConsoleFmt ( "==============================\n" );
    LogConsoleFmt ( "_iVBlkPeriodHist[0] = %d\n", _iVBlkPeriodHist[0] );
    LogConsoleFmt ( "_iVBlkPeriodHist[1] = %d\n", _iVBlkPeriodHist[1] );
    for ( p = _NXNR_BNR_FW_MIN_PERIOD; p <= _NXNR_BNR_FW_MAX_PERIOD; p++ )
    {
        LogConsoleFmt ( "_iVBlkPeriodHist[%2d] = %d\n", p, _iVBlkPeriodHist[p] );
    }
    LogConsoleFmt ( "_iVBlkBestPeriod = %d\n", _iVBlkBestPeriod );
    LogConsoleFmt ( "_iVBlkBestPeriodCnt = %d\n", _iVBlkBestPeriodCnt );
    LogConsoleFmt ( "fgMatchPeriod = %d\n", fgMatchPeriod ? 1 : 0 );
    LogConsoleFmt ( "iLineSumInside = %d\n", iLineSumInside );
    LogConsoleFmt ( "iLineSumOutside = %d\n", iLineSumOutside );
    LogConsoleFmt ( "iArtiRej = %d\n", iArtiRej );
    LogConsoleFmt ( "iLineSumDiff = %d\n", iLineSumDiff );
    LogConsoleFmt ( "iDeltaLevel = %d\n", iDeltaLevel );
    LogConsoleFmt ( "_iVBlkLevel = %d\n", _iVBlkLevel );  
    LogConsoleFmt ( "_iSelectVBlkWindowState = %d\n", _iSelectVBlkWindowState );
    LogConsoleFmt ( "\n\n\n" );
#endif
#endif
}

static UINT8 _BNR_FW_GetMotionLvl(void)
{
#ifndef _C_MODEL_
    return _SWR(NR_MO_STA_01, STA_MO_IDX);
#else
    return 16;
#endif
}

static void _BNR_FW_CopyLineSumTool(void)
{
#ifndef _C_MODEL_
    vDrvNXNRCopyLineSumTool();
/*
    static UINT8 _u1Cnt = 0;

    _u1Cnt = _SWR(NR_CTRL_00, EN_TRIG_COPY_LS) && !_SWR(NR_BLK_STA_03, NR_BLK_READ_FAIL) ? _u1Cnt + 1 : 0;

    if (_u1Cnt >= 4)
    {
        x_memcpy((UINT32*)NR_VBLK_START, _arVBlkLineSum, sizeof(UINT32)*960);
        x_memcpy((UINT32*)NR_HBLK_START, _arHBlkLineSum, sizeof(UINT32)*540);    
        _u1Cnt = 0;
        _SWW(NR_CTRL_00, 0, EN_TRIG_COPY_LS);
    }
*/
#endif
}

static void _BNR_FW_ReadLineSum(void)
{
#ifdef _C_MODEL_
    memcpy ( _arVBlkLineSum, pNXNR->pVBlkLineSumOut, sizeof ( INT32 ) * _iWidth );
    memcpy ( _arHBlkLineSum, pNXNR->pHBlkLineSumOut, sizeof ( INT32 ) * _iHeight );

    int iVLineSumPadBits = ( pNXNR->cCsr.nx_vblk_line_sum_sat_lv == 0 ) ? 2 : 3;
    int iHLineSumPadBits = ( pNXNR->cCsr.nx_hblk_line_sum_sat_lv == 0 ) ? 2 : 3;

    for ( int x = 0; x < _iWidth; x++ )
    {
      _arVBlkLineSum[x] <<= iVLineSumPadBits;
    }

    for ( int y = 0; y < _iHeight; y++ )
    {
      _arHBlkLineSum[y] <<= iHLineSumPadBits;
    }
#else
    u1DrvNXNRReadBlkLineSum();
#endif
}

static void _BNR_FW_WriteBlkPos(void)
{
#ifdef _C_MODEL_
    memcpy( pNXNR->pVBlkPos, iVBlkPos, sizeof(INT32) * _iWidth );
    memcpy( pNXNR->pHBlkPos, iHBlkPos, sizeof(INT32) * _iHeight );
#else
    if (rBnrCpt.fgEnLocWrite)
        u1DrvNXNRWriteBlkPos();
#endif
}

static void _BNR_FW_WriteReg(void)
{
#ifdef _C_MODEL_
    pNXNR->cCsr.nx_manual_vblk_confidence = _iVBlkConfidence;
    pNXNR->cCsr.nx_manual_hblk_confidence = _iHBlkConfidence;
    pNXNR->cCsr.nx_manual_vblk_confidence_pr_r = _iVBlkConfidence;
#else
    NR_W(NXNR_0D, _iVBlkConfidence, NX_MANUAL_VBLK_CONFIDENCE);
    NR_W(NXNR_0C, _iHBlkConfidence, NX_MANUAL_HBLK_CONFIDENCE);
    NR_W(NXNR_16, _iVBlkConfidence, NX_MANUAL_VBLK_CONFIDENCE_PR_R);

    #ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    _SWW(NR_BLK_STA_0F, _u1FoundSizePhs, NR_BLK_FOUND_SP);
    _SWW(NR_BLK_STA_0F, _rBnrDesc.u4S, NR_BLK_EST_SIZE);
    _SWW(NR_BLK_STA_0F, _rBnrDesc.u4P, NR_BLK_EST_PERD);
    #endif    
#endif
}

static void _BNR_FW_WriteRegOnce(void)
{
#ifdef _C_MODEL_
    pNXNR->cCsr.nx_blk_loc_mode = 1;
    pNXNR->cCsr.nx_force_hblk_confidence = 1;
    pNXNR->cCsr.nx_force_hblk_confidence = 0;
    pNXNR->cCsr.nx_force_hblk_size_phase = 1;
    pNXNR->cCsr.nx_manual_hblk_size = 8;
    pNXNR->cCsr.nx_manual_hblk_phase = 0;
    pNXNR->cCsr.nx_force_vblk_confidence = 1;
    pNXNR->cCsr.nx_force_vblk_confidence = 0;
    pNXNR->cCsr.nx_force_vblk_size_phase = 1;
    pNXNR->cCsr.nx_manual_vblk_size = 8;
    pNXNR->cCsr.nx_manual_vblk_phase = 0;
    pNXNR->cCsr.nx_manual_vblk_size_pr_r = 8;
    pNXNR->cCsr.nx_manual_vblk_phase_pr_r = 0;

    if ( _iWidth > 720 )
    {
        // HD
        pNXNR->cCsr.nx_vblk_line_sum_sat_lv = 1;
        pNXNR->cCsr.nx_hblk_line_sum_sat_lv = 1;
    }
    else
    {
        // SD
        pNXNR->cCsr.nx_vblk_line_sum_sat_lv = 0;
        pNXNR->cCsr.nx_hblk_line_sum_sat_lv = 0;
    }
#else
    NR_W(NXNR_05, 1, NX_BLK_LOC_MODE);
    NR_W(NXNR_00, 3, NX_DEBLOCK_V_MASK);
    NR_W(NXNR_0C, 1, NX_FORCE_HBLK_CONFIDENCE);
    NR_W(NXNR_0C, 1, NX_FORCE_HBLK_SIZE_PHASE);
    NR_W(NXNR_0C, 8, NX_MANUAL_HBLK_SIZE);
    NR_W(NXNR_0C, 0, NX_MANUAL_HBLK_PHASE);
    NR_W(NXNR_0D, 1, NX_FORCE_VBLK_CONFIDENCE);
    NR_W(NXNR_0D, 1, NX_FORCE_VBLK_SIZE_PHASE);
    NR_W(NXNR_0D, 8, NX_MANUAL_VBLK_SIZE);
    NR_W(NXNR_0D, 0, NX_MANUAL_VBLK_PHASE);
    NR_W(NXNR_16, 8, NX_MANUAL_VBLK_SIZE_PR_R);
    NR_W(NXNR_16, 0, NX_MANUAL_VBLK_PHASE_PR_R);
#endif
}

static void _BNR_FW_GetControlPoints(void)
{
#ifndef _C_MODEL_
    rBnrCpt.iHGridMinLineSumTh      = _SWR(NR_FWBNR_03, NR_FWBNR_HGRIDMINLINESUMTH);
    rBnrCpt.iHGridActiveTh          = _SWR(NR_FWBNR_03, NR_FWBNR_HGRIDACTIVETH); 
    rBnrCpt.iHGridInactiveTh        = _SWR(NR_FWBNR_03, NR_FWBNR_HGRIDINACTIVETH);
    rBnrCpt.iHBlkLevelActiveTh      = _SWR(NR_FWBNR_04, NR_FWBNR_HBLKLEVELACTIVETH); 
    rBnrCpt.iHBlkLevelConfirmTh     = _SWR(NR_FWBNR_04, NR_FWBNR_HBLKLEVELCONFIRMTH); 
    rBnrCpt.iHBlkLevelLeakage       = _SWR(NR_FWBNR_04, NR_FWBNR_HBLKLEVELLEAKAGE); 
    rBnrCpt.iHBlkInsideOutsideRatio = _SWR(NR_FWBNR_03, NR_FWBNR_HBLKINSIDEOUTSIDERATIO); 
    rBnrCpt.iHBlkActiveTrialTh      = _SWR(NR_FWBNR_03, NR_FWBNR_HBLKACTIVETRIALTH);
    rBnrCpt.iVGridMinLineSumTh      = _SWR(NR_FWBNR_00, NR_FWBNR_VGRIDMINLINESUMTH);
    rBnrCpt.iVGridActiveTh          = _SWR(NR_FWBNR_00, NR_FWBNR_VGRIDACTIVETH);               
    rBnrCpt.iVGridInactiveTh        = _SWR(NR_FWBNR_00, NR_FWBNR_VGRIDINACTIVETH);              
    rBnrCpt.iVBlkLevelActiveTh      = _SWR(NR_FWBNR_01, NR_FWBNR_VBLKLEVELACTIVETH);          
    rBnrCpt.iVBlkLevelConfirmTh     = _SWR(NR_FWBNR_01, NR_FWBNR_VBLKLEVELCONFIRMTH);         
    rBnrCpt.iVBlkLevelLeakage       = _SWR(NR_FWBNR_01, NR_FWBNR_VBLKLEVELLEAKAGE);            
    rBnrCpt.iVBlkInsideOutsideRatio = _SWR(NR_FWBNR_02, NR_FWBNR_VBLKINSIDEOUTSIDERATIO);       
    rBnrCpt.iVBlkActiveTrialTh      = _SWR(NR_FWBNR_02, NR_FWBNR_VBLKACTIVETRIALTH);
    rBnrCpt.iVBlkGaugeIncTh         = _SWR(NR_FWBNR_02, NR_FWBNR_VBLKGAUGEINCTH);
    rBnrCpt.iVBlkWindowTrialCountTh = _SWR(NR_FWBNR_02, NR_FWBNR_VBLKWINDOWTRIALCOUNTTH); // try same window size for N frames  
    rBnrCpt.iVBlkScoreTh            = _SWR(NR_FWBNR_02, NR_FWBNR_VBLKSCORETH); // if score lower than threshold, restart test each window size
    rBnrCpt.fgIgnoreNW              = _SWR(NR_FWBNR_00, NR_FWBNR_VBLK_IGNORE_NW);
    rBnrCpt.fgUseR                  = _SWR(NR_FWBNR_00, NR_FWBNR_READ_RIGHT);
    rBnrCpt.fgEnLocWrite            = _SWR(NR_FWBNR_00, NR_FWBNR_LOC_WRITE);
    rBnrCpt.fgPatchCG               = _SWR(NR_FWBNR_00, NR_FWBNR_CG_PATCH0);
#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    rBnrCpt.u1InitRtoNum            = _SWR(NR_FWBNR_05, NR_FWBNR_INIT_RTO_NUM);
    rBnrCpt.u1InitRtoDen            = _SWR(NR_FWBNR_05, NR_FWBNR_INIT_RTO_DEN);
#endif

#if 0
    if (_iVBlkWindowMode == _NXNR_WINDOW_MODE_N)
    {
        rBnrCpt.iVGridMinLineSumTh  = _SWR(NR_FWBNR_00, NR_FWBNR_VGRIDMINLINESUMTH);
    }
    else
    {
        rBnrCpt.iVGridMinLineSumTh  = MAX(_SWR(NR_FWBNR_00, NR_FWBNR_VGRIDMINLINESUMTH), (_u4VBlkFrmSum / MAX(_iWidth, 64)));
    }
#endif
    _iVGridMinLsTh = rBnrCpt.iVGridMinLineSumTh;
    //LOG(6, "FS[%6d]TH[%4d] ", _u4VBlkFrmSum, _iVGridMinLsTh);

#endif
}

static void _BNR_FW_SetControlPoints(void)
{
#ifdef _C_MODEL_
    rBnrCpt.iHGridMinLineSumTh = _iWidth / 8;
    rBnrCpt.iHGridActiveTh = 12;
    rBnrCpt.iHGridInactiveTh = 4;
    rBnrCpt.iHBlkLevelActiveTh = 512;
    rBnrCpt.iHBlkLevelConfirmTh = 256;
    rBnrCpt.iHBlkLevelLeakage = 64;
    rBnrCpt.iHBlkInsideOutsideRatio = 4;
    rBnrCpt.iHBlkActiveTrialTh = 20;
    rBnrCpt.iVGridMinLineSumTh = _iHeight / 8; 
    rBnrCpt.iVGridActiveTh = 12;               
    rBnrCpt.iVGridInactiveTh = 4;              
    rBnrCpt.iVBlkLevelActiveTh = 512;          
    rBnrCpt.iVBlkLevelConfirmTh = 256;         
    rBnrCpt.iVBlkLevelLeakage = 64;            
    rBnrCpt.iVBlkInsideOutsideRatio = 4;       
    rBnrCpt.iVBlkActiveTrialTh = 20;
    rBnrCpt.iVBlkGaugeIncTh = 2;
    rBnrCpt.iVBlkWindowTrialCountTh = 3; // try same window size for N frames  
    rBnrCpt.iVBlkScoreTh = 32; // if score lower than threshold, restart test each window size
#ifdef SUPPORT_BNR_SIZE_PHASE_PREDICT
    rBnrCpt.u1InitRtoNum = 2;
    rBnrCpt.u1InitRtoDen = 1;
#endif
#else 	
    //_SWW(NR_FWBNR_00, _iHeight / 8, NR_FWBNR_VGRIDMINLINESUMTH);	
    //_SWW(NR_FWBNR_00,  12, NR_FWBNR_VGRIDACTIVETH);		
    //_SWW(NR_FWBNR_00,   4, NR_FWBNR_VGRIDINACTIVETH);
    //_SWW(NR_FWBNR_01, 512, NR_FWBNR_VBLKLEVELACTIVETH);	
    //_SWW(NR_FWBNR_01, 256, NR_FWBNR_VBLKLEVELCONFIRMTH);		
    //_SWW(NR_FWBNR_01,  64, NR_FWBNR_VBLKLEVELLEAKAGE);
    //_SWW(NR_FWBNR_02,   4, NR_FWBNR_VBLKINSIDEOUTSIDERATIO);	
    //_SWW(NR_FWBNR_02,  20, NR_FWBNR_VBLKACTIVETRIALTH);		
    //_SWW(NR_FWBNR_02,   2, NR_FWBNR_VBLKGAUGEINCTH);
    //_SWW(NR_FWBNR_02,   3, NR_FWBNR_VBLKWINDOWTRIALCOUNTTH);		
    //_SWW(NR_FWBNR_02,  32, NR_FWBNR_VBLKSCORETH);
    //_SWW(NR_FWBNR_02,   1, NR_FWBNR_VBLK_IGNORE_NW);
    //_SWW(NR_FWBNR_03, _iWidth / 8, NR_FWBNR_HGRIDMINLINESUMTH);	
    //_SWW(NR_FWBNR_03,  12, NR_FWBNR_HGRIDACTIVETH);		
    //_SWW(NR_FWBNR_03,   4, NR_FWBNR_HGRIDINACTIVETH);
    //_SWW(NR_FWBNR_03,   4, NR_FWBNR_HBLKINSIDEOUTSIDERATIO);	
    //_SWW(NR_FWBNR_03,  20, NR_FWBNR_HBLKACTIVETRIALTH);		
    //_SWW(NR_FWBNR_04, 512, NR_FWBNR_HBLKLEVELACTIVETH);	
    //_SWW(NR_FWBNR_04, 256, NR_FWBNR_HBLKLEVELCONFIRMTH);		
    //_SWW(NR_FWBNR_04,  64, NR_FWBNR_HBLKLEVELLEAKAGE);
#endif

#if 0
    if ( _iWidth * _iHeight >= 1920 * 1080 )
        rBnrCpt.iSizeFactor = 3; // 1080p
    else if ( _iWidth * _iHeight >= 1280 * 720 )
        rBnrCpt.iSizeFactor = 7; // 720p
    else 
        rBnrCpt.iSizeFactor = 16; // SD  
#endif
}

static void _BNR_FW_SetState(INT32 iState)
{
    if (_iBnrFwStop)
    {
        // only update the previous state for future resume
        _iBnrFwStateOnStop = iState;
    }
    else
    {
        _iBnrFwStateOnStop = iState;
        _iBnrFwState = iState;
    }
}

#ifndef _C_MODEL_
static void _BNR_FW_Log(void)
{
    UINT32 i;

    _SWWM(NR_BLK_STA_12, P_Fld(_u1VBlkAvgLS, NR_STA_VBLK_AVGLS)|P_Fld(_u1HBlkAvgLS, NR_STA_HBLK_AVGLS));
        
    if (_rBnrLog.u4LogCnt)
    {
        switch (_rBnrLog.u1LogMode)
        {
        case _NXNR_BNR_FW_LOG_STA:
            LOG(5, "SP[%2d]BP[%2d]BPC[%5d]BL[%4d]TC[%2d] HBP[%d]HBPC[%d]\n",
                _iVBlkSavedPeriod, _iVBlkBestPeriod, _iVBlkBestPeriodCnt, _iVBlkLevel,
                _iVBlkActiveTrialCount, 
                _iHBlkBestPeriod, _iHBlkBestPeriodCnt);        
            break;
        case _NXNR_BNR_FW_LOG_VBLK_PEROID_HIST:
            LOG(5, "VBlkPeriodHist:\n");
            for (i = 0; i < 35; i++)
            {
                LOG(5, "\t[%02d] = %d\n", i, _iVBlkPeriodHist[i]);
            }
            break;
        case _NXNR_BNR_FW_LOG_HBLK_PEROID_HIST:
            LOG(5, "HBlkPeriodHist:\n");
            for (i = 0; i < 35; i++)
            {
                LOG(5, "\t[%02d] = %d\n", i, _iHBlkPeriodHist[i]);
            }
            break;
        }
        _rBnrLog.u4LogCnt --;
    }
}

void BNR_FW_SetLogCfg(UINT8 u1Sel, UINT32 u4Cnt)
{
    _rBnrLog.u1LogMode = u1Sel;
    _rBnrLog.u4LogCnt  = u4Cnt;
}

BOOL BNR_FW_IsStop(void)
{
    return (_iBnrFwStop ? 1 : 0);
}

void BNR_FW_Dbg(void)
{
    if (_iBnrFwStop && _iBnrFwState == _NXNR_BNR_FW_STATE_STOP)
        _iBnrFwState = _NXNR_BNR_FW_STATE_DBG;
}

void BNR_FW_Stop(void)
{
    if (!_iBnrFwStop)
    {
        _iBnrFwStateOnStop = _iBnrFwState;
        _iBnrFwState = _NXNR_BNR_FW_STATE_STOP;
        NR_W(NXNR_05, 0, NX_BLK_LOC_MODE);
        NR_W(NXNR_0C, 0, NX_FORCE_HBLK_CONFIDENCE);
        NR_W(NXNR_0C, 0, NX_FORCE_HBLK_SIZE_PHASE);
        NR_W(NXNR_0D, 0, NX_FORCE_VBLK_CONFIDENCE);
        NR_W(NXNR_0D, 0, NX_FORCE_VBLK_SIZE_PHASE);
        
        //NR_W(NXNR_05, 1, NX_VBLK_MAX_RANGE); 
        NR_W(NXNR_05, 1, NX_VBLK_DISCONTI_MODE);
    }

    _iBnrFwStop = 1;
}

void BNR_FW_Resume(void)
{
    if (_iBnrFwStop)
    {
        _iBnrFwState = _iBnrFwStateOnStop;
        NR_W(NXNR_05, 1, NX_BLK_LOC_MODE);
        NR_W(NXNR_0C, 1, NX_FORCE_HBLK_CONFIDENCE);
        NR_W(NXNR_0C, 1, NX_FORCE_HBLK_SIZE_PHASE);
        NR_W(NXNR_0D, 1, NX_FORCE_VBLK_CONFIDENCE);
        NR_W(NXNR_0D, 1, NX_FORCE_VBLK_SIZE_PHASE);
        
        //NR_W(NXNR_05, 3, NX_VBLK_MAX_RANGE); 
        NR_W(NXNR_05, 0, NX_VBLK_DISCONTI_MODE);
    }

    _iBnrFwStop = 0;
}
#else
static void _BNR_FW_Log(void) {}
void BNR_FW_SetLogCfg(UINT8 u1Sel, UINT32 u4Cnt) {}
BOOL BNR_FW_IsStop(void) { return 1; }
void BNR_FW_Dbg(void) {}
void BNR_FW_Stop(void) {}
void BNR_FW_Resume(void) {}
#endif

#ifdef _C_MODEL_
void BNR_FW_Initialize ( CNXNR * _pNXNR, const char * _szPatternName )
{
    pNXNR = _pNXNR;
    _iHeight = pNXNR->iHeight;
    _iWidth = pNXNR->iWidth;

    _iHeight = MAX(MIN(1080, u4Height), 64);
    _iWidth  = MAX(MIN(1920, u4Width), 64);

    rBnrCpt.iSizeFactor = 720*16 / MAX(720, _iWidth);
    rBnrCpt.iSizeFactorH = 480*16 / MAX(480, _iHeight);
    
    strcpy ( szPatternName, _szPatternName );
    _iBnrFwState = _NXNR_BNR_FW_STATE_INIT;
    _u1Init = 1;
}
#else
void BNR_FW_Initialize(UINT32 u4Width, UINT32 u4Height)
{
    _iHeight = MAX(MIN(1080, u4Height), 64);
    _iWidth  = MAX(MIN(1920, u4Width), 64);

    x_memset(_arVBlkLineSum, 0, sizeof(UINT16)*_NXNR_BNR_FW_MAX_WIDTH);
    x_memset(_arHBlkLineSum, 0, sizeof(UINT16)*_NXNR_BNR_FW_MAX_HEIGHT);
    x_memset(_arVBlkLineSumSlice, 0, sizeof(UINT8)*_NXNR_BNR_FW_MAX_WIDTH);

    rBnrCpt.iSizeFactor = 720*16 / MAX(720, _iWidth);
    rBnrCpt.iSizeFactorH = 480*16 / MAX(480, _iHeight);
    
    _iBnrFwStateOnStop = _NXNR_BNR_FW_STATE_INIT;

    if (!_iBnrFwStop)
        _iBnrFwState = _NXNR_BNR_FW_STATE_INIT;    

    if (!_u1Init)
    {
        //VERIFY(x_sema_create(&_hBNRMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        _u1Init = 1;
    }
}
#endif

void BNR_FW_ActiveProcess(void)
{
    if (!_u1Init)
        return;
    
    // do something extra on initialization
    switch (_iBnrFwState)
    {
    case _NXNR_BNR_FW_STATE_INIT:
        {
            _BNR_FW_SetControlPoints();
            _BNR_FW_WriteRegOnce();
            _BNR_FW_ResetHGridDet();
            _BNR_FW_ResetVGridDet();
            _BNR_FW_SetState(_NXNR_BNR_FW_STATE_ACTIVE);
        }
        break;
        
    case _NXNR_BNR_FW_STATE_ACTIVE:
        {
        #ifndef _C_MODEL_
            UINT32 i, w, h;
            UINT32 u4VBlkFrmSum = 0;
            UINT32 u4HBlkFrmSum = 0;
            UINT32 u4VSkipLine  = 8 * (MAX(1, NR_R(NXNR_00, NX_DEBLOCK_V_MASK)) - 1);

            w = _iWidth;
            h = _iHeight;

            for (i = 0; i < w; i++)
            {
                UINT32 u4Tmp = u2DrvNXNRGetVBlkLineSum(rBnrCpt.fgUseR, i)&0x3FFC;
                u4Tmp = MIN(u4Tmp, 1020);
                u4VBlkFrmSum += u4Tmp;
                _arVBlkLineSum[i] = u4Tmp;
            }

            for (i = 0; i < u4VSkipLine; i++)
            {
                _arHBlkLineSum[i] = 0;
            }

            for (i = u4VSkipLine; i < h; i++)
            {
                UINT32 u4Tmp = u2DrvNXNRGetHBlkLineSum(rBnrCpt.fgUseR, i)&0x3FFC;
                u4Tmp = MIN(u4Tmp, 1020);
                u4HBlkFrmSum += u4Tmp;
                _arHBlkLineSum[i] = u4Tmp;
            }

            _u2VBlkPixCnt = u2DrvNXNRGetVBlkPxCnt(rBnrCpt.fgUseR);
            _u2HBlkPixCnt = u2DrvNXNRGetHBlkPxCnt(rBnrCpt.fgUseR);
            _u1VBlkAvgLS = (_u2VBlkPixCnt != 0) ? (u4VBlkFrmSum / _u2VBlkPixCnt) : 0;
            _u1HBlkAvgLS = (_u2HBlkPixCnt != 0) ? (u4HBlkFrmSum / _u2HBlkPixCnt) : 0;
        #endif
            _u1MoIdx = MIN(4, _BNR_FW_GetMotionLvl());
			_BNR_FW_GetControlPoints();
            _BNR_FW_HGridDet();
            _BNR_FW_VGridDet();
            _BNR_FW_CopyLineSumTool();
            _BNR_FW_Log();
            _BNR_FW_SetState(_NXNR_BNR_FW_STATE_VSYNC);
        }
        break;
        
    case _NXNR_BNR_FW_STATE_DBG:
        _BNR_FW_CopyLineSumTool();
        _iBnrFwState = _NXNR_BNR_FW_STATE_VSYNC_DBG;
        break;
    }
}

void BNR_FW_VSyncProcess(void)
{
    switch (_iBnrFwState)
    {
    case _NXNR_BNR_FW_STATE_VSYNC:
        _BNR_FW_WriteBlkPos();
        _BNR_FW_ReadLineSum();
        _BNR_FW_WriteReg();
        _BNR_FW_SetState(_NXNR_BNR_FW_STATE_ACTIVE);
        break;
        
    case _NXNR_BNR_FW_STATE_VSYNC_DBG:
        _BNR_FW_WriteBlkPos();
        _BNR_FW_ReadLineSum();
        _iBnrFwState = _NXNR_BNR_FW_STATE_DBG;
        break;
    }
}

#endif  // BNR_NEW_FW
