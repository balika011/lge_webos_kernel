/** @addtogroup tr tr
 * @ingroup platform
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 */

#ifndef TR_API_H
#define TR_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initializes the Tamper Resistance layer
 *
 * Initialisation of Tamper Resistance layer. It should setup the TR driver and
 * call np_Init() in the secure part.
 *
 * @param[in] in_membody pointer to allocated memory for use by secure core, or NULL for automatic allocation.
 * @param[in] in_memsize size of memory
 *
 * @return ::ERR_NP_OK                  <br>Indicating Success
 * @return ::ERR_NP_OUTOFMEMORY         <br>A resource failure has occurred
 * @return ::ERR_NP_INTERNALERROR       <br>An unrecoverable error has occurred
 * @return depends of implementation    <br>For the implementations other than pc
 */
np_ret_t
tr_init(void *in_membody, np_size_t in_memsize);

/**
 * @brief Finalizes the Tamper Resistance Layer
 *
 * Finalization of Tamper Resistance.It should close the TR driver and
 * call np_Fin() in the secure part.
 *
 * @return ::ERR_NP_OK                  <br>Indicating Success
 * @return ::ERR_NP_NOTINITIALIZED      <br>Tr is not initialized yet
 * @return depends of implementation    <br>For the implementations other than pc
 */
np_ret_t
tr_fin(void);


/**
 * @brief Sends SMI message to secure core in a tamper-proof way and returns reply
 *
 * Exchanges data with secure core. Sends SMI message and secure storage content to the 
 * secure core in a tamper-proof way. Returns reply and updated data to store in secure storage
 *
 * @param[in] in_sdata          Sdata buffer, in the case it needs to be
 *                              updated, allowed to be NULL.
 * @param[in] in_req_msg Request Message
 * @param[out] out_resp_msg Response Message
 * @param[out] out_sdata Updated sdata
 *
 * @return ::ERR_NP_OK                  <br>Indicating Success
 * @return ::ERR_NP_ILLEGALARGS         <br>The input arguments are not valid. Arguments cannot be NULL
 * @return ::ERR_NP_OUTOFMEMORY         <br>A resource failure has occurred
 * @return ::ERR_NP_INTERNALERROR       <br>An unrecoverable error has occurred
 * @return ::ERR_NP_TOOSMALLBUFFER      <br>The target buffer is to small
 * @return ::smi_sec_ProcMsg            <br>The return error code form smi_sec_ProcMsg
 */
np_ret_t
tr_exec( np_str_t  *in_sdata,
         np_str_t  *in_req_msg,
         np_str_t  **out_resp_msg,
         np_str_t  **out_sdata);

/**
 * @brief Processes SMI message in secure world using secure data supplied
 *
 *
 * @param[in]   in_sdata            Sdata buffer, in the case it needs to be
 *                                  updated, allowed to be NULL.
 * @param[in]   in_request_msg      A request message in smi format.
 * @param[out]  out_response_msg    A response message in smi format.
 * @param[out]  out_sdata           Sdata buffer, with new content after the
 *                                  secure call which needs to be written
 *                                  back to disk.
 *
 * @return ::smi_sec_ProcMsg        <br>The return error code form smi_sec_ProcMsg
 */
np_ret_t
tr_ProcMsg(np_str_t        *in_sdata,
           np_str_t        *in_req_msg,
           np_secbuffer_t  *secbuf_resp,
           np_secbuffer_t  *secbuf_sdata);

NP_END_EXTERN_C

#endif
/** @} */
