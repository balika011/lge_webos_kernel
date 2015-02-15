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
 * $RCSfile: dmx_emu_des.c,v $
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
#include "../des/des_encrypt.h"
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

/******************************************************************************
* TS index 0 and 1 make use of key index 0 ~  7.
* TS index 2 and 3 make use of key index 8 ~ 15.
* This function takes TS index into account when setting keys.
******************************************************************************/
static BOOL _SetKeys(UINT8 u1TsIndex, DES_PARAM_T* prParam)
{
    UINT32 i, j;
    UINT32 au4EvenKey[6], au4OddKey[6];
    UINT8* p;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    for (i = 0; i < MAX_KEY_NO; i++)
    {
        if (prParam->arEnPid[i].fgValid)
        {
            for (j = 0; j < 6; j++)
            {
                p = prParam->arEnPid[i].au1EvenKey + (j * 4);
                au4EvenKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

                p = prParam->arEnPid[i].au1OddKey + (j * 4);
                au4OddKey[j] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            }
        }
        else
        {
            // Reset keys
            for (j = 0; j < 6; j++)
            {
                au4EvenKey[j] = 0;
                au4OddKey[j] = 0;
            }
        }

        // Set keys
        VERIFY(_DMX_SetDesKey(i, prParam->fg3Des, TRUE, au4EvenKey));
        VERIFY(_DMX_SetDesKey(i, prParam->fg3Des, FALSE, au4OddKey));
    }

    return TRUE;
}


static BOOL _EncryptDesPackets(UINT32 u4Addr, UINT32 u4Size,
    DES_PARAM_T* prParam)
{
    ASSERT(_DMX_IsAligned(u4Size, 188));

    while (u4Size > 0)
    {
        if (!_DMX_EncryptDesPacket((UINT8*)u4Addr, prParam))
        {
            return FALSE;
        }

        u4Addr += 188;
        u4Size -= 188;
    }

    return TRUE;
}


static BOOL _DesTest(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1KeyIdx,
    DES_PARAM_T* prParam)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;

    ASSERT(_DMX_IsAligned(u4Size, 188));

    // Allocate source and destination buffers
    u4SrcAddr = 0x2000000;
        //(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT);
    u4DstAddr = 0x2100000;//(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1,
        //DMX_ES_FIFO_ALIGNMENT);
    u4PesAddr = 0x2200000;//(UINT32)x_mem_alloc(u4Size);
    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
        &u4PesSize))
    {
        goto _exit;
    }

    if (!_EncryptDesPackets(u4SrcAddr, u4Size, prParam))
    {
        goto _exit;
    }

    u4DstEnd = _DMX_Align(u4DstAddr + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = u1KeyIdx;    // Map to key 0 only, fixme
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


static BOOL _DesRecTest(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, 
                        UINT8 u1KeyIdx, DES_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize, u4Flags;
    UINT32 u4GoldenAddr;
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

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
        &u4PesSize))
    {
        goto _exit;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)u4SrcAddr, u4Size);

    if (!_EncryptDesPackets(u4SrcAddr, u4Size, prParam))
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
    rFVRPid.u1DescKeyidx = u1KeyIdx;

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
    //if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
    if(!_DDI_RECPULL_Transfer(u1TsIndex, u4SrcAddr, u4Size))
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
    if(!_FVR_FreeGBL(u1TsIndex))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

    return fgRet;
}

static BOOL _DesRecLevel2Test(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid,
                    UINT8 u1KeyIdx, DES_PARAM_T* prParam)
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize;
    UINT32 u4GoldenAddr;
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

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4Size, u2Pid, u4PesAddr,
        &u4PesSize))
    {
        goto _exit;
    }

    x_memcpy((void*)u4GoldenAddr, (void*)u4SrcAddr, u4Size);

    if (!_EncryptDesPackets(u4SrcAddr, u4Size, prParam))
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
    rFVRPid.u1DescKeyidx = u1KeyIdx;    // Descramble key
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

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

BOOL DMX_EMU_Des0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type)
{
    DES_PARAM_T rParam;
    UINT32 u4KeyLen, i, j;
    UCHAR* szKey;
    DMX_CA_MODE_T eMode;
    static UINT8 _u1KeyIdx = 0;

    _u1KeyIdx++;
    if(_u1KeyIdx >= DMX_DMEM_CA_KEY_NUM)
    {
        _u1KeyIdx = 0;
    }

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

    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        for (i = 0; i < 8; i++)
        {
            rParam.au1InitVector[i] = random(256);
        }
    }

    // DES or 3DES
    rParam.fg3Des = (random(2) == 0);

    // Key select
    rParam.eKeySelect = (KEY_SELECT_T)random(3);

    // Map to key 0 only, fixme
    for (i = 0; i < MAX_KEY_NO; i++)
    {
        rParam.arEnPid[i].fgValid = FALSE;
    }
    rParam.arEnPid[_u1KeyIdx].fgValid = TRUE;

    // EnPid
    for (i = 0; i < MAX_KEY_NO; i++)
    {
        if (rParam.arEnPid[i].fgValid)
        {
            rParam.arEnPid[i].u2Pid = u2Pid;
            u4KeyLen = (rParam.fg3Des ? 24 : 8);

            for (j = 0; j < u4KeyLen; j++)
            {
                rParam.arEnPid[i].au1EvenKey[j] = random(256);
                rParam.arEnPid[i].au1OddKey[j] = random(256);
            }
        }
    }

    // Set keys
    if (!_SetKeys(u1TsIndex, &rParam))
    {
        return FALSE;
    }

    if ((rParam.eFeedbackMode != DMX_CA_FB_ECB) && (rParam.eFeedbackMode != DMX_CA_FB_CTR))
    {
        // Set initial vector
        UINT32 u4Hi, u4Lo;

        u4Hi = (rParam.au1InitVector[0] << 24) | (rParam.au1InitVector[1] << 16) |
            (rParam.au1InitVector[2] << 8) | rParam.au1InitVector[3];

        u4Lo = (rParam.au1InitVector[4] << 24) | (rParam.au1InitVector[5] << 16) |
            (rParam.au1InitVector[6] << 8) | rParam.au1InitVector[7];

        if(rParam.eKeySelect == KEY_SELECT_RANDOM) 
        {
            _DMX_SetDesIVEx2(_u1KeyIdx, u4Hi, u4Lo, TRUE);
            _DMX_SetDesIVEx2(_u1KeyIdx, u4Hi, u4Lo, FALSE);
        }
        else if(rParam.eKeySelect == KEY_SELECT_EVEN)
        {
            _DMX_SetDesIVEx2(_u1KeyIdx, u4Hi, u4Lo, TRUE);
        }
        else if(rParam.eKeySelect == KEY_SELECT_ODD)
        {
            _DMX_SetDesIVEx2(_u1KeyIdx, u4Hi, u4Lo, FALSE);
        }
    }

    if(rParam.fg3Des)
    {
        eMode = DMX_CA_MODE_3DES;
    }
    else
    {
        eMode = DMX_CA_MODE_DES;
    }
    
    if(!_DMX_SetCA_Mode(_u1KeyIdx, eMode, rParam.eFeedbackMode, FALSE))
    {
        return FALSE;
    }

    _DMX_SetDesRtbMode(_u1KeyIdx, rParam.eRtb);

    if(rParam.eFeedbackMode == DMX_CA_FB_CTR)
    {
        UINT32 u4SampleId1;
        UINT64 u8Nonce;

        u4SampleId1 = (rParam.au1SampleId[0] << 24) | (rParam.au1SampleId[1] << 16) |
                      (rParam.au1SampleId[2] << 8) | rParam.au1SampleId[3];

        u8Nonce = ((UINT64)u4SampleId1 << 32);

        _DMX_SetCA_CTR(_u1KeyIdx, u8Nonce, 0, TRUE);
        _DMX_SetCA_CTR(_u1KeyIdx, u8Nonce, 0, FALSE);

        _DMX_SetCTR_Rtb(_u1KeyIdx, rParam.fgCTRResidual);
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

    // Show mode and key select
    Printf("    Mode: %s, key select: %s\n", rParam.fg3Des ? "3DES" : "DES", szKey);
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
    u4KeyLen = (rParam.fg3Des ? 24 : 8);
    if (rParam.eKeySelect != KEY_SELECT_ODD)
    {
        Printf("    Even key: ");
        for (i = 0; i < u4KeyLen; i++)
        {
            Printf("%02x", rParam.arEnPid[_u1KeyIdx].au1EvenKey[i]);
            if ((i % 8) == 7)
            {
                Printf("-");
            }
        }
        Printf("\n");
    }
    if (rParam.eKeySelect != KEY_SELECT_EVEN)
    {
        Printf("    Odd key:  ");
        for (i = 0; i < u4KeyLen; i++)
        {
            Printf("%02x", rParam.arEnPid[_u1KeyIdx].au1OddKey[i]);
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
        Printf("    Init vector: ");
        for (i = 0; i < 8; i++)
        {
            Printf("%02x", rParam.au1InitVector[i]);
        }
        Printf("\n");
    }

    if(u1Type == 0)
    {
        return _DesTest(u1TsIndex, u4Size, u2Pid, _u1KeyIdx, &rParam);
    }
    else if(u1Type == 1)
    {
        return _DesRecTest(u1TsIndex, u4Size, u2Pid, _u1KeyIdx, &rParam); 
    }
    else if(u1Type == 2)
    {
        return _DesRecLevel2Test(u1TsIndex, u4Size, u2Pid, _u1KeyIdx, &rParam);
    }

    return FALSE;
}

