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
 * $RCSfile: tee_vudu.c,v $
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



#include <tomcrypt.h>
#define CC_TEE_FLOW_ENHANCEMENT
#define USE_LINUXTIME_INTEE
//#undef CC_TEE_STRICT_PROTECT_SUPPORT
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define AES_CBC  0
#define AES_CTR  1
#define AES_ECB  2
#define AES_ENCRYPT  0
#define AES_DESCRYPT 1
#define MTK_AES_ENCRYPT     0  // defined  in openssl/aes.h
#define MTK_AES_DECRYPT     1



#define AES_KEY_SIZE 16
#define AES_BLOCK_SIZE 16
#define NO_PADDING  0
#define PADDING 1

#define SFU_AES_KEY_LENGTH  (16)
#define SFU_RSA_KEY_LENGTH  (272)
#define VUDU_KEY_LENGTH     (16)
#define VUDU_IV_LENGTH      (16)

#define VUDU_COMMOM_ROOT_KEY_LENGTH      (16)

#define TS_PACKET_LENGTH    (188)

//#define AES_BLOCK_SIZE      (16)
#define DES_BLOCK_SIZE      (8)
#define VUDU_PID_1          0x12
#define VUDU_PID_2          0x13

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum{
    KEY_TYPE_IV = 0,
    KEY_TYPE_EVEN,
    KEY_TYPE_ODD
} KeyType;

typedef enum{
    ALG_NONE = 0,
    ALG_3DES_ABA_ECB = 2,
    ALG_3DES_ABA_CBC = 3,
    ALG_AES_128_CBC  = 4,
    ALG_AES_128_ECB  = 5,
} Algorithm;

typedef enum{
    DIR_PASSTHRU,
    DIR_ENCRYPT,
    DIR_DECRYPT
} Direction;




const UINT8 au1CommonSecret[] = {0x31, 0x5a, 0x2b, 0xe1, 0x42, 0x5d, 0xc1, 0x42,
							  0x14, 0x83, 0xa4, 0x91, 0x3a, 0x73, 0xe3, 0x03};
static UINT8 au1m2mCommonSecret[VUDU_COMMOM_ROOT_KEY_LENGTH];

static UINT8 au1CaCommonSecret[VUDU_COMMOM_ROOT_KEY_LENGTH];

static UINT8 u1VuduM2mAlg = 0;
static UINT8 u1VuduM2mDir = 0;
static UINT8 au1VuduM2mKey[VUDU_KEY_LENGTH];
static UINT8 au1VuduM2mIv[VUDU_KEY_LENGTH];
static UINT8 u1VuduM2mKeyLen = 16;
static UINT8 u1VuduM2mIvLen = 16;

static UINT8 s_caIv[16];
static UINT8 s_caEvenKey[16];
static UINT8 s_caOddKey[16];
static UINT8 u1VuduKeyIdx = 0;

#define CRYPTO_PRINT_INFO dprintf
#define CRYPTO_PRINT_SECF dprintf

void TEE_DUMMY_LOG(void)
{
	dprintf("not implementated!\n");
}




//-----------------------------------------------------------------------------

static void tee_DecryptAesCbcRbt(const UINT8* in,UINT8* out,UINT32 len,const UINT8 *iv, const UINT8 *key)
{
    UINT8 tmpin[AES_BLOCK_SIZE], tmpout[AES_BLOCK_SIZE], tmpiv[AES_BLOCK_SIZE];

    UINT32 remain_bytes    = len % AES_BLOCK_SIZE;
    UINT32 remain_offset   = len - remain_bytes;
	UINT32 i = 0;
#if 0
    memcpy(tmpiv, iv, AES_BLOCK_SIZE);

    AES_KEY aeskey;
    AES_set_decrypt_key(key, 16*8, &aeskey);
    AES_cbc_encrypt(in, out, len-remain_bytes, &aeskey, tmpiv, AES_DECRYPT);

    if (remain_bytes != 0)
	{
        if (len >= AES_BLOCK_SIZE)
		{
            memcpy(tmpin, &in[remain_offset-AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        }
		else
		{
            memcpy(tmpin, tmpiv, AES_BLOCK_SIZE);
        }

        AES_set_encrypt_key(key, 16*8, &aeskey);
        AES_ecb_encrypt(tmpin, tmpout, &aeskey, AES_ENCRYPT);

        for (i = 0 ; i < remain_bytes; i++)
		{
            out[remain_offset+i] = tmpout[i] ^ in[remain_offset+i];
        }
    }
#else
	memcpy(tmpiv, iv, AES_BLOCK_SIZE);
	aes_context rCtx;
	mtk_private_aes_setkey_dec(&rCtx,key,128);
	mtk_private_aes_crypt_cbc(&rCtx,MTK_AES_DECRYPT,len-remain_bytes,tmpiv,in,out);
	if (remain_bytes != 0)
	{
		if (len >= AES_BLOCK_SIZE)
		{
			memcpy(tmpin, &in[remain_offset-AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		}
		else
		{
			memcpy(tmpin, tmpiv, AES_BLOCK_SIZE);
		}
		mtk_private_aes_setkey_enc(&rCtx,key,128);
		mtk_private_aes_crypt_ecb(&rCtx,MTK_AES_ENCRYPT,tmpin,tmpout);
		for (i = 0 ; i < remain_bytes; i++)
		{
			out[remain_offset+i] = tmpout[i] ^ in[remain_offset+i];
		}
	}
#endif
}



BOOL tee_DecryptAes(UINT8 *in,
             UINT8 *out,
             UINT32 len,
             UINT8 *keyInput,
             UINT8 *ivInput,
             Direction direction, BOOL fgCbc)
{
    UINT32 l = len;
    aes_context rCtx;
    UINT8 pIvCopy[16];

    if ((l % AES_BLOCK_SIZE) != 0 ) {
        CRYPTO_PRINT_INFO("Buffer size is not multiple of %d", AES_BLOCK_SIZE);
        return FALSE;
    }

    switch ( direction ) {
    case DIR_ENCRYPT:
        mtk_private_aes_setkey_enc(&rCtx, keyInput, 128);
        break;

    case DIR_DECRYPT:
        mtk_private_aes_setkey_dec(&rCtx, keyInput, 128);
        break;

    default:
        return FALSE;
    }

    if(fgCbc)
    {
        // copy IV so AES_CBC doesn't modify it
        memcpy( pIvCopy, ivInput, 16 );
        mtk_private_aes_crypt_cbc(&rCtx, (direction == DIR_ENCRYPT)?MTK_AES_ENCRYPT:MTK_AES_DECRYPT,
            l, pIvCopy, in, out);
    }
    else
    {
        mtk_private_aes_crypt_ecb_with_len(&rCtx, (direction == DIR_ENCRYPT)?MTK_AES_ENCRYPT:MTK_AES_DECRYPT,
            l, in, out);
    }

    return TRUE;
}


BOOL tee_Decrypt3Des(UINT8 *in,
             UINT8 *out,
             UINT32 len,
             UINT8 *keyInput,
             UINT8 *ivInput,
             Direction direction, BOOL fgCbc)
{
    UINT32 l = len;
    DES3_KS r3DesKs;
    UINT8 pIvCopy[8];
    UINT8 keyBuf[24];

    if ((l % DES_BLOCK_SIZE) != 0 ) {
        CRYPTO_PRINT_INFO("Buffer size is not multiple of %d", DES_BLOCK_SIZE);
        return FALSE;
    }

    memcpy(keyBuf, keyInput, 8);
    memcpy(keyBuf+8, keyInput+8, 8);
    memcpy(keyBuf+16, keyInput, 8);

    switch ( direction ) {
    case DIR_ENCRYPT:
        des3key(r3DesKs, keyBuf, 0);
        break;

    case DIR_DECRYPT:
        des3key(r3DesKs, keyBuf, 1);
        break;

    default:
        return FALSE;
    }

    if(fgCbc)
    {
        // copy IV so AES_CBC doesn't modify it
        memcpy( pIvCopy, ivInput, 8 );
        mtk_private_3des_crypt_cbc(r3DesKs, (direction == DIR_ENCRYPT)?DES_ENCRYPT:DES_DECRYPT,
            l, pIvCopy, in, out);
    }
    else
    {
        mtk_private_3des_crypt_ecb(r3DesKs, (direction == DIR_ENCRYPT)?DES_ENCRYPT:DES_DECRYPT,
            l, in, out);
    }

    return TRUE;
}




static BOOL _tee_vudu_setkey(UINT8 u1Idx, BOOL fgEven, UINT8 *pu1Key)
{
    UINT8 i, *p;
    UINT32 au4Key[8];

	/* fix */
    CRYPTO_PRINT_INFO("key Even: %d\n", fgEven);
    for(i = 0; i < 16; i++)
    {
        CRYPTO_PRINT_INFO("0x%x", pu1Key[i]);
    }
    CRYPTO_PRINT_INFO("\n");

    for (i = 0; i < 8; i++)
    {
        p = pu1Key + (i * 4);
        au4Key[i] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    }
	

//    DMX_SetAesKey(u1Idx, fgEven, au4Key);
//    DMX_SetAesKeyLen(128);
//    DMX_SetRtbMode(0, MTDMX_DESC_RTB_MODE_SCTE52);  ?fix
	

    for(i = 0; i < 32; i++)
    {
        //?MTDMX_SetFileDescramblPid(i, MTDMX_DESC_MODE_AES_CBC, u1Idx);
    }

    return TRUE;
}


static BOOL _tee_vudu_setiv(UINT8 u1Idx, BOOL fgEven, UINT8 *pu1Iv)
{
    UINT8 i, j;
    UINT32 au4Iv[8];

    CRYPTO_PRINT_INFO("Iv Even: %d\n", fgEven);
    for(i = 0; i < 16; i++)
    {
        CRYPTO_PRINT_INFO("0x%x", pu1Iv[i]);
    }
    CRYPTO_PRINT_INFO("\n");

    for (i = 0; i < 4; i++)
    {
        au4Iv[i] = 0;

        for (j = 0; j < 4; j++)
        {
            au4Iv[i] |= (pu1Iv[(i * 4) + j] << ((3 - j) * 8));
        }
    }

	// fix
    //DMX_SetAesIV(0, TRUE, au4Iv);
    //DMX_SetAesIV(0, FALSE, au4Iv);

}



static BOOL _tee_vudu_unwrap_key(const UINT8 * uniqueKey, UINT32 len, const UINT8 *wrapped, UINT8 *unwrapped)
{
    DES3_KS ks;

    // unwrap unique key using common key into key A
    UINT8 keyA[16];
    UINT8 keyBuf[24];
    UINT8 au1Temp[32];
    UINT32 i;

    if (len > 16)
    {
        return FALSE;
    }

    if(uniqueKey == NULL)
    {
        return FALSE;
    }

    memcpy(keyBuf, au1CommonSecret+8, 8);
    memcpy(keyBuf+8, au1CommonSecret, 8);
    memcpy(keyBuf+16, au1CommonSecret+8, 8);
    memcpy(au1Temp, uniqueKey, 16);
    for (i = 0; i < 2; i++)
    {
        des3key(ks, keyBuf, 0);
        des3(ks, au1Temp + i * 8);
    }
    memcpy(keyA, au1Temp, 16);

    memcpy(keyBuf, keyA+8, 8);
    memcpy(keyBuf+8, keyA, 8);
    memcpy(keyBuf+16, keyA+8, 8);

    memcpy(au1Temp, wrapped, len);
    // unwrap content key with key A
    for (i = 0; i < len/8; i++) {
        des3key(ks, keyBuf, 0);
        des3(ks, au1Temp + i * 8);
    }
    memcpy(unwrapped, au1Temp, len);

    return TRUE;
}

static BOOL _tee_vudu_m2m_unwrap_key(const UINT8 * uniqueKey, UINT32 len, const UINT8 *wrapped, UINT8 *unwrapped)
{
    DES3_KS ks;

    // unwrap unique key using common key into key A
    UINT8 keyA[16];
    UINT8 keyBuf[24];
    UINT8 au1Temp[32];
    UINT32 i;

    if (len > 16)
    {
        return FALSE;
    }

    if(uniqueKey == NULL)
    {
        return FALSE;
    }

    memcpy(keyBuf, au1m2mCommonSecret+8, 8);
    memcpy(keyBuf+8, au1m2mCommonSecret, 8);
    memcpy(keyBuf+16, au1m2mCommonSecret+8, 8);
    memcpy(au1Temp, uniqueKey, 16);
    for (i = 0; i < 2; i++)
    {
        des3key(ks, keyBuf, 0);
        des3(ks, au1Temp + i * 8);
    }
    memcpy(keyA, au1Temp, 16);

    memcpy(keyBuf, keyA+8, 8);
    memcpy(keyBuf+8, keyA, 8);
    memcpy(keyBuf+16, keyA+8, 8);

    memcpy(au1Temp, wrapped, len);
    // unwrap content key with key A
    for (i = 0; i < len/8; i++) {
        des3key(ks, keyBuf, 0);
        des3(ks, au1Temp + i * 8);
    }
    memcpy(unwrapped, au1Temp, len);

    return TRUE;
}

static BOOL _tee_vudu_ca_unwrap_key(const UINT8 * uniqueKey, UINT32 len, const UINT8 *wrapped, UINT8 *unwrapped)
{
    DES3_KS ks;

    // unwrap unique key using common key into key A
    UINT8 keyA[16];
    UINT8 keyBuf[24];
    UINT8 au1Temp[32];
    UINT32 i;

    if (len > 16)
    {
        return FALSE;
    }

    if(uniqueKey == NULL)
    {
        return FALSE;
    }

    memcpy(keyBuf, au1CaCommonSecret+8, 8);
    memcpy(keyBuf+8, au1CaCommonSecret, 8);
    memcpy(keyBuf+16, au1CaCommonSecret+8, 8);
    memcpy(au1Temp, uniqueKey, 16);
    for (i = 0; i < 2; i++)
    {
        des3key(ks, keyBuf, 0);
        des3(ks, au1Temp + i * 8);
    }
    memcpy(keyA, au1Temp, 16);

    memcpy(keyBuf, keyA+8, 8);
    memcpy(keyBuf+8, keyA, 8);
    memcpy(keyBuf+16, keyA+8, 8);

    memcpy(au1Temp, wrapped, len);
    // unwrap content key with key A
    for (i = 0; i < len/8; i++) {
        des3key(ks, keyBuf, 0);
        des3(ks, au1Temp + i * 8);
    }
    memcpy(unwrapped, au1Temp, len);

    return TRUE;
}

/* Vudu */
UINT32 _tee_CRYPTO_VUDU_OpenM2mKeySlot(void)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_CloseM2mKeySlot(void)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_SelectM2mKeyAlgorithm(UINT8 alg, UINT8 dir)
{
    u1VuduM2mAlg = alg;
    u1VuduM2mDir = dir;

    CRYPTO_PRINT_INFO("VUDU Alg: %d, dir: %d", alg, dir);

    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_LoadClearM2mKey(const UINT32 keyLen, const UINT8 *pKeyData, const UINT32 ivLen, const UINT8 *pIvData)
{
    if(pKeyData == NULL || pIvData == NULL)
    {
        CRYPTO_PRINT_SECF("[%s:pKeyData and  pIvData is a null pointer]\n", __FUNCTION__);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    u1VuduM2mKeyLen = keyLen;
    memcpy(au1VuduM2mKey, pKeyData, keyLen);

    if(pIvData != NULL)
    {
        u1VuduM2mIvLen = ivLen;
        memcpy(au1VuduM2mIv, pIvData, ivLen);
    }

    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_LoadWrappedM2mKey(const UINT32 keyLen, const UINT8 *pKey3Data, const UINT8 *pKey4Data, const UINT8 *pIvData)
{
    if(pKey3Data == NULL || pKey4Data == NULL)
    {
        CRYPTO_PRINT_SECF("[%s:One or more parrameters are null pointers]\n", __FUNCTION__);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    u1VuduM2mKeyLen = keyLen;
    //_tee_vudu_unwrap_key(pKey3Data, keyLen, pKey4Data, au1VuduM2mKey);
    _tee_vudu_m2m_unwrap_key(pKey3Data, keyLen, pKey4Data, au1VuduM2mKey);
    
    CRYPTO_PRINT_SECF("[TZ]keyLen = %d ,au1VuduM2mKey =  \n", keyLen);
    int i;
    for(i=0; i<VUDU_KEY_LENGTH; i++)
         CRYPTO_PRINT_SECF(" %02x ", au1VuduM2mKey[i]);
    CRYPTO_PRINT_SECF(" \n");

    if(pIvData != NULL)
    {
        u1VuduM2mIvLen = 16;
        //_tee_vudu_unwrap_key(pKey3Data, 16, pIvData, au1VuduM2mIv);
        _tee_vudu_m2m_unwrap_key(pKey3Data, 16, pIvData, au1VuduM2mIv);

        CRYPTO_PRINT_SECF("[TZ]keyLen = 16 ,au1VuduM2mIv =  \n");
        for(i=0; i<16; i++)
         CRYPTO_PRINT_SECF(" %02x ", au1VuduM2mIv[i]);
        CRYPTO_PRINT_SECF(" \n");
    }

    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_LoadWrappedM2mKeyClearIv(const UINT32 keyLen, 	const UINT8 *pKey3Data, const UINT8 *pKey4Data, const UINT32 ivLen, const UINT8 *pIvData)
{
    if(pKey3Data == NULL || pKey4Data == NULL)
    {
        CRYPTO_PRINT_SECF("[%s:One or more parrameters are null pointers]\n", __FUNCTION__);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    u1VuduM2mKeyLen = keyLen;
    //_tee_vudu_unwrap_key(pKey3Data, keyLen, pKey4Data, au1VuduM2mKey);
    _tee_vudu_m2m_unwrap_key(pKey3Data, keyLen, pKey4Data, au1VuduM2mKey);

    CRYPTO_PRINT_SECF("[TZ]keyLen = %d ,au1VuduM2mKey =  \n", keyLen);
    int i;
    for(i=0; i<keyLen; i++)
         CRYPTO_PRINT_SECF(" %02x ", au1VuduM2mKey[i]);
    CRYPTO_PRINT_SECF(" \n");

    if(pIvData != NULL)
    {
        u1VuduM2mIvLen = ivLen;
        memcpy(au1VuduM2mIv, pIvData, ivLen);
       CRYPTO_PRINT_SECF("[TZ]ivLen = %d ,au1VuduM2mIv =  \n", ivLen);
        for(i=0; i<ivLen; i++)
         CRYPTO_PRINT_SECF(" %02x \n", au1VuduM2mIv[i]);
    CRYPTO_PRINT_SECF(" \n");
    }

    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_RunM2mKey(const UINT32 length, const UINT8 *pSrc, UINT8 *pDst)
{
    UINT8 *pu1Src;
    UINT32 l;
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    if(pSrc == NULL || pDst == NULL)
    {
        CRYPTO_PRINT_SECF("[%s:One or more parrameters are null pointers]\n", __FUNCTION__);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    pu1Src = (UINT8 *)pSrc;
    l = length;

    if(u1VuduM2mAlg == ALG_AES_128_CBC || u1VuduM2mAlg == ALG_AES_128_ECB)
    {
        tee_DecryptAes(pu1Src, pDst, l, au1VuduM2mKey, au1VuduM2mIv, u1VuduM2mDir,
            (u1VuduM2mAlg == ALG_AES_128_CBC)?TRUE:FALSE);
    }
    else if(u1VuduM2mAlg == ALG_3DES_ABA_ECB || u1VuduM2mAlg == ALG_3DES_ABA_CBC)
    {
        tee_Decrypt3Des(pu1Src, pDst, l, au1VuduM2mKey, au1VuduM2mIv, u1VuduM2mDir,
            (u1VuduM2mAlg == ALG_3DES_ABA_CBC)?TRUE:FALSE);
    }
    else if(u1VuduM2mAlg == ALG_NONE || u1VuduM2mDir == DIR_PASSTHRU)
    {
        memcpy(pDst, pSrc, length);
    }
    else
    {
        CRYPTO_PRINT_INFO("%s, what is this case??", __FUNCTION__);
    }

    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_OpenCAKeySlotHandle(void)
{
    // alloc key slot
    u1VuduKeyIdx = 0;
    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_CloseCAKeySlotHandle(void)
{
    // free key slot
    u1VuduKeyIdx = 0;
    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_GetCAKeySlotHandle(void)
{
    // assign again
    u1VuduKeyIdx = 0;
    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_SetWrappedCAKey(const UINT8 *pKey, const UINT8 *pUniqueKey, UINT8 keyType)
{
    BOOL fgEven;
    UINT8 au1Key[16];
    CRYPTO_PRINT_SECF("[TZ]in %s, keyType = %d\n", __FUNCTION__, keyType);

    if(keyType == KEY_TYPE_IV)
    {
        //_tee_vudu_unwrap_key(pUniqueKey, 16, pKey, au1Key);
        _tee_vudu_ca_unwrap_key(pUniqueKey, 16, pKey, au1Key);
		memcpy(s_caIv,au1Key,16);
        _tee_vudu_setiv(u1VuduKeyIdx, TRUE, au1Key);
    }
    else
    {
        fgEven = (keyType == KEY_TYPE_ODD)?FALSE:TRUE;
        //_tee_vudu_unwrap_key(pUniqueKey, 16, pKey, au1Key);
        _tee_vudu_ca_unwrap_key(pUniqueKey, 16, pKey, au1Key);

		if(fgEven == TRUE)
		{
		 memcpy(s_caEvenKey,au1Key,16);
		}
		else
		{
		 memcpy(s_caOddKey,au1Key,16);
		}
        _tee_vudu_setkey(u1VuduKeyIdx, fgEven, au1Key);
    }

    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_SetWrappedAllCAKey(const UINT8 *pOddKey, const UINT8 *pEvenKey, const UINT8 *pUniqueKey)
{
    UINT8 au1Key[16];
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    //_tee_vudu_unwrap_key(pUniqueKey, 16, pOddKey, au1Key);
    _tee_vudu_ca_unwrap_key(pUniqueKey, 16, pOddKey, au1Key);

    memcpy(s_caOddKey, au1Key, 16);
    _tee_vudu_setkey(u1VuduKeyIdx, FALSE, au1Key);
    //_tee_vudu_unwrap_key(pUniqueKey, 16, pEvenKey, au1Key);
    _tee_vudu_ca_unwrap_key(pUniqueKey, 16, pEvenKey, au1Key);

    memcpy(s_caEvenKey, au1Key, 16);
    _tee_vudu_setkey(u1VuduKeyIdx, TRUE, au1Key);

    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_LoadCAClearKey(const UINT8 *pKey, UINT8 keyType)
{
    BOOL fgEven;;
    CRYPTO_PRINT_SECF("[TZ]in %s, keyType = %d\n", __FUNCTION__, keyType);

    if(keyType == KEY_TYPE_IV)
    {
		memcpy(s_caIv, pKey, 16);
        _tee_vudu_setiv(u1VuduKeyIdx, TRUE, (UINT8 *)pKey);
    }
    else
    {
        fgEven = (keyType == KEY_TYPE_ODD)?FALSE:TRUE;
		if(fgEven == TRUE)
		{
            memcpy(s_caEvenKey, pKey, 16);
		}
		else
		{
            memcpy(s_caOddKey, pKey, 16);
		}
        _tee_vudu_setkey(u1VuduKeyIdx, fgEven, (UINT8 *)pKey);
    }

    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_StartTSDecrypt(const UINT32 srcLength, const UINT8 *pSrc, UINT32 *pDstLength, UINT8 *pDst)
{
	/* Z S BU ZUO */
#ifdef CC_USE_HW_TSDESC
        CRYPTO_PRINT_SECF("[TZ]in %s, line is 725\n", __FUNCTION__);

		static UINT32 u4UserBaseBuf, u4KernelBaseBuf, u4BaseSize;
		static BOOL fgInited = FALSE;
		MTDMX_GCPU_TSDESC_PARAM_T rParam;
		UINT32 u4SrcBufStart, u4DesBufStart, u4KerSrcBufStart, u4KerDesBufStart, i, j;

		if(!fgInited)
		{
			_Mtdrec_GetUserBaseAddr(&u4UserBaseBuf, &u4KernelBaseBuf, &u4BaseSize);
		}

		u4SrcBufStart = u4UserBaseBuf;
		u4DesBufStart = u4UserBaseBuf + 0x100000;
		u4KerSrcBufStart = u4KernelBaseBuf;
		u4KerDesBufStart = u4KernelBaseBuf + 0x100000;
		memcpy((void *)u4SrcBufStart, pSrc, srcLength);

		rParam.u4SrcStartAddr = u4KerSrcBufStart;
		rParam.u4SrcBufStart = MTDMX_GCPU_LINER_BUFFER_START((UINT32)u4KerSrcBufStart);
		rParam.u4SrcBufEnd = MTDMX_GCPU_LINER_BUFFER_END((UINT32)(u4KerSrcBufStart + srcLength));
		rParam.u4DstStartAddr = u4KerDesBufStart;
		rParam.u4DstBufStart = MTDMX_GCPU_LINER_BUFFER_START((UINT32)u4KerDesBufStart);
		rParam.u4DstBufEnd = MTDMX_GCPU_LINER_BUFFER_END((UINT32)(u4KerDesBufStart + srcLength));
		rParam.u4DatLen = srcLength;
		rParam.eForm = MTDMX_TSDESC_188_PACKET_FORM;
		rParam.eMode = MTDMX_TSDESC_FIRST_PACKET_MODE;
		rParam.eScheme = MTDMX_TSDESC_DESC_SCHEME_CBC_SCTE52;
		rParam.u4TsState[0] = 0;
		rParam.u4TsState[1] = 0;
		rParam.u4TsState[2] = 0;
		rParam.u4TsState[3] = 0;

		for(i = 0; i < 16; i++)
		{
			rParam.au1Key[0][i] = s_caEvenKey[i];
		}

		for(i = 0; i < 16; i++)
		{
			rParam.au1Key[1][i] = s_caOddKey[i];
		}

		for(i = 0; i < 2; i++)
		{
			for(j =0; j < 16; j++)
			{
				rParam.au1Iv[i][j] = s_caIv[j];
			}
		}

		if(MTDMX_GCPU_Cmd(0, MTDMX_GCPU_TSDESC, &rParam, sizeof(rParam), FALSE) != MTR_TZ_SUCCESS)
		{
			printf("GCPU command error\n");
		}

		memcpy(pDst, (void *)u4DesBufStart, srcLength);

		*pDstLength = srcLength;

		UNUSED(tee_DecryptAesCbcRbt);

		return TZ_SUCCESS;
#else
		UINT32		pos = 0;
		const UINT8 *inpktstart;
		UINT8		*outpktstart;
		UINT32		adaptation_field_control, adaptation_field_length = 0;
		UINT32		curPid;
                  CRYPTO_PRINT_SECF("[TZ]in %s, line is 794\n", __FUNCTION__);

		while (pos < srcLength)
		{
			//check the sync byte
			if (pSrc[pos] != 0x47)
			{
				CRYPTO_PRINT_INFO("INVALID SYNC BYTE!!\n");
				while(pSrc[pos] != 0x47 && pos < srcLength)
					pos++;
			}

			inpktstart = pSrc + pos;
			outpktstart = pDst + pos;

			curPid = ((inpktstart[1]&0x1f)<<8) + inpktstart[2];

			if (curPid == VUDU_PID_1 || curPid == VUDU_PID_2)
			{
				adaptation_field_control = (inpktstart[3] & 0x30) >> 4;

				UINT32 remain = 0;
				// copy over TS header + adaptation_field
				if (adaptation_field_control == 1)
				{
					remain = TS_PACKET_LENGTH - 4;
					memcpy(pDst+pos, pSrc+pos, 4);
					pos += 4;
				}
				else if (adaptation_field_control == 2)
				{
					remain = 0;
					memcpy(pDst+pos, pSrc+pos, TS_PACKET_LENGTH);
					pos += TS_PACKET_LENGTH;
					continue;
				}
				else if (adaptation_field_control == 3)
				{
					adaptation_field_length = inpktstart[4];
					UINT32 adaptation_field_copied = 4 + 1 + adaptation_field_length;
					remain = TS_PACKET_LENGTH - adaptation_field_copied;
					memcpy(pDst+pos, pSrc+pos, adaptation_field_copied);
					pos += adaptation_field_copied;
				}
				else
				{
					CRYPTO_PRINT_INFO("Invalid value '%d' in adaption field control\n", adaptation_field_control);
					return TZ_ERROR_ILLEGAL_ARGUMENT;
				}

				UINT8 scctl = inpktstart[3] & 0xc0;

				// overwrite transport_scrambling_control
				outpktstart[3] = outpktstart[3] & 0x3f;
				UINT8 *key = 0;
				if (scctl == 0x80)
				{
					key = s_caEvenKey;
				}
				else if (scctl == 0xc0)
				{
					key = s_caOddKey;
				}

				if(key)
				{
					// decrypt payload
					tee_DecryptAesCbcRbt(pSrc+pos, pDst+pos, remain, s_caIv, key);
				}
				else
				{
				   // CRYPTO_PRINT_INFO("No key/not encrypted, passing through data");
					memcpy(pDst+pos, pSrc+pos, remain);
				}
				pos += remain;
			}
			else
			{
				memcpy(pDst+pos, pSrc+pos, TS_PACKET_LENGTH);
				pos += TS_PACKET_LENGTH;
			}
		}

		*pDstLength = srcLength;

		return TZ_SUCCESS;
#endif

}

UINT32 _tee_CRYPTO_VUDU_SetOddEvenCAKey(const UINT8 *pOddKey, const UINT8 *pEvenKey, UINT8 keyType)
{
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    memcpy(s_caEvenKey, pEvenKey, 16);
    _tee_vudu_setkey(u1VuduKeyIdx, TRUE, (UINT8 *)pEvenKey);
    memcpy(s_caOddKey, pOddKey, 16);
    _tee_vudu_setkey(u1VuduKeyIdx, FALSE, (UINT8 *)pOddKey);

    return TZ_SUCCESS;

}


UINT32 _tee_CRYPTO_VUDU_SVP_Init(UINT32 base, int size)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;

}

UINT32 _tee_CRYPTO_VUDU_SVP_TSDecrypt(const UINT32 srcLength, const UINT8 *pSrc, UINT32 *pDstLength, UINT8 *pDst)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;
}

UINT32 _tee_CRYPTO_VUDU_SVP_GetAU(unsigned int *pAuAddr, int *pAuLength, int *pAuType,
	unsigned int *pWPtr, unsigned int rPtr, unsigned long long *pPTS)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;	
}

UINT32 _tee_CRYPTO_VUDU_SVP_Flush(void)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;	
}

UINT32 _tee_CRYPTO_VUDU_SVP_Release(void)
{
	TEE_DUMMY_LOG();
    return TZ_SUCCESS;	
}

UINT32 _tee_CRYPTO_VUDU_SetM2mkey(const UINT32 keyLen, const UINT8 *pKeyData)
{
    if(pKeyData == NULL )
    {
        CRYPTO_PRINT_SECF("[%s:pKeyData is a null pointer]\n", __FUNCTION__);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);
    memset(au1m2mCommonSecret,0,VUDU_COMMOM_ROOT_KEY_LENGTH);
    if(VUDU_COMMOM_ROOT_KEY_LENGTH < keyLen)
    {
        memcpy(au1m2mCommonSecret, pKeyData, VUDU_COMMOM_ROOT_KEY_LENGTH);
    }
    else
    {
        memcpy(au1m2mCommonSecret, pKeyData, keyLen);
    }

    return TZ_SUCCESS;  

}

UINT32 _tee_CRYPTO_VUDU_SetCAkey(const UINT32 keyLen, const UINT8 *pKeyData)
{
    if(pKeyData == NULL )
    {
        CRYPTO_PRINT_SECF("[%s:pKeyData is a null pointer]\n", __FUNCTION__);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }
    CRYPTO_PRINT_SECF("[TZ]in %s\n", __FUNCTION__);

    memset(au1CaCommonSecret,0,VUDU_COMMOM_ROOT_KEY_LENGTH);
    if(VUDU_COMMOM_ROOT_KEY_LENGTH < keyLen)
    {
        memcpy(au1CaCommonSecret, pKeyData, VUDU_COMMOM_ROOT_KEY_LENGTH);
    }
    else
    {
        memcpy(au1CaCommonSecret, pKeyData, keyLen);
    }
    return TZ_SUCCESS;

}

