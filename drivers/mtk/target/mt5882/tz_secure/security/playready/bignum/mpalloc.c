/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bigpriv.h"

/*
        File mpalloc.c.   Version 20 November, 2002


        This file has helpers for bignum allocation requests.
        The user must provide

             CRYPTO_VOID* bignum_alloc(CRYPTO_DWORD cblen)
             CRYPTO_VOID  bignum_free(CRYPTO_VOID *pvMem)
             CRYPTO_VOID* bignum_realloc(CRYPTO_VOID * pvMem, CRYPTO_DWORD cblen)

        with semantics matching the standard C
        routines malloc, free, realloc.
        [bignum_realloc is not used as of November, 2002.]
       
    Application-callable routines and macros:

          digit_allocate(CRYPTO_DWORD nelmt, const CRYPTO_CHAR *name)

                 Allocates nelmt elements of type digit_t.
                 In debug mode, -name- will be used to identify this storage.

          possible_digit_allocate(digit_tempinfo_t *tempinfo,
                                          const CRYPTO_CHAR *name)
                 For internal use only.  The tempinfo struct
                 holds the amount of data needed and a possible
                 address supplied (typically be the caller of the user).
                 Use the supplied address if not NULL,
                 but allocate if the supplied address is NULL.

 */


/*****************************************************************************/
digit_t* digit_allocate(
    const CRYPTO_DWORD     nelmt,
    const CRYPTO_CHAR      *name )
/*
   Allocate an array with nelmt digit_t elements.
   May return NULL if unsuccessful.
*/
{
    digit_t *temps = NULL;

    if ((nelmt > 0) &&
        (nelmt*sizeof(digit_t) > nelmt))  /* check for integer underflow/overflow */
    {
//        temps = (digit_t*)bignum_alloc(nelmt*sizeof(digit_t), f_pBigCtx);
        temps = malloc(nelmt*sizeof(digit_t));
    }
    
    if (temps == NULL) {
        TRACE( ( "Cannot allocate %ld digit_t entities for %s\n",
               ( CRYPTO_LONG )nelmt, name ) );
    }

    return temps;
} // end digit_allocate
/*****************************************************************************/
CRYPTO_BOOL possible_digit_allocate( 
    digit_tempinfo_t        *tempinfo,
    const CRYPTO_CHAR       *name)
/*
        Look at tempinfo.address.  If this has a non-NULL
        pointer, assume this array has the required space for
        temporaries.  If tempinfo.address is NULL
        and tempinfo.nelmt > 0, then allocate a temporary array,
        storing its address in tempinfo.address, and set tempinfo.need_to_free.
*/
{
    CRYPTO_BOOL OK = TRUE;

    if (tempinfo->need_to_free) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "possible_digit_allocate");
               // Caller is required to clear flag this beforehand.
    }

    if (OK && tempinfo->address == NULL
           && tempinfo->nelmt != 0) {

        tempinfo->address = digit_allocate(tempinfo->nelmt,
                                           name);
        if (tempinfo->address == NULL) {
            OK = FALSE;
        } else {
            tempinfo->need_to_free = TRUE;
        }
    }
    return OK;
}  // end possible_digit_allocate
/*****************************************************************************/

