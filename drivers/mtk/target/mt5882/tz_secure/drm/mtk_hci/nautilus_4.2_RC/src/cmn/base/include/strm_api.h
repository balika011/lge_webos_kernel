/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 *
 */
#ifndef STRM_API_H
#define STRM_API_H

NP_BEGIN_EXTERN_C

/*==========================================================================*/
/**
 * @file
 * pseudo stream operation
 *
 * require: binstr
 */
/*==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    u_int8_t *buf;
    u_int8_t *rp;
    u_int8_t *wp;
    np_size_t len;
} strm_t;

typedef enum {
    STRM_CPY_ALL = -1,
} strm_cpy_t;

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define strm_Len(s)     ((s)->wp - (s)->rp)
#define strm_Buf(s)     ((s)->rp)
#define strm_GetWP(s)   ((s)->wp)

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define ERR_STRM_READ_ERROR     (ERR_STRM_BASE|1)
#define ERR_STRM_WRITE_ERROR    (ERR_STRM_BASE|2)
#define ERR_STRM_OVERFLOW       (ERR_STRM_BASE|3)

/*============================================================================
 * APIs
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * initialize/set
 *--------------------------------------------------------------------------*/
extern retcode_t
npi_strm_Init(strm_t *io_strm, np_size_t in_len, u_int8_t *in_buf);

extern retcode_t
npi_strm_Set(strm_t *io_strm, np_size_t in_len, u_int8_t *in_buf);

extern retcode_t
npi_strm_InitBS(strm_t *io_strm, binstr_t *in_bs);

extern retcode_t
npi_strm_SetBS(strm_t *io_strm, binstr_t *in_bs);

/*----------------------------------------------------------------------------
 * stream access (BE/LE, 1/2/4, R/W)
 *--------------------------------------------------------------------------*/
extern retcode_t
npi_strm_BE1R(strm_t *io_strm, u_int8_t *out_val);

extern retcode_t
npi_strm_BE2R(strm_t *io_strm, u_int16_t *out_val);

extern retcode_t
npi_strm_BE4R(strm_t *io_strm, u_int32_t *out_val);

extern retcode_t
npi_strm_LE1R(strm_t *io_strm, u_int8_t *out_val);

extern retcode_t
npi_strm_LE2R(strm_t *io_strm, u_int16_t *out_val);

extern retcode_t
npi_strm_LE4R(strm_t *io_strm, u_int32_t *out_val);

extern retcode_t
npi_strm_BE1W(strm_t *io_strm, u_int8_t in_val);

extern retcode_t
npi_strm_BE2W(strm_t *io_strm, u_int16_t in_val);

extern retcode_t
npi_strm_BE4W(strm_t *io_strm, u_int32_t in_val);

extern retcode_t
npi_strm_LE1W(strm_t *io_strm, u_int8_t in_val);

extern retcode_t
npi_strm_LE2W(strm_t *io_strm, u_int16_t in_val);

extern retcode_t
npi_strm_LE4W(strm_t *io_strm, u_int32_t in_val);

/*----------------------------------------------------------------------------
 * memcpy/memset/skip
 *--------------------------------------------------------------------------*/
extern retcode_t
npi_strm_REFR(strm_t *io_strm, np_size_t in_len, u_int8_t **out_buf);

extern retcode_t
npi_strm_REFW(strm_t *io_strm, np_size_t in_len, u_int8_t **out_buf);

extern retcode_t
npi_strm_CPYR(strm_t *io_strm, np_size_t in_len, u_int8_t *out_buf);

extern retcode_t
npi_strm_CPYW(strm_t *io_strm, np_size_t in_len, u_int8_t *in_buf);

extern retcode_t
npi_strm_SETW(strm_t *io_strm, np_size_t in_len, u_int8_t in_val);

extern retcode_t
npi_strm_SKIP(strm_t *io_strm, np_size_t in_len);

/*----------------------------------------------------------------------------
 * memcpy from/to binstr
 *--------------------------------------------------------------------------*/
extern retcode_t
npi_strm_S2BS(npi_cmncontext_t in_context, strm_t *io_strm, np_size_t in_len, binstr_t **io_bs);

extern retcode_t
npi_strm_S2Str(npi_cmncontext_t in_context, strm_t *io_strm, np_size_t *out_len, char **out_str);

extern retcode_t
npi_strm_BS2S(strm_t *io_strm, np_size_t in_len, binstr_t *in_bs);

NP_END_EXTERN_C

#endif /* STRM_API_H */
/** @} */
