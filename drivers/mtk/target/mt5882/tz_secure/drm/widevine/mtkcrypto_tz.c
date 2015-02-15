/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * Reference implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "type_c.h"
#include "x_typedef.h"
#include "tzst.h"
#include "OEMCryptoCENC.h"
#include "OEMCryptoArg.h"
#include "rpmb.h"


extern void HalInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);

static inline void TZ_CACHE_STR(const void* addr , size_t size) 
{
    size += (UINT32)addr & TZ_CACHE_LINE_SIZE_MSK;
    addr = (void *)((UINT32)addr&(~TZ_CACHE_LINE_SIZE_MSK));
    TZ_SWD_BufferStart((UINT32)(addr), TZ_CACHE_ALIGN((size)));
}

static inline void TZ_CACHE_END(const void* addr , size_t size)
{
    size += (UINT32)addr & TZ_CACHE_LINE_SIZE_MSK;
    addr = (void *)((UINT32)addr&(~TZ_CACHE_LINE_SIZE_MSK));
    TZ_SWD_BufferEnd((UINT32)(addr), TZ_CACHE_ALIGN((size)));
}

BOOL TZ_OEMCrypto_Initialize(void *arg, UINT32 u4Size)
{
    OEMCryptoResult *rArg = (OEMCryptoResult *)arg;
    if(rArg!=NULL){                 
        *rArg= OEMCrypto_Initialize();        
        return true;
    }
    return false;
}
BOOL TZ_OEMCrypto_Terminate(void *arg, UINT32 u4Size)
{
    OEMCryptoResult *rArg = (OEMCryptoResult *)arg;
    if(rArg!=NULL){                     
        *rArg= OEMCrypto_Terminate();
        return true;        
    }
    return false;    
}
BOOL TZ_OEMCrypto_OpenSession(void *arg, UINT32 u4Size)
{
    OpenSessionArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->session!=NULL)){    
        TZ_CACHE_STR(rArg->session, sizeof(OEMCrypto_SESSION));
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_OpenSession(rArg->session);
        TZ_CACHE_END(rArg->session, sizeof(OEMCrypto_SESSION));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_CloseSession(void *arg, UINT32 u4Size)
{
    CloseSessionArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)){    
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_CloseSession(rArg->session);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GenerateDerivedKeys(void *arg, UINT32 u4Size)
{
    GenerateDerivedKeysArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->mac_key_context!=NULL)&&(rArg->enc_key_context!=NULL)){    
        TZ_CACHE_STR(rArg->mac_key_context, rArg->mac_key_context_length);
        TZ_CACHE_STR(rArg->enc_key_context, rArg->enc_key_context_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));

        *(rArg->result) = OEMCrypto_GenerateDerivedKeys(
                rArg->session,
                rArg->mac_key_context,
                rArg->mac_key_context_length,
                rArg->enc_key_context,
                rArg->enc_key_context_length);

        TZ_CACHE_END(rArg->mac_key_context, rArg->mac_key_context_length);
        TZ_CACHE_END(rArg->enc_key_context, rArg->enc_key_context_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GenerateNonce(void *arg, UINT32 u4Size)
{
    GenerateNonceArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->nonce!=NULL)){    
        TZ_CACHE_STR(rArg->nonce, sizeof(uint32_t));
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));

        *(rArg->result) = OEMCrypto_GenerateNonce(
                rArg->session,
                rArg->nonce);

        TZ_CACHE_END(rArg->nonce, sizeof(uint32_t));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GenerateSignature(void *arg, UINT32 u4Size)
{
    GenerateSignatureArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->message!=NULL)
            &&(rArg->signature_length!=NULL)&&(rArg->signature!=NULL)){    

        TZ_CACHE_STR(rArg->message, rArg->message_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->signature_length, sizeof(size_t));
        TZ_CACHE_STR(rArg->signature, *rArg->signature_length);

        dataSize = *rArg->signature_length;
        *(rArg->result) = OEMCrypto_GenerateSignature(
                rArg->session,
                rArg->message,
                rArg->message_length,
                rArg->signature,
                rArg->signature_length);

        TZ_CACHE_END(rArg->message, rArg->message_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->signature, dataSize);
        TZ_CACHE_END(rArg->signature_length, sizeof(size_t));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_LoadKeys(void *arg, UINT32 u4Size)
{
    LoadKeysArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->message!=NULL)
            &&(rArg->signature!=NULL)&&(rArg->key_array!=NULL)){    

        TZ_CACHE_STR(rArg->message, rArg->message_length);
        TZ_CACHE_STR(rArg->signature, rArg->signature_length);
        TZ_CACHE_STR(rArg->key_array, sizeof(OEMCrypto_KeyObject) * rArg->num_keys);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));

        *(rArg->result) = OEMCrypto_LoadKeys(
                rArg->session,
                rArg->message,
                rArg->message_length,
                rArg->signature,
                rArg->signature_length,
                rArg->enc_mac_keys_iv,
                rArg->enc_mac_keys,
                rArg->num_keys,
                rArg->key_array,
#if WIDEVINE_OEM_API_LEVEL >8
                rArg->pst,        
                rArg->pst_length
#else
                0,0
#endif
                );

        TZ_CACHE_END(rArg->message, rArg->message_length);
        TZ_CACHE_END(rArg->signature, rArg->signature_length);
        TZ_CACHE_END(rArg->key_array, sizeof(OEMCrypto_KeyObject) * rArg->num_keys);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_RefreshKeys(void *arg, UINT32 u4Size)
{
    RefreshKeysArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->key_array!=NULL)
            &&(rArg->message!=NULL)&&(rArg->signature!=NULL)){    

        TZ_CACHE_STR(rArg->message, rArg->message_length);
        TZ_CACHE_STR(rArg->signature, rArg->signature_length);
        TZ_CACHE_STR(rArg->key_array, sizeof(OEMCrypto_KeyRefreshObject) * rArg->num_keys);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_RefreshKeys(
                rArg->session,
                rArg->message,
                rArg->message_length,
                rArg->signature,
                rArg->signature_length,
                rArg->num_keys,
                rArg->key_array);
        TZ_CACHE_END(rArg->message, rArg->message_length);
        TZ_CACHE_END(rArg->signature, rArg->signature_length);
        TZ_CACHE_END(rArg->key_array, sizeof(OEMCrypto_KeyRefreshObject) * rArg->num_keys);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;
    }
    return false;
}
BOOL TZ_OEMCrypto_SelectKey(void *arg, UINT32 u4Size)
{
    SelectKeyArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->key_id!=NULL)){        
        TZ_CACHE_STR(rArg->key_id, rArg->key_id_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_SelectKey(
                rArg->session,
                rArg->key_id,
                rArg->key_id_length);
        TZ_CACHE_END(rArg->key_id, rArg->key_id_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_DecryptCTR(void *arg, UINT32 u4Size)
{
    DecryptCTRArg *rArg =arg;
    BOOL ret = false;

    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->data_addr!=NULL)
            &&(rArg->out_buffer!=NULL)&&(rArg->iv!=NULL)){    

        TZ_CACHE_STR(rArg->data_addr, rArg->data_length);
        TZ_CACHE_STR(rArg->out_buffer, sizeof(OEMCrypto_DestBufferDesc));
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->iv, KEY_IV_SIZE);    


        switch (rArg->out_buffer->type) {
            case OEMCrypto_BufferType_Clear:
                if(rArg->out_buffer->buffer.clear.address==NULL)
                    goto final;                
                TZ_CACHE_STR(rArg->out_buffer->buffer.clear.address, rArg->out_buffer->buffer.clear.max_length);
                break;
            case OEMCrypto_BufferType_Secure:
                if(rArg->out_buffer->buffer.secure.handle + rArg->out_buffer->buffer.secure.offset ==NULL)
                    goto final;                
                HalInvalidateDCacheMultipleLine((UINT32)rArg->out_buffer->buffer.secure.handle + rArg->out_buffer->buffer.secure.offset,
                        rArg->out_buffer->buffer.secure.max_length);
                break;
            default:
                break;
        }

        *(rArg->result) = OEMCrypto_DecryptCTR(
                rArg->session,
                rArg->data_addr,
                rArg->data_length,
                rArg->is_encrypted,
                rArg->iv,
                rArg->block_offset,
                rArg->out_buffer,
                rArg->subsample_flags);

        switch (rArg->out_buffer->type) {
            case OEMCrypto_BufferType_Clear:
                TZ_CACHE_END(rArg->out_buffer->buffer.clear.address, rArg->out_buffer->buffer.clear.max_length);
                break;
            case OEMCrypto_BufferType_Secure:
                HalFlushInvalidateDCacheMultipleLine((UINT32)rArg->out_buffer->buffer.secure.handle + rArg->out_buffer->buffer.secure.offset,
                        rArg->out_buffer->buffer.secure.max_length);                
                break;
            default:
                break;
        }
        ret = true;
final:    

        TZ_CACHE_END(rArg->data_addr, rArg->data_length);
        TZ_CACHE_END(rArg->out_buffer, sizeof(OEMCrypto_DestBufferDesc));    
        TZ_CACHE_END(rArg->iv, KEY_IV_SIZE);    
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));

        return ret;
    }
    return false;
}
BOOL TZ_OEMCrypto_InstallKeybox(void *arg, UINT32 u4Size)
{
    InstallKeyboxArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->keybox!=NULL)){            
        TZ_CACHE_STR(rArg->keybox, rArg->keyBoxLength);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_InstallKeybox(
                rArg->keybox,
                rArg->keyBoxLength);
        TZ_CACHE_END(rArg->keybox, rArg->keyBoxLength);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_IsKeyboxValid(void *arg, UINT32 u4Size)
{
    OEMCryptoResult *rArg = (OEMCryptoResult *)arg;
    if(rArg!=NULL){                
        *rArg = OEMCrypto_IsKeyboxValid();
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GetDeviceID(void *arg, UINT32 u4Size)
{
    GetDeviceIDArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->idLength!=NULL)&&(rArg->deviceID!=NULL)){               
        TZ_CACHE_STR(rArg->idLength, sizeof(size_t));
        TZ_CACHE_STR(rArg->deviceID, *rArg->idLength);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        dataSize = *rArg->idLength;
        *(rArg->result) = OEMCrypto_GetDeviceID(
                rArg->deviceID,
                rArg->idLength);
        TZ_CACHE_END(rArg->deviceID, dataSize);
        TZ_CACHE_END(rArg->idLength, sizeof(size_t));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GetKeyData(void *arg, UINT32 u4Size)
{
    GetKeyDataArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->keyDataLength!=NULL)&&(rArg->keyData!=NULL)){               
        TZ_CACHE_STR(rArg->keyDataLength, sizeof(size_t));
        TZ_CACHE_STR(rArg->keyData, *rArg->keyDataLength);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));

        dataSize = *rArg->keyDataLength;
        *(rArg->result) = OEMCrypto_GetKeyData(
                rArg->keyData,
                rArg->keyDataLength);

        TZ_CACHE_END(rArg->keyData, dataSize);
        TZ_CACHE_END(rArg->keyDataLength, sizeof(size_t));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GetRandom(void *arg, UINT32 u4Size)
{
    GetRandomArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->randomData!=NULL)){               

        TZ_CACHE_STR(rArg->randomData, rArg->dataLength);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_GetRandom(
                rArg->randomData,
                rArg->dataLength);
        TZ_CACHE_END(rArg->randomData, rArg->dataLength);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_WrapKeybox(void *arg, UINT32 u4Size)
{
    WrapKeyboxArg *rArg =arg;
    UINT32 dataSize;

    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->keybox!=NULL)&&(rArg->transportKey!=NULL)
            &&(rArg->wrappedKeyBoxLength!=NULL)&&(rArg->wrappedKeybox!=NULL)
      ){               

        TZ_CACHE_STR(rArg->keybox, rArg->keyBoxLength);
        TZ_CACHE_STR(rArg->transportKey, rArg->transportKeyLength);
        TZ_CACHE_STR(rArg->wrappedKeyBoxLength, sizeof(size_t));
        TZ_CACHE_STR(rArg->wrappedKeybox, *rArg->wrappedKeyBoxLength);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));

        dataSize = *rArg->wrappedKeyBoxLength;
        *(rArg->result) = OEMCrypto_WrapKeybox(
                rArg->keybox,
                rArg->keyBoxLength,
                rArg->wrappedKeybox,
                rArg->wrappedKeyBoxLength,
                rArg->transportKey,
                rArg->transportKeyLength);

        TZ_CACHE_END(rArg->keybox, rArg->keyBoxLength);
        TZ_CACHE_END(rArg->transportKey, rArg->transportKeyLength);
        TZ_CACHE_END(rArg->wrappedKeybox, dataSize);
        TZ_CACHE_END(rArg->wrappedKeyBoxLength, sizeof(size_t));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_RewrapDeviceRSAKey(void *arg, UINT32 u4Size)
{
    RewrapDeviceRSAKeyArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->message!=NULL)&&(rArg->wrapped_rsa_key_length!=NULL)&&(rArg->wrapped_rsa_key!=NULL)
      ){               


        TZ_CACHE_STR(rArg->message, rArg->message_length);
        TZ_CACHE_STR(rArg->wrapped_rsa_key_length, sizeof(size_t));
        TZ_CACHE_STR(rArg->wrapped_rsa_key, *rArg->wrapped_rsa_key_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        dataSize = *rArg->wrapped_rsa_key_length;
        *(rArg->result) = OEMCrypto_RewrapDeviceRSAKey(
                rArg->session,
                rArg->message,
                rArg->message_length,
                rArg->signature,
                rArg->signature_length,
                rArg->nonce,
                rArg->enc_rsa_key,
                rArg->enc_rsa_key_length,
                rArg->enc_rsa_key_iv,
                rArg->wrapped_rsa_key,
                rArg->wrapped_rsa_key_length);
        TZ_CACHE_END(rArg->message, rArg->message_length);
        TZ_CACHE_END(rArg->wrapped_rsa_key, dataSize);
        TZ_CACHE_END(rArg->wrapped_rsa_key_length, sizeof(size_t));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_LoadDeviceRSAKey(void *arg, UINT32 u4Size)
{
    LoadDeviceRSAKeyArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->wrapped_rsa_key!=NULL)
      ){ 
        TZ_CACHE_STR(rArg->wrapped_rsa_key, rArg->wrapped_rsa_key_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_LoadDeviceRSAKey(
                rArg->session,
                rArg->wrapped_rsa_key,
                rArg->wrapped_rsa_key_length);
        TZ_CACHE_END(rArg->wrapped_rsa_key, rArg->wrapped_rsa_key_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_GenerateRSASignature(void *arg, UINT32 u4Size)
{
    GenerateRSASignatureArg *rArg =arg;
    UINT32 dataSize;

    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->message!=NULL)&&(rArg->signature_length!=NULL)&&(rArg->signature!=NULL)
      ){               
        TZ_CACHE_STR(rArg->message, rArg->message_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->signature_length, sizeof(size_t));
        TZ_CACHE_STR(rArg->signature, *rArg->signature_length);
        dataSize = *rArg->signature_length;
        *(rArg->result) = OEMCrypto_GenerateRSASignature(
                rArg->session,
                rArg->message,
                rArg->message_length,
                rArg->signature,
                rArg->signature_length,
                rArg->padding_scheme);
        TZ_CACHE_END(rArg->message, rArg->message_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->signature, dataSize);
        TZ_CACHE_END(rArg->signature_length, sizeof(size_t));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_DeriveKeysFromSessionKey(void *arg, UINT32 u4Size)
{
    DeriveKeysFromSessionKeyArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->enc_session_key!=NULL)&&(rArg->mac_key_context!=NULL)&&(rArg->enc_key_context!=NULL)
      ){     
        TZ_CACHE_STR(rArg->enc_session_key, rArg->enc_session_key_length);
        TZ_CACHE_STR(rArg->mac_key_context, rArg->mac_key_context_length);
        TZ_CACHE_STR(rArg->enc_key_context, rArg->enc_key_context_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_DeriveKeysFromSessionKey(
                rArg->session,
                rArg->enc_session_key,
                rArg->enc_session_key_length,
                rArg->mac_key_context,
                rArg->mac_key_context_length,
                rArg->enc_key_context,
                rArg->enc_key_context_length);
        TZ_CACHE_END(rArg->enc_session_key, rArg->enc_session_key_length);
        TZ_CACHE_END(rArg->mac_key_context, rArg->mac_key_context_length);
        TZ_CACHE_END(rArg->enc_key_context, rArg->enc_key_context_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_Generic_Encrypt(void *arg, UINT32 u4Size)
{
    GenericEncryptArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->in_buffer!=NULL)&&(rArg->iv!=NULL)&&(rArg->out_buffer!=NULL)
      ){       
        TZ_CACHE_STR(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_STR(rArg->iv, KEY_IV_SIZE);
        TZ_CACHE_STR(rArg->out_buffer, rArg->buffer_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_Generic_Encrypt(
                rArg->session,
                rArg->in_buffer,
                rArg->buffer_length,
                rArg->iv,
                rArg->algorithm,
                rArg->out_buffer);
        TZ_CACHE_END(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_END(rArg->iv, KEY_IV_SIZE);
        TZ_CACHE_END(rArg->out_buffer, rArg->buffer_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_Generic_Decrypt(void *arg, UINT32 u4Size)
{
    GenericDecryptArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->in_buffer!=NULL)&&(rArg->iv!=NULL)&&(rArg->out_buffer!=NULL)
      ){       

        TZ_CACHE_STR(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_STR(rArg->iv, KEY_IV_SIZE);
        TZ_CACHE_STR(rArg->out_buffer, rArg->buffer_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_Generic_Decrypt(
                rArg->session,
                rArg->in_buffer,
                rArg->buffer_length,
                rArg->iv,
                rArg->algorithm,
                rArg->out_buffer);
        TZ_CACHE_END(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_END(rArg->iv, KEY_IV_SIZE);
        TZ_CACHE_END(rArg->out_buffer, rArg->buffer_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_Generic_Sign(void *arg, UINT32 u4Size)
{
    GenericSignArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->in_buffer!=NULL)&&(rArg->signature_length!=NULL)&&(rArg->signature!=NULL)
      ){        
        TZ_CACHE_STR(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_STR(rArg->signature_length, sizeof(size_t));
        TZ_CACHE_STR(rArg->signature, *rArg->signature_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));

        dataSize = *rArg->signature_length;
        *(rArg->result) = OEMCrypto_Generic_Sign(
                rArg->session,
                rArg->in_buffer,
                rArg->buffer_length,
                rArg->algorithm,
                rArg->signature,
                rArg->signature_length);
        TZ_CACHE_END(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_END(rArg->signature, dataSize);
        TZ_CACHE_END(rArg->signature_length, sizeof(size_t));
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}
BOOL TZ_OEMCrypto_Generic_Verify(void *arg, UINT32 u4Size)
{
    GenericVerifyArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->in_buffer!=NULL)&&(rArg->signature!=NULL)
      ){      
        TZ_CACHE_STR(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_STR(rArg->signature, rArg->signature_length);
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        *(rArg->result) = OEMCrypto_Generic_Verify(
                rArg->session,
                rArg->in_buffer,
                rArg->buffer_length,
                rArg->algorithm,
                rArg->signature,
                rArg->signature_length);
        TZ_CACHE_END(rArg->in_buffer, rArg->buffer_length);
        TZ_CACHE_END(rArg->signature, rArg->signature_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_UpdateUsageTable(void *arg, UINT32 u4Size)
{
    OEMCryptoResult *rArg = (OEMCryptoResult *)arg;
    if(rArg!=NULL){
        *rArg= OEMCrypto_UpdateUsageTable();
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_DeactivateUsageEntry(void *arg, UINT32 u4Size)
{
    DeactivateUsageEntryArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->pst!=NULL)
      ){  
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));    
        TZ_CACHE_STR(rArg->pst, rArg->pst_length);
        *(rArg->result) = OEMCrypto_DeactivateUsageEntry(
                rArg->pst,
                rArg->pst_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->pst, rArg->pst_length);
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_ReportUsage(void *arg, UINT32 u4Size)
{
    ReportUsageArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->pst!=NULL)&&(rArg->buffer_length!=NULL)&&(rArg->buffer!=NULL)
      ){ 

        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));    
        TZ_CACHE_STR(rArg->pst, rArg->pst_length);
        TZ_CACHE_STR(rArg->buffer_length, sizeof(size_t));
        TZ_CACHE_STR(rArg->buffer, *rArg->buffer_length);
        dataSize = *rArg->buffer_length;    
        *(rArg->result) = OEMCrypto_ReportUsage(
                rArg->session,
                rArg->pst,
                rArg->pst_length,        
                rArg->buffer,
                rArg->buffer_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));    
        TZ_CACHE_END(rArg->pst, rArg->pst_length);
        TZ_CACHE_END(rArg->buffer, dataSize);    
        TZ_CACHE_END(rArg->buffer_length, sizeof(size_t));
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_DeleteUsageEntry(void *arg, UINT32 u4Size)
{
    DeleteUsageEntryArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->message!=NULL)&&(rArg->signature!=NULL)
      ){ 
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));    
        //TZ_CACHE_STR(rArg->pst, rArg->pst_length);
        TZ_CACHE_STR(rArg->message, rArg->message_length);
        TZ_CACHE_STR(rArg->signature, rArg->signature_length);
        *(rArg->result) = OEMCrypto_DeleteUsageEntry(
                rArg->session,
                rArg->pst,
                rArg->pst_length,        
                rArg->message,
                rArg->message_length,
                rArg->signature,
                rArg->signature_length);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));    
        //TZ_CACHE_END(rArg->pst, rArg->pst_length);
        TZ_CACHE_END(rArg->message, rArg->message_length);
        TZ_CACHE_END(rArg->signature, rArg->signature_length);
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_DeleteUsageTable(void *arg, UINT32 u4Size)
{
    OEMCryptoResult *rArg = (OEMCryptoResult *)arg;
    if(rArg!=NULL){
        *rArg= OEMCrypto_DeleteUsageTable();
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_SetEntitlementKey(void *arg, UINT32 u4Size)
{
    SetEntitlementKeyArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->emmKey!=NULL)
      ){ 

        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->emmKey, rArg->emmKeyLength);

        *(rArg->result) = OEMCrypto_SetEntitlementKey(rArg->emmKey,
                rArg->emmKeyLength);

        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->emmKey, rArg->emmKeyLength);
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_DeriveControlWord(void *arg, UINT32 u4Size)
{
    DeriveControlWordArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->ecm!=NULL)&&(rArg->flags!=NULL)
      ){ 
        TZ_CACHE_STR(rArg->result , sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->ecm    , rArg->length);
        TZ_CACHE_STR(rArg->flags  , sizeof(uint32_t));

        *(rArg->result) = OEMCrypto_DeriveControlWord(rArg->ecm,
                rArg->length,
                rArg->flags);

        TZ_CACHE_END(rArg->result , sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->ecm    , rArg->length);
        TZ_CACHE_END(rArg->flags  , sizeof(uint32_t));
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_DecryptVideo(void *arg, UINT32 u4Size)
{
    DecryptVideoArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->input!=NULL)        
            &&(rArg->outputLength!=NULL)&&(rArg->output_handle!=NULL)
      ){
        TZ_CACHE_STR(rArg->result,sizeof(OEMCryptoResult));
        if((rArg->iv!=NULL)){
            TZ_CACHE_STR(rArg->iv, KEY_IV_SIZE );
        }
        TZ_CACHE_STR(rArg->input, rArg->inputLength );
        TZ_CACHE_STR(rArg->outputLength, sizeof(uint32_t) );

        dataSize = *rArg->outputLength;       
        HalInvalidateDCacheMultipleLine(rArg->output_handle, dataSize);

        *(rArg->result) = OEMCrypto_DecryptVideo(rArg->iv,
                rArg->input,
                rArg->inputLength,
                rArg->output_handle,
                rArg->output_offset,
                rArg->outputLength);

        TZ_CACHE_END(rArg->result,sizeof(OEMCryptoResult));
        if((rArg->iv!=NULL)){        
            TZ_CACHE_END(rArg->iv, KEY_IV_SIZE );
        }
        TZ_CACHE_END(rArg->input, rArg->inputLength );
        TZ_CACHE_END(rArg->outputLength, sizeof(uint32_t) );
        HalFlushInvalidateDCacheMultipleLine(rArg->output_handle, dataSize);
        
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_DecryptAudio(void *arg, UINT32 u4Size)
{
    DecryptAudioArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->input!=NULL)        
            &&(rArg->outputLength!=NULL)&&(rArg->output!=NULL)
      ){ 
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        if((rArg->iv!=NULL)){        
            TZ_CACHE_STR(rArg->iv, KEY_IV_SIZE); 
        }
        TZ_CACHE_STR(rArg->input, rArg->inputLength );
        TZ_CACHE_STR(rArg->outputLength, sizeof(uint32_t) );
        TZ_CACHE_STR(rArg->output, *rArg->outputLength);
        dataSize = *rArg->outputLength;
        *(rArg->result) =OEMCrypto_DecryptAudio( rArg->iv,
                rArg->input,
                rArg->inputLength,
                rArg->output,
                rArg->outputLength);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        if((rArg->iv!=NULL)){        
            TZ_CACHE_END(rArg->iv, KEY_IV_SIZE); 
        }
        TZ_CACHE_END(rArg->input, rArg->inputLength );
        TZ_CACHE_END(rArg->output, dataSize);
        TZ_CACHE_END(rArg->outputLength, sizeof(uint32_t));
        return true;        
    }
    return false;
}


BOOL TZ_OEMCrypto_GetUsageTableWriteRequest(void *arg, UINT32 u4Size)
{

    GetUsageTableWriteRequestArg *rArg =arg;
    UINT32 dataSize;
    if((rArg!=NULL)&&(rArg->result!=NULL)
            &&(rArg->buffer_length!=NULL)&&(rArg->buffer!=NULL)        
            &&(rArg->dFrame!=NULL)
      ){ 
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->buffer_length, sizeof(size_t));
        TZ_CACHE_STR(rArg->buffer, *rArg->buffer_length );
        TZ_CACHE_STR(rArg->dFrame, sizeof(RPMB_DATAFRAM));

        dataSize = *rArg->buffer_length;

        *(rArg->result) =OEMCrypto_GetUsageTableWriteRequest(
                rArg->buffer,
                rArg->buffer_length,
                rArg->dFrame);

        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->buffer_length, sizeof(size_t));
        TZ_CACHE_END(rArg->buffer, dataSize);
        TZ_CACHE_END(rArg->dFrame, sizeof(RPMB_DATAFRAM));
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_VerifyWriteResponse(void *arg, UINT32 u4Size)
{

    VerifyWriteResponseArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)){ 
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->dFrame, sizeof(RPMB_DATAFRAM));    
        *(rArg->result) =OEMCrypto_VerifyWriteResponse(rArg->dFrame);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->dFrame, sizeof(RPMB_DATAFRAM));    
        return true;        
    }
    return false;
}

BOOL TZ_OEMCrypto_GetUsageTableReadRequest(void *arg, UINT32 u4Size){
    GetUsageTableReadRequesArg *rArg =arg;

    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->dFrame!=NULL)){ 
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->dFrame, sizeof(RPMB_DATAFRAM));    
        *(rArg->result) =OEMCrypto_GetUsageTableReadRequest(rArg->dFrame);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->dFrame, sizeof(RPMB_DATAFRAM));
        return true;                
    }
    return false;
}

BOOL TZ_OEMCrypto_VerifyReadResponse(void *arg, UINT32 u4Size){

    VerifyReadResponseArg *rArg =arg;
    if((rArg!=NULL)&&(rArg->result!=NULL)&&(rArg->dFrame!=NULL)){ 
        TZ_CACHE_STR(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_STR(rArg->dFrame, sizeof(RPMB_DATAFRAM));        
        TZ_CACHE_STR(rArg->buffer, rArg->buffer_length);
        *(rArg->result) =OEMCrypto_VerifyReadResponse( rArg->buffer, rArg->buffer_length, rArg->dFrame);
        TZ_CACHE_END(rArg->result, sizeof(OEMCryptoResult));
        TZ_CACHE_END(rArg->buffer, rArg->buffer_length);    
        TZ_CACHE_END(rArg->dFrame, sizeof(RPMB_DATAFRAM));  
        return true;        
    }
    return false;
}

