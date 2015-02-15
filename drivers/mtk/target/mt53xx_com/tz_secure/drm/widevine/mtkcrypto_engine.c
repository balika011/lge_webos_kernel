/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "mtkcrypto_engine.h"
#include "mtkcrypto_usage_table.h"
#include "log.h"
#include "mtkcrypto_key.h"
#include "mtkcrypto_sessionctx.h"
#include "mtkcrypto_glue.h"

#include "string_conversions.h"
#include "wv_cdm_constants.h"

void CEInit(CryptoEngine *this) {
    this->ce_state= CE_INITIALIZED;
    this->current_session= NULL;
    this->valid = true;
    MapInit(&this->sessions,MAP_SCTX);
    WKBInit(&this->keybox);
    this->usage_table_ = UTCreate(this);
}

CryptoEngine* CECreate(void) {
    CryptoEngine *ret = malloc(sizeof(CryptoEngine));
    if(ret != NULL)
    {
        CEInit(ret);
    }
    return ret;
}

void CETerm(CryptoEngine *this) {
#ifdef TODO

#endif
    this->current_session = NULL;
    MapTerm(&this->sessions);
    WKBTerm(&this->keybox);
    UTDestroy(this->usage_table_);
}

void CEDestroy(CryptoEngine *this) {
    if(this !=NULL)
    {
        CETerm(this);
        free(this);
    }
}

void CETerminate(CryptoEngine *this) {
}

bool CEInitialized(CryptoEngine *this){
    return (this->ce_state != CE_ILLEGAL);
}

KeyboxError CEValidateKeybox(CryptoEngine *this) {
    if(this == NULL)
    {
        return WKBValidate(NULL);
    }
    return WKBValidate(&this->keybox);
}

SessionId CECreateSession(CryptoEngine *this) {
#ifdef TODO
    Lock lock(this->session_table_lock);
#endif    
    static int unique_id = 1;
    SessionId sid = (SessionId)++unique_id;
    SessionContext* sctx = SCTXCreate(this, sid);
    SCTXPair *sctxpair = SCTXPairCreate(&sid, sctx);
    MapInsert(&this->sessions,sctxpair);
    return sid;
}

bool CEDestroySession(CryptoEngine *this,
        SessionId sid) {
#ifdef TODO
    AutoLock lock(this->session_table_lock_);
#endif    
    return MapErase(&this->sessions,&sid);
}

SessionContext* CEFindSession(CryptoEngine *this,
        SessionId sid) {      
#ifdef TODO        
    Lock lock(this->sessiont_able_lock_);
#endif    
    return MapFind(&this->sessions,&sid);
}

// Internal utility function to decrypt the message
bool CEDecryptMessage(CryptoEngine *this,
        SessionContext* session,
        const Vector* key,
        const Vector* iv,
        const Vector* message,
        Vector* decrypted) {

    if (VecIsEmpty(key) || VecIsEmpty(iv)||VecIsEmpty(message) || !decrypted) {
        LOGE("[DecryptMessage(): OEMCrypto_ERROR_INVALID_CONTEXT]");
        return false;
    }

    VecResize(decrypted , message->size);

    //Fix Key Lengto to 128 Bit according to the reference design test suite
    //return AES_CBC_DECRYPT(key->buf, key->size, iv->buf, message->buf, decrypted->buf, message->size);
    return AES_CBC_DECRYPT(key->buf, 128/8 , iv->buf, message->buf, decrypted->buf, message->size);
}

void CEset_current_session_(CryptoEngine *this, SessionContext* current) {
    this->current_session = current;
}

struct UsageTable* CEusage_table(CryptoEngine *this) { 
    return this->usage_table_; 
}
