/** @addtogroup nplocal nplocal
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef NPLOCAL_API_H
#define NPLOCAL_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief Nautilus Library API (local-file-registration)
     *
     * depend: npcommon
     *
     * @author Norifumi Goto <n-goto@sm.sony.co.jp>
     * @date   02 Jun, 2005 last modified by n-goto
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register NEMO public keys
     *
     * @param[in] in_size length of the NEMO public keys
     * @param[in] in_buf buffer of the NEMO public keys
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistNEMOPublicKeys(np_size_t in_size,
                                 u_int8_t *in_buf,
                                 np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register NEMO private keys
     *
     * @param[in] in_size length of the NEMO private keys
     * @param[in] in_buf buffer of the NEMO private keys
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistNEMOPrivateKeys(np_size_t in_size,
                                  u_int8_t *in_buf,
                                  np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Role Assertion
     *
     * @param[in] in_size length of the Role assertion
     * @param[in] in_buf buffer of the Role assertion
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistRoleAssertion(np_size_t in_size,
                                u_int8_t *in_buf,
                                np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register public personality
     *
     * @param[in] in_pubsize  length of the public personality XML
     * @param[in] in_pubbuf   buffer of the public personality XML
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS        given size is 0 or buf is NULL
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistPublicPersonality(np_size_t in_pubsize,
                                    u_int8_t *in_pubbuf,
                                    np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register private personality
     *
     * @param[in] in_privsize length of the private personality XML
     * @param[in] in_privbuf  buffer of the private personality XML
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistPrivatePersonality(np_size_t in_privsize,
                                     u_int8_t *in_privbuf,
                                     np_secbuffer_t *out_secbuf);

#if defined(NPOPT_ENABLE_DCS)
    /*---------------------------------------------------------------------------*/
    /**
     * @brief register dcs assertion.
     *
     * @param[in] in_dcsnsize  length of the data certification standard name
     * @param[in] in_dcsnbuf   buffer of the data certification standard name
     * @param[in] in_size      length of the dcs assertion XML
     * @param[in] in_buf       buffer of the dcs assertion XML
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NOTSUPPORTED
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistDCSAssertion(np_size_t in_dcsnsize,
                               u_int8_t *in_dcsnbuf,
                               np_size_t in_size,
                               u_int8_t *in_buf,
                               np_secbuffer_t *out_secbuf);
#endif

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Import License signing public keys
     *
     * @param[in] in_pubsize    length of the Import LS public keys
     * @param[in] in_pubbuf     buffer of the Import LS public keys
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS       in_pubsize is zero or in_pubbuf is NULL
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    np_ret_t
    nplocal_RegistImportLSPublicKeys(np_size_t       in_pubsize,
                                     u_int8_t       *in_pubbuf,
                                     np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Import License signing private keys
     *
     * @param[in] in_privsize    length of the Import LS private keys
     * @param[in] in_privbuf     buffer of the Import LS private keys
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS            in_privsize is zero or in_privbuf is NULL
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistImportLSPrivateKeys(np_size_t       in_privsize,
                                      u_int8_t       *in_privbuf,
                                      np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Import Nemo public keys
     *
     * @param[in]  in_pubsize            Length of the Import Nemo public keys
     * @param[in]  in_pubbuf             Buffer of the Import Nemo public keys
     * @praam[out] out_secbuf            Secure buffer containing sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS       in_pubsize is zero or in_pubbuf is NULL
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    np_ret_t
    nplocal_RegistImportNemoPublicKeys(np_size_t       in_pubsize,
                                       u_int8_t       *in_pubbuf,
                                       np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Import Nemo private keys
     *
     * @param[in]  in_privsize           Length of the Import Nemo private keys
     * @param[in]  in_privbuf            Buffer of the Import Nemo private keys
     * @praam[out] out_secbuf            Secure buffer containing sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS      in_privsize is zero or in_privbuf is NULL
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_RegistImportNemoPrivateKeys(np_size_t       in_privsize,
                                        u_int8_t       *in_privbuf,
                                        np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Import CDLC public keys
     *
     * @param[in]  in_pubsize            Length of the Import CDLC public keys
     * @param[in]  in_pubbuf             Buffer of the Import CDLC public keys
     * @praam[out] out_secbuf            Secure buffer containing sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS       in_pubsize is zero or in_pubbuf is NULL
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    np_ret_t
    nplocal_RegistImportCDLCPublicKeys(np_size_t       in_pubsize,
                                       u_int8_t       *in_pubbuf,
                                       np_secbuffer_t *out_secbuf);


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Register Import LDLC public keys
     *
     * @param[in]  in_pubsize            Length of the Import LDLC public keys
     * @param[in]  in_pubbuf             Buffer of the Import LDLC public keys
     * @praam[out] out_secbuf            Secure buffer containing sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS       in_pubsize is zero or in_pubbuf is NULL
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ALREADYREGISTERED
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    np_ret_t
    nplocal_RegistImportLDLCPublicKeys(np_size_t       in_pubsize,
                                       u_int8_t       *in_pubbuf,
                                       np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Set TrustedTime
     *
     * @param[in] in_time    trusted time
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NOMEM
     * @return ::ERR_NP_ILLEGALARGS
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    nplocal_SetTrustedTime(np_date_t *in_time,
                           np_secbuffer_t *out_secbuf);

#ifdef __cplusplus
}
#endif

#endif /* NPLOCAL_API_H */
/** @} */
