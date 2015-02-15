/*****************************************************************
|
|   SKB Cipher - Generic Implementation
|
|   $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/tz_secure/drm/mtk_skb_driver/SkbGenericCipher.c#1 $
|   Original author: Julien Boeuf
|
|   This software is provided to you pursuant to your agreement
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms
|   of the agreement.
|
|   Copyright (c) 2005-2011 by Intertrust. All rights reserved.
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "SkbGenericSecureKeyBox.h"

/*----------------------------------------------------------------------
|   logger
+---------------------------------------------------------------------*/
MTK_SET_LOCAL_LOGGER("skb.generic.cipher")

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
static const SKB_Byte skb_zero_iv[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*----------------------------------------------------------------------
|   SKB_Cipher_Release
+---------------------------------------------------------------------*/
SKB_Result
SKB_Cipher_Release(SKB_Cipher* self)
{
    return self->iface->Release(self);
}

/*----------------------------------------------------------------------
|   SKB_Cipher_ProcessBuffer
+---------------------------------------------------------------------*/
SKB_Result
SKB_Cipher_ProcessBuffer(SKB_Cipher*     self,
                         const SKB_Byte* in_buffer,
                         SKB_Size        in_buffer_size,
                         SKB_Byte*       out_buffer,
                         SKB_Size*       out_buffer_size,
                         const SKB_Byte* iv,
                         SKB_Size        iv_size)
{
    if (self == NULL || in_buffer == NULL || in_buffer_size == 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

	dprintf("@@@SKB_Cipher_ProcessBuffer@@@\n");
	dprintf(">> %s:\n", __FUNCTION__);
	
    return self->iface->ProcessBuffer(self,
                                      in_buffer,
                                      in_buffer_size,
                                      out_buffer,
                                      out_buffer_size,
                                      iv,
                                      iv_size);
}

/*----------------------------------------------------------------------
|   SKB_AesCbcCipher_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesCbcCipher_Release(SKB_Cipher* _self)
{
    SKB_AesCbcCipher* self = (SKB_AesCbcCipher*)_self;
    cbc_done(&self->cbc);
    MTK_FreeMemory(self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesCbcCipher_ProcessBuffer
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesCbcCipher_ProcessBuffer(SKB_Cipher*     _self,
                               const SKB_Byte* in_buffer,
                               SKB_Size        in_buffer_size,
                               SKB_Byte*       out_buffer,
                               SKB_Size*       out_buffer_size,
                               const SKB_Byte* iv,
                               SKB_Size        iv_size)
{
    SKB_AesCbcCipher* self = (SKB_AesCbcCipher*)_self;

	dprintf(">> %s:\n", __FUNCTION__);


    /* check sizes */
    if (in_buffer_size%16 != 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
    if (out_buffer == NULL) {
        *out_buffer_size = in_buffer_size;
        return SKB_SUCCESS;
    }
    if (*out_buffer_size < in_buffer_size) {
        *out_buffer_size = in_buffer_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }

    /* set iv (default one if needed) */
    if (iv == NULL) {
        iv = skb_zero_iv;
    } else if (iv_size != 16) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    MTK_CHECK_WARNING(SKB_MapTomCryptResult(cbc_setiv(iv, 16, &self->cbc)));

	dprintf(">> %s 123 :\n", __FUNCTION__);


    /* encrypt or decrypt */
    *out_buffer_size = in_buffer_size;
    if (self->base.direction == SKB_CIPHER_DIRECTION_DECRYPT) {
        MTK_CHECK_WARNING(SKB_MapTomCryptResult(cbc_decrypt(in_buffer,
                                                            out_buffer,
                                                            in_buffer_size,
                                                            &self->cbc)));
    } else {
        MTK_CHECK_WARNING(SKB_MapTomCryptResult(cbc_encrypt(in_buffer,
                                                            out_buffer,
                                                            in_buffer_size,
                                                            &self->cbc)));
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesCbcCipher interface
+---------------------------------------------------------------------*/
static const SKB_CipherInterface
SKB_AesCbcCipherInterface = {
    SKB_AesCbcCipher_Release,
    SKB_AesCbcCipher_ProcessBuffer
};

/*----------------------------------------------------------------------
|   SKB_AesCtrCipher_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesCtrCipher_Release(SKB_Cipher* _self)
{
    SKB_AesCtrCipher* self = (SKB_AesCtrCipher*)_self;
    ctr_done(&self->ctr);
    MTK_FreeMemory(self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesCtrCipher_ProcessBuffer
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesCtrCipher_ProcessBuffer(SKB_Cipher*     _self,
                               const SKB_Byte* in_buffer,
                               SKB_Size        in_buffer_size,
                               SKB_Byte*       out_buffer,
                               SKB_Size*       out_buffer_size,
                               const SKB_Byte* iv,
                               SKB_Size        iv_size)
{
    SKB_AesCtrCipher* self = (SKB_AesCtrCipher*)_self;

	
	dprintf(">> %s:\n", __FUNCTION__);

    /* check sizes */
    if (out_buffer == NULL) {
        *out_buffer_size = in_buffer_size;
        return SKB_SUCCESS;
    }
    if (*out_buffer_size < in_buffer_size) {
        *out_buffer_size = in_buffer_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    *out_buffer_size = in_buffer_size;

    /* set iv (default one if needed) */
    if (iv == NULL) {
        iv = skb_zero_iv;
    } else if (iv_size != 16) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

	dprintf(">> %s 198:\n", __FUNCTION__);

    MTK_CHECK_WARNING(SKB_MapTomCryptResult(ctr_setiv(iv, 16, &self->ctr)));

    /* encrypt or decrypt */
    if (self->base.direction == SKB_CIPHER_DIRECTION_DECRYPT) {
        MTK_CHECK_WARNING(SKB_MapTomCryptResult(ctr_decrypt(in_buffer,
                                                            out_buffer,
                                                            in_buffer_size,
                                                            &self->ctr)));
    } else {
        MTK_CHECK_WARNING(SKB_MapTomCryptResult(ctr_encrypt(in_buffer,
                                                            out_buffer,
                                                            in_buffer_size,
                                                            &self->ctr)));
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesCtrCipher interface
+---------------------------------------------------------------------*/
static const SKB_CipherInterface
SKB_AesCtrCipherInterface = {
    SKB_AesCtrCipher_Release,
    SKB_AesCtrCipher_ProcessBuffer
};

/*----------------------------------------------------------------------
|   SKB_AesEcbCipher_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesEcbCipher_Release(SKB_Cipher* _self)
{
    SKB_AesEcbCipher* self = (SKB_AesEcbCipher*)_self;
    ecb_done(&self->ecb);
    MTK_FreeMemory(self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesEcbCipher_ProcessBuffer
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesEcbCipher_ProcessBuffer(SKB_Cipher*     _self,
                               const SKB_Byte* in_buffer,
                               SKB_Size        in_buffer_size,
                               SKB_Byte*       out_buffer,
                               SKB_Size*       out_buffer_size,
                               const SKB_Byte* iv,
                               SKB_Size        iv_size)
{
    SKB_AesEcbCipher* self = (SKB_AesEcbCipher*)_self;
	
	dprintf(">> %s:\n", __FUNCTION__);

    MTK_COMPILER_UNUSED(iv_size);

    /* check params */
    if (iv != NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* check sizes */
    if (in_buffer_size%16 != 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
    if (out_buffer == NULL) {
        *out_buffer_size = in_buffer_size;
        return SKB_SUCCESS;
    }
    if (*out_buffer_size < in_buffer_size) {
        *out_buffer_size = in_buffer_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    *out_buffer_size = in_buffer_size;

	dprintf(">> %s 275:\n", __FUNCTION__);

    /* encrypt or decrypt */
    if (self->base.direction == SKB_CIPHER_DIRECTION_DECRYPT) {
        MTK_CHECK_WARNING(SKB_MapTomCryptResult(ecb_decrypt(in_buffer,
                                                            out_buffer,
                                                            in_buffer_size,
                                                            &self->ecb)));
    } else {
        MTK_CHECK_WARNING(SKB_MapTomCryptResult(ecb_encrypt(in_buffer,
                                                            out_buffer,
                                                            in_buffer_size,
                                                            &self->ecb)));
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesEcbCipher interface
+---------------------------------------------------------------------*/
static const SKB_CipherInterface
SKB_AesEcbCipherInterface = {
    SKB_AesEcbCipher_Release,
    SKB_AesEcbCipher_ProcessBuffer
};

/*----------------------------------------------------------------------
|   SKB_RsaCipher_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaCipher_Release(SKB_Cipher* _self)
{
    MTK_FreeMemory(_self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaCipher_ProcessBuffer
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaCipher_ProcessBuffer(SKB_Cipher*     _self,
                            const SKB_Byte* in_buffer,
                            SKB_Size        in_buffer_size,
                            SKB_Byte*       out_buffer,
                            SKB_Size*       out_buffer_size,
                            const SKB_Byte* iv,
                            SKB_Size        iv_size)
{
    SKB_RsaCipher* self = (SKB_RsaCipher*)_self;
    int            stat;
    SKB_Result     result;
    unsigned long  decrypted_key_size = *out_buffer_size;
	
	dprintf(">> %s:\n", __FUNCTION__);

    MTK_COMPILER_UNUSED(iv_size);
    if (iv != NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* check the size */
    if (in_buffer_size != mp_unsigned_bin_size(self->key->N)) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
    if (out_buffer == NULL) {
        /* return a size we know will work */
        *out_buffer_size = in_buffer_size;
        return SKB_SUCCESS;
    }

	dprintf(">> %s 345:\n", __FUNCTION__);

    result = rsa_decrypt_key_ex(in_buffer,
                                in_buffer_size,
                                out_buffer,
                                &decrypted_key_size,
                                NULL,
                                0,
                                find_hash("sha1"),
                                self->padding,
                                &stat,
                                self->key);
    if (result == CRYPT_BUFFER_OVERFLOW) {
        *out_buffer_size = decrypted_key_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    if (result != CRYPT_OK || stat != 1) {
        MTK_LOG_WARNING("rsa decryption failed");
        return SKB_ERROR_INVALID_FORMAT;
    }
    *out_buffer_size = decrypted_key_size;
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaCipher interface
+---------------------------------------------------------------------*/
static const SKB_CipherInterface
SKB_RsaCipherInterface = {
    SKB_RsaCipher_Release,
    SKB_RsaCipher_ProcessBuffer
};


/*----------------------------------------------------------------------
|   SKB_RsaCipher_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaCipher_Create(SKB_CipherDirection   cipher_direction,
                     const SKB_SecureData* cipher_key,
                     SKB_CipherAlgorithm   cipher_algo,
                     SKB_Cipher**          cipher)
{
    SKB_RsaCipher* rsa_cipher;

    /* checks */
    MTK_ASSERT(cipher_algo == SKB_CIPHER_ALGORITHM_RSA_1_5 ||
               cipher_algo == SKB_CIPHER_ALGORITHM_RSA_OAEP);
    if (cipher_direction == SKB_CIPHER_DIRECTION_ENCRYPT) {
        MTK_LOG_WARNING("Encrpytion is a public key operation: not supported");
        return SKB_ERROR_NOT_SUPPORTED;
    }
    if (cipher_key->type != SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
        return MTK_ERROR_INVALID_PARAMETERS;
    }

    /* allocate */
    rsa_cipher = MTK_AllocateZeroMemory(sizeof(SKB_RsaCipher));
    if(rsa_cipher == NULL)
    {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    rsa_cipher->base.iface = &SKB_RsaCipherInterface;
    rsa_cipher->padding = cipher_algo == SKB_CIPHER_ALGORITHM_RSA_1_5?
                          LTC_PKCS_1_V1_5:LTC_PKCS_1_OAEP;
    rsa_cipher->key = &((SKB_RsaKeyData*)cipher_key)->key;
    *cipher = &rsa_cipher->base;
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_AesCtrCipher_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesCtrCipher_Create(const SKB_SecureData* cipher_key,
                        SKB_Cipher**          cipher)
{
    const SKB_Byte*   key;
    SKB_Result        result;
    SKB_AesCtrCipher* ctr_cipher = NULL;

    /* allocate memory and setup the interface */
    MTK_CHECK_WARNING(SKB_GetAes128KeyFromSecureData(cipher_key, &key));
    ctr_cipher = MTK_AllocateZeroMemory(sizeof(SKB_AesCtrCipher));
    if(ctr_cipher == NULL)
    {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    ctr_cipher->base.iface = &SKB_AesCtrCipherInterface;

    /* start the ctr */
    result = SKB_MapTomCryptResult(ctr_start(find_cipher("aes"),
                                             skb_zero_iv,
                                             key,
                                             16,
                                             0,
                                             CTR_COUNTER_BIG_ENDIAN,
                                             &ctr_cipher->ctr));
    if (SKB_FAILED(result)) {
        MTK_FreeMemory(ctr_cipher);
    } else {
        *cipher = &ctr_cipher->base;
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_AesCbcCipher_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesCbcCipher_Create(const SKB_SecureData* cipher_key,
                        SKB_Cipher**          cipher)
{
    const SKB_Byte*   key;
    SKB_Result        result;
    SKB_AesCbcCipher* cbc_cipher = NULL;

    /* allocate memory and setup the interface */
    MTK_CHECK_WARNING(SKB_GetAes128KeyFromSecureData(cipher_key, &key));
    cbc_cipher = MTK_AllocateZeroMemory(sizeof(SKB_AesCbcCipher));
    if(cbc_cipher == NULL)
    {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    cbc_cipher->base.iface = &SKB_AesCbcCipherInterface;

    /* start the cbc */
    result = SKB_MapTomCryptResult(cbc_start(find_cipher("aes"),
                                             skb_zero_iv,
                                             key,
                                             16,
                                             0,
                                             &cbc_cipher->cbc));
    if (SKB_FAILED(result)) {
        MTK_FreeMemory(cbc_cipher);
    } else {
        *cipher = &cbc_cipher->base;
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_AesEcbCipher_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_AesEcbCipher_Create(const SKB_SecureData* cipher_key,
                        SKB_Cipher**          cipher)
{
    const SKB_Byte*   key;
    SKB_Result        result;
    SKB_AesEcbCipher* ecb_cipher = NULL;

    /* allocate memory and setup the interface */
    MTK_CHECK_WARNING(SKB_GetAes128KeyFromSecureData(cipher_key, &key));
    ecb_cipher = MTK_AllocateZeroMemory(sizeof(SKB_AesEcbCipher));
    if(ecb_cipher == NULL)
    {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    ecb_cipher->base.iface = &SKB_AesEcbCipherInterface;

    /* start the ecb */
    result = SKB_MapTomCryptResult(ecb_start(find_cipher("aes"),
                                             key,
                                             16,
                                             0,
                                             &ecb_cipher->ecb));
    if (SKB_FAILED(result)) {
        MTK_FreeMemory(ecb_cipher);
    } else {
        *cipher = &ecb_cipher->base;
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_GenericCipher_Create
+---------------------------------------------------------------------*/
SKB_Result
SKB_GenericCipher_Create(SKB_Engine*           engine,
                         SKB_CipherAlgorithm   algorithm,
                         SKB_CipherDirection   direction,
                         unsigned int          flags,
                         const void*           parameters,
                         const SKB_SecureData* key_index,
                         SKB_Cipher**          cipher)
{
    /* init */
    MTK_COMPILER_UNUSED(flags);
    MTK_COMPILER_UNUSED(parameters);
    *cipher = NULL;

	SKB_SecureData *key = NULL;
	int data_index = 0;

	data_index = (int)key_index;

	dprintf("\n\n>> %s: \n", __FUNCTION__);

	key = skb_secure_data(data_index);






    /* check the parameters */
    if (key == NULL && algorithm != SKB_CIPHER_ALGORITHM_NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    switch (algorithm) {
        case SKB_CIPHER_ALGORITHM_NULL:
            return SKB_ERROR_NOT_SUPPORTED;
        case SKB_CIPHER_ALGORITHM_AES_128_CBC:
            MTK_CHECK_WARNING(SKB_AesCbcCipher_Create(key, cipher));
            break;
        case SKB_CIPHER_ALGORITHM_AES_128_CTR:
            MTK_CHECK_WARNING(SKB_AesCtrCipher_Create(key, cipher));
            break;
        case SKB_CIPHER_ALGORITHM_AES_128_ECB:
            MTK_CHECK_WARNING(SKB_AesEcbCipher_Create(key, cipher));
            break;
        case SKB_CIPHER_ALGORITHM_RSA_1_5:
        case SKB_CIPHER_ALGORITHM_RSA_OAEP:
            MTK_CHECK_WARNING(SKB_RsaCipher_Create(direction,
                                                   key,
                                                   algorithm,
                                                   cipher));
            break;
    }

    /* common fields */
    (*cipher)->engine    = engine;
    (*cipher)->direction = direction;

    return SKB_SUCCESS;
}

