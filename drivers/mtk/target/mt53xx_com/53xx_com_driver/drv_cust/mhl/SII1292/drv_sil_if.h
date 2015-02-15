#ifndef _DRV_SIL_IF_H_
#define _DRV_SIL_IF_H_

#include "x_typedef.h"
#include "x_os.h"
#include "x_debug.h"


#ifndef SUPPORT_ARM_CC
#define SUPPORT_ARM_CC
#endif

#ifndef memcpy
#define  memcpy  x_memcpy
#endif

#ifndef memset
#define  memset  x_memset
#endif

#define SIL_DEBUG(lvl,fmt...)

#define SIL_OK 1
#define SIL_FAIL 0

#define SIL_TRUE  TRUE
#define SIL_FALSE FALSE

#ifndef UNUSED
#define UNUSED(x)               (void)x
#endif

//#define GPIO0_DET_MHLmode

#if !defined(GPIO0_DET_MHLmode)
#define PWR5V_DET_MHLmode
#endif

#if !defined(GPIO0_DET_MHLmode)
#define DETECT_OCP	//enable for detection GPIO0 ; diable for no detection GPIO0
#endif

#define TMDS_MAPPING_For_Cp1292//1292pin22pin23#D2+/-, 1292pin24pin25#D1+/-, 1292pin26pin27#D0+/-, 1292pin28pin29#Clk+/-
#ifndef TMDS_MAPPING_For_Cp1292
#define TMDS_MAPPING_For_Rk1292//1292pin22pin23#Clk-/+, 1292pin24pin25#D0-/+, 1292pin26pin27#D1-/+, 1292pin28pin29#D2-/+
#endif

//Keno11MAY2012
//propose: HDMI CTS8-7 fail on 27MHz, disable EQ to fix.
#define EyeDigm_Magnification

#ifdef EyeDigm_Magnification
//#define DebugMode_1//Page0Reg0x13=0x80 for HDMI, Page0Reg0x13=0x01 for MHL
#ifndef DebugMode_1
#define DebugMode_2//Page0Reg0x13=0x80 for HDMI/MHL
#endif
#endif

#if 1
#define PsCtrl_High //PS_CTRL# is keep high voltage on normal. power switch EN is controlled by MHL_CBL_DET or other pin, and PS_CTRL# can toggle EN.
#else
#define PsCtrl_Low //PS_CTRL# is keep low voltage on normal. power switch EN is controlled by MHL_CBL_DET or other pin, and PS_CTRL# can toggle EN.
#endif

//#if 1
#define VbusCtrl_By_SoC
//#endif

#define SIL_DEV_PAGE_0 0xD4
#define SIL_DEV_PAGE_8 0xC4
#define SIL_DEV_PAGE_C 0xCC

//extern uint32_t g_pass;
extern  UINT8     g_halMsgLevel;

//DEVICE CAPABILITY
#define 	DEVICE_STATE				0x00//Address = 0x00
//#define	MHL_VERSION				(MHL_VER_MAJOR | MHL_VER_MINOR)//Address = 0x01 <-Defined on SI_CbusInitDevCap()
//#define	MHL_DEVICE_CATEGORY//Address = 0x02 <-Defined on SI_CbusInitDevCap()
#define 	ADOPTER_ID_H				0x01
#define	ADOPTER_ID_L				0x43
#define	VID_LINK_MODE				SUPP_RGB444|SUPP_YCBCR444|SUPP_YCBCR422|SUPP_ISLANDS|SUPP_VGA//Address = 0x05
#define	AUD_LINK_MODE				AUD_2CH//Address = 0x06
#define	VIDEO_TYPE					(VT_GRAPHICS | VT_PHOTO | VT_CINEMA | VT_GAME | SUPP_VT)//Address = 0x07
#define	LOG_DEV_MAP              	(LD_DISPLAY)//Address = 0x08
#define	BANDWIDTH					0x0F//Address = 0x09// 0x0F=75 MHz
#define	FEATURE_FLAG				(MHL_RCP_SUPPORT|MHL_RAP_SUPPORT|MHL_SP_SUPPORT)//Address = 0x0A
#define	DEVICE_ID_H				0x00

//modified by zhangyue on 2013-04-08 for meeting sony's spec start
//#define	DEVICE_ID_L				0x02
//modified by zhangyue as required by sony start on 2013-06-18
//#define DEVICE_ID_L				0x01
#define DEVICE_ID_L				0x04
//modified by zhangyue as required by sony end on 2013-06-18

//modified by zhangyue on 2013-04-08 for meeting sony's spec end

#define	SCRATCHPAD_SIZE			16//Address = 0x0D
#define	INT_STAT_SIZE				((MHL_INTERRUPT_SIZE<<4)|MHL_STATUS_SIZE)//Address = 0x0E
#define	Reserved_for_Future_Use	0x00//Address = 0x0F

/*****************************************************************************************/
#define	MHL_DEV_CAT_SINK					0x01
#define	MHL_DEV_CAT_SOURCE				0x02
#define	MHL_DEV_CAT_DONGLE				0x03
#define   	MHL_DEV_CAT_POW					0x10
#define   	MHL_DEV_CAT_500mA					0x00
#define   	MHL_DEV_CAT_900mA					0x20
#define   	MHL_DEV_CAT_1500mA				0x40

#define	SUPP_RGB444						(0x01 << 0)
#define	SUPP_YCBCR444						(0x01 << 1)
#define	SUPP_YCBCR422						(0x01 << 2)
#define	SUPP_PPIXEL						(0x01 << 3)
#define	SUPP_ISLANDS						(0x01 << 4)
#define   	SUPP_VGA							(0x01 << 5)

#define	AUD_2CH							(0x01 << 0)
#define	AUD_8CH							(0x01 << 1)

#define	LD_DISPLAY							(0x01 << 0)
#define	LD_VIDEO							(0x01 << 1)
#define	LD_AUDIO							(0x01 << 2)
#define	LD_MEDIA							(0x01 << 3)
#define	LD_TUNER							(0x01 << 4)
#define	LD_RECORD							(0x01 << 5)
#define	LD_SPEAKER							(0x01 << 6)
#define	LD_GUI								(0x01 << 7)

#define 	MHL_RCP_SUPPORT					(0x01 << 0)
#define 	MHL_RAP_SUPPORT					(0x01 << 1)
#define 	MHL_SP_SUPPORT					(0x01 << 2)

#define	MHL_INTERRUPT_SIZE				3
#define	MHL_STATUS_SIZE					3
/*****************************************************************************************/
//TB: Top and Bottom ; LR: Left and Right ; FS: Frame Sequential
//1292 no support FS

#define Support true
#define Unsupport false

typedef struct
{
    //byte1
    UINT8    EDID_1st_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_1st_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_1st_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_1st_VIC: 1;    	// 1:  support, 0: Not support ; 
    UINT8    EDID_2nd_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_2nd_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_2nd_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_2nd_VIC: 1;    	// 1:  support, 0: Not support
	
    //byte2
    UINT8    EDID_3rd_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_3rd_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_3rd_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_3rd_VIC: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_4th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_4th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_4th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_4th_VIC: 1;    	// 1:  support, 0: Not support
	
    //byte3
    UINT8    EDID_5th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_5th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_5th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_5th_VIC: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_6th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_6th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_6th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_6th_VIC: 1;    	// 1:  support, 0: Not support
	
    //byte4
    UINT8    EDID_7th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_7th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_7th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_7th_VIC: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_8th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_8th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_8th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_8th_VIC: 1;    	// 1:  support, 0: Not support
	
    //byte5
    UINT8    EDID_9th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_9th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_9th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_9th_VIC: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_10th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_10th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_10th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_10th_VIC: 1;    // 1:  support, 0: Not support
	
    //byte6
    UINT8    EDID_11th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_11th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_11th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_11th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_12th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_12th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_12th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_12th_VIC: 1;   // 1:  support, 0: Not support
	
    //byte7
    UINT8    EDID_13th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_13th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_13th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_13th_VIC: 1;   // 1:  support, 0: Not support
    UINT8    EDID_14th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_14th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_14th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_14th_VIC: 1;    // 1:  support, 0: Not support
	
    //byte8
    UINT8    EDID_15th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_15th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_15th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_15th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_16th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_16th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_16th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_16th_VIC: 1;    	// 1:  support, 0: Not support
	
    //byte9
    UINT8    EDID_17th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_17th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_17th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_17th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_18th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_18th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_18th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_18th_VIC: 1;    // 1:  support, 0: Not support
	
    //byte10
    UINT8    EDID_19th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_19th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_19th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_19th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_20th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_20th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_20th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_20th_VIC: 1;   // 1:  support, 0: Not support
	
    //byte11
    UINT8    EDID_21th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_21th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_21th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_21th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_22th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_22th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_22th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_22th_VIC: 1;   // 1:  support, 0: Not support
	
    //byte12
    UINT8    EDID_23th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_23th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_23th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_23th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_24th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_24th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_24th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_24th_VIC: 1;   // 1:  support, 0: Not support
	
    //byte13
    UINT8    EDID_25th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_25th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_25th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_25th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_26th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_26th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_26th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_26th_VIC: 1;    // 1:  support, 0: Not support
	
    //byte14
    UINT8    EDID_27th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_27th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_27th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_27th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_28th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_28th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_28th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_28th_VIC: 1;    // 1:  support, 0: Not support
	
    //byte15
    UINT8    EDID_29th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_29th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_29th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_29th_VIC: 1;    // 1:  support, 0: Not support
    UINT8    EDID_30th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_30th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_30th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_30th_VIC: 1;    // 1:  support, 0: Not support
	
    //byte16
    UINT8    EDID_31th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_31th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_31th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_31th_VIC: 1;   // 1:  support, 0: Not support
    UINT8    EDID_32th_VIC_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_32th_VIC_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_32th_VIC_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_32th_VIC: 1;    // 1:  support, 0: Not support
} MHL_3D_VIC_Structuration_T;

typedef struct
{
    //byte1
    UINT8    EDID_1st_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_1st_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_1st_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_1st_DTD: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_2nd_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_2nd_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_2nd_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_2nd_DTD: 1;    // 1:  support, 0: Not support
    
    //byte2
    UINT8    EDID_3rd_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_3rd_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_3rd_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_3rd_DTD: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_4th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_4th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_4th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_4th_DTD: 1;    	// 1:  support, 0: Not support
    
    //byte3
    UINT8    EDID_5th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_5th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_5th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_5th_DTD: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_6th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_6th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_6th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_6th_DTD: 1;    // 1:  support, 0: Not support
    
    //byte4
    UINT8    EDID_7th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_7th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_7th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_7th_DTD: 1;    	// 1:  support, 0: Not support
    UINT8    EDID_8th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_8th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_8th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_8th_DTD: 1;    // 1:  support, 0: Not support
    
    //byte5
    UINT8    EDID_9th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_9th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_9th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_9th_DTD: 1;    // 1:  support, 0: Not support
    UINT8    EDID_10th_DTD_TB: 1;	// 1:  support, 0: Not support
    UINT8    EDID_10th_DTD_LR: 1;	// 1:  support, 0: Not support
    UINT8    EDID_10th_DTD_FS: 1;	// 1:  support, 0: Not support
    UINT8    EDID_10th_DTD: 1;   // 1:  support, 0: Not support
} MHL_3D_DTD_Structuration_T;

enum eSIL_CHIP_ID
{
  SIL_DEVICE_NONE,
  SIL_DEVICE_9181A,
  SIL_DEVICE_9185,
  SIL_DEVICE_9189,
  SIL_DEVICE_9287,  
  SIL_DEVICE_9381A,
  SIL_DEVICE_9387,
  SIL_DEVICE_9387A,  
  SIL_DEVICE_9389,  
  SIL_DEVICE_9489,  
  SIL_DEVICE_1292,
  SIL_DEVICE_MAX
};

enum eSIL_PORT_ID
{
  SIL_PORT_NONE,
  SIL_PORT_1,
  SIL_PORT_2,
  SIL_PORT_3,
  SIL_PORT_4,
  SIL_PORT_5,
  SIL_PORT_6,
  SIL_PORT_MAX
};

typedef struct
{
    //byte1
    UINT8    SupportEdid: 1;    	//0: no support
    UINT8    SupportHdcp: 1;	//0: no support
    UINT8    SupportMhl: 1;    	// 1:  support, 0: Not support
    UINT8    HdcpKeyGood: 1;	//0: no support
    UINT8    EdidKeyGood: 1;	//0: no support
    UINT8    Reserved5: 1;    	// reserved
    UINT8    Reserved6: 1;    	// reserved
    UINT8    Reserved7: 1;    	// reserved
    //byte2
    UINT8    RxCableDetected: 1;// 1 = MHL connect ; 0 = HDMI connect
    UINT8    RxSync: 1;  		//0: no support
    UINT8    RxStable: 1;  		//0: no support
    UINT8    HdcpActive: 1;  		//0: no support
    UINT8    MhlHdmiDevice: 8;   //bit4: existence device or not, bit0: MHL/Hdmi device (MHL:1, HDMI:0)
    UINT8    HdmiMode: 1;  		//0: no support
    UINT8    Reserved14: 1;  	// reserved
    UINT8    Reserved15: 1;		// reserved
    //byte3
    UINT8    TxCableDetected : 1;	// 1 is Hdmi Tx side cable existence, 0 is Hdmi Tx side cable not existence
    UINT8    TxOutputing: 1;		//0: no support
    UINT8    TxBlackMute: 1;	//0: no support
    UINT8    PowerSaving: 1;    	//0: no support
    UINT8    Reserved20: 1;   	// reserved
    UINT8    Reserved21: 1;    	// reserved
    UINT8    Reserved22: 1;    	// reserved
    UINT8    Reserved23: 1;   	// reserved
    //byte4
    UINT8    Reserved24: 1;    	// reserved
    UINT8    Reserved25: 1;    	// reserved
    UINT8    Reserved26: 1;    	// reserved
    UINT8    Reserved27: 1;    	// reserved
    UINT8    Reserved28: 1;    	// reserved
    UINT8    Reserved29: 1;    	// reserved
    UINT8    Reserved30: 1;    	// reserved
    UINT8    Reserved31: 1;    	// reserved
}   HDMI_SIL_STATUS_T;

typedef struct
{
    //byte0: DevCap0x00 to 0x07
    UINT8    DevCapState_DEVICE_STATE: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_MHL_VERSION: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_MHL_DEVICE_CATEGORY: 1; 	//0=no avail ; 1=avail
    UINT8    DevCapState_ADOPTER_ID_H: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_ADOPTER_ID_L: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_VID_LINK_MODE: 1;    	//0=no avail ; 1=avail
    UINT8    DevCapState_AUD_LINK_MODE: 1;    	//0=no avail ; 1=avail
    UINT8    DevCapState_VIDEO_TYPE: 1;    		//0=no avail ; 1=avail

	//byte1: DevCap0x08 to 0x0F
    UINT8    DevCapState_LOG_DEV_MAP: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_BANDWIDTH: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_FEATURE_FLAG: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_DEVICE_ID_H: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_DEVICE_ID_L: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_SCRATCHPAD_SIZE: 1;    	//0=no avail ; 1=avail
    UINT8    DevCapState_INT_STAT_SIZE: 1;    		//0=no avail ; 1=avail
    UINT8    DevCapState_Reserved_for_Future_Use: 1;	//0=no avail ; 1=avail

    UINT8    DevCapValue_DEVICE_STATE: 8;			//byte2
    UINT8    DevCapValue_MHL_VERSION: 8;			//byte3
    UINT8    DevCapValue_MHL_DEVICE_CATEGORY: 8;	//byte4
    UINT8    DevCapValue_ADOPTER_ID_H: 8;			//byte5
    UINT8    DevCapValue_ADOPTER_ID_L: 8;			//byte6
    UINT8    DevCapValue_VID_LINK_MODE: 8;		//byte7
    UINT8    DevCapValue_AUD_LINK_MODE: 8;		//byte8
    UINT8    DevCapValue_VIDEO_TYPE: 8;			//byte9

    UINT8    DevCapValue_LOG_DEV_MAP: 8;			//byte10
    UINT8    DevCapValue_BANDWIDTH: 8;			//byte11
    UINT8    DevCapValue_FEATURE_FLAG: 8;			//byte12
    UINT8    DevCapValue_DEVICE_ID_H: 8;			//byte13
    UINT8    DevCapValue_DEVICE_ID_L: 8;			//byte11
    UINT8    DevCapValue_SCRATCHPAD_SIZE: 8;		//byte12
    UINT8    DevCapValue_INT_STAT_SIZE: 8;			//byte13
    UINT8    DevCapValue_Reserved_for_Future_Use: 8;//byte14
}HDMI_SIL_DevCap_STATUS_T;

typedef struct
{
  UINT8    major;     // ¡§major¡¨.x.x
  UINT8    second;   //x.¡¨second¡¨.x
  UINT8    third;        //x.x.¡¨third¡¨
} HDMI_SIL_FW_VER_T;

typedef struct
{
  UINT8 MscMsg;
  UINT8 Opcode;
  UINT8 Size;
  UINT8 Data[16];
} CBUS_MSG_DESCRIPTION_T;

// init
BOOL fgSil_Init(void);

//main loop
void vSil_MainLoop(void);

/************************************************************/
//SoC Uses
#define MHL_RAP_NOERROR	   0x00
#define MHL_RAP_UNRECOGNIZED 0x01
#define MHL_RAP_UNSUPPORTED  0x02
#define MHL_RAP_BUSY		   0x03
typedef void (*x_cbus_drv_nfy_data_arrival) (CBUS_MSG_DESCRIPTION_T* msg);
typedef void (*x_cbus_drv_nfy_tx_result) (UINT8 result, void* txtag);

typedef void (*x_vbus_drv_nfy_ocp_status) (UINT8 port, UINT8 result);

void vCbus_NotifyTxResult(UINT8 result, void *tag);
void vCbus_NotifyRxResult(CBUS_MSG_DESCRIPTION_T *msg);

void vCbus_SetRxCallback(x_cbus_drv_nfy_data_arrival nfyfp);
void vCbus_SetTxCallback(x_cbus_drv_nfy_tx_result nfyfp);

void MHL_SetOCPCallback(x_vbus_drv_nfy_ocp_status nfyfp);
BOOL fgVbus_GetOcpStatus(UINT8 port);

void _CheckMhlStatus(UINT8 val);
UINT8 _IsMhlDevice(void);
UINT8 bSendRcpKeys(UINT8 Key, UINT8 event);

// hardwaare reset
BOOL fgSil_ChipRst(UINT8 Polarity, UINT32 MicroSecond);   // 1: normal high, 0: normal low

UINT8 u1MhlGetOcpStatus(UINT8 ports);
void vMhlInit(void);
void vMhlGo(void);
UINT8 u1MhlGetCableStatus(UINT8 ports);
UINT8 u1MhlGetPortId(UINT8 ports);

void vMhlSetSourceStatus(UINT8 port, UINT8 status);
void vMhlSetRapOnOff(UINT8 port, UINT8 OnOff);
UINT8 fgSil_GetSystemPortStatus(UINT8 port);//ON will return 0x00 ; OFF will return0x02 ; BUSY will return0x03
/************************************************************/

//stop Silicon Image chipset driver
VOID fgSil_MHL_Enable_MLoop(BOOL bEnterMloop);

// get chip ID
BOOL fgSil_Get_Chip_ID(HDMI_SIL_FW_VER_T *prSilFwVer);

// firmware version
BOOL fgSil_Get_FW_Version(HDMI_SIL_FW_VER_T *prSilVersion);

// status
BOOL fgSil_GetStatus(UINT8 port, HDMI_SIL_STATUS_T *prSilStatus);

// status
//BOOL fgSil_PortSel(UINT8 sel);

// SIF Function
BOOL fgSil_Sif_WriteBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount);
BOOL fgSil_Sif_ReadBytes(UINT8 DeviceID, UINT8 Offset, UINT8 *pData, UINT8 ByteCount);

// timer
BOOL fgSil_Timer_Set(UINT8 Index, UINT32 MilliSecond);  // in ms
UINT32 u4Sil_Timer_Get(UINT8 Index);     // Return Value=Timer Count Value
BOOL fgSil_Timer_Expired(UINT8 Index);    // 0=Timer Out, 0!=Timer Keep Going
void vSil_Delay1ms(UINT32 ms);    // Delay in millisecond
void vSil_Delay1us(UINT32 us);   // Delay in microsecond


// hardwaare reset
//BOOL fgSil_ChipRst(UINT8 Polarity, UINT32 MicroSecond);   // 1: normal high, 0: normal low

BOOL fgSil_NFYRcp(UINT8 key);//nofity RCP:: Select:0x00...etc.

BOOL fgSil_RcpKeys(UINT8 key);

BOOL fgSil_NFYRAP(UINT8 key);//notify RAP:: POLL:0x00, CONTENT_ON:0x10, CONTENT_OFF: 0x11

BOOL fgSil_RapKeys(UINT8 key);

BOOL fgSil_NFYRAPK(UINT8 key);//notify to SoC when received RAPK:: Driver put 0x00 (NoError) into (UINT8)key ; Driver put 0x01 (UnRecongnized) into (UINT8)key ; Driver put 0x02 (keyUnsupported) into (UINT8)key ; Driver put 0x03 (Busy) into (UINT8)key.

BOOL fgSil_NFYWriteBurstData(UINT8 key, UINT8 *pdata, UINT8 size);//notify MSC:: MSC_WRITE_BURST = 0x6c ...etc.

BOOL fgSil_SendWriteBurstData(UINT8 key, UINT8 *pdata, UINT8 size);//notify MSC:: MSC_WRITE_BURST = 0x6c ...etc.

void fgSil_PrintAllRegs(void);

#ifdef DETECT_OCP
BOOL fgSil_NFYOCPStatus(UINT8 port, UINT8 status);//notify GPIO0 status:: 0x01: GPIO0=1, 0x00: GPIO0=0. For Over Current Protaction.
BOOL fgSil_GetOCPStatus(UINT8 port); // OCP: true, Non-OCP: false.

//Set PS_CTRL to 0 or 1. For control VBUS power out or not.
//Control Confition: If current is MHL mode and OCP(GPIO0) was happened event.
BOOL fgSil_SetVBus(UINT8 value);
#endif

BOOL fgSil_NTF_MSC_CMD(UINT8 port, UINT8 MscCmd, UINT8 OpCode, UINT8 Data);

//Keno20120608, let SoC to decide MHL toggle time, like HPD toggle
//Rlease mean PS_CTRL# low & CBUS Hi-Z & MHL termination discable
//Recovery mean PS_CTRL high & CBUS default 1kohm & MHL termination enable
BOOL fgSil_MHLSignalControl(UINT8 HiZ, UINT8 TermOff, UINT8 VbusDisable);

//added by zhangyue on 2013-03-04 for meeting sony's power seq start
//------------------------------------------------------------------------------
// Function:    fgSil_Enable_Termination
// Description: @bOn True  :  Termination On
//              @bOn False  :  Termination Off

//------------------------------------------------------------------------------

VOID fgSil_Enable_Termination(BOOL bOn);
//------------------------------------------------------------------------------
// Function:	fgSil_Enable_Cbus

// Description: @bOn True  :  Enable Cbus default 1k On
//              @bOn False :  Diable Cbus Hi-Z
//------------------------------------------------------------------------------
	
VOID fgSil_Enable_Cbus(BOOL bOn);

//added by zhangyue on 2013-03-04 for meeting sony's power seq end



BOOL fgSil_Set_MHL2_State( BOOL MHL2_Support,BOOL Support_Charger_900mA_Or_1500mA);
BOOL fgSil_Set_3D_State(BOOL support);//when system initially, SoC calls fgSil_Set_3D_State for desiction DUT supports 3D or not.
BOOL SI_3D_EDID_Configuration_VIC_DTD(MHL_3D_VIC_Structuration_T *p3D_VIC_Struct, MHL_3D_DTD_Structuration_T *p3D_DTD_Struct);

void fgSil_NTFDevCap(HDMI_SIL_DevCap_STATUS_T *pHDMI_SIL_DevCap_STATUS);//notify Device Capability for SoC know.

void fgSil_NTF_CBUS_Connected(void);

void fgSil_NTF_CBUS_Disconnected(void);

void fgSil_NTF_3D_Communicated(void);

void fgSil_NTF_HWRST_1292(void);

#ifdef VbusCtrl_By_SoC
void fgSil_NTF_VBUS_High(void);
void fgSil_NTF_VBUS_Low(void);
#endif
#endif                                  
