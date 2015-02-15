
//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_timer.h"
#include "x_pinmux.h"
#include "x_debug.h"
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#include "x_pdwnc.h"
#include "pdwnc_if.h"

#include "eeprom_if.h"
#include "ir_if.h"
#include "c_model.h"
#include PDWNC_EXT_IMPL
#include GPIO_INCLUDE_FILE
#include "CustomCtrl.h"
#include "PowerSeq.h"
#define LED_ISSUE_DEBUG_MSG 0
//---------------------------------------------------------------------------
// Customer function definitions
//---------------------------------------------------------------------------
#define PCBA_QUERY_FUCTION                  (CustomPCBAQueryFunc)
#define PCBA_RGB_SENSOR_QUERY_FUCTION       (CustomPCBARGBQueryFunc)
#define LOGO_NOTIFY_FUNC                    (CustomLogoNotifyFunc)

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
#ifndef CC_MTK_LOADER
static void CustomIRNfyFunc(UINT32 u4BtnState, UINT32 u4Key,
                            IRRX_RAW_DATA_T *prRawData);
static BOOL _fgStbyLedOn=FALSE;//TRUE;	
static BOOL _fgPowerLedOn=TRUE;	
static BOOL   _bIRRCLedOn = TRUE;	
static BOOL _bIRLedBlinkEnable=TRUE;					
#endif /* CC_MTK_LOADER */

static BOOL _fgFixedLedPattern=FALSE;//d130805_Haibo:To indicate if now is fixed pattern
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
#ifdef CC_MTK_LOADER
BOOL _fgLedOn[D_LED_TOTAL + 1] = {FALSE, FALSE, FALSE, FALSE, FALSE};
#else
BOOL _fgLedOn[D_LED_TOTAL + 1] = {TRUE, FALSE, FALSE, FALSE, FALSE};
#endif
BOOL _fgLedBlink[D_LED_TOTAL + 1] = {FALSE, FALSE, FALSE, FALSE, FALSE};
BOOL _fgLedAllBlink = FALSE;
BOOL _fgStandbyDiable = FALSE;
BOOL _fgIsShowLogo = FALSE;
BOOL _fgTimerOnEnable = FALSE;

#ifndef CC_MTK_LOADER
HANDLE_T _hIRLedCtrlTimer = NULL_HANDLE;
HANDLE_T _hLedCtrlMsgQue = NULL_HANDLE;
HANDLE_T _hLedCtrlHandle = NULL_HANDLE;
#endif /* CC_MTK_LOADER */

#if 1//def STELLAR_LED_SPEC
BOOL bLedMode = TRUE;
#endif/* STELLAR_LED_SPEC */
//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static BOOL CustomIsDisabeStandbyLED(void)
{    
	return TRUE;//d130703_Haibo:Always disable standby led
}
#ifdef TWO_LED_MODE
#ifndef CC_MTK_LOADER    
static void InitializeLEDState(void)
{
    static UINT8 u1Initialized = 0;
    if (!u1Initialized)
    {
        INT32 i4GPIOStatus = GPIO_Output(CUSTOM_GPIO_LED_POWER, 0);
        _fgLedOn[D_LED_POWER] = (i4GPIOStatus==CUSTOM_LED_POWER_ON_VAL ? TRUE: FALSE);
        _fgPowerLedOn=_fgLedOn[D_LED_POWER];
        i4GPIOStatus = GPIO_Output(CUSTOM_GPIO_LED_STANDBY, 0);
        _fgLedOn[D_LED_STANDBY] = (i4GPIOStatus==CUSTOM_LED_STANDBY_ON_VAL ? TRUE: FALSE);
        _fgStbyLedOn=_fgLedOn[D_LED_STANDBY];

        if (_fgLedOn[D_LED_STANDBY] && _fgLedOn[D_LED_POWER])
        {
            //this case should be on-timer
            _fgLedOn[D_LED_TIMER] = TRUE;
            _fgLedOn[D_LED_STANDBY] = FALSE;
        }
        u1Initialized = 1;

#if LED_ISSUE_DEBUG_MSG
        Printf("initialize LED status as %d %d\n",_fgLedOn[D_LED_POWER], _fgLedOn[D_LED_STANDBY]);
#endif
    }
}
#endif
void UpdateLEDGPIOAccordingToLEDOnState(UINT8 u1Update)
{
    //rule: turn on as many as possible
    // D_LED_TIMER:   orange
    // D_LED_TIMER_2: green
    INT32 i4Gpio=0, i4Val=0;
#ifdef TWO_LED_MODE
#ifndef CC_MTK_LOADER  
	  InitializeLEDState();
#endif
#endif
    if (u1Update)
    {
        //check green
        if (_fgLedOn[D_LED_POWER] || _fgLedOn[D_LED_TIMER] || _fgLedOn[D_LED_TIMER_2])
        {
            i4Val = CUSTOM_LED_POWER_ON_VAL;
#if LED_ISSUE_DEBUG_MSG
            Printf("[]g on\n");
#endif
#ifndef CC_MTK_LOADER
            _fgPowerLedOn=TRUE;
#endif	            
        }
        else
        {
            i4Val = 1 - CUSTOM_LED_POWER_ON_VAL;
#if LED_ISSUE_DEBUG_MSG
            Printf("[]g off\n");
#endif
#ifndef CC_MTK_LOADER
            _fgPowerLedOn=FALSE;
#endif
        }
        i4Gpio = CUSTOM_GPIO_LED_POWER;
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
		
#ifdef CUSTOM_GPIO_LED_POWER_SECONDARY
        i4Gpio = CUSTOM_GPIO_LED_POWER_SECONDARY;
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
#endif

        //check red
        if (_fgLedOn[D_LED_STANDBY] || _fgLedOn[D_LED_TIMER])
        {
            i4Val = CUSTOM_LED_STANDBY_ON_VAL;
#if LED_ISSUE_DEBUG_MSG
            Printf("[]r on\n");
#endif
#ifndef CC_MTK_LOADER
            _fgStbyLedOn=TRUE;
#endif			
        }
        else
        {
            i4Val = 1 - CUSTOM_LED_STANDBY_ON_VAL;
#if LED_ISSUE_DEBUG_MSG
            Printf("[]r off\n");
#endif
#ifndef CC_MTK_LOADER
            _fgStbyLedOn=FALSE;
#endif			
        }
        i4Gpio = CUSTOM_GPIO_LED_STANDBY;
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
    }
    else
    {
        //force off
#if LED_ISSUE_DEBUG_MSG
        Printf("[]force two off\n");
#endif
        i4Gpio = CUSTOM_GPIO_LED_POWER;
        i4Val = 1 - CUSTOM_LED_POWER_ON_VAL;
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
#ifdef CUSTOM_GPIO_LED_POWER_SECONDARY
        i4Gpio = CUSTOM_GPIO_LED_POWER_SECONDARY;
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
#endif

        i4Gpio = CUSTOM_GPIO_LED_STANDBY;
        i4Val = 1 - CUSTOM_LED_STANDBY_ON_VAL;
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
#ifndef CC_MTK_LOADER		
		_fgStbyLedOn=FALSE;
		_fgPowerLedOn=FALSE;
#endif		
    }
}
void CustomLEDCtrlHandle(D_LED_TYPE eLedType, UINT32 u4LedState)
{

#ifndef CC_MTK_LOADER    
     CRIT_STATE_T rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */

#if 1//def STELLAR_LED_SPEC
	static BOOL b_init = FALSE;
	DTVCFG_T rDtvCfg;
#if LED_ISSUE_DEBUG_MSG
    Printf("CustomLEDCtrlHandle %d %d\n", eLedType, u4LedState);
#endif
	if(!b_init)
	{
		EEPDTV_GetCfg(&rDtvCfg);
		if(rDtvCfg.u1Flags & DTVCFG_FLAG_POWER_LEDOFF)
		{
#if LED_ISSUE_DEBUG_MSG		
			Printf("[LED CTRL] power on with LED OFF MODE\n");
			if(_fgFixedLedPattern)
			{
			   Printf("[LED CTRL] But now is fixed pattern.LED still need to upadte\n");
			}
#endif			
			bLedMode = FALSE;
		}
	}
#endif /*STELLAR_LED_SPEC*/

    switch (eLedType)
    {
    case D_LED_TOTAL:
        if ((D_LED_GENERIC_STATE)u4LedState == D_LED_GENERIC_ON)
        {
            _fgLedOn[D_LED_POWER] = TRUE;
            _fgLedOn[D_LED_STANDBY] = TRUE;
            _fgLedOn[D_LED_TIMER] = TRUE;
            _fgLedOn[D_LED_TIMER_2] = TRUE;
            _fgLedOn[D_LED_TOTAL] = TRUE;
            
        }
        else
        {
            _fgLedOn[D_LED_POWER] = FALSE;
            _fgLedOn[D_LED_STANDBY] = FALSE;
            _fgLedOn[D_LED_TIMER] = FALSE;
            _fgLedOn[D_LED_TIMER_2] = FALSE;
            _fgLedOn[D_LED_TOTAL] = FALSE;
            
        }
        break;

    case D_LED_POWER:
        if ((D_LED_POWER_STATE)u4LedState == D_LED_POWER_ON)
        {
            _fgLedOn[eLedType] = TRUE;
        }
        else
        {
            _fgLedOn[eLedType] = FALSE;
        }
        break;

    case D_LED_STANDBY:
        if ((D_LED_STANDBY_STATE)u4LedState == D_LED_STANDBY_ON)
        {
            _fgLedOn[eLedType] = TRUE;
        }
        else
        {
            _fgLedOn[eLedType] = FALSE;
        }
        break;

    case D_LED_TIMER:
        switch (u4LedState)
        {
        case D_LED_TIMER_ORANGE:
            _fgLedOn[D_LED_TIMER_2] = FALSE;
            _fgLedOn[D_LED_TIMER] = TRUE;
            break;

        case D_LED_TIMER_GREEN :
            _fgLedOn[D_LED_TIMER_2] = TRUE;
            _fgLedOn[D_LED_TIMER] = FALSE;
            break;

        case D_LED_TIMER_BOTH :
            _fgLedOn[D_LED_TIMER_2] = TRUE;
            _fgLedOn[D_LED_TIMER] = TRUE;
            break;

        default :
            _fgLedOn[D_LED_TIMER_2] = FALSE;
            _fgLedOn[D_LED_TIMER] = FALSE;
            break;
        }
        break;
    default:
    	  break;
    }
#ifndef CC_MTK_LOADER    
    x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */

#if 1//def STELLAR_LED_SPEC
    
	UpdateLEDGPIOAccordingToLEDOnState((UINT8)(bLedMode|_fgFixedLedPattern));
#else
    UpdateLEDGPIOAccordingToLEDOnState(1);
#endif/* STELLAR_LED_SPEC */
}
#else
void CustomLEDCtrlHandle(D_LED_TYPE eLedType, UINT32 u4LedState)
{
#if LED_ISSUE_DEBUG_MSG
    Printf("CustomLEDCtrlHandle %d %d\n", eLedType, u4LedState);
#endif
    INT32 i4Gpio;
    INT32 i4Val;
    
#ifndef CC_MTK_LOADER    
    CRIT_STATE_T rCritState;
#endif /* CC_MTK_LOADER */

    switch (eLedType)
    {
    case D_LED_TOTAL:
        if ((D_LED_GENERIC_STATE)u4LedState == D_LED_GENERIC_ON)
        {
            i4Val = CUSTOM_LED_TIMER_ON_VAL;

#ifndef CC_MTK_LOADER    
            rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */

            _fgLedOn[D_LED_POWER] = TRUE;
            _fgLedOn[D_LED_STANDBY] = TRUE;
            _fgLedOn[D_LED_TIMER] = TRUE;
            _fgLedOn[D_LED_TIMER_2] = TRUE;
            _fgLedOn[D_LED_TOTAL] = TRUE;
            
#ifndef CC_MTK_LOADER    
            x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */
        }
        else
        {
            i4Val = 1 - CUSTOM_LED_TIMER_ON_VAL;

#ifndef CC_MTK_LOADER    
            rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */

            _fgLedOn[D_LED_POWER] = FALSE;
            _fgLedOn[D_LED_STANDBY] = FALSE;
            _fgLedOn[D_LED_TIMER] = FALSE;
            _fgLedOn[D_LED_TIMER_2] = FALSE;
            _fgLedOn[D_LED_TOTAL] = FALSE;
            
#ifndef CC_MTK_LOADER    
            x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */
        }

        VERIFY(GPIO_Output(CUSTOM_GPIO_LED_POWER, &i4Val)== i4Val);
        VERIFY(GPIO_Output(CUSTOM_GPIO_LED_STANDBY, &i4Val)== i4Val);
        VERIFY(GPIO_Output(CUSTOM_GPIO_LED_TIMER_GREEN, &i4Val)== i4Val);
        VERIFY(GPIO_Output(CUSTOM_GPIO_LED_TIMER_AMBER, &i4Val)== i4Val);
        break;

    case D_LED_POWER:
        i4Gpio = CUSTOM_GPIO_LED_POWER;
        if ((D_LED_POWER_STATE)u4LedState == D_LED_POWER_ON)
        {
            _fgLedOn[eLedType] = TRUE;
            i4Val = CUSTOM_LED_POWER_ON_VAL;
        }
        else
        {
            _fgLedOn[eLedType] = FALSE;
            i4Val = (1 - CUSTOM_LED_POWER_ON_VAL);
        }
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
        break;

    case D_LED_STANDBY:
        i4Gpio = CUSTOM_GPIO_LED_STANDBY;
        if ((D_LED_STANDBY_STATE)u4LedState == D_LED_STANDBY_ON)
        {
#ifndef CC_MTK_LOADER    
            rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */

            _fgLedOn[eLedType] = TRUE;

#ifndef CC_MTK_LOADER    
            x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */

            i4Val = CUSTOM_LED_STANDBY_ON_VAL;
        }
        else
        {
#ifndef CC_MTK_LOADER    
            rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */

            _fgLedOn[eLedType] = FALSE;

#ifndef CC_MTK_LOADER    
            x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */

            i4Val = (1 - CUSTOM_LED_STANDBY_ON_VAL);
        }
        VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
        break;

    case D_LED_TIMER:
        switch (u4LedState)
        {
        case D_LED_TIMER_ORANGE:
            i4Gpio = CUSTOM_GPIO_LED_TIMER_GREEN;
            i4Val = 1 - CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER_2] = FALSE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            i4Gpio = CUSTOM_GPIO_LED_TIMER_AMBER;
            i4Val = CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER] = TRUE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            break;

        case D_LED_TIMER_GREEN :
            i4Gpio = CUSTOM_GPIO_LED_TIMER_GREEN;
            i4Val = CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER_2] = TRUE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            i4Gpio = CUSTOM_GPIO_LED_TIMER_AMBER;
            i4Val = 1 - CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER] = FALSE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            break;

        case D_LED_TIMER_BOTH :
            i4Gpio = CUSTOM_GPIO_LED_TIMER_GREEN;
            i4Val = CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER_2] = TRUE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            i4Gpio = CUSTOM_GPIO_LED_TIMER_AMBER;
            i4Val = CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER] = TRUE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            break;

        default :
            i4Gpio = CUSTOM_GPIO_LED_TIMER_GREEN;
            i4Val = 1 - CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER_2] = FALSE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            i4Gpio = CUSTOM_GPIO_LED_TIMER_AMBER;
            i4Val = 1 - CUSTOM_LED_TIMER_ON_VAL;
            _fgLedOn[D_LED_TIMER] = FALSE;
            VERIFY(GPIO_Output(i4Gpio, &i4Val)== i4Val);
            break;
        }
        break;
    }
}
#endif

#ifndef CC_MTK_LOADER    
static void CustomLedCtrlMsgHandle(void *pvArg)
{
    UINT16 u2MsgIdx;
    LED_CTRL_STATE rLedCtrlMsg;
    
    while (1)
    {
        SIZE_T zMsgSize = sizeof(LED_CTRL_STATE);
        INT32  i4MsgRet;
     
        i4MsgRet = x_msg_q_receive(&u2MsgIdx, &rLedCtrlMsg, &zMsgSize,
                                   &_hLedCtrlMsgQue, 1, X_MSGQ_OPTION_WAIT);
        if (i4MsgRet == OSR_OK)
        {
            CustomLEDCtrlHandle(rLedCtrlMsg.eLedType, rLedCtrlMsg.u4LedState);
        }
    }
}
void LEDResourceInit(void)
{
    if (_hLedCtrlMsgQue == NULL_HANDLE)
    {
        VERIFY(x_msg_q_create(&_hLedCtrlMsgQue, "LedCtrlMsgQue", 
                              sizeof(LED_CTRL_STATE), 8) == OSR_OK);
        
        // Create the Led state handle handle task
        if (x_thread_create(&_hLedCtrlHandle, "LedCtrlMsgHandle",
                            0x800, DRVCUST_OptGet(eReleaseWatchdogPriority),
                            CustomLedCtrlMsgHandle, 0, NULL) != OSR_OK)
        {
            LOG(3, "\nError: Create Led state handle handle thread fail\n");
            x_msg_q_delete(_hLedCtrlMsgQue);
            _hLedCtrlMsgQue = NULL_HANDLE;
        }
    }
	
#ifdef TWO_LED_MODE
    InitializeLEDState();
#endif
}
#endif /* CC_MTK_LOADER */
void CustomLEDCtrlFunc(D_LED_TYPE eLedType, UINT32 u4LedState)
{
	
    LED_CTRL_STATE rLedCtrlMsg;
#ifdef CC_MTK_LOADER    
    CustomLEDCtrlHandle(eLedType, u4LedState);
#else

#if LED_ISSUE_DEBUG_MSG
    Printf("CustomLEDCtrlFunc %d %d\n", eLedType, u4LedState);
#endif

    LEDResourceInit();
    if (_hLedCtrlMsgQue != NULL_HANDLE)
    {
        rLedCtrlMsg.eLedType = eLedType;
        rLedCtrlMsg.u4LedState = u4LedState;
        UNUSED(x_msg_q_send(_hLedCtrlMsgQue, &rLedCtrlMsg,
                            sizeof(LED_CTRL_STATE), 1));
    }

#endif /* CC_MTK_LOADER */
}

void CustomDisableStandbyLEDCtrl(UINT8 u1Disable)
{
  //  UINT8 u1ShutdownState = 0;
    
    // Disable standby LED control
    _fgStandbyDiable = (BOOL)(u1Disable);
}

void CustomLEDBlinkCtrl(D_LED_TYPE eLedType)
{
    UINT32 u4LedState;

    // Control the LED blinking,
    // LED On : 1000 ms & LED off : 1000ms
    if (eLedType == D_LED_TIMER)
    {
        u4LedState = (_fgLedOn[D_LED_TIMER]) ? 
                     (UINT32)D_LED_TIMER_OFF : (UINT32)D_LED_TIMER_ORANGE;
    }
    else
    {
        u4LedState = (_fgLedOn[eLedType]) ? 
                     (UINT32)D_LED_GENERIC_OFF : (UINT32)D_LED_GENERIC_ON;
    }
    
    CustomLEDCtrlFunc(eLedType, u4LedState);
}

#ifndef CC_MTK_LOADER    
void CustomLogoNotifyFunc(BOOL bIsShowLogo)
{
    UNUSED(CustomLogoNotifyFunc);
    _fgIsShowLogo = bIsShowLogo;
    
#ifdef CC_TIME_MEASUREMENT
    if (bIsShowLogo)
    {
        x_os_drv_set_timestamp("Before logo shows");
    }
    else
    {
        x_os_drv_set_timestamp("After logo shows");
    }
#endif /* CC_TIME_MEASUREMENT */
}
void CustomIRNfyFunc(UINT32 u4BtnState, UINT32 u4Key, IRRX_RAW_DATA_T *prRawData)
{
    BOOL fEnterPowerOff = FALSE;

    UNUSED(u4Key);
    UNUSED(prRawData);

#ifdef CUSTOM_POWER_STATE_CTRL_ENABLE
    fEnterPowerOff = (CustomQueryPowerState() == STATE_POWER_ON) ? FALSE : TRUE;
#else
    CRIT_STATE_T rCritState = x_crit_start();
    fEnterPowerOff = _fgLedOn[D_LED_STANDBY];
    x_crit_end(rCritState);
#endif /* CUSTOM_POWER_STATE_CTRL_ENABLE */

    IRRC_LedBlinkConfig(!fEnterPowerOff);
}
void Custom_IRLedBlinkEnable(BOOL bEnable)
{
    Printf("%s %d\n",__FUNCTION__,bEnable);
    _bIRLedBlinkEnable=bEnable;
}
static void Custom_IRLEDCtrlFunc(HANDLE_T hHandle, VOID* pTag)
{
    INT32 i4Val,i4Val2;
    
    UNUSED(hHandle);
    if (!_bIRLedBlinkEnable)
    {
        _bIRRCLedOn = FALSE;    // Must turn on the LED
    }
	// On/Off the LED if IR is pressed
    _bIRRCLedOn = (_bIRRCLedOn) ? FALSE : TRUE;

	if(_fgPowerLedOn)//d130926_Haibo:Only blink power led when it is on
	{
    i4Val = (_bIRRCLedOn) ? CUSTOM_LED_POWER_ON_VAL : (1 - CUSTOM_LED_POWER_ON_VAL);
    VERIFY(GPIO_Output(CUSTOM_GPIO_LED_POWER, &i4Val)== i4Val);	
	}
	if(_fgStbyLedOn && (0xffffffff!=CUSTOM_GPIO_LED_STANDBY))
	{
	i4Val2=(_bIRRCLedOn) ? CUSTOM_LED_STANDBY_ON_VAL : (1 - CUSTOM_LED_STANDBY_ON_VAL);
	VERIFY(GPIO_Output(CUSTOM_GPIO_LED_STANDBY, &i4Val2)== i4Val2);	
	}
}
static void Custom_IRLedBlinkFunc(UINT32 u4BtnState, UINT32 u4Key,
                             IRRX_RAW_DATA_T *prRawData)
{
    static UINT32 u4LastCond = IRRX_COND_BTN_UP;

    UNUSED(u4Key);
    UNUSED(prRawData);

    if (!_bIRLedBlinkEnable)
    {
        return;
    }
    //Stop blinking IR Led if system go to standby mode...
    if ((u4Key & (~KEY_SRC_FRONT_PANEL)) == BTN_POWER)
    {
         return;
    }
    if (u4BtnState == IRRX_COND_BTN_DOWN)
    {
        if (u4LastCond != u4BtnState)
        {
            if (((HANDLE_T *)_hIRLedCtrlTimer != NULL) && (IRRX_LED_BLINK_DURATION > 0))
            {
                 UNUSED(x_timer_start(_hIRLedCtrlTimer, IRRX_LED_BLINK_DURATION,
                                     X_TIMER_FLAG_REPEAT, Custom_IRLEDCtrlFunc,
                                     NULL));
            }
            else
            {
                INT32 i4Val = 1 - CUSTOM_LED_POWER_ON_VAL;
                UNUSED(GPIO_Output(CUSTOM_GPIO_LED_POWER, &i4Val));
                _bIRRCLedOn = FALSE;
            }
        }
    }
    else
    {
        if ((HANDLE_T *)_hIRLedCtrlTimer != NULL)
        {
            x_timer_stop(_hIRLedCtrlTimer);
        }
        if(_fgPowerLedOn)
		{
        UNUSED(GPIO_SetOut((INT32)CUSTOM_GPIO_LED_POWER, 
                          CUSTOM_LED_POWER_ON_VAL));
		}				   
		//Printf("Standby LED Should %s\n",_fgStbyLedOn?"ON":"OFF");			
		if(_fgStbyLedOn && (0xffffffff!=CUSTOM_GPIO_LED_STANDBY))//d130705_Haibo:restore the opt led state if ever blink d130709_Haibo:To fix sleep timer enable/disable red led can not off
        {
          UNUSED(GPIO_SetOut((INT32)CUSTOM_GPIO_LED_STANDBY, 
                           CUSTOM_LED_STANDBY_ON_VAL));		
        }
        _bIRRCLedOn = TRUE;
    }

    u4LastCond = u4BtnState;
}

void CustomIRLedHandleStart(void)
{
    static BOOL bInit = FALSE;
    
    if (bInit)
    {
        return;
    }

    if (x_timer_create(&_hIRLedCtrlTimer) != OSR_OK)
    {
        LOG(5, "[GPIO_CustomPowerOnFunc] Create IR LED Blink timer fail !\n");
        return;
    }

    UNUSED(CustomIRNfyFunc);
//    UNUSED(IRRC_RegCallbackFunc(CustomIRNfyFunc));
    UNUSED(IRRC_RegCallbackFunc(Custom_IRLedBlinkFunc));
	bInit=TRUE;
}

void CustomIRLedHandleStop(void)
{
    if (_hIRLedCtrlTimer != NULL_HANDLE)
    {
        x_timer_stop(_hIRLedCtrlTimer);
        x_timer_delete(_hIRLedCtrlTimer);
        _hIRLedCtrlTimer = NULL_HANDLE;
    }
}

void CustomIRLedTimerStop(void)
{
    if (_hIRLedCtrlTimer != NULL_HANDLE)
    {
        x_timer_stop(_hIRLedCtrlTimer);
    }
}
#endif /* CC_MTK_LOADER */
#ifdef CC_MTK_LOADER    
static void _ldr_update_on_timer_state(void)
{
    BOOL bOnTimer = FALSE;
    LDR_DATA_T   *prLdrData = 0;
    bOnTimer = IO32ReadFldAlign(PDWNC_WAKEN, FLD_RTC_WAKEN) ? TRUE : FALSE;

    if (bOnTimer)
    {
        prLdrData = LDR_GetLdrData();           
        bOnTimer = prLdrData->rDtvCfg.u1Flags3 & DTVCFG_FLAG3_AMBER_LED_ON;
    }
    CustomSetTimerLED(bOnTimer);
}
#else
static void _update_on_timer_state(BOOL bPowerOn)
{
    BOOL bOnTimer = FALSE;
    //check flag in DtvCfg first
    DTVCFG_T rDtvCfg;
    if (bPowerOn)
    {
        bOnTimer = IO32ReadFldAlign(PDWNC_WAKEN, FLD_RTC_WAKEN) ? TRUE : FALSE;
        if (bOnTimer)
        {
            UNUSED(EEPDTV_GetCfg(&rDtvCfg));
            bOnTimer = rDtvCfg.u1Flags3 & DTVCFG_FLAG3_AMBER_LED_ON;
        }
    }
    else
    {
        UNUSED(EEPDTV_GetCfg(&rDtvCfg));
        bOnTimer = rDtvCfg.u1Flags3 & DTVCFG_FLAG3_AMBER_LED_ON;
    }

    CustomSetTimerLED(bOnTimer);
}
#endif

void GPIO_CustomLedCtrlFunc(LED_STATUS_T eLedStatus)
{
    UINT32 u4LedCtrl = D_LED_GENERIC_OFF;
    
    BOOL fgLedStandbyDiable = CustomIsDisabeStandbyLED();
#if LED_ISSUE_DEBUG_MSG
    Printf("GPIO_CustomLedCtrlFunc %d\n", eLedStatus);
#endif
	switch(eLedStatus)
	{
	case eLedPanelIDFail:
	case eLedUpgradeBlink:
	case eLedUpgradeFail:
	case eLedUpgradeFinish:
	{
	   	_fgFixedLedPattern=TRUE;
	}
	break;
	default:
	   	_fgFixedLedPattern=FALSE;
        break;	
	}
    switch(eLedStatus)
    {
#ifdef CC_MTK_LOADER        
    case eLedLoaderPowerOn:
        Printf("loader, eLedLoaderPowerOn\n");
        //Power LED ON
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
        
        // VGA Standby LED OFF
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_OFF);
        
        // TIMER LED ON/OFF
        _ldr_update_on_timer_state();
        u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
        #ifdef CC_AC_DETECT_ENABLE
        if(IS_AC_DETECT_STANDBY())
        {
            u4LedCtrl = D_LED_TIMER_OFF;
            Printf("CUSTOM_SHUTDOWN_AC_DET! (force disable timer led)\n");
        }
        #endif 
        CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
        break;

    case eLedLoaderPowerOff:
        Printf("eLedLoaderPowerOff\n");
        // Power LED OFF
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        
        // TIMER LED ON/OFF
        _ldr_update_on_timer_state();
        u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
        CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
        
        // Standby LED ON
        u4LedCtrl = fgLedStandbyDiable ? D_LED_STANDBY_OFF : D_LED_STANDBY_ON;
        CustomLEDCtrlFunc(D_LED_STANDBY, u4LedCtrl);
        break;
#else
    case eLedLoaderPowerOn:
        Printf("arm, eLedLoaderPowerOn\n");
        //Power LED ON
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);

        // VGA Standby LED OFF
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_OFF);
        
        // TIMER LED ON/OFF
//        _ldr_update_on_timer_state();
//        u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
//        CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
        break;
    case eLedSystemPowerOn:
        Printf("eLedSystemPowerOn\n");
	    // Power LED ON
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
        // VGA Standby LED OFF
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_OFF);


        // TIMER LED ON/OFF
        _update_on_timer_state(TRUE);
        u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
        CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
        break;

    case eLedSystemPowerOff:
        Printf("eLedSystemPowerOff\n");
        // Power LED OFF
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        // TIMER LED ON/OFF
#ifdef CUSTOM_POWER_STATE_CTRL_ENABLE
        if (CustomQueryPowerState() == STATE_POWER_OFF)
        {
            CustomLEDCtrlFunc(D_LED_TIMER, D_LED_TIMER_OFF);
        }
        else
        {
            _update_on_timer_state(FALSE);
            u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
			CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
        }
#else    
        _update_on_timer_state(FALSE);
        u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
        CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
#endif /* CUSTOM_POWER_STATE_CTRL_ENABLE */
        // Standby LED ON
        u4LedCtrl = fgLedStandbyDiable ? D_LED_STANDBY_OFF : D_LED_STANDBY_ON;
        CustomLEDCtrlFunc(D_LED_STANDBY, u4LedCtrl);
        break;
#endif
    case eLedDarkLed:
    case eLedIrOn:
    case eLedIrOff:
        break;

    case eLedVgaStandby:
        Printf("eLedVgaStandby\r\n");
        //VGA Standby LED ON
        // Power LED OFF
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        
        // TIMER LED ON/OFF
#ifdef CC_MTK_LOADER
        _ldr_update_on_timer_state();
#else
        _update_on_timer_state(FALSE);
#endif        
        u4LedCtrl = _fgTimerOnEnable ? D_LED_TIMER_ORANGE : D_LED_TIMER_OFF;
        CustomLEDCtrlFunc(D_LED_TIMER, u4LedCtrl);
        
        // Standby LED ON
        u4LedCtrl = fgLedStandbyDiable ? D_LED_STANDBY_OFF : D_LED_STANDBY_ON;
        CustomLEDCtrlFunc(D_LED_STANDBY, u4LedCtrl);
        break;

    case eLedPanelIDFail:
	    Printf("eLedPanelIDFail\r\n");
	    _fgLedBlink[D_LED_POWER] = FALSE;
	    _fgLedBlink[D_LED_STANDBY] = FALSE;
	    _fgLedBlink[D_LED_TIMER] = FALSE;
	    _fgLedAllBlink = FALSE;

	    // Standby LED OFF
	    CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_OFF);
	    // Orange LED OFF
	    CustomLEDCtrlFunc(D_LED_TIMER, D_LED_TIMER_OFF);
	    // Power LED ON
	    CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
	    CustomPanelErrBreakFunc(); //forever loop blinking
	    break;

#ifdef CC_MTK_LOADER
    case eLedUpgradeBlink:
        Printf("eLedUpgradeBlink\r\n");
        LOADER_Custom_TimerOff();
#ifdef TWO_LED_MODE
       CustomLEDCtrlFunc(D_LED_TOTAL, D_LED_GENERIC_OFF);		
#else
        // Power LED OFF
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        // Standby LED ON
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_ON);
        // Orange LED ON
        CustomLEDCtrlFunc(D_LED_TIMER, D_LED_TIMER_ORANGE);
#endif
        //Orange LED blinking On
		_fgLedBlink[D_LED_TOTAL]=FALSE;
        _fgLedAllBlink = TRUE;//blink at the same time

        LOADER_Custom_TimerOn();
        Printf("Begin USB Upgrade\r\n");
		_PRINT_FACTORY_MESSAGE(5,("USB Update Start\n"));		
		_ENABLE_UART_RX(UART_SELECTION_IS_FACTORY(),FALSE);//disable uart RX when do usb upgrade
        break;

    case eLedUpgradeFinish:
        Printf("eLedUpgradeFinish\r\n");
        LOADER_Custom_TimerOff();
		
        _fgLedAllBlink = FALSE;
        // Power LED OFF
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        // Standby LED OFF
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_OFF);
        // Orange LED OFF
        CustomLEDCtrlFunc(D_LED_TIMER, D_LED_TIMER_OFF);
		
		_fgFixedLedPattern=FALSE;//d130816_Haibo:now is power on led.need to check LED Indicator  ON/OFF setting in UI
        // Power LED ON
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
		
        Printf("Finish USB Upgrade\r\n");
		#ifdef CC_SUPPORT_USB_DIRECT_UART 
       // Printf("Set %d as %d\n",USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL?0:1);
		GPIO_SetOut(USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL?0:1);
        #endif
		_ENABLE_UART_RX(TRUE,TRUE)//enable uart RX after do usb upgrade		
        _PRINT_FACTORY_MESSAGE(5,("USB Update Complete\n"));	
        break;

    case eLedUpgradeFail:
        Printf("eLedUpgradeFail\r\n");
		#ifdef CC_SUPPORT_USB_DIRECT_UART 
        //Printf("Set %d as %d\n",USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL?0:1);
		GPIO_SetOut(USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL?0:1);
        #endif	
		_ENABLE_UART_RX(TRUE,TRUE)//enable uart RX after do usb upgrade	
		_PRINT_FACTORY_MESSAGE(5,("USB Update Error\n"));		
#ifdef TWO_LED_MODE
        //it is orange blinking now, and the spec. related to upgrade fail is orange => red => orange => red
        //so, keep blinking and turn on red
        
		LOADER_Custom_TimerOff();
		HAL_Delay_us(CUSTOM_USB_UPGRADE_BLINK_PERIOD*1000);//wait the last timer isr finished
		
		_fgLedAllBlink = FALSE;
		CustomLEDCtrlFunc(D_LED_TOTAL, D_LED_GENERIC_OFF);	

        //power led off		
		CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        // Standby red LED ON
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_ON);
#else
         LOADER_Custom_TimerOff();
        _fgLedBlink[D_LED_POWER] = FALSE;
        _fgLedBlink[D_LED_STANDBY] = FALSE;
        _fgLedBlink[D_LED_TIMER] = FALSE;
        _fgLedAllBlink = FALSE;

        // Standby LED OFF
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_ON);
        // Orange LED OFF
        CustomLEDCtrlFunc(D_LED_TIMER, D_LED_TIMER_OFF);
        // Power LED ON
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
#endif
        _LdrUpgradeBreakFunc();
        break;
    case eLedUpgrageStart:
		#ifdef CC_SUPPORT_USB_DIRECT_UART 
        //Printf("eLedUpgrageStart.Set %d as %d\n",USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL);
		GPIO_SetOut(USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL);
        #endif		
		break; 
    case eLedUpgrageStop:
		#ifdef CC_SUPPORT_USB_DIRECT_UART 
       // Printf("eLedUpgrageStop.Set %d as %d\n",USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL?0:1);
		GPIO_SetOut(USB_DIRECT_UART_SWITCH_GPIO,USB_DIRECT_UART_SWITCH_GPIO_POL?0:1);
        #endif		
		break;
#endif /* CC_MTK_LOADER */
    default:
        break;
    }
}

void CustomSetTimerLED(BOOL bIsEnable)
{
    //enable on timer control for all 2H models
//    if (CustomPCBAQueryCountry() == DRV_CUSTOM_COUNTRY_PAA)
//    {
        _fgTimerOnEnable = bIsEnable;
        #ifdef CC_AC_DETECT_ENABLE
        if(IS_AC_DETECT_STANDBY())
        {
            _fgTimerOnEnable = FALSE;
        }
        #endif 		
//    }
}

void _LdrUpgradeBreakFunc(void)
{
    while (1)
    {
	//upgrade fail led:ORANGE->read->Orange 1sec on,1sec off
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
        //CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_POWER_ON);
        x_thread_delay(CUSTOM_USB_UPGRADE_BLINK_PERIOD);
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_OFF);
        //CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_POWER_OFF);
        x_thread_delay(CUSTOM_USB_UPGRADE_BLINK_PERIOD);
    }
}

void CustomPanelErrBreakFunc(void)
{
    while (1)
    {
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_ON);
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
        x_thread_delay(500);
        CustomLEDCtrlFunc(D_LED_STANDBY, D_LED_STANDBY_OFF);
        CustomLEDCtrlFunc(D_LED_POWER, D_LED_POWER_ON);
        x_thread_delay(500);
    }
}

#ifndef CC_MTK_LOADER 
static HANDLE_T _hLedBlinkTimer=NULL_HANDLE; 
static void LedBlinkFunc(HANDLE_T hHandle, VOID* pTag)
{
   UINT32 u4LedState=D_LED_TIMER_OFF;
   INT32 i4Gpio=0;
   INT32 i4Val=0;
   if(pTag)
   {
     u4LedState=*(UINT32 *)pTag;
   }
   
   if(u4LedState==D_LED_TIMER_ORANGE)
   {
      i4Gpio = CUSTOM_GPIO_LED_POWER;
      i4Val=GPIO_Output(i4Gpio,NULL);
      i4Val=1-i4Val;
      GPIO_SetOut(i4Gpio,i4Val);
	  i4Gpio = CUSTOM_GPIO_LED_STANDBY;
      i4Val=GPIO_Output(i4Gpio,NULL);
      i4Val=1-i4Val;
      GPIO_SetOut(i4Gpio,i4Val);  
   }
   else if(u4LedState==D_LED_TIMER_GREEN)
   {
       i4Gpio = CUSTOM_GPIO_LED_POWER;
	   i4Val=GPIO_Output(i4Gpio,NULL);
       i4Val=1-i4Val;
       GPIO_SetOut(i4Gpio,i4Val);
   }
   else
   {
      return;
   }  
}
void vStartLedBlink(UINT32 u4LedState)
{
   static UINT32 _blinkLedState=D_LED_TIMER_OFF;
   INT32 i4Gpio=-1;
   if((u4LedState!=D_LED_TIMER_ORANGE) && (u4LedState!=D_LED_TIMER_GREEN))
   {
       Printf("Unkown led state %d\n",u4LedState);
	   return;
   }
   
   _blinkLedState=u4LedState;
   
    if((HANDLE_T *)_hLedBlinkTimer == NULL)
	{
    if (x_timer_create(&_hLedBlinkTimer) != OSR_OK)
    {
        Printf("%s create timer fails\n",__FUNCTION__);
        return;
    }	
	}
	else
	{
	    x_timer_stop(_hLedBlinkTimer); 
	}
	//power off firstly
	  i4Gpio = CUSTOM_GPIO_LED_POWER;
      GPIO_SetOut(i4Gpio,CUSTOM_LED_POWER_ON_VAL?0:1);
      i4Gpio = CUSTOM_GPIO_LED_STANDBY;
	  GPIO_SetOut(i4Gpio,CUSTOM_LED_STANDBY_ON_VAL?0:1);  
	  
	if((HANDLE_T *)_hLedBlinkTimer != NULL)
	{
	     UNUSED(x_timer_start(_hLedBlinkTimer, 1000,
                                     X_TIMER_FLAG_REPEAT, LedBlinkFunc,
                                     &_blinkLedState));
	}
}
void vStopLedBlink(UINT32 u4LedState)
{
   INT32 i4Gpio=0;
   
   if((HANDLE_T *)_hLedBlinkTimer != NULL)
   {
      x_timer_stop(_hLedBlinkTimer);
	  x_timer_delete(_hLedBlinkTimer);
	  _hLedBlinkTimer=NULL_HANDLE;
   }
   else
   {
     return;
   }
   
   if(u4LedState==D_LED_TIMER_ORANGE)
   {
      i4Gpio = CUSTOM_GPIO_LED_POWER;
      GPIO_SetOut(i4Gpio,CUSTOM_LED_POWER_ON_VAL?0:1);
      i4Gpio = CUSTOM_GPIO_LED_STANDBY;
	  GPIO_SetOut(i4Gpio,CUSTOM_LED_STANDBY_ON_VAL?0:1);  	  
   }
   else if(u4LedState==D_LED_TIMER_GREEN)
   {
       i4Gpio = CUSTOM_GPIO_LED_POWER;
	   GPIO_SetOut(i4Gpio,CUSTOM_LED_POWER_ON_VAL?0:1);
   }
   else
   {
      Printf("Unkown led state %d\n",u4LedState);
	  return;
   }
   UpdateLEDGPIOAccordingToLEDOnState(bLedMode);  
}
#if 1//def STELLAR_LED_SPEC
void vSonyLedModeChg(UINT16 u2LedMode) 
{
	if(u2LedMode == 0)//d130805_Haibo:OFF ALL LED
	{
	    IRRC_LedBlinkConfig(FALSE);
		UpdateLEDGPIOAccordingToLEDOnState(0);
	}
	else//d130805_Haibo:enable IRRC Responase led & enable current led pattern
	{
	   UpdateLEDGPIOAccordingToLEDOnState(1);
	   IRRC_LedBlinkConfig(TRUE);
	}
	
	bLedMode = u2LedMode?1:0;
	return;
}
#endif /*STELLAR_LED_SPEC*/
void vUpdateLedFixPattern(UINT8 u1Fixpattern)
{
    _fgFixedLedPattern=u1Fixpattern;
	Printf("[%s]update as %d\n",__FUNCTION__,u1Fixpattern);
}
#endif

