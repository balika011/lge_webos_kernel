/** @addtogroup decr decr
 * @ingroup marlin
 * @{
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef DECR_TYPE_H
#define DECR_TYPE_H

NP_BEGIN_EXTERN_C

/**
 * Internal decr structure referenced by decr_handle_t types.
 *
 * This structure contains the type of decr context, its key,
 * the key ID and the decryption info.
 */
typedef struct {
    u_int32_t     type;     /**< One of ::DECR_TYPE_CLEAR, ::DECR_TYPE_NOT_SPECIFIED,
                                       ::DECR_TYPE_ISMACRYP, ::DECR_TYPE_KFCBC,
                                       ::DECR_TYPE_UNDEFINED, ::DECR_TYPE_BBTS_SKL,
                                       ::DECR_TYPE_BBTS_AES128CBC */
    keymng_key_t  key;      /**< Key associated with the decryptor track */
    u_int32_t     keyindex; /**< Index of the key */
    binstr_t     *cid;      /**< Id of the key associated with the decr context */
    decr_info_t   info;     /**< The decryption structure, the contents of which
                                 depend on the incoming type */
} decr_l_context_t;

NP_END_EXTERN_C

#endif /* DECR_TYPE_H */
/** @} */
