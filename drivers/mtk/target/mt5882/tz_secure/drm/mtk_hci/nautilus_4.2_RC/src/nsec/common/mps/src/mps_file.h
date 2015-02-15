/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */

/**
 * @addtogroup mps mps
 * @ingroup goby
 * @{
 *
 * @file
 * @brief Configuration token file handling. For reading from and writing to persistent storage
 */

#ifndef MPS_L_FILE_H
#define MPS_L_FILE_H

/**
 * Write config token to pesistent storage
 *
 * @param[in] in_id  Config token ID
 * @param[in] in_str Config token contents
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_INTERNALERROR
 */
extern np_ret_t
mps_l_WriteConfigToken( binstr_t *in_id, binstr_t *in_str );


/**
 * Read config token from persistent storage
 *
 * @param[in]   in_id    Config token ID
 * @param[out]  out_str  Placeholder for contents
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 * @return ::ERR_NP_FILEERROR
 */
extern np_ret_t
mps_l_ReadConfigToken( binstr_t *in_id, binstr_t **out_str );


/**
 * Check if Config token exists in persistent storage
 *
 * @param[in]   in_id    Config token ID
 * @param[out]  out_flag TRUE if config token is in persistent storage
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 */
extern np_ret_t
mps_l_ConfigTokenIsRegistered( binstr_t *in_id, bool_t *out_flag );


/**
 * Clear config token from persistent storage
 *
 * @param[in]  in_id  Config token ID
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 */
extern np_ret_t
mps_l_RemoveConfigToken( binstr_t *in_id );

/**
 * Reads the contents of the file in to a buffer
 *
 * @param[in]  in_filename  Config token file name
 * @param[out] out_str      Config token contents
 *
 * @return ::ERR_NP_OK
 * @return ::ERR_NP_OUTOFMEMORY
 * @return ::ERR_NP_INTERNALERROR
 */
extern np_ret_t
mps_l_ReadConfigTokenFile( char *in_filename, binstr_t **out_str );

#endif /* MPS_L_FILE_H */
/** @} */
