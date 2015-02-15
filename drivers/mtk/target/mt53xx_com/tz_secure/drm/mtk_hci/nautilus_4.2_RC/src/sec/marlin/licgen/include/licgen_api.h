/** @addtogroup licgen licgen
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef LICGEN_API_H
#define LICGEN_API_H

NP_BEGIN_EXTERN_C


/*---------------------------------------------------------------------------*
 * Error code definitions
 *---------------------------------------------------------------------------*/

#define ERR_LICGEN_NO_CKS_FOUND         (ERR_LICGEN_BASE|1)  /**< No content keys found */
#define ERR_LICGEN_PERSONALITYERROR     (ERR_LICGEN_BASE|2)  /**< Import Personality missing or corrupted */
#define ERR_LICGEN_RUN_AGENT_FAILED     (ERR_LICGEN_BASE|3)  /**< Running LTP agent failed */

/* Flags for License generation */
#define LICGEN_SET_LICENSE_STATUS_PATH_FLAG (1<<0)  /**< Indicates to include delegates in the license */
#define LICGEN_RUN_AGENT_LOCAL_FLAG         (1<<1)  /**< Run the agent locally after license generation */

typedef u_int32_t licgen_flags_t;

/**
 * Type of binding to node
 *
 * Type to specify the node that the a generated license will be bound to
 **/
typedef enum {
    NP_IMPORT_BIND_PERS   = 0x0,         /**< Bind to the local personality */
    NP_IMPORT_BIND_NODE   = 0x1,         /**< Bind to the domain or user node in sdata */
    NP_IMPORT_BIND_DEVICE = 0x2          /**< Bind to a supplied device node */
} licgen_bind_t;


typedef struct {
    licgen_bind_t     type;        /**< Type of license binding */
    union {
        np_str_t     *id;          /**< Node id, selecting a node in sdata */
        np_str_t     *pub_key;     /**< Public key of device to bind the license to */
    } node;
} licgen_binding_t;


/**
 * @brief Generates a license (XML)
 *
 * @note: For the N4.0 & N4.1 release there is only support for binding to the
 *        personality node. However, the licgen_bind_t is added to allow for
 *        different nodes i.e. user nodes being used in future releases
 *
 * @param[in]     in_pkt_template            PKT Template ID
 * @param[in]     in_seashell                Current Seashell DB
 * @param[in]     in_keys                    Key ring, containing content keys
 * @param[in]     in_binding                 License binding info
 * @param[in]     in_bkb                     NULL, or the <BroadcastKeyBlock> string to insert in the license
 * @param[in]     in_esb_for_control_module  ESB Info that will be returned when executing the control module
 * @param[in]     in_licgen_flags            License generation flags
 * @param[in,out] out_license                License
 * @param[in,out] out_seashell               Updated seashell DB
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 * @return ::ERR_LICGEN_NO_CKS_FOUND
 * @return ::ERR_LICGEN_ILLEGALPERSONALITY
 * @return ::keymng_GetCKIterator
 * @return ::keymng_SetBkbProtectedKey
 * @return ::keymng_GetCKDetailsForLicenseGeneration
 * @return ::octobj_SetKeyUid
 * @return ::octobj_CreateContentKey
 * @return ::octobj_CreateProtector
 * @return ::octobj_CreateControl
 * @return ::octobj_CreateController
 * @return ::octobj_CreateLicense
 * @return ::oct2xml_CreateLicense
 */
extern retcode_t
licgen_GenerateLicense(pkt_code_id_t          in_pkt_template,
                       mbinstr_t             *in_seashell,
                       keymng_handle_t        io_keys,
                       licgen_binding_t      *in_binding,
                       binstr_t              *in_bkb,
                       esbman_info_t         *in_esb,
                       licgen_flags_t         in_licgen_flags,
                       binstr_t             **out_license,
                       mbinstr_t            **out_seashell);

/**
 * @brief Returns the License Issuing Service Key(LISK) subject.
 *
 * @param[out] out_lisk_subject   License Issuing Service Key(LISK) subject
 *
 * @return ::ERR_OK                          <br>Indicating success.
 * @return ::ERR_ILLEGALARGS                 <br>Illegal Argument.
 * @return ::ERR_NOMEM                       <br>Nautilus encountered resource problems.
 * @return ::ERR_INTERNALERROR               <br>An unrecoverable failure occurred in Nautilus.
 * @return ::ERR_ILLEGALHANDLE               <br>import context is is wrong state (e.g. license was already generated once)
 * @return ::ERR_LICGEN_PERSONALITYERROR     <br>Import agent personality is missing or wrong
 */
extern retcode_t
licgen_GetLISKSubject(binstr_t **out_lisk_subject);

NP_END_EXTERN_C

#endif /* LICGEN_API_H */
