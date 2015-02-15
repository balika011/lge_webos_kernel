/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2007,2008,2009 Sony Corporation
 */
#ifndef REND_API_IPTVES_H
#define REND_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basic_types_iptves.h"

/*==========================================================================*/
/**
 * @file
 * Renderer Library
 *
 * require:
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define REND_CTX_MAX                    2
#define REND_KEY_LIST_MAX_LEN           10

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef int32_t (*rend_commit_func_t)(
										po_thid_t, 
										const po_key_ref_t *,
                                        u_int32_t, 
                                        bool_t);

typedef struct {
    u_int8_t    key[PO_CKEY_LEN];
    u_int8_t    output_ctrl[PO_EXPORT_INFO_MAX_LEN];
    u_int16_t   output_ctrl_len;
} rend_key_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_REND_OK                     0x00000000
#define RET_REND_NG                     0x00000001
#define RET_REND_ILLEGAL_ARGS           0x00000002
#define RET_REND_KEY_BUFFER_FULL        0x00000003

/*============================================================================
 * APIs
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 */
extern u_int32_t
rend_Init(void);

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 */
extern u_int32_t
rend_Fin(void);

/**
 * Set content key and rendering obligation
 *
 * @param [in] in_rend_id               Renderer id (0 - REND_CTX_MAX)
 * @param [in] in_key_ref               Key reference information
 * @param [in] in_key_1                 Content Key / Scramble Key (odd)
 * @param [in] in_key_2                 NULL        / Scramble Key (even)
 * @param [in] in_ro                    Rendering obligation
 * @param [in] in_commit                Callback function for commit
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_SetContentKey(u_int32_t            in_rend_id,
                   po_thid_t            in_thid,
                   const po_key_ref_t  *in_key_ref,
                   const u_int8_t       in_key_1[PO_CKEY_LEN],
                   const u_int8_t       in_key_2[PO_CKEY_LEN],
                   u_int8_t             in_output_ctrl[PO_EXPORT_INFO_MAX_LEN],
                   u_int16_t            in_output_ctrl_len,
                   u_int32_t            in_not_after,
                   rend_commit_func_t   in_commit);

/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
/**
 * Extract Content to Analog/Digital Interface
 *
 * @param [in]     in_rend_id           Renderer id (0 - REND_CTX_MAX)
 * @param [in]     in_content           Content
 * @param [in]     in_content_len       Content length
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_Extract(u_int32_t                  in_rend_id,
             const u_int8_t            *in_content,
             u_int32_t                  in_content_len);

/**
 * Export Content to Removable Media
 *
 * @param [in]     in_rend_id           Renderer id (0 - REND_CTX_MAX)
 * @param [in]     in_action_param      Action Parameter (for IPMC)
 * @param [in]     in_content           Content
 * @param [in]     in_content_len       Content length
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_Export(u_int32_t                   in_rend_id,
            u_int8_t                    in_action_param,
            const u_int8_t             *in_content,
            u_int32_t                   in_content_len);

/**
 * Record IPMC Content to Local Storage
 *
 * @param [in]     in_rend_id           Renderer id (0 - REND_CTX_MAX)
 * @param [in]     in_content           Content
 * @param [in]     in_content_len       Content length
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_Record(u_int32_t                   in_rend_id,
            const u_int8_t             *in_content,
            u_int32_t                   in_content_len);

/**
 * Cancel to render
 *
 * @param [in]     in_rend_id           Renderer id (0 or 1)
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_Cancel(u_int32_t                   in_rend_id);

/**
 * Reset export_key_ref_list
 *
 * @param [in]     in_rend_id           Renderer id (0 or 1)
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_StartSession(u_int32_t             in_rend_id);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
/**
 * Get content key / scramble keys and rendering obligation
 *
 * @param [in]     in_rend_id           Renderer id (0 - REND_CTX_MAX)
 * @param [out]    out_key_1            Content Key / Scramble Key (odd)
 * @param [out]    out_key_2            NULL        / Scramble Key (even)
 * @param [out]    out_ro               Rendering Obligation
 * @retval RET_REND_OK                  Success
 * @retval RET_REND_NG                  Fail
 * @retval RET_REND_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
rend_GetContentKey(u_int32_t            in_rend_id,
                   rend_key_t          *out_key_list,
                   u_int32_t           *out_key_num,
                   u_int32_t           *out_not_after);
#endif /* !IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* REND_API_H */
