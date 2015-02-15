/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef COMMTYPES_H_
#define COMMTYPES_H_

#include "CommonIncludes.h"

/*
 * REE application ID, currently used only internally in TEE
 */
#define nfTEE_CLIENT_ID_SIZE	16
typedef uint8_t nfTeeClientId_t[nfTEE_CLIENT_ID_SIZE];

/*
 * TEE services are exposed as "operations". Operation definitions are below.
 */

typedef uint32_t nfTeeOperation_t;

/*
 * Each operation can produce a response, and each response has a response
 * code. There are different levels of response codes, and we'd like to
 * encode enough info within the code itself to be able to get a rough
 * idea of what happened just by looking at the code. For example, we'd
 * like to know the following just from looking a the code:
 *
 * 		- what TEE module or application produced this result?  For example,
 * 		  commAgent, cryptoAgent, playreadyAgent, etc.
 *
 * 		- is it a success or failure?
 *
 * 		- what module within the specific TA produced this response (e.g.
 * 		  (key mgmt module within cryptoAgent, ECC crypto module within
 * 		  playready agent, etc.)
 *
 * We can encode TA information by defining result bases. We'll allocate the
 * top 8 bits of the result for the TA/module identifier. This limits us to
 * 255 TEE modules, but this seems quite reasonable. If we start off using
 * only the leftmost 4 bits (bits 0-3 starting with MSB), we'll have the
 * option of using bits 4-7 for something else later if we need to.
 *
 * #define nfTEE_GENERAL_RESULT_BASE	0xF00000
 * #define nfTEE_AGENT_RESULT_BASE		0xE00000
 * #define nfCRYPTO_AGENT_RESULT_BASE	0xD00000
 * #define nfPLAYREAD_AGENT_RESULT_BASE	0xC00000
 *
 * Then, we can further subdivide the result space of each agent, module, etc,
 * and define macros to interpret these.
 *
 */


/*
 * Error codes for each module start at a specific offset
 */
#define nfTEE_GENERAL_RESULT_BASE		0xF0000000
#define nfMGMT_AGENT_RESULT_BASE		0xE0000000
#define nfCRYPTO_AGENT_RESULT_BASE		0xD0000000
#define nfDRM_AGENT_RESULT_BASE			0xC0000000
#define nfSTORAGE_AGENT_RESULT_BASE		0xB0000000

// general TEE results
typedef enum
{
	nfTGR_SUCCESS = nfTEE_GENERAL_RESULT_BASE,
	nfTGR_INMSG_INVALID,
	nfTGR_INMSG_TOO_SHORT,
	nfTGR_OUTMSG_INVALID,
	nfTGR_OUTMSG_TOO_SHORT,
	nfTGR_INVALID_OPERATION,
	nfTGR_OPERATION_NOT_ALLOWED,
//	nfTGR_BAD_PARAMETER,
	nfTGR_INTERNAL_ERROR

} nfTeeGeneralResult_t;

// used for return codes from all modules
typedef uint32_t nfTeeResult_t;


/*
 * TEE drm operation results
 */
typedef enum
{
    nfTDR_NOT_INITIALIZED = nfDRM_AGENT_RESULT_BASE,
    nfTDR_ALREADY_INITIALIZED,
    nfTDR_INTERNAL_ERROR,
    nfTDR_OPERAND_TOO_SHORT,
    nfTDR_INVALID_OPERAND,
    nfTDR_SIGNATURE_INVALID,


} nfTeeDrmResult_t;

/*
 * TEE mgmt operation results
 */
typedef enum
{
    nfTMR_NOT_INITIALIZED = nfMGMT_AGENT_RESULT_BASE,
    nfTMR_ALREADY_INITIALIZED,
    nfTMR_PARAM_TOO_SMALL,
    nfTMR_PARAM_SIZE_MISMATCH,
} nfTeeMgmtResult_t;


/*
 * TEE crypto agent operation results
 * For cryptoAgent, response codes start at nfCRYPTO_AGENT_RESULT_BASE
 */
typedef enum
{
    nfTCR_NOT_INITIALIZED = nfCRYPTO_AGENT_RESULT_BASE,
    nfTCR_KEY_STORE_FULL,
    nfTCR_KEY_NOT_FOUND,
    nfTCR_OPERATION_NOT_ALLOWED,
    nfTCR_MALFORMED_DATA,
    nfTCR_UNSUPPORTED_KEY_ENCODING,
    nfTCR_BAD_PARAMETER,
    nfTCR_CIPHER_ERROR,
    nfTCR_OUT_OF_MEMORY,
    nfTCR_INTERNAL_ERROR,
    nfTCR_KEY_TYPE_ALG_MISMATCH,
    nfTCR_KEY_TYPE_FORMAT_MISMATCH,
    nfTCR_KEY_LENGTH_UNSUPPORTED,
    nfTCR_OPERAND_TOO_SHORT,
    nfTCR_IMPORT_ENC_KEY_FAILED,
    nfTCR_IMPORT_HMAC_KEY_FAILED,
    nfTCR_ALREADY_INITIALIZED,
    nfTCR_UNKNOWN_KEY_TYPE,
    nfTCR_UNSUPPORTED_KEY_PARAMETER,
    nfTCR_UNKOWN_CLIENTID,
    nfTCR_CRYPT_RANDOM_FAILURE,
    nfTCR_SEALED_KEY_DATA_TOO_LARGE,
    nfTCR_SEALED_KEY_VERSION_UNSUPPORTED,
    nfTCR_SEALED_KEY_VALIDATION_ERROR,
    nfTCR_BUFFER_TOO_SMALL,
    nfTCR_HMAC_VERIFY_FAILED,
    nfTCR_INVALID_DH_GENERATOR,
    nfTCR_INVALID_KEY_HANDLE,

} nfTeeCryptoResult_t;


/*
 * Storage agent operation results
 */
typedef enum
{
    nfTSR_MANUF_SSTORE_INVALID = nfSTORAGE_AGENT_RESULT_BASE,
    nfTSR_OPERAND_TOO_SHORT,
    nfTSR_BUFFER_TOO_SHORT,
    nfTSR_NOT_FOUND,
    nfTSR_KEY_DERIV_ERROR,
    nfTSR_CIPHER_ERROR,
    nfTSR_INTERNAL_ERROR,
    nfTSR_CHUNK_SIZE_EXCEEDED,
    nfTSR_CRYPT_RANDOM_FAILURE,
    nfTSR_OPERAND_TOO_LONG,
    nfTSR_UNRECOGNIZED_VERSION,
 	nfTSR_INIT_FAILED,
} nfTeeStorageResult_t;

/*
 * For each operation, there are zero or more "operands". Operands
 * may be passed by reference, or as immediate data. In our initial
 * implementation, we will always pass operands as immediate data,
 * in a message.
 *
 * Messages between the TEE and REE are protocol messages, and
 * resemble network protocol messages. The outermost layer of
 * these messages represent the TEE IPC/IVC (inter-process/
 * inter-virtual machine communications) API.
 *
 * Since the TEE is implemented as a library in this initial
 * implementation, our IPC/IVC mechanism is very simple:
 *
 * 		- every transaction consists of a request (from the REE)
 * 		  and a reply (from the TEE)
 * 		  -  you can think of this as a reliable datagram service
 * 		  -  if your application requires multiple messages for some
 * 		     transactions, it is up to you to implement fragmentation/
 * 		     reassembly and session management on top of this request/
 * 		     response exchange
 *
 * 		- The outer header of the request contains the operation and the message
 * 		  length
 *
 *  	- reply contents depend on the operation. Because we are explicitly linking
 *  	   the request and response, there is no need to include the operation in
 *  	   the reply (just look at the associated request buffer). A "real" TEE would
 *  	   have to include a session header for this purpose.
 *
 * 		- The outer header of the reply contains only the ResponseCode and
 * 		  message length. Any additional data following this header is
 * 		  operation-specific, i.e. each TA will define its own operation headers
 * 		  and data.
 *
 */


typedef struct
{
	nfTeeOperation_t	Operation; // operation definitions are below
	uint32_t			OperandLength; // length of message following this header
} nfTeeApiReqHdr_t;

// generic response header, all responses begin with ResponseCode
typedef struct
{
	uint32_t	ResponseCode;
	uint32_t	ResponseLength;
} nfTeeApiRspHdr_t;

// minimum req/rsp buffer sizes
#define nfTEE_MIN_REQ_SIZE (sizeof(nfTeeApiReqHdr_t))
#define nfTEE_MIN_RSP_SIZE (sizeof(nfTeeApiRspHdr_t))

/*
 * TEE Operation definitions: TEE applications (TAs) each expose their own APIs
 * in terms of "operations". We want to be able to quickly dispatch operation
 * messages to the appropriate TA, so we'll divide up the 32-bit operation space,
 * allocating some number of "slots" to each TA. We're arbitrarily choosing 2048
 * per application, but we can always grow/shrink this in the future if that
 * makes sense.
 */

// operations for each module (cryptoAgent, storageAgent, etc.) are >= base for that module
typedef enum
{
	nfTOB_UNDEFINED = 0,

	/*
	 * Some messages are for TEE (or VM) management. Those start at offset 0
	 */
	nfTOB_TEE_MGMT = 1,

	/*
	 * We're not implementing REE/TEE session management in the first revision, but we'll
	 * need it later, so let's allocate a slot for that.
	 */
    nfTOB_TEE_SESSION = 2048,

    /*
     * The first TA we'll implement is the crypto agent. This agent is used for all
     * webcrypto/msl operations, and also can provide secure store crypto services.
     */
	nfTOB_CRYPTO_AGENT = 4096,

	/*
	 * PlayReady integration is a stretch goal for the first revision, so let's
	 * allocate a slot for it.
	 */
	nfTOB_DRM_AGENT = 6144,

	/*
	 * The storage agent only manages the manufacturing secure store
	 * in the first release.
	 */
	nfTOB_STORAGE_AGENT = 8192,

} nfTeeOperationBase_t;

// TEE mgmt operations (e.g. teeInit() are nrdTOB_TEE_MGMT + offset)
// TEE session operations are (nrdTOB_TEE_SESSION + offset)
// crypto operations are (nrdTOB_CRYPTO_AGENT + offset)
// playready operations are (nrdTOB_PLAYREADY_AGENT + offset)

/*
 * TEE API communications are in network byte order, because not all TEEs
 * and REEs share the same processor architecture type. In addition, some
 * processors do not allow unaligned accesses to memory locations, so anywhere
 * we need to read/write unaligned data, we need to deal with this. The
 * following macros are for this purpose, but you should define assembly
 * functions for this.
 */

// TBD: these should be inline functions; also, define ntohl, htonl
#define nfPUTWORD32(ADDR, VALUE) \
	do { (*((uint8_t *)(ADDR)) = ((VALUE) & 0xFF000000)) >> 24; \
	     (*((uint8_t *)(ADDR) + 1) = ((VALUE) & 0x00FF0000)) >> 16; \
	     (*((uint8_t *)(ADDR) + 2) = ((VALUE) & 0x0000FF00)) >> 8; \
	     (*((uint8_t *)(ADDR) + 3) = ((VALUE) & 0x000000FF)); } while(0)

#define nfPUTWORD16(ADDR, VALUE) \
	do { (*((uint8_t *)(ADDR)) = ((VALUE) & 0xFF00)) >> 8; \
	     (*((uint8_t *)(ADDR) + 1) = ((VALUE) & 0x00FF)); } while(0)

#define nfGETWORD32(ADDR, VALUE) \
	do { VALUE = *((uint8_t *)(ADDR)) << 24; \
	     VALUE |= *((uint8_t *)(ADDR) + 1) << 16; \
	     VALUE |= *((uint8_t *)(ADDR) + 2) << 8; \
	     VALUE |= *((uint8_t *)(ADDR) + 3); } while(0)

#define nfGETWORD16(ADDR, VALUE) \
	do { VALUE = *((uint8_t *)(ADDR)) << 8; \
		 VALUE |= *((uint8_t *)(ADDR) + 1); } while(0)




/**
* 
*/
#define MAX_BUFFER_SIZE 1024*256	//each buffer can be half an MB in size
#define MAX_REQ_DATA_SIZE (MAX_BUFFER_SIZE - nfTEE_MIN_REQ_SIZE)
#define MAX_RSP_DATA_SIZE (MAX_BUFFER_SIZE - nfTEE_MIN_RSP_SIZE)
 	
typedef struct
{
	uint32_t reqBufLen;
	uint8_t reqBuf[MAX_REQ_DATA_SIZE];
	uint32_t rspBufLen;
	uint8_t rspBuf[MAX_REQ_DATA_SIZE];
}nfTeeTransData_t;



#define nfIDFLIE_MAX_FILE_SIZE		(512)
#define nfDRM_ZGPRIV_LEN			(512)
#define nfDRM_BGROUPCERT_SIZE		(4096)
#define nfDRM_CERT_TEMPLATE_SIZE	(4096)



/*
 * This is the TEE entry point function definition. To communicate with
 * the TEE, you must allocate two buffers that are large enough to hold
 * whatever request/response you need.
 *
 * This function expects InMsgPtr to contain nfTeeApiReqHdr_t at offset
 * 0. This header will typically be followed by additional operation-specific
 * headers. The value of InMsgBufLength must be the total size of the buffer.
 * The value of ((nfTeeApiReqHdr_t *)InMsgPtr)->OperandLength must be less than
 * or equal to (InMsgBufLength - sizeof(nfTeeApiReqHdr_t)).
 *
 * Upon return from this function, OutMsgPtr will contain nfTeeApiRspHdr_t at
 * offset 0. That data struct contains a ResponseLength field that, if non-zero,
 * indicates how much additional data is in the buffer. Note that if OutMsgBufLength
 * (total size of the buffer) is not large enough to contain the response, this
 * will result in failure (e.g. return of nfTGR_OUTMSG_TOO_SHORT in the response
 * header). if OutMsgBufLength is not large enough to hold even nfTeeApiRspHdr_t,
 * this will result in an assert (i.e. program exit).
 */
#ifdef __cplusplus
extern "C" {
#endif
void nfTeeDispatchMsg( uint8_t	*InMsgPtr,
   	   	   	   	   	   uint32_t InMsgBufLength,
	   	   	   	   	   uint8_t 	*OutMsgPtr,
	   	   	   	   	   uint32_t OutMsgBufLength );

int _TzServiceCall(int i, int j,int k,int l);


#ifdef __cplusplus
}
#endif


#endif /* COMMTYPES_H_ */



