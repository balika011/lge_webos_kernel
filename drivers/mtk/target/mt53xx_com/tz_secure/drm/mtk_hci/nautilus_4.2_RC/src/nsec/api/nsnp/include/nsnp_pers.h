/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

/**
 * @addtogroup nsnp_pers Personalization
 * @ingroup nsnp
 * @{
 *
 * @file
 * Personalization APIs
 *
 * @see NautilusAPIReference.pdf
 */

#ifndef NSNP_PERS_API_H
#define NSNP_PERS_API_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                                                                 PUBLIC API */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/**
 * @brief Initializes Nautilus context for an nppers session
 *
 * @param[in,out]  io_handle       Nautilus context handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_TRANSPORTLAYERFAILURE
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_InitPersHandle(np_handle_t io_handle);

/* -------------------------------------------------------------------------- */
/**
 * @brief Finalizes an nppers session
 *
 * @param[in,out]  io_handle       Nautilus context handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_TRANSPORTLAYERFAILURE
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_FinPersHandle(np_handle_t io_handle);

/* -------------------------------------------------------------------------- */
/**
 * @brief Generates the request for initiating the personalization
 *
 * @param[in]      in_handle       Nautilus context handle
 * @param]out]     out_request     Generated request message
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_TRANSPORTLAYERFAILURE
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_NEMOPersonalizationStep1(np_handle_t in_handle,
                              np_str_t **out_request);

/* -------------------------------------------------------------------------- */
/**
 * @brief Processes the reply to the initiation request generated in step 1
 *
 * @param[in]      in_handle       Nautilus context handle
 * @param[in]      in_responst     Response message to be parsed
 * @param]out]     out_request     second request message to be sent
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_MESSAGECORRUPTED
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_TRANSPORTLAYERFAILURE
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_NEMOPersonalizationStep2(np_handle_t in_handle,
                              np_str_t *in_response,
                              np_str_t **out_request);

/* -------------------------------------------------------------------------- */
/**
 * @brief Processes the reply to the acquisition request generated in step 2
 *
 * @param[in]      in_handle       Nautilus context handle
 * @param[in]      in_response     Response message to be processed
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_MESSAGECORRUPTED
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_TRANSPORTLAYERFAILURE
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_NEMOPersonalizationStep3(np_handle_t in_handle,
                              np_str_t *in_response);

/* -------------------------------------------------------------------------- */
/**
 * @brief Sets keytype to the personalization context
 *
 * @note This API is called only for pers protocol version 2.
 *
 * @param[in]     in_handle       Nautilus context handle
 * @param[in]     in_keytype      Personality key type
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_SetPersKeyType(np_handle_t          in_handle,
                    np_pers_keytype_t    in_keytype);

/* -------------------------------------------------------------------------- */
/**
 * @brief Sets the Business token in the personalization context for nppers session
 *
 * @param[in]     in_handle              Nautilus context handle
 * @param[in]     in_business_token_len  Length of the business token
 * @param[in]     in_business_token      Business token
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_SetPersBusinessToken(np_handle_t      in_handle,
                          np_size_t        in_business_token_len,
                          u_int8_t        *in_business_token);


NP_END_EXTERN_C

#endif /* NSNP_PERS_API_H */
/** @} */
