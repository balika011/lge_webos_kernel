/*
 * Nautilus version 3.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2004,2005,2006,2007,2008,2009 Sony Corporation, All Rights Reserved.
 */
/*===========================================================================*
 * PKCS8 library
 * $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt53xx_com/tz_secure/drm/mtk_hci/pkcs8.c#1 $
 *===========================================================================*/

#include "basic_types.h"
#include "dbg_api.h"
#include "asn_api.h"
#include "binstr_api.h"
#include "pkcs8_api.h"
//#include "hci_api_util.h"
#include "cmn_api_sec.h"

//#define NPOPT_BUILD_SECURECORE

enum {
    PKCS8_VERSION			= 0,
    PKCS8_KEYALGORITHM,
    PKCS8_KEYINFO
};

enum {
    PKCS8_N				= 1,
    PKCS8_E,
    PKCS8_D,
    PKCS8_P,
    PKCS8_Q,
    PKCS8_dP,
    PKCS8_dQ,
    PKCS8_qInv
};

//#define NPOPT_BUILD_SECURECORE

#if defined(NPOPT_BUILD_SECURECORE)
#define L_USE_LOCAL_ASN
#endif

#if defined(L_USE_LOCAL_ASN)
/*
 * PKCS8: (from http://www.rsa.com/rsalabs/node.asp?id=2130)
 *
 * 6. Private-key information syntax
 *
 * This section gives the syntax for private-key information.
 *
 * Private-key information shall have ASN.1 type: PrivateKeyInfo:
 *
 * PrivateKeyInfo ::= SEQUENCE {
 *  version Version,
 *  privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
 *  privateKey PrivateKey,
 *  attributes [0] IMPLICIT Attributes OPTIONAL }
 * Version ::= INTEGER
 * PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
 * PrivateKey ::= OCTET STRING
 * Attributes ::= SET OF Attribute
 *
 *    version is the syntax version number, for
 *    compatibility with future revisions of this
 *    standard. It shall be 0 for this version of the
 *    standard.
 *
 *    privateKeyAlgorithm identifies the private-key
 *    algorithm. One example of a private-key algorithm
 *    is PKCS #1's rsaEncryption.
 *
 *    privateKey is an octet string whose contents are
 *    the value of the private key. The interpretation
 *    of the contents is defined in the registration of
 *    the private-key algorithm. For an RSA private key,
 *    for example, the contents are a BER encoding of a
 *    value of type RSAPrivateKey.
*/

typedef struct {
    binstr_t atm[PKCS8_qInv];
} l_asn_t;

#define L_ASN_CHECK MSGI3(("current:\n"));DUMP3((in_len,in_buf))
#define L_ASN_ERR RET( ERR_ASN_FORMAT )

static retcode_t /* L_USE_LOCAL_ASN */
l_asn_DecodePKCS8(
    u_int8_t  *in_buf,
    u_int32_t  in_len,
    l_asn_t   *out_key)
{
    retcode_t ret;
    u_int32_t len;
    int i;

    MSGENT(("l_asn_DecodePKCS8/pkcs8"));
    /* skip sequence tag */
    L_ASN_CHECK;
    if ( in_buf[0] != 0x30 || in_buf[1] != 0x82 ) L_ASN_ERR;
    len = (in_buf[2]<<8) | (in_buf[3]);
    if ( len+4 != in_len ) L_ASN_ERR;
    in_buf += 4; in_len -= 4;
    /* Version: should be '02 01 00' */
    L_ASN_CHECK;
    if ( in_len < 3 || in_buf[0] != 0x02 || in_buf[1] != 0x01 || in_buf[2] != 0 )
        L_ASN_ERR;
    in_buf += 3; in_len -= 3;
    /* privateKeyAlgorithm
     * should be RSA(30 0D 06 09 2A 86 48 86 F7 0D 01 01 01 05 00)
     * currently no check
     */
    L_ASN_CHECK;
    if ( in_len < 3 || in_buf[0] != 0x30 || in_buf[2] != 0x06 || (len = in_buf[1]) > 127 )
        L_ASN_ERR;
    len += 2;
    if ( len > in_len || in_buf[3] != len-6 ) L_ASN_ERR;
    in_buf += len; in_len -= len;
    /* privatekey: octet string (begin with 04 82 XX XX) */
    L_ASN_CHECK;
    if ( in_len < 4 || in_buf[0] != 0x04 || in_buf[1] != 0x82 )
        L_ASN_ERR;
    len = (in_buf[2]<<8) | (in_buf[3]);
    if ( len+4 > in_len ) L_ASN_ERR;
    in_buf += 4;
    in_len = len;
    /* RSAPrivateKey
     * RSAPrivateKey ::= SEQUENCE {
     * version Version,(should be 0 for PKCS#1-V1.5)
     * modulus INTEGER, -- n
     * publicExponent INTEGER, -- e
     * privateExponent INTEGER, -- d
     * prime1 INTEGER, -- p
     * prime2 INTEGER, -- q
     * exponent1 INTEGER, -- d mod (p-1)
     * exponent2 INTEGER, -- d mod (q-1)
     * coefficient INTEGER -- (inverse of q) mod p }
     */
    /* skip sequence tag */
    L_ASN_CHECK;
    if ( in_buf[0] != 0x30 || in_buf[1] != 0x82 ) L_ASN_ERR;
    len = (in_buf[2]<<8) | (in_buf[3]);
    if ( len+4 != in_len ) L_ASN_ERR;
    in_buf += 4; in_len -= 4;
    /* version should be '02 01 00' */
    L_ASN_CHECK;
    if ( in_len < 3 || in_buf[0] != 0x02 || in_buf[1] != 0x01 || in_buf[2] != 0 )
        L_ASN_ERR;
    in_buf += 3; in_len -= 3;
    /* INTEGERs */
    for (i = 0; i < 8; i ++) {
        L_ASN_CHECK;
        if ( in_buf[0] != 0x02 || in_len < 2 ) L_ASN_ERR;
        len = in_buf[1];
        in_buf += 2; in_len -= 2;
        if ( len > 0x7F ) {
            switch ( len ) {
            case 0x81:
                if ( in_len < 3 ) L_ASN_ERR;
                len = in_buf[0];
                in_buf ++; in_len --;
                break;
            case 0x82:
                if ( in_len < 4 ) L_ASN_ERR;
                len = (in_buf[0]<<8) | (in_buf[1]);
                in_buf += 2; in_len -= 2;
                break;
            default:
                L_ASN_ERR;
            }
        }
        if ( in_len < len ) L_ASN_ERR;
        /* binstr_SetReference() */
        out_key->atm[i].len = len;
        out_key->atm[i].buf = in_buf;
        in_buf += len;
        in_len -= len;
    }
    ret = ERR_OK;

  EXIT:
    MSGEXT(("l_asn_DecodePKCS8/pkcs8"));
    return ret;
}

static retcode_t /* L_USE_LOCAL_ASN */
pkcs8_l_GetData( /* L_USE_LOCAL_ASN */
    l_asn_t  *in_av,
    int       in_idx,
    binstr_t *out_str)
{
    retcode_t ret;
    if ( !hci_BinstrSetClone(out_str, in_av->atm[in_idx-1].len, in_av->atm[in_idx-1].buf) )
        RET( ERR_NOMEM );
    ret = ERR_OK;
  EXIT:
    return ret;
}

#else /* !L_USE_LOCAL_ASN */

static retcode_t
pkcs8_l_GetData(
    asn_t*			in_av,
    u_int32_t		in_idx,
    int				in_clone,
    binstr_t*			out_str)
{
    retcode_t			ret = ERR_OK;
    asn_t*			av;
    binstr_t*			bs;
    binstr_t*			tmp;

    MSGENT(("pkcs8_l_GetData"));

    /* check */
    av = ASN_ST_AT(in_av, in_idx);
    bs = out_str;
    if ( av == NULL || bs == NULL )
        RET(ERR_NG);

    /* clone */
    if ( av->stflag ) {
        if (!in_clone)
            tmp = hci_BinstrSetReference(bs, av->_src_size, av->_src_addr);
        else
            tmp = hci_BinstrSetClone(bs, av->_src_size, av->_src_addr);
    } else {
        if (!in_clone)
            tmp = hci_BinstrSetReference(bs, av->length, av->v.body);
        else
            tmp = hci_BinstrSetClone(bs, av->length, av->v.body);
    }
    if ( tmp == NULL )
        RET(ERR_NOMEM);


EXIT:
    MSGEXT(("pkcs8_l_GetData"));
    return ret;
}

#endif /* L_USE_LOCAL_ASN */

static retcode_t
pkcs8_l_GetKey(
    binstr_t*			in_pkcs8,
#if !defined(NPOPT_BUILD_SECURECORE)
    int				in_clone,
#endif
    pkcs8_key_t**		out_key)
{
    retcode_t			ret = ERR_OK;
    pkcs8_key_t*		key = NULL;
#if defined(L_USE_LOCAL_ASN)
    l_asn_t  av_keyinfo_body;
    l_asn_t *av_keyinfo = &av_keyinfo_body;
#else
    asn_t*			av_root = NULL;
    asn_t*			av_keyinfo = NULL;
    asn_t*			av;
#endif

    MSGENT(("pkcs8_l_GetKey"));

    /* check */
    if ( in_pkcs8 == NULL || out_key == NULL )
        RET(ERR_ILLEGALARGS);

    /* parse */
#if defined(L_USE_LOCAL_ASN)
    RETR( l_asn_DecodePKCS8(hci_BinstrBuf(in_pkcs8), hci_BinstrLen(in_pkcs8), av_keyinfo),
          CONVR(ERR_PKCS8_ILLEGALFORMAT) );
#else
    RETR(asn_DecodeWoCopy(hci_BinstrBuf(in_pkcs8), hci_BinstrLen(in_pkcs8), NULL, &av_root),
         CONVR(ERR_PKCS8_ILLEGALFORMAT) );
    av = ASN_ST_AT(av_root, PKCS8_KEYINFO);
    if ( av == NULL || av->stflag != 0 )
        RET(ERR_PKCS8_ILLEGALFORMAT);
    RETR(asn_DecodeWoCopy(av->v.body, av->length, NULL, &av_keyinfo),
         CONVR(ERR_PKCS8_ILLEGALFORMAT) );
#endif

    /* copy */
    key = hci_Alloc(sizeof(pkcs8_key_t));
    if ( !key ) RET(ERR_NOMEM);
    tom_memset(key, 0, sizeof(pkcs8_key_t));

#if defined(NPOPT_BUILD_SECURECORE)
    key->flag = 1;
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_N, &key->n));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_E, &key->e));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_D, &key->d));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_P, &key->p));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_Q, &key->q));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_dP, &key->dp));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_dQ, &key->dq));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_qInv, &key->qinv));
#else /* !defined(NPOPT_BUILD_SECURECORE) */
    key->flag = in_clone;
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_N,    in_clone, &key->n));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_E,    in_clone, &key->e));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_D,    in_clone, &key->d));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_P,    in_clone, &key->p));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_Q,    in_clone, &key->q));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_dP,   in_clone, &key->dp));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_dQ,   in_clone, &key->dq));
    RET(pkcs8_l_GetData(av_keyinfo, PKCS8_qInv, in_clone, &key->qinv));
#endif /* !defined(NPOPT_BUILD_SECURECORE) */
    *out_key = key;

EXIT:
    if ( ret != ERR_OK ) {
        pkcs8_FreeKey(key);
    }
#if !defined(L_USE_LOCAL_ASN)
    asn_Free(av_root);
    asn_Free(av_keyinfo);
#endif
    MSGEXT(("pkcs8_l_GetKey"));
    return ret;
}

//#if !defined(NPOPT_BUILD_SECURECORE)
retcode_t
pkcs8_GetKey(
    binstr_t*			in_pkcs8,
    pkcs8_key_t**		out_key)
{
    return pkcs8_l_GetKey(in_pkcs8,
#if !defined(NPOPT_BUILD_SECURECORE)
        FALSE,
#endif
        out_key);
}
//#endif /* !defined(NPOPT_BUILD_SECURECORE) */

retcode_t
pkcs8_GetCloneKey(
    binstr_t*			in_pkcs8,
    pkcs8_key_t**		out_key)
{
#if defined(NPOPT_BUILD_SECURECORE)
    return pkcs8_l_GetKey(in_pkcs8, out_key);
#else /* !defined(NPOPT_BUILD_SECURECORE) */
    return pkcs8_l_GetKey(in_pkcs8, TRUE, out_key);
#endif /* !defined(NPOPT_BUILD_SECURECORE) */
}

retcode_t
pkcs8_FreeKey(pkcs8_key_t *io_key)
{
    MSGENT(("pkcs8_FreeKey"));
    if ( io_key != NULL && io_key->flag ) {
        hci_BinstrFreeClone(&io_key->n);
        hci_BinstrFreeClone(&io_key->e);
        hci_BinstrFreeClone(&io_key->d);
        hci_BinstrFreeClone(&io_key->p);
        hci_BinstrFreeClone(&io_key->q);
        hci_BinstrFreeClone(&io_key->dp);
        hci_BinstrFreeClone(&io_key->dq);
        hci_BinstrFreeClone(&io_key->qinv);
    }
    hci_Free(io_key, sizeof(pkcs8_key_t));
    MSGEXT(("pkcs8_FreeKey"));
    return ERR_OK;
}
