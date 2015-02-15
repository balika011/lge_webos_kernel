
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

#include "eepdef.h"
#include "api_eep.h"
#include "drvcust_if.h"
#include "drv_ycproc.h"
#include "pe_if.h"
#include "eeprom_if.h"
//#include "d_mtk_cust.h"
#include "d_drv_cust.h"

#include "hal_ycproc.h"
#include "../../sony_priv_inc/pq_gamma.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
#include "../../sony_priv_inc/rgb_reg.h"
#include "../../sony_priv_inc/dbl_ctrl.h"
#include "../../sony_drv_inc/wb_pbm.h"
#include "drv_default.h"

#define GAMMA_MAGIC00 0x10
#define GAMMA_MAGIC01 0x20
#define NEW_TCON_MAP

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

//extern void vAging_Bit_Write(UINT16 param);  //temp mark due to build error
extern void DRVCUST_SET_DEFAULT_GAMMA_REMAP(void);
extern void DRVCUST_SET_GAMMA(void);
//extern void DRVCUST_DBC_DEBUG_MODE(UINT8 bDBC_MAGIC, UINT8 bBrightLevel);
#define FUNC_FROM_AP 0

#if FUNC_FROM_AP
#ifndef SYS_MEM_CHK
extern VOID _factory_lock_remote_key (BOOL enable);       //function from ap
#endif
extern VOID SetVidCntVal(INT32 i4_val); //function from ap
extern INT32 GetVidCntVal(VOID); //function from ap
extern INT32 GetVidCntPredefineVal(VOID); //function from ap
extern VOID SetVidBriVal(INT32 i4_val); //function from ap
extern INT32 GetVidBriVal(VOID); //function from ap
extern INT32 GetVidBriPredefineVal(VOID);   //function from ap
extern VOID SetVidPicModeIdx(UINT16 ui2_idx);  //function from ap
extern UINT16 GetVidPicModeIdx(VOID);     //function from ap
extern INT32 a_amb_pause_app (const CHAR* ps_name);  //func from AP
extern void Acfg_update_pic_mode(void);
extern void Acfg_Clr_TemptoAcfg (UINT16 ui2_clr_temp);
extern void Acfg_WBtoAcfg (UINT16 ui2_clr_temp, ST_PARAM_WB_T* WB_ptr);
extern void Acfg_WB_clr_temp_idx_save_reload (UINT16 ui2_save);
#endif
extern void CheckToEnablePBM(void);

static UINT16 ui2_acfg_update_pic_mode=0;
static UINT16 ui2_acfg_update_WB_parm=0;

#ifdef CUST_SOEM_DRV 
extern INT16 GPIO_SetOut(INT16 i4GpioNum, INT16 i4Val);
//static INT32 _i4_org_contrast = 0;
//static INT32 _i4_org_brightness = 0;
//static INT32 _i4_org_pic_mode = 0;
//static INT32 _i4_all_Cnt_Bri[3][2] = {{0,0},{0,0},{0,0}};
#endif

#ifdef CC_SONY_GAMMA
/*-----------------------------------------------------------------------------
	Defines
-----------------------------------------------------------------------------*/

#ifdef PQ_DBG_ON
	#include "../x_inc/x_os.h"
	extern INT32 x_dbg_stmt(const CHAR*  ps_format, ...);
	
	#define PQ_DBG(x...)	x_dbg_stmt(x);
	
	/* debug log with time-stamp */
	#define PQ_DBGT(x...)	pq_GetTimeReference();	\
							x_dbg_stmt(x);

	#ifdef PQ_DBG_GAMMA_ON
	#define PQ_DBG_GAMMA(x...) PQ_DBG(x)
	#else
	#define PQ_DBG_GAMMA(x...)
	#endif

#else
	#define PQ_DBG(x...)
	#define PQ_DBGT(x...)
	#define PQ_DBG_GAMMA(x...)
#endif /* PQ_DBG_ON */
//temp mark due to build error
//#define CUBIC_SPLINE		/* define to use Cubic-spline interpolation */
#ifndef CUBIC_SPLINE
#define PW_LINEAR			/* define to use Piecewise-linear interpolation */
#endif

/* piecewise-linear -> integer/floating point
   cubic-spline     -> floating point only    */
//temp mark due to build error
//#define GAMMA_FLOAT

// temp workaround
//#ifdef GAMMA_FLOAT 
//#undef float
//#endif

/* -------------------------------- */
/*    G A M M A  /  W B   A D J     */
/* -------------------------------- */

#define GAMMA_MIN			0
#define GAMMA_MAX			4095
#define GAMMA_INP_POINTS	257 	/* number of interpolation points */
#define GAMMA_BORDERS		(16+1)	// number of border sections for gamma data (16 x 256 = 4096), +1 for Max level

/* Postproc patgen Y data = [(max - min) * IRE% + 16]*4 */
#define Y_DEFAULT	0x3FF	/* 100IRE */
#define Y_HIGHLIGHT	0x2FD	/* 80IRE -> (235-16)*0.8 + 16)*4        */
#define Y_CUTOFF	0x0EF	/* 20IRE -> (235-16)*0.2 + 16)*4        */

/* Gamma argument bit field defines */
#define BF_GAMMA_SETUP		(b0)
#define BF_GAMMA_SAVE		(b0)
#define BF_GAMMA_QUIT		(b0)
#define BF_GAMMA_EXIT		(b0)
#define BF_GAMMA_LVL		(b3|b2|b1|b0)			/* 0~10   */
#define BF_GAMMA_LVL_DATA	(b11|b10|b9|b8|BYTE_LO)	/* 0~4095 */
#define BF_GAMMA_OFST_DATA	(BYTE_LO)				/* 0~255  */

/* WB argument bit field defines */
#define BF_WB_SETUP			(b0)
#define BF_WB_SAVE			(b0)
#define BF_WB_QUIT			(b0)
#define BF_WB_EXIT			(b0)
#define BF_WB_LVL			(b0)
#define BF_WB_DATA			(b9|b8|BYTE_LO)			/* 0~1023 */
#define BF_WB_COLOR_IDX		(b2|b1|b0)				/* 0~4    */

/* -------------------------------- */
/*      U S E R - W B               */
/* -------------------------------- */

/* scale up the user WB effect */
#define GAIN_FACTOR		4
#define OFST_FACTOR		2

/* gain reg val = user x 4 */
#define GAIN_SCALEUP(x) (((ABS((INT16)(x))) * GAIN_FACTOR) & (b9|b8|BYTE_LO))

/* offset reg val = user x 2 */
/* if user offset is towards -ve, then perform 2's complement conversion after scale up the gain */
#if 0
#define OFST_SCALEUP(x) (((x) & b9) ? (UINT16)(((~((ABS((INT16)x))*OFST_FACTOR))+1) & (b9|b8|BYTE_LO)) 
                                        : (UINT16)(((x)*OFST_FACTOR) & (b9|b8|BYTE_LO)))
#endif

/* color offset range: -512 ~  511 */
/* maps to register:      0 ~ 1023 */
#define OFST_MIN	(-((CT_OFST_MAX/2) + 1))	//-512
#define OFST_MAX	(CT_OFST_MAX/2)				// 511

/* convert 2's complement -> signed integer */
/* padd all 1 if bit9 (offset MSB) is 1 (-ve) */
#define OFST_2COMP(x)	((x & b9) ? ((INT16)(-(~(x |= 0xFC00) + 1))) : ((INT16)(x)))

#define OFST_BOUND(x)	((x > OFST_MAX) ? (OFST_MAX) \
											: ((x < OFST_MIN) ? (OFST_MIN) : (x)))


/* -------------------------------- */
/*      A U T O - W B               */
/* -------------------------------- */

/* Color Gain & Offset1 min/max */
#define CT_GAIN_MIN		0
#define CT_GAIN_MAX		1023
#define CT_OFST_MIN		0
#define CT_OFST_MAX		1023
#define CT_OFST_MASK	0x3FF

#define D_FACTOR		128		/* ambient sensor base divider */
#define X_FACTOR		1000	/* multiply by 1000 to avoid floating point calculation */
#ifdef NEW_TCON_MAP
#define WB_TRACKING1_START_ADDR     0x0300
#define WB_TRACKING2_START_ADDR     0x0320
#define WB_TRACKING3_START_ADDR     0x0380
#else 
#define WB_TRACKING1_START_ADDR     0x100
#define WB_TRACKING2_START_ADDR     0x120
#define WB_TRACKING3_START_ADDR     0x180
#endif
#define SIF_ERROR_RETURN            0
#define BUS_ID                  	1
#define TCON_NVM_SLVADDR            0xAA
#define CLK_DIV                     0x100
#define ADDR_LENGTH                 2

#ifdef CUST_SOEM_DRV

/*-----------------------------------------------------------------------------
	PANEL   EEPROM   Defines
-----------------------------------------------------------------------------*/
//#define EEPROM_PANEL_START 
#define EEPROM_PANEL_MAGIC_OFFSET 0xEE
#define EEPROM_PANEL_PQ_HEADER_OFFSET ( EEPROM_PANEL_MAGIC_OFFSET + 2 )// add MAGIC NUM
#define EEPROM_PANEL_GAMMA_LVL_SIZE_OFFSET (EEPROM_PANEL_PQ_HEADER_OFFSET + NVM_SONY_PQ_HEADER_SIZE)
#define EEPROM_PANEL_WB_OFFSET ( EEPROM_PANEL_GAMMA_LVL_SIZE_OFFSET + NVM_GAMMA_LVL_SIZE )
#define EEPROM_PANEL_WB_SIZE  36

#endif

/*-----------------------------------------------------------------------------
	N V M   Defines
-----------------------------------------------------------------------------*/

/* Sony PQ nvm header */
#define NVM_SONY_PQ_HEADER_OFFSET		EEP_OFFSET_EMCS_PQ_START
#define NVM_SONY_PQ_HEADER_SIZE			5 //4+null
#define NVM_SONY_PQ_HEADER_ID			"PQ1a\0"

/* Gamma adj levels  */
#define NVM_GAMMA_LVL_OFFSET			(NVM_SONY_PQ_HEADER_OFFSET+NVM_SONY_PQ_HEADER_SIZE)
#define NVM_GAMMA_LVL_DATALEN			2
#define NVM_GAMMA_LVL_BLOCK_SIZE		(NVM_GAMMA_LVL_DATALEN*NUM_OF_LUMA_LVL)
#define NVM_GAMMA_LVL_SIZE				(NVM_GAMMA_LVL_BLOCK_SIZE*NUM_OF_GAMMA_BLOCK)

/* Gamma interpolation offset  */
#define NVM_GAMMA_INP_OFST_OFFSET		(NVM_GAMMA_LVL_OFFSET+NVM_GAMMA_LVL_SIZE)
#define NVM_GAMMA_INP_OFST_DATALEN		1
#define NVM_GAMMA_INP_OFST_BLOCK_SIZE	(NVM_GAMMA_INP_OFST_DATALEN*NUM_OF_LUMA_LVL)
#define NVM_GAMMA_INP_OFST_SIZE			(NVM_GAMMA_INP_OFST_BLOCK_SIZE*NUM_OF_GAMMA_BLOCK)

/* White balance adj data */
#define NVM_WB_OFFSET					(NVM_GAMMA_INP_OFST_OFFSET+NVM_GAMMA_INP_OFST_SIZE)
#define NVM_WB_DATALEN					2
#define NVM_WB_BLOCK_SIZE				(NVM_WB_DATALEN*NUM_OF_GAMMA_BLOCK*2) //GAIN & OFFSET1
#define NVM_WB_SIZE						(NVM_WB_BLOCK_SIZE*NUM_OF_WB_INDEX)

/* Gamma interpolation data */
#define NVM_GAMMA_INP_OFFSET			(NVM_WB_OFFSET+NVM_WB_SIZE)
#define NVM_GAMMA_DATA_DATALEN			1
#define NVM_GAMMA_DATA_BLOCK_SIZE		(NVM_GAMMA_DATA_DATALEN*GAMMA_INP_POINTS)
#define NVM_GAMMA_DATA_SIZE				(NVM_GAMMA_DATA_BLOCK_SIZE*NUM_OF_GAMMA_BLOCK)

/* Sony IP NVM */
#define NVM_IP_START_ADDR				0x3AB0
#define NVM_IP_LAST_ADDR				EEP_OFFSET_EMCS_PQ_END

/* Sony Gamma/WB NVM */
#define NVM_GAMMA_START_ADDR			NVM_GAMMA_LVL_OFFSET
#define NVM_GAMMA_LAST_ADDR				(NVM_GAMMA_INP_OFFSET+NVM_GAMMA_DATA_SIZE-1)	// Current used last address
#define NVM_GAMMA_END_ADDR				(NVM_IP_START_ADDR-1)
#define NVM_GAMMA_TOTAL_SIZE			(NVM_IP_START_ADDR-NVM_GAMMA_START_ADDR)
#define NVM_GAMMA_USED_MEMORY			(NVM_GAMMA_LAST_ADDR-NVM_GAMMA_LVL_OFFSET+1)
#define NVM_GAMMA_REMAINING				(NVM_GAMMA_TOTAL_SIZE-NVM_GAMMA_USED_MEMORY)

#define EEP_WB_TRAK_VER_BYTE 			(2)//fatin start
#define EEP_WB_TRAK_VER1				(0x57)
#define EEP_WB_TRAK_VER2				(0x42)//fatin end



/*-----------------------------------------------------------------------------
	Structures
-----------------------------------------------------------------------------*/

#ifdef GAMMA_FLOAT
typedef struct Luma_RGB_F_T
{
	//UINT16 idx; //for debug use only
	float Luma_R, Luma_G, Luma_B;
} Luma_RGB_F_T;
#endif

typedef struct WB_USER_T
{
	UINT16 gain_R, gain_G, gain_B;
	INT16 ofst_R, ofst_G, ofst_B; // 2's complement
} WB_USER_T;

typedef struct WB_AUTOGAIN_T
{
	UINT16 gain_R, gain_G, gain_B;
} WB_AUTOGAIN_T;


typedef struct GAMMA_WB_ECS_T
{
	BOOL 			gamma_setupOnOff;	/* gamma setup state 	*/
	BOOL 			wb_setupOnOff;		/* wb setup state 		*/
	Luma_Lvl_t 		gamma_level; 		/* selected gamma level */
	WB_Lvl_t 		wb_level;			/* selected wb mode 	*/
	WB_Index_t 		wb_color_idx;		/* selected color level */
} GAMMA_WB_ECS_T;


typedef struct AUTO_WB_T
{
	BOOL 			ambientOnOff;		/* ambient sensor status 	*/
	BOOL 			forceOff;			/* force status during adj	*/
	WB_Index_t 		ct_idx;				/* color temperature idx 	*/
	WB_USER_T		user;				/* user wb gain & bias val 	*/
} AUTO_WB_T;


/*-----------------------------------------------------------------------------
	Global variable declarations
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
	Local variable declarations
-----------------------------------------------------------------------------*/

#ifdef PQ_DBG_ON
static UINT32 log_time;
#endif

static struct GAMMA_WB_ECS_T adj_state;						/* adjustment status */
static struct Luma_RGB_T Luma_data[NUM_OF_LUMA_LVL];			/* gamma adjustment data */
static ST_PARAM_WB_T wb_data[NUM_OF_WB_INDEX];				/* wb adjustment data */
static struct Luma_RGB_T curr_lvl;								/* gamma pattern current level */
#ifdef PW_LINEAR
static struct Luma_RGB_T inp_data[GAMMA_INP_POINTS];			/* gamma interpolated data (16bit*257points*3) */
static UINT8 intp_offset[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK];	/* gamma interpolation offset */
#endif

static struct AUTO_WB_T autowb;								/* auto WB status & data */
static UINT16 auto_gain_r, auto_gain_g, auto_gain_b;			/* computed auto color gain */
static UINT16 delta_gain_r, delta_gain_g, delta_gain_b;			/* computed delta color gain */
static UINT16 delta_ofst_r, delta_ofst_g, delta_ofst_b;			/* computed delta color offset */

static ST_PARAM_GAM_PTG gam_param;							/* gamma patgen parameters for callback */
static ST_PARAM_WB_T wb_final;								/* WB final settings for callback */

//static UINT8 *ptr_gamma_bright, *ptr_gamma_middle, *ptr_gamma_dark;
//static UINT16 gamma_size;

/* for design evaluation only */
static BOOL fg_gammaXP;											/* gamma 8->12bit expansion flag, for design use only */
static UINT8 border[GAMMA_BORDERS*NUM_OF_GAMMA_BLOCK];			/* gamma border index for every 8bit overflow, for design use only */
static UINT16 sample_idx;										/* gamma sample index */
static struct Luma_RGB_T sample;								/* gamma sample data */

/* default gamma level data */
static const UINT16 luma_default[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK] = 
{
/*     R     G     B                */
	   0,    0,    0, /* LUMA_LVL_0 */
	 410,  410,  410, /* LUMA_LVL_1 */
	 819,  819,  819, /* LUMA_LVL_2 */
	1229, 1229, 1229, /* LUMA_LVL_3 */
	1638, 1638, 1638, /* LUMA_LVL_4 */
	2048, 2048, 2048, /* LUMA_LVL_5 */
	2457, 2457, 2457, /* LUMA_LVL_6 */
	2867, 2867, 2867, /* LUMA_LVL_7 */
	3276, 3276, 3276, /* LUMA_LVL_8 */
	3686, 3686, 3686, /* LUMA_LVL_9 */
	4095, 4095, 4095  /* LUMA_LVL_10*/
};

/* interpolation constant -> interval between gamma levels */
static const UINT16 intp_interval[NUM_OF_LUMA_LVL] =
{
#if 0
	0  , /* LEVEL 0 (MIN)	*/
	26 , /* LEVEL 1 		*/
	51 , /* LEVEL 2 		*/
	77 , /* LEVEL 3 		*/
	102, /* LEVEL 4 		*/
	128, /* LEVEL 5 		*/
	153, /* LEVEL 6 		*/
	179, /* LEVEL 7 		*/
	204, /* LEVEL 8 		*/
	230, /* LEVEL 9 		*/
	255, /* LEVEL 10 (MAX)	*/
#else
	/* interpolate 257 points to avoid overflow in high IRE area */
	0  , /* LEVEL 0 (MIN)	*/
	26 , /* LEVEL 1 		*/
	51 , /* LEVEL 2 		*/
	77 , /* LEVEL 3 		*/
	102, /* LEVEL 4 		*/
	128, /* LEVEL 5 		*/
	154, /* LEVEL 6 		*/
	179, /* LEVEL 7 		*/
	205, /* LEVEL 8 		*/
	230, /* LEVEL 9 		*/
	256, /* LEVEL 10 (MAX)	*/
#endif
};

#ifdef CUBIC_SPLINE
static const float intp_const[8] =
{
	  10825.62,	// 0
	   2851.28,	// 1
	    772.18,	// 2
	    204.45,	// 3
	     54.88,	// 4
	     14.96,	// 5
	      3.92,	// 6
	1033372.53,	// 7
};
#endif /* CUBIC_SPLINE */

#ifdef PW_LINEAR
/* interpolation offset default data */
static const UINT8 intp_offset_default[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK] =
{
/* LEVEL  0   1   2   3   4   5   6   7   8   9  10 */
		  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* R */
		  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* G */
		  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0   /* B */
};
#endif /* PW_LINEAR */

static const ST_PARAM_WB_T wb_default[NUM_OF_WB_INDEX] = /* WB default data */
{
   /*    Gain       |   Offset1   */
   /* R    G    B     R    G    B */
	{512, 512, 512,   0,   0,   0}, /* 0: Cool    */
	{443, 443, 443,   0,   0,   0}, /* 1: Neutral */
	{406, 406, 406,   0,   0,   0}, /* 2: Warm1   */
	{406, 406, 406,   0,   0,   0}, /* 3: Warm2   */
	{406, 406, 406,   0,   0,   0}, /* 4: Photo   */

	#if 0 /* test data */
   /*    Gain       |   Offset1   */
   /* R    G    B     R    G    B */
	{512, 512, 512,   0,   0,    0}, /* Cool    */
	{530, 512, 436,   1,   0,    1}, /* Neutral */
	{557, 512, 389,   1,   0,    3}, /* Warm1   */
	{577, 512, 298,   1,   0, 1020}, /* Warm2   */
	{530, 512, 436,   1,   0,    1}, /* Photo   */
	#endif
};


/*-----------------------------------------------------------------------------
	Private function prototype
-----------------------------------------------------------------------------*/

/* Pre adjustment setup */
//static void Sony_PreSetup (BOOL bOnOff);

/* Sony PQ nvm verification */
static BOOL Sony_VerifyPQNVM(void);		

/* Gamma */
static void Sony_GammaReadNVM(void);
static void Sony_GammaWriteNVM(void);
//static void Sony_GammaReadRGBNVM (void); //no need
static void Sony_GammaWriteRGBNVM (UINT8 *inp_data_R, UINT8 *inp_data_G, UINT8 *inp_data_B);
static void Sony_GammaInterpolation (void);
static void Sony_GammaLoadTable (void);
static void Sony_GammaBorderSearch (void);
//static void Sony_GammaPatternOn(UINT16 wGammaR, UINT16 wGammaG, UINT16 wGammaB);

/* White balance */
static void Sony_WBSetPatgen (BOOL uWBPatOnOff);
static void Sony_WBReadNVM (void);
static void Sony_WBWriteNVM(void);
static void Sony_WBSetColorHWReg (WB_Index_t color_idx); //cheah_temp
//static void Sony_WBInitDefault (void);



static void _updata_wb_data2EEP_OFFSET(void) //allen eeprom copy to acfg
{
#if FUNC_FROM_AP
  UINT8 i;
  Acfg_WB_clr_temp_idx_save_reload(1);
  for(i=0; i<=NUM_OF_WB_INDEX-1; i++)
  {
    Acfg_WBtoAcfg(i, &wb_data[i]);
  }
  Acfg_WB_clr_temp_idx_save_reload(0);
  Acfg_Clr_TemptoAcfg(adj_state.wb_color_idx);
#endif
  ui2_acfg_update_WB_parm = 1;
}
/* ----------------------------- */
/*      G A M M A    A D J       */
/* ----------------------------- */
//----------------------------------------------------------------------
//  Brief   : Perform pre-gamma/WB adjustment setup
//  @param  : 0-enable OSD/DBL + restore user dimmer
//            1-disable OSD/DBL + set max dimmer
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_PreSetup (BOOL bOnOff)
{
    UINT8 ori_dwBackltMax=0, dwBackltMax=0;
    /* enable/disable osd */
    vGammaOSDControl(&bOnOff);

    if (bOnOff)
    {
        autowb.forceOff = TRUE;						/* disable ambient sensor during pattern on */
        //turn off backlight

        ori_dwBackltMax = PANEL_GetBacklightHigh();
        if (ori_dwBackltMax > PANEL_GetBacklightLow())
        {
            dwBackltMax = ori_dwBackltMax;
        }
        else
        {
            dwBackltMax = 255 - (ori_dwBackltMax - 0);
        } 
        //DRVCUST_DBC_DEBUG_MODE(0x30, dwBackltMax);
    }
    else
    {
        autowb.forceOff = FALSE;
        //DRVCUST_DBC_DEBUG_MODE(0x0, dwBackltMax);
    }
}

#ifdef CUST_SOEM_DRV
//----------------------------------------------------------------------
//  Brief   : Perform pre-gamma/WB adjustment setup
//  @param  : 0-enable OSD/DBL + restore user dimmer
//            1-disable OSD/DBL + set max dimmer
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_Video_PreSetup (UINT16 ui2OnOff)
{
    UINT8 ori_dwBackltMax=0, dwBackltMax=0;
	
    /* enable/disable osd */

    if (ui2OnOff)
    {
        autowb.forceOff = TRUE;						/* disable ambient sensor during pattern on */
        //turn off backlight

        ori_dwBackltMax = PANEL_GetBacklightHigh();
        if (ori_dwBackltMax > PANEL_GetBacklightLow())
        {
            dwBackltMax = ori_dwBackltMax;
        }
        else
        {
            dwBackltMax = 255 - (ori_dwBackltMax - 0);
        } 
        //DRVCUST_DBC_DEBUG_MODE(0x30, dwBackltMax);
    }
    else
    {
        autowb.forceOff = FALSE;
        //DRVCUST_DBC_DEBUG_MODE(0x0, dwBackltMax);
    }
}
#endif

static void _nvm_sync_SonyGamma_to_NPTVGamma(void)
{
    UINT8 buffer[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK*2]={0};
    UINT8 tgt_buffer[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK*2]={0};
    UINT64 u8Offset;
    UINT32 i, j, z;

    //read from sony gamma NVM
    u8Offset = NVM_GAMMA_LVL_OFFSET;
    EEPROM_Read(u8Offset, (UINT32)buffer, (UINT32)(NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK*2));

    //write to tgt_buffer
    u8Offset = 0;
    for (i=0; i<NUM_OF_GAMMA_BLOCK; ++i)
    {
        z=i*2;
        for (j=0; j<NUM_OF_LUMA_LVL; ++j)
        {
            tgt_buffer[u8Offset++] = buffer[z++];
            tgt_buffer[u8Offset++] = buffer[z++];
            z+=4;
        }
    }
    //write to nptv gamma NVM
    u8Offset = EEP_DYN_GAMMA_MAGIC07 + 1;
    EEPNPTV_Write(u8Offset, (UINT32)tgt_buffer, NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK*2);

//    //write to nptv gamma NVM
//    u8Offset = EEP_DYN_GAMMA_MAGIC07 + 1;
//    for (i=0; i<NUM_OF_GAMMA_BLOCK; ++i)
//    {
//        z=i*2;
//        for (j=0; j<NUM_OF_LUMA_LVL; ++j)
//        {
//            fgApiEepromWriteByte(u8Offset++, buffer[z++]);
//            fgApiEepromWriteByte(u8Offset++, buffer[z++]);
//            z+=4;
//        }
//    }

    fgApiEepromWriteByte(EEP_DYN_GAMMA_MAGIC00, GAMMA_MAGIC00);
    fgApiEepromWriteByte(EEP_DYN_GAMMA_MAGIC01, GAMMA_MAGIC01);
}
//----------------------------------------------------------------------
//  Brief   : Init gamma/WB state & fetch adjusted data from nvm.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void WB_Tracking_WriteDefaultToNVMIfNecessary(void)
{
    UINT64 u8Offset;

    if (Sony_VerifyPQNVM() != SV_TRUE)
    {
        //////////////// gamma, write gamma default value to NVM
        u8Offset = NVM_GAMMA_LVL_OFFSET;
        EEPROM_Write(u8Offset, (UINT32)luma_default, (UINT32)NVM_GAMMA_LVL_SIZE);

#ifdef PW_LINEAR
        /* load default offset into NVM */
        u8Offset = NVM_GAMMA_INP_OFST_OFFSET;
        EEPROM_Write(u8Offset, (UINT32)intp_offset_default, (UINT32)NVM_GAMMA_INP_OFST_SIZE);
#endif /* PW_LINEAR */

        ////////////////  wb, i.e. rgb gain/offset
        u8Offset = NVM_WB_OFFSET;
        EEPROM_Write(u8Offset, (UINT32)wb_default, (UINT32)NVM_WB_SIZE);

        ////////////////  sony pq nvm header
        u8Offset = NVM_SONY_PQ_HEADER_OFFSET;
        EEPROM_Write(u8Offset, (UINT32)NVM_SONY_PQ_HEADER_ID, (UINT32)NVM_SONY_PQ_HEADER_SIZE);
    }
}
void DRVCUST_Gamma_Init(void)
{
#ifdef CC_Factory2K12
    _nvm_sync_SonyGamma_to_NPTVGamma();

    if (Sony_VerifyPQNVM() == SV_TRUE)
    {
        Sony_GammaReadNVM();    /* read adjusted gamma level & offset from nvm */
        Sony_WBReadNVM();       /* read adjusted wb gain & offset1 from nvm */
        //Sony_GammaReadRGBNVM(); /* fetch gamma table from nvm */
    }
    else
    {
        x_memcpy(Luma_data, luma_default, sizeof(Luma_data));
        x_memcpy(wb_data, wb_default, sizeof(wb_data));
    }

#endif
}



//----------------------------------------------------------------------
//  Brief   : Register the gamma table addr & size.
//  @param  : pointer to external gamma tables, gamma table size(RGB).
//  @return : none
// 
//----------------------------------------------------------------------
#if 0
void DRVCUST_Gamma_TblReg(UINT8 *ptr_b, UINT8 *ptr_m, UINT8 *ptr_d, UINT16 size)
{
	/* get gamma tbl addr & size */
	ptr_gamma_bright = ptr_b;
	ptr_gamma_middle = ptr_m;
	ptr_gamma_dark   = ptr_d;
	gamma_size       = size/3;
}
#endif

//----------------------------------------------------------------------
//  Brief   : Enter/exit sony gamma adj setup.
//  @param  : ECS -> 0-nothing, 1-enter gamma adj
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaSetup (UINT16 write_data)
{
    if ((write_data & BF_GAMMA_SETUP) == SV_ON)
    {
        if (Sony_VerifyPQNVM() == SV_TRUE)
        {
            Sony_GammaReadNVM();    /* read adjusted gamma level & offset from nvm */
            Sony_WBReadNVM();       /* read adjusted wb gain & offset1 from nvm */
        }
        else
        {
            x_memset(Luma_data, 0, sizeof(Luma_data));
            x_memcpy(wb_data, wb_default, sizeof(wb_data));
    #ifdef PW_LINEAR
            x_memset(intp_offset, 0, sizeof(intp_offset));
    #endif
        }
	
        adj_state.gamma_setupOnOff = SV_ON;

        /* initiate pre adjustment setup */
        Sony_PreSetup(SV_ON);

        /* turn on gamma internal pattern */
        Sony_GammaSetPatgen(SV_ON);
    }
}


//----------------------------------------------------------------------
//  Brief   : Return gamma adj setup state.
//  @param  : none
//  @return : 0: gamma setup state off
//            1: gamma setup state on
// 
//----------------------------------------------------------------------
BOOL Sony_GammaSetup_Read (void)
{
	return adj_state.gamma_setupOnOff;
}


//----------------------------------------------------------------------
//  Brief   : Save all adjusted gamma data into nvm.
//  @param  : ECS -> 0: nothing, 1: save
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaSave (UINT16 write_data)
{
	if (((write_data & BF_GAMMA_SAVE) == SV_TRUE) && (Sony_GammaSetup_Read() == SV_TRUE))
	{
		/* only allow nvm access during pattern on */
		/* write current adjusted gamma level and offset into nvm */
		Sony_GammaWriteNVM();
	}
}


//----------------------------------------------------------------------
//  Brief   : Quit gamma adj without save.
//  @param  : ECS -> 0: nothing, 1: quit
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaQuit (UINT16 write_data)
{
	if (((write_data & BF_GAMMA_QUIT) == SV_TRUE) && (Sony_GammaSetup_Read() == SV_TRUE))
	{
		adj_state.gamma_setupOnOff = SV_OFF;
		adj_state.gamma_level = LUMA_LVL_0; //allen

		/* reverse pre-adjustment setup */
		Sony_PreSetup(SV_OFF);

		/* turn off gamma internal pattern */
		Sony_GammaSetPatgen(SV_OFF);

		/* refresh gamma level & offset data from NVM */
		Sony_GammaReadNVM();
		Sony_GammaLoadTable();		/* load new gamma table */
	}
}


//----------------------------------------------------------------------
//  Brief   : Perform gamma exit with save routine.
//  @param  : ECS -> 0: nothing, 1: exit
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaExit (UINT16 write_data)
{
	if (((write_data & BF_GAMMA_EXIT) == SV_TRUE) && (Sony_GammaSetup_Read() == SV_TRUE))
	{
		/* reset gamma setup & level */
		adj_state.gamma_setupOnOff = SV_OFF;
		adj_state.gamma_level = LUMA_LVL_0;

		/* reverse the pre adjustment setup */
		Sony_PreSetup(SV_OFF);

		/* turn off the gamma pattern */
		Sony_GammaSetPatgen(SV_OFF);

		/* only allow nvm access during pattern on */
		Sony_GammaWriteNVM();		/* write current adjusted gamma level and offset into nvm */
		//Sony_GammaInterpolation();	/* start interpolation and write result into nvm */
		Sony_GammaLoadTable();		/* load new gamma table */
	}
}


//----------------------------------------------------------------------
//  Brief   : Set gamma level according to adj data.
//  @param  : ECS -> selected gamma level (0~10)
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaSelectLvl (UINT16 write_data)
{
	if (Sony_GammaSetup_Read() == SV_OFF)
	{
		return; // ignore if Gamma pattern off
	}

	adj_state.gamma_level = (Luma_Lvl_t)(write_data & BF_GAMMA_LVL);

	if (adj_state.gamma_level > NUM_OF_LUMA_LVL-1)
	{
		adj_state.gamma_level = (Luma_Lvl_t)(NUM_OF_LUMA_LVL-1);
	}
	Sony_GammaPatternRefresh(&(Luma_data[adj_state.gamma_level]));


}


//----------------------------------------------------------------------
//  Brief   : Return current selected gamma level.
//  @param  : none
//  @return : ECS <- gamma level (0~10)
// 
//----------------------------------------------------------------------
Luma_Lvl_t Sony_GammaSelectLvl_Read (void)
{
	return adj_state.gamma_level;
}


//----------------------------------------------------------------------
//  Brief   : Set gamma level data.
//  @param  : ECS -> Gamma:R/G/B, level:luma level, data:0~4095
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaSetLvl (Gamma_RGB_t Gamma, Luma_Lvl_t level, UINT16 write_data)
{
	UINT16 GammaVal;

	if ((Sony_GammaSetup_Read() == SV_OFF)						// Gamma pattern off
	//	|| (level != adj_state.gamma_level) 					// input level not equal to current level
		|| (level > NUM_OF_LUMA_LVL-1))	// invalid input level
	{
		return;
	}

	adj_state.gamma_level = level; /* refresh current level */
	
	GammaVal = write_data & BF_GAMMA_LVL_DATA;

	switch (Gamma)
	{
		case GAMMA_R:
			Luma_data[level].Luma_R = GammaVal;
			break;

		case GAMMA_G:
			Luma_data[level].Luma_G = GammaVal;
			break;

		case GAMMA_B:
			Luma_data[level].Luma_B = GammaVal;
			break;

		default:
			return; /* invalid input */
	}


	/* refresh gamma level with new settings */
	Sony_GammaPatternRefresh(&(Luma_data[level]));


}

//----------------------------------------------------------------------
//  Brief   : Get gamma level data.
//  @param  : ECS -> Gamma:R/G/B, level:luma level
//  @return : ECS <- data:0~4095
// 
//----------------------------------------------------------------------
UINT16 Sony_GammaReadLvl (Gamma_RGB_t Gamma, Luma_Lvl_t level)
{

	if ((level > NUM_OF_LUMA_LVL-1))
	{
		return 0; /* ignore if invalid */
	}

	switch (Gamma)
	{
		case GAMMA_R:
			return (Luma_data[level].Luma_R);

		case GAMMA_G:
			return (Luma_data[level].Luma_G);

		case GAMMA_B:
			return (Luma_data[level].Luma_B);

		default:
			break;
	}
	return 0;
}

//----------------------------------------------------------------------
//  Brief   : Reset gamma level & offset nvm with default data.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaResetNVM (void)
{
	UINT64 u8Offset;

	/* reset gamma setup & level */
	adj_state.gamma_setupOnOff = SV_OFF;
	adj_state.gamma_level = LUMA_LVL_0;

	/* turn off gamma internal pattern */
	Sony_GammaSetPatgen(SV_OFF);

	/* reverse pre-adjustment setup */
	Sony_PreSetup(SV_OFF);

	/* Load default sony pq nvm header */
	EEPROM_Write(NVM_SONY_PQ_HEADER_OFFSET, (UINT32)NVM_SONY_PQ_HEADER_ID, (UINT32)NVM_SONY_PQ_HEADER_SIZE);

	/* load default gamma level into NVM */
	u8Offset = NVM_GAMMA_LVL_OFFSET;
	EEPROM_Write(u8Offset, (UINT32)luma_default, (UINT32)NVM_GAMMA_LVL_SIZE);

	#ifdef PW_LINEAR
	/* load default offset into NVM */
	u8Offset = NVM_GAMMA_INP_OFST_OFFSET;
	EEPROM_Write(u8Offset, (UINT32)intp_offset_default, (UINT32)NVM_GAMMA_INP_OFST_SIZE);
	#endif /* PW_LINEAR */

	Sony_GammaReadNVM();		/* copy new default data into RAM */
	Sony_GammaInterpolation();	/* start interpolation and write result into nvm */
	Sony_GammaLoadTable();		/* load new gamma table */
}


//----------------------------------------------------------------------
//  Brief   : Init gamma data from nvm.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_GammaReadNVM (void)
{
	UINT8 i;
	#ifdef PW_LINEAR	
	UINT8 j;
	#endif
	UINT16 luma_read[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK]={0};
	UINT64 u8Offset;

	/* ------- G A M M A   L E V E L ------------------------------ */

	PQ_DBGT("Initialize gamma levels & offsets\n");

	/* read all gamma level data from nvm */
	u8Offset = NVM_GAMMA_LVL_OFFSET;
	EEPROM_Read(u8Offset, (UINT32)luma_read, (UINT32)NVM_GAMMA_LVL_SIZE);

	PQ_DBG("                    -----[Gamma]----- \n");
	PQ_DBG("                      R     G     B   \n");

	/* initialize gamma level data from nvm */
	for (i=0; i<(UINT8)(NUM_OF_LUMA_LVL); i++)
	{
		Luma_data[i].Luma_R = luma_read[i*NUM_OF_GAMMA_BLOCK];
		Luma_data[i].Luma_G = luma_read[i*NUM_OF_GAMMA_BLOCK+1];
		Luma_data[i].Luma_B = luma_read[i*NUM_OF_GAMMA_BLOCK+2];
	}



	#ifdef PW_LINEAR
	/* ------- G A M M A   O F F S E T ---------------------------- */

	/* read all gamma offset data from nvm */
	u8Offset = NVM_GAMMA_INP_OFST_OFFSET;
	EEPROM_Read(u8Offset, (UINT32)intp_offset, (UINT32)NVM_GAMMA_INP_OFST_SIZE);

	PQ_DBG(" \n -----[Offset R/G/B]----- \n");
	for (i=0; i<(UINT8)NUM_OF_GAMMA_BLOCK; i++)
	{
		for (j=0; j<(UINT8)NUM_OF_LUMA_LVL; j++)
		{
			PQ_DBG("%3d  ",intp_offset[(i*10)+j]);
		}
		PQ_DBG("\n");
	}
	#endif /* PW_LINEAR */
	
}


//----------------------------------------------------------------------
//  Brief   : Write all gamma adj data into nvm. 
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_GammaWriteNVM (void)
{
	UINT8 n;
	UINT16 data[NUM_OF_LUMA_LVL*NUM_OF_GAMMA_BLOCK];
	UINT64 u8Offset = NVM_GAMMA_LVL_OFFSET;

	PQ_DBGT("Write all gamma levels/offsets into NVM\n");

	for (n=0; n<(UINT8)NUM_OF_LUMA_LVL; n++)
	{
		/* store the full range (12bit) adjusted gamma data into NVM */
		data[n*NUM_OF_GAMMA_BLOCK  ] = Luma_data[n].Luma_R;
		data[n*NUM_OF_GAMMA_BLOCK+1] = Luma_data[n].Luma_G;
		data[n*NUM_OF_GAMMA_BLOCK+2] = Luma_data[n].Luma_B;
	}
	/* write all the adjusted gamma data into nvm */
	EEPROM_Write(u8Offset, (UINT32)data, (UINT32)NVM_GAMMA_LVL_SIZE);

	#ifdef PW_LINEAR
	/* write all the gamma offsets into nvm */
	u8Offset = NVM_GAMMA_INP_OFST_OFFSET;
	EEPROM_Write(u8Offset, (UINT32)intp_offset, (UINT32)NVM_GAMMA_INP_OFST_SIZE);
	#endif /* PW_LINEAR */
}


//----------------------------------------------------------------------
//  Brief   : Read gamma table from nvm.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
#if 0
// no need
static void Sony_GammaReadRGBNVM (void)
{
	UINT8 inp_data_R[GAMMA_INP_POINTS];
	UINT8 inp_data_G[GAMMA_INP_POINTS];
	UINT8 inp_data_B[GAMMA_INP_POINTS];
	UINT16 n;
	UINT64 u8Offset;

	PQ_DBGT("read interpolated gamma data from NVM -> [START]\n");

	/* read all interpolation data from NVM */
	u8Offset = NVM_GAMMA_INP_OFFSET;
	EEPROM_Read(u8Offset, (UINT32)inp_data_R, (UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);
	u8Offset += NVM_GAMMA_DATA_BLOCK_SIZE;
	EEPROM_Read(u8Offset, (UINT32)inp_data_G, (UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);
	u8Offset += NVM_GAMMA_DATA_BLOCK_SIZE;
	EEPROM_Read(u8Offset, (UINT32)inp_data_B, (UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);

	/* copy all interpolation data (8bit) to RAM  */

	/* fix all 1st point to 0 */
	*ptr_gamma_middle                  = 0;
	*(ptr_gamma_middle + gamma_size*1) = 0;
	*(ptr_gamma_middle + gamma_size*2) = 0;

	//for (n=1; n<GAMMA_INP_POINTS; n++)
	for (n=1; n<GAMMA_INP_POINTS-1; n++)
	{
		/* populate MTK gamma array with sony gamma table (LSB 8bit) */
		/* apply 8->12 bit expansion during gamma table loading process */
		*(ptr_gamma_middle + n)                = inp_data_R[n];
		*(ptr_gamma_middle + gamma_size*1 + n) = inp_data_G[n];
		*(ptr_gamma_middle + gamma_size*2 + n) = inp_data_B[n];
	}

	/* copy to all tables, to avoid being overwritten during gamma remap (for ATSC spec only) */
	x_memcpy(ptr_gamma_bright, ptr_gamma_middle, gamma_size*3);
	x_memcpy(ptr_gamma_dark  , ptr_gamma_middle, gamma_size*3);

	PQ_DBGT("read interpolated gamma data from NVM -> [END]\n");
}
#endif

//----------------------------------------------------------------------
//  Brief   : Write the interpolation data into nvm.
//  @param  : RGB interpolated results (8bit)
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_GammaWriteRGBNVM (UINT8 *inp_data_R, UINT8 *inp_data_G, UINT8 *inp_data_B)
{
	UINT64 u8Offset;

	PQ_DBGT("save gamma interpolation data into NVM ->[START]\n");

	/* write all interpolation data into NVM */
	u8Offset = NVM_GAMMA_INP_OFFSET;
	EEPROM_Write(u8Offset, (UINT32)inp_data_R, (UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);
	u8Offset += NVM_GAMMA_DATA_BLOCK_SIZE;
	EEPROM_Write(u8Offset, (UINT32)inp_data_G, (UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);
	u8Offset += NVM_GAMMA_DATA_BLOCK_SIZE;
	EEPROM_Write(u8Offset, (UINT32)inp_data_B, (UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);

	PQ_DBGT("save gamma interpolation data into NVM ->[END]\n");
}


//----------------------------------------------------------------------
//  Brief   : Initiate gamma interpolation + write data into nvm.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_GammaInterpolation (void)
{
#ifdef CUBIC_SPLINE
    UINT8 n, h[10];
    UINT16 i, p;
    UINT16 x1, x2, x3;
	
    struct Luma_RGB_T *luma_data_ptr;
    UINT16 luma[NUM_OF_GAMMA_BLOCK][NUM_OF_LUMA_LVL];
	
    Gamma_RGB_t rgb;
    float coeff_x[NUM_OF_GAMMA_BLOCK][8]; 
    float coeff_y[NUM_OF_GAMMA_BLOCK][11];
    float coeff_a[NUM_OF_GAMMA_BLOCK][10];
    float coeff_b[NUM_OF_GAMMA_BLOCK][10];
    float coeff_c[NUM_OF_GAMMA_BLOCK][10];
    float coeff_d[NUM_OF_GAMMA_BLOCK][10];
    float inp_data_f[NUM_OF_GAMMA_BLOCK][GAMMA_INP_POINTS];
    UINT8 inp_data_R[GAMMA_INP_POINTS];
    UINT8 inp_data_G[GAMMA_INP_POINTS];
    UINT8 inp_data_B[GAMMA_INP_POINTS];

    PQ_DBGT("Gamma interpolation -> [START]\n");

    /* copy adjusted luma data into individual arrays */
    luma_data_ptr = Luma_data;
    for(n=0; n<NUM_OF_LUMA_LVL; n++)
    {
        luma[GAMMA_R][n] = luma_data_ptr->Luma_R;
        luma[GAMMA_G][n] = luma_data_ptr->Luma_G;
        luma[GAMMA_B][n] = luma_data_ptr->Luma_B;
        luma_data_ptr++;
    }

    /* compute interval h */
    for(n=0; n<10; n++)
    {
        h[n] = intp_interval[n+1] - intp_interval[n];
        PQ_DBG_GAMMA("h[%d] = %d\n", n, h[n]);
    }

    /* interpolate for each rgb gamma */
    for(rgb=GAMMA_R; rgb<NUM_OF_GAMMA_BLOCK; rgb++)
    {
        PQ_DBG_GAMMA("---[ GAMMA %d ]---\n", rgb);

        /* init first inp point to 0 */
        inp_data_f[rgb][0] = 0;

        /*------ Compute coefficient_x ------*/
        for(n=0; n<8; n++)
        {
            coeff_x[rgb][n] = ((float)(luma[rgb][n+2] - luma[rgb][n+1])/(float)h[n+1]) \
							- ((float)(luma[rgb][n+1] - luma[rgb][n])/(float)h[n]);								

            PQ_DBG_GAMMA("coeff_x[%d][%d] = %f\n", rgb, n, coeff_x[rgb][n]);
        }

        /*------ Compute coefficient_y ------*/

        /* Natural boundary condition ->  min & max = 0 */
        coeff_y[rgb][0]  = 0; 
        coeff_y[rgb][9]  = 0;
        coeff_y[rgb][10] = 0; // dummy data, for last point cal in coeff_a & coeff_c.
        coeff_y[rgb][1] = (coeff_x[rgb][7] + intp_const[0]*coeff_x[rgb][0] - intp_const[1]*coeff_x[rgb][1]				\
						+ intp_const[2]*coeff_x[rgb][2] - intp_const[3]*coeff_x[rgb][3] + intp_const[4]*coeff_x[rgb][4] \
						- intp_const[5]*coeff_x[rgb][5] + intp_const[6]*coeff_x[rgb][6]) / intp_const[7];

        for(n=2; n<=8; n++)
        {
            coeff_y[rgb][n] = (coeff_x[rgb][n-2] - (float)h[n-2]*coeff_y[rgb][n-2] \
							- (float)(2*(h[n-2]+h[n-1]))*coeff_y[rgb][n-1]) / (float)h[n-1];
        }
		
        for(n=0; n<10; n++)
        {
            PQ_DBG_GAMMA("coeff_y[%d][%d] = %f\n", rgb, n, coeff_y[rgb][n]);
        }

        /* start 10-segment interpolation */
        for(n=0; n<10; n++)
        {
            /*------ Compute coefficient_a~d ------*/
            coeff_a[rgb][n] = (coeff_y[rgb][n+1] - coeff_y[rgb][n]) / (float)(6*h[n]);
            coeff_b[rgb][n] = coeff_y[rgb][n]/2;
            coeff_c[rgb][n] = ((float)(luma[rgb][n+1] - luma[rgb][n]) / (float)h[n]) 	\
								- (coeff_y[rgb][n+1]*(float)h[n] / 6)					\
								- (coeff_y[rgb][n]*(float)h[n] / 3);	
            /* base level = lower bound of adjusted luma level */
            coeff_d[rgb][n] = luma[rgb][n];

            PQ_DBG_GAMMA("coeff_a[%d][%d] = %f\n", rgb, n, coeff_a[rgb][n]);
            PQ_DBG_GAMMA("coeff_b[%d][%d] = %f\n", rgb, n, coeff_b[rgb][n]);
            PQ_DBG_GAMMA("coeff_c[%d][%d] = %f\n", rgb, n, coeff_c[rgb][n]);
            PQ_DBG_GAMMA("coeff_d[%d][%d] = %f\n", rgb, n, coeff_d[rgb][n]);

            /* interpolate between the lower & upper bound of each segment */
            for(i=intp_interval[n]+1; i<intp_interval[n+1]; i++)
            {
                x1 = i-intp_interval[n];
                x2 = x1*x1;
                x3 = x1*x1*x1;
                if (i < GAMMA_INP_POINTS)  //boundary check
                {
                    inp_data_f[rgb][i] = BOUND(coeff_a[rgb][n]*(float)x3 + coeff_b[rgb][n]*(float)x2 \
    											+ coeff_c[rgb][n]*(float)x1 + coeff_d[rgb][n], GAMMA_MIN, GAMMA_MAX);	
                    PQ_DBG_GAMMA("inp_data_f[%d][%d] = %f\n", rgb, i, inp_data_f[rgb][i]);
                }
            }				

            /* last point of each segment = upper bound of adjusted luma level */
            inp_data_f[rgb][intp_interval[n+1]] = BOUND_MAX(luma[rgb][n+1], GAMMA_MAX);
            PQ_DBG_GAMMA("inp_data_f[%d][%d] = %f\n", rgb, i, inp_data_f[rgb][i]);

        }
		
        PQ_DBG_GAMMA("----------------------\n");
    }

    PQ_DBG_GAMMA("Round up to the nearest integer.\n"); 
    PQ_DBG_GAMMA("	 R       G       B  \n");

    /* populate the results into arrays */
    for(p=0; p<GAMMA_INP_POINTS; p++)
    {
        /* round up to the nearest integer and truncate to 8bit */
        /* Store only the LSB-8bit in NVM, will expand to 12bit during gamma table loading process */
        inp_data_R[p] = (UINT8)(inp_data_f[GAMMA_R][p] + 0.5);
        inp_data_G[p] = (UINT8)(inp_data_f[GAMMA_G][p] + 0.5);
        inp_data_B[p] = (UINT8)(inp_data_f[GAMMA_B][p] + 0.5);
		
        PQ_DBG_GAMMA("[%3d] %5d   %5d   %5d \n"	,p	
												,(UINT16)(inp_data_f[GAMMA_R][p] + 0.5)	\
												,(UINT16)(inp_data_f[GAMMA_G][p] + 0.5)	\
												,(UINT16)(inp_data_f[GAMMA_B][p] + 0.5));
    }
	 	
    /* write all interpolation data into NVM */
    Sony_GammaWriteRGBNVM(inp_data_R, inp_data_G, inp_data_B);


#else

    /* Piecewise linear interpolation */
    UINT16 n, istart, iend;

    #ifdef GAMMA_FLOAT
    float slope_R, slope_G, slope_B;
    struct Luma_RGB_F_T inp_data_f[GAMMA_INP_POINTS];	/* stores gamma interpolated data in float */
    struct Luma_RGB_F_T *inp_data_f_ptr;
    #else
    UINT16 slope_R, slope_G, slope_B;
    #endif /* GAMMA_FLOAT */

    Luma_Lvl_t lvl;
    struct Luma_RGB_T *inp_data_ptr, *luma_data_ptr;
    
    UINT8 inp_data_R[GAMMA_INP_POINTS];
    UINT8 inp_data_G[GAMMA_INP_POINTS];
    UINT8 inp_data_B[GAMMA_INP_POINTS];

    PQ_DBGT("Gamma interpolation -> [START]\n");

    luma_data_ptr = Luma_data;		// init ptr to adjusted luma level
    luma_data_ptr++;				// skip luma level 0
        
    inp_data_ptr = inp_data;		// init ptr to store interpolation results
    inp_data_ptr->Luma_R = 0;		// init first inp point to 0
    inp_data_ptr->Luma_G = 0;
    inp_data_ptr->Luma_B = 0;
    //inp_data_ptr->idx = 0;		// for debug
    inp_data_ptr++;
        
    #ifdef GAMMA_FLOAT
    inp_data_f_ptr = inp_data_f;	// init ptr to store interpolation results in float
    inp_data_f_ptr->Luma_R = 0;		// init first inp point to 0
    inp_data_f_ptr->Luma_G = 0;
    inp_data_f_ptr->Luma_B = 0;
    //inp_data_f_ptr->idx = 0;		// for debug
    inp_data_f_ptr++;
    #endif /* GAMMA_FLOAT */

    for (lvl=LUMA_LVL_1; lvl<NUM_OF_LUMA_LVL; lvl++)
    {
        PQ_DBG_GAMMA("<level %d>\n", (UINT8)lvl);
    
        #ifdef GAMMA_FLOAT
        /* calculate slope */
        slope_R = (float)(luma_data_ptr->Luma_R - (luma_data_ptr-1)->Luma_R);
        slope_R = slope_R + (float)intp_offset[(GAMMA_R*NUM_OF_LUMA_LVL)+lvl];
        slope_R = slope_R / (float)(intp_interval[lvl] - intp_interval[lvl-1]);

        slope_G = (float)(luma_data_ptr->Luma_G - (luma_data_ptr-1)->Luma_G);
        slope_G = slope_G + (float)intp_offset[(GAMMA_G*NUM_OF_LUMA_LVL)+lvl];
        slope_G = slope_G / (float)(intp_interval[lvl] - intp_interval[lvl-1]);

        slope_B = (float)(luma_data_ptr->Luma_B - (luma_data_ptr-1)->Luma_B);
        slope_B = slope_B + (float)intp_offset[(GAMMA_B*NUM_OF_LUMA_LVL)+lvl];
        slope_B = slope_B / (float)(intp_interval[lvl] - intp_interval[lvl-1]);

        PQ_DBG_GAMMA("slope = R%2f G%2f B%2f\n", slope_R, slope_G, slope_B);

        #else
        /* calculate slope */
        slope_R = luma_data_ptr->Luma_R - (luma_data_ptr-1)->Luma_R;
        slope_R = slope_R + (UINT16)intp_offset[(GAMMA_R*NUM_OF_LUMA_LVL)+lvl];
        slope_R = slope_R / (intp_interval[lvl] - intp_interval[lvl-1]);

        slope_G = luma_data_ptr->Luma_G - (luma_data_ptr-1)->Luma_G;
        slope_G = slope_G + (UINT16)intp_offset[(GAMMA_G*NUM_OF_LUMA_LVL)+lvl];
        slope_G = slope_G / (intp_interval[lvl] - intp_interval[lvl-1]);

        slope_B = luma_data_ptr->Luma_B - (luma_data_ptr-1)->Luma_B;
        slope_B = slope_B + (UINT16)intp_offset[(GAMMA_B*NUM_OF_LUMA_LVL)+lvl];
        slope_B = slope_B / (intp_interval[lvl] - intp_interval[lvl-1]);

        PQ_DBG_GAMMA("slope = R%2d G%2d B%2d\n", slope_R, slope_G, slope_B);
        #endif /* GAMMA_FLOAT */

        /* get the starting and ending interval point */
        istart = intp_interval[lvl-1]+1;
        iend = intp_interval[lvl];
        PQ_DBG_GAMMA("start %d --> end %d \n", istart, iend);
        

        #ifdef GAMMA_FLOAT
        /* 1st starting point of each level should be started from the previous adj luma level */
        inp_data_f_ptr->Luma_R = BOUND_MAX((float)((luma_data_ptr-1)->Luma_R) + slope_R, GAMMA_MAX);
        inp_data_f_ptr->Luma_G = BOUND_MAX((float)((luma_data_ptr-1)->Luma_G) + slope_G, GAMMA_MAX);
        inp_data_f_ptr->Luma_B = BOUND_MAX((float)((luma_data_ptr-1)->Luma_B) + slope_B, GAMMA_MAX);
        //inp_data_f_ptr->idx = istart; // for debug

        /* round up to the nearest integer */
        inp_data_ptr->Luma_R = BOUND_MAX((UINT16)(inp_data_f_ptr->Luma_R + 0.5), GAMMA_MAX);
        inp_data_ptr->Luma_G = BOUND_MAX((UINT16)(inp_data_f_ptr->Luma_G + 0.5), GAMMA_MAX);
        inp_data_ptr->Luma_B = BOUND_MAX((UINT16)(inp_data_f_ptr->Luma_B + 0.5), GAMMA_MAX);
        //inp_data_ptr->idx = istart; // for debug

        PQ_DBG_GAMMA("		  R    G	B\n");
        PQ_DBG_GAMMA("%3d : %4d %4d %4d\n", istart, 				\
											inp_data_ptr->Luma_R,	\
											inp_data_ptr->Luma_G,	\
											inp_data_ptr->Luma_B);

        PQ_DBG_GAMMA("        R    G    B\n");
        PQ_DBG_GAMMA("%3d : %4f %4f %4f\n", istart,					\
											inp_data_f_ptr->Luma_R,	\
											inp_data_f_ptr->Luma_G,	\
											inp_data_f_ptr->Luma_B);

        inp_data_ptr++;
        inp_data_f_ptr++;
        #else

        /* 1st starting point of each level should be started from the previous adj luma level */
        inp_data_ptr->Luma_R = (luma_data_ptr-1)->Luma_R + slope_R;
        inp_data_ptr->Luma_G = (luma_data_ptr-1)->Luma_G + slope_G;
        inp_data_ptr->Luma_B = (luma_data_ptr-1)->Luma_B + slope_B;
        //inp_data_ptr->idx = istart; // for debug

        /* to avoid overflow when (luma n) < (luma n-1) */
        if (inp_data_ptr->Luma_R > 4095) inp_data_ptr->Luma_R = 4095;
        if (inp_data_ptr->Luma_G > 4095) inp_data_ptr->Luma_G = 4095;
        if (inp_data_ptr->Luma_B > 4095) inp_data_ptr->Luma_B = 4095;

        PQ_DBG_GAMMA("        R    G    B\n");
        PQ_DBG_GAMMA("%3d : %4d %4d %4d\n", istart,					\
											inp_data_ptr->Luma_R,	\
											inp_data_ptr->Luma_G,	\
											inp_data_ptr->Luma_B);

        inp_data_ptr++;
        #endif /* GAMMA_FLOAT */

        /* interpolation between the starting & ending point of each levels */
        for (n=istart+1; n<iend; n++)
        {

            #ifdef GAMMA_FLOAT
            inp_data_f_ptr->Luma_R = BOUND_MAX((inp_data_f_ptr-1)->Luma_R + slope_R, GAMMA_MAX);
            inp_data_f_ptr->Luma_G = BOUND_MAX((inp_data_f_ptr-1)->Luma_G + slope_G, GAMMA_MAX);
            inp_data_f_ptr->Luma_B = BOUND_MAX((inp_data_f_ptr-1)->Luma_B + slope_B, GAMMA_MAX);
            //inp_data_f_ptr->idx = n; // for debug

            /* round up to the nearest integer */
            inp_data_ptr->Luma_R = BOUND_MAX((UINT16)(inp_data_f_ptr->Luma_R + 0.5), GAMMA_MAX);
            inp_data_ptr->Luma_G = BOUND_MAX((UINT16)(inp_data_f_ptr->Luma_G + 0.5), GAMMA_MAX);
            inp_data_ptr->Luma_B = BOUND_MAX((UINT16)(inp_data_f_ptr->Luma_B + 0.5), GAMMA_MAX);
            //inp_data_ptr->idx = n; // for debug

            PQ_DBG_GAMMA("%3d : %4d %4d %4d\n", n, 						\
												inp_data_ptr->Luma_R,	\
												inp_data_ptr->Luma_G,	\
												inp_data_ptr->Luma_B);

            PQ_DBG_GAMMA("%3d : %4f %4f %4f\n", n,						\
												inp_data_f_ptr->Luma_R,	\
												inp_data_f_ptr->Luma_G,	\
												inp_data_f_ptr->Luma_B);

            inp_data_ptr++;
            inp_data_f_ptr++;

            #else
            inp_data_ptr->Luma_R = (inp_data_ptr-1)->Luma_R + slope_R;
            inp_data_ptr->Luma_G = (inp_data_ptr-1)->Luma_G + slope_G;
            inp_data_ptr->Luma_B = (inp_data_ptr-1)->Luma_B + slope_B;
            //inp_data_ptr->idx = n; // for debug

            if (inp_data_ptr->Luma_R > 4095) inp_data_ptr->Luma_R = 4095;
            if (inp_data_ptr->Luma_G > 4095) inp_data_ptr->Luma_G = 4095;
            if (inp_data_ptr->Luma_B > 4095) inp_data_ptr->Luma_B = 4095;

            PQ_DBG_GAMMA("%3d : %4d %4d %4d\n", n, 						\
												inp_data_ptr->Luma_R,	\
												inp_data_ptr->Luma_G,	\
												inp_data_ptr->Luma_B);

            inp_data_ptr++;
            #endif /* GAMMA_FLOAT */

        }
		
        #ifdef GAMMA_FLOAT
        inp_data_ptr->Luma_R = luma_data_ptr->Luma_R;
        inp_data_ptr->Luma_G = luma_data_ptr->Luma_G;
        inp_data_ptr->Luma_B = luma_data_ptr->Luma_B;
        //inp_data_ptr->idx = iend; // for debug

        /* last point of each level has to be the same as the current adj luma level */
        inp_data_f_ptr->Luma_R = (float)(luma_data_ptr->Luma_R);
        inp_data_f_ptr->Luma_G = (float)(luma_data_ptr->Luma_G);
        inp_data_f_ptr->Luma_B = (float)(luma_data_ptr->Luma_B);
        //inp_data_f_ptr->idx = iend; // for debug

        PQ_DBG_GAMMA("%3d : %4d %4d %4d\n", n,						\
											inp_data_ptr->Luma_R,	\
											inp_data_ptr->Luma_G,	\
											inp_data_ptr->Luma_B);

        PQ_DBG_GAMMA("%3d : %4f %4f %4f\n", n,						\
											inp_data_f_ptr->Luma_R,	\
											inp_data_f_ptr->Luma_G,	\
											inp_data_f_ptr->Luma_B);


        inp_data_ptr++;
        inp_data_f_ptr++;

        #else
        /* last point of each level has to be the same as the current adj luma level */
        inp_data_ptr->Luma_R = luma_data_ptr->Luma_R;
        inp_data_ptr->Luma_G = luma_data_ptr->Luma_G;
        inp_data_ptr->Luma_B = luma_data_ptr->Luma_B;
        //inp_data_ptr->idx = iend; // for debug

        PQ_DBG_GAMMA("%3d : %4d %4d %4d\n", n,						\
											inp_data_ptr->Luma_R,	\
											inp_data_ptr->Luma_G,	\
											inp_data_ptr->Luma_B);

        inp_data_ptr++;
        #endif /* GAMMA_FLOAT */

        /* proceed to next level */
        luma_data_ptr++;
    }

    /* point to the interpolation data (12bit) */
    inp_data_ptr = inp_data;

    /* populate the results into arrays */
    for (n=0; n<GAMMA_INP_POINTS; n++)
    {
        /* Store only the LSB-8bit in NVM, will expand to 12bit during gamma table loading process */
        inp_data_R[n] = (UINT8)(inp_data_ptr->Luma_R);
        inp_data_G[n] = (UINT8)(inp_data_ptr->Luma_G);
        inp_data_B[n] = (UINT8)(inp_data_ptr->Luma_B);
        inp_data_ptr++;
    }
	 	
    /* write all interpolation data into NVM */
    Sony_GammaWriteRGBNVM(inp_data_R, inp_data_G, inp_data_B);

#endif /* CUBIC_SPLINE */

    PQ_DBGT("Gamma interpolation -> [END]\n");
}


//----------------------------------------------------------------------
//  Brief   : Load gamma nvm into MTK.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_GammaLoadTable (void)
{
    CheckToEnablePBM();
	DRVCUST_SET_GAMMA();
	/* fetch interpolation data from nvm */
	//Sony_GammaReadRGBNVM();

	/* load into mtk reg to interpolate 257-> 1024 points */
	//vGammaLoadTable((void*)0);

	// gamma top points are set after "vDrvGammaLoadTable()" 
	//vDrvGammaSetTopPoint(4095, 4095, 4095);

	PQ_DBGT("Gamma table loaded \n");
}


//----------------------------------------------------------------------
//  Brief   : Controls the gamma internal generator
//  @param  : 0:turn off pattern
//            1:turn on pattern
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaSetPatgen (BOOL uGammaPatOnOff)
{
	if (uGammaPatOnOff > 1)
	{
		return;	/* invalid input */
	}

	gam_param.onOff = uGammaPatOnOff;

	if (gam_param.onOff == SV_OFF)
	{
		adj_state.gamma_level = LUMA_LVL_0;

		sample_idx = 0;
		sample.Luma_R = 0;
		sample.Luma_G = 0;
		sample.Luma_B = 0;
		
		/* turn OFF gamma patgen */
		vGammaSetPattern(&gam_param);
	}
	else
	{
		/* always force to max during 1st turn on */
		adj_state.gamma_level = LUMA_LVL_10;
	}	
	
	Sony_GammaPatternRefresh(&(Luma_data[adj_state.gamma_level]));
}


//----------------------------------------------------------------------
//  Brief   : Determine border idx for full range 12bit gamma data
//  @param  : none
//  @return : none
//  Remarks : for design evaluation only
//----------------------------------------------------------------------
static void Sony_GammaBorderSearch (void)
{
    UINT8 gam, i;
    UINT8 inp_data[GAMMA_INP_POINTS]={0};
    UINT16 idx;

    /* read interpolated gamma data & locate all the borders of 8->12bit expansion result */
    for(gam=0; gam<NUM_OF_GAMMA_BLOCK; gam++)
    {
        EEPROM_Read(NVM_GAMMA_INP_OFFSET+(NVM_GAMMA_DATA_BLOCK_SIZE*gam), \
					(UINT32)inp_data, \
					(UINT32)NVM_GAMMA_DATA_BLOCK_SIZE);	
        PQ_DBG("[GAMMA-%d] from 0x%x ~ 0x%x\n", gam, \
											NVM_GAMMA_INP_OFFSET+(NVM_GAMMA_DATA_BLOCK_SIZE*gam), \
											NVM_GAMMA_INP_OFFSET+(NVM_GAMMA_DATA_BLOCK_SIZE*gam)+NVM_GAMMA_DATA_BLOCK_SIZE-1);

        if (GAMMA_BORDERS*(gam+1) < GAMMA_BORDERS*NUM_OF_GAMMA_BLOCK)  //boundary check
        {
            border[GAMMA_BORDERS*(gam+1)]   = 0;	// 1st element of each block is always 0
            border[GAMMA_BORDERS*(gam+1)-1] = 255;	// last element of each block is always 255
            i = 1; 
            for(idx=1; idx<GAMMA_INP_POINTS; idx++)
            {
                if (inp_data[idx] < inp_data[idx-1])
                {
                    /* save the border idx for each 255 (8bit overflow) border increment */
                    if (i+GAMMA_BORDERS*gam < GAMMA_BORDERS*NUM_OF_GAMMA_BLOCK)  //boundary check
                    {
                        border[i+GAMMA_BORDERS*gam] = idx;
                        PQ_DBG("[GAMMA] border[%d] = %d\n", i+GAMMA_BORDERS*gam, idx);
                    }
                    i++;
                }
            }
        }
    }
}



//----------------------------------------------------------------------
//  Brief   : Set gamma pattern level by adjustment data index
//  @param  : ECS -> sampling point index (0~256)
//  @return : none
//  Remarks : for design evaluation only
//----------------------------------------------------------------------
void Sony_GammaSetSample (UINT16 u2idx)
{
    UINT8 i;
    Gamma_RGB_t gamma;
    UINT8 nvm_R=0, nvm_G=0, nvm_B=0;	
    UINT64 u8Offset;

    if (u2idx > (GAMMA_INP_POINTS-1))
    {
        return;	/* invalid input */
    }
	
    if(!fg_gammaXP) /* call for once only */
    {
        /* to determine all the border points of gamma 12bit data */
        Sony_GammaBorderSearch();
        fg_gammaXP = SV_TRUE;
    }

    if (Sony_GammaSetup_Read() == SV_FALSE) /* set only when pattern is off, to avoid flashing */
    {
        /* To replicate gamma pattern ON condition, 
			the sample gamma level will be overwrite in VISR */
        Sony_GammaSetup((UINT16)SV_ON); 
    }

    /* read the sampled interpolation RGB data from NVM */
    sample_idx = u2idx;
    u8Offset = NVM_GAMMA_INP_OFFSET + sample_idx;
    PQ_DBG("gamma nvm addr R/G/B = 0x%x,  ", (UINT32)u8Offset);
    EEPROM_Read(u8Offset, (UINT32)&nvm_R, (UINT32)NVM_GAMMA_DATA_DATALEN);
    u8Offset += NVM_GAMMA_DATA_BLOCK_SIZE;
    PQ_DBG("0x%x,  ", (UINT32)u8Offset);
    EEPROM_Read(u8Offset, (UINT32)&nvm_G, (UINT32)NVM_GAMMA_DATA_DATALEN);
    u8Offset += NVM_GAMMA_DATA_BLOCK_SIZE;
    PQ_DBG("0x%x\n", (UINT32)u8Offset);
    EEPROM_Read(u8Offset, (UINT32)&nvm_B, (UINT32)NVM_GAMMA_DATA_DATALEN);

    if (sample_idx == GAMMA_INP_POINTS-1)
    {
        /* last point */
        sample.Luma_R = BOUND_MAX((((UINT16)nvm_R+1)*(GAMMA_BORDERS-1)-1), GAMMA_MAX);
        sample.Luma_G = BOUND_MAX((((UINT16)nvm_G+1)*(GAMMA_BORDERS-1)-1), GAMMA_MAX);
        sample.Luma_B = BOUND_MAX((((UINT16)nvm_B+1)*(GAMMA_BORDERS-1)-1), GAMMA_MAX);
    }
    else /* 0 <= sample_idx <= 255 */
    {
        /* expand to 12bit gamma data */
        for(gamma=GAMMA_R; gamma<=GAMMA_B; gamma++)
        {
            for(i=0; i<GAMMA_BORDERS; i++)
            {
                if ((i+gamma*GAMMA_BORDERS)+1 < GAMMA_BORDERS*NUM_OF_GAMMA_BLOCK
                    && (sample_idx >= border[i+gamma*GAMMA_BORDERS]) 
                    && (sample_idx <= border[(i+gamma*GAMMA_BORDERS)+1]))
                {
                    switch (gamma)
                    {
                        case GAMMA_R:
                            sample.Luma_R = BOUND_MAX((UINT16)nvm_R+(UINT16)i*(GAMMA_INP_POINTS-1), GAMMA_MAX);
                            break;
                        case GAMMA_G:	
                            sample.Luma_G = BOUND_MAX((UINT16)nvm_G+(UINT16)i*(GAMMA_INP_POINTS-1), GAMMA_MAX);
                            break;
                        case GAMMA_B:
                            sample.Luma_B = BOUND_MAX((UINT16)nvm_B+(UINT16)i*(GAMMA_INP_POINTS-1), GAMMA_MAX);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    PQ_DBG("sample.gamma[%d] R/G/B = %d  %d  %d \n", sample_idx, sample.Luma_R, sample.Luma_G, sample.Luma_B);
    /* refresh gamma pattern parameters, to be update during next VISR */
    Sony_GammaPatternRefresh(&sample);

}


//----------------------------------------------------------------------
//  Brief   : Returns current sampled index (0~256)
//  @param  : none
//  @return : ECS <- current sampled index
//  Remarks : for design evaluation only
//----------------------------------------------------------------------
UINT16 Sony_GammaGetSampleIndex (void)
{
	return sample_idx;
}



//----------------------------------------------------------------------
//  Brief   : Returns current sampled gamma data (0~256)
//  @param  : none
//  @return : ECS <- sampled gamma data
//  Remarks : for design evaluation only
//----------------------------------------------------------------------
UINT16 Sony_GammaGetSample (Gamma_RGB_t Gamma)
{
	switch (Gamma)
	{
		case GAMMA_R:	return (sample.Luma_R);
		case GAMMA_G:	return (sample.Luma_G);
		case GAMMA_B:	return (sample.Luma_B);
		default:		return 0;
	}
}

//----------------------------------------------------------------------
//  Brief   : Refresh gamma pattern level for gamma pattern gen.
//  @param  : RGB gamma level.
//  @return : none
// 
//----------------------------------------------------------------------
Luma_RGB_T Sony_GammaPatternRGB_Read (void)
{
    Luma_RGB_T Current_GAMMA_PTN;

	Current_GAMMA_PTN.Luma_R = curr_lvl.Luma_R;
	Current_GAMMA_PTN.Luma_G = curr_lvl.Luma_G;
	Current_GAMMA_PTN.Luma_B = curr_lvl.Luma_B;

    return Current_GAMMA_PTN;

	/* gamma pattern is refresh during VISR */
}
//----------------------------------------------------------------------
//  Brief   : Refresh gamma pattern level for gamma pattern gen.
//  @param  : RGB gamma level.
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_GammaPatternRefresh (Luma_RGB_T* ptr)
{
	curr_lvl.Luma_R = ptr->Luma_R;
	curr_lvl.Luma_G = ptr->Luma_G;
	curr_lvl.Luma_B = ptr->Luma_B;
	/* gamma pattern is refresh during VISR */
}


//----------------------------------------------------------------------
//  Brief   : Refresh gamma pattern level during VISR.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void DRVCUST_Gamma_ISR(void)
{
    static UINT32 _counter = 0;
    if ((gam_param.wGammaR != curr_lvl.Luma_R)
		|| (gam_param.wGammaG != curr_lvl.Luma_G) 
		|| (gam_param.wGammaB != curr_lvl.Luma_B))
    {
        /* refersh gamma pattern */
        gam_param.wGammaR = curr_lvl.Luma_R;
        gam_param.wGammaG = curr_lvl.Luma_G;
        gam_param.wGammaB = curr_lvl.Luma_B;

        /* pass to callback function */
        vGammaSetPattern(&gam_param);
    }
    else if (gam_param.onOff)
    {
        if(++_counter >= 10)
        {
            vGammaSetPattern(&gam_param);
            _counter = 0;
        }
    }
}


/* ------------------------------------------- */
/*      W H I T E - B A L A N C E   A D J      */
/* ------------------------------------------- */
void Sony_WB_Acfg_Update_Set (UINT16 write_data)
{
    switch(write_data)
    {
        case 0:
			ui2_acfg_update_pic_mode = 0;
			ui2_acfg_update_WB_parm = 0;
			break;
		case 1:
			ui2_acfg_update_pic_mode = 1;
			ui2_acfg_update_WB_parm = 0;			
			break;
		case 2:
			ui2_acfg_update_pic_mode = 0;
			ui2_acfg_update_WB_parm = 1;			
			break;
		case 3:
			ui2_acfg_update_pic_mode = 1;
			ui2_acfg_update_WB_parm = 1;			
			break;
		default:
			break;
    }
}
UINT16 Sony_WB_Acfg_Update_Get (void)
{
    UINT16 ui2Acfgupdate=0;

    switch(ui2_acfg_update_pic_mode)
    {
        case 0:
			if(ui2_acfg_update_WB_parm == 0) {
				ui2Acfgupdate = 0;
			} else {
			    ui2Acfgupdate = 2;
			}
			break;
		case 1:
			if(ui2_acfg_update_WB_parm == 0) {
				ui2Acfgupdate = 1;
			} else {
			    ui2Acfgupdate = 3;
			}
			break;
		default:
			break;
    }

	return ui2Acfgupdate;
}
WB_CLR_TEMP_DATA_T Sony_WB_Update_PARM_Get (UINT16 WB_CLR_DATA_IDX)
{
    //UINT8 i;
	WB_CLR_TEMP_DATA_T wb_data_temp={0};
    //ST_PARAM_WB_T wb_data_temp[NUM_OF_WB_INDEX];	
    //x_memcpy(wb_data_temp, wb_data, sizeof(wb_data));

    if(WB_CLR_DATA_IDX > NUM_OF_WB_INDEX)
    {
        return wb_data_temp;
    }
	
    wb_data_temp.wRGain = wb_data[WB_CLR_DATA_IDX].wGainR;
	wb_data_temp.wGGain = wb_data[WB_CLR_DATA_IDX].wGainG;
	wb_data_temp.wBGain = wb_data[WB_CLR_DATA_IDX].wGainB;
	wb_data_temp.wROfst = wb_data[WB_CLR_DATA_IDX].wOfstR;
	wb_data_temp.wGOfst = wb_data[WB_CLR_DATA_IDX].wOfstG;
	wb_data_temp.wBOfst = wb_data[WB_CLR_DATA_IDX].wOfstB;

	return wb_data_temp;
}
UINT16 Sony_WB_Update_ADJ_CLR_IDX_Get (void)
{
	return (UINT16)(adj_state.wb_color_idx);;
}

//----------------------------------------------------------------------
//  Brief   : Enter/exit WB adj setup.
//  @param  : ECS -> 0:nothing, 1: enter WB adj
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBSetup (UINT16 write_data)
{
	  UINT16 u2_wb_method=0;
	
#if FUNC_FROM_AP
    vAging_Bit_Write(1);  //function from ap
#endif
    AgingLastPowerControlState_Set(1);
    #ifndef SYS_MEM_CHK
#if FUNC_FROM_AP
    _factory_lock_remote_key(TRUE);   //function from ap
#endif
    #endif
    //UINT16 u2_wb_method = WB_PBM_WBAdjustMethodGet();
    u2_wb_method = WB_PBM_WBAdjustMethodGet();
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"Sony_WBSetup(%d %d)\n",write_data, u2_wb_method));    
    if( u2_wb_method == 0)
    {
    	if ((write_data & BF_WB_SETUP) == SV_TRUE)
    	{
    		adj_state.wb_setupOnOff = SV_ON;

                Sony_WBReadNVM();       /* read adjusted wb gain & offset1 from nvm */
    		/* initiate pre adjustment setup */
    		Sony_PreSetup(SV_ON);
		Sony_WBSetColorHWReg(adj_state.wb_color_idx);  //allen	

    		/* turn on the post-process pattern */
    		Sony_WBSetPatgen(SV_ON);
                 Sony_WBSetLvl(0);	 //allen mode setup default level 0
    	}
    }
    else
    {
        Sony_PreSetup(SV_ON);
        
        DRVCUST_SET_DEFAULT_GAMMA_REMAP();
        DRVCUST_SET_GAMMA();
        x_thread_delay(100);

        WB_PBM_Init(u2_wb_method);
    }
}
//----------------------------------------------------------------------
//  Brief   : Returns current WB adj setup state.
//  @param  : none
//  @return : ECS <- 0:WB adj state inactive, 1:WB adj active
// 
//----------------------------------------------------------------------
BOOL Sony_WBSetup_Read (void)
{
	return adj_state.wb_setupOnOff;
}


//----------------------------------------------------------------------
//  Brief   : Save all WB data into nvm.
//  @param  : ECS -> 0:nothing, 1:save
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBSave (UINT16 write_data)
{
	if (((write_data & BF_WB_SAVE) == SV_TRUE) && (Sony_WBSetup_Read() == SV_TRUE))
	{
		/* only allow nvm access during pattern on */
		/* write current adjusted gamma level and offset into nvm */
		Sony_WBWriteNVM();
	}
}


//----------------------------------------------------------------------
//  Brief   : Quit WB adj without save.
//  @param  : ECS -> 0: nothing, 1: quit
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBQuit (UINT16 write_data)
{
	  UINT16 u2_wb_method=0;
	
    //vAging_Bit_Write(0);               //remove, due to spec
    AgingLastPowerControlState_Set(0);
    #ifndef SYS_MEM_CHK
#if FUNC_FROM_AP
    _factory_lock_remote_key(FALSE);  //function from ap
#endif
    #endif
    //UINT16 u2_wb_method = WB_PBM_WBAdjustMethodGet();
    u2_wb_method = WB_PBM_WBAdjustMethodGet();
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- Sony_WBQuit(0x%x) %d --\n",write_data, u2_wb_method));    
    if(u2_wb_method == 0)
    {
    	if (((write_data & BF_WB_QUIT) == SV_TRUE) && (Sony_WBSetup_Read() == SV_TRUE))
    	{
    		/* reverse pre adjustment setup */
    		Sony_PreSetup(SV_OFF);

    		/* turn off the post-process pattern */
    		Sony_WBSetPatgen(SV_OFF);

    		/* refresh WB gain & offset data from NVM */
    		Sony_WBReadNVM();
					
            CheckToEnablePBM();

	        ui2_acfg_update_pic_mode=1;
			
#if FUNC_FROM_AP
                Acfg_update_pic_mode();   //call ap directly
#endif
    	}
    }
    else
    {
        Sony_PreSetup(SV_OFF);
        WB_PBM_DeInit(write_data);
    }
    //AgingShowPattern(1); //move to AP layer 
}


//----------------------------------------------------------------------
//  Brief   : Exit from WB adj + save all WB data into nvm.
//  @param  : ECS -> 0:nothing, 1:exit
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBExit (UINT16 write_data)
{
	  UINT16 u2_wb_method=0;
	
    //vAging_Bit_Write(0);               //remove, due to spec
    AgingLastPowerControlState_Set(0);
    #ifndef SYS_MEM_CHK
#if FUNC_FROM_AP
    _factory_lock_remote_key(FALSE);  //function from ap
#endif
    #endif
    //UINT16 u2_wb_method = WB_PBM_WBAdjustMethodGet();
    u2_wb_method = WB_PBM_WBAdjustMethodGet();
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"-- Sony_WBExit(0x%x) --\n",write_data));    
    if( u2_wb_method == 0)
    {
        if (((write_data & BF_WB_EXIT) == SV_TRUE) && (Sony_WBSetup_Read() == SV_TRUE))
        {
            /* only allow nvm access during pattern on */
            /* write current adjusted gamma level and offset into nvm */
            Sony_WBWriteNVM();
        
            /* reverse the pre-adjustment process */
            Sony_PreSetup(SV_OFF);
        
            /* turn off the post process pattern */
            Sony_WBSetPatgen(SV_OFF);

            CheckToEnablePBM();

			ui2_acfg_update_pic_mode = 1;
            
#if FUNC_FROM_AP
            Acfg_update_pic_mode();   //call ap directly
#endif
        }
    }
    else
    {
        Sony_PreSetup(SV_OFF);
        WB_PBM_DeInit(u2_wb_method);
    }
}


//----------------------------------------------------------------------
//  Brief   : Set different IRE for Post process generator
//  @param  : ECS -> 0:Highlight, 1:Cutoff
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBSetLvl (UINT16 write_data)
{
	adj_state.wb_level = (WB_Lvl_t)((write_data & BF_WB_LVL));

	if (adj_state.wb_setupOnOff == SV_ON)
	{
		/* only refresh during pattern on */
		Sony_WBSetPatgen(SV_ON);
	}
}


//----------------------------------------------------------------------
//  Brief   : Return current selected WB level
//  @param  : none
//  @return : ECS <- wb_level:0~1
// 
//----------------------------------------------------------------------
WB_Lvl_t Sony_WBReadLvl (void)
{
	return adj_state.wb_level;
}


//----------------------------------------------------------------------
//  Brief   : Set color temperature registers from ECS.
//  @param  : ECS -> data:0~4
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBSetColor (UINT16 write_data)
{
	adj_state.wb_color_idx = (WB_Index_t)(write_data & BF_WB_COLOR_IDX);

	if (adj_state.wb_color_idx >= NUM_OF_WB_INDEX)
	{
		/* color index range check */
		PQ_DBG("invalid color index \n");
	}
	else
	{
		Sony_WBSetColorHWReg(adj_state.wb_color_idx);
	}
}


//----------------------------------------------------------------------
//  Brief   : Return current selected color temperature index in ECS
//  @param  : none
//  @return : ECS <- wb_color_idx:0~4
// 
//----------------------------------------------------------------------
WB_Index_t Sony_WBReadColor (void)
{
	return adj_state.wb_color_idx;
}


//----------------------------------------------------------------------
//  Brief   : Set white balance data into nvm.
//  @param  : ECS -> color_idx:0~4, wb_lvl:0~1, gamma:R/G/B, data
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBSetData (WB_Index_t color_idx, WB_Lvl_t wb_lvl, Gamma_RGB_t gamma, UINT16 write_data)
{
	UINT16 wb_val;
	if ((Sony_WBSetup_Read() == SV_FALSE)			// WB pattern is off
//allen		|| (color_idx != adj_state.wb_color_idx) 	// input color not equal to current selected color idx  
		|| (color_idx >= NUM_OF_WB_INDEX))			// invalid color index input
	{
		return;
	}

	wb_val = write_data & BF_WB_DATA;

	switch (wb_lvl)
	{
		/* Gain */
		case WB_HIGHLIGHT:
			switch (gamma) /* RGB selection */
			{
				case GAMMA_R:	wb_data[color_idx].wGainR = wb_val;	break;
				case GAMMA_G:	wb_data[color_idx].wGainG = wb_val;	break;
				case GAMMA_B:	wb_data[color_idx].wGainB = wb_val;	break;		
				default:		return;	/* invalid input */
			}
			break;
				
		/* Offset 1 */
		case WB_CUTOFF:
			switch (gamma) /* RGB selection */
			{
				case GAMMA_R:	wb_data[color_idx].wOfstR = wb_val;	break;
				case GAMMA_G:	wb_data[color_idx].wOfstG = wb_val;	break;
				case GAMMA_B:	wb_data[color_idx].wOfstB = wb_val;	break;		
				default:		return;	/* invalid input */
			}
			break;

		default:
			/* invalid input */
			return;	
	}

	/* to be trigger from sreg (app call back + d interface) */
	/* refresh WB gain & offset1 settings */
	//Sony_WBSetColorHWReg(color_idx);
}


//----------------------------------------------------------------------
//  Brief   : Fetch white balance data from nvm.
//  @param  : ECS -> color_idx:0~4, wb_lvl:0~1, gamma:R/G/B
//  @return : ECS <- WB gain/offset
// 
//----------------------------------------------------------------------
UINT16 Sony_WBReadData (WB_Index_t color_idx, WB_Lvl_t wb_lvl, Gamma_RGB_t gamma)
{
	if(color_idx >= NUM_OF_WB_INDEX)
	{
		return 1;
	}
	switch (wb_lvl)
	{
		case WB_HIGHLIGHT:	/* Gain */
			switch (gamma)
			{
				case GAMMA_R:	return (wb_data[color_idx].wGainR);
				case GAMMA_G:	return (wb_data[color_idx].wGainG);
				case GAMMA_B:	return (wb_data[color_idx].wGainB);
				default:		break;	/* invalid input */
			}
			break;	
		case WB_CUTOFF:	/* Offset 1 */
			switch (gamma)
			{
				case GAMMA_R:	return (wb_data[color_idx].wOfstR);
				case GAMMA_G:	return (wb_data[color_idx].wOfstG);
				case GAMMA_B:	return (wb_data[color_idx].wOfstB);
				default:		break;	/* invalid input */
			}
			break;

		default:
			/* invalid input */
			break;	
	}


	return 0;
}

//----------------------------------------------------------------------
//  Brief   : Reset white balance nvm with default data.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_WBResetNVM (void)
{
	UINT64 u8Offset = NVM_WB_OFFSET;

	PQ_DBG("Load default WB gain & offset1 into NVM \n");

	/* load default gamma level & offset data into NVM */
	EEPROM_Write(u8Offset, (UINT32)wb_default, (UINT32)NVM_WB_SIZE);

	/* copy new default data into RAM */
	Sony_WBReadNVM();
}


//----------------------------------------------------------------------
//  Brief   : init white balance data from nvm.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_WBReadNVM (void)
{
	UINT8 i;
	UINT16 wb_data_nvm[NUM_OF_WB_INDEX*NUM_OF_WB_ITEMS]={0};
	UINT64 u8Offset;


	PQ_DBGT("Initialize WB gain & offset1 from NVM\n");

	/* read all WB data from nvm */
	u8Offset = EEP_WB_MODE_GAIN_START;
	EEPROM_Read(u8Offset, (UINT32)wb_data_nvm, (UINT32)EEP_WB_MODE_GAIN_OFST_SIZE);

	PQ_DBG("                -----[Gain]-----    ----[Offset1]----\n");
	PQ_DBG("                  R     G     B       R     G     B  \n");

	/* initialize WB data from nvm */
	for (i=0; i<(UINT8)NUM_OF_WB_INDEX; i++)
	{
		wb_data[i].wGainR = wb_data_nvm[i*NUM_OF_WB_ITEMS  ];
		wb_data[i].wGainG = wb_data_nvm[i*NUM_OF_WB_ITEMS+1];
		wb_data[i].wGainB = wb_data_nvm[i*NUM_OF_WB_ITEMS+2];
		wb_data[i].wOfstR = wb_data_nvm[i*NUM_OF_WB_ITEMS+3];
		wb_data[i].wOfstG = wb_data_nvm[i*NUM_OF_WB_ITEMS+4];
		wb_data[i].wOfstB = wb_data_nvm[i*NUM_OF_WB_ITEMS+5];

		PQ_DBG("WB level[%2d] -> %4d  %4d  %4d  %4d  %4d  %4d \n", i,				\
                                                                           wb_data[i].wGainR,   \
                                                                           wb_data[i].wGainG,	\
                                                                           wb_data[i].wGainB,	\
                                                                           wb_data[i].wOfstR,	\
                                                                           wb_data[i].wOfstG,	\
                                                                           wb_data[i].wOfstB);
	}
}


//----------------------------------------------------------------------
//  Brief   : Write adjusted white balance data into nvm.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_WBWriteNVM (void)
{
	UINT64 u8Offset = NVM_WB_OFFSET;
	UINT8 Trak_verbyte[EEP_WB_TRAK_VER_BYTE];
	
	PQ_DBG("Save WB gain & offset1 into NVM -> [START]\n");
	/* write all the adjusted WB data into nvm */
	EEPROM_Write(u8Offset, (UINT32)wb_data, (UINT32)NVM_WB_SIZE);

        _updata_wb_data2EEP_OFFSET(); //allen eeprom copy to acfg

	PQ_DBG("Save WB gain & offset1 into NVM -> [END]\n");
	Trak_verbyte[0]=EEP_WB_TRAK_VER1;
	Trak_verbyte[1]=EEP_WB_TRAK_VER2;
	EEPROM_Write(0x39a7,(UINT32)&Trak_verbyte,2);
}


//----------------------------------------------------------------------
//  Brief   : Controls the post-process internal generator
//  @param  : 0:turn off pattern
//            1:turn on pattern
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_WBSetPatgen (BOOL uWBPatOnOff)
{
    ST_PARAM_SCE_PTG sce_params;

    x_memset(&sce_params, 0, sizeof(ST_PARAM_SCE_PTG));

	if (uWBPatOnOff > 1)
	{
		return;	/* invalid input */
	}

	sce_params.onOff = uWBPatOnOff;

	if (sce_params.onOff == TRUE)
	{
		sce_params.sceProc = SV_OFF;  /* Bypass all post processing */

		switch (adj_state.wb_level)
		{
			case WB_HIGHLIGHT:
				sce_params.data = Y_HIGHLIGHT;
				break;

			case WB_CUTOFF:
				sce_params.data = Y_CUTOFF;
				break;

			default:
				PQ_DBG("invalid mode, H->Highlight(gain), C->Cutoff(Offset1) \n");
				return;
		}
	}
	else
	{
		/* reset wb setup parameters when pattern off */
		adj_state.wb_setupOnOff = SV_OFF;
		adj_state.wb_level = WB_HIGHLIGHT;
		adj_state.wb_color_idx = WB_NEUTRAL;
	
		sce_params.sceProc = SV_ON;  /* Enable all post processing */
		sce_params.data    = Y_DEFAULT;
	}

	/* pass to callback function */
	vGammaSetSCEPattern(&sce_params);
}



//----------------------------------------------------------------------
//  Brief   : temp, to be remove
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
static void Sony_WBSetColorHWReg (WB_Index_t color_idx)
{
	if(color_idx >= NUM_OF_WB_INDEX)
	{
		return;
	}
	/* pass to callback */
	vWBSetGainOfst(&wb_data[color_idx]);

	PQ_DBG("color index[%d] loaded\n", adj_state.wb_color_idx);
}
/* >>> START cheah, fetch WB data from Sony (08-Sep-09) */
//----------------------------------------------------------------------
//  Brief   : fetch WB nvm data
//  @param  : color idx, mode & rgb
//  @return : WB nvm data
// 
//----------------------------------------------------------------------
UINT16 DRVCUST_GetSonyRGB (UINT8 bCTIndex, UINT8 bmode, UINT8 bRGB)
{
	bCTIndex = BOUND_MAX(bCTIndex,RGB_PHOTO);
	bmode    = BOUND_MAX(bmode,RGB_CUTOFF);
	bRGB     = BOUND_MAX(bRGB,RGB_BLUE);
	if (bmode == RGB_HIGHLIGHT)
	{
		switch(bRGB)
		{
			case RGB_RED:	return (wb_data[bCTIndex].wGainR);
			case RGB_GREEN:	return (wb_data[bCTIndex].wGainG);
			case RGB_BLUE:	return (wb_data[bCTIndex].wGainB);
			default: 		break;
		}
	}
	else /* WB_CUTOFF */
	{
		switch(bRGB)
		{
			case RGB_RED:	return (wb_data[bCTIndex].wOfstR);
			case RGB_GREEN:	return (wb_data[bCTIndex].wOfstG);
			case RGB_BLUE:	return (wb_data[bCTIndex].wOfstB);
			default: 		break;
		}
	}

	return(0);
}
/* <<< END cheah, fetch WB data from Sony (08-Sep-09) */


/* -------------------------------- */
/*      A U T O - W B               */
/* -------------------------------- */
//----------------------------------------------------------------------
//  Brief   : init local AutoWB struct
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void DRVCUST_AutoWB_Init(void)
{
	/* init CT/WB related UI parameters */
	autowb.ambientOnOff = SV_OFF;
	autowb.forceOff = FALSE;
	autowb.ct_idx = WB_COOL;
	autowb.user.gain_R = 0;
	autowb.user.gain_G = 0;
	autowb.user.gain_B = 0;
	autowb.user.ofst_R = 0;
	autowb.user.ofst_G = 0;
	autowb.user.ofst_B = 0;

	/* init computed delta color gain */
	delta_gain_r = 0;
	delta_gain_g = 0;
	delta_gain_b = 0;

	/* init computed delta color offset */
	delta_ofst_r = 0;
	delta_ofst_g = 0;
	delta_ofst_b = 0;

	/* init computed auto color gain */
	auto_gain_r = 0;
	auto_gain_g = 0;
	auto_gain_b = 0;

	/* init final gain & offset */
	wb_final.wGainR = 0;
	wb_final.wGainG = 0;
	wb_final.wGainB = 0;
	wb_final.wOfstR = 0;
	wb_final.wOfstG = 0;
	wb_final.wOfstB = 0;
}


//----------------------------------------------------------------------
//  Brief   : Update local AutoWB struct 
//  @param  : color temperature UI struct from app
//  @return : none
// 
//----------------------------------------------------------------------
void DRVCUST_AutoWB_Update(AUTO_COLORTEMP_SETS_INFO_T *aCTInfo)
{
	ST_PARAM_WB_T wb;
	ST_PARAM_WB_T* wb_ptr;

	if (autowb.ct_idx >= NUM_OF_WB_INDEX)
	{
		autowb.ct_idx = (WB_Index_t)(NUM_OF_WB_INDEX-1);
	}

	/* refresh all CT/WB related UI parameters */
	autowb.ambientOnOff	= (BOOL)aCTInfo->ui1AmbientSensorOnOff;
	autowb.ct_idx		= (WB_Index_t)aCTInfo->ui1CTIndex;

	/* user gain range: -40~0 */
	autowb.user.gain_R	= GAIN_SCALEUP(aCTInfo->ui1Bias_RGain);
	autowb.user.gain_G	= GAIN_SCALEUP(aCTInfo->ui1Bias_GGain);
	autowb.user.gain_B	= GAIN_SCALEUP(aCTInfo->ui1Bias_BGain);

	/* user offset range: -20~20 */
	/* offset reg val = user value x 2 */
	//autowb.user.ofst_R = OFST_SCALEUP(aCTInfo->ui1Bias_ROffset);
	//autowb.user.ofst_G = OFST_SCALEUP(aCTInfo->ui1Bias_GOffset);
	//autowb.user.ofst_B = OFST_SCALEUP(aCTInfo->ui1Bias_BOffset);
	autowb.user.ofst_R	= (INT16)aCTInfo->ui1Bias_ROffset * OFST_FACTOR;
	autowb.user.ofst_G	= (INT16)aCTInfo->ui1Bias_GOffset * OFST_FACTOR;
	autowb.user.ofst_B	= (INT16)aCTInfo->ui1Bias_BOffset * OFST_FACTOR;

	/* C O L O R   T E M P  +  U S E R   W B */

	/* get the base CT gain & offset */
	wb_ptr = wb_data;
	wb_ptr += autowb.ct_idx;
	x_memcpy(&wb, wb_ptr, sizeof(wb));

	/* re-compute the delta gain/offset on any CT/WB UI event */
	delta_gain_r = BOUND(wb.wGainR - autowb.user.gain_R, CT_GAIN_MIN, CT_GAIN_MAX);
	delta_gain_g = BOUND(wb.wGainG - autowb.user.gain_G, CT_GAIN_MIN, CT_GAIN_MAX);
	delta_gain_b = BOUND(wb.wGainB - autowb.user.gain_B, CT_GAIN_MIN, CT_GAIN_MAX);
								
	/* 2's complement representation -> (-512~0~511) */
	delta_ofst_r = (OFST_BOUND(OFST_2COMP(wb.wOfstR) + autowb.user.ofst_R)) & CT_OFST_MASK;
	delta_ofst_g = (OFST_BOUND(OFST_2COMP(wb.wOfstG) + autowb.user.ofst_G)) & CT_OFST_MASK;
	delta_ofst_b = (OFST_BOUND(OFST_2COMP(wb.wOfstB) + autowb.user.ofst_B)) & CT_OFST_MASK;

	PQ_DBG("-------------\n");	
	PQ_DBG("delta gain -> ");	
	PQ_DBG("R=%d   G=%d   B=%d \n", delta_gain_r, delta_gain_g, delta_gain_b); 
	PQ_DBG("delta offset -> ");	
	PQ_DBG("R=%d   G=%d   B=%d \n", delta_ofst_r, delta_ofst_g, delta_ofst_b); 

	/* match the ECS color idx to UI CT */
	adj_state.wb_color_idx = autowb.ct_idx;

}


//----------------------------------------------------------------------
//  Brief   : Auto white-balance process (every video mainloop)
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void DRVCUST_AutoWB_Proc(void)
{
	UINT16 i2val;
	RGB_GAIN_DATA rgb_sens_ptr;

	/* A U T O   W B */

	/* fetch latest rgb sensor readings (128~255) */
	RGB_GetRGBGainData(&rgb_sens_ptr);

	if (autowb.ambientOnOff == SV_OFF || autowb.forceOff == TRUE)
	{
		/* force unity gain if ambient sensor off */
		rgb_sens_ptr.gain_r = D_FACTOR;
		rgb_sens_ptr.gain_g = D_FACTOR;
		rgb_sens_ptr.gain_b = D_FACTOR;
	}

	/* multiply the rgb gain with the sensing factor (1~2) */
	i2val = (rgb_sens_ptr.gain_r * X_FACTOR)/D_FACTOR;
	auto_gain_r = BOUND_MAX((delta_gain_r * i2val)/X_FACTOR,CT_GAIN_MAX);

	i2val = (rgb_sens_ptr.gain_g * X_FACTOR)/D_FACTOR;
	auto_gain_g = BOUND_MAX((delta_gain_g * i2val)/X_FACTOR,CT_GAIN_MAX);

	i2val = (rgb_sens_ptr.gain_b * X_FACTOR)/D_FACTOR;
	auto_gain_b = BOUND_MAX((delta_gain_b * i2val)/X_FACTOR,CT_GAIN_MAX);

	/* all color gain & offset regs are set during V-ISR */


}


//----------------------------------------------------------------------
//  Brief   : Auto white-balance ISR (every V-sync)
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void DRVCUST_AutoWB_ISR(void)
{
	BOOL update = SV_FALSE;

	if (auto_gain_r != wb_final.wGainR
		|| auto_gain_g != wb_final.wGainG 
		|| auto_gain_b != wb_final.wGainB)
	{
		/* update final color gain (CT+user+Auto) */
		wb_final.wGainR = auto_gain_r;
		wb_final.wGainG = auto_gain_g;
		wb_final.wGainB = auto_gain_b;
		update = SV_TRUE;
	}

	if (delta_ofst_r != wb_final.wOfstR
		|| delta_ofst_g != wb_final.wOfstG
		|| delta_ofst_b != wb_final.wOfstB)
	{
		/* update final color offset (CT+user) */
		wb_final.wOfstR = delta_ofst_r;
		wb_final.wOfstG = delta_ofst_g;
		wb_final.wOfstB = delta_ofst_b;
		update = SV_TRUE;
	}

	if (update)
	{
		/* pass to callback */
		vWBSetGainOfst(&wb_final);
	}
}


//----------------------------------------------------------------------
//  Brief   : Returns color gain with user/auto offset.
//  @param  : none
//  @return : ECS <- 0~1023
// 
//----------------------------------------------------------------------
UINT16 Sony_AutoWB_GetGain (Auto_Gain_t gain, Gamma_RGB_t Gamma)
{
	if (gain == DELTA_GAIN)
	{
		switch (Gamma)
		{
			case GAMMA_R:	return delta_gain_r;
			case GAMMA_G:	return delta_gain_g;
			case GAMMA_B:	return delta_gain_b;
			default:		break;
		}
	}
	else /* FINAL_GAIN */
	{
		switch (Gamma)
		{
			case GAMMA_R:	return wb_final.wGainR;
			case GAMMA_G:	return wb_final.wGainG;
			case GAMMA_B:	return wb_final.wGainB;
			default:		break;
		}
	}
	return 0;
}



/* ---------------------------------------------- */
/*      P Q   N V M   V E R I F I C A T I O N     */
/* ---------------------------------------------- */
//----------------------------------------------------------------------
//  Brief   : Sony PQ nvm ID verification
//  @param  : none
//  @return : 1-verify OK, 0-verify NG
//
//----------------------------------------------------------------------
static BOOL Sony_VerifyPQNVM(void)
{
	UINT8 sony_pq_header[NVM_SONY_PQ_HEADER_SIZE];

	/* verify sony pq nvm header */
	EEPROM_Read(EEP_OFFSET_EMCS_PQ_START, (UINT32)sony_pq_header, (UINT32)NVM_SONY_PQ_HEADER_SIZE);
	if (x_strcmp((CHAR *)(sony_pq_header), (CHAR *)NVM_SONY_PQ_HEADER_ID))
	{		
		PQ_DBG("PQ nvm ID verify NG!!\n");
		return SV_FALSE;
	}

	PQ_DBG("PQ nvm ID verify OK!!\n");
	return SV_TRUE;
}
#ifdef CUST_SOEM_DRV
//return 0: suc
//return 1: fail
UINT32 Sony_PQ_GAMMA_EDID_Write (UINT16 write_data)
{
    UINT8 sony_pq_header[NVM_SONY_PQ_HEADER_SIZE]={0};
    //UINT8 luma_read[NVM_GAMMA_LVL_SIZE]={0};
    UINT8 *luma_read;
	UINT8 wb_data_nvm[NUM_OF_WB_INDEX*NUM_OF_WB_ITEMS*2]={0};
	UINT8 Trak_verbyte[EEP_WB_TRAK_VER_BYTE]={0};
	//UINT32 compare_eep_buf[131]={0};
	//UINT32 compare_panel_buf[131]={0};  
	UINT32 *compare_eep_buf;
	UINT32 *compare_panel_buf;  //modify to fix warning "Frame size bigger than 1024Byte"
	
	UINT32 u4_result = 0;
	UINT16 j,z;
	UINT8 u1_LastByte[2];
	UINT32 u2_last_byte_addr=0;
	u1_LastByte[0] = 0x57; //trak ver byte
	u1_LastByte[1] = 0x54;
	u2_last_byte_addr = WB_TRACKING3_START_ADDR + NVM_WB_SIZE;
 
 	luma_read = x_mem_alloc(NVM_GAMMA_LVL_SIZE);
	compare_eep_buf = x_mem_alloc(4*131);
	compare_panel_buf = x_mem_alloc(4*131);

	x_memset(luma_read, 0, NVM_GAMMA_LVL_SIZE);	
	x_memset(compare_eep_buf, 0, 4*131);
	x_memset(compare_panel_buf, 0, 4*131);

    if(write_data == 1)  
     {
        GPIO_SetOut(PANEL_BINT_PIN,1);
		do
		{
//==========================   Read verify byte in eep	=================================
			if (0 != EEPROM_Read( 0x39a7, (UINT32)Trak_verbyte, EEP_WB_TRAK_VER_BYTE))
			{
				u4_result = 1;
				break;
			}
			if(Trak_verbyte[0]!=EEP_WB_TRAK_VER1 && Trak_verbyte[1]!=EEP_WB_TRAK_VER2)
			{
				u4_result = 1;
				break;
			}
//========================== write trak into panel	 =====================================

/*                  if (0 != EEPROM_Read( 0xaa, (UINT32)u1_magic, (UINT32)3))

		       {
		                u4_result = 1;
		                break;
		       }
		       if (0 == SIF_X_Write(0, 0x100, 0xaa, 2, 0xee,u1_magic, 3)) //magic & pbm& /gamma
		       {
		                u4_result = 1;
		                break;
		       }
	x_thread_delay(20);

        Factory2K12Debug(c_sprintf(fact_dbg_buf,"== m(%x)(%x)(%x)--\n",u1_magic[0],u1_magic[1],u1_magic[2]));
*/
			if (0 != EEPROM_Read(EEP_OFFSET_EMCS_PQ_START , (UINT32)sony_pq_header, (UINT32)NVM_SONY_PQ_HEADER_SIZE))
			{	
				u4_result = 1;
				break;
			}
			else
			{
				x_memset(compare_eep_buf,0,131);
				j=0;
				for(z=0; z<NVM_SONY_PQ_HEADER_SIZE; z++)
				{
					compare_eep_buf[j]=sony_pq_header[z];
					j++;
				}
			}	
			
			if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING1_START_ADDR, sony_pq_header, NVM_SONY_PQ_HEADER_SIZE))//header
			{
				u4_result = 1;
				break;
			}

	x_thread_delay(20);

			SIF_X_Read(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING1_START_ADDR, sony_pq_header, NVM_SONY_PQ_HEADER_SIZE);
			x_memset(compare_panel_buf,0, 131);
			j=0;
			for(z=0; z<NVM_SONY_PQ_HEADER_SIZE;z++)
				{	
					compare_panel_buf[j]=sony_pq_header[z];
					j++;
				}			

			if (0 != EEPROM_Read( (UINT64 )NVM_GAMMA_LVL_OFFSET, (UINT32)luma_read, (UINT32)NVM_GAMMA_LVL_SIZE))

			{
				u4_result = 1;
				break;
			}
			else
			{
			x_memset(compare_eep_buf+4, 0, 126);
			j=5;
			for(z=0; z<NVM_GAMMA_LVL_SIZE; z++)
				{
					compare_eep_buf[j]=luma_read[z];
					j++;
				}
			}
			
	x_thread_delay(20);
			if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING2_START_ADDR, luma_read, 66))
			{
				u4_result = 1;
				break;
			}

			SIF_X_Read(BUS_ID,CLK_DIV,TCON_NVM_SLVADDR,ADDR_LENGTH,WB_TRACKING2_START_ADDR,luma_read,66);
			x_memset(compare_panel_buf +4,0,126);
			j=5;
			for(z=0; z<66;z++)
			{	
					compare_panel_buf[j]= luma_read[z];
					j++;
			}
							
	x_thread_delay(20);
			
			if (0 != EEPROM_Read((UINT64)NVM_WB_OFFSET, (UINT32)wb_data_nvm, (UINT32)NVM_WB_SIZE))
			{
				u4_result = 1;
				break;
			}
			else
			{
			x_memset(compare_eep_buf+70, 0, 60);
			j=71;
			for(z=0; z<NVM_WB_SIZE; z++)
				{
					compare_eep_buf[j]=wb_data_nvm[z];
					j++;
				}
			}

	x_thread_delay(20);
			if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING3_START_ADDR, wb_data_nvm, NVM_WB_SIZE))
			{
				u4_result = 1;
				break;
			}

			SIF_X_Read(BUS_ID,CLK_DIV,TCON_NVM_SLVADDR,ADDR_LENGTH,WB_TRACKING3_START_ADDR, wb_data_nvm, 60);
			x_memset(compare_panel_buf +70,0,60);
			j=71;
			for(z=0; z<NVM_WB_SIZE;z++)
			{	
					compare_panel_buf[j]= wb_data_nvm[z];
					j++;
			}
	
			
//========================= 			compare byte to byte	   ===================================
		
			for(z=0; z<131; z++)
			{
				if(compare_eep_buf[z]!= compare_panel_buf[z])
				{
					u4_result = 1;
					break;
				}
			}

//======================		  write last byte 0x5754 to panel 	 ==================================
	x_thread_delay(20);
			if (SIF_ERROR_RETURN == SIF_X_Write(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_last_byte_addr ,u1_LastByte, 2)) // write loop by per 16 byte
			{
					u4_result = 1;
					break;
			}

//==========================  write new verify byte in eep	================================
			Trak_verbyte[0] = 0xff;
			Trak_verbyte[1] = 0xff;
			if (0 != EEPROM_Write( 0x39a7, (UINT32)Trak_verbyte,(UINT32)EEP_WB_TRAK_VER_BYTE))
			{
				u4_result = 1;
				break;
			}
		}while(0);
        GPIO_SetOut(PANEL_BINT_PIN,0);
    }
    else if(write_data==3)
	{
		do
		{
//======================		  confirm verify byte in panel		 ==================================

			if(SIF_ERROR_RETURN == SIF_X_Read(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, u2_last_byte_addr ,u1_LastByte, 2)) 
			{
				u4_result = 1;
				break;
			}
			if(u1_LastByte[0]!=0x57 && u1_LastByte[1]!=0x54)
			{
				u4_result = 1;
				break;
			}

//======================		write PBM to EEP		=========================================

			if (SIF_ERROR_RETURN == SIF_X_Read(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING1_START_ADDR, sony_pq_header, NVM_SONY_PQ_HEADER_SIZE)) //header 
			{
				u4_result = 1;
				break;
			}
			else
			{
			x_memset(compare_panel_buf,0,131);
			j=0;
			for(z=0; z<5; z++)
				{
					compare_panel_buf[j]=sony_pq_header[z];
					j++;
				}
			}

			if (0 != EEPROM_Write(EEP_OFFSET_EMCS_PQ_START,(UINT32)sony_pq_header,(UINT32)NVM_SONY_PQ_HEADER_SIZE))
			{
				u4_result = 1;
				break;
			}
			EEPROM_Read(EEP_OFFSET_EMCS_PQ_START , (UINT32)sony_pq_header, (UINT32)NVM_SONY_PQ_HEADER_SIZE);
			x_memset(compare_eep_buf,0,131);
			j=0;
			for(z=0;z<NVM_SONY_PQ_HEADER_SIZE;z++)
			{
				compare_eep_buf[j]=sony_pq_header[z];
				j++;
			}	


			if (SIF_ERROR_RETURN == SIF_X_Read(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING2_START_ADDR, luma_read, NVM_GAMMA_LVL_SIZE)) //luma
			{
				u4_result = 1;
				break;
			}
			else
			{
			x_memset(compare_panel_buf +4,0,126);
			j=5;
			for(z=0; z<66; z++)
				{
					compare_panel_buf[j]=luma_read[z];
					j++;
				}
			}
			
			if (0 != EEPROM_Write((UINT64) NVM_GAMMA_LVL_OFFSET,(UINT32)luma_read,66))
			{
				u4_result = 1;
				break;
			}

			EEPROM_Read((UINT64) NVM_GAMMA_LVL_OFFSET,(UINT32)luma_read,(UINT32)NVM_GAMMA_LVL_SIZE);
			x_memset(compare_eep_buf +4,0,126);
			j=5;
			for(z=0;z<66;z++)
			{
				compare_eep_buf[j]=luma_read[z];
				j++;
			}	

			if (SIF_ERROR_RETURN == SIF_X_Read(BUS_ID, CLK_DIV, TCON_NVM_SLVADDR, ADDR_LENGTH, WB_TRACKING3_START_ADDR, wb_data_nvm, NVM_WB_SIZE)) //wb
			{
				u4_result = 1;
				break;
			}
    else
    {
			x_memset(compare_panel_buf +70,0,60);
			j=71;
			for(z=0; z<NVM_WB_SIZE; z++)
				{
					compare_panel_buf[j]=wb_data_nvm[z];
					j++;
				}
			}
			
			if (0 != EEPROM_Write((UINT64) NVM_WB_OFFSET,(UINT32)wb_data_nvm,NVM_WB_SIZE))
			{
				u4_result = 1;
				break;
			}

			EEPROM_Read((UINT64) NVM_WB_OFFSET,(UINT32)wb_data_nvm,(UINT32)NVM_WB_SIZE);
			x_memset(compare_eep_buf+70,0,60);
			j=71;
			for(z=0;z<NVM_WB_SIZE;z++)
			{
				compare_eep_buf[j]=wb_data_nvm[z];
				j++;
			}


//==============================	 compare byte to byte	===================================
			
			for(z=0; z<131; z++)
			{
			if(compare_panel_buf[z]!= compare_eep_buf[z])
				{
					u4_result = 1;
					break;
				}
			}
			
//==============================	   write verify byte in eep 	 =================================
			Trak_verbyte[0]= EEP_WB_TRAK_VER1;   //ver byte by jig
			Trak_verbyte[1]= EEP_WB_TRAK_VER2;
			if (0 != EEPROM_Write( 0x39a7, (UINT32)Trak_verbyte, (UINT32)EEP_WB_TRAK_VER_BYTE))
			{
				u4_result = 1;
				break;
			}
		}while(0);
    }
    Factory2K12Debug(c_sprintf(fact_dbg_buf,"trac ret(%d)\n", u4_result));
	x_mem_free(luma_read);
	x_mem_free(compare_eep_buf);
	x_mem_free(compare_panel_buf);	
	luma_read = NULL;
	compare_eep_buf = NULL;
	compare_panel_buf = NULL;
   return u4_result;
}
#endif //CUST_SOEM_DRV












/* --------------------------- */
/*      D E B U G              */
/* --------------------------- */
#ifdef PQ_DBG_ON
//----------------------------------------------------------------------
//  Brief   : Print time log
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void pq_GetTimeReference(void)
{
	log_time = x_os_get_sys_tick() * x_os_get_sys_tick_period();
	x_dbg_stmt("PQLOG[%d]-> ",log_time);
}


//----------------------------------------------------------------------
//  Brief   : List PQ nvm mapping.
//  @param  : none
//  @return : none
// 
//----------------------------------------------------------------------
void Sony_ListPQNVM(void)	// for custom CLI
{
	if (Sony_VerifyPQNVM() == SV_FALSE)
	{
		return;
	}

	PQ_DBG("------------------------------------------\n");
	PQ_DBG("EEP_OFFSET_EMCS_PQ_START = %4x\n", EEP_OFFSET_EMCS_PQ_START);
	PQ_DBG("EEP_OFFSET_EMCS_PQ_END   = %4x\n", EEP_OFFSET_EMCS_PQ_END);
	PQ_DBG("Total PQ nvm size        = %4x (%4d byte)\n",						\
							EEP_OFFSET_EMCS_PQ_END-EEP_OFFSET_EMCS_PQ_START+1,	\
							EEP_OFFSET_EMCS_PQ_END-EEP_OFFSET_EMCS_PQ_START+1);

	PQ_DBG("             -----------------------------\n");
	PQ_DBG("              Start ->  End\n");
	PQ_DBG("             -----------------------------\n");
	PQ_DBG("PQ NVM header %x  ->  %x (%4d byte)\n",								\
							NVM_SONY_PQ_HEADER_OFFSET, 							\
							NVM_SONY_PQ_HEADER_OFFSET+NVM_SONY_PQ_HEADER_SIZE-1,\
							NVM_SONY_PQ_HEADER_SIZE);

	PQ_DBG("             -----------------------------\n");
	PQ_DBG("Gamma/WB NVM  %x  ->  %x (%4d byte)\n",								\
							NVM_GAMMA_START_ADDR, 								\
							NVM_GAMMA_END_ADDR,									\
							NVM_GAMMA_END_ADDR-NVM_GAMMA_START_ADDR+1);

	PQ_DBG("             -----------------------------\n");
	PQ_DBG("Gamma level   %x  ->  %x (%4d byte)\n", 							\
							NVM_GAMMA_LVL_OFFSET, 								\
							NVM_GAMMA_LVL_OFFSET+NVM_GAMMA_LVL_SIZE-1,			\
							NVM_GAMMA_LVL_SIZE);

	PQ_DBG("Gamma offset  %x  ->  %x (%4d byte)\n", 							\
							NVM_GAMMA_INP_OFST_OFFSET, 							\
							NVM_GAMMA_INP_OFST_OFFSET+NVM_GAMMA_INP_OFST_SIZE-1,\
							NVM_GAMMA_INP_OFST_SIZE);

	PQ_DBG("WB data       %x  ->  %x (%4d byte)\n", 							\
							NVM_WB_OFFSET, 										\
							NVM_WB_OFFSET+NVM_WB_SIZE-1,						\
							NVM_WB_SIZE);

	PQ_DBG("Gamma table   %x  ->  %x (%4d byte)\n", 							\
							NVM_GAMMA_INP_OFFSET,								\
							NVM_GAMMA_INP_OFFSET+NVM_GAMMA_DATA_SIZE-1,			\
							NVM_GAMMA_DATA_SIZE);

	PQ_DBG("             -----------------------------\n");
	if (NVM_GAMMA_LAST_ADDR > NVM_GAMMA_END_ADDR)
	{
		/* check if gamma nvm range exceeds the limit */
		PQ_DBG("[W A R N I N G] Gamma nvm overflow!\n");
		PQ_DBG("NVM_GAMMA_LAST_ADDR  -> %x\n", NVM_GAMMA_LAST_ADDR);
	}

	PQ_DBG("NVM_GAMMA_TOTAL_SIZE  = %4x (%4d byte)\n", NVM_GAMMA_TOTAL_SIZE	, NVM_GAMMA_TOTAL_SIZE	);
	PQ_DBG("NVM_GAMMA_USED_MEMORY = %4x (%4d byte)\n", NVM_GAMMA_USED_MEMORY, NVM_GAMMA_USED_MEMORY	);
	PQ_DBG("NVM_GAMMA_REMAINING   = %4x (%4d byte)\n", NVM_GAMMA_REMAINING	, NVM_GAMMA_REMAINING	);
	PQ_DBG("------------------------------------------\n");
}








#endif /* PQ_DBL_ON */





#endif /* CC_SONY_GAMMA */

