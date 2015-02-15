/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */
/** @addtogroup nsnp_ipmp IPMP functionality
 *  @ingroup nsnp
 * @{
 *
 * @brief nsnp layer APIs for IPMP
 *
 * @see NautilusAPIReference.pdf
 *
 * @file
 * IPMP APIs
 *
 */


#ifndef NSNP_IPMP_API_H
#define NSNP_IPMP_API_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                                                     PUBLIC DATA STRUCTURES */
/* -------------------------------------------------------------------------- */

/** scheme type ACBC */
#define NP_IPMP_SINF_SCHEME_TYPE       0x41434243

/** scheme version */
#define NP_IPMP_SINF_SCHEME_VERSION    0x0100

/** stream type fixed */
#define NP_IPMP_SINF_STREAMTYPE_AUDIO  "urn:marlin:organization:sne:content-type:audio"
#define NP_IPMP_SINF_STREAMTYPE_VIDEO  "urn:marlin:organization:sne:content-type:video"

/**
 * IPMP sinf Header type
 *
 * Type to specify Header Info of a sinf box
 *
 **/
typedef struct np_ipmpsinf_s {
    u_int32_t    scheme_type;
    u_int16_t    scheme_version;
    char        *stream_type;
    char        *cid;
} np_ipmp_sinf_t;

/* -------------------------------------------------------------------------- */
/*                                                                 PUBLIC API */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/**
 * @brief Generates a SINF box (including HMAC) from supplied data
 *
 * @param[in]     in_handle           Import session handle
 * @param[in]     in_trackhandle      track handle
 * @param[in]     in_sinfinfo         SINF header info
 * @param[out]    out_sinf_box        Binary string containing a sinf box
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::nssmi_GenerateHMAC
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_ipmpGenerateSINFBox(np_handle_t          in_handle,
                         np_trackhandle_t     in_trackhandle,
                         np_ipmp_sinf_t      *in_sinfinfo,
                         np_str_t           **out_sinfbox);
NP_END_EXTERN_C

#endif /* NSNP_IPMP_API_H */
/** @} */
