/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef NVS_API_H
#define NVS_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * NVS (Non-Volatile Storage) Management API
 *
 * require: crypto, base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    NVS_FTYPE_READONLY = 0,     /**< NVS is used for read-only purpose. */
    NVS_FTYPE_READWRITE,        /**< NVS is used for read-write purpose. */
} nvs_ftype_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_NVS_NOT_EXIST       (RET_NVS_BASE|1)
#define RET_NVS_OPEN_ERROR      (RET_NVS_BASE|2)
#define RET_NVS_READ_ERROR      (RET_NVS_BASE|3)
#define RET_NVS_WRITE_ERROR     (RET_NVS_BASE|4)
#define RET_NVS_VERIFY_ERROR    (RET_NVS_BASE|5)
#define RET_NVS_BUFFER_SHORT    (RET_NVS_BASE|6)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_OK               Success
 * @retval RET_INTERNALERROR    An unexpected error has occurred
 */
extern int
nvs_Init(void);

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_OK               Success
 * @retval RET_INTERNALERROR    An unexpected error has occurred
 */
extern int
nvs_Fin(void);

/**
 * Remove NVS (not MT-safe)
 *
 * @param [in]     in_type      Storage type
 * @retval RET_OK               Success
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR    An unexpected error has occurred
 * @retval RET_NVS_OPEN_ERROR   Fail to opne the NVS specified by @p in_type
 */
extern int
nvs_CleanupNVS(nvs_ftype_t      in_type);

/**
 * Load data from NVS
 *
 * @param [in]     in_type      Storage type
 * @param [out]    out_buf      Output data buffer
 * @param [in,out] io_len       Output data length
 * @retval RET_OK               Success
 * @retval RET_NOMEM            Fail to allocate memory
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR    An unexpected error has occurred
 * @retval RET_NVS_NOT_EXIST    The NVS specified by @p in_type does not exist
 * @retval RET_NVS_OPEN_ERROR   Fail to opne the NVS specified by @p in_type
 * @retval RET_NVS_READ_ERROR   Fail to read the NVS specified by @p in_type
 * @retval RET_NVS_VERIFY_ERROR Fail to verify the NVS specified by @p in_type
 * @retval RET_NVS_BUFFER_SHORT @p out_buf length is not enough to load data
 */
extern int
nvs_Load(nvs_ftype_t            in_type,
         u_int8_t              *out_buf,
         u_int32_t             *io_len);

/**
 * Save data to NVS
 *
 * @param [in]     in_type      Storage type
 * @param [in]     in_buf       Input data buffer
 * @param [in]     in_len       Input data length
 * @retval RET_OK               Success
 * @retval RET_NOMEM            Fail to allocate memory
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR    An unexpected error has occurred
 * @retval RET_NVS_OPEN_ERROR   Fail to opne the NVS specified by @p in_type
 * @retval RET_NVS_WRITE_ERROR  Fail to write the NVS specified by @p in_type
 */
extern int
nvs_Save(nvs_ftype_t            in_type,
         const u_int8_t        *in_buf,
         u_int32_t              in_len);


extern int nvs_mem_init(char *in_ro, int in_ro_len, char *in_rw, int in_rw_len);
extern int nvs_mem_fini(char *out_rw, int out_rw_len);
extern int nvs_mem_rw_len(int *out_rw_len);







#ifdef __cplusplus
}
#endif

#endif /* NVS_API_H */
