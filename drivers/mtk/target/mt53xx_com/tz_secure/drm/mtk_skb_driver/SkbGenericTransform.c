/*****************************************************************
|
|   SKB Transform - Generic Implementation
|
|   $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/mtk_skb_driver/SkbGenericTransform.c#1 $
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
MTK_SET_LOCAL_LOGGER("skb.generic.transform")

/*----------------------------------------------------------------------
|   SKB_Transform_Release
+---------------------------------------------------------------------*/
SKB_Result
SKB_Transform_Release(SKB_Transform* self)
{
    return self->iface->Release(self);
}

/*----------------------------------------------------------------------
|   SKB_Transform_AddBytes
+---------------------------------------------------------------------*/
SKB_Result
SKB_Transform_AddBytes(SKB_Transform*  self,
                       const SKB_Byte* data,
                       SKB_Size        data_size)
{
    if (self->output_given) {
        return SKB_ERROR_INVALID_STATE;
    }
    return self->iface->AddBytes(self, data, data_size);
}

/*----------------------------------------------------------------------
|   SKB_Transform_AddSecureData
+---------------------------------------------------------------------*/
SKB_Result
SKB_Transform_AddSecureData(SKB_Transform* self, const SKB_SecureData* in_data_index)
{

	int data_index = 0;
	SKB_SecureData *real_data = NULL;

	data_index = (int)in_data_index;

	dprintf("\n\n>> %s: data_index[%x]\n", __FUNCTION__, data_index);

	real_data = skb_secure_data(data_index);



    if (self->output_given) {
        return SKB_ERROR_INVALID_STATE;
    }
    return self->iface->AddSecureData(self, real_data);
}

/*----------------------------------------------------------------------
|   SKB_Transform_GetOutput
+---------------------------------------------------------------------*/
SKB_Result
SKB_Transform_GetOutput(SKB_Transform* self,
                        SKB_Byte*      output,
                        SKB_Size*      output_size)
{
    return self->iface->GetOutput(self, output, output_size);
    self->output_given = MTK_TRUE;
}

/*----------------------------------------------------------------------
|   SKB_DigestTransform_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_DigestTransform_Release(SKB_Transform* _self)
{
    SKB_DigestTransform* self = (SKB_DigestTransform*)_self;
    MTK_FreeMemory(self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_DigestTransform_AddBytes
+---------------------------------------------------------------------*/
static SKB_Result
SKB_DigestTransform_AddBytes(SKB_Transform*  _self,
                             const SKB_Byte* data,
                             SKB_Size        data_size)
{
    SKB_DigestTransform* self = (SKB_DigestTransform*)_self;
    int result = hash_descriptor[self->hash_idx].process(&self->hash,
                                                         data,
                                                         data_size);
    return SKB_MapTomCryptResult(result);
}

/*----------------------------------------------------------------------
|   SKB_DigestTransform_AddSecureData
+---------------------------------------------------------------------*/
static SKB_Result
SKB_DigestTransform_AddSecureData(SKB_Transform*        _self,
                                  const SKB_SecureData* data)
{
    SKB_DigestTransform* self = (SKB_DigestTransform*)_self;
    SKB_Byte*            payload = NULL;
    SKB_Size             payload_size = 0;
    SKB_Result           result;

    /* only certain data types can be used here */
    if (data->type == SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
        return SKB_ERROR_NOT_SUPPORTED;
    }

    /* get the size of the payload */
    result = data->iface->Serialize(data, NULL, &payload_size);
    if (SKB_FAILED(result)) return result;

    /* allocate the payload buffer */
    payload = (SKB_Byte*)MTK_AllocateMemory(payload_size);
    if (payload == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    /* get the payload */
    result = data->iface->Serialize(data, payload, &payload_size);
    if (SKB_FAILED(result)) goto end;

    /* add the payload to the digest */
    result = hash_descriptor[self->hash_idx].process(&self->hash,
                                                     payload,
                                                     payload_size);
    result = SKB_MapTomCryptResult(result);
end:
    if (payload != NULL) MTK_FreeMemory(payload);
    return result;
}

/*----------------------------------------------------------------------
|   SKB_DigestTransform_GetOutput
+---------------------------------------------------------------------*/
static SKB_Result
SKB_DigestTransform_GetOutput(SKB_Transform* _self,
                              SKB_Byte*      output,
                              SKB_Size*      output_size)
{
    SKB_DigestTransform* self        = (SKB_DigestTransform*)_self;
    SKB_Size             digest_size = hash_descriptor[self->hash_idx].hashsize;
    int                  result      = CRYPT_OK;

    if (output == NULL ) {
        *output_size = digest_size;
        return SKB_SUCCESS;
    } else if (*output_size < digest_size) {
        *output_size = digest_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    *output_size = digest_size;
    result = hash_descriptor[self->hash_idx].done(&self->hash,
                                                  output);
    return SKB_MapTomCryptResult(result);
}

/*----------------------------------------------------------------------
|   SKB_DigestTransform interface
+---------------------------------------------------------------------*/
static const SKB_TransformInterface
SKB_DigestTransformInterface = {
    SKB_DigestTransform_Release,
    SKB_DigestTransform_AddBytes,
    SKB_DigestTransform_AddSecureData,
    SKB_DigestTransform_GetOutput
};

/*----------------------------------------------------------------------
|   SKB_RsaSignTransform_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaSignTransform_Release(SKB_Transform* _self)
{
    SKB_RsaSignTransform* self = (SKB_RsaSignTransform*)_self;
    if (self != NULL) {
        if (self->digested_data != NULL) {
            MTK_DataBuffer_Destroy(self->digested_data);
        }
        MTK_FreeMemory(self);
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaSignTransform_AddBytes
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaSignTransform_AddBytes(SKB_Transform*  _self,
                              const SKB_Byte* data,
                              SKB_Size        data_size)
{
    SKB_RsaSignTransform* self = (SKB_RsaSignTransform*)_self;
    if (self == NULL || data == NULL || data_size == 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    if (MTK_DataBuffer_GetDataSize(self->digested_data) != 0) {
        MTK_LOG_WARNING("cannot add data more than once for this kind of transform");
        return SKB_ERROR_INVALID_STATE;
    }

    if (MTK_FAILED(MTK_DataBuffer_SetData(self->digested_data, data, data_size))) {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaSignTransform_AddSecureData
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaSignTransform_AddSecureData(SKB_Transform*        _self,
                                   const SKB_SecureData* data)
{
    SKB_RsaSignTransform* self   = (SKB_RsaSignTransform*)_self;
    SKB_Result            result = SKB_SUCCESS;
    SKB_DataInfo          data_info;
    SKB_Size              data_size;

    if (self == NULL || data == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    if (MTK_DataBuffer_GetDataSize(self->digested_data) != 0) {
        MTK_LOG_WARNING("cannot add data more than once for this kind of transform");
        return SKB_ERROR_INVALID_STATE;
    }

    result = SKB_SecureData_GetInfo(data, &data_info);
    MTK_CHECK_WARNING(result);
    if (MTK_FAILED(MTK_DataBuffer_SetBufferSize(self->digested_data, data_info.size))) {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }
    data_size = data_info.size;
    result = data->iface->Serialize(data,
                                    MTK_DataBuffer_UseData(self->digested_data),
                                    &data_size);
    MTK_CHECK_WARNING(result);

    return SKB_SUCCESS;
}


/*----------------------------------------------------------------------
|   SKB_RsaSignTransform_GetOutput
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaSignTransform_GetOutput(SKB_Transform* _self,
                               SKB_Byte*      output,
                               SKB_Size*      output_size)
{
    SKB_RsaSignTransform* self     = (SKB_RsaSignTransform*)_self;
    SKB_Result            result   = SKB_SUCCESS;
    unsigned long         sig_size = mp_unsigned_bin_size(self->key->N);

    if (MTK_DataBuffer_GetDataSize(self->digested_data) == 0) {
        MTK_LOG_WARNING("no digest to sign");
        return SKB_ERROR_INVALID_STATE;
    }

    /* check output buffer and size */
    if (output == NULL) {
        *output_size = sig_size;
        return SKB_SUCCESS;
    }
    if (*output_size < sig_size) {
        *output_size = sig_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }

    /* encode and sign the digested data */
    *output_size = sig_size;
    result = pkcs_1_v1_5_encode(MTK_DataBuffer_GetData(self->digested_data),
                                MTK_DataBuffer_GetDataSize(self->digested_data),
                                LTC_PKCS_1_EMSA,
                                sig_size << 3, /* modulus bit length */
                                NULL,
                                -1,
                                output,
                                &sig_size);
    if (result != CRYPT_OK) return SKB_MapTomCryptResult(result);
    MTK_ASSERT(sig_size == *output_size);

    result = rsa_exptmod(output,
                         *output_size,
                         output,
                         &sig_size,
                         PK_PRIVATE,
                         self->key);
    if (result != CRYPT_OK) return SKB_MapTomCryptResult(result);
    MTK_ASSERT(sig_size == *output_size);

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaSignTransform interface
+---------------------------------------------------------------------*/
static const SKB_TransformInterface SKB_RsaSignTransformInterface = {
    SKB_RsaSignTransform_Release,
    SKB_RsaSignTransform_AddBytes,
    SKB_RsaSignTransform_AddSecureData,
    SKB_RsaSignTransform_GetOutput
};

/*----------------------------------------------------------------------
|   SKB_HmacSignTransform_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacSignTransform_Release(SKB_Transform* _self)
{
    SKB_HmacSignTransform* self = (SKB_HmacSignTransform*)_self;
    if (self != NULL) {
        if (self->initialized) {
            /* cleanup hmac: libtomcrypt not that great here... */
            unsigned char dummy;
            unsigned long dummy_size = 1;
            hmac_done(&self->hmac, &dummy, &dummy_size);
        }
        MTK_FreeMemory(self);
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_HmacSignTransform_AddBytes
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacSignTransform_AddBytes(SKB_Transform*  _self,
                               const SKB_Byte* data,
                               SKB_Size        data_size)
{
    SKB_HmacSignTransform* self = (SKB_HmacSignTransform*)_self;
    if (self == NULL || data == NULL || data_size == 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
    return SKB_MapTomCryptResult(hmac_process(&self->hmac,
                                              data,
                                              data_size));
}

/*----------------------------------------------------------------------
|   SKB_HmacSignTransform_AddSecureData
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacSignTransform_AddSecureData(SKB_Transform*        _self,
                                         const SKB_SecureData* data)
{
    SKB_HmacSignTransform* self       = (SKB_HmacSignTransform*)_self;
    SKB_Result             result     = SKB_SUCCESS;
    SKB_Byte*              data_bytes = NULL;
    SKB_DataInfo           data_info;
    SKB_Size               data_size;

    if (self == NULL || data == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    result = SKB_SecureData_GetInfo(data, &data_info);
    MTK_CHECK_WARNING(result);
    data_size = data_info.size;
    data_bytes = (SKB_Byte*)MTK_AllocateMemory(data_size);
    if (data == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    result = data->iface->Serialize(data,
                                    data_bytes,
                                    &data_size);
    MTK_CHECK_LABEL_WARNING(result, end);

    result = SKB_MapTomCryptResult(hmac_process(&self->hmac,
                                                data_bytes,
                                                data_size));
    MTK_CHECK_LABEL_WARNING(result, end);
end:
    if (data_bytes != NULL) {
        MTK_FreeMemory(data_bytes);
    }

    return result;
}
/*----------------------------------------------------------------------
|   SKB_HmacSignTransform_GetOutput
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacSignTransform_GetOutput(SKB_Transform* _self,
                                SKB_Byte*      output,
                                SKB_Size*      output_size)
{
    SKB_HmacSignTransform* self     = (SKB_HmacSignTransform*)_self;
    unsigned long          sig_size = hash_descriptor[self->hash_idx].hashsize;
    SKB_Result             result   = SKB_SUCCESS;

    if (output == NULL) {
        *output_size = sig_size;
        return SKB_SUCCESS;
    } else if (*output_size < sig_size) {
        *output_size = sig_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }

    /* compute hmac */
    *output_size = sig_size;
    result = SKB_MapTomCryptResult(hmac_done(&self->hmac,
                                             output,
                                             &sig_size));
    self->initialized = MTK_FALSE;
    MTK_CHECK_WARNING(result);
    MTK_ASSERT(sig_size == *output_size);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_HmacSignTransform interface
+---------------------------------------------------------------------*/
static const SKB_TransformInterface
SKB_HmacSignTransformInterface = {
    SKB_HmacSignTransform_Release,
    SKB_HmacSignTransform_AddBytes,
    SKB_HmacSignTransform_AddSecureData,
    SKB_HmacSignTransform_GetOutput
};

/*----------------------------------------------------------------------
|   SKB_RsaDigestSignTransform_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaDigestSignTransform_Release(SKB_Transform* _self)
{
    if (_self != NULL) {
        MTK_FreeMemory(_self);
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaDigestSignTransform_AddBytes
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaDigestSignTransform_AddBytes(SKB_Transform*  _self,
                                    const SKB_Byte* data,
                                    SKB_Size        data_size)
{
    SKB_RsaDigestSignTransform* self = (SKB_RsaDigestSignTransform*)_self;
    SKB_Result                  result;

    if (self == NULL || data == NULL || data_size == 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    result = hash_descriptor[self->hash_idx].process(&self->hash,
                                                     data,
                                                     data_size);
    MTK_CHECK_WARNING(SKB_MapTomCryptResult(result));
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaDigestSignTransform_AddSecureData
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaDigestSignTransform_AddSecureData(SKB_Transform*        _self,
                                         const SKB_SecureData* data)
{
    SKB_RsaDigestSignTransform* self = (SKB_RsaDigestSignTransform*)_self;
    SKB_Result         result        = SKB_SUCCESS;
    SKB_Byte*          data_bytes    = NULL;
    SKB_DataInfo       data_info;
    SKB_Size           data_size;

    if (self == NULL || data == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    result = SKB_SecureData_GetInfo(data, &data_info);
    MTK_CHECK_WARNING(result);
    data_size = data_info.size;
    data_bytes = (SKB_Byte*)MTK_AllocateMemory(data_size);
    if (data == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    result = data->iface->Serialize(data,
                                    data_bytes,
                                    &data_size);
    MTK_CHECK_LABEL_WARNING(result, end);

    result = hash_descriptor[self->hash_idx].process(&self->hash,
                                                     data_bytes,
                                                     data_size);
    result = SKB_MapTomCryptResult(result);
    MTK_CHECK_LABEL_WARNING(result, end);

end:
    if (data_bytes != NULL) {
        MTK_FreeMemory(data_bytes);
    }

    return result;
}
/*----------------------------------------------------------------------
|   SKB_RsaDigestSignTransform_GetOutput
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaDigestSignTransform_GetOutput(SKB_Transform* _self,
                                     SKB_Byte*      output,
                                     SKB_Size*      output_size)
{
    SKB_RsaDigestSignTransform* self     = (SKB_RsaDigestSignTransform*)_self;
    unsigned long               sig_size = mp_unsigned_bin_size(self->key->N);
    SKB_Result                  result   = SKB_SUCCESS;

    if (output == NULL) {
        *output_size = sig_size;
        return SKB_SUCCESS;
    } else if (*output_size < sig_size) {
        *output_size = sig_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }

    /* compute the digest */
    *output_size = sig_size;
    result = hash_descriptor[self->hash_idx].done(&self->hash,
                                                  output);
    MTK_CHECK_WARNING(SKB_MapTomCryptResult(result));
    sig_size = hash_descriptor[self->hash_idx].hashsize;

    /* compute the signature */
    sig_size = *output_size;
    result = rsa_sign_hash_ex(output,
                              hash_descriptor[self->hash_idx].hashsize,
                              output,
                              &sig_size,
                              LTC_PKCS_1_V1_5,
                              NULL,
                              -1,
                              self->hash_idx,
                              0,
                              self->key);
    MTK_CHECK_WARNING(SKB_MapTomCryptResult(result));
    MTK_ASSERT(sig_size == *output_size);

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaDigestSignTransform interface
+---------------------------------------------------------------------*/
static const SKB_TransformInterface
SKB_RsaDigestSignTransformInterface = {
    SKB_RsaDigestSignTransform_Release,
    SKB_RsaDigestSignTransform_AddBytes,
    SKB_RsaDigestSignTransform_AddSecureData,
    SKB_RsaDigestSignTransform_GetOutput
};

/*----------------------------------------------------------------------
|   SKB_HmacVerifyTransform_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacVerifyTransform_Release(SKB_Transform* _self)
{
    SKB_HmacVerifyTransform* self = (SKB_HmacVerifyTransform*)_self;
    if (self != NULL) {
        if (self->compare != NULL) {
            MTK_DataBuffer_Destroy(self->compare);
        }
        if (self->initialized) {
            /* cleanup hmac: libtomcrypt not that great here... */
            unsigned char dummy;
            unsigned long dummy_size = 1;
            hmac_done(&self->hmac, &dummy, &dummy_size);
        }
        MTK_FreeMemory(self);
    }
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_HmacVerifyTransform_AddBytes
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacVerifyTransform_AddBytes(SKB_Transform*  _self,
                                 const SKB_Byte* data,
                                 SKB_Size        data_size)
{
    SKB_HmacVerifyTransform* self = (SKB_HmacVerifyTransform*)_self;
    if (self == NULL || data == NULL || data_size == 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
    return SKB_MapTomCryptResult(hmac_process(&self->hmac,
                                              data,
                                              data_size));
}

/*----------------------------------------------------------------------
|   SKB_HmacVerifyTransform_AddSecureData
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacVerifyTransform_AddSecureData(SKB_Transform*        _self,
                                      const SKB_SecureData* data)
{
    SKB_HmacVerifyTransform* self     = (SKB_HmacVerifyTransform*)_self;
    SKB_Result             result     = SKB_SUCCESS;
    SKB_Byte*              data_bytes = NULL;
    SKB_DataInfo           data_info;
    SKB_Size               data_size;

    if (self == NULL || data == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    result = SKB_SecureData_GetInfo(data, &data_info);
    MTK_CHECK_WARNING(result);
    data_size = data_info.size;
    data_bytes = (SKB_Byte*)MTK_AllocateMemory(data_size);
    if (data == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    result = data->iface->Serialize(data,
                                    data_bytes,
                                    &data_size);
    MTK_CHECK_LABEL_WARNING(result, end);

    result = SKB_MapTomCryptResult(hmac_process(&self->hmac,
                                                data_bytes,
                                                data_size));
    MTK_CHECK_LABEL_WARNING(result, end);
end:
    if (data_bytes != NULL) {
        MTK_FreeMemory(data_bytes);
    }

    return result;
}

/*----------------------------------------------------------------------
|   SKB_HmacVerifyTransform_GetOutput
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacVerifyTransform_GetOutput(SKB_Transform* _self,
                                  SKB_Byte*      output,
                                  SKB_Size*      output_size)
{
    SKB_HmacVerifyTransform* self       = (SKB_HmacVerifyTransform*)_self;
    SKB_Byte*                hmac_bytes = NULL;
    unsigned long            hmac_size;
    SKB_Result               result;

    if (output == NULL) {
        *output_size = 1;
        return SKB_SUCCESS;
    } else if (*output_size < 1) {
        *output_size = 1;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    *output_size = 1;

    /* shortcut */
    hmac_size = hash_descriptor[self->hash_idx].hashsize;
    if (hmac_size != MTK_DataBuffer_GetDataSize(self->compare)) {
        *output = 0;
        return SKB_SUCCESS;
    }

    /* compute the signature */
    hmac_bytes = (SKB_Byte*)MTK_AllocateMemory(hmac_size);
    if (hmac_bytes == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    result = SKB_MapTomCryptResult(hmac_done(&self->hmac,
                                             hmac_bytes,
                                             &hmac_size));
    self->initialized = MTK_FALSE;
    MTK_CHECK_LABEL_WARNING(result, end);
    MTK_ASSERT(hmac_size == hash_descriptor[self->hash_idx].hashsize);

    /* compare */
    if (MTK_MemoryEqual(hmac_bytes,
                        MTK_DataBuffer_GetData(self->compare),
                        hmac_size)) {
        *output = 1;
    } else {
        *output = 0;
    }

end:
    if (hmac_bytes != NULL) {
        MTK_FreeMemory(hmac_bytes);
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_HmacVerifyTransform interface
+---------------------------------------------------------------------*/
static const SKB_TransformInterface SKB_HmacVerifyTransformInterface = {
    SKB_HmacVerifyTransform_Release,
    SKB_HmacVerifyTransform_AddBytes,
    SKB_HmacVerifyTransform_AddSecureData,
    SKB_HmacVerifyTransform_GetOutput
};

/*----------------------------------------------------------------------
|   SKB_GetDigestHashIdx
+---------------------------------------------------------------------*/
static int
SKB_GetDigestHashIdx(SKB_DigestAlgorithm algo)
{
    switch (algo) {
        case SKB_DIGEST_ALGORITHM_SHA1:
            return find_hash("sha1");
        case SKB_DIGEST_ALGORITHM_SHA256:
            return find_hash("sha256");
    }
    return -1; /* should have returned from the switch above */
}

/*----------------------------------------------------------------------
|   SKB_GetSignatureHashIdx
+---------------------------------------------------------------------*/
static int
SKB_GetSignatureHashIdx(SKB_SignatureAlgorithm algo)
{
    switch (algo) {
        case SKB_SIGNATURE_ALGORITHM_HMAC_SHA1:
        case SKB_SIGNATURE_ALGORITHM_RSA_SHA1:
            return find_hash("sha1");
        case SKB_SIGNATURE_ALGORITHM_HMAC_SHA256:
        case SKB_SIGNATURE_ALGORITHM_RSA_SHA256:
            return find_hash("sha256");
        case SKB_SIGNATURE_ALGORITHM_RSA:
            MTK_LOG_SEVERE("no digest algorithm for this type of signature");
            return -1;
    }
    return -1; /* should have returned from the switch above */
}

/*----------------------------------------------------------------------
|   SKB_DigestTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_DigestTransform_Create(SKB_Engine*         self,
                           SKB_DigestAlgorithm algorithm,
                           SKB_Transform**     transform)
{
    SKB_DigestTransform* digest_transform;
    SKB_Result           result   = SKB_SUCCESS;
    int                  hash_idx = SKB_GetDigestHashIdx(algorithm);

    if (hash_idx == -1) {
        result = SKB_ERROR_NOT_SUPPORTED;
        goto end;
    }

    *transform = NULL;

    /* create the object */
    digest_transform =
        (SKB_DigestTransform*)MTK_AllocateZeroMemory(sizeof(SKB_DigestTransform));
    if (digest_transform == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    /* create the hash */
    result = hash_descriptor[hash_idx].init(&digest_transform->hash);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);

    /* set the fields */
    digest_transform->base.iface  = &SKB_DigestTransformInterface;
    digest_transform->base.engine = self;
    digest_transform->hash_idx    = hash_idx;
    *transform = (SKB_Transform*)digest_transform;

end:
    if (SKB_FAILED(result) && digest_transform != NULL) {
        MTK_FreeMemory(digest_transform);
        *transform = NULL;
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_RsaSignTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaSignTransform_Create(SKB_Engine*                        engine,
                            const SKB_SignTransformParameters* parameters,
                            SKB_Transform**                    transform)
{
    SKB_Result              result;
    SKB_RsaSignTransform*   sign_transform = NULL;
    rsa_key*                key            = NULL;


	int						rsa_key_index = 0;
	SKB_SecureData 			*real_rsa_key = NULL;




	

    MTK_ASSERT(parameters->algorithm == SKB_SIGNATURE_ALGORITHM_RSA);

	


	rsa_key_index = (int)parameters->key;

	dprintf("\n\n>> %s: rsa_key_index[%x]\n", __FUNCTION__, rsa_key_index);

	real_rsa_key = skb_secure_data(rsa_key_index);



    /* get the key */
    //if (parameters->key == NULL ||
    //    parameters->key->type != SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
    //    return SKB_ERROR_INVALID_PARAMETERS;
    //}
	
    /* get the key */
    if (real_rsa_key == NULL ||
        real_rsa_key->type != SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
	




	//key = &((SKB_RsaKeyData*)(parameters->key))->key;
	key = &((SKB_RsaKeyData*)(real_rsa_key))->key;





	
    

    /* create the object */
    sign_transform =
        (SKB_RsaSignTransform*)MTK_AllocateZeroMemory(sizeof(SKB_RsaSignTransform));
    if (sign_transform == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    /* create the fields */
    sign_transform->base.iface  = &SKB_RsaSignTransformInterface;
    sign_transform->base.engine = engine;
    sign_transform->key = key;
    result = MTK_DataBuffer_Create(0, &sign_transform->digested_data);
    if (MTK_FAILED(result)) {
        result = SKB_ERROR_OUT_OF_RESOURCES;
        goto end;
    }
    *transform = (SKB_Transform*)sign_transform;

end:
    if (SKB_FAILED(result) && sign_transform != NULL) {
        SKB_RsaSignTransform_Release(&sign_transform->base);
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_RsaDigestSignTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaDigestSignTransform_Create(SKB_Engine*                        engine,
                                  const SKB_SignTransformParameters* params,
                                  SKB_Transform**                    transform)
{
    SKB_Result                  result         = SKB_SUCCESS;
    SKB_RsaDigestSignTransform* sign_transform = NULL;
    rsa_key*                    key;

	int						rsa_key_index = 0;
	SKB_SecureData 			*real_rsa_key = NULL;






	rsa_key_index = (int)params->key;

	dprintf("\n\n>> %s: rsa_key_index[%x]\n", __FUNCTION__, rsa_key_index);

	real_rsa_key = skb_secure_data(rsa_key_index);


    /* get the key */
    //if (params->key == NULL ||
    //    params->key->type != SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
    //    return SKB_ERROR_INVALID_PARAMETERS;
    //}


    /* get the key */
    if (real_rsa_key == NULL ||
        real_rsa_key->type != SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }


	
    key = &((SKB_RsaKeyData*)real_rsa_key)->key;




    /* create the object */
    sign_transform =
        (SKB_RsaDigestSignTransform*)MTK_AllocateZeroMemory(sizeof(SKB_RsaDigestSignTransform));
    if (sign_transform == NULL) {
        result = SKB_ERROR_OUT_OF_RESOURCES;
        goto end;
    }

    /* create the fields */
    sign_transform->base.iface  = &SKB_RsaDigestSignTransformInterface;
    sign_transform->base.engine = engine;
    sign_transform->key         = key;
    sign_transform->hash_idx    = SKB_GetSignatureHashIdx(params->algorithm);
    if (sign_transform->hash_idx == -1) {
        result = SKB_ERROR_NOT_SUPPORTED;
        goto end;
    }

    /* init the hash */
    result = hash_descriptor[sign_transform->hash_idx].init(&sign_transform->hash);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);

    /* all good */
    *transform = &sign_transform->base;

end:
    if (SKB_FAILED(result)) {
        if (sign_transform != NULL) {
            SKB_RsaDigestSignTransform_Release(&sign_transform->base);
        }
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_HmacSignTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacSignTransform_Create(SKB_Engine*                        engine,
                             const SKB_SignTransformParameters* params,
                             SKB_Transform**                    transform)
{
    SKB_HmacSignTransform* sign_transform = NULL;
    SKB_Result             result         = SKB_SUCCESS;
    const MTK_DataBuffer*  key;



	int						key_index = 0;
	SKB_SecureData 			*real_key = NULL;





	key_index = (int)params->key;

	dprintf("\n\n>> %s: rsa_key_index[%x]\n", __FUNCTION__, key_index);

	real_key = skb_secure_data(key_index);

    /* get the key */
    //if (params->key == NULL || params->key->type != SKB_DATA_TYPE_BYTES) {
    //    return SKB_ERROR_INVALID_PARAMETERS;
    //}

    /* get the key */
    if (real_key == NULL || real_key->type != SKB_DATA_TYPE_BYTES) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }


	
    //key = ((SKB_BytesData*)params->key)->bytes;
	key = ((SKB_BytesData*)real_key)->bytes;



    /* create the object */
    sign_transform =
        (SKB_HmacSignTransform*)MTK_AllocateZeroMemory(sizeof(SKB_HmacSignTransform));
    if (sign_transform == NULL) {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }

    /* get the hash idx */
    sign_transform->hash_idx = SKB_GetSignatureHashIdx(params->algorithm);

    /* init the hmac */
    result = SKB_MapTomCryptResult(hmac_init(&sign_transform->hmac,
                                             sign_transform->hash_idx,
                                             MTK_DataBuffer_GetData(key),
                                             MTK_DataBuffer_GetDataSize(key)));
    MTK_CHECK_LABEL_WARNING(result, end);
    sign_transform->initialized = MTK_TRUE;

    /* fields */
    sign_transform->base.iface = &SKB_HmacSignTransformInterface;
    sign_transform->base.engine = engine;

    /* all good */
    *transform = &sign_transform->base;

end:
    if (SKB_FAILED(result) && sign_transform != NULL) {
        SKB_HmacSignTransform_Release(&sign_transform->base);
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_SignTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_SignTransform_Create(SKB_Engine*                        engine,
                         const SKB_SignTransformParameters* parameters,
                         SKB_Transform**                    transform)
{
    switch (parameters->algorithm) {
        case SKB_SIGNATURE_ALGORITHM_RSA_SHA1:
        case SKB_SIGNATURE_ALGORITHM_RSA_SHA256:
            return SKB_RsaDigestSignTransform_Create(engine,
                                                     parameters,
                                                     transform);
        case SKB_SIGNATURE_ALGORITHM_HMAC_SHA1:
        case SKB_SIGNATURE_ALGORITHM_HMAC_SHA256:
            return SKB_HmacSignTransform_Create(engine,
                                                parameters,
                                                transform);
        case SKB_SIGNATURE_ALGORITHM_RSA:
            return SKB_RsaSignTransform_Create(engine,
                                               parameters,
                                               transform);
    }
    return SKB_ERROR_INTERNAL; /* should have returned from the switch above */
}


/*----------------------------------------------------------------------
|   SKB_HmacVerifyTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_HmacVerifyTransform_Create(SKB_Engine*                          engine,
                               const SKB_VerifyTransformParameters* params,
                               SKB_Transform**                      transform)
{
    SKB_HmacVerifyTransform* verify_transform = NULL;
    SKB_Result               result           = SKB_SUCCESS;
    const MTK_DataBuffer*    key;


	int						key_index = 0;
	SKB_SecureData 			*real_key = NULL;
	






	key_index = (int)params->key;

	dprintf("\n\n>> %s: rsa_key_index[%x]\n", __FUNCTION__, key_index);

	real_key = skb_secure_data(key_index);



    /* get the key */
    //if (params->key            == NULL                ||
    //    params->key->type      != SKB_DATA_TYPE_BYTES ||
    //    params->signature      == NULL                ||
    //    params->signature_size == 0) {
    //    return SKB_ERROR_INVALID_PARAMETERS;
    //}



    /* get the key */
    if (real_key            == NULL                ||
        real_key->type      != SKB_DATA_TYPE_BYTES ||
        params->signature      == NULL                ||
        params->signature_size == 0) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }


	
    //key = ((SKB_BytesData*)params->key)->bytes;
	key = ((SKB_BytesData*)real_key)->bytes;

	
  


    /* create the object */
    verify_transform =
        (SKB_HmacVerifyTransform*)MTK_AllocateZeroMemory(sizeof(SKB_HmacVerifyTransform));
    if (verify_transform == NULL) {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }

    /* create our internal buffer for holding the signature bytes */
    if (MTK_FAILED(MTK_DataBuffer_Create(params->signature_size,
                                         &verify_transform->compare))) {
        MTK_CHECK_LABEL_SEVERE(result = SKB_ERROR_OUT_OF_RESOURCES, end);
    }
    MTK_DataBuffer_SetData(verify_transform->compare,
                           params->signature,
                           params->signature_size);

    /* get the hash idx */
    verify_transform->hash_idx = SKB_GetSignatureHashIdx(params->algorithm);

    /* init the hmac */
    result = SKB_MapTomCryptResult(hmac_init(&verify_transform->hmac,
                                             verify_transform->hash_idx,
                                             MTK_DataBuffer_GetData(key),
                                             MTK_DataBuffer_GetDataSize(key)));
    MTK_CHECK_LABEL_WARNING(result, end);
    verify_transform->initialized = MTK_TRUE;

    /* fields */
    verify_transform->base.iface = &SKB_HmacVerifyTransformInterface;
    verify_transform->base.engine = engine;

    /* all good */
    *transform = &verify_transform->base;

end:
    if (SKB_FAILED(result) && verify_transform != NULL) {
        SKB_HmacVerifyTransform_Release(&verify_transform->base);
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_VerifyTransform_Create
+---------------------------------------------------------------------*/
static SKB_Result
SKB_VerifyTransform_Create(SKB_Engine*                          engine,
                           const SKB_VerifyTransformParameters* params,
                           SKB_Transform**                      transform)
{
    switch (params->algorithm) {
        case SKB_SIGNATURE_ALGORITHM_HMAC_SHA1:
        case SKB_SIGNATURE_ALGORITHM_HMAC_SHA256:
            return SKB_HmacVerifyTransform_Create(engine,
                                                  params,
                                                  transform);
        case SKB_SIGNATURE_ALGORITHM_RSA:
        case SKB_SIGNATURE_ALGORITHM_RSA_SHA1:
        case SKB_SIGNATURE_ALGORITHM_RSA_SHA256:
            MTK_LOG_SEVERE("SKB doesn't deal with publid key operations");
            return SKB_ERROR_INVALID_PARAMETERS;
    }
    return SKB_ERROR_INTERNAL; /* should have returned from the swith above */
}

/*----------------------------------------------------------------------
|   SKB_GenericTransform_Create
+---------------------------------------------------------------------*/
SKB_Result
SKB_GenericTransform_Create(SKB_Engine*       engine,
                            SKB_TransformType type,
                            const void*       parameters,
                            SKB_Transform**   transform)
{
    *transform = NULL;
    switch (type) {
        case SKB_TRANSFORM_TYPE_DIGEST: {
            const SKB_DigestTransformParameters* digest_parameters =
                (const SKB_DigestTransformParameters*)parameters;
            if (parameters == NULL) return SKB_ERROR_INVALID_PARAMETERS;
            return SKB_DigestTransform_Create(engine,
                                              digest_parameters->algorithm,
                                              transform);
        }

        case SKB_TRANSFORM_TYPE_VERIFY: {
            const SKB_VerifyTransformParameters* verify_parameters =
                (const SKB_VerifyTransformParameters*)parameters;
            if (parameters == NULL) return SKB_ERROR_INVALID_PARAMETERS;
            return SKB_VerifyTransform_Create(engine,
                                              verify_parameters,
                                              transform);
        }

        case SKB_TRANSFORM_TYPE_SIGN: {
            const SKB_SignTransformParameters* sign_parameters =
                (const SKB_SignTransformParameters*)parameters;
            if (parameters == NULL) return SKB_ERROR_INVALID_PARAMETERS;
            return SKB_SignTransform_Create(engine,
                                            sign_parameters,
                                            transform);
        }

        default:
            return SKB_ERROR_NOT_SUPPORTED;
    }
    return SKB_SUCCESS;
}

