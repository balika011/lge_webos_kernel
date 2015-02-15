/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DRMTYPES_H_
#define DRMTYPES_H_

#include "CommTypes.h"
//typedef unsigned long uint64_t;

/*
 * Tee drm messages
 */

typedef enum
{
	nfTO_DRM_OP_PR_ALLOCATE_REGISTER = nfTOB_DRM_AGENT + 1,
	nfTO_DRM_OP_PR_FREE_REGISTER,
	nfTO_DRM_OP_PR_REGISTER_COUNT,
	nfTO_DRM_OP_PR_GET_PRELOADED_INDEX,
	nfTO_DRM_OP_PR_INITIALIZE,
	nfTO_DRM_OP_PR_DEINITIALIZE,
	nfTO_DRM_OP_PR_VERIFY_MSG_SIGNATURE,
	nfTO_DRM_OP_PR_SIGN_WITH_PRIVATE_KEY,
	nfTO_DRM_OP_PR_VERIFY_OMAC1_SIGNATURE,
	nfTO_DRM_OP_PR_CREATE_OMAC1_SIGNATURE,
	nfTO_DRM_OP_PR_ENCRYPT_DATA_BLOCK,
	nfTO_DRM_OP_PR_UNWRAP_KEY,
	nfTO_DRM_OP_PR_WRAP_KEY,
	nfTO_DRM_OP_PR_GENERATE_KEY,
	nfTO_DRM_OP_PR_DECRYPT_CONTENT,
	nfTO_DRM_OP_PR_DECRYPT_CONTENT_OPAQUE,
	nfTO_DRM_OP_PR_CREATE_SAMPLE_ENCRYPTION_CONTEXT,
	nfTO_DRM_OP_PR_ENCRYPT_SAMPLE_DATA,
	nfTO_DRM_OP_PR_QUERY_SAMPLE_METADATA,
	nfTO_DRM_OP_PR_DESTROY_SAMPLE_ENCRYPTION_CONTEXT,
	nfTO_DRM_OP_PR_LOAD_PLAYREADY_REVOCATION_INFO,
	nfTO_DRM_OP_PR_LOAD_PLAYREADY_CRL,


	// insert new message types here...

    nfTO_DRM_OP_INVALID // leave this last!!
} nfTeeOperation_DrmOp_t;

#define nfTEE_IS_DRM_OP(x) (((x) > nfTOB_DRM_AGENT) && ((x) < nfTO_DRM_OP_INVALID))

/**************************************************************************
 *########################################################################
 * Currently, we are only supporting PlayReady in the TEE. Following are
 * the message types needed to implement the functions in oemhal.h
 *########################################################################
 **************************************************************************/



// nfTO_DRM_OP_PR_ALLOCATE_REGISTER req/rsp defs
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_AllocateRegister(
 *   __in              OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __out_ecount( 1 ) OEM_HAL_KEY_REGISTER_INDEX *f_pKeyRegisterIndex );
 */
typedef struct
{
	int		KeyType;				// OEM_HAL_KEY_TYPE f_eKeyType
} nfTeeDrmPRAllocateRegisterReq_t;

#define nfMIN_PR_ALLOC_REGISTER_REQ_SIZE sizeof(nfTeeDrmPRAllocateRegisterReq_t)

typedef struct
{
	long	KeyRegisterIndex;	// OEM_HAL_KEY_REGISTER_INDEX *f_pKeyRegisterIndex
} nfTeeDrmPRAllocateRegisterRsp_t;

#define nfMIN_PR_ALLOC_REGISTER_RSP_SIZE sizeof(nfTeeDrmPRAllocateRegisterRsp_t)


// nfTO_DRM_OP_PR_FREE_REGISTER req/rsp defs
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_FreeRegister(
 *   __in OEM_HAL_KEY_TYPE           f_eKeyType,
 *   __in OEM_HAL_KEY_REGISTER_INDEX f_keyRegisterIndex );
 *
 */
typedef struct
{
	int		KeyType;			// OEM_HAL_KEY_TYPE f_eKeyType
	long	KeyRegisterIndex;	// OEM_HAL_KEY_REGISTER_INDEX f_KeyRegisterIndex
} nfTeeDrmPRFreeRegisterReq_t;

#define nfMIN_PR_FREE_REGISTER_REQ_SIZE sizeof(nfTeeDrmPRFreeRegisterReq_t)

#if 0 // response is empty
typedef struct
{
	<empty>
} nfTeeDrmPRFreeRegisterRsp_t;
#endif

// nfTO_DRM_OP_PR_REGISTER_COUNT req/rsp defs
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_RegisterCount(
 *   __in            OEM_HAL_KEY_TYPE     f_eKeyType,
 *   __out_ecount(1) DRM_DWORD           *f_pdwTotalRegisters,
 *   __out_ecount(1) DRM_DWORD           *f_pdwAllocatedRegisters );
 */
typedef struct
{
	int		KeyType;				// OEM_HAL_KEY_TYPE f_eKeyType
} nfTeeDrmPRRegisterCountReq_t;

#define nfMIN_PR_REGISTER_COUNT_REQ_SIZE sizeof(nfTeeDrmPRRegisterCountReq_t)

typedef struct
{
	uint32_t	TotalRegisters;		// DRM_DWORD *f_pdwTotalRegisters
	uint32_t	AllocatedRegisters;	// DRM_DWORD *f_pdwAllocatedRegisters
} nfTeeDrmPRRegisterCountRsp_t;

#define nfMIN_PR_REGISTER_COUNT_RSP_SIZE sizeof(nfTeeDrmPRRegisterCountRsp_t)

// 	nfTO_DRM_OP_PR_GET_PRELOADED_INDEX,
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_GetPreloadedIndex(
 *   __in                           DRM_VOID                   *f_pOEMContext,
 *   __in                           OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in_bcount( f_cbKeyId ) const DRM_BYTE                   *f_pbKeyId,
 *   __in                           DRM_DWORD                   f_cbKeyId,
 *   __out_ecount( 1 )              OEM_HAL_KEY_REGISTER_INDEX *f_pIndexKey );
 */
/*==============================================
 * ============================================
 * NB: This is the first message using pointers.
 * ============================================
 * =============================================
 */
typedef struct
{
	void		*OEMContextPtr;	// *f_pOEMContext
	int			KeyType;	 	// f_eKeyType
	void		*KeyIdPtr;	 	// *f_pbKeyId
#define nfDRM_PR_MAX_KEY_ID_LENGTH	32 // XXX is this defined by PR?
	uint32_t	KeyIdLength; 	// f_cbKeyId
} nfTeeDrmPRGetPreloadedIndexReq_t;

#define nfMIN_PR_GET_PRELOADED_INDEX_REQ_SIZE sizeof(nfTeeDrmPRGetPreloadedIndexReq_t)

typedef struct
{
	long	IndexKey;
} nfTeeDrmPRGetPreloadedIndexRsp_t;

#define nfMIN_PR_GET_PRELOADED_INDEX_RSP_SIZE sizeof(nfTeeDrmPRGetPreloadedIndexRsp_t)

// 	nfTO_DRM_OP_PR_INITIALIZE
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_Initialize();
 */
// (empty request/response, opcode/responsecode only)

//	nfTO_DRM_OP_PR_DEINITIALIZE
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_Deinitialize();
 */
// (empty request/response, opcode/responsecode only)


// nfTO_DRM_OP_PR_VERIFY_MSG_SIGNATURE
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_VerifyMessageSignature(
 *   __in_bcount( f_cbMessage )   const DRM_BYTE                  *f_pbMessage,
 *   __in                               DRM_DWORD                  f_cbMessage,
 *   __in_bcount( f_cbSignature ) const DRM_BYTE                  *f_pbSignature,
 *   __in                               DRM_DWORD                  f_cbSignature,
 *   __in                               OEM_HAL_HASH_TYPE          f_eHashType,
 *   __in                               OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
 *   __in                               OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey );
 */
typedef struct
{
	uint8_t			*MsgPtr;
	uint32_t		MsgLength; 			// NB: DRM_DWORD is unsigned long
	uint8_t			*SignaturePtr;
	uint32_t		SignatureLength; 	// NB: DRM_DWORD is unsigned long
	int				HashType; 			// OEM_HAL_HASH_TYPE is enum
	int				SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	uint32_t		IndexIntegrityKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
} nfTeeDrmPRVerifyMsgSignatureReq_t;

#define nfMIN_PR_VERIFY_MSG_SIG_REQ_SIZE sizeof(nfTeeDrmPRVerifyMsgSignatureReq_t)

#if 0 // response is empty
typedef struct
{
	void empty;
} nfTeeDrmPRVerifyMsgSignatureRsp_t;
#endif

// nfTO_DRM_OP_PR_SIGN_WITH_PRIVATE_KEY
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_SignWithPrivateKey(
 *   __in                                DRM_VOID                  *f_pOEMContext,
 *   __inout_ecount_opt( 1 )             DRM_CRYPTO_CONTEXT        *f_pCryptoContext,
 *   __in_bcount( f_cbMessage )    const DRM_BYTE                  *f_pbMessage,
 *   __in                                DRM_DWORD                  f_cbMessage,
 *   __out_bcount_opt( *f_pcbSignature ) DRM_BYTE                  *f_pbSignature,
 *   __inout                             DRM_DWORD                 *f_pcbSignature,
 *   __in                                OEM_HAL_HASH_TYPE          f_eHashType,
 *   __in                                OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
 *   __in                                OEM_HAL_KEY_TYPE           f_eKeyType,
 *   __in                                OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey )
 */
typedef struct
{
	uint8_t		*OemContextPtr;		// DRM_VOID *
	uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	uint8_t		*MsgPtr;			// DRM_BYTE *
	uint32_t	MsgLength; 			// NB: DRM_DWORD is unsigned long
	uint8_t		*SignaturePtr;		// DRM_BYTE *
	uint32_t	SignatureLength; 	// NB: DRM_DWORD is unsigned long
	int			HashType; 			// OEM_HAL_HASH_TYPE is enum
	int			SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexIntegrityKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
} nfTeeDrmPRSignWithPrivKeyReq_t;

#define nfMIN_PR_SIGN_WPRIV_KEY_REQ_SIZE sizeof(nfTeeDrmPRSignWithPrivKeyReq_t)

#if 0
/*
 * Not sure if we should put sig at SignaturePtr (passed in request), or if
 * we need to allocate memory upon receiving response. Assume we are passing
 * shmem pointer into tee for now...
 */
typedef struct
{
	void empty;
} nfTeeDrmPRSignWithPrivKeyRsp_t;
#endif


// nfTO_DRM_OP_PR_VERIFY_OMAC1_SIGNATURE
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_VerifyOMAC1Signature(
 *   __in_bcount( f_cbData )      const DRM_BYTE                   *f_pbData,
 *   __in                               DRM_DWORD                   f_cbData,
 *   __in_bcount( f_cbSignature ) const DRM_BYTE                   *f_pbSignature,
 *   __in                               DRM_DWORD                   f_cbSignature,
 *   __in                               OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                               OEM_HAL_KEY_REGISTER_INDEX  f_indexKey );
 */
typedef struct
{
	uint8_t			*DataPtr;			// DRM_BYTE *
	uint32_t		DataLength; 		// NB: DRM_DWORD is unsigned long
	uint8_t			*SignaturePtr;
	uint32_t		SignatureLength; 	// NB: DRM_DWORD is unsigned long
	int				SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	int				KeyType;	 		// OEM_HAL_KEY_TYPE is enum
	uint32_t		IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
} nfTeeDrmPRVerifyOmac1SignatureReq_t;

#define nfMIN_PR_VERIFY_OMAC1_SIG_REQ_SIZE sizeof(nfTeeDrmPRVerifyOmac1SignatureReq_t)

#if 0 // response is empty
typedef struct
{
	void empty;
} nfTeeDrmPRVerifyMsgSignatureRsp_t;
#endif


// nfTO_DRM_OP_PR_CREATE_OMAC1_SIGNATURE
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_CreateOMAC1Signature(
 *   __in_bcount( f_cbData )        const DRM_BYTE                   *f_pbData,
 *   __in                                 DRM_DWORD                   f_cbData,
 *   __out_bcount_opt( *f_pcbSignature )  DRM_BYTE                   *f_pbSignature,
 *   __inout                              DRM_DWORD                  *f_pcbSignature,
 *   __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey )
 */
typedef struct
{
	uint8_t			*DataPtr;			// DRM_BYTE *
	uint32_t		DataLength; 		// NB: DRM_DWORD is unsigned long
	uint8_t			*SignaturePtr;
	uint32_t		*SignatureLengthPtr; 	// NB: DRM_DWORD is unsigned long
	int				SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	uint32_t		IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
} nfTeeDrmPRCreateOmac1SignatureReq_t;

#define nfMIN_PR_CREATE_OMAC1_SIG_REQ_SIZE sizeof(nfTeeDrmPRVerifyOmac1SignatureReq_t)

#if 0 // response is empty
typedef struct
{
	void empty;
} nfTeeDrmPRCreateOmac1SignatureRsp_t;
#endif

// nfTO_DRM_OP_PR_ENCRYPT_DATA_BLOCK
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_EncryptDataBlock(
 *   __in_bcount( f_cbData )             const DRM_BYTE                   *f_pbBlock,
 *   __in                                      DRM_DWORD                   f_cbBlock,
 *   __out_bcount_opt( *f_pcbEncryptedBlock )  DRM_BYTE                   *f_pbEncryptedBlock,
 *   __inout                                   DRM_DWORD                  *f_pcbEncryptedBlock,
 *   __in                                      OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                                      OEM_HAL_KEY_REGISTER_INDEX  f_indexMessageEncryptKey )
 */
typedef struct
{
	uint8_t			*DataPtr;			// DRM_BYTE *
	uint32_t		DataLength; 		// NB: DRM_DWORD is unsigned long
	uint32_t		*CiphertextLengthPtr; 	// NB: DRM_DWORD is unsigned long
	uint8_t			*CiphertextPtr;
	int				KeyType;	 		// OEM_HAL_KEY_TYPE is enum
	uint32_t		IndexMsgEncryptKey; // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
} nfTeeDrmPREncryptDataBlockReq_t;

#define nfMIN_PR_ENCR_DATA_BLOCK_REQ_SIZE sizeof(nfTeeDrmPREncryptDataBlockReq_t)

#if 0 // response is empty
typedef struct
{
	void empty;
} nfTeeDrmPREncryptDataBlockRsp_t;
#endif

// nfTO_DRM_OP_PR_UNWRAP_KEY
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_UnwrapKey(
 *   __inout_ecount_opt( 1 )                    DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
 *   __in                                       OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
 *   __in                                       OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
 *   __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
 *   __in_bcount( f_cbWrappedKeyData )    const DRM_BYTE                   *f_pbWrappedKeyData,
 *   __in                                       DRM_DWORD                   f_cbWrappedKeyData,
 *   __in_bcount_opt( f_cbParameterData ) const DRM_BYTE                   *f_pbParameterData,
 *   __in                                       DRM_DWORD                   f_cbParameterData )
 */
typedef struct
{
	uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	int			WrapKeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexWrapKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	uint8_t		*WrappedKeyDataPtr;			// DRM_BYTE *
	uint32_t	WrappedKeyDataLength; 			// NB: DRM_DWORD is unsigned long
	uint8_t		*ParameterDataPtr;		// DRM_BYTE *
	uint32_t	ParameterDataLength; 	// NB: DRM_DWORD is unsigned long
} nfTeeDrmPRUnwrapKeyReq_t;

#define nfMIN_PR_UNWRAP_KEY_REQ_SIZE sizeof(nfTeeDrmPRUnwrapKeyReq_t)

#if 0
typedef struct
{
	void empty;
} nfTeeDrmPRUnwrapKeyRsp_t;
#endif


// nfTO_DRM_OP_PR_WRAP_KEY
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_WrapKey(
 *   __in                                 DRM_VOID                   *f_pOEMContext,
 *   __inout_ecount_opt( 1 )              DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
 *   __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
 *   __in                                 OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
 *   __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
 *   __out_bcount_opt( *f_pcbWrappedKey ) DRM_BYTE                   *f_pbWrappedKey,
 *   __inout_ecount(1)                    DRM_DWORD                  *f_pcbWrappedKey );
 */
typedef struct
{
	uint8_t		*OemContextPtr;     // DRM_VOID *
	uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	int			WrapKeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexWrapKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	uint8_t		*WrappedKeyDataPtr;			// DRM_BYTE *
	uint32_t	*WrappedKeyDataLengthPtr; 			// NB: DRM_DWORD is unsigned long
} nfTeeDrmPRWrapKeyReq_t;

#define nfMIN_PR_WRAP_KEY_REQ_SIZE sizeof(nfTeeDrmPRWrapKeyReq_t)

#if 0
typedef struct
{
	void empty;
} nfTeeDrmPRWrapKeyRsp_t;
#endif

// nfTO_DRM_OP_PR_GENERATE_KEY
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_GenerateKey(
 *   __in                    DRM_VOID                  *f_pOEMContext,
 *   __inout_ecount_opt( 1 ) DRM_CRYPTO_CONTEXT        *f_pCryptoContext,
 *   __in                    OEM_HAL_KEY_TYPE           f_eKeyType,
 *   __in                    OEM_HAL_KEY_REGISTER_INDEX f_indexKey,
 *   __in                    DRM_DWORD                  f_dwRequestedSecurityLevel );
 */
typedef struct
{
	uint8_t		*OemContextPtr;     // DRM_VOID *
	uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	uint32_t	RequestedSecurityLevel; // DRM_DWORD
} nfTeeDrmPRGenerateKeyReq_t;

#define nfMIN_PR_GENERATE_KEY_REQ_SIZE sizeof(nfTeeDrmPRGenerateKeyReq_t)

#if 0
typedef struct
{
	void empty;
} nfTeeDrmPRGenerateKeyRsp_t;
#endif

// nfTO_DRM_OP_PR_DECRYPT_CONTENT
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_DecryptContent(
 *   __inout_bcount( f_cbData ) DRM_BYTE                   *f_pbData,
 *   __in                       DRM_DWORD                   f_cbData,
 *   __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey,
 *   __in                       DRM_UINT64                  f_qwInitializationVector,
 *   __in                       DRM_UINT64                  f_qwSampleByteOffset )
 */
typedef struct
{
	uint8_t		*DataPtr;			// DRM_BYTE *
	uint32_t	DataLength; 		// NB: DRM_DWORD is unsigned long
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexContentKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	uint64_t	InitializationVector;
	uint64_t	SampleByteOffset;
} nfTeeDrmPRDecryptContentReq_t;

#define nfMIN_PR_DECRYPT_CONTENT_REQ_SIZE sizeof(nfTeeDrmPRDecryptContentReq_t)


// 	nfTO_DRM_OP_PR_DECRYPT_CONTENT_OPAQUE
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_DecryptContentOpaque(
 *   __in                       OEM_OPAQUE_BUFFER_HANDLE    f_hInBuf,
 *   __in                       OEM_OPAQUE_BUFFER_HANDLE    f_hOutBuf,
 *   __in                       DRM_DWORD                   f_cbData,
 *   __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
 *   __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey,
 *   __in                       DRM_UINT64                  f_qwInitializationVector,
 *   __in                       DRM_UINT64                  f_qwSampleByteOffset );
 *
 */
typedef struct
{
	uint8_t		*InBufPtr;			// DRM_VOID *
	uint8_t		*OutBufPtr;			// DRM_VOID *
	uint32_t	DataLength; 		// NB: DRM_DWORD is unsigned long
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexContentKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	uint64_t	InitializationVector;
	uint64_t	SampleByteOffset;
} nfTeeDrmPRDecryptContentOpaqueReq_t;

#define nfMIN_PR_DECRYPT_CONTENT_OPAQUE_REQ_SIZE sizeof(nfTeeDrmPRDecryptContentOpaqueReq_t)


// nfTO_DRM_OP_PR_CREATE_SAMPLE_ENCRYPTION_CONTEXT
/*
 * DRM_API DRM_RESULT DRM_CALL Oem_Hal_CreateSampleEncryptionContext(
 *   __in            OEM_HAL_KEY_TYPE                f_eKeyType,
 *   __in            OEM_HAL_KEY_REGISTER_INDEX      f_indexKey,
 *   __in            OEM_HAL_SAMPLE_ENCRYPTION_MODE  f_eSampleEncryptionMode,
 *   __out_ecount(1) OEM_ENCRYPTION_HANDLE          *f_phEncryptionContext );
 */
typedef struct
{
	int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	uint32_t	IndexKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	int			SampleEncryptionMode; // OEM_HAL_SAMPLE_ENCRYPTION_MODE is enum
	void		*EncryptionContextPtr; // OEM_ENCRYPTION_HANDLE
}nfTeeDrmPRCreateSampleEncryptCtxtReq_t;

#define nfMIN_PR_CREATE_SAMPLE_CRYPT_CTXT_REQ_SIZE sizeof(nfTeeDrmPRCreateSampleEncryptCtxtReq_t)


#endif /* DRMTYPES_H_ */
