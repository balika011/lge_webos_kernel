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

#ifndef NSSMI_MS3_API_H
#define NSSMI_MS3_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initializes a MS3 session
 *
 * @param[in] io_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npms3_InitSession
 */
extern np_ret_t
nssmi_InitMS3Session(np_handle_t io_handle);

/**
 * @brief Finalizes the MS3 session
 *
 * @param[in] io_handle           @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::npms3_FinSession
 */
extern np_ret_t
nssmi_FinMS3Session(np_handle_t io_handle);

/**
 * @brief Retrieve NEMO signing certificate and associated private key.
 *
 * @param[in]   in_handle            @ref example_smi "SMI" target info
 * @param[in]   in_application_data  Opaque application data. Maybe
 *                                   used to protect NEMO keys.
 * @param[out]  out_nemo_cert        NEMO signing certificate path (X509 PKI path, DER)
 * @param[out]  out_nemo_prv         NEMO private key of signing certificate (PKCS8, DER)
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALHANDLE             <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_ILLEGALARGS               <br>NULL was specified in out params
 * @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory
 * @return ::ERR_NP_SMI_MSG_CORRUPTED         <br>SMI communication failed.
 */
extern np_ret_t
nssmi_GetNemoKeys(np_handle_t         in_handle,
                  binstr_t           *in_application_data,
                  binstr_t          **out_nemo_cert,
                  binstr_t          **out_nemo_prv);

/**
 * @brief This function is used to process Parse SAS request
 * Sec layer will parse the SAS and returns the result structure.
 *
 * @param[in]   in_handle            @ref example_smi "SMI" target info
 * @param[in]   in_sas               sas data from nsec level
 * @param[in]   in_surl              s-url
 * @param[in]   in_curit             c-urit.
 * @param[in]   in_application_data  opaque application data. Maybe
 *                                   used to protect SAS and NEMO keys
 * @param[out]  out_sas_result       sas output result structure.
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALARGS               <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory
 * @return ::ERR_NP_SMI_MSG_CORRUPTED         <br>SMI communication failed.
 */
extern np_ret_t
nssmi_ParseSAS(np_handle_t         in_handle,
               binstr_t           *in_sas,
               binstr_t           *in_surl,
               binstr_t           *in_curit,
               binstr_t           *in_application_data,
               np_ms3_sas_info_t **out_sas_result);

/**
 * @brief This function is used to Notify that SAS information is valid and understood by the
 *        Application
 *
 * @param[in]   in_handle            @ref example_smi "SMI" target info
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALHANDLE             <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SASALREADYPERFORMED       <br>indicates that nssmi_SasInfoProcessingComplete is already called
 * @return ::ERR_NP_NOSASFOUND                <br>indicates that no sas is set in the context
 */
extern np_ret_t
nssmi_SasInfoProcessingComplete(np_handle_t in_handle);

/**
 * @brief This function is used to expand the curit
 * Sec layer will expand the curit using the authenticator in ms3 context and returns curl.
 *
 * @param[in]   in_handle            @ref example_smi "SMI" target info
 * @param[in]   in_curit             c-urit
 * @param[out]  out_curl             expanded curl
 *
 * @return ::ERR_NP_OK                        <br>Indicating success
 * @return ::ERR_NP_ILLEGALHANDLE             <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_ILLEGALARGS               <br>NULL was specified in in_curit or out_curl
 * @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALCURIT              <br>Illegal curit
 * @return ::ERR_NP_ILLEGALCOMPOUNDURI        <br>Illegal compound uri
 * @return ::ERR_NP_NOSASFOUND                <br>Unable to find authenticator/surl in ms3 context. i.e SAS is not set
 */
extern np_ret_t
nssmi_CuritExpand(np_handle_t     in_handle,
                  binstr_t       *in_curit,
                  binstr_t      **out_curl);

NP_END_EXTERN_C

#endif /* NSSMI_MS3_API_H */


/** @} */
