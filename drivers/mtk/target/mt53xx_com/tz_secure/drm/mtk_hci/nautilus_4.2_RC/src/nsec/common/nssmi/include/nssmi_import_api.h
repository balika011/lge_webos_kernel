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

#ifndef NSSMI_IMPORT_API_H
#define NSSMI_IMPORT_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Processes a Import session creation with passed import descriptor
 *
 * @param[in,out] io_import_handle       Handle for the import session
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npimport_InitSession
 */
extern np_ret_t
nssmi_importInitSession(np_handle_t       io_import_handle);

/**
 * @brief Processes a Import sesion's finalization
 *
 * @param[in] io_import_handle           Handle for the import session
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npimport_FinSession
 */
extern np_ret_t
nssmi_importFinSession(np_handle_t   io_import_handle);

/**
 * @brief Check whether BKB is up-to-date
 *
 * @param[in]  in_import_handle            Handle for the import session
 * @param[out] out_is_bkb_fresh            value indicates whether BKB is up-to-date
 *
 * @return ::ERR_NP_OK                     <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY            <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS            <br>NULL was specified in out_is_bkb_fresh
 * @return ::ERR_NP_INTERNALERROR          <br>An unrecoverable error has occurred in Nautilus.
 * @return ::ERR_NP_ILLEGALHANDLE          <br>invalid handle
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER  <br>This API is only supported for the local transport layer
 * @return ::ERR_NP_SMI_MSG_CORRUPTED      <br>SMI communication failed.
 * @return ::npimport_CheckBKB
 */
extern np_ret_t
nssmi_CheckBKB(np_handle_t    in_import_handle,
               np_bool_t     *out_is_bkb_fresh);

/**
 * @brief Once the content has been imported generate a license for it.
 *
 * In the case of binding to a domain/user node it is assumed that those nodes
 * have been acquired outside of the import service using either a) a service
 * or b) are made available (in sdata) from a local registration service
 *
 * @note generate license can only successfully generate one license per session
 *
 * @note in the future setter APIs may be added to specific properties of the
 *       generated license. For example we may control the actions in the
 *       license, the export targets etc. These setter APIs must all be called
 *       prior to the nsnp_importGenerateLicense call.
 *
 * @note By default all generated license will bound to the device personality
 *       and will only support the Play action
 *
 * @param[in]     in_import_handle      Handle for the import session
 * @param[in]     in_content_descriptor Import Descriptor
 * @param[in]     in_application_data_len Length of the application data
 * @param[in]     in_application_data  Opaque application data. Passed to the
 *                                     https callbacks and hci for protecting
 *                                     data exchanged between them
 * @param[in]     in_bkb_callbacks     Callback to use for acquiring updating
 *                                     the bk and bkb
 * @param[out]    out_license          Generated Marlin license
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALHANDLE             <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_ILLEGALARGS               <br>NULL was specified in in_curit or out_curl
 * @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER     <br>This API is only supported for the local transport layer
 * @return ::ERR_NP_SMI_MSG_CORRUPTED         <br>SMI communication failed.
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nssmi_importGenerateLicense(np_handle_t           in_import_handle,
                            np_import_desc_t     *in_content_descriptor,
                            binstr_t             *in_application_data,
                            binstr_t            **out_license);

/**
 * @brief Returns the License Issuing Service Key(LISK) subject.
 *
 * @param[in]  in_import_handle   Handle for the import session
 * @param[out] out_lisk_subject   License Issuing Service Key (LISK) subject
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALHANDLE             <br>Invalid handle
 * @return ::ERR_NP_ILLEGALARGS               <br>NULL was specified in in_bkbcontainer
 * @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory
 * @return ::ERR_NP_IMPORT_PERSONALITYERROR   <br>Illegal or missing import personality
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER     <br>This API is only supported for the local transport layer
 * @return ::ERR_NP_SMI_MSG_CORRUPTED         <br>SMI communication failed.
 * @return ::npimport_GetLISK
 */
extern np_ret_t
nssmi_importGetLISKSubject(np_handle_t   in_import_handle,
                           binstr_t    **out_lisk_subject);

/**
 * @brief Store BKB container
 *
 * @param[in] in_import_handle                Handle for the import session
 * @param[in] in_bkbcontainer                 BKB container
 * @param[in] in_application_data             Application data
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALHANDLE             <br>invalid handle
 * @return ::ERR_NP_ILLEGALARGS               <br>NULL was specified in in_bkbcontainer
 * @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER     <br>This API is only supported for the local transport layer
 * @return ::ERR_NP_SMI_MSG_CORRUPTED         <br>SMI communication failed.
 * @return ::npimport_StoreBKBContainer
 */
extern np_ret_t
nssmi_StoreBKBContainer(np_handle_t    in_import_handle,
                        binstr_t      *in_bkbcontainer,
                        binstr_t      *in_application_data);

NP_END_EXTERN_C

#endif /* NSSMI_IMPORT_API_H */


/** @} */
