/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2005,2006,2007,2008 Sony Corporation
 */
#ifndef BYTE_API_IPTVES_H
#define BYTE_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Endian (byte order) Converter API
 *
 * require: none
 */
/*==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * binary access (BE/LE, 1/2/4, R/W)
 *--------------------------------------------------------------------------*/
#define byte_BE1R(p)    (*(u_int8_t*)(p))
#define byte_BE2R(p)    ((u_int16_t)((((u_int8_t*)(p))[0]<< 8) |    \
                                     (((u_int8_t*)(p))[1])))
#define byte_BE3R(p)    ((u_int32_t)((((u_int8_t*)(p))[0]<<16) |    \
                                     (((u_int8_t*)(p))[1]<< 8) |    \
                                     (((u_int8_t*)(p))[2])))
#define byte_BE4R(p)    ((u_int32_t)((((u_int8_t*)(p))[0]<<24) |    \
                                     (((u_int8_t*)(p))[1]<<16) |    \
                                     (((u_int8_t*)(p))[2]<< 8) |    \
                                     (((u_int8_t*)(p))[3])))

#define byte_LE1R(p)    (*(u_int8_t*)(p))
#define byte_LE2R(p)    ((u_int16_t)((((u_int8_t*)(p))[0])     |    \
                                     (((u_int8_t*)(p))[1]<< 8)))
#define byte_LE3R(p)    ((u_int32_t)((((u_int8_t*)(p))[0])     |    \
                                     (((u_int8_t*)(p))[1]<< 8) |    \
                                     (((u_int8_t*)(p))[2]<<16)))
#define byte_LE4R(p)    ((u_int32_t)((((u_int8_t*)(p))[0])     |    \
                                     (((u_int8_t*)(p))[1]<< 8) |    \
                                     (((u_int8_t*)(p))[2]<<16) |    \
                                     (((u_int8_t*)(p))[3]<<24)))

#define byte_BE1W(p,v)  *(u_int8_t*)(p) = (v)
#define byte_BE2W(p,v)                          \
    do {                                        \
        u_int8_t* b_ = (u_int8_t*)(p);          \
        u_int32_t v_ = (v);                     \
        b_[0] = v_>> 8;                         \
        b_[1] = v_;                             \
    } while (0)
#define byte_BE3W(p,v)                          \
    do {                                        \
        u_int8_t* b_ = (u_int8_t*)(p);          \
        u_int32_t v_ = (v);                     \
        b_[0] = v_>>16;                         \
        b_[1] = v_>>8;                          \
        b_[2] = v_;                             \
    } while (0)

#define byte_BE4W(p,v)                          \
    do {                                        \
        u_int8_t* b_ = (u_int8_t*)(p);          \
        u_int32_t v_ = (v);                     \
        b_[0] = v_>>24;                         \
        b_[1] = v_>>16;                         \
        b_[2] = v_>>8;                          \
        b_[3] = v_;                             \
    } while (0)

#define byte_LE1W(p,v)  *(u_int8_t*)(p) = (v)
#define byte_LE2W(p,v)                          \
    do {                                        \
        u_int8_t* b_ = (u_int8_t*)(p);          \
        u_int32_t v_ = (v);                     \
        b_[0] = v_;                             \
        b_[1] = v_>>8;                          \
    } while (0)
#define byte_LE3W(p,v)                          \
    do {                                        \
        u_int8_t* b_ = (u_int8_t*)(p);          \
        u_int32_t v_ = (v);                     \
        b_[0] = v_;                             \
        b_[1] = v_>>8;                          \
        b_[2] = v_>>16;                         \
    } while (0)
#define byte_LE4W(p,v)                          \
    do {                                        \
        u_int8_t* b_ = (u_int8_t*)(p);          \
        u_int32_t v_ = (v);                     \
        b_[0] = v_;                             \
        b_[1] = v_>>8;                          \
        b_[2] = v_>>16;                         \
        b_[3] = v_>>24;                         \
    } while (0)

/*----------------------------------------------------------------------------
 * binary access (BE/LE, 1/2/4, R/W) with incrementing pointer
 *--------------------------------------------------------------------------*/
#define byte_BE1RI(p,v)         do { (v) = byte_BE1R(p); (p) = (u_int8_t*)(p) + 1; } while (0)
#define byte_BE2RI(p,v)         do { (v) = byte_BE2R(p); (p) = (u_int8_t*)(p) + 2; } while (0)
#define byte_BE3RI(p,v)         do { (v) = byte_BE3R(p); (p) = (u_int8_t*)(p) + 3; } while (0)
#define byte_BE4RI(p,v)         do { (v) = byte_BE4R(p); (p) = (u_int8_t*)(p) + 4; } while (0)

#define byte_LE1RI(p,v)         do { (v) = byte_LE1R(p); (p) = (u_int8_t*)(p) + 1; } while (0)
#define byte_LE2RI(p,v)         do { (v) = byte_LE2R(p); (p) = (u_int8_t*)(p) + 2; } while (0)
#define byte_LE3RI(p,v)         do { (v) = byte_LE3R(p); (p) = (u_int8_t*)(p) + 3; } while (0)
#define byte_LE4RI(p,v)         do { (v) = byte_LE4R(p); (p) = (u_int8_t*)(p) + 4; } while (0)

#define byte_BE1WI(p,v)         do { byte_BE1W(p,v); (p) = (u_int8_t*)(p) + 1; } while (0)
#define byte_BE2WI(p,v)         do { byte_BE2W(p,v); (p) = (u_int8_t*)(p) + 2; } while (0)
#define byte_BE3WI(p,v)         do { byte_BE3W(p,v); (p) = (u_int8_t*)(p) + 3; } while (0)
#define byte_BE4WI(p,v)         do { byte_BE4W(p,v); (p) = (u_int8_t*)(p) + 4; } while (0)

#define byte_LE1WI(p,v)         do { byte_LE1W(p,v); (p) = (u_int8_t*)(p) + 1; } while (0)
#define byte_LE2WI(p,v)         do { byte_LE2W(p,v); (p) = (u_int8_t*)(p) + 2; } while (0)
#define byte_LE3WI(p,v)         do { byte_LE3W(p,v); (p) = (u_int8_t*)(p) + 3; } while (0)
#define byte_LE4WI(p,v)         do { byte_LE4W(p,v); (p) = (u_int8_t*)(p) + 4; } while (0)

/*----------------------------------------------------------------------------
 * memcpy/memset with incrementing pointer
 *--------------------------------------------------------------------------*/
#define byte_REFRI(s,d,n)       do { (d) = (s);           (s) = (u_int8_t*)(s) + (n); } while (0)
#define byte_CPYRI(s,d,n)       do { base_Memcpy((d),(n),(s),(n)); (s) = (u_int8_t*)(s) + (n); } while (0)
#define byte_CPYWI(d,s,n)       do { base_Memcpy((d),(n),(s),(n)); (d) = (u_int8_t*)(d) + (n); } while (0)
#define byte_SETWI(d,v,n)       do { memset((d),(v),(n)); (d) = (u_int8_t*)(d) + (n); } while (0)

/*----------------------------------------------------------------------------
 * Four Char Code
 *--------------------------------------------------------------------------*/
#define byte_4CC(c1,c2,c3,c4)   ((u_int32_t)((c1)<<24 | (c2)<<16 | (c3)<<8 | (c4)))

/*============================================================================
 * APIs
 *==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* BYTE_API_H */
