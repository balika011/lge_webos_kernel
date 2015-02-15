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


// Send 1st Field Only
#define VDP_FRC_WA2

// for test only
//#define VDP_STATUS_TEST


// Use PSCAN interface to control AUTO STOP
#define VDP_FRC_AUTO_STOP_CTRL


// Use inverse field control to manual setup output field (currently used in 3:2, TargetNs = 3 case)
#define VDP_FRC_INV_FIELD_CTRL


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
#ifdef CC_MT5399
#define B2R_2160P_OUT_HEIGHT    2250
#endif

#define B2R_1440P_OUT_HEIGHT    1650

#define B2R_1080P_OUT_HEIGHT    1125

#define B2R_720P_1920W_OUT_HEIGHT   848

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

typedef enum
{
    VDP_FRC_FIELD_T = 0,
    VDP_FRC_FIELD_B,
    VDP_FRC_FIELD_MAX
} VDP_FRC_FIELD_ENUM_T;

enum
{
	MIRROR_ON,
	MIRROR_OFF,
	INVALID_MIRROR_STATUS
};

typedef struct
{
	UCHAR ucB2rId;
	UCHAR ucMirrorStatus;
}SKYPE_MIRROR_T;


/** Brief of VDP_FRC_T
 */
typedef struct
{
    UCHAR ucFrcEn;
    UINT16 u2TargetNs;
    UINT16 u2ChangeFieldNs;
    UINT16 u2CurrentNs;
    UCHAR ucMode;
    VDP_FRC_FIELD_ENUM_T eRepeatField;
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

    UINT32 u4Mode422;
    UINT32 u4ModeLinear;
    UINT32 u4FBAddrY;  // Recore the Frame Buffer Start Addr, before x/y shift.
    UINT32 u4FBAddrC;
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
#define B2R_FIELD_LOG(flag, ucB2rId)                            \
    B2R_REG_DB_W_B(B2R_REG_PCRT, B2R_REG_YCCRC_M, B2R_REG_YCCRC_M);\
    if (flag)                                                   \
    {LOG(0, "B2R(%d) CRC(%d), %s %s %s\n", ucB2rId,             \
            _u4B2RCrc[ucB2rId],                                 \
            (_u4CurrentLR[ucB2rId] != 0)? "R":"L",              \
            (_u4CurrentOutputField[ucB2rId] != 0)? "B1":"T1",   \
            (_u4CurrentField[ucB2rId] != 0)? "B2":"T2");} else  \
    {LOG(10, "B2R(%d) CRC(%d), %s %s %s\n", ucB2rId,            \
            _u4B2RCrc[ucB2rId],                                 \
            (_u4CurrentLR[ucB2rId] != 0)? "R":"L",              \
            (_u4CurrentOutputField[ucB2rId] != 0)? "B1":"T1",   \
            (_u4CurrentField[ucB2rId] != 0)? "B2":"T2");}


#define VERIFY_B2R_ID(id)                                   \
          do {                                              \
            if (id >= B2R_NS) { return; }  \
          } while (0)

#ifndef DIVIDE_WITH_CARRIER
#define DIVIDE_WITH_CARRIER(dividend, divisor)      (((dividend) + ((divisor) - 1)) / (divisor))
#endif

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

static void _B2rSetInterruptDelay(UCHAR ucB2rId);

static void _B2rReadStatus(UCHAR ucB2rId);
static void _VdpHandleFrc(UCHAR ucB2rId);

static void _B2rHdIsr(UINT16 u2Vector);

#if defined(CC_FLIP_MIRROR_SUPPORT)
static void _VdpDoMirrorFlip(UCHAR ucB2rId,BOOL fgDo);
#endif
static void _VDP_HalUpdateStc(UCHAR ucB2rId);

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UCHAR _aucB2rSwitch[VDP_NS] =
{
#ifdef FBM_VIRTUAL_MPEG
    B2R_1, B2R_2
#else
#if 1//def CC_SUPPORT_TVE
    B2R_NS, B2R_NS
#else
    B2R_1, B2R_2
#endif
#endif
};

static UCHAR _ucVdpSwitch = VDP_1;

static UINT32 _u4B2rActiveHeight[B2R_NS];
static UINT32 _u4B2rInsideActiveRegion[B2R_NS];

static VDP_FRC_T _rB2rFrc[B2R_NS];

static B2R_INFO_T _rB2rData[B2R_NS];

static VDP_INFO_T _rB2rInfo[B2R_NS];

static UCHAR _ucB2rIsrInitiated = 0;

static DRV_DBASE_T* _prDbase = NULL;

static IC_VERSION_T _eIcVersion = IC_VER_UNKNOWN;

static UINT32 _u4CurrentLineCount[B2R_NS];
static UINT32 _u4CurrentIsrStatus[B2R_NS];
static UINT32 _u4CurrentField[B2R_NS];
static UINT32 _u4LastField[B2R_NS];
static UINT32 _u4CurrentLR[B2R_NS];
static UINT32 _u4CurrentOutputLR[B2R_NS];
static UINT32 _u4CurrentOutputField[B2R_NS];
static UINT32 _u4CurrentUnderFlow[B2R_NS];

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

static UINT32 _u4B2RCrc[B2R_NS];


#ifdef MAIN_SUB_OPEN	
static UINT32 _u4B2RCrcSub;
static UINT32 _u4B2RCrcMain;
static UINT32 _u4CurrentOutputFieldSub;
static UINT32 _u4CurrentOutputFieldMain;
#endif

static BOOL _fgEnableB2RFieldLog = FALSE;


//for MM video master mode
static B2R_STC_T _rB2rStc[B2R_NS];

static BOOL _fgForceResetClk = FALSE;

static SKYPE_MIRROR_T _rSkypeMirror = {B2R_NS, INVALID_MIRROR_STATUS};
static BOOL _afgEnSmartPcr[B2R_NS] = {FALSE, FALSE};
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

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
static void _B2rSetInterruptDelay(UCHAR ucB2rId)
{
    INT32 i4BlankLine;
    UINT32 u4IntDelay;
    UINT32 u4SrcHeight;

    if(ucB2rId >= B2R_NS)
    {
        LOG(3,"warning , ucB2rId >= B2R_NS\n");
        ucB2rId = B2R_1;
    }

    u4SrcHeight = _rB2rInfo[ucB2rId].u4SrcHeight;

    // Set Interrupt Delay according to Output Format
    if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_480P_OUT_HEIGHT)
    {
        i4BlankLine = (525 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_576P_OUT_HEIGHT)
    {
        i4BlankLine = (625 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_720P_OUT_HEIGHT)
    {
        i4BlankLine = (750 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_720P_1920W_OUT_HEIGHT)
    {
        i4BlankLine = (B2R_720P_1920W_OUT_HEIGHT - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_1080P_OUT_HEIGHT)
    {
        i4BlankLine = (1125 - u4SrcHeight);
    }
#ifdef CC_MT5399    
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_1440P_OUT_HEIGHT)
    {
        i4BlankLine = (1650 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_2160P_OUT_HEIGHT)
    {
        i4BlankLine = (2250 - u4SrcHeight);
    }
#endif    
    else
    {
        i4BlankLine = 0;
    }

    if(_rB2rInfo[ucB2rId].u4OutHeight ==0)
    {
        i4BlankLine = 0;
    }
    if (i4BlankLine < 0)
    {
        i4BlankLine = 0;
    }

    // Note: Line Counter of 1st Active Line is 4
    u4IntDelay = DIVIDE_WITH_CARRIER((i4BlankLine + 4), 8);

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
    {
        u4IntDelay /= 2;
    }

    B2R_REG_DB_W_M(B2R_REG_HDER, u4IntDelay, B2R_REG_HDER_HINTD_M, B2R_REG_HDER_HINTD_S);
}

//-----------------------------------------------------------------------------
/** Brief of _B2rReadStatus.
 */
//-----------------------------------------------------------------------------
static void _B2rReadStatus(UCHAR ucB2rId)
{
    UINT32 u4Line1;
    UINT32 u4Line2;
#ifdef VDP_B2R_PSCAN_CRC_FLOW
    UINT32 u4CRC;
#endif

    if(ucB2rId >= B2R_NS)
    {
        LOG(1, "[%s-%d]ucB2rId(%d) error!.\n", __func__, __LINE__, ucB2rId);
        return;
    }

    do
    {
        // Line Counter Register does not handle cross-clock domain
        // Read twice to assure stable

        u4Line1 = B2R_REG_R_M(B2R_REG_HSTA, B2R_REG_HSTA_HTGL_M, B2R_REG_HSTA_HTGL_S);
        u4Line2 = B2R_REG_R_M(B2R_REG_HSTA, B2R_REG_HSTA_HTGL_M, B2R_REG_HSTA_HTGL_S);
    } while (u4Line1 != u4Line2);

    _u4B2RCrc[ucB2rId] = B2R_REG_R(B2R_REG_PIXEL_CRC);
    _u4CurrentLineCount[ucB2rId] = u4Line2;
    _u4CurrentIsrStatus[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
    _u4CurrentField[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_ADIF_M);
    _u4CurrentLR[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_LR_STATUS_M);
    _u4CurrentOutputLR[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HD_DISP_R_ADJI_M);
    _u4CurrentOutputField[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_HD_FLD_ADJ_M);
    _u4CurrentUnderFlow[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_RHFUDF_M);

#ifdef MAIN_SUB_OPEN	
_u4B2RCrcSub=  B2R_REG_R(B2R_REG_PIXEL_Sub_CRC);
_u4B2RCrcMain =  B2R_REG_R(B2R_REG_PIXEL_Main_CRC);
_u4CurrentOutputFieldSub = B2R_REG_R_B(B2R_REG_HSTA_Sub, B2R_REG_HSTA_HD_FLD_ADJ_M);
_u4CurrentOutputFieldMain= B2R_REG_R_B(B2R_REG_HSTA_Main, B2R_REG_HSTA_HD_FLD_ADJ_M);
#endif

#ifdef VDP_B2R_PSCAN_CRC_FLOW
    //u4DramIndex = B2R_REG_R(B2R_REG_RHYS);
    u4DRAMChkSum = B2R_REG_R(B2R_REG_DRAM_CHKSUM);
    u4PixelChkSum = B2R_REG_R(B2R_REG_PIXEL_CRC);

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399)
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

extern VDP_SEEK_PRM_T arVdpSeek[B2R_NS];

//-----------------------------------------------------------------------------
/** Brief of _B2rHdIsr.
 */
//-----------------------------------------------------------------------------
static void _B2rHdIsr(UINT16 u2Vector)
{

    UCHAR ucB2rId =0;
    UCHAR ucVdpId = VDP_NS; 
    BOOL fgHDISR = FALSE;
    BOOL fgHDISR2 = FALSE;
    BOOL fgSDISR = FALSE;
    BOOL fgSDISR2 = FALSE;

#ifdef CC_SUPPORT_TVE
    UCHAR bEnable;
#endif


    UNUSED(u2Vector);



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
            ucB2rId = B2R_1;
            _u4CurrentIsrStatus[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
            fgHDISR = FALSE;
        }
        else if (fgSDISR)
        {
            ucB2rId = B2R_2;
            _u4CurrentIsrStatus[ucB2rId] = B2R_REG_R_B(B2R_REG_HSTA, B2R_REG_HSTA_INT_STATUS_M);
            fgSDISR = FALSE;
        }
        
        ucVdpId  = VDP_GetVdpId(ucB2rId);

        _B2rReadStatus(ucB2rId);

        //clear underflow bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FMON_M);
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FMON_M, B2R_REG_HMODE_FMON_M);
        
        if ((ucB2rId >= B2R_NS) || (ucVdpId >= VDP_NS))
        {
            LOG(4, "ISR ignored as B2rId(%d) or VdpId(%d) invalid.\n", ucB2rId, ucVdpId);
            break;
        }


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
		B2R_FIELD_LOG(_fgEnableB2RFieldLog, ucB2rId);

        //LOG(10,"CRC = %d ucVdpId = %d\n",_u4B2RCrc[ucVdpId], ucVdpId);
        //LOG(10,"LINE = %d ucVdpId = %d\n", _u4CurrentLineCount[ucVdpId], ucVdpId);

        UNUSED(_u4B2RCrc[ucB2rId]);
        UNUSED(_u4CurrentField[ucB2rId]);
        UNUSED(_u4CurrentOutputField[ucB2rId]);
        UNUSED(_u4CurrentLR[ucB2rId]);
        UNUSED(_u4CurrentOutputLR[ucB2rId]);

#ifdef FBM_VIRTUAL_MPEG	
		if (_u4CurrentOutputField)
		{
			B2R_AutoTestSetCRC(CRC_INTERLACE_BOTTOM, _u4B2RCrc);
		}
		else
		{
			B2R_AutoTestSetCRC(CRC_INTERLACE_TOP, _u4B2RCrc);
		}
#ifdef MAIN_SUB_OPEN
		if (_u4CurrentOutputFieldSub)
		{
			B2R_AutoTestSetCRCSub(CRC_INTERLACE_BOTTOM, _u4B2RCrcSub);
		}
		else
		{
			B2R_AutoTestSetCRCSub(CRC_INTERLACE_TOP, _u4B2RCrcSub);
		}

		if (_u4CurrentOutputFieldMain)
		{
			B2R_AutoTestSetCRCMain(CRC_INTERLACE_BOTTOM, _u4B2RCrcMain);
		}
		else
		{
			B2R_AutoTestSetCRCMain(CRC_INTERLACE_TOP, _u4B2RCrcMain);
		}
#endif

#endif

#endif
        if (_u4CurrentIsrStatus[ucB2rId] & VDP_ISR_ADI)
        {
#ifdef VDP_FRC_LOG
            _szDebugBufB2r[_u4DebugIdx] = '0' + (_u4CurrentField[ucB2rId] != 0);
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

                if ((u4LineCount != (_prDbase->rVdp.au4StopLineCnt[ucB2rId] + 1)) &&
                        (u4LineCount != 0))
                {
                    _prDbase->rVdp.au4Tearing[ucB2rId]++;
                }

                _prDbase->rVdp.au4StopLineCnt[ucB2rId] = u4LineCount;

                _prDbase->rVdp.au4StartLineCnt[ucB2rId]++;
            }
#else
            if (_prDbase)
            {
                _prDbase->rVdp.au4StartLineCnt[ucB2rId] = _u4CurrentLineCount[ucB2rId];
                if (_u4CurrentUnderFlow[ucB2rId] != 0)
                {
                    _prDbase->rVdp.au4UnderFlow[ucB2rId]++;
                }
            }
#endif

            // do FRC here
            _rB2rFrc[ucB2rId].u2CurrentNs++;
            _VdpHandleFrc(ucB2rId);
        }
        else if (_u4CurrentIsrStatus[ucB2rId] & VDP_ISR_VEI)
        {
            // VEI is one-shot interrupt
            B2R_REG_DB_W_B(B2R_REG_RHEN,    0, B2R_REG_RHEN_VEIE_M);
        }

#ifdef CC_SUPPORT_TVE
        //HAR bEnable;
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
        _VDP_HalUpdateStc(ucB2rId);

        //    LOG(4, "ISR %d 0x%x 0x%x\n", _u4CurrentLineCount, _u4CurrentField, _u4CurrentIsrStatus);

        // Note: Hardware latch status, Top Field = 0, Bottom Field = 1,
        // Note: Current Field = Top, Next Field = Bottom
        // Note: Current Field = Bottom, Next Field = Top
        // Note: Since we are using double buffer, _VDP_VsyncTick need to prepare frame buffer for next field

        if (_u4LastField[ucB2rId] == _u4CurrentField[ucB2rId])
        {
            LOG(10, "VDP_HAL.c: Wrong field. ucB2rId = %d\n", ucB2rId);
        }
        _u4LastField[ucB2rId] = _u4CurrentField[ucB2rId];
        _VDP_VsyncTick(ucB2rId, (_u4CurrentField[ucB2rId] == 0),(_u4CurrentLR[ucB2rId] == 0), _u4CurrentIsrStatus[ucB2rId]);
    }
    // clear global isr
    /*if (!BIM_ClearIrq(VECTOR_DISPLAY))
    {
    }
    */
    
#if (defined(CC_MT5396) || defined(CC_MT5368)|| defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399))  && !defined(CC_DISABLE_TWO_B2R)
    if (fgHDISR2)
    {
#ifdef VDP_B2R_PSCAN_CRC_FLOW
        B2R_REG_W(B2R_REG_PCRT,0x16);
#else
        B2R_REG_W(B2R_REG_PCRT,0x10);
#endif
    }
    if (fgSDISR2)
    {
        //B2R_REG_W_B(B2R_REG_PCRT, B2R_REG_SDISR_M, B2R_REG_SDISR_M);
#ifdef VDP_B2R_PSCAN_CRC_FLOW
        B2R_REG_W(B2R_REG_PCRT,0x26);
#else
        B2R_REG_W(B2R_REG_PCRT,0x20);
#endif
    }
#endif

}

//-----------------------------------------------------------------------------
/** Brief of _VdpHandleFrc.
 */
//-----------------------------------------------------------------------------
static void _VdpHandleFrc(UCHAR ucB2rId)
{
    B2R_PRM_T* prB2rPrm = NULL;
    UCHAR      ucVdpId  = VDP_GetVdpId(ucB2rId);

    if ((ucB2rId >= B2R_NS) || (ucVdpId >= VDP_NS))
    {
        //ASSERT((ucB2rId < B2R_NS) && (ucVdpId < VDP_NS));
        LOG(0, "[%s-%d]B2rId(%d) VdpId(%d) invalid.\n", __func__, __LINE__, ucB2rId, ucVdpId);
        return;
    }

    //DTV0032281  for step set auto- step to false:

    if(prB2rPrm != NULL)
    {
        if((prB2rPrm->fgB2R3DEnable && _rB2rInfo[ucB2rId].u4OutInterlace == 0) || prB2rPrm->fgProgress2Interlace) // 3d progressive timing don't set FRC mode.
        {
            if (prB2rPrm->fgProgress2Interlace)
            {
                vDrvDIB2RFilmModeFreeze(ucVdpId,1);
            }
            return;
        }
    }

    if (_rB2rFrc[ucB2rId].ucFrcEn == 0)
    {
        return;
    }

    //if (_rB2rFrc.u2CurrentNs == 0)
    // DTV00093447 for di has do some change, b2r should send T/P by order, or will freeze
    if ( ( _rB2rFrc[ucB2rId].u2CurrentNs == 0 ) /* && ( VDP_FRC_TRICK_PLAY != _rB2rFrc[ucVdpId].ucMode  ) */)
    {
        FBM_PIC_HDR_T* prFbmPicHdr = NULL;
        B2R_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucB2rId);

        if(prB2rPrm != NULL)
        {
            prFbmPicHdr = FBM_GetFrameBufferPicHdr((prB2rPrm)->ucFbgId, (prB2rPrm)->ucFbId);
        }

#ifdef VDP_FRC_INV_FIELD_CTRL
        if (_rB2rFrc[ucB2rId].u2TargetNs != 3)
#endif
        {
            if(prFbmPicHdr != NULL)
            {
                if(prFbmPicHdr->fgTrickTopField)
                {
                    B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_NEW_MODE_M);
                }
                else
                {
                    B2R_REG_DB_W_B(B2R_REG_RMAP, B2R_REG_RMAP_REP_NEW_MODE_M, B2R_REG_RMAP_REP_NEW_MODE_M);
                }
            }
            else
            {
                B2R_REG_DB_W_B(B2R_REG_RMAP, B2R_REG_RMAP_REP_NEW_MODE_M, B2R_REG_RMAP_REP_NEW_MODE_M);
            }
            B2R_REG_DB_W_M(B2R_REG_RMAP, _rB2rFrc[ucB2rId].u2TargetNs, B2R_REG_RMAP_REP_CNT_M, B2R_REG_RMAP_REP_CNT_S);
        }

        /* H-sync shift half line must be set OFF or DDDS lock will make frame shake up and down */
        //B2R_REG_DB_W_B(B2R_REG_RMAP, B2R_REG_RMAP_REP_NEW_MODE_M, B2R_REG_RMAP_REP_NEW_MODE_M);
        B2R_REG_DB_W_B(B2R_REG_RMAP, B2R_REG_RMAP_REP_SHIFT_M, B2R_REG_RMAP_REP_SHIFT_M);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);


        if (_rB2rFrc[ucB2rId].eRepeatField == VDP_FRC_FIELD_MAX)
        {
            if (_u4CurrentOutputField[ucB2rId] != 0)
            {
                _rB2rFrc[ucB2rId].eRepeatField = VDP_FRC_FIELD_B;
            }
            else
            {
                _rB2rFrc[ucB2rId].eRepeatField = VDP_FRC_FIELD_T;
            }
        }

#ifdef VDP_FRC_AUTO_STOP_CTRL
        if(prB2rPrm != NULL)
        {
            if(prB2rPrm->fgB2R3DEnable)
            {
                vDrvDIFrameRepeat(ucVdpId, 0);
            }
            else
            {
                if(ucB2rId < B2R_NS)
                {
                    #if defined(CC_USE_DDI)
                    if (VDP_FRC_TRICK_PLAY == _rB2rFrc[ucB2rId].ucMode)
                    #else
                    if (( VDP_FRC_TRICK_PLAY == _rB2rFrc[ucB2rId].ucMode )||
                            (VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD))
                    #endif
                    {
                        vDrvDIFrameRepeat(ucVdpId, 0);
                        vDrvDIB2RFilmModeFreeze(ucVdpId,1);
                    }
                    else
                    {
                        vDrvDIFrameRepeat(ucVdpId, 1);
                        vDrvDIB2RFilmModeFreeze(ucVdpId,0);
                    }
                }
            }
        }
#endif
        LOG(8,"B2R FRC S(%d) \n", _rB2rFrc[ucB2rId].u2TargetNs);
    }

#ifdef VDP_FRC_INV_FIELD_CTRL
    if ((_rB2rFrc[ucB2rId].u2TargetNs == 3) && (_rB2rFrc[ucB2rId].u2CurrentNs != _rB2rFrc[ucB2rId].u2TargetNs))
    {
        VDP_FRC_FIELD_ENUM_T eCurrentField;
        if (_u4CurrentField[ucB2rId] != 0)
        {
            eCurrentField = VDP_FRC_FIELD_B;
        }
        else
        {
            eCurrentField = VDP_FRC_FIELD_T;
        }

        if (eCurrentField == _rB2rFrc[ucB2rId].eRepeatField)
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_HFI_M, B2R_REG_HMODE_HFI_M);
        }
        else
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);
        }
    }
#endif // VDP_FRC_INV_FIELD_CTRL 

    if (_rB2rFrc[ucB2rId].u2CurrentNs == _rB2rFrc[ucB2rId].u2TargetNs)
    {
        B2R_REG_DB_W_M(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_CNT_M, B2R_REG_RMAP_REP_CNT_S);
        B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_NEW_MODE_M);
        B2R_REG_DB_W_B(B2R_REG_RMAP, 0, B2R_REG_RMAP_REP_SHIFT_M);
        B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_REAP_SET_M, B2R_REG_HITRG_REAP_SET_M);
        vDrvDIB2RFilmModeFreeze(ucVdpId,1);
    }

    return;
}

#if defined(CC_FLIP_MIRROR_SUPPORT)
static void _VdpDoMirrorFlip(UCHAR ucB2rId,BOOL fgDo)
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
void _VDP_HalIsrFin(UCHAR ucB2rId)
{
    VERIFY_B2R_ID(ucB2rId);

    // check tearing
	_B2rReadStatus(ucB2rId);

#ifndef VDP_SCPOS_WPTR_COUNT
    if (_prDbase)
    {
        _prDbase->rVdp.au4StopLineCnt[ucB2rId] = _u4CurrentLineCount[ucB2rId];
        if (_u4CurrentLineCount[ucB2rId] > _u4B2rActiveHeight[ucB2rId])
        {
            _prDbase->rVdp.au4Tearing[ucB2rId]++;
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
        UCHAR ucB2rId;

        _ucB2rIsrInitiated = 1;
        _fgForceResetClk = FALSE;
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
        UNUSED(_rSkypeMirror);
        VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2rHdIsr, &pfnOldIsr) == OSR_OK);
        // disable double register buffer
        for(ucB2rId =0 ; ucB2rId< VDP_NS ; ucB2rId++)
        {

            B2R_REG_W_B(B2R_REG_RHEN, 0, B2R_REG_RHEN_VPTE_M);

            // disable 1st MB interrupt
            B2R_REG_W_B(B2R_REG_ROWC, 0, B2R_REG_ROWC_FIE_M);

            B2R_REG_DB_W_B(B2R_REG_BRHEN, 0, (B2R_REG_BRHEN_BRPE_M | B2R_REG_BRHEN_BRWP_M));

            // reset Pattern
            B2R_REG_W_B(B2R_REG_HFC, 0, B2R_REG_HFC_GXYM_M);
        }
        ucB2rId = 0;
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
            _rB2rFrc[u4B2rIdx].u2TargetNs = 0;
            _rB2rFrc[u4B2rIdx].u2ChangeFieldNs = 0;

            _rB2rFrc[u4B2rIdx].u2CurrentNs = 0;
            _rB2rFrc[u4B2rIdx].eRepeatField = VDP_FRC_FIELD_MAX;
        }
        // Init Format
        _VDP_HalSetFormat(B2R_1, 0, ADDR_MODE_64_32);//TBD
        _fgEnableB2RFieldLog = FALSE;
        _rSkypeMirror.ucB2rId = B2R_NS;
        _rSkypeMirror.ucMirrorStatus = INVALID_MIRROR_STATUS;
    }

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
    UCHAR ucB2rId ;
    for(ucB2rId =0 ; ucB2rId < VDP_NS ; ucB2rId++)
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
    ucB2rId = 0;

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
void _VDP_HalSetEnable(UCHAR ucB2rId, UCHAR ucEnable)
{
    UINT32 u4Reset;
    VERIFY_B2R_ID(ucB2rId);

#ifdef VDP_FRC_LOG
    _u4DebugIdx = 0;
#endif
    LOG(1, "B2R HW Status: %d -> %d.\n", _rB2rInfo[ucB2rId].u4En, ucEnable);
    if (_rB2rInfo[ucB2rId].u4En != ucEnable)
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

    _rB2rInfo[ucB2rId].u4En = ucEnable;

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
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
		// This delay should be paired with VTotal/VActive setting, it is done when set out size
		// So don't do it here ...
        //_B2rSetInterruptDelay(ucVdpId);
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



#ifdef VDP_B2R_PSCAN_CRC_FLOW
        B2R_REG_DB_W_B(B2R_REG_PCRT, 0, (B2R_REG_DRAM_CRC_EN_M|B2R_REG_YC_CRC_EN_M|B2R_REG_HDISR_M|B2R_REG_SDISR_M));
#endif
    }

    if (u4Reset)
    {
#ifndef CC_SECOND_B2R_SUPPORT
        B2R_REG_W_B(B2R_REG_SRST, 0, B2R_REG_SRST_DMRST_M);
#endif
    }

	_B2rReadStatus(ucB2rId);
    //B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_RHFINI_M, B2R_REG_HMODE_RHFINI_M);
    B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_RHFT_M, B2R_REG_HITRG_RHFT_M);
    if (ucB2rId < B2R_NS && _afgEnSmartPcr[ucB2rId])
    {
        B2R_HalEnableSmartPcr(ucB2rId, TRUE);
    }
}

void B2R_HalEnableSmartPcr(UCHAR ucB2rId, BOOL fgEn)
{
    _afgEnSmartPcr[ucB2rId] = fgEn;
    if (fgEn)
    {
        if (ucB2rId == B2R_1)
        {
            B2R_REG_DB_W_M(B2R_REG_SRST, 1, B2R_REG_SRST_PCREN_M, B2R_REG_SRST_PCREN_S);
            B2R_REG_W(B2R_REG_PCRERR, 0);
        }
        if (ucB2rId == B2R_2)
        {
            B2R_REG_DB_W_M(B2R_REG_SRST, 1, B2R_REG_SRST_SDPCREN_M, B2R_REG_SRST_SDPCREN_S);
            B2R_REG_W(B2R_REG_SDPCRERR, 0);
        }
    }
}
//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetChromaMode.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetChromaMode(UCHAR ucB2rId, UCHAR ucFrameEncode, UCHAR ucTff)
{
    VERIFY_B2R_ID(ucB2rId);

    _rB2rInfo[ucB2rId].u4FrameEncode = ucFrameEncode;
    _rB2rInfo[ucB2rId].u4Tff = ucTff;

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
    {
        // interlace sequence
        B2R_REG_DB_W_B(B2R_REG_HMODE,     B2R_REG_HMODE_RHI_M, B2R_REG_HMODE_RHI_M);

        // B2R C field encode flag is obsolete at 5381, always = field encode at B2R, MDDI will handle it
        B2R_REG_DB_W_B(B2R_REG_HMODE,     B2R_REG_HMODE_RHCIS_M, B2R_REG_HMODE_RHCIS_M);

#if 0
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
void _VDP_HalSetSrcSize(UCHAR ucB2rId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
{
    VERIFY_B2R_ID(ucB2rId);

    _rB2rInfo[ucB2rId].u4SrcWidth = u4Width;
    _rB2rInfo[ucB2rId].u4SrcHeight = u4Height;
    _rB2rInfo[ucB2rId].u4SrcPitch = u4Pitch;

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
    {
        _u4B2rActiveHeight[ucB2rId] = u4Height >> 1;

        _u4B2rInsideActiveRegion[ucB2rId] = ((u4Height * (100 - B2R_INSIDE_ACTIVE_REGION)) / 100) >> 1;
    }
    else
    {
        _u4B2rActiveHeight[ucB2rId] = u4Height;

        _u4B2rInsideActiveRegion[ucB2rId] = ((u4Height * (100 - B2R_INSIDE_ACTIVE_REGION)) / 100);
    }

    // B2R
#ifdef CC_MT5399
    if ( (u4Pitch >> 4) <= 0xFF)
    {
        B2R_REG_DB_W_M(B2R_REG_HPH, (u4Pitch >> 4), B2R_REG_HPH_RHPH_M, B2R_REG_HPH_RHPH_S);
    }
    else
    {
        UINT32 u4NewPitch = ((u4Pitch >> 4) & 0xFF) | 0X10000000;
        B2R_REG_DB_W_M(B2R_REG_HPH, u4NewPitch, B2R_REG_HPH_RHPH_M, B2R_REG_HPH_RHPH_S);
    }
#else
    B2R_REG_DB_W_M(B2R_REG_HPH, (u4Pitch >> 4), B2R_REG_HPH_RHPH_M, B2R_REG_HPH_RHPH_S);
#endif

#ifdef CC_SCALER_LR_SHIFT_WA
    u4Height++; // for scaler workaround
#endif

    B2R_REG_DB_W_M(B2R_REG_HACT, (u4Width >> 1), B2R_REG_HACT_RHAW_M, B2R_REG_HACT_RHAW_S);

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
    {
        B2R_REG_DB_W_M(B2R_REG_HACT, (u4Height >> 1), B2R_REG_HACT_HAH_M, B2R_REG_HACT_HAH_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_HACT, u4Height, B2R_REG_HACT_HAH_M, B2R_REG_HACT_HAH_S);
    }

#ifdef MAIN_SUB_OPEN	
    /* 1. Copy HD register to SD register */
    //B2R_REG_W(B2R_REG_RHYS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHYS));   //000
    //B2R_REG_W(B2R_REG_RHCS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHCS));   //004
    
    B2R_REG_W(B2R_REG_RMAP+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RMAP));   //008
    B2R_REG_W(B2R_REG_PORCH+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_PORCH)); //00c
    B2R_REG_W(B2R_REG_HFC+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HFC));     //018
    B2R_REG_W(B2R_REG_HACT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HACT));   //01C
    B2R_REG_W(B2R_REG_HHVT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HHVT));   //020
    B2R_REG_W(B2R_REG_HDER+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HDER));   //024
    B2R_REG_W(B2R_REG_HPH+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HPH));     //028
    B2R_REG_W(B2R_REG_HMODE+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HMODE)); //02C
    B2R_REG_W(B2R_REG_HITRG+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HITRG)); //030
    B2R_REG_W(B2R_REG_ROWC+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_ROWC)); //034
    B2R_REG_W(B2R_REG_HSTA+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HSTA)); //03c 
    B2R_REG_W(B2R_REG_RHEN+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHEN));   //040
	B2R_REG_W(B2R_REG_OFFSET+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_OFFSET));   //048
	B2R_REG_W(B2R_REG_YS_R_VIEW+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_YS_R_VIEW));   //050
	B2R_REG_W(B2R_REG_CS_R_VIEW+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_CS_R_VIEW));   //05c
	
#endif 

}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetOutSize.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetOutSize(UCHAR ucB2rId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Interlace, UINT32 u4DoubleClk, UINT32 u4ClkMode)
{
    static UINT32 _u4B2RClkMode = 255;
#ifdef CC_SECOND_B2R_SUPPORT
    static UINT32 Iu4B2RClkModeSdPath = 0xff;
#endif
    UCHAR      ucVdpId  = VDP_GetVdpId(ucB2rId);
    UINT32     u4VdoInReg = 0;

#ifdef CC_B2R_3D_SUPPROT
    B2R_PRM_T* prB2rPrm = NULL;
#endif
    B2R_PRM_T* prB2rPrmAnotherPath = NULL;
    BOOL fgResetDRAMClock = TRUE;

    if ((ucB2rId >= B2R_NS) || (ucVdpId >= VDP_NS))
    {
        ASSERT((ucB2rId < B2R_NS) && (ucVdpId < VDP_NS));
        return;
    }

    if ((u4Interlace != _rB2rInfo[ucB2rId].u4OutInterlace) ||
#ifdef CC_SECOND_B2R_SUPPORT
            (ucB2rId == B2R_1 && u4ClkMode != _u4B2RClkMode) ||
            (ucB2rId == B2R_2 && u4ClkMode != Iu4B2RClkModeSdPath))
#else
            (u4ClkMode != _u4B2RClkMode))
#endif
    {
        //_VDP_SetScposDelayUnMute(ucVdpId, 6, 1);
        _vDrvVideoSetMute(MUTE_MODULE_B2R, ucVdpId, 6, TRUE);
    }

    _rB2rInfo[ucB2rId].u4OutWidth = u4Width;
    _rB2rInfo[ucB2rId].u4OutHeight = u4Height;
    _rB2rInfo[ucB2rId].u4OutInterlace = u4Interlace;
    _rB2rInfo[ucB2rId].u4OutDoubleClk = u4DoubleClk;


    //setting VDOIN register
    //HD path
    u4VdoInReg = IO_REG32(0xf0022000, 0xbe0);
    u4VdoInReg = u4VdoInReg | 0x80000000;
    IO_REG32(0xf0022000, 0xbe0) = u4VdoInReg;

#ifdef CC_SECOND_B2R_SUPPORT
    //SD path
    if(ucB2rId == B2R_2)
    {
        u4VdoInReg = IO_REG32(0xf0022000, 0xbe4);
        u4VdoInReg = u4VdoInReg | 0x80000000;
        IO_REG32(0xf0022000, 0xbe4) = u4VdoInReg;
    }
#endif

#ifdef CC_B2R_3D_SUPPROT
    //set 3D relative register
    prB2rPrm = _VDP_GetVdpPrm(ucB2rId);

    if(prB2rPrm->fgB2R3DEnable && !(prB2rPrm->fgB2RForce2D))
    {
        if(prB2rPrm->u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL)
        {
            B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
            B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_S);
            B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
            B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
            B2R_REG_DB_W_M(B2R_REG_HMODE, 1, B2R_REG_HMODE_TWO_ADDR_EN_M, B2R_REG_HMODE_TWO_ADDR_EN_S);

            if(!prB2rPrm->ucProgressiveSeqFlag)
            {
                //B2R_REG_DB_W_M(B2R_REG_HMODE, 2, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
                B2R_REG_DB_W_M(B2R_REG_HMODE, 1, B2R_REG_HMODE_3D_INTERLACE_M, B2R_REG_HMODE_3D_INTERLACE_S);
            }
        }
        else if(prB2rPrm->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D || prB2rPrm->u1B2R3DType == B2R_3D_SIDE_BY_SIDE)
        {
            //enable auto switch mode
            B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_M);
            B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
            B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
            B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->u4B2R3DWidthOffset, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
        }
        else if(prB2rPrm->u1B2R3DType == B2R_3D_TOP_N_BOTTOM)
        {
            B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_M);
            B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
            B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
            B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->u4B2R3DHeightOffset, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
        }
    }
    else if(prB2rPrm->fgB2R3DEnable && prB2rPrm->fgB2RForce2D)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M);
        B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
        B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
        B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);

        if((prB2rPrm->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D || prB2rPrm->u1B2R3DType == B2R_3D_SIDE_BY_SIDE)
                && (prB2rPrm->u1B2RForce2DType == B2R_FORCE_2D_R_VIEW))
        {
            B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->u4B2R3DWidthOffset, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
        }
        else if((prB2rPrm->u1B2R3DType == B2R_3D_TOP_N_BOTTOM)
                &&(prB2rPrm->u1B2RForce2DType == B2R_FORCE_2D_R_VIEW))
        {
            B2R_REG_DB_W_M(B2R_REG_OFFSET, prB2rPrm->u4B2R3DHeightOffset, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
        }
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M);
        B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
        B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
        B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
        B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_3D_INTERLACE_M, B2R_REG_HMODE_3D_INTERLACE_S);
        B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_TWO_ADDR_EN_M, B2R_REG_HMODE_TWO_ADDR_EN_S);
    }

    if(prB2rPrm->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D)
    {
        //remove 2 tag line for real D
        B2R_REG_DB_W_M(B2R_REG_PORCH, 2, B2R_REG_PORCH_TAG_MODE_M, B2R_REG_PORCH_TAG_LINE_NUM_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_PORCH, 0, B2R_REG_PORCH_TAG_MODE_M, B2R_REG_PORCH_TAG_MODE_S);
    }
#endif
   #ifdef CC_SUPPORT_FULL_3D_PHOTO

    if(prB2rPrm->fg4K2K)
 {
     B2R_REG_DB_W_M(B2R_REG_HMODE, 3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
     B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_AUTO_OFFSET_M, B2R_REG_HMODE_AUTO_OFFSET_S);
     B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
     B2R_REG_DB_W_M(B2R_REG_OFFSET, 0, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
     B2R_REG_DB_W_M(B2R_REG_HMODE, 1, B2R_REG_HMODE_TWO_ADDR_EN_M, B2R_REG_HMODE_TWO_ADDR_EN_S);
 
 }

#endif
    //B2R_REG_W(B2R_REG_PORCH,0x10021);
    // Disable B2R DRAM Control
    prB2rPrmAnotherPath = _VDP_GetVdpPrm(1-ucB2rId);
    if (prB2rPrmAnotherPath && (prB2rPrmAnotherPath->ucReady == 1))
    {
        LOG(1, "VDP(%d) Ignore DRAM clock reset\n", ucB2rId);
        fgResetDRAMClock = FALSE;        
    }
    if (fgResetDRAMClock)
    {
        B2R_REG_W_B(B2R_REG_SRST, B2R_REG_SRST_DMRST_M, B2R_REG_SRST_DMRST_M);
    }

    if (_eIcVersion != IC_VER_FPGA)
    {
        if (_fgForceResetClk)
        {
            _u4B2RClkMode = 0xFF;
#ifdef CC_SECOND_B2R_SUPPORT            
            Iu4B2RClkModeSdPath = 0xFF;
#endif
            _fgForceResetClk = FALSE;
        }

#ifdef CC_SECOND_B2R_SUPPORT
        if((ucB2rId == B2R_1 && (u4ClkMode != _u4B2RClkMode)) ||
                (ucB2rId == B2R_2 && (u4ClkMode != Iu4B2RClkModeSdPath)))
        {
            LOG(3, "B2R CLK CHANGE %d >> %d \n", _u4B2RClkMode, u4ClkMode);
            if(ucB2rId == B2R_1)
            {
                _u4B2RClkMode = u4ClkMode;
            }
            else
            {
                Iu4B2RClkModeSdPath = u4ClkMode;
            }
#else
        if (_fgForceResetClk || u4ClkMode != _u4B2RClkMode)
        {
            LOG(3, "B2R CLK CHANGE %d >> %d \n", _u4B2RClkMode, u4ClkMode);
            _u4B2RClkMode = u4ClkMode;
			_fgForceResetClk = FALSE;
#endif

#ifdef MAIN_SUB_OPEN

 				IO_REG32(CKGEN_BASE, 0x228) = (0x1);
				IO_REG32(CKGEN_BASE, 0x230) = (0x1);
				
                LOG(3, "SYSPLL = 216 MHz\n");
				Printf("----SYSPLL = 216 MHz--TEST_TWO_PATH_CRC---\n");
                switch (u4ClkMode)//HD Path
                {  
                 case VDP_B2R_CLK_297_000:
				 	LOG(1,"=======4K2K  30 FPS==========\n");
				 	IO_REG32(CKGEN_BASE, 0x228) = (0x201);
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1024;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (1117 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20022F,(B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

				 case VDP_B2R_CLK_37_125:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (64 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0008, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (182 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (16 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }	   


                switch (u4ClkMode)//SD Path
                {
                 case VDP_B2R_CLK_297_000:
				 	LOG(1,"=======4K2K  30 FPS==========\n");
				 	IO_REG32(CKGEN_BASE, 0x228) = (0x201);
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1024;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (1117 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20022F,(B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

				case VDP_B2R_CLK_37_125:
				   IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;					// M
				   IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (64 << 16);		 // N
				   B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0xb0008, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
				   break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (182 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (16 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL + 0x24, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
#endif

#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399)
#ifdef CC_SECOND_B2R_SUPPORT
#ifndef CC_DISABLE_TWO_B2R
            if(ucB2rId == B2R_1)
#endif
#endif
            {
                //0xD23C[8] = 1, from syspll
                IO_REG32(CKGEN_BASE, 0x228) = (0x1);

                // SD clock, if RealD, the clock is hardwired connect to the clock of HD
                // It means the M, N of SD will be ignored.

                //IO_REG32(CKGEN_BASE, 0x2e0) = VERIFICATION_REALD ? (0x10001) : (0x1);

                LOG(3, "SYSPLL = 216 MHz\n");

                switch (_u4B2RClkMode)
                {
                 case VDP_B2R_CLK_297_000:
				 	LOG(1,"=======4K2K  30 FPS==========\n");
				 	IO_REG32(CKGEN_BASE, 0x228) = (0x201);
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1024;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (1117 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20022F,(B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_37_125:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (64 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0008, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (182 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (16 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_29_700:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (80 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb000a, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_49_500:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (48 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0006, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                    
                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (8 << 16);    // N
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
                IO_REG32(CKGEN_BASE, 0x230) = (0x1);

                LOG(3, "SYSPLL = 216 MHz\n");

                switch (Iu4B2RClkModeSdPath)
                {
                 case VDP_B2R_CLK_297_000:
				 	LOG(1,"=======4K2K  30 FPS==========\n");
				 	IO_REG32(CKGEN_BASE, 0x228) = (0x201);
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1024;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (1117 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20022F,(B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (364 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (32 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (182 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (16 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_37_125:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (64 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0008, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                    
                case VDP_B2R_CLK_29_700:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (80 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb000a, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                    
                case VDP_B2R_CLK_49_500:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (48 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0006, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8 << 16);    // N
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
            if(ucB2rId == B2R_1)
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
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (8000 << 16);    // N
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
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (8 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (8000 << 16);    // N
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

#if defined(CC_MT5368) || defined(CC_MT5389)
#ifdef CC_SECOND_B2R_SUPPORT
#ifndef CC_DISABLE_TWO_B2R
            if(ucB2rId == B2R_1)
#endif
#endif
            {
                //0xD23C[8] = 1, from syspll
                IO_REG32(CKGEN_BASE, 0x23c) = (0x1);

                // SD clock, if RealD, the clock is hardwired connect to the clock of HD
                // It means the M, N of SD will be ignored.
                IO_REG32(CKGEN_BASE, 0x2e0) = VERIFICATION_REALD ? (0x10001) : (0x1);

                LOG(3, "SYSPLL = 162 MHz\n");

                switch (_u4B2RClkMode)
                {
                case VDP_B2R_CLK_27_027:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1001;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x3e903e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_54_000:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (273 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (24 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 250;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (273 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (12 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x240) = (IO_REG32(CKGEN_BASE, 0x240) & 0x0000FFFF) + (6 << 16);    // N
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
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 2;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (6 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x40002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                case VDP_B2R_CLK_54_054:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 2002;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (8000 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x7D203e8, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_176:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 125;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (273 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f400b6, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_74_250:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (24 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0004, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_352:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 250;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (273 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x1f4005b, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_148_500:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 11;                   // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (12 << 16);        // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0xb0002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;

                case VDP_B2R_CLK_27_000:
                default:
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0xFFFF0000) + 1;                  // M
                    IO_REG32(CKGEN_BASE, 0x2e4) = (IO_REG32(CKGEN_BASE, 0x2e4) & 0x0000FFFF) + (6 << 16);    // N
                    B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                    break;
                }
            }
#endif
#endif

#endif

            B2R_REG_W_B(B2R_REG_PCRT, 0x1, (B2R_REG_PCRT_M|B2R_REG_HDISR_M|B2R_REG_SDISR_M));

            if (_rB2rInfo[ucB2rId].u4OutHeight > B2R_720P_OUT_HEIGHT)
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

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
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

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
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
    _B2rSetInterruptDelay(ucB2rId);

    // The following settings depends on interlace/progressive output mode

    // Consider frame/field encode when mode change
    _VDP_HalSetChromaMode(ucB2rId, _rB2rInfo[ucB2rId].u4FrameEncode, _rB2rInfo[ucB2rId].u4Tff);

    // Consider MDDi mode change
    _VDP_HalSetEnable(ucB2rId, _rB2rInfo[ucB2rId].u4En);
    // Consider Source Size
    _VDP_HalSetSrcSize(ucB2rId,
                       _rB2rInfo[ucB2rId].u4SrcWidth,
                       _rB2rInfo[ucB2rId].u4SrcHeight,
                       _rB2rInfo[ucB2rId].u4SrcPitch);
	
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalTriggerSetResolution.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetPicth(UCHAR ucB2rId, UINT32 u4Pitch)
{
    UNUSED(ucB2rId);

    B2R_REG_DB_W_M(B2R_REG_HPH, (u4Pitch >> 4), B2R_REG_HPH_RHPH_M, B2R_REG_HPH_RHPH_S);
    //LOG(2, "[%s-%d]VDP Seamless Pitch(%d)!!\n", __func__, __LINE__, u4Pitch);
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_HalTriggerSetResolution.
 */
//-----------------------------------------------------------------------------
void _VDP_HalTriggerSetResolution(UCHAR ucB2rId)
{
    VERIFY_B2R_ID(ucB2rId);
    
    B2R_REG_DB_W_M(B2R_REG_HACT, 1, B2R_REG_HACT_SR_M, B2R_REG_HACT_SR_S);
    LOG(2, "[%s-%d]VDP Seamless Trigger!!\n", __func__, __LINE__);
}

/** Brief of _VDP_HalSetBg.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetBg(UCHAR ucB2rId, UINT32 u4BgColor)
{
    VERIFY_B2R_ID(ucB2rId);

    B2R_REG_DB_W_M(B2R_REG_HFC, u4BgColor, B2R_REG_HFC_YCBCR_M, B2R_REG_HFC_YCBCR_S);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalForceBg.
 */
//-----------------------------------------------------------------------------
void _VDP_HalForceBg(UCHAR ucB2rId, UCHAR ucForce)
{
    UINT8 bType1;

    VERIFY_B2R_ID(ucB2rId);

    bType1 = bGetVideoDecType(VDP_1);

    LOG(5, "ForceBg : B2R(%d) En(%d)\n", ucB2rId, ucForce);
    if ((_aucB2rSwitch[VDP_1] != _aucB2rSwitch[VDP_2]) || 
        (bType1 == SV_VD_MPEGHD))
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
void _VDP_HalSetAddr(UCHAR ucB2rId, UINT32 u4AddrY, UINT32 u4AddrC)
{
#ifdef MAIN_SUB_OPEN	
//do nothing
#else
    VERIFY_B2R_ID(ucB2rId);

    if(u4AddrY != 0 && u4AddrC != 0)
    {
        _rB2rInfo[ucB2rId].u4FBAddrY = u4AddrY;
        _rB2rInfo[ucB2rId].u4FBAddrC = u4AddrC;
        B2R_REG_DB_W(B2R_REG_RHYS, u4AddrY);
        B2R_REG_DB_W(B2R_REG_RHCS, u4AddrC);
    }
#endif
}

void _VDP_HalSetRViewAddr(UCHAR ucB2rId, UINT32 u4AddrY, UINT32 u4AddrC)
{
    VERIFY_B2R_ID(ucB2rId);

    if(u4AddrY != 0 && u4AddrC != 0)
    {
        B2R_REG_DB_W(B2R_REG_YS_R_VIEW, u4AddrY);
        B2R_REG_DB_W(B2R_REG_CS_R_VIEW, u4AddrC);
    }
}

void _VDP_HalSetXYShift(UCHAR ucB2rId, INT32 i4XOffset, INT32 i4YOffset)
{
    UINT32  u4Pitch, u4AddrOffset;
    VERIFY_B2R_ID(ucB2rId);
    

    if (_rB2rInfo[ucB2rId].u4ModeLinear)
    {
        #if 0
        //HW H_offset set only work for Block mode.
        B2R_REG_DB_W_M(B2R_REG_OFFSET, i4XOffset, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
        B2R_REG_DB_W_M(B2R_REG_OFFSET, i4YOffset, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
        #else
        u4Pitch      = ((B2R_REG_R(B2R_REG_HPH) & 0x7F)<<4);
        u4AddrOffset = i4YOffset * u4Pitch + i4XOffset;
        
        B2R_REG_DB_W(B2R_REG_RHYS, _rB2rInfo[ucB2rId].u4FBAddrY + u4AddrOffset);
        B2R_REG_DB_W(B2R_REG_RHCS, _rB2rInfo[ucB2rId].u4FBAddrC + u4AddrOffset);
        #endif
        LOG(3, "Raster Set XY shift XY(%d, %d) Addr(%x, %x).\n", i4XOffset, i4YOffset, _rB2rInfo[ucB2rId].u4FBAddrY, _rB2rInfo[ucB2rId].u4FBAddrC);
    }
    else
    {
        LOG(1, "Non-Raster(%d) do not support XY shift Mode(%d).\n", _rB2rInfo[ucB2rId].u4ModeLinear, _rB2rInfo[ucB2rId].u4Mode422);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetFrc.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetFrc(UCHAR ucB2rId, UINT16 u2TargetNs, UINT16 u2ChangeFieldNs, UCHAR ucMode)
{
    B2R_PRM_T* prB2rPrm = _VDP_GetVdpPrm(ucB2rId);

    VERIFY_B2R_ID(ucB2rId);

    if (_rB2rInfo[ucB2rId].u4OutInterlace == 0)
    {
        // Progressive mode, no FRC required
        _rB2rFrc[ucB2rId].ucFrcEn = 0;
        _rB2rFrc[ucB2rId].eRepeatField = VDP_FRC_FIELD_MAX;
        return;
    }

    //IO_READ32(PSCAN_BASE, 0x13c) = (IO_READ32(PSCAN_BASE, 0x13c) & 0xFFFFFF);

    if (u2TargetNs < u2ChangeFieldNs)
    {
        ASSERT(u2TargetNs >= u2ChangeFieldNs);
        return;
    }

#ifdef VDP_FRC_LOG
    _szDebugBufTns[_u4DebugIdx] = '0' + u2TargetNs;
    _szDebugBufCns[_u4DebugIdx] = '0' + u2ChangeFieldNs;
    _szDebugBufSm[_u4DebugIdx] = '0' + ucMode;
#endif


#if 1
    //if (VDP_HalGetTrickMode(ucB2rId)==VDP_PLAY_TRICK)
    if(prB2rPrm != NULL)
    {
        if(_VDP_GetFrcStcSpeed(ucB2rId) != STC_SPEED_TYPE_FORWARD_2X
                && VDP_HalGetTrickMode(ucB2rId)==VDP_PLAY_TRICK
                && prB2rPrm->fgB2R3DEnable)
        {
            ucMode = VDP_FRC_TRICK_PLAY;
        }
    }
#endif
    if (ucMode == VDP_FRC_NONE)
    {
        // wrong field case

        _rB2rFrc[ucB2rId].ucFrcEn = 0;
        _rB2rFrc[ucB2rId].eRepeatField = VDP_FRC_FIELD_MAX;

        // Clean HFI bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);
    }
    else if ((ucMode == VDP_FRC_NORMAL) &&
             ((u2TargetNs == 0) || (u2TargetNs == 3) ||
              ((u2TargetNs == 2) && (u2ChangeFieldNs == 1)) ||
              ((u2TargetNs == 4) && (u2ChangeFieldNs == 2))))
    {
        // Normal: Top + Bottom (Single Field)

        _rB2rFrc[ucB2rId].ucFrcEn = 0;
        _rB2rFrc[ucB2rId].eRepeatField = VDP_FRC_FIELD_MAX;

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
        _rB2rFrc[ucB2rId].ucFrcEn = 0;
        _rB2rFrc[ucB2rId].eRepeatField = VDP_FRC_FIELD_MAX;

        // Clean HFI bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_HFI_M);

        return;
#else

        _rB2rFrc[ucB2rId].ucFrcEn = 1;

#ifdef VDP_FRC_LOG
        _u4DebugPrint = 1;
#endif

#ifdef VDP_FRC_TRAP
        if (u2TargetNs == 20)
        {
            _u4VdpTrap = 1;
        }
        else
        {
            _u4VdpTrap = 0;
        }
#endif

#ifdef VDP_FRC_WA2
        u2ChangeFieldNs = u2TargetNs;
#endif

        if (ucMode == VDP_FRC_STOP_AUTO)
        {
            //copy bottom field to top field WA
            //_VdpGfxCopyField(ucVdpId);
            u2ChangeFieldNs = u2TargetNs;
        }

        _rB2rFrc[ucB2rId].u2CurrentNs = 0;
        _rB2rFrc[ucB2rId].u2TargetNs = u2TargetNs;
        _rB2rFrc[ucB2rId].u2ChangeFieldNs = u2ChangeFieldNs;
        _rB2rFrc[ucB2rId].ucMode = ucMode;

        _VdpHandleFrc(ucB2rId);
#endif
    }


}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetFormat.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetFormat(UCHAR ucB2rId, UINT32 u4Mode422, UINT32 u4AddrMode)
{
    VERIFY_B2R_ID(ucB2rId);

    // 422 Mode vs Repeat C

    _rB2rInfo[ucB2rId].u4Mode422 = u4Mode422;
    _rB2rInfo[ucB2rId].u4ModeLinear = (u4AddrMode == ADDR_MODE_RASTER);

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

	B2R_REG_DB_W_M(B2R_REG_HMODE, u4AddrMode, B2R_REG_HMODE_ADDR_M, B2R_REG_HMODE_ADDR_S);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalConfig.
 */
//-----------------------------------------------------------------------------
void _VDP_HalConfig(UCHAR ucB2rId)
{
    UNUSED(ucB2rId);

    // status will clean after global isr clean
    _B2rReadStatus(ucB2rId);
    _prDbase->rVdp.au4UnderFlow[B2R_1] = _u4CurrentUnderFlow[ucB2rId];
}

/** Brief of _VDP_HalSetBlack.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetBlack(UCHAR ucB2rId, BOOL fgBlack)
{
    UNUSED(ucB2rId);

    LOG(0, "Error. Need config this for Each IC\n");
    UNUSED(fgBlack);
#if 0
    if (fgBlack)
    {
        B2R_REG_DB_W_M(B2R_REG_HFC, 0, B2R_REG_HFC_GXYM_M, B2R_REG_HFC_GXYM_S);
        B2R_REG_DB_W_M(B2R_REG_HFC, 0x108080, B2R_REG_HFC_YCBCR_M, B2R_REG_HFC_YCBCR_S);
        B2R_REG_DB_W_M(B2R_REG_HFC, 1, B2R_REG_HFC_EN_M, B2R_REG_HFC_EN_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_HFC, 0, B2R_REG_HFC_GXYM_M, B2R_REG_HFC_GXYM_S);
        B2R_REG_DB_W_M(B2R_REG_HFC, 0, B2R_REG_HFC_EN_M, B2R_REG_HFC_EN_S);
    }
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalInsideBlankRegion.
 */
//-----------------------------------------------------------------------------
UINT32 _VDP_HalInsideBlankRegion(UCHAR ucB2rId)
{
    ASSERT(ucB2rId < B2R_NS);

    _B2rReadStatus(ucB2rId);
    if (_u4CurrentLineCount[ucB2rId] < _u4B2rInsideActiveRegion[ucB2rId])
    {
        return 0;
    }
    else
    {
        return _u4CurrentLineCount[ucB2rId];
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

void VDP_EnableBurstRead(UCHAR ucB2rId, BOOL fgEn)
{
    if (ucB2rId >= B2R_NS)
    {
        return;
    }
    B2R_REG_DB_W_M(B2R_REG_HMODE, (fgEn == TRUE), B2R_REG_HMODE_BURST_M, B2R_REG_HMODE_BURST_S);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalB2rSwitch.
 */
//-----------------------------------------------------------------------------
void _VDP_HalB2rSwitch(UCHAR ucVdpId, UCHAR ucB2rId)
{
#if 0//ndef CC_SUPPORT_TVE   // US also go next code. 

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
        _VDP_HalSetEnable(ucB2rId, (UCHAR)(_rB2rInfo[ucB2rId].u4En));
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

    LOG(3, "B2R(%d) >> VDP(%d) Switch(%d, %d).\n", ucB2rId, ucVdpId, _aucB2rSwitch[ucVdpId], _aucB2rSwitch[1-ucVdpId]);

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
                //_VDP_CopyFrcPrm(ucVdpId);

                _ucVdpSwitch = (1 - ucVdpId);
#if 1
                // Copy VdpConf here.
                _prVdpConf[_ucVdpSwitch]->u4MpegHeight = _prVdpConf[ucVdpId]->u4MpegHeight;
                _prVdpConf[_ucVdpSwitch]->u4MpegWidth = _prVdpConf[ucVdpId]->u4MpegWidth;
                //_prVdpConf[_ucVdpSwitch]->rOutInfo = _prVdpConf[ucVdpId]->rOutInfo;
                x_memcpy(&_prVdpConf[_ucVdpSwitch]->rOutInfo, &_prVdpConf[ucVdpId]->rOutInfo, sizeof(VDP_OUT_INFO_T));
                x_memcpy(&_rB2rInfo[_ucVdpSwitch], &_rB2rInfo[ucVdpId], sizeof(VDP_INFO_T));
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
            //_VDP_CopyFrcPrm(1 - ucVdpId);
#if 1
            // Copy VdpConf here.
            _prVdpConf[ucVdpId]->u4MpegHeight = _prVdpConf[_ucVdpSwitch]->u4MpegHeight;
            _prVdpConf[ucVdpId]->u4MpegWidth = _prVdpConf[_ucVdpSwitch]->u4MpegWidth;
            //_prVdpConf[ucVdpId]->rOutInfo = _prVdpConf[_ucVdpSwitch]->rOutInfo;
            x_memcpy(&_prVdpConf[ucVdpId]->rOutInfo, &_prVdpConf[_ucVdpSwitch]->rOutInfo, sizeof(VDP_OUT_INFO_T));
            x_memcpy(&_rB2rInfo[ucVdpId], &_rB2rInfo[_ucVdpSwitch], sizeof(VDP_INFO_T));
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
            _VDP_HalSetEnable(ucB2rId, _rB2rInfo[ucB2rId].u4En);
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
void _VDP_HalStatusTest (UCHAR ucB2rId)
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
    B2R_REG_DB_W(B2R_REG_PGCC, 0x000FFFFF);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalDisablePattern.
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void _VDP_HalDisablePattern (UCHAR ucB2rId)
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
void _VDP_HalEnB2rUpdateStc(UINT8 u1B2rId,BOOL fgEnable)
{
    if(u1B2rId < B2R_NS)
    {
        _rB2rStc[u1B2rId].fgEnable = fgEnable;
    }
    else
    {
        LOG(3, "Failed to set B2R update STC\n");
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
        LOG(3, "Failed to get B2R update STC\n");
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

#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalsetMirror(UCHAR ucB2rId, BOOL fgOn)
#else
void VDP_HalsetMirror(BOOL fgOn)
#endif
{
	if (fgOn)
	{
		B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_MIRROR_M, B2R_REG_HMODE_MIRROR_M); 
	}
	else 
	{
		B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_MIRROR_M); 
	}
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetYCStart
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalGetB2rDispInfo(UCHAR ucB2rId,B2R_DISP_INFO_T* ptB2rInfo)
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

void _VDP_HalUpdateStc(UCHAR ucB2rId)
{
#ifdef CC_SECOND_B2R_SUPPORT
#ifdef ENABLE_MULTIMEDIA
    UCHAR ucEsId;
    VDEC_ES_INFO_T *prVdecEsInfo;
    UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);
    ucEsId = (ucVdpId < VDP_NS) ? _prVdpConf[ucVdpId]->ucInputPort[0] : MAX_ES_NS;
    prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;
#endif
#endif

#ifdef CC_SECOND_B2R_SUPPORT
#ifdef ENABLE_MULTIMEDIA
    if(!arVdpSeek[ucB2rId].fgABReach && prVdecEsInfo && ((prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_NETWORK_SKYPE) ||
		(prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_HW_DIRECT_PLAY)))
#else
	if(!arVdpSeek[ucB2rId].fgABReach)
#endif
#else
    if(!arVdpSeek[ucB2rId].fgABReach)
#endif
    {
#if 1

        /* Check whether b2r update stc or not */

        UCHAR ucAvSyncMode = 0xff;
        UCHAR ucStcSrc;
        UCHAR ucFbg;

        ucFbg = _VDP_GetFbg(ucB2rId);

        FBM_GetSyncStc(ucFbg, &ucAvSyncMode, &ucStcSrc);
#ifdef CC_53XX_SWDMX_V2
        if (STC_GetSyncMode(ucStcSrc) == AV_SYNC_MODE_VIDEO_MASTER && STC_IsStartedStc(ucStcSrc))
#else
        if (STC_GetSyncMode() == AV_SYNC_MODE_VIDEO_MASTER && STC_IsStartedStc())
#endif
        {
            _rB2rStc[ucB2rId].fgEnable = TRUE;  // Video update STC if video master and stc started
        }
        else
        {
            _rB2rStc[ucB2rId].fgEnable = FALSE;
        }

        if ( (VID_SYNC_MODE_NONE == ucAvSyncMode) && VDP_Is4To1024XSpeed(_rB2rStc[ucB2rId].u4Speed))
        {
            VDP_SetB2rStc(ucB2rId, _VDP_GetDispingPts(ucB2rId));
#ifdef CC_53XX_SWDMX_V2
            STC_SetStcValue(ucStcSrc, VDP_GetB2rStc(ucB2rId));
#else
            STC_SetStcValue(VDP_GetB2rStc(ucB2rId));
#endif

            LOG(5," Pts update STC(0x%x)\n", VDP_GetB2rStc(ucB2rId));
        }
        else if ((_rB2rStc[ucB2rId].fgEnable) && (ucStcSrc < STC_SRC_V2))
        {
            UINT32 u4Increment;
            UINT32 u4Diff;

#ifdef CC_53XX_SWDMX_V2
            _rB2rStc[ucB2rId].u4B2rStcVal = STC_GetStcValue(ucStcSrc);
#else
            _rB2rStc[ucB2rId].u4B2rStcVal = STC_GetStcValue();
#endif

            if (_rB2rStc[ucB2rId].fgDiv10)
            {
                u4Increment = ((_rB2rStc[ucB2rId].u4VSyncPeriod * _rB2rStc[ucB2rId].u4Speed) >> 10) / 10;
            }
            else
            {
                u4Increment = (_rB2rStc[ucB2rId].u4VSyncPeriod * _rB2rStc[ucB2rId].u4Speed) >> 10;
            }

            u4Diff = (0xffffffff - _rB2rStc[ucB2rId].u4B2rStcVal);

            if (u4Diff > u4Increment)
            {
                _rB2rStc[ucB2rId].u4B2rStcVal += u4Increment;
            }
            else
            {
                _rB2rStc[ucB2rId].u4B2rStcVal = (u4Increment - u4Diff);
            }
            LOG(5, "Speed update STC (0x%x)\n", _rB2rStc[ucB2rId].u4B2rStcVal);
#ifdef CC_53XX_SWDMX_V2
            STC_SetStcValue(ucStcSrc, _rB2rStc[ucB2rId].u4B2rStcVal);
#else
            STC_SetStcValue(_rB2rStc[ucB2rId].u4B2rStcVal);
#endif
        }
        else
        {
            _rB2rStc[ucB2rId].u4B2rStcVal = 0;
        }
#endif
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
#ifndef CC_MT5881
    UCHAR ucB2rId = 0;
#endif
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
UINT32 VDP_HalGetB2RFifoStatus(UCHAR ucB2rId)
{
#else
UINT32 VDP_HalGetB2RFifoStatus(void)
{
    UCHAR ucB2rId;

    ucB2rId = B2R_1;
#endif
    if (ucB2rId >= B2R_NS)
    {
        ASSERT(ucB2rId < B2R_NS);
        return 0;
    }

    if (_prDbase)
    {
        return _prDbase->rVdp.au4UnderFlow[ucB2rId];
    }
    return 0;
}

//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetB2RFifoStatus
 */
//-----------------------------------------------------------------------------
#ifdef CC_SECOND_B2R_SUPPORT
void VDP_HalResetB2RFifoStatus(UCHAR ucB2rId)
{
#else
void VDP_HalResetB2RFifoStatus(void)
{
    UCHAR ucB2rId;

    ucB2rId = B2R_1;
#endif
    if (ucB2rId >= B2R_NS)
    {
        ASSERT(ucB2rId < B2R_NS);
        return;
    }

    if (_prDbase)
    {
        _prDbase->rVdp.au4UnderFlow[ucB2rId] = 0;
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
    //UINT32 u4ClockSetting;
    UINT32 u4VdoInReg;

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
    u4VdoInReg = IO_REG32(0xf0022000, 0xbe4);
    u4VdoInReg = u4VdoInReg | 0x80000000;
    IO_REG32(0xf0022000, 0xbe4) = u4VdoInReg;

    IO_REG32(CKGEN_BASE, 0x2e0) = (0x1);
    IO_REG32(CKGEN_BASE, 0x2e4) = IO_REG32(CKGEN_BASE, 0x240);

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
void VDP_HalGetYCStart(UCHAR ucB2rId,UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
#else
void VDP_HalGetYCStart(UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
#endif
{

#ifndef CC_SECOND_B2R_SUPPORT
    UINT8 ucB2rId =0;
#endif

    *pu4YStart = B2R_REG_R(B2R_REG_RHYS);
    *pu4CStart = B2R_REG_R(B2R_REG_RHCS);
    *pu4LineSize = (B2R_REG_R(B2R_REG_HPH) & 0x7F)<<4;
    *pu4VActive = (B2R_REG_R(B2R_REG_HACT) & 0x7FF00000)>>20;

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
    {
        (*pu4VActive) <<= 1;
    }

    return;
}

#ifdef CC_SECOND_B2R_SUPPORT
VOID VDP_HalSetB2RPower(UCHAR ucB2rId,BOOL fgPowerOn)
#else
VOID VDP_HalSetB2RPower(BOOL fgPowerOn)
#endif
{
#if defined(CC_MT5396) || defined(CC_MT5368) //|| defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)
    //UINT32 u4B2RCLKBitMask = 0xFFFFFFFE;
    //UINT32 u4EnableB2RClKBitMask =0x1;

	UINT32 u4B2RCLKBitMask = 0xFFFFFFFD;
    UINT32 u4EnableB2RClKBitMask =0x2;

#elif defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) ||defined(CC_MT5881) || defined(CC_MT5399)
    UINT32 u4B2RCLKBitMask = 0xFFFFFF7F;
    UINT32 u4EnableB2RClKBitMask =0x80;
	
#endif

    UNUSED(fgPowerOn);
    LOG(0, "fgPowerOn = %d\n", fgPowerOn);
#if defined(CC_MT5396) || defined(CC_MT5368) //|| defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)
    if(fgPowerOn)
    {
        IO_REG32(CKGEN_BASE, 0x278) = (IO_REG32(CKGEN_BASE, 0x278) & u4B2RCLKBitMask) + (u4EnableB2RClKBitMask);
    }
    else
    {
        IO_REG32(CKGEN_BASE, 0x278) = (IO_REG32(CKGEN_BASE, 0x278) & u4B2RCLKBitMask);
    }

#elif defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399)
    if(fgPowerOn)
    {
        IO_REG32(CKGEN_BASE, 0x1c8) = (IO_REG32(CKGEN_BASE, 0x1c8) & u4B2RCLKBitMask) + (u4EnableB2RClKBitMask);
    }
    else
    {
        IO_REG32(CKGEN_BASE, 0x1c0) = (IO_REG32(CKGEN_BASE, 0x1c0) & u4B2RCLKBitMask);
        IO_REG32(CKGEN_BASE, 0x1c0) = (IO_REG32(CKGEN_BASE, 0x1c0) & u4B2RCLKBitMask);
        IO_REG32(CKGEN_BASE, 0x1c0) = (IO_REG32(CKGEN_BASE, 0x1c0) & u4B2RCLKBitMask)+(u4EnableB2RClKBitMask);

        IO_REG32(CKGEN_BASE, 0x1c8) = (IO_REG32(CKGEN_BASE, 0x1c8) & u4B2RCLKBitMask);
    }	
#endif
}

void VDP_SetMirrorStatus(UCHAR ucB2rId, BOOL fgOn)
{
    _rSkypeMirror.ucB2rId = ucB2rId;
    _rSkypeMirror.ucMirrorStatus = (fgOn ? MIRROR_ON : MIRROR_OFF);
}

VOID VDP_HalSetB2RMirrorFlip(UCHAR ucB2rId)
{
#if defined(CC_FLIP_MIRROR_SUPPORT)
    UINT32 u4MainFlipModule = u4GetFlipMirrorModule(VDP_1);
    UINT32 u4SubFlipModule = u4GetFlipMirrorModule(VDP_2);
    UCHAR aucVdpIds[VDP_NS];
    UCHAR ucCnt;
    UCHAR ucVdpId1, ucVdpId2;

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
    ucCnt = VDP_GetVdpIds(ucB2rId, aucVdpIds);    
    switch (ucCnt)
    {
    case 1:
        ucVdpId1 = aucVdpIds[0];
        if (ucVdpId1 == VDP_1)
        {
            if ((u4MainFlipModule == FLIP_BY_MJC)
#ifdef DRV_USE_EXTERNAL_3D_FRC // MT8283_FLIP_MIRROR
				||(u4MainFlipModule == FLIP_BY_EXT3DFRC)
				||(u4MainFlipModule == FLIP_NONE)
#endif
                )
            {
                _VdpDoMirrorFlip(ucB2rId,FALSE);
            }
            else
            {
                if (u4MainFlipModule & FLIP_BY_B2R)
                {
                    _VdpDoMirrorFlip(ucB2rId,TRUE);
                }
                else
                {
                    _VdpDoMirrorFlip(ucB2rId,FALSE);
                }
            }
        }
        if (ucVdpId1 == VDP_2)
        {
            if ((u4SubFlipModule == FLIP_BY_MJC)
#ifdef DRV_USE_EXTERNAL_3D_FRC // MT8283_FLIP_MIRROR
                ||(u4SubFlipModule == FLIP_BY_EXT3DFRC)
                ||(u4SubFlipModule == FLIP_NONE)
#endif
                )
            {
                _VdpDoMirrorFlip(ucB2rId,FALSE);
            }
            else
            {
                if (u4SubFlipModule & FLIP_BY_B2R)
                {
                    _VdpDoMirrorFlip(ucB2rId,TRUE);
                }
                else
                {
                    _VdpDoMirrorFlip(ucB2rId,FALSE);
                }
            }
        }
        break;
    case 2:
        ucVdpId1 = aucVdpIds[0];
        ucVdpId2 = aucVdpIds[1];
        if (ucVdpId1 == VDP_1 && ucVdpId2 == VDP_2)
        {
            if (((u4MainFlipModule == FLIP_BY_MJC)
#ifdef DRV_USE_EXTERNAL_3D_FRC // MT8283_FLIP_MIRROR
				||(u4MainFlipModule == FLIP_BY_EXT3DFRC)
				||(u4MainFlipModule == FLIP_NONE)
#endif
                ) &&
                ((u4SubFlipModule == FLIP_BY_MJC)
#ifdef DRV_USE_EXTERNAL_3D_FRC // MT8283_FLIP_MIRROR
                ||(u4SubFlipModule == FLIP_BY_EXT3DFRC)
                ||(u4SubFlipModule == FLIP_NONE)
#endif
            ))
            {
                _VdpDoMirrorFlip(ucB2rId,FALSE);
            }
            else
            {
                if (u4MainFlipModule & FLIP_BY_B2R || u4SubFlipModule & FLIP_BY_B2R)
                {
                    _VdpDoMirrorFlip(ucB2rId,TRUE);
                }
                else
                {
                    _VdpDoMirrorFlip(ucB2rId,FALSE);
                }
            }
        }
        break;
    default:
        break;
    }
#ifdef CC_SECOND_B2R_SUPPORT
    if (_rSkypeMirror.ucB2rId == B2R_1 || _rSkypeMirror.ucB2rId == B2R_2 || 
        _rSkypeMirror.ucMirrorStatus == MIRROR_ON || _rSkypeMirror.ucMirrorStatus == MIRROR_OFF)
    {
        UCHAR ucB2rId = _rSkypeMirror.ucB2rId;
        BOOL fgOn = (_rSkypeMirror.ucMirrorStatus == MIRROR_ON ? TRUE : FALSE);
        VDP_HalsetMirror(ucB2rId, fgOn);
    }
#endif
#endif
}


#ifdef CC_SECOND_B2R_SUPPORT
VOID VDP_HalSetLRState(UCHAR ucB2rId,BOOL fgLView)
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

#ifdef CC_SECOND_B2R_SUPPORT
VOID VDP_HalSetTBState(UCHAR ucB2rId,BOOL fgTopField)
#else
VOID VDP_HalSetTBState(BOOL fgTopField)
#endif
{
    if(fgTopField)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_RHFINI_M);
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_RHFINI_M, B2R_REG_HMODE_RHFINI_M);
    }
    B2R_REG_DB_W_B(B2R_REG_HITRG, B2R_REG_HITRG_RHFT_M, B2R_REG_HITRG_RHFT_M);
}

VOID VDP_EnableDumpRegister(BOOL fgOn)
{
    //_fgDumpRegister = fgOn;
}

VOID VDP_EnableB2RFieldLog(BOOL fgOn)
{
	_fgEnableB2RFieldLog = fgOn;
}

VOID VDP_ForceResetClk(BOOL fgDo)
{
	_fgForceResetClk = fgDo;
}

#ifdef FBM_VIRTUAL_MPEG
UINT32 VDP_HalDoB2RAutoTestCRCReadSetting(UCHAR ucType, UINT32 u4Value)
{
    UINT32 u4Ret = 0;
    switch (ucType)
    {
    case H_OFFSET:
    	B2R_REG_W_M(B2R_REG_OFFSET, u4Value, B2R_REG_HOFFSET_WIDTH_M, B2R_REG_HOFFSET_WIDTH_S);
    	break;
    case V_OFFSET:
    	B2R_REG_W_M(B2R_REG_OFFSET, u4Value, B2R_REG_HOFFSET_HEIGHT_M, B2R_REG_HOFFSET_HEIGHT_S);
    	break;
    case PICTURE_FLIP:
    	B2R_REG_W_M(B2R_REG_HMODE, u4Value, B2R_REG_HMODE_FLIP_M, B2R_REG_HMODE_FLIP_S);
    	break;
    case PICTURE_MIRROR:
    	B2R_REG_W_M(B2R_REG_HMODE, u4Value, B2R_REG_HMODE_MIRROR_M, B2R_REG_HMODE_MIRROR_S);
    	break;
    case TWO_ADDR:
    	B2R_REG_W_M(B2R_REG_HMODE, u4Value, B2R_REG_HMODE_TWO_ADDR_EN_M, B2R_REG_HMODE_TWO_ADDR_EN_S);
    	break;
    	
#ifndef B2R_SD_TEST
    case TWO_ADDR_Y_ADDR:
    	B2R_REG_W_M(B2R_REG_YS_R_VIEW, u4Value / 16, 0x7FFFFFF0, 4);
    	break;
    case TWO_ADDR_C_ADDR:
    	B2R_REG_W_M(B2R_REG_CS_R_VIEW, u4Value / 16, 0x7FFFFFF0, 4);
    	break;
#endif 
    	
    case PICTURE_TWO_ADDR_VIEW:
    	B2R_REG_W_M(B2R_REG_HMODE, 0x3, B2R_REG_HMODE_DISP_R_VIEW_MODE_M, B2R_REG_HMODE_DISP_R_VIEW_MODE_S);
    	B2R_REG_W_M(B2R_REG_HMODE, u4Value, B2R_REG_HMODE_DISP_R_VIEW_M, B2R_REG_HMODE_DISP_R_VIEW_S);
    	break;
    case PICTURE_ADDR_MODE:
    	B2R_REG_W_M(B2R_REG_HMODE, u4Value, B2R_REG_HMODE_ADDR_M, B2R_REG_HMODE_ADDR_S);
    	break;
    case CRC_READING_ENABLE:
    	B2R_REG_W_M(B2R_REG_PCRT, u4Value, (B2R_REG_YCCRC_M|B2R_REG_HDISR_M|B2R_REG_SDISR_M), B2R_REG_YCCRC_S);
    	break;
    case CLEAR_PICTURE_PITCH:
    	B2R_REG_W_M(B2R_REG_HPH, 0, 0x000000FF, 0);
    	break;
    default:
    	break;
    }
#ifdef MAIN_SUB_OPEN	
    /* 1. Copy HD register to SD register */
    //B2R_REG_W(B2R_REG_RHYS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHYS));   //000
    //B2R_REG_W(B2R_REG_RHCS+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHCS));   //004
    
    B2R_REG_W(B2R_REG_RMAP+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RMAP));   //008
    B2R_REG_W(B2R_REG_PORCH+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_PORCH)); //00c
    B2R_REG_W(B2R_REG_HFC+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HFC));     //018
    B2R_REG_W(B2R_REG_HACT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HACT));   //01C
    B2R_REG_W(B2R_REG_HHVT+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HHVT));   //020
    B2R_REG_W(B2R_REG_HDER+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HDER));   //024
    B2R_REG_W(B2R_REG_HPH+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HPH));     //028
    B2R_REG_W(B2R_REG_HMODE+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HMODE)); //02C
    B2R_REG_W(B2R_REG_HITRG+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HITRG)); //030
    B2R_REG_W(B2R_REG_ROWC+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_ROWC)); //034
    B2R_REG_W(B2R_REG_HSTA+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_HSTA)); //03c 
    B2R_REG_W(B2R_REG_RHEN+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_RHEN));   //040
	B2R_REG_W(B2R_REG_OFFSET+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_OFFSET));   //048
	B2R_REG_W(B2R_REG_YS_R_VIEW+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_YS_R_VIEW));   //050
	B2R_REG_W(B2R_REG_CS_R_VIEW+B2R_SD_PATH_ADDR_OFFSET, B2R_REG_R(B2R_REG_CS_R_VIEW));   //05c
	
#endif 

    return u4Ret;
}
#endif


