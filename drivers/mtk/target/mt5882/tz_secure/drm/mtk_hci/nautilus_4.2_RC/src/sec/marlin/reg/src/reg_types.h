/** @addtogroup reg reg
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef REG_TYPES_H
#define REG_TYPES_H

#define REG_PERSONALITY_VERSION 1

/** One cache slot, 'keys' are manu, model, app_id. Values the caps_obj/caps_str*/
typedef struct caps_cache_tag {
    binstr_t           *manufacturer; /**< DECE specific */
    binstr_t           *model;        /**< NULL: use ns_config.h otherwise DECE specific*/
    binstr_t           *app_id;       /**< DECE specific*/
    octobj_assertion_t *caps_obj;     /**< octobj representation*/
    binstr_t           *caps_str;     /**< str representation ready for SOAP*/
} caps_cache_t;

/** For global structure */
typedef struct reg_account_tag {
#if defined( NPOPT_ENABLE_CAPABILITY_SIGNALLING )
    caps_cache_t        caps_cache[2]; /**< 2 slots: one for Marlin and one for DECE service*/
#endif
    u_int8_t            l_reg_acnt[SHA1_DIGESTSIZE];
    binstr_t            l_reg_acnt_session;
} reg_t;

typedef struct reg_box1 {
    u_int16_t   offset;
    u_int16_t   len[1];
} reg_box1_t;

typedef struct reg_box2 {
    u_int16_t   offset;
    u_int16_t   len[2];
    u_int8_t    reserved[2];
} reg_box2_t;

typedef struct reg_box3 {
    u_int16_t   offset;
    u_int16_t   len[3];
} reg_box3_t;

typedef struct reg_box4 {
    u_int16_t   offset;
    u_int16_t   len[4];
    u_int8_t    reserved[2];
} reg_box4_t;

typedef struct nemo_pub {
    u_int8_t    version;
    u_int8_t    reserved[3];
    reg_box1_t  id;
    reg_box2_t  certs;      /* 0:certs for sig, 1:certs for enc */
} nemo_pub_t;

typedef struct nemo_priv {
    u_int8_t    version;
    u_int8_t    reserved[3];
    reg_box2_t  pkcs8;      /* 0:pkcs#8 for sig, 1:pkcs#8 for enc */
} nemo_priv_t;

typedef struct delegate_tag {
    u_int8_t    version;
    u_int8_t    reserved[3];
    reg_box4_t  header;
} delegate_t;

reg_t *
reg_l_global(void);

retcode_t
reg_l_ParsePKIPath(
    binstr_t    *in_pkipath,
    int32_t     *out_ncerts,
    binstr_t    ***out_certs);

retcode_t
reg_l_ParseNEMOPublicKeys(
    binstr_t    *in_bs,
    binstr_t    **out_id,
    binstr_t    **out_sigcerts,
    binstr_t    **out_enccerts);

retcode_t
reg_l_ComposeNEMOPublicKeys(
    binstr_t    *in_id,
    int32_t     in_nsigcerts,
    binstr_t    **in_sigcerts,
    int32_t     in_nenccerts,
    binstr_t    **in_enccerts,
    binstr_t    **out_bs);

retcode_t
reg_l_ParseCDLC(binstr_t  *in_delegate,
                binstr_t **out_company_name,
                binstr_t **out_entrypoint_name,
                binstr_t **out_delegate_control);

retcode_t
reg_l_ParseLDLC(binstr_t  *in_delegate,
                binstr_t **out_licserv_path,
                binstr_t **out_regserv_path,
                binstr_t **out_entrypoint_name,
                binstr_t **out_delegate_control);

#endif
