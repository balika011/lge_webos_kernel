#ifndef _WB_PBM_H_
#define _WB_PBM_H_

extern void WB_PBM_Init(UINT16 u2_param);
extern void WB_PBM_DeInit(UINT16 u2_param);
extern void WB_PBM_WriteDefaultToNVMIfNecessary(void);
extern void WB_PBM_MagicStrClear(void);
extern void WB_PBM_MagicStrSet(void);
extern BOOL WB_PBM_MagicStrCheck(void);
extern void WB_PBM_ColorTempIdxSet(UINT16 u2_idx);
extern UINT16 WB_PBM_ColorTempIdxGet(void);
extern void WB_PBM_WBAdjustMethodSet(UINT16 u2_idx);
extern UINT16 WB_PBM_WBAdjustMethodGet(void);
extern void WB_PBM_CoefSet(UINT32 u4_rgb_idx, UINT32 u4_pt_idx, UINT16 u2_value);
extern void WB_PBM_PQP_PGAM_LUT_Set(UINT16 *u1_pqp_lut , UINT16 wbt_index );
extern UINT16 WB_PBM_PQP_PGAM_LUT_Get(UINT8 ui1_rgb_idx,UINT8 ui1_lvl_idx , UINT8 wbt_index );

extern UINT16 WB_PBM_CoefGet(UINT32 u4_rgb_idx, UINT32 u4_pt_idx);
extern void WB_PBM_ColorTemp_Idx_Set(UINT16 ui2ClrTempIdx);
extern UINT16 WB_PBM_ColorTemp_Idx_Get(void);

extern void WB_PBM_CoefClear(void);
extern void WB_PBM_CoefWriteDefault(void);
extern void WB_PBM_EasySetPatternLevelSet(UINT16 ui2_level);
extern UINT16 WB_PBM_EasySetPatternLevelGet(void);
extern void WB_PBM_EasySetPatternEnableSet(UINT16 ui2_en);
extern UINT16 WB_PBM_EasySetPatternEnableGet(void);
extern void WB_PBM_InpEasySetRSet(UINT16 u2_idx);
extern UINT16 WB_PBM_InpEasySetRGet(void);
extern void WB_PBM_InpEasySetGSet(UINT16 u2_idx);
extern UINT16 WB_PBM_InpEasySetGGet(void);
extern void WB_PBM_InpEasySetBSet(UINT16 u2_idx);
extern UINT16 WB_PBM_InpEasySetBGet(void);
extern void WB_PBM_WBTIdxChangeStsSet(UINT16 u2_idx);
extern UINT16 WB_PBM_WBTIdxChangeStsGet(void);
extern void WB_PBM_LutAdjOnSet(UINT16 u2_idx);
extern UINT16 WB_PBM_LutAdjOnGet(void);
extern void WB_PBM_PixelMeasCRSet(UINT16 u2_idx);
extern UINT16 WB_PBM_PixelMeasCRGet(void);
extern void WB_PBM_PixelMeasCBSet(UINT16 u2_idx);
extern UINT16 WB_PBM_PixelMeasCBGet(void);
extern void WB_PBM_PixelMeasYSet(UINT16 u2_idx);
extern UINT16 WB_PBM_PixelMeasYGet(void);
extern void WB_PBM_YSumPixelPosSet(UINT16 u2_idx);
extern UINT16 WB_PBM_YSumPixelPosGet(void);
extern void WB_PBM_YSumPosSet(UINT16 u2_idx);
extern UINT16 WB_PBM_YSumPosGet(void);
extern void WB_PBM_YSumMeasSet(UINT16 u2_idx);
extern UINT16 WB_PBM_YSumMeasGet(void);
extern void AgingShowPattern(UINT16 u2_enable);
extern void AgingLastPowerControlState_Set(UINT16 u2_idx);
extern UINT16 AgingLastPowerControlState_Get(void);

#if 1//def APP_LCCT_2H
extern UINT32 Sony_PBM_EDID_Write (UINT16 write_data);
#endif  //APP_LCCT_2H

#endif
