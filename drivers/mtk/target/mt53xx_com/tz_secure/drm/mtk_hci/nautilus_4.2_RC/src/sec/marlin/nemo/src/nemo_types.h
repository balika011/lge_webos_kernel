/** @addtogroup nemo nemo
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef NEMO_TYPES_H
#define NEMO_TYPES_H

#include "ns_defs.h"

/** @file */

/* Public */

#define L_NS_NSU_DCSASSERTION \
    "urn:marlin:broadband:1.0:nemo:services:datacertification-service:assertion"

#define L_NS_ROLE_DEVICE \
    "urn:marlin:core:role:device"

#define L_NS_ET_BASE64   "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary"

#define L_NEMOSEC_URI   NS_NEMOSEC "/secure-protocol/basic/1.0"

/* Sony Connect */
#define L_NS_NEMOPROFILE_URI  "urn:marlin:core:1.0:nemo:protocol:profile:1"

/* nemosec:Usage values */
#define L_NS_NSU_PROTOCOLDECLARATION  NS_NEMOSEC "/secure-protocol"
#define L_NS_NSU_PROFILE              NS_NEMOSEC "/profile"
#define L_NS_NSU_REQ_ENCRYPTEDKEY     L_NEMOSEC_URI "#request-encryptedMessageKey"
#define L_NS_NSU_RESP_ENCRYPTEDKEY    L_NEMOSEC_URI "#response-encryptedMessageKey"
#define L_NS_NSU_CONF_ENCRYPTEDKEY    L_NEMOSEC_URI "#confirmation-encryptedMessageKey"
#define L_NS_NSU_FAULT_ENCRYPTEDKEY   L_NEMOSEC_URI "#fault-encryptedMessageKey"
#define L_NS_NSU_REQ_SIGNATURE        L_NEMOSEC_URI "#request-signature"
#define L_NS_NSU_RESP_SIGNATURE       L_NEMOSEC_URI "#response-signature"
#define L_NS_NSU_CONF_SIGNATURE       L_NEMOSEC_URI "#confirmation-signature"
#define L_NS_NSU_FAULT_SIGNATURE      L_NEMOSEC_URI "#fault-signature"
#define L_NS_NSU_RESP_RETURNEDNONCE   L_NEMOSEC_URI "#response-returnedNonce"
#define L_NS_NSU_CONF_RETURNEDNONCE   L_NEMOSEC_URI "#confirmation-returnedNonce"
#define L_NS_NSU_FAULT_RETURNEDNONCE  L_NEMOSEC_URI "#fault-returnedNonce"
#define L_NS_NSU_REQ_NONCE            L_NEMOSEC_URI "#request-nonce"
#define L_NS_NSU_RESP_NONCE           L_NEMOSEC_URI "#response-nonce"
#define L_NS_NSU_FAULT_NONCE          L_NEMOSEC_URI "#fault-nonce"
#define L_NS_NSU_REQ_TIMESTAMP        L_NEMOSEC_URI "#request-timestamp"
#define L_NS_NSU_RESP_TIMESTAMP       L_NEMOSEC_URI "#response-timestamp"
#define L_NS_NSU_CONF_TIMESTAMP       L_NEMOSEC_URI "#confirmation-timestamp"
#define L_NS_NSU_FAULT_TIMESTAMP      L_NEMOSEC_URI "#fault-timestamp"
#define L_NS_NSU_REQ_TONODE           L_NEMOSEC_URI "#request-toNode"
#define L_NS_NSU_RESP_TONODE          L_NEMOSEC_URI "#response-toNode"
#define L_NS_NSU_CONF_TONODE          L_NEMOSEC_URI "#confirmation-toNode"
#define L_NS_NSU_FAULT_TONODE         L_NEMOSEC_URI "#fault-toNode"
#define L_NS_NSU_REQ_FROMNODE         L_NEMOSEC_URI "#request-fromNode"
#define L_NS_NSU_RESP_FROMNODE        L_NEMOSEC_URI "#response-fromNode"
#define L_NS_NSU_CONF_FROMNODE        L_NEMOSEC_URI "#confirmation-fromNode"
#define L_NS_NSU_FAULT_FROMNODE       L_NEMOSEC_URI "#fault-fromNode"
#define L_NS_NSU_REQ_MSGKEY           L_NEMOSEC_URI "#request-messageKey"
#define L_NS_NSU_RESP_MSGKEY          L_NEMOSEC_URI "#response-messageKey"
#define L_NS_NSU_CONF_MSGKEY          L_NEMOSEC_URI "#confirmation-messageKey"
#define L_NS_NSU_FAULT_MSGKEY         L_NEMOSEC_URI "#fault-messageKey"
#define L_NS_NSU_REQ_SIGNKEY          L_NEMOSEC_URI "#request-signingKey"
#define L_NS_NSU_RESP_SIGNKEY         L_NEMOSEC_URI "#response-signingKey"
#define L_NS_NSU_CONF_SIGNKEY         L_NEMOSEC_URI "#confirmation-signingKey"
#define L_NS_NSU_FAULT_SIGNKEY        L_NEMOSEC_URI "#fault-signingKey"
#define L_NS_NSU_REQ_ENCKEY           L_NEMOSEC_URI "#request-encryptionKey"
#define L_NS_NSU_RESP_ENCKEY          L_NEMOSEC_URI "#response-encryptionKey"
#define L_NS_NSU_CONF_ENCKEY          L_NEMOSEC_URI "#confirmation-encryptionKey"
#define L_NS_NSU_FAULT_ENCKEY         L_NEMOSEC_URI "#fault-encryptionKey"


#define L_NS_NSU_ROLEASSERTION        "http://nemo.intertrust.com/2004/attribute/role"
#define L_NS_NSU_CAPABILITYASSERTION  "urn:marlin:pacs:1.0:profile-capability-attributes:assertion"

/* Value Types */
#define L_NS_VT_SUBJECTKEYIDENTIFIER  "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509v3SubjectKeyIdentifier"
#define L_NS_VT_SAMLKEYIDENTIFIER  "http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.0#SAMLAssertionID"
#define L_NS_VT_SYMBINARYSECTOKEN "http://nemo.intertrust.com/2005/10/security/BST/SymmetricKey"
#define L_NS_VT_X509PKIPATHV1     "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509PKIPathv1"


/* Types */
#define L_NS_TYPE_XMLENC_CONTENT "http://www.w3.org/2001/04/xmlenc#Content"
#define L_NS_TYPE_XMLENC_ELEMENT "http://www.w3.org/2001/04/xmlenc#Element"


/* WSA NS */
#define L_NS_WSA_RELATION_ORIGINATESFROM "http://nemo.intertrust.com/2004/addressing/originatesFrom"
#define L_NS_WSA_RELATION_REPLY          "http://www.w3.org/2005/08/addressing/reply"

/* Fault Codes */
#define L_NEMO_FAULTCODE_VERSIONMISMATCH                "soap:VersionMismatch"
#define L_NEMO_FAULTCODE_MUSTUNDERSTAND                 "soap:MustUnderstand"
#define L_NEMO_FAULTCODE_CLIENT                         "soap:Client"
#define L_NEMO_FAULTCODE_SERVER                         "soap:Server"
#define L_NEMO_FAULTCODE_UNSUPPORTEDSECURITYTOKEN       "wsse:UnsupportedSecurityToken"
#define L_NEMO_FAULTCODE_UNSUPPORTEDALGORITHM           "wsse:UnsupportedAlgorithm"
#define L_NEMO_FAULTCODE_INVALIDSECURITY                "wsse:InvalidSecurity"
#define L_NEMO_FAULTCODE_INVALIDSECURITYTOKEN           "wsse:InvalidSecurityToken"
#define L_NEMO_FAULTCODE_FAILEDAUTHENTICATION           "wsse:FailedAuthentication"
#define L_NEMO_FAULTCODE_FAILEDCHECK                    "wsse:FailedCheck"
#define L_NEMO_FAULTCODE_SECURITYTOKENUNAVAILABLE       "wsse:SecurityTokenUnavailable"
#define L_NEMO_FAULTCODE_MESSAGEEXPIRED                 "wsu:MessageExpired"
#define L_NEMO_FAULTCODE_UNSUPPORTEDSECUREPROTOCOL      "nemoc:UnsupportedSecureProtocol"

/* security policy flags */
#define L_NEMO_POLICY_ILLEGAL_FLAG                (1<<7)
#define L_NEMO_POLICY_NOSECURITY_FLAG             (0)
#define L_NEMO_POLICY_INTEGRITYONLY_FLAG          (1<<0)
#define L_NEMO_POLICY_NONCE_FLAG                  (1<<1)
#define L_NEMO_POLICY_TIMESTAMP_FLAG              (1<<2)
#define L_NEMO_POLICY_INTEGRITYANDFRESHNESS_FLAG  (L_NEMO_POLICY_INTEGRITYONLY_FLAG | \
                                                   L_NEMO_POLICY_NONCE_FLAG | \
                                                   L_NEMO_POLICY_TIMESTAMP_FLAG)
#define L_NEMO_POLICY_CONFIDENTIALITYONLY_FLAG    (1<<3)
#define L_NEMO_POLICY_INTEGRITYANDCONFIDENTIALITY_FLAG (L_NEMO_POLICY_INTEGRITYONLY_FLAG | \
                                                        L_NEMO_POLICY_CONFIDENTIALITYONLY_FLAG | \
                                                        L_NEMO_POLICY_NONCE_FLAG)
#define L_NEMO_POLICY_FULLSECURITY_FLAG           (L_NEMO_POLICY_NOSECURITY_FLAG | \
                                                   L_NEMO_POLICY_INTEGRITYANDFRESHNESS_FLAG | \
                                                   L_NEMO_POLICY_CONFIDENTIALITYONLY_FLAG)

/* other constants */
#define L_ID_SIGNATURE                           "NemoIntegrity"
#define L_ID_ENCRYPTEDKEY                        "NemoConfidentiality"
#define L_ID_ENCRYPTEDMESSAGEKEY                 "EncryptedMessageKey"
#define L_ID_ENCRYPTEDBODY                       "EncryptedBody"
#define L_ID_ENCRYPTEDSIGNATURE                  "EncryptedSignature"

#define L_REF_ID_SIGNATURE                       "#" L_ID_SIGNATURE
#define L_REF_ID_ENCRYPTEDKEY                    "#" L_ID_ENCRYPTEDKEY
#define L_REF_ID_ENCRYPTEDMESSAGEKEY             "#" L_ID_ENCRYPTEDMESSAGEKEY
#define L_REF_ID_ENCRYPTEDBODY                   "#" L_ID_ENCRYPTEDBODY
#define L_REF_ID_ENCRYPTEDSIGNATURE              "#" L_ID_ENCRYPTEDSIGNATURE

#define L_SOAPBODY_ID                            "soapBody"

#define L_NONCE_MAXSIZE  (128)  /* max length of a nonce (bytes) */
#define L_NONCE_SIZE     (16)   /* nonce length (bytes) used for this implementation */

#define L_MSGLIFETIMEPERIOD_DEFAULT  (360)  /* default message life period (in seconds) */

#define L_UTCZ_SIZE  (20)   /* nr chars for the UTC Z time */

#define L_DETAILS_MAXSIZE (25) /* maximum of details in a faultinfo; magic number */

/** Generic wsa node object */
typedef struct {
    u_int32_t type;      /**< ::NEMO_WSA_NOELEMENTSET, ::NEMO_WSA_NOELEMENTSET, ::NEMO_WSA_TYPE_TO,
                              ::NEMO_WSA_TYPE_ACTION, ::NEMO_WSA_TYPE_RELATESTOORIGIN, ::NEMO_WSA_TYPE_RELATESTOREPLY
                              ::NEMO_WSA_TYPE_MESSAGEID, ::NEMO_WSA_TYPE_FROM, ::NEMO_WSA_TYPE_REPLYTO
                              ::NEMO_WSA_TYPE_FAULTTO, ::NEMO_WSA_TYPE_REQMESSAGEID, ::NEMO_WSA_TYPE_RELATESTOOREQUEST */
    xml_node_t *xmlnode; /**< xml node for a wsa element */
} nemo_l_wsa_node_t;

/** Vector of ::nemo_l_wsa_node_t nodes */
typedef struct {
    int32_t size;             /**< Number of nodes */
    nemo_l_wsa_node_t *nodes; /**< wsa nodes */
} nemo_l_wsa_node_vec_t;


/**
 * Generates a nonce (Strong random number)
 *
 * @param[in] in_len   Length in bytes of the nonce to generate
 * @param[out] out_buf Generated nonce (in_len) bytes
 *
 * @return ::ERR_OK
 */
extern retcode_t nemo_l_GenNonce(int32_t in_len, u_int8_t *out_buf);

/**
 * Generates an encryption key
 *
 * @param[in]  in_len  Length in bytes of the key to generate
 * @param[out] out_buf Generated key (in_len) bytes
 *
 * @return ::ERR_OK
 */
extern retcode_t nemo_l_GenEncKey(int32_t in_len, u_int8_t *out_buf);

/**
 * Compose the body of a SOAP message
 *
 * The output xml node is as follows
 * @verbatim
 * <senv:Body wsu:Id=::L_SOAPBODY_ID xmlns:wsu=::NS_WSU xmlns:senv=::L_NS_SOAPENV>
 *   ::in_payload
 * </senv:Body>
 * @endverbatim
 *
 * @param[in]  in_payload   body of the message
 * @param[out] out_soapbody xml node
 *
 * @return ::xml_ComposeElement
 */
extern retcode_t
nemo_l_ComposeSOAPBody(xml_node_t *in_payload, xml_node_t **out_soapbody);

/**
 * Generates SOAP message string from headers and body xml nodes
 * @verbatim
 * <senv:Envelope xmlns:senv=::L_NS_SOAPENV xmlns:xenc=::NS_XENC>
 *   <senv:Header>
 *     ::in_head_nodes
 *   </senv:Header>
 *   <senv:Body wsu:Id=::L_SOAPBODY_ID xmlns:wsu=::L_NS_WSU xmlns:senv=::L_NS_SOAPENV>
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
nemo_l_ComposeSOAP(xml_node_t **in_head_nodes, u_int8_t in_head_nodes_count, xml_node_t *in_body,
                   binstr_t **out_str);

/**
 * Converts internal nemo errors to generic nemo errors
 *
 * @param[in] in_err internal nemo error
 * @return error code
 */
extern retcode_t
nemo_l_ConvErr(retcode_t in_err);

/**
 * Frees a suspension list item
 * @param[in] in_update suspension list update to free
 * @return ::ERR_OK
 */
extern retcode_t nemo_l_FreeSL_update(octobj_sl_update_t *in_update);

/**
 * Frees License Suspension Additions/Subtractions ID list
 * @param[in] in_list list of IDs
 * @return ::ERR_OK
 */
extern retcode_t nemo_l_FreeSL_idlist(octobj_sl_idlist_t *in_list);

/**
 * Parses a SOAP Fault xml node
 *
 * @param[in]  in_body_node    SOAP body node
 * @param[in]  in_header_node  SOAP header node
 * @param[out] out_info        nemo_faultinfo_t object containing the parsed information
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_NEMO_ILLEGALXML
 * @return ::ERR_XML_NOTFOUND
 * @return ::xml_Find
 * @return ::xml_GetContent
 * @return ::nemo_l_ParseFaultDetail
 */
extern retcode_t nemo_l_ParseSoapFault(xml_node_t *in_body_node, xml_node_t *in_header_node, nemo_faultinfo_t **out_info);

/**
 * Parse a detail xml node of a SOAL Fault xml node.
 *
 * @specref See Marlin Core System Specification V1.2, 5.3 Message Faults
 *
 * @param[in]  in_body_node       SOAP body node
 * @param[in]  in_header_node     SOAP header node
 * @param[out] out_details        new ::nemo_faultdetail_t objects containing the
 *                                parsed information (max L_DETAILS_MAXSIZE)
 * @param[out] out_details_count  nr of details (exc:ServiceException elements)
 *
 * @return ::ERR_NOMEM
 * @return ::xml_Find
 * @return :: l_ParseOneFaultDetail
 */
extern retcode_t nemo_l_ParseFaultDetail(xml_node_t         *in_body_node,
                                         xml_node_t         *in_header_node,
                                         nemo_faultdetail_t ***out_details,
                                         u_int32_t          *out_details_count);

/**
 * Frees a SOAP fault detail object
 * @param[in,out] io_detail object to free
 * @return ::ERR_OK
 */
extern retcode_t nemo_l_FreeFaultDetail(nemo_faultdetail_t *io_detail);

/**
 * Frees a NEMO certificate object
 * @param[in,out] io_cert object to free
 * @return ::ERR_OK
 */
extern retcode_t nemo_l_FreeCert(nemo_cert_t *io_cert);


/* NEMO node functions */



/**
 * This function builds the following XML node:
 * @verbatim
 * <AttributeList [Id="..."]>
 *   <Attribute name="Type">personality</Attribute>
 *   <Attribute name="Manufacturer_id">
 *     urn:marlin:organization:sony:connect"
 *   </Attribute>
 *   <Attribute name="Model">Windows PC Client</Attribute>
 *   <Attribute name="Version/Major" type="int">1</Attribute>
 *   <Attribute name="Version/Minor" type="int">0</Attribute>
 *   <Attribute name="Version/Revision" type="int">0</Attribute>
 * </AttributeList>
 * @endverbatim
 * Such XML node is used as one of two parameters in a request message of
 * the Octopus Personalization protocol.
 *
 * @specref Octopus Personalization Protocol 1.0 section 3.1
 *
 * @param[in] in_id     An ID, which will be used as the value of Id attribute
 *                      of AttributeList element. If it is NULL, no Id
 *                      attribute will be inserted.
 * @param[out] out_node A pointer to a buffer which will hold a pointer to
 *                      a generated XML node.
 *
 * @return ::ERR_OK
 * @return ::ERR_XML_*
 */
extern retcode_t
nemo_l_genAttr(char         *in_id,
               xml_node_t **out_node);

/**
 * This funciton constructs WSA message information header XML elements:
 * "To", "Action", "RelatesTo", "MessageID", "From", "ReplyTo" and "FaultTo".
 *
 * @specref Web Services Addressing (WS-Addressing),
 *          NEMO Message Bindings 1.0 section 3.
 *
 * @param[in] in_wsadata      Structure containing information about elements,
 *                            which have to be constructed.
 * @param[out] out_wsanodevec A pointer to a buffer which will hold constructed
 *                            XML nodes upon return. Note that max. number of
 *                            the nodes is NEMO_WSA_ELEMENT_MAX_SIZE.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_NEMO_ILLEGALARGS
 * @return ::ERR_XML_*
 */
extern retcode_t
nemo_l_WSA_compose(nemo_wsadata_t          *in_wsadata,
                   nemo_l_wsa_node_vec_t **out_wsanodevec);


/**
 * This function extracts (if any) "To", "Action", "RelatesTo", "MessageID",
 * "From", "ReplyTo" and "FaultTo" WSA message information header XML elements
 * from a given SOAP header and saves them (and their values) in specified
 * buffers.
 *
 * @specref Web Services Addressing (WS-Addressing),
 *          NEMO Message Bindings 1.0 section 3.
 *
 * @param[in] in_header       A pointer to an XML node containing a SOAP
 *                            header element.
 * @param[io,out] io_wsadata  A pointer to a structure where values and
 *                            types of the extracted elements will be saved
 *                            upon return.
 * @param[out] out_wsanodevec A pointer to a buffer which will hold extracted
 *                            WSA header information elements upon return.
 *                            Note that max. number of extracted nodes is
 *                            NEMO_WSA_ELEMENT_MAX_SIZE.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_NEMO_ILLEGALARGS
 * @return ::ERR_NEMO_ILLEGALXML
 * @return ::ERR_XML_*
 */
extern retcode_t
nemo_l_WSA_decompose(xml_node_t              *in_header,
                     nemo_wsadata_t          *io_wsadata,
                     nemo_l_wsa_node_vec_t **out_wsanodevec);


/**
 * This function sets the message correlation information in case of a request
 * or checks the message correlation otherwise.
 *
 * @param[in,out] io_wsadata    A pointer to the structure where message
 *                              correlation information will be set or
 *                              taken from.
 * @param[in] in_prot_step_type Protocol step type describing if it is a request
 *                              (NEMO_PROTOCOL_STEPTYPE_REQUEST) or not.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_NEMO_ILLEGALARGS
 * @return ::ERR_NEMO_WRONGCORRELATION
 */
extern retcode_t
nemo_l_WSA_docorrelation(nemo_wsadata_t *io_wsadata,
                         u_int8_t         in_prot_step_type);


/**
 * This function frees the nodes composing the vector as well as
 * the vector itself.
 *
 * @param[in,out] io_wsanodevec A pointer to a WSA node vector.
 *
 * @return ::ERR_OK
 * @return ::ERR_XML_*
 */
extern retcode_t
nemo_l_FreeWsaNodeVec(nemo_l_wsa_node_vec_t *io_wsanodevec);


extern retcode_t nemo_l_GetTrustedTimeAsUTCformat(char **out_utc_time);

#endif /* NEMO_TYPES_H */

