/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 *      This file implements all the Crypto functionality like
 *      import/export, wrap/unwrap, encrypt/decrypt, hmac, digest etc
 *
 */

#ifndef _CRYPTOAGENT_H_
#define _CRYPTOAGENT_H_
#include "CryptoTypes.h"

// These are the contexts associated with keys
typedef enum
{
	caKC_UNDEFINED = 0,
	caKC_RSA,
	caKC_DH,
	caKC_RAW,

	// leave this last!!
	caKC_INVALID
} nfKeyContextType_t;

// TODO: Check if we can have a "safe" size of raw keys
#define nfMAX_RAW_KEY_LEN 32 // 256 bits

// Raw context is to just store the bits of the key
typedef struct raw_context_t
{
    uint32_t		KeyDataLength; // length of key in bits
    unsigned char 	KeyData[nfMAX_RAW_KEY_LEN];
} nfRawContext_t;

typedef struct
{
	uint32_t	Generator;
	uint32_t	ModulusLength;
	uint8_t		Modulus[nfMAX_DH_MODULUS_LENGTH];
	uint32_t	PrivKeyLength;
	uint8_t		PrivKey[nfMAX_DH_PRIVKEY_LENGTH];
	/*
	 * We save the shared secret to allow multiple calls (for
	 * deriving multiple keys, e.g. with HKDF-CTR).
	 */
	uint32_t	SharedSecretLength;
	uint8_t		SharedSecret[nfMAX_DH_MODULUS_LENGTH];
	/*
	 * NB: Stoopid webcrypto api generates the key pair,
	 * and then asks for the public key in a separate
	 * step. So, we have to store it here.
	 *
	 * If space is an issue, then don't store the public
	 * value, but instead recompute it if/when they ask
	 * for it.
	 */
	uint32_t	PubKeyLength;
	uint8_t		PubKey[nfMAX_DH_MODULUS_LENGTH];
} nfDHContext_t;

/*
 * TBD - we need to look at how to make key storage more efficient. To
 * avoid malloc, we're using a union and preallocating the key table,
 * but this is wasteful, and we can definitely do better.
 */
typedef struct
{
	nfKeyContextType_t	ContextType;
	union
	{
		//nfRsaContext_t	rsaContext;   // in RsaContext.h
		nfDHContext_t 	dhContext;
		nfRawContext_t 	rawContext;
	}ctxt;
} nfKeyContext_t;

nfTeeResult_t cryptoAgentInit( int argc, char **argv );

nfTeeResult_t cryptoAgentDispatchMsg( nfTeeOperation_t	Operation,
							 uint8_t 			*InMsgPtr,
		  	   				 uint32_t 			InMsgBufLength,
		  	   				 uint8_t 			*OutMsgPtr,
		  	   				 uint32_t 			OutMsgBufLength,
		  	   				 uint32_t			*MsgLengthPtr,
		  	   				 nfTeeClientId_t 	ClientId );

#endif
