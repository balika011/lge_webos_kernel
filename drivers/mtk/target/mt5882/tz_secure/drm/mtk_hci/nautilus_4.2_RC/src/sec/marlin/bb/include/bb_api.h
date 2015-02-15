/** @addtogroup bb bb
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef BB_API_H
#define BB_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*===========================================================================*/
    /** @file
     * @brief library for BroadBand protocols
     *
     * This library contains functions to generate request messages and parse
     * response messages for the BB protocols. It supports the client-side
     * interaction with Registration service, License service,
     * Data Update Service and Data Certification Service.
     *
     * @note The metering protocol is not implemented.
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * error code definitions
     *---------------------------------------------------------------------------*/

#define ERR_BB_ILLEGALXML                  (ERR_BB_BASE|1)
#define ERR_BB_CRYPTOERROR                 (ERR_BB_BASE|2)
#define ERR_BB_NEEDTTUPDATE                (ERR_BB_BASE|3)
#define ERR_BB_NEEDSLUPDATE                (ERR_BB_BASE|4)
#define ERR_BB_NEEDUPDATE                  (ERR_BB_BASE|5)
#define ERR_BB_NOEXISTCERTSET              (ERR_BB_BASE|6)
#define ERR_BB_SOAPFAULT                   (ERR_BB_BASE|7)
#define ERR_BB_TTNOTAVAILABLE              (ERR_BB_BASE|8)
#define ERR_BB_NOSOAPFAULT                 (ERR_BB_BASE|9)
#define ERR_BB_ILLEGALCERT                 (ERR_BB_BASE|10)
#define ERR_BB_MESSAGECORRUPTED            (ERR_BB_BASE|11)
#define ERR_BB_REVOKEDCERT                 (ERR_BB_BASE|12)

    /*---------------------------------------------------------------------------*
     * macro
     *---------------------------------------------------------------------------*/

#define BB_DSREQUEST_TT   0x00000001
#define BB_DSREQUEST_SL   0x00000002

#define BB_PTYPE_REGISTRATION                 (2)
#define BB_PTYPE_LICENSE                      (3)
#define BB_PTYPE_DATACERTIFICATION            (4)
#define BB_PTYPE_DATAUPDATE                   (5)
#define BB_PTYPE_LTP                          (6)

    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

    typedef void *bb_handle_t;

    /*
     * This structure contains metadata items for DCS
     */
    typedef struct {
        int32_t flags;       /**< Flag which indicates with metadata is included */
        time_unix_t *tt;     /**< Trusted time                                   */
        char *ls_name;       /**< Suspension list name                           */
        int32_t *ls_serial;  /**< License suspension serial number               */
        char *dcsn;          /**< Data certification standard name               */
    } bb_metadata_t;

    /*
     * This type represents possible metadata types in Nautilus.
     */
    typedef enum {
        BB_TRUSTED_TIME = 0,
        BB_LS_SERIAL,
        BB_CRL_NUMBER,
        BB_BKB_VERSION
    } bb_metadata_type_t;

    /*
     * This type represents a metadata value.
    */
    typedef union {
        time_unix_t   tt;
        u_int32_t ls_serial;
        crlnumber_t crl_number;
        u_int32_t bkb_ver;
    } bb_metadata_val_t;

    /*
     * This type represents a particular metadata value with a given type.
     */
    typedef struct {
        bb_metadata_type_t type;
        bb_metadata_val_t  val;
    } bb_metadata_data_t;

    /*
     * This structure represents a metadata item together with its value.
     */
    typedef struct {
        binstr_t            *name;
        binstr_t            *ns;
        bb_metadata_data_t data;
    } bb_metadata_item_t;

    /*
     * This stucture describes a Data Certification Standard consisting of
     * a name of the standard and a list of metadata items.
     *
     * @note it is the same type as in npcommon.h - the bb layer cannot use types
     *       defined in the np layer, so we have to redefine them here and convert
     *       them in the npbb layer.
     */
    typedef struct {
        binstr_t           *crt_std_name;
        u_int32_t           num_metadata_items;
        bb_metadata_item_t *metadata_items;
    } bb_crt_std_t;

    /*---------------------------------------------------------------------------*
     * APIs
     *---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------*
     * PROTOCOLs
     *---------------------------------------------------------------------------*/

    /**
     * @brief Initialize a BB handle
     *
     * @param[in] in_ptype     Protocol type
     * @param[out] out_handle  BB handle
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    bb_Init(int32_t in_ptype, bb_handle_t *out_handle);

    /**
     * @brief Finalize a BB handle
     *
     * This function finalizes (deallocates) a BB handle. This function should
     * be called only once for a given handle.
     *
     * @note The handle is not set to NULL.
     *
     * @param[in,out] io_handle BB protocol handle
     */
    extern retcode_t
    bb_Fin(bb_handle_t io_handle);



    /*===========================================================================*
     * Registration Service
     *===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a User Node request message
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle             Protocol handle
     * @param[in,out] io_bbpersonality      BB personality
     * @param[in]     in_num_atnodes        Number of assertion nodes
     * @param[in]     in_atnodes            (DCSA) Assertion nodes
     * @param[in]     in_servernode         Server node ID
     * @param[in]     in_businesstoken      Business token
     * @param[out]    out_req               Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::nemo_GenBusinessToken
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::xml_ComposeElement
     *
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_gen8pusNodeRequest(bb_handle_t io_handle,
                              xml2oct_nemonode_t **io_bbpersonality,
                              u_int8_t in_num_atnodes,
                              xml_node_t **in_atnodes,
                              binstr_t *in_servernode,
                              binstr_t *in_businesstoken,
                              binstr_t **out_req);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse a User Node response message
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle    Protocol handle
     * @param[in]     in_resp      Response message
     * @param[out]    out_node     User node (<oct:Bundle> structure)
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_WSSE_decompose_by_policy
     * @return result from ::xml_Find
     * @return result from ::xml_ToString
     *
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_recv8pusNodeResponse(bb_handle_t io_handle, binstr_t *in_resp,
                                binstr_t **out_node);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a Link acquisition request message
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle             Protocol handle
     * @param[in,out] io_bbpersonality      BB personality
     * @param[in]     in_num_atnodes        Number of assertion nodes
     * @param[in]     in_atnodes            (DCSA) Assertion nodes
     * @param[in]     in_devnode            Device node
     * @param[in]     in_linkfrom           <oct:Bundle> node which contains the
     *                                      public part of the Octopus Personality
     *                                      Node or User Node that should be
     *                                      referenced by <oct:LinkFrom> element.
     *                                      This parameter can be NULL. In this case
     *                                      the device node is used as from node.
     * @param[in]     in_linkto             <oct:Bundle> node which contains the
     *                                      public part of the Octopus Personality
     *                                      Node or User Node that should be
     *                                      referenced by <oct:LinkTo> element
     * @param[in]     in_servernode         Server node ID
     * @param[in]     in_businesstoken      Business token
     * @param[out]    out_req               Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_GenBusinessToken
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::xml_Find
     * @return result from ::xml_ComposeElement
     *
     * @todo Do we really need the in_devnode parameter?
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_genLinkRequest(bb_handle_t io_handle,
                          xml2oct_nemonode_t **io_bbpersonality,
                          u_int8_t in_num_atnodes,
                          xml_node_t **in_atnodes,
                          xml_node_t *in_devnode, xml_node_t *in_linkfrom,
                          xml_node_t *in_linkto, binstr_t *in_servernode,
                          binstr_t *in_businesstoken, binstr_t **out_req);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse a Link acquisition response message
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle    Protocol handle
     * @param[in]     in_resp      Response message
     * @param[out]    out_link     Link (<oct:Bundle> structure)
     * @param[out]    out_ac       [OPTIONAL] Agent Carrier
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_WSSE_decompose_by_policy
     * @return result from ::xml_Find
     * @return result from ::xml_ToString
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_recvLinkResponse(bb_handle_t io_handle, binstr_t *in_resp,
                            binstr_t **out_link, xml_node_t **out_ac);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a confirmation message (for Link acquisition or release)
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle         Protocol handle
     * @param[in]     in_arb            [OPTIONAL] Agent Result Block
     * @param[out]    out_confirmation  Confirmation message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::xml_ComposeElement
     *
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_genConfirmation(bb_handle_t io_handle, xml_node_t *in_arb,
                           binstr_t **out_confirmation);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a link release request message
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle           Protocol handle
     * @param[in,out] io_bbpersonality    Nemo personality
     * @param[in]     in_num_atnodes      Number of assertion nodes
     * @param[in]     in_atnodes          (DCSA) Assertion nodes
     * @param[in]     in_linkfrom         <oct:Bundle> node which contains the
     *                                    public part of the Octopus Personality
     *                                    Node or User Node that should be
     *                                    referenced by <oct:LinkFrom> element.
     *                                    This parameter can be NULL. In this case
     *                                    the device node is used as from node.
     * @param[in]     in_linkto           <oct:Bundle> node which contains the
     *                                    public part of the Octopus Personality
     *                                    Node or User Node that should be
     *                                    referenced by <oct:LinkTo> element
     * @param[in]     in_servernode       Server node ID
     * @param[in]     in_businesstoken    Business token (an opaque data structure
     *                                    containing service-specific data retrieved
     *                                    from the Action token)
     * @param[out]    out_req             Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_GenBusinessToken
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::xml_Find
     * @return result from ::xml_ComposeElement
     *
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_genLinkReleaseRequest(bb_handle_t io_handle,
                                 xml2oct_nemonode_t **io_bbpersonality,
                                 u_int8_t in_num_atnodes,
                                 xml_node_t **in_atnodes,
                                 xml_node_t *in_linkfrom, xml_node_t *in_linkto,
                                 binstr_t *in_servernode, binstr_t *in_businesstoken,
                                 binstr_t **out_req);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse a Link release response message
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle    Protocol handle
     * @param[in]     in_resp      Response message
     * @param[out]    out_ac       Agent carrier
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_WSSE_decompose_by_policy
     * @return result from ::bb_l_ConvErr
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_REG_recvLinkReleaseResponse(bb_handle_t io_handle,
                                   binstr_t *in_resp, xml_node_t **out_ac);

    /*===========================================================================*
     * License Service
     *===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a license acquisition request message
     *
     * This function generates a license acquisition request message.
     *
     * @specref See Marlin Broadband Delivery Sytem Specification V1.2
     *          [sections 2.4.2, 4.2.1, 5.2.1]
     *
     * @param[in,out] io_handle           Protocol handle
     * @param[in,out] io_bbpersonality    Nemo personality
     * @param[in]     in_num_atnodes      Number of assertion nodes
     * @param[in]     in_atnodes          DCSA Assertion nodes
     * @param[in]     in_usernode         User node
     * @param[in]     in_servernode       Server node
     * @param[in]     in_businesstoken    Business token (an opaque data structure
     *                                    containing service-specific data retrieved
     *                                    from the Action token)
     * @param[out]    out_req             Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_GenBusinessToken
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::xml_ComposeElement
     * @return result from ::xml_GetTopElement
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_LA_genLicenseRequest(bb_handle_t io_handle,
                            xml2oct_nemonode_t **io_bbpersonality,
                            u_int8_t in_num_atnodes,
                            xml_node_t **in_atnodes,
                            xml_node_t *in_usernode,
                            binstr_t *in_servernode,
                            binstr_t *in_businesstoken,
                            binstr_t **out_req);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse a License Acquisition response message
     *
     * This function parses a License Acquisition response message
     * and extracts the license. The license is an <oct:Bundle> structure
     * which contains Control, Controller, Content Key(s), and Protector
     * objects related to the node (octopus or user) provided in the
     * request.
     *
     * @specref See Marlin Broadband Delivery Sytem Specification V1.2
     *          [sections 2.4.2, 4.2.1, 5.2.1]
     *
     * @param[in,out] io_handle    Protocol handle
     * @param[in]     in_resp      Response message
     * @param[out]    out_license  License (<oct:Bundle> structure which contains the license)
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_WSSE_decompose_by_policy
     * @return result from ::xml_Find
     * @return result from ::xml_ToString
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_LA_recvLicenseResponse(bb_handle_t io_handle, binstr_t *in_resp,
                              binstr_t **out_license);


    /*===========================================================================*
     * Data Certification Service
     *===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a DCS request message
     *
     * This function generates a Data Certification Service request message.
     * The request will be used to obtain a proof (in the form of a DCSA)
     * that assures certain security-related metadata items (secure clock,
     * license suspension updates, etc.) at the DRM client are reasonably
     * up to date.
     *
     *  @specref See Marlin Broadband Delivery Sytem Specification V1.2
     *          [sections 2.4.5, 4.2.4, 5.2.4]
     *
     * @param[in,out] io_handle         Protocol handle
     * @param[in,out] io_bbpersonality  BB personality
     * @param[in]     in_crt_stds       Certification standards together
     *                                  with corresponding metadata values.
     * @param[in]     in_num_crt_stds   A number of of certification standards.
     * @param[out]    out_req           Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_ILLEGALARGS      Illegal arguments
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::time_Unix2Date
     * @return result from ::xml_ComposeElement
     * @return result from ::xml_FreeNode
     *
     * @todo Remove limitation: A DCS request can contain multiple RequestSecurityToken elements
     *                          (one per required data certification standard)
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_DCS_genRequest(bb_handle_t          io_handle,
                      xml2oct_nemonode_t **io_bbpersonality,
                      binstr_t           *in_servernode,
                      bb_crt_std_t       in_crt_stds[],
                      u_int32_t          in_num_crt_stds,
                      binstr_t           **out_req);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse a DCS response message
     *
     * This function parses a Data Certification Service response message
     * and extracts the DCSA nodes. A DCSA assertion is a proof
     * that assures certain security-related metadata items (secure clock,
     * license suspension updates, etc.) at the DRM client are reasonably
     * up to date.
     *
     * @specref See Marlin Broadband Delivery Sytem Specification V1.2
     *          [sections 2.4.5, 4.2.4, 5.2.4]
     *
     * @param[in,out] io_handle           Protocol handle
     * @param[in]     in_resp             Response message
     * @param[out]    out_num_atnodes     Number of assertion nodes
     * @param[out]    out_atnodes         DCSA assertion nodes
     * @param[out]    out_dcsns           Data Certification Standard names (1 per assertion)
     *
     * @return ::ERR_OK                 Success
     * @return ::ERR_ILLEGALHANDLE      Invalid protocol handle
     * @return ::ERR_ILLEGALARGS        Illegal arguments
     * @return ::ERR_NOMEM              Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::l_ParseErrResponse
     * @return result from ::nemo_WSSE_decompose_by_policy
     * @return result from ::xml_Find
     * @return result from ::xml_ToString
     *
     * @note When the security-related metadata submitted in the DCS request
     *       does not meet the requirements for the data certification standard,
     *       the DCS reponds with the namespaces and names of the security-related
     *       metadata that must be updated.
     *
     * @todo Remove limitation: more than 1 DCSA assertion can be returned
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_DCS_recvResponse(bb_handle_t io_handle, binstr_t *in_resp,
                        np_size_t *out_num_atnodes, binstr_t *** out_atnodes,
                        binstr_t *** out_dcsns);


    /*===========================================================================*
     * Data Update Service
     *===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate a Data Update request message
     *
     * This function generates a request message for a data update
     * of the security-related metadata. The DUS supports two types
     * of data updates: TrustedTimeUpdate and LicenseSuspensionUpdate(s).
     *
     * @specref See Marlin Broadband Delivery Sytem Specification V1.2
     *          [sections 2.4.5, 4.2.4, 5.2.4]
     *
     * @param[in,out] io_handle           Protocol handle
     * @param[in,out] io_bbpersonality  Nemo personality
     * @param[in]     in_servernode       Server node ID
     * @param[in]     in_metadata         Security-related metadata
     * @param[out]    out_req             Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_ILLEGALARGS      Illegal arguments
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::time_Unix2Date
     * @return result from ::xml_ComposeElement
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_DUS_genRequest(bb_handle_t io_handle,
                      xml2oct_nemonode_t **io_bbpersonality,
                      binstr_t *in_servernode,
                      bb_metadata_t *in_metadata,
                      binstr_t **out_req);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse a DUS response message
     *
     * This function parses a Data Update Service response message
     * and extracts the updated suspension list, which contains the
     * suspended licenses, and the trusted time if available.
     *
     * @specref See Marlin Broadband Delivery Sytem Specification V1.2
     *          [sections 2.4.5, 4.2.4, 5.2.4]
     *
     * @param[in,out] io_handle     Protocol handle
     * @param[in]     in_resp       Response message
     * @param[out]    out_tt        Trusted time
     * @param[out]    out_sl        License Suspension list
     *
     * @return ::ERR_OK                 Success
     * @return ::ERR_ILLEGALHANDLE      Invalid protocol handle
     * @return ::ERR_ILLEGALARGS        Illegal arguments
     * @return ::ERR_NOMEM              Out of memory
     * @return ::ERR_BB_ILLEGALXML
     * @return result from ::l_ParseLSL
     * @return result from ::nemo_WSSE_decompose_by_policy
     * @return result from ::time_Date2Unix
     * @return result from ::xml_Find
     * @return result from ::xml_GetContent
     * @return result from ::xml_ParseDateTime
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_DUS_recvResponse(bb_handle_t io_handle, binstr_t *in_resp,
                        time_unix_t **out_tt, octobj_sl_updates_t **out_sl);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Runs an agent (control)
     *
     * @param[in] in_xagent     xml node that contains the Bundle
     * @param[in] in_seashell   current seashell
     * @param[out] out_xresult xml node carrying the result of the agent
     *  execution (AgentResultBlock )
     * @param[out] out_seashell new seashell
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     * @return ::xml_GetAttr
     * @return ::l_GetAgent
     * @return ::drm_AgentEval
     * @return ::xml_ComposeElement
     * @return ::xml_AddAttribute
     * @return ::b64_Encode
     * @return ::xml_AddText
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_AgentEval(xml_node_t *in_xagent,
                 mbinstr_t *in_seashell,
                 u_int32_t  in_kp,
                 xml_node_t **out_xresult,
                 mbinstr_t **out_seashell);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Set the given certificates into the handle.
     *
     * @param[in,out]   io_handle     BB protocol handle
     * @param[in]       in_enccert    Encryption certificate
     * @param[in]       in_signcert   Signing certificate
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_NEMO_ILLEGALCERT
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_SetCert(bb_handle_t *io_handle,
               binstr_t *in_enccert,
               binstr_t *in_signcert);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get the response timestamp
     *
     * @see nemo_GetResponseTimestamp
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_GetResponseTimestamp(bb_handle_t *io_handle, time_unix_t *out_timestamp);

    /**
     * Set parameters to be used for device attestation
     *
     * @param io_handle        BB context to update with capability assertion parameters
     * @param in_manufacturer  Manufacturer value
     * @param in_model         Model value
     * @param in_app_id        Optional application identification value
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    bb_SetAttestation(bb_handle_t io_handle, binstr_t *in_manufacturer, binstr_t *in_model, binstr_t *in_app_id);

#if defined(NPOPT_ENABLE_DUS)
    /**
     *
     * @brief Free a license suspension updates list structure
     *
     * @param[in]  in_sl   License suspension updates list
     *
     * @return ::ERR_OK
     *
     */
    extern retcode_t
    bb_FreeSL(octobj_sl_updates_t *in_sl);
#endif


    /*-----------------------------------------------------------------------*/
    /**
     * @brief Retrieve the Fault information of a previous SOAP Fault message.
     *
     * @note the caller should not free out_faultinfo
     *
     * @param [in] in_handle
     * @param [out] out_faultinfo  fault information
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_BB_NOSOAPFAULT
     */
    /*-----------------------------------------------------------------------*/
    extern retcode_t
    bb_GetSoapFault(bb_handle_t       in_handle,
                    nemo_faultinfo_t  **out_faultinfo);


    /*===========================================================================*
     * Generate LTP Nemo Message
     *===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Generate LTP Nemo Message
     *
     * Initialalize WsseData before calling nemo_WSSE_compose_by_policy
     *
     * @specref See Marlin Broadband Delivery Specification V1.2
     *          [ sections 2.4.3, 4.2.2, 5.2.2 ]
     *
     * @param[in,out] io_handle             Protocol handle
     * @param[in,out] step_type             Step type ( NEMO_PROTOCOL_STEPTYPE_... as defined in nemo_api.h )
     * @param[in,out] io_bbpersonality      BB personality
     * @param[in]     in_servernode         Server node ID
     * @param[in]     in_businesstoken      Business token
     * @param[out]    out_req               Request message
     *
     * @return ::ERR_OK               Success
     * @return ::ERR_ILLEGALHANDLE    Invalid protocol handle
     * @return ::ERR_NOMEM            Out of memory
     * @return result from ::nemo_GenBusinessToken
     * @return result from ::nemo_GenMessageID
     * @return result from ::nemo_SetWSAField
     * @return result from ::bb_l_ConvErr
     * @return result from ::nemo_SetCert
     * @return result from ::nemo_WSSE_compose_by_policy
     * @return result from ::xml_ComposeElement
     */
    /*---------------------------------------------------------------------------*/

    extern retcode_t
    bb_ltp_genMsg( bb_handle_t           io_handle,
                   int32_t               step_type,
                   xml2oct_nemonode_t  **io_nemopersonality,
                   binstr_t             *in_servernode,
                   xml_node_t          **io_data_node,
                   binstr_t            **out_msg );

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Parse LTP Nemo Message
     *
     * Acquire personality, Initialalize WsseData before calling
     * nemo_WSSE_decompose_by_policy
     *
     * @param[in,out] io_handle BB handle
     * @param[in] step_type protocol step_type
     * @param[in] msgId Message Id
     * @param[in] in_message request/response message to be parsed
     * @param[out] out_xml_node Body informations extracted  from the message
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_INTERNALERROR
     * @return ::ltp_GetNEMOPersonality
     * @return ::nemo_WSSE_decompose_by_policy
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    bb_ltp_parseMsg( bb_handle_t  io_handle,
                     int32_t      step_type,
                     char        *msgId,
                     binstr_t    *in_msg,
                     xml_node_t  **out_data );

#ifdef __cplusplus
}
#endif

#endif                          /* BB_API_H */
/** @} */
