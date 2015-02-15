/** @addtogroup ltp ltp
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef LTP_API_H
#define LTP_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief library for License Transfer protocol
 *
 * This library contains functions to generate ltp setup messages, parse the
 * setup request message and store the agent control in the LTP context.It
 * provides api's to run agent on peer and parse the respose message. It also
 * creates action perform parameters.
 */
/*===========================================================================*/

/*----------------------------------------------------------------------------*
 * type definitions
 *----------------------------------------------------------------------------*/

typedef void *ltp_handle_t;

/*----------------------------------------------------------------------------*
 * error code definitions
 *----------------------------------------------------------------------------*/

#define ERR_LTP_BADSETUPMESSAGE         (ERR_LTP_BASE|1)
#define ERR_LTP_ILLEGALXML              (ERR_LTP_BASE|2)
#define ERR_LTP_WRONGESB                (ERR_LTP_BASE|3)
#define ERR_LTP_WRONGCATEGORY           (ERR_LTP_BASE|4)
#define ERR_LTP_BADLICENSE              (ERR_LTP_BASE|5)
#define ERR_LTP_ILLEGALPERSONALITY      (ERR_LTP_BASE|6)


typedef struct ltp_action_param {
    binstr_t    *sink_node_id;
    binstr_t    *transfer_mode;
    u_int32_t    transfer_count;
    u_int32_t    sink_proximity_lastprobe;
} ltp_action_param_t;


/**
 * @brief Initialize an LTP handle.
 *
 * @param[out] out_handle  LTP handle
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 */
extern retcode_t
ltp_Init(ltp_handle_t *out_handle);


/**
 * @brief Finalize an LTP handle.
 *
 * This function finalizes (deallocates) a LTP handle. This function should
 * be called only once for a given handle.
 *
 * @note The handle is not set to NULL.
 *
 * @param[in,out] io_handle LTP protocol handle.
 *
 * @return ::ERR_OK
 */
extern retcode_t
ltp_Fin(ltp_handle_t io_handle);


/**
 * @brief Retrieve Nemo Personality
 *
 * @param[out] out_node  nemo Personality
 *
 * @return ::ERR_OK
 * @return from reg_GetInternalNEMOPublicPersonality
 */
extern retcode_t
ltp_GetNEMOPersonality(xml2oct_nemonode_t **out_node);

/**
 * @brief Retrieve Octopus Public Personality
 *
 * @param[out] out_personality  Octopus Public Personality
 *
 * @return ::ERR_OK
 * @return from reg_GetPersonality
 */
extern retcode_t
ltp_GetOctPubPersonality( binstr_t **out_personality );

/**
 * @brief Build a setup message.
 *
 * It creates an XML LTP setup message encapsulated in a Request element. This
 * message needs a license and the Octopus public personality.
 *
 * @param[in,out] io_handle         LTP protocol handle.
 * @param[in]     in_oct_pub_perso  XML Octopus public personality.
 * @param[in]     in_license        XML license you want to transfer.
 * @param[out]    out_msgnode       XML setup message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_XML_PARSEFAIL
 */
extern retcode_t
ltp_SetupMsgBuild(ltp_handle_t   io_handle,
                  binstr_t      *in_oct_pub_perso,
                  binstr_t      *in_license,
                  xml_node_t   **out_msgnode);


/**
 * @brief Process a setup message.
 *
 * Parse an LTP setup request message, validate it and return the license
 * contained inside.
 *
 * @param[in]     in_xml_setup_request   XML setup request message.
 * @param[out]    out_snk_oct_pub_perso  Octopus public personality form the
 *                                       SINK extracted from the message.
 * @param[out]    out_msgnode            License extracted from the message.
 *
 * @return ::ERR_OK
 * @return ::ERR_LTP_BADSETUPMESSAGE
 */
extern retcode_t
ltp_SetupMsgProc(xml_node_t    *in_xml_setup_request,
                 binstr_t     **out_snk_oct_pub_perso,
                 binstr_t     **out_license);


/**
 * @brief Store an agent control in the context and cache the license.
 *
 * It parses the XML license, extracts the agent control and stores the agent
 * control (code module) in the LTP context. It does the same things for the
 * signature of the control.
 * It also caches the license thats passed in so it can be used in the teardown
 * message build for creating the contentkey xml nodes.
 *
 * @param[in,out] io_handle   LTP protocol handle.
 * @param[in]     in_license  License from which to extract the agent control.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
ltp_StoreAgentControlAndLicense(ltp_handle_t  io_handle,
                                binstr_t     *in_license);



/**
 * @brief Get the Extension Bundle containing controls & signature.
 *
 * @param[in,out] io_handle               LTP protocol handle.
 * @param[out]    out_extension_bundle    Returned Extension Bundle (XML format).
 *
 * @note  The out param shall not be freed by the caller.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
ltp_GetExtensionBundle(ltp_handle_t    io_handle,
                       xml_node_t    **out_extension_bundle);


/**
 * @brief Extract the category value from an ESB encapsulated in a teardown
 *        message.
 *
 * @param[in,out] io_handle              LTP protocol handle.
 * @param[in]     in_teardown_msg        The teardown message.
 * @param[out]    out_category           The category of ESB.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_LTP_WRONGESB
 * @return ::ERR_LTP_WRONGCATEGORY
 * @return ::ERR_XML_NOTFOUND
 */
extern retcode_t
ltp_GetEsbCategoryFromTeardownMessage(ltp_handle_t  io_handle,
                                      xml_node_t   *in_teardown_msg,
                                      u_int32_t    *out_category);


/**
 * @brief Builds a ParameterBlock.
 *
 * Create an XML ParameterBlock
 *
 * @param[in]     in_vallist       List of nodes containing the ROAP Obligation Parameters.
 * @param[out]    out_parameters   XML Node Vector of ParameterBlocks.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return l_CreateXmlElementIntegerValueBlock
 * @return l_CreateXmlElementParameterBlock
 */
extern retcode_t
ltp_BuildParameterBlock(esbman_valuelist_t  *in_vallist,
                        xml_node_vec_t     **out_parameters);


/**
 * @brief Build a RunAgentOnPeer message.
 *
 * Create an XML LTP RunAgentOnPeer message encapsulated in a Request element.
 *
 * @param[in]     in_agent_name          Agent name.
 * @param[in]     in_context_id          Context Id
 * @param[in]     in_control_id          Control Id
 * @param[in]     in_parameter_blocks    ParameterBlocks
 * @param[in]     in_extension_bundle    Extension bundle containing agent controls & signature
 * @param[out]    out_msgnode            XML out message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return from l_CreateXmlElementBundle
 * @return from l_CreateXmlElementAgentCarrier
 * @return from l_CreateXmlElementLtpRunAgentPayload
 * @return from l_CreateXmlElementLtpResponsePayload
 */
extern retcode_t
ltp_RunAgentOnPeerMsgBuild(binstr_t          *in_agent_name,
                           binstr_t          *in_context_id,
                           binstr_t          *in_control_id,
                           xml_node_vec_t    *in_parameter_blocks,
                           xml_node_t        *in_extension_bundle,
                           xml_node_t       **out_msgnode);


/**
 * @brief Process a RunAgentOnPeerMsg message.
 *
 * Parse an LTP RunAgentOnPeerMsg response message, validate it and return the
 * agent contained inside.
 *
 * @param[in,out] io_handle                  LTP protocol handle.
 * @param[in]     in_xml_run_agent_response  XML RunAgentOnPeerMsg response message.
 * @param[out]    out_agent                  Agent extracted from the message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return from l_ParseRunAgentOnPeerMsg
 */
extern retcode_t
ltp_RunAgentOnPeerMsgProc( xml_node_t    *in_xml_run_agent_response,
                           xml_node_t   **out_agent);


/**
 * @brief Build an AgentResult message.
 *
 * It creates an XML LTP AgentResult message encapsulated in a Request element.
 *
 * @param[in,out] io_handle               LTP protocol handle.
 * @param[in]     in_agent_carrie_result  Agent Carrier REsult
 * @param[out]    out_msgnode             XML out message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return from l_CreateXmlElementBundle
 * @return from l_CreateXmlElementAgentCarrier
 * @return from l_CreateXmlElementLtpRunAgentPayload
 * @return from l_CreateXmlElementLtpResponsePayload
 */
extern retcode_t
ltp_AgentResultMsgBuild(ltp_handle_t   io_handle,
                        xml_node_t    *in_agent_carrie_result,
                        xml_node_t   **out_msgnode);


/**
 * @brief Process a AgentResult message.
 *
 * Parse an LTP AgentResult request message, validate it and return the resCode and
 * resVal contained inside.
 *
 * @param[in]     in_xml_agent_res_request  XML RunAgentOnPeerMsg response message.
 * @param[out]    out_resCode               Agent Result code  extracted from the message.
 * @param[out]    out_resVal                Agent REsult Value extracted from the message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return from l_ParseAgentResultMsg
 */
extern retcode_t
ltp_AgentResultMsgProc(xml_node_t    *in_xml_agent_res_request,
                       int32_t       *out_resCode,
                       binstr_t     **out_resVal);


/**
 * @brief Build a teardown message.
 *
 * Create an XML LTP teardown message encapsulated with nemo. This message needs
 * an esb, a list of contentkeys (with their respective secretkey- and
 * contentkey-id), and the id of the key used to encrypt the contentkeys.
 *
 * @param[in,out] in_handle             LTP protocol handle.
 * @param[in]     in_nb_contentkeys     The number of contentkeys passed in.
 * @param[in]     in_contentkeys        The contentkeys.
 * @param[in]     in_midkeys            Any midkeys used if content keys are too large
 * @param[in]     in_secretkeyids       The secretkeyids (of the contentkeys).
 * @param[in]     in_contentkeyids      The contentkeyids (of the contentkeys).
 * @param[in]     in_ck_reencrypt_keyid The id of the key used to encrypt the
 *                                      contentkeys.
 * @param[in]     in_secbuf_cb_esb      The ESB returned bu onAgentCompletion
 *                                      callback.
 * @param[out]    out_msgnode           XML teardown message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_NOMEM
 * @return ::ERR_LTP_WRONGESB
 * @return ::ERR_XML_ILLEGALNODE
 */
extern retcode_t
ltp_TeardownMsgBuild(ltp_handle_t     in_handle,
                     u_int32_t        in_nb_contentkeys,
                     binstr_t       **in_contentkeys,
                     binstr_t       **in_midkeys,
                     binstr_t       **in_secretkeyids,
                     binstr_t       **in_contentkeyids,
                     binstr_t        *in_ck_reencrypt_keyid,
                     binstr_t        *in_cb_esb,
                     xml_node_t     **out_msgnode);

/**
 * @brief Build Action Perform parameters and return the XML bundle node based
 *        on an Octopus Public Personality.
 *
 * @param[in]     in_oct_pub_perso      An XML Octopus Public Personality.
 * @param[out]    out_params            action parameters .
 * @param[out]    out_bundle_node       The parsed bundle node.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_LTP_ILLEGALXML
 */
extern retcode_t
ltp_BuildActionPerformParams(binstr_t           *in_oct_pub_perso,
                             ltp_action_param_t *out_action_params,
                             xml_node_t        **out_bundle_node);


NP_END_EXTERN_C

#endif /* LTP_API_H */
/** @} */
