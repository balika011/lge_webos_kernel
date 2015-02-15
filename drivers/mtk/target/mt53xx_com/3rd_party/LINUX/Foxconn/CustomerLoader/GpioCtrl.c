
//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_timer.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_gpio.h"
#include "x_pdwnc.h"
#include "x_util.h"

#include "ir_if.h"
#include "panel.h"
#include "eeprom_if.h"
#include "PowerSeq.h"
#include "CustomCtrl.h"
#include "Fiji_pdwnc.h"
#include "hw_hdmi.h"
//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
void GPIO_CustomPowerOnFunc(void);
void GPIO_CustomPowerOffFunc(UINT32 u4PowerDownEn);
void GPIO_CustomLedCtrlFunc(LED_STATUS_T eLedStatus);
UINT32 CustomPCBAQueryFunc(VOID);
UINT32 CustomPanelQueyFunc(VOID);
BOOL ICustomPCBARGBQueryFunc(VOID);

extern BOOL PDWNC_Check_Enter_SemiStandy(void);

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
UINT32 _u4gPanelType = CUSTOM_GPIO_PANEL_WXGA;
UINT32 _u4gPCBType = CUSTOM_PCB_TYPE_1_WXGA;
UINT32 _u4gFactoryType = CUSTOM_GPIO_FACTORY_WISTRON;
UINT32 _u4gLVDSType = CUSTOM_GPIO_LVDS_8BIT;
UINT32 _u4gModelType = 0;

BOOL   _bgIsCMO37Panel = FALSE;
BOOL   _bgIsThermalDetect = TRUE;
BOOL   _bgIsLightDetect = TRUE;
    
//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
UINT32 CustomPCBAQueryCountry(VOID)
{
    UINT8  u1Val = 0;
    UINT64 u8Offset = (UINT64)CUSTOM_COUNTRY_EXT_ADDR;
    static BOOL bInit = TRUE;
    static UINT32 u4Country = DRV_CUSTOM_COUNTRY_EU;

    if (!bInit)
    {
        return u4Country;
    }

    if (EEPROM_Read(u8Offset, (UINT32)(void *)&u1Val, 1) == 0)
    {
    	switch (u1Val)
    	{
    	case DRV_CUSTOM_COUNTRY_OPTION_EU:
    	    u4Country= DRV_CUSTOM_COUNTRY_EU;
    	    break;
    	case DRV_CUSTOM_COUNTRY_OPTION_PAD:
    	    u4Country= DRV_CUSTOM_COUNTRY_PAD;
    	    break;
    	case DRV_CUSTOM_COUNTRY_OPTION_PAA:
    	    u4Country= DRV_CUSTOM_COUNTRY_PAA;
    	    break;
    	case DRV_CUSTOM_COUNTRY_OPTION_COL:
    	    u4Country= DRV_CUSTOM_COUNTRY_COL;
    	    break;
    	}
    }
    
	return u4Country;
}

UINT32 CustomPCBAQueryFunc(VOID)
{
    UNUSED(CustomPCBAQueryFunc);
    
    switch (_u4gPCBType)
    {
    case CUSTOM_PCB_TYPE_1_WXGA:
    case CUSTOM_PCB_TYPE_1_FHD:
        return 1;
    case CUSTOM_PCB_TYPE_2:
        return 2;
    case CUSTOM_PCB_TYPE_3:
        return 3;
    case CUSTOM_PCB_TYPE_5:
        return 5;
    }
    
    /*should not reach here*/
    return 1;
}

/* return CUSTOM_GPIO_PANEL_WXGA / CUSTOM_GPIO_PANEL_FHD */
UINT32 CustomPanelQueyFunc(VOID)
{
    UNUSED(CustomPanelQueyFunc);
    
    return _u4gPanelType;
}

BOOL CustomPCBARGBQueryFunc(VOID)
{
    UNUSED(CustomPCBARGBQueryFunc);
    return FALSE;
}

UINT32 CustomModelQueryFunc(VOID)
{
    UNUSED(CustomModelQueryFunc);
    
    return _u4gModelType;
}

UINT32 CustomModelOptionQueryFunc(VOID)
{   
    UINT64 u8Offset;
    static BOOL bInit = TRUE;
    static UINT8 u1Index = 0;
 
    UNUSED(CustomModelOptionQueryFunc); 
    
    if (bInit)
    {
        u8Offset = (UINT64)CUSTOM_MODEL_EXT_ADDR;
        
        if (EEPROM_Read(u8Offset, (UINT32)(void *)&u1Index, 1) == 0)
        {
            /* Model Type:
                0: E,   1: E',  2: L,   3: P,   4: C,   5: F1,  6: F1S
            */
	     if (u1Index > 6)
            {
                 u1Index = 0;
            }
        }
        
        bInit = FALSE;
    }
    
	return u1Index;
}

void GPIO_CustomReadStrapPin(void)
{
    // Set gpio 15 as the input pin
    UINT8 au1Index[] = {PANEL_INVALID, PANEL_INVALID, PANEL_INVALID};
    QUERY_TYPE_T arKey[] = {ePanelIndexOffset1, ePanelIndexOffset2,
                            ePanelIndexOffset3};
    UINT32 u4Size = sizeof(arKey) / sizeof(QUERY_TYPE_T);
    UINT32 u4Value, i;
    
    UNUSED(GPIO_Input(CUSTOM_GPIO_PANEL_NVM_WP));
    
#ifdef CC_3D_SUPPORT
#ifndef CC_MTK_LOADER
    //turn on/off 3D according to 3D option byte
    UINT8 u1PlfOpt3DSupport = 0;
    UNUSED(EEPROM_Read(0x6C5, (UINT32)(VOID *)&u1PlfOpt3DSupport, sizeof(UINT8)));
                    
    if (0 == u1PlfOpt3DSupport)
    {
        //turn off 3D
        vDrvTDTVForceChipSupport3DOFF(1);
    }
    else
    {
        vDrvTDTVForceChipSupport3DOFF(0);
    }
#endif
#endif

#ifdef FORCE_FHD_PANEL
    _u4gPCBType =  DRV_CUSTOM_MODEL_FHD_PANEL ;
    _u4gPanelType = CUSTOM_GPIO_PANEL_FHD;
#else
    _u4gPCBType = DRV_CUSTOM_MODEL_WXGA_PANEL;
    _u4gPanelType = CUSTOM_GPIO_PANEL_WXGA;        
#endif

    _u4gModelType |= (_u4gFactoryType == CUSTOM_GPIO_FACTORY_WISTRON) ?
        DRV_CUSTOM_MODEL_FACTORY_WISTRON : DRV_CUSTOM_MODEL_FACTORY_SONY;
    _u4gModelType |=_u4gPCBType ;

    // Determin Light sensor enable/disable by Option bit
    
#if defined(CC_3D_SUPPORT) && (!defined(CC_MTK_LOADER))
    Printf("[GPIO_CustomReadStrapPin] _u4gModelType = 0x%2x _u4gPanelType=%d, 3D=%d\n", _u4gModelType, _u4gPanelType, u1PlfOpt3DSupport);
#else
    Printf("[GPIO_CustomReadStrapPin] _u4gModelType = 0x%2x _u4gPanelType=%d\n", _u4gModelType, _u4gPanelType);
#endif    

    if ((DRVCUST_PanelQuery(ePanelIndexFromEeprom, &u4Value) == 0) &&
        (u4Value == 1))
    {
        // read configuration from eeprom
        for (i = 0; i < u4Size; i++)
        {
            if (DRVCUST_PanelQuery(arKey[i], &u4Value) == 0)
            {
                UNUSED(EEPROM_Read((UINT64)u4Value, (UINT32)&au1Index[i], 1));
            }
        }
        // Write the default panel index to EEPROM
        if ((au1Index[0] == PANEL_INVALID) && (au1Index[1] == PANEL_INVALID) &&
            (au1Index[2] == PANEL_INVALID))
        {
#ifndef CC_MANUAL_PANEL_SEL        
            UINT32 u4DefaultPanel = DEFAULT_PANEL_WXGA_SELECT;
        
            if(_u4gPanelType == CUSTOM_GPIO_PANEL_NONTCON)
            {
                u4DefaultPanel =  DEFAULT_PANEL_NONTCON_SELECT ;
            }
            else if (_u4gPanelType == CUSTOM_GPIO_PANEL_WXGA)
            {
                u4DefaultPanel =  DEFAULT_PANEL_WXGA_SELECT ;
            }
            else  //fULL HD
            {
                u4DefaultPanel =  DEFAULT_PANEL_FHD_SELECT ;
            }
            Printf("[GPIO_CustomReadStrapPin] u4DefaultPanel = %d\n", u4DefaultPanel);
#else /*CC_MANUAL_PANEL_SEL*/
            UINT32 u4DefaultPanel = (_u4gPanelType == CUSTOM_GPIO_PANEL_FHD) ? 
            DEFAULT_PANEL_FHD_SELECT : DEFAULT_PANEL_WXGA_SELECT;
#endif/*CC_MANUAL_PANEL_SEL*/
            WritePanelIndexToEeprom(u4DefaultPanel);
        }
    }
}
void GPIO_CustomPowerOnFunc(void)
{
    
#ifdef CC_AC_DETECT_ENABLE
    HAL_TIME_T rTime;
#endif /* CC_AC_DETECT_ENABLE */    
#ifdef CC_MTK_LOADER
    UINT32 i;
    UINT32 polarity;
#endif
    UNUSED(GPIO_CustomPowerOnFunc);

#ifdef CC_MTK_LOADER
    //Init for hotel mode , defualt to off
    GPIO_SetOut(DRV_CUSTOM_IR_PASS_THROUGH_GPIO, DRV_CUSTOM_IR_PASS_THROUGH_GPIO_OFF);
#if 0    
	vRegWrite1B(PD_SYS_2, 0xff);
	vRegWriteFldAlign(ANA_INTF_1, 0x1, RG_HDMI_TERM_EN);	
	vRegWriteFldAlign(ANA_INTF_2, 0x1, RG_HDMI_SW_EN);
#endif
			
    // Audio mute circuit mute in loader
    if (DRVCUST_InitQuery(eAudioMuteGpioNum, &i) == 0)
    {
        UNUSED(DRVCUST_InitQuery(eAudioMuteGpioPolarity, &polarity));
        GPIO_SetOut(i, polarity);
    }

    if (DRVCUST_InitQuery(eAudioCustSpec2GpioCtrl, &i) == 0)
    {
        if (i != 0xffffffff)
        {
            Printf("[Power On] Mute Headphone/Audio Out to for AC POP noise\n");
            GPIO_SetOut(i, 1);
        }
    }

    if (!PDWNC_Check_Enter_SemiStandy())
    {
        GPIO_CustomLedCtrlFunc(eLedLoaderPowerOn);
    }

#else /* CC_MTK_LOADER */

    GPIO_CustomReadStrapPin();

    if (PDWNC_Check_Enter_SemiStandy())
    {
        IRRC_LedBlinkConfig(FALSE);
    }
    CustomIRLedHandleStart();
    
#ifdef DRV_3RD_PARTY_CUSTOM_INIT    /* 3rd Pary driver init routine */
    DRV_3RD_PARTY_CUSTOM_INIT();
#endif

#ifdef CC_AC_DETECT_ENABLE
    // Start AC detection
    //HAL_TIME_T rTime;
    HAL_GetTime(&rTime);
    Printf("[Power On]");
    Printf(" (%06d:%03d) ", rTime.u4Seconds, (rTime.u4Micros / 1000));
    Printf("Start AC_DET\n");      
    CustomStartACDetect();
#endif /* CC_AC_DETECT_ENABLE */
    
#endif /* CC_MTK_LOADER */
}

void GPIO_CustomPowerOffFunc(UINT32 u4PowerDownEn)
{
    UINT32 i;
    INT32 i4Val;
    BOOL bOnTimer = (u4PowerDownEn & WAK_RTC) ? TRUE : FALSE;
    
    UNUSED(GPIO_CustomPowerOffFunc);

#ifndef CC_MTK_LOADER
    CustomIRLedHandleStop();
#endif /* CC_MTK_LOADER */

    
    CustomSetTimerLED(bOnTimer);

    if (u4PowerDownEn & WAK_VGA)
    {
        GPIO_CustomLedCtrlFunc(eLedVgaStandby);
    }
    else
    {
#ifdef CC_MTK_LOADER
        GPIO_CustomLedCtrlFunc(eLedLoaderPowerOff);
#else /* CC_MTK_LOADER */
        GPIO_CustomLedCtrlFunc(eLedSystemPowerOff);
#endif /* CC_MTK_LOADER */
    }

#ifdef SIGNAL_X_U_RST
    INT32 i4Value = 0;
    UNUSED(GPIO_Output(SIGNAL_X_U_RST, &i4Value));
    PRINT_TIMESTAMP_DBG_MSG("[Power Off]", "X_U_RST = %d\n", i4Value);
#endif

#ifndef CC_MTK_LOADER
    // Stop Self Diagnostic
    PRINT_TIMESTAMP_MSG("[Power Off]", "Stop Self Diagnostic\n");
    SELF_DIAG_STOP(D_SYSTEM_PANEL_ONOFF);

    // Stop AC detection
    PRINT_TIMESTAMP_MSG("[Power Off]", "Stop AC_DET\n");
    CustomStopACDetect();
#endif /* CC_MTK_LOADER */

    DOWN_SET_GPIO(eBackLightGpio, eBackLightOnVal);
    DOWN_SET_GPIO(ePanelPowerGpio, ePanelPowerOnVal);
    DOWN_SET_GPIO(ePowerCtrlGpio, ePowerCtrlOnVal);
}

