/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    sectk.h                                                       */
/*   DATE:    18/12/08                                                      */
/*   PURPOSE: Header File for CI+  Security Toolkit Module                  */
/*                                                                          */
/****************************************************************************/
#ifndef __SECTK_H_
#define __SECTK_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

typedef enum
{
   SECTK_CERT_EXT_KEY_USAGE,
   SECTK_CERT_EXT_SUBJECT_KEY_IDENTIFIER,
   SECTK_CERT_EXT_AUTHORITY_KEY_IDENTIFIER,
   SECTK_CERT_EXT_BASIC_CONSTRAINTS,
   SECTK_CERT_EXT_SCRAMBLER_CAPABILITIES,
   SECTK_CERT_EXT_CI_PLUS_INFO,
   SECTK_CERT_EXT_CICAM_BRAND_IDENTIFIER,
   SECTK_CERT_EXT_UNKNOWN
} E_SECTK_CERT_EXT_TYPE;

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

typedef struct sectk_cert_ext
{
   E_SECTK_CERT_EXT_TYPE type;
   BOOL critical;
   union
   {
      struct
      {
         BOOL cA;
         BOOL pathLenConstraint_provided;
         S32BIT pathLenConstraint;
      } basic_constraints;
      struct
      {
         BOOL digitalSignature;
         BOOL nonRepudiation;
         BOOL keyEncipherment;
         BOOL dataEncipherment;
         BOOL keyAgreement;
         BOOL keyCertSign;
         BOOL cRLSign;
         BOOL encipherOnly;
         BOOL decipherOnly;
      } key_usage;
      struct
      {
         U8BIT keyIdentifier[20];
      } subject_key_identifier;
      struct
      {
         U8BIT keyIdentifier[20];
         /* authorityCertIssuer and authorityCertSerialNumber are ignored */
      } authority_key_identifier;
      struct
      {
         S32BIT capability;
         S32BIT version;
      } scrambler_capabilities;
      struct
      {
         S32BIT cicamBrandId;
      } cicam_brand_identifier;
   } data;
} S_SECTK_CERT_EXT;

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Initialise security module
 ****************************************************************************/
void SECTK_Initialise(void);

/*!**************************************************************************
 * @brief    Apply AES-128 encryption on a buffer
 * @param    buffer - buffer to encrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 * @note     The result of the encryption is stored in the input buffer
 ****************************************************************************/
void SECTK_AesEncrypt(U8BIT *buffer, U16BIT length, U8BIT *key);

/*!**************************************************************************
 * @brief    Apply AES-128 decryption on a buffer
 * @param    buffer - buffer to decrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 * @note     The result of the decryption is stored in the input buffer
 ****************************************************************************/
void SECTK_AesDecrypt(U8BIT *buffer, U16BIT length, U8BIT *key);

/*!**************************************************************************
 * @brief    Apply AES-128-CBC encryption on a buffer
 * @param    buffer - buffer to encrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 * @param    iv - AES initialisation vector (128 bits = 16 bytes)
 * @note     The result of the encryption is stored in the input buffer
 ****************************************************************************/
void SECTK_AesCbcEncrypt(U8BIT *buffer, U16BIT length, U8BIT *key, U8BIT *iv);

/*!**************************************************************************
 * @brief    Apply AES-128 decryption on a buffer
 * @param    buffer - buffer to decrypt
 * @param    length - length of buffer in bytes
 * @param    key - AES key (128 bits = 16 bytes)
 * @param    iv - AES initialisation vector (128 bits = 16 bytes)
 * @note     The result of the decryption is stored in the input buffer
 ****************************************************************************/
void SECTK_AesCbcDecrypt(U8BIT *buffer, U16BIT length, U8BIT *key, U8BIT *iv);

/*!**************************************************************************
 * @brief    Authenticate buffer using AES-128-XCBC-MAC
 * @param    buffer - buffer to authenticate
 * @param    buffer_len - length of buffer to authenticate
 * @param    key - AES key (128 bits = 16 bytes)
 * @param    mac - Message Authentication Code (128 bits = 16 bytes)
 ****************************************************************************/
void SECTK_Authenticate(U8BIT *buffer, U32BIT buffer_len, U8BIT *key,
                        U8BIT *mac);

/*!**************************************************************************
 * @brief    Apply SHA-256 on a buffer
 * @param    buffer - buffer to decrypt
 * @param    length - length of buffer in bytes
 * @param    digest - result of SHA-256 (256 bits = 32 bytes)
 ****************************************************************************/
void SECTK_Sha256(U8BIT *buffer, U16BIT length, U8BIT *digest);

/*!**************************************************************************
 * @brief    Set pseudo-random number generator seed
 * @param    seed - 128-bit (16 byte) PRNG seed
 * @param    key_k - 128-bit (16 byte) PRNG key (licence constant)
 ****************************************************************************/
void SECTK_SetRandomSeed(U8BIT *seed, U8BIT *key_k);

/*!**************************************************************************
 * @brief    Generate random bit sequence
 * @param    buffer - buffer to fill with the sequence
 * @param    length - length of sequence in bytes
 ****************************************************************************/
void SECTK_GenerateRandomSequence(U8BIT *buffer, U16BIT length);

/*!**************************************************************************
 * @param    dh_g - Diffie-Hellman generator
 * @param    dh_p - Diffie-Hellman modulus
 * @param    dh_pub - Diffie-Hellman public key (2048 bits = 256 bytes)
 * @param    dh_exp - Diffie-Hellman exponent (2048 bits = 256 bytes)
 ****************************************************************************/
void SECTK_GenerateDhKeys(U8BIT *dh_g, U8BIT *dh_p, U8BIT *dh_q,
                          U8BIT *dh_pub, U8BIT *dh_exp);

/*!**************************************************************************
 * @brief    Generate Diffie-Hellman keys
 * @param    dh_pub - Diffie-Hellman public key (2048 bits = 256 bytes)
 * @param    dh_exp - Diffie-Hellman exponent (2048 bits = 256 bytes)
 * @param    dh_p - Diffie-Hellman modulus (2048 bits = 256 bytes)
 * @param    shared - Diffie-Hellman shared key (2048 bits = 256 bytes)
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
void SECTK_CalculateDhShared(U8BIT *dh_pub, U8BIT *dh_exp, U8BIT *dh_p,
                             U8BIT *shared);

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
BOOL SECTK_CheckDhPublicKey(U8BIT *dh_pub, U8BIT *dh_p, U8BIT *dh_q);

/*!**************************************************************************
 * @brief    Parse X.509 certificate
 * @param    buffer - buffer holding certificate in DER format
 * @param    length - length of buffer in bytes
 * @return   Pointer to parsed certificate
 ****************************************************************************/
void *SECTK_ParseCertificate(U8BIT *buffer, U16BIT length);

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
                   U8BIT *signature);

/*!**************************************************************************
 * @brief    Verify signature using RSA public key from certificate
 * @param    buffer - buffer to verify
 * @param    length - buffer length in bytes
 * @param    cert - pointer to certificate
 * @param    signature - RSA signature (2048 bits = 256 bytes)
 * @return   TRUE if signature was verified, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_Verify(U8BIT *buffer, U16BIT length, void *cert,
                     U8BIT *signature);

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
BOOL SECTK_VerifyCertificate(void *cert, void *parent_cert);

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
                                             U8BIT *key_identifier);

/*!**************************************************************************
 * @brief    Extract device ID from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] id - device ID (64 bits = 8 bytes)
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractDeviceId(void *dev_cert, U8BIT *id);
/*!**************************************************************************
 * @brief    SECTK_VerifyDeviceId
 * @param    dev_cert - pointer to device certificate
 * @return   TRUE if valid, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_VerifyDeviceId(void *dev_cert);

/*!**************************************************************************
 * @brief    Extract scrambler capabilities from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] capability - scrambler capability
 * @param    [out] version - capability version
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractScramblerCaps(void *dev_cert, S32BIT *capability,
                                   S32BIT *version);

/*!**************************************************************************
 * @brief    Extract brand ID from device certificate
 * @param    dev_cert - pointer to device certificate
 * @param    [out] brand_id - brand ID
 * @return   TRUE if successful, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_ExtractBrandId(void *dev_cert, U16BIT *brand_id);

/*!**************************************************************************
 * @brief    Return issuer name from certificate
 * @param    cert - pointer to certificate
 * @return   Pointer to issuer name (opaque handle)
 ****************************************************************************/
void *SECTK_GetCertificateIssuer(void *cert);

/*!**************************************************************************
 * @brief    Return subject name from certificate
 * @param    cert - pointer to certificate
 * @return   Pointer to subject name (opaque handle)
 ****************************************************************************/
void *SECTK_GetCertificateSubject(void *cert);

/*!**************************************************************************
 * @brief    Compare certificate names (issuer / subject)
 * @param    name_a - pointer to name
 * @param    name_b - pointer to name
 * @return   TRUE if the names are identical, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_CompareCertificateName(void *name_a, void *name_b);

/*!**************************************************************************
 * @brief    Check whether certificate date/time is not before given time
 * @param    cert - pointer to certificate
 * @param    epoch_time - time since epoch in seconds
 * @return   TRUE if the certificate date/time is not before given time
 ****************************************************************************/
BOOL SECTK_CheckCertificateNotBefore(void *cert, U32BIT epoch_time);

/*!**************************************************************************
 * @brief    Check whether certificate date/time is not after given time
 * @param    cert - pointer to certificate
 * @param    epoch_time - time since epoch in seconds
 * @return   TRUE if the certificate date/time is not before given time
 ****************************************************************************/
BOOL SECTK_CheckCertificateNotAfter(void *cert, U32BIT epoch_time);

/*!**************************************************************************
 * @brief    Return first certificate extension
 * @param    cert - pointer to certificate
 * @param    iter - iterator
 * @param    cert_ext - certificate extension
 * @return   TRUE if extension was found, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_GetFirstCertificateExtension(void *cert, U32BIT *iter,
                                           S_SECTK_CERT_EXT *cert_ext);

/*!**************************************************************************
 * @brief    Return next certificate extension
 * @param    cert - pointer to certificate
 * @param    iter - iterator
 * @param    ext - certificate extension
 * @return   TRUE if extension was found, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_GetNextCertificateExtension(void *cert, U32BIT *iter,
                                          S_SECTK_CERT_EXT *cert_ext);

/*!**************************************************************************
 * @brief    Return certificate extension given extension type
 * @param    cert - pointer to certificate
 * @param    type - extension type
 * @param    cert_ext - certificate extension
 * @return   TRUE if extension was found, FALSE otherwise
 ****************************************************************************/
BOOL SECTK_GetCertificateExtension(void *cert, E_SECTK_CERT_EXT_TYPE type,
                                      S_SECTK_CERT_EXT *cert_ext);

/*!**************************************************************************
 * @brief    Free parsed X.509 certificate
 * @param    cert - pointer to X.509 certificate
 ****************************************************************************/
void SECTK_FreeCertificate(void *cert);

/*!**************************************************************************
 * @brief    Terminate security module
 ****************************************************************************/
void SECTK_Terminate(void);
 
BOOL SECTK_ExtractOU(void *dev_cert, BOOL *pb_product);

#ifdef __cplusplus
  }
#endif
#endif   /* !__SECTK_H */
