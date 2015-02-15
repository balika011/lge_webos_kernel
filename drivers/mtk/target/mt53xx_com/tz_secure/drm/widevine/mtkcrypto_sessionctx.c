#include "x_timer.h"
#include "log.h"

#include "string_conversions.h"

#include "lock.h"
#include "mtkcrypto_sessionctx.h"
#include "mtkcrypto_key.h"
#include "mtkcrypto_keybox.h"
#include "mtkcrypto_engine.h"

#include "wv_cdm_constants.h"
#include "wv_cdm_types.h"

#include "mtkcrypto_usage_table.h"
#include "sec_buf.h"

//typedef struct CryptoEngine CryptoEngine;

SessionContext *SCTXCreate(CryptoEngine *ce, SessionId sid)
{
    SessionContext *ret = malloc(sizeof(SessionContext));
    if(ret ==NULL)
        goto final;

    ret->valid = true;
    ret->id = sid;
    ret->current_content_key = NULL;
    ret->rsa_key = NULL;
    ret->ce = ce;
    ret->allowed_schemes = kSign_RSASSA_PSS;

    VecInit(&ret->mac_key_server);
    VecInit(&ret->mac_key_client);
    VecInit(&ret->encryption_key);
    VecInit(&ret->session_key);
    SKTInit(&ret->session_keys);
    NTInit(&ret->nonce_table);

    ret->usage_entry_ = NULL;
final:
    return ret;
}

static void SCTXTerm(SessionContext *this) {

    this->ce = NULL;

    //TODO Destroy RSA KEY
    VecTerm(&this->mac_key_server);
    VecTerm(&this->mac_key_client);
    VecTerm(&this->encryption_key);
    VecTerm(&this->session_key);
    SKTTerm(&this->session_keys);
    NTTerm(&this->nonce_table);

    RSA_KEY_TERM(&this->rsa_key);

    if (this->usage_entry_) 
        UTEset_session(this->usage_entry_, NULL);    
}

void SCTXDestroy(SessionContext *this)
{
    SCTXTerm(this);
    free(this);
}

// Internal utility function to derive key using CMAC-128
static bool SCTXDeriveKeyWithCnt(SessionContext *this,
        const Vector *key,
        const Vector *context,
        int counter,
        Vector* out) {

    size_t reslen=128;
    uint8_t res[128];
    Vector message;
    bool ret = true;

    VecInit(&message);

    if (VecIsEmpty(key) || counter > 4 ||VecIsEmpty( context) || out == NULL) {
        LOGE("[DeriveKey(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        ret = false;
        goto final;
    }

    VecPushBack(&message,counter);
    VecAppend(&message,context);

    if(CMAC_AES( key->buf,key->size, message.buf, message.size, res, &reslen )==false){
        LOGE("[DeriveKey(): OEMCrypto_ERROR_CMAC_FAILURE]");
        ret =  false;
        goto final;
    }

    VecAssign(out,res,reslen);
final :

    VecTerm(&message);
    return ret;
}

bool SCTXDeriveKeys(SessionContext *this,
        const Vector* master_key,
        const Vector* mac_key_context,
        const Vector* enc_key_context) {

    // Generate derived key for mac key
    Vector mac_key_server;
    Vector mac_key_client;
    Vector mac_key_part2;
    Vector enc_key;

    //Vector master_print;

    bool ret = true;

    VecInit(&mac_key_server);
    VecInit(&mac_key_client);
    VecInit(&mac_key_part2);
    VecInit(&enc_key);

    //VecInit(&master_print);

    if (!SCTXDeriveKeyWithCnt(this, master_key, mac_key_context, 1, &mac_key_server)) {
        ret =false;
        goto final;
    }
    if (!SCTXDeriveKeyWithCnt(this, master_key, mac_key_context, 2, &mac_key_part2)) {
        ret =false;
        goto final;
    }

    VecAppend(&mac_key_server,&mac_key_part2);

    if (!SCTXDeriveKeyWithCnt(this,
                master_key, mac_key_context, 3, &mac_key_client)) {
        ret =false;
        goto final;
    }
    if (!SCTXDeriveKeyWithCnt(this,
                master_key, mac_key_context, 4, &mac_key_part2)) {
        ret =false;
        goto final;
    }

    VecAppend(&mac_key_client,&mac_key_part2);

    // Generate derived key for encryption key
    if (!SCTXDeriveKeyWithCnt(this, master_key, enc_key_context, 1, &enc_key)) {
        ret =false;
        goto final;
    }

    SCTXset_mac_key_server(this,&mac_key_server);
    SCTXset_mac_key_client(this,&mac_key_client);
    SCTXset_encryption_key(this,&enc_key);

    //VecCpy(&master_print , master_key);
    //VecToCStr(&master_print);
    //LOGE("master_print %s",master_print.buf);
    //VecToCStr(&mac_key_server);
    //LOGE("mac_key_server %s",mac_key_server.buf);
    //VecToCStr(&mac_key_client);
    //LOGE("mac_key_client %s",mac_key_client.buf);
    //VecToCStr(&enc_key);
    //LOGE("enc_key %s",enc_key.buf);

final:    
    //VecTerm(&master_print);

    VecTerm(&mac_key_server);
    VecTerm(&mac_key_client);
    VecTerm(&mac_key_part2);
    VecTerm(&enc_key);

    return ret;
}

bool SCTXRSADeriveKeys(SessionContext *this,
        const Vector* enc_session_key,
        const Vector* mac_key_context,
        const Vector* enc_key_context) {

    size_t decrypted_size=0;

    if (!this->rsa_key) {
        LOGE("[RSADeriveKeys(): no RSA key set]");
        return false;
    }
    if (enc_session_key->size != (size_t)(RSA_GET_SIZE(this->rsa_key))) {
        LOGE("[RSADeriveKeys(): encrypted session key is wrong size:%zu, should be %d]",
                enc_session_key->size, RSA_GET_SIZE(this->rsa_key));
        return false;
    }

    VecResize(&this->session_key,RSA_GET_SIZE(this->rsa_key));
    decrypted_size = RSA_GET_SIZE(this->rsa_key);

    if(!RSA_DECRYPT(this->rsa_key,
                enc_session_key->buf , enc_session_key->size, 
                this->session_key.buf, &decrypted_size)){
        LOGE("[RSADeriveKeys(): RSA Decryption Fail ");
        VecTerm(&this->session_key);
        return false;
    }

    if (decrypted_size != KEY_SIZE) {
        LOGE("[RSADeriveKeys(): error.  session key is wrong size: %d.]", decrypted_size);
        VecTerm(&this->session_key);
        return false;
    }

    VecResize(&this->session_key , decrypted_size);

    return SCTXDeriveKeys(this,
            &this->session_key, mac_key_context, enc_key_context);
}

// Utility function to generate a message signature
bool SCTXGenerateSignature(SessionContext *this,
        const uint8_t* message,
        size_t message_length,
        uint8_t* signature,
        size_t* signature_length) {

    unsigned int md_len ;

    if (message == NULL || message_length == 0 ||
            signature == NULL || signature_length == 0) {
        LOGE("[OEMCrypto_GenerateSignature(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return false;
    }

    if (VecIsEmpty(&this->mac_key_client)
            || this->mac_key_client.size != MAC_KEY_SIZE) {
        LOGE("[GenerateSignature(): No MAC Key]");
        return false;
    }

    if (*signature_length < SHA256_DIGEST_LENGTH) {
        *signature_length = SHA256_DIGEST_LENGTH;
        return false;
    }

    if(HMAC_SHA256(this->mac_key_client.buf, this->mac_key_client.size,
                message , message_length,
                signature, &md_len)==true)
    {
        *signature_length = md_len;
        return true;
    }

    return false;
}

size_t SCTXRSASignatureSize(SessionContext *this) {
    if (!this->rsa_key) {
        LOGE("[GenerateRSASignature(): no RSA key set]");
        return 0;
    }
    return RSA_GET_SIZE(this->rsa_key);
}

bool SCTXGenerateRSASignature(SessionContext *this,
        const uint8_t* message,
        const size_t message_length,
        uint8_t* signature,
        size_t* signature_length,
        RSA_Padding_Scheme padding_scheme){

    if ((padding_scheme & this->allowed_schemes) != padding_scheme) {
        return false;
    }
    if (message == NULL || message_length == 0 ||
            signature == NULL || signature_length == 0) {
        LOGE("[GenerateRSASignature(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return false;
    }
    if (!this->rsa_key) {
        LOGE("[GenerateRSASignature(): No RSA key set]");
        return false;
    }
    if (*signature_length < (size_t)(RSA_GET_SIZE(this->rsa_key))) {
        *signature_length = RSA_GET_SIZE(this->rsa_key);
        LOGE("[GenerateRSASignature(): Signature Size Invalid ]");
        return false;
    }

    if(padding_scheme == kSign_RSASSA_PSS){
        return RSA_SIGN(this->rsa_key, message, message_length, signature , signature_length , 0);
    }
    else
    {
        if (message_length > 83) {
            return false;
        }   
        return RSA_SIGN(this->rsa_key, message, message_length, signature , signature_length , 1);
    }
}

// Validate message signature
bool SCTXValidateMessage(SessionContext *this,
        const uint8_t* given_message,
        size_t message_length,
        const uint8_t* given_signature,
        size_t signature_length) {

    uint8_t computed_signature[SHA256_DIGEST_LENGTH];
    unsigned int md_len = SHA256_DIGEST_LENGTH;

    if (signature_length != SHA256_DIGEST_LENGTH) {
        return false;
    }

    if(VecIsEmpty(&this->mac_key_server) || this->mac_key_server.size != SHA256_DIGEST_LENGTH)
    {
        LOGE("[GenerateSignature(): No MAC Key]");
        return false;
    }

    if (!HMAC_SHA256(this->mac_key_server.buf, this->mac_key_server.size,
                given_message, message_length, computed_signature, &md_len)) {
        LOGE("ValidateMessage: Could not compute signature.");
        return false;
    }
    if (memcmp(given_signature, computed_signature, signature_length)) {

        LOGE("Invalid signature    given: xxxx");
        LOGE("Invalid signature computed: xxxx");

        //LOGE("Invalid signature    given: %s",
        //        HexEncode(given_signature, signature_length).c_str());
        //LOGE("Invalid signature computed: %s",
        //        HexEncode(computed_signature, signature_length).c_str());
        return false;
    }
    return true;
}

bool SCTXCheckNonceOrEntry(SessionContext *this , const KeyControlBlock *key_control_block, const Vector *pst) {
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    switch (key_control_block->control_bits & kControlReplayMask) {
        case kControlNonceRequired:  // Online license. Nonce always required.
            if (!SCTXCheckNonce(this,key_control_block->nonce)) 
                return false;
            if (pst->size == 0) {        
                LOGE("KCB: PST null for kControlNonceRequired.");        
                return false;      
            }      
            if (!this->usage_entry_) {
                if(UTFindEntry(this->ce->usage_table_ , pst)) {
                    LOGE("KCB: Cannot create duplicate entries in usage table.");
                    return false;
                }
                this->usage_entry_ = UTCreateEntry(this->ce->usage_table_ , pst, this);
            }
            break;  // Offline license. Nonce required on first use.
        case kControlNonceOrEntry:
            if (pst->size == 0) {        
                LOGE("KCB: PST null for kControlNonceRequired.");        
                return false;      
            }               
            if (!this->usage_entry_) {
                this->usage_entry_ = UTFindEntry(this->ce->usage_table_ , pst);
                
                if (this->usage_entry_) {
                    if (UTEstatus(this->usage_entry_) == kInactive) 
                        return false;
                } else {
                    if (!SCTXCheckNonce(this, key_control_block->nonce)) 
                        return false;
                    this->usage_entry_ = UTCreateEntry(this->ce->usage_table_ , pst, this);
                }
            } else {
                if (UTEstatus(this->usage_entry_) == kInactive) 
                    return false;
            }
            break;  // Usage table not required.  Look at nonce enabled bit.
        default:
            if ((key_control_block->control_bits& kControlNonceEnabled) &&
                    (!SCTXCheckNonce(this, key_control_block->nonce))) {
                LOGE("KCB: BAD Nonce");
                return false;
            }
    }
    return true;
#else
    if ((key_control_block->control_bits& kControlNonceEnabled) &&
            (!SCTXCheckNonce(this, key_control_block->nonce))) {
        LOGE("KCB: BAD Nonce");
        return false;
    }
#endif
}



bool SCTXParseKeyControl(SessionContext *this,
        const Vector* key_control_string,
        KeyControlBlock* key_control_block) {

    //const char* cgms_values[4] = {"free", "BAD", "once", "never"};

    KCBInvalidate(key_control_block);

    if (key_control_string->size < KEY_CONTROL_SIZE) {
        LOGD("ParseKeyControl: wrong size.");
        return false;
    }
    if(!KCBSetFromString(key_control_block, key_control_string)){
        LOGE("KCB: BAD Size or Structure");
        return false;
    }


    LOGD("KCB:");
    LOGD("  valid:    %d", key_control_block->valid);
    LOGD("  duration: %d", key_control_block->duration);
    LOGD("  nonce:    %08X", key_control_block->nonce);
    LOGD("  bits:     %08X", key_control_block->control_bits);

    LOGD("  bit kControlAllowEncrypt %s.",
            (key_control_block->control_bits & kControlAllowEncrypt) ? "set" : "unset");
    LOGD("  bit kControlAllowDecrypt %s.",
            (key_control_block->control_bits & kControlAllowDecrypt) ? "set" : "unset");
    LOGD("  bit kControlAllowSign %s.",
            (key_control_block->control_bits & kControlAllowSign) ? "set" : "unset");
    LOGD("  bit kControlAllowVerify %s.",
            (key_control_block->control_bits & kControlAllowVerify) ? "set" : "unset");
    LOGD("  bit kControlObserveDataPath %s.",
            (key_control_block->control_bits & kControlObserveDataPath) ? "set" : "unset");
    LOGD("  bit kControlObserveHDCP %s.",
            (key_control_block->control_bits & kControlObserveHDCP) ? "set" : "unset");
    LOGD("  bit kControlObserveCGMS %s.",
            (key_control_block->control_bits & kControlObserveCGMS) ? "set" : "unset");
    LOGD("  bit kControlDataPathSecure %s.",
            (key_control_block->control_bits & kControlDataPathSecure) ? "set" : "unset");
    LOGD("  bit kControlNonceEnabled %s.",
            (key_control_block->control_bits & kControlNonceEnabled) ? "set" : "unset");
    LOGD("  bit kControlHDCPRequired %s.",
            (key_control_block->control_bits & kControlHDCPRequired) ? "set" : "unset");

    LOGD("    CGMS = %s", cgms_values[key_control_block->control_bits & 0x3 ]);

    if (!KCBValidate(key_control_block)) {
        LOGE("KCB: BAD Signature");
        return false;
    }
    
    return true;
}

void SCTXStartTimer(SessionContext *this) {
	HAL_TIME_T time;
	HAL_GetTime(&time);
    //this->timer_start = time(NULL);
    this->timer_start = time.u4Seconds;
}

uint32_t SCTXCurrentTimer(SessionContext *this) {
	HAL_TIME_T time;
	HAL_GetTime(&time);
    uint32_t cur = time.u4Seconds;

    if(this ==NULL){
        return cur;
    }
    
    return cur - this->timer_start;
}

OEMCryptoResult SCTXLoadKeys(SessionContext *this,
        const uint8_t* message, size_t message_length, 
        const uint8_t* signature, size_t signature_length, 
        const uint8_t* enc_mac_key_iv,
        const uint8_t* enc_mac_keys, size_t num_keys,
        const OEMCrypto_KeyObject* key_array, 
        const uint8_t* pst,size_t pst_length) {

    bool status = true;

    unsigned int i; 

    Vector key_id;
    Vector enc_key_data;
    Vector key_data_iv;
    Vector key_control;
    Vector key_control_iv;
    Vector enc_mac_keys_str;
    Vector enc_mac_key_iv_str;    
    Vector pstv;

    OEMCryptoResult result= OEMCrypto_SUCCESS;

    VecInit(&key_id);
    VecInit(&enc_key_data);
    VecInit(&key_data_iv);
    VecInit(&key_control);
    VecInit(&key_control_iv);
    VecInit(&enc_mac_keys_str);
    VecInit(&enc_mac_key_iv_str);
    VecInit(&pstv);

    VecAssign(&pstv,pst,pst_length);

    // Validate message signature
    if (!SCTXValidateMessage(this, message, message_length, signature, signature_length)) {
        result = OEMCrypto_ERROR_SIGNATURE_FAILURE;
        goto final;
    }

    SCTXStartTimer(this);

    // Decrypt and install keys in key object
    // Each key will have a key control block.  They will all have the same nonce.
    for (i = 0; i < num_keys; i++) {

        if (key_array[i].key_control == NULL) {
            status = false;
            break;
        }

        VecAssign(&key_id,
                key_array[i].key_id, key_array[i].key_id_length);
        VecAssign(&enc_key_data,
                key_array[i].key_data, key_array[i].key_data_length);
        VecAssign(&key_data_iv, 
                key_array[i].key_data_iv, KEY_IV_SIZE);

        if (key_array[i].key_control == NULL) {
            status = false;
            break;
        }

        VecAssign(&key_control,
                key_array[i].key_control, KEY_CONTROL_SIZE);
        VecAssign(&key_control_iv,
                key_array[i].key_control_iv, KEY_IV_SIZE);

        if (!SCTXInstallKey(this,
                    &key_id, &enc_key_data, &key_data_iv,
                    &key_control, &key_control_iv , &pstv)) {
            status = false;
            break;
        }
    } 

    SCTXFlushNonces(this);

    if (!status) {
        result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
        goto final;
    }

    // enc_mac_key can be NULL if license renewal is not supported
    if (enc_mac_keys != NULL) {

        // V2.1 license protocol: update mac keys after processing license response
        VecAssign(&enc_mac_keys_str, enc_mac_keys , MAC_KEY_SIZE*2);
        VecAssign(&enc_mac_key_iv_str, enc_mac_key_iv ,KEY_IV_SIZE );

        if (!SCTXUpdateMacKeys(this, &enc_mac_keys_str,&enc_mac_key_iv_str)) {
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
            goto final;
        }
    }

#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    if (this->usage_entry_) {
        if (!UTEVerifyOrSetMacKeys(this->usage_entry_, &this->mac_key_server, &this->mac_key_client)) {
            result = OEMCrypto_ERROR_UNKNOWN_FAILURE;
            goto final;            
        }
    }
#endif    
    
final:

    VecTerm(&key_id);
    VecTerm(&enc_key_data);
    VecTerm(&key_data_iv);
    VecTerm(&key_control);
    VecTerm(&key_control_iv);
    VecTerm(&enc_mac_keys_str);
    VecTerm(&enc_mac_key_iv_str);
    VecTerm(&pstv);

    return result;
}

bool SCTXInstallKey(SessionContext *this,
        const KeyId *key_id,
        const Vector *key_data,
        const Vector *key_data_iv,
        const Vector *key_control,
        const Vector *key_control_iv,
        const Vector *pst) {

    Vector content_key;
    Vector key_control_str;
    KeyControlBlock key_control_block;
    Key key;

    bool ret = true;

    VecInit(&content_key);
    VecInit(&key_control_str);
    KCBInit(&key_control_block);
    KeyInit(&key);

    // Decrypt encrypted key_data using derived encryption key and offered iv

    if (!CEDecryptMessage(this->ce ,
                this, &this->encryption_key,
                key_data_iv, key_data, &content_key)) {
        LOGE("[Installkey(): Could not decrypt key data]");
        ret = false;
        goto final;
    }
    // Key control must be supplied by license server
    if (VecIsEmpty(key_control)) {
        LOGE("[Installkey(): WARNING: No Key Control]");
        KCBInvalidate(&key_control_block);
        ret = false;
        goto final;

    } 
    
    if (VecIsEmpty(key_control_iv)) {
        LOGE("[Installkey(): ERROR: No Key Control IV]");
        ret = false;
        goto final;
    }
    if (!CEDecryptMessage(this->ce,this, &content_key, key_control_iv,
                key_control, &key_control_str)) {
        LOGE("[Installkey(): ERROR: Could not key control ]");
        ret = false;
        goto final;
    }
    if (!SCTXParseKeyControl(this, &key_control_str, &key_control_block)) {
        LOGE("Error parsing key control.");
        ret = false;
        goto final;
    }
    
    if(!SCTXCheckNonceOrEntry(this ,&key_control_block, pst)){
        LOGE("Failed Nonce/PST check.");
        ret = false;
        goto final;        
    }

    KeysetType(&key, KEYTYPE_CONTENT);
    KeysetControl(&key , &key_control_block);
    KeysetValue(&key , content_key.buf , content_key.size);

    SKTInsert(&this->session_keys, key_id, &key);

final:

    VecTerm(&content_key);
    VecTerm(&key_control_str);
    KCBTerm(&key_control_block);
    KeyTerm(&key);

    return ret;
}

bool  SCTXDecryptRSAKey(SessionContext *this,
        const uint8_t* enc_rsa_key,
        size_t enc_rsa_key_length,
        const uint8_t* enc_rsa_key_iv,
        uint8_t* pkcs8_rsa_key)  {
    // Decrypt rsa key with keybox.
    return AES_CBC_DECRYPT(this->encryption_key.buf,this->encryption_key.size,
            enc_rsa_key_iv, enc_rsa_key, pkcs8_rsa_key, enc_rsa_key_length);
}

bool SCTXEncryptRSAKey(SessionContext *this,
        const uint8_t* pkcs8_rsa_key,
        size_t enc_rsa_key_length,
        const uint8_t* enc_rsa_key_iv,
        uint8_t* enc_rsa_key) {
    // Encrypt rsa key with keybox.
    return AES_CBC_ENCRYPT(this->encryption_key.buf,this->encryption_key.size,
            enc_rsa_key_iv,pkcs8_rsa_key,enc_rsa_key, enc_rsa_key_length);
}

bool SCTXLoadRSAKey(SessionContext *this,
        uint8_t* pkcs8_rsa_key,
        size_t rsa_keylength,
        const uint8_t* message,
        size_t message_length,
        const uint8_t* signature,
        size_t signature_length) {

    uint32_t *schemes_n;

    // Validate message signature
    if (!SCTXValidateMessage(this,message, message_length, signature, signature_length)) {
        LOGE("[LoadRSAKey(): Could not verify signature]");
        return false;
    }

    if (rsa_keylength < 8) {
        return false;
    }

    if( (memcmp(pkcs8_rsa_key, "SIGN", 4) == 0) ){
        schemes_n = (uint32_t *)(pkcs8_rsa_key + 4);
        this->allowed_schemes = htonl(*schemes_n);
        pkcs8_rsa_key += 8;
        rsa_keylength -= 8;
    }
    
    return RSA_LOAD_PKCS8_KEY(&this->rsa_key,  pkcs8_rsa_key, rsa_keylength);
}

OEMCryptoResult SCTXDecryptCTR(SessionContext* this,
        const uint8_t* iv,
        size_t block_offset,
        const uint8_t* cipher_data,
        size_t cipher_data_length,
        bool is_encrypted,
        void* clear_data,
        BufferType buffer_type) {

    const KeyControlBlock *control;
    const Vector* content_key;


    if (buffer_type == kBufferTypeSecure) {
        if(sec_buf_verify(clear_data , cipher_data_length) == false)
            return OEMCrypto_ERROR_DECRYPT_FAILED;        
    }

    // If the data is clear, we do not need a current key selected.
    if (!is_encrypted) {
        memcpy((uint8_t*)(clear_data), cipher_data, cipher_data_length);
        return OEMCrypto_SUCCESS;
    }

    // Check there is a content key
    if (this->current_content_key == NULL) {
        LOGE("[DecryptCTR(): OEMCrypto_ERROR_NO_CONTENT_KEY]");
        return OEMCrypto_ERROR_DECRYPT_FAILED;
    }
    control = &this->current_content_key->control;
    if (control->control_bits & kControlDataPathSecure) {
        if (buffer_type == kBufferTypeClear) {
            LOGE("[DecryptCTR(): Secure key with insecure buffer]");
            return OEMCrypto_ERROR_DECRYPT_FAILED;
        }
    }

    //if (control->control_bits & kControlHDCPRequired) {
    //    // For reference implementation, we do not implement any HDCP.
    //    LOGD("DecryptCTR()Decrypt Fail :For reference implementation, we do not implement any HDCP.");
    //    return false;
    //}

    if (control->duration > 0) {
        if (control->duration < SCTXCurrentTimer(this)) {
            LOGD("DecryptCTR() Fail : Ctrl Duration too small %d %d" , control->duration,
                    SCTXCurrentTimer(this));
            return OEMCrypto_ERROR_KEY_EXPIRED;
        }
    }

#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    if (control->control_bits & kControlReplayMask) {
        if (!SCTXIsUsageEntryValid(this)) {
            LOGE("[Generic_Encrypt(): usage entry not valid]");
            return OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
#endif  

    content_key = &this->current_content_key->value;

    // Set the AES key.
    if ((int)(content_key->size) != AES_BLOCK_SIZE) {
        LOGE("[DecryptCTR(): CONTENT_KEY has wrong size: %d", content_key->size);
        return OEMCrypto_ERROR_DECRYPT_FAILED;
    }

    if (buffer_type == kBufferTypeDirect) {
        // For reference implementation, we quietly drop direct video.
        return OEMCrypto_SUCCESS;
    }

    // Decryption.
    return  (AES_CTR_DECRYPT(content_key->buf, content_key->size, iv, 
            cipher_data, clear_data, cipher_data_length, block_offset))?
            OEMCrypto_SUCCESS : OEMCrypto_ERROR_DECRYPT_FAILED;
}

OEMCryptoResult SCTXGeneric_Encrypt(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        const uint8_t* iv,
        OEMCrypto_Algorithm algorithm,
        uint8_t* out_buffer) {

    const Vector *key = &this->current_content_key->value;
    const KeyControlBlock * control = &this->current_content_key->control;
    size_t len = buffer_length;

    // Check there is a content key
    if (this->current_content_key == NULL) {
        LOGE("[Generic_Encrypt(): OEMCrypto_ERROR_NO_CONTENT_KEY]");
        return OEMCrypto_ERROR_NO_CONTENT_KEY;
    }

    // Set the AES key.
    if ((int)(key->size) != AES_BLOCK_SIZE) {
        LOGE("[Generic_Encrypt(): CONTENT_KEY has wrong size: %d",key->size);
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (!(control->control_bits & kControlAllowEncrypt)) {
        LOGE("[Generic_Encrypt(): control bit says not allowed.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (control->duration > 0) {
        if (control->duration < SCTXCurrentTimer(this)) {
            LOGE("[Generic_Encrypt(): key expired.");
            return OEMCrypto_ERROR_KEY_EXPIRED;
        }
    }

#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    if (control->control_bits & kControlReplayMask) {
        if (!SCTXIsUsageEntryValid(this)) {
            LOGE("[Generic_Encrypt(): usage entry not valid]");
            return OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
#endif    
    
    if( algorithm != OEMCrypto_AES_CBC_128_NO_PADDING ) {
        LOGE("[Generic_Encrypt(): algorithm bad.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if( buffer_length % AES_BLOCK_SIZE != 0 ) {
        LOGE("[Generic_Encrypt(): buffers size bad.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    //Key Size Should be fix according to the test suite
    //if(!AES_CBC_ENCRYPT(key->buf, key->size, iv, in_buffer, out_buffer,len))
    if(!AES_CBC_ENCRYPT(key->buf, AES_BLOCK_SIZE , iv, in_buffer, out_buffer,len))
    {
        LOGE("[Generic_Encrypt(): FAILURE]");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    return OEMCrypto_SUCCESS;
}

OEMCryptoResult SCTXGeneric_Decrypt(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        const uint8_t* iv,
        OEMCrypto_Algorithm algorithm,
        uint8_t* out_buffer) {

    const Vector *key = &this->current_content_key->value;
    const KeyControlBlock *control = &this->current_content_key->control;
    size_t len = buffer_length;

    // Check there is a content key
    if (this->current_content_key == NULL) {
        LOGE("[Generic_Decrypt(): OEMCrypto_ERROR_NO_CONTENT_KEY]");
        return OEMCrypto_ERROR_NO_CONTENT_KEY;
    }
    // Set the AES key.
    if ((int)(key->size) != AES_BLOCK_SIZE) {
        LOGE("[Generic_Decrypt(): CONTENT_KEY has wrong size.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (!(control->control_bits & kControlAllowDecrypt)) {
        LOGE("[Generic_Decrypt(): control bit says not allowed.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (control->control_bits & kControlDataPathSecure) {
        LOGE("[Generic_Decrypt(): control bit says secure path only.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (control->duration > 0) {
        if (control->duration < SCTXCurrentTimer(this)) {
            LOGE("[Generic_Decrypt(): key expired.");
            return OEMCrypto_ERROR_KEY_EXPIRED;
        }
    }
    
#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    if (control->control_bits & kControlReplayMask) {
        if (!SCTXIsUsageEntryValid(this)) {
            LOGE("[Generic_Encrypt(): usage entry not valid]");
            return OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
#endif  

    
    if( algorithm != OEMCrypto_AES_CBC_128_NO_PADDING ) {
        LOGE("[Generic_Decrypt(): bad algorithm.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if( buffer_length % AES_BLOCK_SIZE != 0 ) {
        LOGE("[Generic_Decrypt(): bad buffer size.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    //Key Size Should be fix according to the test suite
    //if(!AES_CBC_DECRYPT(key->buf, key->size, iv, in_buffer, out_buffer,len))
    if(!AES_CBC_DECRYPT(key->buf, AES_BLOCK_SIZE , iv, in_buffer, out_buffer,len))
    {
        LOGE("[Generic_Decrypt(): FAILURE]");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    return OEMCrypto_SUCCESS;
}

OEMCryptoResult SCTXGeneric_Sign(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        uint8_t* signature,
        size_t* signature_length) {

    unsigned int md_len = *signature_length;
    const Vector *key = &this->current_content_key->value;
    const KeyControlBlock *control = &this->current_content_key->control;

    // Check there is a content key
    if (this->current_content_key == NULL) {
        LOGE("[Generic_Sign(): OEMCrypto_ERROR_NO_CONTENT_KEY]");
        return OEMCrypto_ERROR_NO_CONTENT_KEY;
    }
    if (*signature_length < SHA256_DIGEST_LENGTH) {
        *signature_length = SHA256_DIGEST_LENGTH;
        LOGE("[Generic_Sign(): bad signature length.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if ((int)(key->size) != SHA256_DIGEST_LENGTH) {
        LOGE("[Generic_Sign: CONTENT_KEY has wrong size; %d", key->size);
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (!(control->control_bits & kControlAllowSign)) {
        LOGE("[Generic_Sign(): control bit says not allowed.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (control->duration > 0) {
        if (control->duration < SCTXCurrentTimer(this)) {
            LOGE("[Generic_Sign(): key expired.");
            return OEMCrypto_ERROR_KEY_EXPIRED;
        }
    }

#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    if (control->control_bits & kControlReplayMask) {
        if (!SCTXIsUsageEntryValid(this)) {
            LOGE("[Generic_Encrypt(): usage entry not valid]");
            return OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
#endif 
    
    if( algorithm != OEMCrypto_HMAC_SHA256 ) {
        LOGE("[Generic_Sign(): bad algorithm.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    if(HMAC_SHA256( key->buf, key->size,in_buffer, buffer_length, signature,  &md_len))
    {
        *signature_length = md_len;
        return OEMCrypto_SUCCESS;
    }

    LOGE("[Generic_Sign(): hmac failed.");
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
}

OEMCryptoResult SCTXGeneric_Verify(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        const uint8_t* signature,
        size_t signature_length) {

    const Vector *key = &this->current_content_key->value;
    const KeyControlBlock *control = &this->current_content_key->control;

    unsigned int md_len = signature_length;
    uint8_t computed_signature[SHA256_DIGEST_LENGTH];

    // Check there is a content key
    if (this->current_content_key == NULL) {
        LOGE("[Decrypt_Verify(): OEMCrypto_ERROR_NO_CONTENT_KEY]");
        return OEMCrypto_ERROR_NO_CONTENT_KEY;
    }
    if (signature_length < SHA256_DIGEST_LENGTH) {
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    if ((int)(key->size) != SHA256_DIGEST_LENGTH) {
        LOGE("[Generic_Verify(): CONTENT_KEY has wrong size: %d", key->size);
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if (!(control->control_bits & kControlAllowVerify)) {
        LOGE("[Generic_Verify(): control bit says not allowed.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }

    if (control->duration > 0) {
        if (control->duration < SCTXCurrentTimer(this)) {
            LOGE("[Generic_Verify(): key expired.");
            return OEMCrypto_ERROR_KEY_EXPIRED;
        }
    }

#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
    if (control->control_bits & kControlReplayMask) {
        if (!SCTXIsUsageEntryValid(this)) {
            LOGE("[Generic_Encrypt(): usage entry not valid]");
            return OEMCrypto_ERROR_UNKNOWN_FAILURE;
        }
    }
#endif

    if( algorithm != OEMCrypto_HMAC_SHA256 ) {
        LOGE("[Generic_Verify(): bad algorithm.");
        return OEMCrypto_ERROR_UNKNOWN_FAILURE;
    }
    if(HMAC_SHA256( key->buf, key->size,in_buffer, buffer_length, computed_signature, &md_len))
    {
        return (0 == memcmp( signature, computed_signature, SHA256_DIGEST_LENGTH))?
            OEMCrypto_SUCCESS : OEMCrypto_ERROR_SIGNATURE_FAILURE ;
    }

    LOGE("[Generic_Verify(): HMAC failed.");
    return OEMCrypto_ERROR_UNKNOWN_FAILURE;
}

bool SCTXRefreshKey(SessionContext *this,
        const KeyId* key_id,
        const Vector* key_control,
        const Vector* key_control_iv) {

    KeyControlBlock key_control_block;

    Key* content_key;
    const Vector *content_key_value;
    Vector *control=NULL;
    bool ret = true;

    KCBInitRefresh(&key_control_block , true);

    if (VecIsEmpty(key_id)) {
        // Key control is not encrypted if key id is NULL
        if (!SCTXParseKeyControl(this,key_control, &key_control_block)) {
            LOGD("Parse key control error.");
            ret = false;
            goto final;
        }

        if ((key_control_block.control_bits & kControlNonceEnabled)
                && (!SCTXCheckNonce(this, key_control_block.nonce))) {
            ret = false;
            goto final;
        }
        
        // Apply duration to all keys in this session
        return SKTUpdateDuration(&this->session_keys,&key_control_block);
    }

    content_key = SKTFind(&this->session_keys,key_id);

    if (NULL == content_key) {
        LOGD("Error: no matching content key.");
        ret = false;
        goto final;
    }

    if (VecIsEmpty(key_control)) {
        LOGD("Error: no key_control.");
        ret = false;
        goto final;
    }

    content_key_value = &content_key->value;

    // Decrypt encrypted key control block
    if (VecIsEmpty(key_control_iv)) {
        // TODO(fredg): get confirmation from server team this is valid.
        LOGD("Key control block is NOT encrypted.");

        //Force discard const due to this compatible with declaration
        control = (Vector *)key_control;
    } else {
        // TODO(fredg): get confirmation from server team this is valid, too.
        LOGD("Key control block is encrypted.");
        if (!CEDecryptMessage(this->ce,this, content_key_value, key_control_iv,
                    key_control, control)) {
            LOGD("Error decrypting key control block.");
            ret = false;
            goto final;
        }
    }

    if (!SCTXParseKeyControl(this,control, &key_control_block)) {
        LOGD("Error parsing key control.");
        ret = false;
        goto final;
    }

    if ((key_control_block.control_bits & kControlNonceEnabled)
            && (!SCTXCheckNonce(this, key_control_block.nonce))) {
        ret = false;
        goto final;
    }

    if (!KeyUpdateDuration(content_key,&key_control_block)) {
        LOGD("Error updating duration.");
        ret = false;
        goto final;
    }

final:

    return ret;
}

bool SCTXUpdateMacKeys(SessionContext *this,
        const Vector* enc_mac_keys,
        const Vector* iv) {

    // Decrypt mac key from enc_mac_key using devithis->cekeya
    Vector mac_keys;
    bool ret = true;

    VecInit(&mac_keys);
    if (!CEDecryptMessage(this->ce,this, &this->encryption_key, iv,
                enc_mac_keys, &mac_keys)) {
        ret = false;
        goto final;
    }

    VecAssign(&this->mac_key_server,mac_keys.buf , MAC_KEY_SIZE);
    VecAssign(&this->mac_key_client,mac_keys.buf+MAC_KEY_SIZE 
            , mac_keys.size - MAC_KEY_SIZE);

final:
    VecTerm(&mac_keys);
    return ret;
}

bool SCTXSelectContentKey(SessionContext *this,
        const KeyId* key_id) {

    const Key* content_key;

    LOGE("Set Content Key");
    content_key = SKTFind(&this->session_keys,key_id);
    LOGE("Get Content Key");

    if (NULL == content_key) {
        LOGE("[SelectContentKey(): No key matches key id]");
        return false;
    }
    this->current_content_key = content_key;
    return true;
}

void SCTXAddNonce(SessionContext *this,
        uint32_t nonce) {
    NTAddNonce(&this->nonce_table,nonce);
}

bool SCTXCheckNonce(SessionContext *this,
        uint32_t nonce) {
    return NTCheckNonce(&this->nonce_table,nonce);
}

void SCTXFlushNonces(SessionContext *this)
{
    NTFlush(&this->nonce_table);
}

bool SCTXIsUsageEntryValid(SessionContext *this) {
      if (!this->usage_entry_) return false;
      return UTEUpdateTime(this->usage_entry_);
}

void SCTXReleaseUsageEntry(SessionContext *this) { 
    this->usage_entry_ = NULL; 
}

void SCTXset_mac_key_server(SessionContext *this,
        const Vector* mac_key_server)
{
    VecCpy(&this->mac_key_server,mac_key_server);
}

void SCTXset_mac_key_client(SessionContext *this,
        const Vector* mac_key_client)
{
    VecCpy(&this->mac_key_client,mac_key_client);
}

void SCTXset_encryption_key(SessionContext *this,
        const Vector* enc_key)
{
    VecCpy(&this->encryption_key,enc_key);
}
