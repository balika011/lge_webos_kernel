/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
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

#endif /* SONY_STDLIB_H */
