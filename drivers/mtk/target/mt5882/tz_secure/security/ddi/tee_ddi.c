/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tee_securestorage.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/* ============================================================================
    Includes
============================================================================ */
#include "stdlib.h"
#include "string.h"

#include "tzst.h"
#include "svc_registry.h"
#include "tee_securestorage.h"
#include "tee_ddi.h"
#include <tomcrypt.h>

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TEE_DUMP(x,y,z) Printf("\n[TEE_DUMP]%s:", (x)); \
    for(i = 0; i < (y); i++) { \
        if(i % 16 == 0) \
            Printf("\n"); \
        Printf("0x%02x ", (z)[i]); \
    } \
    Printf("\n\n");

/* ============================================================================
    Extern Functions & Variable
============================================================================ */
extern BYTE au1_TEE_SS_ksec[TEE_SECURESTORAGE_SEC_KEY_SIZE];
extern BYTE au1_TEE_SS_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE];

/* ============================================================================
    Global Variable
============================================================================ */
BYTE au1_TEE_DDI_NormalKey[TEE_DDI_SEC_KEY_SIZE] = {0xea, 0xe6, 0xdd, 0xa2, 0xd3, 0x3e, 0x41, 0xd8, 0x72, 0x6e, 0xb9, 0x26, 0x00, 0x00, 0x00, 0x00};
BYTE au1_TEE_DDI_NommalIV[TEE_DDI_SEC_KEY_SIZE]  = {0xec, 0x16, 0x93, 0x67, 0x73, 0x17, 0x38, 0x48, 0xbc, 0x81, 0x1f, 0x58, 0x94, 0x71, 0x2b, 0xd8};
BYTE au1_TEE_DDI_SecretKey[TEE_DDI_SEC_KEY_SIZE] = {0x27, 0xdc, 0x32, 0xee, 0xc6, 0xab, 0x45, 0x21, 0x3e, 0xd1, 0xa7, 0xe4, 0x00, 0x00, 0x00, 0x00};
BYTE au1_TEE_DDI_SecretIV[TEE_DDI_SEC_KEY_SIZE]  = {0x16, 0x5e, 0xc5, 0x70, 0x78, 0xbf, 0xf8, 0x5f, 0x4c, 0xce, 0x9c, 0x80, 0x1f, 0x4a, 0x0d, 0xd3};

TEE_SECURESTORAGE_USERKEY_T t_TEE_DDI_SecretUserKeymain = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8a}};

/* ============================================================================
    Internal Functions
============================================================================ */


/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 _teeDDIPreEncA1Dec(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 *u4DstLen)
{
    symmetric_CBC t_sym_cbc;
    BYTE *pu1_plaintext = NULL, u1_padding = 0;
    UINT32 l_plainSize = 0, i = 0;
    
    DMSG("[Trustzone][_teeDDIPreEncA1Dec]: Enter.\n");

    /* check header */
    if(pu1SrcBuf[0] == 0xa1 && pu1SrcBuf[1] == 0x5e && pu1SrcBuf[2] == 0xc5 && pu1SrcBuf[3] == 0x70) {
        DMSG("Header match A1 format. Decrypt it..\n");

        /* allocate */
        pu1_plaintext = (BYTE *)malloc(u4SrcLen);
        if(pu1_plaintext == NULL) {
            DMSG("malloc failed.\n");
            return TZ_ERROR_MEMORY;
        }
        memset(pu1_plaintext, 0, u4SrcLen);

        /* Setup key */
        l_plainSize = u4SrcLen - 4;
        *((UINT32 *)(au1_TEE_DDI_NormalKey + 12)) = l_plainSize;
        
        /* decrypt */
        memset(pu1_plaintext, 0, u4SrcLen);
        register_cipher(&aes_desc);
        cbc_start(find_cipher("aes"), au1_TEE_DDI_NommalIV, au1_TEE_DDI_NormalKey, TEE_DDI_SEC_KEY_SIZE, 0, &t_sym_cbc);
        cbc_decrypt(pu1SrcBuf + 4, pu1_plaintext, l_plainSize, &t_sym_cbc);
        cbc_done(&t_sym_cbc);

        /* padding */
        u1_padding = pu1_plaintext[l_plainSize - 1];
        for (i = 1; i < u1_padding; i++) {
            if(u1_padding != pu1_plaintext[l_plainSize - 1 - i]) {
                DMSG("Padding verify fail.\n");
                *u4DstLen = 0;
                free(pu1_plaintext);
                return TZ_ERROR_SECURITY;
            }
        }
        *u4DstLen = l_plainSize - u1_padding;
        memcpy_s2n_chk((VOID*)pu1DstBuf, (VOID*)pu1_plaintext, *u4DstLen, NULL);
        free(pu1_plaintext);
    } else {
        *u4DstLen = 0;
        return TZ_ERROR_SECURITY;
    }

    return TZ_SUCCESS;
}

UINT32 _teeDDIPreEncA2Dec(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 *u4DstLen)
{
    symmetric_CBC t_sym_cbc;
    BYTE u1_padding = 0;
    UINT32 l_plainSize = 0, i = 0;
    
    DMSG("[Trustzone][_teeDDIPreEncA2Dec]: Enter.\n");

    /* check header */
    if(pu1SrcBuf[0] == 0xa2 && pu1SrcBuf[1] == 0x5e && pu1SrcBuf[2] == 0xc5 && pu1SrcBuf[3] == 0x70) {
        DMSG("Header match A2 format. Decrypt it..\n");
        /* Setup key */
        l_plainSize = u4SrcLen - 4;
        *((UINT32 *)(au1_TEE_DDI_SecretKey + 12)) = l_plainSize;
        
        /* decrypt */
        memset(pu1DstBuf, 0, u4SrcLen);
        register_cipher(&aes_desc);
        cbc_start(find_cipher("aes"), au1_TEE_DDI_SecretIV, au1_TEE_DDI_SecretKey, TEE_DDI_SEC_KEY_SIZE, 0, &t_sym_cbc);
        cbc_decrypt(pu1SrcBuf + 4, pu1DstBuf, l_plainSize, &t_sym_cbc);
        cbc_done(&t_sym_cbc);

        /* padding */
        u1_padding = pu1DstBuf[l_plainSize - 1];
        for (i = 1; i < u1_padding; i++) {
            if(u1_padding != pu1DstBuf[l_plainSize - 1 - i]) {
                DMSG("Padding verify fail.\n");
                *u4DstLen = 0;
                return TZ_ERROR_SECURITY;
            }
        }
        *u4DstLen = l_plainSize - u1_padding;
    } else {
        *u4DstLen = 0;
        return TZ_ERROR_SECURITY;
    }

    return TZ_SUCCESS;
}

UINT32 _teeDDISeStoreSign(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1HMACBuf)
{
    int hash;
    unsigned long outDigestLen;
    UINT32  ret = TZ_ERROR_SECURITY;
    UINT8   tzKey[SE_SECURE_STORE_HMAC_KEY_SIZE];
    UINT8   tzWrappedKey[SE_SECURE_STORE_HMAC_KEY_SIZE];
    UINT8   tzDigest[SE_SECURE_STORE_HMAC_VALUE_SIZE];

    NULL_CHECK(pu1SrcBuf);
    NULL_CHECK(pu1HMACBuf);

    DMSG("[Trustzone][_teeDDISeStoreSign]: Enter.\n");

    register_hash(&sha256_desc);

    if((hash = find_hash("sha256")) == -1){
        return TZ_ERROR_SECURITY;
    }
    
    memset(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET, 0, SE_SECURE_STORE_HMAC_KEY_SIZE);
    memset(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET+SE_SECURE_STORE_HMAC_KEY_SIZE, 0, SE_SECURE_STORE_HMAC_VALUE_SIZE);
    TzGetRandomBytes(tzKey, SE_SECURE_STORE_HMAC_KEY_SIZE);
    outDigestLen = SE_SECURE_STORE_HMAC_VALUE_SIZE;

    if(hmac_memory(hash, tzKey, SE_SECURE_STORE_HMAC_KEY_SIZE, pu1SrcBuf, u4SrcLen, tzDigest, &outDigestLen) == CRYPT_OK){
        _teeDDISeStoreEncHMACKey(tzKey, tzWrappedKey, SE_SECURE_STORE_HMAC_KEY_SIZE);
        memcpy_s2n_chk((VOID *)pu1HMACBuf, (VOID *)tzWrappedKey, SE_SECURE_STORE_HMAC_KEY_SIZE, NULL);
        memcpy_s2n_chk((VOID *)(pu1HMACBuf+SE_SECURE_STORE_HMAC_KEY_SIZE), (VOID *)tzDigest, SE_SECURE_STORE_HMAC_VALUE_SIZE, NULL);
        ret = TZ_SUCCESS;
    }

    return ret;
}

UINT32 _teeDDISeStoreVerify(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT32 *u4Result)
{
    int hash;
    unsigned long outDigestLen;
    UINT8   tzKey[SE_SECURE_STORE_HMAC_KEY_SIZE];
    UINT8   tzWrappedKey[SE_SECURE_STORE_HMAC_KEY_SIZE];
    UINT8   tzDigest[SE_SECURE_STORE_HMAC_VALUE_SIZE];

    NULL_CHECK(pu1SrcBuf);
    NULL_CHECK(u4Result);

    DMSG("[Trustzone][_teeDDISeStoreVerify]: Enter.\n");

    register_hash(&sha256_desc);

    if((hash = find_hash("sha256")) == -1){
        return TZ_ERROR_SECURITY;
    }

    memcpy_n2s_chk((VOID *)tzWrappedKey, (VOID *)(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET), SE_SECURE_STORE_HMAC_KEY_SIZE, NULL);
    memcpy_n2s_chk((VOID *)tzDigest, (VOID *)(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET+SE_SECURE_STORE_HMAC_KEY_SIZE), SE_SECURE_STORE_HMAC_VALUE_SIZE, NULL);
    memset(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET, 0, SE_SECURE_STORE_HMAC_KEY_SIZE);
    memset(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET+SE_SECURE_STORE_HMAC_KEY_SIZE, 0, SE_SECURE_STORE_HMAC_VALUE_SIZE);
    _teeDDISeStoreDecHMACKey(tzWrappedKey, tzKey, SE_SECURE_STORE_HMAC_KEY_SIZE);
    outDigestLen = SE_SECURE_STORE_HMAC_VALUE_SIZE;

    if (hmac_memory(hash, tzKey, SE_SECURE_STORE_HMAC_KEY_SIZE, pu1SrcBuf, u4SrcLen, pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET+SE_SECURE_STORE_HMAC_KEY_SIZE, &outDigestLen) == CRYPT_OK &&
        memcmp(pu1SrcBuf+SE_SECURE_STORE_HMAC_OFFSET+SE_SECURE_STORE_HMAC_KEY_SIZE, tzDigest, SE_SECURE_STORE_HMAC_VALUE_SIZE) == 0){
        *u4Result = TZ_SUCCESS;
    } else {
        *u4Result = TZ_ERROR_SECURITY;
    }

    return TZ_SUCCESS;
}

UINT32 _teeDDISeStoreEncHMACKey(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    DMSG("[Trustzone][_teeDDISeStoreEncHMACKey]: Enter.\n");

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_DDI_SecretUserKeymain);
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_encrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;
}

UINT32 _teeDDISeStoreDecHMACKey(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    DMSG("[Trustzone][_teeDDISeStoreDecHMACKey]: Enter.\n");

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_DDI_SecretUserKeymain);
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;
}
