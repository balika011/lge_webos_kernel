/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.org
 */
#include "tomcrypt.h"

/**
  @file der_decode_sequence_flexi.c
  ASN.1 DER, decode an array of ASN.1 types with a flexi parser, Tom St Denis
*/

#ifdef LTC_DER

/*#define DEBUG_PARSING*/
static
unsigned long fetch_length(const unsigned char *in, unsigned long inlen)
{
   unsigned long x, y, z;

   y = 0;

   /* skip type and read len */
   if (inlen < 2) {
      return 0xFFFFFFFF;
   }
   ++in; ++y;
   
   /* read len */
   x = *in++; ++y;
   
   /* <128 means literal */
   if (x < 128) {
      return x+y;
   }
   x &= 0x7F; /* the lower 7 bits are the length of the length */
   inlen -= 2;

   /* INDEFINITE length check */
   if (x == 0) {
      return inlen; /* TODO: some better approach? */
   }
   
   /* len means len of len! */
   if (x > 4 || x > inlen) {
      /* error */
      return 0xFFFFFFFF;
   }
   
   y += x;
   z = 0;
   while (x--) {   
      z = (z<<8) | ((unsigned long)*in);
      ++in;
   }
   return z+y;
}

/** 
   ASN.1 DER Flexi(ble) decoder will decode arbitrary DER packets and create a linked list of the decoded elements.
   @param in      The input buffer
   @param inlen   [in/out] The length of the input buffer and on output the amount of decoded data 
   @param out     [out] A pointer to the linked list
   @return CRYPT_OK on success.
*/   
int der_decode_sequence_flexi(const unsigned char *in, unsigned long *inlen, ltc_asn1_list **out)
{
   ltc_asn1_list *l;
   unsigned long err, type, len, totlen, x, y;
   void          *realloc_tmp;
   
   LTC_ARGCHK(in    != NULL);
   LTC_ARGCHK(inlen != NULL);
   LTC_ARGCHK(out   != NULL);

   l = NULL;
   totlen = 0;
   
   /* scan the input and and get lengths and what not */
   while (*inlen) {     
      /* read the type byte */
      type = *in;

      /* fetch length */
      len = fetch_length(in, *inlen);
#ifdef DEBUG_PARSING
      printf("flexi packet: %d - %d (inlen %d)\n", type, len, *inlen);
#endif
      if (len > *inlen) {
         err = CRYPT_INVALID_PACKET;
         goto error;
      }

      /* alloc new link */
      if (l == NULL) {
         l = XCALLOC(1, sizeof(*l));
         if (l == NULL) {
            err = CRYPT_MEM;
            goto error;
         }
      } else {
         l->next = XCALLOC(1, sizeof(*l));
         if (l->next == NULL) {
            err = CRYPT_MEM;
            goto error;
         }
         l->next->prev = l;
         l = l->next;
      }

      /* now switch on type */
      switch (type) {
         case 0x01: /* BOOLEAN */
             /* init field */
             l->type = LTC_ASN1_BOOLEAN;
             l->size = 1;

             if ((l->data = XCALLOC(1, sizeof(int))) == NULL) {
                err = CRYPT_MEM;
                goto error;
             }
             
             /* decode field */
             if ((err = der_decode_boolean(in, len, l->data)) != CRYPT_OK) {
                 goto error;
             }
             
             /* calc length of object */
             if ((err = der_length_boolean(&len)) != CRYPT_OK) {
                 goto error;
             }
             break;

         case 0x02: /* INTEGER */
             /* init field */
             l->type = LTC_ASN1_INTEGER;
             l->size = 1;
             if ((err = mp_init(&l->data)) != CRYPT_OK) {
                 goto error;
             }
             
             /* decode field */
             if ((err = der_decode_integer(in, *inlen, l->data)) != CRYPT_OK) {
                 goto error;
             }
             
             /* calc length of object */
             if ((err = der_length_integer(l->data, &len)) != CRYPT_OK) {
                 goto error;
             }
             break;

         case 0x03: /* BIT */
            /* init field */
            l->type = LTC_ASN1_BIT_STRING;
            l->size = len * 8; /* *8 because we store decoded bits one per char and they are encoded 8 per char.  */

            if ((l->data = XCALLOC(1, l->size)) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_bit_string(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_bit_string(l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            break;

         case 0x04: /* OCTET */

            /* init field */
            l->type = LTC_ASN1_OCTET_STRING;
            l->size = len;

            if ((l->data = XCALLOC(1, l->size)) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_octet_string(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_octet_string(l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            break;

         case 0x05: /* NULL */
         
            /* valid NULL is 0x05 0x00 */
            if (in[0] != 0x05 || in[1] != 0x00) {
               err = CRYPT_INVALID_PACKET;
               goto error;
            }
            
            /* simple to store ;-) */
            l->type = LTC_ASN1_NULL;
            l->data = NULL;
            l->size = 0;
            len     = 2;
            
            break;
         
         case 0x06: /* OID */
         
            /* init field */
            l->type = LTC_ASN1_OBJECT_IDENTIFIER;
            l->size = len;

            if ((l->data = XCALLOC(len, sizeof(unsigned long))) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_object_identifier(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_object_identifier(l->data, l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            
            /* resize it to save a bunch of mem */
            if ((realloc_tmp = XREALLOC(l->data, l->size * sizeof(unsigned long))) == NULL) {
               /* out of heap but this is not an error */
               break;
            }
            l->data = realloc_tmp;
            break;
         
         case 0x0C: /* UTF8 */
         
            /* init field */
            l->type = LTC_ASN1_UTF8_STRING;
            l->size = len * sizeof(unsigned long);

            if ((l->data = XCALLOC(1, l->size)) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_utf8_string(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
              
            if ((err = der_length_utf8_string(l->data, l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            break;

         case 0x13: /* PRINTABLE */
         
            /* init field */
            l->type = LTC_ASN1_PRINTABLE_STRING;
            l->size = len;

            if ((l->data = XCALLOC(1, l->size)) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_printable_string(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_printable_string(l->data, l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            break;
         
         case 0x14: /* T.61 (TELETEX) */
         
            /* init field */
            l->type = LTC_ASN1_T61_STRING;
            l->size = len;

            if ((l->data = XCALLOC(1, l->size)) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_t61_string(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_t61_string(l->data, l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            break;
         
         case 0x16: /* IA5 */
         
            /* init field */
            l->type = LTC_ASN1_IA5_STRING;
            l->size = len;

            if ((l->data = XCALLOC(1, l->size)) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            if ((err = der_decode_ia5_string(in, *inlen, l->data, &l->size)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_ia5_string(l->data, l->size, &len)) != CRYPT_OK) {
               goto error;
            }
            break;
         
         case 0x17: /* UTC TIME */
         
            /* init field */
            l->type = LTC_ASN1_UTCTIME;
            l->size = 1;

            if ((l->data = XCALLOC(1, sizeof(ltc_time))) == NULL) {
               err = CRYPT_MEM;
               goto error;
            }
            
            len = *inlen;
            if ((err = der_decode_utctime(in, &len, l->data)) != CRYPT_OK) {
               goto error;
            }
            
            if ((err = der_length_utctime(l->data, &len)) != CRYPT_OK) {
               goto error;
            }
            break;
         
         case 0x18: /* GENERALIZED TIME */

             /* init field */
             l->type = LTC_ASN1_GENERALIZEDTIME;
             l->size = 1;

             if ((l->data = XCALLOC(1, sizeof(ltc_time))) == NULL) {
                 err = CRYPT_MEM;
                 goto error;
             }

             len = *inlen;
             if ((err = der_decode_generalized_time(in, &len, l->data)) != CRYPT_OK) {
                 goto error;
             }

             if ((err = der_length_generalized_time(l->data, &len)) != CRYPT_OK) {
                 goto error;
             }
             
             break;

         case 0x30: /* SEQUENCE */
         case 0x31: /* SET */
         
             /* init field */
             l->type = (type == 0x30) ? LTC_ASN1_SEQUENCE : LTC_ASN1_SET;
             l->size = len; /* number of bytes in DER -- useful externally (AM) */
             l->data = (char*) in; /* bytes of DER (AM) */
             
             /* we have to decode the SEQUENCE header and get its length */
             
                /* move past type */
                ++in; --(*inlen);
                
                /* read length byte */
                x = *in++; --(*inlen);
                
                /* smallest SEQUENCE/SET header */
                y = 2;
                
                /* now if it's > 127 the next bytes are the length of the length */
                if (x > 128) {
                   x      &= 0x7F;
                   in     += x;
                   *inlen -= x;
                   
                   /* update sequence header len */
                   y      += x;
                }
             
             /* Sequence elements go as child */
                /* the original LTC code was the following: */
/*           len = *inlen; */
             /* changed to the following now: (AM) */
             len = len - y;
             /* originally, the length of the buffer passed to the child was total
                bytes remaining. this didn't make sense when the subsequence was
                not the final item in current sequence. now, the length passed
                is the length of the sequence element (minus header bytes) */

             /* JB: handle the empty sequence/set case */
             if (len > 0) {
                 /* AM: by saving and restoring seq length, we take care of issue
                 where sequence doesn't return same length */
                unsigned long orig_len = len;
                if ((err = der_decode_sequence_flexi(in, &len, &(l->child))) != CRYPT_OK) {
                   goto error;
                }
                len = orig_len;

                /* len update */
                totlen += y;
             
                /* link them up y0 */
                 if (l->child != NULL) {
                     l->child->parent = l;
                 }
             }
             break;
         default:
             /* see if we have a tagged object */
             if ((type & 0x80 /*TAGGED*/) != 0) {
                 /* with tagged object tag number is bottom 5 bits */
                 int tag_no = type & 0x1f;

                 /* move past type */
                 ++in; --(*inlen);

                 /* read length byte */
                 x = *in++; --(*inlen);

                 /* smallest SEQUENCE/SET header */
                 y = 2;

                /* now if it's > 127 the next bytes are the length of the length */
                if (x > 128) {
                   x      &= 0x7F;
                   in     += x;
                   *inlen -= x;
                   
                   /* update sequence header len */
                   y      += x;
                }

                 if (tag_no == 0x1f)
                 {
                     /* TODO */
                     /*
                     int idx = 0;
                     tagNo = 0;
                     while ((bytes[idx] & 0x80) != 0)
                     {
                         tagNo |= (bytes[idx++] & 0x7f);
                         tagNo <<= 7;
                     }
                     tagNo |= (bytes[idx] & 0x7f);
                     byte[]  tmp = bytes;
                     bytes = new byte[tmp.length - (idx + 1)];
                     System.arraycopy(tmp, idx + 1, bytes, 0, bytes.length);*/
                 }

                 /* TODO */
                 if (x == 0)        /* empty tag! */
                 {
                     /*
                     if ((tag & CONSTRUCTED) == 0)
                     {
                         return new DERTaggedObject(false, tagNo, new DERNull());
                     }
                     else
                     {
                         return new DERTaggedObject(false, tagNo, new DERSequence());
                     }
                     */
                 }

                 l->type = LTC_ASN1_TAG;
                 l->used = tag_no; /* store the tag number in used field */

                 /* move the input stream along by the header bytes */
                 len = len - y;
                 
                 if (type & 0x20 /*CONSTRUCTED*/) {
                     /* parse recursively */
                     if ((err = der_decode_sequence_flexi(in, &len, &(l->child))) != CRYPT_OK) {
                         goto error;
                     }
                 } else { /* simple type - implicit... the child is an octet string */
                     /* construct the child */
                     l->child = XCALLOC(1, sizeof(*l));
                     if (l == NULL) {
                         err = CRYPT_MEM;
                         goto error;
                     }
                     /* init field */
                     l->child->type = LTC_ASN1_OCTET_STRING;
                     l->child->size = len;

                     if ((l->child->data = XCALLOC(1, l->child->size)) == NULL) {
                         err = CRYPT_MEM;
                         goto error;
                     }

                     /* just copy the data */
                     XMEMCPY(l->child->data, in, l->child->size);
                 }
                 /* link them up y0 */
                 l->child->parent = l;

                 /* len update */
                 totlen += y;

                 break;
             }

              /* invalid byte ... this is a soft error */
              /* fprintf(stderr, "unknown type: %d\n", type); */
              /* remove link */
              if (l->prev == NULL) {
                  XFREE(l);
                  l = NULL;
              }
              else {
                  l = l->prev;
                  if (l != NULL && l->next != NULL) {
                      XFREE(l->next);
                      l->next = NULL;
                  }
              }
              goto outside;
      }
      
      /* advance pointers */
      totlen  += len;
      in      += len;
      *inlen  -= len;
   }
   
outside:   

    if (l != NULL) {
        /* rewind l please */
        while (l->prev != NULL || l->parent != NULL) {
            if (l->parent != NULL) {
                l = l->parent;
            } else {
                l = l->prev;
            }
        }
    }
   
   /* return */
   *out   = l;
   *inlen = totlen;
   return CRYPT_OK;

error:
   /* free list (can sometimes be null if there's been an error) */
   if (l != NULL) der_sequence_free(l);

   return err;
}

#endif


/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/sequence/der_decode_sequence_flexi.c,v $ */
/* $Revision: #1 $ */
/* $Date: 2015/02/15 $ */
