/*------------------------------------------------------------------------------
  Copyright 2010-2012 Sony Corporation

  Last Updated  : 2012/03/14
  File Revision : 1.0.0.0
------------------------------------------------------------------------------*/

#ifndef SONYRF_STDLIB_H
#define SONYRF_STDLIB_H

#include "sonyrf_common_re231.h"

/*
 <PORTING> Please modify if ANCI C standard library is not available.
*/

#include <x_os.h> /* For MTK */

#define sonyrf_memcpy  x_memcpy
#define sonyrf_memset  x_memset

#endif /* SONYRF_STDLIB_H */
