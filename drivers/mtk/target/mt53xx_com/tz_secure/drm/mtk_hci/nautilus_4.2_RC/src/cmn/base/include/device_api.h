/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 *
 */

#ifndef DEVICE_API_H
#define DEVICE_API_H

/** @file
 * Device Information Type Definition and API
 *
 * @require cmn/base/binstr
 * @author Manabu Kimura <Manabu.Kimura@jp.sony.com>
 */

NP_BEGIN_EXTERN_C


/*---------------------------------------------------------------------------*
 * APIs
 *---------------------------------------------------------------------------*/

/**
 * Gets Device Unique Identifier
 *
 * @param[in] in_context               Security context
 *
 * @note buffer must be freed by binstr_Free()
 *
 * @return newly-allocated buffer containing device-unique-identifier.
 * @return NULL                if no Uid is there.
 */
extern binstr_t *
npi_device_GetUid(
    npi_cmncontext_t        in_context);

/**
 * Gets Secret Device Unique Identifier
 *
 * @param[in] in_context               Security context
 *
 * @note buffer must be freed by binstr_Free()
 *
 * @return newly-allocated buffer containing a secret device-unique-identifier.
 * @return NULL if there is a memory allocation issue
 */
extern binstr_t *
npi_device_GetSecretUid(
    npi_cmncontext_t        in_context);

/**
 * Sets Device Unique Identifier
 *
 * @param[in] in_context       Security context
 * @param[in] in_len           UID length
 * @param[in] in_buf           UID buffer
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 */
extern retcode_t
npi_device_SetUid(
    npi_cmncontext_t        in_context,
    u_int32_t               in_len,
    u_int8_t                *in_buf);

NP_END_EXTERN_C

#endif /* DEVICE_API_H */
/** @} */
