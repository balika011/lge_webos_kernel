/** @addtogroup reg reg
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */
#ifndef REG_PIPR_API_H
#define REG_PIPR_API_H

NP_BEGIN_EXTERN_C

/*---------------------------------------------------------------------------*/
/**
 * @brief Register an internal structure nemo personality (public part)
 *
 * @param[in] in_nemonode   Internal structure NEMO personality
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegInternalNEMOPublicPersonality(xml2oct_nemonode_t *in_nemonode);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the internal structure nemo personality (public part)
 *
 * @param[out] out_nemonode     Internal structure NEMO personality
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetInternalNEMOPublicPersonality(xml2oct_nemonode_t **out_nemonode);

/*---------------------------------------------------------------------------*/
/**
 * @brief Register an octopus public personality
 *
 * @param[in] in_personality   octopus public personality
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_RegInternalOctopusPublicPersonality(binstr_t *in_personality);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieve the internal structure role assertion
 *
 * @param[out] out_assertion     Internal structure role assertion
 *
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
reg_GetInternalRoleAssertion(octobj_assertion_t **out_assertion);

NP_END_EXTERN_C

#endif /* REG_PIPR_API_H */
/** @} */
