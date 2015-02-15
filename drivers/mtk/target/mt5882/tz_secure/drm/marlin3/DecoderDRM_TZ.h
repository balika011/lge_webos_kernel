//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/tz_secure/drm/marlin3/DecoderDRM_TZ.h#1 $
//-----------------------------------------------------------------------------
/*! @file   DecoderDRM_TZ.h
 *  @brief  DecoderDRM API for calling function in the Trust Zone from outside the Trust Zone
 *  @author Fujitsu) Tetsuhiro CHIBA
 *  @date   2011/10/19 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#ifndef __DECODER_DRM_TZ_H__
#define __DECODER_DRM_TZ_H__

#include <stddef.h> // for size_t
#include "MediaDRM_TZ.h"

#ifdef __cplusplus
extern "C" {
#endif

  // Copy from MediaDRM.h
#if 0
  typedef struct _DRM_IPMCInfo
  {
    unsigned char tier_bit[8];             //!< Subscription tier bits
    unsigned long update_offset;           //!< Update start date offset to NotAfter
    unsigned long not_before;              //!< NotBefore
    unsigned long not_after;               //!< NotAfter
    unsigned char ip_service_provider_id;  //!< IP service provider identifier
    unsigned char version;                 //!< Version
    unsigned char reserve[2];              //!< Unused
  } DRM_IPMCInfo;
#endif

#ifndef DRM_EXTRACT_INFO_DEFINED
#define DRM_EXTRACT_INFO_DEFINED
  typedef struct _DRM_Extract_Info
  {
    unsigned long not_before;              //!< NotBefore
    unsigned long not_after;               //!< NotAfter
    unsigned char rendering_obligation[2]; //!< Rendering obligation
    unsigned char ip_service_provider_id;  //!< IP service provider identifier
    unsigned char reserve[1];              //!< Unused
  } DRM_ExtractInfo;
#endif /* DRM_EXTRACT_INFO_DEFINED */

#ifndef DRM_EXPORT_INFO_DEFINED
#define DRM_EXPORT_INFO_DEFINED
  typedef struct _DRM_Export_Info
  {
    unsigned char export_type;             //!< Type of the target of exportation.
    unsigned char reserve[1];              //!< Unused
    unsigned char ip_service_provider_id;  //!< IP service provider identifier
    unsigned char export_info_len;         //!< Byte size of the "ExportInfo"
    unsigned char export_info[24];         //!< The value of the "ExportInfo" that depends on the value of the export_type
  } DRM_ExportInfo;
#endif /* DRM_EXPORT_INFO_DEFINED */

  typedef struct _DecoderDRM_Offset
  {
    size_t offset;                         //!< offset in the packet
    size_t size;                           //!< size of the payload to decrypt. If 0, this function doesn't decrypt that packet.
  } DecoderDRM_Offset;

  //! Opens an environment, on the Secure Service, of decrypting an encrypted content for VOD and the IP multicast license.
  int DecoderDRM_TZ_Open(int* dec_slot, char* license_db, int secret_data_type, char* license_obj, size_t license_obj_len,
			 char* lid, size_t lid_len, /*DRM_IPMCInfo ipmc_info[2],*/ DRM_ExtractInfo* extract_info, DRM_ErrorInf* inf);

  //! Closes an environment of decrypting an encrypted content on the Secure Service.
  int DecoderDRM_TZ_Close(int dec_slot, DRM_ErrorInf* inf);

  //! Decrypts, on the Secure Service, a VOD content with the content key contained in the matching license.
  int DecoderDRM_TZ_DecodePackets_VOD(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets, 
				      DecoderDRM_Offset* offsets, void* out_buff, size_t out_buff_len,
				      size_t* out_num_of_packets, DRM_ErrorInf* inf);

  //! Decodes, on the Secure Service, an ECM of IP multicast with the content key (the work key).
  int DecoderDRM_TZ_DecodeECM(int dec_slot, void* in_buff, size_t in_buff_len,
			      void* out_buff, size_t out_buff_len, size_t* out_len, DRM_ErrorInf* inf);

  //! Decrypts, on the Secure Service, a content data of IP multicast.
  int DecoderDRM_TZ_DecodePackets_IPMC(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets,
				       DecoderDRM_Offset* offsets, void* out_buff, size_t  out_buff_len,
				       size_t* out_num_of_packets, unsigned long mode, DRM_ErrorInf* inf);

  //! Opens an environment, on the Secure Service, of decrypting an encrypted content for the download license.
  int DecoderDRM_TZ_Open_DL(int* dec_slot, char* lid, size_t lid_len, unsigned int extra_time, int* license_type,
			    DRM_ExtractInfo* extract_info, DRM_ExportInfo* export_info, DRM_ErrorInf* inf);

  //! Closes the decoding environment for the download license opened on the Secure Service.
  int DecoderDRM_TZ_Close_DL(int dec_slot, DRM_ErrorInf* inf);

  //! This function decrypts the encrypted content data with the content key of the download license.
  int DecoderDRM_TZ_DecodePayload_DL(int dec_slot, size_t packet_size, void* in_buff, size_t in_num_of_packets,
				     DecoderDRM_Offset* offsets, void* out_buff, size_t out_buff_len, 
				     size_t* out_num_of_packets, DRM_ErrorInf* inf);

#ifdef __cplusplus
};
#endif

///////////////////////////////////////////////////////////////////////////
// Definition of return codes
///////////////////////////////////////////////////////////////////////////
#ifndef DECODER_DRM_RETURN_OK
#define DECODER_DRM_RETURN_OK        (0)  //!< Normal termination.
#define DECODER_DRM_ARG_ERROR        (-1) //!< Argument invalid.
#define DECODER_DRM_SEQUENCE_ERROR   (-2) //!< DecoderDRM object has not been initialized.
#define DECODER_DRM_NO_LICENSE_ERROR (-3) //!< Specified license is not found.
#define DECODER_DRM_INTERNAL_ERROR   (-4) //!< Internal error.
#define DECODER_DRM_LICENSE_EXPIRE   (-6) //!< It is not possible to decrypt the data because date-time of Notafter has passed.
#define DECODER_DRM_NOT_AVAILABLE    (-7) //!< It is not possible to decrypt the data because there is not any true bit corresponding to necessary subscription.
#endif /* DECODER_DRM_RETURN_OK */

#endif /* __DECODER_DRM_TZ_H__ */
