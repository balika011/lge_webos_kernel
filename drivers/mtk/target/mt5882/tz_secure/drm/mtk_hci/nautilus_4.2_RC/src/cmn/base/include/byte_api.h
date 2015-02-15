/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010,2011,2012 Sony Corporation.
 *
 */
#ifndef BYTE_API_H
#define BYTE_API_H

/** @file
 * Endian (byte order) Converter API
 *
 * @require none
 *
 * @author SAKAMOTO Takahiro <takas@sm.sony.co.jp>
 */

NP_BEGIN_EXTERN_C

/*
 * binary access (BE/LE, 1/2/4, R/W)
 *
 * Note: These macros are machine architecture independent. The BE and LE
 *       notations specific the ordering that the bytes of a value
 *       16 or 32 bits are written to or read from memory. For the LE case
 *       the LSByte is written to the lowest address. For the BE case the
 *       MSB is written the lowest address.
 *
 *       The BE and LE notations do not specific the architecture type of
 *       the chip.
 */

/**
* @brief Reads 1 byte from buffer
*/
#define npi_byte_BE1R(p)    (*(u_int8_t*)(p))
/**
* @brief Reads 2 big-endian bytes from buffer
*/
#define npi_byte_BE2R(p)    ((u_int16_t)((((u_int8_t*)(p))[0]<< 8) | \
                                             (((u_int8_t*)(p))[1])))
/**
* @brief Reads 4 big-endian bytes from buffer
*/
#define npi_byte_BE4R(p)    ((u_int32_t)((((u_int8_t*)(p))[0]<<24) | \
                                             (((u_int8_t*)(p))[1]<<16) | \
                                             (((u_int8_t*)(p))[2]<< 8) | \
                                             (((u_int8_t*)(p))[3])))

/**
* @brief Reads 1 byte from buffer
*/
#define npi_byte_LE1R(p)    (*(u_int8_t*)(p))
/**
* @brief Reads 2 little-endian bytes from buffer
*/
#define npi_byte_LE2R(p)    ((u_int16_t)((((u_int8_t*)(p))[0])     | \
                                             (((u_int8_t*)(p))[1]<< 8)))
/**
* @brief Reads 4 little-endian bytes from buffer
*/
#define npi_byte_LE4R(p)    ((u_int32_t)((((u_int8_t*)(p))[0])     | \
                                             (((u_int8_t*)(p))[1]<< 8) | \
                                             (((u_int8_t*)(p))[2]<<16) | \
                                             (((u_int8_t*)(p))[3]<<24)))

/**
* @brief Writes 1 byte to buffer
*/
#define npi_byte_BE1W(p,v)    *(u_int8_t*)(p) = (v)
/**
* @brief Writes 2-byte integer to buffer (big-endian)
*/
#define npi_byte_BE2W(p,v)    { u_int8_t* b_ = (u_int8_t*)(p); u_int32_t v_ = (v); b_[0] = v_>> 8; b_[1] = v_; }
/**
* @brief Writes 4-byte integer to buffer (big-endian)
*/
#define npi_byte_BE4W(p,v)    { u_int8_t* b_ = (u_int8_t*)(p); u_int32_t v_ = (v); b_[0] = v_>>24; b_[1] = v_>>16; b_[2] = v_>>8; b_[3] = v_; }

/**
* @brief Writes 1 byte to buffer
*/
#define npi_byte_LE1W(p,v)    *(u_int8_t*)(p) = (v)
/**
* @brief Writes 2-byte integer to buffer (little-endian)
*/
#define npi_byte_LE2W(p,v)    { u_int8_t* b_ = (u_int8_t*)(p); u_int32_t v_ = (v); b_[0] = v_; b_[1] = v_>>8; }
/**
* @brief Writes 4-byte integer to buffer (little-endian)
*/
#define npi_byte_LE4W(p,v)    { u_int8_t* b_ = (u_int8_t*)(p); u_int32_t v_ = (v); b_[0] = v_; b_[1] = v_>>8; b_[2] = v_>>16; b_[3] = v_>>24; }

/**
* @brief Writes a 4 byte character code as a 32 bit int
*/
#define npi_byte_4CHARCODE(a,b,c,d)  ((a<<24)|(b<<16)|(c<<8)|d)


NP_END_EXTERN_C

#endif /* BYTE_API_H */
/** @} */
