
/******************************************************************************
*[File]				pq_gamma_if.h
*[Version]			v0.1
*[Revision Date]	2009-07-01
*[Author]			cheah
*[Description]
*    Header file of Sony PQ Gamma
*[Copyright]
*    Copyright (C) 2009 Sony All Rights Reserved.
******************************************************************************/

#ifndef PQ_GAMMA_IF_H
#define PQ_GAMMA_IF_H


/*-----------------------------------------------------------------------------
	include files
-----------------------------------------------------------------------------*/
//#include "../../../../../d_inc/d_mtk_cust.h"

#include "d_drv_cust.h"
#include "u_drv_cust.h" 

#ifdef CC_SONY_GAMMA
/*-----------------------------------------------------------------------------
	Defines
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Enum, typedef 
-----------------------------------------------------------------------------*/

enum{
	RGB_HIGHLIGHT,// 0
	RGB_CUTOFF    // 1
};

enum{
	RGB_COOL,   // 0
	RGB_NEUTRAL,// 1
	RGB_WARM1,  // 2
	RGB_WARM2,  // 3
	RGB_PHOTO   // 4
};

enum{
	RGB_RED,    // 0 
	RGB_GREEN,  // 1
	RGB_BLUE    // 2
};


typedef enum{
	WB_HIGHLIGHT,
	WB_CUTOFF,
	NUM_OF_WB_LEVEL
}WB_Lvl_t;

typedef enum{
	WB_COOL,
	WB_NEUTRAL,
	WB_WARM1,
	WB_WARM2,
	WB_PHOTO,
	NUM_OF_WB_INDEX
}WB_Index_t;

typedef enum{
	GAMMA_R,
	GAMMA_G,
	GAMMA_B,
	NUM_OF_GAMMA_BLOCK
}Gamma_RGB_t;


#define NUM_OF_WB_ITEMS NUM_OF_GAMMA_BLOCK*2		// R/G/B Gain&Offset
#define EEP_WB_MODE_GAIN_OFST_SIZE NUM_OF_WB_INDEX*NUM_OF_WB_ITEMS*2       //2bytes for everyone, 60bytes
#define EEP_WB_PBM_LUT_PER_SIZE 57*2*3

typedef enum
{	
	//ToDo--need to modify, not fix at 0xA600, need get from the system EEPROM layout enum!!
	//Attenion: if EEP_OFFSET_EMCS_PQ_START(can NOT access EEP_OFFSET_EMCS_PQ_START directly) in acfg_atv_eep.h changed, Must modify here 0xA600!
	#ifdef CC_MAPLE_CUST_DRV
	EEP_WB_START = 0xA600,										//0xA600 for Sakura, EEP WB START, PQ HEAD start
	#else
	EEP_WB_START = 0x3600,										//0x3600 for Salvia, EEP WB START, PQ HEAD start
	#endif
	EEP_WB_PQ_HEAD_END = EEP_WB_START + 4,    					//0xA600 ~ 0xA604, PQ HEAD end, EEP PQ Head len: 5 bytes
	EEP_WB_TRAK_MAGIC_NUM_START,								//0xA605~0xA60C, Tracking mode magic num00~08
	EEP_WB_BASE_GAIN_START = EEP_WB_TRAK_MAGIC_NUM_START + 8,	//0xA60D ~ 0xA61B, BASE_GAIN_R/G/B, 5 groups, 3*5=15 bytes
	EEP_WB_GAM_SEL = EEP_WB_BASE_GAIN_START + 15,				//0xA61C
	EEP_WB_OFST_START,											//0xA61D ~ 0xA6C1, WB_OFST_R/G/B, 5 groups, 11*3*5=165 bytes
	EEP_WB_INP_EASY_SET_EN = EEP_WB_OFST_START + 165,			//0xA6C2
	EEP_WB_INP_EASY_SET_LVL,									//0xA6C3
	EEP_WB_INP_LVL_START,										//0xA6C4 ~ 0xA6D9, INP_LVL_WB_00~10, 2 bytes for every one, 11*2=22 bytes
	EEP_WB_GAM_GAIN_START = EEP_WB_INP_LVL_START + 22,			//0xA6DA ~ 0xA6E4, WB_GAM_GAIN_00~10, 11bytes
	EEP_WB_GAM0_START = EEP_WB_GAM_GAIN_START + 11,				//0xA6E5 ~ 0xA705, WB_GAM0_R/G/B, 11*3=33 bytes
	EEP_WB_TRAK_VERIFY_00 = EEP_WB_GAM0_START + 33,				//0xA706
	EEP_WB_TRAK_VERIFY_01,										//0xA707
	EEP_WB_LUT_ADJ_ON,											//0xA708
	EEP_WB_PBM_MAGIC_NUM00,										//0xA709
	EEP_WB_PBM_MAGIC_NUM01,
	EEP_WB_PBM_MAGIC_NUM02,
	EEP_WB_PBM_MAGIC_NUM03,
	EEP_WB_PBM_MAGIC_NUM04,
	EEP_WB_PBM_MAGIC_NUM05,
	EEP_WB_PBM_MAGIC_NUM06,
	EEP_WB_PBM_MAGIC_NUM07,										//0xA710
	EEP_WB_LUT_ADJ_G0_START,									//0xA711 ~ 0xA866, LUT_ADJ_R/G/B00~56, 2 bytes for every one, 57*2*3=342 bytes
	EEP_WB_LUT_ADJ_G1_START = EEP_WB_LUT_ADJ_G0_START + 342,	//0xA867 ~ 0xA9BC
	EEP_WB_LUT_ADJ_G2_START = EEP_WB_LUT_ADJ_G1_START + 342,	//0xA9BD ~ 0xAB12
	EEP_WB_LUT_ADJ_G3_START = EEP_WB_LUT_ADJ_G2_START + 342,	//0xAB13 ~ 0xAC68
	EEP_WB_LUT_ADJ_G4_START = EEP_WB_LUT_ADJ_G3_START + 342,	//0xAC69 ~ 0xADBE
	EEP_WB_PBM_VERIFY_00 = EEP_WB_LUT_ADJ_G4_START + 342,		//0xADBF
	EEP_WB_PBM_VERIFY_01,										//0xADC0
	EEP_WB_MODE_GAIN_START,										//0xADC1 ~ 0xADFC, WB_MODE_GAIN/OFST_R/G/B, 5 groups, 2 byte for every one, 6*2*5=60 bytes
	EEP_WB_WBADJ_GODID = EEP_WB_MODE_GAIN_START + EEP_WB_MODE_GAIN_OFST_SIZE,			//0xADFD
	EEP_WB_WBADJ_PON_TIME,
	EEP_WB_WBADJ_JJG_VER,
	EEP_WB_WBADJ_DS_DATE = EEP_WB_WBADJ_JJG_VER + 2,
	EEP_WB_WBADJ_ADJ_DATE = EEP_WB_WBADJ_DS_DATE + 2,
	EEP_WB_WBADJ_AGING1 = EEP_WB_WBADJ_ADJ_DATE + 2,
	EEP_WB_WBADJ_AGING2 = EEP_WB_WBADJ_AGING1 + 2,
	EEP_WB_WBADJ_JJG_INFO_1 = EEP_WB_WBADJ_AGING2 + 2,
	EEP_WB_WBADJ_JJG_INFO_2 = EEP_WB_WBADJ_JJG_INFO_1 + 2,
	EEP_WB_PGAM_EN = EEP_WB_WBADJ_JJG_INFO_2 + 2,				//0xAE0D
	EEP_WB_INFO_0,												//0xAE0E ~ 0xAE17, INFO_0~4, 2 bytes for every one, 5*2=10 bytes
	EEP_WB_INFO_1 = EEP_WB_INFO_0 + 2,
	EEP_WB_INFO_2 = EEP_WB_INFO_1 + 2,
	EEP_WB_INFO_3 = EEP_WB_INFO_2 + 2,
	EEP_WB_INFO_4 = EEP_WB_INFO_3 + 2,
	EEP_WB_PBM_CTRL_POINT_SEL,									//0xAE18, 0:57 ctrl lut tbl; 1: 33 ctrl lut tbl, UNUSED, FIXED IN THE CODE!!!
	EEP_WB_10P_OFST_G0_START,									//0xAE19 ~ 0xAE39, WB_10P_OFST_R/G/B00~10, 11*3=33 bytes
	EEP_WB_10P_OFST_G1_START = EEP_WB_10P_OFST_G0_START + 33,	//0xAE3A ~ 0xAE5A
	EEP_WB_10P_OFST_G2_START = EEP_WB_10P_OFST_G1_START + 33,	//0xAE5B ~ 0xAE7B
	EEP_WB_10P_OFST_G3_START = EEP_WB_10P_OFST_G2_START + 33,	//0xAE7C ~ 0xAE9C
	EEP_WB_10P_OFST_G4_START = EEP_WB_10P_OFST_G3_START + 33,	//0xAE9D ~ 0xAEBD
	EEP_WB_GAM_10P_GAIN_START = EEP_WB_10P_OFST_G4_START + 33,	//0xAEBE ~ 0xAEC8, WB_GAM_1-P_GAIN_00~10, 11 bytes
	EEP_INP_LVL_10P_START = EEP_WB_GAM_10P_GAIN_START + 11,		//0xAEC9 ~ 0xAEDE
	EEP_WB_RESERVED_START = EEP_INP_LVL_10P_START + 22,			//0xAEDF, reserved: 298 bytes
	EEP_WB_END = EEP_WB_RESERVED_START + 297,					//0xAFFF
}E_CUST_EEP_WB_MAP;


/*-----------------------------------------------------------------------------
	Structures 
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Public functions (for Sony external src)
-----------------------------------------------------------------------------*/
extern void WB_Tracking_WriteDefaultToNVMIfNecessary(void);

/* Sony Gamma */
extern void DRVCUST_Gamma_Init(void);
//extern void DRVCUST_Gamma_TblReg(UINT8 *ptr_b, UINT8 *ptr_m, UINT8 *ptr_d, UINT16 size);
extern void DRVCUST_Gamma_ISR(void);

/* Color temperature */
extern void DRVCUST_AutoWB_Init(void);
extern void DRVCUST_AutoWB_Update(AUTO_COLORTEMP_SETS_INFO_T *aCTInfo);
extern void DRVCUST_AutoWB_Proc(void);
extern void DRVCUST_AutoWB_ISR(void);

//extern void Sony_WB_Acfg_Pic_Update_Set (UINT16 write_data);
//extern UINT16 Sony_WB_Acfg_Pic_Update_Get (void);
extern UINT16 Sony_WB_Update_ADJ_CLR_IDX_Get (void);
extern void Sony_WB_Acfg_Update_Set (UINT16 write_data);
extern UINT16 Sony_WB_Acfg_Update_Get (void);
extern WB_CLR_TEMP_DATA_T Sony_WB_Update_PARM_Get (UINT16 WB_CLR_DATA_IDX);

/*>>> START Cheah: Sony GammaWB callback func 07Sep09 */
extern void Sony_GammaSetup (UINT16 write_data);
extern void Sony_GammaSave (UINT16 write_data);
extern void Sony_GammaQuit (UINT16 write_data);
extern void Sony_GammaExit (UINT16 write_data);
extern void Sony_GammaResetNVM (void);
extern void Sony_GammaSetSample (UINT16 write_data); /* for design evaluation only */
extern void Sony_WBSetup (UINT16 write_data);
extern void Sony_WBSave (UINT16 write_data);
extern void Sony_WBQuit (UINT16 write_data);
extern void Sony_WBExit (UINT16 write_data);
extern void Sony_WBResetNVM (void);
extern void Sony_WBSetLvl (UINT16 write_data);
extern void Sony_WBSetColor (UINT16 write_data);
/*<<< END Cheah: Sony GammaWB callback func 07Sep09 */
/*>>> Lim 170909: Sony test ptn callback func*/
extern void Sony_GammaSetPatgen (BOOL uGammaPatOnOff);
extern void Sony_PreSetup (BOOL bOnOff);
#if 1//def APP_LCCT_2H
extern void Sony_Video_PreSetup (UINT16 ui2OnOff);
#endif
/*<<<Lim 170909: Sony test ptn callback func*/

/*>>> START cheah, fetch WB data from Sony (08-Sep-09) */
UINT16 DRVCUST_GetRGB(UINT8 bCTIndex, UINT8 bmode, UINT8 bRGB);
UINT16 DRVCUST_GetSonyRGB (UINT8 bCTIndex, UINT8 bmode, UINT8 bRGB);
/*<<< END cheah, fetch WB data from Sony (08-Sep-09) */

#if 1//def APP_LCCT_2H
extern UINT32 Sony_PQ_GAMMA_EDID_Write (UINT16 write_data);
#endif 

#endif /* CC_SONY_GAMMA */


#endif /* PQ_GAMMA_IF_H */

