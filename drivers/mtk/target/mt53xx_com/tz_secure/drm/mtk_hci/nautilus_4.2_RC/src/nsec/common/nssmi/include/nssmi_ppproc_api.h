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

#ifndef NSSMI_PPPROC_API_H
#define NSSMI_PPPRCO_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initialize a ppproc session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ppproc_Init
 */
np_ret_t
nssmi_PpprocInit(np_handle_t io_handle);

/**
 * @brief Finalize a ppproc session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ppproc_Fin
 */
np_ret_t
nssmi_PpprocFin(np_handle_t io_handle);

/**
 * @brief Generate a ppproc request
 *
 * @param[in]     in_handle           @ref example_smi "SMI" target info
 * @param[in,out] io_len              Length of output buffer before and after generation
 * @param[out]    out_buf             Output buffer for request
 * @param[in]     in_usage            Reserved
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ppproc_GenerateRequest
 */
/* Response Payload */
np_ret_t
nssmi_PpprocGenerateRequest(np_handle_t      io_handle,
                            u_int32_t       *io_len,
                            u_int8_t        *out_buf,
                            u_int32_t        in_usage);

/**
 * @brief Processes a ppproc response
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_response         Response message
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ppproc_ParseResponse
 */
np_ret_t
nssmi_PpprocParseResponse(np_handle_t   io_handle, binstr_t *in_response);

NP_END_EXTERN_C

#endif /* NSSMI_PPPROC_API_H */


/** @} */
