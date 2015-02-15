/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 */

#ifndef SMI_ITER_API_H
#define SMI_ITER_API_H

NP_BEGIN_EXTERN_C

#include "smi_iter_types.h"

/***************************************************
 * ITERATION ACROSS PARAMETERS INSIDE SMI MESSAGES *
 ***************************************************/
/**
 * @brief Definition for an aggregation function used
 *        by ::npi_smi_l_IterateMsgParams.
 */
typedef retcode_t(*msg_param_aggregation_t)(npi_cmncontext_t in_context, u_int8_t *in_param, void *io_aggregator);

/**
 * @brief This function will traverse all parameters inside the message, and
 *        for each of them, it will call the aggregation function that was
 *        passed in. It also passes in the in_aggregator parameter which can be
 *        used by the aggregation function to capture results of the traversal.
 *
 * @param[in]     in_context    The memory context to use.
 * @param[in]     in_msg        The smi message whose parameters need to be
 *                                  traversed.
 * @param[in]     in_process    The function that needs to be called on each
 *                                  parameter inside the smi message.
 * @param[in,out] io_aggregator An aggregator struct, used by in_process to
 *                                  aggregate intermediate and end results of
 *                                  traversing the params in the smi message.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::in_process
 */
retcode_t
npi_smi_l_IterateMsgParams(npi_cmncontext_t            in_context,
                           binstr_t                   *in_msg,
                           msg_param_aggregation_t     in_process,
                           void                       *io_aggregator);

/**
 * @brief An aggregation function for counting the number of parameters in an
 *        smi message that can be supplied to ::npi_smi_l_IterateMsgParams. It
 *        needs to be provided with an smi_paramscounter_t struct as
 *        aggregation struct to hold results.
 *
 * @param[in]       in_context      The memory context to use.
 * @param[in]       in_param        An smi message parameter.
 * @param[in,out]   io_aggregator   An aggregation struct (needs to be of the
 *                                      type smi_paramscounter_t).
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
retcode_t
npi_smi_l_Increment(npi_cmncontext_t   in_context,
                    u_int8_t          *in_param,
                    void              *io_aggregator);

/**
 * @brief An aggregation function for converting smi message parameters to smi
 *        value parameters (of type <smi_param_val_t>).
 *
 * @param[in]       in_context      The memory context to use.
 * @param[in]       in_param        An smi message parameter.
 * @param[in,out]   io_aggregator   An aggregation struct (needs to be of the
 *                                      type <smi_paramsholder_t>).
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 */
retcode_t
npi_smi_l_ParamConvertor(npi_cmncontext_t    in_context,
                         u_int8_t           *in_param,
                         void               *io_aggregator);

/************************************************************
 * ITERATION ACROSS PARAMETERS INSIDE PARAMETER VALUE LISTS *
 ************************************************************/
/**
 * @brief Definition for an aggregation function used by
 *        ::npi_smi_l_IterateParamVals.
 */
typedef retcode_t(*param_val_aggregation_t)(smi_param_val_t *in_param, void *io_aggregator);

/**
 * @brief This function will traverse all parameter values inside the parameter
 *        value list, and for each of them, it will call the aggregation
 *        function that was passed in. It also passes in the in_aggregator
 *        parameter which can be used by the aggregation function to capture
 *        results of the traversal.
 *
 * @param[in]     in_params     The parameter value list that needs to be
 *                                  traversed.
 * @param[in]     in_process    The function that needs to be called on each
 *                                  parameter in the parameter value list.
 * @param[in,out] io_aggregator An aggregator struct, used by in_process to
 *                                  aggregate intermediate and end results of
 *                                  traversing the params in the parameter
 *                                  value list.
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::in_process
 */
retcode_t
npi_smi_l_IterateParamVals(smi_params_t               *in_params,
                           param_val_aggregation_t     in_process,
                           void                       *io_aggregator);

/**
 * @brief An aggregation function for calculating the size of the payload of an
 *        smi message based on the parameters in the parameter value list.
 *
 * @param[in]       in_param        An smi value parameter.
 * @param[in,out]   io_aggregator   An aggregation struct (needs to be of the
 *                                      type <smi_payloadsize_t>).
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
retcode_t
npi_smi_l_CalcSize(smi_param_val_t   *in_param,
                   void              *io_aggregator);

/**
 * @brief An aggregation function for converting smi value parameters (of type
 *        <smi_param_val_t>) to smi message parameters and store them in a
 *        payload buffer.
 *
 * @param[in]       in_param        An smi value parameter.
 * @param[in,out]   io_aggregator   An aggregation struct (needs to be of the
 *                                      type <smi_payload_t>).
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 */
retcode_t
npi_smi_l_AddPayloadItem(smi_param_val_t    *in_param,
                         void               *io_aggregator);

NP_END_EXTERN_C

#endif /* SMI_ITER_API_H */

