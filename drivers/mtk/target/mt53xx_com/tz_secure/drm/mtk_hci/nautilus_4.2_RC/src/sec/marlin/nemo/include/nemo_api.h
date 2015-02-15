/** @addtogroup nemo nemo
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef NEMO_API_H
#define NEMO_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*===========================================================================*/
    /** @file
     * @brief Nemo library
     *
     * This library is used for composing/decomposing Nemo messages and
     * parsing/retrieving of faults
     *
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * error code definitions
     *---------------------------------------------------------------------------*/

    /** Nemo error codes */
#define ERR_NEMO_ILLEGALXML                  (ERR_NEMO_BASE|1)
#define ERR_NEMO_CRYPTOERROR                 (ERR_NEMO_BASE|2)
#define ERR_NEMO_NEEDTTUPDATE                (ERR_NEMO_BASE|3)
#define ERR_NEMO_NEEDSLUPDATE                (ERR_NEMO_BASE|4)
#define ERR_NEMO_NEEDUPDATE                  (ERR_NEMO_BASE|5)
#define ERR_NEMO_ILLEGALASSERTION            (ERR_NEMO_BASE|6)
#define ERR_NEMO_SOAPFAULT                   (ERR_NEMO_BASE|7)
#define ERR_NEMO_NOSOAPFAULT                 (ERR_NEMO_BASE|8)
#define ERR_NEMO_ILLEGALCERT                 (ERR_NEMO_BASE|9)
#define ERR_NEMO_NOEXISTCERTSET              (ERR_NEMO_BASE|10)
#define ERR_NEMO_ILLEGALARGS                 (ERR_NEMO_BASE|11)
#define ERR_NEMO_SECURITYPOLICYNOTSUPPORTED  (ERR_NEMO_BASE|12)
#define ERR_NEMO_SECURITYPOLICYNOTAPPLIED    (ERR_NEMO_BASE|13)
#define ERR_NEMO_WRONGPROTOCOLSTEPTYPE       (ERR_NEMO_BASE|14)
#define ERR_NEMO_WRONGCORRELATION            (ERR_NEMO_BASE|15)
#define ERR_NEMO_WSAPOLICYNOTAPPLIED         (ERR_NEMO_BASE|16)
#define ERR_NEMO_ILLEGALSIGNATURE            (ERR_NEMO_BASE|17)
#define ERR_NEMO_MESSAGEEXPIRED              (ERR_NEMO_BASE|18)
#define ERR_NEMO_UNSUPPORTEDSECUREPROTOCOL   (ERR_NEMO_BASE|19)
#define ERR_NEMO_INVALIDSECURITY             (ERR_NEMO_BASE|20)
#define ERR_NEMO_INVALIDSECURITYTOKEN        (ERR_NEMO_BASE|21)
#define ERR_NEMO_TTNOTAVAILABLE              (ERR_NEMO_BASE|22)
#define ERR_NEMO_REVOKEDCERT                 (ERR_NEMO_BASE|23)

    /*---------------------------------------------------------------------------*
     * macro
     *---------------------------------------------------------------------------*/

    /** Protocol step types as integers */
#define NEMO_PROTOCOL_STEPTYPE_ILLEGAL        (0x0000)
#define NEMO_PROTOCOL_STEPTYPE_REQUEST        (0x0001)
#define NEMO_PROTOCOL_STEPTYPE_RESPONSE       (0x0002)
#define NEMO_PROTOCOL_STEPTYPE_CONFIRMATION   (0x0004)
#define NEMO_PROTOCOL_STEPTYPE_FAULT          (0x0008)  /* 8 */
#define NEMO_PROTOCOL_STEPTYPE_MAX            (0x0008)  /* this is only for error check */

    /** Protocol step types as strings */
#define NEMO_PROTOCOL_STEPTYPE_REQUEST_STR       "request"
#define NEMO_PROTOCOL_STEPTYPE_RESPONSE_STR      "response"
#define NEMO_PROTOCOL_STEPTYPE_CONFIRMATION_STR  "confirmation"
#define NEMO_PROTOCOL_STEPTYPE_FAULT_STR         "fault"

    /** Security policy for Other protocols */
#define NEMO_SECURITYPOLICY_PROXIMITYCHECK                           (0)
#define NEMO_SECURITYPOLICY_DRMCLIENTINFORMATION_GETOCTOPUSNODE      (1)
#define NEMO_SECURITYPOLICY_DRMCLIENTINFORMATION_GETSECURITYMETADATA (2)
#define NEMO_SECURITYPOLICY_DRMCLIENTINFORMATION_GETDOMAINLINKS      (3)
#define NEMO_SECURITYPOLICY_PROVIDEDOMAININFORMATION                 (4)
#define NEMO_SECURITYPOLICY_PROVIDEDRMOBJECTS_OBTAINOBJECTS          (5)
#define NEMO_SECURITYPOLICY_SECURITYMETADATA_OBTAINDATA              (6)
#define NEMO_SECURITYPOLICY_LICENSETRANSFER                          (7)

    /** Security policy for BroadBand protocols */
#define NEMO_SECURITYPOLICY_LICENSESERVICE                           (8)
#define NEMO_SECURITYPOLICY_DATAUPDATESERVICE                        (9)
#define NEMO_SECURITYPOLICY_DATACERTIFICATESERVICE                   (10)
#define NEMO_SECURITYPOLICY_NODEACQ                                  (11)
#define NEMO_SECURITYPOLICY_LINKACQ                                  (12)
#define NEMO_SECURITYPOLICY_DEREGISTRATION                           (13)
#define NEMO_SECURITYPOLICY_OCTOPUSPERSONALIZATION                   (14)

#define NEMO_SECURITYPOLICY_MAX NEMO_SECURITYPOLICY_OCTOPUSPERSONALIZATION

#define NEMO_WSA_ELEMENT_MAX_SIZE (16)

    /** WS-Addressing field types */
#define NEMO_WSA_NOELEMENTSET           (0x0)
#define NEMO_WSA_TYPE_TO                (1<<0)
#define NEMO_WSA_TYPE_ACTION            (1<<1)
#define NEMO_WSA_TYPE_RELATESTOORIGIN   (1<<2)
#define NEMO_WSA_TYPE_RELATESTOREPLY    (1<<3)
#define NEMO_WSA_TYPE_MESSAGEID         (1<<4)
#define NEMO_WSA_TYPE_FROM              (1<<5)
#define NEMO_WSA_TYPE_REPLYTO           (1<<6)
#define NEMO_WSA_TYPE_FAULTTO           (1<<7)
#define NEMO_WSA_TYPE_REQMESSAGEID      (1<<8)
#define NEMO_WSA_TYPE_RESPMESSAGEID     (1<<9)

    /** NEMO fault codes */
#define NEMO_FAULTCODE_SOAP_SERVER                       "soap:Server"
#define NEMO_FAULTCODE_SOAP_CLIENT                       "soap:Client"
#define NEMO_FAULTCODE_WSSE_INVALIDSECURITY              "wsse:InvalidSecurity"
#define NEMO_FAULTCODE_WSSE_INVALIDSECURITYTOKEN         "wsse:InvalidSecurityToken"
#define NEMO_FAULTCODE_WSSE_UNSUPPORTEDSECURITYTOKEN     "wsse:UnsupportedSecurityToken"
#define NEMO_FAULTCODE_WSSE_FAILEDCHECK                  "wsse:FailedCheck"
#define NEMO_FAULTCODE_WSSE_FAILEDAUTHENTICATION         "wsse:FailedAuthentication"
#define NEMO_FAULTCODE_WSU_MESSAGEEXPIRED                "wsu:MessageExpired"
#define NEMO_FAULTCODE_NEMOSEC_UNSUPPORTEDSECUREPROTOCOL "nemosec:UnsupportedSecureProtocol"

    /** NEMO fault strings */
#define NEMO_FAULTSTR_SOAP_CLIENT                        "Error within message payload"
#define NEMO_FAULTSTR_SOAP_SERVER                        "Error at the server side"
#define NEMO_FAULTSTR_WSSE_INVALIDSECURITY               "Incorrect security used"
#define NEMO_FAULTSTR_WSSE_INVALIDSECURITYTOKEN          "Incorrect ToNode"
#define NEMO_FAULTSTR_WSSE_UNSUPPORTEDSECURITYTOKEN      "Unrecognized protocol step, sequence no, or message element"
#define NEMO_FAULTSTR_WSSE_FAILEDCHECK                   "Signature validation failed"
#define NEMO_FAULTSTR_WSSE_FAILEDAUTHENTICATION          "Signing or Encryption key authentication failed"
#define NEMO_FAULTSTR_WSU_MESSAGEEXPIRED                 "Expired message, according to receiver's timestamp policy"
#define NEMO_FAULTSTR_NEMOSEC_UNSUPPORTEDSECUREPROTOCOL  "Unrecognized protocol"

    /* NEMO fault detail/name */
#define NEMO_FAULTNAME_DEFAULT                           "exc:NemoMessageProcessingException"

    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

    typedef void *nemo_handle_t;

    /** Fault details structure */
    typedef struct {
        binstr_t *name;             /**< Fault name                 */
        binstr_t *redirectURL;      /**< Redirection URL            */
        binstr_t *details;          /**< Detailed fault description */
    } nemo_faultdetail_t;

    /** Fault info structure */
    typedef struct {
        binstr_t *code;             /**< Fault code    */
        binstr_t *str;              /**< Fault string  */
        binstr_t *actor;            /**< Fault action  */
        nemo_faultdetail_t **detail; /**< Fault details */
        u_int32_t detail_count;      /** nr of Fault details */
    } nemo_faultinfo_t;

    /** Key certificate structure */
    typedef struct {
        binstr_t *body;             /**< Key certificate         */
        binstr_t *ski;              /**< Subject Key Indentifier */
        binstr_t *n;                /**< RSA Modulus             */
        binstr_t *e;                /**< Public RSA Exponent     */
    } nemo_cert_t;

    /**
     * This structure contains values of WSA message information headers,
     * which are used in message correlation.
     *
     * @specref Web Services Addressing (WS-Addressing),
     *          NEMO Message Bindings 1.0 section 3.
     */
    typedef struct {
        u_int32_t num;               /**< The number of set headers */
        u_int32_t flags;             /**< NEMO_WSA_TYPE_* flags showing which headers
                                      are set */
        binstr_t *to;                /**< wsa:To header value */
        binstr_t *action;            /**< wsa:Action header value */
        binstr_t *relatesto_origin;  /**< wsa:RelatesTo header value (originatesFrom relationship type) */
        binstr_t *relatesto_reply;   /**< wsa:RelatesTo header value (reply relationship type) */
        binstr_t *messageid;         /**< wsa:MessageID header value */
        binstr_t *from;              /**< wsa:From header value */
        binstr_t *replyto;           /**< wsa:ReplyTo header value */
        binstr_t *faultto;
        binstr_t *req_messageid;     /**< ID issued by the requestor node */
        binstr_t *resp_messageid;    /**< ID issued by the response node (request-responsey relationship type) */

    } nemo_wsadata_t;

    /**
     * This structure contains data which is used when applying the
     * WS-Security (WSSE) bindings.
     *
     * @specref WS-Security bindings
     *          NEMO Security Bindings 1.0.1 section 1.
     */
    typedef struct {
        xml_node_t *xmlbody;            /**> Composed nemo message               */
        xml2oct_nemonode_t *personality;/**> Nemo personality                    */
        u_int8_t  num_assertions;       /**> Number of own assertions            */
        xml_node_t **assertions;        /**> Own assertions                      */
        xml_node_vec_t *peerassertions; /**> Assertions of the peer              */
        binstr_t *nonce;                /**> Own nonce                           */
        binstr_t *peernonce;            /**> Nonce of the peer                   */
        binstr_t *respid;               /**> Response ID                         */
        u_int8_t enckey[16];            /**> Encryption key                      */
        nemo_cert_t *cert[2];           /**> Sign and encryption certificates    */
        nemo_faultinfo_t *faultinfo;    /**> Fault information                   */
        nemo_wsadata_t wsadata;         /**> WS-Addressing data                  */
        u_int32_t msglifeperiod;        /**> Max message life period (in seconds).
                                         "0" means consider timestamp is only
                                         informative                         */
        time_unix_t timestamp;          /**> Timestamp (extracted from response
                                         message)                            */
        int32_t  elem_digestalgo;       /**> Elements digest algorithm,
                                         i.e. RSA_DIGESTALGO_SHA1 or
                                              RSA_DIGESTALGO_SHA256          */
        int32_t  sign_digestalgo;       /**> Signature digest algorithm,
                                         i.e. RSA_DIGESTALGO_SHA1 or
                                              RSA_DIGESTALGO_SHA256          */
#if defined(NPOPT_ENABLE_DECE)
        binstr_t *manufacturer;         /**< manufacturer for Device Attestation*/
        binstr_t *model;                /**< model for Device Attestation*/
        binstr_t *app_id;               /**< application identification for Device Attestation. Can be NULL.*/
#endif
    } nemo_wssedata_t;


    /*---------------------------------------------------------------------------*
     * APIs
     *---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------*
     * PROTOCOLs
     *---------------------------------------------------------------------------*/

    /**
     * @brief Set the encryption and signing certificate in the wssedata structure
     *
     * @param[in,out] io_wssedata   WSSE data structure
     * @param[in]     in_enccert    Encryption certificate
     * @param[in]     in_signcert   Signing certificate
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_NEMO_ILLEGALCERT
     * @return ::ERR_INTERNALERROR
     *
     */
    extern retcode_t
    nemo_SetCert(nemo_wssedata_t *io_wssedata, binstr_t *in_enccert, binstr_t *in_signcert);

    /**
     * @brief Free all the elements of the given WSA structure, except for
     * io_wsadata->req_messageid and io_wsadata->resp_messageid
     *
     * @param[in,out] io_wsadata         WSA data structure
     *
     * @return ::ERR_OK
     */
    extern retcode_t
    nemo_FreePartlyWsadata(nemo_wsadata_t *io_wsadata);

    /**
     * @brief Free all the elements of the given WSA structure
     *
     * @param[in,out] io_wsadata         WSA data structure
     *
     * @return ::ERR_OK
     */
    extern retcode_t
    nemo_FreeWholeWsadata(nemo_wsadata_t *io_wsadata);

    /*
     * @brief Reset the elements of a given WSA structure
     *
     * @note The function assumes that io_wsadata is not NULL.
     *
     * @param[in,out]  io_wsadata  WSA data structure
     *
     * @return ::ERR_OK
     */
    extern retcode_t
    nemo_ResetWsadata(nemo_wsadata_t *io_wsadata);

    /**
     * @brief Free a fault information structure
     *
     * @note The structure is deallocated but not set to NULL
     *
     * @param[in,out] io_info   Fault info structure
     *
     * @return ::ERR_OK
     */
    extern retcode_t
    nemo_FreeSoapFault(nemo_faultinfo_t *io_info);

    /**
     * @brief Free a nemo_wssedata_t structure
     *
     * @note The structure itself is not deallocated
     *
     * @param[in,out] io_wssedata   WSSE data structure
     *
     * @return ::ERR_OK
     */
    extern retcode_t
    nemo_FreeWssedata(nemo_wssedata_t *io_wssedata);

    /**
     * @brief Retrieve a pointer to the latest SOAP fault information
     *
     * @note The structure is not copied! It should not be deallocated.
     *
     * @param[in,out]  io_wssedata   WSSE data
     * @param[out]     out_info      Fault information
     *
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NEMO_NOSOAPFAULT
     */
    extern retcode_t
    nemo_GetSoapFault(nemo_wssedata_t *io_wssedata, nemo_faultinfo_t **out_info);


    /*---------------------------------------------------------------------------*
     * SECURITY POLICY
     *---------------------------------------------------------------------------*/


    /**
     * @brief Given WSSE data and an associated Marlin security policy,
     * and protocol step type, compose a SOAP message. If the security
     * policy requires it, a nonce is generated and set into
     * io_data->nonce.
     *
     * @param[in] io_data            wsse data to wrap
     * @param[in] in_secpolicy       security policy
     *                               (e.g., NEMO_SECURITYPOLICY_PROVIDEDOMAININFORMATION)
     * @param[in] in_prot_step_type  protocol step type
     *                               (e.g., NEMO_PROTOCOL_STEPTYPE_REQUEST)
     * @param[in] out_str            SOAP message
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_NEMO_ILLEGALARGS
     * @return ::ERR_NEMO_CRYPTOERROR
     * @return ::ERR_NEMO_ILLEGALCERT
     * @return ::ERR_NEMO_WRONGPROTOCOLSTEPTYPE
     */
    extern retcode_t
    nemo_WSSE_compose_by_policy(nemo_wssedata_t *io_data,
                                u_int8_t in_secpolicy,
                                u_int8_t in_prot_step_type, binstr_t **out_str);

    /**
     * @brief Given an xml buffer containing a SOAP message, a wsse
     * context, the expected security policy, and protocol step type,
     * check the associated security policy and extract the body.  If
     * this is a fault message, ::ERR_NEMO_SOAPFAULT is returned and the
     * fault details are set to io_data. The wsse context io_data is
     * also set, if applicable, with peer's encryption and signing
     * certificates as well as, peer's nonce, wsadata, respid, and
     * peer's assertions.
     *
     * @note If the signature of the message is going to be verified, the
     * appropriate key purpose corresponding to the peer's signing
     * certificate must be provided.
     *
     * @param[in] in_xmllen          xml buf len
     * @param[in] in_xmlbuf          xml buf
     * @param[in,out] io_data        wsse context
     * @param[in] in_bodyname        expected body name
     * @param[in] in_bodyns          expected body name space
     * @param[in] in_secpolicy       expected security policy
     *                               (e.g., NEMO_SECURITYPOLICY_PROVIDEDOMAININFORMATION)
     * @param[in] in_prot_step_type  expected protocol step type
     *                               (e.g., NEMO_PROTOCOL_STEPTYPE_REQUEST)
     * @param[in] in_signkeypurpose  key purpose of the peer's certificate
     *                               used to verify the message signature, e.g.,
     *                               CERT_MARLIN_KP_SIGN_NEMOPROTOCOLDRMCLIENTDATA
     * @param[in] in_enckeypurpose   key purpose used for checking the peer's encryprion
     *                               certificate, e.g.,
     *                               CERT_MARLIN_KP_ENC_NEMOPROTOCOLDRMCLIENTDATA
     * @param[out] out_node          message body
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_NEMO_ILLEGALARGS
     * @return ::ERR_NEMO_ILLEGALXML
     * @return ::ERR_NEMO_WRONGPROTOCOLSTEPTYPE
     * @return ::ERR_NEMO_ILLEGALSIGNATURE
     * @return ::ERR_NEMO_SECURITYPOLICYNOTSUPPORTED
     * @return ::ERR_NEMO_SECURITYPOLICYNOTAPPLIED
     * @return ::ERR_NEMO_WSAPOLICYNOTAPPLIED
     * @return ::ERR_NEMO_WRONGCORRELATION
     * @return ::ERR_NEMO_SOAPFAULT
     * @return ::ERR_NEMO_ILLEGALCERT
     * @return ::ERR_NEMO_CRYPTOERROR
     * @return ::ERR_NEMO_INVALIDSECURITY
     * @return ::ERR_NEMO_INVALIDSECURITYTOKEN
     */
    extern retcode_t
    nemo_WSSE_decompose_by_policy(np_size_t in_xmllen, u_int8_t *in_xmlbuf,
                                  nemo_wssedata_t *io_data,
                                  char *in_bodyname, char *in_bodyns,
                                  u_int8_t in_secpolicy,
                                  u_int8_t in_prot_step_type,
                                  int32_t in_signkeypurpose,
                                  int32_t in_enckeypurpose, xml_node_t **out_node);

    /*---------------------------------------------------------------------------*
     * Others
     *---------------------------------------------------------------------------*/

    /**
     * Extracts the timestamp from the current nemo protocol data and sets it to ::NEMO_L_TIMESTAMP_DISABLED
     *
     * @note the timestamp which is '00:00:00 on January 1, 1970' is not supported
     *
     * @param[in,out] io_handle handle to nemo protocol containing timestamp to get and to set to ::NEMO_L_TIMESTAMP_DISABLED
     * @param[out]    out_timestamp timestamp in ::time_unix_t format
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NEMO_TTNOTAVAILABLE
     */
    extern retcode_t nemo_GetResponseTimestamp(nemo_wssedata_t *io_wssedata,
                                               time_unix_t *out_timestamp);


    /**
     * This function generates a message ID, which could be used in WSA message
     * information headers. Generated ID has the following format:
     * personality_idXXXXXXXX, where
     * personality_id - a string specified by the caller
     * X - a hex number between 0 and 255 (values from 0 to 0xf have leading 0)
     *
     * @param[in] personality_id A binary string representing personality ID.
     * @param[out] messageid     A pointer to a buffer which will hold a pointer
     *                           to a generated message ID upon return.
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    nemo_GenMessageID(binstr_t   *personality_id,
                      binstr_t **messageid);


    /**
     * This function sets one of the following WSA message information headers
     * in a given data structure: "MessageID",
     * "RelatesTo RelationshipType=&nemo;/originatesFrom", "Action"
     * and request message ID (which is not a WSA message information header).
     *
     * @param[in,out] io_wsadata A pointer to WSA structure where a given
     *                           value will be saved upon return.
     * @param[in] in_type        A type of the information header which value
     *                           is saved.?Must be NEMO_WSA_TYPE_MESSAGEID or
     *                           NEMO_WSA_TYPE_REQMESSAGEID or
     *                           NEMO_WSA_TYPE_RELATESTOORIGIN or
     *                           NEMO_WSA_TYPE_ACTION.
     * @param[in] in_value       A pointer to a binsary string containing the
     *                           value. The value will be copyed.
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    nemo_SetWSAField(nemo_wsadata_t *io_wsadata,
                     int32_t          in_type,
                     binstr_t        *in_value);

    /**
     * Set parameters to be used in the nemo capability assertion
     *
     * @param io_wssedata     Nemo context to update with capability assertion parameters
     * @param in_manufacturer Manufacturer value to put in the nemo context
     * @param in_model        Model value to put in the nemo context
     * @param in_app_id       Optional application identification to put in the nemo context
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    nemo_SetAttestation(nemo_wssedata_t *io_wssedata,
                        binstr_t        *in_manufacturer,
                        binstr_t        *in_model,
                        binstr_t        *in_app_id);
    /**
     * This function builds the following XML node:
     * <BusinessToken>Business token data</BusinessToken> Such XML node is
     * used in the Octopus Personalization, NEMO registartion and License
     * service protocols.
     *
     * @specref Octopus Personalization Protocol 1.0 section 3.1
     *          Marlin Broadband Delivery System Specification 1.2 section 5.2
     *
     * @param[in] in_bzt    Business token data.
     * @param[in] in_nstag  A prefix of the namespace.
     * @param[in] in_nsuri  Namespace, which will be used to construct
     *                      BusinessToken XML element.
     * @param[out] out_node A pointer to a buffer which will hold a pointer to
     *                      a generated XML node.
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_XML_*
     */
    extern retcode_t
    nemo_GenBusinessToken(binstr_t *in_bzt,
                          u_int8_t *in_nstag,
                          u_int8_t *in_nsuri,
                          xml_node_t **out_node);

    /**
     * Generates SOAP message string from headers and body xml nodes
     * @verbatim
     * <senv:Envelope xmlns:senv=::NS_SOAP xmlns:xenc=::NS_XENC>
     *   <senv:Header>
     *     ::in_head_nodes
     *   </senv:Header>
     *   <senv:Body wsu:Id=::L_SOAPBODY_ID xmlns:wsu=::L_NS_WSU xmlns:senv=::NS_SOAP>
     *     ::in_body
     *   </senv:Body>
     * </senv:Envelope>
     * @endverbatim
     *
     * @param[in]  in_head_nodes       Array of SOAP headers
     * @param[in]  in_head_nodes_count Number of SOAP headers
     * @param[in]  in_body             SOAP body
     * @param[out] out_str             Resulting string containing the xml document
     *
     * @return ::ERR_NEMO_ILLEGALARGS
     * @return ::xml_ComposeElement
     * @return ::nemo_l_ComposeSOAPBody
     * @return ::xml_OutputComposedNode
     * @return ::nemo_l_ConvErr
     */
    extern retcode_t
    nemo_ComposeSOAP(xml_node_t **in_head_nodes, u_int8_t in_head_nodes_count,
                     xml_node_t *in_body, binstr_t **out_str);



#ifdef __cplusplus
}
#endif

#endif                          /* NEMO_API_H */
/** @} */
