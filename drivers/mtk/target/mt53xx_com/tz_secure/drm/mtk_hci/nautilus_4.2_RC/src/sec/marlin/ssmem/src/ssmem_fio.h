/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 *
 */
#if 0 /* functions not being called */

#ifndef SSMEM_FIO_H
#define SSMEM_FIO_H

#ifdef __cplusplus
extern "C" {
#endif

    retcode_t
    ssmem_l_Write(
        u_int32_t   in_tag,
        binstr_t    *in_obj);

    retcode_t
    ssmem_l_Read(
        u_int32_t   in_tag,
        binstr_t    **out_protected_obj);

    retcode_t
    ssmem_l_Remove(
        u_int32_t   in_tag);

#ifdef __cplusplus
}
#endif

#endif  /* SSMEM_FIO_H */

#endif /* functions not being called */
