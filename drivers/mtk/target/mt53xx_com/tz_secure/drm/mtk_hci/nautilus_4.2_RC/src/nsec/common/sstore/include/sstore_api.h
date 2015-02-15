/** @addtogroup sstore sstore
 *  @ingroup nsec
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010 Sony Corporation.
 */
#ifndef SSTORE_API_H
#define SSTORE_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief storage manager
 *
 * dependency: (to-do)
 *
 * @author Norifumi Goto <takas@sm.sony.co.jp>
 */
/*===========================================================================*/

/**
 * @brief Load the secure data (read it from files)
 *
 * @param[out] out_sdata  Serialized secure data
 */
retcode_t
sstore_LoadSdata(binstr_t **out_sdata);

/**
 * @brief Save the secure data (write it to files)
 *
 * @param[in] in_sdata  Serialized secure data
 */
retcode_t
sstore_SaveSdata(binstr_t *in_sdata);

NP_END_EXTERN_C

#endif
/** @} */
