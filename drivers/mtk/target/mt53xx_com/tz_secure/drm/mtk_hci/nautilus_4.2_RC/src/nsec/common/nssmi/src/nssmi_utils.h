/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 *
 */

#ifndef NSNP_SMI_COMMON_H
#define NSNP_SMI_COMMON_H

NP_BEGIN_EXTERN_C

/**
 * @brief Check that the target information for SMI messages is valid
 *
 * @param[in] in_tinfo        Target information
 * @param[in] in_msg_id       SMI message ID, some targets are invalid depending on the message
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::nssmi_l_CheckMsgIsAllowed
 */
extern np_ret_t
nssmi_l_ValidateTargetInfo( np_target_info_t *in_tinfo, smi_id_t in_msg_id );

/**
 * @brief Convert an internal error into an ERR_NP value
 *
 * @param[in] in_ret   Internal error
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALESB
 * @return ::ERR_NP_SDATACORRUPTED
 * @return ::ERR_NP_FILEERROR
 * @return ::ERR_NP_INTERNALERROR
 */
extern np_ret_t
nssmi_l_ConvErr( retcode_t in_ret );

/**
 * @brief Verify a message is allowed over remote transport
 *
 * @param[in] in_msg_id  SMI message ID
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 */
extern np_ret_t
nssmi_l_CheckMsgIsAllowed( smi_id_t in_msg_id );



/**
 * @brief This function is used by a (non-secure) gateway to issue a call to a
 *        secure core. This secure core can run in the same process as the
 *        gateway or in a separate device (or there can even be multiple secure
 *        cores in different locations at once, as with LTP).
 *
 * @param[in,out] io_target               The target information containing transport layer
 *                                          callbacks and connection info. If no callbacks
 *                                          are set the local transport will be used and the
 *                                          target structure updated.
 * @param[in]     in_msg_id               The id of the secure core function.
 * @param[in]     in_req_params_list      The list of parameters serving as
 *                                          input to the secure core function.
 * @param[out]    out_resp_params_list    The list of parameters serving as
 *                                          output of the secure core function.
 * @param[out]    out_resp_ret            The return code of the secure core
 *                                          function.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::smi_PrepareMsg
 * @return ::tl_SendRecv
 */
extern np_ret_t
nssmi_l_MsgSend( np_target_info_t  *in_target,
                 smi_id_t           in_msg_id,
                 smi_params_t      *in_req_params_list,
                 smi_params_t     **out_resp_params_list,
                 np_ret_t          *out_resp_ret );

/**
 * @brief This function is used by a (non-secure) gateway to issue a call to a
 *        secure core.
 *
 * @param[in]  in_handle       Component handle and SMI target info
 * @param[in]  in_size         Length of the key
 * @param[in]  in_buf          Buffer of the key
 * @param[in]  in_msg_id       The id of the secure core function.
 * @param[out] out_resp        The return code of the secure core function.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::smi_PrepareMsg
 * @return ::tl_SendRecv
 */
extern np_ret_t
nssmi_l_SendSmiString(np_handle_t in_handle,
                      np_size_t   in_size,
                      u_int8_t   *in_buf,
                      smi_id_t    in_msg_id,
                      binstr_t  **out_resp);

NP_END_EXTERN_C

#endif  /* NSNP_SMI_COMMON_H */
