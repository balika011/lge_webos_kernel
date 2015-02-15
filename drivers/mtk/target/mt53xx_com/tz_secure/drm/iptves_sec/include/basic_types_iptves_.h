/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef BASIC_TYPES_IPTVES__H
#define BASIC_TYPES_IPTVES__H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Basic type and macro definition (PO internal, platform depended)
 *
 * require: none
 */
/*==========================================================================*/

/*============================================================================
 * Check Platform
 *==========================================================================*/

#ifndef __linux__
#define __linux__
#endif




//MTK FIX




#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>


#ifndef IPTVES_PUBLISH_BUILD
//#define IPTVES_PUBLISH_BUILD
#endif



/*
 * def: PO_MODULE
 */

#define PO_MODULE(modulename)                   \
    char *l_modulename = #modulename

/*
 * def: INLINE
 */
//#define INLINE          inline

/*
 * type: fio_size_t, fio_offset_t
 *
 *   There is no necessity which is fio_size_t == np_size_t.
 *   fio_size_t may change to handle the file which exceeds 4G Byte
 *   in the future.
 */
typedef u_int32_t       fio_size_t;
typedef int32_t         fio_offset_t;

/*
 * type: mutex_t
 */
typedef struct {

    //pthread_mutex_t     mutex;
    
    int                 init;
} mutex_t;






#ifdef __cplusplus
}
#endif

#endif
