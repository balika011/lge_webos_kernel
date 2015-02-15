/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#ifndef OEMCRYPTO_ENGINE_MOCK_H_
#define OEMCRYPTO_ENGINE_MOCK_H_


#include "lock.h"

#include "mtkcrypto_keybox.h"
#include "OEMCryptoCENC.h"
#include "mtkutil_map.h"
#include "mtkcrypto_sessionctx.h"

#ifndef TODO
typedef uint32_t SemaLock;
#endif

typedef enum CryptoEngineState {
    CE_ILLEGAL,
    CE_INITIALIZED,
    CE_HAS_KEYBOX,
    CE_HAS_SESSIONS,
    CE_ERROR
}CryptoEngineState;

typedef struct CryptoEngine {

  bool valid;
  CryptoEngineState ce_state;
  struct SessionContext* current_session;
  struct UsageTable* usage_table_;
  Map sessions;

  WvKeybox keybox;
  SemaLock session_table_lock;

}CryptoEngine;

void CEInit(CryptoEngine *this);

void CETerm(CryptoEngine *this); 

CryptoEngine* CECreate(void);
void CEDestroy(CryptoEngine *this);

void CETerminate(CryptoEngine *this); 

bool CEInitialized(CryptoEngine *this);

KeyboxError CEValidateKeybox(CryptoEngine *this); 

SessionId CECreateSession(CryptoEngine *this); 

bool CEDestroySession(CryptoEngine *this,
        SessionId sid); 

struct SessionContext* CEFindSession(CryptoEngine *this,
        SessionId sid); 

// Internal utility function to decrypt the message
bool CEDecryptMessage(CryptoEngine *this,
        struct SessionContext* session,
        const Vector* key,
        const Vector* iv,
        const Vector* message,
        Vector* decrypted); 

void CEset_current_session_(CryptoEngine *this,
    SessionContext* current); 

struct UsageTable* CEusage_table(CryptoEngine *this);

#endif
