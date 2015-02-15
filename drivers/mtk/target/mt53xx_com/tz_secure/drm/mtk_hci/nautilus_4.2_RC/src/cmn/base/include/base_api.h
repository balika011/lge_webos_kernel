/** @addtogroup base base
 *  @ingroup cmn
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 *
 */
#ifndef BASE_API_H
#define BASE_API_H

/** @file
 * This library contains initialization/finalization functions for
 * the base components.
 *
 * @require mem
 *
 * @author SAKAMOTO Takahiro <takas@sm.sony.co.jp>
 */

NP_BEGIN_EXTERN_C

#define BASE_DEFAULT_MEM_SIZE       ((np_size_t)0)
#define BASE_DEFAULT_MAX_SESSIONS   128

#define BASE_MAX_DRM_SESSIONS       BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_MS3_SESSIONS       BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_DATAPATH_SESSIONS  BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_BBTS_SESSIONS      BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_IMPORT_SESSIONS    BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_SAC_SESSIONS       BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_BB_SESSIONS        BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_LTP_SESSIONS       BASE_DEFAULT_MAX_SESSIONS
#define BASE_MAX_PERS_SESSIONS      8

#define BASE_SERVICE_HANDLE_MAP_SIZE  (BASE_MAX_DRM_SESSIONS      + BASE_MAX_MS3_SESSIONS    + \
                                       BASE_MAX_DATAPATH_SESSIONS + BASE_MAX_BBTS_SESSIONS   + \
                                       BASE_MAX_BBTS_SESSIONS     + BASE_MAX_IMPORT_SESSIONS + \
                                       BASE_MAX_SAC_SESSIONS      + BASE_MAX_BB_SESSIONS     + \
                                       BASE_MAX_LTP_SESSIONS      + BASE_MAX_PERS_SESSIONS)


/**
 * @brief Initializes the base components
 *
 * @param[in]  in_context   Security context
 * @param[in]  in_mem       Heap start pointer (if NULL, memory space is dynamically allocated)
 * @param[in]  in_memSize   Heap size (may be BASE_DEFAULT_MEM_SIZE)
 *
 * @note The two memory parameters are ignored when -use_libc_malloc build option is selected
 *
 * @see ::NPI_CMN_CONTEXT_SECURE
 * @see ::NPI_CMN_CONTEXT_NONSECURE
 *
 * @return ::ERR_OK                <br>Indicating success
 * @return ::ERR_NOMEM             <br>A resource failure has occurred.
 * @return ::ERR_ILLEGALARGS       <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NG                <br>If failed to get the context of initialization
 */
retcode_t
npi_base_Init(
    npi_cmncontext_t    in_context,
    char               *in_mem,
    np_size_t           in_memSize);

/**
 * @brief Finalizes the base components
 *
 * @param[in]  in_context Security context
 *
 * @see ::NPI_CMN_CONTEXT_SECURE
 * @see ::NPI_CMN_CONTEXT_NONSECURE
 *
 * @return ::ERR_OK                <br>Indicating Succes
 * @return ::ERR_ILLEGALARGS       <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NG                <br>If failed to get base count of either secure and non-secure memory context.
 */
retcode_t
npi_base_Fin(npi_cmncontext_t in_context);

/**
 * @brief Initializes base components with default values.
 *        Both secure and non-secure memory context are initialized.
 *
 * @return ::ERR_OK                <br>Indicating success
 * @return ::ERR_NOMEM             <br>A resource failure has occurred.
 * @return ::ERR_ILLEGALARGS       <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NG                <br>If failed to get context of memory.
 */
retcode_t
npi_base_InitForSample( void);

/**
 * @brief Finalizes base components
 *        Both secure and non-secure memory context are finalized.
 *
 * @return ::ERR_OK                <br>Indicating success
 * @return ::ERR_ILLEGALARGS       <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NG                <br>If failed to get context of memory.
 */
retcode_t
npi_base_FinForSample( void);

/**
 * @brief Get the value belonging to a certain environment variable.
 *
 * @note If the environment variable is not defined, the empty string is returned.
 *
 * @param[in]   in_env      Name of environment variable
 * @param[out]  out_val     buffer for storing value of environment variable
 * @param[in]   in_len      size of out_val buffer
 *
 * @return ::ERR_OK                <br>Indicating success
 * @return ::ERR_ILLEGALARGS       <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NOMEM             <br>A resource failure has occurred.
 */
retcode_t
npi_base_GetEnv(
    const char           *in_env,
    char                 *out_val,
    np_size_t             in_len);

/**
 * @brief Contains pointers to modules' context.
 */
typedef struct base_glob_tag {
    void   *module_ssmem;
    void   *module_reg;
    void   *module_tmcxt;
    void   *module_cert_crl_cache;
    void   *module_npbb;
    void   *module_nppers;
    void   *module_npltp;
    void   *module_np_services;
    void   *module_device;
    void   *module_mem_usr;
    void   *module_base;
    int32_t np_count;
    int32_t np_drm_session_cnt;
    int32_t np_ms3_session_cnt;
    int32_t np_datapath_session_cnt;
    int32_t np_bbts_session_cnt;
    int32_t np_import_session_cnt;
    int32_t np_sac_session_cnt;
    int32_t np_bb_session_cnt;
    int32_t np_pers_session_cnt;
    int32_t np_ltp_session_cnt;
} base_glob_t;

/**
 * @brief Get static array of pointers used for storing global variables.
 *
 * @return array of pointers
 */
base_glob_t *
npi_base_GetGlobals(void);

#if defined( NPOPT_ENABLE_TESTHOOKS )
#define IDX_PERS_VERSION_VAL      0

/**
 * @brief Creates a structure to hold the model version and crl dp value.
 */
typedef struct hook_glob_tag {
    char   *pacs_model_ver;
    bool_t  zero_lastprobe_val;
} hook_glob_t;

/**
 * @brief Initializes  the model version and the CRL DP_VAL .
 * @param[in] in_context   context
 *
 * @return none
 */
void
npi_base_InitNsCfg(npi_cmncontext_t in_context);

/**
 * @brief Finalizes and free the model version and the CRL DP_VAL .
 * @param[in] in_context   context
 *
 * @return none
 */
void
npi_base_FinNsCfg(npi_cmncontext_t in_context);

/**
 * @brief Get  the current  model version and the CRL DP_VAL .
 *
 * @return                 valid pointer to structure containing model version and the CRL DP_VAL
 * @return NULL
*/
hook_glob_t *
npi_base_GetNsCfg(void);
#endif

NP_END_EXTERN_C

#endif /* BASE_API_H */
/** @} */
