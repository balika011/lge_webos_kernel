/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 */
/** @addtogroup nsff CENC functionality
 *  @ingroup nsff
 * @{
 *
 * @brief nsff layer APIs for cenc
 *
 *
 * @file
 * cenc APIs
 *
 */

#ifndef NSFF_CENC_API_H
#define NSFF_CENC_API_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                     PUBLIC DATA STRUCTURES                                 */
/* -------------------------------------------------------------------------- */
#define NP_CENC_KID_LEN           16
#define NP_CENC_SCHI_HMAC_LEN     32

/**
 * cenc scheme type
 **/
typedef enum {
    NP_CENC_MCEC = 0                 /** MCEC Scheme type */
} np_cenc_scheme_t;


/**
 * cenc 'tenc' default algorithm id
 **/
typedef enum {
    NP_CENC_ALGOID_NO_ENC    = 0,   /** No Encryption */
    NP_CENC_ALGOID_AES128CTR = 1    /** AES CTR */
} np_cenc_tenc_algo_id_t;

/**
 * KID type
 */
typedef u_int8_t   np_cenc_kid_t[NP_CENC_KID_LEN];

/**
 * cenc 'mkid' map
 *
 **/
typedef struct np_cenc_mkid_map_tag {
    np_cenc_kid_t     kid;                       /** Key Identifier found in the content */
    np_str_t         *content_id;                /** Marlin Content Id associated to the KID */
} np_cenc_mkid_map_t;

/**
 * cenc 'pssh' box structure
 **/
typedef struct np_cenc_pssh_box_tag {
    u_int32_t            num_kid_cids;  /** If 0 then cid_kid_map will be NULL */
    np_cenc_mkid_map_t  *kid_cid_map;   /** Array of KID's mapped to their CID's */
    u_int32_t            num_licenses;  /** Number of licenses. If 0 then licenses will be NULL */
    np_str_t           **licenses;      /** Array of string pointers (each holding one license). If NULL no licenses are present */
} np_cenc_pssh_box_t;


/**
 * cenc 'satr' box structure
 **/
typedef struct np_cenc_satr_box_tag {
    u_int32_t    name;                 /** 4CC Box type */
    np_size_t    len;                  /** Length of the Box body */
    u_int8_t    *val;                  /** Pointer to the start of the box body */
} np_cenc_satr_box_t;

/**
 * cenc 'tenc' box structure
 **/
typedef struct np_cenc_tenc_box_tag {
    np_cenc_tenc_algo_id_t   default_algo_id;                   /** Default algorithm ID 0x0 - No Encryption 0x01 - AES CTR */
    u_int8_t                 default_iv_size;                   /** Default IV size 8 or 16 */
    np_cenc_kid_t            default_kid;                       /** Default KID */
} np_cenc_tenc_box_t;

typedef struct np_cenc_sinf_box_tag {
    np_cenc_scheme_t      scheme_type;
    u_int32_t             num_kid_cids;                 /** If 0 then cid_kid_map will be NULL */
    np_cenc_mkid_map_t   *kid_cid_map;                  /** Array of KID's mapped to their CID's */
    np_cenc_tenc_box_t   *tenc_info;                    /** Track Encryption info */
    np_size_t             satr_box_len;                 /** Length of the full satr_box, if 0 satr_box will be NULL */
    u_int8_t             *satr_box;                     /** Complete satr box to be used for HMAC verification */
    u_int32_t             num_satr_boxes;               /** Number of boxes inside the satr box, if 0 the satr_boxes will be NULL */
    np_cenc_satr_box_t   *satr_boxes;                   /** Array of boxes found in schi's satr like styp, sgna, asig, cert */
    u_int8_t              hmac[NP_CENC_SCHI_HMAC_LEN];  /** Value in schi's hmac box calculated on satr_box */
    np_str_t             *license;                      /** The license found in schi's 8bdl box */
} np_cenc_sinf_box_t;

/* -------------------------------------------------------------------------- */
/*                      PUBLIC API                                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Parses a 'pssh' box from supplied data
 *
 * @param[in]  in_pssh_len        'pssh' Box length
 * @param[in]  in_pssh            Binary string containing a 'pssh' box
 * @param[out] out_pssh_box_info  Parsed 'pssh' box structure
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_CENC_INVALID_PSSH
 * @return ::ERR_NP_CENC_UNKNOWN_SYSTEMID
 **/
extern np_ret_t
nsff_cencParsePsshBox(np_size_t            in_pssh_len,
                      u_int8_t            *in_pssh,
                      np_cenc_pssh_box_t **out_pssh_box_info);

/**
 * @brief Free the Parsed 'pssh' box structure
 *
 * @param[in,out] io_pssh_box_info  Parsed 'pssh' box structure
 *
 * @return ::ERR_NP_OK
 **/
extern np_ret_t
nsff_cencFreePsshBox(np_cenc_pssh_box_t *io_pssh_box_info);

/**
 * @brief Parses a 'sinf' box from supplied data
 *
 * @param[in]  in_sinf_len       'sinf' Box length
 * @param[in]  in_sinf           Binary string containing a 'sinf' box
 * @param[out] out_sinf_box_info Parsed 'sinf' box structure
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_CENC_INVALID_SINF
 **/
extern np_ret_t
nsff_cencParseSinfBox(np_size_t            in_sinf_len,
                      u_int8_t            *in_sinf,
                      np_cenc_sinf_box_t **out_sinf_box_info);

/**
 * @brief Free the Parsed 'sinf' box structure
 *
 * @param[in,out] io_sinf_box_info  Parsed 'sinf' box structure
 *
 * @return ::ERR_NP_OK
 **/
extern np_ret_t
nsff_cencFreeSinfBox(np_cenc_sinf_box_t *io_sinf_box_info);

/**
 * @brief Gets the CID by generating one or finding it in the given mkid table.
 * If in_num_kid_cids is 0 and in_mkid_table is NULL then the CID will be generated
 * as described in [MAS 2.3.1]. Else the CID is retrieved from the given KID CID table
 * using KID as a lookup.
 *
 * @param[in]  in_kid            Binary string containing a KID
 * @param[in]  in_num_kid_cids   Number of KID CID mappings in the table
 * @param[in]  in_mkid_table     Table of KID CID mappings.
 * @param[out] out_cid           Generated or found CID based the given input.
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_CENC_NOKIDFOUND
 *
 */
extern np_ret_t
nsff_cencGetCID(np_cenc_kid_t        in_kid,
                u_int32_t            in_num_kid_cids,
                np_cenc_mkid_map_t  *in_mkid_table,
                np_str_t           **out_cid);

/**
 * @brief Generate a pssh box containing an mkid table
 *
 * @param[in]  in_apid_len     Length of the Asset Physical ID
 * @param[in]  in_apid         Asset Physical ID
 * @param[in]  in_kid_num      Number of key IDs
 * @param[in]  in_kids         Array of key IDs, which must all be of length ::NP_KID_LEN
 * @param[out] out_pssh        Generated pssh box as a binary string
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 */
extern np_ret_t
nsff_cencGeneratePsshBoxForDECE(np_size_t       in_apid_len,
                                u_int8_t       *in_apid,
                                u_int32_t       in_kid_num,
                                np_cenc_kid_t  *in_kids,
                                np_str_t      **out_pssh);


/**
 * @brief Add a license to a 'pssh' box
 *
 * @param[in]  in_pssh_len        'pssh' Box length
 * @param[in]  in_pssh            Binary string containing a 'pssh' box
 * @param[in]  in_license_len     license length
 * @param[in]  in_license         Binary string containing a license
 * @param[out] out_pssh           pssh box as a binary string containing the license
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_CENC_INVALID_PSSH
 * @return ::ERR_NP_CENC_UNKNOWN_SYSTEMID
 *
 * @note If unknown boxes are in the input pssh, they will be removed in the ouput pssh.
 **/
extern np_ret_t
nsff_cencAdd8bdlLicensePsshBox(np_size_t   in_pssh_len,
                               u_int8_t   *in_pssh,
                               np_size_t   in_license_len,
                               u_int8_t   *in_license,
                               np_str_t  **out_pssh);

NP_END_EXTERN_C

#endif /* NSFF_CENC_API_H */
/** @} */

