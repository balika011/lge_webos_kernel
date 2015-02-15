/**
 * @addtogroup octcxt octcxt
 * @ingroup marlin
 * @{
 * Octopus Host Contexts Implementation
 *
 * Octopus objects can represent different things depending on the context of
 * the "DRM profile". In our case, the "DRM profile" is the Marlin DRM. This
 * API provides handling of Octopus objects in a Marlin context.
 *
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 * @date   9 Dec, 2004 last modified by n-goto
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef OCTCXT_API_H
#define OCTCXT_API_H

NP_BEGIN_EXTERN_C

#define OCTCXT_ASSERTION_MAX_IDX        256

/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

/** Returned from a failed ::octcxt_SetAssertion for unrecognised assertion type */
#define ERR_OCTCXT_NOTIMPLEMENTED     (ERR_OCTCXT_BASE|1)

/** Returned if an index does not correspond to a valid object */
#define ERR_OCTCXT_OBJECTNOTFOUND     (ERR_OCTCXT_BASE|2)

/** Returned if we are using more than 7 dynamic containers. */
#define ERR_OCTCXT_INSUFFICIENT_SPACE (ERR_OCTCXT_BASE|3)

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

/** Handle to the Octopus Context */
typedef void *octcxt_handle_t;

/**
 * Initialize a Octopus context handle.
 *
 * @param[out] out_handle Handle to the Octopus context
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 */
extern retcode_t
octcxt_Init(octcxt_handle_t *out_handle);

/**
 * Set the ID, attributes and keys for an Octopus context personality.
 *
 * @param[in] in_handle Handle to the Octopus context
 * @param[in] in_personality Pointer to the personality node
 * @return ::ERR_OK or ::ERR_ILLEGALHANDLE
 */
extern retcode_t
octcxt_SetPersonality(octcxt_handle_t in_handle, octobj_node_t *in_personality);

/**
 * Set the parameter block for a given context
 *
 * @param[in] in_hanle Handle the Octopus context
 * @param[in] in_params    Parameter blocks
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 */
extern retcode_t
octcxt_SetParameters(octcxt_handle_t in_handle,
                     octobj_params_t *in_params);

/**
 * Set the control, controller, content keys, BKB and protectors for an
 * Octopus context licence.
 *
 * @param[in] in_handle Handle to the Octopus context
 * @param[in] in_license Pointer to the licence object
 * @return ::ERR_OK or ::ERR_ILLEGALHANDLE
 */
extern retcode_t
octcxt_SetLicense(octcxt_handle_t in_handle, octobj_license_t *in_license);

/**
 * Set the link manager structure for a given context
 *
 * @param[in] in_handle Handle to the Octopus context
 * @param[in] in_links  Pointer to a container of links
 * @return ::ERR_OK or returns from ::octcxt_SetLicense
 */
extern retcode_t
octcxt_SetLinks(octcxt_handle_t in_handle, octobj_vec_t *in_links);

/**
 * Set a role or datacert assertion for a context
 *
 * @param[in] in_handle Handle to the Octopus context
 * @param[in] in_assertion Pointer to the Octopus assertion object
 * @return ::ERR_OK, ::ERR_ILLEGALHANDLE or ::ERR_OCTCXT_NOTIMPLEMENTED
 */
extern retcode_t
octcxt_SetAssertion(octcxt_handle_t in_handle, octobj_assertion_t *in_assertion);

/**
 * Free all assertions for a context
 *
 * @param[in] in_handle Handle to the Octopus context
 * @return ::ERR_OK, ::ERR_ILLEGALHANDLE
 */
extern void
octcxt_FreeAssertions(octcxt_handle_t in_handle);

/**
 * Set the ID for the octopus context.
 *
 * This is set by plankton code in order to keep track of
 * the objects created.
 *
 * @param[in,out] in_handle Handle to the Octopus context
 * @param[in] in_contextid  Context ID
 * @return ::ERR_OK or ::ERR_ILLEGALHANDLE
 */
extern retcode_t
octcxt_SetContextId(octcxt_handle_t io_handle, binstr_t *in_contextid);

/**
 * Destructor for an octopus context
 *
 * @param[in,out] io_handle Handle to the Octopus context
 * @return ::ERR_OK
 */
extern retcode_t
octcxt_Fin(octcxt_handle_t io_handle);

/**
 * Get trusted time, either as an estimate or a precise value
 *
 * @param[in] in_hostcontext  Not used.
 * @param[out] out_time time from ::tmcxt_GetTrustedTime / 60
 * @param[out] flag either ::PKT_TRUSTED_TIME_FLAG_NONE or ::PKT_TRUSTED_TIME_FLAG_ESTIMATE
 * @return void
 */
void
octcxt_GetTrustedTime(pkt_hostcontext_t *in_hostcontext, u_int32_t *out_time, u_int32_t *out_flag);

/**
 * Get local time
 *
 * @param[in] in_hostcontext  Not used.
 * @param[out] out_time time from (::tmcxt_GetTrustedTime + timezone offset) / 60
 * @return void
 */
extern void
octcxt_GetLocalTime(pkt_hostcontext_t *in_hostcontext, u_int32_t *out_time);

/**
 * Get the local time offset
 *
 * The offset is the difference between the local time and the system time
 *
 * @param[in] in_hostcontext  Not used.
 * @param[out] out_offset Pointer to the address to store the offset in
 * @return void
 */
extern void
octcxt_GetLocalTimeOffset(pkt_hostcontext_t *in_hostcontext, int32_t *out_offset);

/**
 * Get local time, adjusted from system time with the offset
 *
 * @param[in] in_hostcontext  Not used.
 * @param[out] out_time Pointer to the address to store the time in
 * @param[out] out_lag  Pointer to the address to store the
 * @return void
 */
extern void
octcxt_GetLocalTrustedTime(pkt_hostcontext_t *in_hostcontext, u_int32_t *out_time, int32_t *out_lag);

/**
 * Read an object
 *
 * For SeaShell objects this function will read the object from the database.
 *
 * @param[in]      in_hostcontext   DRM object with a handle to the SeaShell implementation
 * @param[in]      in_parent        Parent container object
 * @param[in]      in_name          Name of the object (valid characters are
 *                                  A-Z,a-z,0-9,'_','-','+'':','.','$','!','*',' ')
 * @param[in,out]  io_buffer        Object data
 * @param[in]      in_buffersize    Size of the data buffer
 * @param[in]      in_ownerid       Id of the owner (parent) in the database
 * @param[in]      in_ownderidsize  Length of the owner name
 * @param[in]      in_attrs         AttributeList in Control
 * @param[out]     out_typeid
 * @param[out]     out_typeidsize
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_SEASHELL_ACCESSDENIED
 * @return ::ERR_SEASHELL_UNKNOWNPATH
 * @return ::ERR_SEASHELL_TOOSMALLBUFFER
 * @return ::ERR_OCTCXT_OBJECTNOTFOUND
 */
extern retcode_t
octcxt_GetObject(pkt_hostcontext_t *in_hostcontext,
                 int32_t in_parent,
                 u_int8_t *in_name,
                 u_int8_t *io_buffer,
                 u_int32_t in_buffersize,
                 u_int8_t *in_ownerid,
                 u_int32_t in_owneridsize,
                 void *in_attrs,
                 int32_t *out_typeid,
                 int32_t *out_size);

/**
 * Sets the parameters of an Octopus object and stores it.
 *
 * Only SeaShell stored objects are set in this function. This sets them
 * in the object store.
 *
 * \specref See SeaShell Object Store, Version 1.0
 *
 * @param[in] in_hostcontext  DRM object with a handle to the SeaShell implementation
 * @param[in] in_parent       Parent container object for this object
 * @param[in] in_name         Name of the object (valid characters are
 *                            A-Z,a-z,0-9,'_','-','+'':','.','$','!','*',' ')
 * @param[in] in_buffer       Object data
 * @param[in] in_buffersize   Size of data
 * @param[in] in_ownerid      Id of the owner (parent) in the database
 * @param[in] in_owneridsize  Length of the parent container name
 * @param[in] in_typeid       One of ::SSHELL_TYPE_CONTAINER, ::SSHELL_TYPE_INTEGER,
 *                            ::SSHELL_TYPE_STRING, ::SSHELL_TYPE_BINARY
 * @param[out] out_result     Internal error code
 * @return ::ERR_OK or result from ::seashell_SetObject
 */
extern retcode_t
octcxt_SetObject(pkt_hostcontext_t *in_hostcontext,
                 int32_t in_parent,
                 u_int8_t *in_name,
                 u_int8_t *in_buffer,
                 u_int32_t in_buffersize,
                 u_int8_t *in_ownerid,
                 u_int32_t in_owneridsize,
                 int32_t in_typeid,
                 int32_t *out_result);

/**
 * Verify there is a directed path from one node to another
 *
 * @specref Octopus Objects 1.0 section 3.2
 *
 * @param[in,out] io_hostcontext DRM object with a handle to the starting nodes context
 * @param[in]     in_toid        String representation of the node that terminates the path
 * @param[in]     in_toidsize    Length of the ID string
 * @param[out]    out_result     Internal plankton result, either ::PKT_CONTEXT_ERR_SUCCESS or ::PKT_CONTEXT_ERR_FAILURE
 * @return ::ERR_OK or ::ERR_ILLEGALHANDLE
 */
extern retcode_t
octcxt_IsNodeReachable(pkt_hostcontext_t *io_hostcontext,
                       np_size_t in_toidsize,
                       u_int8_t *in_toid,
                       int32_t *out_result);

/**
 * Set the object store destination for an object
 *
 * If the store does not exist it will create one.
 *
 * @param[in,out] io_handle   Handle to the objects context
 * @param[in]     in_seashell pointer to the seashell stores path string
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return values from ::seashell_CreateVolume, ::seashell_Init, ::seashell_Load
 */
retcode_t
octcxt_SetSeaShell(octcxt_handle_t io_handle, mbinstr_t *in_seashell);

/**
 * Get the store path for an object
 *
 *
 * @param[in]     in_handle   Handle to the objects context
 * @param[out]    out_seashell Storage pointer for seashell(s).
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return values from ::seashell_Unload and ::seashell_Fin
 */
retcode_t
octcxt_GetSeaShell(octcxt_handle_t in_handle, mbinstr_t **out_seashell);

#if defined(NPOPT_ENABLE_DUS)
/**
 * Set the license suspension list
 *
 * @param[in,out] in_handle handle to the object context
 * @param[in]     in_sl     serialized suspension list
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 */
retcode_t
octcxt_SetSuspensionList(octcxt_handle_t io_handle, binstr_t *in_sl);
#endif

#if defined(NS_DEBUG)

extern void
octcxt_Print(const octcxt_handle_t in_handle);

#else

#define octcxt_Print(o)

#endif

NP_END_EXTERN_C

#endif /* OCTCXT_API_H */
/** @} */
