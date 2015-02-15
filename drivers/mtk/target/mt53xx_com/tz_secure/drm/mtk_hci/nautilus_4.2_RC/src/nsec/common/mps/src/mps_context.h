/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2012 Sony Corporation.
 */

#ifndef MPS_L_CONTEXT_H
#define MPS_L_CONTEXT_H

/** @addtogroup mps mps
 * @ingroup goby
 * @{
 *
 * @file
 * @brief Internal MPS context
 */

#define L_DCSN_DEFAULT "urn:marlin:organization:sony:connect:client-certification"

#if defined(NPOPT_USE_ARK)
#  define NS_HARDCODED_CRL_DP_VAL "http://crl.marlin-tmo.com/crl/mtmocrls.crl"
#else
#  define NS_HARDCODED_CRL_DP_VAL "http://testcrl.marlin-tmo.com/testcrl/mtmocrls.crl"
#endif

typedef enum {
    MPS_STEP_IDLE,
    MPS_BB_STEP_IDLE,

    MPS_BB_CTKN_ACQ_STEP_1,
    MPS_BB_CTKN_ACQ_STEP_2,

    MPS_BB_CRL_STEP_1,
    MPS_BB_CRL_STEP_2,

    MPS_BB_DUS_STEP_1,
    MPS_BB_DUS_STEP_2,

    MPS_BB_DCS_STEP_1,
    MPS_BB_DCS_STEP_2,

    MPS_BB_NODE_REG_STEP_1,
    MPS_BB_NODE_REG_STEP_2,

    MPS_BB_LINK_REG_STEP_1,
    MPS_BB_LINK_REG_STEP_2,
    MPS_BB_LINK_REG_STEP_3,

    MPS_BB_DEREG_STEP_1,
    MPS_BB_DEREG_STEP_2,
    MPS_BB_DEREG_STEP_3,

    MPS_BB_LIC_ACQ_STEP_1,
    MPS_BB_LIC_ACQ_STEP_2,

    MPS_LTP_SETUP_SOURCE,
    MPS_LTP_SETUP_SINK,
    MPS_LTP_RUN_AGENT,
    MPS_LTP_AGENT_RESULT,
    MPS_LTP_TEARDOWN


} mps_l_step;


typedef struct mps_l_context {
    u_int32_t                type;
    mps_l_step               step;
    np_target_info_t         local_tl;
    np_transporthandle_t     local_transport;
    np_handle_t              transport;
    binstr_t                *srv_id;
    binstr_t                *srv_info;
    stoken_action_token_t   *atkn;
    stoken_config_token_t   *ctkn;
    u_int32_t                atkn_idx;
    u_int32_t                ctkn_idx;
    u_int32_t                ctkn_max;
    char                   **ctkns;
    np_certstandard_list_t  *certstdlst;
    int32_t                  progstep;
    int32_t                  progtotal;
#if defined(NPOPT_ENABLE_DECE)
    binstr_t                *manufacturer; /* manufacturer for Device Attestation*/
    binstr_t                *model;        /* model for Device Attestation*/
    binstr_t                *app_id;       /* app_id for Device Attestation. Can be NULL.*/
#endif
} mps_l_context_t;

#endif /* MPS_L_CONTEXT_H */
/** @} */
