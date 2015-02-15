/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 */

/**
 * @addtogroup nsff nsff
 * @ingroup nsff
 * @{
 *
 * @brief
 * The <i>nsff</i> layer provides all the necessary functionality for integrating Marlin File Format Extensions
 *
 * @see NautilusAPIReference.pdf
 */

#ifndef NSFF_API_H
#define NSFF_API_H

#include "nsff_cenc.h"
#include "nsff_mpd.h"

NP_BEGIN_EXTERN_C

#if defined (NPOPT_MODE_FF)

/**
 * @brief This initializes the nff library. This function shall be called once
 * per process (and not in every thread), before any other nsff function. This
 * allocates heap memory from non-secure world. This api performs the finalize
 * operation if there is any error occurs and free the allocated buffer allocated
 * internally else on success to finalize and free the allocated buffer user need
 * to call nsff_Fin(). The buffer passed from outside of this API in its first argument
 * it is the user responsibility to free it in both success and unsuccess condition of this api.
 * Note: This api is not thread safe.
 *
 * @param[in]  in_nsecmembody      Pointer to non-secure memory that nsff can use as nonsecure
 *                                 heap, or NULL for internal allocation.
 *
 * @param[in]  in_nsecmemsize      The allowed size is zero (NP_DEFAULT_MEM_SIZE) or greater
 *                                 than equal to 1024 bytes.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 */
extern np_ret_t
nsff_Init(void *in_nsecmembody, np_size_t in_nsecmemsize);


/**
 * @brief This function finalizes the use of nsff library.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_NOTINITIALIZED
 */
extern np_ret_t
nsff_Fin(void);
#endif

/**
 * @brief Frees the string, it should be used only and always to free for
 * strings allocated by nsff.
 *
 * @param[in,out]  io_string
 *
 * @return ::ERR_NP_OK
 */
extern np_ret_t
nsff_FreeString(np_str_t *io_string);

NP_END_EXTERN_C

#endif /* NSFF_API_H */
/** @} */
