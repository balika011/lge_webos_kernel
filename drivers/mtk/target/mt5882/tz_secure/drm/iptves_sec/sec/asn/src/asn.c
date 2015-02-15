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
#include "bptr_api_iptves.h"
#include "asn_api_iptves.h"



COMPONENT(ASN);


/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_HEADER_SZ             2
#define L_BOOLEAN_VAL_SZ        1
#define L_BOOLEAN_VAL_FALSE     0
#define L_INTEGER_VAL_SZ_MIN    1
#define L_BITSTRING_VAL_SZ_MIN  2
#define L_TIMESTR_MIN_SZ        8
#define L_UTCTIME_MIN_SZ        11
#define L_GTIME_MIN_SZ          13

#define asn_l_IsDigit(c)        (((c) <= '9') && ((c) >= '0'))
#define asn_l_DToI(d)           ((d) - '0')

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for Debug
 *--------------------------------------------------------------------------*/
#if defined(ENABLE_DEBUG_FUNCTION)
static void
asn_elm_DebugPrint(const asn_elm_t     *in_elm)
{
    assert(NULL != in_elm);
    MSGI3(("===== asn_elm_t =====\n"));
    MSGI3((" tag: 0x%02x len: 0x%08x val_p: 0x%08x\n", in_elm->tag, in_elm->val.sz, in_elm->val.p));
    MSGI3(("----- val -----\n")); DUMP3(in_elm->val.sz, in_elm->val.p);
    return;
}
#endif /* ENABLE_DEBUG_FUNCTION */

static int
asn_l_2DToI(const u_int8_t             *p,
            int                        *io_v,
            const u_int8_t            **io_next)
{
    exh_InitOk();

    if (!asn_l_IsDigit(p[0]) || (!asn_l_IsDigit(p[1])))
        exh_Throw(RET_ASN_PARSE_ERR_FMT);

    *io_v = ((asn_l_DToI(p[0]) * 10) + asn_l_DToI(p[1]));

    if (io_next) *io_next = (p + 2);

exh_CLEANUP:
    exh_Return();
}

static int
l_ParseStruct(const bptr_t             *in_seq,
              asn_elm_t                 io_elms[],
              int                      *io_elms_num,
              const u_int8_t          **io_next,
              size_t                   *io_remain_sz)
{
    const u_int8_t *p;
    size_t    sz;
    int       i, max_elm_num;

    exh_InitOk();
    MSGENT();

    max_elm_num = *io_elms_num;

    p  = in_seq->p;
    sz = in_seq->sz;

    for (i = 0; (i < max_elm_num) && (0 != sz); i++) {
        exh_Check(asn_elm_Parse(p, sz, io_elms + i, &p, &sz));
    }

    *io_elms_num = i;

    if (io_next) *io_next = p;
    if (io_remain_sz) *io_remain_sz = sz;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseTimeStr(const u_int8_t          *in_str,
               size_t                   in_str_sz,
               asn_time_t              *io_time,
               const u_int8_t         **io_next,
               size_t                  *io_remain_sz)
{
    const u_int8_t *p;
    int mon;
    size_t sz;

    exh_InitOk();
    MSGENT();

    if (L_TIMESTR_MIN_SZ > in_str_sz)
        exh_Throw(RET_ASN_PARSE_ERR_FMT);

    p = in_str;
    sz = in_str_sz;

    exh_Check(asn_l_2DToI(p, &mon, &p));

    io_time->mon  = mon;

    exh_Check(asn_l_2DToI(p, &io_time->mday, &p));
    exh_Check(asn_l_2DToI(p, &io_time->hour, &p));
    exh_Check(asn_l_2DToI(p, &io_time->min, &p));

    sz -= L_TIMESTR_MIN_SZ;

    if (asn_l_IsDigit(*p)) {
        exh_Check(asn_l_2DToI(p, &io_time->sec, &p));

        sz -= 2;
    } else {
        io_time->sec = 0;
    }

    if ((io_time->mon == 0 || io_time->mon > 12)
        || (io_time->mday > 31)
        || (io_time->hour > 23)
        || (io_time->min  > 59)
        || (io_time->sec  > 61))
        exh_Throw(RET_ASN_PARSE_ERR_FMT);

    if (io_next) *io_next = p;
    if (io_remain_sz) *io_remain_sz = sz;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseUTCTime(const asn_elm_t         *in_elm,
               asn_time_t              *io_time)
{
    const u_int8_t *p;
    int year;
    size_t sz;

    exh_InitOk();
    MSGENT();

    if (ASN_TAG_UTCTIME != in_elm->tag)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);
    if (L_UTCTIME_MIN_SZ > in_elm->val.sz)
        exh_Throw(RET_ASN_PARSE_ERR_FMT);

    MSGI3(("[Step1] parse year\n"));

    p = in_elm->val.p;

    exh_Check(asn_l_2DToI(p, &year, &p));

    if (year < 50) year += 100;

    io_time->year = year + 1900;

    exh_Check(l_ParseTimeStr(p, in_elm->val.sz - 2,  io_time, &p, &sz));

    if ('+' == *p || '-' == *p)
        exh_Throw(RET_ASN_PARSE_ERR_UNSUPPORTED);

    if ('Z' == *p) {
        sz--;
    } else {
        exh_Throw(RET_ASN_PARSE_ERR_FMT);
    }

    if (0 != sz) exh_Throw(RET_ASN_PARSE_ERR_FMT);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseGTime(const asn_elm_t           *in_elm,
             asn_time_t                *io_time)
{
    const u_int8_t *p;
    int year2, year;
    size_t sz;

    exh_InitOk();
    MSGENT();

    if (ASN_TAG_GTIME != in_elm->tag)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);
    if (L_GTIME_MIN_SZ > in_elm->val.sz)
        exh_Throw(RET_ASN_PARSE_ERR_FMT);

    p = in_elm->val.p;

    exh_Check(asn_l_2DToI(p, &year2, &p));
    exh_Check(asn_l_2DToI(p, &year, &p));

    year += year2 * 100;

    if (1900 > year)
        exh_Throw(RET_ASN_PARSE_ERR_UNSUPPORTED);

    io_time->year = year;

    exh_Check(l_ParseTimeStr(p, in_elm->val.sz - 4,  io_time, &p, &sz));

    if ('+' == *p || '-' == *p || '.' == *p)
        exh_Throw(RET_ASN_PARSE_ERR_UNSUPPORTED);

    if ('Z' == *p) {
        sz--;
    } else {
        exh_Throw(RET_ASN_PARSE_ERR_FMT);
    }

    if (0 != sz)
        exh_Throw(RET_ASN_PARSE_ERR_FMT);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
asn_elm_Parse(const u_int8_t           *in_data,
              size_t                    in_data_sz,
              asn_elm_t                *io_elm,
              const u_int8_t          **io_next,
              size_t                   *io_remain_sz)
{
    const u_int8_t *p;
    size_t elm_len;
    size_t len_len;
    size_t elm_total_len;
    size_t i;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_data && NULL != io_elm);

    /* TAG and Size must exist */
    if (in_data_sz < L_HEADER_SZ)
        exh_Throw(RET_ASN_PARSE_ERR_SIZE);

    p = in_data;

    io_elm->tag = *p++;

    MSGI3(("[Step1] Tag=0x%02x\n", io_elm->tag));

    elm_len = 0;
    len_len = *p++;

    if (len_len < 0x80) {
        elm_len = len_len;
        len_len = 0;
    } else {
        len_len &= 0x7F;
        MSGI3(("[Step2] length of length=0x%02x\n", len_len));

        if ((len_len + L_HEADER_SZ) > in_data_sz)
            exh_Throw(RET_ASN_PARSE_ERR_SIZE);

        for (i = 0 ; i < len_len ; i++) {
            elm_len = elm_len << 8;
            elm_len += *p++;
        }
    }

    MSGI3(("[Step3] length =0x%08x\n", elm_len));

    elm_total_len = elm_len + len_len + L_HEADER_SZ;

    if (elm_total_len > in_data_sz) exh_Throw(RET_ASN_PARSE_ERR_SIZE);

    io_elm->asn.p  = in_data;
    io_elm->asn.sz = elm_total_len;
    io_elm->val.p  = p;
    io_elm->val.sz = elm_len;

    if (io_next) *io_next = p + elm_len;

    if (io_remain_sz) *io_remain_sz = in_data_sz - elm_total_len;

    /* for debug */
    RUN2(asn_elm_DebugPrint(io_elm));

    MSGI3(("remain_sz  :0x%08x\n", in_data_sz - elm_total_len));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
asn_elm_ParseBptr(const bptr_t         *in_data,
                  asn_elm_t            *io_elm,
                  const u_int8_t      **io_next,
                  size_t               *io_remain_sz)
{
    return asn_elm_Parse(in_data->p, in_data->sz, io_elm, io_next, io_remain_sz);
}

int
asn_ParseSeq(const asn_elm_t           *in_seq,
             asn_elm_t                  io_elms[],
             int                       *io_elms_num,
             const u_int8_t           **io_next,
             size_t                    *io_remain_sz)
{
    exh_InitOk();

    assert(NULL != in_seq && NULL != io_elms && NULL != io_elms_num);

    if (in_seq->tag != ASN_TAG_SEQUENCE)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);

    exh_Check(l_ParseStruct(&in_seq->val, io_elms, io_elms_num, io_next, io_remain_sz));

exh_CLEANUP:
    exh_Return();
}

int
asn_ParseSet(const asn_elm_t           *in_seq,
             asn_elm_t                  io_elms[],
             int                       *io_elms_num,
             const u_int8_t           **io_next,
             size_t                    *io_remain_sz)
{
    exh_InitOk();

    assert(NULL != in_seq && NULL != io_elms && NULL != io_elms_num);

    if (in_seq->tag != ASN_TAG_SET)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);

    exh_Check(l_ParseStruct(&in_seq->val, io_elms, io_elms_num, io_next, io_remain_sz));

exh_CLEANUP:
    exh_Return();
}

int
asn_ParseSeqBptr(const bptr_t          *in_data,
                 asn_elm_t             *io_elms,
                 int                   *io_elms_num,
                 const u_int8_t       **io_next,
                 size_t                *io_remain_sz)
{
    asn_elm_t elm;
    size_t    sz;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_data && NULL != io_elms && NULL != io_elms_num);

    exh_Check(asn_elm_Parse(in_data->p, in_data->sz, &elm, io_next, &sz));

    *io_remain_sz = sz;

    exh_Set(asn_ParseSeq(&elm, io_elms, io_elms_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz) exh_Throw(RET_ASN_PARSE_ERR_FMT);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
asn_ParseSeqStr(const u_int8_t         *in_data,
                size_t                  in_data_sz,
                asn_elm_t              *io_elms,
                int                    *io_elms_num,
                const u_int8_t        **io_next,
                size_t                 *io_remain_sz)
{
    bptr_t asn;

    assert(NULL != in_data && NULL != io_elms && NULL != io_elms_num);

    asn.p  = in_data;
    asn.sz = in_data_sz;

    return asn_ParseSeqBptr(&asn, io_elms, io_elms_num, io_next, io_remain_sz);
}

int
asn_ParseBoolean(const asn_elm_t       *in_elm,
                 bool_t                *io_flag)
{
    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_flag);

    if (in_elm->tag != ASN_TAG_BOOLEAN)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);

    if (in_elm->val.sz != L_BOOLEAN_VAL_SZ)
        exh_Throw(RET_ASN_PARSE_ERR_SIZE);

    if (L_BOOLEAN_VAL_FALSE == *in_elm->val.p) {
        *io_flag = FALSE;
    } else {
        *io_flag = TRUE;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
asn_ParseInteger(const asn_elm_t       *in_elm,
                 asn_int_t             *io_int)
{
    const u_int8_t *p;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_int);

    if (in_elm->tag != ASN_TAG_INTEGER)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);

    if (in_elm->val.sz < L_INTEGER_VAL_SZ_MIN)
        exh_Throw(RET_ASN_PARSE_ERR_SIZE);

    p = in_elm->val.p;

    io_int->val.p  = p;
    io_int->val.sz = in_elm->val.sz;

    MSGI3(("----- val -----\n")); DUMP3(in_elm->val.sz, p);

    if (0 == (0x80 & *p)) {
        MSGI3(("sign : TRUE\n"));
        io_int->sign = TRUE;

        if (0 == *p && 1 != io_int->val.sz) {
            io_int->val.p++;
            io_int->val.sz--;
        }
    } else {
        MSGI3(("sign : FALSE\n"));
        io_int->sign = FALSE;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
asn_ParseBitString(const asn_elm_t     *in_elm,
                   asn_bit_str_t       *io_bstr)
{
    const u_int8_t *p;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_bstr);

    if (in_elm->tag != ASN_TAG_BITSTRING)
        exh_Throw(RET_ASN_PARSE_ERR_TAG);

    if (in_elm->val.sz < L_BITSTRING_VAL_SZ_MIN)
        exh_Throw(RET_ASN_PARSE_ERR_SIZE);

    p = in_elm->val.p;

    io_bstr->unused = *p++;
    io_bstr->val.p  = p;
    io_bstr->val.sz = in_elm->val.sz - 1;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
asn_ParseTime(const asn_elm_t          *in_elm,
              asn_time_t               *io_time)
{
    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_time);

    switch (in_elm->tag) {
    case ASN_TAG_UTCTIME:
        exh_Check(l_ParseUTCTime(in_elm, io_time));
        break;

    case ASN_TAG_GTIME:
        exh_Check(l_ParseGTime(in_elm, io_time));
        break;

    default:
        exh_Throw(RET_ASN_PARSE_ERR_TAG);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
