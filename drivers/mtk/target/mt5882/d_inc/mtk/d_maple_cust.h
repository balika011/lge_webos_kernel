/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2004, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $RCSfile: d_sony_2k12_cust.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *---------------------------------------------------------------------------*/

#ifndef _D_SONY_2K12_CUST_H_
#define _D_SONY_2K12_CUST_H_

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "u_common.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/
/* TV Source */
#define UART_WAKEUP_SUB_REASON_TV_CH_MIN 0
#define UART_WAKEUP_SUB_REASON_TV_CH_MAX 199

/* AV Source */
#define UART_WAKEUP_SUB_REASON_AV_START  UART_WAKEUP_SUB_REASON_CVBS_1
#define UART_WAKEUP_SUB_REASON_CVBS_1    200
#define UART_WAKEUP_SUB_REASON_CVBS_2    201
#define UART_WAKEUP_SUB_REASON_CVBS_3    202
#define UART_WAKEUP_SUB_REASON_CVBS_4    203
#define UART_WAKEUP_SUB_REASON_HDMI_1    204
#define UART_WAKEUP_SUB_REASON_HDMI_2    205
#define UART_WAKEUP_SUB_REASON_HDMI_3    206
#define UART_WAKEUP_SUB_REASON_HDMI_4    207
#define UART_WAKEUP_SUB_REASON_YPBPR_1   208
#define UART_WAKEUP_SUB_REASON_YPBPR_2   209
#define UART_WAKEUP_SUB_REASON_YPBPR_3   210
#define UART_WAKEUP_SUB_REASON_YPBPR_4   211
#define UART_WAKEUP_SUB_REASON_PC_1      212
#define UART_WAKEUP_SUB_REASON_AV_END    UART_WAKEUP_SUB_REASON_PC_1

/* PCB configuration */
#define DRV_CUSTOM_MODEL_60HZ_PANEL                 (0)
#define DRV_CUSTOM_MODEL_120HZ_PANEL                (1 << 0)
#define DRV_CUSTOM_MODEL_AUD_NORMAL                 (0 << 1)
#define DRV_CUSTOM_MODEL_AUD_5_1_CHANEL             (1 << 1)
#define DRV_CUSTOM_MODEL_FACTORY_WISTRON            (0 << 2)
#define DRV_CUSTOM_MODEL_FACTORY_SONY               (1 << 2)
#define DRV_CUSTOM_MODEL_WXGA_PANEL                 (0 << 3)
#define DRV_CUSTOM_MODEL_FHD_PANEL                  (1 << 3)
#define DRV_CUSTOM_MODEL_CCFL_PANEL                 (0 << 4)
#define DRV_CUSTOM_MODEL_LED_PANEL                  (1 << 4)
#define DRV_CUSTOM_MODEL_NO_DVD                     (0 << 5)
#define DRV_CUSTOM_MODEL_WITH_DVD                   (1 << 5)
#define DRV_CUSTOM_MODEL_AUD_NO_WOOFER              (0 << 6)
#define DRV_CUSTOM_MODEL_AUD_WOOFER                 (1 << 6)
#define DRV_CUSTOM_MODEL_1H			                (0 << 7)
#define DRV_CUSTOM_MODEL_2H			                (1 << 7)

#define DRV_CUSTOM_MODEL_NO_RTC                     (0 << 9)
#define DRV_CUSTOM_MODEL_WITH_RTC                   (1 << 9)
#define DRV_CUSTOM_LOGO_ACTIVE                      (1 << 0)
#define DRV_CUSTOM_LOGO_DISABLE                     (0 << 0)
#define DRV_CUSTOM_LOGO_MUSIC_ACTIVE                (1 << 1)
#define DRV_CUSTOM_LOGO_MUSIC_DISABLE               (0 << 1)

// Country code
#define DRV_CUSTOM_COUNTRY_EU                       0
#define DRV_CUSTOM_COUNTRY_PAA                      1
#define DRV_CUSTOM_COUNTRY_PAD                      2
#define DRV_CUSTOM_COUNTRY_COL                      3
#define DRV_CUSTOM_COUNTRY_US                       4
#define DRV_CUSTOM_COUNTRY_UNKOWN                   0xFFFFFFFF

#define DRV_CUSTOM_COUNTRY_OPTION_PAA               0
#define DRV_CUSTOM_COUNTRY_OPTION_EU                4
#define DRV_CUSTOM_COUNTRY_OPTION_PAD               5
#define DRV_CUSTOM_COUNTRY_OPTION_COL               7

#define DRV_CUSTOM_MONITOR_OUT_GPIO                 (24)
#ifndef WAFFLE_PCB_PROTO
#define DRV_CUSTOM_MONITOR_OUT_POLARITY             (1)
#define DRV_CUSTOM_CVBS_IN_POLARITY                 (0)
#else
#define DRV_CUSTOM_MONITOR_OUT_POLARITY             (0)
#define DRV_CUSTOM_CVBS_IN_POLARITY                 (1)
#endif

/* follow AP option bit definition */
#define DRV_CUSTOM_MODELID_REGION      (0x6C7)
#define DRV_CUSTOM_MODELID_DVD         (0x6CA)
#if 1//cox 110829 moduify
typedef struct DRV_CUSTOM_LIGHT_SENSOR_TYPE_T
{
	UINT8 u1CHannelID;
	UINT16 u2ClkDiv;
	UINT8 u1DevAddr;
	UINT8 u1WordAddrNum;
	UINT32 u4WordAddr;
	UINT8 *pu1Buf;
	UINT16 u2ByteCnt;
}   DRV_CUSTOM_LIGHT_SENSOR_TYPE_T;
typedef struct _DRV_CUSTOM_IRRX_TYPE_T
{
    UINT16 ui1_channel;
    UINT32 ui4_irrx_read;
}   DRV_CUSTOM_IRRX_TYPE_T;

typedef struct _DRV_CUSTOM_GAMMA_WB_TYPE_T
{
    BOOL ui1_gamma_onoff;                   
}   DRV_CUSTOM_GAMMA_WB_TYPE_T;
typedef struct _DRV_CUSTOM_HDMI_5V_TYPE_T
{
    UINT8 ui1_hdmi_comp_id;                   /* port*/
    UINT8 ui1_5V_status;						//
}   DRV_CUSTOM_HDMI_5V_TYPE_T;
typedef struct _DRV_CUSTOM_GPIO_INPUT_TYPE_T
{
    UINT8 ui1_gpio_input_id;                   /* port*/
    UINT8 ui1_gpio_input_status;						//
}   DRV_CUSTOM_GPIO_INPUT_TYPE_T;
typedef struct _DRV_CUSTOM_GPIO_OUTPUT_TYPE_T
{
    INT32 ui1_gpio_output_id;                   /* port*/
    INT32* ui1_gpio_output_status;						//
}   DRV_CUSTOM_GPIO_OUTPUT_TYPE_T;
#endif
typedef struct _DRV_CUSTOM_GAMMA_REFRESH_T
{
	UINT8  ui1_OnOff;	/*Input*/
}   DRV_CUSTOM_GAMMA_REFRESH_T;
typedef struct _DRV_CUSTOM_ADC_RGB_GAIN_OFFSET__T
{
	UINT16  ui1_r_gain;		/*Input*/
	UINT16  ui1_g_gain;	/*Input*/
	UINT16  ui1_b_gain;	/*Input*/
	UINT16  ui1_r_offset;	/*Input*/
	UINT16  ui1_g_offset;	/*Input*/
	UINT16  ui1_b_offset;	/*Input*/
}   _DRV_CUSTOM_ADC_RGB_GAIN_OFFSET__T;

/*typedef enum
{
    MD_QPSK = 0,
	MD_16QAM,
	MD_64QAM,
	MD_256QAM,
	MD_QPSK_R,
	MD_16QAM_R,
	MD_64QAM_R,
	MD_256QAM_R,
	MD_UNKNOWN
} DRV_CUSTOM_CUST_T2_MODULATION_MODE_TYPE_T;*/

typedef enum
{
    DRV_CFFT_1K_N = 0,
	DRV_CFFT_1K_E,
	DRV_CFFT_2K_N,
	DRV_CFFT_2K_E,
	DRV_CFFT_4K_N,
	DRV_CFFT_4K_E,
	DRV_CFFT_8K_N,
	DRV_CFFT_8K_E,
	DRV_CFFT_16K_N,
	DRV_CFFT_16K_E,
	DRV_CFFT_32K_N,
	DRV_CFFT_32K_E,
	DRV_CFFT_XK_UNKNOWN
	
} DRV_CUSTOM_CUST_T2_CARRIER_FFT_MODE_TYPE_T;

/*typedef enum
{
    SISO = 0,
	MISO,
	XIXO_UNKNOWN
	
} DRV_CUSTOM_CUST_T2_TRANSMISSION_MODE_TYPE_T;*/

/*typedef enum
{
    PP1= 0,
	PP2,
	PP3,
	PP4,
	PP5,
	PP6,
	PP7,
	PP8,
	PPX_UNKNOWN
	
} DRV_CUSTOM_CUST_T2_PILOT_PATTERN_MODE_TYPE_T;*/


/*typedef enum
{
    PAPR_NONE= 0,
	PAPR_ACE,
	PAPR_TR,
	PAPR_BOTH,
	PAPR_UNKNOWN
	
} DRV_CUSTOM_CUST_T2_PAPR_MODE_TYPE_T;*/


/*typedef enum
{
    FEF_YES = 0,
	FEF_NO
	
} DRV_CUSTOM_CUST_T2_FEF_MODE_TYPE_T;*/

/*typedef enum
{
    LDPC_SHORT = 0,
	LDPC_NORMAL,
	LDPC_UNKNOWN
	
} DRV_CUSTOM_CUST_T2_FEC_MODE_TYPE_T;*/

/*typedef enum
{
    FECH_HEM = 0,
	FECH_NORMAL
	
} DRV_CUSTOM_CUST_T2_FECHEADER_MODE_TYPE_T;*/

/*typedef enum
{
    L1_BPSK = 0,
	L1_QPSK,
	L1_16QAM,
	L1_64QAM,
	L1_UNKNOWN
	
} DRV_CUSTOM_CUST_T2_L1MODULATION_MODE_TYPE_T;*/

typedef struct _DRV_CUSTOM_TUNER_DVB_T2_T
{
	//DRV_CUSTOM_CUST_T2_MODULATION_MODE_TYPE_T  eModulation;
	DRV_CUSTOM_CUST_T2_CARRIER_FFT_MODE_TYPE_T eCarrierFFT;
	//DRV_CUSTOM_CUST_T2_TRANSMISSION_MODE_TYPE_T	eTransmisson;
	//DRV_CUSTOM_CUST_T2_PILOT_PATTERN_MODE_TYPE_T ePilotPattern;
	//DRV_CUSTOM_CUST_T2_PAPR_MODE_TYPE_T ePapr;
	//DRV_CUSTOM_CUST_T2_FEF_MODE_TYPE_T eFef;
	UINT8   ui1_plp_num;      /* the number of PLPs. */	
	UINT8   ui1_CurrPlpId;
	//DRV_CUSTOM_CUST_T2_FEC_MODE_TYPE_T eFecMode;
	//DRV_CUSTOM_CUST_T2_FECHEADER_MODE_TYPE_T eFecHeaderMode;
	//DRV_CUSTOM_CUST_T2_L1MODULATION_MODE_TYPE_T eL1Modulation;

}   DRV_CUSTOM_TUNER_DVB_T2_T;

typedef enum
{
    DRV_CUSTOM_MODEL_OPTION_E = 0,
    DRV_CUSTOM_MODEL_OPTION_E_PLUS,
    DRV_CUSTOM_MODEL_OPTION_L,
    DRV_CUSTOM_MODEL_OPTION_P,
    DRV_CUSTOM_MODEL_OPTION_C,
    DRV_CUSTOM_MODEL_OPTION_F1,
    DRV_CUSTOM_MODEL_OPTION_F1S,
    DRV_CUSTOM_MODEL_OPTION_LAST_VALID_ENTRY,
} DRV_CUSTOM_CUST_MODEL_TYPE_T;

typedef enum
{
    DRV_CUSTOM_CUST_SPEC_TYPE_BEGIN = 0,

    DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_PATH,
    DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_PICTURE_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_CINE_MOTION_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_GRAPHIC_AREA,
    DRV_CUSTOM_CUST_SPEC_TYPE_DCC,
    DRV_CUSTOM_CUST_SPEC_TYPE_MASK_AREA,
    DRV_CUSTOM_CUST_SPEC_TYPE_INPUT_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_MICRO_P_NVM_UPGRADE,

    DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_READ_REGISTER,
    DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_WRITE_REGISTER,
    DRV_CUSTOM_CUST_SPEC_TYPE_FACTORY_EXC_COMMAND,

    DRV_CUSTOM_CUST_SPEC_TYPE_PCB_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_NUM,
    DRV_CUSTOM_CUST_SPEC_TYPE_LED_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_BRIGHTNESS_PLUS,
    DRV_CUSTOM_CUST_SPEC_TYPE_STANDBY_LED_DISABLE_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_SELF_DIAG_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_BOOT_COUNT_MODE,
    DRV_CUSTOM_CUST_SPEC_TYPE_LOADER_UPGRADE_MODE,

    DRV_CUSTOM_CUST_SPEC_TYPE_LOGO_STATE,
    DRV_CUSTOM_CUST_SPEC_TYPE_LOGO_DISPLAY,
    DRV_CUSTOM_CUST_SPEC_GET_TAG_VERSION,
    DRV_CUSTOM_CUST_SPEC_TYPE_BACKLIGHT_STATUS,
	DRV_CUSTOM_CUST_SPEC_TYPE_SYNC_MENU_FLAG,
    #if 1//cox 110829 modify
	DRV_CUSTOM_CUST_SPEC_SET_GAMMA_REFRESH,
	DRV_CUSTOM_CUST_SPEC_GET_DEEPCOLOR,
	DRV_CUSTOM_CUST_SPEC_GET_ADC_RGB_GAIN_OFFSET,
	DRV_CUSTOM_CUST_SPEC_GET_LIGHT_SENSOR_STATS,
	DRV_CUSTOM_CUST_SPEC_GET_IRRX,
	DRV_CUSTOM_CUST_SPEC_SET_WB_GAMMA_ORIGINAL,
	DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_POWER5V_STATUS,
	DRV_CUSTOM_CUST_SPEC_TYPE_GPIO_INPUT_STATUS,
	DRV_CUSTOM_CUST_SPEC_TYPE_GPIO_OUTPUT_STATUS,
	DRV_CUSTOM_CUST_SPEC_TYPE_GET_HDMI_STATUS,
	DRV_CUSTOM_SPEC_TYPE_DBC_DEBUG,
	DRV_CUSTOM_SPEC_TYPE_YAPL,
        DRV_CUSTOM_SPEC_TYPE_MHL_OCP_STATUS,	/*20121114 Cox*/
        DRV_CUSTOM_SPEC_TYPE_MHL_HDMI_STATUS,
	#endif
    DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_EDID,
    DRV_CUSTOM_CUST_SPEC_TYPE_HDMI_HDCP,  //jiewen.hdcp
    DRV_CUSTOM_CUST_SPEC_TYPE_MONITOR_OUT,
    DRV_CUSTOM_CUST_SPEC_TYPE_STARTUP_SOUND_STATUS,   /* Allen Kao: wait for start-up sound playback over */
    DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_48,
    DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_50,
    DRV_CUSTOM_CUST_SPEC_GET_DIMMING_FREQ_60,

    DRV_CUSTOM_CUST_SPEC_TYPE_SS_PERMILLAGE,
    DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_CIS,
    DRV_CUSTOM_CUST_SPEC_TYPE_PCB_TYPE,

    /*Customer specific command start*/
    DRV_CUSTOM_CUST_SPEC_TYPE_MMP_MODE,                     /* Ramesh: Set USB Photo/Music/Video/None */
    DRV_CUSTOM_CUST_SPEC_TYPE_MMP_OSD_STATE,                /* Ramesh: Set MMP OSD State */
    DRV_CUSTOM_CUST_SPEC_TYPE_MMP_PHOTO_FRAME_MODE,			/* set photo frame mode state*/
    DRV_CUSTOM_CUST_SPEC_TYPE_OSD_TYPE,                     /* Ramesh: Set Full Screen OSD YES/NO */
//#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY   /* Not Used in ATSC Project */  
    DRV_CUSTOM_CUST_SPEC_TYPE_FM_RADIO_STATE,               /* Ramesh: Set FM Radio ON/OFF */
//#endif
    DRV_CUSTOM_CUST_SPEC_TYPE_AUTOMAN_SCAN_STATE,           /* Ramesh: Set auto tuning status */
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY   /* Not Used in ATSC Project */
    DRV_CUSTOM_CUST_SPEC_TYPE_TELETEXT_MODE,                /* Ramesh: Set Teletext ON/OFF */
#endif
    DRV_CUSTOM_CUST_SPEC_TYPE_SCENE_SELECT_MODE,            /* Ramesh: Set scene select mode */
    DRV_CUSTOM_CUST_SPEC_TYPE_MOTIONFLOW_PATTERN,           /* SY: Generate Motion Flow Status */
/* Start: Hotel Clock */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_TIME,                 /* Liew: Set/Get time into Hotel Clock */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_CFG,                  /* Liew: Configure Hotel Clock */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_MODE,                 /* Liew: Query Hotel Clock module's mode */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_CLK_VERSION,              /* Liew: Query Hotel Clock module's SW version */
/* End: Hotel Clock */
    DRV_CUSTOM_CUST_SPEC_TYPE_TOUCH_SENSOR_READ,            /*Added 5 March 2012 Touch Sensor*/
    DRV_CUSTOM_CUST_SPEC_GET_SONY_LIB_VERSION,              /* SY: Read Sony Library Version */
    DRV_CUSTOM_CUST_SPEC_GET_TEMP_SENSOR_VALUE,             /* SY: Read Temp Sensor Temperature Value */
    DRV_CUSTOM_CUST_SPEC_SET_INTERNAL_SIG_PTN,				      /* Marina: Internal Signal Pattern ( 31 July 2009)*/
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY   /* Not Used in ATSC Project */ 
    DRV_CUSTOM_CUST_SPEC_GET_SIGNAL_LEVEL,					        /* Azlan : Get signal level value (03/08/2009) */
#endif
    /*Tsen - ADC*/
    DRV_CUSTOM_CUST_SPEC_GET_A_GAIN,                        /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_D_GAIN,                        /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_R_OFFSET,                      /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_G_OFFSET,                      /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_B_OFFSET,                      /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_R_GAIN,                        /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_G_GAIN,                        /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_B_GAIN,                        /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_CVBS_TARGET,                   /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL1_TARGET,          /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL2_TARGET,          /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_COMP_CHANNEL3_TARGET,          /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_EFUSE_GAIN,                    /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_YPBPR_VGA_MAX_LVL,                 /* Tsen : AD Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_YPBPR_VGA_BLANK_LVL,               /* Tsen : AD Adjustment for Sony factory */
    /*Tsen - ADC*/
#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY   /* Not Used in ATSC Project */ 
    /* Ooi Start - IP TH */
    DRV_CUSTOM_CUST_SPEC_SET_IP_NOISE_TH,                   /* Ooi : IP Noise threshold to NVM  */
    DRV_CUSTOM_CUST_SPEC_GET_IP_NOISE_TH,                   /* Ooi : IP Noise threshold from NVM  */
    /* Ooi END */
#endif
    /* Ooi Start - SCREEN (11/8/09) */
    DRV_CUSTOM_CUST_SPEC_SET_SOURCE_MAIN,                   /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_SOURCE_MAIN,                   /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_SOURCE_SUB,                    /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_SOURCE_SUB,                    /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_OVERSCAN_MAIN,                 /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_OVERSCAN_MAIN,                 /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_OVERSCAN_SUB,                  /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_OVERSCAN_SUB,                  /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_DISPLAY_MAIN,                  /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_DISPLAY_MAIN,                  /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_DISPLAY_SUB,                   /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_DISPLAY_SUB,                   /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_NONLINEAR_PARAM,               /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_NONLINEAR_SLOPE,               /* Ooi : Screen test for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_NONLINEAR_ENDFACTOR,           /* Ooi : Screen test for Sony factory */
    /* Ooi END */

#ifdef THIRD_PARTY_LIB_EMCS_ATV_ONLY   /* Not Used in ATSC Project */ 
    DRV_CUSTOM_CUST_SPEC_GET_TUNER_REG,                     /* Chow : Get tuner register for factory application */
    DRV_CUSTOM_CUST_SPEC_SET_TUNER_AGC,                     /* Chow : Tuner registers to NVM */
    DRV_CUSTOM_CUST_SPEC_SET_CIS_AGC,                       /* Chow : Tuner registers to CIS case */
    DRV_CUSTOM_CUST_SPEC_SET_INT_PIC_SIG_BSTR,              /* Chow : Intelligent Booster and Signal Booster relation */
#endif    
    DRV_CUSTOM_CUST_SPEC_GET_DRV_SREG,                      /* Ramesh : Get value Factory app from Sony driver */
    DRV_CUSTOM_CUST_SPEC_SET_DRV_SREG,                      /* Ramesh : Set value Factory app to Sony driver */

    /*>>> START Cheah: Sony GammaWB callback func 07Sep09 */
    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SETUP,                   /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_GAMMA_SETUP,
    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SAVE,                    /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_QUIT,                    /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_EXIT,                    /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_RESET,                   /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_SAMPLE,                  /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_SETUP,                      /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_SAVE,                       /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_QUIT,                       /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_EXIT,                       /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_RESET,                      /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_SETLVL,                     /* Cheah : Gamma Adjustment for Sony factory */
    DRV_CUSTOM_CUST_SPEC_SET_WB_SETCLR,                     /* Cheah : Gamma Adjustment for Sony factory */
    /*<<< END Cheah: Sony GammaWB callback func 07Sep09 */
    
#ifndef THIRD_PARTY_LIB_EMCS_ATV_ONLY   /* For IRIS only */
    DRV_CUSTOM_CUST_SPEC_GET_DYN_GAMMA_EN,                  /* Cheah : IRIS Dynamic Gamma control */
	DRV_CUSTOM_CUST_SPEC_SET_DYN_GAMMA_EN,					/* Cheah : IRIS Dynamic Gamma control */
#endif

    DRV_CUSTOM_CUST_SPEC_SET_GAMMA_PAT_GEN,                   /* Lim : Set test pattern for Sony factory */
    DRV_CUSTOM_CUST_SPEC_GET_COLOR_LEVEL,                     /* Marina : Get RGB level at a specific point */


    DRV_CUSTOM_CUST_SPEC_GET_PANEL_INCH_SIZE,                /* YiFen 021009 Read Panel Size for product information*/

    DRV_CUSTOM_CUST_SPEC_SET_AUDIO_INTERDAC_MUTE,            /* Gallen : For Ibiza internal dac mute control  */

    /* Hotel IR Pass Through */
    DRV_CUSTOM_CUST_SPEC_SET_HTL_IR_PASS_THROUGH,           /* Set IR Pass Through of hotel mode */

/* Aizul Start: Hotel Volume */    
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_MODE,                 /* Aizul: Set/Query Hotel Volume module's mode */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_INITVOL,              /* Aizul: Set/Query Hotel Volume initial volume */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_STEPTIMER,            /* Aizul: Set/Query Hotel Volume step timer */
    DRV_CUSTOM_CUST_SPEC_TYPE_HTL_VOL_VOLLVL,               /* Aizul: Set/Query Hotel Volume volume level */       
/* Aizul End: Hotel Volume */
    DRV_CUSTOM_CUST_SPEC_SET_RGB_GAISOU,        /* Ramesh: Set RGB sensor gaisou for SONY factory and service menu */
    DRV_CUSTOM_CUST_SPEC_GET_RGB_GAISOU,        /* Ramesh: Set RGB sensor gaisou for SONY factory and service menu */
    DRV_CUSTOM_CUST_SPEC_TYPE_DEMO_MODE_USB_VIDEO_MODE,     /* Ramesh: Set for USB demo mode video */

/* Aizul Start: Hotel SI */    
    DRV_CUSTOM_CUST_SPEC_TYPE_HTLSI_ONLINEMODE,                 /* Aizul: Set/Query Hotel SI module's online mode */
/* Aizul End: Hotel SI */    
    DRV_CUSTOM_CUST_SPEC_TYPE_DITHER, /* Hew: Dither on/off 29Oct2009*/

    DRV_CUSTOM_CUST_SPEC_RESET_IRIS_MUTE_PIN,   /*Lim 9nov09: Control IRIS mute pin*/

    DRV_CUSTOM_CUST_SPEC_TYPE_APP_COUNTRY,
    DRV_CUSTOM_CUST_SPEC_TYPE_OSD_STATUS_CHANGE,

    /* VGA wakeup on BGM mode*/
    DRV_CUSTOM_CUST_SPEC_TYPE_VGA_DETECT_WAKEUP,
    DRV_CUSTOM_CUST_SPEC_TYPE_VGA_STATUS,

    DRV_CUSTOM_CUST_SPEC_TYPE_PANEL_ID,

    /* Model ID from AP eeprom */
    DRV_CUSTOM_CUST_SPEC_TYPE_DVD_ENABLE,
    DRV_CUSTOM_CUST_SPEC_TYPE_PCB_REGION,

    /* Signal Diagnostics Status */
    DRV_CUSTOM_CUST_SPEC_TYPE_ATV_STATUS, /*[LINUX] FIXED! NOT IMPLEMENTED YET!*/
	DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_SET,
	DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_GET_PRESET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_PBM_PQP_PGRAM_LUT_GET_BY_ELEMENT,

    DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBT_IDX,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_ADJ_METHOD,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_EASY_SET_LVL,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_EASY_SET_EN,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_Y_SUM_POS,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_Y_SUM_MEAS,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_POS,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_Y,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_CB,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_PIXEL_MEAS_CR,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_ON,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBT_IDX_CHANGE_STS,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_R,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_G,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_INP_EASY_SET_B,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R00,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R01,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R02,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R03,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R04,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R05,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R06,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R07,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R08,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R09,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R10,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R11,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R12,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R13,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R14,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R15,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R16,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R17,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R18,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R19,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R20,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R21,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R22,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R23,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R24,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R25,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R26,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R27,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R28,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R29,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R30,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R31,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_R32,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G00,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G01,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G02,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G03,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G04,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G05,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G06,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G07,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G08,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G09,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G10,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G11,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G12,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G13,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G14,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G15,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G16,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G17,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G18,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G19,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G20,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G21,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G22,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G23,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G24,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G25,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G26,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G27,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G28,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G29,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G30,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G31,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_G32,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B00,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B01,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B02,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B03,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B04,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B05,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B06,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B07,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B08,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B09,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B10,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B11,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B12,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B13,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B14,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B15,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B16,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B17,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B18,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B19,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B20,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B21,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B22,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B23,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B24,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B25,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B26,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B27,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B28,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B29,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B30,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B31,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_LUT_ADJ_B32,
    DRV_CUSTOM_CUST_SPEC_TYPE_AGING_LAST_POWER_CONTROL,
    DRV_CUSTOM_CUST_SPEC_TYPE_DMPLL_SPECTRUM_PERMILLAGE,
    DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_LNA,
    DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_DVB_T2_STATUS,
    DRV_CUSTOM_VID_TYPE_LED_MOTION,
    DRV_CUSTOM_CUST_SPEC_TYPE_LED_BLINK_CONTROL,
    DRV_CUSTOM_CUST_SPEC_TYPE_TEST_PTN_RAMP,
    DRV_CUSTOM_CUST_SPEC_TYPE_MOVE_OS_OPTION_POS,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_PRESET_BEFORE_GET_LVL,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_SET_LVL,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_SET_SELECT_LVL,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_GET_LVL,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_GET_SELECT_LVL,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_SETUP_STATUS_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_PTN_REFRESH,
    DRV_CUSTOM_CUST_SPEC_TYPE_GAMMA_VID_PRESET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_PRESET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_SET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WBS_TO_NVM_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLRIDX_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_WBLVL_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_SETUP_STATUS_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_GAIN_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLR_TEMP_IDX_SET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_CLR_TEMP_IDX_GET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_ACFG_UPDATE_SET,
    DRV_CUSTOM_CUST_SPEC_TYPE_WB_ACFG_UPDATE_GET,    
	DRV_CUSTOM_CUST_SPEC_TYPE_WB_UPDATE_PARM_GET,
	DRV_CUSTOM_CUST_SPEC_TYPE_WB_UPDATE_ADJ_CLR_IDX_GET,

#if 1 //aging ptn related
    DRV_CUSTOM_CUST_SPEC_TYPE_AGING_PTN,
#endif
#if 1 //BL related
    DRV_CUSTOM_CUST_SPEC_TYPE_BACKLIGHT_CONTROL,
#endif
#if 1 //cox 110811
    DRV_CUSTOM_CUST_SPEC_SET_DYN_GAMMA,					/*Write Gamma*///cox 110810 for write gamma
    DRV_CUSTOM_CUST_SPEC_GET_DYN_GAMMA,					/*Write Gamma*///cox 110810 for write gamma
    DRV_CUSTOM_VID_TYPE_GAMMA_REFRESH,						/*trigger gamma*///cox 110812
    DRV_CUSTOM_VID_TYPE_YAPL,									/*YAPL*///cox 110812
    DRV_CUSTOM_CUST_SPEC_ADC_LIGHT_SENSOR_GAIN,
    DRV_CUSTOM_CUST_SPEC_ADC_LIGHT_SENSOR_OFFSET,
#endif
    /*maple be api*/
    DRV_CUSTOM_CUST_SPEC_SET_BEPQ_INP_SRC_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BEPQ_INP_SRC_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BEPQ_PIC_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BEPQ_PIC_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BEPQ_SCREEN_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BEPQ_SCREEN_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BEPQ_3D_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BEPQ_3D_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BEPQ_ECO_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BEPQ_ECO_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BEPQ_SERVICE_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BEPQ_SERVICE_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BESYS_HISTORY_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BESYS_HISTORY_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_SET_BESYS_POWER_STATE_INFO_FROM_AP,
    DRV_CUSTOM_CUST_SPEC_GET_BESYS_POWER_STATE_INFO_FROM_AP,
    /*maple be api*/
    DRV_CUSTOM_CUST_SPEC_TYPE_POP_CVBS_COMP_NOTIFY,  
    DRV_CUSTOM_CUST_SPEC_EPG_FLAG,
	DRV_CUSTOM_CUST_SPEC_DBC_DEBUG,
	DRV_CUSTOM_CUST_SPEC_GAMMA_BUF,
    DRV_CUSTOM_CUST_SPEC_OSD_ON_OFF,
	DRV_CUSTOM_CUST_SPEC_TYPE_ALL_TOUCH_KEY_ENA,
	DRV_CUSTOM_CUST_SPEC_TYPE_TOUCH_SENSOR_RECALIBRATION,
#if 1 /* use in sony interface for sony facotry */
	DRV_CUSTOM_CUST_SPEC_V_COM_DRV,
	DRV_CUSTOM_CUST_SPEC_SCART_DRV,
	DRV_CUSTOM_CUST_SPEC_WB_EDID_WRITE,
	DRV_CUSTOM_CUST_SPEC_MICRO_OPTION_RMVB,
	DRV_CUSTOM_CUST_SPEC_EEP_UPDATE,
	DRV_CUSTOM_CUST_SPEC_WB_EDID_WRITE_STATUS,
#endif
	DRV_CUSTOM_CUST_SPEC_AGING_EEP_UPDATE,
	
    DRV_CUSTOM_CUST_SPEC_BATTERY_MODE,
#if 1 
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_ID,             /* Panel ID */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_NAME,           /* Panel Name */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_BACK_LIGHT_HI,  /* Panel Attribute backlight hi */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_BACK_LIGHT_LOW, /* Panel Attribute backlight low */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_SELECT,         /* Panel ID Selection */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_TOTAL_NUM,      /* Panel Total Number */
    DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_R_GAIN,        /* REG WB R-Gain (Register) */
    DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_G_GAIN,        /* REG WB G-Gain (Register) */
    DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_B_GAIN,        /* REG WB B-Gain (Register) */
    DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_R_OFFSET,      /* REG WB R-Offset (Register) */
    DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_G_OFFSET,      /* REG WB G-Offset (Register) */
    DRV_CUSTOM_CUST_SPEC_DISP_REG_WB_B_OFFSET,      /* REG WB B-Offset (Register) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_MAGIC,         /* EEP WB Magic Number (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_R_GAIN,        /* EEP WB R-Gain (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_G_GAIN,        /* EEP WB G-Gain (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_B_GAIN,        /* EEP WB B-Gain (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_R_OFFSET,      /* EEP WB R-Offset (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_G_OFFSET,      /* EEP WB G-Offset (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_EEP_WB_B_OFFSET,      /* EEP WB B-Offset (EEPROM) */
    DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV0,            /* HDCP KSV 0 */
    DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV1,            /* HDCP KSV 1 */
    DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV2,            /* HDCP KSV 2 */
    DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV3,            /* HDCP KSV 3 */
    DRV_CUSTOM_CUST_SPEC_DISP_HDCP_KSV4,            /* HDCP KSV 4 */
    DRV_CUSTOM_CUST_SPEC_USBUART_STATUS,
    DRV_CUSTOM_CUST_SPEC_DISP_BLUE_STRETCH,         /* Blue Stretch */
    DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_MAGIC_NUM,  /* Dynamic GAMMA Magic number */
    DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA,            /* Dynamic GAMMA */
    DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_RELOAD,     /* Dynamic GAMMA Reload Setting */
    DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_REMAP,      /* Dynamic GAMMA Remap Calculate */
    DRV_CUSTOM_CUST_SPEC_DISP_DYN_GAMMA_LINEAR,     /* Linear GAMMA Remapping */
    DRV_CUSTOM_CUST_SPEC_DISP_SCE_ON_OFF,           /* SCE On/Off */
    DRV_CUSTOM_CUST_SPEC_DISP_DCR_ON_OFF,           /* DCR/Light sensor On/Off */    
    DRV_CUSTOM_CUST_SPEC_DISP_WR_VGA_EDID,          /* Write VGA EDID */ //Soar add 20101007 for VGA EDID
    DRV_CUSTOM_CUST_SPEC_DISP_CVBS_AUTO_COLOR,      /* CVBS Auto Color */
    DRV_CUSTOM_CUST_SPEC_DISP_CVBS_D_GAIN,          /* CVBS D_GAIN */
    DRV_CUSTOM_CUST_SPEC_DISP_CVBS_A_GAIN,          /* CVBS A_GAIN */
    DRV_CUSTOM_CUST_SPEC_DISP_Y_LEVEL,              /* Y Level */
    DRV_CUSTOM_CUST_SPEC_DISP_PQ_VERSION,           /* PQ Version */
    DRV_CUSTOM_CUST_SPEC_DISP_PQ_GAMMA_VER,         /* GAMMA QTY Version */
    DRV_CUSTOM_CUST_SPEC_DISP_PQ_SCE_VER,           /* GAMMA QTY Version */
    DRV_CUSTOM_CUST_SPEC_DISP_PQ_QTY_VER,           /* PQ QTY Version */
    DRV_CUSTOM_CUST_SPEC_DISP_AQ_VERSION,           /* AQ Version */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_VERSION,  /* Panel Flash Version */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_NUMBER,   /* Flash Panel total panel Number */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_FLASH_ID_LIST,  /* List of Panel ID */
    DRV_CUSTOM_CUST_SPEC_SERVO_ADC,                 /* Read Servo ADC value */
    //DRV_CUSTOM_CUST_SPEC_DISP_PANEL_INCH_SIZE,      /* Panel Inch Size */
    DRV_CUSTOM_CUST_SPEC_DISP_PANEL_LUT_SEL,        /* Panel LUT Select */
	
    DRV_CUSTOM_CUST_SPEC_VGA_EDID_DATA,            /* VGA EDID Data */
    DRV_CUSTOM_CUST_SPEC_HDMI_EDID_DATA,           /* HDMI EDID Data */

    DRV_CUSTOM_CUST_SPEC_TUNER_REG,                /* Tuner Register */

    DRV_CUSTOM_CUST_SPEC_SIF,                      /* SIF Read/Write */

    DRV_CUSTOM_CUST_SPEC_COLOR_MATRIX_DEF,         /* Default Color Matrix */
    DRV_CUSTOM_CUST_SPEC_COLOR_MATRIX_PQ,          /* Default Color Matrix */

    DRV_CUSTOM_CUST_SPEC_EEP_RW,                   /* R/W EEPROM */

#endif
#if 1//def STELLAR_LED_SPEC
	DRV_CUSTOM_CUST_SPEC_LED_MODE,
#endif/* STELLAR_LED_SPEC */
    DRV_CUSTOM_CUST_SPEC_UPDATE_LED_FIX_PATTERN,/*d130805_Haibo:To indicate if now is led fixed pattern.LED fixed pattern has the topest priority*/
	
#if 1//def PCMCIA_ENABLED//FAC_CMD_FOR_CI
	DRV_CUSTOM_CUST_SPEC_CI_PLUG_IN,
	DRV_CUSTOM_CUST_SPEC_CI_DESCRAMBLE,
#endif
    DRV_CUSTOM_CUST_SPEC_TYPE_TUNER_LNA_CTRL,
	DRV_CUSTOM_CUST_SPEC_TYPE_DTV_PACKET_RX_STATUS,
	DRV_CUSTOM_CUST_SPEC_TYPE_HDCP2X_RW,
	DRV_CUSTOM_CUST_SPEC_READ_ECC_UNCRRCT,//362
  	DRV_CUSTOM_CUST_SPEC_TYPE_TCON_NVM_WRITE,
	DRV_CUSTOM_CUST_SPEC_TYPE_TCON_NVM_READ,
	DRV_CUSTOM_CUST_SPEC_TYPE_V_COM_DRV,
	DRV_CUSTOM_CUST_SPEC_TYPE_SCART_OUT,
	DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_VOL_CTRL,//367
	DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_TONE_CTRL,//368
	DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_LNB_SHORT_STATUS,//369
	DRV_CUSTOM_CUST_SPEC_TYPE_DVBS_DEMOD_CLK_CTRL,//370
	DRV_CUSTOM_CUST_SPEC_TYPE_DEMOD_MOUNT_STATUS,
	DRV_CUSTOM_CUST_SPEC_TYPE_LNB_MOUNT_STATUS,
        DRV_CUSTOM_CUST_SPEC_TYPE_UPDATE_REC_STATUS,
        DRV_CUSTOM_CUST_SPEC_TYPE_IS_SIGNAL_LOSS,
        DRV_CUSTOM_CUST_SPEC_TYPE_START_LED_BLINK,
        DRV_CUSTOM_CUST_SPEC_TYPE_STOP_LED_BLINK,	
    /*Customer specific command END*/
    DRV_CUSTOM_CUST_SPEC_TYPE_END

} DRV_CUSTOM_CUST_SPEC_TYPE_T;

typedef enum
{
        DRV_CUSTOM_VGA_ENABLE_DETECTION,
        DRV_CUSTOM_VGA_DISABLE_DETECTION
} DRV_CUSTOM_VGA_DETECT_WAKEUP_T;

typedef enum
{
        DRV_CUSTOM_VGA_SIGNAL_STABLE,
        DRV_CUSTOM_VGA_SIGNAL_NO_SIGNAL,
        DRV_CUSTOM_VGA_SIGNAL_NOT_SUPPORT
}DRV_CUSTOM_VGA_STATUS_T;

typedef enum {
	D_DEMO_OFF = 0,
	D_BRAVIA_ENGINE = 1,
	D_MOTION_FLOW = 3,
	NUM_OF_D_DEMO_MODE
} D_DEMO_MODE;

typedef enum {
	D_FORMAT_422 = 0,
	D_FORMAT_444,
	NUM_OF_D_PATH
} D_PATH;

typedef enum {
	D_MOTION_OFF = 0,
	D_MOTION_STANDARD,
	D_MOTION_HIGH,
	NUM_OF_D_MOTIONFLOW_MODE
} D_MOTIONFLOW_MODE;

typedef enum {
	D_PICTURE_VIVID = 0,
	D_PICTURE_STANDARD,
	D_PICTURE_CUSTOM,
	D_PICTURE_CINEMA1,
	D_PICTURE_CINEMA2,
	D_PICTURE_GAME,
	D_PICTURE_TEXT,
	D_PICTURE_SPORTS,
	D_PICTURE_PHOTO_VIVID,
	D_PICTURE_PHOTO_STANDARD,
	D_PICTURE_PHOTO_CUSTOM,
	D_PICTURE_PHOTO_ORIGINAL,
	D_PICTURE_GAME2,
	NUM_OF_D_PICTURE_MODE
} D_PICTURE_MODE;

typedef enum {
	D_CINE_MOTION_OFF = 0,
	D_CINE_MOTION_AUTO2,
	D_CINE_MOTION_AUTO1,
	NUM_OF_D_CINE_MOTION
} D_CINE_MOTION_MODE;

typedef struct _SMicroMaskArea
{
    UINT8 u1Index;
    UINT16 u2Width;
    UINT16 u2Height;
    UINT16 u2X;
    UINT16 u2Y;
} D_SMicroMaskArea;


typedef enum {
	D_INPUT_RF = 0,
	D_INPUT_OTHER,
	NUM_OF_D_INPUT_MODE
} D_INPUT_MODE;

typedef struct _SMicroFactory
{
    UINT32 u4InDataSize;
    UINT32 u4OutDataSize;
    UINT8 *pInData;
    UINT8 *pOutData;
} D_SMicroFactory;

/* Start - Hew: Dither on/off 29Oct2009*/
typedef struct _PARAM_DITHER{
	UINT16 write_data;
	UINT8 lsb_off;
	UINT8 round_en;
	UINT8 rdither_en;
	UINT8 edither_en;
	UINT8 dither_lfsr_en;
}PARAM_DITHER;
/* End - Hew: Dither on/off 29Oct2009*/

typedef enum
{
    D_LED_GENERIC_ON,
    D_LED_GENERIC_OFF,
} D_LED_GENERIC_STATE;

typedef enum
{
    D_LED_POWER_ON,
    D_LED_POWER_OFF
} D_LED_POWER_STATE;

typedef enum
{
    D_LED_STANDBY_ON,
    D_LED_STANDBY_OFF
} D_LED_STANDBY_STATE;

typedef enum
{
    D_LED_TIMER_ORANGE,
    D_LED_TIMER_GREEN,
    D_LED_TIMER_BOTH,
    D_LED_TIMER_OFF,
} D_LED_TIMER_STATE;

typedef enum
{
    D_LED_POWER = 0,
    D_LED_STANDBY,
    D_LED_TIMER,
    D_LED_TIMER_2,
    D_LED_TOTAL
} D_LED_TYPE;

typedef struct _ATVLedDisplay
{
    D_LED_TYPE eLedType;
    UINT32 eLedState;
} D_LED_DISPLAY;

typedef enum
{
    D_SYSTEM_NORMAL,
    D_SYSTEM_PANEL_ONOFF
} D_SYSTEM_STATE;

#if 1
typedef enum
{
    D_12V_AMP_ERROR,
    D_12V_SW_ERROR,
    D_24V_PWR_ERROR,
    D_AUD_ERROR,
    D_3V3_5V_ERROR,
    D_PANEL_POWER_ERROR,
    D_BACKLIGHT_ERROR,
    D_TEMPERATURE_ERROR,
    D_BALANCER_ERROR,    
    D_TCON_ERROR,
    D_PANEL_NVM_ERROR,//atm-liang add        
    D_TUNER_ERROR,
    D_RTC_ERROR,
    D_TOUCHSNR_ERR,
    D_CAR_BATTERY,
    D_ALL_ERROR//Should be put at last (for i4SelfDiagClearCounterCmd())
} D_SELF_DIAG_ERROR_TYPE;

#else
typedef enum
{
    D_12V_SW_ERROR,
    D_PANEL_POWER_ERROR,
    D_BACKLIGHT_ERROR,
    D_TEMPERATURE_ERROR,
    D_AUD_ERROR,
    D_BALANCER_ERROR,
    D_TUNER_ERROR,   
    D_TCON_ERROR,
    D_PANEL_NVM_ERROR,//atm-liang add     
    D_12V_AMP_ERROR,
    D_24V_PWR_ERROR,
    D_3V3_5V_ERROR,
    D_ALL_ERROR//Should be put at last (for i4SelfDiagClearCounterCmd())
} D_SELF_DIAG_ERROR_TYPE;
#endif
typedef enum {
    D_COUNTRY_UNKNOWN = 0,
    D_COUNTRY_GA,
    D_COUNTRY_INDIA,
    D_COUNTRY_PHILIPPINE,
    D_COUNTRY_CIS,
    D_COUNTRY_CHINA,
    D_COUNTRY_LATIN
} APP_COUNTRY_SEL;

typedef struct _DRV_CUSTOM_PANEL_ID_T
{
    CHAR   aui1_panel_id_buf[16];
    UINT16 u2_offset;
    UINT16 u2_length;
} DRV_CUSTOM_PANEL_ID_T;

typedef struct _SelfDiagState
{
    D_SELF_DIAG_ERROR_TYPE eErrorType;
    BOOL   bErrorOccur;
    BOOL   bResetErrorCnt;
    UINT32 u4ErrorCnt;
} D_SELF_DIAG_STATE;

typedef struct DRV_CUSTOM_TAG_VERSION_TYPE_T
{
    UINT8 ui1_tag_type;                      /* tag*/
    CHAR szVerStr[20];						//version string
}   DRV_CUSTOM_TAG_VERSION_TYPE_T;

/* Liew: Hotel Clock related */
typedef struct _DRV_CUSTOM_HTL_CLK_TIME_T
{
    UINT8 ui1_hour;
    UINT8 ui1_minute;
    UINT8 ui1_second;
    UINT8 ui1_day_of_week;
    UINT8 ui1_is_valid;     /* Only used when we get time from hotel clock */
} DRV_CUSTOM_HTL_CLK_TIME_T;
           
typedef struct _DRV_CUSTOM_HTL_CLK_CFG_T
{ 
    UINT8 ui1_display;
    UINT8 ui1_intensity;
    UINT8 ui1_reset;
}  DRV_CUSTOM_HTL_CLK_CFG_T;
/* Liew: End Hotel Clock related */

typedef enum
{
	DRV_CUSTOM_TAG_TYPE_LOADER= 0,    /* loader by luis     */
	DRV_CUSTOM_TAG_TYPE_FIRMWARE,     /* firmware by luis   */
	DRV_CUSTOM_TAG_TYPE_PQ ,	      /* PQ data by norman  */
	DRV_CUSTOM_TAG_TYPE_PANEL,  	  /* panel data by cosh */
	DRV_CUSTOM_TAG_TYPE_AQ ,	      /* AQ data by Andrew  */
	DRV_CUSTOM_TAG_TYPE_UPEEP ,       /* uP nvm by Cosh     */
	DRV_CUSTOM_TAG_TYPE_UPIMG ,	      /* uP image by Cosh   */
	DRV_CUSTOM_TAG_TYPE_NVRAM ,       /* eeprom by Jifeng   */
	DRV_CUSTOM_TAG_TYPE_EDID,  	      /* EDID by CI         */
	DRV_CUSTOM_TAG_TYPE_UPBT,  	      /* uP Bootloader      */
    DRV_CUSTOM_TAG_TYPE_KEYPAD,       /* keypad version     */
    DRV_CUSTOM_TAG_TYPE_MHL,          /* MHL version        */
	DRV_CUSTOM_TAG_TYPE_MAX
}DRV_CUSTOM_TAG_TYPE_T;

typedef enum
{
    D_MMP_NONE = 0,
    D_MMP_PHOTO,
    D_MMP_MUSIC,
    D_MMP_MOVIE,
    D_MMP_PHOTO_FRAME,
    NUM_OF_MMP_MODE
} D_MMP_MODE;

typedef enum
{
    D_MMP_OSD_NONE = 0,
    D_MMP_OSD_THUMBNAIL,
    D_MMP_OSD_DEVICE,
    D_MMP_OSD_PLAYBACK,
    NUM_OF_MMP_OSD_STATE
} D_MMP_OSD_STATE;

typedef enum
{
    D_MMP_OSD_NOT_FULL = 0,
    D_MMP_OSD_FULL,
    NUM_OF_OSD_TYPE
} D_MMP_OSD_TYPE;

typedef enum
{
    D_FM_RADIO_OFF = 0,
    D_FM_RADIO_ON,
    NUM_OF_FM_RADIO_STATE
} D_FM_RADIO_STATE;

typedef enum
{
    D_SYNC_MENU_OFF = 0,
    D_SYNC_MENU_ON,
    NUM_OF_SYNC_MENU_STATE
} D_SYNC_MENU_STATE;

typedef enum
{
    D_PICBACKLT_OFF = 0,
    D_PICBACKLT_ON,
    NUM_OF_PICBACKLT_STATE
} D_PICBACKLT_STATE;

typedef enum
{
    D_NOT_SCANNING = 0,
    D_IS_SCANNING,
    NUM_OF_SCAN_STATE
} D_AUTOMAN_SCAN_STATE;

typedef enum
{
    D_TELETEXT_OFF = 0,
    D_TELETEXT_ON,
    NUM_OF_TELETEXT_MODE
} D_TELETEXT_MODE;

typedef enum
{
    D_SCENE_AUTO = 0,
    D_SCENE_GENERAL,
    D_SCENE_PHOTO,
    D_SCENE_MUSIC,
    D_SCENE_CINEMA,
    D_SCENE_GAME,
    D_SCENE_GRAPHICS,
    D_SCENE_SPORTS,
    NUM_OF_SCENE_SELECT_MODE
} D_SCENE_SELECT_MODE;

typedef enum
{
    D_AGING_OFF,
    D_AGING_ON,
    NUM_OF_AGING_MODE  
} D_AGING_MODE;

typedef enum
{
    D_RGB_SREG,
    D_DBL_SREG,
    D_SIG_BSTR_SREG,
    D_TUNER_SREG,
    NUM_OF_DRV_SREG  
} D_DRV_SREG;

typedef enum
{
    D_DEMO_MODE_USB_OFF = 0,
    D_DEMO_MODE_USB_ON,
    NUM_OF_DEMO_MODE_USB
} D_DEMO_MODE_USB_VIDEO_MODE;

/* Marina - Start : Internal Signal Pattern ( 31 July 2009)*/  
typedef struct DRV_CUSTOM_INT_SIG_PTN
{
    UINT8 path;                      /*path*/
    UINT8 pattern;					 /*pattern*/
    UINT8 tag;                       /*tag*/
    UINT16 gamma_r;                  /*gamma_r_value*/
    UINT16 gamma_g;                  /*gamma_g_value*/
    UINT16 gamma_b;                  /*gamma_b_value*/
} DRV_CUSTOM_INT_SIG_PTN;
/* Marina - End : Internal Signal Pattern ( 31 July 2009)*/  

/* Marina - Start : HDMI EDID Read/Write( 10 August 2009)*/  
typedef enum
{
    D_HDMI_PORT1,
    D_HDMI_PORT2,
    D_HDMI_PORT3,
    D_HDMI_PORT4,
} D_HDMI_PORT_NUM;

typedef struct DRV_CUSTOM_EDID_DATA
{
    D_HDMI_PORT_NUM hdmi_port;                      /*HDMI port*/
    UINT8 edid_buffer[256];               /*HDMI EDID Buffer */
} DRV_CUSTOM_EDID_DATA;
/* Marina - End : HDMI EDID Read/Write( 10 August 2009)*/  

/* Ooi START: callback func for IP (1/9/09) */
typedef struct DRV_CUSTOM_IP_NOISE_TH_T
{
	UINT8 lvl;
	UINT8 data;
}DRV_CUSTOM_IP_NOISE_TH;
/* Ooi END */
/*TSEN start*/
typedef struct DRV_PARAM_ADC
{
    UINT8   adc_input;
    UINT8   adc_ch_num;
    UINT16  adc_ch_value;
}DRV_PARAM_ADC;

typedef struct DRV_PARAM_ADC_LVL
{
    UINT8   adc_ch_num;
    UINT16  adc_ch_value;
}DRV_PARAM_ADC_LVL;
/*TSEN end*/

typedef struct DRV_SREG
{
    D_DRV_SREG  type;
    UINT16      sreg;
    UINT16*     value;
}DRV_SREG;
typedef enum
{
    TU_FreqData,
    TU_StepData,
    TU_BandData,
} DRV_SONY_TUNER_DATA;

/* Marina -Start: Color Level(25 September 09)*/
typedef struct DRV_COLOR_LEVEL
{
    UINT16 h_pixel;
    UINT16 v_pixel;
    UINT16 rgb_level;
    UINT8 rgb_tag;
    UINT16 h_pixel_area;
    UINT16 v_pixel_area;
}DRV_COLOR_LEVEL;
/* Marina End */
/* Chow -Start: Sig bstr IP(09 Oct 09)*/
typedef struct DRV_SIG_BSTR_IP_ST
{
    BOOL IP_status;
    BOOL SigBstr_status;
}DRV_SIG_BSTR_IP_ST;
/* Chow End */
typedef enum
{
    SDCVCOM,
    LGDVCOM,
    SDPVCOM,
} DRV_VCOM_TYPE;
typedef struct _DRV_CUSTOM_VCOM_ST
{ 
    DRV_VCOM_TYPE   vcom_type;
    UINT16          *vcom_value;
}  DRV_CUSTOM_VCOM_ST;

/* 2K14 */
typedef struct _DRV_CUSTOM_SCART_ST
{ 
    
    UINT16          *scart_value;
}  DRV_CUSTOM_SCART_ST;


#if 1
typedef struct _WB_T
{
    UINT8                       ui1_magic[2];     // Magic Number
    UINT8                       ui1_color_temp ;  // 0~2 : 0-Cool, 1-Neutral, 2-Warm
    UINT16                      ui2_val;          // 0~1023 : 10bit value of RGB Gain/Offset.
} WB_T;

typedef struct _DYN_GAMMA_T
{
    UINT8       ui1_gamma_point;  // 0~10
    UINT16      ui2_r;            // 0~4095
    UINT16      ui2_g;            // 0~4095
    UINT16      ui2_b;            // 0~4095
    UINT8       ui1_magic_num[8];
} DYN_GAMMA_T;
typedef struct _GAMMA_PTN_RGB_T
{
	//UINT16 idx;	//for debug use only
	UINT16 LUMA_R;
	UINT16 LUMA_G; 
	UINT16 LUMA_B;
} GAMMA_PTN_RGB_T;

typedef struct _GAMMA_SET_PTN_RGB_T
{
	UINT8 ui1Rgb;
	UINT16 ui4Val;
} GAMMA_SET_PTN_RGB_T;

typedef struct _set_LNA_CTRL
{
	UINT16 *u1LNA;
	
} set_LNA_CTRL;

typedef struct _FAC_DCR_T
{
    BOOL        b_onoff;
    BOOL        b_get_bl_max;
    UINT8       ui1_bl_level;
} FAC_DCR_T;
typedef struct _FAC_EDID_T //Soar add 20101007 for VGA EDID
{
    BOOL        b_onoff;
    BOOL        b_finish;
} FAC_EDID_T;

typedef struct _FAC_SERVO_ADC_T
{
    UINT8       ui1_servo_pin;
    INT32       i4_val;

} FAC_SERVO_ADC_T;
typedef struct _FAC_TUNER_REG_T
{
    UINT16      ui2_ret;            /* Actual number of bytes read/write. */
    UINT8       ui1_sub_addr;       /* Serial interface word address. */
    UINT16      ui2_cnt;            /* Number of byte to read. Max = 256 bytes. */
    UINT8       aui1_val[256];      /* Pointer to user's buffer */
} FAC_TUNER_REG_T;

typedef struct _FAC_SIF_T
{
    UINT16      ui2_ret;            /* Actual number of bytes read/write. */
    UINT8       u1CHannelID;        /* SIF bus ID. */
    UINT16      u2ClkDiv;           /* Parameter of divider to divide 27Mhz for SCL. */
    UINT8       u1DevAddr;          /* Serial interface device address. */
    UINT8       u1WordAddrNum;      /* Serial interface word address length. */
    UINT32      u4WordAddr;         /* Serial interface word address. Format XX B2 B1 B0. B2 is sent on SIF bus first. */
    UINT8       au1Buf[256];        /* Pointer to user's buffer */
    UINT16      u2ByteCnt;          /* Number of byte to read. Max = 256 bytes. */
} FAC_SIF_T;
typedef struct _FAC_EEP_T
{
    UINT32 u4Offset;
    UINT32 u4Len;
    UINT8  * pu1Data;
} FAC_EEP_T;
typedef struct _WB_PBM_PQP_LUT_SET_T
{
    UINT16      ui2WbIndex;
    UINT16      aui2PqpLut[99];
} WB_PBM_PQP_LUT_SET_T;

typedef struct _WB_PBM_PQP_LUT_PRESET_T
{
    UINT8      ui1WbIndex;
    UINT8      ui1RgbIdx;
	UINT8      ui1WblvlIdx;
} WB_PBM_PQP_LUT_PRESET_T;

typedef struct _BL_CTRL_OPERATION_T
{
    UINT8      uiOpMode;
    UINT16     ui2OpParameter;
} BL_CTRL_OPERATION_T;
typedef enum
{
	DELTA_MODE,
	FINAL_MODE,
	NUM_OF_AUTO_GAIN_MODE
} D_AUTOWB_GAIN_MODE;
typedef enum
{
	AUTOWB_R,
	AUTOWB_G,
	AUTOWB_B,
	NUM_OF_AUTOWB_RGB_BLOCK
} D_AUTOWB_RGB;
typedef struct _WB_AUTO_GAIN_T
{
    D_AUTOWB_GAIN_MODE  AutoWbMode;
    D_AUTOWB_RGB        AutoWbRgb;
	UINT16              ui2Gain;
} WB_AUTO_GAIN_T;
typedef struct _WB_TCON_NVM_T
{
    UINT8 data_buffer[256];
    UINT16 subAddr;
    UINT16 length;
} WB_TCON_NVM_T;



typedef struct _WB_CLR_TEMP_DATA_T
{
	UINT16 wRGain;
	UINT16 wGGain; 
	UINT16 wBGain;
	UINT16 wROfst;
	UINT16 wGOfst;
	UINT16 wBOfst;
} WB_CLR_TEMP_DATA_T;

typedef struct _WB_CLR_TEMP_DATA_GET_T
{
    WB_CLR_TEMP_DATA_T wClrTempData;
	UINT16 ui2ClrTempIdx;
} WB_CLR_TEMP_DATA_GET_T;
#endif

/* ================================== */
/* add for Maple BEPQ related start*/
//Pic mode
typedef enum {
    D_AP_PIC_MODE_VIVID,
    D_AP_PIC_MODE_STANDARD,
    D_AP_PIC_MODE_CUSTOM,
    D_AP_PIC_MODE_CINEMA,
    D_AP_PIC_MODE_PHOTO,
    D_AP_PIC_MODE_SPORTS,
    D_AP_PIC_MODE_GAME,
    D_AP_PIC_MODE_GRAPHICS,
}D_AP_PIC_MODE;

typedef enum {
    D_AP_PIC_COL_TMP_COOL,
    D_AP_PIC_COL_TMP_NATURAL,
    D_AP_PIC_COL_TMP_WARM,
}D_AP_PIC_COL_TMP;

typedef enum {
    D_AP_PIC_NOISE_REDUCTION_OFF,
    D_AP_PIC_NOISE_REDUCTION_LOW,
    D_AP_PIC_NOISE_REDUCTION_MID,
    D_AP_PIC_NOISE_REDUCTION_HIGH,
    D_AP_PIC_NOISE_REDUCTION_AUTO,
}D_AP_PIC_NOISE_REDUCTION;

typedef enum {
    D_AP_PIC_MPEG_NOISE_REDUCTION_OFF,
    D_AP_PIC_MPEG_NOISE_REDUCTION_STANDARD,
    D_AP_PIC_MPEG_NOISE_REDUCTION_HIGH,
}D_AP_PIC_MPEG_NOISE_REDUCTION;

typedef enum {
    D_AP_PIC_MOTION_FLOW_OFF,
    D_AP_PIC_MOTION_FLOW_STANDARD,
    D_AP_PIC_MOTION_FLOW_HIGH,
}D_AP_PIC_MOTION_FLOW;

typedef enum {
    D_AP_PIC_FILM_MODE_OFF,
    D_AP_PIC_FILM_MODE_AUTO1,
    D_AP_PIC_FILM_MODE_AUTO2,
}D_AP_PIC_FILM_MODE;

typedef enum {
    D_AP_PIC_ADV_CONTRAST_ENHANCER_OFF,
    D_AP_PIC_ADV_CONTRAST_ENHANCER_LOW,
    D_AP_PIC_ADV_CONTRAST_ENHANCER_MID,
    D_AP_PIC_ADV_CONTRAST_ENHANCER_HIGH,
}D_AP_PIC_ADV_CONTRAST_ENHANCER;

typedef enum {
    D_AP_PIC_BLACK_CORRECTOR_OFF,
    D_AP_PIC_BLACK_CORRECTOR_LOW,
    D_AP_PIC_BLACK_CORRECTOR_MID,
    D_AP_PIC_BLACK_CORRECTOR_HIGH,
}D_AP_PIC_BLACK_CORRECTOR;

typedef enum {
    D_AP_PIC_CLEAR_WHITE_OFF,
    D_AP_PIC_CLEAR_WHITE_LOW,
    D_AP_PIC_CLEAR_WHITE_HIGH,
}D_AP_PIC_CLEAR_WHITE;

typedef enum {
    D_AP_PIC_LIVE_COLOR_OFF,
    D_AP_PIC_LIVE_COLOR_LOW,
    D_AP_PIC_LIVE_COLOR_MID,
    D_AP_PIC_LIVE_COLOR_HIGH,
}D_AP_PIC_LIVE_COLOR;

typedef enum {
    D_AP_PIC_LED_MOTION_MODE_OFF,
    D_AP_PIC_LED_MOTION_MODE_ON,
}D_AP_PIC_LED_MOTION_MODE;

typedef enum {
    D_AP_PIC_INTELI_PIC_PLUS_OFF,
    D_AP_PIC_INTELI_PIC_PLUS_ON,
}D_AP_PIC_INTELI_PIC_PLUS;

typedef struct DRV_AP_PIC_SETTING_T
{
    D_AP_PIC_MODE	ePicMode;
    UINT8	u1Backlight;
    UINT8	u1Contrast;
    UINT8	u1Brightness;
    UINT8	u1Color;
    UINT8	u1Hue;
    UINT8	u1Sharpness;
    D_AP_PIC_COL_TMP	eColTmp;
    D_AP_PIC_NOISE_REDUCTION	eNoiseReduction;
    D_AP_PIC_MPEG_NOISE_REDUCTION	eMpegNoiseReduction;
    D_AP_PIC_MOTION_FLOW	eMotionFlow;
    D_AP_PIC_FILM_MODE	eFilmMode;
    D_AP_PIC_ADV_CONTRAST_ENHANCER    eAdvContrastEnhancer;
    D_AP_PIC_BLACK_CORRECTOR	eBlackCorrector;
    UINT8	u1Gamma;
    D_AP_PIC_CLEAR_WHITE	eClearWhite;
    D_AP_PIC_LIVE_COLOR	eLiveColor;
    D_AP_PIC_LED_MOTION_MODE	eLedMotionMode;
    UINT8	u1RGain;
    UINT8	u1GGain;
    UINT8	u1BGain;
    UINT8	u1RBias;
    UINT8	u1GBias;
    UINT8	u1BBias;
    D_AP_PIC_INTELI_PIC_PLUS	eInteliPicPlus;
}DRV_AP_PIC_SETTING_T;

//screen mode
typedef enum {
    D_AP_WIDE_MODE_WIDE_ZOOM,
    D_AP_WIDE_MODE_NORMAL,
    D_AP_WIDE_MODE_FULL,
    D_AP_WIDE_MODE_ZOOM,
    D_AP_WIDE_MODE_14_9,
    D_AP_WIDE_MODE_CAPTION,
    D_AP_WIDE_MODE_UNKNOWN,
}D_AP_WIDE_MODE;

typedef enum {
    D_AP_WIDE_DISPLAY_AREA_FULL, //+1
    D_AP_WIDE_DISPLAY_AREA_NORMAL,
    D_AP_WIDE_DISPLAY_AREA_MINUS_1,
}D_AP_WIDE_DISPLAY_AREA;

typedef enum {
    D_AP_WIDE_ZOOM_1X,
    D_AP_WIDE_ZOOM_2X,
    D_AP_WIDE_ZOOM_4X,
    D_AP_WIDE_ZOOM_FULL,
    D_AP_WIDE_ZOOM_UNKNOWN, 
}D_AP_WIDE_ZOOM;

typedef enum {
    D_AP_WIDE_PC_WIDE_MODE_NORMAL,
    D_AP_WIDE_PC_WIDE_MODE_FULL1,
    D_AP_WIDE_PC_WIDE_MODE_FULL2,
    D_AP_WIDE_PC_WIDE_MODE_UNKNOWN,
}D_AP_WIDE_PC_WIDE_MODE;

typedef struct DRV_AP_SCREEN_SETTING_T 
{
    D_AP_WIDE_MODE	eWideMode;
    D_AP_WIDE_DISPLAY_AREA	eDisplayArea;
    UINT8	u1HCenter;
    UINT8	u1VCenter;
    UINT8	u1VerticalSize;
    D_AP_WIDE_ZOOM eZoom; // For USB and IPTV contents
    D_AP_WIDE_PC_WIDE_MODE	ePcWIdeMode; // For PC
    UINT8	u1PcPhase; // For PC
    UINT8	u1PcPitch; // For PC
    UINT8	u1PcHCenter; // For PC
    UINT8	u1PcVCenter; // For PC
}DRV_AP_SCREEN_SETTING_T;

//3d setting
typedef enum {
    D_AP_3D_DISP_OFF,
    D_AP_3D_DISP_SIM_3D,
    D_AP_3D_DISP_SBS,
    D_AP_3D_DISP_TAB,
    D_AP_3D_DISP_ON,
}D_AP_3D_DISPLAY;

typedef enum {
    D_AP_SIM_3D_EFFECT_LOW,
    D_AP_SIM_3D_EFFECT_MID,
    D_AP_SIM_3D_EFFECT_HIGH,
}D_AP_SIM_3D_EFFECT;

typedef struct DRV_AP_3D_SETTING_T
{
    D_AP_3D_DISPLAY	e3dDisplay;
    UINT8	u13dDepthAdjustment;
    D_AP_SIM_3D_EFFECT	eSim3DEffect;
}DRV_AP_3D_SETTING_T;

//eco setting
typedef enum {
    D_AP_POWER_SAVING_OFF,
    D_AP_POWER_SAVING_LOW,
    D_AP_POWER_SAVING_HIGH,
    D_AP_POWER_SAVING_PICTURE_OFF,
}D_AP_POWER_SAVING;

typedef enum {
    D_AP_LIGHT_SENSOR_OFF,
    D_AP_LIGHT_SENSOR_ON,
}D_AP_LIGHT_SENSOR;

typedef struct DRV_AP_ECO_SETTING_T
{
    D_AP_POWER_SAVING	ePowerSaving;
    D_AP_LIGHT_SENSOR	eLightSensor;
}DRV_AP_ECO_SETTING_T;

//cust service setting
typedef struct DRV_AP_SERVICE_SETTING_T
{
    UINT16	u2RGain;
    UINT16	u2GGain;
    UINT16	u2BGain;
    UINT16	u2ROffset;
    UINT16	u2GOffset;
    UINT16	u2BOffset;
}DRV_AP_SERVICE_SETTING_T;

//system related
typedef struct DRV_AP_HISTORY_DATA_T
{
    UINT32	u4TotalOpTime;
    UINT32	u4PanelOpTime;
}DRV_AP_HISTORY_DATA_T;

typedef enum {
    D_AP_SYS_PANEL_STATE_OFF,
    D_AP_SYS_PANEL_STATE_ON,
}D_AP_SYS_PANEL_STATE; 

typedef struct DRV_AP_POWER_STATE_T
{
    D_AP_SYS_PANEL_STATE	ePanelState;
}DRV_AP_POWER_STATE_T;

typedef enum {    
    D_AP_INPUT_TYPE_A_RF,   
    D_AP_INPUT_TYPE_D_RF,
    D_AP_INPUT_TYPE_COMPOSITE,
    D_AP_INPUT_TYPE_COMPONENT,
    D_AP_INPUT_TYPE_SCART,
    D_AP_INPUT_TYPE_A_PC,
    D_AP_INPUT_TYPE_HDMI,
    D_AP_INPUT_TYPE_USB,
    D_AP_INPUT_TYPE_DNLA,
    D_AP_INPUT_TYPE_IPTV,
    D_AP_INPUT_TYPE_BROWSER,
    D_AP_INPUT_TYPE_WIFI_DISPLAY,
    D_AP_INPUT_TYPE_UNKNOWN,
}D_AP_INPUT_TYPE;

typedef struct DRV_AP_INPUT_TYPE_T
{
    D_AP_INPUT_TYPE	eMainInputType;
	D_AP_INPUT_TYPE	eSubInputType;
}DRV_AP_INPUT_TYPE_T;

/* add for Maple BEPQ related end*/
/*-----------------------------------------------------------------------------
                    function declarations
-----------------------------------------------------------------------------*/
/* Customer specific APIs */
extern INT32 d_maple_cust_spec_set (
    DRV_CUSTOM_CUST_SPEC_TYPE_T    e_cust_spec_type,
    VOID*                          pv_set_info,
    SIZE_T                         z_size,
    BOOL                           b_store
);

extern INT32 d_maple_cust_spec_get (
    DRV_CUSTOM_CUST_SPEC_TYPE_T    e_cust_spec_type,
    VOID*                          pv_get_info,
    SIZE_T*                        pz_size
);

extern void CustomLEDCtrlFunc(
    D_LED_TYPE eLedType,
    UINT32 u4LedState
);

extern void CustomDisableStandbyLEDCtrl(
    UINT8 u1Disable
);

extern UINT32 SelfDiagGetBootCounter(
    UINT32 *pu4BootCounter
);

extern UINT32 SelfDiagGetCounter(
    D_SELF_DIAG_STATE   *prErrorInfo
);

extern void CustomSetLoaderUsbUpgrade(
    BOOL bEnable
);

extern void ADAC_GpioInterDecMute(
    BOOL fgEnable
);

extern BOOL CustomIsLoaderUsbUpgradeEnable(void);
extern UINT32 SelfDiagResetCounter(void);
extern UINT32 SelfDiagResetBootCount(void);

#endif /* _D_SONY_2K12_CUST_H_ */
