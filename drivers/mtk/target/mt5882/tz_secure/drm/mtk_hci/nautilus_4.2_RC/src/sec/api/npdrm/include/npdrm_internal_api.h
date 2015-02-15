/** @addtogroup npdrm npdrm
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010 Sony Corporation.
 */

#ifndef NPDRM_INTERNAL_API_H
#define NPDRM_INTERNAL_API_H

NP_BEGIN_EXTERN_C

extern np_ret_t
np_internal_InitSession( hdls_context_t *out_handle );

extern np_ret_t
np_internal_FinSession( hdls_context_t io_handle );

extern np_ret_t
np_internal_SetLicense( hdls_context_t  io_handle,
                        np_size_t       in_licensesize,
                        u_int8_t       *in_licensebuf );

extern np_ret_t
np_internal_ActionDescribe( hdls_context_t   in_handle,
                            int32_t             in_actiontype,
                            binstr_t       **out_esb );

extern np_ret_t
np_internal_GetTimeConstraints( binstr_t  *in_esb,
                                u_int32_t *out_not_after,
                                u_int32_t *out_not_longer_than );

extern np_ret_t
np_internal_LicenseIDListAppend( binstr_t *in_controller_id,
                                 binstr_t *in_license );

NP_END_EXTERN_C

#endif /* NPDRM_INTERNAL_API_H */
/** @} */
