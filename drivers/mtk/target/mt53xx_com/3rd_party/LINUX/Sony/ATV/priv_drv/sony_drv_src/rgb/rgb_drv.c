
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "ir_if.h"
#include "sif_if.h"
#include "eeprom_if.h"
 
#include "x_dbg.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"

#include "../../sony_priv_inc/utility.h"
#include "../../sony_drv_inc/rgb_if.h"
#include "../../sony_priv_inc/rgb_reg.h"
#include "../../sony_priv_inc/bl_reg.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//#define RGB_ENABLE_LOG

#if defined (RGB_ENABLE_LOG)
#define RGB_LOG(fmt...)         printf(fmt)
#else
#define RGB_LOG(fmt...)            
#endif

#define RGB_THREAD_STACK_SIZE   2048
#define RGB_THREAD_PRIORITY     100
#define RGB_CLK_DIV             0x100

#define RGB_S11059_ADDR         0x54 
#define RGB_CTRL_REG            0x00
#define RGB_SENS_REG            0x03

#define RGB_OK                  0
#define RGB_NG                  1
#define RGB_ERROR_COUNT         50


#define SREG_RGB_SENS_DUMMY     0xFFFF
#define RGB_EEP_SIZE           (EEP_OFFSET_EMCS_RGB_AREA_END - EEP_OFFSET_EMCS_RGB_AREA_START)
#define RGB_EEP_CHKSUM          0x38
#define RGB_TOTAL_MAP_ELEM      36
#define RGB_TOTAL_MAP_SIZE      54
#define EEP_MAP_INVALID_IDX     0xFFFF


#define RGB_MATRIX_COL          4
#define RGB_MATRIX_ROW          3

#define CONTROL_BYTE            3
#define STATUS_BYTE             8

#define IIR_ORDER               8
#define OUTPUT_CHANNEL_NUM      3
#define SENSOR_CHANNEL_NUM      4
#define STANDBY_CYCLE_TIME      8
#define RGB_IR_CONT_TIME        300
#define RGB_IR_RESUME_TIME      1000

#define RGB_IR_THRESHOLD        30

#define X_POINT_PRECISION       1000000

#define RGB_COLOR_VAR_NUM       8
#define RGB_COLOR_VAR_ELEMENT   17
#define RGB_COLOR_VAR_SIZE      30
#define T_INT_CNT				4

#define INC_X(x)          ( (x) * X_POINT_PRECISION )
#define DEC_X(x)          ( (x) / X_POINT_PRECISION )
#define BOUND_MIN(x,min)	( ((x) < (min)) ? (min) : (x) )
#define BOUND_MAX(x,max)	( ((x) > (max)) ? (max) : (x) )
#define BOUND(x,min,max)	( ((x) > (max)) ? (max) : ( ((x) < (min)) ? (min) : (x) ) )

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum {
    MAP_RGB,
    MAP_OPT
} MAP_T;

typedef struct _RGB_DRV_EEP{
	const UINT16 offset;
	const UINT16 sreg;
	const UINT8  byte_size;
	const UINT16 init;
} RGB_DRV_EEP;

typedef enum {
    RGB_WBT_COOL,
    RGB_WBT_NEUTRAL,
    RGB_WBT_WARM1,
    RGB_WBT_WARM2,
    RGB_WBT_PHOTO,
    RGB_WBT_MAX /* Invalid index */
} RGB_WBT_IDX;

typedef struct _RGB_WBT_STEP_FUNC_TH
{
	INT32 th1;
	INT32 th2;
	INT32 th3;
}RGB_WBT_STEP_FUNC_TH;
 
typedef struct _RGB_WBT_STEP_FUNC_OFST
{
	INT32  l;
	INT32  m;
	INT32  h;
}RGB_WBT_STEP_FUNC_OFST;

typedef struct _RGB_DRV_MEMBER_T
{
    HANDLE_T    h_rgbDrvThread;
    HANDLE_T    h_rgbDetTimer;
    HANDLE_T    h_rgbThreadSema;
    HANDLE_T    h_rgbIrContTimer;
    HANDLE_T    h_rgbIr1secTimer;
    HANDLE_T    h_rgbSregMutex;
    
    UINT8       det_on;
    UINT8       ir_timer_on; 
    UINT8       ir_continuous; 

//INT64 is not fully supportted
//    INT64       x_k; /* x_k = k*X_POINT_PRECISION */   
    INT32       x_k; /* x_k = k*X_POINT_PRECISION */     


    UINT16      prev_sensor_r;
    UINT16      prev_sensor_g;
    UINT16      prev_sensor_b;
    UINT16      prev_sensor_ir;
    UINT16      curr_sensor_r;
    UINT16      curr_sensor_g;
    UINT16      curr_sensor_b;
    UINT16      curr_sensor_ir; 
    UINT16      sum_r;
    UINT16      sum_g;
    UINT16      sum_b;
    UINT16      sum_ir;
    UINT16      unfiltered_y;
    UINT16      unfiltered_t;
    UINT16      y_history[9]; 
    UINT16      t_history[9];  
    UINT16      gain_r_history[9]; 
    UINT16      gain_g_history[9];
    UINT16      gain_b_history[9];
    MAP_T       curr_map;
    
    RGB_WBT_IDX s_ui_curr_wbt_idx;
    RGB_WBT_IDX s_ui_prev_wbt_idx;
    
    UINT16      opt_read_data;
    UINT16      SREG[MAX_RGB_SREG];
}RGB_DRV_MEMBER_T;

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

const UINT32 t_int[T_INT_CNT] = {175, 2800, 44800, 358400};

const UINT16 tbl_wbt_idx_max[RGB_TOTAL_MAP_ELEM] = {
    0x1770, 0x1388, 0x0BB8, 0x1770, 0x1388, 0x0BB8, 0x1770, 0x1388, 0x0BB8,
    0x007F, 0x0080, 0x0084, 0x0080, 0x0080, 0x0080, 0x0082, 0x0080, 0x007C,
    0x00A0, 0x0050, 0x0000, 0x00A0, 0x0050, 0x0000, 0x00A0, 0x0050, 0x0000,
    0x0084, 0x0085, 0x0086, 0x0080, 0x0080, 0x0080, 0x007C, 0x0078, 0x0075
};

const UINT16 tbl_color_default[RGB_COLOR_VAR_ELEMENT] = {
    0x00FA, 0x8050, 0x802D, 0x8005, 0x7F60, 0x8019, 0x8069, 0x7FEC, 0x7F92,
    0x7FFB, 0x7FE7, 0x80CD, 0x7F4C, 0x0028, 0x00B2, 0x0005, 0x0064
};

const RGB_DRV_EEP tbl_rgb_eep_main[] = {
/*      offset                              sreg                    byte_size init    */  
    {  EEP_OFFSET_EMCS_RGB_GAISOU_ECS,      SREG_RGB_SENS_DUMMY,        1, 0x0000},       
    {  EEP_OFFSET_EMCS_RGB_CHECKSUM,        SREG_RGB_SENS_DUMMY,        1, RGB_EEP_CHKSUM},
    {  EEP_OFFSET_EMCS_RGB_INT_R,           SREG_RGB_SENS_INT_R,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_INT_G,           SREG_RGB_SENS_INT_G,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_INT_B,           SREG_RGB_SENS_INT_B,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_INT_IR,          SREG_RGB_SENS_INT_IR,       2, 0x0000 },    
    {  EEP_OFFSET_EMCS_RGB_COEF_GAIN_R,     SREG_RGB_SENS_COEF_GAIN_R,  1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_COEF_OFST_R,     SREG_RGB_SENS_COEF_OFST_R,  2, 0x0400 },
    {  EEP_OFFSET_EMCS_RGB_COEF_GAIN_G,     SREG_RGB_SENS_COEF_GAIN_G,  1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_COEF_OFST_G,     SREG_RGB_SENS_COEF_OFST_G,  2, 0x0400 },
    {  EEP_OFFSET_EMCS_RGB_COEF_GAIN_B,     SREG_RGB_SENS_COEF_GAIN_B,  1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_COEF_OFST_B,     SREG_RGB_SENS_COEF_OFST_B,  2, 0x0400 },
    {  EEP_OFFSET_EMCS_RGB_COEF_GAIN_IR,    SREG_RGB_SENS_COEF_GAIN_IR, 1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_COEF_OFST_IR,    SREG_RGB_SENS_COEF_OFST_IR, 2, 0x0400 },
    {  EEP_OFFSET_EMCS_RGB_GAIN_SET,        SREG_RGB_SENS_GAIN_SET,     1, 0x0001 },
    {  EEP_OFFSET_EMCS_RGB_INT_TIME,        SREG_RGB_SENS_INT_TIME,     1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_OPT_THRE_H,      SREG_RGB_SENS_OPT_THRE_H,   2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_OPT_THRE_L,      SREG_RGB_SENS_OPT_THRE_L,   2, 0x012C },
    {  EEP_OFFSET_EMCS_RGB_ON,              SREG_RGB_SENS_ON,           1, 0x0001 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A1,          SREG_RGB_SENS_IIR_A1,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A2,          SREG_RGB_SENS_IIR_A2,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A3,          SREG_RGB_SENS_IIR_A3,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A4,          SREG_RGB_SENS_IIR_A4,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A5,          SREG_RGB_SENS_IIR_A5,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A6,          SREG_RGB_SENS_IIR_A6,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A7,          SREG_RGB_SENS_IIR_A7,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A8,          SREG_RGB_SENS_IIR_A8,       1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_B0,          SREG_RGB_SENS_IIR_B0,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_IIR_A11,         SREG_RGB_SENS_IIR_A11,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A12,         SREG_RGB_SENS_IIR_A12,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A13,         SREG_RGB_SENS_IIR_A13,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A14,         SREG_RGB_SENS_IIR_A14,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A15,         SREG_RGB_SENS_IIR_A15,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A16,         SREG_RGB_SENS_IIR_A16,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A17,         SREG_RGB_SENS_IIR_A17,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_A18,         SREG_RGB_SENS_IIR_A18,      1, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_IIR_B1,          SREG_RGB_SENS_IIR_B1,       1, 0x00FF },
};

const RGB_DRV_EEP tbl_rgb_eep_wbt[] = {
    {  EEP_OFFSET_EMCS_RGB_TR1_R_COOL,      SREG_RGB_SENS_TR1_R,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_R_COOL,      SREG_RGB_SENS_TR2_R,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_R_COOL,      SREG_RGB_SENS_TR3_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_G_COOL,      SREG_RGB_SENS_TR1_G,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_G_COOL,      SREG_RGB_SENS_TR2_G,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_G_COOL,      SREG_RGB_SENS_TR3_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_B_COOL,      SREG_RGB_SENS_TR1_B,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_B_COOL,      SREG_RGB_SENS_TR2_B,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_B_COOL,      SREG_RGB_SENS_TR3_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_RGB1_R_COOL,     SREG_RGB_SENS_RGB1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_R_COOL,     SREG_RGB_SENS_RGB2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_R_COOL,     SREG_RGB_SENS_RGB3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_G_COOL,     SREG_RGB_SENS_RGB1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_G_COOL,     SREG_RGB_SENS_RGB2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_G_COOL,     SREG_RGB_SENS_RGB3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_B_COOL,     SREG_RGB_SENS_RGB1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_B_COOL,     SREG_RGB_SENS_RGB2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_B_COOL,     SREG_RGB_SENS_RGB3_B,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_TO1_R_COOL,      SREG_RGB_SENS_TO1_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_R_COOL,      SREG_RGB_SENS_TO2_R,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_R_COOL,      SREG_RGB_SENS_TO3_R,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_G_COOL,      SREG_RGB_SENS_TO1_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_G_COOL,      SREG_RGB_SENS_TO2_G,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_G_COOL,      SREG_RGB_SENS_TO3_G,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_B_COOL,      SREG_RGB_SENS_TO1_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_B_COOL,      SREG_RGB_SENS_TO2_B,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_B_COOL,      SREG_RGB_SENS_TO3_B,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_OPT1_R_COOL,     SREG_RGB_SENS_OPT1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_R_COOL,     SREG_RGB_SENS_OPT2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_R_COOL,     SREG_RGB_SENS_OPT3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_G_COOL,     SREG_RGB_SENS_OPT1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_G_COOL,     SREG_RGB_SENS_OPT2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_G_COOL,     SREG_RGB_SENS_OPT3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_B_COOL,     SREG_RGB_SENS_OPT1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_B_COOL,     SREG_RGB_SENS_OPT2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_B_COOL,     SREG_RGB_SENS_OPT3_B,       1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_TR1_R_NEUTRAL,   SREG_RGB_SENS_TR1_R,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_R_NEUTRAL,   SREG_RGB_SENS_TR2_R,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_R_NEUTRAL,   SREG_RGB_SENS_TR3_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_G_NEUTRAL,   SREG_RGB_SENS_TR1_G,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_G_NEUTRAL,   SREG_RGB_SENS_TR2_G,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_G_NEUTRAL,   SREG_RGB_SENS_TR3_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_B_NEUTRAL,   SREG_RGB_SENS_TR1_B,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_B_NEUTRAL,   SREG_RGB_SENS_TR2_B,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_B_NEUTRAL,   SREG_RGB_SENS_TR3_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_RGB1_R_NEUTRAL,  SREG_RGB_SENS_RGB1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_R_NEUTRAL,  SREG_RGB_SENS_RGB2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_R_NEUTRAL,  SREG_RGB_SENS_RGB3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_G_NEUTRAL,  SREG_RGB_SENS_RGB1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_G_NEUTRAL,  SREG_RGB_SENS_RGB2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_G_NEUTRAL,  SREG_RGB_SENS_RGB3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_B_NEUTRAL,  SREG_RGB_SENS_RGB1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_B_NEUTRAL,  SREG_RGB_SENS_RGB2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_B_NEUTRAL,  SREG_RGB_SENS_RGB3_B,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_TO1_R_NEUTRAL,   SREG_RGB_SENS_TO1_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_R_NEUTRAL,   SREG_RGB_SENS_TO2_R,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_R_NEUTRAL,   SREG_RGB_SENS_TO3_R,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_G_NEUTRAL,   SREG_RGB_SENS_TO1_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_G_NEUTRAL,   SREG_RGB_SENS_TO2_G,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_G_NEUTRAL,   SREG_RGB_SENS_TO3_G,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_B_NEUTRAL,   SREG_RGB_SENS_TO1_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_B_NEUTRAL,   SREG_RGB_SENS_TO2_B,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_B_NEUTRAL,   SREG_RGB_SENS_TO3_B,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_OPT1_R_NEUTRAL,  SREG_RGB_SENS_OPT1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_R_NEUTRAL,  SREG_RGB_SENS_OPT2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_R_NEUTRAL,  SREG_RGB_SENS_OPT3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_G_NEUTRAL,  SREG_RGB_SENS_OPT1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_G_NEUTRAL,  SREG_RGB_SENS_OPT2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_G_NEUTRAL,  SREG_RGB_SENS_OPT3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_B_NEUTRAL,  SREG_RGB_SENS_OPT1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_B_NEUTRAL,  SREG_RGB_SENS_OPT2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_B_NEUTRAL,  SREG_RGB_SENS_OPT3_B,       1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_TR1_R_WARM1,     SREG_RGB_SENS_TR1_R,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_R_WARM1,     SREG_RGB_SENS_TR2_R,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_R_WARM1,     SREG_RGB_SENS_TR3_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_G_WARM1,     SREG_RGB_SENS_TR1_G,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_G_WARM1,     SREG_RGB_SENS_TR2_G,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_G_WARM1,     SREG_RGB_SENS_TR3_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_B_WARM1,     SREG_RGB_SENS_TR1_B,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_B_WARM1,     SREG_RGB_SENS_TR2_B,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_B_WARM1,     SREG_RGB_SENS_TR3_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_RGB1_R_WARM1,    SREG_RGB_SENS_RGB1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_R_WARM1,    SREG_RGB_SENS_RGB2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_R_WARM1,    SREG_RGB_SENS_RGB3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_G_WARM1,    SREG_RGB_SENS_RGB1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_G_WARM1,    SREG_RGB_SENS_RGB2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_G_WARM1,    SREG_RGB_SENS_RGB3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_B_WARM1,    SREG_RGB_SENS_RGB1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_B_WARM1,    SREG_RGB_SENS_RGB2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_B_WARM1,    SREG_RGB_SENS_RGB3_B,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_TO1_R_WARM1,     SREG_RGB_SENS_TO1_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_R_WARM1,     SREG_RGB_SENS_TO2_R,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_R_WARM1,     SREG_RGB_SENS_TO3_R,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_G_WARM1,     SREG_RGB_SENS_TO1_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_G_WARM1,     SREG_RGB_SENS_TO2_G,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_G_WARM1,     SREG_RGB_SENS_TO3_G,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_B_WARM1,     SREG_RGB_SENS_TO1_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_B_WARM1,     SREG_RGB_SENS_TO2_B,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_B_WARM1,     SREG_RGB_SENS_TO3_B,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_OPT1_R_WARM1,    SREG_RGB_SENS_OPT1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_R_WARM1,    SREG_RGB_SENS_OPT2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_R_WARM1,    SREG_RGB_SENS_OPT3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_G_WARM1,    SREG_RGB_SENS_OPT1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_G_WARM1,    SREG_RGB_SENS_OPT2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_G_WARM1,    SREG_RGB_SENS_OPT3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_B_WARM1,    SREG_RGB_SENS_OPT1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_B_WARM1,    SREG_RGB_SENS_OPT2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_B_WARM1,    SREG_RGB_SENS_OPT3_B,       1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_TR1_R_WARM2,     SREG_RGB_SENS_TR1_R,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_R_WARM2,     SREG_RGB_SENS_TR2_R,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_R_WARM2,     SREG_RGB_SENS_TR3_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_G_WARM2,     SREG_RGB_SENS_TR1_G,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_G_WARM2,     SREG_RGB_SENS_TR2_G,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_G_WARM2,     SREG_RGB_SENS_TR3_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_B_WARM2,     SREG_RGB_SENS_TR1_B,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_B_WARM2,     SREG_RGB_SENS_TR2_B,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_B_WARM2,     SREG_RGB_SENS_TR3_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_RGB1_R_WARM2,    SREG_RGB_SENS_RGB1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_R_WARM2,    SREG_RGB_SENS_RGB2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_R_WARM2,    SREG_RGB_SENS_RGB3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_G_WARM2,    SREG_RGB_SENS_RGB1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_G_WARM2,    SREG_RGB_SENS_RGB2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_G_WARM2,    SREG_RGB_SENS_RGB3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_B_WARM2,    SREG_RGB_SENS_RGB1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_B_WARM2,    SREG_RGB_SENS_RGB2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_B_WARM2,    SREG_RGB_SENS_RGB3_B,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_TO1_R_WARM2,     SREG_RGB_SENS_TO1_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_R_WARM2,     SREG_RGB_SENS_TO2_R,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_R_WARM2,     SREG_RGB_SENS_TO3_R,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_G_WARM2,     SREG_RGB_SENS_TO1_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_G_WARM2,     SREG_RGB_SENS_TO2_G,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_G_WARM2,     SREG_RGB_SENS_TO3_G,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_B_WARM2,     SREG_RGB_SENS_TO1_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_B_WARM2,     SREG_RGB_SENS_TO2_B,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_B_WARM2,     SREG_RGB_SENS_TO3_B,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_OPT1_R_WARM2,    SREG_RGB_SENS_OPT1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_R_WARM2,    SREG_RGB_SENS_OPT2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_R_WARM2,    SREG_RGB_SENS_OPT3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_G_WARM2,    SREG_RGB_SENS_OPT1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_G_WARM2,    SREG_RGB_SENS_OPT2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_G_WARM2,    SREG_RGB_SENS_OPT3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_B_WARM2,    SREG_RGB_SENS_OPT1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_B_WARM2,    SREG_RGB_SENS_OPT2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_B_WARM2,    SREG_RGB_SENS_OPT3_B,       1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_TR1_R_PHOTO,     SREG_RGB_SENS_TR1_R,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_R_PHOTO,     SREG_RGB_SENS_TR2_R,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_R_PHOTO,     SREG_RGB_SENS_TR3_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_G_PHOTO,     SREG_RGB_SENS_TR1_G,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_G_PHOTO,     SREG_RGB_SENS_TR2_G,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_G_PHOTO,     SREG_RGB_SENS_TR3_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TR1_B_PHOTO,     SREG_RGB_SENS_TR1_B,        2, 0x0BB8 },
    {  EEP_OFFSET_EMCS_RGB_TR2_B_PHOTO,     SREG_RGB_SENS_TR2_B,        2, 0x07D0 },
    {  EEP_OFFSET_EMCS_RGB_TR3_B_PHOTO,     SREG_RGB_SENS_TR3_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_RGB1_R_PHOTO,    SREG_RGB_SENS_RGB1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_R_PHOTO,    SREG_RGB_SENS_RGB2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_R_PHOTO,    SREG_RGB_SENS_RGB3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_G_PHOTO,    SREG_RGB_SENS_RGB1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_G_PHOTO,    SREG_RGB_SENS_RGB2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_G_PHOTO,    SREG_RGB_SENS_RGB3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_RGB1_B_PHOTO,    SREG_RGB_SENS_RGB1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_RGB2_B_PHOTO,    SREG_RGB_SENS_RGB2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_RGB3_B_PHOTO,    SREG_RGB_SENS_RGB3_B,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_TO1_R_PHOTO,     SREG_RGB_SENS_TO1_R,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_R_PHOTO,     SREG_RGB_SENS_TO2_R,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_R_PHOTO,     SREG_RGB_SENS_TO3_R,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_G_PHOTO,     SREG_RGB_SENS_TO1_G,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_G_PHOTO,     SREG_RGB_SENS_TO2_G,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_G_PHOTO,     SREG_RGB_SENS_TO3_G,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_TO1_B_PHOTO,     SREG_RGB_SENS_TO1_B,        2, 0x01F4 },
    {  EEP_OFFSET_EMCS_RGB_TO2_B_PHOTO,     SREG_RGB_SENS_TO2_B,        2, 0x0064 },
    {  EEP_OFFSET_EMCS_RGB_TO3_B_PHOTO,     SREG_RGB_SENS_TO3_B,        2, 0x0000 },
    {  EEP_OFFSET_EMCS_RGB_OPT1_R_PHOTO,    SREG_RGB_SENS_OPT1_R,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_R_PHOTO,    SREG_RGB_SENS_OPT2_R,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_R_PHOTO,    SREG_RGB_SENS_OPT3_R,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_G_PHOTO,    SREG_RGB_SENS_OPT1_G,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_G_PHOTO,    SREG_RGB_SENS_OPT2_G,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_G_PHOTO,    SREG_RGB_SENS_OPT3_G,       1, 0x00FF },
    {  EEP_OFFSET_EMCS_RGB_OPT1_B_PHOTO,    SREG_RGB_SENS_OPT1_B,       1, 0x0080 },
    {  EEP_OFFSET_EMCS_RGB_OPT2_B_PHOTO,    SREG_RGB_SENS_OPT2_B,       1, 0x00C8 },
    {  EEP_OFFSET_EMCS_RGB_OPT3_B_PHOTO,    SREG_RGB_SENS_OPT3_B,       1, 0x00FF }
};

const RGB_DRV_EEP tbl_rgb_eep_color[] = {

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE8,       SREG_RGB_SENS_MANU_TIME,    2, 0x00D2 },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE8,           SREG_RGB_SENS_MTX_11,       2, 0x802D },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE8,           SREG_RGB_SENS_MTX_12,       2, 0x7FFE },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE8,           SREG_RGB_SENS_MTX_13,       2, 0x7FD5 },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE8,           SREG_RGB_SENS_MTX_14,       2, 0x803A },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE8,           SREG_RGB_SENS_MTX_21,       2, 0x7FE2 },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE8,           SREG_RGB_SENS_MTX_22,       2, 0x805E },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE8,           SREG_RGB_SENS_MTX_23,       2, 0x7F9B },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE8,           SREG_RGB_SENS_MTX_24,       2, 0x8091 },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE8,           SREG_RGB_SENS_MTX_31,       2, 0x808D },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE8,           SREG_RGB_SENS_MTX_32,       2, 0x7F3B },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE8,           SREG_RGB_SENS_MTX_33,       2, 0x80EF },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE8,           SREG_RGB_SENS_MTX_34,       2, 0x7EC0 },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE8,         SREG_RGB_SENS_GAIN_BL,      1, 0x0041 },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE8,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE8,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE8,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },
    
    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE7,       SREG_RGB_SENS_MANU_TIME,    2, 0x01DB },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE7,           SREG_RGB_SENS_MTX_11,       2, 0x8022 },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE7,           SREG_RGB_SENS_MTX_12,       2, 0x80DA },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE7,           SREG_RGB_SENS_MTX_13,       2, 0x7FA8 },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE7,           SREG_RGB_SENS_MTX_14,       2, 0x7F91 },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE7,           SREG_RGB_SENS_MTX_21,       2, 0x7FAC },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE7,           SREG_RGB_SENS_MTX_22,       2, 0x8101 },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE7,           SREG_RGB_SENS_MTX_23,       2, 0x7FBF },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE7,           SREG_RGB_SENS_MTX_24,       2, 0x7FFC },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE7,           SREG_RGB_SENS_MTX_31,       2, 0x804F },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE7,           SREG_RGB_SENS_MTX_32,       2, 0x7F31 },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE7,           SREG_RGB_SENS_MTX_33,       2, 0x8139 },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE7,           SREG_RGB_SENS_MTX_34,       2, 0x7EFE },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE7,         SREG_RGB_SENS_GAIN_BL,      1, 0x007D },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE7,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE7,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE7,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE6,       SREG_RGB_SENS_MANU_TIME,    2, 0x00D2 },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE6,           SREG_RGB_SENS_MTX_11,       2, 0x7FEF },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE6,           SREG_RGB_SENS_MTX_12,       2, 0x8060 },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE6,           SREG_RGB_SENS_MTX_13,       2, 0x7FC8 },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE6,           SREG_RGB_SENS_MTX_14,       2, 0x7FBC },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE6,           SREG_RGB_SENS_MTX_21,       2, 0x7FBB },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE6,           SREG_RGB_SENS_MTX_22,       2, 0x809A },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE6,           SREG_RGB_SENS_MTX_23,       2, 0x7FB4 },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE6,           SREG_RGB_SENS_MTX_24,       2, 0x7FEC },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE6,           SREG_RGB_SENS_MTX_31,       2, 0x8050 },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE6,           SREG_RGB_SENS_MTX_32,       2, 0x7F7A },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE6,           SREG_RGB_SENS_MTX_33,       2, 0x8104 },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE6,           SREG_RGB_SENS_MTX_34,       2, 0x7EE6 },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE6,         SREG_RGB_SENS_GAIN_BL,      1, 0x005A },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE6,         SREG_RGB_SENS_OFST_BL,      1, 0x00AC },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE6,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE6,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE5,       SREG_RGB_SENS_MANU_TIME,    2, 0x00BD },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE5,           SREG_RGB_SENS_MTX_11,       2, 0x8031 },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE5,           SREG_RGB_SENS_MTX_12,       2, 0x803B },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE5,           SREG_RGB_SENS_MTX_13,       2, 0x7F8E },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE5,           SREG_RGB_SENS_MTX_14,       2, 0x804D },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE5,           SREG_RGB_SENS_MTX_21,       2, 0x8019 },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE5,           SREG_RGB_SENS_MTX_22,       2, 0x8035 },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE5,           SREG_RGB_SENS_MTX_23,       2, 0x8014 },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE5,           SREG_RGB_SENS_MTX_24,       2, 0x801D },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE5,           SREG_RGB_SENS_MTX_31,       2, 0x805C },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE5,           SREG_RGB_SENS_MTX_32,       2, 0x7F64 },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE5,           SREG_RGB_SENS_MTX_33,       2, 0x80C2 },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE5,           SREG_RGB_SENS_MTX_34,       2, 0x7ECA },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE5,         SREG_RGB_SENS_GAIN_BL,      1, 0x0082 },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE5,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE5,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE5,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE4,       SREG_RGB_SENS_MANU_TIME,    2, 0x00D2 },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE4,           SREG_RGB_SENS_MTX_11,       2, 0x802D },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE4,           SREG_RGB_SENS_MTX_12,       2, 0x7FFE },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE4,           SREG_RGB_SENS_MTX_13,       2, 0x7FD5 },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE4,           SREG_RGB_SENS_MTX_14,       2, 0x803A },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE4,           SREG_RGB_SENS_MTX_21,       2, 0x7FE2 },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE4,           SREG_RGB_SENS_MTX_22,       2, 0x805E },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE4,           SREG_RGB_SENS_MTX_23,       2, 0x7F9B },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE4,           SREG_RGB_SENS_MTX_24,       2, 0x8091 },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE4,           SREG_RGB_SENS_MTX_31,       2, 0x808D },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE4,           SREG_RGB_SENS_MTX_32,       2, 0x7F3B },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE4,           SREG_RGB_SENS_MTX_33,       2, 0x80EF },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE4,           SREG_RGB_SENS_MTX_34,       2, 0x7EC0 },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE4,         SREG_RGB_SENS_GAIN_BL,      1, 0x0041 },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE4,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE4,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE4,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE3,       SREG_RGB_SENS_MANU_TIME,    2, 0x01DB },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE3,           SREG_RGB_SENS_MTX_11,       2, 0x8022 },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE3,           SREG_RGB_SENS_MTX_12,       2, 0x80DA },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE3,           SREG_RGB_SENS_MTX_13,       2, 0x7FA8 },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE3,           SREG_RGB_SENS_MTX_14,       2, 0x7F91 },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE3,           SREG_RGB_SENS_MTX_21,       2, 0x7FAC },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE3,           SREG_RGB_SENS_MTX_22,       2, 0x8101 },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE3,           SREG_RGB_SENS_MTX_23,       2, 0x7FBF },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE3,           SREG_RGB_SENS_MTX_24,       2, 0x7FFC },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE3,           SREG_RGB_SENS_MTX_31,       2, 0x804F },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE3,           SREG_RGB_SENS_MTX_32,       2, 0x7F31 },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE3,           SREG_RGB_SENS_MTX_33,       2, 0x8139 },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE3,           SREG_RGB_SENS_MTX_34,       2, 0x7EFE },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE3,         SREG_RGB_SENS_GAIN_BL,      1, 0x007D },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE3,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE3,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE3,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE2,       SREG_RGB_SENS_MANU_TIME,    2, 0x00BD },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE2,           SREG_RGB_SENS_MTX_11,       2, 0x8053 },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE2,           SREG_RGB_SENS_MTX_12,       2, 0x7FD8 },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE2,           SREG_RGB_SENS_MTX_13,       2, 0x8018 },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE2,           SREG_RGB_SENS_MTX_14,       2, 0x7FB0 },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE2,           SREG_RGB_SENS_MTX_21,       2, 0x809A },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE2,           SREG_RGB_SENS_MTX_22,       2, 0x7F6B },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE2,           SREG_RGB_SENS_MTX_23,       2, 0x80E9 },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE2,           SREG_RGB_SENS_MTX_24,       2, 0x7EF2 },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE2,           SREG_RGB_SENS_MTX_31,       2, 0x7FDB },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE2,           SREG_RGB_SENS_MTX_32,       2, 0x8003 },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE2,           SREG_RGB_SENS_MTX_33,       2, 0x800B },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE2,           SREG_RGB_SENS_MTX_34,       2, 0x8004 },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE2,         SREG_RGB_SENS_GAIN_BL,      1, 0x0096 },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE2,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE2,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE2,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },

    {  EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE1,       SREG_RGB_SENS_MANU_TIME,    2, 0x01DB },
    {  EEP_OFFSET_EMCS_RGB_MTX11_CASE1,           SREG_RGB_SENS_MTX_11,       2, 0x8031 },
    {  EEP_OFFSET_EMCS_RGB_MTX12_CASE1,           SREG_RGB_SENS_MTX_12,       2, 0x803B },
    {  EEP_OFFSET_EMCS_RGB_MTX13_CASE1,           SREG_RGB_SENS_MTX_13,       2, 0x7F8E },
    {  EEP_OFFSET_EMCS_RGB_MTX14_CASE1,           SREG_RGB_SENS_MTX_14,       2, 0x804D },
    {  EEP_OFFSET_EMCS_RGB_MTX21_CASE1,           SREG_RGB_SENS_MTX_21,       2, 0x8019 },
    {  EEP_OFFSET_EMCS_RGB_MTX22_CASE1,           SREG_RGB_SENS_MTX_22,       2, 0x8035 },
    {  EEP_OFFSET_EMCS_RGB_MTX23_CASE1,           SREG_RGB_SENS_MTX_23,       2, 0x8014 },
    {  EEP_OFFSET_EMCS_RGB_MTX24_CASE1,           SREG_RGB_SENS_MTX_24,       2, 0x801D },
    {  EEP_OFFSET_EMCS_RGB_MTX31_CASE1,           SREG_RGB_SENS_MTX_31,       2, 0x805C },
    {  EEP_OFFSET_EMCS_RGB_MTX32_CASE1,           SREG_RGB_SENS_MTX_32,       2, 0x7F64 },
    {  EEP_OFFSET_EMCS_RGB_MTX33_CASE1,           SREG_RGB_SENS_MTX_33,       2, 0x80C2 },
    {  EEP_OFFSET_EMCS_RGB_MTX34_CASE1,           SREG_RGB_SENS_MTX_34,       2, 0x7ECA },
    {  EEP_OFFSET_EMCS_RGB_GAIN_BL_CASE1,         SREG_RGB_SENS_GAIN_BL,      1, 0x007D },
    {  EEP_OFFSET_EMCS_RGB_OFST_BL_CASE1,         SREG_RGB_SENS_OFST_BL,      1, 0x00B0 },
    {  EEP_OFFSET_EMCS_RGB_SUM_CNT_CASE1,         SREG_RGB_SENS_SUM_CNT,      1, 0x0005 },
    {  EEP_OFFSET_EMCS_RGB_THRE_IR_CASE1,         SREG_RGB_SENS_THRE_IR,      1, 0x00FF },
};

static RGB_WBT_IDX s_ui_curr_wbt_idx = RGB_WBT_MAX;
static RGB_WBT_IDX s_ui_prev_wbt_idx = RGB_WBT_MAX;
static UINT8 curr_rgb_color_idx = 0;
static UINT8 prev_rgb_color_idx = 0;

static RGB_DRV_MEMBER_T t_rgb_drv;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void rgb_sreg_write(UINT16 sreg,UINT16 val);
static UINT16 rgb_sreg_read(UINT16 sreg);

//-------------------------------------------------------------------------
// _RGB_StandbyMonitor
//
/**
 *  Monitor standby monitor bit (reg 00 bit 5)
 *  
 */
//-------------------------------------------------------------------------

static UINT16 _RGB_StandbyMonitor(VOID)
{
    UINT8  u1Data;
    UINT32 err_cnt = 0;
    UINT16 ret = RGB_OK;
    
    while(1)
    {    
      if(err_cnt > RGB_ERROR_COUNT)
      {
        ret = RGB_NG; /* Communication failure */
        break;
      }
      
      x_thread_delay(STANDBY_CYCLE_TIME);

      if(vSIFRead(RGB_CLK_DIV, RGB_S11059_ADDR, RGB_CTRL_REG, &u1Data, 1) == 0)
      {
        err_cnt++;
        continue;
      }
      else
      {
        err_cnt = 0;      
      }

      if((u1Data & 0x20) == 0x20)
      {
        ret = RGB_OK;
        break;
      }
    }
    
    return ret;
}


//-------------------------------------------------------------------------
// _RGB_CalculateK
//
/**
 *  Calculate K and RGB_SENS_K
 *  
 */
//-------------------------------------------------------------------------

static UINT16 _RGB_CalculateK(VOID)
{
    UINT32 before_tick;
    UINT32 after_tick;
    UINT32 time_meas;
    UINT32 time_set;
    UINT32 sreg_k;
    UINT16 ret;
        
    UINT16 gain_set  = rgb_sreg_read(SREG_RGB_SENS_GAIN_SET);
    UINT16 int_time  = rgb_sreg_read(SREG_RGB_SENS_INT_TIME);
    UINT16 manu_time = rgb_sreg_read(SREG_RGB_SENS_MANU_TIME);
    
    UINT8 u1Data[CONTROL_BYTE];
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;

    x_memset(u1Data, 0, sizeof(UINT8)*CONTROL_BYTE);
    
    u1Data[0] = 0x84 | (gain_set << 3) | int_time;
    u1Data[1] = (UINT8)((manu_time & 0xFF00) >> 8);
    u1Data[2] = (UINT8)(manu_time & 0xFF);
    vSIFWrite(RGB_CLK_DIV, RGB_S11059_ADDR, RGB_CTRL_REG, u1Data, CONTROL_BYTE);
    
    u1Data[0] = u1Data[0] & 0x7F;
    vSIFWrite(RGB_CLK_DIV, RGB_S11059_ADDR, RGB_CTRL_REG, u1Data, (CONTROL_BYTE-2));
    
    before_tick = x_os_get_sys_tick();

    ret = _RGB_StandbyMonitor();
    
    after_tick = x_os_get_sys_tick();
    time_meas = (after_tick - before_tick)*x_os_get_sys_tick_period();
	if (int_time >= T_INT_CNT)
		{
			RGB_LOG("[RGB] CalK:intial time beyond the maxtime cnt!!!\n");
			int_time = T_INT_CNT-1;
		}
    time_set = (t_int[int_time]*manu_time*4)/1000;
    
    if( time_meas == 0 )
    {
      pt_this->x_k = INC_X(1);
    }
    else
    {
      pt_this->x_k = INC_X(time_set)/time_meas; /* Store k as X point precision */
    }
    
    sreg_k = BOUND_MAX(DEC_X(128 * pt_this->x_k), 0xFF);
    
    rgb_sreg_write(SREG_RGB_SENS_K, (UINT16)sreg_k);
   
    return ret;
}

//-------------------------------------------------------------------------
// _RGB_DataSpreadCorrection
//
/**
 *  Correct Data spread with K
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_DataSum(VOID)
{
    UINT8 sens_sum_cnt;    
    UINT8 sens_thre_ir;    
    UINT16 new_r  = 0;
    UINT16 new_g  = 0;
    UINT16 new_b  = 0;
    UINT16 new_ir = 0;
    BOOL store_new = 0;
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    sens_sum_cnt = rgb_sreg_read(SREG_RGB_SENS_SUM_CNT);
    sens_thre_ir = rgb_sreg_read(SREG_RGB_SENS_THRE_IR);
    
    
    if(sens_sum_cnt == 0)
    {
      if(pt_this->curr_sensor_r == 0)
      {
        if(pt_this->curr_sensor_ir < RGB_IR_THRESHOLD)
        {
          store_new = 1;
        }
      }
      else
      {
        if((sens_thre_ir >= (pt_this->curr_sensor_ir / pt_this->curr_sensor_r)) ||
           (pt_this->curr_sensor_ir < RGB_IR_THRESHOLD))
        {
          store_new = 1;
        }
      }
    }
    else
    {
      if(pt_this->curr_sensor_r == 0)
      {
        if((pt_this->curr_sensor_ir < RGB_IR_THRESHOLD) &&
           (pt_this->curr_sensor_g != (0xFFFF / sens_sum_cnt)))
        {
          store_new = 1;
        }      
      }
      else
      {
        if(((sens_thre_ir >= (pt_this->curr_sensor_ir / pt_this->curr_sensor_r)) ||
            (pt_this->curr_sensor_ir < RGB_IR_THRESHOLD))                        &&
           (pt_this->curr_sensor_g != (0xFFFF / sens_sum_cnt))) 
        {
          store_new = 1;        
        }     
      }    
    }
    
    if(store_new)
    {
      new_r = pt_this->curr_sensor_r;
      new_g = pt_this->curr_sensor_g;
      new_b = pt_this->curr_sensor_b;
      new_ir = pt_this->curr_sensor_ir;
    }
    else
    {
      if(pt_this->prev_sensor_r == 0){
        new_r = rgb_sreg_read(SREG_RGB_SENS_INT_R);
      }else{
        new_r = pt_this->prev_sensor_r;
      }

      if(pt_this->prev_sensor_g == 0){
        new_g = rgb_sreg_read(SREG_RGB_SENS_INT_G);
      }else{
        new_g = pt_this->prev_sensor_g;
      }    
    
      if(pt_this->prev_sensor_b == 0){
        new_b = rgb_sreg_read(SREG_RGB_SENS_INT_B);
      }else{
        new_b = pt_this->prev_sensor_b;
      }
      
      if(pt_this->prev_sensor_ir == 0){
        new_ir = rgb_sreg_read(SREG_RGB_SENS_INT_IR);
      }else{
        new_ir = pt_this->prev_sensor_ir;
      }          
    }
    
    pt_this->sum_r  = BOUND_MAX((pt_this->sum_r + new_r),0xFFFF);
    pt_this->sum_g  = BOUND_MAX((pt_this->sum_g + new_g),0xFFFF);
    pt_this->sum_b  = BOUND_MAX((pt_this->sum_b + new_b),0xFFFF);
    pt_this->sum_ir = BOUND_MAX((pt_this->sum_ir + new_ir),0xFFFF);
      
    pt_this->prev_sensor_r = new_r;
    pt_this->prev_sensor_g = new_g;
    pt_this->prev_sensor_b = new_b;
    pt_this->prev_sensor_ir = new_ir;
}

//-------------------------------------------------------------------------
// _RGB_DataSpreadCorrection
//
/**
 *  Correct Data spread with K
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_DataSpreadCorrection(VOID)
{
//INT64 is not fully supportted
#if 0
    UINT8 i;
    UINT16 rgb_sens_x[SENSOR_CHANNEL_NUM];
    INT64 rgb_sens_coef_gain_x[SENSOR_CHANNEL_NUM];
    INT64 rgb_sens_sum_x[SENSOR_CHANNEL_NUM];
    INT64 rgb_sens_coef_offset_x[SENSOR_CHANNEL_NUM];
    INT64 temp_rgb_sens_x;
    
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    rgb_sens_coef_gain_x[0] = rgb_sreg_read(SREG_RGB_SENS_COEF_GAIN_R);
    rgb_sens_coef_gain_x[1] = rgb_sreg_read(SREG_RGB_SENS_COEF_GAIN_G);
    rgb_sens_coef_gain_x[2] = rgb_sreg_read(SREG_RGB_SENS_COEF_GAIN_B);
    rgb_sens_coef_gain_x[3] = rgb_sreg_read(SREG_RGB_SENS_COEF_GAIN_IR);
    
    rgb_sens_sum_x[0] = rgb_sreg_read(SREG_RGB_SENS_SUM_R);
    rgb_sens_sum_x[1] = rgb_sreg_read(SREG_RGB_SENS_SUM_G);   
    rgb_sens_sum_x[2] = rgb_sreg_read(SREG_RGB_SENS_SUM_B);
    rgb_sens_sum_x[3] = rgb_sreg_read(SREG_RGB_SENS_SUM_IR); 
    
    rgb_sens_coef_offset_x[0] = rgb_sreg_read(SREG_RGB_SENS_COEF_OFST_R);
    rgb_sens_coef_offset_x[1] = rgb_sreg_read(SREG_RGB_SENS_COEF_OFST_G); 
    rgb_sens_coef_offset_x[2] = rgb_sreg_read(SREG_RGB_SENS_COEF_OFST_B);
    rgb_sens_coef_offset_x[3] = rgb_sreg_read(SREG_RGB_SENS_COEF_OFST_IR);
    
    for(i=0;i<SENSOR_CHANNEL_NUM;i++)
    {
      temp_rgb_sens_x = DEC_X((pt_this->x_k * rgb_sens_coef_gain_x[i] * rgb_sens_sum_x[i]) / 128) + (rgb_sens_coef_offset_x[i] - 1024);
      rgb_sens_x[i] = BOUND(temp_rgb_sens_x, 0, 0xFFFF);
    }
    
    rgb_sreg_write(SREG_RGB_SENS_R,  rgb_sens_x[0]);
    rgb_sreg_write(SREG_RGB_SENS_G,  rgb_sens_x[1]);
    rgb_sreg_write(SREG_RGB_SENS_B,  rgb_sens_x[2]);
    rgb_sreg_write(SREG_RGB_SENS_IR, rgb_sens_x[3]);
#endif
}

//-------------------------------------------------------------------------
// _RGB_ColorTempSet
//
/**
 *  Calculate RBG_SENS_T and RGB_SENS_Y
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_ColorTempSet(VOID)
{
//INT64 is not fully supportted
#if 0
    UINT8  i, j;
    INT64 a[RGB_MATRIX_ROW][RGB_MATRIX_COL];
    INT64 b[RGB_MATRIX_COL];
    INT64 x_xyz[RGB_MATRIX_ROW];
    INT64 x_plot_x = 0;
    INT64 x_plot_y = 0;
    INT64 x_n = 0;
    INT64 n1, n2, n3;
    INT32 sens_t;
    UINT8 sreg_plot_x, sreg_plot_y;
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    a[0][0] = rgb_sreg_read(SREG_RGB_SENS_MTX_11) - 32768; 
    a[0][1] = rgb_sreg_read(SREG_RGB_SENS_MTX_12) - 32768; 
    a[0][2] = rgb_sreg_read(SREG_RGB_SENS_MTX_13) - 32768;
    a[0][3] = rgb_sreg_read(SREG_RGB_SENS_MTX_14) - 32768;
    a[1][0] = rgb_sreg_read(SREG_RGB_SENS_MTX_21) - 32768;
    a[1][1] = rgb_sreg_read(SREG_RGB_SENS_MTX_22) - 32768;
    a[1][2] = rgb_sreg_read(SREG_RGB_SENS_MTX_23) - 32768;
    a[1][3] = rgb_sreg_read(SREG_RGB_SENS_MTX_24) - 32768;
    a[2][0] = rgb_sreg_read(SREG_RGB_SENS_MTX_31) - 32768;
    a[2][1] = rgb_sreg_read(SREG_RGB_SENS_MTX_32) - 32768;
    a[2][2] = rgb_sreg_read(SREG_RGB_SENS_MTX_33) - 32768;
    a[2][3] = rgb_sreg_read(SREG_RGB_SENS_MTX_34) - 32768;
    
    b[0] = rgb_sreg_read(SREG_RGB_SENS_R);
    b[1] = rgb_sreg_read(SREG_RGB_SENS_G);
    b[2] = rgb_sreg_read(SREG_RGB_SENS_B);
    b[3] = rgb_sreg_read(SREG_RGB_SENS_IR);
    
    if( (b[0] == 0) && (b[1] == 0) && (b[2] == 0) && (b[3] == 0) )
    {
      pt_this->unfiltered_y = 0;
      rgb_sreg_write(SREG_RGB_SENS_Y, pt_this->unfiltered_y);
      return;    
    }

    
    for (i=0;i<RGB_MATRIX_ROW;i++)
    {
      x_xyz[i] = 0;
      for(j=0;j<RGB_MATRIX_COL;j++)
      {
        x_xyz[i] = x_xyz[i] + (a[i][j]*b[j]);
      }
      x_xyz[i] = INC_X(x_xyz[i]) / 100; /*stored as X point precision */
    }
    
    /* plot_x and plot_y will be stored as X point precision */
    if(( x_xyz[0] + x_xyz[1] + x_xyz[2] ) != 0)
    {
      x_plot_x = INC_X(100 * x_xyz[0]) / ( x_xyz[0] + x_xyz[1] + x_xyz[2] );
    
      x_plot_y = INC_X(100 * x_xyz[1]) / ( x_xyz[0] + x_xyz[1] + x_xyz[2] );
    }
    
    sreg_plot_x = BOUND((DEC_X(x_plot_x)), 0, 0xFF);
    
    sreg_plot_y = BOUND((DEC_X(x_plot_y)), 0, 0xFF);
    
    rgb_sreg_write(SREG_RGB_SENS_PLOT_X, sreg_plot_x);
    rgb_sreg_write(SREG_RGB_SENS_PLOT_Y, sreg_plot_y);

    if(rgb_sreg_read(SREG_RGB_SENS_G) > rgb_sreg_read(SREG_RGB_SENS_IR))
    {
        pt_this->unfiltered_y = BOUND((rgb_sreg_read(SREG_RGB_SENS_G) - rgb_sreg_read(SREG_RGB_SENS_IR)), 0, 0xFFFF);
    }
    else
    {
        pt_this->unfiltered_y = 0;
    }
    rgb_sreg_write(SREG_RGB_SENS_Y, pt_this->unfiltered_y);
    
    /* n = real n * X point precision */ 
    if (( DEC_X(100 * x_plot_y) - 1858 ) != 0)
    {
      x_n = INC_X( DEC_X(100 * x_plot_x) - 3320 ) / ( DEC_X(100 * x_plot_y) - 1858 );   
    }  

    n3 = DEC_X(DEC_X(DEC_X((-437) * x_n) * x_n) * x_n);

    n2 = DEC_X(DEC_X(3601 * x_n) * x_n);
    
    n1 = DEC_X((-6861) * x_n);
    
    sens_t = n3 + n2 + n1 + 5514;
   
    pt_this->unfiltered_t = BOUND(sens_t,0,0xFFFF);
    
    rgb_sreg_write(SREG_RGB_SENS_T, pt_this->unfiltered_t);
#endif
}

//-------------------------------------------------------------------------
// _RGB_IIRFilter
//
/**
 *  8 order IIR Filter
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_IIRFilter(UINT16* input, UINT16* output, UINT8* a_coeff, UINT8 b_coeff)
{
    UINT8   i;
    UINT16  x;
    UINT16* u;
    UINT16* y;
    UINT8*  a;
    UINT8   b;
    UINT32  sum = 0;
    
    y = output;
    u = input;
    x = u[0];
    a = a_coeff;
    b = b_coeff;

    for(i=1;i<(IIR_ORDER+1);i++)
    {
      sum += ((a[i]*u[i])/255);
    }
    
    sum += x;
    
    u[0] = sum;
    
    *y = BOUND_MAX(((UINT32)((b*sum)/255)),0xFFFF);

    i=IIR_ORDER;
    do
    {
      u[i] = u[i-1];
      i--;    
    }while(i>0);      
}

//-------------------------------------------------------------------------
// _RGB_FilterYT
//
/**
 *  Filter RGB_SENS_Y and RGB_SENS_T through 8 order IIR Filter.
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_FilterYT(VOID)
{
    UINT8   a_coeff[IIR_ORDER+1];
    UINT8   b_coeff;
    UINT16  output;
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    a_coeff[1] = rgb_sreg_read(SREG_RGB_SENS_IIR_A1);
    a_coeff[2] = rgb_sreg_read(SREG_RGB_SENS_IIR_A2);
    a_coeff[3] = rgb_sreg_read(SREG_RGB_SENS_IIR_A3);
    a_coeff[4] = rgb_sreg_read(SREG_RGB_SENS_IIR_A4);
    a_coeff[5] = rgb_sreg_read(SREG_RGB_SENS_IIR_A5);
    a_coeff[6] = rgb_sreg_read(SREG_RGB_SENS_IIR_A6);
    a_coeff[7] = rgb_sreg_read(SREG_RGB_SENS_IIR_A7);
    a_coeff[8] = rgb_sreg_read(SREG_RGB_SENS_IIR_A8);
    
    b_coeff = rgb_sreg_read(SREG_RGB_SENS_IIR_B0);

    pt_this->y_history[0] = rgb_sreg_read(SREG_RGB_SENS_Y);
    pt_this->t_history[0] = rgb_sreg_read(SREG_RGB_SENS_T);
    
    _RGB_IIRFilter(pt_this->y_history, &output, a_coeff, b_coeff);
    output = BOUND_MAX(output,0xFFFF);
    rgb_sreg_write(SREG_RGB_SENS_Y, output);
       
    _RGB_IIRFilter(pt_this->t_history, &output, a_coeff, b_coeff);
    output = BOUND_MAX(output,0xFFFF);
    rgb_sreg_write(SREG_RGB_SENS_T, output);    

}

//-------------------------------------------------------------------------
// _RGB_FilterRGB
//
/**
 *  Filter RGB_SENS_GAIN_R, RGB_SENS_GAIN_G, RGB_SENS_GAIN_B through 8 order IIR Filter.
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_FilterRGB(UINT8* gain_x, UINT16* gain_x_history)
{
    UINT8   a_coeff[IIR_ORDER+1];
    UINT8   b_coeff;
    UINT16  sens_gain_x;
    
    sens_gain_x = *gain_x;
    
    a_coeff[1] = rgb_sreg_read(SREG_RGB_SENS_IIR_A11);
    a_coeff[2] = rgb_sreg_read(SREG_RGB_SENS_IIR_A12);
    a_coeff[3] = rgb_sreg_read(SREG_RGB_SENS_IIR_A13);
    a_coeff[4] = rgb_sreg_read(SREG_RGB_SENS_IIR_A14);
    a_coeff[5] = rgb_sreg_read(SREG_RGB_SENS_IIR_A15);
    a_coeff[6] = rgb_sreg_read(SREG_RGB_SENS_IIR_A16);
    a_coeff[7] = rgb_sreg_read(SREG_RGB_SENS_IIR_A17);
    a_coeff[8] = rgb_sreg_read(SREG_RGB_SENS_IIR_A18);
    
    b_coeff = rgb_sreg_read(SREG_RGB_SENS_IIR_B1);

    gain_x_history[0] = sens_gain_x;
    
    _RGB_IIRFilter(gain_x_history, &sens_gain_x, a_coeff, b_coeff);
    *gain_x = BOUND_MAX((sens_gain_x),0xFF);
}

static INT32 _RGB_MapStepFunc( const INT32 status,
						                   const RGB_WBT_STEP_FUNC_TH *th,
						                   const RGB_WBT_STEP_FUNC_OFST *ofst )
{
	INT32 ret;
	BOOL param_is_ok = (BOOL)( (th != NULL) && (ofst != NULL) &&  (th->th1 <= th->th2) && (th->th2 <= th->th3) );
	
	if( param_is_ok != (BOOL)FALSE ){
		if(status < th->th1){
			ret = ofst->l;
		}else if(status < th->th2){
			// min(ofst->l, ofst->m) < offset < max(ofst->l, ofst->m)
			// (th->th2 - th->th1) cannot be 0.
			ret = ofst->l +  ( ( (ofst->m - ofst->l) * (status - th->th1) ) / (th->th2 - th->th1) );
		}else if(status < th->th3){
			// min(ofst->m, ofst->h) < offset < max(ofst->m, ofst->h)
			// (th->th4 - th->th3) cannot be 0.
			ret = ofst->m +  ( ( (ofst->h - ofst->m) * (status - th->th2) ) / (th->th3 - th->th2) );
		}else{
			ret = ofst->h;
		}
	}else{
		ret = 0;
	}
	
	return ret;
}

//-------------------------------------------------------------------------
// _RGB_BLCtrlCalc
//
/**
 *  Set Gain from MAP_OPT or MAP_RGB base on threshold value.
 *  Use Unfiltered RGB_SENS_Y 
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_RGBGainSet(VOID)
{
    UINT8    i;
    UINT32   rgb_sens_gain_rgb_x[OUTPUT_CHANNEL_NUM];
    UINT32   rgb_sens_trx_x[OUTPUT_CHANNEL_NUM][OUTPUT_CHANNEL_NUM];
    UINT32   rgb_sens_rgbx_x[OUTPUT_CHANNEL_NUM][OUTPUT_CHANNEL_NUM];
    UINT32   rgb_sens_gain_opt_x[OUTPUT_CHANNEL_NUM];
    UINT32   rgb_sens_tox_x[OUTPUT_CHANNEL_NUM][OUTPUT_CHANNEL_NUM];
    UINT32   rgb_sens_optx_x[OUTPUT_CHANNEL_NUM][OUTPUT_CHANNEL_NUM];
    UINT8    rgb_sens_gain_x[OUTPUT_CHANNEL_NUM];
    UINT32   sens_t, sens_y;
    UINT16   rgb_sens_opt_thre = 0;
    RGB_WBT_STEP_FUNC_TH    map_th;
    RGB_WBT_STEP_FUNC_OFST  map_ofst;  
    
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    sens_t = rgb_sreg_read(SREG_RGB_SENS_T);
    sens_y = rgb_sreg_read(SREG_RGB_SENS_Y);
    
    rgb_sens_trx_x[0][0] = rgb_sreg_read(SREG_RGB_SENS_TR1_R);
    rgb_sens_trx_x[0][1] = rgb_sreg_read(SREG_RGB_SENS_TR2_R);
    rgb_sens_trx_x[0][2] = rgb_sreg_read(SREG_RGB_SENS_TR3_R);
    rgb_sens_trx_x[1][0] = rgb_sreg_read(SREG_RGB_SENS_TR1_G); 
    rgb_sens_trx_x[1][1] = rgb_sreg_read(SREG_RGB_SENS_TR2_G);
    rgb_sens_trx_x[1][2] = rgb_sreg_read(SREG_RGB_SENS_TR3_G);
    rgb_sens_trx_x[2][0] = rgb_sreg_read(SREG_RGB_SENS_TR1_B); 
    rgb_sens_trx_x[2][1] = rgb_sreg_read(SREG_RGB_SENS_TR2_B);
    rgb_sens_trx_x[2][2] = rgb_sreg_read(SREG_RGB_SENS_TR3_B);
    
    rgb_sens_rgbx_x[0][0] = rgb_sreg_read(SREG_RGB_SENS_RGB1_R);
    rgb_sens_rgbx_x[0][1] = rgb_sreg_read(SREG_RGB_SENS_RGB2_R);
    rgb_sens_rgbx_x[0][2] = rgb_sreg_read(SREG_RGB_SENS_RGB3_R);
    rgb_sens_rgbx_x[1][0] = rgb_sreg_read(SREG_RGB_SENS_RGB1_G);
    rgb_sens_rgbx_x[1][1] = rgb_sreg_read(SREG_RGB_SENS_RGB2_G);
    rgb_sens_rgbx_x[1][2] = rgb_sreg_read(SREG_RGB_SENS_RGB3_G);
    rgb_sens_rgbx_x[2][0] = rgb_sreg_read(SREG_RGB_SENS_RGB1_B);
    rgb_sens_rgbx_x[2][1] = rgb_sreg_read(SREG_RGB_SENS_RGB2_B);
    rgb_sens_rgbx_x[2][2] = rgb_sreg_read(SREG_RGB_SENS_RGB3_B);

    rgb_sens_tox_x[0][0] = rgb_sreg_read(SREG_RGB_SENS_TO1_R);
    rgb_sens_tox_x[0][1] = rgb_sreg_read(SREG_RGB_SENS_TO2_R);
    rgb_sens_tox_x[0][2] = rgb_sreg_read(SREG_RGB_SENS_TO3_R);
    rgb_sens_tox_x[1][0] = rgb_sreg_read(SREG_RGB_SENS_TO1_G);
    rgb_sens_tox_x[1][1] = rgb_sreg_read(SREG_RGB_SENS_TO2_G);
    rgb_sens_tox_x[1][2] = rgb_sreg_read(SREG_RGB_SENS_TO3_G);
    rgb_sens_tox_x[2][0] = rgb_sreg_read(SREG_RGB_SENS_TO1_B);
    rgb_sens_tox_x[2][1] = rgb_sreg_read(SREG_RGB_SENS_TO2_B);
    rgb_sens_tox_x[2][2] = rgb_sreg_read(SREG_RGB_SENS_TO3_B);
    
    rgb_sens_optx_x[0][0] = rgb_sreg_read(SREG_RGB_SENS_OPT1_R);
    rgb_sens_optx_x[0][1] = rgb_sreg_read(SREG_RGB_SENS_OPT2_R);
    rgb_sens_optx_x[0][2] = rgb_sreg_read(SREG_RGB_SENS_OPT3_R);
    rgb_sens_optx_x[1][0] = rgb_sreg_read(SREG_RGB_SENS_OPT1_G);
    rgb_sens_optx_x[1][1] = rgb_sreg_read(SREG_RGB_SENS_OPT2_G);
    rgb_sens_optx_x[1][2] = rgb_sreg_read(SREG_RGB_SENS_OPT3_G);
    rgb_sens_optx_x[2][0] = rgb_sreg_read(SREG_RGB_SENS_OPT1_B);
    rgb_sens_optx_x[2][1] = rgb_sreg_read(SREG_RGB_SENS_OPT2_B);
    rgb_sens_optx_x[2][2] = rgb_sreg_read(SREG_RGB_SENS_OPT3_B);   

    
    for(i=0;i<OUTPUT_CHANNEL_NUM;i++)
    {
      map_th.th1 = rgb_sens_trx_x[i][2];
      map_th.th2 = rgb_sens_trx_x[i][1];
      map_th.th3 = rgb_sens_trx_x[i][0];
      
      map_ofst.l = rgb_sens_rgbx_x[i][2];
      map_ofst.m = rgb_sens_rgbx_x[i][1];
      map_ofst.h = rgb_sens_rgbx_x[i][0];
      
      rgb_sens_gain_rgb_x[i] = _RGB_MapStepFunc( sens_t, &map_th, &map_ofst );
    }
    
    for(i=0;i<OUTPUT_CHANNEL_NUM;i++)
    {
      map_th.th1 = rgb_sens_tox_x[i][2];
      map_th.th2 = rgb_sens_tox_x[i][1];
      map_th.th3 = rgb_sens_tox_x[i][0];
      
      map_ofst.l = rgb_sens_optx_x[i][2];
      map_ofst.m = rgb_sens_optx_x[i][1];
      map_ofst.h = rgb_sens_optx_x[i][0];
      
      rgb_sens_gain_opt_x[i] = _RGB_MapStepFunc( sens_y, &map_th, &map_ofst );
    }
    
    
    if(pt_this->curr_map == MAP_OPT)
    {
      rgb_sens_opt_thre = rgb_sreg_read(SREG_RGB_SENS_OPT_THRE_H);
    }
    else if(pt_this->curr_map == MAP_RGB)
    {
      rgb_sens_opt_thre = rgb_sreg_read(SREG_RGB_SENS_OPT_THRE_L);
    }
        
    if(sens_y < rgb_sens_opt_thre)
    {
      pt_this->curr_map = MAP_OPT;       
    }
    else
    {
      pt_this->curr_map = MAP_RGB;  
    } 
      
    
    if(rgb_sreg_read(SREG_RGB_SENS_ON))
    {
      if(pt_this->curr_map == MAP_OPT)
      {
        for(i=0;i<OUTPUT_CHANNEL_NUM;i++)
        {
          rgb_sens_gain_x[i] = (UINT8)rgb_sens_gain_opt_x[i];
        }
      }
      else if(pt_this->curr_map == MAP_RGB)
      {
        for(i=0;i<OUTPUT_CHANNEL_NUM;i++)
        {
          rgb_sens_gain_x[i] = (UINT8)rgb_sens_gain_rgb_x[i];
        }
      }
    }
    else
    {
        for(i=0;i<OUTPUT_CHANNEL_NUM;i++)
        {
          rgb_sens_gain_x[i] = 128;
        }    
    }

    _RGB_FilterRGB(&rgb_sens_gain_x[0], pt_this->gain_r_history);
    _RGB_FilterRGB(&rgb_sens_gain_x[1], pt_this->gain_g_history);
    _RGB_FilterRGB(&rgb_sens_gain_x[2], pt_this->gain_b_history);
    
    rgb_sreg_write(SREG_RGB_SENS_GAIN_R, rgb_sens_gain_x[0]);
    rgb_sreg_write(SREG_RGB_SENS_GAIN_G, rgb_sens_gain_x[1]);
    rgb_sreg_write(SREG_RGB_SENS_GAIN_B, rgb_sens_gain_x[2]);
      
}

//-------------------------------------------------------------------------
// _RGB_BLCtrlCalc
//
/**
 *  Calculate OPT_READ_DATA for backlight control.
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_BLCtrlCalc(VOID)
{
    UINT32 opt_data;
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;    
    UINT16 sens_y  = pt_this->unfiltered_y;
    UINT8  gain_bl = rgb_sreg_read(SREG_RGB_SENS_GAIN_BL);
    UINT8  ofst_bl = rgb_sreg_read(SREG_RGB_SENS_OFST_BL);
    
    opt_data = ((UINT32)gain_bl*(UINT32)sens_y)/255;
    opt_data = BOUND(((INT32)opt_data+(INT32)ofst_bl-128), 0, 0xFFFF);
    
    pt_this->opt_read_data = opt_data;
    
    if(pt_this->ir_continuous == FALSE)
    {
//no need, bl use new module      bl_sreg_write(SREG_OPT_READ_DATA, pt_this->opt_read_data);
    }
}

//-------------------------------------------------------------------------
// _RGB_ReadSensorData
//
/**
 *  Write I2C data to RGB sensor. Control Data to 0x00-0x02
 *  
 */
//-------------------------------------------------------------------------
static UINT16 _RGB_WriteControlData(VOID)
{
    UINT8 u1Data[CONTROL_BYTE];
    UINT16 gain_set  = rgb_sreg_read(SREG_RGB_SENS_GAIN_SET);
    UINT16 int_time  = rgb_sreg_read(SREG_RGB_SENS_INT_TIME);
    UINT16 manu_time = rgb_sreg_read(SREG_RGB_SENS_MANU_TIME);
    
    x_memset(u1Data, 0, sizeof(UINT8)*CONTROL_BYTE);
    
    u1Data[0] = 0x84 | ((UINT8)gain_set << 3) | (UINT8)int_time;
    u1Data[1] = (UINT8)((manu_time & 0xFF00) >> 8);
    u1Data[2] = (UINT8)(manu_time & 0xFF);
    vSIFWrite(RGB_CLK_DIV, RGB_S11059_ADDR, RGB_CTRL_REG, u1Data, CONTROL_BYTE);
    
    u1Data[0] = u1Data[0] & 0x7F;
    vSIFWrite(RGB_CLK_DIV, RGB_S11059_ADDR, RGB_CTRL_REG, u1Data, (CONTROL_BYTE-2));
    
    return 0;
}

//-------------------------------------------------------------------------
// _RGB_ReadSensorData
//
/**
 *  Read I2C data from RGB sensor. Status Data from 0x03-0x0A
 *  
 */
//-------------------------------------------------------------------------
static UINT16 _RGB_ReadSensorData(VOID)
{

    UINT8 u1Data[STATUS_BYTE];
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    x_memset(u1Data, 0, sizeof(UINT8)*STATUS_BYTE);
    vSIFRead(RGB_CLK_DIV, RGB_S11059_ADDR, RGB_SENS_REG, u1Data, STATUS_BYTE);
    
    pt_this->curr_sensor_r  = (UINT16)u1Data[0];
    pt_this->curr_sensor_r  = (pt_this->curr_sensor_r << 8) | (UINT16)u1Data[1];
    pt_this->curr_sensor_g  = (UINT16)u1Data[2];
    pt_this->curr_sensor_g  = (pt_this->curr_sensor_g << 8) | (UINT16)u1Data[3];
    pt_this->curr_sensor_b  = (UINT16)u1Data[4];
    pt_this->curr_sensor_b  = (pt_this->curr_sensor_b << 8) | (UINT16)u1Data[5];
    pt_this->curr_sensor_ir = (UINT16)u1Data[6];
    pt_this->curr_sensor_ir = (pt_this->curr_sensor_ir << 8) | (UINT16)u1Data[7];
   
    rgb_sreg_write(SREG_RGB_SENS_CNT_R,  pt_this->curr_sensor_r);
    rgb_sreg_write(SREG_RGB_SENS_CNT_G,  pt_this->curr_sensor_g);
    rgb_sreg_write(SREG_RGB_SENS_CNT_B,  pt_this->curr_sensor_b);
    rgb_sreg_write(SREG_RGB_SENS_CNT_IR, pt_this->curr_sensor_ir);
 
    return 0;
}

//-------------------------------------------------------------------------
// _RGB_DetectTimeout
//
/**
 *  Timer handler function for h_rgbThreadSema. Integ time/1sec timer to 
 *  release RGB detection thread 
 *  
 */
//-------------------------------------------------------------------------
static VOID _RGB_DetectTimeout(
    HANDLE_T  h_timer,
    VOID      *pv_tag)
{
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;

    if (pt_this->h_rgbThreadSema != NULL_HANDLE)
    {
      SignalSemaphore(pt_this->h_rgbThreadSema);
    }
}


//-------------------------------------------------------------------------
// _RGB_IRTimeout
//
/**
 *  Timer handler function for h_rgbIrContTimer. 300ms timer
 */
//-------------------------------------------------------------------------
static VOID _RGB_IrResumeTimeout(
    HANDLE_T  h_timer,
    VOID      *pv_tag)
{
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    pt_this->ir_continuous = FALSE;
    RGB_LOG("[RGB] Resume optical data get\n"); 
}

//-------------------------------------------------------------------------
// _RGB_IRTimeout
//
/**
 *  Timer handler function for h_rgbIrContTimer. 300ms timer
 */
//-------------------------------------------------------------------------
static VOID _RGB_IRTimeout(
    HANDLE_T  h_timer,
    VOID      *pv_tag)
{
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    pt_this->ir_continuous = TRUE;    
    RGB_LOG("[RGB] Pause optical data get\n");          
}

//-------------------------------------------------------------------------
// _RGB_EEPRead
//
/**
 *  Read data from RGB area on EEP
 */
//-------------------------------------------------------------------------
static VOID _RGB_EEPRead(UINT16 offset, UINT8* buffer, UINT16 len)
{  
    if((offset >= EEP_OFFSET_EMCS_RGB_AREA_START) &&
       (offset <= EEP_OFFSET_EMCS_RGB_AREA_END))
    {
      UNUSED(EEPROM_Read((UINT64)offset, (UINT32)buffer, len));
    }
}

//-------------------------------------------------------------------------
// _RGB_EEPWrite
//
/**
 *  Read data from RGB area on EEP
 */
//-------------------------------------------------------------------------
static VOID _RGB_EEPWrite(UINT16 offset, UINT8* buffer, UINT16 len)
{   
    if((offset >= EEP_OFFSET_EMCS_RGB_AREA_START) &&
       (offset <= EEP_OFFSET_EMCS_RGB_AREA_END))
    {
      UNUSED(EEPROM_Write((UINT64)offset, (UINT32)buffer, len));
    }
}

//-------------------------------------------------------------------------
// _RGB_LoadMap
//
/**
 *  Load MAP_OPT and MAP_RGB from EEP
 */
//-------------------------------------------------------------------------
static VOID _RGB_LoadMap(RGB_WBT_IDX idx, UINT8* buffer)
{
    if (idx != RGB_WBT_MAX)
    {
      UINT16 min_ofst;
      UINT16 max_ofst;
      UINT16 tbl_cnt;
      UINT16 data;
      UINT16 len = 0;
    
      switch(idx)
      {
        case RGB_WBT_COOL:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_COOL;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_COOL;
          break;
        case RGB_WBT_NEUTRAL:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_NEUTRAL;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_NEUTRAL;
          break;
        case RGB_WBT_WARM1:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_WARM1;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_WARM1;
          break;
        case RGB_WBT_WARM2:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_WARM2;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_WARM2;
          break;
        case RGB_WBT_PHOTO:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_PHOTO;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_PHOTO;
          break;
        default:
          min_ofst = EEP_MAP_INVALID_IDX;
          max_ofst = EEP_MAP_INVALID_IDX;
          break;
      }
      
      if((min_ofst == EEP_MAP_INVALID_IDX) || (max_ofst == EEP_MAP_INVALID_IDX))
      {
        return;
      }
    
      x_memset(buffer, 0, (sizeof(UINT8)*RGB_TOTAL_MAP_SIZE));
      for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_wbt)/sizeof(tbl_rgb_eep_wbt[0])); tbl_cnt++)
      {
        if ((tbl_rgb_eep_wbt[tbl_cnt].offset >= min_ofst) &&
            (tbl_rgb_eep_wbt[tbl_cnt].offset <= max_ofst))
        {
      
          len += tbl_rgb_eep_wbt[tbl_cnt].byte_size;
        }
      }
    
      _RGB_EEPRead(min_ofst, buffer, len);
    
      len=0;
      for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_wbt)/sizeof(tbl_rgb_eep_wbt[0])); tbl_cnt++)
      {
        if ((tbl_rgb_eep_wbt[tbl_cnt].offset >= min_ofst) &&
            (tbl_rgb_eep_wbt[tbl_cnt].offset <= max_ofst))
        {
          if(tbl_rgb_eep_wbt[tbl_cnt].byte_size == 2)
          {
            data = (UINT16)buffer[len];
            len++;
            data = data << 8;
            data = data | ((UINT16)buffer[len] & 0xFF);
            len++; 
            rgb_sreg_write(tbl_rgb_eep_wbt[tbl_cnt].sreg, data);         
          }
          else
          {
            data = (UINT16)buffer[len];
            len++;
            rgb_sreg_write(tbl_rgb_eep_wbt[tbl_cnt].sreg, data);              
          }
        }
      }
    }
    else
    {
      /* Should not load this unless wbt idx not set from external before thread start */
      UINT8 i;
      UINT16 sreg_idx = SREG_RGB_SENS_TR1_R;
      
      for (i=0; i<RGB_TOTAL_MAP_ELEM; i++)
      {
        rgb_sreg_write(sreg_idx, tbl_wbt_idx_max[i]);
        sreg_idx++;
      }
    }
}

//-------------------------------------------------------------------------
// _RGB_LoadColorVar
//
/**
 *  Load Color variation related registers
 */
//-------------------------------------------------------------------------
static VOID _RGB_LoadColorVar(UINT8 color_idx, UINT8* buffer)
{

    UINT16 min_ofst;
    UINT16 max_ofst;
    UINT16 tbl_cnt;
    UINT16 data;
    UINT16 len = 0;

    if(color_idx > RGB_COLOR_VAR_NUM)
    {
      color_idx = 0;
    }
          
    switch(color_idx)
    {
      case 0:    
      case 1:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE1;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE1;
        break;
      case 2:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE2;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE2;
        break;
      case 3:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE3;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE3;
        break;
      case 4:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE4;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE4;
        break;
      case 5:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE5;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE5;
        break;
      case 6:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE6;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE6;
        break;
      case 7:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE7;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE7;
        break;
      case 8:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE8;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE8;
        break;
      default:
        min_ofst = EEP_MAP_INVALID_IDX;
        max_ofst = EEP_MAP_INVALID_IDX;
        break;
    }
    
    if((min_ofst == EEP_MAP_INVALID_IDX) || (max_ofst == EEP_MAP_INVALID_IDX))
    {
      return;
    }
        
    if(color_idx > 0)
    {
      x_memset(buffer, 0, (sizeof(UINT8)*RGB_COLOR_VAR_SIZE));
      for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_color)/sizeof(tbl_rgb_eep_color[0])); tbl_cnt++)
      {
        if ((tbl_rgb_eep_color[tbl_cnt].offset >= min_ofst) &&
            (tbl_rgb_eep_color[tbl_cnt].offset <= max_ofst))
        {
      
          len += tbl_rgb_eep_color[tbl_cnt].byte_size;
        }
      }
    
      _RGB_EEPRead(min_ofst, buffer, len);
    
      len=0;
      for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_color)/sizeof(tbl_rgb_eep_color[0])); tbl_cnt++)
      {
        if ((tbl_rgb_eep_color[tbl_cnt].offset >= min_ofst) &&
            (tbl_rgb_eep_color[tbl_cnt].offset <= max_ofst))
        {
          if(tbl_rgb_eep_color[tbl_cnt].byte_size == 2)
          {
            data = (UINT16)buffer[len];
            len++;
            data = data << 8;
            data = data | ((UINT16)buffer[len] & 0xFF);
            len++; 
            rgb_sreg_write(tbl_rgb_eep_color[tbl_cnt].sreg, data);         
          }
          else
          {
            data = (UINT16)buffer[len];
            len++;
            rgb_sreg_write(tbl_rgb_eep_color[tbl_cnt].sreg, data);              
          }
        }
      }
    }
    else
    {
      UINT8 i = 0;
      /* Should not load this unless color_idx not between 1 - 8 */
      for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_color)/sizeof(tbl_rgb_eep_color[0])); tbl_cnt++)
      {
        if ((tbl_rgb_eep_color[tbl_cnt].offset >= min_ofst) &&
            (tbl_rgb_eep_color[tbl_cnt].offset <= max_ofst))
        {
          if(i < RGB_COLOR_VAR_ELEMENT)
          {
            rgb_sreg_write(tbl_rgb_eep_color[tbl_cnt].sreg, tbl_color_default[i++]);      
          }   
        }
      }
    }
}

//-------------------------------------------------------------------------
// _RGB_InitSreg
//
/**
 *  Initialize SREG from EEP data
 */
//-------------------------------------------------------------------------
static VOID _RGB_InitSreg(UINT8* buffer)
{
    UINT16 tbl_cnt;
    UINT16 data;
    
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_main)/sizeof(tbl_rgb_eep_main[0])); tbl_cnt++)
    {  
      x_memset(buffer, 0, (sizeof(UINT8)*RGB_EEP_SIZE));
      
      _RGB_EEPRead(tbl_rgb_eep_main[tbl_cnt].offset, buffer, tbl_rgb_eep_main[tbl_cnt].byte_size);

      if(tbl_rgb_eep_main[tbl_cnt].byte_size == 2)
      {
        data = (UINT16)buffer[0];
        data = data << 8;
        data = data | ((UINT16)buffer[1] & 0xFF);
        rgb_sreg_write(tbl_rgb_eep_main[tbl_cnt].sreg, data);         
      }
      else
      {
        data = (UINT16)buffer[0];
        rgb_sreg_write(tbl_rgb_eep_main[tbl_cnt].sreg, data);              
      }
    }
     
    _RGB_LoadMap(s_ui_curr_wbt_idx, buffer);

    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_main)/sizeof(tbl_rgb_eep_main[0])); tbl_cnt++)
    {
      if(tbl_rgb_eep_main[tbl_cnt].offset == EEP_OFFSET_EMCS_RGB_GAISOU_ECS)
      {
        x_memset(buffer, 0, (sizeof(UINT8)*RGB_EEP_SIZE));
        _RGB_EEPRead(tbl_rgb_eep_main[tbl_cnt].offset, buffer, tbl_rgb_eep_main[tbl_cnt].byte_size);
        curr_rgb_color_idx = buffer[0];
        break;
      }
    }
    
    prev_rgb_color_idx = curr_rgb_color_idx;
    _RGB_LoadColorVar(curr_rgb_color_idx, buffer);

    rgb_sreg_write(SREG_RGB_SENS_GAIN_R, 128);
    rgb_sreg_write(SREG_RGB_SENS_GAIN_G, 128);
    rgb_sreg_write(SREG_RGB_SENS_GAIN_B, 128);
//no need, bl use new module    bl_sreg_write(SREG_OPT_READ_DATA, 255);
    rgb_sreg_write(SREG_RGB_SENS_IIR_RESET,0);
}

//-------------------------------------------------------------------------
// _RGB_EEPCheck
//
/**
 *  Initialize NVM per table
 */
//-------------------------------------------------------------------------
static VOID _RGB_InitEEP(RGB_DRV_EEP* tbl_ptr, UINT8 * buffer, UINT16 size)
{
    UINT16 tbl_cnt;
    UINT16 len = 0;

    x_memset(buffer, 0, (sizeof(UINT8)*RGB_EEP_SIZE));
    for (tbl_cnt=0; tbl_cnt<size; tbl_cnt++)
    {
      if(tbl_ptr[tbl_cnt].byte_size == 2)
      {
        buffer[len] = (UINT8)((tbl_ptr[tbl_cnt].init & 0xFF00) >> 8);
        len++;
        buffer[len] = (UINT8)(tbl_ptr[tbl_cnt].init & 0xFF);
        len++;           
      }
      else
      {
        buffer[len] = (UINT8)(tbl_ptr[tbl_cnt].init & 0xFF);
        len++;              
      }
    }
    _RGB_EEPWrite(tbl_ptr[0].offset, buffer, len);

}
//-------------------------------------------------------------------------
// _RGB_EEPCheck
//
/**
 *  Check for NVM integrity in RGB area, if NG, load default value to
 *  compensate for fatal operation 
 */
//-------------------------------------------------------------------------
static VOID _RGB_EEPCheck(UINT8 * buffer)
{
    UINT16 chksum_marker = 0;
    UINT16 tbl_cnt;
    UINT16 data;
    
    for (tbl_cnt=0; tbl_cnt<(sizeof(tbl_rgb_eep_main)/sizeof(tbl_rgb_eep_main[0])); tbl_cnt++)
    {
      if(tbl_rgb_eep_main[tbl_cnt].offset == EEP_OFFSET_EMCS_RGB_CHECKSUM)
      {
        chksum_marker = tbl_cnt;
        break;
      }
    }

	if (tbl_cnt < (sizeof(tbl_rgb_eep_main)/sizeof(tbl_rgb_eep_main[0])))
    {
    if(tbl_rgb_eep_main[chksum_marker].offset == EEP_OFFSET_EMCS_RGB_CHECKSUM) /* First element in table is checksum byte */
    {
      x_memset(buffer, 0, (sizeof(UINT8)*RGB_EEP_SIZE));
      _RGB_EEPRead(tbl_rgb_eep_main[tbl_cnt].offset, buffer, tbl_rgb_eep_main[tbl_cnt].byte_size);
      data = buffer[0];
      
      if(data != RGB_EEP_CHKSUM)
      {
        /* RGB Data area corruption, init with default data */
        /* Prepare data to write */
        _RGB_InitEEP((RGB_DRV_EEP*)tbl_rgb_eep_main, buffer, (sizeof(tbl_rgb_eep_main)/sizeof(tbl_rgb_eep_main[0])));
        _RGB_InitEEP((RGB_DRV_EEP*)tbl_rgb_eep_wbt, buffer, (sizeof(tbl_rgb_eep_wbt)/sizeof(tbl_rgb_eep_wbt[0])));
        _RGB_InitEEP((RGB_DRV_EEP*)tbl_rgb_eep_color, buffer, (sizeof(tbl_rgb_eep_color)/sizeof(tbl_rgb_eep_color[0])));
      }
    }
    }
	else
		{
			 RGB_LOG("[RGB] EEP check fail!!!\n");
		}
			
}

//-------------------------------------------------------------------------
// rgb_sreg_write
//
/**
 *  local function to write to sreg
 */
//-------------------------------------------------------------------------
static void rgb_sreg_write(UINT16 sreg,UINT16 val)
{
  RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
  
  LockMutex(pt_this->h_rgbSregMutex);
  
	if(sreg<MAX_RGB_SREG)
	{
		pt_this->SREG[sreg] = val;
	}
	
  UnlockMutex(pt_this->h_rgbSregMutex);
}

//-------------------------------------------------------------------------
// rgb_sreg_read
//
/**
 *  local function to write to sreg
 */
//-------------------------------------------------------------------------
static UINT16 rgb_sreg_read(UINT16 sreg)
{
  UINT16 ret = 0;
	RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
	
  LockMutex(pt_this->h_rgbSregMutex);
	
	if(sreg<MAX_RGB_SREG)
	{
		ret = pt_this->SREG[sreg];
	}
	else
	{
		ret = 0;
	}
	
  UnlockMutex(pt_this->h_rgbSregMutex);
	
	return(ret);
}

//-------------------------------------------------------------------------
// rgb_sreg_to_eep_write
//
/**
 *  local function to write to eep in case sreg value change
 */
//-------------------------------------------------------------------------
static void rgb_sreg_to_eep_write(UINT16 sreg,UINT16 val)
{
  RGB_DRV_EEP* tbl_ptr = NULL;
  UINT16 i;
  UINT16 min_ofst = EEP_MAP_INVALID_IDX;
  UINT16 max_ofst = EEP_MAP_INVALID_IDX;
  UINT8 buffer[2];
  

  if((sreg >=  SREG_RGB_SENS_TR1_R) && (sreg <=  SREG_RGB_SENS_OPT3_B))
  {
    if (s_ui_curr_wbt_idx != RGB_WBT_MAX)
    {
      switch(s_ui_curr_wbt_idx)
      {
        case RGB_WBT_COOL:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_COOL;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_COOL;
          break;
        case RGB_WBT_NEUTRAL:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_NEUTRAL;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_NEUTRAL;
          break;
        case RGB_WBT_WARM1:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_WARM1;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_WARM1;
          break;
        case RGB_WBT_WARM2:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_WARM2;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_WARM2;
          break;
        case RGB_WBT_PHOTO:
          min_ofst = EEP_OFFSET_EMCS_RGB_TR1_R_PHOTO;
          max_ofst = EEP_OFFSET_EMCS_RGB_OPT3_B_PHOTO;
          break;
        default:
          min_ofst = EEP_MAP_INVALID_IDX;
          max_ofst = EEP_MAP_INVALID_IDX;
          break;
      }
    }
    
    if ((min_ofst != EEP_MAP_INVALID_IDX) && (max_ofst != EEP_MAP_INVALID_IDX))
    {
      for (i=0; i<(sizeof(tbl_rgb_eep_wbt)/sizeof(tbl_rgb_eep_wbt[0])); i++)
      {
        if ((tbl_rgb_eep_wbt[i].offset >= min_ofst) &&
            (tbl_rgb_eep_wbt[i].offset <= max_ofst))
        {
          if(tbl_rgb_eep_wbt[i].sreg == sreg)
          {
            tbl_ptr = (RGB_DRV_EEP*)&tbl_rgb_eep_wbt[i];
            break;    
          }
        }
      }
    }  
  }
  else if((sreg >=  SREG_RGB_SENS_MANU_TIME) && (sreg <=  SREG_RGB_SENS_THRE_IR))
  {
    switch(curr_rgb_color_idx)
    {
      case 1:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE1;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE1;
        break;
      case 2:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE2;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE2;
        break;
      case 3:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE3;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE3;
        break;
      case 4:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE4;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE4;
        break;
      case 5:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE5;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE5;
        break;
      case 6:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE6;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE6;
        break;
      case 7:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE7;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE7;
        break;
      case 8:
        min_ofst = EEP_OFFSET_EMCS_RGB_MANU_TIME_CASE8;
        max_ofst = EEP_OFFSET_EMCS_RGB_THRE_IR_CASE8;
        break;
      default:
        min_ofst = EEP_MAP_INVALID_IDX;
        max_ofst = EEP_MAP_INVALID_IDX;
        break;
    }
    
    if ((min_ofst != EEP_MAP_INVALID_IDX) && (max_ofst != EEP_MAP_INVALID_IDX))
    {
      for (i=0; i<(sizeof(tbl_rgb_eep_color)/sizeof(tbl_rgb_eep_color[0])); i++)
      {
        if ((tbl_rgb_eep_color[i].offset >= min_ofst) &&
            (tbl_rgb_eep_color[i].offset <= max_ofst))
        {
          if(tbl_rgb_eep_color[i].sreg == sreg)
          {
            tbl_ptr = (RGB_DRV_EEP*)&tbl_rgb_eep_color[i];
            break;    
          }
        }
      }
    }      
  }
  else
  {
    for (i=0; i<(sizeof(tbl_rgb_eep_main)/sizeof(tbl_rgb_eep_main[0])); i++)
    {
      if(tbl_rgb_eep_main[i].sreg == sreg)
      {
        tbl_ptr = (RGB_DRV_EEP*)&tbl_rgb_eep_main[i];
        break;    
      }
    }    
  }
  
  if(tbl_ptr != NULL)
  {
    if(tbl_ptr->byte_size == 2)
    {
      buffer[0] = (UINT8)((val & 0xFF00) >> 8);
      buffer[1] = (UINT8)(val & 0xFF);          
    }
    else
    {
      buffer[0] = (UINT8)(val & 0xFF);             
    }
    _RGB_EEPWrite(tbl_ptr->offset, buffer, tbl_ptr->byte_size);
  } 
}

#if defined (RGB_ENABLE_LOG)
//-------------------------------------------------------------------------
// _RGB_StatRegDump
//
/**
 *  Log output from SREG
 */
//-------------------------------------------------------------------------
static VOID _RGB_StatRegDump(VOID)
{
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    RGB_LOG("[RGB] rgb_sens_k(%d)\n", rgb_sreg_read(SREG_RGB_SENS_K));
    
    RGB_LOG("[RGB] R(%d) G(%d) B(%d) IR(%d)\n",  pt_this->curr_sensor_r, pt_this->curr_sensor_g, 
                                                 pt_this->curr_sensor_b, pt_this->curr_sensor_ir);
                                                 
    RGB_LOG("[RGB] SUM_R(%d) SUM_G(%d) SUM_B(%d) SUM_IR(%d)\n",  
              pt_this->sum_r, pt_this->sum_g, pt_this->sum_b, pt_this->sum_ir);

    RGB_LOG("[RGB] rgb_sens_r(%d) rgb_sens_g(%d) rgb_sens_b(%d) rgb_sens_ir(%d)\n", 
              rgb_sreg_read(SREG_RGB_SENS_R), rgb_sreg_read(SREG_RGB_SENS_G),
              rgb_sreg_read(SREG_RGB_SENS_B), rgb_sreg_read(SREG_RGB_SENS_IR));
              
    RGB_LOG("[RGB] rgb_sens_plot_x(%d) rgb_sens_plot_y(%d) unfiltered: rgb_sens_y(%d) rgb_sens_t(%d)\n",
              rgb_sreg_read(SREG_RGB_SENS_PLOT_X), rgb_sreg_read(SREG_RGB_SENS_PLOT_Y),
              pt_this->unfiltered_y, pt_this->unfiltered_t);
                   
    RGB_LOG("[RGB] y1(%d), y2(%d), y3(%d), y4(%d), y5(%d), y6(%d), y7(%d), y8(%d)\n",                 
                pt_this->y_history[1], pt_this->y_history[2], pt_this->y_history[3], pt_this->y_history[4],
                pt_this->y_history[5], pt_this->y_history[6], pt_this->y_history[7], pt_this->y_history[8]);
                
    RGB_LOG("[RGB] t1(%d), t2(%d), t3(%d), t4(%d), t5(%d), t6(%d), t7(%d), t8(%d)\n",                 
                pt_this->t_history[1], pt_this->t_history[2], pt_this->t_history[3], pt_this->t_history[4],
                pt_this->t_history[5], pt_this->t_history[6], pt_this->t_history[7], pt_this->t_history[8]);
                
    RGB_LOG("[RGB] gain_r1(%d), gain_r2(%d), gain_r3(%d), gain_r4(%d), gain_r5(%d), gain_r6(%d), gain_r7(%d), gain_r8(%d)\n",                 
                pt_this->gain_r_history[1], pt_this->gain_r_history[2], pt_this->gain_r_history[3], pt_this->gain_r_history[4],
                pt_this->gain_r_history[5], pt_this->gain_r_history[6], pt_this->gain_r_history[7], pt_this->gain_r_history[8]);

    RGB_LOG("[RGB] gain_g1(%d), gain_g2(%d), gain_g3(%d), gain_g4(%d), gain_g5(%d), gain_g6(%d), gain_g7(%d), gain_g8(%d)\n",                 
                pt_this->gain_g_history[1], pt_this->gain_g_history[2], pt_this->gain_g_history[3], pt_this->gain_g_history[4],
                pt_this->gain_g_history[5], pt_this->gain_g_history[6], pt_this->gain_g_history[7], pt_this->gain_g_history[8]);

    RGB_LOG("[RGB] gain_b1(%d), gain_b2(%d), gain_b3(%d), gain_b4(%d), gain_b5(%d), gain_b6(%d), gain_b7(%d), gain_b8(%d)\n",                 
                pt_this->gain_b_history[1], pt_this->gain_b_history[2], pt_this->gain_b_history[3], pt_this->gain_b_history[4],
                pt_this->gain_b_history[5], pt_this->gain_b_history[6], pt_this->gain_b_history[7], pt_this->gain_b_history[8]);
    
    RGB_LOG("[RGB] filtered: rgb_sens_y(%d) rgb_sens_t(%d)\n", 
                 rgb_sreg_read(SREG_RGB_SENS_Y), rgb_sreg_read(SREG_RGB_SENS_T));
                 
    RGB_LOG("[RGB] rgb_sens_gain_r(%d) rgb_sens_gain_g(%d) rgb_sens_gain_b(%d) rgb_sens_on(%d) map(%d)\n", 
                 rgb_sreg_read(SREG_RGB_SENS_GAIN_R), rgb_sreg_read(SREG_RGB_SENS_GAIN_G),
                 rgb_sreg_read(SREG_RGB_SENS_GAIN_B), rgb_sreg_read(SREG_RGB_SENS_ON), pt_this->curr_map);
      
//no need, bl use new module    RGB_LOG("[RGB] opt_read_data(%d)\n", bl_sreg_read(SREG_OPT_READ_DATA));
}
#endif

//-------------------------------------------------------------------------
// _RGB_DetectThread
//
/**
 *  RGB driver detection thread
 */
//-------------------------------------------------------------------------
static VOID _RGB_DetectThread(VOID)
{
    UINT32 is_rgb_sensor = FALSE;

    UINT16 int_time;
    UINT16 manu_time;
    UINT32 wait_time;
    UINT8  t_sum_counter;
#if defined (RGB_ENABLE_LOG)
    UINT32 before_tick;
    UINT32 after_tick;
    UINT32 time_meas;
#endif
    UINT8* buf_ptr = NULL;

    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;

    buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(RGB_EEP_SIZE+2));
    if(buf_ptr != NULL)
    {
      _RGB_EEPCheck(buf_ptr);
      _RGB_InitSreg(buf_ptr);
      sony_x_mem_free(buf_ptr);
      buf_ptr = NULL;
    }
    else
    {
      RGB_LOG("[RGB] mem alloc fail!!!\n");
    }

    vIsRgbSensor(&is_rgb_sensor);
    if(is_rgb_sensor == FALSE){
      RGB_LOG("[RGB] not rgb model, exit thread!!!\n");
      x_thread_exit();
    }    

    x_thread_delay(3); /* confirm RGB sensor is ready */

    if(_RGB_CalculateK() == RGB_NG)
    {
      RGB_LOG("[RGB] rgb sensor communication failure, exit thread!!!\n");
      x_thread_exit();
    }
    
    pt_this->det_on = TRUE;

    while(1)
    {
      if(prev_rgb_color_idx != curr_rgb_color_idx)
      {
        buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(RGB_COLOR_VAR_SIZE+2));
        if(buf_ptr != NULL)
        {
          _RGB_LoadColorVar(curr_rgb_color_idx, buf_ptr);
          prev_rgb_color_idx = curr_rgb_color_idx;
          sony_x_mem_free(buf_ptr);
          buf_ptr = NULL;
        }
        else
        {
          RGB_LOG("[RGB] mem alloc fail!!!\n");
        }
      }
      
      if(s_ui_curr_wbt_idx != s_ui_prev_wbt_idx)
      {
        buf_ptr = sony_x_mem_alloc(sizeof(UINT8)*(RGB_TOTAL_MAP_SIZE+2));
        if(buf_ptr != NULL)
        {
          _RGB_LoadMap(s_ui_curr_wbt_idx, buf_ptr);
          s_ui_prev_wbt_idx = s_ui_curr_wbt_idx;
          sony_x_mem_free(buf_ptr);
          buf_ptr = NULL;
        }
        else
        {
          RGB_LOG("[RGB] mem alloc fail!!!\n");
        }
      }
      
      if(rgb_sreg_read(SREG_RGB_SENS_SUM_CNT) == 0)
      {
        x_thread_delay(10);
        continue;
      }
     
      pt_this->prev_sensor_r = 0;
      pt_this->prev_sensor_g  = 0;
      pt_this->prev_sensor_b  = 0;
      pt_this->prev_sensor_ir = 0;
      pt_this->curr_sensor_r  = 0;
      pt_this->curr_sensor_g  = 0;
      pt_this->curr_sensor_b  = 0;
      pt_this->curr_sensor_ir = 0;
      pt_this->sum_r  = 0;
      pt_this->sum_g  = 0;
      pt_this->sum_b  = 0;
      pt_this->sum_ir = 0;

      t_sum_counter = rgb_sreg_read(SREG_RGB_SENS_SUM_CNT);
      
      int_time  = rgb_sreg_read(SREG_RGB_SENS_INT_TIME);
      manu_time = rgb_sreg_read(SREG_RGB_SENS_MANU_TIME);
	  if ( int_time >= T_INT_CNT)
	  	{
	  		RGB_LOG("[RGB] DetT:intial time beyond the maxtime cnt!!!\n");
	  		int_time = T_INT_CNT-1;
	  	}
      wait_time = ((t_int[int_time] * manu_time * SENSOR_CHANNEL_NUM) / 1000 ) + 1;    
      
      while(t_sum_counter > 0)
      {
        UNUSED(_RGB_WriteControlData());
        
        x_timer_start(pt_this->h_rgbDetTimer,
                      wait_time,
                      X_TIMER_FLAG_ONCE,
                      _RGB_DetectTimeout,
                      NULL);
                             
        WaitSemaphore(pt_this->h_rgbThreadSema);
        
        if(_RGB_StandbyMonitor() == RGB_OK)
        {
          UNUSED(_RGB_ReadSensorData());   
        
          _RGB_DataSum();
        
          t_sum_counter--;  
        } 
      }
      
      rgb_sreg_write(SREG_RGB_SENS_SUM_R,  pt_this->sum_r);
      rgb_sreg_write(SREG_RGB_SENS_SUM_G,  pt_this->sum_g);
      rgb_sreg_write(SREG_RGB_SENS_SUM_B,  pt_this->sum_b);
      rgb_sreg_write(SREG_RGB_SENS_SUM_IR, pt_this->sum_ir);            
          
      if(rgb_sreg_read(SREG_RGB_SENS_IIR_RESET) == 1)
      {
        x_memset(pt_this->y_history, 0, sizeof(UINT16)*(IIR_ORDER+1));
        x_memset(pt_this->t_history, 0, sizeof(UINT16)*(IIR_ORDER+1));
        x_memset(pt_this->gain_r_history, 0, sizeof(UINT16)*(IIR_ORDER+1));
        x_memset(pt_this->gain_g_history, 0, sizeof(UINT16)*(IIR_ORDER+1));
        x_memset(pt_this->gain_b_history, 0, sizeof(UINT16)*(IIR_ORDER+1));
        rgb_sreg_write(SREG_RGB_SENS_IIR_RESET,0);   
      }
#if defined (RGB_ENABLE_LOG)        
      before_tick = x_os_get_sys_tick();
#endif
      /* Begin algorithm calculation */
      {
        _RGB_DataSpreadCorrection();
        _RGB_ColorTempSet();
        _RGB_FilterYT();
        _RGB_RGBGainSet();
        _RGB_BLCtrlCalc();
      }
      /* End algorithm calculation */
#if defined (RGB_ENABLE_LOG)
      after_tick = x_os_get_sys_tick();
      time_meas = (after_tick - before_tick)*x_os_get_sys_tick_period();

      _RGB_StatRegDump();

      RGB_LOG("[RGB] Wait_time(%dms),Calculation time (%dms)\n\n",  wait_time, time_meas);
#endif
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
// _RGB_QueryIRCallback
//
/**
 *  Callback function from IR driver in case IR key is pressed
 */
//-------------------------------------------------------------------------
VOID _RGB_QueryIRCallback(UINT32 u4BtnState,
                          UINT32 u4key,
                          IRRX_RAW_DATA_T* prRawData)
{
  BOOL opt_bt_remote_en;
  RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
  
  opt_bt_remote_en = 0 ;//no need, bl use new module  (BOOL)bl_sreg_read(SREG_OPT_BL_REMOTE_ENABLE);
  
  if((opt_bt_remote_en) && (pt_this->det_on))
  {
    switch(u4BtnState)
    {
      case IRRX_COND_BTN_UP:
        if(pt_this->ir_timer_on == TRUE)
        {
          pt_this->ir_timer_on = FALSE;
          x_timer_stop(pt_this->h_rgbIrContTimer);
          x_timer_stop(pt_this->h_rgbIr1secTimer);          
          x_timer_start(pt_this->h_rgbIr1secTimer,
                  RGB_IR_RESUME_TIME,
                  X_TIMER_FLAG_ONCE,
                  _RGB_IrResumeTimeout,
                  NULL);          
        }
        break;
      case IRRX_COND_BTN_DOWN:
        if(pt_this->ir_timer_on != TRUE)
        {
          pt_this->ir_timer_on = TRUE;
          x_timer_start(pt_this->h_rgbIrContTimer,
                        RGB_IR_CONT_TIME,
                        X_TIMER_FLAG_REPEAT,
                        _RGB_IRTimeout,
                        NULL);          
        }
        break;
      default:
        break; 
    }
  }
}

//-------------------------------------------------------------------------
// RGB_Init
//
/**
 *  Initialize RGB driver
 */
//-------------------------------------------------------------------------
VOID RGB_Init(VOID)
{
    RGB_DRV_MEMBER_T* pt_this = &t_rgb_drv;
    
    x_memset(pt_this, 0, sizeof(RGB_DRV_MEMBER_T));
    
    pt_this->curr_map      = MAP_OPT;
    pt_this->det_on        = FALSE;
    pt_this->ir_continuous = FALSE;
    pt_this->ir_timer_on   = FALSE;
    
    UNUSED(IRRC_RegCallbackFunc(_RGB_QueryIRCallback));
    
    CreateMutex(pt_this->h_rgbSregMutex);
    CreateSemaphore(pt_this->h_rgbThreadSema);
    VERIFY(x_timer_create(&pt_this->h_rgbDetTimer) == OSR_OK);
    VERIFY(x_timer_create(&pt_this->h_rgbIrContTimer) == OSR_OK);
    VERIFY(x_timer_create(&pt_this->h_rgbIr1secTimer) == OSR_OK);
    VERIFY(x_thread_create(&pt_this->h_rgbDrvThread, "RGBHDL", 
           RGB_THREAD_STACK_SIZE, RGB_THREAD_PRIORITY, (x_os_thread_main_fct)_RGB_DetectThread, 0, NULL) == OSR_OK);
}

//-------------------------------------------------------------------------
// RGB_SetRgbUiData
//
/**
 *  Set the WBT_IDX
 */
//-------------------------------------------------------------------------
VOID RGB_SetRgbUiData(UINT8 ui1AmbientSensorOnOff, UINT8 ui1CTIndex)
{
  main_rgb_sreg_write(SREG_RGB_SENS_ON, ui1AmbientSensorOnOff);
  
  if(ui1AmbientSensorOnOff == 0)
  {
    rgb_sreg_write(SREG_RGB_SENS_GAIN_R, 128);
    rgb_sreg_write(SREG_RGB_SENS_GAIN_G, 128);
    rgb_sreg_write(SREG_RGB_SENS_GAIN_B, 128);
  }
  
  switch(ui1CTIndex)
  {
    case 0:
      s_ui_curr_wbt_idx = RGB_WBT_COOL;
      break;
    case 1:
      s_ui_curr_wbt_idx = RGB_WBT_NEUTRAL;
      break;
    case 2:
      s_ui_curr_wbt_idx = RGB_WBT_WARM1;
      break;
    case 3:
      s_ui_curr_wbt_idx = RGB_WBT_WARM2;
      break;
    case 4:
      s_ui_curr_wbt_idx = RGB_WBT_PHOTO;
      break;
    default:
      break;  
  }
}

//-------------------------------------------------------------------------
// RGB_GetOptReadData
//
/**
 *  Get opt_read_data value for opt bl coeff calc
 */
//-------------------------------------------------------------------------
UINT16 RGB_GetOptReadData(void)
{
  UINT16 opt_data;
  
  opt_data = 128; //no need, bl use new module bl_sreg_read(SREG_OPT_READ_DATA);
  
  if( opt_data > 255 )
  {
    return 255; 
  }
  else
  {
    return opt_data;  
  }
}

//-------------------------------------------------------------------------
// RGB_GetRGBGainData
//
/**
 *  Get RGB gain value for WB adjustment
 */
//-------------------------------------------------------------------------
void RGB_GetRGBGainData(RGB_GAIN_DATA* gain_ptr)
{
  gain_ptr->gain_r = (UINT8)rgb_sreg_read(SREG_RGB_SENS_GAIN_R);
  gain_ptr->gain_g = (UINT8)rgb_sreg_read(SREG_RGB_SENS_GAIN_G);
  gain_ptr->gain_b = (UINT8)rgb_sreg_read(SREG_RGB_SENS_GAIN_B);
}

//-------------------------------------------------------------------------
// RGB_SetGaisoSetting
//
/**
 *  set RGB gaiso setting
 */
//-------------------------------------------------------------------------
void RGB_SetGaisouSetting(UINT8 data)
{
  UINT8 buffer;
  
  curr_rgb_color_idx = data;
  buffer = data;
  
  _RGB_EEPWrite(EEP_OFFSET_EMCS_RGB_GAISOU_ECS, &buffer, 1);
}

//-------------------------------------------------------------------------
// RGB_GetGaisoSetting
//
/**
 *  get RGB gaiso setting
 */
//-------------------------------------------------------------------------
UINT8 RGB_GetGaisouSetting(void)
{
  return curr_rgb_color_idx;
}

void main_rgb_sreg_write(UINT16 sreg,UINT16 val)
{
  rgb_sreg_write(sreg,val);

  rgb_sreg_to_eep_write(sreg,val); 
}

UINT16 main_rgb_sreg_read(UINT16 sreg)
{
  return rgb_sreg_read(sreg);
}
