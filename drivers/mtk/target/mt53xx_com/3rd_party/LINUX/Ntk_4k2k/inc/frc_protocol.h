
/*! 
********************************************************************************
*  Copyright (c) Novatek Microelectronics Corp., Ltd. All Rights Reserved. 
*  \file    frc_protocol.h
*  \brief 
*  \version ver0.3
*  \date  2012/10/9
*  \chip    NT72314
******************************************************************************** 
*/


#ifndef _FRC_PROTOCOL_H
#define _FRC_PROTOCOL_H

typedef enum _EN_GPIO_LEVEL
{
	EN_GPIO_LEVEL_LOW			=0,			
	EN_GPIO_LEVEL_HIGH			=1,
} EN_GPIO_LEVEL;


////////////////////////////////////////////////////
///////////////    FRC register address     /////////////////
////////////////////////////////////////////////////
#define FRC_REG_GET_VERSION				0x0000FFF0
#define FRC_REG_CMD_QUEUE_ADDRESS		0x9F0A0120
#define FRC_REG_CMD_QUEUE_STATUS		0x9F0A0124
#define FRC_REG_CMD_READ_VALUE			0x9F0A0128



typedef struct  ST_FRC_CMD_FORMAT
{
	u8		u1ReadCmd: 1;
	u8		u1Reserved: 1;
	u8		u6CmdMainType: 6; 
	u8		u8CmdSubType;
	u16		u16CmdValue;		
} ST_FRC_CMD_FORMAT_t;


////////////////////////////////////////////////////////
/** Main Command Type */
#if 0
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

    EN_CMD_MAIN_CUSTOM 				= 0x0F,

	EN_CMD_MAIN_TPVISION			= 0x10,
    EN_CMD_MAIN_TPVISION2			= 0x11,
	
} EN_FRC_CMD_MAIN_TYPE_k; 


/** Sub Command Type and Value */
/////////////////////////////
///////////////   general sub type
/////////////////////////////
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
} EN_FRC_CMD_SUB_GENERAL_TYPE_k;

typedef enum
{	
	EN_BLEND_OPTION_NOVIDEO_OSD_REPLACE_BACKGROUND  = 0,		
	EN_BLEND_OPTION_NOVIDEO_OSD_BLEND_BACKGROUND,		
}EN_BLEND_OPTION_BACKGROUND_TYPE;

typedef enum
{
	EN_BLEND_OPTION_ZEROALPHA_TRANSPARENT_OSD  = 0,		
	EN_BLEND_OPTION_ZEROALPHA_OPAQUE_OSD,		
}EN_BLEND_OPTION_ZEROALPHA_TYPE;

typedef enum
{
	EN_BLEND_OPTION_OSDDATA_PURE_DATA  = 0,		
	EN_BLEND_OPTION_OSDDATA_MULTIPLIED_ALPHA,		
}EN_BLEND_OPTION_OSDDATA_TYPE;

typedef enum
{
	EN_BLEND_OPTION_OUTPUT_NORMAL_BLEND_DATA  = 0,		
	EN_BLEND_OPTION_OUTPUT_PURE_OSD,		
	EN_BLEND_OPTION_OUTPUT_PURE_OSD_ALPHA_DATA,		//debug only	
	EN_BLEND_OPTION_OUTPUT_PURE_VIDEO,		
}EN_BLEND_OPTION_OUTPUT_TYPE;



//////////////////////////////////////////////////////////////

/////////////////////////////
///////////////   frc sub type
/////////////////////////////
typedef enum
{
	EN_CMD_SUB_MEMC_MODE  						= 0x00,
    EN_CMD_SUB_MEMC_DEJUDDER_LEVEL				= 0x01,
    EN_CMD_SUB_MEMC_DEBLUR_LEVEL				= 0x02,
    EN_CMD_SUB_MEMC_LR_SWAP						= 0x03,
	EN_CMD_SUB_MEMC_DEPTH_LEVEL			        = 0x04,
    EN_CMD_SUB_MEMC_DISPARTY_LEVEL				= 0x07,  

    //Advance MEMC control
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_SENSITIVITY   = 0x80,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_SPEED,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_JUDDER,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_MAXJUDDER,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_MINJUDDER,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_BLUR,
    EN_CMD_SUB_MEMC_VIDEO_BREAKUP_MAXBLUR,

    EN_CMD_SUB_MEMC_FILM22_BREAKUP_SENSITIVITY,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_SPEED,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_JUDDER,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_MAXJUDDER,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_MINJUDDER,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_BLUR,
    EN_CMD_SUB_MEMC_FILM22_BREAKUP_MAXBLUR,

    EN_CMD_SUB_MEMC_FILM32_BREAKUP_SENSITIVITY,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_SPEED,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_JUDDER,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_MAXJUDDER,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_MINJUDDER,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_BLUR,
    EN_CMD_SUB_MEMC_FILM32_BREAKUP_MAXBLUR,

    EN_CMD_SUB_MEMC_VIDEO_HALO_SENSITIVITY,
    EN_CMD_SUB_MEMC_VIDEO_HALO_SPEED,
    EN_CMD_SUB_MEMC_VIDEO_HALO_JUDDER,
    EN_CMD_SUB_MEMC_VIDEO_HALO_MAXJUDDER,
    EN_CMD_SUB_MEMC_VIDEO_HALO_MINJUDDER,
    EN_CMD_SUB_MEMC_VIDEO_HALO_BLUR,
    EN_CMD_SUB_MEMC_VIDEO_HALO_MAXBLUR,

    EN_CMD_SUB_MEMC_FILM22_HALO_SENSITIVITY,
    EN_CMD_SUB_MEMC_FILM22_HALO_SPEED,
    EN_CMD_SUB_MEMC_FILM22_HALO_JUDDER,
    EN_CMD_SUB_MEMC_FILM22_HALO_MAXJUDDER,
    EN_CMD_SUB_MEMC_FILM22_HALO_MINJUDDER,
    EN_CMD_SUB_MEMC_FILM22_HALO_BLUR,
    EN_CMD_SUB_MEMC_FILM22_HALO_MAXBLUR,

    EN_CMD_SUB_MEMC_FILM32_HALO_SENSITIVITY,
    EN_CMD_SUB_MEMC_FILM32_HALO_SPEED,
    EN_CMD_SUB_MEMC_FILM32_HALO_JUDDER,
    EN_CMD_SUB_MEMC_FILM32_HALO_MAXJUDDER,
    EN_CMD_SUB_MEMC_FILM32_HALO_MINJUDDER,
    EN_CMD_SUB_MEMC_FILM32_HALO_BLUR,
    EN_CMD_SUB_MEMC_FILM32_HALO_MAXBLUR,

    EN_CMD_SUB_MEMC_HALO_REDUCTION,

    EN_CMD_SUB_MEMC_FILM_MOTION_BLUR_REDUCTION_SENSITIVITY,
    EN_CMD_SUB_MEMC_FILM_MOTION_BLUR_REDUCTION_SPEED,
    EN_CMD_SUB_MEMC_SET_FORCE_LEVEL,
} EN_FRC_CMD_SUB_FRC_TYPE_k;

typedef enum _EN_FRC_MODE
{
	EN_MEMC_OFF  = 0,
	EN_MEMC_STANDARD,
	EN_MEMC_SMOOTH,
	EN_MEMC_CLEAR,
	EN_MEMC_CUSTOM,
	EN_MEMC_DEMO,
} EN_FRC_MODE;

//DEJUDDER_LEVEL & DEBLUR_LEVEL value range: 0 - 10
//DEPTH_LEVEL & DISPARTY_LEVEL value range:    0 - 10
//////////////////////////////////////////////////////////////


/////////////////////////////
///////////////   format sub type
/////////////////////////////
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
} EN_FRC_CMD_SUB_FORMAT_TYPE_k; 

typedef enum _EN_FORMAT_OUTPUT_MODE
{
	EN_FORMAT_VD_MODE  = 0, 
	EN_FORMAT_PC_MODE,
	EN_FORMAT_INVALID_MODE,
} EN_FORMAT_OUTPUT_MODE;

typedef enum _EN_FORMAT_INPUT_FORMAT
{
	EN_FORMAT_IN_2D  = 0,
	EN_FORMAT_IN_3D_LR,
	EN_FORMAT_IN_3D_TB,
	EN_FORMAT_IN_3D_SBS,
	EN_FORMAT_IN_3D_FP,    //frame packing
	EN_FORMAT_IN_3D_LBL,  //line by line
}EN_FORMAT_INPUT_FORMAT;

typedef enum _EN_FORMAT_OUTPUT_FORMAT
{
	EN_FORMAT_OUT_2D  = 0, 
	EN_FORMAT_OUT_3D_PR,
	EN_FORMAT_OUT_3D_SG,
	EN_FORMAT_OUT_3D_SG_BB,	//Bottom Black
	EN_FORMAT_OUT_3D_PR_SIMPLE,
	EN_OUT_3D_PR_AUO,
} EN_FORMAT_OUTPUT_FORMAT;

typedef enum _EN_FORMAT_INPUT_FRAME_RATE
{
	EN_FORMAT_IN_50HZ  = 0,		// 0
	EN_FORMAT_IN_60HZ,		// 1
	EN_FORMAT_IN_24HZ,		// 2
	EN_FORMAT_IN_25HZ,		// 3
	EN_FORMAT_IN_48HZ,		// 4
	EN_FORMAT_IN_100HZ,		// 5
	EN_FORMAT_IN_120HZ,		// 6
	EN_FORMAT_IN_30HZ		// 7
}EN_FORMAT_INPUT_FRAME_RATE;

typedef enum _EN_FORMAT_OUTPUT_FRAME_RATE
{
	EN_OUT_50HZ  = 0,
	EN_OUT_100HZ,
	EN_OUT_200HZ,
	EN_OUT_60HZ,
	EN_OUT_120HZ,
	EN_OUT_240HZ,
	EN_OUT_75HZ,
	EN_OUT_96HZ,
	EN_OUT_48HZ,
	EN_OUT_72HZ,	
	EN_OUT_24HZ,
	EN_OUT_25HZ,
	EN_OUT_30HZ,
	EN_OUT_144HZ,
}EN_FORMAT_OUTPUT_FRAME_RATE;

typedef enum _EN_INPUT_RESOLUTION
{
	EN_INPUT_1920X1080		=0,
	EN_INPUT_1280X720			=1,
	EN_INPUT_1366X768			=2,
	EN_INPUT_960X1080			=3,
	EN_INPUT_1920X540			=4,

	EN_INPUT_3840X2160		=16,
	EN_INPUT_3840X1080		=17,
	EN_INPUT_1920X2160		=18,
	EN_INPUT_4096X2160		=19,
	EN_INPUT_4096X1080		=20,
	EN_INPUT_2048X2160		=21,
} EN_INPUT_RESOLUTION;
//////////////////////////////////////////////////////////////


/////////////////////////////
///////////////   White Balance sub type
/////////////////////////////
typedef enum
{
	EN_CMD_SUB_WB_EN					= 0x00,
    EN_CMD_SUB_WB_R_GAIN				= 0x01,
    EN_CMD_SUB_WB_G_GAIN				= 0x02,
    EN_CMD_SUB_WB_B_GAIN				= 0x03,
    EN_CMD_SUB_WB_R_OFFSET				= 0x04,
    EN_CMD_SUB_WB_G_OFFSET				= 0x05,
    EN_CMD_SUB_WB_B_OFFSET				= 0x06,
} EN_FRC_CMD_SUB_WB_TYPE_k; 

//value range: 0 - 1024
///////////////////////////////////////////////////////////////////////


/////////////////////////////
///////////////   LVDS sub type
/////////////////////////////
typedef enum
{
	EN_CMD_SUB_LVDS_ONOFF   = 0,
	EN_CMD_SUB_LVDS_RX_BITS,   
	EN_CMD_SUB_LVDS_RX_TIMODE,
	EN_CMD_SUB_LVDS_RX_PORTAB_SWAP,
	EN_CMD_SUB_LVDS_DP_4PX_OUT_SWAP,
	EN_CMD_SUB_LVDS_DP_PORTAB_SWAP,
	EN_CMD_SUB_LVDS_DP_PORTCD_SWAP,
	EN_CMD_SUB_LVDS_DP_PORTBC_SWAP,

	//Vx1 control
	EN_CMD_SUB_VX1_TWO_TCON_MODE		= 0x10,
}EN_FRC_CMD_SUB_LVDS_TYPE_k;

//Config LVDS TX & RX by API 
///////////////////////////////////////////////////////////////////////


/////////////////////////////
///////////////   PWM sub type
/////////////////////////////
typedef enum
{
	EN_CMD_SUB_PWM_REVERSE				= 0x00,
	EN_CMD_SUB_PWM_2D_DUTY				= 0x01,
	EN_CMD_SUB_PWM_2D_DELAY				= 0x02,
	EN_CMD_SUB_PWM_3D_DUTY				= 0x03,
	EN_CMD_SUB_PWM_3D_DELAY				= 0x04,
	//----------------------------------------
	EN_CMD_SUB_BT_IR_DUTY				= 0x20,
	EN_CMD_SUB_BT_IR_DELAY				= 0x21,
	EN_CMD_SUB_BT_LR_DUTY				= 0x22,
	EN_CMD_SUB_BT_LR_DELAY				= 0x23,
	EN_CMD_SUB_BT_VSYNC_DUTY			= 0x24,
	EN_CMD_SUB_BT_VSYNC_DELAY			= 0x25,
 } EN_FRC_CMD_SUB_PWM_TYPE_k;

//DUTY & DELAY value range: 0 - 100, 0 is lowest, 100 is highest
///////////////////////////////////////////////////////////////////////


/////////////////////////////
///////////////   SSC sub type
/////////////////////////////
typedef enum
{
	EN_CMD_SUB_SSC_LVDS_EN				= 0x00,
    EN_CMD_SUB_SSC_LVDS_PERCENT			= 0x01,
    EN_CMD_SUB_SSC_LVDS_FREQ			= 0x02,
    EN_CMD_SUB_SSC_DDR_EN				= 0x03,
    EN_CMD_SUB_SSC_DDR_PERCENT			= 0x04,
    EN_CMD_SUB_SSC_DDR_FREQ				= 0x05,
    EN_CMD_SUB_SSC_VX1_EN				= 0x06,
    EN_CMD_SUB_SSC_VX1_PERCENT			= 0x07,
    EN_CMD_SUB_SSC_VX1_FREQ				= 0x08,
} EN_FRC_CMD_SUB_SSC_TYPE_k;
///////////////////////////////////////////////////////////////////////


/////////////////////////////
///////////////   Demo window sub type
/////////////////////////////
typedef enum
{ 
	EN_CMD_SUB_WINDOW_SETID	= 0,
	EN_CMD_SUB_WINDOW_ONOFF,
	EN_CMD_SUB_WINDOW_HEIGHT,
	EN_CMD_SUB_WINDOW_WIDTH,
	EN_CMD_SUB_WINDOW_X_POSITION,
	EN_CMD_SUB_WINDOW_Y_POSITION,
	EN_CMD_SUB_WINDOW_DEMO_MODE,
} EN_FRC_CMD_SUB_WINDOW_TYPE_k;

typedef enum
{
	EN_WINDOW_DEMO_MODE_OFF  = 0,
	EN_WINDOW_DEMO_MODE_RIGHT,
	EN_WINDOW_DEMO_MODE_LEFT,
}EN_WINDOW_DEMO_MODE_k;


/////////////////////////////
///////////////   Panel sub type
/////////////////////////////
typedef enum
{
    EN_CMD_SUB_PANEL_RESOLUTION			= 0x00,
    EN_CMD_SUB_PANEL_VACTIVE			= 0x01,
    EN_CMD_SUB_PANEL_HACTIVE			= 0x02,
    EN_CMD_SUB_PANEL_VTOTAL_MAX_STEP 	= 0x03,
    EN_CMD_SUB_PANEL_HTOTAL_MAX_STEP	= 0x04,

    //each output frequecy has individual below setting
    EN_CMD_SUB_PANEL_VTOTAL				= 0x10,
    EN_CMD_SUB_PANEL_HTOTAL				= 0x11,
    EN_CMD_SUB_PANEL_VTOTAL_MIN			= 0x12,
    EN_CMD_SUB_PANEL_HTOTAL_MIN			= 0x13,
    EN_CMD_SUB_PANEL_VTOTAL_MAX			= 0x14,
    EN_CMD_SUB_PANEL_HTOTAL_MAX			= 0x15,
    EN_CMD_SUB_PANEL_V_BACKPORCH		= 0x16,
    EN_CMD_SUB_PANEL_H_BACKPORCH		= 0x17,
    EN_CMD_SUB_PANEL_VSYNC_WIDTH		= 0x18,
    EN_CMD_SUB_PANEL_HSYNC_WIDTH		= 0x19,
    EN_CMD_SUB_PANEL_VSYNC_POLARITY		= 0x1A,
    EN_CMD_SUB_PANEL_HSYNC_POLARITY		= 0x1B,
    EN_CMD_SUB_PANEL_DE_POLARITY		= 0x1C,
    EN_CMD_SUB_PANEL_VSYNC_EN			= 0x1D,
    EN_CMD_SUB_PANEL_HSYNC_EN			= 0x1E,
    EN_CMD_SUB_PANEL_DE_EN				= 0x1F,

	EN_CMD_SUB_PANEL_STORE_TO_FLASH		= 0x80,
} EN_FRC_CMD_SUB_PANEL_TYPE_k;

typedef enum _EN_PANEL_RESOLUTION_TYPE
{
	RESOLUTION_1920x1080  = 0,	
	RESOLUTION_2560x1080,
	RESOLUTION_4Kx2K,
	RESOLUTION_4Kx1K,
	RESOLUTION_2Kx2K,
} EN_PANEL_RESOLUTION_TYPE;


/////////////////////////////
///////////////   LDC sub type
/////////////////////////////
typedef enum
{
    EN_CMD_SUB_LDC_ONOFF  = 0,
    EN_CMD_SUB_LDC_LEVEL,
    EN_CMD_SUB_DUTY_RESPONSE_TIME,
    EN_CMD_SUB_DUTY_HALO_LEVEL,
    EN_CMD_SUB_COMPENSATION_LEVEL,
} EN_FRC_CMD_SUB_LDC_TYPE_k;


/////////////////////////////
///////////////   OSD sub type
/////////////////////////////
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
} EN_FRC_CMD_SUB_OSD_TYPE_k; 

typedef enum _EN_OSD_Input_Format
{
	EN_OSD_IN_2D  =0,
	EN_OSD_IN_3D_LR,
	EN_OSD_IN_3D_TB,
	EN_OSD_IN_3D_SBS,
	EN_OSD_IN_3D_LBL,
	EN_OSD_IN_4K2K_2_HORIZONTAL_FRAME,
	EN_OSD_IN_4K2K_2_VERTICAL_FRAME,
	EN_OSD_IN_4K2K_4_FRAME
}EN_OSD_Input_Format;

typedef enum _EN_OSD_Output_Format
{
	EN_OSD_OUT_2D=0,
	EN_OSD_OUT_3D_PR,
	EN_OSD_OUT_3D_SG,
	EN_OSD_OUT_3D_LLRR
}EN_OSD_Output_Format;

typedef enum _EN_OSD_PixelFormat_8BIT
{
	EN_PF_8BIT  = 0,
	EN_PF_10BIT
}EN_OSD_PixelFormat_8Bit;


typedef enum _EN_OSD_PixelFormat_444
{
	EN_PF_444  = 0,
	EN_PF_442
}EN_OSD_PixelFormat_444;

typedef enum _EN_OSD_PixelFormat_RGB
{
	EN_PF_RGB  = 0,
	EN_PF_YCC
}EN_OSD_PixelFormat_RGB;

typedef enum _EN_OSD_PixelFormat_PerAlpha
{
	EN_PF_PER_ALPHA  = 0,
	EN_PF_MAU_ALPHA
}EN_OSD_PixelFormat_PerAlpha;

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#endif



// FRC internal Cmd queue status
typedef enum
{
	EN_FRC_CMD_QUEUE_EMPTY 			= 1,
	EN_FRC_CMD_QUEUE_FULL 			= (1<<1),
	EN_FRC_CMD_QUEUE_PENDING 		= (1<<2),
	EN_FRC_CMD_QUEUE_SUSPEND 		= (1<<3),		//for DSU debug
}EN_FRC_CMD_QUEUE_STATUS_k;



/*!
 * \fn FRC_Result API_FRC_Init(void)
 * \brief
 *  Platform FRC Initialize (HAL IIC initialize)
 *
 * \param none
 *
 * \retval FRC_OK        	 	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 *
 */
FRC_Result API_FRC_Init(void);


/*!
 * \fn FRC_Result API_FRC_IsInit(void)
 * \brief
 *  Platform FRC is initialize or not
 *
 * \param none
 *
 * \retval FRC_OK        	 	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 *
 */
bool API_FRC_IsInit(void);


	
/*!
 * \fn FRC_Result API_FRC_UnLock(void)
 * \brief
 *  Set  FRC UnLock
 *
 * \param none
 *
 * \retval FRC_OK        	 	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 *
 */
FRC_Result API_FRC_UnLock(void);


/*!
 * \fn FRC_Result API_FRC_Get_Version(u32 *u32Versiont)
 * \brief
 *  Get FRC Version Number
 *
 * \param *u32Version       	[IN]  FRC Version Number
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 *
 */
FRC_Result API_FRC_Get_Version(u32 *u32Version);


/*!
 * \fn FRC_Result API_FRC_ReadReg(u32 u32RegAddr, u32 *pu32Val)
 * \brief
 *  Read Reg of 312
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 */
FRC_Result API_FRC_ReadReg(u32 u32RegAddr, u32 *pu32Val);


/*!
 * \fn FRC_Result API_FRC_WriteReg(u32 u32RegAddr, u32 pu32Val)
 * \brief
 *  Write Reg of 312
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 */
FRC_Result API_FRC_WriteReg(u32 u32RegAddr, u32 pu32Val);


/*!
 * \fn FRC_Result API_FRC_WriteCmd(u8 u8MainType, u8 u8SubType, u32 u32Value, bool bWaitCmdDone)
 * \brief
 *  Write a control cmd to FRC cmd queue in order to achieve some function
 *
 * \param u8MainType       	[IN]  Main type, refer to EN_FRC_CMD_MAIN_TYPE_k enum
 * \param u8SubType      	 	[IN]  Sub type, refer to each SUB_TYPE enum
 * \param u32Value      	 	[IN]  give value you want to set
 * \param bWaitCmdDone      	[IN]  wait all sent cmds execute done if true, else return immediately if false
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 */
FRC_Result API_FRC_WriteCmd(u8 u8MainType, u8 u8SubType, u32 u32Value, bool bWaitCmdDone);
	

/*!
 * \fn FRC_Result API_FRC_ReadCmd(u8 u8MainType, u8 u8SubType, u32* pu32Value)
 * \brief
 *  Write a read cmd to FRC cmd queue in order to get the cmd value
 *
 * \param u8MainType       	[IN]  Main type, refer to EN_FRC_CMD_MAIN_TYPE_k enum
 * \param u8SubType      	 	[IN]  Sub type, refer to each SUB_TYPE enum
 * \param pu32Value      	 	[out] return the cmd value
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 */
FRC_Result API_FRC_ReadCmd(u8 u8MainType, u8 u8SubType, u32* pu32Value);


/*!
 * \fn FRC_Result API_FRC_Update_OnePackage(void)
 * \brief
 *  Update_OnePackage by calling API_FRC_FW_UpdateProc
 *
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 *
 */
FRC_Result API_FRC_Update_OnePackage(char *Path);


/*!
 * \fn FRC_Result API_FRC_FW_UpdateProc(FILE *fp);
 * \brief
 *  Burning new FW
 *
 * \retval FRC_OK         	Operation Success
 * \retval FRC_NOT_OK        Operation Failure
 */
//FRC_Result API_FRC_FW_UpdateProc(FILE *fp);
FRC_Result API_FRC_FW_UpdateFrimware(char filepath[]);

FRC_Result HAL_FRC_Reset(void);



#endif // _HAL_FRC_H
