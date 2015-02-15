
#include "x_bim.h"
#include "x_debug.h"
#include "x_hal_arm.h"
#include "x_timer.h"
#include "x_assert.h"
#include "ostg_if.h"
#include "panel.h"
#include "pdwnc_if.h"
#include "rtc_if.h"
#include "mute_if.h"
#include "eeprom_if.h"
#include "drv_display.h"
#include "drv_video.h"
#include "pmx_drvif.h"
#include "osd_drvif.h"
#include "aud_if.h"
#include "PowerSeq.h"
#include "CustomCtrl.h"
#include "pe_if.h"
#include "eepdef.h"
#include "api_eep.h"
#include "ir_if.h"
#include "drv_od.h"
#include "pe_if.h"
#include "drv_tcon.h"

#include "mhl_if.h"

//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported funtions 
//-----------------------------------------------------------------------------
extern void CHIP_Reboot(void);
extern void vDrvTurnOnPanel(void);
extern void vDrvTurnOffPanel(void);
extern void vDrvLVDSOff(void);
extern void vDrvLVDSConfig(void);
extern void vDrvLVDSOn(void);
extern void vDrvSetLock(UINT32 u4Src, UINT32 u4LockON);
extern UINT32 PANEL_GetLvdsOnDalay(void);
extern UINT32 PANEL_GetLvdsOffDalay(void);
extern UINT32 PANEL_GetBacklightOnDelay(void);
extern UINT32 PANEL_GetBacklightOffDelay(void);
extern void DRVCUST_EepromLoadODTable(void);
extern void DRVCUST_SetScanPWM_High_Low( UINT32 u4High);
extern void vDRVCUST_BLSet_IP_REF(UINT32 u4Dimming);


extern UINT32 _u4BrightLevel;
extern UINT32 _u4gModelType;
extern UINT32 _u4gPanelType;
extern UINT32 _u4gLVDSType;

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
UINT16 _au2PowerSeqDelay[POWER_SEQ_DELAY_DATA_SIZE];
HAL_TIME_T _rTime;
#ifndef CUSTOM_GET_DELAY_FROM_EEPROM 
static UINT16 _au2DefaultDelay1[POWER_SEQ_DELAY_DATA_SIZE] = 
{
200,//T33
150,//T37
1600,//T34
20,//T35
600,//T36
5,//T41
210,//T39
10,//T38
0,
0,
0,
0,
0,
0};
#endif	  
static UINT32 _au4PowerSeqStartMark[POWER_SEQ_DELAY_DATA_SIZE]={0}; 
#if 0
static UINT16 _au2DefaultDelay2[POWER_SEQ_DELAY_DATA_SIZE] = 
       {10, 300, 40, 300, 500, 400, 400, 100, 100, 30, 10, 200, 0, 100};
static UINT16 _au2DefaultDelay3[POWER_SEQ_DELAY_DATA_SIZE] = 
       {10, 300, 40, 300, 500, 400, 220, 100, 300, 30, 10, 200, 0, 150};
#endif
UINT16 *_au2DefaultDelay;

static BOOL _bfgPowerOnIsDone = FALSE;
static BOOL _bfgBackltOn = FALSE;
static BOOL _bfgLVDSOutput = FALSE;
#ifdef CUSTOM_PANEL_ID_CHECK_ENABLE
static BOOL _bfgCheckPanelID = TRUE;
#else
static BOOL _bfgCheckPanelID = FALSE;
#endif /* CUSTOM_PANEL_ID_CHECK_ENABLE */
#ifdef CC_MHL_HDMI_SWITCH
static BOOL _bfgMhlInit = FALSE;
#endif
static BOOL _bfgCustPowerOnSeqPWMON = TRUE;
//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void CustomGetPowerSequenceDelay(void)
{
    static BOOL bInit = FALSE;
    UINT32 u4Len = sizeof(UINT16) * POWER_SEQ_DELAY_DATA_SIZE;
	UINT8 PanelIdx = 0;
		 

#ifdef CUSTOM_GET_AUTO_DETECT_FROM_EEPROM
    UINT8  u1AutoDetect = 0xFF;
#endif /* CUSTOM_GET_AUTO_DETECT_FROM_EEPROM */

    if (bInit)
    {
        return;
    }

#ifdef CUSTOM_GET_AUTO_DETECT_FROM_EEPROM
    // Get Panel ID check option
    UNUSED(EEPROM_Read(POWER_SEQ_CHECK_PANEL_ADDR, 
                       (UINT32)(VOID *)&u1AutoDetect, sizeof(UINT8)));
    if (u1AutoDetect > 1)
    {
        u1AutoDetect = 1;
        UNUSED(EEPROM_Write(POWER_SEQ_CHECK_PANEL_ADDR, 
                            (UINT32)(VOID *)&u1AutoDetect, sizeof(UINT8)));
    }
    
    _bfgCheckPanelID = FALSE;//(BOOL)u1AutoDetect;
#endif /* CUSTOM_GET_AUTO_DETECT_FROM_EEPROM */

#ifdef CUSTOM_GET_DELAY_FROM_EEPROM
    UINT16 u2Val = 0, i;
    UINT64 u8Addr = POWER_SEQ_DELAY_ADDR;

    x_memset(_au2PowerSeqDelay,  , u4Len);
    
    // Get power sequence delay from EEPROM
    u4Len = sizeof(UINT16);
    for (i = 0; i < POWER_SEQ_DELAY_DATA_SIZE; i++)
    {
        GET_POWER_SEQ_DELAY(u8Addr, _au2PowerSeqDelay[i], 
                            u2Val, u4Len, _au2DefaultDelay[i]);
        u8Addr += sizeof(UINT16);
    }
#else
// ====================================================================
// for 60" , 50" and 40/46/55/other default panel sequential setting
    UNUSED(EEPROM_Read(PANEL_INDEX_1, 
                       (UINT32)(VOID *)&PanelIdx, sizeof(UINT8)));
#if 1 //wangle
	_au2DefaultDelay = _au2DefaultDelay1;
#else
	if ( PanelIdx == 60 || PanelIdx == 61 || PanelIdx == 62 || PanelIdx == 63 )
		_au2DefaultDelay = _au2DefaultDelay1;
	else if ( PanelIdx == 9 ) 
		_au2DefaultDelay = _au2DefaultDelay2;
	else 
		_au2DefaultDelay = _au2DefaultDelay3;
#endif
    x_memcpy(_au2PowerSeqDelay, _au2DefaultDelay, u4Len);
// ====================================================================    
#endif /* CUSTOM_GET_DELAY_FROM_EEPROM */

    bInit = TRUE;
}

UINT32 CustomQueryPowerSequenceDelay(UINT32 u4Index)
{
    CustomGetPowerSequenceDelay();
    return (_au2PowerSeqDelay[u4Index]);
}
void CustomMarkPowerSequenceStart(UINT8 u1PowerSeqIdx)
{
    if(u1PowerSeqIdx<POWER_SEQ_DELAY_DATA_SIZE)
	{
	    _au4PowerSeqStartMark[u1PowerSeqIdx]=x_os_get_sys_tick();
	}
}
UINT32 CustomQueryPowerSequenceStart(UINT32 u4Index)
{
    if(u4Index<POWER_SEQ_DELAY_DATA_SIZE)
    {
	    return (_au4PowerSeqStartMark[u4Index]);
    }
	return 0;
}
void CustomUpdateBacklightState(BOOL bIsBackltOn)
{
    CRIT_STATE_T rCritState;
    
    rCritState = x_crit_start();
    _bfgBackltOn = bIsBackltOn;
    x_crit_end(rCritState);
    
#ifdef CUSTOM_SELF_DIAG_ENABLE
    SelfDiagUpdateBacklightState(bIsBackltOn);
#endif /* CUSTOM_SELF_DIAG_ENABLE */
}

BOOL CustomGetBackltStatus(void)
{
#if 1
    return _bfgBackltOn;
#else
    static BOOL bInitial = TRUE;
    static UINT32 u4Gpio, u4Value;

    if (bInitial)
    {
        u4Gpio = DRVCUST_PanelGet(eBackLightGpio);
        u4Value = DRVCUST_PanelGet(eBackLightOnVal);
        bInitial = FALSE;
    }
    
    return ((u4Value == GPIO_Output(u4Gpio, NULL)) ? TRUE : FALSE);
#endif    
}

void CustomSetPowerSeqStatus(BOOL fgOnOff)
{
    CRIT_STATE_T rCritState;
    
    rCritState = x_crit_start();
    _bfgPowerOnIsDone = fgOnOff;
    x_crit_end(rCritState);
}

BOOL CustomGetPowerSeqStatus(void)
{
    BOOL bPowerOnIsDone;
    CRIT_STATE_T rCritState;
    
    rCritState = x_crit_start();
    bPowerOnIsDone = _bfgPowerOnIsDone;
    x_crit_end(rCritState);
    
    return bPowerOnIsDone;
}

void CustomStartDimmerOutput(void)
{
    PRINT_TIMESTAMP_MSG("[Power On]", "START_DIMMER_OUTPUT\n");
    
    // Start dimmer output        
    // after panel on, we could set backlight
//#ifndef CC_SONY_BACKLIGHT_SEQUENCE
//    vApiSetPanelBright(_u4BrightLevel);
    //BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), (UINT32)_u4BrightLevel);
//#endif /* CC_SONY_BACKLIGHT_SEQUENCE */           //mark define for power on sequence. robin

    if (DRVCUST_PanelGet(eFlagBacklightDimmingLock))
    {
        vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_ON);
    }
    else
    {
        vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_OFF);
    }
}

void CustomLVDSOutputCtrl(HANDLE_T hHandle, VOID* pTag)
{
    CRIT_STATE_T rCritState;
    
    UNUSED(hHandle);
    UNUSED(pTag);
    
    if (_bfgLVDSOutput)
    {
        return;
    }
    
    rCritState = x_crit_start();
    _bfgLVDSOutput = TRUE;
    x_crit_end(rCritState);
    
    // Send LVDS output
    PRINT_TIMESTAMP_MSG("[Power On]", "LVDS_OUTPUT (Send LVDS output)\n");
    vDrvLVDSConfig();
    vDrvLVDSOn();
}

void CustomCheckPanelId(void)
{
    UINT32 u4CurrentPanelType;
    UINT32 u4NewPanelIndex;
    UINT32 u4Resolution = PANEL_GetPanelWidth();

    PRINT_TIMESTAMP_DBG_MSG("[Power On]", "Panel Width = %d\n", u4Resolution);
    switch (u4Resolution)
    {
    case CUSTOM_PANEL_WIDTH_FHD:
			if(PANEL_IsSupportMlvds())
			{
				u4CurrentPanelType = CUSTOM_GPIO_PANEL_NONTCON;
			}
			else
			{
				u4CurrentPanelType = CUSTOM_GPIO_PANEL_FHD;
			}		 

        break;
    case CUSTOM_PANEL_WIDTH_WXGA:
        u4CurrentPanelType = CUSTOM_GPIO_PANEL_WXGA;
        break;
    default:
        u4CurrentPanelType = CUSTOM_GPIO_PANEL_OTHER;
        break;
    }

    // If the resolution of the current panel setting is identical to 
    // the definition of PCB strap pin, then use the current setting
    if (u4CurrentPanelType == _u4gPanelType)
    {
        return;
    }

    PRINT_TIMESTAMP_MSG("[Power On]", "Panel ID mismatch\n");
    
		if(_u4gPanelType == CUSTOM_GPIO_PANEL_NONTCON)
		{
			u4NewPanelIndex =  DEFAULT_PANEL_NONTCON_SELECT ;
		}
		else if (_u4gPanelType == CUSTOM_GPIO_PANEL_WXGA)
		{
			u4NewPanelIndex =  DEFAULT_PANEL_WXGA_SELECT ;
		}
		else  //fULL HD
		{
				u4NewPanelIndex =  DEFAULT_PANEL_FHD_SELECT ;
	   }   
	
		WritePanelIndexToEeprom(u4NewPanelIndex);
		x_thread_delay(100);
		CHIP_Reboot();
	}
void Custom60HzPowerOnSequence(void)
{
#ifndef CC_MTK_LOADER
    HANDLE_T hLVDSOutputTimer = NULL_HANDLE;
    // Create a timer to enable LVDS output.
    UNUSED(x_timer_create(&hLVDSOutputTimer));
#endif /* CC_MTK_LOADER */
#ifdef SIGNAL_X_U_RST
    INT32 i4Value = 1;
    UNUSED(GPIO_Output(SIGNAL_X_U_RST, &i4Value));
    PRINT_TIMESTAMP_DBG_MSG("[Power On]", "X_U_RST = %d\n", i4Value);
#endif
#ifndef CC_MANUAL_PANEL_SEL
    // Panel ID sequence
    PRINT_TIMESTAMP_MSG("[Power On]", "PANEL_ID sequence\n");
    if (_bfgCheckPanelID)
    {
        CustomCheckPanelId();
    }
#endif
     POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T34);//T34 End			
     vDrvTurnOnPanel();
     CustomMarkPowerSequenceStart(E_POWER_SEQ_T35);//T35 Start
    PRINT_TIMESTAMP_DBG_MSG("[Power On]", "LVDS_PWR_ON = %d\n",
                            DRVCUST_PanelGet(ePanelPowerOnVal));  
    // Send LVDS output
//    PRINT_TIMESTAMP_MSG("[Power On]", "LVDS_OUTPUT (Send LVDS output)\n");
    vDrvLVDSConfig();
    POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T35);//T35 End
    vDrvLVDSOn();
	CustomMarkPowerSequenceStart(E_POWER_SEQ_T36);//T36 Start
#ifdef CC_MHL_HDMI_SWITCH  
		  if(!_bfgMhlInit)
		  {
			vMhlSetPanelPower(1); //push rst high
			 vMhlInit();
			_bfgMhlInit = TRUE;
		  }
#endif
#ifndef CC_MTK_LOADER
    UNUSED(x_timer_stop(hLVDSOutputTimer));
    UNUSED(x_timer_delete(hLVDSOutputTimer));
#endif /* CC_MTK_LOADER */
}

void CustomPowerOnSequence(void)
{
    CRIT_STATE_T rCritState;
    
    UNUSED(CustomPowerOnSequence);
    //INT32 i4Value = 0;
	
	// for pwm setting before backlight enable on 
	// JL@111214 start
	//----------------------------------------------------
   // BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), (UINT32)_u4BrightLevel);
    if (DRVCUST_PanelGet(eFlagBacklightDimmingLock))
        vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_ON);
    else
        vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_OFF);

    vDrvLVDSOff();

    Custom60HzPowerOnSequence();
    
    // Turn on backlight
    PRINT_TIMESTAMP_DBG_MSG("[Power On]", "BL_ON = %d\n",
                            DRVCUST_PanelGet(eBackLightOnVal));

	
	POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T36);//T36 End
	if(_bfgCustPowerOnSeqPWMON == TRUE) // modify for backlght sequence, for MHL will influence the delay
	{
		BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), 255);		
		DRVCUST_SetScanPWM_High_Low(1000);
        vDRVCUST_BLSet_IP_REF(50);
	}
    vDrvTurnOnBacklt();
	CustomMarkPowerSequenceStart(E_POWER_SEQ_T41);//T41 start
#if 0//def CC_MHL_HDMI_SWITCH  
			if(!_bfgMhlInit)
			{
			  vMhlSetPanelPower(1); //push rst high
			  vMhlInit();
			  _bfgMhlInit = TRUE;
			}
#endif
    CustomUpdateBacklightState(TRUE);
	PRINT_TIMESTAMP_MSG("[Power On]", "DRVCUST_SetScanPWM_High_Low\n\n");

	//modify for sony baclight sequence
	//DRVCUST_SetScanPWM_High_Low(100);
	
	PRINT_TIMESTAMP_MSG("[Power On]", "END DRVCUST_SetScanPWM_High_Low\n\n");
	if(_bfgCustPowerOnSeqPWMON == TRUE)
	{
	    POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T41);//T41 start
		CustomStartDimmerOutput();
		_bfgCustPowerOnSeqPWMON = FALSE;
	}
	PRINT_TIMESTAMP_MSG("[Power On]", "END_DIMMER_OUTPUT\n");

    // Read RGB sensor
    PRINT_TIMESTAMP_MSG("[Power On]", "RGB sensor = (none)\n");

    // Start Self Diagnostic
    PRINT_TIMESTAMP_MSG("[Power On]", "Start Self Diagnostic\n");
    SELF_DIAG_SET_MODEL_TYPE(_u4gModelType);
    
#if defined (CC_SELF_DIAG_DEFAULT_ON)
    SELF_DIAG_START(D_SYSTEM_PANEL_ONOFF);
#endif

    // Update the power sequence status to be "done"
    rCritState = x_crit_start();
    CustomSetPowerSeqStatus(TRUE);
    x_crit_end(rCritState);
	{
		DTVCFG_T rDtvCfg;
		EEPDTV_GetCfg(&rDtvCfg);
		if(rDtvCfg.u1Flags & DTVCFG_FLAG_POWER_LEDOFF)//d130805_Haibo:LED off mode.No IRRC response led
		{
		     IRRC_LedBlinkConfig(FALSE);
		}
		else
		{
		     IRRC_LedBlinkConfig(TRUE);
		}
	}	
}

void CustomPowerOffSequence(void)
{
	  CRIT_STATE_T rCritState;
    
    UNUSED(CustomPowerOffSequence);
    
    IRRC_LedBlinkConfig(FALSE);
    CustomUpdateBacklightState(FALSE);
    
    // Update the power sequence status to be "not finished"
    rCritState = x_crit_start();
    CustomSetPowerSeqStatus(FALSE);
    x_crit_end(rCritState);

    // Move to customer setup power down function    
#if 0    
    // Stop Self Diagnostic
    PRINT_TIMESTAMP_MSG("[Power Off]", "Stop Self Diagnostic\n");
    SELF_DIAG_STOP(D_SYSTEM_PANEL_ONOFF);

    // Stop AC detection
    PRINT_TIMESTAMP_MSG("[Power Off]", "Stop AC_DET\n");
    CustomStopACDetect();
#endif    

    // Pause Self Diagnostic
    PRINT_TIMESTAMP_MSG("[Power Off]", "Pause Self Diagnostic\n");
    SELF_DIAG_PAUSE();

    // backlight power -> lvds sugnal -> lvds power
    PRINT_TIMESTAMP_DBG_MSG("[Power Off]", "BL_ON = %d\n",
                            (INT32)!DRVCUST_PanelGet(eBackLightOnVal));

    {
        BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), 0);
        vDRVCUST_BLSet_IP_REF(0);
    	DRVCUST_SetScanPWM_High_Low(0);
    	vDrvTurnOffBacklt(); // Backlight off
    }
	CustomMarkPowerSequenceStart(E_POWER_SEQ_T39);//T39_S
    
	_bfgCustPowerOnSeqPWMON = TRUE;
    PRINT_TIMESTAMP_MSG("[Power Off]", "LVDS_OUTPUT = 0\n");
	POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T39);//T39_E
    vDrvLVDSOff();
    CustomMarkPowerSequenceStart(E_POWER_SEQ_T38);//T38_S
    // Clear the LVDS output status
    rCritState = x_crit_start();
    _bfgLVDSOutput = FALSE;
    x_crit_end(rCritState);

    // Panel power power can not turn off right now
    // It will hang SIF bus if the panel power is down.
    PRINT_TIMESTAMP_DBG_MSG("[Power Off]", "LVDS_PWR_ON = %d\n",
                            (INT32)!DRVCUST_PanelGet(ePanelPowerOnVal));
	POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T38);//T38_E						
    vDrvTurnOffPanel(); // Panel off
#ifdef CC_MHL_HDMI_SWITCH
    vMhlSetPanelPower(0); //push rst low
#endif

    #if 0 
    fgSil_MHL_Enable_MLoop(FALSE);
    x_thread_delay(10);
    fgSil_Force_MHL_Mode();
    #else
//NOT TO CALL THIS FUNCTION SINCE IT WILL CAUSE SYSTEM HANG
//    fgSil_MHL_Enable_MLoop(FALSE);
//    fgSil_ChipRst(10);
    #endif

}

#ifdef CC_AUD_SETUP_MELODY 
static void _StartLogoThread(void *pvArgs)
{
    //Unmute Audio
    ADAC_Enable(TRUE);
    ADAC_Mute(FALSE);
    ADAC_CodecMute(FALSE);

    x_thread_delay(50); // For PWM first power on pop noise.
    ADAC_GpioCodecMute(FALSE);
    ADAC_SpeakerEnable(TRUE);

    //Play Setup Melody
    AUD_PlaySetupMelody(AUD_DEC_MAIN, 3, 1);
}
#endif

void CustomDisplayLogo(void)
{
    DTVCFG_T rDtvCfg;
    
#ifdef CC_AUD_SETUP_MELODY 
    HANDLE_T hThread;
#endif

#ifdef CUSTOM_POWER_STATE_CTRL_ENABLE
    if (CustomQueryPowerState() != STATE_POWER_ON)
    {
        return;
    }
#endif /* CUSTOM_POWER_STATE_CTRL_ENABLE */
    
    if (EEPDTV_GetCfg(&rDtvCfg) == 0)
    {
        BOOL bIsShowLogo = (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_DISABLE_LOGO) ?
                           FALSE : TRUE;
        if (bIsShowLogo)
        {
            BOOL fgFinished = FALSE;
            UNUSED(d_custom_misc_set(DRV_CUSTOM_MISC_TYPE_STARTUP_LOGO_STATUS,
                   &fgFinished, sizeof(fgFinished), FALSE));
            
            vApiPanelPowerSequence(TRUE);
            
    #ifdef CC_AUD_SETUP_MELODY
            if (bApiEepromReadByte(EEP_BOOT_MUSIC_ONOFF) & AUD_POWER_ON_MUSIC_MASK)
            {
                x_thread_create(&hThread, LOGO_THREAD_NAME, LOGO_THREAD_STACK_SIZE,
                                LOGO_THREAD_PRIORITY, _StartLogoThread, 0, NULL);
            }
    #endif
            
            OSD_ShowLogo();
            
            fgFinished = TRUE;
            UNUSED(d_custom_misc_set(DRV_CUSTOM_MISC_TYPE_STARTUP_LOGO_STATUS,
                                     &fgFinished, sizeof(fgFinished), FALSE));
        }                
    }
}


