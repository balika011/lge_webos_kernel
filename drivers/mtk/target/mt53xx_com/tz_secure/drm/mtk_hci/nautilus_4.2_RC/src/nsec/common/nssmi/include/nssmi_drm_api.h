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

#ifndef NSSMI_DRM_API_H
#define NSSMI_DRM_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initialized a license session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_InitSession
 */
extern np_ret_t
nssmi_InitSession(np_handle_t io_handle);

/**
 * @brief Finalizes the use of a license
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_FinSession
 */
extern np_ret_t
nssmi_FinSession(np_handle_t io_handle);

/**
 * @brief Sets the license to use in a DRM session
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_licensesize      Length of license buffer
 * @param[in] in_licensebuf       License buffer
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_SetLicense
 */
extern np_ret_t
nssmi_SetLicense(np_handle_t  in_handle,
                 np_size_t    in_licensesize,
                 u_int8_t    *in_licensebuf);

/**
 * @brief Checks if an action such as Play is authorized by the
 * license, and returns an ESB containing typically
 *            <ol>
 *            <li> obligations such as output control
 *            <li> Octopus call-backs that the application shall execute
 *                 at specific times.
 *            </ol>
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_actiontype       ::NP_ACTIONTYPE_PLAY etc
 * @param[out] out_esb             Extended status block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ActionCheck
 */
extern np_ret_t
nssmi_ActionCheck(np_handle_t  in_handle,
                  int32_t      in_actiontype,
                  np_esb_t   **out_esb);

/**
 * @brief Checks authorization, executes the license, passes the decryption key to the
 *        decryption engine and returns the same ESB as nsnp_ActionCheck
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_actiontype       ::NP_ACTIONTYPE_PLAY etc
 * @param[out] out_esb             Extended status block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ActionPerform
 */
extern np_ret_t
nssmi_ActionPerform(np_handle_t  in_handle,
                    int32_t      in_actiontype,
                    np_esb_t   **out_esb);

/**
 * @brief Returns explanations of the rules in the license for a specific action.
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_actiontype       ::NP_ACTIONTYPE_PLAY etc
 * @param[out] out_esb             Extended status block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ActionDescribe
 */
extern np_ret_t
nssmi_ActionDescribe(np_handle_t  in_handle,
                     int32_t      in_actiontype,
                     np_esb_t   **out_esb);

/**
 * @brief Returns a description of the play-back rules in a license. Equivalent to calling
 *        nssmi_ActionDescribe with NP_ACTIONTYPE_PLAY. This function returns the ESB in
 *        serialized format, not as an ESB
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[out] out_esbinfo         Serialized Extended status block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ActionDescribe
 */
np_ret_t
nssmi_ActionDescribePlay(np_handle_t in_handle,
                         binstr_t  **out_esbinfo);

/**
 * @brief Executes and Octopus callback with the supplied arguments and returns its ESB
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_callback         octopus callback to execute
 * @param[in]  in_callback_args    octopus callback arguments, or NULL
 * @param[out] out_esbinfo         Returned ESB or NULL
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_Callback
 */
extern np_ret_t
nssmi_Callback(np_handle_t          in_handle,
               np_callback_t       *in_callback,
               np_callback_args_t  *in_callback_args,
               np_esb_t           **out_esbinfo);

/**
 * @brief Returns an ESB containing the constraints (such as validity
 *        period) in an Octopus link selected by a domain ID
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_domainid         Domain ID
 * @param[out] out_esbinfo         Extended status block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_LinkConstraintDescribe
 */
extern np_ret_t
nssmi_LinkConstraintDescribe(np_handle_t   in_handle,
                             binstr_t     *in_domainid,
                             np_esb_t    **out_esbinfo);

/**
 * @brief Checks if export is authorized and returns an ESB
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_mode             Copy, Move, Render, DontKnow
 * @param[in]  in_target_type      DTCP, VCPS etc
 * @param[out] out_esb             Extended Status Block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ExportCheck
 */
extern np_ret_t
nssmi_ExportCheck(np_handle_t       in_handle,
                  np_exportmode_t   in_mode,
                  binstr_t         *in_target_type,
                  np_esb_t        **out_esb);

/**
 * @brief Describes the rules in the license for exporting the content key to a foreign system in the given
 *        export mode.
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_mode             Copy, Move, Render, DontKnow
 * @param[in]  in_target_type      DTCP, VCPS etc
 * @param[out] out_esb             Extended Status Block
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ExportDescribe
 */
extern np_ret_t
nssmi_ExportDescribe(np_handle_t       in_handle,
                     np_exportmode_t   in_mode,
                     binstr_t         *in_target_type,
                     np_esb_t        **out_esb);

/**
 * @brief Checks authorization, “executes” the export and returns ESB
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_mode             Copy, Move, Render, DontKnow
 * @param[in]  in_target_type      DTCP, VCPS etc
 * @param[out] out_esb             Extended Status Block
 *
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ExportPerform
 */
extern np_ret_t
nssmi_ExportPerform(np_handle_t       in_handle,
                    np_exportmode_t   in_mode,
                    binstr_t         *in_target_type,
                    np_esb_t        **out_esb);

/**
 * @brief Get validity period for a link
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_from             From node ID
 * @param[in]  in_to               To node ID
 * @param[out] out_period_start    Begin validity
 * @param[out] out_period_end      End validity
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_GetLinkPeriod
 */
np_ret_t
nssmi_GetLinkPeriod(np_handle_t  in_handle,
                    binstr_t    *in_from,
                    binstr_t    *in_to,
                    u_int32_t   *out_period_start,
                    u_int32_t   *out_period_end);

NP_END_EXTERN_C

#endif /* NSSMI_DRM_API_H */


/** @} */
