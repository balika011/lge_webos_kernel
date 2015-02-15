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
#include "mutex_api_iptves.h"
#include "byte_api_iptves.h"
#include "cache_api.h"
#include "keymng_api.h"

COMPONENT(KEYMNG);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_OFFSET_KEY_REF_URR                    0
#define L_OFFSET_KEY_REF_URR_LEN               (L_OFFSET_KEY_REF_URR               + PO_URR_LEN)
#define L_OFFSET_KEY_REF_ACTION_ID             (L_OFFSET_KEY_REF_URR_LEN           + sizeof(u_int8_t))
#define L_OFFSET_KEY_REF_ACTION_PARAM          (L_OFFSET_KEY_REF_ACTION_ID         + sizeof(u_int8_t))
#define L_OFFSET_KEY_INFO_FLAGS                (L_OFFSET_KEY_REF_ACTION_PARAM      + sizeof(u_int8_t))
#define L_OFFSET_KEY_INFO_CKEY_ID              (L_OFFSET_KEY_INFO_FLAGS            + sizeof(u_int8_t))
#define L_OFFSET_KEY_INFO_PRIV_DATA            (L_OFFSET_KEY_INFO_CKEY_ID          + PO_CKEY_ID_LEN)
#define L_OFFSET_KEY_INFO_TIERBITS             (L_OFFSET_KEY_INFO_PRIV_DATA        + sizeof(u_int16_t))
#define L_OFFSET_KEY_INFO_OCI                  (L_OFFSET_KEY_INFO_TIERBITS         + PO_TIERBITS_LEN)
#define L_OFFSET_KEY_INFO_EXPORT_INFO          (L_OFFSET_KEY_INFO_OCI              + sizeof(u_int16_t))
#define L_OFFSET_KEY_INFO_EXPORT_INFO_LEN      (L_OFFSET_KEY_INFO_EXPORT_INFO      + PO_EXPORT_INFO_MAX_LEN)
#define L_OFFSET_KEY_INFO_NOT_BEFORE           (L_OFFSET_KEY_INFO_EXPORT_INFO_LEN  + sizeof(u_int16_t))
#define L_OFFSET_KEY_INFO_NOT_AFTER            (L_OFFSET_KEY_INFO_NOT_BEFORE       + sizeof(u_int32_t))
#define L_OFFSET_KEY_INFO_NUM                  (L_OFFSET_KEY_INFO_NOT_AFTER        + sizeof(u_int32_t))
#define L_OFFSET_CKEY                          (L_OFFSET_KEY_INFO_NUM              + sizeof(u_int8_t))

#define L_OFFSET_SRV_SBJ                       (L_OFFSET_CKEY                      + PO_CKEY_LEN)
#define L_OFFSET_TRANS_FLAG                    (L_OFFSET_SRV_SBJ                   + KEYMNG_SRV_SBJ_LEN)

#define L_OBJ_LEN                              (L_OFFSET_CKEY                      + PO_CKEY_LEN)
#define L_OBJ_BIG_LEN                          (L_OFFSET_TRANS_FLAG                + sizeof(u_int8_t))

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    KEYMNG_STATUS_AVAILABLE = 0,
    KEYMNG_STATUS_UNCOMMITTED,
    KEYMNG_STATUS_INUSE,
} keymng_status_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/
static int
l_ConvErr(int                                 in_err)
{
    exh_InitOk();
    exh_Set(in_err);

    switch (exh_Get()) {
    case RET_OK:
    case RET_NOMEM:
        exh_Rethrow();

    /* cache */
    case RET_CACHE_SAVE_ERROR:
        exh_Throw(RET_KEYMNG_SAVE_ERROR);

    default:
        exh_Throw(RET_INTERNALERROR);
        break;
    }

exh_CLEANUP:
    exh_Return();
}

static int
l_GetCacheInfo(u_int8_t                         in_action_id,
               keymng_status_t                  in_status,
               cache_tag_t                     *out_tag,
               cache_attr_t                    *out_attr)
{
    cache_tag_t         tag;
    cache_attr_t        attr;

    exh_InitOk();
    MSGENT();

    switch (in_action_id) {
    case PO_ACTION_ID_EXTRACT_VOD:
        switch (in_status) {
        case KEYMNG_STATUS_AVAILABLE:
            tag  = CACHE_TAG_VODKEY;
            attr = CACHE_ATTR_VOLATILE;
            break;
        case KEYMNG_STATUS_INUSE:
            tag  = CACHE_TAG_INUSEKEY;
            attr = CACHE_ATTR_VOLATILE;
            break;
        default:
            exh_Throw(RET_INTERNALERROR);
        }
        break;
    case PO_ACTION_ID_EXTRACT_IPMC:
        tag  = CACHE_TAG_IPMCKEY;
        attr = CACHE_ATTR_READWRITE;
        break;
    case PO_ACTION_ID_EXTRACT_DL:
        tag  = CACHE_TAG_DLKEY;
        attr = CACHE_ATTR_READWRITE;
        break;
    case PO_ACTION_ID_EXPORT_CP_DL:
        switch (in_status) {
        case KEYMNG_STATUS_AVAILABLE:
            tag  = CACHE_TAG_DLCPKEY;
            attr = CACHE_ATTR_READWRITE;
            break;
        case KEYMNG_STATUS_UNCOMMITTED:
            tag  = CACHE_TAG_UNAVAILABLE;
            attr = CACHE_ATTR_READWRITE;
            break;
        case KEYMNG_STATUS_INUSE:
            tag  = CACHE_TAG_INUSEKEY;
            attr = CACHE_ATTR_VOLATILE;
            break;
        default:
            exh_Throw(RET_INTERNALERROR);
        }
        break;
    default:
        exh_Throw(RET_INTERNALERROR);
    }

    if (out_tag)  *out_tag  = tag;
    if (out_attr) *out_attr = attr;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Parse
 *--------------------------------------------------------------------------*/
static int
l_ParseKeyRef(const u_int8_t                   *in_obj,
              po_key_ref_t                     *out_key_ref)
{
    const u_int8_t     *p = in_obj + L_OFFSET_KEY_REF_URR;

    exh_InitOk();
    MSGENT();

    byte_CPYRI(p, out_key_ref->urr, PO_URR_LEN);
    byte_BE1RI(p, out_key_ref->urr_len);
    byte_BE1RI(p, out_key_ref->action_id);
    byte_BE1RI(p, out_key_ref->action_param);

    MSGEXT(exh_Get());
    exh_Return();
}

static bool_t
l_ParseKeyRefList(const void                   *in_obj,
                  void                         *out_obj_lst,
                  u_int32_t                     in_num)
{
    bool_t              ret = TRUE;
    po_key_ref_t       *key_ref_lst = (po_key_ref_t *)out_obj_lst;

    MSGENT();

    l_ParseKeyRef(in_obj, key_ref_lst + in_num);

    MSGEXT(ret);

    return ret;
}

static int
l_ParseKeyInfo(const u_int8_t                  *in_obj,
               po_key_info_t                   *out_key_info)
{
    const u_int8_t     *p = in_obj + L_OFFSET_KEY_INFO_FLAGS;

    exh_InitOk();
    MSGENT();

    byte_BE1RI(p, out_key_info->flags);
    byte_CPYRI(p, out_key_info->ckey_id, PO_CKEY_ID_LEN);
    byte_BE2RI(p, out_key_info->priv_data);
    byte_CPYRI(p, out_key_info->tierbits, PO_TIERBITS_LEN);
    byte_BE2RI(p, out_key_info->oci);
    byte_CPYRI(p, out_key_info->export_info, PO_EXPORT_INFO_MAX_LEN);
    byte_BE2RI(p, out_key_info->export_info_len);
    byte_BE4RI(p, out_key_info->not_before);
    byte_BE4RI(p, out_key_info->not_after);
    byte_BE1RI(p, out_key_info->num);

    out_key_info->num_in_use = 0;

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseCkey(const u_int8_t                     *in_obj,
            u_int8_t                           *out_ckey)
{
    const u_int8_t     *p = in_obj + L_OFFSET_CKEY;

    exh_InitOk();
    MSGENT();

    byte_CPYRI(p, out_ckey, PO_CKEY_LEN);

    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Data matching for QUERY
 *--------------------------------------------------------------------------*/
static bool_t
l_CompareKeyRef(const void                     *in_key_ref,
                const void                     *in_obj)
{
    bool_t      ret;

    MSGENT();

    if (!in_key_ref || !in_obj) {
        ret = FALSE;
    } else {
        po_key_ref_t *key_ref      = (po_key_ref_t *)in_key_ref;
        u_int8_t     *obj          = (u_int8_t *)in_obj;
        u_int8_t     *urr          = obj + L_OFFSET_KEY_REF_URR;
        u_int8_t      urr_len      = byte_BE1R(obj + L_OFFSET_KEY_REF_URR_LEN);
        u_int8_t      action_id    = byte_BE1R(obj + L_OFFSET_KEY_REF_ACTION_ID);
        u_int8_t      action_param = byte_BE1R(obj + L_OFFSET_KEY_REF_ACTION_PARAM);

        MSGI3(("Searching URR: ")); BDUMP3(key_ref->urr_len, key_ref->urr); MSG3(("\n"));
        MSGI3(("Target URR   : ")); BDUMP3(urr_len, urr); MSG3(("\n"));

        if (key_ref->urr_len      == urr_len &&
            !memcmp(key_ref->urr, urr, urr_len) &&
            key_ref->action_id    == action_id &&
            key_ref->action_param == action_param) {
            MSGI3(("  FOUND !!!\n"));
            ret = TRUE;
        } else {
            ret = FALSE;
        }
    }

    MSGEXT(ret);

    return ret;
}

static bool_t
l_CompareCkeyIdForSet(const void               *in_ckey_id,
                      const void               *in_obj)
{
    bool_t      ret;

    MSGENT();

    if (!in_ckey_id || !in_obj) {
        ret = FALSE;
    } else {
        u_int8_t *obj     = (u_int8_t *)in_obj;
        u_int8_t *ckey_id = obj + L_OFFSET_KEY_INFO_CKEY_ID;

        MSGI3(("Searching CKey ID: ")); BDUMP3(PO_CKEY_ID_LEN, in_ckey_id); MSG3(("\n"));
        MSGI3(("Target CKey ID   : ")); BDUMP3(PO_CKEY_ID_LEN, ckey_id); MSG3(("\n"));

        if (!memcmp(in_ckey_id, ckey_id, PO_CKEY_ID_LEN - 1) &&
            (((u_int8_t *)in_ckey_id)[PO_CKEY_ID_LEN - 1] & 0x1) == (ckey_id[PO_CKEY_ID_LEN - 1] & 0x1)) {
            MSGI3(("  FOUND !!!\n"));
            ret = TRUE;
        } else {
            ret = FALSE;
        }
    }

    MSGEXT(ret);

    return ret;
}

static bool_t
l_CompareCkeyIdForGet(const void               *in_ckey_id,
                      const void               *in_obj)
{
    bool_t      ret;

    MSGENT();

    if (!in_ckey_id || !in_obj) {
        ret = FALSE;
    } else {
        u_int8_t *obj     = (u_int8_t *)in_obj;
        u_int8_t *ckey_id = obj + L_OFFSET_KEY_INFO_CKEY_ID;

        MSGI3(("Searching CKey ID: ")); BDUMP3(PO_CKEY_ID_LEN, in_ckey_id); MSG3(("\n"));
        MSGI3(("Target CKey ID   : ")); BDUMP3(PO_CKEY_ID_LEN, ckey_id); MSG3(("\n"));

        if (!memcmp(in_ckey_id, ckey_id, PO_CKEY_ID_LEN)) {
            MSGI3(("  FOUND !!!\n"));
            ret = TRUE;
        } else {
            ret = FALSE;
        }
    }

    MSGEXT(ret);

    return ret;
}

static bool_t
l_CompareSrvSbj(const void                     *in_srv_sbj,
                const void                     *in_obj)
{
    bool_t      ret;

    MSGENT();

    if (!in_srv_sbj || !in_obj) {
        ret = FALSE;
    } else {
        u_int8_t *obj          = (u_int8_t *)in_obj;
        u_int8_t *srv_sbj      = obj + L_OFFSET_SRV_SBJ;

        if (!memcmp(in_srv_sbj, srv_sbj, KEYMNG_SRV_SBJ_LEN)) {
            ret = TRUE;
        } else {
            ret = FALSE;
        }
    }

    MSGEXT(ret);
    return ret;
}

/*----------------------------------------------------------------------------
 * Cache Wrapper
 *--------------------------------------------------------------------------*/
static int
l_SetContentKey(po_thid_t                       in_thid,
                cache_tkt_t                    *io_tkt,
                const po_key_ref_t             *in_key_ref,
                const u_int8_t                 *in_ckey_id,
                const u_int8_t                 *in_srv_sbj,
                keymng_status_t                 in_status,
                u_int32_t                       in_obj_len,
                u_int8_t                       *in_obj,
                u_int8_t                        in_num)
{
    bool_t            (*cmp_func)(const void *, const void *);
    const void         *search_key;
    cache_tag_t         tag;
    cache_attr_t        attr;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetCacheInfo(in_key_ref->action_id, in_status, &tag, &attr));

    if (tag == CACHE_TAG_VODKEY) {
        search_key = NULL;
        cmp_func   = NULL;
    } else if (tag == CACHE_TAG_IPMCKEY) {
        search_key = in_ckey_id;
        cmp_func   = &l_CompareCkeyIdForSet;
    } else if (tag == CACHE_TAG_UNAVAILABLE) {
        search_key = in_srv_sbj;
        cmp_func   = &l_CompareSrvSbj;
    } else {
        search_key = in_key_ref;
        cmp_func   = &l_CompareKeyRef;
    }

    byte_BE1W(in_obj + L_OFFSET_KEY_INFO_NUM, in_num);

    exh_Set(cache_SetObject(in_thid, io_tkt, tag, attr, search_key,
                            cmp_func, in_obj, in_obj_len));
    if (!exh_IsOk()) {
        if (exh_Get() == RET_NOMEM)
            exh_Rethrow();
        else
            exh_Throw(RET_INTERNALERROR);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetContentKey(po_thid_t                       in_thid,
                cache_tkt_t                    *io_tkt,
                const po_key_ref_t             *in_key_ref,
                const u_int8_t                 *in_ckey_id,
                const u_int8_t                 *in_srv_sbj,
                keymng_status_t                 in_status,
                u_int32_t                       in_exp_obj_len,
                u_int8_t                       *out_obj,
                u_int8_t                       *out_num,
                bool_t                         *out_found)
{
    bool_t            (*cmp_func)(const void *, const void *);
    const void         *search_key;
    cache_tag_t         tag;
    cache_act_t         act     = CACHE_ACT_COPY;
    u_int8_t           *obj     = out_obj;
    u_int32_t           obj_len = in_exp_obj_len;

    exh_InitOk();
    MSGENT();

    if (in_ckey_id) {
        tag        = CACHE_TAG_IPMCKEY;

        search_key = in_ckey_id;
        cmp_func   = &l_CompareCkeyIdForGet;
    } else if (in_srv_sbj) {
        tag        = CACHE_TAG_UNAVAILABLE;

        search_key = in_srv_sbj;
        cmp_func   = &l_CompareSrvSbj;
    } else {
        exh_Check(l_GetCacheInfo(in_key_ref->action_id, in_status, &tag, NULL));

        search_key = in_key_ref;
        cmp_func   = &l_CompareKeyRef;
    }

    exh_Set(cache_GetObject(in_thid, io_tkt, tag, act, search_key,
                            cmp_func, &obj, &obj_len));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);
    if (obj) {
        if (obj_len != in_exp_obj_len)
            exh_Throw(RET_INTERNALERROR);
        if (out_num)
            *out_num = byte_BE1R(obj + L_OFFSET_KEY_INFO_NUM);
        if (out_found)
            *out_found = TRUE;
    } else {
        if (out_found)
            *out_found = FALSE;
        else
            exh_Throw(RET_KEYMNG_KEY_NOT_FOUND);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_DelContentKey(po_thid_t                       in_thid,
                cache_tkt_t                    *io_tkt,
                const po_key_ref_t             *in_key_ref,
                keymng_status_t                 in_status)
{
    bool_t            (*cmp_func)(const void *, const void *) = &l_CompareKeyRef;
    const void         *search_key = in_key_ref;
    cache_tag_t         tag;
    cache_attr_t        attr;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetCacheInfo(in_key_ref->action_id, in_status, &tag, &attr));

    exh_Set(cache_SetObject(in_thid, io_tkt, tag, attr, search_key,
                            cmp_func, NULL, 0));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetKeyRefList(po_thid_t                       in_thid,
                u_int8_t                        in_tag,
                u_int32_t                      *io_num,
                po_key_ref_t                   *out_key_ref_lst)
{
    exh_InitOk();
    MSGENT();

	dprintf("l_GetKeyRefList: io_num:%d out_key_ref_lst:%  in_thid:%d, in_tag:%d\n",
		*io_num, out_key_ref_lst, in_thid, in_tag);

    exh_Set(cache_GetObjectList(in_thid, in_tag, l_ParseKeyRefList,
                                io_num, out_key_ref_lst));
    if (!exh_IsOk()) {
        if (exh_Get() == RET_CACHE_BUFFER_SHORT)
            exh_Throw(RET_KEYMNG_BUFFER_SHORT);
        else
            exh_Throw(RET_INTERNALERROR);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
keymng_SetContentKey(po_thid_t                  in_thid,
                     const po_key_ref_t        *in_key_ref_lst,
                     const u_int8_t            *in_ckey_lst,
                     const po_key_info_t       *in_key_info_lst,
                     u_int32_t                  in_key_num,
                     const u_int8_t             in_srv_sbj[KEYMNG_SRV_SBJ_LEN],
                     u_int8_t                   in_trans_flag)
{
    cache_tkt_t ticket;
    u_int8_t    obj[L_OBJ_BIG_LEN];
    u_int8_t    obj_available[L_OBJ_LEN];
    u_int8_t    obj_uncommitted[L_OBJ_BIG_LEN];
    u_int32_t   i;
    bool_t      is_in_transaction = FALSE;
    bool_t      found;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref_lst || !in_ckey_lst || !in_key_info_lst)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    for (i = 0; i < in_key_num; i++) {
        const po_key_ref_t  *key_ref       = &in_key_ref_lst[i];
        const u_int8_t      *ckey          = &in_ckey_lst[i * PO_CKEY_LEN];
        const po_key_info_t *key_info      = &in_key_info_lst[i];
        u_int8_t            *p             = obj;
        u_int8_t             num_available = 1;

        /* serialize key reference */
        byte_CPYWI(p, key_ref->urr, PO_URR_LEN);
        byte_BE1WI(p, key_ref->urr_len);
        byte_BE1WI(p, key_ref->action_id);
        byte_BE1WI(p, key_ref->action_param);

        /* serialize key information */
        byte_BE1WI(p, key_info->flags);
        byte_CPYWI(p, key_info->ckey_id, PO_CKEY_ID_LEN);
        byte_BE2WI(p, key_info->priv_data);
        byte_CPYWI(p, key_info->tierbits, PO_TIERBITS_LEN);
        byte_BE2WI(p, key_info->oci);
        byte_CPYWI(p, key_info->export_info, PO_EXPORT_INFO_MAX_LEN);
        byte_BE2WI(p, key_info->export_info_len);
        byte_BE4WI(p, key_info->not_before);
        byte_BE4WI(p, key_info->not_after);
        byte_BE1WI(p, num_available);

        /* serialize content key */
        byte_CPYWI(p, ckey, PO_CKEY_LEN);

        if (key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL) {
            exh_Check(l_GetContentKey(in_thid, &ticket,
                                      key_ref, NULL, NULL,
                                      KEYMNG_STATUS_AVAILABLE,
                                      L_OBJ_LEN, obj_available,
                                      &num_available, &found));
            if (found) {
                if (num_available >= KEYMNG_MAX_LICENSES)
                    exh_Throw(RET_KEYMNG_TOO_MANY_LICENSES);

                if (!in_srv_sbj) num_available++;
            }
        }

        if (key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL && in_srv_sbj) {
            /* serialize srv_sbj and TF */
            byte_CPYWI(p, in_srv_sbj, KEYMNG_SRV_SBJ_LEN);
            byte_BE1WI(p, in_trans_flag);

            exh_Check(l_GetContentKey(in_thid, &ticket,
                                      key_ref, NULL, in_srv_sbj,
                                      KEYMNG_STATUS_UNCOMMITTED,
                                      L_OBJ_BIG_LEN, obj_uncommitted,
                                      NULL, &found));
            if (found) exh_Throw(RET_INTERNALERROR);

            exh_Check(l_SetContentKey(in_thid, &ticket,
                                      key_ref, key_info->ckey_id, in_srv_sbj,
                                      KEYMNG_STATUS_UNCOMMITTED,
                                      L_OBJ_BIG_LEN, obj, 1));
        } else {
            exh_Check(l_SetContentKey(in_thid, &ticket,
                                      key_ref, key_info->ckey_id, in_srv_sbj,
                                      KEYMNG_STATUS_AVAILABLE,
                                      L_OBJ_LEN, obj, num_available));
        }
    }

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            exh_Set(l_ConvErr(exh_Get()));
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_GetContentKey(po_thid_t                  in_thid,
                     const po_key_ref_t        *in_key_ref,
                     u_int8_t                  *out_ckey,
                     po_key_info_t             *out_key_info)
{
    cache_tkt_t ticket;
    u_int8_t    obj[L_OBJ_LEN];
    u_int8_t    num_available;
    u_int8_t    num_in_use = 1;
    bool_t      found;
    bool_t      is_in_transaction = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref || !out_ckey || !out_key_info)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    exh_Check(l_GetContentKey(in_thid, &ticket,
                              in_key_ref, NULL, NULL,
                              KEYMNG_STATUS_AVAILABLE,
                              L_OBJ_LEN, obj, &num_available, NULL));
    exh_Check(l_ParseKeyInfo(obj, out_key_info));
    exh_Check(l_ParseCkey(obj, out_ckey));

    if (in_key_ref->action_id == PO_ACTION_ID_EXTRACT_VOD ||
        in_key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL) {
        if (num_available == 0)
            exh_Throw(RET_KEYMNG_KEY_NOT_FOUND);

        exh_Check(l_GetContentKey(in_thid, &ticket,
                                  in_key_ref, NULL, NULL,
                                  KEYMNG_STATUS_INUSE,
                                  L_OBJ_LEN, obj, &num_in_use, &found));
        if (found) {
            if (num_in_use >= num_available)
                exh_Throw(RET_KEYMNG_KEY_IN_USE);

            num_in_use++;
        }

        exh_Check(l_SetContentKey(in_thid, &ticket,
                                  in_key_ref, NULL, NULL,
                                  KEYMNG_STATUS_INUSE,
                                  L_OBJ_LEN, obj, num_in_use));
    }

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            exh_Set(l_ConvErr(exh_Get()));
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_GetContentKeyById(po_thid_t              in_thid,
                         const u_int8_t        *in_ckey_id,
                         u_int8_t              *out_ckey,
                         po_key_info_t         *out_key_info)
{
    u_int8_t    obj[L_OBJ_LEN];

    exh_InitOk();
    MSGENT();

    if (!in_ckey_id || !out_ckey || !out_key_info)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(l_GetContentKey(in_thid, NULL,
                              NULL, in_ckey_id, NULL,
                              KEYMNG_STATUS_AVAILABLE,
                              L_OBJ_LEN, obj, NULL, NULL));
    exh_Check(l_ParseKeyInfo(obj, out_key_info));
    exh_Check(l_ParseCkey(obj, out_ckey));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_DelContentKey(po_thid_t                  in_thid,
                     const po_key_ref_t        *in_key_ref)
{
    cache_tkt_t ticket;
    u_int8_t    obj[L_OBJ_LEN];
    u_int8_t    num_available;
    u_int8_t    num_in_use = 0;
    bool_t      found;
    bool_t      is_in_transaction = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    if (in_key_ref->action_id == PO_ACTION_ID_EXTRACT_VOD ||
        in_key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL) {
        exh_Check(l_GetContentKey(in_thid, &ticket,
                                  in_key_ref, NULL, NULL,
                                  KEYMNG_STATUS_INUSE,
                                  L_OBJ_LEN, obj, &num_in_use, &found));

        /* Get the content key from avaialable tag */
        exh_Check(l_GetContentKey(in_thid, &ticket,
                                  in_key_ref, NULL, NULL,
                                  KEYMNG_STATUS_AVAILABLE,
                                  L_OBJ_LEN, obj, &num_available, &found));
        if (found) {
            if (num_available > 0 && num_available == num_in_use)
                exh_Throw(RET_KEYMNG_KEY_IN_USE);

            if (num_available > 1) {
                exh_Check(l_SetContentKey(in_thid, &ticket,
                                          in_key_ref, NULL, NULL,
                                          KEYMNG_STATUS_AVAILABLE,
                                          L_OBJ_LEN, obj, num_available - 1));
                exh_Throw(RET_OK);
            }
        } else {
            exh_Throw(RET_OK);
        }
    }

    exh_Check(l_DelContentKey(in_thid, &ticket, in_key_ref,
                              KEYMNG_STATUS_AVAILABLE));

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            exh_Set(l_ConvErr(exh_Get()));
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_CommitContentKey(po_thid_t               in_thid,
                        const po_key_ref_t     *in_key_ref_lst,
                        u_int32_t               in_key_num,
                        bool_t                  in_result)
{
    cache_tkt_t ticket;
    u_int8_t    obj[L_OBJ_LEN];
    u_int8_t    num_available;
    u_int8_t    num_in_use;
    u_int32_t   i;
    bool_t      found;
    bool_t      is_in_transaction = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref_lst)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    for (i = 0; i < in_key_num; i++) {
        const po_key_ref_t *key_ref = &in_key_ref_lst[i];

        if (key_ref->action_id == PO_ACTION_ID_EXTRACT_VOD ||
            key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL) {
            exh_Check(l_GetContentKey(in_thid, &ticket,
                                      key_ref, NULL, NULL,
                                      KEYMNG_STATUS_INUSE,
                                      L_OBJ_LEN, obj, &num_in_use, &found));
            if (!found) exh_Throw(RET_INTERNALERROR);

            if (num_in_use > 1) {
                exh_Check(l_SetContentKey(in_thid, &ticket,
                                          key_ref, NULL, NULL,
                                          KEYMNG_STATUS_INUSE,
                                          L_OBJ_LEN, obj,
                                          num_in_use - 1));
            } else {
                exh_Check(l_DelContentKey(in_thid, &ticket, key_ref,
                                          KEYMNG_STATUS_INUSE));
            }

            if (in_result) {
                if (key_ref->action_id == PO_ACTION_ID_EXTRACT_VOD) {
                    exh_Check(l_DelContentKey(in_thid, &ticket, key_ref,
                                              KEYMNG_STATUS_AVAILABLE));
                } else {
                    exh_Check(l_GetContentKey(in_thid, &ticket,
                                              key_ref, NULL, NULL,
                                              KEYMNG_STATUS_AVAILABLE,
                                              L_OBJ_LEN, obj,
                                              &num_available, &found));
                    if (!found) exh_Throw(RET_INTERNALERROR);

                    if (num_available == 0)
                        exh_Throw(RET_INTERNALERROR);

                    exh_Check(l_SetContentKey(in_thid, &ticket,
                                              key_ref, NULL, NULL,
                                              KEYMNG_STATUS_AVAILABLE,
                                              L_OBJ_LEN, obj,
                                              num_available - 1));
                }
            }
        }
    }

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            exh_Set(l_ConvErr(exh_Get()));
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_GetKeyInfo(po_thid_t                     in_thid,
                  const po_key_ref_t           *in_key_ref,
                  po_key_info_t                *out_key_info)
{
    u_int8_t    obj[L_OBJ_BIG_LEN];
    bool_t      found;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref || !out_key_info)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(l_GetContentKey(in_thid, NULL,
                              in_key_ref, NULL, NULL,
                              KEYMNG_STATUS_AVAILABLE,
                              L_OBJ_LEN, obj, NULL, NULL));
    exh_Check(l_ParseKeyInfo(obj, out_key_info));

    if ((in_key_ref->action_id == PO_ACTION_ID_EXTRACT_VOD) ||
        (in_key_ref->action_id == PO_ACTION_ID_EXPORT_CP_DL)) {
        exh_Check(l_GetContentKey(in_thid, NULL,
                                  in_key_ref, NULL, NULL,
                                  KEYMNG_STATUS_INUSE, L_OBJ_LEN, obj,
                                  &out_key_info->num_in_use, &found));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_GetKeyNum(po_thid_t                      in_thid,
                 u_int8_t                       in_action_id,
                 u_int32_t                     *out_num)
{
    cache_tag_t tag;

    exh_InitOk();
    MSGENT();

	
    if (!out_num)
        exh_Throw(RET_ILLEGALARGS);
	

    if (in_action_id == PO_ACTION_ID_ALL) {
        u_int32_t total = 0;
		

        for (tag = CACHE_TAG_VODKEY; tag <= CACHE_TAG_DLCPKEY; tag++) {
            u_int32_t num;
			

            exh_Check(cache_GetObjectNum(in_thid, tag, &num));

            total += num;
        }
		

        *out_num = total;
    } else {
    
        exh_Check(l_GetCacheInfo(in_action_id, KEYMNG_STATUS_AVAILABLE, &tag, NULL));
        exh_Check(cache_GetObjectNum(in_thid, tag, out_num));
    }
	
	dprintf("\n%s: out_num:%d \n", __FUNCTION__, *out_num);

exh_CLEANUP:
    MSGEXT(exh_Get());
	
    exh_Return();
}

int
keymng_GetKeyRefList(po_thid_t                  in_thid,
                     u_int8_t                   in_action_id,
                     u_int32_t                 *io_num,
                     po_key_ref_t              *out_key_ref_lst)
{
    cache_tag_t tag;

    exh_InitOk();
    MSGENT();

    if (!io_num || !out_key_ref_lst)
        exh_Throw(RET_ILLEGALARGS);

    if (in_action_id == PO_ACTION_ID_ALL) {
        u_int32_t     rest        = *io_num;
        po_key_ref_t *key_ref_lst = out_key_ref_lst;

        for (tag = CACHE_TAG_VODKEY; tag <= CACHE_TAG_DLCPKEY; tag++) {
            u_int32_t used = rest;

            exh_Check(l_GetKeyRefList(in_thid, tag, &used, key_ref_lst));

            key_ref_lst += used;
            rest        -= used;
        }

        *io_num -= rest;
    } else {
        exh_Check(l_GetCacheInfo(in_action_id, KEYMNG_STATUS_AVAILABLE, &tag, NULL));
        exh_Check(l_GetKeyRefList(in_thid, tag, io_num, out_key_ref_lst));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_GetTransactionFlag(po_thid_t             in_thid,
                          const u_int8_t       *in_srv_sbj,
                          u_int8_t             *out_trans_flag)
{
    u_int8_t    buf[L_OBJ_BIG_LEN];
    u_int8_t   *obj = buf;
    bool_t      found;

    exh_InitOk();
    MSGENT();

    if (!in_srv_sbj || !out_trans_flag)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(l_GetContentKey(in_thid, NULL,
                              NULL, NULL, in_srv_sbj,
                              KEYMNG_STATUS_UNCOMMITTED,
                              L_OBJ_BIG_LEN, obj, NULL, &found));
    if (found) {
        *out_trans_flag = byte_BE1R(obj + L_OFFSET_TRANS_FLAG);

        MSGI3(("Server Subject  : ")); PRINT3(KEYMNG_SRV_SBJ_LEN, in_srv_sbj); MSG3(("\n"));
        MSGI3(("Transaction Flag: %02x\n", *out_trans_flag));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_EnableContentKey(po_thid_t               in_thid,
                        const u_int8_t         *in_srv_sbj)
{
    cache_tkt_t     ticket;
    u_int8_t        buf[L_OBJ_BIG_LEN];
    u_int8_t       *obj     = buf;
    po_key_ref_t    key_ref;
    u_int8_t        num_available = 1;
    bool_t          found;
    bool_t          is_in_transaction = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_srv_sbj)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    exh_Check(l_GetContentKey(in_thid, &ticket,
                              NULL, NULL, in_srv_sbj,
                              KEYMNG_STATUS_UNCOMMITTED,
                              L_OBJ_BIG_LEN, obj, NULL, &found));
    if (!found) exh_Throw(RET_OK);

    exh_Check(l_ParseKeyRef(obj, &key_ref));

    if (key_ref.action_id == PO_ACTION_ID_EXPORT_CP_DL) {
        exh_Check(l_GetContentKey(in_thid, &ticket,
                                  &key_ref, NULL, NULL,
                                  KEYMNG_STATUS_AVAILABLE,
                                  L_OBJ_LEN, obj, &num_available, &found));
        if (found) {
            if (num_available >= KEYMNG_MAX_LICENSES)
                exh_Throw(RET_INTERNALERROR);

            num_available++;
        }
    }

    exh_Check(l_SetContentKey(in_thid, &ticket,
                              &key_ref, NULL, NULL,
                              KEYMNG_STATUS_AVAILABLE,
                              L_OBJ_LEN, obj, num_available));

    exh_Check(l_DelContentKey(in_thid, &ticket, &key_ref,
                              KEYMNG_STATUS_UNCOMMITTED));

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            exh_Set(l_ConvErr(exh_Get()));
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}

int
keymng_DelUncommittedKey(po_thid_t              in_thid,
                         const po_key_ref_t    *in_key_ref)
{
    cache_tkt_t ticket;
    bool_t      is_in_transaction = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_key_ref)
        exh_Throw(RET_ILLEGALARGS);

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    exh_Check(l_DelContentKey(in_thid, &ticket, in_key_ref,
                              KEYMNG_STATUS_UNCOMMITTED));

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            exh_Set(l_ConvErr(exh_Get()));
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    MSGEXT(exh_Get());
    exh_Return();
}
