/** @addtogroup ms3 ms3
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef MS3_API_H
#define MS3_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief library for MS3
 *
 */
/*===========================================================================*/

/*----------------------------------------------------------------------------*
 * defines
 *----------------------------------------------------------------------------*/

/* Output control flags */

#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_BA_DOT    (1<<0)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_BA_EPN    (1<<1)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_BA_CCI    (1<<2)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_BA_ICT    (1<<3)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_BA_APS    (1<<4)

#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCP_RMM  (1<<5)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCP_RS   (1<<6)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCP_EPN  (1<<7)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCP_CCI  (1<<8)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCP_ICT  (1<<9)
#define MS3_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCP_APS  (1<<10)

/*----------------------------------------------------------------------------*
 * type definitions
 *----------------------------------------------------------------------------*/

typedef void *ms3_handle_t;

typedef struct {
    u_int32_t   flags;
    struct {
        u_int32_t  basic_flags;
        u_int32_t epn;
        u_int32_t cci;  /* copy control information */
        u_int32_t ict;  /* Image constraint token */
        u_int32_t dot;  /* digital only token */
        u_int32_t aps;
    } basic;
    struct {
        u_int32_t  dtcp_flags;
        u_int32_t rmm;  /* Retention move mode */
        u_int32_t rs;   /* Retention state */
        u_int32_t epn;
        u_int32_t cci;
        u_int32_t ict;
        u_int32_t aps;
    } dtcp;
    void *reserved_data;
} ms3_oc_info_t;

typedef enum {
    MS3_CONTENT_RETENTION_NOT_ALLOWED  = 0,  /** Content rentention not allowed */
    MS3_CONTENT_RETENTION_ALLOWED      = 1   /** Content can be retained */
} ms3_retention_flag_t;

typedef struct {
    bool_t                   is_critical;     /** Flag indicating if the extension is critical */
    u_int32_t                extension_type;  /** Type of the extension */
    binstr_t                *extension_data;  /** Extension data */
} ms3_extension_t;

typedef struct {
    ms3_oc_info_t         *moc_info;         /** Marlin Output control info */
    bool_t                 is_curl_present;  /** Flag indicating is the CURL is present */
    binstr_t              *curl;             /** Content URL if there was a curit expanded */
    ms3_retention_flag_t   retention_flag;   /** Retention flag */
    u_int32_t              num_content_ids;  /** Number of content ids */
    binstr_t             **content_ids;      /** Array of hashed content ids.
                                              *  Useful for app integration and testing */
    u_int32_t              num_extensions;   /** Number of extentsion present */
    ms3_extension_t      **extensions;       /** The array of extension */
} ms3_resultdata_t;


/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/
#define ERR_MS3_ILLEGALSAS              (ERR_MS3_BASE|1)
#define ERR_MS3_ILLEGALCURIT            (ERR_MS3_BASE|2)
#define ERR_MS3_NOSASFOUND              (ERR_MS3_BASE|3)
#define ERR_MS3_SASALREADYPROCESSED     (ERR_MS3_BASE|4)
#define ERR_MS3_ILLEGALCOMPOUNDURI      (ERR_MS3_BASE|5)
#define ERR_MS3_SASALREADYSET           (ERR_MS3_BASE|6)

/**
 * @brief Initialize an MS3 handle.
 *
 * @param[out] out_handle  MS3 handle
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 */
retcode_t
ms3_Init(ms3_handle_t *out_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Finalize an MS3 handle.
 *
 * This function finalizes (deallocates) a MS3 handle. This function should
 * be called only once for a given handle.
 *
 * @param[in,out] io_handle MS3 handle.
 *
 */
void
ms3_Fin(ms3_handle_t io_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Get Keymanager handle from MS3 context.
 * This API will get the keyhandle from the MS3 context
 *
 * @param[in] in_handle              ms3 handle
 * @param[out] out_keymng            keymanager handle
 *
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_KeymngContextGet(ms3_handle_t in_handle, keymng_handle_t *out_keymng);

/*----------------------------------------------------------------------------*/
/**
 * @brief Get NEMO Keys from device.
 * This API will get the NEMO keys from the hci and reg components
 * It gets both NEMO private keys and NEMO public certificate from low
 * level
 *
 * @param[in] in_handle              ms3 handle
 * @param[in] in_application_data    Opaque application data. May be
 *                                   used to protect SAS and NEMO keys
 * @param[out] out_nemo_prv_key      NEMO private signing key
 * @param[out] out_nemo_pub_cert     NEMO signing certificate
 *
 * @return ::ERR_NOMEM
 * @return ::ERR_NP_ILLEGALARGS
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_GetNemoKeys(ms3_handle_t   in_handle,
                binstr_t      *in_application_data,
                binstr_t     **out_nemo_prv_key,
                binstr_t     **out_nemo_pub_cert);


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
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_MS3_ILLEGALSAS
 * @return ::ERR_MS3_ILLEGALCURIT
 * @return ::ERR_MS3_SASALREADYSET
 * @return ::ERR_NOMEM
 * @return ::hci_Lock
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_ParseSAS(ms3_handle_t in_handle,
             binstr_t *in_sas_buf,
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
 * @param[in,out] io_handle              ms3 handle

 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_MS3_SASALREADYPROCESSED
 * @return ::ERR_MS3_NOSASFOUND
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_SASInfoProcessingComplete(ms3_handle_t io_handle);

/*----------------------------------------------------------------------------*/
/**
 * @brief Expand c-urit
 * This API will expand curit with authenticator and returns C-URL
 *
 * @param[in] in_handle              ms3 handle
 * @param[in] in_curit               c-urit for expanding with authenticator
 * @param[out] out_curl              C-URL string after expanding c-urit
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_MS3_ILLEGALCURIT
 * @return ::ERR_NOMEM
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_CuritExpand(ms3_handle_t in_handle,
                binstr_t *in_curit,
                binstr_t **out_curl);

/*----------------------------------------------------------------------------*/
/**
 * @brief Move the key from the hci reference to a platform specific location
 *
 * @param[in] in_handle              ms3 handle
 * @param[in,out] io_key             octopus key
 * @param[in] in_pv                  User specific data used when moving the key
 * @param[out] out_pv                User specific data returned after moving the key
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_NOMEM
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_MoveKey(ms3_handle_t in_handle,
            octobj_key_t  *io_key,
            binstr_t      *in_pv,
            binstr_t      **out_pv);

/*----------------------------------------------------------------------------*/
/**
 * @brief Get the Content key corresponding to content id
 * This API will extract the content key from hci and return them
 *
 * @param[in] in_handle            ms3 handle
 * @param[in] in_cid               content id
 * @param[out] out_ck              pointer to content key associated with content
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_NOMEM
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_EnableCKfromCID(ms3_handle_t in_handle,
                    binstr_t *in_cid,
                    octobj_contentkey_t **out_ck);

/*----------------------------------------------------------------------------*/
/**
 * @brief Decrypt the given data with the key
 * This API will decrypt the encrypted buffer
 *
 * @param[in] in_handle            ms3 handle
 * @param[in] in_key               pointer to octopus key
 * @param[in] in_cryptotype        crypto algorithm type
 * @param[in] in_saltbuf           salt buffer
 * @param[in] in_cipherbuf         encrypted buffer data
 * @param[out] out_decdata         decrypted buffer data
 * @param[in,out] io_declen        decrypted data length
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_NOMEM
 */
/*----------------------------------------------------------------------------*/
retcode_t
ms3_Decrypt(ms3_handle_t in_handle,
            octobj_key_t *in_key,
            int32_t in_cryptotype,
            u_int8_t *in_saltbuf,
            binstr_t *in_cipherbuf,
            u_int8_t *out_decdata,
            u_int32_t *io_declen);


/*----------------------------------------------------------------------------*/
/**
 * @brief Free the result data structure
 *
 * @param[in,out] io_resultdata      result data structure
 *
 */
void
ms3_FreeResultData(ms3_resultdata_t  *io_resultdata);

NP_END_EXTERN_C

#endif /* MS3_API_H */
/** @} */
