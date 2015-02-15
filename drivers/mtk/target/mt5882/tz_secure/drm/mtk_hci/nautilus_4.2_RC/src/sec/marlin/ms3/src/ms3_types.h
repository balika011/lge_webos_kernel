/** @addtogroup ms3 ms3
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */
#ifndef MS3_TYPES_H
#define MS3_TYPES_H

NP_BEGIN_EXTERN_C

/** @file : ms3_types.h */

/**
 * MS3 context structure
 *
 */

typedef struct {
    binstr_t             *surl;                   /* s-url is stored in the handle*/
    binstr_t             *authenticator;          /* Authenticator required for expanding uri templates */
    u_int32_t             key_count;              /* count of content_keys*/
    binstr_t            **content_ids;            /* array of content-ids*/
    bool_t                ck_enable_flag;         /* flag indicates keys can be used*/
    keymng_handle_t       keymanager;             /* All content keys are managed by this entity */
} ms3_l_context_t;

/* Macros for output control values */

/* Value :3 (0x11) signifies that MS3 has both basic and DTCP structures and are valid */
#define L_DEFAULT_OC_FLAGS       3

/* Value :31 (0x1F)signifies that all Basic (EPN, CCI, ICT, DOT, APS) are valid and application can use it */
#define L_DEFAULT_OC_BASIC_FLAGS 31
#define L_DEFAULT_OC_BASIC_EPN   1
#define L_DEFAULT_OC_BASIC_CCI   3
#define L_DEFAULT_OC_BASIC_ICT   1
#define L_DEFAULT_OC_BASIC_DOT   0
#define L_DEFAULT_OC_BASIC_APS   1

/* Value :63 (0x3F)signifies that all DTCP(RMM, RS, EPN, CCI, ICT, APS) are valid and application can use it */
#define L_DEFAULT_OC_DTCP_FLAGS  63
#define L_DEFAULT_OC_DTCP_RMM    1
#define L_DEFAULT_OC_DTCP_RS     7
#define L_DEFAULT_OC_DTCP_EPN    1
#define L_DEFAULT_OC_DTCP_CCI    3
#define L_DEFAULT_OC_DTCP_ICT    1
#define L_DEFAULT_OC_DTCP_APS    1

#define L_READ_OUTPUTCONTROL_BASIC_DOT_VALUE(in_ocvalue) (in_ocvalue  & 0x00000001)
#define L_READ_OUTPUTCONTROL_BASIC_EPN_VALUE(in_ocvalue) ((in_ocvalue & 0x00000020) >> 5)
#define L_READ_OUTPUTCONTROL_BASIC_CCI_VALUE(in_ocvalue) ((in_ocvalue & 0x000000C0) >> 6)
#define L_READ_OUTPUTCONTROL_BASIC_ICT_VALUE(in_ocvalue) ((in_ocvalue & 0x00000100) >> 8)
#define L_READ_OUTPUTCONTROL_BASIC_APS_VALUE(in_ocvalue) ((in_ocvalue & 0x00000600) >> 9)
#define L_READ_OUTPUTCONTROL_DTCP_RMM_VALUE(in_ocvalue)  ((in_ocvalue & 0x00000800) >> 11)
#define L_READ_OUTPUTCONTROL_DTCP_RS_VALUE(in_ocvalue)   ((in_ocvalue & 0x00007000) >> 12)
#define L_READ_OUTPUTCONTROL_DTCP_EPN_VALUE(in_ocvalue)  ((in_ocvalue & 0x00008000) >> 15)
#define L_READ_OUTPUTCONTROL_DTCP_CCI_VALUE(in_ocvalue)  ((in_ocvalue & 0x00030000) >> 16)
#define L_READ_OUTPUTCONTROL_DTCP_ICT_VALUE(in_ocvalue)  ((in_ocvalue & 0x00040000) >> 18)
#define L_READ_OUTPUTCONTROL_DTCP_APS_VALUE(in_ocvalue)  ((in_ocvalue & 0x00180000) >> 19)

/* SAS paratemeters related macros */
#define L_MS3_CONTROL_FLAG_SIZE  1
#define L_RETENTION_BIT          1

#define L_MS3_EXT_INFO_SIZE      9
#define L_EXTENSION_HEADER_SIZE  9
#define L_MS3_CRITICAL_FLAG_SIZE 1

#define L_MS3_NAMESPACE  "s"
#define L_MS3_VARNAME    "authenticator"

NP_END_EXTERN_C

#endif /* MS3_TYPES_H */
/** @} */
