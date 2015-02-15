/** @addtogroup ssmem_protect ssmem_protect
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010 Sony Corporation.
 */
#ifndef SSMEM_PROTECT_H
#define SSMEM_PROTECT_H

/** @file */

/*
 * size
 */
#define SSMEM_L_IVSIZE        16
#define SSMEM_L_DIGESTSIZE    20

/*---------------------------------------------------------------------------*/
/**
 * @brief Protect a secure data object
 *
 * @param in_tag    Object tag
 * @param in_obj    Object
 * @param out_obj   Object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_l_Protect(
    u_int32_t   in_tag,
    binstr_t    *in_obj,
    binstr_t    **out_obj);

/*---------------------------------------------------------------------------*/
/**
 * @brief Verify a secure data object
 *
 * @param in_tag    Object tag
 * @param in_obj    Object
 * @param out_obj   Object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
/*---------------------------------------------------------------------------*/
extern retcode_t
ssmem_l_Verify(
    u_int32_t   in_tag,
    binstr_t    *in_obj,
    binstr_t    **out_obj);

#endif /* SSMEM_PROTECT_H */
/** @} */
