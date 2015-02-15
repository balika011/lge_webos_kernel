/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
/*----------------------------------------------------------------------------*
 * $RCSfile: str_util.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: 
 *         This header file contains handle specific definitions, which are
 *         exported.
 *---------------------------------------------------------------------------*/

#include "x_typedef.h"
#include "x_os.h"


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
typedef struct
{
    CHAR  c_max_digit;     /* digit limit */
    CHAR  c_max_letter_lc; /* lower-case limit */
    CHAR  c_max_letter_uc; /* upper-case limit */
}   STR_BASE_LIMITS_T;


/*----------------------------------------------------------------------------
 * Function: strlen()
 *
 * Description:
 *      this API returns length of a character string. it performs
 *      1. parse argument and return 0 if it is a NULL string.
 *      2. move temp pointer to null character.
 *      3. return distance between temp pointer and original pointer.
 *
 * Inputs:
 *      ps_s : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      length of character string excluding null ('\0') character.
 *---------------------------------------------------------------------------*/
size_t strlen(const char *ps_s)
{
    char *pc_c;

    if (ps_s == (CHAR *)NULL)
    {
        return 0;
    }

    pc_c = (char *)ps_s;
    while (*ps_s != '\0')
    {
        ++ps_s;
    }

    return (ps_s - pc_c);
}

#if 0
/*----------------------------------------------------------------------------
 * Function: x_strdup()
 *
 * Description:
 *      this API returns a pointer to a memory block that duplicates ps_str.
 *      1. parse argument and return NULL if it is a NULL string.
 *      2. get string ps_str length plus 1.
 *      3. allocate memory block of such size.
 *      4. memory copy from ps_str to the allocated memory.
 *
 * Inputs:
 *      ps_s : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to copied string.
 *---------------------------------------------------------------------------*/
CHAR *x_strdup(const CHAR *ps_str)
{
    CHAR *ps_dup_str = NULL;

    if (ps_str != NULL)
    {
        size_t z_len;

        z_len = x_strlen(ps_str) + 1;
        ps_dup_str = (CHAR *) x_mem_alloc(z_len);
        if (ps_dup_str != NULL)
        {
            x_memcpy(ps_dup_str, ps_str, z_len);
        }
    }

    return(ps_dup_str);
}

/*----------------------------------------------------------------------------
 * Function: x_strchr()
 *
 * Description:
 *      this API locates the first occurrence of c_c in ps_s. it performs
 *      1. parse argument and return NULL if it is a NULL string.
 *      2. move along ps_s until c_c is found and return.
 *      3. if not found, return NULL.
 *
 * Inputs:
 *      ps_s : null ('\0') terminated character string.
 *      c_c : character to locate.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to the first occurrence of c_c in ps_s.
 *---------------------------------------------------------------------------*/
CHAR *x_strchr(const CHAR *ps_s, const CHAR c_c)
{
    if (ps_s == (CHAR *)NULL)
    {
        return ((CHAR *)NULL);
    }

    do
    {
        if (*ps_s == c_c)
        {
            return ((CHAR *)ps_s);
        }
    } while (*ps_s++ != '\0');

    return ((CHAR *)NULL);
}

/*----------------------------------------------------------------------------
 * Function: x_strrchr()
 *
 * Description:
 *      This API locates the last occurrence of a character in a string. It
 *      returns NULL if the character does not exist in the string.
 *
 * Inputs:
 *      ps_str  : null-terminated character string.
 *      c_cchar : character to locate.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to the last occurrence of c_char in ps_str, or NULL.
 *---------------------------------------------------------------------------*/
CHAR* x_strrchr(const CHAR *ps_str, CHAR c_char)
{
    CHAR *ps_res = NULL;
    
    if (ps_str != NULL)
    {
        do
        {
            if (*ps_str == c_char)
            {
                ps_res = (CHAR *)ps_str;
            }
        } while (*ps_str++ != '\0');
    }
    
    return ps_res;
}

/*----------------------------------------------------------------------------
 * Function: x_strstr()
 *
 * Description:
 *      this API locates the first occurrence of ps_find in ps_s. it performs
 *      1. parse arguments and return NULL if either is NULL string.
 *      2. if ps_find contains '\0', returns ps_s.
 *      3. move both ps_s and ps_find as long as they are identical, otherwise,
 *         strat over ps_find and continue to compare.
 *      4. if not found, return NULL.
 *
 * Inputs:
 *      ps_s : null ('\0') terminated character string.
 *      ps_find : null terminated character string to locate.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to the first occurrence of ps_find in ps_s.
 *---------------------------------------------------------------------------*/
CHAR *x_strstr(const CHAR *ps_s, const CHAR *ps_find)
{
    CHAR *ps_c, *ps_t;

    if ((ps_s == (CHAR *)NULL) || (ps_find == (CHAR *)NULL))
    {
        return ((CHAR *)NULL);
    }

    /* match Linux return */
    if (ps_find[0] == '\0')
    {
        return ((CHAR *) ps_s);
    }

    ps_c = (CHAR *)ps_find;    /* ps_c: walking pointer through "find" */
    ps_t = (CHAR *)NULL;       /* ps_t: match pointer */
    while ((*ps_c != '\0') && (*ps_s != '\0'))
    {
        if (*ps_s == *ps_c)
        {
            if (ps_t == (CHAR *)NULL)
            {
                ps_t = (CHAR *)ps_s;      /* match pointer */
            }
            ++ps_c;
            ++ps_s;
        }
        else
        {
            if (ps_t == (CHAR *)NULL)
            {
                ++ps_s;        /* advance to the next byte */
            }
            else
            { 
                /* do not advance "s", check against "c" (start over) */
                ps_c = (CHAR *)ps_find;       /* start it over */
                ps_t = (CHAR *)NULL;
            }
        }
    }

    if (*ps_c == '\0')     /* reach the end of "find" */
    {
        return ps_t;
    }

    return ((CHAR *)NULL);
}


/*----------------------------------------------------------------------------
 * Function: x_strcpy()
 *
 * Description:
 *      this API copies ps-from to ps_to. it performs
 *      1. parse argument and return NULL if either is NULL.
 *      2. copy character by character from ps_from to ps_to until
 *         '\0' is encountered.
 *
 * Inputs:
 *      ps_to : memory block to be copied to.
 *      ps_from : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to ps_to
 *---------------------------------------------------------------------------*/
CHAR *x_strcpy(CHAR *ps_to, const CHAR *ps_from)
{
    CHAR *ps_c;

    if ((ps_to == (CHAR *)NULL) || (ps_from == (CHAR *)NULL))
    {
        return ps_to;
    }

    ps_c = (CHAR *)ps_to;
    while ((*ps_to = *ps_from) != '\0')
    {
        ++ps_to;
        ++ps_from;
    }
    
    return ps_c;
}


/*----------------------------------------------------------------------------
 * Function: x_strncpy()
 *
 * Description:
 *      this API copies ps-from to ps_to up to z_l bytes. it performs
 *      1. parse argument and return ps_to on error.
 *      2. process to make the most UINT32 copies, then UINT16 copies, then
 *         UINT8 copies. 
 *
 * Inputs:
 *      ps_to : memory block to be copied to.
 *      ps_from : null ('\0') terminated character string.
 *      z_l : bytes to be copied
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to ps_to
 *---------------------------------------------------------------------------*/
CHAR *x_strncpy(CHAR *ps_to, const CHAR *ps_from, size_t z_l)
{
    UINT32 *pui4_t, *pui4_f;
    INT32 i4_i, i4_j, i4_k;
    CHAR *ps_c;

    if ((ps_to == (CHAR *)NULL) || (ps_from == (CHAR *)NULL) || (z_l == 0))
    {
        return ps_to;
    }

    /* if length of ps_from is less than z_l */
    /* pad with null characters */
    ps_c = (CHAR *)ps_from;
    while (*ps_c != '\0')
    {
        ++ps_c;
    }

    i4_j = i4_i = (INT32)(ps_c - ps_from);     /* length of ps_from */

    if ((size_t) i4_i < z_l)
    {
        do
        {
            *(CHAR *)(ps_to + i4_i) = '\0';
            ++i4_i;
        } while ((size_t)i4_i < z_l);

        z_l = (size_t)i4_j; 
        if (z_l == 0)
        {
            return(ps_to);
        }
    }
    
    i4_i = ((UINT32)ps_to) & 0x3;      /* off from INT32 boundary */
    i4_j = ((UINT32)ps_from) & 0x3;
    if (i4_i == i4_j)           /* check for INT32 alignment */
    {
        if (i4_i & 0x1)         /* off 1 or 3 bytes mean 3 or 1 leading bytes */
        {
            i4_i = 4 - i4_i;    /* leading bytes to next INT32 aligned */
        }
        i4_j = z_l - i4_i;      /* remaining bytes excl. leading bytes */
        i4_k = i4_j >> 2;       /* count of INT32 */
    }
    else
    {
        i4_k = 0;
    }

    ps_c = ps_to;          /* return pointer */
    if (i4_k > 0)          /* do INT32 assignment */
    {
        pui4_t = (UINT32 *)(ps_to + i4_i);
        pui4_f = (UINT32 *)(ps_from + i4_i);

        /* leading bytes */
        if (i4_i & 0x1)    /* 1 or 3 leading bytes  */
        {
            *ps_to++ = *ps_from++;
        }
        if (i4_i & 0x2)    /* 2 or 3 leading bytes */
        {
            *(UINT16 *)ps_to = *(UINT16 *)ps_from;
        }

        do      /* do INT32 assignment */
        {
            *pui4_t++ = *pui4_f++;
        } while (--i4_k > 0);

        /* finish the remaining bytes */
        if (i4_j & 0x2)      /* do an INT16 assignment if possible */
        {
            *((UINT16 *)pui4_t) = *((UINT16 *)pui4_f);
            pui4_t = (UINT32 *)((UINT32)pui4_t + (UINT32)2);
            pui4_f = (UINT32 *)((UINT32)pui4_f + (UINT32)2);
        }
        if (i4_j & 0x1)      /* do byte assignment */
        {
            *((CHAR *)pui4_t) = *((CHAR *)pui4_f);
        }
        return(ps_c);
    }
    else
    {
        do
        {
            *ps_to++ = *ps_from++;
        } while (--z_l > 0);
        return(ps_c);
    }
}


/*----------------------------------------------------------------------------
 * Function: x_strcat()
 *
 * Description:
 *      this API concatenates ps-from to ps_to. it performs
 *      1. parse argument and return ps_to if either is NULL.
 *      2. move temp pointer to the end of ps_to.
 *      3. copy character by character from ps_from to temp pointer until
 *         '\0' is encountered.
 *
 * Inputs:
 *      ps_to : null ('\0') terminated character string.
 *      ps_append : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to ps_to
 *---------------------------------------------------------------------------*/
CHAR *x_strcat(CHAR *ps_to, const CHAR *ps_append)
{
    CHAR *ps_c;

    if ((ps_to == (CHAR *)NULL) || (ps_append == (CHAR *)NULL))
    {
        return ps_to;
    }

    /* get the end of "to" */
    ps_c = ps_to;
    while (*ps_to != '\0')
    {
        ++ps_to;
    }

    while ((*ps_to = *ps_append) != '\0')
    {
        ++ps_to;
        ++ps_append;
    }
    
    return(ps_c);
}


/*----------------------------------------------------------------------------
 * Function: x_strncat()
 *
 * Description:
 *      this API concatenates ps-from to ps_to up to z_l bytes. it performs
 *      1. parse argument and return ps_to if either is NULL.
 *      2. move temp pointer to the end of ps_to.
 *      3. call x_strncpy() to copy z_l bytes.
 *
 * Inputs:
 *      ps_to : null ('\0') terminated character string.
 *      ps_append : null ('\0') terminated character string.
 *      z_l : bytes to be concaatenated from ps_append.
 *
 * Outputs: -
 *
 * Returns:
 *      pointer to ps_to
 *---------------------------------------------------------------------------*/
CHAR *x_strncat(CHAR *ps_to, const CHAR *ps_append, size_t z_l)
{
    CHAR *ps_c, *ps_a, c_c;

    if ((ps_to == (CHAR *)NULL) || (ps_append == (CHAR *)NULL) || (z_l == 0))
    {
        return ps_to;
    }

    /* get the end of "to" */
    ps_c = ps_to;
    while (*ps_c != '\0')
    {
        ++ps_c;
    }

    ps_a = (CHAR *)ps_append;
    while (((c_c = *ps_a++) != '\0') && (z_l != 0))
    {
        *(CHAR *)ps_c++ = c_c;
        --z_l;
    }
    
    *(CHAR *) ps_c = '\0';
    return(ps_to);
}

#endif
/*----------------------------------------------------------------------------
 * Function: str_util_strcmp()
 *
 * Description:
 *      This API compares 2 strings ps_s1 and ps_s2 up to z_l bytes. It does
 *      the following:
 *      1. if both are NULL, return 0.
 *      2. if ps_s1 is NULL and ps_s2 is not NULL, return -1.
 *      3. if ps_s1 is not NULL and ps_s2 is NULL, return 1.
 *      4. compute the difference between ps_s1 and ps_s2 side by side.
 *      5. if difference is less than zero, return -1; if difference is
 *         positive, return 1; if diffenece is 0, move both pointers, repeat
 *         steps 4, 5.
 *      6. stop when either one moves to end of string or z_l bytes have met.
 *
 * Inputs:
 *      ps_s1 : null ('\0') terminated character string.
 *      ps_s2 : null ('\0') terminated character string.
 *      z_l : byte count to compare (used only if b_strn is TRUE).
 *      b_strn : specifies whether z_l is to be used.
 *      b_case_sensitive : specifies whether the comparison is case sensitive.
 *
 * Outputs: -
 *
 * Returns:
 *      0 : ps_s1 is identical to ps_s2.
 *      1 : ps_s1 is greater than ps_s2.
 *      -1 : ps_s1 is less than ps_s2.
 *---------------------------------------------------------------------------*/
static INT32 str_util_strcmp(const CHAR*  ps_s1,
                              const CHAR*  ps_s2,
                              size_t       z_l,
                              BOOL         b_strn,
                              BOOL         b_case_sensitive)
{
    if (b_strn && (z_l == 0))
    {
        return 0;
    }
    
    if (ps_s1 == (CHAR *)NULL)
    {
        if (ps_s2 != (CHAR *)NULL)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else if (ps_s2 == (CHAR *)NULL)
    {
        return 1;
    }
    
    do
    {
        CHAR  c_c1;
        CHAR  c_c2;
        
        c_c1 = *ps_s1;
        c_c2 = *ps_s2;
        
        if (! b_case_sensitive)
        {
            /* conversion to lower case */
            
            if ((c_c1 >= 'A') && (c_c1 <= 'Z'))
            {
                c_c1 = c_c1 - 'A' + 'a';
            }
            
            if ((c_c2 >= 'A') && (c_c2 <= 'Z'))
            {
                c_c2 = c_c2 - 'A' + 'a';
            }
        }
        
        if (c_c1 > c_c2)
        {
            return 1;
        }
        else if (c_c1 < c_c2)
        {
            return -1;
        }
    }
    while ((!b_strn || (--z_l > 0))
           &&
           (*ps_s1++ != '\0')
           &&
           (*ps_s2++ != '\0'));
    
    return 0;
}

/*----------------------------------------------------------------------------
 * Function: x_strcmp()
 *
 * Description:
 *      This API compares 2 strings ps_s1 and ps_s2.
 *
 * Inputs:
 *      ps_s1 : null ('\0') terminated character string.
 *      ps_s2 : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      0 : ps_s1 is identical to ps_s2.
 *      1 : ps_s1 is greater than ps_s2.
 *      -1 : ps_s1 is less than ps_s2.
 *---------------------------------------------------------------------------*/
 #if 0
INT32 x_strcmp(const CHAR *ps_s1, const CHAR *ps_s2)
{
    return str_util_strcmp(ps_s1, ps_s2, 0, FALSE, TRUE);
}
#endif
/*----------------------------------------------------------------------------
 * Function: x_strncmp()
 *
 * Description:
 *      This API compares 2 strings ps_s1 and ps_s2 up to z_l bytes.
 *
 * Inputs:
 *      ps_s1 : null ('\0') terminated character string.
 *      ps_s2 : null ('\0') terminated character string.
 *      z_l : byte count to compare.
 *
 * Outputs: -
 *
 * Returns:
 *      0 : ps_s1 is identical to ps_s2.
 *      1 : ps_s1 is greater than ps_s2.
 *      -1 : ps_s1 is less than ps_s2.
 *---------------------------------------------------------------------------*/
INT32 x_strncmp(const CHAR *ps_s1, const CHAR *ps_s2, size_t z_l)
{
    return str_util_strcmp(ps_s1, ps_s2, z_l, TRUE, TRUE);
}

#if 0
/*----------------------------------------------------------------------------
 * Function: x_strcasecmp()
 *
 * Description:
 *      Similar to x_strcmp, but ignoring the case of the characters.
 *
 * Inputs:
 *      ps_s1 : null ('\0') terminated character string.
 *      ps_s2 : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      0 : ps_s1 is identical to ps_s2.
 *      1 : ps_s1 is greater than ps_s2.
 *      -1 : ps_s1 is less than ps_s2.
 *---------------------------------------------------------------------------*/
INT32 x_strcasecmp(const CHAR*  ps_s1,
                      const CHAR*  ps_s2)
{
    return str_util_strcmp(ps_s1, ps_s2, 0, FALSE, FALSE);
}

/*----------------------------------------------------------------------------
 * Function: x_strncasecmp()
 *
 * Description:
 *      Similar to x_strncmp, but ignoring the case of the characters.
 *
 * Inputs:
 *      ps_s1 : null ('\0') terminated character string.
 *      ps_s2 : null ('\0') terminated character string.
 *
 * Outputs: -
 *
 * Returns:
 *      0 : ps_s1 is identical to ps_s2.
 *      1 : ps_s1 is greater than ps_s2.
 *      -1 : ps_s1 is less than ps_s2.
 *---------------------------------------------------------------------------*/
INT32 x_strncasecmp(const CHAR*  ps_s1,
                       const CHAR*  ps_s2,
                       size_t       z_l)
{
    return str_util_strcmp(ps_s1, ps_s2, z_l, TRUE, FALSE);
}

/*----------------------------------------------------------------------------
 * Function: is_base_valid
 *
 * Description:
 *      This API checks whether a numeric base is valid. A valid base is
 *      between 2 and 36, or is the special value 0.
 *
 * Inputs:
 *      ui1_base : the numeric base.
 *
 * Outputs: -
 *
 * Returns:
 *      TRUE or FALSE.
 *---------------------------------------------------------------------------*/
static BOOL is_base_valid(UINT8 ui1_base)
{
    return ((ui1_base == 0)
            ||
            ((ui1_base >= 2) && (ui1_base <= 36)));
}

/*----------------------------------------------------------------------------
 * Function: is_space
 *
 * Description:
 *      This API checks whether a character is a space character, as defined
 *      by C and POSIX.
 *
 * Inputs:
 *      c_char : the character.
 *
 * Outputs: -
 *
 * Returns:
 *      TRUE or FALSE.
 *---------------------------------------------------------------------------*/
static BOOL is_space(CHAR c_char)
{
    return ((c_char == ' ')        /* space */
            || (c_char == '\f')    /* form feed */
            || (c_char == '\n')    /* new line */
            || (c_char == '\r')    /* carriage return */
            || (c_char == '\t')    /* horizontal tab */
            || (c_char == '\v'));  /* vertical tab */
}

/*----------------------------------------------------------------------------
 * Function: skip_spaces
 *
 * Description:
 *      This API skips the space characters in a string and return a pointer to
 *      the first non-space character in the string.
 *
 * Inputs:
 *      ps_str : the string.
 *
 * Outputs: -
 *
 * Returns:
 *      A pointer to the first non-space character in the string.
 *---------------------------------------------------------------------------*/
static const CHAR* skip_spaces(const CHAR* ps_str)
{
    CHAR c_char;
    
    while (((c_char = *ps_str) != '\0')
           &&
           is_space(c_char))
    {
        ps_str++;
    }
    
    return ps_str;
}

/*----------------------------------------------------------------------------
 * Function: parse_base
 *
 * Description:
 *      This API computes the base (if needed) as well as the upper limits of
 *      the characters when parsing a string given that base. For instance, the
 *      base-16 limits are '9' for the digits and 'f' and 'F' for the letters.
 *
 * Inputs:
 *      ps_str    : the string.
 *      pui1_base : contains the base.
 *
 * Outputs:
 *      pt_limits : the character limits.
 *      pui1_base : contains the base. This is modified only if the input value
 *                  was 0.
 *
 * Returns:
 *      A pointer to the string, the base indicators being skipped ("0x" for
 *      instance).
 *---------------------------------------------------------------------------*/
static const CHAR *parse_base(const CHAR*        ps_str,
                                STR_BASE_LIMITS_T*  pt_limits,
                                CHAR*               pui1_base)
{
    CHAR c_char;
    
    if (*pui1_base == 0)
    {
        c_char = *ps_str;
        
        if (c_char == '0')
        {
            ps_str++;
            
            c_char = *ps_str;

            if (c_char == 'x' || c_char == 'X')
            {
                *pui1_base = 16;
                
                pt_limits->c_max_digit = '9';
                pt_limits->c_max_letter_lc = 'f';
                pt_limits->c_max_letter_uc = 'F';

                c_char = *(ps_str + 1);

                if ((c_char >= '0' && c_char <= '9')
                    ||
                    (c_char >= 'a' && c_char <= 'f')
                    ||
                    (c_char >= 'A' && c_char <= 'F'))
                {
                    ps_str++;
                }
            }
            else
            {
                *pui1_base = 8;
                
                pt_limits->c_max_digit = '7';
                pt_limits->c_max_letter_lc = pt_limits->c_max_letter_uc = 0;
            }
        }
        else
        {
            *pui1_base = 10;
            
            pt_limits->c_max_digit = '9';
            pt_limits->c_max_letter_lc = pt_limits->c_max_letter_uc = 0;
        }
    }
    else if (*pui1_base == 16)
    {
        pt_limits->c_max_digit = '9';
        
        pt_limits->c_max_letter_lc = 'f';
        pt_limits->c_max_letter_uc = 'F';
        
        /* skip "0x" if present */
        
        c_char = *ps_str;
        
        if (c_char == '0')
        {
            ps_str++;
            
            c_char = *ps_str;
            
            if (c_char == 'x' || c_char == 'X')
            {
                c_char = *(ps_str + 1);
                
                if ((c_char >= '0' && c_char <= '9')
                    ||
                    (c_char >= 'a' && c_char <= 'f')
                    ||
                    (c_char >= 'A' && c_char <= 'F'))
                {
                    ps_str++;
                }
            }
        }
    }
    else if (*pui1_base <= 10)
    {
        pt_limits->c_max_digit = '0' + (*pui1_base - 1);
        
        pt_limits->c_max_letter_lc = pt_limits->c_max_letter_uc = 0;
    }
    else
    {
        pt_limits->c_max_digit = '9';
        
        pt_limits->c_max_letter_lc = 'a' + (*pui1_base - 11);
        pt_limits->c_max_letter_uc = 'A' + (*pui1_base - 11);
    }

    return ps_str;
}
                               
/*----------------------------------------------------------------------------
 * Function: parse_int_ll
 *
 * Description:
 *      This API parses the string and returns the 64-bit signed integer value
 *      the string was representing.
 *
 * Inputs:
 *      ps_str   : the string.
 *      ui1_base : contains the base.
 *
 * Outputs:
 *      pi8_res : contains the 64-bit integer value represented by the string.
 *
 * Returns:
 *      A pointer to the first invalid characters in the string.
 *---------------------------------------------------------------------------*/
static const CHAR *parse_int_ll(const CHAR*  ps_str,
                                INT64*       pi8_res,
                                CHAR         ui1_base)
{
    INT64              i8_res = 0;
    INT64              i8_base;
    INT64              i8_sign;
    STR_BASE_LIMITS_T  t_limits;
    CHAR               c_char;
    
    if (*ps_str == '+')
    {
        i8_sign = 1;
        ps_str++;
    }
    else if (*ps_str == '-')
    {
        i8_sign = -1;
        ps_str++;
    }
    else
    {
        i8_sign = 1;
    }
    
    ps_str = parse_base(ps_str, & t_limits, & ui1_base);
    
    i8_base = (UINT64)ui1_base;
    
    while (TRUE)
    {
        c_char = *ps_str;

        if (c_char >= '0' && c_char <= t_limits.c_max_digit)
        {
            i8_res = (i8_res * i8_base) + (c_char - '0');
        }
        else if (c_char >= 'a' && c_char <= t_limits.c_max_letter_lc)
        {
            i8_res = (i8_res * i8_base) + (c_char - 'a' + 10);
        }
        else if (c_char >= 'A' && c_char <= t_limits.c_max_letter_uc)
        {
            i8_res = (i8_res * i8_base) + (c_char - 'A' + 10);
        }
        else
        {
            break;
        }
        
        ps_str++;
    }
    
    *pi8_res = (i8_res * i8_sign);
    
    return ps_str;
}

/*----------------------------------------------------------------------------
 * Function: x_strtoll()
 *
 * Description:
 *     This API converts the initial part of the string in 'pc_beg_ptr' to a
 *     signed 64-bit integer value according to the given base. This API is
 *     the equivalent of the ANSI C strtoll API.
 *
 * Inputs:
 *      pc_beg_ptr : Null-terminated character string.
 *      ui1_base :   The base of numeric conversion, which must be between 2
 *                   and 36 inclusive, or be the special value 0.
 *
 * Outputs:
 *      ppc_end_ptr : If ppc_end_ptr is not NULL, *ppc_end_ptr contains the
 *                    address of the first invalid character.
 *
 * Returns:
 *      Signed INT64 value
 *---------------------------------------------------------------------------*/
INT64 x_strtoll(const CHAR*  pc_beg_ptr,
                CHAR**       ppc_end_ptr,
                UINT8        ui1_base)
{
    INT64 i8_res = 0;
    
    if (is_base_valid(ui1_base))
    {
        pc_beg_ptr = skip_spaces(pc_beg_ptr);
        
        pc_beg_ptr = parse_int_ll(pc_beg_ptr, & i8_res, ui1_base);
        
        if (ppc_end_ptr != NULL)
        {
            *ppc_end_ptr = (CHAR *)pc_beg_ptr;
        }
    }
    
    return i8_res;
}

/*----------------------------------------------------------------------------
 * Function: x_strtoull()
 *
 * Description:
 *     This API converts the initial part of the string in 'pc_beg_ptr' to an
 *     unsigned 64-bit integer value according to the given base. This API is
 *     the equivalent of the ANSI C strtoull API.
 *
 * Inputs:
 *      pc_beg_ptr : Null-terminated character string.
 *      ui1_base :   The base of numeric conversion, which must be between 2
 *                   and 36 inclusive, or be the special value 0.
 *
 * Outputs:
 *      ppc_end_ptr : If ppc_end_ptr is not NULL, *ppc_end_ptr contains the
 *                    address of the first invalid character.
 *
 * Returns:
 *      Unsigned INT64 value
 *---------------------------------------------------------------------------*/
UINT64 x_strtoull(const CHAR*  pc_beg_ptr,
                  CHAR**       ppc_end_ptr,
                  UINT8        ui1_base)
{
    UINT64  ui8_res;
    
    ui8_res = (UINT64)x_strtoll(pc_beg_ptr, ppc_end_ptr, ui1_base);
    
    return ui8_res;
}

/*----------------------------------------------------------------------------
 * Function: x_strspn
 *
 * Description:
 *     This API calculates the length of the initial segment of a string which
 *     consists entirely of characters in another string.
 *
 * Inputs:
 *      ps_str     Null-terminated character string.
 *      ps_accept  Character string containing the accepted character set.
 *
 * Outputs:  -
 *
 * Returns:  Length of the segment. If either ps_str or ps_accept is NULL, the
 *           API returns 0.
 *---------------------------------------------------------------------------*/
size_t x_strspn (const CHAR *ps_str, const CHAR *ps_accept)
{
    size_t z_len = 0;
    
    if ((ps_str != NULL) && (ps_accept != NULL))
    {
        UINT32  aui4_flags[8];
        UINT32  ui4_c;
        
        x_memset(aui4_flags, 0, sizeof(aui4_flags));
        
        while ((ui4_c = (UINT32) *(const UCHAR*)ps_accept++) != 0)
        {
            aui4_flags[ui4_c / 32] |= ((UINT32) (1 << (ui4_c % 32)));
        }
        
        ui4_c = (UINT32) *(const UCHAR*)ps_str;
        
        while (aui4_flags[ui4_c / 32] & (UINT32) (1 << (ui4_c % 32)))
        {
            z_len++;
            
            ps_str++;
            ui4_c = (UINT32) *(const UCHAR*)ps_str;
        }
    }
    
    return z_len;
}

/*----------------------------------------------------------------------------
 * Function: x_strcspn
 *
 * Description:
 *     This API calculates the length of the initial segment of a string which
 *     consists entirely of characters not in another string.
 *
 * Inputs:
 *      ps_str     Null-terminated character string.
 *      ps_reject  Character string containing the rejected character set.
 *
 * Outputs:  -
 *
 * Returns:  Length of the segment. If either ps_str or ps_accept is NULL, the
 *           API returns 0.
 *---------------------------------------------------------------------------*/
size_t x_strcspn (const CHAR *ps_str, const CHAR *ps_reject)
{
    size_t z_len = 0;
    
    if ((ps_str != NULL) && (ps_reject != NULL))
    {
        UINT32  aui4_flags[8];
        UINT32  ui4_c;
        
        x_memset(aui4_flags, 0xFF, sizeof(aui4_flags));
        
        /* character '\0' is rejected */
        aui4_flags[0] ^= 1;
        
        while ((ui4_c = (UINT32) *(const UCHAR*)ps_reject++) != 0)
        {
            aui4_flags[ui4_c / 32] &= ~((UINT32) (1 << (ui4_c % 32)));
        }
        
        ui4_c = (UINT32) *(const UCHAR*)ps_str;
        
        while (aui4_flags[ui4_c / 32] & (1 << (ui4_c % 32)))
        {
            z_len++;
            
            ps_str++;
            ui4_c = (UINT32) *(const UCHAR*)ps_str;
        }
    }
    
    return z_len;
}

/*----------------------------------------------------------------------------
 * Function: x_str_toupper
 *
 * Description:
 *     This API converts a string to uppercase.
 *
 * Inputs:
 *      ps_str     Null-terminated character string.
 *
 * Outputs:  -
 *
 * Returns:  the pointer passed in ps_str.
 *---------------------------------------------------------------------------*/
CHAR *x_str_toupper(CHAR *ps_str)
{
    if (ps_str != NULL)
    {
        CHAR *ps_cursor;
        CHAR  c_char;
        
        ps_cursor = ps_str;
        
        while ((c_char = *ps_cursor) != '\0')
        {
            if ((c_char >= 'a') && (c_char <= 'z'))
            {
                *ps_cursor = c_char - 'a' + 'A';
            }
            
            ps_cursor++;
        }
    }
    
    return ps_str;
}

/*----------------------------------------------------------------------------
 * Function: x_str_tolower
 *
 * Description:
 *     This API converts a string to lowercase.
 *
 * Inputs:
 *      ps_str     Null-terminated character string.
 *
 * Outputs:  -
 *
 * Returns:  the pointer passed in ps_str.
 *---------------------------------------------------------------------------*/
CHAR *x_str_tolower(CHAR *ps_str)
{
    if (ps_str != NULL)
    {
        CHAR *ps_cursor;
        CHAR  c_char;
        
        ps_cursor = ps_str;
        
        while ((c_char = *ps_cursor) != '\0')
        {
            if ((c_char >= 'A') && (c_char <= 'Z'))
            {
                *ps_cursor = c_char - 'A' + 'a';
            }
            
            ps_cursor++;
        }
    }
    
    return ps_str;
}

/*----------------------------------------------------------------------------
 * Function: x_strtok
 *
 * Description:
 *      This API returns a pointer to the next 'token' in ps_str. Tokens are
 *      separated by delimiters (specified by ps_delimiter). This API returns
 *      NULL if no token is found (including if the string is empty).
 *
 * Inputs:
 *      ps_str        Null-terminated character string.
 *      ps_delimiter  Null-terminated list of delimiters.
 *
 * Outputs:  pps_str       Will point to the next string to parse.
 *           pz_token_len  Will contain the length of the found token. If
 *                         pz_token_len is NULL then the passed string is
 *                         modified so it will contain a NULL character at the
 *                         end of the token.
 *
 * Returns:  pointer to the first token in ps_str.
 *---------------------------------------------------------------------------*/
CHAR *x_strtok(CHAR*        ps_str,
               const CHAR*  ps_delimiters,
               CHAR**       pps_str,
               size_t*      pz_token_len)
{
    CHAR *ps_token;
    
    ps_token = NULL;
    
    if ((ps_str != NULL) && (ps_delimiters != NULL) && (pps_str != NULL))
    {
        ps_token = ps_str + x_strspn(ps_str, ps_delimiters);
        
        if (*ps_token != '\0')
        {
            size_t z_token_len;
            
            z_token_len = x_strcspn(ps_token, ps_delimiters);
            
            *pps_str = ps_token + z_token_len;
            
            if (pz_token_len != NULL)
            {
                /* original string is not modified */
                *pz_token_len = z_token_len;
            }
            else if (ps_token[z_token_len] != '\0')
            {
                /* a NULL character overwrites part of the original string */
                ps_token[z_token_len] = '\0';
                (*pps_str)++;
            }
        }
        else
        {
            *pps_str = NULL;
            ps_token = NULL;
        }
    }
    
    return ps_token;
}
#endif

