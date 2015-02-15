/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 *
 */

#ifndef MODEL_API_H
#define MODEL_API_H

NP_BEGIN_EXTERN_C

/**
 * Returns a DUID parameter container
 *
 * @param[in]  in_cinfo        Porting specific code described in porting manual
 * @param[out] out_duid        The non-NUL-terminated DUID parameter container
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
extern retcode_t
model_GetDUIDParameters(u_int32_t  in_cinfo,
                        binstr_t **out_duid);

/**
 * Return a fingerprint to identify this device (e.g. during personalization)
 *
 * @param[out] out_fingerprint    The non-NUL-terminated fingerprint
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
extern retcode_t
model_GetFingerprint(binstr_t **out_fingerprint);

NP_END_EXTERN_C

#endif /* MODEL_API_H */
