/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef BPTR_API_IPTVES_H
#define BPTR_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Binary String Type Definition and API
 *
 * require: none
 */
/*==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    const u_int8_t *p;
    size_t          sz;
} bptr_t;

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define BPTR_NULL               { NULL, 0 }
#define bptr_Copy(src,dst)      do { *(dst) = *(src); } while (0)

/*============================================================================
 * APIs
 *==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* BPTR_API_H */
