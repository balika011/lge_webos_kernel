/** @addtogroup npbb npbb
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef NPBB_API_H
#define NPBB_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief Nautilus Library API (BB)
     *
     * depend on npdrm_api.h
     *
     * @author Norifumi Goto <n-goto@sm.sony.co.jp>
     * @date   25 May, 2005 last modified by n-goto
     */
    /*===========================================================================*/

#include <npbb_types.h>

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Initialize BB handle
     *
     * @param[in] in_protocoltype BB protocol type
     * @param[in] in_certstdlst CertificationStandard list (OPTIONAL)
     * @param[out] out_handle buffer for handle
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGSnpbb_handle_t *out_handle
     * @return ::ERR_NP_OUTOFMEMORY
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_InitHandle(int32_t                 in_protocoltype,
                    np_certstandard_list_t *in_certstdlst,
                    hdls_context_t         *out_handle);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Finalize BB handle
     *
     * @param[in]    in_handle handle
     * @return ::ERR_NP_OK
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_FinHandle(hdls_context_t in_handle);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Registration service (user/subscription node acquisition)
     *   Step1: generate nodeAcquisitionRequest
     *   Step2: parse nodeAcquisitionResponse and return user-node
     *
     * @param[in]    in_handle BB handle
     * @param[in] in_bztklen length of business-token
     * @param[in] in_bztkbuf buffer of business-token
     * @param[in] in_servernode
     * @param[in] in_response response message to be parsed
     * @param[out] out_secbuf Step1: buffer for generated request message
     *                            Step2: buffer where sdata will be stored
     */
    /*---------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NONEMOPERSONALITYREGISTERED   (Nemo Personality is not registered)
     * @return ::ERR_NP_ILLEGALPERSONALITY            (Nemo Personality is invalid)
     * @return ::ERR_NP_ASSERTIONNOTFOUND             (One or more DCS Assertion(s) are not registered)
     * @return ::ERR_NP_ILLEGALASSERTION              (Registered assertion is illegal)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_RegistrationNodeAcqStep1(hdls_context_t  in_handle,
                                  np_size_t       in_bztklen,
                                  u_int8_t       *in_bztkbuf,
                                  char           *in_servernode,
                                  np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_MESSAGECORRUPTED      (response message is corrupted)
     * @return ::ERR_NP_SOAPFAULT             (response is a soap-fault)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_RegistrationNodeAcqStep2(hdls_context_t  in_handle,
                                  np_str_t       *in_response,
                                  np_secbuffer_t *out_secbuf_sdata_nodeid,
                                  np_secbuffer_t *out_secbuf_sdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Registration service (acquire link)
     *   Step1: generate linkAcquisitionRequest
     *   Step2: parse linkAcquisitionResponse and return user-node
     *
     * @param[in]    in_handle BB handle
     * @param[in] in_bztklen length of business-token
     * @param[in] in_bztkbuf buffer of business-token
     * @param[in] in_servernode
     * @param[in] in_linkfrom source link node (NULL if LinkFrom is device node)
     * @param[in] in_linkto destination link node
     * @param[in] in_response response message to be parsed
     * @param[out] out_secbuf buffer for generated request message
     * @param[out] out_secsdata pointer to secure buffer

     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NONEMOPERSONALITYREGISTERED   (Nemo Personality is not registered)
     * @return ::ERR_NP_ILLEGALPERSONALITY            (Illegal Octopus or Nemo Personality)
     * @return ::ERR_NP_ASSERTIONNOTFOUND             (One or more DCS Assertion(s) are not registered)
     * @return ::ERR_NP_ILLEGALASSERTION              (Illegal Assertion)
     * @return ::ERR_NP_NOPERSONALITYREGISTERED       (Octpus Personality is not registered)
     * @return ::ERR_NP_ILLEGALNODE                   (Illegal linkfrom/linkto node)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_RegistrationLinkAcqStep1(hdls_context_t  in_handle,
                                  np_size_t       in_bztklen,
                                  u_int8_t       *in_bztkbuf,
                                  char           *in_servernode,
                                  np_str_t       *in_linkfrom,
                                  np_str_t       *in_linkto,
                                  np_secbuffer_t *out_secbuf,
                                  np_secbuffer_t *out_secsdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
     * @return ::ERR_NP_SOAPFAULT             (response is a soap-fault)
     * @return ::ERR_NP_ILLEGALLINK           (illegal link)
     * @return ::ERR_NP_ALREADYREGISTERED     (link with given id is already registered)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_RegistrationLinkAcqStep2(hdls_context_t  in_handle,
                                  np_str_t       *in_response,
                                  np_secbuffer_t *out_secbuf_linkid,
                                  np_secbuffer_t *out_secbuf_cnf,
                                  np_secbuffer_t *out_secbuf_sdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief License acquisition service
     *   Step1: generate licenseAcquisitionRequest
     *   Step2: parse licenseAcquisitionResponse and return the license
     *
     * @param[in]    in_handle BB handle
     * @param[in] in_bztklen length of business-token
     * @param[in] in_bztkbuf buffer of business-token
     * @param[in] in_servernode
     * @param[in] in_nodeid node id where the license will be bound
     * @param[in] in_response response message to be parsed
     * @param[out] out_secbuf buffer for generated request message
     * @param[out] out_secsdata pointer to secure buffer
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NONEMOPERSONALITYREGISTERED   (Nemo Personality is not registered)
     * @return ::ERR_NP_ILLEGALPERSONALITY            (Illegal Octopus or Nemo Personality)
     * @return ::ERR_NP_ASSERTIONNOTFOUND             (One or more DCS Assertion(s) are not registered)
     * @return ::ERR_NP_ILLEGALASSERTION              (Illegal Assertion)
     * @return ::ERR_NP_NOPERSONALITYREGISTERED       (Octopus Personality is not registered)
     * @return ::ERR_NP_ILLEGALNODE                   (Illegal node)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_LicenseAcqStep1(hdls_context_t  in_handle,
                         np_size_t       in_bztklen,
                         u_int8_t       *in_bztkbuf,
                         char           *in_servernode,
                         np_str_t       *in_nodeid,
                         np_secbuffer_t *out_secbuf,
                         np_secbuffer_t *out_secsdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
     * @return ::ERR_NP_SOAPFAULT             (response is a soap-fault)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_LicenseAcqStep2(hdls_context_t  in_handle,
                         np_str_t       *in_response,
                         np_secbuffer_t *out_secbuf_license,
                         np_secbuffer_t *out_secbuf_sdata);

#if defined(NPOPT_ENABLE_DCS)
    /*---------------------------------------------------------------------------*/
    /**
     * @brief Data Certification service
     *   Step1: generate DataCertificationRequest
     *   Step2: parse DataCertificationResponse
     *
     * @param[in]    in_handle BB handle
     * @param[in] in_servernode
     * @param[in] in_crt_stds A list of Certification Standards (together with
     *                           metadata items) which have to be included in the
     *                           request.
     * @param[in] in_num_crt_stds A number of Certification Standards.
     * @param[in] in_response response message to be parsed
     * @param[out] out_secbuf Step1: buffer for generated request message
     *                            Step2: pointer to secure buffer
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NONEMOPERSONALITYREGISTERED   (Nemo Personality is not registered)
     * @return ::ERR_NP_ILLEGALPERSONALITY            (Illegal nemo Personality)
     * @return ::ERR_NP_ILLEGALSL                     (Illegal SuspensionList)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_DataCertificationStep1(hdls_context_t   in_handle,
                                char            *in_servernode,
                                np_crt_std_t   **in_crt_stds,
                                np_size_t        in_num_crt_stds,
                                np_secbuffer_t  *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
     * @return ::ERR_NP_SOAPFAULT             (response is a soap-fault)
     * @return ::ERR_NP_NEEDTTUPDATE          (TrustedTime should be updated)
     * @return ::ERR_NP_NEEDSLUPDATE          (SuspensionList should be updated)
     * @return ::ERR_NP_NEEDUPDATE            (TrustedTime and SuspensionList should be updated)
     */
    /*---------------------------------------------------------------------------*/

    extern np_ret_t
    npbb_DataCertificationStep2(hdls_context_t  in_handle,
                                np_str_t       *in_response,
                                np_secbuffer_t *out_secbuf);
#endif /* #if defined(NPOPT_ENABLE_DCS) */

#if defined(NPOPT_ENABLE_DUS)
    /*---------------------------------------------------------------------------*/
    /**
     * @brief Data Update service
     *   Step1: generate DataUpdateRequest
     *   Step2: parse DataUpdateResponse
     *
     * @param[in]    in_handle BB handle
     * @param[in] in_servernode
     * @param[in] in_updatelfag specified which data will be updated(NPBB_DUS_UPDATE_TT|NPBB_DUS_UPDATE_LSL)
     * @param[in] in_response response message to be parsed
     * @param[out] out_secbuf Step1: buffer for generated request message
     *                            Step2: pointer to secure buffer
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NONEMOPERSONALITYREGISTERED   (Nemo Personality is not registered)
     * @return ::ERR_NP_ILLEGALPERSONALITY            (Illegal nemo Personality)
     * @return ::ERR_NP_ILLEGALSL                     (Illegal SuspensionList)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_DataUpdateStep1(hdls_context_t  in_handle,
                         char           *in_servernode,
                         int32_t         in_updateflags,
                         np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
     * @return ::ERR_NP_SOAPFAULT             (response is a soap-fault)
     * @return ::ERR_NP_ILLEGALSL             (Illegal SuspensionList)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_DataUpdateStep2(hdls_context_t  in_handle,
                         np_str_t       *in_response,
                         np_secbuffer_t *out_secbuf);
#endif /* #if defined(NPOPT_ENABLE_DUS) */

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Registration Link Release service
     *   Step1: generate LinkReleaseRequest
     *   Step2: parse LinkReleaseResponse
     *          generate LinkReleaseConfirmation
     *
     * @param[in]    in_handle BB handle
     * @param[in] in_bztklen Length of business-token
     * @param[in] in_bztkbuf Buffer of business-token
     * @param[in] in_servernode Server node id
     * @param[in] in_linkfrom Link source
     * @param[in] in_linkto Link destination
     * @param[in] in_response Response message to be parsed
     * @param[out] out_secbuf Step1: buffer for generated request message
     *                            Step2: pointer to secure sdata buffer
     * @param[out] out_secsdata pointer to secure buffer
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NONEMOPERSONALITYREGISTERED   (Nemo Personality is not registered)
     * @return ::ERR_NP_ILLEGALPERSONALITY            (Illegal Octopus or Nemo Personality)
     * @return ::ERR_NP_ASSERTIONNOTFOUND             (One or more DCS Assertion(s) are not registered)
     * @return ::ERR_NP_ILLEGALASSERTION              (Illegal Assertion)
     * @return ::ERR_NP_NOLINKREGISTERED              (Link)
     * @return ::ERR_NP_ILLEGALLINK                   (Illegal Link)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_DeregistrationStep1(hdls_context_t  in_handle,
                             np_size_t       in_bztklen,
                             u_int8_t       *in_bztkbuf,
                             char           *in_servernode,
                             np_str_t       *in_linkid,
                             np_str_t       *in_linkfrom,
                             np_str_t       *in_linkto,
                             np_secbuffer_t *out_secbuf,
                             np_secbuffer_t *out_secsdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
     * @return ::ERR_NP_SOAPFAULT             (response is a soap-fault)
     * @return ::ERR_NP_NOLINKREGISTERED      (Link)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_DeregistrationStep2(hdls_context_t  in_handle,
                             np_str_t       *in_response,
                             np_secbuffer_t *out_secbuf_cnf,
                             np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get the Infomation about Soap-Fault
     *   If ::ERR_NP_SOAPFAULT is returned by npbb APIs,
     *   Get a infomation about Soap-fault
     *
     * @param[in]    in_handle BB handle
     * @param[out] out_info infomation about soap-fault
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NOSOAPFAULT           (No Soap-fault information available)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_GetSoapFault(hdls_context_t      in_handle,
                      nputil_faultinfo_t *out_info);

    /*---------------------------------------------------------------------------*/
    /*
     * Set encryption and signing certificates in the BB handle
     *
     * @param in_handle [in, out] NPBB handle
     * @param[in] in_enccertsetlen length of certset
     * @param[in] in_enccertsetbuf buffer of certset
     * @param[in] in_signcertsetlen length of certset
     * @param[in] in_signcertsetbuf buffer of certset
     * @param[in] in_roleastlen length of service role assertion
     * @param[in] in_roleastbuf buffer of service role assertion
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALCERTSET
     * @return ::ERR_NP_ILLEGALASSERTION
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_SetCertSet(hdls_context_t  in_handle,
                    np_size_t       in_enccertsetlen,
                    u_int8_t       *in_enccertsetbuf,
                    np_size_t       in_signcertsetlen,
                    u_int8_t       *in_signcertsetbuf,
                    np_size_t       in_roleastlen,
                    u_int8_t       *in_roleastbuf);


    /*---------------------------------------------------------------------------*/
    /*
     * Set service information for link registration
     *
     * @param[in, out] in_handle  NPBB handle
     * @param[in] in_svcid        the service id
     * @param[in] in_sinfo        the service info
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     */
    /*---------------------------------------------------------------------------*/
    np_ret_t
    npbb_SetSid(hdls_context_t       in_hdls,
                binstr_t            *in_svcid,
                binstr_t            *in_sinfo);

    /**
     * Set parameters to be used for device attestation
     *
     * @param in_hdls          The BB context to update with capability assertion parameters
     * @param in_manufacturer  Manufacturer value
     * @param in_model         Model value
     * @param in_app_id        Optional application identification value
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     */
    extern np_ret_t
    npbb_SetAttestation(hdls_context_t  in_hdls,
                        binstr_t       *in_manufacturer,
                        binstr_t       *in_model,
                        binstr_t       *in_app_id);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Compose a General Nemo Message
     *
     * @param[in]    in_handle BB handle
     * @param[in] step_type protocol step_type
     * @param[in] io_data body message to be wrapped into the Nemo message
     * @param[in] in_dest_node destination node
     * @param[out] out_secbuf buffer for generated request/response message
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_ltpNemoCompose(hdls_context_t    in_handle,
                        int32_t           step_type,
                        xml_node_t      **io_data,
                        np_str_t         *in_dest_node,
                        np_secbuffer_t   *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Decompose a General Nemo Message
     *
     * @param[in]    in_handle BB handle
     * @param[in] step_type protocol step_type
     * @param[in] msgId Message Id
     * @param[in] in_message request/response message to be parsed
     * @param[out] out_xml_node Body informations extracted  from the message
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    npbb_ltpNemoDecompose(hdls_context_t   in_handle,
                          int32_t          step_type,
                          char            *msgId,
                          np_str_t        *in_message,
                          xml_node_t     **out_xml_node);

#ifdef __cplusplus
}
#endif

#endif /* NPBB_API_H */
/** @} */
