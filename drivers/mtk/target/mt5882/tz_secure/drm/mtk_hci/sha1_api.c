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
#include "sha1_api.h"

#include "x_typedef.h"
//#include "tz_drv_crypto.h" //NAUTILUS_FOR_LINUX
#include "x_tz_customer.h"

typedef struct l_sha1_cxt {
    UINT32  handle;
} l_sha1_cxt_t;

retcode_t
sha1_Init( sha1_handle_t *io_sha )
{
    retcode_t      ret = ERR_OK;
    l_sha1_cxt_t  *sha = NULL;
    MSGENT(("sha1_Init"));

    if ( NULL == io_sha )
        RET( ERR_ILLEGALARGS );

    sha = (l_sha1_cxt_t *)tom_malloc( sizeof *sha );
    if ( NULL == sha )
        RET( ERR_NOMEM );

    x_mtk_tz_in_api_sha1_Init( &(sha->handle) );
    *io_sha = (sha1_handle_t)sha;
    sha = NULL;

  EXIT:
    MSGEXT(("sha1_Init"));
    tom_free( sha );
    return ret;
}

retcode_t
sha1_Update( sha1_handle_t    io_sha,
             const u_int8_t  *in_buf,
             u_int32_t        in_len )
{
    retcode_t      ret = ERR_OK;
    l_sha1_cxt_t  *sha = io_sha;
    MSGENT(("sha1_Update"));

    if ( NULL == sha )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_sha1_Update( &(sha->handle), (UINT32)in_len, (BYTE *)in_buf );

  EXIT:
    MSGEXT(("sha1_Update"));
    return ret;
}

retcode_t
sha1_Fin( sha1_handle_t  io_sha,
          u_int8_t       out_digest[SHA1_DIGESTSIZE] )
{
    retcode_t      ret = ERR_OK;
    l_sha1_cxt_t  *sha = io_sha;
    MSGENT(("sha1_Fin"));

    if ( NULL == sha )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_sha1_Final( &(sha->handle), out_digest );
    tom_free( sha );

  EXIT:
    MSGEXT(("sha1_Fin"));
    return ret;
}

retcode_t
sha1_MakeDigest( const u_int8_t  *in_buf,
                 u_int32_t        in_len,
                 u_int8_t         out_digest[SHA1_DIGESTSIZE] )
{
    retcode_t  ret = ERR_OK;
    MSGENT(("sha1_MakeDigest"));

    if ( NULL == in_buf )
        RET( ERR_ILLEGALARGS );

    x_mkt_tz_in_api_sha1_MakeDigest( (BYTE *)in_buf, (UINT32)in_len, (BYTE *)out_digest );

  EXIT:
    MSGEXT(("sha1_MakeDigest"));
    return ret;
}
