/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef KEYMNG_UTILS_H
#define KEYMNG_UTILS_H

NP_BEGIN_EXTERN_C

/**
 * @brief Groups the error codes and returns a value
 *
 * @param[in]    in_err    Error Code
 *
 * @return ::ERR_KEYMNG_CANNOTDECRYPTED
 * @return ::ret
 *
 */
extern retcode_t
keymng_l_GroupErr(retcode_t in_err);

/**
 * @brief Creates a hci key reference for the given algo type
 *
 * @param[in]    in_encalgo    Algorithm type
 * @param[out]   out_ref       HCI reference
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 *
 */
extern retcode_t
keymng_l_GetOpprHciKeyRef(unsigned char   in_encalgo,
                          hci_key_ref_t  *out_ref);

NP_END_EXTERN_C

#endif /* KEYMNG_UTILS_H */

