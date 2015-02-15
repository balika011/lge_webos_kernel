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
 * $RCSfile: hal.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file hal_irq_gic.c
 *  Hardware abstraction rountines for IRQ related function
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_bim.h"
#define __USE_XBIM_IRQS        // UGLY HACK! Make sure we don't include mach/irqs.h from kernel.

#include "hal.h"
#include "x_os.h"
#include "x_assert.h"
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/semaphore.h>
#include "x_linux.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#ifdef CONFIG_ARM_GIC
#if defined(CONFIG_ARCH_MT5882)
#define SPI_OFFSET      64
#else
#define SPI_OFFSET      32
#endif
#else
#define SPI_OFFSET      0
#endif

#ifdef CC_UBOOT
#error This hal should not be used by uboot.
#endif

extern spinlock_t mt53xx_bim_lock;

#ifdef CC_ARM_GIC
// This bitfield maintains IRQ enabled status. We add this because device tree uses irqchip driver without BIM operations.
// We enabled all BIM IRQs in loader, and it will always return IRQ enabled if query its status by BIM register, even the IRQ is not really enabled in GIC.
static UINT32 u4IrqEnabled[4] = {0, 0, 0, 0};
#endif

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

static BOOL _IsVectorValid(UINT32 u4Vector)
{
    return (u4Vector <= MAX_IRQ_VECTOR);
}

//-----------------------------------------------------------------------------
// IRQs
//-----------------------------------------------------------------------------

BOOL BIM_EnableIrq(UINT32 u4Vector)
{
    #ifdef CC_ARM_GIC
    UINT32 u4BitMask = 0;
    #endif

    if (!_IsVectorValid(u4Vector))
    {
        return FALSE;
    }

    #ifdef CC_ARM_GIC
    u4BitMask = 1U << (u4Vector % 32);
    u4IrqEnabled[u4Vector/32] |= u4BitMask;
    #endif

    enable_irq(u4Vector+SPI_OFFSET);
    return TRUE;
}

BOOL BIM_DisableIrq(UINT32 u4Vector)
{
    #ifdef CC_ARM_GIC
    UINT32 u4BitMask = 0;
    #endif

    if (!_IsVectorValid(u4Vector))
    {
        return FALSE;
    }

    #ifdef CC_ARM_GIC
    u4BitMask = ~(1U << (u4Vector % 32));
    u4IrqEnabled[u4Vector/32] &= u4BitMask;
    #endif

    disable_irq(u4Vector+SPI_OFFSET);
    return TRUE;
}

BOOL BIM_IsIrqEnabled(UINT32 u4Vector)
{
#ifdef CC_ARM_GIC
    UINT32 u4BitMask = 0;
    u4BitMask = 1U << (u4Vector % 32);
    return ((u4IrqEnabled[u4Vector/32] & u4BitMask) != 0)?TRUE:FALSE;
#else // CC_ARM_GIC
    UINT32 u4State;

    if (!_IsVectorValid(u4Vector))
    {
        return FALSE;
    }

    #ifdef VECTOR_MISC2_BASE
    if (u4Vector >= VECTOR_MISC2_BASE)
    {
        u4State = BIM_READ32(REG_MISC2_IRQEN);

        return (u4State & _MISC2IRQ(u4Vector)) != 0;
    }
    #endif

    if (u4Vector > VECTOR_MISC)
    {
        u4State = BIM_READ32(REG_MISC_IRQEN);

        return (u4State & _MISCIRQ(u4Vector)) != 0;
    }

    u4State = BIM_READ32(REG_IRQEN);

    return (u4State & _IRQ(u4Vector)) != 0;
#endif // CC_ARM_GIC
}

BOOL BIM_IsIrqPending(UINT32 u4Vector)
{
    UINT32 u4State;

    if (!_IsVectorValid(u4Vector))
    {
        return FALSE;
    }

#ifdef VECTOR_MISC2_BASE
    if (u4Vector >= VECTOR_MISC2_BASE)
    {
        u4State = BIM_READ32(REG_MISC2_IRQST);

        return (u4State & _MISC2IRQ(u4Vector)) != 0;
    }
#endif

    if (u4Vector > VECTOR_MISC)
    {
        u4State = BIM_READ32(REG_MISC_IRQST);
        return (u4State & _MISCIRQ(u4Vector)) != 0;
    }

    u4State = BIM_READ32(REG_IRQST);

    return (u4State & (1 << u4Vector)) != 0;
}

BOOL BIM_ClearIrq(UINT32 u4Vector)
{
    UINT32 u4Irq, u4Reg;
    UINT32 u4State;

    if (!_IsVectorValid(u4Vector))
    {
        return FALSE;
    }

#ifdef VECTOR_MISC2_BASE
    if (u4Vector >= VECTOR_MISC2_BASE)
    {
        u4Irq = _MISC2IRQ(u4Vector);
        u4Reg = REG_MISC2_IRQCLR;
    }
    else // fall-thru
#endif
    if (u4Vector > VECTOR_MISC)
    {
        u4Irq = _MISCIRQ(u4Vector);
        u4Reg = REG_MISC_IRQCLR;
    }
    else
    {
        u4Irq = _IRQ(u4Vector);
        u4Reg = REG_IRQCL;
    }

    spin_lock_irqsave(&mt53xx_bim_lock, u4State);
    BIM_WRITE32(u4Reg, u4Irq);
    spin_unlock_irqrestore(&mt53xx_bim_lock, u4State);

    return TRUE;
}

//-----------------------------------------------------------------------------
/** HalDisableIRQ() Disable IRQ
 */
//-----------------------------------------------------------------------------
void HalDisableIRQ(void)
{
    ASSERT(0);
}

//-----------------------------------------------------------------------------
/** HalEnableFIQ() Enable FIQ
 */
//-----------------------------------------------------------------------------
void HalEnableFIQ(void)
{
    ASSERT(0);
}

void HalDisableFIQ(void)
{
    ASSERT(0);
}

