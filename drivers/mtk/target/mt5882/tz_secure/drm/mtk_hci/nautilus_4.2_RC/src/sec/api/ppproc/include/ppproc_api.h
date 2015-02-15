/** @addtogroup ppproc ppproc
 *  @ingroup api
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef PPPROC_API_H
#define PPPROC_API_H

NP_BEGIN_EXTERN_C

#include <ppproc_types.h>

#if defined( NPOPT_USE_PPPROC )
/*---------------------------------------------------------------------------*/
/**
 ** @brief Initialize PPProc
 **
 ** @return ::ERR_NP_OK                        <br>Indicating success.
 **
 **/
/*---------------------------------------------------------------------------*/
extern np_ret_t
ppproc_Init(void);

/*---------------------------------------------------------------------------*/
/**
 ** @brief Finalize PPProc
 **
 ** @return ::ERR_NP_OK                        <br>Indicating success.
 **
 **/
/*---------------------------------------------------------------------------*/
extern np_ret_t
ppproc_Fin(void);

/*---------------------------------------------------------------------------*/
/**
 ** @brief Create Request Payload
 **
 ** @param[in]  in_usage     usage
 ** @param[in]  in_len       length
 ** @param[out] out_request  buffer
 **
 ** @return ::ERR_NP_OK                        <br>Indicating success.
 ** @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory.
 ** @return ::ERR_NP_ILLEGALARGS               <br>The input arguments to the function are incorrect.
 ** @return ::ERR_NP_PPPROC_SHORTOFBUFFERSIZE  <br>The request buffer size is less than the minimum required.
 **
 **/
/*---------------------------------------------------------------------------*/
extern np_ret_t
ppproc_GenerateRequest( u_int32_t       in_usage,
                        u_int32_t       in_len,
                        np_secbuffer_t  *out_request );

/*---------------------------------------------------------------------------*/
/**
 ** @brief Parse Response Payload
 **
 ** @param[in]  in_len           buffer length
 ** @param[in]  in_buf           buffer
 ** @param[in]  in_format        format of ppproc package.
 **                              (it can be either encrypted or clear)
 ** @param[out] out_secbuf       sdata after parsing
 **
 ** @return ::ERR_NP_OK                        <br>Indicating success.
 ** @return ::ERR_NP_OUTOFMEMORY               <br>Unable to allocate memory.
 ** @return ::ERR_NP_ILLEGALARGS               <br>The input arguments to the function are incorrect.
 ** @return ::ERR_NP_INTERNALERROR             <br>An unrecoverable error has occurred in Nautilus.
 ** @return ::ERR_NP_FILEERROR                 <br>An error occurred reading or writing to file.
 ** @return ::ERR_NP_MESSAGECORRUPTED          <br>response message is corrupted.
 **
 **/
/*---------------------------------------------------------------------------*/
extern np_ret_t
ppproc_ParseResponse( u_int32_t       in_len,
                      u_int8_t       *in_buf,
                      u_int32_t       in_format,
                      np_secbuffer_t *out_secbuf );
#endif /* defined( NPOPT_USE_PPPROC ) */

NP_END_EXTERN_C

#endif /* PPPROC_API_H */
/** @} */
