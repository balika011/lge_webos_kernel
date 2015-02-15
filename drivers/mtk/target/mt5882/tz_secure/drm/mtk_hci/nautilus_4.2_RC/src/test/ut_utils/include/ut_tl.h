/** @addtogroup ut_utils ut_utils
 *  @ingroup ut
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 *
 */
#ifndef UT_TL_H
#define UT_TL_H
#include <stdio.h>

NP_BEGIN_EXTERN_C

/**
 * @brief Initialize a target platform
 *
 * @param[in]  in_info   Incoming target info
 */
void
ut_testPlatformInit( np_target_info_t *in_info );

/**
 * @brief Finalize a target platform
 *
 * @param[in]  in_info   Incoming target info
 */
void
ut_testPlatformFin( np_target_info_t *in_info );

/**
 * @brief Get a local transport layer for unit tests
 *
 */
np_handle_t
ut_getTestLocalTransport(void);

/**
 * @brief Get a remote transport layer for unit tests
 *
 */
np_handle_t
ut_getTestRemoteTransport(void);
NP_END_EXTERN_C

#endif /* UT_UTILS_H */
/** @} */


