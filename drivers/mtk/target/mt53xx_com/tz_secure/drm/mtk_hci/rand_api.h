/** @addtogroup crypto crypto
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 *
 */

#ifndef RAND_API_H
#define RAND_API_H

NP_BEGIN_EXTERN_C

/*
 * Rand generator errors
 */
#define ERR_RAND_INIT_FAIL         (ERR_RAND_BASE|1)
#define ERR_RAND_GENERATIONFAILURE (ERR_RAND_BASE|2)

/*---------------------------------------------------------------------------*
 * APIs
 *---------------------------------------------------------------------------*/

typedef void *rand_ctx_t;

/*
 * Rand context should be set to this value before initialisation
 * to be sure that finalisation is called correctly.
 */
#define RAND_NULL ((void*)0)

/**
 * @brief Initializes the random context
 *
 * @param[in]  in_context  sec/nsec context
 * @param[out] out_ctx     random number generator context
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_NOMEM                  <br>A resource failure has occurred.
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_INTERNALERROR          <br>An unrecoverable error has occurred in Nautilus.
 * @return ::ERR_FIO_CANNOTOPEN         <br>If file cannot open
 * @return ::ERR_FIO_NOTEXIST           <br>If file doesn't exist
 * @return ::ERR_FIO_ILLEGALMODE        <br>If file have some illegal name.
 * @return :: ERR_RAND_INIT_FAIL        <br>If acquisition of cryptographic context failed.
 */
retcode_t
rand_Init(rand_ctx_t       *out_ctx);

/**
 * @brief Sets the random context to allow blocking. RNGs may block while
 * awaiting enough entropy to be gathered. This should be only done
 * for the most secure use scenarios.
 *
 * @param in_context  sec/nsec context
 * @param io_ctx      random number generator context
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_NOMEM                  <br>A resource failure has occurred.
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_ILLEGALHANDLE          <br>If  handle is NULL or in use by another protocol
 * @return ::ERR_FIO_CANNOTOPEN         <br>If file cannot open
 * @return ::ERR_FIO_NOTEXIST           <br>If file doesn't exist
 * @return ::ERR_FIO_ILLEGALMODE        <br>If file have some illegal mode.
 */
retcode_t
rand_SetBlocking(rand_ctx_t        io_ctx);

/**
 * @brief Sets the random context to disallow blocking.
 *
 * @param in_context  sec/nsec context
 * @param io_ctx      random number generator context
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_NOMEM                  <br>A resource failure has occurred.
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_ILLEGALHANDLE          <br>If  handle is NULL or in use by another protocol
 * @return ::ERR_FIO_CANNOTOPEN         <br>If file cannot open
 * @return ::ERR_FIO_NOTEXIST           <br>If file doesn't exist
 * @return ::ERR_FIO_ILLEGALMODE        <br>If file have some illegal mode.
 */
retcode_t
rand_SetNonBlocking(rand_ctx_t        io_ctx);

/**
 * @brief Gets random bytes
 *
 * @param[in]     in_context  sec/nsec context
 * @param[in]     in_ctx      random number generator context
 * @param[in]     in_len      number of bytes to return
 * @param[in]     out_buf     buffer in which to store the random bytes
 *
 * @return ::ERR_OK                         <br>Indicating success
 * @return ::ERR_ILLEGALARGS                <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_ILLEGALHANDLE              <br>If  handle is NULL or in use by another protocol
 * @return ::ERR_FIO_CANNOTREAD             <br>If the file cannot be read
 * @return ::ERR_RAND_GENERATIONFAILURE     <br>If error generation fails.
 */
retcode_t
rand_GetBytes(rand_ctx_t        in_ctx,
              u_int32_t         in_len,
              void             *out_buf);

/**
 * @brief Finalizes the random context
 *
 * @param[in] in_context  sec/nsec context
 * @param[in] in_ctx      random number generator context
 *
 * @return ::ERR_OK                         <br>Indicating success
 * @return ::ERR_ILLEGALARGS                <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_INTERNALERROR              <br>An unrecoverable error has occurred in Nautilus.
 */
retcode_t
rand_Fin(rand_ctx_t        in_ctx);

NP_END_EXTERN_C

#endif /* RAND_API_H */
/** @} */
