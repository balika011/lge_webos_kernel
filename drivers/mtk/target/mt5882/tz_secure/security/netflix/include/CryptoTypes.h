/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef CRYPTOTYPES_H_
#define CRYPTOTYPES_H_

#include "CommTypes.h"

/*
 * Crypto operation definitions: all TEE operations are represented
 * as a 32-bit value. Each module (like cryptoAgent) has its own pool
 * of values that start at a pre-defined offset. These offsets are
 * defined in CommTypes.h. Following are the currently defined crypto
 * operations.
 */
typedef enum
{
	nfTO_CRYPTO_OP_UNWRAP_KEY = nfTOB_CRYPTO_AGENT + 1,
	nfTO_CRYPTO_OP_EXPORT_SEALED_KEY,
	nfTO_CRYPTO_OP_IMPORT_SEALED_KEY,
	nfTO_CRYPTO_OP_DELETE_KEY,
	nfTO_CRYPTO_OP_GET_KEY_INFO,
	nfTO_CRYPTO_OP_GET_NAMED_KEY_HANDLE,
	nfTO_CRYPTO_OP_GET_ESN,
	nfTO_CRYPTO_OP_AES_CBC_ENCRYPT, // atomic encrypt
	nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_INIT,
	nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_UPDATE,
	nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_FINAL,
	nfTO_CRYPTO_OP_AES_CBC_DECRYPT, // atomic decrypt
	nfTO_CRYPTO_OP_AES_CBC_DECRYPT_INIT,
	nfTO_CRYPTO_OP_AES_CBC_DECRYPT_UPDATE,
	nfTO_CRYPTO_OP_AES_CBC_DECRYPT_FINAL,
	nfTO_CRYPTO_OP_EXPORT_KEY,
	nfTO_CRYPTO_OP_IMPORT_KEY,
	nfTO_CRYPTO_OP_HMAC_SHA256, // atomic hmac_sha256
	nfTO_CRYPTO_OP_HMAC_SHA256_INIT,
	nfTO_CRYPTO_OP_HMAC_SHA256_UPDATE,
	nfTO_CRYPTO_OP_HMAC_SHA256_FINAL,
	nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY, // atomic hmac_sha256_verify
	nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_INIT,
	nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_UPDATE,
	nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_FINAL,
	nfTO_CRYPTO_OP_DH_GEN_KEYS,
	nfTO_CRYPTO_OP_NFLX_DH_DERIVE,
	nfTO_CRYPTO_OP_CLEARKEYS,

    // insert new message types here...


    nfTO_CRYPTO_OP_INVALID // leave this last!!
} nfTeeOperation_CryptoOp_t;

// we provide a macro that the dispatcher can use to identify valid crypto ops
#define nfTEE_IS_CRYPTO_OP(x) (((x) > nfTOB_CRYPTO_AGENT) && ((x) < nfTO_CRYPTO_OP_INVALID))

typedef uint32_t nfCryptoKeyHandle_t;

// various types used in API messages
typedef enum
{
	nfKF_UNDEFINED = 0,
	nfKF_RAW,    //< An unformatted sequence of bytes. Intended for secret keys.
	nfKF_JWK,    //< The key is represented as JSON according to the JSON Web Key format.

	// leave this last!
	nfKF_INVALID
} nfKeyFormat_t;

typedef enum
{
	nfCA_UNDEFINED = 0,
	nfCA_HMAC,
	nfCA_AES_CBC,
	nfCA_AES_GCM,
	nfCA_AES_CTR,
	nfCA_RSAES_PKCS1_V1_5,
	nfCA_RSASSA_PKCS1_V1_5,
	nfCA_RSA_OAEP,
	nfCA_SHA1,
	nfCA_SHA224,
	nfCA_SHA256,
	nfCA_SHA384,
	nfCA_SHA512,
	nfCA_AES_KW,
	nfCA_DH_ALGO,
	nfCA_SYSTEM, // XXX - what is this for??
	nfCA_NFLX_DH,

	// leave this last!
	nfCA_INVALID
} nfCryptoAlgorithm_t;

// we provide a macro that the dispatcher can use to identify if algorithm is HMAC, RSA, AES etc
#define nfIS_HMAC_ALG(x) (((x) == nfCA_HMAC))
#define nfIS_AES_ALG(x) (((x) == nfCA_AES_CBC)||((x) == nfCA_AES_GCM)||((x) == nfCA_AES_CTR)||((x)==nfCA_AES_KW))
#define nfIS_RSA_ALG(x) (((x) == nfCA_RSAES_PKCS1_V1_5)||((x) == nfCA_RSASSA_PKCS1_V1_5)||((x) == nfCA_RSA_OAEP))

typedef enum
{
	nfKT_UNDEFINED = 0,
    nfKT_SECRET,
    nfKT_PUBLIC,
    nfKT_PRIVATE,

    // leave this last!
    nfKT_INVALID
} nfKeyType_t;

typedef enum
{
	nfKUF_UNDEFINED = 0x00,

    nfKUF_ENCRYPT = 0x01,
    nfKUF_DECRYPT = 0x02,
    nfKUF_SIGN = 0x04,
    nfKUF_VERIFY = 0x08,
    nfKUF_DERIVE = 0x10,
    nfKUF_WRAP = 0x20,
    nfKUF_UNWRAP = 0x40,

    // XXX - work with MarkW on this for webcrypto
    nfKUF_EXTRACTABLE= 0x80,
} nfKeyUsageFlag_t;

typedef uint32_t nfKeyUsageFlagsWord_t;

#define nfTEE_AES_128_KEY_SIZE_BYTES	16
#define nfTEE_AES_128_KEY_SIZE_BITS		(nfTEE_AES_128_KEY_SIZE_BYTES * 8)
#define nfMIN_ENCODED_KEYDATA_SIZE		nfTEE_AES_128_KEY_SIZE_BYTES

/*
 *  Crypto command message definitions
 *
 *  Two things to keep in mind here: (1) we want to
 *  minimize communications overhead, and (2) word alignment
 *  is important.
 *
 *  For overhead, we can reduce this by packing some fields. For
 *  example, a number of fields are defined as enums (e.g. int32),
 *  but there are far less than 255 possible values. For those, we
 *  can use a single byte in messages.
 */


// delete key request
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
} nfTeeDeleteKeyRequest_t;

#define nfMIN_DELETEKEY_REQ_SIZE sizeof(nfTeeDeleteKeyRequest_t)

// delete key response
#if 0
typedef struct
{
	// do we need this?
} nfTeeDeleteKeyResponse_t;
#endif

// get key info request
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
} nfTeeGetKeyInfoRequest_t;

#define nfMIN_GETKEYINFO_REQ_SIZE sizeof(nfTeeGetKeyInfoRequest_t)

// get key info response
typedef struct
{
	nfKeyType_t				KeyType;
	nfCryptoAlgorithm_t		Algorithm;
	nfKeyUsageFlagsWord_t	KeyUsageFlags;
} nfTeeGetKeyInfoResponse_t;

#define nfMIN_GETKEYINFO_RSP_SIZE sizeof(nfTeeGetKeyInfoResponse_t)

// TBD: should we have a shared REE/TEE constants file?
#define nfMAX_ESN_SIZE	42

// get esn response
typedef struct
{
	uint32_t	EsnLength;
	uint8_t		Esn[nfMAX_ESN_SIZE];
} nfTeeGetEsnResponse_t;

#define nfMAX_ESN_RESPONSE_SIZE sizeof(nfTeeGetEsnResponse_t)

/*
 * Diffie-Hellman definitions
 */
/*
 * We'll define a max generator length of 4, but we'll
 * usually represent it as a uint32_t.
 */
#define nfMAX_DH_GENERATOR_LENGTH 	4 /* 32 bits */

// we impose an artificial maximum for our use case
#define nfMAX_DH_GENERATOR			32

// NB: min modulus length we support is 1024 bits
#define nfMIN_DH_MODULUS_LENGTH 	128 /* 1024 bits */
/*
 * The max supported modulus length is currently 1024 bits,
 * but the Netflix app includes a 0x00 byte at the beginning to
 * address signed-ness bugs in some implementations, so we have
 * to support 1032 bits.
 */
#define nfMAX_DH_MODULUS_LENGTH 	129 /* 1032 bits */

#define nfMAX_DH_PRIVKEY_LENGTH		128 /* 1024 bits */
#define nfMIN_DH_PRIVKEY_LENGTH		128 /* 1024 bits */

// generate key request
typedef struct
{
	nfCryptoAlgorithm_t		Algorithm; /* currently, must be nfCA_NFLX_DH */
	nfKeyUsageFlagsWord_t	KeyUsageFlags;	// not sure if we need these, currently ignored
	uint32_t				Generator; // NB: generator must be <= 0xFFFFFFFF
	//uint8_t					RandomKey[nfMAX_DH_PRIVKEY_LENGTH];
	uint32_t				ModulusLength;
	uint8_t					Modulus[/*ModulusLength*/];
} nfTeeGenerateDHKeyRequest_t;

#define nfMIN_GENDH_REQ_SIZE (sizeof(nfTeeGenerateDHKeyRequest_t)  + nfMIN_DH_MODULUS_LENGTH)

/*
 * XXX - should we include key info with each key handle
 * in the DH responses? If the application needs it, it can
 * always send a getKeyInfo message for each handle. For now,
 * we'll leave it out.
 */
// generate key response
typedef struct
{
	nfCryptoKeyHandle_t	DHPrivKeyHandle;
	nfCryptoKeyHandle_t	DHPubKeyHandle;
} nfTeeGenerateDHKeyResponse_t;

#define nfMIN_GENDH_RSP_SIZE (sizeof(nfTeeGenerateDHKeyResponse_t) + nfMAX_DH_MODULUS_LENGTH)

// derive key request
typedef struct
{
	nfCryptoKeyHandle_t	DHPrivKeyHandle;
	nfCryptoKeyHandle_t	KeyDerivationKeyHandle;
	uint32_t			PeerDHPubValueLength;
	uint8_t				PeerDHPubValue[/*PeerDHPubValueLength*/];
} nfTeeNflxDhDeriveKeyRequest_t;

#define nfMIN_NFLX_DHDERIVE_REQ_SIZE (sizeof(nfTeeNflxDhDeriveKeyRequest_t) + nfMIN_DH_MODULUS_LENGTH)

// derive key response
typedef struct
{
	nfCryptoKeyHandle_t	KceHandle; // encryption key
	nfCryptoKeyHandle_t	KchHandle; // hmac key
	nfCryptoKeyHandle_t	KcdHandle; // key derivation key
} nfTeeNflxDhDeriveKeyResponse_t;

#define nfMIN_NFLX_DHDERIVE_RSP_SIZE sizeof(nfTeeNflxDhDeriveKeyResponse_t)

/*
 * Delete DH private key (and associated context). Note that
 * we don't define a delete response; we just use the response
 * code in nfTeeApiRspHdr_t to convey the result.
 */
typedef struct
{
	nfCryptoKeyHandle_t	DHPrivKeyHandle;
} nfTeeDeleteDHKeyRequest_t;

#define nfMIN__DHDELETE_REQ_SIZE sizeof(nfTeeDeleteKeyRequest_t)

/*
 * Known, pre-provisioned (i.e. named) keys
 */
#define nfTEE_PSK_KPE_NAME	"DKE"
#define nfTEE_PSK_KPE_NAME_LEN	3
#define nfTEE_PSK_KPH_NAME	"DKH"
#define nfTEE_PSK_KPH_NAME_LEN	3
#define nfTEE_PSK_KPW_NAME	"DKW"
#define nfTEE_PSK_KPW_NAME_LEN	3
#if 0 // deprecated
#define nfTEE_ABKP_NAME	"ABKP"
#define nfTEE_ABKP_NAME_LEN	4
#endif

#define nfTEE_CRYPTO_MAX_KEYNAME_SIZE	16
#define nfMIN_GET_NAMEDKEY_REQ_SIZE (sizeof(uint32_t) + 1)
// get named key handle request
typedef struct
{
	uint32_t	KeyNameLength;
	uint8_t	    KeyName[];
} nfTeeGetNamedKeyHandleRequest_t;

// get named key handle response
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
} nfTeeGetNamedKeyHandleResponse_t;

#define nfMIN_GET_NAMEDKEY_RSP_SIZE sizeof(nfTeeGetNamedKeyHandleResponse_t)

//import key request
typedef struct
{
	nfCryptoAlgorithm_t		Algorithm;
	nfKeyUsageFlagsWord_t	KeyUsageFlags;
	nfKeyFormat_t			KeyFormat;
	uint32_t				DataLength;
	uint8_t	    			KeyData[/*DataLength*/];
} nfTeeImportKeyRequest_t;

#define nfMIN_IMPORT_KEY_REQ_SIZE (sizeof(nfTeeImportKeyRequest_t) + nfMIN_ENCODED_KEYDATA_SIZE)

// import key response
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
	nfKeyType_t			KeyType;
} nfTeeImportKeyResponse_t;

#define nfMIN_IMPORT_KEY_RSP_SIZE sizeof(nfTeeImportKeyResponse_t)

//export key request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyFormat_t			KeyFormat;
} nfTeeExportKeyRequest_t;

#define nfMIN_EXPORT_KEY_REQ_SIZE sizeof(nfTeeExportKeyRequest_t)

// export key response
typedef struct
{
	uint32_t				DataLength;
	uint8_t	    			KeyData[/*DataLength*/];
} nfTeeExportKeyResponse_t;

#define nfMIN_EXPORT_KEY_RSP_SIZE (sizeof(nfTeeExportKeyResponse_t) + nfMIN_ENCODED_KEYDATA_SIZE)

/*
 * Sealed key import/export definitions
 */

#define nfMIN_ENCODED_SEALED_KEYDATA_SIZE	76 // see KeyMgmt.c for nfSealedKeyHeader_t definitions

//import sealed key request
typedef struct
{
	uint32_t				DataLength;
	uint8_t	    			KeyData[/*DataLength*/];
} nfTeeImportSealedKeyRequest_t;

#define nfMIN_IMPORT_SEALED_KEY_REQ_SIZE (sizeof(nfTeeImportSealedKeyRequest_t) + nfMIN_ENCODED_SEALED_KEYDATA_SIZE)

// import key response
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t				KeyType;
	nfCryptoAlgorithm_t		Algorithm;
    nfKeyUsageFlagsWord_t 	KeyUsageFlags;
    // TBD: should we be using bits for key lengths? Keylen % 8 == 0 always right now...
	uint32_t				KeyBitCount; // how many bits in this key?
} nfTeeImportSealedKeyResponse_t;

#define nfMIN_IMPORT_SEALED_KEY_RSP_SIZE (sizeof(nfTeeImportSealedKeyResponse_t))

//export sealed key request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
} nfTeeExportSealedKeyRequest_t;

#define nfMIN_EXPORT_SEALED_KEY_REQ_SIZE (sizeof(nfTeeExportSealedKeyRequest_t))

// export sealed key response
typedef struct
{
	uint32_t				DataLength;
	uint8_t	    			SealedKeyData[/*DataLength*/];
} nfTeeExportSealedKeyResponse_t;

#define nfMIN_EXPORT_SEALED_KEY_RSP_SIZE (sizeof(nfTeeExportSealedKeyResponse_t) + nfMIN_ENCODED_SEALED_KEYDATA_SIZE)

// AES definitions
#define nfTEE_AES_IV_MIN	16 // minimum AES IV size in bytes
#define nfTEE_AES_IV_MAX	32 // maximum AES IV size in bytes

#define nfTEE_AES_BLOCK_MIN	16 // minimum AES block size we support (128 bit)

// aes cbc encrypt request
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
	uint8_t	    		IV[nfTEE_AES_IV_MAX];
	uint32_t			DataLength;
	uint8_t	    		Data[];
} nfTeeAesCbcEncryptRequest_t;

#define nfMIN_AES_CBC_ENCR_REQ_SIZE (sizeof(nfTeeAesCbcEncryptRequest_t))

// aes cbc encrypt response
typedef struct
{
	uint32_t	DataLength;
	uint8_t	    EncryptedData[];
} nfTeeAesCbcEncryptResponse_t;

#define nfMIN_AES_CBC_ENCR_RSP_SIZE (sizeof(nfTeeAesCbcEncryptResponse_t) + nfTEE_AES_BLOCK_MIN)

// aes cbc decrypt request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	uint8_t	    			IV[nfTEE_AES_IV_MAX];
	uint32_t				DataLength;
	uint8_t	    			EncryptedData[];
} nfTeeAesCbcDecryptRequest_t;

// aes cbc decrypt response
typedef struct
{
	uint32_t	DataLength;
	uint8_t	    DecryptedData[];
} nfTeeAesCbcDecryptResponse_t;

#define nfMIN_AES_CBC_DECRYPT_REQ_SIZE (sizeof(nfTeeAesCbcDecryptRequest_t) + nfTEE_AES_IV_MIN)
#define nfMIN_AES_CBC_DECRYPT_RSP_SIZE (sizeof(nfTeeAesCbcDecryptResponse_t))

// GCM definitions
#define nfTEE_GCM_IV_MAX	64 // maximum GCM IV size in bytes
#define nfTEE_GCM_AAD_MAX	64 // maximum GCM AAD size in bytes
#define nfTEE_GCM_TAG_MAX	16 // maximum GCM TAG size in bytes

// aes gcm encrypt request
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
	uint16_t			IVLength; // actual length of IV
	uint16_t			AADLength; // actual length of AAD
	uint8_t	    		IV[nfTEE_GCM_IV_MAX];
	uint8_t	    		AAD[nfTEE_GCM_AAD_MAX];
	uint32_t			DataLength;
	uint8_t	    		Data[];
} nfTeeAesGcmEncryptRequest_t;

// aes gcm encrypt response
typedef struct
{
	uint32_t	TagLength;
	uint8_t	    Tag[nfTEE_GCM_TAG_MAX]; // make sure nfTEE_GCM_TAG_MAX is always word-aligned
	uint32_t	DataLength;
	uint8_t	    EncryptedData[];
} nfTeeAesGcmEncryptResponse_t;

// aes gcm decrypt request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	uint16_t				IVLength; // actual length of IV
	uint16_t				AADLength; // actual length of AAD
	uint8_t	    			IV[nfTEE_GCM_IV_MAX];
	uint8_t	    			AAD[nfTEE_GCM_AAD_MAX];
	uint32_t				DataLength;
	uint8_t	    			EncryptedData[];
} nfTeeAesGcmDecryptRequest_t;

// aes gcm decrypt response
typedef struct
{
	uint32_t	DataLength;
	uint8_t	    DecryptedData[];
} nfTeeAesGcmDecryptResponse_t;


/*
 * Following are definitions relating to incremental crypto
 * operations (init/update/final) for AES/HMAC-SHA
 */

// REE has no idea what handle values mean, treat as opaque data structure
#define nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE	20
typedef struct
{
	uint8_t		Handle[nfTEE_CRYPTO_CONTEXT_HANDLE_SIZE];
} nfTeeCryptoContextHandle_t;

// aes cbc encrypt-init request
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
	uint8_t	    		IV[nfTEE_AES_IV_MAX];
} nfTeeAesCbcEncryptInitRequest_t;

#define nfMIN_AES_CBC_ENCRYPT_INIT_REQ_SIZE sizeof(nfTeeAesCbcEncryptInitRequest_t)

typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeAesCbcEncryptInitResponse_t;

#define nfMIN_AES_CBC_ENCRYPT_INIT_RSP_SIZE sizeof(nfTeeAesCbcEncryptInitResponse_t)

// aes cbc encrypt-update request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	uint32_t					DataLength;
	uint8_t	    				Data[];
} nfTeeAesCbcEncryptUpdateRequest_t;

#define nfMIN_AES_CBC_ENCRYPT_UPDATE_REQ_SIZE sizeof(nfTeeAesCbcEncryptUpdateRequest_t)

// aes cbc encrypt update response
typedef struct
{
	uint32_t	EncryptedDataLength;
	uint8_t	    EncryptedData[];
} nfTeeAesCbcEncryptUpdateResponse_t;

#define nfMIN_AES_CBC_ENCRYPT_UPDATE_RSP_SIZE sizeof(nfTeeAesCbcEncryptUpdateResponse_t)

// aes cbc encrypt-final request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeAesCbcEncryptFinalRequest_t;

#define nfMIN_AES_CBC_ENCRYPT_FINAL_REQ_SIZE sizeof(nfTeeAesCbcEncryptFinalRequest_t)

// aes cbc encrypt final response
typedef struct
{
	uint32_t	EncryptedDataLength;
	uint8_t	    EncryptedData[];
} nfTeeAesCbcEncryptFinalResponse_t;

#define nfMIN_AES_CBC_ENCRYPT_FINAL_RSP_SIZE sizeof(nfTeeAesCbcEncryptFinalResponse_t)

// INCREMENTAL DECRYPT messages

// aes cbc decrypt-init request
typedef struct
{
	nfCryptoKeyHandle_t	KeyHandle;
	uint8_t	    		IV[nfTEE_AES_IV_MAX];
} nfTeeAesCbcDecryptInitRequest_t;

#define nfMIN_AES_CBC_DECRYPT_INIT_REQ_SIZE sizeof(nfTeeAesCbcDecryptInitRequest_t)

typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeAesCbcDecryptInitResponse_t;

#define nfMIN_AES_CBC_DECRYPT_INIT_RSP_SIZE sizeof(nfTeeAesCbcDecryptInitResponse_t)

// aes cbc decrypt-update request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	uint32_t					EncryptedDataLength;
	uint8_t	    				EncryptedData[];
} nfTeeAesCbcDecryptUpdateRequest_t;

#define nfMIN_AES_CBC_DECRYPT_UPDATE_REQ_SIZE sizeof(nfTeeAesCbcEncryptUpdateRequest_t)

// aes cbc decrypt update response
typedef struct
{
	uint32_t	DataLength;
	uint8_t	    Data[];
} nfTeeAesCbcDecryptUpdateResponse_t;

#define nfMIN_AES_CBC_DECRYPT_UPDATE_RSP_SIZE sizeof(nfTeeAesCbcDecryptUpdateResponse_t)

// aes cbc decrypt-final request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeAesCbcDecryptFinalRequest_t;

#define nfMIN_AES_CBC_DECRYPT_FINAL_REQ_SIZE sizeof(nfTeeAesCbcDecryptFinalRequest_t)

// aes cbc decrypt final response
typedef struct
{
	uint32_t	DataLength;
	uint8_t	    Data[];
} nfTeeAesCbcDecryptFinalResponse_t;

#define nfMIN_AES_CBC_DECRYPT_FINAL_RSP_SIZE sizeof(nfTeeAesCbcDecryptFinalResponse_t)




// content master key maximum for JWE/JWK
#define nfMAX_CMK_LEN	32

// wrap/unwrap definitions
#define nfTEE_DECODED_JWE_ALG_MAX	8 	// maximum JWE  IV size in bytes
#define nfTEE_DECODED_JWE_ENC_MAX	8   // maximum GCM AAD size in bytes
#define nfTEE_DECODED_JWE_CMK_MAX	32  // maximum GCM TAG size in bytes
#define nfTEE_DECODED_JWE_MAC_MAX	16

// unwrap request
typedef struct
{
	nfCryptoKeyHandle_t		WrappingKeyHandle;
	nfCryptoAlgorithm_t		UnwrapAlgorithm;
	nfCryptoAlgorithm_t		UnwrappedKeyAlgorithm;
	nfKeyUsageFlagsWord_t	KeyUsageFlags; // contains one or more nfKeyUsageFlag_t
	uint32_t	    		AlgLength;
	uint8_t	    			AlgData[nfTEE_DECODED_JWE_ALG_MAX];
	uint32_t	    		EncLength;
	uint8_t	    			EncData[nfTEE_DECODED_JWE_ENC_MAX];
	uint32_t	    		CmkLength;
	uint8_t	    			CmkData[nfTEE_DECODED_JWE_CMK_MAX];
	uint32_t	    		IvLength;
	uint8_t	    			IvData[nfTEE_GCM_IV_MAX];
	uint32_t	    		MacLength;
	uint8_t	    			MacData[nfTEE_DECODED_JWE_MAC_MAX];
	uint32_t	    		CipherLength;
	uint8_t	    			CipherData[/*CipherLength*/];
} nfTeeUnwrapKeyRequest_t;

#define nfMIN_UNWRAP_REQ_SIZE (sizeof(nfTeeUnwrapKeyRequest_t) + nfMIN_ENCODED_KEYDATA_SIZE)

// pick sane upper-bound for JWK and JWE
#define nfMAX_JWK_LENGTH	4096
#define nfMAX_JWE_LENGTH	(nfMAX_JWK_LENGTH + 1024)	// TBD - need a high-quality value here

// unwrap response
typedef struct
{
	nfCryptoKeyHandle_t    KeyHandle;
} nfTeeUnwrapKeyResponse_t;

#define nfMIN_UNWRAP_RSP_SIZE sizeof(nfTeeUnwrapKeyResponse_t)

// HMAC SHA256 request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	uint32_t				DataLength;
	uint8_t	    			Data[];
} nfTeeHmacSha256Request_t;

#define nfHMAC_SHA256_OUTPUT_SIZE	32 // size of SHA256 output
#define nfTEE_HMAC_SHA256_KEY_SIZE_BYTES	32

#define nfMIN_HMACSHA256_REQ_SIZE (sizeof(nfTeeHmacSha256Request_t) + 1)

// HMAC SHA256 response
typedef struct
{
	uint8_t	    HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
} nfTeeHmacSha256Response_t;

// HMAC SHA256 init request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
} nfTeeHmacSha256InitRequest_t;

#define nfMIN_HMACSHA256_INIT_REQ_SIZE (sizeof(nfTeeHmacSha256InitRequest_t))

// HMAC SHA256 init response
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeHmacSha256InitResponse_t;

#define nfMIN_HMACSHA256_INIT_RSP_SIZE (sizeof(nfTeeHmacSha256InitResponse_t))

// hmac-sha256 update request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	uint32_t					DataLength;
	uint8_t	    				Data[];
} nfTeeHmacSha256UpdateRequest_t;

#define nfMIN_HMACSHA256_UPDATE_REQ_SIZE (sizeof(nfTeeHmacSha256UpdateRequest_t))

// hmac-sha256 final request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeHmacSha256FinalRequest_t;

#define nfMIN_HMACSHA256_FINAL_REQ_SIZE (sizeof(nfTeeHmacSha256FinalRequest_t))

// HMAC SHA256 response
typedef struct
{
	uint8_t	    HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
} nfTeeHmacSha256FinalResponse_t;

#define nfMIN_HMACSHA256_FINAL_RSP_SIZE (sizeof(nfTeeHmacSha256FinalResponse_t))

// HMAC SHA256 verify request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	uint8_t	    			HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
	uint32_t				DataLength;
	uint8_t	    			Data[];
} nfTeeHmacSha256VerifyRequest_t;

#define nfMIN_HMACSHA256_VERIFY_REQ_SIZE (sizeof(nfTeeHmacSha256VerifyRequest_t) + 1)


// HMAC SHA256 verify init request
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
} nfTeeHmacSha256VerifyInitRequest_t;

#define nfMIN_HMACSHA256_VERIFY_INIT_REQ_SIZE (sizeof(nfTeeHmacSha256VerifyInitRequest_t))

// HMAC SHA256 verify init response
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
} nfTeeHmacSha256VerifyInitResponse_t;

#define nfMIN_HMACSHA256_VERIFY_INIT_RSP_SIZE (sizeof(nfTeeHmacSha256VerifyInitResponse_t))

// hmac-sha256 verify update request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	uint32_t					DataLength;
	uint8_t	    				Data[];
} nfTeeHmacSha256VerifyUpdateRequest_t;

#define nfMIN_HMACSHA256_VERIFY_UPDATE_REQ_SIZE (sizeof(nfTeeHmacSha256VerifyUpdateRequest_t))

// hmac-sha256 verify final request
typedef struct
{
	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	uint8_t	    				HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
} nfTeeHmacSha256VerifyFinalRequest_t;

#define nfMIN_HMACSHA256_VERIFY_FINAL_REQ_SIZE (sizeof(nfTeeHmacSha256VerifyFinalRequest_t))


#if 0 // currently, this is pass/fail, so response is empty
// HMAC SHA256 verify response
typedef struct
{
} nfTeeHmacSha256VerifyResponse_t;
#endif

//rsa SHA verify request
#define nfRSA_SHA256_MAX_SIZE	512 // size of RSA SHA
typedef struct
{
	nfCryptoKeyHandle_t		KeyHandle;
	uint8_t	    			Sha[nfRSA_SHA256_MAX_SIZE];
	uint32_t				ShaLength;
	uint32_t				DataLength;
	uint8_t	    			Data[];
} nfTeeRsaSha256VerifyRequest_t;

#define nfMIN_RSASHA256_VERIFY_REQ_SIZE (sizeof(nfTeeRsaSha256VerifyRequest_t) + 1)

//rsa SHA256 verify response is just success or not, no struct definition needed

#if 0
// clearKeys request - currently empty...
typedef struct
{

} nfTeeClearKeysRequest_t;
#endif
// clearKeys response
typedef struct
{
	uint32_t	NumHandles;
	uint32_t	DeletedHandles[];
} nfTeeClearKeysResponse_t;

#define nfMIN_CLEARKEYS_RSP_SIZE	sizeof(nfTeeClearKeysResponse_t)

typedef struct
{
	uint8_t			Key[32];
	uint8_t			IV[16];
	uint8_t			cipher[16];
	void*			hmac;
}caAes128CryptoContextItem_t;

// enum defining the type of AES cipher, i.e. CBC or GCM
typedef enum
{
    nfAES_CBC,
    nfAES_GCM,
} nfAesCipher_t;

// Holds the Aes CBC/GCM key and IV
struct aescipher_context_st {
	nfAesCipher_t cipherType;
	unsigned char* key;
	uint32_t keySize;
	unsigned char* iv;
	uint32_t ivSize;
};

typedef struct aescipher_context_st nfAesCipherContext_t;


/***************************************************************************
 *
 *                    JWK/JWE-related definitions
 *
 *  from http://tools.ietf.org/html/draft-ietf-jose-json-web-algorithms-16
 ****************************************************************************/
// These values may (should?) be moved to a different file
#define MAX_RSA_COMPONENT_LEN 2048	//maximum length any of n, d, e etc can be

// include length definitions to do safe compare (no strcmp()!!!)
#define nfJWK_KTY_KEY_LEN 3
#define nfJWK_KTY_KEY	"kty"

#define nfJWK_KTY_EC_LEN 2
#define nfJWK_KTY_EC	"EC"

#define nfJWK_KTY_RSA_LEN 3
#define nfJWK_KTY_RSA	"RSA"

#define nfJWK_KTY_OCT_LEN 3
#define nfJWK_KTY_OCT	"oct"

#define nfJWK_OCT_PARM_k_LEN 1
#define nfJWK_OCT_PARM_k "k"

#define nfJWK_RSA_PARM_n_LEN 1
#define nfJWK_RSA_PARM_n "n"

#define nfJWK_RSA_PARM_d_LEN 1
#define nfJWK_RSA_PARM_d "d"

#define nfJWK_RSA_PARM_e_LEN 1
#define nfJWK_RSA_PARM_e "e"

#define nfJWK_RSA_PARM_p_LEN 1
#define nfJWK_RSA_PARM_p "p"

#define nfJWK_RSA_PARM_q_LEN 1
#define nfJWK_RSA_PARM_q "q"

#define nfJWK_RSA_PARM_dp_LEN 2
#define nfJWK_RSA_PARM_dp "dp"

#define nfJWK_RSA_PARM_dq_LEN 2
#define nfJWK_RSA_PARM_dq "dq"

#define nfJWK_RSA_PARM_qi_LEN 2
#define nfJWK_RSA_PARM_qi "qi"

#define nfJWK_RSA_PARM_oth_LEN 3
#define nfJWK_RSA_PARM_oth "oth"

#define nfJWK_ALG_KEY_LEN 3
#define nfJWK_ALG_KEY	"alg"

#define nfJWK_KID_KEY_LEN 3
#define nfJWK_KID_KEY	"kid"

#define nfJWK_USE_KEY_LEN 3
#define nfJWK_USE_KEY	"use"

#define nfJWK_USE_PARM_sig_LEN 3
#define nfJWK_USE_PARM_sig	"sig"

#define nfJWK_USE_PARM_enc_LEN 3
#define nfJWK_USE_PARM_enc	"enc"

#define nfJWK_USE_PARM_wrap_LEN 4
#define nfJWK_USE_PARM_wrap	"wrap"

#define nfJWK_EXTRACTABLE_KEY_LEN 11
#define nfJWK_EXTRACTABLE_KEY "extractable"

#define nfJWE_ALG_KEY_LEN 3
#define nfJWE_ALG_KEY "alg"

#define nfJWE_ENC_KEY_LEN 3
#define nfJWE_ENC_KEY "enc"

#define nfJWE_ENC_PARM_A128GCM_LEN 7
#define nfJWE_ENC_PARM_A128GCM "A128GCM"

#define nfJWE_ENC_PARM_A256GCM_LEN 7
#define nfJWE_ENC_PARM_A256GCM "A256GCM"


#endif /* CRYPTOTYPES_H_ */

// vim: expandtab tabstop=4 shiftwidth=4
