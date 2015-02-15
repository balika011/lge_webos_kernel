/** @addtogroup esbman esbman
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

#ifndef ESBMAN_TYPES_H
#define ESBMAN_TYPES_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief ESB(Extended Status Block) Manager
 *
 * @author Yoshizumi Tanaka <y-tanaka@sm.sony.co.jp>
 * @date   14 Jun, 2006 last modified by y-tanaka
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/
/* local flags */
#define ESBMAN_LOCALFLAG_OBLIGATIONNOTICE         0x00000001
#define ESBMAN_LOCALFLAG_CALLBACKNOTICE           0x00000002
#define ESBMAN_LOCALFLAG_GENERICCONSTRAINT        0x00000004
#define ESBMAN_LOCALFLAG_TEMPORALCONSTRAINT       0x00000008
#define ESBMAN_LOCALFLAG_SPATIALCONSTRAINT        0x00000010
#define ESBMAN_LOCALFLAG_GROUPCONSTRAINT          0x00000020
#define ESBMAN_LOCALFLAG_DEVICECONSTRAINT         0x00000040
#define ESBMAN_LOCALFLAG_COUNTERCONSTRAINT        0x00000080

/* Action.Check/Perform */
/* category */
#define ESBMAN_CATEGORY_ACTION_GRANTED   0
#define ESBMAN_CATEGORY_ACTION_DENIED    1
/* sub-category */
#define ESBMAN_CATEGORY_ACTION_GRANTED_UNSPECIFIED   0
#define ESBMAN_CATEGORY_ACTION_DENIED_UNSPECIFIED    0

/* Action.Describe */
/* category */
#define ESBMAN_CATEGORY_DESCRIBE_UNSPECIFIED    0
/* sub-category */
#define ESBMAN_CATEGORY_DESCRIBE_UNSPECIFIED_UNSPECIFIED    0

/* cache duration type */
#define ESBMAN_CACHEDURATION_TYPE_RELATIVE 0 /* offset from current */
#define ESBMAN_CACHEDURATION_TYPE_ABSOLETE 1 /* offset from Jan 1,1970 */

typedef struct {
    int32_t type;
    int32_t value;
} esbman_cacheduration_t;

typedef struct {
    u_int32_t               globalflags;
    int32_t                 category;
    int32_t                 subcategory;
    u_int32_t               localflags;
    esbman_cacheduration_t  cacheduration;
    esbman_valuelist_t     *parameters;
} esbman_exstatus_t;


extern retcode_t
npi_esbman_l_ExStatusToBuf(npi_cmncontext_t    in_context,
                           esbman_exstatus_t  *in_exstatus,
                           u_int32_t          *out_esbsize,
                           u_int8_t          **out_esbbuf);

extern retcode_t
npi_esbman_l_BufToExStatus(npi_cmncontext_t    in_context,
                           u_int32_t           in_esbsize,
                           u_int8_t           *in_esbbuf,
                           esbman_exstatus_t **out_exstat);

extern retcode_t
npi_esbman_l_FreeExStatus(npi_cmncontext_t in_context,esbman_exstatus_t *io_exstat);

extern retcode_t
npi_esbman_l_FreeValue(npi_cmncontext_t in_context,esbman_value_t *io_value);

extern retcode_t
npi_esbman_l_FreeValueList(npi_cmncontext_t in_context,esbman_valuelist_t *io_vallist);

extern retcode_t
npi_esbman_l_FreeParameter(npi_cmncontext_t in_context,esbman_parameter_t *io_param);

extern retcode_t
npi_esbman_l_FreeExParameter(npi_cmncontext_t in_context,esbman_exparameter_t *io_exparam);

NP_END_EXTERN_C

#endif /* ESBMAN_TYPES_H */
/** @} */
