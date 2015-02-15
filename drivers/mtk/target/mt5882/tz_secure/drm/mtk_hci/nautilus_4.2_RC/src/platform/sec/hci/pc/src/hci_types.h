/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
 */

#ifndef HCI_TYPES_H
#define HCI_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

    /** @addtogroup hci hci
     * @ingroup marlin
     * @{ */

#define HCI_L_SHA1_DIGESTSIZE             20       /**< Size of SHA1 digest */
#define HCI_L_SHA256_DIGESTSIZE           32       /**< Size of SHA256 digest */

#define HCI_L_AES_KEYSIZE                 16       /**< Size of a AES key*/
#define HCI_L_AES_BLOCKSIZE               16       /**< granulity for AES en/decrypting*/

#define HCI_L_RSA_SIGSIZEMAX              256      /**< RSA signature is size of the modules*/

#define HCI_L_PPPROC_FORMAT_DEFAULT       1        /* Default */
#define HCI_L_PPPROC_FORMAT_MOA_DRBD      2        /* MoA (DRBD) */
#define HCI_L_PPPROC_FORMAT_MOA_QMU       3        /* MoA (QMU) */

    /** Key types */
    typedef enum  {
        HCI_L_KEY_ALGO_TYPE_AES             = 0,     /**< AES CBC */
        HCI_L_KEY_ALGO_TYPE_RSA             = 1      /**< RSA PKCS1 v1.5 */
    } hci_l_key_algo_type_t;

    /** Key format*/
    typedef enum {
        HCI_L_KEY_FORMAT_AES,
        HCI_L_KEY_FORMAT_RSA,
        HCI_L_KEY_FORMAT_STARFISH,
        HCI_L_KEY_FORMAT_UNSPECIFIED
    } hci_l_key_format_t;

    /** Key type - DO NOT CHANGE THE ORDER
     *  The order is used in looking up the attributes of the different
     *  types of keys in a lookup table - see hci.c */
    typedef enum {
        HCI_L_KEY_TYPE_BOOT              = 0,
        HCI_L_KEY_TYPE_OPPR_PRV          = 1,
        HCI_L_KEY_TYPE_OPPR_SEC          = 2,
        HCI_L_KEY_TYPE_NPPR_SIG          = 3,
        HCI_L_KEY_TYPE_NPPR_ENC          = 4,
        HCI_L_KEY_TYPE_LOPR_SIG          = 5,
        HCI_L_KEY_TYPE_CONTENT           = 6,
        HCI_L_KEY_TYPE_DOMAIN            = 7,
        HCI_L_KEY_TYPE_BROADCAST         = 8,
        HCI_L_KEY_TYPE_MESSAGE           = 9,
        HCI_L_KEY_TYPE_OPPU              = 10,
        HCI_L_KEY_TYPE_GENERATED         = 11,
        HCI_L_KEY_TYPE_SAC               = 12,
        HCI_L_KEY_MAX                    = 13
    } hci_l_key_type_t;

    /** Attributes of a key reference */
    typedef enum {
        HCI_L_ATTR_MOVE                  = 0, /** Movable */
        HCI_L_ATTR_EXPORT                = 1, /** Exportable */
        HCI_L_ATTR_SIGN                  = 2, /** Use for signing data */
        HCI_L_ATTR_ENC_KEY               = 3, /** Use as an encryption key */
        HCI_L_ATTR_MAX                   = 4
    } hci_l_attr_t;

    typedef enum {
        L_PERS_TYPE_OPPR_SEC,
        L_PERS_TYPE_OPPR_PRV,
        L_PERS_TYPE_STARFISH
    } hci_l_pers_type_t;


    typedef struct hci_l_key_ref_s {
        binstr_t           *key_data;            /* holds the raw key data if needed by the ref */
        hci_l_key_type_t    key_type;            /* type of key held by the reference */
        hci_l_key_format_t  format;              /* format of key held by the reference */
    } hci_l_key_ref_t;


#ifdef __cplusplus
}
#endif

#endif
