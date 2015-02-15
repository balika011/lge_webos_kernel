/*
 * Nautilus version 3.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2009 Sony Corporation, All Rights Reserved.
 */
#include "basic_types.h"
/*#include "cmn_api_sec.h"
#include "mem_api.h"*/
#include "dbg_api.h"
#include "sha256_api.h"

#include "x_typedef.h"
//#include "tz_drv_crypto.h" //NAUTILUS_FOR_LINUX
#include "x_tz_customer.h"

typedef struct l_sha256_cxt {
    UINT32  handle;
} l_sha256_cxt_t;

retcode_t
sha256_Init( sha256_handle_t *io_sha )
{
    retcode_t        ret = ERR_OK;
    l_sha256_cxt_t  *sha = NULL;
    MSGENT(("sha256_Init"));

    if ( NULL == io_sha )
        RET( ERR_ILLEGALARGS );

    sha = (l_sha256_cxt_t *)tom_malloc( sizeof *sha );
    if ( NULL == sha )
        RET( ERR_NOMEM );

    x_mtk_tz_in_api_sha256_Init( &(sha->handle) );
    *io_sha = (sha256_handle_t)sha;
    sha = NULL;

  EXIT:
    MSGEXT(("sha256_Init"));
    tom_free( sha );
    return ret;
}

retcode_t
sha256_Update( sha256_handle_t    io_sha,
               const u_int8_t    *in_buf,
               u_int32_t          in_len )
{
    retcode_t        ret = ERR_OK;
    l_sha256_cxt_t  *sha = io_sha;
    MSGENT(("sha256_Update"));

    if ( NULL == sha )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_sha256_Update( &(sha->handle), (UINT32)in_len, (BYTE *)in_buf );

  EXIT:
    MSGEXT(("sha256_Update"));
    return ret;
}

retcode_t
sha256_Fin( sha256_handle_t  io_sha,
            u_int8_t         out_digest[SHA256_DIGESTSIZE] )
{
    retcode_t        ret = ERR_OK;
    l_sha256_cxt_t  *sha = io_sha;
    MSGENT(("sha256_Fin"));

    if ( NULL == sha )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_sha256_Final( &(sha->handle), out_digest );
    tom_free( sha );

  EXIT:
    MSGEXT(("sha256_Fin"));
    return ret;
}

retcode_t
sha256_MakeDigest( const u_int8_t  *in_buf,
                   u_int32_t        in_len,
                   u_int8_t         out_digest[SHA256_DIGESTSIZE] )
{
    retcode_t  ret = ERR_OK;
    MSGENT(("sha256_MakeDigest"));

    if ( NULL == in_buf )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_sha256_MakeDigest( (BYTE *)in_buf, (UINT32)in_len, (BYTE *)out_digest );

  EXIT:
    MSGEXT(("sha256_MakeDigest"));
    return ret;
}
