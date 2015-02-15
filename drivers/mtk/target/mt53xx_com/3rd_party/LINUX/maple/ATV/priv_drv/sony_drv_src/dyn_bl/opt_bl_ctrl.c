/* ========================================================================== */
/*                                                                            */
/*   opt_bl_ctrl.c                                                               */
/*   (c) 2009                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include "x_dbg.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
//#include "acfg_atv_eep.h"
#include "ostg_if.h"
#include "eeprom_if.h"
#include "sony_priv_inc/rgb_reg.h"
#include "sony_priv_inc/bl_reg.h"
#include "sony_priv_inc/opt_bl_ctrl.h"

//-----------------------------------------------------------------------------
//                              CONSTANT DEFINITIONS
//-----------------------------------------------------------------------------
#define OPT_BL_COEF_MAX 255
#define SORT_MAX        255

#define FW_RESULT_OK    1
#define FW_RESULT_NG    0

#define PICTURE_MODE_NORMAL               0
#define PICTURE_MODE_CINEMA               1

#define V_SYNC_CTR      5

//-----------------------------------------------------------------------------
//                              TYPE DEFINITIONS
//-----------------------------------------------------------------------------
typedef struct{
	UINT8  dead_band;
	UINT8  th0;
	UINT8  th1;
	UINT8  th2;
	UINT8  th3;
	UINT8  end_tgt1;
	UINT8  end_tgt2;
	UINT8  end_tgt3;
	UINT8  iir;	
	UINT8  sensor_en;
	UINT8  sensor_avg;

}ST_PARAM_OPT_REG;

enum{
    UI_PICTURE_MODE_VIVID,
    UI_PICTURE_MODE_STANDARD,
    UI_PICTURE_MODE_CUSTOM,
    UI_PIC_MODE_DUMMY1,
    UI_PICTURE_MODE_CINEMA,
    UI_PICTURE_MODE_SPORTS,
    UI_PICTURE_MODE_GAME,
    UI_PIC_MODE_DUMMY2,
    UI_PICTURE_MODE_GRAPHICS
};

//typedef UINT32  FW_ERROR;
//typedef UINT32  FW_RESULT;

//-----------------------------------------------------------------------------
//                              GLOBAL VARIABLE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                              STATIC VARIABLE
//-----------------------------------------------------------------------------
static UINT8 opt_bl_ctrl_cmd_set_opt_sensor_avg(void);
static void opt_bl_calc_out_avg(void);
static void calc_backlight_ctrl_opt_reg_copy(ST_PARAM_OPT_REG *p_param);
const ST_PARAM_OPT_REG *calc_backlight_ctrl_opt_reg_get_reg(void);

static UINT8 m_optSensorReq;
static UINT8 m_isOptSensorOn;       // OpticalSensor On/Off Flag
static UINT8 m_isOptReadComp;       // OpticalSensor Read Compleate Flag
static UINT8 m_OptNowCount;         // OpticalCount
static UINT8 m_OptAvgCount;         // OpticalAveregCount
static UINT8 m_OptReadData[255];    // OpticalAveregData
static UINT8 ui1_picMode;
static UINT8 v_sync_ctr;  
static ST_PARAM_OPT_REG	    s_r_opt_params;
static UINT16				s_w_opt_bl_coef_tgt;


/**
 *  Optical backlight ctr main function
 *  @param
 *  @return 
 *  @author
 */
void opt_bl_ctrl_main(void)
{
    v_sync_ctr++;
    if(v_sync_ctr == V_SYNC_CTR)
    {
        v_sync_ctr = 0;
        opt_bl_calc_out_avg();
        opt_bl_ctrl_cmd_set_opt_sensor_avg();
    }
}

/**
 *  Optical backlight IIR filter
 *  @param
 *  @return 
 *  @author
 */
UINT8 calc_backlight_ctrl_opt_bl_iir(void)
{
	const ST_PARAM_OPT_REG *p_reg = calc_backlight_ctrl_opt_reg_get_reg();
	static UINT16 opt_bl_coef = 255;

	if ( p_reg->sensor_en )
	{
		/*  IIR filters */
		if ( ( ( s_w_opt_bl_coef_tgt - p_reg->dead_band) > opt_bl_coef ) ||
			   ( opt_bl_coef > ( s_w_opt_bl_coef_tgt + p_reg->dead_band) ) )
		{
			opt_bl_coef = ( ( opt_bl_coef * p_reg->iir ) + ( s_w_opt_bl_coef_tgt * ( 255 - p_reg->iir ) ) ) / 255;
		}
	}
	else
	{
        opt_bl_coef = OPT_BL_COEF_MAX;
    }
    bl_sreg_write(SREG_OPT_BL_COEF, opt_bl_coef);
    
    return opt_bl_coef;
}

/**
 *  backlight
 *  @param  modh        handle to module.
 *  @return 
 *  @author
 */
static UINT8 opt_bl_ctrl_cmd_set_opt_sensor_avg(void)
{
	ST_PARAM_OPT_REG params;
	UINT8 opt_bl_coef;
	UINT8 result;

    if(ui1_picMode == PICTURE_MODE_CINEMA)
    {
        params.dead_band = (UINT8)bl_sreg_read(SREG_OPT_BL_DEADB_CIN);
    	params.th0       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH0_CIN);
    	params.th1       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH1_CIN);
    	params.th2       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH2_CIN);
    	params.th3       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH3_CIN);
    	params.end_tgt1  = (UINT8)bl_sreg_read(SREG_OPT_BL_END_TGT1_CIN);
    	params.end_tgt2  = (UINT8)bl_sreg_read(SREG_OPT_BL_END_TGT2_CIN);
    	params.end_tgt3  = (UINT8)bl_sreg_read(SREG_OPT_BL_END_TGT3_CIN);
    	params.iir       = (UINT8)bl_sreg_read(SREG_OPT_BL_IIR_CIN); 
    }
    else   /* Other than Cinema mode use same table */
    {
    	params.dead_band = (UINT8)bl_sreg_read(SREG_OPT_BL_DEADB);
    	params.th0       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH0);
    	params.th1       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH1);
    	params.th2       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH2);
    	params.th3       = (UINT8)bl_sreg_read(SREG_OPT_BL_TH3);
    	params.end_tgt1  = (UINT8)bl_sreg_read(SREG_OPT_BL_END_TGT1);
    	params.end_tgt2  = (UINT8)bl_sreg_read(SREG_OPT_BL_END_TGT2);
    	params.end_tgt3  = (UINT8)bl_sreg_read(SREG_OPT_BL_END_TGT3);
    	params.iir       = (UINT8)bl_sreg_read(SREG_OPT_BL_IIR);
    }

	params.sensor_avg  = (UINT16)bl_sreg_read(SREG_OPT_BL_OUT_AVG);
    params.sensor_en   = (UINT8)bl_sreg_read(SREG_OPT_BL_SENSOR);

	calc_backlight_ctrl_opt_reg_copy(&params);
	opt_bl_coef = calc_backlight_ctrl_opt_bl_coef();
	bl_sreg_write(SREG_OPT_BL_COEF_TGT, opt_bl_coef);

#if 0
	if ( FW_CAUSE_MODULE == fw_mod_get_command_cause( modh ) )
	{
		if( p_rqst_param->payload[BACKLIGHT_CTRL_PARAM_OPT_SENSOR_CALC_MODE] == BACKLIGHT_CTRL_PARAM_DATA_OPT_SENSOR_IDX )
		{
			FW_CMDPARAM reply_param;
			reply_param.payload[BACKLIGHT_CTRL_REPLY_OPT_PICTURE_MODE] = APP_MC_BACKLIGHT_CTRL_OK;
			fw_mod_reply_command(modh, CMDID_BACKLIGHT_CTRL_SET_OPT_SENSOR_AVG, &reply_param);
		}
	}
#endif
	result = FW_RESULT_OK;
    
	return result;
}

/**
 *  Calc optical backlight coef tgt
 *  @param
 *  @return 
 *  @author
 */
UINT8 calc_backlight_ctrl_opt_bl_coef(void)
{
	const ST_PARAM_OPT_REG *p_reg = calc_backlight_ctrl_opt_reg_get_reg();
	UINT8 opt_bl_coef;
	INT16 x,a,b,c,d;
	 
	if ( p_reg->sensor_en )
	{
		if ( p_reg->sensor_avg >= p_reg->th0 )
		{
			opt_bl_coef = OPT_BL_COEF_MAX;   // equivalent to optical sensor Off.
		}
		else if ( p_reg->sensor_avg <= p_reg->th3 )
		{
			opt_bl_coef = p_reg->end_tgt3;
		}
		else
		{
			if ( p_reg->sensor_avg >= p_reg->th1 )	/* TH1 <= AVG < TH0 */
			{
				a = p_reg->th1;
				b = p_reg->th0;
				c = OPT_BL_COEF_MAX;
				d = p_reg->end_tgt1;
			}
			else if ( p_reg->sensor_avg >= p_reg->th2 )	/* TH2 <= AVG < TH1 */
			{
				a = p_reg->th2;
				b = p_reg->th1;
				c = p_reg->end_tgt1;
				d = p_reg->end_tgt2;
			}
			else	/* TH3 < AVG < TH2 */
			{
				a = p_reg->th3;
				b = p_reg->th2;
				c = p_reg->end_tgt2;
				d = p_reg->end_tgt3;
			}

			x = p_reg->sensor_avg;
			if ( ( x <= a ) || ( c <= d ) || ( b <= a ) )
			{
				opt_bl_coef = (UINT8)d;
			}
			else
			{
				opt_bl_coef = (UINT8)(( x - a ) * ( c - d )/( b - a ) + d );
			}
		}
	}
	else
	{
		opt_bl_coef = 255;
	}
	s_w_opt_bl_coef_tgt = opt_bl_coef;

	return s_w_opt_bl_coef_tgt;
}

/**
 *  Calc optical backlight coef tgt
 *  @param
 *  @return 
 *  @author
 */
static void opt_bl_calc_out_avg(void)
{
    UINT8 i,j,tmp = 0,min = 0 ,min_cnt = 0,limit_cut = 0;
    UINT16 optavg = 0,decrease = 0;
    UINT8 sort[SORT_MAX];
    
    m_OptAvgCount = bl_sreg_read(SREG_OPT_BL_COUNT);
    limit_cut = bl_sreg_read(SREG_OPT_BL_CUTMINMAX);
    
    if( m_OptAvgCount < ( bl_sreg_read(SREG_OPT_BL_CUTMINMAX) * 2 + 1 ) ) {
        m_OptAvgCount = ( bl_sreg_read(SREG_OPT_BL_CUTMINMAX) * 2 + 1 );
        bl_sreg_write( SREG_OPT_BL_COUNT, m_OptAvgCount );
    }
    if(( m_isOptSensorOn == 1 ) && ( m_isOptReadComp == 1 ) ) {
        for(i = 0;i < (m_OptAvgCount); i++){
            optavg += m_OptReadData[i];
            sort[i] = m_OptReadData[i];
        }
        for(i = 0;i < m_OptAvgCount-1; i++){
          min = sort[i];
          min_cnt = i;
          for(j = i+1; j < m_OptAvgCount; j++){
              if(sort[j] < min){
                  min = sort[j];
                  min_cnt = j;
              }
          }
          tmp = sort[i];
          sort[i] = min;
          sort[min_cnt] = tmp;
        }
        tmp = m_OptAvgCount - limit_cut;
        for( i = 0;i < limit_cut; i++){
            decrease += ( sort[i] + sort[i+tmp] );
        }
        optavg = ( optavg - decrease ) / ( m_OptAvgCount - ( limit_cut * 2 ) );
        
        // TimerLED On Check   
        #if 0
        if( ( io_read(O_LED_ON_TIMER) == IO_HIGH ) || ( io_read(O_LED_PIC_MUTE) == IO_HIGH ) ) {
            if( optavg >= sreg_read(SREG_OPT_TIMER_LED) ){
                optavg = ( optavg - sreg_read(SREG_OPT_TIMER_LED) );
            } else {
                optavg = 0;
            }
        }
        #endif
               
        if( optavg !=  bl_sreg_read(SREG_OPT_BL_OUT_AVG) ) {
            if(m_optSensorReq) {
              #ifdef DBL_ENABLE_LOG
                DBL_LOG("Reply OptOut_Avg:[%d]\n", optavg);
              #endif
            }
        }
        bl_sreg_write( SREG_OPT_BL_OUT_AVG, optavg );
    }
    
    m_OptReadData[m_OptNowCount] = RGB_GetOptReadData();
    m_OptNowCount++;
    if( m_OptNowCount >= m_OptAvgCount ) {
        m_OptNowCount = 0;
        m_isOptReadComp = 1;
    }
    
    #if 0
    } else {
        /* Do Nothing */
        // Long Push Mode Timer Set
        if (m_optSensorTimerId != TIMER_ID_INVALID) {
            m_pTskSensor->StopTskTimer(&m_optSensorTimerId);
            m_optSensorTimerId = TIMER_ID_INVALID;
        }
        m_optSensorTimerId = 
            m_pTskSensor->StartTskTimer( LONG_PUSH_TIMER, TIMER_EVENT_STATUS_REMOCON, false, this);
    }
    #endif
}

/**
 *  Optical backlight control param init
 *  @param
 *  @return 
 *  @author
 */
void calc_backlight_ctrl_opt_sensor_init_param( void )
{
	s_w_opt_bl_coef_tgt = OPT_BL_COEF_MAX;
	m_isOptSensorOn = 1;       // OpticalSensor On/Off Flag
    m_isOptReadComp = 0;       // OpticalSensor Read Compleate Flag
    m_OptNowCount = 0;         // OpticalCount
    m_optSensorReq = 1;
    m_OptAvgCount = 0; 
    v_sync_ctr = 0;
    ui1_picMode = PICTURE_MODE_NORMAL; //Temporary
}

/**
 *  Copy all parameter value for opt bl ctrl
 *  @param
 *  @return 
 *  @author
 */
static void calc_backlight_ctrl_opt_reg_copy(ST_PARAM_OPT_REG *p_param)
{
	s_r_opt_params = *p_param;
}

/**
 *  Get all parameter value for opt bl ctrl
 *  @param
 *  @return 
 *  @author
 */
const ST_PARAM_OPT_REG *calc_backlight_ctrl_opt_reg_get_reg( void )
{
	return &s_r_opt_params;
}

/**
 *  Check User setting Pic Mode status
 *  @param
 *  @return 
 *  @author
 */
void vUserPictureModeStatus(UINT8 ui1_value)
{
    switch(ui1_value)
    {
        case UI_PICTURE_MODE_CINEMA:
            ui1_picMode = PICTURE_MODE_CINEMA;
            break;
        
        case UI_PICTURE_MODE_VIVID:
        case UI_PICTURE_MODE_STANDARD:
        case UI_PICTURE_MODE_CUSTOM:
        case UI_PICTURE_MODE_SPORTS:     
        case UI_PICTURE_MODE_GAME:
        case UI_PICTURE_MODE_GRAPHICS:
        case UI_PIC_MODE_DUMMY1:
        case UI_PIC_MODE_DUMMY2:
        default:    
            ui1_picMode = PICTURE_MODE_NORMAL;
            break;
    }
    bl_sreg_write(SREG_OPT_BL_INDEX, ui1_picMode);
}

