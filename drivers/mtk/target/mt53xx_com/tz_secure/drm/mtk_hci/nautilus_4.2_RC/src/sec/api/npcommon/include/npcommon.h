/** @addtogroup npcommon npcommon
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef NPCOMMON_H
#define NPCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief Nautilus Library common header file
     *
     * @author Norifumi Goto <n-goto@sm.sony.co.jp>
     * @date   08 Jun, 2005 last modified by n-goto
     */
    /*===========================================================================*/

#include <npcommon_types.h>

    /*---------------------------------------------------------------------------*
     * assertion type definitions
     *---------------------------------------------------------------------------*/
#define NP_ASSERTION_DATACERT   0
#define NP_ASSERTION_MAX        1

    /**
     * def: TRUE, FALSE (if it is not yet defined).
     */

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif


    /*---------------------------------------------------------------------------*/
    /**
     * @brief Initialize Nautilus library
     *
     * This API should be invoked first of all
     *
     * @param[in] in_membody the pointer to working memory of nautilus or NULL for self-allocation
     * @param[in] in_memsize the size of working memory of nautilus or NP_DEFAULT_MEM_SIZE
     * @return ::ERR_NP_OK
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_Init(void *in_membody, np_size_t in_memsize);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Finalize Nautilus library
     *
     * This API should be invoked at the end of use of the component
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_NOTINITIALIZED
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_Fin(void);

    /**
     * @brief Check the personality
     *
     * @param[in,out] io_type                   Bitmask specifying the types to check and returning the types which are valid.
     *
     * @return ::ERR_NP_OK                      OK.
     * @return ::ERR_NP_NOPERSONALITYREGISTERED If the personality files are not valid or not registered.
     * @return ::ERR_NP_ILLEGALARGS             If in_type not valid.
     * @return ::ERR_NP_INTERNALERROR           Internal error.
     * @return ::ERR_NP_SDATACORRUPTED          Smem corrupted.
     */
    extern np_ret_t
    np_StatPersonality(u_int32_t *io_type);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief check whether personality has been already registered or not
     *
     * @param[in] in_type personality type (NP_PERSONALITY_NEMO/OCTPUB/OCTPRIV)
     * @return ::ERR_NP_OK                            (the personality has been registered)
     * @return ::ERR_NP_NOPERSONALITYREGISTERED       (the personality has not been registered yet)
     * @return ::ERR_NP_ILLEGALARGS                   (unknown personality type was specified in in_type)
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_CheckPersonality(int32_t in_type);

#if defined(NPOPT_ENABLE_DCS)
    /*---------------------------------------------------------------------------*/
    /**
     * @brief check whether specified assertion has been already registered or not
     *
     * @param[in] in_dcsnlen length of data certification standard name
     * @param[in] in_dcsnbuf buffer of data certification standard name
     * @param[in] in_validity xsd:duration string which indicates the maximum acceptable
     *                         age of the DCSA (OPTIONAL)
     * @return ::ERR_NP_OK                            (the assertion has been registered)
     * @return ::ERR_NP_ASSERTIONNOTFOUND             (the assertion has not been registered yet or is outdated)
     * @return ::ERR_NP_ILLEGALARGS                   (unknown assertion type was specified in in_type)
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_CheckDCSAssertion(np_size_t in_dcsnlen, u_int8_t *in_dcsnbuf, np_str_t *in_validity);
#endif

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get NodeID
     *
     * @param[in] in_node  buffer of node(XML format)
     * @param[out] out_nodeid buffer where the NodeID will is stored
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY           (unable to allocate memory)
     * @return ::ERR_NP_ILLEGALARGS           (NULL was specified in in_node or out_nodeid)
     * @return ::ERR_NP_ILLEGALNODE           (illegal node)
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_GetNodeID(np_str_t *in_node, np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get the serialized secure data
     * @note  This includes only the modified secure data
     *
     * @param[out] out_sdata Serialized secure data
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     *
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_GetSdata(np_secbuffer_t *out_secbuf);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Initialize the secure data cache with the given data
     *
     * @param[in] in_sdata Serialized secure data
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_SetSdata(np_str_t *in_sdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Clear the secure data cache
     *
     * @return ::ERR_NP_OK
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_ClearSdata(void);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief copy buffer from secure to nonsecure
     * secure buffer will be freed after copy
     *
     * @param[in,out] in_secbuffer handle of secure buffer
     * @param[in,out] io_buffer nonsecure buffer
     * @return ::ERR_NP_OK
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_CopyAndFreeSecureBuffer(np_secbuffer_t *io_secbuffer,
                               np_str_t       *io_buffer);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Free a memory that was allocated in the secure part
     *
     * @param[in,out] io_secmem secure memory
     * @return ::ERR_NP_OK
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_FreeSecureMem(void *io_secmem);

    /*---------------------------------------------------------------------------*/
    /**
     * This function checks if the current CRL is up-to-date. If it is not,
     * it tries to get a new one from the registry and checks if this one
     * is fresher. If it is so, it sets it as the current one.
     *
     * @param [io] io_is_crl_fresh A pointer to a buffer which will hold
     *                             TRUE if the current list is up-to-date
     *                             and FALSE if it is not. The result is
     *                             trusted only if the function returns
     *                             ::ERR_NP_OK.
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_IsCRLUptodate(np_bool_t *io_is_crl_fresh);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get the CRL from sdata.
     *
     * @param[out] out_secbuf_crl   CRL
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_OUTOFMEMORY
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_GetCRL(np_secbuffer_t *out_secbuf_crl);

    /*---------------------------------------------------------------------------*/
    /**
     * This function updates a given CRL by setting it as the current one and
     * saves it in the Nautilus' registry.
     *
     * @param [in] in_fetched_crl A pointer to a new CRL.
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_UpdateCRL(np_str_t *in_fetched_crl, np_secbuffer_t  *out_secbuf_sdata);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get the current CRL number
     *
     * @return the current CRL number or a np_cert_crlnumber_t struct with .valid==0 if it has not been set.
     */
    /*---------------------------------------------------------------------------*/
    extern np_cert_crlnumber_t
    np_GetCRLNumber(void);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Get a CRL Distribution Point from a given cert chain.
     *
     * @param[in]  in_cert_chain_b64enc   Certificate chain
     * @param[out] out_sec_crl_dp         CRL Distribution Point
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALCERTSET
     */
    /*---------------------------------------------------------------------------*/
    extern np_ret_t
    np_GetCRLDistributionPoint(np_str_t       *in_cert_chain_b64enc,
                               np_secbuffer_t *out_sec_crl_dp);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Clear the CRL cache.
     */
    /*---------------------------------------------------------------------------*/
    extern void
    np_ClearCRL(void);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Clear the CRL cache.
     */
    /*---------------------------------------------------------------------------*/
    extern void
    np_cd_ClearCRL(void);

    /**
     * Initialize account session
     *
     * @param[in] in_accountid A pointer to a buffer containing a Account ID
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     */
    extern np_ret_t
    np_InitAccountSession(np_str_t *in_accountid);

    /**
     * Finalize account session
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     */
    extern np_ret_t
    np_FinAccountSession(void);

    /**
     * Check sdata status
     *
     * @param[in] in_accountid A pointer to a buffer containing a Account ID
     * @param[out] out_status A pointer to a buffer containing a sdata status
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_SDATACORRUPTED
     * @return ::ERR_NP_NOTACTIVE
     */

    extern np_ret_t
    np_CheckSdataStatus(np_str_t *in_accountid, u_int32_t *out_status);

    /**
     * Clear registration status
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     */
    extern np_ret_t
    np_ClearRegistrationStatus(
        np_str_t        *in_accountid,
        np_secbuffer_t  *out_secbuf_sdata);

    extern np_ret_t
    np_RecoverSdata(
        np_str_t        *in_accountid,
        np_secbuffer_t  *out_secbuf_sdata);

    /**
     * @brief Get a list of from-to nodes
     * Return format is from_1, to_1, from_2, to_2, ..., from_n, to_n
     *
     * @param[out]  out_num           Number of nodes returned
     * @param[out[  out_links         from-to node string list
     * @param[out]  out_secbuf_sdata  Sdata if errors cause link garbage collection
     *
     * @return ::ERR_NP_OK
     */
    extern np_ret_t
    np_GetAvailableLinks(u_int32_t *out_num, np_str_t ***out_links, np_secbuffer_t *out_secbuf_sdata);

    /**
     * @brief Get a list of links with given (svcid, sinfo) service info.
     * Possibly upgrade an N3.3 link to N3.4 in master table.
     * Return format is from_1, to_1, from_2, to_2, ..., from_n, to_n
     *
     * @param[in]   in_svcid    service id
     * @param[in]   in_sinfo    service info
     * @param[in]   in_domainid NULL, or domainid for N3.3 record in master table
     * @param[out]  out_num     Number of nodes returned
     * @param[out[  out_links   from-to node string list
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_NOPERSONALITYREGISTERED
     * @return ::ERR_NP_NOLINKREGISTERED
     * @return ::ERR_NP_SDATACORRUPTED
     */
    extern np_ret_t
    np_GetLinksForServiceID(
        np_str_t   *in_svcid,
        np_str_t   *in_sinfo,
        np_str_t   *in_domainid,
        u_int32_t  *out_num,
        np_str_t ***out_links);

    /**
     * @brief Delete a link based on the from and to nodes
     *
     * @param[in]  in_from           Source node
     * @param[in]  in_to             Destination node
     * @param[out] out_secbuf_sdata  Sdata after link deletion
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_OUTOFMEMORY
     * @return ::ERR_NP_NONODEREGISTERED
     * @return ::ERR_NP_NOLINKREGISTERED
     */
    extern np_ret_t
    np_DeregLinkFromTo(np_str_t       *in_from,
                       np_str_t       *in_to,
                       np_secbuffer_t *out_secbuf_sdata);

    /**
     * @brief Get the trusted time
     *
     * @param[out] out_time         The trusted time
     * @param[out] out_secbuf       Sdata
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_ILLEGALARGS
     * @return ::ERR_NP_TTNOTAVAILABLE
     * @return ::nssmi_GetTrustedTime
     */
    extern np_ret_t
    np_GetTrustedTime(np_date_t *out_time);

    /**
     * @brief Returns the DUID parameters
     *
     * @param[in] in_cinfo      Context Information
     * @param[out] out_request  Buffer containing DUID container
     *
     * @return ::ERR_NP_OK
     * @return ::ERR_NP_INTERNALERROR
     * @return ::ERR_NP_OUTOFMEMORY
     */
    np_ret_t
    np_GetDUIDParameters(u_int32_t        in_cinfo,
                         np_secbuffer_t  *out_request );

#ifdef __cplusplus
}
#endif

#endif /* NPCOMMON_H */
/** @} */
