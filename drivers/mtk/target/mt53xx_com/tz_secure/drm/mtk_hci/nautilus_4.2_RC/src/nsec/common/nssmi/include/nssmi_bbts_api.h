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

#ifndef NSSMI_BBTS_API_H
#define NSSMI_BBTS_API_H

NP_BEGIN_EXTERN_C

extern np_ret_t
nssmi_bbtsInitSession(np_moc_t         in_default_basic_moc,
                      np_moc_t         in_default_dtcp_moc,
                      np_bbts_mode_t   in_bbts_mode,
                      np_handle_t      io_bbts_handle);

/**
 * @brief Processes a BBTS sesion's finalization
 *
 * @param[in] io_bbts_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npbbts_FinSession
 */
extern np_ret_t
nssmi_bbtsFinSession(np_handle_t   io_bbts_handle);

/**
 * @brief Overload and provide an Application specific socID to use
 *
 * @param[in] in_bbts_handle           Handle for the session
 * @param[in] in_socid                 socID
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npbbts_SetSocID
 */
extern np_ret_t
nssmi_bbtsSetSocID(np_handle_t   in_bbts_handle,
                   binstr_t     *in_socid );

/**
 * @brief Initiate a ECM stream with the bbts session. The caller can check
 *        that it has a license of the content id by parsing its licenses using
 *        nsnp_GetLicenseMetaData().
 *
 * @param[in]      in_bbts_handle       Handle for the bbts session
 * @param[in]      in_ecm               ECM to initiate the stream with
 * @param[in]      in_servicebase_cid   ServiceBaseCID
 * @param[out]     out_serv_content_id  Service content id
 * @param[out]     out_prog_content_id  Program content id
 *
 * @return ::ERR_NP_OK                 <br>Indicating success.
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The handle passed to the function is invalid.
 * @return ::ERR_NP_OUTOFMEMORY        <br>Nautilus encountered resource problems.
 * @return ::ERR_NP_ILLEGALARGS        <br>The arguments to the function are incorrect.
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::ERR_NP_BBTS_INVALID_ECM   <br>Invalid ECM.
 * @return ::npbbts_InitECMStream
 */
extern np_ret_t
nssmi_bbtsInitECMStream(np_handle_t    in_bbts_handle,
                        binstr_t      *in_ecm,
                        binstr_t      *in_servicebase_cid,
                        binstr_t     **out_serv_content_id,
                        binstr_t     **out_prog_content_id);


NP_END_EXTERN_C

#endif /* NSSMI_BBTS_API_H */


/** @} */
