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
 * $RCSfile: imgrz_hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file imgrz_hw.c
 *  image resizer hardware interfaces
 *  purely hardware related functions, include crash detection routines,
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_bim.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "imgrz_dif.h"
#include "imgrz_hw.h"
#include "imgrz_cmdque.h"
#include "imgrz_debug.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------
typedef struct _E_IMGRZ_TEMP_DATA
{
    UINT32 u4SrcCntY;
    UINT32 u4SrcCntCb;
    UINT32 u4SrcCntCr;
    UINT32 u4VWrOftY;
    UINT32 u4VWrOftCb;
    UINT32 u4VWrOftCr;
    UINT32 u4VNextCoefY;
    UINT32 u4VNextCoefCb;
    UINT32 u4VNextCoefCr;
} E_IMGRZ_TEMP_DATA;

E_IMGRZ_TEMP_DATA _rTempData;

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

static INT32 _i4RzSetupISR = IMGRZ_DISABLE;


//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** _ImgrzHwRealIsr
 *  interrupt service routine
 *  clear irq, calculate time, trigger hw_isr interface
 */
//-------------------------------------------------------------------------
//extern UINT32  ImgrzhwISR;
static void _ImgrzHwRealIsr(UINT16 u2Vector)
{
    UNUSED(u2Vector);               // for lint happy

    if (BIM_ClearIrq((UINT32)VECTOR_IMGRZ))
    {
        // stop crash detection timer ?
        if ((_IMGRZ_GetCrashDetection() == IMGRZ_ENABLE)  &&
            (_IMGRZ_GetScaleOpt() != E_RZ_INOUT_TYPE_JPGMD))
        {
            VERIFY(x_timer_stop(_IMGRZ_GetCrashDetectTimer()) == OSR_OK);
        }

        IMGRZ_DBG_REC_CUR_STATUS((INT32)E_IMGRZ_IDLE);

    #if !defined(IMGRZ_RISC_MODE)     // cmdque mode
        IMGRZ_HwISR();
    #endif

        // unlock cmdque resource
        // ImgrzhwISR++;
        //LOG(0, "_ImgrzHwRealIsr\n");
        IMGRZ_UnlockWaitIsr();
      //  IMGRZ_UnlockCmdque();

        IMGRZ_DBG_INC_DRV_INT_COUNT();    // for debug use
        IMGRZ_CMDQUE_INC_INT_COUNT();     // for debug use
    }
}


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** IMGRZ_HwInit
 *  init hardware
 *  set control register address to default
 *  set default isr
 *  do hw reset
 */
//-------------------------------------------------------------------------
void IMGRZ_HwInit(void)
{
    x_os_isr_fct pfnOldIsr;

    // setup ISR
    if (_i4RzSetupISR == IMGRZ_DISABLE)
    {
        VERIFY(x_reg_isr((UINT16)VECTOR_IMGRZ, _ImgrzHwRealIsr, &pfnOldIsr) == OSR_OK);
        _i4RzSetupISR = IMGRZ_ENABLE;
    }
}


//-------------------------------------------------------------------------
/** IMGRZ_HwGetRegBase
 *  get hw control register base
 */
//-------------------------------------------------------------------------
void IMGRZ_HwGetRegBase(UINT32 **ppu4RegBase)
{
    IMGRZ_ASSERT(ppu4RegBase != NULL);
    IMGRZ_ASSERT(*ppu4RegBase != NULL);

    *ppu4RegBase = (UINT32 *)(IMGRZ_ADDR);
}

//-------------------------------------------------------------------------
/** IMGRZ_HwISR
 *  hw isr
 *  reset crash detection vars
 *  set engine to idle
 *  notify if notification was set
 */
//-------------------------------------------------------------------------
void IMGRZ_HwISR(void)
{
    // call call-back function
    if (pfnIMGRZ_DifFinNotify != NULL)
    {
        if (_IMGRZ_IsFlushing())  // if MW uses flush (IMGRZ_MwFlush)
        {
            _IMGRZ_SetFlushStatus(FALSE);

            // notify to middleware
            pfnIMGRZ_DifFinNotify((UINT32)E_RZ_OK);

            IMGRZ_DBG_INC_MW_INT_COUNT();  // for debug use
        }
    }
}


//-------------------------------------------------------------------------
/** IMGRZ_HwReset
 *  hw reset
 *  trigger hw reset
 *  start clock
 *  enable interrupt
 *  set register to default
 */
//-------------------------------------------------------------------------
void IMGRZ_HwReset(void)
{
    volatile UINT32 u4Value, u4Value2;
    volatile UINT32 u4RegOffset;

    UINT32 u4ResHi = (UINT32)0x3;
    UINT32 u4ResLo = (UINT32)0x0;

#ifndef IMGRZ_RISC_MODE
    //BC
    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, 0x0);
#endif    

#ifdef CC_LOW_POWER
    // do clock selection (162 MHz)
    IMGRZ_HwSetEngClkSrc((INT32)E_IMGRZ_CLK_SYSPLL_D2);
#else

#if defined(CC_MT5387) && !defined(CC_MT5363) 
    IMGRZ_HwSetEngClkSrc((INT32)E_IMGRZ_CLK_SYSPLL_D1P5);
#elif defined(JPG_CLK_FROM_GRAPHIC)
    IO_WRITE32(CKGEN_BASE, 0x230, 0x1); // 240 MHZ
#else
    // do clock selection (270 MHz)
    //IMGRZ_HwSetEngClkSrc((INT32)E_IMGRZ_CLK_TVDPLL_D2);
    //5363, imgrz clock uses vdec_mc, 216MHz
    IO_WRITE32(CKGEN_BASE, 0x228, 0x3);
#endif

#endif

    // enable engine & dram clock (set non-zero value)
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363) 
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif    
    IMGRZ_WRITE32(u4RegOffset, RZREG_MSK_CKEN | RZREG_MSK_REGRST);    // fg_CKEN = 0x1;
    
    // reset engine :
    // 1. fg_SW_RSTB = 0x3
    // 2. fg_SW_RSTB = 0x0
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363) 
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else    
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif    
    u4Value = IMGRZ_READ32(u4RegOffset);

    u4Value = (u4Value & (~(RZREG_MSK_SW_RSTB)));
    u4Value = ((u4Value) | (u4ResHi << RZREG_SHT_SW_RSTB));
    IMGRZ_WRITE32(u4RegOffset, u4Value);

    u4Value = (u4Value & (~(RZREG_MSK_SW_RSTB)));
    u4Value = ((u4Value) | (u4ResLo << RZREG_SHT_SW_RSTB));
    IMGRZ_WRITE32(u4RegOffset, u4Value);

#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    // reset cmdque :
    // 1.  fg_SW_CMDQ_RST_B = 0x3
    // 2.  fg_SW_CMDQ_RST_B = 0x0
    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);

    u4Value = (u4Value & (~(RZREG_MSK_SW_CMDQ_RST_B)));
    u4Value = ((u4Value) | (u4ResHi << RZREG_SHT_SW_CMDQ_RST_B));
    IMGRZ_WRITE32(u4RegOffset, u4Value);

    u4Value = (u4Value & (~(RZREG_MSK_SW_CMDQ_RST_B)));
    u4Value = ((u4Value) | (u4ResLo << RZREG_SHT_SW_CMDQ_RST_B));
    IMGRZ_WRITE32(u4RegOffset, u4Value);
#endif    
    // wait hw reset ready
    for ( ; ; )
    {
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363) 
        u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else    
        u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif
        u4Value = IMGRZ_READ32(u4RegOffset);

#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
        u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
        u4Value2 = IMGRZ_READ32(u4RegOffset);
#else
        u4Value2 = IMGRZ_CMDQ_RST_READY;
#endif
        
        if (((u4Value  & RZREG_MSK_SW_RSTB)       == IMGRZ_ENG_RST_READY) &&
            ((u4Value2 & RZREG_MSK_SW_CMDQ_RST_B) == IMGRZ_CMDQ_RST_READY))
        {
            break;
        }
    }

#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    // set CMDQ threshold
    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Value = (u4Value) & (~(RZREG_MSK_CMDQ_THRS));
    u4Value = (u4Value) | (0x1 << RZREG_SHT_CMDQ_THRS);
    IMGRZ_WRITE32(u4RegOffset, u4Value);
#endif    
    
    // enable interrupt : set fg_INT_MASK to 0x1
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363) 
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else    
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif    

#ifndef IMGRZ_POLLING
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Value = (u4Value | RZREG_MSK_INT_MASK);
    IMGRZ_WRITE32(u4RegOffset, u4Value);   
#endif    
}


//-------------------------------------------------------------------------
/** IMGRZ_HwGetIdle
 *  get hardware idle status
 */
//-------------------------------------------------------------------------
INT32 IMGRZ_HwGetIdle(void)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_RZDONE << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Value = ((u4Value & RZREG_MSK_IDLE) >> RZREG_SHT_IDLE);

    return ((INT32)(u4Value));
}


//-------------------------------------------------------------------------
/** IMGRZ_HwAction
 *  start hardware engine
 *  used in interrupt mode only
 *  in command queue mode, use command queue action
 */
//-------------------------------------------------------------------------
void IMGRZ_HwAction(void)
{
    return;
}


//-------------------------------------------------------------------------
/** IMGRZ_HwSetRiscMode
 *  disable command que mode
 */
//-------------------------------------------------------------------------
void IMGRZ_HwSetRiscMode(void)
{
#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Value = ((u4Value) & (~RZREG_MSK_EN_DRAMQ));
    IMGRZ_WRITE32(u4RegOffset, u4Value);
#endif    
}


//-------------------------------------------------------------------------
/** IMGRZ_HwSetCmdQMode
 *  enable command que mode
 */
//-------------------------------------------------------------------------
void IMGRZ_HwSetCmdQMode(void)
{
#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Value = ((u4Value) | (RZREG_MSK_EN_DRAMQ));
    IMGRZ_WRITE32(u4RegOffset, u4Value);
#endif    
}


//-------------------------------------------------------------------------
/** IMGRZ_HwSetEngClkSrc
 *  set engine clock source
 */
//-------------------------------------------------------------------------
void IMGRZ_HwSetEngClkSrc(INT32 i4ClkSrc)
{
    // GRA_CK : 0x2000D21C (mt539x)
    CLKSRC_ChangeGFXClock(i4ClkSrc);

    IMGRZ_DBG_REC_ENG_CLK_SRC(i4ClkSrc);
}


//-------------------------------------------------------------------------
/** _IMGRZ_HwSetSRAM
 *  set engine internal SRAM
 */
//-------------------------------------------------------------------------
void _IMGRZ_HwSetSRAM(UINT32 u4Pitch, UINT32 u4Is16BppPixPrec)
{
}

//-------------------------------------------------------------------------
/** _IMGRZ_HwGetCurrentLine
 *  get current scale line
 */
//-------------------------------------------------------------------------
UINT32 _IMGRZ_HwGetCurrentLine(void)
{
    UINT32 u4Ret;
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;
#ifdef CC_MT5363
    u4RegOffset = ((RZ_REG_RZLINECNT << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Ret = (UINT32)((u4Value >> 16) & 0x3FFF);
#else
    u4RegOffset = ((RZ_REG_RZSTMON << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Ret = (UINT32)((u4Value >> 16) & 0xFFF);
#endif
    return u4Ret;
}

#ifdef IMGRZ_POLLING
INT32 IMGRZ_HwPolling(void)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_RZDONE << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4RegOffset);
    u4Value = u4Value & 0x1;

    return ((INT32)(u4Value));
}
#endif

void IMGRZ_HwBreak(void)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_SRCCNTY << 2) & IMGRZ_REG_MASK);
    _rTempData.u4SrcCntY = IMGRZ_READ32(u4RegOffset);
    u4RegOffset = ((RZ_REG_SRCCNTCB << 2) & IMGRZ_REG_MASK);
    _rTempData.u4SrcCntCb = IMGRZ_READ32(u4RegOffset);
    u4RegOffset = ((RZ_REG_SRCCNTCR << 2) & IMGRZ_REG_MASK);
    _rTempData.u4SrcCntCr = IMGRZ_READ32(u4RegOffset);
    u4RegOffset = ((RZ_REG_VWROFTY << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VWrOftY = IMGRZ_READ32(u4RegOffset);    
    u4RegOffset = ((RZ_REG_VWROFTCB << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VWrOftCb = IMGRZ_READ32(u4RegOffset);    
    u4RegOffset = ((RZ_REG_VWROFTCR << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VWrOftCr = IMGRZ_READ32(u4RegOffset);
    u4RegOffset = ((RZ_REG_VNEXTCOEFY << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VNextCoefY = IMGRZ_READ32(u4RegOffset);    
    u4RegOffset = ((RZ_REG_VNEXTCOEFCB << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VNextCoefCb = IMGRZ_READ32(u4RegOffset);    
    u4RegOffset = ((RZ_REG_VNEXTCOEFCR << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VNextCoefCr = IMGRZ_READ32(u4RegOffset); 
}

void IMGRZ_HwResume(void)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    IMGRZ_HwReset();

    u4RegOffset = ((RZ_REG_SRCCNTY << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4SrcCntY);
    u4RegOffset = ((RZ_REG_SRCCNTCB << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4SrcCntCb);
    u4RegOffset = ((RZ_REG_SRCCNTCR << 2) & IMGRZ_REG_MASK);    
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4SrcCntCr);    
    u4RegOffset = ((RZ_REG_VWROFTY << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4VWrOftY);    
    u4RegOffset = ((RZ_REG_VWROFTCB << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4VWrOftCb);    
    u4RegOffset = ((RZ_REG_VWROFTCR << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4VWrOftCr);    
    u4RegOffset = ((RZ_REG_VNEXTCOEFY << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4VNextCoefY);    
    u4RegOffset = ((RZ_REG_VNEXTCOEFCB << 2) & IMGRZ_REG_MASK); 
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4VNextCoefCb);    
    u4RegOffset = ((RZ_REG_VNEXTCOEFCR << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4RegOffset, _rTempData.u4VNextCoefCr);

    u4RegOffset = ((RZ_REG_RESUME << 2) & IMGRZ_REG_MASK);
    u4Value = 0x1;
    IMGRZ_WRITE32(u4RegOffset, u4Value);
}

void IMGRZ_HwPowerDown(void)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);  
    IMGRZ_WRITE32(u4RegOffset, 0);
}

