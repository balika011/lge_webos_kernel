/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#ifndef OEMCRYPTO_SESSION_TZ_H_
#define OEMCRYPTO_SESSION_TZ_H_

#include "oemcrypto_key_tz.h"
#include "oemcrypto_keybox_tz.h"
#include "tomcrypt.h"


#ifdef __cplusplus
extern "C"{
#endif



// SessionKeyTable holds the keys for the current session

bool Insert(Keypair * keypair, int keypairlen, const KeyId key_id, const WvKey* key_data);
WvKey* Find(Keypair * keypair, int keypairlen, const KeyId key_id, int key_id_len);
void Remove(Keypair * keypair, int keypairlen, const KeyId key_id);
bool UpdateDuration(Keypair * keypair, int keypairlen, const KeyControlBlock* control);


bool GenerateNonceNonTable(NonceTable * noncetb,uint32_t * nonce);
bool CheckNonceNonTable(NonceTable * noncetb,uint32_t nonce);
void FlushNonTable(NonceTable * noncetb);


bool SessAddConKey(SessionContext *sesscnt,WvKey * cntkey);



bool DecryptRSAKey(SessionContext * sesscnt,const uint8_t* enc_rsa_key,
                   size_t enc_rsa_key_length,
                   const uint8_t* wrapped_rsa_key_iv,
                   uint8_t* pkcs8_rsa_key);
bool EncryptRSAKey(SessionContext * sesscnt,const uint8_t* pkcs8_rsa_key,
                   size_t enc_rsa_key_length,
                   const uint8_t* enc_rsa_key_iv,
                   uint8_t* enc_rsa_key);
bool LoadRSAKey(SessionContext * sesscnt,uint8_t* pkcs8_rsa_key,
                size_t rsa_key_length,
                const uint8_t* message,
                size_t message_length,
                const uint8_t* signature,
                size_t signature_length);
bool ParseKeyControl(SessionContext * sesscnt,const uint8_t* key_control_string,size_t key_cnt_strlen,
                     KeyControlBlock* key_control_block);

inline void set_mac_key_server(SessionContext * sesscnt,const uint8_t* mac_key_server, size_t mac_key_sev_len)
{
    memcpy(sesscnt->mac_key_server_ , mac_key_server,mac_key_sev_len);
}
//const std::vector<uint8_t>& mac_key_server(SessionContext * sesscnt,) { return mac_key_server_; }
inline void set_mac_key_client(SessionContext * sesscnt,const uint8_t* mac_key_client,size_t mac_key_clt_len)
{
    memcpy(sesscnt->mac_key_client_, mac_key_client,mac_key_clt_len);
};
//const std::vector<uint8_t>& mac_key_client(SessionContext * sesscnt,) { return mac_key_client_; }

inline void set_encryption_key(SessionContext * sesscnt,const uint8_t* enc_key,size_t enc_key_len)
{
    memcpy(sesscnt->encryption_key_ , enc_key, enc_key_len);
}
//const std::vector<uint8_t>& encryption_key(SessionContext * sesscnt) { return encryption_key_; }


bool DeriveKey(SessionContext * sesscnt,const uint8_t* key, size_t keylen, const uint8_t* context,
               size_t cont_len, int counter, uint8_t* out, size_t * outlen);


bool SessAddConKeypair(SessionContext *sesscnt,const uint8_t * keyid, size_t keyid_len, WvKey * cntkey);


///in the usr space
/*warning , this function will not entering into tz. just in the usr space
** METHOD: RSASignatureSize
** computer the rsa signature
** Parameters:
** [in]   sesscnt: the session point in the tz
**  Returns:  the length of the signature.
**               
*/
extern size_t RSASignatureSize(SessionContext *sesscnt);



#if 1
//TZ interface for usr
/*
** METHOD: CreatSessionTz
** creat a session to do the drm function for the wv-drm-plugin
** times to specify multiple filters.
** Parameters:
** [in]   none
** [out] none
**  Returns: NULL : can not creat session, else a point of a SessionContext 
*/
 SessionContext * CreatSessionTz();

/*
** METHOD: DestroySessionTz
** destroy the corresponding session
** Parameters:
** [in]   sesscnt
** [out] none
**  Returns: false : can not destroy session
**               true:  destroy session OK.
*/
extern bool DestroySessionTz( SessionContext *sesscnt);


#define DERIVE_ENC_SER_CLINT_KEY
#ifdef DERIVE_ENC_SER_CLINT_KEY
/*
** METHOD: DeriveKeys
** derive keys for the current session. 
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   master_key: the key point
** [in]   mst_cnt_len: the master key length
** [in]   mac_context: the point of the mac_context
** [in]   mac_cnt_len:  the length of the mac_context
** [in]   enc_context:  the point of the enc_context
** [in]   enc_cnt_len:  the length of the enc_context
**  Returns: false : can not DeriveKeys
**               true:  DeriveKeys OK.
*/
extern bool DeriveKeys( SessionContext * sesscnt,const uint8_t * master_key, size_t mst_cnt_len,
                           const uint8_t* mac_context, size_t mac_cnt_len,
                           const uint8_t* enc_context,size_t enc_cnt_len);

/*
** METHOD: RSADeriveKeys
** derive keys for the current session with session key. 
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   enc_session_key: the key point
** [in]   enc_sesskey_len: the session key length
** [in]   mac_context: the point of the mac_context
** [in]   mac_cnt_len:  the length of the mac_context
** [in]   enc_context:  the point of the enc_context
** [in]   enc_cnt_len:  the length of the enc_context
**  Returns: false : can not DeriveKeys
**               true:  DeriveKeys OK.
*/

extern bool RSADeriveKeys( SessionContext * sesscnt,const uint8_t* enc_session_key, size_t enc_sesskey_len,
                              const uint8_t* mac_context,size_t mac_cnt_len,
                              const uint8_t* enc_context, size_t enc_cnt_len);

#endif

#define SHA256_SIGN_VERIFY
#ifdef SHA256_SIGN_VERIFY
/*
** METHOD: GenerateSignature
** generate some signature of the message
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   message: the key point
** [in]   message_length: the session key length
** [out]   signature: the point used to store the  signature
** [out]   signature_length:  used to store the signature length
**  Returns: false : generate error
**               true:  generate ok
*/
extern bool GenerateSignature( SessionContext *sesscnt,
                                  const uint8_t* message,
                                  size_t message_length,
                                  uint8_t* signature,
                                  size_t* signature_length);


/*
** METHOD: ValidateMessage
** validate the message to check if it has been modified
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   given_message: the message need to valid
** [in]   message_length: the message length
** [in]   given_signature: the signature of the message
** [in]   signature_length:  the length of the signature
**  Returns: false : validate fail
**               true:  validate ok
*/
extern bool ValidateMessage( SessionContext *sesscnt,
                                const uint8_t* given_message,
                                size_t message_length,
                                const uint8_t* given_signature,
                                size_t signature_length);
/*
** METHOD: Generic_Sign
** generate some signature of the message use the content key
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   in_buffer: the key point
** [in]   buffer_length: the session key length
** [in]   algorithm: an enum, can use int type to substitute it
** [out]   signature: the point used to store the  signature
** [out]   signature_length:  used to store the signature length
** [in]    curtime: the time passed in
**  Returns: false : generate error
**               true:  generate ok
*/

extern bool Generic_Sign( SessionContext *sesscnt, const uint8_t* in_buffer,
                             size_t buffer_length,
                             OEMCrypto_Algorithm algorithm,
                             uint8_t* signature,
                             size_t* signature_length, uint32_t curtime);

/*
** METHOD: Generic_Verify
** verify the signature of the message with the content key
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   in_buffer: the key point
** [in]   buffer_length: the session key length
** [in]   algorithm: an enum, can use int type to substitute it
** [in]   signature: the point used to store the  signature
** [in]   signature_length: the signature length
** [in]    curtime: the time passed in
**  Returns: false : verify error
**               true:  verify ok
*/
extern bool Generic_Verify( SessionContext *sesscnt, const uint8_t* in_buffer,
                               size_t buffer_length,
                               OEMCrypto_Algorithm algorithm,
                               const uint8_t* signature,
                               size_t signature_length, uint32_t curtime);

#endif

#define KEY_RELATED
#ifdef  KEY_RELATED
/*
** METHOD: InstallKey
** InstallKey keys to the trust zone
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   key_id: the message need to valid
** [in]   key_id_len: the message length
** [in]   key_data: the signature of the message
** [in]   key_data_len_len:  the length of the signature
** [in]   key_data_iv: the signature of the message
** [in]   key_data_iv_len:  the length of the signature
** [in]   key_control: the signature of the message
** [in]   key_control_len:  the length of the signature
** [in]   key_control_iv: the signature of the message
** [in]   key_control_iv_len:  the length of the signature
**  Returns: false : install fail
**               true:  install ok
*/
extern bool InstallKey( SessionContext * sesscnt,const uint8_t* key_id, size_t key_id_len,
                           const uint8_t* key_data,size_t key_data_len_len,
                           const uint8_t* key_data_iv,size_t key_data_iv_len,
                           const uint8_t* key_control,size_t key_control_len,
                           const uint8_t* key_control_iv,size_t key_control_iv_len);

/*
** METHOD: InstallKey
** InstallKey keys to the trust zone
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   key_id: the message need to valid
** [in]   keyid_len: the message length
** [in]   key_control: the signature of the message
** [in]   key_cnt_len:  the length of the signature
** [in]   key_control_iv: the signature of the message
** [in]   key_cnt_iv_len:  the length of the signature
**  Returns: false : install fail
**               true:  install ok
*/
extern bool RefreshKey( SessionContext * sesscnt,const uint8_t* key_id, size_t keyid_len,
                           uint8_t* key_control,size_t key_cnt_len,
                           const uint8_t* key_control_iv, size_t key_cnt_iv_len);

/*
** METHOD: UpdateMacKeys
** update keys to the trust zone
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   mac_keys: the message need to valid
** [in]   mac_key_len: the message length
** [in]   iv: the signature of the message
** [in]   iv_len:  the length of the signature
**  Returns: false : UpdateMacKeys fail
**               true:  UpdateMacKeys ok
*/
extern bool UpdateMacKeys( SessionContext * sesscnt,const uint8_t* mac_keys, size_t mac_key_len,
                              const uint8_t* iv, size_t iv_len);
/*
** METHOD: SelectContentKey
** select content key
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   key_id: key id of the content key
** [in]   key_id_len: the len of the key id
**  Returns: false : key_id_len fail
**               true:  key_id_len ok
*/
extern bool SelectContentKey( SessionContext * sesscnt,const KeyId key_id ,int key_id_len);

#endif

#define NONCE_RELATED
#ifdef  NONCE_RELATED
/*
** METHOD: GenerateNonceSess
** generate nonce
** Parameters:
** [in]   sesscnt: the session point in the tz
** [out]   nonce: the nonce been generate
**  Returns: false : Generate fail
**               true:  Generate ok
*/
extern void GenerateNonceSess( SessionContext * sesscnt, uint32_t * nonce);
/*
** METHOD: CheckNonceSess
** check the nonce if it is really been generated by this device
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   nonce: nonce need to check
**  Returns: false : key_id_len fail
**               true:  key_id_len ok
*/
extern bool CheckNonceSess( SessionContext * sesscnt,uint32_t nonce);
/*
** METHOD: FlushNoncesSess
** flush nonce
** Parameters:
** [in]   sesscnt: the session point in the tz
**  Returns: none
**               
*/
extern void FlushNoncesSess(  SessionContext * sesscnt);

#endif


#define RSA_RELATED
#ifdef RSA_RELATED
/*
** METHOD: RewrapDeviceRSAKeySess
** encrypt the rsa key and pass it out.
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   message: the message need to valid
** [in]   message_length: the message length
** [in]   signature: the signature of the message
** [in]   signature_length:  the length of the signature
** [in]   nonce: the nonce to be checked
** [in]   enc_rsa_key:  the enc_rsa_key
** [in]   enc_rsa_key_length: the length of enc_rsa_key
** [in]   enc_rsa_key_iv:  enc_rsa_key_iv
** [out]   wrapped_rsa_key: the wrapped rsa 
** [out]   wrapped_rsa_key_length:  the length of the wrapped rsa key
**  Returns: false : install fail
**               true:  install ok
*/
extern int RewrapDeviceRSAKeySess( SessionContext* session_ctx,
                                      const uint8_t* message,
                                      size_t message_length,
                                      const uint8_t* signature,
                                      size_t signature_length,
                                      const uint32_t nonce,
                                      const uint8_t* enc_rsa_key,
                                      size_t enc_rsa_key_length,
                                      const uint8_t* enc_rsa_key_iv,
                                      uint8_t* wrapped_rsa_key,
                                      size_t*  wrapped_rsa_key_length);


/*
** METHOD: LoadDeviceRSAKeySessCtx
** load rsa key into the tz
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   wrapped_rsa_key: the rsa_key need to load
** [in]   wrapped_rsa_key_length: the rsa key length
**  Returns: 0 : ok
**               other:  fail
*/
extern int LoadDeviceRSAKeySessCtx( SessionContext* session_ctx,
                                       const uint8_t* wrapped_rsa_key,
                                       size_t wrapped_rsa_key_length);

/*
** METHOD: GenerateRSASignature
** generate sign
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   message: the message need to generate
** [in]   message_length: the message length
** [out]   signature: the signature signed.
** [out]   signature_length:  the length of the signature
**  Returns: false :  fail
**               true:   ok
*/

extern bool GenerateRSASignature( SessionContext *sesscnt,
                                     const uint8_t* message,
                                     size_t message_length,
                                     uint8_t* signature,
                                     size_t* signature_length);

#endif
#endif



#if 0
///kernel interface for usr

/*
** METHOD: DecryptCTR
** DecryptCTR the data 
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   iv: the iv used to decrypt
** [in]   block_offset: 
** [in]   cipher_data: the addr of the encrypted data
** [in]   cipher_data_length:  the length of the encrypted data
** [in]   is_encrypted: 
** [out]   clear_data: the addr of the decrypted data to be stored
** [in]   buffer_type: the signature of the message
** [in]   curtime:  the length of the signature
**  Returns: false : DecryptCTR fail
**               true:  DecryptCTR ok
*/
extern bool DecryptCTR( SessionContext* session,
                           const uint8_t* iv,
                           size_t block_offset,
                           const uint8_t* cipher_data,
                           size_t cipher_data_length,
                           bool is_encrypted,
                           void* clear_data,
                           BufferType buffer_type,uint32_t curtime);

/*
** METHOD: Generic_Encrypt
** encrypt the data 
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   in_buffer: the point of the data need to encrypt
** [in]   buffer_length: the length of the data need to encrypt
** [in]   iv: the iv used to  encrypt data
** [in]   algorithm:  algorithm used
** [out]   out_buffer: use to store the encrypted data
** [in]   curtime:  the curtime 
**  Returns: false : Generic_Encrypt fail
**               true:  Generic_Encrypt ok
*/
extern bool Generic_Encrypt( SessionContext *sesscnt, const uint8_t* in_buffer,
                                size_t buffer_length,
                                const uint8_t* iv,
                                OEMCrypto_Algorithm algorithm,
                                uint8_t* out_buffer, uint32_t curtime);

/*
** METHOD: Generic_Decrypt
** encrypt the data 
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   in_buffer: the point of the data need to encrypt
** [in]   buffer_length: the length of the data need to encrypt
** [in]   iv: the iv used to  encrypt data
** [in]   algorithm:  algorithm used
** [out]   out_buffer: use to store the encrypted data
** [in]   curtime:  the curtime 
**  Returns: false : Generic_Decrypt fail
**               true:  Generic_Decrypt ok
*/
extern bool Generic_Decrypt( SessionContext *sesscnt, const uint8_t* in_buffer,
                                size_t buffer_length,
                                const uint8_t* iv,
                                OEMCrypto_Algorithm algorithm,
                                uint8_t* out_buffer, uint32_t curtime);

#endif

/*
** METHOD: GetRandomNum
** generate random data
** Parameters:
** [out]   randomData: store the random data
** [in]   dataLength: the length of the random data
**  Returns: false :  fail
**               true:   ok
*/
extern bool GetRandomNum(uint8_t * randomData, size_t dataLength);
 


#if 1 
// these two function provide the interface to check the validation of the current content key.
/*
** METHOD: CheckDecCTRContentKey
** check the current content key if it is valid, and get the slot of the gcpu. this func used for the DecrypCRT to check.
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   curtime: the cur time
** [out]   tmp_slot: use to store the slot of the current key slot
**  Returns: false :  fail
**               true:   ok
*/
bool CheckDecCTRContentKey( SessionContext *sesscnt,uint32_t curtime,BufferType buffer_type,unsigned short * tmp_slot);

/*
** METHOD: CheckEncDecCBCContentKey
** check the current content key if it is valid, and get the slot of the gcpu. this func used for the Generic_Encrypt Generic_Decrypt to check.
** Parameters:
** [in]   sesscnt: the session point in the tz
** [in]   curtime: the cur time
** [int]  enc_dec: indicate for Generic_Encrypt:0  Generic_Decrypt :1
** [out]   tmp_slot: use to store the slot of the current key slot
**  Returns: false :  fail
**               true:   ok
*/
bool CheckEncDecCBCContentKey( SessionContext *sesscnt,uint32_t curtime,int enc_dec,unsigned short * tmp_slot);


#endif

#ifdef __cplusplus
}
#endif

#endif

