/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "byte_api_iptves.h"
#include "time_api_iptves.h"
#include "rend_api_iptves.h"
#include "keymng_api.h"
#include "drm_api.h"

COMPONENT(DRM);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/
static int
l_ConvErr(int                         in_err)
{
    exh_InitOk();
    exh_Set(in_err);

    switch (exh_Get()) {
    case RET_OK:
    case RET_NOMEM:
        exh_Rethrow();

    /* keymng */
    case RET_KEYMNG_KEY_NOT_FOUND:
        exh_Throw(RET_DRM_KEY_NOT_FOUND);
    case RET_KEYMNG_BUFFER_SHORT:
        exh_Throw(RET_DRM_BUFFER_SHORT);
    case RET_KEYMNG_KEY_IN_USE:
        exh_Throw(RET_DRM_KEY_IN_USE);
    case RET_KEYMNG_SAVE_ERROR:
        exh_Throw(RET_DRM_SAVE_ERROR);

    default:
        exh_Throw(RET_INTERNALERROR);
        break;
    }

exh_CLEANUP:
    exh_Return();
}

static int
l_Verify(const po_key_info_t            *in_key_info)
{
    exh_InitOk();
    MSGENT();

    if ((in_key_info->flags & PO_KEY_FLAG_NOT_BEFORE_AFTER)) {
        u_int32_t not_before = in_key_info->not_before;
        u_int32_t not_after  = in_key_info->not_after;
        u_int32_t datetime;

        if ((0x00000000 == not_before && 0x00000000 == not_after) ||
            (0xffffffff == not_before && 0xffffffff == not_after)) {
            exh_Throw(RET_OK);
        }

        exh_Set(time_GetTrustedTime(&datetime));
        if (!exh_IsOk()) exh_Throw(RET_DRM_TT_INVALID);

        if ((0x00000000 != not_before && 0xffffffff != not_before && not_before > datetime) ||
            (0x00000000 != not_after  && 0xffffffff != not_after  && not_after  < datetime)) {
            MSG1(("*** ERROR: Key validity error...\n"));
            MSG1(("      not_before = 0x%08x, %s\n", not_before, base_CMintime(&not_before)));
            MSG1(("      not_after  = 0x%08x, %s\n", not_after,  base_CMintime(&not_after)));
            MSG1(("      datetime   = 0x%08x, %s\n", datetime,   base_CMintime(&datetime)));
            exh_Throw(RET_DRM_KEY_INVALID);
        }
    } else if ((in_key_info->flags & PO_KEY_FLAG_EXPORT_INFO)) {
        if ((in_key_info->export_info_len < 1) || (in_key_info->export_info_len > PO_EXPORT_INFO_MAX_LEN)) {
            MSG1(("*** ERROR: wrong export info length (%i)\n", in_key_info->export_info_len));
            exh_Throw(RET_DRM_KEY_INVALID);
        }
    } else {
        MSG1(("*** ERROR: missing key metadata\n"));
        exh_Throw(RET_DRM_KEY_INVALID);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetContentKey(po_thid_t               in_thid,
                const po_key_ref_t     *in_key_ref,
                u_int8_t               *out_ckey,
                po_key_info_t          *out_key_info)
{
    exh_InitOk();
    MSGENT();

    exh_Set(keymng_GetContentKey(in_thid, in_key_ref, out_ckey, out_key_info));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static bool_t
l_Commit(po_thid_t                      in_thid,
         const po_key_ref_t            *in_key_ref_list,
         u_int32_t                      in_key_ref_list_len,
         bool_t                         in_result)
{
    exh_InitOk();
    MSGENT();

    if (!in_key_ref_list)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(keymng_CommitContentKey(in_thid,
                                      in_key_ref_list,
                                      in_key_ref_list_len,
                                      in_result));

exh_CLEANUP:
    MSGEXT(exh_Get());
    return exh_IsOk();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
drm_ExtractContentKey(po_thid_t                 in_thid,
                      u_int32_t                 in_rend_id,
                      const po_key_ref_t       *in_key_ref)
{
    u_int8_t            ckey[PO_CKEY_LEN];
    const po_key_ref_t *key_ref;
    po_key_info_t       key_info;
    u_int8_t            output_ctrl[PO_EXPORT_INFO_MAX_LEN];
    u_int16_t           output_ctrl_len;
    u_int32_t           not_after = 0;
    rend_commit_func_t  commit    = NULL;
    bool_t              is_in_use = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(l_GetContentKey(in_thid, in_key_ref, ckey, &key_info));
    is_in_use = TRUE;

    exh_Check(l_Verify(&key_info));

    switch (in_key_ref->action_id) {
    case PO_ACTION_ID_EXPORT_CP_DL:
        key_ref = in_key_ref;
        exh_Check(base_Memcpy(output_ctrl,
                              key_info.export_info_len,
                              key_info.export_info,
                              key_info.export_info_len));
        output_ctrl_len = key_info.export_info_len;
        commit  = &l_Commit;
        break;
    case PO_ACTION_ID_EXTRACT_DL:
        not_after = key_info.not_after;
        /* falls through to next case */
    case PO_ACTION_ID_EXTRACT_VOD:
        key_ref = NULL;
        byte_BE2W(output_ctrl, key_info.oci);
        output_ctrl_len = 2;
        commit  = NULL;
        break;
    default:
        exh_Throw(RET_INTERNALERROR);
    }

    exh_Set(rend_SetContentKey(in_rend_id, in_thid,
                               key_ref, ckey, NULL,
                               output_ctrl, output_ctrl_len,
                               not_after, commit));
    if (!exh_IsOk()) exh_Throw(RET_DRM_REND_ERROR);

exh_CLEANUP:
    if (is_in_use && (!commit || !exh_IsOk())) {
        int ret = exh_Get();

        exh_Set(keymng_CommitContentKey(in_thid, in_key_ref, 1, exh_IsOk()));
        if (exh_IsOk()) {
            exh_Set(ret);
        } else {
            exh_Set(l_ConvErr(exh_Get()));
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}

int
drm_DelContentKey(po_thid_t             in_thid,
                  const po_key_ref_t   *in_key_ref)
{
    exh_InitOk();
    MSGENT();

    if (!in_key_ref)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(keymng_DelContentKey(in_thid, in_key_ref));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
drm_DelUncommittedKey(po_thid_t                  in_thid,
                      const po_key_ref_t        *in_key_ref)
{
    exh_InitOk();
    MSGENT();

    if (!in_key_ref)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(keymng_DelUncommittedKey(in_thid, in_key_ref));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
drm_GetKeyInfo(po_thid_t                in_thid,
               const po_key_ref_t      *in_key_ref,
               po_key_info_t           *out_key_info)
{
    exh_InitOk();
    MSGENT();

    if (!in_key_ref || !out_key_info)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(keymng_GetKeyInfo(in_thid, in_key_ref, out_key_info));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
drm_GetKeyNum(po_thid_t                 in_thid,
              u_int8_t                  in_action_id,
              u_int32_t                *out_num)
{
    exh_InitOk();
    MSGENT();

    if (!out_num)
        exh_Throw(RET_ILLEGALARGS);

	dprintf("\n>>%s\n", __FUNCTION__);

    exh_Set(keymng_GetKeyNum(in_thid, in_action_id, out_num));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
drm_GetKeyRefList(po_thid_t             in_thid,
                  u_int8_t              in_action_id,
                  u_int32_t            *io_num,
                  po_key_ref_t         *out_key_ref_lst)
{
    exh_InitOk();
    MSGENT();

    if (!io_num || !out_key_ref_lst)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(keymng_GetKeyRefList(in_thid, in_action_id, io_num, out_key_ref_lst));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

//#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
int
drm_GetContentKey(po_thid_t             in_thid,
                  const po_key_ref_t   *in_key_ref,
                  u_int8_t              out_ckey[PO_CKEY_LEN])
{
    u_int8_t      ckey[PO_CKEY_LEN];
    po_key_info_t key_info;
    bool_t        is_in_use = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref || !out_ckey)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(l_GetContentKey(in_thid, in_key_ref, ckey, &key_info));
    is_in_use = TRUE;

    exh_Check(l_Verify(&key_info));

    exh_Check(base_Memcpy(out_ckey, PO_CKEY_LEN, ckey, PO_CKEY_LEN));

exh_CLEANUP:
    if (is_in_use) {
        exh_Set(keymng_CommitContentKey(in_thid, in_key_ref, 1, FALSE));
        if (!exh_IsOk()) exh_Set(l_ConvErr(exh_Get()));
    }

    MSGEXT(exh_Get());
    exh_Return();
}
//#endif /* !IPTVES_PUBLISH_BUILD */
