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

#include <string.h>
#include <direct.h>
#else

#include "drv_nr.h"
#include "drv_nr_int.h"
#include "drv_bnr_int.h"
#include "hw_nr.h"
#include "nr_debug.h"
#include "hw_sw.h"

#include "x_os.h"
#include "x_assert.h"
#include "drv_di.h"
#include "hw_di_int.h"

//#define BNR_MUTEX_LOCK                  (VERIFY(x_sema_lock(_hBNRMutex, X_SEMA_OPTION_WAIT) == OSR_OK))
//#define BNR_MUTEX_UNLOCK                (VERIFY(x_sema_unlock(_hBNRMutex) == OSR_OK))

#define clip(m, M, x) MAX(MIN((INT32)(x),(INT32)(M)), (INT32)(m))
#endif

#ifdef BNR_NEW_FW

/*
 * control points
 */
typedef struct
{
    UINT8 u1MinHBlkSize;
    UINT8 u1MinVBlkSize;

    UINT8 u1MaxHBlkSize;
    UINT8 u1MaxVBlkSize;

    UINT8 u1HBlkPeriodSensitivity0;
    UINT8 u1VBlkPeriodSensitivity0;

    UINT8 u1HBlkPeriodSensitivity1;
    UINT8 u1VBlkPeriodSensitivity1;

    UINT8 u1HBlkIntensitySensitivity0;
    UINT8 u1VBlkIntensitySensitivity0;

    UINT8 u1HBlkIntensitySensitivity1;
    UINT8 u1VBlkIntensitySensitivity1;

    UINT8 u1HBlkAmountSensitivity0;
    UINT8 u1VBlkAmountSensitivity0;

    UINT8 u1HBlkAmountSensitivity1;
    UINT8 u1VBlkAmountSensitivity1;

    UINT8 u1HBlkPeriodMatchTh;
    UINT8 u1VBlkPeriodMatchTh;

    UINT8 u1HBlkPredictionProceedTh;
    UINT8 u1VBlkPredictionProceedTh;

    UINT8 u1HBlkPredictionValidTh;
    UINT8 u1VBlkPredictionValidTh;

    UINT8 u1HBlkLevelActiveTh;
    UINT8 u1VBlkLevelActiveTh;

    UINT8 u1HBlkLevelInc;
    UINT8 u1VBlkLevelInc;

    UINT8 u1HBlkLevelDec;
    UINT8 u1VBlkLevelDec;

    UINT8 u1HBlkInsideOutsideRatio;
    UINT8 u1VBlkInsideOutsideRatio;

    UINT8 u1HBlkActiveTrialTh;
    UINT8 u1VBlkActiveTrialTh;

    UINT8 u1MnrLowTh;
    UINT8 u1MnrLowGain;
    UINT8 u1MnrHighTh;
    UINT8 u1MnrHighGain;
    UINT8 u1MnrSensitivity;
    
    UINT8 u1TxtrPxlCntThSize0;
    UINT8 u1TxtrPxlCntThSize1;
    UINT8 u1TxtrPxlCntThSize2;
    UINT8 u1TxtrPxlCntThSize3;

    UINT8 u1TxtrPxlCntThValue0;
    UINT8 u1TxtrPxlCntThValue1;
    UINT8 u1TxtrPxlCntThValue2;
    UINT8 u1TxtrPxlCntThValue3;
    
    UINT8 u1BnrAdapNMTh1;
    UINT8 u1BnrAdapNMTh2;
    UINT8 u1BnrAdapVBFSTh1;
    UINT8 u1BnrAdapVBFSTh2;
    UINT8 u1BnrAdapVTBSSTh1;
    UINT8 u1BnrAdapVTBSSTh2;
    UINT8 u1BnrAdapVTBSHTh1;
    UINT8 u1BnrAdapVTBSHTh2;
    
    BOOL  fgEnVDet;
    BOOL  fgEnHDet;
    BOOL  fgEnMnrAuto;
    BOOL  fgEnBnrAuto;
    BOOL  fgEnBnrThAuto;
    
    BOOL  fgPatchCG;
    BOOL  fgUseR;
    BOOL  fgEnLSumRead;
    BOOL  fgEnLocWrite;

    UINT8 u1HBlkLsPadBits;
    UINT8 u1VBlkLsPadBits;
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

static INT32 _iBnrFwStop;
static INT32 _iBnrFwStateOnStop;
static INT32 _iBnrFwState;

static UINT8  _iLineSumArrayIndex;

static UINT16 _au2HBlkLineSum[2][_NXNR_BNR_FW_MAX_HEIGHT];
static UINT16 _au2VBlkLineSum[2][_NXNR_BNR_FW_MAX_WIDTH];
static UINT8 _au1HBlkGridPredict[_NXNR_BNR_FW_MAX_HEIGHT];
static UINT8 _au1VBlkGridPredict[_NXNR_BNR_FW_MAX_WIDTH];

static INT32 _iHBlkPeriod;
static INT32 _iVBlkPeriod;

static UINT16 _u2VBlkPixCnt;
static UINT16 _u2HBlkPixCnt;

UINT8 _u1MoIdx;
static UINT8 _u1VBlkAvgLS;
static UINT8 _u1HBlkAvgLS;

static UINT16 _au2LineSumAvg[_NXNR_BNR_FW_MAX_DIMENSION];

static UINT8 _au1Period[_NXNR_BNR_FW_MAX_DIMENSION];
static UINT8 _au1PeriodHist[_NXNR_BNR_FW_MAX_PERIOD+3];
static UINT8 _au1PeriodHistHBLK[_NXNR_BNR_FW_MAX_PERIOD+3];
static UINT8 _au1PeriodHistVBLK[_NXNR_BNR_FW_MAX_PERIOD+3];

static UINT8 _au1BestPeriod[_NXNR_BNR_FW_MAX_DIMENSION];
static UINT8 _au1BestPeriodHist[_NXNR_BNR_FW_MAX_PERIOD+3];

static UINT8 _u1HBlkFlg;
static UINT8 _u1VBlkFlg;

static INT32 _iHBlkSavedMajorPeriod;
static INT32 _iVBlkSavedMajorPeriod;

static INT32 _iHBlkSavedMinorPeriodCount;
static INT32 _iVBlkSavedMinorPeriodCount;

static INT32 _iHBlkSavedMaxIndex[_NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT];
static INT32 _iVBlkSavedMaxIndex[_NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT];

static INT32 _iHBlkLevel;
static INT32 _iVBlkLevel;

static INT32 _iHBlkConfidence;
static INT32 _iVBlkConfidence;

static INT32 _iHBlkActiveTrialCount;
static INT32 _iVBlkActiveTrialCount;

static INT32 _iHBlkIntensity;
static INT32 _iVBlkIntensity;

static INT32 _iHBlkAmount;
static INT32 _iVBlkAmount;

static INT32 _iHBlkPixCount;
static INT32 _iVBlkPixCount;

static INT32 _iVBlkTxtrPxlCntTh;

static INT32 _iMosPixCount;
static INT32 _iMosPixDiffSum;
static INT32 _iMnrGain;
//static INT32 _iPreMnrLevel;
static INT32 _iMnrLevel;

static INT32 _iBnrVblkBsFltStr;
static INT32 _iBnrVblkTxBsStrSlp;
static INT32 _iBnrVblkTxBsStrTh;

#ifndef _C_MODEL_
//static HANDLE_T _hBNRMutex = (HANDLE_T)NULL;
#endif

static void _BNR_FW_SetControlPoints(void);
static void _BNR_FW_ResetGridDet ( UINT8 fgHV );
static void _BNR_FW_GridDet ( UINT8 fgHV );
static void _BNR_FW_ReadLineSum(void);
static void _BNR_FW_CopyLineSumTool(void);
static void _BNR_FW_WriteBlkPos(void);
static void _BNR_FW_WriteReg(void);
static void _BNR_FW_WriteRegOnce(void);

static BNR_LOG_CFG_T _rBnrLog = {0, 0};

NXNR_BNR_CTRL_PT_T rBnrCpt;

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
EXTERN void vDrvNXNRWritePosH(UINT32 u4Pos, UINT8 u1OnOff);
EXTERN void vDrvNXNRWritePosV(UINT32 u4Pos, UINT8 u1OnOff);
EXTERN void vDrvNXNRTrigToMoveData(void);
EXTERN void vDrvNRLRSwap(void);
#endif

/*
 * functions
 */
static void _BNR_FW_SortAscend ( INT32 * iArray, INT32 iCount )
{
    INT32 a, b;
    INT32 tmp;

    if ( iCount < 2 )
    {
        return;
    }

    for ( a = iCount - 1; a > 0; a-- )
    {
        for ( b = a - 1; b >= 0; b-- )
        {
            if ( iArray[a] < iArray[b] )
            {
                tmp = iArray[a];
                iArray[a] = iArray[b];
                iArray[b] = tmp;
            }
        }
    }
}

static void _BNR_FW_ResetGridDet ( UINT8 fgHV )
{
    if ( fgHV == _NXNR_HBLK )
    {
        x_memset( _au1HBlkGridPredict, 0, sizeof ( UINT8 ) * _NXNR_BNR_FW_MAX_HEIGHT );

        _iHBlkLevel = 0;
        _iHBlkConfidence = 0;
        _iHBlkActiveTrialCount = 0;
        _iHBlkPeriod = 0;
        _iHBlkSavedMajorPeriod = 0;
        _iHBlkSavedMinorPeriodCount = 0;
    }
    else
    {
        x_memset( _au1VBlkGridPredict, 0, sizeof ( UINT8 ) * _NXNR_BNR_FW_MAX_WIDTH );

        _iVBlkLevel = 0;
        _iVBlkConfidence = 0;
        _iVBlkActiveTrialCount = 0;
        _iVBlkPeriod = 0;
        _iVBlkSavedMajorPeriod = 0;
        _iVBlkSavedMinorPeriodCount = 0;

        _iVBlkTxtrPxlCntTh = 0;
    }
}

static void _BNR_FW_GridDet ( UINT8 fgHV )
{
  UINT16 * au2LineSum[2];
    UINT8 * au1GridPredict;

#ifndef _C_MODEL_
    void (*pfnGridSet) (UINT32, UINT8);
#endif

    INT32 iMaxPeriod;
    INT32 iMinPeriod;
    INT32 iDimension;
    INT32 iPeriodSensitivity0;
    INT32 iPeriodSensitivity1;
    INT32 iIntensitySensitivity0;
    INT32 iIntensitySensitivity1;
    INT32 iAmountSensitivity0;
    INT32 iAmountSensitivity1;
    INT32 iPeriodMatchTh;
    INT32 iInsideOutsideRatio;
    INT32 iLevelInc;
    INT32 iLevelDec;
    INT32 iLevelActiveTh;
    INT32 iActiveTrialTh;
    INT32 iLevel;
    INT32 iConfidence;
    INT32 iActiveTrialCount;
    INT32 iTxtrPxlCntTh;
    INT32 iGridLineSum;
    INT32 iAllLineSum;
    INT32 iIntensity;
    INT32 iAmount;
    INT32 iPixCount;
    INT32 iPredictionProceedTh;
    INT32 iPredictionValidTh;
    INT32 iLineSumSelf;
    INT32 iLineSumNeighbor;
    INT32 iWindow;
    INT32 iLastFoundWindow;
    INT32 iMaxValidPeriod;
    INT32 iMinValidPeriod;
    INT32 iPosStart;
    INT32 iPosEnd;
    INT32 iPosDelta;
    INT32 iPrevPos;
    INT32 iDistance;
    INT32 iAllPeriodPct;
    INT32 iSelfPeriodPct;
    INT32 iOthersPeriodPct;
    INT32 iCurrPeriod;
    INT32 iCurrPeriodPct;
    INT32 iPeriod;
    INT32 iCurrIntensity;
    INT32 iCurrAmount;
    INT32 iSensitivity;
    INT32 iPeriod0;
    INT32 iPeriod1;
    INT32 iRound;
    INT32 iHist[_NXNR_BNR_FW_MAX_MAJOR_PERIOD];
    INT32 iMax[_NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT];
    INT32 iCurrMajorPeriod;
    INT32 iCurrMinorPeriodCount;
    INT32 iCurrMaxIndex[_NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT];
    INT32 iMajorPeriod;
    INT32 iMinorPeriodCount;
    INT32 iMaxIndex[_NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT];
    INT32 iTotalSum;
    INT32 iMaxSum;
    INT32 iDiff;
    INT32 d, n, i, j, k;

    UINT32 u4HomoSum;
    UINT8 u1Flg = 0;
    BOOL bCond0;
    BOOL bCond1;
    BOOL bCond2;
    BOOL bCond3;
    BOOL bNewBestPeriod;
    BOOL bPeriodFound;
    BOOL bPeriodMatch;
    BOOL bPredictionFound;
    BOOL bPredictionMatch;

#ifdef _C_MODEL_
    static int index;

    if ( index == 4 && fgHV == _NXNR_VBLK )
      index = index;
#endif

    // check enable flag
    if ( ( fgHV == _NXNR_HBLK ) ? ( ! rBnrCpt.fgEnHDet ) : ( ! rBnrCpt.fgEnVDet ) )
    {
        _BNR_FW_ResetGridDet ( fgHV );
        return;
    }

    // set input variables
    if ( fgHV == _NXNR_HBLK )
    {
        // alias
        #ifndef _C_MODEL_
        pfnGridSet = vDrvNXNRWritePosH;
        #endif

        au2LineSum[0] = _au2HBlkLineSum[0];
        au2LineSum[1] = _au2HBlkLineSum[1];
        au1GridPredict = _au1HBlkGridPredict;

        // input only
        iMaxPeriod = MIN ( rBnrCpt.u1MaxHBlkSize, (UINT8) _NXNR_BNR_FW_MAX_PERIOD );
        iMinPeriod = MAX ( rBnrCpt.u1MinHBlkSize, (UINT8) _NXNR_BNR_FW_MIN_PERIOD );
        iInsideOutsideRatio = rBnrCpt.u1HBlkInsideOutsideRatio * 2;
        iPeriodSensitivity0 = rBnrCpt.u1HBlkPeriodSensitivity0 * 4;
        iPeriodSensitivity1 = rBnrCpt.u1HBlkPeriodSensitivity1 * 4;
        iIntensitySensitivity0 = rBnrCpt.u1HBlkIntensitySensitivity0 * 4;
        iIntensitySensitivity1 = rBnrCpt.u1HBlkIntensitySensitivity1 * 4;
        iAmountSensitivity0 = rBnrCpt.u1HBlkAmountSensitivity0 * 4;
        iAmountSensitivity1 = rBnrCpt.u1HBlkAmountSensitivity1 * 4;
        iPeriodMatchTh = rBnrCpt.u1HBlkPeriodMatchTh * 32;
        iPredictionProceedTh = rBnrCpt.u1HBlkPredictionProceedTh * 32;
        iPredictionValidTh = rBnrCpt.u1HBlkPredictionValidTh * 2;
        iLevelInc = rBnrCpt.u1HBlkLevelInc * 32;
        iLevelDec = rBnrCpt.u1HBlkLevelDec * 32;
        iLevelActiveTh = rBnrCpt.u1HBlkLevelActiveTh * 32;
        iActiveTrialTh = rBnrCpt.u1HBlkActiveTrialTh * 2;

        iDimension = _iHeight;
        iPixCount = _iHBlkPixCount;

        // input & output
        iMajorPeriod = _iHBlkSavedMajorPeriod;
        iMinorPeriodCount = _iHBlkSavedMinorPeriodCount;
        x_memcpy ( iMaxIndex, _iHBlkSavedMaxIndex, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );
        iPeriod = _iHBlkPeriod;
        iLevel = _iHBlkLevel;
        iConfidence = _iHBlkConfidence;
        iActiveTrialCount = _iHBlkActiveTrialCount;
        iIntensity = _iHBlkIntensity;
        iAmount = _iHBlkAmount;
        iTxtrPxlCntTh = 0; // no such thing for HBlk
		x_memcpy ( _au1PeriodHistVBLK, _au1BestPeriodHist, sizeof(UINT8) * (_NXNR_BNR_FW_MAX_PERIOD+2));//for peroidhist debug
    }
    else
    {
        // alias
        #ifndef _C_MODEL_
        pfnGridSet = vDrvNXNRWritePosV;
        #endif

        au2LineSum[0] = _au2VBlkLineSum[0];
        au2LineSum[1] = _au2VBlkLineSum[1];
        au1GridPredict = _au1VBlkGridPredict;

        // input only
        iMaxPeriod = MIN ( rBnrCpt.u1MaxVBlkSize, (UINT8) _NXNR_BNR_FW_MAX_PERIOD );
        iMinPeriod = MAX ( rBnrCpt.u1MinVBlkSize, (UINT8) _NXNR_BNR_FW_MIN_PERIOD );
        iInsideOutsideRatio = rBnrCpt.u1VBlkInsideOutsideRatio * 2;
        iPeriodSensitivity0 = rBnrCpt.u1VBlkPeriodSensitivity0 * 4;
        iPeriodSensitivity1 = rBnrCpt.u1VBlkPeriodSensitivity1 * 4;
        iIntensitySensitivity0 = rBnrCpt.u1VBlkIntensitySensitivity0 * 4;
        iIntensitySensitivity1 = rBnrCpt.u1VBlkIntensitySensitivity1 * 4;
        iAmountSensitivity0 = rBnrCpt.u1VBlkAmountSensitivity0 * 4;
        iAmountSensitivity1 = rBnrCpt.u1VBlkAmountSensitivity1 * 4;
        iPeriodMatchTh = rBnrCpt.u1VBlkPeriodMatchTh * 32;
        iPredictionProceedTh = rBnrCpt.u1VBlkPredictionProceedTh * 32;
        iPredictionValidTh = rBnrCpt.u1VBlkPredictionValidTh * 2;
        iLevelInc = rBnrCpt.u1VBlkLevelInc * 32;
        iLevelDec = rBnrCpt.u1VBlkLevelDec * 32;
        iLevelActiveTh = rBnrCpt.u1VBlkLevelActiveTh * 32;
        iActiveTrialTh = rBnrCpt.u1VBlkActiveTrialTh * 2;

        iDimension = _iWidth;
        iPixCount = _iVBlkPixCount;

        // input & output
        iMajorPeriod = _iVBlkSavedMajorPeriod;
        iMinorPeriodCount = _iVBlkSavedMinorPeriodCount;
        x_memcpy ( iMaxIndex, _iVBlkSavedMaxIndex, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );
        iPeriod = _iVBlkPeriod;
        iLevel = _iVBlkLevel;
        iConfidence = _iVBlkConfidence;
        iActiveTrialCount = _iVBlkActiveTrialCount;
        iIntensity = _iVBlkIntensity;
        iAmount = _iVBlkAmount;
        iTxtrPxlCntTh = _iVBlkTxtrPxlCntTh;
		x_memcpy ( _au1PeriodHistHBLK, _au1BestPeriodHist, sizeof(UINT8) * (_NXNR_BNR_FW_MAX_PERIOD+2));//for peroidhist debug
    }

    // temporal filtering of line sum
    for ( i = 0; i < iDimension; i++ )
    {
        _au2LineSumAvg[i] = ( au2LineSum[0][i] + au2LineSum[1][i] );
    }

    // find period
    bPeriodFound = FALSE;
    iCurrPeriod = 0;
    iCurrPeriodPct = iDimension * iInsideOutsideRatio / 100;
    iLastFoundWindow = -1;
    
    x_memset ( _au1BestPeriod, 0, sizeof ( UINT8 ) * _NXNR_BNR_FW_MAX_DIMENSION );

    for ( n = 2; n * 2 < iMinPeriod; n *= 2 );

    for ( ; n + 1 <= iMaxPeriod; n *= 2 )
    {   
        iWindow = n; // window = [-n:+n]
        iMaxValidPeriod = MIN ( n * 2, iMaxPeriod );
        iMinValidPeriod = MAX ( n + 1, iMinPeriod );

        iPosStart = iWindow;
        iPosEnd = iDimension - 1 - iWindow;
        iPrevPos = 0;
        x_memset ( _au1PeriodHist, 0, sizeof ( UINT8 ) * _NXNR_BNR_FW_MAX_PERIOD + 2 );
        x_memset ( _au1Period, 0, sizeof ( UINT8 ) * _NXNR_BNR_FW_MAX_DIMENSION );

        iPosDelta = 1;
        for ( i = iPosStart; i <= iPosEnd; i += iPosDelta )
        {
            iLineSumSelf = (INT32) _au2LineSumAvg[i];

            for ( d = -iWindow; d <= iWindow; d++ )
            {            
                if ( d == 0 )
                    continue;

                iLineSumNeighbor = (INT32) _au2LineSumAvg[i+d];

                if ( iLineSumSelf <= iLineSumNeighbor )
                {
                    break;
                }
            }

            iPosDelta = ( d < 0 ) ? 1 : d;

            if ( d > iWindow )
            {
                iDistance = i - iPrevPos;
                iPrevPos = i;

                if ( iDistance < iMinPeriod )
                {
                    // period is too small
                    _au1PeriodHist[iMinPeriod - 1]++;
                }
                else if ( iDistance > iMaxPeriod )
                {
                    // period is too large
                    _au1PeriodHist[iMaxPeriod + 2]++;
                }
                else
                {
                    _au1PeriodHist[iDistance]++;
                    _au1Period[i] = ( UINT8 ) iDistance;
                }
            }
        }

        iAllPeriodPct = 0;
        for ( i = iMinPeriod - 1; i <= iMaxPeriod + 1; i++ )
        {
            iAllPeriodPct += _au1PeriodHist[i] * i;
        }

        for ( i = iMinValidPeriod; i <= iMaxValidPeriod; i++ )
        {
            iSelfPeriodPct = _au1PeriodHist[i] * i + _au1PeriodHist[i + 1] * ( i + 1 );
            iOthersPeriodPct = iAllPeriodPct - iSelfPeriodPct;

            bNewBestPeriod = FALSE;
            if ( iSelfPeriodPct > iOthersPeriodPct && iSelfPeriodPct > iCurrPeriodPct )
            {
                if ( ! bPeriodFound || ( iWindow == iLastFoundWindow ) )
                {
                    bNewBestPeriod = TRUE;
                }
                else
                {
                    INT32 iAllPeriodPctX;
                    INT32 iSelfPeriodPctX;
                    INT32 iOthersPeriodPctX;
                
                    iAllPeriodPctX = 0;
                    for ( j = iMinPeriod - 1; j <= iMaxPeriod + 1; j++ )
                    {
                        iAllPeriodPctX += _au1BestPeriodHist[j] * j;
                    }

                    iSelfPeriodPctX = _au1BestPeriodHist[i] * i + _au1BestPeriodHist[i + 1] * ( i + 1 );
                    iOthersPeriodPctX = iAllPeriodPctX - iSelfPeriodPctX;

                    if ( iSelfPeriodPctX > iOthersPeriodPctX && iSelfPeriodPctX > iCurrPeriodPct )
                    {
                        bNewBestPeriod = TRUE;
                    }
                }
            }

            if ( bNewBestPeriod )
            {
                if ( iWindow != iLastFoundWindow )
                {
                    x_memcpy ( _au1BestPeriod, _au1Period, sizeof(UINT8) * _NXNR_BNR_FW_MAX_DIMENSION );
                    x_memcpy ( _au1BestPeriodHist, _au1PeriodHist, sizeof(UINT8) * ( _NXNR_BNR_FW_MAX_PERIOD+2 )  );
                }            
                iCurrPeriod = iSelfPeriodPct * 1000 / ( _au1PeriodHist[i] + _au1PeriodHist[i + 1] );
				if ( fgHV == _NXNR_HBLK )
				{
					_SWW(NR_STA_BNR_04,iCurrPeriod/1000,NR_STA_BNR_HBLK_PERIOD);
				}
				else
				{
					_SWW(NR_STA_BNR_04,iCurrPeriod/1000,NR_STA_BNR_VBLK_PERIOD);
				}
                iCurrPeriodPct = iSelfPeriodPct;
                bPeriodFound = TRUE;
                iLastFoundWindow = iWindow;
            }
        }
    }
    
	u4HomoSum = NR_R(MCVP_FUSION_13, IF_PD_SUM_HOMO_UP) + NR_R(MCVP_FUSION_15, IF_PD_SUM_HOMO_DN);
	if(
        ((u4HomoSum >> 10)<= _SWR(NR_FWBNR_08,NR_FWBNR_HOMOSUM_TH)) &&
		(_SWR(NR_NMSTA_17,NR_NM_STA_FATTAIL_RTO) <= _SWR(NR_FWBNR_08,NR_NM_STA_FATTAIL_RTO_TH)) &&
		((_SWR(NR_NMSTA_17,NR_NM_STA_TAIL_SUM) >> 4) <= _SWR(NR_FWBNR_08,NR_NM_STA_TAIL_SUM_TH)) &&
        (IO32ReadFldAlign(METER_INFO_00, METER_ASL_VAL) <= _SWR(NR_FWBNR_08,NR_FWBNR_ASL_TH))		

	    )
	{
		bPeriodFound = FALSE;
	}
    
    u1Flg |= (UINT8) ( bPeriodFound ? 1 : 0 );
    // update period
    bPeriodMatch = FALSE;

    iSensitivity = iPeriodSensitivity0 + ( iConfidence * ( iPeriodSensitivity1 - iIntensitySensitivity1 ) + 8 ) / 16;
    iRound = ( iCurrPeriod > iPeriod ) ? 63 : 0;
    iPeriod = iPeriod + ( ( iCurrPeriod - iPeriod ) * iSensitivity + iRound ) / 64;

    if ( bPeriodFound )
    {
        if ( ABS ( iCurrPeriod - iPeriod ) <= iPeriodMatchTh )
        {
            bPeriodMatch = TRUE;

            iPeriod0 = MAX ( 1, iPeriod / 1000 );
            iPeriod1 = iPeriod0 + 1;

            for ( i = 0; i < iDimension; i++ )
            {
                bCond0 = ( (INT32) _au1BestPeriod[i] == iPeriod0 );
                bCond1 = ( (INT32) _au1BestPeriod[i] == iPeriod1 );
                bCond2 = ( ( i + iPeriod0 < iDimension ) && ( (INT32) _au1BestPeriod[i + iPeriod0] == iPeriod0 ) );
                bCond3 = ( ( i + iPeriod1 < iDimension ) && ( (INT32) _au1BestPeriod[i + iPeriod1] == iPeriod1 ) );

                if ( ! ( bCond0 || bCond1 || bCond2 || bCond3 ) )
                {
                    _au2LineSumAvg[i] = 0;
                }
            }
        }
    }

    u1Flg |= (UINT8) ( bPeriodMatch ? 2 : 0 );
    // predict grid
    bPredictionFound = FALSE;
    iCurrMajorPeriod = 0;
    iCurrMinorPeriodCount = 0;
   
    if ( bPeriodMatch )
    {
        for ( iCurrMinorPeriodCount = 1; iCurrMinorPeriodCount <= _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT; iCurrMinorPeriodCount++ )
        {                       
            iCurrMajorPeriod = ( iPeriod * iCurrMinorPeriodCount + 500 ) / 1000;

            if ( ABS ( iCurrMajorPeriod * 1000 - iPeriod * iCurrMinorPeriodCount ) > iPredictionProceedTh )
            {
                continue;
            }

            if ( iCurrMajorPeriod > _NXNR_BNR_FW_MAX_MAJOR_PERIOD )
            {
                break;
            }

            x_memset ( iHist, 0, sizeof(INT32) * _NXNR_BNR_FW_MAX_MAJOR_PERIOD );
            x_memset ( iMax, 0, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );
            x_memset ( iCurrMaxIndex, 0, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );

            i = 0; 
            j = 0;
            while ( i < iDimension )
            {
                iHist[j++] += (INT32) _au2LineSumAvg[i++];
                if ( j == iCurrMajorPeriod ) j = 0;
            }

            iTotalSum = 0;
            for ( i = 0; i < iCurrMajorPeriod; i++ )
            {
                iTotalSum += iHist[i];

                for ( j = iCurrMinorPeriodCount - 1; j >= 0; j-- )
                {
                    if ( iHist[i] > iMax[j] )
                    {
                        for ( k = 0; k < j; k++ )
                        {
                            iMax[k] = iMax[k+1];
                            iCurrMaxIndex[k] = iCurrMaxIndex[k+1];
                        }
                        iMax[j] = iHist[i];
                        iCurrMaxIndex[j] = i;
                        break;
                    }
                }
            }

            iMaxSum = 0;
            for ( i = 0; i < iCurrMinorPeriodCount; i++ )
            {
                iMaxSum += iMax[i];
            }

            if ( iMaxSum * ( iCurrMajorPeriod - iCurrMinorPeriodCount ) > ( iTotalSum - iMaxSum ) * iCurrMinorPeriodCount * iPredictionValidTh )
            {
                if ( iCurrMinorPeriodCount == 1 )
                {
                    bPredictionFound = TRUE;
                }
                else
                {
                    _BNR_FW_SortAscend ( iCurrMaxIndex, iCurrMinorPeriodCount );

                    bPredictionFound = TRUE;
                    for ( i = 0; i < iCurrMinorPeriodCount - 1; i++ )
                    {
                        iDiff = ( iCurrMaxIndex[i+1] - iCurrMaxIndex[i] );
                        if ( iDiff != iPeriod0 && iDiff != iPeriod1 )
                        {
                            bPredictionFound = FALSE;
                        }
                    }
                    iDiff = ( iCurrMajorPeriod + iCurrMaxIndex[0] - iCurrMaxIndex[iCurrMinorPeriodCount-1] );
                    if ( iDiff != iPeriod0 && iDiff != iPeriod1 )
                    {
                        bPredictionFound = FALSE;
                    }
                }        
            }

            if ( bPredictionFound )
            {
                break;
            }
        }
    }

    u1Flg |= (UINT8) ( bPredictionFound ? 4 : 0 );
    // check prediction consistency
    bPredictionMatch = FALSE;

    if ( bPredictionFound )
    {
        if ( iLevel == 0 )
        {
            bPredictionMatch = TRUE;

            iMajorPeriod = iCurrMajorPeriod;
            iMinorPeriodCount = iCurrMinorPeriodCount;
            x_memcpy ( iMaxIndex, iCurrMaxIndex, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );
        }
        else
        {
            bPredictionMatch = TRUE;

            if ( iMajorPeriod != iCurrMajorPeriod )
            {
                bPredictionMatch = FALSE;
            }
            else if ( iMinorPeriodCount != iCurrMinorPeriodCount )
            {
                bPredictionMatch = FALSE;
            }
            else
            {
                for ( i = 0; i < iMinorPeriodCount; i++ )
                {
                    if ( iMaxIndex[i] != iCurrMaxIndex[i] )
                    {
                        bPredictionMatch = FALSE;
                    }
                }
            }
        }
    }

    u1Flg |= (UINT8) ( bPredictionMatch ? 8 : 0 );
    // update confidence
    if ( bPredictionMatch )
    {
        iLevel = MIN ( 2048, iLevel + iLevelInc );
    }
    else
    {
        iLevel = MAX (    0, iLevel - iLevelDec );
    }

    if ( iLevel < iLevelActiveTh )
    {
        iActiveTrialCount++;
        if ( iActiveTrialCount > iActiveTrialTh )
        {
            _BNR_FW_ResetGridDet ( fgHV );
            return;
        }
    }
    else
    {
        iActiveTrialCount = 0;
    }

    iConfidence = MIN ( 16, MAX ( 0, ( iLevel - iLevelActiveTh ) / 64 ) );

    // set grid position
    x_memset ( au1GridPredict, 0, sizeof(UINT8) * iDimension );

    if ( iConfidence > 0 )
    {
        for ( i = 0; i < iDimension; i+= iMajorPeriod )
        {
            for ( j = 0; j < iMinorPeriodCount; j++ )
            {
                k = i + iMaxIndex[j];
                if ( k < iDimension )
                {
                    au1GridPredict[k] = 0x3;
                }
            }
        }
    }

    // set block size related control
    if ( iConfidence > 0 && fgHV == _NXNR_VBLK )
    {
        iPeriod0 = iMajorPeriod / MAX ( 1, iMinorPeriodCount );

        if ( iPeriod0 < rBnrCpt.u1TxtrPxlCntThSize0 )
        {
            iTxtrPxlCntTh = rBnrCpt.u1TxtrPxlCntThValue0;
        } 
        else if ( iPeriod0 < rBnrCpt.u1TxtrPxlCntThSize1 )
        {
            iTxtrPxlCntTh = rBnrCpt.u1TxtrPxlCntThValue0 + ( rBnrCpt.u1TxtrPxlCntThValue1 - rBnrCpt.u1TxtrPxlCntThValue0 ) * ( iPeriod0 - rBnrCpt.u1TxtrPxlCntThSize0 ) / MAX ( 1, ( rBnrCpt.u1TxtrPxlCntThSize1 - rBnrCpt.u1TxtrPxlCntThSize0 ) );
        }
        else if ( iPeriod0 < rBnrCpt.u1TxtrPxlCntThSize2 )
        {
            iTxtrPxlCntTh = rBnrCpt.u1TxtrPxlCntThValue1 + ( rBnrCpt.u1TxtrPxlCntThValue2 - rBnrCpt.u1TxtrPxlCntThValue1 ) * ( iPeriod0 - rBnrCpt.u1TxtrPxlCntThSize1 ) / MAX ( 1, ( rBnrCpt.u1TxtrPxlCntThSize2 - rBnrCpt.u1TxtrPxlCntThSize1 ) );
        }
        else if ( iPeriod0 < rBnrCpt.u1TxtrPxlCntThSize3 )
        {
            iTxtrPxlCntTh = rBnrCpt.u1TxtrPxlCntThValue2 + ( rBnrCpt.u1TxtrPxlCntThValue3 - rBnrCpt.u1TxtrPxlCntThValue2 ) * ( iPeriod0 - rBnrCpt.u1TxtrPxlCntThSize2 ) / MAX ( 1, ( rBnrCpt.u1TxtrPxlCntThSize3 - rBnrCpt.u1TxtrPxlCntThSize2 ) );
        }
        else
        {
            iTxtrPxlCntTh = rBnrCpt.u1TxtrPxlCntThValue3;
        }
    }

    // report status
    if ( iConfidence > 0 )
    {
        iGridLineSum = 0;
        iAllLineSum = 0;

        for ( i = 0; i < iDimension; i++ )
        {
            if ( au1GridPredict[i] != 0 )
            {
                iGridLineSum += _au2LineSumAvg[i];
            }

            iAllLineSum += _au2LineSumAvg[i];
        }

        iGridLineSum /= 2;
        iAllLineSum /= 2;

        iCurrIntensity = iAllLineSum * 100 / MAX ( 1, iPixCount );
        iCurrAmount = iGridLineSum * 100 / MAX ( 1, iCurrIntensity );

        iSensitivity = iIntensitySensitivity0 + ( iConfidence * ( iIntensitySensitivity1 - iIntensitySensitivity0 ) + 8 ) / 16;
        iRound = ( iCurrIntensity - iIntensity ) ? 63 : 0;
        iIntensity = iIntensity + ( ( iCurrIntensity - iIntensity ) * iSensitivity + iRound ) / 64;

        iSensitivity = iAmountSensitivity0 + ( iConfidence * ( iAmountSensitivity1 - iAmountSensitivity0 ) + 8 ) / 16;
        iRound = ( iCurrAmount - iAmount ) ? 63 : 0;
        iAmount = iAmount + ( ( iCurrAmount - iAmount ) * iSensitivity + iRound ) / 64;
    }
    
#ifndef _C_MODEL_
    for ( i = 0; i < iDimension; i++ )
    {
        pfnGridSet(i, au1GridPredict[i]);
    }
#endif

    // set output variables
    if ( fgHV == _NXNR_HBLK )
    {
        _iHBlkSavedMajorPeriod = iMajorPeriod;
        _iHBlkSavedMinorPeriodCount = iMinorPeriodCount;
        x_memcpy ( _iHBlkSavedMaxIndex, iMaxIndex, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );
        _iHBlkPeriod = iPeriod;
        _iHBlkLevel = iLevel;
        _iHBlkConfidence = iConfidence;
        _iHBlkActiveTrialCount = iActiveTrialCount;
        _iHBlkIntensity = iIntensity;
        _iHBlkAmount = iAmount;
        _u1HBlkFlg = u1Flg;
    }
    else
    {
        _iVBlkSavedMajorPeriod = iMajorPeriod;
        _iVBlkSavedMinorPeriodCount = iMinorPeriodCount;
        x_memcpy ( _iVBlkSavedMaxIndex, iMaxIndex, sizeof(INT32) * _NXNR_BNR_FW_MAX_MINOR_PERIOD_COUNT );
        _iVBlkPeriod = iPeriod;
        _iVBlkLevel = iLevel;
        _iVBlkConfidence = iConfidence;
        _iVBlkActiveTrialCount = iActiveTrialCount;
        _iVBlkIntensity = iIntensity;
        _iVBlkAmount = iAmount;
        _u1VBlkFlg = u1Flg;
        _iVBlkTxtrPxlCntTh = iTxtrPxlCntTh;
    }

#ifdef _C_MODEL_
#ifndef _DEBUG
#if 0
    char path[256];
    char filename[256];
    _mkdir( "fw_log" );
    sprintf( path, "fw_log\\%s", szPatternName );
    _mkdir( path );
    char * main_file[2] = {"BNR_H.csv", "BNR_V.csv"};
    char * line_sum_file[2] = {"BNR_H_line_sum.csv", "BNR_V_line_sum.csv"};
    char * period_file[2] = {"BNR_H_best_period.csv", "BNR_V_best_period.csv"};
    char * predict_file[2] = {"BNR_H_grid_predict.csv", "BNR_V_grid_predict.csv"};

    bool bExist;
    sprintf( filename, "%s\\%s", path, main_file[fgHV] );
    FILE * fp = fopen( filename, "r");
    if( fp )
    {
      bExist = true;
      fclose(fp);
    } else {
      bExist = false;
    }

    fp = fopen(filename, "a" );
    if ( ! bExist )
      fprintf( fp, "index, bPeriodFound, iCurrPeriod, iCurrPeriodPct, iPeriod, bPeriodMatch, bPredictionFound, bPredictionMatch, iLevel, iConfidence, iActiveTrialCount, iIntensity, iAmount\n" );
    fprintf( fp, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, ", index, bPeriodFound, iCurrPeriod, iCurrPeriodPct, iPeriod, bPeriodMatch, bPredictionFound, bPredictionMatch, iLevel, iConfidence, iActiveTrialCount, iIntensity, iAmount );
    fprintf( fp, "%d, %d, ", iCurrMajorPeriod, iCurrMinorPeriodCount );
    for ( i = 0; i < iCurrMinorPeriodCount; i++ )
    {
      fprintf( fp, "%d, ", iCurrMaxIndex[i] );
    }
    fprintf( fp, "%d, %d, ", iMajorPeriod, iMinorPeriodCount );
    for ( i = 0; i < iMinorPeriodCount; i++ )
    {
      fprintf( fp, "%d, ", iMaxIndex[i] );
    }
    fprintf( fp, "\n" );
    fclose( fp );

    sprintf( filename, "%s\\%s", path, line_sum_file[fgHV] );
    fp = fopen( filename, "a" );
    fprintf( fp, "%d,", index );
    for ( i = 0; i < iDimension; i++ )
    {
      fprintf( fp, "%d,", _au2LineSumAvg[i] );
    }
    fprintf( fp, "\n" );
    fclose( fp );

    sprintf( filename, "%s\\%s", path, period_file[fgHV] );
    fp = fopen( filename, "a" );
    fprintf( fp, "%d,", index );
    for ( i = 0; i < iDimension; i++ )
    {
      fprintf( fp, "%d,", _au1BestPeriod[i] );
    }
    fprintf( fp, "\n" );
    fclose( fp );

    sprintf( filename, "%s\\%s", path, predict_file[fgHV] );
    fp = fopen( filename, "a" );
    fprintf( fp, "%d,", index );
    for ( i = 0; i < iDimension; i++ )
    {
      fprintf( fp, "%d,", au1GridPredict[i] );
    }
    fprintf( fp, "\n" );
    fclose( fp );
#endif
#endif

    if ( fgHV == _NXNR_VBLK )
    {
       index++;
    }
#endif
}

static void _BNR_FW_GetLineSum(void)
{
#ifndef _C_MODEL_
    UINT32 i;
    UINT32 u4Width, u4Height;
    UINT32 u4VBlkFrmSum        = 0;
    UINT32 u4HBlkFrmSum        = 0;
    //UINT32 u4VGridMinLineSumTh = rBnrCpt.iVGridMinLineSumTh;
    //UINT32 u4HGridMinLineSumTh = rBnrCpt.iHGridMinLineSumTh;
    UINT32 u4VLineSumPadBits   = rBnrCpt.u1VBlkLsPadBits;
    UINT32 u4HLineSumPadBits   = rBnrCpt.u1HBlkLsPadBits;
    UINT32 u4Idx               = _iLineSumArrayIndex;

    u4Width  = (UINT32) _iWidth;
    u4Height = (UINT32) _iHeight;

    for (i = 0; i < u4Width; i++)
    {
        UINT32 u4Tmp = u2DrvNXNRGetVBlkLineSum(rBnrCpt.fgUseR, i) << u4VLineSumPadBits;
        u4VBlkFrmSum += u4Tmp;
        _au2VBlkLineSum[u4Idx][i] = u4Tmp; //MAX(u4Tmp, u4VGridMinLineSumTh);
    }

    for (i = 0; i < u4Height; i++)
    {
        UINT32 u4Tmp = u2DrvNXNRGetHBlkLineSum(rBnrCpt.fgUseR, i) << u4HLineSumPadBits;
        u4HBlkFrmSum += u4Tmp;
        _au2HBlkLineSum[u4Idx][i] = u4Tmp; //MAX(u4Tmp, u4HGridMinLineSumTh);
    }

    _u2VBlkPixCnt = u2DrvNXNRGetVBlkPxCnt(rBnrCpt.fgUseR);
    _u2HBlkPixCnt = u2DrvNXNRGetHBlkPxCnt(rBnrCpt.fgUseR);
    _u1VBlkAvgLS = (_u2VBlkPixCnt != 0) ? (u4VBlkFrmSum / _u2VBlkPixCnt) : 0;
    _u1HBlkAvgLS = (_u2HBlkPixCnt != 0) ? (u4HBlkFrmSum / _u2HBlkPixCnt) : 0;

    _iLineSumArrayIndex = ( _iLineSumArrayIndex + 1 ) & 0x1;

    _SWW(NR_BLK_STA_13, MIN(u4VBlkFrmSum, 0xFFFF), NR_STA_VBLK_FRMSUM);
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
    int iVLineSumPadBits = ( pNXNR->cCsr.nx_vblk_line_sum_sat_lv == 0 ) ? 2 : 3;
    int iHLineSumPadBits = ( pNXNR->cCsr.nx_hblk_line_sum_sat_lv == 0 ) ? 2 : 3;

    for ( int x = 0; x < _iWidth; x++ )
    {
        _au2VBlkLineSum[_iLineSumArrayIndex][x] = (UINT16) ( pNXNR->pVBlkLineSumOut[x] << iVLineSumPadBits );
    }

    for ( int y = 0; y < _iHeight; y++ )
    {
        _au2HBlkLineSum[_iLineSumArrayIndex][y] = (UINT16) ( pNXNR->pHBlkLineSumOut[y] << iHLineSumPadBits );
    }

    _iLineSumArrayIndex = ( _iLineSumArrayIndex + 1 ) % 2;

    _iHBlkPixCount = pNXNR->cCsr.nx_sta_hblk_pix_cnt * 16;
    _iVBlkPixCount = pNXNR->cCsr.nx_sta_vblk_pix_cnt * 16;

    // MNR begin
    _iMosPixCount = pNXNR->cCsr.nx_sta_mos_pix_count;
    _iMosPixDiffSum = pNXNR->cCsr.nx_sta_mos_pix_diff_sum;
    // MNR end
#else
    vDrvNRLRSwap();
    if (rBnrCpt.fgEnLSumRead)
        u1DrvNXNRReadBlkLineSum();

    _iVBlkPixCount = NR_R(NXNR_13, NX_STA_VBLK_PIX_CNT) << 4;
    _iHBlkPixCount = NR_R(NXNR_13, NX_STA_HBLK_PIX_CNT) << 4;

    // MNR begin
    _iMosPixCount = NR_R(NXNR_29, NX_STA_MOS_PIX_COUNT);
    _iMosPixDiffSum = NR_R(NXNR_2A, NX_STA_MOS_PIX_DIFF_SUM);
    // MNR end
#endif
}

static void _BNR_FW_WriteBlkPos(void)
{
#ifdef _C_MODEL_
    for ( int x = 0; x < _iWidth; x++ )
    {
        pNXNR->pVBlkPos[x] = _au1VBlkGridPredict[x];
    }

    for ( int y = 0; y < _iHeight; y++ )
    {
        pNXNR->pHBlkPos[y] = _au1HBlkGridPredict[y];
    }
#else
    if (rBnrCpt.fgEnLocWrite)
        u1DrvNXNRWriteBlkPos();
#endif
}

static void _BNR_FW_WriteReg(void)
{
#ifdef _C_MODEL_
    pNXNR->cCsr.nx_manual_hblk_size = 8;
    pNXNR->cCsr.nx_manual_vblk_size = 8;
    pNXNR->cCsr.nx_manual_vblk_size_pr_r = 8;

    pNXNR->cCsr.nx_manual_hblk_confidence = _iHBlkConfidence;
    pNXNR->cCsr.nx_manual_vblk_confidence = _iVBlkConfidence;
    pNXNR->cCsr.nx_manual_vblk_confidence_pr_r = _iVBlkConfidence;

    pNXNR->cCsr.nx_bnr_texture_th1 = _iVBlkTxtrPxlCntTh;

    // MNR begin
    pNXNR->cCsr.nx_demos_gain = _iMnrGain;
    // MNR end
#else
    NR_W(NXNR_0D, _iVBlkConfidence, NX_MANUAL_VBLK_CONFIDENCE);
    NR_W(NXNR_0C, _iHBlkConfidence, NX_MANUAL_HBLK_CONFIDENCE);
    NR_W(NXNR_16, _iVBlkConfidence, NX_MANUAL_VBLK_CONFIDENCE_PR_R);
    NR_W(NXNR_2C, _iVBlkTxtrPxlCntTh, NX_VBLK_TXTR_TAP_TH);

    if (rBnrCpt.fgEnMnrAuto)
        NR_W(NXNR_04, _iMnrGain, NX_MNR_GAIN);

    NR_W(NXNR_2B, _iBnrVblkBsFltStr, NX_VBLK_BASE_FILTER_STR);
    NR_W(NXNR_2C, _iBnrVblkTxBsStrSlp, NX_VBLK_TXTR_BASE_STR_SLOPE);
    NR_W(NXNR_2C, _iBnrVblkTxBsStrTh, NX_VBLK_TXTR_BASE_STR_TH);      
    
#endif
}

static void _BNR_FW_WriteRegOnce(void)
{
#ifdef _C_MODEL_
    pNXNR->cCsr.nx_blk_loc_mode = 1;
    pNXNR->cCsr.nx_force_hblk_confidence = 1;
    pNXNR->cCsr.nx_force_hblk_size_phase = 1;
    pNXNR->cCsr.nx_manual_hblk_size = 8;
    pNXNR->cCsr.nx_manual_hblk_phase = 0;
    pNXNR->cCsr.nx_force_vblk_confidence = 1;
    pNXNR->cCsr.nx_force_vblk_size_phase = 1;
    pNXNR->cCsr.nx_manual_vblk_size = 8;
    pNXNR->cCsr.nx_manual_vblk_phase = 0;
    pNXNR->cCsr.nx_manual_vblk_size_pr_r = 8;
    pNXNR->cCsr.nx_manual_vblk_phase_pr_r = 0;

    if ( _iWidth > 720 )
    {
        // HD
        pNXNR->cCsr.nx_vblk_line_sum_sat_lv = 2; // 4095
        pNXNR->cCsr.nx_hblk_line_sum_sat_lv = 3; // 16383
    }
    else
    {
        // SD
        pNXNR->cCsr.nx_vblk_line_sum_sat_lv = 1; // 2047
        pNXNR->cCsr.nx_hblk_line_sum_sat_lv = 2; // 8191
    }

    // MNR begin
    pNXNR->cCsr.nx_mos_sta_max_str = 8;
    pNXNR->cCsr.nx_mos_sta_min_str = 1;

    pNXNR->cCsr.nx_mos_act_quant = 0x1;
    pNXNR->cCsr.nx_mos_edge_th = 0xA;
    pNXNR->cCsr.nx_mos_edge_gain = 0x6;
    pNXNR->cCsr.nx_mos_edge_mode = 0x3;
    // MNR end
#else
    UINT32  u4MIB     = u4DrvTDTVDIModeQuery();
    E_TD_OUT eMIBOUT = TD_MIB_OUT(u4MIB);
    NR_W(NXNR_05, 1, NX_BLK_LOC_MODE);
    NR_W(NXNR_00, 3, NX_DEBLOCK_V_MASK);
    NR_W(NXNR_0C, (eMIBOUT!=E_TD_OUT_3D_LI), NX_FORCE_HBLK_CONFIDENCE);
    NR_W(NXNR_0C, (eMIBOUT!=E_TD_OUT_3D_LI), NX_FORCE_HBLK_SIZE_PHASE);
    NR_W(NXNR_0C, 8, NX_MANUAL_HBLK_SIZE);
    NR_W(NXNR_0C, 0, NX_MANUAL_HBLK_PHASE);
    NR_W(NXNR_0D, 1, NX_FORCE_VBLK_CONFIDENCE);
    NR_W(NXNR_0D, 1, NX_FORCE_VBLK_SIZE_PHASE);
    NR_W(NXNR_0D, 8, NX_MANUAL_VBLK_SIZE);
    NR_W(NXNR_0D, 0, NX_MANUAL_VBLK_PHASE);
    NR_W(NXNR_16, 8, NX_MANUAL_VBLK_SIZE_PR_R);
    NR_W(NXNR_16, 0, NX_MANUAL_VBLK_PHASE_PR_R);

    NR_W(NXNR_28, 8, NX_MOS_STA_MAX_STR);
    NR_W(NXNR_28, 1, NX_MOS_STA_MIN_STR);
    NR_W(NXNR_09, 1, NX_VBLK_LINE_SUM_SAT_LV);
    NR_W(NXNR_09, 2, NX_HBLK_LINE_SUM_SAT_LV);
#endif
}

static void _BNR_FW_GetControlPoints(void)
{
#ifndef _C_MODEL_
    rBnrCpt.u1MinHBlkSize               = _SWR(NR_FWBNR_01, NR_FWBNR_MIN_H_BK_SIZE);
    rBnrCpt.u1MinVBlkSize               = _SWR(NR_FWBNR_01, NR_FWBNR_MIN_V_BK_SIZE);
    rBnrCpt.u1MaxHBlkSize               = _SWR(NR_FWBNR_01, NR_FWBNR_MAX_H_BK_SIZE);
    rBnrCpt.u1MaxVBlkSize               = _SWR(NR_FWBNR_01, NR_FWBNR_MAX_V_BK_SIZE);
    rBnrCpt.u1HBlkPeriodSensitivity0    = _SWR(NR_FWBNR_02, NR_FWBNR_HBLK_PS0);
    rBnrCpt.u1VBlkPeriodSensitivity0    = _SWR(NR_FWBNR_02, NR_FWBNR_VBLK_PS0);
    rBnrCpt.u1HBlkPeriodSensitivity1    = _SWR(NR_FWBNR_02, NR_FWBNR_HBLK_PS1);
    rBnrCpt.u1VBlkPeriodSensitivity1    = _SWR(NR_FWBNR_02, NR_FWBNR_VBLK_PS1);
    rBnrCpt.u1HBlkIntensitySensitivity0 = _SWR(NR_FWBNR_02, NR_FWBNR_HBLK_IS0);
    rBnrCpt.u1VBlkIntensitySensitivity0 = _SWR(NR_FWBNR_02, NR_FWBNR_VBLK_IS0);
    rBnrCpt.u1HBlkIntensitySensitivity1 = _SWR(NR_FWBNR_02, NR_FWBNR_HBLK_IS1);
    rBnrCpt.u1VBlkIntensitySensitivity1 = _SWR(NR_FWBNR_02, NR_FWBNR_VBLK_IS1);
    rBnrCpt.u1HBlkAmountSensitivity0    = _SWR(NR_FWBNR_03, NR_FWBNR_HBLK_AS0);
    rBnrCpt.u1VBlkAmountSensitivity0    = _SWR(NR_FWBNR_03, NR_FWBNR_VBLK_AS0);
    rBnrCpt.u1HBlkAmountSensitivity1    = _SWR(NR_FWBNR_03, NR_FWBNR_HBLK_AS1);
    rBnrCpt.u1VBlkAmountSensitivity1    = _SWR(NR_FWBNR_03, NR_FWBNR_VBLK_AS1);
    rBnrCpt.u1HBlkPeriodMatchTh         = _SWR(NR_FWBNR_03, NR_FWBNR_HBLK_PMATCH_TH);
    rBnrCpt.u1VBlkPeriodMatchTh         = _SWR(NR_FWBNR_03, NR_FWBNR_VBLK_PMATCH_TH);
    rBnrCpt.u1HBlkPredictionProceedTh   = _SWR(NR_FWBNR_03, NR_FWBNR_HBLK_PDCT_PCD_TH);
    rBnrCpt.u1VBlkPredictionProceedTh   = _SWR(NR_FWBNR_03, NR_FWBNR_VBLK_PDCT_PCD_TH);
    rBnrCpt.u1HBlkPredictionValidTh     = _SWR(NR_FWBNR_04, NR_FWBNR_HBLK_PDCT_VLD_TH);
    rBnrCpt.u1VBlkPredictionValidTh     = _SWR(NR_FWBNR_04, NR_FWBNR_VBLK_PDCT_VLD_TH);

    rBnrCpt.u1HBlkLevelActiveTh         = _SWR(NR_FWBNR_04, NR_FWBNR_HBLK_LVL_ACT_TH); 
    rBnrCpt.u1HBlkLevelInc              = _SWR(NR_FWBNR_04, NR_FWBNR_HBLK_LVL_INC); 
    rBnrCpt.u1HBlkLevelDec              = _SWR(NR_FWBNR_04, NR_FWBNR_HBLK_LVL_DEC); 
    rBnrCpt.u1HBlkInsideOutsideRatio    = _SWR(NR_FWBNR_01, NR_FWBNR_HBLK_IO_RTO); 
    rBnrCpt.u1HBlkActiveTrialTh         = _SWR(NR_FWBNR_05, NR_FWBNR_HBLK_ACT_TRIAL_TH);

    rBnrCpt.u1VBlkLevelActiveTh         = _SWR(NR_FWBNR_04, NR_FWBNR_VBLK_LVL_ACT_TH); 
    rBnrCpt.u1VBlkLevelInc              = _SWR(NR_FWBNR_04, NR_FWBNR_VBLK_LVL_INC); 
    rBnrCpt.u1VBlkLevelDec              = _SWR(NR_FWBNR_04, NR_FWBNR_VBLK_LVL_DEC); 
    rBnrCpt.u1VBlkInsideOutsideRatio    = _SWR(NR_FWBNR_01, NR_FWBNR_VBLK_IO_RTO); 
    rBnrCpt.u1VBlkActiveTrialTh         = _SWR(NR_FWBNR_05, NR_FWBNR_VBLK_ACT_TRIAL_TH);

    rBnrCpt.u1TxtrPxlCntThSize0         = _SWR(NR_FWBNR_06, NR_FWBNR_TXPXL_CNT_TH_SZ_0);
    rBnrCpt.u1TxtrPxlCntThSize1         = _SWR(NR_FWBNR_06, NR_FWBNR_TXPXL_CNT_TH_SZ_1);
    rBnrCpt.u1TxtrPxlCntThSize2         = _SWR(NR_FWBNR_06, NR_FWBNR_TXPXL_CNT_TH_SZ_2);
    rBnrCpt.u1TxtrPxlCntThSize3         = _SWR(NR_FWBNR_06, NR_FWBNR_TXPXL_CNT_TH_SZ_3);

    rBnrCpt.u1TxtrPxlCntThValue0        = _SWR(NR_FWBNR_07, NR_FWBNR_TXPXL_CNT_TH_VAL_0);
    rBnrCpt.u1TxtrPxlCntThValue1        = _SWR(NR_FWBNR_07, NR_FWBNR_TXPXL_CNT_TH_VAL_1);
    rBnrCpt.u1TxtrPxlCntThValue2        = _SWR(NR_FWBNR_07, NR_FWBNR_TXPXL_CNT_TH_VAL_2);
    rBnrCpt.u1TxtrPxlCntThValue3        = _SWR(NR_FWBNR_07, NR_FWBNR_TXPXL_CNT_TH_VAL_3);

    rBnrCpt.fgUseR                      = _SWR(NR_FWBNR_00, NR_FWBNR_READ_RIGHT);
    rBnrCpt.fgEnLocWrite                = _SWR(NR_FWBNR_00, NR_FWBNR_LOC_WRITE);
    rBnrCpt.fgEnLSumRead                = _SWR(NR_FWBNR_00, NR_FWBNR_LSUM_READ);
    rBnrCpt.fgPatchCG                   = _SWR(NR_FWBNR_00, NR_FWBNR_CG_PATCH0);
    rBnrCpt.fgEnVDet                    = _SWR(NR_FWBNR_00, NR_FWBNR_VDET_EN);
    rBnrCpt.fgEnHDet                    = _SWR(NR_FWBNR_00, NR_FWBNR_HDET_EN);

    rBnrCpt.u1HBlkLsPadBits             = NR_R(NXNR_09, NX_HBLK_LINE_SUM_SAT_LV) + 2;
    rBnrCpt.u1VBlkLsPadBits             = NR_R(NXNR_09, NX_VBLK_LINE_SUM_SAT_LV) + 2;

    rBnrCpt.u1MnrLowTh                  = _SWR(NR_MNR_00, NR_MNR_LO_TH);
    rBnrCpt.u1MnrLowGain                = _SWR(NR_MNR_00, NR_MNR_LO_GAIN);
    rBnrCpt.u1MnrHighTh                 = _SWR(NR_MNR_00, NR_MNR_HI_TH);
    rBnrCpt.u1MnrHighGain               = _SWR(NR_MNR_00, NR_MNR_HI_GAIN);
    rBnrCpt.u1MnrSensitivity            = _SWR(NR_MNR_00, NR_MNR_UPDATE_SPEED);
    rBnrCpt.fgEnMnrAuto                 = _SWR(NR_MNR_00, NR_MNR_AUTO_EN);
    
    rBnrCpt.fgEnBnrThAuto               = _SWR(NR_FWBNR_09, NR_FWBNR_ADAP_EN);
    rBnrCpt.u1BnrAdapNMTh1              = _SWR(NR_FWBNR_09, NR_FWBNR_ADAP_TH1);
    rBnrCpt.u1BnrAdapNMTh2              = _SWR(NR_FWBNR_09, NR_FWBNR_ADAP_TH2);
    rBnrCpt.u1BnrAdapVBFSTh1            = _SWR(NR_FWBNR_0A, NR_FWBNR_ADAP_VBFS_TH1);
    rBnrCpt.u1BnrAdapVBFSTh2            = _SWR(NR_FWBNR_0B, NR_FWBNR_ADAP_VBFS_TH2);
    rBnrCpt.u1BnrAdapVTBSSTh1           = _SWR(NR_FWBNR_0A, NR_FWBNR_ADAP_VTBSS_TH1);
    rBnrCpt.u1BnrAdapVTBSSTh2           = _SWR(NR_FWBNR_0B, NR_FWBNR_ADAP_VTBSS_TH2);
    rBnrCpt.u1BnrAdapVTBSHTh1           = _SWR(NR_FWBNR_0A, NR_FWBNR_ADAP_VTBSH_TH1);
    rBnrCpt.u1BnrAdapVTBSHTh2           = _SWR(NR_FWBNR_0B, NR_FWBNR_ADAP_VTBSH_TH2);
    
#endif
}

static void _BNR_FW_SetControlPoints(void)
{
#ifdef _C_MODEL_
    rBnrCpt.u1MinHBlkSize = 8; // 0-15
    rBnrCpt.u1MinVBlkSize = 8; // 0-15

    rBnrCpt.u1MaxHBlkSize = 24; // 0-255
    rBnrCpt.u1MaxVBlkSize = 24; // 0-255

    rBnrCpt.u1HBlkInsideOutsideRatio = 8; // 2 x 0-15
    rBnrCpt.u1VBlkInsideOutsideRatio = 8; // 2 x 0-15       

    rBnrCpt.u1HBlkPeriodSensitivity0 = 12; // 4 x 0-15
    rBnrCpt.u1VBlkPeriodSensitivity0 = 12; // 4 x 0-15

    rBnrCpt.u1HBlkPeriodSensitivity1 = 4; // 4 x 0-15
    rBnrCpt.u1VBlkPeriodSensitivity1 = 4; // 4 x 0-15

    rBnrCpt.u1HBlkIntensitySensitivity0 = 12; // 4 x 0-15
    rBnrCpt.u1VBlkIntensitySensitivity0 = 12; // 4 x 0-15

    rBnrCpt.u1HBlkIntensitySensitivity1 = 4; // 4 x 0-15
    rBnrCpt.u1VBlkIntensitySensitivity1 = 4; // 4 x 0-15

    rBnrCpt.u1HBlkAmountSensitivity0 = 12; // 4 x 0-15
    rBnrCpt.u1VBlkAmountSensitivity0 = 12; // 4 x 0-15

    rBnrCpt.u1HBlkAmountSensitivity1 = 4; // 4 x 0-15
    rBnrCpt.u1VBlkAmountSensitivity1 = 4; // 4 x 0-15

    rBnrCpt.u1HBlkPeriodMatchTh = 8; // 32 x 0-15
    rBnrCpt.u1VBlkPeriodMatchTh = 8; // 32 x 0-15

    rBnrCpt.u1HBlkPredictionProceedTh = 4; // 32 x 0-15
    rBnrCpt.u1VBlkPredictionProceedTh = 4; // 32 x 0-15

    rBnrCpt.u1HBlkPredictionValidTh = 8; // 2 x 0-15
    rBnrCpt.u1VBlkPredictionValidTh = 8; // 2 x 0-15

    rBnrCpt.u1HBlkLevelActiveTh = 12; // 32 x 0-15
    rBnrCpt.u1VBlkLevelActiveTh = 12; // 32 x 0-15          

    rBnrCpt.u1HBlkLevelInc = 4; // 32 x 0-15
    rBnrCpt.u1VBlkLevelInc = 4; // 32 x 0-15         

    rBnrCpt.u1HBlkLevelDec = 2; // 32 x 0-15
    rBnrCpt.u1VBlkLevelDec = 2; // 32 x 0-15

    rBnrCpt.u1HBlkActiveTrialTh = 10; // 2 x 0-15
    rBnrCpt.u1VBlkActiveTrialTh = 10; // 2 x 0-15

    rBnrCpt.u1TxtrPxlCntThSize0 = 9;
    rBnrCpt.u1TxtrPxlCntThSize1 = 11;
    rBnrCpt.u1TxtrPxlCntThSize2 = 13;
    rBnrCpt.u1TxtrPxlCntThSize3 = 15;

    rBnrCpt.u1TxtrPxlCntThValue0 = 64;
    rBnrCpt.u1TxtrPxlCntThValue1 = 128;
    rBnrCpt.u1TxtrPxlCntThValue2 = 128;
    rBnrCpt.u1TxtrPxlCntThValue3 = 192;

    rBnrCpt.u1MnrLowTh = 125; // 2 x 0-255
    rBnrCpt.u1MnrHighTh = 200; // 2 x 0-255
    rBnrCpt.u1MnrLowGain = 4; // 4 x 0-15
    rBnrCpt.u1MnrHighGain = 8; // 4 x 0-15
    rBnrCpt.u1MnrSensitivity = 2; // 0-15

    rBnrCpt.fgEnVDet = TRUE;
    rBnrCpt.fgEnHDet = TRUE;
    rBnrCpt.fgEnBnrAuto = TRUE;
    rBnrCpt.fgEnMnrAuto = TRUE;

    //rBnrCpt.fgPatchCG = FALSE;
    //rBnrCpt.fgUseR = FALSE;
    //rBnrCpt.fgEnLSumRead = FALSE;
    //rBnrCpt.fgEnLocWrite = FALSE;

    //rBnrCpt.u1HBlkLsPadBits = 0;
    //rBnrCpt.u1VBlkLsPadBits = 0;
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
    _SWWM(NR_STA_BNR_06, P_Fld(_iHBlkSavedMinorPeriodCount, NR_STA_BNR_H_MAJOY_PERIOD_CNT)|P_Fld(_iVBlkSavedMinorPeriodCount, NR_STA_BNR_V_MAJOY_PERIOD_CNT));
    _SWWM(NR_STA_BNR_05, P_Fld(_iHBlkSavedMajorPeriod, NR_STA_BNR_H_MAJOY_PERIOD)|P_Fld(_iVBlkSavedMajorPeriod, NR_STA_BNR_V_MAJOY_PERIOD));
    _SWWM(NR_BLK_STA_12, P_Fld(_u1VBlkAvgLS, NR_STA_VBLK_AVGLS)|P_Fld(_u1HBlkAvgLS, NR_STA_HBLK_AVGLS));
    _SWWM(NR_STA_BNR_00, P_Fld(_iHBlkAmount, NR_STA_BNR_H_AMOUNT)|P_Fld(_iHBlkIntensity, NR_STA_BNR_H_INTENSITY));
    _SWWM(NR_STA_BNR_01, P_Fld(_iVBlkAmount, NR_STA_BNR_V_AMOUNT)|P_Fld(_iVBlkIntensity, NR_STA_BNR_V_INTENSITY));
    _SWWM(NR_STA_BNR_02, P_Fld((_u1HBlkFlg|(_u1VBlkFlg<<4)), NR_STA_BNR_GRID_DET_FLG)|P_Fld(_iHBlkConfidence, NR_STA_BNR_HBLK_CONF)|P_Fld(_iVBlkConfidence, NR_STA_BNR_VBLK_CONF));
    _SWWM(NR_STA_BNR_03, P_Fld(_iHBlkLevel, NR_STA_BNR_HBLK_LVL)|P_Fld(_iVBlkLevel, NR_STA_BNR_VBLK_LVL));
    _SWW(NR_STA_MNR_00, _iMnrLevel, NR_STA_MNR_LVL);
        
    if (_rBnrLog.u4LogCnt)
    {
        switch (_rBnrLog.u1LogMode)
        {
        case _NXNR_BNR_FW_LOG_STA:
            LOG(3, "V:BP[%2d]SMP[%2d]SMPC[%4d]BL[%4d]TC[%2d] H:BP[%2d]SMP[%2d]SMPC[%4d]BL[%4d]TC[%2d]\n",
                _iVBlkPeriod,_iVBlkSavedMajorPeriod,_iVBlkSavedMinorPeriodCount, _iVBlkLevel, _iVBlkActiveTrialCount,
                _iHBlkPeriod, _iHBlkSavedMajorPeriod,_iHBlkSavedMinorPeriodCount, _iHBlkLevel, _iHBlkActiveTrialCount);
            break;
        case _NXNR_BNR_FW_LOG_VBLK_PEROID_HIST:
            LOG(3, "VBlkPeriodHist:\n");
            for (i = 0; i < 35; i++)
            {
                LOG(3, "\t[%02d] = %d\n", i, _au1PeriodHistVBLK[i]);
            }
            break;
        case _NXNR_BNR_FW_LOG_HBLK_PEROID_HIST:
            LOG(3, "HBlkPeriodHist:\n");
            for (i = 0; i < 35; i++)
            {
                LOG(3, "\t[%02d] = %d\n", i, _au1PeriodHistHBLK[i]);
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
        //NR_W(NXNR_05, 1, NX_VBLK_DISCONTI_MODE);
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
        //NR_W(NXNR_05, 0, NX_VBLK_DISCONTI_MODE);
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
    strcpy ( szPatternName, _szPatternName );
    _iBnrFwState = _NXNR_BNR_FW_STATE_INIT;
    _u1Init = 1;
    _iLineSumArrayIndex = 0;
}
#else
void BNR_FW_Initialize(UINT32 u4Width, UINT32 u4Height)
{
    if (!_u1Init)
    {
        _u1Init = 1;
    }

    _iHeight = MAX(MIN(1080, u4Height), 64);
    _iWidth  = MAX(MIN(1920, u4Width), 64);
    _iBnrFwStateOnStop = _NXNR_BNR_FW_STATE_INIT;
    _iLineSumArrayIndex = 0;

    if (!_iBnrFwStop)
        _iBnrFwState = _NXNR_BNR_FW_STATE_INIT;    
}
#endif

static void _MNR_FW_ResetAutoGain ( void )
{
    _iMnrLevel = 0;
    _iMnrGain = NR_R(NXNR_04,NX_MNR_GAIN);
}

static void _MNR_FW_AutoGain ( void )
{
    INT32 iRound;
    INT32 iCurrMnrLevel;
    INT32 iSenstivity;
    INT32 iLowTh;
    INT32 iHighTh;
    INT32 iLowGain;
    INT32 iHighGain;

    if (rBnrCpt.fgEnMnrAuto == 0)
    {
       _MNR_FW_ResetAutoGain();
       return;
    }

    iSenstivity = rBnrCpt.u1MnrSensitivity;
    iLowTh = rBnrCpt.u1MnrLowTh * 2;
    iHighTh = rBnrCpt.u1MnrHighTh * 2;
    iLowGain = rBnrCpt.u1MnrLowGain * 4;
    iHighGain = rBnrCpt.u1MnrHighGain * 4;

    iCurrMnrLevel = MIN ( 500, _iMosPixDiffSum * 100 / MAX ( 1, _iMosPixCount ) );

    iRound = ( iCurrMnrLevel > _iMnrLevel ) ? 15 : 0;
    _iMnrLevel = _iMnrLevel + ( ( iCurrMnrLevel - _iMnrLevel ) * iSenstivity + iRound ) / 16;

    if ( _iMnrLevel <= iLowTh )
    {
        _iMnrGain = iLowGain;
    }
    else if ( _iMnrLevel >= iHighTh )
    {
        _iMnrGain = iHighGain;
    }
    else if ( iHighTh != iLowTh )
    {
        _iMnrGain = iLowGain + ( _iMnrLevel - iLowTh ) * ( iHighGain - iLowGain ) / ( iHighTh - iLowTh ) ;
    }
    else
    {
        _iMnrGain = iLowGain;
    }
}

static void _BNR_FW_ResetAutoTh ( void )
{
    _iBnrVblkBsFltStr   =   _SWR(NR_FWBNR_0B, NR_FWBNR_ADAP_VBFS_TH2);
    _iBnrVblkTxBsStrSlp =   _SWR(NR_FWBNR_0B, NR_FWBNR_ADAP_VTBSS_TH2);
    _iBnrVblkTxBsStrTh  =   _SWR(NR_FWBNR_0B, NR_FWBNR_ADAP_VTBSH_TH2);
}

static void _BNR_FW_AutoTh ( void )
{
    INT32 iNMTh1            =   rBnrCpt.u1BnrAdapNMTh1;
    INT32 iNMTh2            =   rBnrCpt.u1BnrAdapNMTh2;
    INT32 iVblkBsFltStrTh1  =   rBnrCpt.u1BnrAdapVBFSTh1;
    INT32 iVblkBsFltStrTh2  =   rBnrCpt.u1BnrAdapVBFSTh2;
    INT32 iVblkTxBsStrSlpTh1=   rBnrCpt.u1BnrAdapVTBSSTh1;
    INT32 iVblkTxBsStrSlpTh2=   rBnrCpt.u1BnrAdapVTBSSTh2;
    INT32 iVblkTxBsStrTh1   =   rBnrCpt.u1BnrAdapVTBSHTh1;
    INT32 iVblkTxBsStrTh2   =   rBnrCpt.u1BnrAdapVTBSHTh2;

    UINT32 u4OriNL= _SWR(NR_NMSTA_0E, NR_NM_STA_NL);
    UINT32 u4NLDbg= _SWR(NR_FWBNR_09, NR_FWBNR_ADAP_THDBG);    
    

    if (rBnrCpt.fgEnBnrThAuto == 0)
    {
       _BNR_FW_ResetAutoTh ();
       return;
    }

    if(u4NLDbg != 0)
    {
        u4OriNL = u4NLDbg;
    }
    
    if ( u4OriNL <= iNMTh1 )
    {
        _iBnrVblkBsFltStr   =   iVblkBsFltStrTh1;
        _iBnrVblkTxBsStrSlp =   iVblkTxBsStrSlpTh1;
        _iBnrVblkTxBsStrTh  =   iVblkTxBsStrTh1;
    }
    else if ( u4OriNL >= iNMTh2 )
    {
        _iBnrVblkBsFltStr   =   iVblkBsFltStrTh2;
        _iBnrVblkTxBsStrSlp =   iVblkTxBsStrSlpTh2;
        _iBnrVblkTxBsStrTh  =   iVblkTxBsStrTh2;
    }
    else if ( iNMTh2 > iNMTh1 )
    {
        _iBnrVblkBsFltStr   =   iVblkBsFltStrTh1 + ( u4OriNL - iNMTh1 ) * ( iVblkBsFltStrTh2 - iVblkBsFltStrTh1 ) / ( iNMTh2 - iNMTh1 ) ;
        _iBnrVblkTxBsStrSlp =   iVblkTxBsStrSlpTh2 + ( iNMTh2 - u4OriNL ) * ( iVblkTxBsStrSlpTh1 - iVblkTxBsStrSlpTh2 ) / ( iNMTh2 - iNMTh1 ) ;
        _iBnrVblkTxBsStrTh  =   iVblkTxBsStrTh1 + ( u4OriNL - iNMTh1 ) * ( iVblkTxBsStrTh2 - iVblkTxBsStrTh1 ) / ( iNMTh2 - iNMTh1 ) ;
    }
    else
    {
        _iBnrVblkBsFltStr   =   iVblkBsFltStrTh2;
        _iBnrVblkTxBsStrSlp =   iVblkTxBsStrSlpTh2;
        _iBnrVblkTxBsStrTh  =   iVblkTxBsStrTh2;
    }
}


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
            _BNR_FW_ResetGridDet( _NXNR_HBLK );
            _BNR_FW_ResetGridDet( _NXNR_VBLK );
            _MNR_FW_ResetAutoGain ();
            _BNR_FW_SetState(_NXNR_BNR_FW_STATE_ACTIVE);
        }
        break;
        
    case _NXNR_BNR_FW_STATE_ACTIVE:
        {
            _BNR_FW_GetControlPoints();            
            _u1MoIdx = MIN((UINT8)4, _BNR_FW_GetMotionLvl());
            _BNR_FW_GetLineSum();
            _BNR_FW_GridDet ( _NXNR_HBLK );
            _BNR_FW_GridDet ( _NXNR_VBLK );
            _MNR_FW_AutoGain();
            _BNR_FW_AutoTh();
            _BNR_FW_CopyLineSumTool();
            _BNR_FW_Log();
            _BNR_FW_SetState(_NXNR_BNR_FW_STATE_VSYNC);
        }
        break;
        
    case _NXNR_BNR_FW_STATE_DBG:
        {
            _BNR_FW_GetControlPoints();
            _BNR_FW_CopyLineSumTool();
            _iBnrFwState = _NXNR_BNR_FW_STATE_VSYNC_DBG;
        }
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

