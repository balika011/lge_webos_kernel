/** @addtogroup ut_utils ut_utils
 *  @ingroup ut
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 *
 */
#ifndef UT_UTILS_H
#define UT_UTILS_H
#include <stdio.h>

NP_BEGIN_EXTERN_C

#ifdef __GNUC__
#define UT_MSGD        MSG0(("ERROR in %s %d\n", __func__, __LINE__))
#define UT_LOGOUT      if ( ret != ERR_OK ) MSG0(("ERROR in %s %x\n", __func__, ret));
#else
#define UT_MSGD        MSG0(("ERROR in %s %d\n", __FILE__, __LINE__))
#define UT_LOGOUT      if ( ret != ERR_OK ) MSG0(("ERROR in %s %x\n", __FILE__, ret));
#endif


/** @file
 * This library contains the Unit Test Utilities functionalities
 *
 * @require base
 *
 * @author Riccardo Ballarino <Riccardo.Ballarino@eu.sony.com>
 */

/**
 * @brief  Initialize the context of ut functions : must be done before using ut functions
 *
 * @param[in]  in_context   File name ( NPI_CMN_CONTEXT_SECURE or NPI_CMN_CONTEXT_NONSECURE )
 *
 */
void
ut_Init( npi_cmncontext_t in_context);

/**
 * @brief  This function reset the UT context to uninitialize state.
 */
void
ut_Fin(void);

/**
 * @brief  Returns the current context for ut framework set in ut_Init()
 * functions.
 *
 * @return ::NPI_CMN_CONTEXT_SECURE
 * @return ::NPI_CMN_CONTEXT_NONSECURE
 * @return -1
 */
npi_cmncontext_t
ut_getContext(void);

/**
 * @brief Output some text
 *
 * @param[in]  in_fmt   Format for text
 *
 */
int32_t
ut_Print(char *in_fmt, ... );

/**
 * @brief Open a file to redirect all ut_Dump
 *
 * @param[in]  in_filename   File name
 *
 * @return ::ERR_OK               if the file is successfully opened.
 * @return ::ERR_ILLEGALARGS      out_handle is NULL
 * @return ::ERR_NOMEM            failed to allocate the file handle buffer
 * @return ::ERR_FIO_ILLEGALMODE  in_openmode is illegal
 * @return ::ERR_FIO_CANNOTOPEN   failed to open the file
 */
np_ret_t
ut_DumpInit( const char *in_filename);

/**
 * @brief Close the dump file
 *
 * @return ::ERR_OK               if the handle is successfully closed
 * @return ::ERR_ILLEGALHANDLE    io_handle is NULL or illegal
 */
np_ret_t
ut_DumpClose(void);

/**
 * @brief Dump to file opend by ut_Init
 *
 * @param[in]  fmt   Format args
 * @param[in]  ...   Arguments
 *
 */
void
ut_Dump(char *fmt, ...);


/**
 * @brief Compare the dump file with the same one in teh result dir
 *
 * @param[in]  in_result_dir   Results directory
 *
 * @return ::ut_CompareFile
 */
np_ret_t
ut_DumpCompare( const char *in_result_dir );

/**
 * @brief  Read a file into a buffer
 *
 * @param[in]  in_filename   File name
 * @param[out] out_buflen    Out Buffer len
 * @param[out] out_buf       Out Buffer
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_FIO_ILLEGALMODE
 * @return ::ERR_FIO_NOTEXIST
 * @return ::ERR_FIO_CANNOTOPEN
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_FIO_CANNOTREAD
 */
np_ret_t
ut_ReadFile( const char *in_filename, u_int32_t  *out_buflen, u_int8_t   **out_buf);

/**
 * @brief  Read a file into a binstr_t buffer
 *
 * @param[in]  in_filename   File name
 * @param[out] out_buf       Out binstr_t Buffer
 *
 * @return ::ut_ReadFile
 */
np_ret_t
ut_ReadFileBinstr( const char *in_filename, binstr_t **out_buf);

/**
 * @brief  Dump a buffer into a file
 *
 * @param[in] in_filename  File name
 * @param[in] in_buflen    Buffer len
 * @param[in] in_buf       Buffer
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_FIO_ILLEGALMODE
 * @return ::ERR_FIO_NOTEXIST
 * @return ::ERR_FIO_CANNOTOPEN
 * @return ::ERR_ILLEGALHANDLE
 * @return ::ERR_FIO_CANNOTWRITE
 */
np_ret_t
ut_WriteFile( const char *in_filename, u_int32_t in_buflen, u_int8_t  *in_buf);

/**
 * @brief  Dump a binstr_t buffer into a file
 *
 * @param[in] in_filename   File name
 * @param[in] in_buf        binstr_t Buffer
 *
 * @return ::ut_WriteFile
 */
np_ret_t
ut_WriteFileBinstr( const char *in_filename, binstr_t *in_buf);

/**
 * @brief  Compares two files
 *
 * @param[in] in_file1  File1 name
 * @param[in] in_file2  File2 name
 *
 * @return ::ERR_OK
 * @return ::npi_str_Cmp
 * @return ::ut_ReadFile
 */
np_ret_t
ut_CompareFile( const char *in_file1, const char *in_file2 );

/**
 * @brief  Move file src to dest
 *
 * @param[in] in_file_src  Source File
 * @param[in] in_file_dst  Dest File
 *
 * @return ::ERR_OK
 * @return ::ERR_NG
 */
np_ret_t
ut_MoveFile( const char *in_file_src, const char *in_file_dst );

/**
 * @brief  clears nphome directory
 *
 * @return ::ERR_OK
 * @return ::ERR_NG
 */
void
ut_clear_nphome(void);

NP_END_EXTERN_C

#endif /* UT_UTILS_H */
/** @} */


