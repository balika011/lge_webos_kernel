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

#define DMX_IGNORE_TRAILING_PACKETS         100
#define DMX_IGNORE_TRAILING_LEN             (188 * DMX_IGNORE_TRAILING_PACKETS)

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

    VERIFY(_DMX_PVRPlay_FlushBuf(0));
    VERIFY(_DMX_PVRPlay_UpdateWp(0, u4Addr, u4Size));
    VERIFY(_DMX_PVRPlay_Start(0));

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

        while (1)
        {
            UINT32 u4Wp1, u4Wp2;

            // Wait for a moment
            u4Wp1 = PID_S_W(u1Pidx, 8);
            x_thread_delay(20);
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

static void _GenerateTsOutPrebyteTsPacket(BOOL fgExtSync, UINT32 u4TsAddr, UINT32 u4TsSize, UINT16 u2Pid,
        UINT32 u4DstAddr, UINT32 *pu2DstSize, UINT16 u2PktSize, UINT16 u2DstPktSize)
{
    UINT8 *pu1Data;
    UINT32 u4Offset, u4DstOffset;
    UINT8 u1CC = 0;
    UINT8 u1PrebyteOffset = 0;
    UINT8 u1SkipCount;

    if (fgExtSync)
    {
        u1PrebyteOffset = u2PktSize - u2DstPktSize;
    }

    pu1Data = (UINT8*)u4TsAddr;
    u4Offset = 0;
    u4DstOffset = 0;
#if 0   // HW bug, fixed
    if (u2PktSize != 188)       /// ????
    {
        u1SkipCount = 0;
    }
    else
#endif
    {
        u1SkipCount = 1;
    }

    *pu2DstSize = 0;

    while (u4TsSize > 0)
    {
        pu1Data[u1PrebyteOffset + u4Offset] = 0x47;
        pu1Data[u1PrebyteOffset + u4Offset + 1] = (u2Pid & 0x1FFF) >> 8;
        pu1Data[u1PrebyteOffset + u4Offset + 2] = (u2Pid & 0xFF);
        pu1Data[u1PrebyteOffset + u4Offset + 3] = 0x10 | (u1CC & 0xF);
        u1CC++;

        if (!fgExtSync && (u1SkipCount < 4))
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


BOOL DMX_TsOut_ToCaOutput(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1TsoutIdx)
{
    UINT32 u4SrcSize,u4SrcBufSize,u4SrcAddr, u4DstBufSize, u4DstAddr, u4DstEndAddr;
    UINT32 u4Size = 0x10000;
    //UINT16 u2Pid = 0x100;
    BOOL fgRet= TRUE;
    UINT32 u4Flags;
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4PesAddr = 0, u4PesSize, u4Reg;
    UINT32 u4SrcDiff, u4DstDiff;

    ASSERT(u1TsoutIdx < DMX_MAX_TSOUT);
    ASSERT(u1TsIndex < DMX_FRAMER_COUNT);

    if (!_DMX_PVRPlay_Init())
    {
        Printf("Failed to initialize PVR!\n");
        return FALSE;
    }

    //Set DBM bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE))
    {
        Printf("Failed to set bypass mode\n");
        return FALSE;
    }

    //Set TSout port
    if (u1TsIndex == 0)
    {
        _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER0);
    }
    else if (u1TsIndex == 1)
    {
        _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER1);
    }
    else if (u1TsIndex == 2)
    {
        _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
    }
    else if (u1TsIndex == 3)
    {
        if (fgBypassFramer)
        {
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
        }
        else
        {
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3);
        }
    }
    else
    {
        ASSERT(0);
    }

    //Set tsout config
    VERIFY(DMX_TsOut_SetDataWidthEx(u1TsoutIdx,DMX_TSOUT_DATAWIDTH_1)); //serial output
    VERIFY(DMX_TsIn_SetDataWidth(u1TsIndex, DMX_TSOUT_DATAWIDTH_1));    //serial input
    VERIFY(DMX_TsOut_SetEndianessEx(u1TsoutIdx,DMX_TSOUT_ENDIAN_MSB));
    VERIFY(DMX_TsOut_SetSyncCycleEx(u1TsoutIdx,DMX_TSOUT_SYNC_CYCLE_8));
    VERIFY(DMX_TsOut_SetValidCycleEx(u1TsoutIdx, 0));
    VERIFY(DMX_TsOut_SetLatchEdgeEx(u1TsoutIdx, DMX_TSOUT_LATCH_EDGE_NEGATIVE));

    // Allocate source buffer.  (Mini-PVR RP/WP 8-byte alignment, 188 * 4 = 752)
    u4SrcSize = _DMX_Align(u4Size, 752);
    u4SrcBufSize = u4SrcSize + DMX_CA_BUFFER_ALIGNMENT;
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4SrcBufSize, DMX_CA_BUFFER_ALIGNMENT);
    if (u4SrcAddr == 0)
    {
        Printf("Failed to allocate source buffer!\n");
        return FALSE;
    }

    // Allocate destination buffer
    // It's important to make the CA output buffer slightly larger.
    u4DstBufSize = _DMX_Align(u4SrcBufSize + 1000, DMX_CA_BUFFER_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstBufSize, DMX_CA_BUFFER_ALIGNMENT);
    if (u4DstAddr == 0)
    {
        Printf("Failed to allocate destination buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    //clear dst buffer
    x_memset((void *)u4DstAddr, 0 , u4DstBufSize);
    u4DstEndAddr = u4DstAddr + u4DstBufSize;

    // Set PVR playback
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4SrcAddr;
    rPlay.u4BufSize = u4SrcBufSize;
    rPlay.u4ThresholdSize = u4SrcSize;  // FIXME
    rPlay.pfnPVRNotify = NULL;
    u4Flags = PVRPLAY_FLAGS_BUFFER | PVRPLAY_FLAGS_THRESHOLD |
              PVRPLAY_FLAGS_CALLBACK;
    if (!_DMX_PVRPlay_Set(u1TsoutIdx, u4Flags, &rPlay))
    {
        Printf("Failed to set PVR properties!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    //Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEndAddr, u4SrcSize))
    {
        Printf("Failed to set CA output buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);

    u4PesAddr = (UINT32)x_mem_alloc(u4SrcSize);
    if (u4PesAddr == 0)
    {
        Printf("Failed to allocate payload buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

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

    // Set Full pull mode
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    VERIFY(_DMX_PVRPlay_FlushBuf(u1TsoutIdx));
    VERIFY(_DMX_PVRPlay_UpdateWp(u1TsoutIdx, u4SrcAddr, u4SrcSize));
    VERIFY(_DMX_PVRPlay_Start(u1TsoutIdx));

    // Wait for some time.
    x_thread_delay(100);
    while (1)
    {
        VERIFY(DMX_PVRPlay_GetEx(u1TsoutIdx, PVRPLAY_FLAGS_BUFFER, &rPlay));
        if (rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }

    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4SrcSize , &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }
    else
    {
        fgRet = TRUE;
    }

FailAndFreeMemory:
    VERIFY(_DMX_PVRPlay_Stop(u1TsoutIdx, TRUE));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsoutIdx));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    if (u4PesAddr != 0)
    {
        x_mem_free((void*)u4PesAddr);
    }

    // Set back
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= ~0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    return fgRet;
}


static BOOL _DMX_SetPlayPid(UINT8 u1TsIdx , UINT8 u1PidIdx, UINT16 u2Pid ,UINT32 u4BufAddr ,UINT32 u4BufSize)
{
    DMX_PID_T rPid;

    // Setup PID
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4BufAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;

    if (!_DMX_SetPid(u1PidIdx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}

BOOL DMX_TsOut_ToFtup(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1TsoutIdx)
{
    UINT32 u4SrcSize,u4SrcBufSize,u4SrcAddr, u4DstBufSize, u4DstAddr;
    UINT32 u4Size = 0x10000;
    UINT16 u2Pid = 0x100;
    BOOL fgRet= TRUE;
    UINT32 u4Flags;
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4PesAddr = 0, u4PesSize, u4Reg;
    UINT32 u4SrcDiff, u4DstDiff;
    UINT8 u1PidIdx = 0;
    DMX_PID_T rPid;
	UINT32 u4Rp=0,u4Wp =0;

    ASSERT(u1TsoutIdx < DMX_MAX_TSOUT);
    ASSERT(u1TsIndex < DMX_FRAMER_COUNT);

    if (!_DMX_PVRPlay_Init())
    {
        Printf("Failed to initialize PVR!\n");
        return FALSE;
    }

    //Set TSout port
    if (u1TsIndex == 0)
    {
        _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER0);
    }
    else if (u1TsIndex == 1)
    {
        _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER1);
    }
    else if (u1TsIndex == 2)
    {
        if (fgBypassFramer)
        {
           if(u1TsoutIdx <2)
           {
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
           }
		   else
		   {
		     Printf("TSout3 Pb3 con't connect to Framer2.\r\n");
			 return FALSE;
		   }
        }
        else
        {
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER2);
        }
    }
    else if (u1TsIndex == 3)
    {
        if (fgBypassFramer)
        {
           if(u1TsoutIdx <2)
           {
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
           }
		   else
		   {
		     Printf("TSout3 Pb3 con't connect to Framer3.\r\n");
			 return FALSE;
		   }
        }
        else
        {
            _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_FRAMER3);
        }
    }
	else if(u1TsIndex == 4)
	{
	     _DMX_PVRPlay_SetPort(u1TsoutIdx,DMX_PVRPLAY_PORT_DBM);
	}
    else
    {
        ASSERT(0);
    }

    //Set tsout config
    VERIFY(DMX_TsOut_SetDataWidthEx(u1TsoutIdx,DMX_TSOUT_DATAWIDTH_1)); //serial output
    if(u1TsIndex <4)
    {
     VERIFY(DMX_TsIn_SetDataWidth(u1TsIndex, DMX_TSOUT_DATAWIDTH_1));    //serial input
    }
    VERIFY(DMX_TsOut_SetEndianessEx(u1TsoutIdx,DMX_TSOUT_ENDIAN_MSB));
    VERIFY(DMX_TsOut_SetSyncCycleEx(u1TsoutIdx,DMX_TSOUT_SYNC_CYCLE_8));
    VERIFY(DMX_TsOut_SetValidCycleEx(u1TsoutIdx, 0));
    VERIFY(DMX_TsOut_SetLatchEdgeEx(u1TsoutIdx, DMX_TSOUT_LATCH_EDGE_NEGATIVE));

    // Allocate source buffer.  (Mini-PVR RP/WP 8-byte alignment, 188 * 4 = 752)
    u4SrcSize = _DMX_Align(u4Size, 752);
    u4SrcBufSize = u4SrcSize + DDI_BUF_ALIGNMENT;
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4SrcBufSize, DDI_BUF_ALIGNMENT);
    if (u4SrcAddr == 0)
    {
        Printf("Failed to allocate source buffer!\n");
        return FALSE;
    }

    // Allocate destination buffer
    // It's important to make the CA output buffer slightly larger.
    u4DstBufSize = _DMX_Align(u4SrcBufSize + 1000, DMX_ES_FIFO_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstBufSize, DMX_ES_FIFO_ALIGNMENT);
    if (u4DstAddr == 0)
    {
        Printf("Failed to allocate destination buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    //clear dst buffer
    x_memset((void *)u4DstAddr, 0 , u4DstBufSize);

	/*allocate buffer for pes datas*/
    u4PesAddr = (UINT32)x_mem_alloc(u4SrcSize);
    if (u4PesAddr == 0)
    {
        Printf("Failed to allocate payload buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

	x_memset((VOID*)u4PesAddr,0,u4SrcSize);

    // Set PVR playback
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4SrcAddr;
    rPlay.u4BufSize = u4SrcBufSize;
	#if  0
    rPlay.u4ThresholdSize = u4SrcSize;  // FIXME
	#else
	rPlay.u4ThresholdSize = 0x1000000; //set this value to big values ,then pvr interrupt can't be trigger.
	#endif
    rPlay.pfnPVRNotify = NULL;
    u4Flags = PVRPLAY_FLAGS_BUFFER | PVRPLAY_FLAGS_THRESHOLD |
              PVRPLAY_FLAGS_CALLBACK;
    if (!_DMX_PVRPlay_Set(u1TsoutIdx, u4Flags, &rPlay))
    {
        Printf("Failed to set PVR properties!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    //Set Pid buffer
    if (!_DMX_SetPlayPid(u1TsIndex, u1PidIdx, u2Pid , u4DstAddr, u4DstBufSize))
    {
        Printf("Failed to set Pid buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);


    // The input to 8295 needs to be in TS packet format.
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4SrcSize, u2Pid, u4PesAddr,
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

    // Set Full pull mode
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    VERIFY(_DMX_PVRPlay_FlushBuf(u1TsoutIdx));
    VERIFY(_DMX_PVRPlay_UpdateWp(u1TsoutIdx, u4SrcAddr, u4SrcSize));
        //Change wp
    if(u1TsoutIdx == 0)
    {
        u4Wp=DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP);
    }
    else if(u1TsoutIdx == 1)
    {
        u4Wp=DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP2);
    }
	else
	{
	   u4Wp=DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_WP3);
	}
	
    VERIFY(_DMX_PVRPlay_Start(u1TsoutIdx));

    // Wait for some time.
    x_thread_delay(100);
	#if 0
    while (1)
    {
        VERIFY(DMX_PVRPlay_GetEx(u1TsoutIdx, PVRPLAY_FLAGS_BUFFER, &rPlay));
        if (rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }
    }
    #else
	
	if(u1TsoutIdx == 0)
	{
	   while(1)
	   	{
	       u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP);
		   if(u4Rp == u4Wp)
		   {
		     break;
		   }
	   	}
	}
	else if(u1TsoutIdx == 1)
	{
	   while(1)
	   	{
	       u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
		   if(u4Rp == u4Wp)
		   {
		     break;
		   }
	   	}
	}
	else
	{
	   while(1)
	   	{
	       u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP3);
		   if(u4Rp == u4Wp)
		   {
		     break;
		   }
	   	}
	}

	#endif
    // Check result
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize , &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }
    else
    {
        fgRet = TRUE;
    }

FailAndFreeMemory:
     // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1PidIdx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1PidIdx));

    VERIFY(_DMX_PVRPlay_Stop(u1TsoutIdx, TRUE));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsoutIdx));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    if (u4PesAddr != 0)
    {
        x_mem_free((void*)u4PesAddr);
    }

    // Set back
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= ~0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    return fgRet;
}

