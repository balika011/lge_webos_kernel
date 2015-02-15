/* ========================================================================== */
/*                                                                            */
/*   bl_dbc.c                                                                 */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_os.h"
#include "acfg_atv_eep.h"

#include "bl_util.h"
#include "bl_dbc.h"
#include "sony_drv_inc/a_bl_ctrl.h"
#include "sony_drv_inc/Sony_drv_cb.h"
#include "sony_priv_inc/bl_reg.h"

#define CHIST_PERCENT 95

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
enum{
  BL_ACE_OFF = 0,
  BL_ACE_LOW,
  BL_ACE_MID,
  BL_ACE_HIGH
};

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct{
	UINT8  dbc_bl_down;
	UINT16 apl_dbc;
	UINT8  dbc_apl;
	UINT16 max_dbc;
	UINT16 dbc_max;
}ST_PARAM_DBC_SREG;

typedef struct{
	UINT8	iir_clr_times;		/* SREG_IIR_CLR_TIMES */
}ST_PARAM_DYNAMIC_COMMON;

typedef struct{
	BOOL	  dbc_apl_en;
	UINT8   dbc_max_thr_i;
	UINT8   dbc_max_det_th;
	UINT8   dbc_max_iir;
	BOOL	  dbc_max_en;
	UINT16	dbc_max_th1;
	UINT16	dbc_max_th2;
}ST_PARAM_DBC;

typedef struct{
	BOOL	  dbc4ps_pic_en;
	UINT8   dbc4ps_bl_b_inc;
	UINT8   dbc4ps_bl_b_dec;
	UINT8   dbc4ps_pic_int_1;
	UINT8   dbc4ps_pic_int_2;
	UINT8   bl_gain;
	UINT8   dbc4ps_lumax_th;
	UINT16  dbc4ps_con_th;
}ST_PARAM_DBC4PS;

typedef struct{
	BOOL    dbc4ps_pic_state;
	UINT8   dbc4ps_chist;
	UINT8   dbc4ps_bl_cont;
	UINT8	  dbc4ps_luma_max;
	UINT16  dbc4ps_counter;
}ST_PARAM_DBC4PS_SREG;

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT8  s_w_remaining_iir_clr_times;	/* remaining iir clr times. if this register is not zero, iir is disabled. */

static UINT8  s_w_dbc_bl_down;
static INT16  s_w_dbc;
static UINT16 s_w_apl_dbc;
static UINT16 s_w_max_dbc;
static INT16  s_w_dbc1;
static UINT32 s_w_apldbc_iir;

static INT16  s_w_dbc4ps;
static UINT8  s_w_dbc4ps_int;
static INT16  s_w_loop_count;
static INT32  s_w_black_count;

static ST_PARAM_DBC_SREG	s_w_dbc_sreg;

static ST_PARAM_DBC s_r_dbc_params;
static ST_PARAM_UTIL_MATH_STEP_FUNC_TH    s_r_dbc_apl;
static ST_PARAM_UTIL_MATH_STEP_FUNC_OFST  s_r_dbc_offset;
static ST_PARAM_UTIL_MATH_STEP_FUNC_TH    s_r_dbc_apldet_th;
static ST_PARAM_UTIL_MATH_STEP_FUNC_OFST  s_r_dbc_apldet_offset;
static ST_PARAM_DYNAMIC_COMMON		        s_r_dynamic_common_params;

static ST_PARAM_DBC4PS      s_r_dbc4ps_params;
static ST_PARAM_DBC4PS_SREG	s_w_dbc4ps_sreg;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
 *      Static Function Prototypes
 *---------------------------------------------------------------------------*/
static void calc_dbc_main_init_params( ST_PARAM_DBC_MAIN *params );
static void calc_dbc_calculate_dbc_bl_down( ST_PARAM_DBC_MAIN *params );
static void calc_dbc_calculate_apl_detection( UINT16 aplm, BOOL scene_change_det );
static INT16 calc_dbc_get_alpha_apldbc(BOOL scene_change_det, UINT16 delta_apldbc);
static void calc_dbc_calculate_max_detection(UINT16 *raw_histy);
static void calc_dbc_calculate_cal_dbc1( void ); 
static INT16 calc_dbc_calculate_apl_dbc( void );
static UINT16 calc_dbc_calculate_max_dbc( void );
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

/**
 *  Get Parameters of Initial
 *  @param  : 
 *  @return : void
 *  @author : Kazuaki Okada
 */
static void calc_dbc_main_init_params( ST_PARAM_DBC_MAIN *params )
{
    vAutoBacklightGetDbcParam(params);
    
    s_r_dbc_apl.th1    = (INT32)bl_sreg_read(SREG_DBC_APL_TH1);          /* SREG_DAPLM_DBC_TH1 */
    s_r_dbc_apl.th2    = (INT32)bl_sreg_read(SREG_DBC_APL_TH2);         /* SREG_DAPLM_DBC_TH2 */
    s_r_dbc_apl.th3    = (INT32)bl_sreg_read(SREG_DBC_APL_TH3);         /* SREG_DAPLM_DBC_TH3 */
    s_r_dbc_apl.th4    = (INT32)bl_sreg_read(SREG_DBC_APL_TH4);         /* SREG_DAPLM_DBC_TH4 */
    s_r_dbc_offset.l   = -( (INT32)bl_sreg_read(SREG_DBC_APL_BL_BLIM) );    /* SREG_DBC_APL_BL_BLIM */
    s_r_dbc_offset.m   = 0;
    s_r_dbc_offset.h   = (INT32)bl_sreg_read(SREG_DBC_APL_BL_TLIM);         /* SREG_DBC_APL_BL_TLIM */

    s_r_dbc_apldet_th.th1   = (INT32)bl_sreg_read(SREG_DAPLM_TH1);       /* SREG_DAPLM_DBC_TH1 */
    s_r_dbc_apldet_th.th2   = (INT32)bl_sreg_read(SREG_DAPLM_TH2);       /* SREG_DAPLM_DBC_TH2 */
    s_r_dbc_apldet_th.th3   = (INT32)bl_sreg_read(SREG_DAPLM_TH3);       /* SREG_DAPLM_DBC_TH3 */
    s_r_dbc_apldet_th.th4   = (INT32)bl_sreg_read(SREG_DAPLM_TH4);       /* SREG_DAPLM_DBC_TH4 */       
    s_r_dbc_apldet_offset.l = (INT32)bl_sreg_read(SREG_DBC_APL_IIR_L);     /* SREG_DBC_APL_IIR_L */
    s_r_dbc_apldet_offset.m = (INT32)bl_sreg_read(SREG_DBC_APL_IIR_M);     /* SREG_DBC_APL_IIR_M */
    s_r_dbc_apldet_offset.h = (INT32)bl_sreg_read(SREG_DBC_APL_IIR_H);     /* SREG_DBC_APL_IIR_H */
    
    s_r_dbc_params.dbc_apl_en     = (BOOL)bl_sreg_read(SREG_DBC_APL_EN);            /* SREG_DBC_APL_EN */  
    s_r_dbc_params.dbc_max_thr_i  = (BOOL)bl_sreg_read(SREG_DBC_MAX_THR_I);         /* SREG_DBC_MAX_THR_I */ 
    s_r_dbc_params.dbc_max_det_th = bl_sreg_read(SREG_DBC_MAX_THR);                 /* SREG_DBC_MAX_THR */  
    s_r_dbc_params.dbc_max_iir    = bl_sreg_read(SREG_DBC_MAX_IIR);                 /* SREG_DBC_MAX_IIR */ 
    s_r_dbc_params.dbc_max_en     = (BOOL)bl_sreg_read(SREG_DBC_MAX_EN);            /* SREG_DBC_MAX_EN */
    s_r_dbc_params.dbc_max_th1    = bl_sreg_read(SREG_DBC_MAX_THR1);                /* SREG_DBC_MAX_THR1 */
    s_r_dbc_params.dbc_max_th2    = bl_sreg_read(SREG_DBC_MAX_THR2);                /* SREG_DBC_MAX_THR2 */
    
    s_r_dynamic_common_params.iir_clr_times = (UINT8)bl_sreg_read(SREG_IIR_CLR_TIMES); /* SREG_IIR_CLR_TIMES */
}

static void calc_dbc_calculate_dbc_bl_down( ST_PARAM_DBC_MAIN *params )
{
    BOOL dbcEnable;
    
    vAutoBacklightGetDbcEnable(&dbcEnable);
    
    if( dbcEnable ){
      calc_dbc_calculate_apl_detection( params->aplm, params->scene_change_det );
    
      calc_dbc_calculate_max_detection( params->wHist32 );
    
      calc_dbc_calculate_cal_dbc1();
    }else{
      s_w_dbc = 0;
      
      s_w_dbc_sreg.apl_dbc      = 0;
      s_w_dbc_sreg.dbc_apl      = 0;
      s_w_dbc_sreg.max_dbc      = 0;
      s_w_dbc_sreg.dbc_max      = 0;
      s_w_dbc_sreg.dbc_bl_down  = 0;
    }   
}

static void calc_dbc_calculate_apl_detection( UINT16 aplm, BOOL scene_change_det )
{
    UINT16 apl_dbc;
    UINT16 delta_apldbc;
    INT16 alpha_apldbc;

    delta_apldbc = (UINT16) ABS((INT32)aplm - (INT32)s_w_apl_dbc);
    alpha_apldbc = calc_dbc_get_alpha_apldbc( scene_change_det, delta_apldbc );

    apl_dbc     = (UINT16)util_math_iir_high_precision_ex( alpha_apldbc, aplm, &s_w_apldbc_iir );

    s_w_dbc_sreg.apl_dbc = apl_dbc;
    s_w_apl_dbc = apl_dbc;
}

static INT16 calc_dbc_get_alpha_apldbc(BOOL scene_change_det, UINT16 delta_apldbc)
{
    INT16 alpha_apldbc;

    if( scene_change_det != FALSE){
      s_w_remaining_iir_clr_times = s_r_dynamic_common_params.iir_clr_times;
    }/* Else Do Nothing */

    if( s_w_remaining_iir_clr_times > 0 ){
      alpha_apldbc = (INT16)IIR_NO_EFFECT_COEF;
      s_w_remaining_iir_clr_times--;
    }else{
      alpha_apldbc = (INT16)util_math_step_func( (INT32)delta_apldbc, &s_r_dbc_apldet_th, &s_r_dbc_apldet_offset);
    }

    return alpha_apldbc; /* [-1,255] */
}

static void calc_dbc_calculate_max_detection( UINT16 *raw_histy )
{
	UINT32 dbc_max_sum, dbc_max_thr;
	UINT16 dbc_max_det = 0;
	UINT16 max_dbc;
	UINT8  dbc_max_iir;
	UINT8  dbc_max_thr_i;
	INT16 i;

	dbc_max_thr = s_r_dbc_params.dbc_max_det_th;
	dbc_max_thr_i = s_r_dbc_params.dbc_max_thr_i;
	dbc_max_sum = 0;
	for( i = ( Y_HISTGRAM_POINT_NUM - 1 ); i >= 0 ; i-- ){
		dbc_max_sum += raw_histy[i];
		if( dbc_max_sum >= dbc_max_thr ){
			if( i >= dbc_max_thr_i ){
				dbc_max_det = (UINT16)( ( ( i * 5 ) - 28 ) * 8 );
			} else {
				dbc_max_det = (UINT16)( ( i * 25 ) - 23 );
			}
			break;
		}
	}
	dbc_max_iir = s_r_dbc_params.dbc_max_iir;
	max_dbc = (UINT16)util_math_iir( dbc_max_iir, dbc_max_det, s_w_max_dbc );
	s_w_dbc_sreg.max_dbc = max_dbc;
  s_w_max_dbc = max_dbc;
}

static void calc_dbc_calculate_cal_dbc1( void )
{
	INT16 dbc_apl_bk;

	dbc_apl_bk = calc_dbc_calculate_apl_dbc( );

  dbc_apl_bk = BOUND(dbc_apl_bk, -255, 255);

  s_w_dbc1 = (INT16)( ( dbc_apl_bk * (INT32)calc_dbc_calculate_max_dbc( ) ) / 255 );		
  	
	if( dbc_apl_bk < 0 ){
		s_w_dbc_bl_down = (UINT8)( -s_w_dbc1 );
	} else {
		s_w_dbc_bl_down = s_w_dbc1;
	}
	
	s_w_dbc_sreg.dbc_bl_down = s_w_dbc_bl_down;
	s_w_dbc = s_w_dbc1;
}

static INT16 calc_dbc_calculate_apl_dbc( void )
{
	INT32 dbc_apl;
	BOOL  dbc_apl_en;
	INT32 apl_dbc;

	dbc_apl_en = s_r_dbc_params.dbc_apl_en;
	apl_dbc    = (INT32)s_w_apl_dbc;

  if( dbc_apl_en == TRUE ){
		dbc_apl = util_math_step_func( apl_dbc, &s_r_dbc_apl, &s_r_dbc_offset );
	} else {
		dbc_apl = 0;
	}
	
	if( dbc_apl < 0 ){
		s_w_dbc_sreg.dbc_apl = (UINT8)( -dbc_apl );
	} else {

		s_w_dbc_sreg.dbc_apl = dbc_apl;
	}
	
	return (INT16)dbc_apl;
}

static UINT16 calc_dbc_calculate_max_dbc( void )
{
	UINT16 dbc_max, max_dbc, dbc_max_th1, dbc_max_th2, th;

	BOOL  dbc_max_en;

	dbc_max_en  = s_r_dbc_params.dbc_max_en;
	max_dbc     = s_w_max_dbc;
	dbc_max_th1 = s_r_dbc_params.dbc_max_th1;
	dbc_max_th2 = s_r_dbc_params.dbc_max_th2;

	if( dbc_max_en == TRUE ){
		if( max_dbc <= dbc_max_th1 ){    /* SWToDo takahashi ‹¤’Ê‰» */
			dbc_max = 255;
		} else if ( max_dbc < dbc_max_th2 ){
			th = dbc_max_th2 - dbc_max_th1;
			if( th != 0 ){
				dbc_max = ( 255 * ( dbc_max_th2 - max_dbc ) ) / th;
			} else {
				dbc_max = 255 * ( dbc_max_th2 - max_dbc );
			}
		} else {
			dbc_max = 0;
		}
	} else {
		dbc_max = 255;
	}
	
	s_w_dbc_sreg.dbc_max = dbc_max;
	
	return dbc_max;
}

static void calc_dbc_main_update_status( ST_PARAM_DBC_MAIN *params)
{
    bl_sreg_write(SREG_APL_LUMA, (UINT16)params->aplm);
    bl_sreg_write(SREG_SCENE_CHANGE_DET, (UINT16)params->scene_change_det);
    bl_sreg_write(SREG_APL_DBC, (UINT16)s_w_dbc_sreg.apl_dbc);
    bl_sreg_write(SREG_DBC_APL, (UINT16)s_w_dbc_sreg.dbc_apl);
    bl_sreg_write(SREG_MAX_DBC, (UINT16)s_w_dbc_sreg.max_dbc);
    bl_sreg_write(SREG_DBC_MAX, (UINT16)s_w_dbc_sreg.dbc_max);
    bl_sreg_write(SREG_DYNAMIC_BACKLIGHT, (UINT16)s_w_dbc_sreg.dbc_bl_down);
}

//-----------------------------------------------------------------------------
//                              DBC4PS
//-----------------------------------------------------------------------------

static void calc_dbc4ps_main_init_params( ST_PARAM_DBC_MAIN *params )
{
    BOOL is_pc_hdmi_pc = FALSE;
    
    vAutoBacklightGetDbcParam(params);
    vSonyIsPcHdmiPc(&is_pc_hdmi_pc);
 
    s_r_dbc4ps_params.dbc4ps_pic_en      = (BOOL)bl_sreg_read(SREG_DBC4PS_PIC_EN);            /* SREG_DBC4PS_PIC_EN */  
    s_r_dbc4ps_params.dbc4ps_bl_b_inc    = (UINT8)bl_sreg_read(SREG_DBC4PS_BL_B_INC);         /* SREG_DBC4PS_BL_B_INC */ 
    s_r_dbc4ps_params.dbc4ps_bl_b_dec    = (UINT8)bl_sreg_read(SREG_DBC4PS_BL_B_DEC);         /* SREG_DBC4PS_BL_B_DEC */ 
    s_r_dbc4ps_params.dbc4ps_pic_int_1   = (UINT8)bl_sreg_read(SREG_DBC4PS_PIC_INT_1);        /* SREG_DBC4PS_PIC_INT_1 */ 
    s_r_dbc4ps_params.dbc4ps_pic_int_2   = (UINT8)bl_sreg_read(SREG_DBC4PS_PIC_INT_2);        /* SREG_DBC4PS_PIC_INT_2 */  
    s_r_dbc4ps_params.bl_gain            = (UINT8)bl_sreg_read(SREG_BL_GAIN);                 /* SREG_BL_GAIN */
    s_r_dbc4ps_params.dbc4ps_con_th      = (UINT16)bl_sreg_read(SREG_DBC4PS_CON_TH);          /* SREG_DBC4PS_LUMAX_TH */

    if( is_pc_hdmi_pc ){
      s_r_dbc4ps_params.dbc4ps_lumax_th  = (UINT8)bl_sreg_read(SREG_DBC4PS_LUMAX_TH_PC);      /* SREG_DBC4PS_LUMAX_TH_PC */
    } else {
      s_r_dbc4ps_params.dbc4ps_lumax_th  = (UINT8)bl_sreg_read(SREG_DBC4PS_LUMAX_TH);         /* SREG_DBC4PS_LUMAX_TH */      
    }

}

static void calc_dbc4ps_calculate_dbc4ps_bl_cont( ST_PARAM_DBC_MAIN *params )
{
    BOOL    dbc4psEnable;
    UINT8   luma_max = params->luma_max;
    UINT16* chistbin = params->cHist8;
    UINT32  ttlchistbin = 0;
    UINT32  chist;
    UINT8   i;
    
    vAutoBacklightGetDbc4psEnable(&dbc4psEnable);
    
    for( i = 0; i < C_HISTGRAM_POINT_NUM ; i++ ){
      ttlchistbin += chistbin[i];
    }
        
    if( ttlchistbin != 0 )
    {
      chist = ( chistbin[0] * 100 ) / ttlchistbin;
    }else{
      chist = 0;
    }        

    s_w_dbc4ps_sreg.dbc4ps_chist = chist;
    s_w_dbc4ps_sreg.dbc4ps_luma_max = luma_max;   
     
    if( ( dbc4psEnable ) && ( s_r_dbc4ps_params.dbc4ps_pic_en ) ){
    
      if( luma_max == ( s_r_dbc4ps_params.dbc4ps_lumax_th + 1 ) ){
        luma_max--;
      }
        
      if( ( chist >= CHIST_PERCENT ) &&
          ( luma_max <= s_r_dbc4ps_params.dbc4ps_lumax_th ) ){
          
          if( s_w_dbc4ps_sreg.dbc4ps_pic_state == 0 ){
            s_w_black_count++;
          }
          
          if( s_w_black_count > s_r_dbc4ps_params.dbc4ps_con_th ){
            s_w_black_count = 0;
            
            if( s_w_dbc4ps_sreg.dbc4ps_pic_state == 0 ){
              s_w_loop_count = 0;
            }
            s_w_dbc4ps_sreg.dbc4ps_pic_state = 1;
            s_w_dbc4ps_int =  s_r_dbc4ps_params.dbc4ps_pic_int_1;
          }
      }else{
          s_w_black_count = 0;
          if( s_w_dbc4ps_sreg.dbc4ps_pic_state == 1 ){
              s_w_loop_count = 0;
          }
          s_w_dbc4ps_sreg.dbc4ps_pic_state = 0;
          s_w_dbc4ps_int =  s_r_dbc4ps_params.dbc4ps_pic_int_2;
      }
      
      
      if( ( --s_w_loop_count ) <= 0 ){
        if( s_w_dbc4ps_sreg.dbc4ps_pic_state == 1 ){
          /* ALL BLACK */                          
          if( s_r_dbc4ps_params.bl_gain > 0 ){
            s_w_dbc4ps += (INT16)s_r_dbc4ps_params.dbc4ps_bl_b_inc;
          }   
                   
        }else{ 
          /* NOT ALL BLACK */          
          if( s_w_dbc4ps > 0 ){
            s_w_dbc4ps -= (INT16)s_r_dbc4ps_params.dbc4ps_bl_b_dec;
          }                
        }

        s_w_dbc4ps = BOUND( s_w_dbc4ps, 0, 255 );
        s_w_dbc4ps_sreg.dbc4ps_bl_cont = (UINT8)s_w_dbc4ps;
      }
    
      if( s_w_loop_count <= 0 ){
        s_w_loop_count = s_w_dbc4ps_int;
      }
      
      s_w_dbc4ps_sreg.dbc4ps_counter = s_w_black_count;
    }else{
      s_w_dbc4ps = 0; 
      s_w_loop_count = 0;  
      s_w_black_count = 0;
      s_w_dbc4ps_sreg.dbc4ps_counter = 0;
      
      s_w_dbc4ps_sreg.dbc4ps_pic_state = 0;
      s_w_dbc4ps_sreg.dbc4ps_bl_cont = (UINT8)s_w_dbc4ps;
      s_w_dbc4ps_sreg.dbc4ps_chist = chist;
      s_w_dbc4ps_sreg.dbc4ps_luma_max = luma_max;
    }        
}

static void calc_dbc4ps_main_update_status( )
{
    bl_sreg_write(SREG_DBC4PS_PIC_STATE, (UINT16)s_w_dbc4ps_sreg.dbc4ps_pic_state);
    bl_sreg_write(SREG_CHIST, (UINT16)s_w_dbc4ps_sreg.dbc4ps_chist);
    bl_sreg_write(SREG_DBC4PS_BL_CONT, (UINT16)s_w_dbc4ps_sreg.dbc4ps_bl_cont);
    bl_sreg_write(SREG_LUMA_MAX, (UINT16)s_w_dbc4ps_sreg.dbc4ps_luma_max);
    bl_sreg_write(SREG_DBC4PS_COUNTER, (UINT16)s_w_dbc4ps_sreg.dbc4ps_counter);
}

//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------

void calc_dbc_init( void )
{
    s_w_remaining_iir_clr_times = 1;/* 1st iir is disabled. */
    s_w_dbc_bl_down  = 0;
    s_w_dbc          = 0;
    s_w_apl_dbc      = 0;
    s_w_max_dbc      = 0;
    s_w_dbc1         = 0;
    s_w_apldbc_iir   = 0;
    
    s_w_dbc4ps = 0;
    s_w_dbc4ps_int = 0;
    s_w_loop_count = 0;
    s_w_black_count = 0;
}

INT16 calc_dbc_main( void )
{
    ST_PARAM_DBC_MAIN    dbc_params;
    
    x_memset(&dbc_params, 0, sizeof(ST_PARAM_DBC_MAIN));

    calc_dbc_main_init_params( &dbc_params );

    calc_dbc_calculate_dbc_bl_down( &dbc_params );
	
    calc_dbc_main_update_status( &dbc_params );
	
    return s_w_dbc;
}

INT16 calc_dbc4ps_main( void )
{
    ST_PARAM_DBC_MAIN    dbc_params;
    
    x_memset(&dbc_params, 0, sizeof(ST_PARAM_DBC_MAIN));

    calc_dbc4ps_main_init_params( &dbc_params );

    calc_dbc4ps_calculate_dbc4ps_bl_cont( &dbc_params );
	
    calc_dbc4ps_main_update_status( );
	
    return s_w_dbc4ps;
}

void vAdvContrastEnhancerStatus(UINT8 ui1_value)
{
    switch( ui1_value ){
      case BL_ACE_OFF:
        bl_sreg_write(SREG_DBC_APL_EN, (UINT16)FALSE);
        bl_sreg_write(SREG_DBC_MAX_EN, (UINT16)FALSE);
        bl_sreg_write(SREG_DBC4PS_PIC_EN, (UINT16)FALSE);
        break;
      case BL_ACE_LOW:
        bl_sreg_write(SREG_DBC_APL_EN, (UINT16)TRUE);
        bl_sreg_write(SREG_DBC_MAX_EN, (UINT16)TRUE);
        bl_sreg_write(SREG_DBC4PS_PIC_EN, (UINT16)TRUE);
        break;
      case BL_ACE_MID:
        bl_sreg_write(SREG_DBC_APL_EN, (UINT16)TRUE);
        bl_sreg_write(SREG_DBC_MAX_EN, (UINT16)TRUE);
        bl_sreg_write(SREG_DBC4PS_PIC_EN, (UINT16)TRUE);
        break;    
      case BL_ACE_HIGH:
        bl_sreg_write(SREG_DBC_APL_EN, (UINT16)TRUE);
        bl_sreg_write(SREG_DBC_MAX_EN, (UINT16)TRUE);
        bl_sreg_write(SREG_DBC4PS_PIC_EN, (UINT16)TRUE);
        break;  
      default:
        break;
    }  
}

