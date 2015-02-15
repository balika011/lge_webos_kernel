/******************************************************************
 *	 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 1999 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 ******************************************************************/

/** @file hal_vpq_def.h
 *
 *  soc driver definition header.
 *
 *  @author	ysjun(greenco@lge.com)
 *  @author	Kim, Kyoung-Hoan(dreamerkim@lge.com)
 *  @version	1.0
 *  @date		2007.02.23
 *  @see
 */

#ifndef	_HAL_VPQ_DEF_H_
#define	_HAL_VPQ_DEF_H_

/******************************************************************
	#include 파일들
	(File Inclusions)
*******************************************************************/

/******************************************************************
	상수 정의
	(Constant Definitions)
*******************************************************************/
#define CHIP_CM_REGION_WHITE    0
#define CHIP_CM_REGION_WHT_BLU  1
#define CHIP_CM_REGION_BLACK    2
#define CHIP_CM_REGION_SKIN     3
#define CHIP_CM_REGION_GRN      4
#define CHIP_CM_REGION_BLU      5
#define CHIP_CM_REGION_RED      6
#define CHIP_CM_REGION_YEL      7
#define CHIP_CM_REGION_MGT      8
#define CHIP_CM_REGION_CYN      9
#define CHIP_CM_REGION_A        10
#define CHIP_CM_REGION_B        11
#define CHIP_CM_REGION_C        12
#define CHIP_CM_REGION_D        13
#define CHIP_CM_REGION_E        14
#define CHIP_CM_REGION_F        15

#define CHIP_CM_TBLPOINT        8
#define CHIP_CM_REGION_MAX      15
#define CHIP_CM_REGION_NUM      (CHIP_CM_REGION_MAX+1)
#define CHIP_CM_DELTANUM        6
#define CHIP_CM_CW_NUM          16
#define CHIP_CM_CW_GAINTB       5

/* define for cm region control num */
#if (PLATFORM_CHIP_VER==lg1156 || PLATFORM_CHIP_VER==lg1154 || PLATFORM_CHIP_VER==lg1152 )
#define CHIP_CM_RGN_CTRL_NUM  10
#else
#define CHIP_CM_RGN_CTRL_NUM  16
#endif

#define PICTURE_CTRL_CONTRAST      0x00000001
#define PICTURE_CTRL_BRIGHTNESS    0x00000002
#define PICTURE_CTRL_SATURATION    0x00000004
#define PICTURE_CTRL_TINT          0x00000008

/*******************************************************************
	매크로 함수 정의
	(Macro Definitions)
*******************************************************************/

/*******************************************************************
   	 형 정의
 	 (Type Definitions)
*******************************************************************/
/**
*	define Picture Control ( PC )
*/
typedef struct CHIP_PC
{
	SINT32 sContrast;
	SINT32 sBrightness;
	SINT32 sSaturation;
	SINT32 sHue;
	SINT16 sDimming;	/* 2007.03.28 ysjun add */
} CHIP_PC_T;
typedef struct { 
    UINT8   u1lc_low; 
    UINT8   u1lc_low_neg; 
    UINT8   u1lc_mid; 
    UINT8   u1lc_mid_neg; 
    UINT8   u1lc_high; 
    UINT8   u1lc_high_neg; 
    UINT8   u1lc_low_coring; 
    UINT8   u1lc_mid_coring; 
    UINT8   u1lc_high_coring; 
    UINT8   u1lc_low_limit_pos; 
    UINT8   u1lc_low_limit_neg; 
    UINT8   u1lc_mid_limit_pos; 
    UINT8   u1lc_mid_limit_neg; 
    UINT8   u1lc_high_limit_pos; 
    UINT8   u1lc_high_limit_neg; 
    UINT8   u1lc_clip_pos_en; 
    UINT8   u1lc_clip_neg_en; 
    UINT8   u1lc_table_sel; 
    UINT8   u1lc_table_gain; 
    UINT8   u1lc_table_offset; 
    UINT8   u1ac_lpf_coef; 
} CHIP_SHP_SR_T; 

/*
*
*    HAL_VPQ_SetSharpness(VIDEO_WID_T wId, UINT16 *pShpVal, void *pstData)
*    CHIP_SHP_T structure
*/

typedef struct { 
    UINT8   u1aaf_sel; 
} CHIP_AAF_T; 


typedef struct { 
    UINT8   u1cbeenable; 
    UINT8   u1wei_it3; 
    UINT8   u1wei_it5; 
} CHIP_CBE_T; 


typedef struct { 
    UINT8   u1low; 
    UINT8   u1low_neg; 
    UINT8   u1mid; 
    UINT8   u1mid_neg; 
    UINT8   u1high; 
    UINT8   u1high_neg; 
    UINT8   u1low_coring; 
    UINT8   u1mid_coring; 
    UINT8   u1high_coring; 
    UINT8   u1low_limit_pos; 
    UINT8   u1low_limit_neg; 
    UINT8   u1mid_limit_pos; 
    UINT8   u1mid_limit_neg; 
    UINT8   u1high_limit_pos; 
    UINT8   u1high_limit_neg; 
    UINT8   u1clip_pos_en; 
    UINT8   u1clip_neg_en; 
    UINT8   u1gb_clip_pos_en_l; 
    UINT8   u1gb_clip_neg_en_l; 
    UINT8   u1gb_clip_pos_en_m; 
    UINT8   u1gb_clip_neg_en_m; 
    UINT8   u1gb_clip_pos_en_h; 
    UINT8   u1gb_clip_neg_en_h; 
    UINT8   u1clip_front_thr_low; 
    UINT8   u1clip_front_thr_high; 
    UINT8   u1clip_pos; 
    UINT8   u1clip_neg; 
    UINT8   u1soft_clip_gain; 
} CHIP_HSHARP_T; 


typedef struct { 
    UINT8   u1cor_zero_h1; 
    UINT8   u1cor_zero_h2; 
    UINT8   u1cor_zero_h3; 
    UINT8   u1cor_zero_d1; 
    UINT8   u1cor_zero_d2; 
    UINT8   u1cor_zero_d3; 
    UINT8   u1cor_zero_v1; 
    UINT8   u1cor_zero_v2; 
    UINT8   u1cor_zero_v3; 
    UINT8   u1cor_ratio_h1; 
    UINT8   u1cor_ratio_h2; 
    UINT8   u1cor_ratio_h3; 
    UINT8   u1cor_ratio_d1; 
    UINT8   u1cor_ratio_d2; 
    UINT8   u1cor_ratio_d3; 
    UINT8   u1cor_ratio_v1; 
    UINT8   u1cor_ratio_v2; 
    UINT8   u1cor_ratio_v3; 
    UINT8   u1cor_thr_h1; 
    UINT8   u1cor_thr_h2; 
    UINT8   u1cor_thr_h3; 
    UINT8   u1cor_thr_d1; 
    UINT8   u1cor_thr_d2; 
    UINT8   u1cor_thr_d3; 
    UINT8   u1cor_thr_v1; 
    UINT8   u1cor_thr_v2; 
    UINT8   u1cor_thr_v3; 
    UINT8   u1gain_h1; 
    UINT8   u1gain_h2; 
    UINT8   u1gain_h3; 
    UINT8   u1gain_d1; 
    UINT8   u1gain_d2; 
    UINT8   u1gain_d3; 
    UINT8   u1gain_v1; 
    UINT8   u1gain_v2; 
    UINT8   u1gain_v3; 
    UINT8   u1limit_h1; 
    UINT8   u1limit_h2; 
    UINT8   u1limit_h3; 
    UINT8   u1limit_d1; 
    UINT8   u1limit_d2; 
    UINT8   u1limit_d3; 
    UINT8   u1limit_v1; 
    UINT8   u1limit_v2; 
    UINT8   u1limit_v3; 
    UINT8   u1limit_ratio_h1; 
    UINT8   u1limit_ratio_h2; 
    UINT8   u1limit_ratio_h3; 
    UINT8   u1limit_ratio_d1; 
    UINT8   u1limit_ratio_d2; 
    UINT8   u1limit_ratio_d3; 
    UINT8   u1limit_ratio_v1; 
    UINT8   u1limit_ratio_v2; 
    UINT8   u1limit_ratio_v3; 
    UINT8   u1bound_h1; 
    UINT8   u1bound_h2; 
    UINT8   u1bound_h3; 
    UINT8   u1bound_d1; 
    UINT8   u1bound_d2; 
    UINT8   u1bound_d3; 
    UINT8   u1bound_v1; 
    UINT8   u1bound_v2; 
    UINT8   u1bound_v3; 
    UINT8   u1clip_ratio_h; 
    UINT8   u1clip_pos_h; 
    UINT8   u1clip_neg_h; 
    UINT8   u1shp_detail_gain_lh; 
    UINT8   u1clip_ratio_d; 
    UINT8   u1clip_pos_d; 
    UINT8   u1clip_neg_d; 
    UINT8   u1shp_detail_gain_md; 
    UINT8   u1clip_ratio_v; 
    UINT8   u1clip_pos_v; 
    UINT8   u1clip_neg_v; 
    UINT8   u1shp_detail_gain_hv; 
    UINT8   u1shp_flat_step; 
    UINT8   u1shp_flat_th; 
    UINT8   u1shp_detail_step; 
    UINT8   u1shp_detail_th; 
    UINT8   u1shp_edge_step; 
    UINT8   u1shp_edge_th; 
    UINT8   u1tds_tap_mode; 
    UINT8   u1tds_tap_en; 
    UINT8   u1tds_tap_clip_en; 
    UINT8   u1tds_tap_limit; 
    UINT8   u1tds_tap_ratio; 
    UINT8   u1tds_tap_sratio; 
    UINT8   u1tds_vert_tap_apt_en; 
    UINT8   u1tds_vert_tap_apt_slope; 
    UINT8   u1tds_vert_tap_apt_edge; 
    UINT8   u1tds_vert_tap_apt_diff; 
    UINT8   u1tds_vert_tap_apt_back; 
    UINT8   u1tds_horiz_tap_apt_en; 
    UINT8   u1tds_horiz_tap_apt_slope; 
    UINT8   u1tds_horiz_tap_apt_edge; 
    UINT8   u1tds_horiz_tap_apt_diff; 
    UINT8   u1tds_horiz_tap_apt_back_0; 
    UINT8   u1tds_horiz_tap_apt_back_1; 
    UINT8   u1tds_horiz_tap_apt_back_2; 
    UINT8   u1tds_horiz_tap_apt_back_3; 
    UINT8   u1tds_horiz_tap_apt_back_h2; 
    UINT8   u1tds_horiz_tap_apt_edge_h2; 
} CHIP_TDSHARP_T; 


typedef struct { 
    UINT8   u1tds_blur; 
    UINT8   u1pbc1_en; 
    UINT8   u1pbc1_edge_en; 
    UINT8   u1pbc1_edge_slope; 
    UINT8   u1pbc1_lpf_en; 
    UINT8   u1pbc1_lpf_level; 
    UINT16  u2pbc1_radius_c1; 
    UINT16  u2pbc1_radius_c2; 
    UINT8   u1pbc1_d_radius; 
    UINT16  u2pbc1_theta_c1; 
    UINT16  u2pbc1_theta_c2; 
    UINT8   u1pbc1_d_theta; 
    UINT8   u1pbc1_coring_h1; 
    UINT8   u1pbc1_coring_h2; 
    UINT8   u1pbc1_coring_h3; 
    UINT8   u1pbc1_coring_d1; 
    UINT8   u1pbc1_coring_d2; 
    UINT8   u1pbc1_coring_d3; 
    UINT8   u1pbc1_coring_v1; 
    UINT8   u1pbc1_coring_v2; 
    UINT8   u1pbc1_coring_v3; 
    UINT8   u1pbc1_gain_h1; 
    UINT8   u1pbc1_gain_h2; 
    UINT8   u1pbc1_gain_h3; 
    UINT8   u1pbc1_gain_d1; 
    UINT8   u1pbc1_gain_d2; 
    UINT8   u1pbc1_gain_d3; 
    UINT8   u1pbc1_gain_v1; 
    UINT8   u1pbc1_gain_v2; 
    UINT8   u1pbc1_gain_v3; 
    UINT8   u1pbc1_limit_h1; 
    UINT8   u1pbc1_limit_h2; 
    UINT8   u1pbc1_limit_h3; 
    UINT8   u1pbc1_limit_d1; 
    UINT8   u1pbc1_limit_d2; 
    UINT8   u1pbc1_limit_d3; 
    UINT8   u1pbc1_limit_v1; 
    UINT8   u1pbc1_limit_v2; 
    UINT8   u1pbc1_limit_v3; 
    UINT8   u1pbc2_en; 
    UINT8   u1pbc2_edge_en; 
    UINT8   u1pbc2_edge_slope; 
    UINT8   u1pbc2_lpf_en; 
    UINT8   u1pbc2_lpf_level; 
    UINT16  u2pbc2_radius_c1; 
    UINT16  u2pbc2_radius_c2; 
    UINT8   u1pbc2_d_radius; 
    UINT16  u2pbc2_theta_c1; 
    UINT16  u2pbc2_theta_c2; 
    UINT8   u1pbc2_d_theta; 
    UINT8   u1pbc2_coring_h1; 
    UINT8   u1pbc2_coring_h2; 
    UINT8   u1pbc2_coring_h3; 
    UINT8   u1pbc2_coring_d1; 
    UINT8   u1pbc2_coring_d2; 
    UINT8   u1pbc2_coring_d3; 
    UINT8   u1pbc2_coring_v1; 
    UINT8   u1pbc2_coring_v2; 
    UINT8   u1pbc2_coring_v3; 
    UINT8   u1pbc2_gain_h1; 
    UINT8   u1pbc2_gain_h2; 
    UINT8   u1pbc2_gain_h3; 
    UINT8   u1pbc2_gain_d1; 
    UINT8   u1pbc2_gain_d2; 
    UINT8   u1pbc2_gain_d3; 
    UINT8   u1pbc2_gain_v1; 
    UINT8   u1pbc2_gain_v2; 
    UINT8   u1pbc2_gain_v3; 
    UINT8   u1pbc2_limit_h1; 
    UINT8   u1pbc2_limit_h2; 
    UINT8   u1pbc2_limit_h3; 
    UINT8   u1pbc2_limit_d1; 
    UINT8   u1pbc2_limit_d2; 
    UINT8   u1pbc2_limit_d3; 
    UINT8   u1pbc2_limit_v1; 
    UINT8   u1pbc2_limit_v2; 
    UINT8   u1pbc2_limit_v3; 
    UINT8   u1pbc3_en; 
    UINT8   u1pbc3_edge_en; 
    UINT8   u1pbc3_edge_slope; 
    UINT8   u1pbc3_lpf_en; 
    UINT8   u1pbc3_lpf_level; 
    UINT16  u2pbc3_radius_c1; 
    UINT16  u2pbc3_radius_c2; 
    UINT8   u1pbc3_d_radius; 
    UINT16  u2pbc3_theta_c1; 
    UINT16  u2pbc3_theta_c2; 
    UINT8   u1pbc3_d_theta; 
    UINT8   u1pbc3_coring_h1; 
    UINT8   u1pbc3_coring_h2; 
    UINT8   u1pbc3_coring_h3; 
    UINT8   u1pbc3_coring_d1; 
    UINT8   u1pbc3_coring_d2; 
    UINT8   u1pbc3_coring_d3; 
    UINT8   u1pbc3_coring_v1; 
    UINT8   u1pbc3_coring_v2; 
    UINT8   u1pbc3_coring_v3; 
    UINT8   u1pbc3_gain_h1; 
    UINT8   u1pbc3_gain_h2; 
    UINT8   u1pbc3_gain_h3; 
    UINT8   u1pbc3_gain_d1; 
    UINT8   u1pbc3_gain_d2; 
    UINT8   u1pbc3_gain_d3; 
    UINT8   u1pbc3_gain_v1; 
    UINT8   u1pbc3_gain_v2; 
    UINT8   u1pbc3_gain_v3; 
    UINT8   u1pbc3_limit_h1; 
    UINT8   u1pbc3_limit_h2; 
    UINT8   u1pbc3_limit_h3; 
    UINT8   u1pbc3_limit_d1; 
    UINT8   u1pbc3_limit_d2; 
    UINT8   u1pbc3_limit_d3; 
    UINT8   u1pbc3_limit_v1; 
    UINT8   u1pbc3_limit_v2; 
    UINT8   u1pbc3_limit_v3; 
} CHIP_PBC_T; 


typedef struct { 
    UINT8   u1flt_lti_h1; 
    UINT8   u1cor_zero_lti_h1; 
    UINT8   u1cor_ratio_lti_h1; 
    UINT8   u1cor_thr_lti_h1; 
    UINT8   u1gain_lti_h1; 
    UINT8   u1limit_lti_h1; 
    UINT8   u1limit_ratio_lti_h1; 
    UINT8   u1bound_lti_h1; 
    UINT8   u1clip_ratio_lti_h1; 
    UINT8   u1clip_pos_lti_h1; 
    UINT8   u1clip_neg_lti_h1; 
    UINT8   u1flt_lti_h2; 
    UINT8   u1cor_zero_lti_h2; 
    UINT8   u1cor_ratio_lti_h2; 
    UINT8   u1cor_thr_lti_h2; 
    UINT8   u1gain_lti_h2; 
    UINT8   u1limit_lti_h2; 
    UINT8   u1limit_ratio_lti_h2; 
    UINT8   u1bound_lti_h2; 
    UINT8   u1clip_ratio_lti_h2; 
    UINT8   u1clip_pos_lti_h2; 
    UINT8   u1clip_neg_lti_h2; 
    UINT8   u1flt_lti_v; 
    UINT8   u1cor_zero_lti_v; 
    UINT8   u1cor_ratio_lti_v; 
    UINT8   u1cor_thr_lti_v; 
    UINT8   u1gain_lti_v; 
    UINT8   u1limit_lti_v; 
    UINT8   u1limit_ratio_lti_v; 
    UINT8   u1bound_lti_v; 
    UINT8   u1clip_ratio_lti_v; 
    UINT8   u1clip_pos_lti_v; 
    UINT8   u1clip_neg_lti_v; 
} CHIP_LTI_T; 


typedef struct { 
    UINT8   u1tds_ylev_en; 
    UINT8   u1tds_ylev_sel; 
    UINT8   u1tds_ylev_alpha; 
    UINT8   u1ylev_apl_zero; 
    UINT8   u1ylev_apl_thr; 
    UINT8   u1ylev_p1; 
    UINT8   u1ylev_p2; 
    UINT8   u1ylev_p3; 
    UINT8   u1ylev_p4; 
    UINT8   u1ylev_g1; 
    UINT8   u1ylev_g2; 
    UINT8   u1ylev_g3; 
    UINT8   u1ylev_g4; 
} CHIP_YLEV_T; 


typedef struct { 
    UINT8   u1shp_combined_neg_gain; 
    UINT8   u1shp_combined_pos_gain; 
    UINT8   u1clip_ratio; 
    UINT8   u1clip_pos; 
    UINT8   u1sat_proc; 
    UINT8   u1ac_lpf_coe; 
} CHIP_TDSHARPALL_T; 


typedef struct { 
    UINT8   u1ena; 
    UINT8   u1hd; 
    UINT8   u1lpf1; 
    UINT8   u1lpf1_sel; 
    UINT8   u1flpf; 
    UINT8   u1flpf_sel; 
    UINT8   u1u_wnd_sz; 
    UINT8   u1v_wnd_sz; 
    UINT8   u1u_stb_gain; 
    UINT8   u1v_stb_gain; 
    UINT8   u1u_stb_ofst1; 
    UINT8   u1v_stb_ofst1; 
    UINT8   u1u_stb_ofst2; 
    UINT8   u1v_stb_ofst2; 
    UINT8   u1flat_gain; 
    UINT8   u1flat_ofst1; 
    UINT8   u1flat_ofst2; 
    UINT8   u1cor; 
} CHIP_CTIF_T; 


typedef struct { 
    UINT8   u1ena; 
    UINT8   u1hd; 
    UINT8   u1lpf1; 
    UINT8   u1lpf1_sel; 
    UINT8   u1flpf; 
    UINT8   u1flpf_sel; 
    UINT8   u1u_wnd_sz; 
    UINT8   u1v_wnd_sz; 
    UINT8   u1u_stb_gain; 
    UINT8   u1v_stb_gain; 
    UINT8   u1u_stb_ofst1; 
    UINT8   u1v_stb_ofst1; 
    UINT8   u1u_stb_ofst2; 
    UINT8   u1v_stb_ofst2; 
    UINT8   u1flat_gain; 
    UINT8   u1flat_ofst1; 
    UINT8   u1flat_ofst2; 
    UINT8   u1cor; 
} CHIP_CTIR_T; 



/**
*	CHIP_SHP_SYSTEM_T stucture.
*/
typedef struct {
    CHIP_AAF_T          stChipAaf;
    CHIP_CBE_T          stChipCbe;
    CHIP_HSHARP_T       stChipHSharp;
    CHIP_TDSHARP_T      stChipTDsharp;
    CHIP_PBC_T		    stChipPbc;
    CHIP_LTI_T          stChipTLI;
    CHIP_YLEV_T		    stChipYlev;
    CHIP_TDSHARPALL_T   stChipTDSharpAll;
    CHIP_CTIF_T         stChipCTIF;
    CHIP_CTIR_T         stChipCTIR;
}CHIP_SHP_T;



/*
*
*    HAL_VPQ_SetGamut(VIDEO_WID_T wId, UINT8 *pCgVal, void *pstData)
*    CHIP_XVYCC_T structure
*/
typedef struct { 
    UINT16  u2reg_matrix00; 
    UINT16  u2reg_matrix01; 
    UINT16  u2reg_matrix02; 
    UINT16  u2reg_matrix10; 
    UINT16  u2reg_matrix11; 
    UINT16  u2reg_matrix12; 
    UINT16  u2reg_matrix20; 
    UINT16  u2reg_matrix21; 
    UINT16  u2reg_matrix22; 
    UINT8   u1gamut_enable; 
    UINT8   u1load_matrix; 
} CHIP_XVYCC_T; 


/*
*
*    HAL_VPQ_SetEdgeEnhance(VIDEO_WID_T wId, UINT16 uEeVal, void *pstData)
*    CHIP_EDGEENHANCE_T structure
*/
typedef struct { 
    UINT8   u1shp_edge_gain_lh; 
    UINT8   u1shp_edge_gain_md; 
    UINT8   u1shp_edge_gain_hv; 
} CHIP_EDGEENHANCE_T; 






/*
*     HAL_VPQ_SetClearWhiteGainCtrl(UINT32 *pCwGain)
*	CHIP_ClearWhiteGain_T structure
*/
//modify temperally, for LG build fail
typedef struct {
	UINT8	u1bs_en_main;
	UINT8	u1bs_en_pip;
	UINT8	u1bs_y_thd;
	UINT8	u1bs_sat_thd;
	UINT8	u1bs_gain_b;
	UINT8	u1bs_gain_g;
	UINT8	u1bs_gain_r;
	UINT8	u1bs_base_1;
	UINT8	u1bs_gain_b_1;
	UINT8	u1bs_gain_g_1;
	UINT8	u1bs_gain_r_1;
} CHIP_ClearWhiteGain_T1;

typedef struct { 
    UINT8   u1bs_en_main; 
    UINT8   u1bs_y_thd; 
    UINT8   u1bs_sat_thd; 
    UINT8   u1bs_gain_b; 
    UINT8   u1bs_gain_g; 
    UINT8   u1bs_gain_r; 
    UINT8   u1bs_base_1; 
    UINT8   u1bs_gain_b_1; 
    UINT8   u1bs_gain_g_1; 
    UINT8   u1bs_gain_r_1; 
    UINT8   u1bs_base_2; 
    UINT8   u1bs_gain_b_2; 
    UINT8   u1bs_gain_g_2; 
    UINT8   u1bs_gain_r_2; 
    UINT8   u1bs_base_3; 
    UINT8   u1bs_gain_b_3; 
    UINT8   u1bs_gain_g_3; 
    UINT8   u1bs_gain_r_3; 
    UINT8   u1bs_base_4; 
    UINT8   u1bs_gain_b_4; 
    UINT8   u1bs_gain_g_4; 
    UINT8   u1bs_gain_r_4; 
} CHIP_ClearWhiteGain_T;
//modify temperally end, for LG build fail


/*
*     HAL_VPQ_Set3DColorCorrection(UINT32 *pCwGain)
*	CHIP_3D_COLOR_T structure
*/
typedef struct { 
    UINT8   u1cm_w1_en; 
    UINT16  u2cm_y_w1_l; 
    UINT16  u2cm_y_w1_point0; 
    UINT16  u2cm_y_w1_point1; 
    UINT16  u2cm_y_w1_point2; 
    UINT16  u2cm_y_w1_point3; 
    UINT16  u2cm_y_w1_point4; 
    UINT16  u2cm_y_w1_u; 
    UINT8   u1cm_y_w1_slope0; 
    UINT8   u1cm_y_w1_slope1; 
    UINT8   u1cm_y_w1_slope2; 
    UINT8   u1cm_y_w1_slope3; 
    UINT8   u1cm_y_w1_slope4; 
    UINT8   u1cm_y_w1_slope5; 
    UINT16  u2cm_y_w1_wgt_lslope; 
    UINT16  u2cm_y_w1_wgt_uslope; 
    UINT16  u2cm_s_w1_l; 
    UINT16  u2cm_s_w1_point0; 
    UINT16  u2cm_s_w1_point1; 
    UINT16  u2cm_s_w1_point2; 
    UINT16  u2cm_s_w1_point3; 
    UINT16  u2cm_s_w1_point4; 
    UINT16  u2cm_s_w1_u; 
    UINT8   u1cm_s_w1_slope0; 
    UINT8   u1cm_s_w1_slope1; 
    UINT8   u1cm_s_w1_slope2; 
    UINT8   u1cm_s_w1_slope3; 
    UINT8   u1cm_s_w1_slope4; 
    UINT8   u1cm_s_w1_slope5; 
    UINT16  u2cm_s_w1_wgt_lslope; 
    UINT16  u2cm_s_w1_wgt_uslope; 
    UINT16  u2cm_h_w1_l; 
    UINT16  u2cm_h_w1_point0; 
    UINT16  u2cm_h_w1_point1; 
    UINT16  u2cm_h_w1_point2; 
    UINT16  u2cm_h_w1_point3; 
    UINT16  u2cm_h_w1_point4; 
    UINT16  u2cm_h_w1_u; 
    UINT8   u1cm_h_w1_slope0; 
    UINT8   u1cm_h_w1_slope1; 
    UINT8   u1cm_h_w1_slope2; 
    UINT8   u1cm_h_w1_slope3; 
    UINT8   u1cm_h_w1_slope4; 
    UINT8   u1cm_h_w1_slope5; 
    UINT16  u2cm_h_w1_wgt_lslope; 
    UINT16  u2cm_h_w1_wgt_uslope; 
    UINT8   u1cm_w2_en; 
    UINT16  u2cm_y_w2_l; 
    UINT16  u2cm_y_w2_point0; 
    UINT16  u2cm_y_w2_point1; 
    UINT16  u2cm_y_w2_point2; 
    UINT16  u2cm_y_w2_point3; 
    UINT16  u2cm_y_w2_point4; 
    UINT16  u2cm_y_w2_u; 
    UINT8   u1cm_y_w2_slope0; 
    UINT8   u1cm_y_w2_slope1; 
    UINT8   u1cm_y_w2_slope2; 
    UINT8   u1cm_y_w2_slope3; 
    UINT8   u1cm_y_w2_slope4; 
    UINT8   u1cm_y_w2_slope5; 
    UINT16  u2cm_y_w2_wgt_lslope; 
    UINT16  u2cm_y_w2_wgt_uslope; 
    UINT16  u2cm_s_w2_l; 
    UINT16  u2cm_s_w2_point0; 
    UINT16  u2cm_s_w2_point1; 
    UINT16  u2cm_s_w2_point2; 
    UINT16  u2cm_s_w2_point3; 
    UINT16  u2cm_s_w2_point4; 
    UINT16  u2cm_s_w2_u; 
    UINT8   u1cm_s_w2_slope0; 
    UINT8   u1cm_s_w2_slope1; 
    UINT8   u1cm_s_w2_slope2; 
    UINT8   u1cm_s_w2_slope3; 
    UINT8   u1cm_s_w2_slope4; 
    UINT8   u1cm_s_w2_slope5; 
    UINT16  u2cm_s_w2_wgt_lslope; 
    UINT16  u2cm_s_w2_wgt_uslope; 
    UINT16  u2cm_h_w2_l; 
    UINT16  u2cm_h_w2_point0; 
    UINT16  u2cm_h_w2_point1; 
    UINT16  u2cm_h_w2_point2; 
    UINT16  u2cm_h_w2_point3; 
    UINT16  u2cm_h_w2_point4; 
    UINT16  u2cm_h_w2_u; 
    UINT8   u1cm_h_w2_slope0; 
    UINT8   u1cm_h_w2_slope1; 
    UINT8   u1cm_h_w2_slope2; 
    UINT8   u1cm_h_w2_slope3; 
    UINT8   u1cm_h_w2_slope4; 
    UINT8   u1cm_h_w2_slope5; 
    UINT16  u2cm_h_w2_wgt_lslope; 
    UINT16  u2cm_h_w2_wgt_uslope; 
    UINT8   u1cm_w3_en; 
    UINT16  u2cm_y_w3_l; 
    UINT16  u2cm_y_w3_point0; 
    UINT16  u2cm_y_w3_point1; 
    UINT16  u2cm_y_w3_point2; 
    UINT16  u2cm_y_w3_point3; 
    UINT16  u2cm_y_w3_point4; 
    UINT16  u2cm_y_w3_u; 
    UINT8   u1cm_y_w3_slope0; 
    UINT8   u1cm_y_w3_slope1; 
    UINT8   u1cm_y_w3_slope2; 
    UINT8   u1cm_y_w3_slope3; 
    UINT8   u1cm_y_w3_slope4; 
    UINT8   u1cm_y_w3_slope5; 
    UINT16  u2cm_y_w3_wgt_lslope; 
    UINT16  u2cm_y_w3_wgt_uslope; 
    UINT16  u2cm_s_w3_l; 
    UINT16  u2cm_s_w3_point0; 
    UINT16  u2cm_s_w3_point1; 
    UINT16  u2cm_s_w3_point2; 
    UINT16  u2cm_s_w3_point3; 
    UINT16  u2cm_s_w3_point4; 
    UINT16  u2cm_s_w3_u; 
    UINT8   u1cm_s_w3_slope0; 
    UINT8   u1cm_s_w3_slope1; 
    UINT8   u1cm_s_w3_slope2; 
    UINT8   u1cm_s_w3_slope3; 
    UINT8   u1cm_s_w3_slope4; 
    UINT8   u1cm_s_w3_slope5; 
    UINT16  u2cm_s_w3_wgt_lslope; 
    UINT16  u2cm_s_w3_wgt_uslope; 
    UINT16  u2cm_h_w3_l; 
    UINT16  u2cm_h_w3_point0; 
    UINT16  u2cm_h_w3_point1; 
    UINT16  u2cm_h_w3_point2; 
    UINT16  u2cm_h_w3_point3; 
    UINT16  u2cm_h_w3_point4; 
    UINT16  u2cm_h_w3_u; 
    UINT8   u1cm_h_w3_slope0; 
    UINT8   u1cm_h_w3_slope1; 
    UINT8   u1cm_h_w3_slope2; 
    UINT8   u1cm_h_w3_slope3; 
    UINT8   u1cm_h_w3_slope4; 
    UINT8   u1cm_h_w3_slope5; 
    UINT16  u2cm_h_w3_wgt_lslope; 
    UINT16  u2cm_h_w3_wgt_uslope; 
} CHIP_3D_COLOR_T; 




typedef struct{
    UINT8 uiColorEngineData[22][28];
    UINT8 uLCMP[344];
}CHIP_COLORENGINE_T;




/**
*	define Noise Reduction ( NR : DNR/ANR )
*/
typedef enum
{
	CHIP_NR_CTRL_NONE	= 0x00000000,
	CHIP_NR_CTRL_TNR	= 0x00000001,
	CHIP_NR_CTRL_DNR	= 0x00000002,
	CHIP_NR_CTRL_SNR	= 0x00000004,
} CHIP_NR_CTRL_TYPE_T; /* same with PQL_NR_CTRL_TYPE_T */



/**
*	define CHIP_TNR_T.
*/

typedef struct { 
    UINT8   u1nr_tbl_ma_0; 
    UINT8   u1nr_tbl_ma_1; 
    UINT8   u1nr_tbl_ma_2; 
    UINT8   u1nr_tbl_ma_3; 
    UINT8   u1nr_tbl_ma_4; 
    UINT8   u1nr_tbl_ma_5; 
    UINT8   u1nr_tbl_ma_6; 
    UINT8   u1nr_tbl_ma_7; 
    UINT8   u1nr_tbl_ma_e_0; 
    UINT8   u1nr_tbl_ma_e_1; 
    UINT8   u1nr_tbl_ma_e_2; 
    UINT8   u1nr_tbl_ma_e_3; 
    UINT8   u1nr_tbl_ma_e_4; 
    UINT8   u1nr_tbl_ma_e_5; 
    UINT8   u1nr_tbl_ma_e_6; 
    UINT8   u1nr_tbl_ma_e_7; 
    UINT8   u1nr_tbl_c_0; 
    UINT8   u1nr_tbl_c_1; 
    UINT8   u1nr_tbl_c_2; 
    UINT8   u1nr_tbl_c_3; 
    UINT8   u1nr_tbl_c_4; 
    UINT8   u1nr_tbl_c_5; 
    UINT8   u1nr_tbl_c_6; 
    UINT8   u1nr_tbl_c_7; 
    UINT8   u1monr_motion_level_th1; 
    UINT8   u1monr_motion_level_th2; 
    UINT8   u1motnr_min_nl_th; 
    UINT8   u1motnr_max_nl_th; 
    UINT8   u1tnr_thr_gain_st; 
    UINT8   u1tnr_thr_gain_mo; 
    UINT8   u1monr_apl_th1; 
    UINT8   u1monr_apl_th2; 
    UINT8   u1monr_apl_gain; 
    UINT8   u1mosnr_min_nl_th; 
    UINT8   u1mosnr_max_nl_th; 
    UINT8   u1mcnr_acdc_mode; 
    UINT8   u1mcnr_acnr_strength; 
    UINT8   u1mcnr_dcnr_strength; 
    UINT8   u1mcnr_dcnr_alpha; 
    UINT8   u1mcnr_dc_mode_diff_thd_y_base; 
    UINT8   u1mcnr_dc_mode_wei_y_base; 
    UINT8   u1mcnr_dc_mode_slop_y; 
    UINT8   u1mcnr_dc_mode_diff_thd_c_base; 
    UINT8   u1mcnr_dc_mode_wei_c_base; 
    UINT8   u1mcnr_dc_mode_slop_c; 
    UINT8   u1nr_iir_max_wei; 
    UINT8   u1nr_iir_max_wei_e; 
    UINT8   u1mcnr_dc_mode_max_wei; 
    UINT8   u1nr_movingtxr_sub_wei_base_dft; 
    UINT8   u1nr_movingtxr_sub_wei_base_hi; 
    UINT8   u1nr_movingtxr_sub_wei_nl_nag_gain_dft; 
    UINT8   u1nr_non_smooth_thd_base; 
    UINT8   u1nm_zoom_tex_non_smooth_thd_nl_gain_dft; 
    UINT8   u1tnr_ma_noise_add_wei_base; 
    UINT8   u1tnr_ma_noise_add_wei_nl_gain; 
    UINT8   u1tnr_ma_prtrbtn_add_wei_base; 
    UINT8   u1tnr_ma_prtrbtn_add_wei_still_gain; 
    UINT8   u1tnr_still_gmv_assist_de_prtrbtn; 
    UINT8   u1tnr_cs_noise_add_wei_base; 
    UINT8   u1tnr_cs_noise_add_wei_nl_gain; 
    UINT8   u1tnr_mc_noise_txtur_base; 
    UINT8   u1tnr_mc_noise_txtur_nl_gain; 
    UINT8   u1mcnr_chroma_movingtxr_sub_wei_max; 
    UINT8   u1mcnr_chroma_dc_add_wei_max; 
    UINT8   u1mcnr_chroma_dc_diff_bnd; 
} CHIP_TNR_T; 



/**
*	define CHIP_SNR_T.
*/
    typedef struct { 
        UINT8   u1nx_chroma_up_en; 
        UINT8   u1nx_rnr_snr_thd_ratio; 
        UINT8   u1nx_snr_gain_y; 
        UINT8   u1snr_adp_max; 
        UINT8   u1snr_adp_min; 
        UINT8   u1snr_thm_gain; 
        UINT16  u2snr_thm_ofst; 
        UINT8   u1bnr_rnr_th; 
        UINT8   u1rnr_max_th; 
        UINT8   u1rnr_con_th; 
        UINT8   u1rnr_agg_thm_gain; 
        UINT16  u2rnr_agg_thm_ofst; 
        UINT8   u1rnr_con_thm_gain; 
        UINT16  u2rnr_con_thm_ofst; 
        UINT8   u1nx_snr_cost_th_dir_adap; 
        UINT8   u1nx_snr_noise_cost_mode; 
        UINT8   u1nx_snr_noise_removal_auto_th_edge_str; 
        UINT8   u1nr_snr_auto_th_general_str; 
        UINT8   u1nr_snr_manual_auto_ratio; 
        UINT8   u1snr_thm_sel; 
        UINT8   u1nx_snr_gain_c; 
        UINT8   u1nx_snr_chroma_preservation; 
        UINT8   u1nx_snr_dz_white_lv; 
        UINT8   u1nx_snr_dz_black_lv; 
        UINT8   u1nx_snr_dz_transition; 
    } CHIP_SNR_T; 
    



/**
*     HAL_VPQ_SetNoiseReduction(VIDEO_WID_T wId, UINT16 *pNrVal, void *pstData)
*	CHIP_NR_T structure
*/
typedef struct CHIP_NR
{
    CHIP_TNR_T  stTnr;
	CHIP_SNR_T  stSnr;
} CHIP_NR_T;





    
/**
*    HAL_VPQ_SetMpegNoiseReduction(VIDEO_WID_T wId, UINT16 uMpegNrVal, void *pstData)
*   CHIP_MPEGNR_T structure
*/
typedef struct { 
    UINT8   u1nx_bnr_gain; 
    UINT8   u1nx_hblk_edge_proc_slope; 
    UINT8   u1nx_hblk_edge_proc_th; 
    UINT8   u1nx_vblk_edge_proc_slope; 
    UINT8   u1nx_vblk_edge_proc_th; 
    UINT8   u1nx_mnr_gain; 
    UINT8   u1nx_mnr_grouping_str; 
    UINT8   u1nx_mnr_str_gain; 
    UINT8   u1momnr_edge_th_mo; 
    UINT8   u1momnr_edge_th_st; 
    UINT8   u1nx_mnr_lere_gain; 
    UINT8   u1nx_mnr_lerf_gain; 
    UINT8   u1nx_mnr_lfre_gain; 
    UINT8   u1nx_mnr_uede_gain; 
    UINT8   u1nx_mnr_uedf_gain; 
    UINT8   u1nx_mnr_ufde_gain; 
} CHIP_MPEGNR_T; 


/**
*       HAL_VPQ_SetBlackLevel(VIDEO_WID_T wId, UINT8 *pBlVal, void *pstData)
*       define CHIP_BLACKLEVEL_T structure.
*/
typedef struct {
    UINT8   u1black_lvl_m;
    UINT8   u1black_lvl_p;
    UINT8   u1in_csc_black_lvl_p;
    UINT8   u1in_csc_black_lvl_m;
} CHIP_BLACKLEVEL_T;



/*
*   HAL_VPQ_SetColorGamut(VIDEO_WID_T wId, UINT8 *pCgVal)
*   xvYCC control factors
*/
typedef struct 
{
    UINT8 u1OnOff;          ///< xvYCC enable control
    UINT8 u1GammaOnOff;     ///< Gamma/anti-gamma control
    UINT16 u2Matrix[9];     ///< 3x3 gain matrix control
} CHIP_PQ_XVYCC_T;


/**
*       HAL_VPQ_SetColorFilter(UINT16 uCfVal)
*	define COLORFILTER.
*/
typedef struct {
    UINT8    u1mode_change_m;
    UINT8    u1mode_change_p;
    UINT16   u2RGain;
    UINT16   u2GGain;
    UINT16   u2BGain;
} CHIP_COLORFILTER_T;


/**
*       HAL_VPQ_SetColorGain(VIDEO_WID_T wId, SINT32 uCgVal)
*	define COLORGAIN.
*/
typedef struct {
    UINT8   u1cgain;
} CHIP_COLORGAIN_T;



/**
*	3x4 csc coefficients' matrix, chip dependant
*/
typedef struct CHIP_CSC_COEFF
{
	float fCs00;
	float fCs01;
	float fCs02;
	float fCs03;
	float fCs04;
	float fCs10;
	float fCs11;
	float fCs12;
	float fCs13;
	float fCs14;
	float fCs20;
	float fCs21;
	float fCs22;
	float fCs23;
	float fCs24;
} CHIP_CSC_COEFF_T;

/**
*	input csc info
*/
typedef struct CHIP_INPUT_CSC_INFO
{
	CHIP_CSC_COEFF_T *matrix;	///< matrix
	UINT32 hdmiDomain;			///< hdmi color domain
} CHIP_INPUT_CSC_INFO_T;

//H13_B0
typedef struct CHIP_TNR_CMN
{
	/* noise level gaining */
	UINT8 adjust_nt_lvl;		///< Adjust NT_Lvl enable
	UINT8 adjust_nt_lvl_val;	///< Adjust NT_Lvl value
	/* alpha gain */
	UINT8 y_gain;				///< Y_TNR_Gain (3.5u)
	UINT8 c_gain;				///< C_TNR_Gain (3.5u)
}CHIP_TNR_CMN_T;


/**
 *	pe temporal noise reduction detailed control parameter type
 *	motion -> noise level(n-lvl) estimate -> n-lvl gaining
 *         -> n-lvl apply -> alpha gaining
 *         -> motion filter -> n-lvl apply
 *         -> scene change -> alpha gaining
 */
typedef struct CHIP_TNR_DETAILMA
{
	/*ma: motion*/
	UINT8 m_coring_en_c;				///< gain accordint to result of ma, mc blending
	UINT8 m_coring_en_y;				///< motion coring enable y
	UINT8 m_gain_c; 					///< motion gain c: 2.2u
	UINT8 m_gain_y; 					///< max = 3.9999
	UINT8 m_coring_th;					///< m_coring_th
	UINT8 m_offset_mode_en; 			///< 0 := motion gain mode, 1 = motion offset mode
	UINT8 m_gain_en_var;				///< motion gain enable by variance
	UINT8 a_gain_en_var;				///< alpha gain enable by variance
	UINT8 wa_y0;						///< variance-adaptive motion/alpha gain : 0.8u
	UINT8 wa_y1;						///< variance-adaptive motion/alpha gain : 0.8u
	UINT8 wa_y2;						///< variance-adaptive motion/alpha gain : 0.8u
	UINT8 wa_y3;						///< variance-adaptive motion/alpha gain : 0.8u
	UINT8 wa_x0;						///< spatial variance threshold 0
	UINT8 wa_x1;						///< spatial variance threshold 1
	UINT8 wa_x2;						///< spatial variance threshold 2
	UINT8 wa_x3;						///< spatial variance threshold 3
	/*ma: alpha gain*/
	UINT8 luma_gain_p0_x;				///< luma_gain_p0_x
	UINT8 luma_gain_p1_x;				///< luma_gain_p1_x
	UINT8 luma_gain_p2_x;				///< luma_gain_p2_x
	UINT8 luma_gain_p3_x;				///< luma_gain_p3_x
	UINT8 luma_gain_p0_y;				///< luma_gain_p0_y
	UINT8 luma_gain_p1_y;				///< luma_gain_p1_y
	UINT8 luma_gain_p2_y;				///< luma_gain_p2_y
	UINT8 luma_gain_p3_y;				///< luma_gain_p3_y
	/*ma: alhpa remapping*/
	UINT8 alpha_avg_en; 				///< alpha spatial avg enable
	UINT8 alpha_mapping_y_en;			///< alpha mapping for y enable
	UINT8 alpha_mapping_c_en;			///< alpha mapping for c enable
	UINT8 alpha_mapping_y_x0;			///< alpha_mapping_y_x0
	UINT8 alpha_mapping_y_x1;			///< alpha_mapping_y_x1
	UINT8 alpha_mapping_y_x2;			///< alpha_mapping_y_x2
	UINT8 alpha_mapping_y_x3;			///< alpha_mapping_y_x3
	UINT8 alpha_mapping_y_y0;			///< alpha_mapping_y_y0
	UINT8 alpha_mapping_y_y1;			///< alpha_mapping_y_y1
	UINT8 alpha_mapping_y_y2;			///< alpha_mapping_y_y2
	UINT8 alpha_mapping_y_y3;			///< alpha_mapping_y_y3
	UINT8 alpha_mapping_c_x0;			///< alpha_mapping_c_x0
	UINT8 alpha_mapping_c_x1;			///< alpha_mapping_c_x1
	UINT8 alpha_mapping_c_x2;			///< alpha_mapping_c_x2
	UINT8 alpha_mapping_c_x3;			///< alpha_mapping_c_x3
	UINT8 alpha_mapping_c_y0;			///< alpha_mapping_c_y0
	UINT8 alpha_mapping_c_y1;			///< alpha_mapping_c_y1
	UINT8 alpha_mapping_c_y2;			///< alpha_mapping_c_y2
	UINT8 alpha_mapping_c_y3;			///< alpha_mapping_c_y3
	/*ma: noise level adjust gain*/
	UINT8 nt_lvl_adjust_gm_enable;		///< noise level adjust (by the global motion amount) enable
	UINT8 nt_lvl_adjust_lpct_enable;	///< noise level adjust (by the percent of smooth area) enable
	UINT8 nt_lvl_adjust_avg_ts_enable;	///< (by the (avg_t - avg_s))  enable, avg_t : frame average of temporal difference, avg_s : frame average of spatial variance
	UINT8 nt_lvl_adjust_lpct_sel;		///< 0 := the percent of smooth area in a picture
	UINT8 nt_lvl_gain_gm_x0;			///< nt_lvl_gain_gm_x0
	UINT8 nt_lvl_gain_gm_x1;			///< nt_lvl_gain_gm_x1
	UINT8 nt_lvl_gain_gm_x2;			///< nt_lvl_gain_gm_x2
	UINT8 nt_lvl_gain_gm_x3;			///< nt_lvl_gain_gm_x3
	UINT8 nt_lvl_gain_gm_y0;			///< nt_lvl_gain_gm_y0
	UINT8 nt_lvl_gain_gm_y1;			///< nt_lvl_gain_gm_y1
	UINT8 nt_lvl_gain_gm_y2;			///< nt_lvl_gain_gm_y2
	UINT8 nt_lvl_gain_gm_y3;			///< nt_lvl_gain_gm_y3
	UINT8 nt_lvl_gain_st_x0;			///< nt_lvl_gain_st_x0
	UINT8 nt_lvl_gain_st_x1;			///< nt_lvl_gain_st_x1
	UINT8 nt_lvl_gain_st_x2;			///< nt_lvl_gain_st_x2
	UINT8 nt_lvl_gain_st_x3;			///< nt_lvl_gain_st_x3
	UINT8 nt_lvl_gain_st_y0;			///< nt_lvl_gain_st_y0
	UINT8 nt_lvl_gain_st_y1;			///< nt_lvl_gain_st_y1
	UINT8 nt_lvl_gain_st_y2;			///< nt_lvl_gain_st_y2
	UINT8 nt_lvl_gain_st_y3;			///< nt_lvl_gain_st_y3
	UINT8 nt_lvl_gain_lpct_x0;			///< nt_lvl_gain_lpct_x0
	UINT8 nt_lvl_gain_lpct_x1;			///< nt_lvl_gain_lpct_x1
	UINT8 nt_lvl_gain_lpct_x2;			///< nt_lvl_gain_lpct_x2
	UINT8 nt_lvl_gain_lpct_x3;			///< nt_lvl_gain_lpct_x3
	UINT8 nt_lvl_gain_lpct_y0;			///< nt_lvl_gain_lpct_y0
	UINT8 nt_lvl_gain_lpct_y1;			///< nt_lvl_gain_lpct_y1
	UINT8 nt_lvl_gain_lpct_y2;			///< nt_lvl_gain_lpct_y2
	UINT8 nt_lvl_gain_lpct_y3;			///< nt_lvl_gain_lpct_y3
	UINT8 m_gain_en_var2;				///< motion gain enable by variance. this motion goes to motion-adaptive spatial blur
	UINT8 m_gain_ctrl2_x0;				///< m_gain_ctrl2_x0
	UINT8 m_gain_ctrl2_x1;				///< m_gain_ctrl2_x1
	UINT8 m_gain_ctrl2_y0;				///< m_gain_ctrl2_y0
	UINT8 m_gain_ctrl2_y1;				///< m_gain_ctrl2_y1
	UINT8 sf_map_flt_en;				///< spatial var. map filtering enable, spatial variance filtering, spatial variance adaptive
	UINT8 sf_map_tap;					///< "01" : 7-tap 7x3, "10" : 5-tap	5x3, "11" : 3-tap  3x3
	UINT8 sf_map_gain;					///< spatial var. map gain(3.3u)
	UINT8 sf_th0;						///< spatial var-to-filter gain
	UINT8 sf_th1;						///< lager than th0, smaller than th1, 3-tap recommand
	UINT8 sf_th2;						///< th2 5-tap
	UINT8 sf_th3;						///< th3 7-tap
	UINT8 sf_th4;						///< th4 9-tap
	UINT8 sf_mb_x0; 					///< x0 x motion
	UINT8 sf_mb_x1; 					///< sf_mb_x1
	UINT8 sf_mb_x2; 					///< sf_mb_x2
	UINT8 sf_mb_x3; 					///< sf_mb_x3
	UINT8 sf_mb_y0; 					///< y0  y gain : bigger gain cause blurer
	UINT8 sf_mb_y1; 					///< sf_mb_y1
	UINT8 sf_mb_y2; 					///< sf_mb_y2
	UINT8 sf_mb_y3; 					///< sf_mb_y3
	UINT8 sf_gain_mode; 				///< 0' : disable gain control, '1' : enable gain control, master gain
	UINT8 sf_gain;						///< sf gain(0.4u)
	UINT8 sf_blend_en_y;				///< motion-adaptive blending enable of blurred data and bypass data: y
	UINT8 sf_blend_en_c;				///< motion-adaptive blending enable of blurred data and bypass data: c
	UINT8 sf_blend_motion_exp_mode; 	///< 0:= no expand, 1 = 11-tap expand, 2 = 9-tap expand, 3 = 5-tap expand
}
CHIP_TNR_DETAILMA_T;

typedef struct { 
    UINT8   u1w1_sat_upper; 
    UINT8   u1w1_sat_lower; 
    UINT8   u1w1_hue_upper; 
    UINT8   u1w1_hue_lower; 
} CHIP_TWO_D_WINDOW1_T;

typedef struct CHIP_TNR_DETAILMC
{
	/*mc*/
	UINT8 sad_sel_pels; 				///< 0 := sad_ma, 1 = alpha_blend(sad_mc, sad_ma)
	UINT8 sad_mamc_blend;				///< 0xff -> sad of mc, 0x00 -> sad of ma
	UINT8 mamc_blend_sel;				///< 0:= spatial variance, 1 = motion vector consistency
	UINT8 mamc_blend_x0;				///< spatial variance threshold
	UINT8 mamc_blend_x1;				///< mamc_blend_x1
	UINT8 mamc_blend_y0;				///< 0xff: mc filter, 0x00: ma filter
	UINT8 mamc_blend_y1;				///< mamc_blend_y1
}
CHIP_TNR_DETAILMC_T;

typedef struct CHIP_TNR_DETAILME
{
	/*me*/
	UINT8 hme_half_pel_en;				///< half pel search enable for motion estimation
	UINT8 mv_cost_smooth_gain;			///< 0.5u
	UINT8 mv_cost_smooth_en;			///< enable for temporal smoothness constraint for block matching cost
	UINT8 mv_cost_gmv_smooth_gain;		///< mv_cost_gmv_smooth_gain
	UINT8 mv_cost_gmv_smooth_en;		///< mv_cost_gmv_smooth_en
	UINT8 mv0_protect_th;				///< sad threshold for zero-motion vector protection
	UINT8 mv0_protect_en;				///< zero-motion vector protection enable
	UINT8 sad_protect_en;				///< sad-based zero-motion vector protection enable
	UINT8 sad_protect_gm_en;			///< zero-motion vector protection enable by global motion
	UINT8 mv_protect_control_x0;		///< variance threshold for 0-motion vector protection
	UINT8 mv_protect_control_x1;		///< mv_protect_control_x1
	UINT8 mv_protect_control_y0;		///< sad threshold for 0-motion vector protection
	UINT8 mv_protect_control_y1;		///< mv_protect_control_y1

}
CHIP_TNR_DETAILME_T;

/**
 * CHIP Tnr main control parameter type
 *	for H13BX,M14AX
 */
typedef struct
{
	CHIP_TNR_CMN_T			main;		///< main ctrl
	CHIP_TNR_DETAILMA_T		detail_a;	///< detail adaptive ctrl
	CHIP_TNR_DETAILMC_T		detail_c;	///< detail compensation ctrl
	CHIP_TNR_DETAILME_T		detail_e;	///< detail estimation ctrl
}
CHIP_TNR_MAIN_B0_T;

/**
 *	CHIP Tnr control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	UINT8 tnr_master_gain;				///< master TNR gain, 0~255
}
CHIP_TNR_CMN01_T;

/**
 *	CHIP Tnr detailed control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	/*ma 26*/
	UINT8 reg_g_blend_a;				///< p0m_tnr_ctrl_25, 31:24, reg_g_blend_a, , set to fw
	UINT8 motion_sc_reset_en;			///< p0m_tnr_ctrl_31,     2, motion_sc_reset_en
	UINT8 luma_gain_ctrl_x0;			///< p0m_tnr_ctrl_33, 31:24, luma_gain_ctrl
	UINT8 luma_gain_ctrl_x1;			///< p0m_tnr_ctrl_33, 23:16, luma_gain_ctrl
	UINT8 luma_gain_ctrl_x2;			///< p0m_tnr_ctrl_33, 15: 8, luma_gain_ctrl
	UINT8 luma_gain_ctrl_x3;			///< p0m_tnr_ctrl_33,  7: 0, luma_gain_ctrl
	UINT8 luma_gain_ctrl_y0;			///< p0m_tnr_ctrl_34, 31:24, luma_gain_ctrl
	UINT8 luma_gain_ctrl_y1;			///< p0m_tnr_ctrl_34, 23:16, luma_gain_ctrl
	UINT8 luma_gain_ctrl_y2;			///< p0m_tnr_ctrl_34, 15: 8, luma_gain_ctrl
	UINT8 luma_gain_ctrl_y3;			///< p0m_tnr_ctrl_34,  7: 0, luma_gain_ctrl
	UINT8 skingain_motioncontrol_x0;	///< p0m_tnr_ctrl_32, 15: 8, skingain_motioncontrol
	UINT8 skingain_motioncontrol_x1;	///< p0m_tnr_ctrl_32, 31:24, skingain_motioncontrol
	UINT8 skingain_motioncontrol_y0;	///< p0m_tnr_ctrl_32,  7: 3, skingain_motioncontrol
	UINT8 skingain_motioncontrol_y1;	///< p0m_tnr_ctrl_32, 23:19, skingain_motioncontrol
	UINT8 skin_lut_y0;					///< p0m_tnr_ctrl_35,  7: 0, skin_lut
	UINT8 skin_lut_y1;					///< p0m_tnr_ctrl_35, 15: 8, skin_lut
	UINT8 skin_lut_y2;					///< p0m_tnr_ctrl_35, 23:16, skin_lut
	UINT8 skin_lut_y3;					///< p0m_tnr_ctrl_35, 31:24, skin_lut
	UINT8 skin_lut_cb0;					///< p0m_tnr_ctrl_36,  7: 0, skin_lut
	UINT8 skin_lut_cb1;					///< p0m_tnr_ctrl_36, 15: 8, skin_lut
	UINT8 skin_lut_cb2;					///< p0m_tnr_ctrl_36, 23:16, skin_lut
	UINT8 skin_lut_cb3;					///< p0m_tnr_ctrl_36, 31:24, skin_lut
	UINT8 skin_lut_cr0;					///< p0m_tnr_ctrl_37,  7: 0, skin_lut
	UINT8 skin_lut_cr1;					///< p0m_tnr_ctrl_37, 15: 8, skin_lut
	UINT8 skin_lut_cr2;					///< p0m_tnr_ctrl_37, 23:16, skin_lut
	UINT8 skin_lut_cr3;					///< p0m_tnr_ctrl_37, 31:24, skin_lut
	/*me 22*/
	UINT8 past_refine_ratio;			///< p0m_tnr_ctrl_07,  7: 4, past_refine_ratio, , set to fw
	UINT8 global_past_refine_ratio;		///< p0m_tnr_ctrl_07, 11: 8, global_past_refine_ratio, , set to fw
	UINT8 mv_refine_ratio;				///< p0m_tnr_ctrl_08, 27:24, spatial vector refine, , set to fw
	UINT8 temporal_refine_adj1;			///< p0m_tnr_ctrl_09,  2: 0, temporal refine vector adjustment
	UINT8 temporal_refine_adj2;			///< p0m_tnr_ctrl_09,  5: 3, temporal refine vector adjustment
	UINT8 zero_refine_ratio;			///< p0m_tnr_ctrl_09, 30:27, zero vector refine
	UINT8 me_conf_scale_ma;				///< p0m_tnr_ctrl_12,  2: 0, me_conf_scale_ma
	UINT8 me_conf_scale_mc;				///< p0m_tnr_ctrl_12,  6: 4, me_conf_scale_mc
	UINT8 past_x0;						///< p0m_tnr_ctrl_39,  7: 0, ME auto temporal refine ratio setting
	UINT8 past_x1;						///< p0m_tnr_ctrl_39, 15: 8, ME auto temporal refine ratio setting
	UINT8 past_x2;						///< p0m_tnr_ctrl_39, 23:16, ME auto temporal refine ratio setting
	UINT8 past_y0;						///< p0m_tnr_ctrl_39, 27:24, ME auto temporal refine ratio setting
	UINT8 past_y1;						///< p0m_tnr_ctrl_39, 31:28, ME auto temporal refine ratio setting
	UINT8 past_y2;						///< p0m_tnr_ctrl_38,  3: 0, ME auto temporal refine ratio setting
	UINT8 past_y3;						///< p0m_tnr_ctrl_38,  7: 4, ME auto temporal refine ratio setting
	UINT8 global_x0;					///< p0m_tnr_ctrl_04,  7: 0, ME auto global refine ratio setting
	UINT8 global_x1;					///< p0m_tnr_ctrl_04, 15: 8, ME auto global refine ratio setting
	UINT8 global_x2;					///< p0m_tnr_ctrl_04, 23:16, ME auto global refine ratio setting
	UINT8 global_y0;					///< p0m_tnr_ctrl_04, 27:24, ME auto global refine ratio setting
	UINT8 global_y1;					///< p0m_tnr_ctrl_04, 31:28, ME auto global refine ratio setting
	UINT8 global_y2;					///< p0m_tnr_ctrl_05,  3: 0, ME auto global refine ratio setting
	UINT8 global_y3;					///< p0m_tnr_ctrl_05,  7: 4, ME auto global refine ratio setting
	/*mc 5*/
	UINT8 am_th_mode;					///< p0m_tnr_ctrl_16,     4, A-mean th mode
	UINT8 am_th_val;					///< p0m_tnr_ctrl_16, 15: 8, A-mean th value(manaul)
	UINT8 mcblnd_th_x0;					///< p0m_tnr_ctrl_19,  7: 0, alpha(normal) +alpha(strong) blending
	UINT8 mcblnd_th_x1;					///< p0m_tnr_ctrl_19, 15: 8, alpha(normal) +alpha(strong) blending
	UINT8 sc_alpha_mode;				///< p0m_tnr_ctrl_20, 23:22, alpha in case of scene-change
	/*ma post 4*/
	UINT8 reg_ma_conf_x0;				///< p0m_tnr_ctrl_43,  7: 3, reg_ma_conf
	UINT8 reg_ma_conf_x1;				///< p0m_tnr_ctrl_43, 15:11, reg_ma_conf
	UINT8 reg_ma_conf_y0;				///< p0m_tnr_ctrl_43, 23:16, reg_ma_conf
	UINT8 reg_ma_conf_y1;				///< p0m_tnr_ctrl_43, 31:24, reg_ma_conf
	/*mc post 4*/
	UINT8 reg_mc_conf_x0;				///< p0m_tnr_ctrl_42, 31:24, reg_mc_conf
	UINT8 reg_mc_conf_x1;				///< p0m_tnr_ctrl_42, 23:16, reg_mc_conf
	UINT8 reg_mc_conf_y0;				///< p0m_tnr_ctrl_42, 15:11, reg_mc_conf
	UINT8 reg_mc_conf_y1;				///< p0m_tnr_ctrl_42,  7: 3, reg_mc_conf
	/*snr 7*/
	UINT8 reg_snr_master_gain;			///< p0m_tnr_ctrl_48,  7: 0, reg_snr_master_gain
	UINT8 reg_snr_c_blur_gain;			///< p0m_tnr_ctrl_48, 23:16, reg_snr_c_blur_gain, , set to fw
	UINT8 reg_snr_y_blur_gain;			///< p0m_tnr_ctrl_48, 31:24, reg_snr_y_blur_gain, , set to fw
	UINT8 snr_blendgain_x0;				///< p0m_tnr_ctrl_50, 23:16, snr_blendgain
	UINT8 snr_blendgain_x1;				///< p0m_tnr_ctrl_50, 31:24, snr_blendgain
	UINT8 snr_blendgain_y0;				///< p0m_tnr_ctrl_50,  7: 0, snr_blendgain
	UINT8 snr_blendgain_y1;				///< p0m_tnr_ctrl_50, 15: 8, snr_blendgain
}
CHIP_TNR_DETAIL01_T;

/**
 * CHIP Tnr main control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	CHIP_TNR_CMN01_T		common;		///< common ctrl
	CHIP_TNR_DETAIL01_T		detail;		///< detail ctrl
}
CHIP_TNR_MAIN01_T;

typedef struct CHIP_DNR_CMN
{
	/* dnr main */
	UINT8 enable_ac_bnr;				///< enable_ac_bnr
	UINT8 enable_mnr;					///< enable_mnr
	UINT8 enable_dc_bnr;				///< enable_dc_bnr
	/* mnr */
	UINT8 reg_mnr_enable;				///< reg_mnr_enable
	UINT8 reg_mnr_master_gain;			///< reg_mnr_master_gain
	UINT8 reg_chroma_master_gain;		///< reg_chroma_master_gain
	UINT8 reg_mnr_h_gain;				///< reg_mnr_h_gain
	UINT8 reg_mnr_v_gain;				///< reg_mnr_v_gain
	/* ac_bnr */
	UINT8 bnr_h_en;						///< bnr_h_en
	UINT8 bnr_v_en;						///< bnr_v_en
	UINT8 ac_bnr_protect_motion_max;	///< ac_bnr_protect_motion_max
	UINT8 ac_bnr_protect_motion_min;	///< ac_bnr_protect_motion_min
	UINT8 hbmax_gain;					///< gain for maximum h-blockiness (0~7) d:4, (hmax * hbmaxgain) - hothers = hb
	UINT8 vbmax_gain;					///< gain for maximum v-blockiness (0~7) d:4, (vmax * vbmaxgain) - vothers = vb
	UINT8 strength_h_max;				///< strength_h_max
	UINT8 strength_v_max;				///< strength_v_max
	/* dc_bnr */
	UINT8 reg_dc_bnr_enable;			///< reg_dc_bnr_enable
	UINT8 reg_dc_bnr_mastergain;		///< reg_dc_bnr_mastergain
	UINT16 reg_dc_bnr_chromagain;		///< reg_dc_bnr_chromagain
}CHIP_DNR_CMN_B0_T;

typedef struct CHIP_DNR_DETAIL
{
	/* mnr */
	UINT8 reg_h_expend;					///< reg_h_expend
	UINT8 reg_gain_flt_size;			///< reg_gain_flt_size
	UINT8 reg_mnr_s1_mmd_min;			///< reg_mnr_s1_mmd_min
	UINT8 reg_mnr_s2_ratio_min;			///< reg_mnr_s2_ratio_min
	UINT8 reg_mnr_s2_ratio_max;			///< reg_mnr_s2_ratio_max
	UINT8 reg_mnr_s2_mmd_min;			///< reg_mnr_s2_mmd_min
	UINT8 reg_filter_x0;				///< reg_filter_x0
	UINT8 reg_filter_x1;				///< reg_filter_x1
	UINT8 reg_filter_y0;				///< reg_filter_y0
	UINT8 reg_filter_y1;				///< reg_filter_y1
	UINT8 reg_motion_mnr_en;			///< reg_motion_mnr_en
	UINT8 reg_motion_mnr_filter;		///< reg_motion_mnr_filter
	UINT8 reg_mnr_motion_min;			///< reg_mnr_motion_min
	UINT8 reg_mnr_motion_max;			///< reg_mnr_motion_max
	UINT8 reg_motion_mnr_x0;			///< reg_motion_mnr_x0
	UINT8 reg_motion_mnr_x1;			///< reg_motion_mnr_x1
	UINT8 reg_motion_mnr_y0;			///< reg_motion_mnr_y0
	UINT8 reg_motion_mnr_y1;			///< reg_motion_mnr_y1
	/* ac_bnr */
	UINT8 motion_protect_enable;		///< motion_protect_enable
	UINT8 source_type;					///< 0 := sd (9 lm), 1 = hd (5 lm)
	UINT8 fiter_type;					///< 0  :=  normal 3-tap avg. 1 1 1, 1  =  center-weighted 3-tap avg : 1 2 1
	UINT8 strength_h_x0;				///< ( hb - offset ) * gain => graph
	UINT8 strength_h_x1;				///< strength_h_x1
	UINT8 detect_min_th;				///< minimun threshold of block line detection
	UINT8 strength_v_x0;				///< ( vb - offset ) * gain => graph
	UINT8 strength_v_x1;				///< strength_v_x1
	/* dc_bnr */
	UINT8 reg_dc_bnr_var_en;			///< reg_dc_bnr_var_en
	UINT8 reg_dc_bnr_motion_en;			///< reg_dc_bnr_motion_en
	UINT8 reg_dc_bnr_acadaptive;		///< reg_dc_bnr_acadaptive
	UINT8 reg_dc_bnr_sdhd_sel;			///< reg_dc_bnr_sdhd_sel
	UINT8 reg_dc_blur_sel;				///< reg_dc_blur_sel
	UINT8 reg_dc_protection_en;			///< reg_dc_protection_en
	UINT8 reg_dc_bnr_var_th0;			///< reg_dc_bnr_var_th0
	UINT8 reg_dc_bnr_var_th1;			///< reg_dc_bnr_var_th1
	UINT8 reg_dc_bnr_var_th2;			///< reg_dc_bnr_var_th2
	UINT8 reg_dc_bnr_var_th3;			///< reg_dc_bnr_var_th3
	UINT8 reg_dc_bnr_motion_th0;		///< reg_dc_bnr_motion_th0
	UINT8 reg_dc_bnr_motion_th1;		///< reg_dc_bnr_motion_th1
	UINT8 reg_dc_bnr_motion_th2;		///< reg_dc_bnr_motion_th2
	UINT8 reg_dc_bnr_motion_th3;		///< reg_dc_bnr_motion_th3
	UINT8 reg_dc_chroma_variance;		///< reg_dc_chroma_variance
	UINT8 reg_dc_var_h_gain;			///< reg_dc_var_h_gain
	UINT8 reg_dc_protection_th;			///< reg_dc_protection_th
}CHIP_DNR_DETAIL_B0_T;

/**
 * CHIP Dnr main control parameter type
 *	for H13BX,M14AX
 */
typedef struct
{
	CHIP_DNR_CMN_B0_T		main;		///< main ctrl
	CHIP_DNR_DETAIL_B0_T	detail;		///< detail ctrl
}
CHIP_DNR_MAIN_B0_T;

/**
 *	CHIP Dnr common control parameter type
 *	ver.01, for H15Ax, M14Bx
 */
typedef struct
{
	/* dnr max ctrl 1 */
	UINT8 reg_dnr_max_enable;			///< dnr_max_ctrl, 0, dnr top enable
	/* dc bnr 3 */
	UINT8 reg_dc_bnr_enable;			///< dc_bnr_ctrl_0, 0, DC BNR enable,0:= off (output debug mode),1  = on
	UINT8 reg_dc_bnr_mastergain;		///< dc_bnr_ctrl_2, 15:10, mastergain
	UINT8 reg_dc_bnr_chromagain;		///< dc_bnr_ctrl_2, 23:16, chromagain
	/* ac bnr 7 */
	UINT8 reg_bnr_ac_h_en;				///< ac_bnr_ctrl_0, 0, ac bnr h en
	UINT8 reg_bnr_ac_v_en;				///< ac_bnr_ctrl_0, 1, ac bnr v en
	UINT8 reg_bnr_ac_h_chroma_en;		///< ac_bnr_ctrl_0, 2, ac bnr h ch en
	UINT8 reg_bnr_ac_v_chroma_en;		///< ac_bnr_ctrl_0, 3, ac bnr v ch en
	UINT8 reg_bnr_ac_acness_resol_h;	///< ac_bnr_ctrl_0,  5: 4, 0x0 is original, 1: /2, 2: /4, 3:/ 8
	UINT8 reg_bnr_ac_acness_resol_v;	///< ac_bnr_ctrl_4,  7: 6, 0x0 is original, 1: /2, 2: /4, 3:/ 8
	UINT8 reg_ac_master_gain;			///< ac_bnr_ctrl_9, 15:10, ac_master_gain
	/* mnr 5 */
	UINT8 reg_mnr_enable;				///< mnr_ctrl_0, 0, mnr_enable
	UINT8 reg_mnr_master_gain;			///< mnr_ctrl_0, 15: 8, mnr_master_gain
	UINT8 reg_chroma_master_gain;		///< mnr_ctrl_0, 23:16, chroma_master_gain
	UINT8 reg_mnr_h_gain;				///< mnr_ctrl_2, 31:24, mnr_h_gain
	UINT8 reg_mnr_v_gain;				///< mnr_ctrl_2, 23:16, mnr_v_gain
}
CHIP_DNR_CMN01_T;

/**
 *	CHIP Dnr detailed control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	/* dc bnr 18 */
	UINT8 reg_dc_var_en;				///< dc_bnr_ctrl_2, 0, dc_var_en
	UINT8 reg_dc_motion_en;				///< dc_bnr_ctrl_2, 1, dc_motion_en
	UINT8 reg_dc_protection_en;			///< dc_bnr_ctrl_2, 2, dc_protection_en
	UINT8 reg_dc_detail_en;				///< dc_bnr_ctrl_2, 3, dc_detail_en
	UINT8 reg_dc_blur_sel;				///< dc_bnr_ctrl_0, 2, dc_blur_sel
	UINT8 reg_dc_motion_max;			///< dc_bnr_ctrl_0, 23:16, dc_motion_max
	UINT8 reg_dc_motion_min;			///< dc_bnr_ctrl_0, 31:24, dc_motion_min
	UINT8 reg_dc_detail_max;			///< dc_bnr_ctrl_1,  7: 0, dc_detail_max
	UINT8 reg_dc_detail_min;			///< dc_bnr_ctrl_1, 15: 8, dc_detail_min
	UINT8 reg_var_v_gain;				///< dc_bnr_ctrl_1, 23:20, var_v_gain
	UINT8 reg_var_h_gain;				///< dc_bnr_ctrl_1, 27:24, var_h_gain
	UINT8 reg_var_cut_resolution;		///< dc_bnr_ctrl_1, 31:28, var_cut_resolution
	UINT8 reg_dc_global_motion_th;		///< dc_bnr_ctrl_2,  7: 4, dc_global_motion_th
	UINT8 reg_dc_protection_th;			///< dc_bnr_ctrl_2, 31:24, dc_protection_th
	UINT8 reg_dc_bnr_var_th0;			///< dc_bnr_ctrl_3, 31:24, dc_bnr_var_th0
	UINT8 reg_dc_bnr_var_th1;			///< dc_bnr_ctrl_3, 23:16, dc_bnr_var_th1
	UINT8 reg_dc_bnr_var_th2;			///< dc_bnr_ctrl_3, 15: 8, dc_bnr_var_th2
	UINT8 reg_dc_bnr_var_th3;			///< dc_bnr_ctrl_3,  7: 0, dc_bnr_var_th3
	/* ac bnr 23 */
	UINT8 reg_bnr_ac_diff_min_v_th;		///< ac_bnr_ctrl_0, 15: 8, bnr_ac_diff_min_v_th
	UINT8 reg_bnr_ac_diff_min_h_th;		///< ac_bnr_ctrl_0, 23:16, bnr_ac_diff_min_h_th
	UINT8 reg_bnr_ac_global_motion_th;	///< ac_bnr_ctrl_1,  7: 0, bnr_ac_global_motion_th
	UINT8 reg_bnr_ac_acness_max;		///< ac_bnr_ctrl_1, 15: 8, bnr_ac_acness_max
	UINT8 reg_bnr_ac_acness_min;		///< ac_bnr_ctrl_1, 23:16, bnr_ac_acness_min
	UINT8 reg_bnr_ac_motion_max;		///< ac_bnr_ctrl_4, 23:16, bnr_ac_motion_max
	UINT8 reg_bnr_ac_motion_min;		///< ac_bnr_ctrl_4, 31:24, bnr_ac_motion_min
	UINT8 reg_bnr_ac_detail_th1;		///< ac_bnr_ctrl_5, 31:24, bnr_ac_detail_th1
	UINT8 reg_bnr_ac_detail_th2;		///< ac_bnr_ctrl_5, 23:16, bnr_ac_detail_th2
	UINT8 reg_bnr_ac_detail_th3;		///< ac_bnr_ctrl_5, 15: 8, bnr_ac_detail_th3
	UINT8 reg_bnr_ac_detail_th4;		///< ac_bnr_ctrl_5,  7: 0, bnr_ac_detail_th4
	UINT8 reg_bnr_ac_pos_gain_h0;		///< ac_bnr_ctrl_6, 23:16, bnr_ac_pos_gain_h0
	UINT8 reg_bnr_ac_pos_gain_h1;		///< ac_bnr_ctrl_6, 15: 8, bnr_ac_pos_gain_h1
	UINT8 reg_bnr_ac_pos_gain_h2;		///< ac_bnr_ctrl_6,  7: 0, bnr_ac_pos_gain_h2
	UINT8 reg_bnr_ac_pos_gain_h3;		///< ac_bnr_ctrl_7, 31:24, bnr_ac_pos_gain_h3
	UINT8 reg_bnr_ac_pos_gain_l0;		///< ac_bnr_ctrl_7, 23:16, bnr_ac_pos_gain_l0
	UINT8 reg_bnr_ac_pos_gain_l1;		///< ac_bnr_ctrl_7, 15: 8, bnr_ac_pos_gain_l1
	UINT8 reg_bnr_ac_pos_gain_l2;		///< ac_bnr_ctrl_7,  7: 0, bnr_ac_pos_gain_l2
	UINT8 reg_bnr_ac_pos_gain_l3;		///< ac_bnr_ctrl_8, 31:24, bnr_ac_pos_gain_l3
	UINT8 reg_bnr_ac_detail_max;		///< detail_ctrl,  7: 0, bnr_ac_detail_max
	UINT8 reg_bnr_ac_detail_min;		///< detail_ctrl, 15: 8, bnr_ac_detail_min
	UINT8 reg_bnr_diff_l;				///< detail_ctrl, 23:16, bnr_diff_l
	UINT8 reg_bnr_diff_p;				///< detail_ctrl, 31:24, bnr_diff_p
	/* mnr 18 */
	UINT8 reg_h_expend;					///< mnr_ctrl_0, 4, h_expend
	UINT8 reg_gain_flt_size;			///< mnr_ctrl_0, 5, gain_flt_size
	UINT8 reg_mnr_s1_mmd_min;			///< mnr_ctrl_1,  7: 0, mnr_s1_mmd_min
	UINT8 reg_mnr_s2_ratio_min;			///< mnr_ctrl_1, 15: 8, mnr_s2_ratio_min
	UINT8 reg_mnr_s2_ratio_max;			///< mnr_ctrl_1, 23:16, mnr_s2_ratio_max
	UINT8 reg_mnr_s2_mmd_min;			///< mnr_ctrl_1, 31:24, mnr_s2_mmd_min
	UINT8 reg_filter_x0;				///< mnr_ctrl_3,  7: 0, filter_x0
	UINT8 reg_filter_x1;				///< mnr_ctrl_3, 15: 8, filter_x1
	UINT8 reg_filter_y0;				///< mnr_ctrl_3, 23:16, filter_y0
	UINT8 reg_filter_y1;				///< mnr_ctrl_3, 31:24, filter_y1
	UINT8 reg_motion_mnr_en;			///< mnr_ctrl_4, 0, motion_mnr_en
	UINT8 reg_motion_mnr_filter;		///< mnr_ctrl_4, 1, motion_mnr_filter
	UINT8 reg_mnr_motion_min;			///< mnr_ctrl_4, 23:16, mnr_motion_min
	UINT8 reg_mnr_motion_max;			///< mnr_ctrl_4, 31:24, mnr_motion_max
	UINT8 reg_motion_mnr_x0;			///< mnr_ctrl_5,  7: 0, motion_mnr_x0
	UINT8 reg_motion_mnr_x1;			///< mnr_ctrl_5, 15: 8, motion_mnr_x1
	UINT8 reg_motion_mnr_y0;			///< mnr_ctrl_5, 23:16, motion_mnr_y0
	UINT8 reg_motion_mnr_y1;			///< mnr_ctrl_5, 31:24, motion_mnr_y1
}
CHIP_DNR_DETAIL01_T;

/**
 *	CHIP Dnr detailed control parameter type
 *	ver.02, for H15Ax
 */
typedef struct
{
	/* dc bnr 18 */
	UINT8 reg_dc_var_en;				///< dc_bnr_ctrl_2, 0, dc_var_en
	UINT8 reg_dc_motion_en;				///< dc_bnr_ctrl_2, 1, dc_motion_en
	UINT8 reg_dc_protection_en;			///< dc_bnr_ctrl_2, 2, dc_protection_en
	UINT8 reg_dc_detail_en;				///< dc_bnr_ctrl_2, 3, dc_detail_en
	UINT8 reg_dc_blur_sel;				///< dc_bnr_ctrl_0, 2, dc_blur_sel
	UINT8 reg_dc_motion_max;			///< dc_bnr_ctrl_0, 23:16, dc_motion_max
	UINT8 reg_dc_motion_min;			///< dc_bnr_ctrl_0, 31:24, dc_motion_min
	UINT8 reg_dc_detail_max;			///< dc_bnr_ctrl_1,  7: 0, dc_detail_max
	UINT8 reg_dc_detail_min;			///< dc_bnr_ctrl_1, 15: 8, dc_detail_min
	UINT8 reg_var_v_gain;				///< dc_bnr_ctrl_1, 23:20, var_v_gain
	UINT8 reg_var_h_gain;				///< dc_bnr_ctrl_1, 27:24, var_h_gain
	UINT8 reg_var_cut_resolution;		///< dc_bnr_ctrl_1, 31:28, var_cut_resolution
	UINT8 reg_dc_global_motion_th;		///< dc_bnr_ctrl_2,  7: 4, dc_global_motion_th
	UINT8 reg_dc_protection_th;			///< dc_bnr_ctrl_2, 31:24, dc_protection_th
	UINT8 reg_dc_bnr_var_th0;			///< dc_bnr_ctrl_3, 31:24, dc_bnr_var_th0
	UINT8 reg_dc_bnr_var_th1;			///< dc_bnr_ctrl_3, 23:16, dc_bnr_var_th1
	UINT8 reg_dc_bnr_var_th2;			///< dc_bnr_ctrl_3, 15: 8, dc_bnr_var_th2
	UINT8 reg_dc_bnr_var_th3;			///< dc_bnr_ctrl_3,  7: 0, dc_bnr_var_th3
	/* ac bnr 27 */
	UINT8 reg_bnr_ac_diff_min_v_th;		///< ac_bnr_ctrl_0, 15: 8, bnr_ac_diff_min_v_th
	UINT8 reg_bnr_ac_diff_min_h_th;		///< ac_bnr_ctrl_0, 23:16, bnr_ac_diff_min_h_th
	UINT8 reg_bnr_ac_global_motion_th;	///< ac_bnr_ctrl_1,  7: 0, bnr_ac_global_motion_th
	UINT8 reg_bnr_ac_h_acness_max;		///< ac_bnr_ctrl_1, 15: 8, reg_bnr_ac_h_acness_max(h15a)
	UINT8 reg_bnr_ac_h_acness_min;		///< ac_bnr_ctrl_1, 23:16, reg_bnr_ac_h_acness_min(h15a)
	UINT8 reg_bnr_ac_v_acness_max;		///< ac_bnr_ctrl_4, 23:16, reg_bnr_ac_v_acness_max(h15a)
	UINT8 reg_bnr_ac_v_acness_min;		///< ac_bnr_ctrl_4, 31:24, reg_bnr_ac_v_acness_min(h15a)
	UINT8 reg_bnr_ac_motion_0;			///< ac_bnr_ctrl_2, 31:24, reg_bnr_ac_motion_0(h15a)
	UINT8 reg_bnr_ac_motion_1;			///< ac_bnr_ctrl_2, 24:16, reg_bnr_ac_motion_1(h15a)
	UINT8 reg_bnr_ac_motion_2;			///< ac_bnr_ctrl_2, 15: 8, reg_bnr_ac_motion_2(h15a)
	UINT8 reg_bnr_ac_motion_3;			///< ac_bnr_ctrl_2,  7: 0, reg_bnr_ac_motion_3(h15a)
	UINT8 reg_bnr_ac_detail_th1;		///< ac_bnr_ctrl_5, 31:24, bnr_ac_detail_th1
	UINT8 reg_bnr_ac_detail_th2;		///< ac_bnr_ctrl_5, 23:16, bnr_ac_detail_th2
	UINT8 reg_bnr_ac_detail_th3;		///< ac_bnr_ctrl_5, 15: 8, bnr_ac_detail_th3
	UINT8 reg_bnr_ac_detail_th4;		///< ac_bnr_ctrl_5,  7: 0, bnr_ac_detail_th4
	UINT8 reg_bnr_ac_pos_gain_h0;		///< ac_bnr_ctrl_6, 23:16, bnr_ac_pos_gain_h0
	UINT8 reg_bnr_ac_pos_gain_h1;		///< ac_bnr_ctrl_6, 15: 8, bnr_ac_pos_gain_h1
	UINT8 reg_bnr_ac_pos_gain_h2;		///< ac_bnr_ctrl_6,  7: 0, bnr_ac_pos_gain_h2
	UINT8 reg_bnr_ac_pos_gain_h3;		///< ac_bnr_ctrl_7, 31:24, bnr_ac_pos_gain_h3
	UINT8 reg_bnr_ac_pos_gain_l0;		///< ac_bnr_ctrl_7, 23:16, bnr_ac_pos_gain_l0
	UINT8 reg_bnr_ac_pos_gain_l1;		///< ac_bnr_ctrl_7, 15: 8, bnr_ac_pos_gain_l1
	UINT8 reg_bnr_ac_pos_gain_l2;		///< ac_bnr_ctrl_7,  7: 0, bnr_ac_pos_gain_l2
	UINT8 reg_bnr_ac_pos_gain_l3;		///< ac_bnr_ctrl_8, 31:24, bnr_ac_pos_gain_l3
	UINT8 reg_bnr_ac_detail_max;		///< detail_ctrl,  7: 0, bnr_ac_detail_max
	UINT8 reg_bnr_ac_detail_min;		///< detail_ctrl, 15: 8, bnr_ac_detail_min
	UINT8 reg_bnr_diff_l;				///< detail_ctrl, 23:16, bnr_diff_l
	UINT8 reg_bnr_diff_p;				///< detail_ctrl, 31:24, bnr_diff_p
	/* mnr 18 */
	UINT8 reg_h_expand;					///< mnr_ctrl_0, 4, h_expand(h15a)
	UINT8 reg_gain_flt_size;			///< mnr_ctrl_0, 5, gain_flt_size
	UINT8 reg_mnr_s1_mmd_min;			///< mnr_ctrl_1,  7: 0, mnr_s1_mmd_min
	UINT8 reg_mnr_s2_ratio_min;			///< mnr_ctrl_1, 15: 8, mnr_s2_ratio_min
	UINT8 reg_mnr_s2_ratio_max;			///< mnr_ctrl_1, 23:16, mnr_s2_ratio_max
	UINT8 reg_mnr_s2_mmd_min;			///< mnr_ctrl_1, 31:24, mnr_s2_mmd_min
	UINT8 reg_filter_x0;				///< mnr_ctrl_3,  7: 0, filter_x0
	UINT8 reg_filter_x1;				///< mnr_ctrl_3, 15: 8, filter_x1
	UINT8 reg_filter_y0;				///< mnr_ctrl_3, 23:16, filter_y0
	UINT8 reg_filter_y1;				///< mnr_ctrl_3, 31:24, filter_y1
	UINT8 reg_motion_mnr_en;			///< mnr_ctrl_4, 0, motion_mnr_en
	UINT8 reg_motion_mnr_filter;		///< mnr_ctrl_4, 1, motion_mnr_filter
	UINT8 reg_mnr_motion_min;			///< mnr_ctrl_4, 23:16, mnr_motion_min
	UINT8 reg_mnr_motion_max;			///< mnr_ctrl_4, 31:24, mnr_motion_max
	UINT8 reg_motion_mnr_x0;			///< mnr_ctrl_5,  7: 0, motion_mnr_x0
	UINT8 reg_motion_mnr_x1;			///< mnr_ctrl_5, 15: 8, motion_mnr_x1
	UINT8 reg_motion_mnr_y0;			///< mnr_ctrl_5, 23:16, motion_mnr_y0
	UINT8 reg_motion_mnr_y1;			///< mnr_ctrl_5, 31:24, motion_mnr_y1
}
CHIP_DNR_DETAIL02_T;

/**
 * CHIP Dnr main control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	CHIP_DNR_CMN01_T		common;		///< main ctrl
	CHIP_DNR_DETAIL01_T		detail;		///< detail ctrl
}
CHIP_DNR_MAIN01_T;

/**
 * CHIP Dnr main control parameter type
 *	ver.02, for H15Ax
 */
typedef struct
{
	CHIP_DNR_CMN01_T		common;		///< main ctrl
	CHIP_DNR_DETAIL02_T		detail;		///< detail ctrl
}
CHIP_DNR_MAIN02_T;

typedef struct
{
	/* h */
	UINT8 edge_y_filter_en;				///< edge_y_filter_en
	UINT8 e_gain_max;					///< a to edge weight: gain
	UINT8 f_gain_max;					///< a to flat weight: gain
	UINT8 mp_lap_h_mode;				///< "000" : h3, "001" : h4, "010" : h5, "011" : h6, "100" : h7
	UINT8 sp_lap_h_mode;				///< "00" : h3, "01" : h5, "10" : h7, "11" : not used
	/* v */
	UINT8 der_v_en;						///< der-v enable
	UINT8 der_gain_mapping;				///< "00" : a-to weight, "01" : a-map, "10  : a-to weight, "11" : a-map
	UINT8 max_sel;						///< , "000" : do not expand, "001" : 3-tap, "010" : 5-tap, "011" : 7-tap, "100" : 9-tap, "101" : 11-tap, o.w   : do not expand
	UINT8 mp_lap_v_mode;				///< laplacian v mode, "000" : v3, "001" : v4, "010" : v5, "011" : v6, "100" : v7
	UINT8 sp_lap_v_mode;				///< 0' : v3, '1' : v5
	/* cmm */
	UINT8 mp_sobel_weight;				///< weight for sobel operator output
	UINT8 mp_laplacian_weight;			///< weight for laplacian operator output
	UINT8 sp_sobel_weight;				///< weight for sobel operator output
	UINT8 sp_laplacian_weight;			///< weight for laplacian operator output
	UINT8 flat_en;						///< flat-filter enable
	UINT8 flat_filter_type;				///< '0' : bi-lateral filter, '1': average filter
	/*d_jag*/
	UINT8 edf_en;						///< edge-directional de-jagging enable
	UINT8 center_blur_en;				///< '0' : use original pixel, '1' : use avg. pixel
	UINT8 count_diff_th;				///< matchness threshod for edge-direction decision
	UINT8 n_avg_mode;					///< mode for dual edges, '0' : use 12 direction results, '1' : use 36 direction results
	UINT8 line_variation_diff_th;		///< line-variation threshold for edge-direction decision
	UINT8 level_th;						///< g0: level threshold
	UINT8 protect_th;					///< g1: protect threshold
	UINT8 n_avg_gain;					///< neighborhood pixel averaing : gain
	UINT8 edf_count_min;				///< g0 : edf_count_min
	UINT8 edf_count_max;				///< g0 : edf_count_max
	UINT8 dj_h_count_min;				///< g0 : dj_h_count_min
	UINT8 dj_h_count_max;				///< g0 : dj_h_count_max
	UINT8 dj_v_count_min;				///< g0 : dj_v_count_min
	UINT8 dj_v_count_max;				///< g0 : dj_v_count_max

}
CHIP_SHP_CMN_B0_T;

typedef struct
{
	UINT8 mp_white_gain;	///< white gain
	UINT8 mp_black_gain;	///< black gain
	UINT8 sp_white_gain;	///< white gain
	UINT8 sp_black_gain;	///< black gain

}
CHIP_SHP_UI_MAIN_T;

typedef struct
{
	UINT8 reg_csft_gain;				///< reg_csft_gain: center shift gain(1.5u)
	UINT8 edge_filter_white_gain;		///< edge filter white gain upper 2bit: int, lower 4 bit: float, max: 63(dec) 3.9999
	UINT8 edge_filter_black_gain;		///< edge filter black gain
	UINT8 a_gen_width;					///< width size for dynamic range
	UINT8 mp_horizontal_gain;			///< horizontal gain(3.5u)
	UINT8 sp_horizontal_gain;			///< horizontal gain(3.5u)
	UINT8 e_gain_th1;					///< a to edge gain: th1
	UINT8 e_gain_th2;					///< a to edge gain: th2
	UINT8 f_gain_th1;					///< a to flat gain: th1
	UINT8 f_gain_th2;					///< a to flat gain: th2
	UINT8 coring_th;					///< coring_th
	UINT8 y_gain;						///< y_gain
	UINT8 c_gain;						///< c_gain

}
CHIP_SHP_UI_H_MAIN_T;

typedef struct
{
	UINT8 bif_manual_th;				///< bilateral filter threshold
	UINT8 csft_gain;					///< center-shift gain (1.5u)
	UINT8 gain_b;						///< gain(b) : 2.5u
	UINT8 gain_w;						///< gain(w): 2.5u
	UINT8 mmd_sel;						///< "000" : 11 tap, "001" : 9-tap, "010" : 7-tap, "011" : 5-tap, "100" : 3-tap , o.w   : 11-tap
	UINT8 mp_vertical_gain;				///< vertical gain(3.5u)
	UINT8 sp_vertical_gain;				///< vertical gain(3.5u)
	UINT8 gain_th1;						///< a-to-weight: th1
	UINT8 gain_th2;						///< a-to-weight: th2

}
CHIP_SHP_UI_V_MAIN_T;

/**
 * Scaler Filter dependant on Sharpness
 * 	- chip dependant
 * @see
*/

typedef struct CHIP_SCLFILTER_SETTINGS
{
	UINT32		ulLumaHorIndex;
	UINT32		ulLumaVerIndex;
	UINT32		ulChromaHorIndex;
	UINT32		ulChromaVerIndex;
}
CHIP_SCLFILTER_SETTINGS_T;

typedef struct
{
	/*e_map*/
	UINT8 amap2_sel;					///< "00" : 5-tap, "01" : 7-tap, "10" : 9-tap, "11" : 15-tap
	UINT8 amap_gain;					///< edge-map gain
	UINT8 ga_max;						///< <a to edge weight control>
	UINT8 ga_th0;						///< th0
	UINT8 ga_th1;						///< th1
	/*t_map*/
	UINT8 amap1_sel;					///< "00" : 15-tap, "01" : 9-tap, "10" : 7-tap, "11" : 5-tap
	UINT8 tmap_max_sel;						///< "000" : bypass, "001" : 5-tap, "010" : 7-tap, "011" : 9-tap, "100" : 11-tap, "101" : 13-tap, "110" : 15-tap, "111" : 17-tap
	UINT8 avg_sel;						///< "00" : bypass, "01" : 3x1 average [1 2 1], "10" : 5x1 average [1 2 2 2 1 ], "11" : bypass
	UINT8 tmap_gain;					///< texture-map gain(2.5u)
	UINT8 gt_th0;						///< th0
	UINT8 gt_th0a;						///< th0a
	UINT8 gt_th0b;						///< th0b
	UINT8 gt_th1;						///< th1
	UINT8 gt_gain0a;					///< gain(th0a) : 1.5u
	UINT8 gt_gain0b;					///< gain(th0b) : 1.5u
	UINT8 gt_max;						///< gain(th1) :1.5u
	UINT8 a2tw_en;						///< strong edge protection , '0': disable, '1': enable
	UINT8 a2tw_th0;						///< strong edge: th0
	UINT8 a2tw_th1;						///< strong edge: th1
	UINT8 exp_mode;						///< "00" : bypass, "01" : 3-tap expansion, "10" : 5-tap expansion, "11" : bypass
	UINT8 coring_mode1;					///< coring mode "00" : remove 0, "01" : remove 0/1, "10" : remove 0/1/2, "11" : remove 0/1/2/3
	UINT8 coring_mode2;					///< coring mode, "00" : remove 0, "01" : remove 0/1, "10" : remove 0/1/2, "11" : remove 0/1/2/3
	UINT8 g_th0;						///< <edge/texture blending> th0
	UINT8 g_th1;						///< th1
	UINT16 var_th;						///< flat region rejection threshold if variance < var_th, reject the region

}
CHIP_SHP_MAP_CMN_T;

typedef struct
{
	UINT8 mp_edge_gain_b;	///< MP: edge gain(b) : 2.5u
	UINT8 mp_edge_gain_w;	///< MP: edge gain(w)
	UINT8 sp_edge_gain_b;	///< SP: edge gain(b) : 2.5u
	UINT8 sp_edge_gain_w;	///< SP: edge gain(w)
}
CHIP_SHP_EE_T;


/**
 *	PQL detail enhancement(DE) control parameter type by UI (0~100)
 */
typedef struct
{
	UINT8 mp_texture_gain_b;	///< MP: texture gain(b)
	UINT8 mp_texture_gain_w;	///< MP: texture gain(w)
	UINT8 sp_texture_gain_b;	///< SP: texture gain(b)
	UINT8 sp_texture_gain_w;	///< SP: texture gain(w)
}
CHIP_SHP_DE_T;

typedef struct
{
	UINT8 enable;					///< ti-h enable
	UINT8 coring_step;					///< n : 2^n (transition width)
	UINT8 gain0_en;						///< gain0 enable
	UINT8 gain1_en;						///< gain1 enable
	UINT8 gain0_th0;					///< gain0_th0
	UINT8 gain1_th1;					///< gain1_th1
	UINT8 gain1_div_mode;				///< gain1: div mode, "00" : div by 32, "01" : div by 16, "10" : div by 8, "11" : div by 64

}
CHIP_TIH_CMN_T;

typedef struct
{
	UINT8 cti_en;						///< cti enable
	UINT8 coring_th0;					///< th with coring th0 mmd
	UINT8 coring_th1;					///< coring_th1
	UINT8 coring_map_filter;			///< "000" : no , "001" : 5-tap , "010" : 7-tap , "011" : 9-tap , 100: : 11-tap , "101" : 13-tap
	UINT8 coring_tap_size;				///< "000" 21 tap, "001" 19 tap, "010" 17 tap, "011" 15 tap, "100" 13 tap, "101" 11 tap, "110"  9 tap
	UINT8 ycm_en1;						///< y c miss matching.
	UINT8 ycm_band_sel;					///< ycm_band_sel
	UINT8 ycm_diff_th;					///< ycm_diff_th

}
CHIP_CTI_CMN_T;

typedef struct
{
	UINT8 tap_size;
	UINT8 cti_gain;						///< gain (3.5u)
	UINT8 ycm_y_gain;					///< ycm_y_gain
	UINT8 ycm_c_gain;					///< ycm_c_gain
}
CHIP_CTI_UI_MAIN_T;

typedef struct
{
	CHIP_SHP_UI_MAIN_T	Shp;	//common
	CHIP_CTI_UI_MAIN_T		UiCti;	//CTI
	CHIP_SHP_UI_H_MAIN_T	HShp;	//h sharp
	CHIP_SHP_UI_V_MAIN_T	VShp;	//v sharp
}
CHIP_SHP_B0_T;

typedef struct
{
	CHIP_CTI_CMN_T			CtiCmn;
	CHIP_SHP_CMN_B0_T		CmnShp;
	CHIP_SHP_MAP_CMN_T		MapShp;
	CHIP_TIH_CMN_T			TihShp;
}
CHIP_SHP_MISC_B0_T;

/**
 *	pe resolution enhancement(RE) misc. control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	/* h 5 */
	UINT8 edge_y_filter_en;				///< edge_y_filter_en
	UINT8 e_gain_max;					///< a to edge weight: gain
	UINT8 f_gain_max;					///< a to flat weight: gain
	UINT8 mp_lap_h_mode;				///< "000" : h3,"001" : h4,"010" : h5,"011" : h6,"100" : h7
	UINT8 sp_lap_h_mode;				///< "00" : h3,"01" : h5,"10" : h7,"11" : not used
	/* v 5 */
	UINT8 der_v_en;						///< der-v enable
	UINT8 der_gain_mapping;				///< "00" : a-to weight,"01" : a-map,"10  : a-to weight,"11" : a-map
	UINT8 max_sel;						///< ,"000" : do not expand,"001" : 3-tap,"010" : 5-tap,"011" : 7-tap,"100" : 9-tap,"101" : 11-tap,o.w   : do not expand
	UINT8 mp_lap_v_mode;				///< laplacian v mode,"000" : v3,"001" : v4,"010" : v5,"011" : v6,"100" : v7
	UINT8 sp_lap_v_mode;				///< 0' : v3,'1' : v5
	/* cmm 6 */
	UINT8 mp_sobel_weight;				///< weight for sobel operator output
	UINT8 mp_laplacian_weight;			///< weight for laplacian operator output
	UINT8 sp_sobel_weight;				///< weight for sobel operator output
	UINT8 sp_laplacian_weight;			///< weight for laplacian operator output
	UINT8 flat_en;						///< flat-filter enable
	UINT8 flat_filter_type;				///< '0' : bi-lateral filter,'1': average filter
	/*d_jag 12 */
	UINT8 edf_en;						///< edge-directional de-jagging enable
	UINT8 center_blur_mode;				///< [m14b]center-pixel averaging for edge-direction calculation,0:use original pixel, 1:use avg. pixel
	UINT8 count_diff_th;				///< matchness threshod for edge-direction decision
	UINT8 n_avg_mode;					///< mode for dual edges,'0' : use 12 direction results,'1' : use 36 direction results
	UINT8 line_variation_diff_th;		///< line-variation threshold for edge-direction decision
	UINT8 level_th;						///< g0: level threshold
	UINT8 protect_th;					///< g1: protect threshold
	UINT8 n_avg_gain;					///< neighborhood pixel averaing : gain
	UINT8 reg_g0_cnt_min;				///< [m14b]G0 : edf_count_min
	UINT8 reg_g0_mul;					///< [m14b]G0 : n x ( Count - min_cnt_th )
	UINT8 reg_g1_protect_min;			///< [m14b]G1 : Min diff th - LR Diff
	UINT8 reg_g1_mul;					///< [m14b]G1 : n x ( th - LR_Diff )
	/*e_map 5 */
	UINT8 amap2_sel;					///< "00" : 5-tap,"01" : 7-tap,"10" : 9-tap,"11" : 15-tap
	UINT8 amap_gain;					///< edge-map gain
	UINT8 ga_max;						///< <a to edge weight control>
	UINT8 ga_th0;						///< th0
	UINT8 ga_th1;						///< th1
	/*t_map 22*/
	UINT8 amap1_sel;					///< "00" : 15-tap,"01" : 9-tap,"10" : 7-tap,"11" : 5-tap
	UINT8 tmap_max_sel;					///< "000" : bypass,"001" : 5-tap,"010" : 7-tap,"011" : 9-tap,"100" : 11-tap,"101" : 13-tap,"110" : 15-tap,"111" : 17-tap
	UINT8 avg_sel;						///< "00" : bypass,"01" : 3x1 average [1 2 1],"10" : 5x1 average [1 2 2 2 1 ],"11" : bypass
	UINT8 tmap_gain;					///< texture-map gain(2.5u)
	UINT8 gt_th0;						///< th0
	UINT8 gt_th0a;						///< th0a
	UINT8 gt_th0b;						///< th0b
	UINT8 gt_th1;						///< th1
	UINT8 gt_gain0a;					///< gain(th0a) : 1.5u
	UINT8 gt_gain0b;					///< gain(th0b) : 1.5u
	UINT8 gt_max;						///< gain(th1) :1.5u
	UINT8 a2tw_en;						///< strong edge protection ,'0': disable,'1': enable
	UINT8 a2tw_th0;						///< strong edge: th0
	UINT8 a2tw_th1;						///< strong edge: th1
	UINT8 exp_mode;						///< "00" : bypass,"01" : 3-tap expansion,"10" : 5-tap expansion,"11" : bypass
	UINT8 coring_mode1;					///< coring mode "00" : remove 0,"01" : remove 0/1,"10" : remove 0/1/2,"11" : remove 0/1/2/3
	UINT8 coring_mode2;					///< coring mode,"00" : remove 0,"01" : remove 0/1,"10" : remove 0/1/2,"11" : remove 0/1/2/3
	UINT8 g_th0;						///< <edge/texture blending> th0
	UINT8 g_th1;						///< th1
	UINT8 var_h_th;						///< [m14b]flat region rejection threshold (H-Direction T-Map)if variance < var_th, reject the region
	UINT8 var_v_th;						///< [m14b]flat region rejection threshold (V-Direction T-Map)if variance < var_th, reject the region
	UINT8 tmap_sc_var_th;				///< [m14b]flat region rejection threshold (for SC T-Map)if variance < var_th, reject the region
	/*ti-h 7 */
	UINT8 enable;						///< ti-h enable
	UINT8 coring_step;					///< n : 2^n (transition width)
	UINT8 gain0_en;						///< gain0 enable
	UINT8 gain1_en;						///< gain1 enable
	UINT8 gain0_th0;					///< gain0_th0
	UINT8 gain0_th1;					///< [m14b]gain0_th1
	UINT8 gain1_div_mode;				///< gain1: div mode,"00" : div by 32,"01" : div by 16,"10" : div by 8,"11" : div by 64
	/* cti 8 */
	UINT8 cti_en;						///< cti enable
	UINT8 coring_th0;					///< th with coring th0 mmd
	UINT8 coring_th1;					///< coring_th1
	UINT8 coring_map_filter;			///< "000" : no ,"001" : 5-tap ,"010" : 7-tap ,"011" : 9-tap ,100: : 11-tap ,"101" : 13-tap
	UINT8 coring_tap_size;				///< "000" 21 tap,"001" 19 tap,"010" 17 tap,"011" 15 tap,"100" 13 tap,"101" 11 tap,"110"  9 tap
	UINT8 ycm_en1;						///< y c miss matching.
	UINT8 ycm_band_sel;					///< ycm_band_sel
	UINT8 ycm_diff_th;					///< ycm_diff_th
}
CHIP_SHP_MISC01_T;

/**
 * CHIP Sharpness main control parameter type
 *	for H13BX,M14AX
 */
typedef struct
{
	CHIP_SHP_B0_T main;				///< main ctrl
	CHIP_SHP_MISC_B0_T misc;		///< misc ctrl
	CHIP_SHP_EE_T edge;				///< edge enhance
	CHIP_SHP_DE_T detail;			///< detail enhance
	CHIP_SCLFILTER_SETTINGS_T scl;	///< scaler filter
}
CHIP_SHP_MAIN_B0_T;

/**
 * CHIP Sharpness main control parameter type
 *	ver.01, for M14Bx
 */
typedef struct
{
	CHIP_SHP_B0_T main;				///< main ctrl
	CHIP_SHP_MISC01_T misc;			///< misc ctrl
	CHIP_SHP_EE_T edge;				///< edge enhance
	CHIP_SHP_DE_T detail;			///< detail enhance
	CHIP_SCLFILTER_SETTINGS_T scl;	///< scaler filter
}
CHIP_SHP_MAIN01_T;

/**
*	SR CMN Param stucture.
*/
typedef struct
{
	UINT8 enable;					///< enable
	UINT8 DB_grade; 				///< DB_grade
	UINT8 Mode_ls;					///<Mode_ls
	UINT8 SR_a_local_scale; 		///<SR_a local scale
	UINT8 SR_b_local_scale ;		///<SR_b local scale
	UINT8 PP_X_filter_coefficient ; ///<PP_X filter coefficient
	UINT8 PP_Y_filter_coefficient ; ///<PP_Y filter coefficient
	UINT8 PP_global_scale;			///<PP_global scale
	UINT8 PP_local_scale;			///<PP_local scale
}
CHIP_SR_CMN_T;

/**
*	SR UI MAIN Param stucture.
*/
typedef struct
{
	UINT8 SR_a_global_scale;		///<PP_global scale
	UINT8 SR_b_global_scale;		///<PP_local scale
}
CHIP_SR_UI_MAIN_T;

/**
*	SR MAIN Param stucture.
*/
typedef struct
{
	CHIP_SR_CMN_T sr_cmn;			///< SR common
	CHIP_SR_UI_MAIN_T sr_ui;		///< SR ui
}
CHIP_SR_MAIN_T;

/**
 *	SR MAIN Param stucture
 *	ver.01, for M14Bx
 *	- enable(reg_out_mux_sel) : 0x0:Bypass, 0x1:Weighting output,
 *					0x2,0x4,0x8,0x10,0x20,0x40:etc
 *	- reg_sc_ls_lv_sra : each value converts to 4bit(1 hex) in order arranged,
 *					Default: [8,8,12,15,15,15,14,14]
 *	- reg_sc_ls_lv_srb : each value converts to 4bit(1 hex) in order arranged,
 *					Default: [4,4,10,15,15,15,10,10]
 */
typedef struct
{
	UINT32 enable;				///< SRE Ouput selection,
	UINT32 reg_sc_gs_sra;		///< [0-127] Global scale of sra
	UINT32 reg_sc_gs_srb;		///< [0-127] Global scale of srb
	UINT32 reg_sc_ls_lv_sra;	///< [0-15]: local scale level of sra
	UINT32 reg_sc_ls_lv_srb;	///< [0-15]: local scale level of srb
	UINT32 reg_mode_wei;		///< [0-7]: wei_sra, 0:only srb,1:only sra
}
CHIP_SR_MAIN01_T;

/**
 *	SR MAIN Param stucture
 *	ver.02, for H15Ax
 */
typedef struct
{
	UINT32 core1_use_iu_en;		///< [0,1] core1_use_iu_en
	UINT32 core1_we_uh_range0;	///< [0~127] core1_we_uh_range0
	UINT32 core1_we_uh_range1;	///< [0~127] core1_we_uh_range1
	UINT32 core1_we_dh_range0;	///< [0~127] core1_we_dh_range0
	UINT32 core1_we_dh_range1;	///< [0~127] core1_we_dh_range1
}
CHIP_SR_MAIN02_T;

/**
*	CMS Param stucture.
*/
typedef struct
{
	SINT32 hueOffset;
	SINT32 satOffset;
	SINT32 valOffset;
}
CHIP_CMS_PARAM_T;

/**
*	CM Skin Param stucture.
*/
typedef struct
{
	UINT8 vpqPath;				/**< pql path */
	UINT8 *pstVpqStatus;		/**< pql status */
	SINT8 uiSkinColorValue;		/**< UI Skin Color Value */
	UINT8 uiFreshColorValue;	/**< UI Fresh Color Value */
	SINT8 uiSkinSocHueValue;	/**< Hue Value */
}
CHIP_CM_SKINCOLOR_PARAM_T;

/**
*	CM Grass Param stucture.
*/
typedef struct
{
	UINT8 vpqPath;				/**< pql path */
	UINT8 *pstVpqStatus;		/**< pql status */
	SINT8 uiGrassColorValue;	/**< UI Grass Color Value */
	UINT8 uiFreshColorValue;	/**< UI Fresh Color Value */
	SINT8 uiGrassSocHueValue;	/**< Hue Value */
}
CHIP_CM_GRASSCOLOR_PARAM_T;

/**
*	CM Blue Sky Param stucture.
*/
typedef struct
{
	UINT8 vpqPath;				/**< pql path */
	UINT8 *pstVpqStatus;		/**< pql status */
	SINT8 uiBSkyColorValue;		/**< UI BSky Color Value */
	UINT8 uiFreshColorValue;	/**< UI Fresh Color Value */
	SINT8 uiBSkySocHueValue;	/**< Hue Value */
}
CHIP_CM_BSKYCOLOR_PARAM_T;

/**
 *	CHIP_CM_REGION_B0
 */
typedef struct
{
	BOOLEAN	region_en;
	UINT32 regionNum;		// 0~15
	UINT16 hue_x[CHIP_CM_TBLPOINT];		// 0~360~720 degree
	UINT8 hue_g[CHIP_CM_TBLPOINT];		// 0~127
	UINT8 sat_x[CHIP_CM_TBLPOINT];		// 0~100
	UINT8 sat_g[CHIP_CM_TBLPOINT];		// 0~127
	UINT8 val_x[CHIP_CM_TBLPOINT];		// 0~255
	UINT8 val_g[CHIP_CM_TBLPOINT];		// 0~127
}
CHIP_CM_REGION_B0_T;

/**
*	CM de-saturation
*/
typedef struct
{
	UINT32 regionNum;		// 0~15
	UINT16	dce_bypass_gain;// 0~255
	UINT8	region_OnOff[CHIP_CM_REGION_NUM];	// 0~1
} CHIP_CM_DESATURATION_B0_T;

/**
*	CHIP_CM_REGIONCTRL_B0
*/
typedef struct
{
	UINT32 regionNum;		// 0~15
	SINT8 reDelta[CHIP_CM_DELTANUM];		// -128 ~ 127, [0]h [1]s [2]v [3]g [4]b [5]r
	UINT8 reMasterGain;		// 0~128~255
} CHIP_CM_REGIONCTRL_B0_T;

/**
*	CHIP_DCM_DCE_SMOOTH_T
*/
typedef struct
{
	UINT32 avg_filter_tap;		///< luma blur for DCE
	UINT32 chroma_blur_mode;	///< chroma blur for DCE
	UINT32 edge_map_tap;		///< edge-map tap size for blending
	UINT32 blur_v_gain;			///< 0x0 : blur V <-> 0xF : original V
	UINT32 e_to_w_th_x0;		///< edge to weight : x0
	UINT32 e_to_w_th_x1;		///< edge to weight : x1
	UINT32 e_to_w_th_y0;		///< edge to weight : y0
	UINT32 e_to_w_th_y1;		///< edge to weight : y2
}
CHIP_DCM_DCE_SMOOTH_T;

/**
*	CHIP_DCM_DCE_SMOOTH_T
*	- for H14,M14B
*/
typedef struct
{
	UINT32 sc_map_sel;			///< 0x0:MAX(e,t map),0x1:t map,0x2:e map
	UINT32 sc_amap_filter_tap;	///< amap, 0x0~0x3 : 5,7,9,15-tap
	UINT32 amap_gain;			///< e map gain:(2.3u)0x0=0 ~ 0x8=1.0 ~ 0x1F=3.99x
	UINT32 tmap_gain;			///< t map gain:(2.3u)0x0=0 ~ 0x8=1.0 ~ 0x1F=3.99x
	UINT32 e_to_w_th_x0;		///< edge to weight : x0
	UINT32 e_to_w_th_x1;		///< edge to weight : x1
	UINT32 e_to_w_th_y0;		///< edge to weight : y0
	UINT32 e_to_w_th_y1;		///< edge to weight : y1
	UINT32 blur_v_gain;			///< 0x0 : blur V <-> 0xF : original V
}
CHIP_DCM_DCE_SMOOTH_V1_T;

/**
*	CHIP_CM_CLEAR_WHITE_T
*/
typedef struct
{
	UINT32 gain_table_x[CHIP_CM_CW_GAINTB];// 16
	UINT32 gain_table_y[CHIP_CM_CW_GAINTB];// 16
	UINT8  color_region_gain;
	UINT8  color_region_onoff[CHIP_CM_CW_NUM];// 0~1
	UINT8  cw_en;
	UINT8  cw_sel;
	UINT32 cw_r;
	UINT32 cw_g;
	UINT32 cw_b;
}
CHIP_CM_CLEAR_WHITE_T;

/**
 *	init adv color type
 */
typedef struct
{
	CHIP_CM_REGION_B0_T data[CHIP_CM_RGN_CTRL_NUM];		///< region data
}
CHIP_CM_INIT_ADV_COLOR_T;

/**
 *	set fresh color type
 */
typedef struct
{
	CHIP_CM_REGIONCTRL_B0_T gain[CHIP_CM_RGN_CTRL_NUM];	///< region gain
	CHIP_CM_CLEAR_WHITE_T clear_white;					///< clear white
}
CHIP_CM_FRESH_COLOR_T;

/**
 *	set preferred color type
 */
typedef struct
{
	CHIP_CM_REGIONCTRL_B0_T gain;			///< region gain
	SINT8 skin_hue_val;						///< skin hue value
	SINT8 bsky_hue_val;						///< blue sky hue value
	SINT8 grass_hue_val;					///< grass hue value
}
CHIP_CM_PREFERRED_COLOR_T;

/**
 *	init exp color type
 */
typedef struct
{
	CHIP_CM_REGION_B0_T data[CHIP_CM_RGN_CTRL_NUM];		///< region data
	CHIP_CM_REGIONCTRL_B0_T gain[CHIP_CM_RGN_CTRL_NUM];	///< region gain
	CHIP_CM_CLEAR_WHITE_T clear_white;					///< clear white
}
CHIP_CM_INIT_EXP_COLOR_T;

/**
 *	set color menagement type
 */
typedef struct
{
	CHIP_CM_REGIONCTRL_B0_T gain;		///< region gain
	SINT32 hue_offset;					///< hue offset
	SINT32 sat_offset;					///< saturation offset
	SINT32 val_offset;					///< value offset
}
CHIP_CM_COLOR_MANAGEMENT_T;

/**
 *	set clear white gain type
 */
typedef struct
{
	UINT32 r_gain;					///< red gain : 0~255
	UINT32 g_gain;					///< green gain : 0~255
	UINT32 b_gain;					///< blue gain : 0~255
}
CHIP_CM_CW_GAIN_CTRL_T;
/**
*	DC2P de-saturation
*/
typedef struct
{
	UINT32 dce_bypass_gain;			///< dce out gain, 1(0%)~128(200%)~255(400%) (forbidden 0)
	UINT8 region_onoff[CHIP_CM_REGION_NUM];	///< color region apply for dce y
	UINT32 color_region_en;			///< color region apply for dce y
	UINT32 y_grad_gain;				///< Gradient of Y  signal,0x0:128to0, 0x1:64to0, 0x2:32to0, 0x3:16to0
	UINT32 cb_grad_gain;			///< Gradient of Cb signal,0x0:128to0, 0x1:64to0, 0x2:32to0, 0x3:16to0
	UINT32 cr_grad_gain;			///< Gradient of Cr signal,0x0:128to0, 0x1:64to0, 0x2:32to0, 0x3:16to0
	UINT32 y_range_min;				///< y_range_min : 0~1023
	UINT32 y_range_max;				///< y_range_max : 0~1023
	UINT32 cb_range_min;			///< cb_range_min : 0~1023
	UINT32 cb_range_max;			///< cb_range_max : 0~1023
	UINT32 cr_range_min;			///< cr_range_min : 0~1023
	UINT32 cr_range_max;			///< cr_range_max : 0~1023
} CHIP_DC2P_DESATURATION_T;

/**
*	fresh contrast control
*/
typedef struct
{
	UINT16 pos_low; 						///< low position
	UINT16 pos_high; 						///< high position
	CHIP_DC2P_DESATURATION_T desat;			///< desaturation
	CHIP_DCM_DCE_SMOOTH_T smooth0;			///< smooth contrast(v0)
	CHIP_DCM_DCE_SMOOTH_V1_T smooth1;		///< smooth contrast(v1)
}
CHIP_DC2P_FRESH_CONTRAST_T;

/******************************************************************/
/* H15** **********************************************************/
/******************************************************************/

/**
 * CHIP_TNR_DB_H15_T for HAL_VPQ_SetNoiseReduction
 *  001._[TNR_MC__]_0xC8004140_[0____]_UINT8__tnr_en_____:__________________reg_tnr_en
 *  002._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[00]_:_tnr_main_lut_01,_eg_detail_iir_pt_luma_76,
 *  003._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[01]_:_tnr_main_lut_01,_eg_detail_iir_pt_luma_54,
 *  004._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[02]_:_tnr_main_lut_01,_eg_detail_iir_pt_luma_32,
 *  005._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[03]_:_tnr_main_lut_01,_eg_detail_iir_pt_luma_10,
 *  006._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[04]_:_tnr_main_lut_01,_eg_detail_iir_pt_chroma_76,
 *  007._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[05]_:_tnr_main_lut_01,_eg_detail_iir_pt_chroma_54,
 *  008._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[06]_:_tnr_main_lut_01,_eg_detail_iir_pt_chroma_32,
 *  009._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[07]_:_tnr_main_lut_01,_eg_detail_iir_pt_chroma_10,
 *  010._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[08]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_76,
 *  011._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[09]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_54,
 *  012._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[10]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_32,
 *  013._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[11]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_10,
 *  014._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[12]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_76,
 *  015._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[13]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_54,
 *  016._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[14]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_32,
 *  017._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_lm[15]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_10,
 *  018._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[00]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_76,
 *  019._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[01]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_54,
 *  020._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[02]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_32,
 *  021._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[03]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_10,
 *  022._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[04]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_76,
 *  023._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[05]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_54,
 *  024._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[06]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_32,
 *  025._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[07]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_10,
 *  026._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[08]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_76,
 *  027._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[09]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_54,
 *  028._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[10]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_32,
 *  029._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[11]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_10,
 *  030._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[12]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_76,
 *  031._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[13]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_54,
 *  032._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[14]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_32,
 *  033._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_ui[15]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_10,
 *  034._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[00]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_76,
 *  035._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[01]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_54,
 *  036._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[02]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_32,
 *  037._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[03]_:_tnr_main_lut_01,_reg_detail_iir_pt_luma_10,
 *  038._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[04]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_76,
 *  039._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[05]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_54,
 *  040._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[06]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_32,
 *  041._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[07]_:_tnr_main_lut_01,_reg_detail_iir_pt_chroma_10,
 *  042._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[08]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_76,
 *  043._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[09]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_54,
 *  044._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[10]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_32,
 *  045._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[11]_:_tnr_main_lut_01,_reg_flat_iir_pt_luma_10,
 *  046._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[12]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_76,
 *  047._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[13]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_54,
 *  048._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[14]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_32,
 *  049._[TNR_MC__]_0xc80041d4_[31:0_]_UINT32_lut_hm[15]_:_tnr_main_lut_01,_reg_flat_iir_pt_chroma_10,
 *  050._[TNR_ME__]_0xC8004140_[7:6__]_2_UINT8__s_m[00]_:_tnr_ctrl_00,_reg_sad_8x3_res,
 *  051._[TNR_ME__]_0xC8004140_[8____]_1_UINT8__s_m[01]_:_tnr_ctrl_00,_reg_sad_8x3_iir_en,
 *  052._[TNR_ME__]_0xC8004140_[31:24]_8_UINT8__s_m[02]_:_tnr_ctrl_00,_reg_sad_8x3_iir_alpha,
 *  053._[TNR_SC__]_0xC8004148_[0____]_1_UINT8__s_m[03]_:_tnr_ctrl_02,_sc_en,
 *  054._[TNR_SC__]_0xC8004148_[23:16]_8_UINT8__s_m[04]_:_tnr_ctrl_02,_sc_th_ma,
 *  055._[TNR_SM__]_0xc8004164_[6:0__]_7_UINT8__s_m[05]_:_tnr_ctrl_09,_sad_base,
 *  056._[TNR_SM__]_0xc800416C_[0____]_1_UINT8__s_m[06]_:_tnr_ctrl_11,_lb_smooth_en,
 *  057._[TNR_SM__]_0xc8004188_[2:0__]_3_UINT8__s_m[07]_:_tnr_ctrl_18,_bsad_scale,
 *  058._[TNR_SM__]_0xc8004188_[6:4__]_3_UINT8__s_m[08]_:_tnr_ctrl_18,_gsad_scale,
 *  059._[TNR_SM__]_0xc8004188_[15___]_1_UINT8__s_m[09]_:_tnr_ctrl_18,_gmv_gmv_tl_en,
 *  060._[TNR_SM__]_0xc8004188_[23___]_1_UINT8__s_m[10]_:_tnr_ctrl_18,_bmv_gmv_tl_en,
 *  061._[TNR_SM__]_0xc8004188_[31___]_1_UINT8__s_m[11]_:_tnr_ctrl_18,_bmv_pmv_tl_en,
 *  062._[TNR_SM__]_0xc8004188_[14:8_]_7_UINT8__s_m[12]_:_tnr_ctrl_18,_gmv_gmv_tl,
 *  063._[TNR_SM__]_0xc8004188_[22:16]_7_UINT8__s_m[13]_:_tnr_ctrl_18,_bmv_gmv_tl,
 *  064._[TNR_SM__]_0xc8004188_[30:24]_7_UINT8__s_m[14]_:_tnr_ctrl_18,_bmv_pmv_tl,
 *  065._[TNR_SM__]_0xc800418c_[23___]_1_UINT8__s_m[15]_:_tnr_ctrl_19,_gmv_0mv_tl_en,
 *  066._[TNR_SM__]_0xc800418c_[31___]_1_UINT8__s_m[16]_:_tnr_ctrl_19,_bmv_0mv_tl_en,
 *  067._[TNR_SM__]_0xc800418c_[22:16]_7_UINT8__s_m[17]_:_tnr_ctrl_19,_gmv_0mv_tl,
 *  068._[TNR_SM__]_0xc800418c_[30:24]_7_UINT8__s_m[18]_:_tnr_ctrl_19,_bmv_0mv_tl,
 *  069._[TNR_SM__]_0xc8004164_[13:8_]_6_UINT8__s_m[19]_:_tnr_ctrl_09,_lb_smooth_clip,
 *  070._[TNR_SM__]_0xc8004158_[7:0__]_8_UINT8__s_m[20]_:_tnr_ctrl_06,_me_lb_sad_base,
 *  071._[TNR_SM__]_0xc800416C_[1____]_1_UINT8__s_m[21]_:_tnr_ctrl_11,_pv_smooth_en,
 *  072._[TNR_SM__]_0xc8004164_[21:16]_6_UINT8__s_m[22]_:_tnr_ctrl_09,_pv_smooth_clip,
 *  073._[TNR_SM__]_0xc8004158_[15:8_]_8_UINT8__s_m[23]_:_tnr_ctrl_06,_pv_smooth_sad_base,
 *  074._[TNR_EDF_]_0xc8004190_[31:24]_8_UINT8__edf[00]_:_tnr_ctrl_20,_reg_edge_alpha_mul,
 *  075._[TNR_EDF_]_0xc8004190_[23:16]_8_UINT8__edf[01]_:_tnr_ctrl_20,_reg_edge_beta_mul,
 *  076._[TNR_EDF_]_0xc8004190_[15:8_]_8_UINT8__edf[02]_:_tnr_ctrl_20,_reg_detail_alpha_mul,
 *  077._[TNR_EDF_]_0xc8004190_[7:0__]_8_UINT8__edf[03]_:_tnr_ctrl_20,_reg_detail_beta_mul,
 *  078._[TNR_EDF_]_0xc80043d0_[5:4__]_2_UINT8__edf[04]_:_tnr_ctrl_40,_reg_t_cut_resolution,
 *  079._[TNR_EDF_]_0xc80043d8_[31:24]_8_UINT8__edf[05]_:_tnr_ctrl_42,_reg_a_lut_edge_x1,
 *  080._[TNR_EDF_]_0xc80043d8_[23:16]_8_UINT8__edf[06]_:_tnr_ctrl_42,_reg_a_lut_edge_y1,
 *  081._[TNR_EDF_]_0xc80043d8_[15:8_]_8_UINT8__edf[07]_:_tnr_ctrl_42,_reg_a_lut_edge_x0,
 *  082._[TNR_EDF_]_0xc80043d8_[7:0__]_8_UINT8__edf[08]_:_tnr_ctrl_42,_reg_a_lut_edge_y0,
 *  083._[TNR_EDF_]_0xc80043dc_[31:24]_8_UINT8__edf[09]_:_tnr_ctrl_43,_reg_a_lut_detail_x1,
 *  084._[TNR_EDF_]_0xc80043dc_[23:16]_8_UINT8__edf[10]_:_tnr_ctrl_43,_reg_a_lut_detail_y1,
 *  085._[TNR_EDF_]_0xc80043dc_[15:8_]_8_UINT8__edf[11]_:_tnr_ctrl_43,_reg_a_lut_detail_x0,
 *  086._[TNR_EDF_]_0xc80043dc_[7:0__]_8_UINT8__edf[12]_:_tnr_ctrl_43,_reg_a_lut_detail_y0,
 *  087._[TNR_EDF_]_0xc80043e0_[31:24]_8_UINT8__edf[13]_:_tnr_ctrl_44,_reg_t_lut_edge_x1,
 *  088._[TNR_EDF_]_0xc80043e0_[23:16]_8_UINT8__edf[14]_:_tnr_ctrl_44,_reg_t_lut_edge_y1,
 *  089._[TNR_EDF_]_0xc80043e0_[15:8_]_8_UINT8__edf[15]_:_tnr_ctrl_44,_reg_t_lut_edge_x0,
 *  090._[TNR_EDF_]_0xc80043e0_[7:0__]_8_UINT8__edf[16]_:_tnr_ctrl_44,_reg_t_lut_edge_y0,
 *  091._[TNR_EDF_]_0xc80043e4_[31:24]_8_UINT8__edf[17]_:_tnr_ctrl_45,_reg_t_lut_detail_x1,
 *  092._[TNR_EDF_]_0xc80043e4_[23:16]_8_UINT8__edf[18]_:_tnr_ctrl_45,_reg_t_lut_detail_y1,
 *  093._[TNR_EDF_]_0xc80043e4_[15:8_]_8_UINT8__edf[19]_:_tnr_ctrl_45,_reg_t_lut_detail_x0,
 *  094._[TNR_EDF_]_0xc80043e4_[7:0__]_8_UINT8__edf[20]_:_tnr_ctrl_45,_reg_t_lut_detail_y0,
 *  095._[TNR_EDF_]_0xc80043e8_[31:24]_8_UINT8__edf[21]_:_tnr_ctrl_46,_reg_final_lut_edge_x1,
 *  096._[TNR_EDF_]_0xc80043e8_[23:16]_8_UINT8__edf[22]_:_tnr_ctrl_46,_reg_final_lut_edge_y1,
 *  097._[TNR_EDF_]_0xc80043e8_[15:8_]_8_UINT8__edf[23]_:_tnr_ctrl_46,_reg_final_lut_edge_x0,
 *  098._[TNR_EDF_]_0xc80043e8_[7:0__]_8_UINT8__edf[24]_:_tnr_ctrl_46,_reg_final_lut_edge_y0,
 *  099._[TNR_EDF_]_0xc80043ec_[31:24]_8_UINT8__edf[25]_:_tnr_ctrl_47,_reg_final_lut_detail_x1,
 *  100._[TNR_EDF_]_0xc80043ec_[23:16]_8_UINT8__edf[26]_:_tnr_ctrl_47,_reg_final_lut_detail_y1,
 *  101._[TNR_EDF_]_0xc80043ec_[15:8_]_8_UINT8__edf[27]_:_tnr_ctrl_47,_reg_final_lut_detail_x0,
 *  102._[TNR_EDF_]_0xc80043ec_[7:0__]_8_UINT8__edf[28]_:_tnr_ctrl_47,_reg_final_lut_detail_y0,
 *  103._[TNR_MC__]_0xc80041a0_[4____]_1_UINT8__m_c[00]_:_tnr_ctrl_24,_reg_luma_gain_en,
 *  104._[TNR_MC__]_0xc80041a0_[5____]_1_UINT8__m_c[01]_:_tnr_ctrl_24,_reg_skin_gain_en,
 *  105._[TNR_MC__]_0xc80041b4_[0____]_1_UINT8__m_c[02]_:_tnr_ctrl_29,_reg_dir_blur_en,
 *  106._[TNR_MC__]_0xc80041b4_[15:8_]_8_UINT8__m_c[03]_:_tnr_ctrl_29,_reg_dir_blur_gain,
 *  107._[TNR_MC__]_0xc80041b4_[31:24]_8_UINT8__m_c[04]_:_tnr_ctrl_29,_reg_diff_limit,
 *  108._[TNR_MC__]_0xc80041b8_[0____]_1_UINT8__m_c[05]_:_tnr_ctrl_30,_reg_denoise_filter_en,
 *  109._[TNR_MC__]_0xc80041b8_[15:8_]_8_UINT8__m_c[06]_:_tnr_ctrl_30,_reg_denoise_gain,
 *  110._[TNR_MC__]_0xc80041b8_[5:4__]_2_UINT8__m_c[07]_:_tnr_ctrl_30,_reg_denoise_cut_resolution,
 *  111._[TNR_MC__]_0xc80041b8_[22:16]_7_UINT8__m_c[08]_:_tnr_ctrl_30,_reg_denoise_G0,
 *  112._[TNR_MC__]_0xc80041bc_[28:24]_5_UINT8__m_c[09]_:_tnr_ctrl_31,_reg_denoise_G1,
 *  113._[TNR_MC__]_0xc80041bc_[20:16]_5_UINT8__m_c[10]_:_tnr_ctrl_31,_reg_denoise_G2,
 *  114._[TNR_MC__]_0xc80041bc_[12:8_]_5_UINT8__m_c[11]_:_tnr_ctrl_31,_reg_denoise_G3,
 *  115._[TNR_MC__]_0xc80041bc_[4:0__]_5_UINT8__m_c[12]_:_tnr_ctrl_31,_reg_denoise_G4,
 *  116._[TNR_MC__]_0xc80041a4_[0____]_1_UINT8__m_c[13]_:_tnr_ctrl_25,_reg_flat_y_blur_en,
 *  117._[TNR_MC__]_0xc80041a4_[5:4__]_2_UINT8__m_c[14]_:_tnr_ctrl_25,_reg_flat_y_cut_resolution,
 *  118._[TNR_MC__]_0xc80041a4_[13:8_]_6_UINT8__m_c[15]_:_tnr_ctrl_25,_reg_flat_y_G0,
 *  119._[TNR_MC__]_0xc80041a4_[31:28]_4_UINT8__m_c[16]_:_tnr_ctrl_25,_reg_flat_y_G1,
 *  120._[TNR_MC__]_0xc80041a4_[27:24]_4_UINT8__m_c[17]_:_tnr_ctrl_25,_reg_flat_y_G2,
 *  121._[TNR_MC__]_0xc80041a4_[23:20]_4_UINT8__m_c[18]_:_tnr_ctrl_25,_reg_flat_y_G3,
 *  122._[TNR_MC__]_0xc80041a4_[19:16]_4_UINT8__m_c[19]_:_tnr_ctrl_25,_reg_flat_y_G4,
 *  123._[TNR_MC__]_0xc80041a8_[0____]_1_UINT8__m_c[20]_:_tnr_ctrl_26,_reg_flat_c_blur_en,
 *  124._[TNR_MC__]_0xc80041a8_[5:4__]_2_UINT8__m_c[21]_:_tnr_ctrl_26,_reg_flat_c_cut_resolution,
 *  125._[TNR_MC__]_0xc80041a8_[13:8_]_6_UINT8__m_c[22]_:_tnr_ctrl_26,_reg_flat_c_G0,
 *  126._[TNR_MC__]_0xc80041a8_[31:28]_4_UINT8__m_c[23]_:_tnr_ctrl_26,_reg_flat_c_G1,
 *  127._[TNR_MC__]_0xc80041a8_[27:24]_4_UINT8__m_c[24]_:_tnr_ctrl_26,_reg_flat_c_G2,
 *  128._[TNR_MC__]_0xc80041a8_[23:20]_4_UINT8__m_c[25]_:_tnr_ctrl_26,_reg_flat_c_G3,
 *  129._[TNR_MC__]_0xc80041a8_[19:16]_4_UINT8__m_c[26]_:_tnr_ctrl_26,_reg_flat_c_G4,
 */
typedef struct
{
	UINT8  tnr_en;			///< tnr enable(ui)
	UINT32 lut_lm[16];		///< lut low margin(ui)
	UINT32 lut_ui[16];		///< lut base(ui)
	UINT32 lut_hm[16];		///< lut high margin(ui)
	UINT8  s_m[24];			///< me,sc,sm
	UINT8  edf[29];			///< edf
	UINT8  m_c[27];			///< mc
}
CHIP_TNR_DB_H15_T;

/**
 * CHIP_DNR_DB_H15_T for HAL_VPQ_SetMpegNoiseReduction
 *  001._[MAX_CTRL]_0xC8007194_[0____]_1_UINT8__common[00]:_dnr_max_ctrl,__reg_dnr_max_enable,
 *  002._[DC_BNR__]_0xC80071BC_[0____]_1_UINT8__common[01]:_dc_bnr_ctrl_0,_reg_dc_bnr_enable,
 *  003._[DC_BNR__]_0xC80071C4_[15:10]_6_UINT8__common[02]:_dc_bnr_ctrl_2,_reg_dc_bnr_mastergain,
 *  004._[DC_BNR__]_0xC80071C4_[23:16]_8_UINT8__common[03]:_dc_bnr_ctrl_2,_reg_dc_bnr_chromagain,
 *  005._[AC_BNR__]_0xC80071D4_[0____]_1_UINT8__common[04]:_ac_bnr_ctrl_0,_reg_bnr_ac_h_en,
 *  006._[AC_BNR__]_0xC80071D4_[1____]_1_UINT8__common[05]:_ac_bnr_ctrl_0,_reg_bnr_ac_v_en,
 *  007._[AC_BNR__]_0xC80071D4_[2____]_1_UINT8__common[06]:_ac_bnr_ctrl_0,_reg_bnr_ac_h_chroma_en,
 *  008._[AC_BNR__]_0xC80071D4_[3____]_1_UINT8__common[07]:_ac_bnr_ctrl_0,_reg_bnr_ac_v_chroma_en,
 *  009._[AC_BNR__]_0xC80071D4_[5:4__]_2_UINT8__common[08]:_ac_bnr_ctrl_0,_reg_bnr_ac_acness_resol_h,
 *  010._[AC_BNR__]_0xC8007204_[15:10]_6_UINT8__common[09]:_ac_bnr_ctrl_9,_reg_ac_master_gain,
 *  011._[MNR_____]_0xC80071A0_[0____]_1_UINT8__common[10]:_mnr_ctrl_0,____reg_mnr_enable,
 *  012._[MNR_____]_0xC80071A0_[15:8_]_8_UINT8__common[11]:_mnr_ctrl_0,____reg_mnr_master_gain,
 *  013._[MNR_____]_0xC80071A0_[23:16]_8_UINT8__common[12]:_mnr_ctrl_0,____reg_chroma_master_gain,
 *  014._[MNR_____]_0xC80071A8_[23:16]_8_UINT8__common[13]:_mnr_ctrl_2,____reg_mnr_v_gain,
 *  015._[MNR_____]_0xC80071A8_[31:24]_8_UINT8__common[14]:_mnr_ctrl_2,____reg_mnr_h_gain,
 *  016._[DC_BNR__]_0xC80071C4_[0____]_1_UINT8__dc_bnr[00]:_dc_bnr_ctrl_2,_reg_dc_var_en,
 *  017._[DC_BNR__]_0xC80071C4_[1____]_1_UINT8__dc_bnr[01]:_dc_bnr_ctrl_2,_reg_dc_motion_en,
 *  018._[DC_BNR__]_0xC80071C4_[2____]_1_UINT8__dc_bnr[02]:_dc_bnr_ctrl_2,_reg_dc_protection_en,
 *  019._[DC_BNR__]_0xC80071C4_[3____]_1_UINT8__dc_bnr[03]:_dc_bnr_ctrl_2,_reg_dc_detail_en,
 *  020._[DC_BNR__]_0xC80071BC_[3:2__]_2_UINT8__dc_bnr[04]:_dc_bnr_ctrl_0,_reg_dc_blur_sel,
 *  021._[DC_BNR__]_0xC80071BC_[23:16]_8_UINT8__dc_bnr[05]:_dc_bnr_ctrl_0,_reg_dc_motion_max,
 *  022._[DC_BNR__]_0xC80071BC_[31:24]_8_UINT8__dc_bnr[06]:_dc_bnr_ctrl_0,_reg_dc_motion_min,
 *  023._[DC_BNR__]_0xC80071C0_[7:0__]_8_UINT8__dc_bnr[07]:_dc_bnr_ctrl_1,_reg_dc_detail_max,
 *  024._[DC_BNR__]_0xC80071C0_[15:8_]_8_UINT8__dc_bnr[08]:_dc_bnr_ctrl_1,_reg_dc_detail_min,
 *  025._[DC_BNR__]_0xC80071C0_[23:20]_4_UINT8__dc_bnr[09]:_dc_bnr_ctrl_1,_reg_var_v_gain,
 *  026._[DC_BNR__]_0xC80071C0_[27:24]_4_UINT8__dc_bnr[10]:_dc_bnr_ctrl_1,_reg_var_h_gain,
 *  027._[DC_BNR__]_0xC80071C0_[31:28]_4_UINT8__dc_bnr[11]:_dc_bnr_ctrl_1,_reg_var_cut_resolution,
 *  028._[DC_BNR__]_0xC80071C4_[7:4__]_4_UINT8__dc_bnr[12]:_dc_bnr_ctrl_2,_reg_dc_global_motion_th,
 *  029._[DC_BNR__]_0xC80071C4_[31:24]_8_UINT8__dc_bnr[13]:_dc_bnr_ctrl_2,_reg_dc_protection_th,
 *  030._[DC_BNR__]_0xC80071C8_[7:0__]_8_UINT8__dc_bnr[14]:_dc_bnr_ctrl_3,_reg_dc_bnr_var_th3,
 *  031._[DC_BNR__]_0xC80071C8_[15:8_]_8_UINT8__dc_bnr[15]:_dc_bnr_ctrl_3,_reg_dc_bnr_var_th2,
 *  032._[DC_BNR__]_0xC80071C8_[23:16]_8_UINT8__dc_bnr[16]:_dc_bnr_ctrl_3,_reg_dc_bnr_var_th1,
 *  033._[DC_BNR__]_0xC80071C8_[31:24]_8_UINT8__dc_bnr[17]:_dc_bnr_ctrl_3,_reg_dc_bnr_var_th0,
 *  034._[DC_BNR__]_0xC80071C4_[8____]_1_UINT8__dc_bnr[18]:_dc_bnr_ctrl_2,_reg_detail_sel,
 *  035._[DETAIL__]_0xC80071B8_[7:0__]_8_UINT8__dc_bnr[19]:_detail_ctrl,___reg_bnr_ac_detail_max,
 *  036._[DETAIL__]_0xC80071B8_[15:8_]_8_UINT8__dc_bnr[20]:_detail_ctrl,___reg_bnr_ac_detail_min,
 *  037._[DETAIL__]_0xC80071B8_[23:16]_8_UINT8__dc_bnr[21]:_detail_ctrl,___reg_bnr_diff_l,
 *  038._[DETAIL__]_0xC80071B8_[31:24]_8_UINT8__dc_bnr[22]:_detail_ctrl,___reg_bnr_diff_p,
 *  039._[SC_BNR__]_0xC800726C_[0____]_1_UINT8__dc_bnr[23]:_sc_bnr_ctrl_0,_reg_sc_bnr_en,
 *  040._[SC_BNR__]_0xC8007280_[7:0__]_8_UINT8__dc_bnr[24]:_sc_bnr_ctrl_5,_reg_pos_gain_3,
 *  041._[SC_BNR__]_0xC8007280_[15:8_]_8_UINT8__dc_bnr[25]:_sc_bnr_ctrl_5,_reg_pos_gain_2,
 *  042._[SC_BNR__]_0xC8007280_[23:16]_8_UINT8__dc_bnr[26]:_sc_bnr_ctrl_5,_reg_pos_gain_1,
 *  043._[TEXTURE_]_0xC800728C_[2:0__]_3_UINT8__dc_bnr[27]:_texture_ctrl_0,reg_texture_cnt_div,
 *  044._[TEXTURE_]_0xC800728C_[15:8_]_8_UINT8__dc_bnr[28]:_texture_ctrl_0,reg_texture_cnt_mul,
 *  045._[TEXTURE_]_0xC800728C_[31:24]_8_UINT8__dc_bnr[29]:_texture_ctrl_0,reg_cross_th,
 *  046._[AC_BNR__]_0xC80071D4_[15:8_]_8_UINT8__ac_bnr[00]:_ac_bnr_ctrl_0,_reg_bnr_ac_diff_min_v_th,
 *  047._[AC_BNR__]_0xC80071D4_[23:16]_8_UINT8__ac_bnr[01]:_ac_bnr_ctrl_0,_reg_bnr_ac_diff_min_h_th,
 *  048._[AC_BNR__]_0xC80071D8_[7:0__]_8_UINT8__ac_bnr[02]:_ac_bnr_ctrl_1,_reg_bnr_ac_global_motion_th,
 *  049._[AC_BNR__]_0xC80071D8_[15:8_]_8_UINT8__ac_bnr[03]:_ac_bnr_ctrl_1,_reg_bnr_ac_h_acness_max,
 *  050._[AC_BNR__]_0xC80071E8_[23:16]_8_UINT8__ac_bnr[04]:_ac_bnr_ctrl_4,_reg_bnr_ac_v_acness_max,
 *  051._[AC_BNR__]_0xC80071D8_[23:16]_8_UINT8__ac_bnr[05]:_ac_bnr_ctrl_1,_reg_bnr_ac_h_acness_min,
 *  052._[AC_BNR__]_0xC80071E8_[31:24]_8_UINT8__ac_bnr[06]:_ac_bnr_ctrl_4,_reg_bnr_ac_v_acness_min,
 *  053._[AC_BNR__]_0xC80071E8_[7:6__]_2_UINT8__ac_bnr[07]:_ac_bnr_ctrl_4,_reg_bnr_ac_acness_resol_v,
 *  054._[AC_BNR__]_0xC80071DC_[7:0__]_8_UINT8__ac_bnr[08]:_ac_bnr_ctrl_2,_reg_bnr_ac_motion_3,
 *  055._[AC_BNR__]_0xC80071DC_[15:8_]_8_UINT8__ac_bnr[09]:_ac_bnr_ctrl_2,_reg_bnr_ac_motion_2,
 *  056._[AC_BNR__]_0xC80071DC_[23:16]_8_UINT8__ac_bnr[10]:_ac_bnr_ctrl_2,_reg_bnr_ac_motion_1,
 *  057._[AC_BNR__]_0xC80071DC_[31:24]_8_UINT8__ac_bnr[11]:_ac_bnr_ctrl_2,_reg_bnr_ac_motion_0,
 *  058._[AC_BNR__]_0xC80071E4_[7:0__]_8_UINT8__ac_bnr[12]:_ac_bnr_ctrl_3,_reg_bnr_ac_motion_y_3,
 *  059._[AC_BNR__]_0xC80071E4_[15:8_]_8_UINT8__ac_bnr[13]:_ac_bnr_ctrl_3,_reg_bnr_ac_motion_y_2,
 *  060._[AC_BNR__]_0xC80071E4_[23:16]_8_UINT8__ac_bnr[14]:_ac_bnr_ctrl_3,_reg_bnr_ac_motion_y_1,
 *  061._[AC_BNR__]_0xC80071E4_[31:24]_8_UINT8__ac_bnr[15]:_ac_bnr_ctrl_3,_reg_bnr_ac_motion_y_0,
 *  062._[AC_BNR__]_0xC80071F4_[31:24]_8_UINT8__ac_bnr[16]:_ac_bnr_ctrl_6,_reg_bnr_ac_detail_gain_th,
 *  063._[AC_BNR__]_0xC80071F0_[7:0__]_8_UINT8__ac_bnr[17]:_ac_bnr_ctrl_5,_reg_bnr_ac_detail_th4,
 *  064._[AC_BNR__]_0xC80071F0_[15:8_]_8_UINT8__ac_bnr[18]:_ac_bnr_ctrl_5,_reg_bnr_ac_detail_th3,
 *  065._[AC_BNR__]_0xC80071F0_[23:16]_8_UINT8__ac_bnr[19]:_ac_bnr_ctrl_5,_reg_bnr_ac_detail_th2,
 *  066._[AC_BNR__]_0xC80071F0_[31:24]_8_UINT8__ac_bnr[20]:_ac_bnr_ctrl_5,_reg_bnr_ac_detail_th1,
 *  067._[AC_BNR__]_0xC80071F8_[31:24]_8_UINT8__ac_bnr[21]:_ac_bnr_ctrl_7,_reg_bnr_ac_pos_gain_h3,
 *  068._[AC_BNR__]_0xC80071F4_[7:0__]_8_UINT8__ac_bnr[22]:_ac_bnr_ctrl_6,_reg_bnr_ac_pos_gain_h2,
 *  069._[AC_BNR__]_0xC80071F4_[15:8_]_8_UINT8__ac_bnr[23]:_ac_bnr_ctrl_6,_reg_bnr_ac_pos_gain_h1,
 *  070._[AC_BNR__]_0xC80071F4_[23:16]_8_UINT8__ac_bnr[24]:_ac_bnr_ctrl_6,_reg_bnr_ac_pos_gain_h0,
 *  071._[AC_BNR__]_0xC80071F8_[7:0__]_8_UINT8__ac_bnr[25]:_ac_bnr_ctrl_7,_reg_bnr_ac_pos_gain_l2,
 *  072._[AC_BNR__]_0xC80071F8_[15:8_]_8_UINT8__ac_bnr[26]:_ac_bnr_ctrl_7,_reg_bnr_ac_pos_gain_l1,
 *  073._[AC_BNR__]_0xC80071F8_[23:16]_8_UINT8__ac_bnr[27]:_ac_bnr_ctrl_7,_reg_bnr_ac_pos_gain_l0,
 *  074._[MNR_____]_0xC80071A0_[5:4__]_2_UINT8__mosqnr[00]:_mnr_ctrl_0,____reg_h_expand,
 *  075._[MNR_____]_0xC80071A0_[6____]_1_UINT8__mosqnr[01]:_mnr_ctrl_0,____reg_gain_flt_size,
 *  076._[MNR_____]_0xC80071A4_[7:0__]_8_UINT8__mosqnr[02]:_mnr_ctrl_1,____reg_mnr_s1_mmd_min,
 *  077._[MNR_____]_0xC80071A4_[15:8_]_8_UINT8__mosqnr[03]:_mnr_ctrl_1,____reg_mnr_s2_ratio_min,
 *  078._[MNR_____]_0xC80071A4_[23:16]_8_UINT8__mosqnr[04]:_mnr_ctrl_1,____reg_mnr_s2_ratio_max,
 *  079._[MNR_____]_0xC80071A4_[31:24]_8_UINT8__mosqnr[05]:_mnr_ctrl_1,____reg_mnr_s2_mmd_min,
 *  080._[MNR_____]_0xC80071AC_[7:0__]_8_UINT8__mosqnr[06]:_mnr_ctrl_3,____reg_filter_x0,
 *  081._[MNR_____]_0xC80071AC_[15:8_]_8_UINT8__mosqnr[07]:_mnr_ctrl_3,____reg_filter_x1,
 *  082._[MNR_____]_0xC80071AC_[23:16]_8_UINT8__mosqnr[08]:_mnr_ctrl_3,____reg_filter_y0,
 *  083._[MNR_____]_0xC80071AC_[31:24]_8_UINT8__mosqnr[09]:_mnr_ctrl_3,____reg_filter_y1,
 *  084._[MNR_____]_0xC80071B0_[0____]_1_UINT8__mosqnr[10]:_mnr_ctrl_4,____reg_motion_mnr_en,
 *  085._[MNR_____]_0xC80071B0_[2:1__]_2_UINT8__mosqnr[11]:_mnr_ctrl_4,____reg_motion_mnr_filter,
 *  086._[MNR_____]_0xC80071B0_[4:3__]_2_UINT8__mosqnr[12]:_mnr_ctrl_4,____reg_motion_mnr_filter_c,
 *  087._[MNR_____]_0xC80071B0_[6:5__]_2_UINT8__mosqnr[13]:_mnr_ctrl_4,____reg_mnr_filter,
 *  088._[MNR_____]_0xC80071B0_[8:7__]_2_UINT8__mosqnr[14]:_mnr_ctrl_4,____reg_mnr_filter_c,
 *  089._[MNR_____]_0xC80071B0_[23:16]_8_UINT8__mosqnr[15]:_mnr_ctrl_4,____reg_mnr_motion_min,
 *  090._[MNR_____]_0xC80071B0_[31:24]_8_UINT8__mosqnr[16]:_mnr_ctrl_4,____reg_mnr_motion_max,
 */
typedef struct
{
	UINT8 common[15];	///< common(ui)
	UINT8 dc_bnr[30];	///< dc bnr
	UINT8 ac_bnr[28];	///< ac bnr
	UINT8 mosqnr[17];	///< mnr
}
CHIP_DNR_DB_H15_T;

/**
 * CHIP_SHP_DB_H15_T
 * UINT32_scl_flt_pre_idx[00]:______luma_hor_index;________///<_luma_hor_index
 * UINT32_scl_flt_pre_idx[01]:______luma_ver_index;________///<_luma_ver_index
 * UINT32_scl_flt_pre_idx[02]:______chrm_hor_index;________///<_chrm_hor_index
 * UINT32_scl_flt_pre_idx[03]:______chrm_ver_index;________///<_chrm_ver_index
 * 
 * UINT8__shp_cmn_pre[00]:__________edge_Y_filter_en;______///<_0xc800a0c8[31]___,_H
 * UINT8__shp_cmn_pre[01]:__________e_gain_max;____________///<_0xc800a0e0[5:0]__,_H
 * UINT8__shp_cmn_pre[02]:__________f_gain_max;____________///<_0xc800a0e0[13:8]_,_H
 * UINT8__shp_cmn_pre[03]:__________der_v_en;______________///<_0xc800a0f0[0]____,_V
 * UINT8__shp_cmn_pre[04]:__________der_gain_mapping;______///<_0xc800a0f0[2:1]__,_V
 * UINT8__shp_cmn_pre[05]:__________max_sel;_______________///<_0xc800a0f8[6:4]__,_V
 * UINT8__shp_cmn_pre[06]:__________mp_laplacian_weight;___///<_0xc800a178[15:8]_,_CMN
 * UINT8__shp_cmn_pre[07]:__________sp_laplacian_weight;___///<_0xc800a138[15:8]_,_CMN
 * UINT8__shp_cmn_pre[08]:__________flat_filter_en;________///<_0xc800a0cc[31]___,_CMN
 * UINT8__shp_cmn_pre[09]:__________flat_filter_type;______///<_0xc800a0cc[30]___,_CMN
 * UINT8__shp_cmn_pre[10]:__________flat_filter_gain;______///<_0xc800a0cc[29:24],_CMN
 * UINT8__shp_cmn_pre[11]:__________edf_en;________________///<_0xc800a220[0]____,_DJAG
 * UINT8__shp_cmn_pre[12]:__________center_blur_mode;______///<_0xc800a220[21:20],_DJAG
 * UINT8__shp_cmn_pre[13]:__________count_diff_th;_________///<_0xc800a220[22]___,_DJAG
 * UINT8__shp_cmn_pre[14]:__________n_avg_mode;____________///<_0xc800a220[3]____,_DJAG
 * UINT8__shp_cmn_pre[15]:__________line_variation_diff_th;///<_0xc800a220[31:24],_DJAG
 * UINT8__shp_cmn_pre[16]:__________level_th;______________///<_0xc800a224[7:0]__,_DJAG
 * UINT8__shp_cmn_pre[17]:__________protect_th;____________///<_0xc800a224[15:8]_,_DJAG
 * UINT8__shp_cmn_pre[18]:__________n_avg_gain;____________///<_0xc800a224[23:16],_DJAG
 * UINT8__shp_cmn_pre[19]:__________reg_G0_cnt_min;________///<_0xc800a228[4:0]__,_DJAG
 * UINT8__shp_cmn_pre[20]:__________reg_g0_mul;____________///<_0xc800a228[12:8]_,_DJAG
 * UINT8__shp_cmn_pre[21]:__________reg_g1_protect_min;____///<_0xc800a22c[7:0]__,_DJAG
 * UINT8__shp_cmn_pre[22]:__________reg_g1_mul;____________///<_0xc800a22c[14:8]_,_DJAG
 * 
 * UINT8__shp_ui_main_pre[00]:______mp_white_gain;_________///<_0xc800a174[6:0]
 * UINT8__shp_ui_main_pre[01]:______mp_black_gain;_________///<_0xc800a174[14:8]
 * UINT8__shp_ui_main_pre[02]:______sp_white_gain;_________///<_0xc800a134[6:0]
 * UINT8__shp_ui_main_pre[03]:______sp_black_gain;_________///<_0xc800a134[14:8]
 * UINT8__shp_ui_main_pre[04]:______mp_lap_h_mode;_________///<_0xc800a188[2:0]
 * UINT8__shp_ui_main_pre[05]:______sp_lap_h_mode;_________///<_0xc800a148[1:0]
 * UINT8__shp_ui_main_pre[06]:______mp_lap_v_mode;_________///<_0xc800a188[6:4]
 * UINT8__shp_ui_main_pre[07]:______sp_lap_v_mode;_________///<_0xc800a148[4]
 * 
 * UINT8__shp_ui_h_main_pre[00]:____reg_csft_gain;__________///<_0xc800a0c4[13:8]
 * UINT8__shp_ui_h_main_pre[01]:____edge_filter_white_gain;_///<_0xc800a0c8[13:8]
 * UINT8__shp_ui_h_main_pre[02]:____edge_filter_black_gain;_///<_0xc800a0c8[21:16]
 * UINT8__shp_ui_h_main_pre[03]:____a_gen_width;____________///<_0xc800a0b0[27:24]
 * UINT8__shp_ui_h_main_pre[04]:____mp_horizontal_gain;_____///<_0xc800a174[23:16]
 * UINT8__shp_ui_h_main_pre[05]:____sp_horizontal_gain;_____///<_0xc800a134[23:16]
 * UINT8__shp_ui_h_main_pre[06]:____e_gain_th1;_____________///<_0xc800a0dc[7:0]
 * UINT8__shp_ui_h_main_pre[07]:____e_gain_th2;_____________///<_0xc800a0dc[15:8]
 * UINT8__shp_ui_h_main_pre[08]:____f_gain_th1;_____________///<_0xc800a0dc[23:16]
 * UINT8__shp_ui_h_main_pre[09]:____f_gain_th2;_____________///<_0xc800a0dc[31:24]
 * 
 * UINT8__shp_ui_v_main_pre[00]:____bif_manual_th;__________///<_0xc800a0f0[15:8]
 * UINT8__shp_ui_v_main_pre[01]:____csft_gain;______________///<_0xc800a0f4[5:0]
 * UINT8__shp_ui_v_main_pre[02]:____gain_b;_________________///<_0xc800a0fc[6:0]
 * UINT8__shp_ui_v_main_pre[03]:____gain_w;_________________///<_0xc800a0fc[14:8]
 * UINT8__shp_ui_v_main_pre[04]:____mmd_sel;________________///<_0xc800a0f8[2:0]
 * UINT8__shp_ui_v_main_pre[05]:____mp_vertical_gain;_______///<_0xc800a174[31:24]
 * UINT8__shp_ui_v_main_pre[06]:____sp_vertical_gain;_______///<_0xc800a134[31:24]
 * UINT8__shp_ui_v_main_pre[07]:____gain_th1;_______________///<_0xc800a0f8[23:16]
 * UINT8__shp_ui_v_main_pre[08]:____gain_th2;_______________///<_0xc800a0f8[31:24]
 * 
 * UINT8__shp_map_cmn_pre[00]:______reg_cross_th_sc;________///<_0xc800a1b0[23:16],_CMN
 * UINT8__shp_map_cmn_pre[01]:______reg_cross_th;___________///<_0xc800a1b0[31:24],_CMN
 * UINT8__shp_map_cmn_pre[02]:______reg_mul_base_t;_________///<_0xc800a1d0[23:16],_CMN
 * UINT8__shp_map_cmn_pre[03]:______reg_mul_base_e;_________///<_0xc800a1d0[31:24],_CMN
 * UINT8__shp_map_cmn_pre[04]:______reg_a_map_h_size;_______///<_0xc800a1b0[1:0]__,_A_MAP
 * UINT8__shp_map_cmn_pre[05]:______reg_a_nomalize_en;______///<_0xc800a1b0[2]____,_A_MAP
 * UINT8__shp_map_cmn_pre[06]:______reg_edge_alpha_mul;_____///<_0xc800a1b4[31:24],_A_MAP
 * UINT8__shp_map_cmn_pre[07]:______reg_edge_beta_mul;______///<_0xc800a1b4[23:16],_A_MAP
 * UINT8__shp_map_cmn_pre[08]:______reg_a_lut_edge_x0;______///<_0xc800a1b8[15:8]_,_A_MAP
 * UINT8__shp_map_cmn_pre[09]:______reg_a_lut_edge_y0;______///<_0xc800a1b8[7:0]__,_A_MAP
 * UINT8__shp_map_cmn_pre[10]:______reg_a_lut_edge_x1;______///<_0xc800a1b8[31:24],_A_MAP
 * UINT8__shp_map_cmn_pre[11]:______reg_a_lut_edge_y1;______///<_0xc800a1b8[23:16],_A_MAP
 * UINT8__shp_map_cmn_pre[12]:______reg_a_lut_detail_x0;____///<_0xc800a1bc[15:8]_,_A_MAP
 * UINT8__shp_map_cmn_pre[13]:______reg_a_lut_detail_y0;____///<_0xc800a1bc[7:0]__,_A_MAP
 * UINT8__shp_map_cmn_pre[14]:______reg_a_lut_detail_x1;____///<_0xc800a1bc[31:24],_A_MAP
 * UINT8__shp_map_cmn_pre[15]:______reg_a_lut_detail_y1;____///<_0xc800a1bc[23:16],_A_MAP
 * UINT8__shp_map_cmn_pre[16]:______reg_edge_minmax_x0;_____///<_0xc800a1c8[15:8]_,_A_MAP
 * UINT8__shp_map_cmn_pre[17]:______reg_edge_minmax_y0;_____///<_0xc800a1c8[7:0]__,_A_MAP
 * UINT8__shp_map_cmn_pre[18]:______reg_edge_minmax_x1;_____///<_0xc800a1c8[31:24],_A_MAP
 * UINT8__shp_map_cmn_pre[19]:______reg_edge_minmax_y1;_____///<_0xc800a1c8[23:16],_A_MAP
 * UINT8__shp_map_cmn_pre[20]:______reg_t_cut_resolution;___///<_0xc800a1b0[5:4]__,_T_MAP
 * UINT8__shp_map_cmn_pre[21]:______reg_t_nomalize_en;______///<_0xc800a1b0[3]____,_T_MAP
 * UINT8__shp_map_cmn_pre[22]:______reg_detail_alpha_mul;___///<_0xc800a1b4[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_pre[23]:______reg_detail_beta_mul;____///<_0xc800a1b4[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_pre[24]:______reg_t_lut_edge_x0;______///<_0xc800a1c0[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_pre[25]:______reg_t_lut_edge_y0;______///<_0xc800a1c0[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_pre[26]:______reg_t_lut_edge_x1;______///<_0xc800a1c0[31:24],_T_MAP
 * UINT8__shp_map_cmn_pre[27]:______reg_t_lut_edge_y1;______///<_0xc800a1c0[23:16],_T_MAP
 * UINT8__shp_map_cmn_pre[28]:______reg_t_lut_detail_x0;____///<_0xc800a1c4[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_pre[29]:______reg_t_lut_detail_y0;____///<_0xc800a1c4[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_pre[30]:______reg_t_lut_detail_x1;____///<_0xc800a1c4[31:24],_T_MAP
 * UINT8__shp_map_cmn_pre[31]:______reg_t_lut_detail_y1;____///<_0xc800a1c4[23:16],_T_MAP
 * UINT8__shp_map_cmn_pre[32]:______reg_detail_minmax_x0;___///<_0xc800a1cc[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_pre[33]:______reg_detail_minmax_y0;___///<_0xc800a1cc[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_pre[34]:______reg_detail_minmax_x1;__///<_0xc800a1cc[31:24],_T_MAP
 * UINT8__shp_map_cmn_pre[35]:______reg_detail_minmax_y1;__///<_0xc800a1cc[23:16],_T_MAP
 * 
 * UINT8__shp_balance_pre[00]:______mp_gb_en;______________///<_0xc800a18c[0]____,_GAIN_MP
 * UINT8__shp_balance_pre[01]:______mp_db_mode;____________///<_0xc800a18c[4]____,_GAIN_MP
 * UINT8__shp_balance_pre[02]:______mp_db_x1;______________///<_0xc800a18c[15:8]_,_GAIN_MP
 * UINT8__shp_balance_pre[03]:______mp_db_x2;______________///<_0xc800a190[7:0]__,_GAIN_MP
 * UINT8__shp_balance_pre[04]:______mp_db_y1;______________///<_0xc800a18c[23:16],_GAIN_MP
 * UINT8__shp_balance_pre[05]:______mp_db_y2;______________///<_0xc800a190[15:8]_,_GAIN_MP
 * UINT8__shp_balance_pre[06]:______mp_db_y3;______________///<_0xc800a190[23:16],_GAIN_MP
 * UINT8__shp_balance_pre[07]:______sp_gb_en;______________///<_0xc800a14c[0]____,_GAIN_SP
 * UINT8__shp_balance_pre[08]:______sp_db_mode;____________///<_0xc800a14c[4]____,_GAIN_SP
 * UINT8__shp_balance_pre[09]:______sp_db_x1;______________///<_0xc800a14c[15:8]_,_GAIN_SP
 * UINT8__shp_balance_pre[10]:______sp_db_x2;______________///<_0xc800a150[7:0]__,_GAIN_SP
 * UINT8__shp_balance_pre[11]:______sp_db_y1;______________///<_0xc800a14c[23:16],_GAIN_SP
 * UINT8__shp_balance_pre[12]:______sp_db_y2;______________///<_0xc800a150[15:8]_,_GAIN_SP
 * UINT8__shp_balance_pre[13]:______sp_db_y3;______________///<_0xc800a150[23:16],_GAIN_SP
 * UINT8__shp_balance_pre[14]:______mp_lum1_x_L0;__________///<_0xc800a194[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[15]:______mp_lum1_x_L1;__________///<_0xc800a194[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[16]:______mp_lum1_x_H0;__________///<_0xc800a194[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[17]:______mp_lum1_x_H1;__________///<_0xc800a194[31:24],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[18]:______mp_lum1_y0;____________///<_0xc800a198[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[19]:______mp_lum1_y1;____________///<_0xc800a198[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[20]:______mp_lum1_y2;____________///<_0xc800a198[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[21]:______mp_lum2_x_L0;__________///<_0xc800a198[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[22]:______mp_lum2_x_L1;__________///<_0xc800a19c[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[23]:______mp_lum2_x_H0;__________///<_0xc800a19c[15:8]_,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[24]:______mp_lum2_x_H1;__________///<_0xc800a19c[23:16],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[25]:______mp_lum2_y0;____________///<_0xc800a19c[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[26]:______mp_lum2_y1;____________///<_0xc800a1a0[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[27]:______mp_lum2_y2;____________///<_0xc800a1a0[15:8]_,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[28]:______sp_lum1_x_L0;__________///<_0xc800a154[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[29]:______sp_lum1_x_L1;__________///<_0xc800a154[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[30]:______sp_lum1_x_H0;__________///<_0xc800a154[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[31]:______sp_lum1_x_H1;__________///<_0xc800a154[31:24],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[32]:______sp_lum1_y0;____________///<_0xc800a158[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[33]:______sp_lum1_y1;____________///<_0xc800a158[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[34]:______sp_lum1_y2;____________///<_0xc800a158[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_pre[35]:______sp_lum2_x_L0;__________///<_0xc800a158[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[36]:______sp_lum2_x_L1;__________///<_0xc800a15c[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[37]:______sp_lum2_x_H0;__________///<_0xc800a15c[15:8]_,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[38]:______sp_lum2_x_H1;__________///<_0xc800a15c[23:16],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[39]:______sp_lum2_y0;____________///<_0xc800a15c[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[40]:______sp_lum2_y1;____________///<_0xc800a160[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_pre[41]:______sp_lum2_y2;____________///<_0xc800a160[15:8]_,_LEVEL_OVER_SHOOT
 * 
 * UINT8__shp_ee_pre[00]:___________mp_edge_gain_b;________///<_0xc800a1d4[6:0]
 * UINT8__shp_ee_pre[01]:___________mp_edge_gain_w;________///<_0xc800a1d4[14:8]
 * UINT8__shp_ee_pre[02]:___________sp_edge_gain_b;________///<_0xc800a114[6:0]
 * UINT8__shp_ee_pre[03]:___________sp_edge_gain_w;________///<_0xc800a114[14:8]
 * 
 * UINT8__shp_de_pre[00]:___________mp_texture_gain_b;_____///<_0xc800a1d4[22:16]
 * UINT8__shp_de_pre[01]:___________mp_texture_gain_w;_____///<_0xc800a1d4[30:24]
 * UINT8__shp_de_pre[02]:___________sp_texture_gain_b;_____///<_0xc800a114[22:16]
 * UINT8__shp_de_pre[03]:___________sp_texture_gain_w;_____///<_0xc800a114[30:24]
 * 
 * UINT8__shp_ti_cmn_pre[00]:_______reg_pv_ti_enable;______///<_0xc800a200[0]__,_PTI_V
 * UINT8__shp_ti_cmn_pre[01]:_______reg_pv_mm_tap_size;____///<_0xc800a200[3:2],_PTI_V
 * UINT8__shp_ti_cmn_pre[02]:_______reg_pv_avg_tap_size;___///<_0xc800a200[5:4],_PTI_V
 * UINT8__shp_ti_cmn_pre[03]:_______reg_ph_ti_enable;______///<_0xc800a204[0]__,_PTI_H
 * UINT8__shp_ti_cmn_pre[04]:_______reg_ph_mm_tap_size;____///<_0xc800a204[3:2],_PTI_H
 * UINT8__shp_ti_cmn_pre[05]:_______reg_ph_avg_tap_size;___///<_0xc800a204[5:4],_PTI_H
 * UINT8__shp_ti_cmn_pre[06]:_______reg_s_ti_enable;_______///<_0xc800a208[0]__,_STI
 * UINT8__shp_ti_cmn_pre[07]:_______reg_s_mm_tap_size;_____///<_0xc800a208[3:2],_STI
 * UINT8__shp_ti_cmn_pre[08]:_______reg_s_avg_tap_size;____///<_0xc800a208[5:4],_STI
 * 
 * UINT8__shp_ti_ui_main_pre[00]:___reg_master_gain_pti_v;_///<_0xc800a200[15:8]
 * UINT8__shp_ti_ui_main_pre[01]:___reg_master_gain_pti_h;_///<_0xc800a204[15:8]
 * UINT8__shp_ti_ui_main_pre[02]:___reg_master_gain_sti;___///<_0xc800a208[15:8]
 * UINT8__shp_ti_ui_main_pre[03]:___reg_edge_gain_sti;_____///<_0xc800a208[31:24]
 * UINT8__shp_ti_ui_main_pre[04]:___reg_texture_gain_sti;__///<_0xc800a208[23:16]
 * 
 * UINT8__shp_tgen_cmn_pre[00]:_____tgen_on_off;___________///<_0xc800a2a0[0]
 * UINT8__shp_tgen_cmn_pre[01]:_____tgen_initial_seed_mode;///<_0xc800a2a0[4]
 * UINT8__shp_tgen_cmn_pre[02]:_____tgen_table_sel;________///<_0xc800a2a0[19:16]
 * UINT8__shp_tgen_cmn_pre[03]:_____tg_delta_H_bound;______///<_0xc800a2a4[4:0]
 * UINT8__shp_tgen_cmn_pre[04]:_____tg_delta_L_bound;______///<_0xc800a2a4[12:9]
 * UINT8__shp_tgen_cmn_pre[05]:_____tg_delta_max;__________///<_0xc800a2a4[22:16]
 * UINT8__shp_tgen_cmn_pre[06]:_____tg_delta_scale;________///<_0xc800a2a4[26:24]
 * UINT8__shp_tgen_cmn_pre[07]:_____tg_rnd_th;_____________///<_0xc800a2a8[7:0]
 * UINT8__shp_tgen_cmn_pre[08]:_____tg_delta_wcurr;________///<_0xc800a2a8[11:8]
 * UINT8__shp_tgen_cmn_pre[09]:_____tg_lc_ldr_th;__________///<_0xc800a2a8[21:16]
 * UINT8__shp_tgen_cmn_pre[10]:_____rand_x0;_______________///<_0xc800a2ac[31:0]
 * UINT8__shp_tgen_cmn_pre[11]:_____tg_dr_ratio;___________///<_0xc800a2b0[2:0]
 * UINT8__shp_tgen_cmn_pre[12]:_____tg_dr_th;______________///<_0xc800a2b0[13:8]
 * UINT8__shp_tgen_cmn_pre[13]:_____tg_mm_offset;__________///<_0xc800a2b0[20:16]
 * UINT8__shp_tgen_cmn_pre[14]:_____tg_mm_sel_mode;________///<_0xc800a2b0[25:24]
 * UINT8__shp_tgen_cmn_pre[15]:_____tg_lavg_th;____________///<_0xc800a2b4[9:0]
 * UINT8__shp_tgen_cmn_pre[16]:_____tg_gs_rnd;_____________///<_0xc800a2b4[20:16]
 * UINT8__shp_tgen_cmn_pre[17]:_____tg_gs_mm;______________///<_0xc800a2b4[28:24]
 * UINT8__shp_tgen_cmn_pre[18]:_____tg_blur_coef_x0;_______///<_0xc800a2b8[29:24]
 * UINT8__shp_tgen_cmn_pre[19]:_____tg_blur_coef_x1;_______///<_0xc800a2b8[13:8]
 * UINT8__shp_tgen_cmn_pre[20]:_____tg_blur_coef_x2;_______///<_0xc800a2b8[5:0]
 * UINT8__shp_tgen_cmn_pre[21]:_____tg_blur_coef_y0;_______///<_0xc800a2bc[29:24]
 * UINT8__shp_tgen_cmn_pre[22]:_____tg_blur_coef_y1;_______///<_0xc800a2bc[13:8]
 * UINT8__shp_tgen_cmn_pre[23]:_____tg_blur_coef_y2;_______///<_0xc800a2bc[5:0]
 * UINT8__shp_tgen_cmn_pre[24]:_____tg_gain0;______________///<_0xc800a2c4[31:28]
 * UINT8__shp_tgen_cmn_pre[25]:_____tg_gain1;______________///<_0xc800a2c4[27:24]
 * UINT8__shp_tgen_cmn_pre[26]:_____tg_gain2;______________///<_0xc800a2c4[23:20]
 * UINT8__shp_tgen_cmn_pre[27]:_____tg_gain3;______________///<_0xc800a2c4[19:16]
 * UINT8__shp_tgen_cmn_pre[28]:_____tg_gain4;______________///<_0xc800a2c4[15:12]
 * UINT8__shp_tgen_cmn_pre[29]:_____tg_gain5;______________///<_0xc800a2c4[11:8]
 * UINT8__shp_tgen_cmn_pre[30]:_____tg_gain6;______________///<_0xc800a2c4[7:4]
 * UINT8__shp_tgen_cmn_pre[31]:_____tg_gain7;______________///<_0xc800a2c4[3:0]
 * 
 * UINT8__shp_tgen_ui_main_pre[00]:_tg_map_edge_gain;______///<_0xc800a2c0[4:0]
 * UINT8__shp_tgen_ui_main_pre[01]:_tg_map_detail_gain;____///<_0xc800a2c0[12:8]
 * UINT8__shp_tgen_ui_main_pre[02]:_tg_map_offset;_________///<_0xc800a2c0[23:16]
 * UINT8__shp_tgen_ui_main_pre[03]:_tg_gs_noise;___________///<_0xc800a2c0[29:24]
 * 
 * UINT32_sr_scl_flt[00]:___________iu_scale_coeff0_0;_____///<_0xc8009000_[26:16]
 * UINT32_sr_scl_flt[01]:___________iu_scale_coeff0_1;_____///<_0xc8009000_[10:0]
 * UINT32_sr_scl_flt[02]:___________iu_scale_coeff0_2;_____///<_0xc8009004_[26:16]
 * UINT32_sr_scl_flt[03]:___________iu_scale_coeff0_3;_____///<_0xc8009004_[10:0]
 * UINT32_sr_scl_flt[04]:___________iu_scale_coeff1_0;_____///<_0xc8009008_[26:16]
 * UINT32_sr_scl_flt[05]:___________iu_scale_coeff1_1;_____///<_0xc8009008_[10:0]
 * UINT32_sr_scl_flt[06]:___________iu_scale_coeff1_2;_____///<_0xc800900C_[26:16]
 * UINT32_sr_scl_flt[07]:___________iu_scale_coeff1_3;_____///<_0xc800900C_[10:0]
 * 
 * UINT8__sr_cmn[00]:_______________lut_iu_um_grdavg00;____///<_0xc8008148_[28:24],_SC_Peaking
 * UINT8__sr_cmn[01]:_______________lut_iu_um_grdavg01;____///<_0xc8008148_[20:16],_SC_Peaking
 * UINT8__sr_cmn[02]:_______________lut_iu_um_grdavg02;____///<_0xc8008148_[12:8]
,_SC_Peaking
 * UINT8__sr_cmn[03]:_______________lut_iu_um_grdavg03;____///<_0xc8008148_[4:0]__,_SC_Peaking
 * UINT8__sr_cmn[04]:_______________lut_iu_um_grdavg04;____///<_0xc800814c_[28:24],_SC_Peaking
 * UINT8__sr_cmn[05]:_______________lut_iu_um_grdavg05;____///<_0xc800814c_[20:16],_SC_Peaking
 * UINT8__sr_cmn[06]:_______________lut_iu_um_grdavg06;____///<_0xc800814c_[12:8] ,_SC_Peaking
 * UINT8__sr_cmn[07]:_______________lut_iu_um_grdavg07;____///<_0xc800814c_[4:0] _,_SC_Peaking
 * UINT8__sr_cmn[08]:_______________lut_iu_um_grdavg08;____///<_0xc8008150_[28:24],_SC_Peaking
 * UINT8__sr_cmn[09]:_______________lut_iu_um_grdavg09;____///<_0xc8008150_[20:16],_SC_Peaking
 * UINT8__sr_cmn[10]:_______________lut_iu_um_grdavg10;____///<_0xc8008150_[12:8]_,_SC_Peaking
 * UINT8__sr_cmn[11]:_______________lut_iu_um_grdavg11;____///<_0xc8008150_[4:0]__,_SC_Peaking
 * UINT8__sr_cmn[12]:_______________lut_iu_um_grdavg12;____///<_0xc8008154_[28:24],_SC_Peaking
 * UINT8__sr_cmn[13]:_______________lut_iu_um_grdavg13;____///<_0xc8008154_[20:16],_SC_Peaking
 * UINT8__sr_cmn[14]:_______________lut_iu_um_grdavg14;____///<_0xc8008154_[12:8]_,_SC_Peaking
 * UINT8__sr_cmn[15]:_______________lut_iu_um_grdavg15;____///<_0xc8008154_[4:0]__,_SC_Peaking
 * UINT8__sr_cmn[16]:_______________lut_id_um_grdavg00;____///<_0xc8008100_[28:24],_MC_Peaking
 * UINT8__sr_cmn[17]:_______________lut_id_um_grdavg01;____///<_0xc8008100_[20:16],_MC_Peaking
 * UINT8__sr_cmn[18]:_______________lut_id_um_grdavg02;____///<_0xc8008100_[12:8]_,_MC_Peaking
 * UINT8__sr_cmn[19]:_______________lut_id_um_grdavg03;____///<_0xc8008100_[4:0]__,_MC_Peaking
 * UINT8__sr_cmn[20]:_______________lut_id_um_grdavg04;____///<_0xc8008104_[28:24],_MC_Peaking
 * UINT8__sr_cmn[21]:_______________lut_id_um_grdavg05;____///<_0xc8008104_[20:16],_MC_Peaking
 * UINT8__sr_cmn[22]:_______________lut_id_um_grdavg06;____///<_0xc8008104_[12:8]_,_MC_Peaking
 * UINT8__sr_cmn[23]:_______________lut_id_um_grdavg07;____///<_0xc8008104_[4:0]__,_MC_Peaking
 * UINT8__sr_cmn[24]:_______________lut_id_um_grdavg08;____///<_0xc8008108_[28:24],_MC_Peaking
 * UINT8__sr_cmn[25]:_______________lut_id_um_grdavg09;____///<_0xc8008108_[20:16],_MC_Peaking
 * UINT8__sr_cmn[26]:_______________lut_id_um_grdavg10;____///<_0xc8008108_[12:8]_,_MC_Peaking
 * UINT8__sr_cmn[27]:_______________lut_id_um_grdavg11;____///<_0xc8008108_[4:0]__,_MC_Peaking
 * UINT8__sr_cmn[28]:_______________lut_id_um_grdavg12;____///<_0xc800810c_[28:24],_MC_Peaking
 * UINT8__sr_cmn[29]:_______________lut_id_um_grdavg13;____///<_0xc800810c_[20:16],_MC_Peaking
 * UINT8__sr_cmn[30]:_______________lut_id_um_grdavg14;____///<_0xc800810c_[12:8]_,_MC_Peaking
 * UINT8__sr_cmn[31]:_______________lut_id_um_grdavg15;____///<_0xc800810c_[4:0]__,_MC_Peaking
 * UINT8__sr_cmn[32]:_______________we_jone_base;__________///<_0xc80081b0_[15:8]_,_MC
 * UINT8__sr_cmn[33]:_______________we_jone_min;___________///<_0xc80081b0_[7:0]__,_MC
 * UINT8__sr_cmn[34]:_______________we_jone_max;___________///<_0xc80081b4_[7:0]__,_MC
 * 
 * UINT8__sr_ui_main[00]:___________use_iu_en;_____________///<_0xc800904c_[0:0]__,_EMP
 * UINT8__sr_ui_main[01]:___________iu_en_4mm_min_th;______///<_0xc8009050_[20:16],_EMP
 * UINT8__sr_ui_main[02]:___________iu_en_4mm_max_th;______///<_0xc8009050_[4:0]__,_EMP
 * UINT8__sr_ui_main[03]:___________lue_delta_c0;__________///<_0xc8009038_[28:24],_EMP
 * UINT8__sr_ui_main[04]:___________lue_delta_c1;__________///<_0xc8009038_[20:16],_EMP
 * UINT8__sr_ui_main[05]:___________lue_delta_c2;__________///<_0xc8009038_[12:8]_,_EMP
 * UINT8__sr_ui_main[06]:___________lue_delta_c3;__________///<_0xc8009038_[4:0]__,_EMP
 * UINT8__sr_ui_main[07]:___________lue_delta_c4;__________///<_0xc800903c_[28:24],_EMP
 * UINT8__sr_ui_main[08]:___________lue_delta_c5;__________///<_0xc800903c_[20:16],_EMP
 * UINT8__sr_ui_main[09]:___________lue_delta_c6;__________///<_0xc800903c_[12:8]_,_EMP
 * UINT8__sr_ui_main[10]:___________lue_delta_c7;__________///<_0xc800903c_[4:0]__,_EMP
 * UINT8__sr_ui_main[11]:___________lue_delta_d0;__________///<_0xc8009040_[28:24],_EMP
 * UINT8__sr_ui_main[12]:___________lue_delta_d1;__________///<_0xc8009040_[20:16],_EMP
 * UINT8__sr_ui_main[13]:___________lue_delta_d2;__________///<_0xc8009040_[12:8]_,_EMP
 * UINT8__sr_ui_main[14]:___________lue_delta_d3;__________///<_0xc8009040_[4:0]__,_EMP
 * UINT8__sr_ui_main[15]:___________lue_delta_d4;__________///<_0xc8009044_[28:24],_EMP
 * UINT8__sr_ui_main[16]:___________lue_delta_d5;__________///<_0xc8009044_[20:16],_EMP
 * UINT8__sr_ui_main[17]:___________lue_delta_d6;__________///<_0xc8009044_[12:8]_,_EMP
 * UINT8__sr_ui_main[18]:___________lue_delta_d7;__________///<_0xc8009044_[4:0]__,_EMP
 * UINT8__sr_ui_main[19]:___________we_dh_range0;__________///<_0xc8008110_[24:16],_MC_Peaking
 * UINT8__sr_ui_main[20]:___________we_dh_range1;__________///<_0xc8008110_[8:0]__,_MC_Peaking
 * UINT8__sr_ui_main[21]:___________we_uh_range0;__________///<_0xc8008158_[24:16],_SC_Peaking
 * UINT8__sr_ui_main[22]:___________we_uh_range1;__________///<_0xc8008158_[8:0]__,_SC_Peaking
 * UINT8__sr_ui_main[23]:___________we_id_whq_base;________///<_0xc8008214_[7:0]__,_FUSION
 * UINT8__sr_ui_main[24]:___________we_id_range0;__________///<_0xc8009054_[7:0]__,_FUSION
 * UINT8__sr_ui_main[25]:___________we_id_range1;__________///<_0xc8009058_[7:0]__,_FUSION
 * 
 * UINT32_scl_flt_post_idx[00]:_____luma_hor_index;________///<_luma_hor_index
 * UINT32_scl_flt_post_idx[01]:_____luma_ver_index;________///<_luma_ver_index
 * UINT32_scl_flt_post_idx[02]:_____chrm_hor_index;________///<_chrm_hor_index
 * UINT32_scl_flt_post_idx[03]:_____chrm_ver_index;________///<_chrm_ver_index
 * 
 * UINT8__shp_cmn_post[00]:_________edge_Y_filter_en;______///<_0xc800d4c8[31]___,_H
 * UINT8__shp_cmn_post[01]:_________e_gain_max;____________///<_0xc800d4e0[5:0]__,_H
 * UINT8__shp_cmn_post[02]:_________f_gain_max;____________///<_0xc800d4e0[13:8]_,_H
 * UINT8__shp_cmn_post[03]:_________der_v_en;______________///<_0xc800d4f0[0]____,_V
 * UINT8__shp_cmn_post[04]:_________der_gain_mapping;______///<_0xc800d4f0[2:1]__,_V
 * UINT8__shp_cmn_post[05]:_________max_sel;_______________///<_0xc800d4f8[6:4]__,_V
 * UINT8__shp_cmn_post[06]:_________mp_laplacian_weight;___///<_0xc800d578[15:8]_,_CMN
 * UINT8__shp_cmn_post[07]:_________sp_laplacian_weight;___///<_0xc800d538[15:8]_,_CMN
 * UINT8__shp_cmn_post[08]:_________flat_filter_en;________///<_0xc800d4cc[31]___,_CMN
 * UINT8__shp_cmn_post[09]:_________flat_filter_type;______///<_0xc800d4cc[30]___,_CMN
 * UINT8__shp_cmn_post[10]:_________flat_filter_gain;______///<_0xc800d4cc[29:24],_CMN
 * UINT8__shp_cmn_post[11]:_________edf_en;________________///<_0xc800d620[0]____,_DJAG
 * UINT8__shp_cmn_post[12]:_________center_blur_mode;______///<_0xc800d620[21:20],_DJAG
 * UINT8__shp_cmn_post[13]:_________count_diff_th;_________///<_0xc800d620[22]___,_DJAG
 * UINT8__shp_cmn_post[14]:_________n_avg_mode;____________///<_0xc800d620[3]____,_DJAG
 * UINT8__shp_cmn_post[15]:_________line_variation_diff_th;///<_0xc800d620[31:24],_DJAG
 * UINT8__shp_cmn_post[16]:_________level_th;______________///<_0xc800d624[7:0]__,_DJAG
 * UINT8__shp_cmn_post[17]:_________protect_th;____________///<_0xc800d624[15:8]_,_DJAG
 * UINT8__shp_cmn_post[18]:_________n_avg_gain;____________///<_0xc800d624[23:16],_DJAG
 * UINT8__shp_cmn_post[19]:_________reg_G0_cnt_min;________///<_0xc800d628[4:0]__,_DJAG
 * UINT8__shp_cmn_post[20]:_________reg_g0_mul;____________///<_0xc800d628[12:8]_,_DJAG
 * UINT8__shp_cmn_post[21]:_________reg_g1_protect_min;____///<_0xc800d62c[7:0]__,_DJAG
 * UINT8__shp_cmn_post[22]:_________reg_g1_mul;____________///<_0xc800d62c[14:8]_,_DJAG
 * 
 * UINT8__shp_ui_main_post[00]:_____mp_white_gain;_________///<_0xc800d574[6:0]
 * UINT8__shp_ui_main_post[01]:_____mp_black_gain;_________///<_0xc800d574[14:8]
 * UINT8__shp_ui_main_post[02]:_____sp_white_gain;_________///<_0xc800d534[6:0]
 * UINT8__shp_ui_main_post[03]:_____sp_black_gain;_________///<_0xc800d534[14:8]
 * UINT8__shp_ui_main_post[04]:_____mp_lap_h_mode;_________///<_0xc800d588[2:0]
 * UINT8__shp_ui_main_post[05]:_____sp_lap_h_mode;_________///<_0xc800d548[1:0]
 * UINT8__shp_ui_main_post[06]:_____mp_lap_v_mode;_________///<_0xc800d588[6:4]
 * UINT8__shp_ui_main_post[07]:_____sp_lap_v_mode;_________///<_0xc800d548[4]
 * 
 * UINT8__shp_ui_h_main_post[00]:___reg_csft_gain;_________///<_0xc800d4c4[13:8]
 * UINT8__shp_ui_h_main_post[01]:___edge_filter_white_gain;///<_0xc800d4c8[13:8]
 * UINT8__shp_ui_h_main_post[02]:___edge_filter_black_gain;///<_0xc800d4c8[21:16]
 * UINT8__shp_ui_h_main_post[03]:___a_gen_width;___________///<_0xc800d4b0[27:24]
 * UINT8__shp_ui_h_main_post[04]:___mp_horizontal_gain;____///<_0xc800d574[23:16]
 * UINT8__shp_ui_h_main_post[05]:___sp_horizontal_gain;____///<_0xc800d534[23:16]
 * UINT8__shp_ui_h_main_post[06]:___e_gain_th1;____________///<_0xc800d4dc[7:0]
 * UINT8__shp_ui_h_main_post[07]:___e_gain_th2;____________///<_0xc800d4dc[15:8]
 * UINT8__shp_ui_h_main_post[08]:___f_gain_th1;____________///<_0xc800d4dc[23:16]
 * UINT8__shp_ui_h_main_post[09]:___f_gain_th2;____________///<_0xc800d4dc[31:24]
 * 
 * UINT8__shp_ui_v_main_post[00]:___bif_manual_th;_________///<_0xc800d4f0[15:8]
 * UINT8__shp_ui_v_main_post[01]:___csft_gain;_____________///<_0xc800d4f4[5:0]
 * UINT8__shp_ui_v_main_post[02]:___gain_b;________________///<_0xc800d4fc[6:0]
 * UINT8__shp_ui_v_main_post[03]:___gain_w;________________///<_0xc800d4fc[14:8]
 * UINT8__shp_ui_v_main_post[04]:___mmd_sel;_______________///<_0xc800d4f8[2:0]
 * UINT8__shp_ui_v_main_post[05]:___mp_vertical_gain;______///<_0xc800d574[31:24]
 * UINT8__shp_ui_v_main_post[06]:___sp_vertical_gain;______///<_0xc800d534[31:24]
 * UINT8__shp_ui_v_main_post[07]:___gain_th1;______________///<_0xc800d4f8[23:16]
 * UINT8__shp_ui_v_main_post[08]:___gain_th2;______________///<_0xc800d4f8[31:24]
 * 
 * UINT8__shp_map_cmn_post[00]:_____reg_cross_th_sc;_______///<_0xc800d5b0[23:16],_CMN
 * UINT8__shp_map_cmn_post[01]:_____reg_cross_th;__________///<_0xc800d5b0[31:24],_CMN
 * UINT8__shp_map_cmn_post[02]:_____reg_mul_base_t;________///<_0xc800d5d0[23:16],_CMN
 * UINT8__shp_map_cmn_post[03]:_____reg_mul_base_e;________///<_0xc800d5d0[31:24],_CMN
 * UINT8__shp_map_cmn_post[04]:_____reg_a_map_h_size;______///<_0xc800d5b0[1:0]__,_A_MAP
 * UINT8__shp_map_cmn_post[05]:_____reg_a_nomalize_en;_____///<_0xc800d5b0[2]____,_A_MAP
 * UINT8__shp_map_cmn_post[06]:_____reg_edge_alpha_mul;____///<_0xc800d5b4[31:24],_A_MAP
 * UINT8__shp_map_cmn_post[07]:_____reg_edge_beta_mul;_____///<_0xc800d5b4[23:16],_A_MAP
 * UINT8__shp_map_cmn_post[08]:_____reg_a_lut_edge_x0;_____///<_0xc800d5b8[15:8]_,_A_MAP
 * UINT8__shp_map_cmn_post[09]:_____reg_a_lut_edge_y0;_____///<_0xc800d5b8[7:0]__,_A_MAP
 * UINT8__shp_map_cmn_post[10]:_____reg_a_lut_edge_x1;_____///<_0xc800d5b8[31:24],_A_MAP
 * UINT8__shp_map_cmn_post[11]:_____reg_a_lut_edge_y1;_____///<_0xc800d5b8[23:16],_A_MAP
 * UINT8__shp_map_cmn_post[12]:_____reg_a_lut_detail_x0;___///<_0xc800d5bc[15:8]_,_A_MAP
 * UINT8__shp_map_cmn_post[13]:_____reg_a_lut_detail_y0;___///<_0xc800d5bc[7:0]__,_A_MAP
 * UINT8__shp_map_cmn_post[14]:_____reg_a_lut_detail_x1;___///<_0xc800d5bc[31:24],_A_MAP
 * UINT8__shp_map_cmn_post[15]:_____reg_a_lut_detail_y1;___///<_0xc800d5bc[23:16],_A_MAP
 * UINT8__shp_map_cmn_post[16]:_____reg_edge_minmax_x0;____///<_0xc800d5c8[15:8]_,_A_MAP
 * UINT8__shp_map_cmn_post[17]:_____reg_edge_minmax_y0;____///<_0xc800d5c8[7:0]__,_A_MAP
 * UINT8__shp_map_cmn_post[18]:_____reg_edge_minmax_x1;____///<_0xc800d5c8[31:24],_A_MAP
 * UINT8__shp_map_cmn_post[19]:_____reg_edge_minmax_y1;____///<_0xc800d5c8[23:16],_A_MAP
 * UINT8__shp_map_cmn_post[20]:_____reg_t_cut_resolution;__///<_0xc800d5b0[5:4]__,_T_MAP
 * UINT8__shp_map_cmn_post[21]:_____reg_t_nomalize_en;_____///<_0xc800d5b0[3]____,_T_MAP
 * UINT8__shp_map_cmn_post[22]:_____reg_detail_alpha_mul;__///<_0xc800d5b4[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_post[23]:_____reg_detail_beta_mul;___///<_0xc800d5b4[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_post[24]:_____reg_t_lut_edge_x0;_____///<_0xc800d5c0[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_post[25]:_____reg_t_lut_edge_y0;_____///<_0xc800d5c0[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_post[26]:_____reg_t_lut_edge_x1;_____///<_0xc800d5c0[31:24],_T_MAP
 * UINT8__shp_map_cmn_post[27]:_____reg_t_lut_edge_y1;_____///<_0xc800d5c0[23:16],_T_MAP
 * UINT8__shp_map_cmn_post[28]:_____reg_t_lut_detail_x0;___///<_0xc800d5c4[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_post[29]:_____reg_t_lut_detail_y0;___///<_0xc800d5c4[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_post[30]:_____reg_t_lut_detail_x1;___///<_0xc800d5c4[31:24],_T_MAP
 * UINT8__shp_map_cmn_post[31]:_____reg_t_lut_detail_y1;___///<_0xc800d5c4[23:16],_T_MAP
 * UINT8__shp_map_cmn_post[32]:_____reg_detail_minmax_x0;__///<_0xc800d5cc[15:8]_,_T_MAP
 * UINT8__shp_map_cmn_post[33]:_____reg_detail_minmax_y0;__///<_0xc800d5cc[7:0]__,_T_MAP
 * UINT8__shp_map_cmn_post[34]:_____reg_detail_minmax_x1;__///<_0xc800d5cc[31:24],_T_MAP
 * UINT8__shp_map_cmn_post[35]:_____reg_detail_minmax_y1;__///<_0xc800d5cc[23:16],_T_MAP
 * 
 * UINT8__shp_balance_post[00]:_____mp_gb_en;______________///<_0xc800d58c[0]____,_GAIN_MP
 * UINT8__shp_balance_post[01]:_____mp_db_mode;____________///<_0xc800d58c[4]____,_GAIN_MP
 * UINT8__shp_balance_post[02]:_____mp_db_x1;______________///<_0xc800d58c[15:8]_,_GAIN_MP
 * UINT8__shp_balance_post[03]:_____mp_db_x2;______________///<_0xc800d590[7:0]__,_GAIN_MP
 * UINT8__shp_balance_post[04]:_____mp_db_y1;______________///<_0xc800d58c[23:16],_GAIN_MP
 * UINT8__shp_balance_post[05]:_____mp_db_y2;______________///<_0xc800d590[15:8]_,_GAIN_MP
 * UINT8__shp_balance_post[06]:_____mp_db_y3;______________///<_0xc800d590[23:16],_GAIN_MP
 * UINT8__shp_balance_post[07]:_____sp_gb_en;______________///<_0xc800d54c[0]____,_GAIN_SP
 * UINT8__shp_balance_post[08]:_____sp_db_mode;____________///<_0xc800d54c[4]____,_GAIN_SP
 * UINT8__shp_balance_post[09]:_____sp_db_x1;______________///<_0xc800d54c[15:8]_,_GAIN_SP
 * UINT8__shp_balance_post[10]:_____sp_db_x2;______________///<_0xc800d550[7:0]__,_GAIN_SP
 * UINT8__shp_balance_post[11]:_____sp_db_y1;______________///<_0xc800d54c[23:16],_GAIN_SP
 * UINT8__shp_balance_post[12]:_____sp_db_y2;______________///<_0xc800d550[15:8]_,_GAIN_SP
 * UINT8__shp_balance_post[13]:_____sp_db_y3;______________///<_0xc800d550[23:16],_GAIN_SP
 * UINT8__shp_balance_post[14]:_____mp_lum1_x_L0;__________///<_0xc800d594[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[15]:_____mp_lum1_x_L1;__________///<_0xc800d594[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[16]:_____mp_lum1_x_H0;__________///<_0xc800d594[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[17]:_____mp_lum1_x_H1;__________///<_0xc800d594[31:24],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[18]:_____mp_lum1_y0;____________///<_0xc800d598[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[19]:_____mp_lum1_y1;____________///<_0xc800d598[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[20]:_____mp_lum1_y2;____________///<_0xc800d598[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[21]:_____mp_lum2_x_L0;__________///<_0xc800d598[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[22]:_____mp_lum2_x_L1;__________///<_0xc800d59c[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[23]:_____mp_lum2_x_H0;__________///<_0xc800d59c[15:8]_,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[24]:_____mp_lum2_x_H1;__________///<_0xc800d59c[23:16],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[25]:_____mp_lum2_y0;____________///<_0xc800d59c[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[26]:_____mp_lum2_y1;____________///<_0xc800d5a0[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[27]:_____mp_lum2_y2;____________///<_0xc800d5a0[15:8]_,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[28]:_____sp_lum1_x_L0;__________///<_0xc800d554[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[29]:_____sp_lum1_x_L1;__________///<_0xc800d554[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[30]:_____sp_lum1_x_H0;__________///<_0xc800d554[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[31]:_____sp_lum1_x_H1;__________///<_0xc800d554[31:24],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[32]:_____sp_lum1_y0;____________///<_0xc800d558[7:0]__,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[33]:_____sp_lum1_y1;____________///<_0xc800d558[15:8]_,_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[34]:_____sp_lum1_y2;____________///<_0xc800d558[23:16],_LEVEL_UNDER_SHOOT
 * UINT8__shp_balance_post[35]:_____sp_lum2_x_L0;__________///<_0xc800d558[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[36]:_____sp_lum2_x_L1;__________///<_0xc800d55c[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[37]:_____sp_lum2_x_H0;__________///<_0xc800d55c[15:8]_,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[38]:_____sp_lum2_x_H1;__________///<_0xc800d55c[23:16],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[39]:_____sp_lum2_y0;____________///<_0xc800d55c[31:24],_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[40]:_____sp_lum2_y1;____________///<_0xc800d560[7:0]__,_LEVEL_OVER_SHOOT
 * UINT8__shp_balance_post[41]:_____sp_lum2_y2;____________///<_0xc800d560[15:8]_,_LEVEL_OVER_SHOOT
 * 
 * UINT8__shp_ee_post[00]:__________mp_edge_gain_b;________///<_0xc800d5d4[6:0]
 * UINT8__shp_ee_post[01]:__________mp_edge_gain_w;________///<_0xc800d5d4[14:8]
 * UINT8__shp_ee_post[02]:__________sp_edge_gain_b;________///<_0xc800d514[6:0]
 * UINT8__shp_ee_post[03]:__________sp_edge_gain_w;________///<_0xc800d514[14:8]
 * 
 * UINT8__shp_de_post[00]:__________mp_texture_gain_b;_____///<_0xc800d5d4[22:16]
 * UINT8__shp_de_post[01]:__________mp_texture_gain_w;_____///<_0xc800d5d4[30:24]
 * UINT8__shp_de_post[02]:__________sp_texture_gain_b;_____///<_0xc800d514[22:16]
 * UINT8__shp_de_post[03]:__________sp_texture_gain_w;_____///<_0xc800d514[30:24]
 * 
 * UINT8__shp_ti_cmn_post[00]:______reg_pv_ti_enable;______///<_0xc800d600[0]__,_PTI_V
 * UINT8__shp_ti_cmn_post[01]:______reg_pv_mm_tap_size;____///<_0xc800d600[3:2],_PTI_V
 * UINT8__shp_ti_cmn_post[02]:______reg_pv_avg_tap_size;___///<_0xc800d600[5:4],_PTI_V
 * UINT8__shp_ti_cmn_post[03]:______reg_ph_ti_enable;______///<_0xc800d604[0]__,_PTI_H
 * UINT8__shp_ti_cmn_post[04]:______reg_ph_mm_tap_size;____///<_0xc800d604[3:2],_PTI_H
 * UINT8__shp_ti_cmn_post[05]:______reg_ph_avg_tap_size;___///<_0xc800d604[5:4],_PTI_H
 * UINT8__shp_ti_cmn_post[06]:______reg_s_ti_enable;_______///<_0xc800d608[0]__,_STI
 * UINT8__shp_ti_cmn_post[07]:______reg_s_mm_tap_size;_____///<_0xc800d608[3:2],_STI
 * UINT8__shp_ti_cmn_post[08]:______reg_s_avg_tap_size;_____///<_0xc800d608[5:4],_STI
 * 
 * UINT8__shp_ti_ui_main_post[00]:__reg_master_gain_pti_v;__///<_0xc800d600[15:8]
 * UINT8__shp_ti_ui_main_post[01]:__reg_master_gain_pti_h;_///<_0xc800d604[15:8]
 * UINT8__shp_ti_ui_main_post[02]:__reg_master_gain_sti;___///<_0xc800d608[15:8]
 * UINT8__shp_ti_ui_main_post[03]:__reg_edge_gain_sti;_____///<_0xc800d608[31:24]
 * UINT8__shp_ti_ui_main_post[04]:__reg_texture_gain_sti;___///<_0xc800d608[23:16]
 * 
 * UINT8__shp_cti_cmn_post[00]:_____cti_en;________________///<_0xc800d630[0]
 * UINT8__shp_cti_cmn_post[01]:_____coring_th0;____________///<_0xc800d634[7:0]
 * UINT8__shp_cti_cmn_post[02]:_____coring_th1;____________///<_0xc800d634[15:8]
 * UINT8__shp_cti_cmn_post[03]:_____coring_map_filter;_____///<_0xc800d634[18:16]
 * UINT8__shp_cti_cmn_post[04]:_____coring_tap_size;_______///<_0xc800d634[22:20]
 * UINT8__shp_cti_cmn_post[05]:_____ycm_en1;_______________///<_0xc800d638[1]
 * UINT8__shp_cti_cmn_post[06]:_____ycm_band_sel;__________///<_0xc800d638[6:4]
 * UINT8__shp_cti_cmn_post[07]:_____ycm_diff_th;___________///<_0xc800d638[15:8]
 * 
 * UINT8__shp_cti_ui_main_post[00]:_cti_filter_tap_size;___///<_0xc800d630[6:4]
 * UINT8__shp_cti_ui_main_post[01]:_cti_gain;______________///<_0xc800d630[15:8]
 * UINT8__shp_cti_ui_main_post[02]:_ycm_y_gain;____________///<_0xc800d638[19:16]
 * UINT8__shp_cti_ui_main_post[03]:_ycm_c_gain;____________///<_0xc800d638[23:20]
 * 
 * UINT8__shp_tgen_cmn_post[00]:____tgen_on_off;____________///<_0xc800d6a0[0]
 * UINT8__shp_tgen_cmn_post[01]:____tgen_initial_seed_mode;_///<_0xc800d6a0[4]
 * UINT8__shp_tgen_cmn_post[02]:____tgen_table_sel;_________///<_0xc800d6a0[19:16]
 * UINT8__shp_tgen_cmn_post[03]:____tg_delta_H_bound;_______///<_0xc800d6a4[4:0]
 * UINT8__shp_tgen_cmn_post[04]:____tg_delta_L_bound;_______///<_0xc800d6a4[12:9]
 * UINT8__shp_tgen_cmn_post[05]:____tg_delta_max;___________///<_0xc800d6a4[22:16]
 * UINT8__shp_tgen_cmn_post[06]:____tg_delta_scale;_________///<_0xc800d6a4[26:24]
 * UINT8__shp_tgen_cmn_post[07]:____tg_rnd_th;______________///<_0xc800d6a8[7:0]
 * UINT8__shp_tgen_cmn_post[08]:____tg_delta_wcurr;_________///<_0xc800d6a8[11:8]
 * UINT8__shp_tgen_cmn_post[09]:____tg_lc_ldr_th;___________///<_0xc800d6a8[21:16]
 * UINT8__shp_tgen_cmn_post[10]:____rand_x0;________________///<_0xc800d6ac[31:0]
 * UINT8__shp_tgen_cmn_post[11]:____tg_dr_ratio;____________///<_0xc800d6b0[2:0]
 * UINT8__shp_tgen_cmn_post[12]:____tg_dr_th;_______________///<_0xc800d6b0[13:8]
 * UINT8__shp_tgen_cmn_post[13]:____tg_mm_offset;___________///<_0xc800d6b0[20:16]
 * UINT8__shp_tgen_cmn_post[14]:____tg_mm_sel_mode;_________///<_0xc800d6b0[25:24]
 * UINT8__shp_tgen_cmn_post[15]:____tg_lavg_th;_____________///<_0xc800d6b4[9:0]
 * UINT8__shp_tgen_cmn_post[16]:____tg_gs_rnd;______________///<_0xc800d6b4[20:16]
 * UINT8__shp_tgen_cmn_post[17]:____tg_gs_mm;_______________///<_0xc800d6b4[28:24]
 * UINT8__shp_tgen_cmn_post[18]:____tg_blur_coef_x0;________///<_0xc800d6b8[29:24]
 * UINT8__shp_tgen_cmn_post[19]:____tg_blur_coef_x1;________///<_0xc800d6b8[13:8]
 * UINT8__shp_tgen_cmn_post[20]:____tg_blur_coef_x2;________///<_0xc800d6b8[5:0]
 * UINT8__shp_tgen_cmn_post[21]:____tg_blur_coef_y0;________///<_0xc800d6bc[29:24]
 * UINT8__shp_tgen_cmn_post[22]:____tg_blur_coef_y1;________///<_0xc800d6bc[13:8]
 * UINT8__shp_tgen_cmn_post[23]:____tg_blur_coef_y2;________///<_0xc800d6bc[5:0]
 * UINT8__shp_tgen_cmn_post[24]:____tg_gain0;_______________///<_0xc800d6c4[31:28]
 * UINT8__shp_tgen_cmn_post[25]:____tg_gain1;_______________///<_0xc800d6c4[27:24]
 * UINT8__shp_tgen_cmn_post[26]:____tg_gain2;_______________///<_0xc800d6c4[23:20]
 * UINT8__shp_tgen_cmn_post[27]:____tg_gain3;_______________///<_0xc800d6c4[19:16]
 * UINT8__shp_tgen_cmn_post[28]:____tg_gain4;_______________///<_0xc800d6c4[15:12]
 * UINT8__shp_tgen_cmn_post[29]:____tg_gain5;_______________///<_0xc800d6c4[11:8]
 * UINT8__shp_tgen_cmn_post[30]:____tg_gain6;_______________///<_0xc800d6c4[7:4]
 * UINT8__shp_tgen_cmn_post[31]:____tg_gain7;_______________///<_0xc800d6c4[3:0]
 * 
 * UINT8__shp_tgen_ui_main_post[00]:tg_map_edge_gain;_______///<_0xc800d6c0[4:0]
 * UINT8__shp_tgen_ui_main_post[01]:tg_map_detail_gain;_____///<_0xc800d6c0[12:8]
 * UINT8__shp_tgen_ui_main_post[02]:tg_map_offset;__________///<_0xc800d6c0[23:16]
 * UINT8__shp_tgen_ui_main_post[03]:tg_gs_noise;____________///<_0xc800d6c0[29:24]
 */
typedef struct
{
	UINT32 scl_flt_pre_idx[4];          ///< scl_flt_pre_idx[4];
	UINT8  shp_cmn_pre[23];             ///< shp_cmn_pre[23];
	UINT8  shp_ui_main_pre[8];          ///< shp_ui_main_pre[8];
	UINT8  shp_ui_h_main_pre[10];       ///< shp_ui_h_main_pre[10];
	UINT8  shp_ui_v_main_pre[9];        ///< shp_ui_v_main_pre[9];
	UINT8  shp_map_cmn_pre[36];         ///< shp_map_cmn_pre[36];
	UINT8  shp_balance_pre[42];         ///< shp_balance_pre[42];
	UINT8  shp_ee_pre[4];               ///< shp_ee_pre[4];
	UINT8  shp_de_pre[4];               ///< shp_de_pre[4];
	UINT8  shp_ti_cmn_pre[9];           ///< shp_ti_cmn_pre[9];
	UINT8  shp_ti_ui_main_pre[5];       ///< shp_ti_ui_main_pre[5];
	UINT8  shp_tgen_cmn_pre[23];        ///< shp_tgen_cmn_pre[23];
	UINT8  shp_tgen_ui_main_pre[10];     ///< shp_tgen_ui_main_pre[10];
	UINT32 scl_flt_post_idx[4];         ///< scl_flt_post_idx[4];
	UINT8  shp_cmn_post[23];            ///< shp_cmn_post[23];
	UINT8  shp_ui_main_post[8];         ///< shp_ui_main_post[8];
	UINT8  shp_ui_h_main_post[10];      ///< shp_ui_h_main_post[10];
	UINT8  shp_ui_v_main_post[9];       ///< shp_ui_v_main_post[9];
	UINT8  shp_map_cmn_post[36];        ///< shp_map_cmn_post[36];
	UINT8  shp_balance_post[42];        ///< shp_balance_post[42];
	UINT8  shp_ee_post[4];              ///< shp_ee_post[4];
	UINT8  shp_de_post[4];              ///< shp_de_post[4];
	UINT8  shp_ti_cmn_post[9];          ///< shp_ti_cmn_post[9];
	UINT8  shp_ti_ui_main_post[5];      ///< shp_ti_ui_main_post[5];
	UINT8  shp_cti_cmn_post[8];         ///< shp_cti_cmn_post[8];
	UINT8  shp_cti_ui_main_post[4];     ///< shp_cti_ui_main_post[4];
	UINT8  shp_tgen_cmn_post[23];       ///< shp_tgen_cmn_post[23];
	UINT8  shp_tgen_ui_main_post[10];    ///< shp_tgen_ui_main_post[10];
}
CHIP_SHP_DB_H15_T;

/**
 *	CHIP_SHP_EE_DB_H15_T for HAL_VPQ_SetEdgeEnhance
 */
typedef struct
{
	UINT8  shp_ee_pre[4];               ///< shp_ee_pre[4];
	UINT8  shp_ee_post[4];              ///< shp_ee_post[4];
}
CHIP_SHP_EE_DB_H15_T;

/**
 *	CHIP_SR_DB_H15_T for HAL_VPQ_SetSuperResolution
 */
typedef struct
{
	UINT32 sr_scl_flt[8];               ///< sr_scl_flt[8];
	UINT8  sr_cmn[35];                  ///< sr_cmn[35];
	UINT8  sr_ui_main[26];              ///< sr_ui_main[26];
	UINT16 sr_ui_minmax[4];             ///< sr_ui_minmax[4];
}
CHIP_SR_DB_H15_T;

/******************************************************************/

/******************************************************************
	Extern 전역변수와 함수 prototype 선언
	(Extern Variables & Function Prototype Declarations)
*******************************************************************/

#endif	/* _HAL_VPQ_DEF_H_ */

