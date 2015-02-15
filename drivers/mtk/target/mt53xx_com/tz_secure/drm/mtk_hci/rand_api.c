/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2009,2010,2011,2012 Sony Corporation.
 */
//#include <openssl/rand.h>
/* cmn/include */
#include "basic_types.h"
/* sec/common */
#include "rand_api.h"
#include "x_typedef.h"


retcode_t
rand_Init(rand_ctx_t *out_ctx)
{
    retcode_t      ret = ERR_OK;

    return ret;
}

retcode_t
rand_SetBlocking(rand_ctx_t io_ctx)
{
    retcode_t      ret = ERR_OK;

    return ret;
}

retcode_t
rand_SetNonBlocking(rand_ctx_t io_ctx)
{
    retcode_t      ret = ERR_OK;

    return ret;
}

retcode_t
rand_GetBytes(rand_ctx_t  in_ctx,
              u_int32_t   in_len,
              void       *io_buf)
{
    retcode_t      ret = ERR_OK;

/*
    if (RAND_bytes((unsigned char *)io_buf, in_len) != 1) {
        ret = ERR_NG;
    }
*/
    x_mtk_tz_in_api_rng((UINT8*)io_buf, (UINT32)in_len);

    return ret;
}

retcode_t
rand_Fin(rand_ctx_t in_ctx)
{
    retcode_t      ret = ERR_OK;

    return ret;
}

