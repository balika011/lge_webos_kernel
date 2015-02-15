/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef NPSAC_API_H
#define NPSAC_API_H

NP_BEGIN_EXTERN_C

extern np_ret_t
npsac_InitSession(hdls_context_t    *out_sac_handle);

extern np_ret_t
npsac_FinSession(hdls_context_t      in_sac_handle);

extern np_ret_t
npsac_GenerateRequest(hdls_context_t       in_sac_handle,
                      np_sac_handle_type_t in_serv_hdl_type,
                      hdls_context_t       in_serv_handle,
                      np_sac_msg_t         in_msg_type,
                      np_sac_msg_info_t   *in_msg_info,
                      u_int32_t            in_transaction_id,
                      np_secbuffer_t      *out_request);

extern np_ret_t
npsac_ProcessRequest(hdls_context_t        in_sac_handle,
                     np_sac_handle_type_t  in_serv_hdl_type,
                     hdls_context_t        in_serv_handle,
                     np_str_t             *in_request,
                     u_int32_t            *out_transaction_id,
                     np_secbuffer_t       *out_response);

extern np_ret_t
npsac_ParseResponse(hdls_context_t       in_sac_handle,
                    np_sac_handle_type_t in_serv_hdl_type,
                    hdls_context_t       in_serv_handle,
                    np_str_t            *in_response,
                    u_int32_t           *out_transaction_id);

NP_END_EXTERN_C

#endif
