/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
 */

/**
 * @addtogroup mps mps
 * @ingroup goby
 * @{
 *
 * @file
 * @brief  Marlin USB protocol sequencer
 * @note   this API is incomplete and subject to change
 */

#ifndef MPS_API_H
#define MPS_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Creates a context for parsing and processing an action token
 *
 * @param[in]    in_type                 One of MPS_ACTION_BB, MPS_ACTION_LTP, MPS_ACTION_TT
 * @param[in]    in_action_token         Action token (in XML)
 * @param[in]    in_service_id           Global unique identifier for the service specific information
 * @param[in]    in_service_information  Service-specific information (i.e. relatedToken in [MUSB]) to pass back to caller
 * @param[in]    in_transport            Enables selection of local client (::goby) or USB 0.8 protocol (::dorado)
 * @param[out]   out_handle              Handle to an ::mps context
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::stoken_ParseActionToken
 * @return ::mps_l_SetOptionalAction
 */
extern np_ret_t
mps_InitManageRights(u_int32_t              in_type,
                     binstr_t              *in_action_token,
                     binstr_t              *in_service_id,
                     binstr_t              *in_service_information,
                     np_handle_t            in_transport,
                     mps_handle_t          *out_handle);


/**
 * @brief Terminates AT processing and frees an ::mps context.
 *
 * @param[in]   io_handle     Handle to the ::mps context
 *
 * @return ::ERR_NP_OK
 */
extern np_ret_t
mps_FinManageRights(mps_handle_t  io_handle);


/**
 * @brief Set parameters to be used for device attestation
 * @param[in,out] in_handle        MPS handle for which to update the context
 * @param[in]     in_manufacturer  Manufacturer value
 * @param[in]     in_model         Model value
 * @param[in]     in_app_id        Optional application identification value
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
extern np_ret_t
mps_SetAttestation(mps_handle_t  io_handle,
                   np_str_t     *in_manufacturer,
                   np_str_t     *in_model,
                   np_str_t     *in_app_id);

/**
 * @brief Executes one step in action token processing for acquiring rights
 *
 * @param[in,out]  io_handle    Handle to an ::mps context
 * @param[in]      in_response  HTTP response to a request returned by previous call in this context, can be NULL
 * @param[out]     out_result   The results of the step, possibly including an HTTP request
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::mps_l_bb_CtknAcqStep1
 * @return ::mps_l_bb_CtknAcqStep2
 * @return ::mps_l_bb_CrlAcqStep1
 * @return ::mps_l_bb_CrlAcqStep2
 * @return ::mps_l_bb_DusStep1
 * @return ::mps_l_bb_DusStep2
 * @return ::mps_l_bb_DcsStep1
 * @return ::mps_l_bb_DcsStep2
 * @return ::mps_l_bb_NodeAcqStep1
 * @return ::mps_l_bb_NodeAcqStep2
 * @return ::mps_l_bb_LinkAcqStep1
 * @return ::mps_l_bb_LinkAcqStep2
 * @return ::mps_l_bb_LinkAcqStep3
 * @return ::mps_l_bb_DeregStep1
 * @return ::mps_l_bb_DeregStep2
 * @return ::mps_l_bb_DeregStep3
 * @return ::mps_l_bb_LicAcqStep1
 * @return ::mps_l_bb_LicAcqStep2
 */
extern np_ret_t
mps_StepManageRights(mps_handle_t            io_handle,
                     ns_http_response_t    *in_response,
                     ns_step_result_t     **out_result);

/**
 * @brief Create the Configuration Token for an LTP action
 *
 * @param[in,out]  io_handle    Handle to an ::mps context
 * @param[out]     out_result   Configuration context encapsulated in a step result for passing to StepManageRights
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::mps_l_CreateHTTPRequest
 * @return ::nssmi_ltpInitSource
 * @return ::stoken_GenerateConfigToken
 */
np_ret_t
mps_GenerateLtpConfigToken(mps_handle_t       io_handle,
                           ns_step_result_t **out_result);

/**
 * @brief Create the Action Token for an LTP transfer
 *
 * @param[out] out_atkn  Token
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::stoken_GenerateActionToken
 */
np_ret_t
mps_GenerateLtpActionToken(binstr_t **out_atkn);

NP_END_EXTERN_C

#endif /* MPS_API_H */
/** @} */
