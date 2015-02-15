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
 * $Date  $
 * $RCSfile: drv_pwm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "drv_pwm.h"
#include "x_hal_5381.h"
#include "x_pinmux.h"
#include "x_assert.h"
#include "drvcust_if.h"


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief
 * This driver control the PWM0 & PWM1 output frequence and duty cycle
 * duty cycle = bDutyON/bDutyAll;
 * PWM freq.(in 8202) = 27MHz/(bDutyAll * bTimeBase * 8 clk)
 * PWM freq.(in 5371) = 125MHz/(bDutyAll+1 * bTimeBase+1)
 * PWM freq.(in 538x) = (CPUclk/2) / ((PWMP+1)x(PWMRSN+1))
 *
 * @param bSrc      PWM source (SrcPWM0/1/2)
 * @param bTimeBase PWMP, PWM basic unit = (0~255) * 8 clk
 * @param bDutyON   PWMH, ON duty width = (0~255) * bTimeBase
 * @param bDutyAll  PWMRSN, Period width = (0~255) * bTimeBase
 * @return void
 *
 * @example  vDrvSetPWM(SrcPWM0, 6, u4DutyON, 255);
 *           Use PWM0 with 2000Hz; Duty Cycle = u4DutyON/255
 */

void vDrvSetPWM(UINT32 u4Src, UINT32 u4TimeBase, UINT32 u4DutyON,
                UINT32 u4DutyAll)
{
    UINT32 u4Reg = REG_RW_PWM1, u4Value;
#if defined(CC_MT5387) && !defined(CC_MT5363) // 5387 use function1
    UINT32 u4FunctionSel = PINMUX_FUNCTION1;
#else // 5363 use function0
    UINT32 u4FunctionSel = PINMUX_FUNCTION0;
#endif

    switch (u4Src)
    {
    case SrcPWM0:
        u4Reg = REG_RW_PWM0;
#if defined(CC_MT8223)
        UNUSED(BSP_PinSet(OPWM1, PINMUX_FUNCTION0));
#elif defined(CC_MT5387) && !defined(CC_MT5363)
        UNUSED(BSP_PinSet(OPWM2, u4FunctionSel));
#else /* defined(CC_MT5387) && !defined(CC_MT5363) */
        UNUSED(BSP_PinSet(OPWM0, u4FunctionSel));
#endif /* defined(CC_MT5387) && !defined(CC_MT5363) */
        break;
    case SrcPWM1:
        u4Reg = REG_RW_PWM1;
        if (DRVCUST_PanelGet(ePanelBacklightGPIO))
        {
#if !defined(CC_MT8223)
            UNUSED(BSP_PinSet(PINMUX_GPIO_00, u4FunctionSel));
#endif
        }
        else
        {
#if defined(CC_MT8223)
            UNUSED(BSP_PinSet(OPWM2, PINMUX_FUNCTION0));
#else
            UNUSED(BSP_PinSet(OPWM1, u4FunctionSel));
#endif
        }
        break;
    case SrcPWM2:
        u4Reg = REG_RW_PWM2;
#if defined(CC_MT8223)
        UNUSED(BSP_PinSet(OPWM3, PINMUX_FUNCTION0));
#elif defined(CC_MT5387) && !defined(CC_MT5363)
        UNUSED(BSP_PinSet(OPWM0, u4FunctionSel));
#else /* defined(CC_MT5387) && !defined(CC_MT5363) */
        UNUSED(BSP_PinSet(OPWM2, u4FunctionSel));
#endif /* defined(CC_MT5387) && !defined(CC_MT5363) */
        break;
    default:
        ASSERT(u4Src <= SrcPWM2);
    }
#if 1
    // workaround for full duty drop issue
    if (u4DutyAll==0xff && u4DutyON==0xff)
    {
        u4DutyAll = 0xfe;
    }
#endif
    u4Value = (((UINT32)u4DutyAll << 24) | ((UINT32)u4DutyON << 16) |
               ((UINT32)(u4TimeBase&0xFFF) << 4) | 1);
    IO_WRITE32(BIM_BASE, u4Reg, u4Value);

    if (u4Src==SrcPWM1)
    {
        IO_WRITE32MSK(BIM_BASE, REG_RW_PWMLVDS, (u4TimeBase&0xF000)>>12, 0x00FF);
    }
    else if  (u4Src==SrcPWM2)
    {
        IO_WRITE32MSK(BIM_BASE, REG_RW_PWMLVDS, (u4TimeBase&0xF000)<<4, 0xFF0000);
    }
}

/**
 * Set PWM sync to LVDS output v-sync signal.
 *
 * @param u4Src      PWM source (SrcPWM0/1/2)
 * @param u4LockON   Lock PWM on LVDS v-sync or not.
 */

void vDrvSetLock(UINT32 u4Src, UINT32 u4LockON)
{
    switch (u4Src)
    {
    default:
    case SrcPWM0:
        // Don't support PWM lock.
        break;
    case SrcPWM1:
        IO_WRITE32MSK(
            BIM_BASE,
            REG_RW_PWMLVDS,
            u4LockON? 0x8000 : 0,
            0xC000);
        break;
    case SrcPWM2:
        IO_WRITE32MSK(
            BIM_BASE,
            REG_RW_PWMLVDS,
            u4LockON? 0x80000000 : 0,
            0xC0000000);
        break;
    }
}

