/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef SMI_UTILS_H
#define SMI_UTILS_H

NP_BEGIN_EXTERN_C

/**
 * @brief Free a crtstdlist from a specific memory context
 *
 * @param[in] in_context  The memory context containing the allocated memory
 * @param[in] in_list     The crtsdtlist to free
 */
void
smi_l_freeCrtSdtList( npi_cmncontext_t in_context, np_crt_std_list_t *in_list );

/**
 * @brief Free a certstandard list from a specific memory context
 *
 * @param[in] in_context  The memory context containing the allocated memory
 * @param[in] in_list     The certstandard list to free
 */
void
smi_l_freeCertStdList( npi_cmncontext_t in_context, np_certstandard_list_t *in_list );

/**
 * @brief Free a trackhandle list from a specific memory context
 *
 * @param[in] in_context  The memory context containing the allocated memory
 * @param[in] in_list     The trackhandle list to free
 */
void
smi_l_freeTrackHandleList( npi_cmncontext_t in_context, np_trackhandle_list_t *in_list );

/**
 * @brief Free a SAC service info structure from a specific memory context
 *
 * @param[in] in_context  The memory context containing the allocated memory
 * @param[in] in_info     The SAC service info
 */
void
smi_l_freeSacServInfo( npi_cmncontext_t in_context, np_sac_msg_info_t *in_info );

/**
 * @brief Free a Cert fault info structure from a specific memory context
 *
 * @param[in] in_context  The memory context containing the allocated memory
 * @param[in] in_list     The fault info struct to free
 */
void
smi_l_freeCertFaultInfo( npi_cmncontext_t in_context, np_faultinfo_t *in_faultinfo );

#if defined( NPOPT_ENABLE_MS3 )
/**
 * @brief Free MS3 result structure from a specific memory context
 *
 * @param[in] in_context    The memory context containing the allocated memory
 * @param[in] in_ms3result  The result info struct to free
 */
void
smi_l_freeMS3Results( npi_cmncontext_t in_context, np_ms3_sas_info_t *in_ms3result );
#endif
NP_END_EXTERN_C

#endif /* SMI_UTILS_H  */
