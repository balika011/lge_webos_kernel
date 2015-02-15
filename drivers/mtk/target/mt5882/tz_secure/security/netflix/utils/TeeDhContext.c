/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeDhContext.h"
#include <string.h>

#include <openssl/dh.h>
#include <openssl/bn.h>
// Holds the OpenSSL DH context
struct dh_context_st {
    DH* pOsslDh;
    unsigned char* sharedSecret;
};

//TODO: deal with unsigned char* warning for strlen. check with Wess why we require these strings to unsigned in first place

nfTeeCryptoResult_t init(unsigned char* const p, unsigned char* const g,  /* out */ dh_context** dhContext)
{
    if (dhContext == NULL) {
        return nfTCR_BAD_PARAMETER;
    }

    *dhContext = malloc(sizeof(dh_context));
    if (*dhContext == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }

    (*dhContext)->sharedSecret = NULL;

    // create a new DH
    (*dhContext)->pOsslDh = DH_new();
    if ((*dhContext)->pOsslDh == NULL)
    {
    	printf("%s: cannot create dhContext->pOsslDh", __FUNCTION__);
    	return nfTCR_OUT_OF_MEMORY;
    }

    // ensure p is 1024 bits - this is a requirement to get a 128-bit key for AES
    if (strlen((char*)p) != 128)
    {
    	printf("%s: p is not 1024 bits in size", __FUNCTION__);
    	DH_free((*dhContext)->pOsslDh);
        return nfTCR_BAD_PARAMETER;
    }

    // DH_new creates a DH with p & g set to NULL; copy in our new values
    ((*dhContext)->pOsslDh)->p = BN_bin2bn(p, strlen((char*)p), NULL);
    if (((*dhContext)->pOsslDh)->p == NULL)
    {
    	printf("%s: Unable to duplicate DH prime using BN_dup()", __FUNCTION__);
    	DH_free((*dhContext)->pOsslDh);
    	// TODO: better error code.
    	return nfTCR_INTERNAL_ERROR;
    }
    ((*dhContext)->pOsslDh)->g = BN_bin2bn(g, strlen((char*)g), NULL);
    if (((*dhContext)->pOsslDh)->g == NULL)
    {
    	printf("%s: Unable to duplicate DH generator using BN_dup()", __FUNCTION__);
    	DH_free((*dhContext)->pOsslDh);
    	// TODO: better error code.
        return nfTCR_INTERNAL_ERROR;
    }

    // check DH
    // NOTE: DH_check() does not seem to work if you set your own p and g on a
    // new DH context. My guess is that it is only appropriate after calling
    // DH_generate parameters(), which we are not doing here.
    //    int errorBitMask = 0;
    //    if (!DH_check(dh_.get(), &errorBitMask))
    //    {
    //        DLOG() << "DiffieHellmanContext::init: DH_check failed\n";
    //        return false;
    //    }
    //    if (errorBitMask)
    //    {
    //        DLOG() << "DiffieHellmanContext::init: Parameters are not suitable: ";
    //        if (errorBitMask & DH_CHECK_P_NOT_PRIME)
    //            DLOG() << "DH_CHECK_P_NOT_PRIME ";
    //        if (errorBitMask & DH_CHECK_P_NOT_SAFE_PRIME)
    //            DLOG() << "DH_CHECK_P_NOT_SAFE_PRIME ";
    //        if (errorBitMask & DH_UNABLE_TO_CHECK_GENERATOR)
    //            DLOG() << "DH_UNABLE_TO_CHECK_GENERATOR ";
    //        if (errorBitMask & DH_NOT_SUITABLE_GENERATOR)
    //            DLOG() << "DH_NOT_SUITABLE_GENERATOR ";
    //        DLOG() << "\n";
    //        return false;
    //    }

    // generate the pub/priv key pair
    if (!DH_generate_key((*dhContext)->pOsslDh))
    {
    	printf("%s: Could not generate key using DH_generate_key", __FUNCTION__);
    	// TODO: better error code.
    	return nfTCR_INTERNAL_ERROR;
    }

    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t terminate(dh_context** dhContext)
{
    if ((dhContext == NULL) || (*dhContext == NULL)) {
        return nfTGR_SUCCESS;
    }

    if ((*dhContext)->pOsslDh != NULL) {
        DH_free((*dhContext)->pOsslDh);
    }

    if ((*dhContext)->sharedSecret == NULL) {
        free((*dhContext)->sharedSecret);
    }

    free(*dhContext);

    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t computeSharedSecret(dh_context* const dhContext, const unsigned char* peerPubKey)
{
    // make sure we have a pub/priv key
    if(((dhContext->pOsslDh)->pub_key == NULL) || ((dhContext->pOsslDh)->priv_key == NULL))
    {
        printf("%s: missing public or private key in DH", __FUNCTION__);
        return nfTCR_BAD_PARAMETER;
    }

    if ((peerPubKey ==  NULL) || (strlen((char*)peerPubKey) == 0))
    {
        printf("%s: peer public key is invalid", __FUNCTION__);
        return nfTCR_BAD_PARAMETER;
    }

    // get size needed for shared secret
    int ssLen = DH_size(dhContext->pOsslDh);
    if(ssLen != 128)
    {
        printf("%s: shared secret (DH_size) not 128 bytes", __FUNCTION__);
        return nfTCR_INTERNAL_ERROR;
    }

    // allocate and zero space for the shared secret
    dhContext->sharedSecret = malloc(129*sizeof(char));
    if (dhContext->sharedSecret == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    memset(dhContext->sharedSecret, '\0', 129);

    // compute the shared secret
    ssLen = DH_compute_key(dhContext->sharedSecret, BN_bin2bn(peerPubKey, strlen((char*)peerPubKey), NULL), dhContext->pOsslDh);
    if(ssLen == -1 || ssLen > 128)
    {
        printf("%s: error computing shared secret", __FUNCTION__);
        return nfTCR_INTERNAL_ERROR;
    }

    // The computed shared secret may be less than 128 bytes so resize it.
    dhContext->sharedSecret = realloc (dhContext->sharedSecret, (ssLen+1));

    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t getPubKey(dh_context* const dhContext, /*out */ unsigned char** pubKey)
{
	*pubKey = malloc (BN_num_bytes((dhContext->pOsslDh)->pub_key));
	if (*pubKey == NULL) {
	    return nfTCR_OUT_OF_MEMORY;
	}
	BN_bn2bin((dhContext->pOsslDh)->pub_key, *pubKey);
	return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t getPrivKey(dh_context* const dhContext, /*out */ unsigned char** privKey)
{
	*privKey = malloc (BN_num_bytes((dhContext->pOsslDh)->priv_key));
	if (*privKey == NULL) {
	    return nfTCR_OUT_OF_MEMORY;
	}
	BN_bn2bin((dhContext->pOsslDh)->priv_key, *privKey);
	return nfTGR_SUCCESS;
}
