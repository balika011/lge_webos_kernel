/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012-03-28
  File Revision : 1.0.0.0
------------------------------------------------------------------------------*/
/**

 @file    sony_stdlib.h

          This file provides the C standard lib function mapping.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_STDLIB_H
#define SONY_STDLIB_H

/*
 PORTING. Please modify if ANCI C standard library is not available.
*/
/*#include <string.h>*/


/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

#include <x_os.h> /* For MTK */

/**
 @brief Alias for memcpy.
*/
#define sony_memcpy  x_memcpy

/**
 @brief Alias for memset.
*/
#define sony_memset  x_memset

/**
 @brief Alias for abs.
*/
#define sony_abs  abs

#endif /* SONY_STDLIB_H */
