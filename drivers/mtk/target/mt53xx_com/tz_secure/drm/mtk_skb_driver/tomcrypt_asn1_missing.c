/*****************************************************************
|
|   Tomcrypt - pkcs#8 missing from lib
|
|   $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/mtk_skb_driver/tomcrypt_asn1_missing.c#1 $
|   Original author:  Adam Miller
|
|   This software is provided to you pursuant to your agreement 
|   with Intertrust Technologies Corporation ("Intertrust"). 
|   This software may be used only in accordance with the terms 
|   of the agreement.
|      
|   Copyright (c) 2003-2011 by Intertrust. All rights reserved. 
|
****************************************************************/

/*----------------------------------------------------------------------
|    includes
+---------------------------------------------------------------------*/
#include "tomcrypt_asn1_missing.h"

/*----------------------------------------------------------------------
|    constants
+---------------------------------------------------------------------*/
static const long LTC_RSA_ENC_OID[] = {1, 2, 840, 113549, 1, 1, 1};  

/*----------------------------------------------------------------------
|   parse_pkcs8_decoded_list 
+---------------------------------------------------------------------*/
/**
* examines a ltc_asn1_list (previously decoded from ASN.1) and returns
* pointers to various known subsections. also checks version number.
* if an item is an invalid type, an error is returned.
*/
static int
parse_pkcs8_decoded_list(const ltc_asn1_list* decoded_list,
                         ltc_asn1_list**      list_alg_id,
                         ltc_asn1_list**      list_private_key)
{
    ltc_asn1_list* lst;
    unsigned long  version;

    /* decoded_list should have a SEQ as a first child */
    if (decoded_list->type != LTC_ASN1_SEQUENCE) {
        return CRYPT_PK_INVALID_TYPE;
    }

    /* 1) first element is the VERSION, which is an INTEGER */
    lst = decoded_list->child; /* point to first element of top-level sequence */
    if (lst->type != LTC_ASN1_INTEGER) {
        return CRYPT_PK_INVALID_TYPE;
    }

    /* the only VERSION we support is 0 */
    version = mp_get_int(lst->data);
    if (version != 0) {
        return CRYPT_PK_INVALID_TYPE;
    }

    /* 2) algorithm identifier */
    *list_alg_id = lst->next;
    if ((*list_alg_id)->type != LTC_ASN1_SEQUENCE) {
        return CRYPT_PK_INVALID_TYPE;
    }
    
    /* 3) privateKey */
    *list_private_key = (*list_alg_id)->next;
    if ((*list_private_key)->type != LTC_ASN1_OCTET_STRING) {
        return CRYPT_PK_INVALID_TYPE;
    }

    /* 4) attributes*/
    /* TODO */

    return CRYPT_OK;
}

/*----------------------------------------------------------------------
|    rsa_import_from_pkcs8
+---------------------------------------------------------------------*/
/*
we are decoding the following ASN.1 sequence:

PrivateKeyInfo ::= SEQUENCE {
  version Version,
  privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
  privateKey PrivateKey,
  attributes [0] IMPLICIT Attributes OPTIONAL  }
Version ::= INTEGER
PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
PrivateKey ::= OCTET STRING
Attributes ::= SET OF Attribute

AlgorithmIdentifier  ::=  SEQUENCE  {
  algorithm               OBJECT IDENTIFIER,
  parameters              ANY DEFINED BY algorithm OPTIONAL  }
*/
int
rsa_import_from_pkcs8(const unsigned char* in, 
                      unsigned long        inlen, 
                      rsa_key*             key)
{
    int            err;
    unsigned long  decode_len;
    ltc_asn1_list* decoded_list;
    ltc_asn1_list* list_alg_id;
    ltc_asn1_list* list_alg_oid;
    ltc_asn1_list* list_private_key;
    unsigned long  i;

    /* decode with flexi */
    decode_len = inlen;
    if ((err = der_decode_sequence_flexi(in, &decode_len, &decoded_list)) != CRYPT_OK) {
        return CRYPT_PK_INVALID_TYPE;
    }
    
    /* parse it into subsections */
    if ((err = parse_pkcs8_decoded_list(decoded_list, &list_alg_id, &list_private_key)) != CRYPT_OK)
        goto LBL_ERR;

    /* ensure algorithm is RSA */
    list_alg_oid = list_alg_id->child;
    if (list_alg_oid == NULL) {
        err = CRYPT_PK_INVALID_TYPE;
        goto LBL_ERR;
    }
    if (list_alg_oid->type != LTC_ASN1_OBJECT_IDENTIFIER) {
        err = CRYPT_PK_INVALID_TYPE;
        goto LBL_ERR;
    }
    /* compare the OID to what we know RSA should be */
    if (list_alg_oid->size != (sizeof(LTC_RSA_ENC_OID) / sizeof(long))) {
        err = CRYPT_PK_INVALID_TYPE;
        goto LBL_ERR;
    }
    for (i = 0; i < list_alg_oid->size; i++) {
        if (((long*)(list_alg_oid->data))[i] != LTC_RSA_ENC_OID[i]) {
            err = CRYPT_PK_INVALID_TYPE;
            goto LBL_ERR;
        }
    }

    /* use the rsa_import for extracting the PKCS #1 */
    if ((err = rsa_import(list_private_key->data, list_private_key->size, key)) 
         != CRYPT_OK) {
        goto LBL_ERR;
    }

    der_sequence_free(decoded_list); /* free all decoded list mem */
    return CRYPT_OK;

LBL_ERR:
    /* make sure we're at least returning some error in case some code above didn't set err */
    if (err == 0)
        err = -1; /* TODO: better error code? */
    der_sequence_free(decoded_list); /* free all decoded list mem */
    return err;
}

