/** @addtogroup npbbts npbbts
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef NPBBTS_API_H
#define NPBBTS_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file npbbts_api.h
     *
     * @brief Nautilus Library API (BBTS)
     *
     * depends on ?
     *
     * @date   3 Jan. 2011 Initial writing
     */
    /*===========================================================================*/


    /**
     * @brief Create a secure bbts session/context with default marlin basic & dtcp output controls
     *
     * @param[in]   in_basic_moc       The default marlin basic output controls
     * @param[in]   in_dtcp_moc        The default marlin dtcp output controls
     * @param[in]   in_bbts_mode       Processing mode for the context. Enables
     *                                 strict checking of the ecm for the supplied
     *                                 mode
     * @param[out]  in_bbts_handle_ctx An allocated and initialized secure BBTS context
     *
     * @return ::ERR_NP_OK             <br>Indicating success.
     * @return ::ERR_NP_ILLEGALARGS    <br>The arguments to the function are incorrect.
     * @return ::ERR_NP_OUTOFMEMORY    <br>Nautilus encountered resource problems.
     * @return ::ERR_NP_INTERNALERROR  <br>An unrecoverable failure occurred in Nautilus.
     * @return ::hdls_InitHdlMap       <br>Errors returned by sub-functions
     * @return ::hdls_MapHdlToIdx      <br>Errors returned by sub-functions
     */
    extern np_ret_t
    npbbts_InitSession(np_moc_t        in_basic_moc,
                       np_moc_t        in_dtcp_moc,
                       np_bbts_mode_t  in_bbts_mode,
                       hdls_context_t *out_bbts_handle_ctx);


    /**
     * @brief Finalise and destroy secure bbts session/context
     *
     * @param[in] in_bbts_handle_ctx The secure BBTS context to remove
     *
     * @return ::ERR_NP_OK             <br>Indicating success.
     * @return ::ERR_NP_OUTOFMEMORY    <br>Nautilus encountered resource problems.
     * @return ::ERR_NP_INTERNALERROR  <br>An unrecoverable failure occurred in Nautilus.
     * @return ::hdls_MapIdxToHdl      <br>Errors returned by sub-functions
     * @return ::hdls_SetHdlMapping    <br>Errors returned by sub-functions
     */
    extern np_ret_t
    npbbts_FinSession(hdls_context_t in_bbts_handle_ctx);


    /* -------------------------------------------------------------------------- */
    /**
     * @brief Overload and provide an Application specific serviceBaseCID to use
     *
     * @param[in]      in_bbts_handle_ctx  Handle for the session
     * @param[in]      in_socid            socID to overload in the session
     *
     * @return ::ERR_NP_OK                 <br>Indicating success.
     * @return ::ERR_NP_ILLEGALHANDLE      <br>The handle passed to the function is invalid.
     * @return ::ERR_NP_OUTOFMEMORY        <br>Nautilus encountered resource problems.
     * @return ::ERR_NP_ILLEGALARGS        <br>The arguments to the function are incorrect.
     */
    /* -------------------------------------------------------------------------- */
    extern np_ret_t
    npbbts_SetSocID(hdls_context_t  in_bbts_handle_ctx,
                    binstr_t       *in_socid);

    /* -------------------------------------------------------------------------- */
    /**
     * @brief Initiate a ECM stream with the bbts session
     *
     * @param[in]      in_bbts_handle_ctx  Handle for the session
     * @param[in]      in_ecm              ECM to be processed
     * @param[in]      in_servicebase_cid  ServiceBaseCID
     * @param[in]      out_serv_content_id Service content ID
     * @param[in]      out_prog_content_id Program content ID
     *
     * @return ::ERR_NP_OK                 <br>Indicating success.
     * @return ::ERR_NP_ILLEGALHANDLE      <br>The handle passed to the function is invalid. Or already called successfully.
     * @return ::ERR_NP_OUTOFMEMORY        <br>Nautilus encountered resource problems.
     * @return ::ERR_NP_ILLEGALARGS        <br>The arguments to the function are incorrect.
     */
    /* -------------------------------------------------------------------------- */
    np_ret_t
    npbbts_InitECMStream(hdls_context_t       in_bbts_handle_ctx,
                         binstr_t            *in_ecm,
                         binstr_t            *in_servicebase_cid,
                         np_secbuffer_t      *out_serv_content_id,
                         np_secbuffer_t      *out_prog_content_id);

#ifdef __cplusplus
}
#endif

#endif /* NPBBTS_API_H */
/** @} */
