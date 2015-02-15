/*
 * Nautilus version 3.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2009 Sony Corporation, All Rights Reserved.
 */
//#include <stdio.h>
//#include <stdlib.h>
#include "basic_types.h"
#include "cmn_api_sec.h"
//#include "mem_api.h"
#include "dbg_api.h"
#include "binstr_api.h"
#include "rsa_api.h"
#include "sha256_api.h"
#include "sha1_api.h"
//#include "hci_api_util.h"

#include "tommath.h"
#include "tomcrypt.h"


#include "x_typedef.h"
//#include "tz_drv_crypto.h" //NAUTILUS_FOR_LINUX
#include "x_tz_customer.h"
#include "s_rel_func.h"

#define L_PRNG_IMPL     "nautilus_prng"
#define L_SHA1_IMPL     "nautilus_sha1"
#define L_SHA256_IMPL   "nautilus_sha256"

#define L_MAX_BUF_SZ    (2048/8)

#if 0 /*zink*/
    #define  L_PROCESSOR_INTENSIVE_BLOCK_ENTER() nautilus_yield()
    #define  L_PROCESSOR_INTENSIVE_BLOCK_EXIT()  nautilus_yield()
#else
    #define  L_PROCESSOR_INTENSIVE_BLOCK_ENTER()
    #define  L_PROCESSOR_INTENSIVE_BLOCK_EXIT()
#endif

/* Forward declarations for PRNG callbacks */
//NAUTILUS_FOR_LINUX add static for each function prototype
static int l_rng_ok            ( prng_state *in_state );
static int l_rng_add_entropy   ( const unsigned char *in,
                          unsigned long        in_len,
                          prng_state          *in_state );
static unsigned long l_rng_read( unsigned char *out_buf,
                          unsigned long  out_len,
                          prng_state    *in_state );
static int l_rng_export        ( unsigned char *out_buf,
                          unsigned long *out_len,
                          prng_state    *in_state );
static int l_rng_import        ( const unsigned char *in_buf,
                          unsigned long        in_len,
                          prng_state          *in_state );
static int l_rng_test          ( void );


/* Forward declarations for SHA1 callbacks */
static int l_sha1_init  ( hash_state *in_hash );
static int l_sha1_update( hash_state          *in_hash,
                   const unsigned char *in_buf,
                   unsigned long        in_len );
static int l_sha1_done  ( hash_state    *in_hash,
                   unsigned char *out_buf );


/* Forward declarations for SHA256 callbacks */
static int l_sha256_init  ( hash_state *in_hash );
static int l_sha256_update( hash_state          *in_hash,
                     const unsigned char *in_buf,
                     unsigned long        in_len );
static int l_sha256_done  ( hash_state    *in_hash,
                     unsigned char *out_buf );

/* Forward declarations for common hash callbacks */
static int l_hash_test( void );


sha1_handle_t   g_sha1_handle;
sha256_handle_t g_sha256_handle;

struct ltc_prng_descriptor nautilus_prng = {
    L_PRNG_IMPL,
    8,
    l_rng_ok,
    l_rng_add_entropy,
    l_rng_ok,
    l_rng_read,
    l_rng_ok,
    l_rng_export,
    l_rng_import,
    l_rng_test
};

struct ltc_hash_descriptor  nautilus_sha1 = {
    L_SHA1_IMPL,
    0,
    SHA1_DIGESTSIZE,
    SHA1_DATASIZE,
    { 1, 3, 14, 3, 2, 26 },
    6,
    l_sha1_init,
    l_sha1_update,
    l_sha1_done,
    l_hash_test
};

struct ltc_hash_descriptor  nautilus_sha256 = {
    L_SHA256_IMPL,
    0,
    SHA256_DIGESTSIZE,
    SHA256_DATASIZE,
    { 2, 16, 840, 1, 101, 3, 4, 2, 1,  },
    9,
    l_sha256_init,
    l_sha256_update,
    l_sha256_done,
    l_hash_test
};

/* -------------------------------------------------------------- */
/* Nautilus error -> Libtomcrypt error                            */
static int
l_ConvToCryptError( retcode_t in_ret )
{
    int cret = CRYPT_ERROR;

    switch ( in_ret ) {
    case ERR_OK:
        cret = CRYPT_OK;
        break;

    case ERR_NOMEM:
        cret = CRYPT_MEM;
        break;

    default:
        cret = CRYPT_ERROR;
        break;
    }

    return cret;
}

/* -------------------------------------------------------------- */
/* Callbacks for PRNG in libtomcrypt                              */
static int
l_rng_ok( prng_state *in_state )
{
    return CRYPT_OK;
}

static int
l_rng_add_entropy( const unsigned char *in,
                   unsigned long        in_len,
                   prng_state          *in_state )
{
    return CRYPT_OK;
}

static unsigned long
l_rng_read( unsigned char *out_buf,
            unsigned long  out_len,
            prng_state    *in_state )
{
    x_mtk_tz_in_api_rng( out_buf, out_len );
    return out_len;
}

static int
l_rng_export( unsigned char *out_buf,
              unsigned long *out_len,
              prng_state    *in_state )
{
    return CRYPT_OK;
}

static int
l_rng_import( const unsigned char *in_buf,
              unsigned long        in_len,
              prng_state          *in_state )
{
    return CRYPT_OK;
}

static int
l_rng_test( void )
{
    return CRYPT_OK;
}

/* -------------------------------------------------------------- */
/* Callbacks for SHA1 in libtomcrypt                              */
static int
l_sha1_init( hash_state *in_hash )
{
    retcode_t ret = ERR_OK;

    RET( sha1_Init( &g_sha1_handle ) );

  EXIT:
    return l_ConvToCryptError( ret );
}

static int
l_sha1_update( hash_state          *in_hash,
               const unsigned char *in_buf,
               unsigned long        in_len )
{
    retcode_t ret = ERR_OK;

    RET( sha1_Update( g_sha1_handle, in_buf, in_len ) );

  EXIT:
    return l_ConvToCryptError( ret );
}

static int
l_sha1_done( hash_state    *in_hash,
             unsigned char *out_buf )
{
    retcode_t ret = ERR_OK;

    RET( sha1_Fin( g_sha1_handle, out_buf ) );

  EXIT:
    return l_ConvToCryptError( ret );
}

/* -------------------------------------------------------------- */
/* Callbacks for SHA256 in libtomcrypt                            */
static int
l_sha256_init( hash_state *in_hash )
{
    retcode_t ret = ERR_OK;

    RET( sha256_Init( &g_sha256_handle ) );

  EXIT:
    return l_ConvToCryptError( ret );
}

static int
l_sha256_update( hash_state          *in_hash,
                 const unsigned char *in_buf,
                 unsigned long        in_len )
{
    retcode_t ret = ERR_OK;

    RET( sha256_Update( g_sha256_handle, in_buf, in_len ) );

  EXIT:
    return l_ConvToCryptError( ret );
}

static int
l_sha256_done( hash_state    *in_hash,
               unsigned char *out_buf )
{
    retcode_t ret = ERR_OK;

    RET( sha256_Fin( g_sha256_handle, out_buf ) );

  EXIT:
    return l_ConvToCryptError( ret );
}

static int
l_hash_test( void )
{
    return CRYPT_OK;
}
/* ---- Callbacks end ---------------------------- */



static retcode_t
l_rsa_CreateKey(
    rsa_key_t   **out_key,
    binstr_t     *in_n,
    binstr_t     *in_e,
    binstr_t     *in_d,
    binstr_t     *in_p,
    binstr_t     *in_q,
    binstr_t     *in_dP,
    binstr_t     *in_dQ,
    binstr_t     *in_qInv)
{
    retcode_t ret = ERR_OK;
    MSGENT(("l_rsa_CreateKey"));

    if ( NULL == out_key )
        RET( ERR_ILLEGALARGS );

    *out_key = (rsa_key_t *)tom_malloc(sizeof(rsa_key_t));
    if (!(*out_key)) RET(ERR_NOMEM);

    (*out_key)->n       = in_n;
    (*out_key)->e       = in_e;
    (*out_key)->d       = in_d;
    (*out_key)->p       = in_p;
    (*out_key)->q       = in_q;
    (*out_key)->dP      = in_dP;
    (*out_key)->dQ      = in_dQ;
    (*out_key)->qInv    = in_qInv;

EXIT:
    MSGEXT(("l_rsa_CreateKey"));
    return ret;
}

static retcode_t
l_ConvTomErr( int in_err )
{
    retcode_t ret = ERR_OK;

    switch( in_err ) {
    case MP_OKAY:
        break;
    case MP_MEM:
        ret = ERR_NOMEM;
        break;
    default:
        ret = ERR_INTERNALERROR;
        break;
    }

    return ret;
}

retcode_t
rsa_Init(rsa_handle_t   *io_rsa,
         const binstr_t *in_n,
         const binstr_t *in_e,
         const binstr_t *in_d,
         const binstr_t *in_p,
         const binstr_t *in_q,
         const binstr_t *in_dP,
         const binstr_t *in_dQ,
         const binstr_t *in_qInv)
{
    retcode_t  ret = ERR_OK;
    rsa_key   *key = NULL;

    MSGENT(("rsa_Init"));

    if ( NULL == io_rsa || NULL == in_n || NULL == in_e )
        RET( ERR_ILLEGALARGS );

    //zink no need
    //crypt_init(); /* initialise the libtomcrypt */

    if ((key = (rsa_key *)tom_malloc(sizeof(rsa_key))) == NULL)
        RETX(ERR_NOMEM, MSG1(("*** tom_malloc: Failed ...\n")));
    tom_memset(key, 0, sizeof(rsa_key));

    key->e  = NULL;
    key->d  = NULL;
    key->N  = NULL;
    key->dQ = NULL;
    key->dP = NULL;
    key->qP = NULL;
    key->p  = NULL;
    key->q  = NULL;

    /* Set public details */
    key->type = PK_PUBLIC;

    RET( l_ConvTomErr(mp_init(&(key->N))) );
    RET( l_ConvTomErr(mp_read_unsigned_bin(key->N, hci_BinstrBuf(in_n), hci_BinstrLen(in_n) )) );

    RET( l_ConvTomErr(mp_init(&(key->e))) );
    RET( l_ConvTomErr(mp_read_unsigned_bin(key->e, hci_BinstrBuf(in_e), hci_BinstrLen(in_e))) );

    if ( in_d ) {
        if ( NULL == in_p || NULL == in_q || NULL == in_dP || NULL == in_dQ || NULL == in_qInv )
            RET( ERR_ILLEGALARGS );

        /* Have D, set private details */
        key->type = PK_PRIVATE;

        /* Memory allocations for private data is HCI */
        RET( l_ConvTomErr(mp_init(&(key->d))) );
        RET( l_ConvTomErr(mp_read_unsigned_bin(key->d, hci_BinstrBuf(in_d), hci_BinstrLen(in_d))) );

        if (in_p) {
            RET( l_ConvTomErr(mp_init(&(key->p))) );
            RET( l_ConvTomErr(mp_read_unsigned_bin(key->p, hci_BinstrBuf(in_p), hci_BinstrLen(in_p))) );
        }
        if (in_q) {
            RET( l_ConvTomErr(mp_init(&(key->q))) );
            RET( l_ConvTomErr(mp_read_unsigned_bin(key->q, hci_BinstrBuf(in_q), hci_BinstrLen(in_q))) );
        }
        if (in_dP) {
            RET( l_ConvTomErr(mp_init(&(key->dP))) );
            RET( l_ConvTomErr(mp_read_unsigned_bin(key->dP, hci_BinstrBuf(in_dP), hci_BinstrLen(in_dP))) );
        }
        if (in_dQ) {
            RET( l_ConvTomErr(mp_init(&(key->dQ))) );
            RET( l_ConvTomErr(mp_read_unsigned_bin(key->dQ, hci_BinstrBuf(in_dQ), hci_BinstrLen(in_dQ))) );
        }
        if (in_qInv) {
            RET( l_ConvTomErr(mp_init(&(key->qP))) );
            RET( l_ConvTomErr(mp_read_unsigned_bin(key->qP, hci_BinstrBuf(in_qInv), hci_BinstrLen(in_qInv))) );
        }
    }

    *io_rsa = (rsa_handle_t)key;
    key = NULL;

  EXIT:
    rsa_Fin( key );
    MSGEXT(("rsa_Init"));
    return ret;
}

retcode_t
rsa_Fin(rsa_handle_t io_rsa)
{
    retcode_t  ret = ERR_OK;
    rsa_key *key = (rsa_key *)io_rsa;

    MSGENT(("rsa_Fin"));

    if (key) {
        if ( key->e  ) mp_clear(key->e);
        if ( key->d  ) mp_clear(key->d);
        if ( key->N  ) mp_clear(key->N);
        if ( key->dQ ) mp_clear(key->dQ);
        if ( key->dP ) mp_clear(key->dP);
        if ( key->qP ) mp_clear(key->qP);
        if ( key->p  ) mp_clear(key->p);
        if ( key->q  ) mp_clear(key->q);

        tom_free(key);
    }

    MSGEXT(("rsa_Fin"));

    return ret;
}

retcode_t
rsa_CreatePubKey(
    rsa_key_t   **out_key,
    binstr_t     *in_n,
    binstr_t     *in_e)
{
    retcode_t ret = ERR_OK;
    MSGENT(("rsa_CreatePubKey"));

    RET( l_rsa_CreateKey(out_key, in_n, in_e,
                         NULL,    NULL, NULL,
                         NULL,    NULL, NULL) );

EXIT:
    MSGEXT(("rsa_CreatePubKey"));
    return ret;
}

retcode_t
rsa_CreatePrivKey(
    rsa_key_t   **out_key,
    binstr_t     *in_n,
    binstr_t     *in_e,
    binstr_t     *in_d,
    binstr_t     *in_p,
    binstr_t     *in_q,
    binstr_t     *in_dP,
    binstr_t     *in_dQ,
    binstr_t     *in_qInv)
{
    retcode_t ret = ERR_OK;
    MSGENT(("rsa_CreatePrivKey"));

    if (out_key == NULL ||
        in_n == NULL ||
        in_e == NULL ||
        in_d == NULL ||
        in_e == NULL ||
        in_p == NULL ||
        in_q == NULL ||
        in_dP == NULL ||
        in_dQ == NULL ||
        in_qInv == NULL) {
        RET( ERR_ILLEGALARGS );
    }

    RET(l_rsa_CreateKey(out_key, in_n,  in_e,
                        in_d,    in_p,  in_q,
                        in_dP,   in_dQ, in_qInv));

EXIT:
    MSGEXT(("rsa_CreatePrivKey"));
    return ret;
}

retcode_t
rsa_FreeKey(rsa_key_t *io_key)
{
    MSGENT(("rsa_FreeKey"));

    if (io_key) {
        hci_BinstrFree(io_key->n);
        hci_BinstrFree(io_key->e);
        hci_BinstrFree(io_key->d);
        hci_BinstrFree(io_key->p);
        hci_BinstrFree(io_key->q);
        hci_BinstrFree(io_key->dP);
        hci_BinstrFree(io_key->dQ);
        hci_BinstrFree(io_key->qInv);
        tom_free(io_key);
    }

    MSGEXT(("rsa_FreeKey"));
    return ERR_OK;
}

/******************************/

retcode_t
rsa_RsaesOaepEncrypt(rsa_handle_t     io_rsa,
                     const binstr_t  *in_M,
                     const binstr_t  *in_P,
                     binstr_t       **out_C)
{
    retcode_t  ret = ERR_OK;
    rsa_key *key = (rsa_key *)io_rsa;
    u_int8_t *buf = NULL;
    unsigned long len;
    binstr_t *str = NULL;

    int hash_idx;
    int prng_idx;
    u_int8_t *param = NULL;
    u_int32_t paramlen = 0;

    MSGENT(("rsa_RsaesOaepEncrypt"));

    if ( NULL == key || NULL == in_M || NULL == out_C )
        RET( ERR_ILLEGALARGS );

    if ( NULL != in_P ) {
        param    = hci_BinstrBuf( in_P );
        paramlen = hci_BinstrLen( in_P );
    }

    if ( 0 != paramlen && NULL == param )
        RET( ERR_ILLEGALARGS );

    buf = (unsigned char *)tom_malloc( L_MAX_BUF_SZ );
    if ( !buf ) RET( ERR_NOMEM );
    len = L_MAX_BUF_SZ;

    if ( register_prng(&nautilus_prng) == -1 ) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }
    prng_idx = find_prng(L_PRNG_IMPL);

    if ( register_hash(&nautilus_sha1) == -1) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }
    hash_idx = find_hash(L_SHA1_IMPL);

    L_PROCESSOR_INTENSIVE_BLOCK_ENTER();

    if ( rsa_encrypt_key_ex (hci_BinstrBuf(in_M),
                             hci_BinstrLen(in_M),
                             buf,
                             &len,
                             param,
                             paramlen,
                             NULL,
                             prng_idx,
                             hash_idx,
                             LTC_PKCS_1_OAEP,
                             key) ) {
        RET( ERR_RSA_INVALID );
    }

    L_PROCESSOR_INTENSIVE_BLOCK_EXIT();

    str = hci_BinstrAlloc(len, buf);
    if ( !str ) RET( ERR_NOMEM );
    *out_C = str;

  EXIT:
    if ( buf ) tom_free(buf);
    MSGEXT(("rsa_RsaesOaepEncrypt"));
    return ret;
}

retcode_t
rsa_RsaesOaepDecrypt(rsa_handle_t     io_rsa,
                     const binstr_t  *in_C,
                     const binstr_t  *in_P,
                     binstr_t       **out_M)
{
    retcode_t  ret = ERR_OK;
    rsa_key *key = (rsa_key *)io_rsa;
    unsigned long len;
    u_int8_t *buf = NULL;
    binstr_t *str = NULL;

    int res;
    int hash_idx;
    u_int8_t *param = NULL;
    u_int32_t paramlen = 0;

    MSGENT(("rsa_RsaesOaepDecypt"));

    if ( NULL == key || NULL == in_C || NULL == out_M )
        RET( ERR_ILLEGALARGS );

    if ( NULL != in_P ) {
        param    = hci_BinstrBuf( in_P );
        paramlen = hci_BinstrLen( in_P );
    }

    if ( 0 != paramlen && NULL == param )
        RET( ERR_ILLEGALARGS );

    buf = hci_Alloc( L_MAX_BUF_SZ );
    if ( !buf ) RET( ERR_NOMEM );
    len = L_MAX_BUF_SZ;

    if ( register_hash(&nautilus_sha1) == -1) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }
    hash_idx = find_hash(L_SHA1_IMPL);

    L_PROCESSOR_INTENSIVE_BLOCK_ENTER();

    if ( rsa_decrypt_key_ex( hci_BinstrBuf(in_C),
                             hci_BinstrLen(in_C),
                             buf,
                             &len,
                             param,
                             paramlen,
                             hash_idx,
                             LTC_PKCS_1_OAEP,
                             &res,
                             key)  ) {
        RET( ERR_RSA_INVALID );
    }

    L_PROCESSOR_INTENSIVE_BLOCK_EXIT();

    if ( res != 1 )
        RET( ERR_RSA_INVALID );

    str = hci_BinstrAlloc(len, buf);

    if ( !str ) RET( ERR_NOMEM );
    *out_M = str;

  EXIT:
    if ( buf ) hci_Free(buf, L_MAX_BUF_SZ);
    MSGEXT(("rsa_RsaesOaepDecypt"));
    return ret;
}

retcode_t
rsa_RsaesPkcs1V1_5Encrypt(rsa_handle_t     io_rsa,
                          const binstr_t  *in_M,
                          binstr_t       **out_C)
{
    retcode_t  ret = ERR_OK;
    rsa_key *key = (rsa_key *)io_rsa;
    u_int8_t *buf = NULL;
    unsigned long len;
    binstr_t *str = NULL;

    int prng_idx;
    int hash_idx = 0; /* Ignored */

    MSGENT(("rsa_RsaesPkcs1V1_5Encrypt"));

    if ( NULL == key || NULL == in_M || NULL == out_C )
        RET( ERR_ILLEGALARGS );

    buf = (unsigned char *)tom_malloc( L_MAX_BUF_SZ );
    if ( !buf ) RET( ERR_NOMEM );
    len = L_MAX_BUF_SZ;

    if ( register_prng(&nautilus_prng) == -1 ) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }
    prng_idx = find_prng(L_PRNG_IMPL);

    L_PROCESSOR_INTENSIVE_BLOCK_ENTER();

    if ( rsa_encrypt_key_ex (hci_BinstrBuf(in_M),
                             hci_BinstrLen(in_M),
                             buf,
                             &len,
                             NULL,
                             0,
                             NULL,
                             prng_idx,
                             hash_idx,
                             LTC_PKCS_1_V1_5,
                             key) ) {
        RET( ERR_RSA_INVALID );
    }

    L_PROCESSOR_INTENSIVE_BLOCK_EXIT();

    str = hci_BinstrAlloc(len, buf);
    if ( !str ) RET( ERR_NOMEM );
    *out_C = str;

  EXIT:
    if ( buf ) tom_free(buf);
    MSGEXT(("rsa_RsaesPkcs1V1_5Encrypt"));
    return ret;
}

retcode_t
rsa_RsaesPkcs1V1_5Decrypt(rsa_handle_t     io_rsa,
                          const binstr_t  *in_C,
                          binstr_t       **out_M)
{
    retcode_t  ret = ERR_OK;
    rsa_key *key = (rsa_key *)io_rsa;
    unsigned long len;
    u_int8_t *buf = NULL;
    binstr_t *str = NULL;
    int res;

    MSGENT(("rsa_RsaesPkcs1V1_5Decrypt"));

    if ( NULL == key || NULL == in_C || NULL == out_M )
        RET( ERR_ILLEGALARGS );

    buf = hci_Alloc( L_MAX_BUF_SZ );
    if ( !buf ) RET( ERR_NOMEM );
    len = L_MAX_BUF_SZ;

    L_PROCESSOR_INTENSIVE_BLOCK_ENTER();

    if ( rsa_decrypt_key_ex( hci_BinstrBuf(in_C),
                             hci_BinstrLen(in_C),
                             buf,
                             &len,
                             NULL,
                             0,
                             0, /* hash_idx, none for pkcs1.5 */
                             LTC_PKCS_1_V1_5,
                             &res,
                             key)  ) {
        RET( ERR_RSA_INVALID );
    }

    L_PROCESSOR_INTENSIVE_BLOCK_EXIT();

    if ( res != 1 )
        RET( ERR_RSA_INVALID );

    str = hci_BinstrAlloc(len, buf);

    if ( !str ) RET( ERR_NOMEM );
    *out_M = str;

  EXIT:
    if ( buf ) hci_Free(buf, L_MAX_BUF_SZ);
    MSGEXT(("rsa_RsaesPkcs1V1_5Decrypt"));
    return ret;
}

retcode_t
rsa_RsassaPkcs1V1_5Sign(rsa_handle_t     io_rsa,
                        int              in_algo,
                        const binstr_t  *in_M,
                        binstr_t       **out_S)
{
    retcode_t  ret = ERR_OK;
    rsa_key *key = (rsa_key *)io_rsa;
    u_int8_t shaedmsg[SHA256_DIGESTSIZE];
    unsigned long len;
    u_int8_t *buf = NULL;
    binstr_t *str = NULL;

    int prng_idx;
    int hash_idx;
    int hash_len;

    MSGENT(("rsa_RsassaPkcs1V1_5Sign"));

    if ( NULL == key || NULL == in_M || NULL == out_S )
        RET( ERR_ILLEGALARGS );

    len = 2048/8;
    buf = (unsigned char *)tom_malloc(len);
    if ( !buf ) RET( ERR_NOMEM );

    if ( register_prng(&nautilus_prng) == -1 ) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }
    prng_idx = find_prng(L_PRNG_IMPL);

    if ( in_algo == RSA_DIGESTALGO_SHA1 )
    {
        if ( register_hash(&nautilus_sha1) == -1 ) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
         hash_idx = find_hash(L_SHA1_IMPL);

        if ( sha1_MakeDigest( hci_BinstrBuf(in_M),
                              hci_BinstrLen(in_M),
                              shaedmsg) ) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_len = SHA1_DIGESTSIZE;
    }
    else if (in_algo == RSA_DIGESTALGO_SHA256)
    {
        if ( register_hash(&nautilus_sha256) == -1) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_idx = find_hash(L_SHA256_IMPL);

        if (sha256_MakeDigest(hci_BinstrBuf(in_M),
                              hci_BinstrLen(in_M),
                              shaedmsg) ) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_len = SHA256_DIGESTSIZE;
    }
    else
    {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }

    L_PROCESSOR_INTENSIVE_BLOCK_ENTER();

    RET( rsa_sign_hash_ex( shaedmsg,
                           hash_len,
                           buf,
                           &len,
                           LTC_PKCS_1_V1_5,
                           NULL,
                           prng_idx,
                           hash_idx,
                           0,
                           key ) );

    L_PROCESSOR_INTENSIVE_BLOCK_EXIT();

    if ( len == 0 ) RET( ERR_RSA_INVALID );
    str = hci_BinstrAlloc(len, buf);
    if ( !str ) RET( ERR_NOMEM );
    *out_S = str;

  EXIT:
    if ( buf ) tom_free(buf);
    MSGEXT(("rsa_RsassaPkcs1V1_5Sign"));

    return ret;
}

retcode_t
rsa_RsassaPkcs1V1_5Verify(rsa_handle_t    io_rsa,
                          int             in_algo,
                          const binstr_t *in_M,
                          const binstr_t *in_S)
{
    retcode_t  ret;
    rsa_key *key = (rsa_key *)io_rsa;
    u_int8_t shaedmsg[SHA256_DIGESTSIZE];
    int hash_idx;
    int res;
    int hash_len;

    MSGENT(("rsa_RsassaPkcs1V1_5Verify"));

    if ( NULL == key || NULL == in_M || NULL == in_S )
        RET( ERR_ILLEGALARGS );

    if ( register_hash(&nautilus_sha1) == -1) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }

    if (in_algo == RSA_DIGESTALGO_SHA1)
    {
        if ( register_hash(&nautilus_sha1) == -1) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_idx = find_hash(L_SHA1_IMPL);

        if ( sha1_MakeDigest(hci_BinstrBuf(in_M),
                             hci_BinstrLen(in_M),
                             shaedmsg) ) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_len = SHA1_DIGESTSIZE;
    }
    else if (in_algo == RSA_DIGESTALGO_SHA256)
    {
        if ( register_hash(&nautilus_sha256) == -1) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_idx = find_hash(L_SHA256_IMPL);

        if ( sha256_MakeDigest(hci_BinstrBuf(in_M),
                               hci_BinstrLen(in_M),
                               shaedmsg) ) {
            RET(ERR_RSA_INVALID_SIGNATURE);
        }
        hash_len = SHA256_DIGESTSIZE;
    }
    else
    {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }

    L_PROCESSOR_INTENSIVE_BLOCK_ENTER();

    if ( rsa_verify_hash_ex( hci_BinstrBuf(in_S),
                             hci_BinstrLen(in_S),
                             shaedmsg,
                             hash_len,
                             LTC_PKCS_1_V1_5,
                             hash_idx,
                             0,
                             &res,
                             key ) ) {
        RET(ERR_RSA_INVALID_SIGNATURE);
    }

    L_PROCESSOR_INTENSIVE_BLOCK_EXIT();

    if ( res != 1 )
        RET(ERR_RSA_INVALID_SIGNATURE);

    ret = ERR_OK;

  EXIT:
    MSGEXT(("rsa_RsassaPkcs1V1_5Verify"));

    return ret;
}

