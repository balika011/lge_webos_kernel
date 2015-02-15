/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "stdlib.h"
#include "string.h"
#include "tzst.h"

#include "../crypto_types.h"
#include "../crypto_results.h"
#include "../drmeccp256.h"
#include "drmbcertparser.h"

#define DoExpirationCheck(ft) ( ( (ft).dwLowDateTime != 0 ) && ( (ft).dwHighDateTime != 0 ) )
																					
#define ChkVerificationErr( pVerificationCtx, fCondition, dwErr )                                             \
{																							\
	if ( !(fCondition) )																	\
	{																						\
		if ( pVerificationCtx->fCollectErrorsInArrayAndContinue == FALSE					\
			|| pVerificationCtx->cResults >= DRM_MAX_VERIFICATIONRESULTS )					\
		{																					\
			ChkDR(dwErr);																	\
		}																					\
		else																				\
		{																					\
			pVerificationCtx->rgoResults[pVerificationCtx->cResults].cCertNumber = pVerificationCtx->cCurrentCert; \
			pVerificationCtx->rgoResults[pVerificationCtx->cResults++].dwResult = (dwErr);						   \
		}																					\
	}																						\
}																							


/*				 Encr	Sign Issuer Issuer Issuer Issuer Issuer Issuer Issuer  Issuer  Issuer  Issuer  Sign 	Issuer */
/* n/a	  Sign	  Key	 CRL	All  Indiv Device	Link Domain   SL	App   CRLSign Metering KF Sign KeyFile	Server */

const CRYPTO_BOOL rgfKeyUsageMapping[ DRM_BCERT_CERTTYPE_MAX_VALUE + 1 ][ DRM_BCERT_KEYUSAGE_MAX_VALUE + 1 ] = 
{/* n/a         */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* PC          */  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* Device      */  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* Domain      */  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* Issuer      */  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,   TRUE,   TRUE,   FALSE,   TRUE,
 /* CRLSigner   */  FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* Service     */  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* SilverLight */  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* Application */  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* Metering    */  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE,
 /* KeyFile Sign*/  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  TRUE,    FALSE,
 /* Server*/        FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,  FALSE,  FALSE,  FALSE,   FALSE
};


/* Note: Issuer cert can have any feature because sometimes (device certs) this is where we look for them instead of leaf cert */

const CRYPTO_BOOL rgfFeatureMapping[ DRM_BCERT_CERTTYPE_MAX_VALUE + 1 ][ DRM_BCERT_FEATURE_MAX_VALUE + 1 ] = 
{
                  /*                    Shared  Secure AntiRB        Lic    Sym          */
                  /* n/a    Tx     Rx     Cert  Clock  Clock  Meter  Sync   Opts   CRLs  */
 /* n/a         */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
 /* PC          */  FALSE, TRUE,  TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,
 /* Device      */  FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, TRUE,
 /* Domain      */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
 /* Issuer      */  FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
 /* CRLSigner   */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 /* Service     */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
 /* SilverLight */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 /* Application */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 /* Metering    */  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 /* KeyFile Sign*/  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
 /* Server*/        FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};


#define BCERT_KEYUSAGE_PARENT_ISSUERS_MASK                          \
    ( BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_INDIV )         \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_DEVICE )        \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_LINK )          \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN )        \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT )   \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION )   \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_CRL )           \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE )  \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_METERING ) )

#if 1  //definition for utf processing
#define GET_BYTE(pb,ib)             (pb)[(ib)]
#define PUT_BYTE(pb,ib,b)           (pb)[(ib)]=(b)
#define GET_CHAR(pch,ich)           (pch)[(ich)]
#define PUT_CHAR(pch,ich,ch)        (pch)[(ich)]=(ch)
				
static void  DRM_BYT_ReverseBytes(CRYPTO_BYTE  *pbData, CRYPTO_DWORD  cbData )
{
    CRYPTO_BYTE bTemp;
    CRYPTO_DWORD i;
    
    if( cbData < 2 )
    {
        return;
    }
    
    cbData--;
    for( i=0; i<cbData; i++,cbData-- )
    {
        bTemp = GET_BYTE(pbData, i);
        PUT_BYTE(pbData, i, GET_BYTE(pbData, cbData));
        PUT_BYTE(pbData, cbData, bTemp);
    }
}
#define DRM_BYT_CompareBytes(pbA,ibA,pbB,ibB,cb) memcmp(&((pbA)[(ibA)]),&((pbB)[(ibB)]),(cb))
#define DRM_BYT_CopyBytes(to,tooffset,from,fromoffset,count) memcpy(&((to)[(tooffset)]),&((from)[(fromoffset)]),(count))
#define COPY_FROMBUFFER(to, from, index, size, buffersize)  {ChkOverflow((index)+(size),(index));ChkBOOL((index)+(size)<=(buffersize),DRM_E_BUFFERTOOSMALL);DRM_BYT_CopyBytes((CRYPTO_BYTE*)&(to),0,(from),(index),(size));(index)+=(size);}		
#define NETWORKBYTES_FROMBUFFER(to, from, index, size, buffersize)       {COPY_FROMBUFFER(to,from,index,size,buffersize);DRM_BYT_ReverseBytes((CRYPTO_BYTE*)&(to),(size));}
#define NETWORKBYTES_FROMBUFFER_TO_WORD(word, byte, index, buffersize)   NETWORKBYTES_FROMBUFFER(word,byte,index,SIZEOF(CRYPTO_WORD),buffersize)
#define NETWORKBYTES_FROMBUFFER_TO_DWORD(dword, byte, index, buffersize) NETWORKBYTES_FROMBUFFER(dword, byte,index,SIZEOF(CRYPTO_DWORD),buffersize)
#define REVERSE_BYTES_WORD(w)               DRM_BYT_ReverseBytes((CRYPTO_BYTE*)&(w),SIZEOF(CRYPTO_WORD))

#define NETWORKBYTES_TO_DWORD(dword, byte, index)   {DRM_BYT_CopyBytes((CRYPTO_BYTE*)&(dword),0,(byte),(index),SIZEOF(CRYPTO_DWORD)); REVERSE_BYTES_DWORD((dword));}

#define DRM_BCert_GetLeafSecurityLevel(secLevel, chain) \
    NETWORKBYTES_TO_DWORD( (secLevel), (chain), SIZEOF( DRM_BCERT_CHAIN_HEADER )  \
                                              + SIZEOF( DRM_BCERT_HEADER )        \
                                              + SIZEOF( DRM_BCERT_OBJECT_HEADER ) \
                                              + SIZEOF( DRM_BCERT_CERT_ID ) );		

/*
** Unicode has 17 "planes", each with 2^16 code points. The exception is the
** "Base Multilingual Plane" (#0) which lacks the values U+D800..U+DFFF.
*/
typedef unsigned int			DRM_WCHAR32;			/* 4 bytes			   2 bytes */
#define UTF_PLANE0_MAX     (CRYPTO_WCHAR32)0x0000FFFFUL
#define UTF_PLANE1_MIN     (DRM_WCHAR32)0x00010000UL

#define UTF8_MIN_TWO_BYTE_CP    0x80UL
#define UTF8_MIN_THREE_BYTE_CP  0x800UL
#define UTF8_MIN_FOUR_BYTE_CP   UTF_PLANE1_MIN

/*
** Any code point greater than 0x10FFFF is ill-formed.
*/
#define UTF_MAX_CODE_POINT (DRM_WCHAR32)0x0010FFFFUL

typedef CRYPTO_UINT DRM_CODE_POINT;

typedef struct tagDRM_STRING_WINDOW
{
    CRYPTO_DWORD m_ichMin; /* inclusive */
    CRYPTO_DWORD m_ichMaxExclusive; /* exclusive */
} DRM_STRING_WINDOW;

#define EMPTY_DRM_STRING_WINDOW { 0, 0 }

/*
** 110yyyyy 10xxxxxx -> 00000000 00000000 00000yyy yyxxxxxx
*/
#define UTF_CP_FROM_TWO_CH(b1,b2) \
    ( ( ( ( (b1) & 0x1FUL ) << 0x6 ) | ( (b2) & 0x3FUL ) ) & 0x7FF )
/*
** 1110zzzz 10yyyyyy 10xxxxxx -> 00000000 00000000 zzzzyyyy yyxxxxxx
*/
#define UTF_CP_FROM_THREE_CH(b1,b2,b3) \
    ( ( ( ( (b1) & 0xFUL ) << 0xC ) | ( ( (b2) & 0x3FUL ) << 0x6 ) | ( (b3) & 0x3FUL ) ) & 0xFFFFUL )

/*
** 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx -> 00000000 000uuuuu zzzzyyyy yyxxxxxx
*/
#define UTF_CP_FROM_FOUR_CH(b1,b2,b3,b4) \
    ( ( ( ( (b1) & 0x7UL ) << 0x12 ) | ( ( (b2) & 0x3FUL ) << 0xC ) | ( ( (b3) & 0x3FUL ) << 0x6 ) | ( (b4) & 0x3FUL ) ) & 0x1FFFFFUL )

/*
** Test that this byte looks like 10vvvvvv
** so that its value is in 80..BF range
*/
#define IS_UTF8_TRAILING_CH(b) (((b) & 0xC0) == 0x80)

#if !defined( min )
    #define min( a, b ) ( (a) < (b) ? (a) : (b) )
#endif

/*
** High-surrogate code unit: A 16-bit code unit in the range 0xD800 to 0xDBFF,
** used in UTF-16 as the leading code unit of a surrogate pair.
**
** Low-surrogate code unit: A 16-bit code unit in the range 0xDC00 to 0xDFFF,
** used in UTF-16 as the trailing code unit of a surrogate pair.
*/
#define UTF16_HIGH_SURROGATE_MIN_CP  (DRM_CODE_POINT)0xD800UL
#define UTF16_HIGH_SURROGATE_MAX_CP  (DRM_CODE_POINT)0xDBFFUL
#define UTF16_LOW_SURROGATE_MIN_CP   (DRM_CODE_POINT)0xDC00UL
#define UTF16_LOW_SURROGATE_MAX_CP   (DRM_CODE_POINT)0xDFFFUL

/* Create a macro that tests for _both_ high and low surrogates. 
** Note: high surrogates are always numerically less than low surrogates
*/
#define IS_UTF16_SURROGATE_CP(_cp_) \
    ( ( _cp_ ) >= UTF16_HIGH_SURROGATE_MIN_CP && \
      ( _cp_ ) <= UTF16_LOW_SURROGATE_MAX_CP )

/*******************************************************************************
** Function: _ReadCodePoint8
**
** Synopsis: Reads a single code point from a buffer of UTF-8 code units.
**
** Arguments:
**
** [f_pchSource]     -- Pointer to UTF-8 data buffer.
** [f_pdstrwSource]  -- Window covering a range of UTF-8 units.
**                      On success, left edge of window is advanced to the next
**                      code point. On error, it is unchanged.
** [f_pcp]           -- Pointer that will receive the next code point. On
**                      error this is unchanged.
**
** Returns:          DRM_SUCCESS on success.
**                   DRM_E_INVALIDARG if the arguments are invalid (such as a
**                   window with zero or negative width).
**                   DRM_E_UTF_INVALID_CODE - if UTF-8 sequence is illegal.
**                   DRM_E_UTF_UNEXPECTED_END - if parsing ended on a partial
**                       character.
**
*******************************************************************************/
DRM_RESULT _ReadCodePoint8(
    const CRYPTO_CHAR          *f_pchSource,
    DRM_STRING_WINDOW *f_pdstrwSource,
    DRM_CODE_POINT    *f_pcp )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_CODE_POINT  cp              = 0;
    CRYPTO_BYTE        cLeadChar       = 0;
    CRYPTO_DWORD       cchCodeUnits    = 0;
    CRYPTO_DWORD       ich             = 0;
    CRYPTO_DWORD       ichCurrentUnit  = 0;
    CRYPTO_DWORD       ichMaxUnit      = 0;
    CRYPTO_BYTE        rgbUnits[4]     = { 0 };

    ChkArg( f_pchSource != NULL );
    ChkArg( f_pdstrwSource != NULL );
    ChkArg( f_pcp != NULL );
    ChkArg( f_pdstrwSource->m_ichMin < f_pdstrwSource->m_ichMaxExclusive );

    ich = f_pdstrwSource->m_ichMin;

    cLeadChar = (CRYPTO_BYTE) GET_CHAR( f_pchSource, ich ); /* grab one byte */

    /*
    ** The number of bits set at the top of the first code unit (aka first
    ** character if readers prefer) encodes the number of units that follow
    ** it. The below code does *some* range validation as well, but is not
    ** guaranteed to do all of it. Subsequent logic will do full validation!!
    */
    if( cLeadChar < 0x80UL )
    {
        /* It's an ASCII value, i.e. cchCodeUnits == 1. All validation is done
        ** at this point, so finish up. */
        *f_pcp = cLeadChar;
        f_pdstrwSource->m_ichMin = ich + 1;

        /* EXIT here (common case) */
        goto ErrorExit;
    }
    else if( cLeadChar >= 0xC2UL && cLeadChar < 0xE0UL )
    {
        cchCodeUnits = 2;
    }
    else if( cLeadChar >= 0xE0UL && cLeadChar < 0xF0UL )
    {
        cchCodeUnits = 3;
    }
    else if( cLeadChar >= 0xF0UL && cLeadChar < 0xF5UL )
    {
        cchCodeUnits = 4;
    }
    else
    {
        ChkDR( DRM_E_UTF_INVALID_CODE );
    }

    /*
    ** Ensure the buffer contains enough room for *all* characters. If
    ** cchCodeUnits is 2, the answer is one larger than the original ich.
    */
    ichMaxUnit = ich + cchCodeUnits - 1;
    ChkOverflow( ichMaxUnit, ich );

    ChkBOOL( ichMaxUnit < f_pdstrwSource->m_ichMaxExclusive, DRM_E_UTF_UNEXPECTED_END );

    /* Special case what's already been read. The for loop helps too by
    ** starting at 1 */
    ich++;
    rgbUnits[0] = cLeadChar;

    /* function min is used for rgbUnits array safety */
    for( ichCurrentUnit = 1; ichCurrentUnit < min(cchCodeUnits, 4); ichCurrentUnit++ )
    {
        /* Advance ich as well while in here */
        rgbUnits[ichCurrentUnit] = GET_CHAR( f_pchSource, ich++ );

        ChkBOOL( IS_UTF8_TRAILING_CH( rgbUnits[ichCurrentUnit] ),
                 DRM_E_UTF_INVALID_CODE );
    }

    switch ( cchCodeUnits )
    {
        case 2:
            cp = UTF_CP_FROM_TWO_CH( rgbUnits[0], rgbUnits[1] );
            ChkBOOL( cp >= UTF8_MIN_TWO_BYTE_CP && cp < UTF8_MIN_THREE_BYTE_CP,
                     DRM_E_UTF_INVALID_CODE );

            break;

        case 3:
            cp = UTF_CP_FROM_THREE_CH( rgbUnits[0], rgbUnits[1], rgbUnits[2] );
            ChkBOOL( ( cp >= UTF8_MIN_THREE_BYTE_CP &&
                       cp < UTF8_MIN_FOUR_BYTE_CP &&
                      !IS_UTF16_SURROGATE_CP(cp) ), DRM_E_UTF_INVALID_CODE );

            break;

        case 4:
            cp = UTF_CP_FROM_FOUR_CH( rgbUnits[0], rgbUnits[1], rgbUnits[2], rgbUnits[3] );
            ChkBOOL( cp >= UTF8_MIN_FOUR_BYTE_CP && cp <= UTF_MAX_CODE_POINT,
                     DRM_E_UTF_INVALID_CODE );

            break;

        default:
            DRMASSERT( FALSE ); /* we should never get here */

            ChkDR( DRM_E_FAIL );

            break;
    }

    *f_pcp = cp;
    f_pdstrwSource->m_ichMin = ich;

ErrorExit:

    return dr;
}


/*******************************************************************************
** Function: DRM_UTF8_VerifyBytes
**
** Synopsis: Function that verifies validity of UTF-8 data
**           according to unicode.org specification.
**
** Arguments:
**
** [f_pbData]            -- Pointer to a data buffer that contains UTF-8.
**
** [f_cbData]            -- Size of f_pbData in bytes.
** [f_iMin]              -- Position in data buffer to begin reading (in
**                          code units, aka storage characters).
** [f_iMax]              -- Position in data buffer to stop reading (in
**                          code units, aka storage characters), inclusive.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: The first valid UTF-8 char should start right at f_iStart.
** If this assumption is too strong I can make a change.
*******************************************************************************/
 DRM_RESULT DRM_UTF8_VerifyBytes(
    const CRYPTO_BYTE  *f_pbData,
    const CRYPTO_DWORD f_cbData,
    const CRYPTO_DWORD f_iMin,
    const CRYPTO_DWORD f_iMax )
{
    DRM_RESULT        dr    = DRM_SUCCESS;
    DRM_STRING_WINDOW dstrw = EMPTY_DRM_STRING_WINDOW;
    DRM_CODE_POINT    cp    = 0;


    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    ChkArg( f_iMax < f_cbData );
    ChkArg( f_iMax >= f_iMin );

    /* Simply read all the code points out to validate their correctness */
    dstrw.m_ichMin = f_iMin;
    dstrw.m_ichMaxExclusive = f_iMax + 1; /* f_iMax is included */

    while ( dstrw.m_ichMin < dstrw.m_ichMaxExclusive )
    {
        ChkDR( _ReadCodePoint8( ( const CRYPTO_CHAR * ) f_pbData, &dstrw, &cp ) );
    }

ErrorExit:

    return dr;
}


#endif


/*****************************************************************************
** Function:    _isUTF8NullTerminatedData
**
** Synopsis:    Verify that byte buffer contains a null-terminated UTF-8 string.
**
** Arguments:   [f_pbData]   : pointer to byte data buffer
**              [f_cbData]   : size in bytes of f_pbData
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_BCERT_STRING_NOT_NULL_TERMINATED
**              DRM_E_UTF8_xxxx - if a string is not UTF-8
**
** Notes:       Strings of length 1 that contain no data and one null are okay.
******************************************************************************/
static DRM_RESULT _isUTF8NullTerminatedData(
    const CRYPTO_BYTE  *f_pbData,
    CRYPTO_DWORD       f_cbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    ChkBOOL( GET_BYTE( f_pbData, f_cbData - 1 ) == 0x0, DRM_E_BCERT_STRING_NOT_NULL_TERMINATED );

    if ( f_cbData > 1 )
    {
        ChkDR( DRM_UTF8_VerifyBytes( f_pbData,
                                     f_cbData - 1,
                                     0,
                                     f_cbData - 2 ) );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _verifyKeyUsage
**
** Synopsis:    Verify that key usage information matches certificate type.
**
** Arguments:   [f_dwKeyUsage]   : key usage data
**              [f_dwCertType]   : certificate type
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_BCERT_INVALID_CERT_TYPE - if cert type is invalid
**              DRM_E_BCERT_INVALID_KEY_USAGE - if key usage is invalid 
**                                              or does not match cert type
**
** Notes:       None
******************************************************************************/
static DRM_RESULT _verifyKeyUsage(
     CRYPTO_DWORD f_dwKeyUsage, 
     CRYPTO_DWORD f_dwCertType )
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    if ( f_dwCertType != DRM_BCERT_CERTTYPE_UNKNOWN )
    {
        ChkBOOL( f_dwCertType <= DRM_BCERT_CERTTYPE_MAX_VALUE, 
            DRM_E_BCERT_INVALID_CERT_TYPE );
        ChkBOOL( f_dwKeyUsage != DRM_BCERT_KEYUSAGE_UNKNOWN 
              && f_dwKeyUsage <= DRM_BCERT_KEYUSAGE_MAX_VALUE, 
            DRM_E_BCERT_INVALID_KEY_USAGE );

        ChkBOOL( rgfKeyUsageMapping[ f_dwCertType ][ f_dwKeyUsage ], 
                 DRM_E_BCERT_INVALID_KEY_USAGE );
    }
ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    _verifyFeature
**
** Synopsis:    Verify that feature information matches certificate type.
**
** Arguments:   [f_dwFeature]   : feature data
**              [f_dwCertType]  : certificate type
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_BCERT_INVALID_CERT_TYPE - if cert type is invalid
**              DRM_E_BCERT_INVALID_FEATURE   - if feature is invalid 
**                                              or does not match cert type
**
** Notes:       None
******************************************************************************/
static DRM_RESULT _verifyFeature( 
    CRYPTO_DWORD f_dwFeature, 
    CRYPTO_DWORD f_dwCertType )
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    if ( f_dwCertType == 0
        || f_dwCertType > DRM_BCERT_CERTTYPE_MAX_VALUE )
    {
        ChkDR( DRM_E_BCERT_INVALID_CERT_TYPE );
    }

    if ( f_dwFeature == 0
        || f_dwFeature > DRM_BCERT_FEATURE_MAX_VALUE )
    {
        ChkDR( DRM_E_BCERT_INVALID_FEATURE );
    }

    ChkBOOL( rgfFeatureMapping[ f_dwCertType ][ f_dwFeature ], 
             DRM_E_BCERT_INVALID_FEATURE );

ErrorExit:

    return dr;
}



/*****************************************************************************
** Function:    _verifyAdjacentCerts
**
** Synopsis:    Verifies that certificates in a chain are correctly linked:
**              1. Issuer key value is the same as a key entry in the next certificate
**                 that issued previous one.
**              2. Security level of the next cert is not lower 
**                 then that of the current cert.
**              3. The parent certificate has the appropriate issuer rights to issue
**                 the child certificate of a particular type with particular key usages and features.
**
** Arguments: [f_poChildCert]  : pointer to structure that contains information
**                               from the child certificate, cannot be NULL
**            [f_poParentCert] : pointer to structure with similar information 
**                               from the parent certificate, cannot be NULL
**                               so this function will never be called for a chain with one cert.
**    [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH
**              DRM_E_BCERT_INVALID_SECURITY_LEVEL
**              DRM_E_BCERT_INVALID_KEY_USAGE
**
******************************************************************************/
static DRM_RESULT _verifyAdjacentCerts(
    const DRM_BCERT_MINIMALDATA     *f_poChildCert,
    const DRM_BCERT_MINIMALDATA     *f_poParentCert,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr                        = DRM_SUCCESS;
    CRYPTO_DWORD  iKey                      = 0;
    CRYPTO_DWORD  iCount                    = 0;
    CRYPTO_DWORD  dwParentKeyUsageMask      = 0;
    CRYPTO_DWORD  dwChildKeyUsageMask       = 0;
    CRYPTO_BOOL   fSupportSecureClock       = FALSE;
    CRYPTO_BOOL   fSupportAntiRollbackClock = FALSE;
    
    
    ChkArg( f_poChildCert != NULL );
    ChkArg( f_poParentCert != NULL );

    /*
    ** Parent cert type should be issuer because it has child certificate
    */
    ChkVerificationErr( f_pVerificationContext, 
                        f_poParentCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_ISSUER, 
                        DRM_E_BCERT_INVALID_CERT_TYPE );

    /*
    ** Security level of the parent cert is higher or equal to its child cert's.
    */
    ChkVerificationErr( f_pVerificationContext,
                        f_poChildCert->oBasicInfo.dwSecurityLevel <= f_poParentCert->oBasicInfo.dwSecurityLevel,
                        DRM_E_BCERT_INVALID_SECURITY_LEVEL );

    /* 
    ** Issuer key for the current cert is the same as one of the key values in the child cert
    */
    dr = DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH;
    
    for ( iKey = 0; iKey < f_poParentCert->oKeyInfo.dwNumKeys; iKey++ )
    {
        if ( DRM_BYT_CompareBytes( f_poChildCert->oSignatureInfo.IssuerKey.rgb, 
                                   0, 
                                   f_poParentCert->oKeyInfo.rgoKeys[iKey].Value.rgb,
                                   0,
                                   DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH ) == 0 )
        {
            dr = CRYPTO_SUCCESS;
            break;
        }
    }
    if ( CRYPTO_FAILED(dr) ) 
    {
        ChkVerificationErr( f_pVerificationContext, FALSE, dr);
    }

    /*
    ** Collect the key usages of the parent certificate for all keys in the certificate
    */
    for ( iKey = 0; iKey < f_poParentCert->oKeyInfo.dwNumKeys; iKey++ )
    {
        for ( iCount = 0; iCount < f_poParentCert->oKeyInfo.rgoKeys[iKey].dwNumUsageEntries; iCount++ )
        {
            dwParentKeyUsageMask |= BCERT_KEYUSAGE_BIT( f_poParentCert->oKeyInfo.rgoKeys[iKey].dwUsageSet[ iCount ] );
        }    
    }
    
    /*
    ** Check that the current certificate has appropriate Issuer rights
    ** to issue a child certificate of particular type.
    */
    if ( !(dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_ALL ) ) ) /* Issuer-All can issue anything */
    {
        if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_DOMAIN )
        {
            /*
            ** Issuer-Domain can issue certificates of Type Domain.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        } 
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_DEVICE )
        {
            /*
            ** Issuer-Device can issue certificate of Type Device.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_PC )
        {
            /*
            ** Issuer-Indiv can issue certificate of Type PC.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_INDIV), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SILVERLIGHT )
        {
            /*
            ** Issuer-SilverLight can issue certificate of Type SilverLight.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }    
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_APPLICATION )
        {
            /*
            ** Issuer-Application can issue certificate of Type Application.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }    
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_METERING)
        {
            /*
            ** Issuer-Metering can issue certificate of Type Metering.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_METERING), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_KEYFILESIGNER )
        {
            /*
            ** Issuer-SignKeyFile can issue certificate of Type KeyFileSigner
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVER )
        {
            /*
            ** Issuer-Server can issue certificate of Type Server
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SERVER), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_CRL_SIGNER )
        {
            /*
            ** Issuer-CLR can issue certificate of Type CRL Signer
            */
            ChkBOOL( dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_CRL), DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_ISSUER )
        {
            /*
            ** Issuer cert is issued by another issuer cert, and Issuer-related key usages should match.
            */
            for ( iKey = 0; iKey < f_poChildCert->oKeyInfo.dwNumKeys; iKey++ )
            {
                for ( iCount = 0; iCount < f_poChildCert->oKeyInfo.rgoKeys[iKey].dwNumUsageEntries; iCount++ )
                {
                    dwChildKeyUsageMask |= BCERT_KEYUSAGE_BIT( f_poChildCert->oKeyInfo.rgoKeys[iKey].dwUsageSet[ iCount ] );
                }    
            }
            
            /* 
            ** Cannot have Issuer-All child cert 
            */
            ChkVerificationErr( f_pVerificationContext,
                !( dwChildKeyUsageMask & BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_ALL ) ),
                DRM_E_BCERT_INVALID_KEY_USAGE ); 
            /*
            ** Check key usages in a child cert: 
            ** the parent cert is not Issuer-All and must have all issuer-... key usages that child cert has
            */
            ChkVerificationErr( f_pVerificationContext,
                    ( dwChildKeyUsageMask & BCERT_KEYUSAGE_PARENT_ISSUERS_MASK ) 
                        <= ( dwParentKeyUsageMask & BCERT_KEYUSAGE_PARENT_ISSUERS_MASK ),
                    DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else
        {
            /*
            ** Service cert type. If it has a parent cert it can only be Issuer_All cert, so fail
            */
            ChkVerificationErr( f_pVerificationContext,
                                f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVICE,
                                DRM_E_BCERT_INVALID_CERT_TYPE );

            ChkVerificationErr( f_pVerificationContext, FALSE, DRM_E_BCERT_INVALID_KEY_USAGE );
        } 
    }

    /*
    ** Some features in a child cert can only be issued by a parent cert with specific key usages.
    */
    for ( iCount = 0; iCount < f_poChildCert->oFeatureInfo.dwNumFeatureEntries; iCount++ )
    {
        if (  f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_TRANSMITTER
           || f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_RECEIVER
           || f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_SHARED_CERTIFICATE )
        {
            /*
            ** Transmitter, Receiver, SharedCertificate can be issued by Issuer-Link.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_LINK), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_SECURE_CLOCK )
        {
            /*
            ** SecureClock can be issued by Issuer-Device.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
            fSupportSecureClock = TRUE;
        }
        else if ( f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK )
        {
            /*
            ** Anti-Rollback Clock can be issued by Issuer-Device.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE), 
                                DRM_E_BCERT_INVALID_KEY_USAGE );
            fSupportAntiRollbackClock = TRUE;
        }
    }

    /*
    ** Either SecureClock or Anti-Rollback Clock can be issued in one certificate, never both.
    */
    ChkVerificationErr( f_pVerificationContext,
                        !( fSupportSecureClock && fSupportAntiRollbackClock ), 
                        DRM_E_BCERT_INVALID_KEY_USAGE );

ErrorExit:    

    return dr;
}


/*****************************************************************************
** Function:    _getObjectHeader
**
** Synopsis:    Parses an object header. If object type is unknown and not 
**              "must understand" function locates the next object header it can read.
**
** Arguments:   [f_pbCertData] : data buffer that is parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_poHeader]   : pointer to an object header structure, cannot be NULL
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if either f_pbOffset or an object length is too big
**              DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG - object length value is too big
**
******************************************************************************/
static DRM_RESULT _getObjectHeader( 
    const CRYPTO_BYTE          *f_pbCertData,
    const CRYPTO_DWORD         f_cbCertData,
    CRYPTO_DWORD               *f_pbOffset,
    DRM_BCERT_OBJECT_HEADER *f_poHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;
    
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( *f_pbOffset < f_cbCertData );
    ChkArg( f_poHeader != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read all object header attributes
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poHeader->wFlags, 
                                     f_pbCertData, 
                                     dwOffset,
                                     f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poHeader->wType, 
                                     f_pbCertData, 
                                     dwOffset,
                                     f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poHeader->cbLength, 
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );
    /*
    ** Object length should be small enough and inside data buffer.
    ** Note that object header is included into object length.
    */
    ChkOverflow( *f_pbOffset + f_poHeader->cbLength, *f_pbOffset );
    ChkBOOL( *f_pbOffset + f_poHeader->cbLength <= f_cbCertData,
             DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG );    

    if ( f_poHeader->wType > DRM_BCERT_OBJTYPE_MAX_VALUE )
    {
        /*
        ** If object's type is unknown but flag is not "must understand"
        ** try to skip it and read the next one
        */
        ChkBOOL( f_poHeader->wFlags != DRM_BCERT_OBJFLAG_MUST_UNDERSTAND, 
                 DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        *f_pbOffset += f_poHeader->cbLength;

        ChkDR( _getObjectHeader( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 f_poHeader ) );
    }
    else
    {
        /* 
        ** move current offset past object header and stop reading
        */
        ChkBoundsLT( dwOffset, f_cbCertData );
        *f_pbOffset = dwOffset;
    }

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function:    _parseCertHeader
**
** Synopsis:    Parses a certificate's header.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poCertHeader] : pointer to a certificate header structure, cannot be NULL
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_CERT_HEADER_TAG
**              DRM_E_BCERT_INVALID_CERT_VERSION
**              DRM_E_BCERT_INVALID_CERT_LENGTH
**              DRM_E_BCERT_INVALID_SIGNEDCERT_LENGTH
**              
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _parseCertHeader(
    const CRYPTO_BYTE   *f_pbCertData,
    const CRYPTO_DWORD   f_cbCertData,
    CRYPTO_DWORD        *f_pbOffset,
    DRM_BCERT_HEADER *f_poCertHeader,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poCertHeader != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->dwHeaderTag, 
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );
    
    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->dwVersion, 
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );
    
    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->cbCertificate, 
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->cbCertificateSigned, 
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );

    
    /*
    ** Header constant tag is correct
    */
    ChkVerificationErr( f_pVerificationContext,
                        f_poCertHeader->dwHeaderTag == DRM_BCERT_HEADER_TAG, 
                        DRM_E_BCERT_INVALID_CERT_HEADER_TAG );
    /*
    ** Version is 1
    */
    ChkVerificationErr( f_pVerificationContext,
                        f_poCertHeader->dwVersion == DRM_BCERT_VERSION, 
                        DRM_E_BCERT_INVALID_CERT_VERSION );
    /* 
    ** length is at least DRM_MIN_CERTIFICATE_LEN and no bigger then
    ** the byte array we are parsing
    */
    ChkBOOL( f_poCertHeader->cbCertificate >= DRM_MIN_CERTIFICATE_LEN, 
             DRM_E_BCERT_INVALID_CERT_LENGTH );
    ChkBOOL( f_poCertHeader->cbCertificate <= f_cbCertData, 
             DRM_E_BCERT_INVALID_CERT_LENGTH );
    /*
    ** length to signature object is <= length
    */
    ChkBOOL( f_poCertHeader->cbCertificateSigned <= f_poCertHeader->cbCertificate, 
             DRM_E_BCERT_INVALID_SIGNEDCERT_LENGTH );
    /*
    ** length up to Signature Object should be larger 
    ** than minimal certificate length - signature length
    */
    ChkBOOL( f_poCertHeader->cbCertificateSigned 
            >=  DRM_MIN_CERTIFICATE_LEN - DRM_BCERT_SIGNATURE_OBJECT_LEN,
            DRM_E_BCERT_INVALID_SIGNEDCERT_LENGTH );

    *f_pbOffset = dwOffset;

ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseCertBasicInfo
**
** Synopsis:    Parses a certificate's basic info.
**
** Arguments:   [f_pbCertData]  : data buffer that is being parsed
**              [f_cbCertData]  : size of the data buffer (in bytes)
**              [f_pbOffset]    : position to start parsing from
**              [f_poBasicInfo] : pointer to a basic info structure, cannot be NULL
**     [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SECURITY_LEVEL
**              DRM_E_BCERT_INVALID_CERT_TYPE
**              DRM_E_BCERT_BASICINFO_CERT_EXPIRED
**              DRM_E_BCERT_EXTDATAFLAG_CERT_TYPE_MISMATCH
**
** Notes:       f_pbOffset points to basic info data not including object header.
******************************************************************************/
static DRM_RESULT _parseCertBasicInfo(
    const CRYPTO_BYTE       *f_pbCertData,
    const CRYPTO_DWORD      f_cbCertData,
    CRYPTO_DWORD            *f_pbOffset,
    DRM_BCERT_BASIC_INFO *f_poBasicInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    CRYPTO_DWORD   dwOffset     = 0;


    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poBasicInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;
        
    /*
    ** read certificate ID
    */
    COPY_FROMBUFFER( f_poBasicInfo->CertificateID.rgb, 
                     f_pbCertData, 
                     dwOffset, 
                     DRM_BCERT_CERT_ID_LENGTH, 
                     f_cbCertData );

    /*
    ** read security level data
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poBasicInfo->dwSecurityLevel, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    /*
    ** read flags
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poBasicInfo->dwFlags, 
            f_pbCertData, 
            dwOffset,      
            f_cbCertData );

    /*
    ** read and verify cert type
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poBasicInfo->dwType, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext, 
                        f_poBasicInfo->dwType <= DRM_BCERT_CERTTYPE_MAX_VALUE, 
                        DRM_E_BCERT_INVALID_CERT_TYPE );

    if ( (f_poBasicInfo->dwFlags & DRM_BCERT_FLAGS_EXTDATA_PRESENT) == DRM_BCERT_FLAGS_EXTDATA_PRESENT )
    {
        ChkVerificationErr( f_pVerificationContext,
                            f_poBasicInfo->dwType == DRM_BCERT_CERTTYPE_PC, 
                            DRM_E_BCERT_EXTDATAFLAG_CERT_TYPE_MISMATCH );
    }
       
    /*
    ** read digest value, no verification is needed
    */
    COPY_FROMBUFFER( 
            f_poBasicInfo->DigestValue.m_rgbDigest, 
            f_pbCertData, 
            dwOffset,
            SHA256_DIGEST_SIZE_IN_BYTES, 
            f_cbCertData );
    
    /*
    ** read expiration date and verify it is post current time.
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poBasicInfo->dwExpirationDate, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    /*
    ** read client ID
    */
    COPY_FROMBUFFER( 
            f_poBasicInfo->ClientID.rgb, 
            f_pbCertData, 
            dwOffset,
            DRM_BCERT_CLIENT_ID_LENGTH, 
            f_cbCertData );
    
    *f_pbOffset = dwOffset;
    
ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseDomainInfo
**
** Synopsis:    Parses a certificate's domain info. The data will not be copied
**              if we only verify it.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poDomainInfo] : pointer to a domain info structure, can be NULL
**                                 in which case we only do verification.
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_DOMAIN_URL_TOO_LONG
**
** Notes:       f_pbOffset points to domain info data not including object header.
******************************************************************************/
static DRM_RESULT _parseDomainInfo(
    const CRYPTO_BYTE        *f_pbCertData,
    const CRYPTO_DWORD       f_cbCertData,
    CRYPTO_DWORD             *f_pbOffset,
    DRM_BCERT_DOMAIN_INFO *f_poDomainInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;
    CRYPTO_DWORD  dwUrlLength = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    if ( f_poDomainInfo != NULL )
    {
        /*
        ** read service ID
        */
        COPY_FROMBUFFER( f_poDomainInfo->ServiceID.rgb, 
                         f_pbCertData, 
                         dwOffset, 
                         DRM_BCERT_SERVICE_ID_LENGTH, 
                         f_cbCertData );
            
        /*
        ** read account ID
        */
        COPY_FROMBUFFER( f_poDomainInfo->AccountID.rgb, 
                         f_pbCertData, 
                         dwOffset,
                         DRM_BCERT_ACCOUNT_ID_LENGTH, 
                         f_cbCertData );
       
        /*
        ** read revision timestamp
        */
        NETWORKBYTES_FROMBUFFER_TO_DWORD( 
                f_poDomainInfo->dwRevision, 
                f_pbCertData, 
                dwOffset,
                f_cbCertData );
    }
    else
    {
        dwOffset += DRM_BCERT_SERVICE_ID_LENGTH + DRM_BCERT_ACCOUNT_ID_LENGTH + SIZEOF( CRYPTO_DWORD );
    }
    /*
    ** Read length of domain URL.  Note that it is length without padding.
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwUrlLength,
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );    
    /*
    ** Domain URL cannot be empty or too big
    */
    ChkBOOL( dwUrlLength > 0, DRM_E_NO_URL );
    ChkBOOL( dwUrlLength < DRM_BCERT_MAX_DOMAIN_URL_LENGTH-2, DRM_E_BCERT_DOMAIN_URL_TOO_LONG );
    /*
    ** No overflow or reading outside buffer boundaries
    */
    ChkOverflow( dwOffset + dwUrlLength, dwOffset );
    ChkOverflow( dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength), dwOffset + dwUrlLength );
    ChkOverflow( f_cbCertData, dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength) );
    /*
    ** Domain URL is a null-terminated UTF-8 string
    */
    dr = _isUTF8NullTerminatedData( f_pbCertData + dwOffset, dwUrlLength );
    ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
    dr = CRYPTO_SUCCESS;

    if ( f_poDomainInfo != NULL )
    {        
        COPY_FROMBUFFER( 
                f_poDomainInfo->DomainURL.rgb, 
                f_pbCertData, 
                dwOffset,
                dwUrlLength + PAD_AMOUNT( dwUrlLength ), 
                f_cbCertData );
        
        f_poDomainInfo->DomainURL.cb = dwUrlLength;
    }
    else
    {
        dwOffset += dwUrlLength + PAD_AMOUNT( dwUrlLength );
    }
    
    *f_pbOffset = dwOffset;
    
ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parsePCInfo
**
** Synopsis:    Parses a certificate's PC info.
**
** Arguments:   [f_pbCertData] : data buffer that is being parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_poPCInfo]   : pointer to a PC info structure, can be NULL
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_HARDWARE_ID_TOO_LONG
**
** Notes:       f_pbOffset points to PC info data not including object header.
******************************************************************************/
static DRM_RESULT _parsePCInfo(
    const CRYPTO_BYTE    *f_pbCertData,
    const CRYPTO_DWORD   f_cbCertData,
    CRYPTO_DWORD         *f_pbOffset,
    DRM_BCERT_PC_INFO *f_poPCInfo )
{
    DRM_RESULT dr     = DRM_SUCCESS;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );
    
    /*
    ** Read security version
    */
    if ( f_poPCInfo != NULL )
    {
        NETWORKBYTES_FROMBUFFER_TO_DWORD( 
                f_poPCInfo->dwSecurityVersion,
                f_pbCertData, 
                *f_pbOffset,
                f_cbCertData );
    }
    else
    {
        ChkOverflow( *f_pbOffset + SIZEOF( CRYPTO_DWORD ), *f_pbOffset );
        ChkBoundsLT( *f_pbOffset + SIZEOF( CRYPTO_DWORD ), f_cbCertData );
        *f_pbOffset += SIZEOF( CRYPTO_DWORD );
    }

ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseServerInfo
**
** Synopsis:    Parses a certificate's Server info.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poServerInfo] : pointer to a Server info structure, can be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to Server info data not including object header.
******************************************************************************/
static DRM_RESULT _parseServerInfo(
    const CRYPTO_BYTE    *f_pbCertData,
    const CRYPTO_DWORD   f_cbCertData,
    CRYPTO_DWORD         *f_pbOffset,
    DRM_BCERT_SERVER_INFO *f_poServerInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    CRYPTO_DWORD  dwWarningDays    = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );
    
    /*
    ** Read warning days
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            dwWarningDays,
            f_pbCertData, 
            *f_pbOffset,
            f_cbCertData );
    ChkVerificationErr( f_pVerificationContext, dwWarningDays > 0, DRM_E_BCERT_INVALID_WARNING_DAYS );
    
    if ( f_poServerInfo != NULL )
    {
        f_poServerInfo->dwWarningDays = dwWarningDays;
    }

ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseSilverLightInfo
**
** Synopsis:    Parses a certificate's SilverLight info.
**
** Arguments:   [f_pbCertData]        : data buffer that is being parsed
**              [f_cbCertData]        : size of the data buffer (in bytes)
**              [f_pbOffset]          : position to start parsing from
**              [f_poSilverLightInfo] : pointer to a SilverLight info structure, can be NULL
**           [f_pVerificationContext] : pointer to verification context structure
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_HARDWARE_ID_TOO_LONG
**
** Notes:       f_pbOffset points to Silverlight info data not including object header.
******************************************************************************/
static DRM_RESULT _parseSilverLightInfo(
    const CRYPTO_BYTE             *f_pbCertData,
    const CRYPTO_DWORD             f_cbCertData,
    CRYPTO_DWORD                  *f_pbOffset,
    DRM_BCERT_SILVERLIGHT_INFO *f_poSilverLightInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    CRYPTO_DWORD  dwData = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );
    
    /*
    ** Read security version
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData, 
                                      f_pbCertData, 
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext, dwData != 0, DRM_E_BCERT_INVALID_SECURITY_VERSION );
    if ( f_poSilverLightInfo != NULL )
    {
        f_poSilverLightInfo->dwSecurityVersion = dwData;
    }

    /*
    ** Read platform identifier
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData, 
                                      f_pbCertData, 
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData <= DRM_BCERT_SILVERLIGHT_PLATFORM_MAX, 
                        DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER);
    if ( f_poSilverLightInfo != NULL )
    {
        f_poSilverLightInfo->dwPlatformIdentifier = dwData;
    }

ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseDeviceInfo
**
** Synopsis:    Parses a certificate's Device info.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poDeviceInfo] : pointer to a device info structure, can be NULL
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_INVALID_MAX_LICENSE_SIZE
**              DRM_E_BCERT_INVALID_MAX_HEADER_SIZE
**              DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH
**
** Notes:       f_pbOffset points to device info data not including object header.
******************************************************************************/
static DRM_RESULT _parseDeviceInfo(
    const CRYPTO_BYTE        *f_pbCertData,
    const CRYPTO_DWORD       f_cbCertData,
    CRYPTO_DWORD             *f_pbOffset,
    DRM_BCERT_DEVICE_INFO *f_poDeviceInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )

{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwData   = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );

    /*
    ** Read and verify max license size
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData, 
                                      f_pbCertData, 
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData == DRM_BCERT_MAX_LICENSE_SIZE, 
                        DRM_E_BCERT_INVALID_MAX_LICENSE_SIZE );
    if ( f_poDeviceInfo != NULL )
    {
        f_poDeviceInfo->cbMaxLicense = dwData;
    }

    /*
    ** Read and verify max header size
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData, 
                                      f_pbCertData, 
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData == DRM_BCERT_MAX_HEADER_SIZE, 
                        DRM_E_BCERT_INVALID_MAX_HEADER_SIZE );
    if ( f_poDeviceInfo != NULL )
    {
        f_poDeviceInfo->cbMaxHeader = dwData;
    }
    /*
    ** Read and verify max chain depth
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData, 
                                      f_pbCertData, 
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData == DRM_BCERT_MAX_LICENSE_CHAIN_DEPTH, 
                        DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );
    if ( f_poDeviceInfo != NULL )
    {
        f_poDeviceInfo->dwMaxChainDepth = dwData;
    }

ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseMeteringInfo
**
** Synopsis:    Parses a certificate's metering info. The data will not be copied
**              if we only verify it.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**            [f_poMeteringInfo] : pointer to a metering info structure, can be NULL
**                                 in which case we only do verification.
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_METERING_URL_TOO_LONG
**
** Notes:       f_pbOffset points to metering info data not including object header.
******************************************************************************/
static DRM_RESULT _parseMeteringInfo(
    const CRYPTO_BYTE          *f_pbCertData,
    CRYPTO_DWORD          f_cbCertData,
    CRYPTO_DWORD               *f_pbOffset,
    DRM_BCERT_METERING_INFO *f_poMeteringInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;
    CRYPTO_DWORD  dwUrlLength = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    if ( f_poMeteringInfo != NULL )
    {
        /*
        ** read metering ID
        */
        COPY_FROMBUFFER( f_poMeteringInfo->MeteringID.rgb, 
                         f_pbCertData, 
                         dwOffset, 
                         DRM_BCERT_METERING_ID_LENGTH, 
                         f_cbCertData );            
    }
    else
    {
        ChkOverflow( dwOffset + DRM_BCERT_METERING_ID_LENGTH, dwOffset );
        dwOffset += DRM_BCERT_METERING_ID_LENGTH;
    }
    
    /*
    ** Read length of metering URL.  Note that it is length without padding.
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwUrlLength,
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );    
    /*
    ** Metering URL cannot be empty or too big
    */
    ChkBOOL( dwUrlLength > 0, DRM_E_NO_URL );
    ChkBOOL( dwUrlLength < DRM_BCERT_MAX_METERING_URL_LENGTH-2, DRM_E_BCERT_METERING_URL_TOO_LONG );
    /*
    ** No overflow or reading outside buffer boundaries
    */
    ChkOverflow( dwOffset + dwUrlLength, dwOffset );
    ChkOverflow( dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength), dwOffset + dwUrlLength );
    ChkOverflow( f_cbCertData, dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength) );
    /*
    ** Metering URL is a null-terminated UTF-8 string
    */
    dr = _isUTF8NullTerminatedData( f_pbCertData + dwOffset, dwUrlLength );
    ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
    dr = CRYPTO_SUCCESS;

    if ( f_poMeteringInfo != NULL )
    {
                COPY_FROMBUFFER( 
                f_poMeteringInfo->MeteringURL.rgb, 
                f_pbCertData, 
                dwOffset,
                dwUrlLength + PAD_AMOUNT( dwUrlLength ), 
                f_cbCertData );
        
        f_poMeteringInfo->MeteringURL.cb = dwUrlLength;
    }
    else
    {
        dwOffset += dwUrlLength + PAD_AMOUNT( dwUrlLength );
    }

    *f_pbOffset = dwOffset;
    
ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseFeatureInfo
**
** Synopsis:    Parses feature info object. Feature info structure is always filled-in
**              because it is needed for adjacent cert verification.
**
** Arguments:   [f_pbCertData]    : data buffer that is being parsed
**              [f_cbCertData]    : size of the data buffer (in bytes)
**              [f_dwCertType]    : type of a certificate being parsed
**              [f_pbOffset]      : position to start parsing from
**              [f_poFeatureInfo] : pointer to a feature info structure, cannot be NULL
**       [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_MAX_FEATURES
**
** Notes:       f_pbOffset points to feature info data not including object header.
******************************************************************************/
static DRM_RESULT _parseFeatureInfo(
    const CRYPTO_BYTE         *f_pbCertData,
    const CRYPTO_DWORD        f_cbCertData,
    CRYPTO_DWORD              *f_pbOffset,
    CRYPTO_DWORD              f_dwCertType,
    DRM_BCERT_FEATURE_INFO *f_poFeatureInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;
    CRYPTO_DWORD  iCount   = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poFeatureInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poFeatureInfo->dwNumFeatureEntries, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    if ( f_poFeatureInfo->dwNumFeatureEntries == 0 )
    {
        *f_pbOffset = dwOffset;
        goto ErrorExit; /* no features in the list */
    }

    /*
    ** Verify that list of features is not too long
    */
    ChkBOOL( f_poFeatureInfo->dwNumFeatureEntries <= DRM_BCERT_MAX_FEATURES, 
             DRM_E_BCERT_INVALID_MAX_FEATURES );

    /*
    ** read each feature and verify
    */
    for ( iCount = 0; iCount < f_poFeatureInfo->dwNumFeatureEntries; iCount++ )
    {
        NETWORKBYTES_FROMBUFFER_TO_DWORD( 
                f_poFeatureInfo->rgdwFeatureSet[ iCount ],
                f_pbCertData, 
                dwOffset,
                f_cbCertData );

        dr = _verifyFeature( f_poFeatureInfo->rgdwFeatureSet[ iCount ], f_dwCertType );        
        ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
        dr = CRYPTO_SUCCESS;
    }

    DRMASSERT( dwOffset == *f_pbOffset 
                         + SIZEOF( CRYPTO_DWORD )
                         + f_poFeatureInfo->dwNumFeatureEntries * SIZEOF( CRYPTO_DWORD ) );

    *f_pbOffset = dwOffset;                                           
    
ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseManufacturerString
**
** Synopsis:    Reads and verifies one manufacturer string structure.
**
** Arguments:   [f_pbCertData]           : data buffer that is being parsed
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pbOffset]             : position to start parsing from
**              [f_poManufacturerString] : pointer to a manufacturer string structure, 
**                                         cannot be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
******************************************************************************/
static DRM_RESULT _parseManufacturerString(
    const CRYPTO_BYTE                *f_pbCertData,
    const CRYPTO_DWORD               f_cbCertData,
    CRYPTO_DWORD                     *f_pbOffset,
    DRM_BCERT_MANUFACTURER_STRING *f_poManufacturerString,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )    
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poManufacturerString != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;    

    /*
    ** String length
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poManufacturerString->cb, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    if ( f_poManufacturerString->cb > 0 )
    {
        ChkBOOL( (f_poManufacturerString->cb + PAD_AMOUNT( f_poManufacturerString->cb )) < 
			DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH-2, 
			DRM_E_BCERT_MANUFACTURER_STRING_TOO_LONG );
        
        /*
        ** Read string
        */    
        ChkOverflow( f_poManufacturerString->cb + PAD_AMOUNT( f_poManufacturerString->cb ), f_poManufacturerString->cb );
        COPY_FROMBUFFER( 
                f_poManufacturerString->rgb, 
                f_pbCertData, 
                dwOffset,
                f_poManufacturerString->cb
                    + PAD_AMOUNT( f_poManufacturerString->cb ),
                f_cbCertData );
        
        dr = _isUTF8NullTerminatedData( f_poManufacturerString->rgb, 
                                          f_poManufacturerString->cb );
        ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
        dr = CRYPTO_SUCCESS;
    }

    DRMASSERT( dwOffset == *f_pbOffset
                         + SIZEOF( CRYPTO_DWORD )                         /* string length */
                         + f_poManufacturerString->cb                  /* string data */
                         + PAD_AMOUNT( f_poManufacturerString->cb ) ); /* padding */

    *f_pbOffset = dwOffset;                                           
    
ErrorExit:

    return dr;
}

/************************************************************************************
** Function:    _parseManufacturerInfo
**
** Synopsis:    Parses manufacturer info object.
**
** Arguments:   [f_pbCertData]         : data buffer that is being parsed
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poManufacturerInfo] : pointer to a manufacturer info structure, 
**                                       can be NULL in which case we just validate data
**                                       and do not copy it.
**            [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              
** Notes:       f_pbOffset points to manufacturer info data not including object header.
**************************************************************************************/
static DRM_RESULT _parseManufacturerInfo( 
    const CRYPTO_BYTE                *f_pbCertData,
    const CRYPTO_DWORD               f_cbCertData,
    CRYPTO_DWORD                     *f_pbOffset,
    DRM_BCERT_MANUFACTURER_INFO   *f_poManufacturerInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )    
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;
    DRM_BCERT_MANUFACTURER_STRING manuStr = { 0 };
        
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;    

    /*
    ** Flags - read and ignore
    */
    ChkOverflow( dwOffset + SIZEOF( CRYPTO_DWORD ), dwOffset );
    dwOffset += SIZEOF( CRYPTO_DWORD );    

    /*
    ** Manufacturer name
    */
    ChkDR( _parseManufacturerString( 
        f_pbCertData,
        f_cbCertData,
        &dwOffset,
        f_poManufacturerInfo != NULL ? &f_poManufacturerInfo->ManufacturerStrings.ManufacturerName : &manuStr,
        f_pVerificationContext ) );
    /*
    ** Model name
    */
    ChkDR( _parseManufacturerString( 
        f_pbCertData,
        f_cbCertData,
        &dwOffset,
        f_poManufacturerInfo != NULL ? &f_poManufacturerInfo->ManufacturerStrings.ModelName : &manuStr,
        f_pVerificationContext ) );
    /*
    ** Model number
    */
    ChkDR( _parseManufacturerString( 
        f_pbCertData,
        f_cbCertData,
        &dwOffset,
        f_poManufacturerInfo != NULL ? &f_poManufacturerInfo->ManufacturerStrings.ModelNumber : &manuStr,
        f_pVerificationContext ) );
    
    *f_pbOffset = dwOffset;

ErrorExit:
    return dr;
}

/**********************************************************************************************************
** Function:    _parseExtDataSignKeyInfo
**
** Synopsis:    Parse extended data signature key object. 
**
** Arguments:   [f_pbCertData]           : data buffer that is being parsed
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pbOffset]             : position to start parsing from
**              [f_poExtDataSignKeyInfo] : pointer to a signature info structure, cannot be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to extended data signature key info data not including object header.
**********************************************************************************************************/
static DRM_RESULT _parseExtDataSignKeyInfo(
    const CRYPTO_BYTE                 *f_pbCertData,
    const CRYPTO_DWORD                f_cbCertData,
    CRYPTO_DWORD                      *f_pbOffset,
    DRM_BCERT_EXTDATA_SIGNKEY_INFO *f_poExtDataSignKeyInfo,
    DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )    
{
    DRM_RESULT dr = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_poExtDataSignKeyInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;    

    /*
    ** Read key type
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( 
            f_poExtDataSignKeyInfo->wType, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataSignKeyInfo->wType == DRM_BCERT_KEYTYPE_ECC256, 
                        DRM_E_BCERT_INVALID_KEY_TYPE );
   
    /*
    ** Read key length
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( 
            f_poExtDataSignKeyInfo->wLength, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
   
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataSignKeyInfo->wLength == SIZEOF( PUBKEY_P256 ) * CHAR_BIT, 
                        DRM_E_BCERT_INVALID_KEY_LENGTH );

    /*
    ** Read flags
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poExtDataSignKeyInfo->dwFlags, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    /*
    ** Read key value - don't reverse bytes, just copy
    */
    COPY_FROMBUFFER(
            f_poExtDataSignKeyInfo->Value.rgb, 
            f_pbCertData, 
            dwOffset,
            SIZEOF( PUBKEY_P256 ),
            f_cbCertData );

    *f_pbOffset = dwOffset;

ErrorExit:

    return dr;
}

/**********************************************************************************************************
** Function:    _parseSignatureInfo
**
** Synopsis:    Parse signature info object. This function does not validate signature data,
**              it only checks data for correct size and values.
**
** Arguments:   [f_pbCertData]       : data buffer that is being parsed
**              [f_cbCertData]       : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poSignatureInfo]  : pointer to a signature info structure, cannot be NULL
**          [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE
**              DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH
**
** Notes:       f_pbOffset points to signature info data not including object header.
**********************************************************************************************************/
static DRM_RESULT _parseSignatureInfo(
    const CRYPTO_BYTE                 *f_pbCertData,
    const CRYPTO_DWORD                f_cbCertData,
    CRYPTO_DWORD                      *f_pbOffset,
    DRM_BCERT_SIGNATURE_INFO       *f_poSignatureInfo,
    DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )    
{
    DRM_RESULT dr = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;


    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_poSignatureInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;    

    /*
    ** Read and verify signature type
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( 
            f_poSignatureInfo->wSignatureType, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    
    ChkVerificationErr( f_pVerificationContext,
                        f_poSignatureInfo->wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256, 
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Read and verify signature data length
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( 
            f_poSignatureInfo->SignatureData.cb, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    
    ChkVerificationErr( f_pVerificationContext,
                        f_poSignatureInfo->SignatureData.cb == SIZEOF( SIGNATURE_P256 ),
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Read signature
    */
    COPY_FROMBUFFER(
            f_poSignatureInfo->SignatureData.rgb, 
            f_pbCertData,
            dwOffset,
            f_poSignatureInfo->SignatureData.cb
                + PAD_AMOUNT( f_poSignatureInfo->SignatureData.cb ),
            f_cbCertData );
    
    /*
    ** Read issuer key length which should be SIZEOF( PUBKEY_P256 ) * CHAR_BIT == 0x200
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poSignatureInfo->dwIssuerKeyLength, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poSignatureInfo->dwIssuerKeyLength == SIZEOF( PUBKEY_P256 ) * CHAR_BIT,
                        DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH );

    /*
    ** Read Issuer key - don't reverse bytes, just copy
    */
    COPY_FROMBUFFER(
            f_poSignatureInfo->IssuerKey.rgb,
            f_pbCertData, 
            dwOffset,
            SIZEOF( PUBKEY_P256 ), 
            f_cbCertData );

    *f_pbOffset = dwOffset;
    
ErrorExit:

    return dr;
}


/**********************************************************************************************************
** Function:    _parseExtDataContainer
**
** Synopsis:    Parse extended data info object. This function does not validate signature data,
**              it only checks data for correct size and values.
**
** Arguments:   [f_pbCertData]         : data buffer that is being parsed
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poExtDataContainer] : pointer to a extended data info structure, cannot be NULL
**            [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to ext data info object not including object header.
**********************************************************************************************************/
static DRM_RESULT _parseExtDataContainer(
    const CRYPTO_BYTE                 *f_pbCertData,
    const CRYPTO_DWORD                f_cbCertData,
    CRYPTO_DWORD                      *f_pbOffset,
    DRM_BCERT_EXTDATA_CONTAINER    *f_poExtDataContainer,
    DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_poExtDataContainer != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read ext data object header
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, &dwOffset, &f_poExtDataContainer->Records[0].Header ) );
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Records[0].Header.wType == DRM_BCERT_OBJTYPE_EXTDATA_HWID
                            && f_poExtDataContainer->Records[0].Header.wFlags == DRM_BCERT_OBJFLAG_EMPTY,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    /*
    ** Number of records is always 1
    */
    f_poExtDataContainer->cbRecordsCount = 1;
    
    /* 
    ** Read record length
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poExtDataContainer->Records[0].dwDataLength,
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    
    /*
    ** Read record data: do not copy data record into any buffer, 
    ** only need to set a pointer to where it starts.
    */
    ChkOverflow( dwOffset + f_poExtDataContainer->Records[0].dwDataLength, dwOffset );
    ChkBOOL( dwOffset + f_poExtDataContainer->Records[0].dwDataLength <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    f_poExtDataContainer->Records[0].pbData = (CRYPTO_BYTE*) (f_pbCertData + dwOffset);

    /*
    ** Skip down to the signature.
    */
    ChkOverflow( dwOffset + f_poExtDataContainer->Records[0].dwDataLength, dwOffset );
    ChkOverflow( dwOffset + f_poExtDataContainer->Records[0].dwDataLength + PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength ), dwOffset + f_poExtDataContainer->Records[0].dwDataLength );
    dwOffset += f_poExtDataContainer->Records[0].dwDataLength + PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength );
    
    /*
    ** Read ext data signature object header
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, &dwOffset, &f_poExtDataContainer->Signature.Header ) );
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Signature.Header.wType == DRM_BCERT_OBJTYPE_EXTDATASIGNATURE
                           && f_poExtDataContainer->Signature.Header.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    /*
    ** Read and verify signature type
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( 
            f_poExtDataContainer->Signature.wSignatureType, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Signature.wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256, 
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Read and verify signature data length
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( 
            f_poExtDataContainer->Signature.SignatureData.cb, 
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Signature.SignatureData.cb == SIZEOF( SIGNATURE_P256 ),
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Read signature
    */
    ChkOverflow( f_poExtDataContainer->Signature.SignatureData.cb + PAD_AMOUNT( f_poExtDataContainer->Signature.SignatureData.cb ), f_poExtDataContainer->Signature.SignatureData.cb );
    COPY_FROMBUFFER(
            f_poExtDataContainer->Signature.SignatureData.rgb, 
            f_pbCertData,
            dwOffset,
            f_poExtDataContainer->Signature.SignatureData.cb
                + PAD_AMOUNT( f_poExtDataContainer->Signature.SignatureData.cb ),
            f_cbCertData );

    *f_pbOffset = dwOffset;
    
ErrorExit:

    return dr;
}
#if 0
/**********************************************************************************************************
** Function:    DRM_BCert_VerifySignature
**
** Synopsis:    Check that the Signature Data matches the ECDSA algorithm over the data
**              up to and not including signature element using IssuerKey in Signature Data.
**
** Arguments:   [f_pbCertificateBegin] : pointer to a place in a data buffer where signed data begins.
**              [f_dwSignedDataLength] : length in bytes of a signed data.
**              [f_poSignatureInfo]    : pointer to a signature info structure, cannot be NULL.
**              [f_pCryptoCtx]         : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE
**              DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH
**
**********************************************************************************************************/
DRM_RESULT DRM_BCert_VerifySignature(
    const CRYPTO_BYTE *f_pbCertificateBegin,
    CRYPTO_DWORD                f_dwSignedDataLength,
    DRM_BCERT_SIGNATURE_INFO *f_poSignatureInfo,
    DRM_CRYPTO_CONTEXT       *f_pCryptoCtx,
    DRM_BB_CONTEXT           *f_pcontextBBX,
    DRM_SECSTORE_CONTEXT     *f_pcontextSST,
    DRM_DST                  *f_pDatastore )
{
    DRM_RESULT     dr         = DRM_SUCCESS;
    SIGNATURE_P256 oSignature = { 0 };
    PUBKEY_P256    oPubKey    = { 0 };

    ChkArg( f_pbCertificateBegin != NULL );
    ChkArg( f_dwSignedDataLength > 0 );
    ChkArg( f_poSignatureInfo != NULL );
    ChkArg( f_pCryptoCtx != NULL );

#if DRM_SUPPORT_CERTIFICATE_CACHING
    if( f_pcontextBBX != NULL && f_pcontextSST != NULL && f_pDatastore != NULL )
    {
        if( DRM_UTL_VerifyCachedCertificate(    f_pcontextBBX, 
                                                f_pcontextSST,
                                                f_pDatastore, 
                                                f_pbCertificateBegin, 
                                                f_dwSignedDataLength ) )
        {
            /* Signature was found in the cache */
            dr = CRYPTO_SUCCESS;
            goto ErrorExit;
        }
        /* Signature not found, we'll have to validate asymmetrically */
    }
#endif

    /*
    ** Verify signature
    */    
    DRM_BYT_CopyBytes( oSignature.m_rgbSignature, 
                       0, 
                       f_poSignatureInfo->SignatureData.rgb, 
                       0, 
                       SIZEOF( SIGNATURE_P256 ) );

    DRM_BYT_CopyBytes( oPubKey.m_rgbPubkey, 
                       0, 
                       f_poSignatureInfo->IssuerKey.rgb, 
                       0, 
                       SIZEOF( PUBKEY_P256 ) );
    
    ChkDR( DRM_ECDSA_Verify_P256(
            f_pbCertificateBegin, 
            f_dwSignedDataLength,
            &oPubKey,
            &oSignature,
            (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ) );

#if DRM_SUPPORT_CERTIFICATE_CACHING
    if( f_pcontextBBX != NULL && f_pcontextSST != NULL && f_pDatastore != NULL )
    {
        /* Signature checked out, add it to the cache */
        ChkDR( DRM_UTL_AddCachedCertificate(f_pcontextBBX, 
                                            f_pcontextSST,
                                            f_pDatastore, 
                                            f_pbCertificateBegin, 
                                            f_dwSignedDataLength ) );
    }
#endif

ErrorExit:

    return dr;
}


/**********************************************************************************************************
** Function:    _verifyExtDataSignature
**
** Synopsis:    Verifies signature over extended data record.
**
** Arguments:   [f_pbCertData]           : data buffer that is being parsed
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pbSignedDataLength]   : Offset to where extended data record begins.
**              [f_poExtDataSignKeyInfo] : pointer to extended data signing key structure, cannot be NULL.
**              [f_poExtDataContainer]   : pointer to extended data container object, cannot be NULL.
**              [f_pVerificationContext] : pointer to DRM_BCERT_VERIFICATIONCONTEXT structure.
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE
**              DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH
**
**********************************************************************************************************/
static DRM_RESULT _verifyExtDataSignature(
    const CRYPTO_BYTE         *f_pbCertData,
    const CRYPTO_DWORD        f_cbCertData,
    CRYPTO_DWORD                                   f_pbSignedDataOffset,
    DRM_BCERT_EXTDATA_SIGNKEY_INFO *f_poExtDataSignKeyInfo,
    DRM_BCERT_EXTDATA_CONTAINER    *f_poExtDataContainer,
    DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )
{
    DRM_RESULT     dr                 = DRM_SUCCESS;
    SIGNATURE_P256 oSignature         = { 0 };
    PUBKEY_P256    oPubKey            = { 0 };
    CRYPTO_DWORD      dwSignedDataLength = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkOverflow( f_cbCertData, f_pbSignedDataOffset );
    ChkArg( f_poExtDataSignKeyInfo != NULL );
    ChkArg( f_poExtDataContainer != NULL );
    ChkArg( f_pVerificationContext != NULL );

    DRMASSERT( f_pVerificationContext->fDontFailOnMissingExtData == FALSE );

    DRM_BYT_CopyBytes( oSignature.m_rgbSignature, 
                       0, 
                       f_poExtDataContainer->Signature.SignatureData.rgb,
                       0, 
                       SIZEOF( SIGNATURE_P256 ) );

    DRM_BYT_CopyBytes( oPubKey.m_rgbPubkey, 
                       0,
                       f_poExtDataSignKeyInfo->Value.rgb,
                       0, 
                       SIZEOF( PUBKEY_P256 ) );

    dwSignedDataLength = SIZEOF( DRM_BCERT_OBJECT_HEADER ) + SIZEOF( CRYPTO_DWORD ); /* ext data record header & record length */

    ChkOverflow(dwSignedDataLength + f_poExtDataContainer->Records[0].dwDataLength, dwSignedDataLength );
    dwSignedDataLength += f_poExtDataContainer->Records[0].dwDataLength; /* record data */

    ChkOverflow( dwSignedDataLength + PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength ), dwSignedDataLength );
    dwSignedDataLength += PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength ); /* padding */

    ChkOverflow( f_cbCertData, f_pbSignedDataOffset + dwSignedDataLength );
    
    /*
    ** verify signature over extended data
    */
    ChkArg( f_pVerificationContext->pCryptoCtx != NULL );

    ChkDR( DRM_ECDSA_Verify_P256( f_pbCertData + f_pbSignedDataOffset, 
                                  dwSignedDataLength,
                                  &oPubKey,
                                  &oSignature,
                                  (struct bigctx_t *)f_pVerificationContext->pCryptoCtx->rgbCryptoContext ) );

ErrorExit:

    return dr;
}
#endif

/*****************************************************************************
** Function:    _parseKeyInfo
**
** Synopsis:    Parses key info object and fills-in key info structure.
**
** Arguments:   [f_pbCertData] : data buffer that is being parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_dwCertType] : type of a certificate being parsed
**              [f_poKeyInfo]  : pointer to a signature info structure, cannot be NULL
**    [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_KEY_TYPE
**              DRM_E_BCERT_INVALID_KEY_LENGTH
**              DRM_E_BCERT_INVALID_MAX_KEY_USAGES
**
** Notes:       f_pbOffset points to key info data not including object header.
**
******************************************************************************/

static DRM_RESULT  _parseKeyInfo( 
    const CRYPTO_BYTE                *f_pbCertData,
    const CRYPTO_DWORD               f_cbCertData,
    CRYPTO_DWORD                     *f_pbOffset,
    CRYPTO_DWORD                     f_dwCertType,
    DRM_BCERT_KEY_INFO            *f_poKeyInfo,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr                      = DRM_SUCCESS;
    CRYPTO_DWORD  iCount                  = 0;
    CRYPTO_DWORD  iKey                    = 0;
    CRYPTO_DWORD  dwOffset                = 0;
    CRYPTO_BOOL   fCheckRequiredKeyUsages = FALSE;
    CRYPTO_DWORD  dwKeyUsageMask          = 0;
    CRYPTO_DWORD  dwRequiredKeyUsageMask  = 0;
    

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poKeyInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read number of certificate keys
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poKeyInfo->dwNumKeys,
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    ChkBOOL( f_poKeyInfo->dwNumKeys > 0, DRM_E_BCERT_PUBLIC_KEY_NOT_SPECIFIED );

    ChkBOOL(f_poKeyInfo->dwNumKeys <= DRM_BCERT_MAX_KEY_USAGES, DRM_E_BCERT_TOO_MANY_PUBLIC_KEYS );

    for ( iKey = 0; iKey < f_poKeyInfo->dwNumKeys; iKey++ )
    {
        /*
        ** Read key type
        */
        NETWORKBYTES_FROMBUFFER_TO_WORD( 
                f_poKeyInfo->rgoKeys[iKey].wType, 
                f_pbCertData, 
                dwOffset,
                f_cbCertData );

        ChkVerificationErr( f_pVerificationContext, 
                            f_poKeyInfo->rgoKeys[iKey].wType == DRM_BCERT_KEYTYPE_ECC256, 
                            DRM_E_BCERT_INVALID_KEY_TYPE );

        /*
        ** Read key length
        */
        NETWORKBYTES_FROMBUFFER_TO_WORD( 
                f_poKeyInfo->rgoKeys[iKey].wLength, 
                f_pbCertData, 
                dwOffset,
                f_cbCertData );
       
        ChkVerificationErr( f_pVerificationContext, f_poKeyInfo->rgoKeys[iKey].wLength == SIZEOF( PUBKEY_P256 ) * CHAR_BIT, 
                 DRM_E_BCERT_INVALID_KEY_LENGTH );

        /*
        ** Read flags
        */
        NETWORKBYTES_FROMBUFFER_TO_DWORD( 
                f_poKeyInfo->rgoKeys[iKey].dwFlags, 
                f_pbCertData, 
                dwOffset,
                f_cbCertData );

        /*
        ** Read key value - don't reverse bytes, just copy
        */
        COPY_FROMBUFFER(
                f_poKeyInfo->rgoKeys[iKey].Value.rgb, 
                f_pbCertData, 
                dwOffset,
                SIZEOF( PUBKEY_P256 ),
                f_cbCertData );
        /*
        ** Read and validate number of key usage entries
        */
        NETWORKBYTES_FROMBUFFER_TO_DWORD( 
                f_poKeyInfo->rgoKeys[iKey].dwNumUsageEntries, 
                f_pbCertData, 
                dwOffset,
                f_cbCertData );
        
        ChkBOOL( f_poKeyInfo->rgoKeys[iKey].dwNumUsageEntries > 0
                     && f_poKeyInfo->rgoKeys[iKey].dwNumUsageEntries <= DRM_BCERT_MAX_KEY_USAGES, 
                 DRM_E_BCERT_INVALID_MAX_KEY_USAGES );
    
        /*
        ** Read and verify key usage entries
        */
        for ( iCount = 0; iCount < f_poKeyInfo->rgoKeys[iKey].dwNumUsageEntries; iCount++ )
        {
            NETWORKBYTES_FROMBUFFER_TO_DWORD( 
                    f_poKeyInfo->rgoKeys[iKey].dwUsageSet[iCount],
                    f_pbCertData, 
                    dwOffset,
                    f_cbCertData );
                                      
            dr = _verifyKeyUsage( f_poKeyInfo->rgoKeys[iKey].dwUsageSet[iCount], f_dwCertType );
            ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
            dr = CRYPTO_SUCCESS;

            if ( fCheckRequiredKeyUsages )
            {
                dwKeyUsageMask |= BCERT_KEYUSAGE_BIT( f_poKeyInfo->rgoKeys[iKey].dwUsageSet[ iCount ] ); /* collect cert key usages */
            }
        }
    }

    if ( fCheckRequiredKeyUsages )
    {
        for ( iCount = 0; iCount < f_pVerificationContext->cRequiredKeyUsages; iCount++ )
        {
            dwRequiredKeyUsageMask |= BCERT_KEYUSAGE_BIT( f_pVerificationContext->rgdwRequiredKeyUsages[iCount] );/* collect required key usages */
        }
        /*
        ** See that all required key usages are present
        */
        ChkBOOL( ( dwKeyUsageMask & dwRequiredKeyUsageMask ) == dwRequiredKeyUsageMask, DRM_E_BCERT_REQUIRED_KEYUSAGE_MISSING );
    }

    *f_pbOffset = dwOffset;

ErrorExit:

    return dr;
}


/*****************************************************************************
** Function:    _parseCertificate
**
** Synopsis:    Internal function that parses one certificate in a chain. 
**              For performance reasons it does not verify certificate signature.
**              Function may be called in two modes:
**              - If called by DRM_BCert_GetCertificate() it will fill in the whole 
**                DRM_BCERT_CERTIFICATE structure. 
**              - If called by DRM_BCert_ParseCertificateChain() it will remember 
**                only minimal info from a cert needed for further verification.
**
** Arguments:   [f_pbCertData]    : data buffer that is being parsed
**              [f_cbCertData]    : size of the data buffer (in bytes)
**              [f_pbOffset]      : position to start parsing from
**              [f_poCertificate] : output parameter, pointer to a full certificate structure.
**                                  Cannot be NULL if f_poMinCertData == NULL.
**                                  Should be NULL if f_poMinCertData != NULL.
**              [f_poMinCertData] : output parameter, pointer to a structure to store minimal cert info.
**                                  Cannot be NULL if f_poCertificate == NULL.
**                                  Should be NULL if f_poCertificate != NULL.
**              [f_pVerificationContext] : Pointer to DRM_BCERT_VERIFICATIONCONTEXT structure. Cannot be NULL.
**
**
** Returns:     CRYPTO_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER
**              DRM_E_BCERT_HWIDINFO_IS_MISSING
******************************************************************************/
static DRM_RESULT _parseCertificate(
    const CRYPTO_BYTE                *f_pbCertData,
    const CRYPTO_DWORD               f_cbCertData,
    CRYPTO_DWORD                     *f_pbOffset,
    DRM_BCERT_CERTIFICATE         *f_poCertificate,
    DRM_BCERT_MINIMALDATA         *f_poMinCertData,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT               dr                = DRM_SUCCESS;
    CRYPTO_DWORD                dwCertificateType = 0;    
    CRYPTO_DWORD                dwCertFlags       = DRM_BCERT_FLAGS_EMPTY;
    DRM_BCERT_OBJECT_HEADER  oObjHeader        = { 0 };
    CRYPTO_VOID                 *pHelperPtr       = NULL;
    DRM_BCERT_EXTDATA_SIGNKEY_INFO oExtDataSignKeyInfo = { 0 }; /* if present this data is needed to verify ext data signature */
    CRYPTO_DWORD                dwCertBegin       = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poCertificate != NULL || f_poMinCertData != NULL );
    ChkArg( f_poCertificate == NULL || f_poMinCertData == NULL );
    ChkArg( f_pVerificationContext != NULL );
    
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwCertBegin = *f_pbOffset; /* Remember where the cert begins */

    /* 
    ** Certificate header.
    */
    pHelperPtr = f_poCertificate != NULL ? &f_poCertificate->Header : &f_poMinCertData->oCertHeader;
    ChkDR( _parseCertHeader( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             ( DRM_BCERT_HEADER* ) pHelperPtr,
                             f_pVerificationContext ) );    
    /* 
    ** Basic info always follows certificate header.
    */
    pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->BasicInfo : &f_poMinCertData->oBasicInfo;
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &(( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header ) );

    ChkVerificationErr( f_pVerificationContext,
                        (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header.wFlags   == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                           && (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header.wType    == DRM_BCERT_OBJTYPE_BASIC
                           && (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header.cbLength == DRM_BCERT_BASIC_INFO_OBJECT_LEN,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    ChkDR( _parseCertBasicInfo( f_pbCertData,
                                f_cbCertData,
                                f_pbOffset,
                                ( DRM_BCERT_BASIC_INFO* )pHelperPtr,
                                f_pVerificationContext ) );

    /* 
    ** Remember certificate type and flags
    */
    dwCertificateType = (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->dwType; 
    dwCertFlags = (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->dwFlags;      
        
    /* 
    ** Get optional type-specific data object.
    ** Object type should match certificate type in basic info
    ** for device, PC, SilverLight, Metering, Server and Domain certificates.
    ** This code assumes that if a certificate type is Domain, PC, SilverLight, Metering, Server
    ** or Device then type-specific information must be present.
    ** Certificates of other types should not have a type-specific data object.
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );

    switch ( dwCertificateType )
    {
    case DRM_BCERT_CERTTYPE_DOMAIN:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_DOMAIN, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_DOMAIN_INFO* )&f_poCertificate->Type )->Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseDomainInfo( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 f_poCertificate != NULL ? ( DRM_BCERT_DOMAIN_INFO* )&f_poCertificate->Type : NULL,
                                 f_pVerificationContext ) );
        
        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;
        
    case DRM_BCERT_CERTTYPE_PC:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_PC, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_PC_INFO* )&f_poCertificate->Type )->Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parsePCInfo( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             f_poCertificate != NULL ? ( DRM_BCERT_PC_INFO* )&f_poCertificate->Type : NULL ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_SILVERLIGHT:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SILVERLIGHT, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_SILVERLIGHT_INFO* )&f_poCertificate->Type )->Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseSilverLightInfo( f_pbCertData,
                                      f_cbCertData,
                                      f_pbOffset,
                                      f_poCertificate != NULL ? ( DRM_BCERT_SILVERLIGHT_INFO* )&f_poCertificate->Type : NULL,
                                      f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_DEVICE:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags   == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                               && oObjHeader.wType    == DRM_BCERT_OBJTYPE_DEVICE
                               && oObjHeader.cbLength == DRM_BCERT_DEVICE_INFO_OBJECT_LEN, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );
                                
        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_DEVICE_INFO* )&f_poCertificate->Type )->Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseDeviceInfo( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 f_poCertificate != NULL ? ( DRM_BCERT_DEVICE_INFO* )&f_poCertificate->Type : NULL,
                                 f_pVerificationContext ) );
        
        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_METERING:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                               && oObjHeader.wType  == DRM_BCERT_OBJTYPE_METERING, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_METERING_INFO* )&f_poCertificate->Type )->Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseMeteringInfo( f_pbCertData,
                                   f_cbCertData,
                                   f_pbOffset,
                                   f_poCertificate != NULL ? ( DRM_BCERT_METERING_INFO* )&f_poCertificate->Type : NULL,
                                   f_pVerificationContext ) );
        
        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );    
        break;
        
    case DRM_BCERT_CERTTYPE_SERVER:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SERVER, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_SERVER_INFO* )&f_poCertificate->Type )->Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseServerInfo( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             f_poCertificate != NULL ? ( DRM_BCERT_SERVER_INFO* )&f_poCertificate->Type : NULL,
                             f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;
        
    case DRM_BCERT_CERTTYPE_ISSUER:    
    case DRM_BCERT_CERTTYPE_CRL_SIGNER:
    case DRM_BCERT_CERTTYPE_SERVICE:
    case DRM_BCERT_CERTTYPE_APPLICATION:
    case DRM_BCERT_CERTTYPE_KEYFILESIGNER:
        break; /* there is no type-specific info object for these cert types */
        
    default:
        ChkVerificationErr( f_pVerificationContext, FALSE, DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER ); /* wrong object type */
        break;
    }

    /* 
    ** Feature info must be present
    */
    ChkVerificationErr( f_pVerificationContext,
                        oObjHeader.wType    == DRM_BCERT_OBJTYPE_FEATURE
                             && oObjHeader.wFlags   == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                             && oObjHeader.cbLength >= DRM_BCERT_FEATURES_OBJECT_MIN_LEN,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    if ( f_poCertificate != NULL )
    {
        DRM_BYT_CopyBytes( &f_poCertificate->FeatureInfo.Header, 
                           0, 
                           &oObjHeader, 
                           0, 
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }
    if ( f_poMinCertData != NULL )
    {
        DRM_BYT_CopyBytes( &f_poMinCertData->oFeatureInfo.Header,
                           0, 
                           &oObjHeader, 
                           0, 
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }    
    pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->FeatureInfo : &f_poMinCertData->oFeatureInfo;
    ChkDR( _parseFeatureInfo( f_pbCertData,
                              f_cbCertData,
                              f_pbOffset,
                              dwCertificateType,
                              ( DRM_BCERT_FEATURE_INFO* ) pHelperPtr,
                              f_pVerificationContext ) );

    /* 
    ** Key info - object must be present
    */
    pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->KeyInfo : &f_poMinCertData->oKeyInfo;

    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &(( DRM_BCERT_KEY_INFO* )pHelperPtr)->Header ) );

    ChkVerificationErr( f_pVerificationContext,
                        (( DRM_BCERT_KEY_INFO* )pHelperPtr)->Header.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                             && (( DRM_BCERT_KEY_INFO* )pHelperPtr)->Header.wType  == DRM_BCERT_OBJTYPE_KEY, 
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );
    
    ChkDR( _parseKeyInfo( f_pbCertData,
                          f_cbCertData,
                          f_pbOffset,
                          dwCertificateType,
                          ( DRM_BCERT_KEY_INFO* ) pHelperPtr,
                          f_pVerificationContext ) );

    /* 
    ** Manufacturer info - optional 
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );

    if ( oObjHeader.wType == DRM_BCERT_OBJTYPE_MANUFACTURER )
    {
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_EMPTY, 
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER ); 
    
        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &f_poCertificate->ManufacturerInfo.Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseManufacturerInfo( f_pbCertData,
                                       f_cbCertData,
                                       f_pbOffset,
                                       ( f_poCertificate != NULL ) ? &f_poCertificate->ManufacturerInfo : NULL,
                                       f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
    }

    /*
    ** Check for presence of a public key object to verify extended data
    ** also indicated by a flag in a basic info.
    ** For now only PC certs can have it, all other types do not (already verified this condition)
    */
    if ( (dwCertFlags & DRM_BCERT_FLAGS_EXTDATA_PRESENT) == DRM_BCERT_FLAGS_EXTDATA_PRESENT )
    {
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_EMPTY
                               && oObjHeader.wType  == DRM_BCERT_OBJTYPE_EXTDATASIGNKEY,
                            DRM_E_BCERT_EXTDATA_IS_NOT_PROVIDED );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &f_poCertificate->ExtDataSignatureInfo.Header, 
                               0, 
                               &oObjHeader, 
                               0, 
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseExtDataSignKeyInfo( f_pbCertData,
                                         f_cbCertData,
                                         f_pbOffset,
                                         ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataSignatureInfo : &oExtDataSignKeyInfo,
                                         f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
    }
    else
    {
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SIGNATURE,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );
    }
    
    /* 
    ** The last object (with exception of PC cert) is signature
    ** which must be present. Header is already read.
    */
    ChkVerificationErr( f_pVerificationContext,
                        oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                           && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SIGNATURE,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    if ( f_poCertificate != NULL )
    {
        DRM_BYT_CopyBytes( &f_poCertificate->SignatureInfo.Header, 
                           0, 
                           &oObjHeader, 
                           0, 
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }
    if ( f_poMinCertData != NULL )
    {
        DRM_BYT_CopyBytes( &f_poMinCertData->oSignatureInfo.Header,
                           0, 
                           &oObjHeader, 
                           0, 
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }    
    ChkDR( _parseSignatureInfo( f_pbCertData,
                                f_cbCertData,
                                f_pbOffset,
                                ( f_poCertificate != NULL ) ? &f_poCertificate->SignatureInfo : &f_poMinCertData->oSignatureInfo,
                                f_pVerificationContext ) );
#if 0
    /*
    ** PC certificate should have signed extended data information.
    ** It follows certificate signature object.
    ** Sometimes the caller of the cert parser allows or expects partially built certificates
    ** and sets f_pVerificationContext->fDontFailOnMissingExtData into TRUE. In this case
    ** if extended data object is present we verify the basics (obj header and structure), 
    ** but do not verify its signature. If extended data is missing do not fail.
    */
    if ( (dwCertFlags & DRM_BCERT_FLAGS_EXTDATA_PRESENT) == DRM_BCERT_FLAGS_EXTDATA_PRESENT )
    {
        DRM_BCERT_EXTDATA_CONTAINER oExtDataContainer    = { 0 };
        CRYPTO_DWORD                   dwExtDataObjectStart = 0;

        if ( CRYPTO_SUCCEEDED(_getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oExtDataContainer.Header ))
                && oExtDataContainer.Header.wType == DRM_BCERT_OBJTYPE_EXTDATACONTAINER
                && oExtDataContainer.Header.wFlags == (DRM_BCERT_OBJFLAG_MUST_UNDERSTAND | DRM_BCERT_OBJFLAG_CONTAINER_OBJ) )
        {        

            if ( f_poCertificate != NULL )
            {
                DRM_BYT_CopyBytes( &f_poCertificate->ExtDataContainer.Header, 
                                   0, 
                                   &oExtDataContainer.Header, 
                                   0, 
                                   SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
            }

            /* 
            ** Remember where extended data record object starts for signature verification.
            ** Container header has just been read, so this is the right place.
            */
            dwExtDataObjectStart = *f_pbOffset;
          
            ChkDR( _parseExtDataContainer( f_pbCertData,
                                           f_cbCertData,
                                           f_pbOffset,
                                           ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataContainer : &oExtDataContainer,
                                           f_pVerificationContext ) );
            /*
            ** Verify extended data signature
            */
            if ( !f_pVerificationContext->fDontFailOnMissingExtData )
            {
                ChkDR( _verifyExtDataSignature( f_pbCertData, 
                                                f_cbCertData,
                                                dwExtDataObjectStart,
                                                ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataSignatureInfo : &oExtDataSignKeyInfo,
                                                ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataContainer : &oExtDataContainer,
                                                f_pVerificationContext ) );
            }
            
            /*
            ** Reserved space for extended data can be larger than the actual extended data objects.
            ** Need to update offset correctly to point to the beginning of the next cert.
            */
            ChkOverflow( dwCertBegin + ( ( f_poCertificate != NULL ) ? f_poCertificate->Header.cbCertificate : f_poMinCertData->oCertHeader.cbCertificate ), dwCertBegin );
            ChkBoundsLT( dwCertBegin + ( ( f_poCertificate != NULL ) ? f_poCertificate->Header.cbCertificate : f_poMinCertData->oCertHeader.cbCertificate ), f_cbCertData );
            *f_pbOffset = dwCertBegin + ( ( f_poCertificate != NULL ) ? f_poCertificate->Header.cbCertificate : f_poMinCertData->oCertHeader.cbCertificate );
        }
        else
        {
            /*
            ** This is a "partially built" certificate,
            ** Record DRM_E_BCERT_HWIDINFO_IS_MISSING code.
            ** The caller will calculate where the next cert begins.
            */
            if ( !f_pVerificationContext->fDontFailOnMissingExtData )
            {
                ChkVerificationErr( f_pVerificationContext, FALSE, DRM_E_BCERT_HWIDINFO_IS_MISSING );
            }
        }
    }
#endif

ErrorExit:
    return dr;
}


/********************************************************************************************
** Function:    DRM_BCert_FindObjectInCertByType
**
** Synopsis:    The function enumerates all objects in a cert until it finds the one
**              with a matching type, or reaches end of data buffer, or the last object
**              in certificate which a signature object.
**
** Arguments:   [f_pbCertData]   : data buffer to parse
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from, should point
**                                 to the beginning of some object's header
**                                 or the beginning of the certificate header 
**                                 (a special case since cert header is not an object).
**                                 On exit if succeeded this offset should point to the beginning
**                                 of an object (including its header!)
**              [f_dwObjectType] : object type
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_NOMORE
**              DRM_E_ARITHMETIC_OVERFLOW - comes from _getObjectHeader
**              DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG - comes from _getObjectHeader
********************************************************************************************/
DRM_RESULT DRM_BCert_FindObjectInCertByType(
    const CRYPTO_BYTE    *f_pbCertData,
    const CRYPTO_DWORD   f_cbCertData,
    CRYPTO_DWORD         *f_pbOffset,
    const CRYPTO_DWORD   f_dwObjectType )
{
    DRM_RESULT              dr          = DRM_SUCCESS;
    CRYPTO_DWORD               dwOffset    = 0;
    CRYPTO_DWORD               dwHeaderTag = 0;
    DRM_BCERT_OBJECT_HEADER oObjHeader  = { 0 };
    
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );
    ChkArg( f_dwObjectType <= DRM_BCERT_OBJTYPE_MAX_VALUE );

    /*
    ** Check for special case:
    ** offset could point to the beginning of the cert which is not an object.
    ** If header is found skip over it.
    */
    dwOffset = *f_pbOffset;
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwHeaderTag, 
                                      f_pbCertData, 
                                      dwOffset,
                                      f_cbCertData );    
    if( dwHeaderTag == DRM_BCERT_HEADER_TAG )
    {
        ChkOverflow( *f_pbOffset + SIZEOF( DRM_BCERT_HEADER ), *f_pbOffset );
       *f_pbOffset += SIZEOF( DRM_BCERT_HEADER );    
    }
    
    /*
    ** loop thru the objects until we reach
    ** end of data, the last object in cert,
    ** or object of sought type
    */
    dr = DRM_E_NOMORE;
    
    while ( *f_pbOffset < f_cbCertData )
    {
        ChkDR( _getObjectHeader( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 &oObjHeader ) );
        
        if ( oObjHeader.wType == f_dwObjectType )
        {
            dr = DRM_SUCCESS;
            break;
        }
        /*
        ** This is the last object in a cert and we are not looking for signature object.
        */    
        if ( oObjHeader.wType == DRM_BCERT_OBJTYPE_SIGNATURE )
        {
            dr = DRM_E_NOMORE;
            break;
        }
        /*
        ** go to the next object
        */
        ChkBOOL( SIZEOF( DRM_BCERT_OBJECT_HEADER ) <= oObjHeader.cbLength, 
                 DRM_E_BCERT_OBJECTHEADER_LEN_TOO_SMALL );
        ChkOverflow( *f_pbOffset + oObjHeader.cbLength - SIZEOF( DRM_BCERT_OBJECT_HEADER ), *f_pbOffset );
        *f_pbOffset += oObjHeader.cbLength - SIZEOF( DRM_BCERT_OBJECT_HEADER );
    }
    
ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    DRM_BCert_GetChainHeader
**
** Synopsis:    Parses a certificate chain header.
**
** Arguments:   [f_pbCertData] : data buffer that is being parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_poChain]    : pointer to a chain header structure, 
**                               output parameter, cannot be NULL
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_CHAIN_HEADER_TAG
**              DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH
**              DRM_E_BCERT_INVALID_CHAIN_VERSION
**              DRM_E_BCERT_INVALID_CHAIN_LENGTH
**
** Notes:  If you need to enumerate all certificates in a chain and read
**         their content into DRM_BCERT_CERTIFICATE structure call this API first
**         1) to ensure the data looks like a cert chain, 2) find out total number 
**         of certs in it, 3) move an offset to the beginning of the first certificate.
******************************************************************************/
DRM_RESULT DRM_BCert_GetChainHeader(
    const CRYPTO_BYTE                *f_pbCertData,
    const CRYPTO_DWORD               f_cbCertData,
    CRYPTO_DWORD                     *f_pbOffset,
    DRM_BCERT_CHAIN_HEADER        *f_poChain )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CRYPTO_DWORD  dwOffset = 0;


    ChkArg( f_pbCertData != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( *f_pbOffset < f_cbCertData );
    ChkArg( f_poChain != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Fill in a chain header structure
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poChain->dwHeaderTag,                 /* tag */
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poChain->dwVersion,                   /* version */
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poChain->cbChain,                     /* chain length */
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poChain->dwFlags,                     /* flags*/
            f_pbCertData, 
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD( 
            f_poChain->cCerts,                      /* number of certs in chain */
            f_pbCertData, 
            dwOffset,
            f_cbCertData );

    /*
    ** Verify that header constant tag is correct
    */
    ChkBOOL( f_poChain->dwHeaderTag == DRM_BCERT_CHAIN_HEADER_TAG, 
             DRM_E_BCERT_INVALID_CHAIN_HEADER_TAG );
    
    /*
    ** There should be 1-6 certificates in a chain
    */
    ChkBOOL( f_poChain->cCerts > 0, 
             DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );
    ChkBOOL( f_poChain->cCerts <= DRM_BCERT_MAX_CERTS_PER_CHAIN, 
             DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );
    /*
    ** Version is always 1
    */
    ChkBOOL( f_poChain->dwVersion == DRM_BCERT_CHAIN_VERSION, 
             DRM_E_BCERT_INVALID_CHAIN_VERSION );
    /*
    ** Chain length in bytes cannot be too small
    */
    ChkBOOL( f_poChain->cbChain >= DRM_MIN_CERT_CHAIN_LEN 
                                   + f_poChain->cCerts * DRM_MIN_CERTIFICATE_LEN, 
             DRM_E_BCERT_INVALID_CHAIN_LENGTH );
    /*
    ** There is enough data in a buffer to accomodate a chain length value
    */
    ChkOverflow( *f_pbOffset + f_poChain->cbChain, f_poChain->cbChain );
    ChkBOOL( f_cbCertData >= *f_pbOffset + f_poChain->cbChain, 
             DRM_E_BUFFERTOOSMALL );

    *f_pbOffset = dwOffset;

ErrorExit:
    if (f_poChain)
    {
        Printf("Invalid argument at %s : %d.\n%x\n", __FILE__, __LINE__,  f_poChain->dwHeaderTag) ;
        Printf("Invalid argument at %s : %d.\n%x\n", __FILE__, __LINE__,  f_poChain->dwVersion) ;
        Printf("Invalid argument at %s : %d.\n%x\n", __FILE__, __LINE__,  f_poChain->cbChain) ;
        Printf("Invalid argument at %s : %d.\n%x\n", __FILE__, __LINE__,  f_poChain->dwFlags) ;
        Printf("Invalid argument at %s : %d.\n%x\n", __FILE__, __LINE__,  f_poChain->cCerts) ;
    }	
    return dr;
}


/*****************************************************************************
** Function:    _setVerificationContext
**
** Synopsis:    Checks consistency of verification context structure and zeroes
**              the array of return results.
**
** Arguments:   [f_pVerificationContext] : pointer to verification context
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
******************************************************************************/
static DRM_RESULT _setVerificationContext(
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    ChkArg( f_pVerificationContext != NULL );
    
    ChkArg( f_pVerificationContext->dwType <= DRM_BCERT_CERTTYPE_MAX_VALUE );

    /*
    ** Crypto context cannot be NULL if the parser may be doing signature verification
    */
    ChkArg( ( f_pVerificationContext->fVerifyCertSignatures == FALSE 
            && f_pVerificationContext->fDontFailOnMissingExtData ) );

    f_pVerificationContext->cResults = 0;
    
    f_pVerificationContext->cCurrentCert = 0;
    
ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    DRM_BCert_InitVerificationContext
**
** Synopsis:    Initializes parser verification context.
**
** Arguments:   
**      [f_pftExpirationTime]           : these parameters correspond to the 
**      [f_pRootPubKey]                   data fields in verification context
**      [f_dwCertType]                    structure. All pointer params are optional.
**      [f_pCryptoCtx]                    For default values and meanings see
**      [f_fVerifyCertSignatures]         DRM_BCERT_VERIFICATIONCONTEXT structure
**      [f_fDontFailOnMissingExtData]     in drmbcertparser.h
**      [f_pKeyUsages]
**      [f_NumKeyUsages]
**      [f_fCollectErrs]                
**      [f_pVerificationContext]        : pointer to verification context
**                                        to be initialized. Cannot be NULL.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
******************************************************************************/
DRM_RESULT DRM_BCert_InitVerificationContext(
    CRYPTOFILETIME                      *f_pftExpirationTime,
    const PUBKEY_P256                *f_pRootPubKey,
    const CRYPTO_DWORD                  f_dwCertType,
    CRYPTO_BOOL                         f_fVerifyCertSignatures,
    CRYPTO_BOOL                         f_fDontFailOnMissingExtData,
    CRYPTO_DWORD                     *f_pKeyUsages,
    CRYPTO_DWORD                        f_NumKeyUsages,
    CRYPTO_BOOL                         f_fCollectErrs,
    DRM_BCERT_VERIFICATIONCONTEXT    *f_pVerificationContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pVerificationContext != NULL );
    
    //ChkArg( f_pCryptoCtx != NULL || ( f_fVerifyCertSignatures == FALSE && f_fDontFailOnMissingExtData == TRUE ) );
    ChkArg( f_pKeyUsages == NULL || ( f_NumKeyUsages > 0 && f_NumKeyUsages < DRM_BCERT_MAX_KEY_USAGES ) );

    memset( f_pVerificationContext, 0,  SIZEOF( DRM_BCERT_VERIFICATIONCONTEXT ) );
    
    if ( f_pftExpirationTime != NULL )
    {
        DRM_BYT_CopyBytes(&f_pVerificationContext->ftExpirationTime, 0, f_pftExpirationTime, 0, SIZEOF(CRYPTOFILETIME) );
    }

    f_pVerificationContext->pRootPubKey = f_pRootPubKey;
    f_pVerificationContext->dwType = f_dwCertType;
    //f_pVerificationContext->pCryptoCtx = f_pCryptoCtx;
    f_pVerificationContext->fVerifyCertSignatures = f_fVerifyCertSignatures;
    f_pVerificationContext->fDontFailOnMissingExtData = f_fDontFailOnMissingExtData;
    //f_pVerificationContext->pBlackboxCtx = f_pcontextBBX;
    //f_pVerificationContext->pSecStoreCtx = f_pcontextSST;
    //f_pVerificationContext->pDSTCtx = f_pDatastore;

    if ( f_pKeyUsages != NULL && f_NumKeyUsages > 0 )
    {
        ChkArg (f_NumKeyUsages < DRM_BCERT_MAX_KEY_USAGES-2);
        DRM_BYT_CopyBytes( f_pVerificationContext->rgdwRequiredKeyUsages, 0, f_pKeyUsages, 0, SIZEOF(CRYPTO_DWORD) * f_NumKeyUsages );
    }

    f_pVerificationContext->fCollectErrorsInArrayAndContinue = f_fCollectErrs;

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    DRM_BCert_ParseCertificateChain
**
** Synopsis:    The function parses binary data that represents device certificate chain
**              and verifies its validity. Optionally it can verify that all certs
**              are not expired, and the issuer key for of top certificate in a chain
**              matches specified value.
**
** Arguments:   [f_pbCertData]           : data buffer to parse
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pVerificationContext] : pointer to verification context structure that defines
**                                         optional verification conditions on the cert and may collect list of
**                                         verification errors not fatal for cert parser (i.e. parsing can continue).
**                                         Note that if on exit errors list is not empty this function will return
**                                         specific error code DRM_E_BCERT_VERIFICATION_ERRORS.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH
**              DRM_E_BCERT_VERIFICATION_ERRORS
******************************************************************************/
DRM_RESULT DRM_BCert_ParseCertificateChain(
    const CRYPTO_BYTE          *f_pbCertData,
    const CRYPTO_DWORD         f_cbCertData,
    DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    CRYPTO_RESULT             dr               = CRYPTO_SUCCESS;
    CRYPTO_DWORD              bCurrOffset      = 0;
    DRM_BCERT_CHAIN_HEADER oChainHeader     = { 0 };
    DRM_BCERT_MINIMALDATA  roCertData[2]    = { 0, 0 };
    CRYPTOFILETIME            ftCertTime       = { 0 };
    CRYPTO_UINT64             ui64CurrentTime  = DRM_UI64LITERAL( 0, 0 );
    CRYPTO_UINT64             uiCertTime       = DRM_UI64LITERAL( 0, 0 );


    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );    
    ChkDR( _setVerificationContext( f_pVerificationContext ) );
    
    /*
    ** Parse a cert chain header
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData, f_cbCertData, &bCurrOffset, &oChainHeader ) );

    /*
    ** Parse certificates in a chain
    */
    for ( f_pVerificationContext->cCurrentCert = 0; 
          f_pVerificationContext->cCurrentCert < oChainHeader.cCerts; 
          f_pVerificationContext->cCurrentCert++ )
    {        
        CRYPTO_DWORD dwCertificateBegin = bCurrOffset;

        /*
        ** Parse certificate and remember only data needed for verification purpose
        */
        ChkDR( _parseCertificate( f_pbCertData, 
                                  f_cbCertData, 
                                  &bCurrOffset, 
                                  NULL, 
                                  &roCertData[ f_pVerificationContext->cCurrentCert % 2 ], 
                                  f_pVerificationContext ) );
        /*
        ** Calculate where certificate ends
        */
        ChkOverflow( dwCertificateBegin + roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificate, dwCertificateBegin );
        bCurrOffset = dwCertificateBegin + roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificate;
        
        /*
        ** Match leaf certificate type if it is provided
        */
        if ( 0 == f_pVerificationContext->cCurrentCert
            && f_pVerificationContext->dwType > 0 )
        {
            ChkBOOL( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwType == f_pVerificationContext->dwType, 
                    DRM_E_BCERT_INVALID_CERT_TYPE );
        }
#if 0		
        /*
        ** Verify signature 
        */
        if( f_pVerificationContext->fVerifyCertSignatures )
        {
            ChkBoundsLT( dwCertificateBegin, f_cbCertData );
            ChkBoundsLT( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificateSigned, f_cbCertData - dwCertificateBegin );
            ChkDR( DRM_BCert_VerifySignature( f_pbCertData + dwCertificateBegin,
                                     roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificateSigned,
                                     &roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oSignatureInfo,
                                     f_pVerificationContext->pCryptoCtx,
                                     f_pVerificationContext->pBlackboxCtx,
                                     f_pVerificationContext->pSecStoreCtx,
                                     f_pVerificationContext->pDSTCtx ) );
        }
#endif		
        /*
        ** Check if the certificate is expired - optional
        */
        if ( DoExpirationCheck( f_pVerificationContext->ftExpirationTime ) )
        {
            FILETIME_TO_UI64( f_pVerificationContext->ftExpirationTime, ui64CurrentTime );
            CREATE_FILE_TIME( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwExpirationDate, ftCertTime );
            FILETIME_TO_UI64( ftCertTime, uiCertTime );    
            ChkBOOL( !CRYPTO_UI64Les( uiCertTime, ui64CurrentTime ), 
                    DRM_E_BCERT_BASICINFO_CERT_EXPIRED );
        }
        /*
        ** Additional verification between the certs
        */
        if ( f_pVerificationContext->cCurrentCert > 0 )
        {
            ChkDR( _verifyAdjacentCerts( &roCertData[(f_pVerificationContext->cCurrentCert + 1) % 2],
                                         &roCertData[f_pVerificationContext->cCurrentCert % 2],
                                         f_pVerificationContext ) );
        }
        /*
        ** Check that Issuer key of the last cert is f_pRootPubkey.
        */
        if ( f_pVerificationContext->pRootPubKey != NULL
            && f_pVerificationContext->cCurrentCert == oChainHeader.cCerts - 1 )
        {
            ChkBOOL( DRM_BYT_CompareBytes(
                        roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oSignatureInfo.IssuerKey.rgb,
                        0, 
                        f_pVerificationContext->pRootPubKey->m_rgbPubkey,
                        0,
                        SIZEOF( PUBKEY_P256 ) ) == 0,
                     DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH );            
        }
    }
    
ErrorExit:

    if ( f_pVerificationContext != NULL )
    {
        if ( f_pVerificationContext->cResults > 0 )
        {
            dr = DRM_E_BCERT_VERIFICATION_ERRORS;
        }
    }
    return dr;
}


/********************************************************************************************
** Function:    DRM_BCert_GetCertificate
**
** Synopsis:    Given the beginning of a certificate in a chain the function skips over it
**              and moves the offset to the next one. If DRM_BCERT_CERTIFICATE is not NULL
**              the certificate's content will be parsed and stored in it.
**
** Arguments:   [f_pbCertData]         : data buffer to parse
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from, should point
**                                       to the beginning of some certificate. On return
**                                       this value will be updated with the next cert's beginning.
**              [f_poCertificate]      : pointer to a certificate structure, optional.
**              [f_dwRequiredCertType] : expected type of certificate, set into DRM_BCERT_CERTTYPE_UNKNOWN
**                                       if it is not important. Otherwise the function will return
**                                       DRM_BCERT_CERTTYPE_UNKNOWN if type does not match.
**                                       Also this parameter is ignored if f_poCertificate == NULL.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_S_FALSE      - if it was the last certificate in chain.
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW
**              DRM_E_BCERT_INVALID_CERT_LENGTH
**              DRM_E_BCERT_INVALID_CERT_TYPE
**
** Note:  1. The function will not validate the certificate if f_poCertificate == NULL,
**           only skip over it. 
**
**        2. If f_poCertificate != NULL then cert data will be checked for compliance with
**           format only! Cert signature, expiration date and public key will NOT be verified.
**           Call DRM_BCert_ParseCertificateChain() API for full validation!
**
**        3. If DRM_S_FALSE is returned it means that there are no more certs in a chain.
**           The f_pbOffset will still be updated so that an attempt to use this value
**           somewhere else would fail.
**
**        4. To enumerate all certs in a chain first call DRM_BCert_GetChainHeader(),
**           then DRM_BCert_GetCertificate. See notes for DRM_BCert_GetChainHeader.
********************************************************************************************/
DRM_RESULT DRM_BCert_GetCertificate(
    const CRYPTO_BYTE         *f_pbCertData,
    const CRYPTO_DWORD        f_cbCertData,
    CRYPTO_DWORD              *f_pbOffset,
    DRM_BCERT_CERTIFICATE  *f_poCertificate,
    const CRYPTO_DWORD        f_dwRequiredCertType )
{
    CRYPTO_RESULT dr                  = CRYPTO_SUCCESS;
    CRYPTO_DWORD  dwOffset            = 0;
    CRYPTO_DWORD  dwCertificateLength = 0;
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );
    dwOffset = *f_pbOffset;

    if ( f_poCertificate != NULL )  /* parse current certificate */
    {
        ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                                  NULL,
                                                  f_dwRequiredCertType,
                                                  FALSE,
                                                  TRUE,
                                                  NULL,
                                                  0,
                                                  FALSE,
                                                  &oVerificationContext ) );

        ChkDR( _parseCertificate( f_pbCertData, 
                                  f_cbCertData, 
                                  &dwOffset,
                                  f_poCertificate,
                                  NULL,
                                  &oVerificationContext ) );

        dwCertificateLength = f_poCertificate->Header.cbCertificate;
    }
    else  /* only read the header */
    {
        DRM_BCERT_HEADER oCertHeader = { 0 };
        ChkDR( _parseCertHeader( f_pbCertData, f_cbCertData, &dwOffset, &oCertHeader, &oVerificationContext ) );

        dwCertificateLength = oCertHeader.cbCertificate;
    }
    ChkOverflow( *f_pbOffset + dwCertificateLength, *f_pbOffset );  /* cert length is not too big */
    ChkOverflow( f_cbCertData, *f_pbOffset + dwCertificateLength ); /* new offset is not too big */
    
    *f_pbOffset  += dwCertificateLength;  /* move the offset to the beginning of the next certificate */

    ChkBOOL( *f_pbOffset < f_cbCertData, DRM_S_FALSE ); /* otherwise we were at the last certificate in a chain */

ErrorExit:
    return dr;
}


/***********************************************************************************************************
** Function:    DRM_BCert_GetPublicKeyByUsage
**
** Synopsis:    The function retrieves the public key of the cert with specified 
**              key usage and index in a certificate chain. 
**              On success returns a key value and an array of all key usages for the found key.
**              If key usage is set into DRM_BCERT_KEYUSAGE_UNKNOWN then the first public key will be returned.
**
** Arguments:   [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**              [f_dwCertIndex]     : index of certificate in the chain. Leaf cert is 0.
**              [f_dwKeyUsage]      : requested key usage of public key.
**              [f_pPubkey]         : pointer to a public key structure
**              [f_pdwKeyUsages]    : array of key usages of the returned key. The caller is responsible
**                                    for allocating an array of size of DRM_BCERT_MAX_KEY_USAGES or less
**                                    (it's usually safe to allocate size of DRM_BCERT_KEYUSAGE_MAX_VALUE ).
**                                    This parameter is optional and can be NULL.
**              [f_pcbKeyUsages]    : if f_pdwKeyUsages != NULL then it's a pointer to the size of f_pdwKeyUsages array
**                                    which on success is equal to the actual number of key usage entries of the returned public key.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BUFFERTOOSMALL - if there is not enough room in f_pdwKeyUsages to save all key usages
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
************************************************************************************************************/
DRM_RESULT DRM_BCert_GetPublicKeyByUsage(
    const CRYPTO_BYTE    *f_pbCertData,
    const CRYPTO_DWORD    f_cbCertData,
    const CRYPTO_DWORD    f_dwCertIndex,
    const CRYPTO_DWORD    f_dwKeyUsage,
    PUBKEY_P256       *f_pPubkey,
    CRYPTO_DWORD         *f_pdwKeyUsages,
    CRYPTO_DWORD         *f_pcbKeyUsages )
{
    DRM_RESULT              dr             = DRM_SUCCESS;
    CRYPTO_DWORD               bCurrOffset    = 0;
    DRM_BCERT_CHAIN_HEADER  oChainHeader   = {0};
    CRYPTO_DWORD               iCertCounter   = 0;
    CRYPTO_DWORD               iKeyCounter    = 0;
    CRYPTO_DWORD               iUsageCounter  = 0;
    CRYPTO_BOOL                fFoundKey      = FALSE;
    CRYPTO_DWORD               iFoundKey      = 0;
    DRM_BCERT_KEY_INFO      oKeyInfo       = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pPubkey != NULL );
#if 0
    /*
    ** Parse a cert chain header and make sure
    ** that there are enough certs in the chain.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData, 
                                     f_cbCertData, 
                                     &bCurrOffset,
                                     &oChainHeader ) );

    ChkBOOL( oChainHeader.cCerts > f_dwCertIndex, DRM_E_BCERT_INVALID_CHAIN_LENGTH );
#endif
    /*
    ** bCurrOffset currently points to the 0-th leaf cert.
    ** Loop thru certificates if needed. 
    ** Do not parse, only look for the start position.
    */
    for ( iCertCounter = 1; iCertCounter <= f_dwCertIndex; iCertCounter++ )
    {
        ChkDR( DRM_BCert_GetCertificate( f_pbCertData,
                                         f_cbCertData,
                                         &bCurrOffset,
                                         NULL,
                                         DRM_BCERT_CERTTYPE_UNKNOWN ) );
    }

    //ZEROMEM( f_pPubkey->m_rgbPubkey, ECC_P256_POINT_SIZE_IN_BYTES );
    memset(f_pPubkey->m_rgbPubkey, 0, ECC_P256_POINT_SIZE_IN_BYTES );

    /*
    ** Loop thru the objects until we find
    ** a key information or reach the end
    */
    ChkDR( DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                             f_cbCertData,
                                             &bCurrOffset,
                                             DRM_BCERT_OBJTYPE_KEY ) );

    ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_UNKNOWN ,
                                              FALSE,
                                              TRUE,
                                              NULL,
                                              0,
                                              FALSE,
                                              &oVerificationContext ) );

    ChkDR( _parseKeyInfo( f_pbCertData,
                          f_cbCertData,
                          &bCurrOffset,
                          DRM_BCERT_CERTTYPE_UNKNOWN,
                          &oKeyInfo,
                          &oVerificationContext ) );

    if ( f_dwKeyUsage != DRM_BCERT_KEYUSAGE_UNKNOWN )
    {
        /*
        ** Find the first key with requested key usage
        */
        for ( iKeyCounter = 0; iKeyCounter < oKeyInfo.dwNumKeys; iKeyCounter++ )
        {
            for ( iUsageCounter = 0; iUsageCounter < oKeyInfo.rgoKeys[iKeyCounter].dwNumUsageEntries; iUsageCounter++ )
            {
                if ( !fFoundKey 
                    && f_dwKeyUsage == oKeyInfo.rgoKeys[iKeyCounter].dwUsageSet[iUsageCounter] )
                {
                    iFoundKey = iKeyCounter;
                    fFoundKey = TRUE;
                }
            }
        }
    }
    else
    {
        iFoundKey = 0; /* if any keyusage is requested just get the first public key */
        fFoundKey = TRUE;
    }
    
    /*
    ** If the loop is finished then the key with requested usage was not found
    */
    ChkBOOL( fFoundKey, DRM_E_BCERT_NO_PUBKEY_WITH_REQUESTED_KEYUSAGE );

    /*
    ** Copy key usages and the key value
    */
    if ( f_pdwKeyUsages != NULL )
    {
        ChkBOOL( *f_pcbKeyUsages >= oKeyInfo.rgoKeys[iFoundKey].dwNumUsageEntries, DRM_E_BUFFERTOOSMALL );

        DRM_BYT_CopyBytes( f_pdwKeyUsages,
                           0,
                           oKeyInfo.rgoKeys[iFoundKey].dwUsageSet,
                           0,
                           SIZEOF(CRYPTO_DWORD) * oKeyInfo.rgoKeys[iFoundKey].dwNumUsageEntries );
        *f_pcbKeyUsages = oKeyInfo.rgoKeys[iFoundKey].dwNumUsageEntries;
    }
    DRM_BYT_CopyBytes( f_pPubkey->m_rgbPubkey,
                       0, 
                       oKeyInfo.rgoKeys[iFoundKey].Value.rgb, 
                       0, 
                       ECC_P256_POINT_SIZE_IN_BYTES );
    
ErrorExit:

    return dr;
}

/********************************************************************************************
** Function:    DRM_BCert_GetPublicKey
**
** Synopsis:    The function retrieves the public key of the specified cert
**              in a certificate chain. If there are several public keys in the cert
**              the first one will be returned.
**
** Arguments:   [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**              [f_dwCertIndex]     : index of certificate in the chain. Leaf cert is 0.
**              [f_pPubkey]         : pointer to a public key structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
********************************************************************************************/
DRM_RESULT DRM_BCert_GetPublicKey(
    const CRYPTO_BYTE    *f_pbCertData,
    const CRYPTO_DWORD    f_cbCertData,
    const CRYPTO_DWORD    f_dwCertIndex,
    PUBKEY_P256       *f_pPubkey )
{
    DRM_RESULT              dr             = DRM_SUCCESS;

    
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pPubkey != NULL );

    ChkDR( DRM_BCert_GetPublicKeyByUsage( f_pbCertData,
                                          f_cbCertData,
                                          f_dwCertIndex,
                                          DRM_BCERT_KEYUSAGE_UNKNOWN,
                                          f_pPubkey,
                                          NULL,
                                          NULL ) );
    
ErrorExit:
    return dr;
}

