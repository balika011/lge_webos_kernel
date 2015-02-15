/** @addtogroup nppers nppers
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010 Sony Corporation.
 */

#ifndef NPPERS_API_H
#define NPPERS_API_H

NP_BEGIN_EXTERN_C

#if defined(NPOPT_USE_NPPERS)

/*---------------------------------------------------------------------------*/
/**
 * @brief Initialize Personalization handle
 *
 * @param[out] out_handle buffer for handle
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nppers_InitHandle(hdls_context_t *out_handle);

/*---------------------------------------------------------------------------*/
/**
 * @brief Finalize Personalization handle
 *
 * @param[in] in_hdls handle
 * @return ::ERR_NP_OK
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nppers_FinHandle(hdls_context_t in_hdls);

/*---------------------------------------------------------------------------*/
/**
 * @brief Marlin NEMO Personalization service
 *   Step1: generate initiatePersonalityAcquisitionRequest
 *   Step2: parse initiatePersonalityAcquisitionResponse and
 *          generate completePersonalityAcquisitionRequest
 *   Step3: parse completePersonalityAcquisitionResponse and register Personalization-personality
 *
 * @param[in]     in_hdls       Personalization handle
 * @param[in]     in_response   response message to be parsed
 * @param[out]    out_secbuf    Step1/Step2: buffer for generated request message
 *                              Step3: pointer to secure buffer
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/**
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nppers_NEMOPersonalizationStep1(hdls_context_t  in_hdls,
                                np_secbuffer_t *out_secbuf);

/*---------------------------------------------------------------------------*/
/**
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nppers_NEMOPersonalizationStep2(hdls_context_t  in_hdls,
                                np_str_t       *in_response,
                                np_secbuffer_t *out_secbuf);

/*---------------------------------------------------------------------------*/
/**
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_MESSAGECORRUPTED      (response is corrupted)
 * @return ::ERR_NP_ALREADYREGISTERED     (NEMO personality is already registered)
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nppers_NEMOPersonalizationStep3(hdls_context_t  in_hdls,
                                np_str_t       *in_response,
                                np_secbuffer_t *out_secbuf);

/* -------------------------------------------------------------------------- */
/**
 * @brief Set key type to the personalization context
 *
 * @param[in] in_hdls            Handle for personalization session
 * @param[in] in_keytype         Key type
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOTSUPPORTED
 *
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nppers_SetKeyType(hdls_context_t       in_hdls,
                  np_pers_keytype_t    in_keytype);

/* -------------------------------------------------------------------------- */
/**
 * @brief Set business token  to the personalization context
 *
 * @param[in] in_hdls                   Handle for personalization session
 * @param[in] in_business_token         Business token
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOTSUPPORTED
 *
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nppers_SetBusinessToken(hdls_context_t  in_hdls,
                        np_str_t       *in_business_token);


#endif /* defined(NPOPT_USE_NPPERS) */

NP_END_EXTERN_C

#endif /* NPPERS_API_H */
/** @} */
