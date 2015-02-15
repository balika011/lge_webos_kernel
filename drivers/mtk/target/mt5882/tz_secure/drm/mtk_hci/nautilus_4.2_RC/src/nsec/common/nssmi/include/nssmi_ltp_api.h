/** @addtogroup nssmi nssmi
 *  @ingroup nsec
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

#ifndef NSSMI_LTP_API_H
#define NSSMI_LTP_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Perform the initialization of npltp_handle_t handle for source
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_certstdlst       Certification standards
 * @param[out] out_nenccerts       Number of source encrytption certificates
 * @param[out] out_enccerts        Source encrytption certificates
 * @param[out] out_nsigcerts       Number of source signature certificates
 * @param[out] out_sigcerts        Source signature certificates
 * @param[out] out_roleast         Source role assertion
 * @param[out] out_nodeid          Source node id
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npltp_Init
 */
extern np_ret_t
nssmi_ltpInitSource(np_handle_t              io_handle,
                    np_certstandard_list_t   *in_certstdlist,
                    u_int32_t                *out_nenccerts,
                    binstr_t               ***out_enccerts,
                    u_int32_t                *out_nsigcerts,
                    binstr_t               ***out_sigcerts,
                    binstr_t                **out_roleast,
                    binstr_t                **out_nodeid);

/**
 * @brief  Perform the initialization of npltp_handle_t handle for sink
 *
 * @param[in]      in_handle          @ref example_smi "SMI" target info
 * @param[in,out]  io_certstdlst      Certification standards
 * @param[in]      in_nenccerts       Number of source encrytption certificates
 * @param[in,out]  io_enccerts        Source encrytption certificates
 * @param[in]      in_nsigcerts       Number of source signature certificates
 * @param[in,out]  io_sigcerts        Source signature certificates
 * @param[in]      in_roleast         Source role assertion
 * @param[in]      in_nodeid          Source node id
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npltp_Init
 */
extern np_ret_t
nssmi_ltpInitSink(np_handle_t               io_handle,
                  np_certstandard_list_t  **io_certstdlist,
                  u_int32_t                 in_nenccerts,
                  binstr_t                **io_enccerts,
                  u_int32_t                 in_nsigcerts,
                  binstr_t                **io_sigcerts,
                  binstr_t                 *in_roleast,
                  binstr_t                 *in_nodeid);

/*----------------------------------------------------------------------------*/
/**
 * @brief Perform the initialization of npltp_handle_t handles for source and sink
 *
 * @param[in,out] io_src_handle LTP handle for source and SMI target info.
 * @param[in,out] io_snk_handle LTP handle for sink   and SMI target info.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * %%CR%JR%% missing functions
 * @return ::l_ltpInitDefaultCertStdLst
 * @return ::npltp_Init
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpInit(np_handle_t  io_src_handle,
              np_handle_t  io_snk_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Perform the finalization of a npltp_handle_t
 *
 * @param[in,out] io_handle        LTP and connection info handle.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * %%CR%JR%% missing functions
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpFin(np_handle_t io_handle );


/*----------------------------------------------------------------------------*/
/**
 * @brief License Transfer Protocol setup message build.
 *
 * Create a setup message on the sink that will be processed on the source
 *
 * @param[in] ihandle            LTP handle of the sink and SMI target info
 * @param[in] in_license         License that will be transfered.
 * @param[out] out_message       Buffer for the generated setup message.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * %%CR%JR%% missing functions
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpSetupMsgBuild(np_handle_t      in_handle,
                       binstr_t        *in_license,
                       binstr_t       **out_message);


/*----------------------------------------------------------------------------*/
/**
 * @brief License Transfer Protocol setup message processing.
 *
 * Process the setup message on the source, set the license in DRM
 * engine, run the transfer action perform operation and return the ESB.
 *
 * @param[in] io_src_handle              LTP handle of the source and SMI target info
 * @param[in] in_setup_message           Setup message provided by sink.
 * @param[out] out_run_agent_message     Buffer for generated message.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALLICENSE
 * @return ::ERR_NP_ACTIONNOTGRANTED
 * @return ::ERR_NP_ILLEGALESB
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * %%CR%JR%% missing functions
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpSetupMsgProc(np_handle_t      in_handle,
                      binstr_t        *in_setup_message,
                      binstr_t       **out_run_agent_message);


/*----------------------------------------------------------------------------*/
/**
 * @brief License Transfer Protocol RunAgentOnPeer message processing.
 *
 * @param[in] io_snk_handle              LTP handle and SMI target info
 * @param[in] in_run_agent_message       message
 * @param[out] out_agent_res_message     Buffer for Body data.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::npltp_RunAgentOnPeerMsgProc
 * @return ::npltp_AgentResultMsgBuild
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * %%CR%JR%% missing functions
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpRunAgentOnPeerMsgProc(np_handle_t      in_handle,
                               binstr_t        *in_run_agent_message,
                               binstr_t       **out_agent_res_message);


/*----------------------------------------------------------------------------*/
/**
 * @brief License Transfer Protocol AgentResult message processing.
 *
 * @param[in] io_src_handle             LTP handle and SMI target info
 * @param[in] agent_res_message         message
 * @param[out] out_teardown_message     Buffer for Body data.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::l_GetCallbackInfosFromEsb
 * @return ::npltp_AgentResultMsgProc
 * @return ::npltp_TeardownMsgBuild
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * %%CR%JR%% missing functions
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpAgentResultMsgProc(np_handle_t      in_handle,
                            binstr_t        *agent_res_message,
                            binstr_t       **out_teardown_message);

/*----------------------------------------------------------------------------*/
/**
 * @brief Convert SRC license to SNK.
 *
 * This will replace the encrypted key data in the license with key data encrypted
 * by the SNK public key if the ESB action is GRANTED in the message
 *
 * @param[in]  in_snk_handle  Handle of the Sink device and SMI target info
 * @param[in]  in_license     SRC license
 * @param[in]  in_message     Teardown message containing the new content key list
 * @param[out] out_license    Output destination for new license
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::npltp_TeardownMsgProc
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * %%CR%JR%% missing functions
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ltpTeardownMsgProc(np_handle_t      in_handle,
                         binstr_t        *in_license,
                         binstr_t        *in_message,
                         binstr_t       **out_license);

NP_END_EXTERN_C

#endif /* NSSMI_LTP_API_H */


/** @} */
