/** @addtogroup oct2xml oct2xml
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef OCT2XML_VALIDATION_H
#define OCT2XML_VALIDATION_H

NP_BEGIN_EXTERN_C

/**
 * @brief Validate an 8pus license objects contents
 *
 * @param[in] in_license  License object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
oct2xml_l_ValidateLicense( octobj_license_t *in_license );

/**
 * @brief Validate an 8pus content key
 *
 * @param[in] in_contentkey   Key object
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 */
extern retcode_t
oct2xml_l_ValidateContentKey( octobj_contentkey_t  *in_contentkey );

NP_END_EXTERN_C


#endif /* OCT2XML_VALIDATION_H */

/** @} */
