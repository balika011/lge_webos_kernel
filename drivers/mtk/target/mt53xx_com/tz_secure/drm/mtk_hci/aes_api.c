/*
 * Nautilus version 3.0
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Sony Corporation, All Rights Reserved.
 */

#include "basic_types.h"
/*#include "cmn_api_sec.h"*/
#include "mem_api.h"
#include "dbg_api.h"
#include "binstr_api.h"
#include "crypto_api.h"
#include "aes_api.h"

#include "x_typedef.h"
//#include "tz_drv_crypto.h" //NAUTILUS_FOR_LINUX
#include "x_tz_customer.h"

typedef struct {
    u_int32_t  dir;
    BYTE      *key;
} aes_ctx_t;

retcode_t
aes_Init( aes_handle_t   *io_aes,
          u_int32_t       in_dir,
          u_int32_t       in_keyLen,
          const u_int8_t *in_keyBuf )
{
    retcode_t   ret = ERR_OK;
    aes_ctx_t  *aes = NULL;
    MSGENT(("aes_Init"));

    aes = (aes_ctx_t  *)tom_malloc( sizeof *aes );
    if ( NULL == aes )
        RET( ERR_NOMEM );

    /* check: direction */
    switch ( in_dir ) {
    case AES_ENCRYPT:
    case AES_DECRYPT:
        aes->dir = in_dir;
        break;
    default:
        RET( ERR_AES_KEY_DIR );
        break;
    }

    /* check: key length */
    switch ( in_keyLen ) {
    case AES_KEYLENGTH_128:
        /* DO NOTHING */
        break;
    case AES_KEYLENGTH_192:
    case AES_KEYLENGTH_256:
    default:
        RET( ERR_AES_KEY_MAT );
        break;
    }

    aes->key = (BYTE *)tom_malloc( in_keyLen );
    if ( NULL == aes->key )
        RET( ERR_NOMEM );

    tom_memcpy( aes->key, in_keyBuf, in_keyLen );
    *io_aes = (aes_handle_t)aes; aes = NULL;

  EXIT:
    aes_Fin( aes );
    MSGEXT(("aes_Init"));
    return ret;
}

retcode_t
aes_Fin( aes_handle_t io_aes )
{
    retcode_t   ret = ERR_OK;
    aes_ctx_t  *aes = (aes_ctx_t *)io_aes;
    MSGENT(("aes_Fin"));

    if ( aes ) {
        tom_free( aes->key );
        tom_free( aes );
    }

    MSGEXT(("aes_Fin"));
    return ret;
}

retcode_t
aes_BlockEncrypt( aes_handle_t    in_aes,
                  const u_int8_t *in_plain,
                  u_int8_t       *out_cipher )
{
    retcode_t   ret = ERR_OK;
    aes_ctx_t  *aes = (aes_ctx_t *)in_aes;
    MSGENT(("aes_BlockEncrypt"));

    if ( aes == NULL || aes->dir == AES_DECRYPT )
        RET( ERR_AES_CIPHER_STATE );

    if ( in_plain == NULL || out_cipher == NULL || aes->key == NULL )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_aes128e_ecb( aes->key, (BYTE *)in_plain, (BYTE *)out_cipher );

  EXIT:
    MSGEXT(("aes_BlockEncrypt"));
    return ret;
}

retcode_t
aes_BlockDecrypt( aes_handle_t    in_aes,
                  const u_int8_t *in_cipher,
                  u_int8_t       *out_plain )
{
    retcode_t   ret = ERR_OK;
    aes_ctx_t  *aes = (aes_ctx_t *)in_aes;
    MSGENT(("aes_BlockDecrypt"));

    if ( aes == NULL || aes->dir == AES_ENCRYPT )
        RET( ERR_AES_CIPHER_STATE );

    if ( in_cipher == NULL || out_plain == NULL || aes->key == NULL )
        RET( ERR_ILLEGALARGS );

    x_mtk_tz_in_api_aes128d_ecb( aes->key, (BYTE *)in_cipher, (BYTE *)out_plain );

  EXIT:
    MSGEXT(("aes_BlockDecrypt"));
    return ret;
}

