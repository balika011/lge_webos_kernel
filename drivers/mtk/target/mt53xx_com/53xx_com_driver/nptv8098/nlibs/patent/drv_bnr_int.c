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
#include "drv_bnr_int.h"
#include "hw_nr.h"
#include "nr_debug.h"
#include "hw_sw.h"

#include "x_os.h"
#include "x_assert.h"

#define BNR_MUTEX_LOCK                  (VERIFY(x_sema_lock(_hBNRMutex, X_SEMA_OPTION_WAIT) == OSR_OK))
#define BNR_MUTEX_UNLOCK                (VERIFY(x_sema_unlock(_hBNRMutex) == OSR_OK))
#endif

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

static INT32 _iBnrFwStop;
static INT32 _iBnrFwStateOnStop;
static INT32 _iBnrFwState;
static INT32 _iSelectVBlkWindowState;

#ifdef _C_MODEL_
static INT32 _arVBlkLineSum[_NXNR_BNR_FW_MAX_WIDTH];
static INT32 _arHBlkLineSum[_NXNR_BNR_FW_MAX_HEIGHT];
static INT32 iVBlkPos[_NXNR_BNR_FW_MAX_WIDTH];
static INT32 iHBlkPos[_NXNR_BNR_FW_MAX_HEIGHT];
#else
static UINT16 _arVBlkLineSum[_NXNR_BNR_FW_MAX_WIDTH];
static UINT16 _arHBlkLineSum[_NXNR_BNR_FW_MAX_HEIGHT];
#endif

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
static INT32 _iVBlkRawStr;

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
static INT32 _iVBlkBestPeriodPct;

static INT32 _iVBlkWindowMode;
static INT32 _iVBlkWindowTrialCount;
static INT32 _iVBlkWindowTrialScore[3];
static INT32 _iPrevVBlkWindowMode;

#ifndef _C_MODEL_
static HANDLE_T _hBNRMutex = (HANDLE_T)NULL;
#endif

static void _BNR_FW_FindGroup ( INT32 _iBestPeriod, INT32 * _iGroup, INT32 * _iMember );
static void _BNR_FW_SetControlPoints(void);
static void _BNR_FW_SelectVBlkWindow(void);
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
EXTERN void vDrvNXNRTrigToMoveData(void);
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

static void _BNR_FW_SelectVBlkWindow(void)
{
    switch (_iSelectVBlkWindowState)
    {
    case _NXNR_SELECT_WINDOW_STATE_START:
        {            
            _iPrevVBlkWindowMode = _NXNR_WINDOW_MODE_NONE;
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_NW] = 0;
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N]  = 0;
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W]  = 0;
            _iVBlkWindowTrialCount = 0;
            _iVBlkWindowMode = rBnrCpt.fgIgnoreNW ? _NXNR_WINDOW_MODE_N : _NXNR_WINDOW_MODE_NW;
            _iSelectVBlkWindowState = rBnrCpt.fgIgnoreNW ? 
                _NXNR_SELECT_WINDOW_STATE_WAIT_N : _NXNR_SELECT_WINDOW_STATE_WAIT_NW;
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_WAIT_NW:
        {
            _BNR_FW_ResetVGridDet();
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_NW] = 0;
            _iVBlkWindowTrialCount++;
            _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_TEST_NW;
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_TEST_NW:
        {    
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_NW] += _iVBlkRawStr;

            if ( _iVBlkWindowTrialCount < rBnrCpt.iVBlkWindowTrialCountTh )
            {
                _iVBlkWindowTrialCount++;
                _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_TEST_NW;
            }
            else
            {
                _iVBlkWindowMode = _NXNR_WINDOW_MODE_N;
                _iVBlkWindowTrialCount = 0;
                _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_WAIT_N;
            }
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_WAIT_N:
        {
            _BNR_FW_ResetVGridDet();
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N] = 0;
            _iVBlkWindowTrialCount++;
            _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_TEST_N;
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_TEST_N:
        {
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N] += _iVBlkRawStr;

            if ( _iVBlkWindowTrialCount < rBnrCpt.iVBlkWindowTrialCountTh )
            {
                _iVBlkWindowTrialCount++;
                _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_TEST_N;
            }
            else
            {
                _iVBlkWindowMode = _NXNR_WINDOW_MODE_W;
                _iVBlkWindowTrialCount = 0;
                _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_WAIT_W;
            }
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_WAIT_W:
        {
            _BNR_FW_ResetVGridDet();
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W] = 0;
            _iVBlkWindowTrialCount++;
            _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_TEST_W;
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_TEST_W:
        {
            _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W] += _iVBlkRawStr;

            if ( _iVBlkWindowTrialCount < rBnrCpt.iVBlkWindowTrialCountTh )
            {
                _iVBlkWindowTrialCount++;
                _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_TEST_W;
            }
            else
            {
                if ( _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_NW] < rBnrCpt.iVBlkScoreTh
                    && _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N] < rBnrCpt.iVBlkScoreTh
                    && _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W] < rBnrCpt.iVBlkScoreTh )
                {
                    // scores are not meaningful, restart window selection
                    _iVBlkWindowMode = rBnrCpt.fgIgnoreNW ? _NXNR_WINDOW_MODE_N : _NXNR_WINDOW_MODE_NW;;
                    _iVBlkWindowTrialCount = 0;
                    _iSelectVBlkWindowState = rBnrCpt.fgIgnoreNW ? 
                        _NXNR_SELECT_WINDOW_STATE_WAIT_N : _NXNR_SELECT_WINDOW_STATE_WAIT_NW;
                }
                else
                {
                    // scores are meaningful, select window based on scores
                    if ( _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N] > (rBnrCpt.fgIgnoreNW ? 0 : _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_NW])
                        && _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N] > _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W] )
                    {
                        _iVBlkWindowMode = _NXNR_WINDOW_MODE_N;
                    }
                    else if ( _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W] > (rBnrCpt.fgIgnoreNW ? 0 : _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_NW])
                        && _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_W] > _iVBlkWindowTrialScore[_NXNR_WINDOW_MODE_N] )
                    {
                        _iVBlkWindowMode = _NXNR_WINDOW_MODE_W;
                    }
                    else
                    {
                        _iVBlkWindowMode = rBnrCpt.fgIgnoreNW ? _NXNR_WINDOW_MODE_N : _NXNR_WINDOW_MODE_NW;
                    }

                    // decision must be the same for twice
                    if ( _iPrevVBlkWindowMode == _iVBlkWindowMode )
                    {
                        _iVBlkWindowTrialCount = 0;
                        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_WAIT_READY;
                    }
                    else
                    {
                        _iPrevVBlkWindowMode = _iVBlkWindowMode;
                        _iVBlkWindowMode = rBnrCpt.fgIgnoreNW ? _NXNR_WINDOW_MODE_N : _NXNR_WINDOW_MODE_NW;
                        _iVBlkWindowTrialCount = 0;
                        _iSelectVBlkWindowState = rBnrCpt.fgIgnoreNW ? 
                            _NXNR_SELECT_WINDOW_STATE_WAIT_N : _NXNR_SELECT_WINDOW_STATE_WAIT_NW;
                    }        
                }
            }
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_WAIT_READY:
        {
            _BNR_FW_ResetVGridDet();
            _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_READY;
        }
        break;

    case _NXNR_SELECT_WINDOW_STATE_FORCE_NW:
        _iVBlkWindowMode = _NXNR_WINDOW_MODE_NW;
        break;
    case _NXNR_SELECT_WINDOW_STATE_FORCE_N:
        _iVBlkWindowMode = _NXNR_WINDOW_MODE_N;
        break;
    case _NXNR_SELECT_WINDOW_STATE_FORCE_W:
        _iVBlkWindowMode = _NXNR_WINDOW_MODE_W;
        break;

    case _NXNR_SELECT_WINDOW_STATE_READY:
        // do nothing 
        break;

    default:
        // if somthing goes wrong, re-start window selection
        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_START;
        break;
    }
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
    iArtiRej = ((iBestPeriodPct - iArtiRej) * rBnrCpt.iSizeFactor) / 16;
#if 0
    iLineSumDiff = iLineSumInside * rBnrCpt.iHBlkInsideOutsideRatio * iBestPeriod / 8 - iLineSumOutside;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactor ) >> ( 8 + 4 ) ); // iLineSumDiff could be negative
#else
    iLineSumDiff = (iBestPeriodCnt*rBnrCpt.iHBlkInsideOutsideRatio*2 - (h / MAX(iBestPeriod, 1)))/4;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactor ) /16 ); // iLineSumDiff could be negative
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

static void _BNR_FW_VGridDet (void)
{ 
    // slice and get period histogram
    UINT32 u4VGridMinLineSumTh = rBnrCpt.iVGridMinLineSumTh;
    UINT32 u4SliceSum = 0;
    INT32 iArtiRej = 0;
    INT32 iVBlkRawStr = 0;
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

    x_memset ( _iVBlkPeriodHist, 0, sizeof(INT32) * 35 );
    x_memset ( _au1VBlkPeriodArray, 0, sizeof(UINT8) * _NXNR_BNR_FW_MAX_WIDTH);

    w = _iWidth;

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
        u4SliceLv = ( u4SliceSum >> 6 );

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
    _iVBlkBestPeriodPct = iBestPeriodPct;
    _BNR_FW_FindGroup ( iBestPeriod, & iGroup, & iMember );
    fgMatchPeriod = ( iBestPeriod != 0 ) && ( iGroup == _iVBlkSavedGroup || iBestPeriod == _iVBlkSavedPeriod );

    // update grid position gauge
    if ( fgMatchPeriod )
    {
        for (x = 0; x < w; x++)
        {
            BOOL fgCond0 = ( _au1VBlkPeriodArray[x] == iBestPeriod );
            BOOL fgCond1 = ( _au1VBlkPeriodArray[x] == ( iBestPeriod + 1 ) );
            BOOL fgCond2 = ( ( x+iBestPeriod < w ) && ( _au1VBlkPeriodArray[x+iBestPeriod] == iBestPeriod ) );
            BOOL fgCond3 = ( ( x+iBestPeriod+1 < w ) && ( _au1VBlkPeriodArray[x+iBestPeriod+1] == ( iBestPeriod + 1 ) ) );
            //BOOL fgCond4 = (x-iBestPeriod >= 0) && ((_au1VBlkPeriodArray[x-iBestPeriod] == iBestPeriod) || (_au1VBlkPeriodArray[x-iBestPeriod] == (iBestPeriod+1)));
            //BOOL fgCond5 = (x-iBestPeriod-1 >= 0) && ((_au1VBlkPeriodArray[x-iBestPeriod-1] == iBestPeriod) || (_au1VBlkPeriodArray[x-iBestPeriod-1] == (iBestPeriod+1)));

            if ( fgCond0 || fgCond1 || fgCond2 || fgCond3 )
            {
                _au1VBlkPosGauge[x] = (UINT8) MIN(15, (_au1VBlkPosGauge[x]+rBnrCpt.iVBlkGaugeIncTh));
                iLineSumInside += _arVBlkLineSum[x];
            }
            //else if ((fgCond4 || fgCond5) && ((x-1 >= 0) && _au1VBlkPeriodArray[x-1] != 0))
            //{
            //    _au1VBlkPosGauge[x] = (UINT8) MIN(15, (_au1VBlkPosGauge[x]+rBnrCpt.iVBlkGaugeIncTh));
            //    _au1VBlkPeriodArray[x] = fgCond5 ? iBestPeriod+1 : iBestPeriod;
            //    //iLineSumInside += _arVBlkLineSum[x];
            //}
            else
            {
                if ( _au1VBlkPosGauge[x] > 0 )
                {
                    _au1VBlkPosGauge[x]--;
                }

                iLineSumOutside += MAX ( (UINT32) _arVBlkLineSum[x], u4VGridMinLineSumTh );
            }
        }
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

    iVBlkRawStr = iLineSumInside * rBnrCpt.iVBlkInsideOutsideRatio * iBestPeriod / 8 - iLineSumOutside;
    iVBlkRawStr = clip (-2048, 2047, iVBlkRawStr*rBnrCpt.iSizeFactor / 4096);

#if 0    
    iLineSumDiff = iLineSumInside * rBnrCpt.iVBlkInsideOutsideRatio * iBestPeriod / 8 - iLineSumOutside;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactor ) >> ( 8 + 4 ) ); // iLineSumDiff could be negative
#else
    iLineSumDiff = (iBestPeriodCnt*rBnrCpt.iVBlkInsideOutsideRatio*2 - (w / MAX(iBestPeriod, 1))) / 4;
    iDeltaLevel = clip ( -512, 511, ( iLineSumDiff * rBnrCpt.iSizeFactor ) / 16 ); // iLineSumDiff could be negative
#endif

    _iVBlkRawStr = iVBlkRawStr;

    if ( fgMatchPeriod )
    {
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
        //(((rBnrCpt.iVBlkLevelLeakage*_u1MoIdx) + (MAX(128, rBnrCpt.iVBlkLevelLeakage)*(4-_u1MoIdx))) / 4);
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

    // restart window selection if not confirmed 
    if ( _iVBlkLevel == 0 && _iSelectVBlkWindowState == _NXNR_SELECT_WINDOW_STATE_READY )
    {
        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_START;
    }
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
    // 1. write loc; 2. read line sum; 3. trigger HW to move loc
    if (rBnrCpt.fgEnLocWrite)
        vDrvNXNRTrigToMoveData();
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
    pNXNR->cCsr.nx_vblk_window_mode = _iVBlkWindowMode;
#else
    NR_W(NXNR_0D, _iVBlkConfidence, NX_MANUAL_VBLK_CONFIDENCE);
    NR_W(NXNR_0C, _iHBlkConfidence, NX_MANUAL_HBLK_CONFIDENCE);
    NR_W(NXNR_16, _iVBlkConfidence, NX_MANUAL_VBLK_CONFIDENCE_PR_R);
    NR_W(NXNR_00, _iVBlkWindowMode, NX_VBLK_WINDOW_MODE);
#endif
}

static void _BNR_FW_WriteRegOnce(void)
{
#ifdef _C_MODEL_
    //pNXNR->cCsr.nx_vblk_arti_reject_th = 64;
    //pNXNR->cCsr.nx_vblk_max_range = 0;
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
    //NR_W(NXNR_09, 64, NX_VBLK_ARTI_REJ_TH);
    //NR_W(NXNR_05, 3, NX_VBLK_MAX_RANGE);    // ? why 0
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

    if ( _iWidth * _iHeight >= 1920 * 1080 )
        rBnrCpt.iSizeFactor = 3; // 1080p
    else if ( _iWidth * _iHeight >= 1280 * 720 )
        rBnrCpt.iSizeFactor = 7; // 720p
    else 
        rBnrCpt.iSizeFactor = 16; // SD  
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
    _SWWM(NR_BLK_STA_13, P_Fld(_iVBlkBestPeriodPct, NR_STA_VBLK_BEST_PCT));
    _SWWM(NR_BLK_STA_0E, P_Fld(_iVBlkBestPeriodCnt, NR_BLK_GOOD_BLK_CNT));
        
    if (_rBnrLog.u4LogCnt)
    {
        switch (_rBnrLog.u1LogMode)
        {
        case _NXNR_BNR_FW_LOG_STA:
            LOG(5, "WS[%d]PWM[%2d]WM[%d]SP[%2d]BP[%2d]BPC[%5d]BL[%4d]TC[%2d]TS0[%04d]TS1[%04d]TS2[%04d] HBP[%d]HBPC[%d]\n",
                _iSelectVBlkWindowState, _iPrevVBlkWindowMode, _iVBlkWindowMode,
                _iVBlkSavedPeriod, _iVBlkBestPeriod, _iVBlkBestPeriodCnt, _iVBlkLevel,
                _iVBlkActiveTrialCount, _iVBlkWindowTrialScore[0], _iVBlkWindowTrialScore[1], _iVBlkWindowTrialScore[2],
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
        NR_W(NXNR_00, 1, NX_VBLK_WINDOW_MODE);
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
        NR_W(NXNR_00, 0, NX_VBLK_WINDOW_MODE);
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
    strcpy ( szPatternName, _szPatternName );
    _iBnrFwState = _NXNR_BNR_FW_STATE_INIT;
    _u1Init = 1;
}
#else
void BNR_FW_Initialize(UINT32 u4Width, UINT32 u4Height)
{
    if (!_u1Init)
    {
        VERIFY(x_sema_create(&_hBNRMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        _u1Init = 1;
    }

    _iHeight = MAX(MIN(1080, u4Height), 64);
    _iWidth  = MAX(MIN(1920, u4Width), 64);
    _iBnrFwStateOnStop = _NXNR_BNR_FW_STATE_INIT;

    if (!_iBnrFwStop)
        _iBnrFwState = _NXNR_BNR_FW_STATE_INIT;    
}
#endif

void BNR_FW_SetWindowMode(UINT8 u1Mode)
{
    if (!_u1Init)
        return;

#ifndef _C_MODEL_    
    BNR_MUTEX_LOCK;
#endif

    switch (u1Mode)
    {
    case 0:
        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_START;
        break;
    case 1:
        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_FORCE_NW;
        break;
    case 2:
        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_FORCE_N;
        break;
    case 3:
        _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_FORCE_W;
        break;
    case 4:
        if (_iSelectVBlkWindowState < _NXNR_SELECT_WINDOW_STATE_FORCE_NW)
            _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_START;        
        break;
    }

#ifndef _C_MODEL_
    BNR_MUTEX_UNLOCK;
#endif        
}

void BNR_FW_ActiveProcess(void)
{
    if (!_u1Init)
        return;

#ifndef _C_MODEL_    
    BNR_MUTEX_LOCK;
#endif
    
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
            _iSelectVBlkWindowState = _NXNR_SELECT_WINDOW_STATE_START;
        }
        break;
        
    case _NXNR_BNR_FW_STATE_ACTIVE:
        {
        #ifndef _C_MODEL_
            UINT32 i;
            UINT32 u4VBlkFrmSum = 0;
            UINT32 u4HBlkFrmSum = 0;

            for (i = 0; i < _iWidth; i++)
            {
                UINT32 u4Tmp = u2DrvNXNRGetVBlkLineSum(rBnrCpt.fgUseR, i)&0x3FFC;
                u4Tmp = MIN(u4Tmp, 1020);
                u4VBlkFrmSum += u4Tmp;
                //_arVBlkLineSumRaw[i] = u4Tmp;
                _arVBlkLineSum[i] = u4Tmp;
            }

            for (i = 0; i < _iHeight; i++)
            {
                UINT32 u4Tmp = u2DrvNXNRGetHBlkLineSum(rBnrCpt.fgUseR, i)&0x3FFC;
                u4Tmp = MIN(u4Tmp, 1020);
                u4HBlkFrmSum += u4Tmp;
                _arHBlkLineSum[i] = u4Tmp;
            }

#if 0
            for (i = 1; i < _iWidth-1; i++)
            {
                INT32 i4Tmp = (INT32) _arVBlkLineSumRaw[i]*2 - _arVBlkLineSumRaw[i-1] - _arVBlkLineSumRaw[i+1];
                _arVBlkLineSum[i] = MAX(i4Tmp, 0);
            }
#endif
            _u2VBlkPixCnt = u2DrvNXNRGetVBlkPxCnt(rBnrCpt.fgUseR);
            _u2HBlkPixCnt = u2DrvNXNRGetHBlkPxCnt(rBnrCpt.fgUseR);
            _u1VBlkAvgLS = (_u2VBlkPixCnt != 0) ? (u4VBlkFrmSum / _u2VBlkPixCnt) : 0;
            _u1HBlkAvgLS = (_u2HBlkPixCnt != 0) ? (u4HBlkFrmSum / _u2HBlkPixCnt) : 0;
        #endif
            _u1MoIdx = MIN(4, _BNR_FW_GetMotionLvl());
			_BNR_FW_GetControlPoints();
            _BNR_FW_HGridDet();
            _BNR_FW_VGridDet();
            _BNR_FW_SelectVBlkWindow();
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

#ifndef _C_MODEL_
    BNR_MUTEX_UNLOCK;
#endif
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


