/** @addtogroup nssmi nssmi
 *  @ingroup nsec
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef NSSMI_API_H
#define NSSMI_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Set the secure data. Used on initialization
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_sdata            Serialized secure data
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_SetSdata
 */
extern np_ret_t
nssmi_SetSdata(np_handle_t in_handle, binstr_t *in_sdata);

/**
 * @brief Check the specified personality. See @ref np_pers_t
 *
 * @param[in]       in_handle       @ref example_smi "SMI" target info
 * @param[in,out]   io_type         The personality types to check (OR-able bits).
 *
 * @return ::ERR_NP_OK              Success, then io_type should be zero indicating all keys are valid.
 * @return ::ERR_NP_NOPERSONALITYREGISTERED If at least one of the personality file is not valid or not registered (check io_type then!).
 * @return ::ERR_NP_OUTOFMEMORY     Unable to allocate memory
 * @return ::ERR_NP_ILLEGALHANDLE   Handle is invalid
 * @return ::ERR_NP_ILLEGALARGS     io_type == NULL.
 * @return ::ERR_NP_INTERNALERROR   Undefined error, io_type is undefined.
 * @return ::ERR_NP_SDATACORRUPTED  Smem corrupted.
 */
extern np_ret_t
nssmi_StatPersonality(np_handle_t   in_handle,
                      u_int32_t     *io_type);

/**
 * @brief Checks if a personality (NEMO, Octopus public or private)
 *        has been already stored
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_type             Personality type to check
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_CheckPersonality
 */
extern np_ret_t
nssmi_CheckPersonality(np_handle_t in_handle, int32_t in_type);

/**
 * @brief Checks if a DCS assertion (given by its standard name) has
 *        been stored and has not expired
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_dcsnlen          Length of DCS assertion name
 * @param[in] in_dcsnbuf          DCS assertion name (case sensitive)
 * @param[in] in_validity         Validity period of the DCS assertion (or NULL for
 *                                skipping validity check). The validity period is a
 *                                duration expressed in xsd:duration format
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_CheckDCSAssertion
 */
extern np_ret_t
nssmi_CheckDCSAssertion(np_handle_t  in_handle,
                        np_size_t    in_dcsnlen,
                        u_int8_t    *in_dcsnbuf,
                        binstr_t    *in_validity);

/*---------------------------------------------------------------------------*/
/**
 * @brief Get Node ID
 *
 * @param[in]  in_handle      @ref example_smi "SMI" target info
 * @param[in]  in_node        Buffer of node(XML format)
 * @param[out] out_nodeid     Buffer where the NodeID will be stored
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALNODE        <br>Illegal node
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_GetNodeID
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_GetNodeID(np_handle_t in_handle, binstr_t *in_node, binstr_t **out_nodeid);

/**
 * @brief Check if CRL is up-to-date
 *
 * This function checks if the current CRL is up-to-date or not. If it is,
 * nothing happens and the function quits. Otherwise it tries to download
 * a new CRL using a given function and a given URL.
 *
 * @note if there is no CRL set, it first of all tries to load it from
 *       the Nautilus' store and check if the loaded CRL is valid or not.
 * @note if an error occurs while retrieving a CRL from a distribution point
 *       the old CRL is not removed from the system even if it is invalid.
 *
 * @param[in] in_handle  @ref example_smi "SMI" target info
 * @param[in] in_get A pointer to a function which will fetches a CRL from
 *                   a given URL. The prototype for this function is
 *                   <br><br><tt>(void *, const char*, u_int8_t**, np_size_t*)</tt>
 *                   <br><br>The function parameters are as follows:
 *                   <ol>
 *                   <li> Void pointer to an optional structure, implementation dependent
 *                   <li> A pointer to a URL where a new CRL has to be
 *                      retrieved.
 *                   <li> A pointer to a buffer which will hold a pointer
 *                      to a buffer containing fetched CRL.
 *                   <li> A pointer to a buffer which will hold size of the
 *                      fetched CRL.
 *                   </ol>
 *                   This function should return 0 in case of success and
 *                   ~0 otherwise.
 * @param[in] in_free A pointer to a deallocation function that frees memory allocated
 *                    in the <i>in_get</i> function.
 *                   The function parameters are as follows:
 *                   <ol>
 *                   <li> Void pointer to an optional structure, implementation dependent
 *                   <li> A pointer to the buffer filled by the call to <i>in_get</i>
 *                   </ol>
 * @param[in] in_url A URL which should be used in case a new CRL has to be
 *                   retrieved.
 * @param[in] in_ptr Pointer to the first parameter for the function in_get
 *
 * @return ::ERR_NP_OK                  <br>Indicating success
 * @return ::ERR_NP_INTERNALERROR       <br>An unrecoverable error has occurred in Nautilus.
 * @return ::ERR_NP_ILLEGALHANDLE       <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED   <br>SMI communication failed.
 */
extern np_ret_t
nssmi_CheckCRL(np_handle_t    in_handle,
               int32_t      (*in_get) (void *, const char *, u_int8_t **, np_size_t *),
               int32_t      (*in_free)(void *, u_int8_t *),
               const char    *in_url,
               void          *in_ptr);

/**
 * This function gets the CRL from sdata
 *
 * @param[in]  in_handle  @ref example_smi "SMI" target info
 * @param[out] out_crl    Output CRL got from sdata.
 *
 * @return ::ERR_NP_OK                  <br>Indicating success.
 * @return ::ERR_NP_INTERNALERROR       <br>An unrecoverable error has occurred in Nautilus.
 * @return ::ERR_NP_ILLEGALHANDLE       <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED   <br>SMI communication failed.
 * @return ::np_GetCRL
 */
extern np_ret_t
nssmi_GetCRL(np_handle_t in_handle, binstr_t **out_crl);

/**
 * This function sets a new CRL in the system.
 * This function parses the CRL it, saving it in
 * the Nautilus registry and setting it as the current CRL.
 * @note The CRL must be in DER format.
 *
 * @param[in] in_handle  @ref example_smi "SMI" target info
 * @param[in] in_crl     A pointer to a string type containing a CRL.
 *
 * @return ::ERR_NP_OK                  <br>Indicating success.
 * @return ::ERR_NP_INTERNALERROR       <br>An unrecoverable error has occurred in Nautilus.
 * @return ::ERR_NP_ILLEGALHANDLE       <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED   <br>SMI communication failed.
 * @return ::np_UpdateCRL
 */
extern np_ret_t
nssmi_UpdateCRL(np_handle_t in_handle, binstr_t *in_crl);

/**
 * This function gets a CRL distribution point from a certificate chain,
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_certchain_b64enc A pointer to a buffer containing a certchain
 *                                encoded as base64.
 * @param[out] out_cr_dp          A pointer to a string containing the distribution point URL.
 *
 * @return ::ERR_NP_OK                  <br>Indicating success
 * @return ::ERR_NP_INTERNALERROR       <br>An unrecoverable error has occurred in Nautilus.
 * @return ::ERR_NP_ILLEGALARGS         <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NP_ILLEGALCERTSET      <br>The certificate chain failed validation.
 * @return ::ERR_NP_OUTOFMEMORY         <br>A resource failure has occurred.
 * @return ::ERR_NP_ILLEGALHANDLE       <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED   <br>SMI communication failed.
 */
extern np_ret_t
nssmi_GetCRLDistributionPoint(np_handle_t  in_handle,
                              binstr_t    *in_certchain_b64enc,
                              binstr_t   **out_cr_dp);

/**
 * @brief Initializes a session of acquisition for a domain ID
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_account          Domain ID
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_InitAccountSession
 */
extern np_ret_t
nssmi_InitAccountSession(np_handle_t in_handle, binstr_t *in_account);

/**
 * @brief Finalizes a domain session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_FinAccountSession
 */
extern np_ret_t
nssmi_FinAccountSession(np_handle_t in_handle);

/**
 * @brief Checks integrity of the items available for a given domain ID
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_account          Domain ID
 * @param[out] out_status          Status flags
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_CheckSdataStatus
 */
extern np_ret_t
nssmi_CheckSdataStatus(np_handle_t in_handle, binstr_t *in_accountid, u_int32_t *out_status);

/**
 * @briefClears nodes and links (for a given domain ID or for all domain IDs)
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_account          Domain ID
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ClearRegistrationStatus
 */
extern np_ret_t
nssmi_ClearRegistrationStatus(np_handle_t in_handle, binstr_t *in_accountid);

/**
 * @brief Removes corrupted items from the persistent store (for a given domain ID)
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_account          Domain ID
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_RecoverSdata
 */
extern np_ret_t
nssmi_RecoverSdata(np_handle_t in_handle, binstr_t *in_accountid);

/**
 * @brief Gets device-specific information, for use e.g. in the request for an action token acquisition.
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_cinfo            Context Information
 * @param[out] out_buf             Placeholder for retrieved information
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_GetDUIDParameters
 */
extern np_ret_t
nssmi_GetDUIDParameters(np_handle_t in_handle, u_int32_t in_cinfo, binstr_t **out_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Get the trusted time
 *
 * @param[in] in_handle     Component handle and SMI target info
 * @param[out] out_time     Trusted time
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOMEM
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::npcommon_GetTrustedTime
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_GetTrustedTime(np_handle_t in_handle, np_date_t *out_time);

/*---------------------------------------------------------------------------*/
/**
 * @brief Clear the secure data cache
 *
 * @return ::ERR_NP_OK
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_ClearSdata(np_handle_t in_handle);

/**
 * This function returns a CRL number of the current CRL. If there is no
 * CRL set then np_cert_crlnumber_t.valid will be set to 0.
 *
 * @param[in] in_handle  @ref example_smi "SMI" target info
 *
 * @return A CRL number in a @ref np_cert_crlnumber_t structure
 */
extern np_cert_crlnumber_t
nssmi_GetCRLNumber(np_handle_t in_handle);

/**
 * This function clears the CRL.
 *
 * @param[in]  in_handle  @ref example_smi "SMI" target info
 */
extern void
nssmi_ClearCRL(np_handle_t in_handle);

/**
 * @brief List all liks currently in persistent store
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[out] out_num             Number of links
 * @param[out] out_links           Array of links
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_GetAvailableLinks
 */
np_ret_t
nssmi_GetAvailableLinks(np_handle_t       in_handle,
                        u_int32_t        *out_num,
                        binstr_t       ***out_links);

/**
 * @brief Look up links based on service information
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_svcid            Service ID
 * @param[in]  in_sinfo            Service information
 * @param[in]  in_domainid         Generated Domain ID for links
 * @param[out] out_num             Number of links
 * @param[out] out_links           Array of links
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_GetLinksForServiceID
 */
np_ret_t
nssmi_GetLinksForServiceID(np_handle_t    in_handle,
                           binstr_t      *in_svcid,
                           binstr_t      *in_sinfo,
                           binstr_t      *in_domainid,
                           u_int32_t     *out_num,
                           binstr_t    ***out_links);

/**
 * @brief Creates personalization request
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[out] out_request         Body of an HTTP request to send to personalization server
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ppproc_GenerateRequest
 */
np_ret_t
nssmi_GeneratePersonalizationRequest(np_handle_t in_handle, binstr_t  **out_request);

/**
 * @brief Sets NEMO and Octopus personalities to selected Marlin BB client
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_personality      a personality message
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ppproc_ParseResponse
 */
np_ret_t
nssmi_ProcessPersonalizationResponse(np_handle_t in_handle, binstr_t  *in_personality);

/**
 * @brief Delete a selected link
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_from             From node ID
 * @param[in] in_to               To node ID
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_DeregLinkFromTo
 */
np_ret_t
nssmi_DeleteLink(np_handle_t in_handle, binstr_t *in_from, binstr_t *in_to);

/**
 * @brief This function is used by a device to process a message it received
 *        from a gateway. It will call a non-secure <-> secure transformation
 *        layer that will deliver to smi request to the secure core and return
 *        the response back.
 *        Certain messages, such as decryption and action perform are not allowed
 *        over remote transport layers.
 *
 * @param[in]   in_request_msg      A request message in smi format.
 * @param[out]  out_response_msg    A response message in smi format.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 */
extern np_ret_t
nssmi_ProcMsg(binstr_t    *in_request_msg,
              binstr_t   **out_response_msg);

/**
 * @brief This function initilizes the smi
 *
 * @return ::ERR_NP_OK
 * @return ::mutex_Init
 */
extern np_ret_t
nssmi_Init(void);

/**
 * @brief This function finalizes the smi
 *
 * @return ::ERR_NP_OK
 * @return ::mutex_Fin
 */
extern np_ret_t
nssmi_Fin(void);

NP_END_EXTERN_C

#endif /* NSSMI_API_H */


/** @} */
