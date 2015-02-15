//=============================================================================
// $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/marlin3/ErrorDetail_TZ.h#1 $
//-----------------------------------------------------------------------------
/*! @file   ErrorDetail_TZ.h
 *  @brief  Detailed error codes
 *  @author Fujitsu) Yuuki Matsueda
 *  @date   2011/10/24 Created
 *  $Revision: #1 $
 */
//-----------------------------------------------------------------------------
//  COPYRIGHT FUJITSU LIMITED 2011, All rights reserved.
//=============================================================================
#ifndef __ERROR_DETAIL_TZ_H__
#define __ERROR_DETAIL_TZ_H__

#include "MediaDRM_TZ.h" /* for DRM_ErrorInf */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef REQUIRE_ERROR_DETAIL
  void TZ_SetErrorDetail(int error_id);
  void TZ_InitErrorDetail(void);
  void TZ_GetErrorInf(DRM_ErrorInf* inf);
#endif /* REQUIRE_ERROR_DETAIL */

#ifdef __cplusplus
};
#endif

//////////////////////////////////////////////////////////////////////
// Definition of detailed error code 1 and 2 (12 bits, up to 2047)
//////////////////////////////////////////////////////////////////////
#define MARLIN_ERROR_DETAIL_TZ_054_00005 (1840) // [730] Argument error (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00249 (1841) // [731] Cannot allocate memory (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00250 (1842) // [732] Cannot allocate memory (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00251 (1843) // [733] Cannot allocate memory (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00400 (1844) // [734] Error in reading from the secure area (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00401 (1845) // [735] Data not found in the secure area (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00402 (1846) // [736] The root public key certificate is too long (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00403 (1847) // [737] The client CA public key certificate is too long (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00404 (1848) // [738] The client public key certificate is too long (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00405 (1849) // [739] Error in caching certificates and private key into the Trust Zone (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00406 (1850) // [73a] The value of secret_data_type is invalid (MediaDRM_TZ_SetSecretDataType)
#define MARLIN_ERROR_DETAIL_TZ_054_00407 (1851) // [73b] The value of secret_data_type is invalid (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00408 (1852) // [73c] The value of secret_data_type is invalid (writing to the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_054_00409 (1853) // [73d] Argument error (writing to the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_054_00025 (1854) // [73e] Cannot allocate memory (writing to the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_054_00026 (1855) // [73f] Cannot allocate memory (writing to the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_054_00027 (1856) // [740] Cannot allocate memory (writing to the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_054_00028 (1857) // [741] Cannot allocate memory (writing to the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_054_00410 (1858) // [742] Failed to read a root public key certificate (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00411 (1859) // [743] The buffer for the root public key certificate is of short length (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00412 (1860) // [744] Failed to read a client CA public key certificate (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00413 (1861) // [745] The buffer for the client CA public key certificate is of short length (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00414 (1862) // [746] Failed to read a client public key certificate (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00415 (1863) // [747] The buffer for the client public key certificate is of short length (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00416 (1864) // [748] Failed to read a client private key (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00417 (1865) // [749] The buffer for the client private key is of short length (reading from the secure area)
#define MARLIN_ERROR_DETAIL_TZ_054_00418 (1866) // [74a] The value of secret_data_type is invalid (writing to the Trust Zone)
//#define MARLIN_ERROR_DETAIL_TZ_054_00419 (1867) // [74b] Argument error (MediaDRM_GetLicenseObject)
#define MARLIN_ERROR_DETAIL_TZ_056_00420 (1868) // [74c] Argument error (DecoderDRM_TZ_Close)
#define MARLIN_ERROR_DETAIL_TZ_056_00421 (1869) // [74d] Argument error (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00422 (1870) // [74e] Argument error (DecoderDRM_TZ_DecodePackets_VOD)
#define MARLIN_ERROR_DETAIL_TZ_056_00423 (1871) // [74f] Argument error (DecoderDRM_TZ_DecodeECM)
#define MARLIN_ERROR_DETAIL_TZ_056_00424 (1872) // [750] Argument error (DecoderDRM_TZ_DecodePackets_IPMC)
#define MARLIN_ERROR_DETAIL_TZ_056_00425 (1873) // [751] Argument error (DecoderDRM_TZ_Open_DL)
#define MARLIN_ERROR_DETAIL_TZ_056_00426 (1874) // [752] Argument error (DecoderDRM_TZ_Close_DL)
#define MARLIN_ERROR_DETAIL_TZ_056_00427 (1875) // [753] Argument error (DecoderDRM_TZ_DecodePayload_DL)
#define MARLIN_ERROR_DETAIL_TZ_056_00428 (1876) // [754] The license data failed to be encrypted (MediaDRM_GetLicenseObject)
//#define MARLIN_ERROR_DETAIL_TZ_054_00429 (1877) // [755] The pointer int the cach area failed in acquisition (MediaDRM_GetLicenseObject)
//#define MARLIN_ERROR_DETAIL_TZ_054_00430 (1878) // [756] The license data was not found (MediaDRM_GetLicenseObject)
#define MARLIN_ERROR_DETAIL_TZ_056_00431 (1879) // [757] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00432 (1880) // [758] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00433 (1881) // [759] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00434 (1882) // [75a] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00435 (1883) // [75b] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00436 (1884) // [75c] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00437 (1885) // [75d] The encryption processing failed (TZ_cbc_scte52)
#define MARLIN_ERROR_DETAIL_TZ_056_00438 (1886) // [75e] The encryption processing failed (TZ_crypt_buffer)
#define MARLIN_ERROR_DETAIL_TZ_056_00439 (1887) // [75f] The encryption processing failed (TZ_crypt_buffer)
#define MARLIN_ERROR_DETAIL_TZ_056_00440 (1888) // [760] The encryption processing failed (TZ_crypt_buffer)
#define MARLIN_ERROR_DETAIL_TZ_056_00441 (1889) // [761] The encryption processing failed (TZ_crypt_buffer)
#define MARLIN_ERROR_DETAIL_TZ_056_00442 (1890) // [762] The structure of certificate cannot be recognized (TZ_cert_get_subject)
#define MARLIN_ERROR_DETAIL_TZ_056_00443 (1891) // [763] The structure of certificate cannot be recognized (TZ_cert_get_subject)
#define MARLIN_ERROR_DETAIL_TZ_056_00444 (1892) // [764] Error in reading cached data from the Trust Zone (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00445 (1893) // [765] Taking out subject from the certificate failed (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00446 (1894) // [766] Decoding the license decryption key failed (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00447 (1895) // [767] The memory allocation failed (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00448 (1896) // [768] The memory allocation failed (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00449 (1897) // [769] The management table of the decoding environment became overcapacity (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00450 (1898) // [76a] The conversion of sender id failed (DecoderDRM_TZ_Open)
#define MARLIN_ERROR_DETAIL_TZ_056_00451 (1899) // [76b] Argument error (DecoderDRM_TZ_DecodePackets_VOD)
#define MARLIN_ERROR_DETAIL_TZ_056_00452 (1900) // [76c] Decoding the packets failed (DecoderDRM_TZ_DecodePackets_VOD)
#define MARLIN_ERROR_DETAIL_TZ_056_00453 (1901) // [76d] The memory allocation failed (DecoderDRM_TZ_DecodePackets_VOD)
#define MARLIN_ERROR_DETAIL_TZ_056_00454 (1902) // [76e] Argument error (TZ_crypt_buffer)
#define MARLIN_ERROR_DETAIL_TZ_056_00200 (1903) // [76f] Argument error (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00201 (1904) // [770] The value of secret_data_type is invalid (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00202 (1905) // [771] The value of secret_data_type is invalid (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00203 (1906) // [772] Data is not cached (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00204 (1907) // [773] The buffer for the root public key certificate is of short length (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00205 (1908) // [774] Cannot allocate memory (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00206 (1909) // [775] The buffer for the client CA public key certificate is of short length (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00207 (1910) // [776] Cannot allocate memory (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00208 (1911) // [777] The buffer for the client public key certificate is of short length (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00209 (1912) // [778] Cannot allocate memory (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00210 (1913) // [779] The buffer for the client private key is of short length (reading from the Trust Zone)
#define MARLIN_ERROR_DETAIL_TZ_056_00211 (1914) // [77a] Cannot allocate memory (reading from the Trust Zone)

#endif /* __ERROR_DETAIL_TZ_H__ */
