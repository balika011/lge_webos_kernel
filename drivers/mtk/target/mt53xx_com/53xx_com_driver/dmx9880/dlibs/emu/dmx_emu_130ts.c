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
 * $RCSfile: dmx_emu_130ts.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_130ts.c
 *  Demux driver - Multi2 cipher emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_emu_if.h"
#include "dmx_emu_130_source.h"
#include "dmx_emu_130_pid_064.h"
#include "dmx_emu_130_pid_078.h"
#include "dmx_mm.h"
//#include "../fvr.h"
#include "x_lint.h"

//LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_926.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"

//LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//#define TS_TSINDEX  0

#define TS_064_PID_IDX  1

#define TS_064_PID  0x64

#define TS_078_PID_IDX  2

#define TS_078_PID  0x78


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _u4TsSourceBufAddr;
static UINT32 _u4TsSourceBufSize;

static UINT32 _u4TsPid_064GoldenAddr;
static UINT32 _u4TsPid_064GoldenSize;

static UINT32 _u4TsPid_064BufAddr;
static UINT32 _u4TsPid_064BufSize;
static BOOL   _fgTsPid_064_Set;

static UINT32 _u4TsPid_078GoldenAddr;
static UINT32 _u4TsPid_078GoldenSize;

static UINT32 _u4TsPid_078BufAddr;
static UINT32 _u4TsPid_078BufSize;
static BOOL   _fgTsPid_078_Set;


static HANDLE_T _hDmxEmuDDIThread_MultiSize = NULL_HANDLE;
static HANDLE_T _hDmxEmuPVRThread_MultiSize = NULL_HANDLE;
static volatile BOOL _fgDmxEmuDDIEnable_MultiSize = FALSE;
static volatile BOOL _fgDmxEmuPVREnable_MultiSize = FALSE;

static HANDLE_T _hDmxEmuDDISema_MultiSize = NULL_HANDLE;
static HANDLE_T _hDmxEmuPVRSema_MultiSize = NULL_HANDLE;

//-----------------------------------------------------------------------------
/** _DMX_ResetFramer
 */
//-----------------------------------------------------------------------------
void _DMX_Emu_ResetFramer(UINT8 u1Framer)
{
    UINT32 u4Ctrl;

	//enable bit to reset the DBM some framer releated register
	u4Ctrl = DMXCMD_READ32(DMX_REG_PCR_NUM_CONTROL);
	u4Ctrl |= (0x1 << 5);
	DMXCMD_WRITE32(DMX_REG_PCR_NUM_CONTROL, u4Ctrl);

    // Reset framer - bit12 for framer0 reset, bi13 for framer 1 reset
    if(u1Framer == 0)
    {
        u4Ctrl = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & (~0x1000);
        u4Ctrl |= 0x1000;
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Ctrl);
        //x_thread_delay(1);
        HAL_Delay_us(1);
        u4Ctrl &= (~0x1000);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Ctrl);
    }
    else if(u1Framer == 1)
    {
        u4Ctrl = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & (~0x2000);
        u4Ctrl |= 0x2000;
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Ctrl);
        HAL_Delay_us(1);
        u4Ctrl &= (~0x2000);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Ctrl);
    }
    else if(u1Framer == 2)
    {
        u4Ctrl = DMXCMD_READ32(DMX_REG_FRAMER_CTRL1);
        u4Ctrl |= 0x10;
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, u4Ctrl);
        HAL_Delay_us(1);
        u4Ctrl &= ~0x10;
        DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL1, u4Ctrl);
    }
    else if(u1Framer == 3)
    {
        u4Ctrl = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & (~0x8000);
        u4Ctrl |= 0x8000;
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Ctrl);
        //x_thread_delay(1);
        HAL_Delay_us(1);
        u4Ctrl &= (~0x8000);
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Ctrl);
    }
    else
    {
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void _DMX_Emu_130Ts_Init(void)
{
    _u4TsSourceBufAddr = 0;
    _u4TsSourceBufSize = 0;

    _u4TsPid_064GoldenAddr = 0;
    _u4TsPid_064GoldenSize = 0;

    _u4TsPid_064BufAddr = 0;
    _u4TsPid_064BufSize = 0;
    _fgTsPid_064_Set = FALSE;

	_u4TsPid_078GoldenAddr = 0;
    _u4TsPid_078GoldenSize = 0;

    _u4TsPid_078BufAddr = 0;
    _u4TsPid_078BufSize = 0;
    _fgTsPid_078_Set = FALSE;

}

static BOOL _DMX_Emu_130Ts_Cleanup(void)
{
    DMX_PID_T rPid;

    if (_fgTsPid_064_Set)
    {
        // Disable PID, and then free PID.
        rPid.fgEnable = FALSE;
        VERIFY(_DMX_SetPid(TS_064_PID_IDX,DMX_PID_FLAG_VALID,&rPid,FALSE));
        VERIFY(_DMX_FreePid(TS_064_PID_IDX));
    }

	if (_fgTsPid_078_Set)
    {
        // Disable PID, and then free PID.
        rPid.fgEnable = FALSE;
        VERIFY(_DMX_SetPid(TS_078_PID_IDX,DMX_PID_FLAG_VALID,&rPid,FALSE));
        VERIFY(_DMX_FreePid(TS_078_PID_IDX));
    }

    return TRUE;
}

static BOOL _DMX_Emu_130Ts_SetPid(UINT8 u1TsIndex, UINT8 u1Pidx, UINT16 u2Pid, DMX_PID_TYPE_T ePidType,
                        UINT32 u4BufAddr, UINT32 u4BufSize, BOOL fgDescramble)
{
    DMX_PID_T rPid;
    UINT32 u4BufEnd, u4Flags;

    u4BufEnd = _DMX_Align(u4BufAddr + u4BufSize, DMX_ES_FIFO_ALIGNMENT);

    // Setup PID
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4BufAddr;
    rPid.u4BufSize = u4BufEnd - u4BufAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = ePidType;      // Set the Output mode bit
    rPid.eDescMode = (fgDescramble) ? DMX_DESC_MODE_MULTI2_BIG : DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        Printf("_DMX_SetPid() returns FALSE!\n");
		return FALSE;
    }

	x_memset((void*)u4BufAddr, 0, u4BufEnd - u4BufAddr);

    return TRUE;
}
static BOOL _DMX_Emu_130Ts_PrepareInput_Data(void)
{
	_u4TsSourceBufSize = sizeof(au1130TsPattern);
	_u4TsPid_064GoldenSize = sizeof(au1130TsPid_064);
	_u4TsPid_078GoldenSize = sizeof(au1130TsPid_078);

    //check source size alignment
	 if (!_DMX_IsAligned(_u4TsSourceBufSize, 130))
    {
        Printf("Source size is not aligned to 130!\n");
        return FALSE;
    }

	//check golden size alignment
	 if (!_DMX_IsAligned(_u4TsPid_064GoldenSize, 130))
    {
        Printf("Pid_064 golden size is not aligned to 130!\n");
        return FALSE;
    }

	 if (!_DMX_IsAligned(_u4TsPid_078GoldenSize, 130))
    {
        Printf("Pid_064 golden size is not aligned to 130!\n");
        return FALSE;
    }

	//check pid_064 golden addr
	_u4TsPid_064GoldenAddr = (UINT32)au1130TsPid_064;
	_u4TsPid_078GoldenAddr = (UINT32)au1130TsPid_078;
	if ((_u4TsPid_064GoldenAddr == 0) || (_u4TsPid_078GoldenAddr == 0))
	{
		Printf("Pid 064/078 golden address is 0.");
		return FALSE;
	}

	// Allocate buffers.  Make sure the ES FIFO is at least 768 bytes larger
	// than the size of the input data.
	_u4TsPid_064BufSize  = 0x100000; //1MB is enough
	_u4TsPid_078BufSize  = 0x100000; //1MB is enough

	//allocate fixed address.
	_u4TsSourceBufAddr   =  0x3400000;
	_u4TsPid_064BufAddr  =  0x3A00000;
	_u4TsPid_078BufAddr  =  0x3C00000;

	//clean pid buffer
	x_memset((void*)_u4TsPid_064BufAddr, 0, _u4TsPid_064BufSize);
	x_memset((void*)_u4TsPid_078BufAddr, 0, _u4TsPid_078BufSize);
	
	//prepare 130ts data
    x_memcpy((void*)_u4TsSourceBufAddr, (const void*)au1130TsPattern, _u4TsSourceBufSize);

    HalFlushInvalidateDCache();

	Printf("Source address: 0x%08x (size: %x).\n", _u4TsSourceBufAddr, _u4TsSourceBufSize);
	Printf("Pid 064 Golden address: 0x%08x(size : %x).\n", _u4TsPid_064GoldenAddr , _u4TsPid_064GoldenSize);
	Printf("Pid 064 Buffer address: 0x%08x(size : %x).\n", _u4TsPid_064BufAddr ,_u4TsPid_064BufSize);
	Printf("Pid 078 Golden address: 0x%08x(size : %x).\n", _u4TsPid_078GoldenAddr , _u4TsPid_078GoldenSize);
	Printf("Pid 078 Buffer address: 0x%08x(size : %x).\n", _u4TsPid_078BufAddr ,_u4TsPid_078BufSize);
	return TRUE;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
 BOOL DMX_Emu_130TsTest(BOOL fgExtSync ,UINT8 u1Tsindex)
{
	 UINT32 u4Pid_064GoldenDiff, u4Pid_064BufferDiff;
	 UINT32 u4Pid_078GoldenDiff, u4Pid_078BufferDiff;
	 BOOL fgRet = FALSE;
	 UINT32 u4Reg;

    _DMX_Emu_130Ts_Init();

	if (!_DMX_Emu_130Ts_PrepareInput_Data())
	{
		return FALSE;
	}

    // Set 064 Pid
    if (!_DMX_Emu_130Ts_SetPid(u1Tsindex, TS_064_PID_IDX, TS_064_PID, DMX_PID_TYPE_PES,
                            _u4TsPid_064BufAddr, _u4TsPid_064BufSize, FALSE))
    {
        Printf("Cannot set 064 Pid!\n");
        goto _exit;
    }
    _fgTsPid_064_Set = TRUE;

	// Set 078 Pid
    if (!_DMX_Emu_130Ts_SetPid(u1Tsindex, TS_078_PID_IDX, TS_078_PID, DMX_PID_TYPE_PES,
                            _u4TsPid_078BufAddr, _u4TsPid_078BufSize, FALSE))
    {
        Printf("Cannot set 078 Pid!\n");
        goto _exit;
    }
    _fgTsPid_078_Set = TRUE;

	//Set ExternalFramer DMX_EXT_FRAMER_2_DDI_3_EXT
	_DMX_SetExternalFramer(DMX_EXT_FRAMER_2_DDI_3_EXT);

	// Init DDI,ts index 0
    if (!_DMX_EMU_DDI_Init(u1Tsindex))
    {
        Printf("_DMX_EMU_DDI_Init() returns FALSE!\n");
        goto _exit;
    }

	// Set 130 byte DDI enable
    if (!DMX_DDI_130byteEnable(TRUE , 130))
    {
        Printf("Set 130 byte enable returns FALSE!\n");
        goto _exit;
    }

	// Set 130 byte Framer enable
	// enable both framer 0 and framer 3 for FPGA emulation.
	// DDI connect both frmar 0 and 3,if we don't set 130 byte framer enable
	// one of framer will use the 188 packet size as difault value.
	if (!_DMX_Framer_130byteEnable(0 ,FALSE , FALSE , 130 , TRUE , 130))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto _exit;
	}

	if (!_DMX_Framer_130byteEnable(3 ,FALSE , FALSE , 130 , TRUE , 130))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto _exit;
	}

	//disable framer error handling ,framer 0  framer 3
	_DMX_BypassErrorHandlingTable_Enable(0, TRUE);
	_DMX_BypassErrorHandlingTable_Enable(3, TRUE);

	//disable framer befor reset framer
	u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);
	u4Reg &= 0xFFFFF000;
	DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);

    // Set framer, must after setpktsize .set external sync
    u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & 0xFFF0F000;
	u4Reg |= 0x30000;
	if(fgExtSync)
	{
		u4Reg |= 0xFFF;
	}
	else
	{
		u4Reg |= 0xBFB;
	}
	DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);

    // Invoke DDI transfer (Type 5)
    if (!_DMX_EMU_DDI_Transfer(5, _u4TsSourceBufAddr, _u4TsSourceBufSize))
    {
        Printf("_DMX_EMU_DDI_Transfer() returns FALSE!\n");
        goto _exit;
    }

    // Compare the result with golden pattern.
    if(!fgExtSync)
    {
		if (!_MemCmp(((UINT32)(_u4TsPid_064GoldenAddr + 130)), (UINT32)_u4TsPid_064BufAddr, (_u4TsPid_064GoldenSize - 130), &u4Pid_064GoldenDiff, &u4Pid_064BufferDiff))
	    {
	       Printf("Differ at PID-064 Golden: 0x%08x, PID-064 Buffer: 0x%08x\n", u4Pid_064GoldenDiff, u4Pid_064BufferDiff);
	       goto _exit;
	    }
		if (!_MemCmp(((UINT32)_u4TsPid_078GoldenAddr + 130), (UINT32)_u4TsPid_078BufAddr, (_u4TsPid_078GoldenSize - 130), &u4Pid_078GoldenDiff, &u4Pid_078BufferDiff))
	    {
	       Printf("Differ at PID-078 Golden: 0x%08x, PID-078 Buffer: 0x%08x\n", u4Pid_078GoldenDiff, u4Pid_078BufferDiff);
	       goto _exit;
	    }
	}
	else
	{
		if (!_MemCmp(((UINT32)_u4TsPid_064GoldenAddr), (UINT32)_u4TsPid_064BufAddr, _u4TsPid_064GoldenSize, &u4Pid_064GoldenDiff, &u4Pid_064BufferDiff))
	    {
	       Printf("Differ at PID-064 Golden: 0x%08x, PID-064 Buffer: 0x%08x\n", u4Pid_064GoldenDiff, u4Pid_064BufferDiff);
	       goto _exit;
	    }
		if (!_MemCmp(((UINT32)_u4TsPid_078GoldenAddr), (UINT32)_u4TsPid_078BufAddr, _u4TsPid_078GoldenSize, &u4Pid_078GoldenDiff, &u4Pid_078BufferDiff))
	    {
	       Printf("Differ at PID-078 Golden: 0x%08x, PID-078 Buffer: 0x%08x\n", u4Pid_078GoldenDiff, u4Pid_078BufferDiff);
	       goto _exit;
	    }
	}
    Printf("Pid 064 data compare with golden data ok\n");
    Printf("Pid 078 data compare with golden data ok\n");

    Printf("Pass! (Source data size: %d)\n", _u4TsSourceBufSize);
	fgRet = TRUE;

_exit:

    _DMX_Emu_130Ts_Cleanup();

	//enable framer error handling ,framer 0  framer 3
	_DMX_BypassErrorHandlingTable_Enable(0, FALSE);
	_DMX_BypassErrorHandlingTable_Enable(3, FALSE);

    return fgRet;
}

BOOL DMX_Emu_130Ts_MultiPktSize(UINT8 u1Tsindex)
{
	 BOOL fgRet = FALSE;
	 UINT32 u4BufStart, u4BufEnd, u4Wp;

    _DMX_Emu_130Ts_Init();

	if (!_DMX_Emu_130Ts_PrepareInput_Data())
	{
		return FALSE;
	}

    // Set 064 Pid
    if (!_DMX_Emu_130Ts_SetPid(u1Tsindex, TS_064_PID_IDX, TS_064_PID, DMX_PID_TYPE_PES,
                            _u4TsPid_064BufAddr, _u4TsPid_064BufSize, FALSE))
    {
        Printf("Cannot set 064 Pid!\n");
        goto _exit;
    }
    _fgTsPid_064_Set = TRUE;

	// Set 078 Pid
    if (!_DMX_Emu_130Ts_SetPid(u1Tsindex, TS_078_PID_IDX, TS_078_PID, DMX_PID_TYPE_PES,
                            _u4TsPid_078BufAddr, _u4TsPid_078BufSize, FALSE))
    {
        Printf("Cannot set 078 Pid!\n");
        goto _exit;
    }
    _fgTsPid_078_Set = TRUE;

	//Set ExternalFramer DMX_EXT_FRAMER_2_DDI_3_EXT
	_DMX_SetExternalFramer(DMX_EXT_FRAMER_2_DDI_3_EXT);

	// Init DDI,ts index 0
    if (!_DMX_EMU_DDI_Init(u1Tsindex))
    {
        Printf("_DMX_EMU_DDI_Init() returns FALSE!\n");
        goto _exit;
    }

	// Set 130 byte DDI enable
    if (!DMX_DDI_130byteEnable(TRUE , 130))
    {
        Printf("Set 130 byte enable returns FALSE!\n");
        goto _exit;
    }

	// Set 130 byte Framer enable
	// enable both framer 0 and framer 3 for FPGA emulation.
	// DDI connect both frmar 0 and 3,if we don't set 130 byte framer enable
	// one of framer will use the 188 packet size as difault value.
	if (!_DMX_Framer_130byteEnable(0 ,FALSE , FALSE , 130 , TRUE , 130))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto _exit;
	}

	if (!_DMX_Framer_130byteEnable(3 ,FALSE , FALSE , 130 , TRUE , 130))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto _exit;
	}

	//disable framer error handling ,framer 0  framer 3
	_DMX_BypassErrorHandlingTable_Enable(0, TRUE);
	_DMX_BypassErrorHandlingTable_Enable(3, TRUE);


    // Invoke DDI transfer (Type 5)
    if (!_DMX_IsAligned(_u4TsSourceBufAddr, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        Printf("Incorrect address alignment!\n");
        goto _exit;
    }

    u4BufStart = PHYSICAL(_u4TsSourceBufAddr);
	u4Wp = _u4TsSourceBufAddr + _u4TsSourceBufSize ;
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

	fgRet = TRUE;
	return fgRet;

_exit:

    _DMX_Emu_130Ts_Cleanup();

	//enable framer error handling ,framer 0  framer 3
	_DMX_BypassErrorHandlingTable_Enable(0, FALSE);
	_DMX_BypassErrorHandlingTable_Enable(3, FALSE);

    return fgRet;
}


static BOOL _FVREmuSetPVRPlay(UINT32 u4SrcAddr,UINT32 u4DataSize)
{
    DMX_PVR_PLAY_T rPlay;

    // Play init
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4SrcAddr;
    rPlay.u4BufSize = u4DataSize;
    rPlay.u4ThresholdSize = 0x10000000;     // Not usable
    rPlay.pfnPVRNotify = NULL;

    if(!_DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_THRESHOLD |PVRPLAY_FLAGS_BUFFER |
                                 PVRPLAY_FLAGS_CALLBACK), &rPlay))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _FVREmuSetPlayPid(UINT8 u1TsIdx , UINT16 u2Pid ,UINT32 u4BufAddr ,UINT32 u4BufSize)
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

    if (!_DMX_SetPid(0, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _FVREmuSetTimestamp(BOOL fgOn)
{
    DMX_PVR_PLAY_T rPlay;

    if(fgOn)
    {
        rPlay.eMode = DMX_PVR_PLAY_STREAM;
    }
    else
    {
        rPlay.eMode = DMX_PVR_PLAY_SINGLEMOVE;
    }

    rPlay.fgContainTimeStamp = FALSE;
    rPlay.fgIgnoreTimeStamp = !fgOn;
    rPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    if(!DMX_PVRPlay_Set((UINT32)(PVRPLAY_FLAGS_MODE | PVRPLAY_FLAGS_TIMESTAMP), &rPlay))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** DMX_Emu_Tsout180_TransmitData
 */
//-----------------------------------------------------------------------------
BOOL DMX_Emu_Tsout188_TransmitData( UINT8 u1TsIndex)
{
    BOOL fgRet = FALSE;
    UINT32 u4SrcBufSize, u4DstBufSize;
    UINT32 u4SrcAddr = 0, u4SrcDiff, u4SrcSize;
    UINT32 u4DstAddr = 0, u4DstDiff;
    UINT32 u4PesAddr = 0, u4PesSize ;
    DMX_PVR_PLAY_T rPlay;
	//UINT8 u1TsIndex = 0x1;
	UINT32 u4Size = 0x10000;
	UINT16 u2Pid = 0x100;
	DMX_PID_T rPid;
	
	//VERIFY(DMX_TsOut_SetRoute(u1TsIndex, DMX_TSOUT_ROUTE_DBM_LOOPBACK));
	_DMX_SetFrontEndEx(u1TsIndex, DMX_FE_TSOUT);
	VERIFY(DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_1));
	VERIFY(DMX_TsIn_SetDataWidth(u1TsIndex, DMX_TSOUT_DATAWIDTH_1));
	VERIFY(DMX_TsOut_SetEndianess(DMX_TSOUT_ENDIAN_MSB));
	VERIFY(DMX_TsOut_SetSyncCycle(DMX_TSOUT_SYNC_CYCLE_8));
	VERIFY(DMX_TsOut_SetValidCycle(0));
	VERIFY(DMX_TsOut_SetLatchEdge(DMX_TSOUT_LATCH_EDGE_NEGATIVE));

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
    u4SrcBufSize = u4SrcSize ;
    u4SrcAddr = 0x3000000;

    // Allocate destination buffer
    u4DstBufSize = 0x20000;
    u4DstAddr = 0x3200000;

	x_memset((void*)u4DstAddr, 0, u4DstBufSize);
    //u4DstEndAddr = u4DstAddr + u4DstBufSize;

	//Allocate pes address
	u4PesAddr = (UINT32)x_mem_alloc(u4SrcSize);
    if (u4PesAddr == 0)
    {
        Printf("Failed to allocate payload buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

	//Set pvr play back
	VERIFY(_FVREmuSetPVRPlay(u4SrcAddr ,u4SrcBufSize));

	VERIFY(_FVREmuSetPlayPid(u1TsIndex,u2Pid , u4DstAddr , u4DstBufSize));

	VERIFY(_FVREmuSetTimestamp(FALSE));

	VERIFY(DMX_PVRPlay_SetBuffer(u4SrcAddr, u4SrcAddr + u4Size + 0x500 - 1,
							  u4SrcAddr + u4SrcBufSize, u4SrcAddr));

	//Generate Ts pattern
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

    Printf("Src address: 0x%08x(Size: %x!)\n", u4SrcAddr ,u4SrcBufSize );
    Printf("Dst address: 0x%08x(Size: %x!)\n", u4DstAddr ,u4DstBufSize);

	//set 130 byte play back disable, default packet size is 188
	 if (!_DMX_PVRPlay_130byteEnable(FALSE, 188))
    {
        Printf("Set 130 byte enable returns FALSE!\n");
        goto FailAndFreeMemory;
    }

	// Set 130 byte Framer disable, default packet size is 188
	if (!_DMX_Framer_130byteEnable(u1TsIndex , FALSE ,FALSE , 188 , FALSE , 188))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto FailAndFreeMemory;
	}

	//Begin to transfer
    VERIFY(DMX_PVRPlay_Start());

    while(1)
    {
        VERIFY(DMX_PVRPlay_Get(PVRPLAY_FLAGS_BUFFER, &rPlay));
        if(rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }
        x_thread_delay(5);
    }

    VERIFY(DMX_PVRPlay_Stop(TRUE));



    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr,u4SrcSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }
    else
    {
        fgRet = TRUE;
    }

FailAndFreeMemory:
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf());
    if (u4PesAddr != 0)
    {
        x_mem_free((void*)u4PesAddr);
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
/** DMX_Emu_Tsout130_TransmitData
 */
//-----------------------------------------------------------------------------
BOOL DMX_Emu_Tsout130_TransmitData( BOOL fgExtSync ,UINT8 u1TsIndex)
{

	 UINT32 u4Pid_064GoldenDiff, u4Pid_064BufferDiff;
	 UINT32 u4Pid_078GoldenDiff, u4Pid_078BufferDiff;
	 BOOL fgRet = FALSE;
	 UINT32 u4Reg;
	 DMX_PVR_PLAY_T rPlay;
	 UINT32 u4BufEnd;
	 //UINT8 u1TsIndex = 0x1;

	//VERIFY(DMX_TsOut_SetRoute(u1TsIndex, DMX_TSOUT_ROUTE_DBM_LOOPBACK));
	_DMX_SetFrontEndEx(u1TsIndex, DMX_FE_TSOUT);
	VERIFY(DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_1));
	VERIFY(DMX_TsIn_SetDataWidth(u1TsIndex, DMX_TSOUT_DATAWIDTH_1));
	VERIFY(DMX_TsOut_SetEndianess(DMX_TSOUT_ENDIAN_MSB));
	VERIFY(DMX_TsOut_SetSyncCycle(DMX_TSOUT_SYNC_CYCLE_8));
	VERIFY(DMX_TsOut_SetValidCycle(0));
	VERIFY(DMX_TsOut_SetLatchEdge(DMX_TSOUT_LATCH_EDGE_NEGATIVE));

	//prepare 130 ts input data
	 _DMX_Emu_130Ts_Init();

	if (!_DMX_Emu_130Ts_PrepareInput_Data())
	{
		return FALSE;
	}
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

	//Set pvr play back
	VERIFY(_FVREmuSetPVRPlay(_u4TsSourceBufAddr ,_u4TsSourceBufSize));

	VERIFY(_FVREmuSetTimestamp(FALSE));

	u4BufEnd = _DMX_Align(_u4TsSourceBufAddr + _u4TsSourceBufSize, DMX_ES_FIFO_ALIGNMENT);

	VERIFY(DMX_PVRPlay_SetBuffer(_u4TsSourceBufAddr, u4BufEnd - 1,
							  _u4TsSourceBufAddr + _u4TsSourceBufSize, _u4TsSourceBufAddr));

	 // Set 064 Pid
    if (!_DMX_Emu_130Ts_SetPid(u1TsIndex, TS_064_PID_IDX, TS_064_PID, DMX_PID_TYPE_PES,
                            _u4TsPid_064BufAddr, _u4TsPid_064BufSize, FALSE))
    {
        Printf("Cannot set 064 Pid!\n");
        goto FailAndFreeMemory;
    }
    _fgTsPid_064_Set = TRUE;

	// Set 078 Pid
    if (!_DMX_Emu_130Ts_SetPid(u1TsIndex, TS_078_PID_IDX, TS_078_PID, DMX_PID_TYPE_PES,
                            _u4TsPid_078BufAddr, _u4TsPid_078BufSize, FALSE))
    {
        Printf("Cannot set 078 Pid!\n");
        goto FailAndFreeMemory;
    }
    _fgTsPid_078_Set = TRUE;

	//set 130 byte play back enable
	 if (!_DMX_PVRPlay_130byteEnable(TRUE , 130))
    {
        Printf("Set 130 byte enable returns FALSE!\n");
        goto FailAndFreeMemory;
    }

	// Set 130 byte Framer enable
	if (!_DMX_Framer_130byteEnable(u1TsIndex , FALSE ,FALSE , 130 , TRUE , 130))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto FailAndFreeMemory;
	}


	//disable framer error handling ,framer 1
	_DMX_BypassErrorHandlingTable_Enable(u1TsIndex, TRUE);

	#if 1
	//disable framer befor reset framer
	u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);
	u4Reg &= 0xFFFFF000;
	DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);

    // Set framer, must after setpktsize .set external sync
    u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & 0xFFF0F000;
	u4Reg |= 0x30000;
	if(fgExtSync)
	{
		u4Reg |= 0xDDD;
	}
	else
	{
		u4Reg |= 0x999;
	}
	DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
	#endif

	//clear Dcache
	HalFlushInvalidateDCache();
	
	//Begin to transfer
    VERIFY(DMX_PVRPlay_Start());

    while(1)
    {
        VERIFY(DMX_PVRPlay_Get(PVRPLAY_FLAGS_BUFFER, &rPlay));
        if(rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }
        x_thread_delay(5);
    }

	x_thread_delay(100);
	
    VERIFY(DMX_PVRPlay_Stop(TRUE));



    // Check result
    // Compare the result with golden pattern.
    if(!fgExtSync)
    {
		if (!_MemCmp(((UINT32)(_u4TsPid_064GoldenAddr + 130)), (UINT32)_u4TsPid_064BufAddr, (_u4TsPid_064GoldenSize - 130), &u4Pid_064GoldenDiff, &u4Pid_064BufferDiff))
	    {
	       Printf("Differ at PID-064 Golden: 0x%08x, PID-064 Buffer: 0x%08x\n", u4Pid_064GoldenDiff, u4Pid_064BufferDiff);
	       goto FailAndFreeMemory;
	    }
		Printf("Pid 064 data compare with golden data ok\n");
		if (!_MemCmp(((UINT32)_u4TsPid_078GoldenAddr + 130), (UINT32)_u4TsPid_078BufAddr, (_u4TsPid_078GoldenSize - 130), &u4Pid_078GoldenDiff, &u4Pid_078BufferDiff))
	    {
	       Printf("Differ at PID-078 Golden: 0x%08x, PID-078 Buffer: 0x%08x\n", u4Pid_078GoldenDiff, u4Pid_078BufferDiff);
	       goto FailAndFreeMemory;
	    }
		Printf("Pid 078 data compare with golden data ok\n");
	}
	else
	{
		if (!_MemCmp(((UINT32)_u4TsPid_064GoldenAddr), (UINT32)_u4TsPid_064BufAddr, _u4TsPid_064GoldenSize, &u4Pid_064GoldenDiff, &u4Pid_064BufferDiff))
	    {
	       Printf("Differ at PID-064 Golden: 0x%08x, PID-064 Buffer: 0x%08x\n", u4Pid_064GoldenDiff, u4Pid_064BufferDiff);
	       goto FailAndFreeMemory;
	    }
		Printf("Pid 064 data compare with golden data ok\n");
		if (!_MemCmp(((UINT32)_u4TsPid_078GoldenAddr), (UINT32)_u4TsPid_078BufAddr, _u4TsPid_078GoldenSize, &u4Pid_078GoldenDiff, &u4Pid_078BufferDiff))
	    {
	       Printf("Differ at PID-078 Golden: 0x%08x, PID-078 Buffer: 0x%08x\n", u4Pid_078GoldenDiff, u4Pid_078BufferDiff);
	       goto FailAndFreeMemory;
	    }
		Printf("Pid 078 data compare with golden data ok\n");
	}
    Printf("Pass! (Source data size: %d)\n", _u4TsSourceBufSize);
	fgRet = TRUE;

FailAndFreeMemory:
    _DMX_Emu_130Ts_Cleanup();
    VERIFY(_DMX_PVRPlay_FreeBuf());

	//enable framer error handling ,framer 1
	_DMX_BypassErrorHandlingTable_Enable(u1TsIndex, FALSE);
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DmxEmuLoopbackThread
 */
//-----------------------------------------------------------------------------
static VOID _DmxEmuDDIThread_MultiSize(VOID* pvArg)
{
    UINT32 u4Rp, u4Wp;

    UNUSED(pvArg);

    while(1)
    {
        if(_fgDmxEmuDDIEnable_MultiSize)
        {
            do
            {
                u4Rp = DDI_READ32(DDI_REG_DMA_RP);
                u4Wp = DDI_READ32(DDI_REG_DMA_WP);
				if (!_DMX_IsAligned(u4Wp, DDI_POINTER_ALIGNMENT))
			    {
			        u4Wp -= (u4Wp % 4);
					u4Wp += 0x4;
			    }
            } while(u4Wp != u4Rp);

			Printf("DDI transfer done!\n");
			VERIFY(x_sema_unlock(_hDmxEmuDDISema_MultiSize) == OSR_OK);

			_DMX_Lock();
	  		_fgDmxEmuDDIEnable_MultiSize = FALSE;
			_DMX_Unlock();
        }

        x_thread_delay(10);
    }
}

//-----------------------------------------------------------------------------
/** _DmxEmuLoopbackThread
 */
//-----------------------------------------------------------------------------
static VOID _DmxEmuPVRThread_MultiSize(VOID* pvArg)
{
	DMX_PVR_PLAY_T rPlay;

    UNUSED(pvArg);

    while(1)
    {
        if(_fgDmxEmuPVREnable_MultiSize)
        {
            do
            {
                VERIFY(DMX_PVRPlay_Get(PVRPLAY_FLAGS_BUFFER, &rPlay));
            } while(rPlay.u4Rp != rPlay.u4Wp);

			Printf("PVR transfer done!\n");
            VERIFY(x_sema_unlock(_hDmxEmuPVRSema_MultiSize) == OSR_OK);

			_DMX_Lock();
	  		_fgDmxEmuPVREnable_MultiSize = FALSE;
			_DMX_Unlock();
        }
        x_thread_delay(10);
    }
}


BOOL _DMX_EMU_Multi_PacketSize_Init(void)
{
	static BOOL _fgInit = FALSE;

    if(!_fgInit)
    {
        if (x_thread_create(&_hDmxEmuDDIThread_MultiSize, "DMXEMUDDI_MULTISIZE", 1024, 60, _DmxEmuDDIThread_MultiSize, 0, NULL) != OSR_OK)
        {
            Printf("Fail to create DMX emu ddi thread!\n");
            return FALSE;
        }

		 if (x_thread_create(&_hDmxEmuPVRThread_MultiSize, "DMXEMUPVR_MULTISIZE", 1024, 60, _DmxEmuPVRThread_MultiSize, 0, NULL) != OSR_OK)
        {
            Printf("Fail to create DMX emu pvr thread!\n");
            return FALSE;
        }

		if(_hDmxEmuDDISema_MultiSize == NULL_HANDLE)
	    {
	        VERIFY(OSR_OK == x_sema_create(&_hDmxEmuDDISema_MultiSize,
	                         X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
	    }

		if(_hDmxEmuPVRSema_MultiSize == NULL_HANDLE)
	    {
	        VERIFY(OSR_OK == x_sema_create(&_hDmxEmuPVRSema_MultiSize,
	                         X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
	    }
        _fgInit = TRUE;
    }

	_DMX_Lock();
    _fgDmxEmuDDIEnable_MultiSize = FALSE;
	_fgDmxEmuPVREnable_MultiSize = FALSE;
	_DMX_Unlock();

	return TRUE;
}

BOOL _DMX_EMU_Multi_PacketSize_Test( BOOL fgFramer0Reset, BOOL fgFramer1Reset)
{

	BOOL fgRet = FALSE;
    UINT32 u4SrcBufSize, u4DstBufSize;
    UINT32 u4SrcAddr = 0, u4SrcDiff, u4SrcSize;
    UINT32 u4DstAddr = 0, u4DstDiff;
    UINT32 u4PesAddr = 0, u4PesSize ;
    //DMX_PVR_PLAY_T rPlay;
	UINT8 u1TsIndex = 0x1;
	UINT32 u4Size = 0x10000;
	UINT16 u2Pid = 0x100;
	DMX_PID_T rPid;
	UINT32 u4Pid_064GoldenDiff, u4Pid_064BufferDiff;
	UINT32 u4Pid_078GoldenDiff, u4Pid_078BufferDiff;

	//for FPGA emulation  ddi connet both framer 0 and framer 3 .
	// In this case, we should set framer 0 and framer 3 the same before set others.
	_DMX_SetFramerMode(0, DMX_FRAMER_PARALLEL, TRUE, TRUE);
	_DMX_SetFramerMode(3, DMX_FRAMER_PARALLEL, TRUE, TRUE);

	VERIFY(_DMX_EMU_Multi_PacketSize_Init());

	VERIFY(DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_1));
	VERIFY(DMX_TsIn_SetDataWidth(u1TsIndex, DMX_TSOUT_DATAWIDTH_1));
	VERIFY(DMX_TsOut_SetEndianess(DMX_TSOUT_ENDIAN_MSB));
	VERIFY(DMX_TsOut_SetSyncCycle(DMX_TSOUT_SYNC_CYCLE_8));
	VERIFY(DMX_TsOut_SetValidCycle(1)); //
	VERIFY(DMX_TsOut_SetLatchEdge(DMX_TSOUT_LATCH_EDGE_NEGATIVE));

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

    // Allocate source buffer.  (Mini-PVR RP/WP 8-byte alignment, 188 * 4 = 752)
    u4SrcSize = _DMX_Align(u4Size, 752);
    u4SrcBufSize = u4SrcSize ;
    u4SrcAddr = 0x3000000;

    // Allocate destination buffer
    u4DstBufSize = 0x20000;
    u4DstAddr = 0x3200000;

	x_memset((void*)u4DstAddr, 0, u4DstBufSize);
    //u4DstEndAddr = u4DstAddr + u4DstBufSize;

	//Allocate pes address
	u4PesAddr = (UINT32)x_mem_alloc(u4SrcSize);
    if (u4PesAddr == 0)
    {
        Printf("Failed to allocate payload buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

	//Set pvr play back
	VERIFY(_FVREmuSetPVRPlay(u4SrcAddr ,u4SrcBufSize));

	VERIFY(_FVREmuSetPlayPid(u1TsIndex,u2Pid , u4DstAddr , u4DstBufSize));

	VERIFY(_FVREmuSetTimestamp(FALSE));

	VERIFY(DMX_PVRPlay_SetBuffer(u4SrcAddr, u4SrcAddr + u4Size + 0x500 - 1,
							  u4SrcAddr + u4SrcBufSize, u4SrcAddr));

	//Generate Ts pattern
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

    Printf("Src address: 0x%08x(Size: %x!)\n", u4SrcAddr ,u4SrcBufSize );
    Printf("Dst address: 0x%08x(Size: %x!)\n", u4DstAddr ,u4DstBufSize);

	//set 130 byte play back disable, default packet size is 188
	 if (!_DMX_PVRPlay_130byteEnable(FALSE, 188))
    {
        Printf("Set 130 byte enable returns FALSE!\n");
        goto FailAndFreeMemory;
    }

	// Set 130 byte Framer disable, default packet size is 188
	if (!_DMX_Framer_130byteEnable(u1TsIndex , FALSE ,FALSE , 188 , FALSE , 188))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto FailAndFreeMemory;
	}

	// Init 130 byte ts from tsindex 0
	if (!DMX_Emu_130Ts_MultiPktSize(0))
	{
		Printf("Set 130 byte enable returns FALSE!\n");
		goto FailAndFreeMemory;
	}

	//set ddi to demux output delay byte for fpga emulation
	_DMX_DDI_SetDelayByte(0xf);

	//set framer 1 frontend
	_DMX_SetFrontEndEx(u1TsIndex, DMX_FE_TSOUT);

	//disable framer 3
	_DMX_SetFramer(3, FALSE, TRUE, TRUE, TRUE);

	//set framer 0 and framer 1 internal sync for multipacket reset framer test.
	_DMX_SetFramer(0, TRUE, TRUE, FALSE, TRUE);
	_DMX_SetFramer(1, TRUE, FALSE, FALSE, TRUE);

	//Begin to transfer
    VERIFY(DMX_PVRPlay_Start());

	// Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

	//reset framer test
	//org design:reset framer don't reset some logic in dbm. so we need reset dbm ,this will cause drop all
	//               the packet in the dbm.
	//new design:reset framer will also reset the logic in dbm.if enable the 17204[5]
	if (fgFramer0Reset)
	{
		_DMX_Emu_ResetFramer(0);
		Printf("Reset framer 0!\n");
	}
	if (fgFramer1Reset)
	{
		_DMX_Emu_ResetFramer(1);
		Printf("Reset framer 1!\n");
	}

	_DMX_Lock();
	_fgDmxEmuPVREnable_MultiSize = TRUE;
    _fgDmxEmuDDIEnable_MultiSize = TRUE;
	_DMX_Unlock();

	//wait for complete
	VERIFY(x_sema_lock(_hDmxEmuPVRSema_MultiSize, X_SEMA_OPTION_WAIT) == OSR_OK);
	VERIFY(x_sema_lock(_hDmxEmuDDISema_MultiSize, X_SEMA_OPTION_WAIT) == OSR_OK);


    VERIFY(DMX_PVRPlay_Stop(TRUE));


    // Check result PVR Play back.
    if (!fgFramer1Reset)
    {
	    if (!_MemCmp((u4SrcAddr + 0x234), u4DstAddr,(u4SrcSize - 0x234), &u4SrcDiff, &u4DstDiff)) //0x234= 188 * 3. internal sync will drop 3 packet
	    {
	        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
			goto FailAndFreeMemory;
	    }
    }

	// Compare the result with golden pattern.
	if (!fgFramer0Reset)
	{
		if (!_MemCmp(((UINT32)(_u4TsPid_064GoldenAddr + 130)), (UINT32)_u4TsPid_064BufAddr, (_u4TsPid_064GoldenSize - 130), &u4Pid_064GoldenDiff, &u4Pid_064BufferDiff))
	    {
	       Printf("Differ at PID-064 Golden: 0x%08x, PID-064 Buffer: 0x%08x\n", u4Pid_064GoldenDiff, u4Pid_064BufferDiff);
	       goto FailAndFreeMemory;
	    }
		Printf("Pid 064 data compare with golden data ok\n");

		if (!_MemCmp(((UINT32)(_u4TsPid_078GoldenAddr + 130)), (UINT32)_u4TsPid_078BufAddr, (_u4TsPid_078GoldenSize - 130), &u4Pid_078GoldenDiff, &u4Pid_078BufferDiff))
	    {
	       Printf("Differ at PID-078 Golden: 0x%08x, PID-078 Buffer: 0x%08x\n", u4Pid_078GoldenDiff, u4Pid_078BufferDiff);
	       goto FailAndFreeMemory;
	    }
	    Printf("Pid 078 data compare with golden data ok\n");
	}

    Printf("Pass! (Source data size: %d)\n", _u4TsSourceBufSize);

	fgRet = TRUE;

FailAndFreeMemory:
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf());
    if (u4PesAddr != 0)
    {
        x_mem_free((void*)u4PesAddr);
    }

	 _DMX_Emu_130Ts_Cleanup();

	//enable framer error handling ,framer 0  framer 3
	_DMX_BypassErrorHandlingTable_Enable(0, FALSE);
	_DMX_BypassErrorHandlingTable_Enable(3, FALSE);

    return fgRet;


}




