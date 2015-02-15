/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2007,2008,2009 Sony Corporation
 */
#ifndef STRM_API_IPTVES_H
#define STRM_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * pseudo stream operation
 *
 * require: byte
 */
/*==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    u_int8_t *buf;
    u_int8_t *rp;
    u_int8_t *wp;
    u_int32_t len;
} strm_t;

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
#define strm_Init(s,b,l)                                                \
    do { (s)->buf = (b); (s)->rp = (b); (s)->wp = (b)      ; (s)->len = (l); } while (0)
#define strm_Set(s,b,l)                                                 \
    do { (s)->buf = (b); (s)->rp = (b); (s)->wp = (b) + (l); (s)->len = (l); } while (0)
#define strm_Len(s)     ((s)->wp - (s)->rp)
#define strm_Buf(s)     ((s)->rp)

/*----------------------------------------------------------------------------
 * read/write (local use only)
 *--------------------------------------------------------------------------*/
#define strm_Read(s,n,exec)                     \
    do {                                        \
        if ((s)->rp + (n) > (s)->wp)            \
            exh_Throw(RET_STRM_READ_ERROR);     \
        exec;                                   \
        (s)->rp += (n);                         \
    } while (0)

#define strm_Write(s,n,exec)                        \
    do {                                            \
        if ((s)->wp + (n) > (s)->buf + (s)->len)    \
            exh_Throw(RET_STRM_WRITE_ERROR);        \
        exec;                                       \
        (s)->wp += (n);                             \
    } while (0)

/*----------------------------------------------------------------------------
 * stream access (BE/LE, 1/2/4, R/W)
 *--------------------------------------------------------------------------*/
#define strm_BE1R(s,v)          strm_Read(s,1,(v) = byte_BE1R((s)->rp))
#define strm_BE2R(s,v)          strm_Read(s,2,(v) = byte_BE2R((s)->rp))
#define strm_BE3R(s,v)          strm_Read(s,3,(v) = byte_BE3R((s)->rp))
#define strm_BE4R(s,v)          strm_Read(s,4,(v) = byte_BE4R((s)->rp))

#define strm_LE1R(s,v)          strm_Read(s,1,(v) = byte_LE1R((s)->rp))
#define strm_LE2R(s,v)          strm_Read(s,2,(v) = byte_LE2R((s)->rp))
#define strm_LE3R(s,v)          strm_Read(s,3,(v) = byte_LE3R((s)->rp))
#define strm_LE4R(s,v)          strm_Read(s,4,(v) = byte_LE4R((s)->rp))

#define strm_BE1W(s,v)          strm_Write(s,1,byte_BE1W((s)->wp,v))
#define strm_BE2W(s,v)          strm_Write(s,2,byte_BE2W((s)->wp,v))
#define strm_BE3W(s,v)          strm_Write(s,3,byte_BE3W((s)->wp,v))
#define strm_BE4W(s,v)          strm_Write(s,4,byte_BE4W((s)->wp,v))

#define strm_LE1W(s,v)          strm_Write(s,1,byte_LE1W((s)->wp,v))
#define strm_LE2W(s,v)          strm_Write(s,2,byte_LE2W((s)->wp,v))
#define strm_LE3W(s,v)          strm_Write(s,3,byte_LE3W((s)->wp,v))
#define strm_LE4W(s,v)          strm_Write(s,4,byte_LE4W((s)->wp,v))

/*----------------------------------------------------------------------------
 * memcpy/memset
 *--------------------------------------------------------------------------*/
#define strm_REFR(s,b,n)        strm_Read(s,n,(b) = (s)->rp)
#define strm_REFW(s,b,n)        strm_Write(s,n,(b) = (s)->wp)
#define strm_CPYR(s,b,n)        strm_Read(s,n,base_Memcpy((b),(n),(s)->rp,(n)))
#define strm_CPYW(s,b,n)        strm_Write(s,n,base_Memcpy((s)->wp,(n),(b),(n)))
#define strm_SETW(s,v,n)        strm_Write(s,n,memset((s)->wp,(v),(n)))

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_STRM_READ_ERROR     (RET_STRM_BASE|1)
#define RET_STRM_WRITE_ERROR    (RET_STRM_BASE|2)

/*============================================================================
 * APIs
 *==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* STRM_API_H */
