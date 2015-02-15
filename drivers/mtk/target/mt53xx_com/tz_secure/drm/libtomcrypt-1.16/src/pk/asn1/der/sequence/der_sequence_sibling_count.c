#include "tomcrypt.h"


#ifdef LTC_DER

unsigned long der_sequence_sibling_count(ltc_asn1_list *l)
{
   unsigned long sibling_count = 0;
   ltc_asn1_list* l2 = l;

   /* check */
   LTC_ARGCHK(l != NULL);

   /* older siblings */
   while (l->prev != NULL) {
      l = l->prev;
      sibling_count++;
   }

   /* younger siblings */
   while (l2->next != NULL) {
      l2 = l2->next;
      sibling_count++;
   }

   return sibling_count;
}

#endif /* LTC_DER */
