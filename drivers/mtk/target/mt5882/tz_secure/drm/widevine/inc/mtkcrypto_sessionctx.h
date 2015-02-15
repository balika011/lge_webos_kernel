#ifndef MTKCRYPTO_SESSIONCTX_H_
#define MTKCRYPTO_SESSIONCTX_H_

#include "OEMCryptoCENC.h"
#include "mtkcrypto_sessionkeytbl.h"
#include "mtkcrypto_noncetbl.h"
#include "mtkcrypto_key.h"
#include "mtkcrypto_glue.h"

typedef enum {
  kBufferTypeClear,
  kBufferTypeSecure,
  kBufferTypeDirect
}BufferType;

typedef struct SessionContext {

  bool valid;
  struct CryptoEngine* ce;
  SessionId id;
  Vector mac_key_server;
  Vector mac_key_client;
  Vector encryption_key;
  Vector session_key;

  const Key* current_content_key;
  SessionKeyTable session_keys;
  NonceTable nonce_table;
  RSA* rsa_key;

  uint32_t  timer_start;
  uint32_t allowed_schemes;  // for RSA signatures.
  
  struct UsageTableEntry* usage_entry_;
  
}SessionContext;

SessionContext * SCTXCreate(struct CryptoEngine *ce, SessionId sid);
void SCTXDestroy(SessionContext *this);

bool SCTXDeriveKeys(SessionContext *this,
        const Vector* master_key,
        const Vector* mac_key_context,
        const Vector* enc_key_context);

bool SCTXRSADeriveKeys(SessionContext *this,
        const Vector* enc_session_key,
        const Vector* mac_key_context,
        const Vector* enc_key_context);

bool SCTXGenerateSignature(SessionContext *this,
        const uint8_t* message,
        size_t message_length,
        uint8_t* signature,
        size_t* signature_length);

size_t SCTXRSASignatureSize(SessionContext *this);

bool SCTXGenerateRSASignature(SessionContext *this,
        const uint8_t* message,
        const size_t message_length,
        uint8_t* signature,
        size_t* signature_length,
        RSA_Padding_Scheme padding_scheme);

bool SCTXValidateMessage(SessionContext *this,
        const uint8_t* given_message,
        size_t message_length,
        const uint8_t* given_signature,
        size_t signature_length);

bool SCTXParseKeyControl(SessionContext *this,
        const Vector* key_control_string,
        KeyControlBlock* key_control_block);

void SCTXStartTimer(SessionContext *this);

uint32_t SCTXCurrentTimer(SessionContext *this);

OEMCryptoResult SCTXLoadKeys(SessionContext *this,
        const uint8_t* message, size_t message_length, 
        const uint8_t* signature, size_t signature_length, 
        const uint8_t* enc_mac_key_iv,
        const uint8_t* enc_mac_keys, size_t num_keys,
        const OEMCrypto_KeyObject* key_array, 
        const uint8_t* pst,size_t pst_length);

bool SCTXInstallKey(SessionContext *this,
        const KeyId *key_id,
        const Vector *key_data,
        const Vector *key_data_iv,
        const Vector *key_control,
        const Vector *key_control_iv,
        const Vector *pst);

bool  SCTXDecryptRSAKey(SessionContext *this,
        const uint8_t* enc_rsa_key,
        size_t enc_rsa_keylength,
        const uint8_t* enc_rsa_keyiv,
        uint8_t* pkcs8_rsa_key);

bool SCTXEncryptRSAKey(SessionContext *this,
        const uint8_t* pkcs8_rsa_key,
        size_t enc_rsa_keylength,
        const uint8_t* enc_rsa_keyiv,
        uint8_t* enc_rsa_key);

bool SCTXLoadRSAKey(SessionContext *this,
        uint8_t* pkcs8_rsa_key,
        size_t rsa_keylength,
        const uint8_t* message,
        size_t message_length,
        const uint8_t* signature,
        size_t signature_length);

OEMCryptoResult SCTXDecryptCTR(SessionContext* this,
        const uint8_t* iv,
        size_t block_offset,
        const uint8_t* cipher_data,
        size_t cipher_data_length,
        bool is_encrypted,
        void* clear_data,
        BufferType buffer_type);

OEMCryptoResult SCTXGeneric_Encrypt(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        const uint8_t* iv,
        OEMCrypto_Algorithm algorithm,
        uint8_t* out_buffer);

OEMCryptoResult SCTXGeneric_Decrypt(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        const uint8_t* iv,
        OEMCrypto_Algorithm algorithm,
        uint8_t* out_buffer);

OEMCryptoResult SCTXGeneric_Sign(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        uint8_t* signature,
        size_t* signature_length);

OEMCryptoResult SCTXGeneric_Verify(SessionContext *this,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        const uint8_t* signature,
        size_t signature_length);

bool SCTXRefreshKey(SessionContext *this,
        const KeyId* key_id,
        const Vector* key_control,
        const Vector* key_control_iv);

bool SCTXUpdateMacKeys(SessionContext *this,
        const Vector* enc_mac_keys,
        const Vector* iv);

bool SCTXSelectContentKey(SessionContext *this, const KeyId* key_id);

void SCTXAddNonce(SessionContext *this, uint32_t nonce);
bool SCTXCheckNonce(SessionContext *this, uint32_t nonce);
void SCTXFlushNonces(SessionContext *this);

bool SCTXIsUsageEntryValid(SessionContext *this);
void SCTXReleaseUsageEntry(SessionContext *this);

void SCTXset_mac_key_server(SessionContext *this, const Vector* mac_key_server);
void SCTXset_mac_key_client(SessionContext *this, const Vector* mac_key_client);
void SCTXset_encryption_key(SessionContext *this, const Vector* enc_key);

#endif
