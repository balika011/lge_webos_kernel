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

static HANDLE_T _hDmxDDIEmuSema = NULL_HANDLE;
static HANDLE_T _hDmxPVREmuSema = NULL_HANDLE;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

static BOOL _EMU_DDI_RecBypass_Transfer(UINT8 u1TsIndex, UINT32 u4Size)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 u4Cfg;
    FVR_GBL_T rGBL;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000; // + random(16);
    u4DstAddr = 0x3100000;// + random(16);

    printf("u4SrcAddr: 0x%x, u4DstAddr: 0x%x\n", u4SrcAddr, u4DstAddr);
    
    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 0x800, DMX_CA_BUFFER_ALIGNMENT);

    // Set bypass
    u4Cfg = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
    u4Cfg &= ~0x3C000000;
    u4Cfg |= (1 << (26 + u1TsIndex));
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Cfg);

    // Record bypass
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0x800000);

    DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0x0);

    VERIFY(FVR_Init());

    // Capture init
    x_memset((void*)&rGBL, 0, sizeof(rGBL));
    rGBL.fgAllocateBuffer = FALSE;
    rGBL.u2TargetPktCount = 0;
    rGBL.u4BufStart = u4DstAddr;
    rGBL.u4BufSize =  u4DstEnd - u4DstAddr;
    rGBL.pfnNotifyFunc = NULL;
    rGBL.pvNotifyTag = NULL;
    rGBL.u4TimeToNotify = 0;//1000;
    
    if(!_FVR_SetGBL(u1TsIndex, (UINT32)(FVR_GBL_FLAG_TARGET_PKT | FVR_GBL_FLAG_BUFFER | 
                    FVR_GBL_FLAG_TIME_INFO | FVR_GBL_FLAG_NOTIFY), &rGBL))
    {
        return FALSE;
    }

    VERIFY(_FVR_Start(u1TsIndex));

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    HalFlushInvalidateDCache();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Byte_Transfer(0, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    x_thread_delay(100);
    VERIFY(_FVR_Stop(u1TsIndex));

    HalFlushInvalidateDCache();

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    VERIFY(_FVR_FreeGBL(u1TsIndex));

    u4Cfg = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
    u4Cfg &= ~0x3C000000;
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Cfg);

    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0x0);
    DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0xF0C0F0C0);
    
    return fgRet;
}


static BOOL _EMU_DDI_PlayBypass_Transfer(UINT8 u1TsIndex, UINT32 u4Size)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff, u4Reg;
    BOOL fgRet = FALSE;
    DMX_PID_T rPid;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000;
    u4DstAddr = 0x3100000;

    printf("u4SrcAddr: 0x%x, u4DstAddr: 0x%x\n", u4SrcAddr, u4DstAddr);
    
    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

    // Set bypass
    u4Reg = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
    u4Reg &= ~0x3C000000;
    u4Reg |= (1 << (26 + u1TsIndex));
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Reg);

    // Record bypass
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0x20);
    DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0x0);

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = 0x1FFF;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = 0x100000;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    if (!_DMX_SetPid(0, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        fgRet = FALSE;
    }

    PID_INDEX_TABLE(0) = 0;
    u4Reg = PID_S_W(0, 0);
    u4Reg &= ~0x10;
    PID_S_W(0, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(0, 1) = u4Size;
    PID_S_W(0, 3) = (0 << 24) | 1;     // skip size = 0, and trigger    

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Byte_Transfer(0, u4SrcAddr, u4Size))
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

    if (!_DMX_FreePid(0))
    {
        fgRet = FALSE;
    }

    u4Reg = DMXCMD_READ32(DMX_REG_DBM_CONTROL);
    u4Reg &= ~0x3C000000;
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Reg);

    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0x0);
    DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0xF0C0F0C0);

    PID_INDEX_TABLE(0) = 0;

    return fgRet;
}

BOOL DMX_EMU_DDI_Bypass_Test(UINT8 u1TsIndex, BOOL fgRec)
{
    UINT32 u4Size;

    u4Size = random(0xF0000);
    u4Size -= (u4Size % 188);   // size must bb 188 alignment
    
    if(fgRec)
    {
        if(!_EMU_DDI_RecBypass_Transfer(u1TsIndex, u4Size))  
        {
            return FALSE;
        }
    }
    else
    {
        if(!_EMU_DDI_PlayBypass_Transfer(u1TsIndex, u4Size))
        {
            return FALSE;
        }
    }
    
    return TRUE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Below is DDI, PVR reset test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void _DDI_EMU_Notify(void)
{
    // DDI soft reset
    DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0);
    DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0x208);

    VERIFY(OSR_OK == x_sema_unlock(_hDmxDDIEmuSema));
}

static BOOL _DDIResetTransfer(UINT32 u4Addr, UINT32 u4Size, BOOL fgIntr)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, i;
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

    if(fgIntr)
    {
        _DMX_DDI_SetDMAInt(TRUE, FALSE);
    }

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    if(fgIntr)
    {
        VERIFY(OSR_OK == x_sema_lock(_hDmxDDIEmuSema, X_SEMA_OPTION_WAIT));
        return TRUE;
    }

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;

    while(1)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_REAL_RP);
        if (u4Rp == u4Wp)
        {
            //x_thread_delay(100);

            // DDI soft reset
            DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0);
            DDI_WRITE32(DDI_REG_GLOBAL_CTRL, 0x208);

            // Complete
            fgRet = TRUE;
            //Printf("   Wait count: %u\n", i);
            break;
        }
    }

    return fgRet;
}

BOOL DMX_EMU_DDI_ResetTransfer(UINT8 u1TsIndex, UINT32 u4Size, BOOL fgIntr)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    static BOOL _fgInit = FALSE;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if(!_fgInit)
    {
        VERIFY(OSR_OK == x_sema_create(&_hDmxDDIEmuSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
        _fgInit = TRUE;
    }

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
    if (!_DDIResetTransfer(u4SrcAddr, u4Size, fgIntr))
    {
        // Error on transferring data
        goto _exit;
    }

    x_thread_delay(200);

    HalFlushInvalidateDCache();

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

void _PVR_EMU_Notify(void)
{
    ASSERT((DMXCMD_READ32(224) & 0x1) == 0); // 0x17380, pvr read buffer status    

    DMXCMD_WRITE32(DMX_REG_PVR_CONTROL, 0x100);
    HAL_Delay_us(1);
    DMXCMD_WRITE32(DMX_REG_PVR_CONTROL, 0x0);

    VERIFY(OSR_OK == x_sema_unlock(_hDmxPVREmuSema));
}

static BOOL _PVR_ResetTransfer(UINT32 u4Addr, UINT32 u4Size, BOOL fgIntr)
{
    UINT32 u4BufStart, u4BufEnd, u4Wp, i;
    BOOL fgRet;
    DMX_PVR_PLAY_T rPlay;

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

    if(!_DMX_PVRPlay_FreeBuf())
    {
        return FALSE;
    }

    // Play init
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4BufStart;
    rPlay.u4BufSize = u4BufEnd + 0x100;
    if(fgIntr)
    {
        rPlay.u4ThresholdSize = u4Size;
    }
    else
    {
        rPlay.u4ThresholdSize = 0x10000000;     // Not usable
    }   
    rPlay.pfnPVRNotify = NULL;
    if(!_DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_THRESHOLD |PVRPLAY_FLAGS_BUFFER | 
                                 PVRPLAY_FLAGS_CALLBACK), &rPlay))
    {
        return FALSE;
    }    

    // Enable PLAY timestamp
    rPlay.fgContainTimeStamp = FALSE;
    rPlay.fgIgnoreTimeStamp = TRUE;
    rPlay.u2TimeStampFreqDiv = 8;    
    if(!_DMX_PVRPlay_Set((UINT32)PVRPLAY_FLAGS_TIMESTAMP, &rPlay))
    {
        return FALSE;
    }

    DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP, u4Wp);

    if(!_DMX_PVRPlay_Start())
    {
        return FALSE;
    }

    if(fgIntr)
    {
        VERIFY(OSR_OK == x_sema_lock(_hDmxPVREmuSema, X_SEMA_OPTION_WAIT));
        return TRUE;
    }

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;

    while(1)
    {
        UINT32 u4Rp;

        u4Rp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP);    
        if (u4Rp == u4Wp)
        {
            ASSERT((DMXCMD_READ32(224) & 0x1) == 0); // 0x17380, pvr read buffer status
            DMXCMD_WRITE32(DMX_REG_PVR_CONTROL, 0x100);
            HAL_Delay_us(1);
            DMXCMD_WRITE32(DMX_REG_PVR_CONTROL, 0x0);

            // Complete
            fgRet = TRUE;
            //Printf("   Wait count: %u\n", i);
            break;
        }
    }

    x_thread_delay(10);

    if(!_DMX_PVRPlay_Stop(TRUE))
    {
        return FALSE;
    }        

    return fgRet;
}

BOOL DMX_EMU_PVR_ResetTransfer(UINT8 u1TsIndex, UINT32 u4Size, BOOL fgIntr)
{
    static BOOL _fgInit = FALSE;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 i, u4Reg;

    ASSERT(u1TsIndex < MAX_TS_INDEX);

    if(!_fgInit)
    {
        VERIFY(OSR_OK == x_sema_create(&_hDmxPVREmuSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
        _fgInit = TRUE;
    }

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
    
    //// Below is set TS Out
    if (!DMX_TsOut_SetRoute(u1TsIndex, DMX_TSOUT_ROUTE_DBM_LOOPBACK))
    {
        Printf("Failed to set route!\n");
        return -1;
    }
    if (!DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_1))
    {
        Printf("Failed to set data width!\n");
        return -1;
    }  
    if (!DMX_TsIn_SetDataWidth(u1TsIndex, DMX_TSOUT_DATAWIDTH_1))
    {
        Printf("Failed to set data width!\n");
        return -1;
    }        
    if (!DMX_TsOut_SetEndianess(DMX_TSOUT_ENDIAN_MSB))
    {
        Printf("Failed to set endian mode!\n");
        return -1;
    }
    if (!DMX_TsOut_SetSyncCycle(DMX_TSOUT_SYNC_CYCLE_8))
    {
        Printf("Failed to set sync cycle!\n");
        return -1;
    }
    if (!DMX_TsOut_SetValidCycle(0))
    {
        Printf("Failed to set valid space!\n");
        return -1;
    }
    if (!DMX_TsOut_SetLatchEdge(DMX_TSOUT_LATCH_EDGE_NEGATIVE))
    {
        Printf("Failed to set Latch edge!\n");
        return -1;
    }
    //// Above is set TS Out

    // Set Full pull mode
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

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
    // Enable all demux interrupts
    DMXCMD_WRITE32(DMX_REG_INT_MASK, 0xffffffff);

    // Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEnd,
        u4DstEnd - u4DstAddr))
    {
        goto _exit;
    }

    if (!_PVR_ResetTransfer(u4SrcAddr, u4Size, fgIntr))
    {
        // Error on transferring data
        goto _exit;
    }

    x_thread_delay(200);

    HalFlushInvalidateDCache();

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


