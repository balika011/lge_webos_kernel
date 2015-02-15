/** @addtogroup reg reg
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */
#ifndef REG_API_H
#define REG_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Registration of secure data
 *
 * This component will be used to (un)register secure data (aka 'sdata'). The
 * sdata will not be written to/read from the file system by this component.
 * Instead, the reg component is used as a cache for the persistent secure
 * store access.
 *
*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * error code
 *---------------------------------------------------------------------------*/

#define ERR_REG_ALREADYREGISTERED           (ERR_REG_BASE|1)
#define ERR_REG_NOPERSONALITYREGISTERED     (ERR_REG_BASE|2)
#define ERR_REG_NOLINKREGISTERED            (ERR_REG_BASE|3)
#define ERR_REG_ILLEGALPERSONALITY          (ERR_REG_BASE|4)
#define ERR_REG_ILLEGALLINK                 (ERR_REG_BASE|5)
#define ERR_REG_NOASSERTIONREGISTERED       (ERR_REG_BASE|6)
#define ERR_REG_ILLEGALASSERTION            (ERR_REG_BASE|7)
#define ERR_REG_NOSLREGISTERED              (ERR_REG_BASE|8)
#define ERR_REG_ILLEGALSL                   (ERR_REG_BASE|9)
#define ERR_REG_NOTFOUND                    (ERR_REG_BASE|10)
#define ERR_REG_UNSUPPORTED                 (ERR_REG_BASE|11)
#define ERR_REG_NOSECURITYDATAREGISTERED    (ERR_REG_BASE|12)
#define ERR_REG_ILLEGALSECURITYDATA         (ERR_REG_BASE|13)
#define ERR_REG_NOCRLREGISTERED             (ERR_REG_BASE|14)
#define ERR_REG_ASSERTIONEXPIRED            (ERR_REG_BASE|15)
#define ERR_REG_INVALIDDURATION             (ERR_REG_BASE|16)
#define ERR_REG_NONODEREGISTERED            (ERR_REG_BASE|17)
#define ERR_REG_REGISTEREDMAX               (ERR_REG_BASE|18)
#define ERR_REG_NOTACTIVE                   (ERR_REG_BASE|19)
#define ERR_REG_REVOKEDCERT                 (ERR_REG_BASE|20)

/*---------------------------------------------------------------------------*
 * id type
 *---------------------------------------------------------------------------*/

#define REG_IDTYPE_LINKTO     0
#define REG_IDTYPE_LINKFROM   1
#define REG_IDTYPE_ACCOUNTID  2

/*---------------------------------------------------------------------------*
 * AccountID size
 *---------------------------------------------------------------------------*/

#define REG_ACCOUNTID_SIZE (20)

/*---------------------------------------------------------------------------*
 * definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * API
 *---------------------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * Account Session
 *--------------------------------------------------------------*/
/* sdata status */
/* personality bit */
#define REG_S_NPUB        (1<<0)     /**< Nemo public key exists */
#define REG_S_NPRV        (1<<1)     /**< Nemo private key exists */
#define REG_S_OPUB        (1<<2)     /**< Octopus public key exists */
#define REG_S_OPRV        (1<<3)     /**< Octopus private key exists */
#define REG_S_ROLE        (1<<4)     /**< Role assertion exists */
/* registration bit */
#define REG_S_SEASHELL_RS   (1<<8)     /**< SeaShell registration status exists */
#define REG_S_SEASHELL_LS   (1<<9)     /**< SeaShell license status exists */
#define REG_S_LIL           (1<<10)    /**< License suspension list exists */
#define REG_S_NMASTER       (1<<11)    /**< Node data exists */
#define REG_S_LMASTER       (1<<12)    /**< Link data exits */
#define REG_S_NODE          (1<<13)    /**< Node exists */
#define REG_S_LINK          (1<<14)    /**< Link exists */
#define REG_S_CRL           (1<<15)    /**< CRL exists */
#define REG_S_SUBNODE       (1<<16)    /**< SubNode exists */
#define REG_S_SUBLINK       (1<<17)    /**< SubLink exists */
/* status bit */
#define REG_S_SUBSREGISTERED    (1<<23)    /**< Nautilus subscription data is registered */
#define REG_S_PERSONALIZED      (1<<24)    /**< Nautilus personality is registered */
#define REG_S_REGISTERED        (1<<25)    /**< Nautilus user data is registered */
#define REG_S_CORRUPTED_PERS    (1<<28)    /**< Nautilus personality is corrupt */
#define REG_S_CORRUPTED_USR     (1<<29)    /**< Nautilus user data is corrupt */
#define REG_S_CORRUPTED_FATAL   (1<<30)    /**< Nautilus registry is corrupt */
#define REG_S_CORRUPTED_USRSUB  (1<<31)    /**< Nautilus subscription data is corrupt */
#define REG_IS_CORRUPTED_PERS(f) (f & REG_S_CORRUPTED_PERS)
#define REG_IS_MISSING_PERS(f)   (!f)
#define REG_IS_CORRUPTED_REG(f) (f & (REG_S_CORRUPTED_USR | REG_S_CORRUPTED_FATAL))
#define REG_IS_CORRUPTED_SUBREG(f) (f & (REG_S_CORRUPTED_USRSUB | REG_S_CORRUPTED_FATAL))

/* macros for checking */
#define REG_S_ISPERS(f)     (!REG_IS_CORRUPTED_PERS(f) && (f&REG_S_NPUB) && (f&REG_S_NPRV) &&  (f&REG_S_OPUB) && (f&REG_S_OPRV) && (f&REG_S_ROLE))
#define REG_S_ISREG(f)      (!REG_IS_CORRUPTED_REG(f) && (f&REG_S_SEASHELL_RS) && (f&REG_S_NMASTER) && (f&REG_S_LMASTER) && (f&REG_S_NODE) && (f&REG_S_LINK))
#define REG_S_ISSUBREG(f)      (!REG_IS_CORRUPTED_SUBREG(f) && (f&REG_S_SEASHELL_RS) && (f&REG_S_NMASTER) && (f&REG_S_LMASTER) && (f&REG_S_SUBNODE) && (f&REG_S_SUBLINK))

extern retcode_t
reg_InitAccountSession(u_int8_t *in_sha1);

extern retcode_t
reg_FinAccountSession(void);

extern binstr_t *
reg_GetActiveAccount(void);

extern retcode_t
reg_CheckRegistrationStatus(binstr_t *in_domainId, u_int32_t *out_status);

extern retcode_t
reg_ClearPersonality(void);

extern retcode_t
reg_CheckPersonalityStatus(u_int32_t *out_status);

extern retcode_t
reg_ClearRegistrationStatus(binstr_t *in_domainId);

extern retcode_t
reg_RecoverSdata(binstr_t *in_domainId);

/*--------------------------------------------------------------*
 * Secure Storage
 *--------------------------------------------------------------*/


/**
 * @brief Returns the secure data cache content in serialized form
 *
 * @param[in] in_dirtyonly  Flag to indicate whether all secure data or
 *                           only the modified ('dirty') secure data should
 *                           be returned.
 * @param[out] out_storage Serialized snapshot of the secure data cache
 *
 * @return ::ssmem_GetSdata
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetSdata(int32_t in_dirtyonly, binstr_t **out_storage);

/*---------------------------------------------------------------------------*/
/**
 * @brief Initializes the secure data cache with given serialized
 *        storage content
 *
 * @param[in] in_storage Serialized secure data storage content
 *
 * @return ::ssmem_SetSdata
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_SetSdata(binstr_t *in_storage);


/*---------------------------------------------------------------------------*/
/**
 * @brief Empty the secure data cache
 *
 * @return ::ssmem_ClearSdata
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_ClearSdata(void);

/*--------------------------------------------------------------*
 * Personality
 *--------------------------------------------------------------*/

typedef enum  {
    REG_PERSONALITY_TYPE_DEVICE_NPPU,     /* NEMO public keys */
    REG_PERSONALITY_TYPE_DEVICE_OPPU,     /* Octopus public personality */
    REG_PERSONALITY_TYPE_DEVICE_NROL,     /* Role assertion */
    REG_PERSONALITY_TYPE_IMPORT_INPU,     /* Import nemo public key*/
    REG_PERSONALITY_TYPE_IMPORT_LROL,     /* Import Role assertion: ls-role */
    REG_PERSONALITY_TYPE_IMPORT_RROL,     /* Import Role assertion: rs-role */
    REG_PERSONALITY_TYPE_IMPORT_IROL,     /* Import Role assertion: ia-role */
    REG_PERSONALITY_TYPE_IMPORT_LOPU,     /* Import lic sign public keys  */
    REG_PERSONALITY_TYPE_IMPORT_ROPU,     /* Import reg public keys */
    REG_PERSONALITY_TYPE_IMPORT_CDLC,     /* Company delegate Control */
    REG_PERSONALITY_TYPE_IMPORT_LDLC,     /* Delegate Control for License */
} reg_pers_type_t;

#if defined( NPOPT_USE_NPLOCAL ) || defined( NPOPT_USE_PPPROC )
/*---------------------------------------------------------------------------*/
/**
 * @brief Register a personality object in the secure data cache
 *
 * @param[in] in_type  Personality type @see reg_pers_type_t
 * @param[in] in_len   Length of the buffer
 * @param[in] in_buf   Buffer which contains a personality structure
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_ALREADYREGISTERED  A personality of the given type
 *                                    has already been registered
 * @return ::reg_l_ConvertPersonalityType
 * @return ::ssmem_SetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegPersonality(reg_pers_type_t in_type, np_size_t in_len, u_int8_t *in_buf);
#endif /* defined( NPOPT_USE_NPLOCAL ) || defined( NPOPT_USE_PPPROC ) */

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve a personality object (public part) from the secure data cache
 *
 * @param[in]  in_type          Personality type @see reg_pers_type_t
 * @param[out] out_personality  Personality node
 *
 * @return ::ERR_OK
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 * @return ::reg_l_ConvertPersonalityType
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetPersonality(reg_pers_type_t in_type, binstr_t **out_personality);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the internal octopus personality from the secure data cache
 *
 * @param[out] out_personality  Octopus personality node (public part)
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 * @return ::ERR_REG_ILLEGALPERSONALITY
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetInternalOctopusPersonality(octobj_node_t **out_personality);

/*---------------------------------------------------------------------------*/
/**
 * @brief Check that a personality object of given type is registered.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_CheckPersonality(reg_pers_type_t in_type);

/*---------------------------------------------------------------------------*/
/**
 * @brief Remove from ssmen a personality object of given type.
 *
 * @param[in] in_type  Personality type
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_DeletePersonality(reg_pers_type_t in_type);

/*---------------------------------------------------------------------------*/
/**
 * @brief  Returns public Octopus personality for License Service (Import)
 *
 * @param[out]    out_num_certs     number of certificates
 * @param[out]    out_certs         list of certificates
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLOPU(np_size_t *out_num_certs, binstr_t ***out_certs);


/*---------------------------------------------------------------------------*/
/**
 * @brief  Returns Company Delegate Control parameters
 *
 * @param[out]  out_company_name      Company name
 * @param[out]  out_entrypoint_name   Entry point name
 * @param[out]  out_delegate_control  Delegate Control
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_ILLEGALPERSONALITY
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetCDLC(binstr_t **out_company_name,
            binstr_t **out_entrypoint_name,
            binstr_t **out_delegate_control);

/**
 * @brief  Returns Delegate Control for Import License parameters
 *
 * @param[out]  out_licserv_path      Absolute path for the License Service for the Delegate Control
 * @param[out]  out_regserv_path      Absolute path for the Registration Service for the Delegate Control
 * @param[out]  out_entrypoint_name   Entry point name
 * @param[out]  out_delegate_control  Delegate Control
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_ILLEGALPERSONALITY
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLDLC(binstr_t **out_licserv_path,
            binstr_t **out_regserv_path,
            binstr_t **out_entrypoint_name,
            binstr_t **out_delegate_control);

/*--------------------------------------------------------------*
 * Link
 *--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 * @brief Register an octopus link in the secure data cache
 * @note if in_sinfo is not NULL, in_svcid cannot be NULL
 *
 * @param[in]  in_toid      NodeID(LinkTo)
 * @param[in]  in_fromid    NodeID(LinkFrom) - may be NULL
 * @param[in]  in_link      Link
 * @param[in]  in_svcid     The service id - may be NULL
 * @param[in]  in_sinfo     The service info - may be NULL
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ssmem_SetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegLinkByID(binstr_t *in_toid, binstr_t *in_fromid, binstr_t *in_link, binstr_t *in_svcid, binstr_t *in_sinfo);

/*---------------------------------------------------------------------------*/
/**
 * @brief De-registers an octopus link from the secure data cache
 *
 * @param[in] in_link         Registered link
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 * @return ::ERR_REG_NOLINKREGISTERED
 * @return ::ssmem_SetObject
 * @return ::ERR_CORRUPTED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_DeregLink(binstr_t *in_link);


/*---------------------------------------------------------------------------*/
/**
 * @brief Return the number of registered links, and create an iterator
 * @note the intended use of this function is as follows:
 *    int32_t n;
 *    void *iterator = NULL;
 *    RET(reg_GetLinkStart(&n,&iterator));
 *    for (i = 0; i <n; i++)
 *        RET(reg_GetLinkNext(&iterator,&link));
 * EXIT:
 *    reg_GetLinkEnd((iterator);
 *
 * @param[out] out_number         Number of registered links
 * @param[out] out_link_iterator  an iterator for use by reg_GetLinkNext and reg_GetLinkEnd
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 * @return ::ERR_CORRUPTED
 * @return ::ERR_REG_REGISTEREDMAX
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLinkStart(int32_t *out_number, void **out_link_iterator);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the next registered link
 *
 * @param[in]  io_link_iterator  Iterator to the registered links list
 * @param[out] out_link          the current link
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLinkNext(void **io_link_iterator, binstr_t **out_link);

/*---------------------------------------------------------------------------*/
/**
 * @brief Ends iteration on links
 *
 * @param[in,out] io_iterator         The iterator on links
 *
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLinkEnd(void *io_link_iterator);

/*---------------------------------------------------------------------------*/
/**
 * @brief Looks up a link by its domainid, and returns the first one found
 *
 * @param[in]  in_domainid  the domain ID to look up
 * @param[out] out_link the link found
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOLINKREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLinkFromDomainID(binstr_t *in_domainid, binstr_t **out_link);

/*---------------------------------------------------------------------------*/
/**
 * @brief Looks up links by their (svcid,sinfo)
 *
 * @param[in]  in_svcid       the service id for the links
 * @param[in]  in_sinfo       the service info for the links
 * @param[out] out_num        the number of links found
 * @param[out] out_links      the links found (as an array of from1, to1, from2, to2, etc), or NULL
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_CORRUPTED
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetLinksForServiceID(binstr_t *in_svcid, binstr_t *in_sinfo, u_int32_t *out_num, binstr_t ***out_links);

/**
 * @brief Looks up a link by its from/to node, and returns the first one found
 *
 * @param[in]  in_from         The from-node: NULL for a personality. Non-NULL for a user node.
 * @param[in]  in_to           The to-node. Should be a user or subscription node. Cannot be NULL.
 * @param[out] out_link_as_xml The link found in case of ERR_OK
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOLINKREGISTERED
 * @return ::ERR_CORRUPTED
 */
extern retcode_t
reg_GetLinkFromNodePair(binstr_t *in_from, binstr_t *in_to, binstr_t **out_link_as_xml);


/**
 * @brief Upgrades a link in master table, from N3.3 format to N3.4
 *
 * @note does nothing if link is missing or is already in N3.4 format
 *
 * @param[in]  in_from         The from-node: NULL for a personality. Non-NULL for a user node.
 * @param[in]  in_to           The to-node. Should be a user or subscription node. Cannot be NULL.
 * @param[in]  in_svcid        the Service ID.
 * @param[in]  in_sinfo        the service info
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_CORRUPTED
 */
extern retcode_t
reg_UpgradeLink(binstr_t *in_from, binstr_t *in_to, binstr_t *in_svcid, binstr_t *in_sinfo);


/*--------------------------------------------------------------*
 * Assertion
 *--------------------------------------------------------------*/

#define REG_ASSERTION_TYPE_ROLE        0  /* Role assertion */
#define REG_ASSERTION_TYPE_DATACERT    1  /* Data certification assertion */
#define REG_ASSERTION_TYPE_VERSION     2  /* Version assertion */
#define REG_ASSERTION_TYPE_CAPABILITY  3  /* Capability assertion */

/*---------------------------------------------------------------------------*/
/**
 * @brief Check that an assertion of the given type has been registered,
 *        that it is a valid assertion and that its conditions are met.
 *
 * @param[in] in_type Assertion type
 *
 * @see REG_ASSERTION_TYPE_ROLE
 * @see REG_ASSERTION_TYPE_DATACERT
 * @see REG_ASSERTION_TYPE_VERSION
 * @see REG_ASSERTION_TYPE_CAPABILITY
 *
 * @note REG_ASSERTION_TYPE_DATACERT is not supported
 *
 * @todo Check if this function is also obsolete
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOASSERTIONREGISTERED
 * @return ::ERR_REG_ILLEGALASSERTION
 * @return ::ERR_REG_UNSUPPORTED
 * @return ::ERR_REG_ILLEGALPERSONALITY
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_CheckAssertion(int32_t in_type);

/*---------------------------------------------------------------------------*/
/**
 * @brief Check that a Data Certification Standard (DCS) assertion has been
 *              registered for a given DCS name, that it is a valid assertion and
 *              that its conditions are met.
 *
 * @param[in] in_dcsnlen  Length of the buffer
 * @param[in] in_dcsnbuf  Buffer containing a data certification standard name
 * @param[in] in_validity xsd:duration string which indicates the maximum acceptable
 *                        age of the DCSA (OPTIONAL)
 *
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_NOASSERTIONREGISTERED
 * @return ::ERR_REG_ILLEGALASSERTION
 * @return ::ERR_REG_UNSUPPORTED
 * @return ::ERR_REG_ILLEGALPERSONALITY
 * @return ::ERR_REG_ASSERTIONEXPIRED
 * @return ::ERR_REG_INVALIDDURATION
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_CheckDCSAssertion(np_size_t in_dcsnlen,
                      u_int8_t *in_dcsnbuf,
                      binstr_t *in_validity);


/*---------------------------------------------------------------------------*/
#if defined( NPOPT_ENABLE_CAPABILITY_SIGNALLING )
/**
 * @brief Get the capabilities for this Marlin implementation.
 *
 * The returned structure is an assertion containing the capabilities
 * of a particular build of Nautilus.
 *
 * @param[in]  in_manufacturer Mandatory manufacturer to add to capability assertion
 * @param[in]  in_model        Mandatory model to add to capability assertion
 * @param[in]  in_app_id       Optional application ID to add to capability assertion
 * @param[out] out_assertion   The assertion to populate with the capabilities
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS   <br>A mandatory argument is NULL.
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetCapabilityAssertion(binstr_t  *in_manufacturer,
                           binstr_t  *in_model,
                           binstr_t  *in_app_id,
                           binstr_t **out_assertion);
#endif

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve a role assertion from the secure data cache
 *
 * @param[out] out_assertion  Assertion
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_NOASSERTIONREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetRoleAssertion(binstr_t **out_assertion);

#if defined(NPOPT_ENABLE_DCS)
/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve a registered DCS assertion which matches to a given DCS name.
 *
 * @param[in]  in_dcsnlen     Length of the buffer
 * @param[in]  in_dcsnbuf     Buffer which contains the Data Certification Standard Name
 * @param[out] out_assertion  DCSA assertion
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_NOASSERTIONREGISTERED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetDCSAssertion(np_size_t in_dcsnlen, u_int8_t *in_dcsnbuf, binstr_t **out_assertion);
#endif

/*---------------------------------------------------------------------------*/
/**
 * @brief Register a DCS assertion in the secure data cache.
 *
 * @param[in] in_dcsnlen Length of the DCS name buffer
 * @param[in] in_dcsnbuf Buffer which contains the Data Certification Standard name
 * @param[in] in_len     Length of the assertion buffer
 * @param[in] in_buf     Buffer which contains the DCS assertion
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_ILLEGALASSERTION
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegDCSAssertion(np_size_t in_dcsnlen, u_int8_t *in_dcsnbuf, np_size_t in_len, u_int8_t *in_buf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve an internal assertion from the secure data cache
 *
 * @note Internal means it is extracted from a (Nemo) personality node.
 *
 * @param[in]  in_type          Assertion type
 * @param[out] out_personality  Assertion
 *
 * @see REG_ASSERTION_TYPE_ROLE
 * @see REG_ASSERTION_TYPE_DATACERT
 * @see REG_ASSERTION_TYPE_VERSION
 * @see REG_ASSERTION_TYPE_CAPABILITY
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_REG_ILLEGALPERSONALITY
 * @return ::ERR_REG_NOPERSONALITYREGISTERED
 * @return ::ERR_REG_UNSUPPORTED
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetInternalAssertion(int32_t in_type, octobj_assertion_t **out_assertion);

#if defined(NPOPT_ENABLE_DCS)
/*---------------------------------------------------------------------------*/
/**
 * @brief Return the number of registered DCS Assertions
 *
 * @param[out] out_number Number of registered assertions
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetDCSACount(int32_t *out_number);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the i-th registered assertion where i corresponds to the given index
 *
 * @note This function can be used to iterate over all registered assertions.
 * @note The first index number is 0.
 *
 * @param[in]  in_index  Index
 * @param[out] out_assertion  DCS Assertion
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_REG_NOASSERTIONREGISTERED
 * @return ::ERR_REG_ILLEGALASSERTION
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetDCSAFromIndex(int32_t in_index, octobj_assertion_t **out_assertion);

#endif

/*--------------------------------------------------------------*
 * License Suspension List
 *--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 * @brief Register a License Suspension List in the secure data cache.
 *
 * This function registers a license suspension list. If the reset flag of the
 * new suspension list is set, then it will replace the one in the secure data cache.
 * In the other case the new suspension list will be appended to the existing
 * suspension list.
 *
 * @specref Marlin Broadband Delivery System Specification V1.2 Final
 *          Section 6.2
 *
 * @todo octobj_sl_t structure assumes that the license suspension list is applicable
 *       to only 1 namespace?
 *
 * @param[in] in_sl  License suspension update list
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_ILLEGALSL
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegSuspensionList(octobj_sl_updates_t *in_sl);

/**
 * Get the serialized form of the master suspension list
 *
 * @param[out] out_sl pointer to the string to store list
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_REG_NOSLREGISTERED
 */
extern retcode_t
reg_GetSuspensionList(binstr_t **out_sl);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the serial number of the registered License Suspension List
 *
 * @param[out] out_serial Serial number
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_NOSLREGISTERED
 * @return ::ERR_REG_ILLEGALSL
 *
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetSuspensionListSerial(binstr_t *in_sn, int32_t *out_serial);

/*--------------------------------------------------------------*
 * Security Metadata
 *--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 *
 * @brief Register the security metadata structure. Overwrite the
 * previous security metadata.
 *
 * @param[in] in_len    length of the buffer
 * @param[in] in_buf    buffer containing the security metadata structure
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegSecurityMetadata(u_int32_t in_len, u_int8_t *in_buf);


/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the security metadata structure.
 *
 * @param[out] out_securitydata    security metadata structure [XML]
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_NOSECURITYDATAREGISTERED
 *
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetSecurityMetadata(binstr_t **out_securitydata);


/*---------------------------------------------------------------------------*/
/**
 *
 * @brief Check the consistency of the stored security metadata
 * structure. Check that the xml structure is well-formed. Check the
 * presence of element SecurityMetadata. If SecurityMetadata/Assertion
 * elements are present, check their associated conditions.
 *
 * @return ::ERR_OK
 * @return ::ERR_REG_NOSECURITYDATAREGISTERED
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_REG_ILLEGALSECURITYDATA
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_CheckSecurityMetadata(void);

/*--------------------------------------------------------------*
 * Seashell
 *--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 *
 * @brief Register the seashell DB.
 *
 * If the new seashell DB equals to the old one then nothing is done (avoid writing in ssmem when nothing has changed).
 *
 * @param[in] in_new_seashell    The seashell DB to be registered.
 * @param[in] in_old_seashell    The seashell DB to be compared with. Put it to NULL to force the registration of in_new_seashell into Reg.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_SetSeaShell(mbinstr_t *in_new_seashell, mbinstr_t *in_old_seashell);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the seashell D/B.
 *
 * @param[out] out_seashell    seashell D/B
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 *
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetSeaShell(mbinstr_t **out_seashell);

/*--------------------------------------------------------------*
 * CRL
 *--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 * Gets CRL from Nautilus' registry.
 *
 * @return ::ERR_OK
 * @return ::ERR_REG_NOCRLREGISTERED
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
retcode_t
reg_GetCRL(binstr_t **out_crl);

/*---------------------------------------------------------------------------*/
/**
 * Saves CRL in Nautilus' registry.
 *
 * @return ::ERR_OK
 * @return ::ERR_REG_ALREADYREGISTERED
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
retcode_t
reg_RegCRL(binstr_t *in_crl);

/*---------------------------------------------------------------------------*/
/**
 * Check if CRL is registered in Nautilus' registry.
 *
 * @return ::ERR_OK
 * @return ::ERR_REG_NOCRLREGISTERED
 */
/*---------------------------------------------------------------------------*/
retcode_t
reg_CheckCRL(void);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register an octopus node in the secure data cache
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ssmem_SetObject
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegNodeByID(binstr_t *in_id, binstr_t *in_node);

/*---------------------------------------------------------------------------*/
/**
 * @brief Deregister link from sdata
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 * @return ::ssmem_SetObject
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_DeregLinkFromTo(binstr_t *in_fromid, binstr_t *in_id);

/*---------------------------------------------------------------------------*/
/**
 * @brief Get an octopus node from the secure data cache
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetNodeByID(binstr_t *in_id, binstr_t **out_node);

/*---------------------------------------------------------------------------*/
/**
 * @brief Deregister an octopus node from the secure data cache
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_DeregNodeByID(binstr_t *in_id);


/**
 * @brief Checks the integrity of the links/nodes master table & file in the secure data cache
 * and fixes it if any corruption is found.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ssmem_GetObject
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_CheckAndRestoreIntegrity(void);

/*--------------------------------------------------------------*
 * LIL(License id list)
 *--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 * Gets LIL from Nautilus' registry.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
retcode_t
reg_GetLIL(binstr_t **out_lil);

/*---------------------------------------------------------------------------*/
/**
 * Saves LIL in Nautilus' registry.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
retcode_t
reg_RegLIL(binstr_t *in_lil);

NP_END_EXTERN_C

#endif /* REG_API_H */
/** @} */

