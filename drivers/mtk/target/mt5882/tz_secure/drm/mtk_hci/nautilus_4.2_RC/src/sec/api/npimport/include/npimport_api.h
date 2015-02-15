/** @addtogroup npimport npimport
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef NPIMPORT_API_H
#define NPIMPORT_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file npimport_api.h
 *
 * @brief secure API layer for the License Import features
 *
 *
 * @date   11 May 2011 Initial writing
 */
/*===========================================================================*/

/**
 * @brief  Create a secure npimport session/context
 *
 * @param[out]  out_import_handle_ctx    An allocated and initialized secure Import context
 *
 * @return ::ERR_NP_OK             <br>Indicating success.
 * @return ::ERR_NP_ILLEGALARGS    <br>The arguments to the function are incorrect.
 * @return ::ERR_NP_OUTOFMEMORY    <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_INTERNALERROR  <br>An unrecoverable failure occurred in Nautilus.
 * @return ::hdls_InitHdlMap       <br>Errors returned by sub-functions
 * @return ::hdls_MapHdlToIdx      <br>Errors returned by sub-functions
 */
extern np_ret_t
npimport_InitSession(hdls_context_t   *out_import_handle_ctx);


/**
 * @brief Finalise and destroy secure npimport session/context
 *
 * @param[in] in_import_handle_ctx The secure npimport context to remove
 *
 * @return ::ERR_NP_OK             <br>Indicating success.
 * @return ::ERR_NP_ILLEGALHANDLE  <br>Wrong state or context
 * @return ::ERR_NP_OUTOFMEMORY    <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_INTERNALERROR  <br>An unrecoverable failure occurred in Nautilus.
 * @return ::hdls_MapIdxToHdl      <br>Errors returned by sub-functions
 * @return ::hdls_SetHdlMapping    <br>Errors returned by sub-functions
 */
extern np_ret_t
npimport_FinSession(hdls_context_t in_import_handle_ctx);

/**
 * @brief Check if stored BKB is up-to-date
 *
 * @param[in]  in_import_handle_ctx  Secure import context that contains the octopus objects that
 *                                   are bundled into a Marlin license
 * @param[out] out_is_bkb_fresh      Value indicates if stored BKB is up-to-date
 *
 * @praam[out] out_secbuf_sdata      sdata
 *
 * @return ::ERR_NP_OK               <br>Indicating success.
 * @return ::ERR_NP_ILLEGALHANDLE    <br>Wrong state or context
 * @return ::ERR_NP_OUTOFMEMORY      <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_INTERNALERROR    <br>An unrecoverable failure occurred in Nautilus.
 * @return ::hdls_MapIdxToHdl        <br>Errors returned by sub-functions
 * @return ::hdls_SetHdlMapping      <br>Errors returned by sub-functions
 * @return ::sf_GetBKBInfo           <br>Errors returned by sub-functions
 */
extern np_ret_t
npimport_CheckBKB(hdls_context_t  in_import_handle_ctx,
                  np_bool_t      *out_is_bkb_fresh,
                  np_secbuffer_t *out_secbuf_sdata);
/**
 * @brief Generate a Marlin License
 *
 * @param[in]  in_import_handle_ctx    Secure import context that contains the octopus objects that
 *                                     are bundled into a Marlin license
 * @param[in]  in_content_descriptor   Import Descriptor
 * @param[out] out_secbuf_license      Generated license
 * @param[out] out_secbuf_sdata        sdata
 *
 * @return ::ERR_NP_OK                      <br>Indicating success.
 * @return ::ERR_NP_ILLEGALHANDLE           <br>import context is is wrong state (e.g. license was already generated once)
 * @return ::ERR_NP_ILLEGALARGS             <br>One of the given arguments is wrong.
 * @return ::ERR_NP_OUTOFMEMORY             <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_INTERNALERROR           <br>An unrecoverable failure occurred in Nautilus.
 * @return ::ERR_NP_IMPORT_PERSONALITYERROR <br>Import agent personality is missing or wrong (cannot sign license)
 * @return ::ERR_NP_IMPORT_INVALID_DESC     <br>Invalid import descriptor
 * @return ::ERR_NP_IMPORT_NO_CKS_FOUND     <br>No content keys were found
 * @return ::ERR_NP_NOPERSONALITYREGISTERED <br>cannot bind license to device because personality is missing
 * @return ::hdls_MapIdxToHdl               <br>Errors returned by sub-functions
 * @return ::hdls_SetHdlMapping             <br>Errors returned by sub-functions
 * @return ::import_GenerateLicense         <br>Errors returned by sub-functions
 */
extern np_ret_t
npimport_GenerateLicense(hdls_context_t    in_import_handle_ctx,
                         np_import_desc_t *in_content_descriptor,
                         np_secbuffer_t   *out_secbuf_license,
                         np_secbuffer_t   *out_secbuf_sdata);

/**
 * @brief Free a Content Descriptor
 *
 * @param[io]  in_content_descriptor   Import Descriptor
 *
 */
extern void
npimport_FreeContentDescriptor(np_import_desc_t  *io_content_descriptor);

/**
 * @brief Store BKB container
 *
 * @param[in]   in_import_handle_ctx Secure import context that contains the octopus objects that
 *                                   are bundled into a Marlin license
 * @param[in]  in_bkbcontainer       BKB container
 * @param[in]  in_pv                 Application data
 * @praam[out] out_secbuf_sdata      sdata
 *
 * @return ::ERR_NP_OK               <br>Indicating success.
 * @return ::ERR_NP_OUTOFMEMORY      <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_ILLEGALHANDLE    <br>Wrong state or context
 * @return ::ERR_NP_INTERNALERROR    <br>An unrecoverable failure occurred in Nautilus.
 * @return ::hdls_MapIdxToHdl        <br>Errors returned by sub-functions
 * @return ::hdls_SetHdlMapping      <br>Errors returned by sub-functions
 * @return ::sf_UpdateBKBContainer   <br>Errors returned by sub-functions
 */
extern np_ret_t
npimport_StoreBKBContainer(hdls_context_t  in_import_handle_ctx,
                           binstr_t       *in_bkbcontainer,
                           binstr_t       *in_pv,
                           np_secbuffer_t *out_secbuf_sdata);


/**
 * @brief Returns the License Issuing Service Key(LISK) subject.
 *
 * @param[in]  in_import_handle_ctx  Secure import context that contains the octopus objects that
 *                                   are bundled into a Marlin license
 * @param[out] out_secbuf_lisk       License Issuing Service Key (LISK) subject
 *
 * @return ::ERR_NP_OK                      <br>Indicating success.
 * @return ::ERR_NP_OUTOFMEMORY             <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_ILLEGALHANDLE           <br>Wrong state or context
 * @return ::ERR_NP_INTERNALERROR           <br>An unrecoverable failure occurred in Nautilus.
 * @return ::ERR_NP_IMPORT_PERSONALITYERROR <br> Import agent personality is missing or wrong
 * @return ::hdls_MapIdxToHdl               <br>Errors returned by sub-functions
 * @return ::hdls_SetHdlMapping             <br>Errors returned by sub-functions
 */
extern np_ret_t
npimport_GetLISKSubject(hdls_context_t  in_import_handle_ctx,
                        np_secbuffer_t *out_secbuf_lisk);


/**
 * @brief Generate a SAC OCI_CK import request (import service)
 *
 * @param in_msg_type           The message type
 * @param in_serv_hdl_type      The service handle type
 * @param in_import_handle_ctx  The service handle
 * @param in_msg_info           The service info structure
 * @param out_request           The generated request
 *
 * @return ::ERR_NP_ILLEGALHANDLE Bad handle
 * @return ::ERR_NP_ILLEGALARGS Bad arguments
 * @return ::ERR_NP_SAC_SERV_MSG Service message was not created
 * @return ::ERR_NP_OUTOFMEMORY Resource problem
 *
 * @note in_msg_info contains oci_data_type, server_content_id and
 *       client_content_id in that specific order
 */
extern np_ret_t
npimport_sacGenerateRequest(np_sac_msg_t         in_msg_type,
                            np_sac_handle_type_t in_serv_hdl_type,
                            hdls_context_t       in_import_handle_ctx,
                            np_sac_msg_info_t   *in_msg_info,
                            np_str_t           **out_request);

/**
 * @brief Process the SAC request for the import service (Server function)
 * @note Not supported in Nautilus 4.2
 *
 * @param in_msg_type           The message type
 * @param in_serv_hdl_type      The service handle type
 * @param in_import_handle_ctx  The service handle
 * @param in_request            The request to process (@see npimport_sacGenerateRequest)
 * @param out_response          The created response
 *
 * @return ::ERR_NP_NOTSUPPORTED
 */
extern np_ret_t
npimport_sacProcessRequest(np_sac_msg_t         in_msg_type,
                           np_sac_handle_type_t in_serv_hdl_type,
                           hdls_context_t       in_import_handle_ctx,
                           np_str_t            *in_request,
                           np_str_t           **out_response);

/**
 * @brief Parse the response coming from the server (for the import service)
 *
 * @param in_msg_type           The message type
 * @param in_serv_hdl_type      The service handle type
 * @param in_import_handle_ctx  The service handle
 * @param in_response           The response
 * @param in_smid               The global smid
 *
 * @return ::ERR_NP_ILLEGALHANDLE    Bad handle
 * @return ::ERR_NP_MESSAGECORRUPTED in_response corrupt
 * @return ::ERR_NP_INTERNALERROR    Generic error
 * @return ::ERR_NP_ILLEGALARGS      Bad arguments
 * @return ::ERR_NP_SAC_SERV_MSG     Service message was not created
 * @return ::ERR_NP_NOTSUPPORTED     The response has fields which are not understood/supported
 * @return ::ERR_NP_OUTOFMEMORY      Resource problem
 */
extern np_ret_t
npimport_sacProcessResponse(np_sac_msg_t         in_msg_type,
                            np_sac_handle_type_t in_serv_hdl_type,
                            hdls_context_t       in_import_handle_ctx,
                            np_str_t            *in_response,
                            u_int32_t            in_smid);

NP_END_EXTERN_C

#endif /* NPIMPORT_API_H */
/** @} */
