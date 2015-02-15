/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef KEYMNG_API_H
#define KEYMNG_API_H

#include "basic_types_iptves.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Key Manager API
 *
 * require: cache, base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define KEYMNG_SRV_SBJ_LEN                      64
#define KEYMNG_MAX_LICENSES                     254

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_KEYMNG_KEY_NOT_FOUND                (RET_KEYMNG_BASE|1)
#define RET_KEYMNG_BUFFER_SHORT                 (RET_KEYMNG_BASE|2)
#define RET_KEYMNG_TOO_MANY_LICENSES            (RET_KEYMNG_BASE|3)
#define RET_KEYMNG_KEY_IN_USE                   (RET_KEYMNG_BASE|4)
#define RET_KEYMNG_SAVE_ERROR                   (RET_KEYMNG_BASE|5)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Set content key
 *
 * @param [in]     in_thid                      Trust Hierarchy ID
 * @param [in]     in_key_ref_lst               Key reference
 * @param [in]     in_ckey_lst                  Content key
 * @param [in]     in_key_info_lst              Key information
 * @param [in]     in_key_num                   Number of keys
 * @param [in]     in_srv_sbj                   Subject of service cert
 * @param [in]     in_trans_flag                Transaction Flag
 * @retval RET_OK                               Success
 * @retval RET_NOMEM                            Fail to allocate memory
 * @retval RET_ILLEGALARGS                      Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR                    An unexpected error has occurred
 * @retval RET_KEYMNG_TOO_MANY_LICENSES         Number of licenses alloted exceeds maximum value
 * @retval RET_KEYMNG_SAVE_ERROR                Fail to save read-write NVS
 */
extern int
keymng_SetContentKey(po_thid_t                  in_thid,
                     const po_key_ref_t        *in_key_ref_lst,
                     const u_int8_t            *in_ckey_lst,
                     const po_key_info_t       *in_key_info_lst,
                     u_int32_t                  in_key_num,
                     const u_int8_t             in_srv_sbj[KEYMNG_SRV_SBJ_LEN],
                     u_int8_t                   in_trans_flag);

/**
 * Get content key
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_key_ref           Key reference
 * @param [out]    out_ckey             Content key
 * @param [out]    out_key_info         Key information
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_KEY_NOT_FOUND     The key specified by @p in_key_ref is not found
 * @retval RET_KEYMNG_KEY_IN_USE        All keys and licenses are already used by renderer
 */
extern int
keymng_GetContentKey(po_thid_t                  in_thid,
                     const po_key_ref_t        *in_key_ref,
                     u_int8_t                  *out_ckey,
                     po_key_info_t             *out_key_info);

/**
 * Get content key by content id
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_ckey_id           Content key id
 * @param [out]    out_ckey             Content key
 * @param [out]    out_key_info         Key information
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_KEY_NOT_FOUND     The key specified by @p in_key_ref is not found
 */
extern int
keymng_GetContentKeyById(po_thid_t              in_thid,
                         const u_int8_t        *in_ckey_id,
                         u_int8_t              *out_ckey,
                         po_key_info_t         *out_key_info);

/**
 * Delete content key
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_key_ref           Key reference
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_KEY_IN_USE        All keys and licenses are currently used by renderer
 * @retval RET_KEYMNG_SAVE_ERROR        Fail to save read-write NVS
 */
extern int
keymng_DelContentKey(po_thid_t                  in_thid,
                     const po_key_ref_t        *in_key_ref);

/**
 * Commit content key
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_key_ref_lst       Key reference list
 * @param [in]     in_key_num           Key reference list length
 * @param [in]     in_result            Result
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_SAVE_ERROR        Fail to save read-write NVS
 */
int
keymng_CommitContentKey(po_thid_t               in_thid,
                        const po_key_ref_t     *in_key_ref_lst,
                        u_int32_t               in_key_num,
                        bool_t                  in_result);

/**
 * Get key information
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_key_ref           Key reference
 * @param [out]    out_key_info         Key information
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_KEY_NOT_FOUND     The key specified by @p in_key_ref is not found
 */
extern int
keymng_GetKeyInfo(po_thid_t                     in_thid,
                  const po_key_ref_t           *in_key_ref,
                  po_key_info_t                *out_key_info);

/**
 * Get key number
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_action_id         Action ID
 * @param [out]    out_num              Key number
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
keymng_GetKeyNum(po_thid_t                      in_thid,
                 u_int8_t                       in_action_id,
                 u_int32_t                     *out_num);

/**
 * Get key reference list
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_action_id         Action ID
 * @param [in,out] io_num               Key number
 * @param [out]    out_key_ref_lst      Key reference list
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_BUFFER_SHORT      @p io_num is not enough to get key reference list
 */
extern int
keymng_GetKeyRefList(po_thid_t                  in_thid,
                     u_int8_t                   in_action_id,
                     u_int32_t                 *io_num,
                     po_key_ref_t              *out_key_ref_lst);

/**
 * Get TransactionFlag
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_srv_sbj           Subject of service cert
 * @param [out]    out_trans_flag       Transaction flag
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
keymng_GetTransactionFlag(po_thid_t             in_thid,
                          const u_int8_t        in_srv_sbj[KEYMNG_SRV_SBJ_LEN],
                          u_int8_t             *out_trans_flag);

/**
 * Delete TransactionFlag and make ContentKey available
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_srv_sbj           Subject of service cert
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_TOO_MANY_LICENSES Number of licenses alloted exceeds maximum value
 * @retval RET_KEYMNG_SAVE_ERROR        Fail to save read-write NVS
 */
extern int
keymng_EnableContentKey(po_thid_t               in_thid,
                        const u_int8_t          in_srv_sbj[KEYMNG_SRV_SBJ_LEN]);

/**
 * Delete uncommitted key
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_key_ref           Key reference
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_KEYMNG_SAVE_ERROR        Fail to save read-write NVS
 */
extern int
keymng_DelUncommittedKey(po_thid_t              in_thid,
                         const po_key_ref_t    *in_key_ref);

#ifdef __cplusplus
}
#endif

#endif /* KEYMNG_API_H */
