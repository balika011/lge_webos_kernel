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
 * $RCSfile: dmx_emu_tsout.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_tsout.c
 *  Demux driver - TS_output interface emulation
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
#include "x_hal_926.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_ckgen.h"

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

//static UINT32 _u4SrcAddr = 0;
//static UINT32 _u4DstAddr = 0;
//static UINT32 _u4Size    = 0;
//static UINT8  _u1TsIndex = 0;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

#if 0
static void dmx_TSOut_ResetRegisters(void)
{
    UINT32 u4Reg;
    BOOL fgParallelMode = random(2) ? TRUE : FALSE;
    // BOOL fgParallelMode = FALSE;

    // Framer 0/1: Positive, External Sync, Serial, Enabled
    // Internal TS_out can work with Positive/Negative, but external TS_out
    // works only with Negative.  Thus, Framer is set to Positive.
    u4Reg = (DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & 0xFFFFFF00);
    if (_u1TsIndex == 0)
    {
        u4Reg |= 0x0D;
        if (fgParallelMode)
        {
            u4Reg |= 1 << 1;
        }
        else
        {
            u4Reg &= ~(1 << 1);
        }
    }
    else if (_u1TsIndex == 1)
    {
        u4Reg |= 0xD0;
        if (fgParallelMode)
        {
            u4Reg |= 1 << 5;
        }
        else
        {
            u4Reg &= ~(1 << 5);
        }
    }
    else
    {
        Printf("Framer 0 and Framer 1 are disabled!\n");
    }
    DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);

    // Disable both Framer's error handling mechanism
    DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0x0);

    u4Reg = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
    u4Reg |= 1 << (_u1TsIndex + 26);
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Reg);

    // enable TS Output route; disable PES/DBM route; LSB first; 8-byte sync
    u4Reg = 0x00010000;
    if (fgParallelMode)
    {
        u4Reg |= 1 << 23;
    }
    else
    {
        u4Reg &= ~(1 << 23);
    }
    DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
}


static BOOL dmx_TSOut_RandomizeSteerCtrl(void)
{
    UINT32 u4Reg, u4Space, u4Sync;

    u4Space = (UINT32)random(4);
    u4Sync = (UINT32)random(2);

    u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
    u4Reg = (u4Reg & ~(0x3 << 20)) | (u4Space << 20);
    u4Reg = (u4Reg & ~(0x1 << 19)) | (u4Sync << 19);
    u4Reg = (u4Reg & ~(0x1 << 22)) | (1 << 22);  // negative edge
    DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

    return TRUE;
}


static BOOL dmx_TSOut_InitPvr(void)
{
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4Flags;

    if (!_DMX_PVRPlay_FreeBuf())
    {
        Printf("Failed to free PVR buffer!\n");
        return FALSE;
    }

    if (!_DMX_PVRPlay_Init())
    {
        Printf("Failed to initialize PVR!\n");
        return FALSE;
    }

    // _DMX_PVRPlay_Init will reset tsout setting in PVRPlay
    if(_u1TsIndex == 0 || _u1TsIndex == 1) 
    {
        _DMX_PVRPlay_SetTSOut(TRUE);
    }
    
    rPlay.fgAllocBuf = TRUE;
    rPlay.u4BufSize = 0x100000;
    rPlay.u4ThresholdSize = 0x20000;
    rPlay.pfnPVRNotify = NULL;
    u4Flags = PVRPLAY_FLAGS_BUFFER | PVRPLAY_FLAGS_THRESHOLD |
              PVRPLAY_FLAGS_CALLBACK;
    if (!_DMX_PVRPlay_Set(u4Flags, &rPlay))
    {
        Printf("Failed to set PVR properties!\n");
        return FALSE;
    }

    if (!DMX_PVRPlay_Get(PVRPLAY_FLAGS_BUFFER, &rPlay))
    {
        Printf("Failed to get PVR properties!\n");
        return FALSE;
    }

    _u4SrcAddr = rPlay.u4BufStart;

    return TRUE;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


BOOL DMX_EMU_TSOut_Init(UINT8 u1TsIndex, UINT32 u4Size)
{
    UINT32 u4Ctrl;
    
    if (u1TsIndex > MAX_TS_INDEX)   // MAX_TS_INDEX = 3
    {
        Printf("Invalid argument!\n");
        return FALSE;
    }

    if(u1TsIndex == 0) 
    {    
        u4Ctrl = CKGEN_READ32(0x24C);
        u4Ctrl |= 0x3;
        CKGEN_WRITE32(0x24C, u4Ctrl);

        u4Ctrl = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
        u4Ctrl |= (1 << 16);
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Ctrl);        
    }
    else if(u1TsIndex == 1)
    {
        u4Ctrl = CKGEN_READ32(0x24C);
        u4Ctrl |= 0x30000;
        CKGEN_WRITE32(0x24C, u4Ctrl);

        u4Ctrl = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
        u4Ctrl |= (1 << 16);
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Ctrl);        
    }
    else if(u1TsIndex == 2) 
    {
        u4Ctrl = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
        u4Ctrl |= (1 << 17);
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Ctrl);   
    }
    else
    {
        Printf("Tsindex: 0x%d does not support tsout.\n", u1TsIndex);
        return FALSE;
    }
    
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE))
    {
        Printf("Failed to set bypass mode\n");
        return FALSE;
    }

    _u1TsIndex = u1TsIndex;
    _u4Size = DMX_Align(u4Size, 16);
    // It's important to make the CA output buffer slightly larger.
    _u4DstAddr = (UINT32) BSP_AllocAlignedDmaMemory(_u4Size + 64,
                                                    DMX_CA_BUFFER_ALIGNMENT);
    if (_u4DstAddr == 0)
    {
        Printf("Failed to allocate buffers!\n");
        return FALSE;
    }

    VERIFY(dmx_TSOut_InitPvr());

    Printf("Src address: 0x%08x\n", _u4SrcAddr);
    Printf("Dst address: 0x%08x\n", _u4DstAddr);

    return TRUE;
}


// The input to 8295 needs to be in TS packet format.
//#define GENERATE_TS_HEADER

BOOL DMX_EMU_TSOut_StressTest(void)
{
    BOOL fgRet;
    UINT32 u4End, u4SrcDiff, u4DstDiff, count;
#ifdef GENERATE_TS_HEADER
    UINT32 u4PesAddr, u4PesSize;
#endif  // GENERATE_TS_HEADER

    // Reset some registers just in case they are modified somewhere.
    dmx_TSOut_ResetRegisters();


    // It's important to make the CA output buffer slightly larger.
    u4End = (_u4DstAddr + _u4Size + 64);
    u4End -= (u4End % 32);              // for CA alignment
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, _u4DstAddr, u4End, _u4Size))
    {
        Printf("Failed to set CA output buffer\n");
        return FALSE;
    }

    VERIFY(dmx_TSOut_RandomizeSteerCtrl());

    _GenerateRandomPattern(_u4SrcAddr, _u4Size);

#ifdef GENERATE_TS_HEADER
    u4PesAddr = (UINT32)x_mem_alloc(_u4Size);
    if (u4PesAddr == 0)
    {
        ASSERT(0);
    }

    if (!_GenerateSimpleTsHeader(_u4SrcAddr, _u4Size, 0, u4PesAddr,
        &u4PesSize))
    {
        ASSERT(0);
    }
#endif  // GENERATE_TS_HEADER

    HalFlushInvalidateDCache();

    VERIFY(_DMX_PVRPlay_FlushBuf());
    VERIFY(_DMX_PVRPlay_UpdateWp(_u4SrcAddr, _u4Size));

    // TODO: add back
    //VERIFY(_DMX_PVRPlay_SetTSOut(TRUE));
    VERIFY(_DMX_PVRPlay_Start());

    // Wait for some time.
    // x_thread_delay(3000);
    count = 0;
    while (1)
    {
        UINT32 u4Rp, u4Wp;
        u4Wp = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_WP);
        u4Rp = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_RP);
        if ((u4Wp - u4Rp) >= _u4Size)  // Do not consider wrap around for now.
        {
            break;
        }
        if (((++count) >= 0x00000600))  // two outmost pairs of parenthesis are necessary
        {
            fgRet = FALSE;
            goto tsoEnd;
        }
        x_thread_delay(1);
    }

    VERIFY(_DMX_PVRPlay_Stop(TRUE));

    // Check result
#ifdef GENERATE_TS_HEADER
    if (!_MemCmp(_u4SrcAddr, _u4DstAddr, _u4Size, &u4SrcDiff, &u4DstDiff))
#else
    if (!_MemCmp(_u4SrcAddr, _u4DstAddr, _u4Size, &u4SrcDiff, &u4DstDiff))
#endif
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }
    else
    {
        fgRet = TRUE;
    }

tsoEnd:
    VERIFY(_DMX_PVRPlay_FreeBuf());
    VERIFY(BSP_FreeAlignedDmaMemory((void*)_u4DstAddr));
#ifdef GENERATE_TS_HEADER
    x_mem_free((void*)u4PesAddr);
#endif  // GENERATE_TS_HEADER

    return fgRet;
}
#endif

//-----------------------------------------------------------------------------
/** _SetTestTsOutPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetTestTsOutPid(UINT8 u1TsIdx, UINT16 u2Pid, UINT32 u4BufAddr, UINT32 u4Size)
{
    DMX_PID_T rPid;
    UINT32 u4Reg;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4BufAddr;
    rPid.u4BufSize = 0x200000;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    if (!_DMX_SetPid(0, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    u4Reg = PID_S_W(0, 0);
    PID_S_W(0, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(0, 1) = u4Size;
    PID_S_W(0, 3) = (0 << 24) | 1;     // skip size = 0, and trigger    

    return  TRUE;
}


//-----------------------------------------------------------------------------
// _TSOUT_PVR_Transfer
//-----------------------------------------------------------------------------
static BOOL _TSOUT_PVR_Transfer(UINT32 u4Addr, UINT32 u4Size)
{
    PID_STRUCT_T* prPidStruct;
    UINT32 u4DstBufStart, u4DstBufEnd, u4DstRp, u4DstWp;
    const UINT8 u1Pidx = 0;
    BOOL fgPrint = FALSE;

    // Set read pointer to make buffer full
    u4DstBufStart = VIRTUAL(PID_S_W(u1Pidx, 5));
    u4DstBufEnd = VIRTUAL(PID_S_W(u1Pidx, 6));
    u4DstWp = VIRTUAL(PID_S_W(u1Pidx, 8));
    ASSERT(u4DstWp == u4DstBufStart);
    u4DstRp = u4DstWp + 0x100;

    // Debug
    Printf("Dst buffer: 0x%08x - 0x%08x, RP: 0x%08x, WP: 0x%08x\n",
        u4DstBufStart, u4DstBufEnd, u4DstRp, u4DstWp);

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    _DMX_Lock();
    PID_S_W(u1Pidx, 9) = PHYSICAL(u4DstRp);
    prPidStruct->u4Rp = u4DstRp;
    _DMX_Unlock();

    HalFlushInvalidateDCache();
    
    VERIFY(_DMX_PVRPlay_FlushBuf());
    VERIFY(_DMX_PVRPlay_UpdateWp(u4Addr, u4Size));
    VERIFY(_DMX_PVRPlay_Start());

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
        if (!_DMX_UpdateReadPointer2(u1Pidx, u4DstRp, 0))
        {
            return FALSE;
        }

        while(1)
        {
            UINT32 u4Wp1, u4Wp2;
            
            // Wait for a moment
            u4Wp1 = PID_S_W(u1Pidx, 8);
            x_thread_delay(20);
            u4Wp2 = PID_S_W(u1Pidx, 8);
            if(u4Wp1 == u4Wp2)
            {
                if(DMXCMD_READ32(52) & 0x4)
                {
                    if(!fgPrint)
                    {
                        Printf("External full\n");
                        fgPrint = TRUE;
                    }
                }
                break;
            }
        }

        // Check if destination buffer wp exceeds rp
        u4DstWp = VIRTUAL(PID_S_W(u1Pidx, 8));
        if (u4DstWp >= u4DstRp)
        {
            return FALSE;
        }

        ASSERT(u4Size >= u4StepSize);
        u4Size -= u4StepSize;
    }

    // Update read pointer of destination buffer to buffer end to complete
    // transferring
    if (!_DMX_UpdateReadPointer2(u1Pidx, u4DstBufEnd - 1, 0))
    {
        return FALSE;
    }

    x_thread_delay(50);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** DMX_TsOut_BackStop_TransmitData
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_BackStop_TransmitData(UINT8 u1TsIndex, UINT32 u4Size)
{
    BOOL fgRet = FALSE;
    UINT32 u4SrcBufSize, u4DstBufSize;
    UINT32 u4SrcAddr = 0, u4SrcDiff, u4SrcSize;
    UINT32 u4DstAddr = 0, u4DstDiff;
    UINT32 u4PesAddr = 0, u4PesSize;
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4Flags;
    //UINT32 u4Rp, u4Wp;
    UINT32 i, u4Reg;

    // Init PVR
    if (!_DMX_PVRPlay_Init())
    {
        Printf("Failed to initialize PVR!\n");
        return FALSE;
    }

    if((u1TsIndex == 0) || (u1TsIndex == 1) || (u1TsIndex == 3)) 
    {
        _DMX_PVRPlay_SetTSOut(TRUE);
    }

	if(u1TsIndex == 3)
    {
		//Set ExternalFramer DMX_EXT_FRAMER_2_DDI_3_EXT
		_DMX_SetExternalFramer(DMX_EXT_FRAMER_2_DDI_3_EXT);
	}
		
    // Allocate source buffer.  (Mini-PVR RP/WP 8-byte alignment, 188 * 4 = 752)
    u4SrcSize = _DMX_Align(u4Size, 752);
    u4SrcBufSize = u4SrcSize + DMX_CA_BUFFER_ALIGNMENT;
    u4SrcAddr = 0x3000000;

    // Allocate destination buffer
    // It's important to make the CA output buffer slightly larger.
    u4DstBufSize = _DMX_Align(u4SrcBufSize + 1000, DMX_CA_BUFFER_ALIGNMENT);
    u4DstAddr = 0x3400000;

    for ( i = 0 ; i < u4DstBufSize/4 ; i++ )
    {
      ((UINT32*)u4DstAddr)[i] = 0;
    }
    //u4DstEndAddr = u4DstAddr + u4DstBufSize;

    // Set PVR playback
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4SrcAddr;
    rPlay.u4BufSize = u4SrcBufSize;
    rPlay.u4ThresholdSize = u4SrcSize;  // FIXME
    rPlay.pfnPVRNotify = NULL;
    u4Flags = PVRPLAY_FLAGS_BUFFER | PVRPLAY_FLAGS_THRESHOLD |
              PVRPLAY_FLAGS_CALLBACK;
    if (!_DMX_PVRPlay_Set(u4Flags, &rPlay))
    {
        Printf("Failed to set PVR properties!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    if(!_SetTestTsOutPid(u1TsIndex, 0, u4DstAddr, u4SrcSize))
    {
        return FALSE;
    }
    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);
    u4PesAddr = 0x3800000;

    // The input to 8295 needs to be in TS packet format.
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4SrcSize, 0, u4PesAddr,
        &u4PesSize))
    {
        Printf("Failed to generate TS packets!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    HalFlushInvalidateDCache();

    Printf("Src address: 0x%08x\n", u4SrcAddr);
    Printf("Dst address: 0x%08x\n", u4DstAddr);
    Printf("Original Src size: %u.  New Src Size: %u\n", u4Size, u4SrcSize);

    //VERIFY(_DMX_PVRPlay_FlushBuf());
    //VERIFY(_DMX_PVRPlay_UpdateWp(u4SrcAddr, u4SrcSize));
    //VERIFY(_DMX_PVRPlay_Start());

    // Set Full pull mode
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);
    

    // Wait for some time.
    // x_thread_delay(3000);
    if(!_TSOUT_PVR_Transfer(u4SrcAddr, u4SrcSize))
    {
        Printf("TSOut transfer fail\n");
        fgRet = FALSE;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr,
        u4SrcSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }
    else
    {
        fgRet = TRUE;
    }

FailAndFreeMemory:
    VERIFY(_DMX_PVRPlay_Stop(TRUE));
    VERIFY(_DMX_PVRPlay_FreeBuf());

    _DMX_FreePid(0);    

    // Set back
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= ~0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);
    
    return fgRet;
}


static void _GenerateTsOutPrebyteTsPacket(BOOL fgExtSync, UINT32 u4TsAddr, UINT32 u4TsSize, UINT16 u2Pid, 
                UINT32 u4DstAddr, UINT32 *pu2DstSize, UINT16 u2PktSize, UINT16 u2DstPktSize)
{
    UINT8 *pu1Data;
    UINT32 u4Offset, u4DstOffset;
    UINT8 u1CC = 0;
    UINT8 u1PrebyteOffset = 0;
    UINT8 u1SkipCount;

    if(fgExtSync)
    {
        u1PrebyteOffset = u2PktSize - u2DstPktSize;
    }

    pu1Data = (UINT8*)u4TsAddr;
    u4Offset = 0;
    u4DstOffset = 0;
#if 0   // HW bug, fixed
    if(u2PktSize != 188)        /// ????
    {
        u1SkipCount = 0;
    }
    else
#endif
    {
        u1SkipCount = 1;
    }

    *pu2DstSize = 0;

    while(u4TsSize > 0)
    {
        pu1Data[u1PrebyteOffset + u4Offset] = 0x47;
        pu1Data[u1PrebyteOffset + u4Offset + 1] = (u2Pid & 0x1FFF) >> 8;
        pu1Data[u1PrebyteOffset + u4Offset + 2] = (u2Pid & 0xFF);
        pu1Data[u1PrebyteOffset + u4Offset + 3] = 0x10 | (u1CC & 0xF);
        u1CC++;

        if(!fgExtSync && (u1SkipCount < 4))
        {
            u1SkipCount++;
        }
        else
        {
            x_memcpy((void*)(u4DstAddr + u4DstOffset),  &pu1Data[u1PrebyteOffset + u4Offset], u2DstPktSize);
            u4DstOffset += u2DstPktSize;
        }

        u4TsSize -= u2PktSize;
        u4Offset += u2PktSize;
    }

    *pu2DstSize = u4DstOffset;
}


//-----------------------------------------------------------------------------
/** DMX_TsOut_Framer_TransmitData
 *  TS out framer only support internal mode
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_Framer_TransmitData(UINT8 u1TsIndex, UINT32 u4Size, UINT8 u1PktSize, BOOL fgErrTest)
{
    BOOL fgRet = TRUE;
    UINT32 u4SrcBufSize, u4DstBufSize;
    UINT32 u4SrcAddr = 0, u4SrcSize;
    UINT32 u4DstAddr = 0, u4DstEndAddr;
    UINT32 u4PesAddr = 0, u4PesSize;
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4Flags;
    //UINT32 u4Rp, u4Wp;
    UINT32 i, u4Reg;
    UINT16 u2Pid;
    UINT32 u4ErrorPkt = 0;

    u2Pid = random(0x1FFB);

    // This function must before setpid - set packet size will reset pid valid bit
    VERIFY(_DMX_EMU_SetPktSize(u1TsIndex, u1PktSize, 188));

    // Set framer, must after setpktsize
    u4Reg = IO_READ32(DEMUX0_BASE, 0x20) & 0xFFF0FF00;
    u4Reg |= 0x30000;
    u4Reg |= 0xFF;
    IO_WRITE32(DEMUX0_BASE, 0x20, u4Reg);
    _DMX_ResetFramer(0);
    _DMX_ResetFramer(1);
    _DMX_ResetFramer(2);
    _DMX_ResetFramer(3);

    // Init PVR
    if (!_DMX_PVRPlay_Init())
    {
        Printf("Failed to initialize PVR!\n");
        return FALSE;
    }

    if(u1TsIndex == 0 || u1TsIndex == 1) 
    {
        _DMX_PVRPlay_SetTSOut(TRUE);
    }

    u4SrcSize = u4Size;
    u4SrcSize -= (u4SrcSize % u1PktSize);
    u4SrcBufSize = u4SrcSize + DMX_CA_BUFFER_ALIGNMENT;
    u4SrcAddr = 0x3000000;

    // Allocate destination buffer
    // It's important to make the CA output buffer slightly larger.
    u4DstBufSize = _DMX_Align(u4SrcBufSize + 1000, DMX_CA_BUFFER_ALIGNMENT);
    u4DstAddr = 0x3400000;

    for ( i = 0 ; i < u4DstBufSize/4 ; i++ )
    {
        ((UINT32*)u4DstAddr)[i] = 0;
    }
    u4DstEndAddr = u4DstAddr + u4DstBufSize;

    // Set PVR playback
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4SrcAddr;
    rPlay.u4BufSize = u4SrcBufSize;
    rPlay.u4ThresholdSize = u4SrcSize;  // FIXME
    rPlay.pfnPVRNotify = NULL;
    u4Flags = PVRPLAY_FLAGS_BUFFER | PVRPLAY_FLAGS_THRESHOLD |
              PVRPLAY_FLAGS_CALLBACK;
    if (!_DMX_PVRPlay_Set(u4Flags, &rPlay))
    {
        Printf("Failed to set PVR properties!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);
    u4PesAddr = 0x3800000;

    _GenerateTsOutPrebyteTsPacket(0, u4SrcAddr, u4SrcSize, u2Pid, u4PesAddr, 
                             &u4PesSize, u1PktSize, 188);

    u4Reg = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
    u4Reg &= 0xc3ffffff;
    u4Reg |= (1 << (u1TsIndex + 26));
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Reg);
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0x0);    

    // Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEndAddr, u4DstBufSize))
    {
        return FALSE;
    }
    
    Printf("Src address: 0x%08x\n", u4SrcAddr);
    Printf("Dst address: 0x%08x\n", u4DstAddr);
    Printf("Original Src size: %u.  New Src Size: %u\n", u4Size, u4SrcSize);

    // Set Full pull mode
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    if(fgErrTest)
    {
        UINT32 u4ErrOffset;        
        
        u4ErrorPkt = (random(100) + 10);
        Printf("Set packet %d error\n", u4ErrorPkt);
        u4ErrOffset = u4ErrorPkt * u1PktSize;
        u4ErrOffset = (u4ErrOffset > u4SrcSize) ? 0 : u4ErrOffset;
        ASSERT(*(volatile UINT8*)(u4SrcAddr + u4ErrOffset) == 0x47);
        *(volatile UINT8*)(u4SrcAddr + u4ErrOffset) = 0x0;
    }
    if(!_TSOUT_PVR_Transfer(u4SrcAddr, u4SrcSize))
    {
        Printf("TSOut transfer fail\n");
        fgRet = FALSE;
    }

    {
    	UINT8 *pu1S1, *pu1S2;
    	UINT32 u4ErrByteCount=0, j, k;
    	BOOL fgTmp = FALSE;

        pu1S1 = (UINT8*)u4PesAddr;
        pu1S2 = (UINT8*)u4DstAddr;

        u4ErrorPkt = 0;
    	for (i=0,j=0; i < u4PesSize; i+=188)
    	{
    	    for(k=0; k<188;k++)
    	    {
    	        fgTmp = TRUE;
        		if (pu1S1[i+k] != pu1S2[j+k])
        		{
        		    if(fgErrTest)
        		    {
        		        Printf("Packet %d different\n", i/188);
        		        u4ErrorPkt++;
        		        fgTmp = FALSE;
        		        break;
        		    }
        		    u4ErrByteCount++;
        		    if ( u4ErrByteCount < 100 )
        		    {
                        Printf("Byte Error, offset=%d, 0x%x:0x%x\n", i+k, pu1S1[i+k], pu1S2[j+k]);
                    }
        		}
    	    }
    	    if(fgTmp)
    	    {
    	        j+=188;
    	    }
    	}
/*
        if(fgErrTest)
        {
        	if(u4ErrorPkt != 4)
        	{
        	    Printf("Error packet too much\n");
        	    fgRet = FALSE;
        	}
        }
*/
        if ( u4ErrByteCount != 0 )
        {
            Printf("Error Byte Count : %d\n", u4ErrByteCount);
            fgRet = FALSE;
        }
    }

FailAndFreeMemory:
    VERIFY(_DMX_PVRPlay_Stop(TRUE));
    VERIFY(_DMX_PVRPlay_FreeBuf());

    _DMX_FreePid(0);    
    x_thread_delay(50);

    // Set back
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= ~0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, 0); 

    u4Reg = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
    u4Reg &= 0xc3ffffff;
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Reg);
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0x0);        
    
    return fgRet;
}



