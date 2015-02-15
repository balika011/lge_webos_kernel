/*********************************************************************
 * OEMCryptoCENC.h
 *
 * (c) Copyright 2013 Google, Inc.
 *
 * Reference APIs needed to support Widevine's crypto algorithms.
 *********************************************************************/

#ifndef OEMCRYPTO_CENC_H_

#define OEMCRYPTO_CENC_H_

#ifndef WIDEVINE_OEM_API_LEVEL
#define WIDEVINE_OEM_API_LEVEL 9
#endif


#define SHA256_DIGEST_LENGTH    32

#define  KEY_CONTROL_SIZE   16
#define  KEY_IV_SIZE        16
#define  KEY_PAD_SIZE       16
#define  KEY_SIZE           16
#define  MAC_KEY_SIZE       32

//===================================
// WV Level 1 Wrapper
//===================================

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
    OEMCrypto_ERROR_KEY_EXPIRED                  = 36,
    OEMCrypto_ERROR_INSUFFICIENT_RESOURCES       = 37,
    OEMCrypto_ERROR_INSUFFICIENT_HDCP            = 38,
} OEMCryptoResult;


typedef enum OEMCrypto_Algorithm {
    OEMCrypto_AES_CBC_128_NO_PADDING = 0,
    OEMCrypto_HMAC_SHA256            = 1,
} OEMCrypto_Algorithm;

/*
 * Flags indicating data endpoints in OEMCrypto_DecryptCTR.
 */
#define OEMCrypto_FirstSubsample 1
#define OEMCrypto_LastSubsample  2

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

OEMCryptoResult OEMCrypto_Initialize(void);

OEMCryptoResult OEMCrypto_Terminate(void);

OEMCryptoResult OEMCrypto_OpenSession(OEMCrypto_SESSION *session);

OEMCryptoResult OEMCrypto_CloseSession(OEMCrypto_SESSION session);

OEMCryptoResult OEMCrypto_GenerateDerivedKeys(
                            OEMCrypto_SESSION session,
                            const uint8_t *mac_key_context,
                            uint32_t mac_key_context_length,
                            const uint8_t *enc_key_context,
                            uint32_t enc_key_context_length);

OEMCryptoResult OEMCrypto_GenerateNonce(
                            OEMCrypto_SESSION session,
                            uint32_t* nonce);

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
        size_t pst_length);

typedef struct {
    const uint8_t* key_id;
    size_t key_id_length;
    const uint8_t* key_control_iv;
    const uint8_t* key_control;
} OEMCrypto_KeyRefreshObject;

OEMCryptoResult OEMCrypto_RefreshKeys(OEMCrypto_SESSION session,
                      const uint8_t* message,
                      size_t message_length,
                      const uint8_t* signature,
                      size_t signature_length,
                      size_t num_keys,
                      const OEMCrypto_KeyRefreshObject* key_array);

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

OEMCryptoResult OEMCrypto_DecryptCTR(OEMCrypto_SESSION session,
                                     const uint8_t *data_addr,
                                     size_t data_length,
                                     bool is_encrypted,
                                     const uint8_t *iv,
                                     size_t block_offset,
                                     const OEMCrypto_DestBufferDesc* out_buffer,
                                     uint8_t subsample_flags);

OEMCryptoResult OEMCrypto_InstallKeybox(const uint8_t *keybox,
                                        size_t keyBoxLength);

OEMCryptoResult OEMCrypto_IsKeyboxValid(void);

OEMCryptoResult OEMCrypto_GetDeviceID(uint8_t* deviceID,
                                      size_t *idLength);

OEMCryptoResult OEMCrypto_GetKeyData(uint8_t* keyData,
                                     size_t *keyDataLength);

OEMCryptoResult OEMCrypto_GetRandom(uint8_t* randomData,
                                    size_t dataLength);

OEMCryptoResult OEMCrypto_WrapKeybox(const uint8_t *keybox,
                                     size_t keyBoxLength,
                                     uint8_t *wrappedKeybox,
                                     size_t *wrappedKeyBoxLength,
                                     const uint8_t *transportKey,
                                     size_t transportKeyLength);

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

OEMCryptoResult OEMCrypto_LoadDeviceRSAKey(OEMCrypto_SESSION session,
                                           const uint8_t* wrapped_rsa_key,
                                           size_t wrapped_rsa_key_length);

OEMCryptoResult OEMCrypto_GenerateRSASignature(OEMCrypto_SESSION session,
                                               const uint8_t* message,
                                               size_t message_length,
                                               uint8_t* signature,
                                               size_t *signature_length,
                                               RSA_Padding_Scheme padding_scheme);
                                               

OEMCryptoResult OEMCrypto_DeriveKeysFromSessionKey(OEMCrypto_SESSION session,
                                                   const uint8_t* enc_session_key,
                                                   size_t enc_session_key_length,
                                                   const uint8_t *mac_key_context,
                                                   size_t mac_key_context_length,
                                                   const uint8_t *enc_key_context,
                                                   size_t enc_key_context_length);

OEMCryptoResult OEMCrypto_Generic_Encrypt(OEMCrypto_SESSION session,
                                          const uint8_t* in_buffer,
                                          size_t buffer_length,
                                          const uint8_t* iv,
                                          OEMCrypto_Algorithm algorithm,
                                          uint8_t* out_buffer);

OEMCryptoResult OEMCrypto_Generic_Decrypt(OEMCrypto_SESSION session,
                                          const uint8_t* in_buffer,
                                          size_t buffer_length,
                                          const uint8_t* iv,
                                          OEMCrypto_Algorithm algorithm,
                                          uint8_t* out_buffer);

OEMCryptoResult OEMCrypto_Generic_Sign(OEMCrypto_SESSION session,
                                       const uint8_t* in_buffer,
                                       size_t buffer_length,
                                       OEMCrypto_Algorithm algorithm,
                                       uint8_t* signature,
                                       size_t* signature_length);

OEMCryptoResult OEMCrypto_Generic_Verify(OEMCrypto_SESSION session,
                                         const uint8_t* in_buffer,
                                         size_t buffer_length,
                                         OEMCrypto_Algorithm algorithm,
                                         const uint8_t* signature,
                                         size_t signature_length);


OEMCryptoResult OEMCrypto_UpdateUsageTable(void);

OEMCryptoResult OEMCrypto_DeactivateUsageEntry(const uint8_t *pst,
        size_t pst_length);


OEMCryptoResult OEMCrypto_ReportUsage(OEMCrypto_SESSION session,
        const uint8_t *pst,
        size_t pst_length,
        OEMCrypto_PST_Report *buffer,
        size_t *buffer_length);

OEMCryptoResult OEMCrypto_DeleteUsageEntry(OEMCrypto_SESSION session,
        const uint8_t* pst,
        size_t pst_length,
        const uint8_t *message,
        size_t message_length,
        const uint8_t *signature,
        size_t signature_length); 

OEMCryptoResult OEMCrypto_DeleteUsageTable(void) ;

struct RPMB_DATAFRAM;

OEMCryptoResult OEMCrypto_GetUsageTableWriteRequest(
        uint8_t* buffer,
        size_t * buffer_length,
        struct RPMB_DATAFRAM *dFrame);

OEMCryptoResult OEMCrypto_VerifyWriteResponse(const struct RPMB_DATAFRAM *dFrame);

OEMCryptoResult OEMCrypto_GetUsageTableReadRequest(struct RPMB_DATAFRAM *dFrame);

OEMCryptoResult OEMCrypto_VerifyReadResponse(
        const uint8_t* buffer,
        size_t  buffer_length,
        const struct RPMB_DATAFRAM *dFrame);

OEMCryptoResult OEMCrypto_SetEntitlementKey(const uint8_t *emmKey , 
        uint32_t emmKeyLength);

OEMCryptoResult OEMCrypto_DeriveControlWord(const uint8_t *ecm,
        uint32_t length,
        uint32_t *flags);

OEMCryptoResult OEMCrypto_DecryptVideo(uint8_t *iv,
        uint8_t *input,
        uint32_t inputLength,
        uint32_t output_handle,
        uint32_t output_offset,
        uint32_t *outputLength);

OEMCryptoResult OEMCrypto_DecryptAudio(uint8_t *iv,
        uint8_t *input,
        uint32_t inputLength,
        uint8_t *output,
        uint32_t *outputLength);

#endif  // OEMCRYPTO_CENC_H_
