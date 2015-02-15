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
#include "vdec_drvif.h"
#include "tve_if.h"
#include "mute_if.h"
#include "../tve/tve_hal.h"
#ifdef CC_FLIP_MIRROR_SUPPORT
#include "drv_display.h"
#endif
#include "b2r_hal.h"
#include "b2r_reg_v1.h"
#include "b2r_reg_v2.h"

#include "x_lint.h"
//LINT_EXT_HEADER_BEGIN
//#include "drv_dbase.h"
#include "x_ckgen.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_assert.h"
#include "vdo_misc.h"

//LINT_EXT_HEADER_END

//LINT_SUPPRESS_BRACE(717)        // Info 717: do ... while(0);
//LINT_SUPPRESS_BRACE(572)        // Warning 572: Excessive shift value (precision 1 shifted


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

#define B2R_2160P_OUT_HEIGHT    2250

#define B2R_1440P_OUT_HEIGHT    1650

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
static BOOL _afgEnSmartPcr[B2R_NS] = {FALSE, FALSE};

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef enum
{
    VDP_FRC_FIELD_T = 0,
    VDP_FRC_FIELD_B,
    VDP_FRC_FIELD_MAX
} VDP_FRC_FIELD_ENUM_T;


/** Brief of VDP_FRC_T
 */
typedef struct
{
    UCHAR ucFrcEn;
    UCHAR u2TargetNs;
    UCHAR u2ChangeFieldNs;
    UCHAR ucCurrentNs;
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
} VDP_INFO_T;


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
static void _B2rSetInterruptDelay(UCHAR ucB2rId);

static void _B2rReadStatus(UCHAR ucB2rId);
static void _VdpHandleFrc(UCHAR ucB2rId);

void _B2rHdIsr(UINT16 u2Vector);


void _VDP_HalSetFormat(UCHAR ucB2rId, UINT32 u4Mode422, UINT32 u4AddrMode);
VOID VDP_HalSetB2RPower(UCHAR ucB2rId,BOOL fgPowerOn);

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UCHAR _aucB2rSwitch[VDP_NS] =
{
#if 1//def CC_SUPPORT_TVE
    B2R_NS, B2R_NS
#else
    B2R_1, B2R_2
#endif
};

static UCHAR _ucVdpSwitch = VDP_1;


static UINT32 _u4B2rActiveHeight[B2R_NS];
static UINT32 _u4B2rInsideActiveRegion[B2R_NS];

static VDP_FRC_T _rB2rFrc[B2R_NS];

static VDP_INFO_T _rB2rInfo[B2R_NS];

static UCHAR _ucB2rIsrInitiated = 0;

static UINT32 _u4CurrentLineCount[B2R_NS];
static UINT32 _u4CurrentIsrStatus[B2R_NS];
static UINT32 _u4CurrentField[B2R_NS];
static UINT32 _u4LastField[B2R_NS];
static UINT32 _u4CurrentLR[B2R_NS];
static UINT32 _u4CurrentOutputLR[B2R_NS];
static UINT32 _u4CurrentOutputField[B2R_NS];
static UINT32 _u4CurrentUnderFlow[B2R_NS];
static UINT32 _u4B2RCrc[B2R_NS];

static VDP_CFG_T*  _prVdpCfg[VDP_NS];

static BOOL _fgEnableB2RFieldLog = FALSE;

static BOOL _fgForceResetClk = FALSE;


static VOID *_b2rObject[B2R_NS] = {0, 0};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/** Brief of _B2rReadStatus.
 */
//-----------------------------------------------------------------------------
static void _B2rReadStatus(UCHAR ucB2rId)
{
    UINT32 u4Line1;
    UINT32 u4Line2;

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


}

//-----------------------------------------------------------------------------
/** Brief of _B2rHdIsr.
 */
//-----------------------------------------------------------------------------
void _B2r_HAL_SetObject(UCHAR ucB2rId, VOID *pvData)
{
    
    if (ucB2rId >= B2R_NS)
    {
        return;
    }
    _b2rObject[ucB2rId] = pvData;
}

void _B2rHdIsrCb(UCHAR ucB2rId)
{
    UCHAR ucVdpId = VDP_NS; 

    ucVdpId  = _B2R_GetVdpId(ucB2rId);
    
    if ((ucB2rId >= B2R_NS) || (ucVdpId >= VDP_NS))
    {
        LOG(4, "ISR ignored as B2rId(%d) or VdpId(%d) invalid.\n", ucB2rId, ucVdpId);
        return;
    }

    //just for test one b2r
    if (ucB2rId != B2R_1)
    {
        return;
    }

    // For PCR error verification
    B2R_FIELD_LOG(_fgEnableB2RFieldLog, ucB2rId);


    if (_u4CurrentIsrStatus[ucB2rId] & VDP_ISR_ADI)
    {

        //_prDbase->rVdp.au4StartLineCnt[ucB2rId] = _u4CurrentLineCount[ucB2rId];
        _B2R_HalSetStartLineCnt(ucB2rId, _u4CurrentLineCount[ucB2rId]);
        if (_u4CurrentUnderFlow[ucB2rId] != 0)
        {
            //_prDbase->rVdp.au4UnderFlow[ucB2rId]++;
            VDP_HalIncreaseUnderFlow(ucB2rId, 1);
        }

        // do FRC here
        _rB2rFrc[ucB2rId].ucCurrentNs++;
        _VdpHandleFrc(ucB2rId);
    }
    else if (_u4CurrentIsrStatus[ucB2rId] & VDP_ISR_VEI)
    {
        // VEI is one-shot interrupt
        B2R_REG_DB_W_B(B2R_REG_RHEN,    0, B2R_REG_RHEN_VEIE_M);
    }

    if (_u4LastField[ucB2rId] == _u4CurrentField[ucB2rId])
    {
        LOG(10, "VDP_HAL.c: Wrong field. ucB2rId = %d\n", ucB2rId);
    }
    _u4LastField[ucB2rId] = _u4CurrentField[ucB2rId];

    if (_b2rObject[ucB2rId])
    {
        B2R_HAL_COMM_T * host = (B2R_HAL_COMM_T *)_b2rObject[ucB2rId];
        B2R_HAL_STATUS_T tStatus;

        tStatus.u4CurrentLR    = _u4CurrentLR[ucB2rId];
        tStatus.u4CurrentField = _u4CurrentField[ucB2rId];
        tStatus.u4UnderFlow    = _u4CurrentUnderFlow[ucB2rId];
        tStatus.u4Status       = _u4CurrentIsrStatus[ucB2rId];
        tStatus.u4LineCnt      = _u4CurrentLineCount[ucB2rId];
        
        if (host->IsrCb)
        {
            host->IsrCb(host->pvData, (VOID *)&tStatus);
        }
    }
    
}

void _B2rHdIsr(UINT16 u2Vector)
{

    UCHAR ucB2rId =0;
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

        _B2rReadStatus(ucB2rId);

        //clear underflow bit
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_FMON_M);
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_FMON_M, B2R_REG_HMODE_FMON_M);

        _B2rHdIsrCb(ucB2rId);
        
    }
    // clear global isr

    if (fgHDISR2)
    {
        B2R_REG_W(B2R_REG_PCRT,0x10);
    }
    if (fgSDISR2)
    {
        B2R_REG_W(B2R_REG_PCRT,0x20);
    }

    UNUSED(u2Vector);

}


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
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_1080P_OUT_HEIGHT)
    {
        i4BlankLine = (1125 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_1440P_OUT_HEIGHT)
    {
        i4BlankLine = (1650 - u4SrcHeight);
    }
    else if (_rB2rInfo[ucB2rId].u4OutHeight <= B2R_2160P_OUT_HEIGHT)
    {
        i4BlankLine = (2250 - u4SrcHeight);
    }
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
/** Brief of _VdpHandleFrc.
 */
//-----------------------------------------------------------------------------
static void _VdpHandleFrc(UCHAR ucB2rId)
{
    B2R_PRM_T* prB2rPrm = NULL;
    UCHAR      ucVdpId  = _B2R_GetVdpId(ucB2rId);

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

    //if (_rB2rFrc.ucCurrentNs == 0)
    // DTV00093447 for di has do some change, b2r should send T/P by order, or will freeze
    if ( ( _rB2rFrc[ucB2rId].ucCurrentNs == 0 ) /* && ( VDP_FRC_TRICK_PLAY != _rB2rFrc[ucVdpId].ucMode  ) */)
    {
        FBM_PIC_HDR_T* prFbmPicHdr = NULL;
        B2R_PRM_T* prB2rPrm = VDP_GetVdpPrm(ucB2rId);

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
                            (_B2R_GetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD))
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
    if ((_rB2rFrc[ucB2rId].u2TargetNs == 3) && (_rB2rFrc[ucB2rId].ucCurrentNs != _rB2rFrc[ucB2rId].u2TargetNs))
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

    if (_rB2rFrc[ucB2rId].ucCurrentNs == _rB2rFrc[ucB2rId].u2TargetNs)
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
void _VdpDoMirrorFlip(UCHAR ucB2rId,BOOL fgDo)
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

    //_prDbase->rVdp.au4StopLineCnt[ucB2rId] = _u4CurrentLineCount[ucB2rId];
    VDP_HalSetStopLineCnt(ucB2rId, _u4CurrentLineCount[ucB2rId]);
    if (_u4CurrentLineCount[ucB2rId] > _u4B2rActiveHeight[ucB2rId])
    {
        //_prDbase->rVdp.au4Tearing[ucB2rId]++;
        _B2R_HalIncreaseTearing(ucB2rId, 1);
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalInit.
 */
//-----------------------------------------------------------------------------
void _VDP_HalInit(void)
{
    if (_ucB2rIsrInitiated == 0)
    {
        //x_os_isr_fct pfnOldIsr;
        //CRIT_STATE_T _rCritState;
        UINT32 u4VdpIdx;
        UINT32 u4B2rIdx;
        UCHAR ucB2rId;

        _ucB2rIsrInitiated = 1;
        _fgForceResetClk = FALSE;

        for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
        {
            _prVdpCfg[u4VdpIdx] = _B2R_GetVdpConf((UCHAR)u4VdpIdx);
        }
        UNUSED(_prVdpCfg); /* for warning free */

        //_prDbase = DBS_Lock(&_rCritState);
        //ASSERT(_prDbase);
        //VERIFY(DBS_Unlock(_prDbase, _rCritState));

        //_VDP_HalReset();

        //VERIFY(x_reg_isr(VECTOR_DISPLAY, _B2rHdIsr, &pfnOldIsr) == OSR_OK);
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
            _rB2rFrc[u4B2rIdx].ucFrcEn = 0;
            _rB2rFrc[u4B2rIdx].u2TargetNs = 0;
            _rB2rFrc[u4B2rIdx].u2ChangeFieldNs = 0;

            _rB2rFrc[u4B2rIdx].ucCurrentNs = 0;
            _rB2rFrc[u4B2rIdx].eRepeatField = VDP_FRC_FIELD_MAX;
        }
        // Init Format
        _VDP_HalSetFormat(B2R_1, 0, ADDR_MODE_64_32);//TBD
        _fgEnableB2RFieldLog = FALSE;
    }

}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalReset.
 */
//-----------------------------------------------------------------------------
void _VDP_HalReset(void)
{


    UCHAR ucB2rId ;
    for(ucB2rId =0 ; ucB2rId < VDP_NS ; ucB2rId++)
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
    ucB2rId = 0;

    // Reset: Write 1 than Write 0
    B2R_REG_W_B(B2R_REG_SRST,
                (B2R_REG_SRST_HDRST_M | B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M),
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M));

    B2R_REG_W_B(B2R_REG_SRST,
                0,
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M));
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetEnable.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetEnable(UCHAR ucB2rId, UCHAR ucEnable)
{
    UINT32 u4Reset;
    VERIFY_B2R_ID(ucB2rId);
    LOG(1, "[%s]B2r(%d) HW Status(%d->%d).\n", __func__, ucB2rId, _rB2rInfo[ucB2rId].u4En, ucEnable);
    if (_rB2rInfo[ucB2rId].u4En != ucEnable)
    {
        u4Reset = 1;
    }
    else
    {
        u4Reset = 0;
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
        VDP_HalSetB2RPower(ucB2rId, TRUE);
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


    }
    else
    {
        // disable double register buffer
        B2R_REG_DB_W_B(B2R_REG_RHEN, 0, B2R_REG_RHEN_VPTE_M);

        B2R_REG_DB_W_B(B2R_REG_RHEN,
                       0,
                       (B2R_REG_RHEN_M|B2R_REG_RHEN_ADIE_M|B2R_REG_RHEN_VEIE_M));

#ifndef CC_SECOND_B2R_SUPPORT
        VDP_HalSetB2RPower(ucB2rId, FALSE);
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


    //copy from _B2R_GetSequenceInfo
    if ((u4Width < 128 && u4Height < 128))
    {
        B2R_REG_DB_W_M(B2R_REG_HMODE, 0, B2R_REG_HMODE_BURST_M, B2R_REG_HMODE_BURST_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_HMODE, 1, B2R_REG_HMODE_BURST_M, B2R_REG_HMODE_BURST_S);
    }

}

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetOutSize.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetOutSize(UCHAR ucB2rId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Interlace, UINT32 u4DoubleClk, UINT32 u4ClkMode)
{
    static UINT32 _u4B2RClkMode = 255;
    static UINT32 Iu4B2RClkModeSdPath = 0xff;
    UCHAR      ucVdpId  = _B2R_GetVdpId(ucB2rId);
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
            (ucB2rId == B2R_1 && u4ClkMode != _u4B2RClkMode) ||
            (ucB2rId == B2R_2 && u4ClkMode != Iu4B2RClkModeSdPath))
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

    //SD path
    if(ucB2rId == B2R_2)
    {
        u4VdoInReg = IO_REG32(0xf0022000, 0xbe4);
        u4VdoInReg = u4VdoInReg | 0x80000000;
        IO_REG32(0xf0022000, 0xbe4) = u4VdoInReg;
    }

#ifdef CC_B2R_3D_SUPPROT
    //set 3D relative register
    prB2rPrm = VDP_GetVdpPrm(ucB2rId);

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
        B2R_REG_DB_W_M(B2R_REG_PORCH, 2, B2R_REG_PORCH_TAG_MODE_M, B2R_REG_PORCH_TAG_MODE_S);
    }
    else
    {
        B2R_REG_DB_W_M(B2R_REG_PORCH, 0, B2R_REG_PORCH_TAG_MODE_M, B2R_REG_PORCH_TAG_MODE_S);
    }
#endif

    //B2R_REG_W(B2R_REG_PORCH,0x10021);
    // Disable B2R DRAM Control
    prB2rPrmAnotherPath = VDP_GetVdpPrm(1-ucB2rId);
    if (prB2rPrmAnotherPath && (prB2rPrmAnotherPath->ucReady == 1))
    {
        LOG(1, "VDP(%d) Ignore DRAM clock reset\n", ucB2rId);
        fgResetDRAMClock = FALSE;        
    }
    if (fgResetDRAMClock)
    {
        B2R_REG_W_B(B2R_REG_SRST, B2R_REG_SRST_DMRST_M, B2R_REG_SRST_DMRST_M);
    }

    if (_fgForceResetClk)
    {
        _u4B2RClkMode = 0xFF;
        Iu4B2RClkModeSdPath = 0xFF;
        _fgForceResetClk = FALSE;
    }

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

        if(ucB2rId == B2R_1)
        {
            //0xD23C[8] = 1, from syspll
            IO_REG32(CKGEN_BASE, 0x228) = (0x1);

            // SD clock, if RealD, the clock is hardwired connect to the clock of HD
            // It means the M, N of SD will be ignored.

            //IO_REG32(CKGEN_BASE, 0x2e0) = VERIFICATION_REALD ? (0x10001) : (0x1);

            LOG(3, "SYSPLL = 216 MHz\n");

            switch (_u4B2RClkMode)
            {                  
            case VDP_B2R_CLK_064_800:
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 3;                  // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (100 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x2805, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            case VDP_B2R_CLK_08_100:
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 3;                  // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (80 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0xC01, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            case VDP_B2R_CLK_10_800:
            case VDP_B2R_CLK_32_400:
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 6;                  // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (120 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x30007d, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            case VDP_B2R_CLK_297_000:
                LOG(1,"=======4K2K  30 FPS==========\n");
                IO_REG32(CKGEN_BASE, 0x228) = (0x201);
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 1024;                   // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (1117 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x400045d,(B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            case VDP_B2R_CLK_10_368:
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 6;                  // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (125 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x30007d, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            case VDP_B2R_CLK_37_125:
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 11;                   // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (64 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0xb0008, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            case VDP_B2R_CLK_21_996:
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0xFFFF0000) + 611;                   // M
                IO_REG32(CKGEN_BASE, 0x22c) = (IO_REG32(CKGEN_BASE, 0x22c) & 0x0000FFFF) + (6000 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x26302ee, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
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
        }
        else
        {
            //0xD23C[8] = 1, from syspll
            //IO_REG32(CKGEN_BASE, 0x2e0) = (IO_REG32(CKGEN_BASE, 0x2e0) & 0xFFFFFEFE) + (0x101);
            IO_REG32(CKGEN_BASE, 0x230) = (0x1);

            LOG(3, "SYSPLL = 216 MHz\n");

            switch (Iu4B2RClkModeSdPath)
            {
             case VDP_B2R_CLK_08_100:
             case VDP_B2R_CLK_10_800:
             case VDP_B2R_CLK_32_400:
				IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 6;                  // M
                IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (120 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x30007d, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
				break;
             case VDP_B2R_CLK_297_000:
			 	LOG(1,"=======4K2K  30 FPS==========\n");
			 	IO_REG32(CKGEN_BASE, 0x228) = (0x201);
                IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1024;                   // M
                IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (1117 << 16);        // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x400045d,(B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
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

            case VDP_B2R_CLK_27_000:
            default:
                IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0xFFFF0000) + 1;                  // M
                IO_REG32(CKGEN_BASE, 0x234) = (IO_REG32(CKGEN_BASE, 0x234) & 0x0000FFFF) + (8 << 16);    // N
                B2R_REG_W_B(B2R_REG_RPLL, 0x20002, (B2R_REG_RPLL_PLLN_M|B2R_REG_RPLL_PLLM_M));
                break;
            }
        }




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

    // B2R_REG_HMODE_HTBM_M = 0
    // B2R_REG_HMODE_HBTM_M = 0
    //B2R_REG_DB_W_B(B2R_REG_HMODE, 0, (B2R_REG_HMODE_HTBM_M | B2R_REG_HMODE_HBTM_M));

    if (_rB2rInfo[ucB2rId].u4OutInterlace != 0)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_RHI_M, B2R_REG_HMODE_RHI_M);

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
    VERIFY_B2R_ID(ucB2rId);

    if(u4AddrY != 0 && u4AddrC != 0)
    {
        B2R_REG_DB_W(B2R_REG_RHYS, u4AddrY);
        B2R_REG_DB_W(B2R_REG_RHCS, u4AddrC);
    }
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

//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetFrc.
 */
//-----------------------------------------------------------------------------
void _VDP_HalSetFrc(UCHAR ucB2rId, UCHAR u2TargetNs, UCHAR u2ChangeFieldNs, UCHAR ucMode)
{
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
        _rB2rFrc[ucB2rId].ucFrcEn = 1;


#ifdef VDP_FRC_WA2
        u2ChangeFieldNs = u2TargetNs;
#endif

        if (ucMode == VDP_FRC_STOP_AUTO)
        {
            //copy bottom field to top field WA
            //_VdpGfxCopyField(ucVdpId);
            u2ChangeFieldNs = u2TargetNs;
        }

        _rB2rFrc[ucB2rId].ucCurrentNs = 0;
        _rB2rFrc[ucB2rId].u2TargetNs = u2TargetNs;
        _rB2rFrc[ucB2rId].u2ChangeFieldNs = u2ChangeFieldNs;
        _rB2rFrc[ucB2rId].ucMode = ucMode;

        _VdpHandleFrc(ucB2rId);
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

    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_H422_M);

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
    //_prDbase->rVdp.au4UnderFlow[B2R_1] = _u4CurrentUnderFlow[ucB2rId];
    _B2R_HalSetUnderFlow(B2R_1, _u4CurrentUnderFlow[ucB2rId]);
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

void VDP_EnableBurstRead(UCHAR ucB2rId, BOOL fgEn)
{
    if (ucB2rId >= B2R_NS)
    {
        return;
    }
    LOG(0, "ucB2rId = %d, fgEn = %d\n", ucB2rId, fgEn);
    B2R_REG_DB_W_M(B2R_REG_HMODE, (fgEn == TRUE), B2R_REG_HMODE_BURST_M, B2R_REG_HMODE_BURST_S);
}


void VDP_HalsetMirror(UCHAR ucB2rId, BOOL fgOn)
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
void VDP_HalGetB2rDispInfo(UCHAR ucB2rId,B2R_DISP_INFO_T* ptB2rInfo)
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
    VDP_HalSetB2RPower(0, FALSE);
#endif

    B2R_REG_W_B(B2R_REG_SRST,
                (B2R_REG_SRST_HDRST_M | B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M),
                (B2R_REG_SRST_HDRST_M |B2R_REG_SRST_F27RST_M |B2R_REG_SRST_DMRST_M|B2R_REG_SRST_DMEN_M));

    return;
}



//-----------------------------------------------------------------------------
/** Brief of VDP_HalGetYCStart
 */
//-----------------------------------------------------------------------------
void VDP_HalGetYCStart(UCHAR ucB2rId,UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
{


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

VOID VDP_HalSetB2RPower(UCHAR ucB2rId,BOOL fgPowerOn)
{
    UINT32 u4B2RCLKBitMask = 0xFFFFFF7F;
    UINT32 u4EnableB2RClKBitMask =0x80;

    UNUSED(fgPowerOn);
    LOG(0, "fgPowerOn = %d\n", fgPowerOn);
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

    UNUSED(ucB2rId);
}


VOID VDP_HalSetLRState(UCHAR ucB2rId,BOOL fgLView)
{
    if(fgLView)
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, 0, B2R_REG_HMODE_DISP_R_VIEW_M);
    }
    else
    {
        B2R_REG_DB_W_B(B2R_REG_HMODE, B2R_REG_HMODE_DISP_R_VIEW_M, B2R_REG_HMODE_DISP_R_VIEW_M);
    }

    UNUSED(ucB2rId);
}

VOID VDP_HalSetTBState(UCHAR ucB2rId,BOOL fgTopField)
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
    
    UNUSED(ucB2rId);
}

VOID VDP_EnableB2RFieldLog(BOOL fgOn)
{
	_fgEnableB2RFieldLog = fgOn;
}

VOID VDP_ForceResetClk(BOOL fgDo)
{
	_fgForceResetClk = fgDo;
}



//-----------------------------------------------------------------------------
/** Brief of _VDP_HalB2rSwitch.
 */
//-----------------------------------------------------------------------------


void _B2R_B2rSwitch(UCHAR ucVdpId, UCHAR ucB2rId)
{
    if ((ucVdpId >= VDP_NS))
    {
        return;
    }

    _B2R_VsyncMutexLock(ucB2rId);

#ifdef CC_SCPOS_EN

    LOG(3, "B2R(%d) >> VDP(%d) switch(%d, %d).\n", ucB2rId, ucVdpId, _aucB2rSwitch[ucVdpId], _aucB2rSwitch[1-ucVdpId]);

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
                _prVdpCfg[_ucVdpSwitch]->u4MpegHeight = _prVdpCfg[ucVdpId]->u4MpegHeight;
                _prVdpCfg[_ucVdpSwitch]->u4MpegWidth = _prVdpCfg[ucVdpId]->u4MpegWidth;
                //_prVdpConf[_ucVdpSwitch]->rOutInfo = _prVdpConf[ucVdpId]->rOutInfo;
                x_memcpy(&_prVdpCfg[_ucVdpSwitch]->rOutInfo, &_prVdpCfg[ucVdpId]->rOutInfo, sizeof(B2R_OUT_INFO_T));
                x_memcpy(&_rB2rInfo[_ucVdpSwitch], &_rB2rInfo[ucVdpId], sizeof(VDP_INFO_T));
                _prVdpCfg[_ucVdpSwitch]->u4SrcLineSize = _prVdpCfg[ucVdpId]->u4SrcLineSize;
                _prVdpCfg[_ucVdpSwitch]->u4SrcHeight = _prVdpCfg[ucVdpId]->u4SrcHeight;
                _prVdpCfg[_ucVdpSwitch]->u4SrcWidth = _prVdpCfg[ucVdpId]->u4SrcWidth;
                _prVdpCfg[_ucVdpSwitch]->u4FrameRate = _prVdpCfg[ucVdpId]->u4FrameRate;

                _prVdpCfg[_ucVdpSwitch]->ucEarlyDisp = _prVdpCfg[ucVdpId]->ucEarlyDisp;
                _prVdpCfg[_ucVdpSwitch]->u4AspectRatio = _prVdpCfg[ucVdpId]->u4AspectRatio;
                _prVdpCfg[_ucVdpSwitch]->u4Afd = _prVdpCfg[ucVdpId]->u4Afd;
#endif
            }
        }
    }

    if ((_ucVdpSwitch >= VDP_NS))
    {
        _B2R_VsyncMutexUnlock(ucB2rId);
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
            _prVdpCfg[ucVdpId]->u4MpegHeight = _prVdpCfg[_ucVdpSwitch]->u4MpegHeight;
            _prVdpCfg[ucVdpId]->u4MpegWidth = _prVdpCfg[_ucVdpSwitch]->u4MpegWidth;
            //_prVdpConf[ucVdpId]->rOutInfo = _prVdpConf[_ucVdpSwitch]->rOutInfo;
            x_memcpy(&_prVdpCfg[ucVdpId]->rOutInfo, &_prVdpCfg[_ucVdpSwitch]->rOutInfo, sizeof(B2R_OUT_INFO_T));
            x_memcpy(&_rB2rInfo[ucVdpId], &_rB2rInfo[_ucVdpSwitch], sizeof(VDP_INFO_T));
            _prVdpCfg[ucVdpId]->u4SrcLineSize = _prVdpCfg[_ucVdpSwitch]->u4SrcLineSize;
            _prVdpCfg[ucVdpId]->u4SrcHeight = _prVdpCfg[_ucVdpSwitch]->u4SrcHeight;
            _prVdpCfg[ucVdpId]->u4SrcWidth = _prVdpCfg[_ucVdpSwitch]->u4SrcWidth;
            _prVdpCfg[ucVdpId]->u4FrameRate = _prVdpCfg[_ucVdpSwitch]->u4FrameRate;

            /* To protect ucEarlyDisp is set between "connect sub" and "connect main" */
            _prVdpCfg[ucVdpId]->ucEarlyDisp = _prVdpCfg[_ucVdpSwitch]->ucEarlyDisp;
#ifdef VDP_MUTE_EARLY_DISPLAY
            if (_prVdpCfg[ucVdpId]->ucEarlyDisp==1)
            {
                _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, FOREVER_MUTE, FALSE);
                LOG(3, "DTV Mute Early Display -3 VDP(%d)\n", ucVdpId);
            }
#else // VDP_MUTE_EARLY_DISPLAY
            LOG(0, "WARNING: DTV mute early display is disabled \n");
#endif
            _prVdpCfg[ucVdpId]->u4AspectRatio = _prVdpCfg[_ucVdpSwitch]->u4AspectRatio;
            _prVdpCfg[ucVdpId]->u4Afd = _prVdpCfg[_ucVdpSwitch]->u4Afd;

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
    B2R_MUTEX_UNLOCK(ucB2rId);

}


void _B2R_HalSetEnable(UCHAR ucB2rId, UCHAR ucEnable)
{
    VERIFY_B2R_ID(ucB2rId);
    LOG(1, "[%s]B2r(%d) HW Status(%d->%d).\n", __func__, ucB2rId, _rB2rInfo[ucB2rId].u4En, ucEnable);
    _rB2rInfo[ucB2rId].u4En = ucEnable;
}


void _B2R_HalSetSrcSize(UCHAR ucB2rId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
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

}


void _B2R_HalSetOutSize(UCHAR ucB2rId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Interlace, UINT32 u4DoubleClk, UINT32 u4ClkMode)
{
    static UINT32 _u4B2RClkMode = 255;
    static UINT32 Iu4B2RClkModeSdPath = 0xff;
    UCHAR      ucVdpId  = _B2R_GetVdpId(ucB2rId);

    if ((ucB2rId >= B2R_NS) || (ucVdpId >= VDP_NS))
    {
        ASSERT((ucB2rId < B2R_NS) && (ucVdpId < VDP_NS));
        return;
    }

    if ((u4Interlace != _rB2rInfo[ucB2rId].u4OutInterlace) ||
            (ucB2rId == B2R_1 && u4ClkMode != _u4B2RClkMode) ||
            (ucB2rId == B2R_2 && u4ClkMode != Iu4B2RClkModeSdPath))
    {
        //_VDP_SetScposDelayUnMute(ucVdpId, 6, 1);
        _vDrvVideoSetMute(MUTE_MODULE_B2R, ucVdpId, 6, TRUE);
    }

    _rB2rInfo[ucB2rId].u4OutWidth = u4Width;
    _rB2rInfo[ucB2rId].u4OutHeight = u4Height;
    _rB2rInfo[ucB2rId].u4OutInterlace = u4Interlace;
    _rB2rInfo[ucB2rId].u4OutDoubleClk = u4DoubleClk;

    #if 0
    {
    UINT32 u4VdoInReg;
    //setting VDOIN register
    //HD path
    u4VdoInReg = IO_REG32(0xf0022000, 0xbe0);
    u4VdoInReg = u4VdoInReg | 0x80000000;
    IO_REG32(0xf0022000, 0xbe0) = u4VdoInReg;

    //SD path
    if(ucB2rId == B2R_2)
    {
        u4VdoInReg = IO_REG32(0xf0022000, 0xbe4);
        u4VdoInReg = u4VdoInReg | 0x80000000;
        IO_REG32(0xf0022000, 0xbe4) = u4VdoInReg;
    }
    }
    #endif

    // Consider MDDi mode change
    //_VDP_HalSetEnable(ucB2rId, _rB2rInfo[ucB2rId].u4En);
    // Consider Source Size
    //_VDP_HalSetSrcSize(ucB2rId, _rB2rInfo[ucB2rId].u4SrcWidth, _rB2rInfo[ucB2rId].u4SrcHeight, _rB2rInfo[ucB2rId].u4SrcPitch);
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

#if defined(CC_MT5890)
static UINT16 uHTotal =0xFF;
static UINT16 uVTotal =0xFF;
static UINT16 uHActive =0xFF;
static UINT16 uVActive =0xFF;

#define Front_Porch  88
#define Sync_width 44
#define Back_porch 144
#define Porch_offset 28
#define vsyncOffset 9
#define vFrontPorch 8
#define VsyncWidth 10
#define VBackPorch 72
#define VB1EnableAdd 0x34010
#define VB1ColorTranf 0x068
#define VB1ClkSel 0xD230
#define VB1SrcSel 0x064
#define VB1OClk   0x3

UINT16 VDP_GetNB2R_Htotal(VOID)
{
  if(uHTotal!=0xFF)
  {
      return uHTotal;
  }
  else
  {
       return 0;
  }
}
UINT16 VDP_GetNB2R_Vtotal(VOID)
{
  if(uVTotal!=0xFF)
  {
      return uVTotal;
  }
  else
  {
       return 0;
  }
}
UINT16 VDP_GetNB2R_HActive(VOID)
{
  if(uHActive!=0xFF)
  {
      return uHActive;
  }
  else
  {
       return 0;
  }
}
UINT16 VDP_GetNB2R_VActive(VOID)
{
  if(uVActive!=0xFF)
  {
      return uVActive;
  }
  else
  {
       return 0;
  }
}

void VDP_SetNB2R_Bypass_H_ACTIVE(UINT16 u2HActive)
{
   uHActive=u2HActive;
   u2HActive =u2HActive>>1;
   NB2R_REG_W_M(NB2R_REG_HACT, (u2HActive >> 1), NB2R_REG_HACT_RHAW_M, NB2R_REG_HACT_RHAW_S);
}
void VDP_SetNB2R_Bypass_H_TOTAL(UINT16 u2HTotal)
{
   
   uHTotal=(u2HTotal>>1)-1;
   NB2R_REG_W_M(NB2R_REG_HTOTAL,uHTotal, NB2R_REG_HHVT_HHT_M, NB2R_REG_HHVT_HHT_S);

}
void VDP_SetNB2R_Bypass_V_ACTIVE(UINT16 u2VActive)
{
    uVActive=u2VActive;
    NB2R_REG_W_M(NB2R_REG_HACT, u2VActive, NB2R_REG_HACT_HAH_M, NB2R_REG_HACT_HAH_S);

}
void VDP_SetNB2R_Bypass_V_TOTAL(UINT16 u2VTotal)
{
    uVTotal=(u2VTotal - 1);
    NB2R_REG_W_M(NB2R_REG_HTOTAL, (u2VTotal - 1), NB2R_REG_HHVT_HVT_M, NB2R_REG_HHVT_HVT_S);

}

void VDP_SetNB2R_Bypass_H_FrontPorch(UINT16 u2Fp)
{
    UINT16 u2Porch;
    if(uHTotal==0xFF)
    {
        LOG(0,"Warning uHTotal Should is not set by video path");
    }
    u2Porch=uHTotal-(Back_porch-Porch_offset)-Sync_width;//-(NB2R_H_PORCH_BACK_WIDTH-NB2R_H_PORCH_OFFSET)-NB2R_H_PORCH_SYNC_WIDTH;
    NB2R_REG_W_M(NB2R_REG_H_FRONTPORCH, u2Porch, NB2R_REG_H_FRONTPORCH_M, NB2R_REG_H_FRONTPORCH_S);

}
void VDP_SetNB2R_Bypass_H_BackPorch(UINT16 u2Fp)
{
    UINT16 u2Porch;
    if(uHTotal==0xFF)
    {
        LOG(0,"Warning uHTotal Should is not set by video path");
    }
     u2Porch=uHTotal-(Back_porch-Porch_offset);//-NB2R_H_PORCH_BACK_WIDTH-NB2R_H_PORCH_OFFSET;
     NB2R_REG_W_M(NB2R_REG_H_BACKPORCH, u2Porch, NB2R_REG_H_BACKPORCH_M, NB2R_REG_H_BACKPORCH_S);

}

void VDP_SetNB2R_Bypass_V_FrontPorch(UINT16 u2Fp)
{
    UINT16 u2Porch;
    if(uVTotal==0xFF)
    {
        LOG(0,"Warning uHTotal Should is not set by video path");
    }
    u2Porch=uVTotal-(VBackPorch-vsyncOffset)-VsyncWidth;//-NB2R_V_PORCH_BACK_WIDTH-NB2R_V_PORCH_OFFSET-NB2R_V_PORCH_SYNC_WIDTH;
    NB2R_REG_W_M(NB2R_REG_V_FRONTPORCH, u2Porch, NB2R_REG_V_FRONTPORCH_M, NB2R_REG_V_FRONTPORCH_S);

}
void VDP_SetNB2R_Bypass_V_BackPorch(UINT16 u2Fp)
{
    UINT16 u2Porch;
    u2Porch=uVTotal-(VBackPorch-vsyncOffset);//-NB2R_V_PORCH_BACK_WIDTH-NB2R_V_PORCH_OFFSET;
    if(uVTotal==0xFF)
    {
        LOG(0,"Warning uHTotal Should is not set by video path");
    }
    NB2R_REG_W_M(NB2R_REG_V_BACKPORCH, u2Porch, NB2R_REG_V_BACKPORCH_M, NB2R_REG_V_BACKPORCH_S);

    Printf("VDP_SetNB2R_Bypass_V_BackPorch=%x\n",NB2R_REG_R(NB2R_REG_V_BACKPORCH));

}
void VDP_SetNB2R_Bypass_conf(BOOL HV_SYNC,BOOL enable)
{
    if(HV_SYNC)
    {
        if (enable)
          {
              
               NB2R_REG_W_B(NB2R_REG_HS_INV, NB2R_REG_HS_INV_H, NB2R_REG_HS_INV_H);  //enable Hsync invert
           }
         else
           {
               NB2R_REG_W_B(NB2R_REG_HS_INV, 0, NB2R_REG_HS_INV_H);  //disnble Hsync invert
           }
    }
    else
    {
    //#ifdef CC_B2R_RM_SUPPORT
       vSetVB1SrcType(TRUE);
       vSetVB1Color_Transf(TRUE,BT601_2_RGB);
       NB2R_REG_VB1(VB1ClkSel,VB1OClk);
    //#endif
        if (enable)
           {
                  
               NB2R_REG_W_B(NB2R_REG_HS_INV, NB2R_REG_HS_INV_V, NB2R_REG_HS_INV_V);  //enable Vsync invert
           }
         else
            {
                NB2R_REG_W_B(NB2R_REG_HS_INV, 0, NB2R_REG_HS_INV_V);  //disnble Vsync invert
            }
    }

      if(enable)
          NB2R_REG_W_B(NB2R_REG_UFO, NB2R_REG_UFO_M, NB2R_REG_UFO_M);
      else
          NB2R_REG_W_B(NB2R_REG_UFO, 0, NB2R_REG_HS_INV_V);

}
void VDP_SetNB2R_Bypass_enable(BOOL enable,BOOL bittype)
{
  if(enable)
    {
    
	    if(bittype)
	    {
	        NB2R_REG_W_M(NB2R_REG_UFO,NB2R_REG_UFO_MODE_10,NB2R_REG_UFO_MODE_M,NB2R_REG_UFO_MODE_S);
	    }
	    else
	    {
	        NB2R_REG_W_M(NB2R_REG_UFO,NB2R_REG_UFO_MODE_8,NB2R_REG_UFO_MODE_M,NB2R_REG_UFO_MODE_S);
	    }

        NB2R_REG_W_B_BITS(B2R_SRC_SEL, VB1_PATH_SRC_SEL, 1);
	  
    }

}


#endif


