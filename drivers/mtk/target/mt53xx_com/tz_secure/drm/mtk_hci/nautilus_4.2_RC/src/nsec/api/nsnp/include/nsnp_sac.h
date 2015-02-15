/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

/**
 * @addtogroup nsnp_sac functionality
 * @ingroup nsnp
 * @{
 *
 * @brief nsnp layer APIs for the Nautilus SAC
 *
 * @see NautilusAPIReference.pdf
 *
 * @file
 * SAC APIs
 *
 */

#ifndef NSNP_SAC_API_H
#define NSNP_SAC_API_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                             PUBLIC API                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Create a sac session
 *
 * @param[in,out] io_sac_handle        Handle for the session
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nsnp_sacInitSession(np_handle_t       io_sac_handle);


/**
 * @brief Finalise a sac session
 *
 * @param[in,out] io_sac_handle      Handle for the session to close
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nsnp_sacFinSession(np_handle_t         io_sac_handle);

/**
 * @brief Generate a SAC request message
 *
 * Call a SAC client to generate the SAC request message
 *
 * @param[in]     in_sac_handle        Handle for the sac session
 * @param[in]     in_serv_handle       Handle for the service session to use
 *                                     to generate the service specific SAC
 *                                     message. It can be either a np_handle_t
 *                                     or a np_trackhandle_t
 * @param[in]     in_msg_type          Type of request message to generate
 * @param[in]     in_msg_info          Service specific information
 *                                     used when generating the request message
 * @param[in]     in_transaction_id    Opaque Application data that can be used
 *                                     to track the progress of a SAC protocol
 *                                     message. Its use is optional
 * @param[out]    out_request          SAC message containing the request
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_SAC_PROTO
 * @return ::ERR_NP_SAC_SERV_MSG
 * @return ::ERR_NP_SHAREDKEY_INVALID
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nsnp_sacGenerateRequest(np_handle_t           in_sac_handle,
                        np_sac_serv_handle_t *in_serv_handle,
                        np_sac_msg_t          in_msg_type,
                        np_sac_msg_info_t    *in_msg_info,
                        u_int32_t             in_transaction_id,
                        np_str_t            **out_request);

/**
 * @brief Process a SAC request message and generate a SAC response
 *
 * Call a SAC server to process the SAC request message
 *
 * @param[in]     in_sac_handle        Handle for the sac session
 * @param[in]     in_serv_handle       Handle for the service session to use
 *                                     to parse the service specific SAC
 *                                     message and generate a response to it
 *                                     It can be either a np_handle_t or a
 *                                     np_trackhandle_t
 * @param[in]     in_request           SAC message containing the request
 * @param[out]    out_transaction_id   Opaque Application id used for tracking
 *                                     the SAC protocol messages. Its use
 *                                     is optional
 * @param[out]    out_response         SAC message containing the response
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_SAC_PROTO
 * @return ::ERR_NP_SAC_SERV_MSG
 * @return ::ERR_NP_SHAREDKEY_INVALID
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nsnp_sacProcessRequest(np_handle_t           in_sac_handle,
                       np_sac_serv_handle_t *in_serv_handle,
                       np_str_t             *in_request,
                       u_int32_t            *out_transaction_id,
                       np_str_t            **out_response);

/**
 * @brief Parse a SAC response message
 *
 * Call a SAC client to process the SAC response message
 *
 * @param[in]     in_sac_handle        Handle for the sac session
 * @param[in]     in_serv_handle       Handle for the service session to use
 *                                     to parse the service specific SAC
 *                                     message. It is either a np_handle_t or
 *                                     a np_trackhandle_t
 * @param[in]     in_response          SAC message containing the response
 * @param[out]    out_transaction_id   Opaque Application id used for tracking
 *                                     the SAC protocol messages. Its use
 *                                     is optional
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_SAC_PROTO
 * @return ::ERR_NP_SAC_SERV_MSG
 * @return ::ERR_NP_SHAREDKEY_INVALID
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nsnp_sacParseResponse(np_handle_t           in_sac_handle,
                      np_sac_serv_handle_t *in_serv_handle,
                      np_str_t             *in_response,
                      u_int32_t            *out_transaction_id);

NP_END_EXTERN_C

#endif /* NSNP_SAC_API_H */
/** @} */
