/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef NPPERS_TYPES_H
#define NPPERS_TYPES_H

NP_BEGIN_EXTERN_C

/* -------------------------------------------------------------------------- */
/*                        DATA STRUCTURES                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Personalization keytype (Used by Personalization 2.0.0)
 *
 * Type to specify the type of Personalization keytype
 **/
typedef enum {
     NP_PERS_DEVICE_KEYTYPE           = 1,    /** device keytype*/
     NP_PERS_IMPORT_KEYTYPE           = 2,    /** import keytype*/
     NP_PERS_DEVICE_IMPORT_KEYTYPE    = 3     /** device + import keytype */
} np_pers_keytype_t;

NP_END_EXTERN_C

#endif /* NPPERS_TYPES_H */

