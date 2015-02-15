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
#include "mem_api_iptves.h"
#include "mutex_api_iptves.h"
#include "byte_api_iptves.h"
#include "nvs_api.h"
#include "cache_api.h"

COMPONENT(CACHE);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_GET_TABLE(thid,tag)  (&l_cache_table[thid][tag])
#define L_GET_INF(attr)        (&l_inf_table[attr])

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct cache_elm_tag {
    struct cache_elm_tag       *next;
    struct cache_elm_tag       *updated;
    u_int8_t                    buf[1];
} cache_elm_t;

typedef struct {
    cache_elm_t                *get_lst;
    cache_elm_t                *set_lst;
    cache_attr_t                attr;
    u_int32_t                   num;
    u_int32_t                   len;
    u_int32_t                   updated_len;
} cache_tbl_t;

typedef struct {
    nvs_ftype_t                 file_type;
    bool_t                      dirty_flag;
} cache_inf_t;

typedef enum {
    CACHE_GET_LST = 0,
    CACHE_SET_LST,
} cache_lst_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/
static cache_tbl_t      l_cache_table[PO_THID_MAX][CACHE_TAG_MAX];
static cache_inf_t      l_inf_table[CACHE_ATTR_MAX] = {
    { NVS_FTYPE_READONLY,  FALSE },
    { NVS_FTYPE_READWRITE, FALSE },
    { -1,                  FALSE },
};
static mutex_t          l_set_lock[1];
static mutex_t          l_get_lock[1];
static cache_tkt_t     *l_active_ticket = NULL;

/*============================================================================
 * Local Functions
 *==========================================================================*/
static int
l_ConvErr(cache_attr_t          in_attr,
          int                 in_err)
{
    exh_InitOk();
    exh_Set(in_err);

    switch (exh_Get()) {
    case RET_NOMEM:
        exh_Rethrow();
    case RET_NVS_NOT_EXIST:
        if (in_attr == CACHE_ATTR_READONLY) {
            exh_Throw(RET_CACHE_RODATA_CORRUPTED);
        } else {
            CLEARLOG();
            exh_Throw(RET_OK);
        }
    case RET_NVS_OPEN_ERROR:
    case RET_NVS_READ_ERROR:
    case RET_NVS_VERIFY_ERROR:
        if (in_attr == CACHE_ATTR_READONLY) {
            exh_Throw(RET_CACHE_RODATA_CORRUPTED);
        } else {
            exh_Throw(RET_CACHE_LOAD_ERROR);
        }
    case RET_CACHE_INVALID_OBJECT:
        exh_Throw(RET_CACHE_PARSE_ERROR);
    default:
        exh_Throw(RET_INTERNALERROR);
    }

exh_CLEANUP:
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Element Management
 *--------------------------------------------------------------------------*/
static int
l_InitElement(const u_int8_t   *in_obj,
              u_int32_t         in_obj_len,
              cache_elm_t     **out_elm)
{
    cache_elm_t *elm = NULL;

    exh_InitOk();

    elm = (cache_elm_t *)mem_Calloc(1, sizeof(cache_elm_t) + in_obj_len - 1);
    if (!elm) exh_Throw(RET_NOMEM);

    if (in_obj)
        exh_Check(base_Memcpy(elm->buf, in_obj_len, in_obj, in_obj_len));
    elm->updated = elm; /* set own address (this means normal) */

    *out_elm = elm;

exh_CLEANUP:
    exh_Return();
}

static int
l_FinElement(cache_elm_t       *io_elm)
{
    exh_InitOk();

    if (io_elm)
        mem_Free(io_elm);

    exh_Return();
}

static int
l_AddElement(cache_tbl_t       *io_table,
             cache_lst_t        in_lst,
             cache_attr_t       in_attr,
             const u_int8_t    *in_obj,
             u_int32_t          in_obj_len)

{
    cache_elm_t *elm = NULL;

    exh_InitOk();

    if (io_table->num == 0) {
        io_table->attr = in_attr;
        io_table->len  = in_obj_len;
    } else {
        if (io_table->attr != in_attr)
            exh_Throw(RET_CACHE_INVALID_OBJECT);
        if (io_table->len  != in_obj_len)
            exh_Throw(RET_CACHE_INVALID_OBJECT);
    }

    exh_Check(l_InitElement(in_obj, in_obj_len, &elm));

    if (in_lst == CACHE_GET_LST) {
        elm->next         = io_table->get_lst;
        io_table->get_lst = elm;

        io_table->num++;
    } else {
        elm->next         = io_table->set_lst;
        io_table->set_lst = elm;
    }

exh_CLEANUP:
    exh_Return();
}

static int
l_UpdateElement(cache_tbl_t    *io_table,
                cache_lst_t     in_lst,
                cache_elm_t   **io_elm)
{
    cache_elm_t *elm     = *io_elm;
    cache_elm_t *updated = elm->updated;

    exh_InitOk();
    MSGENT();

    if (io_table->updated_len > 0) {
        io_table->len         = io_table->updated_len;
        io_table->updated_len = 0;
    }

    *io_elm       = updated;
    updated->next = elm->next;

    l_FinElement(elm);

    if (in_lst == CACHE_GET_LST) {
        /* do nothing */
    } else {
        io_table->num++;
    }

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_DelElement(cache_tbl_t       *io_table,
             cache_lst_t        in_lst,
             cache_elm_t      **io_elm)
{
    cache_elm_t *elm = *io_elm;

    exh_InitOk();
    MSGENT();

    *io_elm = elm->next;

    l_FinElement(elm);

    if (in_lst == CACHE_GET_LST) {
        io_table->num--;
    } else {
        /* do nothing */
    }

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetElement(cache_tbl_t       *in_table,
             cache_lst_t        in_lst,
             const void        *in_key,
             bool_t           (*in_comp)(const void *, const void *),
             cache_elm_t      **out_elm)
{
    cache_elm_t *elm;

    exh_InitOk();

    if (in_key && in_comp) {
        elm = in_lst == CACHE_GET_LST ? in_table->get_lst : in_table->set_lst;

        while (elm) {
            if ((*in_comp)(in_key, elm->buf))
                break;
            elm = elm->next;
        }
    } else {
        elm = in_table->get_lst;
    }

    *out_elm = elm;

    exh_Return();
}

static int
l_CommitElement(cache_tbl_t    *io_table,
                cache_lst_t     in_lst,
                cache_elm_t   **io_elm,
                bool_t          in_is_commit)
{
    cache_elm_t *elm     = *io_elm;
    cache_elm_t *updated = elm->updated;

    exh_InitOk();
    MSGENT();

    if (in_is_commit) {
        if (updated == NULL) {  /* deleted */
            exh_Check(l_DelElement(io_table, in_lst, io_elm));
        } else if (updated != elm) {    /* updated */
            exh_Check(l_UpdateElement(io_table, in_lst, io_elm));
        } else {
            if (in_lst == CACHE_GET_LST) {
                /* do nothing */
            } else {    /* added */
                io_table->num++;
            }
        }
    } else {
        if (updated != NULL && updated != elm)
            exh_Check(l_FinElement(updated));

        if (in_lst == CACHE_GET_LST) {
            elm->updated = elm;
        } else {
            exh_Check(l_DelElement(io_table, CACHE_SET_LST, io_elm));
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Initialization/Finalization
 *--------------------------------------------------------------------------*/
static int
l_Init(void)
{
    po_thid_t    thid;
    cache_tbl_t *table = NULL;
    cache_tag_t  tag;

    exh_InitOk();
    MSGENT();

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        for (tag = CACHE_TAG_TA; tag < CACHE_TAG_MAX; tag++) {
            table = L_GET_TABLE(thid, tag);

            while (table->get_lst)
                l_DelElement(table, CACHE_GET_LST, &table->get_lst);
            table->set_lst     = NULL;
            table->attr        = CACHE_ATTR_NONE;
            table->num         = 0;
            table->len         = 0;
            table->updated_len = 0;
        }
    }

    exh_Set(mutex_Init(l_set_lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    exh_Set(mutex_Init(l_get_lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    exh_Check(nvs_Init());

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Fin(void)
{
    po_thid_t    thid;
    cache_tbl_t *table = NULL;
    cache_tag_t  tag;

    exh_InitOk();
    MSGENT();

    exh_Check(nvs_Fin());

    exh_Set(mutex_Fin(l_get_lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    exh_Set(mutex_Fin(l_set_lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        for (tag = CACHE_TAG_TA; tag < CACHE_TAG_MAX; tag++) {
            table = L_GET_TABLE(thid, tag);

            while (table->get_lst)
                l_DelElement(table, CACHE_GET_LST, &table->get_lst);
            table->set_lst     = NULL;
            table->attr        = CACHE_ATTR_NONE;
            table->num         = 0;
            table->len         = 0;
            table->updated_len = 0;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Parse/Serialization
 *--------------------------------------------------------------------------*/
static int
l_Parse(cache_attr_t            in_attr,
        u_int8_t               *in_buf,
        u_int32_t               in_len)
{
    po_thid_t    thid;
    u_int8_t    *bp      = in_buf;
    u_int32_t    buf_len = in_len;
    cache_tbl_t *table   = NULL;
    cache_tag_t  tag;
    cache_attr_t attr;
    u_int32_t    len;
    u_int32_t    num;
    u_int32_t    i;

	u_int32_t j = 0;

    exh_InitOk();
    MSGENT();

	//dprintf(">>%s\n", __FUNCTION__);
	///DumpHex("in_buf", in_buf, in_len);
	
    while (buf_len > 16) {
        byte_BE1RI(bp, thid); buf_len -= 1;
        byte_BE3RI(bp, tag);  buf_len -= 3;
        byte_BE4RI(bp, attr); buf_len -= 4;
        byte_BE4RI(bp, len);  buf_len -= 4;
        byte_BE4RI(bp, num);  buf_len -= 4;

		//dprintf("\n[%d] thid:%d  ", j, thid);
		//dprintf("tag:%d  ", tag);
		//dprintf("attr:%d  len:%d  num:%d\n", attr, len, num);


        if (buf_len < len * num)
            exh_Throw(RET_CACHE_PARSE_ERROR);

        table = L_GET_TABLE(thid, tag);

        for (i = 0; i < num; i++) {
            exh_Set(l_AddElement(table, CACHE_GET_LST, attr, bp, len));
            if (!exh_IsOk()) exh_Throw(l_ConvErr(CACHE_ATTR_NONE, exh_Get()));

            bp += len; buf_len -= len;
        }


		j++;
		
    }
	
	//dprintf("\n\n\n");

    if (buf_len != 0)
        exh_Throw(RET_CACHE_PARSE_ERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Serialize(cache_attr_t        in_attr,
            u_int8_t          **out_buf,
            u_int32_t          *out_len)
{
    po_thid_t    thid;
    u_int8_t    *bp      = NULL;
    u_int8_t    *buf     = NULL;
    u_int32_t    buf_len = 0;
    cache_elm_t *elm     = NULL;
    cache_tbl_t *table   = NULL;
    cache_tag_t  tag;

    exh_InitOk();
    MSGENT();

    /* calculate buffer size */
    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        for (tag = CACHE_TAG_TA; tag < CACHE_TAG_MAX; tag++) {
            table = L_GET_TABLE(thid, tag);

            if (table->attr == in_attr && table->num > 0) {
                buf_len += (table->len * table->num) + (4 * sizeof(u_int32_t));
            }
        }
    }

    if (buf_len == 0)
        exh_Throw(RET_OK);

    bp = buf = (u_int8_t *)mem_Malloc(buf_len);
    if (!buf) exh_Throw(RET_NOMEM);

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        for (tag = CACHE_TAG_TA; tag < CACHE_TAG_MAX; tag++) {
            table = L_GET_TABLE(thid, tag);

            if (table->attr == in_attr && table->num > 0) {
                byte_BE1WI(bp, thid);
                byte_BE3WI(bp, tag);
                byte_BE4WI(bp, table->attr);
                byte_BE4WI(bp, table->len);
                byte_BE4WI(bp, table->num);

                for (elm = table->get_lst; elm; elm = elm->next) {
                    byte_CPYWI(bp, elm->buf, table->len);
                }
            }
        }
    }

    *out_buf = buf; buf = NULL;
    *out_len = buf_len;

exh_CLEANUP:
    if (buf) mem_Free(buf);

    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * NVS Management
 *--------------------------------------------------------------------------*/
static int
l_Load(cache_attr_t             in_attr)
{
    u_int8_t    *buf = NULL;
    u_int32_t    len = 0;
    cache_inf_t *inf;

    exh_InitOk();
    MSGENT();

    inf = L_GET_INF(in_attr);

    exh_Set(nvs_Load(inf->file_type, NULL, &len));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(in_attr, exh_Get()));

	dprintf("%s mallloc len:%d\n", __FUNCTION__, len);
    buf = (u_int8_t *)mem_Malloc(len);
    if (!buf) exh_Throw(RET_NOMEM);

    exh_Set(nvs_Load(inf->file_type, buf, &len));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(in_attr, exh_Get()));

    exh_Check(l_Parse(in_attr, buf, len));

exh_CLEANUP:
    if (buf) mem_Free(buf);

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Save(cache_attr_t             in_attr)
{
    u_int8_t    *buf = NULL;
    u_int32_t    len = 0;
    cache_inf_t *inf;

    exh_InitOk();
    MSGENT();

    inf = L_GET_INF(in_attr);

    if (inf->dirty_flag == FALSE)
        exh_Throw(RET_OK);

    exh_Check(l_Serialize(in_attr, &buf, &len));

    exh_Set(nvs_Save(inf->file_type, buf, len));
    if (!exh_IsOk()) {
        switch (exh_Get()) {
        case RET_NVS_OPEN_ERROR:
        case RET_NVS_WRITE_ERROR:
            exh_Throw(RET_CACHE_SAVE_ERROR);
        default:
            exh_Throw(RET_INTERNALERROR);
        }
    }

    inf->dirty_flag = FALSE;

exh_CLEANUP:
    if (buf) mem_Free(buf);

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Remove(cache_attr_t           in_attr)
{
    cache_inf_t *inf;

    exh_InitOk();
    MSGENT();

    inf = L_GET_INF(in_attr);

    exh_Set(nvs_CleanupNVS(inf->file_type));
    if (!exh_IsOk()) {
        switch (exh_Get()) {
        case RET_NVS_OPEN_ERROR:
            exh_Throw(RET_CACHE_REMOVE_ERROR);
        default:
            exh_Throw(RET_INTERNALERROR);
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
cache_Init(void)
{
    exh_InitOk();
    MSGENT();

    exh_Check(l_Init());
    exh_Check(l_Load(CACHE_ATTR_READONLY));
    exh_Check(l_Load(CACHE_ATTR_READWRITE));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_Fin(void)
{
    exh_InitOk();
    MSGENT();

    exh_Check(l_Save(CACHE_ATTR_READWRITE));
    exh_Check(l_Fin());

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_CleanupNVS(void)
{
    exh_InitOk();
    MSGENT();

    exh_Check(l_Remove(NVS_FTYPE_READWRITE));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_Begin(cache_tkt_t        *io_tkt)
{
    po_thid_t     thid;
    cache_tbl_t  *table = NULL;
    cache_tag_t   tag;

    exh_InitOk();
    MSGENT();

    if (io_tkt == NULL)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(mutex_Lock(l_set_lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    l_active_ticket = io_tkt;

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        for (tag = CACHE_TAG_TA; tag < CACHE_TAG_MAX; tag++) {
            table = L_GET_TABLE(thid, tag);

            table->set_lst = table->get_lst;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_Commit(cache_tkt_t       *io_tkt,
             bool_t             in_is_commit)
{
    po_thid_t     thid;
    cache_tbl_t  *table = NULL;
    cache_tag_t   tag;
    cache_elm_t **elm;
    cache_elm_t  *next;
    bool_t        is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    if (io_tkt == NULL)
        exh_Throw(RET_ILLEGALARGS);
    if (io_tkt != l_active_ticket)
        exh_Throw(RET_CACHE_PERMISSION_DENIED);

    exh_Set(mutex_Lock(l_get_lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    is_locked = TRUE;

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        for (tag = CACHE_TAG_TA; tag < CACHE_TAG_MAX; tag++) {
            table = L_GET_TABLE(thid, tag);

            elm  = &table->set_lst;

            while (*elm != table->get_lst) {
                next = (*elm)->next;

                exh_Check(l_CommitElement(table, CACHE_SET_LST,
                                          elm, in_is_commit));

                if (*elm != next)
                    elm = &(*elm)->next;
            }
            while (*elm != NULL) {
                next = (*elm)->next;

                exh_Check(l_CommitElement(table, CACHE_GET_LST,
                                          elm, in_is_commit));

                if (*elm != next)
                    elm = &(*elm)->next;
            }

            if (in_is_commit) {
                table->get_lst = table->set_lst;
                table->set_lst = NULL;
            } else {
                table->set_lst = NULL;
            }
        }
    }

    if (in_is_commit) {
        exh_Check(l_Save(NVS_FTYPE_READWRITE));
    } else {
        L_GET_INF(NVS_FTYPE_READWRITE)->dirty_flag = FALSE;
    }

exh_CLEANUP:
    l_active_ticket = NULL;

    if (is_locked) mutex_Unlock(l_get_lock);

    mutex_Unlock(l_set_lock);

    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_SetObject(po_thid_t       in_thid,
                cache_tkt_t    *io_tkt,
                cache_tag_t     in_tag,
                cache_attr_t    in_attr,
                const void     *in_key,
                bool_t        (*in_comp)(const void *, const void *),
                const u_int8_t *in_obj,
                u_int32_t       in_obj_len)
{
    cache_tbl_t *table;
    cache_elm_t *elm;
    cache_inf_t *inf;

    exh_InitOk();
    MSGENT();

    if (io_tkt == NULL ||
        in_tag < CACHE_TAG_TA || CACHE_TAG_MAX <= in_tag ||
        in_attr < CACHE_ATTR_READONLY || CACHE_ATTR_VOLATILE < in_attr ||
        (!in_key && in_comp) || (in_key && !in_comp) ||
        (!in_obj && in_obj_len != 0) || (in_obj && in_obj_len == 0))
        exh_Throw(RET_ILLEGALARGS);
    if (io_tkt != l_active_ticket)
        exh_Throw(RET_CACHE_PERMISSION_DENIED);

    table = L_GET_TABLE(in_thid, in_tag);
    inf   = L_GET_INF(in_attr);

    if (table->attr == CACHE_ATTR_READONLY)
        exh_Throw(RET_CACHE_PERMISSION_DENIED);

    exh_Check(l_GetElement(table, CACHE_SET_LST, in_key, in_comp, &elm));
    if (elm) {
        if (in_obj_len == 0) {  /* delete */
            if (elm->updated != NULL && elm->updated != elm)
                exh_Check(l_FinElement(elm->updated));

            elm->updated       = NULL;  /* set NULL address (this measn deleted) */
            table->updated_len = 0;
        } else if (in_obj_len == table->len || table->num == 1) {
            /* update */
            cache_elm_t *new_elm = NULL;

            if (elm->updated == elm) {  /* normal */
                if (in_obj_len == table->len &&
                    memcmp(in_obj, elm->buf, in_obj_len) == 0) {
                    /* need not to overwrite */
                    exh_Throw(RET_OK);
                }
            } else if (elm->updated == NULL) {  /* already deleted */
                /* need to update */
            } else {   /* already updated */
                if (in_obj_len == table->updated_len &&
                    memcmp(in_obj, elm->updated->buf, in_obj_len) == 0) {
                    /* need not to overwrite */
                    exh_Throw(RET_OK);
                }
            }

            exh_Check(l_InitElement(in_obj, in_obj_len, &new_elm));

            if (elm->updated != NULL && elm->updated != elm)
                exh_Check(l_FinElement(elm->updated));

            elm->updated       = new_elm;
            table->updated_len = in_obj_len;
        } else {
            exh_Throw(RET_CACHE_INVALID_OBJECT);
        }
    } else {
        if (in_obj_len == 0) {  /* do nothing */
            exh_Throw(RET_OK);
        } else {/* add */
            exh_Check(l_AddElement(table, CACHE_SET_LST, in_attr,
                                   in_obj, in_obj_len));
        }
    }

    inf->dirty_flag = TRUE;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_GetObject(po_thid_t       in_thid,
                cache_tkt_t    *io_tkt,
                cache_tag_t     in_tag,
                cache_act_t     in_act,
                const void     *in_key,
                bool_t        (*in_comp)(const void *, const void *),
                u_int8_t      **io_obj,
                u_int32_t      *io_obj_len)
{
    cache_tbl_t *table;
    cache_elm_t *elm;
    bool_t       is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    if (in_tag < CACHE_TAG_TA || CACHE_TAG_MAX <= in_tag ||
        in_act < CACHE_ACT_REFER || CACHE_ACT_COPY < in_act ||
        (!in_key && in_comp) || (in_key && !in_comp) ||
        !io_obj || !io_obj_len ||
        (*io_obj && *io_obj_len == 0) || (!*io_obj && *io_obj_len > 0))
        exh_Throw(RET_ILLEGALARGS);
    if (io_tkt != NULL && io_tkt != l_active_ticket)
        exh_Throw(RET_CACHE_PERMISSION_DENIED);

    table = L_GET_TABLE(in_thid, in_tag);

    if (table->attr == CACHE_ATTR_NONE) {
        *io_obj     = NULL;
        *io_obj_len = 0;

        exh_Throw(RET_OK);
    }

    if (table->attr != CACHE_ATTR_READONLY) {
        if (in_act == CACHE_ACT_REFER)
            exh_Throw(RET_CACHE_PERMISSION_DENIED);

        exh_Set(mutex_Lock(l_get_lock));
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

        is_locked = TRUE;
    }

    if (io_tkt == NULL) {
        exh_Check(l_GetElement(table, CACHE_GET_LST, in_key, in_comp, &elm));
    } else {
        exh_Check(l_GetElement(table, CACHE_SET_LST, in_key, in_comp, &elm));
        if (elm)
            elm = elm->updated;
    }
    if (elm) {
        if (in_act == CACHE_ACT_REFER) {/* refer */
            *io_obj     = elm->buf;
            *io_obj_len = table->len;
        } else {/* copy */
            if (*io_obj_len < table->len)
                exh_Throw(RET_CACHE_BUFFER_SHORT);

            exh_Check(base_Memcpy(*io_obj, table->len, elm->buf, table->len));
            *io_obj_len = table->len;
        }
    } else {
        *io_obj     = NULL;
        *io_obj_len = 0;
    }

exh_CLEANUP:
    if (is_locked) mutex_Unlock(l_get_lock);

    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_GetObjectNum(po_thid_t    in_thid,
                   cache_tag_t  in_tag,
                   u_int32_t   *out_num)
{
    cache_tbl_t *table;
    bool_t       is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    if (in_tag < CACHE_TAG_TA || CACHE_TAG_MAX <= in_tag || !out_num)
        exh_Throw(RET_ILLEGALARGS);

    table = L_GET_TABLE(in_thid, in_tag);

    if (table->attr != CACHE_ATTR_READONLY) {
        exh_Set(mutex_Lock(l_get_lock));
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

        is_locked = TRUE;
    }

    *out_num = table->num;

exh_CLEANUP:
    if (is_locked) mutex_Unlock(l_get_lock);

    MSGEXT(exh_Get());
    exh_Return();
}

int
cache_GetObjectList(po_thid_t   in_thid,
                    cache_tag_t in_tag,
                    bool_t    (*in_parse)(const void *, void *, u_int32_t),
                    u_int32_t  *io_num,
                    void       *out_obj_lst)
{
    cache_tbl_t *table;
    cache_elm_t *elm;
    u_int32_t    num = 0;
    void        *buf = out_obj_lst;
    bool_t       is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    if (in_tag < CACHE_TAG_TA || CACHE_TAG_MAX <= in_tag ||
        !in_parse || !io_num || !out_obj_lst)
        exh_Throw(RET_ILLEGALARGS);

    table = L_GET_TABLE(in_thid, in_tag);

    if (table->attr != CACHE_ATTR_READONLY) {
        exh_Set(mutex_Lock(l_get_lock));
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

        is_locked = TRUE;
    }

    for (elm = table->get_lst; elm; elm = elm->next) {
        if (*io_num <= num)
            exh_Throw(RET_CACHE_BUFFER_SHORT);

        if (!(*in_parse)(elm->buf, buf, num++))
            exh_Throw(RET_INTERNALERROR);
    }

    *io_num = num;

exh_CLEANUP:
    if (is_locked) mutex_Unlock(l_get_lock);

    MSGEXT(exh_Get());
    exh_Return();
}
