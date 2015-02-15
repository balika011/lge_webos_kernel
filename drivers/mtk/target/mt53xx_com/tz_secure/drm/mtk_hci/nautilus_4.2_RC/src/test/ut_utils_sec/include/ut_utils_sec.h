/** @addtogroup ut_utils ut_utils
 *  @ingroup ut
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 *
 */
#ifndef UT_UTILS_SEC_H
#define UT_UTILS_SEC_H

NP_BEGIN_EXTERN_C

/** @file
 * This library contains the Unit Test Utilities Secure functionalities
 *
 * @require base ut_utils
 *
 * @author Riccardo Ballarino <Riccardo.Ballarino@eu.sony.com>
 */

/**
 * @brief  Set Trusted Time
 *
 * @return ::ERR_OK
 * @return ::ERR_NG
 */
retcode_t
ut_SetTrustedTime(void);

/**
 * @brief  Set Trusted Time with delay
 *
 * @param[in] in_year_offset  Offset the number of years in TT by this much
 *
 * @return ::ERR_OK
 * @return ::ERR_NG
 */
retcode_t
ut_SetTrustedTimeWithDelay( int32_t in_year_offset );

/**
 * @brief  Construct NEMO Personality form files
 *
 * @param[in]  in_nemopub_fn       Nemo Public  File Name
 * @param[in]  in_roleassertion_fn Role Assertions File Name
 * @param[out] out_nemonode        Nemo Node
 *
 * @return ::ut_ConstructNEMOPersonality
 * @return ::ut_ReadFileBinstr
 */
retcode_t
ut_ConstructNEMOPersonalityFromFile(u_int8_t  *in_nemopub_fn,
                                    u_int8_t  *in_roleassertion_fn,
                                    void     **out_nemonode); /* xml2oct_nemonode_t ** */

/**
 * @brief  Octopus Personalisation
 *
 * @param[in] in_nemoxml   Nemo node Pub
 * @param[in] in_nemoass   Assertions
 * @param[out] out_nemo          Nemo node
 * @param[out] out_roleassertion Role Assertions Node
 * @param[out] out_dcsassertion  DCS  Assertions Node
 *
 * @return ::ut_ConstructNEMOPersonality
 * @return ::xml2oct_ParseAssertion
 */
retcode_t
ut_NemoPersonalization( binstr_t *in_nemoxml,
                        binstr_t *in_nemoass,
                        void **out_nemo,          /* xml2oct_nemonode_t ** */
                        void **out_roleassertion, /* octobj_assertion_t ** */
                        void **out_dcsassertion    /* octobj_assertion_t ** */
                      );

/**
 * @brief  Octopus Personalisation
 *
 * @param[in]  in_pubxml      Octopus Public
 * @param[in,out] out_personality  Personality
 *
 * @return ::xml2oct_ParsePublicPersonality
 * @return ::ut_ParseOctopusPrivatePersonality
 */
retcode_t
ut_OctPersonalization( binstr_t  *in_pubxml,
                       void     **out_personality ); /* octobj_node_t** */

/**
 * @brief  Octopus Personalisation From Public files
 *
 * @param[in]  file           Octopus Public
 * @param[out] io_personality  Personality
 *
 * @return ::ut_ReadFileBinstr
 * @return ::ut_OctPersonalization
 */
retcode_t
ut_OctPersonalizationFormFile( char  *in_pub_file,
                               void **out_personality ); /* octobj_node_t**  */


/**
 * @brief  Get Octopus License Object from file
 *
 * @param[in]  in_filename  Octopus Public
 * @param[out] out_license  Octopus License Object
 *
 * @return ::ut_ReadFileBinstr
 * @return ::xml2oct_ParseLicense
 */
retcode_t
ut_GetOctLicenseObj( const char *in_filename  , void **out_license ); /* xml2oct_license_t ** */

NP_END_EXTERN_C

#endif /* UT_UTILS_SEC_H */
/** @} */


