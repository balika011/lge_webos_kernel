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

#ifndef KEYMNG_API_H
#define KEYMNG_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Octopus Key Manager Implementation
 *
 * The managment system used a simple linked list to act as a "key ring",
 * storing ::keymng_l_item_tag objects in the list
 *
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 * @date   17 Dec, 2004 last modified by n-goto
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

#define ERR_KEYMNG_ALREADYREGISTERED    (ERR_KEYMNG_BASE|1) /**< Key is already in key ring */
#define ERR_KEYMNG_NOTFOUND             (ERR_KEYMNG_BASE|2) /**< Key with given id is not in key ring */
#define ERR_KEYMNG_CANNOTDECRYPTED      (ERR_KEYMNG_BASE|3) /**< Key data payload corruption */
#define ERR_KEYMNG_ILLEGALDATA          (ERR_KEYMNG_BASE|4) /**< PKCS #8 error in key data */
#define ERR_KEYMNG_NOTREGISTERED        (ERR_KEYMNG_BASE|5) /**< Key with given id is not registered in key ring  */
#define ERR_KEYMNG_CANNOTENCRYPTED      (ERR_KEYMNG_BASE|6) /**< Key data payload corruption */
#define ERR_KEYMNG_FAILEDREENCRYPTION   (ERR_KEYMNG_BASE|7) /**< Key data payload corruption */

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

#define KEYMNG_AES_CONTENT_KEY_LEN       (16)

typedef void               *keymng_handle_t;
typedef void               *keymng_key_t;
typedef void               *keymng_iterator_t;
#define KEYMNG_NULL         ((keymng_handle_t)0)

/**
 * Attribute types to apply to a key
 */
typedef enum {
    KEYMNG_ATTR_CID_HASHED          = 0, /**< Indicates that the key cid is hashed */
    KEYMNG_ATTR_KEY_EXPORTABLE      = 1, /**< Indicates the key can be exported */
    KEYMNG_ATTR_KEY_INVALID         = 2, /**< Indicates the key is invalid */
    KEYMNG_ATTR_KEY_VALID           = 3, /**< Indicates the key is valid */
    KEYMNG_ATTR_KEY_IS_CONTENT_KEY  = 4, /**< Indicates the key is a content key*/
    KEYMNG_ATTR_KEY_IMPORT_RESTRICT = 5, /**< Indicates a restricted key for encryption/HMAC only*/
} keymng_key_attr_t;


/**
 * Type of key handled by the keymng
 */
typedef enum {
    KEYMNG_OCTOBJ_KEY_SHARED  = 0,   /* Octobj shared key */
    KEYMNG_OCTOBJ_KEY_PRIVATE = 1,   /* Octobj private key */
    KEYMNG_OCTOBJ_KEY_PUBLIC  = 2,   /* Octobj public key */
    KEYMNG_SIMPLE_KEY         = 3,   /* Simple key */
} keymng_key_type_t;


/**
 *
 */
typedef struct {
    keymng_key_type_t       key_type;
    union {
        void                *hci_key_ref;  /** hci_key_ref_t */
        octobj_key_t        *oct_key;
        octobj_pairedkey_t  *paired_key;
    } key;
} keymng_key_data_t;


/**
 * Initialize a ::keymng_l_context_t type and return the handle to the zeroed structure
 *
 * @param[out] out_handle Handle to the context
 * @return     ::ERR_OK
 * @return     ::ERR_NOMEM
 */
extern retcode_t
keymng_Init(keymng_handle_t   *out_handle);

/**
 * Destroy a ::keymng_l_context_t type
 *
 * @param[in,out] io_handle Handle to the context
 */
extern void
keymng_Fin(keymng_handle_t    io_handle);

/**
 * Add a key to the key ring
 *
 * @param[in,out] io_handle  Handle to the ::keymng_l_context_t type
 * @param[in]     in_key     Octopus key object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_KEYMNG_ALREADYREGISTERED
 * @return ::ERR_NOMEM
 */
extern retcode_t
keymng_RegistKey(keymng_handle_t     io_handle,
                 keymng_key_data_t  *in_key_data,
                 keymng_key_t       *out_key);

/**
 * Add a key pair to the key ring
 *
 * @param[in,out] io_handle     Handle to the ::keymng_l_context_t type
 * @param[in]     in_pairedkey  Octopus key pair object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_KEYMNG_ALREADYREGISTERED
 * @return ::ERR_NOMEM
 */
extern retcode_t
keymng_RegistPairedKey(keymng_handle_t      io_handle,
                       keymng_key_data_t   *in_key_data,
                       keymng_key_t        *out_key);

/**
 * Remove a key from the key ring
 *
 * @param[in,out] io_handle  Handle to the ::keymng_l_context_t type
 * @param[in]     in_key     Handle to the Octopus key object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_KEYMNG_NOTREGISTERED
 * @return ::ERR_NOMEM
 */
extern retcode_t
keymng_DeregKey(keymng_handle_t     io_handle,
                binstr_t           *in_key_id);

/**
 * @brief Get a key from the key ring. In case of BKB, i.e., in_bkb != NULL,
 * the key is decryted by calling starfish as well.
 *
 * @param[in,out] io_handle       Handle to the ::keymng_l_context_t type
 * @param[in]     in_id           Id of the key in the key ring, represented
 *                                as a string
 * @param[in]     in_bkb          BKB data
 * @param[out]    out_key         Pointer to the stored key in the key ring
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_KEYMNG_NOTFOUND
 * @return ::ERR_NOMEM
 * @return ::ERR_KEYMNG_CANNOTDECRYPTED
 * @return Returns from ::hci_*
 */
extern retcode_t
keymng_GetBkbProtectedKey(keymng_handle_t     io_handle,
                          binstr_t           *in_id,
                          octobj_bkb_t       *in_bkb);


/**
 * @brief Retrieve a key based on it's associated ID
 *
 * @param[in]   in_handle       Handle to the ::keymng_l_context_t type
 * @param[in]   in_id           Key ID
 * @param[out]  out_key         Found Key
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_KEYMNG_NOTFOUND
 * @return ::ERR_KEYMNG_CANNOTDECRYPTED
 * @return Returns from ::hci_*
 */
extern retcode_t
keymng_GetKey(keymng_handle_t     in_handle,
              binstr_t           *in_id,
              keymng_key_t       *out_key);

/**
 * @brief Find the decrypted content key from the keyring and re-encrypt
 * it with the Sink's device key
 *
 * @param[in]     in_handle             Handle to the ::keymng_l_context_t type.
 * @param[in]     in_sink_key           The public key of the sink device (expected
 *                                      to be the rsa public key parameters).
 * @param[in]     in_keyid              Id of the key in the key ring, represented as a string.
 * @param[out]    out_new_contentkey    Re-encrypted content key
 * @param[out]    out_midkey_encryption If re-encryption required a midkey for size reasons,
 *                                      this will be populated with the RSA protected mid key.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_KEYMNG_NOTFOUND
 */
extern retcode_t
keymng_ReencryptCK( keymng_handle_t      in_handle,
                    rsa_key_t           *in_sink_key,
                    binstr_t            *in_keyid,
                    binstr_t           **out_new_contentkey,
                    binstr_t           **out_midkey_encryption);


/**
 * @brief Decrypt the mid key by the Source device key, and re-encrypt it
 * with the Sink's device key.
 *
 * @param[in]     in_handle             Handle to the ::keymng_l_context_t type.
 * @param[in]     in_sink_key           The public key of the sink device (expected
 *                                      to be the rsa public key parameters).
 * @param[in]     in_keyid              Id of the key in the key ring, represented
 *                                      as a string.
 * @param[out]    out_new_midkey        Re-encrypted mid key.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_KEYMNG_NOTFOUND
 * @return ::ERR_KEYMNG_CANNOTDECRYPTED
 */
extern retcode_t
keymng_ReencryptMidKey(    keymng_handle_t      in_handle,
                           rsa_key_t           *in_sink_key,
                           binstr_t            *in_keyid,
                           binstr_t           **out_new_midkey);


/**
 * @brief Calculate HMAC value with the key in_key,
 * and check whether the result is the same as in_digest or not.
 */
extern retcode_t
keymng_ck_CheckHMAC( int32_t       in_digest_algo,
                     hci_key_ref_t in_key,
                     binstr_t     *in_data,
                     binstr_t     *in_digest );

/**
 * @brief Calculate SHA1/256 value of serialized octopus-ContentKey object
 * where ck is in_key, and check whether the result is the same as in_digest or not.
 */
extern retcode_t
keymng_ck_CheckDigest( int32_t       in_digest_algo,
                       hci_key_ref_t in_key,
                       size_t        in_s11nlen,
                       u_int8_t     *in_s11nbuf,
                       size_t        in_digestlen,
                       u_int8_t     *in_digestbuf);



/**
 * @brief Associate the supplied cid with a key in the keymng
 *
 * @param[in,out] io_handle       Handle to the ::keymng_l_context_t type.
 * @param[in]     in_key_id       key ID
 * @param[in]     in_cid          Content ID to associate with the specified key
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_NG
 */
extern retcode_t
keymng_RegistCID(keymng_handle_t      io_handle,
                 binstr_t            *in_key_id,
                 binstr_t            *in_cid);

/**
 * @brief Create a key and add it to the key ring
 *
 * @param[in,out] io_handle       Handle to the ::keymng_l_context_t type.
 * @param[in]     in_type         Key type (shared, public, private, simple)
 * @param[in]     in_num_cids     Number of content ids associated with the key, or 0
 * @param[in]     in_cids         Content IDs associated with this key, or NULL if in_num_cids is 0
 * @param[out]    out_key         Resulting key as an opaque type
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
extern retcode_t
keymng_CreateKey(keymng_handle_t      io_handle,
                 keymng_key_type_t    in_type,
                 np_size_t            in_num_cids,
                 binstr_t           **in_cids,
                 keymng_key_t        *out_key);

/**
 * @brief Associate a HCI reference with a key
 *
 * @param[in, out] io_key              Opaque key from a key ring
 * @param[in]      in_iv               Initialisation vector for the key
 * @param[in]      in_valid_key_index  Flag specifiying if the key index passed as an argument is valid
 * @param[in]      in_key_index        Index of the key in the key ring.
 * @param[in]      in_key_ref          HCI key
 * @param[in]      in_key_ref_type     HCI key type
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
keymng_LoadHciKey(keymng_key_t    io_key,
                  binstr_t       *in_iv,
                  bool_t          in_valid_key_index,
                  u_int32_t       in_key_index,
                  hci_key_ref_t   in_key_ref,
                  u_int32_t       in_key_ref_type);

/**
 * @brief Find a key in the key ring based on the content ID
 *
 * @param[in]     in_handle       Handle to the ::keymng_l_context_t type.
 * @param[in]     in_cid          Content ID. This can be NULL. A key with no CID
 *                                associated with it will be returned if it exists
 * @param[out]    out_key         Found key
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_KEYMNG_NOTFOUND
 */
extern retcode_t
keymng_FindKey(keymng_handle_t    in_handle,
               binstr_t          *in_cid,
               keymng_key_t      *out_key);

/**
 * @brief Get a HCI reference from a key in the key ring
 *
 * @param[in]   in_key   Key from the key ring
 * @param[out]  out_key  HCI reference
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_KEYMNG_NOTFOUND
 */
extern retcode_t
keymng_GetHciRef(keymng_key_t in_key, hci_key_ref_t *out_key_ref);

/**
 * @brief Set a key attribute
 *
 * @param[in,out] io_key  Key containing the attributes to change
 * @param[in]     in_attr Attribute value to set
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
keymng_SetAttr(keymng_key_t io_key, keymng_key_attr_t in_attr);

/**
 * @brief Check the validity of key attribute
 *
 * @param[in]  in_key     Key containing the attributes to check
 * @param[in]  in_attr    Attribute value to check
 * @param[out] out_enable Output flag signalling the attribute status
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
keymng_CheckAttr(keymng_key_t in_key, keymng_key_attr_t in_attr, bool_t *out_enabled);

/**
 * @brief Clear all keys of a certain attribute
 *
 * @param[in,out] io_handle       Handle to the ::keymng_l_context_t type.
 * @param[in]     in_attr         Attribute value to clear
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
keymng_ClearAttr(keymng_handle_t io_handle, keymng_key_attr_t in_attr);

/**
 * @brief Set the export flag in the attributes of the keys in the key ring
 *
 * @param[in,out]  io_handle  Handle to the key ring
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::keymng_SetAttr
 */
extern retcode_t
keymng_MarkExportable(keymng_handle_t io_handle);

/**
 * @brief Starts an iterator for use by ::keymng_GetNextCK,
 *        and returns the number of content keys in a key ring
 *
 * @param[in]     in_keymng_handle Handle to the keymng context (ring) holding
 *                                 the keymng keys including the content keys
 * @param[in,out] io_ck_iterator   the address for the iterator; it is typically
 *                                 a local variable of the caller function;
 *                                 it need not be initialized before calling this function;
 * @param[out]    out_num_cks      Number of content keys
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ILLEGALHANDLE
 */
extern retcode_t
keymng_StartCKIterator(keymng_handle_t    in_handle,
                       keymng_iterator_t *io_ck_iterator,
                       np_size_t         *out_num_cks);

/**
 * @brief Get the next content key
 *
 * @param[in,out] io_ck_iterator  Iterator on the keymng key ring
 * @param[out]    out_ck_p        pointer to the next Content key
 *
 * @note After all the content keys have been iterated, extra calls of this function would return ERR_INTERNALERROR
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
keymng_GetNextCK(keymng_iterator_t *io_ck_iterator, keymng_key_t *out_ck_p);

/**
 * @brief Get Content IDs associated with a content key
 *
 * @param[in]    in_key            The content key
 * @param[out]   out_num_cids      The number of content IDs associated with the content key
 * @param[out]   out_cids          The array with the duplicated content ID from in_key
 *
 * @note The caller must free the array and all the array's slots
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
extern retcode_t
keymng_GetContentIDs(keymng_key_t    *in_key,
                     np_size_t       *out_num_cids,
                     binstr_t      ***out_cids);

/**
 * @brief Creates an encrypted version of a content key, a.k.a as <SecretKey> in XML
 *
 * @note  Encrypts the content key by the BK, if available, and then by device's Scuba secret key
 *
 * @param[in]   in_ck               The content key to encrypt
 * @param[in]   in_encrypt_with_bk  TRUE if and only if ck must be encrypted with BK first
 * @param[in]   in_handle           Ignored.  In the future, handle to a key ring
 *                                  containing the Scuba keys for encrypting the content key
 * @param[in]   in_enckey_id        NULL (in the future, this may select a node's Scuba key)
 * @param[in]   in_enc_algo         Encryption Algorithm.
 *                                  For N4.0 OCTOBJ_KEY_ALGORITHM_AES128CBC (use secret key) is supported.
 * @param[in]   in_uid              uid of the new secret key
 * @param[in]   in_type             Key type. Content, broadcast etc.
 * @param[out]  out_secret_key      The encrypted content key
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_KEYMNG_NOTFOUND
 * @return ::ERR_KEYMNG_CANNOTDECRYPTED
 * @return ::hci_CreateKeyRef
 * @return ::hci_EncryptKeyData
 * @return ::octobj_CreateSecretKey
 */
extern retcode_t
keymng_CreateSecretKey(keymng_key_t        in_ck,
                       bool_t              in_encrypt_with_bk,
                       keymng_handle_t     in_handle,
                       binstr_t           *in_enckey_id,
                       u_int8_t            in_enc_algo,
                       binstr_t           *in_id,
                       hci_key_type_t      in_type,
                       octobj_key_t      **out_secret_key);

NP_END_EXTERN_C

#endif /* KEYMNG_API_H */
/** @} */
