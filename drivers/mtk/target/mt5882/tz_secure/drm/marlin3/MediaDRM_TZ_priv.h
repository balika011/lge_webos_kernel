//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/tz_secure/drm/marlin3/MediaDRM_TZ_priv.h#1 $
//-----------------------------------------------------------------------------
/*! @file   MediaDRM_TZ_priv.h
 *  @brief  Things necessary to implement in MediaDRM_TZ.cpp
 *  @author Fujitsu) Yuuki Matsueda
 *  @date   2011/10/13 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#include "DecoderDRM_TZ_priv.h" /* for DecoderDRM_TZ */
#include "DRM_TZ_priv.h"
#include "ErrorDetail_TZ.h"

#ifndef __MEDIA_DRM_TZ_PRIV_H__
#define __MEDIA_DRM_TZ_PRIV_H__

//! Certificates and private key stored in the secure area
// This example program reads it from files.
#define SECURE_AREA_DIR_GENERAL "secure_area_general"
#define SECURE_AREA_FILE_ROOT_CERT	"root_cert"
#define SECURE_AREA_FILE_CA_CERT	"ca_cert"
#define SECURE_AREA_FILE_CLIENT_CERT	"client_cert"
#define SECURE_AREA_FILE_CLIENT_KEY	"client_key"

#ifdef __cplusplus
extern "C" {
#endif

  // Prototype declaration of internal functions
  int _MediaDRM_TZ_Lock(int secret_data_type);
  int _MediaDRM_TZ_UnLock(int secret_data_type);
  unsigned char* TZ_WriteASN1Header(unsigned char* write_buff, unsigned char tag, size_t _size);
  unsigned char* TZ_ReadASN1Header(unsigned char* read_buff, unsigned char* tag, size_t* _size);
  int TZ_ReadFromSecureArea(int secret_data_type,
			    unsigned char* root_cert_buf, size_t root_cert_buf_len, size_t* root_cert_len,
			    unsigned char* ca_cert_buf, size_t ca_cert_buf_len, size_t* ca_cert_len,
			    unsigned char* client_cert_buf, size_t client_cert_buf_len, size_t* client_cert_len,
			    unsigned char* client_key_buf, size_t client_key_buf_len, size_t* client_key_len);
  int TrustZone_SetSecretData(int secret_data_type,
			      const unsigned char* root_cert,   size_t root_cert_len,
			      const unsigned char* ca_cert,     size_t ca_cert_len,
			      const unsigned char* client_cert, size_t client_cert_len,
			      const unsigned char* client_key,  size_t client_key_len);

#ifdef __cplusplus
};
#endif

#endif /* __MEDIA_DRM_TZ_PRIV_H__ */
