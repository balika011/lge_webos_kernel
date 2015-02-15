/** @addtogroup drm drm
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef DRM_API_H
#define DRM_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Marlin DRM Implementation
 *
 * dependency: base,octobj,octcxt,pkt,keymng
 *
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 * @date   17 Dec, 2004 last modified by n-goto
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

#define ERR_DRM_ILLEGALBYTECODE         (ERR_DRM_BASE|1)
#define ERR_DRM_ILLEGALPERSONALITY      (ERR_DRM_BASE|2)
#define ERR_DRM_ILLEGALLINK             (ERR_DRM_BASE|3)
#define ERR_DRM_ILLEGALLICENSE          (ERR_DRM_BASE|4)
#define ERR_DRM_NOPERSONALITYSPECIFIED  (ERR_DRM_BASE|5)
#define ERR_DRM_PLAYNOTGRANTED          (ERR_DRM_BASE|6)
#define ERR_DRM_NOKEYREGISTERED         (ERR_DRM_BASE|7)
#define ERR_DRM_CANNOTDECRYPT           (ERR_DRM_BASE|8)
#define ERR_DRM_SIGNATUREFAILURE        (ERR_DRM_BASE|9)
#define ERR_DRM_DESCRIBEERROR           (ERR_DRM_BASE|10)
#define ERR_DRM_PERFORMERROR            (ERR_DRM_BASE|11)
#define ERR_DRM_NODESCRIPTION           (ERR_DRM_BASE|12)
#define ERR_DRM_NOERRORINFO             (ERR_DRM_BASE|13)
#define ERR_DRM_ILLEGALASSERTION        (ERR_DRM_BASE|14)
#define ERR_DRM_FAILUREINPKT            (ERR_DRM_BASE|15)
#define ERR_DRM_INITERROR               (ERR_DRM_BASE|16)
#define ERR_DRM_RELEASEERROR            (ERR_DRM_BASE|17)

/* Callback id */
#define DRM_CALLBACK_ID_RESET                 (0)
#define DRM_CALLBACK_ID_CONTINUE              (1)

/*---------------------------------------------------------------------------*
 * action-type definitions
 *---------------------------------------------------------------------------*/

enum {
    DRM_ACTIONTYPE_PLAY = 0,
    DRM_ACTIONTYPE_WRITETOMEDIUM,
    DRM_ACTIONTYPE_TRANSFER,
    DRM_ACTIONTYPE_EXPORT,
    DRM_ACTIONTYPE_DISCARD,
    DRM_ACTIONTYPE_MAX
};

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

typedef void *drm_handle_t;

typedef struct {
    int32_t   id;
    binstr_t  *entrypoint;
    int32_t   cookie;
} drm_callback_t;


/*---------------------------------------------------------------------------*/
/**
 * Initialize DRM
 *
 * This should be the first call before the DRM compoment is used
 *
 * @param[out] out_handle buffer where DRM handle is stored
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 */
extern retcode_t
drm_Init(drm_handle_t *out_handle);

/*---------------------------------------------------------------------------*/
/**
 * Finalize DRM
 *
 * This should be the last call to release the DRM component
 *
 * @param[in,out] io_handle DRM handle
 */
extern void
drm_Fin(drm_handle_t io_handle);

/*---------------------------------------------------------------------------*/
/**
 * Return the keymng associated with a DRM context
 *
 * @param[in]     in_handle   DRM handle
 * @param[out]    out_keymng  Keymng associated with the DRM context
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
drm_KeymngContextGet(drm_handle_t in_handle, keymng_handle_t *out_keymng);

/*---------------------------------------------------------------------------*/
/**
 * Set DRM personality data
 *
 * Setting a personality just means adding all the keys associated with
 * the personality to the \ref keymng module and setting the personality
 * in the \ref octcxt module.
 *
 * @param[in,out] io_handle      DRM handle
 * @param[in] in_personality personality to be set
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_ILLEGALPERSONALITY
 * @return returns from ::keymng_Init, ::octcxt_Init and ::octcxt_SetPersonality
 */
extern retcode_t
drm_SetPersonality(drm_handle_t io_handle, octobj_node_t *in_personality);

/*---------------------------------------------------------------------------*/
/**
 * Set DRM link data
 *
 * Setting links in the DRM just means adding all the scuba keys associated
 * with the links to the \ref keymng module and setting the link pointer in
 * the DRM context
 *
 * @param[in,out] io_handle  Handle to the DRM context
 * @param[in]     in_links   links to be set
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_ILLEGALLINK
 * @return ::ERR_NOMEM
 * @return returns from ::keymng_Init, ::octcxt_Init and ::octcxt_SetLinks
 */
extern retcode_t
drm_SetLinks(drm_handle_t io_handle, octobj_vec_t *in_links);

/*---------------------------------------------------------------------------*/
/**
 * Set DRM Assertion
 *
 * Setting assertions delegates to the octopus context call as well as
 * setting the assertion pointer in the DRM context
 *
 * @param[in,out] io_handle DRM handle
 * @param[in] in_assertion  assertion to be set
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_ILLEGALASSERTION
 * @return returns from ::octcxt_Init and ::octcxt_SetAssertion
 */
extern retcode_t
drm_SetAssertion(drm_handle_t io_handle, octobj_assertion_t *in_assertion);

/*---------------------------------------------------------------------------*/
/**
 * Set DRM SeaShell data
 *
 * Setting SeaShell info delegates to the octopus context via ::octcxt_SetSeaShell
 *
 * @param[in,out] io_handle    DRM handle
 * @param[in]     in_seashell  seashell data to be set
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return returns from ::octcxt_Init and ::octcxt_SetSeaShell
 */
extern retcode_t
drm_SetSeaShell(drm_handle_t io_handle, mbinstr_t *in_seashell);

/*---------------------------------------------------------------------------*/
/**
 * Get DRM SeaShell data
 *
 * Getting SeaShell info delegates from the octopus context via ::octcxt_GetSeaShell
 *
 * @param[in,out] io_handle     DRM handle
 * @param[out]    out_seashell  buffer where the seashell is stored
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return returns from ::octcxt_Init and ::octcxt_SetSeaShell
 */
extern retcode_t
drm_GetSeaShell(drm_handle_t io_handle, mbinstr_t **out_seashell);

#if defined(NPOPT_ENABLE_DUS)
/*---------------------------------------------------------------------------*/
/**
 * Set DRM License suspendion list data
 *
 * @param[in,out] io_handle DRM context handle
 * @param[in]     in_sl     Suspension list to be set
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return see ::octcxt_SetSuspensionList
 */
extern retcode_t
drm_SetSuspensionList(drm_handle_t io_handle, binstr_t *in_sl);
#endif

/*---------------------------------------------------------------------------*/
/**
 * Set license data into the DRM handle, create license handle,
 * and set the license handle to the DRM handle.
 *
 * Before calling this, personality should be set. This function should be
 * called before calling licence controlled APIs.
 *
 * @param[in,out] io_handle   DRM handle
 * @param[in] in_license      license to be set
 * @param[in] in_ncontentids  number of content-ids to be rendered
 * @param[in] in_contentids   content-ids to be rendered
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_NOPERSONALITYSPECIFIED
 * @return ::ERR_DRM_ILLEGALLICENSE
 * @return ::ERR_DRM_ILLEGALBYTECODE
 * @return ::ERR_NOMEM
 * @return returns from ::octcxt_Init, ::octcxt_SetLicense, ::pkt_Init, ::pkt_SetCodeModule,
 *         ::pkt_LoadCodeModule and ::pkt_Exec
 */
extern retcode_t
drm_LicenseInit(drm_handle_t io_handle, octobj_license_t *in_license);

/*---------------------------------------------------------------------------*/
/**
 * Register a key, associated with some content, in the \ref keymng module
 *
 * @param[in,out]  io_handle  DRM handle
 * @param[in]      in_cid     Content object ID
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return return value from ::keymng_Init
 */
extern retcode_t
drm_EnableCKfromCID(drm_handle_t io_handle, binstr_t *in_cid);

/*---------------------------------------------------------------------------*/
/**
 * Finalize DRM license handle
 *
 * This function will release all running \ref pkt "plankton" code
 * @param[in,out] io_handle DRM handle
 */
extern void
drm_LicenseFin(drm_handle_t io_handle);

/*---------------------------------------------------------------------------*/
/**
 * Check whether the given \ref pkt "plankton" action is granted or not
 *
 * This function delegates to the \ref pkt "plankton" subsystem to check
 * the actions and optionally initialize the \ref pkt "plankton" code
 *
 * @param[in]     in_handle     DRM handle
 * @param[in]     in_actiontype action type (Play, WriteToMedium, Transfer or Export)
 * @param[in]     in_parameters Parameters to pass to code module. Can be NULL.
 * @param[out]    out_esb       Result of a check action as an extended status block.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_PLAYNOTGRANTED
 * @return ::ERR_DRM_PERFORMERROR
 * @return returns from ::pkt_Exec, ::pkt_PopStack and ::pkt_GenExStatusAddr
 */
extern retcode_t
drm_ActionCheck(drm_handle_t      in_handle,
                int32_t           in_actiontype,
                octobj_params_t  *in_parameters,
                binstr_t        **out_esb);

/*---------------------------------------------------------------------------*/
/**
 * Perform a given action (Play or WriteToMedium)
 *
 * If the action is granted, decrypt content-keys and set them into the DRM handle
 *
 * @param[in,out] io_handle     DRM handle
 * @param[in]     in_actiontype action type (Play, WriteToMedium, Transfer or Export)
 * @param[in]     in_parameters Parameters to pass to code module. Can be NULL.
 * @param[out]    out_ckeys     Decrypted Content-keys
 * @param[out]    out_esb       Result of a check action as an extended status block.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_NOKEYREGISTERED
 * @return ::ERR_DRM_PLAYNOTGRANTED
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_DRM_PERFORMERROR
 * @return returns from ::octcxt_Init, ::pkt_Exec, ::pkt_PopStack, ::pkt_GenExStatusAddr,
 *         ::keymng_GetKey and ::sf_GetKey
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
drm_ActionPerform(drm_handle_t         io_handle,
                  int32_t              in_actiontype,
                  octobj_params_t      *in_parameters,
                  octobj_contentkey_t **out_ckeys,
                  binstr_t            **out_esb);


/*---------------------------------------------------------------------------*/
/**
 * @brief Run the routine within in_callback with the plankton VM.
 *
 * @param[in, out] io_handle      DRM handle
 * @param[in]      in_callback    callback to use
 * @param[in]      in_data_len    length of in_data
 * @param[in]      in_data        data to be used with callback routine
 * @param[out]     out_esb        pointer to the returned esb
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_DRM_NOKEYREGISTERED
 * @return ::ERR_DRM_PLAYNOTGRANTED
 * @return ::ERR_DRM_PERFORMERROR
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
drm_Callback(drm_handle_t   io_handle,
             drm_callback_t *in_callback,
             u_int32_t      in_data_len,
             u_int8_t       *in_data,
             binstr_t       **out_esb);

/*---------------------------------------------------------------------------*/
/**
 * Remove the given content-keys from \ref keymng "key manager" subsystem
 *
 * @param[in,out] io_handle DRM handle
 * @param[in]     in_nkeys  the number of content keys
 * @param[in]     in_ckeys  buffer of content keys
 * @return ::ERR_OK
 */
extern retcode_t
drm_DeregContentKeys(drm_handle_t io_handle, int32_t in_nkeys, octobj_contentkey_t **in_keys);

/**
 * Clear the invalid attribute of all the content keys
 *
 * @param[in] in_handle  DRM handle
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::keymng_ClearAttr
 */
extern retcode_t
drm_EnableContentKeys(drm_handle_t in_handle);

/*---------------------------------------------------------------------------*/
/**
 * @brief Get the description of the license
 *
 * @param[in,out] io_handle DRM handle
 * @param[in]     in_actiontype action type (Play, WriteToMedium, Transfer or Export)
 * @param[in]     in_parameters Parameters to pass to code module. Can be NULL.
 * @param[out]    out_esb       Result of a check action as an extended status block.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_ILLEGALBYTECODE
 * @return ::ERR_NOMEM
 */
extern retcode_t
drm_ActionDescribe(drm_handle_t      io_handle,
                   int32_t           in_actiontype,
                   octobj_params_t  *in_parameters,
                   binstr_t        **out_esb);

/*---------------------------------------------------------------------------*/
/**
 * Process a link constraint description query and format the result as an \ref esbman "Extended Status Block"
 *
 * @param[in,out] io_handle  DRM handle
 * @param[in]     in_control    control of link to describe
 * @param[out]    out_esb   returned esb
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_DRM_NODESCRIPTION
 * @return ::ERR_DRM_DESCRIBEERROR
 * @return returns from ::pkt_Init, ::pkt_SetCodeModule,
 *         ::pkt_LoadCodeModule, ::pkt_Exec, ::pkt_PopStack, ::pkt_GenExStatusAddr and
 *         ::pkt_Fin
 */
extern retcode_t
drm_LinkConstraintDescribe(drm_handle_t      io_handle,
                           octobj_control_t *in_control,
                           binstr_t        **out_esbs);

/*---------------------------------------------------------------------------*/
/**
 * Initialize, Run and Relase a control agent.
 *
 * This function will execute a conrtol agent in \ref pkt "plankton"
 * and return the resultant \ref pkt "plankton" code and \ref seashell "SeaShell"
 * database.
 *
 * @param[in]  in_octrlnum    size of \ref pkt "plankton" bytecode module
 * @param[in]  in_octrl       \ref pkt "plankton" bytecode module
 * @param[in]  in_entry       entry function name
 * @param[in]  in_contextid   \ref pkt "plankton" context id
 * @param[in]  in_parameters  \ref pkt "plankton" parameter block
 * @param[in]  in_seashell    \ref seashell "SeaShell" serialized database
 * @param[in]  in_param_name  Parameter from the agent
 * @param[in]  in_param_value Parameter value from the agent
 * @param[out] out_retcode    result code from the execution of the bytecode
 * @param[out] out_retbody    Data segment from the \ref pkt "plankton" bytecode module
 * @param[out] out_seashell   Modified \ref seashell "SeaShell" serialized database
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_DRM_FAILUREINPKT
 * @return return values from :
 * @return ::octcxt_Init, ::octcxt_SetContextId, ::octcxt_SetSeaShell, ::octcxt_GetSeaShell
 * @return ::pkt_SetCodeModule, ::pkt_LoadCodeModule, ::pkt_Exec, ::pkt_PopStack, ::pkt_ReadDS
 */
/*---------------------------------------------------------------------------*/

retcode_t
drm_AgentEval(
    int32_t             in_octrlnum,
    octobj_control_t   *in_octrl[],
    binstr_t           *in_entry,
    binstr_t           *in_contextid,
    octobj_params_t    *in_parameters,
    mbinstr_t          *in_seashell,
    int32_t            *out_retcode,
    binstr_t          **out_retbody,
    mbinstr_t         **out_seashell);

/*---------------------------------------------------------------------------*/
/**
 * @brief Re-encrypt the internal contentkeys with a new public key and return
 *        them together with their secretkey ids and their contentkey ids.
 *
 * @note Each of the output arrays have the same size, which is out_nb_contentkeys.
 *
 * @param[in]  in_handle          DRM handle.
 * @param[in]  in_key             The key with which the contentkeys need to be re-encrypted.
 * @param[out] out_nb_contentkeys The number of contentkeys that were re-encrypted.
 * @param[out] out_contentkeys    The re-encrypted contentkeys
 * @param[out[ out_midkeys        Midkey if it was necessary. This array will not be null, but may contain nulls
 * @param[out] out_secretkeyids   The secretkey ids of the re-encrypted content keys.
 * @param[out] out_contentkeyids  The contentkey ids of the re-encrypted content keys.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_DRM_NOKEYREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
drm_ReencryptCK(drm_handle_t       in_handle,
                rsa_key_t         *in_key,
                u_int32_t         *out_nb_contentkeys,
                binstr_t        ***out_contentkeys,
                binstr_t        ***out_midkeys,
                binstr_t        ***out_secretkeyids,
                binstr_t        ***out_contentkeyids);

extern retcode_t
drm_MarkExportable(drm_handle_t in_handle);

NP_END_EXTERN_C

#endif /* DRM_API_H */
/** @} */
