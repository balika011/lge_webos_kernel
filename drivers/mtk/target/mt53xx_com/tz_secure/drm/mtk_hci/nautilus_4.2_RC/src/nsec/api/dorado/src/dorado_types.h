/** @addtogroup dorado dorado
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010 Sony Corporation.
 */

#ifndef DOR_L_TYPES
#define DOR_L_TYPES

typedef struct {
    dor_callback_t      *cb;
    dor_notify_progress  progress;
    np_handle_t          smi;
    void                *connection;
} dor_l_context_t;

#endif /* DOR_L_TYPES */
/** @} */
