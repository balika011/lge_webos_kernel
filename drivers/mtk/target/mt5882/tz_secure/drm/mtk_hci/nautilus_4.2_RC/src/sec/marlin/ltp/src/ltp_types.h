/** @addtogroup ltp ltp
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011,2012 Sony Corporation.
 */
#ifndef LTP_TYPES_H
#define LTP_TYPES_H

NP_BEGIN_EXTERN_C

#include "ns_defs.h"

typedef struct ltp_l_context_tag {
    binstr_t       *license;
    xml_node_t     *extention_bundle;
} ltp_l_context_t;

#define LTP_OPERATION_MOVE     "urn:marlin:core:1-0:service:license-transfer:move"
#define LTP_OPERATION_COPY     "urn:marlin:core:1-0:service:license-transfer:copy"
#define LTP_OPERATION_RENDER   "urn:marlin:core:1-0:service:license-transfer:render"
#define LTP_OPERATION_CHECKOUT "urn:marlin:core:1-0:service:license-transfer:checkout"
#define LTP_OPERATION_RELEASE  "urn:marlin:core:1-3:service:license-transfer:release"

/* Marlin LTP */

NP_END_EXTERN_C

#endif /* LTP_TYPES_H */

