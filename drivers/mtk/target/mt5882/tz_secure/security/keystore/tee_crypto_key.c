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
#include "tzst.h"
#include "tz_dmx_if.h"
#include "tee_dtcp.h"
#include "tee_crypto_key.h"
#include <tomcrypt.h>

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Extern Functions and Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT8 au1Khdist[] = {0x87, 0x5A, 0x78, 0x6F, 0x55, 0xB9, 0xB8, 0x23, 0x68, 0x89, 0x78, 0x84, 0xC8, 0x1A, 0x4A, 0x65};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static BOOL TZ_HMAC_SHA256(const UINT8 *pu1Key, UINT32 u4KeyLen,
                    const UINT8 *pMsg, UINT32 u4MsgLen,
                    UINT8 *pu1Out, UINT32 *pu4OutLen)
{
    UINT32 u4OutLen = *pu4OutLen;
   
    if (register_hash(&sha256_desc) < 0)
    {
        return FALSE;
    }

    if (CRYPT_OK == hmac_memory(find_hash("sha256"), pu1Key, u4KeyLen, pMsg, u4MsgLen, pu1Out, &u4OutLen))
    {
        *pu4OutLen = u4OutLen;
        return TRUE;
    }
    
    return FALSE;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** _teeCryptoDecryptSecurePvrAesKey0
 *  Decrypt secure PVR AES key0. This key will be used by SONY and Turnkey.
 *
 *  @param  pu1CipherKey                Pointer to the cipher key
 *  @param  pu1PlainKey                 Pointer to the plain key
 *  @param  u4KeyLen                    The key length
 *
 *  @retval TZ_SUCCESS                  Succeed
 *          TZ_ERROR_ILLEGAL_ARGUMENT   Illegal argument
 *          TZ_ERROR_CRYPTO             crypto error
 */
//-----------------------------------------------------------------------------
UINT32 _teeCryptoDecryptSecurePvrAesKey0(const UINT8 *pu1CipherKey, UINT8 *pu1PlainKey, UINT32 u4KeyLen)
{
    UINT8 au1CtrKey[16] = {0x77, 0x00, 0x51, 0xC1, 0x7E, 0xD4, 0x7A, 0xAE,
                        0x5D, 0x03, 0x2B, 0xC2, 0x04, 0x50, 0xA8, 0x19};

    UINT8 au1CtrIv[16] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};
    UINT8 au1PvrKey[32];
    UINT32 u4RandNum, i;
    BOOL fgRet;
    symmetric_CTR rCTR;

    if (!pu1CipherKey || !pu1PlainKey || (u4KeyLen > sizeof(au1PvrKey)))
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (register_cipher(&aes_desc) < 0)
    {
        return TZ_ERROR_CRYPTO;
    }

    memcpy_n2s_chk(au1PvrKey, pu1CipherKey, u4KeyLen, NULL);

    ctr_start(find_cipher("aes"), au1CtrIv, au1CtrKey, sizeof(au1CtrKey), 0, CTR_COUNTER_BIG_ENDIAN, &rCTR);
    ctr_decrypt(&au1PvrKey[0], &u4RandNum, 4, &rCTR);
    ctr_done(&rCTR);
    
    memcpy_s2s_chk((void*)&au1PvrKey[0], (void*)&u4RandNum, sizeof(u4RandNum), NULL);
    memcpy_s2s_chk((void*)&au1PvrKey[8], (void*)&u4RandNum, sizeof(u4RandNum), NULL);
    memcpy_s2s_chk((void*)pu1PlainKey, (void*)au1PvrKey, u4KeyLen, NULL);
    
    dprintf("[TZ]secure key\n");
    for (i = 0; i < u4KeyLen; i++)
    {
        dprintf("0x%02x ", pu1PlainKey[i]);
    }
    dprintf("\n");

    return TZ_SUCCESS;
}

//-----------------------------------------------------------------------------
/** _teeCryptoSignWithKL
 *  Sign the message with local key.
 *
 *  @param  pu1Data                     Pointer to the message
 *  @param  u4Size                      message size
 *  @param  pu1Digest                   Pointer to a buffer that receives a generated signature
 *
 *  @retval TZ_SUCCESS                  Succeed
 *          TZ_ERROR_ILLEGAL_ARGUMENT   Illegal argument
 *          TZ_ERROR_CRYPTO             crypto error
 */
//-----------------------------------------------------------------------------
UINT32 _teeCryptoSignWithKL(const UINT8 *pu1Data, UINT32 u4Size, UINT8 pu1Digest[TEE_CRYPTO_SIGNATURE_SIZE_IN_BYTES])
{
    UINT8 au1HmacOut[32];
    UINT32 u4OutLen = sizeof(au1HmacOut);

    if (!pu1Data)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (!TZ_HMAC_SHA256(au1Khdist, sizeof(au1Khdist), pu1Data, u4Size, au1HmacOut, &u4OutLen))
    {
        return TZ_ERROR_CRYPTO;
    }

    memcpy_s2s_chk((void*)pu1Digest, (void*)au1HmacOut, u4OutLen, NULL);

    return TZ_SUCCESS;
}
