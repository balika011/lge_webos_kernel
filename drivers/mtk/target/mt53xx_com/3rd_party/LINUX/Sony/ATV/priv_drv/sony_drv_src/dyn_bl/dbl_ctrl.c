/* ========================================================================== */
/*                                                                            */
/*   control.c                                                               */
/*   (c) 2009 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include "x_dbg.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
#include "acfg_atv_eep.h"
#include "sony_priv_inc/bl_reg.h"
#include "sony_priv_inc/bl_ctrl.h"
#include "bl_dbc.h"
#include "bl_util.h"
#include "ostg_if.h"
#include "drv_video.h"
#include "sony_priv_inc/dbl_ctrl.h"
#include "sony_priv_inc/opt_bl_ctrl.h"
#include "sony_drv_inc/Sony_drv_cb.h"
#include "sony_drv_inc/a_bl_ctrl.h"
#include "sony_drv_inc/temp_setup_if.h"

//-----------------------------------------------------------------------------
//                              CONSTANT DEFINITIONS
//-----------------------------------------------------------------------------
#define BL_GAIN_MAX     255
#define PWM_FRQ_DEADB   32
#define LOGO_DUTY       70
#define COUNTER_RESET   0
//-----------------------------------------------------------------------------
//                              TYPE/ENUM DEFINITIONS
//-----------------------------------------------------------------------------
enum{
    CRI_CTRL_OFF,
    CRI_CTRL_ON
};

enum{
    CRI_STATE_IMPOS,
    CRI_STATE_POS,
    CRI_STATE_INIT
};

enum{
    CRI_TEMP_NG,
    CRI_TEMP_OK
};

enum{
    PANEL_CCFL,
    PANEL_LED
};
//-----------------------------------------------------------------------------
//                              GLOBAL VARIABLE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                              STATIC VARIABLE
//-----------------------------------------------------------------------------
static UINT8  s_w_cri_state;
static UINT8  s_w_last_cri_state;
static UINT8  cri_ctrl_en;
static UINT16 u2_pwm_Freq;
static UINT16 u2_pwm_duty;
static UINT16 u2_current_pwm_Freq;
static UINT16 u2_current_pwm_duty;
static BOOL   u1_dynBLenable;
static UINT8  u1_bl_state_change;
static BOOL   u1_last_gpio_state;
static Enum_BacklightState  uE_BL_STATE;
static UINT16 cri_bloff_wait_ctr;

static void vDynBacklightRegInit(void);
static void cri_state_check(void);
static UINT8 cri_low_temp_chk(void);
static UINT8 cri_control_onoff_check(void);
static UINT16 vAutoBacklightSetPWMFreq(void);
static VOID cri_backlightoff_timeout(void);
static void backlightGpioSwitch(INT32 u1_bl_switch);
static BOOL backlight_gpio_status(void);


void blPwmSwitch(INT32 u1_switch, BOOL is24p)
{
    vAutoBacklightPwmSwitch(u1_switch);
    bl_sreg_write(SREG_BL_DIMMER_SEL, (UINT16)u1_switch);
    
    if(is24p == FALSE)
    {
      u1_last_gpio_state = (BOOL)u1_switch;
    }
}

/*
 * 
 * @param 
 * @return
 */
void vAutoBacklightInit(void)
{
    calc_dbc_init();
    vDynBacklightRegInit();
    calc_backlight_ctrl_opt_sensor_init_param();
}

/* Set Backlight State with MTK power sequence
 * 
 * @param 
 * @return 0 - OK 1 - Wrong sequence state   
 */
INT32 vAutoBacklightState(Enum_BacklightState u1_state, UINT16* u2_ret_ptr)
{ 
    /* This API is used to connect to MTK interface to turn on/off backlight */
    if(uE_BL_STATE != u1_state)
    {
        uE_BL_STATE = u1_state;
        u1_bl_state_change = TRUE;
    }    

    switch(u1_state)
    {
      case BL_INIT_STATE:
        sreg_write_from_flash();
        vAPIBacklightRegSet();
      case BL_OFF_STATE:
        u2_pwm_Freq = 0;
        u2_pwm_duty = 0;
        blPwmSwitch(MTK_PWM, FALSE);
        break;
      case BL_FIX_DUTY_STATE:
        u2_pwm_Freq = bl_sreg_read(SREG_BL_PWM_FRQ);
        u2_pwm_duty = ((bl_sreg_read(SREG_BL_REDUCTION)*255)/100);
        break;
      case BL_ON_STATE:
        if(u2_ret_ptr != NULL)
          *u2_ret_ptr = (bl_sreg_read(SREG_BL_REDUCTION_TIME)*10); 
        break;
      case BL_SWITCH_OVER_STATE:
        u2_pwm_Freq = bl_sreg_read(SREG_BL_PWM_FRQ);
        u2_pwm_duty = ((bl_sreg_read(SREG_BL_REDUCTION)*255)/100);
        blPwmSwitch(EXT_PWM, FALSE);
        if(u2_ret_ptr != NULL)
          *u2_ret_ptr = 0;
        break;
      case BL_STARTUP_LOGO_STATE:
        u2_pwm_Freq = bl_sreg_read(SREG_BL_PWM_FRQ);
        u2_pwm_duty = ((LOGO_DUTY*255)/100);
        break;
      default:
        break;
    }
    bl_sreg_write(SREG_BL_STATE, u1_state);
    
    if(u2_pwm_Freq < PWM_FRQ_DEADB)
    {
        u2_pwm_Freq = PWM_FRQ_DEADB;
    }
    
    return 0;
}

/*
 * 
 * @param 
 * @return
 */
void vAutoBacklightISR(void)
{
    ST_PARAM_DIMMER param;
    
    if((u2_current_pwm_Freq != u2_pwm_Freq) || (u2_current_pwm_duty != u2_pwm_duty) || (u1_bl_state_change == TRUE))
    {
        u1_bl_state_change = FALSE;
        u2_current_pwm_Freq = u2_pwm_Freq;
        u2_current_pwm_duty = u2_pwm_duty;
        
        param.u2_pwm_Freq = u2_current_pwm_Freq;
        param.u2_pwm_Duty = u2_current_pwm_duty;
        vAutoBacklightSetDimming(&param);
    }
}
extern INT32 DRVCUST_Gpio_Read(INT32 i4GpioNum);
/*
 * Update dimmer duty cycle and frequency
 * @param 
 * @return
 */
UINT32 vAutoBacklightProc(void)
{
    INT16 dynBacklight;
    INT16 dbc4ps_bl_cont;
    UINT16 OPT_BL_COEF;
    UINT8 lcdBacklightCtrl;
    UINT8 minDuty;
    UINT8 maxDuty;
    INT16 blGain;
    INT16 bl_duty;
    UINT8 bl_manu_en;
    UINT8 dimmerDuty;
    BOOL noSigGain;
    BOOL stat24p = FALSE;
    INT32 bl_status;
    BOOL osdOn = FALSE;
    
    opt_bl_ctrl_main();
    
    OPT_BL_COEF = calc_backlight_ctrl_opt_bl_iir();
    
    cri_ctrl_en = cri_control_onoff_check();
    
    bl_manu_en = bl_sreg_read(SREG_BL_MANU_EN);
    lcdBacklightCtrl = vAPIGetDimmingValue();
    maxDuty = bl_sreg_read(SREG_BL_MAX_DUTY);
    minDuty = bl_sreg_read(SREG_BL_MIN_DUTY);
    
    dynBacklight = calc_dbc_main();
    dbc4ps_bl_cont = calc_dbc4ps_main();
    bl_status = backlight_gpio_status();
    
    vAutoBacklightGetNoSigGain(&noSigGain);
    if(noSigGain)
    {
      blGain = bl_sreg_read(SREG_BL_NO_SIGNAL_GAIN);
    }
    else
    {
      blGain = (((INT16)lcdBacklightCtrl + dynBacklight - dbc4ps_bl_cont)) * (INT16)OPT_BL_COEF / 255;
    }
    
    blGain = BOUND(blGain, 0, BL_GAIN_MAX);
    bl_sreg_write(SREG_BL_GAIN, (UINT8)blGain);
    bl_duty = maxDuty - minDuty;
    bl_duty = BOUND_MIN(bl_duty, 0);
    dimmerDuty = ((UINT8)bl_duty * (UINT8)blGain / 255) + minDuty;
    
    if(uE_BL_STATE == BL_READY_STATE)
    {
        if(u1_dynBLenable == TRUE)
        { 
            if(bl_manu_en == 0)
            {
                if(cri_ctrl_en == CRI_CTRL_ON)
                {
                    if(bl_sreg_read(SREG_PANEL_TYPE) == PANEL_LED)
                    {    
						u2_pwm_Freq = bl_sreg_read(SREG_BL_PWM_FRQ);   
                        vGetBacklightOsdStatus(&osdOn);
                        cri_bloff_wait_ctr++;
                        if(osdOn == TRUE)
                        {
                            cri_bloff_wait_ctr = COUNTER_RESET;
                            if(bl_status == BACKLIGHT_OFF)
                            {
                                backlightGpioSwitch(BACKLIGHT_ON);
                            }
                        }
                        else if (bl_status == BACKLIGHT_OFF)
                        {
                            cri_bloff_wait_ctr = COUNTER_RESET;
                        }
						else if((cri_bloff_wait_ctr >= (bl_sreg_read(SREG_CRI_BLOFF_WAIT)*100)) && (bl_status == BACKLIGHT_ON))
                        {
                            cri_backlightoff_timeout();
                        }
                    }
                    else
                    {
                        u2_pwm_Freq = bl_sreg_read(SREG_BL_CRI_PWM_FRQ);
                    }
                    u2_pwm_duty = ((bl_sreg_read(SREG_BL_CRI_DUTY)*255)/100);
                }
                else
                {
                    u2_pwm_Freq = vAutoBacklightSetPWMFreq();
                    u2_pwm_duty = dimmerDuty;
                    if(bl_sreg_read(SREG_PANEL_TYPE) == PANEL_LED)
                    {
                        cri_bloff_wait_ctr = COUNTER_RESET;
                        if(vAPIGetPowerSavingStatus() != ECO_BLACKOUT)
                        {
                            if(bl_status == BACKLIGHT_OFF)
                            {
                                backlightGpioSwitch(BACKLIGHT_ON);
                            }
                        }
                    }
                }
            }
            else
            {
                u2_pwm_Freq = vAutoBacklightSetPWMFreq();
                u2_pwm_duty = bl_sreg_read(SREG_FACT_BL_GAIN);
            }
        }
        else
        {
            u2_pwm_Freq = bl_sreg_read(SREG_BL_PWM_FRQ);
            u2_pwm_duty = bl_sreg_read(SREG_BL_AGING_GAIN);
        }
        
        vAutoBacklightGet24P(&stat24p);
        if(stat24p)
        {
          blPwmSwitch(MTK_PWM, TRUE);
        }
        else
        {
          if(bl_sreg_read(SREG_DEEP_DIMMER_TH_EN))
          {
            if((bl_sreg_read(SREG_DEEP_DIMMER_TH_H)) < (bl_sreg_read(SREG_DIMMER_DUTY)))
            {
              blPwmSwitch(MTK_PWM, FALSE);
            }
            else if((bl_sreg_read(SREG_DEEP_DIMMER_TH_L)) > (bl_sreg_read(SREG_DIMMER_DUTY)))
            {
              blPwmSwitch(EXT_PWM, FALSE);
            }
            else
            {
              blPwmSwitch(u1_last_gpio_state, FALSE);
            }          
          }
          else
          {
            blPwmSwitch(u1_last_gpio_state, FALSE);
          }              
        }
        bl_sreg_write(SREG_DIMMER_DUTY, u2_pwm_duty); 
    }
    /* PWM frequency checking, cannot below than 32 Hz */
    if(u2_pwm_Freq < PWM_FRQ_DEADB)
    {
        u2_pwm_Freq = PWM_FRQ_DEADB;
    }

    return 0;
}

/*
 * Control enable/disable dyn backlight control
 * @param 
 * @return
 */
void vDynBacklightCtrlEnable(DBL_EN_CTRL value)
{
    switch(value)
    {
        case DYN_BL_DISABLE:
            u1_dynBLenable = FALSE;
            break;

        case DYN_BL_ENABLE:
        default:
            u1_dynBLenable = TRUE;
            break;
    }
}

/*
 * Initialize Dynamic backlight register
 * @param 
 * @return
 */
static void vDynBacklightRegInit(void)
{
    u2_pwm_Freq = 0;
    u2_pwm_duty = 0;
    u2_current_pwm_Freq = 0;
    u2_current_pwm_duty = 0;
    u1_bl_state_change = 0;
    u1_dynBLenable = TRUE;
     
    uE_BL_STATE = BL_INIT_STATE;
    
    s_w_cri_state = CRI_STATE_INIT;
    s_w_last_cri_state = CRI_STATE_INIT;
    cri_ctrl_en = CRI_CTRL_OFF;
    cri_bloff_wait_ctr = COUNTER_RESET;  
}

/*
 * CRI on/off condition check
 * @param 
 * @return
 */
static UINT8 cri_control_onoff_check(void)
{
    UINT8 bl_gain, bl_cri_onoff;
    
    bl_gain = bl_sreg_read(SREG_BL_GAIN);
    bl_cri_onoff = bl_sreg_read(SREG_BL_CRI_ONOFF);
    
    cri_state_check();
    
    if((bl_sreg_read(SREG_DBC4PS_PIC_STATE) == 1) && (bl_gain == 0)
        && (bl_cri_onoff == 1) && (cri_low_temp_chk() == CRI_TEMP_OK)) {
        return CRI_CTRL_ON;
    }
    return CRI_CTRL_OFF;
}

/*
 * Checking CRI temp state
 * @param 
 * @return
 */
static UINT8 cri_low_temp_chk(void)
{      
    if(s_w_cri_state == CRI_STATE_IMPOS) {
        return CRI_TEMP_NG;
    }
    return CRI_TEMP_OK;
}

/*
 * Checking CRI state
 * @param 
 * @return
 */
static void cri_state_check(void)
{
    UINT16 readTemp = 0;
    
    readTemp = (UINT8) (vAPIGetTempSensorValue() / 10); 
    
    if(readTemp >= (bl_sreg_read(SREG_BL_CRI_TEMP_H)/10)) {
        s_w_cri_state = CRI_STATE_POS;
    } else if(readTemp <= (bl_sreg_read(SREG_BL_CRI_TEMP_L)/10)) {
        s_w_cri_state = CRI_STATE_IMPOS;
    } else {
        s_w_cri_state = s_w_last_cri_state;
    }

    if(s_w_last_cri_state != s_w_cri_state) {
        s_w_last_cri_state = s_w_cri_state;      
    }
}

static UINT16 vAutoBacklightSetPWMFreq(void)
{
    BOOL stat24p = FALSE;
    
    vAutoBacklightGet24P(&stat24p);
    if(stat24p) 
    {
        return bl_sreg_read(SREG_BL_24P_PWM_FRQ);
    } 
    return bl_sreg_read(SREG_BL_PWM_FRQ);
}

static VOID cri_backlightoff_timeout(void)
{
    cri_bloff_wait_ctr = COUNTER_RESET;
    backlightGpioSwitch(BACKLIGHT_OFF);
}

static BOOL backlight_gpio_status(void)
{
    BOOL bl_gpio_status;
    
    vGetBacklightGetGpioStatus(&bl_gpio_status);    
    return bl_gpio_status;     
}

static void backlightGpioSwitch(INT32 u1_bl_switch)
{
    vSetBacklightGpioOffOn(u1_bl_switch);  
}

