/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 *
 */

#ifndef VLBINSTR_API_H
#define VLBINSTR_API_H

/** @file
 * Variable-Length Binary String Type Definition and API
 *
 * @require cmn/base/binstr
 * @require cmn/base/mem
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 */

NP_BEGIN_EXTERN_C

/**
 * vlbinstr Structure
 */
typedef struct {
    /** Length of string */
    np_size_t   len;
    /** Buffer of string */
    u_int8_t *buf;
    /** Size of allocated memory */
    np_size_t   alloc;
} vlbinstr_t;

/*---------------------------------------------------------------------------*
 * APIs
 *---------------------------------------------------------------------------*/
/**
 * duplicate string
 *
 * @param[in] in_str   string to be duplicated.
 * @return duplicated string typed vlbinstr_t
 * @note in_str can be typed either binstr_t or vlbinstr_t.
 */
#define npi_vlbinstr_Dup(in_context, in_str) \
    npi_vlbinstr_Alloc((in_context), (in_str)->len, (in_str)->buf)

/**
 * @brief Appends in_buf (size of which is in_len) to the end of io_str
 *
 * @param[in] in_context       context
 * @param[in,out] io_str       string to which in_str2 is appended
 * @param[in] in_len           length of the appended buffer
 * @param[in] in_buf           buffer which is appended to in_str1
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_NOMEM                  <br>A resource failure has occurred.
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 */
extern retcode_t
npi_vlbinstr_AppendBuf(
    npi_cmncontext_t    in_context,
    vlbinstr_t         *io_str,
    np_size_t           in_len,
    const u_int8_t     *in_buf);

/**
 * @brief Appends in_str2 to the end of in_str1
 *
 * @param[in] in_context       context
 * @param[in,out] in_str1      string to which in_str2 is appended
 * @param[in] in_str2          string which is appended to in_str1
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 * @return ::ERR_NOMEM                  <br>A resource failure has occurred.
 * @note in_str2 can be typed to either binstr_t or vlbinstr_t
 */
#define npi_vlbinstr_Append(in_context, io_str1, io_str2) \
    npi_vlbinstr_AppendBuf((in_context), (io_str1), (io_str2)->len, (io_str2)->buf)

/**
 * @brief Truncates the string to the specified length
 *
 * @param[in] in_context       context
 * @param[in,out] io_str       string to be truncated
 * @param[in] in_newlen        new length of the string
 *
 * @return ::ERR_OK                     <br>Indicating success
 * @return ::ERR_ILLEGALARGS            <br>The input arguments are incorrect. Arguments cannot be NULL.
 */
extern retcode_t
npi_vlbinstr_Trunc(
    npi_cmncontext_t   in_context,
    vlbinstr_t        *io_str,
    np_size_t          in_newlen);

/**
 * @brief Compares 2 strings
 *
 * @param[in] in_context       context
 * @param[in] in_str1          string
 * @param[in] in_str2          string
 * @return 0            if in_str1 == in_str2
 * @return >0           if in_str1 >  in_str2
 * @return <0           if in_str1 <  in_str2
 */
extern int32_t
npi_vlbinstr_CmpFun(
    npi_cmncontext_t    in_context,
    const vlbinstr_t   *in_str1,
    const vlbinstr_t   *in_str2);

/**
 * @brief Compares 2 strings
 *
 * @param[in] in_context       context
 * @param[in] in_str1          string
 * @param[in] in_str2          string
 * @return 0            if in_str1 == in_str2
 * @return >0           if in_str1 >  in_str2
 * @return <0           if in_str1 <  in_str2
 * @note in_str1 and in_str2 can be typed to either binstr_t or vlbinstr_t
 */
#define npi_vlbinstr_Cmp(in_context, in_str1, in_str2) \
    npi_binstr_CmpBuf((in_context), (in_str1)->len, (in_str1)->buf, (in_str2)->len, (in_str2)->buf)

/**
 * @brief length of string
 *
 * @param[in] in_str   string
 * @return length of in_str
 * @note in_str can be typed to either binstr_t or vlbinstr_t
 */
#define npi_vlbinstr_Len(in_str) ((in_str)->len)

/**
 * @brief buffer of string
 *
 * @param[in] in_str  string
 * @return pointer of the buffer of in_str
 * @note in_str can be typed to either binstr_t or vlbinstr_t
 */
#define npi_vlbinstr_Buf(in_str) ((in_str)->buf)

/**
 * @brief Allocates string buffer
 *
 * @param[in] in_context       context
 * @param[in] in_size          size of string you want to create
 * @param[in] in_buf           initial string (NULL if you do not want to initialize)
 * @return allocated string
*/
extern vlbinstr_t *
npi_vlbinstr_Alloc(
    npi_cmncontext_t        in_context,
    np_size_t               in_size,
    const u_int8_t         *in_buf);

/**
 * @brief Allocates string buffer and fills it by the given byte.
 *
 * @param[in] in_context       context
 * @param[in] in_size          size of string you want to create
 * @param[in] in_byte          the byte by which the buffer is fulfilled
 * @return allocated string
 *
 */
extern vlbinstr_t *
npi_vlbinstr_AllocFill(
    npi_cmncontext_t        in_context,
    np_size_t               in_size,
    u_int8_t                in_byte);

/**
 * @brief Frees string buffer
 *
 * @param[in] in_context       context
 * @param[in] in_str           string to be freed
 * @note in_str is not set to NULL by this function
 *
 * @return none
 */
extern void
npi_vlbinstr_Free(
    npi_cmncontext_t    in_context,
    vlbinstr_t         *in_str);

/**
 * @brief Dumps the content of the string for debugging
 *
 * @param[in] in_str string to be dumped
 * @return none
 * @note in_str can be typed to either binstr_t or vlbinstr_t
 */
#define npi_vlbinstr_Dump(in_str)  DUMP1(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Dump0(in_str) DUMP0(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Dump1(in_str) DUMP1(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Dump2(in_str) DUMP2(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Dump3(in_str) DUMP3(((in_str)->len,(in_str)->buf))

/**
 * @brief print the contents of the string for debugging
 *
 * @param[in] in_str string to be printed
 * @return none
 * @note in_str can be typed to either binstr_t or vlbinstr_t
 */
#define npi_vlbinstr_Print(in_str)  PRINT1(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Print0(in_str) PRINT0(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Print1(in_str) PRINT1(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Print2(in_str) PRINT2(((in_str)->len,(in_str)->buf))
#define npi_vlbinstr_Print3(in_str) PRINT3(((in_str)->len,(in_str)->buf))

NP_END_EXTERN_C

#endif /* VLBINSTR_API_H */
/** @} */
