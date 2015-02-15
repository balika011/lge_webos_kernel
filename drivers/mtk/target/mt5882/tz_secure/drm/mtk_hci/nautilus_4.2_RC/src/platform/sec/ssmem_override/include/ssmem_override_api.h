/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011 Sony Corporation.
 */
#ifndef SSMEM_OVERRIDE_API_H
#define SSMEM_OVERRIDE_API_H

/* Getters */

extern retcode_t
ssmem_override_NemoPubPersObjGet(int32_t     in_type, 
                                 int32_t     in_index, 
                                 binstr_t  **out_obj);

extern retcode_t
ssmem_override_RoleAssertObjGet(int32_t     in_type, 
                                int32_t     in_index, 
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_OctPubPersObjGet(int32_t     in_type, 
                                int32_t     in_index, 
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_TrustedTimeObjGet(int32_t     in_type, 
                                 int32_t     in_index, 
                                 binstr_t  **out_obj);
extern retcode_t
ssmem_override_ImportLsPubPersObjGet(int32_t     in_type,
                                     int32_t     in_index,
                                     binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportNemoPubPersObjGet(int32_t     in_type,
                                       int32_t     in_index,
                                       binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportIROLObjGet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportRROLObjGet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportLROLObjGet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportCDLCObjGet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportLDLCObjGet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t  **out_obj);

extern retcode_t
ssmem_override_ImportRsPubPersObjGet(int32_t     in_type,
                                     int32_t     in_index,
                                     binstr_t  **out_obj);

/* Setters */

extern retcode_t
ssmem_override_NemoPubPersObjSet(int32_t    in_type, 
                                 int32_t    in_index, 
                                 binstr_t  *in_obj);

extern retcode_t
ssmem_override_RoleAssertObjSet(int32_t     in_type, 
                                int32_t     in_index, 
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_OctPubPersObjSet(int32_t     in_type, 
                                int32_t     in_index, 
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_TrustedTimeObjSet(int32_t    in_type,
                                 int32_t    in_index,
                                 binstr_t  *in_obj);

extern retcode_t
ssmem_override_ImportLsPubPersObjSet(int32_t     in_type,
                                     int32_t     in_index,
                                     binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportNemoPubPersObjSet(int32_t     in_type,
                                       int32_t     in_index,
                                       binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportIROLObjSet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportRROLObjSet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportLROLObjSet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportCDLCObjSet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportLDLCObjSet(int32_t     in_type,
                                int32_t     in_index,
                                binstr_t   *in_obj);

extern retcode_t
ssmem_override_ImportRsPubPersObjSet(int32_t     in_type,
                                     int32_t     in_index,
                                     binstr_t   *in_obj);

#endif /* SSMEM_OVERRIDE_API_H */

