/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     sectk.c                                                    */
/*                                                                           */
/* Description:   CI+  Security Toolkit Module                               */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 10/04/2001                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */
/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "techtype.h"
#include "sectk.h"
#include "celo.h"
#include "ccbn.h"
#include "x_os.h"
#include "ci_trust_zone.h"

#if 0
#define CC_PRINT(x) CNTCTRL_DBG_ERROR(x)
#endif

#ifndef CC_PRINT_BUFFER
#define CC_PRINT_BUFFER(d,l) do {                \
      int jj;                                    \
      for (jj = 0; jj < (l); ++jj)               \
      {                                          \
         CC_PRINT(("%02x ", (d)[jj]));           \
         if ((jj & 0xf) == 0xf)                  \
         {                                       \
            CC_PRINT(("\n"));                    \
         }                                       \
      }                                          \
      if (((l) & 0xf) != 0)                      \
      {                                          \
         CC_PRINT(("\n"));                       \
      }                                          \
   } while (0)
#endif

/* #define SECTK_DEBUG */
#ifdef SECTK_DEBUG
#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)
#else
#define CC_DEBUG_PRINT(x)
#define CC_DEBUG_BUFFER(x,y)
#endif

//#define FUNCTION_START(x) do { CC_PRINT((">> %s\n",#x)); } while(0)
#define FUNCTION_START(x) 

//#define FUNCTION_FINISH(x) do { CC_PRINT((">> %s\n",#x)); } while(0)
#define FUNCTION_FINISH(x)

#define CC_TO_BOOL(x) (((x) != 0) ? TRUE : FALSE)

#define KEY_LENGTH 16
#define BLOCK_SIZE_BITS 128
#define BLOCK_SIZE_BYTES 16
#define BLOCK_SIZE_WORDS 4
#define BLOCK_SIZE_MASK ~0xfL
#define DH_EXP_SIZE 256
#define DH_PUB_SIZE 256
#define DH_G_SIZE 256
#define DH_P_SIZE 256
#define DH_Q_SIZE 32
#define DH_SIZE 256
#define RSA_MOD_SIZE 256
#define SALT_LEN 20
#define DEVICE_ID_BYTES 16
#define OU_TEST_LENGTH 4
#define OU_PRODUCTION_LENGTH 10

/* Debug: print time */
/* #define SECTK_PRINT_TIME */

/* --- local typedef structs for this file --- */

typedef enum
{
   ASN1_TAG_BOOLEAN           = 0x01,
   ASN1_TAG_INTEGER           = 0x02,
   ASN1_TAG_BIT_STRING        = 0x03,
   ASN1_TAG_OCTET_STRING      = 0x04,
   ASN1_TAG_OBJECT_IDENTIFIER = 0x06,
   ASN1_TAG_UTF8_STRING       = 0x0c,
   ASN1_TAG_PRINTABLE_STRING  = 0x13,
   ASN1_TAG_UTCTIME           = 0x17,
   ASN1_TAG_SEQUENCE          = 0x30,
   ASN1_TAG_SET               = 0x31,
   ASN1_TAG_KEY_IDENTIFIER    = 0x80,
   ASN1_TAG_VERSION           = 0xa0,
   ASN1_TAG_EXTENSIONS        = 0xa3
} E_ASN1_TAG;

typedef struct
{
   U8BIT s_i[BLOCK_SIZE_BYTES];
   U8BIT k[BLOCK_SIZE_BYTES];
   U8BIT dt_i[BLOCK_SIZE_BYTES];
} S_PRNG;

typedef struct
{
   U8BIT *data;
   U16BIT len;
} S_BUFFER;

typedef struct
{
   U32BIT notBefore;
   U32BIT notAfter;
} S_VALIDITY;

typedef struct
{
   S_BUFFER modulus;
   S_BUFFER publicExponent;
   S_BUFFER privateExponent;
   S_BUFFER prime1;
   S_BUFFER prime2;
   S_BUFFER exponent1;
   S_BUFFER exponent2;
   S_BUFFER coefficient;
} S_RSA_KEY;

typedef struct
{
   S_BUFFER bitstring;
   S_RSA_KEY key;
} S_KEY_INFO;

typedef struct
{
   S_BUFFER extnID;
   BOOL critical;
   S_BUFFER extnValue;
} S_X509_EXT;

typedef struct
{
   S_BUFFER tbsCertificate;
   S_BUFFER serialNumber;
   S_BUFFER signature;
   S_BUFFER issuer;
   S_VALIDITY validity;
   S_BUFFER subject;
   S_KEY_INFO subjectPublicKeyInfo;
   S_BUFFER modulus;
   S_BUFFER publicExponent;
   S_BUFFER extensions;
   S_BUFFER signatureAlgorithm;
   S_BUFFER signatureValue;
   U8BIT *data;
   U16BIT len;
} S_X509;

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

static S_PRNG prng;
static U8BIT dh_zero[DH_SIZE];
static U8BIT dh_one[DH_SIZE];

static const struct ext_info
{
   U16BIT obj_length;
   U16BIT id_length;
   U16BIT id_offset;
   U8BIT obj_id[10];
   E_SECTK_CERT_EXT_TYPE type;
} ext_info[] = {
   {
      5, 3, 2,
      {0x06, 0x03, 0x55, 0x1d, 0x0e},
      SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER,
   },
   {
      5, 3, 2,
      {0x06, 0x03, 0x55, 0x1d, 0x0f},
      SECTK_CERT_EXT_KEY_USAGE,
   },
   {
      5, 3, 2,
      {0x06, 0x03, 0x55, 0x1d, 0x13},
      SECTK_CERT_EXT_BASIC_CONSTRAINTS
   },
   {
      5, 3, 2,
      {0x06, 0x03, 0x55, 0x1d, 0x23},
      SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER
   },
   {
      10, 8, 2,
      {0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x19},
      SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES
   },
   {
      10, 8, 2,
      {0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x1a},
      SECTK_CERT_EXT_CI_PLUS_INFO,
   },
   {
      10, 8, 2,
      {0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x1b},
      SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER
   }
};

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

static void XorBlock(U8BIT *first_block, U8BIT *second_block,
                     U8BIT *result_block);
static void UpdateCounter(void);
static void GenerateRandomBlock(U8BIT *block);
static BOOL AppendBlock(U8BIT *buffer, U16BIT length, U16BIT offset,
                           U8BIT *block);
static BOOL CheckDhPublicKey(U8BIT *dh_pub, U8BIT *dh_p, U8BIT *dh_q);
static BOOL SignBuffer(U8BIT *buffer, U16BIT length, S_RSA_KEY *key,
                          U8BIT *signature);
static BOOL EmsaPssEncode(U8BIT *buffer, U16BIT length, U8BIT *EM,
                             U32BIT emBits);
static BOOL VerifyBuffer(U8BIT *buffer, U16BIT length, S_RSA_KEY *key,
                            U8BIT *signature, U16BIT signature_len);
static BOOL EmsaPssVerify(U8BIT *buffer, U16BIT length, U8BIT *EM,
                             U32BIT emBits);
static void Mgf1(U8BIT *mgfSeed, U16BIT maskLen, U8BIT *mask);
static void Mgf1Xor(U8BIT *mgfSeed, U16BIT maskLen, U8BIT *mask);
static BOOL ParseCertificate(S_X509 *x509);
static BOOL ParseTbsCertificate(S_X509 *x509, U32BIT *offset);
static BOOL VerifyCertificateVersion(U8BIT *data, U32BIT *offset,
                                        U32BIT limit);
static BOOL ParseSerialNumber(U8BIT *data, U32BIT *offset, U32BIT limit,
                                 S_BUFFER *serialNumber);
static BOOL ParseSignature(U8BIT *data, U32BIT *offset, U32BIT limit,
                              S_BUFFER *signature);
static BOOL ParseIssuer(U8BIT *data, U32BIT *offset, U32BIT limit,
                           S_BUFFER *issuer);
static BOOL ParseValidity(U8BIT *data, U32BIT *offset, U32BIT limit,
                             S_VALIDITY *validity);
static BOOL ConvertUTCTimeToSeconds(U8BIT *data, U32BIT length,
                                       U32BIT *seconds);
static BOOL ParseSubject(U8BIT *data, U32BIT *offset, U32BIT limit,
                            S_BUFFER *subject);
static BOOL ParseSubjectPublicKeyInfo(U8BIT *data, U32BIT *offset,
                                         U32BIT limit, S_KEY_INFO *subjectPublicKeyInfo);
static BOOL ParseExtensions(U8BIT *data, U32BIT *offset, U32BIT limit,
                               S_BUFFER *extensions);
static BOOL GetExtensionByOid(S_BUFFER *extensions, const U8BIT *oid,
                                 U16BIT oid_len, S_X509_EXT *ext);
static BOOL GetNextExtension(U8BIT *data, S_X509_EXT *ext, U32BIT *offset,
                                U32BIT limit);
static void ParseCertificateExtension(S_X509_EXT *x509_ext,
                                      S_SECTK_CERT_EXT *cert_ext);
static E_SECTK_CERT_EXT_TYPE GetExtensionType(S_X509_EXT *x509_ext);
static BOOL ParseKeyUsage(S_X509_EXT *x509_ext, S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseSubjectKeyIdentifier(S_X509_EXT *x509_ext,
                                         S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseAuthorityKeyIdentifier(S_X509_EXT *x509_ext,
                                           S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseBasicConstraints(S_X509_EXT *x509_ext,
                                     S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseScramblerCapabilities(S_X509_EXT *x509_ext,
                                          S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseCicamBrandIdentifier(S_X509_EXT *x509_ext,
                                         S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseCiPlusInfo(S_X509_EXT *x509_ext,
                               S_SECTK_CERT_EXT *cert_ext);
static BOOL ParseSignatureAlgorithm(S_X509 *x509, U32BIT *offset);
static BOOL VerifySignatureAlgorithm(S_BUFFER *signatureAlgorithm);
static BOOL ParseSignatureValue(S_X509 *x509, U32BIT *offset);
static BOOL ParseRsaKey(U8BIT *data, U16BIT len, S_RSA_KEY *key);
static void ClearRsaKey(S_RSA_KEY *key);
static BOOL ParseAsn1PositiveInteger(U8BIT *data, S_BUFFER *integer,
                                        U32BIT *offset, U32BIT limit);
static BOOL GetAsn1Tag(U8BIT *data, U32BIT *tag, U32BIT *offset,
                          U32BIT limit);
static BOOL MatchAsn1Item(U8BIT *data, U32BIT *offset, U32BIT limit,
                             E_ASN1_TAG tag, U32BIT *length);
static BOOL GetAsn1Length(U8BIT *data, U32BIT *length, U32BIT *offset,
                             U32BIT limit);
static BOOL GetAsn1IntegerValue(U8BIT *data, U32BIT length, S32BIT *value,
                                   U32BIT *offset, U32BIT limit);
static BOOL GetAsn1BooleanValue(U8BIT *data, U32BIT length, BOOL *value,
                                   U32BIT *offset, U32BIT limit);
static S32BIT SignExtend(U32BIT u_value);
static BOOL ConvertHexToBin(char *buffer, U8BIT *id);

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/
#ifdef SECTK_PRINT_TIME
extern U32BIT STB_OSGetClockMilliseconds(void);
#endif
extern CC_CI_PLUS_KEY_TYPE gui1_key_type;
extern VOID TZ_ciplus_read_cred (E_TZ_STB_CIPLUS_KEY_TYPE type, UINT8 **key, UINT16 *length);


/*!**************************************************************************
 * @brief    Initialise security module
 ****************************************************************************/
void SECTK_Initialise(void)
{
   FUNCTION_START(SECTK_Initialise);

   memset(&prng, 0x00, sizeof prng);
   memset(dh_zero, 0, sizeof dh_zero);
   memset(dh_one, 0, sizeof dh_one);
   dh_one[sizeof(dh_one)-1] = 1;

   FUNCTION_FINISH(SECTK_Initialise);
}

/*!**************************************************************************
 * @brief    Apply AES-128 encryption on a buffer
 *
 * This function encrypts the input buffer, storing the result in the same
 * buffer. If the length of the buffer is not a multiply of 128 bits, the
 * last (incomplete) block will not be encrypted.
 *
 * @param    buffer - buffer to encrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 ****************************************************************************/
void SECTK_AesEncrypt(U8BIT *buffer, U16BIT length, U8BIT *key)
{
   U16BIT block_length, offset;
   S_AES128_KEY aes_key;

   FUNCTION_START(SECTK_AesEncrypt);

   CE_Aes128SetEncryptKey(key, &aes_key);

   block_length = length & BLOCK_SIZE_MASK;
   for (offset = 0; offset < block_length; offset += BLOCK_SIZE_BYTES)
   {
      CE_Aes128Encrypt(buffer, &aes_key, buffer);
      buffer += BLOCK_SIZE_BYTES;
   }

   /* Clear variables on the stack */
   memset(&aes_key, 0xbd, sizeof aes_key);
   block_length = offset = 0xbdbd;

   FUNCTION_FINISH(SECTK_AesEncrypt);
}

/*!**************************************************************************
 * @brief    Apply AES-128 decryption on a buffer
 *
 * This function decrypts the input buffer, storing the result in the same
 * buffer. If the length of the buffer is not a multiply of 128 bits, the
 * last (incomplete) block will not be decrypted.
 *
 * @param    buffer - buffer to decrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 ****************************************************************************/
void SECTK_AesDecrypt(U8BIT *buffer, U16BIT length, U8BIT *key)
{
   U16BIT block_length, offset;
   S_AES128_KEY aes_key;

   FUNCTION_START(SECTK_AesDecrypt);

   CE_Aes128SetDecryptKey(key, &aes_key);

   block_length = length & BLOCK_SIZE_MASK;
   for (offset = 0; offset < block_length; offset += BLOCK_SIZE_BYTES)
   {
      CE_Aes128Decrypt(buffer, &aes_key, buffer);
      buffer += BLOCK_SIZE_BYTES;
   }

   /* Clear variables on the stack */
   memset(&aes_key, 0xbd, sizeof aes_key);
   block_length = offset = 0xbdbd;

   FUNCTION_FINISH(SECTK_AesDecrypt);
}

/*!**************************************************************************
 * @brief    Apply AES-128-CBC encryption on a buffer
 * @param    buffer - buffer to encrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 * @param    iv - AES initialisation vector (128 bits = 16 bytes)
 * @note     The result of the encryption is stored in the input buffer
 ****************************************************************************/
void SECTK_AesCbcEncrypt(U8BIT *buffer, U16BIT length, U8BIT *key, U8BIT *iv)
{
   S_AES128_KEY aes_key;

   FUNCTION_START(SECTK_AesCbcEncrypt);

   CE_Aes128SetEncryptKey(key, &aes_key);
   CE_Aes128EncryptCbc(buffer, length, &aes_key, iv, buffer);

   /* Clear variable on the stack */
   memset(&aes_key, 0xbd, sizeof aes_key);

   FUNCTION_FINISH(SECTK_AesCbcEncrypt);
}

/*!**************************************************************************
 * @brief    Apply AES-128 decryption on a buffer
 * @param    buffer - buffer to decrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 * @param    iv - AES initialisation vector (128 bits = 16 bytes)
 * @note     The result of the decryption is stored in the input buffer
 ****************************************************************************/
void SECTK_AesCbcDecrypt(U8BIT *buffer, U16BIT length, U8BIT *key, U8BIT *iv)
{
   S_AES128_KEY aes_key;

   FUNCTION_START(SECTK_AesCbcDecrypt);

   CE_Aes128SetDecryptKey(key, &aes_key);
   CE_Aes128DecryptCbc(buffer, length, &aes_key, iv, buffer);

   /* Clear variable on the stack */
   memset(&aes_key, 0xbd, sizeof aes_key);

   FUNCTION_FINISH(SECTK_AesCbcDecrypt);
}

/*!**************************************************************************
 * @brief    Authenticate buffer using AES-128-XCBC-MAC
 * @param    buffer - buffer to authenticate
 * @param    buffer_len - length of buffer to authenticate
 * @param    key - AES key (128 bits = 16 bytes)
 * @param    mac - Message Authentication Code (128 bits = 16 bytes)
 ****************************************************************************/
void SECTK_Authenticate(U8BIT *buffer, U32BIT buffer_len, U8BIT *key,
                        U8BIT *mac)
{
   U8BIT k1[BLOCK_SIZE_BYTES] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };
   U8BIT k2[BLOCK_SIZE_BYTES] = { 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02 };
   U8BIT k3[BLOCK_SIZE_BYTES] = { 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03 };
   U8BIT e[BLOCK_SIZE_BYTES];
   U8BIT padded[BLOCK_SIZE_BYTES] = { 0x00 };
   U32BIT k1_length;
   U32BIT offset;
   U16BIT remainder;
   S_AES128_KEY aes_key;

   FUNCTION_START(SECTK_Authenticate);

   memset(e, 0x00, BLOCK_SIZE_BYTES);

   CE_Aes128SetEncryptKey(key, &aes_key);
   CE_Aes128Encrypt(k1, &aes_key, k1);
   CE_Aes128Encrypt(k2, &aes_key, k2);
   CE_Aes128Encrypt(k3, &aes_key, k3);

   /* This assumes that BLOCK_SIZE_BYTES is a power of two */
   remainder = buffer_len & ( BLOCK_SIZE_BYTES - 1 );
   k1_length = buffer_len - remainder;
   if (remainder == 0 && k1_length > 0)
   {
      /* k1_length contains the last block */
      k1_length -= BLOCK_SIZE_BYTES;
   }

   CE_Aes128SetEncryptKey(k1, &aes_key);

   for (offset = 0; offset < k1_length; offset += BLOCK_SIZE_BYTES)
   {
      XorBlock(e, buffer + offset, e);
      CE_Aes128Encrypt(e, &aes_key, e);
   }

   if (offset + BLOCK_SIZE_BYTES == buffer_len)
   {
      /* last block is complete, use k2 */
      XorBlock(e, buffer + offset, e);
      XorBlock(e, k2, e);
      CE_Aes128Encrypt(e, &aes_key, e);
   }
   else
   {
      /* last block requires padding */
      memcpy(padded, buffer + offset, remainder);
      padded[remainder] = 0x80;
      /* and k3 */
      XorBlock(e, padded, e);
      XorBlock(e, k3, e);
      CE_Aes128Encrypt(e, &aes_key, e);
   }

   memcpy(mac, e, BLOCK_SIZE_BYTES);

   /* Clear variable on the stack */
   memset(k1, 0xbd, sizeof k1);
   memset(k2, 0xbd, sizeof k2);
   memset(k3, 0xbd, sizeof k3);
   memset(e, 0xbd, sizeof e);
   memset(padded, 0xbd, sizeof padded);
   k1_length = offset = 0xbdbdbdbd;
   remainder = 0xbdbd;
   memset(&aes_key, 0xbd, sizeof aes_key);

   FUNCTION_FINISH(SECTK_Authenticate);
}

/*!**************************************************************************
 * @brief    Apply SHA-256 on a buffer
 * @param    buffer - buffer to decrypt
 * @param    length - length of buffer in bytes
 * @param    digest - result of SHA-256 (256 bits = 32 bytes)
 ****************************************************************************/
void SECTK_Sha256(U8BIT *buffer, U16BIT length, U8BIT *digest)
{
   FUNCTION_START(SECTK_Sha256);

   CE_Sha256(buffer, length, digest);

   FUNCTION_FINISH(SECTK_Sha256);
}

/*!**************************************************************************
 * @brief    Set pseudo-random number generator seed
 * @param    seed - 128-bit (16 byte) PRNG seed
 ****************************************************************************/
void SECTK_SetRandomSeed(U8BIT *seed, U8BIT *key_k)
{
   FUNCTION_START(SECTK_SetRandomSeed);

   /* Copy prng_seed (receiver constant) */
   memcpy(prng.s_i, seed, BLOCK_SIZE_BYTES);
   /* Copy prng_key_k (licence constant) */
   memcpy(prng.k, key_k, BLOCK_SIZE_BYTES);
   memset(prng.dt_i, 0, BLOCK_SIZE_BYTES);

   UpdateCounter();

   FUNCTION_FINISH(SECTK_SetRandomSeed);
}

/*!**************************************************************************
 * @brief    Generate random bit sequence
 * @param    buffer - buffer to fill with the sequence
 * @param    length - length of sequence in bytes
 ****************************************************************************/
void SECTK_GenerateRandomSequence(U8BIT *buffer, U16BIT length)
{
   U16BIT offset;
   U8BIT block[BLOCK_SIZE_BYTES];
   BOOL not_truncated;

   FUNCTION_START(SECTK_GenerateRandomSequence);

   not_truncated = TRUE;
   offset = 0;

   while ((not_truncated) && (offset < length))
   {
      GenerateRandomBlock(block);
      not_truncated = AppendBlock(buffer, length, offset, block);
      offset += BLOCK_SIZE_BYTES;
   }

   /* Clear variable on the stack */
   offset = 0xbdbd;
   memset(block, 0xbd, sizeof block);
   not_truncated = FALSE;

   FUNCTION_FINISH(SECTK_GenerateRandomSequence);
}

/*!**************************************************************************
 * @param    dh_g - Diffie-Hellman generator
 * @param    dh_p - Diffie-Hellman modulus
 * @param    dh_pub - Diffie-Hellman public key (2048 bits = 256 bytes)
 * @param    dh_exp - Diffie-Hellman exponent (2048 bits = 256 bytes)
 ****************************************************************************/
void SECTK_GenerateDhKeys(U8BIT *dh_g, U8BIT *dh_p, U8BIT *dh_q,
                             U8BIT *dh_pub, U8BIT *dh_exp)
{
   BOOL valid;

   FUNCTION_START(SECTK_GenerateDhKeys);
   #ifdef SECTK_PRINT_TIME
   CNTCTRL_DBG_INFO(("SECTK_GenerateDhKeysS (%lu)\n", STB_OSGetClockMilliseconds()));
   #endif               
   do
   {
      /* dh_exp is a random 2048-bit number */
      SECTK_GenerateRandomSequence(dh_exp, DH_EXP_SIZE);

      /* dh_exp = dh_g ^ dh_exp mod dh_p */
      CC_ModExp(dh_g, DH_G_SIZE, dh_exp, DH_EXP_SIZE, dh_p, DH_P_SIZE, dh_pub);

      /* Should be valid, unless dh_exp is 1 or 0 */
      valid = CheckDhPublicKey(dh_pub, dh_p, dh_q);

   } while (!valid);
   #ifdef SECTK_PRINT_TIME
   CNTCTRL_DBG_INFO(("SECTK_GenerateDhKeysE (%lu)\n", STB_OSGetClockMilliseconds()));
   #endif
   CC_DEBUG_PRINT(("dh_pub =\n"));
   CC_DEBUG_BUFFER(dh_pub, 256);
   CC_DEBUG_PRINT(("dh_exp =\n"));
   CC_DEBUG_BUFFER(dh_exp, 256);
   CC_DEBUG_PRINT(("dh_p =\n"));
   CC_DEBUG_BUFFER(dh_p, 256);
   CC_DEBUG_PRINT(("dh_g =\n"));
   CC_DEBUG_BUFFER(dh_g, 256);
   CC_DEBUG_PRINT(("dh_q =\n"));
   CC_DEBUG_BUFFER(dh_q, 32);

   FUNCTION_FINISH(SECTK_GenerateDhKeys);
}

/*!**************************************************************************
 * @brief    Generate Diffie-Hellman keys
 * @param    dh_pub - Diffie-Hellman public key (2048 bits = 256 bytes)
 * @param    dh_exp - Diffie-Hellman exponent (2048 bits = 256 bytes)
 * @param    dh_p - Diffie-Hellman modulus (2048 bits = 256 bytes)
 * @param    shared - Diffie-Hellman shared key (2048 bits = 256 bytes)
 ****************************************************************************/
void SECTK_CalculateDhShared(U8BIT *dh_pub, U8BIT *dh_exp, U8BIT *dh_p,
                             U8BIT *shared)
{
   FUNCTION_START(SECTK_CalculateDhShared);
   #ifdef SECTK_PRINT_TIME
   CNTCTRL_DBG_INFO(("SECTK_CalculateDhSharedS (%lu)\n", STB_OSGetClockMilliseconds()));
   #endif
   CC_ModExp(dh_pub, DH_P_SIZE, dh_exp, DH_EXP_SIZE, dh_p, DH_P_SIZE, shared);
   #ifdef SECTK_PRINT_TIME
   CNTCTRL_DBG_INFO(("SECTK_CalculateDhSharedE (%lu)\n", STB_OSGetClockMilliseconds()));
   #endif

   FUNCTION_FINISH(SECTK_CalculateDhShared);
}

/*!**************************************************************************
 * @brief    Check DH public key
 *
 * This function checks the following conditions for DH public key DH_pub:
 * -# 1 < DH_pub < DH_p
 * -# DH_pub ^ DH_q mod DH_p = 1
 *
 * @param    dh_pub - DH public key (2048 bits = 256 bytes)
 * @param    dh_p - Diffie-Hellman modulus (2048 bits)
 * @param    dh_q - Diffie-Hellman "Sophie-Germain" constant (2048 bits)
 * @return   TRUE if DH public key is valid, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_CheckDhPublicKey(U8BIT *dh_pub, U8BIT *dh_p, U8BIT *dh_q)

{
   BOOL valid;

   FUNCTION_START(SECTK_CheckDhPublicKey);
   #ifdef SECTK_PRINT_TIME
   CNTCTRL_DBG_INFO(("CheckDhPublicKeyE (%lu)\n", STB_OSGetClockMilliseconds()));
   #endif
   valid = CheckDhPublicKey(dh_pub, dh_p, dh_q);
   #ifdef SECTK_PRINT_TIME
   CNTCTRL_DBG_INFO(("CheckDhPublicKeyE (%lu)\n", STB_OSGetClockMilliseconds()));
   #endif
   FUNCTION_FINISH(SECTK_CheckDhPublicKey);

   return valid;
}

/*!**************************************************************************
 * @brief    Parse X.509 certificate
 * @param    buffer - buffer holding certificate in DER format
 * @param    length - length of buffer in bytes
 * @return   Pointer to parsed certificate
 ****************************************************************************/
void *SECTK_ParseCertificate(U8BIT *buffer, U16BIT length)
{
   U8BIT *x509_buffer;
   S_X509 *x509 = NULL;
   BOOL success;

   FUNCTION_START(SECTK_ParseCertificate);

   x509_buffer = malloc(sizeof *x509 + length);
   if (x509_buffer != NULL)
   {
      x509 = (S_X509 *)x509_buffer;
      x509->data = x509_buffer + sizeof *x509;
      x509->len = length;
      memcpy(x509->data, buffer, length);

      success = ParseCertificate(x509);
      if (!success)
      {
         free(x509_buffer);
         x509 = NULL;
      }
   }

   FUNCTION_FINISH(SECTK_ParseCertificate);

   return x509;
}

/*!**************************************************************************
 * @brief    Sign buffer using RSA private key
 * @param    buffer - buffer to sign
 * @param    length - buffer length in bytes
 * @param    key - DER encoded private key
 * @param    key_len - length of private key
 * @param    [out] signature - RSA signature (2048 bits = 256 bytes)
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_Sign(U8BIT *buffer, U16BIT length, U8BIT *key, U16BIT key_len,
                   U8BIT *signature)
{
   S_RSA_KEY rsa_key;
   BOOL success;

   FUNCTION_START(SECTK_Sign);

   success = ParseRsaKey(key, key_len, &rsa_key);
   if (success)
   {
      success = SignBuffer(buffer, length, &rsa_key, signature);
   }

   /* Clear variable on the stack */
   memset(&rsa_key, 0xbd, sizeof rsa_key);

   FUNCTION_FINISH(SECTK_Sign);

   return success;
}

/*!**************************************************************************
 * @brief    Verify signature using RSA public key from certificate
 * @param    buffer - buffer to verify
 * @param    length - buffer length in bytes
 * @param    cert - pointer to certificate
 * @param    signature - RSA signature (2048 bits = 256 bytes)
 * @return   TRUE if signature was verified, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_Verify(U8BIT *buffer, U16BIT length, void *cert,
                     U8BIT *signature)
{
   S_X509 *x509 = cert;
   BOOL success;

   FUNCTION_START(SECTK_Verify);

   success = VerifyBuffer(buffer, length, &x509->subjectPublicKeyInfo.key,
                          signature, 256);

   FUNCTION_FINISH(SECTK_Verify);

   return success;
}

/*!**************************************************************************
 * @brief    Verify certificate signature
 *
 * This function verifies the signature on a given certificate using the public
 * key of another certificate. It uses RSASSA-PSS for the verification.
 *
 * @param    cert - pointer to certificate
 * @param    parent_cert - pointer to certificate containing public key
 * @return   TRUE if certificate signature was verified, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_VerifyCertificate(void *cert, void *parent_cert)
{
   S_X509 *x509 = cert;
   S_X509 *x509_parent = parent_cert;
   BOOL success;

   FUNCTION_START(SECTK_VerifyCertificate);

   success = VerifyBuffer(x509->tbsCertificate.data,
                          x509->tbsCertificate.len,
                          &x509_parent->subjectPublicKeyInfo.key,
                          x509->signatureValue.data,
                          x509->signatureValue.len);

   FUNCTION_FINISH(SECTK_VerifyCertificate);

   return success;
}

/*!**************************************************************************
 * @brief    Verify certificate key identifier
 *
 * This function verifies the given key identifier is the SHA-1 hash value of
 * the public key stored in the certificate.
 *
 * @param    cert - pointer to certificate
 * @param    key_identifier - key identifier (160 bits = 20 bytes)
 * @return   TRUE if key identifier was verified, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_VerifyCertificateKeyIdentifier(void *cert,
                                             U8BIT *key_identifier)
{
   S_X509 *x509 = cert;
   U8BIT digest[CE_SHA1_DIGEST_SIZE];
   BOOL success;

   FUNCTION_START(SECTK_VerifyCertificateKeyIdentifier);

   success = FALSE;
   CE_Sha1(x509->subjectPublicKeyInfo.bitstring.data,
           x509->subjectPublicKeyInfo.bitstring.len, digest);
   if (memcmp(digest, key_identifier, CE_SHA1_DIGEST_SIZE) == 0)
   {
      success = TRUE;
   }

   FUNCTION_FINISH(SECTK_VerifyCertificateKeyIdentifier);

   return success;
}

/*!**************************************************************************
 * @brief    Extract device ID from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] id - device ID (64 bits = 8 bytes)
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractDeviceId(void *dev_cert, U8BIT *id)
{
   static U8BIT oid[] = { 0x55, 0x04, 0x03 };
   S_X509 *x509 = dev_cert;
   U8BIT *data;
   U32BIT limit;
   U32BIT tag;
   U32BIT length;
   U32BIT offset;
   U8BIT *found_oid;
   U32BIT found_oid_len;
   BOOL success;
   BOOL found;
   char buffer[DEVICE_ID_BYTES+1];

   FUNCTION_START(SECTK_ExtractDeviceId);

   data = x509->subject.data;
   limit = x509->subject.len;
   offset = 0;

   success = TRUE;
   found = FALSE;

   while (success)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SET, &length);
      if (success)
      {
         success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE,
                                 &length);
      }
      if (success)
      {
         success = MatchAsn1Item(data, &offset, limit,
                                 ASN1_TAG_OBJECT_IDENTIFIER, &length);
      }
      if (success)
      {
         found_oid = data + offset;
         found_oid_len = length;

         offset += length;
         success = GetAsn1Tag(data, &tag, &offset, limit);
      }
      if (success && (tag == ASN1_TAG_PRINTABLE_STRING ||
                      tag == ASN1_TAG_UTF8_STRING))
      {
         success = GetAsn1Length(data, &length, &offset, limit);
      }
      if (success)
      {
         if ((found_oid_len == sizeof(oid)) &&
             (memcmp(found_oid, oid, found_oid_len) == 0) &&
             (length == DEVICE_ID_BYTES))
         {
            memcpy(buffer, data + offset, DEVICE_ID_BYTES);
            buffer[DEVICE_ID_BYTES] = '\0';
            ConvertHexToBin(buffer, id);
            found = TRUE;
            break;
         }
         else
         {
            offset += length;
         }
      }
   }

   FUNCTION_FINISH(SECTK_ExtractDeviceId);

   return found;
}
/*!**************************************************************************
 * @brief    SECTK_VerifyDeviceId
 * @param    dev_cert - pointer to device certificate
 * @return   TRUE if valid, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_VerifyDeviceId(void *dev_cert)
{
   static U8BIT oid[] = { 0x55, 0x04, 0x03 };
   S_X509 *x509 = dev_cert;
   U8BIT *data;
   U32BIT limit;
   U32BIT tag;
   U32BIT length;
   U32BIT offset;
   U8BIT *found_oid;
   U32BIT found_oid_len;
   BOOL success;
   BOOL b_valid;

   FUNCTION_START(SECTK_ExtractDeviceId);

   data = x509->subject.data;
   limit = x509->subject.len;
   offset = 0;

   success = TRUE;
   b_valid = FALSE;

   while (success)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SET, &length);
      if (success)
      {
         success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE,
                                 &length);
      }
      if (success)
      {
         success = MatchAsn1Item(data, &offset, limit,
                                 ASN1_TAG_OBJECT_IDENTIFIER, &length);
      }
      if (success)
      {
         found_oid = data + offset;
         found_oid_len = length;

         offset += length;
         success = GetAsn1Tag(data, &tag, &offset, limit);
      }
      if (success && (tag == ASN1_TAG_PRINTABLE_STRING ||
                      tag == ASN1_TAG_UTF8_STRING))
      {
         success = GetAsn1Length(data, &length, &offset, limit);
      }
      if (success)
      {
         if ((found_oid_len == sizeof(oid)) &&
             (memcmp(found_oid, oid, found_oid_len) == 0) &&
             (length == DEVICE_ID_BYTES))
         {
            /*maybe add other check in future*/
            CC_DEBUG_PRINT(("sectk device id valid \n"));    
            b_valid = TRUE;
            break;
         }
         else
         {
            offset += length;
         }
      }
   }

   FUNCTION_FINISH(SECTK_ExtractDeviceId);

   return b_valid;
}

/*!**************************************************************************
 * @brief    Extract scrambler capabilities from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] capability - scrambler capability
 * @param    [out] version - capability version
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractScramblerCaps(void *dev_cert, S32BIT *capability,
                                   S32BIT *version)
{
   S_SECTK_CERT_EXT cert_ext;
   BOOL success;

   FUNCTION_START(SECTK_ExtractScramblerCaps);

   success = SECTK_GetCertificateExtension(dev_cert, SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES, &cert_ext);
   if (success)
   {
      *capability = cert_ext.data.scrambler_capabilities.capability;
      *version = cert_ext.data.scrambler_capabilities.version;
   }

   FUNCTION_FINISH(SECTK_ExtractScramblerCaps);

   return success;
}

/*!**************************************************************************
 * @brief    Extract brand ID from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] brand_id - brand ID
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractBrandId(void *dev_cert, U16BIT *brand_id)
{
   S_SECTK_CERT_EXT cert_ext;
   S32BIT cicam_brand_id;
   BOOL success;

   FUNCTION_START(SECTK_ExtractBrandId);

   success = SECTK_GetCertificateExtension(dev_cert, SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER, &cert_ext);
   if (success)
   {
      cicam_brand_id = cert_ext.data.cicam_brand_identifier.cicamBrandId;
      if (cicam_brand_id > 0 && cicam_brand_id < 65536)
      {
         *brand_id = cicam_brand_id;
      }
      else
      {
         /* Out of range */
         success = FALSE;
      }
   }

   FUNCTION_FINISH(SECTK_ExtractBrandId);

   return success;
}

/*!**************************************************************************
 * @brief    Return issuer name from certificate
 * @param    cert - pointer to certificate
 * @return   Pointer to issuer name (opaque handle)
 ****************************************************************************/
void *SECTK_GetCertificateIssuer(void *cert)
{
   S_X509 *x509 = cert;
   void *issuer;

   FUNCTION_START(SECTK_GetCertificateIssuer);

   issuer = &x509->issuer;

   FUNCTION_FINISH(SECTK_GetCertificateIssuer);

   return issuer;
}

/*!**************************************************************************
 * @brief    Return subject name from certificate
 * @param    cert - pointer to certificate
 * @return   Pointer to subject name (opaque handle)
 ****************************************************************************/
void *SECTK_GetCertificateSubject(void *cert)
{
   S_X509 *x509 = cert;
   void *subject;

   FUNCTION_START(SECTK_GetCertificateSubject);

   subject = &x509->subject;

   FUNCTION_FINISH(SECTK_GetCertificateSubject);

   return subject;
}

/*!**************************************************************************
 * @brief    Compare certificate names (issuer / subject)
 * @param    name_a - pointer to name
 * @param    name_b - pointer to name
 * @return   TRUE if the names are identical, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_CompareCertificateName(void *name_a, void *name_b)
{
   S_BUFFER *buf_a = name_a;
   S_BUFFER *buf_b = name_b;
   BOOL same;

   FUNCTION_START(SECTK_CompareCertificateName);

   same = FALSE;
   if ((buf_a->len == buf_b->len) &&
       (memcmp(buf_a->data, buf_b->data, buf_a->len) == 0))
   {
      same = TRUE;
   }

   FUNCTION_FINISH(SECTK_CompareCertificateName);

   return same;
}

/*!**************************************************************************
 * @brief    Check whether certificate date/time is not before the given time
 * @param    cert - pointer to certificate
 * @param    epoch_time - time since epoch in seconds
 * @return   TRUE if the certificate date/time is not before the given time
 ****************************************************************************/
BOOL SECTK_CheckCertificateNotBefore(void *cert, U32BIT epoch_time)
{
   S_X509 *x509 = cert;
   BOOL valid;

   FUNCTION_START(SECTK_CheckCertificateNotBefore);
   valid = FALSE;
   if (epoch_time >= x509->validity.notBefore)
   {
      valid = TRUE;
   }

   FUNCTION_FINISH(SECTK_CheckCertificateNotBefore);

   return valid;
}

/*!**************************************************************************
 * @brief    Check whether certificate date/time is not after the given time
 * @param    cert - pointer to certificate
 * @param    epoch_time - time since epoch in seconds
 * @return   TRUE if the certificate date/time is not after the given time
 ****************************************************************************/
BOOL SECTK_CheckCertificateNotAfter(void *cert, U32BIT epoch_time)
{
   S_X509 *x509 = cert;
   BOOL valid;

   FUNCTION_START(SECTK_CheckCertificateNotAfter);

   valid = FALSE;
   if (epoch_time <= x509->validity.notAfter)
   {
      valid = TRUE;
   }

   FUNCTION_FINISH(SECTK_CheckCertificateNotAfter);

   return valid;
}

/*!**************************************************************************
 * @brief    Return first certificate extension
 * @param    cert - pointer to certificate
 * @param    iter - iterator
 * @param    ext - certificate extension
 * @return   TRUE if extension was found, FALSE otherwise
 * @note     The extension is valid only as long as the certificate is valid
 ****************************************************************************/
BOOL SECTK_GetFirstCertificateExtension(void *cert, U32BIT *iter,
                                           S_SECTK_CERT_EXT *cert_ext)
{
   S_X509 *x509 = cert;
   S_X509_EXT x509_ext;
   U8BIT *data;
   U32BIT limit;
   U32BIT offset;
   BOOL success;
   U32BIT length;

   FUNCTION_START(SECTK_GetFirstCertificateExtension);

   data = x509->extensions.data;
   limit = x509->extensions.len;

   offset = 0;
   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (offset + length == limit))
   {
      data = data + offset;
      limit = length;
      *iter = 0;

      success = GetNextExtension(data, &x509_ext, iter, limit);
      if (success)
      {
         /* Parse extension data */
         ParseCertificateExtension(&x509_ext, cert_ext);
      }
   }
   else
   {
      success = FALSE;
   }

   FUNCTION_FINISH(SECTK_GetFirstCertificateExtension);

   return success;
}

/*!**************************************************************************
 * @brief    Return next certificate extension
 * @param    cert - pointer to certificate
 * @param    iter - iterator
 * @param    ext - certificate extension
 * @return   TRUE if extension was found, FALSE otherwise
 * @note     The extension is valid only as long as the certificate is valid
 ****************************************************************************/
BOOL SECTK_GetNextCertificateExtension(void *cert, U32BIT *iter,
                                          S_SECTK_CERT_EXT *cert_ext)
{
   S_X509 *x509 = cert;
   S_X509_EXT x509_ext;
   U8BIT *data;
   U32BIT limit;
   U32BIT offset;
   BOOL success;
   U32BIT length;

   FUNCTION_START(SECTK_GetNextCertificateExtension);

   data = x509->extensions.data;
   limit = x509->extensions.len;

   offset = 0;
   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (offset + length == limit))
   {
      data = data + offset;
      limit = length;

      success = GetNextExtension(data, &x509_ext, iter, limit);
      if (success)
      {
         /* Parse extension data */
         ParseCertificateExtension(&x509_ext, cert_ext);
      }
   }
   else
   {
      success = FALSE;
   }

   FUNCTION_FINISH(SECTK_GetNextCertificateExtension);

   return success;
}

/*!**************************************************************************
 * @brief    Return certificate extension given extension type
 * @param    cert - pointer to certificate
 * @param    type - extension type
 * @param    cert_ext - certificate extension
 * @return   TRUE if extension was found, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_GetCertificateExtension(void *cert, E_SECTK_CERT_EXT_TYPE type,
                                      S_SECTK_CERT_EXT *cert_ext)
{
   S_X509 *x509;
   const U8BIT *oid;
   U16BIT oid_len;
   BOOL success;
   U16BIT i;
   S_X509_EXT x509_ext;

   FUNCTION_START(SECTK_GetCertificateExtension);

   x509 = cert;
   success = FALSE;
   oid_len = 0;

   if (x509->extensions.data != NULL && x509->extensions.len > 0)
   {
      oid = NULL;

      /* Find extension object identifier */
      for (i = 0; i < sizeof(ext_info)/sizeof(struct ext_info); ++i)
      {
         if (ext_info[i].type == type)
         {
            oid = ext_info[i].obj_id + ext_info[i].id_offset;
            oid_len = ext_info[i].id_length;
            break;
         }
      }

      if (oid != NULL)
      {
         CC_DEBUG_PRINT(("Found object\n"));
         CC_DEBUG_BUFFER(x509->extensions.data, x509->extensions.len);

         /* Look for extension data in the certificate */
         success = GetExtensionByOid(&x509->extensions, oid, oid_len,
                                     &x509_ext);
         if (success)
         {
            CC_DEBUG_PRINT(("Found extension (critical=%s)\n",
                            x509_ext.critical ? "TRUE" : "FALSE"));
            CC_DEBUG_BUFFER(x509_ext.extnValue.data, x509_ext.extnValue.len);

            /* Parse extension data */
            ParseCertificateExtension(&x509_ext, cert_ext);
            if (cert_ext->type == type)
            {
               success = TRUE;
            }
         }
      }
   }

   FUNCTION_FINISH(SECTK_GetCertificateExtension);

   return success;
}

/*!**************************************************************************
 * @brief    Free parsed X.509 certificate
 * @param    cert - pointer to X.509 certificate
 ****************************************************************************/
void SECTK_FreeCertificate(void *cert)
{
   FUNCTION_START(SECTK_FreeCertificate);

   free(cert);

   FUNCTION_FINISH(SECTK_FreeCertificate);
}

/*!**************************************************************************
 * @brief    Terminate security module
 ****************************************************************************/
void SECTK_Terminate(void)
{
   FUNCTION_START(SECTK_Terminate);

   /*XXX*/

   FUNCTION_FINISH(SECTK_Terminate);
}
void SECTK_key_certificate_valid_period_print(U8BIT* cert_data)
{
#if 0
    /*Print logs*/
    S_X509 *x509 = (S_X509 *)cert_data;
    DTG_T t_dtg_utc;
    x_dt_utc_sec_to_dtg((TIME_T)x509->validity.notBefore,&t_dtg_utc);
    DBG_LOG_PRINT(("{MW CI}Host certification validity.notBefore**year = %d , month = %d, day = %d, hour = %d,min = %d, sec = %d******\n",t_dtg_utc.ui2_yr,t_dtg_utc.ui1_mo,t_dtg_utc.ui1_day,t_dtg_utc.ui1_hr,t_dtg_utc.ui1_min,t_dtg_utc.ui1_sec));
    x_dt_utc_sec_to_dtg((TIME_T)x509->validity.notAfter,&t_dtg_utc);
    DBG_LOG_PRINT(("{MW CI}Host certification validity.notAfter**year = %d , month = %d, day = %d, hour = %d,min = %d, sec = %d******\n",t_dtg_utc.ui2_yr,t_dtg_utc.ui1_mo,t_dtg_utc.ui1_day,t_dtg_utc.ui1_hr,t_dtg_utc.ui1_min,t_dtg_utc.ui1_sec));
    /*end print logs*/
#endif
}
/*!**************************************************************************
 * @brief    Extract OU from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] pb_product -  TRUE Production key,FALSE test key
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractOU(void *dev_cert, BOOL *pb_product)
{
   static U8BIT oid[] = { 0x55, 0x04, 0x0B };//
   S_X509 *x509 = dev_cert;
   UINT8 *data;
   UINT32 limit;
   UINT32 tag;
   UINT32 length;
   UINT32 offset;
   UINT32 *found_oid;
   UINT32 found_oid_len;
   BOOL success;
   BOOL found;

   FUNCTION_START(SECTK_ExtractOU);

   data = x509->issuer.data;
   limit = x509->issuer.len;
   offset = 0;

   success = TRUE;
   found = FALSE;
   *pb_product = FALSE;
   while (success)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SET, &length);
      if (success)
      {
         success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE,
                                 &length);
      }
      if (success)
      {
         success = MatchAsn1Item(data, &offset, limit,
                                 ASN1_TAG_OBJECT_IDENTIFIER, &length);
      }
      if (success)// can find OBJECT IDENTIFIER
      {
         found_oid = data + offset;
         found_oid_len = length;

         offset += length;
         success = GetAsn1Tag(data, &tag, &offset, limit);
      }
      if (success && (tag == ASN1_TAG_PRINTABLE_STRING ||
                      tag == ASN1_TAG_UTF8_STRING))
      {
         success = GetAsn1Length(data, &length, &offset, limit);
      }
      if (success)
      {
         if ((found_oid_len == sizeof(oid)) &&
             (memcmp(found_oid, oid, found_oid_len) == 0))
         {
            if(length == OU_TEST_LENGTH)
            {
                *pb_product = FALSE;
                dprintf("SECTK_ExtractOU host use test key\n");
            }
            else if(length == OU_PRODUCTION_LENGTH)
            {
                *pb_product = TRUE;
                dprintf("SECTK_ExtractOU host use production key\n");
            }
            else
            {
                *pb_product = TRUE;
                dprintf("SECTK_ExtractOU length error host default use production key len = %d\n",length);
            }
            found = TRUE;
            break;
         }
         else
         {
            offset += length;
         }
      }
   }

   FUNCTION_FINISH(SECTK_ExtractDeviceId);

   return found;
}

BOOL SECTK_RecognizeCertType(void)
{
   void *dev_cert;

   UINT8 *cert_data = NULL;
   UINT16 cert_len = 0;

   BOOL success;
   BOOL b_product_key = FALSE;
   FUNCTION_START(SECTK_RecognizeCertType);


  TZ_ciplus_read_cred(TZ_STB_CIPLUS_KEY_DEVICE_CERT, &cert_data, &cert_len);
  if((cert_data == NULL)||
     (cert_len == 0))
  {
      return FALSE;
  }
  dev_cert = SECTK_ParseCertificate(cert_data, cert_len);
  if (dev_cert != NULL)
  {

     success = SECTK_ExtractOU(dev_cert, &b_product_key);
     if (!success)
     {
        b_product_key = FALSE;
     }
  }
  if (dev_cert != NULL)
  {
     SECTK_FreeCertificate(dev_cert);
  }
   FUNCTION_FINISH(SECTK_RecognizeCertType);

   return b_product_key;
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Xor two blocks together
 * @param    first_block - first block
 * @param    second_block - seocnd block
 * @param    result_blcok - result block
 *
 * @note     result_block can be the same as first_block or second_block (or
 *           even both).
 ****************************************************************************/
static void XorBlock(U8BIT *first_block, U8BIT *second_block,
                     U8BIT *result_block)
{
   U16BIT offset;

   FUNCTION_START(XorBlock);

   for (offset = 0; offset < BLOCK_SIZE_BYTES; ++offset)
   {
      result_block[offset] = first_block[offset] ^ second_block[offset];
   }

   FUNCTION_FINISH(XorBlock);
}

/*!**************************************************************************
 * @brief    Update monotonic counter
 ****************************************************************************/
static void UpdateCounter(void)
{
   U16BIT offset = BLOCK_SIZE_BYTES;

   FUNCTION_START(UpdateCounter);

   do
   {
      --offset;
      prng.dt_i[offset] += 1;
   } while ((offset > 0) && (prng.dt_i[offset] == 0));

   FUNCTION_FINISH(UpdateCounter);
}

/*!**************************************************************************
 * @brief    Generate a random block (128 bits)
 * @param    [out] block - output block
 ****************************************************************************/
static void GenerateRandomBlock(U8BIT *block)
{
   S_AES128_KEY aes_key;
   U8BIT i_i[BLOCK_SIZE_BYTES];
   U8BIT t_i[BLOCK_SIZE_BYTES];

   FUNCTION_START(GenerateRandomBlock);

   CE_Aes128SetEncryptKey(prng.k, &aes_key);

   /* Ii = E_AES-128{k}(DTi) */
   CE_Aes128Encrypt(prng.dt_i, &aes_key, i_i);
   UpdateCounter();

   /* Ri = E_AES-128{k}(Ii + Si) */
   XorBlock(i_i, prng.s_i, t_i);
   CE_Aes128Encrypt(t_i, &aes_key, block);

   /* S_i+1 = E_AES(k}(Ii + Ri) */
   XorBlock(i_i, block, t_i);
   CE_Aes128Encrypt(t_i, &aes_key, prng.s_i);

   /* Clear variables on the stack */
   memset(&aes_key, 0xbd, sizeof aes_key);
   memset(i_i, 0xbd, sizeof i_i);
   memset(t_i, 0xbd, sizeof t_i);

   FUNCTION_FINISH(GenerateRandomBlock);
}

/*!**************************************************************************
 * @brief    Append block to buffer
 * @param    buffer - the buffer
 * @param    length - total length of buffer in bytes
 * @param    offset - offset for block
 * @param    block - the block to append (128 bits)
 * @return   TRUE if the block was appended without truncation, FALSE otherwise
 ****************************************************************************/
static BOOL AppendBlock(U8BIT *buffer, U16BIT length, U16BIT offset,
                           U8BIT *block)
{
   U16BIT block_offset;
   U16BIT to_copy;
   BOOL not_truncated;

   FUNCTION_START(AppendBlock);

   /* Truncation leaves the LSB of the block and throws away the rest */
   if (offset + BLOCK_SIZE_BYTES <= length)
   {
      to_copy = BLOCK_SIZE_BYTES;
      block_offset = 0;
      not_truncated = TRUE;
   }
   else
   {
      to_copy = length - offset;
      block_offset = BLOCK_SIZE_BYTES - to_copy;
      not_truncated = FALSE;
   }
   memcpy(buffer + offset, block + block_offset, to_copy);

   /* Clear variables on the stack */
   block_offset = to_copy = 0xbdbd;

   FUNCTION_FINISH(AppendBlock);

   return not_truncated;
}

/*!**************************************************************************
 * @brief    Check Diffie-Hellman public key
 *
 * This function checks the following conditions for DH public key DH_pub:
 * -# 1 < DH_pub < DH_p
 * -# DH_pub ^ DH_q mod DH_p = 1
 *
 * @param    dh_pub - Diffie-Hellman public key (2048 bits)
 * @param    dh_p - Diffie-Hellman modulus (2048 bits)
 * @param    dh_q - Diffie-Hellman "Sophie Germain" constant (256 bits)
 * @return   TRUE if DH public key is valid, FALSE otherwise
 ****************************************************************************/
static BOOL CheckDhPublicKey(U8BIT *dh_pub, U8BIT *dh_p, U8BIT *dh_q)
{
   U8BIT dh_res[DH_SIZE];
   BOOL valid;

   FUNCTION_START(CheckDhPublicKeys);

   valid = TRUE;

   /* Check 1 < DH_pub < DH_p */
   if (memcmp(dh_pub, dh_zero, DH_SIZE) == 0)
   {
      valid = FALSE;
   }
   if (valid && memcmp(dh_pub, dh_one, DH_SIZE) == 0)
   {
      valid = FALSE;
   }

   /* Check DH_pub ^ DH_q mod DH_p = 1 */
   if (valid)
   {
      CC_ModExp(dh_pub, DH_P_SIZE, dh_q, DH_Q_SIZE, dh_p, DH_P_SIZE, dh_res);
      if (memcmp(dh_res, dh_one, DH_SIZE) != 0)
      {
         valid = FALSE;
      }
   }

   /* Clear variables on the stack */
   memset(dh_res, 0xbd, sizeof dh_res);

   FUNCTION_FINISH(CheckDhPublicKeys);

   return valid;
}

/*!**************************************************************************
 * @brief    Sign a buffer using RSASSA-PSS (with SHA-1 and MGF1/SHA-1)
 * @param    buffer - the buffer to sign
 * @param    length - length of buffer in bytes
 * @param    key - the RSA key
 * @param    signature - the signature
 * @return   TRUE if the signature was calcluated, FALSE otherwise
 ****************************************************************************/
static BOOL SignBuffer(U8BIT *buffer, U16BIT length, S_RSA_KEY *key,
                          U8BIT *signature)
{
   U32BIT modBits;
   U32BIT emBits;
   U16BIT emLen;
   U8BIT *EM;
   BOOL success;

   FUNCTION_START(SignBuffer);

   success = FALSE;

   modBits = key->modulus.len * 8;
   emBits = modBits - 1;
   emLen = (emBits + 7) / 8;

   CC_DEBUG_PRINT(("modBits = %ld, emBits = %ld, emLen = %d\n",
                   modBits, emBits, emLen));

   EM = malloc(emLen);
   if (EM != NULL)
   {
      CC_DEBUG_PRINT(("buffer:\n"));
      CC_DEBUG_BUFFER(buffer, length);
      success = EmsaPssEncode(buffer, length, EM, emBits);
      CC_DEBUG_PRINT(("EM:\n"));
      CC_DEBUG_BUFFER(EM, emLen);

      CC_ModExp(EM, emLen,
                key->privateExponent.data, key->privateExponent.len,
                key->modulus.data, key->modulus.len,
                signature);

      memset(EM, 0xbd, emLen);
      free(EM);
   }

   FUNCTION_FINISH(SignBuffer);

   return success;
}

/*!**************************************************************************
 * @brief    Perform EMSA-PSS-ENCODE operation as described in PKCS#1 v2.1
 * @param    buffer - buffer to encode
 * @param    length - length of buffer in bytes
 * @param    EM - encoded message
 * @param    emBits - length of encoded message in bits
 * @return   TRUE if message was encoded successfully, FALSE otherwise
 ****************************************************************************/
static BOOL EmsaPssEncode(U8BIT *buffer, U16BIT length, U8BIT *EM,
                             U32BIT emBits)
{
   enum { hLen = CE_SHA1_DIGEST_SIZE };
   enum { sLen = SALT_LEN };

   U16BIT emLen;
   U8BIT salt[sLen];
   U8BIT M_tag[8 + hLen + sLen];
   U8BIT H[hLen];
   U16BIT PSLen, i;
   BOOL success;
   U8BIT zeroMask[] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x3, 0x1 };

   FUNCTION_START(EmsaPssEncode);

   memset (salt, 0x0, sLen*sizeof(U8BIT)); /* fix klocwork warnings */

   emLen = (emBits + 7) / 8;

   success = FALSE;
   if (emLen >= hLen + sLen + 2)
   {
      /* Generate salt */
      SECTK_GenerateRandomSequence(salt, sLen);

      CC_DEBUG_PRINT(("salt:\n"));
      CC_DEBUG_BUFFER(salt, sLen);
      
      /* M' = (0x) 00 00 00 00 00 00 00 00 || mHash || salt
       *
       * mHash is calculated directly into M'
       */
      memset(M_tag, 0, 8);
      CE_Sha1(buffer, length, M_tag + 8);
      memcpy(M_tag + 8 + hLen, salt, sLen);

      CC_DEBUG_PRINT(("M'\n"));
      CC_DEBUG_BUFFER(M_tag, 8 + hLen + sLen);

      /* H = Hash(M') */
      CE_Sha1(M_tag, 8 + hLen + sLen, H);

      CC_DEBUG_PRINT(("H\n"));
      CC_DEBUG_BUFFER(H, hLen);

      /* dbMask = MGF(H, emLen - hLen - 1)
       *
       * MGF is calculated directly into EM
       */
      Mgf1(H, emLen - hLen - 1, EM);

      /* DB = PS || 0x01 || salt
       * maskedDB = DB xor dbMask
       *
       * dbMask is already in EM, need to xor DB. No need to xor 0x00...
       */
      PSLen = emLen - sLen - hLen - 2;
      EM[PSLen] ^= 0x01;
      for (i = 0; i < sLen; ++i)
      {
         EM[PSLen + 1 + i] ^= salt[i];
      }

      /* EM = maskedDB || H || 0xbc */
      memcpy(EM + emLen - hLen - 1, H, hLen);
      EM[emLen-1] = 0xbc;

      /* Set the leftmost 8emLen - emBits bits of the leftmost octet in
       * maskedDB to zero.
       */
      if((8*emLen - emBits)<8)
      {
          EM[0] &= zeroMask[8*emLen - emBits];
      }
      CC_DEBUG_PRINT(("EM\n"));
      CC_DEBUG_BUFFER(EM, emLen);

      success = TRUE;
   }

   /* Clear variables on the stack */
   memset(salt, 0xbd, sizeof salt);
   memset(M_tag, 0xbd, sizeof M_tag);
   memset(H, 0xbd, sizeof H);

   FUNCTION_FINISH(EmsaPssEncode);

   return success;
}

/*!**************************************************************************
 * @brief    Verify a buffer using RSASSA-PSS (with SHA-1 and MGF1/SHA-1)
 * @param    buffer - the buffer to verify
 * @param    length - length of buffer in bytes
 * @param    key - the RSA key
 * @param    signature - the signature
 * @param    signature_len - length of signature in bytes
 * @return   TRUE if the signature was calcluated, FALSE otherwise
 ****************************************************************************/
static BOOL VerifyBuffer(U8BIT *buffer, U16BIT length, S_RSA_KEY *key,
                            U8BIT *signature, U16BIT signature_len)
{
   U32BIT modBits;
   U32BIT emBits;
   U16BIT emLen;
   U8BIT *EM;
   BOOL success;

   FUNCTION_START(VerifyBuffer);

   success = FALSE;

   modBits = key->modulus.len * 8;
   emBits = modBits - 1;
   emLen = (emBits + 7) / 8;

   CC_DEBUG_PRINT(("modBits = %ld, emBits = %ld, emLen = %d\n",
                   modBits, emBits, emLen));

   EM = malloc(emLen);
   if (EM != NULL)
   {
      CC_ModExp(signature, signature_len,
                key->publicExponent.data, key->publicExponent.len,
                key->modulus.data, key->modulus.len,
                EM);

      CC_DEBUG_PRINT(("EM:\n"));
      CC_DEBUG_BUFFER(EM, emLen);
      success = EmsaPssVerify(buffer, length, EM, emBits);

      memset(EM, 0xbd, emLen);
      free(EM);
   }

   FUNCTION_FINISH(VerifyBuffer);

   return success;
}

/*!**************************************************************************
 * @brief    Perform EMSA-PSS-VERIFY operation as described in PKCS#1 v2.1
 * @param    buffer - buffer to encode
 * @param    length - length of buffer in bytes
 * @param    EM - encoded message
 * @param    emBits - length of encoded message in bits
 * @return   TRUE if message was encoded successfully, FALSE otherwise
 ****************************************************************************/
static BOOL EmsaPssVerify(U8BIT *buffer, U16BIT length, U8BIT *EM,
                             U32BIT emBits)
{
   enum { hLen = CE_SHA1_DIGEST_SIZE };
   enum { sLen = SALT_LEN };

   U16BIT emLen;
   U8BIT *salt;
   U8BIT H[hLen];
   U8BIT M_tag[8 + hLen + sLen];
   U8BIT H_tag[hLen];
   U16BIT i;
   BOOL success;
   U8BIT zeroMask[] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x3, 0x1 };

   FUNCTION_START(EmsaPssEncode);

   emLen = (emBits + 7) / 8;

   success = FALSE;
   if ((emLen >= hLen + sLen + 2) &&
       (EM[emLen-1] == 0xbc) &&
       (8*emLen - emBits)<8 &&
       (EM[0] & zeroMask[8*emLen - emBits]) == EM[0])
   {
      memcpy(H, EM + emLen - hLen - 1, hLen);

      CC_DEBUG_PRINT(("H (part of EM):\n"));
      CC_DEBUG_BUFFER(H, hLen);

      /* dbMask = MGF(H, emLen - hLen - 1)
       * DB = maskedDB xor dbMask
       *
       * Instead of calculating dbMask and then xor it with maskedDB, we
       * do both at the same time.
       */
      Mgf1Xor(H, emLen - hLen - 1, EM);

      CC_DEBUG_PRINT(("DB (first %d bytes):\n", emLen - hLen - 1));
      CC_DEBUG_BUFFER(EM, emLen - hLen - 1);

      /* Set the leftmost 8emLen - emBits bits of the leftmost octet in
       * maskedDB to zero.
       */
      if((8*emLen - emBits)<8)
      {
          EM[0] &= zeroMask[8*emLen - emBits];
      }

      /* If the emLen-hLen-sLen-2 leftmost octets of DB are not zero or if the
       * octet at position emLen-hLen-sLen-1 (the leftmost position is
       * "position 1") does not have hexadecimal value 0x01, output
       * "inconsistent" and stop.
       */
      success = TRUE;
      for (i = 0; success && i < emLen - hLen - sLen - 2; ++i)
      {
         if (EM[i] != 0x00)
         {
            success = FALSE;
         }
      }
      if (success && EM[emLen-hLen-sLen-2] != 0x01)
      {
         success = FALSE;
      }

      if (success)
      {
         salt = EM + emLen - hLen - sLen - 1;
         CC_DEBUG_PRINT(("salt:\n"));
         CC_DEBUG_BUFFER(salt, sLen);

         /* M' = (0x) 00 00 00 00 00 00 00 00 || mHash || salt
          *
          * mHash is calculated directly into M'
          * salt is copied into M'
          */
         memset(M_tag, 0, 8);
         CE_Sha1(buffer, length, M_tag + 8);
         memcpy(M_tag + 8 + hLen, salt, sLen);

         CC_DEBUG_PRINT(("M'\n"));
         CC_DEBUG_BUFFER(M_tag, 8 + hLen + sLen);

         /* H' = Hash(M') */
         CE_Sha1(M_tag, 8 + hLen + sLen, H_tag);

         CC_DEBUG_PRINT(("H'\n"));
         CC_DEBUG_BUFFER(H_tag, hLen);

         if (memcmp(H, H_tag, hLen) != 0)
         {
            success = FALSE;
         }
      }
   }

   /* Clear variables on the stack */
   memset(M_tag, 0xbd, sizeof M_tag);
   memset(H, 0xbd, sizeof H);
   memset(H_tag, 0xbd, sizeof H_tag);

   FUNCTION_FINISH(EmsaPssEncode);

   return success;
}

/*!**************************************************************************
 * @brief    Calculate MGF1 as specified in PKCS#1 v2.1
 * @param    mgfSeed - seed from which mask is generated
 * @param    maskLen - intended length in cotets of the mask
 * @param    mask - mask, an octet string of length maskLen
 ****************************************************************************/
static void Mgf1(U8BIT *mgfSeed, U16BIT maskLen, U8BIT *mask)
{
   enum { hLen = CE_SHA1_DIGEST_SIZE };

   U8BIT C[hLen + 4];
   U32BIT counter, limit, offset;
   U8BIT H[hLen];

   FUNCTION_START(Mgf1);

   memcpy(C, mgfSeed, hLen);

   limit = (maskLen + hLen - 1) / hLen;
   for (counter = 0, offset = 0;
        counter < limit && maskLen > 0;
        ++counter, offset += hLen, maskLen -= hLen)
   {
      C[hLen+0] = (U8BIT)(counter >> 24);
      C[hLen+1] = (U8BIT)(counter >> 16);
      C[hLen+2] = (U8BIT)(counter >> 8);
      C[hLen+3] = (U8BIT)counter;

      CE_Sha1(C, hLen + 4, H);
      if (maskLen >= hLen)
      {
         memcpy(mask + offset, H, hLen);
      }
      else if (maskLen > 0)
      {
         memcpy(mask + offset, H, maskLen);
      }
   }

   /* Clear variables on the stack */
   memset(C, 0xbd, sizeof C);
   memset(H, 0xbd, sizeof H);

   FUNCTION_FINISH(Mgf1);
}

/*!**************************************************************************
 * @brief    Calculate MGF1 as specified in PKCS#1 v2.1 and xor it with
 *           the given mask
 * @param    mgfSeed - seed from which mask is generated
 * @param    maskLen - intended length in cotets of the mask
 * @param    mask - mask, an octet string of length maskLen
 ****************************************************************************/
static void Mgf1Xor(U8BIT *mgfSeed, U16BIT maskLen, U8BIT *mask)
{
   enum { hLen = CE_SHA1_DIGEST_SIZE };

   U8BIT C[hLen + 4];
   U32BIT counter, limit, offset;
   U8BIT H[hLen];
   U16BIT i;

   FUNCTION_START(Mgf1);

   memcpy(C, mgfSeed, hLen);

   limit = (maskLen + hLen - 1) / hLen;
   for (counter = 0, offset = 0;
        counter < limit && maskLen > 0;
        ++counter, offset += hLen, maskLen -= hLen)
   {
      C[hLen+0] = (U8BIT)(counter >> 24);
      C[hLen+1] = (U8BIT)(counter >> 16);
      C[hLen+2] = (U8BIT)(counter >> 8);
      C[hLen+3] = (U8BIT)counter;

      CE_Sha1(C, hLen + 4, H);
      if (maskLen >= hLen)
      {
         for (i = 0; i < hLen; ++i)
         {
            mask[offset+i] ^= H[i];
         }
      }
      else if (maskLen > 0)
      {
         for (i = 0; i < maskLen; ++i)
         {
            mask[offset+i] ^= H[i];
         }
      }
   }

   /* Clear variables on the stack */
   memset(C, 0xbd, sizeof C);
   memset(H, 0xbd, sizeof H);

   FUNCTION_FINISH(Mgf1);
}

/*!**************************************************************************
 * @brief    Parse X.509 certificate in DER format
 * @param    x509 - X.509 certificate
 * @return   TRUE if certificate was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseCertificate(S_X509 *x509)
{
   U8BIT *data;
   U32BIT limit;
   BOOL success;
   U32BIT length;
   U32BIT offset;

   FUNCTION_START(ParseCertificate);

   data = x509->data;
   limit = x509->len;

   CC_DEBUG_PRINT(("x509 data:\n"));
   CC_DEBUG_BUFFER(x509->data, x509->len);

   offset = 0;
   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);

   /* Parse tbsCertificate */
   if (success)
   {
      success = ParseTbsCertificate(x509, &offset);
   }

   /* Parse signatureAlgorithm */
   if (success)
   {
      success = ParseSignatureAlgorithm(x509, &offset);
   }

   /* Parse signatureValue */
   if (success)
   {
      success = ParseSignatureValue(x509, &offset);
   }

   FUNCTION_FINISH(ParseCertificate);

   return success;
}

/*!**************************************************************************
 * @brief    Parse X.509 tbsCertificate section
 * @param    x509 - X.509 certificate
 * @param    offset - offset of section
 * @return   TRUE if tbsCertificate was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseTbsCertificate(S_X509 *x509, U32BIT *offset)
{
   U8BIT *data;
   U32BIT limit;
   BOOL success;
   U32BIT length;
   U32BIT pos = *offset;

   FUNCTION_START(ParseTbsCertificate);

   data = x509->data;
   limit = x509->len;

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      x509->tbsCertificate.data = data + pos;
      x509->tbsCertificate.len = *offset - pos + length;
      limit = *offset + length;

      success = VerifyCertificateVersion(data, offset, limit);
   }
   if (success)
   {
      success = ParseSerialNumber(data, offset, limit, &x509->serialNumber);
   }
   if (success)
   {
      success = ParseSignature(data, offset, limit, &x509->signature);
   }
   if (success)
   {
      success = ParseIssuer(data, offset, limit, &x509->issuer);
   }
   if (success)
   {
      success = ParseValidity(data, offset, limit, &x509->validity);
   }
   if (success)
   {
      success = ParseSubject(data, offset, limit, &x509->subject);
   }
   if (success)
   {
      success = ParseSubjectPublicKeyInfo(data, offset, limit,
                                          &x509->subjectPublicKeyInfo);
   }
   if (success)
   {
      success = ParseExtensions(data, offset, limit, &x509->extensions);
   }

   FUNCTION_FINISH(ParseTbsCertificate);

   return success;
}

/*!**************************************************************************
 * @brief    Verify that the certificate is X.509v3
 * @param    data - version data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @return   TRUE if certificate version is 3, FALSE otherwise
 ****************************************************************************/
static BOOL VerifyCertificateVersion(U8BIT *data, U32BIT *offset,
                                        U32BIT limit)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(VerifyCertificateVersion);

   /* Since CI+ uses X.509v3, the version field must be included as
    * the default (v1) cannot be used.
    */
   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_VERSION, &length);
   if (success)
   {
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_INTEGER, &length);
   }
   if (success && (length != 1 || data[*offset] != 0x2))
   {
      success = FALSE;
   }
   if (success)
   {
      *offset += length;
      CC_DEBUG_PRINT(("Certificate version verified\n"));
   }

   FUNCTION_FINISH(VerifyCertificateVersion);

   return success;
}

/*!**************************************************************************
 * @brief    Parse certificate serial number
 * @param    data - serial number data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    serialNumber - serial number
 * @return   TRUE if serial number was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseSerialNumber(U8BIT *data, U32BIT *offset, U32BIT limit,
                                 S_BUFFER *serialNumber)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseSerialNumber);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_INTEGER, &length);
   if (success && *offset + length > limit)
   {
      success = FALSE;
   }
   if (success)
   {
      serialNumber->data = data + *offset;
      serialNumber->len = length;
      *offset += length;

      CC_DEBUG_PRINT(("serialNumber:\n"));
      CC_DEBUG_BUFFER(serialNumber->data, serialNumber->len);
   }

   FUNCTION_FINISH(ParseSerialNumber);

   return success;
}

/*!**************************************************************************
 * @brief    Parse certificate signature (in tbsCertificate section)
 * @param    data - signature data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    signature - the signature
 * @return   TRUE if signature was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseSignature(U8BIT *data, U32BIT *offset, U32BIT limit,
                              S_BUFFER *signature)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseSignature);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      signature->data = data + *offset;
      signature->len = length;
      success = VerifySignatureAlgorithm(signature);
   }
   if (success)
   {
      *offset += length;
      CC_DEBUG_PRINT(("signature:\n"));
      CC_DEBUG_BUFFER(signature->data, signature->len);
   }

   FUNCTION_FINISH(ParseSignature);

   return success;
}

/*!**************************************************************************
 * @brief    Parse certificate issuer (in tbsCertificate section)
 * @param    data - issuer data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    issuer - the issuer
 * @return   TRUE if issuer was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseIssuer(U8BIT *data, U32BIT *offset, U32BIT limit,
                           S_BUFFER *issuer)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseIssuer);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      issuer->data = data + *offset;
      issuer->len = length;
      *offset += length;

      CC_DEBUG_PRINT(("issuer:\n"));
      CC_DEBUG_BUFFER(issuer->data, issuer->len);
   }

   FUNCTION_FINISH(ParseIssuer);

   return success;
}

/*!**************************************************************************
 * @brief    Parse certificate validity (in tbsCertificate section)
 * @param    data - validity data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    validity - the certificate validity
 * @return   TRUE if validity was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseValidity(U8BIT *data, U32BIT *offset, U32BIT limit,
                             S_VALIDITY *validity)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseValidity);

   validity->notBefore = 0;
   validity->notAfter = 0;

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      limit = *offset + length;
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_UTCTIME, &length);
   }
   if (success)
   {
      success = ConvertUTCTimeToSeconds(data + *offset, length,
                                        &validity->notBefore);
   }
   if (success)
   {
      *offset += length;
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_UTCTIME, &length);
   }
   if (success)
   {
      success = ConvertUTCTimeToSeconds(data + *offset, length,
                                        &validity->notAfter);
   }
   if (success)
   {
      *offset += length;
   }

   if (*offset != limit)
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseValidity);

   return success;
}

/*!**************************************************************************
 * @brief    Convert UTCTime value to seconds since the epoch
 * @param    data - UTCTime value
 * @param    length - length of value in bytes
 * @param    seconds - seconds since the epoch (epoch=1.1.1970)
 * @return   TRUE if value was converted successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ConvertUTCTimeToSeconds(U8BIT *data, U32BIT length,
                                       U32BIT *seconds)
{
   static U16BIT MONTH_DAYS[] = {
      0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
   };
                                  
   U16BIT year, month, day, hour, minute, second;
   U16BIT days;
   U16BIT i;
   BOOL success;
   BOOL leap;

   FUNCTION_START(ConvertUTCTimeToSeconds);

   /* Format must be YYMMDDHHMMSSZ */
   success = TRUE;
   if (length == 13 && data[12] == 'Z')
   {
      for (i = 0; success && i < 12; ++i)
      {
         if (data[i] < '0' || data[i] > '9')
         {
            success = FALSE;
         }
      }
   }
   if (success)
   {
      year = (data[0] - '0') * 10 + (data[1] - '0');
      month = (data[2] - '0') * 10 + (data[3] - '0');
      day = (data[4] - '0') * 10 + (data[5] - '0');
      hour = (data[6] - '0') * 10 + (data[7] - '0');
      minute = (data[8] - '0') * 10 + (data[9] - '0');
      second = (data[10] - '0') * 10 + (data[11] - '0');

      year += 2000;

      /* Check leap-year - no need for full condition, the only year
       * divisible by 100 is also divisible by 400 (since 1970 until 2100).
       */
      leap = FALSE;
      if (year % 4 == 0)
      {
         leap = TRUE;
      }

      if ((month < 1 || month > 12) ||
          (day < 1 || day > MONTH_DAYS[month]) ||
          (hour > 23) || (minute > 59) || (second > 59) ||
          (!leap && month == 2 && day == 29))
      {
         success = FALSE;
      }
      
   }
   if (success)
   {
      if (year >= 2038)
      {
         /* epoch overflow, give up */
         *seconds = 2147483647;
      }
      else
      {
         /* Number of days in past years */
         days = (year - 1970) * 365;

         /* Add leap days (number of years divisible by 4) */
         days += ((year-1) - 1970 + 2) / 4;

         /* Add days for current year */
         for (i = 1; i < month; ++i)
         {
            days += MONTH_DAYS[i];
         }
         days += (day - 1);

         /* Remove leap day if not required */
         if (month > 2 && year % 4 != 0)
         {
            --days;
         }

         *seconds = days * 86400 + hour * 3600 + minute * 60 + second;
      }
      CC_DEBUG_PRINT(("UTCTime %.*s: %ld seconds\n",
                      (int)length, data, *seconds));
   }

   FUNCTION_FINISH(ConvertUTCTimeToSeconds);

   return success;
}

/*!**************************************************************************
 * @brief    Parse certificate subject (in tbsCertificate section)
 * @param    data - subject data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    subject - the subject
 * @return   TRUE if subject was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseSubject(U8BIT *data, U32BIT *offset, U32BIT limit,
                            S_BUFFER *subject)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseSubject);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      subject->data = data + *offset;
      subject->len = length;
      *offset += length;

      CC_DEBUG_PRINT(("subject:\n"));
      CC_DEBUG_BUFFER(subject->data, subject->len);
   }

   FUNCTION_FINISH(ParseSubject);

   return success;
}

/*!**************************************************************************
 * @brief    Parse subject public key info
 * @param    data - subject public key info data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    subjectPublicKeyInfo - the subject public key information
 * @return   TRUE if public key info was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseSubjectPublicKeyInfo(U8BIT *data, U32BIT *offset,
                                         U32BIT limit, S_KEY_INFO *subjectPublicKeyInfo)
{
   static U8BIT rsaEncryption[] = {
      0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
      0x01, 0x01, 0x01, 0x05, 0x00
   };
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseSubjectPublicKeyInfo);

   /* Clear public key */
   ClearRsaKey(&subjectPublicKeyInfo->key);

   /* Parse subjectPublicKeyInfo */
   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      limit = *offset + length;
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
      if (success)
      {
         /* Make sure the algorithm is RSA and parameter is NULL */
         if (memcmp(data + *offset, rsaEncryption, length) == 0)
         {
            *offset += length;
         }
         else
         {
            success = FALSE;
         }
      }
   }
   if (success)
   {
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_BIT_STRING,
                              &length);
      if (success && (*offset + length != limit))
      {
         success = FALSE;
      }
   }
   if (success)
   {
      /* The BIT STRING actually contains (RFC3279):
       *
       *   RSAPublicKey ::= SEQUENCE {
       *      modulus            INTEGER,    -- n
       *      publicExponent     INTEGER  }  -- e
       *
       * However, we first have to skip the first byte which must be 0x00
       * (definition of BIT STRING).
       */
      if (length > 0 && data[*offset] == 0x00)
      {
         ++(*offset);
         subjectPublicKeyInfo->bitstring.data = data + *offset;
         subjectPublicKeyInfo->bitstring.len = length - 1;
      }
      else
      {
         success = FALSE;
      }
   }
   if (success)
   {
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
      if (success && (*offset + length != limit))
      {
         success = FALSE;
      }
   }
   if (success)
   {
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_INTEGER, &length);
      if (success)
      {
         /* Store RSA modulus */
         if (length > 1 && length <= RSA_MOD_SIZE + 1 && data[*offset] == 0x00)
         {
            subjectPublicKeyInfo->key.modulus.data = data + *offset + 1;
            subjectPublicKeyInfo->key.modulus.len = length - 1;
            *offset += length;
         }
         else if (length > 0 && length <= RSA_MOD_SIZE)
         {
            subjectPublicKeyInfo->key.modulus.data = data + *offset;
            subjectPublicKeyInfo->key.modulus.len = length;
            *offset += length;
         }
         else
         {
            success = FALSE;
         }
      }
   }
   if (success)
   {
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_INTEGER, &length);
      if (success)
      {
         /* Store public exponent */
         if (length > 1 && data[*offset] == 0x00)
         {
            subjectPublicKeyInfo->key.publicExponent.data = data + *offset + 1;
            subjectPublicKeyInfo->key.publicExponent.len = length - 1;
            *offset += length;
         }
         else if (length > 0)
         {
            subjectPublicKeyInfo->key.publicExponent.data = data + *offset;
            subjectPublicKeyInfo->key.publicExponent.len = length;
            *offset += length;
         }
         else
         {
            success = FALSE;
         }
      }
   }

   FUNCTION_FINISH(ParseSubjectPublicKeyInfo);

   return success;
}

/*!**************************************************************************
 * @brief    Parse certificate extensions (in tbsCertificate section)
 * @param    data - extensions data (DER encoded)
 * @param    offset - offset in data
 * @param    limit - maximum offset
 * @param    extensions - the X509v3 extensions
 * @return   TRUE if extensions were parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseExtensions(U8BIT *data, U32BIT *offset, U32BIT limit,
                               S_BUFFER *extensions)
{
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseExtensions);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_EXTENSIONS, &length);
   if (success && (*offset + length != limit))
   {
      success = FALSE;
   }
   if (success)
   {
      extensions->data = data + *offset;
      extensions->len = length;
      *offset += length;

      CC_DEBUG_PRINT(("extensions:\n"));
      CC_DEBUG_BUFFER(extensions->data, extensions->len);
   }

   FUNCTION_FINISH(ParseExtensions);

   return success;
}

/*!**************************************************************************
 * @brief    Return X.509v3 extension (raw) given OID
 * @param    extensions - extensions data (DER encoded)
 * @param    oid - the OID (encoded)
 * @param    oid_len - length of encoded OID in bytes
 * @param    ext - the X509v3 extension, NULL/0 if not found
 * @return   TRUE if extension was found, FALSE otherwise
 ****************************************************************************/
static BOOL GetExtensionByOid(S_BUFFER *extensions, const U8BIT *oid,
                                 U16BIT oid_len, S_X509_EXT *ext)
{
   U8BIT *data;
   U32BIT limit;
   U32BIT offset;
   BOOL success;
   U32BIT length;

   FUNCTION_START(GetExtensionByOid);

   data = extensions->data;
   limit = extensions->len;

   offset = 0;
   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (offset + length == limit))
   {
      data = data + offset;
      limit = length;
      offset = 0;
   }
   else
   {
      success = FALSE;
   }
   while (success)
   {
      success = GetNextExtension(data, ext, &offset, limit);
      if (success)
      {
         if ((ext->extnID.len == oid_len) &&
             (memcmp(ext->extnID.data, oid, oid_len) == 0))
         {
            break;
         }
      }
   }

   FUNCTION_FINISH(GetExtensionByOid);

   return success;
}

/*!**************************************************************************
 * @brief    Return next X.509v3 extension given OID
 * @param    data - extensions data (DER encoded)
 * @param    ext - the X509v3 extension
 * @param    offset - offset in data
 * @param    limit - offset limit
 * @return   TRUE if extension was returned, FALSE otherwise
 ****************************************************************************/
static BOOL GetNextExtension(U8BIT *data, S_X509_EXT *ext, U32BIT *offset,
                                U32BIT limit)
{
   BOOL success;
   U32BIT tag;
   U32BIT length;

   FUNCTION_START(GetNextExtension);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      limit = *offset + length;
      success = MatchAsn1Item(data, offset, limit, ASN1_TAG_OBJECT_IDENTIFIER,
                              &length);
      if (success)
      {
         ext->extnID.data = data + *offset;
         ext->extnID.len = length;
         ext->critical = FALSE;
         *offset += length;
      }
   }
   if (success)
   {
      success = GetAsn1Tag(data, &tag, offset, limit);
   }
   if (success && tag == ASN1_TAG_BOOLEAN)
   {
      /* BOOL DEFAULT FALSE */
      success = GetAsn1Length(data, &length, offset, limit);
      if (success && length == 1)
      {
         ext->critical = (data[*offset] != 0) ? TRUE : FALSE;
         *offset += 1;
         success = GetAsn1Tag(data, &tag, offset, limit);
      }
      else
      {
         success = FALSE;
      }
   }
   if (success && tag == ASN1_TAG_OCTET_STRING)
   {
      /* OCTET STRING */
      success = GetAsn1Length(data, &length, offset, limit);
      if (success && (*offset + length == limit))
      {
         ext->extnValue.data = data + *offset;
         ext->extnValue.len = length;
         *offset += length;

         CC_DEBUG_PRINT(("GetNextExtension:\n"));
         CC_DEBUG_PRINT(("extnID:\n"));
         CC_DEBUG_BUFFER(ext->extnID.data, ext->extnID.len);
         CC_DEBUG_PRINT(("critical: %s\n", ext->critical ? "TRUE" : "FALSE"));
         CC_DEBUG_PRINT(("extnValue:\n"));
         CC_DEBUG_BUFFER(ext->extnValue.data, ext->extnValue.len);
      }
      else
      {
         success = FALSE;
      }
   }

   FUNCTION_FINISH(GetNextExtension);

   return success;
}

/*!**************************************************************************
 * @brief    Tell whether a certificate extension is critical
 * @param    ext_buffer - buffer holding raw extension data
 * @param    cert_ext - pointer to certificate extension
 * @return   TRUE if extension is critical, FALSE otherwise
 ****************************************************************************/
static void ParseCertificateExtension(S_X509_EXT *x509_ext,
                                      S_SECTK_CERT_EXT *cert_ext)
{
   BOOL success;

   FUNCTION_START(ParseCertificateExtension);

   cert_ext->critical = x509_ext->critical;
   cert_ext->type = GetExtensionType(x509_ext);
   switch (cert_ext->type)
   {
   case SECTK_CERT_EXT_KEY_USAGE:
      success = ParseKeyUsage(x509_ext, cert_ext);
      break;
   case SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER:
      success = ParseSubjectKeyIdentifier(x509_ext, cert_ext);
      break;
   case SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER:
      success = ParseAuthorityKeyIdentifier(x509_ext, cert_ext);
      break;
   case SECTK_CERT_EXT_BASIC_CONSTRAINTS:
      success = ParseBasicConstraints(x509_ext, cert_ext);
      break;
   case SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES:
      success = ParseScramblerCapabilities(x509_ext, cert_ext);
      break;
   case SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER:
      success = ParseCicamBrandIdentifier(x509_ext, cert_ext);
      break;
   case SECTK_CERT_EXT_CI_PLUS_INFO:
      success = ParseCiPlusInfo(x509_ext, cert_ext);
      break;
   default:
      CC_DEBUG_PRINT(("type = %d\n", cert_ext->type));
      CC_DEBUG_BUFFER(x509_ext->extnValue.data, x509_ext->extnValue.len);
      success = FALSE;
      ;
   }

   if (!success)
   {
      cert_ext->type = SECTK_CERT_EXT_UNKNOWN;
   }

   FUNCTION_FINISH(ParseCertificateExtension);
}

/*!**************************************************************************
 * @brief    Return certificate extension type
 * @param    x509_ext - pointer to extension
 * @return   Extension type, or SECTK_CERT_EXT_UNKNOWN if unknown
 ****************************************************************************/
static E_SECTK_CERT_EXT_TYPE GetExtensionType(S_X509_EXT *x509_ext)
{
   E_SECTK_CERT_EXT_TYPE type;
   U16BIT i;

   FUNCTION_START(GetExtensionType);

   type = SECTK_CERT_EXT_UNKNOWN;

   for (i = 0; i < sizeof(ext_info) / sizeof(struct ext_info); ++i)
   {
      if ((ext_info[i].id_length == x509_ext->extnID.len) &&
          (memcmp(ext_info[i].obj_id + ext_info[i].id_offset,
                  x509_ext->extnID.data, ext_info[i].id_length) == 0))
      {
         type = ext_info[i].type;
         break;
      }
   }

   FUNCTION_FINISH(GetExtensionType);

   return type;
}

/*!**************************************************************************
 * @brief    Parse key usage extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseKeyUsage(S_X509_EXT *x509_ext, S_SECTK_CERT_EXT *cert_ext)
{
   U8BIT *data;
   BOOL success;
   U32BIT offset;
   U32BIT limit;
   U32BIT length;
   S16BIT bitcount;
   BOOL b_value;

   FUNCTION_START(ParseKeyUsage);

   data = x509_ext->extnValue.data;
   limit = x509_ext->extnValue.len;
   offset = 0;

   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_BIT_STRING, &length);
   /* Make sure the BIT STRING is encoded correctly */
   if (success && (offset + length != limit || length == 0))
   {
      success = FALSE;
   }
   if (success)
   {
      /* First byte is number of bits to ignore at the end */
      bitcount = (length-1) * 8 - data[offset];
      ++offset;
      if (bitcount < 0 || bitcount > 9)
      {
         success = FALSE;
      }
   }
   if (success)
   {
      /* Initialise structure */
      cert_ext->data.key_usage.digitalSignature = FALSE;
      cert_ext->data.key_usage.nonRepudiation = FALSE;
      cert_ext->data.key_usage.keyEncipherment = FALSE;
      cert_ext->data.key_usage.dataEncipherment = FALSE;
      cert_ext->data.key_usage.keyAgreement = FALSE;
      cert_ext->data.key_usage.keyCertSign = FALSE;
      cert_ext->data.key_usage.cRLSign = FALSE;
      cert_ext->data.key_usage.encipherOnly = FALSE;
      cert_ext->data.key_usage.decipherOnly = FALSE;

      /* Following bytes contain actual information */
      if (bitcount > 0)
      {
         b_value = CC_TO_BOOL((data[offset] >> 7) & 0x1);
         cert_ext->data.key_usage.digitalSignature = b_value;
      }
      if (bitcount > 1)
      {
         b_value = CC_TO_BOOL((data[offset] >> 6) & 0x1);
         cert_ext->data.key_usage.nonRepudiation = b_value;
      }
      if (bitcount > 2)
      {
         b_value = CC_TO_BOOL((data[offset] >> 5) & 0x1);
         cert_ext->data.key_usage.keyEncipherment = b_value;
      }
      if (bitcount > 3)
      {
         b_value = CC_TO_BOOL((data[offset] >> 4) & 0x1);
         cert_ext->data.key_usage.dataEncipherment = b_value;
      }
      if (bitcount > 4)
      {
         b_value = CC_TO_BOOL((data[offset] >> 3) & 0x1);
         cert_ext->data.key_usage.keyAgreement = b_value;
      }
      if (bitcount > 5)
      {
         b_value = CC_TO_BOOL((data[offset] >> 2) & 0x1);
         cert_ext->data.key_usage.keyCertSign = b_value;
      }
      if (bitcount > 6)
      {
         b_value = CC_TO_BOOL((data[offset] >> 1) & 0x1);
         cert_ext->data.key_usage.cRLSign = b_value;
      }
      if (bitcount > 7)
      {
         b_value = CC_TO_BOOL(data[offset] & 0x1);
         cert_ext->data.key_usage.encipherOnly = b_value;
      }
      if (bitcount > 8)
      {
         b_value = CC_TO_BOOL((data[offset+1] >> 7) & 0x1);
         cert_ext->data.key_usage.decipherOnly = b_value;
      }
   }

   FUNCTION_FINISH(ParseKeyUsage);

   return success;
}

/*!**************************************************************************
 * @brief    Parse subject key identifier extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseSubjectKeyIdentifier(S_X509_EXT *x509_ext,
                                         S_SECTK_CERT_EXT *cert_ext)
{
   U8BIT *data;
   BOOL success;
   U32BIT offset;
   U32BIT limit;
   U32BIT length;

   FUNCTION_START(ParseSubjectKeyIdentifier);

   data = x509_ext->extnValue.data;
   limit = x509_ext->extnValue.len;
   offset = 0;

   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_OCTET_STRING,
                           &length);
   if (success && offset + length == limit && length == 20)
   {
      memcpy(cert_ext->data.subject_key_identifier.keyIdentifier,
             &data[offset], 20);
   }
   else
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseSubjectKeyIdentifier);

   return success;
}

/*!**************************************************************************
 * @brief    Parse authority key identifier extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseAuthorityKeyIdentifier(S_X509_EXT *x509_ext,
                                           S_SECTK_CERT_EXT *cert_ext)
{
   U8BIT *data;
   BOOL success;
   U32BIT offset;
   U32BIT limit;
   U32BIT length;

   FUNCTION_START(ParseAuthorityKeyIdentifier);

   data = x509_ext->extnValue.data;
   limit = x509_ext->extnValue.len;
   offset = 0;

   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (offset + length != limit))
   {
      success = FALSE;
   }
   if (success)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_KEY_IDENTIFIER,
                              &length);
      if (success && offset + length == limit && length == 20)
      {
         memcpy(cert_ext->data.authority_key_identifier.keyIdentifier,
                &data[offset], 20);
      }
      else
      {
         success = FALSE;
      }
   }

   FUNCTION_FINISH(ParseAuthorityKeyIdentifier);

   return success;
}

/*!**************************************************************************
 * @brief    Parse basic constraints extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseBasicConstraints(S_X509_EXT *x509_ext,
                                     S_SECTK_CERT_EXT *cert_ext)
{
   U8BIT *data;
   BOOL success;
   U32BIT offset;
   U32BIT limit;
   S32BIT i_value;
   BOOL b_value = FALSE;
   U32BIT length;

   FUNCTION_START(ParseBasicConstraints);

   data = x509_ext->extnValue.data;
   limit = x509_ext->extnValue.len;
   offset = 0;

   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (offset + length != limit))
   {
      success = FALSE;
   }

   cert_ext->data.basic_constraints.cA = FALSE;
   cert_ext->data.basic_constraints.pathLenConstraint_provided = FALSE;

   /* cA is optional, default FALSE */
   if (success && offset != limit)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_BOOLEAN, &length);
      if (success)
      {
         success = GetAsn1BooleanValue(data, length, &b_value, &offset,
                                       limit);
         if (success)
         {
            cert_ext->data.basic_constraints.cA = b_value;
         }
      }
   }

   /* pathLenConstraint is optional */
   if (success && offset != limit)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_INTEGER, &length);
      if (success)
      {
         success = GetAsn1IntegerValue(data, length, &i_value, &offset, limit);
         if (success)
         {
            cert_ext->data.basic_constraints.pathLenConstraint = i_value;
            cert_ext->data.basic_constraints.pathLenConstraint_provided = TRUE;
         }
      }
   }

   if (success && (offset != limit))
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseBasicConstraints);

   return success;
}

/*!**************************************************************************
 * @brief    Parse scrambler capabilities extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseScramblerCapabilities(S_X509_EXT *x509_ext,
                                          S_SECTK_CERT_EXT *cert_ext)
{
   U8BIT *data;
   U32BIT length;
   U32BIT offset;
   U32BIT limit;
   S32BIT value;
   BOOL success;

   FUNCTION_START(ParseScramblerCapabilities);

   data = x509_ext->extnValue.data;
   limit = x509_ext->extnValue.len;
   offset = 0;

   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (offset + length != limit))
   {
      success = FALSE;
   }
   if (success)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_INTEGER, &length);
      if (success)
      {
         success = GetAsn1IntegerValue(data, length, &value, &offset, limit);
         if (success)
         {
            cert_ext->data.scrambler_capabilities.capability = value;
         }
      }
   }
   if (success)
   {
      success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_INTEGER, &length);
      if (success)
      {
         success = GetAsn1IntegerValue(data, length, &value, &offset, limit);
         if (success)
         {
            cert_ext->data.scrambler_capabilities.version = value;
         }
      }
   }

   if (success && (offset != limit))
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseScramblerCapabilities);

   return success;
}

/*!**************************************************************************
 * @brief    Parse CICAM brand identifier extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseCicamBrandIdentifier(S_X509_EXT *x509_ext,
                                         S_SECTK_CERT_EXT *cert_ext)
{
   U8BIT *data;
   U32BIT length;
   U32BIT offset;
   U32BIT limit;
   S32BIT value;
   BOOL success;

   FUNCTION_START(ParseCicamBrandIdentifier);

   data = x509_ext->extnValue.data;
   limit = x509_ext->extnValue.len;
   offset = 0;

   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_INTEGER, &length);
   if (success)
   {
      success = GetAsn1IntegerValue(data, length, &value, &offset, limit);
      if (success)
      {
         cert_ext->data.cicam_brand_identifier.cicamBrandId = value;
      }
   }

   if (success && (offset != limit))
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseCicamBrandIdentifier);

   return success;
}

/*!**************************************************************************
 * @brief    Parse CICAM CiPlus information extension
 * @param    x509_cert - raw extension data
 * @param    cert_ext - extension structure
 * @return   TRUE if the extension was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseCiPlusInfo(/*@unused@*/ S_X509_EXT *x509_ext,
                               /*@unused@*/ S_SECTK_CERT_EXT *cert_ext)
{
   FUNCTION_START(ParseCiPlusInfo);

   /* Nothing to do */

   FUNCTION_FINISH(ParseCiPlusInfo);

   return TRUE;
}

/*!**************************************************************************
 * @brief    Parse X.509 signatureAlgorithm section
 * @param    x509 - X.509 certificate
 * @param    offset - offset of section
 * @return   TRUE if signature algorithm was parsed successfully,
 *           FALSE otherwise
 ****************************************************************************/
static BOOL ParseSignatureAlgorithm(S_X509 *x509, U32BIT *offset)
{
   U8BIT *data;
   U32BIT limit;
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseSignatureAlgorithm);

   data = x509->data;
   limit = x509->len;

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success)
   {
      x509->signatureAlgorithm.data = data + *offset;
      x509->signatureAlgorithm.len = length;

      success = VerifySignatureAlgorithm(&x509->signatureAlgorithm);
      if (success)
      {
         *offset += length;

         CC_DEBUG_PRINT(("signatureAlgorithm:\n"));
         CC_DEBUG_BUFFER(x509->signatureAlgorithm.data,
                         x509->signatureAlgorithm.len);
      }
   }
   else
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseSignatureAlgorithm);

   return success;
}

/*!**************************************************************************
 * @brief    Verify that the signature algorithm is RSA-SSAPSS
 * @param    signatureAlgorithm - the signature algorithm field
 * @return   TRUE if signature algorithm was verified, FALSE otherwise
 ****************************************************************************/
static BOOL VerifySignatureAlgorithm(S_BUFFER *signatureAlgorithm)
{
   U8BIT *data;
   U32BIT limit;
   BOOL success;
   U32BIT length;
   U32BIT offset;

   static U8BIT id_RSASSA_PSS[] = {
      0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0a
   };

   FUNCTION_START(VerifySignatureAlgorithm);

   data = signatureAlgorithm->data;
   limit = signatureAlgorithm->len;

   offset = 0;
   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_OBJECT_IDENTIFIER,
                           &length);
   if (success && ((offset + length <= limit) &&
                   (length == sizeof(id_RSASSA_PSS)) &&
                   (memcmp(id_RSASSA_PSS, data + offset, length) == 0)))
   {
      offset += length;

      CC_DEBUG_PRINT(("Signature algorithm verified\n"));
   }
   else
   {
      success = FALSE;
   }

   FUNCTION_FINISH(VerifySignatureAlgorithm);

   return success;
}

/*!**************************************************************************
 * @brief    Parse X.509 signatureValue section
 * @param    x509 - X.509 certificate
 * @param    offset - offset of section
 * @return   TRUE if signature value was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseSignatureValue(S_X509 *x509, U32BIT *offset)
{
   U8BIT *data;
   U32BIT limit;
   BOOL success;
   U32BIT length;

   FUNCTION_START(ParseSignatureValue);

   data = x509->data;
   limit = x509->len;

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_BIT_STRING, &length);
   if (success && ((length == RSA_MOD_SIZE + 1) &&
                   (*offset + length == limit) &&
                   (data[*offset] == 0x00)))
   {
      x509->signatureValue.data = data + *offset + 1;
      x509->signatureValue.len = length - 1;

      *offset += length;
      CC_DEBUG_PRINT(("signatureValue:\n"));
      CC_DEBUG_BUFFER(x509->signatureValue.data, x509->signatureValue.len);
   }
   else
   {
      success = FALSE;
   }

   FUNCTION_FINISH(ParseSignatureValue);

   return success;
}

/*!**************************************************************************
 * @brief    Parse RSA key (DER encoded)
 * @param    data - encoded RSA key
 * @param    len - length of encoded RSA key
 * @param    key - parsed RSA key
 * @return   TRUE if key was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseRsaKey(U8BIT *data, U16BIT len, S_RSA_KEY *key)
{
   U32BIT limit;
   U32BIT offset;
   U32BIT length;
   BOOL success;
   S_BUFFER version;

   FUNCTION_START(ParseRsaKey);

   /* Clear key */
   ClearRsaKey(key);

   /* Parse key */
   limit = len;
   offset = 0;
   success = MatchAsn1Item(data, &offset, limit, ASN1_TAG_SEQUENCE, &length);
   if (success && (length + offset == limit))
   {
      success = ParseAsn1PositiveInteger(data, &version, &offset, limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->modulus, &offset, limit);

      /* Modulus must be at most 2048 bits */
      if (success && (key->modulus.len > RSA_MOD_SIZE))
      {
         success = FALSE;
      }
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->publicExponent, &offset,
                                         limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->privateExponent, &offset,
                                         limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->prime1, &offset, limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->prime2, &offset, limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->exponent1, &offset,
                                         limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->exponent2, &offset,
                                         limit);
   }
   if (success)
   {
      success = ParseAsn1PositiveInteger(data, &key->coefficient, &offset,
                                         limit);
   }

   FUNCTION_FINISH(ParseRsaKey);

   return success;
}

/*!**************************************************************************
 * @brief    Clear RSA key
 * @param    key - RSA key
 ****************************************************************************/
static void ClearRsaKey(S_RSA_KEY *key)
{
   FUNCTION_START(ClearRsaKey);

   key->modulus.len = 0;
   key->modulus.data = NULL;
   key->publicExponent.len = 0;
   key->publicExponent.data = NULL;
   key->privateExponent.len = 0;
   key->privateExponent.data = NULL;
   key->prime1.len = 0;
   key->prime1.data = NULL;
   key->prime2.len = 0;
   key->prime2.data = NULL;
   key->exponent1.len = 0;
   key->exponent1.data = NULL;
   key->exponent2.len = 0;
   key->exponent2.data = NULL;
   key->coefficient.len = 0;
   key->coefficient.data = NULL;

   FUNCTION_FINISH(ClearRsaKey);
}

/*!**************************************************************************
 * @brief    Parse ASN.1 positive integer
 * @param    data - pointer to data buffer
 * @param    integer - positive integer (byte sequence)
 * @param    offset - offset inside data buffer
 * @param    limit - offset limit
 * @return   TRUE if integer was parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL ParseAsn1PositiveInteger(U8BIT *data, S_BUFFER *integer,
                                        U32BIT *offset, U32BIT limit)
{
   U32BIT length;
   BOOL success;

   FUNCTION_START(ParseAsn1PositiveInteger);

   success = MatchAsn1Item(data, offset, limit, ASN1_TAG_INTEGER, &length);
   if (success)
   {
      if ((length > 1) && (data[*offset] == 0x00))
      {
         integer->data = data + *offset + 1;
         integer->len = length - 1;
         *offset += length;
      }
      else if ((length > 0) && ((data[*offset] & 0x80) == 0x00))
      {
         integer->data = data + *offset;
         integer->len = length;
         *offset += length;
      }
      else
      {
         success = FALSE;
      }
   }

   FUNCTION_FINISH(ParseAsn1PositiveInteger);

   return success;
}

/*!**************************************************************************
 * @brief    Return tag from ASN.1 data
 * @param    data - pointer to data buffer
 * @param    [out] tag - tag
 * @param    [in,out] offset - offset inside data buffer (updated on exit)
 * @param    limit - offset limit
 * @return   TRUE if parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL GetAsn1Tag(U8BIT *data, U32BIT *tag, U32BIT *offset,
                          U32BIT limit)
{
   U32BIT i;
   BOOL success;

   FUNCTION_START(GetAsn1Tag);

   success = FALSE;

   i = *offset;
   if (i < limit)
   {
      *tag = data[i++];
      if ((data[i-1] & 0x1f) == 0x1f)
      {
         while (i < limit && (data[i] & 0x80) == 0x80)
         {
            *tag <<= 7;
            *tag |= data[i] & 0x7f;
            ++i;
         }
         if (i < limit)
         {
            *tag |= data[i];
            success = TRUE;
         }
      }
      else
      {
         success = TRUE;
      }
   }

   if (success)
   {
      *offset = i;
   }

   FUNCTION_FINISH(GetAsn1Tag);

   return success;
}

/*!**************************************************************************
 * @brief    Match next ASN.1 item against given tag
 * @param    data - pointer to data buffer
 * @param    [in,out] offset - offset inside data buffer (updated on exit)
 * @param    limit - offset limit
 * @param    tag - ASN.1 tag to match against
 * @param    [out] length - length of tag data
 * @return   TRUE if matched successfully, FALSE otherwise
 ****************************************************************************/
static BOOL MatchAsn1Item(U8BIT *data, U32BIT *offset, U32BIT limit,
                             E_ASN1_TAG tag, U32BIT *length)
{
   U32BIT actual_tag;
   BOOL success;

   FUNCTION_START(MatchAsn1Item);

   success = GetAsn1Tag(data, &actual_tag, offset, limit);
   if (success && actual_tag != tag)
   {
      success = FALSE;
   }
   if (success)
   {
      success = GetAsn1Length(data, length, offset, limit);
      if (success && (*offset + *length > limit))
      {
         success = FALSE;
      }
   }

   FUNCTION_FINISH(MatchAsn1Item);

   return success;
}

/*!**************************************************************************
 * @brief    Return length from ASN.1 data
 * @param    data - pointer to data buffer
 * @param    [out] length - length of tag data
 * @param    [in,out] offset - offset inside data buffer (updated on exit)
 * @param    limit - offset limit
 * @return   TRUE if parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL GetAsn1Length(U8BIT *data, U32BIT *length, U32BIT *offset,
                             U32BIT limit)
{
   U32BIT i;
   U8BIT lenlen;
   BOOL success;

   FUNCTION_START(GetAsn1Length);

   success = FALSE;

   i = *offset;
   if (i < limit)
   {
      *length = data[i];
      if ((*length & 0x80) != 0)
      {
         lenlen = data[i] & 0x7f;
         ++i;
         *length = 0;
         while (lenlen > 0 && i < limit)
         {
            *length <<= 8;
            *length |= data[i];
            ++i;
            --lenlen;
         }
         if (lenlen == 0)
         {
            success = TRUE;
         }
      }
      else
      {
         ++i;
         success = TRUE;
      }
   }

   if (success)
   {
      *offset = i;
   }

   FUNCTION_FINISH(GetAsn1Length);

   return success;
}

/*!**************************************************************************
 * @brief    Return integer value from ASN.1 data
 * @param    data - pointer to data buffer
 * @param    length - length of integer data
 * @param    value - integer value
 * @param    [in,out] offset - offset inside data buffer (updated on exit)
 * @param    limit - offset limit
 * @return   TRUE if parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL GetAsn1IntegerValue(U8BIT *data, U32BIT length, S32BIT *value,
                                   U32BIT *offset, U32BIT limit)
{
   U32BIT i;
   U32BIT u_value;
   BOOL sign_extend;
   BOOL success;

   FUNCTION_START(GetAsn1IntegerValue);

   success = FALSE;
   if (*offset + length <= limit)
   {
      u_value = data[*offset];
      ++(*offset);

      if ((u_value & 0x80) != 0)
      {
         sign_extend = TRUE;
      }
      else
      {
         sign_extend = FALSE;
      }

      for (i = 1; i < length; ++i)
      {
         u_value <<= 8;
         u_value |= data[*offset];
         ++(*offset);
      }

      if (sign_extend)
      {
         *value = SignExtend(u_value);
      }
      else
      {
         *value = u_value;
      }

      success = TRUE;
   }

   FUNCTION_FINISH(GetAsn1IntegerValue);

   return success;
}

/*!**************************************************************************
 * @brief    Return BOOL value from ASN.1 data
 * @param    data - pointer to data buffer
 * @param    length - length of integer data
 * @param    value - BOOL value
 * @param    [in,out] offset - offset inside data buffer (updated on exit)
 * @param    limit - offset limit
 * @return   TRUE if parsed successfully, FALSE otherwise
 ****************************************************************************/
static BOOL GetAsn1BooleanValue(U8BIT *data, U32BIT length, BOOL *value,
                                   U32BIT *offset, U32BIT limit)
{
   BOOL success;

   FUNCTION_START(GetAsn1BooleanValue);

   success = FALSE;
   if (*offset + length <= limit)
   {
      if (length == 1)
      {
         if (data[*offset] == 0)
         {
            *value = FALSE;
         }
         else
         {
            *value = TRUE;
         }
         ++(*offset);
         success = TRUE;
      }
   }

   FUNCTION_FINISH(GetAsn1BooleanValue);

   return success;
}

/*!**************************************************************************
 * @brief    Sign-extend integer value
 * @param    u_value - unsigned value that will be sign-extended
 * @return   sign-extended value
 ****************************************************************************/
static S32BIT SignExtend(U32BIT u_value)
{
   U32BIT t_value;
   S32BIT s_value;
   
   FUNCTION_START(SignExtend);

   if ((u_value & 0x80000000L) == 0)
   {
      t_value = u_value;

      /* This magic finds the next power of two */
      t_value--;
      t_value |= t_value >> 1;
      t_value |= t_value >> 2;
      t_value |= t_value >> 4;
      t_value |= t_value >> 8;
      t_value |= t_value >> 16;
      t_value++;

      /* This magic sign extends */
      u_value |= ~(t_value - 1); 
   }
   s_value = (S32BIT)u_value;

   FUNCTION_FINISH(SignExtend);

   return s_value;
}
static CHAR *ci_strchr(const CHAR *ps_s, const CHAR c_c)
{
    if (ps_s == (CHAR *)NULL)
    {
        return ((CHAR *)NULL);
    }

    do
    {
        if (*ps_s == c_c)
        {
            return ((CHAR *)ps_s);
        }
    } while (*ps_s++ != '\0');

    return ((CHAR *)NULL);
}
static size_t ci_strspn (const CHAR *ps_str, const CHAR *ps_accept)
{
    size_t z_len = 0;
    
    if ((ps_str != NULL) && (ps_accept != NULL))
    {
        UINT32  aui4_flags[8];
        UINT32  ui4_c;
        
        x_memset(aui4_flags, 0, sizeof(aui4_flags));
        
        while ((ui4_c = (UINT32) *(const UCHAR*)ps_accept++) != 0)
        {
            aui4_flags[ui4_c / 32] |= ((UINT32) (1 << (ui4_c % 32)));
        }
        
        ui4_c = (UINT32) *(const UCHAR*)ps_str;
        
        while (aui4_flags[ui4_c / 32] & (UINT32) (1 << (ui4_c % 32)))
        {
            z_len++;
            
            ps_str++;
            ui4_c = (UINT32) *(const UCHAR*)ps_str;
        }
    }
    
    return z_len;
}

/*!**************************************************************************
 * @brief    Convert hexadecimal string to binary string
 * @param    buffer - pointer to string (containing 16 ASCII characters)
 * @param    [out] id - device ID (64 bits = 8 bytes)
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
static BOOL ConvertHexToBin(char *buffer, U8BIT *id)
{
   static const /*@observer@*/ char *hex = "0123456789ABCDEF";
   U16BIT i, j;
   BOOL success;

   FUNCTION_START(ConvertHexToBin);

   success = FALSE;
   if (ci_strspn(buffer, hex) == DEVICE_ID_BYTES)
   {
      /* Found device ID, now convert to bits */
      for (i = 0, j = 0; i < DEVICE_ID_BYTES; i += 2, ++j)
      {
         id[j] = ((ci_strchr(hex, buffer[i]) - hex) * 16 +
                  (ci_strchr(hex, buffer[i+1]) - hex));
      }
      success = TRUE;
   }

   FUNCTION_FINISH(ConvertHexToBin);

   return success;
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
