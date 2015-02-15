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
#include "x_chipreg.h"
#include <tomcrypt.h>

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define REG_EFUSE_SID_0  (0x678)
#define REG_EFUSE_SID_1  (0x67c)
#define REG_EFUSE_SID_2  (0x680)
#define REG_EFUSE_SID_3  (0x684)
#define SECURE_ID_LEN    (16)
#define REG_EFUSE_KEY    (0x688)

#define TEE_DUMP(x,y,z) Printf("\n[TEE_DUMP]%s:", (x)); \
    for(i = 0; i < (y); i++) { \
        if(i % 16 == 0) \
            Printf("\n"); \
        Printf("0x%02x ", (z)[i]); \
    } \
    Printf("\n\n");

/* ============================================================================
    Global Variable
============================================================================ */
TEE_SECURESTORAGE_USERKEY_T t_TEE_SS_SecretUserKey = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
BOOL b_TEE_SS_Init = FALSE;
TEE_SECURESTORAGE_USERKEY_T t_TEE_SS_CurUserKey = {{0}};
BYTE au1_TEE_SS_ksec[TEE_SECURESTORAGE_SEC_KEY_SIZE] = {0};
BYTE au1_TEE_SS_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE] = {0};
BYTE au1_TEE_SS_khsec[TEE_SECURESTORAGE_HASH_KEY_SIZE] = {0};

BOOL b_TEE_SSV1_Init = FALSE;
BYTE au1_TEE_SSV1_ksec[TEE_SECURESTORAGE_SEC_KEY_SIZE] = {0};
BYTE au1_TEE_SSV1_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE] = {0};
BYTE au1_TEE_SSV1_khsec[TEE_SECURESTORAGEV1_HASH_KEY_SIZE] = {0};

/* ============================================================================
    Internal Functions
============================================================================ */
UINT32 _teeSecureStorageInit(TEE_SECURESTORAGE_USERKEY_T *ptUserKey) {
    
    hash_state t_hash_state;
    BYTE au1_seed[TEE_SECURESTORAGE_SEED_SIZE];
    BYTE au1_md[SHA512_DIGEST_LENGTH];
    UINT32 au4SID[4];
    int i4_offset = 0;//, i = 0;
    
    /* Check and initial libtomcrypt library */
    if(find_cipher("aes") == -1) {
        register_cipher(&aes_desc);
    }
    if(find_hash("sha1") == -1) {
        register_hash(&sha1_desc);
    }
    if(find_hash("sha256") == -1) {
        register_hash(&sha256_desc);
    }
    if(find_hash("sha384") == -1) {
        register_hash(&sha384_desc);
    }
    if(find_hash("sha512") == -1) {
        register_hash(&sha512_desc);
    }

    if(memcmp(&t_TEE_SS_CurUserKey, ptUserKey, sizeof(TEE_SECURESTORAGE_USERKEY_T)) || b_TEE_SS_Init == FALSE) {
        memset(au1_seed, 0, TEE_SECURESTORAGE_SEED_SIZE);
        
        /* Get ROM data & ECN ID */
        BIM_WRITE32(REG_EFUSE_KEY, 0x883);
        BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

        au4SID[1] = BIM_READ32(REG_EFUSE_SID_1);
        au4SID[3] = BIM_READ32(REG_EFUSE_SID_3);
        au4SID[0] = BIM_READ32(REG_EFUSE_SID_0);
        au4SID[2] = BIM_READ32(REG_EFUSE_SID_2);

        memcpy(au1_seed + i4_offset, ptUserKey->au1EncKey, 4);
        i4_offset += 4;
        memcpy_s2s_chk(au1_seed + i4_offset, &au4SID[1], 4, NULL);
        i4_offset += 4;
        memcpy_n2s_chk(au1_seed + i4_offset, (VOID *)TEE_ROMDATA_START_ADDR, 64, NULL);
        i4_offset += 64;
        memcpy(au1_seed + i4_offset, ptUserKey->au1EncKey + 4, 4);
        i4_offset += 4;
        memcpy_s2s_chk(au1_seed + i4_offset, &au4SID[3], 4, NULL);
        i4_offset += 4;
        memcpy_n2s_chk(au1_seed + i4_offset, (VOID *)TEE_ROMDATA_START_ADDR + 64, 64, NULL);
        i4_offset += 64;
        memcpy(au1_seed + i4_offset, ptUserKey->au1EncKey + 8, 4);
        i4_offset += 4;
        memcpy_s2s_chk(au1_seed + i4_offset, &au4SID[0], 4, NULL);
        i4_offset += 4;
        memcpy_n2s_chk(au1_seed + i4_offset, (VOID *)TEE_ROMDATA_START_ADDR + 128, 64, NULL);
        i4_offset += 64;
        memcpy(au1_seed + i4_offset, ptUserKey->au1EncKey + 12, 4);
        i4_offset += 4;
        memcpy_s2s_chk(au1_seed + i4_offset, &au4SID[2], 4, NULL);
        i4_offset += 4;
        memcpy_n2s_chk(au1_seed + i4_offset, (VOID *)TEE_ROMDATA_START_ADDR + 192, 64, NULL);
        i4_offset += 64;

        /* Compute SHA-512 */
        sha512_init(&t_hash_state);
        sha512_process(&t_hash_state, au1_seed, TEE_SECURESTORAGE_SEED_SIZE);
        sha512_done(&t_hash_state, au1_md);

        /* Assign au1_TEE_SS_ksec, au1_TEE_SS_khsec, au1_TEE_SS_iv */
        /* first 16 bytes: security key */
        memcpy_s2s_chk(au1_TEE_SS_ksec, au1_md, TEE_SECURESTORAGE_SEC_KEY_SIZE, NULL);
        /* second 16 bytes: iv */
        memcpy_s2s_chk(au1_TEE_SS_iv, au1_md + TEE_SECURESTORAGE_SEC_KEY_SIZE, TEE_SECURESTORAGE_SEC_KEY_SIZE, NULL);	
        /* last 32 bytes: hash key */
        memcpy_s2s_chk(au1_TEE_SS_khsec, au1_md + TEE_SECURESTORAGE_SEC_KEY_SIZE * 2, TEE_SECURESTORAGE_HASH_KEY_SIZE, NULL);

        /* Assign userKey & b_TEE_SS_Init = TRUE */
        memcpy(&t_TEE_SS_CurUserKey, ptUserKey, sizeof(TEE_SECURESTORAGE_USERKEY_T));
        b_TEE_SS_Init = TRUE;

        //TEE_DUMP("au1_seed:", TEE_SECURESTORAGE_SEED_SIZE, au1_seed);
        //TEE_DUMP("au1_md:", SHA512_DIGEST_LENGTH, au1_md);
        //TEE_DUMP("au1_TEE_SS_ksec:", TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_TEE_SS_ksec);
        //TEE_DUMP("au1_TEE_SS_iv:", TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_TEE_SS_iv);
        //TEE_DUMP("au1_TEE_SS_khsec:", TEE_SECURESTORAGE_HASH_KEY_SIZE, au1_TEE_SS_khsec);
    }

    return TZ_SUCCESS;
}

UINT32 _SecureStorageV1Init() {
    
    hash_state t_hash_state;
    BYTE au1_seed[TEE_SECURESTORAGEV1_SEED_SIZE];
    BYTE au1_md[SHA384_DIGEST_LENGTH];
    UINT32 au4SID[4];
    //int i = 0;
    
    if(b_TEE_SSV1_Init == TRUE) {
        return TZ_SUCCESS;
    }

    memset(au1_seed, 0, TEE_SECURESTORAGEV1_SEED_SIZE);

    /* Get ROM data & ECN ID */
    BIM_WRITE32(REG_EFUSE_KEY, 0x883);
    BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

    au4SID[1] = BIM_READ32(REG_EFUSE_SID_1);
    au4SID[3] = BIM_READ32(REG_EFUSE_SID_3);
    au4SID[0] = BIM_READ32(REG_EFUSE_SID_0);
    au4SID[2] = BIM_READ32(REG_EFUSE_SID_2);

    memcpy_s2s_chk(au1_seed, (void*)au4SID, TEE_SECURESTORAGEV1_ECNID_SIZE, NULL);
    memcpy_n2s_chk(au1_seed + TEE_SECURESTORAGEV1_ECNID_SIZE, (VOID *)TEE_ROMDATA_START_ADDR, TEE_SECURESTORAGEV1_ROMDATA_SIZE, NULL);

    /* Compute SHA-384 */
    sha384_init(&t_hash_state);
    sha384_process(&t_hash_state, au1_seed, TEE_SECURESTORAGEV1_SEED_SIZE);
    sha384_done(&t_hash_state, au1_md);

    /* Assign au1_TEE_SSV1_ksec, au1_TEE_SSV1_khsec, au1_TEE_SSV1_iv */
    /* first 16 bytes: security key */
	memcpy_s2s_chk(au1_TEE_SSV1_ksec, au1_md, TEE_SECURESTORAGE_SEC_KEY_SIZE, NULL);                                       
	/* 20 bytes starts from middle: hash key */
	memcpy_s2s_chk(au1_TEE_SSV1_khsec, au1_md + SHA384_DIGEST_LENGTH / 2, TEE_SECURESTORAGEV1_HASH_KEY_SIZE, NULL); 
	/* last 16 bytes: iv */
	memcpy_s2s_chk(au1_TEE_SSV1_iv, au1_md + SHA384_DIGEST_LENGTH - TEE_SECURESTORAGE_SEC_KEY_SIZE, TEE_SECURESTORAGE_SEC_KEY_SIZE, NULL);	

    b_TEE_SSV1_Init = TRUE;

    //TEE_DUMP("au1_seed:", TEE_SECURESTORAGEV1_SEED_SIZE, au1_seed);
    //TEE_DUMP("au1_md:", SHA384_DIGEST_LENGTH, au1_md);
    //TEE_DUMP("au1_TEE_SSV1_ksec:", TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_TEE_SSV1_ksec);
    //TEE_DUMP("au1_TEE_SSV1_iv:", TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_TEE_SSV1_iv);
    //TEE_DUMP("au1_TEE_SSV1_khsec:", TEE_SECURESTORAGEV1_HASH_KEY_SIZE, au1_TEE_SSV1_khsec);

    return TZ_SUCCESS;
}


UINT32 _SecureStorageV1Dec(UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen)
{    
    symmetric_CBC t_sym_cbc;
    hmac_state t_hmac_state;
    BYTE *pu1_plaintext = NULL, u1_padding = 0;
    BYTE au1_digest[TEE_SECURESTORAGEV1_HASH_KEY_SIZE];
    UINT32 i = 0;

    DMSG("[Trustzone][_SecureStorageV1Dec]: Enter.\n");
    
    _SecureStorageV1Init();

    /* digest */
    i = TEE_SECURESTORAGEV1_HASH_KEY_SIZE;
    memset(au1_digest, 0, TEE_SECURESTORAGEV1_HASH_KEY_SIZE);
    hmac_init(&t_hmac_state, find_hash("sha1"), au1_TEE_SSV1_khsec, TEE_SECURESTORAGEV1_HASH_KEY_SIZE); 
    hmac_process(&t_hmac_state, pu1SrcBuf, lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE);
    hmac_done(&t_hmac_state, au1_digest, &i);
    
    /* check digest */
    if(memcmp(au1_digest, pu1SrcBuf + lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE, TEE_SECURESTORAGEV1_HASH_KEY_SIZE)) {
        DMSG("Digest verify fail.\n");
        return TZ_ERROR_SECURITY;
    }

    /* allocate */
    pu1_plaintext = (BYTE *)malloc(lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE);
    if(pu1_plaintext == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

    /* decrypt */
    memset(pu1_plaintext, 0, lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE);
    cbc_start(find_cipher("aes"), au1_TEE_SSV1_iv, au1_TEE_SSV1_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1SrcBuf, pu1_plaintext, lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    /* padding */
    u1_padding = pu1_plaintext[lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE - 1];
    for (i = 1; i < u1_padding; i++) {
        if(u1_padding != pu1_plaintext[lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE - 1 - i]) {
            DMSG("Padding verify fail.\n");
            free(pu1_plaintext);
            return TZ_ERROR_SECURITY;
        }
    }

    /* complete */
    *plDstLen = lSrcLen - TEE_SECURESTORAGEV1_HASH_KEY_SIZE - u1_padding;
    memcpy_s2n_chk(pu1DstBuf, pu1_plaintext, *plDstLen, NULL);
    
    /* free */
    free(pu1_plaintext);

    return TZ_SUCCESS;
}

UINT32 _teeSecureStorageV2Dec(TEE_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen)
{    
    symmetric_CBC t_sym_cbc;
    hmac_state t_hmac_state;
    BYTE *pu1_plaintext = NULL, u1_padding = 0;
    BYTE au1_digest[TEE_SECURESTORAGE_HASH_KEY_SIZE];
    UINT32 i = 0;

    DMSG("[Trustzone][_teeSecureStorageV2Dec]: Enter.\n");
    
    /* check the buffer size */
    if (lSrcLen > TEE_SECURESTORAGE_MAX_SIZE || *plDstLen > TEE_SECURESTORAGE_MAX_SIZE) {
        DMSG("lSrcLen or *plDstLen size exceed 2MB.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (lSrcLen < (TEE_SECURESTORAGE_HASH_KEY_SIZE + TEE_SECURESTORAGE_SEC_KEY_SIZE + TEE_SECURESTORAGE_HEADER_SIZE)) {
        DMSG("lSrcLen size sbould be >= %d.\n", TEE_SECURESTORAGE_HASH_KEY_SIZE + TEE_SECURESTORAGE_SEC_KEY_SIZE + TEE_SECURESTORAGE_HEADER_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    _teeSecureStorageInit(ptUserKey);

    /* check header */
    if(pu1SrcBuf[0] != 0x5e || pu1SrcBuf[1] != 0xc5 || pu1SrcBuf[2] != 0x70 || pu1SrcBuf[3] != 0x02) {
        DMSG("Header doesn't match. Try _SecureStorageV1Dec.\n");
        if (_SecureStorageV1Dec(pu1SrcBuf, lSrcLen, pu1DstBuf, plDstLen) == TZ_SUCCESS) {
            DMSG("_SecureStorageV1Dec Ok.\n");
            return TZ_SUCCESS;
        } else {
            DMSG("_SecureStorageV1Dec Fail.\n");
            return TZ_ERROR_ILLEGAL_ARGUMENT;
        }
    }

    /* digest */
    i = TEE_SECURESTORAGE_HASH_KEY_SIZE;
    memset(au1_digest, 0, TEE_SECURESTORAGE_HASH_KEY_SIZE);
    hmac_init(&t_hmac_state, find_hash("sha256"), au1_TEE_SS_khsec, TEE_SECURESTORAGE_HASH_KEY_SIZE);
    hmac_process(&t_hmac_state, pu1SrcBuf + TEE_SECURESTORAGE_HEADER_SIZE, lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE);
    hmac_done(&t_hmac_state, au1_digest, &i);
    
    /* check digest */
    if(memcmp(au1_digest, pu1SrcBuf + lSrcLen - TEE_SECURESTORAGE_HASH_KEY_SIZE, TEE_SECURESTORAGE_HASH_KEY_SIZE)) {
        DMSG("Digest verify fail. Try _SecureStorageV1Dec.\n");
        if (_SecureStorageV1Dec(pu1SrcBuf, lSrcLen, pu1DstBuf, plDstLen) == TZ_SUCCESS) {
            DMSG("_SecureStorageV1Dec Ok.\n");
            return TZ_SUCCESS;
        } else {
            DMSG("_SecureStorageV1Dec Fail.\n");
            return TZ_ERROR_SECURITY;
        }
    }

    /* allocate */
    pu1_plaintext = (BYTE *)malloc(lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE);
    if(pu1_plaintext == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

    /* decrypt */
    memset(pu1_plaintext, 0, lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE);
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1SrcBuf + TEE_SECURESTORAGE_HEADER_SIZE, pu1_plaintext, lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    /* padding */
    u1_padding = pu1_plaintext[lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE - 1];
    for (i = 1; i < u1_padding; i++) {
        if(u1_padding != pu1_plaintext[lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE - 1 - i]) {
            DMSG("Padding verify fail. Try _SecureStorageV1Dec.\n");
            free(pu1_plaintext);
            if (_SecureStorageV1Dec(pu1SrcBuf, lSrcLen, pu1DstBuf, plDstLen) == TZ_SUCCESS) {
                DMSG("_SecureStorageV1Dec Ok.\n");
                return TZ_SUCCESS;
            } else {
                DMSG("_SecureStorageV1Dec Fail.\n");
                return TZ_ERROR_SECURITY;
            }
        }
    }

    /* complete */
    *plDstLen = lSrcLen - TEE_SECURESTORAGE_HEADER_SIZE - TEE_SECURESTORAGE_HASH_KEY_SIZE - u1_padding;
    if (memcmp(ptUserKey, &t_TEE_SS_SecretUserKey, sizeof(TEE_SECURESTORAGE_USERKEY_T)) == 0) {
        memcpy(pu1DstBuf, pu1_plaintext, *plDstLen);
    } else {
        memcpy_s2n_chk(pu1DstBuf, pu1_plaintext, *plDstLen, NULL);
    }
    
    /* free */
    free(pu1_plaintext);

    return TZ_SUCCESS;
}

/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 _teeSecureStorageEnc(TEE_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen)
{    

    symmetric_CBC t_sym_cbc;
    hmac_state t_hmac_state;
    BYTE *pu1_cipher = NULL, u1_padding = 0;
    BYTE au1_padding[TEE_SECURESTORAGE_SEC_KEY_SIZE];
    BYTE au1_digest[TEE_SECURESTORAGE_HASH_KEY_SIZE];
    UINT32 ui4_plaintext_offset = 0, i = 0;

    DMSG("[Trustzone][_teeSecureStorageEnc]: Enter.\n");

    _teeSecureStorageInit(ptUserKey);

    /* check the buffer size */
    if (lSrcLen > TEE_SECURESTORAGE_MAX_SIZE || *plDstLen > TEE_SECURESTORAGE_MAX_SIZE) {
        DMSG("lSrcLen or *plDstLen size exceed 2MB.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (*plDstLen < (lSrcLen + TEE_SECURESTORAGE_HASH_KEY_SIZE + TEE_SECURESTORAGE_SEC_KEY_SIZE + TEE_SECURESTORAGE_HEADER_SIZE)) {
        *plDstLen = lSrcLen + TEE_SECURESTORAGE_HASH_KEY_SIZE + TEE_SECURESTORAGE_SEC_KEY_SIZE + TEE_SECURESTORAGE_HEADER_SIZE;
        DMSG("*plDstLen size sbould be >= %d.\n", *plDstLen);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* padding */
    u1_padding = TEE_SECURESTORAGE_SEC_KEY_SIZE - (lSrcLen % TEE_SECURESTORAGE_SEC_KEY_SIZE);
    ui4_plaintext_offset = lSrcLen / TEE_SECURESTORAGE_SEC_KEY_SIZE * TEE_SECURESTORAGE_SEC_KEY_SIZE;
    memset(au1_padding, u1_padding, TEE_SECURESTORAGE_SEC_KEY_SIZE);
    if (memcmp(ptUserKey, &t_TEE_SS_SecretUserKey, sizeof(TEE_SECURESTORAGE_USERKEY_T)) == 0) {
        memcpy(au1_padding, pu1SrcBuf + ui4_plaintext_offset, lSrcLen % TEE_SECURESTORAGE_SEC_KEY_SIZE);
    } else {
        memcpy_n2s_chk(au1_padding, pu1SrcBuf + ui4_plaintext_offset, lSrcLen % TEE_SECURESTORAGE_SEC_KEY_SIZE, NULL);
    }
    
    /* allocate */
    pu1_cipher = (BYTE *)malloc(ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE);
    if(pu1_cipher == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

    /* encrypt */
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_encrypt(pu1SrcBuf, pu1_cipher, ui4_plaintext_offset, &t_sym_cbc);
    cbc_encrypt(au1_padding, pu1_cipher + ui4_plaintext_offset, TEE_SECURESTORAGE_SEC_KEY_SIZE, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    /* digest */
    i = TEE_SECURESTORAGE_HASH_KEY_SIZE;
    memset(au1_digest, 0, TEE_SECURESTORAGE_HASH_KEY_SIZE);
    hmac_init(&t_hmac_state, find_hash("sha256"), au1_TEE_SS_khsec, TEE_SECURESTORAGE_HASH_KEY_SIZE);
    hmac_process(&t_hmac_state, pu1_cipher, ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE);
    hmac_done(&t_hmac_state, au1_digest, &i);
    
    /* complete */
    pu1DstBuf[0] = 0x5e;
    pu1DstBuf[1] = 0xc5;
    pu1DstBuf[2] = 0x70;
    pu1DstBuf[3] = 0x02;
    if (memcmp(ptUserKey, &t_TEE_SS_SecretUserKey, sizeof(TEE_SECURESTORAGE_USERKEY_T)) == 0) {
        memcpy(pu1DstBuf + TEE_SECURESTORAGE_HEADER_SIZE, pu1_cipher, ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE);
        memcpy(pu1DstBuf + TEE_SECURESTORAGE_HEADER_SIZE + ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_digest, TEE_SECURESTORAGE_HASH_KEY_SIZE);
    } else {
        memcpy_s2n_chk(pu1DstBuf + TEE_SECURESTORAGE_HEADER_SIZE, pu1_cipher, ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE, NULL);
        memcpy_s2n_chk(pu1DstBuf + TEE_SECURESTORAGE_HEADER_SIZE + ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_digest, TEE_SECURESTORAGE_HASH_KEY_SIZE, NULL);
    }
    *plDstLen = TEE_SECURESTORAGE_HEADER_SIZE + ui4_plaintext_offset + TEE_SECURESTORAGE_SEC_KEY_SIZE + TEE_SECURESTORAGE_HASH_KEY_SIZE;

    /* free */
    free(pu1_cipher);

    return TZ_SUCCESS;

}

UINT32 _teeSecureStorageSecretEnc(UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen)
{    
    
    DMSG("[Trustzone][_teeSecureStorageSecretEnc]: Enter.\n");
    
    return _teeSecureStorageEnc(&t_TEE_SS_SecretUserKey, pu1SrcBuf, lSrcLen, pu1DstBuf, plDstLen);

}

UINT32 _teeSecureStorageDec(TEE_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen)
{    

    UINT32 ret = 0;

    DMSG("[Trustzone][_teeSecureStorageDec]: Enter.\n");
    
    if (memcmp(ptUserKey, &t_TEE_SS_SecretUserKey, sizeof(TEE_SECURESTORAGE_USERKEY_T) - 1) == 0) {
        DMSG("_teeSecureStorageDec ptUserKey not support.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    ret = _teeSecureStorageV2Dec(ptUserKey, pu1SrcBuf, lSrcLen, pu1DstBuf, plDstLen);

    return ret;

}

UINT32 _teeSecureStorageSecretDec(UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen)
{    
    
    DMSG("[Trustzone][_teeSecureStorageSecretDec]: Enter.\n");
    
    return _teeSecureStorageV2Dec(&t_TEE_SS_SecretUserKey, pu1SrcBuf, lSrcLen, pu1DstBuf, plDstLen);

}

