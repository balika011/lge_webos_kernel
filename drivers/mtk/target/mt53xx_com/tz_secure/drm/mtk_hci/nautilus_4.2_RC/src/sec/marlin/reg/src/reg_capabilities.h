/** @addtogroup reg reg
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2012 Sony Corporation.
 */

#ifndef REG_CAPABILITIES_H
#define REG_CAPABILITIES_H

#if defined( NPOPT_ENABLE_CAPABILITY_SIGNALLING )
/**
 * @brief Clear whole cache
 *
 * @return ::ERR_OK
 */
retcode_t
reg_l_ClearCache(void);

/**
 * @brief Capabilities accessor, if no capabilities exist they will
 *        be created.
 *
 * @param[out] out_assertion out assertions
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 */
retcode_t
reg_l_GetCapabilities( octobj_assertion_t **out_assertion );
#endif

#endif /* REG_CAPABILITIES_H */
