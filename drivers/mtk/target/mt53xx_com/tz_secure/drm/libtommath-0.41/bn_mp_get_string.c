#include <tommath.h>
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

/*----------------------------------------------------------------------
|    string_length
+---------------------------------------------------------------------*/
static int
string_length(char* src)
{
    int len = 0;
    while(src[len] != '\0') len++;
    return len;
}
/*----------------------------------------------------------------------
|    copy_string
+---------------------------------------------------------------------*/
static void
copy_string(char* dst, const char* src)
{
    while((*dst++ = *src++));
}

/*----------------------------------------------------------------------
|   integer_to_ascii
+---------------------------------------------------------------------*/
static int
integer_to_ascii(int value, char* buffer, int buffer_size)
{
    char s[32];
    char* c = &s[31];
    int negative = 0;
    *c-- = '\0';

    /* default value */
    if (buffer == NULL || buffer_size == 0) return -1;
    buffer[0] = '\0';

    /* handle the sign */
    negative = 0;
    if (value < 0) {
        negative = 1;
        value = -value;
    }

    /* process the digits */
    do {
        int digit = value%10;
        *c-- = '0'+digit;
        value /= 10;
    } while(value);

    if (negative) {
        *c = '-';
    } else {
        ++c;
    }

    /* check that the string fits */
    if ((int)(&s[31]-c)+1 > buffer_size) return -1;

    /* copy the string */
    copy_string(buffer, c);

    return 0;
}
/* Get MP string */
int mp_get_string(void* aa, char* out,int size)
{
    int i  = 0;
    int j = 0;
    int res  = 0;
    mp_int* a = (mp_int*)aa;

    out[0] = '\0';

    if (a->used == 0) {
        return 0;
    }

    for(i=a->used; i>0;i--)    {
        j = string_length(out);
        res = integer_to_ascii(DIGIT(a,i-1),out + j, size - j);
        if (res != 0)
            break;
    }
    return res;
}

/* $Source: /cvs/libtom/libtommath/bn_mp_get_int.c,v $ */
/* $Revision: #1 $ */
/* $Date: 2015/02/15 $ */
