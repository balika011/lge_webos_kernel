/**
 * PKCS #8 (Private-key information syntax)
 * Copyright (c) 2006-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "asn1.h"
#include "bignum.h"
#include "rsapkcs.h"
#include "pkcs8.h"


struct crypto_rsa_key * pkcs8_key_import(const u8 *buf, size_t len)
{
	struct asn1_hdr hdr;
	const u8 *pos, *end;
	mp_int *zero;
	struct asn1_oid oid;
	char obuf[80];

	DMSG(" enter,len=%d \n",len);
	
	/** PKCS #8, Chapter 6 */
	DMSG(" enter 1\n");
	
	/** PrivateKeyInfo ::= SEQUENCE */
	if (asn1_get_next(buf, len, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL ||
	    hdr.tag != ASN1_TAG_SEQUENCE) {
		DMSG("PKCS #8: Does not start with PKCS #8 "
			   "header (SEQUENCE); assume PKCS #8 not used \n");
		return NULL;
	}
	pos = hdr.payload;
	end = pos + hdr.length;

	DMSG(" enter 2\n");
	/** version Version (Version ::= INTEGER) */
	if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL || hdr.tag != ASN1_TAG_INTEGER) {
		DMSG("PKCS #8: Expected INTEGER - found "
			   "class %d tag 0x%x; assume PKCS #8 not used \n",
			   hdr.tclass, hdr.tag);
		return NULL;
	}

	DMSG(" enter 3\n");
	zero = bignum_init();
	if (zero == NULL)
	{	
		DMSG("PKCS #8: bignum_init fail \n");
		return NULL;
	}
	//dprintf("enter 4:zero->used=%d,zero->alloc=%d,zero->sign=%d \n", zero->used,zero->alloc,zero->sign);
	//dprintf("enter 4:zero->dp=0x%x \n",zero->dp);
	DMSG(" enter 4, hdr.length=%d \n",hdr.length);
	if (bignum_set_unsigned_bin(zero, hdr.payload, hdr.length) < 0) {
		DMSG("PKCS #8: Failed to parse INTEGER \n");
		bignum_deinit(zero);
		return NULL;
	}
	pos = hdr.payload + hdr.length;
	
	DMSG(" enter 5\n");
	if (bignum_cmp_d(zero, 0) != 0) {
		DMSG("PKCS #8: Expected zero INTEGER in the "
			   "beginning of private key; not found; assume "
			   "PKCS #8 not used \n");
		bignum_deinit(zero);
		return NULL;
	}

	DMSG(" enter 6\n");
	bignum_deinit(zero);

	/** privateKeyAlgorithm PrivateKeyAlgorithmIdentifier
	 * (PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier) */
	 DMSG(" enter 7\n");
	if (asn1_get_next(pos, len, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL ||
	    hdr.tag != ASN1_TAG_SEQUENCE) {
		DMSG("PKCS #8: Expected SEQUENCE "
			   "(AlgorithmIdentifier) - found class %d tag 0x%x; "
			   "assume PKCS #8 not used \n",
			   hdr.tclass, hdr.tag);
		return NULL;
	}

	DMSG(" enter 8\n");
	if (asn1_get_oid(hdr.payload, hdr.length, &oid, &pos)) {
		DMSG( "PKCS #8: Failed to parse OID "
			   "(algorithm); assume PKCS #8 not used \n");
		return NULL;
	}

	DMSG(" enter 9\n");
	//asn1_oid_to_str(&oid, obuf, sizeof(obuf));
	//DMSG("PKCS #8: algorithm=%s \n", obuf);

	if (oid.len != 7 ||
	    oid.oid[0] != 1 /** iso */ ||
	    oid.oid[1] != 2 /** member-body */ ||
	    oid.oid[2] != 840 /** us */ ||
	    oid.oid[3] != 113549 /** rsadsi */ ||
	    oid.oid[4] != 1 /** pkcs */ ||
	    oid.oid[5] != 1 /** pkcs-1 */ ||
	    oid.oid[6] != 1 /** rsaEncryption */) {
		DMSG("PKCS #8: Unsupported private key \n ");
		return NULL;
	}

	pos = hdr.payload + hdr.length;

	DMSG(" enter 10\n");
	/** privateKey PrivateKey (PrivateKey ::= OCTET STRING) */
	if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL ||
	    hdr.tag != ASN1_TAG_OCTETSTRING) {
		DMSG("PKCS #8: Expected OCTETSTRING "
			   "(privateKey) - found class %d tag 0x%x \n",
			   hdr.tclass, hdr.tag);
		return NULL;
	}
	DMSG("PKCS #8: Try to parse RSAPrivateKey \n");

	return (struct crypto_rsa_key *)
		crypto_rsa_import_private_key(hdr.payload, hdr.length);
}

#if 0
struct crypto_private_key *
pkcs8_enc_key_import(const u8 *buf, size_t len, const char *passwd)
{
	struct asn1_hdr hdr;
	const u8 *pos, *end, *enc_alg;
	size_t enc_alg_len;
	u8 *data;
	size_t data_len;

	if (passwd == NULL)
		return NULL;

	/**
	 * PKCS #8, Chapter 7
	 * EncryptedPrivateKeyInfo ::= SEQUENCE {
	 *   encryptionAlgorithm EncryptionAlgorithmIdentifier,
	 *   encryptedData EncryptedData }
	 * EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
	 * EncryptedData ::= OCTET STRING
	 */

	if (asn1_get_next(buf, len, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL ||
	    hdr.tag != ASN1_TAG_SEQUENCE) {
		printf("PKCS #8: Does not start with PKCS #8 "
			   "header (SEQUENCE); assume encrypted PKCS #8 not "
			   "used");
		return NULL;
	}
	pos = hdr.payload;
	end = pos + hdr.length;

	/** encryptionAlgorithm EncryptionAlgorithmIdentifier */
	if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL ||
	    hdr.tag != ASN1_TAG_SEQUENCE) {
		printf( "PKCS #8: Expected SEQUENCE "
			   "(AlgorithmIdentifier) - found class %d tag 0x%x; "
			   "assume encrypted PKCS #8 not used",
			   hdr.tclass, hdr.tag);
		return NULL;
	}
	enc_alg = hdr.payload;
	enc_alg_len = hdr.length;
	pos = hdr.payload + hdr.length;

	/** encryptedData EncryptedData */
	if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
	    hdr.tclass != ASN1_CLASS_UNIVERSAL ||
	    hdr.tag != ASN1_TAG_OCTETSTRING) {
		printf( "PKCS #8: Expected OCTETSTRING "
			   "(encryptedData) - found class %d tag 0x%x",
			   hdr.tclass, hdr.tag);
		return NULL;
	}

	data = pkcs5_decrypt(enc_alg, enc_alg_len, hdr.payload, hdr.length,
			     passwd, &data_len);
	if (data) {
		struct crypto_private_key *key;
		key = pkcs8_key_import(data, data_len);
		free(data);
		return key;
	}

	return NULL;
}

#endif