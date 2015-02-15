/*
 * StorageAgentTPI.h
 *
 *  Created on: Jan 31, 2014
 *      Author: scott
 */

#ifndef STORAGEAGENTTPI_H_
#define STORAGEAGENTTPI_H_

/*
 * This file contains definitions that storageAgent exports across
 * the TPI. If you implement TPI-specific code for your platform,
 * you'll need to include this file. The TPI functions advance the
 * storageAgent state, which must be nfSAS_RUN before client
 * services are available.
 *
 * storageAgent requires the following TPI functions:
 *
 * 	- saTpiInit( uint8_t *InitDataPtr, uint32_t InitDataLength,
 * 					nfStorageAgentState_t	*StatePtr )
 *
 * 	- saTpiLoadIdFile( uint8_t *DataPtr, uint32_t DataLength )
 *
 * 	- saTpiLoadDrmCredentials( uint8_t *DataPtr, uint32_t DataLength )
 *
 * 	- saTpiProtectData( nfTeeClientId_t	ClientId, uint8_t *DataPtr, uint32_t DataLength,
 * 						uint8_t *ProtectedDataPtr, uint32_t MaxProtectedDataLength,
 * 						uint32_t *ProtectedDataLengthPtr )
 *
 * 	- saTpiUnprotectData( nfTeeClientId_t ClientId,
 * 						  uint8_t *ProtectedDataPtr, uint32_t ProtectedDataLength,
 * 						uint8_t *DataPtr, uint32_t MaxDataLength,
 * 						uint32_t *UnprotectedDataLengthPtr )
 *
 * These functions are more fully defined at the end of this file. You should add
 * your implementations to the TeePortInterface subdirectory.
 *
 */

/*
 * In the first release, the storage agent provides services
 * to TEE internal applications. In particular, it loads the
 * manufacturing secure store, and makes the associated data
 * available to the cryptoAgent and drmAgent.
 *
 * Since memory is at a premium in the TEE, rather than making copies
 * of data elements, we load the manufacturing secure store into memory,
 * verify and decrypt in-place, and then maintain pointers to the
 * data in the original buffer. See StorageAgentInternals.h for documentation
 * of the manufacturing secure store structure.
 */
typedef struct
{
	nfSecureStoreClientKey_t	ClientKey;
	nfTeeClientId_t				*ClientIdPtr;
	uint32_t					DataLength;
	uint8_t						*DataPtr;
} nfSecureStoreTableRow_t;

/*
 * storageAgent uses 2 internal keys (AES and HMAC). Sizes are defined here.
 */

#define saAES_128_KEY_SIZE_BYTES 		16
#define saHMAC_SHA256_KEY_SIZE_BYTES	32

/*
 * Today, we have 10 items in the secure store, so we need one table row
 * for each one:
 *
 * 	- ESN
 * 	- Kpe
 * 	- Kph
 * 	- Kpw (optional factory provisioning, we derive from Kpe/Kph)
 * 	- PlayReady device cert template
 * 	- PlayReady model group cert
 * 	- PlayReady model group priv key
 * 	- storageAgent (secure store) AES key
 * 	- storageAgent (secure store) HMAC key
 * 	- NRD app ID
 *
 * Following are the definitions for the manufacturing secure store table
 */
extern nfSecureStoreTableRow_t saManufSSTable[];

uint8_t saInternalKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalAesKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalHMACKey[saHMAC_SHA256_KEY_SIZE_BYTES];


// to make life easier, we'll create index identifiers for each element in the table
typedef enum
{
	nfSSRI_ESN = 0,
	nfSSRI_KPE,
	nfSSRI_KPH,
	nfSSRI_KPW,
	nfSSRI_DRM_CERTTEMPLATE,
	nfSSRI_DRM_GROUP_CERT,
	nfSSRI_DRM_GROUP_PRIV,
	nfSSRI_SA_ENCRYPTION_KEY,
	nfSSRI_SA_HMAC_KEY,
	nfSSRI_NRD_APP_ID,

	// leave this last
	nfSSRI_INVALID
}nfSecureStoreRowIndex_t;

#define saMANUF_SS_TABLE_SIZE	nfSSRI_INVALID
extern nfSecureStoreTableRow_t saManufSSTable[]; // one true declaration is in StorageAgent.c

/*
 * While the simple-minded approach is to load everything at once (e.g. from
 * an encrypted flash sector), we may need to load things incrementally. For
 * example, some implementations push the idfile and drm credentials from
 * the REE in separate steps. In others, we generate netflix credentials from
 * the ESN, and do DRM elsewhere. We need to support multiple use cases.
 *
 * Below is the storage agent state definitions. In order to provide services,
 * the storage agent must be fully initialized. At minimum, it needs to load
 * secure store keys.
 *
 * When storageAgent receives the init message, it may contain an opaque
 * initialization argument. This will be passed to saTpiInit() for interpretation.
 * In the existing refapp, this is expected to be NULL, and we'll read manuf_ss.bin
 * and load all credentials.
 *
 * In the case of the PS4, this will contain the ESN. From this, saTpiInit will
 * derive the application keys and populate saManufSSTable[], and then advance the
 * storageAgent state to nfSAS_RUN. For refapp, we load secure store keys, load
 * manuf_ss.bin, and advance the state to nfSAS_RUN.
 *
 * If your implementation loads the idfile and credentials separately, you'll need
 * to add code to initialize those before advancing to nfSAS_RUN.
 *
 */
typedef enum
{
	nfSAS_UNINIT = 0,
	nfSAS_SS_KEYS_LOADED,
	nfSAS_NFLX_IDFILE_LOADED,
	nfSAS_DRM_LOADED,

	nfSAS_RUN,  // good to go, ready to offer services

	// leave this last
	nfSAS_INVALID
} nfStorageAgentState_t;

/*
 * Storage Agent TPI functions defined below.
 */

/**************************************************
 * @function saTpiInit
 *
 * @brief   storage agent TPI initialization
 *
 * @param[in] InitDataPtr - pointer to initialization data
 * @param[in] InitDataLength - length of initialization data
 * @param[out] StatePtr - pointer to where you want storage agent state
 *
 * @return nfTGR_SUCCESS if initialized, error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiInit( uint8_t 				*InitDataPtr,
								uint32_t 				InitDataLength,
								nfStorageAgentState_t	*StatePtr );

/**************************************************
 * @function saTpiLoadNflxIdFile
 *
 * @brief storage agent TPI function to load Netflix idfile
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to idfile data
 * @param[in] DataLength - length of data
 *
 * @return nfTGR_SUCCESS if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxIdFile( nfTeeClientId_t 	ClientId,
								   uint8_t 			*DataPtr,
								   uint32_t 		DataLength );

/**************************************************
 * @function saTpiLoadDrmCredentials
 *
 * @brief   storage agent TPI function to load DRM credentials
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to credential data
 * @param[in] DataLength - length of data
 *
 * @return nfTGR_SUCCESS if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxDrmCredentials( nfTeeClientId_t 	ClientId,
										   uint8_t 			*DataPtr,
										   uint32_t 		DataLength );

/**************************************************
 * @function saTpiProtectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] DataPtr - pointer to data that you want protected
 * @param[in] DataLength - length of unprotected data
 * @param[out] ProtectedDataPtr - pointer to preallocated buffer for protected data
 * @param[in] MaxProtectedDataLength - size of buffer at ProtectedDataPtr
 * @param[out] ProtectedDataLengthPtr - actual output length will be stored here.
 *
 * @return nfTGR_SUCCESS if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiProtectData( nfTeeClientId_t	ClientId,
					   	  	    	   uint8_t 		*DataPtr,
					   	  	    	   uint32_t 		DataLength,
					   	  	    	   uint8_t 		*ProtectedDataPtr,
					   	  	    	   uint32_t 		MaxProtectedDataLength,
					   	  	    	   uint32_t 		*ProtectedDataLengthPtr );


/**************************************************
 * @function saTpiUnrotectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] ProtectedDataPtr - pointer to protected data
 * @param[in] ProtectedDataLength - length of protected data
 * @param[out] DataPtr - pointer to preallocated buffer for unprotected data
 * @param[in] MaxDataLength - size of buffer at DataPtr
 * @param[out] UnprotectedDataLengthPtr - actual output length will be stored here.
 *
 * @return nfTGR_SUCCESS if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiUnprotectData( nfTeeClientId_t	ClientId,
					    		  	  	  uint8_t 			*ProtectedDataPtr,
										  uint32_t 			ProtectedDataLength,
										  uint8_t 			*DataPtr,
										  uint32_t 			MaxDataLength,
										  uint32_t 			*UnprotectedDataLengthPtr );

/*********************************************************************
 * @function saTpiGetTAKeyLadderBase
 *
 * @brief creates device-unique key ladder base for a given (trusted app) client ID
 *         (NB: NETFLIX INTERNAL USE ONLY, not exported)
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] Buf - buffer for key ladder
 * @param[in] BufSize - size of buffer
 * @param[out] ActualSizePtr - pointer to where you output size
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiGetTAKeyLadderBase( nfTeeClientId_t 	ClientId,
									  	   	  uint8_t 			Buf[],
										  	  uint32_t 		BufSize,
										  	  uint32_t			*ActualSizePtr );

#endif /* STORAGEAGENTTPI_H_ */
