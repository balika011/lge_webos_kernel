/*********************************************************************
  *OEMCryptoCENC.h
 *
 * (c) Copyright 2013 Google, Inc.
 *
 * Reference APIs needed to support Widevine's crypto algorithms.
 *********************************************************************/

#ifndef OEMCRYPTO_ARG_H_
#define OEMCRYPTO_ARG_H_

#include "rpmb.h"

typedef struct OpenSessionArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION *session;
}OpenSessionArg;

typedef struct CloseSessionArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
}CloseSessionArg;

typedef struct GenerateDerivedKeysArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t *mac_key_context;
    uint32_t mac_key_context_length;
    const uint8_t *enc_key_context;
    uint32_t enc_key_context_length;
}GenerateDerivedKeysArg;

typedef struct GenerateNonceArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    uint32_t *nonce;
}GenerateNonceArg;

typedef struct GenerateSignatureArg{
    OEMCryptoResult *result ; 
    OEMCrypto_SESSION session;
    const uint8_t *message;
    size_t message_length;
    uint8_t *signature;
    size_t *signature_length;
}GenerateSignatureArg;

typedef struct LoadKeysArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t *message;
    size_t message_length;
    const uint8_t *signature;
    size_t signature_length;
    const uint8_t *enc_mac_keys_iv;
    const uint8_t *enc_mac_keys;
    size_t num_keys;
    const OEMCrypto_KeyObject *key_array;
    const uint8_t* pst;
    size_t pst_length;    
}LoadKeysArg;

typedef struct RefreshKeysArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t *message;
    size_t message_length;
    const uint8_t *signature;
    size_t signature_length;
    size_t num_keys;
    const OEMCrypto_KeyRefreshObject *key_array;
}RefreshKeysArg;

typedef struct SelectKeyArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t *key_id;
    size_t key_id_length;
}SelectKeyArg;

typedef struct DecryptCTRArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *data_addr;
    size_t data_length;
    bool is_encrypted;
    const uint8_t *iv;
    size_t block_offset;
    const OEMCrypto_DestBufferDesc *out_buffer;
    uint8_t subsample_flags;
}DecryptCTRArg;

typedef struct InstallKeyboxArg{
    OEMCryptoResult *result ;
    const uint8_t *keybox;
    size_t keyBoxLength;
}InstallKeyboxArg;

typedef struct GetDeviceIDArg{
    OEMCryptoResult *result ;
    uint8_t *deviceID;
    size_t *idLength;
}GetDeviceIDArg;

typedef struct GetKeyDataArg{
    OEMCryptoResult *result ;
    uint8_t *keyData;
    size_t *keyDataLength;
}GetKeyDataArg;

typedef struct GetRandomArg{
    OEMCryptoResult *result ;
    uint8_t *randomData;
    size_t dataLength;
}GetRandomArg;

typedef struct WrapKeyboxArg{
    OEMCryptoResult *result ;
    const uint8_t *keybox;
    size_t keyBoxLength;
    uint8_t *wrappedKeybox;
    size_t *wrappedKeyBoxLength;
    const uint8_t *transportKey;
    size_t transportKeyLength;
}WrapKeyboxArg;

typedef struct RewrapDeviceRSAKeyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *message;
    size_t message_length;
    const uint8_t *signature;
    size_t signature_length;
    const uint32_t *nonce;
    const uint8_t *enc_rsa_key;
    size_t enc_rsa_key_length;
    const uint8_t *enc_rsa_key_iv;
    uint8_t *wrapped_rsa_key;
    size_t *wrapped_rsa_key_length;
}RewrapDeviceRSAKeyArg;

typedef struct LoadDeviceRSAKeyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *wrapped_rsa_key;
    size_t wrapped_rsa_key_length;
}LoadDeviceRSAKeyArg;

typedef struct GenerateRSASignatureArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *message;
    size_t message_length;
    uint8_t *signature;
    size_t *signature_length;    
    RSA_Padding_Scheme padding_scheme;
}GenerateRSASignatureArg;

typedef struct DeriveKeysFromSessionKeyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *enc_session_key;
    size_t enc_session_key_length;
    const uint8_t *mac_key_context;
    size_t mac_key_context_length;
    const uint8_t *enc_key_context;
    size_t enc_key_context_length;
}DeriveKeysFromSessionKeyArg;

typedef struct GenericEncryptArg{                                                   
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *in_buffer;
    size_t buffer_length;
    const uint8_t *iv;
    OEMCrypto_Algorithm algorithm;
    uint8_t *out_buffer;
}GenericEncryptArg;

typedef struct GenericDecryptArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *in_buffer;
    size_t buffer_length;
    const uint8_t *iv;
    OEMCrypto_Algorithm algorithm;
    uint8_t *out_buffer;
}GenericDecryptArg;

typedef struct GenericSignArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *in_buffer;
    size_t buffer_length;
    OEMCrypto_Algorithm algorithm;
    uint8_t *signature;
    size_t *signature_length;
}GenericSignArg;

typedef struct GenericVerifyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *in_buffer;
    size_t buffer_length;
    OEMCrypto_Algorithm algorithm;
    const uint8_t *signature;
    size_t signature_length;
}GenericVerifyArg;

typedef struct DeactivateUsageEntryArg{
        OEMCryptoResult *result ;
        const uint8_t *pst;
        size_t pst_length;
}DeactivateUsageEntryArg;

typedef struct ReportUsageArg{
    OEMCryptoResult *result ;
        OEMCrypto_SESSION session;
        const uint8_t *pst;
        size_t pst_length;
        OEMCrypto_PST_Report *buffer;
        size_t *buffer_length;
}ReportUsageArg;

typedef struct DeleteUsageEntryArg{
        OEMCryptoResult *result ;
        OEMCrypto_SESSION session;
        const uint8_t* pst;
        size_t pst_length;
        const uint8_t *message;
        size_t message_length;
        const uint8_t *signature;
        size_t signature_length; 
}DeleteUsageEntryArg;

typedef struct GetUsageTableWriteRequestArg{
        OEMCryptoResult *result ;
        uint8_t* buffer;
        size_t * buffer_length;
        RPMB_DATAFRAM *dFrame;
}GetUsageTableWriteRequestArg;

typedef struct VerifyWriteResponseArg{
        OEMCryptoResult *result ;
        const RPMB_DATAFRAM *dFrame;
}VerifyWriteResponseArg;

typedef struct GetUsageTableReadRequesArg{
        OEMCryptoResult *result ;
        RPMB_DATAFRAM *dFrame;
}GetUsageTableReadRequesArg;

typedef struct VerifyReadResponseArg{
        OEMCryptoResult *result ;
        const uint8_t* buffer;
        size_t  buffer_length;
        const RPMB_DATAFRAM *dFrame;
}VerifyReadResponseArg;

typedef struct SetEntitlementKeyArg{
    OEMCryptoResult *result;
    const uint8_t *emmKey;
    uint32_t emmKeyLength;
}SetEntitlementKeyArg;

typedef struct DeriveControlWordArg{
    OEMCryptoResult *result;
    const uint8_t *ecm;
    uint32_t length;
    uint32_t *flags;
}DeriveControlWordArg;

typedef struct DecryptVideoArg{
    OEMCryptoResult *result;
    uint8_t *iv;
    uint8_t *input;
    uint32_t inputLength;
    uint32_t output_handle;
    uint32_t output_offset;
    uint32_t *outputLength;
}DecryptVideoArg;

typedef struct DecryptAudioArg{
    OEMCryptoResult *result;
    uint8_t *iv; 
    uint8_t *input;
    uint32_t inputLength;
    uint8_t *output;
    uint32_t *outputLength;
}DecryptAudioArg;

#endif  // OEMCRYPTO_CENC_H_
