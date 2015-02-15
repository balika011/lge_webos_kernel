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
static void _memxor(UINT8* pu1Dest, UINT8* pu1Src, INT32 i4Count)
{
    INT32 i;

    for (i = 0; i < i4Count; i++)
    {
        pu1Dest[i] ^= pu1Src[i];
    }
}

static UINT16 _GetPid(UINT8* pu1Buf)
{
    UINT16 u2Pid;

    ASSERT(pu1Buf != NULL);

    pu1Buf++;
    u2Pid = ((*pu1Buf << 8) | *(pu1Buf + 1)) & 0x1fff;

    return u2Pid;
}

static INT32 _MatchKey(UINT16 u2Pid, DES_PARAM_T* prParam)
{
    INT32 i4Key;

    ASSERT(prParam != NULL);

    for (i4Key = 0; i4Key < MAX_KEY_NO; i4Key++)
    {
        if (prParam->arEnPid[i4Key].fgValid &&
                (u2Pid == prParam->arEnPid[i4Key].u2Pid))
        {
            return i4Key;
        }
    }

    return -1;
}

static void _hton32(UINT32 u4Src, UINT8* pu1Dst)
{
    UINT32 u4Bytes, i;
    UINT8* pu1Src;

    pu1Src = (UINT8*)&u4Src;
    u4Bytes = sizeof (UINT32);

    for (i = 0; i < u4Bytes; i++)
    {
        pu1Dst[i] = pu1Src[u4Bytes - 1 - i];
    }
}

static void _Xor(UINT8* pu1Dst, UINT8* pu1Src, UINT32 u4Size)
{
    UINT32 i;

    for (i = 0; i < u4Size; i++)
    {
        pu1Dst[i] ^= pu1Src[i];
    }
}

static BOOL _DesEncryptCTR(UINT32 u4Addr, UINT32 u4Size, UINT8 *pu1Key, DES_PARAM_T* prParam)
{
    DES3_KS r3DesKs;
    DES_KS rDesKs;
    UINT32 u4BlockId = 0;
    UINT8 au1Text[8], au1Output[8];
    UINT8* pu1Buf = (UINT8*)u4Addr;

    // Setup key scheduling
    if (prParam->fg3Des)
    {
        des3key(r3DesKs, pu1Key, 0);
    }
    else
    {
        deskey(rDesKs, pu1Key, 0);
    }

    // Copy sample id as first half of text
    x_memcpy(au1Text, prParam->au1SampleId, 4);

    while (u4Size > 0)
    {
        UINT32 u4RoundSize;

        if (!prParam->fgCTRResidual)
        {
            if (u4Size < 8)
            {
                break;
            }
        }

        u4RoundSize = MIN(u4Size, 8);

        // Copy block id as second half of text
        _hton32(u4BlockId, au1Text + 4);

        x_memcpy(au1Output, au1Text, 8);
        if (prParam->fg3Des)
        {
            des3(r3DesKs, au1Output);
        }
        else
        {
            des(rDesKs, au1Output);
        }

        _Xor(pu1Buf, au1Output, u4RoundSize);

        pu1Buf += u4RoundSize;
        u4Size -= u4RoundSize;
        u4BlockId++;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

static BOOL _DMX_EncryptDesPacket(UINT8* pu1Buf, DES_PARAM_T* prParam)
{
    DES3_KS r3DesKs;
    DES_KS rDesKs;
    UINT8* pu1Skip4 = pu1Buf + 4;
    UINT32 u4Offset, u4KeyLen;
    INT32 i4WhichKey = -1;
    UINT16 u2Pid = 0;
    UINT8 au1Key[24], au1Text[8], au1InitVector[8];
    UINT8 u1AdaptField = 0, u1PayloadOffset = 0;
    BOOL fgEven = TRUE;
    UINT8 au1Backup[8];

    ASSERT(pu1Buf != NULL);
    ASSERT(prParam != NULL);

    u2Pid = _GetPid(pu1Buf);

    if ((i4WhichKey = _MatchKey(u2Pid, prParam)) >= 0)
    {
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

        u4KeyLen = (prParam->fg3Des ? 24 : 8);

        if (fgEven) // Even
        {
            x_memcpy(au1Key, prParam->arEnPid[i4WhichKey].au1EvenKey, u4KeyLen);
            pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
        }
        else        // Odd
        {
            x_memcpy(au1Key, prParam->arEnPid[i4WhichKey].au1OddKey, u4KeyLen);
            pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
        }

        if (prParam->eFeedbackMode == DMX_CA_FB_CTR)
        {
            return _DesEncryptCTR((UINT32)(pu1Skip4 + u1PayloadOffset),
                                  184 - u1PayloadOffset, au1Key, prParam);
        }

        if (prParam->fg3Des)
        {
            des3key(r3DesKs, au1Key, 0);
        }
        else
        {
            deskey(rDesKs, au1Key, 0);
        }

        // make klocwork happy
        x_memset(au1Backup, 0, 8);
        x_memset(au1InitVector, 0, 8);

        if (prParam->eFeedbackMode != DMX_CA_FB_ECB)
        {
            x_memcpy(au1InitVector, prParam->au1InitVector, 8);
        }

        for (u4Offset = u1PayloadOffset; u4Offset <= (TS_PACKET_SIZE - 12);
                u4Offset += 8)
        {
            x_memcpy(au1Backup, pu1Skip4 + u4Offset, 8);   // Get plain text
            x_memcpy(au1Text, pu1Skip4 + u4Offset, 8);   // Get plain text

            if (prParam->eFeedbackMode == DMX_CA_FB_ECB)
            {
                if (prParam->fg3Des)
                {
                    des3(r3DesKs, au1Text);
                }
                else
                {
                    des(rDesKs, au1Text);
                }
            }
            else if (prParam->eFeedbackMode == DMX_CA_FB_CBC)
            {
                _memxor(au1Text, au1InitVector, 8);

                if (prParam->fg3Des)
                {
                    des3(r3DesKs, au1Text);
                }
                else
                {
                    des(rDesKs, au1Text);
                }

                x_memcpy(au1InitVector, au1Text, 8);
            }
            else if (prParam->eFeedbackMode == DMX_CA_FB_CFB)
            {
                INT32 iv_offset = 0;
                if (prParam->fg3Des)
                {
                    des3_crypt_cfb(r3DesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
                else
                {
                    des_crypt_cfb(rDesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
            }
            else if (prParam->eFeedbackMode == DMX_CA_FB_OFB)
            {
                INT32 iv_offset = 0;
                if (prParam->fg3Des)
                {
                    des3_crypt_ofb(r3DesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
                else
                {
                    des_crypt_ofb(rDesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
            }
            x_memcpy(pu1Skip4 + u4Offset, au1Text, 8);  // Copy back
        }

        if ((prParam->eRtb == DMX_DESC_RTB_MODE_CTS) && (prParam->eFeedbackMode == DMX_CA_FB_ECB) &&
                ((184 - u4Offset) != 0) && ((184 - u1PayloadOffset) > 8))
        {
            UINT8 u1Residual;

            x_memcpy(pu1Skip4 + u4Offset - 8, au1Backup, 8);       // Copy plain back

            u1Residual = 184 - u4Offset;
            x_memcpy(au1Text, (pu1Skip4 + (184 - 8)), 8);
            if (prParam->fg3Des)
            {
                des3(r3DesKs, au1Text);
            }
            else
            {
                des(rDesKs, au1Text);
            }
            x_memcpy((pu1Skip4 + 184 - u1Residual), (au1Text + (8 - u1Residual)), u1Residual);  // Copy back

            x_memcpy(au1Backup, pu1Skip4 + u4Offset - 8, u1Residual);
            x_memcpy(au1Backup + u1Residual, au1Text, 8 - u1Residual);
            if (prParam->fg3Des)
            {
                des3(r3DesKs, au1Backup);
            }
            else
            {
                des(rDesKs, au1Backup);
            }
            x_memcpy(pu1Skip4 + u4Offset - 8, au1Backup, 8);  // Copy back
        }

        if ((prParam->eRtb == DMX_DESC_RTB_MODE_SCTE52) && ((TS_PACKET_SIZE - 4 - u4Offset) != 0))
        {
            UINT8 u1Cnt = TS_PACKET_SIZE - 4 - u4Offset;

            if (prParam->fg3Des)
            {
                des3(r3DesKs, au1InitVector);
            }
            else
            {
                des(rDesKs, au1InitVector);
            }

            x_memcpy(au1Text, pu1Skip4 + u4Offset, u1Cnt);   // Get plain text
            _memxor(au1Text, au1InitVector, u1Cnt);
            x_memcpy(pu1Skip4 + u4Offset, au1Text, u1Cnt);  // Copy back
        }

    }

_exit:

    return TRUE;
}

/*
 * DES-CFB buffer encryption/decryption
 */
void des_crypt_cfb( DES_KS ks,
                    int mode,
                    int length,
                    int *iv_off,
                    unsigned char iv[8],
                    unsigned char *input,
                    unsigned char *output )
{
    int c, n = *iv_off;

    if ( mode == DES_ENCRYPT )
    {
        while ( length-- )
        {
            if ( n == 0 )
                des(ks, iv);

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while ( length-- )
        {
            if ( n == 0 )
                des(ks, iv);

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = (n + 1) & 0x07;
        }
    }

    *iv_off = n;
}

/*
 * DES-OFB buffer encryption/decryption
 */
void des_crypt_ofb( DES_KS ks,
                    int mode,
                    int length,
                    int *iv_off,
                    unsigned char iv[8],
                    unsigned char *input,
                    unsigned char *output )
{
    int n = *iv_off;

    if ( mode == DES_ENCRYPT )
    {
        while ( length-- )
        {
            if ( n == 0 )
                des(ks, iv);

            *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while ( length-- )
        {
            if ( n == 0 )
                des(ks, iv);

            *output++ = (unsigned char)( *input++ ^ iv[n] );

            n = (n + 1) & 0x07;
        }
    }

    *iv_off = n;
}

/*
 * 3DES-CFB buffer encryption/decryption
 */
void des3_crypt_cfb( DES3_KS ks,
                     int mode,
                     int length,
                     int *iv_off,
                     unsigned char iv[8],
                     unsigned char *input,
                     unsigned char *output )
{
    int c, n = *iv_off;

    if ( mode == DES_ENCRYPT )
    {
        while ( length-- )
        {
            if ( n == 0 )
                des3(ks, iv);

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while ( length-- )
        {
            if ( n == 0 )
                des3(ks, iv);

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = (n + 1) & 0x07;
        }
    }

    *iv_off = n;
}

/*
 * 3DES-OFB buffer encryption/decryption
 */
void des3_crypt_ofb( DES3_KS ks,
                     int mode,
                     int length,
                     int *iv_off,
                     unsigned char iv[8],
                     unsigned char *input,
                     unsigned char *output )
{
    int n = *iv_off;

    if ( mode == DES_ENCRYPT )
    {
        while ( length-- )
        {
            if ( n == 0 )
                des3(ks, iv);

            *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while ( length-- )
        {
            if ( n == 0 )
                des3(ks, iv);

            *output++ = (unsigned char)( *input++ ^ iv[n] );

            n = (n + 1) & 0x07;
        }
    }

    *iv_off = n;
}

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
    UINT32 u4Ctrl;

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
    rPid.eDescMode = DMX_DESC_MODE_NONE; //this is workaround for mt5398 fpga emulation

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
    UINT32 u4PesAddr, u4PesSize;
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
    rFVRPid.u1DescKeyidx = u1KeyIdx;

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
    //if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4Size))
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
    rFVRPid.u1DescKeyidx = u1KeyIdx;    // Descramble key
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

BOOL DMX_EMU_Des0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type)
{
    DES_PARAM_T rParam;
    UINT32 u4KeyLen, i, j;
    UCHAR* szKey = "EVEN";
    DMX_CA_MODE_T eMode;
    static UINT8 _u1KeyIdx = 0;

    _u1KeyIdx++;
    if (_u1KeyIdx >= DMX_DMEM_CA_KEY_NUM)
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
    if ((rParam.eRtb == DMX_DESC_RTB_MODE_SCTE52) && (rParam.eFeedbackMode != DMX_CA_FB_CBC))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }
    if ((rParam.eRtb == DMX_DESC_RTB_MODE_CTS) && (rParam.eFeedbackMode != DMX_CA_FB_ECB))
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    }
    if (rParam.eFeedbackMode == DMX_CA_FB_CTR)
    {
        rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
        for (i=0; i<8; i++)
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

        if (rParam.eKeySelect == KEY_SELECT_RANDOM)
        {
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, TRUE);
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, FALSE);
        }
        else if (rParam.eKeySelect == KEY_SELECT_EVEN)
        {
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, TRUE);
        }
        else if (rParam.eKeySelect == KEY_SELECT_ODD)
        {
            _DMX_SetDesIV(_u1KeyIdx, u4Hi, u4Lo, FALSE);
        }
    }

    if (rParam.fg3Des)
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

    _DMX_SetRtbMode(_u1KeyIdx, rParam.eRtb);

    if (rParam.eFeedbackMode == DMX_CA_FB_CTR)
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

    if (u1Type == 0)
    {
        return _DesTest(u1TsIndex, u4Size, u2Pid, _u1KeyIdx, &rParam);
    }
    else if (u1Type == 1)
    {
        return _DesRecTest(u1TsIndex, u4Size, u2Pid, _u1KeyIdx, &rParam);
    }
    else if (u1Type == 2)
    {
        return _DesRecLevel2Test(u1TsIndex, u4Size, u2Pid, _u1KeyIdx, &rParam);
    }

    return FALSE;
}

