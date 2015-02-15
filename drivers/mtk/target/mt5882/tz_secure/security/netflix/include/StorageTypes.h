/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STORAGETYPES_H_
#define STORAGETYPES_H_

#include "CommonIncludes.h"

/*
 * storage agent messages
 */

typedef enum
{
	nfTO_STORAGE_OP_SIMPLE_READ = nfTOB_STORAGE_AGENT + 1,
	nfTO_STORAGE_OP_COMPOUND_READ,
	nfTO_STORAGE_OP_PROTECT,
	nfTO_STORAGE_OP_UNPROTECT,
	// insert new message types here...

    nfTO_STORAGE_OP_INVALID // leave this last!!
} nfTeeOperation_StorageOp_t;

#define nfTEE_IS_STORAGE_OP(x) (((x) > nfTOB_STORAGE_AGENT) && ((x) < nfTO_STORAGE_OP_INVALID))

/*
 * Secure store data elements are defined by a "key". To keep
 * the TEE code simple, keys are 32-bit values (no strings).
 */
typedef uint32_t nfSecureStoreClientKey_t;

typedef struct
{
	nfSecureStoreClientKey_t	RequestedKey;
} nfTeeStorageSimpleReadReq_t;

#define nfMIN_SIMPLE_READ_REQ_SIZE sizeof(nfTeeStorageSimpleReadReq_t)

typedef struct
{
	nfTeeClientId_t	ClientId; // client id associated with this data
	uint32_t		DataLength;
	uint8_t			Data[/*DataLength*/];
} nfTeeStorageSimpleReadRsp_t;

#define nfMIN_SIMPLE_READ_RSP_SIZE sizeof(nfTeeStorageSimpleReadRsp_t)

/*
 * For application secure store, the app requests "protect" operations to
 * encrypt/authenticate date, and "unprotect" operations to verify/decrypt
 * data. It is expected that the application will ensure that the item to
 * be protected/unprotected fits entirely in a single message. We may
 * add multi-message data support in the future (but not today).
 *
 * When you protect data, it is expanded according to the following format:
 *
 * {version}{IV}{encrypted data}{HMAC}
 * |-------------RawLength ----------|
 * |----------------- b64Length ---------------|
 *
 * where b64Length = 4/3 * RawLength.
 *
 * Version is 4 bytes, IV is 16 bytes, and HMAC is 32 bytes. In addition, if
 * your data size is not evenly divisible by 16, it will be padded up to 15
 * bytes. So, maximum expansion is 4+16+32+15=67. We have to limit memory
 * usage in the TEE, so we will fix the preallocated buffer size at 32K; if your
 * data is larger than 3/4 of this, you will have to break it into chunks.
 *
 * Keep in mind that this is the size of the base64-encoded data, 32K=4(n/3),
 * and your data must be no more than 24523 bytes when protected.
 *
 * If this is not large enough, then you should increase the allocation amount
 * (by increasing nfTEE_MAX_INTERNAL_SIZE below)
 */

#define nfTEE_STORAGE_OVERHEAD	(4+16+32+15)
#define nfTEE_MAX_INTERNAL_SIZE	(32768*4) // SDK requires 128K
#define nfTEE_MAX_PROTECT_SIZE (((3*nfTEE_MAX_INTERNAL_SIZE)/4)-nfTEE_STORAGE_OVERHEAD)


// TBD: define all related defs somewhere central so defs can be shared...
#define nfTEE_MIN_PROTECTED_DATA_SIZE (sizeof(uint32_t) + 16 + 16 + 32)

typedef struct
{
	uint32_t		DataLength;
	uint8_t			Data[/*DataLength*/];
} nfTeeStorageProtectReq_t;

#define nfMIN_STORAGE_PROTECT_REQ_SIZE (sizeof(nfTeeStorageProtectReq_t) + 1)

typedef struct
{
	uint32_t		ProtectedDataLength;
	uint8_t			ProtectedData[/*DataLength*/];
} nfTeeStorageProtectRsp_t;

#define nfMIN_STORAGE_PROTECT_RSP_SIZE (sizeof(nfTeeStorageProtectRsp_t) + nfTEE_MIN_PROTECTED_DATA_SIZE)

typedef struct
{
	uint32_t		ProtectedDataLength;
	uint8_t			ProtectedData[/*DataLength*/];
} nfTeeStorageUnprotectReq_t;

#define nfMIN_STORAGE_UNPROTECT_REQ_SIZE (sizeof(nfTeeStorageUnprotectReq_t) + nfTEE_MIN_PROTECTED_DATA_SIZE)


typedef struct
{
	uint32_t		DataLength;
	uint8_t			Data[/*DataLength*/];
} nfTeeStorageUnprotectRsp_t;

#define nfMIN_STORAGE_UNPROTECT_RSP_SIZE (sizeof(nfTeeStorageUnprotectRsp_t) + 1)

#endif /* STORAGETYPES_H_ */
