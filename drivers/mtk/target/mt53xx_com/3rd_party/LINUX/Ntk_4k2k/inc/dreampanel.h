/*************************************************************************
 * 
 * Filename: 
 * ---------
 *   $Workfile: dreampanel.h $
 *
 * Project: 
 * -------- 
 *    
 * 
 * Description: 
 * ------------ 
 *    
 *    
 * Author: 
 * ------- 
 *   
 * 
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Modtime: 2012/6/5 $
 *
 * $Revision: #1 $
*************************************************************************/

#ifndef __DREAMPANEL_H__
#define __DREAMPANEL_H__

#include "typedef.h"


#define EEPROM_DREAM_PANEL_MODE (0xD17)
#define EEPROM_MANUAL_BACKLIGHT (0xD18)
#define EEPROM_LAST_TIME_BACKLIGHT (0xD19)
#define EEPROM_LIGHT_SENSOR_ADC_OFFSET (0xD1A)
#define EEPROM_LIGHT_SENSOR_NUMERATOR (0xD1B)
#define EEPROM_LIGHT_SENSOR_DENOMINATOR (0xD1C)
#define EEPROM_DREAM_PANEL_LOG_ENABLE (0xD1D)
#define EEPROM_DREAM_DEBUG_MODE (0xD24)
#define EEPROM_DREAM_PANEL_DELTA_VALUE (0xD25)
#define EEPROM_DREAM_PANEL_FIX_PWM_VALUE (0xD26)
#define EEPROM_DREAM_PANEL_NEED_FIX_POWER (0xD27)
#define EEPROM_DREAM_PANEL_FIX_PWM_VALUE_FOR_NINE_WINDOW (0xD28)
#define EEPROM_DREAM_PANEL_HANDLE_CNT (0xD29)

#define DELAY_CNT_TO_DREAM_PANEL_SETTING (700L) // 700x0.03 = 21 second

#define LIGHT_SENCE_ADC_ID 4    // for adc port:ADIN4
#define MTK_SZ_APP_SKY_DRM_PNL_CONTROL
#ifdef MTK_SZ_APP_SKY_DRM_PNL_CONTROL
	#define SUPPORT_DREAM_PANEL 1
	#define SUPPORT_LIGHT_PWM_VALUE_CHANGE_SLOWLY 1
	#define LG_OPC_PANEL 0
	#define LG_NON_OPC_PANEL 0
	#define PANEL_CMO_V546H1_LE1 0
	#define PANEL_CMO_V420H1_LE1 0

#if SUPPORT_DREAM_PANEL
#define BACKLIGHT_LIGHT_SENSE_INTERVAL (200) //200ms
#define BACKLIGHT_CONTENT_OR_MULTI_INTERVAL (300) //300ms
#define DISPLAY_HISTOGRAM_UPDATE_INTERVAL (1000/20) //1000ms
#define LIGHT_SENCE_AD_GET_INTERVAL 20 //20ms

#define V12_backlight_pwm_offset 0

typedef enum
{
	Off_Mode,
	Environ_Light_Mode,
	Image_Content_Mode,
	Multi_Detect_Mode,
	DreamPanelDemo_Mode,//gmh add 20120606
	Backlightcontrol_Mode_Invalid,
}eBacklightcontrol_mode;

typedef enum
{
    BACKLIGHT_FOR_LIGHT_SENSE_STATE,
    BACKLIGHT_CONTENT_OR_MULTI_STATE,
    BACKLIGHT_IDLE_STATE
}BACKLIGHTCONTROL_STATE;

void BackLightControlModeSet(eBacklightcontrol_mode mode,BOOL bFirstPowerOn);
void LightControlHandler(void);
void BackLightControl_ManualValueSet(UINT8 value);
UINT8 BackLightControl_ManualValueGet(void);

void BACKLIGHT_control_init(void);
void read_histogram(UINT8 *hist);
void vDreamPanelSetNoSignalState(BOOL bNosignal);

//Light[0] - Light[15]:ImageContent[0] - ImageContent[15]
//Light[16]:BackLightLevel,Light[17]:light sense value
//data range 0-255
void vDreamPanelLightDataGet(UINT8 *Light);
UINT8 bDreamPanelLastLightGet(void);
eBacklightcontrol_mode DreamPanelModeGet(void);
#endif


UINT8 bDrvGetViiHist(UINT16 *wHistgram);

#endif  // kk 100415 v6 end


#endif //__DREAMPANEL_H__


