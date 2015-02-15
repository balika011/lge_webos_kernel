
#ifndef CUSTOM_SELF_DIAG_H
#define CUSTOM_SELF_DIAG_H

//#include "d_Sipadan_cust.h"
#ifdef TARGET_SPEC_DRV_CUST_CUSTOMIZED_HDR_FILE
#include TARGET_SPEC_DRV_CUST_CUSTOMIZED_HDR_FILE
#endif



//---------------------------------------------------------------------------
// Macro
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

#define SELF_DIAG_OK                            (UINT32)0
#define SELF_DIAG_FAIL                          (UINT32)(-1)
#define SELF_DIAG_ENABLE                        (0)
#define SELF_DIAG_DISABLE                       (1)

// Self Diagnostic definition
#define SELF_DIAG_12V_AMP_DROP_DET              0xffffffff//TCON1
#define SELF_DIAG_12V_SW_DROP_DET               ADIN(5)
#define SELF_DIAG_V_DROP_DET                    0xffffffff//ADIN(0)    // ADIN0
#define SELF_DIAG_PANEL_V_DROP_DET              ADIN(4)
#define SELF_DIAG_TUNER_PWR_DROP_DET            0xffffffff  // VGA_SCL
#ifdef CC_SOEM_BOARD
#define SELF_DIAG_AUD_AMP_ERROR_DET             GPIO(91)//TCON11 A10(X_AUDIO_MON)
#ifdef SALVIA_MODEL
#define SELF_DIAG_AUD_SUBWOOFER_ERROR_DET       GPIO(88)//TCON8 A11(X_AUDIO_MON1)
#endif
#else
#define SELF_DIAG_AUD_AMP_ERROR_DET             0xffffffff//GPIO(63)    // TCON 1
#ifdef SALVIA_MODEL
#define SELF_DIAG_AUD_SUBWOOFER_ERROR_DET       0xffffffff
#endif
#endif
#define SELF_DIAG_TCON_RDY_DET                  0xffffffff    // GPIO 0
#define SELF_DIAG_LAMP_DET                      GPIO(78)//BL ERR


//#endif

// Threshold & default value
#define SELF_DIAG_TEMP_LIMIT_DEFAULT            0xA0        // 160 (80C)

#define SELF_DIAG_SERVO_VALID_MAX               255
#define SELF_DIAG_TEMP_VALID_MAX                0x1FF
#define SELF_DIAG_COUNTER_VALID_MAX             0xFFFFFFFF
#define SELF_DIAG_ON_OFF_RETRY_LIMIT            0x3

#define SELF_DIAG_12V_SW_DROP_VAL_MIN           160
#define SELF_DIAG_12V_SW_DROP_VAL_MAX           213

#define SELF_DIAG_BATTERY_VAL_MIN           93
#define SELF_DIAG_BATTERY_VAL_MAX           140
#define SELF_DIAG_BATTERY_VAL_LOW            106   
enum{
	BATTERY_NORMAL,
	BATTERY_LOW,
	BATTERY_NONE,
};

#define SELF_DIAG_PANEL_POWER_DROP_VAL_MIN           160//46
#define SELF_DIAG_PANEL_POWER_DROP_VAL_MAX           226//57

#define SELF_DIAG_22_INCH_PANEL_POWER_DROP_VAL_MIN             49//46
#define SELF_DIAG_22_INCH_PANEL_POWER_DROP_VAL_MAX           67//57
#define SELF_DIAG_BL_VALID_MAX_VOLTAGE          80


#define SELF_DIAG_LAMP_VAL_MAX                      209

// Detect time period definition
#define SELF_DIAG_DETECT_PERIOD                 100         // 100 ms

#define SELF_DIAG_POWER_DET_TIME_CNT            10         // 900 ms
#define SELF_DIAG_MAIN_DET_TIME                 3000        // 3 ses
#define SELF_DIAG_TUNER_DET_TIME                3000        // 3 sec
#define SELF_DIAG_INVERTER_DET_TIME             900         // 900 ms
#define SELF_DIAG_LAMP_DET_CNT                  10        // 3 sec
#define SELF_DIAG_TCON_DET_TIME                 3000
#define SELF_DIAG_BL_DELAY_TIME                 400         // 400 ms
#define SELF_DIAG_BL_RESET_TIME                 5000        // 5 sec
#define SELF_DIAG_RTC_DET_TIME                  300        //300ms
#ifndef SONY_LCCT_24
#define SELF_DIAG_TOUCHSNR_DET_TIME             300        //300ms
#endif


// EEPROM information
#ifdef CC_ANDROID_TWO_WORLDS
#define SELF_DIAG_BASE_ADDR                     (0x9F60)
#else
#define SELF_DIAG_BASE_ADDR                     (0x2F60)
#endif

#define SELF_DIAG_DATA_INIT_ADDR                (SELF_DIAG_BASE_ADDR)
#define SELF_DIAG_SHUTDOWN_OCCUR_ADDR           (SELF_DIAG_BASE_ADDR + 0x01)
#define SELF_DIAG_DET_ENABLE_ADDR               (SELF_DIAG_BASE_ADDR + 0x40)
#define SELF_DIAG_ERROR_LOG_ADDR                (SELF_DIAG_BASE_ADDR + 0x42)
#define SELF_DIAG_BOOT_RETRY_ADDR               (SELF_DIAG_BASE_ADDR + 0x44)
#define SELF_DIAG_ERROR_CNT_ADDR                (SELF_DIAG_BASE_ADDR + 0x48)
#define SELF_DIAG_ON_OFF_CNT_ADDR               (SELF_DIAG_BASE_ADDR + 0x5A)
#define SELF_DIAG_RETRY_CNT_ADDR                (SELF_DIAG_BASE_ADDR + 0x5B)
#define SELF_DIAG_BOOT_CNT_ADDR                 (SELF_DIAG_BASE_ADDR + 0x5C)

//0x1f40
#define SELF_DIAG_12V_AMP_ERR_OFFSET            D_12V_AMP_ERROR//0
#define SELF_DIAG_12V_SW_ERR_OFFSET             D_12V_SW_ERROR//1
#define SELF_DIAG_24V_PWR_ERR_OFFSET            D_24V_PWR_ERROR//2
#define SELF_DIAG_AUD_ERR_OFFSET                D_AUD_ERROR//3
#define SELF_DIAG_3V3_5V_ERR_OFFSET             D_3V3_5V_ERROR//4
#define SELF_DIAG_PANEL_PWR_ERR_OFFSET          D_PANEL_POWER_ERROR//5
#define SELF_DIAG_BACKLIGHT_ERR_OFFSET          D_BACKLIGHT_ERROR//6
#define SELF_DIAG_TEMPERATURE_ERR_OFFSET        D_TEMPERATURE_ERROR//7
//0x1f41
#define SELF_DIAG_TCON_ERR_OFFSET               D_TCON_ERROR//8
#define SELF_DIAG_BALANCE_ERR_OFFSET            D_BALANCER_ERROR//9
#define SELF_DIAG_TUNER_ERR_OFFSET              D_TUNER_ERROR//10
#define SELF_DIAG_PANEL_NVM_ERR_OFFSET          D_PANEL_NVM_ERROR // 11
#ifdef SALVIA_MODEL
#define SELF_DIAG_SUBWOOFER_ERR_OFFSET          D_SUBWOOFER_ERROR   //12
#else
#define SELF_DIAG_RTC_ERR_OFFSET                D_RTC_ERROR   //12
#endif
#define SELF_DIAG_TOUCHSNR_ERR_OFFSET           D_TOUCHSNR_ERR  //13
#define SELF_DIAG_CAR_BATTERY_ERROR_OFFSET      D_CAR_BATTERY

#define PANEL_ID_FUNTION_BASE_ADDR              (SELF_DIAG_BASE_ADDR+0x70)
#define PANEL_ID_STORE_COUNT                    (PANEL_ID_FUNTION_BASE_ADDR)
#define PANEL_ID_PANEL_MODE                     (PANEL_ID_FUNTION_BASE_ADDR+ 0x01)
#define PANEL_ID_READ_ADDR                      (PANEL_ID_FUNTION_BASE_ADDR+ 0x02)
#define WakeupReasonFlag                        (PANEL_ID_FUNTION_BASE_ADDR+ 0x04)



enum{
	PANEL_LESS_MODE,
	PANEL_MODE,
	PANEL_DISABLE_LESS_MODE,
	PANEL_DET_DIABLE,
};

enum{
	PANEL_EDID_READ_0XE0,
	PANEL_EDID_READ_0X00E0,
};




#define ATV_60HZ_PANEL_SIF_BUS         0 //d140528_Haibo:For Salvia.Panel IIc use OSCL0/OSDA0
#define ATV_60HZ_PANEL_SIF_CLK_DIV         0x100
#define ATV_60HZ_PANEL_NVM_ADDRESS         0xAA
#define ATV_60HZ_PANEL_ID_ON_NVM_OFFSET    0xE0
#define ATV_PANEL_ID_STRING_LEN            16




#define SELF_DIAG_FUNCTION_ENABLE(x)                     (1U << (x)) 

//#ifndef SALVIA_MODEL
//#if ((STELLAR_MODEL==STELLAR_PAA) || (STELLAR_MODEL==STELLAR_PAD_VIETNAM) || (STELLAR_MODEL==STELLAR_PAD))//d130711_Haibo:PAA & PAD has Car Battery
//#endif
#ifdef SALVIA_MODEL
//#if ((SALVIA_MODEL==SALVIA_PAA) || (SALVIA_MODEL==SALVIA_PAD_VIETNAM) || (SALVIA_MODEL==SALVIA_PAD))
#if (SALVIA_MODEL==SALVIA_PAA)
#define DEFAULT_SELF_DIAG_ENABLED_ITEMS         (                                              \
                                                    SELF_DIAG_FUNCTION_ENABLE(D_12V_SW_ERROR)		|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_AUD_ERROR)			|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_BACKLIGHT_ERROR)	|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_PANEL_POWER_ERROR)	|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_PANEL_NVM_ERROR)	|   \
                                                    SELF_DIAG_FUNCTION_ENABLE(D_SUBWOOFER_ERROR)	|   \
                                                    SELF_DIAG_FUNCTION_ENABLE(D_CAR_BATTERY)        |   \
                                                    0)

#else
#define DEFAULT_SELF_DIAG_ENABLED_ITEMS         (                                              \
                                                    SELF_DIAG_FUNCTION_ENABLE(D_12V_SW_ERROR)		|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_AUD_ERROR)			|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_BACKLIGHT_ERROR)	|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_PANEL_POWER_ERROR)	|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_PANEL_NVM_ERROR)	|   \
                                                    SELF_DIAG_FUNCTION_ENABLE(D_CAR_BATTERY)        |   \
                                                    0)
#endif
#else
#define DEFAULT_SELF_DIAG_ENABLED_ITEMS         (                                              \
                                                    SELF_DIAG_FUNCTION_ENABLE(D_12V_SW_ERROR)		|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_AUD_ERROR)			|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_BACKLIGHT_ERROR)	|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_PANEL_POWER_ERROR)	|	\
                                                    SELF_DIAG_FUNCTION_ENABLE(D_PANEL_NVM_ERROR)	|   \
                                                    0)
#endif													
//#endif

#define SELF_DIAG_TEMP_LIMIT_ADDR               (SELF_DIAG_BASE_ADDR + 0x60)
#define SELF_DIAG_RETRY_LIMIT_ADDR              (SELF_DIAG_BASE_ADDR + 0x58)
#define SELF_DIAG_RETRY_LIMIT_CNT                2

#define SELF_DIAG_MPWR_STDN_TIME_CNT            (SELF_DIAG_BASE_ADDR + 0x62) 
#define SELF_DIAG_STDWN_MASTER_EN               (SELF_DIAG_BASE_ADDR + 0x63) 
#define SELF_DIAG_DEFALUT_STDWN_MASTER          1
#define SELF_DIAG_ON_OFF_CLR_CNT_ADDR           (SELF_DIAG_BASE_ADDR + 0x64)
#define SELF_DIAG_DEFALUT_ON_OFF_CLR_CNT          60
#define SELF_DIAG_IS_SHUTDOWN_ADDR              (SELF_DIAG_BASE_ADDR + 0x65)
#define SELF_DIAG_DEFALUT_IS_SHUTDOWN           0
#define SELF_DIAG_AC_DET_STANDBY                (SELF_DIAG_BASE_ADDR + 0x66)
#define SELF_DIAG_PPWR_STDN_TIME_CNT            (SELF_DIAG_BASE_ADDR + 0x67)
#ifndef SALVIA_MODEL
#define SELF_DIAG_RTC_DET                       0x6c3
#else
#define SELF_DIAG_SUBWOOFER_DET                 0x6ca
#endif
#define SELF_DIAG_CAR_BATTERY_DET               0x6c6

#define SELF_DIAG_EEPROM_SIZE                   64

// LOAD default data
#define SELF_DIAG_LOAD_DEFAULT                  (1 << 0)
#define SELF_DIAG_CLEAR_DATA_INIT               (0)

typedef enum
{
    SELF_DIAG_ACT_REBOOT,
    SELF_DIAG_ACT_PANEL_ONOFF,
    SELF_DIAG_ACT_SHUTDOWN,
    SELF_DIAG_ACT_START,
    SELF_DIAG_ACT_STOP
} SELF_DIAG_ERR_ACTION;

//-----------------------------------------------------------------------------
// The order of the following struct members should be sync. to 'Shutdown Log' on
// Self-diagnostical eeprom layout 
// If new monitor item were added, the item should be next to u2AllError and u2Reverved should be decrease one
//-----------------------------------------------------------------------------
#if 1
typedef struct _SELF_DIAG_TABLE_T
{
    
    UINT16  u2Amp12VError : 1;          //MSB of low byte of Shutdown Log
    UINT16  u2SW12VError : 1;           //Bit6 of low byte of Shutdown Log   
    UINT16  u224VError : 1;             //Bit5 of low byte of Shutdown Log
    UINT16  u2AudError : 1;             //Bit4 of low byte of Shutdown Log
    UINT16  u2VDrop3V_5VError : 1;      //Bit3 of low byte of Shutdown Log
    UINT16  u2PanelPowerError : 1;      //Bit2 of low byte of Shutdown Log 
    UINT16  u2BacklightError : 1;       //Bit1 of low byte of Shutdown Log
    UINT16  u2TemperatureError : 1;     //Bit0 of low byte of Shutdown Log  
    UINT16  u2BalancerError : 1;        //Bit6 of high byte of Shutdown Log
    UINT16  u2TconError : 1;            //MSB of high byte of Shutdown Log
    UINT16  u2PanelNIMError : 1;
    UINT16  u2TunerError : 1;           //Bit5 of high byte of Shutdown Log
    #ifdef SALVIA_MODEL
    UINT16  u2SubWooferError : 1;           //Bit5 of high byte of Shutdown Log
    #else
    UINT16  u2RtcError : 1;           //Bit5 of high byte of Shutdown Log
    #endif
    UINT16  u2TouchSnrError : 1;             //Bit3~0 of high byte of Shutdown Log
    UINT16  u2CarBatteryError : 1;             //Bit3~0 of high byte of Shutdown Log
    UINT16  u2AllError : 1;             //Bit4 of high byte of Shutdown Log
} SELF_DIAG_TABLE;
#else
typedef struct _SELF_DIAG_TABLE_T
{
    
    UINT16  u2SW12VError : 1;           //Bit6 of low byte of Shutdown Log
    UINT16  u2PanelPowerError : 1;      //Bit2 of low byte of Shutdown Log
    UINT16  u2BacklightError : 1;       //Bit1 of low byte of Shutdown Log
	UINT16	u2TemperatureError : 1; 	//Bit0 of low byte of Shutdown Log
    UINT16  u2AudError : 1;             //Bit4 of low byte of Shutdown Log
    UINT16  u2BalancerError : 1;        //Bit6 of high byte of Shutdown Log
    UINT16  u2VDrop3V_5VError : 1;      //Bit3 of low byte of Shutdown Log
    UINT16  u2TconError : 1;            //MSB of high byte of Shutdown Log
    UINT16  u2PanelNIMError : 1;
	UINT16  u224VError : 1;             //Bit5 of low byte of Shutdown Log
    UINT16  u2Amp12VError : 1;          //MSB of low byte of Shutdown Log
    UINT16  u2Reverved : 3;             //Bit3~0 of high byte of Shutdown Log
    UINT16  u2AllError : 1;             //Bit4 of high byte of Shutdown Log
    UINT16  u2TunerError : 1;           //Bit5 of high byte of Shutdown Log
} SELF_DIAG_TABLE;
#endif
//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
void SelfDiagSetModelType(UINT32 u4ModelType);
void SelfDiagSetShutdown(D_SELF_DIAG_ERROR_TYPE eErrorType);
void SelfDiagStart(D_SYSTEM_STATE eState);
void SelfDiagStop(D_SYSTEM_STATE eState);
void SelfDiagPause(void);
void SelfDiagResume(void);
void SelfDiagUpdateBacklightState(BOOL bIsBackltOn);
BOOL SelfDiagInitStatus(void);
UINT32 SelfDiagGetCounter(D_SELF_DIAG_STATE *prErrorInfo);
UINT32 SelfDiagUpdateOnOffCount(void);
UINT32 SelfDiagResetCounter(void);
UINT32 SelfDiagResetBootCount(void);


//#else
//#error "reinclude SelfDiag.h..."
#endif

