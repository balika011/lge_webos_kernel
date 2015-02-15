/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef IMPORT_MAPPING_H
#define IMPORT_MAPPING_H

NP_BEGIN_EXTERN_C

/**
 * @brief Map import descriptor to esb
 *
 * @param[in]    in_import_desc    Import descriptor, such as DTCP descriptor
 * @param[out]   out_esb           OC obligations and permissions for PLAY
 *
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALARGS
 *
 */
retcode_t
import_l_MapUsageRules(import_desc_t   *in_import_desc,
                       esbman_info_t  **out_esb);


NP_END_EXTERN_C

#endif /* IMPORT_MAPPING_H */

