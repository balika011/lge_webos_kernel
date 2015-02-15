//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/marlin3/MediaDRM_TZ.h#1 $
//-----------------------------------------------------------------------------
/*! @file   MediaDRM_TZ.h
 *  @brief  MediaDRM API for calling function in the Trust Zone from outside the Trust Zone
 *  @author Fujitsu) Yuuki Matsueda
 *  @date   2011/10/12 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#ifndef __MEDIA_DRM_TZ_H__
#define __MEDIA_DRM_TZ_H__

#include <stddef.h> // for size_t

////////////////////////////////////////
//// Types of terminal-specific certificates and private key
//! For the general service
#define MEDIA_DRM_SECRET_DATA_GENERAL (1)
//! For the RTDB, the retransmission of BS digital broadcasting service.
#define MEDIA_DRM_SECRET_DATA_RTDB    (2)
//! Another
#define MEDIA_DRM_SECRET_DATA_OTHER   (3)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DRM_ERROR_INF_DEFINED
#define DRM_ERROR_INF_DEFINED
  // Copy from ErrorInf.h
  typedef struct _DRM_ErrorInf
  {
    unsigned long code1;	//!< Detailed code 1 (used inside Fujitsu)
    unsigned long code2;	//!< Detailed code 2 (used inside Fujitsu)
    unsigned long code3;	//!< Detailed code 3 (public detailed error code)
  } DRM_ErrorInf;		//!< Error information structure
#endif /* DRM_ERROR_INF_DEFINED */

  //! Read certificates and private key from the secure area
  int MediaDRM_TZ_SetSecretDataType(const char* license_db, int secret_data_type,
				    unsigned char* root_cert_buf, size_t root_cert_buf_len, size_t* root_cert_len,
				    unsigned char* ca_cert_buf, size_t ca_cert_buf_len, size_t* ca_cert_len,
				    unsigned char* client_cert_buf, size_t client_cert_buf_len, size_t* client_cert_len,
				    unsigned char* client_key_buf, size_t client_key_buf_len, size_t* client_key_len,
				    DRM_ErrorInf* inf);
#ifdef __cplusplus
};
#endif

///////////////////////////////////////////////////////////////////////////
// Definition of return codes
///////////////////////////////////////////////////////////////////////////
#ifndef MEDIA_DRM_RETURN_OK
#define MEDIA_DRM_RETURN_OK                (0) //!< Successful completion
#define MEDIA_DRM_RETURN_ERROR             (1) //!< An internal error occured.
#define MEDIA_DRM_RETURN_ARG_ERROR         (5) //!< An error exists in an argument.
#define MEDIA_DRM_RETURN_NOT_FOUND_LICENSE (9) //!< Specified license is not found.
#endif /* MEDIA_DRM_RETURN_OK */

#endif /* __MEDIA_DRM_TZ_H__ */
