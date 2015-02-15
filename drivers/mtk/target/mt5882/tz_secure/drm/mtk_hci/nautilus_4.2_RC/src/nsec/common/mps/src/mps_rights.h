/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */

/**
 * @addtogroup mps mps
 * @ingroup goby
 * @{
 *
 * @file
 * @brief Rights management steps for node, link, license etc...
 */

#ifndef MPS_L_RIGHTS_H
#define MPS_L_RIGHTS_H

/**
 * Set the rights management step to an optional value.
 * Optional actions after config token and before processing BB actions are (in order)
 *
 * - Download CRL
 * - DUS
 * - DCS
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_atkn      Action token with steps to be used when no options remain
 * @param[in,out]  io_result    Step result, may be NULL
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_STOKEN_ILLEGALST
 */
np_ret_t
mps_l_SetOptionalAction( mps_l_context_t        *io_cxt,
                         stoken_action_token_t  *in_atkn,
                         ns_step_result_t       *io_result );


/**
 * Process the first step of config token acquitistion. The config token URL is specified in an action token if
 * one was provided, otherwise this is the first step in a TT update which gets config tokens from file
 *
 * @param[in,out]  io_cxt       MPS context with action token
 * @param[in,out]  io_result    Step result
 * @param[in,out]  io_response  If the configuration tokens are on file, the token will be in the response
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 */
np_ret_t
mps_l_bb_CtknAcqStep1( mps_l_context_t      *io_cxt,
                       ns_step_result_t     *io_result,
                       ns_http_response_t   *io_response );


/**
 * Process a HTTP response containg a downloaded config token
 *
 * @param[in,out]  io_cxt       MPS context with step information and config token ID
 * @param[in]      in_response  HTTP response with config token
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::stoken_ParseConfigToken
 * @return ::mps_l_WriteConfigToken
 * @return ::mps_l_SetOptionalAction
 */
np_ret_t
mps_l_bb_CtknAcqStep2( mps_l_context_t      *io_cxt,
                       ns_http_response_t   *in_response,
                       ns_step_result_t     *io_result );


/**
 * Create a HTTP request for CRL download.
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 */
np_ret_t
mps_l_bb_CrlAcqStep1( mps_l_context_t     *io_cxt,
                      ns_step_result_t    *io_result );


/**
 * Process a HTTP response containg a downloaded CRL
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with CRL
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_STOKEN_ILLEGALST
 */
np_ret_t
mps_l_bb_CrlAcqStep2( mps_l_context_t       *io_cxt,
                      ns_http_response_t    *in_response,
                      ns_step_result_t      *io_result );


#if defined( NPOPT_ENABLE_DUS )
/**
 * Create a HTTP request for data update.service
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_SetBBCertSet
 * @return ::nssmi_DataUpdateStep1
 */
np_ret_t
mps_l_bb_DusStep1( mps_l_context_t     *io_cxt,
                   ns_step_result_t    *io_result );


/**
 * Process a HTTP response containg a downloaded data update message
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with DUS message
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_DataUpdateStep2
 */
np_ret_t
mps_l_bb_DusStep2( mps_l_context_t      *io_cxt,
                   ns_http_response_t   *in_response,
                   ns_step_result_t     *io_result );
#endif

#if defined( NPOPT_ENABLE_DCS )
/**
 * Create a HTTP request for data certification.service
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_SetBBCertSet
 * @return ::l_GetRequestedDataCertificationStdandards
 * @return ::nssmi_DataCertificationStep1
 */
np_ret_t
mps_l_bb_DcsStep1( mps_l_context_t      *io_cxt,
                   ns_step_result_t     *io_result );


/**
 * Process a HTTP response containg a downloaded data certification message
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with DCS message
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_DataCertificationStep2
 */
np_ret_t
mps_l_bb_DcsStep2( mps_l_context_t      *io_cxt,
                   ns_http_response_t   *in_response,
                   ns_step_result_t     *io_result );
#endif

/**
 * Create a HTTP request for Node acquisition
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_SetBBCertSet
 * @return ::nssmi_RegistrationNodeAcqStep1
 */
np_ret_t
mps_l_bb_NodeAcqStep1( mps_l_context_t     *io_cxt,
                       ns_step_result_t    *io_result );


/**
 * Process a HTTP response containg a downloaded node
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with node
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_RegistrationNodeAcqStep2
 */
np_ret_t
mps_l_bb_NodeAcqStep2( mps_l_context_t      *io_cxt,
                       ns_http_response_t   *in_response,
                       ns_step_result_t     *io_result );


/**
 * Create a HTTP request for Link acquisition
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_SetBBCertSet
 * @return ::nssmi_RegistrationLinkAcqStep1
 */
np_ret_t
mps_l_bb_LinkAcqStep1( mps_l_context_t    *io_cxt,
                       ns_step_result_t   *io_result );


/**
 * Process a HTTP response containg a downloaded link
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with link
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::nssmi_RegistrationLinkAcqStep2
 */
np_ret_t
mps_l_bb_LinkAcqStep2( mps_l_context_t      *io_cxt,
                       ns_http_response_t   *in_response,
                       ns_step_result_t     *io_result );


/**
 * Finalise link acquisition
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_STOKEN_ILLEGALST
 */
np_ret_t
mps_l_bb_LinkAcqStep3( mps_l_context_t      *io_cxt,
                       ns_step_result_t     *io_result );


/**
 * Create a HTTP request for deregistration
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_SetBBCertSet
 * @return ::nssmi_DeregistrationStep1
 */
np_ret_t
mps_l_bb_DeregStep1( mps_l_context_t      *io_cxt,
                     ns_step_result_t     *io_result );


/**
 * Process a HTTP response containg a downloaded deregistration response
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with dereg message
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::nssmi_DeregistrationStep2
 */
np_ret_t
mps_l_bb_DeregStep2( mps_l_context_t      *io_cxt,
                     ns_http_response_t   *in_response,
                     ns_step_result_t     *io_result );


/**
 * Finalise deregistration
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_STOKEN_ILLEGALST
 */
np_ret_t
mps_l_bb_DeregStep3( mps_l_context_t      *io_cxt,
                     ns_step_result_t     *io_result );


/**
 * Create a HTTP request for License acquisition
 *
 * @param[in,out]  io_cxt        MPS context
 * @param[in,out]  io_result     Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_SetBBCertSet
 * @return ::nssmi_LicenseAcqStep1
 */
np_ret_t
mps_l_bb_LicAcqStep1( mps_l_context_t     *io_cxt,
                      ns_step_result_t    *io_result );


/**
 * Process a HTTP response containg a downloaded license
 *
 * @param[in,out]  io_cxt       MPS context with step information
 * @param[in]      in_response  HTTP response with license
 * @param[in,out]  io_result    Step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_LicenseAcqStep2
 */
np_ret_t
mps_l_bb_LicAcqStep2( mps_l_context_t      *io_cxt,
                      ns_http_response_t   *in_response,
                      ns_step_result_t     *io_result );

/**
 * @brief Initialise a LTP context for a sink
 *
 * @param[in,out]    io_cxt       MPS context with step information
 * @param[in,out]    io_result    Result of a SetupMsgBuild on the sink
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::nssmi_ltpInitSink
 * @return ::nssmi_ltpSetupMsgBuild
 * @return ::mps_l_CreateHTTPRequest
 */
np_ret_t
mps_l_ltp_SetupSink( mps_l_context_t      *io_cxt,
                     ns_step_result_t     *io_result );

/**
 * @brief Initialise a LTP context for a source
 *
 * @param[in,out] io_cxt       MPS context with step information
 * @param[in]     in_response  Setup message created by the sink
 * @param[in,out] io_result    RunAgentOnPeerMessage to send to the sink wrapped in the request of a step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::nssmi_ltpSetupMsgProc
 * @return ::mps_l_CreateHTTPRequest
 */
np_ret_t
mps_l_ltp_SetupSource( mps_l_context_t      *io_cxt,
                       ns_http_response_t   *in_response,
                       ns_step_result_t     *io_result );

/**
 * @brief Runs the agent on a peer (sink)
 *
 * @param[in,out] io_cxt       MPS context with step information
 * @param[in]     in_response  RunAgentOnPeerMessage from the source
 * @param[in,out] io_result    The AgentResult message to send to the source wrapped in the request of a step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::nssmi_ltpRunAgentOnPeerMsgProc
 * @return ::mps_l_CreateHTTPRequest
 */
np_ret_t
mps_l_ltp_RunAgent( mps_l_context_t      *io_cxt,
                    ns_http_response_t   *in_response,
                    ns_step_result_t     *io_result );

/**
 * @brief Process the agent result from the sink, this closes the LTP session on the source
 *
 * @param[in,out] io_cxt       MPS context with step information
 * @param[in]     in_response  AgentResult message from the sink
 * @param[in,out] io_result    Teardown message to send to the sink wrapped in the request of a step result
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::nssmi_ltpAgentResultMsgProc
 * @return ::nssmi_ltpFin
 * @return ::mps_l_CreateHTTPRequest
 */
np_ret_t
mps_l_ltp_AgentResult( mps_l_context_t      *io_cxt,
                       ns_http_response_t   *in_response,
                       ns_step_result_t     *io_result );

/**
 * @brief Process the teardown message from the source, this closes the LTP session on the sink
 *
 * @param[in,out] io_cxt       MPS context with step information
 * @param[in]     in_response  Teardown message from the source
 * @param[in,out] io_result    No request is possible, but a write message action will be returned with the transferred license
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::nssmi_ltpTeardownMsgProc
 * @return ::nssmi_ltpFin
 */
np_ret_t
mps_l_ltp_Teardown( mps_l_context_t      *io_cxt,
                    ns_http_response_t   *in_response,
                    ns_step_result_t     *io_result );

/**
 * Initialise BB with the correct keys and certificate standard list, based on the
 * incoming protocol information
 *
 * @param[in]  in_cxt      MPS context with config token and SMI handle
 * @param[in]  in_proto    BB protocol type (DUS, DCS, Dereg etc.)
 * @param[out] out_nodeid  Server node ID for protocol steps
 * @param[out] out_url     URL for service required by HTTP callbacks
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_STOKEN_ILLEGALST
 * @return ::nssmi_InitBBHandle
 * @return ::nssmi_SetBBCertSet
 */
np_ret_t
mps_l_SetupBB( mps_l_context_t   *in_cxt,
               u_int32_t          in_proto,
               char             **out_nodeid,
               char             **out_url );

/**
 * Create a HTTP request structure with URL, payload, POST/GET flag and extra headers
 *
 * @param[in]   in_url          URL of server to connect to
 * @param[in]   in_method       NS_HTTP_METHOD_GET or NS_HTTP_METHOD_POST
 * @param[in]   in_body         SOAP message payload
 * @param[out]  out_request     Placeholder for HTTP request structure
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::l_GetExtraHeaders
 */
extern np_ret_t
mps_l_CreateHTTPRequest( char                  *in_url,
                         u_int32_t              in_method,
                         binstr_t              *in_body,
                         ns_http_request_t    **out_request );

#endif /* MPS_L_RIGHTS_H */
/** @} */
