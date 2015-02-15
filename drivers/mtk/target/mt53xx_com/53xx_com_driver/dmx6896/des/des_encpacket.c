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
 * $RCSfile: des_encpacket.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_init.c
 *  Demux driver - initialization
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "../dmx.h"
#include "dmx_drm_drvif.h"
#include "des_encrypt.h"
#include "x_assert.h"
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

    for(i4Key = 0; i4Key < MAX_KEY_NO; i4Key++)
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

        if(!prParam->fgCTRResidual)
        {
            if(u4Size < 8)
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

BOOL _DMX_EncryptDesPacket(UINT8* pu1Buf, DES_PARAM_T* prParam)
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

        if(fgEven)  // Even
        {
            x_memcpy(au1Key, prParam->arEnPid[i4WhichKey].au1EvenKey, u4KeyLen);
			pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
        }
        else        // Odd
        {
            x_memcpy(au1Key, prParam->arEnPid[i4WhichKey].au1OddKey, u4KeyLen);
			pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
        }

        if(prParam->eFeedbackMode == DMX_CA_FB_CTR)
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

            if(prParam->eFeedbackMode == DMX_CA_FB_ECB)
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
            else if(prParam->eFeedbackMode == DMX_CA_FB_CBC)
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
            else if(prParam->eFeedbackMode == DMX_CA_FB_CFB)
            {
                INT32 iv_offset = 0;
                if(prParam->fg3Des)
                {
                    des3_crypt_cfb(r3DesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
                else
                {
                    des_crypt_cfb(rDesKs, DES_ENCRYPT, 8, (int*)&iv_offset, au1InitVector, au1Text, au1Text);
                }
            }
            else if(prParam->eFeedbackMode == DMX_CA_FB_OFB)
            {
                INT32 iv_offset = 0;
                if(prParam->fg3Des)
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

        if((prParam->eRtb == DMX_DESC_RTB_MODE_CTS) && (prParam->eFeedbackMode == DMX_CA_FB_ECB) &&
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

        if((prParam->eRtb == DMX_DESC_RTB_MODE_SCTE52) && ((TS_PACKET_SIZE - 4 - u4Offset) != 0))
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

    if( mode == DES_ENCRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                des(ks, iv);

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
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

    if( mode == DES_ENCRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                des(ks, iv);

            *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
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

    if( mode == DES_ENCRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                des3(ks, iv);

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
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

    if( mode == DES_ENCRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                des3(ks, iv);

            *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = (n + 1) & 0x07;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
                des3(ks, iv);

            *output++ = (unsigned char)( *input++ ^ iv[n] );

            n = (n + 1) & 0x07;
        }
    }

    *iv_off = n;
}

