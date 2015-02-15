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
 * $RCSfile: dmx_emu_aes.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_mm.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
// The number of bytes in a TS packet header.
#define TS_HEADER_LEN                   (UINT8)4

// The number of bytes in the PES prefix header.
#define PES_HEADER_LEN                  (UINT8)6

// The index of the PES_packet_length from the PES header.
#define PES_PACKET_LEN0_IDX             (UINT8)4
#define PES_PACKET_LEN1_IDX             (UINT8)5

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
static void _GeneratePrebyteTsPacket(BOOL fgExtSync, UINT32 u4TsAddr, UINT32 u4TsSize, UINT16 u2Pid,
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
    u1SkipCount = 0;
    *pu2DstSize = 0;

    while (u4TsSize > 0)
    {
        pu1Data[u1PrebyteOffset + u4Offset] = 0x47;
        pu1Data[u1PrebyteOffset + u4Offset + 1] = (u2Pid & 0x1FFF) >> 8;
        pu1Data[u1PrebyteOffset + u4Offset + 2] = (u2Pid & 0xFF);
        pu1Data[u1PrebyteOffset + u4Offset + 3] = 0x10 | (u1CC & 0xF);
        u1CC++;

        if (!fgExtSync && (u1SkipCount < 3))
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
// Public functions
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_SetPktSize(UINT8 u1TsIndex, UINT16 u2PktSize, UINT16 u2ValidSize)
{
    UINT32 u4Reg;

    // DDI
    u4Reg = DDI_READ32(DDI_REG_PKT_QUADBYTE_LIMIT);
    u4Reg = (u4Reg & 0xFFFFFF00) | (u2PktSize >> 2);
    DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u4Reg);

    if (u1TsIndex == 0)
    {
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL) & 0xFFFF0000;
        u4Reg |= 0x0100;
        u4Reg |= (UINT32)(u2PktSize & 0xFF);
        DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);
    }
    else if (u1TsIndex == 1)
    {
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL) & 0xFFFF;
        u4Reg |= 0x01000000;
        u4Reg |= ((UINT32)(u2PktSize & 0xFF) << 16);
        DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL, u4Reg);
    }
    else if (u1TsIndex == 2)
    {
        // Framer2 size
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER2_CTRL2) & ~0xFF;
        u4Reg |= (UINT32)(u2PktSize & 0xFF);
        DMXCMD_WRITE32(DMX_REG_FRAMER2_CTRL2, u4Reg);

        // enable pre-byte scheme
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER2_CTRL);
        u4Reg |= 0x1 << 6;
        DMXCMD_WRITE32(DMX_REG_FRAMER2_CTRL, u4Reg);
    }
    else if (u1TsIndex == 3)
    {
        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_PREBYTE_CTRL2) & 0xFFFF0000;
        u4Reg |= 0x0100;
        u4Reg |= (UINT32)(u2PktSize & 0xFF);
        DMXCMD_WRITE32(DMX_REG_FRAMER_PREBYTE_CTRL2, u4Reg);
    }
    else
    {
        ASSERT(0);
    }

    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2) & 0xFFFF00FF;
    u4Reg |= (u2ValidSize << 8);
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    if (!_DMX_ResetDbmSafely())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL DMX_EMU_PreByte_Test(UINT8 u1TsIndex, BOOL fgExtSync, UINT32 u4Size, UINT16 u2PktSize)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Ctrl;
    UINT32 u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid;

    ASSERT(u1TsIndex <= 3);

    u2Pid = random(0x1FFB);
    u4Size -= (u4Size % u2PktSize);

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000;
    u4DstAddr = 0x3400000;
    u4PesAddr = 0x3800000;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    _GeneratePrebyteTsPacket(fgExtSync, u4SrcAddr, u4Size, u2Pid, u4PesAddr, &u4PesSize, u2PktSize, 188);
    Printf("TS size %u\n", u4Size);

    //Set DBM input source
    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
    if(u1TsIndex == 2)
    {
         goto _exit;
    }
    // Init DDI
    // This function must before set packet size -> it will reset DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // This function must before setpid - set packet size will reset pid valid bit
    VERIFY(_DMX_EMU_SetPktSize(u1TsIndex, u2PktSize, 188));

    //Set Framer internal and external sync
    if (fgExtSync)
    {
        _DMX_SetFramerMode(u1TsIndex, DMX_FRAMER_PARALLEL, TRUE, TRUE);
    }
    else
    {
        _DMX_SetFramerMode(u1TsIndex, DMX_FRAMER_PARALLEL, FALSE, TRUE);
    }

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 0x100, DMX_ES_FIFO_ALIGNMENT);

    // Setup PID
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

    u4Ctrl = PID_S_W(u1Pidx, 0);
    PID_S_W(u1Pidx, 0) = u4Ctrl | 0x4;       // Enable output mode
    PID_S_W(u1Pidx, 1) = u4PesSize;
    PID_S_W(u1Pidx, 3) = (0 << 24) | 1;     // skip size = 0, and trigger

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    x_thread_delay(100);

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

    return fgRet;
}

