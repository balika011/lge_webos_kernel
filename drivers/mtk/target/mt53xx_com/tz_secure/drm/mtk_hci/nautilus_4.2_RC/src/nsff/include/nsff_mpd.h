/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 */

/** @addtogroup nsff mpd
 *  @ingroup nsff
 * @{
 *
 * @file
 * @brief Support for parsing of Marlin specific Presentation Description data
 *
 * @see NautilusAPIReference.pdf
 */

#ifndef NSFF_MPD_H
#define NSFF_MPD_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                          PUBLIC DATA STRUCTURES                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief MPD mode type
 *
 * Type to specify the Marlin Media Presentation Description mode detected
 */
typedef enum {
    NP_MPD_BASELINE = 0,
    NP_MPD_FULL     = 1
} np_mpd_mode_t;

/**
 * @brief MPD encryption type formats
 *
 * Type to specify the Marlin Media Presentation Description encryption format types
 * Only valid in the case of FULL mode.
 */
typedef enum {
    NP_MPD_ENC_FORMAT_NOT_SET  = 0,
    NP_MPD_ENC_FORMAT_BBTS_AES = 1,
    NP_MPD_ENC_FORMAT_BBTS_CSA = 2,
    NP_MPD_ENC_FORMAT_BBTS_SKL = 3,
    NP_MPD_ENC_FORMAT_ACBC     = 4,
    NP_MPD_ENC_FORMAT_ACGK     = 5,
    NP_MPD_ENC_FORMAT_CENC_CTR = 6,
    NP_MPD_ENC_FORMAT_CENC_CGK = 7
} np_mpd_enc_format_t;

/**
 * @brief MPD Marlin format
 *
 * Parsed MPD format
 */
typedef struct {
    np_mpd_mode_t        mode;
    u_int32_t            major;                   /* Defaults to 1 if not present, but MUST be 1 if present */
    u_int32_t            minor;                   /* Defaults to 0 if not present, but MUST be 0 if present */
    u_int32_t            num_content_ids;         /* If 0 then content_ids are NULL */
    np_str_t           **content_ids;
    np_str_t            *silent_rights_url;       /* NULL if not present */
    np_str_t            *preview_rights_url;      /* NULL if not present */
    np_str_t            *rights_issuer_url;       /* NULL if not present */
    np_bool_t            uris_templated;
    np_mpd_enc_format_t  enc_format;
} np_mpd_t;

/* -------------------------------------------------------------------------- */
/*                             PUBLIC API                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Parse the mpd
 *
 * @param[in]     in_mpd_len         Length of the MPD
 * @param[in]     in_mpd_str         String containing the MPD
 * @param[out]    out_mpd            Parsed mpd
 *
 * It is assumed that the caller will have looked at the uuid before calling
 * parsing function
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_MPD_INVALID
 */
extern np_ret_t
nsff_mpdParseDescription(np_size_t   in_mpd_len,
                         u_int8_t   *in_mpd,
                         np_mpd_t  **out_mpd);

/**
 * @brief Free the mpd struct
 *
 * @param[in,out]     io_mpd         mpd structure to be freed
 */
extern np_ret_t
nsff_mpdFreeDescription(np_mpd_t      *io_mpd);

NP_END_EXTERN_C

#endif /* NSFF_MPD_H */
/** @} */
