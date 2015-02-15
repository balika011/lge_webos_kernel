/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2004,2005,2006,2007,2008 Sony Corporation
 */
#ifndef FIO_API_IPTVES_H
#define FIO_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * File I/O API
 *
 * require: base
 */
/*==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
/**
 * file-handle definition
 */
typedef void *          fio_handle_t;

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
/**
 * initial value for file-handle
 */
#define FIO_NULL        ((fio_handle_t)0)

/**
 * Open Mode
 */
/* internal use */
#define FIO_L_OPENMODEFLAG_READ         (1<<0)
#define FIO_L_OPENMODEFLAG_WRITE        (1<<1)
#define FIO_L_OPENMODEFLAG_APPEND       (1<<2)
#define FIO_L_OPENMODEFLAG_TEXT         (1<<3)
/* exported macros */
#define FIO_OPENMODEMASK_ACCESS         (FIO_L_OPENMODEFLAG_READ|FIO_L_OPENMODEFLAG_WRITE|FIO_L_OPENMODEFLAG_APPEND)
#define FIO_OPENMODEFLAG_TEXT           FIO_L_OPENMODEFLAG_TEXT
#define FIO_OPENMODE_READ               FIO_L_OPENMODEFLAG_READ
#define FIO_OPENMODE_WRITE              FIO_L_OPENMODEFLAG_WRITE
#define FIO_OPENMODE_READWRITE          (FIO_L_OPENMODEFLAG_READ|FIO_L_OPENMODEFLAG_WRITE)
#define FIO_OPENMODE_APPEND             FIO_L_OPENMODEFLAG_APPEND
#define FIO_OPENMODE_READ_TEXT          (FIO_OPENMODE_READ|FIO_OPENMODEFLAG_TEXT)
#define FIO_OPENMODE_WRITE_TEXT         (FIO_OPENMODE_WRITE|FIO_OPENMODEFLAG_TEXT)
#define FIO_OPENMODE_READWRITE_TEXT     (FIO_OPENMODE_READWRITE|FIO_OPENMODEFLAG_TEXT)
#define FIO_OPENMODE_APPEND_TEXT        (FIO_OPENMODE_APPEND|FIO_OPENMODEFLAG_TEXT)

/**
 * Base Position definition
 */
#define FIO_SEEK_SET    0       /**< Beginning of file */
#define FIO_SEEK_CUR    1       /**< Current position */
#define FIO_SEEK_END    2       /**< End of file */

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_FIO_CANNOTOPEN      (RET_FIO_BASE|1)
#define RET_FIO_NOTEXIST        (RET_FIO_BASE|2)
#define RET_FIO_CANNOTREAD      (RET_FIO_BASE|3)
#define RET_FIO_CANNOTWRITE     (RET_FIO_BASE|4)
#define RET_FIO_CANNOTSEEK      (RET_FIO_BASE|5)
#define RET_FIO_CANNOTTELL      (RET_FIO_BASE|6)
#define RET_FIO_CANNOTSTAT      (RET_FIO_BASE|7)
#define RET_FIO_ILLEGALMODE     (RET_FIO_BASE|8)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Open file
 * Open the file specified by in_filename with the mode in_openmode.
 * If it is successfully opened, a new file handle is created and stored
 * in out_handle.  Otherwise the contents of out_handle is not modified.
 *
 * @param [in]     in_filename  File name to be opened
 * @param [in]     in_openmode  Open mode (FIO_OPENMODE_*)
 * @param [out]    out_handle   The buffer where a new file handle is stored
 * @retval RET_OK               Success
 * @retval RET_NOMEM            Fail to allocate memory
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_FIO_ILLEGALMODE  @p in_openmode is illegal
 * @retval RET_FIO_CANNOTOPEN   Fail to open the file specified by @p in_filename
 */
extern int
fio_Open(const char            *in_filename,
         int                    in_openmode,
         fio_handle_t          *out_handle);

/**
 * Close file
 * Close the file handle. The buffer of the file handle is freed here.
 *
 * @param [in,out] io_handle    Handle to be closed
 * @retval RET_OK               Success
 * @retval RET_ILLEGALHANDLE    Illegal handle is passed to this API
 */
extern int
fio_Close(fio_handle_t          io_handle);

/**
 * Read from file
 *
 * @param [in,out] io_handle    File handle
 * @param [in]     in_size      Size of read data
 * @param [out]    out_size     The buffer where the real read size is stored
 * @param [out]    out_buffer   The buffer where read data are stored
 * @retval RET_OK               Success
 * @retval RET_ILLEGALHANDLE    Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_FIO_CANNOTREAD   Fail to read
 */
extern int
fio_Read(fio_handle_t           io_handle,
         fio_size_t             in_size,
         fio_size_t            *out_size,
         void                  *out_buffer);

/**
 * Write to file
 *
 * @param [in,out] io_handle    File handle
 * @param [in]     in_size      Size of buffer to be written to file
 * @param [in]     in_buffer    Buffer to be written to file
 * @retval RET_OK               Success
 * @retval RET_ILLEGALHANDLE    Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_FIO_CANNOTWRITE  Fail to wirte
 */
extern int
fio_Write(fio_handle_t          io_handle,
          fio_size_t            in_size,
          const void           *in_buffer);

/**
 * Set current file position
 *
 * @param [in,out] io_handle    File handle
 * @param [in]     in_offset    Offset from base position
 * @param [in]     in_whence    Base position (FIO_SEEK_SET/CUR/END)
 * @retval RET_OK               Success
 * @retval RET_ILLEGALHANDLE    Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_FIO_CANONOTSEEK  Fail to seek
 */
extern int
fio_Seek(fio_handle_t           io_handle,
         fio_offset_t           in_offset,
         int                    in_whence);

/**
 * Get current file position
 *
 * @param [in,out] io_handle    File handle
 * @param [out]    out_size     Buffer where the current position is stored
 * @retval RET_OK               Success
 * @retval RET_ILLEGALHANDLE    Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_FIO_CANNOTTELL   Fail to tell
 */
extern int
fio_Tell(fio_handle_t           io_handle,
         fio_offset_t          *out_size);

/**
 * Get file-size
 *
 * @param [in,out] io_handle    File handle
 * @param [out]    out_size     Buffer where the file size is stored
 * @retval RET_OK               Success
 * @retval RET_ILLEGALHANDLE    Illegal handle is passed to this API
 * @retval RET_ILLEGALARGS      Illegal arguments are passed to this API
 * @retval RET_FIO_CANONOTSEEK  Fail to seek
 * @retval RET_FIO_CANNOTTELL   Fail to tell
 */
extern int
fio_Length(fio_handle_t         io_handle,
           fio_size_t          *out_size);

/**
 * Remove a file
 *
 * @param [in]     in_filename  File name to be removed
 * @retval RET_OK               Success
 * @retval RET_FIO_CANNOTOPEN   Fail to remove the file specified by @p in_filename
 * @retval RET_FIO_NOTEXIST     The file specified by @p in_filename is not exist
 */
extern int
fio_Remove(const char          *in_filename);

#ifdef __cplusplus
}
#endif

#endif /* FIO_API_H */
