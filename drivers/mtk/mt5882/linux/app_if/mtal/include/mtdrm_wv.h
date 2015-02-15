/*********************************************************************
 * OEMCryptoCENC.h
 *
 * (c) Copyright 2013 Google, Inc.
 *
 * Reference APIs needed to support Widevine's crypto algorithms.
 *********************************************************************/

#ifndef OEMCRYPTO_CENC_H_
#define OEMCRYPTO_CENC_H_

#include "mtdrm.h"

//API 8
#define OEMCrypto_Initialize               _oecc01
#define OEMCrypto_Terminate                _oecc02
#define OEMCrypto_InstallKeybox            _oecc03
#define OEMCrypto_GetKeyData               _oecc04
#define OEMCrypto_IsKeyboxValid            _oecc05
#define OEMCrypto_GetRandom                _oecc06
#define OEMCrypto_GetDeviceID              _oecc07
#define OEMCrypto_WrapKeybox               _oecc08
#define OEMCrypto_OpenSession              _oecc09
#define OEMCrypto_CloseSession             _oecc10
#define OEMCrypto_DecryptCTR               _oecc11
#define OEMCrypto_GenerateDerivedKeys      _oecc12
#define OEMCrypto_GenerateSignature        _oecc13
#define OEMCrypto_GenerateNonce            _oecc14
#define OEMCrypto_LoadKeys_V8              _oecc15
#define OEMCrypto_RefreshKeys              _oecc16
#define OEMCrypto_SelectKey                _oecc17
#define OEMCrypto_RewrapDeviceRSAKey       _oecc18
#define OEMCrypto_LoadDeviceRSAKey         _oecc19
#define OEMCrypto_GenerateRSASignature_V8  _oecc20
#define OEMCrypto_DeriveKeysFromSessionKey _oecc21
#define OEMCrypto_APIVersion               _oecc22
#define OEMCrypto_SecurityLevel            _oecc23
#define OEMCrypto_Generic_Encrypt          _oecc24
#define OEMCrypto_Generic_Decrypt          _oecc25
#define OEMCrypto_Generic_Sign             _oecc26
#define OEMCrypto_Generic_Verify           _oecc27

//API 9
#define OEMCrypto_GetHDCPCapability        _oecc28
#define OEMCrypto_SupportsUsageTable       _oecc29
#define OEMCrypto_UpdateUsageTable         _oecc30
#define OEMCrypto_DeactivateUsageEntry     _oecc31
#define OEMCrypto_ReportUsage              _oecc32
#define OEMCrypto_DeleteUsageEntry         _oecc33
#define OEMCrypto_DeleteUsageTable         _oecc34
#define OEMCrypto_LoadKeys                 _oecc35
#define OEMCrypto_GenerateRSASignature     _oecc36

//Classical DRM
#define OEMCrypto_Initialize_ex            _oec01
#define OEMCrypto_Terminate_ex             _oec02
#define OEMCrypto_SetEntitlementKey        _oec03
#define OEMCrypto_DeriveControlWord        _oec04
#define OEMCrypto_DecryptVideo             _oec05
#define OEMCrypto_DecryptAudio             _oec06
#define OEMCrypto_DecryptCTS               _oec21

//Dummy Function for linking
#define OEMCrypto_InstallKeybox_ex         _oec07
#define OEMCrypto_GetKeyData_ex            _oec08
#define OEMCrypto_IsKeyboxValid_ex         _oec09
#define OEMCrypto_GetRandom_ex             _oec10
#define OEMCrypto_GetDeviceID_ex           _oec11
#define OEMCrypto_EnterSecurePlayback_ex   _oec12
#define OEMCrypto_ExitSecurePlayback_ex    _oec13
#define OEMCrypto_WrapKeybox_ex            _oec14



#define SHA256_DIGEST_LENGTH    32

#define  KEY_CONTROL_SIZE   16
#define  KEY_IV_SIZE        16
#define  KEY_PAD_SIZE       16
#define  KEY_SIZE           16
#define  MAC_KEY_SIZE       32
#define  CTRL_WORD_SIZE     32
#define  FLAGS_SIZE          4


#define RPMB_NONCE_SIZE 16
#define RPMB_DATA_SIZE 256

//===================================
// WV Level 1 Wrapper
//===================================

typedef struct RPMB_DATAFRAM{
    unsigned short type;
    unsigned short result;
    unsigned short blkCnt;
    unsigned short addr;
    unsigned int  writeCnt;
    unsigned char nonce[RPMB_NONCE_SIZE];
    unsigned char data[RPMB_DATA_SIZE];
    unsigned char mac[32];
    unsigned char stuff[196];
}RPMB_DATAFRAM;

typedef struct {
    uint8_t signature[MAC_KEY_SIZE];
    uint8_t context[MAC_KEY_SIZE];
    uint8_t iv[KEY_IV_SIZE];
#ifdef TODO
    //    enc_rsa_key is a variable size
#else
    uint8_t enc_rsa_key[512];
#endif
} WrappedRSAKey;

typedef uint32_t OEMCrypto_SESSION;

typedef enum OEMCryptoResult {
    OEMCrypto_SUCCESS                            = 0,
    OEMCrypto_ERROR_INIT_FAILED                  = 1,
    OEMCrypto_ERROR_TERMINATE_FAILED             = 2,
    OEMCrypto_ERROR_OPEN_FAILURE                 = 3,
    OEMCrypto_ERROR_CLOSE_FAILURE                = 4,
    OEMCrypto_ERROR_ENTER_SECURE_PLAYBACK_FAILED = 5,
    OEMCrypto_ERROR_EXIT_SECURE_PLAYBACK_FAILED  = 6,
    OEMCrypto_ERROR_SHORT_BUFFER                 = 7,
    OEMCrypto_ERROR_NO_DEVICE_KEY                = 8,
    OEMCrypto_ERROR_NO_ASSET_KEY                 = 9,
    OEMCrypto_ERROR_KEYBOX_INVALID               = 10,
    OEMCrypto_ERROR_NO_KEYDATA                   = 11,
    OEMCrypto_ERROR_NO_CW                        = 12,
    OEMCrypto_ERROR_DECRYPT_FAILED               = 13,
    OEMCrypto_ERROR_WRITE_KEYBOX                 = 14,
    OEMCrypto_ERROR_WRAP_KEYBOX                  = 15,
    OEMCrypto_ERROR_BAD_MAGIC                    = 16,
    OEMCrypto_ERROR_BAD_CRC                      = 17,
    OEMCrypto_ERROR_NO_DEVICEID                  = 18,
    OEMCrypto_ERROR_RNG_FAILED                   = 19,
    OEMCrypto_ERROR_RNG_NOT_SUPPORTED            = 20,
    OEMCrypto_ERROR_SETUP                        = 21,
    OEMCrypto_ERROR_OPEN_SESSION_FAILED          = 22,
    OEMCrypto_ERROR_CLOSE_SESSION_FAILED         = 23,
    OEMCrypto_ERROR_INVALID_SESSION              = 24,
    OEMCrypto_ERROR_NOT_IMPLEMENTED              = 25,
    OEMCrypto_ERROR_NO_CONTENT_KEY               = 26,
    OEMCrypto_ERROR_CONTROL_INVALID              = 27,
    OEMCrypto_ERROR_UNKNOWN_FAILURE              = 28,
    OEMCrypto_ERROR_INVALID_CONTEXT              = 29,
    OEMCrypto_ERROR_SIGNATURE_FAILURE            = 30,
    OEMCrypto_ERROR_TOO_MANY_SESSIONS            = 31,
    OEMCrypto_ERROR_INVALID_NONCE                = 32,
    OEMCrypto_ERROR_TOO_MANY_KEYS                = 33,
    OEMCrypto_ERROR_DEVICE_NOT_RSA_PROVISIONED   = 34,
    OEMCrypto_ERROR_INVALID_RSA_KEY              = 35,
    //API LEVEL9
    OEMCrypto_ERROR_KEY_EXPIRED                  = 36,
    OEMCrypto_ERROR_INSUFFICIENT_RESOURCES       = 37,
    OEMCrypto_ERROR_INSUFFICIENT_HDCP            = 38
} OEMCryptoResult;


typedef enum OEMCrypto_Algorithm {
    OEMCrypto_AES_CBC_128_NO_PADDING = 0,
    OEMCrypto_HMAC_SHA256            = 1,
} OEMCrypto_Algorithm;


typedef enum OEMCrypto_Usage_Entry_Status {
    kUnused = 0,
    kActive = 1,
    kInactive = 2
} OEMCrypto_Usage_Entry_Status;

typedef struct {
    uint8_t signature[20]; //  -- HMAC SHA1 of the rest of the report.
    uint8_t status;  // current status of entry. (OEMCrypto_Usage_Entry_Status)
    uint8_t clock_security_level;
    uint8_t pst_length;
  uint8_t padding;                         // make int64's word aligned.
  int64_t seconds_since_license_received; // now - time_of_license_received
  int64_t seconds_since_first_decrypt; // now - time_of_first_decrypt
  int64_t seconds_since_last_decrypt; //  now - time_of_last_decrypt
    uint8_t pst[];
} __attribute__((packed)) OEMCrypto_PST_Report;

/*
 *  OEMCrypto_Clock_Security_Level.
 *  Valid values for clock_security_level in OEMCrypto_PST_Report.
 */
typedef enum OEMCrypto_Clock_Security_Level {
    kInsecureClock = 0,
    kSecureTimer = 1,
    kSecureClock = 2,
    kHardwareSecureClock = 3
} OEMCrypto_Clock_Security_Level;


typedef enum RSA_Padding_Scheme {
    kSign_RSASSA_PSS     = 0x1, // RSASSA-PSS with SHA1.
    kSign_PKCS1_Block1   = 0x2, // PKCS1 with block type 1 padding (only).
} RSA_Padding_Scheme;

/*
 * Flags indicating data endpoints in OEMCrypto_DecryptCTR.
 */
#define OEMCrypto_FirstSubsample 1
#define OEMCrypto_LastSubsample  2


OEMCryptoResult OEMCrypto_Initialize(void);


OEMCryptoResult OEMCrypto_Terminate(void);

typedef struct OpenSessionArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION *session;
}OpenSessionArg;

OEMCryptoResult OEMCrypto_OpenSession(OEMCrypto_SESSION *session);

typedef struct CloseSessionArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
}CloseSessionArg;

OEMCryptoResult OEMCrypto_CloseSession(OEMCrypto_SESSION session);

typedef struct GenerateDerivedKeysArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t *mac_key_context;
    uint32_t mac_key_context_length;
    const uint8_t *enc_key_context;
    uint32_t enc_key_context_length;
}GenerateDerivedKeysArg;

OEMCryptoResult OEMCrypto_GenerateDerivedKeys(
                            OEMCrypto_SESSION session,
                            const uint8_t *mac_key_context,
                            uint32_t mac_key_context_length,
                            const uint8_t *enc_key_context,
                            uint32_t enc_key_context_length);

typedef struct GenerateNonceArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    uint32_t* nonce;
}GenerateNonceArg;

OEMCryptoResult OEMCrypto_GenerateNonce(
                            OEMCrypto_SESSION session,
                            uint32_t* nonce);


typedef struct GenerateSignatureArg{
    OEMCryptoResult *result ; 
    OEMCrypto_SESSION session;
    const uint8_t* message;
    size_t message_length;
    uint8_t* signature;
    size_t* signature_length;
}GenerateSignatureArg;

OEMCryptoResult OEMCrypto_GenerateSignature(
                            OEMCrypto_SESSION session,
                            const uint8_t* message,
                            size_t message_length,
                            uint8_t* signature,
                            size_t* signature_length);

typedef struct {
    const uint8_t* key_id;
    size_t         key_id_length;
    const uint8_t* key_data_iv;
    const uint8_t* key_data;
    size_t         key_data_length;
    const uint8_t* key_control_iv;
    const uint8_t* key_control;
} OEMCrypto_KeyObject;

typedef struct LoadKeysArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t* message;
    size_t message_length;
    const uint8_t* signature;
    size_t signature_length;
    const uint8_t* enc_mac_keys_iv;
    const uint8_t* enc_mac_keys;
    size_t num_keys;
    const OEMCrypto_KeyObject* key_array;
    const uint8_t* pst;
    size_t pst_length;
}LoadKeysArg;

OEMCryptoResult OEMCrypto_LoadKeys(OEMCrypto_SESSION session,
                                   const uint8_t* message,
                                   size_t message_length,
                                   const uint8_t* signature,
                                   size_t signature_length,
                                   const uint8_t* enc_mac_keys_iv,
                                   const uint8_t* enc_mac_keys,
                                   size_t num_keys,
        const OEMCrypto_KeyObject* key_array,
        const uint8_t* pst,
        size_t pst_length);

OEMCryptoResult OEMCrypto_LoadKeys_V8(OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        const uint8_t* signature,
        size_t signature_length,
        const uint8_t* enc_mac_keys_iv,
        const uint8_t* enc_mac_keys,
        size_t num_keys,
                                   const OEMCrypto_KeyObject* key_array);

typedef struct {
    const uint8_t* key_id;
    size_t key_id_length;
    const uint8_t* key_control_iv;
    const uint8_t* key_control;
} OEMCrypto_KeyRefreshObject;

typedef struct RefreshKeysArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t* message;
    size_t message_length;
    const uint8_t* signature;
    size_t signature_length;
    size_t num_keys;
    const OEMCrypto_KeyRefreshObject* key_array;
}RefreshKeysArg;

OEMCryptoResult OEMCrypto_RefreshKeys(OEMCrypto_SESSION session,
                      const uint8_t* message,
                      size_t message_length,
                      const uint8_t* signature,
                      size_t signature_length,
                      size_t num_keys,
                      const OEMCrypto_KeyRefreshObject* key_array);


typedef struct SelectKeyArg{
    OEMCryptoResult *result;
    OEMCrypto_SESSION session;
    const uint8_t* key_id;
    size_t key_id_length;
}SelectKeyArg;

OEMCryptoResult OEMCrypto_SelectKey(const OEMCrypto_SESSION session,
                                    const uint8_t* key_id,
                                    size_t key_id_length);

typedef enum OEMCryptoBufferType {
    OEMCrypto_BufferType_Clear,
    OEMCrypto_BufferType_Secure,
    OEMCrypto_BufferType_Direct
} OEMCryptoBufferType;

typedef struct OEMCrypto_DestBufferDesc{
    OEMCryptoBufferType type;
    union {
        struct {                   // type == OEMCrypto_BufferType_Clear
            uint8_t* address;
            size_t max_length;
        } clear;
        struct {                   // type == OEMCrypto_BufferType_Secure
            void* handle;
            size_t max_length;
            size_t offset;
        } secure;
        struct {                   // type == OEMCrypto_BufferType_Direct
            bool is_video;
        } direct;
    } buffer;

} OEMCrypto_DestBufferDesc;

typedef struct DecryptCTRArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t *data_addr;
    size_t data_length;
    bool is_encrypted;
    const uint8_t *iv;
    size_t block_offset;
    const OEMCrypto_DestBufferDesc* out_buffer;
    uint8_t subsample_flags;
}DecryptCTRArg;

OEMCryptoResult OEMCrypto_DecryptCTR(OEMCrypto_SESSION session,
                                     const uint8_t *data_addr,
                                     size_t data_length,
                                     bool is_encrypted,
                                     const uint8_t *iv,
                                     size_t block_offset,
                                     const OEMCrypto_DestBufferDesc* out_buffer,
                                     uint8_t subsample_flags);

typedef struct InstallKeyboxArg{
    OEMCryptoResult *result ;
    const uint8_t *keybox;
    size_t keyBoxLength;
}InstallKeyboxArg;

OEMCryptoResult OEMCrypto_InstallKeybox(const uint8_t *keybox,
                                        size_t keyBoxLength);


OEMCryptoResult OEMCrypto_IsKeyboxValid(void);


typedef struct GetDeviceIDArg{
    OEMCryptoResult *result ;
    uint8_t* deviceID;
    size_t *idLength;
}GetDeviceIDArg;

OEMCryptoResult OEMCrypto_GetDeviceID(uint8_t* deviceID,
                                      size_t *idLength);

typedef struct GetKeyDataArg{
    OEMCryptoResult *result ;
    uint8_t* keyData;
    size_t *keyDataLength;
}GetKeyDataArg;

OEMCryptoResult OEMCrypto_GetKeyData(uint8_t* keyData,
                                     size_t *keyDataLength);

typedef struct GetRandomArg{
    OEMCryptoResult *result ;
    uint8_t *randomData;
    size_t dataLength;
}GetRandomArg;

OEMCryptoResult OEMCrypto_GetRandom(uint8_t* randomData,
                                    size_t dataLength);

typedef struct WrapKeyboxArg{
    OEMCryptoResult *result ;
    const uint8_t *keybox;
    size_t keyBoxLength;
    uint8_t *wrappedKeybox;
    size_t *wrappedKeyBoxLength;
    const uint8_t *transportKey;
    size_t transportKeyLength;
}WrapKeyboxArg;

OEMCryptoResult OEMCrypto_WrapKeybox(const uint8_t *keybox,
                                     size_t keyBoxLength,
                                     uint8_t *wrappedKeybox,
                                     size_t *wrappedKeyBoxLength,
                                     const uint8_t *transportKey,
                                     size_t transportKeyLength);

typedef struct RewrapDeviceRSAKeyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* message;
    size_t message_length;
    const uint8_t* signature;
    size_t signature_length;
    const uint32_t *nonce;
    const uint8_t* enc_rsa_key;
    size_t enc_rsa_key_length;
    const uint8_t* enc_rsa_key_iv;
    uint8_t* wrapped_rsa_key;
    size_t *wrapped_rsa_key_length;
}RewrapDeviceRSAKeyArg;

OEMCryptoResult OEMCrypto_RewrapDeviceRSAKey(OEMCrypto_SESSION session,
                                             const uint8_t* message,
                                             size_t message_length,
                                             const uint8_t* signature,
                                             size_t signature_length,
                                             const uint32_t *nonce,
                                             const uint8_t* enc_rsa_key,
                                             size_t enc_rsa_key_length,
                                             const uint8_t* enc_rsa_key_iv,
                                             uint8_t* wrapped_rsa_key,
                                             size_t *wrapped_rsa_key_length);

typedef struct LoadDeviceRSAKeyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* wrapped_rsa_key;
    size_t wrapped_rsa_key_length;
}LoadDeviceRSAKeyArg;

OEMCryptoResult OEMCrypto_LoadDeviceRSAKey(OEMCrypto_SESSION session,
                                           const uint8_t* wrapped_rsa_key,
                                           size_t wrapped_rsa_key_length);

typedef struct GenerateRSASignatureArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* message;
    size_t message_length;
    uint8_t* signature;
    size_t *signature_length;
    RSA_Padding_Scheme padding_scheme;
}GenerateRSASignatureArg;

OEMCryptoResult OEMCrypto_GenerateRSASignature(OEMCrypto_SESSION session,
                                               const uint8_t* message,
                                               size_t message_length,
                                               uint8_t* signature,
        size_t *signature_length,
        RSA_Padding_Scheme padding_scheme);

OEMCryptoResult OEMCrypto_GenerateRSASignature_V8(OEMCrypto_SESSION session,
        const uint8_t* message,
        size_t message_length,
        uint8_t* signature,
                                               size_t *signature_length);

typedef struct DeriveKeysFromSessionKeyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* enc_session_key;
    size_t enc_session_key_length;
    const uint8_t *mac_key_context;
    size_t mac_key_context_length;
    const uint8_t *enc_key_context;
    size_t enc_key_context_length;
}DeriveKeysFromSessionKeyArg;

OEMCryptoResult OEMCrypto_DeriveKeysFromSessionKey(OEMCrypto_SESSION session,
                                                   const uint8_t* enc_session_key,
                                                   size_t enc_session_key_length,
                                                   const uint8_t *mac_key_context,
                                                   size_t mac_key_context_length,
                                                   const uint8_t *enc_key_context,
                                                   size_t enc_key_context_length);
typedef struct GenericEncryptArg{                                                   
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* in_buffer;
    size_t buffer_length;
    const uint8_t* iv;
    OEMCrypto_Algorithm algorithm;
    uint8_t* out_buffer;
}GenericEncryptArg;

OEMCryptoResult OEMCrypto_Generic_Encrypt(OEMCrypto_SESSION session,
                                          const uint8_t* in_buffer,
                                          size_t buffer_length,
                                          const uint8_t* iv,
                                          OEMCrypto_Algorithm algorithm,
                                          uint8_t* out_buffer);

typedef struct GenericDecryptArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* in_buffer;
    size_t buffer_length;
    const uint8_t* iv;
    OEMCrypto_Algorithm algorithm;
    uint8_t* out_buffer;
}GenericDecryptArg;

OEMCryptoResult OEMCrypto_Generic_Decrypt(OEMCrypto_SESSION session,
                                          const uint8_t* in_buffer,
                                          size_t buffer_length,
                                          const uint8_t* iv,
                                          OEMCrypto_Algorithm algorithm,
                                          uint8_t* out_buffer);
typedef struct GenericSignArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* in_buffer;
    size_t buffer_length;
    OEMCrypto_Algorithm algorithm;
    uint8_t* signature;
    size_t* signature_length;
}GenericSignArg;

OEMCryptoResult OEMCrypto_Generic_Sign(OEMCrypto_SESSION session,
                                       const uint8_t* in_buffer,
                                       size_t buffer_length,
                                       OEMCrypto_Algorithm algorithm,
                                       uint8_t* signature,
                                       size_t* signature_length);
typedef struct GenericVerifyArg{
    OEMCryptoResult *result ;
    OEMCrypto_SESSION session;
    const uint8_t* in_buffer;
    size_t buffer_length;
    OEMCrypto_Algorithm algorithm;
    const uint8_t* signature;
    size_t signature_length;
}GenericVerifyArg;

OEMCryptoResult OEMCrypto_Generic_Verify(OEMCrypto_SESSION session,
        const uint8_t* in_buffer,
        size_t buffer_length,
        OEMCrypto_Algorithm algorithm,
        const uint8_t* signature,
        size_t signature_length);

//API 9
//
//typedef uint8_t OEMCrypto_HDCP_Capability;
//
//OEMCryptoResult OEMCrypto_GetHDCPCapability(
//        OEMCrypto_HDCP_Capability *current,
//        OEMCrypto_HDCP_Capability *maximum);

bool OEMCrypto_SupportsUsageTable(void);

OEMCryptoResult OEMCrypto_UpdateUsageTable(void);


typedef struct DeactivateUsageEntryArg{
    OEMCryptoResult   *result;
    const uint8_t     *pst;
    size_t            pst_length;
}DeactivateUsageEntryArg;

OEMCryptoResult OEMCrypto_DeactivateUsageEntry(const uint8_t *pst, size_t pst_length);

typedef struct ReportUsageArg{
    OEMCryptoResult        *result;
    OEMCrypto_SESSION      session;
    const uint8_t          *pst;
    size_t                 pst_length;
    OEMCrypto_PST_Report   *buffer;
    size_t                 *buffer_length;
}ReportUsageArg;

OEMCryptoResult OEMCrypto_ReportUsage(OEMCrypto_SESSION session,
        const uint8_t *pst,
        size_t pst_length,
        OEMCrypto_PST_Report *buffer,
        size_t *buffer_length);

typedef struct DeleteUsageEntryArg{
    OEMCryptoResult         *result;
    OEMCrypto_SESSION       session;
    const uint8_t*          pst;
    size_t                  pst_length;
    const uint8_t           *message;
    size_t                  message_length;
    const uint8_t           *signature;
    size_t                  signature_length;
}DeleteUsageEntryArg;

OEMCryptoResult OEMCrypto_DeleteUsageEntry(OEMCrypto_SESSION session,
        const uint8_t* pst,
        size_t pst_length,
        const uint8_t *message,
        size_t message_length,
        const uint8_t *signature,
        size_t signature_length);

OEMCryptoResult OEMCrypto_DeleteUsageTable(void);

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

//Classical DRM

typedef struct SetEntitlementKeyArg{
    OEMCryptoResult *result ;
    const uint8_t* emmKey;
    uint32_t emmKeyLength;
}SetEntitlementKeyArg;

OEMCryptoResult OEMCrypto_SetEntitlementKey(const uint8_t* emmKey, uint32_t emmKeyLength);

typedef struct DeriveControlWordArg{
    OEMCryptoResult *result ;
    const uint8_t* ecm;
    uint32_t length;
    uint32_t* flags;
}DeriveControlWordArg;

OEMCryptoResult OEMCrypto_DeriveControlWord(const uint8_t* ecm,
        uint32_t length,
        uint32_t* flags);

typedef struct DecryptVideoArg{
    OEMCryptoResult *result;
    uint8_t* iv;
    const uint8_t* input;
    uint32_t inputLength;
    uint32_t output_handle;
    uint32_t output_offset;
    uint32_t *outputLength;
}DecryptVideoArg;

OEMCryptoResult OEMCrypto_DecryptVideo(uint8_t* iv,
        const uint8_t* input,
        uint32_t inputLength,
        uint32_t output_handle,
        uint32_t output_offset,
        uint32_t *outputLength);

typedef struct DecryptAudioArg{
    OEMCryptoResult *result ;
    uint8_t* iv;
    const uint8_t* input;
    uint32_t inputLength;
    uint8_t *output;
    uint32_t *outputLength;
}DecryptAudioArg;

OEMCryptoResult OEMCrypto_DecryptAudio(uint8_t* iv,
        const uint8_t* input,
        uint32_t inputLength,
        uint8_t *output,
        uint32_t *outputLength);

typedef struct RpmbLoadKeyArg{
    uint32_t *result;
    uint8_t *key;
    uint32_t keySize;        
}RpmbLoadKeyArg;

typedef struct RpmGetWriteCntReqArg{
    uint32_t *result;
    RPMB_DATAFRAM *dFrm;
}RpmGetWriteCntReqArg;

typedef struct RpmSetWriteCntRespArg{
    uint32_t *result;
    RPMB_DATAFRAM *dFrm;
}RpmSetWriteCntRespArg;

#endif  // OEMCRYPTO_CENC_H_
