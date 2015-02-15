//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/tz_secure/drm/marlin3/DecoderDRM_TZ_priv.h#1 $
//-----------------------------------------------------------------------------
/*! @file   DecoderDRM_TZ_priv.h
 *  @brief  Things necessary to implement in DecoderDRM_TZ.cpp
 *  @author Fujitsu) Tetsuhiro CHIBA
 *  @date   2011/10/19 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#ifndef __DECODER_DRM_TZ_PRIV_H__
#define __DECODER_DRM_TZ_PRIV_H__

#include "tomcrypt.h"
#include "DRM_TZ_priv.h"
#include "ErrorDetail_TZ.h"

//! Block size of AES
#define TZ_AES_BLOCK_SIZE     (16)
//! Key length of AES
#define TZ_AES_KEY_SIZE       (16)
//! Length of SenderID
#define TZ_SENDER_ID_LEN      (8)
//! Size of encrypted license
#define TZ_ENC_LICENSE_SIZE   (400)
//! Length of ChannelNumber
#define TZ_CHANNEL_NO_LEN     (32)
//! Length of LicenseID
#define TZ_LICENSE_ID_LEN     (16)
//! Length of ContentID
#define TZ_CONTENT_ID_LEN     (20)
//! Length of ChannelID
#define TZ_CHANNEL_ID_LEN     (16)
//! Length of Content Key
#define TZ_CONTENT_KEY_LEN    (16)
//! Length of Content Key ID
#define TZ_CONTENT_KEY_ID_LEN (16)
//! Scramble key (odd)
#define TZ_SCRAMBLE_KEY_ODD  (1)
//! Scramble key (even)
#define TZ_SCRAMBLE_KEY_EVEN (2)



//! DTCP (Digital Transmission Content Protection) Export information
typedef struct
{
  unsigned int reserv_1;
  unsigned int retention_move_mode;
  unsigned int retention_state;
  unsigned int epn;
  unsigned int dtcp_cci;

  unsigned int reserv_2;
  unsigned int image_constraint_token;
  unsigned int aps;
} ExportInfo_DTCP_TZ;


//! CPRM for DVD Export information
typedef struct
{
  unsigned int cgms;
  unsigned int apstb;
  unsigned int reserv_1;
  unsigned int epn;
  unsigned int reserv_2;
} ExportInfo_CPRM_DVD_TZ;

//! CPRM for SD Video Export information
typedef struct
{
  unsigned int initial_move_control_information;
  unsigned int current_move_control_information;
  unsigned int copy_count_control_information;
  unsigned int reserv_1;
  unsigned int apstb;
  unsigned int reserv_2;
} ExportInfo_CPRM_SD_VIDEO_TZ;


//! CPRM for SD Audio Export information
typedef struct
{
  unsigned int reserv_1;
  unsigned int cci;
  unsigned int move_control_information;
  unsigned int check_out_control_information;
} ExportInfo_CPRM_SD_AUDIO_TZ;


//! MG-R(SVR: Secure Video Recording) for Memmory Stick Pro Export information
typedef struct
{
  unsigned int cci;
  unsigned int epn;
  unsigned int apstb;
  unsigned int ict;
  unsigned int reserv_1;
} ExportInfo_MGR_SVR_TZ;


//! MG-R(SAR: Secure Audio Recording) for Memmory Stick Pro Export information
typedef struct
{
  unsigned int copyright_protection_information;
  unsigned int copy_generation_information;
  unsigned int reserv_1;
  unsigned int copy_attribution_information;
  unsigned int reserv_2;
  unsigned int number_of_check_out_information;
} ExportInfo_MGR_SAR_TZ;


//! VCPS Export information
typedef struct
{
  unsigned int aps;
  unsigned int cgms1;
  unsigned int cgms2;
  unsigned int epn1;
  unsigned int epn2;
  unsigned int reserv_1;
  unsigned int reserv_2;
  unsigned int reserv_3;
} ExportInfo_VCPS_TZ;


//! Definition of ExportInfo_TZ structure
typedef struct
{
  unsigned char data[sizeof(ExportInfo_DTCP_TZ)          + sizeof(ExportInfo_CPRM_DVD_TZ) +
		     sizeof(ExportInfo_CPRM_SD_VIDEO_TZ) + sizeof(ExportInfo_CPRM_SD_AUDIO_TZ) +
		     sizeof(ExportInfo_MGR_SVR_TZ)       + sizeof(ExportInfo_MGR_SAR_TZ) +
		     sizeof(ExportInfo_VCPS_TZ)];
} ExportInfo_TZ;


//! Definition of ExtractInfo_TZ structure
typedef struct
{
  unsigned int digital_recording_control_data;
  unsigned int copy_control_type;
  unsigned int aps_control_data;
  unsigned int image_constraint_token;
  unsigned int retention_mode;
  unsigned int retention_state;
  unsigned int encryption_mode;
  unsigned int user_defined_data;
  unsigned long not_before;
  unsigned long not_after;
} ExtractInfo_TZ;


//! Definition of RecordInfo_TZ structure
typedef struct
{
  unsigned int digital_recording_control_data;
  unsigned int copy_control_type;
  unsigned int aps_control_data;
  unsigned int image_constraint_token;
  unsigned int retention_mode;
  unsigned int retention_state;
  unsigned int encryption_mode;
  unsigned int user_defined_data;
} RecordInfo_TZ;


//! Definition of ::MarlinLicense_TZ structure
typedef struct MarlinLicense_tz
{
  char channel_no[TZ_CHANNEL_NO_LEN + 2];
  unsigned char license_id[TZ_LICENSE_ID_LEN + 1];
  char content_id[TZ_CONTENT_ID_LEN + 1];
  char channel_id[TZ_CHANNEL_ID_LEN + 1];
  unsigned char content_key[TZ_CONTENT_KEY_LEN + 1];
  unsigned char content_key_id[TZ_CONTENT_KEY_ID_LEN + 1];

  ExtractInfo_TZ extract_info;
  ExportInfo_TZ export_info;
  RecordInfo_TZ record_info;

  unsigned char ip_service_provider_id;
} MarlinLicense_TZ;

//! Definition of ::DecoderDRM_TZ structure
typedef struct crypt_decoder_tz
{
  unsigned char* license_obj;           //!< encrypted license data
  unsigned char lid[16];
  unsigned char key[16];
  unsigned char ip_service_provider_id;
  unsigned char rendering_obligation[2];
  unsigned int extra_time;              //!< ExtraTime
  unsigned int not_before;              //!< NotBefore
  unsigned int not_after;               //!< NotAfter
  unsigned short dl_check_time_counter; //!< use for @ref DecoderDRM_TZ_DecodePayload_DL() implement
} DecoderDRM_TZ;

#endif /* __DECODER_DRM_TZ_PRIV_H__ */
