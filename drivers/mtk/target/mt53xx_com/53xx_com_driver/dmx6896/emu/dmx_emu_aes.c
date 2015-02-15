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

/** @file dmx_emu_des.c
 *  Demux driver - DES/3DES emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "../dmx_mm.h"
#include "../fvr.h"
#include "../aes/aes.h"
#include "../aes/aes_packet.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MAX_KEY_NO                      DMX_DMEM_CA_KEY_NUM


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _u4Offset = 0x0;

//-----------------------------------------------------------------------------
// Extern variables
//-----------------------------------------------------------------------------
extern UINT8 au1peshdcp_128_enc;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static BOOL _SetAesKeys(UINT8 u1TsIndex, AES_PARAM_T* prParam)
{
    UINT32 i, j, u4Ctrl, u4Mode;
    UINT32 au4EvenKey[8], au4OddKey[8];
    UINT8 u1KeyIndex;
    UINT8* p;
    UINT32 u4KeyIndexOffset;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);
    ASSERT(prParam != NULL);

    //u4KeyIndexOffset = ((u1TsIndex & 0x2) ? 8 : 0);
    u4KeyIndexOffset = 0;

    u1KeyIndex = prParam->arKeyMap[0].u1KeyIndex;

    for (i = 0; i < MAX_KEY_NO; i++)
    {
        if (i == u1KeyIndex)
        {
            for (j = 0; j < 8; j++)
            {
                p = prParam->arKey[i].au1Even + (j * 4);
                au4EvenKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

                p = prParam->arKey[i].au1Odd + (j * 4);
                au4OddKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            }
        }
        else
        {
            // Reset keys
            for (j = 0; j < 8; j++)
            {
                au4EvenKey[j] = 0;
                au4OddKey[j] = 0;
            }
        }

        // Set keys
        VERIFY(_DMX_SetAesKey((u4KeyIndexOffset + i), TRUE, au4EvenKey));
        VERIFY(_DMX_SetAesKey((u4KeyIndexOffset + i), FALSE, au4OddKey));
    }

    VERIFY(_DMX_SetAesKeyLen(prParam->arKeyMap[0].u1KeyIndex, prParam->u2KeyLen));

    return TRUE;
}


BOOL _EncryptAesPackets(UINT32 u4Addr, UINT32 u4Size, AES_PARAM_T* prParam, UINT8 u1ForceKeyId)
{
    ASSERT(_DMX_IsAligned(u4Size, 188));

    while (u4Size > 0)
    {
        if (!_DMX_EncryptAesPacket((UINT8*)u4Addr, prParam, u1ForceKeyId))
        {
            return FALSE;
        }

        u4Addr += 188;
        u4Size -= 188;
    }

    return TRUE;
}


static BOOL _AesTest(UINT8 u1TsIndex, UINT32 u4Size, AES_PARAM_T* prParam)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Flags;
    UINT16 u2Pid;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000;
        //(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstAddr = 0x3400000;//(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DMX_ES_FIFO_ALIGNMENT);
    u4PesAddr = 0x3800000; //(UINT32)x_mem_alloc(u4Size);

    u4DstAddr += _u4Offset;
    _u4Offset = (_u4Offset + 0x40) % 0x1000;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    u2Pid = prParam->arKeyMap[u1Pidx].u2Pid;
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
        &u4PesSize))
    {
        goto _exit;
    }

    if (!_EncryptAesPackets(u4SrcAddr, u4Size, prParam, 0xFF))
    {
        goto _exit;
    }

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);
    x_memset((void*)u4DstAddr, 0, u4Size);

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = prParam->arKeyMap[u1Pidx].u1KeyIndex;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4DstEnd - u4DstAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
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

    // Invoke DDI transfer
    //if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
    if (!_DDI_PULLTransfer(u1Pidx, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    //HalInvalidateDCacheMultipleLine(u4DstAddr, u4PesSize);

    // Check result
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Src: 0x%08x, Dst: 0x%08x, size: %u\n", u4PesAddr, u4DstAddr,
            u4PesSize);
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
    //x_mem_free((void*)u4PesAddr);
    //VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    //VERIFY(BSP_FreeAlignedDmaMemory((void*)u4SrcAddr));

    return fgRet;

}

static BOOL _AesRecTest(UINT8 u1TsIndex, UINT32 u4Size, AES_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Flags;
    UINT32 u4GoldenAddr;
    UINT16 u2Pid;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    FVR_PID_T rFVRPid;
    FVR_GBL_T rGblPid;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000;
    u4DstAddr = 0x3400000;
    u4PesAddr = 0x3800000;
    u4GoldenAddr = 0x3C00000;

    u4DstAddr += _u4Offset;
    _u4Offset = (_u4Offset + 0x40) % 0x1000;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    u2Pid = prParam->arKeyMap[u1Pidx].u2Pid;
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
        &u4PesSize))
    {
        goto _exit;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)u4SrcAddr, u4Size);

    if (!_EncryptAesPackets(u4SrcAddr, u4Size, prParam, 0xFF))
    {
        goto _exit;
    }

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);
    x_memset((void*)u4DstAddr, 0, u4Size);

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
    rFVRPid.u2Pid = u2Pid;
    rFVRPid.fgDesc = TRUE;
    rFVRPid.u1DescKeyidx = prParam->arKeyMap[u1Pidx].u1KeyIndex;;

    if(!FVR_SetPid(u1Pidx, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
    {
        return FALSE;
    }

    VERIFY(_FVR_Start(u1TsIndex));

//    Printf("Pidx index table: 0x%x\n", FVR_PID_INDEX_TABLE(u1Pidx));

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if(!_DDI_RECPULL_Transfer(u1TsIndex, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

	//HalInvalidateDCacheMultipleLine(u4DstAddr, u4Size);

    // Check result
    if (!_MemCmp(u4GoldenAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Src: 0x%08x, Dst: 0x%08x, size: %u\n", u4PesAddr, u4DstAddr,
            u4PesSize);
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    VERIFY(FVR_FreePid(u1Pidx));
    // Free GBL
    if(!_FVR_FreeGBL(u1TsIndex))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

    return fgRet;
}


static BOOL _AesRecLevel2Test(UINT8 u1TsIndex, UINT32 u4Size, AES_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Flags;
    UINT32 u4GoldenAddr;
    UINT16 u2Pid;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE, fgCBC;
    FVR_PID_T rFVRPid;
    FVR_GBL_T rGblPid;
    UINT32 i, j, u4KeyLen, au4IV[4], au4Key[8];
    AES_PARAM_T rParam2;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000;
    u4DstAddr = 0x3400000;
    u4PesAddr = 0x3800000;
    u4GoldenAddr = 0x3C00000;
    fgCBC = (BOOL)random(2);
    u4KeyLen = 128 + (64 * random(3));

    u4DstAddr += _u4Offset;
    _u4Offset = (_u4Offset + 0x40) % 0x1000;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    u2Pid = prParam->arKeyMap[u1Pidx].u2Pid;
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
        &u4PesSize))
    {
        goto _exit;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)u4SrcAddr, u4Size);

    if (!_EncryptAesPackets(u4SrcAddr, u4Size, prParam, 0xFF))
    {
        goto _exit;
    }

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
    rParam2.arKeyMap[0].u2Pid = u2Pid;
    rParam2.arKeyMap[0].u1KeyIndex = 0;

    if (!_EncryptAesPackets(u4GoldenAddr, u4Size, &rParam2, 0xFF))
    {
        goto _exit;
    }

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);
    x_memset((void*)u4DstAddr, 0, u4Size);

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

    // PID
    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIndex;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = TRUE;
    rFVRPid.fgPicSearch = FALSE;
    rFVRPid.u1PicSearchMask = 0x0;
    rFVRPid.pfnPicSearchNotify = NULL;
    rFVRPid.u2Pid = u2Pid;
    rFVRPid.fgDesc = TRUE;
    rFVRPid.u1DescKeyidx = prParam->arKeyMap[u1Pidx].u1KeyIndex;    // Descramble key
    rFVRPid.u1KeyIdx = 0;                                           // Encrypt key
    rFVRPid.fgCBCMode = fgCBC;
    rFVRPid.u4KeyLen = u4KeyLen;

    if(!FVR_SetPid(u1Pidx, FVR_PID_FLAG_ALL, &rFVRPid))
    {
        return FALSE;
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

    VERIFY(_FVR_Start(u1TsIndex));

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if(!_DDI_RECPULL_Transfer(u1TsIndex, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

    //HalInvalidateDCacheMultipleLine(u4DstAddr, u4Size);

    // Check result
    if (!_MemCmp(u4GoldenAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Src: 0x%08x, Dst: 0x%08x, size: %u\n", u4PesAddr, u4DstAddr,
            u4PesSize);
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    VERIFY(FVR_FreePid(u1Pidx));
    // Free GBL
    if(!_FVR_FreeGBL(u1TsIndex))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

    return fgRet;
}


static BOOL _VuduAesTest(UINT8 u1TsIndex, UINT32 u4Size, AES_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4PatAddr, u4Reg;
    BOOL fgRet = FALSE;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = 0x2000000;
    u4DstAddr = 0x2100000;
    u4PatAddr = 0x2200000;
    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PatAddr == 0))
    {
        goto _exit;
    }

    // loading data to DRAM here
    fgRet = FALSE;

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, FALSE))
    {
        goto _exit;
    }

    // Set descramble mode - AES CBC - Use TsIndex 3
    DMXCMD_WRITE32(DMX_REG_DBM_BYPASS_PID_4, 0x2800);

    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstAddr + 0x100000, u4Size))
    {
        Printf("Failed to set CA output buffer!\n");
        goto _exit;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Set AES engine to be controlled by CA
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG1);
    DMXCMD_WRITE32(DMX_REG_CONFIG1, u4Reg & ~(1 << 11));

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    if (!_MemCmp(u4PatAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Src: 0x%08x, Dst: 0x%08x, size: %u\n", u4PatAddr, u4DstAddr,
            u4Size);
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    return fgRet;

}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_Aes0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type)
{
    AES_PARAM_T rParam;
    UINT32 i, j;
    UINT8 u1KeyIndex;
    UCHAR* szKey;
    static UINT8 _u1AesKey = 0;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Generate DES parameters
    // ECB or CBC (and init vector)
    rParam.eFeedbackMode = (DMX_CA_FEEDBACK_MODE_T)random(5);
    rParam.eRtb = (DMX_DESC_RTB_MODE_T)random(3);
    rParam.fgCTRResidual = (BOOL)random(2);

    if((rParam.eRtb == DMX_DESC_RTB_MODE_SCTE52) && (rParam.eFeedbackMode != DMX_CA_FB_CBC))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }

    if((rParam.eRtb == DMX_DESC_RTB_MODE_CTS) && (rParam.eFeedbackMode != DMX_CA_FB_ECB))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }

    if(rParam.eFeedbackMode == DMX_CA_FB_CTR)
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
        for(i=0; i<8; i++)
        {
            rParam.au1SampleId[i] = random(256);
        }
    }

    if (rParam.eFeedbackMode != DMX_CA_FB_ECB)
    {
        for (i = 0; i < 16; i++)
        {
            rParam.au1InitVector[i] = random(256);
        }
    }

    // Key length
    rParam.u2KeyLen = 128 + (random(3) * 64);

    // Key select
    rParam.eKeySelect = (KEY_SELECT_T)random(3);

    // Map to key 0 only, fixme
    for (i = 1; i < DMX_NUM_PID_INDEX; i++)
    {
        rParam.arKeyMap[i].fgValid = FALSE;
    }
    rParam.arKeyMap[0].fgValid = TRUE;
    rParam.arKeyMap[0].u2Pid = u2Pid;
    rParam.arKeyMap[0].u1KeyIndex = _u1AesKey++;//random(DMX_DMEM_CA_KEY_NUM);
    if(_u1AesKey >= DMX_DMEM_CA_KEY_NUM)
    {
        _u1AesKey = 0;
    }

    // Generate keys
    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        if (rParam.arKeyMap[i].fgValid)
        {
            u1KeyIndex = rParam.arKeyMap[i].u1KeyIndex;
            for (j = 0; j < rParam.u2KeyLen / 8; j++)
            {
                rParam.arKey[u1KeyIndex].au1Even[j] = random(256);
                rParam.arKey[u1KeyIndex].au1Odd[j] = random(256);
            }
        }
    }

    // Set keys
    if (!_SetAesKeys(u1TsIndex, &rParam))
    {
        return FALSE;
    }

    u1KeyIndex = rParam.arKeyMap[0].u1KeyIndex;

    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        // Set initial vector
        UINT32 au4Iv[4];

        for (i = 0; i < 4; i++)
        {
            au4Iv[i] = 0;

            for (j = 0; j < 4; j++)
            {
                au4Iv[i] |= (rParam.au1InitVector[(i * 4) + j] << ((3 - j) * 8));
            }
        }
        if(rParam.eKeySelect == KEY_SELECT_RANDOM)
        {
            _DMX_SetAesIVEx2(u1KeyIndex, au4Iv, TRUE);
            _DMX_SetAesIVEx2(u1KeyIndex, au4Iv, FALSE);
        }
        else if(rParam.eKeySelect == KEY_SELECT_EVEN)
        {
            _DMX_SetAesIVEx2(u1KeyIndex, au4Iv, TRUE);
        }
        else if(rParam.eKeySelect == KEY_SELECT_ODD)
        {
            _DMX_SetAesIVEx2(u1KeyIndex, au4Iv, FALSE);
        }
    }

    if(!_DMX_SetCA_Mode(u1KeyIndex, DMX_CA_MODE_AES, rParam.eFeedbackMode, FALSE))
    {
        return FALSE;
    }

    _DMX_SetAesRtbMode(u1KeyIndex, rParam.eRtb);

    if(rParam.eFeedbackMode == DMX_CA_FB_CTR)
    {
        UINT32 u4SampleId1, u4SampleId2;
        UINT64 u8Nonce;

        u4SampleId1 = (rParam.au1SampleId[0] << 24) | (rParam.au1SampleId[1] << 16) |
                      (rParam.au1SampleId[2] << 8) | rParam.au1SampleId[3];

        u4SampleId2 = (rParam.au1SampleId[4] << 24) | (rParam.au1SampleId[5] << 16) |
                      (rParam.au1SampleId[6] << 8) | rParam.au1SampleId[7];

        u8Nonce = ((UINT64)u4SampleId1 << 32) | (UINT64)u4SampleId2;

        _DMX_SetCA_CTR(u1KeyIndex, u8Nonce, 0, TRUE);
        _DMX_SetCA_CTR(u1KeyIndex, u8Nonce, 0, FALSE);

        _DMX_SetCTR_Rtb(u1KeyIndex, rParam.fgCTRResidual);
    }

    // Show AES parameters
    switch (rParam.eKeySelect)
    {
    case KEY_SELECT_EVEN:
        szKey = "EVEN";
        break;

    case KEY_SELECT_ODD:
        szKey = "ODD";
        break;

    case KEY_SELECT_RANDOM:
        szKey = "RANDOM";
        break;

    default:
        ASSERT(0);
        break;
    }

    u1KeyIndex = rParam.arKeyMap[0].u1KeyIndex;

    // Show mode and key select
    Printf(" Key size: %u, key select: %s, key index: %u\n",
        rParam.u2KeyLen, szKey, u1KeyIndex);
    switch(rParam.eFeedbackMode)
    {
    case DMX_CA_FB_ECB:
        Printf("     mode: ECB\n");
        break;
    case DMX_CA_FB_CBC:
        Printf("     mode: CBC\n");
        break;
    case DMX_CA_FB_CFB:
        Printf("     mode: CFB\n");
        break;
    case DMX_CA_FB_OFB:
        Printf("     mode: OFB\n");
        break;
    case DMX_CA_FB_CTR:
        Printf("     mode: CTR, do residual:%d\n", (INT32)rParam.fgCTRResidual);
        break;
    }

    switch (rParam.eRtb)
    {
    case DMX_DESC_RTB_MODE_CLEAR:
        Printf("     rtb: CLEAR\n");
        break;
    case DMX_DESC_RTB_MODE_CTS:
        Printf("     rtb: CTS\n");
        break;
    case DMX_DESC_RTB_MODE_SCTE52:
        Printf("     rtb: SCTE52\n");
        break;
    }

    // Show keys
    if (rParam.eKeySelect != KEY_SELECT_ODD)
    {
        Printf(" Even key: ");
        for (i = 0; i < rParam.u2KeyLen / 8; i++)
        {
            Printf("%02x", rParam.arKey[u1KeyIndex].au1Even[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }
    if (rParam.eKeySelect != KEY_SELECT_EVEN)
    {
        Printf(" Odd key:  ");
        for (i = 0; i < rParam.u2KeyLen / 8; i++)
        {
            Printf("%02x", rParam.arKey[u1KeyIndex].au1Odd[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }

    // Show initial vector
    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        Printf(" Init vector: ");
        for (i = 0; i < 16; i++)
        {
            Printf("%02x", rParam.au1InitVector[i]);
        }
        Printf("\n");
    }

    if(u1Type == 0)
    {
        return _AesTest(u1TsIndex, u4Size, &rParam);
    }
    else if(u1Type == 1)
    {
        return _AesRecTest(u1TsIndex, u4Size, &rParam);
    }
    else if(u1Type == 2)
    {
        return _AesRecLevel2Test(u1TsIndex, u4Size, &rParam);
    }

    return FALSE;
}

BOOL DMX_EMU_VuduAes0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, BOOL fgSCTE)
{
    AES_PARAM_T rParam;
    UINT32 i, j;
    UINT8 u1KeyIndex;
    UCHAR* szKey;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Setup AES parameters
    // ECB or CBC (and init vector)
    rParam.eFeedbackMode = DMX_CA_FB_CBC;

    // Key length
    rParam.u2KeyLen = 128;

    // Key select
    rParam.eKeySelect = KEY_SELECT_RANDOM; //(KEY_SELECT_T)random(3);

    // Map to key 0 only, fixme
    for (i = 1; i < DMX_NUM_PID_INDEX; i++)
    {
        rParam.arKeyMap[i].fgValid = FALSE;
    }
    rParam.arKeyMap[0].fgValid = TRUE;
    rParam.arKeyMap[0].u2Pid = u2Pid;
    rParam.arKeyMap[0].u1KeyIndex = 0; //random(DMX_DMEM_CA_KEY_NUM);

    // Generate keys
    for (i = 0; i < DMX_NUM_PID_INDEX; i++)
    {
        if (rParam.arKeyMap[i].fgValid)
        {
            u1KeyIndex = rParam.arKeyMap[i].u1KeyIndex;
            for (j = 0; j < rParam.u2KeyLen / 8; j++)
            {
                rParam.arKey[u1KeyIndex].au1Even[j] = 0x55;
                rParam.arKey[u1KeyIndex].au1Odd[j] = 0xaa;
            }
        }
    }

//    rParam.fgSCTE = fgSCTE;

    // Set keys
    if (!_SetAesKeys(u1TsIndex, &rParam))
    {
        return FALSE;
    }

    if (rParam.eFeedbackMode != DMX_CA_FB_ECB)
    {
        // Set initial vector
        UINT32 au4Iv[4];

        au4Iv[0] = 0x12345678;
        au4Iv[1] = 0xdeadbeef;
        au4Iv[2] = 0xbabeabba;
        au4Iv[3] = 0xaa55aa55;

        if(rParam.eKeySelect == KEY_SELECT_RANDOM)
        {
            _DMX_SetAesIVEx2(0, au4Iv, TRUE);
            _DMX_SetAesIVEx2(0, au4Iv, FALSE);
        }
        else if(rParam.eKeySelect == KEY_SELECT_EVEN)
        {
            _DMX_SetAesIVEx2(0, au4Iv, TRUE);
        }
        else if(rParam.eKeySelect == KEY_SELECT_ODD)
        {
            _DMX_SetAesIVEx2(0, au4Iv, FALSE);
        }
    }

    // Show DES parameters
    switch (rParam.eKeySelect)
    {
    case KEY_SELECT_EVEN:
        szKey = "EVEN";
        break;

    case KEY_SELECT_ODD:
        szKey = "ODD";
        break;

    case KEY_SELECT_RANDOM:
        szKey = "RANDOM";
        break;

    default:
        ASSERT(0);
        break;
    }

    u1KeyIndex = rParam.arKeyMap[0].u1KeyIndex;

    // Show mode and key select
    Printf(" Key size: %u, key select: %s, key index: %u\n",
        rParam.u2KeyLen, szKey, u1KeyIndex);

    // Show keys
    if (rParam.eKeySelect != KEY_SELECT_ODD)
    {
        Printf(" Even key: ");
        for (i = 0; i < rParam.u2KeyLen / 8; i++)
        {
            Printf("%02x", rParam.arKey[u1KeyIndex].au1Even[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }
    if (rParam.eKeySelect != KEY_SELECT_EVEN)
    {
        Printf(" Odd key:  ");
        for (i = 0; i < rParam.u2KeyLen / 8; i++)
        {
            Printf("%02x", rParam.arKey[u1KeyIndex].au1Odd[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }

    // Show initial vector
    if (rParam.eFeedbackMode != DMX_CA_FB_ECB)
    {
        Printf(" Init vector: ");
        for (i = 0; i < 16; i++)
        {
            Printf("%02x", rParam.au1InitVector[i]);
        }
        Printf("\n");
    }

    return _VuduAesTest(u1TsIndex, u4Size, &rParam);
}


BOOL DMX_EMU_HDCP()
{
    DMX_PS_T rPs;
    DMX_MM_DATA_T rMm;
    UINT8 u1Pidx;
    UINT32 u4Buf, i;
    UINT8 au1Key[] = {0x60, 0x11, 0x47, 0x8f, 0xf7, 0x37, 0xe6, 0xc8,
                    0xa4, 0xf2, 0x03, 0xaf, 0xf3, 0x4f , 0xdd, 0x16};

    UINT8 au1Iv[] = {0x9a, 0x6d, 0x11, 0x00, 0xa9, 0xb7, 0x6f, 0x64,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    DMX_MUL_EnableMultipleInst(TRUE);
    DMX_MUL_SetInstType(0, DMX_IN_PLAYBACK_PS);
    DMX_PS_SetDataFormat(0, DMX_DATA_FORMAT_RAW);
    _DMX_PS_CPU_SetHDCPKeyIv(0, au1Key, au1Iv);
    x_memset(&rPs, 0, sizeof(rPs));
    rPs.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rPs.u1DeviceId = 0;
    rPs.fgAllocateBuffer = TRUE;
    rPs.u4BufSize = 0x100000;
    rPs.u1StreamID = 0xe0;
    rPs.fgEnable = TRUE;
    u1Pidx = DMX_MUL_GetAvailablePidx(0);
    DMX_PS_SetStream(u1Pidx, DMX_PID_FLAG_ALL, &rPs);

    u4Buf = BSP_AllocAlignedDmaMemory(sizeof(au1peshdcp_128_enc), 32);
    x_memcpy(u4Buf, au1peshdcp_128_enc, sizeof(au1peshdcp_128_enc));

    rMm.u4BufStart = u4Buf;
    rMm.u4BufEnd = u4Buf + DMX_Align(sizeof(au1peshdcp_128_enc), 32) + 0x100;
    rMm.fgFrameHead = TRUE;
    rMm.fgEOS = FALSE;

    for(i = 0; i + 32 < sizeof(au1peshdcp_128_enc); i += 32)
    {
        rMm.u4StartAddr = u4Buf + i;
        rMm.u4FrameSize = 32;
        DMX_MUL_MoveData(0, &rMm);
    }

    // final data
    rMm.u4StartAddr = u4Buf + i;
    rMm.u4FrameSize = sizeof(au1peshdcp_128_enc) - i;
    DMX_MUL_MoveData(0, &rMm);

    return TRUE;
}

