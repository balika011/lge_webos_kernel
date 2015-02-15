/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "stdlib.h"
#include "tzst.h"
#include "crypto_types.h"
#include "crypto_results.h"
#include "bignum/bigdefs.h"
#include "bignum/bignum.h"
#include "bignum/field.h"
#include "bignum/ecurve.h"
#include "drmeccp160.h"
#include "bignum/mprand.h"
//#include "drmsha1.h"



#define TABLE_SPACING 4

/* ============================================================================
    Extern Functions & Variable
============================================================================ */
extern void TzGetRandomBytes(UINT8 *pu1_buf, UINT32 u4_len);


static const CRYPTO_DWORD ecdw_q[5] = 
{
    0x141424F7, 0x31415926, 0x27182818, 0x01234567, 0x89ABCDEF
};
/* ecdw_q = 785963102379428822376694789446897396207498568951 */

static const CRYPTO_DWORD ecdw_order[5] = 
{
    0x28C2A675, 0xEC149044, 0x2716B26E, 0x01234567, 0x89ABCDEF
};
/* ecdw_order = 785963102379428822376693024881714957612686157429 */

static const CRYPTO_DWORD ecdw_a[5] = 
{
    0xEBE41497, 0x4780C009, 0x7632FF3D, 0xD277BCE8, 0x37A5ABCC
};
/* ecdw_a = 317689081251325503476317476413827693272746955927 */

static const CRYPTO_DWORD ecdw_b[5] = 
{
    0x9328239E, 0xD78FDEDF, 0x28E85F1A, 0x725E2F32, 0x0DD8DABF
};
/* ecdw_b = 79052896607878758718120572025718535432100651934 */

static const CRYPTO_DWORD ecdw_genx[5] = 
{
    0x109FA120, 0xBA38DAF0, 0x3510C07D, 0xD6A3A1E5, 0x8723947F
};
/* ecdw_genx = 771507216262649826170648268565579889907769254176 */

static const CRYPTO_DWORD ecdw_geny[5] = 
{
    0xA379936F, 0xD4ED7ACD, 0x8C3C5856, 0x1075522D, 0x44574491
};




/* ecdw_geny = 390157510246556628525279459266514995562533196655 */
#if 0
CRYPTO_VOID* bignum_alloc(const CRYPTO_DWORD cblen, struct bigctx_t *f_pBigCtx)
{
    CRYPTO_VOID *pbRet = NULL;

    DRMASSERT(  f_pBigCtx != NULL );

    if (CRYPTO_FAILED (DRM_STK_Alloc ((DRM_STACK_ALLOCATOR_CONTEXT*) f_pBigCtx, (CRYPTO_DWORD)cblen, &pbRet)))
    {
        return NULL;
    }

    return pbRet;
}

CRYPTO_VOID bignum_free(CRYPTO_VOID *pvMem, struct bigctx_t *f_pBigCtx)
{

    if ( pvMem != NULL &&  f_pBigCtx != NULL )
    {
        DRM_STK_Free( (DRM_STACK_ALLOCATOR_CONTEXT*) f_pBigCtx, pvMem );
    }
 
}
 
 
static CRYPTO_VOID endian_reverse_dwords( const CRYPTO_DWORD f_rgdwIn[], CRYPTO_DWORD f_rgdwOut[], CRYPTO_DWORD cdw )
{
    if( f_rgdwOut != f_rgdwIn )
    {
        memcpy( f_rgdwOut, f_rgdwIn, cdw*sizeof(CRYPTO_DWORD) );
    }

}
#endif
static CRYPTO_BOOL dw_to_modular(               
    const  CRYPTO_DWORD        *dwnum,
    const  CRYPTO_DWORD         lngdw,
           digit_t          *answer,      /* OUT, length LNGQ */
    const  mp_modulus_t     *modulo,
    struct bigctx_t         *f_pBigCtx    )
{
    digit_t temp1[LNGQ];
    CRYPTO_BOOL OK = FALSE;

    if( temp1 != NULL )
    {

        if (DWORDS_TO_DIGITS(lngdw) <= LNGQ)
        {        
            dwords_to_digits(dwnum, temp1, lngdw );
            OK = to_modular(temp1, DWORDS_TO_DIGITS(lngdw), answer, modulo, f_pBigCtx);
        }

        memset( temp1, 0, sizeof (digit_t) * LNGQ );
    }

    return OK;
} /* end dw_to_modular */

#if 0
CRYPTO_BOOL words_to_ecaffine( 
    const CRYPTO_DWORD *in, digit_t *P,        /* OUT */
    const ecurve_t  *E,
    struct bigctx_t *f_pBigCtx )  
/* map bits of "in" to a point on the curve such that the first n-1 bytes can be recovered

  NOTE: this is only supposed to work for my ec over FIELD_Q_MP and not intended to be
        used as a general purpose library function.    marcuspe

 NOTE -- We never return the point at infinity,
         so this may fail if the group has no other points,
         such as y^2 == x^3 - x - 1 (mod 3)
 NOTE -- A given point of order 2 (with y = 0) is twice as
         likely to be chosen as a given point of order > 2.
*/
{
    CRYPTO_BOOL found = FALSE;
    CRYPTO_INT tries;    
    flongest_t *t1;
    CRYPTO_DWORD *tmp; /*[LNGQDW] */

    tmp = (CRYPTO_DWORD*) bignum_alloc (sizeof (CRYPTO_DWORD) * LNGQDW + sizeof (flongest_t), f_pBigCtx);

    if( tmp == NULL )
    {
        return found;
    }

    t1 = (flongest_t*)(tmp + LNGQDW);
    memcpy( tmp, in,( LNGQDW-1 )*sizeof( digit_t ));
    switch(E->fdesc->ftype) {
    case FIELD_Q_MP:
    case FIELD_2_POLYNOMIAL: 
    case FIELD_2_NORMAL:
        for (tries = 0; 
            tries < 100 && !found;
            tries++) {

                tmp[LNGQDW-1] = tries; 
                if( !dw_to_modular( tmp, LNGQDW, P, E->fdesc->modulo, f_pBigCtx) )
                {
                    found = FALSE;
                    break;
                }
                if (E->fdesc->ftype == FIELD_Q_MP || Kiszero(P, E->fdesc, f_pBigCtx) )
                {
                    if (Kmuladd (P, P,   E->a, *t1, E->fdesc, NULL, f_pBigCtx) 
                    &&  Kmuladd (P, *t1, E->b, *t1, E->fdesc, NULL, f_pBigCtx))
                    /*      t1 = x^2 + a                      t1 = x^3 + ax + b */
                    {
                        /* fSquare must be volatile to work in optimized builds of some compilers */
                        volatile CRYPTO_BOOL fSquare = FALSE;
                        found = Kprime_sqrter(*t1, (P + E->fdesc->elng), 1, E->fdesc, ( CRYPTO_BOOL* )&fSquare, f_pBigCtx) && fSquare;/* Is x^3 + ax + b  a square? */
                        continue;
                    }
                    found = FALSE;
                    break;
                } 
                else 
                {   
                    found = FALSE;
                    break;
                }
            } /* for tries */
            break;
    } /* switch */

    OEM_SECURE_ZERO_MEMORY( tmp, sizeof (CRYPTO_DWORD) * LNGQDW + sizeof (flongest_t) );
    bignum_free (tmp, f_pBigCtx);
    return found;
} /* words_to_ecaffine */


static CRYPTO_BOOL ecaffine_to_dwords( digit_t *P, CRYPTO_DWORD *out, const ecurve_t  *E)
{
    /* inverts the mapping of dwords_to_ecaffine */
    CRYPTO_DWORD tmp[LNGQDW + (sizeof( digit_t ) * LNGQ)/sizeof( CRYPTO_DWORD )]; /*[LNGQDW]; */
    digit_t *digs; /*[LNGQ]; */

    if ( E != NULL &&
         E->fdesc != NULL &&
         E->fdesc->modulo != NULL &&
         E->fdesc->modulo->length == LNGQ &&
         tmp != NULL )
    {
        digs = (digit_t*)(tmp + LNGQDW );
        from_modular ( P, digs, E->fdesc->modulo);
        digits_to_dwords( digs, tmp, LNGQDW );
        endian_reverse_dwords( (const CRYPTO_DWORD *)tmp, out, LNGQDW-1 );

        OEM_SECURE_ZERO_MEMORY( tmp, sizeof(CRYPTO_DWORD) * LNGQDW + sizeof( digit_t ) * LNGQ );
        return( TRUE );
    }

    return FALSE;
}
#endif   

#if defined( _PREFAST_ )
#pragma prefast(suppress:22112) /* security mitigation warning */
#endif
static CRYPTO_VOID _PKLock();
static CRYPTO_VOID _PKUnlock();

#if DRM_THREAD_SAFE
#define USEPKLOCK CRYPTO_BOOL fLocked = FALSE;
#define PKLOCK {_PKLock();fLocked=TRUE;}
#define PKUNLOCK {if(fLocked){_PKUnlock();}}
#else
#define USEPKLOCK
#define PKLOCK
#define PKUNLOCK
#endif

static CRYPTO_RESULT ec_init( 
    PK_DATA         *pEc, 
    const CRYPTO_DWORD *q, 
    const CRYPTO_DWORD *order, 
    const CRYPTO_DWORD *a, 
    const CRYPTO_DWORD *b, 
    const CRYPTO_DWORD *genx, 
    const CRYPTO_DWORD *geny, 
    struct bigctx_t *f_pBigCtx) 
{
    CRYPTO_RESULT dr = CRYPTO_SUCCESS;

    ChkArg( pEc   != NULL 
         || q     != NULL 
         || order != NULL 
         || a     != NULL 
         || b     != NULL 
         || genx  != NULL 
         || geny  != NULL );
    
    dwords_to_digits( q, pEc->q, LNGQDW );
    dwords_to_digits( order, pEc->r, LNGQDW );

    ChkBOOL( create_modulus( pEc->q, LNGQ, FROM_RIGHT, &pEc->qmodulus, f_pBigCtx, (struct bigctx_t*)&pEc->pkdHeap ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( a, LNGQDW, pEc->a, &pEc->qmodulus, f_pBigCtx ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( b, LNGQDW, pEc->b, &pEc->qmodulus, f_pBigCtx ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( Kinitialize_prime( &pEc->qmodulus, &pEc->fdesc, f_pBigCtx , (struct bigctx_t*)&pEc->pkdHeap ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( ec_initialize( pEc->a, pEc->b, &pEc->fdesc, &pEc->ecurve, f_pBigCtx, (struct bigctx_t*)&pEc->pkdHeap ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( ecdw_genx, LNGQDW, pEc->g, &pEc->qmodulus, f_pBigCtx ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( ecdw_geny, LNGQDW, pEc->g + LNGQ, &pEc->qmodulus, f_pBigCtx ), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( ecaffine_on_curve(pEc->g, &pEc->ecurve, NULL, NULL, f_pBigCtx), CRYPTO_E_PKCRYPTO_FAILURE);
    ChkBOOL( ecaffine_table_construction( pEc->g, TABLE_SPACING, TABLE_LAST, pEc->TABLE, &pEc->ecurve, f_pBigCtx ), CRYPTO_E_PKCRYPTO_FAILURE);

    if (pEc != NULL) {
        pEc->lngr = LNGQ;
        divide_precondition_1( pEc->r, pEc->lngr, &pEc->rrecip );
    }

ErrorExit:
    return dr;
}


CRYPTO_RESULT CRYPTO_ECC_ThreadUnSafePKInit( 
    CRYPTO_VOID* f_pContext, 
    CRYPTO_VOID* f_pOEMContext )

{
    CRYPTO_RESULT dr = CRYPTO_E_PKCRYPTO_FAILURE;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT*) f_pContext;

    DRMCASSERT( sizeof( DRMBIGNUM_CONTEXT_STRUCT ) <= DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE );
    
    /* We need to check that big num stack is in good shape. 
    ** It is not trivial to check if it is initialized or not.
    ** I found taht sometimes this object is used as temporary scratch pad
    ** and pContext->oHeap might be in invalid state
    ** We make the check based on consistent value of cbStack and pbStack
    */
    if (    pContext->oHeap.cbStack != DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE 
         || pContext->oHeap.pbStack != pContext->rgbHeap
       )
    { 
        pContext->oHeap.nStackTop = 0;
        pContext->oHeap.cbStack = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
        pContext->oHeap.pbStack = pContext->rgbHeap;
    }
    
    if( pContext->fInited )
    {
        return CRYPTO_SUCCESS;
    }

    
    pContext->pOEMContext = f_pOEMContext;
    
    memset( (CRYPTO_BYTE*)&pContext->pkData, 0, sizeof( pContext->pkData ));    
    
    pContext->pkData.pkdHeap.nStackTop = 0;
    pContext->pkData.pkdHeap.cbStack = PK_DATA_HEAP_SIZE;
    pContext->pkData.pkdHeap.pbStack = pContext->pkData.rgbHeap;

    dr = ec_init(&pContext->pkData, ecdw_q, ecdw_order, ecdw_a, ecdw_b, ecdw_genx, ecdw_geny, (struct bigctx_t*)&pContext->oHeap);
    if( CRYPTO_SUCCEEDED( dr ) )
    {
        pContext->fInited = TRUE;
    }
    
    return dr;
}

CRYPTO_VOID DRM_RSA_PKInit(
            CRYPTO_VOID *f_pContext )
{
    DRM_RSA_PKInitWithSize( f_pContext, (CRYPTO_DWORD)sizeof(DRMBIGNUM_CONTEXT_STRUCT) );
}

CRYPTO_VOID DRM_RSA_PKInitWithSize(
            CRYPTO_VOID *f_pContext,
            CRYPTO_DWORD f_dwSize )
{
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT*) f_pContext;

    DRMASSERT( f_dwSize >= sizeof(DRMBIGNUM_CONTEXT_STRUCT) );

    f_dwSize -= sizeof( DRMBIGNUM_CONTEXT_STRUCT );
    f_dwSize += DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
    f_dwSize += sizeof( PK_DATA );

    if (    pContext->oHeap.cbStack != f_dwSize
         || pContext->oHeap.pbStack != pContext->rgbHeap
       )
    { 
        pContext->oHeap.nStackTop = 0;
        pContext->oHeap.cbStack = f_dwSize;
        pContext->oHeap.pbStack = pContext->rgbHeap;
    }
    pContext->fInited = TRUE;
}

#if 0
/*********************************************************************
**
**  Function:  CRYPTO_ECC_GenKeyPair_P160
**
**  Synopsis:  Generate a matched public and private key pair
**
**  Arguments:  
**     [f_pContext] -- Pointer to context the size of CRYPTO_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]  -- New public key
**     [f_pprivkey] -- New private key
**
*********************************************************************/
CRYPTO_RESULT CRYPTO_ECC_GenKeyPair_P160( 
            CRYPTO_VOID *f_pContext,
            PUBKEY   *f_ppubkey,
            PRIVKEY  *f_pprivkey )
{
    CRYPTO_RESULT dr = CRYPTO_E_PKCRYPTO_FAILURE;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    struct bigctx_t *pBigCtx                                                            = NULL;
    digit_t    tmp[2 * LNGQ + ( sizeof( digit_t ) * 2 *LNGQ )/sizeof(digit_t)]          = {0}; /* [LNGQ] */
    digit_t   *priv                                                                     = NULL; /* [LNGQ] */
    digit_t   *pub                                                                      = NULL; /* [2*LNGQ] */
    
    
    USEPKLOCK;

    ChkArg( f_pContext != NULL 
         && f_ppubkey  != NULL 
         && f_pprivkey != NULL );

    DRMASSERT( (CRYPTO_DWORD_PTR)f_ppubkey % sizeof( CRYPTO_DWORD ) == 0 );
    DRMASSERT( (CRYPTO_DWORD_PTR)f_pprivkey % sizeof( CRYPTO_DWORD ) == 0 );

    ChkDR( CRYPTO_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
    pContext->oHeap.pbStack = pContext->rgbHeap;

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    priv = (digit_t*)(tmp  + LNGQ);
    pub  = (digit_t*)(priv + LNGQ);
    
    random_mod_nonzero( pContext->pkData.q, priv, LNGQ, pBigCtx );
    if( ecaffine_exponentiation_tabular( pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, priv, LNGQ, pub, &(pContext->pkData.ecurve), pBigCtx ) )
    {

        digits_to_dwords( priv, (CRYPTO_DWORD*)f_pprivkey->x, LNGQDW );
        endian_reverse_dwords( (CRYPTO_DWORD*)f_pprivkey->x, (CRYPTO_DWORD*)f_pprivkey->x, DRM_ECC160_PRIVATE_KEY_LEN / sizeof( CRYPTO_DWORD ) );
        
        from_modular ( pub, tmp, &(pContext->pkData.qmodulus) );
        digits_to_dwords( tmp, (CRYPTO_DWORD*)f_ppubkey->y, LNGQDW ); 

        from_modular ( pub+LNGQ, tmp, &(pContext->pkData.qmodulus) );
        digits_to_dwords( tmp, ((CRYPTO_DWORD*)f_ppubkey->y) + LNGQDW, LNGQDW ); 
        endian_reverse_dwords( (CRYPTO_DWORD*)f_ppubkey->y, (CRYPTO_DWORD *)f_ppubkey->y, DRM_ECC160_PUBLIC_KEY_LEN / sizeof( CRYPTO_DWORD ) );

        dr = CRYPTO_SUCCESS;
    }

    OEM_SECURE_ZERO_MEMORY( tmp, (sizeof ( digit_t ) * 2 * LNGQ) + ( sizeof( digit_t ) * 2 *LNGQ ) );

ErrorExit:
    PKUNLOCK;
   

    
    return dr;    
}


/*********************************************************************
**
**  Function:  CRYPTO_ECC_Encrypt_P160
**
**  Synopsis:  Encrypt a block of data with a given public key.
**
**  Arguments:  
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]  -- Public key to encrypt with
**     [f_rgbIn]    -- Data to be encrypted
**     [f_rgbOut]   -- Buffer to hold the encrypted data value.
**
**  Notes:  Data is not encrpyted in place.  It is put in the rgbOut buffer.
*********************************************************************/
CRYPTO_RESULT CRYPTO_ECC_Encrypt_P160( 
            CRYPTO_VOID *f_pContext, 
            const PUBKEY   *f_ppubkey,
            const CRYPTO_BYTE  f_rgbIn[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)],
            CRYPTO_BYTE  f_rgbOut[__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)] )
{
    CRYPTO_RESULT dr = CRYPTO_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    CRYPTO_DWORD rgdwPubKey[ DRM_ECC160_PUBLIC_KEY_LEN / sizeof( CRYPTO_DWORD )  ];
    digit_t    k[LNGQ + ( sizeof(digit_t) * 5*2*LNGQ )/sizeof(digit_t)]; /* [LNGQ]   */
    struct bigctx_t *pBigCtx = NULL;
    digit_t *a   = NULL; /* [2*LNGQ] */
    digit_t *b   = NULL; /* [2*LNGQ] */
    digit_t *tmp = NULL; /* [2*LNGQ] */
    digit_t *ecM = NULL; /* [2*LNGQ] */
    digit_t *pk  = NULL; /* [2*LNGQ] */
    USEPKLOCK;

    ChkArg( f_pContext    != NULL
         && f_ppubkey     != NULL
         && f_rgbIn       != NULL
         && f_rgbOut      != NULL );

    DRMASSERT( (CRYPTO_DWORD_PTR)f_ppubkey % sizeof( CRYPTO_DWORD ) == 0 );

    ChkDR( CRYPTO_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack   = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
    pContext->oHeap.pbStack   = pContext->rgbHeap;

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    a   = (digit_t*)(k   + LNGQ);
    b   = (digit_t*)(a   + 2 * LNGQ);
    tmp = (digit_t*)(b   + 2 * LNGQ);
    ecM = (digit_t*)(tmp + 2 * LNGQ);
    pk  = (digit_t*)(ecM + 2 * LNGQ);

    endian_reverse_dwords( (const CRYPTO_DWORD*)f_ppubkey->y, rgdwPubKey, DRM_ECC160_PUBLIC_KEY_LEN / sizeof( CRYPTO_DWORD ) );

    ChkArg( pContext->pkData.qmodulus.length == LNGQ );

    dw_to_modular( rgdwPubKey,          LNGQDW, pk,      &(pContext->pkData.qmodulus), pBigCtx );
    dw_to_modular( rgdwPubKey + LNGQDW, LNGQDW, pk+LNGQ, &(pContext->pkData.qmodulus), pBigCtx );

    if( !ecaffine_on_curve(pk, &(pContext->pkData.ecurve), NULL, NULL, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    /* Do a byte copy from the input buffer to the output buffer since
    ** only the output buffer is guaranteed to be DWORD aligned
    */
    memcpy( f_rgbOut, f_rgbIn, DRM_ECC160_PLAINTEXT_LEN );
    
    endian_reverse_dwords( (const CRYPTO_DWORD*)f_rgbOut, (CRYPTO_DWORD*)f_rgbOut, DRM_ECC160_PLAINTEXT_LEN / sizeof(CRYPTO_DWORD) );
    if( !words_to_ecaffine( (CRYPTO_DWORD *)f_rgbOut, ecM, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    random_mod_nonzero( pContext->pkData.q, k, LNGQ, pBigCtx );

    if( !ecaffine_exponentiation_tabular( pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, k, LNGQ, a, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_on_curve( pk, &pContext->pkData.ecurve, NULL, NULL, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_exponentiation_tabular( pk, TABLE_SPACING, 0, k, LNGQ, tmp, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_addition( tmp, ecM, b, 1, &(pContext->pkData.ecurve), NULL, pBigCtx  ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }


    from_modular ( a, tmp, &(pContext->pkData.qmodulus) );
    digits_to_dwords( tmp, (CRYPTO_DWORD *)f_rgbOut, LNGQDW ); 
    from_modular ( a+LNGQ, tmp, &pContext->pkData.qmodulus );
    digits_to_dwords( tmp, ((CRYPTO_DWORD *)f_rgbOut)+LNGQDW, LNGQDW ); 

    from_modular ( b, tmp, &pContext->pkData.qmodulus );
    digits_to_dwords( tmp, ((CRYPTO_DWORD *)f_rgbOut) + 2*LNGQDW, LNGQDW ); 
    from_modular ( b+LNGQ, tmp, &pContext->pkData.qmodulus );
    digits_to_dwords( tmp, ((CRYPTO_DWORD *)f_rgbOut) + 3*LNGQDW, LNGQDW ); 

    endian_reverse_dwords( (CRYPTO_DWORD *)f_rgbOut, (CRYPTO_DWORD*)f_rgbOut, 4*LNGQDW );

ErrorExit:
    PKUNLOCK;
    if( k )
    {
        OEM_SECURE_ZERO_MEMORY( k, (sizeof( digit_t) * LNGQ ) + ( sizeof (digit_t) * 5*2*LNGQ ) );
    }
    return dr;
}


/*********************************************************************
**
**  Function:  CRYPTO_ECC_Decrypt_P160
**
**  Synopsis:  
**
**  Arguments:  
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_pprivkey] -- Private key to decrypt with
**     [f_rgbIn]    -- Encrypted bytes that are to be decrypted
**     [f_rgbOut]   -- Clear text result
**
**  Notes:  Data is not decrpyted in place.  It is put in the rgbOut buffer.
**
*********************************************************************/
CRYPTO_RESULT CRYPTO_ECC_Decrypt_P160(
            CRYPTO_VOID *f_pContext,
            const PRIVKEY  *f_pprivkey,
            const CRYPTO_BYTE  f_rgbIn[__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)],
            CRYPTO_BYTE  f_rgbOut[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)] )
{
    CRYPTO_RESULT dr = CRYPTO_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    CRYPTO_DWORD rgdwPrivKey[ DRM_ECC160_PRIVATE_KEY_LEN / sizeof( CRYPTO_DWORD ) ];
    CRYPTO_DWORD rgdwIn     [DRM_ECC160_CIPHERTEXT_LEN / sizeof( CRYPTO_DWORD )];
    CRYPTO_DWORD rgdwOut    [DRM_ECC160_PLAINTEXT_LEN / sizeof( CRYPTO_DWORD )];
    digit_t *a   = NULL; /* [2*LNGQ] */
    digit_t *b   = NULL; /* [2*LNGQ] */
    digit_t *tmp = NULL; /* [2*LNGQ] */
    digit_t *ecM = NULL; /* [2*LNGQ] */
    digit_t    pk[(sizeof( digit_t) * LNGQ ) + ( sizeof (digit_t) * 4*2*LNGQ )] = {0}; /* [LNGQ]   */
    struct bigctx_t *pBigCtx = NULL;


    USEPKLOCK;
    
    ChkArg( f_pContext    != NULL
         && f_pprivkey    != NULL 
         && f_rgbIn       != NULL 
         && f_rgbOut      != NULL );

    DRMASSERT( (CRYPTO_DWORD_PTR)f_pprivkey % sizeof( CRYPTO_DWORD ) == 0 );

    ChkDR( CRYPTO_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack   = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
    pContext->oHeap.pbStack   = pContext->rgbHeap;

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    a   = (digit_t*)(pk  +     LNGQ);
    b   = (digit_t*)(a   + 2 * LNGQ);
    tmp = (digit_t*)(b   + 2 * LNGQ);
    ecM = (digit_t*)(tmp + 2 * LNGQ);


    endian_reverse_dwords( (const CRYPTO_DWORD *)f_pprivkey->x, rgdwPrivKey, DRM_ECC160_PRIVATE_KEY_LEN / sizeof( CRYPTO_DWORD ) );
    dwords_to_digits( rgdwPrivKey, pk, LNGQDW );

    /* Do a byte copy from the input buffer to the local buffer since
    ** only the local buffer is guaranteed to be DWORD aligned
    */
    memcpy( rgdwIn, f_rgbIn, DRM_ECC160_CIPHERTEXT_LEN );

    endian_reverse_dwords( (const CRYPTO_DWORD *)rgdwIn, rgdwIn, DRM_ECC160_CIPHERTEXT_LEN / sizeof(CRYPTO_DWORD) );

    dw_to_modular( rgdwIn,          LNGQDW, a,      &pContext->pkData.qmodulus, pBigCtx );
    dw_to_modular( rgdwIn + LNGQDW, LNGQDW, a+LNGQ, &pContext->pkData.qmodulus, pBigCtx );
    dw_to_modular( rgdwIn+2*LNGQDW, LNGQDW, b,      &pContext->pkData.qmodulus, pBigCtx );
    dw_to_modular( rgdwIn+3*LNGQDW, LNGQDW, b+LNGQ, &pContext->pkData.qmodulus, pBigCtx );

    if( !ecaffine_on_curve( a, &pContext->pkData.ecurve, NULL, NULL, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_exponentiation_tabular( a, TABLE_SPACING, 0, pk, LNGQ, tmp, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( !ecaffine_addition( b, tmp, ecM, -1, &(pContext->pkData.ecurve), NULL, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( !ecaffine_to_dwords( ecM, rgdwOut, &(pContext->pkData.ecurve) ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    memcpy( f_rgbOut, rgdwOut, DRM_ECC160_PLAINTEXT_LEN );
    OEM_SECURE_ZERO_MEMORY( rgdwOut, DRM_ECC160_PLAINTEXT_LEN );
    
ErrorExit:
    PKUNLOCK;
    if( pContext )
    {
        if( pk )
        {
            OEM_SECURE_ZERO_MEMORY( pk, (sizeof( digit_t) * LNGQ ) + ( sizeof (digit_t) * 4*2*LNGQ ) );
        }
        
    }


    
    return( dr );
}

#define SIGN_BUF_LEN  (LNGQDW*sizeof(CRYPTO_DWORD))

static CRYPTO_BOOL byte_array_mod_bignum(
            const  CRYPTO_BYTE     buf[__CB_DECL(SIGN_BUF_LEN)],                                  
            digit_t      remainder[],
            PK_DATA     *pPKData )               /* Given a message to be signed, in byte form reduce it modulo pkData.r[].  */
{

#define PACKED_CHAR_BIT (CHAR_BIT/CB_NATIVE_BYTE)
#define BYTES_PER_DIGIT (RADIX_BITS/PACKED_CHAR_BIT)

#if (RADIX_BITS % CHAR_BIT != 0) 
#error "Radix_bits not multiple of CHAR_BIT"
#endif    
    digit_t dividend[SIGN_BUF_LEN]; /*[SIGN_BUF_LEN] */
    CRYPTO_DWORD ib;

    if( dividend == NULL )
    {
        return FALSE;
    }

    memset(dividend, 0, SIGN_BUF_LEN*sizeof( digit_t ));
        
    for (ib = 0; ib < SIGN_BUF_LEN; ib++) 
    {
        const CRYPTO_DWORD idigit = ib/BYTES_PER_DIGIT;
        const CRYPTO_DWORD ishift = PACKED_CHAR_BIT*ib - RADIX_BITS*idigit;
        dividend[idigit] |= ((digit_t)GET_BYTE(buf,ib)) << ishift;
    }

    /* Do the division, discard quotient. */
    divide(dividend, SIGN_BUF_LEN, pPKData->r, pPKData->lngr, &pPKData->rrecip, NULL, remainder);
    OEM_SECURE_ZERO_MEMORY( dividend, SIGN_BUF_LEN * sizeof( digit_t ) );
    return TRUE;
} /* end byte_array_mod_bignum */

static CRYPTO_BOOL FE2IPmod(
    const digit_t     *fdata,
    const field_desc_t    *fdesc,
    const digit_t          divisor[],
    const CRYPTO_DWORD        ldivisor,
    const reciprocal_1_t  *recip,
    digit_t                remainder[],    /* OUT */
    CRYPTO_VOID              *pContext )
    /* Convert (i.e., cast) a field element to an integer, */
    /* reduce the value modulo divisor.  FE2IP is a P1363 function. */
    /* TBD -- Do we need byte reversal? */
{
    CRYPTO_BOOL OK = TRUE;
    digit_t *mdata; /*[MP_LONGEST]; */
    const digit_t *data;
    struct bigctx_t *pBigCtx = (struct bigctx_t*) pContext;

    mdata = (digit_t*)bignum_alloc( sizeof( digit_t ) * MP_LONGEST, pBigCtx );
    if( mdata == NULL )
    {
        return FALSE;
    }

    if (fdesc->ftype == FIELD_Q_MP)
    {
        from_modular(fdata, mdata, fdesc->modulo);    /* Get standard representation */
        data = mdata;
    }
    else
    {  /* Characteristic 2 */
        data = fdata;
        if( !(mp_significant_bit_count(fdata, fdesc->elng) <= fdesc->degree) )
        {
            OK = FALSE;
            /* Leading zeros expected */
        }        
    }
    if( OK )
    {
        divide(data, fdesc->elng, divisor, ldivisor, recip, NULL, remainder);
    }
    OEM_SECURE_ZERO_MEMORY( mdata, sizeof( digit_t ) * MP_LONGEST );
    bignum_free( mdata, pBigCtx );
    return OK;
} /* end FE2IPmod */



/*********************************************************************
**
**  Function:  CRYPTO_ECC_Sign_P160
**
**  Synopsis:  Generate a digital signature with a private key
**
**  Arguments:  
**     [f_pContext]     -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_privkey]      -- Private key to create a signature with
**     [f_pbData]       -- Array of bytes to create a signature over
**     [f_cbData]       -- Length of pbBuffer in bytes
**     [f_rgbSignature] -- Buffer to hold result signature
*********************************************************************/
CRYPTO_RESULT CRYPTO_ECC_Sign_P160(
            CRYPTO_VOID  *f_pContext,
            const PRIVKEY   *f_privkey,
            const CRYPTO_BYTE  *f_pbData,
            CRYPTO_DWORD  f_cbData,
            CRYPTO_BYTE   f_rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] )
{    
    CRYPTO_RESULT dr = CRYPTO_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    CRYPTO_DWORD rgdwPrivKey[ DRM_ECC160_PRIVATE_KEY_LEN / sizeof( CRYPTO_DWORD )];
    CRYPTO_DWORD ntry = 0;
    CRYPTO_DWORD    lgcd   = 0;
    digit_t  *Gpriv2    = NULL;  /* [2*MP_LONGEST] */
    digit_t  *gcd       = NULL;  /* [MP_LONGEST] */
    digit_t  *priv2     = NULL;  /* [MP_LONGEST] */
    digit_t  *priv2inv  = NULL;  /* [MP_LONGEST] */
    digit_t  *sigc      = NULL;  /* [LNGQ] */
    digit_t  *sigd      = NULL;  /* [LNGQ] */
    digit_t  *privexpon = NULL;  /* [LNGQ] */
    CRYPTO_BYTE *buffer    = NULL;  /* [SIGN_BUF_LEN]     */
    struct bigctx_t *pBigCtx = NULL;
    
  
    
    USEPKLOCK;  
    
    ChkArg( f_pContext     != NULL 
         && f_privkey      != NULL 
         && f_pbData       != NULL 
         && f_cbData       != 0 
         && f_rgbSignature != NULL );

    DRMASSERT( (CRYPTO_DWORD_PTR)f_privkey % sizeof( CRYPTO_DWORD ) == 0 );

    ChkDR( CRYPTO_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack   = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
    pContext->oHeap.pbStack   = pContext->rgbHeap;

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    ChkMem( gcd = (digit_t*)bignum_alloc( (sizeof( digit_t) * (3*LNGQ + 3*MP_LONGEST) ) 
                                         +(sizeof(digit_t) * 2*MP_LONGEST ) 
                                         +SIGN_BUF_LEN,
                                         pBigCtx ) );

    priv2     = gcd      + MP_LONGEST;
    priv2inv  = priv2    + MP_LONGEST;
    sigc      = priv2inv + MP_LONGEST;
    sigd      = sigc     + LNGQ;
    privexpon = sigd     + LNGQ;

    Gpriv2    = (digit_t*)(privexpon + LNGQ);
    buffer    = (CRYPTO_BYTE*)(Gpriv2      + 2 * MP_LONGEST);

    endian_reverse_dwords( (const CRYPTO_DWORD *)f_privkey->x, rgdwPrivKey, DRM_ECC160_PRIVATE_KEY_LEN / sizeof( CRYPTO_DWORD ) );
    dwords_to_digits( rgdwPrivKey, privexpon, LNGQDW );

    {
        DRM_SHA_CONTEXT shadata;

        memset( buffer, 0, SIGN_BUF_LEN );

        ChkDR( CRYPTO_SHA_Init( &shadata, eDRM_SHA_1 ) );
        ChkDR( CRYPTO_SHA_Update( (CRYPTO_BYTE*) f_pbData, f_cbData, &shadata ) );
        ChkDR( CRYPTO_SHA_Finalize( &shadata, DRM_SHA1_DIGEST_LEN, buffer ) );
    }

    for( ntry = 0; ntry < 1000; ntry++ )
    {
        random_mod_nonzero( pContext->pkData.q, priv2, LNGQ, pBigCtx );
        
        if( !ecaffine_exponentiation_tabular( pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, priv2, LNGQ, Gpriv2, &pContext->pkData.ecurve, pBigCtx ) )
        {
            ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
        }

        if( ecaffine_is_infinite(Gpriv2, &pContext->pkData.ecurve, pBigCtx ) )
        {
            break;
        }

        if( !FE2IPmod(Gpriv2, pContext->pkData.ecurve.fdesc, pContext->pkData.r, pContext->pkData.lngr, &pContext->pkData.rrecip, sigc, pBigCtx ) )
        {
            break;
        }

        if( compare_immediate(gcd, 1, mp_gcdex(priv2, pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr,  priv2inv, NULL, gcd, NULL, &lgcd, NULL, pBigCtx)) != 0 )
        {
            break;
        }

        multiply(sigc, pContext->pkData.lngr, privexpon, pContext->pkData.lngr, Gpriv2);   
        if( !byte_array_mod_bignum(buffer, gcd, &pContext->pkData ) )
        {
            break;
        }
        
        add_diff(Gpriv2, 2*pContext->pkData.lngr, gcd, pContext->pkData.lngr, Gpriv2, NULL);  /* Overflow impossible */
        divide(Gpriv2, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &pContext->pkData.rrecip, NULL, gcd);
        multiply(priv2inv, pContext->pkData.lngr, gcd, pContext->pkData.lngr, Gpriv2);
        divide(Gpriv2, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &pContext->pkData.rrecip, NULL, sigd);            
        
        /* TBD -- should check more error codes */

        if( significant_digit_count(sigc,  pContext->pkData.lngr) != 0 
         && significant_digit_count(sigd,  pContext->pkData.lngr) != 0 )
        {
            break;
        }

    }

    memset(priv2, 0, pContext->pkData.lngr*sizeof( digit_t ));    /* P1363 recommends this for security */
    memset(priv2inv, 0, pContext->pkData.lngr*sizeof( digit_t ));
    memset(Gpriv2, 0, MAX(2*pContext->pkData.lngr, 2*pContext->pkData.ecurve.fdesc->elng)*sizeof( digit_t ));

    digits_to_dwords( sigc, (CRYPTO_DWORD *)f_rgbSignature, LNGQDW );
    digits_to_dwords( sigd, ((CRYPTO_DWORD *)f_rgbSignature) + LNGQDW, LNGQDW );

    endian_reverse_dwords( (const CRYPTO_DWORD *)f_rgbSignature, (CRYPTO_DWORD*) f_rgbSignature, 2*LNGQDW );

ErrorExit:
    PKUNLOCK;
    if( pContext )
    {
        if( gcd )
        {
            OEM_SECURE_ZERO_MEMORY( gcd, (sizeof( digit_t) * (3*LNGQ + 3*MP_LONGEST) ) + (sizeof(digit_t) * 2*MP_LONGEST ) + SIGN_BUF_LEN );
        }
        bignum_free( gcd, pBigCtx );
    }

        
    return dr;
} /* end CRYPTO_ECC_Sign_P160 */


/*********************************************************************
**
**  Function:  CRYPTO_ECC_Verify_P160
**
**  Synopsis:  Verify a digital signature created by CRYPTO_ECC_Sign_P160.
**
**  Arguments:  
**     [f_pContext]     -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]      -- Pubkey to check the signature with
**     [f_pbData]       -- Data buffer that the signature was created over
**     [f_cbData]       -- Length of pbBuffer in bytes
**     [f_rgbSignature] -- The signature to verify
**
**  Returns:  TRUE if the signature verified correctly.  FALSE is it didn't
**
*********************************************************************/
CRYPTO_BOOL CRYPTO_ECC_Verify_P160( 
            CRYPTO_VOID  *f_pContext,
            const PUBKEY    *f_ppubkey, 
            const CRYPTO_BYTE  *f_pbData, 
            CRYPTO_DWORD  f_cbData, 
            const CRYPTO_BYTE   f_rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] )
{    
    CRYPTO_RESULT dr = CRYPTO_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    CRYPTO_DWORD rgdwPubKey[ DRM_ECC160_PUBLIC_KEY_LEN / sizeof( CRYPTO_DWORD ) ];
    CRYPTO_DWORD rgdwSignature[ 2*LNGQDW ];
    CRYPTO_DWORD   lgcd     = 0;
    digit_t    *sigdinv  = NULL; /* [MP_LONGEST] */
    digit_t    *gcd      = NULL; /* [MP_LONGEST] */
    digit_t    *sigc     = NULL; /* [LNGQ] */
    digit_t    *sigd     = NULL; /* [LNGQ] */
    digit_t    *h1G      = NULL; /* [2*MP_LONGEST] */
    digit_t    *h2pubkey = NULL; /* [2*MP_LONGEST] */
    digit_t    *pubkey   = NULL; /* [2*LNGQ] */
    CRYPTO_BYTE   *buffer   = NULL; /* [SIGN_BUF_LEN] */
    struct bigctx_t *pBigCtx  = NULL;
    const ecurve_t  *ecurve   = NULL;
    
    
    USEPKLOCK;
    
    ChkArg( f_pContext     != NULL 
         && f_ppubkey      != NULL 
         && f_pbData       != NULL 
         && f_cbData       != 0 
         && f_rgbSignature != NULL );

    DRMASSERT( (CRYPTO_DWORD_PTR)f_ppubkey % sizeof( CRYPTO_DWORD ) == 0 );

    ChkDR( CRYPTO_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack   = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE;
    pContext->oHeap.pbStack   = pContext->rgbHeap;

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    ChkMem( sigdinv = (digit_t*)bignum_alloc( (sizeof( digit_t) * (2*LNGQ + 2*MP_LONGEST) ) 
                                             +(sizeof(digit_t) * (2*2*MP_LONGEST + 2*LNGQ) )
                                             + SIGN_BUF_LEN, 
                                             pBigCtx ) );

    gcd      = sigdinv  + MP_LONGEST;
    sigc     = gcd      + MP_LONGEST;
    sigd     = sigc     + LNGQ;
    h1G      = (digit_t*)(sigd     +  LNGQ);
    h2pubkey = h1G      + 2*MP_LONGEST;
    pubkey   = h2pubkey + 2*MP_LONGEST;
    buffer   = (CRYPTO_BYTE*)(pubkey   +2*LNGQ);

    ecurve = &pContext->pkData.ecurve;

    endian_reverse_dwords( (const CRYPTO_DWORD *)f_rgbSignature, rgdwSignature, 2*LNGQDW );

    dwords_to_digits( rgdwSignature,          sigc, LNGQDW );
    dwords_to_digits( rgdwSignature + LNGQDW, sigd, LNGQDW );

    endian_reverse_dwords( (const CRYPTO_DWORD *)f_ppubkey->y, rgdwPubKey, DRM_ECC160_PUBLIC_KEY_LEN / sizeof( CRYPTO_DWORD ) );

    dw_to_modular( rgdwPubKey,        LNGQDW, pubkey,      &(pContext->pkData.qmodulus), pBigCtx );
    dw_to_modular( rgdwPubKey+LNGQDW, LNGQDW, pubkey+LNGQ, &(pContext->pkData.qmodulus), pBigCtx );


    {
        DRM_SHA_CONTEXT shadata;

        memset( buffer, 0, SIGN_BUF_LEN);

        ChkDR( CRYPTO_SHA_Init( &shadata, eDRM_SHA_1 ) );
        ChkDR( CRYPTO_SHA_Update( (CRYPTO_BYTE*)f_pbData, f_cbData, &shadata ) );
        ChkDR( CRYPTO_SHA_Finalize( &shadata, DRM_SHA1_DIGEST_LEN, buffer ) ); 
    }


    if( !ecaffine_on_curve(pubkey, ecurve, NULL, NULL, pBigCtx) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( significant_digit_count(sigc,  pContext->pkData.lngr) == 0 
     && significant_digit_count(sigd,  pContext->pkData.lngr) != 0 )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( compare_same(sigc, pContext->pkData.r, pContext->pkData.lngr) >= 0 )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( compare_same(sigd, pContext->pkData.r, pContext->pkData.lngr) >= 0 )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }


    if( compare_immediate(gcd, 1, mp_gcdex(sigd, pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, sigdinv, NULL, gcd, NULL, &lgcd, NULL, pBigCtx)) != 0 )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( !byte_array_mod_bignum(buffer, gcd, &pContext->pkData) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    multiply(sigdinv, pContext->pkData.lngr, gcd, pContext->pkData.lngr, h1G);      
    divide(h1G, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &(pContext->pkData.rrecip), NULL, gcd);
    
    if( !ecaffine_exponentiation_tabular(pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, gcd, pContext->pkData.lngr, h1G, ecurve, pBigCtx) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    multiply(sigdinv, pContext->pkData.lngr, sigc, pContext->pkData.lngr, h2pubkey);
    divide(h2pubkey, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &(pContext->pkData.rrecip), NULL, gcd);

    if( !ecaffine_on_curve( pubkey, ecurve, NULL, NULL, pBigCtx) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( !ecaffine_exponentiation_tabular(pubkey, TABLE_SPACING, 0, gcd, pContext->pkData.lngr, h2pubkey, ecurve, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    
    ecaffine_addition(h1G, h2pubkey, h1G, +1, ecurve, NULL, pBigCtx );
    
    if( ecaffine_is_infinite(h1G, ecurve, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }
    if( !FE2IPmod(h1G, ecurve->fdesc, pContext->pkData.r, pContext->pkData.lngr, &(pContext->pkData.rrecip), gcd, pBigCtx ) )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

    if( compare_same(sigc, gcd, pContext->pkData.lngr) != 0 )
    {
        ChkDR( CRYPTO_E_PKCRYPTO_FAILURE );
    }

ErrorExit:
    PKUNLOCK;
    if( pContext )
    {
        if( sigdinv )
        {
            OEM_SECURE_ZERO_MEMORY( sigdinv, (sizeof( digit_t) * (2*LNGQ + 2*MP_LONGEST) ) + (sizeof(digit_t) * (2*2*MP_LONGEST + 2*LNGQ) ) + SIGN_BUF_LEN );
        }
        bignum_free( sigdinv, pBigCtx );
    }
    

        
    return DRM_SUCCEEDED( dr );
} /* end ecvp_dsa */

CRYPTO_BOOL random_bytes(
    CRYPTO_BYTE* byte_array, 
    const CRYPTO_DWORD nbyte )
{
    /*
    ** BIGNUM context is passed all around BIGNUM code.
    ** Convert to proper structure and get pOEMContext. 
    ** pOEMContext can be NULL on certain cases, for example in tools. It is set in Drm_Initialize
    ** If Drm_Initialize is called, then pBigNumCtx is present and pOEMContext also present.
    */

    DRMBIGNUM_CONTEXT_STRUCT *pBigNumCtx  = (DRMBIGNUM_CONTEXT_STRUCT *)f_pBigCtx;
    CRYPTO_VOID                 *pOEMContext = NULL;

    DRMASSERT( pBigNumCtx != NULL );
    
    if ( pBigNumCtx != NULL )
    {
        pOEMContext = pBigNumCtx->pOEMContext;
    }

    TzGetRandomBytes((UINT8 *)byte_array, (UINT32) nbyte);


    return TRUE;
}





#endif
 

