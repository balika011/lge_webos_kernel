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
  @file der_decode_generalized_time.c
  ASN.1 DER, decode a  GENERALIZED TIME, Tom St Denis
*/

#ifdef LTC_DER

static int char_to_int(unsigned char x)
{
   switch (x)  {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
   }
   return 100;
}

static int string_to_int(char* buf, int size, int max)
{
    int count = 0;
    int i = 0;

    for (i=0;i<size;i++)
    {
        count *= 10;
        count += char_to_int(buf[i]);
    }
    if (count > max)
        return -1;

    return count;

}
#define DECODE_V(y, size, max) \
    y  = string_to_int(buf + x, size, max); \
    if (y == -1) return CRYPT_INVALID_PACKET; \
    x += size;

/**
  Decodes a GENERALIZED time structure in DER format: ASN.1 GeneralizedTime format is YYYYMMDDHHSSZ
  @param in     Input buffer
  @param inlen  Length of input buffer in octets
  @param out    [out] Destination of UTC time structure
  @return CRYPT_OK   if successful
*/
int der_decode_generalized_time(const unsigned char *in, unsigned long *inlen,
                                ltc_time   *out)
{
   unsigned char buf[32];
   unsigned long x;
   int           y;

   LTC_ARGCHK(in    != NULL);
   LTC_ARGCHK(inlen != NULL);
   LTC_ARGCHK(out   != NULL);

   /* check header */
   if (*inlen < 2UL || (in[1] >= sizeof(buf)) || ((in[1] + 2UL) > *inlen)) {
      return CRYPT_INVALID_PACKET;
   }

   /* decode the string */
   for (x = 0; x < in[1]; x++) {
       y = der_ia5_value_decode(in[x+2]);
       if (y == -1) {
          return CRYPT_INVALID_PACKET;
       }
       buf[x] = y;
   }
   *inlen = 2 + x;


   /* possible encodings are 
    YYYYMMDDHHMMSSZ
   */

    out->type = LTC_TIME_TYPE_GENERALIZED;

    x = 0;
    DECODE_V(out->YY, 4, 10000);
    DECODE_V(out->MM, 2, 13);
    DECODE_V(out->DD, 2, 32);
    DECODE_V(out->hh, 2 ,24);
    DECODE_V(out->mm, 2, 60);

    /* clear timezone and seconds info */
    out->off_dir = out->off_hh = out->off_mm = out->ss = 0;

    /* now is it Z, +, - or 0-9 */
    if (buf[x] == 'Z') {
       return CRYPT_OK;
    } else if (buf[x] == '+' || buf[x] == '-') {
       out->off_dir = (buf[x++] == '+') ? 0 : 1;
       DECODE_V(out->off_hh, 2, 24);
       DECODE_V(out->off_mm, 2, 60);
       return CRYPT_OK;
    }

    /* decode seconds */
    DECODE_V(out->ss, 2, 60);

    /* now is it Z, +, - */
    if (buf[x] == 'Z') {
       return CRYPT_OK;
    } else if (buf[x] == '+' || buf[x] == '-') {
       out->off_dir = (buf[x++] == '+') ? 0 : 1;
       DECODE_V(out->off_hh, 2,24);
       DECODE_V(out->off_mm, 2, 60);
       return CRYPT_OK;
    } else {
       return CRYPT_INVALID_PACKET;
    }
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/utctime/der_decode_utctime.c,v $ */
/* $Revision: #1 $ */
/* $Date: 2015/02/15 $ */
