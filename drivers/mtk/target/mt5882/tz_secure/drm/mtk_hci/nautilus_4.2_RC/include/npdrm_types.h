/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */

/**
 * @file
 * DRM structures: ESB, Callbacks, track information
 *
 * @see API.reference.pdf
 */
#ifndef NPDRM_TYPES_H
#define NPDRM_TYPES_H

NP_BEGIN_EXTERN_C

/*---------------------------------------------------------------------------*
 * action type definitions
 *---------------------------------------------------------------------------*/

/** Action types */
#define NP_ACTIONTYPE_PLAY              (0)
#define NP_ACTIONTYPE_WRITETOMEDIUM     (1)
#define NP_ACTIONTYPE_TRANSFER          (2)
#define NP_ACTIONTYPE_EXPORT            (3)
#define NP_ACTIONTYPE_DISCARD           (4)
#define NP_ACTIONTYPE_MAX               (5)

/*---------------------------------------------------------------------------*
 * constants for ex-status structures
 *---------------------------------------------------------------------------*/

/* flags */
#define NP_ESB_FLAGS_HASUNKNOWNPART (1<<0)

#define NP_ESB_DESCRIPTION_FLAG_DEFAULT (1<<0)
#define NP_ESB_DESCRIPTION_FLAG_SHORT   (1<<1)
#define NP_ESB_DESCRIPTION_FLAG_LONG    (1<<2)

#define NP_ESB_GENERIC_FLAG_NODEREACHABILITY      (1<<0)
#define NP_ESB_GENERIC_FLAG_REQUIREDLICENSESTATUS (1<<1)

#define NP_ESB_TEMPORAL_FLAG_NOTBEFORE  (1<<0)
#define NP_ESB_TEMPORAL_FLAG_NOTAFTER   (1<<1)
#define NP_ESB_TEMPORAL_FLAG_NOTDURING  (1<<2)
#define NP_ESB_TEMPORAL_FLAG_NOTLONGER  (1<<3)
#define NP_ESB_TEMPORAL_FLAG_NOTMORE    (1<<4)

#define NP_ESB_SPATIAL_FLAG_PROXIMITYREQUIRED (1<<0)

#define NP_ESB_GROUP_FLAG_MEMBERSHIP (1<<0)
#define NP_ESB_GROUP_FLAG_IDENTITY   (1<<1)

#define NP_ESB_DEVICE_FLAG_TYPE              (1<<0)
#define NP_ESB_DEVICE_FLAG_FEATURE           (1<<1)
#define NP_ESB_DEVICE_FLAG_ID                (1<<2)
#define NP_ESB_DEVICE_FLAG_LICENSESUSPENSION (1<<3)
#define NP_ESB_DEVICE_FLAG_METERPLAY         (1<<4)
#define NP_ESB_DEVICE_FLAG_TRUSTEDTIME       (1<<5)
#define NP_ESB_DEVICE_FLAG_VERSIONMAJOR      (1<<6)
#define NP_ESB_DEVICE_FLAG_VERSIONMINOR      (1<<7)
#define NP_ESB_DEVICE_FLAG_DCSFRESHNESS      (1<<8)

/* Callback parameter types */
#define NP_ESB_CALLBACK_TYPE_ONACCEPT            (1)
#define NP_ESB_CALLBACK_TYPE_ONTIME              (2)
#define NP_ESB_CALLBACK_TYPE_ONTIMEELAPSED       (3)
#define NP_ESB_CALLBACK_TYPE_ONEVENT             (4)
#define NP_ESB_CALLBACK_TYPE_ONAGENTCOMPLETION   (5)

/* Types of callback events */
#define  NP_ESB_EVENT_TYPE_UNKNOWN               (0)
#define  NP_ESB_EVENT_TYPE_ONPLAY                (1)
#define  NP_ESB_EVENT_TYPE_ONSTOP                (2)
#define  NP_ESB_EVENT_TYPE_ONTIMECODE            (3)
#define  NP_ESB_EVENT_TYPE_ONSEEK                (4)

/* Callback id */
#define NP_ESB_CALLBACK_ID_RESET                 (0)
#define NP_ESB_CALLBACK_ID_CONTINUE              (1)

/* Obligation parameter types */
#define NP_ESB_OBLIGATION_TYPE_UNKNOWN           (0)
#define NP_ESB_OBLIGATION_TYPE_METERPLAY         (1)
#define NP_ESB_OBLIGATION_TYPE_RUNAGENTONPEER    (2)
#define NP_ESB_OBLIGATION_TYPE_OUTPUTCONTROL     (3)

/* Permissions parameter types */
#define NP_ESB_PERMISSION_TYPE_UNKNOWN               (0)
#define NP_ESB_PERMISSION_TYPE_OUTPUTCONTROLOVERRIDE (1)

/* Output control technologies for obligations and permissions */
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_TYPE_UNKNOWN         (0)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_TYPE_BASIC           (1)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_TYPE_DTCP            (2)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_TYPE_BASIC_EXT       (3)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_TYPE_OUTPUTENABLER   (4)

#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_UNKNOWN (1<<0)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_EPN     (1<<1)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_CCI     (1<<2)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_ICT     (1<<3)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_DOT     (1<<4)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_APS     (1<<5)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_RMM     (1<<6)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_RS      (1<<7)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_AVE     (1<<8)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCPVE  (1<<9)
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_FLAG_DVE     (1<<10)

/* Output control parameter values for license ESB creation. Refer [MOC] and [DTCP] */
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EPN_ASSERTED         0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EPN_UNASSERTED       0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_CCI_UNASSERTED       0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_CCI_NO_MORE_COPIES   0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_CCI_COPY_ONE_GEN     0x2
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_ICT_CONSTRAINED      0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_ICT_HIGH_DEF         0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_DOT_ANALOG_DIGITAL   0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_DOT_DIGITAL_ONLY     0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_COPY_FREE        0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_TYPE1            0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_TYPE2            0x2
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_TYPE3            0x3
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_RMM_ASSERTED          0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_RMM_UNASSERTED        0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_ICT_HIGH_DEF          0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_APS_COPY_FREE         0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_RS_90                 0x7
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_EPN_ASSERTED          0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_EPN_UNASSERTED        0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_CCI_COPY_FREE         0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_CCI_NO_MORE_COPIES    0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_ICT_HIGH_DEF          0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DTCP_APS_COPY_FREE         0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_OE_ANALOGUE_VIDEO_ENABLER  0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_OE_DTCP_VIDEO_ENABLER      0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_UNASSERTED   0x0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_RESERVED     0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_COPY_ONCE    0x2
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_NEVER_COPY   0x3

#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_NAME(ext, idx)     \
    ((ext)->e.basic_extension.extensions[(idx)].name)

#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_GET_SCMS_VALUE(ext, idx)     \
    ((ext)->e.basic_extension.extensions[(idx)].value & 0x03)

/*
 * the values below are defined
 *   in the spec 'Marlin - Output Control Specification'
 * If the spec is changed, the value must be fixed
 */
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_EPN_ASSERTED   0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_EPN_UNASSERTED 1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_CCI_NOTASSERTED   0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_CCI_NOMORECOPY    1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_CCI_ONEGENERATION 2
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_CCI_NEVERCOPY     3
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_ICT_CONSTRAINED    0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_ICT_NOTCONSTRAINED 1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DOT_ANALOGOK    0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_DOT_DIGITALONLY 1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_OFF    0
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_TYPE1  1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_AGC    1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_TYPE2  2
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_AGC_2L 2
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_TYPE3  3
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_APS_AGC_4L 3
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_OE_ANALOGUE_VIDEO_ENABLER  0x1
#define NP_ESB_OUTPUTCONTROLTECHNOLOGY_OE_DTCP_VIDEO_ENABLER      0x1

/* Ex-Parameter flags */
#define NP_ESB_PARAMETER_FLAG_CRITICAL           (1<<0)
#define NP_ESB_PARAMETER_FLAG_HUMANREADABLE      (1<<1)

/* ESB Value types */
#define NP_ESB_VALUE_TYPE_INTEGER                (0)
#define NP_ESB_VALUE_TYPE_REAL                   (1)
#define NP_ESB_VALUE_TYPE_STRING                 (2)
#define NP_ESB_VALUE_TYPE_DATE                   (3)
#define NP_ESB_VALUE_TYPE_PARAMETER              (4)
#define NP_ESB_VALUE_TYPE_EXPARAMETER            (5)
#define NP_ESB_VALUE_TYPE_RESOURCE               (6)
#define NP_ESB_VALUE_TYPE_VALUELIST              (7)

/* counter contraint parameters */
#define NP_ESB_COUNTER_FLAG_REPEATCOUNT          (1<<0)

/* export info parameter */
#define NP_ESB_EXPORT_FLAG_EXINFO_PRESENT        (1<<0)

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

struct np_esb_value_tag;
typedef struct {
    np_str_t                *name;
    struct np_esb_value_tag *value;
} np_esb_parameter_t;

typedef struct {
    np_esb_parameter_t param;
    u_int32_t          flags;  /*  NP_ESB_PARAMETER_FLAG_*  */
} np_esb_exparameter_t;

typedef struct {
    int32_t count;
    struct np_esb_value_tag *values[1];
} np_esb_valuelist_t;

typedef struct np_esb_value_tag {
    int32_t  type;   /*  NP_ESB_VALUE_TYPE_*  */
    union {
        int32_t               i;
        float                 f;
        np_str_t             *str;
        u_int32_t             date;
        np_esb_parameter_t   *param;
        np_esb_exparameter_t *exparam;
        np_str_t             *resource;
        np_esb_valuelist_t   *vallist;
        np_str_t             *bytearray;
    } data;
} np_esb_value_t;

typedef struct {
    u_int32_t     type;
    union {
        struct {
            u_int32_t  event_type;
            union {
                struct {
                    int32_t  seek_position_offset;
                    int32_t  seek_position_range;
                } os;
            } t;
        } oe;
        struct {
            int32_t  status_code;
        } oac;
    } e;
} np_callback_args_t;

typedef struct {
    int32_t   id;
    np_str_t  *entrypoint;
    int32_t   cookie;
} np_callback_t;

typedef struct {
    u_int32_t     type;
    u_int32_t     flags;
    np_callback_t callback;
    union {
        struct {
            int32_t dummy;
        } oa;
        struct {
            np_date_t *date;
        } ot;
        struct {
            u_int32_t duration;
        } ote;
        struct {
            u_int32_t event_type;
            int32_t  event_flags;
            int32_t  event_param;
        } oe;
        struct {
            int32_t  instance_id;
        } oac;
    } e;
} np_callback_param_t;

typedef struct np_esb_oc_basic_extension_tag {
    np_str_t    *name;
    u_int32_t    value;
} np_esb_oc_basic_extension_t;

typedef struct {
    u_int32_t   type;
    u_int32_t   flags;
    struct {
        struct {
            u_int32_t epn;
            u_int32_t cci;
            u_int32_t ict;
            u_int32_t dot;
            u_int32_t aps;
        } basic;
        struct {
            u_int32_t rmm;
            u_int32_t rs;
            u_int32_t epn;
            u_int32_t cci;
            u_int32_t ict;
            u_int32_t aps;
        } dtcp;
        struct {
            u_int32_t ave;    /* Analog Video Enabler */
            u_int32_t dtcpve; /* DTCP Video Enabler */
            u_int32_t dve;    /* Digital Video Enabler */
        } output_enabler;
        struct {
            u_int32_t                     num;
            np_esb_oc_basic_extension_t  *extensions;   /* array of name:value extensions */
        } basic_extension;
    } e;
} np_esb_outputcontrol_t;

typedef struct {
    u_int32_t    type;
    u_int32_t    flags;
    union {
        struct {
            np_str_t *service_id;
            np_str_t *logical_id;
        } mp;
        struct {
            np_str_t           *control_id;
            np_str_t           *agent_name;
            int32_t            instance_id; 
            np_str_t           *context_id;
            np_esb_valuelist_t *parameters;
        } raop;
        struct {
            np_str_t               *control_id;
            np_esb_outputcontrol_t *parameters;
        } oc;
    } e;
} np_obligation_param_t;

typedef struct {
    u_int32_t    type;
    u_int32_t    flags;
    union {
        struct {
            np_str_t               *control_id;
            np_esb_outputcontrol_t *parameters;
        } oco;
    } e;
} np_permission_param_t;

typedef struct {
	u_int32_t flags;
    u_int32_t             obligation_params_count;
    np_obligation_param_t *obligation_params;
    u_int32_t           callback_params_count;
    np_callback_param_t *callback_params;
    u_int32_t             permission_params_count;
    np_permission_param_t *permission_params;
    struct {
    	u_int32_t flags;
        np_str_t *mdefault;
        np_str_t *mshort;
        np_str_t *mlong;
    } desc_c;
    struct {
    	u_int32_t flags;
    	u_int32_t nodereachability_count;
        np_str_t **nodereachability_array;
        int32_t required_license_status;
    } generic_c;
    struct {
    	u_int32_t flags;
    	u_int32_t notbefore_count;
    	u_int32_t notafter_count;
        np_date_t *notbefore[2];
        np_date_t *notafter[2];
        np_date_t *notduring[2];
        int32_t notlongerthan;
        int32_t notmorethan;
    } temporal_c;
    struct {
    	u_int32_t flags;
        u_int32_t proximityrequired;
    } spatial_c;
    struct {
    	u_int32_t flags;
        np_str_t *membership;
        np_str_t *identity;
    } group_c;
    struct {
    	u_int32_t flags;
        np_str_t *type;
        np_str_t *feature;
        np_str_t *id;
        int32_t version_major;
        int32_t version_minor;
        int32_t dcs_freshness;
    } device_c;
    struct { 
        u_int32_t flags;
        u_int32_t repeat_count_max;
        u_int32_t repeat_count_remain;
    } counter_c;
    struct {
        u_int32_t  flags;
        union {
            u_int32_t i;
        } result;
    } export_info;
} np_esb_t;

typedef struct np_transfer {
    np_str_t    *sink_node_id;
    np_str_t    *transfer_mode;
    u_int32_t    transfer_count;
    u_int32_t    sink_proximity_lastprobe;
} np_transfer_t;

typedef struct np_export {
    np_str_t    *target_system;
    int32_t     export_mode;
} np_export_t;

typedef struct {
    union {
        np_transfer_t transfer_params;
        np_export_t   export_params;
    } action;
} np_action_params_t;

NP_END_EXTERN_C

#endif /* NPDRM_TYPES_H */
