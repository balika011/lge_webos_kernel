/** @addtogroup npdrm npdrm
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef NPDRM_API_H
#define NPDRM_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief Nautilus Library API (DRM + Render)
     *
     * @author Norifumi Goto <n-goto@sm.sony.co.jp>
     */
    /*===========================================================================*/

#include <npdrm_types.h>

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Initialize DRM/Renderer component
     *
     * This API should be invoked first of all
     *  (except Personality/Link Register/Unregister APIs)
     *
     * @param[out] out_handle the buffer where the component handle will be stored
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALARGS           (NULL was specified in out_handle)
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_InitSession(hdls_context_t *out_handle);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Finalize DRM/Renderer component
     *
     * This API should be invoked at the end of use of the component
     *  (except Personality/Link Register/Unregister APIs)
     *
     * @param[in,out] io_handle session handle to be finalized
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_FinSession(hdls_context_t io_handle);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief initialize License
     *
     * @param[in,out] io_handle component handle
     * @param[in] in_licensesize license XML size
     * @param[in] in_licensebuf pointer to the license XML buffer
     * @param[out]    out_secbuf_sdata    Secure buffer containing sdata
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALLICENSE
     * @return ::ERR_NP_ALREADYSET
     * @return ::ERR_NP_NOPERSONALITYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_SetLicense(hdls_context_t  io_handle,
                  np_size_t       in_licensesize,
                  u_int8_t       *in_licensebuf,
                  np_secbuffer_t *out_secbuf_sdata);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Set license attributes used by ActionPerform.
     *
     * @param[in,out] io_handle       component handle
     * @param[in]     in_bundle_node  bundle node of for example an Octopus Public
     *                                Personality containing an <AttributeList>
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALPERSONALITY
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_SetLicenseAttributes(hdls_context_t  io_handle,
                            xml_node_t     *in_bundle_node);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief invoke Action.Check
     *
     * @param[in]  in_handle         Component handle
     * @param[in]  in_actiontype     Action type
     * @param[in]  in_action_params  Input parameters for code module
     * @param[out] out_secbuf        Secure buffer containing ESB information
     *
     * ESB information will be set only when
     *  return code is either ::ERR_NP_OK or ERR_NP_ACTIONNOTGRANTED
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALLICENSE
     * @return ::ERR_NP_LICENSENOTSET
     * @return ::ERR_NP_ALREADYPERFORMED
     * @return ::ERR_NP_ACTIONNOTGRANTED
     * @return ::ERR_NP_NOTSUPPORTED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ActionCheck(hdls_context_t        in_handle,
                   int32_t               in_actiontype,
                   np_action_params_t   *in_action_params,
                   np_secbuffer_t       *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief invoke Action.Perform
     *
     * @param[in,out] io_handle           Component handle
     * @param[in]     in_actiontype       Action type
     * @param[in]     in_action_params    Input parameters for code module
     * @param[out]    out_secbuf_esb      Secure buffer containing ESB information
     * @param[out]    out_secbuf_sdata    Secure buffer containing sdata
     *
     * ESB information/sdata will be set only when
     *  return code is either ::ERR_NP_OK or ERR_NP_ACTIONNOTGRANTED
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALLICENSE
     * @return ::ERR_NP_LICENSENOTSET
     * @return ::ERR_NP_CONTENTNOTSET
     * @return ::ERR_NP_ALREADYPERFORMED
     * @return ::ERR_NP_ACTIONNOTGRANTED
     * @return ::ERR_NP_NOTSUPPORTED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ActionPerform(hdls_context_t        io_handle,
                     int32_t               in_actiontype,
                     np_action_params_t   *in_action_params,
                     np_secbuffer_t       *out_secbuf_esb,
                     np_secbuffer_t       *out_secbuf_sdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief invoke Action.Describe
     *
     * @param[in]  in_handle          Component handle
     * @param[in]  in_actiontype      Action type
     * @param[in]  in_action_params   Input parameters for code module
     * @param[out] out_secbuf         Secure buffer containing ESB information
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALLICENSE
     * @return ::ERR_NP_LICENSENOTSET
     * @return ::ERR_NP_NODESCRIPTION
     * @return ::ERR_NP_NOTSUPPORTED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ActionDescribe(hdls_context_t        in_handle,
                      int32_t               in_actiontype,
                      np_action_params_t   *in_action_params,
                      np_secbuffer_t       *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Run the routine within in_callback with the plankton VM.
     *
     * @param[in, out] io_handle      component handle
     * @param[in]      in_callback    callback to use
     * @param[in]      in_data_len    length of in_data
     * @param[in]      in_data        data to be used with callback routine
     * @param[out]     out_esb        pointer to the returned esb
     * @param[out]     out_secbuf_sdata Secure buffer containing sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_ILLEGALLICENSE
     * @return ::ERR_NP_LICENSENOTSET
     * @return ::ERR_NP_CONTENTNOTSET
     * @return ::ERR_NP_ACTIONNOTGRANTED
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_Callback(hdls_context_t     io_handle,
                np_callback_t   *in_callback,
                np_size_t       in_data_len,
                u_int8_t        *in_data,
                np_secbuffer_t  *out_secbuf_esb,
                np_secbuffer_t  *out_secbuf_sdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief invoke Control.Link.Constraint.Describe
     *
     * @param[in] in_handle component handle
     * @param[in] in_domainid domain of the link to describe
     * @param[out] out_secbuf the buffer where description info will be stored
     * @param[out]    out_secbuf_sdata    Secure buffer containing sdata
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_SDATACORRUPTED
     * @return ::ERR_NP_NODESCRIPTION
     * @return ::ERR_NP_NOLINKREGISTERED
     * @return ::ERR_NP_ILLEGALLINK
     * @return ::ERR_NP_NOTSUPPORTED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_LinkConstraintDescribe(hdls_context_t    in_handle,
                              binstr_t         *in_domainid,
                              np_secbuffer_t   *io_secbufs,
                              np_secbuffer_t *out_secbuf_sdata);

    /**
     * @brief Get the start and end date/time for a links validity period
     *
     * This information is found in the ESB as the not_before and not_after date_time.
     *
     * @param[in]  in_from          The from-node: NULL for a personality. Non-NULL for a user node.
     * @param[in]  in_to            The to-node. Should be a user or subscription node. Cannot be NULL.
     * @param[out] out_period_start The valid start date of the link's period.
     * @param[out[ out_period_start The valid end date of the link's period.
     * @param[out] out_secbuf_sdata Secure buffer containing sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_SDATACORRUPTED
     * @return ::ERR_NP_NONODEREGISTERED
     * @return ::ERR_NP_NOLINKREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_ILLEGALLINK
     * @return ::ERR_NP_NODESCRIPTION
     * @return ::ERR_NP_ILLEGALESB
     */
    extern np_ret_t
    np_GetLinkPeriod(np_str_t       *in_from,
                     np_str_t       *in_to,
                     u_int32_t      *out_period_start,
                     u_int32_t      *out_period_end,
                     np_secbuffer_t *out_secbuf_sdata);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Re-encrypt the internal contentkeys with a new public key.
     *
     * @param[in] in_handle           component handle.
     * @param[in] in_cert_pubkey      The public key with which the contentkeys
     *                                need to be re-encrypted (in X509 format).
     * @param[out] out_nb_contentkeys The number of contentkeys that were
     *                                re-encrypted.
     * @param[out] out_contentkeys    The re-encrypted contentkeys.
     * @param[out] out_midkeys        Any midkeys when re-encrypting contentkeys. Each
     *                                key MAY be NULL.
     * @param[out] out_secretkeyids   The secretkey ids of the re-encrypted content
     *                                keys.
     * @param[out] out_contentkeyids  The contentkey ids of the re-encrypted content
     *                                keys.
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALHANDLE
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ReencryptCK(hdls_context_t     in_handle,
                   np_str_t          *in_cert_pubkey,
                   u_int32_t         *out_nb_contentkeys,
                   np_str_t        ***out_contentkeys,
                   np_str_t        ***out_midkeys,
                   np_str_t        ***out_secretkeyids,
                   np_str_t        ***out_contentkeyids);

#if defined( NPOPT_ENABLE_EXPORT_ACTION )
    /*---------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ExportCheck( hdls_context_t    in_handle,
                    np_exportmode_t   in_mode,
                    np_str_t         *in_target_type,
                    np_secbuffer_t   *out_esb );

    /*---------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ExportDescribe( hdls_context_t    in_handle,
                       np_exportmode_t   in_mode,
                       np_str_t         *in_target_type,
                       np_secbuffer_t   *out_esb );


    /*---------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ExportPerform( hdls_context_t    in_handle,
                      np_exportmode_t   in_mode,
                      np_str_t         *in_target_type,
                      np_secbuffer_t   *out_esb,
                      np_secbuffer_t   *out_sdata );

#endif

#ifdef __cplusplus
}
#endif

#endif /* NPDRM_API_H */
/** @} */
