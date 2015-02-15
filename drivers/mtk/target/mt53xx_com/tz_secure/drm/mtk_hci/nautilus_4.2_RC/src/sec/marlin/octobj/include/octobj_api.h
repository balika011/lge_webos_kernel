/** @addtogroup octobj octobj
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

#ifndef OCTOBJ_API_H
#define OCTOBJ_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * Octopus Objects Internal Structure Definition and Library.
     *
     * These objects represent the generic Octopus types, and are agnostic in
     * relation to the actual implementation of the DRM architecture. DRM
     * functionality is implemented using Octopus Contexts.
     *
     * The hierarchy is as followd Marlin DRM --> Octopus Contexts --> Octopus Objects
     *
     * dependency base
     * @author Norifumi Goto <n-goto@sm.sony.co.jp>
     * @date   2 Dec, 2004 last modified by n-goto
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * error code definitions
     *---------------------------------------------------------------------------*/

#define ERR_OCTOBJ_UNSUPPORTED       (ERR_OCTOBJ_BASE|1)
#define ERR_OCTOBJ_NOTRAWKEY         (ERR_OCTOBJ_BASE|4)
#define ERR_OCTOBJ_INVALIDOBJECT     (ERR_OCTOBJ_BASE|5)
#define ERR_OCTOBJ_NOEXTSCUBAKEYS    (ERR_OCTOBJ_BASE|6)
#define ERR_OCTOBJ_NOEXTTORPEDOKEY   (ERR_OCTOBJ_BASE|7)

    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

#define OCTOBJ_TYPE_NULL              0
#define OCTOBJ_TYPE_PROTECTOR         1
#define OCTOBJ_TYPE_CONTENTKEY        2
#define OCTOBJ_TYPE_CONTROLLER        3
#define OCTOBJ_TYPE_CONTROL           4
#define OCTOBJ_TYPE_LINK              5
#define OCTOBJ_TYPE_NODE              6
#define OCTOBJ_TYPE_REFERENCE         7
#define OCTOBJ_TYPE_ATTRIBUTE         8
#define OCTOBJ_TYPE_INTEXTENSION      9
#define OCTOBJ_TYPE_EXTEXTENSION     10
#define OCTOBJ_TYPE_KEY              11
#define OCTOBJ_TYPE_PAIREDKEY        12
#define OCTOBJ_TYPE_STRING           13
#define OCTOBJ_TYPE_SAMLATTRIBUTE    14
#define OCTOBJ_TYPE_MEMBUFFER        15

#define OCTOBJ_KEY_USAGE_SHARE        0
#define OCTOBJ_KEY_USAGE_CONF         1
#define OCTOBJ_KEY_USAGE_NONE         2
#define OCTOBJ_KEY_USAGE_MAX          3

#define OCTOBJ_KEY_FORMAT_PKCS8       0
#define OCTOBJ_KEY_FORMAT_X509SPKI    1
#define OCTOBJ_KEY_FORMAT_RAW         2
#define OCTOBJ_KEY_FORMAT_MAX         3

#define OCTOBJ_KEY_ALGORITHM_NONE      0
#define OCTOBJ_KEY_ALGORITHM_AES128CBC 1
#define OCTOBJ_KEY_ALGORITHM_RSA1_5    2
#define OCTOBJ_KEY_ALGORITHM_STARFISH  3
#define OCTOBJ_KEY_ALGORITHM_RSA_OAEP  4
#define OCTOBJ_KEY_ALGORITHM_MAX       5

#define OCTOBJ_ATTRIBUTE_TYPE_INT       0
#define OCTOBJ_ATTRIBUTE_TYPE_STR       1
#define OCTOBJ_ATTRIBUTE_TYPE_BYTE      2
#define OCTOBJ_ATTRIBUTE_TYPE_LIST      3
#define OCTOBJ_ATTRIBUTE_TYPE_ARRAY     4

#define OCTOBJ_ATTRIBUTE_TYPE_ID_INT    "int"
#define OCTOBJ_ATTRIBUTE_TYPE_ID_STR    "string"
#define OCTOBJ_ATTRIBUTE_TYPE_ID_BYTE   "bytes"
#define OCTOBJ_ATTRIBUTE_TYPE_ID_LIST   "list"
#define OCTOBJ_ATTRIBUTE_TYPE_ID_ARRAY  "array"

#define OCTOBJ_SAMLATTRIBUTE_TYPE_INT   0
#define OCTOBJ_SAMLATTRIBUTE_TYPE_STR   1
#define OCTOBJ_SAMLATTRIBUTE_TYPE_NONE  2

#define OCTOBJ_ASSERTION_TYPE_ROLE          0
#define OCTOBJ_ASSERTION_TYPE_DATACERT      1
#define OCTOBJ_ASSERTION_TYPE_CAPABILITIES  2

#define OCTOBJ_EXTENSION_TYPE_UNKNOWN 0
#define OCTOBJ_EXTENSION_TYPE_SCUBA   1
#define OCTOBJ_EXTENSION_TYPE_TORPEDO 2


    /** Generic object container object */
    typedef struct octobj_vec_tag {
        int32_t size;       /**< Number of objects contained */
        void *item[1];      /**< Array of objects */
    } octobj_vec_t;

    /**
     * Reference ID representation.
     *
     * Protector and Controller objects maintain references to
     * other objects (Content, ContentKey, Control etc) by
     * recording the objects IDs.
     *
     * A reference is an ID of another object. If an object has
     * a reference to another, it keeps a reference of the objects
     * ID.
     *
     * \specref See "Octopus Objects 1.0.1, section 2.0"
     */
    typedef struct {
        binstr_t     *id;         /**< The ID of the object being referenced */
        octobj_vec_t *transforms; /**< optional - digest format */
        binstr_t     *algorithm;  /**< optional - Hash used for digest */
        binstr_t     *value;      /**< optional - Digest value */
    } octobj_reference_t;

    /**
     * Encrypted key
     *
     * \specref See "Octopus Objects 1.0.1, section 4.5"
     */

    typedef struct {
        binstr_t      *id;             /**< Unique id for this key */
        unsigned char  usage;          /**< Sharing key or Confidentiality key */
        unsigned char  format;         /**< PKCS #8 / X509SPKI / Raw */
        unsigned char  encalgorithm;   /**< Payload encryption algorithm */
        binstr_t      *oaepparam ;     /**< oaep parameters for RSA-OAEP encryption algorithm>*/
        unsigned char  midalgorithm;   /**< AES-128-CBC or Null. @specref Marlin Core Specification section 3.3.3.2.2 */
        binstr_t      *enckeyid;       /**< key id */
        binstr_t      *midkeydata;     /**< NULL if no middle-key is used */
        binstr_t      *keydata;        /**< Encrypted payload */
        void          *key_cache;      /**< hci_key_ref_t from keymng */
        int            key_type;       /**<  for domain and link keys and    */
        /**  HCI_IMP_KEY_TYPE_CONTENT{BROADCAST} for content key w/o or w/ Starfish */
    } octobj_key_t;

    /** Asymmetric Key */
    typedef struct {
        octobj_key_t key;              /**< Encrypted key */
        binstr_t *pairid;              /**< ID for the key pair */
    } octobj_pairedkey_t;

    /**
     * Attribute object
     *
     * There are five types of attribute. Integer, String, ByteArray,
     * List and Array.
     **/
    typedef struct {
        /**
         * Must be specified for all attribute types except ArrayAttributes
         */
        binstr_t *name;

        /**
         * Attribute type. One of
         * <table>
         *   <tr> <th> Type <th> Description
         *   <tr> <td> ::OCTOBJ_ATTRIBUTE_TYPE_INT   <td> Integer value type
         *   <tr> <td> ::OCTOBJ_ATTRIBUTE_TYPE_STR   <td> String value type
         *   <tr> <td> ::OCTOBJ_ATTRIBUTE_TYPE_BYTE  <td> Byte sequence value type
         *   <tr> <td> ::OCTOBJ_ATTRIBUTE_TYPE_LIST  <td> Named attribute container type
         *   <tr> <td> ::OCTOBJ_ATTRIBUTE_TYPE_ARRAY <td> Unnamed attribute container type
         * </table>
         */
        int32_t type;

        /** Attribute contents */
        union {
            int32_t i;              /**< Value for integer type */
            binstr_t *s;            /**< Either a string or byte sequence */
            octobj_vec_t *attrs;    /**< Either a list or array */
        } value;
    } octobj_attribute_t;

    /**
     * For SAML Attribute
     *
     * Structure that maps to the saml:AttributeStatement/saml:Attribute element
     * of a SAML file
     */
    typedef struct {
        binstr_t *name;      /**< "namespace:name" string */
        int32_t type;        /**< One of ::OCTOBJ_SAMLATTRIBUTE_TYPE_NONE, ::OCTOBJ_SAMLATTRIBUTE_TYPE_INT,
                              ::OCTOBJ_SAMLATTRIBUTE_TYPE_STR */
        union {
            int32_t i;       /**< Attribute value if type is ::OCTOBJ_SAMLATTRIBUTE_TYPE_INT */
            binstr_t *s;     /**< Attribute value if type is ::OCTOBJ_SAMLATTRIBUTE_TYPE_STR */
        } value;
    } octobj_samlattribute_t;

    /** \nd */
    typedef struct {
        octobj_vec_t  *secretkeys;
        octobj_vec_t  *privatekeys;
        octobj_vec_t  *publickeys;
    } octobj_scubakeys_t;

    /**
     * Internal or External extension
     *
     * Common elements of an Octopus object. Extensions are elements that
     * can be added to objects to carry out optional or mandatory extra
     * data.
     */
    typedef struct {
        u_int32_t      type;           /**< Unknown or scuba */
        binstr_t      *id;             /**< Unique identifier */
        bool_t         critical;       /**< The data type must be known to clients of this object - internal */
        octobj_vec_t  *transforms;     /**< Digest of this object - optional */
        binstr_t      *algorithm;      /**< Hash algorithm for transform - optional */
        binstr_t      *value;          /**< digest value - optional */
        binstr_t      *subject;        /**< ID of the object this extends - external */
        /**
         * Extensions can be scuba or other. 'Other' extensions
         * are unknown/unsupported extensions and are ignored.
         */
        union _extensiondata {
            void                *unknown;  /**< The extension data is unsupported */
            octobj_scubakeys_t  *scuba;    /**< Pointer to scuba key extension data */
        } data;
    } octobj_extension_t;

#define octobj_IsScubaExtension(o)   ((o)->type==OCTOBJ_EXTENSION_TYPE_SCUBA)
#define octobj_ScubaExtension(o)     ((o)->data.scuba)

    /**
     * Octopus protector object
     *
     * Protector objects maintain references to a key and
     * one or more content objects
     *
     * \specref See "Octopus Objects 1.0, section 2.4"
     */
    typedef struct {
        binstr_t           *id;            /**< optional */
        octobj_reference_t *contentkeyref; /**< Content key ID */
        octobj_vec_t       *contentrefs;   /**< container of content IDs */
    } octobj_protector_t;

    /**
     * Content key object
     *
     * \specref See "Octopus Objects 1.0.1, section 2.3"
     */
    typedef struct {
        binstr_t      *id;          /**< ID for references used by Protectors and Controllers */
        octobj_key_t  *secretkey;   /**< Unique key for encrypting content */
        octobj_vec_t  *attrs;       /**< Container of content key attributes */
        octobj_vec_t  *extensions;  /**< Internal extension list */
    } octobj_contentkey_t;

    /**
     * \nd
     */
    typedef struct {
        binstr_t *revocationinfo;
        binstr_t *encryptedkeys;
    } octobj_bkb_t;

    /**
     * Octopus Controller object
     *
     * Controller objects maintain references to a control
     * object and one or more content key objects
     *
     * \specref See "Octopus Objects 1.0.1, section 2.6"
     */
    typedef struct {
        binstr_t           *id;             /**< Unique object identifier */
        octobj_reference_t *controlref;     /**< Control object ID */
        octobj_vec_t       *contentkeyrefs; /**< Container of CK object IDs */
        octobj_vec_t       *attrs;          /**< Container of controller attributes */
        octobj_vec_t       *extensions;     /**< Internal extension list */
    } octobj_controller_t;

#define OCTOBJ_VB_TYPE_INT        1
#define OCTOBJ_VB_TYPE_REAL       2
#define OCTOBJ_VB_TYPE_STR        3
#define OCTOBJ_VB_TYPE_DATE       4
#define OCTOBJ_VB_TYPE_PARAM      5
#define OCTOBJ_VB_TYPE_EXPARAM    6
#define OCTOBJ_VB_TYPE_RES        7
#define OCTOBJ_VB_TYPE_VALLIST    8
#define OCTOBJ_VB_TYPE_BYTEARRAY  9


    /**
     *
     */
    typedef struct {
        binstr_t                        *name;
        size_t                           values_len;
        struct octobj_param_value_block *values;
    } octobj_param_block_t;

    typedef struct {
        size_t len;
        octobj_param_block_t *params;
    } octobj_params_t;

    typedef struct octobj_param_value_block {
        int32_t  type;
        union {
            int32_t               i;
            float                 f;
            binstr_t             *str;
            u_int32_t             date;
            binstr_t             *resource;
            octobj_params_t      *param;
            /*        np_esb_exparameter_t *exparam;*/
            octobj_param_block_t *vallist;
            binstr_t             *bytearray;
        } data;
    } octobj_param_value_block_t;

    /**
     * Octopus Control object
     *
     * Controls contain the information that allows Octopus to make
     * decisions on actions (Play, Pause, etc)
     *
     * \specref See "Octopus Objects 1.0.1, section 2.5"
     */
    typedef struct {
        binstr_t      *signature;    /**< Signature of the control (XML) */
        binstr_t      *sigref;       /**< Signature Reference Id */
        binstr_t      *id;           /**< Unique object identifier */
        octobj_vec_t  *attrs;        /**< Container of control attributes */
        octobj_vec_t  *extensions;   /**< Internal extension list */
        binstr_t      *protocol;     /**< Set to http://www.octopus-drm.com/specs/scp-1_0 */
        binstr_t      *type;         /**< Set to http://www.octopus-drm.com/specs/pkcm-1_0 */
        binstr_t      *codemodule;   /**< Plankton module for control governance */
        binstr_t      *ownerid;      /**< Object owner identifier */
    } octobj_control_t;

    /**
     * Signed directed edge assertion object
     *
     * Used when discovering if there a given node is "reachable" from another.
     *
     * \specref See "Octopus Objects 1.0.1, section 3.2"
     */
    typedef struct {
        binstr_t           *id;         /**< Unique object identifier */
        binstr_t           *fromid;     /**< Id of the "from" node object */
        binstr_t           *toid;       /**< Id of the "to" node object */
        octobj_vec_t       *attrs;      /**< Container of link attributes (e.g. lifetimes) - optional */
        octobj_vec_t       *extensions; /**< Nodes sharing keys for distribution - optional */
        octobj_control_t   *control;    /**< Constraint for link validity - optional */
    } octobj_link_t;

    /**
     * Any entity on the DRM system (User, Device, etc)
     *
     * Nodes can be connected to each other (e.g. in a "belongs to"
     * relationship for users and domains) via links. A node object
     * has no semantics for describing what it actually represents.
     * The DRM profile (e.g. Marlin DRM) will be in charge of defining
     * what is exactly associated with the node.
     *
     * \specref See "Octopus Objects 1.0.1, section 3.1"
     */
    typedef struct {
        binstr_t            *id;           /**< Unique object identifier */
        octobj_vec_t        *attrs;        /**< Define certain aspects of what the node represents */
        /* internal */
        octobj_vec_t        *extensions;   /**< public keys (Scuba) */
        /* external */
        octobj_extension_t  *escubakeys;   /**< secret/private keys */
    } octobj_node_t;

    /**
     * External license extension used for VOD (License Transfer Protocol),
     *
     */
    typedef struct {
        octobj_extension_t *extension;     /**< Bundle extension */
        octobj_vec_t       *controls;      /**< Agent and delegates */
    } octobj_ltpextension_t;

    /**
     * Governing object for content access.
     */
    typedef struct {
        octobj_control_t      *control;       /**< includes and protects the control program */
        octobj_controller_t   *controller;    /**< Binds the control with the content keys */
        int32_t                n_keys;        /**< Number of content keys governed by this licence */
        octobj_vec_t          *contentkeys;   /**< Container of content keys under governance */
        octobj_bkb_t          *bkb;           /**< Broadcast ket block handle */
        octobj_vec_t          *protectors;    /**< container of bound content and content keys */
        octobj_ltpextension_t *ltp_extension; /**< External extension for LTP. */
        octobj_vec_t          *external;      /**< External attributes used for Action Perform. */
    } octobj_license_t;

    typedef struct {
        u_int32_t  num;
        binstr_t **id;
    } octobj_sl_idlist_t;

    typedef struct {
        int32_t reset;
        int32_t serial; /** Serial number. The client must maintain the
                       highest serial number value it has processed
                       for a given service container name */
        binstr_t *servicename; /** Service container name */
        octobj_sl_idlist_t *add; /** Set of logical Ids to add to this service container */
        octobj_sl_idlist_t *sub; /** Set of logical Ids to subtract from this service container */
    } octobj_sl_update_t; /** License Suspension List Update for a given service container name */

    typedef struct {
        u_int32_t num;
        octobj_sl_update_t **update;
    } octobj_sl_updates_t; /** Multiple License Suspension Updates */

    typedef struct {
        int32_t serial;
        int32_t num;
        binstr_t *servicename;
    } octobj_sl_header_t;

    typedef struct octobj_sl_listnode {
        struct octobj_sl_listnode *prev;
        struct octobj_sl_listnode *next;
        binstr_t *id;
    } octobj_sl_listnode_t;

    typedef struct octobj_sl_list {
        octobj_sl_header_t    *header;
        octobj_sl_listnode_t  *first;
        octobj_sl_listnode_t  *cur;
        octobj_sl_listnode_t  *last;
        int32_t count;
    } octobj_sl_list_t;

    typedef struct octobj_sl_master {
        int32_t num;
        octobj_sl_list_t **lists;
    } octobj_sl_master_t;

    /**
     * Security assertion for a link
     */
    typedef struct {
        int32_t             type;          /**< Assertion type (Role/Data Cert) */
        binstr_t            *id;           /**< Unique object identifier */
        time_date_t         issueInstant;  /**< Time assertion was created  */
        time_date_t         *notBefore;    /**< Assertion commencement - optional */
        time_date_t         *notOnOrAfter; /**< Assertion expiration - optional */
        octobj_vec_t        *attrs;        /**< container of SAML attributes - optional */
        binstr_t            *nameId;
    } octobj_assertion_t;

    /** generic object base */
    typedef struct {
        binstr_t      *id; /**< optional */
    } octobj_common_t;

    /**
     * Generic Octopus object
     *
     * This acts as a sort of base class when passing objects around
     * or populating containers
     */
    typedef struct {
        int32_t type;                         /**< Specifies the object type */
        union {
            octobj_protector_t  *protector;
            octobj_contentkey_t *contentkey;
            octobj_controller_t *controller;
            octobj_control_t    *control;
            octobj_link_t       *link;
            octobj_node_t       *node;
            octobj_common_t     *common;
        } obj;                                /**< Pointer to the type of object */
    } octobj_t;

    /*---------------------------------------------------------------------------*/

    /**
     * Container constructor
     *
     * @param[in] in_size Container size
     *
     * @return Pointer to a new container, or NULL if there is a memory error
     */
    extern octobj_vec_t *
    octobj_AllocVec(int32_t in_size);

    /**
     * Container descructor
     *
     * @param[in,out] io_vec Pointer to the container
     * @param[in] in_itemtype The type of objects in the container (Link, Node, etc)
     *
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeVec(octobj_vec_t *io_vec, int32_t in_itemtype);

#define octobj_VecSize(in_vec) ((in_vec)->size)             /**< Container size macro \todo \em in_vec is not checked for non-null*/
#define octobj_VecItem(in_vec,idx) ((in_vec)->item[idx])    /**< Container acessor macro \todo \em in_vec and \em idx are not checked for validity */

    /**
     * Free the resources on an allocated reference object
     *
     * @param[in,out] io_reference Pointer to reference object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeReference(octobj_reference_t *io_reference);

    /**
     * Free the resources of an allocated key object
     *
     * @param[in,out] io_key Pointer to key object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeKey(octobj_key_t *io_key);

    /**
     * Free the resources of an allocated paired key object
     *
     * @param[in,out] io_pairkey Pointer to paired key object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreePairedKey(octobj_pairedkey_t *io_pairkey);

    /**
     * Free the resources of an allocated scuba keys object
     *
     * @param[in,out] io_scubakeys Pointer to scuba keys object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeScubaKeys(octobj_scubakeys_t *io_scubakeys);

    /**
     * Free the resources of an allocated attributes object
     *
     * @param[in,out] io_attr Pointer to attributes key object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeAttribute(octobj_attribute_t *io_attr);

    /**
     * Free the resources of an allocated SAML attribute object
     *
     * @param[in,out] io_attr Pointer to SAML attribute object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeSamlAttribute(octobj_samlattribute_t *io_attr);

    /**
     * Free the resources of an allocated Protector object
     *
     * @param[in,out] io_prot Pointer to Protector object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeProtector(octobj_protector_t *io_prot);

    /**
     * Free the resources of an allocated Content key object
     *
     * @param[in,out] io_ckey Pointer to Content key object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeContentKey(octobj_contentkey_t *io_ckey);

    /**
     * Free the resources of an allocated BKB block object
     *
     * @param[in,out] io_bkb Pointer to BKB block object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeBKB(octobj_bkb_t *io_bkb);

    /**
     * Free the resources of an allocated Controller object
     *
     * @param[in,out] io_controller Pointer to Controller object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeController(octobj_controller_t *io_controller);

    /**
     * Free the resources of an allocated Control object
     *
     * @param[in,out] io_control Pointer to Control object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeControl(octobj_control_t *io_control);

    /**
     * @brief Free the resources of an allocated LTP Extension object
     *
     * @param[in,out] io_ext Pointer to LTP extension object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeLtpExtension(octobj_ltpextension_t *io_ext);

    /**
     * @brief Free the resources of an allocated Extension object
     *
     * @param[in,out] io_ext Pointer to extension object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeExtension(octobj_extension_t *io_ext);

    /**
     * Free the resources of an allocated Link object
     *
     * @param[in,out] io_link Pointer to Link object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeLink(octobj_link_t *io_link);

    /**
     * Free the resources of an allocated Octopus Node object
     *
     * @param[in,out] io_node Pointer to Octopus Node object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeNode(octobj_node_t *io_node);

    /**
     * Constructor for an Octopus Licence
     *
     * @return Pointer to a new licence object, or NULL if there is a memory error
     */
    extern octobj_license_t *
    octobj_AllocLicense(void);

    /**
     * Free the resources of an allocated licence
     *
     * @param[in,out] io_licence Pointer to licence
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeLicense(octobj_license_t *io_licence);

    /**
     * Free the resources of an allocated assertion object
     *
     * @param[in,out] io_assertion Pointer to assertion object
     * @return ::ERR_OK
     */
    extern retcode_t
    octobj_FreeAssertion(octobj_assertion_t *io_assertion);

    extern void
    octobj_FreeParams( octobj_params_t *io_params );

    extern void
    octobj_FreeParamBlock( octobj_param_block_t *io_param );

    extern void
    octobj_FreeParamValueBlock( octobj_param_value_block_t *io_vb );

    /**
     * Set the structure of an Octopus key object
     *
     * This initializer function sets the keys flags to NONE.
     *
     * @param[in,out] io_key          Key to populate
     * @param[in]    in_id           Unique identifier for this object
     * @param[in]    in_usage        Designates the key as sharing or confidentiality
     * @param[in]    in_format       File format of key (PKCS #8, X509SPKI or Raw)
     * @param[in]    in_encalgorithm Algorithm used to encrypt the key
     * @param[in]    in_oaepparam   oaep parameters for rsa-oaep encryption algorithm
     * @param[in]    in_enckeyid     ID of the key used to encrypt this key
     * @param[in]    in_midalgorithm If protected data is too large and public keys are used, an intermediate
     *                               block cipher (AES-128-CBC) is used to encrypt the data and the symmetric
     *                               key is encrypted with the public key.
     *                               @specref Marlin Core Specification section 3.3.3.2.2
     * @param[in]    in_midkeydata   Mid algorithm key, encrypted with a public key
     * @param[in]    in_keydata      Payload of the encrypted key
     * @param[in]    in_keytype      Key type, one of ::HCI_IMP_KEY_TYPE_CONTENT, ::, ::HCI_IMP_KEY_TYPE_BROADCAST etc
     *
     * @return ::ERR_OK, ::ERR_ILLEGALARGS
     */
    retcode_t
    octobj_SetKey(
        octobj_key_t       *io_key,
        binstr_t           *in_id,
        int32_t             in_usage,
        int32_t             in_format,
        int32_t             in_encalgorithm,
        binstr_t           *in_oaepparam,
        binstr_t           *in_enckeyid,
        int32_t             in_midalgorithm,
        binstr_t           *in_midkeydata,
        binstr_t           *in_keydata,
        int32_t             in_keytype);

    /**
     * Set the structure of an Octopus paired key object
     *
     * This initializer function sets the keys flags to NONE.
     *
     * @param[in,out] io_key          Key to populate
     * @param[in]    in_pairid       Unique id for the pair
     * @param[in]    in_id           Unique identifier for this object
     * @param[in]    in_usage        Designates the key as sharing or confidentiality
     * @param[in]    in_format       File format of key (PKCS #8, X509SPKI or Raw)
     * @param[in]    in_encalgorithm Algorithm used to encrypt the key
     * @param[in]    in_oaepparam   oaep parameters for rsa-oaep encryption algorithm
     * @param[in]    in_enckeyid     ID of the key used to encrypt this key
     * @param[in]    in_midalgorithm If protected data is too large and public keys are used, an intermediate
     *                               block cipher (AES-128-CBC) is used to encrypt the data and the symmetric
     *                               key is encrypted with the public key.
     *                               @specref Marlin Core Specification section 3.3.3.2.2
     * @param[in]    in_midkeydata   Mid algorithm key, encrypted with a public key
     * @param[in]    in_keydata      Payload of the encrypted key
     * @param[in]    in_keytype      Key type, when decrypting xml this will be the domain key
     *
     * @return ::ERR_OK, ::ERR_ILLEGALARGS
     */
    retcode_t
    octobj_SetPairedKey(
        octobj_pairedkey_t    *io_key,
        binstr_t              *in_pairid,
        binstr_t              *in_id,
        int32_t                in_usage,
        int32_t                in_format,
        int32_t                in_encalgorithm,
        binstr_t              *in_oaepparam,
        binstr_t              *in_enckeyid,
        int32_t                in_midalgorithm,
        binstr_t              *in_midkeydata,
        binstr_t              *in_keydata,
        int32_t                in_keytype);

    /*---------------------------------------------------------------------------*/
    /**
     * Serialize out a Master Licence Suspension List.
     *
     * @param[in,out] io_sl Master LSL structure
     * @param[out]    out_buf String buffer to hold the serialized info
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    retcode_t
    octobj_SerializeSL(octobj_sl_master_t *io_sl, binstr_t **out_buf);

    /*---------------------------------------------------------------------------*/
    /**
     * Parse a serialized Master Licence Suspension list
     *
     * @param[in] in_sl          Serialized Master Suspension List
     * @param[out] out_sl        Suspension Master list structure to populate
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     */
    retcode_t
    octobj_GetSuspensionList(binstr_t *in_sl,
                             octobj_sl_master_t *out_sl);

    /*---------------------------------------------------------------------------*/
    /**
     * Parse a serialized Master Licence Suspension list and return the
     * list corresponding to the service container name
     *
     * @param[in] in_sl          Serialized Master Suspension List
     * @param[in] in_servicename Service container name for the specific list
     * @param[out] out_sl        Suspension list structure to populate
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_NG
     */
    retcode_t
    octobj_GetSpecificSuspensionList(binstr_t *in_sl,
                                     binstr_t *in_servicename,
                                     octobj_sl_list_t **out_sl);

    /*---------------------------------------------------------------------------*/
    /**
     * Destroy a License Suspension list
     *
     * @param[in,out] io_sl Pointer to a single suspension list
     */
    void
    octobj_FreeSL(octobj_sl_list_t *io_sl);


    /*---------------------------------------------------------------------------*/
#if defined(NS_DEBUG)

    extern void
    octobj_PrintVec(const octobj_vec_t *in_obj, int32_t in_type);

    extern void
    octobj_PrintReference(const octobj_reference_t *in_reference);

    extern void
    octobj_PrintKey(const octobj_key_t *in_key);

    extern void
    octobj_PrintPairedKey(const octobj_pairedkey_t *in_pairedkey);

    extern void
    octobj_PrintExtension(const octobj_extension_t *in_ext);

    extern void
    octobj_PrintAttribute(const octobj_attribute_t *in_attribute);

    extern void
    octobj_PrintSamlAttribute(const octobj_samlattribute_t *in_attribute);

    extern void
    octobj_PrintProtector(const octobj_protector_t *in_protector);

    extern void
    octobj_PrintContentKey(const octobj_contentkey_t *in_contentkey);

    extern void
    octobj_PrintController(const octobj_controller_t *in_controller);

    extern void
    octobj_PrintControl(const octobj_control_t *in_control);

    extern void
    octobj_PrintLink(const octobj_link_t *in_link);

    extern void
    octobj_PrintNode(const octobj_node_t *in_node);

    extern void
    octobj_PrintLicense(const octobj_license_t *in_license);

    extern void
    octobj_PrintAssertion(const octobj_assertion_t *in_assertion);

#else

#define octobj_PrintVec(o,t)
#define octobj_PrintReference(o)
#define octobj_PrintKey(o)
#define octobj_PrintPairedKey(o)
#define octobj_PrintExtension(o)
#define octobj_PrintAttribute(o)
#define octobj_PrintSamlAttribute(o)
#define octobj_PrintProtector(o)
#define octobj_PrintContentKey(o)
#define octobj_PrintController(o)
#define octobj_PrintControl(o)
#define octobj_PrintLink(o)
#define octobj_PrintNode(o)
#define octobj_PrintLicense(o)
#define octobj_PrintAssertion(o)

#endif

    /*---------------------------------------------------------------------------*/

    /**
     * Serialisation of a generic octopus object to a string
     *
     * @param[in] in_obj The octopus object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK, ::ERR_NOMEM, ::ERR_ILLEGALARGS, ::ERR_OCTOBJ_NOTRAWKEY, ::ERR_INTERNALERROR, ::ERR_OCTOBJ_UNSUPPORTED
     */
    extern retcode_t
    octobj_CBSSerialize(const octobj_t *in_obj, binstr_t **out_str);

    /**
     * Serialisation of an octopus controller object to a string
     *
     * @param[in] in_obj The octopus controller object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK, ::ERR_NOMEM, ::ERR_ILLEGALARGS
     */
    extern retcode_t
    octobj_CBSSerializeController(const octobj_controller_t *in_obj, binstr_t **out_str);

    /**
     * Serialisation of an octopus control object to a string
     *
     * @param[in] in_obj The octopus control object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK, ::ERR_NOMEM, ::ERR_ILLEGALARGS
     */
    extern retcode_t
    octobj_CBSSerializeControl(const octobj_control_t *in_obj, binstr_t **out_str);

    /**
     * Serialization of an octopus content key object to a string
     *
     * @note  The actual value of the content key is Highly-Confidential Information
     *        and cannot be returned in plain text.
     *        Therefore, the value is replaced by all '0's.
     *
     * @param[in] in_obj The octopus content key object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK, ::ERR_NOMEM, ::ERR_ILLEGALARGS, ::ERR_OCTOBJ_NOTRAWKEY, ::ERR_INTERNALERROR
     */
    extern retcode_t
    octobj_CBSSerializeContentKey(const octobj_contentkey_t *in_obj, binstr_t **out_str);

    /**
     * Serialisation of an octopus node object to a string
     *
     * @param[in] in_obj The octopus node object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK, ::ERR_NOMEM, ::ERR_ILLEGALARGS
     */
    extern retcode_t
    octobj_CBSSerializeNode(const octobj_node_t *in_obj, binstr_t **out_str);

    /**
     * Serialisation of an octopus scuba key (internal extension) object to a string
     *
     * @param[in] in_obj The octopus scuba key (as internal extension) object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_OCTOBJ_NOTRAWKEY
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_OCTOBJ_INVALIDOBJECT
     * @return ::ERR_ILLEGALARGS
     */
    extern retcode_t
    octobj_CBSSerializeIntScubaKeys(const octobj_scubakeys_t *in_obj, binstr_t **out_str);

    /**
     * Serialisation of an octopus link object to a string
     *
     * @param[in] in_obj The octopus link object to serialize
     * @param[out] out_str The buffer to create and populate
     *
     * @return ::ERR_OK, ::ERR_NOMEM, ::ERR_ILLEGALARGS
     */
    extern retcode_t
    octobj_CBSSerializeLink(const octobj_link_t *in_obj, binstr_t **out_str);


    /**
     * @brief Constructs a secret key object, a.k.a. as <SecretKey> in XML
     *
     * @note  *out_secret_key becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeKey
     *
     * @param[in]    in_id           Unique identifier for this object
     * @param[in]    in_usage        Designates the key as sharing or confidentiality
     * @param[in]    in_format       File format of key (PKCS #8, X509SPKI or Raw)
     * @param[in]    in_encalgorithm Algorithm used to encrypt the key
     * @param[in]    in_oaepparam    oaep parameters for rsa-oaep encryption algorithm
     * @param[in]    in_enckeyid     ID of the key used to encrypt this key
     * @param[in]    in_midalgorithm If protected data is too large and public keys are used, an intermediate
     *                               block cipher (AES-128-CBC) is used to encrypt the data and the symmetric
     *                               key is encrypted with the public key.
     *                               @specref Marlin Core Specification section 3.3.3.2.2
     * @param[in]    in_midkeydata   Mid algorithm key, encrypted with a public key
     * @param[in]    in_keydata      Payload of the encrypted key
     * @param[in]    in_keytype      Key type, one of ::HCI_IMP_KEY_TYPE_CONTENT, ::, ::HCI_IMP_KEY_TYPE_BROADCAST etc
     * @param[out]   out_secret_key  the created secret key object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    retcode_t
    octobj_CreateSecretKey(binstr_t           *in_id,
                           int32_t             in_usage,
                           int32_t             in_format,
                           int32_t             in_encalgorithm,
                           binstr_t           *in_oaepparam,
                           binstr_t           *in_enckeyid,
                           int32_t             in_midalgorithm,
                           binstr_t           *in_midkeydata,
                           binstr_t           *in_keydata,
                           int32_t             in_keytype,
                           octobj_key_t      **out_secret_key);

    /**
     * @brief Constructs the Reference Object
     *
     * @note  *out_reference becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeReference
     *
     * @param[in]  in_id          The ID of the object being referenced
     * @param[in]  in_transforms  Optional - digest format
     * @param[in]  in_algorithm   Optional - Hash used for digest
     * @param[in]  in_value       Optional - Digest value
     * @param[out] out_reference  Reference to the object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateReference(binstr_t             *in_id,
                           octobj_vec_t         *in_transforms,
                           binstr_t             *in_algorithm,
                           binstr_t             *in_value,
                           octobj_reference_t  **out_reference);

    /**
     * @brief Constructs the Content Key Object
     *
     * @note  *out_content_key becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeContentKey
     *
     * @param[in]  in_uid          uid of the content key
     * @param[in]  in_key          secret key object
     * @param[in]  in_attributes   NULL, or list of attributes
     * @param[in]  in_extensions   NULL, or list of extensions
     * @param[out] out_content_key the created Ck object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateContentKey(binstr_t              *in_uid,
                            octobj_key_t          *in_secret_key,
                            octobj_vec_t          *in_attributes,
                            octobj_vec_t          *in_extensions,
                            octobj_contentkey_t  **out_content_key);

    /**
     * @brief Constructs the License Protector object
     *
     * @note  *out_protector becomes the owner of in_uid, in_contentkeyref & in_contentrefs;
     *        they shall be freed by octobj_FreeProtector
     *
     * @param[in]  in_uid           uid of the protector
     * @param[in]  in_contentkeyref ContentKey ref
     * @param[in]  in_contentrefs   Content refs
     * @param[out] out_protector    the protector created
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateProtector(binstr_t             *in_uid,
                           octobj_reference_t   *in_contentkeyref,
                           octobj_vec_t         *in_contentrefs,
                           octobj_protector_t  **out_protector);

    /**
     * @brief Constructs an Attribute of type Integer
     *
     * @note  *out_attribute becomes the owner of in_name;
     *        it shall be freed by octobj_FreeAttribute
     *
     * @param[in]  in_name          Attribute's name
     * @param[in]  in_value         Attribute's value
     * @param[out] out_attribute    The created attribute
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateIntegerAttribute(binstr_t             *in_name,
                                  int32_t               in_value,
                                  octobj_attribute_t  **out_attribute);

    /**
     * @brief Constructs an Attribute of type String
     *
     * @note  in_value must not be NULL-terminated, since the <Attribute> element must not contain a NULL character.
     *        Since attributes of a control are made available in the Plankton context,
     *        and Plankton functions, such as Octopus.Links.IsNodeReachable, expect a NULL-terminated string
     *        it is octcxt's responsibility to add the NULL termination.
     *
     * @note  *out_attribute becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeAttribute
     *
     * @param[in]  in_name          Attribute's name
     * @param[in]  in_value         Attribute's value (without the NULL termination)
     * @param[out] out_attribute    The created attribute
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateStringAttribute(binstr_t             *in_name,
                                 binstr_t             *in_value,
                                 octobj_attribute_t  **out_attribute);

    /**
     * @brief Constructs an Attribute of type ByteArray
     *
     * @note  *out_attribute becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeAttribute
     *
     * @param[in]  in_name          Attribute's name
     * @param[in]  in_value         Attribute's value (array of bytes, not base64)
     * @param[out] out_attribute    The created attribute
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateByteArrayAttribute(binstr_t             *in_name,
                                    binstr_t             *in_value,
                                    octobj_attribute_t  **out_attribute);

    /**
     * @brief Constructs the License Control object
     *
     * @note  Control Protocol and Byte Code Type are specified in [Octopus - Controls] section 2.3 and 2.4
     *        They have fixed values, so they are not passed to the function
     *
     * @note ownerid field of octobj_control_t is not passed in to this function, because :
     *   - when generating a license, this field is not used;
     *   - when evaluating a license, this field is not set while parsing the license,
     *     but only afterwards, when the signatures are verified.
     *
     * @note  *out_control becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeControl
     *
     * @param[in]  in_uid              uid of the control
     * @param[in]  in_attributes       NULL, or list of attributes
     * @param[in]  in_extensions       NULL, or list of extensions
     * @param[in]  in_code_module      Code Module
     * @param[in]  in_control_protocol Control protocol
     * @param[in]  in_control_type     Control Type
     * @param[in]  in_signature        Control Signature (XML)
     * @param[in]  in_sigref           Control Signature reference Id
     * @param[out] out_control         The data for license's Control
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateControl(binstr_t            *in_uid,
                         octobj_vec_t        *in_attributes,
                         octobj_vec_t        *in_extensions,
                         binstr_t            *in_code_module,
                         binstr_t            *in_control_protocol,
                         binstr_t            *in_control_type,
                         binstr_t            *in_signature,
                         binstr_t            *in_sigref,
                         octobj_control_t   **out_control);

    /**
     * @brief Constructs the License Controller object
     *
     * @note  *out_controller becomes the owner of in_uid, in_attributes,in_contentkeyrefs,
     *         in_controlref and in_extensions they shall be freed by octobj_FreeController;
     *
     * @param[in]  in_uid             uid of the controller
     * @param[in]  in_contentkeyrefs  Vector of ContentKey refs
     * @param[in]  in_controlref      Control object ref
     * @param[in]  in_attributes      NULL, or list of attributes
     * @param[in]  in_extensions      NULL, or list of extensions
     * @param[out] out_controller     The created controller object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateController(binstr_t             *in_uid,
                            octobj_vec_t         *in_contentkeyrefs,
                            octobj_reference_t   *in_controlref,
                            octobj_vec_t         *in_attributes,
                            octobj_vec_t         *in_extensions,
                            octobj_controller_t **out_controller);

    /**
     * @brief Constructs a license
     *
     * @note  *out_license becomes the owner of the input parameters;
     *        they shall be freed by octobj_FreeLicense
     *
     * @param[in]  in_content_keys  Vector of ContentKey objects
     * @param[in]  in_protectors    Vector of Protectors
     * @param[in]  in_control       Control
     * @param[in]  in_controller    Controller
     * @param[in]  in_bkb           NULL, or bkb
     * @param[in]  in_ltp_extension NULL, or external extension for LTP
     * @param[in]  in_external      NULL, or external attributes used for action perform
     * @param[out] out_license      the created license object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateLicense(octobj_vec_t           *in_content_keys,
                         octobj_vec_t           *in_protectors,
                         octobj_control_t       *in_control,
                         octobj_controller_t    *in_controller,
                         octobj_bkb_t           *in_bkb,
                         octobj_ltpextension_t  *in_ltp_extension,
                         octobj_vec_t           *in_external,
                         octobj_license_t      **out_license);

    /**
     * @brief Constructs an extension
     *
     * @note  *out_extension becomes the owner of in_uid, in_transforms, in_algorithm, in_subject
     *         in_value and they shall be freed by octobj_FreeExtension;
     *
     * @param[in]   in_uid              Unique object ID
     * @param[in]   in_critical         Flag that denotes if the extension data type must be known
     * @param[in]   in_transforms       (Optional) Canonicalisation method
     * @param[in]   in_algorithm        (Optional) Hash algorithm
     * @param[in]   in_value            (Optional) Value of the hash
     * @param[in]   in_subject          Specifies the subject this extends
     * @param[in]   in_extension_data   Extension data. If non-null this is Scuba.
     * @param[out]  out_extension       The created extension object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateExtension(binstr_t            *in_uid,
                           bool_t               in_critical,
                           octobj_vec_t        *in_transforms,
                           binstr_t            *in_algorithm,
                           binstr_t            *in_value,
                           binstr_t            *in_subject,
                           void                *in_extension_data,
                           octobj_extension_t **out_extension);

    /**
     * @brief Constructs an LTP extension
     *
     * @note  *out_extension becomes the owner of in_uid, in_extension, in_controls
     *         and they shall be freed by octobj_FreeLtpExtension;
     *
     * @param[in]   in_extension        Extension body
     * @param[in]   in_controls         The agent and delegate controls of the extension
     * @param[out]  out_ltp_extension   The created LTP extension object
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    octobj_CreateLtpExtension(octobj_extension_t     *in_extension,
                              octobj_vec_t           *in_controls,
                              octobj_ltpextension_t **out_ltp_extension);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* OCTOBJ_API_H */

