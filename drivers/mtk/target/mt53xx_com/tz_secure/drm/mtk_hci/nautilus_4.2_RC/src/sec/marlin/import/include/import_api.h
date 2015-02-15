/** @addtogroup import import
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef IMPORT_API_H
#define IMPORT_API_H

NP_BEGIN_EXTERN_C


/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

#define ERR_IMPORT_INVALID_DESC         (ERR_IMPORT_BASE | 1)
#define ERR_IMPORT_NO_CKS_FOUND         (ERR_IMPORT_BASE | 2)
#define ERR_IMPORT_PERSONALITYERROR     (ERR_IMPORT_BASE | 3)
#define ERR_IMPORT_DEVICEIDTOOLONG      (ERR_IMPORT_BASE | 4)

/*===========================================================================*/
/** @file
 * @brief Component implementing the Marlin License Import specifications
 *
 */
/*===========================================================================*/

/*----------------------------------------------------------------------------*
 * defines
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 * type definitions
 *----------------------------------------------------------------------------*/

typedef void *import_handle_t;

/**
 * @brief Import Content types
 *
 * Type to specify the type of content to import
 **/
typedef enum {
    IMPORT_DVB         = 0x0,      /**< Content comes from DVB */
    IMPORT_DTCP        = 0x1,      /**< Content comes from DTCP */
    IMPORT_ARIB        = 0x2,      /**< Content comes from ARIB */
    IMPORT_OPENMG      = 0x3       /**< Content comes from OpenMG */
} import_content_type_t;

/**
 * @brief Type of binding node
 *
 * Type to specify the node that the a generated license will be bound to
 **/
typedef enum {
    IMPORT_BIND_PERS   = 0x0,    /**< Bind to the local personality */
    IMPORT_BIND_DEVICE = 0x1,    /**< Bind to a supplied device node */
    IMPORT_BIND_NODE   = 0x2     /**< Bind to the domain or user node */
} import_bind_t;

/**
 * @brief EPN types
 *
 * Type to specify the source EPN value used for the license generation
 **/
typedef enum {
    IMPORT_EPN_ASSERTED    = 0x0,  /**< EPN asserted */
    IMPORT_EPN_UNASSERTED  = 0x1   /**< EPN unasserted */
} import_epn_t;

/**
 * @brief DTCP CCI types
 *
 * Type to specify the source CCI value used for the license generation
 **/
typedef enum {
    IMPORT_CCI_COPY_FREE      = 0x0,  /**< Copy free */
    IMPORT_CCI_NO_MORE_COPIES = 0x1,  /**< No more copies */
    IMPORT_CCI_COPY_ONE_GEN   = 0x2   /**< Copy one generation */
} import_cci_t;


/**
 * @brief ICT types
 *
 * Type to specify the source ICT value used for the license generation
 **/
typedef enum {
    IMPORT_ICT_CONSTRAINED = 0x0,  /**< High def analog in constrained form */
    IMPORT_ICT_HIGH_DEF    = 0x1   /**< High def analog in high def form */
} import_ict_t;

/**
 * @brief CCT types
 *
 * Type to specify the source CCT value used for the license generation
 */
typedef enum {
    IMPORT_CCT_NONE          = 0x0,  /**< No Copy Control Type available */
    IMPORT_CCT_ONE_SEGMENT   = 0x1,  /**< One segment copy control type */
    IMPORT_CCT_FULL_SEGMENT  = 0x2,  /**< Full segment copy control type */
} import_cct_t;

/**
 * @brief APS types
 *
 * Type to specify the source APS value used for the license generation
 **/
typedef enum {
    IMPORT_APS_COPY_FREE  = 0x0,  /**<  Copy free */
    IMPORT_APS_TYPE1      = 0x1,  /**<  APS is on : type1 (AGC) */
    IMPORT_APS_TYPE2      = 0x2,  /**<  APS is on : type2 (AGC + 2L colorstripe) */
    IMPORT_APS_TYPE3      = 0x3   /**<  APS is on : type3 (AGC + 4L colorstripe) */
} import_aps_t;


/**
 * @brief Import DTCP descriptor type
 *
 * Type to specify the dtcp content data
 **/
typedef struct import_dtcp {
    import_epn_t  epn;           /**< Source EPN */
    import_ict_t  ict;           /**< Source ICT */
    import_aps_t  aps;           /**< Source APS */
    import_cci_t  cci;           /**< DTCP CCI */
} import_dtcp_t;


/**
 * @brief Import ARIB descriptor type
 *
 * Type to specify the arib content data
 **/
typedef struct import_arib {
    import_epn_t  epn;           /**< Source EPN */
    import_aps_t  aps;           /**< Source APS */
    import_cci_t  cci;           /**< ARIB CCI */
    import_cct_t  cct;           /**< ARIB CCT */
} import_arib_t;


/**
 * @brief Import OpenMG descriptor type
 *
 * Type to specify the OpenMG content data
 **/
typedef struct {
    import_aps_t       aps;           /**< Source APS */
} import_openmg_t;

/**
 * @brief Import descriptor type
 *
 * Type to specify the mode of the bbts service context
 **/
typedef struct import_desc {
    import_content_type_t   content_type;    /**< Import content type */
    union {
        import_dtcp_t        dtcp_desc;        /**< DTCP content description */
        import_arib_t        arib_desc;        /**< ARIB content description */
        import_openmg_t      openmg_desc;      /**< OpenMG content description */
    } descriptor;
} import_desc_t;

/**
 * @brief Initialize an Import handle.
 *
 * @param[out] out_handle             Import handle
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE
 */
extern retcode_t
import_Init(import_handle_t       *out_handle);

/**
 * @brief Finalize an Import handle.
 *
 * This function finalizes (deallocates) a Import handle. This function should
 * be called only once for a given handle.
 *
 * @param[in,out] io_handle Import handle.
 *
 */
extern void
import_Fin(import_handle_t io_handle);

/**
 * @brief Generate a Marlin license
 *
 * @note : this function assumes that the BK/BKB have been refreshed as appropriate by the caller.
 *
 * @param[in]  in_handle             Import handle
 * @param[in]  in_seashell           Current seashell
 * @param[in]  in_import_desc        Import Descriptor
 * @param[in]  io_keymng             Keymanager handle, containing CKs
 * @param[in]  in_bkb                NULL, or the <BroadcastKeyBlock> string to insert in the license
 * @param[in]  out_seashell          New seashell
 * @param[out] out_license           Generated license
 *
 * @return ::ERR_OK                          <br>Indicating success.
 * @return ::ERR_ILLEGALARGS                 <br>Illegal Argument.
 * @return ::ERR_NOMEM                       <br>Nautilus encountered resource problems.
 * @return ::ERR_INTERNALERROR               <br>An unrecoverable failure occurred in Nautilus.
 * @return ::ERR_ILLEGALHANDLE               <br>import context is is wrong state (e.g. license was already generated once)
 * @return ::ERR_IMPORT_PERSONALITYERROR     <br>Import agent personality is missing or wrong (cannot sign license)
 * @return ::ERR_IMPORT_BKBERROR             <br>A BKB was acquired but cannot be used successfully
 * @return ::ERR_IMPORT_INVALID_DESC         <br>Invalid import descriptor
 * @return ::ERR_IMPORT_NO_CKS_FOUND         <br>Missing content keys
 * @return ::ERR_IMPORT_DEVICEIDTOOLONG      <br>Device ID is too long
 * @return ::ERR_REG_NOPERSONALITYREGISTERED <br>cannot bind license to device because personality is missing
 * @return ::ERR_REG_ILLEGALPERSONALITY      <br>cannot bind license to device because personality is invalid
 * @return ::licgen_GenerateLicense
 */
extern retcode_t
import_GenerateLicense(import_handle_t   in_handle,
                       mbinstr_t        *in_seashell,
                       import_desc_t    *in_import_desc,
                       keymng_handle_t   io_keymng,
                       binstr_t         *in_bkb,
                       mbinstr_t       **out_seashell,
                       binstr_t        **out_license);

/**
 * @brief Returns the License Issuing Service Key(LISK) uid.
 *
 *
 * @param[in]  in_hande             Import handle
 * @param[out] out_lisk_subject     License Issuing Service Key(LISK) subject
 *
 * @return ::ERR_OK                          <br>Indicating success.
 * @return ::ERR_ILLEGALARGS                 <br>Illegal Argument.
 * @return ::ERR_NOMEM                       <br>Nautilus encountered resource problems.
 * @return ::ERR_INTERNALERROR               <br>An unrecoverable failure occurred in Nautilus.
 * @return ::ERR_ILLEGALHANDLE               <br>import context is is wrong state (e.g. license was already generated once)
 * @return ::licgen_GetLISKSubject
 */
extern retcode_t
import_GetLISKSubject(import_handle_t   in_handle,
                      binstr_t        **out_lisk_subject);

NP_END_EXTERN_C

#endif /* IMPORT_API_H */
/** @} */

