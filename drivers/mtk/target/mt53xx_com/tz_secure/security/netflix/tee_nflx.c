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
#include "tee_nflx.h"
#include <tomcrypt.h>

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TEE_NFLX_KPE_SIZE   (16)
#define TEE_NFLX_KPH_SIZE   (32)
#define TEE_NFLX_DH_SIZE    (128)
#define TEE_NFLX_ESN_SIZE    (43)

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
extern void TzGetRandomBytes(UINT8 *pu1_buf, UINT32 u4_len);
extern TEE_SECURESTORAGE_USERKEY_T t_TEE_SS_SecretUserKey;
extern BYTE au1_TEE_SS_ksec[TEE_SECURESTORAGE_SEC_KEY_SIZE];
extern BYTE au1_TEE_SS_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE];

/* ============================================================================
    Global Variable
============================================================================ */
TEE_SECURESTORAGE_USERKEY_T t_TEE_NFLX_SecretUserKey = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01}};
BYTE au1_TEE_NFLX_kpe[TEE_NFLX_KPE_SIZE] = {0};
BYTE au1_TEE_NFLX_kph[TEE_NFLX_KPH_SIZE] = {0};
static BYTE au1_TEE_NFLX_DH_p[]={
    0x96,0x94,0xE9,0xD8,0xD9,0x3A,0x5A,0xC7,0x4C,0x50,0x9B,0x4B,
    0xBC,0xE8,0x5E,0x92,0x13,0x2C,0xD1,0x9C,0xCE,0x47,0x7D,0x1A,
    0x7E,0x47,0xD5,0x27,0xD9,0xEC,0x29,0x15,0x15,0xF0,0xB8,0xB3,
    0xE1,0xEA,0xED,0x50,0x06,0xE1,0xB1,0xB9,0x1E,0xA2,0x5B,0x91,
    0xA0,0x1B,0x10,0xE2,0xE8,0x34,0xB8,0xD6,0x60,0xB2,0xE3,0x21,
    0xAD,0x64,0x4C,0xE1,0xA8,0x3B,0x32,0x8D,0x90,0x14,0xEE,0x7E,
    0x16,0xF1,0xE4,0x4F,0xFE,0x89,0x57,0x9A,0xC3,0xEE,0x47,0xD6,
    0x68,0xB6,0xB7,0x66,0x87,0xC2,0xFE,0x90,0xA3,0x5B,0x5E,0x60,
    0x28,0xFD,0x04,0xEF,0xEA,0x88,0x23,0x73,0xEC,0xF6,0x0B,0xA2,
    0xF6,0x37,0xE4,0xCD,0xAA,0x1B,0x60,0x89,0xD6,0xC0,0xB5,0x61,
    0xA8,0xE5,0x20,0xE7,0x96,0xDE,0x27,0xDF
};
static BYTE au1_TEE_NFLX_DH_g[]={0x05};
BYTE au1_TEE_NFLX_DH_priv[TEE_NFLX_DH_SIZE] = {0};
/* ============================================================================
    Internal Functions
============================================================================ */
UINT32 _teeNflxProtect(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_NFLX_SecretUserKey); 
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_encrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;

}

UINT32 _teeNflxUnprotect(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_NFLX_SecretUserKey); 
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;

}

/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 _teeNflxBaseUT()
{
    return TZ_SUCCESS;
}

UINT32 _teeNflxReadIdfile(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, UINT32 *pu4DstLen)
{
    BYTE au1_plaintext[TEE_NFLX_IDFILE_MAX_SIZE];
    BYTE au1_decodeBuf[TEE_NFLX_IDFILE_MAX_SIZE];
    BYTE *pu1_esn = NULL;
    BYTE *pu1_kpe = NULL;
    BYTE *pu1_kph = NULL;
    BYTE *pu1_tmp = NULL;
    long l_plainSize = 0;
    UINT32 i = 0;

    DMSG("[Trustzone][_teeNflxReadIdfile]: Enter.\n");

    /* Check the idfile size */
    if(u4SrcLen < 0 || u4SrcLen > TEE_NFLX_IDFILE_MAX_SIZE) {
        *pu4DstLen = 0;
        DMSG("idfile size exceed %d bytes.\n", TEE_NFLX_IDFILE_MAX_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Initial au1_plaintext array*/
    memset(au1_plaintext, 0, TEE_NFLX_IDFILE_MAX_SIZE);
    l_plainSize = u4SrcLen;
    
    /* Check header and decrypt */
    if(pu1SrcBuf[0] == 0xa2 && pu1SrcBuf[1] == 0x5e && pu1SrcBuf[2] == 0xc5 && pu1SrcBuf[3] == 0x70) {
        DMSG("Header match A2 pre-encrypted format. Decrypt it..\n");
        _teeDDIPreEncA2Dec(pu1SrcBuf, u4SrcLen, au1_plaintext, (UINT32 *)&l_plainSize);
        if(l_plainSize == 0) {
            DMSG("Decrypt error.\n");
            *pu4DstLen = 0;
            return TZ_ERROR_SECURITY;
        }
    } else {
        DMSG("Header doesn't match pre-encrypted format. Try _teeSecureStorageSecretDec..\n");
        /* Decrypt by secure storage API */
        l_plainSize = u4SrcLen;
        if(_teeSecureStorageSecretDec(pu1SrcBuf, u4SrcLen, au1_plaintext, &l_plainSize) != TZ_SUCCESS) {
            DMSG("Decrypt error.\n");
            *pu4DstLen = 0;
            return TZ_ERROR_SECURITY;
        }
    }
    
    /* Tokenize idfile */
    pu1_esn = au1_plaintext;
    pu1_tmp = pu1_esn;
    while(pu1_tmp && *pu1_tmp != '\0') {
        if(*pu1_tmp == '\r' || *pu1_tmp == '\n') {
            if(*(pu1_tmp+1) == '\r' || *(pu1_tmp+1) == '\n') {
                pu1_kpe = pu1_tmp + 2;
            } else {
                pu1_kpe = pu1_tmp + 1;
            }
            *pu1_tmp = '\0';
            break;
        }
        pu1_tmp++;
    }
    pu1_tmp = pu1_kpe;
    while(pu1_tmp && *pu1_tmp != '\0') {
        if(*pu1_tmp == '\r' || *pu1_tmp == '\n') {
            if(*(pu1_tmp+1) == '\r' || *(pu1_tmp+1) == '\n') {
                pu1_kph = pu1_tmp + 2;
            } else {
                pu1_kph = pu1_tmp + 1;
            }
            *pu1_tmp = '\0';
            break;
        }
        pu1_tmp++;
    }
    pu1_tmp = pu1_kph;
    while(pu1_tmp && *pu1_tmp != '\0') {
        if(*pu1_tmp == '\r' || *pu1_tmp == '\n') {
            *pu1_tmp = '\0';
            break;
        }
        pu1_tmp++;
    }
    
    /* Kpe */
    if (pu1_kpe) {
        memset(au1_decodeBuf, 0, TEE_NFLX_IDFILE_MAX_SIZE);
        i = TEE_NFLX_IDFILE_MAX_SIZE;
        tom_base64_decode(pu1_kpe, strlen((const char *)pu1_kpe), au1_decodeBuf, &i);
        if(i != TEE_NFLX_KPE_SIZE) {
            DMSG("Decrypt kpe error.\n");
            return TZ_ERROR_SECURITY;
        }
		memcpy_s2s_chk((VOID*)au1_TEE_NFLX_kpe, (VOID*)au1_decodeBuf, TEE_NFLX_KPE_SIZE, NULL);
    }
    
    /* Kph */
    if (pu1_kph) {
        memset(au1_decodeBuf, 0, TEE_NFLX_IDFILE_MAX_SIZE);
        i = TEE_NFLX_IDFILE_MAX_SIZE;
        tom_base64_decode(pu1_kph, strlen((const char *)pu1_kph), au1_decodeBuf, &i);
        if(i != TEE_NFLX_KPH_SIZE) {
            DMSG("Decrypt kph error.\n");
            return TZ_ERROR_SECURITY;
        }
		memcpy_s2s_chk((VOID*)au1_TEE_NFLX_kph, (VOID*)au1_decodeBuf, TEE_NFLX_KPH_SIZE, NULL);
    }
    
    /* ESN */
    if (pu1_esn) {
        *pu4DstLen = strlen((const char *)pu1_esn) + 1;
        if(*pu4DstLen > u4DstLen || *pu4DstLen > TEE_NFLX_ESN_SIZE) {
            DMSG("u4DstLen size not enough.\n");
            *pu4DstLen = 0;
            return TZ_ERROR_MEMORY;
        }
		memcpy_s2n_chk((VOID*)pu1DstBuf, (VOID*)pu1_esn, *pu4DstLen, NULL);
    }

    return TZ_SUCCESS;

}

UINT32 _teeNflxCipherEncrypt(UINT8 *pu1KceKey, UINT32 u4KceLen, UINT8 *pu1Iv, UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, UINT32 *pu4DstLen)
{
    symmetric_CBC t_sym_cbc;
    BYTE *pu1_cipher = NULL, u1_padding = 0;
    BYTE au1_clear_kce[TEE_NFLX_KPE_SIZE];
    BYTE au1_padding[TEE_NFLX_KPE_SIZE];
    UINT32 ui4_plaintext_offset = 0;
    
    DMSG("[Trustzone][_teeNflxCipherEncrypt]: Enter.\n");

    /* padding */
    u1_padding = TEE_NFLX_KPE_SIZE - (u4SrcLen % TEE_NFLX_KPE_SIZE);
    ui4_plaintext_offset = u4SrcLen / TEE_NFLX_KPE_SIZE * TEE_NFLX_KPE_SIZE;
    memset(au1_padding, u1_padding, TEE_NFLX_KPE_SIZE);
    memcpy_n2s_chk((VOID*)au1_padding, (VOID*)(pu1SrcBuf + ui4_plaintext_offset), u4SrcLen % TEE_NFLX_KPE_SIZE, NULL);
    
    /* allocate */
    pu1_cipher = (BYTE *)malloc(ui4_plaintext_offset + TEE_NFLX_KPE_SIZE);
    if(pu1_cipher == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

    /* encrypt */
    memset(pu1_cipher, 0, ui4_plaintext_offset + TEE_NFLX_KPE_SIZE);
	if ((pu1KceKey != NULL) && (u4KceLen != TEE_NFLX_KPE_SIZE)) {
        DMSG("Kce Length not right.\n");
        /* free */
				if(pu1_cipher != NULL)
						free(pu1_cipher);
        return TZ_ERROR_MEMORY;
    }
		
    if (pu1KceKey != NULL) {
        /* decrypt kce first */
        if(_teeNflxUnprotect(pu1KceKey, au1_clear_kce, TEE_NFLX_KPE_SIZE) != TZ_SUCCESS) {
            DMSG("Unprotect pu1KceKey error.\n");
            /* free */
    				if(pu1_cipher != NULL)
    						free(pu1_cipher);
            return TZ_ERROR_CRYPTO;
        }
        cbc_start(find_cipher("aes"), pu1Iv, au1_clear_kce, TEE_NFLX_KPE_SIZE, 0, &t_sym_cbc);
    } else {
        cbc_start(find_cipher("aes"), pu1Iv, au1_TEE_NFLX_kpe, TEE_NFLX_KPE_SIZE, 0, &t_sym_cbc);
    }
    cbc_encrypt(pu1SrcBuf, pu1_cipher, ui4_plaintext_offset, &t_sym_cbc);
    cbc_encrypt(au1_padding, pu1_cipher + ui4_plaintext_offset, TEE_NFLX_KPE_SIZE, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    /* complete */
    *pu4DstLen = ui4_plaintext_offset + TEE_NFLX_KPE_SIZE;
    if(*pu4DstLen > u4DstLen) {
        DMSG("u4DstLen size not enough.\n");
        /* free */
				if(pu1_cipher != NULL)
						free(pu1_cipher);           
        return TZ_ERROR_MEMORY;
    }
    memcpy_s2n_chk((VOID*)pu1DstBuf, (VOID*)pu1_cipher, *pu4DstLen, NULL);

    /* free */
    free(pu1_cipher);

    return TZ_SUCCESS;
}

UINT32 _teeNflxCipherDecrypt(UINT8 *pu1KceKey, UINT32 u4KceLen, UINT8 *pu1Iv, UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, UINT32 *pu4DstLen)
{
    symmetric_CBC t_sym_cbc;
    BYTE au1_clear_kce[TEE_NFLX_KPE_SIZE];
    BYTE *pu1_plaintext = NULL, u1_padding = 0;
    UINT32 i = 0;

    DMSG("[Trustzone][_teeNflxCipherDecrypt]: Enter.\n");

    /* allocate */
    pu1_plaintext = (BYTE *)malloc(u4SrcLen);
    if(pu1_plaintext == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

    /* decrypt */
    memset(pu1_plaintext, 0, u4SrcLen);
	if ((pu1KceKey != NULL) && (u4KceLen != TEE_NFLX_KPE_SIZE)) {
        DMSG("Kce Length not right.\n");
				/* free */
				if(pu1_plaintext != NULL)
						free(pu1_plaintext);            
        return TZ_ERROR_MEMORY;
    }
	
    if (pu1KceKey != NULL) {
        /* decrypt kce first */
        if(_teeNflxUnprotect(pu1KceKey, au1_clear_kce, TEE_NFLX_KPE_SIZE) != TZ_SUCCESS) {
            DMSG("Unprotect pu1KceKey error.\n");
						/* free */
    				if(pu1_plaintext != NULL)
    						free(pu1_plaintext);
            return TZ_ERROR_CRYPTO;
        }
        cbc_start(find_cipher("aes"), pu1Iv, au1_clear_kce, TEE_NFLX_KPE_SIZE, 0, &t_sym_cbc);
    } else {
        cbc_start(find_cipher("aes"), pu1Iv, au1_TEE_NFLX_kpe, TEE_NFLX_KPE_SIZE, 0, &t_sym_cbc);
    }
    cbc_decrypt(pu1SrcBuf, pu1_plaintext, u4SrcLen, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    /* padding */
    u1_padding = pu1_plaintext[u4SrcLen-1];
    for(i = 0; i < u1_padding; i++) {
        if(u1_padding != pu1_plaintext[u4SrcLen-1-i]) {
            DMSG("Padding verify fail.\n");
            *pu4DstLen = 0;
						/* free */
    				if(pu1_plaintext != NULL)
    						free(pu1_plaintext);
            return TZ_ERROR_SECURITY;
        }
    }
    
    /* complete */
    *pu4DstLen = u4SrcLen - u1_padding;
    if(*pu4DstLen > u4DstLen) {
        DMSG("u4DstLen size not enough.\n");
				/* free */
				if(pu1_plaintext != NULL)
						free(pu1_plaintext);            			
        return TZ_ERROR_MEMORY;
    }
    memcpy_s2n_chk((VOID*)pu1DstBuf, (VOID*)pu1_plaintext, *pu4DstLen, NULL);
    
    /* free */
    free(pu1_plaintext);

    return TZ_SUCCESS;
}

UINT32 _teeNflxHMAC(UINT8 *pu1KchKey, UINT32 u4KchLen, UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf)
{
    symmetric_CBC t_sym_cbc;
    hmac_state t_hmac_state;
    BYTE au1_clear_kch[TEE_NFLX_KPH_SIZE];
    unsigned long ul4_ret = TEE_NFLX_KPH_SIZE;

    DMSG("[Trustzone][_teeNflxHMAC]: Enter.\n");

	if ((pu1KchKey != NULL) && (u4KchLen != TEE_NFLX_KPH_SIZE)) {
        DMSG("Kch Length not right.\n");
        return TZ_ERROR_MEMORY;
    }

    if (pu1KchKey != NULL) {
        /* decrypt kch first */
        if(_teeNflxUnprotect(pu1KchKey, au1_clear_kch, TEE_NFLX_KPH_SIZE) != TZ_SUCCESS) {
            DMSG("Unprotect pu1KchKey error.\n");
            return TZ_ERROR_CRYPTO;
        }
        hmac_init(&t_hmac_state, find_hash("sha256"), au1_clear_kch, TEE_NFLX_KPH_SIZE);
    } else {
        hmac_init(&t_hmac_state, find_hash("sha256"), au1_TEE_NFLX_kph, TEE_NFLX_KPH_SIZE);
    }
    hmac_process(&t_hmac_state, pu1SrcBuf, u4SrcLen);
    hmac_done(&t_hmac_state, pu1DstBuf, &ul4_ret);

    return TZ_SUCCESS;
}

UINT32 _teeNflxDHPublicKey(UINT8 *pu1PubKey, UINT32 lPubKeyLen, UINT32 *plPubKeyLen, UINT8 *pu1PrivKey)
{
    symmetric_CBC t_sym_cbc;
    void *p, *g, *a, *A;
    
    DMSG("[Trustzone][_teeNflxDHPublicKey]: Enter.\n");

    /* Random DH private key */
    TzGetRandomBytes(au1_TEE_NFLX_DH_priv, TEE_NFLX_DH_SIZE);
    
    /* Init p, g, a */
    mp_init_multi(&p, &g, &a, &A, NULL);
    mp_read_unsigned_bin(p, au1_TEE_NFLX_DH_p, sizeof(au1_TEE_NFLX_DH_p));
    mp_read_unsigned_bin(g, au1_TEE_NFLX_DH_g, sizeof(au1_TEE_NFLX_DH_g));
    mp_read_unsigned_bin(a, au1_TEE_NFLX_DH_priv, TEE_NFLX_DH_SIZE);
    
    /* A = g^a mod p */
    mp_exptmod(g, a, p, A);
    *plPubKeyLen = mp_unsigned_bin_size(A);
    if(*plPubKeyLen > lPubKeyLen) {
        DMSG("lPubKeyLen size not enough.\n");
        return TZ_ERROR_MEMORY;
    }
    mp_to_unsigned_bin(A, pu1PubKey);
    mp_clear_multi(p, g, a, A, NULL);

    /* Encrypt private key in V2 */
    if (pu1PrivKey != NULL) {
        /* Encrypt kce & kch */
        if(_teeNflxProtect(au1_TEE_NFLX_DH_priv, pu1PrivKey, TEE_NFLX_DH_SIZE) != TZ_SUCCESS) {
            DMSG("Protect au1_TEE_NFLX_DH_priv error.\n");
            return TZ_ERROR_CRYPTO;
        }
    }

    return TZ_SUCCESS;
}

UINT32 _teeNflxDHSharedSecret(UINT8 *pu1PubKey, UINT32 u4PubLen, UINT8 *pu1KceKch, UINT8 *pu1PrivKey, UINT32 u4PrivLen)
{
    symmetric_CBC t_sym_cbc;
    hash_state t_hash_state;
    BYTE au1_buf[TEE_NFLX_DH_SIZE];
    BYTE au1_md[SHA384_DIGEST_LENGTH];
    void *p, *a, *B, *K;
    UINT32 u4_size = 0, u4_zero = 0;

    DMSG("[Trustzone][_teeNflxDHSharedSecret]: Enter.\n");
	
	if ((pu1PubKey == NULL) || (u4PubLen != TEE_NFLX_DH_SIZE) || (pu1KceKch == NULL) ||
	   (pu1PrivKey == NULL) || (u4PrivLen != TEE_NFLX_DH_SIZE)) {
        DMSG("Pointer or Length not right.\n");
        return TZ_ERROR_MEMORY;
    }

    /* Decrypt private key in V2 */
    if (pu1PrivKey != NULL) {
        /* Decrypt DH priv key */
        if(_teeNflxUnprotect(pu1PrivKey, au1_TEE_NFLX_DH_priv, TEE_NFLX_DH_SIZE) != TZ_SUCCESS) {
            DMSG("Unprotect pu1PrivKey error.\n");
            return TZ_ERROR_CRYPTO;
        }
    }

    /* Init p, g, a */
    mp_init_multi(&p, &a, &B, &K, NULL);
    mp_read_unsigned_bin(p, au1_TEE_NFLX_DH_p, sizeof(au1_TEE_NFLX_DH_p));
    mp_read_unsigned_bin(a, au1_TEE_NFLX_DH_priv, TEE_NFLX_DH_SIZE);
    mp_read_unsigned_bin(B, pu1PubKey, TEE_NFLX_DH_SIZE);
    
    /* K = B^a mod p */
    mp_exptmod(B, a, p, K);
    memset(au1_buf, 0, TEE_NFLX_DH_SIZE);
    u4_size = mp_unsigned_bin_size(K);
    mp_to_unsigned_bin(K, au1_buf);
    mp_clear_multi(p, a, B, K, NULL);

    /* Remove leading zero */
    while (u4_zero < u4_size && au1_buf[u4_zero] == 0x00)
        ++u4_zero;
    if (u4_zero != 0) {
        memmove_s2s_chk((VOID*)au1_buf, (VOID*)(au1_buf + u4_zero), u4_size - u4_zero, NULL);
        u4_size -= u4_zero;
    }

    /* SHA-384 */
    memset(au1_md, 0, SHA384_DIGEST_LENGTH);
    sha384_init(&t_hash_state);
    sha384_process(&t_hash_state, au1_buf, u4_size);
    sha384_done(&t_hash_state, au1_md);

    /* Encrypt kce & kch */
    if(_teeNflxProtect(au1_md, au1_buf, TEE_NFLX_KPE_SIZE) != TZ_SUCCESS) {
        DMSG("Protect kce error.\n");
        return TZ_ERROR_CRYPTO;
    }
    if(_teeNflxProtect(au1_md + TEE_NFLX_KPE_SIZE, au1_buf + TEE_NFLX_KPE_SIZE, TEE_NFLX_KPH_SIZE) != TZ_SUCCESS) {
        DMSG("Protect kch error.\n");
        return TZ_ERROR_CRYPTO;
    }

    memcpy_s2n_chk((VOID*)pu1KceKch, (VOID*)au1_buf, SHA384_DIGEST_LENGTH, NULL);
    return TZ_SUCCESS;
}

UINT32 _teeNflxGetRandomBytes(UINT8 *pu1Buf, UINT32 u4BufLen)
{

    DMSG("[Trustzone][_teeNflxGetRandomBytes]: Enter.\n");

    TzGetRandomBytes(pu1Buf, u4BufLen);

    return 0;

}

