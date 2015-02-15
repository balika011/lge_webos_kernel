/** @addtogroup hci hci
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
 */

#ifndef HCI_API_H
#define HCI_API_H

#include "hci_basic_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* -------------------------------------------------------------------------- */
    /*                     PUBLIC DATA STRUCTURES                                 */
    /* -------------------------------------------------------------------------- */

    /** Abstract type for an hci lock */
    typedef void *hci_lock_t;

    /** Abstract type for a reference to a key protected by the hci interface */
    typedef void *hci_key_ref_t;

    typedef enum {
        HCI_FALSE = 0,
        HCI_TRUE  = 1,
    } hci_bool_t;

    typedef enum  {
        HCI_KEY_TYPE_BOOT           = 0, /**< Bootstrap signing key */
        HCI_KEY_TYPE_OPPR_PRV       = 1, /**< Octopus private rsa key */
        HCI_KEY_TYPE_OPPR_SEC       = 2, /**< Octopus secret aes key */
        HCI_KEY_TYPE_NPPR_SIG       = 3, /**< Nemo private rsa key (signing) */
        HCI_KEY_TYPE_NPPR_ENC       = 4, /**< Nemo private rsa key (encoding) */
        HCI_KEY_TYPE_LOPR_SIG       = 5, /**< License Service signing key */
    } hci_key_type_t;

    typedef enum  {
        HCI_IMP_KEY_TYPE_CONTENT    = 0, /**< Content key */
        HCI_IMP_KEY_TYPE_DOMAIN     = 1, /**< Domain key */
        HCI_IMP_KEY_TYPE_BROADCAST  = 2, /**< Broadcast key */
        HCI_IMP_KEY_TYPE_MESSAGE    = 3  /**< Nemo message key */
    } hci_imp_key_type_t;

    /**
     * HCI Signature Algorithm types
     *
     * Type to specify the algorithm for a signing operation
     **/
    typedef enum  {
        HCI_SIG_ALGO_RSA_PKCS1_V1_5_SHA1   = 0, /**< Use SHA1 hashing before signing */
        HCI_SIG_ALGO_RSA_PKCS1_V1_5_SHA256 = 1  /**< Use SHA256 hashing before signing */
    } hci_sig_algo_t;

    /**
     * HCI Encryption Algorithm types
     *
     * Type to specify the algorithm for an encryption operation
     **/
    typedef enum  {
        HCI_ENC_ALGO_RSA  = 0, /**< RSA */
        HCI_ENC_ALGO_AES  = 1  /**< AES */
    } hci_enc_algo_t;

    /**
     * HCI Encryption Mode types
     *
     * Type to specify the mode for an encryption operation
     **/
    typedef enum  {
        HCI_ENC_MODE_NONE  = 0, /**< No mode used */
        HCI_ENC_MODE_CBC   = 1, /**< Cipher block chaining mode */
        HCI_ENC_MODE_CTR   = 2, /**< Counter mode */
        HCI_ENC_MODE_RTB   = 3  /**< Residual termination block mode */
    } hci_enc_mode_t;

    /**
     * HCI Encryption Padding types
     *
     * Type to specify the padding for an encryption operation
     **/
    typedef enum  {
        HCI_ENC_PAD_NONE        = 0, /**< No padding added*/
        HCI_ENC_PAD_PKCS1_V1_5  = 1, /**< PKCS1 v1.5 padding */
        HCI_ENC_PAD_OAEP        = 2, /**< OAEP padding */
        HCI_ENC_PAD_RFC2630     = 3, /**< Padding similar to pkcs5 */
        HCI_ENC_PAD_XMLENC      = 4  /**< Padding used for xmlenc also known as iso10126 padding */
    } hci_enc_pad_t;

    /**
     * HCI Digest Algorithm types
     *
     * Type to specify the algorithm for a digest operation
     **/
    typedef enum  {
        HCI_DIG_ALGO_SHA1    = 0, /**< Use SHA1 digest algorithm */
        HCI_DIG_ALGO_SHA256  = 1  /**< Use SHA256 digest algorithm */
    } hci_dig_algo_t;


    /**
     * HCI Personality types
     *
     * Type to specify the personality
     **/
    typedef enum  {
        HCI_PERSONALITY_TYPE_DEVICE_NPPR  = 0, /**< Nemo device private key */
        HCI_PERSONALITY_TYPE_DEVICE_OPPR  = 1, /**< Octopus device private key */
        HCI_PERSONALITY_TYPE_IMPORT_INPR  = 2, /**< Nemo import private key */
        HCI_PERSONALITY_TYPE_IMPORT_LOPR  = 3, /**< Lic sign import private key */
        HCI_PERSONALITY_TYPE_IMPORT_ROPR  = 4  /**< Reg import private key */
    } hci_pers_key_type_t;

    /**
     *
     * Type to specify the SAS keys for ms3
     **/
    typedef struct hci_sas_keys_s {
        uint32_t            key_count;  /**< number of keys */
        uint8_t           **contentids; /**< list of content ids of 20 bytes (no 0-termination, these originated from SHA1 hash values)*/
        hci_key_ref_t     **key_refs;   /**< list of key refs */
    } hci_sas_keys_t;

    /**
     * HCI Key load type
     *
     * Type of key to load into a reference
     **/
    typedef enum  {
        HCI_LOAD_KEY_TYPE_OCTPUB        = 0,  /**< Load Octopus public key */
        HCI_LOAD_KEY_TYPE_BROADCASTKEY  = 1   /**< Load Broadcast key */
    } hci_load_key_type_t;


    /**
     * Type to specify the key data to load into the hci
     **/
    typedef union hci_key_data_s {
        struct {
            uint32_t    len;
            uint8_t    *buf;
        } raw_key; /* bk */
        struct {
            uint32_t    pub_n_len;
            uint8_t    *pub_n;
            uint32_t    pub_e_len;
            uint8_t    *pub_e;
        } pub_key; /* public key */
    } hci_key_data_t;

    /**
     * Type to specify the format of ppproc
     **/
    typedef enum {
        HCI_PLAIN_PPPROC_FORMAT     = 1, /**< Plain PPProc format*/
        HCI_ENCRYPTED_PPPROC_FORMAT = 2  /**< Encrypted PPProc format*/
    } hci_ppproc_format_t;

    /*
     * Error base is based on that from Nautilus base to avoid collision of error codes.
     * See basic_types.h
     * (Layer=ERR_LYR_PLATFORM=4) | (Module=ERR_MOD_NONE=0) | (Component=0) | ERR_HCI_*
     * ERR_HCI_OK matches the existing ERR_OK/ERR_NP_OK
     */

    /** API call executed successfully */
#define ERR_HCI_OK                          0

#define ERR_HCI_BASE                        ((4 << 28) | (0 << 24) | (0 << 16))
#define ERR_EXTERN_BASIC_BASE               ((0 << 28)) /* must match ERR_BASIC_BASE (see basic_types.h)*/

    /** Internal hci memory allocation error (must equal ERR_NOMEM)*/
#define ERR_HCI_NOMEM                       (ERR_EXTERN_BASIC_BASE|2)
    /** Checking of supplied API args failed (must equal ERR_ILLEGALARGS) */
#define ERR_HCI_ILLEGALARGS                 (ERR_EXTERN_BASIC_BASE|3)
    /** The operation failed for an external cause (e.g. OS or programmatic) (must equal ERR_INTERNALERROR)*/
#define ERR_HCI_INTERNALERROR               (ERR_EXTERN_BASIC_BASE|5)

    /** An operation was attempted on a key whose keydata is not in the HCI key reference */
#define ERR_HCI_KEYNOTINREF                 (ERR_HCI_BASE|1)
    /** Illegal personality found */
#define ERR_HCI_ILLEGALPERSONALITY          (ERR_HCI_BASE|2)
    /** No personality was available to be used */
#define ERR_HCI_NOPERSONALITYREGISTERED     (ERR_HCI_BASE|3)
    /** An operation was attempted without having acquired the lock */
#define ERR_HCI_NOTLOCKED                   (ERR_HCI_BASE|4)
    /** Parsing the SAS failed */
#define ERR_HCI_ILLEGALSAS                  (ERR_HCI_BASE|5)
    /** Parsing the BKB failed */
#define ERR_HCI_ILLEGALBKB                  (ERR_HCI_BASE|6)
    /** Parsing the PPPROC failed */
#define ERR_HCI_ILLEGALPPPROC               (ERR_HCI_BASE|7)
    /** Lock is already taken */
#define ERR_HCI_ALREADYLOCKED               (ERR_HCI_BASE|8)
    /** Destination buffer for a cryptographic operation is not big enough */
#define ERR_HCI_TOOSMALLBUFFER              (ERR_HCI_BASE|9)
    /** The API is disable on purpose by hci_config.h */
#define ERR_HCI_NOTSUPPORTED                (ERR_HCI_BASE|10)
    /** A cryptographic error occurred in hci_crypto.c */
#define ERR_HCI_CRYPTOLIB                   (ERR_HCI_BASE|11)

    /**
     * @brief Initialise the hci interface.
     *
     * This will be called during np_Init().
     * It gives the platform a chance to setup the hci side before
     * any other hci calls are invoked.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR   An OS related error (not no-mem.) prevents initialization
     */
    extern uint32_t
    hci_Init(void);

    /**
     * @brief Finalise the hci interface.
     *
     * Called from np_Fin. It allows the platform an opportunity to cleanup
     * any resources held by the hci side before Nautilus is finalised.
     */
    extern void
    hci_Fin(void);

    /**
     * @brief Lock the hci interface.
     *
     * Generally, the hci interface needs to be locked before
     * operations using key references.
     * The hci interface can be unlocked again with @ref hci_Unlock.
     *
     * @param[out] out_lock   Lock handle
     *
     * @note If the lock was already acquired by another thread, the implementation
     *       must block until the lock is released again.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_ALREADYLOCKED
     * @return ::ERR_HCI_INTERNALERROR   An OS related error (not no-mem. or 'already locked') prevents locking
     */
    extern uint32_t
    hci_Lock(hci_lock_t *out_lock);

    /**
     * @brief Unlock the hci interface.
     *
     * @param[in,out] io_lock   Lock handle
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     */
    extern uint32_t
    hci_Unlock(hci_lock_t io_lock);

    /**
     * @brief Create a key reference.
     *
     * This is used any time an HCI operation is performed using the key.
     * A key reference must be destroyed with @ref hci_DestroyKeyRef.
     *
     * @param[in]  in_key_type  The requested key type
     * @param[out] out_ref      Reference to the key
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     * @return ::ERR_HCI_NOPERSONALITYREGISTERED
     * @return ::ERR_HCI_INTERNALERROR
     */
    extern uint32_t
    hci_CreateKeyRef(hci_key_type_t  in_key_type,
                     hci_key_ref_t  *out_ref);

    /**
      * @brief Create a SAC key reference.
      *
      * This is used any time an HCI operation is performed using a SAC key.
      * A key reference must be destroyed with @ref hci_DestroyKeyRef.
      *
      * @param[in]  in_sac_smid      The requested SAC key type.    The format of the
      *                                                             id is the standard 4CC boxtype. Note that the
      *                                                             the id is in big-endian format. Current supported
      *                                                             secure module IDs are "MRLN", "DTCP" and "ARIB"
      * @param[out] out_ref          Reference to the SAC key
      *
      * @return ::ERR_HCI_OK
      * @return ::ERR_HCI_ILLEGALARGS
      * @return ::ERR_HCI_NOMEM
      * @return ::ERR_HCI_NOTLOCKED
      * @return ::ERR_HCI_INTERNALERROR
      * @return ::ERR_HCI_NOTSUPPORTED
      */
    extern uint32_t
    hci_CreateSACKeyRef(uint32_t        in_sac_smid,
                        hci_key_ref_t  *out_ref);

    /**
    * @brief Check whether a key reference is still valid.
    *
    * Decrypted node keys can be cached for efficiency but
    * some platforms can only handle 1 decrypted key at the time in the cache.
    * A key reference is valid if its associated node key is decrypted & cached.
    *
    * @param[in]  in_ref      Key reference to check.
    *                         If NULL, ERR_HCI_OK is returned with out_result set to HCI_FALSE!
    * @param[in]  out_result  Boolean output of the check. HCI_TRUE if the key
    *                         reference is valid. HCI_FALSE otherwise
    *
    * @return ::ERR_HCI_OK
    * @return ::ERR_HCI_ILLEGALARGS
    * @return ::ERR_HCI_INTERNALERROR
    * @return ::ERR_HCI_NOTLOCKED
    */
    extern uint32_t
    hci_CheckKeyRef(hci_key_ref_t  in_ref,
                    hci_bool_t    *out_result);

    /**
     * @brief Destroy the key reference.
     *
     * @param[in,out] io_ref  Reference to the key to be destroyed.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     */
    extern uint32_t
    hci_DestroyKeyRef(hci_key_ref_t io_ref);

    /**
     * @brief Get the NULL terminated id from a personality key
     *
     * The personality keys are held by device which
     * are protected within the HCI component.
     * The id is used to detect if a octopus node or content key is encrypted
     * with the device personality private sharing or secret key.
     *
     * The returned id must be freed using ::hci_FreeMem() function.
     *
     * @param[in]  in_key_type  Type of key to get the id from. Must be either
     *                          HCI_KEY_TYPE_OPPR_PRV or HCI_KEY_TYPE_OPPR_SEC
     * @param[out] out_id       Null terminated string containing the id
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOPERSONALITYREGISTERED
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_GetDevID(hci_key_type_t   in_key_type,
                 char           **out_id);

    /**
     * @brief Import a key that is encrypted with the supplied key reference. This
     *        function will decrypt the supplied key and create a reference for it.
     *        The new key reference can therefore be used in turn to decrypt other
     *        (import) keys.
     *
     * @param[in]  in_ref             The reference to the key to decrypt the protected
     *                                key
     * @param[in]  in_imp_type        Type of key to import
     * @param[in]  in_enc_algo        Type of the decryption algorithm to use.
     * @param[in]  in_enc_mode        Type of encryption mode to use
     * @param[in]  in_enc_pad         The padding type
     * @param[in]  in_oaep_param_len  Length of the OAEP data
     * @param[in]  in_oaep_param      Optional oaep data to use during the decrypt with
     *                                @ref HCI_ENC_ALGO_RSA, HCI_ENC_PAD_OAEP
     * @param[in]  in_iv_len          Length of IV
     * @param[in]  in_iv              IV used to the decryption of the content key
     * @param[in]  in_key_data_len    Length of supplied key data
     * @param[in]  in_key_data        Encrypted key data
     * @param[out] out_ref            Output key reference to hold the decrypted key
     *
     * @note @ref HCI_ENC_ALGO_AES_CTR is not supported
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_ImportKey(hci_key_ref_t      in_ref,
                  hci_imp_key_type_t in_imp_type,
                  hci_enc_algo_t     in_enc_algo,
                  hci_enc_mode_t     in_enc_mode,
                  hci_enc_pad_t      in_enc_pad,
                  uint32_t           in_oaep_param_len,
                  uint8_t           *in_oaep_param,
                  uint32_t           in_iv_len,
                  uint8_t           *in_iv,
                  uint32_t           in_key_data_len,
                  uint8_t           *in_key_data,
                  hci_key_ref_t     *out_ref);

    /**
     * @brief Get the torpedo id for the device protected with the HCI interface.
     *        This is used when removing starfish protection of a content key.
     *
     * @param[out] out_id  NULL terminated id
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOPERSONALITYREGISTERED
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_GetStarfishId(char **out_id);

    /**
     * @brief Return the key data from a key that was previously exported to the
     *        key reference if the key is protected with starfish. Otherwise NULL.
     *
     * @param[in]  in_ref            Reference to the key
     * @param[out] out_key_data_len  Length of the key data
     * @param[out] out_key_data      Key data (starfish encrypted)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOPERSONALITYREGISTERED
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_GetKeyDataIfStarfish(hci_key_ref_t  in_ref,
                             uint32_t      *out_key_data_len,
                             uint8_t      **out_key_data);

    /**
     * @brief Decrypt the input encrypted key data using starfish and the device
     *        key set.
     *
     * @param[in]  in_num_iterations  Number of hash iterations
     * @param[in]  in_dks_key_index   Index of the key in the dks
     * @param[in]  in_enc_bk_len      Length of encrypted broadcast key
     * @param[in]  in_enc_bk          Encrypted broadcast key
     * @param[in]  in_bk_hash_len     Length of hash of the plain broadcast key
     * @param[in]  in_bk_hash         Hash of the plain broadcast key. Checked
     *                                after decrypting the broadcast key to ensure
     *                                it is correct
     * @param[in]  in_iv_len          Length of IV
     * @param[in]  in_iv              IV used to the decryption of the content key
     * @param[in]  in_enc_ck_len      Length of encrypted CK to extract
     * @param[in]  in_enc_ck          Encrypted content key to extract
     * @param[out] out_ref            Output key reference to hold the content key
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_ImportStarfishKey(uint32_t        in_num_iterations,
                          uint32_t        in_dks_key_index,
                          uint32_t        in_enc_bk_len,
                          uint8_t        *in_enc_bk,
                          uint32_t        in_bk_hash_len,
                          uint8_t        *in_bk_hash,
                          uint32_t        in_iv_len,
                          uint8_t        *in_iv,
                          uint32_t        in_enc_ck_len,
                          uint8_t        *in_enc_ck,
                          hci_key_ref_t  *out_ref);

    /**
     * @brief Sign the supplied data using the key in the supplied key reference.
     *
     * @param[in]  in_ref            Reference to the key to sign the data with
     * @param[in]  in_sig_algo       Type of the signature to generate
     * @param[in]  in_data_len       Length of data to generate the signature over
     * @param[in]  in_data           Data to generate the signature over
     * @param[out] out_sig_data_len  Length of the output signature
     * @param[out] out_sig_data      Signature data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_SignData(hci_key_ref_t       in_ref,
                 hci_sig_algo_t      in_sig_algo,
                 uint32_t            in_data_len,
                 uint8_t            *in_data,
                 uint32_t           *out_sig_data_len,
                 uint8_t           **out_sig_data);

    /**
     * @brief Decrypt the supplied data using the key in the supplied key reference.
     *        The hci lock is checked and will block until available.
     *        It will only be possible to use this decrypt call with a NEMO
     *        message key or a content key.
     *
     * @param[in]      in_ref             Reference to the key to decrypt the data with
     * @param[in]      in_enc_algo        Type of the decryption algorithm to use
     * @param[in]      in_enc_mode        Type of encryption mode to use
     * @param[in]      in_enc_pad         Type of padding used
     * @param[in]      in_oaep_param_len  OAEP data len
     * @param[in]      in_oaep_param      Optional oaep data to use during the decrypt
     * @param[in]      in_iv_len          Length of the IV
     * @param[in]      in_iv              IV for decrypting
     * @param[in]      in_data_len        Length of data to decrypt
     * @param[in]      in_data            Data to decrypt
     * @param[out]     out_data_len       Length of the plaintext output data
     * @param[out]     out_data           Decrypted data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_DecryptData(hci_key_ref_t     in_ref,
                    hci_enc_algo_t    in_enc_algo,
                    hci_enc_mode_t    in_enc_mode,
                    hci_enc_pad_t     in_enc_pad,
                    uint32_t          in_oaep_param_len,
                    uint8_t          *in_oaep_param,
                    uint32_t          in_iv_len,
                    uint8_t          *in_iv,
                    uint32_t          in_data_len,
                    uint8_t          *in_data,
                    uint32_t         *out_data_len,
                    uint8_t         **out_data);

    /**
     * @brief Decrypt the supplied data using the key in the supplied key reference.
     *        This API can be called outside of a hci lock in order to prevent
     *        content decryption calls being blocked by other crypto operations.
     *        This function expects a user supplied buffer of adequate size. A
     *        destination buffer with the same size as the incoming ciphertext
     *        is sufficient.
     *
     *        It will only be possible to use this decrypt call with a content key.
     *
     * @param[in]     in_ref       Reference to the key to decrypt the data with
     * @param[in]     in_enc_algo  Type of the decryption algorithm to use
     * @param[in]     in_enc_mode  Type of encryption mode to use
     * @param[in]     in_enc_pad   Type of padding used
     * @param[in]     in_is_final  Flag to notify if the Decrypt is the last one of
     *                             a multipart decrypt and padding is to be applied
     * @param[in]     in_iv_len    Length of iv
     * @param[in,out] io_iv        IV for decrypting, this is overwritten with the IV for the next round
     * @param[in]     in_data_len  Length of data to decrypt
     * @param[in]     in_data      Data to decrypt
     * @param[in,out] io_data_len  Length of plaintext output, the initial value is the
     *                             size of the provided destination buffer
     * @param[out]    out_data     Decrypted data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_DecryptStreamWithIV(hci_key_ref_t     in_ref,
                            hci_enc_algo_t    in_enc_algo,
                            hci_enc_mode_t    in_enc_mode,
                            hci_enc_pad_t     in_enc_pad,
                            hci_bool_t        in_is_final,
                            uint32_t          in_iv_len,
                            uint8_t          *io_iv,
                            uint32_t          in_data_len,
                            uint8_t          *in_data,
                            uint32_t         *io_data_len,
                            uint8_t          *out_data);

    /**
     * @brief Decryption for streams
     *
     * @param[in]     in_ref       Reference to the key to decrypt the data with
     * @param[in]     in_enc_algo  Type of the decryption algorithm to use
     * @param[in]     in_enc_mode  Type of encryption mode to use
     * @param[in]     in_enc_pad   Type of padding used
     * @param[in]     in_offset    Offset from the first byte in the cipher text
     * @param[in]     in_ctrsize   Counter size
     * @param[in]     in_ctr       Counter
     * @param[in]     in_data_len  Input ciphertext length
     * @param[in]     in_data      Input ciphertext
     * @param[in,out] io_data_len  Decrypted data length
     * @param[out]    out_data     Decrypted data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_DecryptStreamWithOffset(hci_key_ref_t     in_ref,
                                hci_enc_algo_t    in_enc_algo,
                                hci_enc_mode_t    in_enc_mode,
                                hci_enc_pad_t     in_enc_pad,
                                uint32_t          in_offset,
                                uint32_t          in_ctrsize,
                                uint8_t          *in_ctr,
                                uint32_t          in_data_len,
                                uint8_t          *in_data,
                                uint32_t         *io_data_len,
                                uint8_t          *out_data);

    /**
     * @brief Encrypt the key data in the supplied reference with the key in
     *        another supplied reference
     *
     * @param[in]  in_ref             Reference to the key to use to encrypt
     * @param[in]  in_enc_algo        Type of the encryption algorithm to use
     * @param[in]  in_enc_mode        Type of encryption mode to use
     * @param[in]  in_enc_pad         Type of padding used
     * @param[in]  in_oaep_param_len  OAEP data len
     * @param[in]  in_oaep_param      Optional oaep data to use during the encrypt
     * @param[in]  in_key             Key whose data in the reference is to be encrypted
     * @param[out] out_iv_len         Length of returned IV
     * @param[out] out_iv             IV used for the encryption of key data
     * @param[out] out_key_data_len   Length of encrypted key data
     * @param[out] out_key_data       Encrypted key data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_EncryptKeyData(hci_key_ref_t   in_ref,
                       hci_enc_algo_t  in_enc_algo,
                       hci_enc_mode_t  in_enc_mode,
                       hci_enc_pad_t   in_enc_pad,
                       uint32_t        in_oaep_param_len,
                       uint8_t        *in_oaep_param,
                       hci_key_ref_t   in_key,
                       uint32_t       *out_iv_len,
                       uint8_t       **out_iv,
                       uint32_t       *out_key_data_len,
                       uint8_t       **out_key_data);

    /**
     * @brief Calculate the HMAC on the supplied data using the key in keyref.
     *
     * @param[in]  in_ref                Reference to the key to calculate the HMAC with
     * @param[in]  in_dig_algo           Digest algorithm to be used
     * @param[in]  in_data_len           Length of data to calculate the HMAC over
     * @param[in]  in_data               Data to calculate the HMAC over
     * @param[in]  in_expected_hmac_len  Length of expected HMAC value
     * @param[in]  in_expected_hmac      Expected HMAC value
     * @param[out] out_result            Flag holding the check result. HCI_TRUE for a match
     *                                   HCI_FALSE for a difference
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_CheckHMAC(hci_key_ref_t   in_ref,
                  hci_dig_algo_t  in_dig_algo,
                  uint32_t        in_data_len,
                  uint8_t        *in_data,
                  uint32_t        in_expected_hmac_len,
                  uint8_t        *in_expected_hmac,
                  hci_bool_t     *out_result);

    /**
     * @brief Calculate the hash on the supplied data concatenated with the key in
     *        reference, i.e. in_data || in_ref.keydata. If the in_data is NULL then
     *        the hash is just calculated on in_ref.keydata.
     *
     * @param[in]  in_ref                Reference to the key to calculate the hash with
     * @param[in]  in_dig_algo           Digest algorithm to be used
     * @param[in]  in_data_len           Length of data to prepend to the key before hashing
     * @param[in]  in_data               Data to prepend to the key before hashing
     * @param[in]  in_expected_hash_len  Expected hash value length
     * @param[in]  in_expected_hash      Expected hash value
     * @param[out] out_result            Flag holding the check result. TRUE for a match,
     *                                   FALSE otherwise.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_CheckHash(hci_key_ref_t   in_ref,
                  hci_dig_algo_t  in_dig_algo,
                  uint32_t        in_data_len,
                  uint8_t        *in_data,
                  uint32_t        in_expected_hash_len,
                  uint8_t        *in_expected_hash,
                  hci_bool_t     *out_result);

    /**
     * @brief Generate the hash on the supplied data concatenated with the key in
     *        reference, i.e. in_data || in_ref.keydata. If the in_data is NULL then
     *        the hash is just generated on in_ref.keydata .
     *
     * @param[in]  in_ref        Reference to the key to generate the hash over
     * @param[in]  in_dig_algo   Digest algorithm to be used
     * @param[in]  in_data_len   Length of data to prepend to the key before hashing
     * @param[in]  in_data       Data to prepend to the key before hashing
     * @param[out] out_hash_len  Length of the generated hash
     * @param[out] out_hash      Generated hash value
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_GenerateHash(hci_key_ref_t   in_ref,
                     hci_dig_algo_t  in_dig_algo,
                     uint32_t        in_data_len,
                     uint8_t        *in_data,
                     uint32_t       *out_hash_len,
                     uint8_t       **out_hash);

    /**
     * @brief Move the hci key data in a platform specific way to another location.
     *
     * It is assumed that the key will be protected by the ported HCI export-implementation.
     * Nautilus has no protection added for export by default.
     * Use the in_private_data & out_private_data to support the key protection.
     *
     * The key reference becomes unavailable after the move operation
     *
     * @param[in,out] io_ref                Reference to the key to move
     * @param[in]     in_private_data_len   Length of platform data
     * @param[in]     in_private_data       Platform data from the Application used
     *                                      when moving the key
     * @param[out]    out_private_data_len  Length of returned platform data
     * @param[out]    out_private_data      Platform data returned after the key is
     *                                      moved
     *
     * @note This functionality is only supported for content keys.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_MoveKey(hci_key_ref_t   io_ref,
                uint32_t        in_private_data_len,
                uint8_t        *in_private_data,
                uint32_t       *out_private_data_len,
                uint8_t       **out_private_data);

    /** @brief Export the key in a reference to an external caller.
     *
     * It is assumed that the key will be protected by the ported HCI export-implementation.
     * Nautilus has no protection added for export by default.
     * Use the in_private_data & out_private_data to support the key protection.
     *
     * The HCI layer is also responsible to only allow the export of content keys.
     *
     * @param[in]  in_ref                Reference to the key to export
     * @param[in]  in_private_data_len   Length of platform data
     * @param[in]  in_private_data       Platform data from the Application used
     *                                   when exporting the key
     * @param[out] out_private_data_len  Length of returned platform data
     * @param[out] out_private_data      Platform data returned after the key is
     *                                   exported
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_ExportKey(hci_key_ref_t    in_ref,
                  uint32_t         in_private_data_len,
                  uint8_t         *in_private_data,
                  uint32_t        *out_private_data_len,
                  uint8_t        **out_private_data);

    /**
     * @brief This API will be used to load device personality private keys into
     *        the HCI component. If the device keys are loaded at factory time then
     *        they will be accessed directly from the hci component.
     *        It is called when private keys are to be set into the device.
     *        It is the platform owners responsibility to protect the keys
     *        loaded into the hci component. If in_buf = NULL then the function should
     *        delete the Personality key.
     *
     * @param[in]  in_type  Personality key type (nemopriv or octpriv)
     * @param[in]  in_len   Size of the personality private key
     * @param[in]  in_buf   Personality key data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_LoadPersonalityKey(hci_pers_key_type_t  in_type,
                           uint32_t             in_len,
                           uint8_t             *in_buf);

    /**
     * @brief Check whether the personality private keys are present.
     *
     * @param[in] in_type  Personality type
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOPERSONALITYREGISTERED
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     * @return ::ERR_HCI_NOTLOCKED
     */
    extern uint32_t
    hci_CheckPersonalityKey(hci_pers_key_type_t in_type);

    /**
     * @brief Parse SAS to extract the content keys and content ids from it.
     *
     * Note: platform has to add the implementation to decrypt the SAS, if it is
     * in encrypted form.
     *
     * @param[in]  in_sas_len           Length of the supplied SAS
     * @param[in]  in_sas               SAS info (encrypted/unencrypted)
     * @param[in]  in_private_data_len  Length of the opaque application data
     * @param[in]  in_private_data      Opaque application data, maybe
     *                                  used to protect SAS
     * @param[out] out_public_sas_len   Length of public sas info
     * @param[out] out_public_sas       Public sas info (excluding keys and content ids)
     * @param[out] out_sas_keys         Structure holds keycount,contentids and keyrefs
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_ILLEGALSAS
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_ImportSAS(uint32_t          in_sas_len,
                  uint8_t          *in_sas,
                  uint32_t          in_private_data_len,
                  uint8_t          *in_private_data,
                  uint32_t         *out_public_sas_len,
                  uint8_t         **out_public_sas,
                  hci_sas_keys_t  **out_sas_keys);

    /**
     * @brief Free SAS keys structure.
     *
     * @param[in,out]  io_sas_keys  SAS keys structure to be freed
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_FreeSASKeys(hci_sas_keys_t  *io_sas_keys);

    /**
     * @brief Generate a random AES content key value or use provided
     *        in_private_data as key value and hold it in a reference
     *
     * @param[in]  in_key_sz            Size of the key to generate
     * @param[in]  in_private_data_len  Length of protected data
     * @param[in]  in_private_data      Platform data when importing a key
     * @param[out] out_ref              Output key reference holding the key
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_GenerateContentKey(uint32_t        in_key_sz,
                           uint32_t        in_private_data_len,
                           uint8_t        *in_private_data,
                           hci_key_ref_t  *out_ref);

    /**
     * @brief This API loads keys into the HCI component.
     *
     * For personality keys use hci_LoadPersonalityKey
     *
     * @param[in]  in_type      Type of key to load
     * @param[in]  in_key_data  Key data to load into a reference.
     * @param[out] out_ref      Reference holding the key
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_LoadKey(hci_load_key_type_t  in_type,
                hci_key_data_t      *in_key_data,
                hci_key_ref_t       *out_ref);

    /**
     * @brief Generates HMAC over the supplied data
     *
     * @param[in]  in_ref        Key ref of the key to be used
     * @param[in]  in_dig_algo   Digest algorithm
     * @param[in]  in_data_len   Length of data to generate the HMAC over
     * @param[in]  in_data       Data, to calculate the HMAC over
     * @param[out] out_hmac_len  Length of the Generated HMAC
     * @param[out] out_hmac      Generated HMAC
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_GenerateHMAC(hci_key_ref_t   in_ref,
                     hci_dig_algo_t  in_dig_algo,
                     uint32_t        in_data_len,
                     uint8_t        *in_data,
                     uint32_t       *out_hmac_len,
                     uint8_t       **out_hmac);

    /**
     * @brief Import a BKB container and extract the broadcast key
     *
     * @param[in]  in_container_len     Protected BKB container length
     * @param[in]  in_container         Protected BKB container
     * @param[in]  in_from_ppproc       Flag indicates that bkb container is from ppproc bundle
     *                                  and it has been protected/scrambled in platform specific way.
     *                                  (Also, if in_from_ppproc == HCI_TRUE, ignore in_private_data).
     * @param[in]  in_private_data_len  Length of protected data
     * @param[in]  in_private_data      Platform data used for removing the BKB container
     *                                  protection
     * @param[out] out_container_len    0, or length of the returned container
     * @param[out] out_container        NULL, or plaintext version of the BKB container with the
     *                                  Broadcast key scrubbed out
     * @param[out] out_bk_len           0, or length of the returned protected BK.
     * @param[out] out_bk               NULL, or protected BK. This can be written to the
     *                                  Nautilus registry
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_ILLEGALBKB
     * @return ::ERR_HCI_NOTSUPPORTED
     */
    extern uint32_t
    hci_ImportBKBContainer(uint32_t    in_container_len,
                           uint8_t    *in_container,
                           hci_bool_t  in_from_ppproc,
                           uint32_t    in_private_data_len,
                           uint8_t    *in_private_data,
                           uint32_t   *out_container_len,
                           uint8_t   **out_container,
                           uint32_t   *out_bk_len,
                           uint8_t   **out_bk);

    /**
     * @brief Encrypt the supplied data using the key in the supplied key reference.
     *        The hci lock is checked and will block until available.
     *
     * @param[in]  in_ref             Reference to the key to encrypt the data with
     * @param[in]  in_enc_algo        Type of the encryption algorithm to use
     * @param[in]  in_enc_mode        Type of encryption mode to use
     * @param[in]  in_enc_pad         Type of padding used
     * @param[in]  in_oaep_param_len  OAEP data len
     * @param[in]  in_oaep_param      Optional oaep data to use during the encrypt
     * @param[in]  in_data_len        Length of data to encrypt
     * @param[in]  in_data            Data to encrypt
     * @param[out] out_iv_len         Length of the generated IV
     * @param[out] out_iv             IV used for the encryption of data
     * @param[out] out_data_len       Length of the encrypted output data
     * @param[out] out_data           Encrypted data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTLOCKED
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_EncryptData(hci_key_ref_t     in_ref,
                    hci_enc_algo_t    in_enc_algo,
                    hci_enc_mode_t    in_enc_mode,
                    hci_enc_pad_t     in_enc_pad,
                    uint32_t          in_oaep_param_len,
                    uint8_t          *in_oaep_param,
                    uint32_t          in_data_len,
                    uint8_t          *in_data,
                    uint32_t         *out_iv_len,
                    uint8_t         **out_iv,
                    uint32_t         *out_data_len,
                    uint8_t         **out_data);

    /**
     * @brief Encrypt the supplied data with the key in the supplied key reference
     *
     * @note HCI Lock is not required for this API
     *
     * @param[in]     in_ref       Key reference to use to encrypt the supplied data
     * @param[in]     in_enc_algo  Type of the encryption algorithm to use
     * @param[in]     in_enc_mode  Type of encryption mode to use
     * @param[in]     in_enc_pad   Type of padding used
     * @param[in]     in_is_final  The Encrypt is the last one of a multipart encrypt
     *                             and padding is to be applied
     * @param[in]     in_iv_len    Length of IV
     * @param[in,out] io_iv        IV to use for the encryption of data
     * @param[in]     in_data_len  Length of data to encrypt
     * @param[in]     in_data      Data to encrypt
     * @param[in,out] io_data_len  Length of the ciphertext output
     * @param[out]    out_data     Binary encrypted data
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_KEYNOTINREF
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     * @return ::ERR_HCI_TOOSMALLBUFFER
     */
    extern uint32_t
    hci_EncryptStreamWithIV(hci_key_ref_t     in_ref,
                            hci_enc_algo_t    in_enc_algo,
                            hci_enc_mode_t    in_enc_mode,
                            hci_enc_pad_t     in_enc_pad,
                            hci_bool_t        in_is_final,
                            uint32_t          in_iv_len,
                            uint8_t          *io_iv,
                            uint32_t          in_data_len,
                            uint8_t          *in_data,
                            uint32_t         *io_data_len,
                            uint8_t          *out_data);


    /**
     * @brief Parse the ppproc data bundle to securely extract the hci keys
     *
     * This call will return the public ppproc data for parsing by the non
     * hci component. The parsing of the hci keys will be done inside the hci
     * component. The hci keys will be removed from the input before returning the
     * public elements.
     *
     * @param[in]  in_ppproc_data_len     Length of the supplied ppproc bundle
     * @param[in]  in_ppproc_data         ppproc bundle data
     * @param[in]  in_format              Format of in_ppproc_data
     * @param[out] out_public_ppproc_len  Length of public ppproc data
     * @param[out] out_public_ppproc      Public ppproc info (excluding hci keys)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_ILLEGALPPPROC
     * @return ::ERR_HCI_NOTSUPPORTED
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_ImportPpprocData(uint32_t             in_ppproc_data_len,
                         uint8_t             *in_ppproc_data,
                         hci_ppproc_format_t  in_format,
                         uint32_t            *out_public_ppproc_len,
                         uint8_t            **out_public_ppproc);

    /**
     * @brief Free the memory space previously returned by an HCI function
     *
     * Many HCI functions return a result in dynamically allocated HCI memory.
     * That memory, needs to be released back to HCI. There is 1 exception:
     * - For content data stream de/en-cryption the buffer is provided by the caller
     *
     * @note The memory can be freed without the HCI lock!
     * On sample implementation for PC, however, the lock is required for freeing memory
     *
     * @param[in]  in_ptr  The buffer to be returned to HCI
     */
    extern void
    hci_FreeMem(void *in_ptr);

#ifdef __cplusplus
}
#endif

#endif                          /* HCI_API_H */
/** @} */
