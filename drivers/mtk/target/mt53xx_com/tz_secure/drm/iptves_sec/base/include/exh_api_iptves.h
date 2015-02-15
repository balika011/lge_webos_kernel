/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef EXH_API_IPTVES_H
#define EXH_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Error Handling Definition and API
 *
 * require: base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define exh_IsOk()      (RET_EXH_OK == exh_Get())
#define exh_InitOk()    u_int32_t exh_ret = RET_EXH_OK
#define exh_Set(e)      exh_ret = (e)
#define exh_SetOk()     exh_Set(RET_EXH_OK)
#define exh_Get()       exh_ret
#define exh_Rethrow()   goto exh_CLEANUP
#define exh_Return()    return exh_Get()
#define exh_Check(o)                            \
    do {                                        \
        exh_Set(o);                             \
        if ( !exh_IsOk() ) {                    \
            IPTV_ERRLOG(exh_Get(),#o);               \
            exh_Rethrow();                      \
        }                                       \
    } while (0)
#define exh_Throw(e)                            \
    do {                                        \
        exh_Set(e);                             \
        if ( !exh_IsOk() ) {                    \
            IPTV_ERRLOG(exh_Get(),#e);               \
        }                                       \
        exh_Rethrow();                          \
    } while (0)

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_EXH_OK              0

/*============================================================================
 * APIs
 *==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* EXH_API_H */
