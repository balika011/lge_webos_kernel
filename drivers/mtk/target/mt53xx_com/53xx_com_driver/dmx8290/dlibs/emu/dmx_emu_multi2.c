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
 * $RCSfile: dmx_emu_multi2.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_multi2.c
 *  Demux driver - Multi2 cipher emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_emu_if.h"
#include "dmx_emu_multi2_dec.h"
#include "dmx_emu_multi2_enc.h"
#include "dmx_mm.h"
#include "fvr.h"
#include "dmx_drm_drvif.h"
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

#ifdef CC_DMX_SUPPORT_MULTI2

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MULTI2_VIDEO_PID_IDX        0
#define MULTI2_AUDIO_PID_IDX        1
#define MULTI2_BYPASS_PID_IDX       2

#define MULTI2_VIDEO_PID            0x0111
#define MULTI2_AUDIO_PID            0x0112
#define MULTI2_BYPASS_PID           0x1003


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _u4SourceBufAddr;
static UINT32 _u4SourceBufSize;

static UINT32 _u4GoldenBufAddr;
static UINT32 _u4GoldenBufSize;

static UINT32 _u4VideoBufAddr;
static UINT32 _u4VideoBufSize;
static BOOL _fgVideoPidSet;

static UINT32 _u4AudioBufAddr;
static UINT32 _u4AudioBufSize;
static BOOL _fgAudioPidSet;

static UINT32 _u4BypassBufAddr;
static UINT32 _u4BypassBufSize;
static BOOL _fgBypassPidSet;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void _Dmx_Multi2_Init(void)
{
    _u4SourceBufAddr = 0;
    _u4SourceBufSize = 0;

    _u4GoldenBufAddr = 0;
    _u4GoldenBufSize = 0;

    _u4VideoBufAddr = 0;
    _u4VideoBufSize = 0;
    _fgVideoPidSet = FALSE;

    _u4AudioBufAddr = 0;
    _u4AudioBufSize = 0;
    _fgAudioPidSet = FALSE;

    _u4BypassBufAddr = 0;
    _u4BypassBufSize = 0;
    _fgBypassPidSet = FALSE;

    // Clear the memory occupied by even/odd keys.
    _DMX_DMEM_CA_Init();
}


static BOOL _Dmx_Multi2_Cleanup(void)
{
    DMX_PID_T rPid;

    if (_fgVideoPidSet)
    {
        // Disable PID, and then free PID.
        rPid.fgEnable = FALSE;
        VERIFY(_DMX_SetPid(MULTI2_VIDEO_PID_IDX, DMX_PID_FLAG_VALID, &rPid, FALSE));
        VERIFY(_DMX_FreePid(MULTI2_VIDEO_PID_IDX));
    }

    if (_fgAudioPidSet)
    {
        // Disable PID, and then free PID.
        rPid.fgEnable = FALSE;
        VERIFY(_DMX_SetPid(MULTI2_AUDIO_PID_IDX, DMX_PID_FLAG_VALID, &rPid, FALSE));
        VERIFY(_DMX_FreePid(MULTI2_AUDIO_PID_IDX));
    }

    if (_fgBypassPidSet)
    {
        // Disable PID, and then free PID.
        rPid.fgEnable = FALSE;
        VERIFY(_DMX_SetPid(MULTI2_BYPASS_PID_IDX, DMX_PID_FLAG_VALID, &rPid, FALSE));
        VERIFY(_DMX_FreePid(MULTI2_BYPASS_PID_IDX));
    }

    if (_u4SourceBufAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4SourceBufAddr));
    }

    if (_u4VideoBufAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4VideoBufAddr));
    }

    if (_u4AudioBufAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4AudioBufAddr));
    }

    if (_u4BypassBufAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4BypassBufAddr));
    }

    return TRUE;
}


static BOOL _Dmx_Multi2_SetPid(UINT8 u1TsIndex, UINT8 u1Pidx, UINT16 u2Pid,
                        UINT32 u4BufAddr, UINT32 u4BufSize, BOOL fgDescramble)
{
    DMX_PID_T rPid;
    UINT32 u4BufEnd, u4Flags;

    u4BufEnd = _DMX_Align(u4BufAddr + u4BufSize, DMX_ES_FIFO_ALIGNMENT);

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
    rPid.u4BufAddr = u4BufAddr;
    rPid.u4BufSize = u4BufEnd - u4BufAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEOCLIP;      // Set the Output mode bit
    rPid.eDescMode = (fgDescramble) ? DMX_DESC_MODE_MULTI2_BIG : DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        Printf("_DMX_SetPid() returns FALSE!\n");
    }

    x_memset((void*)u4BufAddr, 0, u4BufEnd - u4BufAddr);

    return TRUE;
}


static BOOL _Dmx_Multi2_SetBypassMode(UINT8 u1TsIndex, UINT8 u1BypassPidx)
{
    UINT32 u4Ctrl;
    UINT32 u4BypassReg, u4Value;

    if (!_DMX_SetBypassMode(u1TsIndex, 188, TRUE, TRUE))
    {
        Printf("Failed to set bypass mode!\n");
        return FALSE;
    }

    // FIXME: Put the following code snippet into _DMX_Reset().
    // Set PID structure size
    u4Ctrl = DMXCMD_READ32(DMX_REG_CONFIG1);
    u4Ctrl &= 0xFFFFFF00;
    u4Ctrl |= (DMX_DMEM_ENTRY_LEN * 4);         // Size of each PID data
    u4Ctrl |= (1 << 9);                         // DMA delay ack, debug
    u4Ctrl |= (1 << 10);                        // Enable multi-PID channel
    DMXCMD_WRITE32(DMX_REG_CONFIG1, u4Ctrl);

    switch (u1TsIndex)
    {
    case 0:
        u4BypassReg = DMX_REG_DBM_BYPASS_PID;
        break;
    case 1:
        u4BypassReg = DMX_REG_DBM_BYPASS_PID_2;
        break;
    case 2:
        u4BypassReg = DMX_REG_DBM_BYPASS_PID_3;
        break;
    case 3:
        u4BypassReg = DMX_REG_DBM_BYPASS_PID_4;
        break;
	case 4:
		u4BypassReg = DMX_REG_DBM_BYPASS_PID_5;
        break;
    default:
        Printf("TS index (%d) is out of range!\n", u1TsIndex);
        return FALSE;
    }

    // Because the golden data in non-A/V PID are not scrambled, there is no
    // need to set "Descrambling Scheme" in the "DBM BYPASS PID" register to 7.
    u4Value = DMXCMD_READ32(u4BypassReg);
    u4Value = (u4Value & 0xFF00FFFF) | (u1BypassPidx << 16);
    u4Value = (u4Value & (~(0xF << 10)));
    //u4Value = (u4Value & (~(0xF << 10))) | (0x7 << 10);
    DMXCMD_WRITE32(u4BypassReg, u4Value);

    // Reset some registers just in case they are modified somewhere.
    // Framer 0/1: Positive, External Sync, Serial, Enabled
    u4Value = (DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & 0xFFFFF000);
    if (u1TsIndex == 0)
    {
        u4Value |= 0x0F;
    }
    else if (u1TsIndex == 1)
    {
        u4Value |= 0xF0;
    }
    //else if((u1TsIndex == 3) && (_DMX_GetExternalFramer() != DMX_EXT_FRAMER_NORMAL))
    else if(u1TsIndex == 3)
    {
        u4Value |= 0xF00;
    }
    else
    {
        Printf("Both Framer 0 and Framer 1 are disabled!\n");
    }
    DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Value);

    // Disable both Framer's error handling mechanism
    //disable error handling
#if defined(CC_MT5890)
    _DMX_SetFramerPacketErrorHandling(u1TsIndex, FALSE, 0x0);
#else  //MT5882
    if(u1TsIndex != 2)
    {
        _DMX_SetFramerPacketErrorHandling(u1TsIndex, FALSE, 0x0);
    }
#endif

    // Bypass a certain TS index
    if(u1TsIndex !=4)
    {
     u4Value = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
     u4Value |= 1 << (u1TsIndex + 26);
	 DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Value);
    }
	else
	{
	  DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, DMXCMD_READ32(DMX_REG_DBM_CONTROL) | (1<<25));
	}


    return TRUE;
}


static BOOL _Dmx_Multi2_SetKeys(void)
{
    UINT32 au4Keys[2];
    UINT32 au4SystemKeys[8];
    UINT8 u1KeyIndex;
    UINT16 u2Iteration;
    BOOL fgEven, fgOFBFreeRun;

    // Set Even keys
    x_memset((void*)au4Keys, 0, sizeof(au4Keys));
    au4Keys[0] = 0xC388E7F3;
    au4Keys[1] = 0xBFAF666D;
    u1KeyIndex = 0;
    fgEven = TRUE;
    VERIFY(_DMX_SetMulti2Key(u1KeyIndex, fgEven, (UINT32*)au4Keys));

    // Set Odd keys
    x_memset((void*)au4Keys, 0, sizeof(au4Keys));
    au4Keys[0] = 0x8001BDBF;
    au4Keys[1] = 0x33D600D3;
    fgEven = FALSE;
    VERIFY(_DMX_SetMulti2Key(u1KeyIndex, fgEven, (UINT32*)au4Keys));

    // Set Multi2 Configuration Register
    fgOFBFreeRun = TRUE;
    u2Iteration = 0x20;
    _DMX_SetMulti2Config(u1KeyIndex, fgOFBFreeRun, u2Iteration);

    // Set CBC Inital Vector
    _DMX_SetMulti2CbcIV(u1KeyIndex, 0xFE271999, 0x19690911);

    // Set System keys
    x_memset((void*)au4SystemKeys, 0, sizeof(au4SystemKeys));
    au4SystemKeys[0] = 0x0D2E7D98;
    au4SystemKeys[1] = 0xB74E4084;
    au4SystemKeys[2] = 0x689D4BB7;
    au4SystemKeys[3] = 0xEC993921;
    au4SystemKeys[4] = 0xC4C36C1B;
    au4SystemKeys[5] = 0x32DF9CF5;
    au4SystemKeys[6] = 0x4B17EA5C;
    au4SystemKeys[7] = 0x36310466;
    _DMX_SetMulti2SysKey(0, au4SystemKeys);

    if(!_DMX_SetCA_Mode(0, DMX_CA_MODE_MULTI2_BIG, DMX_CA_FB_ECB, FALSE))
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL _DmxEmu_Multi2Test(UINT8 u1TsIndex)
{
    UINT8 u1Type;
    UINT32 u4SourceSize, u4EsDiff;
    UINT32 u4GoldenSize, u4GoldenOffset, u4GoldenDiff;
    UINT32 u4VideoBufOffset, u4AudioBufOffset, u4BypassBufOffset;
    UINT8 *pu1Golden, *pu1EsFifo;
    UINT32 u4Reg;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    _Dmx_Multi2_Init();

    _u4SourceBufSize = sizeof(au1Multi2Encrypted);
    _u4GoldenBufSize = sizeof(au1Multi2Decrypted);
    u4SourceSize = _u4SourceBufSize;

    if (u4SourceSize != _u4GoldenBufSize)
    {
        Printf("Source size and destination size differ!\n");
        return FALSE;
    }

    if (!_DMX_IsAligned(u4SourceSize, 188))
    {
        Printf("Source size is not aligned to 188!\n");
        return FALSE;
    }

    _u4GoldenBufAddr = (UINT32)au1Multi2Decrypted;
    if (_u4GoldenBufAddr == 0)
    {
        Printf("Golden address is 0.");
        return FALSE;
    }

    // Allocate buffers.  Make sure the ES FIFO is at least 768 bytes larger
    // than the size of the input data.
    _u4VideoBufSize  = u4SourceSize + 0x100000;
    _u4AudioBufSize  = u4SourceSize + 0x100000;
    _u4BypassBufSize = u4SourceSize + 0x100000;
    _u4SourceBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(_u4SourceBufSize, DDI_BUF_ALIGNMENT);
    _u4VideoBufAddr  = (UINT32)BSP_AllocAlignedDmaMemory(_u4VideoBufSize,  DMX_ES_FIFO_ALIGNMENT);
    _u4AudioBufAddr  = (UINT32)BSP_AllocAlignedDmaMemory(_u4AudioBufSize,  DMX_ES_FIFO_ALIGNMENT);
    _u4BypassBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(_u4BypassBufSize, DMX_ES_FIFO_ALIGNMENT);
    if ((_u4SourceBufAddr == 0) || (_u4VideoBufAddr == 0) ||
        (_u4BypassBufAddr == 0) || (_u4AudioBufAddr == 0))
    {
        Printf("Failed to allocate memory!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }

    Printf("Source address: 0x%08x (size: %d)\n", _u4SourceBufAddr, u4SourceSize);
    Printf("Golden address: 0x%08x\n", _u4GoldenBufAddr);
    Printf("Video  buffer address: 0x%08x\n", _u4VideoBufAddr);
    Printf("Audio  buffer address: 0x%08x\n", _u4AudioBufAddr);
    Printf("Bypass buffer address: 0x%08x\n", _u4BypassBufAddr);

    // Set the desired Bypass mode.
    _Dmx_Multi2_SetBypassMode(u1TsIndex, MULTI2_BYPASS_PID_IDX);

    // Set Video PID
    if (!_Dmx_Multi2_SetPid(u1TsIndex, MULTI2_VIDEO_PID_IDX, MULTI2_VIDEO_PID,
                            _u4VideoBufAddr, _u4VideoBufSize, TRUE))
    {
        Printf("Cannot set video PID!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }
    _fgVideoPidSet = TRUE;

    // Set Audio PID
    if (!_Dmx_Multi2_SetPid(u1TsIndex, MULTI2_AUDIO_PID_IDX, MULTI2_AUDIO_PID,
                            _u4AudioBufAddr, _u4AudioBufSize, TRUE))
    {
        Printf("Cannot set audio PID!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }
    _fgAudioPidSet = TRUE;

    // Set Bypass PID
    if (!_Dmx_Multi2_SetPid(u1TsIndex, MULTI2_BYPASS_PID_IDX, MULTI2_BYPASS_PID,
                            _u4BypassBufAddr, _u4BypassBufSize, FALSE))
    {
        Printf("Cannot set Bypass PID!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }
    _fgBypassPidSet = TRUE;

    if (!_DMX_EMU_SyncToggleBitInBypassCtrlWord(u1TsIndex, MULTI2_BYPASS_PID_IDX))
    {
        Printf("Cannot set the Toggle bit in the Bypass Control Word.\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }

    u4Reg = PID_S_W(MULTI2_VIDEO_PID_IDX, 0);
    PID_S_W(MULTI2_VIDEO_PID_IDX, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(MULTI2_VIDEO_PID_IDX, 1) = u4SourceSize;
    PID_S_W(MULTI2_VIDEO_PID_IDX, 3) = (0 << 24) | 1;     // skip size = 0, and trigger

    u4Reg = PID_S_W(MULTI2_AUDIO_PID_IDX, 0);
    PID_S_W(MULTI2_AUDIO_PID_IDX, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(MULTI2_AUDIO_PID_IDX, 1) = u4SourceSize;
    PID_S_W(MULTI2_AUDIO_PID_IDX, 3) = (0 << 24) | 1;     // skip size = 0, and trigger

    u4Reg = PID_S_W(MULTI2_BYPASS_PID_IDX, 0);
    PID_S_W(MULTI2_BYPASS_PID_IDX, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(MULTI2_BYPASS_PID_IDX, 1) = u4SourceSize;
    PID_S_W(MULTI2_BYPASS_PID_IDX, 3) = (0 << 24) | 1;     // skip size = 0, and trigger

    x_memcpy((void*)_u4SourceBufAddr, (const void*)au1Multi2Encrypted, u4SourceSize);

    HalFlushInvalidateDCache();

    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
#if defined(CC_MT5890)
    if (u1TsIndex == 2)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }

	if(u1TsIndex == 4)
	{
	    DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#else  //MT5882
    if(u1TsIndex == 2)
	{
	    DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#endif

    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
    
    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        Printf("_DMX_EMU_DDI_Init() returns FALSE!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }

    _Dmx_Multi2_SetKeys();

    // Invoke DDI transfer (Type 0)
    u1Type = 0;
    if (!_DMX_EMU_DDI_Transfer(u1Type, _u4SourceBufAddr, u4SourceSize))
    {
        Printf("_DMX_EMU_DDI_Transfer() returns FALSE!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }

    // Compare the result with golden pattern.
    u4VideoBufOffset = 0;
    u4AudioBufOffset = 0;
    u4BypassBufOffset = 0;
    u4GoldenOffset = 0;
    u4GoldenSize = _u4GoldenBufSize;   // _u4GoldenBufSize must be 188*N bytes.
    pu1Golden = (UINT8*)_u4GoldenBufAddr;
    while (u4GoldenOffset < u4GoldenSize)
    {
        UINT16 u2Pid;

        if (pu1Golden[u4GoldenOffset] != 0x47)
        {
            Printf("pu1Golden[u4GoldenOffset] != 0x47\n");
            _Dmx_Multi2_Cleanup();
            return FALSE;
        }

        u2Pid = (pu1Golden[u4GoldenOffset + 1] << 8) + pu1Golden[u4GoldenOffset + 2];
        u2Pid &= 0x1FFF;
        if (u2Pid == MULTI2_VIDEO_PID)
        {
            pu1EsFifo = (UINT8*)(_u4VideoBufAddr + u4VideoBufOffset);
            u4VideoBufOffset += 188;
        }
        else if (u2Pid == MULTI2_AUDIO_PID)
        {
            pu1EsFifo = (UINT8*)(_u4AudioBufAddr + u4AudioBufOffset);
            u4AudioBufOffset += 188;
        }
        else
        {
            pu1EsFifo = (UINT8*)(_u4BypassBufAddr + u4BypassBufOffset);
            u4BypassBufOffset += 188;
        }

        if (!_MemCmp(((UINT32)pu1Golden + u4GoldenOffset), (UINT32)pu1EsFifo, 188, &u4GoldenDiff, &u4EsDiff))
        {
            Printf("Differ at Golden: 0x%08x, ES-FIFO: 0x%08x\n", u4GoldenDiff, u4EsDiff);
            _Dmx_Multi2_Cleanup();
            return FALSE;
        }

        u4GoldenOffset += 188;
    }
/*
    if (!_DMX_EMU_DDI_Transfer(u1Type, _u4SourceBufAddr, u4SourceSize))
    {
        Printf("_DMX_EMU_DDI_Transfer() returns FALSE!\n");
        _Dmx_Multi2_Cleanup();
        return FALSE;
    }
*/
    Printf("Pass! (data size: %d)\n", u4SourceSize);
    _Dmx_Multi2_Cleanup();
    return TRUE;
}

static BOOL _DmxEmu_Multi2RecTest(UINT8 u1TsIndex)
{
    //UINT8 u1Type;
    UINT32 u4SourceSize, u4EsDiff;
    UINT32 u4GoldenDiff, u4DstAddr;
    FVR_GBL_T rGblPid;
    BOOL fgRet = TRUE;
    UINT32 u4RegValue;
    FVR_PID_T rFVRPid;

    if(u1TsIndex == 4)
    {
        Printf("Can't test for ts index 4 because there is no record global region in DMEM for ts index 4.\n");
        return FALSE;
    }
    
    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    _Dmx_Multi2_Init();

    _u4SourceBufSize = sizeof(au1Multi2Encrypted);
    _u4GoldenBufSize = sizeof(au1Multi2Decrypted);
    u4SourceSize = _u4SourceBufSize;
    u4DstAddr = 0x3400000;
    x_memset((void*)u4DstAddr, 0, u4SourceSize);

    if (u4SourceSize != _u4GoldenBufSize)
    {
        Printf("Source size and destination size differ!\n");
        return FALSE;
    }

    if (!_DMX_IsAligned(u4SourceSize, 188))
    {
        Printf("Source size is not aligned to 188!\n");
        return FALSE;
    }

    _u4GoldenBufAddr = (UINT32)au1Multi2Decrypted;
    if (_u4GoldenBufAddr == 0)
    {
        Printf("Golden address is 0.");
        return FALSE;
    }

    // Allocate buffers.  Make sure the ES FIFO is at least 768 bytes larger
    // than the size of the input data.
    _u4SourceBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(_u4SourceBufSize, DDI_BUF_ALIGNMENT);

    Printf("Source address: 0x%08x (size: %d)\n", _u4SourceBufAddr, u4SourceSize);
    Printf("Golden address: 0x%08x\n", _u4GoldenBufAddr);

    // Set FVR
    VERIFY(FVR_Init());

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u4BufStart = u4DstAddr;
    rGblPid.u4BufSize = 0x100000;
    rGblPid.u2TargetPktCount = 0xFFFF;
    if(!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        fgRet = FALSE;
        goto _Exit;
    }

    // Set Bypass PID
    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIndex;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = FALSE;
    rFVRPid.fgPicSearch = FALSE;
    rFVRPid.u1PicSearchMask = 0x0;
    rFVRPid.pfnPicSearchNotify = NULL;
    rFVRPid.u2Pid = MULTI2_BYPASS_PID;
    rFVRPid.fgDesc = TRUE;
    rFVRPid.u1DescKeyidx = 0;
    if(!FVR_SetPid(MULTI2_BYPASS_PID_IDX, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
    {
        fgRet = FALSE;
        goto _Exit;
    }

    // Bypass a certain TS index
    u4RegValue = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
    u4RegValue |= 1 << (u1TsIndex + 26);
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4RegValue);

    // Set bypass register
    // Record bypass pidx must add offset 128
    u4RegValue = ((128 + MULTI2_BYPASS_PID_IDX) << 16) | (1<<11);   // key index is 0
    u4RegValue |= ((PID_S_W(MULTI2_BYPASS_PID_IDX, 0) & 0x10) << 4);
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, u4RegValue);

    x_memcpy((void*)_u4SourceBufAddr, (const void*)au1Multi2Encrypted, u4SourceSize);

    HalFlushInvalidateDCache();

    VERIFY(_FVR_Start(u1TsIndex));

    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
#if defined(CC_MT5890)
    if (u1TsIndex == 2)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
#else  //MT5882
    if(u1TsIndex == 2)
	{
	    DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#endif

    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        Printf("_DMX_EMU_DDI_Init() returns FALSE!\n");
        fgRet = FALSE;
        goto _Exit;
    }

    _Dmx_Multi2_SetKeys();

    // Invoke DDI transfer (Type 0)
    //u1Type = 0;
    if(!_DDI_RECPULL_Transfer(u1TsIndex, _u4SourceBufAddr, u4SourceSize))
    {
        Printf("_DMX_EMU_DDI_Transfer() returns FALSE!\n");
        fgRet = FALSE;
        goto _Exit;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

    if (!_MemCmp((UINT32)u4DstAddr, _u4GoldenBufAddr, _u4SourceBufSize, &u4GoldenDiff, &u4EsDiff))
    {
        Printf("Differ at Golden: 0x%08x, ES-FIFO: 0x%08x\n", u4GoldenDiff, u4EsDiff);
        fgRet = FALSE;
        goto _Exit;
    }

    Printf("Pass! (data size: %d)\n", u4SourceSize);

_Exit:
    if (_u4SourceBufAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4SourceBufAddr));
        _u4SourceBufAddr = 0;
    }

    _FVR_FreePid(MULTI2_BYPASS_PID_IDX);

    VERIFY(_FVR_FreeGBL(u1TsIndex));

    u4RegValue = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4RegValue);

    // Set bypass register
    // Record bypass pidx must add offset 128
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0);

    if(!fgRet)
    {
        return FALSE;
    }

    return TRUE;
}


static BOOL _DmxEmu_Multi2RecLevel2Test(UINT8 u1TsIndex)
{
    UINT32 u4DstAddr = 0, u4GoldenAddr;
    BOOL fgRet = FALSE, fgCBC;
    FVR_GBL_T rGblPid;
    UINT32 i, j, u4KeyLen, au4IV[4], au4Key[8];
    AES_PARAM_T rParam2;
    UINT32 u4SourceSize;
    UINT32 u4GoldenDiff, u4EsDiff;
    UINT32 u4RegValue;

    if(u1TsIndex == 4)
    {
        Printf("Can't test for ts index 4 because there is no record global region in DMEM for ts index 4.\n");
        return FALSE;
    }
    
    // Allocate source and destination buffers
    u4DstAddr = 0x3400000;
    u4GoldenAddr = 0x3C00000;

    fgCBC = (BOOL)random(2);
    u4KeyLen = 128 + (64 * random(3));

    _Dmx_Multi2_Init();

    _u4SourceBufSize = sizeof(au1Multi2Encrypted);
    _u4GoldenBufSize = sizeof(au1Multi2Decrypted);
    u4SourceSize = _u4SourceBufSize;
    x_memset((void*)u4DstAddr, 0, _u4GoldenBufAddr);

    if (u4SourceSize != _u4GoldenBufSize)
    {
        Printf("Source size and destination size differ!\n");
        return FALSE;
    }

    if (!_DMX_IsAligned(u4SourceSize, 188))
    {
        Printf("Source size is not aligned to 188!\n");
        return FALSE;
    }

    _u4GoldenBufAddr = (UINT32)au1Multi2Decrypted;
    if (_u4GoldenBufAddr == 0)
    {
        Printf("Golden address is 0.");
        return FALSE;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)_u4GoldenBufAddr, _u4GoldenBufSize);

    // Allocate buffers.  Make sure the ES FIFO is at least 768 bytes larger
    // than the size of the input data.
    _u4SourceBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(_u4SourceBufSize, DDI_BUF_ALIGNMENT);

    Printf("Source address: 0x%08x (size: %d)\n", _u4SourceBufAddr, u4SourceSize);
    Printf("Golden address: 0x%08x\n", _u4GoldenBufAddr);

    // Encrypt for level2
    // Set record key
    for(i=0; i<8; i++)
    {
        au4Key[i] = (UINT32)rand();
    }
    for(i=0; i<4; i++)
    {
        au4IV[i] = (UINT32)rand();
    }

    rParam2.eFeedbackMode = fgCBC ? DMX_CA_FB_CBC : DMX_CA_FB_ECB;
    rParam2.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    rParam2.u2KeyLen = (UINT16)u4KeyLen;
    rParam2.eKeySelect = KEY_SELECT_EVEN;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            rParam2.au1InitVector[i*4 + j] = GET_BYTE(au4IV[i], 3 - j);
        }
    }
    for(i=0; i<8; i++)
    {
        for(j=0; j<4; j++)
        {
            rParam2.arKey[0].au1Even[i*4 + j] = GET_BYTE(au4Key[i], j);
            rParam2.arKey[0].au1Odd[i*4 + j] = GET_BYTE(au4Key[i], j);
        }
    }
    for(i=0; i<DMX_NUM_PID_INDEX; i++)
    {
        rParam2.arKeyMap[i].fgValid = FALSE;
    }
    rParam2.arKeyMap[0].fgValid = TRUE;
    rParam2.arKeyMap[0].u2Pid = 0;
    rParam2.arKeyMap[0].u1KeyIndex = 0;

    if (!_EncryptAesPackets(u4GoldenAddr, u4SourceSize, &rParam2, 0x0))
    {
        goto _exit;
    }

    // Set FVR
    VERIFY(FVR_Init());

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u4BufStart = u4DstAddr;
    rGblPid.u4BufSize = 0x100000;
    rGblPid.u2TargetPktCount = 0xFFFF;
    if(!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Bypass a certain TS index
    u4RegValue = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
    u4RegValue |= 1 << (u1TsIndex + 26);
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4RegValue);

    // Set bypass register
    // Record bypass pidx must add offset 128
    u4RegValue = ((128 + MULTI2_BYPASS_PID_IDX) << 16) | (1<<11);   // key index is 0
    u4RegValue |= ((fgCBC ? 1 : 0) | 2);
    switch(u4KeyLen)
    {
    case 192:
        u4RegValue |= (1<<2);
        break;
    case 256:
        u4RegValue |= (1<<3);
        break;
    }
    u4RegValue |= ((FVR_PER_PID_S_W(MULTI2_BYPASS_PID_IDX, 0) & 0x10) << 8);
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, u4RegValue);

    x_memcpy((void*)_u4SourceBufAddr, (const void*)au1Multi2Encrypted, u4SourceSize);
    HalFlushInvalidateDCache();

    if(!_FVR_SetAesKey(0, TRUE, (UINT8*)au4Key))
    {
        return FALSE;
    }
    if(!_FVR_SetAesKey(0, FALSE, (UINT8*)au4Key))
    {
        return FALSE;
    }
    if(fgCBC)
    {
        VERIFY(_FVR_SetAesEvenOddIV(0, TRUE, au4IV));
        VERIFY(_FVR_SetAesEvenOddIV(0, FALSE, au4IV));
    }

    VERIFY(_FVR_Start(u1TsIndex));

    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
#if defined(CC_MT5890)
    if (u1TsIndex == 2)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
#else  //MT5882
    if(u1TsIndex == 2)
	{
	    DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}
#endif

    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    _Dmx_Multi2_SetKeys();

    // Invoke DDI transfer
    if(!_DDI_RECPULL_Transfer(u1TsIndex, _u4SourceBufAddr, u4SourceSize))
    {
        // Error on transferring data
        goto _exit;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

    if (!_MemCmp((UINT32)u4DstAddr, u4GoldenAddr, _u4SourceBufSize, &u4GoldenDiff, &u4EsDiff))
    {
        Printf("Differ at Golden: 0x%08x, ES-FIFO: 0x%08x\n", u4GoldenDiff, u4EsDiff);
        fgRet = FALSE;
        goto _exit;
    }

    fgRet = TRUE;

_exit:
    if (_u4SourceBufAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4SourceBufAddr));
        _u4SourceBufAddr = 0;
    }

    _FVR_FreePid(MULTI2_BYPASS_PID_IDX);

    VERIFY(_FVR_FreeGBL(u1TsIndex));

    u4RegValue = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4RegValue);

    // Set bypass register
    // Record bypass pidx must add offset 128
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID + u1TsIndex, 0);

    if(!fgRet)
    {
        return FALSE;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
BOOL DMX_EMU_Multi2Test(UINT8 u1TsIndex, UINT8 u1Type)
{
    if(u1Type == 0)
    {
        return _DmxEmu_Multi2Test(u1TsIndex);
    }
    else if(u1Type == 1)
    {
        return _DmxEmu_Multi2RecTest(u1TsIndex);
    }
    else if(u1Type == 2)
    {
        return _DmxEmu_Multi2RecLevel2Test(u1TsIndex);
    }

    return FALSE;
}

#endif

