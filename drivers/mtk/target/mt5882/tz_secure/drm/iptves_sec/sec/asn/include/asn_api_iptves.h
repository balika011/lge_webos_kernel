/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef ASN_API_H
#define ASN_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * ASN.1 Decode API
 *
 * require: base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define ASN_BOOLEAN_VAL_FALSE   0
#define ASN_BOOLEAN_VAL_TRUE    1

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    u_int8_t   tag;
    bptr_t     asn;
    bptr_t     val;
} asn_elm_t;

typedef struct {
    bool_t     sign;
    bptr_t     val;
} asn_int_t;

typedef struct {
    u_int8_t   unused;
    bptr_t     val;
} asn_bit_str_t;

typedef struct {
    int        sec;
    int        min;
    int        hour;
    int        mday;
    int        mon;
    int        year;
} asn_time_t;

enum asn_tag {
    ASN_TAG_BOOLEAN    = 0x01,
    ASN_TAG_INTEGER    = 0x02,
    ASN_TAG_BITSTRING  = 0x03,
    ASN_TAG_OSTRING    = 0x04,
    ASN_TAG_OID        = 0x06,
    ASN_TAG_UTF8STRING = 0x0C,
    ASN_TAG_UTCTIME    = 0x17,
    ASN_TAG_GTIME      = 0x18,
    ASN_TAG_SEQUENCE   = 0x30,
    ASN_TAG_SET        = 0x31,
};

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_ASN_PARSE_ERR_SIZE          (RET_ASN_BASE|1)
#define RET_ASN_PARSE_ERR_TAG           (RET_ASN_BASE|2)
#define RET_ASN_PARSE_ERR_FMT           (RET_ASN_BASE|3)
#define RET_ASN_PARSE_ERR_UNSUPPORTED   (RET_ASN_BASE|4)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 *
 *
 * @param [in]     in_data
 * @param [in]     in_data_sz
 * @param [in,out] io_elm
 * @param [in,out] io_next
 * @param [in,out] io_remain_sz
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_elm_Parse(const u_int8_t           *in_data,
              size_t                    in_data_sz,
              asn_elm_t                *io_elm,
              const u_int8_t          **io_next,
              size_t                   *io_remain_sz);

/**
 *
 *
 * @param [in]     in_data
 * @param [in,out] io_elm
 * @param [in,out] io_next
 * @param [in,out] io_remain_sz
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_elm_ParseBptr(const bptr_t         *in_data,
                  asn_elm_t            *io_elm,
                  const u_int8_t      **io_next,
                  size_t               *io_remain_sz);

/**
 *
 *
 * @param [in]     in_seq
 * @param [in,out] io_elms
 * @param [in,out] io_elms_num
 * @param [in,out] io_next
 * @param [in,out] io_remain_sz
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseSeq(const asn_elm_t           *in_seq,
             asn_elm_t                  io_elms[],
             int                       *io_elms_num,
             const u_int8_t           **io_next,
             size_t                    *io_remain_sz);

/**
 *
 *
 * @param [in]     in_seq
 * @param [in,out] io_elms
 * @param [in,out] io_elms_num
 * @param [in,out] io_next
 * @param [in,out] io_remain_sz
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseSet(const asn_elm_t           *in_seq,
             asn_elm_t                  io_elms[],
             int                       *io_elms_num,
             const u_int8_t           **io_next,
             size_t                    *io_remain_sz);

/**
 *
 *
 * @param [in]     in_data
 * @param [in,out] io_elms
 * @param [in,out] io_elms_num
 * @param [in,out] io_next
 * @param [in,out] io_remain_sz
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseSeqBptr(const bptr_t          *in_data,
                 asn_elm_t             *io_elms,
                 int                   *io_elms_num,
                 const u_int8_t       **io_next,
                 size_t                *io_remain_sz);

/**
 *
 *
 * @param [in]     in_data
 * @param [in]     in_data_sz
 * @param [in,out] io_elms
 * @param [in,out] io_elms_num
 * @param [in,out] io_next
 * @param [in,out] io_remain_sz
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseSeqStr(const u_int8_t         *in_data,
                size_t                  in_data_sz,
                asn_elm_t              *io_elms,
                int                    *io_elms_num,
                const u_int8_t        **io_next,
                size_t                 *io_remain_sz);

/* element parse APIs */

/**
 *
 *
 * @param [in]     in_elm
 * @param [in,out] io_flag
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseBoolean(const asn_elm_t       *in_elm,
                 bool_t                *io_flag);

/**
 *
 *
 * @param [in]     in_elm
 * @param [in,out] io_int
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseInteger(const asn_elm_t       *in_elm,
                 asn_int_t             *io_int);

/**
 *
 *
 * @param [in]     in_elm
 * @param [in,out] io_bstr
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseBitString(const asn_elm_t     *in_elm,
                   asn_bit_str_t       *io_bstr);

/**
 *
 *
 * @param [in]     in_elm
 * @param [in,out] io_time
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_ASN_PARSE_ERR_SIZE
 * @retval RET_ASN_PARSE_ERR_TAG
 * @retval RET_ASN_PARSE_ERR_FMT
 * @retval RET_ASN_PARSE_ERR_UNSUPPORTED
 */
extern int
asn_ParseTime(const asn_elm_t          *in_elm,
              asn_time_t               *io_time);

#ifdef __cplusplus
}
#endif

#endif /* ASN_API_H */
