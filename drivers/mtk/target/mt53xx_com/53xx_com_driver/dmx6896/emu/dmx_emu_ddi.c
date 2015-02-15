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
 * $RCSfile: dmx_emu_ddi.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_ddi.c
 *  Demux driver - DDI interface emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "../dmx_mm.h"
#include "../fvr.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"

LINT_EXT_HEADER_END


extern BOOL _DMX_EMU_DDI_Byte_Transfer(UINT8 u1Type, UINT32 u4Addr, UINT32 u4Size);

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UINT32 _u4SrcAddr = 0;

static UINT32 _u4DstAddr = 0;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_DDI_Init(UINT8 u1TsIndex)
{
    UINT32 u4DstEnd;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    // Allocate source buffer
    ASSERT(_DMX_IsAligned(DEF_SRC_BUF_SIZE, DDI_BUF_ALIGNMENT));
    _u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(DEF_SRC_BUF_SIZE,
        DDI_BUF_ALIGNMENT);

    // Allocate destination buffer
    ASSERT(_DMX_IsAligned(DEF_DST_BUF_SIZE, DMX_CA_BUFFER_ALIGNMENT));
    _u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(DEF_DST_BUF_SIZE,
        DMX_CA_BUFFER_ALIGNMENT);

    if ((_u4SrcAddr == 0) || (_u4DstAddr == 0))
    {
        goto _err;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _err;
    }

    // Set CA output buffer
    u4DstEnd = _u4DstAddr + DEF_DST_BUF_SIZE;
    ASSERT(_DMX_IsAligned(u4DstEnd, DMX_CA_BUFFER_ALIGNMENT));
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, _u4DstAddr, u4DstEnd,
        u4DstEnd - _u4DstAddr))
    {
        goto _err;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _err;
    }

    // Set DDI buffer
    if (!_DMX_EMU_DDI_SetBuffer(_u4SrcAddr, DEF_SRC_BUF_SIZE))
    {
        goto _err;
    }

    return TRUE;

_err:

    // Free buffers
    VERIFY(BSP_FreeAlignedDmaMemory((void*)_u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)_u4SrcAddr));

    _u4DstAddr = (_u4SrcAddr = 0);

    return FALSE;
}


BOOL DMX_EMU_DDI_Exit(void)
{
    // Free buffers
    VERIFY(BSP_FreeAlignedDmaMemory((void*)_u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)_u4SrcAddr));

    _u4DstAddr = (_u4SrcAddr = 0);

    return TRUE;
}


BOOL DMX_EMU_DDI_Transfer0(UINT8 u1Type, UINT8 u1TsIndex, UINT32 u4Size)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
//    static UINT32 _u4Size = 0x1000 - (0x1000 % 188);

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if ((u1Type != 0) && (u1Type != 1))
    {
        return FALSE;
    }

//    u4Size = _u4Size;

    // Allocate source and destination buffers
    u4SrcAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1,
        DMX_CA_BUFFER_ALIGNMENT);
    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

    // Setup PID

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEnd,
        u4DstEnd - u4DstAddr))
    {
        goto _exit;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(u1Type, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4SrcAddr));

    return fgRet;
}


BOOL DMX_EMU_DDI_Transfer1(UINT8 u1Type, UINT32 u4Size)
{
    UINT32 u4SrcAddr, u4DstBufStart, u4DstBufEnd, u4DstWp, u4Diff;
    BOOL fgRet = FALSE;

    if ((u1Type != 0) && (u1Type != 1))
    {
        return FALSE;
    }

    // Allocate source buffer
    u4SrcAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    if (u4SrcAddr == 0)
    {
        return FALSE;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    u4DstBufStart = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_START);
    u4DstBufEnd = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_END) + 1;
    u4DstWp = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_WP);

    // Invoke DDI transfer type 2
    if (!_DMX_EMU_DDI_Transfer(u1Type + 2, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    u4Diff = _MemCmpRing(u4DstWp, u4DstBufStart, u4DstBufEnd, u4SrcAddr,
        u4SrcAddr, u4SrcAddr + u4Size, u4Size);
    if (u4Diff != 0xFFFFFFFF)
    {
        Printf("Differ at 0x%08x\n", u4Diff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Release source buffer
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4SrcAddr));

    // Update CA buffer read pointer to write pointer
    u4DstWp = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_WP);
    DMXCMD_WRITE32(DMX_REG_CA_OUT_BUF_RP, u4DstWp);

    return fgRet;
}


BOOL DMX_EMU_DDI_Transfer2(UINT8 u1Type, UINT8 u1TsIndex, UINT32 u4Size)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if ((u1Type != 0) && (u1Type != 1) && (u1Type != 2))
    {
        return FALSE;
    }

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000 + random(16);
    u4DstAddr = 0x3100000 + random(16);

    printf("u4SrcAddr: 0x%x, u4DstAddr: 0x%x\n", u4SrcAddr, u4DstAddr);

    
    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

    // Setup PID

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEnd,
        u4DstEnd - u4DstAddr))
    {
        goto _exit;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Byte_Transfer(u1Type, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    return fgRet;
}

