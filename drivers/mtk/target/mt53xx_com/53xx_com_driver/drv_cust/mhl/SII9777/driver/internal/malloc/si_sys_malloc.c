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
//#define SII_DEBUG 3

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_malloc);

/***** local macro definitions ***********************************************/

#define MEMPOOL_SIZE_IN_BYTES   (0x2000)
#define MEMPOOL_SIZE_IN_WORDS   ((MEMPOOL_SIZE_IN_BYTES>>2)+2)

/***** local prototypes ******************************************************/

static void sMemoryClear( uint16_t size, uint16_t ptr );

/***** local data objects ****************************************************/

static uint32_t  sMemPool[MEMPOOL_SIZE_IN_WORDS]; /* Ensure memory pool location at 4-byte boundary */
static uint16_t  sPtr     = 0;                    /* Pointer to next avaialble memory location      */
static bool_t    sbLock   = false;

/***** public functions ******************************************************/

void* SiiSysMallocCreate( uint16_t size )
{
    uint16_t words = ((size>>2)+1); /* Round up to nearest number of words (1 word = 4 bytes) */

    /* Check if memory pool is locked */
    SII_ASSERT(!sbLock);

    if( MEMPOOL_SIZE_IN_WORDS > (sPtr+words) )
    {
        uint16_t ptr = sPtr;
 
        /* Clear memory */
        sMemoryClear(words, sPtr);

        /* Increase pointer to next available memory location */
        sPtr += words;

        return (void*)(&sMemPool[ptr]);
    }
    else
    {
        SII_ASSERT(0);
        return NULL;
    }
}

void SiiSysMallocDelete( void* p )
{
    /* Make sure that delete pointer is in allocated memory space */
    SII_ASSERT((uint32_t*)p >= &sMemPool[0]);
    SII_ASSERT((uint32_t*)p <  &sMemPool[sPtr]);

    /* Make sure that delete pointer is on a even 4 byte boundary */
    SII_ASSERT(!((((uint8_t*)p) - ((uint8_t*)&sMemPool[0]))%4));

    sPtr = (((uint8_t*)p) - ((uint8_t*)&sMemPool[0])) >> 2;
}

uint16_t SiiSysMallocBytesAllocatedGet( void )
{
    /* Return total amound of bytes allocated */
    return (uint16_t)(sPtr<<2);
}

#if 0 // not used
void SiiSysMallocLock( void )
{
    sbLock = true;
}

void SiiSysMallocDeleteAll( void )
{
    sbLock = false;
    sPtr   = 0;
}
#endif

/***** local functions *******************************************************/

static void sMemoryClear( uint16_t size, uint16_t ptr )
{
    uint32_t* pData = &sMemPool[ptr];

    while( size-- )
    {
        *pData = 0;
        pData++;
    }
}

/***** end of file ***********************************************************/
