/** @addtogroup pkt pkt
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef PKT_API_H
#define PKT_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Plankton VM engine
 *
 * @specref Plankton Virtual Machine
 * This contains functions to load Plankton Code Module,Add to the VM engine
 * and executes the Plankton bytes code.It also provides functions to read
 * Data Segment part,Calculate Stack size , push/pop data into/from the stack.
 *
 * dependency base
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 * @date   1 Dec, 2004 last modified by n-goto
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

#define ERR_PKT_END                     (ERR_PKT_BASE|1)
#define ERR_PKT_TOOSMALLDSSIZE          (ERR_PKT_BASE|2)
#define ERR_PKT_LOADFAILED              (ERR_PKT_BASE|3)
#define ERR_PKT_ENTRYNOTFOUND           (ERR_PKT_BASE|4)
#define ERR_PKT_OUTOFDSBUFFER           (ERR_PKT_BASE|5)
#define ERR_PKT_STACKUNDERFLOW          (ERR_PKT_BASE|6)
#define ERR_PKT_STACKOVERFLOW           (ERR_PKT_BASE|7)
#define ERR_PKT_CALLSTACKOVERFLOW       (ERR_PKT_BASE|8)
#define ERR_PKT_DIVIDEBYZERO            (ERR_PKT_BASE|9)
#define ERR_PKT_UNKNOWNINSTRUCTION      (ERR_PKT_BASE|10)
#define ERR_PKT_UNKNOWNSYSTEMCALL       (ERR_PKT_BASE|11)
#define ERR_PKT_UNKNOWNVMHANDLE         (ERR_PKT_BASE|12)
#define ERR_PKT_ACCESS_VIOLATION        (ERR_PKT_BASE|13)
#define ERR_PKT_MAXIMUMCODEMODULE       (ERR_PKT_BASE|14)
#define ERR_PKT_UNLOADFAILED            (ERR_PKT_BASE|15)
#define ERR_PKT_UNHANDLEDPKTERROR       (ERR_PKT_BASE|16)
#define ERR_PKT_CODEMODULEALREADYLOADED (ERR_PKT_BASE|17) /* Error code that was introduced to signal that
* only one CM per VM is allowed, as explained in Nautilus_2.1 #217 */

#define PKT_CONTEXT_OBJTYPE_CONTAINER      0
#define PKT_CONTEXT_OBJTYPE_INTEGER        1
#define PKT_CONTEXT_OBJTYPE_STRING         2
#define PKT_CONTEXT_OBJTYPE_BINARY         3

#define PKT_CONTEXT_ERR_SUCCESS              0
#define PKT_CONTEXT_ERR_FAILURE             -1
#define PKT_CONTEXT_ERR_INTERNAL            -2
#define PKT_CONTEXT_ERR_INVALID_PARAMETER   -3
#define PKT_CONTEXT_ERR_OUT_OF_MEMORY       -4
#define PKT_CONTEXT_ERR_OUT_OF_RESOURCES    -5
#define PKT_CONTEXT_ERR_NO_SUCH_ITEM        -6
#define PKT_CONTEXT_ERR_INSUFFICIENT_SPACE  -7
#define PKT_CONTEXT_ERR_PERMISSION_DENIED   -8
#define PKT_CONTEXT_ERR_RUNTIME_EXCEPTION   -9
#define PKT_CONTEXT_ERR_INVALID_FORMAT      -10

#define PKT_RESULT_CODE_IS_SUCCESS(err)    ((err) == PKT_CONTEXT_ERR_SUCCESS)
#define PKT_RESULT_CODE_IS_ERROR(err)      ((err) != PKT_CONTEXT_ERR_SUCCESS)
#define PKT_RESULT_OUT_OF_MEMORY(err)      ((err) == PKT_CONTEXT_ERR_OUT_OF_MEMORY)
#define PKT_STATUS_OK               0
#define PKT_STATUS_ACTION_DENIED    1

/*---------------------------------------------------------------------------*
 * Type definitions
 *---------------------------------------------------------------------------*/

#define PKT_TRUSTED_TIME_FLAG_NONE        0x00
#define PKT_TRUSTED_TIME_FLAG_ESTIMATE    0x01
#define PKT_INIT_SIZE                     32768
#define PKT_CONTROLPROTOCOL               "http://www.octopus-drm.com/specs/scp-1_0"
#define PKT_CONTROLTYPE                   "http://www.octopus-drm.com/specs/pkcm-1_0"

#define PKT_IMPORT_TRANSFERABLE_ATTR_COUNT             6
#define PKT_IMPORT_TRANSFERABLE_NOFRESHNESS_ATTR_COUNT 5
#define PKT_IMPORT_TRANSFERABLE_AGENT_ATTR_COUNT       5

/* @note These are hard coded values based on the Plankton Generator tool 1.8.0.33 */
#define PKT_IMPORT_AGENT_CONTROL_ENTRY_POINT           "TransferMove"
#define PKT_IMPORT_AGENT_CONTROL_CONTEXT_ID            "contextId"


/**
 * @brief Plankton code identifier
 *
 * Type to select plankton code template
 **/
typedef enum {
    /* Template -> PKT_ID_IMPORT_TRANSFERABLE -> Constant Names (In Order) and Data Type
     * "playGrantedParameterBlock"  , Byte Array
       "playGrantedLocalFlags"      , Integer
       "licenseContainer"           , String
       "agentControlUid"            , String
       "InstanceId"                 , Integer
       "ProximityRequiredFreshness" , Integer
    */
    PKT_ID_IMPORT_TRANSFERABLE             = 0,  /**< Code module with transferable license, and with support for PLAY and TRANSFER */
    /* Template -> PKT_ID_IMPORT_TRANSFERABLE_NOFRESHNESS -> Constant Names (In Order) Data and Type
     * "playGrantedParameterBlock"  , Byte Array
       "playGrantedLocalFlags"      , Integer
       "licenseContainer"           , String
       "agentControlUid"            , String
       "InstanceId"                 , Integer
    */
    PKT_ID_IMPORT_TRANSFERABLE_NOFRESHNESS = 1,  /**< Code module for transferable license, and with support for PLAY and TRANSFER, no requiredFreshness */
    /* Template -> PKT_ID_IMPORT_TRANSFERABLE_AGENT -> Constant Names (In Order) and Data Type
       "licenseContainer"           , String
       "delegate1Uid"               , String
       "delegate1EntryPoint"        , String
       "delegate2Uid"               , String
       "delegate2EntryPoint"        , String
    */
    PKT_ID_IMPORT_TRANSFERABLE_AGENT       = 2   /**< Code module with transferable license with Agent, and with support for PLAY and TRANSFER */
} pkt_code_id_t;

/**
 * @brief Constant/Attribute data type
 *
 * Selects the type of constant/attribute used by Code templates
 **/
typedef enum {
    PKT_DATA_TYPE_INTEGER    = 0,  /**< Unsigned Integer data type */
    PKT_DATA_TYPE_STRING     = 1,  /**< String data type */
    PKT_DATA_TYPE_BYTE_ARRAY = 2,  /**< Byte Array data type */
} pkt_data_type_t;


/**
 * @brief  Constant used to supply control attribute value
 */
typedef struct pkt_constant {
    pkt_data_type_t     type;       /**< Type of the constant */
    union {
        u_int32_t       integer;    /**< Integer constant */
        binstr_t       *string;     /**< String constant, without NULL-termination */
        binstr_t       *byte_array; /**< Byte array constant, in binary form (not base64) */
    } value;
} pkt_constant_t;

/**
 * @brief Description of the code module to be returned from the Plankton code generator
 *
 */
typedef struct pkt_descriptor {
    pkt_code_id_t    code_template_id; /**< ID of a selected code module */
    np_size_t        num_constants;    /**< Number of constants for use when
                                      *   generating the Plankton module */
    pkt_constant_t  *constants;        /**< Array of constants modifying the template.
                                      *   At code generation time, these are meant to be placed
                                      *   as attributes in the license's control, or
                                      *   hard-coded in the Plankton code module itself.
                                      *   Eventually, they will be available for Plankton
                                      *   execution when evaluating the license */
} pkt_descriptor_t;

/**
 * @brief Description of an attribute associated with the generated code module
 */
typedef struct pkt_attribute {
    pkt_data_type_t       attribute_type;   /**< Type of the attribute */
    binstr_t             *attribute_name;   /**< Attribute name */
    union {
        u_int32_t         integer_value;    /**< Value of integer type */
        binstr_t         *string_value;     /**< Value of string type */
        binstr_t         *byte_array_value; /**< Value of byte array type */
    } value;
} pkt_attribute_t;

/**
 * @brief Container for the attributes associated with a generated code module
 */
typedef struct pkt_attributes {
    np_size_t          num_attributes;  /**< Number of attributes */
    pkt_attribute_t  **attributes;      /**< Array of attributes */
} pkt_attributes_t;


/**
 * Plankton VM Host Context
 */
typedef struct pkt_hostcontext_tag {
    void            *handle;

    /*
     * base
     */

    /**
     * Allows a program to access objects provided by the VM's Host
     *
     * @note called by System.Host.GetObject
     *
     * @param[in]  in_hostcontext context
     * @param[in]  in_parent      32-bit handle of the parent container
     * @param[in]  in_name        address of a null-terminated string containing the path to the requested object, relative to the parent container
     * @param[in]  in_buffer      address of a memory buffer where the value of the object is to be stored
     * @param[in]  in_buffersize  size in bytes of the memory buffer where the value of the object is to be stored
     * @param[in]  in_ownerid     owner identifier (subject of tbs certificate of the octopus control object)
     * @param[in]  in_attrs       AttributeList in Control
     * @param[in]  in_owneridsize size of owner identifier
     * @param[out] out_typeid     object type id, or negative error code if the call failed
     * @param[out] out_size       size in bytes of the data returned in the buffer supplied by the caller, or the size required if the caller provided a buffer that was too small
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NG
     * @return ::ERR_INTERNALERROR
     * @return ::seashell_GetObject
     * @return ::l_GetContainerObject
     * @return ::l_SetString
     * @return ::l_SetInteger
     * @return ::l_SetBinary
     * @return ::getobjectbynum
     * @return ::getobject
     */
    retcode_t (*GetObject)(
        struct pkt_hostcontext_tag *in_hostcontext,
        int32_t                     in_parent,
        u_int8_t                   *in_name,
        u_int8_t                   *in_buffer,
        u_int32_t                   in_buffersize,
        u_int8_t                   *in_ownerid,
        u_int32_t                   in_owneridsize,
        void                       *in_attrs,
        int32_t                    *out_typeid,
        int32_t                    *out_size);

    /**
     * Allows a program to create, write and destroy objects provided by the VM's Host
     *
     * @note called by System.Host.SetObject
     *
     * @param[in]  in_hostcontext context
     * @param[in]  in_parent      32-bit handle of the parent container
     * @param[in]  in_name        address of a null-terminated string containing the path to the object, relative to the parent container
     * @param[in]  in_buffer      address of a memory buffer where the value of the object is stored
     * @param[in]  in_buffersize  size in bytes of the memory buffer where the value of the object is stored
     * @param[in]  in_ownerid     owner identifier (subject of tbs certificate of the octopus control object)
     * @param[in]  in_owneridsize size of owner identifier
     * @param[in]  in_typeid      the type Id of the object
     * @param[out] out_reuslt     result code, 0 if the call succeeded, or negative error code if the call failed
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_INTERNALERROR
     * @return ::ERR_NG
     * @return ::seashell_SetObject
     */
    retcode_t (*SetObject)(
        struct pkt_hostcontext_tag *in_hostcontext,
        int32_t                     in_parent,
        u_int8_t                   *in_name,
        u_int8_t                   *in_buffer,
        u_int32_t                   in_buffersize,
        u_int8_t                   *in_ownerid,
        u_int32_t                   in_owneridsize,
        int32_t                     in_typeid,
        int32_t                    *out_reuslt);

    /**
     * Get the trusted time
     *
     * @note called by System.Host.GetTrustedTime
     * @note Only relevant on systems that implement a trusted clock that can be synchronized
     *            with a trusted time source and maintain a monotonic time counter.
     *
     * @param[in]  in_hostcontext context
     * @param[out] out_time       trusted time, number of minutes elapsed since January 1, 1970 00:00:00 UTC, or a negative error code
     * @param[out] out_flag       ::PKT_TRUSTED_TIME_FLAG_NONE or ::PKT_TRUSTED_TIME_FLAG_ESTIMATE
     * @return ::ERR_OK
     * @return ::ERR_TMCXT_TTNOTAVAILABLE
     */
    void (*GetTrustedTime)(
        struct pkt_hostcontext_tag *in_hostcontext,
        u_int32_t                  *out_time,
        u_int32_t                  *out_flag);

    /*
     * Get the local time (deprecated)
     *
     * @deprecated
     * @called by System.Host.GetLocalTime
     *
     * @param[in]  in_hostcontext context
     * @param[out] out_time       local time, number of minutes elapsed since January 1, 1970 00:00:00, or a negative error code
     * @return ::ERR_OK
     */
    void (*GetLocalTime)(
        struct pkt_hostcontext_tag *in_hostcontext,
        u_int32_t                  *out_time);

    /**
     * Get the local time offset (deprecated)
     *
     * @deprecated
     * @called by System.Host.GetLocalTimeOffset
     *
     * @param[in]  in_hostcontext context
     * @param[out] out_offset     local time offset, number of minutes difference between local time and UTC time (i.e. LocalTime - UTC)
     * @return ::ERR_OK
     */
    void (*GetLocalTimeOffset)(
        struct pkt_hostcontext_tag *in_hostcontext,
        int32_t                    *out_offset);

    /**
     * Get the local trusted time and lag between trusted and local
     *
     * @note called by System.Host.GetLocalTrustedTime
     * @note This is not a standard system call
     *
     * @param[in]  in_hostcontext context
     * @param[out] out_time       local trusted time, number of minutes elapsed since January 1, 1970 00:00:00, or a negative error code
     * @param[out] out_flag       lag between local time and trusted time (LocalTime - LocalTrustedTime)
     * @return ::ERR_OK
     * @return ::ERR_TMCXT_TTNOTAVAILABLE
     */
    void (*GetLocalTrustedTime)(
        struct pkt_hostcontext_tag *in_hostcontext,
        u_int32_t                  *out_time,
        int32_t                    *out_flag);

    /**
     * Is Node Reachable
     *
     * @note called by Octopus.Links.IsNodeReachable
     * @note does not return an ExtendedStatusBlock on the VM stack
     *
     * @param[in]  in_hostcontext context
     * @param[in]  in_toidsize    length of the string ::in_toid
     * @param[in]  in_toid        address of a (null-terminated) string containing the ID of the target link to be tested for reachability
     * @param[out] out_result     the result value is 0 if the node is reachable, or a negative error code if it is not
     * @return ::ERR_OK
     */
    retcode_t (*IsNodeReachable)(
        struct pkt_hostcontext_tag *in_hostcontext,
        np_size_t                   in_toidsize,
        u_int8_t                   *in_toid,
        int32_t                    *out_result);

} pkt_hostcontext_t;

/** Handle of Plankton VM */
typedef void *pkt_handle_t;

/*---------------------------------------------------------------------------*/
/**
 * Initialize a Plankton VM from a given Host context
 *
 * @param[in]  in_dssize  Data Segment (DS) size
 * @param[in]  in_context Plankton VM Host context
 * @param[out] out_handle handle of the created VM
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_PKT_TOOSMALLDSSIZE
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_Init(int32_t in_dssize, pkt_hostcontext_t *in_context, pkt_handle_t *out_handle);

/*---------------------------------------------------------------------------*/
/**
 *
 * Prints informations related to a VM
 * (data section size, code section size, heap/stack usage...)
 *
 * @todo Debugging Only?
 *
 * @param[in] in_handle handle of the VM from which informations are requested
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_Stat(pkt_handle_t in_handle);

/*---------------------------------------------------------------------------*/
/**
 * Destroys a Plakton VM
 *
 * @note calls the optional Global.OnUnload prior to destroying the VM
 *
 * @param[in,out] io_handle handle of the VM to destroy
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_Fin(pkt_handle_t io_handle);

/*---------------------------------------------------------------------------*/
/**
 * Plankton VM Module Loader
 * Loads pkCM (pkDS, pkCS, pkEX, pkRQ)
 *
 * @note calls the optional Global.OnLoad
 *
 * @param[in,out] io_handle       handle of the Plankton VM
 * @param[in]     in_codemoduleid id of the code module to load
 * @return ::ERR_OK
 * @return ::ERR_PKT_LOADFAILED
 * @return ::ERR_PKT_OUTOFDSBUFFER
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_LoadCodeModule(pkt_handle_t  io_handle,
                   binstr_t     *in_codemoduleid);

/*---------------------------------------------------------------------------*/
/**
 * Adds a code module to a Plankton VM
 *
 * @param[in,out] io_handle       handle of the Plankton VM
 * @param[in]     in_codemodule   Code Module (pkCM)
 * @param[in]     in_codemoduleid id of the Code Module in the VM
 * @param[in]     in_protocolid   Must be "http://www.octopus-drm.com/specs/scp-1_0"
 * @param[in]     in_bctypeid     Must be "http://www.octopus-drm.com/specs/pkcm-1_0"
 * @param[in]     in_ownerid      owner identifier (subject of tbs certificate of the octopus control object)
 * @param[in]     in_attrs        AttributeList
 * @return ::ERR_OK
 * @return ::ERR_PKT_LOADFAILED
 * @return ::ERR_PKT_MAXIMUMCODEMODULE
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_SetCodeModule(pkt_handle_t  io_handle,
                  binstr_t     *in_codemodule,
                  binstr_t     *in_codemoduleid,
                  binstr_t     *in_protocolid,
                  binstr_t     *in_bctypeid,
                  binstr_t     *in_ownerid,
                  void         *in_attrs);

/*---------------------------------------------------------------------------*/
/**
 * Executes the Plankton byte code associated to an entry of the export table of the code module
 *
 * @note result code is push on the VM stack
 *
 * @param[in,out] io_handle handle of the Plankton VM
 * @param[in]     in_entryname null-terminated string that specifies the name of the entry point to call
 * @return ::ERR_OK
 * @return ::ERR_PKT_ENTRYNOTFOUND
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_Exec(pkt_handle_t io_handle, const char *in_entryname);

/*---------------------------------------------------------------------------*/
/**
 * Reads a part of the Data Segment (DS)
 *
 * @param[in]  in_handle handle of the Plankton VM
 * @param[in]  in_offset offset relative to the beginning of the the DS
 * @param[in]  in_size   number of bytes to read from the DS+in_offset
 * @param[out] out_buf   output buffer in which the part of the DS is copied to
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_ReadDS(pkt_handle_t in_handle, int32_t in_offset, int32_t in_size, void *out_buf);

/*---------------------------------------------------------------------------*/
/**
 * Computes the stack size (used + free space), making sure reserved data space is not calculated.
 * The stack size is the number of words the stack can contain, not the number of octets.
 *
 * @param[in]  in_handle       handle of the Plankton VM
 * @param[in]  out_stack_size  total size of stack (1 unit is 4 octets)
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_StackSize(pkt_handle_t in_handle, int32_t *out_stack_size);

/*---------------------------------------------------------------------------*/
/**
 * Pushes a 32-bit value to the Plankton VM Stack
 *
 * @param[in,out] io_handle handle of the Plankton VM
 * @param[in]     in_val    value to be pushed to the stack
 * @return ::ERR_OK
 * @return ::ERR_PKT_STACKOVERFLOW
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_PushStack(pkt_handle_t io_handle, int32_t in_val);

/*---------------------------------------------------------------------------*/
/**
 * @brief Push a data array to the Plankton VM Stack. If the size is
 * not a multiple of 4, the data on the stack will be padded with
 * 0-value bytes to ensure that the stack pointer remains a multiple
 * of 4.
 *
 * @param[in,out] io_handle       handle of the Plankton VM
 * @param[in]     in_array_size   size of data array to be pushed to the stack
 * @param[in]     in_array        data array to be pushed to the stack
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_PKT_STACKOVERFLOW
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_PushArrayStack(pkt_handle_t    io_handle,
                   u_int32_t       in_array_size,
                   u_int8_t        *in_array);

/*---------------------------------------------------------------------------*/
/**
 * @brief Pop a 32-bit value from the Plankton VM Stack.
 *
 * @param[in,out] io_handle handle of the Plankton VM
 * @param[out]    out_val   value that has been popped from the stack
 *
 * @return ::ERR_OK
 * @return ::ERR_PKT_STACKUNDERFLOW
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_PopStack(pkt_handle_t io_handle, int32_t *out_val);

/*---------------------------------------------------------------------------*/
/**
 * Clear the stack of the Plankton VM
 *
 * @todo only used in tests (should not this be removed from release builds?)
 *
 * @param[in,out] io_handle handle of the Plankton VM
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_ClearStack(pkt_handle_t io_handle);

/*---------------------------------------------------------------------------*/
/**
 * Reads an ExtendedStatusBlock in the Plankton VM Data Segment
 *
 * @note the ExtendedStatusBlock is checked for memory overruns
 * @specref Plankton Virtual Machine, Standard Extended Status (ExtendedStatusBlock)
 *
 * @param[in,out] io_handle  handle of the Plankton VM
 * @param[in]     in_offset  offset (from Data Segment) typically given by ::pkt_PopStack
 * @param[out]    out_esb    the ExtendedStatusBlock
 * @param[out]    out_status ::PKT_STATUS_ACTION_DENIED or ::PKT_STATUS_OK
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_GenExStatusAddr(pkt_handle_t io_handle,
                    int32_t      in_offset,
                    binstr_t   **out_esb,
                    u_int32_t   *out_status);

/*---------------------------------------------------------------------------*/
/**
 * Generates a Plankton code module and its Control Attributes
 *
 * @param[in]     in_descriptor      Description of the code to use, and its
 *                                   constants
 * @param[out]    out_code_module    Code module selected (or generated)
 * @param[out]    out_attributes     Array of attribute values
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
pkt_GetCodeModule(pkt_descriptor_t   *in_descriptor,
                  binstr_t          **out_code_module,
                  pkt_attributes_t  **out_attributes);

/**
 * Free the pkt attributes
 *
 * @param[in]     in_attributes      Container of attributes
 *
 */
/*---------------------------------------------------------------------------*/
extern void
pkt_FreeAttributes(pkt_attributes_t  *in_attributes);

/**
 * @brief Return the context ID for transfer action
 */
extern retcode_t
pkt_GetTransferContextId(binstr_t **out_context_id);

/**
 * @brief Return the agent entry point for transfer action
 */
extern retcode_t
pkt_GetTransferAgentEntryPoint(binstr_t **out_agent_entry_point);

NP_END_EXTERN_C

#endif /* PKT_API_H */
/** @} */
