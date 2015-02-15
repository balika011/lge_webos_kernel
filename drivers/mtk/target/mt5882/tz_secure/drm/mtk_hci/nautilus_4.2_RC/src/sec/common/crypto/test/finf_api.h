/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef FINF_API_H
#define FINF_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief File Support Definition and API
     *
     * dependency: base vlbinstr
     *
     * @author Yoshizumi Tanaka <y-tanaka@sm.sony.co.jp>
     * @date   6 Dec, 2004 by y-tanaka
     */
    /*===========================================================================*/
    /*------------------------------------------------------------------------*
     * type definition
     *------------------------------------------------------------------------*/
    typedef struct {
        char         *fname;
        fio_handle_t  fio;
        binstr_t     *data;
    } finf_t;

    /*------------------------------------------------------------------------*
     * error definition
     *------------------------------------------------------------------------*/
#define ERR_FINF_BASE    9000

    enum err_finf_tag {
        ERR_FINF_NOMEM    = ERR_FINF_BASE,
        ERR_FINF_OPEN
    };

    /*========================================================================*/
    /**
     * @brief  Initialize FINF context
     *
     * @param[in,out] io_crypto finf handle
     * @return ::ERR_OK
     * @return ::ERR_FINF_NOMEM
     */
    /*========================================================================*/
    extern retcode_t
    finf_Init(finf_t **io_inf);

    /*========================================================================*/
    /**
     * @brief  Finalize FINF context
     *
     * @param[in,out] io_finf finf handle
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    finf_Fin(finf_t *io_inf);

    /*========================================================================*/
    /**
     * @brief  Open file
     *
     * @param[in,out] io_finf finf handle
     * @param[in] in_path file path
     * @param[in] in_mode open mode ("rb"|"wb")
     * @return ::ERR_OK
     * @return ::ERR_FINF_OPEN
     */
    /*========================================================================*/
    extern retcode_t
    finf_Open(finf_t     *io_inf,
              const char *in_path,
              const char *in_mode);

    /*========================================================================*/
    /**
     * @brief  Close file
     *
     * @param[in,out] io_finf finf handle
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    finf_Close(finf_t *io_inf);

    /*========================================================================*/
    /**
     * @brief  Allocate buffer and read data from file
     *
     * @param[in,out] io_finf finf handle
     * @return ::ERR_OK
     * @return ::ERR_FINF_NOMEM
     */
    /*========================================================================*/
    extern retcode_t
    finf_Read(finf_t *io_inf);

    /*========================================================================*/
    /**
     * @brief  Write data to file
     *
     * @param[in,out] io_finf finf handle
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    finf_Write(finf_t *io_inf);

#ifdef __cplusplus
}
#endif

#endif /* FINF_API_H */
