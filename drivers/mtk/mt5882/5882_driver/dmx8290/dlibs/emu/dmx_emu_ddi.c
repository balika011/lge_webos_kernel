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
#include "dmx_emu_if.h"

#include "dmx_mm.h"
#include "fvr.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_drv_cli.h"
#include "x_hal_arm.h"


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

static HANDLE_T _hDmxFramerUnlockSema = NULL_HANDLE;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
void _Framer_Unlock_Notify(void)
{
    Printf("Framer Unlock Notify Success!\n");
    VERIFY(OSR_OK == x_sema_unlock(_hDmxFramerUnlockSema));
}


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
    VERIFY(BSP_FreeAlignedDmaMemory(_u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(_u4SrcAddr));

    _u4DstAddr = (_u4SrcAddr = 0);

    return FALSE;
}


BOOL DMX_EMU_DDI_Exit(void)
{
    // Free buffers
    VERIFY(BSP_FreeAlignedDmaMemory(_u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(_u4SrcAddr));

    _u4DstAddr = (_u4SrcAddr = 0);

    return TRUE;
}

BOOL DMX_EMU_DDI_To_PID_FILTER_Transfer(UINT8 u1TsIndex,BOOL b_match)
{
	DMX_PID_T rPid;
	UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
	UINT32 u4PesAddr, u4PesSize = 0, u4Flags,u4Reg, i;
	const UINT8 u1Pidx = 0;
	BOOL fgRet = FALSE;
    UINT32 u4Size = 18800;
    UINT32 u4_pid = 0x300;
	if(b_match)
	{/*if match ,let the datas pass ,error datas transfer error.*/
	  u4_pid=0x17c;
	}
	// Allocate source and destination buffers
	u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);

	u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1,DMX_CA_BUFFER_ALIGNMENT);

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
	//generate	golden data.
	for(i = 0; i <u4Size; i++)
	{
		*(BYTE *)(u4SrcAddr+i) = i % 0xff;
	}

	// Translate to TS format
	if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, 0x17c, u4PesAddr,
								 &u4PesSize))
	{
		goto _exit;
	}

	Printf("TS size %u, PES size = %u\n", u4Size, u4PesSize);

	u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

	// Setup PID
	x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
	rPid.fgEnable = TRUE;
	rPid.fgAllocateBuffer = TRUE;
	rPid.fgPrimary = TRUE;
	rPid.u1TsIndex = u1TsIndex;
	rPid.u1DeviceId = 0;
	rPid.u1KeyIndex = 0;
	rPid.u1SteerMode = DMX_STEER_TO_FTUP;
	rPid.u2Pid = u4_pid;
	rPid.ePcrMode = DMX_PCR_MODE_NONE;
	rPid.ePidType = DMX_PID_TYPE_PES;
	rPid.eDescMode = DMX_DESC_MODE_NONE;
	rPid.pfnNotify = NULL;
	rPid.pvNotifyTag = NULL;
	rPid.pfnScramble = NULL;
	rPid.pvScrambleTag = NULL;

	u4Flags = DMX_PID_FLAG_TS_INDEX|DMX_PID_FLAG_PID|DMX_PID_FLAG_VALID;
	if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
	{
		return FALSE;
	}

	/*set register ,hardware path*/
	
    /*set dbm port*/
    if ((u1TsIndex == 2) || (u1TsIndex == 3) || (u1TsIndex == 4))
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);      
    }
    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (3 << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);
   /*set dbm ,not do bypass*/
   if(u1TsIndex != 4)
   {
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<(26+u1TsIndex)));
   }
   else
   {
     DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, DMXCMD_READ32(DMX_REG_DBM_CONTROL)&~(1<<25));
   }

   //set  pid filter 
  
	Printf("(Before)Pid index table: 0x%08x\n", PID_INDEX_TABLE(u1Pidx));


	
	// Set CA output buffer
	if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEnd,
								u4DstEnd - u4DstAddr))
	{
		Printf("set ca output error.\r\n");
		goto _exit;
	}

	// Invoke DDI transfer
	if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
	{
		// Error on transferring data
		goto _exit;
	}

	Printf("(After)Pid index table: 0x%08x\n", PID_INDEX_TABLE(u1Pidx));

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



BOOL DMX_EMU_DDI_Transfer0(UINT8 u1Type, UINT8 u1TsIndex, UINT32 u4Size ,PUL_MODE_T eMode , BOOL fgBypassFramer)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff, u4Reg;
    BOOL fgRet = FALSE;
//    static UINT32 _u4Size = 0x1000 - (0x1000 % 188);

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if ((u1Type != 0) && (u1Type != 1))
    {
        return FALSE;
    }

//    u4Size = _u4Size;

    // Allocate source and destination buffers
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);
    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    Printf("src: 0x%08x, dst: 0x%08x\n", u4SrcAddr, u4DstAddr);

    //src and dst memset 0
    x_memset((VOID *)u4SrcAddr, 0 , u4Size);
    x_memset((VOID *)u4DstAddr, 0 , u4Size);

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

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
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    return fgRet;
}


BOOL DMX_EMU_DDI_Transfer1(UINT8 u1Type, UINT32 u4Size,PUL_MODE_T eMode)
{
    UINT32 u4SrcAddr, u4DstBufStart, u4DstBufEnd, u4DstWp, u4Diff,u4Reg;
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

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (eMode << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

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
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

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


BOOL DMX_EMU_FramerUnlock_Test(UINT8 u1TsIndex, UINT32 u4Size ,PUL_MODE_T eMode )
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4Reg;
    BOOL fgRet = FALSE;
    UINT32 u4PesAddr = 0, u4PesSize;
    UINT16 u2Pid = 0x100, i;
    UINT8* pu1TsAddr = NULL;
    UINT32 u4Threshold = 500, u4Ctrl;
    static BOOL _fgInit = FALSE;


    ASSERT(u1TsIndex <= MAX_TS_INDEX);
    ASSERT(u4Threshold * 188 <= u4Size);

    if (!_fgInit)
    {
        VERIFY(OSR_OK == x_sema_create(&_hDmxFramerUnlockSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
        _fgInit = TRUE;
    }

    // Allocate source and destination buffers
    u4SrcAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);

    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1,
                DMX_CA_BUFFER_ALIGNMENT);

    u4PesAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    Printf("src: 0x%08x, dst: 0x%08x, PesAddr: 0x%08x.\n", u4SrcAddr, u4DstAddr, u4PesAddr);

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

    //src and dst memset 0
    x_memset((VOID *)u4SrcAddr, 0 , u4Size);
    x_memset((VOID *)u4DstAddr, 0 , u4Size);
    x_memset((VOID *)u4PesAddr, 0 , u4Size);

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    //Generate 1100 Framer unlock pattern
    //From first five packet ,
    pu1TsAddr = (UINT8 *)u4SrcAddr;
    for (i = 0; i < u4Threshold ; i++)
    {
        ASSERT(pu1TsAddr[(i+5) * 188] == 0x47);
        pu1TsAddr[(i+5) * 188] = 0;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    //Enalbe Framer unlock check
    u4Ctrl = DMXCMD_READ32(DMX_REG_FRAMER_UNLOCK_CONTROL) & 0xFFFE0000;
    u4Ctrl |= ((UINT32)0x1 << 16);//enable
    u4Ctrl |= ((UINT32)0x3E8);//threshold 1000 packet
    DMXCMD_WRITE32(DMX_REG_FRAMER_UNLOCK_CONTROL, u4Ctrl);


    // Enable all demux interrupts
    DMXCMD_WRITE32(DMX_REG_INT_MASK, 0xffffffff);

    // Set CA output buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEnd,
                                u4DstEnd - u4DstAddr))
    {
        goto _exit;
    }

    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
#if defined(CC_MT5890)
    if (u1TsIndex == 2)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
#endif

    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
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

    //Change framer mode to internal sync
    _DMX_SetFramerMode(u1TsIndex,DMX_FRAMER_PARALLEL, FALSE, TRUE);

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

    // Check result wait DBM error ISR
    //VERIFY(OSR_OK == x_sema_lock(_hDmxFramerUnlockSema, X_SEMA_OPTION_WAIT));

    fgRet = TRUE;

_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4PesAddr));

    return fgRet;
}


static BOOL DDI_NOTIFY(DDI_EVENT_CODE_T eCode)
{
    //Printf("%s %d\n", __FUNCTION__, eCode);
    return TRUE;
}

BOOL DMX_EMU_DDI_UpdateWp()
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4Size = 2800 * 188, u4FilledSize;
    UINT32 u4StepSize = 4 * 188, u4Sent = 0;
    UINT32 u4PesAddr = 0, u4PesSize;
    DMX_DDI_T rDDI;
    UINT8 u1Inst, u1Pidx;
    DMX_PID_T rPid;
    HAL_TIME_T rTime1, rTime2, rTimeDiff;

    u4SrcAddr = 0x3000000;
    u4DstAddr = 0x3100000;

    printf("u4SrcAddr: 0x%x, u4DstAddr: 0x%x\n", u4SrcAddr, u4DstAddr);

    CLI_Parser("d.i");

    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    u1Inst = _DMX_MUL_GetAvailableInst(DMX_CONN_TYPE_BUFAGENT_0);
    u1Pidx = _DMX_MUL_GetAvailablePidx(u1Inst);

    x_memset(&rPid, 0, sizeof(rPid));
    rPid.u1TsIndex = 0;
    rPid.fgEnable = TRUE;
    rPid.u2Pid = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.ePidType = DMX_PID_TYPE_TS_RAW;
    rPid.fgAllocateBuffer = FALSE;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = 0x100000;
    rPid.fgPrimary = TRUE;
    rPid.u1ChannelId = 0;

    if(!DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL, &rPid))
    {
        Printf("fail to set pid\n");
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    u4PesAddr = (UINT32)x_mem_alloc(u4Size);
    if (u4PesAddr == 0)
    {
        Printf("Failed to allocate payload buffer!\n");
        goto _exit;
    }

    // The input to 8295 needs to be in TS packet format.
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, 0, u4PesAddr,
        &u4PesSize))
    {
        Printf("Failed to generate TS packets!\n");
        goto _exit;
    }

    HalFlushInvalidateDCache();

    //u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_CA_BUFFER_ALIGNMENT);

    // Init DDI
    _DMX_DDI_SetPortEx(DMX_DDI_PORT_FRAMER0, u1Pidx, TRUE, FALSE);

    if(!_DMX_SetFramerPktSize(0, 188))
    {
        return FALSE;
    }

    if(!_DMX_DDI_SetPacketSize(0, 188))
    {
        return FALSE;
    }

    rDDI.eMode = DMX_DDI_MODE_STREAM;
    rDDI.u4BufAddr = u4SrcAddr;
    rDDI.u4BufSize = u4Size;
    rDDI.u4Threshold = 0;
    rDDI.u4RateN = 0;
    rDDI.u4RateN = 0;
    rDDI.pfnDDINotify = DDI_NOTIFY;

    if (!DMX_DDI_Set(DMX_DDI_FLAG_ALL, &rDDI))
    {
        goto _exit;
    }

    if (!_DMX_DDI_StreamStart())
    {
        goto _exit;
    }

    HAL_GetTime(&rTime1);

    for(u4Sent = 0; u4Sent < u4Size; u4Sent+=u4StepSize)
    {
        if (!_DMX_DDI_FillBuf(u4SrcAddr + u4Sent, u4StepSize, &u4FilledSize))
        {
            goto _exit;
        }

        if(u4StepSize != u4FilledSize)
        {
            Printf("Wrong Size, buffer is full?\n");
        }

        //x_thread_delay(10);
    }

    HAL_GetTime(&rTime2);
    HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
    Printf("DDI time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);

    x_thread_delay(10);

    HalFlushInvalidateDCache();

    // Check result, ignore some data in hardware buffer " - u4StepSize"
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size - u4StepSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    else
    {
        Printf("Compare OK!\n");
    }

    if (!_DMX_DDI_StreamStop())
    {
        goto _exit;
    }

    _DMX_FreePid(u1Pidx);
    _DMX_MUL_FreePidx(u1Pidx);

    return 0;

_exit:
    return 1;
}

