/** @addtogroup box box
 *  @ingroup cmn
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 *
 * @depends basic_types.h
 * @depends binstr_api.h
 * @depends time_api.h
 */

#ifndef BOX_API_H
#define BOX_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Box processing library
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * Macro definitions
 *---------------------------------------------------------------------------*/

/* Based on ISO IEC 14496-12-2008 */
#define BOX_FULL_VERSION_LEN    (sizeof(u_int8_t))     /* Bit(8) */
#define BOX_FULL_FLAG_LEN       (3 * sizeof(u_int8_t)) /* Bit(24) */
#define BOX_EXTENDED_NAME_LEN   (16 * sizeof(u_int8_t))
#define BOX_SIZE_LEN            (sizeof(u_int32_t))
#define BOX_TYPE_LEN            (sizeof(u_int32_t))    /* 4CC Definition */
#define BOX_HEADER_SIZE         (BOX_SIZE_LEN + BOX_TYPE_LEN)
#define BOX_FULL_HEADER_SIZE    (BOX_HEADER_SIZE +      \
                                 BOX_FULL_VERSION_LEN + \
                                 BOX_FULL_FLAG_LEN)

/**
 * Box Type
 */
typedef enum box_type_tag {
    BOX_TYPE_COMPACT  = 0,
    BOX_TYPE_EXTENDED = 1
} box_type;

/**
 * Box Element
 */
typedef struct box_tag {
    u_int32_t  size;                                    /* Box size including the header (Compact) */
    box_type   boxtype;                                 /* Box type (Compact or Extended) */
    union {
        u_int32_t  boxname;                             /* 4CC format */
        u_int8_t   extendedname[BOX_EXTENDED_NAME_LEN]; /* Extended Type(UUID) */
    } bt;
    u_int8_t   version;                                 /* Valid For full-box */
    u_int8_t   flags[3];                                /* Valid For full-box */
    strm_t     boxbody;                                 /* Box Body */
} box_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define ERR_BOX_CORRUPTED    (ERR_BOX_BASE | 1)
#define ERR_BOX_NOTSUPPORTED (ERR_BOX_BASE | 2)

/**
 * Parse a Box buffer
 *
 * @param[in]  in_context    Memory context
 * @param[in]  io_box        Stream on box buffer
 * @param[in]  in_fullbox    Parse a Full Box
 * @param[out] out_box       Box contents
 *
 * @note After parsing io_boxsize returns the remaining bytes in the buffer
 * and io_boxbuf points to the start of the next box in the buffer
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_BOX_CORRUPTED
 * @return ::ERR_BOX_NOTSUPPORTED
 */
extern retcode_t
npi_box_ParseBuf(npi_cmncontext_t  in_context,
                 strm_t           *io_box,
                 bool_t            in_fullbox,
                 box_t            *out_box);

NP_END_EXTERN_C

#endif /* BOX_API_H */
/** @} */
