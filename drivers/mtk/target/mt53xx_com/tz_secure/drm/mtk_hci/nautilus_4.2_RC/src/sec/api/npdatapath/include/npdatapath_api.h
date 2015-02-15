/** @addtogroup npdatapath npdatapath
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef NPDP_API_H
#define NPDP_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Nautilus Library API (Data Path support)
 */
/*===========================================================================*/

#include <npdatapath_types.h>

/*---------------------------------------------------------------------------*/
/**
 * @brief Initialize track handle and set track info
 * you should call np_SetLicense() before calling this API
 *
 * @param[in,out] io_handle     component handle
 * @param[in]     in_trackinfo  track information
 * @param[out]    out_htrack    track handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_LICENSENOTSET
 * @return ::ERR_NP_ALREADYPERFORMED
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_SDATACORRUPTED
 * @return ::ERR_NP_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_InitTrack(hdls_context_t    io_handle,
             np_trackinfo_t   *in_trackinfo,
             np_trackhandle_t *out_htrack);

/*---------------------------------------------------------------------------*/
/**
 * @brief Creates a track based on the track info and returns the handle
 *
 * @param[in]   in_handle     Component handle
 * @param[in]   in_trackinfo  Track information
 * @param[out]  out_htrack    Track handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SDATACORRUPTED
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_CreateTrack(hdls_context_t    in_handle,
               np_trackinfo_t   *in_trackinfo,
               np_trackhandle_t *out_htrack);

/*---------------------------------------------------------------------------*/
/**
 * @brief Creates a key (either randomly generated / app passed)
 * Sets this key into the track context associated
 *
 * @param[in]   in_handle      Component handle
 * @param[in]   in_num_tracks  Number of tracks associated
 * @param[in]   in_htrack      Array of track handles
 * @param[in]   in_pv          Application key data

 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_KEYALREADYEXIST
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_CreateCK(hdls_context_t      in_hdls,
            u_int32_t           in_num_tracks,
            np_trackhandle_t   *in_htrack,
            binstr_t           *in_pv);

/*---------------------------------------------------------------------------*/
/**
 * @brief Finalize track handle
 *
 * @param[in,out] io_handle track handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_FinTrack(np_trackhandle_t io_htrack);

/*---------------------------------------------------------------------------*/
/**
 * @brief decrypt data
 *
 * @param[in,out] io_trackhandle  track handle
 * @param[in]     in_is_final     The Decrypt is the last one of a multipart decrypt and padding is to be applied
 * @param[in]     in_ibsize       size of the input buffer
 * @param[in]     in_buffer       input buffer
 * @param[in]     in_ivsize       size of the IV
 * @param[in]     in_iv           initialization vector
 * @param[in,out] io_obsize       size of the output buffer
 * @param[out]    out_buffer      output buffer
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_DECRYPTERROR
 * @return ::ERR_NP_DECRYPTDENIED
 * @return ::ERR_NP_TOOSMALLBUFFER
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_Decrypt(np_trackhandle_t  io_trackhandle,
           bool_t            in_is_final,
           np_size_t         in_ibsize,
           u_int8_t         *in_buffer,
           np_size_t         in_ivsize,
           u_int8_t         *in_iv,
           np_size_t        *io_obsize,
           u_int8_t         *out_buffer);

/*---------------------------------------------------------------------------*/
/**
 * @brief Decryption for streams
 *
 * @param[in,out] io_trackhandle     Track handle
 * @param[in]     in_offset          Offset from the first byte in the cipher text (high 32 bits)
 * @param[in]     in_offset          Offset from the first byte in the cipher text (low 32 bits)
 * @param[in]     in_ctrsize         Counter size
 * @param[in]     in_ctr             Counter
 * @param[in]     in_ibsize          Input ciphertext length
 * @param[in]     in_buffer          Input ciphertext
 * @param[in,out] io_obsize          Decrypted data length
 * @param[out]    out_buffer         Decrypted data
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_DECRYPTERROR
 * @return ::ERR_NP_DECRYPTDENIED
 * @return ::ERR_NP_TOOSMALLBUFFER
 * @return ::ERR_NP_NOTSUPPORTED
 * @return ::ERR_NP_NOKEY
 */
/*---------------------------------------------------------------------------*/
np_ret_t
np_DecryptWithOffset(np_trackhandle_t  io_trackhandle,
                     np_size_t         in_offset_high,
                     np_size_t         in_offset_low,
                     np_size_t         in_ctrsize,
                     u_int8_t         *in_ctr,
                     np_size_t         in_ibsize,
                     u_int8_t         *in_buffer,
                     np_size_t        *io_obsize,
                     u_int8_t         *out_buffer);

/*---------------------------------------------------------------------------*/
/**
 * @brief Encrypts data
 *
 * @param[in]     in_trackhandle  track handle
 * @param[in]     in_is_final     value indicating final block, padding is applied
 * @param[in]     in_ibsize       size of the plain input buffer
 * @param[in]     in_buffer       plain input buffer
 * @param[in]     in_ivsize       initialization vector size
 * @param[in,out] io_iv           initialization vector
 * @param[in,out] io_obsize       size of the output encrypted buffer
 * @param[in,out] io_buffer       output encrypted buffer
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ENCRYPTERROR
 */
/*---------------------------------------------------------------------------*/
np_ret_t
np_Encrypt(np_trackhandle_t in_trackhandle,
           bool_t           in_is_final,
           np_size_t        in_ibsize,
           u_int8_t        *in_buffer,
           np_size_t        in_ivsize,
           u_int8_t        *io_iv,
           np_size_t       *io_obsize,
           u_int8_t        *io_buffer);

/*---------------------------------------------------------------------------*/
/**
 * @brief Check a HMAC over the supplied data using the key associated with
 *        the supplied track handle
 *
 * @param[in,out] io_trackhandle  track handle
 * @param[in]     in_digestalgo   hashing algorithm to use with the HMAC
 * @param[in]     in_bufsize      size of the input buffer
 * @param[in]     in_buf          input buffer
 * @param[in]     in_hmacsize     size of the hmac value to check against
 * @param[in]     in_hmacval      hamc value to check against
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_DECRYPTERROR
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_CheckHMAC(hdls_context_t   io_trackhandle,
             int32_t          in_digestalgo,
             np_size_t        in_bufsize,
             u_int8_t        *in_buf,
             np_size_t        in_hmacsize,
             u_int8_t        *in_hmacval);

/*---------------------------------------------------------------------------*/
/**
 * @brief Export a key associated with a track
 *
 * The key must be marked as exportable by the service associated with the
 * track
 *
 * @param[in,out] io_trackhandle  track handle
 * @param[in]     in_pv           Private Application data used to protect the
 *                                key being exported
 * @param[out]    out_ck          Platform protected key
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_SDATACORRUPTED
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_EXPORTDENIED
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_ExportGetKey( np_trackhandle_t  in_trackhandle,
                 np_str_t         *in_pv,
                 np_secbuffer_t   *out_ck );

/* -------------------------------------------------------------------------- */
/**
 * @brief Move a key associated with a track outside Nautilus.
 *
 * @note After move, the key may no	 longer used in the same DRM session.
 *
 * @param[in]     in_trackhandle  Track handle
 * @param[in]     in_pv           Private Application data used to protect the
 *                                key being moved
 * @param[out]    out_pv          Platform specific string containing the
 *                                protected key
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
np_MoveKey( np_trackhandle_t   in_trackhandle,
            np_str_t          *in_pv,
            np_str_t         **out_pv );

/*---------------------------------------------------------------------------*/
/**
 * @brief Generate a HMAC over the supplied data using the key associated with
 *        the supplied track handle
 *
 * @param[in,out] io_trackhandle  track handle
 * @param[in]     in_digestalgo   hashing algorithm to use with the HMAC
 * @param[in]     in_bufsize      size of the input buffer
 * @param[in]     in_buf          input buffer
 * @param[out]    out_hmac        generated hmac
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_DECRYPTERROR
 * @return ::ERR_NP_DECRYPTDENIED
 */
/*---------------------------------------------------------------------------*/
extern np_ret_t
np_GenerateHMAC(hdls_context_t  io_trackhandle,
                int32_t         in_digestalgo,
                np_size_t       in_bufsize,
                u_int8_t       *in_buf,
                np_secbuffer_t *out_hmac);

NP_END_EXTERN_C

#endif /* NPDP_API_H */
/** @} */
