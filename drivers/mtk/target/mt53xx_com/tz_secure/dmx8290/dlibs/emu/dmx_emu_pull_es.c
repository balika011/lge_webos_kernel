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
 * $RCSfile: dmx_emu_pull_es.c,v $
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
#include "dmx_emu_if.h"

#include "dmx_mm.h"
#include "dmx_imem_pe1.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"

LINT_EXT_HEADER_END

extern BOOL _GetMicroCodeVersion(UINT32* pu4Version, UINT32* pu4Type);

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


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_PullEsTransfer0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid ,PUL_MODE_T eMode)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4DstBufSize, u4Reg;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Allocate source and destination buffers
    u4SrcAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstBufSize = u4Size + 0x5000 + DEF_BUF_FULL_GAP;
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstBufSize,
                DMX_ES_FIFO_ALIGNMENT);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);
    u4DstEnd = _DMX_Align(u4DstAddr + u4DstBufSize, DMX_ES_FIFO_ALIGNMENT);

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, TRUE, TRUE))
    {
        goto _exit;
    }

    // Enable interrupts
    _DMX_InitISR();

    // Setup PID
     x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4DstEnd - u4DstAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEOCLIP;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    if (!_DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    if (!_DMX_EMU_SyncToggleBitInBypassCtrlWord(u1TsIndex, u1Pidx))
    {
        Printf("Cannot set the Toggle bit in the Bypass Control Word.\n");
        goto _exit;
    }

    u4Reg = PID_S_W(u1Pidx, 0);
    PID_S_W(u1Pidx, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(u1Pidx, 1) = u4Size;
    PID_S_W(u1Pidx, 3) = (0 << 24) | 1;     // skip size = 0, and trigger

    if( u1TsIndex == 2 )
    {
#if defined(CC_MT5890)
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
#else  //MT5882
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
#endif
    }

	if(u1TsIndex == 3)
	{
	    DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
	}

#if defined(CC_MT5890)
	if(u1TsIndex == 4 )
	{
	    DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#endif

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (eMode << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
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

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    return fgRet;
}


BOOL DMX_EMU_PullEsTransfer1(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, PUL_MODE_T eMode)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstBufSize, u4DstEnd;
    UINT32 u4SrcDiff, u4DstDiff;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT32 u4Version ,u4Reg;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //-------------------------------------------
    // Load 3.08 version micro-code
    //-------------------------------------------
    // Check uCode type
    if (!_GetMicroCodeVersion(&u4Version, NULL))
    {
        return FALSE;
    }

    if ((u4Version & 0xF) != 8)
    {
        if (!_DMX_LoadIMem(TRUE, FTI_IMEM_PE1, FTI_IMEM_PE1_LEN))
        {
            return FALSE;
        }
    }

    // Enable FTI
    _DMX_EnableFTI(TRUE);


    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Allocate source and destination buffers
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstBufSize = u4Size + 1 + DEF_BUF_FULL_GAP;
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstBufSize, DMX_ES_FIFO_ALIGNMENT);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);
    /*
        // Translate to TS format
        if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
            &u4PesSize))
        {
            goto _exit;
        }
    */
    u4DstEnd = _DMX_Align(u4DstAddr + u4DstBufSize, DMX_ES_FIFO_ALIGNMENT);

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, TRUE, TRUE))
    {
        goto _exit;
    }
    
#if defined(CC_MT5890)
	if(u1TsIndex == 4 )
	{
	   DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#endif

#if defined(CC_MT5882)
	if(u1TsIndex == 2 )
	{
	   DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#endif


    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Setup PID
     x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4DstEnd - u4DstAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEOCLIP;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    if (!_DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    if (!_DMX_EMU_SyncToggleBitInBypassCtrlWord(u1TsIndex, u1Pidx))
    {
        Printf("Cannot set the Toggle bit in the Bypass Control Word.\n");
        goto _exit;
    }

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (eMode << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(1, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Move flush cache to memcmp
    //HalFlushInvalidateDCache();

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    return fgRet;
}

