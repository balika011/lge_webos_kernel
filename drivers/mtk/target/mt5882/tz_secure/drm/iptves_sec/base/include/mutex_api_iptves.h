/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2005,2006,2007,2008 Sony Corporation
 */
#ifndef MUTEX_API_IPTVES_H
#define MUTEX_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Mutex API
 *
 * require: base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Error Codes
 *==========================================================================*/

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Initialize mutex variable
 *
 * @param [out]    out_mutex    Mutex variable
 * @retval RET_OK               Success
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 */
extern int
mutex_Init(mutex_t             *out_mutex);

/**
 * Finalize mutex variable
 *
 * @param [in,out] io_mutex     Mutex variable
 * @retval RET_OK               Success
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 */
extern int
mutex_Fin(mutex_t              *io_mutex);

/**
 * Get lock
 *
 * @param [in,out] io_mutex     Mutex variable
 * @retval RET_OK               Success
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR    An unexpected error has occurred
 */
extern int
mutex_Lock(mutex_t             *io_mutex);

/**
 * Release lock
 *
 * @param [in,out] io_mutex     Mutex variable
 * @retval RET_OK               Success
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 */
extern int
mutex_Unlock(mutex_t           *io_mutex);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_API_H */
