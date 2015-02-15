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
#include "tee_securestorage.h"
#include <tomcrypt.h>

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define TEE_DTCP_MAX_HDD_DATA_SIZE          (2*192*1024)
#define TEE_DTCP_LOCAL_KEY_SIZE             (16)
#define DWORDSWAP(u4Tmp) (((u4Tmp & 0xff) << 24) | ((u4Tmp & 0xff00) << 8) | ((u4Tmp & 0xff0000) >> 8) | ((u4Tmp & 0xff000000) >> 24))

//-----------------------------------------------------------------------------
// Extern Functions and Variable
//-----------------------------------------------------------------------------
extern TEE_SECURESTORAGE_USERKEY_T t_TEE_SS_SecretUserKey;
extern BYTE au1_TEE_SS_ksec[TEE_SECURESTORAGE_SEC_KEY_SIZE];
extern BYTE au1_TEE_SS_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE];

extern BOOL RSADecryption(UINT32 *pu4Signature, UINT32 *pu4PublicKey, UINT32 *pu4CheckSum, UINT32 e);
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT8 au1Kdist[] = {0x27, 0x5D, 0x2A, 0x16, 0x70, 0x5E, 0xE1, 0x28, 0x66, 0xFF, 0x9E, 0x90, 0x24, 0xE3, 0x17, 0x38};
static UINT8 au1KdistIv[] = {0x5D, 0xD1, 0x86, 0xBB, 0x01, 0xA3, 0xB9, 0x83, 0x7B, 0xDE, 0x5C, 0xFD, 0xC7, 0xB3, 0xC6, 0x82};
static UINT8 au1Kl[TEE_DTCP_NUM_LOCAL_KEY][TEE_DTCP_LOCAL_KEY_SIZE] = {
    {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f}
};

static UINT8 au1KlWorkIv[TEE_DTCP_NUM_LOCAL_KEY][16] = {
    {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}
};

static UINT8 au1KlIv[TEE_DTCP_NUM_LOCAL_KEY][16] =  {
    {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}
};

static UINT8 au1Km[] = {0x97, 0xC9, 0x6B, 0x63, 0x53, 0x16, 0x39, 0xD6, 0x59, 0xD7, 0xDF, 0x2E, 0xB5, 0x52, 0xA9, 0xA7};
static UINT8 au1KmIv[] = {0xC1, 0x0D, 0xA5, 0xE1, 0xCB, 0x39, 0xB0, 0x12, 0xEC, 0x6C, 0xC6, 0xE8, 0x3D, 0x41, 0xB1, 0x4D};

#if 0
static UINT8 au1RSAPublickey[] =
{
0xc3, 0x4e, 0xe0, 0xbc, 0x83, 0x1e, 0xcc, 0x42, 0x1b, 0x28, 0xb7, 0xf4, 0x76, 0xfa, 0x72, 0x68,
0x5a, 0x07, 0x54, 0xfb, 0xf0, 0x1a, 0x59, 0x79, 0x30, 0x51, 0x6e, 0xb2, 0xdd, 0xa4, 0x35, 0x2c,
0xc9, 0x53, 0xdc, 0xf9, 0x12, 0xf4, 0xea, 0x1d, 0x93, 0x8d, 0x55, 0x50, 0x46, 0x90, 0x29, 0xe7,
0x49, 0xec, 0x62, 0x67, 0x64, 0xd7, 0xb1, 0x92, 0x67, 0x12, 0x29, 0x48, 0x85, 0x40, 0x61, 0x4a,
0x1f, 0x03, 0x48, 0x49, 0x7d, 0x3b, 0xcb, 0x76, 0x6c, 0x83, 0x31, 0x9b, 0x49, 0xfa, 0x09, 0xef,
0x02, 0x6b, 0xae, 0xb5, 0x0f, 0x18, 0x02, 0x3c, 0xd4, 0xc6, 0x28, 0xea, 0x03, 0x0a, 0x2b, 0x97,
0x80, 0x57, 0x0c, 0xb6, 0xd2, 0x10, 0x28, 0xab, 0x04, 0x08, 0x98, 0x74, 0x63, 0x48, 0x82, 0x09,
0x74, 0x9d, 0xda, 0x80, 0x58, 0x76, 0xd0, 0x71, 0x2e, 0x50, 0x0c, 0x0d, 0xf4, 0x86, 0x20, 0x55,
0x99, 0x67, 0x91, 0x51, 0xb0, 0xd8, 0x75, 0x3d, 0x9c, 0x17, 0x7c, 0x33, 0xf1, 0xdf, 0xe4, 0xee,
0x68, 0x8d, 0xc3, 0x4d, 0xc8, 0xf8, 0x0d, 0xc4, 0x00, 0x67, 0xbb, 0x05, 0xd2, 0x20, 0xcc, 0x4e,
0x31, 0x09, 0x9f, 0x8e, 0x3b, 0xa3, 0x6f, 0x12, 0x33, 0x45, 0xd2, 0x46, 0x7a, 0x92, 0xfd, 0x33,
0x87, 0x7d, 0x23, 0x1d, 0x0f, 0x2a, 0x43, 0xe7, 0x6d, 0x12, 0x5b, 0x27, 0x0a, 0xda, 0x4c, 0x47,
0x60, 0x35, 0x9c, 0xb2, 0x52, 0xd1, 0x53, 0xc8, 0x1f, 0x54, 0x7f, 0x8e, 0x70, 0xd9, 0x5f, 0x62,
0x95, 0xfd, 0xf4, 0x89, 0xc1, 0x56, 0x6c, 0xa4, 0xb4, 0x41, 0x36, 0x5d, 0xa0, 0x1b, 0x14, 0xc1,
0x2a, 0x0e, 0x14, 0xf6, 0x63, 0x95, 0x73, 0x9d, 0x91, 0x78, 0x62, 0x7e, 0x00, 0x20, 0x30, 0x69,
0xbf, 0xb6, 0x3a, 0x86, 0x42, 0xf8, 0x06, 0x9f, 0x06, 0xbe, 0x57, 0x4b, 0x27, 0x0f, 0x17, 0xb5    
};
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void DataSwap(UINT32 *pu4Dest, UINT32 *pu4Src, UINT32 u4Size, UINT32 u4Mode)
{
     int i = 0;
     UINT32 u4Tmp;
 
     if(u4Mode == 0)
     {
         for(i =0; i < u4Size; i++) //memcpy
         {
             *(pu4Dest + i) = *(pu4Src + i);
         }
     }
     else if(u4Mode == 1) //Endien Change
     {
         for(i =0; i < u4Size; i++)
         {
             u4Tmp = *(pu4Src + i);
             u4Tmp = DWORDSWAP(u4Tmp);
             *(pu4Dest + i) = u4Tmp;
         }
     }
    else if(u4Mode == 2) //Head Swap
    {
        for(i =0; i < u4Size; i++)
        {
            *(pu4Dest + u4Size - 1 - i) = *(pu4Src + i);
        }
    }
    else if(u4Mode == 3) //Head Swap + Endien Change
    {
        for(i =0; i < u4Size; i++)
        {
            u4Tmp = *(pu4Src + i);
            u4Tmp = DWORDSWAP(u4Tmp);
            *(pu4Dest + u4Size - 1 - i) = u4Tmp;
        }
    }
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
UINT32 _teeDtcpEncryptHCI(UINT8 *pu1Hci, UINT32 u4Size, UINT8 *pu1EncHci)
{
    symmetric_CBC rCBC;

    if (!pu1Hci || !pu1EncHci)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (u4Size & 0xF)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    
    if (register_cipher(&aes_desc) < 0)
    {
        return TZ_ERROR_CRYPTO;
    }

    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &rCBC);
    cbc_encrypt(pu1Hci, pu1EncHci, u4Size, &rCBC);
    cbc_done(&rCBC);

    return TZ_SUCCESS;
}

UINT32 _teeDtcpDecryptHCI(UINT8 *pu1EncHci, UINT32 u4Size, UINT8 *pu1Hci)
{
    symmetric_CBC rCBC;

    if (!pu1Hci || !pu1EncHci)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (u4Size & 0xF)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    
    if (register_cipher(&aes_desc) < 0)
    {   
        return TZ_ERROR_CRYPTO;
    }

    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &rCBC);
    cbc_decrypt(pu1EncHci, pu1Hci, u4Size, &rCBC);
    cbc_done(&rCBC);

    return TZ_SUCCESS;
}


UINT32 _teeDtcpSwapEncryptedHCI(UINT8 *pu1EncDistHci, UINT32 u4Size, 
                                UINT8 *pu1Digest, UINT32 u4DigestLen, 
                                UINT8 *pu1EncHci)
{
    UINT8 au1HmacOut[TEE_CRYPTO_SIGNATURE_SIZE_IN_BYTES];
    UINT32 u4OutLen = u4DigestLen;
    UINT32 u4Result = TZ_SUCCESS;
    UINT8 *pu1Hci = NULL;
    symmetric_CBC rCBC;
    int i;

    if (!pu1EncDistHci || !pu1Digest || !pu1EncHci)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    if (u4Size & 0xF)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    if (register_cipher(&aes_desc) < 0)
    {   
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

    pu1Hci = (UINT8 *)malloc(u4Size);
    if (!pu1Hci)
    {
        DMSG("malloc failed.\n");
        u4Result = TZ_ERROR_MEMORY;
        goto lbExit1;
    }

    u4Result = _teeCryptoSignWithKL(pu1EncDistHci, u4Size, au1HmacOut);
    if (u4Result != TZ_SUCCESS)
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }
    
    if (memcmp(pu1Digest, au1HmacOut, u4DigestLen))
    {
        DMSG("fail to verify digest.\n");
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }
    
    cbc_start(find_cipher("aes"), au1KdistIv, au1Kdist, sizeof(au1Kdist), 0, &rCBC);
    cbc_decrypt(pu1EncDistHci, pu1Hci, u4Size, &rCBC);
    cbc_done(&rCBC);

    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &rCBC);
    cbc_encrypt(pu1Hci, pu1EncHci, u4Size, &rCBC);
    cbc_done(&rCBC);

lbExit1:
    if (pu1Hci)
    {
        free(pu1Hci);
    }

    return u4Result;
}

UINT32 _teeDtcpSetEncLocalKey(UINT32 u4KeyIdx,
                            UINT8 *pu1EncKey, UINT32 u4EncKeyLen, 
                            UINT8 *pu1Iv, UINT32 u4IvLen)
{
    UINT32 u4Result = TZ_SUCCESS;
    UINT32 i;
    symmetric_CBC rCBC;
    UINT32 au4Result[64];

    if (u4KeyIdx >= TEE_DTCP_NUM_LOCAL_KEY)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    if (!pu1EncKey || !pu1Iv)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }
    
    if ((u4EncKeyLen > TEE_DTCP_LOCAL_KEY_SIZE) || (u4IvLen > sizeof(au1KlIv[0])))
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    if (register_cipher(&aes_desc) < 0)
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

    memcpy_n2s_chk(au1KlIv[u4KeyIdx], pu1Iv, u4IvLen, NULL);
    memcpy_n2s_chk(au1KlWorkIv[u4KeyIdx], pu1Iv, u4IvLen, NULL);
    
    cbc_start(find_cipher("aes"), au1KmIv, au1Km, sizeof(au1Km), 0, &rCBC);
    cbc_decrypt(pu1EncKey, au1Kl[u4KeyIdx], u4EncKeyLen, &rCBC);
    cbc_done(&rCBC);

    dprintf("\n"); 
    for (i = 0; i < u4EncKeyLen; i++)
    {
        dprintf("0x%02x ", au1Kl[u4KeyIdx][i]); 
    }
    dprintf("\n"); 

    for (i = 0; i < u4IvLen; i++)
    {
        dprintf("0x%02x ", au1KlIv[u4KeyIdx][i]); 
    }
    dprintf("\n");

lbExit1:

    return u4Result;
}

UINT32 _teeDtcpSetLocalKeyToGCPU(UINT32 u4KeyIdx, UINT32 u4GcpuKeyPtr, UINT32 u4GcpuIvPtr)
{
    UINT32 u4Result = TZ_SUCCESS;
    
    if (u4KeyIdx >= TEE_DTCP_NUM_LOCAL_KEY)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }
    
    if (S_GCPU_OK != GCPU_SetSecureSlot(u4GcpuKeyPtr, au1Kl[u4KeyIdx], sizeof(au1Kl[u4KeyIdx])))
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

    if (S_GCPU_OK != GCPU_SetSecureSlot(u4GcpuIvPtr, au1KlIv[u4KeyIdx], sizeof(au1KlIv[u4KeyIdx])))
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }
    
lbExit1:

    return u4Result;
}

UINT32 _teeDtcpDecryptHddData(UINT8 *pu1Input, UINT32 u4Size, UINT8 *pu1Output,
                              UINT8 *pu1SecCode, UINT32 u4CodeSize) 
{
    UINT8 au1HmacOut[TEE_CRYPTO_SIGNATURE_SIZE_IN_BYTES] = {0};
    UINT8 au1Khdd[16];
    UINT32 u4OutLen = sizeof(au1HmacOut);
    UINT32 u4Result = TZ_SUCCESS;
    int i;

    if (!pu1Input || !pu1Output || !pu1SecCode)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    if (u4Size > TEE_DTCP_MAX_HDD_DATA_SIZE)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    /*
    Len  16    32
        +-----+-----------------------------+
        |Nonce|           Digest            |
        +-----+-----------------------------+
    */
    if (u4CodeSize != 48)
    {
        u4Result = TZ_ERROR_ILLEGAL_ARGUMENT;
        goto lbExit1;
    }

    if (!TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_SECURE_PVR_AES_0, DMX_KEY_IDX_SECURE_PVR, au1Khdd, sizeof(au1Khdd)))
    {
        u4Result = TZ_ERROR_ITEM_NOT_FOUND;
        goto lbExit1;
    }

    u4Result = _teeCryptoSignWithKL(pu1SecCode, 16, au1HmacOut);
    if (u4Result != TZ_SUCCESS)
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

    if (memcmp(&pu1SecCode[16], au1HmacOut, u4OutLen))
    {
        DMSG("fail to verify digest.\n");
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

    dprintf("%s - input=%x size=%u output=%x\n", __FUNCTION__, (UINT32)pu1Input, u4Size, (UINT32)pu1Output); 
    for (i = 0; i < 16; i++)
    {
        dprintf("0x%02x ", pu1Input[i]); 
    }
    dprintf("\n"); 

    if (!TZ_GCPU_DECRYPT_TS(au1Khdd, sizeof(au1Khdd), NULL,
                            pu1Input, pu1Output, u4Size, 
                            TSDESC_CIPHER_ECB, TRUE))
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

    for (i = 0; i < 16; i++)
    {
        dprintf("0x%02x ", pu1Output[i]); 
    }
    dprintf("\n"); 

lbExit1:

    return u4Result;
}

UINT32 _teeDtcpSwapEncryptedHddData(UINT8 *pu1Input, UINT32 u4Size, UINT8 *pu1Output, 
                                    UINT32 u4KeyIdx, BOOL fgReloadIv)
{
    UINT32 u4Result = TZ_SUCCESS;
    UINT32 u4AlignSize = TZ_CACHE_ALIGN(u4Size);
    UINT8 *pu1PlainTxt = NULL, *pu1PktBuf = NULL;
    UINT8 au1Khdd[16];
    int i;

    if (!pu1Input || !pu1Output)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if ((u4Size > TEE_DTCP_MAX_HDD_DATA_SIZE) || (u4Size & 0xF))
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (u4KeyIdx >= TEE_DTCP_NUM_LOCAL_KEY)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    } 
 
    if (!TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_SECURE_PVR_AES_0, DMX_KEY_IDX_SECURE_PVR, au1Khdd, sizeof(au1Khdd)))
    {
        return TZ_ERROR_ITEM_NOT_FOUND;
    }

    pu1PktBuf = (UINT8 *)malloc(u4AlignSize + TZ_CACHE_LINE_SIZE);
    if (!pu1PktBuf)
    {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

    pu1PlainTxt = TZ_CACHE_ALIGN((UINT32)pu1PktBuf);

    if (!TZ_GCPU_DECRYPT_TS(au1Khdd, sizeof(au1Khdd), NULL,
                            pu1Input, pu1PlainTxt, u4Size, 
                            TSDESC_CIPHER_ECB, TRUE)) 
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }

#if 1
    if (fgReloadIv)
    {
        memcpy_s2s_chk(au1KlWorkIv[u4KeyIdx], au1KlIv[u4KeyIdx], sizeof(au1KlWorkIv[u4KeyIdx]), NULL);
    }
 
    if (!TZ_GCPU_AES_CBC(au1Kl[u4KeyIdx], TEE_DTCP_LOCAL_KEY_SIZE, 
                        au1KlWorkIv[u4KeyIdx], au1KlWorkIv[u4KeyIdx],
                        pu1PlainTxt, pu1Output, u4Size, TRUE)) 
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }
#else
    if (TZ_GCPU_SECURE_AES_CBC(AES_TZ_SECURE_KEY_PTR, 16, AES_TZ_SECURE_IV_PTR,
                                pu1PlainTxt, pu1Output, u4Size, TRUE))
    {
        u4Result = TZ_ERROR_CRYPTO;
        goto lbExit1;
    }
#endif

lbExit1:
    if (pu1PktBuf)
    {
        free(pu1PktBuf);
    }

    return u4Result;
}

