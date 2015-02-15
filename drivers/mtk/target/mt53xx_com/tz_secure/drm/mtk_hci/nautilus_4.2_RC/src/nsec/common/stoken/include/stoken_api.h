/** @addtogroup stoken stoken
 *  @ingroup nsec
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

/**
 * @file
 *
 * Library for parsing configuration and action tokens
 *
 */

#ifndef STOKEN_API_H
#define STOKEN_API_H

/* vector type */
#define STOKEN_VEC_TYPE_BINSTR      (1)
#define STOKEN_VEC_TYPE_BSC_DI      (2)
#define STOKEN_VEC_TYPE_BSC_POLICY  (3)
#define STOKEN_VEC_TYPE_BSC_CRTSTD  (4)
#define STOKEN_VEC_TYPE_BSC_BASIC   (5)
#define STOKEN_VEC_TYPE_BSC_DUS     (6)
#define STOKEN_VEC_TYPE_BSC_DCS     (7)
#define STOKEN_VEC_TYPE_BSA_CRTSTD  (8)

/* action type */
#define STOKEN_ACTION_TYPE_LICENSEACQ  (1)
#define STOKEN_ACTION_TYPE_NODEACQ     (2)
#define STOKEN_ACTION_TYPE_LINKACQ     (3)
#define STOKEN_ACTION_TYPE_DEREG       (4)
#define STOKEN_ACTION_TYPE_LTP         (5)
#define STOKEN_ACTION_TYPE_MS3         (6)

/* certification standard */
#define STOKEN_CRTSTD_USE_MUST      (1)
#define STOKEN_CRTSTD_USE_SHOULD    (2)

/* macro for ConfigToken */
#define stoken_ConfigRegNum(x)     ( (x) ? ((x)->reg.n) : 0 )
#define stoken_ConfigRegItem(x,i)  ( (x) && ((x)->reg.c) ? ((stoken_ct_reg_t*)((x)->reg.c[i])) : NULL )
#define stoken_ConfigLsNum(x)      ( (x) ? ((x)->ls.n) : 0 )
#define stoken_ConfigLsItem(x,i)   ( (x) && ((x)->ls.c) ? ((stoken_ct_ls_t*)((x)->ls.c[i])) : NULL )
#define stoken_ConfigDUSNum(x)     ( (x) ? ((x)->dus.n) : 0 )
#define stoken_ConfigDUSItem(x,i)  ( (x) && ((x)->dus.c) ? ((stoken_ct_dus_t*)((x)->dus.c[i])) : NULL )
#define stoken_ConfigDCSNum(x)     ( (x) ? ((x)->dcs.n) : 0 )
#define stoken_ConfigDCSItem(x,i)  ( (x) && ((x)->dcs.c) ?  ((stoken_ct_dcs_t*)((x)->dcs.c[i])) : NULL )
#define stoken_ConfigMNPNum(x)     ( (x) ? ((x)->mnp.n) : 0 )
#define stoken_ConfigMNPItem(x,i)  ( (x) && ((x)->mnp.c) ?  ((stoken_ct_mnp_t*)((x)->mnp.c[i])) : NULL )
#define stoken_ConfigMOPNum(x)     ( (x) ? ((x)->mop.n) : 0 )
#define stoken_ConfigMOPItem(x,i)  ( (x) && ((x)->mop.c) ?  ((stoken_ct_mop_t*)((x)->mop.c[i])) : NULL )
#define stoken_ConfigLtpNum(x)     ( (x) ? ((x)->ltp.n) : 0 )
#define stoken_ConfigLtpItem(x,i)  ( (x) && ((x)->ltp.c) ? ((stoken_ct_ltp_t*)((x)->ltp.c[i])) : NULL )
#define stoken_ConfigWsdlURL(itm)  ( (itm) ? ((itm)->wsdl.url) : NULL )
#define stoken_ConfigNodeInfo(itm) ( (itm) ? ((itm)->ninfo) : NULL )
#define stoken_ConfigNodeID(itm)   ( (itm) ? ((itm)->ninfo.nodeid) : NULL )
#define stoken_ConfigEncKey(itm)   ( (itm) ? ((itm)->ninfo.enckey) : NULL )
#define stoken_ConfigSignKey(itm)  ( (itm) ? ((itm)->ninfo.signkey) : NULL )
#define stoken_Assertion(itm,i)    ( (itm) && ((itm)->ninfo.assertions.c) ? ((itm)->ninfo.assertions).c[i] : NULL )
/* ERROR CODE */
#define ERR_NP_STOKEN_ILLEGALST    (ERR_NP_STOKEN | 0x0001)

/* Common */
typedef struct stoken_vec {
    u_int16_t   type;
    u_int16_t   n;
    void        **c;
} stoken_vec_t;


typedef struct stoken_ninfo {
    binstr_t     *nodeid;
    binstr_t     *enckey;
    binstr_t     *signkey;
    stoken_vec_t assertions;
} stoken_ninfo_t;

typedef struct stoken_wsdl {
    char *url;
} stoken_wsdl_t;

typedef struct stoken_config_token {
    binstr_t    *bsid;
    binstr_t    *bsfn;
    binstr_t    *ver;
    stoken_vec_t    reg;
    stoken_vec_t    ls;
    stoken_vec_t    dus;
    stoken_vec_t    dcs;
    stoken_vec_t    mnp;
    stoken_vec_t    mop;
    stoken_vec_t    ltp;
    void        *svc;
} stoken_config_token_t;

typedef struct stoken_ct_basic {
    binstr_t    *id;
    stoken_ninfo_t ninfo;
    stoken_wsdl_t   wsdl;
    /* stoken_vec_t    policies; */
    stoken_vec_t    extension;
} stoken_ct_basic_t;

typedef stoken_ct_basic_t stoken_ct_reg_t;
typedef stoken_ct_basic_t stoken_ct_ls_t;
typedef stoken_ct_basic_t stoken_ct_dus_t;
typedef stoken_ct_basic_t stoken_ct_dcs_t;
typedef stoken_ct_basic_t stoken_ct_mnp_t;
typedef stoken_ct_basic_t stoken_ct_mop_t;
typedef stoken_ct_basic_t stoken_ct_ltp_t;

typedef struct stoken_ct_policy {
    binstr_t    *op_name;
    binstr_t    *uri;
} stoken_ct_policy_t;

typedef struct stoken_ct_di {
    binstr_t        *name;
    binstr_t        *name_space;
} stoken_ct_di_t;

typedef struct stoken_ct_crt_std {
    binstr_t    *name;
    stoken_vec_t    dis;
} stoken_ct_crt_std_t;

/* ActionToken Parse */
typedef struct stoken_at_config_info {
    stoken_vec_t    rl;
    binstr_t    *id;
    binstr_t    *ver;
} stoken_at_config_info_t;

typedef struct stoken_at_actions {
    int32_t     type;
    binstr_t    *id;
    binstr_t    *bztkn;
    binstr_t    *from;
    binstr_t    *to;
    binstr_t    *ms3_sas_location;
    /* stoken_vec_t    css; */
} stoken_at_actions_t;

typedef struct stoken_action_token {
    stoken_at_config_info_t   cinfo;
    int32_t                num;
    stoken_at_actions_t       **actions;
} stoken_action_token_t;

typedef struct stoken_at_crt_std {
    binstr_t    *name;
    int32_t     use;
    binstr_t    *validity;
} stoken_at_crt_std_t;

/* API */

/**
 * @brief Parse XML Configuration Token and create handle
 *
 * @param[in] in_config_len  length of xml configration token
 * @param[in] in_config_buf  buffer of xml configration token
 * @param[out] out_cnf configuration token handle
 *
 * @return  ::ERR_NP_OK
 *  @return ::ERR_NP_OUTOFMEMORY
 *  @return ::ERR_NP_ILLEGALARG
 *  @return ::ERR_NP_STOKEN_ILLEGALST
 *  @return ::ERR_NP_INTERNALERROR
 */
np_ret_t
stoken_ParseConfigToken(
    u_int32_t in_config_len,
    u_int8_t *in_config_buf,
    stoken_config_token_t  **out_cnf);

/**
 * @brief Free Configuration Token handle
 *
 * @param[in,out] io_cnf configuration token handle
 *
 * @return  ::ERR_NP_OK
 */
np_ret_t
stoken_FreeConfigToken(
    stoken_config_token_t *io_cnf);

/**
 * @brief Parse XML Action Token and create handle
 *
 * @param[in] in_atkn_len  length of xml action token
 * @param[in] in_atkn_buf  buffer of xml action token
 * @param[out] out_act action token handle
 *
 * @return  ::ERR_NP_OK
 *  @return ::ERR_NP_OUTOFMEMORY
 *  @return ::ERR_NP_ILLEGALARG
 *  @return ::ERR_NP_STOKEN_ILLEGALST
 *  @return ::ERR_NP_INTERNALERROR
 */
np_ret_t
stoken_ParseActionToken(
    u_int32_t in_atkn_len,
    u_int8_t *in_atkn_buf,
    stoken_action_token_t **out_act);

/**
 * @brief Free Action Token handle
 *
 * @param[in,out] io_act action token  handle
 *
 * @return  ::ERR_NP_OK
 */
np_ret_t
stoken_FreeActionToken(
    stoken_action_token_t *io_act);

np_ret_t
stoken_GenerateConfigToken(
    u_int32_t    in_nenccert,
    binstr_t   **in_enccert,
    u_int32_t    in_nsigcert,
    binstr_t   **in_sigcert,
    binstr_t    *in_roleast,
    binstr_t    *in_nodeid,
    char       **out_ctkn );

np_ret_t
stoken_GenerateActionToken(
    char **out_atkn);

#endif /* STOKEN_API_H */
/** @} */
