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
#include "x_typedef.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "x_chip_id.h"
#include "x_gpio.h"
#include "drvcust_if.h"
#include "x_os.h"
#include "x_assert.h"

#ifndef CC_MTK_LOADER
static HANDLE_T _hGcpuHwSema = NULL_HANDLE;
static HANDLE_T _hGcpuIrqSema = NULL_HANDLE;


void _Bim_TrngLock(void)
{
    VERIFY(x_sema_lock(_hGcpuHwSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}


void _Bim_TrngUnlock(void)
{
    VERIFY(x_sema_unlock(_hGcpuHwSema) == OSR_OK);
}


static void GCPU_IRQ_LOCK(void)
{
    VERIFY(x_sema_lock(_hGcpuIrqSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}


static void GCPU_IRQ_UNLOCK(void)
{
    VERIFY(x_sema_unlock(_hGcpuIrqSema) == OSR_OK);
}


static VOID _Gcpu_IrqHandle(UINT16 u2Vector)
{

    ASSERT(u2Vector == VECTOR_TRNG);
    if (!BIM_IsIrqPending(VECTOR_TRNG))
    {
        return;
    }

#if !defined(__linux__)
    // Check if it's aes interrupt
    while (BIM_IsIrqPending(VECTOR_TRNG))
#endif
    {
        // Clear interrupt
        TRNG_WRITE32(TRNG_INT_CLR, TRNG_READ32(TRNG_INT_CLR));
        VERIFY(BIM_ClearIrq(VECTOR_TRNG));

        GCPU_IRQ_UNLOCK();
    }
}



static BOOL _Bim_TrngInit(void)
{
    static BOOL isInit = FALSE;
    x_os_isr_fct pfnOldIsr;

    if(!isInit)
    {
        // create a semaphore
        if (NULL_HANDLE == _hGcpuHwSema)
        {
            if(x_sema_create(&_hGcpuHwSema, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) != OSR_OK)
            {
                Printf("TRNG: failed to create _hGcpuHwSema...\n");
                return FALSE;
            }
        }

        if (NULL_HANDLE == _hGcpuIrqSema)
        {
            if(x_sema_create(&_hGcpuIrqSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
            {
                Printf("TRNG: failed to create _hGcpuIrqSema...\n");
                return FALSE;
            }
        }

        BIM_ClearIrq(VECTOR_TRNG);
        if (x_reg_isr(VECTOR_TRNG, _Gcpu_IrqHandle, &pfnOldIsr) != OSR_OK)
        {
            Printf("Error: fail to register TRNG ISR!\n");
        }
        isInit = TRUE;
    }

    UNUSED(pfnOldIsr);

    return TRUE;
}


UINT32 BIM_Random(VOID)
{
    UINT32 u4Value;

    _Bim_TrngInit();
    _Bim_TrngLock();

    TRNG_WRITE32(TRNG_CTRL, TRNG_READ32(TRNG_CTRL) | TRNG_CTRL_START);
    #if 1
    GCPU_IRQ_LOCK(); // interrupt mode
    #else
    while (0 == ((TRNG_READ32(TRNG_CTRL)) & TRNG_CTRL_RDY)); //polling mode
    #endif
    u4Value = TRNG_READ32(TRNG_DATA);
    TRNG_WRITE32(TRNG_CTRL, TRNG_READ32(TRNG_CTRL) & ~TRNG_CTRL_START);

    _Bim_TrngUnlock();

    return u4Value;
}

#endif // CC_MTK_LOADER


