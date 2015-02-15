/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeRsaContext.h"
#include "CryptoTypes.h"
#include "CryptoAgent.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
#include "b64.h"

#include <string.h>
#include <assert.h>

#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#define teeMAX_B64_DECODE_SIZE	1024
static uint8_t b64DecodeBuf[teeMAX_B64_DECODE_SIZE];

nfTeeCryptoResult_t teeRsaCreateCtxt(char  		*nPtr,
								   char 		*ePtr,
								   char			*dPtr,
								   char			*pPtr,
								   char			*qPtr,
								   char			*dpPtr,
								   char			*dqPtr,
								   char			*qiPtr,
								   nfRsaContext_t	*rsaContextPtr )
{
	/*
	 * This is an RSA key -- is it a public key or a private key? Both have
	 * n and e, but only the private key has d, so check for that.
	 *
	 */
	//check if required parameters n is present, return error otherwise
	if (nPtr == NULL)
	{
		RSADBG("%s: nPtr is NULL, returning \n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}

    if ( ( rsaContextPtr->pOsslRsa_ = RSA_new() ) == NULL )
    {
        RSADBG("%s: failed to allocate RSA context\n", __FUNCTION__ );
        return nfTCR_OUT_OF_MEMORY;
    }

    // process n, first, get decode size of n
    int DecodeLen = Base64decode_len((const char *) nPtr );
    if ( DecodeLen >= teeMAX_B64_DECODE_SIZE)
    {
        RSADBG("%s: n is too large (%d)\n", __FUNCTION__, DecodeLen );
        goto ERROR;
    }
    // okay, length is okay, decode
    DecodeLen = Base64decode((char*)b64DecodeBuf, (const char *) nPtr );

    // Get it into the RSA struct
    rsaContextPtr->pOsslRsa_->n = BN_bin2bn(b64DecodeBuf, DecodeLen, NULL);

    // now process d. if d exists then set hasPrivateKey_
    // is d present?
	if ( dPtr != NULL )
	{
		rsaContextPtr->hasPrivateKey_ = 1;
		rsaContextPtr->hasPublicKey_ = 1;


		DecodeLen = Base64decode_len((const char *) dPtr );
		if ( DecodeLen >= teeMAX_B64_DECODE_SIZE)
		{
			RSADBG("%s: d is too large (%d)\n", __FUNCTION__, DecodeLen );
			goto ERROR;
		}
		// okay, length is okay, decode
		DecodeLen = Base64decode((char*)b64DecodeBuf, (const char *) dPtr );

		// Get it into the RSA struct
		rsaContextPtr->pOsslRsa_->d = BN_bin2bn(b64DecodeBuf, DecodeLen, NULL);
		rsaContextPtr->hasPrivateKey_ = 1;
	}

    // is e present? if yes set hasPublicKey_
    if ( ePtr != NULL )
    {
        RSADBG("%s: RSA priv key includes public exponent...\n", __FUNCTION__);
        DecodeLen = Base64decode_len((const char *) ePtr );
        if ( DecodeLen >= teeMAX_B64_DECODE_SIZE)
        {
            RSADBG("%s: e is too large (%d)\n", __FUNCTION__, DecodeLen );
            goto ERROR;
        }
        // okay, length is okay, decode
        DecodeLen = Base64decode((char*)b64DecodeBuf, (const char *) ePtr );

        // Get it into the RSA struct
        rsaContextPtr->pOsslRsa_->e = BN_bin2bn(b64DecodeBuf, DecodeLen, NULL);
        RSADBG("%s: XXX - need to check e for NULL...\n", __FUNCTION__ );
        rsaContextPtr->hasPublicKey_ = 1;
    }

    // is q present?
    if ( qPtr != NULL )
    {
        RSADBG("%s: RSA priv key includes factor q...\n", __FUNCTION__);
        DecodeLen = Base64decode_len((const char *) qPtr );
        if ( DecodeLen >= teeMAX_B64_DECODE_SIZE)
        {
            RSADBG("%s: q is too large (%d)\n", __FUNCTION__, DecodeLen );
            goto ERROR;
        }
        // okay, length is okay, decode
        DecodeLen = Base64decode((char*)b64DecodeBuf, (const char *) qPtr );

        // Get it into the RSA struct
        rsaContextPtr->pOsslRsa_->q = BN_bin2bn(b64DecodeBuf, DecodeLen, NULL);
        RSADBG("%s: XXX - need to check q for NULL...\n", __FUNCTION__ );
    }

    // is p present?
    if ( pPtr != NULL )
    {
        RSADBG("%s: RSA priv key includes factor p...\n", __FUNCTION__);
        DecodeLen = Base64decode_len((const char *) pPtr );
        if ( DecodeLen >= teeMAX_B64_DECODE_SIZE)
        {
            RSADBG("%s: p is too large (%d)\n", __FUNCTION__, DecodeLen );
            goto ERROR;
        }
        // okay, length is okay, decode
        DecodeLen = Base64decode((char*)b64DecodeBuf, (const char *) pPtr );

        // Get it into the RSA struct
        rsaContextPtr->pOsslRsa_->p = BN_bin2bn(b64DecodeBuf, DecodeLen, NULL);
        RSADBG("%s: XXX - need to check p for NULL...\n", __FUNCTION__ );
    }

    // is dp present?
	if ( dpPtr != NULL )
	{
		RSADBG("%s: XXX - dp present, need to add code to handle it\n", __FUNCTION__);
	}
	// is dq present?
	if ( dqPtr != NULL )
	{
		RSADBG("%s: XXX - dq present, need to add code to handle it\n", __FUNCTION__);
	}
	// is qi present?
	if ( qiPtr != NULL )
	{
		RSADBG("%s: XXX - qi present, need to add code to handle it\n", __FUNCTION__);
	}
	return nfTGR_SUCCESS;

// NOTREACHED
ERROR:
    RSADBG("%s: XXX - FIX MEMORY LEAK!!! - XXX\n", __FUNCTION__ );
    return nfTCR_INTERNAL_ERROR; // TBD: fix this.

}/* end teeRsaCreateCtxt */

nfTeeCryptoResult_t  teePrivateDecrypt(nfRsaContext_t		*rsaContextPtr,
									  const unsigned char	*encStr,
									  uint32_t				encStrLen,
									  Padding 				padding,
									  uint32_t				*decStrLen,
									  unsigned char			**decStrPtr)
{
    if (!rsaContextPtr->hasPrivateKey_) {
        RSADBG("%s: RSA context does not have private key, cannot perform private decrypt operation\n", __FUNCTION__ );
        return nfTCR_OPERATION_NOT_ALLOWED;
    }
    int osslPadding = RSA_NO_PADDING;
    switch (padding)
    {
        case NONE:          osslPadding = RSA_NO_PADDING;           break;
        case PKCS1:         osslPadding = RSA_PKCS1_PADDING;        break;
        case PKCS1_OAEP:    osslPadding = RSA_PKCS1_OAEP_PADDING;   break;
        default:            assert(0);                          break;
    }
    const int rsaSize = RSA_size(rsaContextPtr->pOsslRsa_);
    if (rsaSize <= 0) {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    *decStrPtr = (unsigned char*) teeMemAlloc((uint32_t)rsaSize);
    if (*decStrPtr == NULL) {
    	RSADBG("%s: Out of memory, cannot allocate for decStrPtr\n", __FUNCTION__ );
    	return nfTCR_OUT_OF_MEMORY;
    }
    int res = RSA_blinding_on(rsaContextPtr->pOsslRsa_, NULL); // blinding for private key ops only
    if (res != 1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    res = RSA_private_decrypt(encStrLen, encStr, *decStrPtr, rsaContextPtr->pOsslRsa_, osslPadding);
    RSA_blinding_off(rsaContextPtr->pOsslRsa_);
    if (res == -1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }

    *decStrLen = res;
    *decStrPtr = teeMemRealloc(*decStrPtr, *decStrLen);
    if (*decStrPtr == NULL) {
    	*decStrLen = 0;
        return nfTCR_OUT_OF_MEMORY;
    }
    return nfTGR_SUCCESS;
} // end teeprivateDecrypt

nfTeeCryptoResult_t teeRsaGetPublicRaw(nfRsaContext_t	*rsaContextPtr,
									 unsigned char	**encPubMod,
									 uint32_t		*modLen,
									 unsigned char	**encPubExp,
									 uint32_t 		*expLen)
{
    if(!rsaContextPtr->hasPublicKey_) {
    	RSADBG("%s: RSA context does not have public key to export\n", __FUNCTION__ );
        return nfTCR_OPERATION_NOT_ALLOWED;
    }

    int decLen = BN_bn2bin(rsaContextPtr->pOsslRsa_->n, b64DecodeBuf);
    *modLen = Base64encode_len(decLen);
    *encPubMod = (unsigned char*) teeMemAlloc(*modLen);
    if ((*encPubMod) == NULL)
    {
    	RSADBG("%s: cannot allocate memory for base-64 encoded public modulus (n)\n", __FUNCTION__ );
    	return nfTCR_OUT_OF_MEMORY;
    }
    *modLen = Base64encode((char*) (*encPubMod), (const char*)b64DecodeBuf, decLen);

    decLen = BN_bn2bin(rsaContextPtr->pOsslRsa_->e, b64DecodeBuf);
    *expLen = Base64encode_len(decLen);
    *encPubExp = (unsigned char*) teeMemAlloc(*expLen);
    if ((*encPubExp) == NULL)
    {
    	RSADBG("%s: cannot allocate memory for base-64 encoded public exponent (e)\n", __FUNCTION__ );
    	teeMemFree((uint8_t**)encPubMod);
    	return nfTCR_OUT_OF_MEMORY;
    }
    *expLen = Base64encode((char*) (*encPubExp), (const char*)b64DecodeBuf, decLen);

	return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t teeRsaSha256Verify(nfRsaContext_t		*rsaContextPtr,
									   uint32_t 			dataLen,
									   const unsigned char  *data,
									   uint32_t 			shaLen,
									   unsigned char		*sha)
{
    if (!rsaContextPtr->hasPublicKey_) {
    	RSADBG("%s: RSA context does not have public key to verify with\n", __FUNCTION__ );
    	return nfTCR_OPERATION_NOT_ALLOWED;
    }

    // first need to calculate the sha on the input data
    const EVP_MD *evpMdPtr = EVP_sha256();
    EVP_MD_CTX evpMdCtx;
    EVP_MD_CTX_init(&evpMdCtx);

    //init
    if(!(EVP_DigestInit_ex(&evpMdCtx, evpMdPtr, NULL))) {
    	RSADBG("%s: Exception!! error in EVP_DigestInit_ex()\n", __FUNCTION__ );
    	return nfTCR_INTERNAL_ERROR;
    }

	//update
    if(!(EVP_DigestUpdate(&evpMdCtx, data, dataLen))) {
    	RSADBG("%s: Exception!! error in EVP_DigestUpdate()\n", __FUNCTION__ );
    	return nfTCR_INTERNAL_ERROR;
   }

    //final
    unsigned char computedSha[EVP_MAX_MD_SIZE];
    unsigned int computedShaLen = 0;
    if(!(EVP_DigestFinal_ex(&evpMdCtx, computedSha, &computedShaLen))) {
    	RSADBG("%s: Exception!! error in EVP_DigestFinal_ex()\n", __FUNCTION__ );
    	return nfTCR_INTERNAL_ERROR;
    }

    EVP_MD_CTX_cleanup(&evpMdCtx);

    // now verify the computed sha against expected one
    if (RSA_blinding_on(rsaContextPtr->pOsslRsa_, NULL) != 1)
    {
    	RSADBG("%s: Cannot turn blinding on\n", __FUNCTION__ );
        return nfTCR_INTERNAL_ERROR;
    }
    // OSX RAS_verify needs the sigbuf to not be const but promises not to modify it
    int ret = RSA_verify(NID_sha256, computedSha, computedShaLen,
                         sha, shaLen, rsaContextPtr->pOsslRsa_);
    RSA_blinding_off(rsaContextPtr->pOsslRsa_);
    if (ret != 1)
    {
    	RSADBG("%s: RSA verify did not pass, returning error\n", __FUNCTION__ );
        return nfTCR_INTERNAL_ERROR;
    }

    return nfTGR_SUCCESS;
}

#if 0
nfTeeCryptoResult_t getRsaContext(rsa_context** context)
{
    if (context == NULL)
        return nfTCR_BAD_PARAMETER;

    *context = malloc(sizeof(rsa_context));

    if (*context == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }

    (*context)->hasPrivateKey_ = 0;
    (*context)->hasPublicKey_ = 0;
    (*context)->pOsslRsa_ = NULL;

    return nfTGR_SUCCESS;
}

void freeRsaContext(rsa_context** context)
{
    if ((context == NULL) || (*context == NULL))
        return;

    if((*context)->pOsslRsa_ != NULL) {
        RSA_free((*context)->pOsslRsa_);
    }

    free(*context);
}

nfTeeCryptoResult_t setPublicSpki(rsa_context* context, const unsigned char** pubKeySpkiDer)
{
    if (context == NULL) {
        printf("invalid args");
        return nfTCR_BAD_PARAMETER;
    }

    RSA *rsa = d2i_RSA_PUBKEY(NULL, pubKeySpkiDer, strlen((const char*)*pubKeySpkiDer));
    if (rsa == NULL)
        return nfTCR_OUT_OF_MEMORY;

    if (context->pOsslRsa_)
        RSA_free(context->pOsslRsa_);

    context->pOsslRsa_ = rsa;
    context->hasPublicKey_ = 1;
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t setPrivatePkcs8(rsa_context* context, const unsigned char* pkcs8)
{
    if (context == NULL) {
        return nfTCR_BAD_PARAMETER;
    }

    // OpenSSL does not make it easy to import a private key in PKCS#8 format.
    // Must go through some monkey-motions.

    // make a mem BIO pointing to the incoming PKCS#8 data

    // TODO: Think about how to do error handling and along with freeing all open SSL handles.
    //       It was done using ScopedOpenSSL class.


    BIO* bio = BIO_new_mem_buf((void *)(pkcs8), strlen((const char*)pkcs8));
    if (!bio) {
        printf("BIO_new_mem_buf() failed");
        return nfTCR_OUT_OF_MEMORY;
    }

    PKCS8_PRIV_KEY_INFO* p8inf = d2i_PKCS8_PRIV_KEY_INFO_bio(bio, NULL);
    if (!p8inf) {
        BIO_free_all(bio);
        return nfTCR_OUT_OF_MEMORY;
    }

    EVP_PKEY* pkey = EVP_PKCS82PKEY(p8inf);
    if (!pkey) {
        PKCS8_PRIV_KEY_INFO_free(p8inf);
        BIO_free_all(bio);
        return nfTCR_OUT_OF_MEMORY;
    }

    RSA* const rsa = EVP_PKEY_get1_RSA(pkey);
    if (!rsa) {
        EVP_PKEY_free(pkey);
        PKCS8_PRIV_KEY_INFO_free(p8inf);
        BIO_free_all(bio);
        return nfTCR_OUT_OF_MEMORY;
    }

    context->pOsslRsa_ = rsa;
    context->hasPrivateKey_ = 1;
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t generate(rsa_context* context, unsigned int nBits, unsigned long long int publicExponent)
{
    if (context == NULL) {
        return nfTCR_BAD_PARAMETER;
    }

    // make sure publicExponent is odd
    publicExponent |= 1;

    // this method clobbers any existing context
    if (context->pOsslRsa_)
        RSA_free(context->pOsslRsa_);

    // The OpenSSL spec says the publicExponent must be odd. Yung found that
    // RSA_generate_key hangs if this is not true, so we better make it true.
    // Typical values are 3, 17 or 65537, with 65537 the most common.
    publicExponent |= (unsigned long long int)1;


    int keygenSuccess = 0;
    unsigned int retryCount = 0;
    const unsigned int MAX_RETRIES=4;
    while (!keygenSuccess && (retryCount < MAX_RETRIES))
    {
        context->pOsslRsa_ = RSA_generate_key(nBits, publicExponent, 0, 0);
        if (context->pOsslRsa_)
            keygenSuccess = (RSA_check_key(context->pOsslRsa_) == 1);
        if (!keygenSuccess && context->pOsslRsa_)
            RSA_free(context->pOsslRsa_);
        retryCount++;
    }

    if (keygenSuccess)
    {
        context->hasPrivateKey_ = 1;
        context->hasPublicKey_  = 1;
        return nfTGR_SUCCESS;
    }

    // TODO: Need a better error code
    return nfTCR_INTERNAL_ERROR;
}

nfTeeCryptoResult_t setPublicPkcs1(rsa_context* context, const unsigned char** pubKeyDer)
{
    RSA *rsa = d2i_RSAPublicKey(NULL, pubKeyDer, strlen((const char*)*pubKeyDer));
    if (rsa == NULL) {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    if (context->pOsslRsa_)
        RSA_free(context->pOsslRsa_);
    context->pOsslRsa_ = rsa;
    context->hasPublicKey_ = 1;
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t getPublicPkcs1(rsa_context* context, /*out*/unsigned char** pubKeyPkcs1Der)
{
    if(!context->hasPublicKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }

    int keyLen = i2d_RSAPublicKey(context->pOsslRsa_, NULL);
    *pubKeyPkcs1Der = realloc(*pubKeyPkcs1Der, keyLen);
    if (*pubKeyPkcs1Der == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    i2d_RSAPrivateKey(context->pOsslRsa_, pubKeyPkcs1Der);
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t getPublicSpki(rsa_context* context, /*out*/unsigned char** pubKeySpkiDer)
{
    if (!context->hasPublicKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }
    int keyLen = i2d_RSA_PUBKEY(context->pOsslRsa_, NULL);
    *pubKeySpkiDer = realloc(*pubKeySpkiDer, keyLen);
    if (*pubKeySpkiDer == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    i2d_RSA_PUBKEY(context->pOsslRsa_, pubKeySpkiDer);
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t setPrivatePkcs1(rsa_context* context, const unsigned char** privKeyPkcs1Der)
{
    RSA *rsa = d2i_RSAPrivateKey(NULL, privKeyPkcs1Der, strlen(*privKeyPkcs1Der));
    if (rsa == NULL) {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    if (context->pOsslRsa_)
        RSA_free(context->pOsslRsa_);
    context->pOsslRsa_ = rsa;
    context->hasPrivateKey_ = 1;
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t getPrivatePkcs1(rsa_context* context, /*out*/unsigned char** privKeyPkcs1Der)
{
    if (!context->hasPrivateKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }
    int keyLen = i2d_RSAPrivateKey(context->pOsslRsa_, NULL);
    *privKeyPkcs1Der = realloc(*privKeyPkcs1Der, keyLen);
    if (*privKeyPkcs1Der == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    i2d_RSAPrivateKey(context->pOsslRsa_, privKeyPkcs1Der);

    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t getPrivatePkcs8(rsa_context* context, /*out*/unsigned char** pkcs8)
{
    if (!context->hasPrivateKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }

    EVP_PKEY* pkey = EVP_PKEY_new();
    if (pkey == 0) {
        return nfTCR_OUT_OF_MEMORY;
    }

    int ret = EVP_PKEY_set1_RSA(pkey, context->pOsslRsa_);
    if (!ret)
    {
        EVP_PKEY_free(pkey);
        return nfTCR_OUT_OF_MEMORY;
    }

    PKCS8_PRIV_KEY_INFO* p8inf = EVP_PKEY2PKCS8(pkey);
    if (p8inf == 0)
    {
        EVP_PKEY_free(pkey);
        return nfTCR_OUT_OF_MEMORY;
    }

    int outLen = i2d_PKCS8_PRIV_KEY_INFO(p8inf, NULL);
    if (outLen <= 0)
    {
        EVP_PKEY_free(pkey);
        PKCS8_PRIV_KEY_INFO_free(p8inf);
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }

    *pkcs8 = realloc(*pkcs8, outLen);
    if (pkcs8 == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }

    ret = i2d_PKCS8_PRIV_KEY_INFO(p8inf, pkcs8);
    if (!ret)
    {

        EVP_PKEY_free(pkey);
        PKCS8_PRIV_KEY_INFO_free(p8inf);
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t publicEncrypt(rsa_context* context, const unsigned char* in, /*out*/unsigned char** out, Padding padding)
{
    if (!context->hasPublicKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }
    int osslPadding = RSA_NO_PADDING;
    switch (padding)
    {
        case NONE:          osslPadding = RSA_NO_PADDING;           break;
        case PKCS1:         osslPadding = RSA_PKCS1_PADDING;        break;
        case PKCS1_OAEP:    osslPadding = RSA_PKCS1_OAEP_PADDING;   break;
        default:            assert(0);                              break;
    }
    const unsigned int rsaSize = RSA_size(context->pOsslRsa_);
    if (rsaSize <= 0) {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    if ((strlen(in)) > (rsaSize - 11))
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    *out = realloc(*out, rsaSize);
    if (*out == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    int res = RSA_public_encrypt(strlen(in), in, *out,
            context->pOsslRsa_, osslPadding);
    if (res == -1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }

    *out = realloc(*out, res);
    if (*out == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    return nfTGR_SUCCESS;
}

unsigned char* computeDigest(const unsigned char* inVuc, NfShaAlgo shaAlgo)
{
    // TODO: Need DigestAlgo and Digester here.
    return NULL;
}

nfTeeCryptoResult_t privateSign(rsa_context* context, const unsigned char* data, NfShaAlgo shaAlgo,
                 /*out*/unsigned char** sig)
{
    if (!context->hasPrivateKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }

    unsigned char* hashVuc = computeDigest(data, shaAlgo);

    const int rsaSize = RSA_size(context->pOsslRsa_);

    if (rsaSize <= 0) {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }

    *sig = realloc(*sig, rsaSize);
    if (*sig == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }

    // use blinding for all private key operations
    if (RSA_blinding_on(context->pOsslRsa_, NULL) != 1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    unsigned int outLen;
    // TODO: Implement nidSha
    int res = RSA_sign(nidSha(shaAlgo), &hashVuc[0], strlen(hashVuc), *sig,
                       &outLen, context->pOsslRsa_);
    RSA_blinding_off(context->pOsslRsa_);
    if (res != 1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }

    // size and return the signature
    *sig = realloc(*sig, outLen);
    if (*sig == NULL) {
        return nfTCR_OUT_OF_MEMORY;
    }
    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t publicVerify(rsa_context* context, const unsigned char* data, NfShaAlgo shaAlgo, unsigned char** sig)
{
    if (!context->hasPublicKey_) {
        // TODO: need a better error code maybe?
        return nfTCR_OPERATION_NOT_ALLOWED;
    }

    // first need to calculate the hash on the input data, using the specified ShaAlgo
    unsigned char* hashVuc = computeDigest(data, shaAlgo);

    // now verify the computed hash
    if (RSA_blinding_on(context->pOsslRsa_, NULL) != 1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    // OSX RAS_verify needs the sigbuf to not be const but promises not to modify it
    int res = RSA_verify(nidSha(shaAlgo), &hashVuc[0], strlen(hashVuc),
                         *sig, strlen(*sig), context->pOsslRsa_);
    RSA_blinding_off(context->pOsslRsa_);
    if (res != 1)
    {
        // TODO: Need a better error code
        return nfTCR_INTERNAL_ERROR;
    }
    return nfTGR_SUCCESS;
}
#endif


