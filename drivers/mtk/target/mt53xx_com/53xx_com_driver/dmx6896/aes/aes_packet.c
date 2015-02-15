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
 * $RCSfile: aes_packet.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_init.c
 *  Demux driver - initialization
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "../dmx_mm.h"
#include "../emu/dmx_emu.h"
#include "aes.h"
#include "aes_packet.h"
#include "x_assert.h"
#include "x_rand.h"
#include "x_os.h"


extern void aes_crypt_cbc_scte52( aes_context *ctx,
                    INT32 mode,
                    INT32 length,
                    UINT8 iv[16],
                    UINT8 *input,
                    UINT8 *output );

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

static UINT16 _GetPid(UINT8* pu1Buf)
{
    UINT16 u2Pid;

    pu1Buf++;
    u2Pid = ((*pu1Buf << 8) | *(pu1Buf + 1)) & 0x1fff;

    return u2Pid;
}


static BOOL _MatchKey(UINT16 u2Pid, AES_PARAM_T* prParam, UINT8* pu1KeyId)
{
    UINT8 u1Pidx;

    ASSERT(prParam != NULL);
    ASSERT(pu1KeyId != NULL);

    for(u1Pidx = 0; u1Pidx < DMX_NUM_PID_INDEX; u1Pidx++)
    {
        if (prParam->arKeyMap[u1Pidx].fgValid &&
            (u2Pid == prParam->arKeyMap[u1Pidx].u2Pid))
        {
            ASSERT(prParam->arKeyMap[u1Pidx].u1KeyIndex < DMX_DMEM_CA_KEY_NUM);
            *pu1KeyId = prParam->arKeyMap[u1Pidx].u1KeyIndex;

            return TRUE;
        }
    }

	return FALSE;
}


static void _hton64(UINT64 u8Src, UINT8* pu1Dst)
{
    UINT32 u4Bytes, i;
    UINT8* pu1Src;

    pu1Src = (UINT8*)&u8Src;
    u4Bytes = sizeof (UINT64);

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

static BOOL _EncryptCTR(UINT32 u4Addr, UINT32 u4Size, UINT8 *pu1Key, AES_PARAM_T* prParam)
{
    aes_context rCtx;
    UINT64 u8BlockId = 0;
	UINT8 au1Text[32], au1Output[32];
    UINT8* pu1Buf = (UINT8*)u4Addr;

    // Setup key scheduling
    aes_setkey_enc(&rCtx, pu1Key, prParam->u2KeyLen);

    // Copy sample id as first half of text
    x_memcpy(au1Text, prParam->au1SampleId, 8);

    while (u4Size > 0)
    {
        UINT32 u4RoundSize;

        if(!prParam->fgCTRResidual)
        {
            if(u4Size < 16)
            {
                break;
            }
        }

        u4RoundSize = MIN(u4Size, 16);

        // Copy block id as second half of text
        _hton64(u8BlockId, au1Text + 8);

        aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Output);

        _Xor(pu1Buf, au1Output, u4RoundSize);

        pu1Buf += u4RoundSize;
        u4Size -= u4RoundSize;
        u8BlockId++;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL _DMX_EncryptAesPacket(UINT8* pu1Buf, AES_PARAM_T* prParam, UINT8 u1ForceKeyId)
{
    aes_context rCtx;
	UINT8* pu1Skip4 = pu1Buf + 4;
	UINT32 u4Offset, u4KeyLen;
	UINT16 u2Pid = 0;
	UINT8 au1Text[32], au1InitVector[16];
    UINT8 u1AdaptField = 0, u1PayloadOffset = 0;
	UINT8 u1KeyId;
    BOOL fgEven = TRUE;
    UINT8 au1Backup[16];

// Debug
//  UINT8 au1Key[32];
    UINT8* au1Key = au1Text;

    ASSERT(pu1Buf != NULL);
    ASSERT(prParam != NULL);
    ASSERT((prParam->u2KeyLen == 128) || (prParam->u2KeyLen == 192) ||
        (prParam->u2KeyLen == 256));

    if(u1ForceKeyId == 0xFF)
    {
        u2Pid = _GetPid(pu1Buf);

        if (!_MatchKey(u2Pid, prParam, &u1KeyId))
        {
            return TRUE;
        }
    }
    else
    {
        u1KeyId = u1ForceKeyId;
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

    u1AdaptField = (pu1Buf[3] >> 4) & 0x3;
    if (0 == (u1AdaptField & 0x1))  // 00 and 10 => skip
    {
        // Follow hardware
        if(fgEven)  // Even
        {
    		pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
        }
        else        // Odd
        {
    		pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
        }
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

    if(u1KeyId >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    u4KeyLen = prParam->u2KeyLen / 8;

    if(u4KeyLen > 32)
    {
        return FALSE;
    }

    if(fgEven)  // Even
    {
        x_memcpy(au1Key, prParam->arKey[u1KeyId].au1Even, u4KeyLen);
		pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
    }
    else        // Odd
    {
        x_memcpy(au1Key, prParam->arKey[u1KeyId].au1Odd, u4KeyLen);
		pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
    }

    if(prParam->eFeedbackMode == DMX_CA_FB_CTR)
    {
        return _EncryptCTR((UINT32)(pu1Skip4 + u1PayloadOffset), 184 - u1PayloadOffset, au1Key, prParam);
    }

    // Setup key scheduling
    aes_setkey_enc(&rCtx, au1Key, prParam->u2KeyLen);

    if (prParam->eFeedbackMode != DMX_CA_FB_ECB)
    {
        x_memcpy(au1InitVector, prParam->au1InitVector, 16);
    }

    // make klocwork happy
    x_memset(au1Backup, 0, 16);
    
    for (u4Offset = u1PayloadOffset; u4Offset <= (TS_PACKET_SIZE - 20);
        u4Offset += 16)
    {
        x_memcpy(au1Backup, pu1Skip4 + u4Offset, 16);   // Get plain text
        x_memcpy(au1Text, pu1Skip4 + u4Offset, 16);   // Get plain text

        // Encrypt
        if(prParam->eFeedbackMode == DMX_CA_FB_CBC)
        {
            aes_crypt_cbc(&rCtx, AES_ENCRYPT, 16, au1InitVector, au1Text, au1Text);
        }
        else if(prParam->eFeedbackMode == DMX_CA_FB_CFB)
        {
            INT32 iv_offset = 0;
            aes_crypt_cfb(&rCtx, AES_ENCRYPT, 16, &iv_offset, au1InitVector, au1Text, au1Text);
        }
        else if(prParam->eFeedbackMode == DMX_CA_FB_OFB)
        {
            INT32 iv_offset = 0;
            aes_crypt_ofb(&rCtx, AES_ENCRYPT, 16, (int *)&iv_offset, au1InitVector, au1Text, au1Text);
        }
        else if(prParam->eFeedbackMode == DMX_CA_FB_ECB)
        {
            aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Text);
        }

        x_memcpy(pu1Skip4 + u4Offset, au1Text, 16);  // Copy back
    }

    if((prParam->eRtb == DMX_DESC_RTB_MODE_CTS) && (prParam->eFeedbackMode == DMX_CA_FB_ECB) &&
       ((184 - u4Offset) != 0) && ((184 - u1PayloadOffset) > 16))
    {
        UINT8 u1Residual;

        x_memcpy(pu1Skip4 + u4Offset - 16, au1Backup, 16);       // Copy plain back

        u1Residual = 184 - u4Offset;
        x_memcpy(au1Text, (pu1Skip4 + (184 - 16)), 16);
        aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Text);
        x_memcpy((pu1Skip4 + 184 - u1Residual), (au1Text + (16 - u1Residual)), u1Residual);  // Copy back

        x_memcpy(au1Backup, pu1Skip4 + u4Offset - 16, u1Residual);
        x_memcpy(au1Backup + u1Residual, au1Text, 16 - u1Residual);
        aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Backup, au1Backup);
        x_memcpy(pu1Skip4 + u4Offset - 16, au1Backup, 16);  // Copy back
    }

    if((prParam->eRtb == DMX_DESC_RTB_MODE_SCTE52) && ((184 - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = 184 - u4Offset;
        x_memcpy(au1Text, pu1Skip4 + u4Offset, u1Cnt);   // Get plain text
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1InitVector, au1Text, au1Texto);
        x_memcpy(pu1Skip4 + u4Offset, au1Texto, u1Cnt);  // Copy back
    }

_exit:

    return TRUE;
}

