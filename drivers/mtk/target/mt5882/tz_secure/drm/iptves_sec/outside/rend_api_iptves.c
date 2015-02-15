/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2007,2008,2009 Sony Corporation
 */

#include "basic_types_iptves.h"

#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "byte_api_iptves.h"
#include "time_api_iptves.h"



#include "rend_api_iptves.h"

COMPONENT(REND);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define REND_VALIDITY_EXTENTION_TIME    0xF0 /* 4 hours */

#define L_RO_SHIFT_IMAGECONSTRAINTTOKEN 9
#define L_RO_SHIFT_RETENTIONMODE        8
#define L_RO_SHIFT_RETENTIONSTATE       5
/* bit 0 of rendering obligation in specs is most significant bit */

#define L_RO_MASK_IMAGECONSTRAINTTOKEN  1
#define L_RO_MASK_RETENTIONMODE         1
#define L_RO_MASK_RETENTIONSTATE        0x7

#define L_RO_SAFE_IMAGECONSTRAINTTOKEN  1
#define L_RO_SAFE_RETENTIONMODE         0
#define L_RO_SAFE_RETENTIONSTATE        0x7

#define REND_STATUS_NOT_USED            0
#define REND_STATUS_NORMAL_USAGE        1
#define REND_STATUS_EXPORT_SESSION      2

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    po_key_ref_t        key_ref_list[REND_KEY_LIST_MAX_LEN];
    rend_key_t          key_list[REND_KEY_LIST_MAX_LEN];
    u_int32_t           key_list_len;
    u_int32_t           status;
    u_int32_t           not_after;
    po_thid_t           thid;
    rend_commit_func_t  commit;
} rend_ctx_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/
static rend_ctx_t       l_ctx_tbl[REND_CTX_MAX];

/*============================================================================
 * Local Functions
 *==========================================================================*/
static u_int32_t
l_GetContext(u_int32_t                  in_rend_id,
             rend_ctx_t               **out_ctx)
{
    exh_InitOk();
    MSGENT();

    if (in_rend_id >= REND_CTX_MAX)
        exh_Throw(RET_REND_ILLEGAL_ARGS);

    *out_ctx = &l_ctx_tbl[in_rend_id];

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static u_int32_t
l_MakeSafe(u_int8_t                     io_output_ctrl[PO_EXPORT_INFO_MAX_LEN])
{
    /*
     * [IPTVCRVOD] 2.2.1.2: Output Control by RenderingObligation
     * [IPTVCRMC]  2.2.1.2: Output Control by RenderingObligation
     * [IPTVCRDL]  2.2.1.2: Output Control by RenderingObligation
     */
    u_int16_t   ro = byte_BE2R(io_output_ctrl);

    exh_InitOk();
    MSGENT();

    ro &= ~(L_RO_MASK_IMAGECONSTRAINTTOKEN << L_RO_SHIFT_IMAGECONSTRAINTTOKEN)
        & ~(L_RO_MASK_RETENTIONMODE        << L_RO_SHIFT_RETENTIONMODE)
        & ~(L_RO_MASK_RETENTIONSTATE       << L_RO_SHIFT_RETENTIONSTATE);

    ro |= (L_RO_SAFE_IMAGECONSTRAINTTOKEN << L_RO_SHIFT_IMAGECONSTRAINTTOKEN)
        | (L_RO_SAFE_RETENTIONMODE        << L_RO_SHIFT_RETENTIONMODE)
        | (L_RO_SAFE_RETENTIONSTATE       << L_RO_SHIFT_RETENTIONSTATE);

    byte_BE2W(io_output_ctrl, ro);

    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
u_int32_t
rend_Init(void)
{
    u_int32_t   i;

    exh_InitOk();
    MSGENT();

    /* If necessary, codes for initialization should be written here. */
    for (i = 0; i < REND_CTX_MAX; i++) {
        l_ctx_tbl[i].key_list_len = 0;
        l_ctx_tbl[i].status = REND_STATUS_NOT_USED;
		dprintf("%s rend[%d] status[%d]\n", __FUNCTION__, i, l_ctx_tbl[i].status);
    }

    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
rend_Fin(void)
{
    exh_InitOk();
    MSGENT();

    /* If necessary, codes for finalization should be written here. */

    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
rend_SetContentKey(u_int32_t            in_rend_id,
                   po_thid_t            in_thid,
                   const po_key_ref_t  *in_key_ref,
                   const u_int8_t       in_key_1[PO_CKEY_LEN],
                   const u_int8_t       in_key_2[PO_CKEY_LEN],
                   u_int8_t             in_output_ctrl[PO_EXPORT_INFO_MAX_LEN],
                   u_int16_t            in_output_ctrl_len,
                   u_int32_t            in_not_after,
                   rend_commit_func_t   in_commit)
{
    rend_ctx_t *ctx;
    rend_key_t *ckey = NULL;

    exh_InitOk();
    MSGENT();

 
	dprintf("%s thid:%d rend_id:%d \n", __FUNCTION__, 
		 in_thid, 
		 in_rend_id);


    if ((!in_key_ref && in_commit) || (in_key_ref && !in_commit) || !in_key_1 ||
        !in_output_ctrl || (in_output_ctrl_len == 0) ||
        (in_output_ctrl_len > PO_EXPORT_INFO_MAX_LEN)) {
        exh_Throw(RET_REND_ILLEGAL_ARGS);
    }

    exh_Check(l_GetContext(in_rend_id, &ctx));
	dprintf("%s ctx->status:%d\n", __FUNCTION__, ctx->status);

	/*ignore ctx stats check */
#if 0	
    if ((ctx->status != REND_STATUS_NOT_USED) &&
        (ctx->status != REND_STATUS_EXPORT_SESSION)) {
        exh_Throw(RET_REND_NG);
    }
#endif
    ckey = &ctx->key_list[ctx->key_list_len];

    if ((in_key_ref) &&
        (in_key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL)) {

        if (ctx->key_list_len >= REND_KEY_LIST_MAX_LEN)
            exh_Throw(RET_REND_KEY_BUFFER_FULL);

        ctx->key_ref_list[ctx->key_list_len] = *in_key_ref;
        exh_Check(base_Memcpy(ckey->key, PO_CKEY_LEN, in_key_1, PO_CKEY_LEN));
        exh_Check(base_Memcpy(ckey->output_ctrl, in_output_ctrl_len, in_output_ctrl, in_output_ctrl_len));
        ckey->output_ctrl_len = in_output_ctrl_len;
        ctx->key_list_len++;
        ctx->status = REND_STATUS_EXPORT_SESSION;

		dprintf("%s set1 ctx->status:%d\n", __FUNCTION__, ctx->status);
		
    } else {
        if (ctx->status == REND_STATUS_EXPORT_SESSION)
            exh_Throw(RET_REND_NG);

        if (in_key_ref)
            ctx->key_ref_list[ctx->key_list_len] = *in_key_ref;
		
        exh_Check(base_Memcpy(ckey->key, PO_CKEY_LEN, in_key_1, PO_CKEY_LEN));
        exh_Check(base_Memcpy(ckey->output_ctrl, in_output_ctrl_len, in_output_ctrl, in_output_ctrl_len));
        ckey->output_ctrl_len = in_output_ctrl_len;
        ctx->key_list_len++;
        ctx->status = REND_STATUS_NORMAL_USAGE;
		
		dprintf("%s set2 ctx->status:%d\n", __FUNCTION__, ctx->status);

        exh_Check(l_MakeSafe(ckey->output_ctrl));
    }

    if (in_key_2) {
        exh_Check(base_Memcpy(&ctx->key_list[ctx->key_list_len], PO_CKEY_LEN, in_key_2, PO_CKEY_LEN));
        ctx->key_list_len++;
    }

    ctx->commit = in_commit;
    ctx->thid = in_thid;
    ctx->not_after = in_not_after;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}



/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
u_int32_t
rend_Extract(u_int32_t                  in_rend_id,
             const u_int8_t            *in_content,
             u_int32_t                  in_content_len)
{
    rend_ctx_t *ctx;
    int32_t     result;
    u_int32_t   tt;

    exh_InitOk();
    MSGENT();

    if ((!in_content && in_content_len != 0) ||
        ( in_content && in_content_len == 0))
        exh_Throw(RET_REND_ILLEGAL_ARGS);

    exh_Check(l_GetContext(in_rend_id, &ctx));
    if (ctx->status != REND_STATUS_NORMAL_USAGE)
        exh_Throw(RET_REND_NG);

    /* Check Rendering Obligation */

    while (FALSE) { /* should become: while (packets in video) */

        if (ctx->key_ref_list[0].action_id == PO_ACTION_ID_EXTRACT_DL) 
		{

            exh_Set(time_GetTrustedTime(&tt));
            if (!exh_IsOk()) exh_Throw(RET_REND_NG);

            if (tt > ctx->not_after + REND_VALIDITY_EXTENTION_TIME) {
                break; /* rendering */
            }
        }

        /* Call Decrytor */
    }

    /* Extract to Digital/Analog Interface */
    result = 1;

    if (ctx->commit) {
        result = ctx->commit(ctx->thid, ctx->key_ref_list,
                             ctx->key_list_len, result);
        exh_Set(result ? RET_REND_OK : RET_REND_NG);
    }

    memset(ctx, 0, sizeof(rend_ctx_t));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
rend_Export(u_int32_t                   in_rend_id,
            u_int8_t                    in_action_param,
            const u_int8_t             *in_content,
            u_int32_t                   in_content_len)
{
    rend_ctx_t *ctx;
    int32_t     result;

    exh_InitOk();
    MSGENT();

    if ((!in_content && in_content_len != 0) ||
        ( in_content && in_content_len == 0))
        return RET_REND_ILLEGAL_ARGS;

    exh_Check(l_GetContext(in_rend_id, &ctx));
    if (ctx->status != REND_STATUS_EXPORT_SESSION)
        exh_Throw(RET_REND_NG);

    /* Check Rendering Obligation */

    /* Call Decrytor */

    /* Export to Removable Media */
    result = 1;

    if (ctx->commit) {
        result = ctx->commit(ctx->thid, ctx->key_ref_list,
                             ctx->key_list_len, result);
        exh_Set(result ? RET_REND_OK : RET_REND_NG);
    }

    memset(ctx, 0, sizeof(rend_ctx_t));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
rend_Record(u_int32_t                   in_rend_id,
            const u_int8_t             *in_content,
            u_int32_t                   in_content_len)
{
    rend_ctx_t *ctx;
    int32_t     result;

    exh_InitOk();
    MSGENT();

    if ((!in_content && in_content_len != 0) ||
        ( in_content && in_content_len == 0))
        exh_Throw(RET_REND_ILLEGAL_ARGS);

    exh_Check(l_GetContext(in_rend_id, &ctx));
    if (ctx->status != REND_STATUS_NORMAL_USAGE)
        exh_Throw(RET_REND_NG);

    /* Check Rendering Obligation */

    /* Call Decrytor */

    /* Recored to Local Storage */
    result = 1;

    if (ctx->commit) {
        result = ctx->commit(ctx->thid, ctx->key_ref_list,
                             ctx->key_list_len, result);
        exh_Set(result ? RET_REND_OK : RET_REND_NG);
    }

    memset(ctx, 0, sizeof(rend_ctx_t));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
rend_Cancel(u_int32_t                   in_rend_id)
{
    rend_ctx_t *ctx;
    int32_t     result;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetContext(in_rend_id, &ctx));
    if (ctx->status == REND_STATUS_NOT_USED)
        exh_Throw(RET_REND_NG);

    if (ctx->commit) {
        result = ctx->commit(ctx->thid, ctx->key_ref_list,
                             ctx->key_list_len, 0);
        exh_Set(result ? RET_REND_OK : RET_REND_NG);
    }

    memset(ctx, 0, sizeof(rend_ctx_t));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
rend_StartSession(u_int32_t             in_rend_id)
{
    rend_ctx_t *ctx;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetContext(in_rend_id, &ctx));
    if (ctx->status != REND_STATUS_NOT_USED)
        exh_Throw(RET_REND_NG);

    ctx->key_list_len = 0;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
u_int32_t
rend_GetContentKey(u_int32_t            in_rend_id,
                   rend_key_t          *out_key_list,
                   u_int32_t           *out_key_num,
                   u_int32_t           *out_not_after)
{
    rend_ctx_t *ctx;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetContext(in_rend_id, &ctx));
    if (ctx->status == REND_STATUS_NOT_USED)
        exh_Throw(RET_REND_NG);

    if (out_key_list)
        exh_Check(base_Memcpy(out_key_list, ctx->key_list_len * sizeof(rend_key_t), ctx->key_list, ctx->key_list_len * sizeof(rend_key_t)));

    if (out_key_num)
        *out_key_num = ctx->key_list_len;

    if (out_not_after)
        *out_not_after = ctx->not_after;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
#endif /* !IPTVES_PUBLISH_BUILD */



