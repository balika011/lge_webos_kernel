/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 */

#ifndef STRTOL_H
#define STRTOL_H

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================*/
/** @file linux_strtol v1.0
 *
 * @brief Provide the same functionality as strtol from C "stdlib.h". 
 * That is, convert an ASCII string to a type long representation, given the 
 * radix.
 * 
 * @author Valentin Mesaros <valentinm@sonycom.com>
 * @date   29 Nov, 2006
 */
/*===========================================================================*/


/**
 * @brief Given a radix, convert an ASCII string pointed to by str to
 * a type long representation (2^32 - 1). The ASCII characters will be
 * converted from ranges '0' - '9', 'A' - 'Z' and 'a' - 'z' to
 * corresponding values within ranges 0 - 9 and 10 - 35, respectively.
 *
 * @see Unix strtol
 */
extern int32_t 
strtol(const char  *str,
       char        **endstr,
       int32_t         base);


#ifdef __cplusplus
}
#endif
#endif /* STRTOL_H */
