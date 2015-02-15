/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

/** @addtogroup nsnp_import functionality
 *  @ingroup nsnp
 * @{
 *
 * @brief nsnp layer APIs for Import
 *
 * @see NautilusAPIReference.pdf
 *
 * @file
 * Import APIs
 *
 */

#ifndef NSNP_IMPORT_API_H
#define NSNP_IMPORT_API_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                             PUBLIC Structs                                 */
/* -------------------------------------------------------------------------- */

/**
 * BKB callback type
 *
 * Type for acquiring a bkb.
 *
 **/
typedef struct {
    /** Opaque data to return to the caller in order to track the outstanding
     *  requests for bkb's */
    void        *opaque;
    /** URL to request the bkb from */
    const char  *url;
    /** Get a bkb from the supplied url. Return 0 for success and !0 for failure  */
    int32_t (*cb_get_bkb)(void        *in_opaque,
                          const char  *in_url,
                          np_size_t    in_app_data_len,
                          u_int8_t    *in_app_data_buf,
                          np_size_t    in_crl_len,
                          u_int8_t    *in_crl_buf,
                          np_size_t   *out_bkb_len,
                          u_int8_t   **out_bkb_buf);
    /** Free the bkb data using the supplied opaque pointer. Return 0 for success and !0 for failure */
    int32_t (*cb_free_bkb)(void       *in_opaque,
                           u_int8_t   *in_bkb_buf);
} np_bkb_callbacks_t;

/**
 * CRL callback type
 *
 * Type for acquiring a crl using http which is then used for acquiring a bkb
 *
 **/
typedef struct {
    /** Opaque data to return to the caller in order to track the outstanding
     *  requests for crl's */
    void        *opaque;
    /** URL to request the crl from */
    const char  *url;
    /** Get a crl from the supplied url. Return 0 for success and !0 for failure  */
    int32_t (*cb_get_crl)(void        *in_opaque,
                          const char  *in_url,
                          u_int8_t   **out_crl_buf,
                          np_size_t   *out_crl_len);
    /** Free the crl data using the supplied opaque pointer. Return 0 for
     *  success and !0 for failure */
    int32_t (*cb_free_crl)(void       *in_opaque,
                           u_int8_t   *in_crl_buf);
} np_crl_callbacks_t;

/* -------------------------------------------------------------------------- */
/*                             PUBLIC API                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/**
 * @brief Create an import session
 *
 * @param[in,out] io_import_handle        Handle for the session
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_importInitSession(np_handle_t  io_import_handle);

/* -------------------------------------------------------------------------- */
/**
 * @brief Finalise an import session
 *
 * @param[in,out] io_import_handle      Handle for the session to close
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_importFinSession(np_handle_t   io_import_handle);

/* -------------------------------------------------------------------------- */
/**
 * @brief Once the content has been imported generate a license for it.
 *
 * In the case of binding to a domain/user node it is assumed that those nodes
 * have been acquired outside of the import service using either a) a service
 * or b) are made available (in sdata) from a local registration service
 *
 * @note generate license can only successfully generate one license
 *
 * @note in the future setter APIs may be added to specific properties of the
 *       generated license. For example we may control the actions in the
 *       license, the export targets etc. These setter APIs must all be called
 *       prior to the nsnp_importGenerateLicense call.
 *
 * @param[in]     in_import_handle     Handle for the import session
 * @param[in]     in_content_description  Description of content being imported
 * @param[in]     in_application_data_len Length of the application data
 * @param[in]     in_application_data  Opaque application data. Passed to the
 *                                     https callbacks and hci for protecting
 *                                     data exchanged between them
 * @param[in]     in_bkb_callbacks     Callback to use for acquiring updating
 *                                     the bk and bkb
 * @param[in]     in_crl_callbacks     Callback to use for acquiring crl data
 * @param[out]    out_license          Generated Marlin license
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_IMPORT_NO_CKS_FOUND
 * @return ::ERR_NP_IMPORT_PERSONALITYERROR
 * @return ::ERR_NP_IMPORT_INVALID_DESC
 * @return ::ERR_NP_NOPERSONALITYREGISTERED
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_IMPORT_DEVICEIDTOOLONG
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_importGenerateLicense(np_handle_t           in_import_handle,
                           np_import_desc_t     *in_content_description,
                           np_size_t             in_application_data_len,
                           u_int8_t             *in_application_data,
                           np_bkb_callbacks_t   *in_bkb_callbacks,
                           np_crl_callbacks_t   *in_crl_callbacks,
                           np_str_t            **out_license);


/* -------------------------------------------------------------------------- */
/**
 * @brief Returns the License Issuing Service Key(LISK) uid.
 *
 * @param[in]   in_import_handle  Handle for the import session
 * @param[out]  out_lisk_subject  License Issuing Service Key(LISK) subject
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_IMPORT_PERSONALITYERROR
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_importGetLISKSubject(np_handle_t   in_import_handle,
                          np_str_t    **out_lisk_subject);

NP_END_EXTERN_C

#endif /* NSNP_IMPORT_API_H */
/** @} */
