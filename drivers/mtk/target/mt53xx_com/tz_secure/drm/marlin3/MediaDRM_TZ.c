//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/marlin3/MediaDRM_TZ.c#1 $
//-----------------------------------------------------------------------------
/*! @file   MediaDRM_TZ.cpp
 *  @brief  MediaDRM API for calling function in the Trust Zone from outside the Trust Zone
 *  @author Fujitsu) Yuuki Matsueda
 *  @date   2011/10/12 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
//#include <stdio.h> // for sprintf(), fopen(), fseek(), ftell(), rewind(), fread(), fclose()
#include <stdlib.h> // for malloc(), free()
#include <string.h> // for memcpy(), memset()

//#include "MarlinDRM.h"
//#include "MarlinLDB.h"
//#include "MediaDRM.h"
#include "MediaDRM_TZ.h"
#include "MediaDRM_TZ_priv.h"
#include "aes.h"
#include "DecoderDRM_TZ.h"
#include "tzst.h"


typedef enum
{
    DMX_DRM_MODE_BYPASS,
    DMX_DRM_MODE_AES,
    DMX_DRM_MODE_AES_SCTE52,
    DMX_DRM_MODE_ND,
    DMX_DRM_MODE_PD,
    DMX_DRM_MODE_AES_ENCRYPTION,
#ifdef DMX_SUPPORT_CPRM
    DMX_DRM_MODE_CPRM_C2D,
    DMX_DRM_MODE_CPRM_C2E,
    DMX_DRM_MODE_CPRM_C2G,
    DMX_DRM_MODE_CPRM_C2DCBC,
#endif // DMX_SUPPORT_CPRM
    DMX_DRM_MODE_UNKNOWN
} DMX_DRM_MODE_T;

typedef struct
{
    UINT16 u2KeyLen;
    UINT8 au1Key[32];
    UINT8 au1InitVector[16];
    BOOL fgCbc;
    DMX_DRM_MODE_T eMode;
} DRM_PARAM_T;

UINT8 au1SecretKey[32] = {0x09, 0xF8, 0x2D, 0xCF, 0x62, 0xCA, 0x2D, 0x9C, 0x0F, 0x9B, 0x01, 0x2E, 0xB3, 0x96, 0x67, 0xDE,
						0x72, 0x30, 0xBA, 0x44, 0x21, 0x84, 0x98, 0xEF, 0x66, 0x49, 0xF8, 0xCD, 0x18, 0xBE, 0x6C, 0xCD};

//! The anchor for referring to the area where the Trust Zone function holds data in the Secure Service
DRMControlTable __g_trust_zone_anchor;

#ifdef REQUIRE_ERROR_DETAIL
# define TZ_SET_ERROR_DETAIL(error_id) TZ_SetErrorDetail(error_id);
# define TZ_INIT_ERROR_DETAIL          TZ_InitErrorDetail();
#else /* not REQUIRE_ERROR_DETAIL */
# define TZ_SET_ERROR_DETAIL(error_id)
# define TZ_INIT_ERROR_DETAIL
#endif /* not REQUIRE_ERROR_DETAIL */

#ifdef REQUIRE_ERROR_DETAIL
int __g_TZ_error_detail_stack = 0;
unsigned long __g_TZ_error_detail_code[32];
unsigned long __g_TZ_error_detail_code3;

void TZ_SetErrorDetail(int error_id)
{
  __g_TZ_error_detail_code[__g_TZ_error_detail_stack < 32 ? __g_TZ_error_detail_stack++ : 31] = error_id;
}

void TZ_InitErrorDetail(void)
{
  __g_TZ_error_detail_stack = 0;__g_TZ_error_detail_code3 = 0;
}

// In the Debug version, the DRM prints to the screen the call stack at the point where the error occured.
// In the Release version, the DRM holds up to 5 12-bit numbers in the detailed error code which is 64-bit in total.
// The format inside the 64 bits is as follows.
//
//             code1                         code2
// +---------+---------+------+ +------+---------+---------+-----+
// | 12bits  | 12bits  |8bits | |4bits | 12bits  | 12bits  |4bits|
// +---------+---------+------+ +------+---------+---------+-----+
//   stack0    stack1       stack2       stack3    stack4   reserved
//
// The number of 1~2047 in each 12 bits corresponds to the values defined in MediaDRM_TZ.h.
// By greping the macro name in the source, you find the position where the error occured.
//
void TZ_GetErrorInf(DRM_ErrorInf* inf)
{
#ifdef DEBUG_MARLIN
  int i = 0;
  int j = 0;

  //  dprintf("CALL STACK:");
  for (i = 0; i < __g_TZ_error_detail_stack; ++i) {
    //    dprintf("#%d  %04d\n", i, __g_TZ_error_detail_code[i]);
    for (j = 0; j < 2000; ++j) {
      if (pair[j].code == 0) {
	dprintf("#%d  0x%08lx\n", i, __g_TZ_error_detail_code[i]);
	break;
      } else if (pair[j].code == __g_TZ_error_detail_code[i]) {
	dprintf("#%d  0x%08lx at %s\n", i, __g_TZ_error_detail_code[i], pair[j].locate);
	break;
      }
    }
  }
#endif

  //! First, initialize the detailed error codes.
  inf->code1 = 0;
  inf->code2 = 0;

  switch (__g_TZ_error_detail_stack) {
  case 0: // If no error, do nothing.
    break;
  case 1: // 1 step only
    inf->code1 = __g_TZ_error_detail_code[0] << 20;
    break;
  case 2: // 2 steps
    inf->code1 = (__g_TZ_error_detail_code[0] << 20) | (__g_TZ_error_detail_code[1] << 8);
    break;
  case 3: // 3 steps
    inf->code1 = (__g_TZ_error_detail_code[0] << 20) | (__g_TZ_error_detail_code[1] << 8) | (__g_TZ_error_detail_code[2] >> 4);
    inf->code2 = (__g_TZ_error_detail_code[2] & 0x0f) << 28;
    break;
  case 4: // 4 steps
    inf->code1 = (__g_TZ_error_detail_code[0] << 20) | (__g_TZ_error_detail_code[1] << 8) | (__g_TZ_error_detail_code[2] >> 4);
    inf->code2 = ((__g_TZ_error_detail_code[2] & 0x0f) << 28) | (__g_TZ_error_detail_code[3] << 16);
    break;
  case 5: // 5 steps
  default: // More than 5 steps
    inf->code1 = (__g_TZ_error_detail_code[0] << 20) | (__g_TZ_error_detail_code[1] << 8) | (__g_TZ_error_detail_code[2] >> 4);
    inf->code2 = ((__g_TZ_error_detail_code[2] & 0x0f) << 28) | (__g_TZ_error_detail_code[3] << 16) | (__g_TZ_error_detail_code[4] << 4);
    break;
  }

  inf->code3 = __g_TZ_error_detail_code3;

#ifdef DEBUG_MARLIN
  dprintf("inf->code1 = 0x%08lx\n", inf->code1);
  dprintf("inf->code2 = 0x%08lx\n", inf->code2);
  dprintf("inf->code3 = 0x%08lx (HTTP %ld, Marlin=%04lxh)\n", inf->code3, inf->code3 >> 16, inf->code3 & 0xffff);
#endif /* DEBUG_MARLIN */
}
#endif /* REQUIRE_ERROR_DETAIL */

/*! Lock the MediaDRM
 */
int _MediaDRM_TZ_Lock(int secret_data_type)
{
  // Omitted
  return 0;
}

/*! Unlock the MediaDRM
 */
int _MediaDRM_TZ_UnLock(int secret_data_type)
{
  // Omitted
  return 0;
}

static BOOL _DecryptAes(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16], au1Save[32];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;

    // Setup key scheduling
    aes_setkey_dec(&rCtx, prParam->au1Key, prParam->u2KeyLen);

    if (prParam->fgCbc)
    {
        memcpy(au1InitVector, prParam->au1InitVector, 16);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text
		memcpy(au1Save, pu1SrcBuf + u4Offset, 16);

        // descrypt
        if (prParam->fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_DECRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_DECRYPT, au1Text, au1Text);
        }

        memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

	if((prParam->eMode == DMX_DRM_MODE_AES_SCTE52) &&
        (prParam->fgCbc == TRUE) && ((u4Size - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = (UINT8)(u4Size - u4Offset);
        memcpy(au1Text, pu1SrcBuf + u4Offset, u1Cnt);   // Get plain text
		aes_setkey_enc(&rCtx, prParam->au1Key, prParam->u2KeyLen);
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1Save, au1Text, au1Texto);
        memcpy(pu1DstBuf + u4Offset, au1Texto, u1Cnt);  // Copy back
    }
    else
    {
        memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    }

    return TRUE;
}


static unsigned int _MKEY_Decrypt_A1_Data(unsigned char* buff, unsigned int buff_size)
{
	unsigned char *buff128 = NULL;
	unsigned char temp;
	unsigned int n = 0;
	unsigned int nResult = 0;

	buff128 = (unsigned char*)malloc(sizeof(unsigned char)*buff_size);
	if(buff128 == NULL) {
		goto EXIT;
	}

	memset(buff128, 0x0, buff_size);
	
	for(n = 0; n < buff_size; n++)
	{
		temp = *(buff+n);

		if(temp & 0x01)
		{
			buff128[n] = (((temp & 0xF0) >> 3) | ((temp & 0x0E) << 4)) | 0x01;
		}
		else
		{
			buff128[n] = (((temp & 0xC0) >> 5) | ((temp & 0x3E) << 2)) & 0xFE;
		}
	}

	memcpy(buff, buff128, buff_size);

	memset(buff128, 0x0, buff_size);
	free(buff128);
	nResult = 1;

	EXIT:
	return nResult;
}

/*! Write an ASN.1 header into the specified area.
 *
 *  The written data is in the format of `tag + length octets' of ASN.1.
 *
 * @param write_buff [output] Points to an area to write in. The caller SHALL allocate an area enough to write in.
 * @param tag        [input]  Specifies a tag value. It is typically 0x30.
 * @param _size      [input]  Specifies a size to write.
 *
 * @return This returns the pointer to the next to the written. I.e. it returns `write_buff + written size'.
 *
 * @note No error occurs.
 */
unsigned char* TZ_WriteASN1Header(unsigned char* write_buff, unsigned char tag, size_t _size)
{
  *(write_buff++) = tag;

  if (_size < 128) {
    *(write_buff++) = _size;
  } else if (_size < 256) {
    *(write_buff++) = 0x81;
    *(write_buff++) = _size;
  } else if (_size < 65536) {
    *(write_buff++) = 0x82;
    *(write_buff++) = _size >> 8;
    *(write_buff++) = _size & 0xff;
  } else {
    *(write_buff++) = 0x84;
    *(write_buff++) = _size >> 24;
    *(write_buff++) = (_size >> 16) & 0xff;
    *(write_buff++) = (_size >> 8) & 0xff;
    *(write_buff++) = _size & 0xff;
  }

  return write_buff;
}

/*! Read, from the specified area, a size in ASN.1 format.
 * @par
 *  The data to read is in the format `tag + length octets' of ASN.1.
 *
 * @param read_buff [input]  Points to an area to read from.
 * @param tag       [output] The read tag is returned.
 * @param _size     [output] The read size.
 *
 * @return This returns the pointer to the next to the read. I.e. it returns `read_buff + read size'.
 *
 * @note No error occurs.
 */
unsigned char* TZ_ReadASN1Header(unsigned char* read_buff, unsigned char* tag, size_t* _size)
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

/*! The function which checks whether the size header exists at the beginning of a certificate
 *
 * @param cert     [input] Points to the beginning a certificate
 * @param cert_len [input] Specifies the size of the certificate
 *
 * @retval 0  The size header exists at the beginning of the certificate.
 * @retval -1 The size header doesn't exist at the beginning of the certificate, or an error.
 */
static
int TZ_CheckCertSize(unsigned char* cert, size_t cert_len)
{
  size_t header_size = 0;
  size_t _size = 0;
  unsigned char tag1 = 0;

  if (!cert || (cert_len < 6)) {
    return -1;
  }

  header_size = (size_t) (TZ_ReadASN1Header(cert, &tag1, &_size) - cert);
  if (tag1 != 0x30) {
    return -1;
  }

  if (header_size + _size == cert_len) {
    // In this case, the size header exists.
    return 0;
  }

  return -1;
}

/*! Scramble a secret data
 *
 * @param private_key	[input] Points to a secret data, for example a private key. The caller SHALL allocate memory.
 * @param tamper_key	[output] Scrambled data is stored. The caller SHALL allocate memory.
 * @param key_len	[input] Specifies the area size of private_key and tamper_key.
 *
 * @return None
 *
 * @note The data size is the same between before and after scrambling.
 */
void TZ_Marlin_TamperKey(const unsigned char* private_key, unsigned char* tamper_key, size_t key_len)
{
  int i;
  int shift;
  unsigned char uc;

  //! Fill the area of tamper_key with 0
  (void) memset(tamper_key, 0, key_len);

  //! Execute scrambling
  for (i = 0; i < (int) key_len; ++i) {
    shift = (i % 7) + 1;
    uc = (private_key[i] & 0xf0) | (private_key[key_len - i - 1] & 0x0f);
    tamper_key[i] = (uc >> shift) | (uc << (8 - shift));
  }

  return;
}


/*! Cache certificates and private key into the Trust Zone
 *
 * @param secret_data_type	[input] Specifies which type of terminal-specific certificates and private key to use by the following macro.
 *					@li ::MEDIA_DRM_SECRET_DATA_GENERAL (1)
 *					@li ::MEDIA_DRM_SECRET_DATA_RTDB    (2)
 *					@li ::MEDIA_DRM_SECRET_DATA_OTHER   (3)
 * @param root_cert		[input] Points to a root public key certificate.
 * @param root_cert_len		[input] Specifies the size of the root public key certificate pointed to by the root_cert argument.
 * @param ca_cert		[input] Points to a client CA public key certificate.
 * @param ca_cert_len		[input] Specifies the size of the client CA public key certificate pointed to by the ca_cert argument.
 * @param client_cert		[input] Points to a client public key certificate.
 * @param client_cert_len	[input] Specifies the size of the client public key certificate pointed to by the client_cert argument.
 * @param client_key		[input] Points to a client private key.
 * @param client_key_len	[input] Specifies the size in bytes of the client private key pointed to by the client_key argument. It SHALL be 28.
 *
 * @retval ::MEDIA_DRM_RETURN_OK	Successful completion
 * @retval ::MEDIA_DRM_RETURN_ARG_ERROR	secret_data_type is invalid, or a buffer for output is of short length.
 * @retval ::MEDIA_DRM_RETURN_ERROR	Cannot allocate memory
 *
 * @note This is an internal function, so this function doesn't have to do the NULL check and 28 check of arguments.
 */
int TrustZone_SetSecretData(int secret_data_type,
			    const unsigned char* root_cert,   size_t root_cert_len,
			    const unsigned char* ca_cert,     size_t ca_cert_len,
			    const unsigned char* client_cert, size_t client_cert_len,
			    const unsigned char* client_key,  size_t client_key_len)
{
  int i;
  int rc = MEDIA_DRM_RETURN_OK;
  CertsAndKey* cache = NULL; // The place where the secret data is cached
  DRM_ErrorInf error_inf;

  //! Check arguments
  if (!root_cert || !ca_cert || !client_cert || !client_key ||
      !root_cert_len || !ca_cert_len || !client_cert_len || !client_key_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00409);
    rc = MEDIA_DRM_RETURN_ARG_ERROR;
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
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00418);
    return MEDIA_DRM_RETURN_ARG_ERROR;

  default:
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00408);
    return MEDIA_DRM_RETURN_ARG_ERROR;
  }

  //! If The secret data is already cached, free it.
  if (cache->_enc_root_cert) {
    free((void*) cache->_enc_root_cert);
    cache->_enc_root_cert = NULL;
  }
  if (cache->_enc_ca_cert) {
    free((void*) cache->_enc_ca_cert);
    cache->_enc_ca_cert = NULL;
  }
  if (cache->_enc_client_cert) {
    free((void*) cache->_enc_client_cert);
    cache->_enc_client_cert = NULL;
  }
  if (cache->_enc_client_key) {
    free((void*) cache->_enc_client_key);
    cache->_enc_client_key = NULL;
  }

  //! Allocate areas for the secret data
  if ((cache->_enc_root_cert = (unsigned char*) malloc(root_cert_len + 2)) == NULL) {
    rc = MEDIA_DRM_RETURN_ERROR;
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00025);
    goto EXIT;
  }
  if ((cache->_enc_ca_cert = (unsigned char*) malloc(ca_cert_len + 2)) == NULL) {
    rc = MEDIA_DRM_RETURN_ERROR;
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00026);
    goto EXIT;
  }
  if ((cache->_enc_client_cert = (unsigned char*) malloc(client_cert_len + 2)) == NULL) {
    rc = MEDIA_DRM_RETURN_ERROR;
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00027);
    goto EXIT;
  }
  if ((cache->_enc_client_key = (unsigned char*) malloc(client_key_len + 2)) == NULL) {
    rc = MEDIA_DRM_RETURN_ERROR;
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00028);
    goto EXIT;
  }

  //! Scramble the secret data and cache it in the Trust Zone
  cache->_enc_root_cert[0] = root_cert_len / 256;
  cache->_enc_root_cert[1] = root_cert_len % 256;
  TZ_Marlin_TamperKey(root_cert,   cache->_enc_root_cert   + 2,   root_cert_len);

  cache->_enc_ca_cert[0] = ca_cert_len / 256;
  cache->_enc_ca_cert[1] = ca_cert_len % 256;
  TZ_Marlin_TamperKey(ca_cert,     cache->_enc_ca_cert     + 2,     ca_cert_len);

  cache->_enc_client_cert[0] = client_cert_len / 256;
  cache->_enc_client_cert[1] = client_cert_len % 256;
  TZ_Marlin_TamperKey(client_cert, cache->_enc_client_cert + 2, client_cert_len);

  cache->_enc_client_key[0] = client_key_len / 256;
  cache->_enc_client_key[1] = client_key_len % 256;
  TZ_Marlin_TamperKey(client_key,  cache->_enc_client_key  + 2,  client_key_len);

  (void) memset(__g_trust_zone_anchor.ddrm, 0x00, sizeof(__g_trust_zone_anchor.ddrm));

  //! Close decrypting environments, if any
  for (i=0; i < SECURE_AREA_MAX_DECODE_ENVIRONMENT; i++) {
    if (__g_trust_zone_anchor.ddrm[i] != NULL) {
      if ((rc = DecoderDRM_TZ_Close(i, &error_inf)) != DECODER_DRM_RETURN_OK) {
	rc = MEDIA_DRM_RETURN_ERROR;
	TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00420);
	goto EXIT;
      }
    }
  }

  rc = MEDIA_DRM_RETURN_OK;

 EXIT:
  if (rc != MEDIA_DRM_RETURN_OK && cache) {
    if (cache->_enc_root_cert) {
      free((void*) cache->_enc_root_cert);
      cache->_enc_root_cert = NULL;
    }
    if (cache->_enc_ca_cert) {
      free((void*) cache->_enc_ca_cert);
      cache->_enc_ca_cert = NULL;
    }
    if (cache->_enc_client_cert) {
      free((void*) cache->_enc_client_cert);
      cache->_enc_client_cert = NULL;
    }
    if (cache->_enc_client_key) {
      free((void*) cache->_enc_client_key);
      cache->_enc_client_key = NULL;
    }
  }

  return rc;
}

int dump(void* vp, size_t size)
{
  size_t i;
  unsigned long offset = 0;
  unsigned char* buff = (unsigned char*) vp;
  int y = 0;

  while (1) {
    if (!y) {
      dprintf("  Addr     0 1  2 3  4 5  6 7  8 9  A B  C D  E F 0 2 4 6 8 A C E \n");
      dprintf("--------  ---- ---- ---- ---- ---- ---- ---- ---- ----------------\n");
    }
    dprintf("%08lx  ", offset);

    for (i = 0; i < 16; ++i) 
	{
		if (size <= offset + i)
		{
			dprintf("  ");
		}
		else
		{
			dprintf("%02x", buff[offset + i]);
		}
		
		if (i % 2) 
		{
			dprintf(" ");
		}

    }
    /*
    for (i = 0; i < 16; ++i) {
      if (size <= offset + i)
	break;
      dprintf("%c", isprint(buff[offset + i]) ? buff[offset + i] : '.');
    }
    */
    dprintf("\n");

    if (size <= offset + i)
      break;

    offset += 16;
    y++;
    if (y == 16) {
      dprintf("\n");
      y = 0;
    }
  }

  return 0;
}

/*! Read certificates and private key from the secure area
 * See Inspirium DRM library API specification for details.
 */
int MediaDRM_TZ_LoadSecretDataType(const char* license_db, int secret_data_type,
				  unsigned char* root_cert_buf, size_t root_cert_buf_len, size_t* root_cert_len,
				  unsigned char* ca_cert_buf, size_t ca_cert_buf_len, size_t* ca_cert_len,
				  unsigned char* client_cert_buf, size_t client_cert_buf_len, size_t* client_cert_len,
				  unsigned char* client_key_buf, size_t client_key_buf_len, size_t* client_key_len,
				  DRM_ErrorInf* error_inf)
{
  int rc = MEDIA_DRM_RETURN_OK;
  int lock_flg = 0;
  unsigned char* root_cert_with_size_header = NULL;
  unsigned char* ca_cert_with_size_header = NULL;
  unsigned char* client_cert_with_size_header = NULL;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!root_cert_buf || !ca_cert_buf || !client_cert_buf || !client_key_buf || !error_inf ||
      !root_cert_len || !ca_cert_len || !client_cert_len || !client_key_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00005);
    rc = MEDIA_DRM_RETURN_ARG_ERROR;
    goto EXIT;
  }
  if ((secret_data_type != MEDIA_DRM_SECRET_DATA_GENERAL) &&
      (secret_data_type != MEDIA_DRM_SECRET_DATA_RTDB) &&
      (secret_data_type != MEDIA_DRM_SECRET_DATA_OTHER)) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00406);
    rc = MEDIA_DRM_RETURN_ARG_ERROR;
    goto EXIT;
  }

  {
	DRM_PARAM_T rParam;
    UINT32 i;

	// setup parameter
	rParam.u2KeyLen = 256;
	rParam.fgCbc = TRUE;
	rParam.eMode = DMX_DRM_MODE_AES_SCTE52;

	for(i = 0; i < 32; i++)
	{
		rParam.au1Key[i] = au1SecretKey[i];
	}

	for(i = 0; i < 16; i++)
	{
		rParam.au1InitVector[i] = i;
	}

	_DecryptAes((UINT32)root_cert_buf, (UINT32)root_cert_buf, *root_cert_len, &rParam);
	_DecryptAes((UINT32)ca_cert_buf, (UINT32)ca_cert_buf, *ca_cert_len, &rParam);
	_DecryptAes((UINT32)client_cert_buf, (UINT32)client_cert_buf, *client_cert_len, &rParam);
	_DecryptAes((UINT32)client_key_buf, (UINT32)client_key_buf, *client_key_len, &rParam);

  }
  _MKEY_Decrypt_A1_Data(root_cert_buf, *root_cert_len);
  _MKEY_Decrypt_A1_Data(ca_cert_buf, *ca_cert_len);
  _MKEY_Decrypt_A1_Data(client_cert_buf, *client_cert_len);
  _MKEY_Decrypt_A1_Data(client_key_buf, *client_key_len);

#ifdef CC_DEBUG
    dprintf("\n root_cert_len: %d\n", *root_cert_len);
    dump(root_cert_buf, *root_cert_len);
    dprintf("\n ca_cert_buf: %d\n", *ca_cert_len);
    dump(ca_cert_buf, *ca_cert_len);
    dprintf("\n client_cert_len: %d\n", *client_cert_len);
    dump(client_cert_buf, *client_cert_len);
    dprintf("\n client_key_len: %d\n", *client_key_len);
    dump(client_key_buf, *client_key_len);
#endif

  ////////////////////////////////////////////////////////////////////////////////
  //! Lock the MediaDRM (@ref _MediaDRM_TZ_Lock function)
  ////////////////////////////////////////////////////////////////////////////////
  (void) _MediaDRM_TZ_Lock(secret_data_type);
  lock_flg = 1;

  //! Check whether the size header exists in the certificates
  if (TZ_CheckCertSize(root_cert_buf, *root_cert_len) == -1) {
    size_t write_size = 0;
    if ((root_cert_with_size_header = (unsigned char*) malloc(*root_cert_len + 8)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00249);
      rc = MEDIA_DRM_RETURN_ERROR;
      goto EXIT;
    }
    // Write tag, length and value into root_cert_with_size_header, and copy it back to root_cert_buf
    write_size = TZ_WriteASN1Header(root_cert_with_size_header, 0x30, *root_cert_len) - root_cert_with_size_header; /* This function doesn't return an error. */
    (void) memcpy(root_cert_with_size_header + write_size, root_cert_buf, *root_cert_len);
    *root_cert_len += write_size;
    if (*root_cert_len > root_cert_buf_len) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00402);
      rc = MEDIA_DRM_RETURN_ERROR;
      goto EXIT;
    }
    (void) memcpy(root_cert_buf, root_cert_with_size_header, *root_cert_len);
  }

  if (TZ_CheckCertSize(ca_cert_buf, *ca_cert_len) == -1) {
    size_t write_size = 0;
    if ((ca_cert_with_size_header = (unsigned char*) malloc(*ca_cert_len + 8)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00250);
      rc = MEDIA_DRM_RETURN_ERROR;
      goto EXIT;
    }
    // Write tag, length and value into ca_cert_with_size_header, and copy it back to ca_cert_buf
    write_size = TZ_WriteASN1Header(ca_cert_with_size_header, 0x30, *ca_cert_len) - ca_cert_with_size_header; /* This function doesn't return an error. */
    (void) memcpy(ca_cert_with_size_header + write_size, ca_cert_buf, *ca_cert_len);
    *ca_cert_len += write_size;
    if (*ca_cert_len > ca_cert_buf_len) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00403);
      rc = MEDIA_DRM_RETURN_ERROR;
      goto EXIT;
    }
    (void) memcpy(ca_cert_buf, ca_cert_with_size_header, *ca_cert_len);
  }

  if (TZ_CheckCertSize(client_cert_buf, *client_cert_len) == -1) {
    size_t write_size = 0;
    if ((client_cert_with_size_header = (unsigned char*) malloc(*client_cert_len + 8)) == NULL) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00251);
      rc = MEDIA_DRM_RETURN_ERROR;
      goto EXIT;
    }
    // Write tag, length and value into client_cert_with_size_header, and copy it back to client_cert_buf
    write_size = TZ_WriteASN1Header(client_cert_with_size_header, 0x30, *client_cert_len) - client_cert_with_size_header; /* This function doesn't return an error. */
    (void) memcpy(client_cert_with_size_header + write_size, client_cert_buf, *client_cert_len);
    *client_cert_len += write_size;
    if (*client_cert_len > client_cert_buf_len) {
      TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00404);
      rc = MEDIA_DRM_RETURN_ERROR;
      goto EXIT;
    }
    (void) memcpy(client_cert_buf, client_cert_with_size_header, *client_cert_len);
  }

  //! Cache certificates and private key into the Trust Zone
  if ((rc = TrustZone_SetSecretData(secret_data_type,
				    root_cert_buf,   *root_cert_len,
				    ca_cert_buf,     *ca_cert_len,
				    client_cert_buf, *client_cert_len,
				    client_key_buf,  *client_key_len)) != MEDIA_DRM_RETURN_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00405);
    rc = MEDIA_DRM_RETURN_ERROR;
    goto EXIT;
  }

  rc = MEDIA_DRM_RETURN_OK;

 EXIT:
  //! If work areas are allocated, free them.
  if (root_cert_with_size_header) {
    free((void*) root_cert_with_size_header);
  }
  if (ca_cert_with_size_header) {
    free((void*) ca_cert_with_size_header);
  }
  if (client_cert_with_size_header) {
    free((void*) client_cert_with_size_header);
  }

  if (rc != MEDIA_DRM_RETURN_OK) {
#ifdef REQUIRE_ERROR_DETAIL
    if (error_inf) {
      TZ_GetErrorInf(error_inf);
    }
#endif
  }
  if (lock_flg == 1) {
    //! Unlock the MediaDRM (@ref _MediaDRM_TZ_UnLock function)
    (void) _MediaDRM_TZ_UnLock(secret_data_type);
  }

  return rc;
}

/*! Read certificates and private key from the secure area
 * See Inspirium DRM library API specification for details.
 */
int MediaDRM_TZ_SetSecretDataType(const char* license_db, int secret_data_type,
				  unsigned char* root_cert_buf, size_t root_cert_buf_len, size_t* root_cert_len,
				  unsigned char* ca_cert_buf, size_t ca_cert_buf_len, size_t* ca_cert_len,
				  unsigned char* client_cert_buf, size_t client_cert_buf_len, size_t* client_cert_len,
				  unsigned char* client_key_buf, size_t client_key_buf_len, size_t* client_key_len,
				  DRM_ErrorInf* error_inf)
{
  int rc = MEDIA_DRM_RETURN_OK;
  int lock_flg = 0;

  //! Initialize the stack of the detailed error code
  TZ_INIT_ERROR_DETAIL;

  //! Check arguments
  if (!root_cert_buf || !ca_cert_buf || !client_cert_buf || !client_key_buf || !error_inf ||
      !root_cert_len || !ca_cert_len || !client_cert_len || !client_key_len) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00005);
    rc = MEDIA_DRM_RETURN_ARG_ERROR;
    goto EXIT;
  }
  if ((secret_data_type != MEDIA_DRM_SECRET_DATA_GENERAL) &&
      (secret_data_type != MEDIA_DRM_SECRET_DATA_RTDB) &&
      (secret_data_type != MEDIA_DRM_SECRET_DATA_OTHER)) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_054_00406);
    rc = MEDIA_DRM_RETURN_ARG_ERROR;
    goto EXIT;
  }

  // read back the clear data
  rc = TrustZone_GetSecretData(secret_data_type,
				root_cert_buf,   root_cert_buf_len,   root_cert_len,
				ca_cert_buf,     ca_cert_buf_len,     ca_cert_len,
				client_cert_buf, client_cert_buf_len, client_cert_len,
				client_key_buf,  client_key_buf_len,  client_key_len);
  if (rc != MEDIA_DRM_RETURN_OK) {
    TZ_SET_ERROR_DETAIL(MARLIN_ERROR_DETAIL_TZ_056_00444);
    rc = MEDIA_DRM_RETURN_ERROR;
    goto EXIT;
  }

  rc = MEDIA_DRM_RETURN_OK;

 EXIT:
  if (rc != MEDIA_DRM_RETURN_OK) {
#ifdef REQUIRE_ERROR_DETAIL
    if (error_inf) {
      TZ_GetErrorInf(error_inf);
    }
#endif
  }
  if (lock_flg == 1) {
    //! Unlock the MediaDRM (@ref _MediaDRM_TZ_UnLock function)
    (void) _MediaDRM_TZ_UnLock(secret_data_type);
  }

  return rc;
}

