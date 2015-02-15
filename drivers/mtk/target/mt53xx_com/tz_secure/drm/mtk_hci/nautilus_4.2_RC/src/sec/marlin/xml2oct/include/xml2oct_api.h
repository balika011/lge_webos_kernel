/**
 * @addtogroup xml2oct xml2oct
 * @ingroup marlin
 * @{
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef XML2OCT_API_H
#define XML2OCT_API_H

#ifdef __cplusplus
extern "C" {
#endif


    /*===========================================================================*/
    /** @file
     * @brief library for parsing XMLs and generating Octopus Objects
     *
     * dependency base,b64,xml,octobj
     * @author Norifumi Goto <n-goto@sm.sony.co.jp>
     * @date   31 Jan, 2005 last modified by n-goto
     */
    /*===========================================================================*/


    /*---------------------------------------------------------------------------*
     * error code definitions
     *---------------------------------------------------------------------------*/

#define ERR_XML2OCT_ILLEGALSIGNATURE        (ERR_XML2OCT_BASE|1)
#define ERR_XML2OCT_ILLEGALXML              (ERR_XML2OCT_BASE|2)
#define ERR_XML2OCT_NOSIGNATURE             (ERR_XML2OCT_BASE|3)
#define ERR_XML2OCT_UNSUPPORTEDSIGNATURE    (ERR_XML2OCT_BASE|4)
#define ERR_XML2OCT_CONTENTKEYNOTDECRYPTED  (ERR_XML2OCT_BASE|5)
#define ERR_XML2OCT_REVOKEDCERT             (ERR_XML2OCT_BASE|6)


    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

#define XML2OCT_KEYPAIR_DRM_SIG         0
#define XML2OCT_KEYPAIR_DRM_ENC         1
#define XML2OCT_KEYPAIR_APP_SIG         2
#define XML2OCT_KEYPAIR_APP_ENC         3
#define XML2OCT_KEYPAIR_MAX             2

#define XML2OCT_ASSERTION_ROLE          0
#define XML2OCT_ASSERTION_DATACERT      1
#define XML2OCT_ASSERTION_DRMSERVICE_ROLE  2
#define XML2OCT_ASSERTION_DCSSERVICE_ROLE  3


    /**
     * HMAC signature.
     */
    typedef struct {
        binstr_t *s11n;   /**< Data */
        binstr_t *digest; /**< Signature */
    } xml2oct_hmacsig_t;


    /**
     * Octopus License.
     * This structure is returned by the ::xml2oct_ParseLicense().
     */
    typedef struct {
        octobj_license_t  *license;  /**< Octopus license object */
        xml2oct_hmacsig_t *hmacs;    /**< HMAC signatures */
    } xml2oct_license_t;


    /**
     * Private key and public key certs.
     */
    typedef struct {
        xml_certchain_t certs;  /**< Public key cets */
    } xml2oct_keypair_t;


    /**
     * NEMO personality.
     *
     * @specref NemoNode.xsd
     */
    typedef struct {
        binstr_t *id;
        xml2oct_keypair_t keys[XML2OCT_KEYPAIR_MAX]; /**< digitalSignature and
                                                      keyEncipherment keys */
        binstr_t *assertion;                         /**< SAML assetion(s) */
    } xml2oct_nemonode_t;


    /*---------------------------------------------------------------------------*/
    /**
     * This function frees a given license object.
     *
     * @param[in,out] io_license A pointer to a license object.
     *
     * @return ::ERR_OK
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_FreeLicense(xml2oct_license_t *io_license);


    /*---------------------------------------------------------------------------*/
    /**
     * This function parses an Octopus License represented as an XML document.
     *
     * @param[in]  in_licensexml License text in XML format.
     * @param[out] out_license   A pointer to a buffer which will hold octopus
     *                           license objects upon return.
     *
     * @specref Octopus Objects 1.0.1
     *          Octopus.xsd
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML2OCT_NOSIGNATURE
     * @return ::ERR_XML2OCT_UNSUPPORTEDSIGNATURE
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParseLicense(const binstr_t    *in_licensexml,
                         xml2oct_license_t **out_license);


    /*---------------------------------------------------------------------------*/
    /**
     * This function verifies Octopus License HMAC signatures.
     *
     * @param[in] in_license A pointer to a license object.
     * @param[in] in_keys    An array of content keys object
     *
     * @specref Octopus Objects 1.0.1
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_XML2OCT_ILLEGALSIGNATURE
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_NG
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_VerifyLicense(xml2oct_license_t   *in_license,
                          octobj_contentkey_t **in_ckeys);


    /*---------------------------------------------------------------------------*/
    /**
     * This function parses an Octopus Link represented as an XML document.
     *
     * @param[in] in_linkxml A link represented as an XML document.
     * @param[out] out_link  A pointer to a biffer which will hold link object
     *                       upon return.
     *
     * @specref Marlin - Core System Specification 1.3 section 3
     *          Octopus.xsd
     *          Scuba Key Distribution 1.0.1
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML2OCT_NOSIGNATURE
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParseLink(const binstr_t *in_linkxml,
                      octobj_link_t  **out_link);


    /*---------------------------------------------------------------------------*/
    /**
     * This function parses Octopus Public Personality represented as
     * an XML document. If it is required, the signature of the personality
     * can be verified.
     *
     * @param[in] in_pubxml     A pointer to public part of an Octopus Personality
     *                          represented as an XML document.
     * @param[in] in_verifyflag A flag indicating if the signature of the
     *                          personality has to be checked (~0) or not (0).
     * @param[out] out_node     A pointer to a buffer which will hold a pointer to
     *                          the parsed public part of personality node upon return.
     *
     * @specref Marlin - Core System Specification 1.3 section 3
     *          Octopus.xsd
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML2OCT_NOSIGNATURE
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParsePublicPersonality(const binstr_t *in_pubxml,
                                   int32_t        in_verifyflag,
                                   octobj_node_t  **out_node);


    /*---------------------------------------------------------------------------*/
    /**
     * This function parses an Octopus User Node represented as an XML document.
     *
     * @param[in] in_node     ? A pointer to a buffer containing an User Node
     *                          represented as an XML document.
     * @param[out] out_node     A pointer to a buffer which will hold a pointer to
     *                          the parsed User Node upon return.
     * @param[in] in_verifyflag ~0 if is neccessary to check the signature.
     *                           0 otherwise.
     *
     * @specref Marlin - Core System Specification 1.3 section 3
     *          Octopus.xsd
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML2OCT_NOSIGNATURE
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParseNode(const binstr_t *in_node,
                      octobj_node_t  **out_node,
                      int32_t        in_verifyflag);


    /*---------------------------------------------------------------------------*/
    /**
     * This function frees a given NEMO node object.
     *
     * @param[in,out] io_nemonode A pointer to a NEMO node object to be freed.
     *
     * @return ::ERR_OK
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_FreeNEMOPersonality(xml2oct_nemonode_t *io_nemonode);


    /*---------------------------------------------------------------------------*/
    /**
     * This function parses a SAML assertion represented as an XML document.
     *
     * @param[in] in_assertion   A SAML assertion represented as an XML document.
     * @param[in] in_type        A type of the assertion. Must be either
     *                           XML2OCT_ASSERTION_ROLE or XML2OCT_ASSERTION_DATACERT or XML2OCT_ASSERTION_DRMSERVICE_ROLE or XML2OCT_ASSERTION_DCSSERVICE_ROLE
     *                           Note that this type shows which keys are used
     *                           to check the signature of the assertion - it is
     *                           not used if signature check if off.
     * @param[out] out_assertion A pointer to a buffer which will hold a point to
     *                           the parsed assertion upon return.
     * @param[in] in_verifyflag  ~0 if is neccessary to check the signature.
     *                           0 otherwise.
     *
     * @specref Assertions and Protocol for the OASIS Security Assertion Markup
     *          Language (SAML) V1.1
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_XML2OCT_ILLEGALXML
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParseAssertion(const binstr_t     *in_assertion,
                           int32_t            in_type,
                           octobj_assertion_t **out_assertion,
                           int32_t            in_verifyflag);


    /*---------------------------------------------------------------------------*/
    /**
     * This function parses an Octopus Control object  represented as
     * an XML document.
     *
     * @param[in] in_control   A pointer to an Octopus Control object represented
     *                         as an XML document (an instance of xml_node_t type).
     * @param[out] out_control A pointer to a buffer which will hold a pointer to
     *                         the parsed Octopus Control object upon return.
     *
     * @specref Octopus Objects 1.0, section 2.5
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML_*
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParseControlFromXML(const xml_node_t *in_control,
                                octobj_control_t **out_control);

    /*---------------------------------------------------------------------------*/
    /**
     * This function verify service role assertion
     * an XML document.
     *
     * @param[in] in_ast      A pointer to an service role assertion
     * @param[in] in_type     A type of the assertion. Must be either
     * @param[in] in_svcrole  A pointer to an service role
     * @param[out] out_nameId A pointer to a buffer which will hold a pointer to
     *                        the NameIdentifier.
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML_*
     */
    /*---------------------------------------------------------------------------*/
    retcode_t
    xml2oct_VerifyServiceRoleAssertion(const binstr_t  *in_ast,
                                       int32_t          in_type,
                                       const binstr_t  *in_svcrole,
                                       binstr_t        **out_nameId);

    /*---------------------------------------------------------------------------*/
    /**
     * Parses an XML Octopus bundle node and extract attributes.
     *
     * @param[in]  in_octobj   An XML Octopus bundle node.
     * @param[out] out_attrs   A list of attributes.
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_XML2OCT_ILLEGALXML
     * @return ::ERR_XML_*
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    xml2oct_ParseOctopusAttributes(xml_node_t    *in_octobj,
                                   octobj_vec_t **out_attrs);
    /**
     * Parse an XML parameter block (used for Agent evaluation or Action perform.
     *
     * @param[in]  in_xml_params Xml parameters
     * @param[out] out_params    xml2oct representation of parameters
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     * @return ::ERR_XML2OCT_ILLEGALXML
     */
    extern retcode_t
    xml2oct_ParseParameterBlock(xml_node_vec_t   *in_xml_params,
                                octobj_params_t **out_params);
#ifdef __cplusplus
}
#endif

#endif /* XML2OCT_API_H */

/** @} */
