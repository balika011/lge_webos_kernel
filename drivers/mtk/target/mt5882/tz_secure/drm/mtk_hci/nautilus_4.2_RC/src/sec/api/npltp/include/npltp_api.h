/** @addtogroup npltp npltp
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

#ifndef NPLTP_API_H
#define NPLTP_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Nautilus Library API (LTP)
 */
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
/**
 * @brief Initialize LTP component.
 *
 * Init BB with the nemo credentials from another party of the transfer,
 * if they are specified. Otherwise output the credentials for communicating with
 * another party.
 * Credentials include the following : role assertion, nemo pub. This information must
 * also be persistent for the life of the transfer protocol.
 *
 * @param[out] out_handle     The buffer where the component handle will be
 *                                stored, for the source.
 * @param[in]  in_certstdlst  The certificate standard list of the source.
 * @param[in,out] io_nenccert The number of encryption certs in the list
 * @param[in,out] io_enccert  The nemo encryption certificate list.
 * @param[in,out] io_nsigcert The number of signature certs in the list
 * @param[in,out] io_sigcert  The nemo signature  certificate list.
 * @param[in,out] io_roleast  The nemo role assertion.
 * @param[in,out] io_node_id  The nemo node id.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_Init(hdls_context_t          *out_handle,
           np_certstandard_list_t  *in_certstdlst,
           u_int32_t               *io_nenccert,
           np_str_t              ***io_enccert,
           u_int32_t               *io_nsigcert,
           np_str_t              ***io_sigcert,
           np_str_t               **io_roleast,
           np_str_t               **io_node_id);

/*----------------------------------------------------------------------------*/
/**
 * @brief Finalize LTP component.
 *
 * This API should be invoked at the end of use of the component, both for the
 * source and the sink.
 *
 * @param[in]     io_handle Session handle to be finalized.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_Fin(hdls_context_t in_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Build an LTP setup message.
 *
 * It compose an LTP setup message and then protect it with Nemo.
 *
 * @param[in]  io_snk_handle   NP ltp handle.
 * @param[in]  in_license      The license that will be encapsulated in the
 *                             message.
 * @param[out] out_secbuf_msg  The setup message as secure buffer.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_SetupMsgBuild(hdls_context_t  in_snk_handle,
                    np_str_t       *in_license,
                    np_secbuffer_t *out_secbuf_msg);

/*----------------------------------------------------------------------------*/
/**
 * @brief Process an LTP setup message.
 *
 *   - Decompose the message protected by Nemo.
 *   - Parse the message.
 *   - Set license in DRM component.
 *   - Store the agent control in LTP component.
 *   - Call transfer.action.perform and get the ESB.
 *   - Cache first play date in LTP component.
 *
 * @param[in]  io_src_handle      NP ltp handle.
 * @param[in]  in_setup_message   The setup message protected by Nemo.
 * @param[out] out_secbuf_msg     The run_agent_message message as secure buffer.
 * @param[out] out_secbuf_sdata   A snapshot of sdata as secure buffer.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALLICENSE
 * @return ::ERR_NP_ACTIONNOTGRANTED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_SetupMsgProc(hdls_context_t  in_src_handle,
                   np_str_t       *in_setup_message,
                   np_secbuffer_t *out_secbuf_msg,
                   np_secbuffer_t *out_secbuf_sdata);



/*----------------------------------------------------------------------------*/
/**
 * @brief Process an LTP RunAgentOnPeerMsg message.
 *
 * @param[in]  in_snk_handle         NP ltp handle.
 * @param[in]  in_run_agent_message  The message protected by Nemo.
 * @param[out] out_secbuf_msg        The out message as secure buffer.
 * @param[out] out_secbuf_sdata      A snapshot of sdata as secure buffer.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_RunAgentOnPeerMsgProc(hdls_context_t  in_snk_handle,
                            np_str_t       *in_run_agent_message,
                            np_secbuffer_t *out_secbuf_msg,
                            np_secbuffer_t *out_secbuf_sdata);

/*----------------------------------------------------------------------------*/
/**
 * @brief Process an LTP AgentResult message.
 *
 * @param[in]  in_src_handle                  NP ltp handle.
 * @param[in]  in_agent_res_message           The message protected by Nemo.
 * @param[out] out_secbuf_msg                The out message as secure buffer.
 * @param[out] out_secbuf_sdata              A snapshot of sdata as secure buffer.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_AgentResultMsgProc(hdls_context_t  in_src_handle,
                         np_str_t       *in_agent_res_message,
                         np_secbuffer_t *out_secbuf_msg,
                         np_secbuffer_t *out_secbuf_sdata);

/*----------------------------------------------------------------------------*/
/**
 * @brief Process an LTP teardown message.
 *
 * Get the content key list from the teardown message and create the new license.
 *
 * @param[in]  in_snk_handle    NP ltp handle of the Sink device.
 * @param[in]  in_license       The original license on the source
 * @param[in]  in_message       The teardown message containing the new content keys
 * @param[out] out_secbuf_lic   The new license
 * @param[out] out_secbuf_sdata A snapshot of sdata as secure buffer.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
extern np_ret_t
npltp_TeardownMsgProc( hdls_context_t   in_snk_handle,
                       np_str_t        *in_license,
                       np_str_t        *in_message,
                       np_secbuffer_t  *out_secbuf_lic,
                       np_secbuffer_t  *out_secbuf_sdata );


NP_END_EXTERN_C

#endif /* NPLTP_API_H */
/** @} */
