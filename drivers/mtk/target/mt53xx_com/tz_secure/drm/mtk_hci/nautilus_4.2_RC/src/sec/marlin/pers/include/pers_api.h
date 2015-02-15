/** @addtogroup pers pers
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef PERS_API_H
#define PERS_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*===========================================================================*/
    /** @file
     * @brief library for Personalization
     *
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * error code definitions
     *---------------------------------------------------------------------------*/

#define ERR_PERS_ILLEGALXML          (ERR_PERS_BASE|1)
#define ERR_PERS_CRYPTOERROR         (ERR_PERS_BASE|2)
#define ERR_PERS_SOAPFAULT           (ERR_PERS_BASE|3)
#define ERR_PERS_TTNOTAVAILABLE      (ERR_PERS_BASE|4)
#define ERR_PERS_NOSOAPFAULT         (ERR_PERS_BASE|5)
#define ERR_PERS_ILLEGALCERT         (ERR_PERS_BASE|6)
#define ERR_PERS_MESSAGECORRUPTED    (ERR_PERS_BASE|7)
#define ERR_PERS_UNSUPPORTED         (ERR_PERS_BASE|8)
#define ERR_PERS_REVOKEDCERT         (ERR_PERS_BASE|9)

    /*---------------------------------------------------------------------------*
     * type definitions
     *---------------------------------------------------------------------------*/

    typedef void *pers_handle_t;

    /*---------------------------------------------------------------------------*
     * APIs
     *---------------------------------------------------------------------------*/

    /**
     * @brief Initialize a PERS handle
     *
     * @param[out] out_handle  PERS handle
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    pers_Init(pers_handle_t *out_handle);

    /**
     * @brief Finalize a PERS handle
     *
     * This function finalizes (deallocates) a PERS handle. This function should
     * be called only once for a given handle.
     *
     * @note The handle is not set to NULL.
     *
     * @param[in,out] io_handle PERS protocol handle
     */
    extern retcode_t
    pers_Fin(pers_handle_t io_handle);

    /**
     * @brief Generate a Nemo Personalization request
     *
     * This function generates a Nemo Personalization request message.
     *
     * @specref NEMO Personalization Protocol R1.0
     *
     * @param[in,out] io_handle  Protocol handle
     * @param[out]    out_req    Request message
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return result from ::xml_ComposeElement
     * @return result from ::nemo_ComposeSOAP
     * @return result from ::pers_l_ConvErr
     */
    extern retcode_t
    pers_GenPersonalityRequest(pers_handle_t   io_handle,
                               binstr_t      **out_req);


    /**
     * @brief Parse a Nemo personalization challenge request
     *
     * This function parses a Nemo personalization challenge request message:
     * - Extract the Server's public signing key from the certificate chain
     *   provided in the <ds:Signature>/<ds:X509Certificate> element.
     * - Verify the signature
     * - Check that SessionID matches with the one sent in the initial personalization
     *   request message.
     * - Verify that the server encryption certificate chain is valid
     *   under the boot root certificate
     *
     * @specref NEMO Personalization Protocol R1.0
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_NOMEM
     * @return ::ERR_PERS_ILLEGALXML
     * @return result from ::l_verifySignature
     * @return result from ::pers_l_ConvErr
     * @return result from ::b64_Decode
     * @return result from ::cert_marlin_VerifyChain
     * @return result from ::xml_ParseBuf
     * @return result from ::xml_Find
     * @return result from ::xml_GetContent
     */
    extern retcode_t
    pers_RecvChallengeRequest(pers_handle_t  io_handle,
                              np_size_t      in_msglen,
                              u_int8_t      *in_msgbuf);

    /**
     * @brief Generate a Nemo personalization challenge response
     *
     * This function generates a Nemo personalization challenge response message
     * by performing the following steps:
     * - Generate a Session Key which will be used by the server to encrypt
     *   the PERS personality node.
     * - Generate a ChallengeResponse
     * - Sign the response with the secret bootstrap signing key
     * - Encrypt the challenge response with the server's public encryption key
     * - Construct the PERSPersonalizationChallengeResponse message
     *
     * @specref NEMO Personalization Protocol R1.0
     *
     * @param[in,out] io_handle         PERS protocol handle
     * @param[out]    out_resp          Response message
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return result from ::l_genChallengeResponseSessionData
     * @return result from ::l_genChallengeResponse
     * @return result from ::l_genEncryptedData
     * @return result from ::nemo_ComposeSOAP
     * @return result from ::pers_l_ConvErr
     * @return result from ::rsa_Init
     * @return result from ::xml_ComposeElement
     * @return result from ::xml_ExclC14N
     * @return result from ::xml_CreatePKISignature
     */
    extern retcode_t
    pers_GenChallengeResponse(pers_handle_t   io_handle,
                              binstr_t      **out_resp);

    /**
     * @brief Parse a Nemo personality reponse
     *
     * This function parses a Nemo personality response message and
     * extracts the Nemo personality.
     *
     * @specref NEMO Personalization Protocol R1.0
     *
     * @param[in,out]  io_handle        PERS protocol handle
     * @param[in]      in_msglen        Response message length
     * @param[in]      in_msgbuf        Response message buffer
     * @param[out]     out_personality  Nemo personality
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_NOMEM
     * @return ::ERR_PERS_ILLEGALXML
     * @return result from ::l_verifySignature
     * @return result from ::pers_l_ConvErr
     * @return result from ::b64_Decode
     * @return result from ::crypto_Init
     * @return result from ::crypto_Decrypt
     * @return result from ::xml_ParseBuf
     * @return result from ::xml_Find
     * @return result from ::xml_GetContent
     * @return result from ::xml_GetAttr
     * @return result from ::xml_ParseAndReplace
     */
    extern retcode_t
    pers_RecvPersonalityResponse(pers_handle_t   io_handle,
                                 np_size_t       in_msglen,
                                 u_int8_t       *in_msgbuf,
                                 binstr_t      **out_personality);

    /**
     * @brief Set keytype to the personalization context
     *
     * @param[in,out]  io_handle        PERS protocol handle
     * @param[in]      in_keytype       Key type
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    pers_SetKeyType(pers_handle_t   io_handle,
                    binstr_t       *in_keytype);

    /**
     * @brief Set Business token to the personalization context
     *
     * @param[in,out]  io_handle           PERS protocol handle
     * @param[in]      in_business_token   Business Token
     *
     * @return ::ERR_OK
     * @return ::ERR_ILLEGALHANDLE
     * @return ::ERR_ILLEGALARGS
     * @return ::ERR_NOMEM
     */
    extern retcode_t
    pers_SetBusinessToken(pers_handle_t   io_handle,
                          binstr_t       *in_business_token);

#ifdef __cplusplus
}
#endif

#endif    /* PERS_API_H */
/** @} */
