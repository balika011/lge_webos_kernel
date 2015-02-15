/** @addtogroup asn asn
 *  @ingroup common
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef ASN_API_H
#define ASN_API_H

#ifdef __cplusplus
extern "C" {
#endif


    /*===========================================================================*/
    /** @file
     * @brief ASN.1 Encode/Decode
     *
     * dependency: base, mem
     *
     * @author SAKAMOTO Takahiro <takas@sm.sony.co.jp>
     * @date   25 May, 2005
     */
    /*===========================================================================*/

    /**
     * ASN error codes
     */

#define ERR_ASN_INPUT           (ERR_ASN_BASE|1)
#define ERR_ASN_INDEFLEN        (ERR_ASN_BASE|2)
#define ERR_ASN_SIZE            (ERR_ASN_BASE|3)
#define ERR_ASN_OVERFLOW        (ERR_ASN_BASE|4)
#define ERR_ASN_TYPE            (ERR_ASN_BASE|5)
#define ERR_ASN_FORMAT          (ERR_ASN_BASE|6)

    /*
     * defs
     */

#define ASN_ELEM_ID_SZ               (1)
#define ASN_ELEM_LEN_ID_SZ           (1)
#define ASN_ELEM_MIN_SEQ_SZ          (ASN_ELEM_ID_SZ + ASN_ELEM_LEN_ID_SZ)

#define ASN_MAX_NUM_LEN_OCTS         (4)

#define ASN_INDEF_ELEM_ID_SZ         (ASN_ELEM_ID_SZ)
#define ASN_INDEF_ELEM_LEN_SZ        (ASN_ELEM_LEN_ID_SZ)
#define ASN_INDEF_ELEM_END_SZ        (2)

#define ASN_CLASS_BITOFF             (6)
#define ASN_CLASS_BITMSK             (0x3)
#define ASN_CLASS_GET(identifer_octet)             \
    (((identifer_octet) >> ASN_CLASS_BITOFF) & ASN_CLASS_BITMSK)

#define ASN_CLASS_IDENTIFER_BITOFF   (5)
#define ASN_CLASS_IDENTIFER_GET(identifer_octet)   \
    ((identifer_octet) & (1 << ASN_CLASS_IDENTIFER_BITOFF))

#define ASN_TAG_BITOFF               (0)
#define ASN_TAG_BITMSK               (0x1F)
#define ASN_TAG_GET(identifer_octet)               \
    (((identifer_octet) >> ASN_TAG_BITOFF) & ASN_TAG_BITMSK)

    enum _asn_class {
        ASN_CLASS_UNIVERSAL       = 0x00,
        ASN_CLASS_APPLICATION     = 0x01,
        ASN_CLASS_CONTEXT         = 0x02,
        ASN_CLASS_PRIVATE         = 0x03
    };

    enum _asn_class_identifer {
        ASN_CLASS_PRIMITIVE       = 0x00,
        ASN_CLASS_CONSTRUCTED     = 0x20
    };

    enum _asn_len_form {
        ASN_LEN_FORM_DEF          = 0x00,
        ASN_LEN_FORM_INDEF        = 0x80
    };

    enum _asn_values {
        ASN_TAG_BOOLEAN           = 0x01,
        ASN_TAG_INTEGER           = 0x02,
        ASN_TAG_BIT_STRING        = 0x03,
        ASN_TAG_OCTET_STRING      = 0x04,
        ASN_TAG_NULL              = 0x05,
        ASN_TAG_OBJECT_IDENTIFIER = 0x06,
        ASN_TAG_OBJECT_DESCRIPTOR = 0x07,
        ASN_TAG_EXTERNAL          = 0x08,
        ASN_TAG_REAL              = 0x09,
        ASN_TAG_ENUMERATED        = 0x0A,
        ASN_TAG_UTF8_STRING       = 0x0C,
        ASN_TAG_SEQUENCE          = 0x10,
        ASN_TAG_SET               = 0x11,
        ASN_TAG_NUMERIC_STRING    = 0x12,
        ASN_TAG_PRINTABLE_STRING  = 0x13,
        ASN_TAG_TELETEX_STRING    = 0x14,
        ASN_TAG_VIDEOTEX_STRING   = 0x15,
        ASN_TAG_IA5_STRING        = 0x16,
        ASN_TAG_UTC_TIME          = 0x17,
        ASN_TAG_GENERALIZED_TIME  = 0x18,
        ASN_TAG_GRAPHIC_STRING    = 0x19,
        ASN_TAG_VISIBLE_STRING    = 0x1A,
        ASN_TAG_GENERAL_STRING    = 0x1B,
        ASN_TAG_CHARACTER_STRING  = 0x1C,
        ASN_TAG_MAX               = 0x1F
    };

    /*
     * types
     */

    /**
     * ASN structure based on TLV (Type-Length-Value) structure
     */
    typedef struct _asn {
        /* Type */
        unsigned char clazz;        /**< Class type according to ASN specs */
        unsigned char stflag;       /**< P/C flag according to ASN specs (Primitive/Constructed) */
        unsigned char tag;          /**< Object tag according to ASN specs */
        unsigned char flags;        /**< internal usage flag */

        np_size_t length;           /**< Length */

        union _v {
            unsigned char *body;    /**< Primitive value */
            struct _asn **seq;      /**< Sequence value (Constructed) */
        } v;                        /**< Value */

        unsigned char *_src_addr;   /**< Original buffer before parsing */
        u_int32_t      _src_size;   /**< Original buffer size */
    } asn_t;

    typedef struct _asn_time {
        /* time_t      time; */
        time_unix_t    time;
        int32_t        msec;
    } asn_time_t;

#define ASN_ST_CHK(av, idx)       ((av) != NULL && (av)->stflag && (av)->length > (idx) && (av)->v.seq != NULL)
#define ASN_ST_AT(av, idx)        (ASN_ST_CHK(av, idx) ? (av)->v.seq[(idx)] : NULL)
#define ASN_BODY(av)              (((av) != NULL && !(av)->stflag && (av)->length > 0) ? (av)->v.body : NULL)
#define ASN_BODY_CHECK(av)        ((av) != NULL && !(av)->stflag && (av)->length > 0 && (av)->v.body != NULL)
#define ASN_CHECK(av, _clz, _stf, _tag) ((av) != NULL && (av)->clazz == (_clz) && (((av)->stflag?1:0) == ((_stf)?1:0)) && (av)->tag == (_tag))

    /*---------------------------------------------------------------------------*
     * prototype
     *---------------------------------------------------------------------------*/

    /*
     * encode, decode
     */

    /**
     * @brief Return the size of the flat representation of the given asn_t
     * structure. Do not encode the data any more if the asn_t structure is
     * already encoded.
     *
     * @param[in]  in_asn   given asn_t structure
     * @param[out] out_sz   corresponding size
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_EncodeGetSize(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        np_size_t       *out_sz);

    /**
     * @brief Return the flat representation of the given asn_t structure as
     * well as its size. The output buffer is allocated by the function.
     * Thus, the caller should free it when not needed anymore.
     *
     * @param[in]  in_asn   given asn_t structure
     * @param[out] out_buf  corresponding flat representation
     * @param[out] out_sz   size of out_buf
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_INDEFLEN
     */
    retcode_t
    npi_asn_Encode(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        u_int8_t       **out_buf,
        np_size_t       *out_sz);

    /**
     * @brief Return the flat representation of the given asn_t structure as
     * well as its size. The output buffer is provided by the caller.
     *
     * @param[in]  in_asn   given asn_t structure
     * @param[out] out_buf  corresponding flat representation
     * @param[out] out_sz   size of out_buf
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_INDEFLEN
     */
    retcode_t
    npi_asn_EncodeWoMalloc(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        u_int8_t        *out_buf,
        np_size_t       *out_sz);

    /**
     * @brief Given an ASN.1 encoded buffer, return its asn_t
     * representation. Also return a pointer to the end of the encoding,
     * within in_buf. The output buffer is allocated by the
     * function. Thus, the caller should free it when not needed anymore.
     *
     * @param[in]  in_buf       ASN.1 encoded buffer
     * @param[in]  in_bufsize   size of in_buf
     * @param[out] out_next     pointer to the end of the encoding in in_buf
     * @param[out] out_asn      corresponding asn_t representation
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_INDEFLEN
     * @return ::ERR_ASN_OVERFLOW
     * @return ::ERR_ASN_SIZE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_Decode(
        npi_cmncontext_t in_context,
        u_int8_t        *in_buf,
        np_size_t        in_bufsize,
        u_int8_t       **out_next,
        asn_t          **out_asn);

    /**
     * @brief Given an ASN.1 encoded buffer, return its asn_t
     * representation. Also return a pointer to the end of the encoding,
     * within in_buf. The output buffer is allocated by the
     * function. Thus, the caller should free it when not needed anymore.
     *
     * @param[in]  in_buf       ASN.1 encoded buffer
     * @param[in]  in_bufsize   size of in_buf
     * @param[out] out_next     pointer to the end of the encoding in in_buf
     * @param[out] out_asn      corresponding asn_t representation
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_INDEFLEN
     * @return ::ERR_ASN_OVERFLOW
     * @return ::ERR_ASN_SIZE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_DecodeWoCopy(
        npi_cmncontext_t in_context,
        u_int8_t        *in_buf,
        np_size_t        in_bufsize,
        u_int8_t       **out_next,
        asn_t          **out_asn);

    /**
     * @brief Given an ASN.1 encoded buffer, parse it. Return a pointer to
     * the end of the encoding, within in_buf.
     *
     * @param[in]  in_buf       ASN.1 encoded buffer
     * @param[in]  in_bufsize   size of in_buf
     * @param[out] out_next     pointer to the end of the encoding in in_buf
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_INDEFLEN
     * @return ::ERR_ASN_OVERFLOW
     * @return ::ERR_ASN_SIZE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_DecodeCheck(
        npi_cmncontext_t in_context,
        u_int8_t        *in_buf,
        np_size_t        in_bufsize,
        u_int8_t       **out_next);

    /**
     * @brief Given an ASN.1 encoded element parse it and extract the following
     * pieces of information on it:
     * + The pointer to the next nested element.
     * + The length of the content data associated asn element
     * + The length of the element itself including the content and header
     *
     * @param[in]  in_elem         ASN.1 encoded buffer
     * @param[in]  in_elem_sz      Size of parent element
     * @param[out] out_next_elem   Start of the content associated with the element
     * @param[out] out_content_len  Length of the elements content
     * @param[out] out_elem_len     Length of the element
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_INDEFLEN
     * @return ::ERR_ASN_OVERFLOW
     * @return ::ERR_ASN_SIZE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_ElemInfoGet(
        npi_cmncontext_t in_context,
        u_int8_t        *in_elem,
        np_size_t        in_elem_sz,
        u_int8_t       **out_next_elem,
        np_size_t       *out_content_len,
        np_size_t       *out_elem_len);

    /*
     * control
     */

    /**
     * @brief Mark the given asn_t structure as definite (in_bool <= 0) or
     * indefinite (in_bool > 0).
     *
     * @param[in,out] io_asn      given asn_t structure
     * @param[in]     in_bool     flag to determine definite (<=0) or indefinite (>0)
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_SetDefinite(
        npi_cmncontext_t in_context,
        asn_t           *io_asn,
        int32_t          in_bool);

    /**
     * @brief Check whether the given asn_t structure is marked as
     * definite or indefinite.
     *
     * @param[in] in_asn     given asn_t structure
     *
     * @return a value equal to zero for indefinite and bigger than zero otherwise
     */
    int32_t
    npi_asn_GetDefinite(
        npi_cmncontext_t in_context,
        asn_t           *in_asn);

    /**
     * @brief Clone the give asn_t structure.
     *
     * @param[in]  in_asn     given asn_t structure to clone
     * @param[out] out_asn    new copied asn_t
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_Clone(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        asn_t          **out_asn);

    /*
     * new, free
     */

    /**
     * @brief Create a new asn_t structure and set the input buffer and
     * size into it.
     *
     * @param[in] in_src_size    size of data in in_src_addr
     * @param[in] in_src_addr    data to set
     * @param[out] out_asn       new asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewEncoded(
        npi_cmncontext_t in_context,
        np_size_t        in_src_size,
        void            *in_src_addr,
        asn_t          **out_asn);

    /**
     * @brief Create a new asn_t structure and initialize it with the given
     * values.
     *
     * @param[in]  in_class     class
     * @param[in]  in_stflags   simple type flag
     * @param[in]  in_tag       tag
     * @param[in]  in_length    size of in_body
     * @param[in]  in_body      structure body
     * @param[out] out_aval     generated asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewRaw(
        npi_cmncontext_t in_context,
        int32_t          in_class,
        int32_t          in_stflag,
        int32_t          in_tag,
        np_size_t        in_length,
        void            *in_body,
        asn_t          **out_aval);

    /**
     * @brief Create a new asn_t structure and initialize it with the
     * given values.
     *
     * @param[in]  in_class      class
     * @param[in]  in_tag        tag
     * @param[out] out_aval      generated asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewStructured(
        npi_cmncontext_t in_context,
        int32_t          in_class,
        int32_t          in_tag,
        asn_t          **out_aval);

    /**
     * @brief Create a new asn_t structure and initialize it with the
     * given values.
     *
     * @param[in]  in_class     class
     * @param[in]  in_tag       tag
     * @param[in]  in_length    size of in_body
     * @param[in]  in_body      structure body
     * @param[out] out_aval     generated asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewUnstructured(
        npi_cmncontext_t in_context,
        int32_t          in_class,
        int32_t          in_tag,
        np_size_t        in_length,
        void            *in_body,
        asn_t          **out_aval);

    /**
     * @brief Free the given asn_t structure.
     *
     * @param[in] in_asn   given asn_t structure
     *
     * @return ::ERR_OK
     */
    retcode_t
    npi_asn_Free(
        npi_cmncontext_t in_context,
        asn_t           *in_asn);

    /**
     * @brief Free the given buffer.
     *
     * @param[in] in_buf
     *
     * @return ::ERR_OK
     */
    retcode_t
    npi_asn_EncodeFree(
        npi_cmncontext_t in_context,
        u_int8_t        *in_buf);


    /*
     * structured operations
     */

    /**
     * @brief Insert the given in_asn into io_asn at the given index.
     *
     * @param[in, out] io_asn      asn_t to insert into
     * @param[in]      in_index    index at which to insert
     * @param[in]      in_asn      asn_t to insert
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_Insert(
        npi_cmncontext_t in_context,
        asn_t           *io_asn,
        int32_t          in_index,
        asn_t           *in_asn);

    /**
     * @brief Append in_asn to io_asn.
     *
     * @param[in, out] io_asn    asn_t to append to
     * @param[in]      in_asn    asn_t to append
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_Append(
        npi_cmncontext_t in_context,
        asn_t           *io_asn,
        asn_t           *in_asn);

    /**
     * @brief Remove element from the given index.
     *
     * @param[in, out] io_asn     asn_t to remove from
     * @param[in]      in_index   index to remove from
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_Remove(
        npi_cmncontext_t in_context,
        asn_t           *io_asn,
        int32_t          in_index);

    /**
     * @brief Insert the given in_asn into io_asn at the given index and
     * return the former element.
     *
     * @param[in, out] io_asn        asn_t to insert into
     * @param[in]      in_index      index to insert at
     * @param[in]      in_asn        asn_t to insert
     * @param[out]     out_asn       the previous asn_t at the given index
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_Attach(
        npi_cmncontext_t in_context,
        asn_t           *io_asn,
        int32_t          in_index,
        asn_t           *in_asn,
        asn_t          **out_asn);

    /**
     * @brief Set to NULL the element at position in_index into
     * io_asn. Return the previous element at the given index.
     *
     * @param[in, out] io_asn        asn_t to detach from
     * @param[in]      in_index      index to detach from
     * @param[out]     out_asn       the previous asn_t at the given index
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_Detach(
        npi_cmncontext_t in_context,
        asn_t           *io_asn,
        int32_t          in_index,
        asn_t          **out_asn);

    /**
     * @brief Find the first occurence of an element to the given class
     * and tag.
     *
     * @param[in] in_asn
     * @param[in] in_class
     * @param[in] in_tag
     *
     * @return    the element found, or NULL otherwise
     */
    asn_t *
    npi_asn_FindFirst(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        int32_t          in_class,
        int32_t          in_tag);

    /*
     * create universal class
     */

    /**
     * @brief Create a new Integer asn_t structure element from the given
     * value.
     *
     * @param[in]  in_int        Integer value
     * @param[out] out_aval      new Integer asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewInteger(
        npi_cmncontext_t in_context,
        int32_t          in_int,
        asn_t          **out_aval);

    /**
     * @brief Create a new string asn_t structure element from the given
     * values.
     *
     * @param[in]  in_length   string length
     * @param[in]  in_body     string buffer
     * @param[out] out_aval    new String asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewString(
        npi_cmncontext_t in_context,
        np_size_t        in_length,
        void            *in_body,
        asn_t          **out_aval);

    /**
     * @brief Create a new enumerated asn_t structure of size in_enum.
     *
     * @param[in]  in_enum    enumeration size
     * @param[out] out_aval   new enumerated asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewEnumerated(
        npi_cmncontext_t in_context,
        int32_t          in_enum,
        asn_t          **out_aval);

    /**
     * @brief Create a new sequence asn_t structure element.
     *
     * @param[out]    out_aval  new sequence asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     */
    retcode_t
    npi_asn_NewSequence(
        npi_cmncontext_t in_context,
        asn_t          **out_aval);

    /*
     * parse universal class
     */

    /**
     * @brief Parse the given boolean asn_t structure element.
     *
     * @param[in]  in_asn    given asn_t structure
     * @param[out] out_bool  output boolean
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_SIZE
     */
    retcode_t
    npi_asn_ParseBoolean(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        int32_t         *out_bool);

    /**
     * @brief Parse the given integer asn_t structure element.
     *
     * @param[in]  in_asn    given asn_t structure
     * @param[out] out_int   output integer
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_SIZE
     */
    retcode_t
    npi_asn_ParseInteger(
        npi_cmncontext_t in_context,
        asn_t          *in_asn,
        int32_t        *out_int);

    /**
     * @brief Parse the given integer asn_t structure element and output it in an octet buffer in Big Endian.
     *
     * @param[in]  in_asn      given asn_t structure
     * @param[in]  in_bufsize  the length of the output buffer
     * @param[out] out_buf     output unsigned multibyte integer in Big Endian
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_SIZE
     */
    retcode_t
    npi_asn_ParseUnsignedInteger(
        npi_cmncontext_t in_context,
        asn_t           *in__asn,
        int32_t          in_bufsize,
        u_int8_t        *out_buf);

    /**
     * @brief Parse the given bit string asn_t structure element.
     *
     * @param[in]  in_asn      given asn_t structure
     * @param[in]  in_buf_sz   the size of out_buf
     * @param[out] out_buf     output bit string
     * @param[out] out_bitsz   output bit string size
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_SIZE
     */
    retcode_t
    npi_asn_ParseBitstring(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        np_size_t        in_buf_sz,
        unsigned char   *out_buf,
        np_size_t       *out_bitsz);

    /**
     * @brief Parse the given string asn_t structure element.
     *
     * @param[in]  in_asn             given asn_t structure
     * @param[out] out_string         output string
     * @param[out] out_string_size    output string size
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_ParseString(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        unsigned char  **out_string,
        np_size_t       *out_string_size);

    /**
     * @brief Parse the given enumerated asn_t structure element.
     *
     * @param[in]  in_asn     given asn_t structure
     * @param[out] out_enum   output enumerated
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_SIZE
     */
    retcode_t
    npi_asn_ParseEnumerated(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        int32_t         *out_enum);

    /**
     * @brief Parse the given UTCTime asn_t structure element. Get the
     * time as an asn_time_t structure.
     *
     * @param[in]  in_asn        given asn_t structure
     * @param[in]  in_year19xx   time reference to adjust UTC time (e.g., 2007)
     * @param[out] out_time      output time
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_ParseUTCTime(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        int32_t          in_year19xx,
        asn_time_t      *out_time);

    /**
     * @brief Parse the given generalized time asn_t structure element. Get
     * the time as an asn_time_t structure.
     *
     * @param[in]  in_asn        given asn_t structure
     * @param[out] out_time      output time
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_ParseGeneralizedTime(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        asn_time_t      *out_time);

    /**
     * @brief Parse the given UTCTime asn_t structure element. Get the
     * time as a time_date_t structure.
     *
     * @param[in]  in_asn        given asn_t structure
     * @param[in]  in_year19xx   time reference to adjust UTC time (e.g., 2007)
     * @param[out] out_date      output date
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_ParseUTCTime_Ex(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        int32_t          in_year19xx,
        time_date_t     *out_date);

    /**
     * @brief Parse the given generalized time asn_t structure element. Get
     * the time as an time_date_t structure.
     *
     * @param[in]  in_asn        given asn_t structure
     * @param[out] out_date      output date
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_TYPE
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_ParseGeneralizedTime_Ex(
        npi_cmncontext_t in_context,
        asn_t           *in_asn,
        time_date_t     *out_date);


    /*
     * implicit, explicit
     */

    /**
     * @brief Set io_aval->clazz to ASN_CLASS_CONTEXT and the corresponding
     * tag to in_tag.
     *
     * @param[in]     in_tag     tag to set into io_aval
     * @param[in,out] io_aval    asn_t to be modified
     *
     * @return ::ERR_OK
     */
    retcode_t
    npi_asn_SetImplicit(
        npi_cmncontext_t in_context,
        int32_t          in_tag,
        asn_t           *io_aval);

    /**
     * @brief Given in_tag, create a new asn_t structure and append
     * in_aval to it.
     *
     * @param[in]  in_tag     tag to set into out_aval
     * @param[in]  in_aval    asn_t to be added to out_aval
     * @param[out] out_aval   output asn_t structure
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_FORMAT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_NewExplicit(
        npi_cmncontext_t in_context,
        int32_t          in_tag,
        asn_t           *in_aval,
        asn_t          **out_aval);

    /*
     * heap free parsing
     */

    /**
     * @brief Parse a sequence given by in_buf and in_size, into a given
     * array of asn_t structures.
     *
     * @param[in] in_buf        buffer to be parsed
     * @param[in] in_size       size of in_buf
     * @param[in,out] io_asn    array of asn_t structures of size in_num
     * @param[in] in_num        size of io_asn
     * @param[out] out_num      actual number of elements parsed
     *
     * @return ::ERR_OK
     * @return ::ERR_ASN_OVERFLOW
     * @return ::ERR_ASN_FORMAT
     */
    retcode_t
    npi_asn_ParseStructured(
        npi_cmncontext_t in_context,
        unsigned char   *in_buf,
        np_size_t        in_size,
        asn_t           *io_asn,
        int32_t          in_num,
        int32_t         *out_num);

    /*
     * debug
     */

    /**
     * @brief Print the context of an asn_t structure.
     *
     * @param[in] in_asn  asn_t structure to print
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     * @return ::ERR_ASN_INPUT
     * @return ::ERR_ASN_FORMAT
     * @return ::ERR_ASN_TYPE
     */
    retcode_t
    npi_asn_Dump(
        npi_cmncontext_t in_context,
        asn_t           *in_asn);


#ifdef __cplusplus
}
#endif
#endif /* ASN_API_H */
/** @} */
