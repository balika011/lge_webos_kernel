/** @addtogroup npimport npimport
 *  @ingroup api
 * @{ */

/*===========================================================================*/
/** @brief Import Data Structures and Enumerations
 *
 * @see API.reference.pdf
 * @file
 *
 */
/** @} */
/*===========================================================================*/

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef NPIMPORT_TYPES_H
#define NPIMPORT_TYPES_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                        DATA STRUCTURES                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Import Content types
 *
 * Type to specify the type of content to import
 **/
typedef enum {
    NP_IMPORT_DTCP         = 0x0,      /**< Content comes from DTCP */
    NP_IMPORT_ARIB         = 0x1,      /**< Content comes from ARIB */
    NP_IMPORT_DTCP_MPEG_TS = 0x2,      /**< Content comes from DTCP MPEG TS */
    NP_IMPORT_ARIB_SERIAL  = 0x3,      /**< Content comes from ARIB serialised descriptor */
    NP_IMPORT_NONE         = 0x4,      /**< Content not specified because it is made available
                                            using another means, such as SAC */
} np_import_content_t;


/**
 * @brief EPN types
 *
 * Type to specify the source EPN value used for the license generation
 **/
typedef enum {
    NP_IMPORT_EPN_ASSERTED    = 0x0,  /**< EPN asserted */
    NP_IMPORT_EPN_UNASSERTED  = 0x1   /**< EPN unasserted */
} np_import_epn_t;


/**
 * @brief CCI types
 *
 * Type to specify the source DTCP CCI value used for the license generation
 **/
typedef enum {
    NP_IMPORT_CCI_COPY_FREE       = 0x0,  /**< Copy free */
    NP_IMPORT_CCI_NO_MORE_COPIES  = 0x1,  /**< No more copies */
    NP_IMPORT_CCI_COPY_ONE_GEN    = 0x2   /**< Copy one generation */
} np_import_cci_t;

/**
 * @brief ARIB CCT types
 *
 * Type to specify the source ARIB CCT value used for the license generation
 **/
typedef enum {
    NP_IMPORT_CCT_NONE          = 0x0,  /**< No Copy Control Type available */
    NP_IMPORT_CCT_ONE_SEGMENT   = 0x1,  /**< One segment copy control type */
    NP_IMPORT_CCT_FULL_SEGMENT  = 0x2,  /**< Full segment copy control type */
} np_import_cct_t;

/**
 * @brief ICT types
 *
 * Type to specify the source ICT value used for the license generation
 **/
typedef enum {
    NP_IMPORT_ICT_CONSTRAINED  = 0x0,  /**< High def analog in constrained form */
    NP_IMPORT_ICT_HIGH_DEF     = 0x1   /**< High def analog in high def form */
} np_import_ict_t;


/**
 * @brief APS types
 *
 * Type to specify the source APS value used for the license generation
 **/
typedef enum {
    NP_IMPORT_APS_COPY_FREE  = 0x0,  /**<  Copy free */
    NP_IMPORT_APS_TYPE1      = 0x1,  /**<  APS is on : type1 (AGC) */
    NP_IMPORT_APS_TYPE2      = 0x2,  /**<  APS is on : type2 (AGC + 2L colorstripe) */
    NP_IMPORT_APS_TYPE3      = 0x3   /**<  APS is on : type3 (AGC + 4L colorstripe) */
} np_import_aps_t;


/**
 * @brief Import DTCP descriptor type
 *
 * Type to specify the dtcp content data
 **/
typedef struct np_import_dtcp {
    np_import_epn_t  epn;           /**< Source EPN */
    np_import_ict_t  ict;           /**< Source ICT */
    np_import_aps_t  aps;           /**< Source APS */
    np_import_cci_t  cci;           /**< DTCP CCI */
} np_import_dtcp_t;


/**
 * @brief Import RAW DTCP descriptor type protected by hmac
 *
 * Type with DTCP usage rules. Tamper protected by an hmac. The CID defines the key.
 **/
typedef struct np_import_dtcp_mpeg_ts {
    np_size_t  mpeg_ts_descriptor_len;   /**< Len of DTCP descriptor */
    u_int8_t  *mpeg_ts_descriptor;       /**< DTCP Descriptor for MPEG Transport
                                          *   streams. Do no 0 terminate.*/
    np_size_t  hmac_len;                 /**< Length of HMAC (20 for SHA1 and 
                                          *   32 for SHA256) */
    u_int8_t  *hmac;                     /**< HMAC of the mpeg descriptor using
                                          *   the DTCP CK. Do no 0 terminate.*/
    np_size_t  cid_len;                  /**< Length of CID*/
    u_int8_t  *cid;                      /**< Content id associated with the key
                                          *   use to hmac the mpeg ts descriptor.
                                              Do not 0 terminate.*/
} np_import_dtcp_mpeg_ts_t;

/**
 * @brief Import RAW serialised ARIB descriptor type protected by hmac
 *
 * Type with ARIB usage rules. Tamper protected by an hmac. The CID defines the key.
 **/
typedef struct np_import_arib_serial_s {
    np_size_t  arib_descriptor_len;   /**< Len of serialised ARIB descriptor */
    u_int8_t  *arib_descriptor;       /**< ARIB descriptor 4 bytes in size
                                       *   arib[0] = epn, see np_import_epn_t for supported values
                                       *   arib[1] = aps, see np_import_aps_t for supported values
                                       *   arib[2] = cci, see np_import_cci_t for supported values
                                       *   arib[3] = cct, see np_import_cct_t for supported values
                                       *   Do not 0 terminate.*/
    np_size_t  hmac_len;                 /**< Length of HMAC (20 for SHA1 and 
                                          *   32 for SHA256) */
    u_int8_t  *hmac;                     /**< HMAC of the mpeg descriptor using
                                          *   the ARIB CK. Do no 0 terminate.*/
    np_size_t  cid_len;                  /**< Length of CID*/
    u_int8_t  *cid;                      /**< Content id associated with the key
                                          *   used to hmac the arib descriptor.
                                              Do not 0 terminate.*/
} np_import_arib_serial_t;

/**
 * @brief Import ARIB descriptor type
 *
 * Type to specify the arib content data
 **/
typedef struct np_import_arib {
    np_import_epn_t  epn;           /**< Source EPN */
    np_import_aps_t  aps;           /**< Source APS */
    np_import_cci_t  cci;           /**< ARIB Copy Control Information */
    np_import_cct_t  cct;           /**< ARIB Copy Control Type used to distinguish
                                     *   between full or one segment content */
} np_import_arib_t;


/**
 * @brief Import descriptor type
 *
 * Type holding non-marlin content usage rules
 **/
typedef struct np_import_desc {
    np_import_content_t          content_type;      /**< Import content type */
    union descriptor {
        np_import_dtcp_t         dtcp_desc;         /**< DTCP content description */
        np_import_dtcp_mpeg_ts_t dtcp_mpeg_ts_desc; /**< MPEGTS DTCP content description */
        np_import_arib_t         arib_desc;         /**< ARIB content description */
        np_import_arib_serial_t  arib_serial_desc;  /**< Simple serialised ARIB content description */
    } descriptor;
} np_import_desc_t;


NP_END_EXTERN_C

#endif /* NPIMPORT_TYPES_H */


