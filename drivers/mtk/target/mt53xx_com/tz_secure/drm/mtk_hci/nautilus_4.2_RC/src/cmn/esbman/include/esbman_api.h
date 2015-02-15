/** @addtogroup esbman esbman
 *  @ingroup cmn
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef ESBMAN_API_H
#define ESBMAN_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief ESB(Extended Status Block) Manager
 *
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 * @date   17 Jun, 2005 last modified by n-goto
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * error definitions
 *---------------------------------------------------------------------------*/

#define ERR_ESBMAN_ILLEGALDATA   (ERR_ESBMAN_BASE|1)
#define ERR_ESBMAN_ILLEGALESB    (ERR_ESBMAN_BASE|2)

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

#define ESBMAN_FLAGS_HASUNKNOWNPART           (1<<0)

/* Obligation parameter types */
#define ESBMAN_OBLIGATION_TYPE_UNKNOWN        (0)
#define ESBMAN_OBLIGATION_TYPE_METERPLAY      (1)
#define ESBMAN_OBLIGATION_TYPE_RUNAGENTONPEER (2)
#define ESBMAN_OBLIGATION_TYPE_OUTPUTCONTROL  (3)
#define ESBMAN_OBLIGATION_TYPE_CUSTOM         (4)

#define ESBMAN_DESCRIPTION_FLAG_DEFAULT       (1<<0)
#define ESBMAN_DESCRIPTION_FLAG_SHORT         (1<<1)
#define ESBMAN_DESCRIPTION_FLAG_LONG          (1<<2)

#define ESBMAN_GENERIC_FLAG_NODEREACHABILITY      (1<<0)
#define ESBMAN_GENERIC_FLAG_REQUIREDLICENSESTATUS (1<<1)

#define ESBMAN_TEMPORAL_FLAG_NOTBEFORE        (1<<0)
#define ESBMAN_TEMPORAL_FLAG_NOTAFTER         (1<<1)
#define ESBMAN_TEMPORAL_FLAG_NOTDURING        (1<<2)
#define ESBMAN_TEMPORAL_FLAG_NOTLONGER        (1<<3)
#define ESBMAN_TEMPORAL_FLAG_NOTMORE          (1<<4)

#define ESBMAN_SPATIAL_FLAG_PROXIMITYREQUIRED (1<<0)

#define ESBMAN_GROUP_FLAG_MEMBERSHIP          (1<<0)
#define ESBMAN_GROUP_FLAG_IDENTITY            (1<<1)

#define ESBMAN_DEVICE_FLAG_TYPE               (1<<0)
#define ESBMAN_DEVICE_FLAG_FEATURE            (1<<1)
#define ESBMAN_DEVICE_FLAG_ID                 (1<<2)
#define ESBMAN_DEVICE_FLAG_LICENSESUSPENSION  (1<<3) /* marlin extension */
#define ESBMAN_DEVICE_FLAG_METERPLAY          (1<<4) /* marlin extension */
#define ESBMAN_DEVICE_FLAG_TRUSTEDTIME        (1<<5) /* marlin extension */
#define ESBMAN_DEVICE_FLAG_VERSIONMAJOR       (1<<6) /* marlin extension */
#define ESBMAN_DEVICE_FLAG_VERSIONMINOR       (1<<7) /* marlin extension */
#define ESBMAN_DEVICE_FLAG_DCSFRESHNESS       (1<<8) /* marlin extension */

/* Callback parameter types */
#define ESBMAN_CALLBACK_TYPE_UNKNOWN             (0)
#define ESBMAN_CALLBACK_TYPE_ONACCEPT            (1)
#define ESBMAN_CALLBACK_TYPE_ONTIME              (2)
#define ESBMAN_CALLBACK_TYPE_ONTIMEELAPSED       (3)
#define ESBMAN_CALLBACK_TYPE_ONEVENT             (4)
#define ESBMAN_CALLBACK_TYPE_ONAGENTCOMPLETION   (5)

/* Types of callback events */
#define  ESBMAN_EVENT_TYPE_UNKNOWN               (0)
#define  ESBMAN_EVENT_TYPE_ONPLAY                (1)
#define  ESBMAN_EVENT_TYPE_ONSTOP                (2)
#define  ESBMAN_EVENT_TYPE_ONTIMECODE            (3)
#define  ESBMAN_EVENT_TYPE_ONSEEK                (4)

/* Callback id */
#define ESBMAN_CALLBACK_ID_RESET                 (0)
#define ESBMAN_CALLBACK_ID_CONTINUE              (1)

/* Ex-Parameter flags */
#define ESBMAN_PARAMETER_FLAG_CRITICAL           (1<<0)
#define ESBMAN_PARAMETER_FLAG_HUMANREADABLE      (1<<1)

/* ESB Value types */
#define ESBMAN_VALUE_TYPE_INTEGER                (0)
#define ESBMAN_VALUE_TYPE_REAL                   (1)
#define ESBMAN_VALUE_TYPE_STRING                 (2)
#define ESBMAN_VALUE_TYPE_DATE                   (3)
#define ESBMAN_VALUE_TYPE_PARAMETER              (4)
#define ESBMAN_VALUE_TYPE_EXPARAMETER            (5)
#define ESBMAN_VALUE_TYPE_RESOURCE               (6)
#define ESBMAN_VALUE_TYPE_VALUELIST              (7)
#define ESBMAN_VALUE_TYPE_BYTEARRAY              (8)

/* Permissions parameter types */
#define ESBMAN_PERMISSION_TYPE_UNKNOWN               (0)
#define ESBMAN_PERMISSION_TYPE_OUTPUTCONTROLOVERRIDE (1)

/* Output control technologies for obligations and permissions */
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_TYPE_UNKNOWN             (0)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_TYPE_BASIC               (1)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_TYPE_DTCP                (2)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_TYPE_BASIC_EXT           (3)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_TYPE_OUTPUTENABLER       (4)

#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_UNKNOWN (1<<0)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_EPN     (1<<1)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_CCI     (1<<2)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_ICT     (1<<3)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_DOT     (1<<4)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_APS     (1<<5)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_RMM     (1<<6)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_RS      (1<<7)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_AVE     (1<<8)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_DTCPVE  (1<<9)
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_FLAG_DVE     (1<<10)


/* Output control parameter values for license ESB creation. Refer [MOC] and [DTCP] */
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EPN_ASSERTED         0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EPN_UNASSERTED       0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_CCI_UNASSERTED       0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_CCI_NO_MORE_COPIES   0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_CCI_COPY_ONE_GEN     0x2
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_ICT_CONSTRAINED      0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_ICT_HIGH_DEF         0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_DOT_ANALOG_DIGITAL   0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_DOT_DIGITAL_ONLY     0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_COPY_FREE        0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_TYPE1            0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_TYPE2            0x2
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_APS_TYPE3            0x3
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_RMM_ASSERTED          0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_RMM_UNASSERTED        0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_ICT_HIGH_DEF          0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_APS_COPY_FREE         0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_RS_90                 0x7
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_EPN_ASSERTED          0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_EPN_UNASSERTED        0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_CCI_COPY_FREE         0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_DTCP_CCI_NO_MORE_COPIES    0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_OE_ANALOGUE_VIDEO_ENABLER  0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_OE_DTCP_VIDEO_ENABLER      0x1

#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_UNASSERTED  0x0
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_RESERVED    0x1
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_COPY_ONCE   0x2
#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_SCMS_NEVER_COPY  0x3

#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_BASIC_EXT_NAME(ext, idx)     \
    ((ext)->e.basic_extension.extensions[(idx)].name)

#define ESBMAN_OUTPUTCONTROLTECHNOLOGY_GET_SCMS_VALUE(ext, idx)    \
    (((ext)->e.basic_extension.extensions[(idx)].value) & 0x03)

/* counter contraint parameters */
#define ESBMAN_COUNTER_FLAG_REPEATCOUNT             (1<<0)

#if defined( NPOPT_ENABLE_EXPORT_ACTION )
/* export info parameter */
#define ESBMAN_EXPORT_FLAG_EXINFO_PRESENT           (1<<0)
#endif

struct esbman_value_tag;
typedef struct {
    binstr_t                *name;
    struct esbman_value_tag *value;
} esbman_parameter_t;

typedef struct {
    esbman_parameter_t param;
    u_int32_t          flags;   /*  ESBMAN_PARAMETER_FLAG_*  */
} esbman_exparameter_t;

typedef struct {
    int32_t count;
    struct esbman_value_tag *values[1];
} esbman_valuelist_t;

typedef struct esbman_value_tag {
    int32_t  type;  /*  ESBMAN_VALUE_TYPE_*  */
    union {
        int32_t               i;
        float                 f;
        binstr_t             *str;
        u_int32_t             date;
        esbman_parameter_t   *param;
        esbman_exparameter_t *exparam;
        binstr_t             *resource;
        esbman_valuelist_t   *vallist;
        binstr_t             *bytearray;
    } data;
} esbman_value_t;

typedef struct {
    int32_t   id;
    binstr_t  *entrypoint;
    int32_t   cookie;
} l_callback_t;

typedef struct {
    u_int32_t    type;  /* ESBMAN_CALLBACK_TYPE_* */
    u_int32_t    flags;   /*  ESBMAN_PARAMETER_FLAG_*  */
    l_callback_t callback;
    /* A structure per callback */
    union {
        /* OnAccept */
        struct {
            int32_t dummy;
        } oa;
        /* OnTime */
        struct {
            time_date_t *date; /* UTC */
        } ot;
        /* OnTimeElapsed */
        struct {
            u_int32_t duration; /* in seconds */
        } ote;
        /* OnEvent */
        struct {
            u_int32_t event_type;  /*  ESBMAN_EVENT_TYPE_*  */
            int32_t   event_flags;
            int32_t   event_param;
        } oe;
        /* OnAgentCompletion */
        struct {
            int32_t  instance_id;
        } oac;
    } e;
} l_callback_param_t;

typedef struct l_basic_extension_tag {
    binstr_t    *name;
    u_int32_t    value;
} l_basic_extension_t;

typedef struct l_output_control_technnology_tag {
    u_int32_t   type;
    u_int32_t   flags;
    struct {
        struct {
            u_int32_t epn;
            u_int32_t cci;
            u_int32_t ict;  /* Image constraint token */
            u_int32_t dot;  /* digital only token */
            u_int32_t aps;
        } basic;
        struct {
            u_int32_t rmm;  /* Retention move mode */
            u_int32_t rs;   /* Retention state */
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
            u_int32_t             num;
            l_basic_extension_t  *extensions;   /* Array of name:value extensions */
        } basic_extension;
    } e;
} l_output_contol_technology_t;

typedef struct {
    u_int32_t    type;  /* ESBMAN_OBLIGATION_TYPE_* */
    u_int32_t    flags;   /*  ESBMAN_PARAMETER_FLAG_*  */
    union {
        struct {
            binstr_t *service_id;
            binstr_t *logical_id;
        } mp;
        struct {
            binstr_t           *control_id;
            binstr_t           *agent_name;
            int32_t            instance_id;
            binstr_t           *context_id;
            esbman_valuelist_t *parameters;
        } raop;
        struct {
            binstr_t                     *control_id;
            l_output_contol_technology_t *parameters;
        } oc;
    } e;
} l_obligation_param_t;

typedef struct {
    u_int32_t    type;  /* ESBMAN_PERMISSION_TYPE_* */
    u_int32_t    flags;   /*  ESBMAN_PARAMETER_FLAG_*  */
    union {
        struct {
            binstr_t                     *control_id;
            l_output_contol_technology_t *parameters;
        } oco;
    } e;
} l_permission_param_t;

typedef struct {
    u_int32_t flags; /* bit-OR of ESBMAN_FLAGS_* */
    /* obligations */
    u_int32_t            obligation_params_count;
    l_obligation_param_t *obligation_params;
    /* callbacks */
    u_int32_t           callback_params_count;
    l_callback_param_t  *callback_params;
    /* permissions */
    u_int32_t             permission_params_count;
    l_permission_param_t  *permission_params;
    /* description */
    struct {
        u_int32_t flags; /* represents which parameters below are enabled.
                        * bit-OR of ESBMAN_DESCRIPTION_FLAG_* */
        binstr_t *mdefault;
        binstr_t *mshort;
        binstr_t *mlong;
    } desc_c;
    /* generic constraint */
    struct {
        u_int32_t flags; /* represents which parameters below are enabled.
                        * bit-OR of ESBMAN_GENERIC_FLAG_* */
        u_int32_t nodereachability_count;
        binstr_t **nodereachability_array;
        int32_t required_license_status;
    } generic_c;
    /* temporal constraint */
    struct {
        u_int32_t flags; /* represents which parameters below are enabled.
                        * bit-OR of ESBMAN_TEMPORAL_FLAG_* */
        u_int32_t notbefore_count;
        u_int32_t notafter_count;
        time_date_t *notbefore[2]; /* notbefore */
        time_date_t *notafter[2];  /* notafter */
        time_date_t *notduring[2];
        int32_t notlongerthan;
        int32_t notmorethan;
    } temporal_c;
    /* spatial constraint */
    /* @note : no standard spatial constraint is defined in [8pusControls v1.0.1] spec */
    struct {
        u_int32_t flags; /* represents which parameters below are enabled.
                        * bit-OR of ESBMAN_SPATIAL_FLAG_* */
        u_int32_t proximityrequired;
    } spatial_c;
    /* group constraint */
    struct {
        u_int32_t flags; /* represents which parameters below are enabled.
                    * bit-OR of ESBMAN_GROUP_FLAG_* */
        binstr_t *membership;
        binstr_t *identity;
    } group_c;
    /* device constraint */
    struct {
        u_int32_t flags; /* represents which parameters below are enabled.
                    * bit-OR of ESBMAN_DEVICE_FLAG_* */
        binstr_t *type;
        binstr_t *feature;
        binstr_t *id;
        int32_t version_major;
        int32_t version_minor;
        int32_t dcs_freshness;
    } device_c;
    /* counter constraint */
    struct {
        u_int32_t flags;                 /* Represents which parameters are enabled ESBMAN_COUNTER_FLAG_* */
        u_int32_t repeat_count_max;      /* number of repeats that must not be exceeded */
        u_int32_t repeat_count_remain;   /* number of repeats that are still allowed.  */
    } counter_c;
#if defined( NPOPT_ENABLE_EXPORT_ACTION )
    /* information for export action */
    struct {
        u_int32_t     flags; /* ESBMAN_EXPORT_FLAG_* */
        union {
            u_int32_t i; /* All currently known result types are integers */
        } result;
    } export_info;
#endif
    /* custom callback
     * This structure is only used for testing purposes and is
     * not used/available in the public np_esb_t that maps the
     * esbman_info_t structure.
     *
     * For this reason any additions to esbman_info_t should be
     * defined befure this structure, not after */
    struct {
        bool_t       is_filled;
        int32_t      int_type;
        float        real_type;
        binstr_t    *string_type;
        time_date_t *date_type;
        binstr_t    *resource_type;
        binstr_t    *bytearray;
    } custom_cb;
} esbman_info_t;

/*---------------------------------------------------------------------------*/
/**
 * @brief Parse an Extended Status Block
 *
 * @param[in]  in_context    memory context
 * @param[in]  in_esbsize  ESB buffer length
 * @param[in]  in_esbbuf   ESB buffer
 * @param[out] out_info    ESB info structure; to be freed by npi_esbman_Free
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ESBMAN_ILLEGALDATA
 * @return ::ERR_ESBMAN_ILLEGALESB
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
npi_esbman_Parse(npi_cmncontext_t in_context,
                 u_int32_t        in_esbsize,
                 u_int8_t        *in_esbbuf,
                 esbman_info_t  **out_info);

/*---------------------------------------------------------------------------*/
/**
 * @brief Free the memory detained for io_esbinfo.
 *
 * @param[in]  in_context    memory context
 * @param[in,out]  io_esbinfo
 *
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
npi_esbman_Free( npi_cmncontext_t  in_context,
                 esbman_info_t    *io_info );

/*---------------------------------------------------------------------------*/
/**
 * @brief Serialize an Extended Status Block
 *
 * @note  As of N4.0, only obligations/permissions are serialized by this function
 *
 *
 * @param[in]  in_context   Memory context
 * @param[in]  in_info      ESB info structure to serialize
 * @param[out] out_esb      Serialized ESB; to be freed by caller.
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ESBMAN_ILLEGALDATA
 * @return ::ERR_ESBMAN_ILLEGALESB
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
npi_esbman_Compose(npi_cmncontext_t  in_context,
                   esbman_info_t    *in_info,
                   u_int32_t        *out_esbsize,
                   u_int8_t        **out_esbbuf);

/*---------------------------------------------------------------------------*/
/**
 * @brief Verify that an ESB does not contain any unknown critical parameters
 *
 * @param[in]  in_context    memory context
 * @param[in]  in_info       ESB info structure
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_ESBMAN_ILLEGALESB
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
npi_esbman_VerifyCritical( npi_cmncontext_t  in_context,
                           esbman_info_t    *in_info );

/*---------------------------------------------------------------------------*/
/**
 * @brief Free the memory detained for output contol technology structure (oct).
 *
 * @param[in]       in_context  memory context
 * @param[in,out]   io_oct      output contol technology structure
 *
 * @return ::ERR_OK
 */
/*---------------------------------------------------------------------------*/
extern void
npi_esbman_FreeOutputControlTechnology(npi_cmncontext_t in_context, l_output_contol_technology_t *io_oct);

NP_END_EXTERN_C

#endif /* ESBMAN_API_H */
/** @} */
