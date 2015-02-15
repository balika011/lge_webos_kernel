
#include "x_hal_arm.h"
#include "x_hal_io.h"
#include "x_gpio.h"
#include "x_timer.h"
#include "eeprom_if.h"
#include "pdwnc_if.h"
#include "mute_if.h"
#include "rtc_if.h"
#include "sv_const.h"
#include "drvcust_if.h"
#include "drv_display.h"
#include "SelfDiag.h"
#include "SelfDiag_debug.h"
#include "CustomCtrl.h"
#ifdef CUSTOM_EEPROM_FILE
#include CUSTOM_EEPROM_FILE
#endif
//-----------------------------------------------------------------------------
// Constant definitions & Macro
//-----------------------------------------------------------------------------
#define CHK_SELF_DIAG(Condition, TimeCount, TimeThres, ErrorCode)       \
    if (!_bfgSelfDiagGo)                                                \
    {                                                                   \
        break;                                                          \
    }                                                                   \
    if (!_u1StdwnMasterEn)                                              \
    {                                                                   \
        break;                                                          \
    }                                                                   \
    if (Condition)                                                      \
    {                                                                   \
        TimeCount += SELF_DIAG_DETECT_PERIOD;                           \
        if (TimeCount >= TimeThres)                                     \
        {                                                               \
           printk(KERN_ALERT"Enter Safety Shutdown for Error (%d)\n", ErrorCode);\
           printk(KERN_ALERT"TimeCount = %d ms, TimeThres = %d ms\n", TimeCount, TimeThres);\
            _bfgSelfDiagGo = FALSE;                                     \
            SelfDiagSetShutdown(ErrorCode);                             \
            break;                                                      \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        TimeCount = 0;                                                  \
    }

#define GET_SELF_DIAG_THRES(Addr, Thres, Max, Value, DataLen, Default)  \
    UNUSED(EEPROM_Read(Addr, (UINT32)(VOID *)&Value, DataLen));         \
    if (DataLen == sizeof(UINT16))                                      \
    {                                                                   \
        Value = (((Value & 0xFF00) >> 8) | ((Value & 0xFF) << 8));      \
    }                                                                   \
    Thres = (Value >= Max) ? Default : Value;                           \
    if (Thres != Value)                                                 \
    {                                                                   \
        Value = Thres;                                                  \
        if (DataLen == sizeof(UINT16))                                  \
        {                                                               \
            Value = (((Thres & 0xFF00) >> 8) | ((Thres & 0xFF) << 8));  \
        }                                                               \
        UNUSED(EEPROM_Write(Addr, (UINT32)(VOID *)&Value, DataLen));    \
    }

#define CHK_SELF_DIAG_PAUSE()                                           \
    if (_bfgSelfDiagPause)                                              \
    {                                                                   \
        x_thread_suspend();                                             \
    }


//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Imported funtions 
//-----------------------------------------------------------------------------
extern void CHIP_Reboot(void);
extern UINT32 FlashPanel_GetInchSize(void);
extern void PDWNC_Custom_EnterNextState(UINT32 u4NextState);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static INT32  _i4ThresTemp = 0;
static UINT8  _u1SelfDiagOnOffCnt = 0;
static UINT32 _u4SelfDiagModelType = 0;
//static UINT32 _u4Detect12VAmpTimeCnt = 0;
static UINT32 _u4Detect12VSWTimeCnt = 0;
static UINT32 _u4Detect24VPwrTimeCnt = 0;
static UINT32 _u4DetectAudTimeCnt = 0;
#ifdef SALVIA_MODEL
static UINT32 _u4DetectSubWooferTimeCnt = 0;
#endif
//static UINT32 _u4Detect3V3_5VTimeCnt = 0;
static UINT32 _u4DetectPanelPWRTimeCnt = 0;
static UINT32 _u4DetectBacklightTimeCnt = 0;
static UINT32 _u4BacklightIsNormalTime = 0;
//static UINT32 _u4DetectTemperatureTimeCnt = 0;
//static UINT32 _u4DetectTunerTimeCnt = 0;
//static UINT32 _u4DetectTconTimeCnt = 0;
//#ifdef SONY_LCCT_24
static UINT32 _u4DetectBatteryTimeCnt = 0;
static BOOL _bfgUseBattery = FALSE;
#define DETECT_BATTERY_TIME  150
//#endif
static HANDLE_T _hSelfDiagHandle = NULL_HANDLE;
static HANDLE_T _hSelfDiagSema = NULL_HANDLE;
static HANDLE_T _hSelfDiagErrorHandle = NULL_HANDLE;
static HANDLE_T _hSelfDiagTimer = NULL_HANDLE;
static BOOL _bfgSelfDiagCheckBacklight = FALSE;
static BOOL _bfgSelfDiagMuteOff = FALSE;
static BOOL _bfgSelfDiagPause = FALSE;
static BOOL _bfgSelfDiagBackltOn = FALSE;
static BOOL _bfgSelfDiagInit = FALSE;
//static BOOL _bfgDetectRTC =TRUE;
#ifndef SALVIA_MODEL
static BOOL _bfgDetectRTC =TRUE;
#else
static BOOL _bfgDetectSubWoofer =TRUE;
#endif
static BOOL _bfgDetectCarBattery =TRUE;
//#ifndef SONY_LCCT_24
//static BOOL _bfgDetectTouchSnr =TRUE;
//#endif
#ifdef CC_SELF_DIAG_FORCE_DISABLE
static BOOL _bfgSelfDiagForceDisable = TRUE;
#else
static BOOL _bfgSelfDiagForceDisable = FALSE;
#endif /* CC_SELF_DIAG_FORCE_DISABLE */

//#ifdef SONY_LCCT_24
UINT8 _u1Battery_mode =0xff;
//#endif

BOOL   _bfgSelfDiagGo = FALSE;
HANDLE_T _hSelfDiagMsgQue = NULL_HANDLE;
SELF_DIAG_TABLE _eSelfDiagEnable;
SELF_DIAG_TABLE _eSelfDiagShutdownLog;
SELF_DIAG_TABLE _eSelfDiagShutdownRetryEnable;

UINT8  _u1StdwnMasterEn =0;
extern BOOL _bgIsCMO37Panel;
extern BOOL _bgIsThermalDetect;
extern UINT32 _u4gModelType;
#ifdef Panel_ID_ERROR_DETECT
static UINT32 _u4DetectPanelNimTimeCnt = 0;
#endif

#ifndef SALVIA_MODEL
#define EXT_RTC_I2C_ADDRESS  0xa2
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
//#define MAX_SELF_DIAG_ERROR_IDX 12
/*
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
    D_TCON_ERROR,
    D_BALANCER_ERROR,    
//    D_PANEL_NVM_ERROR,//atm-liang add    
    D_TUNER_ERROR,
    D_ALL_ERROR//Should be put at last (for i4SelfDiagClearCounterCmd())
} D_SELF_DIAG_ERROR_TYPE;
*/
#if 0
static const char* _SELF_DIAG_ERR_Name[] = 
{
	"12V_SW Error",
	"Panel Power Error",
    "Backlight Error",	
    "Temperature Error", 
	"Audio Error",
	"Balancer Error",
    "Tuner Error",	
    "T-con Error",	
	"Panel I2C Error",
	"12V_AMP Error",
	"24V Error",
	"3V3/5V Error",
    "All Error",	    
    "Undefine Error",    
};
#else
static const char* _SELF_DIAG_ERR_Name[] = 
{
	"12V_AMP Error",
	"12V_SW Error",
	"24V Error",
	"Audio Error",
	"3V3/5V Error",
    "Panel Power Error",	
    "Backlight Error",	
    "Temperature Error",	
    "T-con Error",	
	"Balancer Error",
	"Panel I2C Error",
    "Tuner Error",	
    #ifdef SALVIA_MODEL
    "SubWoofer Error" 
    #endif
    "All Error",	    
    "Undefine Error",    
};
#endif
static const char* _StateCode2String(UINT32 u4code)
{
    UINT32 u4ErrTypes = sizeof(_SELF_DIAG_ERR_Name)/sizeof(char*);
	if(u4code < u4ErrTypes)
	{
		return _SELF_DIAG_ERR_Name[u4code];
	}
	else
	{
		return _SELF_DIAG_ERR_Name[u4ErrTypes - 1]; //"Undefine Error"
	}
}
/*
typedef struct _SELF_DIAG_TABLE_T
{
    UINT16  u2TemperatureError : 1;     //Bit0 of low byte of Shutdown Log
    UINT16  u2BacklightError : 1;       //Bit1 of low byte of Shutdown Log
    UINT16  u2PanelPowerError : 1;      //Bit2 of low byte of Shutdown Log
    UINT16  u2VDrop3V_5VError : 1;      //Bit3 of low byte of Shutdown Log
    UINT16  u2AudError : 1;             //Bit4 of low byte of Shutdown Log
    UINT16  u224VError : 1;             //Bit5 of low byte of Shutdown Log
    UINT16  u2SW12VError : 1;           //Bit6 of low byte of Shutdown Log
    UINT16  u2Amp12VError : 1;          //MSB of low byte of Shutdown Log
    UINT16  u2Reverved : 3;             //Bit3~0 of high byte of Shutdown Log
    UINT16  u2AllError : 1;             //Bit4 of high byte of Shutdown Log
    UINT16  u2TunerError : 1;           //Bit5 of high byte of Shutdown Log
    UINT16  u2BalancerError : 1;        //Bit6 of high byte of Shutdown Log
    UINT16  u2TconError : 1;            //MSB of high byte of Shutdown Log
} SELF_DIAG_TABLE;
*/
#if defined (CC_SELF_DIAG_DEFAULT_ON)
static SELF_DIAG_TABLE _SetSelfDiagEnable(UINT16 u2flag)
{
    SELF_DIAG_TABLE eEnable;
    *((UINT16 *)((VOID *)&eEnable)) = 0xffff; //disable all, 1: disable; 0: enable
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_12V_AMP_ERROR))
    {
        eEnable.u2Amp12VError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_12V_SW_ERROR))
    {
        eEnable.u2SW12VError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_24V_PWR_ERROR))
    {
        eEnable.u224VError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_AUD_ERROR))
    {
        eEnable.u2AudError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_3V3_5V_ERROR))
    {
        eEnable.u2VDrop3V_5VError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_PANEL_POWER_ERROR))
    {
        eEnable.u2PanelPowerError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_BACKLIGHT_ERROR))
    {
        eEnable.u2BacklightError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_TEMPERATURE_ERROR))
    {
        eEnable.u2TemperatureError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_TCON_ERROR))
    {
        eEnable.u2TconError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_BALANCER_ERROR))
    {
        eEnable.u2BalancerError = SELF_DIAG_ENABLE;
    }
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_TUNER_ERROR))
    {
        eEnable.u2TunerError = SELF_DIAG_ENABLE;
    }
	if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_PANEL_NVM_ERROR))
    {
        eEnable.u2PanelNIMError = SELF_DIAG_ENABLE;
    }
    #ifdef SALVIA_MODEL
	if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_SUBWOOFER_ERROR))
    {
        eEnable.u2SubWooferError = SELF_DIAG_ENABLE;
    }
    #else
    if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_RTC_ERROR))
    {
        eEnable.u2RtcError = SELF_DIAG_ENABLE;
    }
    #endif
	if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_TOUCHSNR_ERR))
    {
        eEnable.u2TouchSnrError = SELF_DIAG_ENABLE;
    }
	if(u2flag & SELF_DIAG_FUNCTION_ENABLE(D_CAR_BATTERY))
    {
        eEnable.u2CarBatteryError = SELF_DIAG_ENABLE;
    }
    eEnable.u2AllError = SELF_DIAG_ENABLE;
    return eEnable;
}
#endif
#if 0
static UINT32 CustomCheckAudioAmp(void)
{
    UINT8  u1AmpStatus = 0x0;
//need to write 0 to 0x2 reg. before read 0x2 reg.
    UNUSED(SIF_Write(CUSTOM_AMP_SIF_CLK_DIV, 
                    CUSTOM_AMP_SIF_DEV_ADDR,
                    CUSTOM_AMP_SIF_SUB_ADDR,
                    &u1AmpStatus, CUSTOM_AMP_DATA_LEN));

   	u1AmpStatus = 0xff;
    UNUSED(SIF_Read(CUSTOM_AMP_SIF_CLK_DIV, 
                    CUSTOM_AMP_SIF_DEV_ADDR,
                    CUSTOM_AMP_SIF_SUB_ADDR,
                    &u1AmpStatus, CUSTOM_AMP_DATA_LEN));
/*
d0	d1	
0	0	Normal
0	1	Error
1	0	Error
1	1	Error
*/                    
    if(u1AmpStatus & 0x03)
    {
        return 0;//something wrong...
    }
    else
    {    
        return 1;        
    }
}
#endif

#if 0
UINT32 _u4AMPErrorCount=0;
static void SelfDiagAMPErrorDetect(INT32 i4Gpio, BOOL fgStatus)
{
    UNUSED(i4Gpio);
    UNUSED(fgStatus);
    _u4AMPErrorCount++;
	//Printf("_u4AMPErrorCount1 %d \n",_u4AMPErrorCount);
}
#endif

static void SelfDiagTimerHandle(HANDLE_T hHandle, VOID* pTag)
{
    UNUSED(hHandle);
    UNUSED(pTag);
    
    x_thread_resume(_hSelfDiagHandle);
}

static void SelfDiagTimerClrOnoffCnt(void)
{
    UINT8 u1Data =0;
	Printf("SelfDiagTimerClrOnoffCnt \n");
    UNUSED(EEPROM_Write(SELF_DIAG_RETRY_CNT_ADDR, (UINT32)(VOID *)&u1Data, 1));
}


static void SelfDiagErrorHandle(void *pvArg)
{
    UINT16 u2MsgIdx;
    UINT32 u4Msg;
    PFN_USR_POWER_SEQUENCE pfnUsrPowerOffSequence = NULL;
    PFN_USR_POWER_SEQUENCE pfnUsrPowerOnSequence = NULL;

    // Query panel power on/off sequence function
    UNUSED(DRVCUST_InitQuery(eCustomerPowerOffSequence, 
                             (UINT32 *)(void *)&(pfnUsrPowerOffSequence)));
    UNUSED(DRVCUST_InitQuery(eCustomerPowerOnSequence, 
                             (UINT32 *)(void *)&(pfnUsrPowerOnSequence)));

    UNUSED(pvArg);
    while (1)
    {
        SIZE_T zMsgSize = sizeof(UINT32);
        INT32  i4MsgRet;
     
        i4MsgRet = x_msg_q_receive(&u2MsgIdx, &u4Msg, &zMsgSize,
                               &_hSelfDiagMsgQue, 1, X_MSGQ_OPTION_WAIT);
        if (i4MsgRet == OSR_OK)
        {
            switch (u4Msg)
            {
            case SELF_DIAG_ACT_REBOOT:
                {
                    if (pfnUsrPowerOffSequence)
                    {
                        LOG(5, "[SelfDiagErrorHandle] ");
                        LOG(5, "Enter panel power off sequence\n");
                        pfnUsrPowerOffSequence();
                    }

                    LOG(5, "D_BACKLIGHT_ERROR : try to reboot (%d time)\n",
                        _u1SelfDiagOnOffCnt);
                    PRINT_TIMESTAMP_DBG_MSG("[Self Diag]",
                                            "D_BACKLIGHT_ERROR : try to reboot (%d time)\n",
                                            _u1SelfDiagOnOffCnt);
                    CHIP_Reboot();
                }
                break;
            case SELF_DIAG_ACT_PANEL_ONOFF:
                {
                    if (pfnUsrPowerOffSequence)
                    {
                        LOG(5, "[SelfDiagErrorHandle] ");
                        LOG(5, "Enter panel power off sequence\n");
                        pfnUsrPowerOffSequence();
                    }

                    // Wait for SMicro reboot
                    x_thread_delay(3000);
                
                    if (pfnUsrPowerOnSequence)
                    {
                        LOG(5, "[SelfDiagErrorHandle] ");
                        LOG(5, "Enter panel power on sequence\n");
                        pfnUsrPowerOnSequence();
                    }
                }
                break;
            }
        }
    }
}

static void SelfDiagHandle(void *pvArg)
{
    BOOL   bIsWoofer = FALSE;
  //  INT32  i4Temperature = 0;
    UINT32 u4Value = 0;
    UINT8  u1Value1 = 0;
    UINT32 u4AmpErroGPIOValue = 0;	
    #ifdef SALVIA_MODEL
    UINT32 u4SubWooferErroGPIOValue = 0;
    #endif
    UINT32 u4BackLightDelayTime = 0;
    //UINT32 u4MuteState;
   // UINT8  u1PanelNimValue =0;
    UINT32 u4SelfDiagtimeCnt =0;
    UINT32 gpio;
    UINT32 value; 
    UINT8 u1Value=0;
    
    UNUSED(pvArg);
    UNUSED(bIsWoofer);
    UNUSED(_u4Detect24VPwrTimeCnt);
    UNUSED(_u4DetectAudTimeCnt);
    

    PRINT_TIMESTAMP_MSG("[Power On]", "Self-Diag goes\n");

    UNUSED(EEPROM_Read(SELF_DIAG_ON_OFF_CLR_CNT_ADDR,(UINT32)&u1Value, sizeof(UINT8)));
    //Printf("*************TV CLR_CNT time %d\n",u1Value);

    while (_bfgSelfDiagGo)
    {
        // Detect D_12V_AMP_ERROR 
        /*CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2Amp12VError == SELF_DIAG_ENABLE)
        {
            u4Value = GPIO_GetIn(SELF_DIAG_12V_AMP_DROP_DET);
            LOG(9, "SELF_DIAG_12V_AMP_DROP_DET = %d\n", u4Value);
            CHK_SELF_DIAG((u4Value == 0), _u4Detect12VAmpTimeCnt,
                          SELF_DIAG_POWER_DET_TIME, D_12V_AMP_ERROR);
        }*/



        u4SelfDiagtimeCnt++;
        #if 0
        if((u4SelfDiagtimeCnt>590) && (u4SelfDiagtimeCnt<610))
        {
           Printf("@@@@@@@@@@@@@@@@TV u4SelfDiagtimeCnt %d\n",u4SelfDiagtimeCnt);
           Printf("@@@@@@@@@@@@@@@@TV CLR_CNT time %d\n",u1Value);
        }
        #endif
        if((u4SelfDiagtimeCnt*SELF_DIAG_DETECT_PERIOD) == (u1Value*60*1000))
        {
            SelfDiagTimerClrOnoffCnt();
        }
#if 0
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2Amp12VError == SELF_DIAG_ENABLE)
        {
        u4Value = PDWNC_ReadServoADCChannelValue(SELF_DIAG_12V_AMP_DROP_DET);
        LOG(9, "SELF_DIAG_12V_AMP_DROP_DET = %d\n", u4Value);
        CHK_SELF_DIAG((u4Value < SELF_DIAG_12V_AMP_DROP_MIN_VOLTAGE), _u4Detect12VAmpTimeCnt,
          SELF_DIAG_POWER_DET_TIME, D_12V_AMP_ERROR);
        }
#endif  
#ifndef SALVIA_MODEL //Caihua
        // Detect D_RTC_ERROR 
        CHK_SELF_DIAG_PAUSE();
        if ((_eSelfDiagEnable.u2RtcError == SELF_DIAG_ENABLE)&&(_bfgDetectRTC ==TRUE))
        {
            UINT8 u1Data = 0;
            UINT32 u4Value1 = 0,u4Value2 = 0,u4Value3 = 0;
            u4Value1 =SIF_X_Read(0 ,0x100, EXT_RTC_I2C_ADDRESS, 0 , 0 ,&u1Data, 1);
            LOG(9, "SELF_DIAG_RTC_ERROR = %d\n", u4Value1);
            //Printf("SELF_DIAG_RTC_ERROR u4Value1 = %d\n",u4Value1);
            u4Value2 =SIF_X_Read(0 ,0x100, EXT_RTC_I2C_ADDRESS, 0 , 0 ,&u1Data, 1);
            LOG(9, "SELF_DIAG_RTC_ERROR = %d\n", u4Value2);
            //Printf("SELF_DIAG_RTC_ERROR u4Value2 = %d\n",u4Value2);
            u4Value3 =SIF_X_Read(0 ,0x100, EXT_RTC_I2C_ADDRESS, 0 , 0 ,&u1Data, 1);
            LOG(9, "SELF_DIAG_RTC_ERROR = %d\n", u4Value3);
            //Printf("SELF_DIAG_RTC_ERROR u4Value3 = %d\n",u4Value3);

            _bfgDetectRTC =FALSE;

            //   u8Offset = (UINT64)CUSTOM_NEW_PHOTO_FRAME_OPTION_ADDR;
            //   EEPROM_Read(u8Offset, (UINT32)(void *)&u1Index, 1);
            if((u4Value1 <= 0)&&(u4Value2 <= 0)&&(u4Value3 <= 0))
            {
            UINT8 u1Count = 0;
            UINT64 u8Offset = SELF_DIAG_ERROR_CNT_ADDR;
            u8Offset += SELF_DIAG_RTC_ERR_OFFSET;
            // Read the shutdown counter from EEPROM
            UNUSED(EEPROM_Read(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));

            // Increase the shutdown counter & update to EEPROM
            u1Count++;
            UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
            }
        }
#endif
/*
#ifndef SONY_LCCT_24
        // Detect D_TOUCHSNR_ERR 
        CHK_SELF_DIAG_PAUSE();
        if ((_eSelfDiagEnable.u2TouchSnrError == SELF_DIAG_ENABLE)&&(_bfgDetectTouchSnr ==TRUE))
        {
            UINT8 u1Data = 0;
            UINT32 u4Value1 = 0,u4Value2 = 0,u4Value3 = 0;

            Printf("PANEL_EDID_READ_0X00E0 \n");
            u4Value1 =SIF_X_Read(0 ,0x100, Touch_Sensor_DEV_ADDR, 0x2, 0X40 ,&u1Data, 2);
            LOG(9, "SELF_DIAG_TouchSnr_ERROR = %d\n", u4Value1);
            //Printf("SELF_DIAG_TouchSnr_ERROR 0X00E0 u4Value1 = %d\n", u4Value1);
            u4Value2 =SIF_X_Read(0 ,0x100, Touch_Sensor_DEV_ADDR, 0x2, 0X40 ,&u1Data, 2);
            LOG(9, "SELF_DIAG_TouchSnr_ERROR = %d\n", u4Value2);
            //Printf("SELF_DIAG_TouchSnr_ERROR 0X00E0 u4Value2 = %d\n", u4Value2);
            u4Value3 =SIF_X_Read(0 ,0x100, Touch_Sensor_DEV_ADDR, 0x2, 0X40 ,&u1Data, 2);
            LOG(9, "SELF_DIAG_TouchSnr_ERROR = %d\n", u4Value3);
            //Printf("SELF_DIAG_TouchSnr_ERROR 0X00E0 u4Value3 = %d\n", u4Value3);


            _bfgDetectTouchSnr =FALSE;

            //   u8Offset = (UINT64)CUSTOM_NEW_PHOTO_FRAME_OPTION_ADDR;
            //   EEPROM_Read(u8Offset, (UINT32)(void *)&u1Index, 1);
            if((u4Value1 <= 0)&&(u4Value2 <= 0)&&(u4Value3 <= 0))
            {
                UINT8 u1Count = 0;
                UINT64 u8Offset = SELF_DIAG_ERROR_CNT_ADDR;
                u8Offset += SELF_DIAG_TOUCHSNR_ERR_OFFSET;
                // Read the shutdown counter from EEPROM
                UNUSED(EEPROM_Read(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));

                // Increase the shutdown counter & update to EEPROM
                u1Count++;
                UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
            }
        }
#endif*/




//#ifdef SONY_LCCT_24
//detect battery,should be before the REG19.5V detect
		CHK_SELF_DIAG_PAUSE();
		if ((_eSelfDiagEnable.u2CarBatteryError == SELF_DIAG_ENABLE)&&(_bfgDetectCarBattery ==TRUE))
		{
			u4Value = PDWNC_ReadServoADCChannelValue(SELF_DIAG_12V_SW_DROP_DET);
			LOG(9, "SELF_DIAG_12V_SW_DROP_DET = %d\n", u4Value);
			_u4DetectBatteryTimeCnt ++;
			if((_u4DetectBatteryTimeCnt <=5)&&(u4Value>SELF_DIAG_BATTERY_VAL_MIN)&&(u4Value < SELF_DIAG_BATTERY_VAL_MAX))
		    {
			    Printf("Use Battery now %d!!!\n",u4Value);
				_bfgUseBattery =TRUE;
			}
			_u1Battery_mode = BATTERY_NONE;
			if(_bfgUseBattery)
			{
			if((u4Value>SELF_DIAG_BATTERY_VAL_MIN)&&(u4Value<SELF_DIAG_BATTERY_VAL_MAX)&&(_u4DetectBatteryTimeCnt<=DETECT_BATTERY_TIME))
			{
			_u1Battery_mode = BATTERY_NORMAL;
			}
			if((u4Value<SELF_DIAG_BATTERY_VAL_LOW)&&(_u4DetectBatteryTimeCnt>DETECT_BATTERY_TIME))
			{
			_u1Battery_mode = BATTERY_LOW;
			}
			}
		}
//#endif
        // Detect D_12V_SW_ERROR 
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2SW12VError == SELF_DIAG_ENABLE)
        {
            u4Value = PDWNC_ReadServoADCChannelValue(SELF_DIAG_12V_SW_DROP_DET);
            LOG(9, "SELF_DIAG_12V_SW_DROP_DET = %d\n", u4Value);
            UNUSED(EEPROM_Read(SELF_DIAG_MPWR_STDN_TIME_CNT,
                                       (UINT32)(VOID *)&u1Value1,
                                       sizeof(UINT8)));
			//#ifdef SONY_LCCT_24
			if((_eSelfDiagEnable.u2CarBatteryError == SELF_DIAG_ENABLE) && (_bfgUseBattery))
			{
			   CHK_SELF_DIAG(u4Value<SELF_DIAG_BATTERY_VAL_LOW,_u4Detect12VSWTimeCnt,u1Value1*SELF_DIAG_DETECT_PERIOD,D_12V_SW_ERROR);
			}
			else
			{
			   CHK_SELF_DIAG(((u4Value < SELF_DIAG_12V_SW_DROP_VAL_MIN)||(u4Value > SELF_DIAG_12V_SW_DROP_VAL_MAX)), _u4Detect12VSWTimeCnt,
                          (u1Value1*SELF_DIAG_DETECT_PERIOD), D_12V_SW_ERROR);
			}
//			#else
//            CHK_SELF_DIAG(((u4Value < SELF_DIAG_12V_SW_DROP_VAL_MIN)||(u4Value > SELF_DIAG_12V_SW_DROP_VAL_MAX)), _u4Detect12VSWTimeCnt,
//                          (u1Value1*SELF_DIAG_DETECT_PERIOD), D_12V_SW_ERROR);
//			#endif
        }
#if 0    
        // Detect D_24V_ERROR 
        CHK_SELF_DIAG_PAUSE();
        if ((_eSelfDiagEnable.u224VError == SELF_DIAG_ENABLE) && (bIsWoofer))
        {
            u4Value = GPIO_GetIn(SELF_DIAG_24V_AMP_VDD_DROP_DET);
            LOG(9, "SELF_DIAG_24V_POWER_DET = %d\n", u4Value);
            CHK_SELF_DIAG((u4Value == 0), _u4Detect24VPwrTimeCnt,
                          SELF_DIAG_POWER_DET_TIME, D_24V_PWR_ERROR);
        }
#endif   
#if 0
// Detect D_AUD_ERROR 

	CHK_SELF_DIAG_PAUSE();
	if (_eSelfDiagEnable.u2AudError == SELF_DIAG_ENABLE)
	{   
		LOG(1,"_u4AMPErrorCount = %d \n", _u4AMPErrorCount);
		CHK_SELF_DIAG((_u4AMPErrorCount >= 5), _u4DetectAudTimeCnt,
					  SELF_DIAG_MAIN_DET_TIME, D_AUD_ERROR);
		_u4AMPErrorCount =0;
	}
#else
        // Detect D_AUD_ERROR 
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2AudError == SELF_DIAG_ENABLE)
        {
            u4AmpErroGPIOValue = GPIO_GetIn(SELF_DIAG_AUD_AMP_ERROR_DET);
            LOG(9,"SELF_DIAG_AUD_AMP_ERROR_DET = %d, SPI_CLE = %d\n", u4Value, u4AmpErroGPIOValue);
            CHK_SELF_DIAG((u4AmpErroGPIOValue == 0), _u4DetectAudTimeCnt,
                          SELF_DIAG_MAIN_DET_TIME, D_AUD_ERROR);
        }
        #ifdef SALVIA_MODEL
        //Detect D_SUBWOOFER_ERROR
        CHK_SELF_DIAG_PAUSE();
        if ((_eSelfDiagEnable.u2SubWooferError == SELF_DIAG_ENABLE)&&(_bfgDetectSubWoofer == TRUE))
        {
            u4SubWooferErroGPIOValue = GPIO_GetIn(SELF_DIAG_AUD_SUBWOOFER_ERROR_DET);
            LOG(9,"SELF_DIAG_AUD_SUBWOOFER_ERROR_DET = %d, SPI_CLE = %d\n", u4Value, u4SubWooferErroGPIOValue);
            CHK_SELF_DIAG((u4SubWooferErroGPIOValue == 0), _u4DetectSubWooferTimeCnt,
                          SELF_DIAG_MAIN_DET_TIME, D_SUBWOOFER_ERROR);
        }
        #endif
#endif
#if 0
        // Detect D_3V3_5V_ERROR 
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2VDrop3V_5VError == SELF_DIAG_ENABLE)
        {
            u4Value = GPIO_GetIn(SELF_DIAG_V_DROP_DET);
            LOG(9, "SELF_DIAG_V_DROP_DET = %d\n", u4Value);
            CHK_SELF_DIAG((u4Value == 0), _u4Detect3V3_5VTimeCnt,
                          SELF_DIAG_MAIN_DET_TIME, D_3V3_5V_ERROR);
        }
     
		// Detect D_TUNER_ERROR
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2TunerError == SELF_DIAG_ENABLE)
        {
            u4Value = GPIO_GetIn(SELF_DIAG_TUNER_PWR_DROP_DET);
            LOG(9, "SELF_DIAG_TUNER_PWR_DROP_DET = %d\n", u4Value);
            CHK_SELF_DIAG((u4Value == 1), _u4DetectTunerTimeCnt,
                          SELF_DIAG_TUNER_DET_TIME, D_TUNER_ERROR);
        }    
#endif
        // Detect D_PANEL_POWER_ERROR 
        
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2PanelPowerError == SELF_DIAG_ENABLE)
        {
                
            u4Value = PDWNC_ReadServoADCChannelValue(SELF_DIAG_PANEL_V_DROP_DET);
            //printf("SELF_DIAG_PANEL_V_DROP_DET = %d\n", u4Value);
            UNUSED(EEPROM_Read(SELF_DIAG_PPWR_STDN_TIME_CNT,
                                       (UINT32)(VOID *)&u1Value1,
                                       sizeof(UINT8)));

#if 0                                       
            UINT32 u1Val,u1Val2;
            DRVCUST_PanelQuery(ePanelIndexOffset1, &u1Val);
            UNUSED(EEPROM_Read(u1Val,
                                  (UINT32)(VOID *)&u1Val2,
                                  sizeof(UINT8)));
            if(u1Val2 == 1)//use the 22 inch panel the panel power should change to 0.64v
            {
            CHK_SELF_DIAG(((u4Value < SELF_DIAG_22_INCH_PANEL_POWER_DROP_VAL_MIN)||(u4Value > SELF_DIAG_22_INCH_PANEL_POWER_DROP_VAL_MAX)), _u4DetectPanelPWRTimeCnt,
                      (u1Value1*SELF_DIAG_DETECT_PERIOD), D_PANEL_POWER_ERROR);
            }
            else
            {
#endif
            CHK_SELF_DIAG(((u4Value < SELF_DIAG_PANEL_POWER_DROP_VAL_MIN)||(u4Value > SELF_DIAG_PANEL_POWER_DROP_VAL_MAX)), _u4DetectPanelPWRTimeCnt,
                          (u1Value1*SELF_DIAG_DETECT_PERIOD), D_PANEL_POWER_ERROR);
//              }
        }


        // Detect D_TCON_ERROR 
#if 0
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2TconError == SELF_DIAG_ENABLE)
        {
            u4Value = GPIO_GetIn(SELF_DIAG_TCON_RDY_DET);
            LOG(9, "SELF_DIAG_TCON_RDY_DET = %d\n", u4Value);
            UNUSED(EEPROM_Read(PANEL_ID_PANEL_MODE,
                                       (UINT32)(VOID *)&u1Value1,
                                       sizeof(UINT8)));
            CHK_SELF_DIAG(((u4Value == 0)&&((u1Value1 ==PANEL_MODE)||(u1Value1 ==PANEL_DISABLE_LESS_MODE)||(u1Value1 ==PANEL_DET_DIABLE))), _u4DetectTconTimeCnt,
                          SELF_DIAG_TCON_DET_TIME, D_TCON_ERROR);
        }
#endif

        // Detect PANEL_NIM_ERROR 
       #ifdef Panel_ID_ERROR_DETECT
        CHK_SELF_DIAG_PAUSE();
        if (_eSelfDiagEnable.u2PanelNIMError == SELF_DIAG_ENABLE)
        {
            UINT8 u1PanelNimValue[ATV_PANEL_ID_STRING_LEN];
            u4Value = SIF_X_Read(ATV_60HZ_PANEL_SIF_BUS,ATV_60HZ_PANEL_SIF_CLK_DIV, ATV_60HZ_PANEL_NVM_ADDRESS, 0x1,
        						ATV_60HZ_PANEL_ID_ON_NVM_OFFSET,
        						u1PanelNimValue, ATV_PANEL_ID_STRING_LEN);

          //    u4Value = PANEL_I2C_ERROR;
           LOG(9, "SELF_DIAG_PANEL_NIM_RDY_DET = %d\n", u4Value);
            UNUSED(EEPROM_Read(PANEL_ID_PANEL_MODE,
                                       (UINT32)(VOID *)&u1Value1,
                                       sizeof(UINT8)));
            CHK_SELF_DIAG(((u4Value == 0)&&((u1Value1 ==PANEL_MODE)||(u1Value1 ==PANEL_DISABLE_LESS_MODE))), _u4DetectPanelNimTimeCnt,
                          SELF_DIAG_TCON_DET_TIME, D_PANEL_NVM_ERROR);
        }
        #endif
        // Detect D_BACKLIGHT_ERROR 
        CHK_SELF_DIAG_PAUSE();
    //OG(9, "SELF_DIAG_LAMP_DET = %d\n", u4Value);
    if (DRVCUST_PanelQuery(eBackLightGpio, &gpio) == 0 &&
        DRVCUST_PanelQuery(eBackLightOnVal, &value) == 0 )
        {
           if((GPIO_Enable(gpio, NULL) == 1) && (GPIO_Output(gpio, NULL) == value))
             {
//    #ifndef SONY_LCCT_24
               u4Value = GPIO_GetIn(SELF_DIAG_LAMP_DET);
               if (u4Value == 0)
//    #else
//               u4Value =  PDWNC_ReadServoADCChannelValue(SELF_DIAG_LAMP_DET);
//               if (u4Value < SELF_DIAG_BL_VALID_MAX_VOLTAGE)    
//    #endif
                 {
                   if ((_eSelfDiagEnable.u2BacklightError == SELF_DIAG_ENABLE) &&
                       _bfgSelfDiagCheckBacklight && _bfgSelfDiagBackltOn)
                      {
                        PRINT_TIMESTAMP_DBG_MSG("[Self Diag]", "LAMP_DET = %x \n", u4Value);
                       _u4BacklightIsNormalTime = 0;
                       _u4DetectBacklightTimeCnt += SELF_DIAG_DETECT_PERIOD;
                       if (_u4DetectBacklightTimeCnt >= SELF_DIAG_INVERTER_DET_TIME)                                     \
                        {
                           // Read OnOff counter from EEPROM
                           
//    #ifndef SONY_LCCT_24
                           UNUSED(EEPROM_Read(PANEL_ID_PANEL_MODE,
                                              (UINT32)(VOID *)&u1Value1,
                                               sizeof(UINT8)));
                           if((u1Value1 !=PANEL_MODE)&&(u1Value1 !=PANEL_DISABLE_LESS_MODE)&&(u1Value1 !=PANEL_DET_DIABLE))
                               break;
/*    #else
	                       UNUSED(EEPROM_Read(PANEL_ID_PANEL_MODE,
                                              (UINT32)(VOID *)&u1Value1,
                                               sizeof(UINT8)));
                           if(u1Value1 ==PANEL_LESS_MODE)
                               break;
	#endif*/
                       
                           UNUSED(EEPROM_Read(SELF_DIAG_ON_OFF_CNT_ADDR,
                                              (UINT32)(VOID *)&_u1SelfDiagOnOffCnt,
                                              sizeof(UINT8)));
                    
                           // If the EEPROM is empty, reset the OnOff counter
                           if (_u1SelfDiagOnOffCnt == 0xFF)
                           {
                               _u1SelfDiagOnOffCnt = 0;
                           }
                    
                           // If the OnOff counter < retry limit (3), reboot TV
                           if (++_u1SelfDiagOnOffCnt < SELF_DIAG_ON_OFF_RETRY_LIMIT)
                           { 
                               UNUSED(EEPROM_Write(SELF_DIAG_ON_OFF_CNT_ADDR,
                                                   (UINT32)(VOID *)&_u1SelfDiagOnOffCnt,
                                                   sizeof(UINT8)));

                               // Start the Error handle task to reboot
                               //UINT32 u4Value = SELF_DIAG_ACT_REBOOT;
                               u4Value = SELF_DIAG_ACT_REBOOT;
                               UNUSED(x_msg_q_send(_hSelfDiagMsgQue,
                                               &u4Value, sizeof(UINT32), 1));
                           }
                           else
                           {
                               _u1SelfDiagOnOffCnt = 0;
                               if (!_u1StdwnMasterEn)                                              \
                               {                                                                   \
                                 break;                                                          \
                               }
                               SelfDiagSetShutdown(D_BACKLIGHT_ERROR);
                               break;
                           }
                         }
                      }
                  }
            else
            {
                static BOOL bfgCheckNormal = TRUE;
            
                _u4DetectBacklightTimeCnt = 0;
                if (bfgCheckNormal)
                {
                    _u4BacklightIsNormalTime += SELF_DIAG_DETECT_PERIOD;
                    if (_u4BacklightIsNormalTime > SELF_DIAG_BL_RESET_TIME)
                    {
                        // Reset the reboot count
                        _u1SelfDiagOnOffCnt = 0;
                        bfgCheckNormal = FALSE;
                        UNUSED(EEPROM_Write(SELF_DIAG_ON_OFF_CNT_ADDR,
                                            (UINT32)(VOID *)&_u1SelfDiagOnOffCnt,
                                            sizeof(UINT8)));
                        PRINT_TIMESTAMP_MSG("[Power On]", "Reset the reboot count\n");
                    }
                }
            }
          }
        }
        UNUSED(x_timer_start(_hSelfDiagTimer, SELF_DIAG_DETECT_PERIOD,
                             X_TIMER_FLAG_ONCE, SelfDiagTimerHandle, NULL));
        x_thread_suspend();

        if (!_bfgSelfDiagCheckBacklight)
        {
#if 0//d130705_Haibo:Should detect backlight error in all condition		
            if (!_bfgSelfDiagMuteOff)
            {
                u4MuteState = _u4DrvVideoGetMute(SV_VP_MAIN);
                LOG(5, "[SelfDiagHandle] Check mute state = %d\n",
                       u4MuteState);
                if (u4MuteState == SV_OFF)
                {
                    PRINT_TIMESTAMP_MSG("[Power On]", "Picture mute off\n");
                    _bfgSelfDiagMuteOff = TRUE;
                }
                else
                {
                    continue;
                }
            }
#endif            
            u4BackLightDelayTime += SELF_DIAG_DETECT_PERIOD;
            if (u4BackLightDelayTime > SELF_DIAG_BL_DELAY_TIME)
            {
                PRINT_TIMESTAMP_MSG("[Power On]", "Start backlight & balancer check\n");
                _bfgSelfDiagCheckBacklight = TRUE;
                u4BackLightDelayTime = 0;
            }
        }
    }
    
    VERIFY(x_sema_unlock(_hSelfDiagSema) == OSR_OK);
    x_sema_delete(_hSelfDiagSema);
    _hSelfDiagSema = NULL_HANDLE;

    x_thread_exit();
}

static void SelfDiagSetDefault(void)
{
    UINT8 au1Data[SELF_DIAG_EEPROM_SIZE];
    UINT8 u1DataInit = 0;
    UINT8 u1Val = 0;
    UINT32 u4Len = sizeof(au1Data);
    UINT32 i;
    SELF_DIAG_TABLE eSelfDiagTbl;

    x_memset((void *)au1Data, 0xFF, u4Len);
    
    // Get Self-Diag data init bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_DATA_INIT_ADDR,
                      (UINT32)(VOID *)&u1DataInit, sizeof(UINT8)));

    if (!(u1DataInit & SELF_DIAG_LOAD_DEFAULT))
    {
        // Get Self-Diag data from EEPROM
        UNUSED(EEPROM_Read(SELF_DIAG_DET_ENABLE_ADDR,
                           (UINT32)(VOID *)au1Data, u4Len));

        // Check if all 64 bytes are 0xFF
        // If yes -> EEPROM is empty and should load default value
        for (i = 0; i < u4Len; i++)
        {
	        if (au1Data[i] != 0xFF)
            {
                return;
            }
        }
    }

    // Set default value if EEPROM is empty
    // Disable Self-Diag Enable bit
#if defined (CC_SELF_DIAG_DEFAULT_ON)
    //*((UINT16 *)((VOID *)&eSelfDiagTbl)) = 0xC020;  /*Disable: u224VError, u2BalancerError and u2TconError detection */
    eSelfDiagTbl = _SetSelfDiagEnable((UINT16)DEFAULT_SELF_DIAG_ENABLED_ITEMS);
    Printf("Self-Diag default on (0x%2x)\n",*((UINT16 *)((VOID *)&eSelfDiagTbl)));
#else
    *((UINT16 *)((VOID *)&eSelfDiagTbl)) = 0xFFFF;  /*Disable*/
    Printf("Self-Diag default off\n");
#endif
    UNUSED(EEPROM_Write(SELF_DIAG_DET_ENABLE_ADDR,
                        (UINT32)(VOID *)&eSelfDiagTbl, sizeof(UINT16)));
    
	 // Clear Self-Diag Shutdown log
    *((UINT16 *)((VOID *)&eSelfDiagTbl)) = 0;
    UNUSED(EEPROM_Write(SELF_DIAG_ERROR_LOG_ADDR,
                        (UINT32)(VOID *)&eSelfDiagTbl, sizeof(UINT16)));
    // Enable Self-Diag Recovery bit
    *((UINT16 *)((VOID *)&eSelfDiagTbl)) = 0xffff;
    UNUSED(EEPROM_Write(SELF_DIAG_BOOT_RETRY_ADDR,
                        (UINT32)(VOID *)&eSelfDiagTbl, sizeof(UINT16)));

    // Clear Shutdown state in EEPROM
    //UINT8 u1Val = 0;
    UNUSED(EEPROM_Write(SELF_DIAG_SHUTDOWN_OCCUR_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	 // Set main power error dectection count  in EEPROM
    u1Val = SELF_DIAG_POWER_DET_TIME_CNT;
    UNUSED(EEPROM_Write(SELF_DIAG_MPWR_STDN_TIME_CNT,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	
	// Set Panel power error dectection count  in EEPROM
    u1Val = SELF_DIAG_LAMP_DET_CNT;
    UNUSED(EEPROM_Write(SELF_DIAG_PPWR_STDN_TIME_CNT,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	
	// Set the default retry count in EEPROM
	u1Val = SELF_DIAG_RETRY_LIMIT_CNT;
    UNUSED(EEPROM_Write(SELF_DIAG_RETRY_LIMIT_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	 // Set  stdwn master enable byte   in EEPROM
    u1Val = SELF_DIAG_DEFALUT_STDWN_MASTER;
    UNUSED(EEPROM_Write(SELF_DIAG_STDWN_MASTER_EN,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	 // Set  stdwn Onoff clean byte  in EEPROM
    u1Val = SELF_DIAG_DEFALUT_ON_OFF_CLR_CNT;
    UNUSED(EEPROM_Write(SELF_DIAG_ON_OFF_CLR_CNT_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	// Set  is shutdown byte in eeprom
	u1Val = SELF_DIAG_DEFALUT_IS_SHUTDOWN;
    UNUSED(EEPROM_Write(SELF_DIAG_IS_SHUTDOWN_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
	// Set  is retry cnt  byte in eeprom
	u1Val = 0;
    UNUSED(EEPROM_Write(SELF_DIAG_RETRY_CNT_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    // Clear counters
    SelfDiagResetCounter();
    SelfDiagResetBootCount();   

    // Clear Self-Diag data init bit in EEPROM
    u1DataInit = SELF_DIAG_CLEAR_DATA_INIT;
    UNUSED(EEPROM_Write(SELF_DIAG_DATA_INIT_ADDR,
                        (UINT32)(VOID *)&u1DataInit, sizeof(UINT8)));
}

static void SelfDiagInit(void)
{
    UINT8 u1Val = 0;
    UINT16 u2Value = 0;
    UINT32 u4Len = sizeof(UINT16);
    PDWNC_FuncTbl *prPdwncFuncTbl;

    // Set Failure LED blink
    prPdwncFuncTbl = PDWNC_EXT_GetFunc();
	Printf("__%s__%p\n",__FUNCTION__,prPdwncFuncTbl);
    if (prPdwncFuncTbl != NULL)
    {
        // Reset T8032 blink control
        prPdwncFuncTbl->pfnSet8032uPLedBlink(0);
    }
	
    SelfDiagSetDefault();
    x_memset((void *)&_eSelfDiagShutdownLog, 0, u4Len);
    
    // Clear Shutdown state in EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_SHUTDOWN_OCCUR_ADDR,
                       (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    u1Val &= (~CUSTOM_SHUTDOWN_ERROR);
    UNUSED(EEPROM_Write(SELF_DIAG_SHUTDOWN_OCCUR_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));

#if 0
    // Clear the shutdown log
    UNUSED(EEPROM_Write(SELF_DIAG_ERROR_LOG_ADDR,
                        (UINT32)(VOID *)&_eSelfDiagShutdownLog, u4Len));
#else    
    // Get Shutdown log from EEPROM
    u4Len = sizeof(SELF_DIAG_TABLE);
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_LOG_ADDR,
                      (UINT32)(VOID *)&u2Value, u4Len));
    
    // The EEPROM is empty, try to initial the shotdown log
    if (u2Value == 0xFFFF)
    {
        UNUSED(EEPROM_Write(SELF_DIAG_ERROR_LOG_ADDR,
                            (UINT32)(VOID *)&_eSelfDiagShutdownLog, u4Len));
    }
#endif

   //Get the Shutdown Master enable byte val
    UNUSED(EEPROM_Read(SELF_DIAG_STDWN_MASTER_EN,
                      (UINT32)(VOID *)&_u1StdwnMasterEn, sizeof(UINT8)));

    
    // Get Enable bit from EEPROM
    u4Len = sizeof(SELF_DIAG_TABLE);
    UNUSED(EEPROM_Read(SELF_DIAG_DET_ENABLE_ADDR,
                      (UINT32)(VOID *)&_eSelfDiagEnable, u4Len));

    // If ALL_ERROR bit == disable, Disable all enable bit
    if (_eSelfDiagEnable.u2AllError == SELF_DIAG_DISABLE)
    {
        u2Value = 0xFFFF;
        x_memset(&_eSelfDiagEnable, u2Value, sizeof(UINT16));
    }
    
    // Get Threshold default value from EEPROM
    u4Len = sizeof(UINT16);
    GET_SELF_DIAG_THRES(SELF_DIAG_TEMP_LIMIT_ADDR, _i4ThresTemp,
                        SELF_DIAG_TEMP_VALID_MAX, u2Value, u4Len,
                        SELF_DIAG_TEMP_LIMIT_DEFAULT);
                       
    if (_i4ThresTemp & (1 << 8))
    {
        _i4ThresTemp = 0xFFFFFF00 | (_i4ThresTemp & 0x00FF);
    }
    Printf("[SelfDiagInit] Set _i4ThresTemp as %d\n", _i4ThresTemp);

    #ifndef SALVIA_MODEL
    // det RTC whether exist:0 Auto ,1 NO,2 With in EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_RTC_DET,
                       (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    if(2 == u1Val)
        _bfgDetectRTC =TRUE; //begin RTC detection
    else
        _bfgDetectRTC =FALSE; //stop RTC detection
    #else
    // det SubWoofer whether exist:0 Auto ,1 NO,2 With in EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_SUBWOOFER_DET,
                       (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    if(1 == u1Val)
        _bfgDetectSubWoofer =TRUE; //begin SubWoofer detection
    else
        _bfgDetectSubWoofer =FALSE; //stop SubWoofer detection
    #endif
    
    // det Car Battery whether exist:0 NO ,1 YES
    UNUSED(EEPROM_Read(SELF_DIAG_CAR_BATTERY_DET,
                       (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    if(1 == u1Val)
        _bfgDetectCarBattery =TRUE; //begin Car Battery
    else
        _bfgDetectCarBattery =FALSE; //stop Car Battery

    #ifndef SALVIA_MODEL
	Printf("[SelfDiagInit] _bfgDetectRTC: %d _bfgDetectCarBattery:%d DET_En:%04X\n", _bfgDetectRTC,_bfgDetectCarBattery,(UINT32)(VOID *)&_eSelfDiagEnable);	
    #else
    Printf("[SelfDiagInit] _bfgDetectSubWoofer: %d _bfgDetectCarBattery:%d DET_En:%04X\n",_bfgDetectSubWoofer,_bfgDetectCarBattery,(UINT32)(VOID *)&_eSelfDiagEnable);	
    #endif
}

//-----------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------
void SelfDiagSetModelType(UINT32 u4ModelType)
{
    UNUSED(_u4SelfDiagModelType);
    _u4SelfDiagModelType = u4ModelType;
}

BOOL SelfDiagInitStatus(void)
{
    return _bfgSelfDiagInit; 
}

void SelfDiagPause(void)
{
    _bfgSelfDiagPause = TRUE;
	if(NULL_HANDLE!=_hSelfDiagTimer)//d130701_Haibo:to protect after SelfDiag stopped
    {
	   UNUSED(x_timer_stop(_hSelfDiagTimer));
    }
}

void SelfDiagResume(void)
{
    _bfgSelfDiagPause = FALSE;
    x_thread_resume(_hSelfDiagHandle);
}

void SelfDiagStart(D_SYSTEM_STATE eState)
{
    UINT32 u4Priority = DRVCUST_OptGet(eReleaseWatchdogPriority);
    UINT32 u4BootCounter=0;
	
    if (_bfgSelfDiagForceDisable)
    {
        Printf("[SelfDiagStart] Force disable self-diag....\n");
        return;
    }

    if (_bfgSelfDiagGo == TRUE)
    {
        // Self-diag task already starts now
        Printf("[SelfDiagStart] Self-Diag task already runs....\n");

        if (_bfgSelfDiagPause)
        {
            printf("[SelfDiagStart] Self-Diag task resume....\n");
            SelfDiagResume();
        }
        return;
    }
    //d130805_Haibo:reset the global variable for car battery to avoid it uses the previous value when resume from suspend
	_bfgUseBattery=FALSE;
	_u4DetectBatteryTimeCnt=0;
	
	//D131119_Haibo:update boot count here to meet both AC and DC case
	    // Read the boot counter from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_BOOT_CNT_ADDR, 
                       (UINT32)(VOID *)&u4BootCounter, sizeof(UINT32)));
    
    // Increase the boot counter & update to EEPROM
    u4BootCounter++;
	Printf("[%s]Update boot count %d\n",__FUNCTION__,u4BootCounter);
    UNUSED(EEPROM_Write(SELF_DIAG_BOOT_CNT_ADDR, 
                        (UINT32)(VOID *)&u4BootCounter, sizeof(UINT32)));
	
    SelfDiagInit();
    if (_hSelfDiagMsgQue == NULL_HANDLE)
    {
        VERIFY(x_msg_q_create(&_hSelfDiagMsgQue, "SelfDiagMsgQue", 
                              sizeof(UINT32), 8) == OSR_OK);
        
        // Create the self-diag error handle task
        if (x_thread_create(&_hSelfDiagErrorHandle, "SelfDiagErrHandle",
                            0x800, u4Priority, SelfDiagErrorHandle,
                            0, NULL) != OSR_OK)
        {
            Printf("\nError: Create Self-Diag error handle thread fail\n");
            x_msg_q_delete(_hSelfDiagMsgQue);
            _hSelfDiagMsgQue = NULL_HANDLE;
        }
    }

    _bfgSelfDiagPause = FALSE;

//    _bfgDetectRTC =TRUE; //begin RTC detection
    
//#ifndef SONY_LCCT_24
//    _bfgDetectTouchSnr =TRUE;//begin TouchSnr detection
//#endif

    // If self-diag starts in panel power on state,
    // it is necessary to wait 5 sec. to reset the reboot counter.
    if (eState == D_SYSTEM_PANEL_ONOFF)
    {
        _bfgSelfDiagCheckBacklight = FALSE;
        _bfgSelfDiagMuteOff = FALSE;
        _u4BacklightIsNormalTime = 0;
    }

    // Create self-diag Handle timer.
    UNUSED(x_timer_create(&_hSelfDiagTimer));

	
    // Create the self-diag handle semaphore
    VERIFY(x_sema_create(&_hSelfDiagSema, X_SEMA_TYPE_BINARY,
                         X_SEMA_STATE_LOCK) == OSR_OK);
    
    // Create the self-diag handle task
    _bfgSelfDiagGo = TRUE;

#if 0
	if(_eSelfDiagEnable.u2AudError == SELF_DIAG_ENABLE)
	{
	_u4AMPErrorCount=0;
	VERIFY(GPIO_Reg((INT32)SELF_DIAG_AUD_AMP_ERROR_DET, GPIO_TYPE_INTR_FALL, SelfDiagAMPErrorDetect) >= 0);
	}
#endif
    if (x_thread_create(&_hSelfDiagHandle, "SelfDiagHandle", 0x800,
                        u4Priority, SelfDiagHandle, 0, NULL) != OSR_OK)
    {
        printf("\nError: Create error handle thread fail\n");
        x_sema_delete(_hSelfDiagSema);
        _hSelfDiagSema = NULL_HANDLE;
        _bfgSelfDiagGo = FALSE;
    }

    _bfgSelfDiagInit = TRUE;
	
}

void SelfDiagStop(D_SYSTEM_STATE eState)
{
    INT32 i4_ret;
    
    if (_bfgSelfDiagGo == FALSE)
    {
        // Self-diag task stops now
       printf("[SelfDiagStop] Self-Diag task already stops....\n");
        return;
    }
    
    SelfDiagPause();
    
    // Wait self-diag thread exit
    _bfgSelfDiagGo = FALSE;
    x_thread_resume(_hSelfDiagHandle);
    if (_hSelfDiagSema != NULL_HANDLE)
    {
        i4_ret = x_sema_lock_timeout(_hSelfDiagSema, 1500);
        if (i4_ret == OSR_OK)
        {
            x_sema_delete(_hSelfDiagSema);
            _hSelfDiagSema = NULL_HANDLE;
        }
        else if (i4_ret != OSR_TIMEOUT)
        {
            Printf("Self-Diag: lock semaphore timeout, force stop!!!\n");
        }
        else if (i4_ret != OSR_TIMEOUT)
        {
            Assert("x_sema_lock_timeout(_hSelfDiagSema, 1500)", __FILE__, (INT32)__LINE__);
        }
    }
    
    UNUSED(x_timer_delete(_hSelfDiagTimer));
    _hSelfDiagHandle = NULL_HANDLE;
    _hSelfDiagTimer = NULL_HANDLE;
    
    // If self-diag stops in panel power off state,
    // it is necessary to delete the timer handle of backlight detection.
    if (eState == D_SYSTEM_PANEL_ONOFF)
    {
        _bfgSelfDiagCheckBacklight = FALSE;
    }
}

void SelfDiagSetShutdown(D_SELF_DIAG_ERROR_TYPE eErrorType)
{
    UINT8  u1Val = 0, u1Count = 0;
    UINT32 u4LEDBlinkTime = 0;
    UINT64 u8Offset = SELF_DIAG_ERROR_CNT_ADDR;
	BOOL   _bPanelPowerError = FALSE;
    SELF_DIAG_TABLE eLastShutdownLog;
    PDWNC_FuncTbl *prPdwncFuncTbl;
    PFN_GPIO_LEDCTRL_FUNC pfnLedCtrlFunc = NULL;
	
    PRINT_TIMESTAMP_DBG_MSG("[Self Diag]", "Shutdown by error (%d)\n", 
                            (UINT32)eErrorType); 
    PRINT_TIMESTAMP_DBG_MSG("[Self Diag]", "%s\n", _StateCode2String(eErrorType)); 
    // Set Shutdown state as "Error occur"
    UNUSED(EEPROM_Read(SELF_DIAG_SHUTDOWN_OCCUR_ADDR,
                       (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    u1Val |= CUSTOM_SHUTDOWN_ERROR;
    UNUSED(EEPROM_Write(SELF_DIAG_SHUTDOWN_OCCUR_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
                        
    LOG(5, "[SelfDiagSetShutdown] error type = %d\n", eErrorType);
    // Read the last shutdown log from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_LOG_ADDR, 
                       (UINT32)(VOID *)&eLastShutdownLog, sizeof(UINT16)));
	u1Val =1;
    UNUSED(EEPROM_Write(SELF_DIAG_IS_SHUTDOWN_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
    // Self diagnostic is abnormal, record the error information
    switch (eErrorType)
    {
    case D_12V_AMP_ERROR:
        u4LEDBlinkTime = 2;
        _eSelfDiagShutdownLog.u2Amp12VError = 1;
        u8Offset += SELF_DIAG_12V_AMP_ERR_OFFSET;
		_bPanelPowerError = TRUE;
        break;
    case D_12V_SW_ERROR:
        u4LEDBlinkTime = 2;
        _eSelfDiagShutdownLog.u2SW12VError = 1;
        u8Offset += SELF_DIAG_12V_SW_ERR_OFFSET;
        break;
    case D_AUD_ERROR:
        u4LEDBlinkTime = 3;
        _eSelfDiagShutdownLog.u2AudError = 1;
        u8Offset += SELF_DIAG_AUD_ERR_OFFSET;
        break;
    case D_3V3_5V_ERROR:
        u4LEDBlinkTime = 3;
        _eSelfDiagShutdownLog.u2VDrop3V_5VError = 1;
        u8Offset += SELF_DIAG_3V3_5V_ERR_OFFSET;
        break;
    case D_PANEL_POWER_ERROR:
        u4LEDBlinkTime = 4;
        _eSelfDiagShutdownLog.u2PanelPowerError = 1;
        u8Offset += SELF_DIAG_PANEL_PWR_ERR_OFFSET;
        break;
    case D_BACKLIGHT_ERROR:
        u4LEDBlinkTime = 6;
        _eSelfDiagShutdownLog.u2BacklightError = 1;
        u8Offset += SELF_DIAG_BACKLIGHT_ERR_OFFSET;
        break;

	case D_TEMPERATURE_ERROR:
		u4LEDBlinkTime = 7;
		_eSelfDiagShutdownLog.u2TemperatureError = 1;
		u8Offset += SELF_DIAG_TEMPERATURE_ERR_OFFSET;
		break;
		
    case D_TUNER_ERROR:
        u4LEDBlinkTime = 3;
        _eSelfDiagShutdownLog.u2TunerError = 1;
        u8Offset += SELF_DIAG_TUNER_ERR_OFFSET;
        break;
	case D_TCON_ERROR:
		u4LEDBlinkTime = 5;
		_eSelfDiagShutdownLog.u2TconError = 1;
		u8Offset += SELF_DIAG_TCON_ERR_OFFSET;
		break;

    case D_PANEL_NVM_ERROR:
        u4LEDBlinkTime = 5;
        _eSelfDiagShutdownLog.u2PanelNIMError = 1;
        u8Offset += SELF_DIAG_PANEL_NVM_ERR_OFFSET;
        break;
    #ifdef SALVIA_MODEL    
	case D_SUBWOOFER_ERROR:
        u4LEDBlinkTime = 7; //spec:blink x 7
        _eSelfDiagShutdownLog.u2SubWooferError = 1;
        u8Offset += SELF_DIAG_SUBWOOFER_ERR_OFFSET;
        break; 
    #else
    case D_RTC_ERROR:
        u4LEDBlinkTime = 5;
        _eSelfDiagShutdownLog.u2RtcError = 1;
        u8Offset += SELF_DIAG_RTC_ERR_OFFSET;
        break; 
    #endif
    case D_TOUCHSNR_ERR:
        u4LEDBlinkTime = 5;
        _eSelfDiagShutdownLog.u2TouchSnrError = 1;
        u8Offset += SELF_DIAG_TOUCHSNR_ERR_OFFSET;
        break;
	default:
		break;

    }
    
    // Write the shutdown log to EEPROM
    UNUSED(EEPROM_Write(SELF_DIAG_ERROR_LOG_ADDR, 
                        (UINT32)(VOID *)&_eSelfDiagShutdownLog, sizeof(UINT16)));

    // Read the shutdown counter from EEPROM
    UNUSED(EEPROM_Read(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));

    // Increase the shutdown counter & update to EEPROM
    u1Count++;
    UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));

    //wistron require that 12V_AMP_ERROR & PANEL_POWER_ERROR should be record at the same time  /xiaojiao.li
	if (_bPanelPowerError)
    {
	u8Offset = SELF_DIAG_ERROR_CNT_ADDR;
	u8Offset += SELF_DIAG_PANEL_PWR_ERR_OFFSET;
    UNUSED(EEPROM_Read(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
    u1Count++;
    UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
	}


    // Compare the current shutdown log and the previous one
    if (x_memcmp((void *)&eLastShutdownLog, (void *)&_eSelfDiagShutdownLog,
                 sizeof(UINT16)) == 0)
    {
        // There is the same error
        // Read the retry counter from EEPROM
        UNUSED(EEPROM_Read(SELF_DIAG_RETRY_CNT_ADDR,
                           (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
        // Increase the retry counter
        u1Count++;     
    }
    else
    {
        // Recovery successes
        // Reset the retry counter
        u1Count = 0;
    }
    
    // Update the retry counter to EEPROM
    UNUSED(EEPROM_Write(SELF_DIAG_RETRY_CNT_ADDR,
                        (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
        
    // Enter standby 
    UNUSED(DRVCUST_InitQuery(eGpioLedCtrlFunc, (UINT32 *)(void *)&pfnLedCtrlFunc));
    if (pfnLedCtrlFunc != NULL)//d130705_Haibo:power off green led 
    {
        pfnLedCtrlFunc(eLedSystemPowerOff);
    }
    // Set Failure LED blink
    prPdwncFuncTbl = PDWNC_EXT_GetFunc();
    if (prPdwncFuncTbl != NULL)
    {
	    Printf("blink the LED %d times\n",u4LEDBlinkTime);
        prPdwncFuncTbl->pfnSet8032uPLedBlink(u4LEDBlinkTime);
    }

	PDWNC_Custom_EnterNextState(STATE_STANDBY);
    PDWNC_EnterPowerDown((WAK_IRRX | WAK_UNOR), PDWNC_POWER_DOWN_IRRC);
}

UINT32 SelfDiagGetBootCounter(UINT32 *pu4BootCounter)
{
    UINT32 u4BootCounter = 0;
    
    // Read the boot counter from EEPROM
    if (EEPROM_Read(SELF_DIAG_BOOT_CNT_ADDR, 
                    (UINT32)(VOID *)&u4BootCounter, sizeof(UINT32)) == 0)
    {
        *pu4BootCounter = u4BootCounter;
        return (SELF_DIAG_OK);
    }
    else
    {
        return (SELF_DIAG_FAIL);
    }
}

UINT32 SelfDiagGetCounter(D_SELF_DIAG_STATE *prErrorInfo)
{
    UINT32 u4Ret = SELF_DIAG_OK;
    UINT32 u4Value = 0;
    UINT64 u8Offset = SELF_DIAG_ERROR_CNT_ADDR;
    SELF_DIAG_TABLE eShutdownLog;

    x_memset(&eShutdownLog, 0, sizeof(SELF_DIAG_TABLE));

    // Read the shutdown log from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_LOG_ADDR,
                       (UINT32)(VOID *)&eShutdownLog, sizeof(UINT16)));

    // Get the EEPROM offset of the shutdown counter
    switch (prErrorInfo->eErrorType)
    {
    case D_12V_AMP_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2Amp12VError;
        u8Offset += SELF_DIAG_12V_AMP_ERR_OFFSET;
        break;
    case D_12V_SW_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2SW12VError;
        u8Offset += SELF_DIAG_12V_SW_ERR_OFFSET;
        break;
#if 0		
    case D_24V_PWR_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2SW12VError;
        u8Offset += SELF_DIAG_24V_PWR_ERR_OFFSET;
        break;
#endif		
    case D_AUD_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2AudError;
        u8Offset += SELF_DIAG_AUD_ERR_OFFSET;
        break;
    case D_3V3_5V_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2VDrop3V_5VError;
        u8Offset += SELF_DIAG_3V3_5V_ERR_OFFSET;
        break;
	case D_TUNER_ERROR:
		prErrorInfo->bErrorOccur = eShutdownLog.u2TunerError;
		u8Offset += SELF_DIAG_TUNER_ERR_OFFSET;
		break;
    case D_PANEL_POWER_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2PanelPowerError;
        u8Offset += SELF_DIAG_PANEL_PWR_ERR_OFFSET;
        break;
    case D_BACKLIGHT_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2BacklightError;
        u8Offset += SELF_DIAG_BACKLIGHT_ERR_OFFSET;
        break;
    case D_TEMPERATURE_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2TemperatureError;
        u8Offset += SELF_DIAG_TEMPERATURE_ERR_OFFSET;
        break;
	case D_TCON_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2TconError;
        u8Offset += SELF_DIAG_TCON_ERR_OFFSET;
        break;
	case D_PANEL_NVM_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2PanelNIMError;
        u8Offset += SELF_DIAG_PANEL_NVM_ERR_OFFSET;
        break;
    #ifdef SALVIA_MODEL      
    case D_SUBWOOFER_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2SubWooferError;
        u8Offset += SELF_DIAG_SUBWOOFER_ERR_OFFSET;
        break;
    #else
    case D_RTC_ERROR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2RtcError;
        u8Offset += SELF_DIAG_RTC_ERR_OFFSET;
        break;
    #endif
	case D_TOUCHSNR_ERR:
        prErrorInfo->bErrorOccur = eShutdownLog.u2TouchSnrError;
        u8Offset += SELF_DIAG_TOUCHSNR_ERR_OFFSET;
        break;

    default:
        u4Ret = SELF_DIAG_FAIL;
        break;
    }

    // Read the shutdown counter from EEPROM
    if (u4Ret == SELF_DIAG_FAIL)
    {
        prErrorInfo->bErrorOccur = 0;
        prErrorInfo->u4ErrorCnt = 0;
    }
    else
    {
        UNUSED(EEPROM_Read(u8Offset, (UINT32)(VOID *)&u4Value, sizeof(UINT8)));
        prErrorInfo->u4ErrorCnt = u4Value;
    }
    return u4Ret;
}

UINT32 SelfDiagUpdateOnOffCount(void)
{
    // Update OnOff counter
    UNUSED(EEPROM_Write(SELF_DIAG_ON_OFF_CNT_ADDR,
                        (UINT32)(VOID *)&_u1SelfDiagOnOffCnt, sizeof(UINT8)));
    return SELF_DIAG_OK;
}

UINT32 SelfDiagResetCounter(void)
{
    UINT8  u1Data[D_ALL_ERROR];
    UINT32 u4DataLen = D_ALL_ERROR * sizeof(UINT8);
    UINT64 u8Offset = SELF_DIAG_ERROR_CNT_ADDR;

    x_memset(u1Data, 0, u4DataLen);
    UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)u1Data, u4DataLen));
    u8Offset = SELF_DIAG_ON_OFF_CNT_ADDR;
    UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)u1Data, 2));
    return SELF_DIAG_OK;
}

UINT32 SelfDiagResetBootCount(void)
{
    UINT32 u4Count = 0;
    UINT64 u8Offset = SELF_DIAG_BOOT_CNT_ADDR;

    UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u4Count, sizeof(u4Count)));
    return SELF_DIAG_OK;
}

void SelfDiagUpdateBacklightState(BOOL bIsBackltOn)
{
    CRIT_STATE_T rCritState;
    
    rCritState = x_crit_start();
    _bfgSelfDiagBackltOn = bIsBackltOn;
    x_crit_end(rCritState);
}
//#ifdef SONY_LCCT_24
UINT8 SelfDiagGetBatteryMode(void)
{
    return _u1Battery_mode;
}
//#endif

