/** @addtogroup nssmi nssmi
 *  @ingroup nsec
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef NSSMI_LOCAL_API_H
#define NSSMI_LOCAL_API_H

NP_BEGIN_EXTERN_C

/*---------------------------------------------------------------------------*/
/**
 * @brief Register NEMO public keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size   Length of the NEMO public keys
 * @param[in] in_buf    Buffer of the NEMO public keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nplocal_RegistNEMOPublicKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistNEMOPublicKeys(np_handle_t in_handle,
                           np_size_t in_size,
                           u_int8_t  *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register NEMO private keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size   Length of the NEMO private keys
 * @param[in] in_buf    Buffer of the NEMO private keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nplocal_RegistNEMOPrivateKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistNEMOPrivateKeys(np_handle_t in_handle,
                            np_size_t in_size,
                            u_int8_t  *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register role assertion
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size   Length of the role assertion
 * @param[in] in_buf    Buffer of the role asserion
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nplocal_RegistRoleAssertion
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistRoleAssertion(np_handle_t in_handle,
                          np_size_t in_size,
                          u_int8_t  *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register a public octopus personality
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_pubsize  Length of the public personality XML
 * @param[in] in_pubbuf   Buffer of the public personality XML
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nplocal_RegistPublicPersonality
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistPublicPersonality(np_handle_t in_handle,
                              np_size_t in_pubsize,
                              u_int8_t *in_pubbuf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register a private octopus personality
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_privsize Length of the private personality XML
 * @param[in] in_privbuf  Buffer of the private personality XML
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::nplocal_RegistPrivatePersonality
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistPrivatePersonality(np_handle_t in_handle,
                               np_size_t in_privsize,
                               u_int8_t *in_privbuf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register a dcs assertion.
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_dcsnsize  Length of the data certification standard name
 * @param[in] in_dcsnbuf   Buffer of the data certification standard name
 * @param[in] in_size      Length of the dcs assertion XML
 * @param[in] in_buf       Buffer of the dcs assertion XML
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::nplocal_RegistDCSAssertion
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistDCSAssertion(np_handle_t in_handle,
                         np_size_t in_dcsnsize,
                         u_int8_t *in_dcsnbuf,
                         np_size_t in_size,
                         u_int8_t *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register Import License service public keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size   Length of the Import License public keys
 * @param[in] in_buf    Buffer of the Import License public keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::nplocal_RegistImportLSPubKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistImportLSPubKeys(np_handle_t  in_handle,
                            np_size_t    in_size,
                            u_int8_t    *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register Import License service private keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size   Length of the Import License private keys
 * @param[in] in_buf    Buffer of the Import License private keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::nplocal_RegistImportLSPrivKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistImportLSPrivKeys(np_handle_t  in_handle,
                             np_size_t    in_size,
                             u_int8_t    *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register Import Nemo public keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size        Length of the Import Nemo public keys
 * @param[in] in_buf         Buffer of the Import Nemo public keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::nplocal_RegistImportNemoPubKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistImportNemoPubKeys(np_handle_t  in_handle,
                              np_size_t    in_size,
                              u_int8_t    *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register Import Nemo private keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size        Length of the Import Nemo private keys
 * @param[in] in_buf         Buffer of the Import Nemo private keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::nplocal_RegistImportNemoPrivKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistImportNemoPrivKeys(np_handle_t  in_handle,
                               np_size_t    in_size,
                               u_int8_t    *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register Import CDLC public keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size        Length of the Import CDLC public keys
 * @param[in] in_buf         Buffer of the Import CDLC public keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::nplocal_RegistImportCDLCPublicKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistImportCDLCPubKeys(np_handle_t  in_handle,
                              np_size_t    in_size,
                              u_int8_t    *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register Import LDLC public keys
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_size        Length of the Import LDLC public keys
 * @param[in] in_buf         Buffer of the Import LDLC public keys
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS        given size is 0 or given buf is NULL
 * @return ::ERR_NP_ALREADYREGISTERED
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::nplocal_RegistImportLDLCPublicKeys
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_RegistImportLDLCPubKeys(np_handle_t  in_handle,
                              np_size_t    in_size,
                              u_int8_t    *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Set the trusted time
 *
 * @param[in] in_handle      Component handle and SMI target info
 * @param[in] in_time    Trusted time
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOMEM
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::nplocal_SetTrustedTime
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
nssmi_SetTrustedTime(np_handle_t in_handle, np_date_t *in_time);

NP_END_EXTERN_C

#endif /* NSSMI_LOCAL_API_H */


/** @} */
