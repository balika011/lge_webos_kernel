
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
#include "maple_drv/maple_pdwnc.h"
#include "hw_hdmi.h"
#include "panel_cust.h"
#include "drv_display.h"
#ifdef CC_MTK_LOADER
#include "drv_lvds.h"
#include "ostg_if.h"
#include "hw_tcon.h"
#include "drv_tcon.h"
#include "drv_vbyone.h"
#include "drv_pwm.h"
#include "drvcust_if.h"
#endif
//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
void GPIO_CustomPowerOnFunc(void);
void GPIO_CustomPowerOffFunc(UINT32 u4PowerDownEn);
void GPIO_CustomLedCtrlFunc(LED_STATUS_T eLedStatus);
UINT32 CustomPCBAQueryFunc(VOID);
UINT32 CustomPanelQueyFunc(VOID);
BOOL ICustomPCBARGBQueryFunc(VOID);
void ATV_60HzReadPanelID(UINT8 *pu1PanelID);

extern BOOL PDWNC_Check_Enter_SemiStandy(void);

//#ifndef CC_MTK_LOADER
//extern void d_sony_2k12_cust_drv_init(void);
//#endif


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
UINT32 _u4gPanelType = CUSTOM_GPIO_PANEL_WXGA;
UINT32 _u4gPCBType = CUSTOM_PCB_TYPE_1_WXGA;
UINT32 _u4gFactoryType = CUSTOM_GPIO_FACTORY_WISTRON;
UINT32 _u4gLVDSType = CUSTOM_GPIO_LVDS_8BIT;
UINT32 _u4gModelType = 0;
//#ifdef Panel_ID_ERROR_DETECT
UINT32 PANEL_DET =0;
UINT32 PANEL_I2C_ACK =FALSE;
//#endif

BOOL   _bgIsCMO37Panel = FALSE;
BOOL   _bgIsThermalDetect = TRUE;
BOOL   _bgIsLightDetect = TRUE;

#define EXT_FRC_I2C_ADDRESS  0x34  
#define EXT_FRC_I2C_BUS       2 

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
extern void PDWNC_Reboot(void);
extern void vDrvTurnOffPanel(void);
extern void vDrvTurnOffBacklt(void);
#if 1
extern SELF_DIAG_TABLE _eSelfDiagEnable;
extern void PDWNC_Custom_EnterNextState(UINT32 u4NextState);
extern void PDWNC_Custom_ReadWakeupReason(UINT32* pu4Reason);

void CustomPanelNimError(void)
{
    UINT8 u1State=0;    
	UNUSED(EEPROM_Read(PANEL_ID_STORE_COUNT, 
					   (UINT32)(VOID *)&u1State, sizeof(UINT8)));
#ifdef CC_MTK_LOADER
    UINT32 u4Reason = 4;
  if(u1State!=1)
  	{
  	u1State =1;
  	EEPROM_Write(PANEL_ID_STORE_COUNT,(UINT32)&u1State,sizeof(UINT8)); 
    PDWNC_Custom_ReadWakeupReason(&u4Reason);
    if(30 != u4Reason)
        {
          EEPROM_Write(WakeupReasonFlag,(UINT32)&u4Reason,sizeof(UINT32));
        } 
	PDWNC_Reboot();
  	}
  else if(u1State==1)
  	{
  	u1State =0;
    UINT32 u4Next = STATE_POWER_ON;
  	EEPROM_Write(PANEL_ID_STORE_COUNT,(UINT32)&u1State,sizeof(UINT8));
  	Printf("SelfDiagSetShutdown(D_PANEL_NVM_ERROR); \n");
	PDWNC_Custom_EnterNextState(u4Next);
    PDWNC_Custom_ReadWakeupReason(&u4Reason);
    if(30 != u4Reason)
        {
          EEPROM_Write(WakeupReasonFlag,(UINT32)&u4Reason,sizeof(UINT32));
        }
  	}
 #endif
}
#endif

#ifndef CC_MTK_LOADER//add for Sipadan 
/*
	get the Panel NVM address width 0: 1byte  1 :2 bytes
	 *  @retval   0        SUCCESS.
 *  @retval   1        FAIL.
*/
INT32 a_nvm_get_i2c_addr_width(UINT8* pui1_width)
{
	return (EEPROM_Read(PANEL_ID_READ_ADDR, 
                       (UINT32)(VOID *)pui1_width, sizeof(UINT8)));
}
#endif /*CC_MTK_LOADER*/

void ATV_60HzReadPanelID(UINT8 *pu1PanelID)
{
	//UINT32 u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
	UINT32 u4DefaultPanel= 0;//DEFAULT_PANEL_FHD_SELECT;
	BOOL   bMatch = FALSE;
	UINT8  i,u1Readaddr=0,u1PaneledidAddr=0;
	UINT8  au1PanelID[3][ATV_PANEL_ID_STRING_LEN];
	UINT32 u4Len;
	UINT32 u4ErrorCnt = 0;
	
	// Read panel ID by SIF bus 
	// Device address : 0xAA, sub address : 0xE0
	SIF_Init();
	UNUSED(EEPROM_Read(PANEL_ID_PANEL_MODE, (UINT32)(VOID *)&PANEL_DET, sizeof(UINT8)));
	switch(PANEL_DET)
	{
	    case PANEL_DET_DIABLE:
		{
		  Printf("Disable Panel detected \n");
		  break;
		}
		case PANEL_LESS_MODE:
		case PANEL_MODE:
		case PANEL_DISABLE_LESS_MODE:
		default:
		{
		  Printf("Panel detected \n");
		  UNUSED(EEPROM_Read(PANEL_ID_READ_ADDR, (UINT32)(VOID *)&u1Readaddr, sizeof(UINT8)));
		  if(u1Readaddr ==0xFF)
		  {
		    u1Readaddr = PANEL_EDID_READ_0X00E0;
			EEPROM_Write(PANEL_ID_READ_ADDR,(UINT32)(VOID *)&u1Readaddr,sizeof(UINT8));
		  }
			if(u1Readaddr == PANEL_EDID_READ_0XE0)
			{
			  u1PaneledidAddr = ATV_60HZ_PANEL_ID_ON_NVM_OFFSET;
			  Printf("PANEL_EDID_READ_0XE0 \n");
			  for (i = 0; i < 3; i++)
			  {
			    x_memset(&au1PanelID[i][0], 0, ATV_PANEL_ID_STRING_LEN);						   
				u4Len = SIF_X_Read(ATV_60HZ_PANEL_SIF_BUS,ATV_60HZ_PANEL_SIF_CLK_DIV, ATV_60HZ_PANEL_NVM_ADDRESS, 0x1,
								   u1PaneledidAddr, &au1PanelID[i][0], ATV_PANEL_ID_STRING_LEN);
				Printf("ATV_60HzReadPanelID I2C Len = %d\n",u4Len);
			    if (u4Len != ATV_PANEL_ID_STRING_LEN)
			    {
				  u4ErrorCnt++;
				}
			  }
			}
			else
			{
			    u1PaneledidAddr = ATV_60HZ_PANEL_ID_ON_NVM_OFFSET;
				Printf("PANEL_EDID_READ_0X00E0 \n");
				for (i = 0; i < 3; i++)
				{
				  x_memset(&au1PanelID[i][0], 0, ATV_PANEL_ID_STRING_LEN);	   
				  u4Len = SIF_X_Read(ATV_60HZ_PANEL_SIF_BUS,ATV_60HZ_PANEL_SIF_CLK_DIV, ATV_60HZ_PANEL_NVM_ADDRESS, 0x2,
									 u1PaneledidAddr, &au1PanelID[i][0], ATV_PANEL_ID_STRING_LEN);
                  Printf("ATV_60HzReadPanelID I2C Len = %d\n",u4Len);
				  if (u4Len != ATV_PANEL_ID_STRING_LEN)
				  {
				    u4ErrorCnt++;
				  }
				}
			}
			// Check if 2 strings are mached or not
			if (u4ErrorCnt < 2)
			{
			  if (x_memcmp(&au1PanelID[0][0], &au1PanelID[1][0], ATV_PANEL_ID_STRING_LEN) == 0)
			  {
			    x_memcpy(pu1PanelID, &au1PanelID[0][0], ATV_PANEL_ID_STRING_LEN);
				bMatch = TRUE;
			  }
			  else
			  {
				if (x_memcmp(&au1PanelID[0][0], &au1PanelID[2][0], ATV_PANEL_ID_STRING_LEN) == 0)
				{
				  x_memcpy(pu1PanelID, &au1PanelID[0][0], ATV_PANEL_ID_STRING_LEN);
				  bMatch = TRUE;
				}
				else if (x_memcmp(&au1PanelID[1][0], &au1PanelID[2][0], ATV_PANEL_ID_STRING_LEN) == 0)
				{
				  x_memcpy(pu1PanelID, &au1PanelID[1][0], ATV_PANEL_ID_STRING_LEN);
				  bMatch = TRUE;
				}
			  }
			}
			else
			{
			  if(PANEL_DET ==PANEL_DISABLE_LESS_MODE)
			  {
			    CustomPanelNimError();
			  }
			  else
			  {
				PANEL_DET =PANEL_LESS_MODE;
				EEPROM_Write(PANEL_ID_PANEL_MODE,(UINT32)(VOID *)&PANEL_DET,sizeof(UINT8));
				Printf("[Power On] Panel ID is not on the panel table, load panel 15\n");
				//UINT32 u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
				//u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
				u4DefaultPanel= 0;
				WritePanelIndexToEeprom(u4DefaultPanel); 	   
			  }
			  PANEL_I2C_ACK =FALSE;
			  return ;
			}
			if (!bMatch)
			{  
			  CustomPanelNimError();
			}
			else
			{
			  if(PANEL_DET !=PANEL_DISABLE_LESS_MODE)
			  {
				PANEL_DET = PANEL_MODE;
				EEPROM_Write(PANEL_ID_PANEL_MODE,(UINT32)(VOID *)&PANEL_DET,sizeof(UINT8));
			  }
			}
			PANEL_I2C_ACK=TRUE;
			break;
		}
	}
	return;
}

#ifdef Panel_ID_ERROR_DETECT
extern void WritePanelIndexToEeprom(UINT32 u4PanelIndex);
extern BOOL CustomPanelLoadTableFromFlash(UINT32 u4Type, void *pData);
extern UINT32 FlashPanel_GetID(void);
#define LOAD_PANEL_FROM_FLASH_BY_ID     0
#define LOAD_PANEL_FROM_FLASH_BY_NAME     1
void LedPanelIDFailFunction(void);
UINT8 _fgeLEDPanelIDFail =FALSE;

void ATV_CheckPanelID(VOID *pPanelID)
{
    #ifndef CC_UBOOT
	  #ifndef Panel_ID_ERROR_DETECT
    //UINT32 u4DefaultPanel= DEFAULT_PANEL_WXGA_SELECT;
	UINT32 u4DefaultPanel= 0;
    #endif
    
    if (!CustomPanelLoadTableFromFlash(LOAD_PANEL_FROM_FLASH_BY_NAME, pPanelID))
    {
    #ifdef Panel_ID_ERROR_DETECT
	//#ifdef CC_MTK_LOADER
	  Printf("[Power On] Panel ID is not on the panel table--> hang\n");
	  EEPROM_Write(0x2B90,(UINT32)pPanelID,16);
	  _fgeLEDPanelIDFail =TRUE;
	  LedPanelIDFailFunction(); //need to open it
	//#endif
	#else
      Printf("[Power On] Panel ID is not on the panel table\n");
      //UINT32 u4DefaultPanel= DEFAULT_PANEL_WXGA_SELECT;
      //u4DefaultPanel= DEFAULT_PANEL_WXGA_SELECT;
	  u4DefaultPanel= 0;  
      Printf("[Power On] Write Default panel ID\n");
      WritePanelIndexToEeprom(u4DefaultPanel);
	#endif
    }
    else
    {
        Printf("[Power On] Write panel ID to EEPROM\n");
        WritePanelIndexToEeprom(FlashPanel_GetID());    // Write panel ID to EEPROM        
   
       // x_thread_delay(100);

        // Need to reboot
     //   Printf("[Power On] Reboot !!\n");
       // CHIP_Reboot();
    }
	#endif
}

//#ifdef CC_MTK_LOADER
void LedPanelIDFailFunction(void)
{
    PFN_GPIO_LEDCTRL_FUNC pfnLedCtrlFunc = NULL;
	if(_fgeLEDPanelIDFail)
	{
      vDrvTurnOffPanel();
	  vDrvTurnOffBacklt();
	  UNUSED(DRVCUST_InitQuery(eGpioLedCtrlFunc, (UINT32 *)(void *)&pfnLedCtrlFunc));
	  if (pfnLedCtrlFunc != NULL)
	  {
	    pfnLedCtrlFunc(eLedPanelIDFail);
	  }
    }
}
//#endif
//PANEL_DET
#endif

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
#if 0
static UINT32 _read_plf_opt_model(void)
{
    UINT8 u1PlfOptModel = 0;
    UINT32 u4Buf = 0;
    EEPROM_Read(0x6CB, (UINT32)(VOID*)(&u1PlfOptModel), 1);
    u4Buf = u1PlfOptModel;
    u4Buf <<= 16;
    return u4Buf;
}
#endif
#ifdef Panel_ID_ERROR_DETECT
void CustomPanelDetectFlow(VOID)
{
    UINT8  au1PanelID[16];
	UINT8  u1State =1;
	INT32  i4PanelStatus;
	UINT32 u4PanelDetMode;
    UNUSED(CustomPanelDetectFlow);

    x_thread_delay(20);
	//GPIO_SetOut(51, 0);
	//GPIO_SetOut(52, 0);	
    x_thread_delay(10);
	#ifdef SONY_LOADER_POWER_SEQ
	#ifdef CC_MTK_LOADER
    x_thread_delay((10 * PANEL_GetLvdsOnDalay()) + 1);
    switch(PANEL_GetDisplayType())	
    {
        default:
        case PANEL_DISPLAY_TYPE_MAX:
            // error display type, defaultly do nothing
            break;            
		case PANEL_DISPLAY_TYPE_LVDS:
            vDrvLVDSOff();
            vDrvLVDSConfig();
            vDrvLVDSOn();
            break;
        case PANEL_DISPLAY_TYPE_MLVDS:
            vDrvLVDSConfig();
            u1DrvTconInit();
            break;
	    #ifdef CC_MT5399
        case PANEL_DISPLAY_TYPE_VB1:
            vDrvLVDSOff();
            vDrvLVDSConfig();
            vVByOneInit();				
            vDrvVByOneOn();
            break;
	    #endif
    }
	#endif
    #endif
	x_thread_delay(80);
	//GPIO_SetOut(11, 1);
	//GPIO_SetOut(12, 1);
    i4PanelStatus = GPIO_GetIn(DRV_CUSTOM_PANEL_LESS_GPIO);
	if(i4PanelStatus == DRV_CUSTOM_PANEL_LESS_ON)
	{
		u4PanelDetMode = PANEL_LESS_MODE;
		EEPROM_Write(PANEL_ID_PANEL_MODE,(UINT32)(VOID *)&u4PanelDetMode,sizeof(UINT8));
	}
	else
	{
	  ATV_60HzReadPanelID(au1PanelID);
	}
	//GPIO_SetOut(11, 0);
	//GPIO_SetOut(12, 0);
	
	//vDrvTurnOffPanel();
	
    Printf("[Power On] Panel ID = %s\n", au1PanelID);        
    Printf("[Power On] PANEL_ID sequence\n");
	if(PANEL_I2C_ACK)
	{
  	EEPROM_Write(PANEL_ID_STORE_COUNT,(UINT32)&u1State,1);
    ATV_CheckPanelID((VOID *)au1PanelID);
	}
}
#endif
#if 1
void GPIO_CustomReadStrapPin(void)
{
#ifdef CC_COMBINE_4K_2K
    UINT32 u4SonyModelType;
#endif
	UINT8 u1SonyCISupport;
    #if 0
    UINT32 u4DefaultPanel = DEFAULT_PANEL_WXGA_SELECT;
    INT32 fgSet = 0;
    UINT8 au1Index[] = {PANEL_INVALID, PANEL_INVALID, PANEL_INVALID};
    QUERY_TYPE_T arKey[] = {ePanelIndexOffset1, ePanelIndexOffset2,
                            ePanelIndexOffset3};
    UINT32 u4Size = sizeof(arKey) / sizeof(QUERY_TYPE_T);
    UINT32 u4Value, i;
	#endif
	#ifndef Panel_ID_ERROR_DETECT
	UINT8 au1Index[] = {PANEL_INVALID, PANEL_INVALID, PANEL_INVALID};
	#endif
    UINT32 u4Value;

        
#ifdef FORCE_FHD_PANEL
    _u4gPCBType =  DRV_CUSTOM_MODEL_FHD_PANEL ;
    _u4gPanelType = CUSTOM_GPIO_PANEL_FHD;
#else
    _u4gPCBType = DRV_CUSTOM_MODEL_WXGA_PANEL;
    _u4gPanelType = CUSTOM_GPIO_PANEL_WXGA;        
#endif    
    
    //GPIO_Output(CUSTOM_GPIO_PANEL_NVM_WP, &fgSet);
	//vDrvTurnOnPanel();
    //_u4gPanelType = (UINT32)GPIO_Input(CUSTOM_GPIO_PANEL_ID_PIN_0);
    _u4gPCBType = (_u4gPanelType == CUSTOM_GPIO_PANEL_WXGA) ? 
        DRV_CUSTOM_MODEL_WXGA_PANEL : DRV_CUSTOM_MODEL_FHD_PANEL;
	
    _u4gModelType |= (_u4gFactoryType == CUSTOM_GPIO_FACTORY_WISTRON) ?
        DRV_CUSTOM_MODEL_FACTORY_WISTRON : DRV_CUSTOM_MODEL_FACTORY_SONY;
    _u4gModelType |=_u4gPCBType ;

    //4K & 2K support get
    #ifdef CC_COMBINE_4K_2K
    if (DRVCUST_InitQuery(eGet2KModelSupport, &u4SonyModelType) == 0)
    {
    	if((u4SonyModelType&0x01)==0x00)
		_u4gModelType=_u4gModelType|DRV_CUSTOM_MODEL_NO_4K;
		else
		_u4gModelType=_u4gModelType|DRV_CUSTOM_MODEL_4K;
    }
	#endif
	//CI support  get
    EEPROM_Read(0x10086, (UINT32)(VOID*)(&u1SonyCISupport), 1);
	if(u1SonyCISupport & 0x01)
	_u4gModelType=_u4gModelType|DRV_CUSTOM_MODEL_CI;
	else
	_u4gModelType=_u4gModelType|DRV_CUSTOM_MODEL_NO_CI;
	
    //_u4gModelType |= _read_plf_opt_model();
    // Determin Light sensor enable/disable by Option bit
    
    Printf("[GPIO_CustomReadStrapPin] _u4gModelType = 0x%2x _u4gPanelType=%d\n", _u4gModelType, _u4gPanelType);
    vDrvTurnOnPanel(); //turn on lvds power for sif read
    //UINT8 au1Index[] = {PANEL_INVALID, PANEL_INVALID, PANEL_INVALID};
    //QUERY_TYPE_T arKey[] = {ePanelIndexOffset1, ePanelIndexOffset2,
    //                        ePanelIndexOffset3};
    //UINT32 u4Size = sizeof(arKey) / sizeof(QUERY_TYPE_T);
    //UINT32 u4Value, i;

    if ((DRVCUST_PanelQuery(ePanelIndexFromEeprom, &u4Value) == 0) &&
        (u4Value == 1))
    {
        #if 0
        // read configuration from eeprom
        for (i = 0; i < u4Size; i++)
        {
            if (DRVCUST_PanelQuery(arKey[i], &u4Value) == 0)
            {
                UNUSED(EEPROM_Read((UINT64)u4Value, (UINT32)&au1Index[i], 1));
            }
        }
		#endif
#ifdef Panel_ID_ERROR_DETECT
			  CustomPanelDetectFlow();
#else

        // Write the default panel index to EEPROM
        if ((au1Index[0] == PANEL_INVALID) && (au1Index[1] == PANEL_INVALID) &&
            (au1Index[2] == PANEL_INVALID))
        {
            WritePanelIndexToEeprom(0);
        }
#endif
    }
}
#endif
void GPIO_CustomPowerOnFunc(void)
{
#ifdef CC_AC_DETECT_ENABLE
    HAL_TIME_T rTime;
#endif /* CC_AC_DETECT_ENABLE */
#ifndef CC_MTK_LOADER
    INT32 i4Value;
#endif
#ifdef CC_COMBINE_4K_2K
    UINT32 u4SonyModelType;
#endif

#ifdef CC_MTK_LOADER
    UINT32 i;
    UINT32 polarity;

    //Init for hotel mode , defualt to off
    GPIO_SetOut(DRV_CUSTOM_IR_PASS_THROUGH_GPIO, DRV_CUSTOM_IR_PASS_THROUGH_GPIO_OFF);
#if 0    
	vRegWrite1B(PD_SYS_2, 0xff);
	vRegWriteFldAlign(ANA_INTF_1, 0x1, RG_HDMI_TERM_EN);	
	vRegWriteFldAlign(ANA_INTF_2, 0x1, RG_HDMI_SW_EN);
#endif

    UNUSED(GPIO_CustomPowerOnFunc);
    			
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

    //GPIO_CustomReadStrapPin();

    if (PDWNC_Check_Enter_SemiStandy())
    {
        IRRC_LedBlinkConfig(FALSE);
    }
    CustomIRLedHandleStart();
    
#ifdef DRV_3RD_PARTY_CUSTOM_INIT    /* 3rd Pary driver init routine */
    DRV_3RD_PARTY_CUSTOM_INIT();
#endif
    //d_sony_2k12_cust_drv_init(); //for RGB init

#if defined (CC_SELF_DIAG_DEFAULT_ON)
    #ifndef CC_MTK_LOADER
    SELF_DIAG_START(D_SYSTEM_PANEL_ONOFF);
	#endif
#endif

#ifdef CC_AC_DETECT_ENABLE
    // Start AC detection
    //HAL_TIME_T rTime;
    HAL_GetTime(&rTime);
    Printf("[Power On]");
    Printf(" (%06d:%03d) ", rTime.u4Seconds, (rTime.u4Micros / 1000));
    Printf("Start AC_DET\n");
    #ifdef CC_SOEM_ACDET_PAA
	Printf("set gpio %d to AC detection pin \n",SIGNAL_AC_DET);
	GPIO_Init();
	VERIFY(GPIO_Reg(SIGNAL_AC_DET, GPIO_TYPE_INTR_FALL, ACErrorDetect) >= 0);
    #else
	CustomStartACDetect();
    #endif
#endif /* CC_AC_DETECT_ENABLE */
    i4Value = WIFI_MODUDLE_RST_POLARITY;
    UNUSED(GPIO_Output(WIFI_MODUDLE_RST_GPIO, &i4Value));
	x_thread_delay(10);
	i4Value = 1-i4Value;
	UNUSED(GPIO_Output(WIFI_MODUDLE_RST_GPIO, &i4Value));
    Printf("WIFI_RST Done!!\n");
#ifdef CC_COMBINE_4K_2K	
	if (DRVCUST_InitQuery(eGet2KModelSupport, &u4SonyModelType) == 0)
	{
		if((u4SonyModelType&0x01)==0x00)
		{
			Printf("Now is 2K model \n");
			#ifdef CC_SAKURA_CVT_SUPPORT
			GPIO_SetOut(268,0);//Set SA mode low when init
			#else
			GPIO_SetOut(59,0);//Set SA mode low when init
			#endif
		}
		else
		{
			Printf("Now is 4K model \n");
		}
	}
	GPIO_SetOut(220,0);//set initial value STBY_MHL
	GPIO_SetOut(247,0);//set initial value OPWRSB
#endif
#endif /* CC_MTK_LOADER */
}

void GPIO_CustomPowerOffFunc(UINT32 u4PowerDownEn)
{	    
    UINT32 i;
    INT32 i4Val;
#ifdef SIGNAL_X_U_RST
    INT32 i4Value = 0;
#endif 
    BOOL bOnTimer = (u4PowerDownEn & WAK_RTC) ? TRUE : FALSE;
#ifdef CC_COMBINE_4K_2K
    UINT32 u4SonyModelType;
#endif
    	   
	  UNUSED(GPIO_CustomPowerOffFunc);
    
#ifndef CC_MTK_LOADER
    CustomIRLedHandleStop();
#endif /* CC_MTK_LOADER */

    //BOOL bOnTimer = (u4PowerDownEn & WAK_RTC) ? TRUE : FALSE;
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
    //INT32 i4Value = 0;
    UNUSED(GPIO_Output(SIGNAL_X_U_RST, &i4Value));
    PRINT_TIMESTAMP_DBG_MSG("[Power Off]", "X_U_RST = %d\n", i4Value);
#endif

#ifndef CC_MTK_LOADER
    #ifdef CUSTOM_SELF_DIAG_ENABLE
    // Stop Self Diagnostic
    PRINT_TIMESTAMP_MSG("[Power Off]", "Stop Self Diagnostic\n");
    SELF_DIAG_STOP(D_SYSTEM_PANEL_ONOFF);
    #endif
	#ifdef CC_AC_DETECT_ENABLE
    // Stop AC detection
    #ifdef CC_SOEM_ACDET_PAA
	PRINT_TIMESTAMP_MSG("[Power Off]", "Stop AC_DET\n");
    GPIO_Reg(SIGNAL_AC_DET, GPIO_TYPE_INTR_FALL, NULL);
	#else
	PRINT_TIMESTAMP_MSG("[Power Off]", "Stop AC_DET\n");
    CustomStopACDetect();
	#endif

	#endif
#endif /* CC_MTK_LOADER */

#ifdef CC_SAKURA_CVT_SUPPORT
    GPIO_SetOut(247,1);//OPWRSB
#endif
	GPIO_SetOut(219,1);//BOSTON_RST_CTRL
	//GPIO_SetOut(243,0);//WIFI/BT_RST
	GPIO_SetOut(208,0);//MHL_VBUS_EN1
	GPIO_SetOut(51,0);//X_AUDIO_MUTE_SCART 
	GPIO_SetOut(52,0);//X_AUDIO_MUTE_SPHP 
	GPIO_SetOut(67,0);//X_DAMP_RESET 
	GPIO_SetOut(205,0);//X_DAMP_MUTE
	GPIO_SetOut(45,0);//USB_PWR_EN0
	GPIO_SetOut(43,0);//USB_PWR_EN1
	GPIO_SetOut(44,0);//USB_PWR_EN2


    DOWN_SET_GPIO(eBackLightGpio, eBackLightOnVal);
#ifdef CC_COMBINE_4K_2K
	if (DRVCUST_InitQuery(eGet2KModelSupport, &u4SonyModelType) == 0)
	{
		if((u4SonyModelType&0x01)!=0x00)
		{
    DOWN_SET_GPIO(ePanelPowerGpio, ePanelPowerOnVal);
		}
	}
#endif
    DOWN_SET_GPIO(ePowerCtrlGpio, ePowerCtrlOnVal);
	//GPIO_SetOut(220,1);//STBY_MHL
}

#ifdef CC_PAA_GET_COUNTRY_SUPPORT
UINT32 CustomGetAPPCountry(void)
{
    UINT8  u1Val = 0;
    UINT64 u8Offset = (UINT64)0x2BE0;

    if (EEPROM_Read(u8Offset, (UINT32)(void *)&u1Val, 1) != 0)
    {
		Printf("CustomGetAPPCountry Fail!!\n");
    }
    
	return (UINT32)u1Val;
}

void  CustomSetAPPCountry(UINT8 u1PaaCountry)
{
    UINT8  u1Val = 0;
    UINT64 u8Offset = (UINT64)0x2BE0;

    u1Val = u1PaaCountry;

    if (EEPROM_Write(u8Offset, (UINT32)(void *)&u1Val, 1) != 0)
    {
      Printf("CustomSetAPPCountry Fail!!\n");	
    }
}
#endif

#ifdef CC_MTK_LOADER
/**************************************************************************/
//         Customized PWM Driver
/**************************************************************************/
// u1Channel : 0 is I-Duty. 1 is for I-Peak.
#define PWM_I_DUTY 0
#define PWM_I_PEAK 1
#define PWM_RETURN_ERROR 0xFFFF

// Private function : To get PWM Addr.
UINT32 LDR_PwmGetAddr(UINT8 u1Channel)
{
	UINT32 u4Reg = 0;
	switch(u1Channel)
	{
		case PWM_I_DUTY:
			u4Reg = REG_RW_PWM0;
			break;

		case PWM_I_PEAK:
			u4Reg = REG_RW_PWM1;
			break;

		default:
			//LOG(0, "PWM NOT support (%d) Channel", u1Channel);
			Printf("PWM NOT support (%d) Channel", u1Channel);
	}	

	return u4Reg;
}

// Frequency = 216MHz / (timeBase + 1) / (1024 + 1) // DutyAll is fix to 1024.
// timeBase is (4 ~ 4400)
// Func: set TimeBase, it is another way of saying PreScaler, it is PWM P
void LDR_SetBLPwmFrequency(UINT8 u1Channel, UINT32 u4TimeBase)
{
	UINT32 u4Reg;

	//Check timeBase
	if(u4TimeBase >= 4 && u4TimeBase <= 4400)
	{
		u4Reg = LDR_PwmGetAddr(u1Channel);

		if(u4Reg != 0)
		{
			IO_WRITE32MSK(HPDS_BASE, u4Reg+4, u4TimeBase, 0xFFFFF);
		}
	}
	else
	{
		//LOG(0, "PWM timeBase(%d) is too large", u4TimeBase);
		Printf("PWM timeBase(%d) is too large", u4TimeBase);
	}
}

// Duty All is RSN, so Duty cycle is Duty On, is PWM H.
// Paramters :
// u2Ratio : Duty cycle (0 ~ 1024 : 1-100%)
void LDR_SetBLPwmDuty(UINT8 u1Channel, UINT16 u2Ratio)
{
	UINT32 u4Value;
	UINT32 u4Reg;

	u4Reg = LDR_PwmGetAddr(u1Channel);

	if(u4Reg != 0)
	{
		if(u2Ratio <= 1024)
		{
		    u4Value = ((UINT32)(u2Ratio & 0xFFF) << 4) ;
			IO_WRITE32MSK(HPDS_BASE, u4Reg, u4Value, 0xFFF0);
		}
		else// means error
		{
			//LOG(0, "PWM Ratio(%d) is too large", u2Ratio);
			Printf("PWM Ratio(%d) is too large", u2Ratio);
		}
	}
}

/**************************************************************************/
//         Customized scan PWM Driver
/**************************************************************************/
typedef struct{
    UINT16 u2PulseNumber;	
    UINT16 u2Ratio;
    UINT32 u4CommonPhase;	
    INT32  wPhase;
    UINT8  u1Channel;
    UINT8  u1Mode;	
}LDR_BACKLIGHT_INFO_T;

void LDR_SetBLScanPwmNumberOfPulses(LDR_BACKLIGHT_INFO_T *pBackLight)
{
	UINT16 u2Devider = 1;
	UINT8  u1Src;
	UINT32 u4Start=0;
	UINT32 u4High=0;
	UINT32 u4Low=0;
	
	u1Src = pBackLight->u1Channel;
	u2Devider = pBackLight->u2PulseNumber;

	u4Start = u4Start/u2Devider;
	u4High = u4High/u2Devider;
	u4Low = u4Low/u2Devider;

	if(!fgIsScanPWMSetDataLatch())
	{
		vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA_OFF,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Aligned Vsync
	}
		
	// Invert PWM dimming signal
    if(DRVCUST_PanelGet(eFlagBacklightDimmingInvert))
    {
		vDrvSetScanPWMPolarity(DRVCUST_PanelGet(ePanelBacklightPwmPort), 1);
    }
	
	vDrvSetScanPWM(u1Src,u4Start,u4High,u4Low);

	vDrvScanPWMDataFire();
}

void LDR_SetBLScanPwmPhase(LDR_BACKLIGHT_INFO_T *pBackLight)
{
	UINT16 u2HTotal, u2VTotal;
	UINT8  u1Src;
	UINT16 u2Duty;
	UINT32 u4Start=0;
	UINT32 u4High=0;
	UINT32 u4Low=0;
	UINT32 u4MPhase=0;
	INT32  wSPhase=0;

	u2HTotal = wDrvGetOutputHTotal();
    u2VTotal = wDrvGetOutputVTotal();
	
	u1Src = pBackLight->u1Channel;
	u2Duty = pBackLight->u2Ratio;
	if(u2Duty >= 1024)
    {
        u2Duty = 1024;
    }
	u4MPhase = pBackLight->u4CommonPhase/u2HTotal*u2VTotal;
	u4MPhase = u4MPhase * 1000;
	wSPhase = pBackLight->wPhase;
	u4Start = ((1024-u2Duty)/1024)-(wSPhase/u2HTotal*u2VTotal);
	u4Start = u4Start * 1000;
	u4High = (u2Duty / 1024) * 1000;
	u4Low = 1000 - u4High - u4Start;

	if(!fgIsScanPWMSetDataLatch())
	{
		vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA_OFF,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Aligned Vsync
	}
		
	// Invert PWM dimming signal
    if(DRVCUST_PanelGet(eFlagBacklightDimmingInvert))
    {
		vDrvSetScanPWMPolarity(DRVCUST_PanelGet(ePanelBacklightPwmPort), 1);
    }

	vDrvSetScanPWMStartPhase(u1Src,u4MPhase);
	vDrvSetScanPWM(u1Src,u4Start,u4High,u4Low);

	vDrvScanPWMDataFire();
	
}

/**************************************************************************/
//         Customized PWM Driver ---- common part
/**************************************************************************/
void LDR_SetBLPwmMode(UINT8 channel, UINT8 mode)  // mode(0:normal; 1: high fix; 2: low fix)
{
	vDrvSetScanFixHighLow(channel,mode);
	vDrvScanPWMDataFire();
}

void CustomPowerOnSequence(void)
{
    #ifndef SUPPORT_PANEL_SCAN_PWM
	UINT32 _u4BrightLevel = 0x80; // 0:dark --> 255:bright
    #else
	UINT32 _u4BrightLevel = 50;   // 50 means 50%; 0%:dark --> 100%:bright
    #endif
	#ifdef SONY_LOADER_POWER_SEQ
	LDR_BACKLIGHT_INFO_T rBacklight;
	#endif

	#ifdef CC_COMBINE_4K_2K
    	UINT32 u4SonyModelType;
    #endif

    #if defined(CC_SUPPORT_BL_DLYTIME_CUT)&&defined(CC_MTK_LOADER)
		UINT32 u4DeltaTime;
		UINT32 u4BLOnDelay;
		UINT32 gpio, value, temp;
    #endif /* end of CC_SUPPORT_BL_DLYTIME_CUT */
    UINT8 i=0,j=0; 
    INT32 i4Ret = 0;
    UINT8 u1ValTmp = 0;
	UINT8 ui1_data[4];
	UINT32 u4Value=0;
	
	#ifndef SONY_LOADER_POWER_SEQ
	vDrvLVDSOff();
    x_thread_delay(10);
    vDrvTurnOnPanel();
    x_thread_delay((10 * PANEL_GetLvdsOnDalay()) + 1);
    switch(PANEL_GetDisplayType())	
    {
        default:
        case PANEL_DISPLAY_TYPE_MAX:
        // error display type, defaultly do nothing
            break;            
		case PANEL_DISPLAY_TYPE_LVDS:
            vDrvLVDSOff();
    vDrvLVDSConfig();
    vDrvLVDSOn();
            break;
        case PANEL_DISPLAY_TYPE_MLVDS:
            vDrvLVDSConfig();
            u1DrvTconInit();
            break;
		#ifdef CC_MT5399
        case PANEL_DISPLAY_TYPE_VB1:
            vDrvLVDSOff();
            vDrvLVDSConfig();
            vVByOneInit();				
            vDrvVByOneOn();
            break;
		#endif
    }
	#else
	
    Printf("SONY Custom PowerOn Sequence !!\n");
	// normal power sequence, lvds power -> lvds signal -> backlight power
	vDrvTurnOnPanel();

	//x_thread_delay((10 * PANEL_GetLvdsOnDalay()) + 1);
	x_thread_delay(20); //SONY fix delay 10~25 ms
	#ifdef CC_SAKURA_CVT_SUPPORT
	    u4Value=PDWNC_ReadServoADCChannelValue(3);
		while(u4Value<=358)
		{
		    u4Value=PDWNC_ReadServoADCChannelValue(3);
			Printf("Panel power too low !!\n");
		}
	#endif	
	
   //FRC check ,only for 2K FRC model
#ifdef CC_COMBINE_4K_2K
		if (DRVCUST_InitQuery(eGet2KModelSupport, &u4SonyModelType) == 0)
		{
			if((u4SonyModelType&0x01)==0x00) //need to modify for 2K with FRC
			{
			    #ifdef CC_SAKURA_CVT_SUPPORT
				//FRC init  ,need 400ms
				u1ValTmp=0x31;
				i4Ret = SIF_X_Write(EXT_FRC_I2C_BUS,0x43,EXT_FRC_I2C_ADDRESS,1,0xFE,&u1ValTmp,1);
				if (i4Ret == 0)
   				{
	   				Printf("FRC init first command fail \n");
   				}
				u1ValTmp=0x72;
				i4Ret = SIF_X_Write(EXT_FRC_I2C_BUS,0x43,EXT_FRC_I2C_ADDRESS,1,0xFF,&u1ValTmp,1);
                if (i4Ret == 0)
   				{
	   				Printf("FRC init second command fail \n");
   				}
                x_thread_delay(380);//380+20=400ms
                
                //FRC init status address setting ,10 times x10ms
				ui1_data[0]=0x5c;
				ui1_data[1]=0x01;
				ui1_data[2]=0x0a;
				ui1_data[3]=0x9f;
				i4Ret = SIF_X_Write(EXT_FRC_I2C_BUS,0x43,EXT_FRC_I2C_ADDRESS,1,0x08,ui1_data,4);
				if (i4Ret == 0)
   				{
	   				Printf("FRC init status check address fail \n");
   				}
				//Check FRC init status ,10 times x10ms
				for(i=0;i<10;i++)
				{
				    x_thread_delay(10);
					i4Ret = SIF_X_Read(EXT_FRC_I2C_BUS,0x43,EXT_FRC_I2C_ADDRESS,1,0x0c,ui1_data,4);
					if (i4Ret == 0)
	   				{
		   				Printf("Read FRC init status fail \n");
	   				}
					if((ui1_data[0]==0x01)&&(ui1_data[1]==0x00)&&(ui1_data[2]==0x00)&&(ui1_data[3]==0x00))
					{
						Printf("Read FRC init successfully \n");
						break;
					}
					else
					{
						if(i==9)
						{
						    for(j=0;j<=3;j++)
						    {
						    	Printf("Read FRC init status register X[%d]=%d \n",j,ui1_data[j]);
						    }
							Printf("Read FRC init fail \n");
							
						}
					}
				}
				//Get FRC verison
				ui1_data[0]=0x58;
				ui1_data[1]=0x01;
				ui1_data[2]=0x0a;
				ui1_data[3]=0x9f;
				i4Ret = SIF_X_Write(EXT_FRC_I2C_BUS,0x43,EXT_FRC_I2C_ADDRESS,1,0x08,ui1_data,4);
				i4Ret = SIF_X_Read(EXT_FRC_I2C_BUS,0x43,EXT_FRC_I2C_ADDRESS,1,0x0c,ui1_data,4);
				if (i4Ret == 0)
   				{
	   				Printf("FRC get FW verison fail \n");
   				}
				Printf("FRC FW version =%x%x%x%x \n",ui1_data[2],ui1_data[2],ui1_data[1],ui1_data[0]);
                #endif
			}
		}
#endif

#ifdef DRV_SUPPORT_EXTMJC

	if(IS_COMPANION_CHIP_ON())
	{
		UINT8 u1MsgErr;
		d_custom_extmjc_set_fastmute(TRUE, &u1MsgErr);
		//drv_extmjc_set_lvds_on_delay(PANEL_GetLvdsOnDalay());
		drv_extmjc_set_lvds_enable(SV_ON, &u1MsgErr);
		d_custom_extmjc_set_fastmute_timeout(0x20,&u1MsgErr);
	}

#endif
	vDrvTurnOnPDPVs();

	switch(PANEL_GetDisplayType())
	{
		default:
		case PANEL_DISPLAY_TYPE_MAX:
			// error display type, defaultly do nothing
			break;

		case PANEL_DISPLAY_TYPE_LVDS:
			vDrvLVDSOff();
			vDrvLVDSConfig();
			vDrvLVDSOn();
			break;

		case PANEL_DISPLAY_TYPE_MLVDS:
			vDrvLVDSConfig();
			vApiTCONMINIPower(VIDEO_TCON_MINISIGNAL,TRUE);
			vApiTCONMINIPower(VIDEO_TCON_MINI_ALL,TRUE);
			break;
#if defined(CC_MT5890)
		case PANEL_DISPLAY_TYPE_VB1:
			vDrvLVDSOff();
			vDrvLVDSConfig();
    #if defined( CC_SUPPORT_4K2K)||defined(CC_SUPPORT_HDMI_4K2K30)
			if((GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_60_VB1)||(GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_VB1) || (GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_60_VB1_TEST))
			{
				x_thread_delay(1200);
			}
	#endif
			vVByOneInit();
    #if defined( CC_SUPPORT_4K2K)||defined(CC_SUPPORT_HDMI_4K2K30)
			if((GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_60_VB1)||(GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_VB1) || (GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_60_VB1_TEST))
			{
				vDrvVB1LockCtrl(SV_ON,1);
			}
	#endif
			vDrvVByOneOn();
			break;
#endif
		case PANEL_DISPLAY_TYPE_EPI:
//				  u1DrvTconInit();
#ifndef CC_USE_DDI
			vApiTCONPower(VIDEO_TCON_GST, TRUE);
			vApiTCONPower(VIDEO_TCON_MCLK, TRUE);
			vApiTCONPower(VIDEO_TCON_GCLK, TRUE);
			vApiTCONPower(VIDEO_TCON_EO, TRUE);
			vApiTCONPower(VIDEO_TCON_VCOM_DYN, TRUE);
			vApiTCONPower(VIDEO_TCON_EPISIGNAL, TRUE);
			vApiTCONPower(VIDEO_TCON_ALL, TRUE);
#endif
			break;
	}

#ifdef DRV_SUPPORT_EXTMJC

	if(IS_COMPANION_CHIP_ON())
	{
		UINT8 u1MsgErr;
		drv_extmjc_set_RX_reset(&u1MsgErr);
	}

#endif
	// set pwm before enable backlight power supply, so that backlight would be expected pwm level once we supply the power
#ifdef SUPPORT_PANEL_SCAN_PWM
	_u4BrightLevel = PANEL_GetScanPWMDuty();
#endif
	//LOG(3, "_|-|_ vApiPanelPowerSequence Bright:%d%\n", _u4BrightLevel);
	BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), _u4BrightLevel);
#if defined(CC_SUPPORT_BL_DLYTIME_CUT)&&defined(CC_MTK_LOADER)
	u4BLOnDelay = 10 * PANEL_GetBacklightOnDelay() * 1000;
	u4DeltaTime = _BIM_GetCurTime() + u4BLOnDelay;
	Printf("[LVDS][backlight] DeltaTime=%d us, BLOnDelay=%d us\n", u4DeltaTime, u4BLOnDelay);
	BIM_WRITE32((REG_RW_GPRB0 + (4 << 2)), u4DeltaTime);
	if(DRVCUST_PanelQuery(eBackLightGpio, &gpio) == 0 &&
		DRVCUST_PanelQuery(eBackLightOnVal, &value) == 0)
	{
		temp = ((UINT16)value << 16)|(UINT16)gpio;
		Printf("[LVDS][backlight] GPIO:%d, value=%d, temp=0x%x\n", gpio, value, temp);
		BIM_WRITE32((REG_RW_GPRB0 + (5 << 2)), temp);
	}
#else        
#ifndef __MODEL_slt__ // save booting time
	//x_thread_delay(10 * PANEL_GetBacklightOnDelay());
#endif

#if defined( CC_SUPPORT_4K2K)||defined(CC_SUPPORT_HDMI_4K2K30)
	if((GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_60_VB1) || (GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_60_VB1_TEST))
	{	
    #ifndef CC_MTK_LOADER
		Printf("[4K2K][VB1]------send FHD_60 CMD #2\n");
		vSend4k2kFrcPanelTypeCmd(PANEL_4K2K_FHD60);
    #else
		Printf("[4K2K][VB1]------send FHD_60 CMD #1\n");
		com4k2kSendCmd(_au1cmiMstFHD60VB1Buf, sizeof(_au1cmiMstFHD60VB1Buf));
    #endif
		vDrvVB1LockCtrl(SV_OFF,0);
	}
	else if (GetCurrentPanelIndex() == PANEL_CMO_V580DK1_LS1_INX_VB1)
	{
	#ifndef CC_MTK_LOADER
		Printf("[4K2K][VB1]------send FHD_120 CMD #2\n");
		vSend4k2kFrcPanelTypeCmd(PANEL_4K2K_FHD120);
    #else
		Printf("[4K2K][VB1]------send FHD_120 CMD #1\n");
		com4k2kSendCmd(_au1cmiMstFHD120VB1Buf, sizeof(_au1cmiMstFHD120VB1Buf));
	#endif

		vDrvVB1LockCtrl(SV_OFF,0);
	}
#endif

    x_thread_delay(400); //SONY fix delay 800ms (400+400=800ms)
   
	// turn on backlight GPIO, supply backlight power
	vDrvTurnOnBacklt();
#endif /* end of CC_SUPPORT_BL_DLYTIME_CUT */


#ifndef CC_MTK_LOADER
	DRVCUST_UpdateBacklightState(BL_STATE_POWERON, _u4BrightLevel);
#endif

	if(DRVCUST_PanelGet(eFlagBacklightDimmingLock))
	{
		vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_ON);
	}
	else
	{
		vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_OFF);
	}
	
	Printf("vApiPanelPowerSequence= TRUE (END)\n"); 	
	
	#endif
	#ifndef SONY_LOADER_POWER_SEQ
    #ifdef SUPPORT_PANEL_SCAN_PWM
    _u4BrightLevel = PANEL_GetScanPWMDuty();
    #endif
	BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), _u4BrightLevel);
	x_thread_delay(10 * PANEL_GetBacklightOnDelay());
	vDrvTurnOnBacklt();
	if (DRVCUST_PanelGet(eFlagBacklightDimmingLock))
	{
        vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_ON);
	}
    else
    {
        vDrvSetLock(DRVCUST_PanelGet(ePanelBacklightPwmPort), SV_OFF);
    }
	#else
#if 0 //smith mark
	LDR_SetBLPwmMode(0,0);
	LDR_SetBLPwmMode(1,0);
	LDR_SetBLPwmMode(2,0);
	LDR_SetBLPwmMode(3,0);
	LDR_SetBLPwmMode(4,0);
	LDR_SetBLPwmMode(5,0);
	LDR_SetBLPwmFrequency(0,4);
	LDR_SetBLPwmDuty(0,512);
	LDR_SetBLPwmFrequency(1,4);
	LDR_SetBLPwmDuty(1,512);
	rBacklight.u1Channel = 2;
	rBacklight.u1Mode = 0;
	rBacklight.u2PulseNumber = 6;
	rBacklight.u2Ratio = 973;
	rBacklight.wPhase = 100;
	rBacklight.u4CommonPhase = 0;
	LDR_SetBLScanPwmNumberOfPulses(&rBacklight);
	LDR_SetBLScanPwmPhase(&rBacklight);
	rBacklight.u1Channel = 3;
	LDR_SetBLScanPwmNumberOfPulses(&rBacklight);
	LDR_SetBLScanPwmPhase(&rBacklight);
	rBacklight.u1Channel = 4;
	LDR_SetBLScanPwmNumberOfPulses(&rBacklight);
	LDR_SetBLScanPwmPhase(&rBacklight);
	rBacklight.u1Channel = 5;
	LDR_SetBLScanPwmNumberOfPulses(&rBacklight);
	LDR_SetBLScanPwmPhase(&rBacklight);
#endif
	#endif
	//CustomStartDimmerOutput();	
}

void CustomPowerOffSequence(void)
{
    // backlight power -> lvds sugnal -> lvds power
    vDrvTurnOffBacklt(); // Backlight off
    x_thread_delay(10 * PANEL_GetBacklightOffDelay());

#ifdef DRV_SUPPORT_EXTMJC
        if(IS_COMPANION_CHIP_ON())
        {
            UINT8 u1MsgErr;
            drv_extmjc_set_lvds_enable(SV_OFF, &u1MsgErr);
        }
#endif	

    switch(PANEL_GetDisplayType())	
    {
            default:
            case PANEL_DISPLAY_TYPE_MAX:
                // error display type, defaultly do nothing
                break;

            case PANEL_DISPLAY_TYPE_LVDS:
                vDrvLVDSOff();
                break;

            case PANEL_DISPLAY_TYPE_MLVDS:
				vApiTCONMINIPower(VIDEO_TCON_MINISIGNAL,FALSE);
 				vApiTCONMINIPower(VIDEO_TCON_MINI_ALL,FALSE);
                break;
#if defined(CC_MT5890)
            case PANEL_DISPLAY_TYPE_VB1:
				vVByOneAnalogOffSetting();
                break;

            case PANEL_DISPLAY_TYPE_EPI:
				#ifndef CC_USE_DDI
                   vApiTCONPower(VIDEO_TCON_GST, FALSE);
                   vApiTCONPower(VIDEO_TCON_MCLK, FALSE);
                   vApiTCONPower(VIDEO_TCON_GCLK, FALSE);
                   vApiTCONPower(VIDEO_TCON_EO, FALSE);
                   vApiTCONPower(VIDEO_TCON_VCOM_DYN, FALSE);
                   vApiTCONPower(VIDEO_TCON_EPISIGNAL, FALSE);
                   vApiTCONPower(VIDEO_TCON_ALL, FALSE);
                #endif
                break;
#endif
        }

	vDrvTurnOffPDPVs();
	x_thread_delay(10 * PANEL_GetLvdsOffDalay());
	vDrvTurnOffPanel(); // Panel off
}

#endif

