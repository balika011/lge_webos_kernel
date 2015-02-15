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
//#include "tee_crypto.h"
#include "tee_playready.h"
#include "tee_ddi.h"
#include "tz_dmx_if.h"
#include "drmeccp256.h"
#include "certs/drmbcert.h"

#include <tomcrypt.h>
#define CC_TEE_FLOW_ENHANCEMENT
#define USE_LINUXTIME_INTEE
//#undef CC_TEE_STRICT_PROTECT_SUPPORT
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
typedef struct __TEE_KF_GUID {
    unsigned long Data1;
    unsigned short  Data2;
    unsigned short  Data3;
    unsigned char  Data4 [8];
} TEE_KF_GUID;

const TEE_KF_GUID TEE_guidKeyFileAESKeyID = 
	{
            0x0001D3BE,
            0xD26A,
            0x44F1,
            {
                0x9E, 0x42, 0xAD, 0xF6,
                0x63, 0xBF, 0x2B, 0x2C
          }
	};

const BYTE TEE_rgbKeyFileAESKey[16] = 
	{
            0x8B, 0x22, 0x2F, 0xFD, 0x1E, 0x76, 0x19, 0x56,
            0x59, 0xCF, 0x27, 0x03, 0x89, 0x8C, 0x42, 0x7F
    };

#define TEE_DUMP(x,y,z) Printf("\n[TEE_DUMP]%s:", (x)); \
    for(i = 0; i < (y); i++) { \
        if(i % 16 == 0) \
            Printf("\n"); \
        Printf("0x%02x ", (z)[i]); \
    } \
    Printf("\n\n");
#define TEE_PLAYREADY_SWBUF_SIZE (32)
#define TEE_PLAYREADY_HWBUF_SIZE (512 * 1024)

/* ============================================================================
    Extern Functions & Variable
============================================================================ */
extern void TzGetRandomBytes(UINT8 *pu1_buf, UINT32 u4_len);
#ifdef CC_ENABLE_HCI
extern BOOL _MtkAesDecryption(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size);
#endif
extern TEE_SECURESTORAGE_USERKEY_T t_TEE_SS_SecretUserKey;
extern BYTE au1_TEE_SS_ksec[TEE_SECURESTORAGE_SEC_KEY_SIZE];
extern BYTE au1_TEE_SS_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE];

/* ============================================================================
    Global Variable
============================================================================ */
BOOL b_TEE_PLAYREADY_DMXInit = FALSE;
TEE_SECURESTORAGE_USERKEY_T t_TEE_PlayReady_SecretUserKeymain = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x11}};
TEE_SECURESTORAGE_USERKEY_T t_TEE_PlayReady_SecretUserKeyminor = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x12}};
BYTE au1_TEE_PLAYREADY_ECCP256Key[ECC_P256_INTEGER_SIZE_IN_BYTES]   = {0};
BYTE au1_TEE_PLAYREADY_OMAC1Key[TEE_PLAYREADY_OMAC1_KEY_SIZE]       = {0};
BYTE au1_TEE_PLAYREADY_ContentKey[TEE_PLAYREADY_CONTENT_KEY_SIZE]   = {0};
BYTE au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE]    = {0};
BYTE au1_TEE_PLAYREADY_AESECBKey[TEE_PLAYREADY_AESECB_KEY_SIZE]     = {0};
BYTE au1_TEE_PLAYREADY_AESCBCKey[TEE_PLAYREADY_AESCBC_KEY_SIZE]     = {0};
BYTE *pu1_TEE_PLAYREADY_SwBuf = NULL;
BYTE *pu1_TEE_PLAYREADY_HwBuf = NULL;
BYTE *pu1_TEE_PLAYREADY_HwAlignBuf = NULL;
BYTE *pu1_TEE_PLAYREADY_ClearBuf = NULL;
BYTE au1_TEE_PLAYREADY_ECCP256GCKey[ECC_P256_INTEGER_SIZE_IN_BYTES];
BYTE au1_TEE_PLAYREADY_ECCP256Key_PubEnc[ECC_P256_POINT_SIZE_IN_BYTES];
BYTE au1_TEE_PLAYREADY_ECCP256Key_PubSign[ECC_P256_POINT_SIZE_IN_BYTES];
BYTE au1_TEE_PLAYREADY_ContentKey_Audio[TEE_PLAYREADY_CONTENT_KEY_SIZE]   = {0};

//UINT8 au1SecureIvTmp[16];
BYTE au1_TEE_PLAYREADY_ECCP256Key_PrivSign[ECC_P256_INTEGER_SIZE_IN_BYTES];

/* ============================================================================
    Internal Functions
============================================================================ */

UINT32 _teePlayReadyProtect_main(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
	symmetric_CBC t_sym_cbc;

	if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
		DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	}

	memset(pu1Dst, 0, u4Len);

	_teeSecureStorageInit(&t_TEE_PlayReady_SecretUserKeymain); 
	cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
	cbc_encrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
	cbc_done(&t_sym_cbc);

	return TZ_SUCCESS;

}

UINT32 _teePlayReadyUnprotect_main(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_PlayReady_SecretUserKeymain); 
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;

}

UINT32 _teePlayReadyProtect_minor(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_PlayReady_SecretUserKeyminor); 
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_encrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;

}

UINT32 _teePlayReadyUnprotect_minor(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len)
{
    symmetric_CBC t_sym_cbc;

    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    memset(pu1Dst, 0, u4Len);

    _teeSecureStorageInit(&t_TEE_PlayReady_SecretUserKeyminor); 
    cbc_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1Src, pu1Dst, u4Len, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    return TZ_SUCCESS;

}


/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 _teePlayReadyBaseUT(UINT8 *pu1Buf, UINT32 u4Len)
{
    symmetric_CTR t_sym_ctr;
    int i;    
#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    return TZ_ERROR_ILLEGAL_ARGUMENT;
#endif
    TEE_DUMP("before enc = ", u4Len, pu1Buf);
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_encrypt(pu1Buf, pu1Buf, u4Len, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
    TEE_DUMP("after enc = ", u4Len, pu1Buf);
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyCopyBytes(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromIdx, UINT32 u4FromSize, UINT32 u4Count)
{
    symmetric_CTR t_sym_ctr;
    UINT8 *pu1SrcBuf = NULL, *pu1DstBuf = NULL;
    
    DMSG("[Trustzone][_teePlayReadyCopyBytes]: Enter.\n");

    if (u4FromIdx + u4Count > u4FromSize) {
        DMSG("u4FromIdx + u4Count > u4FromSize.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#ifdef CC_TEE_FLOW_ENHANCEMENT	

	if (u4Count%TEE_SECURESTORAGE_SEC_KEY_SIZE != 0){
		DMSG("_teePlayReadyCopyBytes u4Count is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	}
#endif
    /* allocate */
    pu1SrcBuf = (BYTE *)malloc(u4FromSize);
    if(pu1SrcBuf == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }
    memset(pu1SrcBuf, 0, u4FromSize);

    pu1DstBuf = (BYTE *)malloc(u4Count);
    if(pu1DstBuf == NULL) {
        DMSG("malloc failed.\n");
        free(pu1SrcBuf);
        return TZ_ERROR_MEMORY;
    }
    memset(pu1DstBuf, 0, u4Count);
#ifndef CC_TEE_FLOW_ENHANCEMENT	

    /* unprotect Data */
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_decrypt(pu1From, pu1SrcBuf, u4FromSize, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(pu1From, pu1SrcBuf, u4FromSize);
#endif
    /* copy data from from to to */
    memcpy(pu1DstBuf, pu1SrcBuf + u4FromIdx, u4Count);
#ifndef CC_TEE_FLOW_ENHANCEMENT	

    /* protect Data */
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_encrypt(pu1DstBuf, pu1To, u4Count, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
 #else
	_teePlayReadyProtect_main(pu1DstBuf, pu1To, u4Count);
#endif   
    /* free */
    free(pu1SrcBuf);
    free(pu1DstBuf);

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyECCP256GenKeyPair(UINT8 *pu1PubKeyBuf, UINT8 * pub1PrivKeyBuf, BOOL bEncKeyPair)
{
    symmetric_CTR t_sym_ctr;
    UINT8 ProtectPrivKeyBuf[ECC_P256_INTEGER_SIZE_IN_BYTES];
    UINT32 ret = 0;
	
    DMSG("[Trustzone][_teePlayReadyECCP256GenKeyPair]: Enter.\n");

    if ((ret = CRYPTO_ECC_GenKeyPairRestrictedPriv_P256(pu1PubKeyBuf, ProtectPrivKeyBuf)))
    {
        DMSG("_teePlayReadyECCP256GenKeyPair fail!! Ret = %08x.\n", ret);
        return TZ_ERROR_CRYPTO;
    }
	
    if(bEncKeyPair == TRUE)
    {
        memcpy(au1_TEE_PLAYREADY_ECCP256Key_PubEnc, pu1PubKeyBuf, ECC_P256_POINT_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(au1_TEE_PLAYREADY_ECCP256Key_PubSign, pu1PubKeyBuf, ECC_P256_POINT_SIZE_IN_BYTES);
		memcpy(au1_TEE_PLAYREADY_ECCP256Key_PrivSign, ProtectPrivKeyBuf, ECC_P256_INTEGER_SIZE_IN_BYTES); // add to save private key			
    }
#ifdef DEBUG
{
    int i;
    Printf("au1_TEE_PLAYREADY_ECCP256Key_PubEnc\n");
    for (i = 0; i < ECC_P256_POINT_SIZE_IN_BYTES; i++)
            Printf( "[%x], ", au1_TEE_PLAYREADY_ECCP256Key_PubEnc[i]);
    Printf("\au1_TEE_PLAYREADY_ECCP256Key_PubSign\n");
    for (i = 0; i < ECC_P256_POINT_SIZE_IN_BYTES; i++)
            Printf( "[%x], ", au1_TEE_PLAYREADY_ECCP256Key_PubSign[i]);
    Printf("\n");
   }	
#endif
#ifndef CC_TEE_FLOW_ENHANCEMENT	

    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_encrypt(ProtectPrivKeyBuf, pub1PrivKeyBuf, ECC_P256_INTEGER_SIZE_IN_BYTES, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyProtect_main(ProtectPrivKeyBuf, pub1PrivKeyBuf, ECC_P256_INTEGER_SIZE_IN_BYTES);
#endif
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyECCP256SetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4KeyLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;

    DMSG("[Trustzone][_teePlayReadyECCP256SetKey]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
//#if 0 //add by xavier
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    if (u4KeyLen <  (u4BufIdx + ECC_P256_INTEGER_SIZE_IN_BYTES)) {
        DMSG("u4KeyLen != %d.\n", ECC_P256_INTEGER_SIZE_IN_BYTES);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (bProtect) {
        /* unprotect key */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_decrypt(pu1KeyBuf + u4BufIdx, au1_TEE_PLAYREADY_ECCP256Key, ECC_P256_INTEGER_SIZE_IN_BYTES, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(pu1KeyBuf + u4BufIdx, au1_TEE_PLAYREADY_ECCP256Key, ECC_P256_INTEGER_SIZE_IN_BYTES);
#endif
    } else {
        /* copy key */
        memcpy(au1_TEE_PLAYREADY_ECCP256Key, pu1KeyBuf + u4BufIdx, ECC_P256_INTEGER_SIZE_IN_BYTES);
    }

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyECDSAP256Sign(UINT8 *pu1Data, UINT32 u4DataLen, UINT8 * pu1Signature)
{

    PUBKEY_P256              IssuerKey         = {0};
    DRM_RESULT              dr = DRM_SUCCESS;
    DMSG("[Trustzone][_teePlayReadyECDSAP256Sign]: Enter.\n");
    BOOL bDeviceCert = FALSE;
    BOOL bEncKeyMatched = FALSE;
    BOOL bSignKeyMatched = FALSE;
	
#ifdef DEBUG	
{
    int i = 0;	
    Printf("au1_TEE_PLAYREADY_ECCP256Key_PubEnc\n");
    for (i = 0; i < ECC_P256_POINT_SIZE_IN_BYTES; i++)
            Printf( "[%x], ", au1_TEE_PLAYREADY_ECCP256Key_PubEnc[i]);
    Printf("\au1_TEE_PLAYREADY_ECCP256Key_PubSign\n");
    for (i = 0; i < ECC_P256_POINT_SIZE_IN_BYTES; i++)
            Printf( "[%x], ", au1_TEE_PLAYREADY_ECCP256Key_PubSign[i]);
    Printf("\n");
}
#endif

    /* check if pu1Data point to device cert */
    dr = DRM_BCert_GetPublicKeyByUsage( pu1Data,
                                          u4DataLen,
                                          0,
                                          DRM_BCERT_KEYUSAGE_ENCRYPT_KEY,
                                          &IssuerKey,
                                          NULL,
                                          NULL );

    if (dr == DRM_SUCCESS)
    { 
        bDeviceCert = TRUE;
        /* make sure that public enc key is gen from TZ */
        if (memcmp(IssuerKey.m_rgbPubkey, au1_TEE_PLAYREADY_ECCP256Key_PubEnc, 
			ECC_P256_POINT_SIZE_IN_BYTES) != 0)
        {
            int i;
            DMSG("au1_TEE_PLAYREADY_ECCP256Key_PubEnc incorrect !! \n");
            for (i = 0; i < ECC_P256_POINT_SIZE_IN_BYTES; i++)
                Printf( "[%x], ", IssuerKey.m_rgbPubkey[i]);
            Printf( "\n"); 	
            return   TZ_ERROR_CRYPTO;
        }
        bEncKeyMatched = TRUE;		
    }
    else
    {
        DMSG("DRM_BCert_GetPublicKeyByUsage DRM_BCERT_KEYUSAGE_ENCRYPT_KEY fail %x!! \n", dr);
    }	
    memset(&IssuerKey, 0, ECC_P256_POINT_SIZE_IN_BYTES);
    dr = DRM_BCert_GetPublicKeyByUsage( pu1Data,
                                          u4DataLen,
                                          0,
                                          DRM_BCERT_KEYUSAGE_SIGN,
                                          &IssuerKey,
                                          NULL,
                                          NULL );
	
    if (dr == DRM_SUCCESS)
    {
        bDeviceCert = TRUE;
        /* make sure that public sign key is gen from TZ */
        if (memcmp(IssuerKey.m_rgbPubkey, au1_TEE_PLAYREADY_ECCP256Key_PubSign, 
			ECC_P256_POINT_SIZE_IN_BYTES) != 0)
        {
            int i;        
            DMSG("au1_TEE_PLAYREADY_ECCP256Key_PubSign incorrect !! \n");
            for (i = 0; i < ECC_P256_POINT_SIZE_IN_BYTES; i++)
                Printf( "[%x], ", IssuerKey.m_rgbPubkey[i]);
            Printf( "\n"); 			
            return   TZ_ERROR_CRYPTO;
        }
        bSignKeyMatched = TRUE;
    }
    else
    {
        DMSG("DRM_BCert_GetPublicKeyByUsage DRM_BCERT_KEYUSAGE_SIGN fail %x!! \n", dr);
    }

    if((TRUE == bDeviceCert) && (bEncKeyMatched == TRUE) && (bSignKeyMatched == TRUE ))
    {
        DMSG("sign using GCKey!! \n");
        if (CRYPTO_ECDSA_Sign_P256(pu1Data, u4DataLen, au1_TEE_PLAYREADY_ECCP256GCKey, pu1Signature))
        {
            DMSG("CRYPTO_ECDSA_Sign_P256 fail!! \n");
            return TZ_ERROR_CRYPTO;
        }    	
	return TZ_SUCCESS;
    }
	
    if (CRYPTO_ECDSA_Sign_P256(pu1Data, u4DataLen, au1_TEE_PLAYREADY_ECCP256Key, pu1Signature))
    {
        DMSG("CRYPTO_ECDSA_Sign_P256 fail!! \n");
        return TZ_ERROR_CRYPTO;
    }    
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyECCP256Decrypt(UINT8 * pu1CipherText, UINT8 * pu1PlainText, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    UINT8 pu1ProtectPlainText[ECC_P256_INTEGER_SIZE_IN_BYTES];

    DMSG("[Trustzone][_teePlayReadyECCP256Decrypt]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
//#if 0 // add by xavier
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    memset(pu1ProtectPlainText, 0, ECC_P256_INTEGER_SIZE_IN_BYTES);
    if (CRYPTO_ECC_Decrypt_P256(au1_TEE_PLAYREADY_ECCP256Key, pu1CipherText, pu1ProtectPlainText))
    {
        DMSG("CRYPTO_ECC_Decrypt_P256 fail!! \n");
        return TZ_ERROR_CRYPTO;
    }

    if (bProtect) {
        /* protect key */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(pu1ProtectPlainText, pu1PlainText, ECC_P256_INTEGER_SIZE_IN_BYTES, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
		_teePlayReadyProtect_main(pu1ProtectPlainText, pu1PlainText, ECC_P256_INTEGER_SIZE_IN_BYTES);
#endif
    } else {
        /* copy key */
        memcpy(pu1PlainText, pu1ProtectPlainText, ECC_P256_INTEGER_SIZE_IN_BYTES);
    }
    
    return TZ_SUCCESS;    
}

//xavier add for test
UINT32 _teePlayReadyGetKFKEY(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, BOOL bProtect)
{   
    symmetric_CTR t_sym_ctr;
	symmetric_CBC t_sym_cbc;
    BYTE *pu1_plaintext = NULL;
    long l_plainSize = 0;

/* check the buffer size */
	if (u4SrcLen > TEE_SECURESTORAGE_MAX_SIZE || u4DstLen > TEE_SECURESTORAGE_MAX_SIZE) {
		DMSG("lSrcLen or *plDstLen size exceed 2MB.\n");
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	}


	if (u4DstLen < sizeof(TEE_KF_GUID)+ 16)
	   {
	   DMSG("u4DstLen size is too short.\n");
	   return TZ_ERROR_ILLEGAL_ARGUMENT;
	   }
	
    /* allocate */
    pu1_plaintext = (BYTE *)malloc(16);
    if(pu1_plaintext == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }
	memset(pu1_plaintext, 0, 16);
#ifndef CC_TEE_FLOW_ENHANCEMENT
    memcpy(pu1_plaintext,TEE_rgbKeyFileAESKey,16);
#else
	_teeSecureStorageInit(&t_TEE_PlayReady_SecretUserKeyminor); 
    memcpy(pu1_plaintext,(char*) &t_TEE_PlayReady_SecretUserKeyminor,16);
#endif
	
	memcpy(pu1DstBuf,(char*) &TEE_guidKeyFileAESKeyID,sizeof(TEE_KF_GUID));

	if (bProtect) {
#ifndef CC_TEE_FLOW_ENHANCEMENT			
		 _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
		 ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
		 ctr_encrypt(pu1_plaintext, pu1DstBuf+sizeof(TEE_KF_GUID), 16, &t_sym_ctr);
		 ctr_done(&t_sym_ctr);
#else
		_teePlayReadyProtect_main(pu1_plaintext, pu1DstBuf+sizeof(TEE_KF_GUID), 16);
#endif		 
		 //cbc_start(find_cipher("aes"),au1_TEE_SS_iv,au1_TEE_SS_ksec,TEE_SECURESTORAGE_SEC_KEY_SIZE,0,&t_sym_cbc);
		 //cbc_encrypt(pu1_plaintext, pu1DstBuf+sizeof(TEE_KF_GUID), 16, &t_sym_cbc);
		 //cbc_done(&t_sym_cbc);
	 } else {
		 memcpy(pu1DstBuf+sizeof(TEE_KF_GUID), pu1_plaintext, 16);
	 }

     

	free(pu1_plaintext);
	return TZ_SUCCESS;
}


UINT32 _teePlayReadyGetProvisionData(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 u4DstLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    BYTE *pu1_plaintext = NULL;
    long l_plainSize = 0;
    
    DMSG("[Trustzone][_teePlayReadyGetProvisionData]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
//#if 0  //add by xavier
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    /* check the buffer size */
    if (u4SrcLen > TEE_SECURESTORAGE_MAX_SIZE || u4DstLen > TEE_SECURESTORAGE_MAX_SIZE) {
        DMSG("lSrcLen or *plDstLen size exceed 2MB.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* allocate */
    pu1_plaintext = (BYTE *)malloc(u4SrcLen);
    if(pu1_plaintext == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }
    memset(pu1_plaintext, 0, u4SrcLen);

    /* check header */
    if(pu1SrcBuf[0] == 0xa2 && pu1SrcBuf[1] == 0x5e && pu1SrcBuf[2] == 0xc5 && pu1SrcBuf[3] == 0x70) {
        DMSG("Header match pre-encrypted format. Decrypt it..\n");
        _teeDDIPreEncA2Dec(pu1SrcBuf, u4SrcLen, pu1_plaintext, (UINT32 *)&l_plainSize);
        if(l_plainSize == 0) {
            DMSG("Decrypt error.\n");
            free(pu1_plaintext);
            return TZ_ERROR_SECURITY;
        }
    } else {
        DMSG("Header doesn't match pre-encrypted format. Try _teeSecureStorageSecretDec..\n");
        /* Decrypt by secure storage API */
        l_plainSize = u4SrcLen;
        if(_teeSecureStorageSecretDec(pu1SrcBuf, u4SrcLen, pu1_plaintext, &l_plainSize) != TZ_SUCCESS) {
            DMSG("Decrypt error.\n");
            free(pu1_plaintext);
            return TZ_ERROR_SECURITY;
        }
    }
    if (l_plainSize != ECC_P256_INTEGER_SIZE_IN_BYTES)
    {
        DMSG("ProvisionData has incorrect len.\n");
	    free(pu1_plaintext);
        return TZ_ERROR_SECURITY;    
    }
    /* keep group key in tz */	
    memcpy(au1_TEE_PLAYREADY_ECCP256GCKey, pu1_plaintext, ECC_P256_INTEGER_SIZE_IN_BYTES);

#if 1 //add by xavier	  
    /* Do not Output result to normal world */
    if (bProtect) {
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(pu1_plaintext, pu1DstBuf, (l_plainSize < u4DstLen) ? l_plainSize : u4DstLen, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
    } else {
        memcpy(pu1DstBuf, pu1_plaintext, (l_plainSize < u4DstLen) ? l_plainSize : u4DstLen);
    }
#endif
#if 0  // add by xavier
    memset(pu1DstBuf, 0, u4DstLen);
#endif
    /* free */
    free(pu1_plaintext);

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyOMAC1SetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4BufLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    BYTE *pu1_plaintext = NULL;
    
    DMSG("[Trustzone][_teePlayReadyOMAC1SetKey]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    if (u4BufLen < u4BufIdx + TEE_PLAYREADY_OMAC1_KEY_SIZE) {
        DMSG("u4BufLen < u4BufIdx + %d.\n", TEE_PLAYREADY_OMAC1_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    register_cipher(&aes_desc);

    if (bProtect) {
        /* allocate */
        pu1_plaintext = (BYTE *)malloc(u4BufLen);
        if(pu1_plaintext == NULL) {
            DMSG("malloc failed.\n");
            return TZ_ERROR_MEMORY;
        }
        memset(pu1_plaintext, 0, u4BufLen);
        /* unprotect key */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_decrypt(pu1KeyBuf, pu1_plaintext, u4BufLen, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(pu1KeyBuf, pu1_plaintext, u4BufLen);
#endif
        memcpy(au1_TEE_PLAYREADY_OMAC1Key, pu1_plaintext + u4BufIdx, TEE_PLAYREADY_OMAC1_KEY_SIZE);
        free(pu1_plaintext);
    } else {
        /* copy key */
        memcpy(au1_TEE_PLAYREADY_OMAC1Key, pu1KeyBuf + u4BufIdx, TEE_PLAYREADY_OMAC1_KEY_SIZE);
    }
    /*DMSG("set au1_TEE_PLAYREADY_AESCBCKey into %02x%02x%02x%02x%02x%02x\n", au1_TEE_PLAYREADY_OMAC1Key[0]
	 	,au1_TEE_PLAYREADY_OMAC1Key[1]
	 	,au1_TEE_PLAYREADY_OMAC1Key[2]
	 	,au1_TEE_PLAYREADY_OMAC1Key[3]
	 	,au1_TEE_PLAYREADY_OMAC1Key[4]
	 	,au1_TEE_PLAYREADY_OMAC1Key[5]);*/
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyOMAC1Verify(UINT8 *pu1DataBuf, UINT32 u4BufIdx, UINT32 u4DataLen, UINT8 *pu1Signature, UINT32 u4SigOffset, BOOL *pbResult)
{
    omac_state t_omac_state;
    BYTE au1_signature[TEE_PLAYREADY_OMAC1_KEY_SIZE];
    UINT32 u4_len = TEE_PLAYREADY_OMAC1_KEY_SIZE;
    
    DMSG("[Trustzone][_teePlayReadyOMAC1Verify]: Enter.\n");

    if (u4BufIdx >= u4DataLen && u4DataLen != 0) {
        DMSG("u4BufIdx >= u4DataLen.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Compute OMAC1 result */
    memset(au1_signature, 0, TEE_PLAYREADY_OMAC1_KEY_SIZE);
    omac_init(&t_omac_state, find_cipher("aes"), au1_TEE_PLAYREADY_OMAC1Key, TEE_PLAYREADY_OMAC1_KEY_SIZE);
    omac_process(&t_omac_state, pu1DataBuf + u4BufIdx, u4DataLen - u4BufIdx);
    omac_done(&t_omac_state, au1_signature, &u4_len);

    /* Compare */
    *pbResult = (memcmp(au1_signature, pu1Signature + u4SigOffset, TEE_PLAYREADY_OMAC1_KEY_SIZE) == 0);

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyOMAC1Sign(UINT8 *pu1DataBuf, UINT32 u4BufIdx, UINT32 u4DataLen, UINT8 *pu1Signature, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    omac_state t_omac_state;
    BYTE au1_signature[TEE_PLAYREADY_OMAC1_KEY_SIZE];
    UINT32 u4_len = TEE_PLAYREADY_OMAC1_KEY_SIZE;
    
    DMSG("[Trustzone][_teePlayReadyOMAC1Sign]: Enter.\n");

//#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
#if 0
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    /* Compute OMAC1 result */
    memset(au1_signature, 0, TEE_PLAYREADY_OMAC1_KEY_SIZE);
    omac_init(&t_omac_state, find_cipher("aes"), au1_TEE_PLAYREADY_OMAC1Key, TEE_PLAYREADY_OMAC1_KEY_SIZE);
    omac_process(&t_omac_state, pu1DataBuf + u4BufIdx, u4DataLen);
    omac_done(&t_omac_state, au1_signature, &u4_len);

    /* Output result */
    if (bProtect) {
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(au1_signature, pu1Signature, TEE_PLAYREADY_OMAC1_KEY_SIZE, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
	_teePlayReadyProtect_main(au1_signature, pu1Signature, TEE_PLAYREADY_OMAC1_KEY_SIZE);
#endif
    } else {
        memcpy(pu1Signature, au1_signature, TEE_PLAYREADY_OMAC1_KEY_SIZE);
    }

    return TZ_SUCCESS;
}

static UINT32 alloc_playready_contentbuf()
{
	/* Allocation */
	pu1_TEE_PLAYREADY_SwBuf = (BYTE *)malloc(TEE_PLAYREADY_SWBUF_SIZE);
	if(pu1_TEE_PLAYREADY_SwBuf == NULL) {
		DMSG("malloc SwBuf failed.\n");
		return TZ_ERROR_MEMORY;
	}
	memset(pu1_TEE_PLAYREADY_SwBuf, 0, TEE_PLAYREADY_SWBUF_SIZE);
	pu1_TEE_PLAYREADY_HwBuf = (BYTE *)malloc(TEE_PLAYREADY_HWBUF_SIZE);
	if(pu1_TEE_PLAYREADY_HwBuf == NULL) {
		DMSG("malloc HwBuf failed.\n");
		return TZ_ERROR_MEMORY;
	}
	memset(pu1_TEE_PLAYREADY_HwBuf, 0, TEE_PLAYREADY_HWBUF_SIZE);
	pu1_TEE_PLAYREADY_HwAlignBuf = (BYTE *)(((unsigned int)pu1_TEE_PLAYREADY_HwBuf + 0x1f) & ~(0x1f));
	pu1_TEE_PLAYREADY_ClearBuf = (BYTE *)malloc(TEE_PLAYREADY_HWBUF_SIZE);
	if(pu1_TEE_PLAYREADY_ClearBuf == NULL) {
		DMSG("malloc ClearBuf failed.\n");
		return TZ_ERROR_MEMORY;
	}
	memset(pu1_TEE_PLAYREADY_ClearBuf, 0, TEE_PLAYREADY_HWBUF_SIZE);
return TZ_SUCCESS;
}



UINT32 _teePlayReadyContentSetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4BufLen, BOOL bProtect, ENUM_CONTENTKEY_TYPE_T keytype)
{
    symmetric_CTR t_sym_ctr;
    BYTE *pu1_plaintext = NULL, au1_key[TEE_SECURESTORAGE_SEC_KEY_SIZE * 2], au1_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE];
    int i;
	BYTE zerostring[TEE_SECURESTORAGE_SEC_KEY_SIZE] = {0};
    UINT8 au1SecureKey[32];

    DMSG("[Trustzone][_teePlayReadyContentSetKey]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    if (u4BufLen < u4BufIdx + TEE_PLAYREADY_CONTENT_KEY_SIZE) {
        DMSG("u4BufLen < u4BufIdx + %d.\n", TEE_PLAYREADY_CONTENT_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    register_cipher(&aes_desc);
	if (bProtect){
	    if (keytype == PLAYREADY_CONTENTKEY_VIDEO) {
	        /* allocate */
	        pu1_plaintext = (BYTE *)malloc(u4BufLen);
	        if(pu1_plaintext == NULL) {
	            DMSG("malloc failed.\n");
	            return TZ_ERROR_MEMORY;
	        }
	        memset(pu1_plaintext, 0, u4BufLen);
	        /* unprotect key */
 #ifndef CC_TEE_FLOW_ENHANCEMENT
	        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
	        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
	        ctr_decrypt(pu1KeyBuf, pu1_plaintext, u4BufLen, &t_sym_ctr);
	        ctr_done(&t_sym_ctr);
#else
               _teePlayReadyUnprotect_main(pu1KeyBuf, pu1_plaintext, u4BufLen);
#endif
	        memcpy(au1_TEE_PLAYREADY_ContentKey, pu1_plaintext + u4BufIdx, TEE_PLAYREADY_CONTENT_KEY_SIZE);
	        free(pu1_plaintext);
	    }
		else if (keytype == PLAYREADY_CONTENTKEY_AUDIO) {
	        /* allocate */
	        pu1_plaintext = (BYTE *)malloc(u4BufLen);
	        if(pu1_plaintext == NULL) {
	            DMSG("malloc failed.\n");
	            return TZ_ERROR_MEMORY;
	        }
	        memset(pu1_plaintext, 0, u4BufLen);
	        /* unprotect key */
 #ifndef CC_TEE_FLOW_ENHANCEMENT
	        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
	        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
	        ctr_decrypt(pu1KeyBuf, pu1_plaintext, u4BufLen, &t_sym_ctr);
	        ctr_done(&t_sym_ctr);
#else
                _teePlayReadyUnprotect_main(pu1KeyBuf, pu1_plaintext, u4BufLen);
#endif
	        memcpy(au1_TEE_PLAYREADY_ContentKey_Audio, pu1_plaintext + u4BufIdx, TEE_PLAYREADY_CONTENT_KEY_SIZE);
	        free(pu1_plaintext);
	    }
#ifdef CC_ENABLE_HCI
		else if (keytype == NAUTILUS_CONTENTKEY_VIDEO) {
	        /* allocate */
	        pu1_plaintext = (BYTE *)malloc(u4BufLen);
	        if(pu1_plaintext == NULL) {
	            DMSG("malloc failed.\n");
	            return TZ_ERROR_MEMORY;
	        }
			memset(pu1_plaintext, 0, u4BufLen);
	        /* unprotect key */
			    DMSG("before _MtkAesDecryption BEGIN:\n");
				TEE_DUMP("_MtkAesDecryption:",u4BufLen,pu1KeyBuf);
				DMSG(" before _MtkAesDecryption END\n");
			_MtkAesDecryption(pu1_plaintext,pu1KeyBuf,u4BufLen);
	        memcpy(au1_TEE_PLAYREADY_ContentKey, pu1_plaintext + u4BufIdx, TEE_PLAYREADY_CONTENT_KEY_SIZE);
				DMSG("after au1_TEE_PLAYREADY_ContentKey BEGIN:\n");
				TEE_DUMP("au1_TEE_PLAYREADY_ContentKey:",TEE_PLAYREADY_CONTENT_KEY_SIZE,au1_TEE_PLAYREADY_ContentKey);
				DMSG("after au1_TEE_PLAYREADY_ContentKey END\n");
	        free(pu1_plaintext);
			}
		else if (keytype == NAUTILUS_CONTENTKEY_AUDIO) {
	        /* allocate */
	        pu1_plaintext = (BYTE *)malloc(u4BufLen);
	        if(pu1_plaintext == NULL) {
	            DMSG("malloc failed.\n");
	            return TZ_ERROR_MEMORY;
	        }
			memset(pu1_plaintext, 0, u4BufLen);
	        /* unprotect key */
			    DMSG("before _MtkAesDecryption BEGIN:\n");
				TEE_DUMP("_MtkAesDecryption:",u4BufLen,pu1KeyBuf);
				DMSG(" before _MtkAesDecryption END\n");
			_MtkAesDecryption(pu1_plaintext,pu1KeyBuf,u4BufLen);
	        memcpy(au1_TEE_PLAYREADY_ContentKey_Audio, pu1_plaintext + u4BufIdx, TEE_PLAYREADY_CONTENT_KEY_SIZE);
				DMSG("after au1_TEE_PLAYREADY_ContentKey_Audio BEGIN:\n");
				TEE_DUMP("au1_TEE_PLAYREADY_ContentKey_Audio:",TEE_PLAYREADY_CONTENT_KEY_SIZE,au1_TEE_PLAYREADY_ContentKey_Audio);
				DMSG("after au1_TEE_PLAYREADY_ContentKey_Audio END\n");
	        free(pu1_plaintext);
			}
#endif
	}
    else{
        /* copy key */
		if ((keytype == NAUTILUS_CONTENTKEY_AUDIO)||(keytype == PLAYREADY_CONTENTKEY_AUDIO))
        	memcpy(au1_TEE_PLAYREADY_ContentKey_Audio, pu1KeyBuf + u4BufIdx, TEE_PLAYREADY_CONTENT_KEY_SIZE);
		else
        	memcpy(au1_TEE_PLAYREADY_ContentKey, pu1KeyBuf + u4BufIdx, TEE_PLAYREADY_CONTENT_KEY_SIZE);
    }

    /* Setup GCPU keyslot for PlayReady content key */

    if ((keytype == NAUTILUS_CONTENTKEY_AUDIO)||(keytype == PLAYREADY_CONTENTKEY_AUDIO))
		GCPU_SetSecureSlot(AES_PLAYREADY_AUD_KEY_PTR, au1_TEE_PLAYREADY_ContentKey_Audio, TEE_PLAYREADY_CONTENT_KEY_SIZE);
	else
    	GCPU_SetSecureSlot(AES_PLAYREADY_KEY_PTR, au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE);

    /* Setup DMX & GCPU keyslot for vdec decrypt and sw/hw buffer allocation */
    if (b_TEE_PLAYREADY_DMXInit == FALSE) {
		memset(au1_key, 0, TEE_SECURESTORAGE_SEC_KEY_SIZE * 2);
        memset(au1_iv, 0, TEE_SECURESTORAGE_SEC_KEY_SIZE);
		TzGetRandomBytes(au1_key, TEE_SECURESTORAGE_SEC_KEY_SIZE);
        
        TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1_key, sizeof(au1_key));
        TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TZ_MM_DRM_IV, DMX_KEY_IDX_MM_DRM_PR_IV, au1_iv, sizeof(au1_iv));
		
        GCPU_SetSecureSlot(AES_SVP_CTR_KEY_PTR, au1_key, TEE_SECURESTORAGE_SEC_KEY_SIZE);		
#ifndef CC_TEE_FLOW_ENHANCEMENT
        /* Allocation */
        pu1_TEE_PLAYREADY_SwBuf = (BYTE *)malloc(TEE_PLAYREADY_SWBUF_SIZE);
        if(pu1_TEE_PLAYREADY_SwBuf == NULL) {
            DMSG("malloc SwBuf failed.\n");
            return TZ_ERROR_MEMORY;
        }
        memset(pu1_TEE_PLAYREADY_SwBuf, 0, TEE_PLAYREADY_SWBUF_SIZE);
        pu1_TEE_PLAYREADY_HwBuf = (BYTE *)malloc(TEE_PLAYREADY_HWBUF_SIZE);
        if(pu1_TEE_PLAYREADY_HwBuf == NULL) {
            DMSG("malloc HwBuf failed.\n");
            return TZ_ERROR_MEMORY;
        }
        memset(pu1_TEE_PLAYREADY_HwBuf, 0, TEE_PLAYREADY_HWBUF_SIZE);
        pu1_TEE_PLAYREADY_HwAlignBuf = (BYTE *)(((unsigned int)pu1_TEE_PLAYREADY_HwBuf + 0x1f) & ~(0x1f));
        pu1_TEE_PLAYREADY_ClearBuf = (BYTE *)malloc(TEE_PLAYREADY_HWBUF_SIZE);
        if(pu1_TEE_PLAYREADY_ClearBuf == NULL) {
            DMSG("malloc ClearBuf failed.\n");
            return TZ_ERROR_MEMORY;
        }
        memset(pu1_TEE_PLAYREADY_ClearBuf, 0, TEE_PLAYREADY_HWBUF_SIZE);
#else
   if (alloc_playready_contentbuf() != TZ_SUCCESS)
   	   return TZ_ERROR_MEMORY;
#endif
        b_TEE_PLAYREADY_DMXInit = TRUE;
    }
        
    TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1SecureKey, sizeof(au1SecureKey));
    TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM_IV, DMX_KEY_IDX_MM_DRM_PR_IV, au1_iv, sizeof(au1_iv));

	if ((memcmp(au1SecureKey, zerostring , TEE_SECURESTORAGE_SEC_KEY_SIZE) == 0)
		||(memcmp(au1_iv, zerostring , TEE_SECURESTORAGE_SEC_KEY_SIZE) == 0))//  if content protect key had been cleared
	{
		/* Setup keyslot */
#if 0		
		memset(au1_key, 0, TEE_SECURESTORAGE_SEC_KEY_SIZE * 2);
		memset(au1_iv, 0, TEE_SECURESTORAGE_SEC_KEY_SIZE);
		_teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
		memcpy(au1_key, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE);
		memcpy(au1_iv, au1_TEE_SS_iv, TEE_SECURESTORAGE_SEC_KEY_SIZE / 2);
		DMX_SetSecureKeyIv(0, au1_key, au1_iv);
	
		memcpy(au1SecureIvTmp, au1SecureIv, 16); 
		/* Setup GCPU keyslot for PlayReady content key */
		GCPU_SetSecureSlot(AES_SVP_CTR_KEY_PTR, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE);
#else
		memset(au1_key, 0, TEE_SECURESTORAGE_SEC_KEY_SIZE * 2);
        memset(au1_iv, 0, TEE_SECURESTORAGE_SEC_KEY_SIZE);
		TzGetRandomBytes(au1_key, TEE_SECURESTORAGE_SEC_KEY_SIZE);

        TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1_key, sizeof(au1_key));
        TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TZ_MM_DRM_IV, DMX_KEY_IDX_MM_DRM_PR_IV, au1_iv, sizeof(au1_iv));

		TEE_DUMP("[xavier debug]au1_key = ", TEE_SECURESTORAGE_SEC_KEY_SIZE * 2, au1_key);
        TEE_DUMP("[xavier debug]au1_iv = ", TEE_SECURESTORAGE_SEC_KEY_SIZE, au1_iv);
		GCPU_SetSecureSlot(AES_SVP_CTR_KEY_PTR, au1_key, TEE_SECURESTORAGE_SEC_KEY_SIZE);
#endif
	}
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyDMXSetKey(UINT8 *pu1Buf, UINT32 u4Len)
{
    BYTE *pu1_plaintext = NULL;
    BYTE  au1_key[TEE_PLAYREADY_CONTENT_KEY_SIZE * 2];
    BYTE au1_iv[TEE_PLAYREADY_CONTENT_KEY_SIZE];	
    symmetric_CTR t_sym_ctr;
    int i;
	
    TZ_NDDRM_T * ptNddr = (TZ_NDDRM_T *) pu1Buf;

    TZ_SWD_BufferStart((UINT32)ptNddr->pu1KeyBuf, TZ_CACHE_ALIGN(ptNddr->u4KeyLen));
    TZ_SWD_BufferStart((UINT32)ptNddr->pu1IvBuf, TZ_CACHE_ALIGN(ptNddr->u4IvLen));

    DMSG("[Trustzone][_teePlayReadyDMXSetKey]: Enter.\n");

    if (ptNddr->u4KeyLen != TEE_PLAYREADY_CONTENT_KEY_SIZE) {
        DMSG("u4KeyLen < %d.\n", TEE_PLAYREADY_CONTENT_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (ptNddr->pu1KeyBuf == NULL)
    {
        DMSG("pu1KeyBuf is NULL. \n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (ptNddr->u4IvLen != TEE_PLAYREADY_CONTENT_KEY_SIZE/2) {
        DMSG("u4IvLen < %d.\n", TEE_PLAYREADY_CONTENT_KEY_SIZE/2);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (ptNddr->pu1IvBuf  == NULL)
    {
        DMSG("pu1KeyBuf is NULL. \n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }


    /* allocate */
    pu1_plaintext = (BYTE *)malloc(ptNddr->u4KeyLen);
    if(pu1_plaintext == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }
    memset(pu1_plaintext, 0, ptNddr->u4KeyLen);

    /* unprotect key */
#ifndef CC_TEE_FLOW_ENHANCEMENT		
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_decrypt(ptNddr->pu1KeyBuf, pu1_plaintext, ptNddr->u4KeyLen, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(ptNddr->pu1KeyBuf, pu1_plaintext, ptNddr->u4KeyLen);
#endif		
    memcpy(au1_TEE_PLAYREADY_ContentKey, pu1_plaintext, TEE_PLAYREADY_CONTENT_KEY_SIZE);

    free(pu1_plaintext);

    memset(au1_key, 0, TEE_PLAYREADY_CONTENT_KEY_SIZE*2);
    memset(au1_iv, 0, TEE_PLAYREADY_CONTENT_KEY_SIZE);
    memcpy(au1_key, au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE);

    /* setup last 8 bytes IV */
    for (i = 0; (i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2) && (i <ptNddr->u4IvLen) ; i++) {
        au1_iv[i] = ptNddr->pu1IvBuf[ i];
    }    	

    TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1_key, sizeof(au1_key));
    TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TZ_MM_DRM_IV, DMX_KEY_IDX_MM_DRM_PR_IV, au1_iv, sizeof(au1_iv));

    //TEE_DUMP("content key = ", TEE_PLAYREADY_CONTENT_KEY_SIZE, au1_key);
    //TEE_DUMP("IV = ", TEE_PLAYREADY_CONTENT_KEY_SIZE, au1_iv);
    TZ_SWD_BufferEnd((UINT32)ptNddr->pu1KeyBuf, TZ_CACHE_ALIGN(ptNddr->u4KeyLen));
    TZ_SWD_BufferEnd((UINT32)ptNddr->pu1IvBuf, TZ_CACHE_ALIGN(ptNddr->u4IvLen));

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyContentDecrypt(DRM_AES_COUNTER_MODE_CONTEXT *pCtrContext, UINT8 *pu1Buf, UINT32 u4Len, BOOL bProtect, UINT64 u8ByteOffset)
{
    symmetric_ECB t_sym_ecb;
    symmetric_CTR t_sym_ctr;
    BYTE au1_pad[TEE_PLAYREADY_CONTENT_KEY_SIZE], au1_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE], au1_retCtr[TEE_SECURESTORAGE_SEC_KEY_SIZE], u1_use = 0;
    UINT32 u4_lft = u4Len, u4_cur = 0, i = 0;
    UINT64 u8_blockOffset = u8ByteOffset / 16;

    DMSG("[Trustzone][_teePlayReadyContentDecrypt]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    /* check the buffer size */
    if (u4Len > TEE_SECURESTORAGE_MAX_SIZE) {
        DMSG("u4Len size exceed 2MB.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* clear buffer */
    memset(pu1_TEE_PLAYREADY_SwBuf, 0, TEE_PLAYREADY_SWBUF_SIZE);

    /* Setup first 8 bytes IV */
    for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
        au1_TEE_PLAYREADY_ContentIV[i] = ((UINT8 *)&pCtrContext->qwInitializationVector)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
    }

    /* Special decryption case for data is in the middle of a block */
    if( pCtrContext->bByteOffset > 0 ) {		
        /* calculate number of bytes to decrypt */
        u1_use = TEE_PLAYREADY_CONTENT_KEY_SIZE - pCtrContext->bByteOffset;
        u1_use = (u1_use < u4_lft) ? u1_use : u4_lft;
        /* setup last 8 bytes IV */
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&pCtrContext->qwBlockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }

        /* AES encrypt IV to au1_pad */
        ecb_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, &t_sym_ecb);
        ecb_encrypt(au1_TEE_PLAYREADY_ContentIV, au1_pad, TEE_PLAYREADY_CONTENT_KEY_SIZE, &t_sym_ecb);
        ecb_done(&t_sym_ecb);
        /* XOR */
        for (i = 0; i < u1_use; i++) {
            if (pCtrContext->bByteOffset + i < 16) {
                pu1_TEE_PLAYREADY_SwBuf[i] = pu1Buf[i] ^ au1_pad[pCtrContext->bByteOffset + i];
            }
        }
        u4_cur += u1_use;
        u4_lft -= u1_use;
        pCtrContext->qwBlockOffset++;

    }

    /* General decryption case */
    if (u4_lft > 0) {
        /* setup last 8 bytes IV */
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&pCtrContext->qwBlockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }
        /* AES-CTR decrypt */
        if(u4_lft < 16) {
            /* Software solution */
            ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
            ctr_decrypt(pu1Buf + u4_cur, pu1_TEE_PLAYREADY_SwBuf + u4_cur, u4_lft, &t_sym_ctr);
            ctr_done(&t_sym_ctr);
        } else {
#ifndef TZ_USE_SW_AES
            /* Hardware GCPU solution */
            memcpy(pu1_TEE_PLAYREADY_HwAlignBuf, pu1Buf + u4_cur, u4_lft);
            if(TZ_PLAYREADY_AES_CTR(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft, au1_TEE_PLAYREADY_ContentIV, au1_retCtr) != S_GCPU_OK) {
                DMSG("malloc failed.\n");
                return TZ_ERROR_CRYPTO;
            }
#else
            /* Software solution */
            ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
            ctr_decrypt(pu1Buf + u4_cur, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft, &t_sym_ctr);
            ctr_done(&t_sym_ctr);
#endif
        }
    }

    pCtrContext->qwBlockOffset += u4_lft/16;
	
    /* Output result */
    if (bProtect) {
        UINT8 au1SecureKey[32];

        TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1SecureKey, sizeof(au1SecureKey));

        /* Setup IV for protection */		
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&u8_blockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }

        if(u4_lft < 16 && u4Len < TEE_PLAYREADY_SWBUF_SIZE) {
            /* Software solution */
            ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1SecureKey, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
            ctr_encrypt(pu1_TEE_PLAYREADY_SwBuf, pu1Buf, u4Len, &t_sym_ctr);
            ctr_done(&t_sym_ctr);
        } else {
            /* Hardware GCPU solution */
            memmove(pu1_TEE_PLAYREADY_HwAlignBuf + u4_cur, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft);
            memcpy(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_SwBuf, u4_cur);
#ifndef TZ_USE_SW_AES
            if(TZ_SVP_AES_CTR(pu1_TEE_PLAYREADY_HwAlignBuf, pu1Buf, u4Len, au1_TEE_PLAYREADY_ContentIV, au1_retCtr) != S_GCPU_OK) {
                DMSG("malloc failed.\n");
                return TZ_ERROR_CRYPTO;
            }
#else
            /* Software solution */
            ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1SecureKey, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
            ctr_encrypt(pu1_TEE_PLAYREADY_HwAlignBuf, pu1Buf, u4Len, &t_sym_ctr);
            ctr_done(&t_sym_ctr);
#endif
        }
    } else {
        if(u4_lft < 16 && u4Len < TEE_PLAYREADY_SWBUF_SIZE) {
            memcpy(pu1Buf, pu1_TEE_PLAYREADY_SwBuf, u4Len);
        } else {
            memcpy(pu1Buf, pu1_TEE_PLAYREADY_SwBuf, u4_cur);
            memcpy(pu1Buf + u4_cur, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft);
        }
    }

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyMultiContentDecrypt(DRM_AES_COUNTER_MODE_CONTEXT *pCtrContext, UINT8 *pu1Buf, UINT32 u4Len, UINT32 *au4Seg, UINT32 u4SegLen, UINT64 u8ByteOffset, BOOL bProtect)
{
    symmetric_ECB t_sym_ecb;
    symmetric_CTR t_sym_ctr;
    BYTE au1_pad[TEE_PLAYREADY_CONTENT_KEY_SIZE], au1_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE], au1_retCtr[TEE_SECURESTORAGE_SEC_KEY_SIZE], u1_use = 0;
    UINT32 u4_bufOffset = 0, u4_encOffset = 0, u4_tmp = 0, u4_lft = 0, u4_cur = 0, i = 0, segIdx = 0;
    UINT32 au4_seg[24], u4_segLen = 0;
    UINT64 u8_blockOffset = u8ByteOffset / TEE_PLAYREADY_CONTENT_KEY_SIZE;
    
    DMSG("[Trustzone][_teePlayReadyMultiContentDecrypt]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    /* check the buffer size */
    if (u4Len > TEE_PLAYREADY_HWBUF_SIZE) {
        DMSG("u4Len size exceed TEE_PLAYREADY_HWBUF_SIZE.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* clear buffer */
    memset(pu1_TEE_PLAYREADY_SwBuf, 0, TEE_PLAYREADY_SWBUF_SIZE);
    memset(pu1_TEE_PLAYREADY_ClearBuf, 0, TEE_PLAYREADY_HWBUF_SIZE);
    memset(au4_seg, 0, sizeof(au4_seg));

    /* Setup first 8 bytes IV */
    for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
        au1_TEE_PLAYREADY_ContentIV[i] = ((UINT8 *)&pCtrContext->qwInitializationVector)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
    }

    /* Calculate local au4_seg & u4_segLen from au4Seg, u4SegLen & u8ByteOffset, and count qwBlockOffset & bByteOffset */
    for (segIdx = 0; segIdx < u4SegLen; segIdx++) {
        u4_tmp = au4Seg[segIdx] & 0x7fffffff;
        if (u4_bufOffset + u4_tmp > u8ByteOffset) {
            memcpy(au4_seg, &au4Seg[segIdx], (u4SegLen - segIdx) * sizeof(UINT32));
            u4_segLen = u4SegLen - segIdx;
            au4_seg[0] -= (u8ByteOffset - u4_bufOffset);
            if (au4Seg[segIdx] & 0x80000000) {
                u4_encOffset += (u8ByteOffset - u4_bufOffset);
            } 
            pCtrContext->qwBlockOffset = u4_encOffset >> 4;
            pCtrContext->bByteOffset = u4_encOffset & 0xf;
            break;
        } else if (au4Seg[segIdx] & 0x80000000) {
            u4_encOffset += u4_tmp;
        }
        u4_bufOffset += u4_tmp;
    }

    /* Process clear data & encrypted data */
    u4_encOffset = 0;
    u4_bufOffset = 0;
    for (segIdx = 0; segIdx < u4_segLen && u4_bufOffset < u4Len; segIdx++) {
        u4_tmp = au4_seg[segIdx] & 0x7fffffff;
        if (u4_bufOffset + u4_tmp > u4Len) {
            u4_tmp = u4Len - u4_bufOffset;
        }
        if (au4_seg[segIdx] & 0x80000000) { /* Encrypt */
            memcpy(pu1_TEE_PLAYREADY_HwAlignBuf + u4_encOffset, pu1Buf + u4_bufOffset, u4_tmp);
            u4_encOffset += u4_tmp;
        } else { /* Clear */
            memcpy(pu1_TEE_PLAYREADY_ClearBuf + u4_bufOffset, pu1Buf + u4_bufOffset, u4_tmp);
        }
        u4_bufOffset += u4_tmp;
    }

    /* Decrypt */
    u4_lft = u4_encOffset;
    u4_cur = 0;
    /* Special decryption case for data is in the middle of a block */
    if( pCtrContext->bByteOffset > 0 ) {		
        /* calculate number of bytes to decrypt */
        u1_use = TEE_PLAYREADY_CONTENT_KEY_SIZE - pCtrContext->bByteOffset;
        u1_use = (u1_use < u4_lft) ? u1_use : u4_lft;
        /* setup last 8 bytes IV */
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&pCtrContext->qwBlockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }

        /* AES encrypt IV to au1_pad */
        ecb_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, &t_sym_ecb);
        ecb_encrypt(au1_TEE_PLAYREADY_ContentIV, au1_pad, TEE_PLAYREADY_CONTENT_KEY_SIZE, &t_sym_ecb);
        ecb_done(&t_sym_ecb);
        /* XOR */
        for (i = 0; i < u1_use; i++) {
            if (pCtrContext->bByteOffset + i < 16) {
                pu1_TEE_PLAYREADY_SwBuf[i] = pu1_TEE_PLAYREADY_HwAlignBuf[i] ^ au1_pad[pCtrContext->bByteOffset + i];
            }
        }
        u4_cur += u1_use;
        u4_lft -= u1_use;
        pCtrContext->qwBlockOffset++;
    }
    /* General decryption case */
    if (u4_lft > 0) {
        /* setup last 8 bytes IV */
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&pCtrContext->qwBlockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }
        /* AES-CTR decrypt */
        if (u4_cur) {
            memmove(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf + u4_cur, u4_lft);
        }
#ifndef TZ_USE_SW_AES /* Hardware GCPU solution */
        if(TZ_PLAYREADY_AES_CTR(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft, au1_TEE_PLAYREADY_ContentIV, au1_retCtr) != S_GCPU_OK) {
            DMSG("malloc failed.\n");
            return TZ_ERROR_CRYPTO;
        }
#else /* Software solution */
        ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
        ctr_decrypt(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#endif
    }

    /* Copy clear data to pu1_TEE_PLAYREADY_HwAlignBuf */
    if (u4_cur) {
        memmove(pu1_TEE_PLAYREADY_HwAlignBuf + u4_cur, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft);
        memcpy(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_SwBuf, u4_cur);
    }

    /* Copy clear data to pu1_TEE_PLAYREADY_ClearBuf */
    u4_encOffset = 0;
    u4_bufOffset = 0;
    for (segIdx = 0; segIdx < u4_segLen && u4_bufOffset < u4Len; segIdx++) {
        u4_tmp = au4_seg[segIdx] & 0x7fffffff;
        if (u4_bufOffset + u4_tmp > u4Len) {
            u4_tmp = u4Len - u4_bufOffset;
        }
        if (au4_seg[segIdx] & 0x80000000) { /* Encrypt */
            memcpy(pu1_TEE_PLAYREADY_ClearBuf + u4_bufOffset, pu1_TEE_PLAYREADY_HwAlignBuf + u4_encOffset, u4_tmp);
            u4_encOffset += u4_tmp;
        }
        u4_bufOffset += u4_tmp;
    }
	
    /* Output result */
    if (bProtect) {
        UINT8 au1SecureKey[32];

        TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1SecureKey, sizeof(au1SecureKey));

        /* Setup IV for protection */		
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&u8_blockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }
#ifndef TZ_USE_SW_AES
        /* Hardware GCPU solution */
        memcpy(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_ClearBuf, u4Len);
        if(TZ_SVP_AES_CTR(pu1_TEE_PLAYREADY_HwAlignBuf, pu1Buf, u4Len, au1_TEE_PLAYREADY_ContentIV, au1_retCtr) != S_GCPU_OK) {
            DMSG("malloc failed.\n");
            return TZ_ERROR_CRYPTO;
        }
#else
        /* Software solution */
        ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1SecureKey, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
        ctr_encrypt(pu1_TEE_PLAYREADY_ClearBuf, pu1Buf, u4Len, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#endif
    } else {
        memcpy(pu1Buf, pu1_TEE_PLAYREADY_ClearBuf, u4Len);
    }

    return TZ_SUCCESS;
}

static int AddTwoCharArray(UINT8* input1,UINT8* input2,UINT8* result,UINT32 size)
{
    int i;
    UINT8 overflow = 0;

    if (input1==NULL||input2 ==NULL||result==NULL||size ==0)
        return -1;

    for (i=size-1;i>=0;i--)
    {
        if (input1[i] > (0xff - input2[i] - overflow))
    	{
            result[i] = input1[i] + input2[i] + overflow;
            overflow = 1;
	    }
        else
		{
            result[i] = input1[i] + input2[i] + overflow;
            overflow = 0;
	    }
    
    }

    return overflow;
}

UINT32 _teePlayReadyDashSampleDecrypt(DRM_DASHAES_COUNTER_MODE_CONTEXT *pCtrContext, UINT8 *pu1Buf, UINT32 u4Len, UINT32 *au4Seg, UINT32 u4SegLen, UINT64 u8ByteOffset, BOOL bProtect, BOOL isAudio)
{
    symmetric_ECB t_sym_ecb;
    symmetric_CTR t_sym_ctr;
    BYTE au1_pad[TEE_PLAYREADY_CONTENT_KEY_SIZE], au1_iv[TEE_SECURESTORAGE_SEC_KEY_SIZE], au1_retCtr[TEE_SECURESTORAGE_SEC_KEY_SIZE], au1_tmp[TEE_PLAYREADY_CONTENT_KEY_SIZE], u1_use = 0;
    UINT32 u4_bufOffset = 0, u4_encOffset = 0, u4_tmp = 0, u4_lft = 0, u4_cur = 0, i = 0, segIdx = 0;
    UINT32 au4_seg[24], u4_segLen = 0;
    UINT64 u8_blockOffset = u8ByteOffset / TEE_PLAYREADY_CONTENT_KEY_SIZE;
	
    DMSG("[Trustzone][_teePlayReadyMultiContentDecrypt]: Enter.\n");

#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    /* check the buffer size */
    if (u4Len > TEE_PLAYREADY_HWBUF_SIZE) {
        DMSG("u4Len size exceed TEE_PLAYREADY_HWBUF_SIZE.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* clear buffer */
    memset(pu1_TEE_PLAYREADY_SwBuf, 0, TEE_PLAYREADY_SWBUF_SIZE);
    memset(pu1_TEE_PLAYREADY_ClearBuf, 0, TEE_PLAYREADY_HWBUF_SIZE);
    memset(au4_seg, 0, sizeof(au4_seg));
	memset(au1_tmp, 0, TEE_PLAYREADY_CONTENT_KEY_SIZE);

	/* Setup first 8 bytes IV */
    for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
        au1_TEE_PLAYREADY_ContentIV[i] = ((UINT8 *)&pCtrContext->qwInitializationVector)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
    }

    /* Calculate local au4_seg & u4_segLen from au4Seg, u4SegLen & u8ByteOffset, and count qwBlockOffset & bByteOffset */
    for (segIdx = 0; segIdx < u4SegLen; segIdx++) {
        u4_tmp = au4Seg[segIdx] & 0x7fffffff;
        if (u4_bufOffset + u4_tmp > u8ByteOffset) {
            memcpy(au4_seg, &au4Seg[segIdx], (u4SegLen - segIdx) * sizeof(UINT32));
            u4_segLen = u4SegLen - segIdx;
            au4_seg[0] -= (u8ByteOffset - u4_bufOffset);
            if (au4Seg[segIdx] & 0x80000000) {
                u4_encOffset += (u8ByteOffset - u4_bufOffset);
            } 
            pCtrContext->qwBlockOffset = u4_encOffset >> 4;
            pCtrContext->bByteOffset = u4_encOffset & 0xf;
            break;
        } else if (au4Seg[segIdx] & 0x80000000) {
            u4_encOffset += u4_tmp;
        }
        u4_bufOffset += u4_tmp;
    }

    /* Process clear data & encrypted data */
    u4_encOffset = 0;
    u4_bufOffset = 0;
    for (segIdx = 0; segIdx < u4_segLen && u4_bufOffset < u4Len; segIdx++) {
        u4_tmp = au4_seg[segIdx] & 0x7fffffff;
        if (u4_bufOffset + u4_tmp > u4Len) {
            u4_tmp = u4Len - u4_bufOffset;
        }
        if (au4_seg[segIdx] & 0x80000000) { /* Encrypt */
            memcpy(pu1_TEE_PLAYREADY_HwAlignBuf + u4_encOffset, pu1Buf + u4_bufOffset, u4_tmp);
            u4_encOffset += u4_tmp;
        } else { /* Clear */
            memcpy(pu1_TEE_PLAYREADY_ClearBuf + u4_bufOffset, pu1Buf + u4_bufOffset, u4_tmp);
        }
        u4_bufOffset += u4_tmp;
    }

    /* Decrypt */
    u4_lft = u4_encOffset;
    u4_cur = 0;
    /* Special decryption case for data is in the middle of a block */
    if( pCtrContext->bByteOffset > 0 ) {		
        /* calculate number of bytes to decrypt */
        u1_use = TEE_PLAYREADY_CONTENT_KEY_SIZE - pCtrContext->bByteOffset;
        u1_use = (u1_use < u4_lft) ? u1_use : u4_lft;
        /* setup last 8 bytes IV */
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8*) &pCtrContext->qwCount)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
			au1_tmp[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8*) &pCtrContext->qwBlockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }
		AddTwoCharArray(au1_TEE_PLAYREADY_ContentIV, au1_tmp, au1_TEE_PLAYREADY_ContentIV, TEE_PLAYREADY_CONTENT_KEY_SIZE);

        /* AES encrypt IV to au1_pad */
        ecb_start(find_cipher("aes"), isAudio ? au1_TEE_PLAYREADY_ContentKey_Audio : au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, &t_sym_ecb);
        ecb_encrypt(au1_TEE_PLAYREADY_ContentIV, au1_pad, TEE_PLAYREADY_CONTENT_KEY_SIZE, &t_sym_ecb);
        ecb_done(&t_sym_ecb);
        /* XOR */
        for (i = 0; i < u1_use; i++) {
            if (pCtrContext->bByteOffset + i < 16) {
                pu1_TEE_PLAYREADY_SwBuf[i] = pu1_TEE_PLAYREADY_HwAlignBuf[i] ^ au1_pad[pCtrContext->bByteOffset + i];
            }
        }
        u4_cur += u1_use;
        u4_lft -= u1_use;
        pCtrContext->qwBlockOffset++;
    }
    /* General decryption case */
    if (u4_lft > 0) {
        /* setup last 8 bytes IV */
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[i] = ((UINT8 *)&pCtrContext->qwInitializationVector)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&pCtrContext->qwBlockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
			au1_tmp[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8*) &pCtrContext->qwCount)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }
		AddTwoCharArray(au1_TEE_PLAYREADY_ContentIV, au1_tmp, au1_TEE_PLAYREADY_ContentIV, TEE_PLAYREADY_CONTENT_KEY_SIZE);

        /* AES-CTR decrypt */
        if (u4_cur) {
            memmove(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf + u4_cur, u4_lft);
        }
#ifndef TZ_USE_SW_AES /* Hardware GCPU solution */
        if(TZ_PLAYREADY_AES_CTR_Ex(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft, au1_TEE_PLAYREADY_ContentIV, au1_retCtr,isAudio) != S_GCPU_OK) {
            DMSG("malloc failed.\n");
            return TZ_ERROR_CRYPTO;
        }	
#else /* Software solution */
        ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, isAudio ? au1_TEE_PLAYREADY_ContentKey_Audio : au1_TEE_PLAYREADY_ContentKey, TEE_PLAYREADY_CONTENT_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
        ctr_decrypt(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft, &t_sym_ctr);
        ctr_done(&t_sym_ctr);

#endif
    }

    /* Copy clear data to pu1_TEE_PLAYREADY_HwAlignBuf */
    if (u4_cur) {
        memmove(pu1_TEE_PLAYREADY_HwAlignBuf + u4_cur, pu1_TEE_PLAYREADY_HwAlignBuf, u4_lft);
        memcpy(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_SwBuf, u4_cur);
    }

    /* Copy clear data to pu1_TEE_PLAYREADY_ClearBuf */
    u4_encOffset = 0;
    u4_bufOffset = 0;
    for (segIdx = 0; segIdx < u4_segLen && u4_bufOffset < u4Len; segIdx++) {
        u4_tmp = au4_seg[segIdx] & 0x7fffffff;
        if (u4_bufOffset + u4_tmp > u4Len) {
            u4_tmp = u4Len - u4_bufOffset;
        }
        if (au4_seg[segIdx] & 0x80000000) { /* Encrypt */
            memcpy(pu1_TEE_PLAYREADY_ClearBuf + u4_bufOffset, pu1_TEE_PLAYREADY_HwAlignBuf + u4_encOffset, u4_tmp);
            u4_encOffset += u4_tmp;
        }
        u4_bufOffset += u4_tmp;
    }
	
    /* Output result */
    if (bProtect) {
        UINT8 au1SecureKey[32];

        TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, DMX_KEY_IDX_MM_DRM_PR, au1SecureKey, sizeof(au1SecureKey));

        /* Setup IV for protection */		
        for (i = 0; i < TEE_PLAYREADY_CONTENT_KEY_SIZE / 2; i++) {
            au1_TEE_PLAYREADY_ContentIV[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 + i] = ((UINT8 *)&u8_blockOffset)[TEE_PLAYREADY_CONTENT_KEY_SIZE / 2 - 1 - i];
        }
#ifndef TZ_USE_SW_AES
        /* Hardware GCPU solution */
        memcpy(pu1_TEE_PLAYREADY_HwAlignBuf, pu1_TEE_PLAYREADY_ClearBuf, u4Len);
        if(TZ_SVP_AES_CTR(pu1_TEE_PLAYREADY_HwAlignBuf, pu1Buf, u4Len, au1_TEE_PLAYREADY_ContentIV, au1_retCtr) != S_GCPU_OK) {
            DMSG("malloc failed.\n");
            return TZ_ERROR_CRYPTO;
        }
#else
        /* Software solution */
        ctr_start(find_cipher("aes"), au1_TEE_PLAYREADY_ContentIV, au1SecureKey, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN, &t_sym_ctr);
        ctr_encrypt(pu1_TEE_PLAYREADY_ClearBuf, pu1Buf, u4Len, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#endif
    } else {
        memcpy(pu1Buf, pu1_TEE_PLAYREADY_ClearBuf, u4Len);
    }

    return TZ_SUCCESS;
}


UINT32 _teePlayReadyAESECBSetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4KeyLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    
#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect ) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    DMSG("[Trustzone][_teePlayReadyAESECBSetKey]: Enter.\n");

    if (NULL == pu1KeyBuf)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    
    if (u4KeyLen <  (u4BufIdx + TEE_PLAYREADY_AESECB_KEY_SIZE)) {
        DMSG("u4KeyLen < u4BufIdx + %d.\n", TEE_PLAYREADY_AESECB_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    register_cipher(&aes_desc);

    if (bProtect) {
#ifndef CC_TEE_FLOW_ENHANCEMENT		
        /* unprotect key */
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_decrypt(pu1KeyBuf + u4BufIdx, au1_TEE_PLAYREADY_AESECBKey, TEE_PLAYREADY_AESECB_KEY_SIZE, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
		_teePlayReadyUnprotect_main(pu1KeyBuf + u4BufIdx,au1_TEE_PLAYREADY_AESECBKey,TEE_PLAYREADY_AESECB_KEY_SIZE);
#endif
    } else {
        /* copy key */
        memcpy(au1_TEE_PLAYREADY_AESECBKey, pu1KeyBuf + u4BufIdx, TEE_PLAYREADY_AESECB_KEY_SIZE);
    }
    
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyAESECBEncrypt(UINT8 * pu1Data, UINT32 u4DataLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    symmetric_ECB t_sym_ecb;
    UINT8 * pu1ProtectData;
    
    DMSG("[Trustzone][_teePlayReadyAESECBEncrypt]: Enter.\n");

    pu1ProtectData = malloc(sizeof(UINT8) * u4DataLen);
    if (!pu1ProtectData) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;        
    }
    /* encrypt data */
    ecb_start(find_cipher("aes"), au1_TEE_PLAYREADY_AESECBKey, TEE_PLAYREADY_AESECB_KEY_SIZE, 0, &t_sym_ecb);
    ecb_encrypt(pu1Data, pu1ProtectData, u4DataLen, &t_sym_ecb);
    ecb_done(&t_sym_ecb);
    if (bProtect) {
        /* protect */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(pu1ProtectData, pu1Data, u4DataLen, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
	_teePlayReadyProtect_main(pu1ProtectData,pu1Data,u4DataLen);
#endif
    } else {
        /* copy */
        memcpy(pu1Data, pu1ProtectData, u4DataLen);
    }    

    if (pu1ProtectData) {
        free(pu1ProtectData);
    }    
    return TZ_SUCCESS;
}
	   	
UINT32 _teePlayReadyAESECBDecrypt(UINT8 * pu1Data, UINT32 u4DataLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    symmetric_ECB t_sym_ecb;
    UINT8 * pu1ProtectData;
    
    DMSG("[Trustzone][_teePlayReadyAESECBDecrypt]: Enter.\n");

    pu1ProtectData = malloc(sizeof(UINT8) * u4DataLen);
    if (!pu1ProtectData) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;        
    }
    /* decrypt data */
    ecb_start(find_cipher("aes"), au1_TEE_PLAYREADY_AESECBKey, TEE_PLAYREADY_AESECB_KEY_SIZE, 0, &t_sym_ecb);
    ecb_decrypt(pu1Data, pu1ProtectData, u4DataLen, &t_sym_ecb);
    ecb_done(&t_sym_ecb);

    if (bProtect) {
        /* protect */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(pu1ProtectData, pu1Data, u4DataLen, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
	_teePlayReadyProtect_main(pu1ProtectData,pu1Data,u4DataLen);
#endif		
    } else {
        /* copy */
        memcpy(pu1Data, pu1ProtectData, u4DataLen);
    }      

    if (pu1ProtectData) {
        free(pu1ProtectData);
    }    

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyAESCBCSetKey(UINT8 *pu1KeyBuf, UINT32 u4BufIdx, UINT32 u4KeyLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    
#ifdef CC_TEE_STRICT_PROTECT_SUPPORT
    if (!bProtect ) {
        DMSG("bProtect doesn't support 'false'.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
#endif

    DMSG("[Trustzone][_teePlayReadyAESCBCSetKey]: Enter.\n");

    if (NULL == pu1KeyBuf)
    {
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    
    if (u4KeyLen <  (u4BufIdx + TEE_PLAYREADY_AESCBC_KEY_SIZE)) {
        DMSG("u4KeyLen < u4BufIdx + %d.\n", TEE_PLAYREADY_AESCBC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    register_cipher(&aes_desc);

    if (bProtect) {
        /* unprotect key */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_decrypt(pu1KeyBuf + u4BufIdx, au1_TEE_PLAYREADY_AESCBCKey, TEE_PLAYREADY_AESCBC_KEY_SIZE, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(pu1KeyBuf + u4BufIdx,au1_TEE_PLAYREADY_AESCBCKey,TEE_PLAYREADY_AESCBC_KEY_SIZE);
#endif		
    } else {
        /* copy key */
        memcpy(au1_TEE_PLAYREADY_AESCBCKey, pu1KeyBuf + u4BufIdx, TEE_PLAYREADY_AESCBC_KEY_SIZE);
    }
     DMSG("set au1_TEE_PLAYREADY_AESCBCKey into %02x%02x%02x%02x%02x%02x\n", au1_TEE_PLAYREADY_AESCBCKey[0]
	 	,au1_TEE_PLAYREADY_AESCBCKey[1]
	 	,au1_TEE_PLAYREADY_AESCBCKey[2]
	 	,au1_TEE_PLAYREADY_AESCBCKey[3]
	 	,au1_TEE_PLAYREADY_AESCBCKey[4]
	 	,au1_TEE_PLAYREADY_AESCBCKey[5]);
    return TZ_SUCCESS;
}

UINT32 _teePlayReadyAESCBCEncrypt(UINT8 * pu1Data, UINT32 u4DataLen, UINT8* IV,UINT32 IVLen, BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    symmetric_CBC t_sym_cbc;
    UINT8 * pu1ProtectData;
	int i=0;
    
    DMSG("[Trustzone][_teePlayReadyAESCBCEncrypt]: Enter.\n");

    pu1ProtectData = malloc(sizeof(UINT8) * u4DataLen);
    if (!pu1ProtectData) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;        
    }
    DMSG("set _teePlayReadyAESCBCEncrypt IV BEGIN:\n");
	for(i=0;i<16;i++)
		DMSG("%02x",IV[i]);
	DMSG(" IV END\n");
    /* encrypt data */
    cbc_start(find_cipher("aes"),IV,au1_TEE_PLAYREADY_AESCBCKey, TEE_PLAYREADY_AESCBC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_encrypt(pu1Data, pu1ProtectData, u4DataLen, &t_sym_cbc);
    cbc_done(&t_sym_cbc);
    
    if (bProtect) {
        /* protect */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(pu1ProtectData, pu1Data, u4DataLen, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
_teePlayReadyProtect_minor(pu1ProtectData,pu1Data,u4DataLen);

#endif
    } else {
        /* copy */
        memcpy(pu1Data, pu1ProtectData, u4DataLen);
    }    

    if (pu1ProtectData) {
        free(pu1ProtectData);
    }    
    return TZ_SUCCESS;
}
	   	
UINT32 _teePlayReadyAESCBCDecrypt(UINT8 * pu1Data, UINT32 u4DataLen, UINT8* IV,UINT32 IVLen,BOOL bProtect)
{
    symmetric_CTR t_sym_ctr;
    symmetric_CBC t_sym_cbc;
    UINT8 * pu1ProtectData;
	int i;
    
    DMSG("[Trustzone][_teePlayReadyAESCBCDecrypt]: Enter.\n");

    pu1ProtectData = malloc(sizeof(UINT8) * u4DataLen);
    if (!pu1ProtectData) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;        
    }
    DMSG("set _teePlayReadyAESCBCEncrypt IV BEGIN:\n");
	for(i=0;i<16;i++)
		DMSG("%02x",IV[i]);
	DMSG(" IV END\n");
    /* decrypt data */
    cbc_start(find_cipher("aes"), IV,au1_TEE_PLAYREADY_AESCBCKey, TEE_PLAYREADY_AESCBC_KEY_SIZE, 0, &t_sym_cbc);
    cbc_decrypt(pu1Data, pu1ProtectData, u4DataLen, &t_sym_cbc);
    cbc_done(&t_sym_cbc);

    if (bProtect) {
        /* protect */
#ifndef CC_TEE_FLOW_ENHANCEMENT			
        _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
        ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
        ctr_encrypt(pu1ProtectData, pu1Data, u4DataLen, &t_sym_ctr);
        ctr_done(&t_sym_ctr);
#else
_teePlayReadyProtect_minor(pu1ProtectData,pu1Data,u4DataLen);

#endif		
    } else {
        /* copy */
        memcpy(pu1Data, pu1ProtectData, u4DataLen);
    }      

    if (pu1ProtectData) {
        free(pu1ProtectData);
    }    

    return TZ_SUCCESS;
}

UINT32 _teePlayReadyUnshuffleContentKey(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromIdx, UINT32 u4FromSize, UINT32 u4Count)
{
    symmetric_CTR t_sym_ctr;
    UINT8 *pu1SrcBuf = NULL, *pu1DstBuf = NULL;
	UINT8 *rgbTmp[ 16 * 2 ];
	int i,j = 0;
    
    DMSG("[Trustzone][_teePlayReadyUnshuffleContentKey]: Enter.\n");

	if (u4FromSize!= 32 ||  u4Count!= 32) {
        DMSG("u4FromSize u4Count illegal.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
	
    if (u4FromIdx + u4Count > u4FromSize) {
        DMSG("u4FromIdx + u4Count > u4FromSize.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* allocate */
    pu1SrcBuf = (BYTE *)malloc(u4FromSize);
    if(pu1SrcBuf == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }
    memset(pu1SrcBuf, 0, u4FromSize);

    pu1DstBuf = (BYTE *)malloc(u4Count);
    if(pu1DstBuf == NULL) {
        DMSG("malloc failed.\n");
        free(pu1SrcBuf);
        return TZ_ERROR_MEMORY;
    }
    memset(pu1DstBuf, 0, u4Count);

    /* unprotect Data */
#ifndef CC_TEE_FLOW_ENHANCEMENT		
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_decrypt(pu1From, pu1SrcBuf, u4FromSize, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(pu1From, pu1SrcBuf, u4FromSize);
#endif

    /* copy data from from to to */
    //memcpy(pu1DstBuf, pu1SrcBuf + u4FromIdx, u4Count);
    for ( i = 0; i < 16; i++ )
    {
        pu1DstBuf[ i ] = pu1SrcBuf[ j++ ];
        pu1DstBuf[ i + 16 ] = pu1SrcBuf[ j++ ];
    }
    /* protect Data */
#ifndef CC_TEE_FLOW_ENHANCEMENT		
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_encrypt(pu1DstBuf, pu1To, u4Count, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyProtect_main(pu1DstBuf, pu1To, u4Count);
#endif
    
    /* free */
    free(pu1SrcBuf);
    free(pu1DstBuf);

    return TZ_SUCCESS;
}

BYTE g_rgbMagicConstantZero[ 16 ] = {
    0x7e, 0xe9, 0xed, 0x4a, 0xf7, 0x73, 0x22, 0x4f,
    0x00, 0xb8, 0xea, 0x7e, 0xfb, 0x02, 0x7c, 0xbb };


UINT32 _teePlayReadyPrepareContentKey_Prime(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromIdx, UINT32 u4FromSize, UINT32 u4Count)
{
    symmetric_CTR t_sym_ctr;
    UINT8 *pu1SrcBuf = NULL, *pu1DstBuf = NULL;
	UINT8 *rgbTmp[ 16 * 2 ];
	symmetric_ECB t_sym_ecb;
	int i,j = 0;
    
    DMSG("[Trustzone][_teePlayReadyUnshuffleContentKey]: Enter.\n");

	if (u4FromSize!= 16 ||  u4Count!= 16) {
        DMSG("u4FromSize u4Count illegal.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
	
    if (u4FromIdx + u4Count > u4FromSize) {
        DMSG("u4FromIdx + u4Count > u4FromSize.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* allocate */
    pu1SrcBuf = (BYTE *)malloc(u4FromSize);
    if(pu1SrcBuf == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }
    memset(pu1SrcBuf, 0, u4FromSize);

    pu1DstBuf = (BYTE *)malloc(u4Count);
    if(pu1DstBuf == NULL) {
        DMSG("malloc failed.\n");
        free(pu1SrcBuf);
        return TZ_ERROR_MEMORY;
    }
    memset(pu1DstBuf, 0, u4Count);

    /* unprotect Data */
#ifndef CC_TEE_FLOW_ENHANCEMENT		
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_decrypt(pu1From, pu1SrcBuf, u4FromSize, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
		_teePlayReadyUnprotect_main(pu1From, pu1SrcBuf, u4FromSize);
#endif

    /* copy data from from to to */
    //memcpy(pu1DstBuf, pu1SrcBuf + u4FromIdx, u4Count);
    for ( i = 0; i < 16; i++ )
    {
        pu1DstBuf[ i ] = pu1SrcBuf[ i ] ^ g_rgbMagicConstantZero[ i ];
    }
    /* protect Data */
 //   ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
 //   ctr_encrypt(pu1DstBuf, pu1To, u4Count, &t_sym_ctr);
 //   ctr_done(&t_sym_ctr);
    ecb_start(find_cipher("aes"), pu1SrcBuf, TEE_PLAYREADY_AESECB_KEY_SIZE, 0, &t_sym_ecb);
    ecb_encrypt(pu1DstBuf, pu1To, TEE_PLAYREADY_AESECB_KEY_SIZE, &t_sym_ecb);
    ecb_done(&t_sym_ecb); 
    /* free */
    free(pu1SrcBuf);
    free(pu1DstBuf);

    return TZ_SUCCESS;
}


UINT32 _teePlayReadyLinkBytes(UINT8 *pu1To, UINT8 *pu1From, UINT32 u4FromSize, UINT8 *pu1From2, UINT32 u4FromSize2)
{
    symmetric_CTR t_sym_ctr;
    UINT8 *pu1SrcBuf1 = NULL, *pu1DstBuf = NULL;
	UINT8 *pu1SrcBuf2 = NULL;
    
    DMSG("[Trustzone][_teePlayReadyCopyBytes]: Enter.\n");

    if (u4FromSize == 0) {
        DMSG("u4FromSize is 0.\n");
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* allocate */
    pu1SrcBuf1 = (BYTE *)malloc(u4FromSize);
    if(pu1SrcBuf1 == NULL) {
        DMSG("malloc failed.\n");
        return TZ_ERROR_MEMORY;
    }

	pu1SrcBuf2 = (BYTE *)malloc(u4FromSize2);
	 if(pu1SrcBuf2 == NULL) {
        DMSG("malloc failed.\n");
		free(pu1SrcBuf1);
        return TZ_ERROR_MEMORY;
    }
	 
    memset(pu1SrcBuf1, 0, u4FromSize);
	memset(pu1SrcBuf2, 0, u4FromSize2);

    pu1DstBuf = (BYTE *)malloc(u4FromSize + u4FromSize2);
    if(pu1DstBuf == NULL) {
        DMSG("malloc failed.\n");
        free(pu1SrcBuf1);
		free(pu1SrcBuf2);
        return TZ_ERROR_MEMORY;
    }
    memset(pu1DstBuf, 0, u4FromSize + u4FromSize2);
#ifndef CC_TEE_FLOW_ENHANCEMENT	

    /* unprotect Data */
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_decrypt(pu1From, pu1SrcBuf1, u4FromSize, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
    _teeSecureStorageInit(&t_TEE_SS_SecretUserKey);
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_decrypt(pu1From2, pu1SrcBuf2, u4FromSize2, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyUnprotect_main(pu1From, pu1SrcBuf1, u4FromSize);
	_teePlayReadyUnprotect_main(pu1From2, pu1SrcBuf2, u4FromSize2);
#endif
    /* copy data from from to to */
    memcpy(pu1DstBuf, pu1SrcBuf1, u4FromSize);
	memcpy(pu1DstBuf+u4FromSize, pu1SrcBuf2, u4FromSize2);
#ifndef CC_TEE_FLOW_ENHANCEMENT	

    /* protect Data */
    ctr_start(find_cipher("aes"), au1_TEE_SS_iv, au1_TEE_SS_ksec, TEE_SECURESTORAGE_SEC_KEY_SIZE, 0, CTR_COUNTER_BIG_ENDIAN|LTC_CTR_RFC3686, &t_sym_ctr);
    ctr_encrypt(pu1DstBuf, pu1To, u4FromSize + u4FromSize2, &t_sym_ctr);
    ctr_done(&t_sym_ctr);
#else
	_teePlayReadyProtect_main(pu1DstBuf, pu1To, u4FromSize + u4FromSize2);
#endif
    /* free */
    free(pu1SrcBuf1);
	free(pu1SrcBuf2);
    free(pu1DstBuf);

    return TZ_SUCCESS;
}

#if 1 // securestopintee


#include "tzst.h"
#include "x_ckgen.h"
#include "x_chipreg.h"
#include "x_bim.h"

#define TIMER_HZ 27000000
static unsigned long boot_time = 0;
#define BIRTH_DATE 1401351194

typedef struct __TEE_LINUX_TIME {
    unsigned long boot_time;
	unsigned long update_time;
	unsigned long long update_counter;
} TEE_LINUX_TIME;

TEE_LINUX_TIME TEE_linuxtime_data = 
	{
            0,
            0,
            0
	};

static void dumpcurrent_securetime(void)
{
	/*unsigned long counter_offset = 0;
	unsigned long long current_count = 0;

	int i = 0;
	unsigned long high = BIM_READ32(REG_RW_TIMER2_HIGH);
	unsigned long low = BIM_READ32(REG_RW_TIMER2_LOW);
	DMSG("dumpcurrent_securetime:\n");
	TEE_DUMP("TEE_linuxtime_data***** ", sizeof(TEE_LINUX_TIME), (char*)&TEE_linuxtime_data);
	TEE_DUMP("high***** ", sizeof(unsigned long), (char*)&high);
	TEE_DUMP("low***** ", sizeof(unsigned long), (char*)&low);
	DMSG("current count high: %u  low: %u \n",high,low);
	current_count = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
	counter_offset = (unsigned long) (( TEE_linuxtime_data.update_counter - current_count )/ TIMER_HZ);
	DMSG("current offset: %u \n",counter_offset);*/
}

static unsigned long readlinuxtime()
{
	unsigned long counter = 0;
	_tee_antirollback_gettime(&counter);

	return counter;
}

BYTE au1_TEE_SECURECLOCK_SIGNATUREKEY[TEE_PLAYREADY_AESECB_KEY_SIZE] =
		{
            0x8B, 0x22, 0x2F, 0xFD, 0x1E, 0x76, 0x19, 0x56,
            0x59, 0xCF, 0x27, 0x03, 0x89, 0x8C, 0x42, 0x7F,
    };

UINT32 _teePlayReadyHmac_minor(UINT8 *pu1Data, UINT32 u4Len, UINT8 *pu1Hmac)
{
    hmac_state t_hmac_state;
    BYTE au1_digest[TEE_SECURESTORAGE_HASH_KEY_SIZE];
    UINT32 i;
    if (pu1Data == NULL || pu1Hmac == NULL)
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	
    if (u4Len % TEE_SECURESTORAGE_SEC_KEY_SIZE != 0) {
        DMSG("u4Len is not %d bytes alignment.\n", TEE_SECURESTORAGE_SEC_KEY_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
	register_hash(&sha256_desc);
	
    memset(au1_digest, 0, TEE_SECURESTORAGE_HASH_KEY_SIZE);
    //TEE_DUMP("au1_digest hmac input ***** ",u4Len , (char*)pu1Data);
    
    hmac_init(&t_hmac_state, find_hash("sha256"), au1_TEE_SECURECLOCK_SIGNATUREKEY, TEE_SECURESTORAGE_HASH_KEY_SIZE/2);
    hmac_process(&t_hmac_state, pu1Data, u4Len);
    hmac_done(&t_hmac_state, au1_digest, &i);
	
    //TEE_DUMP("au1_digest hmac ***** ",TEE_SECURESTORAGE_HASH_KEY_SIZE , (char*)au1_digest);
    memcpy(pu1Hmac, au1_digest + TEE_SECURESTORAGE_HASH_KEY_SIZE / 4, TEE_SECURESTORAGE_HASH_KEY_SIZE / 4);
    
    return TZ_SUCCESS;

}

int _tee_antirollback_gettime(unsigned long *time)
{
	unsigned long long current_count = 0;
	unsigned long counter_offset = 0;

	dumpcurrent_securetime();

	if (TEE_linuxtime_data.update_time == 0)
		{
		(*time) = BIRTH_DATE;

		return TZ_SUCCESS;

		}

	current_count = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
	if (TEE_linuxtime_data.update_time <  BIRTH_DATE)
		{
		(*time) = BIRTH_DATE;
		}
	else if (current_count < TEE_linuxtime_data.update_counter)
		{
		counter_offset = (unsigned long) ((TEE_linuxtime_data.update_counter - current_count)/ TIMER_HZ);

		(*time) = TEE_linuxtime_data.update_time + counter_offset;		
		}
	else
		{
		DMSG("This situation should not happen for counter in linux had been rollbacked.\n");
		(*time) = TEE_linuxtime_data.update_time;
		return TZ_ERROR_GENERIC;
		/*
		TEE_linuxtime_data.update_time = time;
		TEE_linuxtime_data.update_counter = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
		*/
		}
	return TZ_SUCCESS;

}


int _tee_antirollback_loadinclock(char* updatedclock , int updateclock_len)
{
	int i=0;
	symmetric_ECB t_sym_ecb;
	UINT8 pu1ProtectData[TEE_PLAYREADY_AESECB_KEY_SIZE];
	TEE_LINUX_TIME updated_time;
	unsigned char signature[8] = {0};

    if (updatedclock == NULL)
		return TEE_ERR_FAIL;
	
	dumpcurrent_securetime();

	if (updateclock_len < sizeof(TEE_LINUX_TIME) + 8)
		return TZ_ERROR_ACCESS_DENIED;

	memcpy_n2s_chk((char*) &updated_time,updatedclock,sizeof(TEE_LINUX_TIME),NULL);
	_teePlayReadyHmac_minor((char*) &updated_time,sizeof(TEE_LINUX_TIME),signature);

	//TEE_DUMP("load updated_time***** ", sizeof(TEE_LINUX_TIME), (char*)&updated_time);
	//TEE_DUMP("load signature***** ", 8, (char*)signature);

	if (memcmp(signature,updatedclock+sizeof(TEE_LINUX_TIME) , 8 ) == 0)
		{
		if (TEE_linuxtime_data.boot_time == 0 && updated_time.update_time > TEE_linuxtime_data.update_time)
		{
		TEE_linuxtime_data.boot_time = updated_time.update_time;
		TEE_linuxtime_data.update_time = updated_time.update_time;
		TEE_linuxtime_data.update_counter = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
		}
		return TEE_SUCCESS;
		}
	else
		return TEE_ERR_FAIL;

}

int _tee_antirollback_updateclock(unsigned long time , char* updatedclock , int updateclock_len)
{
	int i = 0;
    symmetric_ECB t_sym_ecb;
	//UINT8 pu1ProtectData[TEE_PLAYREADY_AESECB_KEY_SIZE];
	unsigned long long current_count = 0;
	unsigned long counter_offset = 0;
	unsigned char signature[8] = {0};

	//DMSG("input time was %u\n", time);
	if (updatedclock == NULL || (updateclock_len < sizeof(TEE_LINUX_TIME)+8))
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	
	dumpcurrent_securetime();	
	if (TEE_linuxtime_data.boot_time == 0) // first update clock without file
		{
		if (time > BIRTH_DATE)
			{
			TEE_linuxtime_data.boot_time = time;
			TEE_linuxtime_data.update_time = time;
			}
		else
			{
			TEE_linuxtime_data.boot_time = BIRTH_DATE;
			TEE_linuxtime_data.update_time = BIRTH_DATE;

			}
		TEE_linuxtime_data.update_counter = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
		}
	else if (time > TEE_linuxtime_data.update_time) //  increase the time to the bigger one between offset of counter and offset of time.
		{
		current_count = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
		if (current_count < TEE_linuxtime_data.update_counter)
			{
			counter_offset = (unsigned long) ((TEE_linuxtime_data.update_counter -  current_count)/ TIMER_HZ) ;
			TEE_linuxtime_data.update_counter = current_count;

			if (counter_offset > (time - TEE_linuxtime_data.update_time))
				{
				TEE_linuxtime_data.update_time += counter_offset;
				}
			else
				TEE_linuxtime_data.update_time = time;
				
			}
		else
			{
			DMSG("This situation should not happen for counter in linux had been rollbacked.\n");
			/*
			TEE_linuxtime_data.update_time = time;
			TEE_linuxtime_data.update_counter = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
			*/
			}
		
		}
	else  // system time was rollbacked but counter in Linux should not be rollbacked
		{
		current_count = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
		if (current_count < TEE_linuxtime_data.update_counter)
			{
			counter_offset = (unsigned long) (( TEE_linuxtime_data.update_counter - current_count )/ TIMER_HZ);
			TEE_linuxtime_data.update_counter = current_count;
			TEE_linuxtime_data.update_time += counter_offset;			
			}
		else
			{
			DMSG("This situation should not happen for counter in linux had been rollbacked.\n");
			}	
		}


	    DMSG("[Trustzone][_tee_antirollback_updateclock]: Enter.\n");

	   
	_teePlayReadyHmac_minor((unsigned char*)&TEE_linuxtime_data,sizeof(TEE_LINUX_TIME),signature);


	if (updateclock_len < sizeof(TEE_LINUX_TIME) + 8)
		return TZ_ERROR_ACCESS_DENIED;
	else
		{
		//TEE_DUMP("save updated_time***** ", sizeof(TEE_LINUX_TIME), (char*)&TEE_linuxtime_data);
		//TEE_DUMP("save signature***** ", 8, (char*)signature);
		memcpy(updatedclock,(unsigned char*)&TEE_linuxtime_data,sizeof(TEE_LINUX_TIME));
		memcpy(updatedclock + sizeof(TEE_LINUX_TIME),signature,8);
		}
	    
	    return TZ_SUCCESS;
}


void * _tee_malloc(size_t size) {
    return malloc(size);
}

void _tee_free(void * ptr) {
    if (ptr != NULL) free(ptr);
}

void * _tee_memcpy (void * destination, const void * source, size_t num ) {
    return memcpy(destination, source, num);
}

void * _tee_memset ( void * ptr, int value, size_t num ) {
    return memset(ptr, value, num);
}

#define PRINT_HEADER printf("****** %s: %s [%d]: ", func, header, inlen); 
/*
 * print binary data as sequence of hex
 */
int _print_hex_func(const char * const func, const char * const header, const unsigned char inbuf[], const int inlen) {
    PRINT_HEADER
    int i;
    for (i = 0; i < inlen; i++) {
        if ((i % 32) == 0 && i != 0) {
            printf("\n");
        }
        printf("%02x ", inbuf[i]);
    }
    printf("\n");
    return TEE_SUCCESS;
}

char ctostr(unsigned char input)
{
if (input > 0xF)
	return 0;
else if (input < 0xA)
	return (input + 0x30);
else
	return (input - 0xA + 'A');
}
int _get_hex(const unsigned char inbuf[], const int inlen, char outbuf[]) {
    int i;
    char * p = outbuf;
    for (i = 0; i < inlen; i++, p += 2) {
        //sprintf(p, "%02x ", inbuf[i]);
        *p = ctostr((0xF0 & inbuf[i]) >> 4);
		*(p+1) = ctostr(0x0F & inbuf[i]);
    }
    *p = '\0';
	return inlen;
}

/*
 * print binary data as sequence of hex or printable characters
 */
int _print_bin_func(const char * const func, const char * const header, const unsigned char * const inbuf, const int inlen) {
    PRINT_HEADER
    int i;
    for (i = 0; i < inlen; ++i) {
        if ((i % 32) == 0 && i != 0) {
            printf("\n");
        }
        printf("%02x ", inbuf[i]);
    }
    printf("\n");
    return TEE_SUCCESS;
}

/*
 * print ascii data
 */
int _print_txt_func(const char * const func, const char * const header, const unsigned char * const inbuf, const int inlen) {
    PRINT_HEADER
    int i;
    for (i = 0; i < inlen; ++i) {
        if ((i % 96) == 0 && i != 0) {
            printf("\n");
        }
        printf("%c", inbuf[i]);
    }
    printf("\n");
    return TEE_SUCCESS;
}
#undef PRINT_HEADER

#define __print_secure_stop(x) __print_secure_stop_func(__func__, x)

void __print_secure_stop_func(const char * const header, const SECURE_STOP * const ss) {
    char id_str[2*TEE_SESSION_ID_LEN + 1];
    _get_hex(ss->session_id, TEE_SESSION_ID_LEN, id_str);
    char kid_str[2*TEE_KEY_ID_LEN + 1];
    _get_hex(ss->kid, TEE_KEY_ID_LEN, kid_str);
    printf("****** %s: sid=%s, kid=%s, v=%d, t=(%lx %lx %lx), dur=%ld s=%d\n",
        header, id_str, kid_str, ss->version,
        ss->start_time, ss->update_time, ss->auth_time,
        ss->update_time - ss->start_time, ss->stopped);
}


typedef struct {
    TEE_OBJ_TYPE type;
    int id;
    void* obj;
} TEE_OBJ;

#define MAX_NUM_TEE_OBJS 100
TEE_OBJ TEE_OBJS[MAX_NUM_TEE_OBJS];

#define MAX_NUM_SESSIONS TEE_MAX_NUM_SECURE_STOPS
TEE_SESSION TEE_SESSIONS[MAX_NUM_SESSIONS];

#define SECURE_STOP_VERSION 1

int lastHandleId;

/********securestop util*******************/
// session is active if its internal session id is set
int _is_active_session(const TEE_SESSION * const s) {
    return s->id != UNINIT_TYPE_ID;
}

// uninitialize session by un-setting its internal session id
void _uninit_session(TEE_SESSION * const s) {
    s->id = UNINIT_TYPE_ID;
}

// secure stop is initialized if its version is set to non-0 value
int _is_init_secure_stop(const SECURE_STOP * const ss) {
    return ss->version != 0;
}

// initialize secure stop by setting its version and session ID.
// set to null
int _init_secure_stop(SECURE_STOP * const ss, const unsigned char * const session_id) {
    ss->version = SECURE_STOP_VERSION;
    memcpy(ss->session_id, session_id, TEE_SESSION_ID_LEN);
    memset(ss->kid, 0, TEE_KEY_ID_LEN);
    ss->stopped = 0;
	return 0;
}

/*
 * calculate the encoding length of DER header for a single field of a given length
 */
int DER_GetHeaderLen(int inlen)
{
    int len = 1; // DER data type byte
    if (inlen <= 0x7f) {
        len++;
    } else if (inlen <= 0xff) {
        len += 2;
    } else if (inlen <= 0x7fff) {
        len += 3;
    } else { // bigger size not supported
        return -1;
    }
    return len;

}

unsigned char * DER_WriteHeader(unsigned char * p, int type, int inlen) {
    *p = (unsigned char)type;
    ++p;

    if (inlen <= 0x7f) {
        *p = (unsigned char)inlen; ++p;
    } else if (inlen <= 0xff) {
        *p = 0x81; ++p;
        *p = (unsigned char)(inlen & 0xff); ++p;
    } else if (inlen <= 0x7fff) {
        *p = 0x82; ++p;
        *p = (unsigned char)((inlen >> 8) & 0xff); ++p;
        *p = (unsigned char)(inlen & 0xff); ++p;
    } else { // bigger size not supported
        return NULL;
    }

    return p;
}


int _der_get_field_enc_len(int const inlen)
{
    return DER_GetHeaderLen(inlen) + inlen;
}

/*
 * calculate the encoding length for a single integer secure stop field
 */
int _der_get_int_field_enc_len(long const value)
{
    return _der_get_field_enc_len(4); // assume for now that time stamp always takes 4 bytes
}

/*
 * write DER OCTET STRING
 */
unsigned char * _der_write_octets(unsigned char * p, unsigned char * inbuf, int inlen)
{
	if(NULL == p)
		return NULL;


    p = DER_WriteHeader(p, DER_OCTETS, inlen);

	if(NULL == p)
		return NULL;

	
    _tee_memcpy(p, inbuf, inlen);

	if(NULL == p)
		return NULL;
	else
	    return p + inlen;			
}

/*
 * write DER INTEGER. Remove the first bytes equal to 0x00 (at least 1 byte must be retained).
 */
unsigned char * _der_write_int(unsigned char * p, long val)
{
    // convert bytes to BIG ENDIAN, and point at most significant byte
    val = INT32(val);
    unsigned char *p1 = (unsigned char *)&val;

    // skip bytes == 0x00 and find length for serialization
    int i;
    for (i = 0; i < 3; i++) {
        if ( (p1[0] == 0x00) && ((p1[1] & 0x80) == 0x00) ) ++p1;
        else break;
    }

    int len = 4 - i;
    p = DER_WriteHeader(p, DER_INTEGER, len);
    _tee_memcpy(p, p1, len);
    return p + len;
}

/*
 * calculate the combined length of all encoded fields of secure stop TBS data
 */
int __tee_get_secure_stop_fields_enc_len(SECURE_STOP * ss)
{
    return    _der_get_field_enc_len(1) // version
            + _der_get_field_enc_len(TEE_SESSION_ID_LEN) // NONCE
            + _der_get_field_enc_len(TEE_KEY_ID_LEN) // KID
            + _der_get_int_field_enc_len(ss->start_time) // start_time
            + _der_get_int_field_enc_len(ss->update_time) // update_time
            + _der_get_int_field_enc_len(ss->auth_time) // sign_time
            + _der_get_field_enc_len(1) // stopped
            ;
}
/*
 * calculate the length of the entire to-be-signed portion of secure stop
 */
int __tee_get_secure_stop_enc_len(SECURE_STOP * ss)
{
    return _der_get_field_enc_len(__tee_get_secure_stop_fields_enc_len(ss)); // including DER SEQUENCE header
}


/********securestop util*******************/

int _uninit_secure_stop(SECURE_STOP * const ss) {
    memset(ss, 0, sizeof(SECURE_STOP));
	return sizeof(SECURE_STOP);
}


// return session by session id
TEE_SESSION * __tee_get_session(const unsigned char * const session_id) {
    TEE_SESSION * session = NULL;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0)) {
            session = s;
            break;
        }
    }
    return session;
}

INT32 _tee_get_session(const unsigned char * const session_id, TEE_SESSION * outsession) {
	TEE_SESSION* tmpsession;
	
	if (outsession == NULL)
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	tmpsession = __tee_get_session(session_id);
	if(NULL != tmpsession)
	{
		memcpy(outsession, tmpsession, sizeof(TEE_SESSION));   
	}
	else
	{
		Printf("_tee_get_session tmpsession NULL");
		
	}
	return TZ_SUCCESS;
}
TEE_SESSION * __tee_get_session_by_kid_and_sessionid(const unsigned char * const session_id, const unsigned char * const kid) {
    TEE_SESSION * session = NULL;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0) && (memcmp(s->ss.kid, kid, TEE_KEY_ID_LEN) == 0)) {
            session = s;
            break;
        }
    }
    return session;
}

// return session by content KID
TEE_SESSION * __tee_get_session_by_kid(const unsigned char * const kid) {
    TEE_SESSION * session = NULL;
    int i;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i) {
        if (memcmp(TEE_SESSIONS[i].ss.kid, kid, TEE_KEY_ID_LEN) == 0) {
            session = &TEE_SESSIONS[i];
            break;
        }
    }
    return session;
}

INT32 _tee_get_session_by_kid(const unsigned char * const kid, TEE_SESSION * outsession) {
	TEE_SESSION* tmpsession;
	
	if (outsession == NULL)
		return TZ_ERROR_ILLEGAL_ARGUMENT;
	tmpsession = __tee_get_session_by_kid(kid);
	if(NULL != tmpsession)
	{
		memcpy(outsession, tmpsession, sizeof(TEE_SESSION));  
	}
	else
	{
		Printf("_tee_get_session_by_kid tmpsession NULL");
		
	}

	return TZ_SUCCESS;

}

/*********************
 * Session Lifecycle *
 *********************/

int nextSessionIndex = 0;

/*
 * add session with assigned ID
 */
int _tee_add_session(const unsigned char * const session_id) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    TEE_SESSION * s = &TEE_SESSIONS[nextSessionIndex % MAX_NUM_SESSIONS];
    s->id = nextSessionIndex++;
    s->key_id = UNINIT_TYPE_ID;
    _init_secure_stop(&s->ss, session_id);

    //Exit:
    DBG_EXIT;
    return status;
}

/*
 * cancel session with given ID
 */
int _tee_cancel_session(const unsigned char * const session_id) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    TEE_SESSION * s = __tee_get_session(session_id);
    if (s == NULL) RETURN(TEE_ERR_FAIL);
    _uninit_session(s);
    _uninit_secure_stop(&s->ss);

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * bind session ID to content KID at license storage time
 */
int _tee_bind_license(const unsigned char  * const session_id,
                      const unsigned char  * const kid)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    TEE_SESSION * s = __tee_get_session(session_id);
    if (s == NULL) {
        _print_hex("ERROR: unknown session: id ", session_id, TEE_SESSION_ID_LEN);
        goto Exit;
    }

    memcpy(s->ss.kid, kid, TEE_KEY_ID_LEN);

    s->ss.start_time = s->ss.update_time = s->ss.auth_time = 0L;

    __print_secure_stop(&s->ss);

Exit:
    DBG_EXIT;
    return status;
}

/*
 * bind session to content decryption key and content KID at playback start time
 */
int _tee_bind_session(const TEE_SECRET_KEY * const secKey ,
                      const unsigned char  * const kid, long update_time, const unsigned char  * const SessionID)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    TEE_SESSION * s = __tee_get_session_by_kid_and_sessionid(SessionID, kid); //__tee_get_session_by_kid(kid);
    if (s == NULL) {
        _print_hex("ERROR: unknown session: kid ", kid, TEE_KEY_ID_LEN);
        _print_hex("ERROR: unknown session: id ", SessionID, TEE_SESSION_ID_LEN);
        goto Exit;
    }

	/*  must be fix

    TEE_SECRET_KEY_IMPL* keyImpl = (TEE_SECRET_KEY_IMPL*)_tee_get_obj(SECRET_KEY_TYPE, secKey->id);
    if (keyImpl == NULL) {
        _dbg("ERROR: missing key: key-id %d\n", secKey->id);
        _print_hex("ERROR: missing key: kid", kid, TEE_KEY_ID_LEN);
        s = NULL;
        goto Exit;
    }
	*/

    s->key_id = secKey->id; // binding happens here
    /*  must be fix
    s->ss.start_time = s->ss.update_time = _get_drm_time();
    */
#ifndef USE_LINUXTIME_INTEE 
    s->ss.start_time = s->ss.update_time = update_time;
#else
	s->ss.start_time = s->ss.update_time = readlinuxtime();
#endif
    s->ss.auth_time = 0L;

    __print_secure_stop(&s->ss);

Exit:
    DBG_EXIT;
    return status;
}

/*
 * remove active session by decryption key id
 */
int _tee_remove_session(const int key_id) {
    int status = TEE_SUCCESS;
    //DBG_ENTER;

    int i;
    int found = 0;
    TEE_SESSION * s = &TEE_SESSIONS[0];;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s)) {
            if (s->key_id == key_id) {
                if (found) {
                    _print_hex("PANIC: duplicate session_id ", s->ss.session_id, TEE_SESSION_ID_LEN);
                    RETURN(TEE_ERR_FAIL);
                }
                _print_hex("session_id ", s->ss.session_id, TEE_SESSION_ID_LEN);
                _dbg("key_id: %d\n", s->key_id);
                _uninit_session(s);
                found = 1;
            }
        }
    }
    if (!found) {
        status = TEE_ERR_FAIL;
        goto Exit;
    }

    Exit:
    //DBG_EXIT;
    return status;
}

// remove all active sessions - normally, should never find any
int _tee_reset_sessions() {
    int status = TEE_SUCCESS;

    DBG_ENTER;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (s->id != UNINIT_TYPE_ID) {
            if (s->key_id != UNINIT_TYPE_ID) {
                _dbg("destroying session %d\n", s->id);
                _print_hex("destroying session", s->ss.session_id, TEE_SESSION_ID_LEN);
                s->key_id = UNINIT_TYPE_ID;
                
            }
        }
    }
    nextSessionIndex = 0;

    // Exit:
    DBG_EXIT;
    return status;
}

void __tee_write_secure_stop(SECURE_STOP   * const ss,
                             unsigned char * const outbuf,
                             int             const inlen)
{
	unsigned char *tmp = NULL;
	
    // get the length of all concatenated encodings of secure stop fields
    int fields_len = __tee_get_secure_stop_fields_enc_len(ss);

    // write DER SEQUENCE header
    unsigned char * p = DER_WriteHeader(outbuf, DER_SEQUENCE, fields_len);

	if(NULL == p)
	{
		return;
	}

    // write each field
    p = _der_write_int(p, 1L);
	
	if(NULL == p)
	{
		return;
	}
	
    p = _der_write_octets(p, ss->session_id, TEE_SESSION_ID_LEN);  //@@@@@@@@@@@@@@@@@@@@@@
	if(NULL == p)
	{
		return;
	}

	
    p = _der_write_octets(p, ss->kid, TEE_KEY_ID_LEN);
	if(NULL == p)
	{
		return;
	}
    p = _der_write_int(p, ss->start_time);
	if(NULL == p)
	{
		return;
	}
	
    p = _der_write_int(p, ss->update_time);
	if(NULL == p)
	{
		return;
	}

	
    p = _der_write_int(p, ss->auth_time);
	if(NULL == p)
	{
		return;
	}

	
    p = _der_write_int(p, ss->stopped);
	if(NULL == p)
	{
		return;
	}	

    if ((p - outbuf) != inlen) {
        _dbg("Serialization Error: %d vs %d\n", inlen, (p - outbuf));
    }
}

/*************************
 * Secure Stop Lifecycle *
 *************************/

/*
 * Initialize outstanding secure stops from persisted data
 */
int _tee_init_secure_stops(const unsigned char * const inbuf,
                                          int    const inlen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;

    _tee_memset(&TEE_SESSIONS, 0, sizeof(TEE_SESSIONS));

    if (inbuf != NULL) {
        if (inlen > 0) {
            int n = inlen / sizeof(SECURE_STOP);
            if (n > MAX_NUM_SESSIONS) {
                RETURN(TEE_ERR_FAIL);
            }
            if ((inlen % sizeof(SECURE_STOP)) != 0) {
                RETURN(TEE_ERR_FAIL);
            }
            _info("Secure Stops: %d\n", n);
            SECURE_STOP * ss = (SECURE_STOP*)inbuf;
            TEE_SESSION * s = &TEE_SESSIONS[0];
            for (nextSessionIndex = 0; nextSessionIndex < n; nextSessionIndex++, ss++, s++) {
                _tee_memcpy(&s->ss, ss, sizeof(SECURE_STOP));
                //s->id = s->key_id = UNINIT_TYPE_ID;
                s->id = nextSessionIndex;
				s->key_id = UNINIT_TYPE_ID;                
                __print_secure_stop(ss);
            }
        } else {
            _dbg("Secure Stop Data Too Small\n");
        }
    } else {
        _dbg("NULL Secure Stop Buffer\n");
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * Marshal all outstanding secure stops
 */
int _tee_get_secure_stops(unsigned char * const outbuf, int * const outlen) {
    DBG_ENTER;
    int status = TEE_SUCCESS;

    int num = 0;
    int i;
    TEE_SESSION * s = NULL;

    // get the number of secure stops, both outstanding and active
    s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_init_secure_stop(&s->ss)) ++num;
    }

    // return error if buffer too small
    int minlen = num * sizeof(SECURE_STOP);
    if (*outlen < minlen) {
        *outlen = minlen;
        status = TEE_ERR_BUFFERTOOSMALL;
        goto Exit;
    }

    // copy secure stops if buffer big enough
    s = &TEE_SESSIONS[0];
    SECURE_STOP * p = (SECURE_STOP *)outbuf;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_init_secure_stop(&s->ss)) {
            _tee_memcpy(p, &s->ss, sizeof(SECURE_STOP));
            __print_secure_stop(&s->ss);
            ++p;
        }
    }
    *outlen = (unsigned char *)p - outbuf;

    Exit:
    DBG_EXIT;
    return status;
}

int _tee_get_secure_stop_ids(unsigned char          session_id[][TEE_SESSION_ID_LEN],
                                       int * const outnum)
{
    DBG_ENTER;
    int status = TEE_SUCCESS;

    SECURE_STOP * ss = NULL;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    int out = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            ss = &s->ss;
            _tee_memcpy(session_id[out], ss->session_id, TEE_SESSION_ID_LEN);
            _print_hex("id", session_id[out], TEE_SESSION_ID_LEN);
            ++out;
        }
    }
    *outnum = out;

    //Exit:
    DBG_EXIT;
    return status;
}

/*
 * return marshalled outstanding secure stop by session ID.
 * "Outstanding" means secure stop with no active session.
 * Such secure stop can be returned for signing and giving to the application.
 */

int _tee_get_secure_stop(const unsigned char * const session_id,
                               unsigned char * const outSecureStop,
                                         int * const outSecureStopLen, long update_time , TEE_SIGNATURE * sigout)
{
    _print_hex("id", session_id, TEE_SESSION_ID_LEN);
 
    int status = TEE_SUCCESS;

    SECURE_STOP * ss = NULL;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            if (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0) {
                ss = &s->ss;
                __print_secure_stop(ss);
                break;
            }
        }
    }

    if (ss != NULL) {
		 /*  must be fix
        ss->auth_time = _get_drm_time();
        */
#ifndef USE_LINUXTIME_INTEE        
        ss->auth_time = update_time;
#else
		ss->auth_time = readlinuxtime();
#endif
        int len = __tee_get_secure_stop_enc_len(ss);
        if (*outSecureStopLen < len) {
            _dbg("Secure Stop Buffer Too Small: %d, should be %d\n", *outSecureStopLen, len);
            *outSecureStopLen = len;
            status = TEE_ERR_BUFFERTOOSMALL; // this flow is normal
            goto Exit;
        } else {
            __tee_write_secure_stop(ss, outSecureStop, *outSecureStopLen);
            *outSecureStopLen = len;
        }
    } else {
        RETURN(TEE_ERR_FAIL);
    }

    if (CRYPTO_ECDSA_Sign_P256(outSecureStop, *outSecureStopLen, au1_TEE_PLAYREADY_ECCP256Key_PrivSign,  sigout->data))
    {
        DMSG("secure stop CRYPTO_ECDSA_Sign_P256 fail!! \n");
        return TEE_ERR_FAIL;
    }

	sigout->length = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;


    Exit:
    DBG_EXIT;
    return status;
}

/*
 * Update secure stop with a given TEE key ID
 * Update only if more than SECURE_STOP_UPDATE_TIME_SEC elapsed since the last update.
 */

int _tee_update_secure_stop(const int key_id, int * const updated, long update_time) {
    int status = TEE_SUCCESS;
    //DBG_ENTER;

    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    SECURE_STOP * ss = NULL;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            if (s->key_id == key_id) {
                ss = &s->ss;
            }
        }
    }

    if (ss != NULL) {
		 /*  must be fix
        time_t t = _get_drm_time();
        */
#ifndef USE_LINUXTIME_INTEE   
    long t = update_time;
#else
	long t = readlinuxtime();
#endif
        if (t > (ss->update_time + SECURE_STOP_UPDATE_TIME_SEC)) {
            ss->update_time = t;
            __print_secure_stop(ss);
            *updated = 1;
        } else {
            *updated = 0;
        }
    }

    //DBG_EXIT;
    return status;
}

/*
 * return outstanding secure stop by session ID.
 * "Outstanding" means secure stop with no active session. Such secure stop can be returned for signing and giving to the application.
 */

int _tee_commit_secure_stop(const unsigned char * const session_id) {
    int status = TEE_SUCCESS;
    DBG_ENTER;

    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    int found = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            if (found) {
                _print_hex("PANIC: duplicate session ID", session_id, TEE_SESSION_ID_LEN);
                RETURN(TEE_ERR_FAIL);
            }
            if (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0) {
                __print_secure_stop(&s->ss);
                _uninit_secure_stop(&s->ss);
                found = 1;
                break;
            }
        }
    }
    status = found ? TEE_SUCCESS : TEE_ERR_FAIL;

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * Clean secure stops
 */
int _tee_reset_secure_stops(int * const outNumRemoved) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    int numRemoved = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            __print_secure_stop(&s->ss);
            _uninit_secure_stop(&s->ss);
            ++numRemoved;
        }
    }

    // Exit:
    DBG_EXIT;
    *outNumRemoved = numRemoved;
    return status;
}

TEE_SESSION * __tee_get_session_by_seckey(const TEE_SECRET_KEY * const secKey) {
    TEE_SESSION * session = NULL;
    int i;
	int found = 0;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && (s->key_id == secKey->id)&&(s->ss.version != 0)&&(s->ss.stopped != 1) ) {
			if (found == 1)
			printf("[XAVIER]WARNING found multi session by keyid\n");
            session = s;
			found = 1;
        }
    }	
   // __print_secure_stop(&s->ss);
    return session;
}


int _tee_UnloadSecretKey(TEE_SECRET_KEY * const inSecretKey){
    int status = TEE_SUCCESS;
    DBG_ENTER;
	if (inSecretKey == NULL)
		return TEE_ERR_FAIL;
	TEE_SESSION * s = __tee_get_session_by_seckey(inSecretKey);
	if (s == NULL) {
		printf("ERROR: unknown SecretKey: %d \n", inSecretKey->id);
		goto Exit;
	}	
	
	s->ss.stopped= 1;
	
	__print_secure_stop(&s->ss);


    Exit:
    DBG_EXIT;
    return status;
}

#endif


