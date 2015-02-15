/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.com
 */
#include "tomcrypt.h"

/**
  @file der_length_generalized_time.c
  ASN.1 DER, get length of GENERALIZED TIME, Tom St Denis
*/

#ifdef LTC_DER

/**
  Gets length of DER encoding of GENERALIZED TIME
  @param generalized_time      The GENERALIZED time structure to get the size of
  @param outlen [out] The length of the DER encoding
  @return CRYPT_OK if successful
*/
int der_length_generalized_time(ltc_time *generalized_time, unsigned long *outlen)
{
   LTC_ARGCHK(outlen  != NULL);
   LTC_ARGCHK(generalized_time != NULL);

   if (generalized_time->off_hh == 0 && generalized_time->off_mm == 0) {
      /* we encode as YYYYMMDDhhmmssZ */
      *outlen = 2 + 15;
   } else {
      return CRYPT_INVALID_PACKET;
   }

   return CRYPT_OK;
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/utctime/der_length_generalized_time.c,v $ */
/* $Revision: #1 $ */
/* $Date: 2015/02/15 $ */
