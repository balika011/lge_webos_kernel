/** @addtogroup seashell seashell
 * @ingroup marlin
 * SeaShell provides a secure state storage mechanism, such as
 * play-counts, date of first use, etc
 *
 * A SeaShell DB contains objects. Objects are stored in a hierarchy
 * of container objects.
 *
 * There are four types of object: String, Int, Array & Container.
 *
 * @specref SeaShell Object Store version 1.0.1
 *
 * @{
*/

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef SEASHELL_API_H
#define SEASHELL_API_H

/** @file
 * Interface for seashell database operation
 */

/***************************************
  def
 ***************************************/

typedef void            *seashell_handle_t;

#define ERR_SEASHELL_NOTSUPPORTED       (ERR_SEASHELL_BASE+0x01)
#define ERR_SEASHELL_CORRUPTED          (ERR_SEASHELL_BASE+0x02)
#define ERR_SEASHELL_NOTMOUNTED         (ERR_SEASHELL_BASE+0x10)
#define ERR_SEASHELL_UNKNOWNPATH        (ERR_SEASHELL_BASE+0x11)
#define ERR_SEASHELL_ILLEGALTYPE        (ERR_SEASHELL_BASE+0x12)
#define ERR_SEASHELL_ACCESSDENIED       (ERR_SEASHELL_BASE+0x13)
#define ERR_SEASHELL_TOOSMALLBUFFER     (ERR_SEASHELL_BASE+0x14)

#define SEASHELL_ROOT_CONTAINER_HANDLE        0
#define SSHELL_OCTPATH_CONTAINER_HANDLE_START 0x1000

/***************************************
  API
 ***************************************/

/**
 * Create an initial volume image
 *
 * @param[in]        in_path        the mount path or a null for the root path
 * @param[out]        out_dbsize    the SeaShell database size
 * @param[out]        out_dbsize    the SeaShell database
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_SEASHELL_UNKNOWNPATH
 * @return ::ERR_INTERNALERROR
 * @return Returns from ::asn_NewSequence, ::asn_NewInteger, ::asn_Append, ::asn_NewString,
 *         ::asn_Encode
 */
retcode_t
seashell_CreateVolume(
    u_int8_t             *in_path,
    np_size_t            *out_dbsize,
    u_int8_t            **out_dbbody);

/**
 * Create a SeaShell handle
 *
 * @param[out]        out_handle    the SeaShell handle
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
retcode_t
seashell_Init(
    seashell_handle_t        *out_handle);

/**
 * Destoy a SeaShell handle
 *
 * @param[in]        in_handle    the SeaShell handle
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 */
retcode_t
seashell_Fin(
    seashell_handle_t        in_handle);


/**
 * Load a SeaShell database
 *
 * @param[in,out]    io_handle    the SeaShell handle
 * @param[in]        in_dbsize    the SeaShell database size
 * @param[in]        in_dbbody    the SeaShell database
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_SEASHELL_NOTSUPPORTED
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_SEASHELL_CORRUPTED
 */
retcode_t
seashell_Load(
    seashell_handle_t    io_handle,
    np_size_t            in_dbsize,
    u_int8_t            *in_dbbody);

retcode_t
seashell_LoadLicenseStatus(
    seashell_handle_t    io_handle,
    np_size_t            in_dbsize,
    u_int8_t            *in_dbbody);

/**
 * Unload a SeaShell database
 *
 * @param[in,out]    io_handle    the SeaShell handle
 * @param[in]        in_path        the path or a null for the root path
 * @param[out]        out_dbsize    the SeaShell database size or a zero
 * @param[out]        out_dbbody    the SeaShell database or a null
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_SEASHELL_NOTSUPPORTED
 * @return ::ERR_SEASHELL_NOTMOUNTED
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_SEASHELL_CORRUPTED
 */
retcode_t
seashell_Unload(
    seashell_handle_t    io_handle,
    u_int8_t            *in_path,
    np_size_t           *out_dbsize,
    u_int8_t           **out_dbbody);

/**
 * Get an object
 *
 * @specref See section 4.1 of SeaShell Object Store version 1.0.1
 *
 * @param[in,out]    io_handle    the SeaShell handle
 * @param[in]        in_owner    the owner-id
 * @param[in]        in_ownersize    the owner-id size
 * @param[in]        in_parent    the parent container handle
 * @param[in]        in_name        the path name
 * @param[in,out]    io_buf        the buffer address
 * @param[in]        in_bufsize    the buffer size
 * @param[out]        out_typeid    the object type id
 * @param[out]        out_bufsize    the object size
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_SEASHELL_UNKNOWNPATH
 * @return ::ERR_SEASHELL_CORRUPTED
 * @return ::ERR_SEASHELL_TOOSMALLBUFFER
 * @return ::ERR_SEASHELL_NOTMOUNTED
 * @return ::ERR_SEASHELL_ACCESSDENIED
 */
retcode_t
seashell_GetObject(
    seashell_handle_t    io_handle,
    u_int8_t            *in_subj,
    np_size_t            in_subjsize,
    int32_t              in_parent,
    u_int8_t            *in_path,
    u_int8_t            *io_buf,
    u_int32_t            in_bufsize,
    int32_t             *out_typeid,
    u_int32_t           *out_bufsize);

/**
 * Set an object
 *
 * @specref See section 4.2 of SeaShell Object Store version 1.0.1
 *
 * @param[in,out]    io_handle    the SeaShell handle
 * @param[in]        in_owner    the owner-id
 * @param[in]        in_ownersize    the owner-id size
 * @param[in]        in_parent    the parent container handle
 * @param[in]        in_name        the path name
 * @param[in]        in_buf        the body of content
 * @param[in]        in_bufsize    the size of content
 * @param[in]        in_typeid    the object type id
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_SEASHELL_UNKNOWNPATH
 * @return ::ERR_SEASHELL_CORRUPTED
 * @return ::ERR_SEASHELL_ACCESSDENIED
 * @return ::ERR_SEASHELL_ILLEGALTYPE
 * @return ::ERR_SEASHELL_NOTMOUNTED
 * @return Returns from ::asn_Append, ::asn_Attach, ::asn_NewSequence, ::asn_NewString
 */
retcode_t
seashell_SetObject(
    seashell_handle_t    io_handle,
    u_int8_t            *in_subj,
    np_size_t            in_subjsize,
    int32_t              in_parent,
    u_int8_t            *in_path,
    u_int8_t            *in_buf,
    size_t               in_bufsize,
    int32_t              in_typeid);

#endif

/** @} */
