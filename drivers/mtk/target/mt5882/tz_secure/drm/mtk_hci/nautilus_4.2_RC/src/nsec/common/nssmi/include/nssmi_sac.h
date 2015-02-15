/** @addtogroup nssmi nssmi
 *  @ingroup nsec
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef NSSMI_SAC_API_H
#define NSSMI_SAC_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Create a sac session
 *
 * @param[in,out] io_sac_handle        Handle for the session
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nssmi_sacInitSession(np_handle_t       io_sac_handle);


/**
 * @brief Finalise a sac session
 *
 * @param[in,out] io_sac_handle      Handle for the session to close
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nssmi_sacFinSession(np_handle_t         io_sac_handle);

/**
 * @brief Generate a SAC request message
 *
 * Call a SAC client to generate the SAC request message
 *
 * @param[in]     in_sac_handle        Handle for the sac session
 * @param[in]     in_hdl_type          Service handle type (track or import session)
 * @param[in]     in_proto             Handle for the service session to use (as np_protocol_t)
 *                                     to generate the service specific SAC
 *                                     message
 * @param[in]     in_msg_type          Type of request message to create
 * @param[in]     in_msg_info         (optional) Service specific information
 *                                     used when creating the request message
 * @param[in]     in_transaction_id    Opaque application value
 * @param[out]    out_request          SAC message containing the request
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_SAC_SERV_MSG
 * @return ::ERR_NP_SHAREDKEY_INVALID
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
extern np_ret_t
nssmi_sacGenerateRequest(np_handle_t            in_sac_handle,
                         np_sac_handle_type_t   in_hdl_type,
                         np_protocol_t          in_proto,
                         np_sac_msg_t           in_msg_type,
                         np_sac_msg_info_t     *in_msg_info,
                         u_int32_t              in_transaction_id,
                         np_str_t             **out_request);

/**
 * @brief Parse a SAC request message and create a SAC response
 *
 * Call a SAC server to process the SAC request message
 *
 * @param[in]     in_sac_handle        Handle for the sac session
 * @param[in]     in_hdl_type          Service handle type (track or import session)
 * @param[in]     in_proto             Handle for the service session to use (as np_protocol_t)
 *                                     to parse the service specific SAC
 *                                     message and create a response to it
 * @param[in]     in_request           SAC message containing the request
 * @param[in]     in_transaction_id    Opaque application value
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
nssmi_sacProcessRequest(np_handle_t           in_sac_handle,
                        np_sac_handle_type_t in_hdl_type,
                        np_protocol_t         in_proto,
                        np_str_t             *in_request,
                        u_int32_t            *out_transaction_id,
                        np_str_t            **out_response);

/**
 * @brief Parse a SAC response message
 *
 * Call a SAC client to process the SAC response message
 *
 * @param[in]     in_sac_handle        Handle for the sac session
 * @param[in]     in_hdl_type          Service handle type (track or import session)
 * @param[in]     in_proto             Handle for the service session to use (as np_protocol_t)
 *                                     to parse the service specific SAC
 *                                     message
 * @param[in]     in_response          SAC message containing the response
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
nssmi_sacParseResponse(np_handle_t          in_sac_handle,
                       np_sac_handle_type_t in_hdl_type,
                       np_protocol_t        in_proto,
                       np_str_t            *in_response,
                       u_int32_t           *out_transaction_id);

NP_END_EXTERN_C

#endif /* NSSMI_SAC_API_H */
/** @} */
