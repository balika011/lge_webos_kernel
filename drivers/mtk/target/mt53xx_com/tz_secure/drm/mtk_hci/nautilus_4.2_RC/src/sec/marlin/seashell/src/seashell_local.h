/** @addtogroup seashell seashell
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef SEASHELL_LOCAL_H
#define SEASHELL_LOCAL_H

/** @file
 * Internal structures and constant definitions for the SeaShell code
 */

#define SSHELL_CURRENT_VERSION      0x01000000

#define SSHELL_ASN_HEAD_VERSION     0x00
#define SSHELL_ASN_HEAD_PATH        0x01
#define SSHELL_ASN_HEAD_NODE        0x02

#define SSHELL_ASN_NODE_ATTR        0x00
#define SSHELL_ASN_NODE_DATA        0x01

#define SSHELL_ASN_NODE_TAG_CONTAINER  0x01
#define SSHELL_ASN_NODE_TAG_INTEGER    0x02
#define SSHELL_ASN_NODE_TAG_STRING     0x03
#define SSHELL_ASN_NODE_TAG_BINARY     0x04

/**
 * Wrapper for mounted database volume
 */
typedef struct sshell_volume_tag {
    struct sshell_volume_tag *m_next;      /**< Multiple volumes are stored as a linked list
                                            *   it can be implemented but hasn't been */
    u_int32_t                 m_version;   /**< Database version  */
    u_int8_t                 *m_path;      /**< Container path to this object  */
    asn_t                    *m_root;      /**< ASN.1 BER structure of the database */
    binstr_t                  m_src;       /**< Buffer containing the ASN.1 BER encoding of the database */
    int32_t                   m_dirty;     /**< Dumping images to file is only done on volumes
                                            *   that have been modified. This flag signifies a volume
                                            *   that was modified after the last dump */
} sshell_vol_t;

/**
 * Container object wrapper
 */
typedef struct sshell_container_tag {
    struct sshell_container_tag *m_next;    /**< Next container in the db  */
    u_int32_t                    m_handle;  /**< Handle of this container (indexed by sequence number) */
    u_int8_t                    *m_path;    /**< Path to this container */
} sshell_cont_t;

/**
 * General SeaShell structure
 */
typedef struct sshell_tag {
    sshell_vol_t   *m_vols;      /**< Root of database volumes             */
    int32_t         m_cont_seq;  /**< Number of containers in the database */
    sshell_cont_t  *m_cont;      /**< Root of container objects            */
} sshell_t;

#define SSHELL_TYPE_DUMMY        -1
#define SSHELL_TYPE_CONTAINER     0
#define SSHELL_TYPE_INTEGER       1
#define SSHELL_TYPE_STRING        2
#define SSHELL_TYPE_BINARY        3
#define SSHELL_FLAG_PUBLIC_READ    0x0001

/**
 * Metadata container for a stored object
 *
 * @specref See SeaShell Object Store version 1.0.1 section 2.1
 */
typedef struct sshell_attr_tag {
    binstr_t   m_name;     /**< Name of the object */
    binstr_t   m_owner;    /**< Id of the owner of the object */
    u_int32_t  m_type;     /**< ::SSHELL_TYPE_BINARY, ::SSHELL_TYPE_STRING,
                                ::SSHELL_TYPE_INTEGER, ::SSHELL_TYPE_CONTAINER */
    u_int32_t  m_flags;    /**< None or ::SSHELL_FLAG_PUBLIC_READ if the object can be read
                                by any client regardless of its identity */
    u_int32_t  m_create;   /**< Date of creation, as seconds since the epoc, in UTC */
    u_int32_t  m_modified; /**< Date of modification, as seconds since the epoc, in UTC */
    u_int32_t  m_expire;   /**< Date of expiration, as seconds since the epoc, in UTC */
} sshell_attr_t;

#define SSHELL_OCTPATH_MAX_DIR_DEPTH            16

#define SSHELL_OCTPATH_ROOT                "/Octopus/SeaShell/Databases/Marlin"
#define SSHELL_OCTPATH_ROOT_PREFIX         "/Octopus/SeaShell/Databases"
#define SSHELL_OCTPATH_ROOT_OWNER          "urn:marlin:drmservices:seashell"
#define SSHELL_OCTPATH_LICENSE_STATUS      "/Octopus/SeaShell/Databases/Marlin/sne/l"

/**
 * Volume snapshot
 */
typedef struct seashell_image_tag {
    u_int8_t  *m_path;   /**< Full path of the volume    */
    np_size_t  m_size;   /**< Size of database           */
    u_int8_t  *m_body;   /**< Pointer to database buffer */
} seashell_image_t;



/**
 * Dump modified SeaShell database images
 *
 * @param[in]           in_handle       the SeaShell handle
 * @param[out]          out_num         the number of databases
 * @param[out]          out_images      the databases
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_SEASHELL_CORRUPTED
 */
retcode_t
seashell_DumpImage(
    seashell_handle_t           in_handle,
    int32_t            *out_num,
    seashell_image_t          **out_images);

/**
 * Free a SeaShell database image
 *
 * @param[in]           in_handle       the SeaShell handle
 * @param[in]           in_image        the database
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 */
retcode_t
seashell_FreeImage(
    seashell_handle_t           in_handle,
    seashell_image_t           *in_image);

/**
 * Free SeaShell database images
 *
 * @param[in]           in_handle       the SeaShell handle
 * @param[in]           in_num          the number of databases
 * @param[in]           in_images       the databases
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return Returns from ::seashell_FreeImage
 */
retcode_t
seashell_FreeImages(
    seashell_handle_t           in_handle,
    int32_t                             in_num,
    seashell_image_t           *in_images);

#endif
/** @} */
