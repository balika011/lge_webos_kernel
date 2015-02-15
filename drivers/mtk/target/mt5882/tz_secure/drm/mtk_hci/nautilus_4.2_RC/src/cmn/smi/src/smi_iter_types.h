/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 */


#ifndef SMI_ITER_TYPES_H
#define SMI_ITER_TYPES_H

NP_BEGIN_EXTERN_C

/**
 * @brief Aggregation struct to be used in conjunction with
 *        npi_smi_ParamConvertor. The purpose of this combo is to convert smi
 *        message parameters to smi value parameters (of type <smi_param_val_t>).
 */
typedef struct {
    u_int32_t        current_param_idx;
    smi_param_val_t *params;
} smi_paramsholder_t;

/**
 * @brief Aggregation struct to be used in conjunction with
 *        npi_smi_AddPayloadItem. The purpose of this combo is to convert smi
 *        value parameters (of type <smi_param_val_t>) to smi message
 *        parameters and store them in a payload buffer.
 */
typedef struct {
    u_int32_t    last_byte;
    u_int32_t    size;
    u_int8_t    *buffer;
} smi_payload_t;

NP_END_EXTERN_C

#endif /* SMI_ITER_TYPES_H */

