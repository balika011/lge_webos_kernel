/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef PO_BASICTYPES_H
#define PO_BASICTYPES_H

#include <po_basictypes_.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Basic type and macro definition
 *
 * require: none
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/**
 * def: TRUE, FALSE
 */
#ifndef TRUE
#  define TRUE   1
#endif
#ifndef FALSE
#  define FALSE  0
#endif

/**
 * macro: data length
 */
#define PO_URR_LEN              16
#define PO_CKEY_ID_LEN          6
#define PO_TIERBITS_LEN         8
#define PO_CKEY_LEN             16
#define PO_ECM_LEN              98
#define PO_DEVICE_ID_LEN        8
#define PO_EXPORT_INFO_MAX_LEN  16

/**
 * macro: action id (for po_key_ref_t)
 */
#define PO_ACTION_ID_EXTRACT_VOD    0x01
#define PO_ACTION_ID_EXTRACT_IPMC   0x02
#define PO_ACTION_ID_EXTRACT_DL     0x03
#define PO_ACTION_ID_EXPORT_CP_DL   0x10
#define PO_ACTION_ID_EXPORT_MV_DL   0x11
#define PO_ACTION_ID_EXPORT_IPMC    0x12
#define PO_ACTION_ID_RECORD         0x20
#define PO_ACTION_ID_ALL            0xFF

/**
 * macro: action parameter (for po_key_ref_t)
 */
#define PO_ACTION_PARAM_EXTRACT                             0xFF
#define PO_ACTION_PARAM_EXPORT_DTCP                         0x00
#define PO_ACTION_PARAM_EXPORT_CPRM_DVD                     0x01
#define PO_ACTION_PARAM_EXPORT_CPRM_SDV                     0x02
#define PO_ACTION_PARAM_EXPORT_CPRM_SDA                     0x03
#define PO_ACTION_PARAM_EXPORT_MGR_SVR_MEMSTICKPRO          0x04
#define PO_ACTION_PARAM_EXPORT_MGR_SAR_MEMSTICK_MEMSTICKPRO 0x05
#define PO_ACTION_PARAM_EXPORT_VCPS                         0x06
#define PO_ACTION_PARAM_EXPORT_MGR_SVR_EMPR                 0x07
#define PO_ACTION_PARAM_EXPORT_MGR_SAR_ATRAC                0x08
#define PO_ACTION_PARAM_EXPORT_SAFIA_IVDR_TV_REC            0x09
#define PO_ACTION_PARAM_EXPORT_SAFIA_IVDR_AUDIO_REC         0x0A
#define PO_ACTION_PARAM_EXPORT_AACS_BLURAY_BDR_RE           0x0B
#define PO_ACTION_PARAM_EXPORT_AACS_BLURAY_RED_LASER        0x0C
#define PO_ACTION_PARAM_RECORD_ARIB                         0x01

/**
 * macro: key flag (for po_key_info_t)
 */
#define PO_KEY_FLAG_CKEY_ID             (0x01<<0)
#define PO_KEY_FLAG_PRIV_DATA           (0x01<<1)
#define PO_KEY_FLAG_TIERBITS            (0x01<<2)
#define PO_KEY_FLAG_OCI                 (0x01<<3)
#define PO_KEY_FLAG_EXPORT_INFO         (0x01<<4)
#define PO_KEY_FLAG_NOT_BEFORE_AFTER    (0x01<<5)

/**
 * macro: request type (for po_req_t)
 */
#define PO_REQ_TYPE_GETPERM     0
#define PO_REQ_TYPE_GETTT       1

/*============================================================================
 * Type Definitions
 *==========================================================================*/
/**
 * type: return code
 */

typedef u_int32_t po_int;

/**
 * type: key reference
 */

typedef struct {
    u_int8_t     urr[PO_URR_LEN];
    u_int8_t     urr_len;
    u_int8_t     action_id;
    u_int8_t     action_param;
} po_key_ref_t;

/**
 * type: key information
 */

typedef struct {
    u_int8_t     flags;
    u_int8_t     ckey_id[PO_CKEY_ID_LEN];
    u_int16_t    priv_data;
    u_int8_t     tierbits[PO_TIERBITS_LEN];
    u_int16_t    oci;
    u_int8_t     export_info[PO_EXPORT_INFO_MAX_LEN];
    u_int16_t    export_info_len;
    u_int32_t    not_before;
    u_int32_t    not_after;
    u_int8_t     num;
    u_int8_t     num_in_use;
} po_key_info_t;

/**
 * type: request
 */

typedef struct {
    u_int8_t     type;
    u_int8_t     num;
    po_key_ref_t key_ref;
} po_req_t;

/**
 * type: trust hierarchy id
 */


typedef enum {
    PO_THID_COMMON = 0,
    PO_THID_RTDB,
    PO_THID_MAX,
} po_thid_t;

#ifdef __cplusplus
}
#endif

#endif /* PO_BASICTYPES_H */
