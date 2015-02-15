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
 * $RCSfile: dmx_emu_dvb.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_dvb.c
 *  Demux driver - DVB/DVB conformance emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_mm.h"
#include "dmx_emu_dvb_enc.h"
#include "dmx_emu_dvb_dec.h"
#include "fvr.h"
#include "dmx_drm_drvif.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"
#include "x_hal_arm.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define DMX_DVB_PID_NUM         4
#define DMX_DVB_PACKET_TIMES    (100)
#define DMX_DVB_PACKETS         (8 * DMX_DVB_PACKET_TIMES)

// key length in word
#define DMX_DVB_KEY_LEN         2

// TS0/TS1 use key index 0~7, and TS2/TS3 use key index 8~15.
//#define _DMX_MAX_DVB_KEY_INDEX  16

#define DMX_DVB_PID1            0x1FFF
#define DMX_DVB_PID2            0x1FA5
#define DMX_DVB_PID3            0x005A
#define DMX_DVB_PID4            0x00A5


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT8 au1EvenKey[8];
    UINT8 au1OddKey[8];
    UINT8 u1KeyIndex;
    DMX_CA_MODE_T eCAMode;
    UINT8 u1TsIndex;                // affect the "real" key index
    UINT8 u1Pidx;
    UINT16 u2Pid;
    UINT32 u4BufStart;
    UINT32 u4BufSize;
    UINT32 u4CmpOffset;
} DMX_DVB_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static DMX_DVB_T _rDvbData[4];

static UINT32 _u4DvbInitVectorLow;
static UINT32 _u4DvbInitVectorHigh;

static UINT32 _u4DvbSrcAddr;
static UINT32 _u4DvbSrcSize;

static UINT32 _u4DvbGoldenAddr;
static UINT32 _u4DvbGoldenSize;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

/*****************************************************************************
*
*****************************************************************************/
static void _Dmx_Emu_DvbClearKeyMemory(void)
{
    x_memset((void*)DMX_DMEM_CA_KEY_BASE, 0, 96 * DMX_DMEM_CA_KEY_NUM);
    x_memset((void*)DMX_DMEM_CA_CTRL_BASE, 0, 4 * DMX_DMEM_CA_KEY_NUM);
}


/*****************************************************************************
*
*****************************************************************************/
static BOOL _Dmx_Emu_DvbInit(UINT8 u1TsIndex)
{
    HAL_RAW_TIME_T rRawTime;

    HAL_GetRawTime(&rRawTime);
    srand((unsigned int)(rRawTime.u4Cycles));

    _Dmx_Emu_DvbClearKeyMemory();

    _rDvbData[0].au1EvenKey[0] = 0x12;
    _rDvbData[0].au1EvenKey[1] = 0x34;
    _rDvbData[0].au1EvenKey[2] = 0x56;
    _rDvbData[0].au1EvenKey[3] = 0x78;
    _rDvbData[0].au1EvenKey[4] = 0x90;
    _rDvbData[0].au1EvenKey[5] = 0xAB;
    _rDvbData[0].au1EvenKey[6] = 0xCD;
    _rDvbData[0].au1EvenKey[7] = 0xEF;
    _rDvbData[0].au1OddKey[0] = 0x12;
    _rDvbData[0].au1OddKey[1] = 0x34;
    _rDvbData[0].au1OddKey[2] = 0x56;
    _rDvbData[0].au1OddKey[3] = 0x78;
    _rDvbData[0].au1OddKey[4] = 0x90;
    _rDvbData[0].au1OddKey[5] = 0xAB;
    _rDvbData[0].au1OddKey[6] = 0xCD;
    _rDvbData[0].au1OddKey[7] = 0xEF;
    _rDvbData[0].u1KeyIndex= 0;
    _rDvbData[0].eCAMode = DMX_CA_MODE_DVB_CONF;
    _rDvbData[0].u1TsIndex = u1TsIndex;
    _rDvbData[0].u1Pidx = 0;//0 + random(8);
    _rDvbData[0].u2Pid = DMX_DVB_PID1;
    // Buffer for _rDvbData[0] is allocated somewhere else.
    Printf("PID index: %2d\n", _rDvbData[0].u1Pidx);

    _rDvbData[1].au1EvenKey[0] = 0x01;
    _rDvbData[1].au1EvenKey[1] = 0x23;
    _rDvbData[1].au1EvenKey[2] = 0x45;
    _rDvbData[1].au1EvenKey[3] = 0x67;
    _rDvbData[1].au1EvenKey[4] = 0x89;
    _rDvbData[1].au1EvenKey[5] = 0xAB;
    _rDvbData[1].au1EvenKey[6] = 0xCD;
    _rDvbData[1].au1EvenKey[7] = 0xEF;
    _rDvbData[1].au1OddKey[0] = 0x01;
    _rDvbData[1].au1OddKey[1] = 0x23;
    _rDvbData[1].au1OddKey[2] = 0x45;
    _rDvbData[1].au1OddKey[3] = 0x67;
    _rDvbData[1].au1OddKey[4] = 0x89;
    _rDvbData[1].au1OddKey[5] = 0xAB;
    _rDvbData[1].au1OddKey[6] = 0xCD;
    _rDvbData[1].au1OddKey[7] = 0xEF;
    _rDvbData[1].u1KeyIndex= 1;
    _rDvbData[1].eCAMode = DMX_CA_MODE_DVB_CONF;
    _rDvbData[1].u1TsIndex = u1TsIndex;
    _rDvbData[1].u1Pidx = 1; //8 + random(8);
    _rDvbData[1].u2Pid = DMX_DVB_PID2;
    // Buffer for _rDvbData[1] is allocated somewhere else.
    Printf("PID index: %2d\n", _rDvbData[1].u1Pidx);

    _rDvbData[2].au1EvenKey[0] = 0x64;
    _rDvbData[2].au1EvenKey[1] = 0x9D;
    _rDvbData[2].au1EvenKey[2] = 0xBD;
    _rDvbData[2].au1EvenKey[3] = 0x20;
    _rDvbData[2].au1EvenKey[4] = 0x0C;
    _rDvbData[2].au1EvenKey[5] = 0xF0;
    _rDvbData[2].au1EvenKey[6] = 0xCF;
    _rDvbData[2].au1EvenKey[7] = 0x91;
    _rDvbData[2].au1OddKey[0] = 0x64;
    _rDvbData[2].au1OddKey[1] = 0x9D;
    _rDvbData[2].au1OddKey[2] = 0xBD;
    _rDvbData[2].au1OddKey[3] = 0x20;
    _rDvbData[2].au1OddKey[4] = 0x0C;
    _rDvbData[2].au1OddKey[5] = 0xF0;
    _rDvbData[2].au1OddKey[6] = 0xCF;
    _rDvbData[2].au1OddKey[7] = 0x91;
    _rDvbData[2].u1KeyIndex= 2;
    _rDvbData[2].eCAMode = DMX_CA_MODE_DVB_CONF;
    _rDvbData[2].u1TsIndex = u1TsIndex;
    _rDvbData[2].u1Pidx = 2; //16 + random(8);
    _rDvbData[2].u2Pid = DMX_DVB_PID3;
    // Buffer for _rDvbData[2] is allocated somewhere else.
    Printf("PID index: %2d\n", _rDvbData[2].u1Pidx);

    _rDvbData[3].au1EvenKey[0] = 0x64;
    _rDvbData[3].au1EvenKey[1] = 0x9D;
    _rDvbData[3].au1EvenKey[2] = 0xBD;
    _rDvbData[3].au1EvenKey[3] = 0x20;
    _rDvbData[3].au1EvenKey[4] = 0x0C;
    _rDvbData[3].au1EvenKey[5] = 0xF0;
    _rDvbData[3].au1EvenKey[6] = 0xCF;
    _rDvbData[3].au1EvenKey[7] = 0x91;
    _rDvbData[3].au1OddKey[0] = 0x64;
    _rDvbData[3].au1OddKey[1] = 0x9D;
    _rDvbData[3].au1OddKey[2] = 0xBD;
    _rDvbData[3].au1OddKey[3] = 0x20;
    _rDvbData[3].au1OddKey[4] = 0x0C;
    _rDvbData[3].au1OddKey[5] = 0xF0;
    _rDvbData[3].au1OddKey[6] = 0xCF;
    _rDvbData[3].au1OddKey[7] = 0x91;
    _rDvbData[3].u1KeyIndex= 3;
    _rDvbData[3].eCAMode = DMX_CA_MODE_DVB;
    _rDvbData[3].u1TsIndex = u1TsIndex;
    _rDvbData[3].u1Pidx = 3; //24 + random(8);
    _rDvbData[3].u2Pid = DMX_DVB_PID4;
    // Buffer for _rDvbData[3] is allocated somewhere else.
    Printf("PID index: %2d\n", _rDvbData[3].u1Pidx);

    _u4DvbInitVectorLow  = 0;
    _u4DvbInitVectorHigh = 0;

    _u4DvbSrcAddr = 0;
    _u4DvbSrcSize = 0;

    _u4DvbGoldenAddr = 0;
    _u4DvbGoldenSize = 0;

    return TRUE;
}


/*****************************************************************************
*
*****************************************************************************/
static BOOL _Dmx_Emu_DvbReleaseBuffers(void)
{
    UINT8 i;

    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        if (_rDvbData[i].u4BufStart != 0)
        {
            VERIFY(BSP_FreeAlignedDmaMemory(_rDvbData[i].u4BufStart));
            _rDvbData[i].u4BufStart = 0;
            _rDvbData[i].u4BufSize  = 0;
            _rDvbData[i].u4CmpOffset = 0;
        }
    }

    if (_u4DvbSrcAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4DvbSrcAddr));
        _u4DvbSrcAddr = 0;
        _u4DvbSrcSize = 0;
    }

    if (_u4DvbGoldenAddr != 0)
    {
        VERIFY(BSP_FreeAlignedDmaMemory(_u4DvbGoldenAddr));
        _u4DvbGoldenAddr = 0;
        _u4DvbGoldenSize = 0;
    }

    return TRUE;
}


/*****************************************************************************
* Free PID first, and then release all allocated buffers.
*****************************************************************************/
static BOOL _Dmx_Emu_DvbCleanUp()
{
    UINT8 i;
    DMX_PID_T rPid;

    rPid.fgEnable = FALSE;
    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        VERIFY(_DMX_SetPid(_rDvbData[i].u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));
        VERIFY(_DMX_FreePid(_rDvbData[i].u1Pidx));
    }

    VERIFY(_Dmx_Emu_DvbReleaseBuffers());

    return TRUE;
}


/*****************************************************************************
*
*****************************************************************************/
static BOOL _Dmx_Emu_DvbAllocateBuffers(UINT32 u4BufSize)
{
    UINT8 i;
    UINT32 u4BufStart;

    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        u4BufStart = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize*2, DMX_ES_FIFO_ALIGNMENT);
        if (u4BufStart == 0)
        {
            return FALSE;
        }
        _rDvbData[i].u4BufStart = u4BufStart;
        _rDvbData[i].u4BufSize  = u4BufSize;
        _rDvbData[i].u4CmpOffset = 0;
    }

    u4BufStart = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    if (u4BufStart == 0)
    {
        return FALSE;
    }
    _u4DvbSrcAddr = u4BufStart;
    _u4DvbSrcSize = u4BufSize;
    Printf("Source addr: 0x%08X, size: %d\n", _u4DvbSrcAddr, _u4DvbSrcSize);

    // Allocate memory for Golden data.
    u4BufStart = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize, DMX_ES_FIFO_ALIGNMENT);
    if (u4BufStart == 0)
    {
        return FALSE;
    }
    _u4DvbGoldenAddr = u4BufStart;
    _u4DvbGoldenSize = u4BufSize;
    Printf("Golden addr: 0x%08X\n", _u4DvbGoldenAddr);

    return TRUE;
}


/*****************************************************************************
*
*****************************************************************************/
static BOOL _Dmx_Emu_DvbPrepareInputData(void)
{
    UINT32 u4EncDataSize, u4DecDataSize, i;

    u4EncDataSize = sizeof(au1DvbEncrypted);
    u4DecDataSize = sizeof(au1DvbDecrypted);

    if ((u4EncDataSize != u4DecDataSize) || (_u4DvbSrcSize < u4EncDataSize) ||
        (_u4DvbGoldenSize < u4EncDataSize))
    {
        return FALSE;
    }

    for(i=0; i<DMX_DVB_PACKET_TIMES; i++)
    {
        x_memcpy((void*)(_u4DvbSrcAddr + (i * u4EncDataSize)), au1DvbEncrypted, u4EncDataSize);
        x_memcpy((void*)(_u4DvbGoldenAddr + (i * u4EncDataSize)), au1DvbDecrypted, u4DecDataSize);
    }

    HalFlushInvalidateDCache();

    return TRUE;
}


/*****************************************************************************
*
*****************************************************************************/
#if 0
static BOOL _Dmx_Emu_DvbSetBypassMode(UINT8 u1TsIndex)
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
    default:
        Printf("TS index (%d) is out of range!\n", u1TsIndex);
        return FALSE;
    }

    // Because the golden data in non-A/V PID are not scrambled, there is no
    // need to set "Descrambling Scheme".
    u4Value = DMXCMD_READ32(u4BypassReg);
    u4Value = u4Value & 0xFF00FFFF;
    u4Value = (u4Value & (~(0xF << 10)));
    DMXCMD_WRITE32(u4BypassReg, u4Value);

    // Reset some registers just in case they are modified somewhere.
    // Framer 0/1: Positive, External Sync, Serial, Enabled
    u4Value = (DMXCMD_READ32(DMX_REG_FRAMER_CONTROL) & 0xFFFFFF00);
    if (u1TsIndex == 0)
    {
        u4Value |= 0x0F;
    }
    else if (u1TsIndex == 1)
    {
        u4Value |= 0xF0;
    }
    else
    {
        Printf("Both Framer 0 and Framer 1 are disabled!\n");
    }
    DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Value);

    // Disable both Framer's error handling mechanism
    DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0x0);

    // Bypass a certain TS index
    u4Value = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
    u4Value |= 1 << (u1TsIndex + 26);
    DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Value);

    return TRUE;
}
#endif

/*****************************************************************************
*
*****************************************************************************/
static BOOL _Dmx_Emu_DvbSetPids(UINT32 u4DataSize)
{
    UINT8 i;
    UINT32 u4Flags, u4Reg;
    DMX_PID_T rPid;
	UINT32 u4Ctrl;

    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        // Setup PID
        rPid.fgEnable = TRUE;
        rPid.fgAllocateBuffer = FALSE;
        rPid.fgPrimary = TRUE;
        rPid.u1TsIndex = _rDvbData[i].u1TsIndex;
        rPid.u1DeviceId = 0;
        rPid.u1KeyIndex = _rDvbData[i].u1KeyIndex;
        rPid.u1SteerMode = DMX_STEER_TO_FTUP;
        rPid.u2Pid = _rDvbData[i].u2Pid;
        rPid.u4BufAddr = _rDvbData[i].u4BufStart;
        rPid.u4BufSize = _rDvbData[i].u4BufSize;
        rPid.ePcrMode = DMX_PCR_MODE_NONE;
        rPid.ePidType = DMX_PID_TYPE_PES; //DMX_PID_TYPE_ES_VIDEOCLIP;  // video clip mode: uCode 3.8 or 3.18(later)
        rPid.pfnNotify = NULL;
        rPid.pvNotifyTag = NULL;
        rPid.pfnScramble = NULL;
        rPid.pvScrambleTag = NULL;
		rPid.eDescMode = DMX_DESC_MODE_NONE; //this is workaround for mt5398 fpga emulation

        u4Flags = DMX_PID_FLAG_ALL;
        if (!_DMX_SetPid(_rDvbData[i].u1Pidx, u4Flags, &rPid, FALSE))
        {
            return FALSE;
        }

        u4Reg = PID_S_W(_rDvbData[i].u1Pidx, 0);
        PID_S_W(_rDvbData[i].u1Pidx, 0) = u4Reg | 0x4;       // Enable output mode
        PID_S_W(_rDvbData[i].u1Pidx, 1) = u4DataSize;
        PID_S_W(_rDvbData[i].u1Pidx, 3) = (0 << 24) | 1;     // skip size = 0, and trigger
        //PID_S_W(_rDvbData[i].u1Pidx, 3) = 0xFFFF0000;

		//after set descmode none,dmx_setpid would't set descrambling scheme.
		//so we should set this after dmx_setpid
		 _DMX_Lock();
		u4Ctrl = PID_INDEX_TABLE(_rDvbData[i].u1Pidx);
		u4Ctrl |= 0x400; //set ca descramble enable
		PID_INDEX_TABLE(_rDvbData[i].u1Pidx) = u4Ctrl;
		_DMX_Unlock();

        x_memset((void*)_rDvbData[i].u4BufStart, 0, _rDvbData[i].u4BufSize);
    }

    return TRUE;
}


/*****************************************************************************
*
*****************************************************************************/
static void _Dmx_Emu_DvbSetInitialVector(void)
{
    UINT8 i;

    for(i=0; i<DMX_DVB_PID_NUM; i++)
    {
        DMX_DMEM_CA_EVEN_IV(i, 2) =  _u4DvbInitVectorLow;
        DMX_DMEM_CA_EVEN_IV(i, 3) =  _u4DvbInitVectorHigh;
        DMX_DMEM_CA_ODD_IV(i, 2) =  _u4DvbInitVectorLow;
        DMX_DMEM_CA_ODD_IV(i, 3) =  _u4DvbInitVectorHigh;
    }
}


/*****************************************************************************
*
*****************************************************************************/
static BOOL _Dmx_Emu_DvbSetKey(UINT8 u1KeyIndex, BOOL fgEven,
                               const UINT32 au4Key[DMX_DVB_KEY_LEN])
{
    UINT32 i;

    ASSERT(au4Key != NULL);

    _DMX_Lock();
    for (i = 0; i < DMX_DVB_KEY_LEN; i++)
    {
        if(fgEven)
        {
            DMX_DMEM_CA_EVEN_KEY(u1KeyIndex, 7 - i) = au4Key[i];
        }
        else
        {
            DMX_DMEM_CA_ODD_KEY(u1KeyIndex, 7 - i) = au4Key[i];
        }
    }
    _DMX_Unlock();

    return TRUE;
}


/******************************************************************************
* TS index 0 and 1 make use of key index 0 ~  7.
* TS index 2 and 3 make use of key index 8 ~ 15.
* This function takes TS index into account when setting keys.
******************************************************************************/
static void _Dmx_Emu_DvbSetKeys(void)
{
    UINT32 i, j;

    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        UINT32 au4EvenKey[DMX_DVB_KEY_LEN], au4OddKey[DMX_DVB_KEY_LEN];
        UINT8 u1KeyIndex;
        UINT32 u4KeyIndexOffset;

        ASSERT(_rDvbData[i].u1TsIndex <= MAX_TS_INDEX);

        u4KeyIndexOffset = ((_rDvbData[i].u1TsIndex & 0x2) ? 8 : 0);
        u1KeyIndex = _rDvbData[i].u1KeyIndex;
        u1KeyIndex += u4KeyIndexOffset;

        // Swap key ordering in unit of a word (4 bytes).
        for (j = 0; j < DMX_DVB_KEY_LEN; j++)
        {
            UINT8* p;
            p = _rDvbData[i].au1EvenKey + (j * 4);
            au4EvenKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

            p = _rDvbData[i].au1OddKey + (j * 4);
            au4OddKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        }

        // Set even/odd keys
        VERIFY(_Dmx_Emu_DvbSetKey(u1KeyIndex, TRUE, au4EvenKey));
        VERIFY(_Dmx_Emu_DvbSetKey(u1KeyIndex, FALSE, au4OddKey));
    }
}

static BOOL _DmxEmu_DvbMain(UINT8 u1TsIndex)
{
    UINT32 u4GoldenDiff, u4GoldenOffset, u4DstDiff;
    UINT32 u4BufSize = 188 * DMX_DVB_PACKETS;
    UINT8 i;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4BufSize, 188))
    {
        return FALSE;
    }

    _Dmx_Emu_DvbInit(u1TsIndex);

    //_Dmx_Emu_DvbSetBypassMode(u1TsIndex);

    if (!_Dmx_Emu_DvbAllocateBuffers(u4BufSize))
    {
        VERIFY(_Dmx_Emu_DvbReleaseBuffers());
        return FALSE;
    }

    _Dmx_Emu_DvbPrepareInputData();

    if (!_Dmx_Emu_DvbSetPids(u4BufSize))
    {
        VERIFY(_Dmx_Emu_DvbCleanUp());
        return FALSE;
    }

    _Dmx_Emu_DvbSetInitialVector();

    _Dmx_Emu_DvbSetKeys();

    for(i=0; i<DMX_DVB_PID_NUM; i++)
    {
        _DMX_SetCA_Mode(_rDvbData[i].u1KeyIndex, _rDvbData[i].eCAMode, DMX_CA_FB_ECB, FALSE);
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        _Dmx_Emu_DvbCleanUp();
        return FALSE;
    }

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, _u4DvbSrcAddr, _u4DvbSrcSize))
    {
        _Dmx_Emu_DvbCleanUp();
        return FALSE;
    }

    u4GoldenOffset = 0;
    i = 0;
    while (u4GoldenOffset < _u4DvbGoldenSize)
    {
        //Printf("Compare packet %d.\n", ((u4GoldenOffset / 188) + 1));
        if (!_MemCmp((_u4DvbGoldenAddr + u4GoldenOffset),
            (_rDvbData[i].u4BufStart + _rDvbData[i].u4CmpOffset), 188,
            &u4GoldenDiff, &u4DstDiff))
        {
            Printf("Differ at golden: 0x%08X, dst: 0x%08X\n", u4GoldenDiff, u4DstDiff);
            _Dmx_Emu_DvbCleanUp();
            return FALSE;
        }
        _rDvbData[i].u4CmpOffset += 188;
        u4GoldenOffset += 188;

        // Update the index to DVB data structure.
        i++;
        i = (i >= DMX_DVB_PID_NUM) ? 0 : i;
    }

    _Dmx_Emu_DvbCleanUp();
    return TRUE;
}


static BOOL _DmxEmu_DVBSetFVRPid(void)
{
    FVR_PID_T rFVRPid;
    UINT8 i;

    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        // Set Bypass PID
        x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
        rFVRPid.u1TsIndex = _rDvbData[i].u1TsIndex;
        rFVRPid.fgEnable = TRUE;
        rFVRPid.eKeyType= FVR_KEY_EVEN;
        rFVRPid.fgPktSwap = FALSE;
        rFVRPid.pfnSwapNotify = NULL;
        rFVRPid.fgScramble = FALSE;
        rFVRPid.fgPicSearch = FALSE;
        rFVRPid.u1PicSearchMask = 0x0;
        rFVRPid.pfnPicSearchNotify = NULL;
        rFVRPid.u2Pid = _rDvbData[i].u2Pid;
        rFVRPid.fgDesc = TRUE;
        rFVRPid.u1DescKeyidx = _rDvbData[i].u1KeyIndex;
        if(!FVR_SetPid(_rDvbData[i].u1Pidx, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
        {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL _DmxEmu_RecDvbMain(UINT8 u1TsIndex)
{
    UINT32 u4GoldenDiff, u4DstDiff;
    UINT32 u4BufSize = 188 * DMX_DVB_PACKETS;
    BOOL fgRet = TRUE;
    UINT8 i;
    FVR_GBL_T rGblPid;
    UINT32 u4DstAddr;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    u4DstAddr = 0x3C00000;
    x_memset((void*)u4DstAddr, 0, u4BufSize);

    if (!_DMX_IsAligned(u4BufSize, 188))
    {
        return FALSE;
    }

    _Dmx_Emu_DvbInit(u1TsIndex);

    if (!_Dmx_Emu_DvbAllocateBuffers(u4BufSize))
    {
        fgRet = FALSE;
        goto _exit;
    }

    _Dmx_Emu_DvbPrepareInputData();

    // Set FVR
    VERIFY(FVR_Init());

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u4BufStart = u4DstAddr;
    rGblPid.u4BufSize = 0x100000;
    rGblPid.u2TargetPktCount = 0xFFFF;
    if(!FVR_SetGBL(0, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        fgRet = FALSE;
        goto _exit;
    }

    if (!_DmxEmu_DVBSetFVRPid())
    {
        fgRet = FALSE;
        goto _exit;
    }

    _Dmx_Emu_DvbSetInitialVector();

    _Dmx_Emu_DvbSetKeys();

    for(i=0; i<DMX_DVB_PID_NUM; i++)
    {
        _DMX_SetCA_Mode(_rDvbData[i].u1KeyIndex, _rDvbData[i].eCAMode, DMX_CA_FB_ECB, FALSE);
    }

    VERIFY(FVR_Start(0));

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        fgRet = FALSE;
        goto _exit;
    }

    // Invoke DDI transfer
    if(!_DDI_RECPULL_Transfer(u1TsIndex, _u4DvbSrcAddr, _u4DvbSrcSize))
    {
        fgRet = FALSE;
        goto _exit;
    }

    VERIFY(FVR_Stop(0));

    if (!_MemCmp(_u4DvbGoldenAddr, u4DstAddr, _u4DvbSrcSize, &u4GoldenDiff, &u4DstDiff))
    {
        Printf("Differ at 0x%08x, 0x%08x\n", u4GoldenDiff, u4DstDiff);
        fgRet = FALSE;
        goto _exit;
    }

    Printf("Pass\n");

_exit:
    VERIFY(_FVR_FreePid(0));
    VERIFY(_FVR_FreePid(1));
    VERIFY(_FVR_FreePid(2));
    VERIFY(_FVR_FreePid(3));

    VERIFY(_Dmx_Emu_DvbReleaseBuffers());

    if(!fgRet)
    {
        return FALSE;
    }

    return TRUE;
}


static BOOL _DmxEmu_RecDvbRecLevel2Test(UINT8 u1TsIndex)
{
    UINT32 u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4GoldenAddr;
    BOOL fgRet = FALSE, fgCBC;
    FVR_PID_T rFVRPid;
    FVR_GBL_T rGblPid;
    UINT32 i, j, u4KeyLen, au4IV[4], au4Key[8];
    AES_PARAM_T rParam2;
    UINT32 u4BufSize = 188 * DMX_DVB_PACKETS;

    // Allocate source and destination buffers
    u4DstAddr = 0x3400000;
    x_memset((void*)u4DstAddr, 0, u4BufSize);
    u4GoldenAddr = 0x3C00000;
    fgCBC = (BOOL)random(2);
    u4KeyLen = 128 + (64 * random(3));

    if (!_DMX_IsAligned(u4BufSize, 188))
    {
        return FALSE;
    }

    _Dmx_Emu_DvbInit(u1TsIndex);

    if (!_Dmx_Emu_DvbAllocateBuffers(u4BufSize))
    {
        fgRet = FALSE;
        goto _exit;
    }

    _Dmx_Emu_DvbPrepareInputData();

    x_memcpy((void*)u4GoldenAddr, (void*)_u4DvbGoldenAddr, u4BufSize);

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

    if (!_EncryptAesPackets(u4GoldenAddr, u4BufSize, &rParam2, 0))
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
    if(!FVR_SetGBL(0, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        fgRet = FALSE;
        goto _exit;
    }

    for (i = 0; i < DMX_DVB_PID_NUM; i++)
    {
        // Set Bypass PID
        x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
        rFVRPid.u1TsIndex = _rDvbData[i].u1TsIndex;
        rFVRPid.fgEnable = TRUE;
        rFVRPid.eKeyType= FVR_KEY_EVEN;
        rFVRPid.fgPktSwap = FALSE;
        rFVRPid.pfnSwapNotify = NULL;
        rFVRPid.fgScramble = TRUE;
        rFVRPid.fgPicSearch = FALSE;
        rFVRPid.u1PicSearchMask = 0x0;
        rFVRPid.pfnPicSearchNotify = NULL;
        rFVRPid.u2Pid = _rDvbData[i].u2Pid;
        rFVRPid.fgDesc = TRUE;
        rFVRPid.u1DescKeyidx = _rDvbData[i].u1KeyIndex;
        rFVRPid.u1KeyIdx = 0;
        rFVRPid.fgCBCMode = fgCBC;
        rFVRPid.u4KeyLen = u4KeyLen;

        if(!FVR_SetPid(_rDvbData[i].u1Pidx, FVR_PID_FLAG_ALL, &rFVRPid))
        {
            fgRet = FALSE;
            goto _exit;
        }
    }

    _Dmx_Emu_DvbSetInitialVector();

    _Dmx_Emu_DvbSetKeys();

    for(i=0; i<DMX_DVB_PID_NUM; i++)
    {
        _DMX_SetCA_Mode(_rDvbData[i].u1KeyIndex, _rDvbData[i].eCAMode, DMX_CA_FB_ECB, FALSE);
    }

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

    VERIFY(FVR_Start(0));

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        fgRet = FALSE;
        goto _exit;
    }
#if 1
    // Invoke DDI transfer
    if(!_DDI_RECPULL_Transfer(u1TsIndex, _u4DvbSrcAddr, _u4DvbSrcSize))
    {
        // Error on transferring data
        fgRet = FALSE;
        goto _exit;
    }
#else
    // N/M Mbits/sec
    DDI_WRITE32(DDI_REG_PERIOD_M, 16 * 188 * 8 * 27);
    DDI_WRITE32(DDI_REG_PERIOD_N, 1);
    DDI_WRITE32(DDI_REG_RATE_CMD, 0x2);     // Update M and N

    if(!_DMX_EMU_DDI_Transfer(0, _u4DvbSrcAddr, _u4DvbSrcSize))
    {
        return FALSE;
    }
#endif
    VERIFY(FVR_Stop(0));

    // Check result
    if (!_MemCmp(u4GoldenAddr, u4DstAddr, u4BufSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Golden: 0x%08x, Dst: 0x%08x, size: %u\n", u4GoldenAddr, u4DstAddr,
            u4BufSize);
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    Printf("Pass\n");

    fgRet = TRUE;

_exit:
    VERIFY(_FVR_FreePid(0));
    VERIFY(_FVR_FreePid(1));
    VERIFY(_FVR_FreePid(2));
    VERIFY(_FVR_FreePid(3));

    VERIFY(_Dmx_Emu_DvbReleaseBuffers());

    if(!fgRet)
    {
        return FALSE;
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_DvbMain(UINT8 u1TsIndex, UINT8 u1Type)
{
    if(u1Type == 0)
    {
        return _DmxEmu_DvbMain(u1TsIndex);
    }
    else if(u1Type == 1)
    {
        return _DmxEmu_RecDvbMain(u1TsIndex);
    }
    else if(u1Type == 2)
    {
        return _DmxEmu_RecDvbRecLevel2Test(u1TsIndex);
    }

    return FALSE;
}

