/** @addtogroup keymng keymng
 * @ingroup marlin
 * @{
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */
#ifndef KEYMNG_TYPES_H
#define KEYMNG_TYPES_H

NP_BEGIN_EXTERN_C
/**
 * @file
 * Internal structures for handling key manager objects
 */

#define KEYMNG_L_ATTR_RESET                 0
#define KEYMNG_L_ATTR_CID_HASHED            1
#define KEYMNG_L_ATTR_KEY_EXPORTABLE        2
#define KEYMNG_L_ATTR_KEY_INVALID           4
#define KEYMNG_L_ATTR_KEY_IS_CONTENT_KEY    8
#define KEYMNG_L_ATTR_KEY_IMPORT_RESTRICT  16

#define KEYMNG_L_ATTR_SET(state, attr) ((state) = (state | (1 << attr)))
#define KEYMNG_L_ATTR_CLR(state, attr) ((state) = (state & (0xffffffff ^ (1 << attr))))
#define KEYMNG_L_ATTR_CHK(state, attr) ((state >> attr) & 1)

/**
 * Structure that wraps keys stored in the management subsystem
 */
typedef struct keymng_l_item_tag {
    keymng_key_type_t         type;            /**< Type of key held by the keymng */
    u_int32_t                 attrs;           /**< Key attributes */
    np_size_t                 num_cids;        /**< number of cids associated with the key */
    binstr_t                **cids;            /**< String identifier for content ids associated with the key */
    binstr_t                 *id;              /**< String identifier for key */
    bool_t                    valid_key_index; /**< Flag indicating if the index is valid */
    u_int32_t                 key_index;       /**< Index of the key in the keymng */
    octobj_key_t             *key;             /**< The real key stored as an Octopus object  */
    bool_t                    key_is_owned;    /**< Flag indicating if we own the pointer to the octopus object */
    binstr_t                 *iv;              /**< IV associated with the key */
    struct keymng_l_item_tag *sibling;         /**< Other key if this is a part of a key-pair  */
    struct keymng_l_item_tag *next;            /**< Next key in the chain  */
    binstr_t                 *sf_key;          /**< Starfish key data */
    hci_key_ref_t             key_ref;         /**< the key reference */
} keymng_l_item_t;

/**
 * All purpose key-ring type structure
 *
 * The structure is an array of ::keymng_l_item_t objects
 */
typedef struct {
    keymng_l_item_t      *keys; /**< Stored keys  */
    binstr_t            **devkey_ids; /**< ids of OPPR keys   */
    u_int32_t             devkey_ids_num; /**< number of devkey_ids; */
} keymng_l_context_t;

NP_END_EXTERN_C

#endif /* KEYMNG_TYPES_H */
/** @} */
