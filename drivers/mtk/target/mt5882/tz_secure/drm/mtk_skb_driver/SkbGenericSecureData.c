/*****************************************************************
|
|   SKB Secure Data - Generic Implementation
|
|   $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/tz_secure/drm/mtk_skb_driver/SkbGenericSecureData.c#1 $
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
#include "tomcrypt_asn1_missing.h"

/*----------------------------------------------------------------------
|   logger
+---------------------------------------------------------------------*/
MTK_SET_LOCAL_LOGGER("skb.generic.securedata")






/*----------------------------------------------------------------------
|   declarations
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BytesData_Create(SKB_Engine*      engine,
                     unsigned int     flags,
                     const SKB_Byte*  bytes,
                     SKB_Size         bytes_size,
                     SKB_SecureData** data);
static SKB_Result
SKB_RsaKeyData_Create(SKB_Engine*      engine,
                      unsigned int     flags,
                      const SKB_Byte*  encoded_key,
                      SKB_Size         encoded_key_size,
                      MTK_Boolean      from_exported,
                      SKB_SecureData** data);



#define MAX_SECURE_SZ		(1024)
static SKB_SecureData*g_secure_data[MAX_SECURE_SZ+1] = {NULL};


/*
	!!!: NULL was not added into the table
*/
static int skb_secure_insert(SKB_SecureData* data)
{
    int index = 0;


	dprintf("\n\n%s: to-add data[%x]\n", __FUNCTION__, data);

	if(NULL == data)
	{
		return -1;
	}

    for (index = 1; index < MAX_SECURE_SZ+1; index++)
    {
        if (g_secure_data[index] == NULL)
        {
            g_secure_data[index] = data;
			
			dprintf("\n\n%s: sucure data [%x] added on index [%x]\n", __FUNCTION__, data, index);
			
      		return index;
        }
    }
	
    dprintf("\n\n%s: ERROR*** SKB Mapping table is full\n", __FUNCTION__);

	return -1;

}


static int skb_secure_match(SKB_SecureData* data)
{
    int index = 0;


	dprintf("\n\n%s: scan data[%x]\n", __FUNCTION__, data);

	if(NULL == data)
	{
		return -1;
	}

    for (index = 1; index < MAX_SECURE_SZ+1; index++)
    {
        if (g_secure_data[index] == data)
        {
            g_secure_data[index] = data;
			
			dprintf("\n\n%s: sucure data [%x] found on index [%x]\n", __FUNCTION__, data, index);
			
      		return 0;
        }
    }
	
	dprintf("\n\n%s: sucure data [%x] NOT found \n", __FUNCTION__, data);


	return -1;


}

static int skb_secure_delete(SKB_SecureData* data)
{
    int index = 0;


	dprintf("\n\n%s: delete data[%x]\n", __FUNCTION__, data);

	if(NULL == data)
	{
		return -1;
	}

    for (index = 1; index < MAX_SECURE_SZ+1; index++)
    {
        if (g_secure_data[index] == data)
        {
            g_secure_data[index] = data;
			
			dprintf("\n\n%s: sucure data [%x] found on index [%x]\n", __FUNCTION__, data, index);
			
			g_secure_data[index] = NULL;
			
      		return 0;
        }
    }
	
	dprintf("\n\n%s: sucure data [%x] NOT found \n", __FUNCTION__, data);


	return -1;


}




SKB_SecureData *skb_secure_data(int index)
{



	dprintf("\n\n%s: get index[%x]\n", __FUNCTION__, index);

	if(index<1 || index>=MAX_SECURE_SZ+1)
	{
		dprintf("\n\n%s: ERROR: index overflow\n", __FUNCTION__);
		return NULL;
	}

	dprintf("\n\n%s: sucure data [%x] return \n", __FUNCTION__, g_secure_data[index]);

	if(NULL == g_secure_data[index])
	{
		dprintf("\n\n%s: WARNING: data[%d] NULL\n", __FUNCTION__, index);
	}
	
	return g_secure_data[index];
	
}

static int skb_secure_delete_index(int index)
{

	dprintf("\n\n%s: get index[%x]\n", __FUNCTION__, index);

	if(index<1 || index>=MAX_SECURE_SZ+1)
	{
		dprintf("\n\n%s: ERROR: index overflow\n", __FUNCTION__);
		return -1;
	}

	dprintf("\n\n%s: data[%d]: [%x] clean\n", __FUNCTION__, index, g_secure_data[index]);

	g_secure_data[index] = NULL;

	return 0;
}


































/*----------------------------------------------------------------------
|   SKB_SecureData_Release
+---------------------------------------------------------------------*/
SKB_Result
SKB_SecureData_Release(SKB_SecureData* self)
{
	int data_index = 0;
	SKB_SecureData *real_data = NULL;

	data_index = (int)self;

	dprintf("\n\n>> %s: \n", __FUNCTION__);

	real_data = skb_secure_data(data_index);


    if (real_data == NULL) return SKB_SUCCESS;

	real_data->iface->Release(real_data);


	return skb_secure_delete_index(data_index);


	
}

/*----------------------------------------------------------------------
|   SKB_SecureData_GetInfo
+---------------------------------------------------------------------*/
SKB_Result
SKB_SecureData_GetInfo(const SKB_SecureData* self, SKB_DataInfo* info)
{
	int data_index = 0;
	SKB_SecureData *real_data = NULL;

	data_index = (int)self;

	dprintf("\n\n>> %s: \n", __FUNCTION__);

	real_data = skb_secure_data(data_index);




    if (real_data == NULL) return SKB_ERROR_INVALID_PARAMETERS;
    return real_data->iface->GetInfo(real_data, info);
}

/*----------------------------------------------------------------------
|   SKB_SecureData_Derive
+---------------------------------------------------------------------*/
SKB_Result
SKB_SecureData_Derive(const SKB_SecureData*   self,
                      SKB_DerivationAlgorithm algorithm,
                      const void*             parameters,
                      SKB_SecureData**        data)
{

	int data_index = 0;
	SKB_SecureData *real_data = NULL;

	data_index = (int)self;

	dprintf("\n\n>> %s: \n", __FUNCTION__);

	real_data = skb_secure_data(data_index);




    if (real_data == NULL) return SKB_ERROR_INVALID_PARAMETERS;
    return real_data->iface->Derive(real_data, algorithm, parameters, data);
}






/*----------------------------------------------------------------------
|   SKB_SecureData_Export
|
|   Serialize the key data in an encrypted form
|   The serialized byte sequence in this implementation is:
|
|   [Header][IV][AES-CBC-data][HMAC]
|
|   Header:
|   [Magic]        4  bytes: magic number
|   [Version]      1  byte:  format version (2)
|   [Persist]      1  byte:  0 for cross-engine, 1 for persistent,
|   [ClassID]      4  byte:  data class ID
|   [Flags]        4  bytes: data flags
|
|   Payload:
|   [IV]           16 bytes: IV for the AES-CBC cipher
|   [AES-CBC-data] 4+N+4+P bytes: AES CBC encrypted payload
|       [SerializedSize] 4 bytes
|       [SerializedData] N bytes
|       [ExtensionCount] 4 bytes
|       [Extensions]     P bytes
|   [HMAC]         20 bytes: HMAC-SHA1 Signature for the payload
|
|   Extension (embedded in the [Extensions] field of [AES-CBC-data]:
|   [ExtensionType]  4 bytes
|   [ExtensionSize]  4 bytes
|   [ExtensionData]  ExtensionSize bytes
|
+---------------------------------------------------------------------*/
#define SKB_EXPORT_FORMAT_VERSION 2
#define SKB_EXPORT_HEADER_SIZE (4+1+1+4+4)
SKB_Result
SKB_SecureData_Export(const SKB_SecureData* self_index,
                      SKB_ExportTarget      target,
                      const void*           target_parameters,
                      SKB_Byte*             buffer,
                      SKB_Size*             buffer_size)
{
    MTK_Size        export_size  = 0;
    SKB_Size        serialized_payload_size = 0;
    SKB_Size        total_payload_size = 0;
    SKB_Result      result;
    symmetric_CBC   cbc;
    MTK_Boolean     cbc_initialized = MTK_FALSE;
    hmac_state      mac;
    MTK_Boolean     mac_initialized = MTK_FALSE;
    unsigned long   mac_size        = 20;
    const SKB_Byte* export_key;

    MTK_COMPILER_UNUSED(target_parameters);



	int data_index = 0;
	SKB_SecureData *self = NULL;

	data_index = (int)self_index;

	dprintf("\n\n>> %s: \n", __FUNCTION__);

	self = skb_secure_data(data_index);






    /* check parameters */
    if (buffer_size == NULL || self == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

#define SKB_CONFIG_ENABLE_CLEARTEXT_EXPORT /* In this version, we allow cleartext export */
#if defined (SKB_CONFIG_ENABLE_CLEARTEXT_EXPORT)
    if (target == SKB_EXPORT_TARGET_CLEARTEXT) {
        /* only certain data types can be used here */
        if (self->type == SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
            return SKB_ERROR_NOT_SUPPORTED;
        }
        return self->iface->Serialize(self, buffer, buffer_size);
    }
#endif

    /* check if we support the export target */
    if (!(target == SKB_EXPORT_TARGET_PERSISTENT ||
          target == SKB_EXPORT_TARGET_CROSS_ENGINE)) {
        return SKB_ERROR_NOT_SUPPORTED;
    }

    /* check the buffer size */
    result = self->iface->Serialize(self, NULL, &serialized_payload_size);
    if (SKB_FAILED(result)) return result;

    /* the total payload is the serialized payload plus the extensions.     */
    /* since we don't have any extension here, extensions have a fixed size */
    total_payload_size = serialized_payload_size+8;

    /* epxort_size = header + 16-byte IV + payload and CBC padding + HMAC */
    export_size = SKB_EXPORT_HEADER_SIZE + 16*(2+total_payload_size/16) + 20;
    if (buffer == NULL) {
        *buffer_size = export_size;
        return SKB_SUCCESS;
    } else if (*buffer_size < export_size) {
        *buffer_size = export_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    *buffer_size = export_size;

    /* write the magic number */
    buffer[0] = 'S';
    buffer[1] = 'K';
    buffer[2] = 'B';
    buffer[3] = '!';

    /* write the version */
    buffer[4] = SKB_EXPORT_FORMAT_VERSION;

    /* select the export key and persistence flag */
    MTK_ASSERT(self->engine);
    if (target == SKB_EXPORT_TARGET_PERSISTENT) {
		
        //export_key = self->engine->persistent_export_key;
		export_key = SKB_Engine_persistent_export_key(self->engine); 
        buffer[5]  = 1;
    } else {
    
        //export_key = self->engine->cross_engine_export_key;
		export_key = SKB_Engine_cross_engine_export_key(self->engine); 
        buffer[5]  = 0;
    }
    MTK_ASSERT(export_key);

    /* store the class ID */
    MTK_BytesFromInt32Be(&buffer[6], self->iface->GetClassId(self));

    /* store the flags */
    MTK_BytesFromInt32Be(&buffer[10], self->flags);

    /* create the hmac and add the header */
    result = hmac_init(&mac, find_hash("sha1"), export_key, 16);
    if (result != CRYPT_OK) {
        result = SKB_MapTomCryptResult(result);
        MTK_CHECK_LABEL_WARNING(result, end);
    }
    mac_initialized = MTK_TRUE;
    result = hmac_process(&mac, buffer, SKB_EXPORT_HEADER_SIZE);
    if (result != CRYPT_OK) {
        result = SKB_MapTomCryptResult(result);
        MTK_CHECK_LABEL_WARNING(result, end);
    }

    /* get a random IV */
    {
        unsigned long iv_size = rng_get_bytes(&buffer[14], 16, NULL);
        if (iv_size != 16) {
            MTK_LOG_SEVERE("could not get 16 random bytes for IV");
            result = SKB_ERROR_INTERNAL;
            goto end;
        }
    }

    /* write the serialized size */
    MTK_BytesFromInt32Be(&buffer[30], serialized_payload_size);

    /* serialize the payload */
    result = self->iface->Serialize(self, &buffer[34], &serialized_payload_size);
    MTK_CHECK_LABEL_WARNING(result, end);

    /* serialize the extensions (empty list here) */
    MTK_BytesFromInt32Be(&buffer[34+serialized_payload_size], 0);

    /* add the payload to the mac */
    result = hmac_process(&mac, &buffer[30], total_payload_size);
    if (result != CRYPT_OK) {
        result = SKB_MapTomCryptResult(result);
        MTK_CHECK_LABEL_WARNING(result, end);
    }

    /* now pad */
    {
        unsigned char i;
        unsigned char padding_length = (SKB_Byte)(16 - total_payload_size%16);
        for (i = 0; i < padding_length; i++) {
            buffer[30+i+total_payload_size] = i + 1;
        }
        total_payload_size += padding_length; /* adjust */
    }

    /* encrypt the payload with the export key, preceded by the IV */
    result = cbc_start(find_cipher("aes"),
                       &buffer[14], /* iv */
                       export_key,
                       16,
                       0,
                       &cbc);
    if (result != CRYPT_OK) {
        result = SKB_MapTomCryptResult(result);
        MTK_CHECK_LABEL_WARNING(result, end);
    }
    cbc_initialized = MTK_TRUE;

    result = cbc_encrypt(&buffer[30], &buffer[30], total_payload_size, &cbc);
    if (result != CRYPT_OK) {
        result = SKB_MapTomCryptResult(result);
        MTK_CHECK_LABEL_WARNING(result, end);
    }

    /* store the mac */
    result = hmac_done(&mac, &buffer[30+total_payload_size], &mac_size);
    mac_initialized = MTK_FALSE;
    MTK_ASSERT(mac_size == 20);

end:
    if (cbc_initialized) {
        cbc_done(&cbc);
    }
    if (mac_initialized) {
        /* idiotic way to cleanup if done hasn't already been called... */
        unsigned char dummy;
        unsigned long dummy_size = 1;
        hmac_done(&mac, &dummy, &dummy_size);
    }
    return result;
}

/*----------------------------------------------------------------------
|   SKB_BaseData_GetInfo
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BaseData_GetInfo(const SKB_SecureData* self, SKB_DataInfo* info)
{
    if (info == NULL) return SKB_ERROR_INVALID_PARAMETERS;
    info->type  = self->type;
    info->size  = self->size;

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_BaseData_DeriveSlice
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BaseData_DeriveSlice(const SKB_SecureData*                self,
                         const SKB_SliceDerivationParameters* params,
                         SKB_SecureData**                     data)
{
    SKB_Size             size;
    const SKB_BytesData* bytes_data;
    const SKB_Byte*      slice = NULL;

    if (params == NULL) return SKB_ERROR_INVALID_PARAMETERS;

    /* make sure that we have bytes */
    if (self->type != SKB_DATA_TYPE_BYTES) {
        return SKB_ERROR_INVALID_FORMAT;
    }

    bytes_data = (const SKB_BytesData*)self;
    size = MTK_DataBuffer_GetDataSize(bytes_data->bytes);
    if (size < params->size + params->first) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    slice = MTK_DataBuffer_GetData(bytes_data->bytes) + params->first;
    return SKB_BytesData_Create(self->engine,
                                self->flags,
                                slice,
                                params->size,
                                data);
}

/*----------------------------------------------------------------------
|   SKB_BaseData_DeriveSha1
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BaseData_DeriveSha1(const SKB_SecureData*               self,
                        const SKB_Sha1DerivationParameters* params,
                        SKB_SecureData**                    data)
{
    const SKB_BytesData* bytes_data;
    unsigned int         i;
    SKB_Byte             digest[20];
    SKB_Result           result       = SKB_SUCCESS;
    const SKB_Byte*      payload      = NULL;
    unsigned long        payload_size = 0;
    unsigned long        digest_size  = sizeof(digest);

    /* paramaters (have default values if NULL) */
    unsigned int  round_count = 1;
    unsigned long output_size = sizeof(digest);
    if (params != NULL) {
        if (params->output_size == 0 ||
            params->output_size > sizeof(digest))
        {
            return SKB_ERROR_INVALID_PARAMETERS;
        }
        round_count = params->round_count;
        output_size = params->output_size;
    }

    /* type checking */
    if (self->type != SKB_DATA_TYPE_BYTES) {
        return SKB_ERROR_INVALID_FORMAT;
    }

    /* get the payload */
    bytes_data   = (const SKB_BytesData*)self;
    payload      = MTK_DataBuffer_GetData(bytes_data->bytes);
    payload_size = MTK_DataBuffer_GetDataSize(bytes_data->bytes);

    if (round_count == 0) {
        /* just copy the key data */
        if (output_size > payload_size) {
            return SKB_ERROR_INVALID_PARAMETERS;
        }
        MTK_CopyMemory(digest, payload, output_size);
    } else {
        int sha1_idx = find_hash("sha1");
        if (sha1_idx == -1) {
            MTK_LOG_SEVERE("looks like sha-1 hasn't been registered");
            return SKB_ERROR_INTERNAL;
        }

        /* compute the first digest */
        result = hash_memory(sha1_idx,
                             payload,
                             payload_size,
                             digest,
                             &digest_size);
        if (result != CRYPT_OK) {
            return SKB_MapTomCryptResult(result);
        }

        /* now do the remaining rounds */
        for (i = 1; i < round_count; i++) {
            result = hash_memory(sha1_idx,
                                 digest,
                                 output_size,
                                 digest,
                                 &digest_size);
            if (result != CRYPT_OK) {
                return SKB_MapTomCryptResult(result);
            }
        }
    }

    return SKB_BytesData_Create(self->engine,
                                self->flags,
                                digest,
                                (SKB_Size)output_size,
                                data);
}

/*----------------------------------------------------------------------
|   SKB_BaseData_Derive
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BaseData_Derive(const SKB_SecureData*   self,
                    SKB_DerivationAlgorithm algorithm,
                    const void*             params,
                    SKB_SecureData**        data)
{
    if (self == NULL || data == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
    *data = NULL;

    switch (algorithm) {
        case SKB_DERIVATION_ALGORITHM_SLICE:
            return SKB_BaseData_DeriveSlice(self,
                                            (const SKB_SliceDerivationParameters*)params,
                                            data);
        case SKB_DERIVATION_ALGORITHM_SHA_1:
            return SKB_BaseData_DeriveSha1(self,
                                           (const SKB_Sha1DerivationParameters*)params,
                                           data);
        default:
            return SKB_ERROR_INTERNAL; /* should never happen */
    }
}

/*----------------------------------------------------------------------
|   SKB_RsaKeyData_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaKeyData_Release(SKB_SecureData* _self)
{
    SKB_RsaKeyData* self = (SKB_RsaKeyData*)_self;
    rsa_free(&self->key);
    MTK_FreeMemory(self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_RsaKeyData_GetClassId
+---------------------------------------------------------------------*/
static MTK_UInt32
SKB_RsaKeyData_GetClassId(const SKB_SecureData* self)
{
    MTK_COMPILER_UNUSED(self);
    return SKB_CLASS_ID_RSA_KEY;
}

/*----------------------------------------------------------------------
|   SKB_RsaKeyData_Serialize
+---------------------------------------------------------------------*/
static SKB_Result
SKB_RsaKeyData_Serialize(const SKB_SecureData* _self,
                         SKB_Byte*             buffer,
                         SKB_Size*             buffer_size)
{
    SKB_RsaKeyData* self = (SKB_RsaKeyData*)_self;
    SKB_Result      result = SKB_SUCCESS;
    SKB_Byte*       out;
    unsigned long   out_size;
    SKB_Byte        dummy_out;

    /* check parameters */
    if (buffer_size == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* see if we're called not to just get the output size */
    if (buffer == NULL) {
        out = &dummy_out; /* we need a non-NULL buffer for rsa_export */
        out_size = 1;
    } else {
        out = buffer;
        out_size = *buffer_size;
    }

    /* try to export the rsa key */
    result = rsa_export(out, &out_size, PK_PRIVATE, &self->key);
    if (result == CRYPT_BUFFER_OVERFLOW) {
        *buffer_size = out_size;
        if (buffer == NULL) {
            return SKB_SUCCESS;
        } else  {
            return SKB_ERROR_BUFFER_TOO_SMALL;
        }
    } else if (result == CRYPT_OK) {
        *buffer_size = out_size;
        return SKB_SUCCESS;
    } else {
        /* unknown error */
        MTK_LOG_SEVERE_1("rsa_export failed with error %d", result);
        return SKB_MapTomCryptResult(result);
    }

    return result;
}

/*----------------------------------------------------------------------
|   SKB_RsaKeyData interface
+---------------------------------------------------------------------*/
static const SKB_SecureDataInterface
SKB_RsaKeyData_Interface = {
    SKB_RsaKeyData_Release,
    SKB_BaseData_GetInfo,
    SKB_BaseData_Derive,
    SKB_RsaKeyData_GetClassId,
    SKB_RsaKeyData_Serialize
};

/*----------------------------------------------------------------------
|   SKB_BytesData_Release
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BytesData_Release(SKB_SecureData* _self)
{
    SKB_BytesData* self = (SKB_BytesData*)_self;
    if (self->bytes) MTK_DataBuffer_Destroy(self->bytes);
    MTK_FreeMemory(self);
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_BytesData_GetClassId
+---------------------------------------------------------------------*/
static MTK_UInt32
SKB_BytesData_GetClassId(const SKB_SecureData* _self)
{
    MTK_COMPILER_UNUSED(_self);
    return SKB_CLASS_ID_BYTES;
}

/*----------------------------------------------------------------------
|   SKB_BytesData_Serialize
+---------------------------------------------------------------------*/
static SKB_Result
SKB_BytesData_Serialize(const SKB_SecureData* _self,
                        SKB_Byte*             buffer,
                        SKB_Size*             buffer_size)
{
    SKB_BytesData* self = (SKB_BytesData*)_self;
    MTK_Size bytes_size = MTK_DataBuffer_GetDataSize(self->bytes);
    if (buffer == NULL) {
        *buffer_size = bytes_size;
        return SKB_SUCCESS;
    } else if (*buffer_size < bytes_size){
        *buffer_size = bytes_size;
        return SKB_ERROR_BUFFER_TOO_SMALL;
    }
    MTK_CopyMemory(buffer, MTK_DataBuffer_GetData(self->bytes), bytes_size);
    *buffer_size = bytes_size;

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_BytesData interface
+---------------------------------------------------------------------*/
static const SKB_SecureDataInterface
SKB_BytesData_Interface = {
    SKB_BytesData_Release,
    SKB_BaseData_GetInfo,
    SKB_BaseData_Derive,
    SKB_BytesData_GetClassId,
    SKB_BytesData_Serialize
};
/*----------------------------------------------------------------------
|   SKB_BytesData_Create
+---------------------------------------------------------------------*/
SKB_Result
SKB_BytesData_Create(SKB_Engine*      engine,
                     unsigned int     flags,
                     const SKB_Byte*  bytes,
                     unsigned int     bytes_size,
                     SKB_SecureData** _data)
{
    SKB_BytesData*  data;

	int data_index = 0;

    *_data = NULL;
    /* allocate */
    data = (SKB_BytesData*)MTK_AllocateZeroMemory(sizeof(SKB_BytesData));
    if (data == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    if (bytes != NULL) {
        /* create the internal buffer */
        MTK_DataBuffer_Create(bytes_size, &data->bytes);
        if(data->bytes == NULL)
        {
            MTK_FreeMemory(data);
            return SKB_ERROR_OUT_OF_RESOURCES;
        }
        MTK_DataBuffer_SetData(data->bytes, bytes, bytes_size);
    } else {
        MTK_FreeMemory(data);
        return SKB_ERROR_OUT_OF_RESOURCES;
    }

    /* fields */
    *_data = (SKB_SecureData*)data;
    data->base.iface  = &SKB_BytesData_Interface;
    data->base.size   = bytes_size;

    /* set the common fields */
    (*_data)->engine = engine;
    (*_data)->type   = SKB_DATA_TYPE_BYTES;
    (*_data)->flags  = flags;

	dprintf("\n\n>> %s: \n", __FUNCTION__);
	data_index = skb_secure_insert(data);
	if(-1 != data_index)
	{
		*_data = (SKB_SecureData*)data_index;
	}
	else
	{
		dprintf("\n\n>> %s: ERROR!!\n", __FUNCTION__);
		*_data = NULL;
	}
	
    return SKB_SUCCESS;
}


/*----------------------------------------------------------------------
|   SKB_RsaKeyData_Create
+---------------------------------------------------------------------*/
SKB_Result
SKB_RsaKeyData_Create(SKB_Engine*      engine,
                      unsigned int     flags,
                      const SKB_Byte*  bytes,
                      unsigned int     bytes_size,
                      MTK_Boolean      from_exported,
                      SKB_SecureData** _data)
{
    int             result;
    SKB_RsaKeyData* data;

	int data_index = 0;


    *_data = NULL;
    /* allocate */
    data = (SKB_RsaKeyData*)MTK_AllocateZeroMemory(sizeof(SKB_RsaKeyData));
    if (data == NULL) return SKB_ERROR_OUT_OF_RESOURCES;

    /* create the rsa key */
    if (from_exported) {
        result = rsa_import(bytes, bytes_size, &data->key); /* PKCS#1 */
    } else {
        result = rsa_import_from_pkcs8(bytes, bytes_size, &data->key); /* PKCS#8 */
    }
    if (result != CRYPT_OK) {
        dprintf("rsa_import failed with %d", result);
        MTK_FreeMemory(data);
        return SKB_MapTomCryptResult(result);
    }

    /* fields */
    *_data = (SKB_SecureData*)data;
    data->base.iface = &SKB_RsaKeyData_Interface;
    data->base.size  = mp_unsigned_bin_size(data->key.N); /* modulus */

    /* set the common fields */
    (*_data)->engine = engine;
    (*_data)->type   = SKB_DATA_TYPE_RSA_PRIVATE_KEY;
    (*_data)->flags  = flags;


	dprintf("\n\n>> %s: \n", __FUNCTION__);
	data_index = skb_secure_insert(data);
	if(-1 != data_index)
	{
		*_data = (SKB_SecureData*)data_index;
	}
	else
	{
		dprintf("\n\n>> %s: ERROR!!\n", __FUNCTION__);
		*_data = NULL;
	}
	


	
    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_GetAes128KeyFromSecureData
+---------------------------------------------------------------------*/
SKB_Result
SKB_GetAes128KeyFromSecureData(const SKB_SecureData* sdata,
                               const SKB_Byte**      key)
{
    *key = NULL;
    if (sdata->type == SKB_DATA_TYPE_BYTES) {
        const SKB_BytesData* key_bytes = (const SKB_BytesData*)sdata;
        if (MTK_DataBuffer_GetDataSize(key_bytes->bytes) != 16) {
            return SKB_ERROR_INVALID_PARAMETERS;
        }
        *key = MTK_DataBuffer_GetData(key_bytes->bytes);
        return SKB_SUCCESS;
    } else {
        return SKB_ERROR_INVALID_PARAMETERS;
    }
}

/*----------------------------------------------------------------------
|   SKB_UnwrapAesCbc
+---------------------------------------------------------------------*/
static SKB_Result
SKB_UnwrapAesCbc(const SKB_Byte*       wrapped,
                 SKB_Size              wrapping_size,
                 const SKB_SecureData* unwrapping_key,
                 MTK_DataBuffer*       unwrapped)
{
    symmetric_CBC   cbc;
    SKB_Result      result;
    const SKB_Byte* key;
    unsigned long   padding_size;
    unsigned long   unwrapped_size = wrapping_size - 16;

    /* check the size */
    if (wrapping_size < 32) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* get the key */
    MTK_CHECK_WARNING(SKB_GetAes128KeyFromSecureData(unwrapping_key, &key));

    /* reserve some space */
    MTK_CHECK_SEVERE(MTK_DataBuffer_Reserve(unwrapped, unwrapped_size));


    /* decrypt */
    result = cbc_start(find_cipher("aes"), wrapped, key, 16, 0, &cbc);
    MTK_CHECK_WARNING(SKB_MapTomCryptResult(result));
    result = cbc_decrypt(wrapped + 16,
                         MTK_DataBuffer_UseData(unwrapped),
                         unwrapped_size,
                         &cbc);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);

    /* now unpad */
    padding_size = MTK_DataBuffer_GetData(unwrapped)[unwrapped_size-1];
    if (padding_size > unwrapped_size || padding_size > 16 || padding_size == 0) {
        dprintf("invalid padding detected");
        result = SKB_ERROR_INVALID_FORMAT;
        goto end;
    }
    unwrapped_size -= padding_size;
    MTK_DataBuffer_SetDataSize(unwrapped, unwrapped_size);

end:
    cbc_done(&cbc);
    return result;
}

/*----------------------------------------------------------------------
|   SKB_UnwrapAesEcb
+---------------------------------------------------------------------*/
static SKB_Result
SKB_UnwrapAesEcb(const SKB_Byte*       wrapped,
                 SKB_Size              wrapping_size,
                 const SKB_SecureData* wrapping_key,
                 MTK_DataBuffer*       unwrapped)
{
    const MTK_Byte* key;
    symmetric_ECB   ecb;
    SKB_Result      result;

    /* check the size */
    if (wrapping_size != 16) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* get the key */
    MTK_CHECK_WARNING(SKB_GetAes128KeyFromSecureData(wrapping_key, &key));

    /* reserve */
    MTK_CHECK_SEVERE(MTK_DataBuffer_Reserve(unwrapped, 16));

    /* decrypt */
    result = ecb_start(find_cipher("aes"), key, 16, 0, &ecb);
    MTK_CHECK_WARNING(SKB_MapTomCryptResult(result));
    result = ecb_decrypt(wrapped,
                         MTK_DataBuffer_UseData(unwrapped),
                         16,
                         &ecb);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);
    MTK_DataBuffer_SetDataSize(unwrapped, 16);

end:
    ecb_done(&ecb);
    return result;
}

/*----------------------------------------------------------------------
|   SKB_UnwrapRsa
+---------------------------------------------------------------------*/
static SKB_Result
SKB_UnwrapRsa(const SKB_Byte*       wrapped,
              SKB_Size              wrapped_size,
              SKB_CipherAlgorithm   wrapping_algorithm,
              const SKB_SecureData* unwrapping_key,
              MTK_DataBuffer*       unwrapped)
{
    rsa_key*      key;
    int           padding;
    SKB_Result    result;
    int           stat;
    unsigned long unwrapped_size;

    /* check the key */
    if (unwrapping_key->type == SKB_DATA_TYPE_RSA_PRIVATE_KEY) {
        key = &((SKB_RsaKeyData*)(unwrapping_key))->key;
    } else {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* check the size */
    if (wrapped_size != mp_unsigned_bin_size(key->N)) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* check the padding */
    switch (wrapping_algorithm) {
        case SKB_CIPHER_ALGORITHM_RSA_1_5:
            padding = LTC_PKCS_1_V1_5;
            break;
        case SKB_CIPHER_ALGORITHM_RSA_OAEP:
            padding = LTC_PKCS_1_OAEP;
            break;
        default:
            return SKB_ERROR_INTERNAL; /* should never get there */
    }

    /* reserve a size which we know will work (upper bound) */
    unwrapped_size = wrapped_size;
    if (MTK_FAILED(MTK_DataBuffer_Reserve(unwrapped, unwrapped_size))) {
        return SKB_ERROR_OUT_OF_RESOURCES;
    }

    /* now decrypt */
    result = rsa_decrypt_key_ex(wrapped,
                                wrapped_size,
                                MTK_DataBuffer_UseData(unwrapped),
                                &unwrapped_size,
                                NULL,
                                0,
                                find_hash("sha1"),
                                padding,
                                &stat,
                                key);
    if (result != CRYPT_OK || !stat) {
        dprintf("rsa decryption failed");
        return SKB_ERROR_INVALID_FORMAT;
    }

    /* set the data size */
    MTK_DataBuffer_SetDataSize(unwrapped, unwrapped_size);

    return SKB_SUCCESS;
}

/*----------------------------------------------------------------------
|   SKB_Unwrap
+---------------------------------------------------------------------*/
static SKB_Result
SKB_Unwrap(const SKB_Byte*       wrapped,
           SKB_Size              wrapped_size,
           SKB_CipherAlgorithm   wrapping_algorithm,
           const void*           wrapping_parameters,
           const SKB_SecureData* unwrapping_key,
           MTK_DataBuffer*       unwrapped)
{
    MTK_COMPILER_UNUSED(wrapping_parameters);




	dprintf("\n\n>> %s: \n", __FUNCTION__);

	dprintf("\n\n>> wrapped:%x: \n", wrapped);
	dprintf("\n\n>> wrapped_size:%x: \n", wrapped_size);
	dprintf("\n\n>> wrapping_algorithm:%x: \n", wrapping_algorithm);
	dprintf("\n\n>> wrapping_parameters:%x: \n", wrapping_parameters);
	dprintf("\n\n>> unwrapping_key:%x: \n", unwrapping_key);
	dprintf("\n\n>> unwrapped:%x: \n", unwrapped);	


	
    if (wrapping_algorithm != SKB_CIPHER_ALGORITHM_NULL &&
        unwrapping_key == NULL) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    /* check the wrapping key type */
    switch (wrapping_algorithm) {
        case SKB_CIPHER_ALGORITHM_NULL:
            return MTK_DataBuffer_SetData(unwrapped, wrapped, wrapped_size);
        case SKB_CIPHER_ALGORITHM_AES_128_CBC:
            return SKB_UnwrapAesCbc(wrapped,
                                    wrapped_size,
                                    unwrapping_key,
                                    unwrapped);
        case SKB_CIPHER_ALGORITHM_AES_128_CTR:
            dprintf("AES CTR not supported for unwrapping keys");
            return SKB_ERROR_NOT_SUPPORTED;
        case SKB_CIPHER_ALGORITHM_AES_128_ECB:
            return SKB_UnwrapAesEcb(wrapped,
                                    wrapped_size,
                                    unwrapping_key,
                                    unwrapped);
        case SKB_CIPHER_ALGORITHM_RSA_1_5:
        case SKB_CIPHER_ALGORITHM_RSA_OAEP:
            return SKB_UnwrapRsa(wrapped,
                                 wrapped_size,
                                 wrapping_algorithm,
                                 unwrapping_key,
                                 unwrapped);
    }
    return SKB_ERROR_INTERNAL; /* should have returned in the switch above */
}






/*----------------------------------------------------------------------
|   SKB_GenericSecureData_CreateFromExported
+---------------------------------------------------------------------*/
SKB_Result
SKB_GenericSecureData_CreateFromExported(SKB_Engine*      engine,
                                         const SKB_Byte*  exported,
                                         SKB_Size         exported_size,
                                         SKB_SecureData** data)
{
    SKB_Result      result  = SKB_SUCCESS;
    SKB_Byte*       payload = NULL;
    unsigned long   payload_size;
    unsigned int    flags;
    MTK_UInt32      class_id;
    symmetric_CBC   cbc;
    MTK_Boolean     cbc_initialized = MTK_FALSE;
    hmac_state      mac;
    MTK_Boolean     mac_initialized = MTK_FALSE;
    unsigned long   mac_size = 20;
    unsigned char   mac_bytes[20];
    const SKB_Byte* export_key;
    unsigned long   padding_size;
    unsigned long   serialized_size;


    /* check parameters: need at least (header) + 32 (IV+min(payload)) + 20 (hmac) */
    if (exported == NULL || data == NULL || exported_size < SKB_EXPORT_HEADER_SIZE+16+32+20) {
        return SKB_ERROR_INVALID_PARAMETERS;
    }

    *data = NULL;



    /* check the magic number */
    if (exported[0] != 'S' ||
        exported[1] != 'K' ||
        exported[2] != 'B' ||
        exported[3] != '!') {
        dprintf("invalid magic number");
        return SKB_ERROR_INVALID_FORMAT;
    }

	//dprintf("magic number ok\n");
	

    /* check the version */
    if (exported[4] != SKB_EXPORT_FORMAT_VERSION) {
        dprintf("version %d unsupported", exported[4]);
        return SKB_ERROR_NOT_SUPPORTED;
    }

	//dprintf("version [%d]\n", exported[4]);





	

    /* export key */
    if (exported[5] > 1) {
        return SKB_ERROR_INVALID_FORMAT;
    }

	//dprintf("exported[5] [%d]\n", exported[5]);

	
    //export_key = exported[5]?
    //             engine->persistent_export_key:
    //             engine->cross_engine_export_key;

    export_key = exported[5]?
                 SKB_Engine_persistent_export_key(engine):
                 SKB_Engine_cross_engine_export_key(engine);

#if 0
	if(export_key = exported[5])
	{

	}
	else
	{

	}
#endif


    /* class id */
    class_id = MTK_BytesToInt32Be(exported + 6);


	//dprintf("class_id [%x]\n", class_id);


    /* flags */
    flags = MTK_BytesToInt32Be(exported + 10);


	//dprintf("flags [%x]\n", flags);

    /* decrypt the payload */
    payload_size = exported_size-SKB_EXPORT_HEADER_SIZE-16-20;


	//dprintf("payload_size [%x]\n", payload_size);

	
    payload = MTK_AllocateMemory(payload_size);



    result = cbc_start(find_cipher("aes"),
                       exported + SKB_EXPORT_HEADER_SIZE,
                       export_key,
                       16,
                       0,
                       &cbc);

    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);
    cbc_initialized = MTK_TRUE;



    result = cbc_decrypt(exported + SKB_EXPORT_HEADER_SIZE + 16,
                         payload,
                         payload_size,
                         &cbc);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);

    /* unpad */
    padding_size = payload[payload_size-1];
    if (padding_size > 16 || padding_size > payload_size || padding_size == 0) {
        dprintf("invalid padding detected\n");
        result = SKB_ERROR_INVALID_FORMAT; goto end;
    }
    payload_size -= padding_size;

    /* compute the mac */
    result = hmac_init(&mac,
                       find_hash("sha1"),
                       export_key,
                       16);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);
	
    result = hmac_process(&mac, exported, SKB_EXPORT_HEADER_SIZE);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);

	
    result = hmac_process(&mac, payload, payload_size);
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);
	
    result = hmac_done(&mac, mac_bytes, &mac_size);
    mac_initialized = MTK_FALSE;
	
    MTK_CHECK_LABEL_WARNING(result = SKB_MapTomCryptResult(result), end);

    /* verify the mac */
    if (!MTK_MemoryEqual(mac_bytes, exported + exported_size - 20, 20)) {
       dprintf("hmac verification failed\n");
	   
       result = SKB_ERROR_INVALID_FORMAT; goto end;
    }

    /* get the serialized size */
    serialized_size = MTK_BytesToInt32Be(payload);

    /* sanity check */
    if (serialized_size+4+4 > payload_size) {
        result = SKB_ERROR_INVALID_FORMAT; goto end;
    }

    /* create the data */
    switch (class_id) {
        case SKB_CLASS_ID_BYTES:
            result = SKB_BytesData_Create(engine,
                                          flags,
                                          payload+4,
                                          serialized_size,
                                          data);
            MTK_CHECK_LABEL_WARNING(result, end);
            break;
        case SKB_CLASS_ID_RSA_KEY:
            result = SKB_RsaKeyData_Create(engine,
                                           flags,
                                           payload+4,
                                           serialized_size,
                                           MTK_TRUE,
                                           data);
            MTK_CHECK_LABEL_WARNING(result, end);
            break;
        default:
            dprintf("Unknown class ID\n");
            result = SKB_ERROR_INVALID_FORMAT; goto end;
    }

    /* here the extensions are just skipped */

end:
    if (payload != NULL) {
        MTK_FreeMemory(payload);
    }
    if (cbc_initialized) {
        cbc_done(&cbc);
    }
    if (mac_initialized) {
        /* idiotic way to cleanup if done hasn't already been called... */
        unsigned char dummy;
        unsigned long dummy_size = 1;
        hmac_done(&mac, &dummy, &dummy_size);
    }
    return result;
}









/*----------------------------------------------------------------------
|   SKB_GenericSecureData_CreateFromWrapped
+---------------------------------------------------------------------*/
SKB_Result
SKB_GenericSecureData_CreateFromWrapped(SKB_Engine*           engine,
                                        const SKB_Byte*       wrapped,
                                        SKB_Size              wrapped_size,
                                        SKB_DataType          wrapped_type,
                                        SKB_DataFormat        wrapped_format,
                                        SKB_CipherAlgorithm   wrapping_algorithm,
                                        const void*           wrapping_parameters,
                                        const SKB_SecureData* in_unwrapping_key_index,
                                        SKB_SecureData**      data)
{
    MTK_DataBuffer* unwrapped = NULL;
    SKB_Result      result;

	int unwrapping_key_index = 0;
	SKB_SecureData *unwrapping_key = NULL;



    /* default value */
    *data = NULL;

    /* create a data buffer to hold the unwrapped data */
    result = MTK_DataBuffer_Create(0, &unwrapped);
    if (MTK_FAILED(result)) return SKB_ERROR_OUT_OF_RESOURCES;




	unwrapping_key_index = (int)in_unwrapping_key_index;

	dprintf("\n\n>> %s: \n", __FUNCTION__);

	unwrapping_key = skb_secure_data(unwrapping_key_index);








    /* unwrap the data */
    result = SKB_Unwrap(wrapped,
                        wrapped_size,
                        wrapping_algorithm,
                        wrapping_parameters,
                        unwrapping_key,
                        unwrapped);
    if (SKB_FAILED(result)) {
        if (unwrapped) MTK_DataBuffer_Destroy(unwrapped);
        dprintf("failed to unwrap data (%d)", result);
        return result;
    }

    /* check the format */
    switch (wrapped_type) {
        case SKB_DATA_TYPE_BYTES:
            /* the format must be RAW */
            if (wrapped_format != SKB_DATA_FORMAT_RAW) {
                result = SKB_ERROR_NOT_SUPPORTED;
                goto end;
            }
            result = SKB_BytesData_Create(engine,
                                          0,
                                          MTK_DataBuffer_GetData(unwrapped),
                                          MTK_DataBuffer_GetDataSize(unwrapped),
                                          data);
            break;
        case SKB_DATA_TYPE_RSA_PRIVATE_KEY:
            /* we only support PKCS8 encoding for RSA private keys */
            if (wrapped_format != SKB_DATA_FORMAT_PKCS8) {
                result = SKB_ERROR_NOT_SUPPORTED;
                goto end;
            }
            result = SKB_RsaKeyData_Create(engine,
                                           0,
                                           MTK_DataBuffer_GetData(unwrapped),
                                           MTK_DataBuffer_GetDataSize(unwrapped),
                                           MTK_FALSE,
                                           data);
            break;
    }

end:
    if (unwrapped) MTK_DataBuffer_Destroy(unwrapped);
    return result;
}

