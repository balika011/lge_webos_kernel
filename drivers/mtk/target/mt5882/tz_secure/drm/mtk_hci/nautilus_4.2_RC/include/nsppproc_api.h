/** @addtogroup nsppproc nsppproc
 *  @ingroup nsec
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef NSPPPROC_API_H
#define NSPPPROC_API_H

#define MINIMUM_REQUESTBUFFERSIZE 256

NP_BEGIN_EXTERN_C

/**
 * @file
 * nsppproc library
 *
 * depends on:
 *      - ppproc_types.h
 *
 * @see NautilusAPIReference.pdf
 */

/**
 * @brief Intialize the ppproc protocol stack
 *
 * @param[in,out] io_handle PPProc handle and target info
 *
 * @return ::ERR_NP_OK
 * @return ::nssmi_PpprocInit
 */
extern np_ret_t
nsppproc_Init( np_handle_t io_handle );

/**
 * @brief Finalize the ppproc protocol stack
 *
 * @param[in,out] io_handle PPProc handle and target info
 *
 * @return ::ERR_NP_OK
 * @return ::nssmi_PpprocFin
 */
extern np_ret_t
nsppproc_Fin( np_handle_t io_handle );

/**
 * @brief Create Request Payload
 *
 * @param[in,out] io_handle  PPProc handle and target info
 * @param[in,out] io_len     Length of payload
 * @param[out]    out_buf    Payload text
 * @param[in]     in_usage   usage
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nssmi_PpprocGenerateRequest
 */
extern np_ret_t
nsppproc_GenerateRequest( np_handle_t  io_handle,
                          u_int32_t   *io_len,
                          u_int8_t    *out_buf,
                          u_int32_t    in_usage );

/**
 * @brief Parse the response and register the personalities
 *
 * @param[in,out] io_handle PPProc handle and target info
 * @param[in]     in_len    The response buffer length
 * @param[in]     in_buf    The response buffer
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_MESSAGECORRUPTED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nssmi_PpprocParseResponse
 */
extern np_ret_t
nsppproc_ParseResponse( np_handle_t  io_handle,
                        u_int32_t    in_len,
                        u_int8_t    *in_buf );

NP_END_EXTERN_C

#endif /* NSPPPROC_API_H */
/** @} */
