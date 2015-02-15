//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/marlin3/DecoderDRM_TZ.c#1 $
//-----------------------------------------------------------------------------
/*! @file   DecoderDRM_TZ.cpp
 *  @brief  DecoderDRM API for calling function in the Trust Zone from outside the Trust Zone
 *  @author Fujitsu) Tetsuhiro CHIBA
 *  @date   2011/10/19 created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#include <stdlib.h> // for malloc(), free()
#include <string.h> // for memcpy(), memset()
#include <ctype.h> // for isdigit()

#include "DecoderDRM_TZ.h"
#include "DecoderDRM_TZ_priv.h"
#include "tzst.h"

#ifdef REQUIRE_ERROR_DETAIL
# define TZ_SET_ERROR_DETAIL(error_id) TZ_SetErrorDetail(error_id);
# define TZ_INIT_ERROR_DETAIL          TZ_InitErrorDetail();
#else /* not REQUIRE_ERROR_DETAIL */
# define TZ_SET_ERROR_DETAIL(error_id)
# define TZ_INIT_ERROR_DETAIL
# define TZ_GetErrorInf(inf)
#endif /* not REQUIRE_ERROR_DETAIL */


/*! Encryption / Decription function (ANSI / SCTE 52)
 *
 * @param aes_ctx [input] The pointer to the AES context structure is specified with aes_ctx.
 * @param key16   [input] The cryptographic key in 16 bytes is specified with key16.
 * @param iv      [input] The initialization vector in 16 bytes is specified with iv.
 * @param dest    [input] The result of the encryption or decryption is stored in dest.
 * @param src     [input] The plaintext is specified with src when encrypting it. The encrypted data is specified with src when decrypting it.
 * @param size    [input] The size of data that does the encryption processing is specified with size.
 *                        The any value in 16 bytes or more is specified. Size need not be a multiple of 16.
 * @param mode    [input] Encrypting / Decrypting flag is specified with mode. Either of the following values are specified for mode.
 *                         @li 0: Encryption
 *                         @li 1: Decryption
 *
 * @retval DECODER_DRM_RETURN_OK(0)        Normal termination.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)  Internal error.
 *
 * @note The algorithm of the encryption is as follows.
 *       As the method to encrypt the fraction part (part that remains of the multiple of 16)
 *       in the code block, "Result of encrypting the result of the Block cipher immediately
 *       before with the cryptographic key in addition" and "Fraction part" are assumed and
 *       the one that was XOR (However, only the part where the bit exists is an object of XOR
 *       operation) each bit is assumed to be "Code result in the fraciton part" in ANSI/SCTE 52.
 */
int TZ_cbc_scte52(symmetric_CBC* aes_ctx, const unsigned char* key16, unsigned char* iv, unsigned char* dest, unsigned char* src, size_t size, int mode)
{
  int rc;
  int idx;
  size_t n;
  size_t normal_size;
  size_t termination_block_size; //! Size in the last fraction part (0~15)
  unsigned char prev_terminate_block[TZ_AES_BLOCK_SIZE];
  unsigned char* p;
  unsigned char* q;

  termination_block_size = size % TZ_AES_BLOCK_SIZE;

  ////////////////////////////////////////////////////////////////////////////////
  //! A usual AES encryption/decryption is done when there is no fraction part.
  ////////////////////////////////////////////////////////////////////////////////
  if (termination_block_size == 0) {
    rc = (mode == 0 ?
	  cbc_encrypt(src, dest, size, aes_ctx) :
	  cbc_decrypt(src, dest, size, aes_ctx));
    if (rc != CRYPT_OK) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00431);
      rc = DECODER_DRM_INTERNAL_ERROR;
    } else {
      rc = DECODER_DRM_RETURN_OK;
    }
    goto EXIT;
  }

  if ((idx = find_cipher("aes")) == -1) {
    if ((idx = find_cipher("rijndael")) == -1) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00432);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //! When there is only terminate block.
  ////////////////////////////////////////////////////////////////////////////////
  if (size < TZ_AES_BLOCK_SIZE) {
    //! IV and the last 16 bytes are processed as 0x00.
    unsigned char zero_buff[TZ_AES_BLOCK_SIZE] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    cbc_done(aes_ctx);

    //! The reinitialization of AES is done.
    if ((rc = cbc_start(idx, iv, key16, TZ_AES_KEY_SIZE, 0, aes_ctx)) != CRYPT_OK) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00433);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    } 

    if (cbc_encrypt(zero_buff, prev_terminate_block, TZ_AES_BLOCK_SIZE, aes_ctx) != CRYPT_OK) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00434);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }

    //! XOR of the one that the last 16 bytes were doubly encrypted and the last block is done.
    for (n = 0, p = dest, q = src; n < termination_block_size; ++n, ++p, ++q) {
      *p = prev_terminate_block[n] ^ (*q);
    }
    goto EXIT;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // When there is data of 16 bytes or more (route usually)
  ////////////////////////////////////////////////////////////////////////////////

  //! The code separately for the former part and latter part is processed because there is a fraction part.
  normal_size = size - termination_block_size;

  ////////////////////////////////////////
  //! The former part are calculated.
  rc = (mode == 0 ?
	cbc_encrypt(src, dest, normal_size, aes_ctx) :
	cbc_decrypt(src, dest, normal_size, aes_ctx));
  if (rc != CRYPT_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00435);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  ////////////////////////////////////////
  //! The latter part are calculated.
  cbc_done(aes_ctx);

  //! The reinitialization of AES is done.
  if ((rc = cbc_start(idx, iv, key16, TZ_AES_KEY_SIZE, 0, aes_ctx)) != CRYPT_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00436);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  } 

  //! The last 16 bytes of the former part are encrypted again.
  if ((rc = cbc_encrypt((mode == 0 ? dest : src) + (normal_size - TZ_AES_BLOCK_SIZE),
			prev_terminate_block, TZ_AES_BLOCK_SIZE, aes_ctx)) != CRYPT_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00437);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  //! Data that doubley encrypts the last 16 bytes and XOR of the last block are done.
  for (n = 0, p = dest + normal_size, q = src + normal_size; n < termination_block_size; ++n, ++p, ++q) {
    *p = prev_terminate_block[n] ^ (*q);
  }

 EXIT:
  return rc;
}


/*! Encryption / Decription tiny function (ANSI / SCTE 52)
 *
 * @param data          [input/output] The data store area for encrypting/decrypting is specified with data. When out_buff is NULL, the coding result is stored in data.
 * @param data_size     [input]        The size of data is specified with data_size. The any value in 16 bytes or more is specified. Size need not be a multiple of 16.
 * @param out_buff      [input/output] The area where the conversion result is stored is specified with out_buff. When NULL is specified for out_buff,
 *                                     the conversion result is stored in data.
 * @param out_buff_size [input]        The size of out_buff is specified with out_buff_size. When out_buff is not NULL, out_buff_size should be a value of data_size or more.
 * @param key           [input]        The cryptographic key in 16 bytes is specified with key.
 * @param custom_iv     [input]        The initialization vector in 16 bytes is specified with custom_iv. When IV is not specified, NULL is specified.
 *                                     In this case, 0 in 16 bytes is used as IV.
 * @param mode          [input]        Encrypting / Decrypting flag is specified with mode. Either of the following values are specified for mode.
 *                                       @li 0: Encryption
 *                                       @li 1: Decryption
 *
 * @retval DECODER_DRM_RETURN_OK(0)        Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)       Argument invalid.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)  Internal error.
 */
int TZ_crypt_buffer(unsigned char* data, const size_t data_size, unsigned char* out_buff, const size_t out_buff_size,
		    const unsigned char* key, const unsigned char* custom_iv, const int mode)
{
  int rc = DECODER_DRM_RETURN_OK;                    // return value
  int idx;
  unsigned char* iv = NULL;
  unsigned char zero_iv[TZ_AES_KEY_SIZE] = { // initialization vector
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
  };
  symmetric_CBC aes_ctx; 	// AES context structure
  int alloc_flag = 0;

  (void) memset(&aes_ctx, 0, sizeof(symmetric_CBC));

  iv = custom_iv ? (unsigned char*) custom_iv : (unsigned char*) zero_iv;

  if (out_buff) {
    if (out_buff_size < data_size) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00454);
      rc = DECODER_DRM_ARG_ERROR;
      goto EXIT;
    }
  } else {
    //! The work area is allocated.
    if ((out_buff = (unsigned char*) malloc(data_size)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00438);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
    alloc_flag = 1;
  }

  //! The AES environment is initialized
  if ((idx = find_cipher("aes")) == -1) {
    if ((idx = find_cipher("rijndael")) == -1) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00439);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
  }
  if ((rc = cbc_start(idx, iv, key, TZ_AES_KEY_SIZE, 0, &aes_ctx)) != CRYPT_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00440);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  } 

  //! SCTE 52 encryption function is executed (@ref TZ_cbc_scte52)
  if ((rc = TZ_cbc_scte52(&aes_ctx, key, iv, out_buff, data, data_size, mode)) != 0) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00441);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  //! The work area is written in former area.
  if (alloc_flag) {
    (void) memcpy(data, out_buff, data_size);
    (void) memset(out_buff, 0x00, data_size);
  }
  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  cbc_done(&aes_ctx);

  //! When the work area is used, the memory is freed.
  if (alloc_flag) {
    free((void*) out_buff);
  }

  //! Return.
  return rc;
}


/*! Calculates digest value of SHA-1
 *
 * @param data      [input]  The pointer of the calculation domain of the digest value is specified with data.
 * @param data_size [input]  The size of data is specified with data_size.
 * @param digest    [output] The area where the calculation result of the digest value is stored is specified with digest.
 *
 * @return none.
 */
void TZ_calc_digest(const unsigned char* data, const size_t data_size, unsigned char* digest)
{
  hash_state context;

  //! The SHA-1 function is initialized.
  sha1_init(&context);

  //! The digest is calculated.
  sha1_process(&context, data, data_size);

  //! The calculation result of the digest is stored.
  sha1_done(&context, digest);

  return;
}


/*! This function reads the size of the ASN.1 form from the specified area.
 *
 * @param read_buff [input]  The area where the data of the ASN.1 form was staored is specified with read_buff.
 * @param tag       [output] The tag of the result of reading is stored in tag.
 * @param _size     [output] The read size is stored in _size.
 *
 * @return The following read pointer is retured. That is, "read_buff + _size" is returend.
 */
static unsigned char* TZ_read_asn1_header(unsigned char* read_buff, unsigned char* tag, size_t* _size)
{
  *tag = *(read_buff++);

  switch (*read_buff) {
  case 0x81:
    read_buff++;
    *_size = (size_t) *(read_buff++);
    break;
  case 0x82:
    read_buff++;
    *_size = (size_t) *(read_buff++);
    *_size = (((size_t) (*_size)) << 8) + ((size_t) (*(read_buff++)));
    break;
  case 0x84:
    read_buff++;
    *_size = (size_t) *(read_buff++);
    *_size = (((size_t) (*_size)) << 8) + ((size_t) (*(read_buff++)));
    *_size = (((size_t) (*_size)) << 8) + ((size_t) (*(read_buff++)));
    *_size = (((size_t) (*_size)) << 8) + ((size_t) (*(read_buff++)));
    break;
  default:
    *_size = (size_t) *(read_buff++);
    break;
  }

  return read_buff;
}


/*! Subject is taken out of the certificate for Marlin IPTV-ES.
 *
 * @param cert        [input]  The pointer in the area where the certificate was stored is specified with cert.
 * @param cert_size   [input]  The size of cert is specified with cert_size.
 * @param subject     [output] The pointer to subject in the certificate storage area is tored in subject.
 *                             The caller of the function must not free this pointer because this pointer does the area in cert in the point.
 *
 * @retval DECODER_DRM_RETURN_OK(0)       Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)      Argument invalid.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4) Internal error.
 */
int TZ_cert_get_subject(const unsigned char* cert, const size_t cert_size, unsigned char** subject)
{
  int rc = DECODER_DRM_INTERNAL_ERROR;
  size_t size = 0;
  size_t tbs_cert_size = 0;
  unsigned char tag;
  unsigned char* p;
  unsigned char* buff;
  int i;

  //! Check arguments
  if (!cert || !cert_size || !subject) return DECODER_DRM_ARG_ERROR;

  // device1fff000010000000.der
  // 30 82 01 a6
  //    30 82 01 54
  //       a0 03 ...
  //       02 08 ...
  //       30 0a ...
  //       30 53 ...
  //       30 1e ...
  //       30 23
  //          31 21
  //             30 1f
  //                06 0b
  //                   2b 06
  //                      01 04
  //                         01 81 b9 2f
  //                   03 01
  //                      01
  //                0c 10
  //                   31 66 66 66 30 30 30 30 31 30 30 30 30 30 30 30 ' 1fff000010000000
  //       ...

  //! The size of the header of the certificate is read (@ref TZ_read_asn1_header).
  p = TZ_read_asn1_header((unsigned char*) cert, &tag, &size);

  //! The size of the tbs_cert is read (@ref TZ_read_asn1_header).
  p = TZ_read_asn1_header((unsigned char*) p,    &tag, &tbs_cert_size);
  buff = p;

  for (i = 0; (i < 5) && (p < buff + tbs_cert_size); ++i) {
    p = TZ_read_asn1_header((unsigned char*) p, &tag, &size);
    p += size;
  }
  if (i != 5) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00442);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  p = TZ_read_asn1_header((unsigned char*) p, &tag, &size); // 30 23
  p = TZ_read_asn1_header((unsigned char*) p, &tag, &size); //    31 21
  p = TZ_read_asn1_header((unsigned char*) p, &tag, &size); //       30 1f
  p = TZ_read_asn1_header((unsigned char*) p, &tag, &size); //          06 0b ...
  p += size;

  p = TZ_read_asn1_header((unsigned char*) p, &tag, &size); // 0c 10
  if (tag != 0x0c) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00443);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }
  *subject = p;

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  return rc;
}


/*! The subject of hexadecimal format is conveted into SenderID of the binary.
 *
 * @param hex           [input]  The subject of hexadecimal format of 16 digits is specified with hex. The NULL terminal is necessary for hex.
 * @param sender_id     [output] SenderID is stored in the area specified with sender_id. The caller of this function should allocate the area in 8 bytes or more.
 * @param sender_id_len [output] The length after sender_id is converted is stored in sender_id_len.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 */
static int TZ_hex2bin(char* hex, unsigned char* sender_id, size_t* sender_id_len)
{
  int rc = DECODER_DRM_RETURN_OK;
  unsigned long len = 0;	// This variable is used to confirm the length of sender_id.
  char* p = NULL;		// work pointer
  unsigned char* q = NULL;	// work pointer

  //! The subject of hexadecimal format in 16 bytes is converted into the binary data of 8 bytes.
  for (p = hex, q = sender_id, len = 0; *p && (len < 16 + 1); len += 2, p += 2, q++) { // In len, the funciton to observe the buffer overflow is provided, too.

    //! The first processing of the first byte of tow bytes.
    if (isdigit(*p)) { //! case of digits
      *q = ((*p) - '0') << 4;
    } else if (('A' <= *p) && (*p <= 'F')) { //! case of A~F
      *q = (*p - 'A' + 10) << 4;
    } else if (('a' <= *p) && (*p <= 'f')) { //! case of a~f
      *q = (*p - 'a' + 10) << 4;
    } else {
      //! Besides, the character kind is illegal.
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }

    //! Processing of 1 byte in back of two bytes.
    if (isdigit(*(p + 1))) { //! case of digits
      *q = (*q) | ((*(p + 1)) - '0');
    } else if (('A' <= *(p + 1)) && (*(p + 1) <= 'F')) { //! case of A~F
      *q = (*q) | (*(p + 1) - 'A' + 10);
    } else if (('a' <= *(p + 1)) && (*(p + 1) <= 'f')) { //! case of a~f
      *q = (*q) | (*(p + 1) - 'a' + 10);
    } else {
      //! Besides, the character kind is illegal.
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
  }

  *sender_id_len = ((16 <= len) ? 8 : len / 2);

  if (*sender_id_len != TZ_SENDER_ID_LEN) {
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  return rc;
}


/*! De-scramble a secret data
 *
 * @param tamper_key	[input] Points to a scrambled secret data, for example a private key. The caller SHALL allocate memory.
 * @param private_key	[output] De-scrambled data is stored. The caller SHALL allocate memory.
 * @param key_len	[input] Specifies the area size of tamper_key and private_key.
 *
 * @return None
 *
 * @note The data size is the same between before and after de-scrambling.
 */
void TZ_Marlin_DeTamperKey(const unsigned char* tamper_key, unsigned char* private_key, size_t key_len)
{
  int i;
  int shift;
  unsigned char uc = 0;

  //! Fill the area of private_key with 0
  (void) memset(private_key, 0, key_len);

  //! Execute de-scrambling
  for (i = 0; i < (int) key_len; ++i) {
    shift = (i % 7) + 1;
    uc = (tamper_key[i] << shift) | (tamper_key[i] >> (8 - shift));
    private_key[i] |= (uc & 0xf0);
    private_key[key_len - i - 1] |= (uc & 0x0f);
  }

  return;
}


/*! Read cached certificates and private key from the Trust Zone and de-scramble them.
 *
 * @param secret_data_type	[input] Specifies which type of terminal-specific certificates and private key to use by the following macro.
 *					@li ::MEDIA_DRM_SECRET_DATA_GENERAL (1)
 *					@li ::MEDIA_DRM_SECRET_DATA_RTDB    (2)
 *					@li ::MEDIA_DRM_SECRET_DATA_OTHER   (3)
 * @param root_cert_buf		[output] Points to an area to which the root public key certificate is returned. It is de-scrambled.
 * @param root_cert_buf_len	[input] Specifies the size of the area pointed by root_cert_buf.
 * @param root_cert_len		[output] Points to an area to which the size of the root public key certificate is returned.
 * @param ca_cert_buf		[output] Points to an area to which the client CA public key certificate is returned. It is de-scrambled.
 * @param ca_cert_buf_len	[input] Specifies the size of the area pointed by ca_cert_buf.
 * @param ca_cert_len		[output] Points to an area to which the size of the client CA public key certificate is returned.
 * @param client_cert_buf	[output] Points to an area to which the client public key certificate is returned. It is de-scrambled.
 * @param client_cert_buf_len	[input] Specifies the size of the area pointed by client_cert_buf.
 * @param client_cert_len	[output] Points to an area to which the size of the client public key certificate is returned.
 * @param client_key_buf	[output] Points to an area to which the client private key is returned. It is de-scrambled.
 * @param client_key_buf_len	[input] Specifies the size in bytes of the area pointed by client_key_buf. This argument SHALL be 28.
 * @param client_key_len	[output] Points to an area to which the size of the client private key is returned.
 *
 * @retval ::DECODER_DRM_RETURN_OK	Successful completion
 * @retval ::DECODER_DRM_ARG_ERROR	secret_data_type is invalid, or a buffer for output is of short length, or the cached data is invalid and the data size in it is very large.
 * @retval ::DECODER_DRM_INTERNAL_ERROR	Data is not cached, or cannot allocate memory
 *
 * @note This is an internal function, so this function doesn't have to do the NULL check and 28 check of arguments.
 */
int TrustZone_GetSecretData(int secret_data_type,
			    unsigned char* root_cert_buf,   size_t root_cert_buf_len,   size_t* root_cert_len,
			    unsigned char* ca_cert_buf,     size_t ca_cert_buf_len,     size_t* ca_cert_len,
			    unsigned char* client_cert_buf, size_t client_cert_buf_len, size_t* client_cert_len,
			    unsigned char* client_key_buf,  size_t client_key_buf_len,  size_t* client_key_len)
{
  int rc = MEDIA_DRM_RETURN_OK;
  CertsAndKey* cache = NULL; // The place where the secret data is cached
  unsigned char* descramble_buff = NULL;
  size_t descramble_buff_len = 0;
  unsigned char buff_area[1022]; // If this size is not enough, this function allocates memory below.

  descramble_buff = buff_area;
  descramble_buff_len = sizeof(buff_area);

  //! Check arguments
  if (!root_cert_buf || !ca_cert_buf || !client_cert_buf || !client_key_buf ||
      !root_cert_len || !ca_cert_len || !client_cert_len || !client_key_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00200);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  switch (secret_data_type) {
  case MEDIA_DRM_SECRET_DATA_GENERAL:
    cache = &__g_trust_zone_anchor.data_for_general;
    break;

  case MEDIA_DRM_SECRET_DATA_RTDB:
#if 0
    cache = &__g_trust_zone_anchor.data_for_rtdb;
    break;
#endif
  case MEDIA_DRM_SECRET_DATA_OTHER:
#if 0
    cache = &__g_trust_zone_anchor.data_for_other;
    break;
#endif
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00201);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;

  default:
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00202);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  //! NULL check
  if (!cache->_enc_root_cert ||
      !cache->_enc_ca_cert ||
      !cache->_enc_client_cert ||
      !cache->_enc_client_key) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00203);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }


  //! Read a root public key certificate and de-scramble it
  *root_cert_len = ((unsigned long) cache->_enc_root_cert[0] << 8) | (unsigned long) cache->_enc_root_cert[1];
  // Check the length of output buffer
  if (root_cert_buf_len < *root_cert_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00204);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }
  // Check the length of de-scrambling buffer
  if (*root_cert_len > descramble_buff_len) {
    if (descramble_buff != buff_area) {
      free((void*) descramble_buff);
    }
    if ((descramble_buff = (unsigned char*) malloc(*root_cert_len)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00205);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
    descramble_buff_len = *root_cert_len;
  }
  // De-scramble the data
  (void) memcpy(descramble_buff, cache->_enc_root_cert + 2, *root_cert_len);
  TZ_Marlin_DeTamperKey(descramble_buff, root_cert_buf, *root_cert_len);


  //! Read a client CA public key certificate and de-scramble it
  *ca_cert_len = ((unsigned long) cache->_enc_ca_cert[0] << 8) | (unsigned long) cache->_enc_ca_cert[1];
  // Check the length of output buffer
  if (ca_cert_buf_len < *ca_cert_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00206);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }
  // Check the length of de-scrambling buffer
  if (*ca_cert_len > descramble_buff_len) {
    if (descramble_buff != buff_area) {
      free((void*) descramble_buff);
    }
    if ((descramble_buff = (unsigned char*) malloc(*ca_cert_len)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00207);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
    descramble_buff_len = *ca_cert_len;
  }
  // De-scramble the data
  (void) memcpy(descramble_buff, cache->_enc_ca_cert + 2, *ca_cert_len);
  TZ_Marlin_DeTamperKey(descramble_buff, ca_cert_buf, *ca_cert_len);


  //! Read a client public key certificate and de-scramble it
  *client_cert_len = ((unsigned long) cache->_enc_client_cert[0] << 8) | (unsigned long) cache->_enc_client_cert[1];
  // Check the length of output buffer
  if (client_cert_buf_len < *client_cert_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00208);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }
  // Check the length of de-scrambling buffer
  if (*client_cert_len > descramble_buff_len) {
    if (descramble_buff != buff_area) {
      free((void*) descramble_buff);
    }
    if ((descramble_buff = (unsigned char*) malloc(*client_cert_len)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00209);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
    descramble_buff_len = *client_cert_len;
  }
  // De-scramble the data
  (void) memcpy(descramble_buff, cache->_enc_client_cert + 2, *client_cert_len);
  TZ_Marlin_DeTamperKey(descramble_buff, client_cert_buf, *client_cert_len);


  //! Read a client private key and de-scramble it
  *client_key_len = ((unsigned long) cache->_enc_client_key[0] << 8) | (unsigned long) cache->_enc_client_key[1];
  // Check the length of output buffer
  if (client_key_buf_len < *client_key_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00210);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }
  // Check the length of de-scrambling buffer
  if (*client_key_len > descramble_buff_len) {
    if (descramble_buff != buff_area) {
      free((void*) descramble_buff);
    }
    if ((descramble_buff = (unsigned char*) malloc(*client_key_len)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00211);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
    descramble_buff_len = *client_key_len;
  }
  // De-scramble the data
  (void) memcpy(descramble_buff, cache->_enc_client_key + 2, *client_key_len);
  TZ_Marlin_DeTamperKey(descramble_buff, client_key_buf, *client_key_len);


  rc = MEDIA_DRM_RETURN_OK;

 EXIT:
  if (descramble_buff != buff_area) {
    free((void*) descramble_buff);
    descramble_buff = NULL;
  }

  return rc;
}


/*! Generates the key for the LDB encryption
 *
 * @param _ldb_key   [input] SenderID of the binary in 8 bytes is specified by _ldb_key.
 * @param secret_key [input] The generated LDB key is stored in secret_key.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 */
static int TZ_CreateLDBKey(const unsigned char* _ldb_key, unsigned char* secret_key)
{
  int rc = DECODER_DRM_RETURN_OK;
  unsigned char digest[20];

  //! The digest value of SenderID is calculated (@ref TZ_calc_digest)
  TZ_calc_digest(_ldb_key, TZ_SENDER_ID_LEN, digest);

  //! The one that 16 head bytes of the digest value were scrambled is a secret_key (@ref TZ_Marlin_TamperKey).
  TZ_Marlin_TamperKey((unsigned char*) digest, secret_key, 16);

  (void) memset(digest, 0x00, sizeof(digest));

  return DECODER_DRM_RETURN_OK;
}


/*! Opens an environment, on the Secure Service, of decrypting an encrypted content for VOD and the IP multicast license.
 *
 * @param dec_slot         [output] Points to an area to which the slot number of the @ref DecoderDRM environment
 *                                  opened in the Secure Service is returned.
 * @param license_db       [input]  Specifies the pathname of a license database file. This argument SHALL be a
 *                                  NULL-terminated ASCII string which is not more than 256 bytes including the 
 *                                  terminating NULL byte. If this argument is a zero-length string, this function 
 *                                  returns an error.
 *                                  If the file with the specified pathname doesn't exist, this function returns an error. 
 *                                  In the case of opening an environment for decrypting an encrypted content 
 *                                  specifying a VOD license, you SHALL specify a NULL pointer for this argument. 
 * @param secret_data_type [input]  Specifies which type of terminal-specific certificates and private key
 *                                  to use by the following macro.
 *
 *                                    @li @ref MEDIA_DRM_SECRET_DATA_GENERAL(1) Specifies a private key and certificates for the general service.
 *                                    @li @ref MEDIA_DRM_SECRET_DATA_RTDB(2)    Specifies a private key and certificates for the Retransmission of
 *                                                                         the Terrestrial Digital Broadcast (RTDB), the retransmission of BS
 *                                                                         digital broadcasting service.
 *                                    @li @ref MEDIA_DRM_SECRET_DATA_OTHER(3)   Specifies other private key and certificates.
 * @param license_obj      [input]  Points to an encrypted object data (400 bytes fixed) of the VOD license. In
 *                                  the case of decrypting an encrypted content data with a VOD license, you need 
 *                                  to search this data and take it from the license DB in advance, and pass it as this argument. 
 *                                  This argument SHALL be a NULL pointer except for the case of VOD. 
 * @param license_obj_len  [input]  Specifies the length in bytes of the object data stored in license_obj. It is always 400. 
 *                                  This argument SHALL be 0 except for the case of VOD.
 * @param lid              [input]  Points to a license ID of the IP multicast. lid is specified by the binary data 
 *                                  (16 bytes). When the length of license ID is less than 16 bytes, the rest part 
 *                                  SHALL be padded with 0. 
 *                                  This argument SHALL be a NULL pointer except for the case of IP multicast. 
 * @param lid_len          [input]  Specifies the size of license ID(lid). The value of lid_len SHALL be 16 or 0. 
 *                                  This argument SHALL be 0 except for the case of IP multicast. 
 * @param extract_info     [output] Points to a ::DRM_ExtractInfo structure. This argument is intended to use in 
 *                                  the case of VOD. When NULL is specified for extract_info, information is not acquired.   
 * @param inf              [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_NO_LICENSE_ERROR(-3) Specified license is not found.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 *
 * @note 
 */
int DecoderDRM_TZ_Open(int* dec_slot, char* license_db, int secret_data_type, char* license_obj, size_t license_obj_len,
		       char* lid, size_t lid_len, DRM_ExtractInfo* extract_info, DRM_ErrorInf* inf)
{
  int i;
  int rc = DECODER_DRM_RETURN_OK;
  char id[TZ_LICENSE_ID_LEN * 2 + 1]; //! LID of 32 characters.
  unsigned char req_lid[16];
  unsigned char* content_key_id = NULL;
  //  MarlinDRM* marlin_drm = NULL;
  DecoderDRM_TZ* obj = NULL;
  MarlinLicense_TZ* license;
  ExtractInfo_TZ* ext = NULL;
  unsigned char secret_key[16];
  unsigned char tmp_buff[TZ_ENC_LICENSE_SIZE];
  unsigned char root_cert_buf[1280];
  unsigned char ca_cert_buf[1280];
  unsigned char client_cert_buf[1280];
  unsigned char client_key_buf[128];
  unsigned char sender_id[32];
  size_t root_cert_len = sizeof(root_cert_buf);
  size_t ca_cert_len = sizeof(ca_cert_buf);
  size_t client_cert_len = sizeof(client_cert_buf);
  size_t client_key_len = sizeof(client_key_buf);
  unsigned char* tmp = NULL;
  char subject[32 + 1];
  size_t sender_id_len = TZ_SENDER_ID_LEN * 2;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!dec_slot || !license_obj || !((license_obj_len == TZ_ENC_LICENSE_SIZE) || (license_obj_len == 0)) ||
    !((lid_len == 16) || (lid_len == 0)) || !inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00421);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  if ((rc = register_cipher(&aes_desc)) == -1) { //! registration of tomcrypt's AES
    goto EXIT;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //! The cryptographic key to license_obj is generated.
  ////////////////////////////////////////////////////////////////////////////////
  rc = TrustZone_GetSecretData(secret_data_type,
				root_cert_buf,   sizeof(root_cert_buf),   &root_cert_len,
				ca_cert_buf,     sizeof(ca_cert_buf),     &ca_cert_len,
				client_cert_buf, sizeof(client_cert_buf), &client_cert_len,
				client_key_buf,  sizeof(client_key_buf),  &client_key_len);
  if (rc != MEDIA_DRM_RETURN_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00444);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  rc = TZ_cert_get_subject(client_cert_buf, client_cert_len, (unsigned char**) &tmp);
  if (rc != DECODER_DRM_RETURN_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00445);
    goto EXIT;
  }

  (void) memset(subject, 0x00, sizeof(subject));
  (void) memcpy(subject, tmp, TZ_SENDER_ID_LEN * 2);

  rc = TZ_hex2bin(subject, sender_id, &sender_id_len);
  if (rc != DECODER_DRM_RETURN_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00450);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  rc = TZ_CreateLDBKey(sender_id, secret_key);
  if (rc != DECODER_DRM_RETURN_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00446);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //! When license_obj is decoded, the ::MarlinLicense_TZ structure is taken out.
  ////////////////////////////////////////////////////////////////////////////////
  (void) memcpy(tmp_buff, license_obj, license_obj_len);
  rc = TZ_crypt_buffer((unsigned char*) tmp_buff, license_obj_len, NULL, 0, (const unsigned char*) secret_key, NULL, 1);
  if (rc != 0) {
    rc = DECODER_DRM_INTERNAL_ERROR;
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00428);
    goto EXIT;
  }
  (void) memset(secret_key, 0x00, 16);
  license = (MarlinLicense_TZ*) tmp_buff;

  *dec_slot = 0;
  obj = NULL;
  for (i = 0; i < SECURE_AREA_MAX_DECODE_ENVIRONMENT; ++i) {
    if (__g_trust_zone_anchor.ddrm[i] == NULL) {
      if ((__g_trust_zone_anchor.ddrm[i] = (DecoderDRM_TZ*) malloc(sizeof(DecoderDRM_TZ))) == NULL) {
	TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00447);
	rc = DECODER_DRM_INTERNAL_ERROR;
	goto EXIT;
      }
      if ((__g_trust_zone_anchor.ddrm[i]->license_obj = (unsigned char*) malloc(TZ_ENC_LICENSE_SIZE)) == NULL) {
	free((void*) __g_trust_zone_anchor.ddrm[i]);
	__g_trust_zone_anchor.ddrm[i] = NULL;
	TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00448);
	rc = DECODER_DRM_INTERNAL_ERROR;
	goto EXIT;
      }
      obj = __g_trust_zone_anchor.ddrm[i];
      *dec_slot = i;
      break;
    }
  }
  if (obj == NULL) {
    //! The management table of the decoding environment became overcapacity
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00449);
    rc = DECODER_DRM_INTERNAL_ERROR;
    goto EXIT;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //! Various information is taken outof the decoded ::MarlinLicense_TZ structure.
  ////////////////////////////////////////////////////////////////////////////////
  (void) memcpy(obj->license_obj, license_obj, license_obj_len);
  (void) memcpy(obj->lid, license->license_id, 16);
  (void) memcpy(obj->key, license->content_key, 16);

#ifdef CC_DEBUG
  Printf("*******************\ncontent key:\n");
  for(i = 0; i < 16; i++)
  {
	Printf("0x%x ", obj->key[i]);
  }
  Printf("\n*******************\n");
#endif

  obj->ip_service_provider_id = license->ip_service_provider_id;

  ext = &license->extract_info;
  if (ext) {
    obj->not_before = ext->not_before;
    obj->not_after  = ext->not_after;
    obj->rendering_obligation[0] =
	((ext->digital_recording_control_data << 6) |
	 (ext->copy_control_type              << 4) |
	 (ext->aps_control_data               << 2) |
	 (ext->image_constraint_token         << 1) |
	  ext->retention_mode);
    obj->rendering_obligation[1] =
        ((ext->retention_state << 5) |
         (ext->encryption_mode << 4) |
          ext->user_defined_data);
  }

  if (extract_info) {
    extract_info->ip_service_provider_id = obj->ip_service_provider_id;

    if (ext != NULL) {
      extract_info->not_before = obj->not_before;
      extract_info->not_after  = obj->not_after;
      extract_info->rendering_obligation[0] = obj->rendering_obligation[0];
      extract_info->rendering_obligation[1] = obj->rendering_obligation[1];
      extract_info->reserve[0] = 0;
    }
  }
  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  (void) memset(tmp_buff, 0x00, TZ_ENC_LICENSE_SIZE);
  (void) memset(client_key_buf, 0x00, sizeof(client_key_buf));
  (void) memset(sender_id, 0x00, sizeof(sender_id));

  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_Open", rc, inf, NULL);
#endif
  }

  return rc;
}


/*! Closes an environment of decrypting an encrypted content on the Secure Service.
 *
 * @param dec_slot         [input]  Specifies the slot number of the decrypting environment returned from @ref DecoderDRM_TZ_Open().
 * @param inf              [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_SEQUENCE_ERROR(-2)   @ref DecoderDRM object has not been initialized.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 *
 * @note If you closed the DecoderDRM environment, by @ref DecoderDRM_TZ_Close() function, that the 
 *       following API functions were using on other thread(s), the DRM is not guaranteed to work correctly. 
 *         @li @ref DecoderDRM_TZ_DecodePackets_VOD() function 
 *         @li @ref DecoderDRM_TZ_DecodeECM() function 
 *         @li @ref DecoderDRM_TZ_DecodePackets_IPMC() function 
 */
int DecoderDRM_TZ_Close(int dec_slot, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00420);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  (void) memset(__g_trust_zone_anchor.ddrm[dec_slot]->license_obj, 0x00, TZ_ENC_LICENSE_SIZE);
  free((void*) __g_trust_zone_anchor.ddrm[dec_slot]->license_obj);
  __g_trust_zone_anchor.ddrm[dec_slot]->license_obj = NULL;

  (void) memset(__g_trust_zone_anchor.ddrm[dec_slot], 0x00, sizeof(DecoderDRM_TZ));
  free((void*) __g_trust_zone_anchor.ddrm[dec_slot]);
  __g_trust_zone_anchor.ddrm[dec_slot] = NULL;


  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  //if (unregister_cipher(&aes_desc) == -1) { //! unregistration of tomcrypt's AES
  //  rc = -1;
  //}

  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_Close", rc, inf, NULL);
#endif
  }
  return rc;
}


/*! Decrypts, on the Secure Service, a VOD content with the content key contained in the matching license.
 *
 * You specify, as the encrypted data, an array of in_num_of_packets elements, and each
 * element is a packet_size-byte data (hereinafter "packet". A TS or TTS packet). You specify,
 * with offsets argument, the encrypted part in each packet (payload. Up to 1 part in each
 * packet). As for the decrypting result, you are able to select either overwriting the input data
 * (encrypted data) with it or storing it in the other buffer area.
 *
 * @param dec_slot           [input]  The slot number of the decrypting environment returned from
 *                                    @ref DecoderDRM_TZ_Open() is specified with dec_slot.
 * @param packet_size        [input]  The packet size in bytes is specified with packet_size.
 * @param in_buff            [input/output] The encrypted data (TS packets etc.) is specified with in_buff.
 *                                          The size of the encrypted data is (packet_size * in_num_of_packets) bytes.
 *                                          If NULL is specified for out_buff, the decrypted data is stored
 *                                          in in_buff after executing this function.
 * @param in_num_of_packets  [input]  The number of packets specified for in_buff is specified with in_num_of_packets.
 * @param offsets            [input]  The part to decrypt in each packet stored in in_buff is specified with
 *                                    offsets. It is specified as an array, which has in_num_of_packets elements,
 *                                    of the @ref DecoderDRM_Offset structure. 
 * @param out_buff           [output] The decoded data is stored in out_buff. If NULL is specified for out_buff,
 *                                    the area pointed by in_buff is overwritten with the decrypted data.
 * @param out_buff_len       [input]  The  size  in bytes of  the area pointed by out_buff  is specified with 
 *                                    out_buff_len. This value SHALL be not less than (packet_size * in_num_of_packets).
 *                                    If NULL is specified for out_buff, out_buff_len is ignored.
 * @param out_num_of_packets [output] The number of packets that the DRM succeeded in decrypting is stored in
 *                                    out_num_of_packets. If this function returned the success, the value pointed
 *                                    by out_num_of_packets becomes the same as in_num_of_packets.
 * @param inf                [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 *
 * @note This API is the one providing the correspondent function with the
 *       @ref DecoderDRM_DecodePayload_VOD() function, the
 *       @ref DecoderDRM_DecodePayload_VOD_annex() function for the case of using the Trust Zone.
 */
int DecoderDRM_TZ_DecodePackets_VOD(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets, 
				    DecoderDRM_Offset* offsets, void* out_buff, size_t out_buff_len,
				    size_t* out_num_of_packets, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;
  int i;
  DecoderDRM_TZ* obj = NULL;
  unsigned char* pi;
  unsigned char* po;
  int alloc_flag = 0;
  size_t out_len;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!packet_size || !in_buff || !in_num_of_packets || !offsets ||
      (out_buff && out_buff_len < packet_size * in_num_of_packets) || !out_num_of_packets || !inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00422);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  obj = __g_trust_zone_anchor.ddrm[dec_slot];
  if (obj == NULL) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00451);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  if (out_buff == NULL) {
    if ((out_buff = (void*) malloc(packet_size * in_num_of_packets)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00453);
      rc = DECODER_DRM_INTERNAL_ERROR;
      goto EXIT;
    }
    (void) memset(out_buff, 0x00, packet_size * in_num_of_packets);
    alloc_flag = 1;
  }
  
  (void) memcpy(out_buff, in_buff, packet_size * in_num_of_packets);

  for (i = 0; i < in_num_of_packets; ++i) {
    if (offsets[i].size) { //! For the packet to be decoded
      pi = (unsigned char*) in_buff + packet_size * i + offsets[i].offset;
      po = (unsigned char*) out_buff + packet_size * i + offsets[i].offset;
      rc = TZ_crypt_buffer(pi, offsets[i].size,
			   po, offsets[i].size, obj->key, NULL, 1);
      if (rc != DECODER_DRM_RETURN_OK) {
	TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00452);
	rc = DECODER_DRM_INTERNAL_ERROR;
	goto EXIT;
      }
    }
    *out_num_of_packets = i+1;
  } /* for i */

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  if (alloc_flag) {
    if (rc == DECODER_DRM_RETURN_OK) {
      (void) memcpy(in_buff, out_buff, packet_size * *(out_num_of_packets));
    }
    (void) memset(out_buff, 0x00, packet_size * in_num_of_packets);
    free((void*) out_buff);
  }
  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_DecodePackets_VOD", rc, inf, NULL);
#endif
  }
  return rc;
}


/*! Decodes, on the Secure Service, an ECM of IP multicast with the content key (the work key).
 *
 * @param dec_slot         [input]  The slot number of the decrypting environment returned from
 *                                    @ref DecoderDRM_TZ_Open() is specified with dec_slot.
 * @param in_buff          [input/output] The encrypted data is specified with in_buff. If NULL is specified for out_buff,
 *                                        the decrypted data is stored in in_buff after executing this function.
 * @param in_buff_len      [input]  The size of the encrypted data stored in in_buff is specified with in_buff_len.
 * @param out_buff         [output] The decoded data is stored in out_buff. If NULL is specified for out_buff,
 *                                  the area pointed by in_buff is overwritten with the decrypted data.
 * @param out_buff_len     [input]  The size of the area pointed by out_buff is specified with out_buff_len.
 *                                  This value SHALL be not less than in_buff_len. If NULL is specified for out_buff,
 *                                  out_buff_len is ignored. 
 * @param out_len          [output] The decoded data size is stored in out_len (out_len becomes the same length as in_buff_len).
 * @param inf              [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_NO_LICENSE_ERROR(-3) Specified license is not found.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 * @retval DECODER_DRM_LICENSE_EXPIRE(-6)   It is not possible to decrypt the data because date-time of Notafter has passed.
 * @retval DECODER_DRM_NOT_AVAILABLE(-7)    It is not possible to decrypt the data because there is not any true bit corresponding to necessary subscription.
 *
 * @note This API is the one providing the correspondent function with the @ref DecoderDRM_DecodeECM
 *       function, the DecoderDRM_DecodeECM_annex function for the case of using the Trust Zone.
 *       The fraction part of the multiple of 16 is decoded with OBF within the range of the encryption of
 *       ECM (However, there is no fraction because it is 80 bytes under the present situation).
 *       The range of the encryption of ECM is at 18-97 bytes from the head of in_buff.
 *       This function uses 0 as the initialization vector.
 *       This function acquires the license (work key) corresponding to contents key ID of specified ECM
 *       from license DB. When the corresponding license is not acquired,
 *       @ref DECODER_DRM_NO_LICENSE_ERROR is returned.
 *       If this function failed to read the license DB because of a conflict with the @ref MediaDRM_LoadLicense
 *       function or the MediaDRM_DownloadLicenses function, it selects the correspondent license (work
 *       key) from the previous cache of licenses on the DecoderDRM object and uses it for decrypting the
 *       ECM. If it failed to get the license from the license DB because of the above conflict and if there's
 *       no previous cache of license on the DecoderDRM object, it returns
 *       @ref DECODER_DRM_INTERNAL_ERROR.
 */
int DecoderDRM_TZ_DecodeECM(int dec_slot, void* in_buff, size_t in_buff_len,
			    void* out_buff, size_t out_buff_len, size_t* out_len, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!in_buff || !in_buff_len || !out_len || !inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00423);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  //! Write some code here.

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_DecodeECM", rc, inf, NULL);
#endif
  }
  return rc;
}


/*! Decrypts, on the Secure Service, a content data of IP multicast.
 *
 * You specify, as the encrypted data, an array of in_num_of_packets elements, and each 
 * element is a packet_size-byte data (hereinafter "packet". A TS or TTS packet). You specify, 
 * with offsets argument, the encrypted part in each packet (payload. Up to 1 part in each 
 * packet). As for the decrypting result, you are able to select either overwriting the input data 
 * (encrypted data) with it or storing it in the other buffer area.
 *
 * @param dec_slot           [input]  The slot number of the decrypting environment returned from
 *                                    @ref DecoderDRM_TZ_Open() is specified with dec_slot.
 * @param packet_size        [input]  The packet size in bytes is specified with packet_size.
 * @param in_buff            [input/output] The encrypted data (TS packets etc.) is specified with in_buff. The size
 *                                          of the encrypted data is (packet_size * in_num_of_packets) bytes. If
 *                                          NULL is specified for out_buff, the decrypted data is stored in in_buff
 *                                          after executing this function.
 * @param in_num_of_packets  [input]  The number of packets specified for in_buff is specified with in_num_of_packets
 * @param offsets            [input]  The part to decrypt in each packet stored in in_buff is specified with
 *                                    offsets. It is specified as an array, which has in_num_of_packets elements, 
 *                                    of the @ref DecoderDRM_Offset structure (see @ref DecoderDRM_TZ_DecodePackets_VOD).
 * @param out_buff           [output] The decoded data is stored in out_buff. If NULL is specified for out_buff,
 *                                    the area pointed by in_buff is overwritten with the decrypted data.
 * @param out_buff_len       [input]  The size in bytes of the area pointed by out_buff is specified with
 *                                    out_buff_len. This value SHALL be not less than (packet_size * in_num_of_packets).
 *                                    If NULL is specified for out_buff, out_buff_len is ignored.
 * @param out_num_of_packets [output] The number of packets that the DRM succeeded in decrypting is stored in
 *                                    out_num_of_packets. If this function returned the success, the value
 *                                    pointed by out_num_of_packets becomes the same as in_num_of_packets.
 * @param mode               [input]  The type of the scramble key used for decoding is specified in mode. Either of
 *                                    the following SHALL be specified.
 *                                      @li @ref TZ_SCRAMBLE_KEY_ODD(1) Scramble key (odd number)
 *                                      @li @ref TZ_SCRAMBLE_KEY_EVEN(2) Scramble key (even number)
 * @param inf                [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 *
 * @note This API is the one providing the correspondent function with the
 *       @ref DecoderDRM_DecodePayload_IPMC function, the
 *       @ref DecoderDRM_DecodePayload_IPMC_annex function for the case of using the Trust Zone.
 */
int DecoderDRM_TZ_DecodePackets_IPMC(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets,
				     DecoderDRM_Offset* offsets, void* out_buff, size_t  out_buff_len,
				     size_t* out_num_of_packets, unsigned long mode, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!packet_size || !in_buff || !in_num_of_packets || !offsets || !out_num_of_packets ||
      !((mode == TZ_SCRAMBLE_KEY_ODD) || (mode == TZ_SCRAMBLE_KEY_EVEN)) || !inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00424);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  //! Write some code here.

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_DecodePackets_IPMC", rc, inf, NULL);
#endif
  }
  return rc;
}


/*! Opens an environment, on the Secure Service, of decrypting an encrypted content for the download license.
 *
 * @param dec_slot         [output] The slot number of the DecoderDRM environment opened in the Secure
 *                                  Service is stored in dec_slot.
 * @param lid              [input] License ID of DOWNLOAD is specified with lid. lid is specified by the
 *                                 binary data (16 bytes). When the length of license ID is less than 16 bytes, the
 *                                 rest part SHALL be padded with 0.
 * @param lid_len          [input] The size of license ID (lid) is specified with lid_len. The value of lid_len SHALL be 16
 * @param extra_time       [input] The encrypted contents can be decoded only for extra_time [seconds] in
 *                                 addition when it reaches reproduction time limit (not_after) after this
 *                                 function is executed. extra_time is specified in seconds. The value of 0
 *                                 second ~ 360000 seconds (100 hours) can be specified for extra_time.
 *                                 Please specify 14400 seconds (4 hours).
 * @param license_type     [output] The type of the license is stored in license_type. The returned value is the following.
 *                                    @li @ref MEDIA_DRM_LICENSE_TYPE_EXTRACT(1) EXTRACT license. License information is stored in extract_info.
 *                                    @li @ref MEDIA_DRM_LICENSE_TYPE_EXPORT(2)  EXPORT license. License information is stored in export_info.
 * @param extract_info     [output] When license_type is @ref MEDIA_DRM_LICENSE_TYPE_EXTRACT,
 *                                  license information is stored in extract_info. When the NULL
 *                                  pointer is specified for extract_info, license information is not
 *                                  stored. Please refer to "@ref MediaDRM_ListCachedID_DL" of this
 *                                  specifications for the structure of the @ref DRM_ExtractInfo.
 * @param export_info      [output] When license_type is @ref MEDIA_DRM_LICENSE_TYPE_EXPORT,
 *                                  license information is stored in export_info. When the NULL
 *                                  pointer is specified for export_info, license information is not stored.
 *                                  Please refer to "@ref MediaDRM_ListCachedID_DL" of this
 *                                  specifications for the structure of the @ref DRM_ExportInfo
 * @param inf              [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_NO_LICENSE_ERROR(-3) Specified license is not found.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 * @retval DECODER_DRM_LICENSE_EXPIRE(-6)   It is not possible to decrypt the data because date-time of Notafter has passed.
 *
 * @note If an EXTRACT license is specified and if in the following cases, this function returns
 *       @ref DECODER_DRM_LICENSE_EXPIRE.
 *         @li Time now is before the action starting date (not_before).
 *         @li Time now is after the action completion date (not_after).
 *       If it reached the reproduction time limit after this function is executed, the
 *       @ref DecoderDRM_TZ_DecodePackets_DL() function can decode the encrypted contents data
 *       until becoming "Action completion date (not_after) + extra_time". extra_time is specified by
 *       the argument of this function.<BR>
 *       When a license other than the download license (VOD, IPMC) is specified, this function returns
 *       @ref DECODER_DRM_ARG_ERROR.
 *       It is necessary to delete the EXPORT license by using the @ref MediaDRM_DeleteCachedID_DL()
 *       function when outputting the download contents to external media is completed. It SHALL be when
 *       export of the download contents ends completely that you delete the EXPORT license, because
 *       export to external media might be interrupted.<BR>
 *       Do not enter the following states.
 *         @li Case where the corresponding license is deleted before export of download contents to external
 *             medium completely ends. As a result, acquiring a new export license is needed to output the
 *             remainder of the contents data to an external medium.
 *         @li Case where the corresponding license remains in license data base file though export of
 *             download contents to external medium completely ended. As a result, export of the download
 *             contents can be done again.
 */
int DecoderDRM_TZ_Open_DL(int* dec_slot, char* lid, size_t lid_len, unsigned int extra_time, int* license_type,
			  DRM_ExtractInfo* extract_info, DRM_ExportInfo* export_info, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!dec_slot || !((0 <= extra_time) && (extra_time <= 360000)) || !license_type || !inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00425);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  //! Write some code here.

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_Open_DL", rc, inf, NULL);
#endif
  }
  return rc;
}


/*! Closes the decoding environment for the download license opened on the Secure Service.
 *
 * @param dec_slot         [input]  The slot number of the decrypting environment returned from
 *                                  @ref DecoderDRM_TZ_Open_DL() is specified with dec_slot.
 * @param inf              [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_SEQUENCE_ERROR(-2)   @ref DecoderDRM object has not been initialized.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 *
 * @note When the slot number of the decrypting environment that was returned from the
 *       @ref DecoderDRM_TZ_Open() function is specified for this function, this function returns
 *       @ref DECODER_DRM_SEQUENCE_ERROR.
 * @note If you closed a decrypting environment, by @ref DecoderDRM_TZ_Close_DL() function, that the
 *       following API function was using on other thread(s), the DRM is not guaranteed to work correctly.
 *         @li @ref DecoderDRM_TZ_DecodePackets_DL() function
 */
int DecoderDRM_TZ_Close_DL(int dec_slot, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00426);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  //! Write some code here.

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_Close_DL", rc, inf, NULL);
#endif
  }
  return rc;
}


/*! This function decrypts the encrypted content data with the content key of the download license.
 *
 * You specify, as the encrypted data, an array of in_num_of_packets elements, and each
 * element is a packet_size-byte data (hereinafter "packet". A TS or TTS packet). You specify,
 * with offsets argument, the encrypted part in each packet (payload. Up to 1 part in each
 * packet). As for the decrypting result, you are able to select either overwriting the input data
 * (encrypted data) with it or storing it in the other buffer area.
 *
 * @param dec_slot           [input]  The slot number of the decrypting environment returned from
 *                                    @ref DecoderDRM_TZ_Open_DL() is specified with dec_slot.
 * @param packet_size        [input]  The packet size in bytes is specified with packet_size.
 * @param in_buff            [input/output] The encrypted data (TS packets etc.) is specified with in_buff.
 *                                          The size of the encrypted data is (packet_size * in_num_of_packets) bytes.
 *                                          If NULL is specified for out_buff, the decrypted data is stored
 *                                          in in_buff after executing this function.
 * @param in_num_of_packets  [input]  The number of packets specified for in_buff is specified with in_num_of_packets.
 * @param offsets            [input]  The part to decrypt in each packet stored in in_buff is specified with
 *                                    offsets. It is specified as an array, which has in_num_of_packets elements,
 *                                    of the @ref DecoderDRM_Offset structure (see @ref DecoderDRM_TZ_DecodePackets_VOD).
 * @param out_buff           [output] The decoded data is stored in out_buff. If NULL is specified for out_buff,
 *                                    the area pointed by in_buff is overwritten with the decrypted data.
 * @param out_buff_len       [input]  The size in bytes of the area pointed by out_buff is specified with
 *                                    out_buff_len. This value SHALL be not less than (packet_size * in_num_of_packets).
 *                                    If NULL is specified for out_buff, out_buff_len is ignored.
 * @param out_num_of_packets [output] The number of packets that the DRM succeeded in decrypting is stored in 
 *                                    out_num_of_packets. If this function returned the success, the value
 *                                    pointed by out_num_of_packets becomes the same as in_num_of_packets
 * @param inf                [output] When the error occurs, detailed error code is stored in inf.
 *
 * @retval DECODER_DRM_RETURN_OK(0)         Normal termination.
 * @retval DECODER_DRM_ARG_ERROR(-1)        Argument invalid.
 * @retval DECODER_DRM_SEQUENCE_ERROR(-2)   @ref DecoderDRM object has not been initialized.
 * @retval DECODER_DRM_NO_LICENSE_ERROR(-3) Specified license is not found.
 * @retval DECODER_DRM_INTERNAL_ERROR(-4)   Internal error.
 * @retval DECODER_DRM_LICENSE_EXPIRE(-6)   It is not possible to decrypt the data because date-time of Notafter has passed.
 * @retval DECODER_DRM_NOT_AVAILABLE(-7)    It is not possible to decrypt the data because there is not any true bit corresponding to necessary subscription.
 *
 * @note When the slot number of the decrypting environment that was returned from the
 *       @ref DecoderDRM_TZ_Open() function is specified for this function, this function returns
 *       @ref DECODER_DRM_SEQUENCE_ERROR.
 *       This function can decode the encrypted contents data until becoming "Action completion date
 *       (not_after) + extra_time". The license, which prescribes the action completion date, and
 *       extra_time are specified by the arguments of the @ref DecoderDRM_TZ_Open_DL() function.
 *       This function returns @ref DECODER_DRM_LICENSE_EXPIRE after this.
 * @note The fraction part of the multiple of 16 in the payload is decoded with OBF.
 *       This function uses 0 as the initialization vector.
 */
int DecoderDRM_TZ_DecodePackets_DL(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets,
				   DecoderDRM_Offset* offsets, void* out_buff, size_t out_buff_len, 
				   size_t* out_num_of_packets, DRM_ErrorInf* inf)
{
  int rc = DECODER_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!packet_size || !in_buff || !in_num_of_packets || !offsets || !out_num_of_packets || !inf) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00427);
    rc = DECODER_DRM_ARG_ERROR;
    goto EXIT;
  }

  //! Write some code here.

  rc = DECODER_DRM_RETURN_OK;

 EXIT:
  if (rc != DECODER_DRM_RETURN_OK) {
    if (inf) {
      TZ_GetErrorInf(inf);
    }
#ifdef FJ_MARLIN_PLTFM_VX
    //! When an error occurs, detailed information is outputted to a log.
    PrintErrorInf("DecoderDRM_TZ_DecodePayload_DL", rc, inf, NULL);
#endif
  }
  return rc;
}


int DecoderDRM_TZ_DecodePayload_DL(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets,
				   DecoderDRM_Offset* offsets, void* out_buff, size_t out_buff_len,
				   size_t* out_num_of_packets, DRM_ErrorInf* inf)
{
    return DecoderDRM_TZ_DecodePackets_DL(dec_slot, packet_size, in_buff, in_num_of_packets, offsets, out_buff, out_buff_len, out_num_of_packets, inf);
}
