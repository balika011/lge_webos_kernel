/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * Reference implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "type_c.h"

#include "OEMCryptoCENC.h"

#include "log.h"
#include "mtkcrypto_engine.h"
#include "mtkcrypto_sessionctx.h"
#include "mtkcrypto_usage_table.h"

#include "mtkutil_vector.h"
#include "mtkcrypto_glue.h"
#include "tee_securestorage.h"

#include "wv_cdm_constants.h"
#include "wv_keybox.h"


#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT
#include "rpmb.h"
#endif

CryptoEngine* crypto_engine = NULL;

extern WidevineKeybox kDefaultKeybox;

#define DEV_KEYBOX_SIZE 128
#define FACTORY_KEYBOX_SIZE 180

//Response Didn't send back within constrained time
#if defined(CC_WIDEVINE_SUPPORT_USAGE_REPORT)&&!defined(NO_PERSIST_USAGE_TABLE)

static unsigned char writeReqWait =0;
static unsigned char rpmbfailed   =0;

#define RPMBCHECK() do{if(rpmbfailed||(!UTRPMBUpdateChecker())) {rpmbfailed=1 ; return OEMCrypto_ERROR_UNKNOWN_FAILURE;}}while(0);

//Response Didn't send back yet.
#define RPMB_WAITING_CHECK() do{if(rpmbfailed||UTWaitingEcho()) {rpmbfailed=1 ; return OEMCrypto_ERROR_UNKNOWN_FAILURE;}}while(0);

#else
#define RPMBCHECK() 
#define RPMB_WAITING_CHECK() 
#endif

//typedef struct {
//    uint8_t signature[MAC_KEY_SIZE];
//    uint8_t context[MAC_KEY_SIZE];
//    uint8_t iv[KEY_IV_SIZE];
//#ifdef TODO
////    enc_rsa_key is a variable size
//#else
//    uint8_t enc_rsa_key[512];
//#endif    
//} WrappedRSAKey;


#define dump_hex(...)
#define dump_array_part(...)

//static void dump_hex(String name, const uint8_t* vector, size_t length) {
//    // printf("%s = ", name.c_str());
//    // if (vector == NULL) {
//    //   printf("NULL;\n");
//    //   return;
//    // }
//    // // TODO(fredgc): replace with HEXEncode.
//    // for (size_t i = 0; i < length; i++) {
//    //   if (i == 0) {
//    //     printf("\n     a2b_hex(\"");
//    //   } else if (i % 32 == 0) {
//    //     printf("\"\n                  \"");
//    //   }
//    //   printf("%02X", vector[i]);
//    // }
//    // printf("\");\n");
//}
//
//static void dump_array_part(String array, size_t index,
//        String name, const uint8_t* vector, size_t length) {
//    //if (vector == NULL) {
//    //  printf("%s[%zu].%s = NULL;\n", array.c_str(), index, name.c_str());
//    //  return;
//    //}
//    //printf("String s%zu_", index);
//    //dump_hex(name, vector, length);
//    //printf("%s[%zu].%s =  message_ptr + message.find(s%zu_%s.data());\n",
//    //array.c_str(), index, name.c_str(), index, name.c_str());
//}

OEMCryptoResult OEMCrypto_Initialize(void) {

    if(crypto_engine!=NULL){
        return OEMCrypto_SUCCESS;
    }
    
    //RPMBCHECK();
    
    HAL_InitTimer();
    
    crypto_engine = CECreate();

    if (!crypto_engine || !CEInitialized(crypto_engine)) {
        LOGE("[OEMCrypto_Initialize(): failed]");
        return OEMCrypto_ERROR_INIT_FAILED;
    }
    LOGD("[OEMCrypto_Initialize(): success]");

    return OEMCrypto_SUCCESS;
}


OEMCryptoResult OEMCrypto_Terminate(void) {

    //RPMBCHECK();
    
    if (!crypto_engine) {
        LOGE("[OEMCrypto_Terminate(): failed]");
        return OEMCrypto_ERROR_TERMINATE_FAILED;
    }

    if (CEInitialized(crypto_engine)) {
        CETerminate(crypto_engine);
    }

    CEDestroy(crypto_engine);
    crypto_engine = NULL;
    LOGD("[OEMCrypto_Terminate(): success]");
    return OEMCrypto_SUCCESS;
}


OEMCryptoResult OEMCrypto_OpenSession(OEMCrypto_SESSION* session) {
    SessionId sid;
    RPMBCHECK();

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    sid = CECreateSession(crypto_engine);

    *session = (OEMCrypto_SESSION)sid;
    LOGD("[OEMCrypto_OpenSession(): SID=%08x]", sid);
    return OEMCrypto_SUCCESS;
}


OEMCryptoResult OEMCrypto_CloseSession(OEMCrypto_SESSION session) {

    RPMBCHECK();
    
    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    if (!CEDestroySession(crypto_engine,(SessionId)session)) {
        LOGD("[OEMCrypto_CloseSession(SID=%08X): failed]", session);
        return OEMCrypto_ERROR_CLOSE_SESSION_FAILED;
    } else {
        LOGD("[OEMCrypto_CloseSession(SID=%08X): success]", session);
        return OEMCrypto_SUCCESS;
    }
}


OEMCryptoResult OEMCrypto_GenerateNonce(OEMCrypto_SESSION session,
        uint32_t* nonce) {
    uint32_t nonce_value;
    uint8_t* nonce_string = (uint8_t*)(&nonce_value);
    SessionContext* session_ctx;
#if WIDEVINE_OEM_API_LEVEL > 8
    static uint32_t last_nonce_time = 0;
    static int nonce_count = 0;    
    uint32_t now;
    
    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }  

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        return OEMCrypto_ERROR_INVALID_SESSION;
    }
    
    now = SCTXCurrentTimer(NULL);
   
    if (now == last_nonce_time) {
          nonce_count++;
          if (nonce_count > 20) {
                return OEMCrypto_ERROR_UNKNOWN_FAILURE;
          }
    } else {
          nonce_count = 1;
          last_nonce_time = now;
    }
#else
    if(crypto_engine==NULL){
        return NULL;
    }  
    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        return OEMCrypto_ERROR_INVALID_SESSION;
    }    
#endif

    RPMBCHECK();

    // Generate 4 bytes of random data
    if (!RAND_GEN(nonce_string, 4)) {
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    SCTXAddNonce(session_ctx,nonce_value);
    *nonce = nonce_value;

    return OEMCrypto_SUCCESS;
}


OEMCryptoResult OEMCrypto_GenerateDerivedKeys(OEMCrypto_SESSION session,
        const uint8_t* mac_key_context,
        uint32_t mac_key_context_length,
        const uint8_t* enc_key_context,
        uint32_t enc_key_context_length) {

    SessionContext* session_ctx;

    Vector mac_ctx_str;
    Vector enc_ctx_str;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    OEMCryptoResult result =  OEMCrypto_SUCCESS;;

    VecInit(&mac_ctx_str);
    VecInit(&enc_ctx_str);


    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_GenerateDerivedKeys(): ERROR_KEYBOX_INVALID]");
        result =  OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_GenerateDerivedKeys(): ERROR_NO_INVALID_SESSION]");
        result =  OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }


    VecAssign(&mac_ctx_str , mac_key_context , mac_key_context_length);
    VecAssign(&enc_ctx_str , enc_key_context , enc_key_context_length);

    // Generate mac and encryption keys for current session context
    if (!SCTXDeriveKeys(session_ctx,
                &crypto_engine->keybox.device_key.value,
                &mac_ctx_str,   &enc_ctx_str)) {
        result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        goto final;
    }

final:

    VecTerm(&mac_ctx_str);
    VecTerm(&enc_ctx_str);

    return result;
}


OEMCryptoResult OEMCrypto_GenerateSignature(
        OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        uint8_t* signature,
        size_t* signature_length) {
    SessionContext* session_ctx;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_GenerateSignature(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    if(signature_length ==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    if (*signature_length < SHA256_DIGEST_LENGTH) {
        *signature_length = SHA256_DIGEST_LENGTH;
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }

    if (message == NULL || message_length == 0 ||
            signature == NULL || signature_length == 0) {
        LOGE("[OEMCrypto_GenerateSignature(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_GenerateSignature(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }

    if (SCTXGenerateSignature(session_ctx,
                message, message_length,
                signature, signature_length)) {

        return OEMCrypto_SUCCESS;
    }
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;;
}

static inline bool RangeCheck(const uint8_t* message,
        uint32_t message_length,
        const uint8_t* field,
        uint32_t field_length,
        bool allow_null) {
    if (field == NULL) return allow_null;
    if (field < message) return false;
    if (field + field_length > message + message_length) return false;
    return true;
}


OEMCryptoResult OEMCrypto_LoadKeys(OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        const uint8_t* signature,
        size_t signature_length,
        const uint8_t* enc_mac_key_iv,
        const uint8_t* enc_mac_keys,
        size_t num_keys,
        const OEMCrypto_KeyObject* key_array,
        const uint8_t* pst,
        size_t pst_length) {

    unsigned int i; 
    SessionContext* session_ctx;

    OEMCryptoResult result =  OEMCrypto_SUCCESS;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_LoadKeys(): ERROR_KEYBOX_INVALID]");
        result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;

    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_LoadKeys(): ERROR_NO_INVALID_SESSION]");
        result = OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }

    if (message == NULL || message_length == 0 ||
            signature == NULL || signature_length == 0 ||
            key_array == NULL || num_keys == 0) {
        LOGE("[OEMCrypto_LoadKeys(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;

    }

    if ((pst == NULL && pst_length > 0) || (pst != NULL && pst_length == 0)){    
        LOGE("[OEMCrypto_LoadKeys(): OEMCrypto_ERROR_INVALID_ONCTEXT - null pst.]");    
        return OEMCrypto_ERROR_INVALID_CONTEXT;  
    }

    // Range check
    if (!RangeCheck(message, message_length, enc_mac_keys, 2*MAC_KEY_SIZE, true) ||
        !RangeCheck(message, message_length, enc_mac_key_iv,  KEY_IV_SIZE, true) ||
        !RangeCheck(message, message_length, pst, pst_length, true) 
        ) 
    {
        LOGE("[OEMCrypto_LoadKeys(): OEMCrypto_ERROR_SIGNATURE_FAILURE - range check.]");
        result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto final;

    }

    for (i = 0; i < num_keys; i++) {
        if (!RangeCheck(message, message_length, key_array[i].key_id,
                    key_array[i].key_id_length, false) ||
                !RangeCheck(message, message_length, key_array[i].key_data,
                    key_array[i].key_data_length, false) ||
                !RangeCheck(message, message_length, key_array[i].key_data_iv,
                    KEY_IV_SIZE, false) ||
                !RangeCheck(message, message_length, key_array[i].key_control,
                    KEY_CONTROL_SIZE, false) ||
                !RangeCheck(message, message_length, key_array[i].key_control_iv,
                    KEY_IV_SIZE, false)) {
            LOGE("[OEMCrypto_LoadKeys(): OEMCrypto_ERROR_SIGNATURE_FAILURE -range check %d]", i);
            result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            goto final;

        }
    }
    
    result =  SCTXLoadKeys(session_ctx , 
        message, message_length, 
        signature, signature_length, 
        enc_mac_key_iv, enc_mac_keys,
        num_keys, key_array, 
        pst, pst_length);
final:
    return result;
}


OEMCryptoResult OEMCrypto_RefreshKeys(OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        const uint8_t* signature,
        size_t signature_length,
        size_t num_keys,
        const OEMCrypto_KeyRefreshObject* key_array) {

    unsigned int i;
    bool status = true;
    Vector key_id;
    Vector key_control;
    Vector key_control_iv;

    SessionContext* session_ctx;
    OEMCryptoResult result =  OEMCrypto_SUCCESS;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    VecInit(&key_id);
    VecInit(&key_control);
    VecInit(&key_control_iv);


    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_RefreshKeys(): ERROR_KEYBOX_INVALID]");
        result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;
    }

    session_ctx = CEFindSession(crypto_engine,session);
    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_RefreshKeys(): ERROR_NO_INVALID_SESSION]");
        result = OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }

    if (message == NULL || message_length == 0 ||
        signature == NULL || signature_length == 0 ||
        num_keys == 0) {
        LOGE("[OEMCrypto_RefreshKeys(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;
    }

    // Range check
    for (i = 0; i < num_keys; i++) {
        if (!RangeCheck(message, message_length, key_array[i].key_id,key_array[i].key_id_length, true) ||
            !RangeCheck(message, message_length, key_array[i].key_control, KEY_CONTROL_SIZE, false) ||
            !RangeCheck(message, message_length, key_array[i].key_control_iv, KEY_IV_SIZE, true)) {
            LOGE("[OEMCrypto_RefreshKeys(): Range Check %d]", i);
            result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            goto final;
        }
    }

    // Validate message signature
    if (!SCTXValidateMessage(session_ctx, message, message_length,
                signature, signature_length)) {
        LOGE("[OEMCrypto_RefreshKeys(): signature was invalid]");
        result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto final;

    }

    // Decrypt and refresh keys in key refresh object
    for (i = 0; i < num_keys; i++) {
        if (key_array[i].key_id != NULL) {

            VecAssign(&key_id,key_array[i].key_id, key_array[i].key_id_length);
            VecAssign(&key_control,key_array[i].key_control, KEY_CONTROL_SIZE);

            if (key_array[i].key_control_iv == NULL ) {
                VecTerm(&key_control_iv);
            } else {
                VecAssign(&key_control_iv,key_array[i].key_control_iv, KEY_IV_SIZE);
            }

        } else {
            // key_id could be null if special control key type
            // key_control is not encrypted in this case
            VecTerm(&key_id);
            VecTerm(&key_control_iv);

            VecAssign(&key_control, key_array[i].key_control, KEY_CONTROL_SIZE);
        }

        if (!SCTXRefreshKey(session_ctx,&key_id, &key_control, &key_control_iv)) {
            LOGE("[OEMCrypto_RefreshKeys():  error in key %i]", i);
            status = false;
            break;
        }
    }

    SCTXFlushNonces(session_ctx);

    if (!status) {
        result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        goto final;
    }

    SCTXStartTimer(session_ctx);

final:

    VecTerm(&key_id);
    VecTerm(&key_control);
    VecTerm(&key_control_iv);

    return result;
}


OEMCryptoResult OEMCrypto_SelectKey(const OEMCrypto_SESSION session,
        const uint8_t* key_id,
        size_t key_id_length) {

    SessionContext* session_ctx;
    OEMCryptoResult result =  OEMCrypto_SUCCESS;
    Vector key_id_str;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();
    
    VecInit(&key_id_str);

#ifndef NDEBUG
    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_SelectKey(): ERROR_KEYBOX_INVALID]");
        result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;
    }
#endif

    session_ctx = CEFindSession(crypto_engine,session);
    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_SelectKey(): ERROR_NO_INVALID_SESSION]");
        result = OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }

    VecAssign(&key_id_str ,key_id, key_id_length);

    if (!SCTXSelectContentKey(session_ctx,&key_id_str)) {
        LOGE("[OEMCrypto_SelectKey(): FAIL]");
        result = OEMCrypto_ERROR_NO_CONTENT_KEY;
        goto final;
    }

final:
    VecTerm(&key_id_str);
    return result;
}


OEMCryptoResult OEMCrypto_DecryptCTR(OEMCrypto_SESSION session,
        const uint8_t* data_addr,
        size_t data_length,
        bool is_encrypted,
        const uint8_t* iv,
        size_t block_offset,
        const OEMCrypto_DestBufferDesc* out_buffer,
        uint8_t subsample_flags) {

    BufferType buffer_type = kBufferTypeDirect;
    uint8_t* destination = NULL;
    size_t max_length = 0;
    SessionContext* session_ctx;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (data_addr == NULL || data_length == 0 ||
            iv == NULL || out_buffer == NULL) {
        LOGE("[OEMCrypto_DecryptCTR(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    switch (out_buffer->type) {
        case OEMCrypto_BufferType_Clear:
            buffer_type = kBufferTypeClear;
            destination = out_buffer->buffer.clear.address;
            max_length =  out_buffer->buffer.clear.max_length;
            break;
        case OEMCrypto_BufferType_Secure:
            buffer_type = kBufferTypeSecure;
            destination = ((uint8_t*)out_buffer->buffer.secure.handle
                    + out_buffer->buffer.secure.offset);
            max_length =  out_buffer->buffer.secure.max_length;
            break;
        default:
        case OEMCrypto_BufferType_Direct:
            return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    if (buffer_type != kBufferTypeDirect && max_length < data_length) {
        LOGE("[OEMCrypto_DecryptCTR(): OEMCrypto_ERROR_SHORT_BUFFER]");
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }

#ifndef NDEBUG
    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_DecryptCTR(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }
#endif

    session_ctx = CEFindSession(crypto_engine,session);
    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_DecryptCTR(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }

    return SCTXDecryptCTR(session_ctx, iv, (int)block_offset,
                data_addr, data_length, is_encrypted,
                destination, buffer_type);
}


OEMCryptoResult OEMCrypto_InstallKeybox(const uint8_t* keybox, size_t keyBoxLength) {
    
    uint8_t installKey[FACTORY_KEYBOX_SIZE];
    size_t keySize = FACTORY_KEYBOX_SIZE;
    void *engine_key = NULL;
 
    if(crypto_engine!=NULL){
        engine_key=&crypto_engine->keybox;
    }

    if(keyBoxLength==FACTORY_KEYBOX_SIZE)
    {
        //FACTORY KEYBOX is 180 byte , which contain the pading
        _teeSecureStorageSecretDec(keybox, keyBoxLength, installKey, &keySize);

        if (WKBInstallKeyboxWithoutWrap(engine_key, installKey, DEV_KEYBOX_SIZE)) {
            return OEMCrypto_SUCCESS;
        }
    }
    else if(keyBoxLength == DEV_KEYBOX_SIZE)
    {
        //TRUE KEYBOX is 128 bytes 
        if (WKBInstallKeybox(engine_key, keybox, DEV_KEYBOX_SIZE)) {
            return OEMCrypto_SUCCESS;
        }
    }

    return OEMCrypto_ERROR_WRITE_KEYBOX;
}


OEMCryptoResult OEMCrypto_IsKeyboxValid(void) {

    //if(crypto_engine==NULL){       
    //    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    //}
        
    switch(CEValidateKeybox(crypto_engine)) {
        case NO_ERROR_PASS:    return OEMCrypto_SUCCESS;
        case BAD_CRC:     return OEMCrypto_ERROR_BAD_CRC;
        case BAD_MAGIC:   return OEMCrypto_ERROR_BAD_MAGIC;
        default:
        case OTHER_ERROR: return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
}


OEMCryptoResult OEMCrypto_GetDeviceID(uint8_t* deviceID,
        size_t* idLength) {

    Vector *dev_id_string;
    size_t dev_id_len;
    
    if(crypto_engine==NULL){

        if(OEMCrypto_IsKeyboxValid()!=OEMCrypto_SUCCESS){
            return OEMCrypto_ERROR_KEYBOX_INVALID;
        }
        
        if (*idLength < sizeof(kDefaultKeybox.device_id_)){
            *idLength = sizeof(kDefaultKeybox.device_id_);
            return OEMCrypto_ERROR_SHORT_BUFFER;
        }
        
        memset(deviceID, 0, *idLength);
        *idLength = sizeof(kDefaultKeybox.device_id_);        
        memcpy_s2n_chk(deviceID, kDefaultKeybox.device_id_, sizeof(kDefaultKeybox.device_id_),NULL);

        return OEMCrypto_SUCCESS;        
    }

    dev_id_string = &crypto_engine->keybox.device_id;

    if (VecIsEmpty(dev_id_string)) {
        LOGE("[OEMCrypto_GetDeviceId(): Keybox Invalid]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    dev_id_len = dev_id_string->size;
    if (*idLength < dev_id_len) {
        *idLength = dev_id_len;
        LOGE("[OEMCrypto_GetDeviceId(): ERROR_SHORT_BUFFER]");
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }
    memset(deviceID, 0, *idLength);
    memcpy_s2n_chk(deviceID, dev_id_string->buf, dev_id_len,NULL);
    *idLength = dev_id_len;
    LOGD("[OEMCrypto_GetDeviceId(): success]");
    return OEMCrypto_SUCCESS;
}


OEMCryptoResult OEMCrypto_GetKeyData(uint8_t* keyData,
        size_t* keyDataLength) {

    size_t length;

    if(crypto_engine==NULL){
        
        length = WKBkey_data_length(NULL);
        if (*keyDataLength < length) {
            *keyDataLength = length;
            return OEMCrypto_ERROR_SHORT_BUFFER;
        }
        memset(keyData, 0, *keyDataLength);
        memcpy_s2n_chk(keyData, kDefaultKeybox.data_, length,NULL);
        *keyDataLength = length;        
        return OEMCrypto_SUCCESS;
    }

    length = WKBkey_data_length(&crypto_engine->keybox);

    if (*keyDataLength < length) {
        *keyDataLength = length;
        LOGE("[OEMCrypto_GetKeyData(): ERROR_SHORT_BUFFER]");
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }
    memset(keyData, 0, *keyDataLength);
    memcpy_s2n_chk(keyData, crypto_engine->keybox.key_data, length,NULL);
    *keyDataLength = length;
    LOGD("[OEMCrypto_GetKeyData(): success]");
    return OEMCrypto_SUCCESS;
}


OEMCryptoResult OEMCrypto_GetRandom(uint8_t* randomData, size_t dataLength) {

    if (!randomData) {
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (RAND_GEN(randomData, dataLength)) {
        return OEMCrypto_SUCCESS;
    }
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
}


OEMCryptoResult OEMCrypto_WrapKeybox(const uint8_t* keybox,
        size_t keyBoxLength,
        uint8_t* wrappedKeybox,
        size_t* wrappedKeyBoxLength,
        const uint8_t* transportKey,
        size_t transportKeyLength) {

    if (!keybox || !wrappedKeybox || !wrappedKeyBoxLength
            || (keyBoxLength != *wrappedKeyBoxLength)) {
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }

    return (WKBWrapKeybox(keybox, keyBoxLength,  wrappedKeybox,  wrappedKeyBoxLength) == true)
        ? OEMCrypto_SUCCESS : OEMCrypto_ERROR_UNKNOWN_FAILURE; 
}


OEMCryptoResult OEMCrypto_RewrapDeviceRSAKey(OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        const uint8_t* signature,
        size_t signature_length,
        const uint32_t* nonce,
        const uint8_t* enc_rsa_key,
        size_t enc_rsa_key_length,
        const uint8_t* enc_rsa_key_iv,
        uint8_t* wrapped_rsa_key,
        size_t*  wrapped_rsa_key_length) {

    uint8_t* pkcs8_rsa_key=NULL;

    size_t buffer_size;
    size_t padding;
    size_t rsa_key_length;
    size_t sig_length;
    SessionContext* session_ctx;

    OEMCryptoResult result = OEMCrypto_SUCCESS;

    Vector context;
    WrappedRSAKey* wrapped;
    
    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    VecInit(&context);

    if (wrapped_rsa_key_length == NULL) {
        LOGE("[OEMCrypto_RewrapDeviceRSAKey(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;
    }
    // For the reference implementation, the wrapped key and the encrypted
    // key are the same size -- just encrypted with different keys.
    // We add 32 bytes for a context, 32 for iv, and 32 bytes for a signature.
    // Important: This layout must match OEMCrypto_LoadDeviceRSAKey below.
    buffer_size = enc_rsa_key_length + sizeof(WrappedRSAKey);

    if (wrapped_rsa_key == NULL || *wrapped_rsa_key_length < buffer_size) {
        LOGW("[OEMCrypto_RewrapDeviceRSAKey(): Wrapped Keybox Short Buffer]");
        *wrapped_rsa_key_length = buffer_size;
        result = OEMCrypto_ERROR_SHORT_BUFFER;
        goto final;
    }

    *wrapped_rsa_key_length = buffer_size;  // Tell caller how much space we used.
    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_RewrapDeviceRSAKey(): ERROR_KEYBOX_INVALID]");
        result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;
    }
    session_ctx = CEFindSession(crypto_engine,session);
    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_RewrapDeviceRSAKey(): ERROR_NO_INVALID_SESSION]");
        result = OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }
    if (message == NULL || message_length == 0 || signature == NULL
            || signature_length == 0 || nonce == NULL || enc_rsa_key == NULL) {
        LOGE("[OEMCrypto_RewrapDeviceRSAKey(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;
    }

    // Range check
    if (!RangeCheck(message, message_length, (const uint8_t*)(nonce),
                sizeof(uint32_t), true) ||
            !RangeCheck(message, message_length, enc_rsa_key, enc_rsa_key_length,
                true) ||
            !RangeCheck(message, message_length, enc_rsa_key_iv, KEY_IV_SIZE,
                true)) {
        LOGE("[OEMCrypto_RewrapDeviceRSAKey():  - range check.]");
        result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto final;
    }

    // Validate nonce
    if (!SCTXCheckNonce(session_ctx,*nonce)) {
        result = OEMCrypto_ERROR_INVALID_NONCE;
        goto final;
    }

    SCTXFlushNonces(session_ctx);

    // Decrypt RSA key.
    pkcs8_rsa_key = malloc(enc_rsa_key_length);
    if(pkcs8_rsa_key ==NULL)
    {
        goto final;
    }


    if (!SCTXDecryptRSAKey(session_ctx, enc_rsa_key, enc_rsa_key_length,
                enc_rsa_key_iv, pkcs8_rsa_key)) {
        result = OEMCrypto_ERROR_INVALID_RSA_KEY;
    }
    padding = pkcs8_rsa_key[enc_rsa_key_length - 1];

    if( result == OEMCrypto_SUCCESS) {
        if (padding > 16) {
            LOGE("[RewrapRSAKey(): Encrypted RSA has bad padding: %d]", padding);
            result = OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }
    rsa_key_length = enc_rsa_key_length - padding;
    // verify signature, verify RSA key, and load it.
    if( result == OEMCrypto_SUCCESS) {
        if (!SCTXLoadRSAKey(session_ctx, pkcs8_rsa_key, rsa_key_length,
                    message, message_length,
                    signature, signature_length)) {
            result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            goto final;
            // return OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }

    // Now we generate a wrapped keybox.
    wrapped = (WrappedRSAKey*)wrapped_rsa_key;
    // Pick a random context and IV for generating keys.
    if( result == OEMCrypto_SUCCESS) {
        if (!RAND_GEN(wrapped->context, sizeof(wrapped->context))) {
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
        if (!RAND_GEN(wrapped->iv, sizeof(wrapped->iv))) {
            result =  OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
        
        VecAssign(&context,wrapped->context , sizeof(wrapped->context));        
    }

    // Generate mac and encryption keys for encrypting the signature.
    if( result == OEMCrypto_SUCCESS) {
        if (!SCTXDeriveKeys(session_ctx,
                    &crypto_engine->keybox.device_key.value,
                    &context, &context)) {
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }

    // Encrypt rsa key with eybox.

    if( result == OEMCrypto_SUCCESS) {
        if (!SCTXEncryptRSAKey(session_ctx,pkcs8_rsa_key, enc_rsa_key_length,
                    wrapped->iv, wrapped->enc_rsa_key)) {
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }


    // The wrapped keybox must be signed with the same key we verify with. I'll
    // pick the server key, so I don't have to modify LoadRSAKey.

#ifdef TODO
    //Server mac key may be changed , can not be reload later???
#endif
    if( result == OEMCrypto_SUCCESS) {
        sig_length = sizeof(wrapped->signature);

        if(!HMAC_SHA256( session_ctx->mac_key_server.buf, session_ctx->mac_key_server.size,
                    wrapped->context, buffer_size - sizeof(wrapped->signature) ,
                    wrapped->signature , &sig_length ))
        {
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }

final:    
    if(pkcs8_rsa_key !=NULL)
    {
        free(pkcs8_rsa_key);
    }
    VecInit(&context);
    return result;
}


OEMCryptoResult OEMCrypto_LoadDeviceRSAKey(OEMCrypto_SESSION session,
        const uint8_t* wrapped_rsa_key,
        size_t wrapped_rsa_key_length) {

    const WrappedRSAKey* wrapped
        = (const WrappedRSAKey*)(wrapped_rsa_key);

    OEMCryptoResult result = OEMCrypto_SUCCESS;
    SessionContext* session_ctx;

    uint8_t* pkcs8_rsa_key=NULL;
    size_t enc_rsa_key_length;
    size_t padding;
    size_t rsa_key_length;

    Vector context;
    
    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();
    
    VecInit(&context);

    if (wrapped_rsa_key == NULL) {
        LOGE("[OEMCrypto_LoadDeviceRSAKey(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        result = OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;
    }
        
    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_LoadDeviceRSAKey(): ERROR_KEYBOX_INVALID]");
        result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_LoadDeviceRSAKey(): ERROR_NO_INVALID_SESSION]");
        result = OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }

    VecAssign(&context , wrapped->context , sizeof(wrapped->context));

    // Generate mac and encryption keys for encrypting the signature.
    if (!SCTXDeriveKeys(session_ctx,&crypto_engine->keybox.device_key.value,
                &context, &context)) {
        result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        goto final;
    }
    // Decrypt RSA key.
    pkcs8_rsa_key = malloc( wrapped_rsa_key_length - sizeof(wrapped->signature));

    if(pkcs8_rsa_key ==NULL){
        goto final;
    }

    enc_rsa_key_length = wrapped_rsa_key_length - sizeof(WrappedRSAKey);
    result = OEMCrypto_SUCCESS;

    if (!SCTXDecryptRSAKey(session_ctx,wrapped->enc_rsa_key, enc_rsa_key_length,
                wrapped->iv, pkcs8_rsa_key)) {
        LOGE("[LoadDeviceRSAKey(): OEMCrypto_ERROR_INVALID_RSA_KEY]");
        result = OEMCrypto_ERROR_INVALID_RSA_KEY;
    }

    padding = pkcs8_rsa_key[enc_rsa_key_length - 1];
    if( result == OEMCrypto_SUCCESS) {
        if (padding > 16) {
            LOGE("[LoadDeviceRSAKey(): Encrypted RSA has bad padding: %d]", padding);
            result = OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }
    rsa_key_length = enc_rsa_key_length - padding;
    // verify signature.
    if( result == OEMCrypto_SUCCESS) {
        if (!SCTXLoadRSAKey(session_ctx,pkcs8_rsa_key, rsa_key_length,
                    wrapped->context,
                    wrapped_rsa_key_length - sizeof(wrapped->signature),
                    wrapped->signature,
                    sizeof(wrapped->signature))) {
            LOGE("[LoadDeviceRSAKey(): Load RSA Key Fail]");
            result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
            // result = OEMCrypto_ERROR_INVALID_RSA_KEY;
        }
    }


final:

    free(pkcs8_rsa_key);
    return result;
}

OEMCryptoResult OEMCrypto_GenerateRSASignature(OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        uint8_t* signature,
        size_t* signature_length,
        RSA_Padding_Scheme padding_scheme) {

    SessionContext* session_ctx;
    size_t required_size;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_GenerateRSASignature(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    if (signature_length == 0) {
        LOGE("[OEMCrypto_GenerateRSASignature(): OEMCrypto_ERROR_INVALID_CONTEXT Signature Length 0]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    session_ctx = CEFindSession(crypto_engine,session);
    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_GenerateRSASignature(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }

    required_size = SCTXRSASignatureSize(session_ctx);
    if (*signature_length < required_size) {
        *signature_length = required_size;
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }

    if (message == NULL || message_length == 0 ||
            signature == NULL || signature_length == 0) {
        LOGE("[OEMCrypto_GenerateRSASignature(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    if (SCTXGenerateRSASignature(session_ctx,message,
                message_length, signature, signature_length , padding_scheme)) {
        return OEMCrypto_SUCCESS;
    }

    return OEMCrypto_ERROR_UNKNOWN_FAILURE;;
}


OEMCryptoResult OEMCrypto_DeriveKeysFromSessionKey(
        OEMCrypto_SESSION session,
        const uint8_t* enc_session_key,
        size_t enc_session_key_length,
        const uint8_t* mac_key_context,
        size_t mac_key_context_length,
        const uint8_t* enc_key_context,
        size_t enc_key_context_length) {

    SessionContext* session_ctx;
    Vector ssn_key_str;
    Vector mac_ctx_str;
    Vector enc_ctx_str;

    OEMCryptoResult result = OEMCrypto_SUCCESS;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    VecInit(&ssn_key_str);
    VecInit(&mac_ctx_str);
    VecInit(&enc_ctx_str);

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_GenerateDerivedKeys(): ERROR_KEYBOX_INVALID]");
        result = OEMCrypto_ERROR_KEYBOX_INVALID;
        goto final;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_GenerateDerivedKeys(): ERROR_NO_INVALID_SESSION]");
        result = OEMCrypto_ERROR_INVALID_SESSION;
        goto final;
    }

    if (session_ctx->allowed_schemes != kSign_RSASSA_PSS) {
      LOGE("[OEMCrypto_GenerateDerivedKeys(): x509 key used to derive keys]");
      return OEMCrypto_ERROR_INVALID_RSA_KEY;
    }

    VecAssign(&ssn_key_str,enc_session_key,enc_session_key_length);
    VecAssign(&mac_ctx_str,mac_key_context,mac_key_context_length);
    VecAssign(&enc_ctx_str,enc_key_context,enc_key_context_length);

    // Generate mac and encryption keys for current session context
    if (!SCTXRSADeriveKeys(session_ctx, &ssn_key_str, 
            &mac_ctx_str, &enc_ctx_str)) {
        result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        goto final;
    }

final:
    VecTerm(&ssn_key_str);
    VecTerm(&mac_ctx_str);
    VecTerm(&enc_ctx_str);

    return result;
}


//uint32_t OEMCrypto_APIVersion() {
//    return oec_latest_version;
//}
//
//
//const char* OEMCrypto_SecurityLevel() {
//    return "L3";
//}


OEMCryptoResult OEMCrypto_Generic_Encrypt(OEMCrypto_SESSION session,
        const uint8_t* in_buffer,
        size_t buffer_length,
        const uint8_t* iv,
        OEMCrypto_Algorithm algorithm,
        uint8_t* out_buffer) {

    SessionContext* session_ctx;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_Generic_Enrypt(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_Generic_Enrypt(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }
    if (in_buffer == NULL || buffer_length == 0 ||
            iv == NULL || out_buffer == NULL) {
        LOGE("[OEMCrypto_Generic_Enrypt(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }
    
    return SCTXGeneric_Encrypt(session_ctx,in_buffer, buffer_length, iv, algorithm, out_buffer);
}


OEMCryptoResult OEMCrypto_Generic_Decrypt(OEMCrypto_SESSION session,
        const uint8_t* in_buffer,
        size_t buffer_length,
        const uint8_t* iv,
        OEMCrypto_Algorithm algorithm,
        uint8_t* out_buffer) {

    SessionContext* session_ctx;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_Generic_Decrypt(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_Generic_Decrypt(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }

    if (in_buffer == NULL || buffer_length == 0 ||
            iv == NULL || out_buffer == NULL) {
        LOGE("[OEMCrypto_Generic_Decrypt(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    return SCTXGeneric_Decrypt(session_ctx,in_buffer, buffer_length, iv, algorithm,
                out_buffer); 
}


OEMCryptoResult OEMCrypto_Generic_Sign(OEMCrypto_SESSION session,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        uint8_t* signature,
        size_t* signature_length) {

    SessionContext* session_ctx;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_Generic_Sign(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_Generic_Sign(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }
    if (*signature_length < SHA256_DIGEST_LENGTH) {
        *signature_length = SHA256_DIGEST_LENGTH;
        return OEMCrypto_ERROR_SHORT_BUFFER;
    }
    if (in_buffer == NULL || buffer_length == 0 || signature == NULL) {
        LOGE("[OEMCrypto_Generic_Sign(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }

    return SCTXGeneric_Sign(session_ctx,in_buffer, buffer_length, algorithm,
                signature, signature_length);
}


OEMCryptoResult OEMCrypto_Generic_Verify(OEMCrypto_SESSION session,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        const uint8_t* signature,
        size_t signature_length) {

    SessionContext* session_ctx;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMBCHECK();

    if (NO_ERROR_PASS != CEValidateKeybox(crypto_engine)) {
        LOGE("[OEMCrypto_Generic_Verify(): ERROR_KEYBOX_INVALID]");
        return OEMCrypto_ERROR_KEYBOX_INVALID;
    }

    session_ctx = CEFindSession(crypto_engine,session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_Generic_Verify(): ERROR_NO_INVALID_SESSION]");
        return OEMCrypto_ERROR_INVALID_SESSION;
    }
    if (signature_length != SHA256_DIGEST_LENGTH) {
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (in_buffer == NULL || buffer_length == 0 || signature == NULL) {
        LOGE("[OEMCrypto_Generic_Verify(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return OEMCrypto_ERROR_INVALID_CONTEXT;
    }
   return SCTXGeneric_Verify(session_ctx,in_buffer, buffer_length, algorithm,
                signature, signature_length);
   
}

OEMCryptoResult OEMCrypto_UpdateUsageTable(void) {
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT 
    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMB_WAITING_CHECK();
    return UTUpdateTable(crypto_engine->usage_table_);
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif 
}

OEMCryptoResult OEMCrypto_DeactivateUsageEntry(const uint8_t *pst,
        size_t pst_length) {
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT        
    OEMCryptoResult result;  
    Vector pstv;
    
    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMB_WAITING_CHECK();    

    VecInit(&pstv);
    VecAssign(&pstv, pst, pst_length);

    result = UTDeactivateEntry(crypto_engine->usage_table_ , &pstv);
    VecTerm(&pstv);
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif    
}

OEMCryptoResult OEMCrypto_ReportUsage(OEMCrypto_SESSION session,
        const uint8_t *pst,
        size_t pst_length,
        OEMCrypto_PST_Report *buffer,
        size_t *buffer_length) {
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT
        
    OEMCryptoResult result;                                        
    UsageTableEntry* entry ;
    SessionContext* session_ctx ;
    Vector pstv;

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMB_WAITING_CHECK();    

    VecInit(&pstv);
    VecAssign(&pstv, pst, pst_length);

    session_ctx = CEFindSession(crypto_engine, session);

    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_ReportUsage(): ERROR_INVALID_SESSION]");
        result =  OEMCrypto_ERROR_INVALID_SESSION;
        goto final;        
    }

    entry = UTFindEntry(crypto_engine->usage_table_, &pstv);

    if (!entry) {
        LOGE("[OEMCrypto_ReportUsage(): Usage Table Entry not found]");
        result =  OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;
    }

    result = UTEReportUsage(entry , session_ctx, &pstv, buffer, buffer_length);
    UTUpdateTable(crypto_engine->usage_table_);

final:
    VecTerm(&pstv);
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}


OEMCryptoResult OEMCrypto_DeleteUsageEntry(OEMCrypto_SESSION session,
        const uint8_t* pst,
        size_t pst_length,
        const uint8_t *message,
        size_t message_length,
        const uint8_t *signature,
        size_t signature_length) {
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT
    OEMCryptoResult result = OEMCrypto_ERROR_UNKNOWN_FAILURE;                                        
    Vector pstv;
    SessionContext* session_ctx = CEFindSession(crypto_engine, session);

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMB_WAITING_CHECK();    

    VecInit(&pstv);
    
    if (!session_ctx || !session_ctx->valid) {
        LOGE("[OEMCrypto_DeleteUsageEntry(): ERROR_INVALID_SESSION]");
        result =  OEMCrypto_ERROR_INVALID_SESSION;
        goto final;                
    }
    if (message == NULL || message_length == 0 || signature == NULL ||
            signature_length == 0 || pst == NULL || pst_length == 0) {
        LOGE("[OEMCrypto_DeleteUsageEntry(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        result =  OEMCrypto_ERROR_INVALID_CONTEXT;
        goto final;                
    }
    if (!RangeCheck(message, message_length, pst, pst_length, false)) {
        LOGE("[OEMCrypto_DeleteUsageEntry(): range check.]");
        result =  OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto final;                
    }
    // Validate message signature
    if (!SCTXValidateMessage(session_ctx , message, message_length, signature,
                signature_length)) {
        LOGE("[OEMCrypto_DeleteUsageEntry(): OEMCrypto_ERROR_SIGNATURE_FAILURE.]");
        result =  OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto final;                
    }
    
    VecAssign(&pstv, pst, pst_length);
    
    if (UTDeleteEntry(crypto_engine->usage_table_ , &pstv , true)) {
        result =  OEMCrypto_SUCCESS;
        goto final;                
    }

final:
    VecTerm(&pstv);
    
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}

OEMCryptoResult OEMCrypto_DeleteUsageTable() {
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT

    if(crypto_engine==NULL){
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    RPMB_WAITING_CHECK();    

    if (UTClear(crypto_engine->usage_table_)) {
        return OEMCrypto_SUCCESS;
    }
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}

OEMCryptoResult OEMCrypto_GetUsageTableWriteRequest(
        uint8_t* buffer,
        size_t * buffer_length,
        struct RPMB_DATAFRAM *dFrame) {
#if defined(CC_WIDEVINE_SUPPORT_USAGE_REPORT)&&!defined(NO_PERSIST_USAGE_TABLE)

    OEMCryptoResult result = OEMCrypto_ERROR_UNKNOWN_FAILURE;   
    int64_t generation;    
    unsigned char dFrmData[RPMB_DATA_SIZE];
    size_t fileSize;

    if(!UTWaitingEcho()){
        //No Need to Update Flash.
        return OEMCrypto_ERROR_NOT_IMPLEMENTED;
    }
    
    RPMBCHECK();    
    if(writeReqWait == true) {
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    fileSize = UTGetFakeFileData(NULL);
    if(*buffer_length < fileSize){
        goto final;       
    }

    UTGetFakeFileData(buffer);

    generation = UTLoadGenerationFromFakeFile();
    memset(dFrmData, 0 , sizeof(dFrmData));
    
    memcpy(dFrmData, &generation, sizeof(generation));
        
    *dFrame = rpmbGetWriteReq(dFrmData , RPMB_WV_GENERATION, 1);
    result = OEMCrypto_SUCCESS; 
    writeReqWait = true;
    
final:    
    *buffer_length = fileSize;
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}

OEMCryptoResult OEMCrypto_VerifyWriteResponse(const struct RPMB_DATAFRAM *dFrame){
    
#if defined(CC_WIDEVINE_SUPPORT_USAGE_REPORT)&&!defined(NO_PERSIST_USAGE_TABLE)

    OEMCryptoResult result = OEMCrypto_ERROR_UNKNOWN_FAILURE;  

    RPMBCHECK();
    if(writeReqWait == false) 
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    
    if(rpmbSetWriteResp(*dFrame))
    {
        UTFileIsWritten();
        result = OEMCrypto_SUCCESS; 
    }
    else
    {
        rpmbfailed=true;
    }

    writeReqWait=false;
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}

OEMCryptoResult OEMCrypto_GetUsageTableReadRequest(struct RPMB_DATAFRAM *dFrame){
#if defined(CC_WIDEVINE_SUPPORT_USAGE_REPORT)&&!defined(NO_PERSIST_USAGE_TABLE)

    OEMCryptoResult result = OEMCrypto_ERROR_UNKNOWN_FAILURE;   
    RPMB_DATAFRAM req;

    RPMBCHECK();    
    if(dFrame !=NULL){
        req = rpmbGetReadeReq(RPMB_WV_GENERATION);
        memcpy(dFrame , &req, sizeof(RPMB_DATAFRAM));
        result = OEMCrypto_SUCCESS;         
    }
    
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}

OEMCryptoResult OEMCrypto_VerifyReadResponse(
        const uint8_t* buffer,
        size_t buffer_length,
        const struct RPMB_DATAFRAM *dFrame){
    
#if defined(CC_WIDEVINE_SUPPORT_USAGE_REPORT)&&!defined(NO_PERSIST_USAGE_TABLE)

    unsigned short addr;
    unsigned char data[RPMB_DATA_SIZE];
    OEMCryptoResult result = OEMCrypto_ERROR_UNKNOWN_FAILURE; 
    int64_t gen;
    int64_t epoch;
    static char init = false;

    RPMBCHECK();
    if(init==true)
        return OEMCrypto_SUCCESS;

    if(rpmbSetReadResp(*dFrame,&addr,data))
    {   
        //Init System Time
        memcpy(&epoch , buffer , sizeof(int64_t));    
        UTSetFileEpoch(epoch);
        //Usage Repout
        memcpy(&gen , data , sizeof(int64_t));    
        UTSetGenerationToFakeFile(gen);
        UTSetFakeFileData(buffer +sizeof(int64_t) ,buffer_length-sizeof(int64_t) );
        result = OEMCrypto_SUCCESS; 
    }
    else
    {
        rpmbfailed=true;
    }

    init = true;    
    return result;
#else
    return OEMCrypto_ERROR_NOT_IMPLEMENTED;
#endif
}

