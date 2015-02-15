/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 *
 */
#ifndef MUTEX_API_H
#define MUTEX_API_H

NP_BEGIN_EXTERN_C

/** @file
 * Mutex functions
 *
 * @require cmn/base/binstr
 *
 * @author SAKAMOTO Takahiro <takas@sm.sony.co.jp>
 */

/*******************************
 * ERROR CODES
 *******************************/

#define ERR_MUTEX_UNSUPPORTED    (ERR_MUTEX_BASE|1)
#define ERR_MUTEX_BUSY           (ERR_MUTEX_BASE|2)
#define ERR_MUTEX_TIMEOUT        (ERR_MUTEX_BASE|3)
#define ERR_MUTEX_DEADLOCK       (ERR_MUTEX_BASE|4)

/*******************************
 * inter thread lock
 *******************************/

/**
 * @brief Initializes a mutex
 *
 * @param[out] out_mutex    the mutex variable allocated by the caller.
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 */
extern retcode_t
npi_mutex_Init(mutex_t *out_mutex);

/**
 * @brief Locks the mutex
 *
 * @param[out] io_mutex      the mutex variable.
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_INTERNALERROR          <br>An unrecoverable error has occurred in Nautilus.
 */
extern retcode_t
npi_mutex_Lock(mutex_t *io_mutex);

/**
 * @brief Unlocks the mutex
 *
 * @param[out] io_mutex      the mutex variable.
 *
 * @return ::ERR_OK                      <br>Indicating success
 * @return ::ERR_ILLEGALARGS             <br>The input arguments are incorrect. Arguments cannot be NULL.
 */
extern retcode_t
npi_mutex_Unlock(mutex_t *io_mutex);

/**
 * @brief Finalizes a mutex
 *
 * @param[out] io_mutex      the mutex variable.
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 */
extern retcode_t
npi_mutex_Fin(mutex_t *io_mutex);

/*******************************
 * inter process lock
 *******************************/

typedef void                *mutex_proc_t;

/**
 * @brief Obtains an inter process lock
 *
 * @param[in] in_context            context
 * @param[in] in_id                 the lock identifier
 * @param[in] in_timeout            Neg: Non-blocking, Zero: blocking, Pos: timeout (msec).
 * @param[out] out_mutex            the lock handle
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_INTERNALERROR          <br>An unrecoverable error has occurred in Nautilus.
 */
extern retcode_t
npi_mutex_ProcLock(npi_cmncontext_t in_context, binstr_t *in_id, int32_t in_timeout, mutex_proc_t *out_mutex);

/**
 * @brief Releases the inter process lock
 *
 * @param[in] in_context     context
 * @param[in] in_mutex       the lock handle
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_INTERNALERROR          <br>An unrecoverable error has occurred in Nautilus.
 */
extern retcode_t
npi_mutex_ProcUnlock(npi_cmncontext_t in_context, mutex_proc_t in_mutex);

NP_END_EXTERN_C

#endif /* MUTEX_API_H */
/** @} */
