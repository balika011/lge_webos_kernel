/** @addtogroup reg reg
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef REG_MULTISDATA_H
#define REG_MULTISDATA_H

#define L_MULTIPLE_SDATA_TYPE_LINK 0
#define L_MULTIPLE_SDATA_TYPE_DCSA 1
#define L_MULTIPLE_SDATA_TYPE_NODE 2

#define L_SDATA_MASTER      0
#define L_SDATA_CHILDTOP    1

typedef struct reg_bitmap_tag {
    int32_t        m_max;
    u_int8_t       m_body[1];
} reg_bitmap_t;

extern retcode_t
reg_l_GetMultipleObjectFromIndex(int32_t in_index, int32_t in_object_type, binstr_t **out_sdata);

extern retcode_t
reg_l_GetMultipleObjectCount(int32_t *out_count, int32_t in_object_type);

extern retcode_t
reg_l_DeregMultipleSdata(int32_t in_type, u_int8_t *in_id1, u_int8_t *in_id2, u_int8_t *in_id3, bool_t in_propagate);

extern retcode_t
reg_l_GetMultipleSdata(int32_t in_type, u_int8_t *in_id1, u_int8_t *in_id2, u_int8_t *in_id3, binstr_t **out_sdata, bool_t in_propagate);

extern retcode_t
reg_l_RegMultipleSdata(int32_t in_type, u_int8_t *in_id1, u_int8_t *in_id2, u_int8_t *in_id3, np_size_t in_sdatasize, u_int8_t *in_sdatabuf);

extern retcode_t
reg_l_ClearSdataByType(int32_t in_regtype);

extern retcode_t
reg_l_CheckIntegrity(int32_t in_type);

#if defined(NPOPT_ENABLE_CHECK_SINGLEUSER)
extern retcode_t
reg_l_CheckSingleUser(u_int8_t *in_accountid, int32_t in_object_type);
#endif

extern retcode_t
reg_l_BitmapInit(int32_t in_n,reg_bitmap_t **out_bm);

extern retcode_t
reg_l_BitmapSet(reg_bitmap_t *in_bm,int32_t  in_index, int32_t   in_bool);

extern retcode_t
reg_l_BitmapScan(reg_bitmap_t  *in_bm,int32_t in_bool,int32_t *out_index);

extern retcode_t
reg_l_BitmapFin(reg_bitmap_t *in_bm);

#endif /* REG_MULTISDATA_H */
