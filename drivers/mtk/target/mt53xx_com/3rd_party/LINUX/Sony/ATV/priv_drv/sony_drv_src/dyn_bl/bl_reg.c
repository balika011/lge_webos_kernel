/* ========================================================================== */
/*                                                                            */
/*   bl_reg.c                                                               */
/*   (c) 2009 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include "x_os.h"
#include "eeprom_if.h"

#include "sony_priv_inc/utility.h"
#include "sony_priv_inc/bl_ctrl.h"
#include "acfg_atv_eep.h"
#include "sony_priv_inc/bl_reg.h"
#include "sony_drv_inc/Sony_drv_cb.h"

//-----------------------------------------------------------------------------
//                              CONSTANT DEFINITIONS
//-----------------------------------------------------------------------------
#define EEP_OFFSET_EMCS_BL_DUMMY    0xFFFF
#define BL_EEP_CHK                  0x0017
//-----------------------------------------------------------------------------
//                              TYPE DEFINITIONS
//-----------------------------------------------------------------------------
typedef struct _DYN_BL_DRV_EEP{
	const UINT16 offset;
	const UINT16 sreg;
	const UINT8  byte_size;
	const UINT16 init;
} DYN_BL_DRV_EEP;
//-----------------------------------------------------------------------------
//                              GLOBAL VARIABLE
//-----------------------------------------------------------------------------
void main_bl_sreg_write(UINT16 sreg,UINT16 val);
//-----------------------------------------------------------------------------
//                              STATIC VARIABLE
//-----------------------------------------------------------------------------
static void bl_sreg_to_eep_write(UINT16 sreg, UINT16 value);
static void bl_eep_read(UINT64 offset, UINT16* memPtr, UINT8 memLen);
static void bl_eep_write(UINT64 offset, UINT16 memPtr, UINT16 memLen);
static UINT16 SREG[SREG_COUNT];

const DYN_BL_DRV_EEP tbl_dyn_bl_eep[] = {
            /* Offset */                        /* SREG */            /* Byte Size */    /* Init */
    {   EEP_OFFSET_EMCS_NVM_CS,             SREG_BL_DUMMY,                  1,            BL_EEP_CHK },
    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_BL_TEMP_TGT,               1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_BL_TEMP_CNT,               1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_EMG_MODE,                  1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_NORMAL_BRIGHT_CONT,        1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DIMMER_DUTY,               1,               0x0000   },
    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_OPT_READ_DATA,             2,               0x00FF   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_OPT_BL_OUT_AVG,            2,               0x0000   },
    {   EEP_OFFSET_EMCS_OPT_BL_REMOTE_EN,   SREG_OPT_BL_REMOTE_ENABLE,      1,               0x0001   },
    {   EEP_OFFSET_EMCS_OPT_BL_COUNT,       SREG_OPT_BL_COUNT,              1,               0x001e   },
    {   EEP_OFFSET_EMCS_OPT_BL_CUTMINMAX,   SREG_OPT_BL_CUTMINMAX,          1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_OPT_BL_SENSOR,             1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_OPT_BL_COEF,               1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC4PS_PIC_STHIS,          1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_LCD_BACKLIGHT_CONTROL,     1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DYNAMIC_BACKLIGHT,         2,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_OPT_BL_COEF_TGT,           1,               0x0000   },
    
    {   EEP_OFFSET_EMCS_BL_MANU_EN,         SREG_BL_MANU_EN,                1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_BL_GAIN,                   1,               0x00ff   },
    {   EEP_OFFSET_EMCS_BL_AGING_GAIN,      SREG_BL_AGING_GAIN,             1,               0x00ff   },
    {   EEP_OFFSET_EMCS_BL_NO_SIGNAL_GAIN,  SREG_BL_NO_SIGNAL_GAIN,         1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_BL_DIMMER_SEL,             1,               0x0000   },
    {   EEP_OFFSET_EMCS_BL_REDUCTION,       SREG_BL_REDUCTION,              1,               0x0046   },
    
    {   EEP_OFFSET_EMCS_OPT_BL_TH0,         SREG_OPT_BL_TH0,                1,               0x00f0   },
    {   EEP_OFFSET_EMCS_OPT_BL_TH1,         SREG_OPT_BL_TH1,                1,               0x00a0   },
    {   EEP_OFFSET_EMCS_OPT_BL_TH2,         SREG_OPT_BL_TH2,                1,               0x0050   },
    {   EEP_OFFSET_EMCS_OPT_BL_TH3,         SREG_OPT_BL_TH3,                1,               0x0000   },
    {   EEP_OFFSET_EMCS_OPT_BL_END_TGT1,    SREG_OPT_BL_END_TGT1,           1,               0x00c8   },
    {   EEP_OFFSET_EMCS_OPT_BL_END_TGT2,    SREG_OPT_BL_END_TGT2,           1,               0x0064   },
    {   EEP_OFFSET_EMCS_OPT_BL_END_TGT3,    SREG_OPT_BL_END_TGT3,           1,               0x0040   },
    {   EEP_OFFSET_EMCS_OPT_BL_IIR,         SREG_OPT_BL_IIR,                1,               0x00d2   },
    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_OPT_BL_INDEX,              1,               0x0000   },
    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_IIR_CLR_TIMES,             1,               0x0000   },
    {   EEP_OFFSET_EMCS_DAPLM_TH1,          SREG_DAPLM_TH1,                 2,               0x001a   },
    {   EEP_OFFSET_EMCS_DAPLM_TH2,          SREG_DAPLM_TH2,                 2,               0x00b0   },
    {   EEP_OFFSET_EMCS_DAPLM_TH3,          SREG_DAPLM_TH3,                 2,               0x00ff   },
    {   EEP_OFFSET_EMCS_DAPLM_TH4,          SREG_DAPLM_TH4,                 2,               0x00ff   },
    {   EEP_OFFSET_EMCS_DBC_APL_EN,         SREG_DBC_APL_EN,                1,               0x0001   },
    {   EEP_OFFSET_EMCS_DBC_APL_TH1,        SREG_DBC_APL_TH1,               2,               0x0000   },
    {   EEP_OFFSET_EMCS_DBC_APL_TH2,        SREG_DBC_APL_TH2,               2,               0x0034   },
    {   EEP_OFFSET_EMCS_DBC_APL_TH3,        SREG_DBC_APL_TH3,               2,               0x00ff   },  
    {   EEP_OFFSET_EMCS_DBC_APL_TH4,        SREG_DBC_APL_TH4,               2,               0x00ff   },
    {   EEP_OFFSET_EMCS_DBC_APL_IIR_L,      SREG_DBC_APL_IIR_L,             1,               0x0001   },
    {   EEP_OFFSET_EMCS_DBC_APL_IIR_M,      SREG_DBC_APL_IIR_M,             1,               0x00b0   },
    {   EEP_OFFSET_EMCS_DBC_APL_IIR_H,      SREG_DBC_APL_IIR_H,             1,               0x00ff   },
    {   EEP_OFFSET_EMCS_DBC_APL_BL_TLIM,    SREG_DBC_APL_BL_TLIM,           1,               0x00ff   },
    {   EEP_OFFSET_EMCS_DBC_APL_BL_BLIM,    SREG_DBC_APL_BL_BLIM,           1,               0x0028   },
    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_SCENE_CHANGE_DET,          1,               0x0000   },    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_APL_LUMA,                  1,               0x0000   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_APL_DBC,                   1,               0x0000   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC_APL,                   1,               0x0000   }, 
    
    {   EEP_OFFSET_EMCS_DBC_MAX_IIR,        SREG_DBC_MAX_IIR,               1,               0x000a   }, 
    {   EEP_OFFSET_EMCS_DBC_MAX_THR,        SREG_DBC_MAX_THR,               2,               0x0012   }, 
    {   EEP_OFFSET_EMCS_DBC_MAX_THR1,       SREG_DBC_MAX_THR1,              2,               0x00c8   }, 
    {   EEP_OFFSET_EMCS_DBC_MAX_THR2,       SREG_DBC_MAX_THR2,              2,               0x01c2   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC_MAX_EN,                1,               0x0001   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC_MAX,                   2,               0x0000   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_MAX_DBC,                   2,               0x0000   }, 
    {   EEP_OFFSET_EMCS_DBC_MAX_THR_I,      SREG_DBC_MAX_THR_I,             1,               0x000c   },

    {   EEP_OFFSET_EMCS_OPT_BL_TH0_C,       SREG_OPT_BL_TH0_CIN,            1,               0x005a   },
    {   EEP_OFFSET_EMCS_OPT_BL_TH1_C,       SREG_OPT_BL_TH1_CIN,            1,               0x0050   },
    {   EEP_OFFSET_EMCS_OPT_BL_TH2_C,       SREG_OPT_BL_TH2_CIN,            1,               0x0046   },
    {   EEP_OFFSET_EMCS_OPT_BL_TH3_C,       SREG_OPT_BL_TH3_CIN,            1,               0x003c   },
    {   EEP_OFFSET_EMCS_OPT_BL_END_TGT1_C,  SREG_OPT_BL_END_TGT1_CIN,       1,               0x00c8   },
    {   EEP_OFFSET_EMCS_OPT_BL_END_TGT2_C,  SREG_OPT_BL_END_TGT2_CIN,       1,               0x0064   },
    {   EEP_OFFSET_EMCS_OPT_BL_END_TGT3_C,  SREG_OPT_BL_END_TGT3_CIN,       1,               0x0000   },
    {   EEP_OFFSET_EMCS_OPT_BL_IIR_C,       SREG_OPT_BL_IIR_CIN,            1,               0x00d2   },
    {   EEP_OFFSET_EMCS_FACT_BL_GAIN,       SREG_FACT_BL_GAIN,              1,               0x00ff   },
    
    {   EEP_OFFSET_EMCS_DBC4PS_PIC_EN,      SREG_DBC4PS_PIC_EN,             1,               0x0001   }, 
    {   EEP_OFFSET_EMCS_DBC4PS_BL_B_INC,    SREG_DBC4PS_BL_B_INC,           1,               0x0004   }, 
    {   EEP_OFFSET_EMCS_DBC4PS_BL_B_DEC,    SREG_DBC4PS_BL_B_DEC,           1,               0x0004   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC4PS_BL_CONT,            1,               0x0000   }, 
    {   EEP_OFFSET_EMCS_DBC4PS_PIC_INT_1,   SREG_DBC4PS_PIC_INT_1,          1,               0x0002   }, 
    {   EEP_OFFSET_EMCS_DBC4PS_PIC_INT_2,   SREG_DBC4PS_PIC_INT_2,          1,               0x0001   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_CHIST,                     1,               0x0000   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC4PS_PIC_STATE,          1,               0x0000   }, 
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_LUMA_MAX,                  1,               0x0000   },
    {   EEP_OFFSET_EMCS_DBC4PS_LUMAX_TH,    SREG_DBC4PS_LUMAX_TH,           1,               0x0016   },
    {   EEP_OFFSET_EMCS_DBC4PS_LUMAX_TH_PC, SREG_DBC4PS_LUMAX_TH_PC,        1,               0x0014   },
        
    {   EEP_OFFSET_EMCS_BL_REDUCTION_TIME,  SREG_BL_REDUCTION_TIME,         1,               0x000a   },
    {   EEP_OFFSET_EMCS_DBC4PS_CON_TH,      SREG_DBC4PS_CON_TH,             2,               0x016d   },
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_DBC4PS_COUNTER,            2,               0x0000   },
    
    {   EEP_OFFSET_EMCS_BL_DUMMY,           SREG_BL_STATE,                  1,               0x0000   },
    {   EEP_OFFSET_EMCS_CRI_BLOFF_WAIT,     SREG_CRI_BLOFF_WAIT,            1,               0x000a   }
};

/*
 * Initialize backlight SREG value
 * @param 
 * @return
 */
void set_initial_sreg(void)
{
    UINT16 tbl_cnt;
    UINT8  eep_chksum;
    UINT16 memData;
    UINT16 buffer[2];
    UINT16 data;
    
    EEPROM_Read((UINT64)EEP_OFFSET_EMCS_NVM_CS, (UINT32)&eep_chksum, 1);
    
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_dyn_bl_eep)/sizeof(tbl_dyn_bl_eep[0])); tbl_cnt++)    
    {
        if(eep_chksum == BL_EEP_CHK)
        {
            bl_eep_read(tbl_dyn_bl_eep[tbl_cnt].offset, &memData, tbl_dyn_bl_eep[tbl_cnt].byte_size);  
         
            if(tbl_dyn_bl_eep[tbl_cnt].byte_size == 2)
            {
                buffer[0] = (UINT16)((memData & 0xFF00) >> 8);
                buffer[1] = (UINT16)((memData & 0x00FF) << 8);    
                data = buffer[0] | buffer[1];     
            }
            else
            {
                buffer[0] = (UINT8)(memData & 0xFF);     
                data = buffer[0];    
            }
                     
            if(tbl_dyn_bl_eep[tbl_cnt].offset == EEP_OFFSET_EMCS_BL_DUMMY)
            {
                bl_sreg_write(tbl_dyn_bl_eep[tbl_cnt].sreg, tbl_dyn_bl_eep[tbl_cnt].init);
            }
            else
            {
                bl_sreg_write(tbl_dyn_bl_eep[tbl_cnt].sreg, data);
            }
        }
        else
        {          
            main_bl_sreg_write(tbl_dyn_bl_eep[tbl_cnt].sreg, tbl_dyn_bl_eep[tbl_cnt].init);
        }
    }
}   


/*
 *  Read eep data for Backlight control
 * @param 
 * @return
 */
void bl_eep_read(UINT64 offset, UINT16* memPtr, UINT8 memLen)
{
    if((offset >= EEP_OFFSET_EMCS_NVM_CS) && (offset <= EEP_OFFSET_EMCS_BL_CTRL_AREA_END))
    {
        EEPROM_Read((UINT64)offset, (UINT32)memPtr, memLen);
    }
}

/*
 *  Write eep data for Backlight control
 * @param 
 * @return
 */
void bl_eep_write(UINT64 offset, UINT16 memPtr, UINT16 memLen)
{
    UINT8 buffer[2];
    
    if((offset >= EEP_OFFSET_EMCS_NVM_CS) && (offset <= EEP_OFFSET_EMCS_BL_CTRL_AREA_END))
    {
        if(memLen == 2)
        {
            buffer[0] = (UINT8)((memPtr & 0xFF00) >> 8);
            buffer[1] = (UINT8)(memPtr & 0xFF);
        }
        else
        {
            buffer[0] = (UINT8)(memPtr & 0xFF);
        }
        EEPROM_Write((UINT64)offset, (UINT32)buffer, memLen);
    }
}

/*
 *  Backlight SREG read function
 * @param 
 * @return
 */
UINT16 bl_sreg_read(UINT16 sreg)
{
    DYN_BL_MEMBER_T* pt_this = t_dyn_bl_ext();
	UINT16 a_ret;
	
	LockMutex(pt_this->h_blSregMutex);
	if(sreg<SREG_COUNT)
	{
		a_ret = SREG[sreg];
	}
	else
	{
		a_ret = 0;
	#ifdef DBL_ENABLE_LOG
		DBL_LOG("SREG Read Table Overflow at %d \n",sreg);
	#endif
	}
	UnlockMutex(pt_this->h_blSregMutex);
	return(a_ret);
}

/*  
 *  Backlight SREG write function
 * @param 
 * @return
 */
void bl_sreg_write(UINT16 sreg, UINT16 val)
{
    DYN_BL_MEMBER_T* pt_this = t_dyn_bl_ext();
    
    LockMutex(pt_this->h_blSregMutex); 
	if(sreg<SREG_COUNT)
	{
		SREG[sreg] = val;
	}
	else
	{
	#ifdef DBL_ENABLE_LOG
		DBL_LOG("SREG Write Table Overflow at %d \n",sreg);
    #endif
	}
	UnlockMutex(pt_this->h_blSregMutex);
}

/*  
 *  Backlight SREG to eep write function
 * @param 
 * @return
 */
static void bl_sreg_to_eep_write(UINT16 sreg, UINT16 value)
{
    UINT16 i;
    UINT16 tbl_cnt = 0xFFFF;
    
    if(sreg < SREG_COUNT)
    {
        for (i=0; i<(sizeof(tbl_dyn_bl_eep)/sizeof(tbl_dyn_bl_eep[0])); i++)
        {
            if(tbl_dyn_bl_eep[i].sreg == sreg)
            {
                tbl_cnt = i;
                break;    
            }
        }  
    }
    if(tbl_dyn_bl_eep[tbl_cnt].offset != EEP_OFFSET_EMCS_BL_DUMMY)
    {
        bl_eep_write((UINT64)tbl_dyn_bl_eep[tbl_cnt].offset, (UINT16)value, tbl_dyn_bl_eep[tbl_cnt].byte_size);
    }
}

/*  
 *  Backlight SREG write function
 * @param 
 * @return
 */
UINT16 main_bl_sreg_read(UINT16 sreg)
{
    return bl_sreg_read(sreg);
}

/*
 *  Write sreg and eep data for Backlight control
 * @param 
 * @return
 */
//void main_bl_sreg_write(UINT16 sreg, UINT16 offset, UINT16 value, UINT8 memLen)
void main_bl_sreg_write(UINT16 sreg, UINT16 value)
{
    bl_sreg_write(sreg, value);
    bl_sreg_to_eep_write(sreg, value);
}

void sreg_write_from_flash(void)
{
    ST_PARAM_DBL_CTRL param;

    vAutoBacklightGetDblParam(&param);

    bl_sreg_write(SREG_BL_TOP, param.bl_top);
    bl_sreg_write(SREG_BL_MID, param.bl_mid);
    bl_sreg_write(SREG_BL_BOTTOM, param.bl_bottom);
    bl_sreg_write(SREG_BL_PWM_FRQ, param.bl_pwm_frq);
    bl_sreg_write(SREG_BL_24P_PWM_FRQ, param.bl_24p_pwm_frq);
    bl_sreg_write(SREG_BL_MID_KNEE, param.bl_mid_knee);
    bl_sreg_write(SREG_BL_SETUDEN, param.bl_setuden);
    bl_sreg_write(SREG_BL_PSH, param.bl_psh);
    bl_sreg_write(SREG_BL_CRI_ONOFF, param.bl_cri_onoff);
    bl_sreg_write(SREG_BL_CRI_PWM_FRQ, param.bl_cri_pwm_frq);
    bl_sreg_write(SREG_BL_CRI_DUTY, param.bl_cri_duty);
    bl_sreg_write(SREG_BL_MAX_DUTY, param.bl_max_duty);
    bl_sreg_write(SREG_BL_MIN_DUTY, param.bl_min_duty);
    bl_sreg_write(SREG_DEEP_DIMMER_TH_EN, param.d_dimmer_th_en);
    bl_sreg_write(SREG_DEEP_DIMMER_TH_H, param.d_dimmer_th_h);
    bl_sreg_write(SREG_DEEP_DIMMER_TH_L, param.d_dimmer_th_l); 
    bl_sreg_write(SREG_TEMP_TH_LOW, param.bl_temp_th_low);
    bl_sreg_write(SREG_TEMP_TH_HIGH, param.bl_temp_th_high);
    bl_sreg_write(SREG_TEMP_BL_LIMIT, param.temp_bl_limit);
    bl_sreg_write(SREG_BL_TEMP_CNTTH, param.bl_temp_cntth);
    bl_sreg_write(SREG_BL_TEMP_INC, param.bl_temp_inc);
    bl_sreg_write(SREG_TEMP_BL_BOTTOM, param.temp_bl_bottom);
    bl_sreg_write(SREG_OPT_BL_DEADB, param.opt_bl_deadb);
    bl_sreg_write(SREG_OPT_BL_DEADB_CIN, param.opt_bl_deadb_cin);
    bl_sreg_write(SREG_BL_CRI_TEMP_L, param.bl_cri_temp_l);
    bl_sreg_write(SREG_BL_CRI_TEMP_H, param.bl_cri_temp_h);
    bl_sreg_write(SREG_PANEL_TYPE, param.panel_type);
}

