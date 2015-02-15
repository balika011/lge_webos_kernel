/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

/** @addtogroup nsnp_datapath Marlin DRM
 *  @ingroup nsnp
 * @{
 *
 * @file
 * Data Path APIs
 *
 * @see NautilusAPIReference.pdf
 */


#ifndef NSNP_DATAPATH_API_H
#define NSNP_DATAPATH_API_H

NP_BEGIN_EXTERN_C

/* Handle can be ms3, drm, bbts, ... */

/* -------------------------------------------------------------------------- */
/*                             PUBLIC API                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/**
 * @brief Initializes a track session for decryption
 *
 * @param[in]     in_handle     Component handle for the session
 * @param[in]     in_trackinfo  Track information
 * @param[out]    out_htrack    Track handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SDATACORRUPTED
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_InitTrack(np_handle_t       in_handle,
               np_trackinfo_t   *in_trackinfo,
               np_trackhandle_t *out_htrack);

/* -------------------------------------------------------------------------- */
/**
 * @brief Initializes the track session for encryption
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
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_CreateTrack(np_handle_t       in_handle,
                 np_trackinfo_t   *in_trackinfo,
                 np_trackhandle_t *out_htrack);

/* -------------------------------------------------------------------------- */
/**
 * @brief Creates a content key, either randomly generated (if in_pvsize=0 and in_pvbuf=NULL) or
 * app passed. The key is to be used for content encryption and it is associated
 * with the content IDs in the supplied tracks.
 * Sets this key into the track context associated.
 *
 * @param[in]   in_handle      Component handle
 * @param[in]   in_num_tracks  Number of tracks associated
 * @param[in]   in_tracks      Array of track handles
 * @param[in]   in_pvsize      Application key data size
 * @param[in]   in_pvbuf       Application key data

 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_KEYALREADYEXIST
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_CreateCK(np_handle_t       in_handle,
              u_int32_t         in_num_tracks,
              np_trackhandle_t *in_tracks,
              np_size_t         in_pvsize,
              u_int8_t         *in_pvbuf);

/* -------------------------------------------------------------------------- */
/**
 * @brief Finalize track handle
 *
 * @param[in]     in_handle      Component handle
 * @param[in,out] io_handle      track handle
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_FinTrack(np_handle_t in_handle, np_trackhandle_t io_htrack);

/* -------------------------------------------------------------------------- */
/**
 * @brief Decrypts a part of content block using the given IV (Initialization
 * Vector)
 *
 * @param[in]     in_handle       Component handle
 * @param[in,out] io_trackhandle  track handle
 * @param[in]     in_is_final     The Decrypt is the last one of a multipart
 *                                decrypt and padding is to be applied
 * @param[in]     in_ivsize       size of the IV
 * @param[in,out] io_iv           IV buffer. This is overwritten with the IV needed for
 *                                the next time this is called. This cannot be NULL.
 * @param[in]     in_ibsize       size of the input buffer
 * @param[in]     in_buffer       input buffer
 * @param[in,out] io_obsize       size of the output buffer
 * @param[out]    out_buffer      output buffer
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_DECRYPTERROR
 * @return ::ERR_NP_DECRYPTDENIED
 * @return ::ERR_NP_TOOSMALLBUFFER
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_DecryptWithIV(np_handle_t       in_handle,
                   np_trackhandle_t  io_trackhandle,
                   np_bool_t         in_is_final,
                   np_size_t         in_ivsize,
                   u_int8_t         *io_iv,
                   np_size_t         in_ibsize,
                   u_int8_t         *in_buffer,
                   np_size_t        *io_obsize,
                   u_int8_t         *out_buffer);

/**
 * @brief Decryption for streams
 *
 * @param[in]     in_handle          np context handle
 * @param[in,out] io_trackhandle     Track context handle
 * @param[in]     in_offset_high     Offset from the first byte in the cipher text (high 32 bits)
 * @param[in]     in_offset_low      Offset from the first byte in the cipher text (low 32 bits)
 * @param[in]     in_ctrsize         Counter size
 * @param[in]     in_ctr             Counter
 * @param[in]     in_ibsize          Input ciphertext length
 * @param[in]     in_buffer          Input ciphertext
 * @param[in,out] io_obsize          Decrypted data length
 * @param[out]    out_buffer         Decrypted data
 */
extern np_ret_t
nsnp_DecryptWithOffset(np_handle_t       in_handle,
                       np_trackhandle_t  io_trackhandle,
                       np_size_t         in_offset_high,
                       np_size_t         in_offset_low,
                       np_size_t         in_ctrsize,
                       u_int8_t         *in_ctr,
                       np_size_t         in_ibsize,
                       u_int8_t         *in_buffer,
                       np_size_t        *io_obsize,
                       u_int8_t         *out_buffer);

/**
 * @brief Encrypts a block of plain content
 *
 * @param[in]     in_handle           np context handle
 * @param[in,out] io_trackhandle      Track context handle
 * @param[in]     in_is_final         The Encrypt is the last one of a multipart
                                      encrypt and padding is to be applied
 * @param[in]     in_ivsize           Input IV size
 * @param[in,out] io_iv               IV buffer. This is overwritten with the IV needed for
 *                                    the next time this is called. This cannot be NULL.
 * @param[in]     in_ibsize           Input buffer size
 * @param[in]     in_buffer           Plain buffer
 * @param[in,out] io_obsize           Output buffer size
 * @param[out]    out_buffer          Encrypted buffer
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_TOOSMALLBUFFER
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_ENCRYPTERROR
 */
extern np_ret_t
nsnp_EncryptWithIV(np_handle_t       in_handle,
                   np_trackhandle_t  io_trackhandle,
                   np_bool_t         in_is_final,
                   np_size_t         in_ivsize,
                   u_int8_t         *io_iv,
                   np_size_t         in_ibsize,
                   u_int8_t         *in_buffer,
                   np_size_t        *io_obsize,
                   u_int8_t         *out_buffer);

/* -------------------------------------------------------------------------- */
/**
 * @brief Decrypts a content block
 *
 * @param[in]     in_handle       Component handle
 * @param[in,out] io_trackhandle  track handle
 * @param[in]     in_ibsize       size of the input buffer
 * @param[in]     in_buffer       input buffer
 * @param[in,out] io_obsize       size of the output buffer
 * @param[out]    out_buffer      output buffer
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_TOOSMALLBUFFER
 * @return ::ERR_NP_DECRYPTERROR
 * @return ::ERR_NP_DECRYPTDENIED
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_Decrypt(np_handle_t       in_handle,
             np_trackhandle_t  io_trackhandle,
             np_size_t         in_ibsize,
             u_int8_t         *in_buffer,
             np_size_t        *io_obsize,
             u_int8_t         *out_buffer);

/* -------------------------------------------------------------------------- */
/**
 * @brief Check a HMAC over the supplied data using the key associated with
 *        the supplied track handle
 *
 * @param[in]     in_handle       Component handle
 * @param[in]     in_trackhandle  track handle
 * @param[in]     in_digestalgo   hashing algorithm to use with the HMAC
 * @param[in]     in_bufsize      size of the input buffer
 * @param[in]     in_buf          input buffer
 * @param[in]     in_hmacsize     size of the hmac value to check against
 * @param[in]     in_hmacval      hamc value to check against
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_DECRYPTERROR
 * @return ::ERR_NP_ILLEGALTRANSPORTLAYER
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_CheckHMAC(np_handle_t      in_handle,
               np_trackhandle_t in_trackhandle,
               int              in_digestalgo,
               np_size_t        in_bufsize,
               u_int8_t        *in_buf,
               np_size_t        in_hmacsize,
               u_int8_t        *in_hmacval);

/* -------------------------------------------------------------------------- */
/**
 * @brief Export a key associated with a track
 *
 * @note The key must be marked as exportable by the service associated with the
 * track
 *
 * @param[in]     in_handle       Component handle
 * @param[in]     in_trackhandle  track handle
 * @param[in]     in_pv           Private Application data used to protect the
 *                                key being moved
 * @param[out]    out_ck          Platform protected key
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_ILLEGALARGS
 * @return ::ERR_NP_ILLEGALHANDLE
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_SDATACORRUPTED
 * @return ::ERR_NP_NOKEY
 * @return ::ERR_NP_EXPORTDENIED
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_CORRUPTED
 * @return ::ERR_NP_SMI_MSG_TYPE_MISMATCH
 * @return ::ERR_NP_FILEERROR (when built with -enable_multithread or -enable_multiprocess)
 */
/* -------------------------------------------------------------------------- */
extern np_ret_t
nsnp_ExportGetKey( np_handle_t        in_handle,
                   np_trackhandle_t   in_trackhandle,
                   np_str_t          *in_pv,
                   np_str_t         **out_ck );

/* -------------------------------------------------------------------------- */
/**
 * @brief Move a key associated with a track outside Nautilus.
 *
 * @note After move, the key may no longer used in the same DRM session.
 *
 * @param[in]     in_handle       Component handle
 * @param[in]     in_trackhandle  track handle
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
nsnp_MoveKey( np_handle_t        in_handle,
              np_trackhandle_t   in_trackhandle,
              np_str_t          *in_pv,
              np_str_t         **out_pv );

NP_END_EXTERN_C

#endif /* NSNP_DATAPATH_API_H */
/** @} */
