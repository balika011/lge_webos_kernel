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

#ifndef NSSMI_DATAPATH_API_H
#define NSSMI_DATAPATH_API_H

NP_BEGIN_EXTERN_C

/**
 * @brief Initializes the decryption engine for a given track
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_trackinfo        Track information (this is copied internally and can be de-allocated
 *                                 immediately after the call to nsnp_InitTrack)
 * @param[out] out_htrack          Track context handle
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_InitTrack
 */
extern np_ret_t
nssmi_InitTrack(np_handle_t       in_handle,
                np_trackinfo_t   *in_trackinfo,
                np_trackhandle_t *out_htrack);

/**
 * @brief Creates the track used for importing or encrypting content
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_trackinfo        Track information (this is copied internally and can be de-allocated
 *                                 immediately after the call to nsnp_CreateTrack)
 * @param[out] out_htrack          Track context handle
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_trackinfo or out_htrack
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_CreateTrack
 */
extern np_ret_t
nssmi_CreateTrack(np_handle_t       in_handle,
                  np_trackinfo_t   *in_trackinfo,
                  np_trackhandle_t *out_htrack);


/**
 * @brief Creates Content Key
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_num_tracks       Number of tracks associated with the key
 * @param[in]  in_tracks           Array of track handles associated with the key
 * @param[in]  in_pvsize           Size of the key wrapped with platform specific data
 * @param[in]  in_pvbuf            Buf has the key wrapped with platform specific data
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_tracks or in_num_tracks
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_CreateCK
 */

extern np_ret_t
nssmi_CreateCK(np_handle_t       in_handle,
               u_int32_t         in_num_tracks,
               np_trackhandle_t *in_tracks,
               np_size_t         in_pvsize,
               u_int8_t         *in_pvbuf);


/**
 * @brief Finalizes the decryption engine
 *
 * @param[in]     in_handle           @ref example_smi "SMI" target info
 * @param[in,out] io_htrack           Track context handle
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_FinTrack
 */
extern np_ret_t
nssmi_FinTrack(np_handle_t in_handle, np_trackhandle_t io_htrack);


/**
 *  @brief Encrypts a block of plain content
 *
 * @param[in]     in_handle           @ref example_smi "SMI" target info
 * @param[in,out] io_trackhandle      Track context handle
 * @param[in]     in_is_final         The Encrypt is the last one of a multipart encrypt and padding is to be applied
 * @param[in]     in_ibsize           Input buffer size
 * @param[in]     in_buffer           Plain buffer
 * @param[in]     in_ivsize           Input IV size
 * @param[in]     in_iv               IV buffer
 * @param[in,out] io_obsize           Output buffer size
 * @param[in,out] io_buffer           Encrypted buffer
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_Encrypt
 */
np_ret_t
nssmi_Encrypt(np_handle_t        in_handle,
              np_trackhandle_t   io_trackhandle,
              bool_t             in_is_final,
              np_size_t          in_ibsize,
              u_int8_t          *in_buffer,
              np_size_t          in_ivsize,
              u_int8_t          *in_iv,
              np_size_t         *io_obsize,
              u_int8_t          *io_buffer);

/**
 * @brief Decrpts a block of encrypted content
 *
 * @param[in]     in_handle           @ref example_smi "SMI" target info
 * @param[in,out] io_trackhandle      Track context handle
 * @param[in]     in_is_final         The Decrypt is the last one of a multipart decrypt and padding is to be applied
 * @param[in]     in_ibsize           Input buffer size
 * @param[in]     in_buffer           Encrypted buffer
 * @param[in]     in_ivsize           The IV
 * @param[in]     in_iv               The IV size
 * @param[in,out] io_obsize           Output buffer size
 * @param[out]    out_buffer          Output plaintext buffer
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_Decrypt
 */
extern np_ret_t
nssmi_Decrypt(np_handle_t       in_handle,
              np_trackhandle_t  io_trackhandle,
              bool_t            in_is_final,
              np_size_t         in_ibsize,
              u_int8_t         *in_buffer,
              np_size_t         in_ivsize,
              u_int8_t         *in_iv,
              np_size_t        *io_obsize,
              u_int8_t         *out_buffer);

/**
 * @brief Decryption for streams
 *
 * @param[in]     in_handle          @ref example_smi "SMI" target info
 * @param[in,out] io_trackhandle     Track context handle
 * @param[in]     in_offset          Offset from the first byte in the cipher text (high 32 bits)
 * @param[in]     in_offset          Offset from the first byte in the cipher text (low 32 bits)
 * @param[in]     in_ctrsize         Counter size
 * @param[in]     in_ctr             Counter
 * @param[in]     in_ibsize          Input ciphertext length
 * @param[in]     in_buffer          Input ciphertext
 * @param[in,out] io_obsize          Decrypted data length
 * @param[out]    out_buffer         Decrypted data
 */
extern np_ret_t
nssmi_DecryptWithOffset(np_handle_t       in_handle,
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
 * @brief Checks the integrity of the metadata in track content
 *
 * @param[in] in_handle           @ref example_smi "SMI" target info
 * @param[in] in_trackhandle      Track context handle
 * @param[in] in_digestalgo       Hash algorithm
 * @param[in] in_bufsize          Size of the data
 * @param[in] in_buf              Data to check
 * @param[in] in_hmacsize         Lenght of the HMAC
 * @param[in] in_hmacval          The HMAC
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_CheckHMAC
 */
extern np_ret_t
nssmi_CheckHMAC(np_handle_t      in_handle,
                np_trackhandle_t in_trackhandle,
                int32_t          in_digestalgo,
                np_size_t        in_bufsize,
                u_int8_t        *in_buf,
                np_size_t        in_hmacsize,
                u_int8_t        *in_hmacval);

/**
 * @brief Generate HMAC of  given data
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_trackhandle      Track context handle
 * @param[in]  in_digestalgo       Hash algorithm
 * @param[in]  in_bufsize          Size of the data
 * @param[in]  in_buf              Data to check
 * @param[out] out_hmac            Generated HMAC
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_GenHMAC
 */
extern np_ret_t
nssmi_GenerateHMAC(np_handle_t      in_handle,
                   np_trackhandle_t io_trackhandle,
                   int32_t          in_digestalgo,
                   np_size_t        in_bufsize,
                   u_int8_t        *in_buf,
                   binstr_t       **out_hmac);

/**
 * @brief Gets the content key
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_trackhandle      Track context handle
 * @param[in]  in_pv               Pointer to a context used by foreign memory
 * @param[out] out_ck              The content key
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_ExportGetKey
 */
extern np_ret_t
nssmi_ExportGetKey(np_handle_t        in_handle,
                   np_trackhandle_t   in_trackhandle,
                   binstr_t          *in_pv,
                   binstr_t         **out_ck);

/**
 * @brief Move a content key into another memory space (used by HCI)
 *
 * @param[in]  in_handle           @ref example_smi "SMI" target info
 * @param[in]  in_trackhandle      Track context handle
 * @param[in]  in_pv               Pointer to a context used by foreign memory
 * @param[out] out_pv              Memory returned from move
 *
 * @return ::ERR_NP_OK                 <br>Indicating success
 * @return ::ERR_NP_OUTOFMEMORY        <br>Unable to allocate memory
 * @return ::ERR_NP_ILLEGALARGS        <br>NULL was specified in in_node or out_nodeid
 * @return ::ERR_NP_ILLEGALHANDLE      <br>The @ref example_smi "SMI" handle is invalid
 * @return ::ERR_NP_SMI_MSG_CORRUPTED  <br>SMI communication failed.
 * @return ::np_MoveKey
 */
extern np_ret_t
nssmi_MoveKey(np_handle_t        in_handle,
              np_trackhandle_t   in_trackhandle,
              binstr_t          *in_pv,
              binstr_t         **out_pv);

NP_END_EXTERN_C

#endif /* NSSMI_DATAPATH_API_H */


/** @} */
