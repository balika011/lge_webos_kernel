/** @addtogroup bbts bbts
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef BBTS_API_H
#define BBTS_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*===========================================================================*/
    /** @file
     * @brief Marlin BBTS helper functions
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * error code definitions
     *---------------------------------------------------------------------------*/

#define ERR_BBTS_INVALID_ECM   (ERR_BBTS_BASE | 1)


    /*---------------------------------------------------------------------------*
     * macro
     *---------------------------------------------------------------------------*/

#define BBTS_ALGO_MPEG2_TS_CRYPT                 (7)   /* [IEC] value is overriden by [BBTS] value */
#define BBTS_FLAG_TRUE                           (1)
#define BBTS_FLAG_FALSE                          (0)
#define BBTS_TIME_STAMP_SIZE                     (40/8)
#define BBTS_KEY_SIZE                            (128/8)
#define BBTS_MAC_SIZE                            (96/8)
#define BBTS_CIPHER_MODE_CBC                     (0x01)
#define BBTS_CIPHER_MODE_CSA                     (0x02)
#define BBTS_MPEG2_TS_CRYPT_AES_128              (0x05)
#define BBTS_MPEG2_TS_CRYPT_AES_128_IV_LEN       (0x10)
#define BBTS_MPEG2_TS_CRYPT_AES_128_ETK_LEN      (0x10)
#define BBTS_MPEG2_TS_CRYPT_DVB_CSA_64           (0x00)
#define BBTS_MPEG2_TS_CRYPT_DVB_CSA_ETK_LEN      (0x08)
#define BBTS_SERVICE_PROGRAM_CID_DELIMETRE_SIZE  (0x02)
#define BBTS_KSM_PROTOCOL_VERSION                (0x01)
#define BBTS_PIDSID_EXTENSION_HEX_SIZE           (0x08)

    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

    /**
     * BBTS types
     *
     * Service Type
     */
    typedef enum {
        BBTS_IS_SERVICE   = 0x01,  /**< Service */
        BBTS_IS_PROGRAMME = 0x02   /**< Programme */
    } bbts_type_t;

    /**
     * BBTS access information descriptor types
     *
     * Type to specify the type of an access info descriptor
     */
    typedef enum {
        BBTS_UNKNOWN_DESCRIPTOR           = 0x00,  /**< Unknown */
        BBTS_PARENTAL_RATING_DESCRIPTOR   = 0x01,  /**< Parental*/
        BBTS_COPY_CONTROL_INFO_DESCRIPTOR = 0x02,  /**< Copy Control Information*/
        BBTS_BLACKOUT_SPOTBEAM_DESCRIPTOR = 0x03   /**< Blackout spotbeam */
    } bbts_access_desc_type_t;

    /**
     * BBTS access information descriptor
     *
     * Type to specify the access info descriptor data
     */
    typedef struct {
        bbts_access_desc_type_t   desc_type;  /** Type of the access descriptor */
        /** Data found in the descriptor */
        union {
            np_str_t           *value;      /**< Descriptor value */
            u_int8_t            cci_info;   /**< Descriptor moc data */
        } descriptor;
    } bbts_access_desc_t;

    typedef struct {
        u_int32_t             num_accessDesc;  /**< Number of Access Descriptors present*/
        bbts_access_desc_t  **accessDescs;     /**< Array of Access Descriptors */
    } bbts_access_criteria_t;

    /**
     * BBTS ECM structure (see [iec] p.64 & [BBTS] l. 340)
     */
    typedef struct bbts_ecm {
        u_int8_t                   protocol_version;
        u_int8_t                   access_criteria_flag;
        u_int8_t                   traffic_protection_protocol;
        u_int8_t                   traffic_authentication_flag;
        u_int8_t                   next_traffic_key_flag;
        u_int8_t                   timestamp_flag;
        u_int8_t                   programme_flag;
        u_int8_t                   service_flag;
        u_int8_t                   content_key_index;
        u_int8_t                   odd_even_flag;
        u_int8_t                   cipher_mode;
        u_int8_t                   traffic_key_lifetime;
        u_int8_t                   permissions_flag;
        u_int8_t                   permissions_category;
        binstr_t                   initial_vector;
        binstr_t                   next_initial_vector;
        binstr_t                   encrypted_traffic_key_material;
        binstr_t                   next_encrypted_traffic_key_material;
        binstr_t                   timestamp;
        binstr_t                   encrypted_PEK;
        u_int32_t                  programme_cid_extension;
        u_int32_t                  programme_mac_data_len; /**< length of data over which programme mac is calculated*/
        binstr_t                   programme_mac;
        u_int32_t                  service_cid_extension;
        u_int32_t                  service_mac_data_len; /**< length of data over which service mac is calculated*/
        binstr_t                   service_mac;
        bbts_access_criteria_t     access_descs;
    } bbts_ecm_t;


    /*---------------------------------------------------------------------------*
     * APIs
     *---------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /**
     * @brief Parses the ECM stream and returns the fields in a structure.
     *
     * @note the caller should not free out_faultinfo
     *
     * @param [in]   in_ecm               ECM Stream
     * @param [in]   is_single_key_layer  Flag indicating if it is Single Key layer processing
     * @param [out]  out_ecm_info         ECM Info structure
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_BBTS_INVALID_ECM
     *
     */
    /*-----------------------------------------------------------------------*/
    extern retcode_t
    bbts_ParseECM(binstr_t     *in_ecm,
                  bool_t        is_single_key_layer,
                  bbts_ecm_t  **out_ecm_info);


    /*-----------------------------------------------------------------------*/
    /**
     * @brief Frees the ECM info
     *
     *
     * @param [in]   io_ecm_info               ECM Info
     *
     */
    /*-----------------------------------------------------------------------*/
    extern void
    bbts_FreeECM(bbts_ecm_t  *io_ecm_info);

    /*-----------------------------------------------------------------------*/
    /**
     * @brief Generates content ID's.
     *
     *
     * @param [in]   in_socID                  SOC ID
     * @param [in]   in_ServiceBaseCID         Service Base CID
     * @param [in]   in_type                   Service or Programme type
     * @param [in]   in_extension              Programme or Service extension based on in_type value
     * @param [out]  out_cid                   Generated Content ID
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_ILLEGALARGS
     */
    /*-----------------------------------------------------------------------*/
    extern retcode_t
    bbts_GenerateContentID(binstr_t       *in_socID,
                           binstr_t       *in_ServiceBaseCID,
                           bbts_type_t     in_type,
                           u_int32_t       in_extension,
                           binstr_t      **out_cid);


#ifdef __cplusplus
}
#endif

#endif                          /* BBTS_API_H */
/** @} */
