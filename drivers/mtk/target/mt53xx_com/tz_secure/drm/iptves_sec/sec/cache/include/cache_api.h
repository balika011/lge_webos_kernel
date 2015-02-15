/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CACHE_API_H
#define CACHE_API_H

#include "basic_types_iptves.h"

#include <po_basictypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Cache API
 *
 * require: nvs, base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    CACHE_TAG_TA   = 0,    /**< Trust Anchor */
    CACHE_TAG_PKIPATH,     /**< Device Pki-Path */
    CACHE_TAG_PRIVKEY,     /**< Device Private Key */
    CACHE_TAG_CRLFLAG,     /**< CLR Flag */
    CACHE_TAG_CRL,         /**< Certificate Revocation List */
    CACHE_TAG_VODKEY,      /**< VOD Keys for EXTRACT */
    CACHE_TAG_IPMCKEY,     /**< IP-Multicast Keys for EXTRACT */
    CACHE_TAG_DLKEY,       /**< Download Keys for EXTRACT */
    CACHE_TAG_DLCPKEY,     /**< Download Keys for EXPORT Copy */
    CACHE_TAG_UNAVAILABLE, /**< Uncommitted Keys*/
    CACHE_TAG_INUSEKEY,    /**< In Use Download Keys for EXPORT Copy */
    CACHE_TAG_MAX,
} cache_tag_t;

typedef enum {
    CACHE_ATTR_NONE = -1,   /**< Default (Attribute is not set yet) */
    CACHE_ATTR_READONLY,    /**< Object is only read */
    CACHE_ATTR_READWRITE,   /**< Object is read/written/updated/deleted */
    CACHE_ATTR_VOLATILE,    /**< Object is not saved to NVS */
    CACHE_ATTR_MAX,
} cache_attr_t;

typedef enum {
    CACHE_ACT_REFER = 0,    /**< Object reference is returned */
    CACHE_ACT_COPY,         /**< Object is copied */
} cache_act_t;

typedef u_int32_t           cache_tkt_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_CACHE_RODATA_CORRUPTED      (RET_CACHE_BASE|1)
#define RET_CACHE_LOAD_ERROR            (RET_CACHE_BASE|2)
#define RET_CACHE_SAVE_ERROR            (RET_CACHE_BASE|3)
#define RET_CACHE_REMOVE_ERROR          (RET_CACHE_BASE|4)
#define RET_CACHE_PARSE_ERROR           (RET_CACHE_BASE|5)
#define RET_CACHE_INVALID_OBJECT        (RET_CACHE_BASE|6)
#define RET_CACHE_BUFFER_SHORT          (RET_CACHE_BASE|7)
#define RET_CACHE_PERMISSION_DENIED     (RET_CACHE_BASE|8)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_RODATA_CORRUPTED   Fail to load read-only NVS
 * @retval RET_CACHE_LOAD_ERROR         Fail to load read-write NVS
 * @retval RET_CACHE_PARSE_ERROR        Fail to parse serialized data
 */
extern int
cache_Init(void);

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_SAVE_ERROR         Fail to save read-write NVS
 */
extern int
cache_Fin(void);

/**
 * Remove Read-Write NVS (not MT-safe)
 *
 * @retval RET_OK                       Success
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_REMOVE_ERROR       Fail to remove read-write NVS
 */
extern int
cache_CleanupNVS(void);

/**
 * Begin transaction and acquire lock
 *
 * @param [in/out] io_tkt               Ticket for transaction
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
cache_Begin(cache_tkt_t        *io_tkt);

/**
 * Commit transaction and release lock
 *
 * @param [in/out] io_tkt               Ticket for transaction
 * @param [in]     in_is_commit         If TRUE, it means commit, else, it means rollback.
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_SAVE_ERROR         Fail to save read-write NVS
 * @retval RET_CACHE_PERMISSION_DENIED  Read-only object is overwritten/deleted.
 */
extern int
cache_Commit(cache_tkt_t       *io_tkt,
             bool_t             in_is_commit);

/**
 * Set object to cache
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in/out] io_tkt               Ticket for transaction
 * @param [in]     in_tag               Object type
 * @param [in]     in_attr              Object attribute
 * @param [in]     in_key               Database key
 * @param [in]     in_comp              Callback function for compare object with key
 * @param [in]     in_obj               Data
 * @param [in]     in_obj_len           Data length
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_INVALID_OBJECT     Different length object is set to the object list
 * @retval RET_CACHE_PERMISSION_DENIED  Read-only object is overwritten/deleted.
 */
extern int
cache_SetObject(

				po_thid_t       in_thid,
                cache_tkt_t    *io_tkt,
                cache_tag_t     in_tag,
                cache_attr_t    in_attr,
                const void     *in_key,
                bool_t        (*in_comp)(const void *, const void *),
                const u_int8_t *in_obj,
                u_int32_t       in_obj_len);

/**
 * Get object from cache
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in/out] io_tkt               Ticket for transaction
 * @param [in]     in_tag               Object type
 * @param [in]     in_act               Action
 * @param [in]     in_key               Database key
 * @param [in]     in_comp              Callback function for compare object with key
 * @param [in,out] io_obj               Data
 * @param [in,out] io_obj_len           Data length
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_BUFFER_SHORT       @p io_obj length is not enough to get object
 * @retval RET_CACHE_PERMISSION_DENIED  An object other than read-only is referred
 */
extern int
cache_GetObject(po_thid_t       in_thid,
                cache_tkt_t    *io_tkt,
                cache_tag_t     in_tag,
                cache_act_t     in_act,
                const void     *in_key,
                bool_t        (*in_comp)(const void *, const void *),
                u_int8_t      **io_obj,
                u_int32_t      *io_obj_len);

/**
 * Get object number
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_tag               Object type
 * @param [out]    out_num              Object number
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
cache_GetObjectNum(po_thid_t    in_thid,
                   cache_tag_t  in_tag,
                   u_int32_t   *out_num);

/**
 * Get next object
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_tag               Object type
 * @param [in]     in_parse             Callback function for parse object
 * @param [in,out] io_num               Object num
 * @param [out]    out_obj_lst          Object list
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CACHE_BUFFER_SHORT       @p io_num is not enough to get objects
 */
extern int
cache_GetObjectList(po_thid_t   in_thid,
                    cache_tag_t in_tag,
                    bool_t    (*in_parse)(const void *, void *, u_int32_t),
                    u_int32_t  *io_num,
                    void       *out_obj_lst);

#ifdef __cplusplus
}
#endif

#endif /* CACHE_API_H */
