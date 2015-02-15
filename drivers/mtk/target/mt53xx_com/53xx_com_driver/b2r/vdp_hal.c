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
 * $RCSfile: vdp_hal.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdp_hal.c
 *  Brief of file vdp_hal.c.
 *  Details of file vdp_hal.c (optional).
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "vdp_drvif.h"
#include "b2r_drvif.h"
#include "vdp_hal.h"
#include "vdp_vsync.h"
#include "vdp_frc.h"
#include "vdp_hw.h"
#include "vdp_debug.h"
#include "drv_video.h"
#include "drv_di.h"
#include "drv_scaler.h"
#ifdef CC_SECOND_B2R_SUPPORT
#include "vdec_drvif.h"
#endif
#include "tve_if.h"
#include "mute_if.h"
#include "../tve/tve_hal.h"
#ifdef CC_FLIP_MIRROR_SUPPORT
#include "drv_display.h"
#endif
#include "x_lint.h"
//LINT_EXT_HEADER_BEGIN
#include "drv_dbase.h"
#include "x_ckgen.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_assert.h"
//LINT_EXT_HEADER_END

//LINT_SUPPRESS_BRACE(717)        // Info 717: do ... while(0);
//LINT_SUPPRESS_BRACE(572)        // Warning 572: Excessive shift value (precision 1 shifted


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

// By Pass FRC
//#define VDP_BYPASS_FRC

// Log FRC status, Print if FRC happen
//#define VDP_FRC_LOG

// Freeze SCPOS, dump SCPOS frame buffer
//#define VDP_FRC_TRAP

// Log FRC status, Print if invalid FRC happen
//#define VDP_FRC_LOG_CHECK

// Send Zebra Choma Value to Backend
//#define VDP_FRAME_ENCODE_TEST

// Send 1st Field Only
#define VDP_FRC_WA2

// for test only
//#define VDP_STATUS_TEST

#if 1 && !defined(CC_B2R_EMULATION) // defined(CC_MT5360B) || defined (CC_MT5392B) //MT5360B or MT5392B
// Use PSCAN interface to control AUTO STOP
#define VDP_FRC_AUTO_STOP_CTRL
#endif


// Check if SCPOS WPTR running well
//#define VDP_SCPOS_WPTR_COUNT

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define VDP_FRC_LOG_NS          64

#define B2R_CHG_FLD_DELAY       2

#define B2R_SD_TOTAL_WIDTH      864

#define B2R_SD_WIDTH            720

#define B2R_1080I_WIDTH         1920

#define B2R_1080I_OUT_WIDTH     2200

#define B2R_1080P_OUT_HEIGHT    1125

#define B2R_720P_OUT_HEIGHT     750

#define B2R_576P_OUT_HEIGHT     625

#define B2R_480P_OUT_HEIGHT     525

#define B2R_INTERRUPT_NORMAL    (0)

#define B2R_INTERRUPT_DELAY     (40)    // 2.5 MacroBlock

#define B2R_FRC_POLLING_PROTECT     1024

#define B2R_INSIDE_ACTIVE_REGION (10)   // <90% for Active Region

#define STATUS_LOG_STATE_INIT                       0
#define STATUS_LOG_STATE_INIT_TOGGLE                1
#define STATUS_LOG_STATE_NORMAL                     2
#define STATUS_LOG_STATE_TOGGLE                     3

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

/** Brief of VDP_FRC_T
 */
typedef struct
{
    UCHAR ucFrcEn;
    UCHAR ucTargetNs;
    UCHAR ucChangeFieldNs;
    UCHAR ucCurrentNs;
    UCHAR ucMode;
} VDP_FRC_T;

/** Brief of VDP_INFO_T
 */
typedef struct
{
    UINT32 u4En;
    UINT32 u4FrameEncode;
    UINT32 u4Tff;
    UINT32 u4SrcWidth;
    UINT32 u4SrcHeight;
    UINT32 u4SrcPitch;

    UINT32 u4OutWidth;
    UINT32 u4OutHeight;
    UINT32 u4OutInterlace;
    UINT32 u4OutDoubleClk;

    UINT32 u4OutClk;        // 148.5 or 148.5/1.001

    UINT32 u4Mode422;
    UINT32 u4ModeLinear;
} VDP_INFO_T;

typedef struct
{
    UCHAR ucTrickPlay; /* Trick Play at 20081001 */
    UCHAR ucStepForward; /* Step Forward at 20081030 */
    UCHAR ucSlowForward; /* Slow Forward at 20090506 */
} B2R_INFO_T;

typedef struct _VDP_STATUS_LOG_T
{
    UINT32 u4State;
    UINT32 u4CurrentValue;
    UINT32 u4Total;
    UINT32 u4AvgTime;
    UINT32 u4MinId;
    UINT32 u4MinTime;
    UINT32 u4MaxId;
    UINT32 u4MaxTime;
    HAL_TIME_T rLastTime;
} VDP_STATUS_LOG_T;


/** Brief of VDP_STC_T
 */
typedef struct
{
    BOOL fgEnable;
    BOOL fgDiv10;
    UINT32 u4Speed;
    UINT32 u4VSyncPeriod;
    UINT32 u4B2rStcVal;
} B2R_STC_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#ifndef CC_SECOND_B2R_SUPPORT
#define B2R_R_STATUS(vdp)                   (_B2rReadStatus())
#endif

//#define B2R_TO_VDP(b2r)                     (_ucVdpSwitch)

#define VDP_TO_B2R(vdp)                     (_aucB2rSwitch[vdp])

#define VERIFY_VDP_ID(id)                                   \
          do {                                              \
            if ((_aucB2rSwitch[id]) >= B2R_NS) { return; }  \
          } while (0)

#define DIVIDE_WITH_CARRIER(dividend, divisor)      (((dividend) + ((divisor) - 1)) / (divisor))

#define RSHIFT_WITH_CARRIER(dividend, shift)        (((dividend) + ((1 << (shift)) - 1)) >> (shift))

#define B2R_REG_DB_R                        B2R_REG_R
#define B2R_REG_DB_W                        B2R_REG_W
#define B2R_REG_DB_R_M                      B2R_REG_R_M
#define B2R_REG_DB_W_M                      B2R_REG_W_M
#define B2R_REG_DB_R_B                      B2R_REG_R_B
#define B2R_REG_DB_W_B                      B2R_REG_W_B


//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

#ifdef VDP_STATUS_TEST
static void _VdpTestStatusChange(VDP_STATUS_LOG_T* prLog, UINT32 u4Value, CHAR* psStr);
static void _VdpPrintfStatus(VDP_STATUS_LOG_T* prLog, CHAR* psStr);
#endif

#ifdef CC_SECOND_B2R_SUPPORT
static void _B2rSetInterruptDelay(UCHAR ucVdpId);
static void _B2rReadStatus(UCHAR ucVdpId);
static void _VdpHandleFrc(UCHAR ucVdpId);
#else
static void _B2rSetInterruptDelay(void);
static void _B2rReadStatus(void);
static void _VdpHandleFrc(void);
#endif

static void _B2rHdIsr(UINT16 u2Vector);

#if defined(CC_FLIP_MIRROR_SUPPORT)
static void _VdpDoMirrorFlip(BOOL fgDo);
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UCHAR _aucB2rSwitch[VDP_NS] =
{
#if defined(FBM_VIRTUAL_MPEG) || defined(CC_B2R_EMULATION)
    B2R_1, B2R_2
#else
#ifdef CC_SUPPORT_TVE
    B2R_NS, B2R_NS
#else
    B2R_1, B2R_2
#endif
#endif
};

static UCHAR _ucVdpSwitch = VDP_1;

#ifdef CC_SECOND_B2R_SUPPORT
static UINT32 _u4B2rActiveHeight[VDP_NS];
static UINT32 _u4B2rInsideActiveRegion[VDP_NS];
#else
static UINT32 _u4B2rActiveHeight;
static UINT32 _u4B2rInsideActiveRegion;
#endif

static VDP_FRC_T _rB2rFrc[B2R_NS];

static B2R_INFO_T _rB2rData[B2R_NS];

static VDP_INFO_T _rB2rInfo[B2R_NS];

static UCHAR _ucB2rIsrInitiated = 0;

static DRV_DBASE_T* _prDbase = NULL;

static IC_VERSION_T _eIcVersion = IC_VER_UNKNOWN;

#ifdef CC_SECOND_B2R_SUPPORT
static UINT32 _u4CurrentLineCount[VDP_NS];
static UINT32 _u4CurrentIsrStatus[VDP_NS];
static UINT32 _u4CurrentField[VDP_NS];
static UINT32 _u4LastField[VDP_NS];
static UINT32 _u4CurrentLR[VDP_NS];
static UINT32 _u4CurrentOutputLR[VDP_NS];
static UINT32 _u4CurrentOutputField[VDP_NS];
static UINT32 _u4CurrentUnderFlow[VDP_NS];
#else
static UINT32 _u4CurrentLineCount;
static UINT32 _u4CurrentIsrStatus;
static UINT32 _u4CurrentField;
static UINT32 _u4CurrentLR;
static UINT32 _u4CurrentOutputLR;
static UINT32 _u4CurrentOutputField;
static UINT32 _u4CurrentUnderFlow;
#endif


#ifdef VDP_B2R_PSCAN_CRC_FLOW
//UINT32 u4DramIndex;
UINT32 u4DRAMChkSum;
UINT32 u4PixelChkSum;
UINT32 u4PSCANCrc;
UINT32 u4PSCANSrcCrc;
#endif

static VDP_CONF_T* _prVdpConf[VDP_NS];

#ifdef VDP_FRC_LOG
static CHAR _szDebugBufB2r[VDP_FRC_LOG_NS];
static CHAR _szDebugBufTns[VDP_FRC_LOG_NS];
static CHAR _szDebugBufCns[VDP_FRC_LOG_NS];
static CHAR _szDebugBufSm[VDP_FRC_LOG_NS];
static CHAR _szDebugBufInv[VDP_FRC_LOG_NS];
#ifdef CC_SCPOS_EN
static CHAR _szDebugBufWrite[VDP_FRC_LOG_NS];
static CHAR _szDebugBufRead[VDP_FRC_LOG_NS];
static CHAR _szDebugBufCtrl[VDP_FRC_LOG_NS];
#endif
static UINT32 _u4DebugIdx;
static UINT32 _u4DebugPrint;
#endif

#ifdef VDP_STATUS_TEST
static VDP_STATUS_LOG_T rHdIntLog;
#endif

#ifdef VDP_FRC_TRAP
static UINT32 _u4VdpTrap = 0;
static UINT32 _u4VdpTrapCountDown = 0;
static UINT32 _u4VdpCounter = 0;
#endif

#ifdef VDP_FRAME_ENCODE_TEST
static UINT32 _u4FieldEncodeCounter = 0;
static UINT32 _u4FieldEncodeLuma1 = 0;
static UINT32 _u4FieldEncodeLuma2 = 0;
static UINT32 _u4FieldEncodeChroma = 0;
#endif

#ifdef CC_SECOND_B2R_SUPPORT
static UINT32 _u4B2RCrc[VDP_NS];

#else
static UINT32 _u4B2RCrc;
#endif

#ifdef B2R_AUTO_TEST
static UINT32 _u4B2RCrc;
#endif

//for MM video master mode
static B2R_STC_T _rB2rStc[B2R_NS];

static BOOL _fgDumpRegister = FALSE;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

UCHAR _B2R_TO_VDP(UCHAR ucB2RId)
{
#ifdef CC_SUPPORT_TVE
#if 0
    if ((_prVdpConf[VDP_1]->ucB2rId == B2R_1) &&
            (_prVdpConf[VDP_2]->ucB2rId == B2R_1)&&
            (_prVdpConf[VDP_1]->ucEnable == 1) &&
            (_prVdpConf[VDP_2]->ucEnable == 1) )
    {
        return VDP_1;
    }
#endif
    if ((_aucB2rSwitch[VDP_1]==B2R_1) && (_aucB2rSwitch[VDP_1] == _aucB2rSwitch[VDP_2]))
    {
        return VDP_1;
    }
#endif
    return _ucVdpSwitch;
}

#ifdef VDP_STATUS_TEST
//-----------------------------------------------------------------------------
/** Brief of _VdpTestStatusChange.
 */
//-----------------------------------------------------------------------------
static void _VdpTestStatusChange(VDP_STATUS_LOG_T* prLog, UINT32 u4Value, CHAR* psStr)
{
    HAL_TIME_T rTime;
    HAL_TIME_T rDeltaTime;

    switch (prLog->u4State)
    {
    case STATUS_LOG_STATE_NORMAL:
        prLog->u4State = STATUS_LOG_STATE_TOGGLE;
        break;

    case STATUS_LOG_STATE_TOGGLE:
        prLog->u4State = STATUS_LOG_STATE_NORMAL;

        rTime = prLog->rLastTime;
        HAL_GetTime(&(prLog->rLastTime));
        HAL_GetDeltaTime(&rDeltaTime, &(prLog->rLastTime), &rTime);

        if (prLog->u4Total)
        {
            // Overflow Protection
            if ((0xFFFFFFFF - prLog->u4AvgTime) < rDeltaTime.u4Micros)
            {
                Printf("%s: Overflow\n", psStr);
                prLog->u4State = STATUS_LOG_STATE_INIT;
            }

            prLog->u4AvgTime += rDeltaTime.u4Micros;

            if (rDeltaTime.u4Micros > prLog->u4MaxTime)
            {
                if (rDeltaTime.u4Micros > ((prLog->u4MaxTime) << 2))
                {
                    Printf("%s: Max From(%d.%06d) To(%d.%06d)\n", psStr,
                           prLog->rLastTime.u4Seconds, prLog->rLastTime.u4Micros,
                           rTime.u4Seconds, rTime.u4Micros);
                }

                prLog->u4MaxTime = rDeltaTime.u4Micros;
                prLog->u4MaxId = prLog->u4Total;
            }

            if (rDeltaTime.u4Micros < prLog->u4MinTime)
            {
                if (rDeltaTime.u4Micros < ((prLog->u4MinTime) >> 2))
                {
                    Printf("%s: Min From(%d.%06d) To(%d.%06d)\n", psStr,
                           prLog->rLastTime.u4Seconds, prLog->rLastTime.u4Micros,
                           rTime.u4Seconds, rTime.u4Micros);
                }

                prLog->u4MinTime = rDeltaTime.u4Micros;
                prLog->u4MinId = prLog->u4Total;
            }
        }
        else
        {
            prLog->u4AvgTime = rDeltaTime.u4Micros;
            prLog->u4MinTime = rDeltaTime.u4Micros;
            prLog->u4MinId = prLog->u4Total;
            prLog->u4MaxTime = rDeltaTime.u4Micros;
            prLog->u4MaxId = prLog->u4Total;
        }
        (prLog->u4Total)++;

        break;

    case STATUS_LOG_STATE_INIT:
        prLog->u4State = STATUS_LOG_STATE_INIT_TOGGLE;
        prLog->u4Total = 0;
        break;

    case STATUS_LOG_STATE_INIT_TOGGLE:
        prLog->u4State = STATUS_LOG_STATE_NORMAL;
        HAL_GetTime(&(prLog->rLastTime));
        break;
    }

    prLog->u4CurrentValue = u4Value;
}

//-----------------------------------------------------------------------------
/** Brief of _VdpPrintfStatus.
 */
//-----------------------------------------------------------------------------
static void _VdpPrintfStatus(VDP_STATUS_LOG_T* prLog, CHAR* psStr)
{
    if (prLog->u4Total)
    {
        Printf("%s: Avg(%d/%d) Min(%d/%d) Max(%d/%d)\n",
               psStr, prLog->u4Total, (prLog->u4AvgTime / prLog->u4Total),
               prLog->u4MinId, prLog->u4MinTime,
               prLog->u4MaxId, prLog->u4MaxTime);
    }
}
#endif

//-----------------------------------------------------------------------------
/** Brief of _B2rSetInterruptDelay.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
static void _B2rSetInterruptDelay(UCHAR ucVdpId)
#else
static void _B2rSetInterruptDelay(void)
#endif
{
    UINT32 u4BlankLine;
    UINT32 u4IntDelay;
    UINT32 u4SrcHeight;

#ifndef CC_SECOND_B2R_SUPPORT
    UINT8 ucVdpId = 0;
#endif

    if (_rB2rInfo[ucVdpId].u4OutInterlace == 1)
    {
        u4SrcHeight = _rB2rInfo[ucVdpId].u4SrcHeight;
    }
    else
    {
        u4SrcHeight = _rB2rInfo[ucVdpId].u4SrcHeight;
    }

    // Set Interrupt Delay according to Output Format
    if (_rB2rInfo[ucVdpId].u4OutHeight <= B2R_480P_OUT_HEIGHT)
    {
        u4BlankLine = (525 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucVdpId].u4OutHeight <= B2R_576P_OUT_HEIGHT)
    {
        u4BlankLine = (625 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucVdpId].u4OutHeight <= B2R_720P_OUT_HEIGHT)
    {
        u4BlankLine = (750 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucVdpId].u4OutHeight <= B2R_1080P_OUT_HEIGHT)
    {
        u4BlankLine = (1125 - u4SrcHeight);
    }
    else
    {
        u4BlankLine = 0;
    }

    if(_rB2rInfo[ucVdpId].u4OutHeight ==0)
    {
        u4BlankLine = 0;
    }

#if 1
    if(u4BlankLine > 500)  // register only reserve 6 bit to store this information
    {
        u4BlankLine = 500;
    }
#endif

    // Note: Line Counter of 1st Active Line is 4
    u4IntDelay = DIVIDE_WITH_CARRIER((u4BlankLine + 4), 8);

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        u4IntDelay /= 2;
    }

    B2R_REG_DB_W_M(B2R_REG_HDER, u4IntDelay, B2R_REG_HDER_HINTD_M, B2R_REG_HDER_HINTD_S);
}

//-----------------------------------------------------------------------------
/** Brief of _B2rReadStatus.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
static void _B2rReadStatus(UCHAR ucVdpId)
#else
static void _B2rReadStatus(void)
#endif
{
    UINT32 u4Line1;
    UINT32 u4Line2;
#ifdef VDP_B2R_PSCAN_CRC_FLOW
    UINT32 u4CRC;
#endif
    do
    {
        // Line Counter Register does not handle cross-clock domain
        // Read twice to assure stable

        u4Line1 = B2R_REG_R_M(B2R_REG_HSTA, B2R_REG_HSTA_HTGL_M, B2R_REG_HSTA_HTGL_S);
        u4Line2 = B2R_REG_R_M(B2R_REG_HSTA, B2R_REG_HSTA_HTGL_M, B2R_REG_HSTA_HTGL_S);
    } while (u4Line1 != u4Line2);

#ifdef CC_SECOND_B2R_SUPPORT
    _u4B2RCrc[ucVdpId] = B2R_REG_R(B2R_REG_PIXEL_CRC);
    _u4CurrentLineCount[ucVdpId] = u4Line2;
    _u4CurrentIsrStatus[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
    _u4CurrentField[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_ADIF_M);
    _u4CurrentLR[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_LR_STATUS_M);
    _u4CurrentOutputLR[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HD_DISP_R_ADJI_M);
    _u4CurrentOutputField[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_HD_FLD_ADJ_M);
    _u4CurrentUnderFlow[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_RHFUDF_M);
#else
    _u4B2RCrc = B2R_REG_R(B2R_REG_PIXEL_CRC);
    _u4CurrentLineCount = u4Line2;
    _u4CurrentIsrStatus = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
    _u4CurrentField = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_ADIF_M);
    _u4CurrentLR = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_LR_STATUS_M);
    _u4CurrentOutputLR = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HD_DISP_R_ADJI_M);
    _u4CurrentOutputField = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_HD_FLD_ADJ_M);
    _u4CurrentUnderFlow = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_RHFUDF_M);
#endif

#ifdef B2R_AUTO_TEST
    _u4B2RCrc = B2R_REG_R(B2R_REG_PIXEL_CRC);
#endif

#ifdef VDP_B2R_PSCAN_CRC_FLOW
    //u4DramIndex = B2R_REG_R(B2R_REG_RHYS);
    u4DRAMChkSum = B2R_REG_R(B2R_REG_DRAM_CHKSUM);
    u4PixelChkSum = B2R_REG_R(B2R_REG_PIXEL_CRC);

#if defined(CC_MT5365) || defined(CC_MT5395)
    u4PSCANSrcCrc = u4DrvDIGetInputCRC(VDP_1);
    u4CRC = u4DrvDIGetOutputCRC(VDP_1);
#else
    u4PSCANSrcCrc = vDrvDIGetSrcCRC();
    u4CRC = vDrvDIGetCRCResult();
#endif


    if (u4CRC>0)
    {
        u4PSCANCrc = u4CRC;
    }
#endif
}


#if defined(CC_B2R_EMULATION) || defined(B2R_AUTO_TEST)
UINT32 _B2rHdGetCrc(void)
{
    return _u4B2RCrc;
}
#endif

/* VDP Seek mode structure */
typedef struct _VDP_SEEK_PRM_T
{
    UCHAR ucSeekMode;
    UINT32 u4SeekPts;
    UCHAR ucNotifyTrigger;
    UCHAR ucABNotifyTrigger;
    BOOL fgABReach;
} VDP_SEEK_PRM_T;

extern VDP_SEEK_PRM_T _rVdpSeek;
//-----------------------------------------------------------------------------
/** Brief of _B2rHdIsr.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
static void _B2rHdIsr(UINT16 u2Vector)
{
#ifdef ENABLE_MULTIMEDIA
    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ES0);
#endif

    UNUSED(u2Vector);

    UCHAR ucVdpId =0;
    BOOL fgHDISR = FALSE;
    BOOL fgHDISR2 = FALSE;
    BOOL fgSDISR = FALSE;
    BOOL fgSDISR2 = FALSE;

    fgHDISR = B2R_REG_R_B(B2R_REG_PCRT, B2R_REG_HDISR_M);
    fgSDISR = B2R_REG_R_B(B2R_REG_PCRT, B2R_REG_SDISR_M);

    fgHDISR2 = fgHDISR;
    fgSDISR2 = fgSDISR;

    if (fgHDISR == 0 && fgSDISR == 0)
    {
        ASSERT(0);
        return;
    }

    while(fgHDISR || fgSDISR)
    {
        if (fgHDISR)
        {
            ucVdpId = 0;
            _u4CurrentIsrStatus[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
            fgHDISR = FALSE;
        }
        else if (fgSDISR)
        {
            ucVdpId = 1;
            _u4CurrentIsrStatus[ucVdpId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
            fgSDISR = FALSE;
        }

        _B2rReadStatus(ucVdpId);

        //clear underflow bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FMON_M);
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FMON_M, B2R_REG_HMODE_FMON_M);


#ifdef VDP_STATUS_TEST
        _VdpTestStatusChange(&rHdIntLog, 0, "HD-INT");
        _VdpTestStatusChange(&rHdIntLog, 1, "HD-INT");
        return;
#endif

// For PCR error verification
#if 0
        do
        {
            static UINT32 u4Tick = 0;
            if (((++u4Tick) % 1000) == 0)
            {
                static HAL_TIME_T rLastTime;
                HAL_TIME_T rNewTime;
                HAL_TIME_T rResultTime;

                HAL_GetTime(&rNewTime);

                HAL_GetDeltaTime(&rResultTime, &rLastTime, &rNewTime);

                rLastTime = rNewTime;

                LOG(1, "VSYNC(%d) (%d.%6d)\n", u4Tick, rResultTime.u4Seconds, rResultTime.u4Micros);
            }
        } while (0);
#else
        if(_u4CurrentLR[ucVdpId] != 0)
        {
            LOG(10,"R ");
        }
        else
        {
            LOG(10,"L ");
        }

        if(_u4CurrentField[ucVdpId] != 0)
        {
            LOG(10,"B ");
        }
        else
        {
            LOG(10,"T ");
        }

        LOG(10,"CRC = %d ucVdpId = %d\n",_u4B2RCrc[ucVdpId], ucVdpId);
        //LOG(10,"LINE = %d ucVdpId = %d\n", _u4CurrentLineCount[ucVdpId], ucVdpId);

        UNUSED(_u4B2RCrc[ucVdpId]);
        UNUSED(_u4CurrentField[ucVdpId]);
        UNUSED(_u4CurrentOutputField[ucVdpId]);
        UNUSED(_u4CurrentLR[ucVdpId]);
        UNUSED(_u4CurrentOutputLR[ucVdpId]);
#endif

        if (_u4CurrentIsrStatus[ucVdpId] & VDP_ISR_ADI)
        {
#ifdef VDP_FRC_LOG
            _szDebugBufB2r[_u4DebugIdx] = '0' + (_u4CurrentField[ucVdpId] != 0);
            _szDebugBufSm[_u4DebugIdx] = '?';
            _szDebugBufTns[_u4DebugIdx] = '0';
            _szDebugBufCns[_u4DebugIdx] = '0';
            _szDebugBufInv[_u4DebugIdx] = 'X';

#ifdef CC_SCPOS_EN
            _szDebugBufWrite[_u4DebugIdx] = '0' + (((IO_READ32(SCPOS_BASE, 0x07c)) >> 30) & 0x3);
            _szDebugBufRead[_u4DebugIdx] = '0' + (((IO_READ32(SCPOS_BASE, 0x07c)) >> 28) & 0x3);
            _szDebugBufCtrl[_u4DebugIdx] = '0' + (((IO_READ32(PSCAN_BASE, 0x13c)) >> 24) & 0xF);
#endif
#endif

#ifdef VDP_FRC_TRAP
            if (_u4VdpTrapCountDown != 0)
            {
                if (_u4VdpCounter == 0)
                {
                    IO_READ32(SCPOS_BASE, 0x020) = (IO_READ32(SCPOS_BASE, 0x020) & 0xFFFFFFF7);
                    IO_READ32(PSCAN_BASE, 0x100) = 0x2A0;
                    LOG(1, "Trap\n");
                }
                _u4VdpCounter--;
            }
#endif

#ifdef VDP_SCPOS_WPTR_COUNT
            if (_prDbase)
            {
                UINT32 u4LineCount;
                u4LineCount = (((IO_READ32(SCPOS_BASE, 0x07c)) >> 28) & 0x3);

                if ((u4LineCount != (_prDbase->rVdp.au4StopLineCnt[ucVdpId] + 1)) &&
                        (u4LineCount != 0))
                {
                    _prDbase->rVdp.au4Tearing[ucVdpId]++;
                }

                _prDbase->rVdp.au4StopLineCnt[ucVdpId] = u4LineCount;

                _prDbase->rVdp.au4StartLineCnt[ucVdpId]++;
            }
#else
            if (_prDbase)
            {
                _prDbase->rVdp.au4StartLineCnt[ucVdpId] = _u4CurrentLineCount[ucVdpId];

                if (_u4CurrentUnderFlow[ucVdpId] != 0)
                {
                    _prDbase->rVdp.au4UnderFlow[ucVdpId]++;
                }
            }
#endif

            // do FRC here
            _rB2rFrc[ucVdpId].ucCurrentNs++;
            _VdpHandleFrc(ucVdpId);
        }
        else if (_u4CurrentIsrStatus[ucVdpId] & VDP_ISR_VEI)
        {
            // VEI is one-shot interrupt
            B2R_REG_DB_W_B(B2R_REG_RHEN,    0, B2R_REG_RHEN_VEIE_M);
        }

#ifdef CC_SUPPORT_TVE
        UCHAR bEnable;
        // call tve isr, for VBI CC and TTX
        TVE_GetCcEnable(TVE_1, &bEnable);
        if (bEnable)
        {
            TVE_HalSendCcIsr(TVE_1, TVE_GetFieldInfo(TVE_1), 0);
        }
        TVE_GetTTXEnable(TVE_1, &bEnable);
        if (bEnable)
        {
            TVE_HalSendTTXIsr(TVE_1, TVE_GetFieldInfo(TVE_1), 0);
        }

#endif

        //for MM video master mode

#ifdef ENABLE_MULTIMEDIA
        if(!_rVdpSeek.fgABReach && prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_NETWORK_SKYPE)
#else
        if(!_rVdpSeek.fgABReach)
#endif
        {
#if 1

            /* Check whether b2r update stc or not */

            UCHAR ucAvSyncMode = 0xff;
            UCHAR ucStcSrc;
            UCHAR ucFbg;
            //VDP_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucVdpId);

            ucFbg = _VDP_GetFbg(ucVdpId);

            FBM_GetSyncStc(ucFbg, &ucAvSyncMode, &ucStcSrc);

            if ( (VID_SYNC_MODE_NONE == ucAvSyncMode) && VDP_Is4To1024XSpeed(_rB2rStc[B2R_1].u4Speed))
            {
                VDP_SetB2rStc(B2R_1,_VDP_GetDispingPts(ucVdpId));
#ifdef CC_53XX_SWDMX_V2
                STC_SetStcValue(ucStcSrc, VDP_GetB2rStc(B2R_1));
#else
                STC_SetStcValue(VDP_GetB2rStc(B2R_1));
#endif

                LOG(5," Pts update STC(0x%x)\n",VDP_GetB2rStc(B2R_1));
            }
            else if ((_rB2rStc[B2R_1].fgEnable) && (ucStcSrc < STC_SRC_V2))
            {
                UINT32 u4Increment;
                UINT32 u4Diff;

#ifdef CC_53XX_SWDMX_V2
                _rB2rStc[B2R_1].u4B2rStcVal = STC_GetStcValue(ucStcSrc);
#else
                _rB2rStc[B2R_1].u4B2rStcVal = STC_GetStcValue();
#endif

                if (_rB2rStc[B2R_1].fgDiv10)
                {
                    u4Increment = ((_rB2rStc[B2R_1].u4VSyncPeriod * _rB2rStc[B2R_1].u4Speed) >> 10) / 10;
                }
                else
                {
                    u4Increment = (_rB2rStc[B2R_1].u4VSyncPeriod * _rB2rStc[B2R_1].u4Speed) >> 10;
                }

                u4Diff = (0xffffffff - _rB2rStc[B2R_1].u4B2rStcVal);

                if (u4Diff > u4Increment)
                {
                    _rB2rStc[B2R_1].u4B2rStcVal += u4Increment;
                }
                else
                {
                    _rB2rStc[B2R_1].u4B2rStcVal = (u4Increment - u4Diff);
                }
                LOG(5, "Speed update STC (0x%x)\n", _rB2rStc[B2R_1].u4B2rStcVal);
#ifdef CC_53XX_SWDMX_V2
                STC_SetStcValue(ucStcSrc, _rB2rStc[B2R_1].u4B2rStcVal);
#else
                STC_SetStcValue(_rB2rStc[B2R_1].u4B2rStcVal);
#endif
            }
            else
            {
                _rB2rStc[B2R_1].u4B2rStcVal = 0;
            }
#endif
        }

        //    LOG(4, "ISR %d 0x%x 0x%x\n", _u4CurrentLineCount, _u4CurrentField, _u4CurrentIsrStatus);

        // Note: Hardware latch status, Top Field = 0, Bottom Field = 1,
        // Note: Current Field = Top, Next Field = Bottom
        // Note: Current Field = Bottom, Next Field = Top
        // Note: Since we are using double buffer, _VDP_VsyncTick need to prepare frame buffer for next field

        if (_u4LastField[ucVdpId] == _u4CurrentField[ucVdpId])
        {
            LOG(10, "VDP_HAL.c: Wrong field. ucVdpId = %d\n", ucVdpId);
        }
        _u4LastField[ucVdpId] = _u4CurrentField[ucVdpId];
        _VDP_VsyncTick(ucVdpId, (_u4CurrentField[ucVdpId] == 0),(_u4CurrentLR[ucVdpId] == 0), _u4CurrentIsrStatus[ucVdpId]);
    }
    // clear global isr
    /*if (!BIM_ClearIrq(VECTOR_DISPLAY))
    {
    }
    */

#if defined(CC_MT5395) && !defined(CC_DISABLE_TWO_B2R)
    if (fgHDISR2)
    {
        //B2R_REG_W_B(B2R_REG_PCRT, B2R_REG_HDISR_M, B2R_REG_HDISR_M);
        B2R_REG_W(B2R_REG_PCRT,0x10);
    }
    if (fgSDISR2)
    {
        //B2R_REG_W_B(B2R_REG_PCRT, B2R_REG_SDISR_M, B2R_REG_SDISR_M);
        B2R_REG_W(B2R_REG_PCRT,0x20);
    }
#endif

}
#else
static void _B2rHdIsr(UINT16 u2Vector)
{
    UCHAR ucVdpId;

    UNUSED(u2Vector);

    // status will clean after global isr clean
    B2R_R_STATUS(B2R_1);

    //clear underflow bit
    B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FMON_M);
    B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FMON_M, B2R_REG_HMODE_FMON_M);

    // clear local isr (5391 new)
#ifndef B2R_SD_TEST
    B2R_REG_W_B(B2R_REG_PCRT, B2R_REG_HDISR_M, B2R_REG_HDISR_M);
#else
B2R_REG_W_B(B2R_REG_PCRT, B2R_REG_SDISR_M, B2R_REG_SDISR_M);
#endif

    // clear global isr
    if (!BIM_ClearIrq(VECTOR_DISPLAY))
    {
    }

#ifdef VDP_STATUS_TEST
    _VdpTestStatusChange(&rHdIntLog, 0, "HD-INT");
    _VdpTestStatusChange(&rHdIntLog, 1, "HD-INT");
    return;
#endif

    ucVdpId = _B2R_TO_VDP(B2R_1);
    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return;
    }

#if 0
    do
    {
        static UINT32 u4Tick = 0;
        if (((++u4Tick) % 1000) == 0)
        {
            static HAL_TIME_T rLastTime;
            HAL_TIME_T rNewTime;
            HAL_TIME_T rResultTime;

            HAL_GetTime(&rNewTime);

            HAL_GetDeltaTime(&rResultTime, &rLastTime, &rNewTime);

            rLastTime = rNewTime;

            LOG(1, "VSYNC(%d) (%d.%6d)\n", u4Tick, rResultTime.u4Seconds, rResultTime.u4Micros);
        }
    } while (0);
#else
{
    if(_u4CurrentOutputLR != 0)
    {
        LOG(10,"R ");
    }
    else
    {
        LOG(10,"L ");
    }
    if(_u4CurrentOutputField != 0)
    {
        LOG(10,"B1 ");
    }
    else
    {
        LOG(10,"T1 ");
    }

    if(_u4CurrentField != 0)
    {
        LOG(10,"B2 ");
    }
    else
    {
        LOG(10,"T2 ");
    }

    LOG(10,"CRC = %d \n",_u4B2RCrc);
    UNUSED(_u4B2RCrc);
    UNUSED(_u4CurrentField);
    UNUSED(_u4CurrentOutputField);
    UNUSED(_u4CurrentLR);
    UNUSED(_u4CurrentOutputLR);
    //UNUSED(_u4CurrentField);

    //static UINT32 u4counter = 0;
    //UNUSED(u4counter);
    //LOG(3,"vsync interrupt %d\n",u4counter++);
}
#endif

    if (_u4CurrentIsrStatus & VDP_ISR_ADI)
    {
#ifdef VDP_FRC_LOG
        _szDebugBufB2r[_u4DebugIdx] = '0' + (_u4CurrentField != 0);
        _szDebugBufSm[_u4DebugIdx] = '?';
        _szDebugBufTns[_u4DebugIdx] = '0';
        _szDebugBufCns[_u4DebugIdx] = '0';
        _szDebugBufInv[_u4DebugIdx] = 'X';

#ifdef CC_SCPOS_EN
        _szDebugBufWrite[_u4DebugIdx] = '0' + (((IO_READ32(SCPOS_BASE, 0x07c)) >> 30) & 0x3);
        _szDebugBufRead[_u4DebugIdx] = '0' + (((IO_READ32(SCPOS_BASE, 0x07c)) >> 28) & 0x3);
        _szDebugBufCtrl[_u4DebugIdx] = '0' + (((IO_READ32(PSCAN_BASE, 0x13c)) >> 24) & 0xF);
#endif
#endif

#ifdef VDP_FRC_TRAP
        if (_u4VdpTrapCountDown != 0)
        {
            if (_u4VdpCounter == 0)
            {
                IO_READ32(SCPOS_BASE, 0x020) = (IO_READ32(SCPOS_BASE, 0x020) & 0xFFFFFFF7);
                IO_READ32(PSCAN_BASE, 0x100) = 0x2A0;
                LOG(1, "Trap\n");
            }
            _u4VdpCounter--;
        }
#endif

#ifdef VDP_SCPOS_WPTR_COUNT
        if (_prDbase)
        {
            UINT32 u4LineCount;
//            u4LineCount = (((IO_READ32(SCPOS_BASE, 0x07c)) >> 30) & 0x3);
            u4LineCount = (((IO_READ32(SCPOS_BASE, 0x07c)) >> 28) & 0x3);

            if ((u4LineCount != (_prDbase->rVdp.au4StopLineCnt[ucVdpId] + 1)) &&
                    (u4LineCount != 0))
            {
                _prDbase->rVdp.au4Tearing[ucVdpId]++;
            }

            _prDbase->rVdp.au4StopLineCnt[ucVdpId] = u4LineCount;

            _prDbase->rVdp.au4StartLineCnt[ucVdpId]++;
        }
#else
if (_prDbase)
{
    _prDbase->rVdp.au4StartLineCnt[ucVdpId] = _u4CurrentLineCount;

    if (_u4CurrentUnderFlow != 0)
    {
        _prDbase->rVdp.au4UnderFlow[ucVdpId]++;
    }
}
#endif

        // do FRC here
        _rB2rFrc[ucVdpId].ucCurrentNs++;
        _VdpHandleFrc();
    }
    else if (_u4CurrentIsrStatus & VDP_ISR_VEI)
    {
        // VEI is one-shot interrupt
        B2R_REG_DB_W_B(B2R_REG_RHEN,    0, B2R_REG_RHEN_VEIE_M);
    }

#ifdef CC_SUPPORT_TVE
    UCHAR bEnable;
    // call tve isr, for VBI CC and TTX
    TVE_GetCcEnable(TVE_1, &bEnable);
    if (bEnable)
    {
        TVE_HalSendCcIsr(TVE_1, TVE_GetFieldInfo(TVE_1), 0);
    }
    TVE_GetTTXEnable(TVE_1, &bEnable);
    if (bEnable)
    {
        TVE_HalSendTTXIsr(TVE_1, TVE_GetFieldInfo(TVE_1), 0);
    }

#endif

    //for MM video master mode


    if(!_rVdpSeek.fgABReach)
    {
#if 1

        /* Check whether b2r update stc or not */

        UCHAR ucAvSyncMode = 0xff;
        UCHAR ucStcSrc;
        UCHAR ucFbg;
        //VDP_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucVdpId);

        ucFbg = _VDP_GetFbg(ucVdpId);

        FBM_GetSyncStc(ucFbg, &ucAvSyncMode, &ucStcSrc);

        if ( (VID_SYNC_MODE_NONE == ucAvSyncMode) && VDP_Is4To1024XSpeed(_rB2rStc[B2R_1].u4Speed))
        {
            VDP_SetB2rStc(B2R_1,_VDP_GetDispingPts(ucVdpId));
#ifdef CC_53XX_SWDMX_V2
            STC_SetStcValue(ucStcSrc, VDP_GetB2rStc(B2R_1));
#else
STC_SetStcValue(VDP_GetB2rStc(B2R_1));
#endif

            LOG(5," Pts update STC(0x%x)\n",VDP_GetB2rStc(B2R_1));
        }
        else if ((_rB2rStc[B2R_1].fgEnable) && (ucStcSrc < STC_SRC_V2))
        {
            UINT32 u4Increment;
            UINT32 u4Diff;

#ifdef CC_53XX_SWDMX_V2
            _rB2rStc[B2R_1].u4B2rStcVal = STC_GetStcValue(ucStcSrc);
#else
_rB2rStc[B2R_1].u4B2rStcVal = STC_GetStcValue();
#endif

            if (_rB2rStc[B2R_1].fgDiv10)
            {
                u4Increment = ((_rB2rStc[B2R_1].u4VSyncPeriod * _rB2rStc[B2R_1].u4Speed) >> 10) / 10;
            }
            else
            {
                u4Increment = (_rB2rStc[B2R_1].u4VSyncPeriod * _rB2rStc[B2R_1].u4Speed) >> 10;
            }

            u4Diff = (0xffffffff - _rB2rStc[B2R_1].u4B2rStcVal);

            if (u4Diff > u4Increment)
            {
                _rB2rStc[B2R_1].u4B2rStcVal += u4Increment;
            }
            else
            {
                _rB2rStc[B2R_1].u4B2rStcVal = (u4Increment - u4Diff);
            }
            LOG(5, "Speed update STC (0x%x)\n", _rB2rStc[B2R_1].u4B2rStcVal);
#ifdef CC_53XX_SWDMX_V2
            STC_SetStcValue(ucStcSrc, _rB2rStc[B2R_1].u4B2rStcVal);
#else
STC_SetStcValue(_rB2rStc[B2R_1].u4B2rStcVal);
#endif
        }
        else
        {
            _rB2rStc[B2R_1].u4B2rStcVal = 0;
        }
#endif
    }

    //    LOG(4, "ISR %d 0x%x 0x%x\n", _u4CurrentLineCount, _u4CurrentField, _u4CurrentIsrStatus);

    // Note: Hardware latch status, Top Field = 0, Bottom Field = 1,
    // Note: Current Field = Top, Next Field = Bottom
    // Note: Current Field = Bottom, Next Field = Top
    // Note: Since we are using double buffer, _VDP_VsyncTick need to prepare frame buffer for next field

    _VDP_VsyncTick(ucVdpId, (_u4CurrentField == 0),(_u4CurrentLR == 0), _u4CurrentIsrStatus);
}
#endif

//-----------------------------------------------------------------------------
/** Brief of _VdpHandleFrc.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
static void _VdpHandleFrc(UCHAR ucVdpId)
{
#else
static void _VdpHandleFrc(void)
{
    UCHAR ucVdpId;

    ucVdpId = _B2R_TO_VDP(B2R_1);
#endif

    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return;
    }

    VDP_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucVdpId);

    //DTV0032281  for step set auto- step to false:
    UINT8 u1DecoderSrcId;

    if(prB2rPrm != NULL)
    {
        u1DecoderSrcId = FBM_GetDecoderSrcId(prB2rPrm->rPortPrm.ucFbgId);
    }
    else
    {
        u1DecoderSrcId = B2R_1;
    }

    if(prB2rPrm != NULL)
    {
        if(prB2rPrm->rPortPrm.fgB2R3DEnable) // 3d timing don't set FRC mode.
        {
            return;
        }
    }

    if (_rB2rFrc[ucVdpId].ucFrcEn == 0)
    {
        return;
    }

    //if (_rB2rFrc[ucVdpId].ucCurrentNs == 0)
    // DTV00093447 for di has do some change, b2r should send T/P by order, or will freeze
    if ( ( _rB2rFrc[ucVdpId].ucCurrentNs == 0 ) /* && ( VDP_FRC_TRICK_PLAY != _rB2rFrc[ucVdpId].ucMode  ) */)
    {
        B2R_REG_DB_W_M(B2R_REG_RMAP, _rB2rFrc[ucVdpId].ucTargetNs, B2R_REG_RMAP_REP_CNT_M, B2R_REG_RMAP_REP_CNT_S);
        /* H-sync shift half line must be set OFF or DDDS lock will make frame shake up and down */
        B2R_REG_DB_W_B(B2R_REG_RMAP, B2R_REG_RMAP_REP_NEW_MODE_M, B2R_REG_RMAP_REP_NEW_MODE_M);
        B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_SHIFT_M);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);

#ifdef VDP_FRC_AUTO_STOP_CTRL
        if(u1DecoderSrcId < B2R_NS)
        {
            if(( VDP_FRC_TRICK_PLAY == _rB2rFrc[ucVdpId].ucMode )||
                    (VDP_HalGetStepForward(u1DecoderSrcId) == VDP_PLAY_STEP_FORWARD))
            {
                vDrvDIFrameRepeat(ucVdpId, 0);
            }
            else
            {
                vDrvDIFrameRepeat(ucVdpId, 1);
            }
        }
#endif
        LOG(8,"B2R FRC S(%d) \n", _rB2rFrc[ucVdpId].ucTargetNs);
    }

    if (_rB2rFrc[ucVdpId].ucCurrentNs == _rB2rFrc[ucVdpId].ucTargetNs)
    {
        B2R_REG_DB_W_M(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_CNT_M, B2R_REG_RMAP_REP_CNT_S);
        B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_NEW_MODE_M);
        B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_SHIFT_M);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);
    }

    return;
}

#if defined(CC_FLIP_MIRROR_SUPPORT)
static void _VdpDoMirrorFlip(BOOL fgDo)
{
    UINT8 u1FlipMirrorEnable = u1GetFlipMirrorConfig();

    if (fgDo != 0)
    {
        if (u1FlipMirrorEnable & SYS_FLIP_CONFIG_ON)
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FLIP_M, B2R_REG_HMODE_FLIP_M);  //enable b2r flip
        }
        else
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FLIP_M);  //disnble b2r flip
        }

        if (u1FlipMirrorEnable & SYS_MIRROR_CONFIG_ON)
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_MIRROR_M, B2R_REG_HMODE_MIRROR_M);  //enable b2r mirror
        }
        else
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_MIRROR_M);  //disable b2r mirror
        }
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FLIP_M);  //disnble b2r flip
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_MIRROR_M);  //disable b2r mirror
    }
}
#endif

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalIsrFin.
 */
//-----------------------------------------------------------------------------
void _VDP_HalIsrFin(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId);

    // check tearing
#ifndef CC_SECOND_B2R_SUPPORT
    B2R_R_STATUS(B2R_1);
#endif

#ifndef VDP_SCPOS_WPTR_COUNT
    if (_prDbase)
    {
        _prDbase->rVdp.au4StopLineCnt[ucVdpId] = _u4CurrentLineCount;

        if (_u4CurrentLineCount > _u4B2rActiveHeight)
        {
            _prDbase->rVdp.au4Tearing[ucVdpId]++;
        }
    }
#endif

#ifdef VDP_FRC_LOG
    if (++_u4DebugIdx >= (VDP_FRC_LOG_NS - 1))
    {
        if (_u4DebugPrint)
        {
            LOG(1, "B2R %s\n", _szDebugBufB2r);
            LOG(1, "STA %s\n", _szDebugBufSm);
            LOG(1, "TNS %s\n", _szDebugBufTns);
            LOG(1, "CNS %s\n", _szDebugBufCns);
            LOG(1, "INV %s\n", _szDebugBufInv);

#ifdef CC_SCPOS_EN
            LOG(1, "SCW %s\n", _szDebugBufWrite);
            LOG(1, "SCR %s\n", _szDebugBufRead);
            LOG(1, "CTL %s\n", _szDebugBufCtrl);
#endif

            LOG(1, "\n");
        }

        _u4DebugIdx = 0;
        _u4DebugPrint = 0;
    }
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalInit.
 */
//-----------------------------------------------------------------------------
void _VDP_HalInit(void)
{
    if (_ucB2rIsrInitiated == 0)
    {
        x_os_isr_fct pfnOldIsr;
        CRIT_STATE_T _rCritState;
        UINT32 u4VdpIdx;
        UINT32 u4B2rIdx;
#ifdef CC_SECOND_B2R_SUPPORT
        UCHAR ucVdpId;
#endif

        _ucB2rIsrInitiated = 1;

        _eIcVersion = BSP_GetIcVersion();

        for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
        {
            _prVdpConf[u4VdpIdx] = VDP_GetConf((UCHAR)u4VdpIdx);
        }
        UNUSED(_prVdpConf); /* for warning free */

        _prDbase = DBS_Lock(&_rCritState);
        ASSERT(_prDbase);
        VERIFY(DBS_Unlock(_prDbase, _rCritState));

        _VDP_HalReset();

        VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2rHdIsr, &pfnOldIsr) == OSR_OK);
        // disable double register buffer
#ifdef CC_SECOND_B2R_SUPPORT
        for(ucVdpId =0 ; ucVdpId< VDP_NS ; ucVdpId++)
#endif
        {

            B2R_REG_W_B(B2R_REG_RHEN, 0, B2R_REG_RHEN_VPTE_M);

            // disable 1st MB interrupt
            B2R_REG_W_B(B2R_REG_ROWC, 0, B2R_REG_ROWC_FIE_M);

            B2R_REG_DB_W_B(B2R_REG_BRHEN, 0, (B2R_REG_BRHEN_BRPE_M | B2R_REG_BRHEN_BRWP_M));

            // reset Pattern
            B2R_REG_W_B(B2R_REG_HFC, 0, B2R_REG_HFC_GXYM_M);
        }

#ifdef CC_SECOND_B2R_SUPPORT
        ucVdpId = 0;
#endif
        // Bypass RR mode
        B2R_REG_DB_W_B(B2R_REG_SRST, B2R_REG_RR_BYPASS_M, B2R_REG_RR_BYPASS_M);

        for (u4B2rIdx = 0; u4B2rIdx < B2R_NS; u4B2rIdx++)
        {
            /* Trick Play at 20081001 */
            _rB2rData[u4B2rIdx].ucTrickPlay = 0;

            /* Step Forward at 20081030 */
            _rB2rData[u4B2rIdx].ucStepForward = 0;

            /* Slow Forward at 20090506 */
            _rB2rData[u4B2rIdx].ucSlowForward = 0;

            _rB2rFrc[u4B2rIdx].ucFrcEn = 0;
            _rB2rFrc[u4B2rIdx].ucTargetNs = 0;
            _rB2rFrc[u4B2rIdx].ucChangeFieldNs = 0;

            _rB2rFrc[u4B2rIdx].ucCurrentNs = 0;
        }
        // Init Format
        _VDP_HalSetFormat(VDP_1, 0, 0);

    }

#ifdef VDP_FRAME_ENCODE_TEST
    do
    {
        void* pMallocAddr;
        UINT32 u4Size;
        UINT32 u4Idx;
        UINT32* pu4Addr;
        UINT8* pu1Addr;

        u4Size = (1920*270) + 2048;
        pMallocAddr = x_mem_alloc(u4Size);    // 420

        _u4FieldEncodeChroma = (UINT32) pMallocAddr;
        _u4FieldEncodeChroma = (UINT32) ((_u4FieldEncodeChroma + 2047) & (~(2047)));
        pu4Addr = (UINT32*) _u4FieldEncodeChroma;

        u4Size -= 2048;
        u4Size /= 4;    // 8 Bit >> 32 Bit

        for (u4Idx = 0; u4Idx < u4Size; u4Idx+=16)
        {
            if ((u4Idx & 0x10) == 0)
            {
                pu4Addr[0] = 0xFF00FF00;
                pu4Addr[1] = 0xFF00FF00;
                pu4Addr[2] = 0xFF00FF00;
                pu4Addr[3] = 0xFF00FF00;
                pu4Addr[4] = 0xFF00FF00;
                pu4Addr[5] = 0xFF00FF00;
                pu4Addr[6] = 0xFF00FF00;
                pu4Addr[7] = 0xFF00FF00;
                pu4Addr[8] = 0xFF00FF00;
                pu4Addr[9] = 0xFF00FF00;
                pu4Addr[10] = 0xFF00FF00;
                pu4Addr[11] = 0xFF00FF00;
                pu4Addr[12] = 0xFF00FF00;
                pu4Addr[13] = 0xFF00FF00;
                pu4Addr[14] = 0xFF00FF00;
                pu4Addr[15] = 0xFF00FF00;
            }
            else
            {
                pu4Addr[0] = 0x00FF00FF;
                pu4Addr[1] = 0x00FF00FF;
                pu4Addr[2] = 0x00FF00FF;
                pu4Addr[3] = 0x00FF00FF;
                pu4Addr[4] = 0x00FF00FF;
                pu4Addr[5] = 0x00FF00FF;
                pu4Addr[6] = 0x00FF00FF;
                pu4Addr[7] = 0x00FF00FF;
                pu4Addr[8] = 0x00FF00FF;
                pu4Addr[9] = 0x00FF00FF;
                pu4Addr[10] = 0x00FF00FF;
                pu4Addr[11] = 0x00FF00FF;
                pu4Addr[12] = 0x00FF00FF;
                pu4Addr[13] = 0x00FF00FF;
                pu4Addr[14] = 0x00FF00FF;
                pu4Addr[15] = 0x00FF00FF;
            }

            pu4Addr += 16;
        }

        u4Size = (1920*540) + 2048;
        pMallocAddr = x_mem_alloc(u4Size);

        _u4FieldEncodeLuma1 = (UINT32) pMallocAddr;
        _u4FieldEncodeLuma1 = (UINT32) ((_u4FieldEncodeLuma1 + 2047) & (~(2047)));
        pu1Addr = (UINT8*) _u4FieldEncodeLuma1;

        for (u4Idx = 0; u4Idx < (u4Size - 4096); u4Idx++)
        {
            if ((u4Idx & 1) == 0)
            {
                pu1Addr[u4Idx] = 0xFF;
            }
            else
            {
                pu1Addr[u4Idx] = 0x00;
            }
        }

        pMallocAddr = x_mem_alloc(u4Size);
        _u4FieldEncodeLuma2 = (UINT32) pMallocAddr;
        _u4FieldEncodeLuma2 = (UINT32) ((_u4FieldEncodeLuma2 + 2047) & (~(2047)));
        pu1Addr = (UINT8*) _u4FieldEncodeLuma2;

        for (u4Idx = 0; u4Idx < (u4Size - 4096); u4Idx++)
        {
            if ((u4Idx & 1) == 0)
            {
                pu1Addr[u4Idx] = 0x00;
            }
            else
            {
                pu1Addr[u4Idx] = 0xFF;
            }
        }
    } while (0);
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalReset.
 */
//-----------------------------------------------------------------------------
void _VDP_HalReset(void)
{

#ifndef CC_SECOND_B2R_SUPPORT
    // Reset: Write 1 than Write 0
    B2R_REG_W_B(B2R_REG_SRST,
                (B2R_REG_SRST_HDRST_M | B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M),
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M));

    B2R_REG_W_B(B2R_REG_SRST,
                0,
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M));
#endif

#ifdef CC_SECOND_B2R_SUPPORT
    UCHAR ucVdpId ;
    for(ucVdpId =0 ; ucVdpId < VDP_NS ; ucVdpId++)
#endif
    {
        // DRAM Req = 32
        B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_RHRQTH_M, B2R_REG_HMODE_RHRQTH_S);

        //DRAM Burst Read
        B2R_REG_DB_W_M(B2R_REG_HMODE, 1, B2R_REG_HMODE_BURST_M, B2R_REG_HMODE_BURST_S);

        // 64 FIFO
        // 32 x 16 bytes FIFO for each Y/C
        B2R_REG_DB_W_M(B2R_REG_HPH, 0, B2R_REG_HPH_RHFS_M, B2R_REG_HPH_RHFS_S);

        // HBTD = HTBD = 0
        B2R_REG_DB_W_M(B2R_REG_HHVT, 0, B2R_REG_HHVT_HTBD_M, B2R_REG_HHVT_HTBD_S);
        B2R_REG_DB_W_M(B2R_REG_HHVT, 0, B2R_REG_HHVT_HBTD_M, B2R_REG_HHVT_HBTD_S);

        // FMON (Underflow Check)
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FMON_M, B2R_REG_HMODE_FMON_M);
    }
#ifdef CC_SECOND_B2R_SUPPORT
    ucVdpId = 0;

    // Reset: Write 1 than Write 0
    B2R_REG_W_B(B2R_REG_SRST,
                (B2R_REG_SRST_HDRST_M | B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M),
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M));

    B2R_REG_W_B(B2R_REG_SRST,
                0,
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M));
#endif
#if 0 // pibben 5387
    // 5382p, Bottom Field First Always ON
    if (_eIcVersion >= IC_VER_5382P_AA)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_HBFST_M, B2R_REG_HMODE_HBFST_M);
    }
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetEnable.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetEnable(UCHAR ucVdpId, UCHAR ucEnable)
{
    UINT32 u4Reset;

    VERIFY_VDP_ID(ucVdpId);

#ifdef VDP_FRC_LOG
    _u4DebugIdx = 0;
#endif

    if (_rB2rInfo[ucVdpId].u4En != ucEnable)
    {
        u4Reset = 1;
    }
    else
    {
        u4Reset = 0;
    }

    if (u4Reset)
    {
#ifndef CC_SECOND_B2R_SUPPORT
        B2R_REG_W_B(B2R_REG_SRST, B2R_REG_SRST_DMRST_M, B2R_REG_SRST_DMRST_M);
#endif
    }

    _rB2rInfo[ucVdpId].u4En = ucEnable;

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        // interlace mode, enable auto field trigger

        //B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_RHFAUT_M, B2R_REG_HMODE_RHFAUT_M);
    }
    else
    {
        // progressive mode, disable auto field trigger

        //B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_RHFAUT_M);
    }

    if (ucEnable)
    {

#ifndef CC_SECOND_B2R_SUPPORT
        VDP_HalSetB2RPower(TRUE);
#endif

        //Enable adjustable interrupt
        B2R_REG_DB_W_B(B2R_REG_RHEN,
                       (B2R_REG_RHEN_M|B2R_REG_RHEN_ADIE_M),
                       (B2R_REG_RHEN_M|B2R_REG_RHEN_ADIE_M|B2R_REG_RHEN_VEIE_M));

        // enable double register buffer
        B2R_REG_DB_W_B(B2R_REG_RHEN, B2R_REG_RHEN_VPTE_M, B2R_REG_RHEN_VPTE_M);

#ifdef CC_DYNAMIC_POWER_ONOFF
        B2R_REG_W_B(B2R_REG_SRST, B2R_REG_SRST_DMEN_M,
                    (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M|B2R_REG_SRST_DMEN_M));
#endif

        // Set Interrupt Delay according to Output Format
#ifndef CC_SECOND_B2R_SUPPORT
        _B2rSetInterruptDelay();
#endif

#if defined(B2R_AUTO_TEST) || defined(CC_B2R_EMULATION)
        B2R_REG_DB_W_B(B2R_REG_PCRT, (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M),
                                              (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M|B2R_REG_HDISR_M|B2R_REG_SDISR_M));

#endif

#ifdef VDP_B2R_PSCAN_CRC_FLOW
        B2R_REG_DB_W_B(B2R_REG_PCRT, (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M),
                                              (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M|B2R_REG_HDISR_M|B2R_REG_SDISR_M));

#endif
    }
    else
    {
        // disable double register buffer
        B2R_REG_DB_W_B(B2R_REG_RHEN, 0, B2R_REG_RHEN_VPTE_M);

        B2R_REG_DB_W_B(B2R_REG_RHEN,
                       0,
                       (B2R_REG_RHEN_M|B2R_REG_RHEN_ADIE_M|B2R_REG_RHEN_VEIE_M));

#ifndef CC_SECOND_B2R_SUPPORT
        VDP_HalSetB2RPower(FALSE);
#endif

#ifdef VDP_FRC_AUTO_STOP_CTRL
        //vDrvDIFrameRepeat(ucVdpId, 0);
#else
        IO_READ32(PSCAN_BASE, 0x150) = (IO_READ32(PSCAN_BASE, 0x150) & 0xBFFFFFFF);
#endif

#ifdef CC_DYNAMIC_POWER_ONOFF
#ifndef CC_SECOND_B2R_SUPPORT
        B2R_REG_W_B(B2R_REG_SRST,
                    (B2R_REG_SRST_HDRST_M | B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M),
                    (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M|B2R_REG_SRST_DMEN_M));
#endif
#endif

#if defined(B2R_AUTO_TEST) || defined(CC_B2R_EMULATION)
        B2R_REG_DB_W_B(B2R_REG_PCRT, 0,
                       (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M));
#endif

#ifdef VDP_B2R_PSCAN_CRC_FLOW
        B2R_REG_DB_W_B(B2R_REG_PCRT, 0,
                       (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M));
#endif
    }

    if (u4Reset)
    {
#ifndef CC_SECOND_B2R_SUPPORT
        B2R_REG_W_B(B2R_REG_SRST, 0, B2R_REG_SRST_DMRST_M);
#endif
    }

#ifndef CC_SECOND_B2R_SUPPORT
    _B2rReadStatus();
#endif
    //B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_RHFINI_M, B2R_REG_HMODE_RHFINI_M);
    B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_RHFT_M, B2R_REG_HITRG_RHFT_M);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetChromaMode.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetChromaMode(UCHAR ucVdpId, UCHAR ucFrameEncode, UCHAR ucTff)
{
    VERIFY_VDP_ID(ucVdpId);

    _rB2rInfo[ucVdpId].u4FrameEncode = ucFrameEncode;
    _rB2rInfo[ucVdpId].u4Tff = ucTff;

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        // interlace sequence
        B2R_REG_DB_W_B(B2R_REG_HMODE,     B2R_REG_HMODE_RHI_M, B2R_REG_HMODE_RHI_M);

        // B2R C field encode flag is obsolete at 5381, always = field encode at B2R, MDDI will handle it
        B2R_REG_DB_W_B(B2R_REG_HMODE,     B2R_REG_HMODE_RHCIS_M, B2R_REG_HMODE_RHCIS_M);

#ifdef CC_SCPOS_EN
#ifndef CC_NO_MDDI
        if (ucVdpId == VDP_1)
        {
            if (ucFrameEncode)
            {
                // frame encode
                //// TODO, bottom field first
#if !defined(CC_MT5368) && !defined(CC_MT5396) //FIX MT5368 drv build error
                vDrvDIFrmEncModeOnOff(ucVdpId, 1, (ucTff == 0));
#endif

                LOG(5, "Chroma Frame Encode TFF(%d)\n", ucTff);
            }
            else
            {
                // field encode
#if !defined(CC_MT5368) && !defined(CC_MT5396) //FIX MT5368 drv build error
                vDrvDIFrmEncModeOnOff(ucVdpId, 0, 0);
#endif
            }
        }
#endif
#endif
    }
    else
    {
        // progressive sequence
        B2R_REG_DB_W_B(B2R_REG_HMODE,     0, B2R_REG_HMODE_RHI_M);

        // progressive sequence >> frame encode
        B2R_REG_DB_W_B(B2R_REG_HMODE,     0, B2R_REG_HMODE_RHCIS_M);

        // reset Field Invert
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);
    }

}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetSrcSize.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetSrcSize(UCHAR ucVdpId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
{
    VERIFY_VDP_ID(ucVdpId);

    _rB2rInfo[ucVdpId].u4SrcWidth = u4Width;
    _rB2rInfo[ucVdpId].u4SrcHeight = u4Height;
    _rB2rInfo[ucVdpId].u4SrcPitch = u4Pitch;

#ifdef CC_SECOND_B2R_SUPPORT
    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        _u4B2rActiveHeight[ucVdpId] = u4Height >> 1;

        _u4B2rInsideActiveRegion[ucVdpId] = ((u4Height * (100 - B2R_INSIDE_ACTIVE_REGION)) / 100) >> 1;
    }
    else
    {
        _u4B2rActiveHeight[ucVdpId] = u4Height;

        _u4B2rInsideActiveRegion[ucVdpId] = ((u4Height * (100 - B2R_INSIDE_ACTIVE_REGION)) / 100);
    }
#else
    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        _u4B2rActiveHeight = u4Height >> 1;

        _u4B2rInsideActiveRegion = ((u4Height * (100 - B2R_INSIDE_ACTIVE_REGION)) / 100) >> 1;
    }
    else
    {
        _u4B2rActiveHeight = u4Height;

        _u4B2rInsideActiveRegion = ((u4Height * (100 - B2R_INSIDE_ACTIVE_REGION)) / 100);
    }
#endif

    // B2R
    B2R_REG_DB_W_M(B2R_REG_HPH, (u4Pitch >> 4), B2R_REG_HPH_RHPH_M, B2R_REG_HPH_RHPH_S);

#ifdef CC_SCALER_LR_SHIFT_WA
    u4Height++; // for scaler workaround
#endif

    B2R_REG_DB_W_M(B2R_REG_HACT, (u4Width >> 1), B2R_REG_HACT_RHAW_M, B2R_REG_HACT_RHAW_S);

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        B2R_REG_DB_W_M(B2R_REG_HACT, (u4Height >> 1), B2R_REG_HACT_HAH_M, B2R_REG_HACT_HAH_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_HACT, u4Height, B2R_REG_HACT_HAH_M, B2R_REG_HACT_HAH_S);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetSrcSize.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetPswWidth(UCHAR ucVdpId, UINT32 u4PswEn, UINT32 u4Width, UINT32 u4Pitch)
{
    //// TODO
    UNUSED(ucVdpId);
    UNUSED(u4PswEn);
    UNUSED(u4Width);
    UNUSED(u4Pitch);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetOutClock.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetOutClock(UCHAR ucVdpId, UINT32 u4Clock)
{
    UNUSED(ucVdpId);

    if (_rB2rInfo[ucVdpId].u4OutClk != u4Clock)
    {
        _rB2rInfo[ucVdpId].u4OutClk = u4Clock;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetOutSize.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetOutSize(UCHAR ucVdpId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Interlace, UINT32 u4DoubleClk, UINT32 u4ClkMode)
{
    static UINT32 _u4B2RClkMode = 255;
#ifdef CC_SECOND_B2R_SUPPORT
    static UINT32 Iu4B2RClkModeSdPath = 0xff;
#endif
	VDP_PRM_T* prB2rPrmAnotherPath = NULL;
    BOOL fgResetDRAMClock = TRUE;

    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return;
    }

    VERIFY_VDP_ID(ucVdpId);

    if ((u4Interlace != _rB2rInfo[ucVdpId].u4OutInterlace) ||
#ifdef CC_SECOND_B2R_SUPPORT
            (ucVdpId == VDP_1 && u4ClkMode != _u4B2RClkMode) ||
            (ucVdpId == VDP_2 && u4ClkMode != Iu4B2RClkModeSdPath))
#else
            (u4ClkMode != _u4B2RClkMode))
#endif
    {
        //_VDP_SetScposDelayUnMute(ucVdpId, 6, 1);
        _vDrvVideoSetMute(MUTE_MODULE_B2R, ucVdpId, 6, TRUE);
    }

    _rB2rInfo[ucVdpId].u4OutWidth = u4Width;
    _rB2rInfo[ucVdpId].u4OutHeight = u4Height;
    _rB2rInfo[ucVdpId].u4OutInterlace = u4Interlace;
    _rB2rInfo[ucVdpId].u4OutDoubleClk = u4DoubleClk;


#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396)
    {
        //setting VDOIN register
        //HD path
        UINT32 u4VdoInReg = IO_REG32(0xf0022000, 0xbe0);
        u4VdoInReg = u4VdoInReg | 0x80000000;
        IO_REG32(0xf0022000, 0xbe0) = u4VdoInReg;

#ifdef CC_SECOND_B2R_SUPPORT
        //turn off mjc interrupt
        //u4VdoInReg = IO_REG32(0xf0036000, 0x8);
        //u4VdoInReg = u4VdoInReg | 0xF000000;
        //IO_REG32(0xf0036000, 0x8) = u4VdoInReg;

        //SD path
        if(ucVdpId == VDP_2)
        {
            /*VDP_REGION_T rOutRegion;

            rOutRegion.u4X =0;
            rOutRegion.u4Y =0;
            rOutRegion.u4Width =5000;
            rOutRegion.u4Height =5000;

            VDP_SetOutRegion(VDP_2,0,rOutRegion);
            */

            u4VdoInReg = IO_REG32(0xf0022000, 0xbe4);
            u4VdoInReg = u4VdoInReg | 0x80000000;
            IO_REG32(0xf0022000, 0xbe4) = u4VdoInReg;
        }
#if defined(CC_FLIP_MIRROR_SUPPORT)
        if(u4GetFlipMirrorModule(VDP_1)==FLIP_BY_B2R)
        {
            UINT8 u1FlipMirrorEnable = u1GetFlipMirrorConfig();
            if(u1FlipMirrorEnable & SYS_FLIP_CONFIG_ON)
            {
                B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FLIP_M, B2R_REG_HMODE_FLIP_M);  //enable b2r flip
            }
            if(u1FlipMirrorEnable & SYS_MIRROR_CONFIG_ON)
            {
                B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_MIRROR_M, B2R_REG_HMODE_MIRROR_M);  //enable b2r mirror
            }
        }
        else
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FLIP_M);  //disnble b2r flip
            B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_MIRROR_M);  //disable b2r mirror
        }
#endif
#endif

#ifdef CC_B2R_3D_SUPPROT
        //set 3D relative register
        VDP_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucVdpId);
        //static BOOL tmp = TRUE;
        //if(tmp)
        {
            if(prB2rPrm->rPortPrm.fgB2R3DEnable && !(prB2rPrm->rPortPrm.fgB2RForce2D))
            {
                if(prB2rPrm->rPortPrm.ucProgressiveSeqFlag) // progressive && 3D
                {
                    VDP_HalSetRepeatForever(ucVdpId,FALSE);
                }
                else
                {
#ifdef CC_B2R_3D_MVC_INTERLACE_SUPPORT
                    VDP_HalSetRepeatForever(ucVdpId,FALSE);

#else
                    VDP_HalSetRepeatForever(ucVdpId,TRUE);
#endif
                }

                if(prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL)
                {
#ifdef CC_B2R_3D_MVC_INTERLACE_SUPPORT
                    if(prB2rPrm->rPortPrm.ucProgressiveSeqFlag)
                    {
                        B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                    }
                    else
                    {
                        B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                    }
#else
                    B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
#endif
                    B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_S);
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
                }
                else if(prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D || prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_SIDE_BY_SIDE)
                {
                    //enable auto switch mode
                    B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_M);
                    B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                    //B2R_REG_W_B(B2R_REG_OFFSET, _prB2rPrm->rPortPrm.u4B2R3DWidthOffset, B2R_REG_HOFFSET_WIDTH_M);
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->rPortPrm.u4B2R3DWidthOffset, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
                }
                else if(prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_TOP_N_BOTTOM)
                {
                    B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_M);
                    B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                    //B2R_REG_W_B(B2R_REG_OFFSET, _prB2rPrm->rPortPrm.u4B2R3DHeightOffset, B2R_REG_HOFFSET_HEIGHT_M);
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->rPortPrm.u4B2R3DHeightOffset, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
                }
            }
            else if(prB2rPrm->rPortPrm.fgB2R3DEnable && prB2rPrm->rPortPrm.fgB2RForce2D)
            {
                B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M);
                B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
                B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);

                if((prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D || prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_SIDE_BY_SIDE)
                        && (prB2rPrm->rPortPrm.u1B2RForce2DType == B2R_FORCE_2D_R_VIEW))
                {
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->rPortPrm.u4B2R3DWidthOffset, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
                }
                else if((prB2rPrm->rPortPrm.u1B2R3DType == B2R_3D_TOP_N_BOTTOM)
                        &&(prB2rPrm->rPortPrm.u1B2RForce2DType == B2R_FORCE_2D_R_VIEW))
                {
                    B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->rPortPrm.u4B2R3DHeightOffset, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
                }
            }
            else
            {
                B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M);
                B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
                B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
            }
        }
#endif
    }

    B2R_REG_W(B2R_REG_PORCH,0x10021);
#else
#ifndef B2R_AUTO_TEST
    // set porch for MM mode
    if (VDP_GetPlayMode(0) == FBM_FBG_MM_MODE)
    {
        B2R_REG_W(B2R_REG_PORCH,0x21);
    }
    else
    {
        B2R_REG_W(B2R_REG_PORCH,0x21);
    }
#endif
#endif

    // Disable B2R DRAM Control
    prB2rPrmAnotherPath = _VDP_GetVdpPrm(1-ucVdpId);
    if (prB2rPrmAnotherPath && (prB2rPrmAnotherPath->rPortPrm.ucReady == 1))
    {
        LOG(1, "VDP(%d) Ignore DRAM clock reset\n", ucVdpId);
        fgResetDRAMClock = FALSE;        
    }
    if (fgResetDRAMClock)
    {
        B2R_REG_W_B(B2R_REG_SRST, B2R_REG_SRST_DMRST_M, B2R_REG_SRST_DMRST_M);
    }

    if (_eIcVersion != IC_VER_FPGA)
    {

#ifdef CC_SECOND_B2R_SUPPORT
        if((ucVdpId == VDP_1 && (u4ClkMode != _u4B2RClkMode)) ||
                (ucVdpId == VDP_2 && (u4ClkMode != Iu4B2RClkModeSdPath)))
        {
            LOG(3, "B2R CLK CHANGE %d >> %d \n", _u4B2RClkMode, u4ClkMode);
            if(ucVdpId == VDP_1)
            {
                _u4B2RClkMode = u4ClkMode;
            }
            else
            {
                Iu4B2RClkModeSdPath = u4ClkMode;
            }
#else
        if (u4ClkMode != _u4B2RClkMode)
        {
            LOG(3, "B2R CLK CHANGE %d >> %d \n", _u4B2RClkMode, u4ClkMode);
            _u4B2RClkMode = u4ClkMode;
#endif

#if defined(CC_MT5387) && !defined(CC_MT5363)
            //0xD23C[8] = 1, from syspll
            IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFEFF) + (0x100);

            if (IS_SYSPLL_432())
            {
                LOG(3, "syspll = 432MHz\n");
                switch (_u4B2RClkMode)
                {
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;          // Div 2
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (8000 << 16);    // N

                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 1;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 1;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
#ifndef B2R_SD_TEST
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
#else
                    IO_REG32(CKGEN_BASE, 0x244) = (IO_REG32(CKGEN_BASE, 0x244) & 0xFFFFFEFF) + (0x100);
                    IO_REG32(CKGEN_BASE, 0x244) = (IO_REG32(CKGEN_BASE, 0x244) & 0xFFFFFFF8) + 2;
                    IO_REG32(CKGEN_BASE, 0x248) = (IO_REG32(CKGEN_BASE, 0x248) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x248) = (IO_REG32(CKGEN_BASE, 0x248) & 0x0000FFFF) + (8 << 16);    // N
#endif
                    break;
                }
            }
            else
            {
                LOG(3, "syspll = 324MHz\n");
                switch (_u4B2RClkMode)
                {
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;          // Div 2
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6000 << 16);    // N

                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 250;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (546 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 22;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (48 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 1;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 250;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (546 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 1;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (24 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
#ifndef B2R_SD_TEST
                    IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 2;                    // No Div 2
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6 << 16);    // N
#else
                    IO_REG32(CKGEN_BASE, 0x244) = (IO_REG32(CKGEN_BASE, 0x244) & 0xFFFFFEFF) + (0x100);
                    IO_REG32(CKGEN_BASE, 0x244) = (IO_REG32(CKGEN_BASE, 0x244) & 0xFFFFFFF8) + 2;                    // No Div 2
                    IO_REG32(CKGEN_BASE, 0x248) = (IO_REG32(CKGEN_BASE, 0x248) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x248) = (IO_REG32(CKGEN_BASE, 0x248) & 0x0000FFFF) + (6 << 16);    // N
#endif
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
            }
#endif

#ifdef CC_MT5363
            //0xD23C[8] = 1, from syspll
            IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFEFF) + (0x100);


            LOG(3, "syspll = 324MHz\n");
            switch (_u4B2RClkMode)
            {
            case VDP_B2R_CLK_27_027:
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;          // Div 2
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1001;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6000 << 16);    // N

                B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_74_176:
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 250;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (546 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_74_250:
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 22;                   // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (48 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_148_352:
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 250;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (546 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_148_500:
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (24 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_27_000:
            default:
#ifndef B2R_SD_TEST
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;                    // No Div 2
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6 << 16);    // N
#else
                IO_REG32(CKGEN_BASE, 0x244) = (IO_REG32(CKGEN_BASE, 0x244) & 0xFFFFFEFF) + (0x100);
                IO_REG32(CKGEN_BASE, 0x244) = (IO_REG32(CKGEN_BASE, 0x244) & 0xFFFFFFF8) + 4;                    // No Div 2
                IO_REG32(CKGEN_BASE, 0x248) = (IO_REG32(CKGEN_BASE, 0x248) & 0xFFFF0000) + 1;                  // M
                IO_REG32(CKGEN_BASE, 0x248) = (IO_REG32(CKGEN_BASE, 0x248) & 0x0000FFFF) + (6 << 16);    // N
#endif
                B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            }
#endif

#ifdef CC_MT5365
            //0xD23C[8] = 1, from syspll
            //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFEFF) + (0x100);
            IO_REG32(0xf0022000, 0xbe0) = ((IO_REG32(0xf0022000, 0xbe0) & 0xFFFFFFFF) | 0x80000000);
            IO_REG32(CKGEN_BASE, 0x23c) = (0x1);

            LOG(3, "DMPLL = 175.5 MHz\n");
            switch (_u4B2RClkMode)
            {
            case VDP_B2R_CLK_27_027:
                //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;          // Div 2
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 77;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (500 << 16);    // N

                B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_74_176:
                //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 500;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (1183 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_74_250:
                //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (26 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_148_352:
                //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1000;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (1183 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_148_500:
                //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (13 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;

            case VDP_B2R_CLK_27_000:
            default:
                //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;                    // No Div 2
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2;                  // M
                IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (13 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            }
#endif

#ifdef CC_MT5395
#ifdef CC_SECOND_B2R_SUPPORT
#ifndef CC_DISABLE_TWO_B2R
            if(ucVdpId == VDP_1)
#endif
#endif
            {
                //0xD23C[8] = 1, from syspll
                IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFEFE) + (0x101);

                LOG(3, "SYSPLL = 168.75 MHz\n");

                switch (_u4B2RClkMode)
                {
                case VDP_B2R_CLK_27_027:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;          // Div 2
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6250 << 16);    // N

                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 8;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (25 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6250 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_74_176:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 40;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (91 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (25 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 80;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (91 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 22;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (25 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;                    // No Div 2
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 4;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (25 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
            }
#ifdef CC_SECOND_B2R_SUPPORT
#ifndef CC_DISABLE_TWO_B2R
            else
            {
                //0xD23C[8] = 1, from syspll
                IO_REG32(CKGEN_BASE, 0x2e0) = (IO_REG32(CKGEN_BASE, 0x2e0) & 0xFFFFFEFE) + (0x101);

                LOG(3, "SYSPLL = 168.75 MHz\n");

                switch (Iu4B2RClkModeSdPath)
                {
                case VDP_B2R_CLK_27_027:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;          // Div 2
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (6250 << 16);    // N

                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 40;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (91 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (25 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 80;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (91 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 3;
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 22;                   // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (25 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    //IO_REG32(CKGEN_BASE, 0x23c) = (IO_REG32(CKGEN_BASE, 0x23c) & 0xFFFFFFF8) + 4;                    // No Div 2
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 4;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (25 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
            }
#endif
#endif

#endif


#ifdef CC_MT5396
#ifdef CC_SECOND_B2R_SUPPORT
#ifndef CC_DISABLE_TWO_B2R
            if(ucVdpId == VDP_1)
#endif
#endif
            {
                //0xD23C[8] = 1, from syspll
                IO_REG32(CKGEN_BASE, 0x23c) = (0x1);

                // SD clock, if RealD, the clock is hardwired connect to the clock of HD
                // It means the M, N of SD will be ignored.
                IO_REG32(CKGEN_BASE, 0x2e0) = VERIFICATION_REALD ? (0x10001) : (0x1);

                LOG(3, "SYSPLL = 216 MHz\n");

                switch (_u4B2RClkMode)
                {
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 8;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (25 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6250 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (182 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (16 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
            }
#ifdef CC_SECOND_B2R_SUPPORT
#ifndef CC_DISABLE_TWO_B2R
            else
            {
                //0xD23C[8] = 1, from syspll
                //IO_REG32(CKGEN_BASE, 0x2e0) = (IO_REG32(CKGEN_BASE, 0x2e0) & 0xFFFFFEFE) + (0x101);
                IO_REG32(CKGEN_BASE, 0x2e0) = (0x1);

                LOG(3, "SYSPLL = 216 MHz\n");

                switch (Iu4B2RClkModeSdPath)
                {
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 8;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (25 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (6250 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (182 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (16 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
            }
#endif
#endif

#endif



            B2R_REG_W_B(B2R_REG_PCRT, 0x1, (B2R_REG_PCRT_M|B2R_REG_HDISR_M|B2R_REG_SDISR_M));


            if (_rB2rInfo[ucVdpId].u4OutHeight > B2R_720P_OUT_HEIGHT)
            {
                // DRAM Req = 16
                B2R_REG_DB_W_M(B2R_REG_HMODE, 1, B2R_REG_HMODE_RHRQTH_M, B2R_REG_HMODE_RHRQTH_S);
            }
            else
            {
                // DRAM Req = 32
                B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_RHRQTH_M, B2R_REG_HMODE_RHRQTH_S);
            }
        }
    }

    // B2R_REG_HMODE_HTBM_M = 0
    // B2R_REG_HMODE_HBTM_M = 0
    //B2R_REG_DB_W_B(B2R_REG_HMODE, 0, (B2R_REG_HMODE_HTBM_M | B2R_REG_HMODE_HBTM_M));

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_RHI_M, B2R_REG_HMODE_RHI_M);

#if 0 // defined(CC_MT5360B) || defined (CC_MT5392B) //MT5360B or MT5392B
#ifdef VDP_FRC_AUTO_STOP_CTRL
        vDrvDIFrameRepeat(ucVdpId, 1);
#else
        IO_READ32(PSCAN_BASE, 0x150) = (IO_READ32(PSCAN_BASE, 0x150) & 0xBFFFFFFF) | 0x40000000;
#endif
#endif

    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_RHI_M);
    }

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        B2R_REG_DB_W_M(B2R_REG_HHVT, (u4Height >> 1), B2R_REG_HHVT_HVT_M, B2R_REG_HHVT_HVT_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_HHVT, (u4Height - 1), B2R_REG_HHVT_HVT_M, B2R_REG_HHVT_HVT_S);
    }

    if (u4DoubleClk == 1)
    {
        B2R_REG_DB_W_M(B2R_REG_HHVT, ((u4Width << 1) - 1), B2R_REG_HHVT_HHT_M, B2R_REG_HHVT_HHT_S);
        B2R_REG_DB_W_M(B2R_REG_HDER, 0x200, B2R_REG_HDER_M, B2R_REG_HDER_S);
    }
    else  if (u4DoubleClk == 2)
    {
        B2R_REG_DB_W_M(B2R_REG_HHVT, (((u4Width * 5) >> 1) - 1), B2R_REG_HHVT_HHT_M, B2R_REG_HHVT_HHT_S);
        B2R_REG_DB_W_M(B2R_REG_HDER, 0x19a, B2R_REG_HDER_M, B2R_REG_HDER_S);
    }
    else  if (u4DoubleClk == 3)
    {
        B2R_REG_DB_W_M(B2R_REG_HHVT, (((u4Width * 5) >> 2) - 1), B2R_REG_HHVT_HHT_M, B2R_REG_HHVT_HHT_S);
        B2R_REG_DB_W_M(B2R_REG_HDER, 0x334, B2R_REG_HDER_M, B2R_REG_HDER_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_HHVT, (u4Width - 1), B2R_REG_HHVT_HHT_M, B2R_REG_HHVT_HHT_S);
        B2R_REG_DB_W_M(B2R_REG_HDER, 0, B2R_REG_HDER_M, B2R_REG_HDER_S);
    }

    if (_eIcVersion == IC_VER_FPGA)
    {
        // special case for FPGA, 1080i
        if (u4Width == 2200)
        {
            B2R_REG_DB_W_M(B2R_REG_HHVT, (u4Width - 1), B2R_REG_HHVT_HHT_M, B2R_REG_HHVT_HHT_S);
            B2R_REG_DB_W_M(B2R_REG_HDER, 0, B2R_REG_HDER_M, B2R_REG_HDER_S);
        }
    }

    // Enable B2R DRAM Control
    if (fgResetDRAMClock)
    {
        B2R_REG_W_B(B2R_REG_SRST, 0, B2R_REG_SRST_DMRST_M);
    }

    // Set Interrupt Delay according to Output Format
#ifdef CC_SECOND_B2R_SUPPORT
    _B2rSetInterruptDelay(ucVdpId);
#else
    _B2rSetInterruptDelay();
#endif

    // The following settings depends on interlace/progressive output mode

    // Consider frame/field encode when mode change
    _VDP_HalSetChromaMode(ucVdpId, _rB2rInfo[ucVdpId].u4FrameEncode, _rB2rInfo[ucVdpId].u4Tff);

    // Consider MDDi mode change
    _VDP_HalSetEnable(ucVdpId, _rB2rInfo[ucVdpId].u4En);

    // Consider Source Size
    _VDP_HalSetSrcSize(ucVdpId,
                       _rB2rInfo[ucVdpId].u4SrcWidth,
                       _rB2rInfo[ucVdpId].u4SrcHeight,
                       _rB2rInfo[ucVdpId].u4SrcPitch);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetBg.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetBg(UCHAR ucVdpId, UINT32 u4BgColor)
{
    if( ucVdpId >= VDP_NS )
    {
        return;
    }

    B2R_REG_DB_W_M(B2R_REG_HFC, u4BgColor, B2R_REG_HFC_YCBCR_M, B2R_REG_HFC_YCBCR_S);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalForceBg.
 */
//-----------------------------------------------------------------------------
void _VDP_HalForceBg(UCHAR ucVdpId, UCHAR ucForce)
{
    VERIFY_VDP_ID(ucVdpId);

    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return;
    }

    if (_aucB2rSwitch[ucVdpId] != _aucB2rSwitch[1-ucVdpId])
    {
        if (ucForce != 0)
        {
            B2R_REG_DB_W_B(B2R_REG_HFC, B2R_REG_HFC_EN_M, B2R_REG_HFC_EN_M);
        }
        else
        {
            B2R_REG_DB_W_B(B2R_REG_HFC, 0, B2R_REG_HFC_EN_M);
        }
    }
    else
    {
        if (ucForce == 0)
        {
            B2R_REG_DB_W_B(B2R_REG_HFC, 0, B2R_REG_HFC_EN_M);
        }
        else
        {
            LOG(5, "DTV Scart out, can't set B2R to BG mode\n");
        }
    }

}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetAddr.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetAddr(UCHAR ucVdpId, UINT32 u4AddrY, UINT32 u4AddrC)
{
    VERIFY_VDP_ID(ucVdpId);

#ifdef VDP_FRAME_ENCODE_TEST
    _u4FieldEncodeCounter++;
    if (_u4FieldEncodeCounter & 1)
    {
        u4AddrY = _u4FieldEncodeLuma1;
    }
    else
    {
        u4AddrY = _u4FieldEncodeLuma2;
    }
    u4AddrC = _u4FieldEncodeChroma;
#endif

    if(u4AddrY!= NULL && u4AddrC != NULL)
    {
        B2R_REG_DB_W(B2R_REG_RHYS, u4AddrY);
        B2R_REG_DB_W(B2R_REG_RHCS, u4AddrC);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetTopFieldFirst.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetTopFieldFirst(UCHAR ucVdpId, UCHAR ucTff)
{
    UNUSED(ucVdpId);
    UNUSED(ucTff);

    // B2R Top Field First is NOT required
    // Software can control Top/Bottm Field First
    // Top Field First: Change Frame Buffer when bottom finish
    // Bottom Field First: Change Frame Buffer when top finish
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetFrc.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetFrc(UCHAR ucVdpId, UCHAR ucTargetNs, UCHAR ucChangeFieldNs, UCHAR ucMode)
{
    VERIFY_VDP_ID(ucVdpId);
    VDP_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucVdpId);
    UINT8 u1DecoderSrcId;

    if(prB2rPrm != NULL)
    {
        u1DecoderSrcId = FBM_GetDecoderSrcId(prB2rPrm->rPortPrm.ucFbgId);
    }
    else
    {
        u1DecoderSrcId = B2R_1;
        ASSERT(0);
    }

    if (_rB2rInfo[ucVdpId].u4OutInterlace == 0)
    {
        // Progressive mode, no FRC required
        _rB2rFrc[ucVdpId].ucFrcEn = 0;
        return;
    }

    //IO_READ32(PSCAN_BASE, 0x13c) = (IO_READ32(PSCAN_BASE, 0x13c) & 0xFFFFFF);

    if (ucTargetNs < ucChangeFieldNs)
    {
        ASSERT(ucTargetNs >= ucChangeFieldNs);
        return;
    }

#ifdef VDP_FRC_LOG
    _szDebugBufTns[_u4DebugIdx] = '0' + ucTargetNs;
    _szDebugBufCns[_u4DebugIdx] = '0' + ucChangeFieldNs;
    _szDebugBufSm[_u4DebugIdx] = '0' + ucMode;
#endif


    if (VDP_HalGetTrickMode(u1DecoderSrcId)==VDP_PLAY_TRICK)
    {
        ucMode = VDP_FRC_TRICK_PLAY;
    }

    if (ucMode == VDP_FRC_NONE)
    {
        // wrong field case

        _rB2rFrc[ucVdpId].ucFrcEn = 0;

        // Clean HFI bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);
    }
    else if ((ucMode == VDP_FRC_NORMAL) &&
             ((ucTargetNs == 0) ||
              ((ucTargetNs == 2) && (ucChangeFieldNs == 1))))
    {
        // Normal: Top + Bottom (Single Field)

        _rB2rFrc[ucVdpId].ucFrcEn = 0;

        // Clean HFI bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);
    }
    else if (ucMode == VDP_FRC_CANCEL)
    {
        //clear repeat count toogle bit
        B2R_REG_DB_W_M(B2R_REG_RMAP,0, B2R_REG_RMAP_REP_CNT_M, B2R_REG_RMAP_REP_CNT_S);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);
    }
    else
    {
#ifdef VDP_BYPASS_FRC
        _rB2rFrc[ucVdpId].ucFrcEn = 0;

        // Clean HFI bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);

        return;
#else

        _rB2rFrc[ucVdpId].ucFrcEn = 1;

#ifdef VDP_FRC_LOG
        _u4DebugPrint = 1;
#endif

#ifdef VDP_FRC_TRAP
        if (ucTargetNs == 20)
        {
            _u4VdpTrap = 1;
        }
        else
        {
            _u4VdpTrap = 0;
        }
#endif

#ifdef VDP_FRC_WA2
        ucChangeFieldNs = ucTargetNs;
#endif

        if (ucMode == VDP_FRC_STOP_AUTO)
        {
            //copy bottom field to top field WA
            //_VdpGfxCopyField(ucVdpId);
            ucChangeFieldNs = ucTargetNs;
        }

        _rB2rFrc[ucVdpId].ucCurrentNs = 0;
        _rB2rFrc[ucVdpId].ucTargetNs = ucTargetNs;
        _rB2rFrc[ucVdpId].ucChangeFieldNs = ucChangeFieldNs;
        _rB2rFrc[ucVdpId].ucMode = ucMode;

#ifdef CC_SECOND_B2R_SUPPORT
        _VdpHandleFrc(ucVdpId);
#else
        _VdpHandleFrc();
#endif

#endif
    }


}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetFormat.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetFormat(UCHAR ucVdpId, UINT32 u4Mode422, UINT32 u4ModeLinear)
{
    VERIFY_VDP_ID(ucVdpId);

    // 422 Mode vs Repeat C

    _rB2rInfo[ucVdpId].u4Mode422 = u4Mode422;
    _rB2rInfo[ucVdpId].u4ModeLinear = u4ModeLinear;

    if (u4Mode422 != 0)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_H422_M, B2R_REG_HMODE_H422_M);

        //B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HCRPT_M);

#if 0//CC_SCPOS_EN
        if (ucVdpId == VDP_1)
        {
            vDrvDIOnOff(ucVdpId, 0);
        }
#endif
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_H422_M);

#if 0
        if (ucVdpId == VDP_1)
        {
            vDrvDISetColorMode(ucVdpId, 1);
        }
#endif
    }

    if (u4ModeLinear != 0)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_ADDR_M, B2R_REG_HMODE_ADDR_M);
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_ADDR_M);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetQVEnable.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetQVEnable(UCHAR ucVdpId, UCHAR ucEnable)
{
    ASSERT(ucEnable == 0);
    UNUSED(ucVdpId);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetDeintEnable.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetDeintEnable(UCHAR ucVdpId, UCHAR ucEnable)
{
    ASSERT(ucEnable == 0);
    UNUSED(ucVdpId);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetColorMode.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetColorMode(UCHAR ucVdpId, UCHAR ucColorMode)
{
    UNUSED(ucVdpId);
    UNUSED(ucColorMode);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetQVdoLT.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetQVdoLT(UCHAR ucVdpId, UINT32 u4AddrY, UINT32 u4AddrC)
{
    UNUSED(ucVdpId);
    UNUSED(u4AddrY);
    UNUSED(u4AddrC);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetQVdoRT.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetQVdoRT(UCHAR ucVdpId, UINT32 u4AddrY, UINT32 u4AddrC)
{
    UNUSED(ucVdpId);
    UNUSED(u4AddrY);
    UNUSED(u4AddrC);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetQVdoLB.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetQVdoLB(UCHAR ucVdpId, UINT32 u4AddrY, UINT32 u4AddrC)
{
    UNUSED(ucVdpId);
    UNUSED(u4AddrY);
    UNUSED(u4AddrC);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetQVdoRB.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetQVdoRB(UCHAR ucVdpId, UINT32 u4AddrY, UINT32 u4AddrC)
{
    UNUSED(ucVdpId);
    UNUSED(u4AddrY);
    UNUSED(u4AddrC);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalConfig.
 */
//-----------------------------------------------------------------------------
void _VDP_HalConfig(UCHAR ucVdpId)
{
    UNUSED(ucVdpId);

    // status will clean after global isr clean
#ifdef CC_SECOND_B2R_SUPPORT
    _B2rReadStatus(ucVdpId);
#else
    B2R_R_STATUS(B2R_1);
#endif

    _prDbase->rVdp.au4UnderFlow[VDP_1] = _u4CurrentUnderFlow;
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalInsideBlankRegion.
 */
//-----------------------------------------------------------------------------
UINT32 _VDP_HalInsideBlankRegion(UCHAR ucVdpId)
{
    if (VDP_TO_B2R(ucVdpId) >= B2R_NS)
    {
        return 0;
    }

#ifdef CC_SECOND_B2R_SUPPORT
    _B2rReadStatus(ucVdpId);
#else
    _B2rReadStatus();
#endif

    if (_u4CurrentLineCount < _u4B2rInsideActiveRegion)
    {
        return 0;
    }
    else
    {
        return _u4CurrentLineCount;
    }
}

#if 0 /* RR mode removal */
//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetExpansion.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetExpansion(UCHAR ucVdpId, BOOL fgYEnable, BOOL fgCEnable,BOOL fgYRatio34, BOOL fgCRatio34)
{
    VERIFY_VDP_ID(ucVdpId);

    if (fgYEnable||fgCEnable)
    {
        B2R_REG_DB_W_B(B2R_REG_SRST, 0x0, B2R_REG_RR_BYPASS_M);
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0x1000000, B2R_REG_HMODE_ADDR_M);

        B2R_REG_DB_W_B(B2R_REG_SRST,
                       ((fgYEnable << B2R_REG_RR_HDY_RREN_S)|(fgYRatio34 <<B2R_REG_RR_HDY_RRTP_S) |
                        (fgCEnable << B2R_REG_RR_HDC_RREN_S)|(fgCRatio34 <<B2R_REG_RR_HDC_RRTP_S)),
                       (B2R_REG_RR_HDY_RREN_M|B2R_REG_RR_HDY_RRTP_M|B2R_REG_RR_HDC_RREN_M|B2R_REG_RR_HDC_RRTP_M));

#if defined(CC_MT5360B) || defined (CC_MT5392B) //MT5360B or MT5392B
        if (fgYEnable)
        {
            if (_rB2rInfo[ucVdpId].u4OutInterlace)
            {
                B2R_REG_DB_W_B(B2R_REG_SRST, B2R_REG_RR_NEWMODEY_M, B2R_REG_RR_NEWMODEY_M);
            }
            else
            {
                B2R_REG_DB_W_B(B2R_REG_SRST, 0x0, B2R_REG_RR_NEWMODEY_M);
            }

            B2R_REG_DB_W_B(B2R_REG_SRST, B2R_REG_RR_CACHEY_M, B2R_REG_RR_CACHEY_M);
        }

        if (fgCEnable)
        {
            if (_rB2rInfo[ucVdpId].u4OutInterlace)
            {
                B2R_REG_DB_W_B(B2R_REG_SRST, B2R_REG_RR_NEWMODEC_M, B2R_REG_RR_NEWMODEC_M);
            }
            else
            {
                B2R_REG_DB_W_B(B2R_REG_SRST, 0x0, B2R_REG_RR_NEWMODEC_M);
            }

            B2R_REG_DB_W_B(B2R_REG_SRST, B2R_REG_RR_CACHEC_M, B2R_REG_RR_CACHEC_M);

        }
#endif
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_SRST, 0x0, (B2R_REG_RR_HDY_RREN_M|B2R_REG_RR_HDC_RREN_M));
        B2R_REG_DB_W_B(B2R_REG_SRST, B2R_REG_RR_BYPASS_M, B2R_REG_RR_BYPASS_M);
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0x0, B2R_REG_HMODE_ADDR_M);

    }
}
#endif

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalB2rSwitch.
 */
//-----------------------------------------------------------------------------
void _VDP_HalB2rSwitch(UCHAR ucVdpId, UCHAR ucB2rId)
{
#ifndef CC_SUPPORT_TVE

    _VDP_VsyncMutexLock();
    if ((ucB2rId >= B2R_NS) || (ucVdpId >= VDP_NS))
    {
        _VDP_VsyncMutexUnlock();
        return;
    }

#ifdef CC_SCPOS_EN
    if (_aucB2rSwitch[ucVdpId] != ucB2rId)
    {
        UCHAR ucTemp;

        LOG(3, "B2R(%d) >> VDP(%d)\n", ucB2rId, ucVdpId);

        // switching vdp-to-b2r
        ucTemp = _aucB2rSwitch[VDP_1];
        _aucB2rSwitch[VDP_1] = _aucB2rSwitch[VDP_2];
        _aucB2rSwitch[VDP_2] = ucTemp;

        // switching b2r-to-vdp
        _ucVdpSwitch = ucVdpId;

        // double check
        ASSERT(_aucB2rSwitch[ucVdpId] == ucB2rId);

        // Consider MDDi frame buffer addr change
        _VDP_HalSetEnable(ucVdpId, (UCHAR)(_rB2rInfo[ucVdpId].u4En));
    }
#endif
    _VDP_VsyncMutexUnlock();

#else
    _VDP_VsyncMutexLock();
    if ((ucVdpId >= VDP_NS))
    {
        _VDP_VsyncMutexUnlock();
        return;
    }

#ifdef CC_SCPOS_EN

    LOG(3, "B2R(%d) >> VDP(%d)\n", ucB2rId, ucVdpId);

    /* Main and Sub are both with B2R and to detach B2R from Main */
    /* Main is DTV, Sub is DTV SCART out, then change Main's input source */
    if ((_aucB2rSwitch[ucVdpId] == _aucB2rSwitch[1-ucVdpId])/* && (VDP_1 == ucVdpId)*/)
    {
        if ( ucB2rId == B2R_NS)
        {
            if (_ucVdpSwitch == ucVdpId)
            {
                _ucVdpSwitch = (1 - ucVdpId);
            }

            if (VDP_1 == ucVdpId)
            {
                _VDP_CopyFrcPrm(ucVdpId);

                _ucVdpSwitch = (1 - ucVdpId);
#if 1
                // Copy VdpConf here.
                _prVdpConf[_ucVdpSwitch]->u4MpegHeight = _prVdpConf[ucVdpId]->u4MpegHeight;
                _prVdpConf[_ucVdpSwitch]->u4MpegWidth = _prVdpConf[ucVdpId]->u4MpegWidth;
                //_prVdpConf[_ucVdpSwitch]->rOutInfo = _prVdpConf[ucVdpId]->rOutInfo;
                x_memcpy(&_prVdpConf[_ucVdpSwitch]->rOutInfo, &_prVdpConf[ucVdpId]->rOutInfo, sizeof(VDP_OUT_INFO_T));
                _prVdpConf[_ucVdpSwitch]->u4SrcLineSize = _prVdpConf[ucVdpId]->u4SrcLineSize;
                _prVdpConf[_ucVdpSwitch]->u4SrcHeight = _prVdpConf[ucVdpId]->u4SrcHeight;
                _prVdpConf[_ucVdpSwitch]->u4SrcWidth = _prVdpConf[ucVdpId]->u4SrcWidth;
                _prVdpConf[_ucVdpSwitch]->u4FrameRate = _prVdpConf[ucVdpId]->u4FrameRate;

                _prVdpConf[_ucVdpSwitch]->ucEarlyDisp = _prVdpConf[ucVdpId]->ucEarlyDisp;
                _prVdpConf[_ucVdpSwitch]->u4AspectRatio = _prVdpConf[ucVdpId]->u4AspectRatio;
                _prVdpConf[_ucVdpSwitch]->u4Afd = _prVdpConf[ucVdpId]->u4Afd;
#endif
            }
        }
    }

    if ((_ucVdpSwitch >= VDP_NS))
    {
        _VDP_VsyncMutexUnlock();
        ASSERT(_ucVdpSwitch < VDP_NS);
        return;
    }

    _aucB2rSwitch[ucVdpId] = ucB2rId;
    if ( ucB2rId < B2R_NS)
    {
        if (_aucB2rSwitch[ucVdpId] == _aucB2rSwitch[1-ucVdpId])
        {
            //When TV scart out and Main is DTV, we use Main to play DTV.
            _VDP_CopyFrcPrm(1 - ucVdpId);
#if 1
            // Copy VdpConf here.
            _prVdpConf[ucVdpId]->u4MpegHeight = _prVdpConf[_ucVdpSwitch]->u4MpegHeight;
            _prVdpConf[ucVdpId]->u4MpegWidth = _prVdpConf[_ucVdpSwitch]->u4MpegWidth;
            //_prVdpConf[ucVdpId]->rOutInfo = _prVdpConf[_ucVdpSwitch]->rOutInfo;
            x_memcpy(&_prVdpConf[ucVdpId]->rOutInfo, &_prVdpConf[_ucVdpSwitch]->rOutInfo, sizeof(VDP_OUT_INFO_T));
            _prVdpConf[ucVdpId]->u4SrcLineSize = _prVdpConf[_ucVdpSwitch]->u4SrcLineSize;
            _prVdpConf[ucVdpId]->u4SrcHeight = _prVdpConf[_ucVdpSwitch]->u4SrcHeight;
            _prVdpConf[ucVdpId]->u4SrcWidth = _prVdpConf[_ucVdpSwitch]->u4SrcWidth;
            _prVdpConf[ucVdpId]->u4FrameRate = _prVdpConf[_ucVdpSwitch]->u4FrameRate;

            /* To protect ucEarlyDisp is set between "connect sub" and "connect main" */
            _prVdpConf[ucVdpId]->ucEarlyDisp = _prVdpConf[_ucVdpSwitch]->ucEarlyDisp;
#ifdef VDP_MUTE_EARLY_DISPLAY
            if (_prVdpConf[ucVdpId]->ucEarlyDisp==1)
            {
                _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, FOREVER_MUTE, FALSE);
                LOG(3, "DTV Mute Early Display -3 VDP(%d)\n", ucVdpId);
            }
#else // VDP_MUTE_EARLY_DISPLAY
            LOG(0, "WARNING: DTV mute early display is disabled \n");
#endif
            _prVdpConf[ucVdpId]->u4AspectRatio = _prVdpConf[_ucVdpSwitch]->u4AspectRatio;
            _prVdpConf[ucVdpId]->u4Afd = _prVdpConf[_ucVdpSwitch]->u4Afd;

#endif

            // switching b2r-to-vdp
            _ucVdpSwitch = ucVdpId;

        }
        else
        {
            // switching b2r-to-vdp
            _ucVdpSwitch = ucVdpId;

            // Consider MDDi frame buffer addr change
            _VDP_HalSetEnable(ucVdpId, _rB2rInfo[ucVdpId].u4En);
        }

    }

#endif
    _VDP_VsyncMutexUnlock();
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalStatusTest.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void _VDP_HalStatusTest (UCHAR ucVdpId)
#else
void _VDP_HalStatusTest (void)
#endif
{
#ifdef VDP_STATUS_TEST
    UINT32 u4Loop = 0;
    UINT32 u4Reg;

    HAL_TIME_T rStartTime;

    VDP_STATUS_LOG_T rHdHsLog;
    VDP_STATUS_LOG_T rHdVsLog;
    VDP_STATUS_LOG_T rHdFieldLog;
    VDP_STATUS_LOG_T rHdFieldLatchLog;

    HAL_GetTime(&rStartTime);

    u4Reg = B2R_REG_R(B2R_REG_HSTA);

    rHdIntLog.u4State = STATUS_LOG_STATE_INIT;
    rHdIntLog.u4CurrentValue = 0;
    rHdIntLog.u4Total = 0;

    rHdHsLog.u4State = STATUS_LOG_STATE_INIT;
    rHdHsLog.u4CurrentValue = (u4Reg & B2R_REG_HSTA_HHACT_M);
    rHdHsLog.u4Total = 0;

    rHdVsLog.u4State = STATUS_LOG_STATE_INIT;
    rHdVsLog.u4CurrentValue = (u4Reg & B2R_REG_HSTA_HVBLK_M);
    rHdVsLog.u4Total = 0;

    rHdFieldLog.u4State = STATUS_LOG_STATE_INIT;
    rHdFieldLog.u4CurrentValue = (u4Reg & B2R_REG_HSTA_HVFLD_M);
    rHdFieldLog.u4Total = 0;

    rHdFieldLatchLog.u4State = STATUS_LOG_STATE_INIT;
    rHdFieldLatchLog.u4CurrentValue = (u4Reg & B2R_REG_HSTA_ADIF_M);
    rHdFieldLatchLog.u4Total = 0;

    while (1)
    {
        u4Reg = B2R_REG_R(B2R_REG_HSTA);

#if 0
        if (rHdHsLog.u4CurrentValue != (u4Reg & B2R_REG_HSTA_HHACT_M))
        {
            _VdpTestStatusChange(&rHdHsLog, (u4Reg & B2R_REG_HSTA_HHACT_M), "HD-HS");
        }
#endif
        if (rHdVsLog.u4CurrentValue != (u4Reg & B2R_REG_HSTA_HVBLK_M))
        {
            _VdpTestStatusChange(&rHdVsLog, (u4Reg & B2R_REG_HSTA_HVBLK_M), "HD-VS");
        }
        if (rHdFieldLog.u4CurrentValue != (u4Reg & B2R_REG_HSTA_HVFLD_M))
        {
            _VdpTestStatusChange(&rHdFieldLog, (u4Reg & B2R_REG_HSTA_HVFLD_M), "HD-F");
        }
        if (rHdFieldLatchLog.u4CurrentValue != (u4Reg & B2R_REG_HSTA_ADIF_M))
        {
            _VdpTestStatusChange(&rHdFieldLatchLog, (u4Reg & B2R_REG_HSTA_ADIF_M), "HD-FL");
        }

        if ((u4Loop & 0xFFFFF) == 0)
        {
            HAL_TIME_T rTime;
            HAL_TIME_T rDeltaTime;

            _VdpPrintfStatus(&rHdIntLog, "HD-INT");
            _VdpPrintfStatus(&rHdHsLog, "HD-HS");
            _VdpPrintfStatus(&rHdVsLog, "HD-VS");
            _VdpPrintfStatus(&rHdFieldLog, "HD-F");
            _VdpPrintfStatus(&rHdFieldLatchLog, "HD-FL");

            HAL_GetTime(&rTime);
            HAL_GetDeltaTime(&rDeltaTime, &rStartTime, &rTime);

            if (rDeltaTime.u4Seconds > 10)
            {
                return;
            }
        }
        u4Loop++;
    }

#endif

    B2R_REG_DB_W_M(B2R_REG_HFC, 4, B2R_REG_HFC_GXYM_M, B2R_REG_HFC_GXYM_S);
    B2R_REG_DB_W_B(B2R_REG_HFC, B2R_REG_HFC_EN_M, B2R_REG_HFC_EN_M);

    // Pattern
    B2R_REG_DB_W(B2R_REG_PGGC, 0x9FFF9FFF);
    B2R_REG_DB_W(B2R_REG_PGCC, 0xFFFFFFFF);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalDisablePattern.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void _VDP_HalDisablePattern (UCHAR ucVdpId)
#else
void _VDP_HalDisablePattern (void)
#endif
{
    B2R_REG_DB_W_M(B2R_REG_HFC, 0, B2R_REG_HFC_GXYM_M, B2R_REG_HFC_GXYM_S);
    B2R_REG_DB_W_B(B2R_REG_HFC, 0, B2R_REG_HFC_EN_M);
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_EnableStcCal
 */
//-----------------------------------------------------------------------------
void _VDP_HalStcSpeed(UINT8 u1B2rId,UINT32 u4Speed, BOOL fgDiv10)
{
    if(u1B2rId < B2R_NS)
    {
        _rB2rStc[u1B2rId].u4Speed = u4Speed;
        _rB2rStc[u1B2rId].fgDiv10 = fgDiv10;
    }
    else
    {
        ASSERT(0);
    }
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_EnableStcCal
 */
//-----------------------------------------------------------------------------
void _VDP_HalStcPeriod(UINT8 u1B2rId,UINT32 u4VSyncPeriod)
{
    if(u1B2rId < B2R_NS)
    {
        _rB2rStc[u1B2rId].u4VSyncPeriod = u4VSyncPeriod;
    }
    else
    {
        ASSERT(0);
    }
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_EnableStcCal
 */
//-----------------------------------------------------------------------------
void _VDP_HalUpdateStc(UINT8 u1B2rId,BOOL fgEnable)
{
    if(u1B2rId < B2R_NS)
    {
        _rB2rStc[u1B2rId].fgEnable = fgEnable;
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalGetUpdateStc
 */
//-----------------------------------------------------------------------------
BOOL _VDP_HalGetUpdateStc(UINT8 u1B2rId)
{
    if(u1B2rId < B2R_NS)
    {
        return _rB2rStc[u1B2rId].fgEnable;
    }
    else
    {
        ASSERT(0);
        return FALSE;
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetB2rStc
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetB2rStc(UINT8 u1B2rId,UINT32 u4B2rStc)
{
    if(u1B2rId < B2R_NS)
    {
        _rB2rStc[u1B2rId].u4B2rStcVal = u4B2rStc;
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalGetB2rStc
 */
//-----------------------------------------------------------------------------
UINT32 _VDP_HalGetB2rStc(UINT8 u1B2rId)
{
    if(u1B2rId < B2R_NS)
    {
        return _rB2rStc[u1B2rId].u4B2rStcVal;
    }
    else
    {
        ASSERT(0);
        return 0;
    }
}


//-----------------------------------------------------------------------------
/** Brief of VDP_DumpB2RRegister
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_DumpB2RRegister(UCHAR ucVdpId)
#else
void VDP_DumpB2RRegister(void)
#endif
{
    UCHAR u4RegMap[40];
    UINT32 u4Tmp = 0;
    UINT8 i, j, k, l;

    if (!_fgDumpRegister)
    {
        return;
    }


    i = 0;
    j = 0;
    k = 0;
    l = 0;

    u4Tmp = B2R_REG_R(i);
    sprintf((CHAR*)(u4RegMap), "%08X ", u4Tmp);

    i = 4;
    j = 1;
    k = 1;
    l = 8;

    do
    {
        u4Tmp = B2R_REG_R(i);
        if ((j + 1) % 4 == 0)
        {
            if ((l + k) < 40)
            {
                sprintf((CHAR*)(u4RegMap+l+k), "%08X", u4Tmp);
            }
            else
            {
                ASSERT((l + k) < 40);
            }
            LOG(1, "%s\n", u4RegMap);
            l = 0;
            k = 0;
        }
        else
        {
            if ((l + k) < 40)
            {
                sprintf((CHAR*)(u4RegMap+l+k), "%08X ", u4Tmp);
            }
            else
            {
                ASSERT((l + k) < 40);
            }
            l += 8;
            ++k;
        }

        i += 4;
        ++j;
    } while(i <= B2R_REG_OFFSET);

    LOG(1, "%s\n", u4RegMap);

}


//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetYCStart
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalGetB2rDispInfo(UCHAR ucVdpId,B2R_DISP_INFO_T* ptB2rInfo)
#else
void VDP_HalGetB2rDispInfo(B2R_DISP_INFO_T* ptB2rInfo)
#endif
{
    ptB2rInfo->u4YStart     = B2R_REG_R(B2R_REG_RHYS);
    ptB2rInfo->u4CStart     = B2R_REG_R(B2R_REG_RHCS);
    ptB2rInfo->u4Height     = ((B2R_REG_R(B2R_REG_HACT) & 0x7FF00000)>>20);
    ptB2rInfo->u4With       = ((B2R_REG_R(B2R_REG_HACT) & 0x3FF)<<1);
    ptB2rInfo->u4Pitch      = ((B2R_REG_R(B2R_REG_HPH) & 0x7F)<<4);
    ptB2rInfo->u1OutMode    = ((B2R_REG_R(B2R_REG_HMODE) & 0x1000)>>12);
    ptB2rInfo->u1BlockMode  = ((B2R_REG_R(B2R_REG_HMODE) & 0x3000000)>>24);

    LOG(1,"Y(0x%x) C(0x%x) Width(%d) Height(%d) Pitch(%d) OM(%d) BM(%d) \n",
        ptB2rInfo->u4YStart,
        ptB2rInfo->u4CStart,
        ptB2rInfo->u4With,
        ptB2rInfo->u4Height,
        ptB2rInfo->u4Pitch,
        ptB2rInfo->u1OutMode,
        ptB2rInfo->u1BlockMode
       );

    return;
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalSetTrickMode
 */
//-----------------------------------------------------------------------------
void VDP_HalSetTrickMode(UINT8 u1B2rId,UCHAR ucTrick)
{
    /* Trick Play at 20081001 */
    if(u1B2rId < B2R_NS)
    {
        _rB2rData[u1B2rId].ucTrickPlay = ucTrick;
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetTrickMode
 */
//-----------------------------------------------------------------------------
UCHAR VDP_HalGetTrickMode(UINT8 u1B2rId)
{
    /* Trick Play at 20081001 */
    if(u1B2rId < B2R_NS)
    {
        return _rB2rData[u1B2rId].ucTrickPlay;
    }
    else
    {
        ASSERT(0);
        return _rB2rData[B2R_1].ucTrickPlay;
    }
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalSetStepForward
 */
//-----------------------------------------------------------------------------
void VDP_HalSetStepForward(UINT8 u1B2rId,UCHAR ucSet)
{
    /* Step Forward at 20081030 */
    if(u1B2rId < B2R_NS)
    {
        _rB2rData[u1B2rId].ucStepForward = ucSet;
        LOG(1, "VDP_HalSetStepForward(%d)\n", _rB2rData[u1B2rId].ucStepForward);
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetStepForward
 */
//-----------------------------------------------------------------------------
UCHAR VDP_HalGetStepForward(UINT8 u1B2rId)
{
    /* Step Forward at 20081030 */
    if(u1B2rId < B2R_NS)
    {
        return _rB2rData[u1B2rId].ucStepForward;
    }
    else
    {
        ASSERT(0);
        return _rB2rData[B2R_1].ucStepForward;
    }
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalSetSlowForward
 */
//-----------------------------------------------------------------------------
void VDP_HalSetSlowForward(UINT8 u1B2rId,UCHAR ucSet)
{
    /* Slow Forward at 20090506 */
    if(u1B2rId < B2R_NS)
    {
        _rB2rData[u1B2rId].ucSlowForward = ucSet;
        LOG(1, "VDP_HalSetSlowForward(%d)\n", _rB2rData[u1B2rId].ucSlowForward);
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetSlowForward
 */
//-----------------------------------------------------------------------------
UCHAR VDP_HalGetSlowForward(UINT8 u1B2rId)
{
    /* Slow Forward at 20090506 */
    if(u1B2rId < B2R_NS)
    {
        return _rB2rData[u1B2rId].ucSlowForward;
    }
    else
    {
        ASSERT(0);
        return _rB2rData[B2R_1].ucSlowForward;
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of VDP_HalPowerOffB2R
 */
//-----------------------------------------------------------------------------
void VDP_HalPowerOffB2R(void)
{
#ifdef CC_SECOND_B2R_SUPPORT
    UCHAR ucVdpId = 0;
#endif
    /* Refer to _VDP_HalSetEnable disable part */
    B2R_REG_DB_W_B(B2R_REG_RHEN,
                   0,
                   (B2R_REG_RHEN_M|B2R_REG_RHEN_ADIE_M|B2R_REG_RHEN_VEIE_M));

#ifndef CC_SECOND_B2R_SUPPORT
    VDP_HalSetB2RPower(FALSE);
#endif

    B2R_REG_W_B(B2R_REG_SRST,
                (B2R_REG_SRST_HDRST_M | B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M),
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M|B2R_REG_SRST_DMEN_M));

    return;
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetB2RFifoStatus
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
UINT32 VDP_HalGetB2RFifoStatus(UCHAR ucVdpId)
{
#else
UINT32 VDP_HalGetB2RFifoStatus(void)
{
    UCHAR ucVdpId;

    ucVdpId = _B2R_TO_VDP(B2R_1);
#endif
    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return 0;
    }

    if (_prDbase)
    {
        return _prDbase->rVdp.au4UnderFlow[ucVdpId];
    }
    return 0;
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetB2RFifoStatus
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalResetB2RFifoStatus(UCHAR ucVdpId)
{
#else
void VDP_HalResetB2RFifoStatus(void)
{
    UCHAR ucVdpId;

    ucVdpId = _B2R_TO_VDP(B2R_1);
#endif
    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return;
    }

    if (_prDbase)
    {
        _prDbase->rVdp.au4UnderFlow[ucVdpId] = 0;
    }
    return;
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalTestB2RSdPath
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalTestB2RSdPath(void)
{
#if 0
    UINT32 u4ClockSetting;

    /* 1. Copy HD register to SD register */
    B2R_REG_W(B2R_REG_RHYS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHYS));   //000
    B2R_REG_W(B2R_REG_RHCS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHCS));   //004
    B2R_REG_W(B2R_REG_RMAP+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RMAP));   //008
    B2R_REG_W(B2R_REG_HFC+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HFC));     //018
    B2R_REG_W(B2R_REG_HACT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HACT));   //01C
    B2R_REG_W(B2R_REG_HHVT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HHVT));   //020
    B2R_REG_W(B2R_REG_HDER+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HDER));   //024
    B2R_REG_W(B2R_REG_HPH+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HPH));     //028
    B2R_REG_W(B2R_REG_HMODE+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HMODE)); //02C
    B2R_REG_W(B2R_REG_HITRG+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HITRG)); //030
    B2R_REG_W(B2R_REG_RHEN+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHEN));   //040

#ifdef CC_MT5395
    /* 2. Copy Clock setting */
    u4ClockSetting = (IO_REG32(CKGEN_BASE, 0x23C) & 0xFFFF);
    IO_REG32(CKGEN_BASE, 0x2e0) = u4ClockSetting;
    IO_REG32(CKGEN_BASE, 0x2e4) = IO_REG32(CKGEN_BASE, 0x240);
#endif

#if 0
    /* 3. Switch OMUX selection */
    IO_REG32(VIDEO_IN0_BASE, 0x7FC) = (IO_REG32(VIDEO_IN0_BASE, 0x7FC)&0xFFFFFF0F) + 0x90;
#endif

#endif
    return;
}
#else
void VDP_HalTestB2RSdPath(void)
{
    UINT32 u4ClockSetting;

    /* 1. Copy HD register to SD register */
    B2R_REG_W(B2R_REG_RHYS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHYS));   //000
    B2R_REG_W(B2R_REG_RHCS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHCS));   //004
    B2R_REG_W(B2R_REG_RMAP+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RMAP));   //008
    B2R_REG_W(B2R_REG_HFC+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HFC));     //018
    B2R_REG_W(B2R_REG_HACT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HACT));   //01C
    B2R_REG_W(B2R_REG_HHVT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HHVT));   //020
    B2R_REG_W(B2R_REG_HDER+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HDER));   //024
    B2R_REG_W(B2R_REG_HPH+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HPH));     //028
    B2R_REG_W(B2R_REG_HMODE+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HMODE)); //02C
    B2R_REG_W(B2R_REG_HITRG+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HITRG)); //030
    B2R_REG_W(B2R_REG_RHEN+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHEN));   //040

    /* 2. Copy Clock setting */
    u4ClockSetting = (IO_REG32(CKGEN_BASE, 0x23C) & 0xFFFF);
    IO_REG32(CKGEN_BASE, 0x244) = u4ClockSetting;
    IO_REG32(CKGEN_BASE, 0x248) = IO_REG32(CKGEN_BASE, 0x240);

    /* 3. Switch OMUX selection */
    IO_REG32(VIDEO_IN0_BASE, 0x7FC) = (IO_REG32(VIDEO_IN0_BASE, 0x7FC)&0xFFFFFF0F) + 0x90;
    return;
}
#endif
//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetYCStart
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalGetYCStart(UCHAR ucVdpId,UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
#else
void VDP_HalGetYCStart(UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
#endif
{

#ifndef CC_SECOND_B2R_SUPPORT
    UINT8 ucVdpId =0;
#endif

    *pu4YStart = B2R_REG_R(B2R_REG_RHYS);
    *pu4CStart = B2R_REG_R(B2R_REG_RHCS);
    *pu4LineSize = (B2R_REG_R(B2R_REG_HPH) & 0x7F)<<4;
    *pu4VActive = (B2R_REG_R(B2R_REG_HACT) & 0x7FF00000)>>20;

    if (_rB2rInfo[ucVdpId].u4OutInterlace != 0)
    {
        (*pu4VActive) <<= 1;
    }

    return;
}

UINT8 VDP_HalGetUnderFlowFlag(UCHAR ucVdpId)
{
    if(_prDbase)
    {
        if(_prDbase->rVdp.au4UnderFlow[ucVdpId] ==0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

VOID VDP_HalResetUnderFlowFlag(UCHAR ucVdpId)
{
    if(_prDbase)
    {
        _prDbase->rVdp.au4UnderFlow[ucVdpId] = 0;
    }
}

VOID VDP_HalSetRepeatForever(UCHAR ucVdpId,BOOL fgRepeatForever)
{
    UNUSED(ucVdpId);
    if(fgRepeatForever)
    {
        B2R_REG_DB_W_B(B2R_REG_RMAP, B2R_REG_RMAP_REP_FOREVER_M, B2R_REG_RMAP_REP_FOREVER_M);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_FOREVER_M);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);
    }
}

#ifdef CC_SECOND_B2R_SUPPORT
VOID VDP_HalSetB2RPower(UCHAR ucVdpId,BOOL fgPowerOn)
#else
VOID VDP_HalSetB2RPower(BOOL fgPowerOn)
#endif
{
    UNUSED(fgPowerOn);
#if defined(CC_MT5365)
    UINT32 u4B2RCLKBitMask = 0xFFFEFFFF;
    UINT32 u4EnableB2RClKBitMask =0x10000;
#elif defined(CC_MT5395)
    UINT32 u4B2RCLKBitMask = 0xFFFFFFFE;
    UINT32 u4EnableB2RClKBitMask =0x1;
#endif

#if defined(CC_MT5365) || defined(CC_MT5395)
    if(fgPowerOn)
    {
        IO_REG32(CKGEN_BASE, 0x278) = (IO_REG32(CKGEN_BASE, 0x278) & u4B2RCLKBitMask) + (u4EnableB2RClKBitMask);
    }
    else
    {
        IO_REG32(CKGEN_BASE, 0x278) = (IO_REG32(CKGEN_BASE, 0x278) & u4B2RCLKBitMask);
    }
#endif
}

VOID VDP_HalSetB2RMirrorFlip(UCHAR ucVdpId)
{
#if defined(CC_FLIP_MIRROR_SUPPORT)
    UINT32 u4MainFlipModule = u4GetFlipMirrorModule(VDP_1);
    UINT32 u4SubFlipModule = u4GetFlipMirrorModule(VDP_2);

    UINT8 ucTVEEnable;  // If TVEEnable then it is SCART out DTV
    TVE_GetEnable(TVE_1, &ucTVEEnable);

    // if M/F by MJC => B2R(X), others(X)
    // else if not M/F by MJC then follow the table
    // (X) means don't do Mirror/Flip
    // (O) means do Mirror/Flip
    // M means Main path
    // S means Sub path
    // Main    Sub     SCART       B2R   Others   Output
    // -------------------------------------------------
    // DTV     X       DTV      M:  O      X        O
    //                          S:  O      O        X
    // HDMI    X       DTV      M:  X      O        O
    //                          S:  O      O        X
    // DTV     HDMI    X        M:  O      X        O
    //                          S:  X      O        O
    // HDMI    DTV     X        M:  X      O        O
    //                          S:  X      O        O
    LOG(10, "u4MainFlipModule = %d, u4SubFlipModule = %d\n", u4MainFlipModule, u4SubFlipModule);

    switch (ucVdpId)
    {
    case VDP_1:
        if ((u4MainFlipModule == FLIP_BY_MJC)
#ifdef DRV_USE_EXTERNAL_3D_FRC // MT8283_FLIP_MIRROR
				||(u4MainFlipModule == FLIP_BY_EXT3DFRC)
				||(u4MainFlipModule == FLIP_NONE)
#endif
				)
        {
            _VdpDoMirrorFlip(FALSE);
        }
        else
        {
            if (u4MainFlipModule == FLIP_BY_B2R)
            {
                _VdpDoMirrorFlip(TRUE);
            }
        }
        break;
    case VDP_2:
        if ((u4SubFlipModule == FLIP_BY_MJC)
#ifdef DRV_USE_EXTERNAL_3D_FRC // MT8283_FLIP_MIRROR
				||(u4SubFlipModule == FLIP_BY_EXT3DFRC)
				||(u4SubFlipModule == FLIP_NONE)
#endif
				)
        {
            _VdpDoMirrorFlip(FALSE);
        }
        else
        {
            if (ucTVEEnable != 0)
            {
                _VdpDoMirrorFlip(TRUE);
            }
            else
            {
                if (u4SubFlipModule == FLIP_BY_B2R)
                {
                    _VdpDoMirrorFlip(TRUE);
                }
                else
                {
                    _VdpDoMirrorFlip(FALSE);
                }
            }
        }
        break;
    default:
        LOG(0, "VDP ID wrong, ucVdpId = %d\n", ucVdpId);
        ASSERT(0);
        break;
    }
#endif
}


#ifdef CC_SECOND_B2R_SUPPORT
VOID VDP_HalSet3DMode(UCHAR ucVdpId,UINT8 u1B2R3DMode)
#else
VOID VDP_HalSet3DMode(UINT8 u1B2R3DMode)
#endif
{
    B2R_REG_DB_W_M(B2R_REG_HMODE, u1B2R3DMode, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
}

#ifdef CC_SECOND_B2R_SUPPORT
VOID VDP_HalSetLRState(UCHAR ucVdpId,BOOL fgLView)
#else
VOID VDP_HalSetLRState(BOOL fgLView)
#endif
{
    if(fgLView)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_DISP_R_VIEW_M);
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_DISP_R_VIEW_M, B2R_REG_HMODE_DISP_R_VIEW_M);
    }
}

VOID VDP_EnableDumpRegister(BOOL fgOn)
{
    _fgDumpRegister = fgOn;
}

