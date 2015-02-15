
//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_timer.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_gpio.h"
#include "x_pdwnc.h"
#include "x_util.h"
#include "x_pinmux.h"

#include "ir_if.h"
#include "panel.h"
#include "eeprom_if.h"
#include "PowerSeq.h"
#include "CustomCtrl.h"
#include "c_model.h"
#include PDWNC_EXT_IMPL
#include "hw_hdmi.h"
#include "panel_cust.h"
#include "drv_display.h"
//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
void GPIO_CustomPowerOnFunc(void);
void GPIO_CustomPowerOffFunc(UINT32 u4PowerDownEn);
void GPIO_CustomLedCtrlFunc(LED_STATUS_T eLedStatus);
UINT32 CustomPCBAQueryFunc(VOID);
UINT32 CustomPanelQueyFunc(VOID);
BOOL ICustomPCBARGBQueryFunc(VOID);
UINT32 u4PanelStringCmp(CHAR *pSrcStr, CHAR *DesStr);
void ATV_60HzReadPanelID(UINT8 *pu1PanelID);

extern BOOL PDWNC_Check_Enter_SemiStandy(void);

#ifndef CC_MTK_LOADER
extern void d_sony_2k12_cust_drv_init(void);
#endif

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
    
//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
//PANEL_DET
UINT32 u4PanelStringCmp(CHAR *pSrcStr, CHAR *DesStr)
{
    // remove pending 0x20 and 0xff
    UINT32 u4DesLen = x_strlen(DesStr);
    UINT32 u4SrcLen = x_strlen(pSrcStr);
    UINT32 idx;

    u4DesLen = (u4DesLen>16)? 16: u4DesLen;
    u4SrcLen = (u4SrcLen>16)? 16: u4SrcLen;

    if ((u4DesLen < 1) || (u4SrcLen < 1))
    {
        return 1;
    }

    for (idx = u4DesLen-1; idx>0; idx--)
    {
        if ((*(DesStr+idx) == 0x20) || (*(DesStr+idx) == 0xff))
        {
            *(DesStr+idx) = '\0';
        }
        else
        {
            break;
        }  
    }

    for (idx = u4SrcLen-1; idx>0; idx--)
    {
        if ((*(pSrcStr+idx) == 0x20) || (*(pSrcStr+idx) == 0xff))
        {
            *(pSrcStr+idx) = '\0';
        }
        else
        {
            break;
        }  
    }

    return x_strcmp(DesStr, pSrcStr);
}

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

void ATV_60HzReadPanelID(UINT8 *pu1PanelID)
{
	UINT32 u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
#ifndef SONY_LCCT_24
	BOOL   bMatch = FALSE;
	UINT8  i,u1Readaddr=0,u1PaneledidAddr=0;
	UINT8  au1PanelID[3][ATV_PANEL_ID_STRING_LEN];
	UINT32 u4Len;
	UINT32 u4ErrorCnt = 0;
#endif   
	
	// Read panel ID by SIF bus 
	// Device address : 0xAA, sub address : 0xE0
	SIF_Init();//it will select the pinmux,so do not need siwthc pinmux about PAD_OSCL2 AND PAD_OSDA2
	UNUSED(EEPROM_Read(PANEL_ID_PANEL_MODE, (UINT32)(VOID *)&PANEL_DET, sizeof(UINT8)));
	switch(PANEL_DET)
	{
	    case PANEL_DET_DIABLE:
		{
	      #if defined(SONY_LCCT_24)
		  WritePanelIndexToEeprom(15); 
		  Printf("Disable Panel detected,set the panel Index to 1 \n");
	      #else
		  //WritePanelIndexToEeprom(19); 
		  //Printf("Disable Panel detected set the pannel index as 19 \n");
	      #endif
		  break;
		}
		case PANEL_LESS_MODE:
		case PANEL_MODE:
		case PANEL_DISABLE_LESS_MODE:
		default:
		{
		  #if defined(SONY_LCCT_24)
		  PANEL_DET = PANEL_LESS_MODE;
		  //UINT32 u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
		  u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
		  Printf("[Power On] Write Default panel ID\n",u4DefaultPanel);
		  WritePanelIndexToEeprom(u4DefaultPanel);
		  EEPROM_Write(PANEL_ID_PANEL_MODE,(UINT32)(VOID *)&PANEL_DET,sizeof(UINT8));
		  #else
		  Printf("Panel detected \n");
		  UNUSED(EEPROM_Read(PANEL_ID_READ_ADDR, (UINT32)(VOID *)&u1Readaddr, sizeof(UINT8)));
		  if(u1Readaddr ==0xFF)
		  {
		    u1Readaddr = PANEL_EDID_READ_0X00E0;
			EEPROM_Write(PANEL_ID_READ_ADDR,(UINT32)(VOID *)&u1Readaddr,sizeof(UINT8));
		  }
		  #ifdef CC_PRJ_2K11
		    if(1)
		  #else
		    POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T37);
			if(u1Readaddr == PANEL_EDID_READ_0XE0)
		  #endif
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
				u4DefaultPanel= DEFAULT_PANEL_FHD_SELECT;
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
            #endif
			break;
		}
	}
	return;
}
//extern UINT32 u4PanelStringCmp(CHAR *pSrcStr, CHAR *DesStr);
//extern void WritePanelIndexToEeprom(UINT32 u4PanelIndex);
extern BOOL CustomPanelLoadTableFromFlash(UINT32 u4Type, void *pData);
extern UINT32 FlashPanel_GetID(void);
#define LOAD_PANEL_FROM_FLASH_BY_ID     0
#define LOAD_PANEL_FROM_FLASH_BY_NAME     1
void LedPanelIDFailFunction(void);
UINT8 _fgeLEDPanelIDFail =FALSE;

void ATV_CheckPanelID(VOID *pPanelID)
{
	  #ifndef Panel_ID_ERROR_DETECT
    UINT32 u4DefaultPanel= DEFAULT_PANEL_WXGA_SELECT;
    #endif
    
    if (!CustomPanelLoadTableFromFlash(LOAD_PANEL_FROM_FLASH_BY_NAME, pPanelID))
    {
    #ifdef Panel_ID_ERROR_DETECT
	//#ifdef CC_MTK_LOADER
	  Printf("[Power On] Panel ID is not on the panel table--> hang\n");
	  _fgeLEDPanelIDFail =TRUE;
	  LedPanelIDFailFunction(); //need to open it
	//#endif
	#else
      Printf("[Power On] Panel ID is not on the panel table\n");
      //UINT32 u4DefaultPanel= DEFAULT_PANEL_WXGA_SELECT;
      u4DefaultPanel= DEFAULT_PANEL_WXGA_SELECT;        
      Printf("[Power On] Write Default panel ID\n",u4DefaultPanel);
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
#endif /*CC_MTK_LOADER*/
//#endif
//PANEL_DET

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
static UINT32 _read_plf_opt_model(void)
{
    UINT8 u1PlfOptModel = 0;
    UINT32 u4Buf = 0;
    EEPROM_Read(0x6CB, (UINT32)(VOID*)(&u1PlfOptModel), 1);
    u4Buf = u1PlfOptModel;
    u4Buf <<= 16;
    return u4Buf;
}
#ifndef CC_MTK_LOADER
#ifdef Panel_ID_ERROR_DETECT
void CustomPanelDetectFlow(VOID)
{
    UINT8  au1PanelID[16];
	UINT8  u1State =1;
	INT32 i4InputMode=0;
    UNUSED(CustomPanelDetectFlow);
	
    ATV_60HzReadPanelID(au1PanelID);

	//x_thread_delay(3);
	//GPIO_SetOut(11, 0);
	//GPIO_SetOut(12, 0);
	//set the GPIO as input mode
	//BSP_PinSet(PIN_OSCL1 , PINMUX_FUNCTION0);
	//BSP_PinSet(PIN_OSDA1 , PINMUX_FUNCTION0);
	GPIO_Enable(PANEL_I2C_SDA_GPIO,&i4InputMode);
	GPIO_Enable(PANEL_I2C_SCL_GPIO,&i4InputMode);
	//T33 200ms
	POWER_SEQ_DO_REAL_DELAY(E_POWER_SEQ_T33);
	vDrvTurnOffPanel();
    CustomMarkPowerSequenceStart(E_POWER_SEQ_T34);//T34_Start

    Printf("[Power On] Panel ID = %s\n", (CHAR*)au1PanelID);        
    Printf("[Power On] PANEL_ID sequence\n");
	if(PANEL_I2C_ACK)
	{
  	EEPROM_Write(PANEL_ID_STORE_COUNT,(UINT32)&u1State,1);
    ATV_CheckPanelID((VOID *)au1PanelID);
	}
}
#endif
void GPIO_CustomReadStrapPin(void)
{
#ifndef Panel_ID_ERROR_DETECT
    UINT32 u4DefaultPanel = DEFAULT_PANEL_WXGA_SELECT;
#endif
    INT32 fgSet = 0;
    UINT8 au1Index[] = {PANEL_INVALID, PANEL_INVALID, PANEL_INVALID};
    QUERY_TYPE_T arKey[] = {ePanelIndexOffset1, ePanelIndexOffset2,
                            ePanelIndexOffset3};
    UINT32 u4Size = sizeof(arKey) / sizeof(QUERY_TYPE_T);
    UINT32 u4Value, i;
        
#ifdef FORCE_FHD_PANEL
    _u4gPCBType =  DRV_CUSTOM_MODEL_FHD_PANEL ;
    _u4gPanelType = CUSTOM_GPIO_PANEL_FHD;
#else
    _u4gPCBType = DRV_CUSTOM_MODEL_WXGA_PANEL;
    _u4gPanelType = CUSTOM_GPIO_PANEL_WXGA;        
#endif    
    
    GPIO_Output(CUSTOM_GPIO_PANEL_NVM_WP, &fgSet);

    //_u4gPanelType = (UINT32)GPIO_Input(CUSTOM_GPIO_PANEL_ID_PIN_0);
    _u4gPCBType = (_u4gPanelType == CUSTOM_GPIO_PANEL_WXGA) ? 
        DRV_CUSTOM_MODEL_WXGA_PANEL : DRV_CUSTOM_MODEL_FHD_PANEL;
	
    _u4gModelType |= (_u4gFactoryType == CUSTOM_GPIO_FACTORY_WISTRON) ?
        DRV_CUSTOM_MODEL_FACTORY_WISTRON : DRV_CUSTOM_MODEL_FACTORY_SONY;
    _u4gModelType |=_u4gPCBType ;

    _u4gModelType |= _read_plf_opt_model();
    // Determin Light sensor enable/disable by Option bit
    //_u4gModelType |= (GPIO_Input(CI_DET) == 0)?DRV_CUSTOM_MODEL_NO_CI : DRV_CUSTOM_MODEL_CI;
	//_u4gModelType |= DRV_CUSTOM_MODEL_CI;
    Printf("[GPIO_CustomReadStrapPin] _u4gModelType = 0x%2x _u4gPanelType=%d CI status %d\n", _u4gModelType, _u4gPanelType,GPIO_Input(110));

    //UINT8 au1Index[] = {PANEL_INVALID, PANEL_INVALID, PANEL_INVALID};
    //QUERY_TYPE_T arKey[] = {ePanelIndexOffset1, ePanelIndexOffset2,
    //                        ePanelIndexOffset3};
    //UINT32 u4Size = sizeof(arKey) / sizeof(QUERY_TYPE_T);
    //UINT32 u4Value, i;

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
#ifdef Panel_ID_ERROR_DETECT
			  vDrvTurnOnPanel();
              CustomMarkPowerSequenceStart(E_POWER_SEQ_T33);//t33_start
			  CustomMarkPowerSequenceStart(E_POWER_SEQ_T37);//t37_start
			  #if 0
              fgSet=1;
			  GPIO_Output(PANEL_I2C_SDA_GPIO,&fgSet);
			  GPIO_Output(PANEL_I2C_SCL_GPIO,&fgSet);
			  #endif
			  CustomPanelDetectFlow();
#else

        // Write the default panel index to EEPROM
        if ((au1Index[0] == PANEL_INVALID) && (au1Index[1] == PANEL_INVALID) &&
            (au1Index[2] == PANEL_INVALID))
        {
#ifndef CC_MANUAL_PANEL_SEL        
            //UINT32 u4DefaultPanel = DEFAULT_PANEL_WXGA_SELECT;
            u4DefaultPanel = DEFAULT_PANEL_WXGA_SELECT;
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
            u4DefaultPanel = (_u4gPanelType == CUSTOM_GPIO_PANEL_FHD) ? 
            DEFAULT_PANEL_FHD_SELECT : DEFAULT_PANEL_WXGA_SELECT;
#endif/*CC_MANUAL_PANEL_SEL*/
            WritePanelIndexToEeprom(u4DefaultPanel);
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
#ifdef CC_MTK_LOADER
    UINT32 i;
    UINT32 polarity;
#ifdef WIFI_WOWLAN_SUPPORT		
    DTVCFG_T    rDtvCfg;
#endif	
    //Init for hotel mode , defualt to off
    GPIO_SetOut(DRV_CUSTOM_IR_PASS_THROUGH_GPIO, DRV_CUSTOM_IR_PASS_THROUGH_GPIO_OFF);
#if 0    
	vRegWrite1B(PD_SYS_2, 0xff);
	vRegWriteFldAlign(ANA_INTF_1, 0x1, RG_HDMI_TERM_EN);	
	vRegWriteFldAlign(ANA_INTF_2, 0x1, RG_HDMI_SW_EN);
#endif
    
    UNUSED(GPIO_CustomPowerOnFunc);
    
    //reset WIFI
	#ifdef WIFI_WOWLAN_SUPPORT	
	#ifdef WIFI_MODULE_RESET_GPIO
	if((PDWNC_ReadWakeupReason()!=PDWNC_WAKE_UP_REASON_AC_POWER) && (0==EEPDTV_GetCfg(&rDtvCfg)) && (rDtvCfg.u1Flags4 & (UINT8)DTVCFG_FLAG4_WAKEUP_ON_WLAN))
	{
	GPIO_SetOut(WIFI_MODULE_RESET_GPIO,WIFI_MODULE_RESET_GPIO_POL);
	HAL_Delay_us(WIFI_MODULE_RESET_DURATION*1000);
	GPIO_SetOut(WIFI_MODULE_RESET_GPIO,WIFI_MODULE_RESET_GPIO_POL?0:1);	
	}
	#endif
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
    _PRINT_FACTORY_MESSAGE(5,("Booting\n"));
#else /* CC_MTK_LOADER */

    GPIO_CustomReadStrapPin();

    if (PDWNC_Check_Enter_SemiStandy())
    {
        IRRC_LedBlinkConfig(FALSE);
    }
	else
	{
	   GPIO_CustomLedCtrlFunc(eLedSystemPowerOn);
	}
    CustomIRLedHandleStart();
    
#ifdef DRV_3RD_PARTY_CUSTOM_INIT    /* 3rd Pary driver init routine */
    DRV_3RD_PARTY_CUSTOM_INIT();
#endif
    d_sony_2k12_cust_drv_init(); //for RGB init

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
    
   
	  UNUSED(GPIO_CustomPowerOffFunc);
    
#ifndef CC_MTK_LOADER
    CustomIRLedHandleStop();
#endif /* CC_MTK_LOADER */

    //BOOL bOnTimer = (u4PowerDownEn & WAK_RTC) ? TRUE : FALSE;
    CustomSetTimerLED(bOnTimer);
	
#ifdef SIGNAL_X_U_RST
    //INT32 i4Value = 0;
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

