/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file si_sys_malloc.c
 *
 * @brief Dynamic memory allocation from static memory pool
 *
 *****************************************************************************/

#ifndef SI_SYS_MALLOC_API_H
#define SI_SYS_MALLOC_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public functions ******************************************************/

void* SiiSysMallocCreate( uint16_t size );
void SiiSysMallocDelete( void* p );
uint16_t SiiSysMallocBytesAllocatedGet( void );
void SiiSysMallocLock( void );
void SiiSysMallocDeleteAll( void );

#endif /* SI_SYS_MALLOC_API_H */

/***** end of file ***********************************************************/
