/*
 * Nautilus version 3.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2009 Sony Corporation, All Rights Reserved.
 */
#ifndef CC_SECURE_WORLD
#include <linux/limits.h>
#endif
#include "basic_types.h"
#include "binstr_api.h"
#include "mem_api.h"
//#include "hci_api_util.h"
#include "cmn_api_sec.h"

#include "tz_nautilus.h"
#include "x_os.h"

extern void *
hci_Alloc( np_size_t in_size )
{
    void *buf = NULL;

    buf = TzSecureMemoryAllocate( in_size ); //NAUTILUS_FOR_LINUX
    if ( buf )
    {
        tom_memset( buf, 0x00, in_size );
    }

    return buf;
}


extern void
hci_Free( void *io_buf, np_size_t in_size )
{
    if ( NULL != io_buf ) {
        tom_memscrub( io_buf, in_size );
        //x_free_dtcm_mem( io_buf );
        TzSecureMemoryRelease( io_buf ); //NAUTILUS_FOR_LINUX
    }
}

binstr_t *
hci_BinstrAlloc( np_size_t in_size, const u_int8_t *in_buf )
{
    binstr_t *bs = NULL;

    /* check */
    if ( in_size > (UINT_MAX - sizeof(binstr_t)) )
        return NULL;

    /* alloc */
    bs = hci_Alloc( in_size + sizeof(binstr_t) );
    if ( NULL != bs ) {
        hci_BinstrLen( bs ) = in_size;
        hci_BinstrBuf( bs ) = (u_int8_t*)bs + sizeof(binstr_t);

        /* copy */
        if ( in_buf != NULL ) {
            tom_memcpy( hci_BinstrBuf(bs), in_buf, in_size );
        } else {
            tom_memset( hci_BinstrBuf(bs), 0, in_size );
        }
    }

    return bs;
}

void
hci_BinstrFree( binstr_t *io_str )
{
    if ( io_str )
        hci_Free( io_str, hci_BinstrLen( io_str ) + sizeof(binstr_t) );
}

binstr_t *
hci_BinstrSetClone( binstr_t       *io_str,
                    np_size_t       in_size,
                    const u_int8_t *in_buf )
{
    u_int8_t *buf = NULL;

    if ( io_str == NULL )
        return NULL;

    if ( in_size > 0 ) {
        buf = hci_Alloc( in_size );
        if ( buf == NULL )
            return NULL;

        tom_memcpy( buf, in_buf, in_size );

        hci_BinstrLen( io_str ) = in_size;
        hci_BinstrBuf( io_str ) = buf;
    }

    return io_str;
}

void
hci_BinstrFreeClone( binstr_t *io_str )
{
    if ( io_str ) {
        hci_Free( hci_BinstrBuf(io_str), hci_BinstrLen(io_str) );
        hci_BinstrBuf( io_str ) = NULL;
    }
}

binstr_t *
hci_BinstrSetReference( binstr_t*       io_str,
                        np_size_t       in_size,
                        u_int8_t*       in_buf )
{
    if ( io_str == NULL)
        return NULL;

    io_str->len  = in_size;
    io_str->buf  = in_buf;

    return io_str;
}

retcode_t
hci_BinstrCheckCmpArgs(
                const void *    in_arg1,
                const void *    in_arg2,
                int32_t * out_cmp)
{
    retcode_t ret = ERR_OK;

    if ( !in_arg1 || !in_arg2 )
    {
        ret = ERR_ILLEGALARGS;
        if ( !in_arg1 && !in_arg2)
            *out_cmp = 0;
        else if ( !in_arg1 )
            *out_cmp = -2;
        else
            *out_cmp = 2;
    }

    return ret;
}

int32_t
hci_BinstrCmpBuf(
    np_size_t			in_len1,
    const u_int8_t *		in_buf1,
    np_size_t			in_len2,
    const u_int8_t *		in_buf2)
{
    int32_t cmp;
    int32_t d;
    np_size_t len;

    if (ERR_ILLEGALARGS == hci_BinstrCheckCmpArgs( in_buf1, in_buf2, &cmp ))
        return cmp;

    len = in_len1 <= in_len2 ? in_len1 : in_len2;
    d = tom_memcmp(in_buf1, in_buf2, len);
    if ( d != 0 ) return  d;
    if ( in_len1 > len ) return 1;
    if ( in_len2 > len ) return -1;
    return 0;
}

int32_t
hci_BinstrCmp(
    const binstr_t *		in_str1,
    const binstr_t *		in_str2)
{
    int32_t cmp;

    if (ERR_ILLEGALARGS == hci_BinstrCheckCmpArgs( in_str1, in_str2, &cmp ))
        return cmp;

    return hci_BinstrCmpBuf(in_str1->len, in_str1->buf,
                            in_str2->len, in_str2->buf);
}


