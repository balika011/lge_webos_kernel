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
 * $RCSfile: dmx_emu_ddi_api.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_ddi.c
 *  Demux driver - DDI interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_mm.h"
#include "fvr.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_hal_arm.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_assert.h"
#include "x_os.h"

#define DEFINE_IS_LOG    DMX_IsLog
#include "x_debug.h"

LINT_EXT_HEADER_END

extern void _DDI_Reset(BOOL fgResetHardware);

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MAX_WAIT_COUNT                  10000

#define CYCLES_PER_MS                   312000


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static void _Delay(UINT32 u4Ms)
{
    x_thread_delay(u4Ms);
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

static BOOL _DDI_Transfer0(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp;
    BOOL fgRet;

    if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        return FALSE;
    }

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4Wp = _DMX_Align(u4Addr + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    DMXCMD_WRITE32(49, 0); //0xf00170c4 FTUP TS PROCESSING RATE REGISTER 1
    DMXCMD_WRITE32(51, 0); //0xf00170cc FTUP TS PROCESSING RATE REGISTER 2

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;
#if 0
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            // Complete
            fgRet = TRUE;
            Printf("   Wait count: %u\n", i);
            break;
        }

//        VERIFY(DMX_DRAMMeasure(DMX_LOCAL_ARBITOR_DDI, 500));
        _Delay(1);
    }
#else
    {
        HAL_TIME_T rTime1, rTime2, rTimeDiff;

        HAL_GetTime(&rTime1);
        while (1)
        {
            UINT32 u4Rp;

            u4Rp = DDI_READ32(DDI_REG_DMA_RP);
            if (u4Rp == u4Wp)
            {
                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            //_Delay(1);
        }
        HAL_GetTime(&rTime2);
        HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
        LOG(3, "DDI time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);
    }
#endif

    return fgRet;
}


static BOOL _DDI_Transfer1(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, i;
    BOOL fgRet = FALSE;

    //if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    //{
    // Incorrect address alignment
    //    return FALSE;
    //}

    //if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    //{
    // Incorrect size alignment
    //    return FALSE;
    //}

    u4BufStart = PHYSICAL(u4Addr);
    u4BufEnd = _DMX_Align(u4Addr + u4Size + 1, DDI_BUF_ALIGNMENT);
    u4Wp = u4BufStart;

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        // Randomly generate step size
        u4StepSize = _DMX_Align((UINT32)random((INT32)u4Size) + 1,
                                DDI_POINTER_ALIGNMENT);
        u4Wp += u4StepSize;
        Printf("    Step size: %u, ", u4StepSize);

        // Update write pointer
        DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);

        // Waiting for completion by comparing RP with WP
        fgRet = FALSE;
        for (i = 0; i < MAX_WAIT_COUNT; i++)
        {
            UINT32 u4Rp;

            u4Rp = DDI_READ32(DDI_REG_DMA_RP);
            if (u4Rp == u4Wp)
            {
                // Complete
                fgRet = TRUE;
                Printf("wait count: %u\n", i);
                break;
            }

            _Delay(1);
        }

        if (!fgRet)
        {
            return FALSE;
        }

        u4Size -= u4StepSize;
    }

    return fgRet;
}

#define DDITEST2_RESET
static BOOL _DDI_Transfer2(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, u4Rp, u4BufSize, u4DataSize;
    UINT32 u4EmptySize,i;
    BOOL fgRet;
    UINT32 u4Reg;

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    // Get buffer status
    u4BufStart = DDI_READ32(DDI_REG_DMA_BUF_START);
    u4BufEnd = DDI_READ32(DDI_REG_DMA_BUF_END);
    u4Rp = DDI_READ32(DDI_REG_DMA_RP);
    u4Wp = DDI_READ32(DDI_REG_DMA_WP);
    u4BufSize = u4BufEnd - u4BufStart;
    u4DataSize = DATASIZE(u4Rp, u4Wp, u4BufSize);
    u4EmptySize = u4BufSize - u4DataSize;

    // Check if buffer is available
    if (u4EmptySize < u4Size)
    {
        return FALSE;
    }

    // Copy data to DDI buffer
    if (!_DMX_CopyDestRingBuffer(u4Wp, u4BufStart, u4BufEnd, u4Addr, u4Size))
    {
        return FALSE;
    }

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

#ifdef DDITEST2_RESET
    DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0);
    DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0x208);

    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4Wp);

    // after ddi soft reset ,Pkt quadbyte limit need set again  for mt5398
    u4Reg = DDI_READ32(DDI_REG_PKT_QUADBYTE_LIMIT);
    u4Reg = (u4Reg & 0xFFFFFF00) | (188 >> 2);
    DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u4Reg);
#endif


    // Update write pointer
    u4Wp += u4Size;
    if (u4Wp >= u4BufEnd)
    {
        u4Wp -= u4BufSize;
    }
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    Printf("DDI wp:0x%x\n", u4Wp);

#ifdef DDITEST2_RESET
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);
#endif

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            _Delay(1);
            // Complete
            fgRet = TRUE;
            Printf("   Wait count: %u\n", i);
            break;
        }

        _Delay(1);
    }
#ifdef DDITEST2_RESET
    HAL_Delay_us(1);
    DDI_WRITE32(DDI_REG_DMA_CTRL, 4);
#endif

    return fgRet;
}


static BOOL _DDI_Transfer3(UINT32 u4Addr, UINT32 u4Size)
{
    PID_STRUCT_T* prPidStruct;
    UINT32 u4BufStart, u4BufEnd, u4Wp, u4Rp, u4RpBound, i;
    UINT32 u4DstBufStart, u4DstBufEnd, u4DstRp, u4DstWp;
    BOOL fgRet;
    const UINT8 u1Pidx = 0;

    if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        return FALSE;
    }

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4Wp = _DMX_Align(u4Addr + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Set read pointer to make buffer full
    u4DstBufStart = VIRTUAL(PID_S_W(u1Pidx, 5));
    u4DstBufEnd = VIRTUAL(PID_S_W(u1Pidx, 6));
    u4DstWp = VIRTUAL(PID_S_W(u1Pidx, 8));
    ASSERT(u4DstWp == u4DstBufStart);
    u4DstRp = u4DstWp + DEF_BUF_FULL_GAP;

    // Debug
    Printf("Dst buffer: 0x%08x - 0x%08x, RP: 0x%08x, WP: 0x%08x\n",
           u4DstBufStart, u4DstBufEnd, u4DstRp, u4DstWp);

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    _DMX_Lock();
    PID_S_W(u1Pidx, 9) = PHYSICAL(u4DstRp);
    prPidStruct->u4Rp = u4DstRp;
    _DMX_Unlock();

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    u4RpBound = u4BufStart + TS_RP_BOUND + DEF_BUF_FULL_GAP;
    u4Rp = DDI_READ32(DDI_REG_DMA_RP);
    if (u4Rp > u4RpBound)
    {
        return FALSE;
    }

    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        // Debug
        Printf("  Dst RP: 0x%08x, WP: 0x%08x,", u4DstRp, u4DstWp);

        // Randomly generate step size
        u4StepSize = _DMX_Align((UINT32)random((INT32)u4Size) + 1,
                                DDI_POINTER_ALIGNMENT);
        Printf("    Step size: %u\n", u4StepSize);

        // Update read pointer of destination buffer
        u4DstRp += u4StepSize;
        if (!_DMX_UpdateReadPointer2(u1Pidx, u4DstRp, 0))
        {
            return FALSE;
        }

        // Wait for a moment
        _Delay(1);

        // Check if destination buffer wp exceeds rp
        u4DstWp = VIRTUAL(PID_S_W(u1Pidx, 8));
        if (u4DstWp >= u4DstRp)
        {
            return FALSE;
        }

        // Check if source read pointer exceeds step size
        u4RpBound += u4StepSize;
        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        /*
                // Fixme
                if (u4Rp > u4RpBound)
                {
                    return FALSE;
                }
        */
        u4Size -= u4StepSize;
    }

    // Update read pointer of destination buffer to buffer end to complete
    // transferring
    if (!_DMX_UpdateReadPointer2(u1Pidx, u4DstBufEnd - 1, 0))
    {
        return FALSE;
    }

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            // Complete
            fgRet = TRUE;
            Printf("    Wait count: %u\n", i);
            break;
        }

        _Delay(1);
    }

    return fgRet;
}


static BOOL _DDI_Transfer4(UINT32 u4Addr, UINT32 u4Size)
{
    PID_STRUCT_T* prPidStruct;
    UINT32 u4BufStart, u4BufEnd, u4BufSize, u4DataSize, u4Wp, u4Rp, u4RpBound;
    UINT32 u4DstBufStart, u4DstBufEnd, u4DstBufSize, u4DstRp, u4DstWp;
    UINT32 /*u4DstDataSize, */u4EmptySize, i;
    BOOL fgRet;
    const UINT8 u1Pidx = 0;

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    // Get buffer status
    u4BufStart = DDI_READ32(DDI_REG_DMA_BUF_START);
    u4BufEnd = DDI_READ32(DDI_REG_DMA_BUF_END);
    u4Rp = DDI_READ32(DDI_REG_DMA_RP);
    u4Wp = DDI_READ32(DDI_REG_DMA_WP);
    u4BufSize = u4BufEnd - u4BufStart;
    u4DataSize = DATASIZE(u4Rp, u4Wp, u4BufSize);
    u4EmptySize = u4BufSize - u4DataSize;

    // Check if buffer is available
    if (u4EmptySize < u4Size)
    {
        return FALSE;
    }

    // Copy data to DDI buffer
    if (!_DMX_CopyDestRingBuffer(u4Wp, u4BufStart, u4BufEnd, u4Addr, u4Size))
    {
        return FALSE;
    }

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    u4DstBufStart = prPidStruct->u4BufStart;
    u4DstBufEnd = prPidStruct->u4BufEnd + 1;
    u4DstBufSize = u4DstBufEnd - u4DstBufStart;
    while (1)
    {
        UINT32 u4DstRp2;

        u4DstRp = PID_S_W(u1Pidx, 9);
        u4DstRp2 = PID_S_W(u1Pidx, 9);
        if (u4DstRp == u4DstRp2)
        {
            break;
        }
    }
    while (1)
    {
        UINT32 u4DstWp2;

        u4DstWp = PID_S_W(u1Pidx, 8);
        u4DstWp2 = PID_S_W(u1Pidx, 8);
        if (u4DstWp == u4DstWp2)
        {
            break;
        }
    }

    Printf("Src buffer: 0x%08x - 0x%08x, RP: 0x%08x, WP: 0x%08x\n",
           u4BufStart, u4BufEnd, u4Rp, u4Wp);
    Printf("Dst buffer: 0x%08x - 0x%08x, RP: 0x%08x, WP: 0x%08x\n",
           u4DstBufStart, u4DstBufEnd, u4DstRp, u4DstWp);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Update write pointer
    u4RpBound = u4BufStart + TS_RP_BOUND + DEF_BUF_FULL_GAP;
    if (u4RpBound >= u4BufEnd)
    {
        u4RpBound -= u4BufSize;
    }
    u4Wp += u4Size;
    if (u4Wp >= u4BufEnd)
    {
        u4Wp -= u4BufSize;
    }
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        // Randomly generate step size
        u4StepSize = _DMX_Align((UINT32)random((INT32)u4Size) + 1,
                                DDI_POINTER_ALIGNMENT);
        Printf("    Step size: %u\n", u4StepSize);

        // Update read pointer of destination buffer
        u4DstRp += u4StepSize;
        if (u4DstRp >= u4DstBufEnd)
        {
            u4DstRp -= u4DstBufSize;
        }
        if (!_DMX_UpdateReadPointer2(u1Pidx, u4DstRp, 0))
        {
            return FALSE;
        }

        // Wait for a moment
        _Delay(1);

        // Check if destination buffer wp exceeds rp
        while (1)
        {
            UINT32 u4DstWp2;

            u4DstWp = VIRTUAL(PID_S_W(u1Pidx, 8));
            u4DstWp2 = VIRTUAL(PID_S_W(u1Pidx, 8));
            if (u4DstWp == u4DstWp2)
            {
                break;
            }
        }
        /*
                u4DstDataSize = DATASIZE(u4DstRp, u4DstWp, u4DstBufSize);
                if (u4DstDataSize < (u4DstBufSize / 2))
                {
                    return FALSE;
                }
        */
        /*
                // Check if source read pointer exceeds step size
                u4RpBound += u4StepSize;
                if (u4RpBound >= u4BufEnd)
                {
                    u4RpBound -= u4BufSize;
                }
                u4Rp = DDI_READ32(DDI_REG_DMA_RP);
                if (DATASIZE(u4Rp, u4Wp, u4BufSize) < DATASIZE(u4RpBound, u4Wp, u4BufSize))
                {
                    return FALSE;
                }
        */
        u4Size -= u4StepSize;
    }
    /*
        // Update read pointer of destination buffer to buffer end to complete
        // transferring
        if (!_DMX_UpdateReadPointer2(u1Pidx, u4DstBufEnd - 1, 0))
        {
            return FALSE;
        }
    */
    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            // Complete
            fgRet = TRUE;
            Printf("    Wait count: %u\n", i);
            break;
        }

        _Delay(1);
    }

    return fgRet;
}

static BOOL _DDI_Transfer5(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp;
    BOOL fgRet;

    if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        return FALSE;
    }

#if 0
    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }
#endif

    u4BufStart = PHYSICAL(u4Addr);
    u4Wp = u4Addr + u4Size ;
    //u4Wp = _DMX_Align(u4Addr + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    DMXCMD_WRITE32(49, 0); //0xf00170c4 FTUP TS PROCESSING RATE REGISTER 1
    DMXCMD_WRITE32(51, 0); //0xf00170cc FTUP TS PROCESSING RATE REGISTER 2

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;
#if 0
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            // Complete
            fgRet = TRUE;
            Printf("   Wait count: %u\n", i);
            break;
        }

//        VERIFY(DMX_DRAMMeasure(DMX_LOCAL_ARBITOR_DDI, 500));
        _Delay(1);
    }
#else
    {
        HAL_TIME_T rTime1, rTime2, rTimeDiff;

        HAL_GetTime(&rTime1);
        while (1)
        {
            UINT32 u4Rp;

            u4Rp = DDI_READ32(DDI_REG_DMA_REAL_RP);
            if (u4Rp == u4Wp)
            {
                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            //_Delay(1);
        }
        HAL_GetTime(&rTime2);
        HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
        LOG(3, "DDI time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);
    }
#endif

    return fgRet;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL _DMX_EMU_DDI_Transfer(UINT8 u1Type, UINT32 u4Addr, UINT32 u4Size)
{
    BOOL fgRet;

    switch (u1Type)
    {
    case 0:
        fgRet = _DDI_Transfer0(u4Addr, u4Size);
        break;

    case 1:
        fgRet = _DDI_Transfer1(u4Addr, u4Size);
        break;

    case 2:
        fgRet = _DDI_Transfer2(u4Addr, u4Size);
        break;

    case 3:
        fgRet = _DDI_Transfer3(u4Addr, u4Size);
        break;

    case 4:
        fgRet = _DDI_Transfer4(u4Addr, u4Size);
        break;

    case 5:
        fgRet = _DDI_Transfer5(u4Addr, u4Size);
        break;

    default:
        fgRet = FALSE;
    }

    // Make sure data really output to DRAM
    x_thread_delay(10);

    if (!fgRet)
    {
        Printf("DDI transfer fail!\n");
    }

    return fgRet;
}


BOOL _DMX_EMU_DDI_SetBuffer(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd;

    if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        return FALSE;
    }

    if (!_DMX_IsAligned(u4Size, DDI_BUF_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4BufEnd = u4BufStart + u4Size;

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4BufStart);            // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    return TRUE;
}

BOOL _DMX_EMU_DDI_130byteEnable(BOOL fgEnable, UINT8 u1PktSize)
{
    UINT32 u4Reg;
    if (fgEnable)
    {
        //set limit register
        u4Reg = DDI_READ32(DDI_REG_PKT_QUADBYTE_LIMIT);
        u4Reg = (u4Reg & 0xFFFFFF00) | u1PktSize;
        DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u4Reg);

        //set quad limit register as pkt size meaning
        //enable ddi 130 byte transfer
        u4Reg = DDI_READ32(DDI_REG_DMX_RX_CTRL);
        u4Reg |= (0x3 << 6);
        DDI_WRITE32(DDI_REG_DMX_RX_CTRL, u4Reg);
    }
    else
    {
        //set limit register
        u4Reg = DDI_READ32(DDI_REG_PKT_QUADBYTE_LIMIT);
        u4Reg = (u4Reg & 0xFFFFFF00) | (u1PktSize >> 2);
        DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u4Reg);

        //set quad limit register / 4  as pkt size meaning
        //disable ddi 130 byte transfer
        u4Reg = DDI_READ32(DDI_REG_DMX_RX_CTRL);
        u4Reg &= ~(0x3 << 6);
        DDI_WRITE32(DDI_REG_DMX_RX_CTRL, u4Reg);
    }

    return TRUE;

}

BOOL _DMX_EMU_PalyBack_130byteEnable(BOOL fgEnable, UINT8 u1PktSize)
{
    UINT32 u4Reg;
    if (fgEnable)
    {
        //set input packet size register 0x17908 [31:24] for 130 packet size
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CTRL2);
        u4Reg &= ~((UINT32)0xFF << 24);
        u4Reg |= (u1PktSize << 24);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL2, u4Reg);

        //set output packet size register 0x17904 [31:24]
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CTRL1);
        u4Reg &= ~((UINT32)0xFF << 24);
        u4Reg |= (u1PktSize << 24);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, u4Reg);

        //set input output enable .and playback 130 byte enable
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CTRL1);
        u4Reg |= (0x3 << 5);
        u4Reg |= (0x1 << 3);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, u4Reg);

    }
    else
    {
        //set input packet size register 0x17908 [31:24] for 188 packet size
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CTRL2);
        u4Reg &= ~((UINT32)0xFF << 24);
        u4Reg |= (u1PktSize << 24);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL2, u4Reg);

        //set output packet size register 0x17904 [31:24]
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CTRL1);
        u4Reg &= ~((UINT32)0xFF << 24);
        u4Reg |= (u1PktSize << 24);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, u4Reg);

        //set input output enable .and playback 130 byte enable
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CTRL1);
        u4Reg &= ~(0x3 << 5);
        u4Reg &= ~(0x1 << 3);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, u4Reg);
    }

    return TRUE;

}


BOOL _DMX_EMU_Framer_130byteEnable(UINT8 u1Framer, BOOL fgDemod130byteTs, BOOL fgInputEnable, UINT8 u1InputPktSize ,BOOL fgOutputEnable, UINT8 u1OutputPktSize)
{
    UINT32 u4Reg;

    ASSERT(u1Framer < 4);

    if (u1Framer == 2)
    {
        Printf("Framer 2 no need set 130 byte enable!\n");
        return TRUE;
    }
    else if (u1Framer == 0)
    {
        //set input packet size
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL);
        u4Reg &= ~(0xFF);
        u4Reg |= u1InputPktSize;
        DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);

        //set input enable
        if (fgInputEnable)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL);
            u4Reg |= (0x1 << 8);
            DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL);
            u4Reg &= ~(0x1 << 8);
            DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);
        }

        //set output packet size
        u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
        u4Reg &= ~(0xFF);
        u4Reg |= (u1OutputPktSize);
        DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);

        //set output enable  and dbm auto switch
        if (fgOutputEnable)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg |= (0x1 << 24); //output enable
            u4Reg |= (0x1 << 27); //dbm auto switch
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg &= ~(0x1 << 24);
            u4Reg &= ~(0x1 << 27);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }

        //set Demod 130byte ts enable or disable 17920[28]
        if (fgDemod130byteTs)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg |= (0x1 << 28);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg &= ~(0x1 << 28);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }

    }
    else if (u1Framer == 1)
    {
        //set input packet size
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL);
        u4Reg &= ~(0xFF << 16);
        u4Reg |= (u1InputPktSize << 16);
        DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);

        //set input enable
        if (fgInputEnable)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL);
            u4Reg |= (0x1 << 24);
            DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL);
            u4Reg &= ~(0x1 << 24);
            DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);
        }

        //set output packet size
        u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
        u4Reg &= ~(0xFF << 8);
        u4Reg |= (u1OutputPktSize << 8);
        DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);

        //set output enable  and dbm auto switch
        if (fgOutputEnable)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg |= (0x1 << 25); //output enable
            u4Reg |= (0x1 << 27); //dbm auto switch
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg &= ~(0x1 << 25);
            u4Reg &= ~(0x1 << 27);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }

        //set Demod 130byte ts enable or disable 17920[29]
        if (fgDemod130byteTs)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg |= (0x1 << 29);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg &= ~(0x1 << 29);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }

    }
    else if (u1Framer == 3)
    {
        //set input packet size
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL2);
        u4Reg &= ~(0xFF);
        u4Reg |= (u1InputPktSize);
        DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL2, u4Reg);

        //set input enable
        if (fgInputEnable)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL2);
            u4Reg |= (0x1 << 8);
            DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL2, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL2);
            u4Reg &= ~(0x1 << 8);
            DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL2, u4Reg);
        }

        //set output packet size
        u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
        u4Reg &= ~(0xFF << 16);
        u4Reg |= (u1OutputPktSize << 16);
        DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);

        //set output enable  and dbm auto switch
        if (fgOutputEnable)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg |= (0x1 << 26); //output enable
            u4Reg |= (0x1 << 27); //dbm auto switch
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg &= ~(0x1 << 26);
            u4Reg &= ~(0x1 << 27);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }

        //set Demod 130byte ts enable or disable 17920[30]
        if (fgDemod130byteTs)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg |= (0x1 << 30); //output enable
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }
        else
        {
            u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
            u4Reg &= ~(0x1 << 30);
            DMXCMD_WRITE32(DMX_REG_CONFIG4, u4Reg);
        }

    }
    else
    {
        Printf("Input the wrong Framer number!\n");
        return FALSE;
    }

    return TRUE;

}

BOOL _DMX_EMU_DDI_Init(UINT8 u1TsIndex)
{
    UINT32 u4Reg, i;
    DMX_DBM_INPUT_SOURCE_T eSource;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    // Clear all interrupts
    for (i = 0; i < DMX_INT_QUEUE_DEPTH; i++)
    {
        DMXCMD_WRITE32(DMX_REG_DBM_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_DBM_NONERR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_DESCRAMBLER_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_DESCRAMBLER_NONERR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_STEER_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_STEER_NONERR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_FTuP_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_FTuP_NONERR_STATUS_REG1, 1);
        DMXCMD_WRITE32(DMX_REG_PCR_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_PCR_NONERR_STATUS_REG1, 1);
    }

    // DDI soft reset
    DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0);
    x_thread_delay(10);//delay 100 ms
    DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0x208);

    // after ddi soft reset ,Pkt quadbyte limit need set again
    u4Reg = DDI_READ32(DDI_REG_PKT_QUADBYTE_LIMIT);
    u4Reg = (u4Reg & 0xFFFFFF00) | (188 >> 2);
    DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u4Reg);

    // Set to full pull mode, DMA burst size
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffff00;
    u4Reg |= (3U << 30);    // 0: full push, 1: half pull, 3: full pull
    u4Reg |= 8;             // DMA burst size = 8
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    // Enable "DMB input full" signal to TS input device
    //DMXCMD_WRITE32(DMX_REG_DBM_BUF_CTRL, 0);
    u4Reg = DMXCMD_READ32(DMX_REG_DBM_BUF_CTRL) & (~ 0x3);
    u4Reg |= 0x1;
    DMXCMD_WRITE32(DMX_REG_DBM_BUF_CTRL, u4Reg);

    // Select port, set delay byte --- begin
    /*
        u4Reg = DDI_READ32(DDI_REG_DMX_RX_CTRL);
        u4Reg = (u4Reg & 0xffffffe0) | random(0x10);    // port 0, Delay byte = random(16)

        if ((u1TsIndex == 0) || (u1TsIndex == 1))
        {
            u4Reg &= ~(1 << 4);             // DDI connects to Framer.
        }
        else if (u1TsIndex == 2)
        {
            Printf("DDI cannot be the input of TS_index 2!\n");
        }
        else if (u1TsIndex == 3)
        {
            u4Reg |= (1 << 4);              // DDI does not connects to Framer
        }
        DDI_WRITE32(DDI_REG_DMX_RX_CTRL, u4Reg);
        // Select port, set delay byte --- end
    */
    if (u1TsIndex == 0)
    {
        _DMX_DDI_SetPort(DMX_DDI_PORT_FRAMER0, TRUE);
    }
    else if (u1TsIndex == 1)
    {
        _DMX_DDI_SetPort(DMX_DDI_PORT_FRAMER1, TRUE);
    }
    else if (u1TsIndex == 2)
    {
        //get DBM input source
        eSource = DMX_GetDbm_InputSource(u1TsIndex);
        if (eSource == DMX_DBM_INPUT_DDI)
        {
            _DMX_DDI_SetPort(DMX_DDI_PORT_FRAMER2_BYPASS, TRUE);
        }
        else
        {
            _DMX_DDI_SetPort(DMX_DDI_PORT_FRAMER2, TRUE);
        }

    }
    else if (u1TsIndex == 3)
    {
        //get DBM input source
        eSource = DMX_GetDbm_InputSource(u1TsIndex);
        if (eSource == DMX_DBM_INPUT_DDI)
        {
            _DMX_DDI_SetPort(DMX_DDI_PORT_FRAMER3_BYPASS, TRUE);
        }
        else
        {
            _DMX_DDI_SetPort(DMX_DDI_PORT_FRAMER3, TRUE);
        }
    }
    else
    {
        ASSERT(0);
    }


    /* DDI Pause test - Begin - */
    DMX_DDI_T rDDI;
    rDDI.u4RateN = 0;               // Max rate
    rDDI.u4RateM = 0;
    // rDDI.u4RateN = 4;               // Max rate
    //rDDI.u4RateM = 1;

    if (!_DMX_DDI_Set(DMX_DDI_FLAG_RATE, &rDDI))
    {
        Printf("Failed to set DDI rate!\n");
        return FALSE;
    }
    /* DDI Pause test - End - */

    return TRUE;
}

//-----------------------------------------------------------------------------
// DDI byte alignment specific functions
//      - start address can be byte alignment
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _DDI_Byte_Transfer0
 *  The function is the same as _DDI_Transfer0,
 *  except the u4Addr can be byte alignment
*/
//-----------------------------------------------------------------------------
static BOOL _DDI_Byte_Transfer0(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, i;
    BOOL fgRet;

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4Wp = _DMX_Align(u4Addr + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            // Complete
            fgRet = TRUE;
            Printf("   Wait count: %u\n", i);
            break;
        }

//        VERIFY(DMX_DRAMMeasure(DMX_LOCAL_ARBITOR_DDI, 500));

        _Delay(1);
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DDI_Byte_Transfer1_Reset
 *  The function is similar to _DDI_Transfer1,
 *  but in the first move, its write pointer must be in four-bytes alignment position.
 *  It means if u4Wp = u4BufStart + u4MoveSize
 *      (u4BufStart + u4MoveSize) % 4 == 0
 *  The size of the other moves must also four-bytes alignment.
 *
 *  Note: As MT5365/95 design,
 *        we need to reset DDI after every byte alignment transfer.
 *        This is a example that it can move data with above rules,
 *        then we don't need to reset DDI.
*/
//-----------------------------------------------------------------------------
static BOOL _DDI_Byte_Transfer1(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, i;
    BOOL fgRet = FALSE;
    BOOL isFirstMove = TRUE;

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4BufEnd = _DMX_Align(u4Addr + u4Size + 1000, DDI_BUF_ALIGNMENT);
    u4Wp = u4BufStart;

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        if (isFirstMove)
        {
            // Randomly generate step size
            u4StepSize = _DMX_Align((UINT32)random((INT32)u4Size) + 1 + u4BufStart,
                                    DDI_POINTER_ALIGNMENT) - u4BufStart;
            isFirstMove = FALSE;
        }
        else
        {
            u4StepSize = _DMX_Align((UINT32)random((INT32)u4Size) + 1,
                                    DDI_POINTER_ALIGNMENT);
        }

        u4Wp += u4StepSize;
        Printf("    Step size: %u, ", u4StepSize);

        // Update write pointer
        DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);

        // Waiting for completion by comparing RP with WP
        fgRet = FALSE;
        for (i = 0; i < MAX_WAIT_COUNT; i++)
        {
            UINT32 u4Rp;

            u4Rp = DDI_READ32(DDI_REG_DMA_RP);
            if (u4Rp >= u4Wp)
            {
                // Complete
                fgRet = TRUE;
                Printf("wait count: %u\n", i);
                break;
            }

            _Delay(1);
        }

        if (!fgRet)
        {
            return FALSE;
        }

        if (u4Size > u4StepSize)
        {
            u4Size -= u4StepSize;
        }
        else
        {
            u4Size = 0;
        }

    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DDI_Byte_Transfer1_Reset
 *  The function is similar to _DDI_Transfer1,
 *  but it will reset DDI and set DMA again every time
 *  before change write pointer to new position.
 *
 *  Note: As MT5365/95 design,
 *        we need to reset DDI after every byte alignment transfer.
 *        Or the read pointer could be ahead of write pointer,
 *        we will lost some bytes of non-4 bytes alignment data
*/
//-----------------------------------------------------------------------------
static BOOL _DDI_Byte_Transfer1_Reset(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, i;
    BOOL fgRet = FALSE;

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4BufEnd = _DMX_Align(u4Addr + u4Size + 1 + 2000, DDI_BUF_ALIGNMENT);
    u4Wp = u4BufStart;

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        // Randomly generate step size
        u4StepSize = _DMX_Align((UINT32)random((INT32)u4Size) + 1,
                                DDI_POINTER_ALIGNMENT);
        u4Wp += u4StepSize;
        Printf("    Step size: %u, ", u4StepSize);

        // Update write pointer
        DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);

        // Waiting for completion by comparing RP with WP
        fgRet = FALSE;
        for (i = 0; i < MAX_WAIT_COUNT; i++)
        {
            UINT32 u4Rp;

            u4Rp = DDI_READ32(DDI_REG_DMA_RP);
            if (u4Rp >= u4Wp)
            {
                // Complete
                fgRet = TRUE;
                Printf("wait count: %u\n", i);
                break;
            }

            _Delay(1);
        }

        if (!fgRet)
        {
            return FALSE;
        }

        u4Size -= u4StepSize;

        _DDI_Reset(FALSE);

        // Setup DMA
        DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
        DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
        DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4Wp);       // Initial RP
        DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);
        // Start DMA operation
        DDI_WRITE32(DDI_REG_DMA_CTRL, 5);
    }

    return fgRet;
}


BOOL _DMX_EMU_DDI_Byte_Transfer(UINT8 u1Type, UINT32 u4Addr, UINT32 u4Size)
{
    BOOL fgRet;

    switch (u1Type)
    {
    case 0:
        fgRet = _DDI_Byte_Transfer0(u4Addr, u4Size);
        break;

    case 1:
        fgRet = _DDI_Byte_Transfer1(u4Addr, u4Size);
        break;

    case 2:
        fgRet = _DDI_Byte_Transfer1_Reset(u4Addr, u4Size);
        break;

    default:
        fgRet = FALSE;
    }

    if (!fgRet)
    {
        Printf("DDI transfer fail!\n");
    }

    return fgRet;
}


BOOL _DDI_RECPULL_Transfer(UINT8 u1TsIndex, UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp;
    UINT32 u4DstRp, u4DstWp, u4DstBufStart, u4DstBufEnd;
    BOOL fgPrint = FALSE;

    if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        return FALSE;
    }

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4DstBufStart = FVR_GBL_ARY_W(u1TsIndex, 0);
    u4DstBufEnd = FVR_GBL_ARY_W(u1TsIndex, 1);
    u4DstWp = FVR_GBL_ARY_W(u1TsIndex, 3);
    ASSERT(u4DstWp == u4DstBufStart);
    u4DstRp = u4DstWp + 0x200;
    FVR_GBL_ARY_W(u1TsIndex, 4) = u4DstRp;  // read pointer

    // DDI
    u4BufStart = PHYSICAL(u4Addr);
    u4Wp = _DMX_Align(u4Addr + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();
    x_memset((void*)0x83400000, 0xFF, 32);
//    x_thread_delay(10);

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        // Debug
        Printf("  Dst RP: 0x%08x, WP: 0x%08x,", u4DstRp, u4DstWp);

        // Randomly generate step size
        u4StepSize = (UINT32)random((INT32)u4Size) + 1;
        Printf("    Step size: %u\n", u4StepSize);

        // Update read pointer of destination buffer
        u4DstRp += u4StepSize;
        FVR_GBL_ARY_W(0, 4) = u4DstRp;

        while (1)
        {
            UINT32 u4Wp1, u4Wp2;

            // Wait for a moment
            u4Wp1 = FVR_GBL_ARY_W(u1TsIndex, 3);
            //x_thread_delay(20);
            x_thread_delay(100);
            u4Wp2 = FVR_GBL_ARY_W(u1TsIndex, 3);
            if (u4Wp1 == u4Wp2)
            {
                if (DMXCMD_READ32(52) & 0x4)
                {
                    if (!fgPrint)
                    {
                        Printf("External full\n");
                        fgPrint = TRUE;
                    }
                }
                break;
            }
        }

        // Check if destination buffer wp exceeds rp
        u4DstWp = FVR_GBL_ARY_W(u1TsIndex, 3);
        if (u4DstWp >= u4DstRp)
        {
            return FALSE;
        }

        ASSERT(u4Size >= u4StepSize);
        u4Size -= u4StepSize;
    }

    // Update read pointer of destination buffer to buffer end to complete
    // transferring
    FVR_GBL_ARY_W(u1TsIndex, 4) = u4DstBufEnd - 1;

    while (1)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            break;
        }
    }

    x_thread_delay(50);

    return TRUE;
}


BOOL _DDI_PULLTransfer(UINT8 u1Pidx, UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp;
    UINT32 u4DstRp, u4DstWp;
    BOOL fgPrint = FALSE;

    if (!_DMX_IsAligned(u4Addr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        return FALSE;
    }

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        return FALSE;
    }

    u4BufStart = PHYSICAL(u4Addr);
    u4Wp = _DMX_Align(u4Addr + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    u4DstRp = PID_S_W(u1Pidx, 9);
    u4DstRp += 0x200;
    PID_S_W(u1Pidx, 9) = u4DstRp;                       // Read pointer

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    // Waiting for completion by comparing RP with WP
    while (u4Size > 0)
    {
        UINT32 u4StepSize;

        // Debug
        Printf("  Dst RP: 0x%08x, WP: 0x%08x,", u4DstRp, PID_S_W(u1Pidx, 8));

        // Randomly generate step size
        u4StepSize = (UINT32)random((INT32)u4Size) + 1;
        Printf("    Step size: %u\n", u4StepSize);

        // Update read pointer of destination buffer
        u4DstRp += u4StepSize;
        PID_S_W(u1Pidx, 9) = u4DstRp;

        while (1)
        {
            UINT32 u4Wp1, u4Wp2;

            // Wait for a moment
            u4Wp1 = PID_S_W(u1Pidx, 8);
            //x_thread_delay(20);
            x_thread_delay(100);
            u4Wp2 = PID_S_W(u1Pidx, 8);
            if (u4Wp1 == u4Wp2)
            {
                if (DMXCMD_READ32(52) & 0x4)
                {
                    if (!fgPrint)
                    {
                        Printf("External full\n");
                        fgPrint = TRUE;
                    }
                }
                break;
            }
        }

        // Check if destination buffer wp exceeds rp
        u4DstWp = PID_S_W(u1Pidx, 8);
        if (u4DstWp >= u4DstRp)
        {
            return FALSE;
        }

        ASSERT(u4Size >= u4StepSize);
        u4Size -= u4StepSize;
    }

    // Update read pointer of destination buffer to buffer end to complete
    // transferring
    PID_S_W(u1Pidx, 9) = PID_S_W(u1Pidx, 6);

    while (1)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            break;
        }
    }

    x_thread_delay(50);

    return TRUE;
}

