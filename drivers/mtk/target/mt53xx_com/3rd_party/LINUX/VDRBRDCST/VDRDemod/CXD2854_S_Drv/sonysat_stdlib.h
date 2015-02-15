/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**

 @file    sonysat_stdlib.h

          This file provides the C standard lib function mapping.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONYSAT_STDLIB_H
#define SONYSAT_STDLIB_H


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
#define sonysat_memcpy  x_memcpy

/**
 @brief Alias for memset.
*/
#define sonysat_memset  x_memset

/**
 @brief Alias for abs.
*/
#define sonysat_abs  abs

#endif /* SONYSAT_STDLIB_H */
