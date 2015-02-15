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
 * $RCSfile: dmx_emu_pull_ts.c,v $
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
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"

LINT_EXT_HEADER_END


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

static UINT32 _u4PesAddr = 0;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_PullTsTransfer0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, PUL_MODE_T eMode, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags,u4Reg, i;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Allocate source and destination buffers
    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);

    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1,DMX_ES_FIFO_ALIGNMENT);

    u4PesAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    Printf("src: 0x%08x,PesAddr: 0x%08x, dst: 0x%08x\n", u4SrcAddr, u4PesAddr, u4DstAddr);

    //clear source destination, pes buffer
    x_memset((VOID *)u4SrcAddr, 0 , u4Size);
    x_memset((VOID *)u4DstAddr, 0 , u4Size);
    x_memset((VOID *)u4PesAddr, 0 , u4Size);

    // Prepare data
#if 0
    _GenerateRandomPattern(u4SrcAddr, u4Size);
#else
    //generate  golden data.
    for(i = 0; i <u4Size; i++)
    {
        *(BYTE *)(u4SrcAddr+i) = i % 0xff;
    }
#endif
    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4Size, u4PesSize);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);

#if 0
    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, TRUE, FALSE))
    {
        goto _exit;
    }

    // Enable interrupts
    _DMX_InitISR();
#endif

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
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

#if 0
    if (!_DMX_EMU_SyncToggleBitInBypassCtrlWord(u1TsIndex, u1Pidx))
    {
        Printf("Cannot set the Toggle bit in the Bypass Control Word.\n");
        goto _exit;
    }
#endif

    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
    if (u1TsIndex == 2)
    {
#if defined(CC_MT5890)
        if (!fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
        else
#endif
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
            
        }
    }

    if (u1TsIndex == 3)
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

#if defined(CC_MT5890)
	if(u1TsIndex == 4)
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

    Printf("(Before)Pid index table: 0x%08x\n", PID_INDEX_TABLE(u1Pidx));

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    Printf("(After)Pid index table: 0x%08x\n", PID_INDEX_TABLE(u1Pidx));

    // Check result
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
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
    VERIFY(BSP_FreeAlignedDmaMemory(u4PesAddr));

    return fgRet;
}


BOOL DMX_EMU_PullTsTransfer1(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid,PUL_MODE_T eMode)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstBufSize, u4DstEnd;
    UINT32 u4PesAddr, u4PesSize = 0, u4SrcDiff, u4DstDiff, u4Flags,u4Reg;
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
    u4DstBufSize = u4Size + 1 + DEF_BUF_FULL_GAP;
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstBufSize,
                DMX_ES_FIFO_ALIGNMENT);
    u4PesAddr = (UINT32)x_mem_alloc(u4Size);
    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }
    Printf("TS size %u, PES size = %u\n", u4Size, u4PesSize);

    u4DstEnd = _DMX_Align(u4DstAddr + u4DstBufSize, DMX_ES_FIFO_ALIGNMENT);

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
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

#if defined(CC_MT5890)
	if(u1TsIndex == 4)
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
    if (!_DMX_EMU_DDI_Transfer(1, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
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
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    return fgRet;
}


BOOL DMX_EMU_PullTsTransfer2(UINT32 u4Size ,PUL_MODE_T eMode)
{
    PID_STRUCT_T* prPidStruct;
    UINT32 u4SrcAddr, u4PesSize = 0, u4DstBufStart = 0, u4DstBufEnd = 0, u4DstWp;
    UINT32 u4DstRp, u4Diff,u4Reg;
    const UINT16 u2Pid = 0;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Allocate source buffer
    u4SrcAddr = (UINT32)x_mem_alloc(u4Size);
    if (u4SrcAddr == 0)
    {
        return FALSE;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, _u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }
    Printf("TS size %u, PES size = %u\n", u4Size, u4PesSize);

    u4DstBufStart = PID_S_W(u1Pidx, 5);
    u4DstBufEnd = PID_S_W(u1Pidx, 6) + 1;
    u4DstWp = PID_S_W(u1Pidx, 8);

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (eMode << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(4, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    u4Diff = _MemCmpRing(u4DstWp, u4DstBufStart, u4DstBufEnd, _u4PesAddr,
                         _u4PesAddr, _u4PesAddr + u4PesSize, u4PesSize);
    if (u4Diff != 0xFFFFFFFF)
    {
        Printf("Differ at 0x%08x\n", u4Diff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Release source buffer
    x_mem_free((void*)u4SrcAddr);

    // Update destination read pointer to write pointer
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
    u4DstRp = u4DstWp + DEF_BUF_FULL_GAP;
    if (u4DstRp >= u4DstBufEnd)
    {
        u4DstRp -= (u4DstBufEnd - u4DstBufStart);
    }
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    _DMX_Lock();
    prPidStruct->u4Wp = u4DstWp;
    prPidStruct->u4Rp = u4DstRp;
    PID_S_W(u1Pidx, 9) = u4DstRp;
    _DMX_Unlock();

    return fgRet;
}

BOOL DMX_EMU_PullTsTransfer3(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid,PUL_MODE_T eMode)
{
    //DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Reg;
    //const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT8 *u1PsrcAddr;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Allocate source and destination buffers
    u4SrcAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1,
                DMX_CA_BUFFER_ALIGNMENT);
    u4PesAddr = (UINT32)x_mem_alloc(u4Size);
    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    //modify error ts packet!
    u1PsrcAddr = (UINT8 *)u4SrcAddr;
    u1PsrcAddr[0] = 0x46; //modify ts packet sync byte.
    u1PsrcAddr[189] |= 0x80; // modify ts transport error indicator.

    Printf("TS size %u, TS Address: %x. PES size = %u, PES Address: %x.\n", u4Size,u4SrcAddr,u4PesSize,u4PesAddr);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

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

    //Set ExternalFramer DMX_EXT_FRAMER_2_DDI_3_EXT
    DMX_SetDbm_InputSource(2, DMX_DBM_INPUT_FRAMER);

    DMX_SetDbm_InputSource(3, DMX_DBM_INPUT_FRAMER);

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

    // enable framer packet error handling  for framer 0 and framer 1
    //enable error handling
    _DMX_SetFramerPacketErrorHandling(u1TsIndex, TRUE, 0xF0C0);

    //enable framer 3 error handling
    u4Reg = DMXCMD_READ32(DMX_REG_DBM_BUF_CTRL);
    u4Reg &= ~((UINT32)0xFFFF << 16);
    u4Reg |= ((UINT32)0xF0F0 << 16);
    DMXCMD_WRITE32(DMX_REG_DBM_BUF_CTRL , u4Reg);

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Set CA output disable
    if (!_DMX_SetCaOutputBuffer(FALSE, TRUE, u4DstAddr, u4DstEnd,
                                u4DstEnd - u4DstAddr))
    {
        goto _exit;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr + 0x178, u4DstAddr, u4Size - 0x178, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Free memory
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    return fgRet;
}

BOOL DMX_EMU_InitPullTsBuffers(UINT8 u1TsIndex)
{
    DMX_PID_T rPid;
    PID_STRUCT_T* prPidStruct;
    UINT32 u4DstBufEnd, u4DstRp, u4Flags;
    const UINT16 u2Pid = 0;
    const UINT8 u1Pidx = 0;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    // Allocate source buffer
    ASSERT(_DMX_IsAligned(DEF_SRC_BUF_SIZE, DDI_BUF_ALIGNMENT));
    _u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(DEF_SRC_BUF_SIZE,
                 DDI_BUF_ALIGNMENT);

    // Allocate destination buffer
    ASSERT(_DMX_IsAligned(DEF_DST_BUF_SIZE, DMX_ES_FIFO_ALIGNMENT));
    _u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(DEF_DST_BUF_SIZE,
                 DMX_ES_FIFO_ALIGNMENT);

    // Allocate PES buffer
    _u4PesAddr = (UINT32)x_mem_alloc(DEF_DST_BUF_SIZE);

    if ((_u4SrcAddr == 0) || (_u4DstAddr == 0) || (_u4PesAddr == 0))
    {
        goto _exit;
    }

    u4DstBufEnd = _DMX_Align(_u4DstAddr + DEF_DST_BUF_SIZE, DMX_ES_FIFO_ALIGNMENT);

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
    rPid.u4BufAddr = _u4DstAddr;
    rPid.u4BufSize = u4DstBufEnd - _u4DstAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Set DDI buffer
    if (!_DMX_EMU_DDI_SetBuffer(_u4SrcAddr, DEF_SRC_BUF_SIZE))
    {
        goto _exit;
    }

    // Set read pointer to make buffer full
    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    u4DstRp = prPidStruct->u4Rp + DEF_BUF_FULL_GAP;
    _DMX_Lock();
    PID_S_W(u1Pidx, 9) = PHYSICAL(u4DstRp);
    prPidStruct->u4Rp = u4DstRp;
    _DMX_Unlock();

    return TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)_u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(_u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(_u4SrcAddr));

    _u4PesAddr = (_u4DstAddr = (_u4SrcAddr = 0));

    return FALSE;
}


BOOL DMX_EMU_ReleasePullTsBuffers(void)
{
    DMX_PID_T rPid;
    const UINT8 u1Pidx = 0;

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)_u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(_u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(_u4SrcAddr));

    _u4PesAddr = (_u4DstAddr = (_u4SrcAddr = 0));

    return TRUE;
}





