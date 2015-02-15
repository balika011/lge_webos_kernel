//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/marlin3/DRM_TZ_priv.h#1 $
//-----------------------------------------------------------------------------
/*! @file   DRM_TZ_priv.h
 *  @brief  Things necessary to implement in MediaDRM_TZ.cpp and DecoderDRM_TZ.cpp
 *  @author Fujitsu) Yuuki Matsueda
 *  @date   2011/10/24 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#ifndef __DRM_TZ_PRIV_H__
#define __DRM_TZ_PRIV_H__

#include <stddef.h> // for size_t

//! A set of certificates and private key
typedef struct
{
  unsigned char* _enc_root_cert;
  unsigned char* _enc_ca_cert;
  unsigned char* _enc_client_cert;
  unsigned char* _enc_client_key;
} CertsAndKey;

//! The maximum of the decrypting environment which can be held.
#define SECURE_AREA_MAX_DECODE_ENVIRONMENT (16)

//! Control table of DRM library
typedef struct
{
  //! Certificates and private key for the general service
  CertsAndKey data_for_general;
  //! Certificates and private key for the RTDB and the retransmission of BS digital broadcasting service
  CertsAndKey data_for_rtdb;
  //! Another set of certificates and private key
  CertsAndKey data_for_other;

  // Decrypting environments
  struct crypt_decoder_tz* ddrm[SECURE_AREA_MAX_DECODE_ENVIRONMENT];
} DRMControlTable;

extern DRMControlTable __g_trust_zone_anchor;

#ifdef __cplusplus
extern "C" {
#endif

  // Prototype declaration of internal functions
  void TZ_Marlin_TamperKey(const unsigned char* private_key, unsigned char* tamper_key, size_t key_len);

#ifdef __cplusplus
};
#endif

#endif /* __DRM_TZ_PRIV_H__ */
