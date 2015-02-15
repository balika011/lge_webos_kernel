/** @addtogroup npms3 npms3
 *  @ingroup api
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef NPMS3_API_H
#define NPMS3_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Nautilus Library API (MS3)
 */
/*===========================================================================*/


typedef struct {
    binstr_t *nemo_pvt_key;                  /** nemo private keys */
    binstr_t *nemo_pub_cert;                 /** nemo public certificates */
} npms3_nemokeys_t;


/*----------------------------------------------------------------------------*/
/**
 * @brief Initialize MS3 component.
 * This API will initialize the MS3 component
 *
 * @param[out] out_handle     The buffer where the component handle will be
 *                                stored.
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 */

/*----------------------------------------------------------------------------*/
np_ret_t
npms3_InitSession(hdls_context_t *out_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Finalize MS3 component.
  *
 * This API should be invoked at the end of use of the component
 *
 * @param[in,out]     io_handle Session handle to be finalized.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_FinSession(hdls_context_t io_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Get NEMO signing keys.
 *
 * @param[in] in_handle          ms3 handle
 * @param[in] in_applicatin_data Opaque application data. May be
 *                               used to protect SAS and NEMO keys
 * @param[out] out_nemo_prv_key  NEMO private signaing key
 * @param[out] out_nemo_pub_cert NEMO signaing certificate path
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_NOPERSONALITYREGISTERED
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_GetNemoKeys(hdls_context_t      in_handle,
                  binstr_t           *in_application_data,
                  np_secbuffer_t     *out_nemo_prv_key,
                  np_secbuffer_t     *out_nemo_pub_cert);

/*----------------------------------------------------------------------------*/
/**
 * @brief parse SAS
 * This API will parse SAS buffer and return the output structure, which has elements
 * extracted from SAS buffer
 *
 * @param[in] in_handle              ms3 handle
 * @param[in] in_SAS_Buffer          SAS buffer to parse
 * @param[in] in_surl                s-url for storing in the ms3 handle
 * @param[in] in_curit               c-urit for expanding with authenticator
 * @param[in] in_application_data    opaque application data, maybe used to protect SAS
 * @param[out] out_ms3_result_data   out result data structure with SAS parsed info

 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_PARSE_SAS
 * @return ::ERR_NP_ILLEGALCURIT
 * @return ::ERR_NP_SASALREADYSET
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_ParseSAS(hdls_context_t in_handle,
               binstr_t *in_sas,
               binstr_t *in_surl,
               binstr_t *in_curit,
               binstr_t *in_application_data,
               ms3_resultdata_t  **out_ms3_result_data);

/*----------------------------------------------------------------------------*/
/**
 * @brief SAS Info processing complete
 * This API will indicate that the processing of SAS is complete and the
 * content keys are usable.
 *
 * @param[in] in_handle              ms3 handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_SASALREADYPROCESSED
 * @return ::ERR_NP_NOSASFOUND
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_SASInfoProcessingComplete(hdls_context_t in_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Expand c-urit
 * This API will expand curit with authenticator and returns C-URL
 *
 * @param[in] in_handle              ms3 handle
 * @param[in] in_curit               c-urit for expanding with authenticator
 * @param[out] out_curl              C-URL string after expanding c-urit
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INVALID_CURIT
 * @return ::ERR_NP_NOSASFOUND
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_CuritExpand(hdls_context_t   in_handle,
                  binstr_t        *in_curit,
                  np_secbuffer_t  *out_curl);

/*----------------------------------------------------------------------------*/
/**
 * @brief Resolve ms3 context
 * This API will resolve the context to ms3 context, which is required for the
 * decryption use cases (which needs to be used from np_MoveKey)
 *
 * @param[in] hdls_context_t              np handle context
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_ResolveContext(hdls_context_t *in_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Prepare octopus objects for InitTrack
 * This API will resolve the context for InitTrack and prepare content key objects
 *
 * @param[in] in_anyhdl         np handle context
 * @param[in] in_cid            content id
 * @param[out] out_context      ms3 context
 * @param[out] out_ck           octopus content key
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
/*----------------------------------------------------------------------------*/
np_ret_t
npms3_InitTrack(hdls_anyhdl_t         in_anyhdl,
                binstr_t             *in_cid,
                void                **out_context,
                octobj_contentkey_t **out_ck);

NP_END_EXTERN_C

#endif /* NPMS3_API_H */
