/** @addtogroup npsmi npsmi
 *  @ingroup api
 * @{ */
/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

#ifndef NP_SMI_API_H
#define NP_SMI_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief This function will process an smi request message by extracting the
 *        parameters in the payload, and calling the intended secure function.
 *        It will also construct an smi response message, and marshall the
 *        results of the secure function call into it.
 *
 * @param[in]   in_sdata            Sdata buffer, in the case it needs to be
 *                                      updated, allowed to be NULL.
 * @param[in]   in_request_msg      A request message in smi format.
 * @param[out]  out_response_msg    A response message in smi format.
 * @param[out]  out_sdata           Sdata buffer, with new content after the
 *                                      secure call which needs to be written
 *                                      back to disk.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::smi_ExtractParamVals
 * @return ::smi_PrepareMsg
 */
extern np_ret_t
smi_sec_ProcMsg(np_str_t        *in_sdata,
                np_str_t        *in_request_msg,
                np_secbuffer_t  *out_response_msg,
                np_secbuffer_t  *out_sdata);

NP_END_EXTERN_C

#endif /* NP_SMI_API_H */
/** @} */
