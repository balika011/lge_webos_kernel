/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "KeyMgmt.h"
#include "CryptoAgent.h"
#include "StorageAgent.h"
#include "TeeDebug.h"
#include "TeeRsaContext.h"
#include "TeeInternalDpi.h"
#include "TeeAesCipher.h"
#include "TeeAesWrapper.h"
#include "cJSON.h"
#include "b64.h"
#include "KeyMgmtJson.h"

//*********************  Private function declarations *****************************

/*********************************************************************
 * @function caCreateRsaContextFromJson
 *
 * @brief create RSA context at *KeyContextPtr from JWK
 *
 * @param[in] JsonPtr - the JSON key structure
  * @param[out] KeyContextPtr - pointer to key context
 *
 *
 * @returns nfTGR_SUCCESS if successful, nfTCR_BAD_PARAMETER
 *
 *********************************************************************/
static nfTeeCryptoResult_t caCreateRsaContextFromJson( cJSON				*JsonPtr,
													nfKeyContext_t 		*KeyContextPtr );

/*********************************************************************
 * @function caCreateRawContextFromJson
 *
 * @brief create RAW context at *KeyContextPtr from JWK
 *
 * @param[in] JsonPtr - the JSON key structure
  * @param[out] KeyContextPtr - pointer to key context
 *
 *
 * @returns nfTGR_SUCCESS if successful, nfTCR_BAD_PARAMETER
 *
 *********************************************************************/
static nfTeeCryptoResult_t caCreateRawContextFromJson( cJSON				*JsonPtr,
													nfKeyContext_t 		*KeyContextPtr );
/*********************************************************************
 * @function caGetJwkAlgorithmFromStr
 *
 * @brief gets enum value nfCryptoAlgorithm_t from the string
 *        NOTE: this method only gets algorithms that JWK can have
 *
 * @param[in] algStr - the algorithm in string
 *
 *
 * @returns nfCryptoAlgorithm_t algorithm
 *
 *********************************************************************/
static nfCryptoAlgorithm_t caGetJwkAlgorithmFromStr (char* algStr);

/*********************************************************************
 * @function caGetJwkUsageFromStr
 *
 * @brief gets enum value nfKeyUsageFlag_t from the string
 *
 * @param[in] usageStr - key usage in string
 *
 *
 * @returns nfKeyUsageFlag_t usage
 *
 *********************************************************************/
static nfKeyUsageFlag_t caGetJwkUsageFromStr (char* usageStr);

//*********************  Function implementations *****************************

nfTeeCryptoResult_t caParseJwk( uint8_t 				KeyData[],
                                uint32_t 				dataLength,
                                nfCryptoAlgorithm_t 	*algoPtr,
                                nfKeyUsageFlag_t 		*keyUsageFlagsPtr,
                                nfKeyType_t				*keyTypePtr,
                                nfKeyContext_t			*KeyContextPtr)
{
	int 				i;
	cJSON				*JsonPtr;
	cJSON				*algPtr;
	cJSON				*usePtr;
	cJSON				*extPtr;
	nfTeeCryptoResult_t result;

	/*
	 * We need to do the following:
	 * 	- parse/validate the JWK into a JSON struct
	 * 	- check to make sure we have all necessary info in the JWK
	 * 	- import the key
	 * 	- send a response
	 */

	// first, do a safety check
	if ( dataLength >= nfMAX_JWK_LENGTH )
	{
		CRYPTODBG("%s: KeyDataLength is too long (%d/%d)\n", __FUNCTION__, dataLength, nfMAX_JWK_LENGTH);
		return nfTCR_BAD_PARAMETER;
	}

	for ( i = 0; i < (int)dataLength; ++i )
	{
		if ( KeyData[i] == '\0' )
			break;
	}

	if ( (i == (int)dataLength) && (KeyData[i] != '\0') )
	{
		CRYPTODBG("%s: KeyDataLength is too long (%d/%d)\n", __FUNCTION__, dataLength, nfMAX_JWK_LENGTH);
		return nfTCR_MALFORMED_DATA;
	}

	if ((result = caCreateContextFromJson(KeyData, KeyContextPtr)) != (nfTeeCryptoResult_t) nfTGR_SUCCESS)
	{
		CRYPTODBG("%s: caCreateContextFromJson returned error %08x\n", __FUNCTION__, result);
		return nfTCR_MALFORMED_DATA;
	}

	// set keytype
	if (KeyContextPtr->ContextType == caKC_RAW)
	{
		*keyTypePtr = nfKT_SECRET;
	}
	else //(KeyContextPtr->ContextType == caKC_RSA
	{
		*keyTypePtr = (KeyContextPtr->ctxt.rsaContext.hasPrivateKey_) ? nfKT_PRIVATE :nfKT_PUBLIC;
	}

	if ((JsonPtr = (cJSON_Parse((const char*) KeyData))) != NULL)
	{
		// override algo, keyUsageFlags with values of "alg", "use" and "extractable" from json
		if ((algPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_ALG_KEY)) != NULL)
		{
			if ((algPtr->valuestring) != NULL)
			{
				*algoPtr = caGetJwkAlgorithmFromStr(algPtr->valuestring);
			}
		}
		if ((usePtr = cJSON_GetObjectItem(JsonPtr, nfJWK_USE_KEY)) != NULL)
		{
			if ((usePtr->valuestring) != NULL)
			{
				*keyUsageFlagsPtr = caGetJwkUsageFromStr(usePtr->valuestring);
			}
		}
		if ((extPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_EXTRACTABLE_KEY)) != NULL)
		{
			if ((extPtr->valueint) == 0)
			{
				// remove extractable from keyUsageFlags
				(*keyUsageFlagsPtr) &= ~(nfKUF_EXTRACTABLE);
			}
			else
			{
				// add extractable from keyUsageFlags
				(*keyUsageFlagsPtr) |= nfKUF_EXTRACTABLE;
			}
		}
		cJSON_Delete(JsonPtr);
	}

	return nfTGR_SUCCESS;
}/* end ImportJwk */

nfTeeCryptoResult_t caCreateJWK ( nfCryptoAlgorithm_t 		Algorithm,
									uint8_t					*algStr,
									nfKeyUsageFlagsWord_t 	KeyUsageFlags,
									uint8_t					*useStr,
									nfKeyContext_t 			*KeyContextPtr,
									uint32_t				*jwkStrLen,
									uint8_t					*jwkStrPtr)
{
	if ((algStr == NULL) || (KeyContextPtr == NULL) || (jwkStrLen == NULL) || (jwkStrPtr == NULL))
	{
		CRYPTODBG("%s: one of the required parameters are NULL\n", __FUNCTION__);
		return nfTCR_BAD_PARAMETER;
	}

	cJSON* jwkPtr = cJSON_CreateObject();

	//	JWK has the following JSON structure
	//	{
	//	      'kty':          key type, e.g. 'RSA', 'EC', 'oct' (REQUIRED)
	//	      'use':          key usage, e.g. 'sig', 'enc', 'wrap' (OPTIONAL)
	//	      'alg:           key algorithm, e.g. 'RSA1_5, 'A128CBC', 'A128GCM', 'HS256',
	//	                                          'A128KW' (OPTIONAL)
	//	      'kid':          key ID, e.g. ignore this (OPTIONAL)
	//	      'extractable':  true or false (OPTIONAL)
	//	      <kty-dependent parms>
	//	}

    // RAW AES or HMAC key
    if (( nfIS_AES_ALG(Algorithm) || nfIS_HMAC_ALG(Algorithm))
    	&& (KeyContextPtr->ContextType == caKC_RAW))
    {
    	// add "kty":"oct"
    	cJSON_AddStringToObject(jwkPtr, nfJWK_KTY_KEY, nfJWK_KTY_OCT);

    	// add "k":"<key data>"
    	int encKeyLen = Base64encode_len(KeyContextPtr->ctxt.rawContext.KeyDataLength);
    	char encKey[encKeyLen];
    	encKeyLen = Base64encode(encKey,
    							(const char*) KeyContextPtr->ctxt.rawContext.KeyData,
    							KeyContextPtr->ctxt.rawContext.KeyDataLength);
    	cJSON_AddStringToObject(jwkPtr, nfJWK_OCT_PARM_k, (const char*) encKey);
    }
    // RSA public key
    else if (nfIS_RSA_ALG(Algorithm)
    		&& (KeyContextPtr->ContextType == caKC_RSA)
    		&& (KeyContextPtr->ctxt.rsaContext.hasPublicKey_))
    {
    	// add "kty":"oct"
    	cJSON_AddStringToObject(jwkPtr, nfJWK_KTY_KEY, nfJWK_KTY_RSA);

    	// get base-64 encoded 'n' and 'e'
    	unsigned char* nPtr;
    	uint32_t nLen;
    	unsigned char* ePtr;
    	uint32_t eLen;
    	nfTeeCryptoResult_t result = teeRsaGetPublicRaw(&(KeyContextPtr->ctxt.rsaContext),
														&nPtr, &nLen,
														&ePtr, &eLen);
        if (result != (nfTeeCryptoResult_t) nfTGR_SUCCESS)
        {
        	CRYPTODBG("%s: Error extracting fields of RSA key\n", __FUNCTION__);
        	cJSON_Delete(jwkPtr);
            return result;
        }
        else
        {
        	cJSON_AddStringToObject(jwkPtr, nfJWK_RSA_PARM_n, (const char*) nPtr);
        	cJSON_AddStringToObject(jwkPtr, nfJWK_RSA_PARM_e, (const char*) ePtr);
        }
    }
    else
    {
		CRYPTODBG("%s: ALgorithm must be one of AES, RSA or HMAC\n", __FUNCTION__);
		cJSON_Delete(jwkPtr);
		return nfTCR_BAD_PARAMETER;
    }

    // add 'use'
    if (useStr != NULL)
    {
    	cJSON_AddStringToObject(jwkPtr, nfJWK_USE_KEY, (const char*) useStr);
    }

    // add 'alg'
    cJSON_AddStringToObject(jwkPtr, nfJWK_ALG_KEY, (const char*) algStr);

    // add 'extractable'
    if (KeyUsageFlags & nfKUF_EXTRACTABLE)
    {
    	cJSON_AddStringToObject(jwkPtr, nfJWK_EXTRACTABLE_KEY, "true");
    }
    else
    {
    	cJSON_AddStringToObject(jwkPtr, nfJWK_EXTRACTABLE_KEY, "false");
    }

    // get the string of JSON representation
    char* jsonStr = cJSON_PrintUnformatted(jwkPtr);
    *jwkStrLen = (uint32_t) teeStrlen((const char*) jsonStr);
    teeMemcpy(jwkStrPtr, jsonStr, *jwkStrLen);

    //delete cJSON object and return
    teeMemFree((uint8_t**)(&jsonStr));
    cJSON_Delete(jwkPtr);
	return nfTGR_SUCCESS;
}

void caCreateJweHeader (uint8_t		*algData,
						uint32_t	algLength,
						uint8_t		*encData,
						uint32_t	encLength,
						uint8_t		**header,
						uint32_t	*headerLength)
{
	// we can assume that all arguments are valid since calling function would have checked them

	char algStr[algLength+1];
	teeMemcpy(algStr, algData, algLength);
	algStr[algLength] = '\0';
	char encStr[algLength+1];
	teeMemcpy(encStr, encData, encLength);
	encStr[encLength] = '\0';

	cJSON* cjsonHdr = cJSON_CreateObject();
	cJSON_AddStringToObject(cjsonHdr, nfJWE_ALG_KEY, algStr);
	cJSON_AddStringToObject(cjsonHdr, nfJWE_ENC_KEY, encStr);
	char* strHdr = cJSON_PrintUnformatted(cjsonHdr);
	int strHdrLen = teeStrlen(strHdr);
	CRYPTODBG("%s: JWE header before encoding: %s\n", __FUNCTION__, strHdr);

	*headerLength = Base64encode_len(strHdrLen);
	*header = (uint8_t*) (teeMemAlloc(*headerLength));
	Base64encode((char*)(*header), strHdr, strHdrLen);
	//decrement length because Base64encode_len returns size for terminating \0 as well
	(*headerLength) = (*headerLength) - 1;
	CRYPTODBG("%s: JWE header after base64 encoding: %s\n", __FUNCTION__, *header);

	//free memory and return
	teeMemFree((uint8_t**) (&strHdr));
	cJSON_Delete(cjsonHdr);
}

nfTeeCryptoResult_t caCreateContextFromJson( uint8_t				*jsonStr,
											nfKeyContext_t 		*KeyContextPtr )
{
	if (( jsonStr == NULL ) || (KeyContextPtr == NULL))
	{
		CRYPTODBG("%s: jsonStr or KeyContextPtr are NULL\n", __FUNCTION__);
		return nfTCR_BAD_PARAMETER;
	}

	// create JSON object
	cJSON *JsonPtr = cJSON_Parse((const char *)jsonStr);
	if (JsonPtr == NULL)
	{
		CRYPTODBG("%s: could not create cJSON instance\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	// extract key type
	cJSON *ktyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_KTY_KEY);

	if ( ktyPtr == NULL )
	{
		CRYPTODBG("%s: JSON does not contain kty.\n", __FUNCTION__);
		cJSON_Delete(JsonPtr);
		return nfTCR_MALFORMED_DATA;
	}
	else
	{
		if ( ktyPtr->valuestring == NULL )
		{
			CRYPTODBG("%s: JSON does not contain kty string?!.\n", __FUNCTION__);
			cJSON_Delete(JsonPtr);
			return nfTCR_MALFORMED_DATA;
		}
		CRYPTODBG("%s: ktyPtr->valuestring: >%s<\n", __FUNCTION__, ktyPtr->valuestring);

		// else
		nfTeeCryptoResult_t result = nfTCR_NOT_INITIALIZED;
		if (teeMemcmp( ktyPtr->valuestring, nfJWK_KTY_OCT, nfJWK_KTY_OCT_LEN ) == 0 )
		{
			result = caCreateRawContextFromJson( JsonPtr, KeyContextPtr);
			CRYPTODBG("%s: caCreateRawContextFromJson() returned %08x, ContextPtr is %p\n", __FUNCTION__, result, KeyContextPtr);
			CRYPTODBG("%s: key data length %d...\n", __FUNCTION__ , KeyContextPtr->ctxt.rawContext.KeyDataLength);
		}
		else if (teeMemcmp( ktyPtr->valuestring, nfJWK_KTY_RSA, nfJWK_KTY_RSA_LEN ) == 0 )
		{
			result = caCreateRsaContextFromJson( JsonPtr, KeyContextPtr);
			CRYPTODBG("%s: caCreateRsaContextFromJson() returned %08x, ContextPtr is %p\n", __FUNCTION__, result, KeyContextPtr);
		}

		cJSON_Delete(JsonPtr);
		return result;
	}
} //end caCreateContextFromJson

static nfTeeCryptoResult_t caCreateRsaContextFromJson( cJSON				*JsonPtr,
								  					nfKeyContext_t 		*KeyContextPtr )
{
	// args checking
	if (( JsonPtr == NULL ) || (KeyContextPtr == NULL))
	{
		CRYPTODBG("%s: JsonPtr or KeyContextPtr are NULL\n", __FUNCTION__);
		return nfTCR_BAD_PARAMETER;
	}

	cJSON *dKeyPtr;
	cJSON *nKeyPtr;
	cJSON *eKeyPtr;
	cJSON *pKeyPtr;
	cJSON *qKeyPtr;
	cJSON *dpKeyPtr;
	cJSON *dqKeyPtr;
	cJSON *qiKeyPtr;
	cJSON *othKeyPtr;
	nfTeeCryptoResult_t Result;

	/*
	 * Okay, we have an RSA key in a JSON structure. At this point, we
	 * don't know if it's a private key or a public key. We have to look
	 * at the values in the JWK and figure out which it is, keeping in
	 * mind that it might be a trick (?!)
	 *
	 * A private key potentially has the following parameters:
	 *
	 * kty: RSA
	 * n: b64 bignum (modulus)
	 * e: b64 bignum (public exponent)
	 * d: b64 bignum (private exponent)
	 * p: b64 bignum  (optional )
	 * q: b64 bignum  (optional)
	 * dp: b64 bignum (optional)
	 * dq: b64 bignum (optional)
	 * qi: b64 bignum (optional)
	 * alg: string
	 * kid:  (optional) string
	 * oth: (array, special handling required, hope I don't see this for a while :-))
	 */

	CRYPTODBG("%s - XXX - need defensive checks - XXX\n", __FUNCTION__ );

	// let's see whatcha got.
	dKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_d);
	nKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_n);
	eKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_e);
	pKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_p);
	qKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_q);

	dpKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_dp);
	dqKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_dq);
	qiKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_qi);

	othKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_RSA_PARM_oth);

	// we don't handle "oth" right now, so puke if that is here.
	if ( othKeyPtr != NULL )
	{
		CRYPTODBG("%s: unsupported RSA parm (oth)\n", __FUNCTION__);
		return nfTCR_UNSUPPORTED_KEY_PARAMETER;
	}

	// ---------------------------------------------------------------------------------
	// BEGIN SANITY CHECKS

	/* SC-2: we must have n */
	if ( nKeyPtr == NULL )
	{
		CRYPTODBG("%s: unsupported RSA parm (oth)\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}
	// SC-3: if p, then q, if q then p
	if ( (pKeyPtr != NULL) ^ (qKeyPtr != NULL) )
	{
		CRYPTODBG("%s: must provide both or neither q and p\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	// SC-4: if dp then p
	if ( (dpKeyPtr != NULL ) && (pKeyPtr == NULL) )
	{
		CRYPTODBG("%s: can't provide dp without p\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	// SC-5: if dq then q
	if ( (dqKeyPtr != NULL ) && (qKeyPtr == NULL) )
	{
		CRYPTODBG("%s: can't provide dq without q\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	// SC-6: if qi then p and q
	if ( ( qiKeyPtr != NULL ) && ( (pKeyPtr == NULL) || (qKeyPtr == NULL) ) )
	{
		CRYPTODBG("%s: can't provide qi without p and q\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;

	}

	// TBD: need othKeyPtr sanity checks

	/*
	 * TBD: not all implementations support all parameters; partners will need
	 * sanity checks specific to their implementations.
	 */

	// END SANITY CHECKS
	// ---------------------------------------------------------------------------------

	/*
	 * Okay, now we need to create the key context.
	 */
	CRYPTODBG("%s: calling teeRsaCreateCtxt()\n", __FUNCTION__ );

	KeyContextPtr->ContextType = caKC_RSA;
	Result = teeRsaCreateCtxt( nKeyPtr->valuestring,
							   ((eKeyPtr) ? eKeyPtr->valuestring : NULL),
							   ((dKeyPtr) ? dKeyPtr->valuestring : NULL),
							   ((pKeyPtr) ? pKeyPtr->valuestring : NULL),
							   ((qKeyPtr) ? qKeyPtr->valuestring : NULL),
							   ((dpKeyPtr) ? dpKeyPtr->valuestring : NULL),
							   ((dqKeyPtr) ? dqKeyPtr->valuestring : NULL),
							   ((qiKeyPtr) ? qiKeyPtr->valuestring : NULL),
								&((KeyContextPtr->ctxt).rsaContext) );

	if ( Result != (nfTeeCryptoResult_t)nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: teeRsaCreateCtxt failed.\n", __FUNCTION__);
		return Result;
	}

	return nfTGR_SUCCESS;

}/* end importRsaJwk */

static nfTeeCryptoResult_t caCreateRawContextFromJson( cJSON				*JsonPtr,
													nfKeyContext_t 		*KeyContextPtr )
{
	// args checking
	if (( JsonPtr == NULL ) || (KeyContextPtr == NULL))
	{
		CRYPTODBG("%s: JsonPtr or KeyContextPtr are NULL\n", __FUNCTION__);
		return nfTCR_BAD_PARAMETER;
	}

	// extract k value, it is required
	cJSON *kKeyPtr = cJSON_GetObjectItem(JsonPtr, nfJWK_OCT_PARM_k);
	if (( kKeyPtr == NULL ) || ((kKeyPtr->valuestring) == NULL ))
	{
		CRYPTODBG("%s: must provide k for oct\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	int decKLen = Base64Urldecode_len(kKeyPtr->valuestring);
	// dynamic stack allocation a bad idea from a security perspective.
	// this will go away once we finish DH, so I will leave it for now.
	char kKeyDec [decKLen+1];

	decKLen = Base64decode_url(kKeyDec, kKeyPtr->valuestring);
	CRYPTODBG("%s: k: %s, length:%d, decoded length:%d", __FUNCTION__, kKeyPtr->valuestring, teeStrlen(kKeyPtr->valuestring), decKLen);

	KeyContextPtr->ContextType = caKC_RAW;
	KeyContextPtr->ctxt.rawContext.KeyDataLength = (uint32_t) decKLen;
	teeMemcpy((KeyContextPtr->ctxt.rawContext.KeyData), kKeyDec, KeyContextPtr->ctxt.rawContext.KeyDataLength);

	return nfTGR_SUCCESS;
}

static nfCryptoAlgorithm_t caGetJwkAlgorithmFromStr (char* algStr)
{
    /*
      From an email from Mark. These are the JWK alg parameters to support
      | HS256        | HMAC using SHA-256 hash        | REQUIRED          |
      |              | algorithm                      |                   |
      | HS384        | HMAC using SHA-384 hash        | OPTIONAL          |
      |              | algorithm                      |                   |
      | HS512        | HMAC using SHA-512 hash        | OPTIONAL          |
      |              | algorithm                      |                   |
      | RS256        | RSASSA using SHA-256 hash      | RECOMMENDED       |
      |              | algorithm                      |                   |
      | RS384        | RSASSA using SHA-384 hash      | OPTIONAL          |
      |              | algorithm                      |                   |
      | RS512        | RSASSA using SHA-512 hash      | OPTIONAL          |
      |              | algorithm                      |                   |
      +----------------+---------------------------------+----------------+
      | RSA1_5         | RSAES-PKCS1-V1_5 [RFC3447]      | REQUIRED       |
      | RSA-OAEP       | RSAES using Optimal Asymmetric  | OPTIONAL       |
      |                | Encryption Padding (OAEP)       |                |
      |                | [RFC3447], with the default     |                |
      |                | parameters specified by RFC     |                |
      |                | 3447 in Section A.2.1           |                |
      | A128KW         | Advanced Encryption Standard    | RECOMMENDED    |
      |                | (AES) Key Wrap Algorithm        |                |
      |                | [RFC3394] using 128 bit keys    |                |
      | A256KW         | AES Key Wrap Algorithm using    | RECOMMENDED    |
      |                | 256 bit keys                    |                |
      | A128GCM       | AES in Galois/Counter Mode (GCM) | RECOMMENDED    |
      |               | [AES] [NIST.800-38D] using 128   |                |
      |               | bit keys                         |                |
      | A256GCM       | AES GCM using 256 bit keys       | RECOMMENDED    |
      +---------------+----------------------------------+----------------+

      What is missing is A128CBC (and A128CTR, though we don't need that), because
      there is no JSON Web Algorithm that uses A128CBC alone.
      There can be much doubt what the string should be (AxxxCBC) for
      xxx=128, 256, 384 etc. so we're on safe ground.
    */
	nfCryptoAlgorithm_t algo = nfCA_UNDEFINED;

	//HS256, HS384, HS512
	if (teeMemcmp(algStr, "HS", 2) == 0)
	{
		algo = nfCA_HMAC;
	}
	else if (teeMemcmp(algStr, "RSA1_5", 6) == 0)
	{
		algo = nfCA_RSAES_PKCS1_V1_5;
	}
	else if (teeMemcmp(algStr, "RSA-OAEP", 8) == 0)
	{
		algo = nfCA_RSA_OAEP;
	}
	//RS256, RS384, RS512
	else if (teeMemcmp(algStr, "RS", 2) == 0)
	{
		algo = nfCA_RSASSA_PKCS1_V1_5;
	}
	//all algorithms starting with A
	else if (algStr[0] == 'A')
	{
		//AXXXKW
		if (teeMemcmp(algStr+4, "KW", 2) == 0)
		{
			algo = nfCA_AES_KW;
		}
		//AXXXGCM
		else if (teeMemcmp(algStr+4, "GCM", 3) == 0)
		{
			algo = nfCA_AES_GCM;
		}
		//AXXXCBC
		else if (teeMemcmp(algStr+4, "CBC", 3) == 0)
		{
			algo = nfCA_AES_CBC;
		}
	}

	return algo;
}

static nfKeyUsageFlag_t caGetJwkUsageFromStr (char* usageStr)
{
	nfKeyUsageFlag_t usage = 0;
	if (teeMemcmp(usageStr, nfJWK_USE_PARM_sig, nfJWK_USE_PARM_sig_LEN) == 0)
	{
		usage |= nfKUF_SIGN;
		usage |= nfKUF_VERIFY;
	}
	if (teeMemcmp(usageStr, nfJWK_USE_PARM_enc, nfJWK_USE_PARM_enc_LEN) == 0)
	{
		usage |= nfKUF_ENCRYPT;
		usage |= nfKUF_DECRYPT;
	}
	if (teeMemcmp(usageStr, nfJWK_USE_PARM_wrap, nfJWK_USE_PARM_wrap_LEN) == 0)
	{
		usage |= nfKUF_WRAP;
		usage |= nfKUF_UNWRAP;
	}
	return usage;
}
#if 0
/*********************************************************************
 * @function caUnwrapJwe
 *
 * @brief Unwraps the JWE to get JWK and imports it
 *
 * @param[in]  clientId  	  	- the TEE client ID
 * @param[in]  unwrapKeyHandle	- handle to the unwrapping key
 * @param[in]  unwrapAlgo		- algorithm to unwrap the JWE
 * @param[in]  keyAlgo			- algorithm of the JWK key
 * @param[in]  keyUsageFlags	- usage flags for the JWK key
 * @param[in]  algLength		- length of algData
 * @param[in]  algData			- base-64 decoded 'alg' data
 * @param[in]  encLength		- length of encData
 * @param[in]  encData			- base-64 decoded 'enc' data
 * @param[in]  cmkLength		- length of cmkData
 * @param[in]  cmkData			- base-64 decoded encrypted content master key data
 * @param[in]  ivLength			- length of ivData
 * @param[in]  ivData			- base-64 decoded initialization vector data
 * @param[in]  macLength		- length of integrity value
 * @param[in]  macData			- base-64 decoded integrity value (a.k.a mac)
 * @param[in]  cipherLength		- length of cipherData
 * @param[in]  cipherData		- base-64 decoded cipher text
 * @param[out] keyHandlePtr	    - pointer to handle of the imported JWK key
 *
 * @returns nfTGR_SUCCESS if successful, nfTCR_MALFORMED_DATA, nfTCR_BAD_PARAMETER,
 *         nfTCR_OPERATION_NOT_ALLOWED, nfTCR_INTERNAL_ERROR otherwise
 *
 *********************************************************************/
nfTeeCryptoResult_t caUnwrapJwe(nfTeeClientId_t 		clientId,
							   nfCryptoKeyHandle_t 		unwrapKeyHandle,
							   nfCryptoAlgorithm_t		unwrapAlgo,
							   nfCryptoAlgorithm_t 		keyAlgo,
							   nfKeyUsageFlagsWord_t	keyUsageFlags,
							   uint32_t	    			algLength,
							   uint8_t	    			*algData,
							   uint32_t	    			encLength,
							   uint8_t	    			*encData,
							   uint32_t	    			cmkLength,
							   uint8_t	    			*cmkData,
							   uint32_t	    			ivLength,
							   uint8_t	    			*ivData,
							   uint32_t	    			macLength,
							   uint8_t	    			*macData,
							   uint32_t	    			cipherLength,
							   uint8_t	    			*cipherData,
							   nfCryptoKeyHandle_t    	*keyHandlePtr)
{
	(void)clientId;	//TODO: this is just to remove compiler warning. Use it or remove from func signature.
	nfTeeCryptoResult_t result = nfTGR_SUCCESS;
	caKeyObject_t* unwrapKeyPtr = NULL;
	unsigned char* decCmkPtr = NULL;
	uint32_t decCmkLen = 0;
	uint32_t expectedDecCmkLen = 0;
	unsigned char* jwkPtr = NULL;

	// make sure right 'enc' type is set for respective algo
	int isEncA128GCM = (teeMemcmp(encData, nfJWE_ENC_PARM_A128GCM, nfJWE_ENC_PARM_A128GCM_LEN) == 0) ? 1: 0;
	int isEncA256GCM = (teeMemcmp(encData, nfJWE_ENC_PARM_A256GCM, nfJWE_ENC_PARM_A256GCM_LEN) == 0) ? 1: 0;

	//TBD: following two checks need not be done here since they are already done in webcrypto.cpp, delete them?
	// For AES key upwrap, 'enc' must be A128GCM (the spec also allows A128CBC+HS256 but we don't support it)
	if (unwrapAlgo == nfCA_AES_KW && isEncA128GCM == 0)
	{
		CRYPTODBG("%s: For AES key unwrap, the JWE Header 'enc' field must be A128GCM.\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	// For RSA-OAEP key unwrap, 'enc' must be A128GCM or A256GCM
	if (unwrapAlgo == nfCA_RSA_OAEP && isEncA128GCM == 0 && isEncA128GCM == 0)
	{
		CRYPTODBG("%s: For RSA_OAEP key unwrap, the JWE Header 'enc' field must be A128GCM or A256GCM.\n", __FUNCTION__);
		return nfTCR_MALFORMED_DATA;
	}

	// get the key length to check the CMK length once it is decrypted
	expectedDecCmkLen = (isEncA128GCM) ? 128/8 : ((isEncA256GCM) ? 256/8 : 0);


	// get the wrapping key
	int index = caGetIndexFromHandle(unwrapKeyHandle);
	if (index != caKeyInvalidIndex)
	{
		unwrapKeyPtr = &(caKeyStore[index]);
	}
	//invalid handle
	else
	{
		CRYPTODBG("%s: invalid handle for unwrap key...\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	// check if key can be used to unwrap
	if (!(unwrapKeyPtr->KeyUsageFlags & nfKUF_UNWRAP))
	{
		CRYPTODBG("%s: key cannot be used for unwrap, returning OP_NOT_ALLOWED...\n", __FUNCTION__ );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	if (unwrapAlgo != unwrapKeyPtr->Algorithm)
    {
        CRYPTODBG("%s: Wrapping key algorithm does not match JWE header algorithm.\n", __FUNCTION__);
        return nfTCR_MALFORMED_DATA;
    }

    if ((unwrapAlgo == nfCA_RSA_OAEP) && (unwrapKeyPtr->KeyContextPtr->ContextType != caKC_RSA))
    {
        CRYPTODBG("%s: RSA Wrapping key not initialized.\n", __FUNCTION__);
        return nfTCR_INTERNAL_ERROR;
    }
    if (unwrapAlgo == nfCA_AES_KW)
    {
    	if (unwrapKeyPtr->KeyContextPtr->ContextType != caKC_RAW)
    	{
			CRYPTODBG("%s: AES Raw Wrapping key not initialized.\n", __FUNCTION__);
			return nfTCR_INTERNAL_ERROR;
    	}
    	int keySize = extractIntFromString(algData);
    	if ( keySize != (int) (unwrapKeyPtr->KeyContextPtr->ctxt.rawContext.KeyDataLength)*8)
    	{
    		CRYPTODBG("%s: AES Raw Wrapping key has wrong key size, expected %d, actual %d\n",
    							__FUNCTION__, keySize, unwrapKeyPtr->KeyContextPtr->ctxt.rawContext.KeyDataLength);
    		return nfTCR_BAD_PARAMETER;
    	}
    }

    // decrypt the CMK
    if (unwrapAlgo == nfCA_RSA_OAEP)
    {
    	result = teePrivateDecrypt (&(unwrapKeyPtr->KeyContextPtr->ctxt.rsaContext),
    					(const unsigned char*) cmkData, cmkLength, PKCS1_OAEP, &decCmkLen, &decCmkPtr);
    }
    else // unwrapAlgo == AES_KW
    {
        // test the input encrypted CMK size against the expected decrypted CMK size,
        // the latter as read from the JWE header 'enc' field. For AES key
        // wrapping specifically, we know that the ciphertext size is always the
        // cleartext size + 8.
        if (cmkLength < (expectedDecCmkLen + 8 ))
        {
        	CRYPTODBG("%s: encrypted CMK seems too short, expected %d, actual %d\n",
        	    							__FUNCTION__, (expectedDecCmkLen + 8), cmkLength);
        	return nfTCR_MALFORMED_DATA;
        }
        nfAesWrapContext_t* aesWrapCtxtPtr;
        result = teeAesWrapInit(unwrapKeyPtr->KeyContextPtr->ctxt.rawContext.KeyData,
								unwrapKeyPtr->KeyContextPtr->ctxt.rawContext.KeyDataLength,
								NULL, &aesWrapCtxtPtr);
        if (result == nfTGR_SUCCESS)
        {
        	result = teeAesUnwrap((const unsigned char*) cmkData, cmkLength, aesWrapCtxtPtr, &decCmkLen, &decCmkPtr);
        	teeAesWrapTerm(aesWrapCtxtPtr);
        }
    }
    // return if not a success
    if (result != nfTGR_SUCCESS) {
    	CRYPTODBG("%s: could nor decrypt CMK, result is %08x\n", __FUNCTION__, result);
        return result; // cipher error
    }

    // verify that decrypted CMK is of proper length
    if (decCmkLen != expectedDecCmkLen)
    {
    	CRYPTODBG("%s: decrypted CMK is not of expected length, expected %d\n", __FUNCTION__, expectedDecCmkLen);
    	teeMemFree(&decCmkPtr);
    	return nfTCR_MALFORMED_DATA;
    }

    // create AES-GCM context for decrypting the cipher
    nfAesCipherContext_t* aesCipherCtxtPtr;
    result = teeAesCipherInit(nfAES_GCM, decCmkPtr, decCmkLen,
    					ivData, ivLength, &aesCipherCtxtPtr);
    teeMemFree(&decCmkPtr);

    /*
     * compute AAD for AES-GCM algorithm which is concatenation of
     * Encoded JWE Header value (JSON string of 'alg' and 'enc'), a period character ('.'),
     * Encoded JWE Encrypted Key (CMK), a second period character ('.'), and
     * Encoded JWE Initialization Vector
     */
    // compute encoded header string
    uint8_t* encHdr;
    uint32_t encHdrLength;
    caCreateJweHeader(algData, algLength, encData, encLength, &encHdr, &encHdrLength);
    //TODO: this is a hack to get it working. somehow base64 encoded header from server does not have '=' in the end
    if(encHdr[encHdrLength-1] == '=')
    {
    	encHdr[encHdrLength-1] = '\0';
    	encHdrLength--;
    }
    // compute size of AAD which is size of header, CMK, IV + 2 for adjunctors
    uint32_t encCmkLength = Base64encode_len(cmkLength) - 1; //since function accounts for '\0' as well
    uint32_t encIvLength = Base64encode_len(ivLength) - 1;
    int aadLength = encHdrLength + encCmkLength + encIvLength + 2;
    // construct the AAD
	unsigned char *aadPtr = (unsigned char*) (teeMemAlloc(aadLength + 1));
	if (aadPtr == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for aad\n", __FUNCTION__ );
		teeMemFree(&encHdr);
		return nfTCR_OUT_OF_MEMORY;
	}
	unsigned char *tmpAadPtr = aadPtr;
	teeMemcpy(tmpAadPtr, encHdr, encHdrLength);
	tmpAadPtr[encHdrLength] = '.';
	tmpAadPtr += encHdrLength + 1;
	encCmkLength = Base64encode_url((char*)(tmpAadPtr), (const char*) cmkData, cmkLength) - 1;
	tmpAadPtr[encCmkLength] = '.';
	tmpAadPtr += encCmkLength + 1;
	encIvLength = Base64encode_url((char*)(tmpAadPtr), (const char*) ivData, ivLength) - 1;
	tmpAadPtr[encIvLength] = '\0';
	AESDBG("%s: AAD length: %d, AAD: %s\n", __FUNCTION__, aadLength, aadPtr);

    // do the decrypt, if successful jwkPtr will contain the JWK
    result = teeAesGcmDecrypt(cipherData, cipherLength,
							macData, macLength,
							aadPtr, aadLength,
							aesCipherCtxtPtr,
							&jwkPtr);
    if (result != nfTGR_SUCCESS)
    {
    	CRYPTODBG("%s: teeAesGcmDecrypt failed, could not decrypt cipher text, result is %08x\n", __FUNCTION__, result);
    	teeMemFree(&encHdr);
    	teeMemFree(&aadPtr);
    	return result;
    }

    // destroy the aescipher context
    teeAesCipherTerm(aesCipherCtxtPtr);
    // free memory
    teeMemFree(&encHdr);
    teeMemFree(&aadPtr);

    // import the JWK and we are done
    nfKeyType_t keyType;
    return caImportKey(clientId, nfKF_JWK, jwkPtr, teeStrlen((const char*)jwkPtr),
    				keyAlgo, keyUsageFlags, keyHandlePtr, &keyType);
}
#endif /* 0 */
