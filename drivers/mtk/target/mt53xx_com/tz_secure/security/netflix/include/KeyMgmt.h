/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __KEYMGMT_H__
#define __KEYMGMT_H__

#include "CryptoTypes.h"

nfTeeCryptoResult_t caGetEsn( nfTeeClientId_t 	ClientId,
							  uint32_t			EsnMaxLength,
						   	  uint8_t 			*EsnPtr,
							  uint32_t 			*EsnLengthPtr );

nfTeeCryptoResult_t keyMgmtInit();

nfTeeCryptoResult_t caGetNamedKeyHandle( nfTeeClientId_t		ClientId,
						 	 	 	 	 uint8_t 				KeyName[],
						 	 	 	 	 uint32_t 				KeyNameLength,
						 	 	 	 	 nfCryptoKeyHandle_t 	*KeyHandlePtr );

nfTeeCryptoResult_t caImportKey( nfTeeClientId_t 		ClientId,
							   	 nfKeyFormat_t 			KeyFormat,
							   	 uint8_t 				KeyData[],
							     uint32_t 				DataLength,
							     nfCryptoAlgorithm_t	Algorithm,
							     nfKeyUsageFlag_t 		KeyUsageFlags,
							     nfCryptoKeyHandle_t	*KeyHandlePtr,
							     nfKeyType_t			*KeyTypePtr );



nfTeeCryptoResult_t caExportKey( nfTeeClientId_t 	ClientId,
							   nfCryptoKeyHandle_t 	KeyHandle,
							   nfKeyFormat_t 		KeyFormat,
							   uint8_t* 			KeyDataPtr,
							   uint32_t				KeyDataLength,
							   uint32_t* 			DataLengthPtr );



nfTeeCryptoResult_t caGetKeyInfoFromHandle( nfTeeClientId_t 		ClientId,
											nfCryptoKeyHandle_t		KeyHandle,
											nfKeyType_t 			*KeyTypePtr,
											nfCryptoAlgorithm_t 	*AlgorithmPtr,
											nfKeyUsageFlagsWord_t 	*KeyUsageFlagsPtr );

nfTeeCryptoResult_t caGetAesKeyFromHandle( nfTeeClientId_t 		ClientId,
										   nfCryptoKeyHandle_t	KeyHandle,
										   uint8_t				**KeyPtrPtr,
										   uint32_t				*KeyLengthPtr );

nfTeeCryptoResult_t caGetHmacKeyFromHandle( nfTeeClientId_t 	ClientId,
							 			    nfCryptoKeyHandle_t	KeyHandle,
										    uint8_t				**KeyPtrPtr,
										    uint32_t			*KeyLengthPtr );

nfTeeCryptoResult_t caExportSealedKey( nfTeeClientId_t 		ClientId,
                       		    	   nfCryptoKeyHandle_t	KeyHandle,
                                       uint8_t				*SealedKeyPtr,
                                       uint32_t				*SealedKeyLengthPtr,
                                       uint32_t				SealedKeyMaxLength );

nfTeeCryptoResult_t caImportSealedKey( nfTeeClientId_t 			ClientId,
                       		    	   nfKeyType_t				*KeyTypePtr,
                       		    	   nfCryptoAlgorithm_t		*AlgorithmPtr,
                       		    	   nfKeyUsageFlagsWord_t	*KeyUsageFlagsPtr,
                       		    	   uint32_t					*KeyBitCountPtr,
                       		    	   nfCryptoKeyHandle_t		*KeyHandlePtr,
                       		    	   uint32_t					KeyDataLength,
                       		    	   uint8_t					KeyData[] );

nfTeeCryptoResult_t caDhGenKeys( nfTeeClientId_t 		ClientId,
							     uint32_t 				Generator,
							    // uint8_t*				RandomKey,
							     //uint32_t				RandomKeyLen,
							     uint8_t 				*ModulusPtr,
							     uint32_t 				ModulusLength,
							     nfCryptoKeyHandle_t 	*PrivKeyHandlePtr );

nfTeeCryptoResult_t caDhDeriveKeys( nfTeeClientId_t 	ClientId,
									nfCryptoKeyHandle_t DHPrivKeyHandle,
									nfCryptoKeyHandle_t	KeyDerivationKeyHandle,
									uint32_t			PeerPubLength,
									uint8_t				*DHPubValuePtr,
									nfCryptoKeyHandle_t *KceHandlePtr,
									nfCryptoKeyHandle_t	*KchHandlePtr,
									nfCryptoKeyHandle_t	*KcdHandlePtr );

nfTeeCryptoResult_t caClearKeys( nfTeeClientId_t 	ClientId,
	     	 	 	 	 	 	 uint32_t 			MaxHandleListSize,
	     	 	 	 	 	 	 uint32_t			*NumHandlesPtr,
	     	 	 	 	 	 	 uint32_t			DeletedHandlesList[] );

nfTeeCryptoResult_t caDeleteKey( nfTeeClientId_t 		ClientId,
								 nfCryptoKeyHandle_t	KeyHandle );

// definitions for incremental encrypt/decrypt/hmac
typedef enum
{
	caICCS_UNINIT,
	caICCS_ENCRYPT_UPDATING,

	caICCS_DECRYPT_UPDATING,

	caICCS_HMAC_UPDATING,

	// leave this last!
	caICCS_INVALID
} caIncrementalCryptoContextState_t;



typedef struct
{
	uint32_t							KeyHandle;
	caIncrementalCryptoContextState_t	State;
	uint32_t							Validator;
	void								*CryptoContextPtr;
	void								*cryptoItemPtr;
} caIncrementalCryptoContext_t;

nfTeeCryptoResult_t caAes128CbcEncryptInit( nfCryptoKeyHandle_t			KeyHandle,
											uint8_t						*KeyPtr,
									 	 	uint8_t						IV[],
									 	 	nfTeeCryptoContextHandle_t	*CryptoContextHandlePtr );

nfTeeCryptoResult_t caAes128CbcEncryptUpdate( caIncrementalCryptoContext_t 	*ContextPtr,
											  uint8_t						*DataPtr,
											  uint32_t						DataLength,
											  uint8_t						*EncryptedDataPtr,
											  uint32_t						EncryptedDataMaxLength,
											  uint32_t						*EncryptedDataLengthPtr );

nfTeeCryptoResult_t caAes128CbcEncryptFinal( caIncrementalCryptoContext_t 	*ContextPtr,
											 uint8_t						*EncryptedDataPtr,
											 uint32_t						EncryptedDataMaxLength,
											 uint32_t						*EncryptedDataLengthPtr );

nfTeeCryptoResult_t caAes128CbcDecryptInit( nfCryptoKeyHandle_t			KeyHandle,
											uint8_t						*KeyPtr,
									 	 	uint8_t						IV[],
									 	 	nfTeeCryptoContextHandle_t	*CryptoContextHandlePtr );

nfTeeCryptoResult_t caAes128CbcDecryptUpdate( caIncrementalCryptoContext_t 	*ContextPtr,
											  uint8_t						*EncryptedDataPtr,
											  uint32_t						EncryptedDataLength,
											  uint8_t						*DataPtr,
											  uint32_t						DataMaxLength,
											  uint32_t						*DataLengthPtr );

nfTeeCryptoResult_t caAes128CbcDecryptFinal( caIncrementalCryptoContext_t 	*ContextPtr,
											 uint8_t						*DataPtr,
											 uint32_t						DataMaxLength,
											 uint32_t						*DataLengthPtr );

nfTeeCryptoResult_t caHmacSha256Init( nfCryptoKeyHandle_t			KeyHandle,
									  uint8_t						*KeyPtr,
									  nfTeeCryptoContextHandle_t	*CryptoContextHandlePtr );

nfTeeCryptoResult_t caHmacSha256Update( caIncrementalCryptoContext_t	*ContextPtr,
										uint8_t							*DataPtr,
										uint32_t						DataLength );

nfTeeCryptoResult_t caHmacSha256Final( caIncrementalCryptoContext_t *ContextPtr,
									   uint8_t						*HmacDataPtr,
									   uint32_t						HmacMaxLength,
									   uint32_t						*HmacLengthPtr );


#endif // __KEYMGMT_H__
