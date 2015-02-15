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
#include "dmx_mm.h"
#include "fvr.h"
#include "dmx_drm_drvif.h"
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


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
void _SMS4_ByteOrder(UINT32 *pu4Buf, UINT32 u4ByteLen)
{
    UINT32 i;
    UINT32 u4Tmp;
    //UINT8 *pu1Tmp;

    for (i=0; i<(u4ByteLen/4); i++)
    {
        u4Tmp = pu4Buf[i];
        pu4Buf[i] = ((GET_BYTE(u4Tmp, 0) & 0xFF) << 24)  | ((GET_BYTE(u4Tmp, 1) & 0xFF) << 16) |
                    ((GET_BYTE(u4Tmp, 2) & 0xFF) << 8) | (GET_BYTE(u4Tmp, 3) & 0xFF);
    }
}

static BOOL _EncryptSMS4Packet(UINT8* pu1Buf, SMS4_PARAM_T* prParam)
{
    UINT8* pu1Skip4 = pu1Buf + 4;
    UINT32 u4Offset;
    //UINT16 u2Pid = 0;
    UINT8 au1Text[16], au1InitVector[16], au1Backup[16];
    UINT8 u1AdaptField = 0, u1PayloadOffset = 0;
    //UINT8 u1KeyId;
    BOOL fgEven = TRUE;
    //int i, j;

    UINT8* au1Key = au1Text;

    ASSERT(pu1Buf != NULL);
    ASSERT(prParam != NULL);

    //u2Pid = prParam->u2Pid;
    //u1KeyId = prParam->u1KeyIdx;

    u1AdaptField = (pu1Buf[3] >> 4) & 0x3;
    if (0 == (u1AdaptField & 0x1))  // 00 and 10 => skip
    {
        goto _exit;
    }
    else if (u1AdaptField == 0x3)   // adaptation + payload
    {
        u1PayloadOffset = 1 + pu1Buf[4];
    }
    else                            // 0x1 payload only
    {
        u1PayloadOffset = 0;
    }

    switch (prParam->eKeySelect)
    {
    case KEY_SELECT_EVEN:
        fgEven = TRUE;
        break;

    case KEY_SELECT_ODD:
        fgEven = FALSE;
        break;

    case KEY_SELECT_RANDOM:
        fgEven = ((rand() & 1) == 0) ? TRUE : FALSE;
        break;

    default:
        ASSERT(0);
        break;
    }

    //u4KeyLen = 16;

    x_memcpy(au1InitVector, prParam->au1InitVector, 16);
    _SMS4_ByteOrder((UINT32*)au1InitVector, 16);

    if (fgEven) // Even
    {
        x_memcpy(au1Key, prParam->au1EvenKey, 16);
        pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
    }
    else        // Odd
    {
        x_memcpy(au1Key, prParam->au1OddKey, 16);
        pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
    }
    _SMS4_ByteOrder((UINT32*)au1Key, 16);

    // Setup key scheduling
    SMS4SetKey((UINT32*)au1Key, 1);

    for (u4Offset = u1PayloadOffset; u4Offset <= (TS_PACKET_SIZE - 20);
            u4Offset += 16)
    {
        x_memcpy(au1Backup, pu1Skip4 + u4Offset, 16);   // Get plain text
        x_memcpy(au1Text, pu1Skip4 + u4Offset, 16);   // Get plain text
        _SMS4_ByteOrder((UINT32*)au1Text, 16);
        // Encrypt
        if (prParam->eFeedbackMode == DMX_CA_FB_ECB)
        {
            SMS4Encrypt_ecb((UINT32*)au1Text);
        }
        else if (prParam->eFeedbackMode == DMX_CA_FB_CBC)
        {
            SMS4Encrypt_cbc((UINT32*)au1Text, (UINT32*)au1InitVector);
        }
        else if (prParam->eFeedbackMode == DMX_CA_FB_OFB)
        {
            SMS4Encrypt_ofb((UINT32*)au1Text, (UINT32*)au1InitVector);
        }
        else if (prParam->eFeedbackMode == DMX_CA_FB_CFB)
        {
            SMS4Encrypt_cfb((UINT32*)au1Text, (UINT32*)au1InitVector);
        }
        _SMS4_ByteOrder((UINT32*)au1Text, 16);
        x_memcpy(pu1Skip4 + u4Offset, au1Text, 16);  // Copy back
    }

    if ((prParam->eRtb == DMX_DESC_RTB_MODE_CTS) && (prParam->eFeedbackMode == DMX_CA_FB_ECB) &&
            ((184 - u4Offset) != 0) && ((184 - u1PayloadOffset) > 16))
    {
        UINT8 u1Residual;

        x_memcpy(pu1Skip4 + u4Offset - 16, au1Backup, 16);       // Copy plain back

        u1Residual = 184 - u4Offset;
        x_memcpy(au1Text, (pu1Skip4 + (184 - 16)), 16);
        _SMS4_ByteOrder((UINT32*)au1Text, 16);
        SMS4Encrypt_ecb((UINT32*)au1Text);
        _SMS4_ByteOrder((UINT32*)au1Text, 16);
        x_memcpy((pu1Skip4 + 184 - u1Residual), (au1Text + (16 - u1Residual)), u1Residual);  // Copy back

        x_memcpy(au1Backup, pu1Skip4 + u4Offset - 16, u1Residual);
        x_memcpy(au1Backup + u1Residual, au1Text, 16 - u1Residual);
        _SMS4_ByteOrder((UINT32*)au1Backup, 16);
        SMS4Encrypt_ecb((UINT32*)au1Backup);
        _SMS4_ByteOrder((UINT32*)au1Backup, 16);
        x_memcpy(pu1Skip4 + u4Offset - 16, au1Backup, 16);  // Copy back
    }

    if ((prParam->eRtb == DMX_DESC_RTB_MODE_SCTE52) && ((184 - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        u1Cnt = 184 - u4Offset;
        x_memcpy(au1Text, pu1Skip4 + u4Offset, u1Cnt);   // Get plain text
        _SMS4_ByteOrder((UINT32*)au1Text, 16);
        SMS4Encrypt_scte52((UINT32*)au1Text, (UINT32*)au1InitVector);
        _SMS4_ByteOrder((UINT32*)au1Text, 16);
        x_memcpy(pu1Skip4 + u4Offset, au1Text, u1Cnt);  // Copy back
    }

_exit:

    return TRUE;
}


static BOOL _DmxEncryptSMS4(UINT32 u4Addr, UINT32 u4Size, SMS4_PARAM_T* prParam)
{
    ASSERT(_DMX_IsAligned(u4Size, 188));

    while (u4Size > 0)
    {
        if (!_EncryptSMS4Packet((UINT8*)u4Addr, prParam))
        {
            return FALSE;
        }

        u4Addr += 188;
        u4Size -= 188;
    }

    return TRUE;
}


static BOOL _SMS4Test(UINT8 u1TsIndex, UINT32 u4Size, SMS4_PARAM_T* prParam)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Flags;
    UINT16 u2Pid;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT32 u4Ctrl;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DMX_ES_FIFO_ALIGNMENT);
    u4PesAddr = (UINT32)x_mem_alloc(u4Size);
    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    u2Pid = prParam->u2Pid;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr, &u4PesSize))
    {
        goto _exit;
    }

    if (!_DmxEncryptSMS4(u4SrcAddr, u4Size, prParam))
    {
        goto _exit;
    }

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = prParam->u1KeyIdx;
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

    // Determine descramble mode
    rPid.eDescMode = DMX_DESC_MODE_NONE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //after set descmode none,dmx_setpid would't set descrambling scheme.
    //so we should set this after dmx_setpid
    _DMX_Lock();
    u4Ctrl = PID_INDEX_TABLE(u1Pidx);
    u4Ctrl |= 0x400; //set ca descramble enable
    PID_INDEX_TABLE(u1Pidx) = u4Ctrl;
    _DMX_Unlock();

    //Set DBM input source
    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
    if(u1TsIndex == 2)
    { 
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
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
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    return fgRet;

}


static BOOL _SMS4RecTest(UINT8 u1TsIndex, UINT32 u4Size, SMS4_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize;
    UINT32 u4GoldenAddr;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    FVR_PID_T rFVRPid;
    FVR_GBL_T rGblPid;
    UINT16 u2Pid;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = 0x3000000;
    u4DstAddr = 0x3400000;
    u4PesAddr = 0x3800000;
    u4GoldenAddr = 0x3C00000;

    u2Pid = prParam->u2Pid;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)u4SrcAddr, u4Size);

    if (!_DmxEncryptSMS4(u4SrcAddr, u4Size, prParam))
    {
        goto _exit;
    }

    //u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);

    // Set FVR
    VERIFY(FVR_Init());

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u4BufStart = u4DstAddr;
    rGblPid.u4BufSize = 0x100000;
    rGblPid.u2TargetPktCount = 0xFFFF;
    if (!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
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
    rFVRPid.u1DescKeyidx = prParam->u1KeyIdx;

    if (!FVR_SetPid(u1Pidx, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
    {
        return FALSE;
    }

    VERIFY(_FVR_Start(u1TsIndex));

    //Set DBM input source
    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
    if(u1TsIndex == 2)
    { 
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
    }


    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if (!_DDI_RECPULL_Transfer(u1TsIndex, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

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
    if (!_FVR_FreeGBL(u1TsIndex))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

    return fgRet;
}


static BOOL _SMS4RecLevel2Test(UINT8 u1TsIndex, UINT32 u4Size, SMS4_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize;
    UINT32 u4GoldenAddr;
    const UINT8 u1Pidx = 0;
    UINT16 u2Pid;
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
    u2Pid = prParam->u2Pid;

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)u4SrcAddr, u4Size);

    if (!_DmxEncryptSMS4(u4SrcAddr, u4Size, prParam))
    {
        goto _exit;
    }

    // Encrypt for level2
    // Set record key
    for (i=0; i<8; i++)
    {
        au4Key[i] = (UINT32)rand();
    }
    for (i=0; i<4; i++)
    {
        au4IV[i] = (UINT32)rand();
    }

    rParam2.eFeedbackMode = fgCBC ? DMX_CA_FB_CBC : DMX_CA_FB_ECB;
    rParam2.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    rParam2.u2KeyLen = (UINT16)u4KeyLen;
    rParam2.eKeySelect = KEY_SELECT_EVEN;
    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            rParam2.au1InitVector[i*4 + j] = GET_BYTE(au4IV[i], 3 - j);
        }
    }
    for (i=0; i<8; i++)
    {
        for (j=0; j<4; j++)
        {
            rParam2.arKey[0].au1Even[i*4 + j] = GET_BYTE(au4Key[i], j);
            rParam2.arKey[0].au1Odd[i*4 + j] = GET_BYTE(au4Key[i], j);
        }
    }
    for (i=0; i<DMX_NUM_PID_INDEX; i++)
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

    // Set FVR
    VERIFY(FVR_Init());

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u4BufStart = u4DstAddr;
    rGblPid.u4BufSize = 0x100000;
    rGblPid.u2TargetPktCount = 0xFFFF;
    if (!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
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
    rFVRPid.u1DescKeyidx = prParam->u1KeyIdx;;    // Descramble key
    rFVRPid.u1KeyIdx = 0;                                           // Encrypt key
    rFVRPid.fgCBCMode = fgCBC;
    rFVRPid.u4KeyLen = u4KeyLen;

    if (!FVR_SetPid(u1Pidx, FVR_PID_FLAG_ALL, &rFVRPid))
    {
        return FALSE;
    }

    if (!_FVR_SetAesKey(0, TRUE, (UINT8*)au4Key))
    {
        return FALSE;
    }
    if (!_FVR_SetAesKey(0, FALSE, (UINT8*)au4Key))
    {
        return FALSE;
    }
    if (fgCBC)
    {
        VERIFY(_FVR_SetAesEvenOddIV(0, TRUE, au4IV));
        VERIFY(_FVR_SetAesEvenOddIV(0, FALSE, au4IV));
    }

    VERIFY(_FVR_Start(u1TsIndex));

    //Set DBM input source
    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }
    if(u1TsIndex == 2)
    { 
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if (!_DDI_RECPULL_Transfer(u1TsIndex, u4SrcAddr, u4Size))
    {
        // Error on transferring data
        goto _exit;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

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
    if (!_FVR_FreeGBL(u1TsIndex))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_SMS4(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type)
{
    UINT32 i, j;
    //UCHAR* szKey;
    SMS4_PARAM_T rParam;
    UINT32 au4Data[4];

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    rParam.u2Pid = u2Pid;
    rParam.u1KeyIdx = (UINT8)random(DMX_DMEM_CA_KEY_NUM);
    rParam.eKeySelect = KEY_SELECT_EVEN;

    rParam.eFeedbackMode = (DMX_CA_FEEDBACK_MODE_T)random(4);
    rParam.eRtb = (DMX_DESC_RTB_MODE_T)random(3);
    if ((rParam.eRtb == DMX_DESC_RTB_MODE_SCTE52) && (rParam.eFeedbackMode != DMX_CA_FB_CBC))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }
    if ((rParam.eRtb == DMX_DESC_RTB_MODE_CTS) && (rParam.eFeedbackMode != DMX_CA_FB_ECB))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }

    if (rParam.eFeedbackMode != DMX_CA_FB_ECB)
    {
        for (i = 0; i < 16; i++)
        {
            rParam.au1InitVector[i] = random(256);
        }
    }

    // Generate keys
    for (i=0; i<(128/8); i++)
    {
        rParam.au1EvenKey[i] = random(256);
        rParam.au1OddKey[i] = random(256);
    }

    // Set keys
    for (i=0; i<4; i++)
    {
        au4Data[i] = 0;
        for (j=0; j<4; j++)
        {
            au4Data[i] |= (rParam.au1EvenKey[(i * 4) + j] << ((3 - j) * 8));
        }
    }
    if(!_DMX_SetSMS4Key(rParam.u1KeyIdx, TRUE, au4Data))
    {
        return FALSE;
    }

    for (i=0; i<4; i++)
    {
        au4Data[i] = 0;
        for (j=0; j<4; j++)
        {
            au4Data[i] |= (rParam.au1OddKey[(i * 4) + j] << ((3 - j) * 8));
        }
    }
    if(!_DMX_SetSMS4Key(rParam.u1KeyIdx, FALSE, au4Data))
    {
        return FALSE;
    }

    if (rParam.eFeedbackMode != DMX_CA_FB_ECB)
    {
        for (i = 0; i < 4; i++)
        {
            au4Data[i] = 0;

            for (j = 0; j < 4; j++)
            {
                au4Data[i] |= (rParam.au1InitVector[(i * 4) + j] << ((3 - j) * 8));
            }
        }
        // temp
        _DMX_SetSMS4IV(rParam.u1KeyIdx, au4Data, TRUE);
        _DMX_SetSMS4IV(rParam.u1KeyIdx, au4Data, FALSE);
    }

    if(!_DMX_SetCA_Mode(rParam.u1KeyIdx, DMX_CA_MODE_SMS4, rParam.eFeedbackMode, FALSE))
    {
        return FALSE;
    }

    _DMX_SetRtbMode(rParam.u1KeyIdx, rParam.eRtb);

    // Show mode and key select
    Printf("Key index: %u\n", rParam.u1KeyIdx);
    switch (rParam.eFeedbackMode)
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
    }

    // Show keys
    if (rParam.eKeySelect != KEY_SELECT_ODD)
    {
        Printf(" Even key: ");
        for (i = 0; i <(128/8); i++)
        {
            Printf("%02x", rParam.au1EvenKey[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }

    if (rParam.eKeySelect != KEY_SELECT_EVEN)
    {
        Printf(" Odd key: ");
        for (i = 0; i <(128/8); i++)
        {
            Printf("%02x", rParam.au1OddKey[i]);
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

    if (u1Type == 0)
    {
        return _SMS4Test(u1TsIndex, u4Size, &rParam);
    }
    else if (u1Type == 1)
    {
        return _SMS4RecTest(u1TsIndex, u4Size, &rParam);
    }
    else if (u1Type == 2)
    {
        return _SMS4RecLevel2Test(u1TsIndex, u4Size, &rParam);
    }

    return FALSE;
}

