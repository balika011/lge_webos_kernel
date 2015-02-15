/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2009,2010,2011,2012 Sony Corporation.
 */

extern np_ret_t
np_smi_l_ConvErrR(retcode_t in_ret);

extern np_ret_t
smi_sec_l_Process_INIT_SESSION(smi_params_t   *in_req_params_list,
                               smi_params_t  **out_resp_params_list,
                               np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_FIN_SESSION(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_STAT_PERSONALITY(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CHECK_PERSONALITY(smi_params_t   *in_req_params_list,
                                    smi_params_t  **out_resp_params_list,
                                    np_ret_t       *out_resp_ret);

#if defined(NPOPT_ENABLE_DCS)
extern np_ret_t
smi_sec_l_Process_CHECK_DCSASSERTION(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);
#endif

extern np_ret_t
smi_sec_l_Process_CHECK_CRL_UPTODATE(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_GET_CRL(smi_params_t   *in_req_params_list,
                          smi_params_t  **out_resp_params_list,
                          np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CHECK_SDATA_STATUS(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_GET_NODEID(smi_params_t   *in_req_params_list,
                             smi_params_t  **out_resp_params_list,
                             np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CRL_UPDATE(smi_params_t   *in_req_params_list,
                             smi_params_t  **out_resp_params_list,
                             np_ret_t       *out_resp_ret,
                             binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_CRL_GETNUMBER(smi_params_t   *in_req_params_list,
                                smi_params_t  **out_resp_params_list,
                                np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_GET_DISTRIBUTIONPOINT(smi_params_t   *in_req_params_list,
                                        smi_params_t  **out_resp_params_list,
                                        np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CLEAR_REGISTRATIONSTATUS(smi_params_t  *in_req_params_list,
                                           smi_params_t **out_resp_params_list,
                                           np_ret_t      *out_resp_ret,
                                           binstr_t     **out_sdata);

extern np_ret_t
smi_sec_l_Process_INIT_ACCOUNTSESSION(smi_params_t   *in_req_params_list,
                                      smi_params_t  **out_resp_params_list,
                                      np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_RECOVER_SDATA(smi_params_t     *in_req_params_list,
                                smi_params_t    **out_resp_params_list,
                                np_ret_t         *out_resp_ret,
                                binstr_t        **out_sdata);

extern np_ret_t
smi_sec_l_Process_SET_SDATA(smi_params_t     *in_req_params_list,
                            smi_params_t    **out_resp_params_list,
                            np_ret_t         *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_STORE_REMOTE_SDATA(smi_params_t     *in_req_params_list,
                                     smi_params_t    **out_resp_params_list,
                                     np_ret_t         *out_resp_ret,
                                     binstr_t        **out_sdata);

#if defined(NPOPT_USE_NPPERS)
extern np_ret_t
smi_sec_l_Process_INIT_PERS_HANDLE(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_FIN_PERS_HANDLE(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_NEMO_PERS_1(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_NEMO_PERS_2(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_NEMO_PERS_3(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret,
                              binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_SET_PERS_KEYTYPE(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SET_PERS_BUSINESSTOKEN(smi_params_t   *in_req_params_list,
                                         smi_params_t  **out_resp_params_list,
                                         np_ret_t       *out_resp_ret);
#endif /* defined(NPOPT_USE_NPPERS) */

extern np_ret_t
smi_sec_l_Process_INIT_BB_HANDLE(smi_params_t   *in_req_params_list,
                                 smi_params_t  **out_resp_params_list,
                                 np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_FIN_BB_HANDLE(smi_params_t   *in_req_params_list,
                                smi_params_t  **out_resp_params_list,
                                np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_BB_NODEACQ_1(smi_params_t   *in_req_params_list,
                               smi_params_t  **out_resp_params_list,
                               np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_BB_NODEACQ_2(smi_params_t  *in_req_params_list,
                               smi_params_t **out_resp_params_list,
                               np_ret_t      *out_resp_ret,
                               binstr_t     **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_LINKACQ_1(smi_params_t   *in_req_params_list,
                               smi_params_t  **out_resp_params_list,
                               np_ret_t       *out_resp_ret,
                               binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_LINKACQ_2(smi_params_t  *in_req_params_list,
                               smi_params_t **out_resp_params_list,
                               np_ret_t      *out_resp_ret,
                               binstr_t     **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_LICACQ_1(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret,
                              binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_LICACQ_2(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret,
                              binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_DEREG_1(smi_params_t    *in_req_params_list,
                             smi_params_t   **out_resp_params_list,
                             np_ret_t        *out_resp_ret,
                             binstr_t       **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_DEREG_2(smi_params_t    *in_req_params_list,
                             smi_params_t   **out_resp_params_list,
                             np_ret_t        *out_resp_ret,
                             binstr_t       **out_sdata);

extern np_ret_t
smi_sec_l_Process_BB_SET_CERTSET(smi_params_t   *in_req_params_list,
                                 smi_params_t  **out_resp_params_list,
                                 np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_BB_SET_SID(smi_params_t   *in_req_params_list,
                             smi_params_t  **out_resp_params_list,
                             np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_BB_SET_ATTESTATION(smi_params_t  *in_req_params_list,
                                     smi_params_t **out_resp_params_list,
                                     np_ret_t      *out_resp_ret);
extern np_ret_t
smi_sec_l_Process_BB_GETSOAPFAULT(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SET_LICENSE(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret,
                              binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_INIT_TRACK(smi_params_t   *in_req_params_list,
                             smi_params_t  **out_resp_params_list,
                             np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CREATE_TRACK(smi_params_t   *in_req_params_list,
                               smi_params_t  **out_resp_params_list,
                               np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CREATE_CK(smi_params_t   *in_req_params_list,
                            smi_params_t  **out_resp_params_list,
                            np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_FIN_TRACK(smi_params_t   *in_req_params_list,
                            smi_params_t  **out_resp_params_list,
                            np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_DRM_ACTION(smi_id_t         in_msg_type,
                             smi_params_t    *in_req_params_list,
                             smi_params_t   **out_resp_params_list,
                             np_ret_t        *out_resp_ret,
                             binstr_t       **out_sdata);

extern np_ret_t
smi_sec_l_Process_DRM_CALLBACK(smi_params_t  *in_req_params_list,
                               smi_params_t **out_resp_params_list,
                               np_ret_t      *out_resp_ret,
                               binstr_t     **out_sdata);

extern np_ret_t
smi_sec_l_Process_DRM_LINKCONSTRAINTDESCRIBE(smi_params_t   *in_req_params_list,
                                             smi_params_t  **out_resp_params_list,
                                             np_ret_t       *out_resp_ret,
                                             binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_DRM_DECRYPT(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_DRM_DECRYPT_WITH_OFFSET(smi_params_t   *in_req_params_list,
                                          smi_params_t  **out_resp_params_list,
                                          np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_DRM_ENCRYPT(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_DRM_CHECK_HMAC(smi_params_t   *in_req_params_list,
                                 smi_params_t  **out_resp_params_list,
                                 np_ret_t       *out_resp_ret);

#if defined(NPOPT_ENABLE_DCS)
extern np_ret_t
smi_sec_l_Process_DATA_CERTIFICATION_1(smi_params_t   *in_req_params_list,
                                       smi_params_t  **out_resp_params_list,
                                       np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_DATA_CERTIFICATION_2(smi_params_t  *in_req_params_list,
                                       smi_params_t **out_resp_params_list,
                                       np_ret_t      *out_resp_ret,
                                       binstr_t     **out_sdata);
#endif /* #if defined(NPOPT_ENABLE_DCS) */

#if defined(NPOPT_ENABLE_DUS)
extern np_ret_t
smi_sec_l_Process_DATA_UPDATE_1(smi_params_t   *in_req_params_list,
                                smi_params_t  **out_resp_params_list,
                                np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_DATA_UPDATE_2(smi_params_t     *in_req_params_list,
                                smi_params_t    **out_resp_params_list,
                                np_ret_t         *out_resp_ret,
                                binstr_t        **out_sdata);
#endif /* #if defined(NPOPT_ENABLE_DUS) */

#if defined(NPOPT_USE_NPLOCAL)
extern np_ret_t
smi_sec_l_Process_REG_by_str(smi_id_t         in_msg_type,
                             smi_params_t    *in_req_params_list,
                             smi_params_t   **out_resp_params_list,
                             np_ret_t        *out_resp_ret,
                             binstr_t       **out_sdata);

#if defined(NPOPT_ENABLE_DCS)
extern np_ret_t
smi_sec_l_Process_REG_DCSA(smi_params_t  *in_req_params_list,
                           smi_params_t **out_resp_params_list,
                           np_ret_t      *out_resp_ret,
                           binstr_t     **out_sdata);
#endif

extern np_ret_t
smi_sec_l_Process_SET_TRUSTEDTIME(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret,
                                  binstr_t      **out_sdata);
#endif /* defined(NPOPT_USE_NPLOCAL) */

extern np_ret_t
smi_sec_l_Process_GET_TRUSTEDTIME(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret,
                                  binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_single_call(smi_id_t        in_msg_type,
                              smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

#if defined( NPOPT_USE_PPPROC )
extern np_ret_t
smi_sec_l_Process_PPPROC_INIT(smi_params_t   *in_req_params_list,
                              smi_params_t  **out_resp_params_list,
                              np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_PPPROC_FIN(smi_params_t   *in_req_params_list,
                             smi_params_t  **out_resp_params_list,
                             np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_PPPROC_GENERATEREQUEST(smi_params_t   *in_req_params_list,
                                         smi_params_t  **out_resp_params_list,
                                         np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_PPPROC_PARSERESPONSE(smi_params_t   *in_req_params_list,
                                       smi_params_t  **out_resp_params_list,
                                       np_ret_t       *out_resp_ret,
                                       binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_GOBY_PPPROC_GENERATEREQUEST(smi_params_t  *in_req_params_list,
                                              smi_params_t **out_resp_params_list,
                                              np_ret_t      *out_resp_ret);
extern np_ret_t
smi_sec_l_Process_GOBY_PPPROC_PROCESSRESPONSE(smi_params_t   *in_req_params_list,
                                              smi_params_t  **out_resp_params_list,
                                              np_ret_t       *out_resp_ret,
                                              binstr_t      **out_sdata);

#endif /* defined( NPOPT_USE_PPPROC ) */

#if defined( NPOPT_ENABLE_TESTHOOKS )
extern np_ret_t
smi_sec_l_Process_TH_UPDATESSTORAGE(smi_params_t   *in_req_params_list,
                                    smi_params_t  **out_resp_params_list,
                                    np_ret_t       *out_resp_ret,
                                    binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_TH_LIL_APPEND(smi_params_t   *in_req_params_list,
                                smi_params_t  **out_resp_params_list,
                                np_ret_t       *out_resp_ret,
                                binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_TH_REGSUSPENSIONLIST(smi_params_t   *in_req_params_list,
                                       smi_params_t  **out_resp_params_list,
                                       np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_TH_CHECK_ASSERTION(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_TH_GETROLEASSERTION(smi_params_t   *in_req_params_list,
                                      smi_params_t  **out_resp_params_list,
                                      np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_TH_LILAPPEND(smi_params_t   *in_req_params_list,
                               smi_params_t  **out_resp_params_list,
                               np_ret_t       *out_resp_ret,
                               binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_TH_SETNSCFGVAL(smi_params_t   *in_req_params_list,
                                 smi_params_t  **out_resp_params_list,
                                 np_ret_t       *out_resp_ret);

#ifdef NPOPT_USE_ENC_SDATA
extern np_ret_t
smi_sec_l_Process_TH_GETSECUREDSDATA(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);
#endif /* defined( NPOPT_USE_ENC_SDATA ) */

extern np_ret_t
smi_sec_l_Process_TH_REG_LINK(smi_params_t  *in_req_params_list,
                              smi_params_t **out_resp_params_list,
                              np_ret_t      *out_resp_ret,
                              binstr_t     **out_sdata);

#ifdef NPOPT_ENABLE_IMPORT
extern np_ret_t
smi_sec_l_Process_TH_SET_BKB(smi_params_t   *in_req_params_list,
                             smi_params_t  **out_resp_params_list,
                             np_ret_t       *out_resp_ret,
                             binstr_t      **out_sdata);
#endif
extern np_ret_t
smi_sec_l_Process_TH_GET_PERSONALITY(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);

#endif /* defined( NPOPT_ENABLE_TESTHOOKS ) */

extern np_ret_t
smi_sec_l_Process_LTP_INIT( smi_params_t   *in_req_params_list,
                            smi_params_t  **out_resp_params_list,
                            np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_LTP_FIN(smi_params_t   *in_req_params_list,
                          smi_params_t  **out_resp_params_list,
                          np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_LTP_SETUP_MSGBUILD( smi_params_t   *in_req_params_list,
                                      smi_params_t  **out_resp_params_list,
                                      np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_LTP_SETUP_MSGPROC(smi_params_t   *in_req_params_list,
                                    smi_params_t  **out_resp_params_list,
                                    np_ret_t       *out_resp_ret,
                                    binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_LTP_RUNAGENT_MSGPROC(smi_params_t   *in_req_params_list,
                                       smi_params_t  **out_resp_params_list,
                                       np_ret_t       *out_resp_ret,
                                       binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_LTP_AGENTRESULT_MSGPROC(smi_params_t   *in_req_params_list,
                                          smi_params_t  **out_resp_params_list,
                                          np_ret_t       *out_resp_ret,
                                          binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_LTP_TEARDOWN_MSGPROC(smi_params_t   *in_req_params_list,
                                       smi_params_t  **out_resp_params_list,
                                       np_ret_t       *out_resp_ret,
                                       binstr_t      **out_sdata);

#if defined( NPOPT_ENABLE_EXPORT_ACTION )
extern np_ret_t
smi_sec_l_Process_DRM_ACTION_EXPORT( smi_id_t         in_msg_type,
                                     smi_params_t    *in_req_params_list,
                                     smi_params_t   **out_resp_params_list,
                                     np_ret_t        *out_resp_ret,
                                     binstr_t       **out_sdata );

extern np_ret_t
smi_sec_l_Process_DRM_ACTION_EXPORT_GETKEY( smi_params_t  *in_req_params_list,
                                            smi_params_t **out_resp_params_list,
                                            np_ret_t      *out_resp_ret );
#endif

extern np_ret_t
smi_sec_l_process_DRM_MOVEKEY( smi_params_t  *in_req_params_list,
                               smi_params_t **out_resp_params_list,
                               np_ret_t      *out_resp_ret );

extern np_ret_t
smi_sec_l_Process_GOBY_GET_AVAILABLE_LINKS( smi_params_t  *in_req_params_list,
                                            smi_params_t **out_resp_params_list,
                                            np_ret_t      *out_resp_ret,
                                            binstr_t     **out_sdata );

extern np_ret_t
smi_sec_l_Process_GOBY_GET_LINKS_FOR_SERVICE_ID( smi_params_t  *in_req_params_list,
                                                 smi_params_t **out_resp_params_list,
                                                 np_ret_t      *out_resp_ret );

#if defined (NPOPT_ENABLE_SONY_EXTENSION)
extern np_ret_t
smi_sec_l_Process_GOBY_DELETE_LINK( smi_params_t  *in_req_params_list,
                                    smi_params_t **out_resp_params_list,
                                    np_ret_t      *out_resp_ret,
                                    binstr_t     **out_sdata );
#endif /* if defined (NPOPT_ENABLE_SONY_EXTENSION) */


extern np_ret_t
smi_sec_l_Process_GOBY_GET_LINK_PERIOD(smi_params_t  *in_req_params_list,
                                       smi_params_t **out_resp_params_list,
                                       np_ret_t      *out_resp_ret,
                                       binstr_t     **out_sdata);

extern np_ret_t
smi_sec_l_ProcessCall(smi_id_t         in_msg_type,
                      smi_params_t    *in_req_params_list,
                      binstr_t       **out_response_msg,
                      binstr_t       **out_sdata);

extern np_ret_t
smi_sec_l_Process_GET_DUID_PARAMETERS(smi_params_t   *in_req_params_list,
                                      smi_params_t  **out_resp_params_list,
                                      np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_INIT_MS3_SESSION(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_FIN_MS3_SESSION(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_PARSE_SAS(smi_params_t   *in_req_params_list,
                            smi_params_t  **out_resp_params_list,
                            np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_GET_NEMO_KEYS(smi_params_t   *in_req_params_list,
                                smi_params_t  **out_resp_params_list,
                                np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SAS_INFO_COMPLETE(smi_params_t   *in_req_params_list,
                                    smi_params_t  **out_resp_params_list,
                                    np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_CURIT_EXPAND(smi_params_t   *in_req_params_list,
                               smi_params_t  **out_resp_params_list,
                               np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_BBTS_INIT_SESSION(smi_params_t   *in_req_params_list,
                                    smi_params_t  **out_resp_params_list,
                                    np_ret_t       *out_resp_ret);
extern np_ret_t
smi_sec_l_Process_BBTS_FIN_SESSION(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_BBTS_SET_SOCID( smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret );

extern np_ret_t
smi_sec_l_Process_BBTS_SET_SERVICEBASECID( smi_params_t   *in_req_params_list,
                                           smi_params_t  **out_resp_params_list,
                                           np_ret_t       *out_resp_ret );

extern np_ret_t
smi_sec_l_Process_BBTS_INIT_ECM_STREAM( smi_params_t   *in_req_params_list,
                                        smi_params_t  **out_resp_params_list,
                                        np_ret_t       *out_resp_ret );

extern np_ret_t
smi_sec_l_Process_DRM_GENERATE_HMAC(smi_params_t   *in_req_params_list,
                                    smi_params_t  **out_resp_params_list,
                                    np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_IMPORT_INIT_SESSION(smi_params_t   *in_req_params_list,
                                      smi_params_t  **out_resp_params_list,
                                      np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_IMPORT_FIN_SESSION(smi_params_t   *in_req_params_list,
                                     smi_params_t  **out_resp_params_list,
                                     np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_IMPORT_CHECK_BKB(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret,
                                   binstr_t      **out_sdata);
extern np_ret_t
smi_sec_l_Process_IMPORT_GENERATE_LICENSE(smi_params_t   *in_req_params_list,
                                          smi_params_t  **out_resp_params_list,
                                          np_ret_t       *out_resp_ret,
                                          binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_IMPORT_STORE_BKB(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret,
                                   binstr_t      **out_sdata);

extern np_ret_t
smi_sec_l_Process_IMPORT_GET_LISK_SUBJECT(smi_params_t   *in_req_params_list,
                                          smi_params_t  **out_resp_params_list,
                                          np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SAC_INIT_SESSION(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SAC_FIN_SESSION(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SAC_GENERATE_REQ(smi_params_t   *in_req_params_list,
                                   smi_params_t  **out_resp_params_list,
                                   np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SAC_PROCESS_REQ(smi_params_t   *in_req_params_list,
                                  smi_params_t  **out_resp_params_list,
                                  np_ret_t       *out_resp_ret);

extern np_ret_t
smi_sec_l_Process_SAC_PARSE_RESP(smi_params_t   *in_req_params_list,
                                 smi_params_t  **out_resp_params_list,
                                 np_ret_t       *out_resp_ret);
