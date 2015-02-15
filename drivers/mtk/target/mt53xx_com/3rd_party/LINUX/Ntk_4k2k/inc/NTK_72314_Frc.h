#ifndef NTK_72314_FRC
#define  NTK_72314_FRC

#include "Def_types.h"

#ifdef  __cplusplus
extern "C" {
#endif
/*-----------------------------------------------------------------------------*/
/* Including Files                                                             */
/*-----------------------------------------------------------------------------*/
//#include "hal_frc.h"

#define PLAT_FRC_VERSION               "PLAT-FRC 0.0.1"
#define FRC_DBG_OK 0
#define FRC_DBG_ERR 1
#define TIMERBASE_PWM_DUTY_CONTROL_MODE          2   // 0:Line-count, 1:0..100 percent, 2:Duty(0..1023),Delay(0..100)
/*-----------------------------------------------------------------------------*/
/* Constant Definitions                                                        */
/*-----------------------------------------------------------------------------*/
//#include <stdio.h>

/***************need data structure list**************************************/

typedef enum
{
	EN_MAILBOX_QUEUE_EMPTY 			= 1,
	EN_MAILBOX_QUEUE_FULL 				= (1<<1),
	EN_MAILBOX_QUEUE_PENDING 		= (1<<2),
	EN_MAILBOX_QUEUE_SUSPEND 		= (1<<3),		//for DSU debug
}EN_PLAT_FRC_MAILBOX_STATUS_k;

typedef enum
{
	EN_PLAT_FRC_314_REG_GET_VERSION,			//!<
	
	EN_PLAT_FRC_314_REG_MAILBOX_ADDRESS,        //!<
	EN_PLAT_FRC_314_REG_MAILBOX_STATUS,         //!<

	EN_PLAT_FRC_314_REG_MAX					//!<
  		
}EN_PLAT_FRC_314_REG_k;						//!<



typedef struct ST_PLAT_FRC_314__REG_TABLE_s
{
	EN_PLAT_FRC_314_REG_k				eRegName;			//!<
	u32								Addr;				//!<
} ST_PLAT_FRC_314_REG_TABLE_t;							//!<


/***************************************************************************/
/** Main Command Type */
typedef enum
{
    EN_CMD_MAIN_GENERAL 			= 0x00,
    EN_CMD_MAIN_MEMC 				= 0x01,
    EN_CMD_MAIN_FORMAT 				= 0x02,
    EN_CMD_MAIN_WB 					= 0x03,
    EN_CMD_MAIN_LVDS 				= 0x04,
    EN_CMD_MAIN_PWM 				= 0x05,
    EN_CMD_MAIN_SSC 				= 0x06,
    EN_CMD_MAIN_WINDOW 				= 0x07,
    EN_CMD_MAIN_PANEL 				= 0x08,
    EN_CMD_MAIN_LDC 				= 0x09,
    EN_CMD_MAIN_OSD 				= 0x0A,
    EN_CMD_MAIN_PQ 					= 0x0B,

    EN_CMD_MAIN_MISC 				= 0x0E,
    EN_CMD_MAIN_CUSTOM 				= 0x0F,

#if 0 //(PROJECT_SELECTION == PRJ_PHILIPS)
    EN_CMD_MAIN_TPVISION			= 0x10,
    EN_CMD_MAIN_TPVISION2			= 0x11,
#endif

    EN_CMD_MAIN_SLAVE_DBG			= 0x2F,

    //24 bits value command
    EN_CMD_MAIN_ISP_MEM_WRITE		= 0x30,
    EN_CMD_MAIN_ISP_MEM_READ 		= 0x31,
    EN_CMD_MAIN_ISP_FW_STATUS		= 0x32,
    EN_CMD_MAIN_ISP_BUF_CHKSUM		= 0x33,
    EN_CMD_MAIN_ISP_CHIP_PROTECT	= 0x34,
    EN_CMD_MAIN_ISP_BLOCK_ERASE		= 0x35,
    EN_CMD_MAIN_ISP_SECTOR_ERASE	= 0x36,
    EN_CMD_MAIN_ISP_PAGE_WRITE		= 0x37,
    EN_CMD_MAIN_ISP_PAGE_READ		= 0x38,
    EN_CMD_MAIN_ISP_REBOOT			= 0x3A,
    EN_CMD_MAIN_ISP_SLAVE_COMMAND	= 0x3D,
    EN_CMD_MAIN_ISP_SLAVE_PASSWORD	= 0x3E,
    EN_CMD_MAIN_ISP_MASTER_PASSWORD	= 0x3F,

} EN_PLAT_FRC_314_CMD_MAIN_TYPE_k; 

/*********************Sub Command Type *************************/
//GENERAL
typedef enum
{   
    EN_CMD_SUB_GENERAL_VIDEO_MUTE				= 0x00,
    EN_CMD_SUB_GENERAL_OSD_MUTE					= 0x01,
    EN_CMD_SUB_GENERAL_ALL_MUTE					= 0x02,
    EN_CMD_SUB_GENERAL_TESTPAT_MODE				= 0x03,
    EN_CMD_SUB_GENERAL_TESTPATTERN				= 0x04,
    EN_CMD_SUB_GENERAL_PANELMODEL				= 0x05,
    EN_CMD_SUB_GENERAL_BLEND_OPTION_NOVIDEO		= 0x06,
    EN_CMD_SUB_GENERAL_BLEND_OPTION_ZEROALPHA	= 0x07,
    EN_CMD_SUB_GENERAL_BLEND_OPTION_OSDDATA		= 0x08,
    EN_CMD_SUB_GENERAL_BLEND_OPTION_OUTPUT		= 0x09,
    EN_CMD_SUB_GENERAL_VSPLL_EN					= 0x0A,

	//maybe no need, if needed change number next to previous number
    EN_CMD_SUB_GENERAL_PIP_STATS				= 0x20,		//not release
    EN_CMD_SUB_GENERAL_PICTURE_IMPROVEMENT		= 0x21, 	//not release
    EN_CMD_SUB_GENERAL_WATCHDOGTASK_EN			= 0x22,		//not release
	  
} EN_CMD_GENERAL_TYPE;

//MEMC OR FRC
typedef enum
{
    EN_CMD_SUB_MEMC_MODE  						= 0x00,
    EN_CMD_SUB_MEMC_DEJUDDER_LEVEL				= 0x01,
    EN_CMD_SUB_MEMC_DEBLUR_LEVEL				= 0x02,
    EN_CMD_SUB_MEMC_LR_SWAP						= 0x03,
    EN_CMD_SUB_MEMC_DEPTH_LEVEL			= 0x04,
    EN_CMD_SUB_MEMC_DISPARTY_LEVEL				= 0x07, 

    //Advance MEMC control form Philips spec
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_SENSITIVITY	= 0x80,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_SPEED			= 0x81,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_JUDDER		= 0x82,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_MAXJUDDER		= 0x83,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_MINJUDDER		= 0x84,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_BLUR			= 0x85,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_MAXBLUR		= 0x86,

    EN_CMD_SUB_MEMC_FILM22_BREAKUP_SENSITIVITY	= 0x87,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_SPEED		= 0x88,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_JUDDER		= 0x89,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_MAXJUDDER	= 0x8A,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_MINJUDDER	= 0x8B,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_BLUR			= 0x8C,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_MAXBLUR		= 0x8D,

    EN_CMD_SUB_MEMC_FILM32_BREAKUP_SENSITIVITY	= 0x8E,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_SPEED		= 0x8F,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_JUDDER		= 0x90,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_MAXJUDDER	= 0x91,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_MINJUDDER	= 0x92,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_BLUR			= 0x93,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_MAXBLUR		= 0x94,

    EN_CMD_SUB_MEMC_VIDEO_HALO_SENSITIVITY		= 0x95,
    EN_CMD_SUB_MEMC_VIDEO_HALO_SPEED			= 0x96,
    EN_CMD_SUB_MEMC_VIDEO_HALO_JUDDER			= 0x97,
    EN_CMD_SUB_MEMC_VIDEO_HALO_MAXJUDDER		= 0x98,
    EN_CMD_SUB_MEMC_VIDEO_HALO_MINJUDDER		= 0x99,
    EN_CMD_SUB_MEMC_VIDEO_HALO_BLUR				= 0x9A,
    EN_CMD_SUB_MEMC_VIDEO_HALO_MAXBLUR			= 0x9B,

    EN_CMD_SUB_MEMC_FILM22_HALO_SENSITIVITY		= 0x9C,
    EN_CMD_SUB_MEMC_FILM22_HALO_SPEED			= 0x9D,
    EN_CMD_SUB_MEMC_FILM22_HALO_JUDDER			= 0x9E,
    EN_CMD_SUB_MEMC_FILM22_HALO_MAXJUDDER		= 0x9F,
    EN_CMD_SUB_MEMC_FILM22_HALO_MINJUDDER		= 0xA0,
    EN_CMD_SUB_MEMC_FILM22_HALO_BLUR			= 0xA1,
    EN_CMD_SUB_MEMC_FILM22_HALO_MAXBLUR			= 0xA2,

    EN_CMD_SUB_MEMC_FILM32_HALO_SENSITIVITY		= 0xA3,
    EN_CMD_SUB_MEMC_FILM32_HALO_SPEED			= 0xA4,
    EN_CMD_SUB_MEMC_FILM32_HALO_JUDDER			= 0xA5,
    EN_CMD_SUB_MEMC_FILM32_HALO_MAXJUDDER		= 0xA6,
    EN_CMD_SUB_MEMC_FILM32_HALO_MINJUDDER		= 0xA7,
    EN_CMD_SUB_MEMC_FILM32_HALO_BLUR			= 0xA8,
    EN_CMD_SUB_MEMC_FILM32_HALO_MAXBLUR			= 0xA9,

    EN_CMD_SUB_MEMC_HALO_REDUCTION							= 0xAA,

    EN_CMD_SUB_MEMC_FILM_MOTION_BLUR_REDUCTION_SENSITIVITY	= 0xAB,
    EN_CMD_SUB_MEMC_FILM_MOTION_BLUR_REDUCTION_SPEED		= 0xAC,
    EN_CMD_SUB_MEMC_SET_FORCE_LEVEL		=0xAD,
} EN_CMD_MEMC_ST_TYPE;

//FORMAT
typedef enum
{
    EN_CMD_SUB_FORMAT_OUTPUT_MODE 			= 0x00,
    EN_CMD_SUB_FORMAT_INPUT_FORMAT			= 0x01,
    EN_CMD_SUB_FORMAT_OUTPUT_FORMAT			= 0x02,
    EN_CMD_SUB_FORMAT_INPUT_FRAME_RATE		= 0x03,
    EN_CMD_SUB_FORMAT_OUTPUT_FRAME_RATE		= 0x04,
    EN_CMD_SUB_FORMAT_INPUT_OUTPUT_FORMAT	= 0x05,
    EN_CMD_SUB_FORMAT_AUTO_DETECT_3D_ONOFF	= 0x06,
    EN_CMD_SUB_FORMAT_INPUT_RESOLUTION		= 0x07,

    // Leen add for Skyworth to switch 3D on off. Feb.05.2013
	EN_CMD_SUB_FORMAT_SKYWORTH_3D_FORMAT	= 0x31,


	//--------------------------------------------
    EN_CMD_SUB_FORMAT_FHD2UHD 				= 0xFF,

} EN_CMD_FORMAT_ST_TYPE;

//WINDOW
typedef enum
{
    EN_CMD_SUB_WINDOW_SETID				= 0x00,
    EN_CMD_SUB_WINDOW_ONOFF				= 0x01,
    EN_CMD_SUB_WINDOW_HEIGHT			= 0x02,
    EN_CMD_SUB_WINDOW_WIDTH				= 0x03,
    EN_CMD_SUB_WINDOW_X_POSITION		= 0x04,
    EN_CMD_SUB_WINDOW_Y_POSITION		= 0x05,
    EN_CMD_SUB_WINDOW_DEMO_MODE			= 0x06,
} EN_CMD_WINDOW_ST_TYPE;

//OSD
typedef enum
{
	EN_CMD_SUB_OSD_INPUT_FORMAT				= 0x00,
	EN_CMD_SUB_OSD_OUTPUT_FORMAT 			= 0x01,
	EN_CMD_SUB_OSD_INPUT_OUTPUT_FORMAT 		= 0x02,
	EN_CMD_SUB_OSD_PIXEL_FORMAT_8BIT 		= 0x03,
	EN_CMD_SUB_OSD_PIXEL_FORMAT_444 		= 0x04,
	EN_CMD_SUB_OSD_PIXEL_FORMAT_RGB 		= 0x05,
	EN_CMD_SUB_OSD_PIXEL_FORMAT_PERALPHA 	= 0x06,
	EN_CMD_SUB_OSD_INPUT_WIDTH 				= 0x07,
	EN_CMD_SUB_OSD_INPUT_HEIGHT 			= 0x08,
	EN_CMD_SUB_OSD_OUTPUT_WIDTH 			= 0x09,
	EN_CMD_SUB_OSD_OUTPUT_HEIGHT 			= 0x0A,
	EN_CMD_SUB_OSD_X_POSITION 				= 0x0B,
	EN_CMD_SUB_OSD_Y_POSITION 				= 0x0C,

	EN_CMD_SUB_OSD_COLORKEY_R 				= 0x0D,
	EN_CMD_SUB_OSD_COLORKEY_G 				= 0x0E,
	EN_CMD_SUB_OSD_COLORKEY_B 				= 0x0F,
	EN_CMD_SUB_OSD_COLORKEYALPHA 			= 0x10,
	EN_CMD_SUB_OSD_COLORKEY_EN 				= 0x11,
	EN_CMD_SUB_OSD_NONCOLORKEYALPHA 		= 0x12,
	EN_CMD_SUB_OSD_NONCOLORKEY_EN 			= 0x13,
	EN_CMD_SUB_OSD_PLANEALPHA 				= 0x14,
	EN_CMD_SUB_OSD_PLANEALPHA_EN 			= 0x15,

	EN_CMD_SUB_OSD_ONOFF 					= 0x16,
	EN_CMD_SUB_OSD_FREEZE_EN 				= 0x17,
	EN_CMD_SUB_OSD_SEAMLESS_CHANGE 			= 0x18,


} EN_CMD_OSD_ST_TYPE;

//PQ
typedef enum
{
       EN_CMD_SUB_PQ_CM_EN                             = 0x00,  //Color Management
       EN_CMD_SUB_PQ_SR_ONOFF                       = 0x01,  //Super Resolution
       EN_CMD_SUB_PQ_SR_DEMO_ONOFF                  = 0x02,
       EN_CMD_SUB_PQ_SR_LEVEL                         = 0x03,
       EN_CMD_SUB_PQ_CE_GAIN_L                       = 0x04,
       EN_CMD_SUB_PQ_CE_GAIN_R                       = 0x05,
       EN_CMD_SUB_PQ_AUTO_CONTRAST                   = 0x06,
       EN_CMD_SUB_PQ_LUMA_STRETCH               = 0x07,
       EN_CMD_SUB_PQ_VEP_GO                            = 0x08,
       EN_CMD_SUB_PQ_XVYCC                              = 0x09,
       EN_CMD_SUB_PQ_XVYCC_GO_1                   = 0x0A,
       EN_CMD_SUB_PQ_XVYCC_GO_2                   = 0x0B,
       EN_CMD_SUB_PQ_GMA                                = 0x0C,
       EN_CMD_SUB_PQ_GAMMA_EN                            = 0x0D,
       EN_CMD_SUB_PQ_GAMMA_RW                           = 0x0E,
       EN_CMD_SUB_PQ_BRIGHTNESS                    = 0x0F,
       EN_CMD_SUB_PQ_CONTRAST                       = 0x10,
       EN_CMD_SUB_PQ_SATRUATION                    = 0x11,
       EN_CMD_SUB_PQ_HUE                                  = 0x12,
       EN_CMD_SUB_PQ_SHARPNESS                            = 0x13,

	//need expend
} EN_CMD_PQ_ST_TYPE;

/** Sub Command Define Macro of LVDS Main Command  */
typedef enum
{
    EN_CMD_SUB_LVDS_ONOFF   			= 0x00,
    EN_CMD_SUB_LVDS_RX_BITS				= 0x01,
    EN_CMD_SUB_LVDS_RX_TIMODE			= 0x02,
    EN_CMD_SUB_LVDS_RX_PORTAB_SWAP		= 0x03,
    EN_CMD_SUB_LVDS_DP_4PX_OUT_SWAP		= 0x04,
    EN_CMD_SUB_LVDS_DP_PORTAB_SWAP		= 0x05,
    EN_CMD_SUB_LVDS_DP_PORTCD_SWAP		= 0x06,
    EN_CMD_SUB_LVDS_DP_PORTBC_SWAP		= 0x07,
    EN_CMD_SUB_LVDS_DP_TX_BITS			= 0x08,
    EN_CMD_SUB_LVDS_SETVIDEOPORT			= 0x09,
    EN_CMD_SUB_LVDS_SETOSDPORT			= 0x0a,
	//Vx1 control
	EN_CMD_SUB_VX1_TWO_TCON_MODE		= 0x10,
} EN_CMD_LVDS_ST_TYPE;

/*** Value Type***/
/******************3D change format****************************/

typedef enum
{
	EN_PLAT_FRC_314_3D_L_TO_R,			//!<
	EN_PLAT_FRC_314_3D_R_TO_L,			//!<
	EN_PLAT_FRC_314_3DLR_MAX				//!<
} EN_PLAT_314_MEMC_3DLRToggle;		//!<



typedef enum
{
    EN_CMD_SUB_WB_EN    = 0,
    EN_CMD_SUB_WB_R_GAIN,
    EN_CMD_SUB_WB_G_GAIN,   
    EN_CMD_SUB_WB_B_GAIN,
    EN_CMD_SUB_WB_R_OFFSET ,
    EN_CMD_SUB_WB_G_OFFSET,
    EN_CMD_SUB_WB_B_OFFSET ,
    EN_CMD_SUB_WB_R_LIMIT,
    EN_CMD_SUB_WB_G_LIMIT ,
    EN_CMD_SUB_WB_B_LIMIT ,
    EN_CMD_SUB_WB_GRADATION_R,
    EN_CMD_SUB_WB_GRADATION_G,
    EN_CMD_SUB_WB_GRADATION_B,
} EN_CMD_WB_ST_TYPE; 



/******************frc function list******************************************/
typedef enum
{
	//OSD 
	EN_PLAT_FRC_FUNC_OSD_INPUT_FORMAT,                        //!<
	EN_PLAT_FRC_FUNC_OSD_OUTPUT_FORMAT,                        //!<
	EN_PLAT_FRC_FUNC_OSD_INPUT_OUTPUT_FORMAT,                        //!<
	EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_8BIT,                        //!<
	EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_444,                        //!<
	EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_RGB,                        //!<
	EN_PLAT_FRC_FUNC_OSD_PIXEL_FORMAT_PERALPHA,                        //!<
	EN_PLAT_FRC_FUNC_OSD_INPUT_WIDTH,                        //!<
	EN_PLAT_FRC_FUNC_OSD_INPUT_HEIGHT,                        //!<
	EN_PLAT_FRC_FUNC_OSD_OUTPUT_WIDTH,                        //!<
	EN_PLAT_FRC_FUNC_OSD_OUTPUT_HEIGHT,                        //!<
	EN_PLAT_FRC_FUNC_OSD_X_POSITION,                        //!<
	EN_PLAT_FRC_FUNC_OSD_Y_POSITION,                        //!<

	//COLORKEY
	EN_PLAT_FRC_FUNC_OSD_COLORKEY_R,                        //!<
	EN_PLAT_FRC_FUNC_OSD_COLORKEY_G,                        //!<
	EN_PLAT_FRC_FUNC_OSD_COLORKEY_B,                        //!<
	EN_PLAT_FRC_FUNC_OSD_COLORKEYALPHA,                        //!<
	EN_PLAT_FRC_FUNC_OSD_COLORKEY_EN,                        //!<
	EN_PLAT_FRC_FUNC_OSD_NONCOLORKEYALPHA,                        //!<
	EN_PLAT_FRC_FUNC_OSD_NONCOLORKEY_EN,                        //!<
	EN_PLAT_FRC_FUNC_OSD_PLANEALPHA,                        //!<
	EN_PLAT_FRC_FUNC_OSD_PLANEALPHA_EN,                        //!<
	
	EN_PLAT_FRC_FUNC_OSD_ON_OFF,                        //!<
	EN_PLAT_FRC_FUNC_OSD_FREEZE_EN,                        //!<

	EN_PLAT_FRC_FUNC_FHDTO4K2K,

	EN_PLAT_FRC_FUNC_OSD_PORT,

	//video mute
	EN_PLAT_FRC_FUNC_VIDEO_MUTE_N,                        //!<

	//three d<->two d
	EN_PLAT_FRC_FUNC_INPUT_FORMAT,
	EN_PLAT_FRC_FUNC_OUTPUT_FORMAT,
	EN_PLAT_FRC_FUNC_SET_DISPARITY_LEVEL,
	EN_PLAT_FRC_FUNC_SET_DEPTH_LEVEL,
	EN_PLAT_FRC_FUNC_3D_LR_TOGGLE,

	//PC mode
	EN_PLAT_FRC_FUNC_PCMODE,

	//FRC mode
	EN_PLAT_FRC_FUNC_FRCMODE,
	EN_PLAT_FRC_FUNC_SET_DEJUDDERLEVEL,
	EN_PLAT_FRC_FUNC_SET_DEBLURLEVEL,

	//FRC DEMO MODE
	EN_PLAT_FRC_FUNC_SET_FRC_DEMOMODE,

	//White Balance
	EN_PLAT_FRC_FUNC_SET_WB_ONOFF,
	EN_PLAT_FRC_FUNC_SET_WB_R_GAIN,
	EN_PLAT_FRC_FUNC_SET_WB_G_GAIN,
	EN_PLAT_FRC_FUNC_SET_WB_B_GAIN,
	EN_PLAT_FRC_FUNC_SET_WB_R_OFFSET,
	EN_PLAT_FRC_FUNC_SET_WB_G_OFFSET,
	EN_PLAT_FRC_FUNC_SET_WB_B_OFFSET,
	EN_PLAT_FRC_FUNC_SET_WB_R_LIMIT,
	EN_PLAT_FRC_FUNC_SET_WB_G_LIMIT,
	EN_PLAT_FRC_FUNC_SET_WB_B_LIMIT,
	EN_PLAT_FRC_FUNC_SET_WB_R_GRADATION,
	EN_PLAT_FRC_FUNC_SET_WB_G_GRADATION,
	EN_PLAT_FRC_FUNC_SET_WB_B_GRADATION,

	//PQ 
	EN_PLAT_FRC_FUNC_SET_PQ_CONTRAST,
	EN_PLAT_FRC_FUNC_SET_PQ_BRIGHTNESS,
	EN_PLAT_FRC_FUNC_SET_PQ_SHARPNESS,
	EN_PLAT_FRC_FUNC_SET_PQ_SR_ONOFF,
	EN_PLAT_FRC_FUNC_SET_PQ_SR_DEMO_ONOFF,
	EN_PLAT_FRC_FUNC_SET_PQ_SR_LEVEL,

	//FHD&4K2K

	//3D
	EN_PLAT_FRC_FUNC_SET_SKYWORTH_3D,

	EN_PLAT_FRC_FUNC_MAX								//!<  
}EN_PLAT_FRC_FUNCTION_k;//!<



/************3D TYPE*****************************/
typedef enum
{
	EN_PLAT_FRC_314_3D_MODE_OFF = 0,				//!<
	EN_PLAT_FRC_314_3D_MODE_SIDE_BY_SIDE,		//!<
	EN_PLAT_FRC_314_3D_MODE_TOP_BOTTOM,		//!<
	EN_PLAT_FRC_314_3D_MODE_FRAME_PACKING,		//!<
	EN_PLAT_FRC_314_3D_MODE_2D_3D_CONVERSION,	//!<
	EN_PLAT_FRC_314_3D_MODE_CHECKER_BOARD,		//!<
	EN_PLAT_FRC_314_3D_MODE_LINE_BY_LINE,		//!<
	EN_PLAT_FRC_314_3D_MODE_VERTICAL_STRIPE,	//!<
	EN_PLAT_FRC_314_3D_MODE_FRAME_SEQUENTIAL,	//!<
	EN_PLAT_FRC_314_3D_MODE_INIT,				//!<
	EN_PLAT_FRC_314_3D_MODE_3D_2D_CHANGE,		//!<
	EN_PLAT_FRC_314_3D_MODE_MAX					//!<
}EN_PLAT_FRC_314_3DEffectMode_k;				//!<

typedef enum
{
	EN_PLAT_FRC_3DOUTPUT_2D_60 = 0,
	EN_PLAT_FRC_3DOUTPUT_SHARP_120_PLUS = 1,
	EN_PLAT_FRC_3DOUTPUT_CMI_120       	  = 2,
	EN_PLAT_FRC_3DOUTPUT_AUO_120       	  = 3,
	EN_PLAT_FRC_3DOUTPUT_3D_60             		= 4,  //normal 60Hz panel,no FRC
	EN_PLAT_FRC_3DOUTPUT_UPSIDE_SBS       =5,
	EN_PLAT_FRC_3DOUTPUT_MAX
}EN_PLAT_FRC_3DOutput_k;

typedef enum
{
 EN_PLAT_FRC_314_PANEL_AMLCD_120_PLUS = 0,
 EN_PLAT_FRC_314_PANEL_SHARP_120_PLUS = 1,
 EN_PLAT_FRC_314_PANEL_CMI_120        = 2,
 EN_PLAT_FRC_314_PANEL_AUO_120        = 3,
 EN_PLAT_FRC_314_PANEL_AMLCD_120_NORMAL = 4,//--2D panel
 EN_PLAT_FRC_314_PANEL_MAX
}EN_PLAT_FRC_314_Panel_k;

/**********************************frc mode****************************/
typedef enum
{
 EN_FRC_OFF,
 EN_FRC_STANDARD,
 EN_FRC_SMOOTH,
 EN_FRC_CLEAR,
 EN_FRC_CUSTOM,
 EN_FRC_DEMO,
 EN_FRC_MAX
}EN_FRC_MODE;

typedef enum _EN_FORMAT_OUTPUT_MODE
{
	EN_FORMAT_VD_MODE, 
	EN_FORMAT_PC_MODE,
	EN_FORMAT_INVALID_MODE,
} EN_FORMAT_OUTPUT_MODE;


/*-----------------------------------------------------------------------------*/
/* Types Declarations                                                          */
/*-----------------------------------------------------------------------------*/

typedef struct ST_PLAT_FRC_314_FUNCTION_TABLE_s
{
	EN_PLAT_FRC_FUNCTION_k			eFunctionID;		//!<

	EN_PLAT_FRC_314_CMD_MAIN_TYPE_k  u8MainType;
	u8 								u8SubType;
	u8 								u8Priority;

	u32								u32Default;			//!<
} ST_PLAT_FRC_314_FUNCTION_TABLE_t;					//!<

typedef enum _EN_INPUT_FORMAT
{
         EN_INPUT_2D,

         EN_INPUT_3D_LR,

         EN_INPUT_3D_TB,

         EN_INPUT_3D_SBS,

         EN_INPUT_3D_FP,

         EN_INPUT_3D_LINE_BY_LINE


}	EN_INPUT_314_FORMAT;

typedef enum _EN_OUTPUT_FORMAT
{
         EN_OUTPUT_2D, 

         EN_OUTPUT_3D_SG,

         EN_OUTPUT_3D_SSG_ME,

         EN_OUTPUT_3D_SSG_DDR,

         EN_OUTPUT_3D_PR1X_ME,

         EN_OUTPUT_3D_PR1X_DDR,

         EN_OUTPUT_3D_PR2X_DDR,

         EN_OUTPUT_3D_AUOPR_DDR,

         EN_OUTPUT_2D2X, 


} EN_OUTPUT_314_FORMAT;


typedef struct ST_PLAT_FRC_INPUT_FORMAT_TABLE_s
{
	EN_INPUT_314_FORMAT	eIFormatID;			//!<
	u32								u32Command;		//!<
} ST_PLAT_FRC_INPUT_FORMAT_TABLE_t;				//!<


typedef struct ST_PLAT_FRC_OUTPUT_FORMAT_TABLE_s
{
	EN_OUTPUT_314_FORMAT		eOFormatID;	//!<
	u32									u32Command;	//!<
} ST_PLAT_FRC_OUTPUT_FORMAT_TABLE_t;			//!<

typedef struct ST_PLAT_FRC_FRC_MODE_TABLE_s
{
	EN_FRC_MODE	eFRCModeID;			//!<
	u32								u32Command;		//!<
} ST_PLAT_FRC_FRC_MODE_TABLE_t;				//!<


FRC_Result App_FRC_314_Init(void); 
FRC_Result App_SET_FHD_TO_4K2K(void);
FRC_Result App_SET_4K2K_TO_FHD(void); 
FRC_Result App_FRC_Set_314_VideoMute(bool bOnOff);
FRC_Result App_FRC_Set_314_3DInputFormat(EN_INPUT_314_FORMAT e3DInputFormat);
FRC_Result App_FRC_Set_314_3DOutputFormat(EN_OUTPUT_314_FORMAT e3DOutputFormat);
FRC_Result APP_FRC_Update_CodebySoc(char *Path);
FRC_Result APP_FRC_Set_314_Skyworth_Enter3D(void);
FRC_Result APP_FRC_Set_314_Skyworth_Exit3D(void);
FRC_Result APP_FRC_Set_314_FRCMode(EN_FRC_MODE enFRCMode);
FRC_Result APP_FRC_Set_314_FRC_Demo_OnOff(bool bOnOff);
FRC_Result APP_FRC_Set_314_SR_OnOff(bool bOnOff);
FRC_Result APP_FRC_Set_314_SR_Demo_OnOff(bool bOnOff);
FRC_Result APP_FRC_Get_314_FWVersion(u32 *u32Version);
#ifdef  __cplusplus
}
#endif

#endif




