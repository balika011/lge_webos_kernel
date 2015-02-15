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

#ifndef NSSMI_PERS_API_H
#define NSSMI_PERS_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initializes a nppers session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_InitHandle
 */
extern np_ret_t
nssmi_InitPersHandle(np_handle_t io_handle);

/**
 * @brief Finalizes a nppers session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_FinHandle
 */
extern np_ret_t
nssmi_FinPersHandle(np_handle_t io_handle);

/**
 * @brief Generates the first nppers request message
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[out] out_request         Generated request
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_NEMOPersonalizationStep1
 */
extern np_ret_t
nssmi_NEMOPersonalizationStep1(np_handle_t in_handle,
                               binstr_t  **out_request);

/**
 * @brief Processes the first nppers reply and generates the second nppers request
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_response         Reply to first request
 * @param[out] out_request         Second request
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_NEMOPersonalizationStep2
 */
extern np_ret_t
nssmi_NEMOPersonalizationStep2(np_handle_t in_handle,
                               binstr_t *in_response,
                               binstr_t **out_request);

/**
 * @brief Processs the second npppers reply
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param]in] in_response         Second reply
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_NEMOPersonalizationStep3
 */
extern np_ret_t
nssmi_NEMOPersonalizationStep3(np_handle_t in_handle,
                               binstr_t *in_response);

/**
 * @brief Set key type to the personalization context
 *
 * @param[in] in_handle         Handle for personalization session
 * @param[in] in_keytype        Key type
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_SetKeyType
 */
extern np_ret_t
nssmi_SetPersKeyType(np_handle_t          in_handle,
                     np_pers_keytype_t    in_keytype);


/**
 * @brief Set business token to the personalization context
 *
 * @param[in] in_handle          Handle for personalization session
 * @param[in] in_business_token  Business token
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::nppers_SetBusinessToken
 */
extern np_ret_t
nssmi_SetPersBusinessToken(np_handle_t            in_handle,
                           binstr_t              *in_business_token);

NP_END_EXTERN_C

#endif /* NSSMI_PERS_API_H */


/** @} */
