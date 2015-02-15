
/******************************************************************************
*[File]             pq_gamma.c
*[Version]          v0.1
*[Revision Date]    2009-07-01
*[Author]           
*[Description]
*    Sony PQ gamma and white balance adj implementation.
*[Copyright]
*    Copyright (C) 2009 Sony All Rights Reserved.
******************************************************************************/

/*-----------------------------------------------------------------------------
	include files
-----------------------------------------------------------------------------*/
#include "../x_inc/x_os.h"
#include "eeprom_if.h"
#include "osd_drvif.h"
#include "../../../../../../../../../custom/Sony/2K12/res/app_util/config/acfg_atv_eep.h"
#include "../../sony_priv_inc/pq_gamma.h"
#include "wb_pbm.h"
#include "../../../../d_inc/CustomCtrl.h"
#include "drv_default.h"

#define FUNC_FROM_AP 0
extern void DRVCUST_SetPBMGammaByte(UINT8 bOnOff);
extern UINT8 DRVCUST_GetPBMGammaByte(void);
extern void vDrvGammaMute(UINT16 wGammaMuteR, UINT16 wGammaMuteG, UINT16 wGammaMuteB);
extern void vDrvGammaUnmute(void);
extern void DRVCUST_AutoColorTempUpdate(AUTO_COLORTEMP_SETS_INFO_T *aCTInfo);
#if FUNC_FROM_AP
extern void vUser_ColorTemp_Read(UINT16* param);   //function from ap
extern void vUser_ColorTemp_Exec(VOID);            //function from ap
extern INT32 a_amb_pause_app (const CHAR* ps_name);   //function from ap
#endif
extern void vDrvSceDbgInfo(UINT16 u2PosX, UINT16 u2PosY, UINT16 u2YCbCrIn[3], UINT16 u2YCbCrOut[3]); // allen add
extern void CheckToEnablePBM(void);
extern void DRVCUST_TurnOnInkMark(UINT8 ui1_onOff);
extern void DRVCUST_SetInkMark(UINT16 ui2_x, UINT16 ui2_y);
extern void vGammaOSDControl(BOOL* param);
#ifdef CUST_SOEM_DRV 
extern INT16 GPIO_SetOut(INT16 i4GpioNum, INT16 i4Val);
#endif //CUST_SOEM_DRV
extern void DRVCUST_SetAgingFlag(UINT8 bOnOff);

/*-----------------------------------------------------------------------------
	Defines
-----------------------------------------------------------------------------*/
#define NEW_TCON_MAP
#define EEP_WB_PBM_MAGIC_LEN        (8)
#define EEP_WB_PBM_MAGIC0           (0x10)
#define EEP_WB_PBM_MAGIC1           (0x20)
#ifdef NEW_TCON_MAP
#define WB_PBM_START_ADDR           (0x03C0)
#else
#define WB_PBM_START_ADDR           (0x01C0)
#endif
#define SIF_ERROR_RETURN            (0)
#define BUS_ID                      (1)
#define TCON_NVM_SLVADDR            (0xAA)
#define CLK_DIV                     (0x100)
#define ADDR_LENGTH                 (2)

/*-----------------------------------------------------------------------------
	N V M   Defines
-----------------------------------------------------------------------------*/
#define EEP_WB_PBM_MAGIC_OFFSET         EEP_OFFSET_EMCS_WB_PBM_START
#define EEP_WB_PBM_MAGIC_LEN            (8)

#define EEP_WB_PBM_COEF_OFFSET          (EEP_WB_PBM_MAGIC_OFFSET+EEP_WB_PBM_MAGIC_LEN)
#ifdef CC_MAPLE_CUST_DRV
#define EEP_WB_PBM_NUM_PT               (57)
#else
#define EEP_WB_PBM_NUM_PT               (33)
#endif
#define EEP_WB_PBM_DATALEN              (2)
#define EEP_WB_PBM_SIZE_PER_TEMP        (EEP_WB_PBM_NUM_PT*EEP_WB_PBM_DATALEN*3)
#define EEP_WB_PBM_COEF_TOTAL_SIZE      (EEP_WB_PBM_SIZE_PER_TEMP*NUM_OF_WB_INDEX)

#ifdef CUST_SOEM_DRV
#define EEP_WB_PBM_SIZE_ALL_TEMP  EEP_WB_PBM_MAGIC_LEN + (EEP_WB_PBM_SIZE_PER_TEMP *3 ) 
#define EEP_WB_VER_BYTE 			    (2)//fatin starts
#define EEP_WB_PBM_VER1					(0x57)
#define EEP_WB_PBM_VER2					(0x42)//fatin ends
#endif

#define  INP_EASY_TYPE_R 0
#define  INP_EASY_TYPE_G 1
#define  INP_EASY_TYPE_B 2
/*-----------------------------------------------------------------------------
	Structures
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global variable declarations
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
	Local variable declarations
-----------------------------------------------------------------------------*/

static UINT16 _u2_ysum_pos;
static UINT16 _u2_ysum_pixel_pos;
static UINT16 _u2_lut_adj_on = 0;
static UINT16 _u2_wb_adj_method = 0;
static UINT16 _u2_wbt_idx_change_sts;

static UINT16 _u2_setup_started = 0;
static UINT16 _u2_wbt_idx;
static UINT16 _u2_easy_set_pattern_en;
static UINT16 _u2_easy_set_pattern_level;
static UINT16 _inp_easy_white_pattern_table[EEP_WB_PBM_NUM_PT][3][NUM_OF_WB_INDEX];
#if 1  // allen add
static UINT16 _inp_easy_red_pattern_table[EEP_WB_PBM_NUM_PT][3][NUM_OF_WB_INDEX];
static UINT16 _inp_easy_green_pattern_table[EEP_WB_PBM_NUM_PT][3][NUM_OF_WB_INDEX];
static UINT16 _inp_easy_blue_pattern_table[EEP_WB_PBM_NUM_PT][3][NUM_OF_WB_INDEX];
static UINT16 _en6_inp_easy_red_value;
static UINT16 _en6_inp_easy_green_value;
static UINT16 _en6_inp_easy_blue_value;
#endif
static UINT16 _original_color_temp;

INT32 _iUsedCtlValue_R[EEP_WB_PBM_NUM_PT], _iUsedCtlValue_G[EEP_WB_PBM_NUM_PT], _iUsedCtlValue_B[EEP_WB_PBM_NUM_PT];
INT32 _iUsedColorTemp;
/*-----------------------------------------------------------------------------
	Private function prototype
-----------------------------------------------------------------------------*/
#ifdef CC_MAPLE_CUST_DRV
    static UINT16 _calculate_default_level(UINT16 ui2_level)
    {
        if(ui2_level <= 32)
        {
            return ui2_level * 32;
        }
        else if(ui2_level <= 56)
        {
            return (ui2_level - 32) * 128 + 1024;
        }
        else
        {
            return 4096;
        }
    }
#else
static UINT16 _calculate_default_level(UINT16 ui2_level)
{
    if(ui2_level <= 16)
    {
        return ui2_level * 64;
    }
    else if(ui2_level <= 32)
    {
        return (ui2_level - 16) * 192 + 1024;
    }
    else
    {
        return 4096;
    }
}
#endif
static void _reset_dram_table_to_default(UINT16 ct)
{
    UINT32 i, j;
    UINT16 u2_gray;
    if (ct >= NUM_OF_WB_INDEX)
    {
        return;
    }
    for (i=0; i<EEP_WB_PBM_NUM_PT; ++i)
    {
        u2_gray = _calculate_default_level(i);
        for (j=0; j<3; ++j)
        {
            _inp_easy_white_pattern_table[i][j][ct] = u2_gray;
        }
	 _inp_easy_red_pattern_table[i][INP_EASY_TYPE_R][ct]= u2_gray;
	 _inp_easy_red_pattern_table[i][INP_EASY_TYPE_G][ct]= 0;
	 _inp_easy_red_pattern_table[i][INP_EASY_TYPE_B][ct]= 0;

	 
        _inp_easy_green_pattern_table[i][INP_EASY_TYPE_G][ct]= u2_gray;
        _inp_easy_green_pattern_table[i][INP_EASY_TYPE_R][ct]= 0;
        _inp_easy_green_pattern_table[i][INP_EASY_TYPE_B][ct]= 0;
		
        _inp_easy_blue_pattern_table[i][INP_EASY_TYPE_B][ct]= u2_gray;
        _inp_easy_blue_pattern_table[i][INP_EASY_TYPE_R][ct]= 0;
        _inp_easy_blue_pattern_table[i][INP_EASY_TYPE_G][ct]= 0;
		
    }
}
/*-----------------------------------------------------------------------------
    Global function prototype
-----------------------------------------------------------------------------*/
#ifdef CC_Factory2K12_Debug
extern INT32 c_sprintf (CHAR*        ps_str,
                        const CHAR*  ps_format,
                        ...);
extern CHAR fact_dbg_buf[128];  
extern void fact_show_msg(void);
#define Factory2K12Debug(x)	(x); fact_show_msg();
#else
#define Factory2K12Debug(x) 
#endif 

////////////////////////////////////////////
//  adjustment
////////////////////////////////////////////
static void _easySetPatternDisplay(UINT16 ui2_en, UINT16 ui_level)
{
	  UINT16 u2_inp_easy_set_r,u2_inp_easy_set_g,u2_inp_easy_set_b;

    x_thread_delay(100);
    switch(ui2_en)
    {
        case 2:
            u2_inp_easy_set_r =_inp_easy_white_pattern_table[ui_level][INP_EASY_TYPE_R][_u2_wbt_idx];
            u2_inp_easy_set_g =_inp_easy_white_pattern_table[ui_level][INP_EASY_TYPE_G][_u2_wbt_idx];
            u2_inp_easy_set_b =_inp_easy_white_pattern_table[ui_level][INP_EASY_TYPE_B][_u2_wbt_idx];
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;

//            Factory2K12Debug(c_sprintf(fact_dbg_buf,"w easy set vDrvGammaMute (%d %d %d)\n",u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b));
            vDrvGammaMute(u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b);
            break;
#if 1
        case 3:
            u2_inp_easy_set_r =_inp_easy_red_pattern_table[ui_level][INP_EASY_TYPE_R][_u2_wbt_idx];
            u2_inp_easy_set_g =_inp_easy_red_pattern_table[ui_level][INP_EASY_TYPE_G][_u2_wbt_idx];
            u2_inp_easy_set_b =_inp_easy_red_pattern_table[ui_level][INP_EASY_TYPE_B][_u2_wbt_idx];
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;

//            Factory2K12Debug(c_sprintf(fact_dbg_buf,"r easy set vDrvGammaMute (%d %d %d)\n",u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b));
            vDrvGammaMute(u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b);
            break;

	  case 4:
            u2_inp_easy_set_r =_inp_easy_green_pattern_table[ui_level][INP_EASY_TYPE_R][_u2_wbt_idx];
            u2_inp_easy_set_g =_inp_easy_green_pattern_table[ui_level][INP_EASY_TYPE_G][_u2_wbt_idx];
            u2_inp_easy_set_b =_inp_easy_green_pattern_table[ui_level][INP_EASY_TYPE_B][_u2_wbt_idx];
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;

//            Factory2K12Debug(c_sprintf(fact_dbg_buf,"g easy set vDrvGammaMute (%d %d %d)\n",u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b));
            vDrvGammaMute(u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b);
            break;

	  case 5:
            u2_inp_easy_set_r =_inp_easy_blue_pattern_table[ui_level][INP_EASY_TYPE_R][_u2_wbt_idx];
            u2_inp_easy_set_g =_inp_easy_blue_pattern_table[ui_level][INP_EASY_TYPE_G][_u2_wbt_idx];
            u2_inp_easy_set_b =_inp_easy_blue_pattern_table[ui_level][INP_EASY_TYPE_B][_u2_wbt_idx];
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;

//            Factory2K12Debug(c_sprintf(fact_dbg_buf,"b easy set vDrvGammaMute (%d %d %d)\n",u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b));
            vDrvGammaMute(u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b);
            break;
#else// allen disable			
        case 3: case 4: case 5:
            u2_inp_easy_set_r =0 ;
            u2_inp_easy_set_g =0 ;
            u2_inp_easy_set_b =0 ;
            if(ui2_en == 3)
            {
                u2_inp_easy_set_r =_inp_easy_white_pattern_table[ui_level][INP_EASY_TYPE_R][_u2_wbt_idx];
            }
            else if (ui2_en == 4)
            {
                u2_inp_easy_set_g =_inp_easy_white_pattern_table[ui_level][INP_EASY_TYPE_G][_u2_wbt_idx];
            }
            else
            {
                u2_inp_easy_set_b =_inp_easy_white_pattern_table[ui_level][INP_EASY_TYPE_B][_u2_wbt_idx];
            }
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;
            Factory2K12Debug(c_sprintf(fact_dbg_buf,"easy set vDrvGammaMute (%d %d %d)\n",u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b));
            vDrvGammaMute(u2_inp_easy_set_r, u2_inp_easy_set_g, u2_inp_easy_set_b);
            break;
#endif			
        default:
            Factory2K12Debug(c_sprintf(fact_dbg_buf,"easy set vDrvGammaUnmute\n"));
            vDrvGammaUnmute();
            break;
    }
}
void WB_PBM_EasySetPatternLevelSet(UINT16 ui2_level)
{
    _u2_easy_set_pattern_level = ui2_level;
    _easySetPatternDisplay(_u2_easy_set_pattern_en, _u2_easy_set_pattern_level);
}
UINT16 WB_PBM_EasySetPatternLevelGet(void)
{
    return _u2_easy_set_pattern_level;
}

void WB_PBM_EasySetPatternEnableSet(UINT16 ui2_en)
{
    
    _u2_easy_set_pattern_en = ui2_en;
    if(_u2_easy_set_pattern_en == 6)
    	{
        _en6_inp_easy_red_value = 1023;
        _en6_inp_easy_green_value = 1023;
        _en6_inp_easy_blue_value = 1023;
        vDrvGammaMute(4095, 4095, 4095);
	}
    else
    	{
    	  _en6_inp_easy_red_value = 0;
	 _en6_inp_easy_green_value = 0;
	 _en6_inp_easy_blue_value = 0;
    	    _easySetPatternDisplay(_u2_easy_set_pattern_en, _u2_easy_set_pattern_level);
    	}
}
UINT16 WB_PBM_EasySetPatternEnableGet(void)
{
    return _u2_easy_set_pattern_en;
}

void WB_PBM_InpEasySetRSet(UINT16 u2_idx)
{
    UINT32 u2_inp_easy_set_r,u2_inp_easy_set_g,u2_inp_easy_set_b;
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_InpEasySetRSet(0x%x) --\n",u2_idx));    

    if(_u2_easy_set_pattern_en == 6)
    	{
        if(u2_idx > 1023)
    		{
            u2_idx = 1023;
    		}
	_en6_inp_easy_red_value =u2_idx ;
	u2_inp_easy_set_r = (_en6_inp_easy_red_value * 4096) /1023;
	u2_inp_easy_set_g = (_en6_inp_easy_green_value * 4096) /1023;
	u2_inp_easy_set_b = (_en6_inp_easy_blue_value * 4096) /1023;
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;	
       vDrvGammaMute( (UINT16) u2_inp_easy_set_r, (UINT16) u2_inp_easy_set_g, (UINT16) u2_inp_easy_set_b);
    	}
}
UINT16 WB_PBM_InpEasySetRGet(void)
{

#if 1  // allen
	return _en6_inp_easy_red_value;
#else
    if (_u2_easy_set_pattern_en == 2 || _u2_easy_set_pattern_en == 3)
    {
        return _inp_easy_white_pattern_table[_u2_easy_set_pattern_level][INP_EASY_TYPE_R][_u2_wbt_idx];
    }
    else
    {
        return 0;
    }
#endif
}
void WB_PBM_InpEasySetGSet(UINT16 u2_idx)
{
    UINT32 u2_inp_easy_set_r,u2_inp_easy_set_g,u2_inp_easy_set_b;
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_InpEasySetGSet(0x%x) --\n",u2_idx));    
    if(_u2_easy_set_pattern_en == 6)
    	{
 	   if(u2_idx > 1023)
   		 {
       	 u2_idx = 1023;
   		 }
	_en6_inp_easy_green_value =u2_idx ;
	u2_inp_easy_set_r = (_en6_inp_easy_red_value * 4096) /1023;
	u2_inp_easy_set_g = (_en6_inp_easy_green_value * 4096) /1023;
	u2_inp_easy_set_b = (_en6_inp_easy_blue_value * 4096) /1023;
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;	
       vDrvGammaMute( (UINT16) u2_inp_easy_set_r, (UINT16) u2_inp_easy_set_g, (UINT16) u2_inp_easy_set_b);
	}
 }
UINT16 WB_PBM_InpEasySetGGet(void)
{
#if 1  // allen
        return _en6_inp_easy_green_value;
#else
    if (_u2_easy_set_pattern_en == 2 || _u2_easy_set_pattern_en == 4)
    {
        return _inp_easy_white_pattern_table[_u2_easy_set_pattern_level][INP_EASY_TYPE_G][_u2_wbt_idx];
    }
    else
    {
        return 0;
    }
#endif	
}
void WB_PBM_InpEasySetBSet(UINT16 u2_idx)
{
    UINT32 u2_inp_easy_set_r,u2_inp_easy_set_g,u2_inp_easy_set_b;
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_InpEasySetBSet(0x%x) --\n",u2_idx));    
    if(_u2_easy_set_pattern_en == 6)
    	{
        if(u2_idx > 1023)
    		{
            u2_idx = 1023;
    		}
	_en6_inp_easy_blue_value =u2_idx ;
	u2_inp_easy_set_r = (_en6_inp_easy_red_value * 4096) /1023;
	u2_inp_easy_set_g = (_en6_inp_easy_green_value * 4096) /1023;
	u2_inp_easy_set_b = (_en6_inp_easy_blue_value * 4096) /1023;
            if(u2_inp_easy_set_r > 4095)
                u2_inp_easy_set_r = 4095;
            if(u2_inp_easy_set_g > 4095)
                u2_inp_easy_set_g = 4095;
            if(u2_inp_easy_set_b > 4095)
                u2_inp_easy_set_b = 4095;	
       vDrvGammaMute( (UINT16) u2_inp_easy_set_r, (UINT16) u2_inp_easy_set_g, (UINT16) u2_inp_easy_set_b);
    	}
}
UINT16 WB_PBM_InpEasySetBGet(void)
{

#if 1  // allen
          return _en6_inp_easy_blue_value;
#else
    if (_u2_easy_set_pattern_en == 2 || _u2_easy_set_pattern_en == 5)
    {
        return _inp_easy_white_pattern_table[_u2_easy_set_pattern_level][INP_EASY_TYPE_B][_u2_wbt_idx];
    }
    else
    {
        return 0;
    }
#endif
}
void WB_PBM_CoefSet(UINT32 u4_rgb_idx, UINT32 u4_pt_idx, UINT16 u2_value)
{
    if (u2_value > 4096)
    {
        u2_value = 4096;
    }
    if (  _u2_easy_set_pattern_en==2  )    
    {
        _inp_easy_white_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx] = u2_value;
    }
  else if  (_u2_easy_set_pattern_en==3 ) 
    	{
        _inp_easy_red_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx] = u2_value;
	}
  else if   (_u2_easy_set_pattern_en==4 )  
    	{
        _inp_easy_green_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx] = u2_value;
	}
  else if   (_u2_easy_set_pattern_en==5 )   
    	{
        _inp_easy_blue_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx] = u2_value;
	}
	
    _easySetPatternDisplay(_u2_easy_set_pattern_en, _u2_easy_set_pattern_level);
}

extern UINT16 WB_PBM_PQP_PGAM_LUT_Get(UINT8 ui1_rgb_idx,UINT8 ui1_lvl_idx , UINT8 wbt_index )
{
    return _inp_easy_white_pattern_table[ui1_lvl_idx][ui1_rgb_idx][wbt_index];
}

extern void WB_PBM_PQP_PGAM_LUT_Set(UINT16 *u1_pqp_lut , UINT16 wbt_index )
{
    UINT8 ui1_rgb_idx , ui1_lvl_idx,i=0;
    UINT16 ui1_PQP_GAM_LUT[EEP_WB_PBM_NUM_PT*3];

    x_memcpy(&ui1_PQP_GAM_LUT[0], (UINT16 *)u1_pqp_lut, sizeof(ui1_PQP_GAM_LUT));
		
    for(ui1_rgb_idx =0; ui1_rgb_idx <=2 ;ui1_rgb_idx++)
    {
        for(ui1_lvl_idx =0; ui1_lvl_idx <EEP_WB_PBM_NUM_PT ;ui1_lvl_idx++)
        {
            _inp_easy_white_pattern_table[ui1_lvl_idx][ui1_rgb_idx][wbt_index] = ui1_PQP_GAM_LUT[i];
            i++; 
        }
    }

    _easySetPatternDisplay(_u2_easy_set_pattern_en, _u2_easy_set_pattern_level);

}

UINT16 WB_PBM_CoefGet(UINT32 u4_rgb_idx, UINT32 u4_pt_idx)
{
#if 1 //allen
    if(u4_rgb_idx >= NUM_OF_WB_INDEX)

    {
        return 0;
    }
    if (_u2_setup_started)
    {
	    if (_u2_easy_set_pattern_en==2 ) 
    		{
		return  _inp_easy_white_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx];
		}
   	 else if (_u2_easy_set_pattern_en==3 ) 
    		{
		return  _inp_easy_red_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx];
		}
   	 else if (_u2_easy_set_pattern_en==4 ) 
    		{
		return  _inp_easy_green_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx];
		}
   	 else if (_u2_easy_set_pattern_en==5 ) 
    		{
		return  _inp_easy_blue_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx];
		}
     	else
       	 {
       	 return 0;
   		 }
    }
  else
    {
        if (u4_rgb_idx == 0)
        {
            return _iUsedCtlValue_R[u4_pt_idx];
        }
        else if (u4_rgb_idx == 1)
        {
            return _iUsedCtlValue_G[u4_pt_idx];
        }
        else
        {
            return _iUsedCtlValue_B[u4_pt_idx];
        }
    }	
#else
    if(u4_rgb_idx >= 3)

    {
        return 0;
    }
    if(u4_pt_idx >= EEP_WB_PBM_NUM_PT)
    {
        return 0;
    }
    if (_u2_setup_started)
    {
        if ((_u2_easy_set_pattern_en==2) || (_u2_easy_set_pattern_en==u4_rgb_idx+3))
        {
            return _inp_easy_white_pattern_table[u4_pt_idx][u4_rgb_idx][_u2_wbt_idx];
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (u4_rgb_idx == 0)
        {
            return _iUsedCtlValue_R[u4_pt_idx];
        }
        else if (u4_rgb_idx == 1)
        {
            return _iUsedCtlValue_G[u4_pt_idx];
        }
        else
        {
            return _iUsedCtlValue_B[u4_pt_idx];
        }
    }
#endif
}
void WB_PBM_ColorTemp_Idx_Set(UINT16 ui2ClrTempIdx)
{
  _original_color_temp = ui2ClrTempIdx;
}
UINT16 WB_PBM_ColorTemp_Idx_Get(void)
{
  return _original_color_temp;
}
////////////////////////////////////////////
//  NVM
////////////////////////////////////////////
static void _save_table_to_NVM(UINT16 u2_colorTemp)
{
    UINT64  u8Offset;
    UINT8 i, j;
    UINT8 data[EEP_WB_PBM_LUT_PER_SIZE]={0};    //EEP_WB_PBM_SIZE_PER_TEMP      weibin, Salvia's eeprom map is same as Sakura, 57 point layout
    //UINT8 *ptr;
    UINT16 u2_value;

    if( u2_colorTemp >= NUM_OF_WB_INDEX)
    {
        return;
    }
    u8Offset = (UINT64)EEP_WB_LUT_ADJ_G0_START;
    u8Offset += (UINT64)(u2_colorTemp*EEP_WB_PBM_LUT_PER_SIZE);   //EEP_WB_PBM_SIZE_PER_TEMP      weibin, Salvia's eeprom map is same as Sakura, 57 point layout
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"[]_save_table_to_NVM 0x%x 0x%x--\n",u8Offset, EEP_WB_PBM_LUT_PER_SIZE));
    //ptr = data;
    for (j=0; j<3; ++j)
    {
        for (i=0; i<EEP_WB_PBM_NUM_PT; ++i)
        {
	   if(_u2_easy_set_pattern_en == 2)
	   	{
 		        u2_value = _inp_easy_white_pattern_table[i][j][u2_colorTemp];
	   	}
	   else if(_u2_easy_set_pattern_en == 3)
	   	{
		        u2_value = _inp_easy_red_pattern_table[i][j][u2_colorTemp];
	   	}
	   else if(_u2_easy_set_pattern_en == 4)
	   	{
		        u2_value = _inp_easy_green_pattern_table[i][j][u2_colorTemp];
	   	}
	   else if(_u2_easy_set_pattern_en == 5)
	   	{
 		        u2_value = _inp_easy_blue_pattern_table[i][j][u2_colorTemp];
	   	}
	   else
	   	{
 		        u2_value = _inp_easy_white_pattern_table[i][j][u2_colorTemp];
	   	}
            data[57*2*j + i*2] = (UINT8)(u2_value & 0xFF);
	   	    data[57*2*j + i*2+1] = (UINT8)(u2_value >> 8);
            //*ptr++ = (UINT8)(u2_value & 0xFF);
            //*ptr++ = (UINT8)(u2_value >> 8);
        }
    }
    EEPROM_Write(u8Offset, (UINT32)data, EEP_WB_PBM_LUT_PER_SIZE);
}
static void _save_all_table_to_NVM(void)
{
    UINT32 i;
	UINT8  PBM_ver_byte[EEP_WB_VER_BYTE];
    for (i=0; i<NUM_OF_WB_INDEX; ++i)
    {
        _save_table_to_NVM(i);
    }
	PBM_ver_byte[0]=EEP_WB_PBM_VER1;
	PBM_ver_byte[1]=EEP_WB_PBM_VER2;
	EEPROM_Write(EEP_WB_PBM_VERIFY_00, (UINT32)PBM_ver_byte,2);	
}
static void _load_table_from_NVM(UINT16 u2_colorTemp)
{
    UINT64  u8Offset;
    UINT8 i, j;
    UINT8 data[2];
    UINT16 u2_value;
    x_memset(data, 0, 2);

    if( u2_colorTemp >= NUM_OF_WB_INDEX)
    {
        return;
    }

    //u8Offset = (UINT64)EEP_WB_LUT_ADJ_G0_START;
    //u8Offset += (UINT64)(u2_colorTemp*EEP_WB_PBM_LUT_PER_SIZE);   //EEP_WB_PBM_SIZE_PER_TEMP      weibin, Salvia's eeprom map is same as Sakura, 57 point layout
    for (j=0; j<3; ++j)
    {
        u8Offset = (UINT64)(EEP_WB_LUT_ADJ_G0_START + (u2_colorTemp*EEP_WB_PBM_LUT_PER_SIZE) + j*57*2);
        for (i=0; i<EEP_WB_PBM_NUM_PT; ++i)
        {
            EEPROM_Read(u8Offset, (UINT32)data, 2);
            u8Offset += 2;

            u2_value = data[1];
            u2_value <<= 8;
            u2_value |= data[0];
            _inp_easy_white_pattern_table[i][j][u2_colorTemp] = u2_value;
        }
    }
}
void WB_PBM_MagicStrClear(void)
{
    UINT64  u8Offset;
    UINT8   data[EEP_WB_PBM_MAGIC_LEN];

    u8Offset = EEP_WB_PBM_MAGIC_OFFSET;
    x_memset(data, 0, EEP_WB_PBM_MAGIC_LEN);

    EEPROM_Write(u8Offset, (UINT32)data, (UINT32)EEP_WB_PBM_MAGIC_LEN);
}
void WB_PBM_MagicStrSet(void)
{
    UINT64  u8Offset;
    UINT8   data[EEP_WB_PBM_MAGIC_LEN];

    u8Offset = EEP_WB_PBM_MAGIC_NUM00;
    x_memset(data, 0, EEP_WB_PBM_MAGIC_LEN);
    data[0] = EEP_WB_PBM_MAGIC0;
    data[1] = EEP_WB_PBM_MAGIC1;

    EEPROM_Write(u8Offset, (UINT32)data, (UINT32)EEP_WB_PBM_MAGIC_LEN);
}

BOOL WB_PBM_MagicStrCheck(void)
{
    UINT64  u8Offset;
    UINT8   data[EEP_WB_PBM_MAGIC_LEN];
    x_memset(data, 0, EEP_WB_PBM_MAGIC_LEN);

    u8Offset = EEP_WB_PBM_MAGIC_NUM00;
    EEPROM_Read(u8Offset, (UINT32)data, (UINT32)EEP_WB_PBM_MAGIC_LEN);

    if(data[0] == EEP_WB_PBM_MAGIC0 && data[1] == EEP_WB_PBM_MAGIC1)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
void WB_PBM_CoefClearNVM(void)
{
    UINT64  u8Offset;
    UINT32 u4_i;
    UINT8   data[EEP_WB_PBM_SIZE_PER_TEMP];

    x_memset(data, 0, EEP_WB_PBM_SIZE_PER_TEMP);

    u8Offset = (UINT64)EEP_WB_PBM_COEF_OFFSET;
    for (u4_i=0; u4_i<NUM_OF_WB_INDEX; ++u4_i)
    {
        EEPROM_Write(u8Offset, (UINT32)data, EEP_WB_PBM_SIZE_PER_TEMP);
        u8Offset += EEP_WB_PBM_SIZE_PER_TEMP;
    }
}
void WB_PBM_CoefWriteDefaultToNVM(void)
{
    UINT64  u8Offset;
    UINT32 u4_i, u4_j;
    UINT16 u2_value;
    UINT8  data[EEP_WB_PBM_SIZE_PER_TEMP];
    UINT8  *ptr;

    ptr = data;
    for (u4_j=0; u4_j<3; ++u4_j)
    {
        for (u4_i=0; u4_i<EEP_WB_PBM_NUM_PT; ++u4_i)
        {
            u2_value = _calculate_default_level(u4_i);
            *ptr = (UINT8)(u2_value & 0xFF);
            ++ptr;
            *ptr = (UINT8)(u2_value >> 8);
            ++ptr;
        }
    }
    u8Offset = (UINT64)EEP_WB_PBM_COEF_OFFSET;
    for (u4_i=0; u4_i<NUM_OF_WB_INDEX; ++u4_i)
    {
        EEPROM_Write(u8Offset, (UINT32)data, EEP_WB_PBM_SIZE_PER_TEMP);
        u8Offset += EEP_WB_PBM_SIZE_PER_TEMP;
    }
}
void WB_PBM_WriteDefaultToNVMIfNecessary(void)
{
    if(!WB_PBM_MagicStrCheck())
    {
        WB_PBM_CoefWriteDefaultToNVM();
        WB_PBM_MagicStrSet();
    }
}
////////////////////////////////////////////
//  flow control
////////////////////////////////////////////
static void _apply_color_temp(UINT16 ct)
{
    AUTO_COLORTEMP_SETS_INFO_T ct_info;
    x_memset(&ct_info, 0, sizeof(AUTO_COLORTEMP_SETS_INFO_T));
    ct_info.ui1CTIndex = (ct | 0x80);   //force update gamma
    DRVCUST_AutoColorTempUpdate(&ct_info);

    _easySetPatternDisplay(_u2_easy_set_pattern_en, _u2_easy_set_pattern_level);
}
void WB_PBM_Init(UINT16 u2_param)
{
    UINT32 k;

    //useless
    _u2_ysum_pos = 0;
    _u2_ysum_pixel_pos = 0;
    _u2_wbt_idx_change_sts = 0;

    _u2_setup_started = 1;
    _u2_wbt_idx = 0;
    _u2_easy_set_pattern_en = 2;
    _u2_easy_set_pattern_level = EEP_WB_PBM_NUM_PT-1;

    if (u2_param == 1 || u2_param == 3)
    {
        for (k=0; k<NUM_OF_WB_INDEX; ++k)
        {
            _reset_dram_table_to_default(k);
        }
    }
    else if (u2_param == 4)
    {
        if( WB_PBM_MagicStrCheck())
        {
            for (k=0; k<NUM_OF_WB_INDEX; ++k)
            {
            	  _reset_dram_table_to_default(k);
                _load_table_from_NVM(k);
            }
        }
        else
        {
    for (k=0; k<NUM_OF_WB_INDEX; ++k)
    {
        _reset_dram_table_to_default(k);
    }
        }
    }
    #if 0
    //debug use, request from EMCS
    _inp_easy_white_pattern_table[32][0][1] = 3000;
    _inp_easy_white_pattern_table[32][1][1] = 4096;
    _inp_easy_white_pattern_table[32][2][1] = 3000;
    
    _inp_easy_white_pattern_table[32][0][2] = 4000;
    _inp_easy_white_pattern_table[32][1][2] = 4000;
    _inp_easy_white_pattern_table[32][2][2] = 3000;
    #endif

#if FUNC_FROM_AP
    vUser_ColorTemp_Read(&_original_color_temp);  //backup current ui color temp  //function from ap
#endif
    WB_PBM_LutAdjOnSet(1);  //pbm, initialize _u2_lut_adj_on
    _easySetPatternDisplay(_u2_easy_set_pattern_en, _u2_easy_set_pattern_level);

}
void WB_PBM_DeInit(UINT16 u2_param)
{

    _u2_easy_set_pattern_en = 2; 
    if(u2_param <= 1)
    {
        WB_PBM_MagicStrClear();
        _save_all_table_to_NVM();
        WB_PBM_MagicStrSet();
    }
    _u2_easy_set_pattern_level = EEP_WB_PBM_NUM_PT - 1;  // allen
    _u2_setup_started = 0;

    CheckToEnablePBM();

    _u2_wbt_idx = _original_color_temp;
    _apply_color_temp(_original_color_temp); //write back original ui color temp
}
void WB_PBM_ColorTempIdxSet(UINT16 u2_idx)
{
    if (_u2_setup_started)
    {
        _u2_wbt_idx = u2_idx;

        _apply_color_temp(_u2_wbt_idx);
    }
    else
    {
        _apply_color_temp(u2_idx);
    }
}
UINT16 WB_PBM_ColorTempIdxGet(void)
{
    if (_u2_setup_started)
    {
        return _u2_wbt_idx;
    }
    else
    {
        return _iUsedColorTemp;
    }
}

void WB_PBM_WBAdjustMethodSet(UINT16 u2_idx)
{
    _u2_wb_adj_method = u2_idx;
}
UINT16 WB_PBM_WBAdjustMethodGet(void)
{
    return _u2_wb_adj_method;
}
void WB_PBM_LutAdjOnSet(UINT16 u2_idx)
{
    _u2_lut_adj_on = (u2_idx ? 1 : 0);
    DRVCUST_SetPBMGammaByte(_u2_lut_adj_on);
#if FUNC_FROM_AP
    vUser_ColorTemp_Exec();       //function from ap
#endif	
}
UINT16 WB_PBM_LutAdjOnGet(void)
{
    _u2_lut_adj_on = DRVCUST_GetPBMGammaByte();
    if(_u2_lut_adj_on != 1)     //if EEPROM is initialized as FF, set to 0 as default
    {
        _u2_lut_adj_on = 0;
    }
    return _u2_lut_adj_on;
}
void WB_PBM_WBTIdxChangeStsSet(UINT16 u2_idx)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_WBTIdxChangeStsSet(%x) --\n",u2_idx));
    _u2_wbt_idx_change_sts = u2_idx;
}
UINT16 WB_PBM_WBTIdxChangeStsGet(void)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_WBTIdxChangeStsGet(%x) --\n",_u2_wbt_idx_change_sts));
    return _u2_wbt_idx_change_sts;
}
static void _extract_x_y(UINT16 u2In, UINT16 *p_u2X, UINT16 *p_u2Y)
{
    *p_u2Y =	(u2In & 0x00ff)*8;
    *p_u2X =	((u2In >> 8) & 0x00ff)*8;
}
static void _capture_single_pt(UINT16 u2PosX, UINT16 u2PosY, UINT16 u2YCbCrIn[3], UINT16 u2YCbCrOut[3])
{
    DRVCUST_TurnOnInkMark(0);
    
    vDrvSceDbgInfo(u2PosX , u2PosY, u2YCbCrIn, u2YCbCrOut);
	
//    DRVCUST_TurnOnInkMark(1);
//    DRVCUST_SetInkMark(u2PosX, u2PosY);
}
void WB_PBM_PixelMeasCRSet(UINT16 u2_idx)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_PixelMeasCRSet(%x) --\n",u2_idx));
}
UINT16 WB_PBM_PixelMeasCRGet(void)
{
    UINT16 sceDbgInfo_YCbCrIn[3]; 
    UINT16 sceDbgInfo_YCbCrOut[3];
    UINT16 _u2PosX=0, _u2PosY=0;

    _extract_x_y(_u2_ysum_pixel_pos, &_u2PosX, &_u2PosY);
    _capture_single_pt(_u2PosX, _u2PosY, sceDbgInfo_YCbCrIn, sceDbgInfo_YCbCrOut);
	
    return sceDbgInfo_YCbCrIn[2];
}
void WB_PBM_PixelMeasCBSet(UINT16 u2_idx)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_PixelMeasCBSet(%x) --\n",u2_idx));
}
UINT16 WB_PBM_PixelMeasCBGet(void)
{
    UINT16 sceDbgInfo_YCbCrIn[3]; 
    UINT16 sceDbgInfo_YCbCrOut[3];
    UINT16 _u2PosX=0, _u2PosY=0;

    _extract_x_y(_u2_ysum_pixel_pos, &_u2PosX, &_u2PosY);
    _capture_single_pt(_u2PosX, _u2PosY, sceDbgInfo_YCbCrIn, sceDbgInfo_YCbCrOut);

    return sceDbgInfo_YCbCrIn[1];
}
void WB_PBM_PixelMeasYSet(UINT16 u2_idx)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_PixelMeasYSet(%x) --\n",u2_idx));
}
UINT16 WB_PBM_PixelMeasYGet(void)
{
    UINT16 sceDbgInfo_YCbCrIn[3]; 
    UINT16 sceDbgInfo_YCbCrOut[3];
    UINT16 _u2PosX=0, _u2PosY=0;

    _extract_x_y(_u2_ysum_pixel_pos, &_u2PosX, &_u2PosY);
    _capture_single_pt(_u2PosX, _u2PosY, sceDbgInfo_YCbCrIn, sceDbgInfo_YCbCrOut);
    return sceDbgInfo_YCbCrIn[0];
}
void WB_PBM_YSumPixelPosSet(UINT16 u2_idx)
{
    UINT16 _u2PosX=0, _u2PosY=0;
    _u2_ysum_pixel_pos = u2_idx;

    _extract_x_y(_u2_ysum_pixel_pos, &_u2PosX, &_u2PosY);

    DRVCUST_TurnOnInkMark(1);
    DRVCUST_SetInkMark(_u2PosX, _u2PosY);
}
UINT16 WB_PBM_YSumPixelPosGet(void)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_YSumPixelPosGet(%x) --\n",_u2_ysum_pixel_pos));
    return _u2_ysum_pixel_pos;
}
void WB_PBM_YSumPosSet(UINT16 u2_idx)
{
    UINT16 _u2PosX=0, _u2PosY=0;
    _u2_ysum_pos = u2_idx;
    _extract_x_y(_u2_ysum_pos, &_u2PosX, &_u2PosY);

    DRVCUST_TurnOnInkMark(1);
    DRVCUST_SetInkMark(_u2PosX, _u2PosY);
}
UINT16 WB_PBM_YSumPosGet(void)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_YSumPosGet(%x) --\n",_u2_ysum_pos));
    return _u2_ysum_pos;
}
void WB_PBM_YSumMeasSet(UINT16 u2_idx)
{
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- WB_PBM_YSumMeasSet(%x) --\n",u2_idx));
}
UINT16 WB_PBM_YSumMeasGet(void)
{
    UINT16 sceDbgInfo_YCbCrIn[3]; 
    UINT16 sceDbgInfo_YCbCrOut[3];
    UINT8   i,j;
    UINT16 _u2PosX=0, _u2PosY=0;
    UINT32 u4_ysum_meas =0;

    _extract_x_y(_u2_ysum_pos, &_u2PosX, &_u2PosY);

    DRVCUST_TurnOnInkMark(0);

    for( i=0 ; i <=9 ; i++)
    {
        for( j=0 ; j<=9 ;j++)
        {
            vDrvSceDbgInfo( _u2PosX+ i , _u2PosY + j , sceDbgInfo_YCbCrIn, sceDbgInfo_YCbCrOut);
            u4_ysum_meas += sceDbgInfo_YCbCrIn[0];
        }
    }
    u4_ysum_meas = u4_ysum_meas/100;

//    DRVCUST_TurnOnInkMark(1);
//    DRVCUST_SetInkMark(_u2PosX, _u2PosY);

    return  (UINT16)u4_ysum_meas;
}
////////////////////////////////////////////
//  aging
////////////////////////////////////////////
void AgingShowPattern(UINT16 u2_enable)
{
    BOOL bOnOff = u2_enable ? TRUE : FALSE;
    if(bOnOff)
    {
#if FUNC_FROM_AP
        a_amb_pause_app("menu");  //function from ap
#endif
    }
    vGammaOSDControl(&bOnOff);

    if(u2_enable)
    {
        vDrvGammaMute(4095, 4095, 4095);
		DRVCUST_SetAgingFlag(TRUE);
    }
    else
    {
        vDrvGammaUnmute();
		DRVCUST_SetAgingFlag(FALSE);
    }
}
void AgingLastPowerControlState_Set(UINT16 u2_idx)
{
	UINT8 u1AgingLastPowerControlState = u2_idx > 0 ? 0 : 1;
	
	UNUSED(EEPROM_Write(POWER_SEQ_FORCE_TO_STANDBY_ADDR, (UINT32)(void *)&u1AgingLastPowerControlState, sizeof(UINT8)));
	
//temp mark due to build error
#if 0 
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- AgingLastPowerControlState_Set(%x) --\n",u2_idx));
    UINT32 u4AgingLastPowerControlOffset;
    if (DRVCUST_InitQuery(eFactoryAgingLastPowerControlOffset, &u4AgingLastPowerControlOffset) == 0)
    {
        UINT8 u1AgingLastPowerControlState = u2_idx > 0 ? 1 : 0;
    
        UNUSED(EEPROM_Write(u4AgingLastPowerControlOffset, (UINT32)(void *)&u1AgingLastPowerControlState, sizeof(UINT8)));
    }
#endif
}
UINT16 AgingLastPowerControlState_Get(void)
{
	UINT8 uiRet,u1AgingLastPowerControlState = 0;
	
	UNUSED(EEPROM_Read(POWER_SEQ_FORCE_TO_STANDBY_ADDR, (UINT32)(void *)&u1AgingLastPowerControlState, sizeof(UINT8)));
	uiRet = u1AgingLastPowerControlState > 0 ? 0 : 1;
	return uiRet;

//temp mark due to build error
#if 0 
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- AgingLastPowerControlState_Get(%x) --\n"));
    
    UINT32 u4AgingLastPowerControlOffset;
    if (DRVCUST_InitQuery(eFactoryAgingLastPowerControlOffset, &u4AgingLastPowerControlOffset) == 0)
    {

    }
#endif
    //return 0;
}

#ifdef CUST_SOEM_DRV
UINT32 Sony_PBM_EDID_Write (UINT16 write_data)
{
    UINT8 i;
	UINT8 u2_LastByte[2]; //2+null
	UINT8 PBM_ver_byte[EEP_WB_VER_BYTE]={0};
	UINT16 z;
    //UINT8 data[EEP_WB_PBM_SIZE_ALL_TEMP]={0};
	//UINT32 compare_eep_buf[EEP_WB_PBM_SIZE_ALL_TEMP]={0};
	//UINT32 compare_panel_buf[EEP_WB_PBM_SIZE_ALL_TEMP]={0};  //modify to fix warning "Frame size bigger than 1024Byte"
	UINT8 *data;
	UINT8 *compare_eep_buf;
//	UINT32 *compare_panel_buf;
		
    UINT64  u8Offset;
	UINT16 u2_Write_times=0,u2_temp_addr =0,u2_shift_addr =0,u2_last_addr =0,u2_last_data_len =0 ,u2_last_byte_addr =0;
    UINT32 u4_temp_address;
    UINT32 u4_return = 0;


    u2_LastByte[0]=0x57; //verify byte for PBM
	u2_LastByte[1]=0x50;	
	u2_last_byte_addr = WB_PBM_START_ADDR + EEP_WB_PBM_SIZE_ALL_TEMP;
    u2_Write_times = (EEP_WB_PBM_SIZE_ALL_TEMP)/ 0x10;

	data = x_mem_alloc(EEP_WB_PBM_SIZE_ALL_TEMP);
	compare_eep_buf = x_mem_alloc(EEP_WB_PBM_SIZE_ALL_TEMP);
//	compare_eep_buf = x_mem_alloc(sizeof(UINT32)*EEP_WB_PBM_SIZE_ALL_TEMP);
//	compare_panel_buf = x_mem_alloc(sizeof(UINT32)*EEP_WB_PBM_SIZE_ALL_TEMP);

	
	x_memset(data, 0, EEP_WB_PBM_SIZE_ALL_TEMP);	
    x_memset(compare_eep_buf, 0, EEP_WB_PBM_SIZE_ALL_TEMP);	
//	x_memset(compare_eep_buf, 0, sizeof(UINT32)*EEP_WB_PBM_SIZE_ALL_TEMP);
//	x_memset(compare_panel_buf, 0,sizeof(UINT32)*EEP_WB_PBM_SIZE_ALL_TEMP);

    if( write_data == 0)
    {
		do 
		{
//========================         confirm verify byte from EEP  ==============================

			if (0 != EEPROM_Read( 0x3e5a, (UINT32)PBM_ver_byte, (UINT32)EEP_WB_VER_BYTE))
			{
				u4_return = 1;
				break;
			}

			if(PBM_ver_byte[0]!=EEP_WB_PBM_VER1 && PBM_ver_byte[1]!=EEP_WB_PBM_VER2)
			{
				u4_return = 1;
				break;
			}
//===================		read PBM data from EEP		================================================
        u8Offset = (UINT64)EEP_WB_PBM_MAGIC_OFFSET;
            if (0 != EEPROM_Read( u8Offset, (UINT32)data, EEP_WB_PBM_SIZE_ALL_TEMP))



            {
                u4_return = 1;
                break;
            }		

        GPIO_SetOut(PANEL_BINT_PIN,1);
	
//======================      write PBM data to Panel   ===================================================
           u2_temp_addr = WB_PBM_START_ADDR;
        for(i = 0 ; i < u2_Write_times; i++)
        {
            u2_shift_addr = i*0x10; // shift address
	    x_thread_delay(20);
                if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_temp_addr + u2_shift_addr ,&data[u2_shift_addr], 0x10)) // write loop by per 16 byte
                {
                    u4_return = 1;
                    break;
                }
            }
            if (u4_return)
            {
                break;
        }
		x_thread_delay(20);
            u2_last_data_len = (EEP_WB_PBM_SIZE_ALL_TEMP) % 0x10;
            u2_last_addr = ((WB_PBM_START_ADDR + EEP_WB_PBM_SIZE_ALL_TEMP) / 0x10)*0x10;
            if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_last_addr ,&data[u2_shift_addr +0x10 ], u2_last_data_len))
            {
                u4_return = 1;
                break;
			}
			x_thread_delay(20);

			SIF_X_Read(BUS_ID,CLK_DIV,TCON_NVM_SLVADDR,ADDR_LENGTH,WB_PBM_START_ADDR,compare_eep_buf,602);
#if 0
			x_memset(compare_panel_buf, 0,EEP_WB_PBM_SIZE_ALL_TEMP);			
			j = 0;
			for(z=0; z<EEP_WB_PBM_SIZE_ALL_TEMP; z++)
			{
				compare_panel_buf[j]=data[z];
				j++;
			}
//======================	   compare byte by byte 	==========================================
#endif
		
			for (z=0; z<EEP_WB_PBM_SIZE_ALL_TEMP; z++)
				{
				if (data[z]!= compare_eep_buf[z])
					{
						u4_return = 1;
						break;
					}
				}			
//======================          write last byte 0x5750 to panel      ===================================================
	        x_thread_delay(20);
            if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_last_byte_addr ,u2_LastByte,2)) // write loop by per 16 byte
            {
                u4_return = 1;
                break;
            }

//==========================  write new verify byte in eep	================================
			PBM_ver_byte[0]=0xff;
			PBM_ver_byte[1]=0xff;
			if (0 != EEPROM_Write( 0x3e5a, (UINT32)PBM_ver_byte, (UINT32)EEP_WB_VER_BYTE))
			{
				u4_return = 1;
				break;
			}
		}while(0);

        GPIO_SetOut(PANEL_BINT_PIN,0);
	
    }
    else if(write_data == 2)
    {
		do 
		{
//======================		  confirm verify byte in panel		 ==========================================
			
			if (SIF_ERROR_RETURN == SIF_X_Read(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_last_byte_addr ,u2_LastByte, EEP_WB_VER_BYTE))
			{
				u4_return = 1;
				break;
			}
			if(u2_LastByte[0]!=0x57 && u2_LastByte[1]!=0x50)
    {
				u4_return = 1;
				break;
			}

//======================		write PBM to EEP		===================================================
  
			u2_temp_addr = WB_PBM_START_ADDR;
        u2_Write_times = 0x25;                                 //EEP_WB_PBM_SIZE_ALL_TEMP / 0x10;
        u2_last_data_len = EEP_WB_PBM_SIZE_ALL_TEMP  % 0x10;   // get the last data len
        u2_last_addr = EEP_WB_PBM_MAGIC_OFFSET + u2_Write_times *0x10;

			if (SIF_ERROR_RETURN == SIF_X_Read(BUS_ID, TCON_NVM_SLVADDR, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_temp_addr,data, 602))
			{
				u4_return = 1;
				break;
			}
	
        for(i = 0 ; i < u2_Write_times; i++)
        {
            u2_shift_addr = i*0x10; // shift address
            u4_temp_address =(UINT32) &data[u2_shift_addr];
				if (0 != EEPROM_Write((UINT64) EEP_WB_PBM_MAGIC_OFFSET + u2_shift_addr,u4_temp_address,0x10))
				{
					u4_return = 1;
					break;
				}
	    x_thread_delay(20);
        }
			if (u4_return)
			{
				break;
			}
			
        u4_temp_address =(UINT32) &data[u2_Write_times * 0x10];
			if (0 != EEPROM_Write((UINT64) u2_last_addr,u4_temp_address,u2_last_data_len))
			{
				u4_return = 1;
				break;
			}
	    x_thread_delay(20);			
			EEPROM_Read(EEP_WB_PBM_MAGIC_OFFSET,(UINT32)compare_eep_buf,602);
#if 0		
			x_memset(compare_eep_buf, 0,EEP_WB_PBM_SIZE_ALL_TEMP);

			j=0;
			for(z=0; z<EEP_WB_PBM_SIZE_ALL_TEMP; z++)
			{
			compare_eep_buf[j]=data[z];
			j++;
			}

#endif
		
//======================	 compare byte by byte	  ===========================================
		
			for (z=0; z<EEP_WB_PBM_SIZE_ALL_TEMP; z++)
			{
				if (data[z] != compare_eep_buf[z])
				{
					u4_return =1;
					break;
				}			
			}

//======================   write verify byte in eep   ==========================================
			PBM_ver_byte[0]=EEP_WB_PBM_VER1;  //verify byte by jig
			PBM_ver_byte[1]=EEP_WB_PBM_VER2;		
			if (0 != EEPROM_Write(0x3e5a ,(UINT32)PBM_ver_byte, (UINT32)EEP_WB_VER_BYTE))
			{
				u4_return = 1;
				break;
			}

		}while(0);
	
    }
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"pbm ret(%d)\n", u4_return));
	x_mem_free(data);
	x_mem_free(compare_eep_buf);
//	x_mem_free(compare_panel_buf);	
	data = NULL;
	compare_eep_buf = NULL;
//	compare_panel_buf = NULL;
    return u4_return;
}
#endif //CUST_SOEM_DRV


