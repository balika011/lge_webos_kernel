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

/*****************************************************************************
*  Uart : Diagnostic command
*****************************************************************************/
#define CC_CLI
#include "x_lint.h"

LINT_SAVE_AND_DISABLE

#ifdef __MW_CLI_DEF__
#include "x_mid_cli.h"
#else
#include "x_drv_cli.h"
#endif

#include "x_os.h"
#include "x_printf.h"
#include "x_stl_lib.h"

#include "SelfDiag_debug.h"
#include "x_util.h"
#include "x_hal_5381.h"
#include "eeprom_if.h"
#include "pdwnc_if.h"
#include "drv_pwm.h"
#include "CustomCtrl.h"
#include "SelfDiag.h"

#ifdef CC_CLI

/******************************************************************************
* Declare the macros and reglist for RegTest functions
******************************************************************************/
#define CHECK_SELT_DIAG_STATE       (_bfgSelfDiagGo)

#define CHECK_SELT_DIAG_STOP                                                \
    if (CHECK_SELT_DIAG_STATE)                                              \
    {                                                                       \
        Printf("Self Diagnostic is running\n");                             \
        Printf("Please stop Self Diagnostic first\n");                      \
        return 0;                                                           \
    }
/*
SelfDiag.h
#define SELF_DIAG_12V_AMP_ERR_OFFSET            0
#define SELF_DIAG_12V_SW_ERR_OFFSET             1
#define SELF_DIAG_24V_PWR_ERR_OFFSET            2
#define SELF_DIAG_AUD_ERR_OFFSET                3
#define SELF_DIAG_3V3_5V_ERR_OFFSET             4
#define SELF_DIAG_PANEL_PWR_ERR_OFFSET          5
#define SELF_DIAG_BACKLIGHT_ERR_OFFSET          6
#define SELF_DIAG_TEMPERATURE_ERR_OFFSET        7
#define SELF_DIAG_TCON_ERR_OFFSET              8
#define SELF_DIAG_BALANCE_ERR_OFFSET            9
#define SELF_DIAG_TUNER_ERR_OFFSET              10
*/

#define LOG_ERROR_TYPE  Printf("<Error type> 0 - Main Power Error\n");         \
                        Printf("             1 - DC Alert Error\n");          \
                        Printf("             2 - Audio Error\n");           \
                        Printf("             3 - Tuner Demod Error\n");          \
                        Printf("             4 - LD Error\n");       \
                        Printf("             5 - VLED Error\n");     \      
                        Printf("             6 - Tcon Error\n");     \
                        Printf("             7 - Panel ID Error\n");       \
                        Printf("             8 - Backlight Error\n");           \
                        Printf("             9 - Temperature Error\n");     \
                        Printf("             10 -SW Error\n");       \
                        Printf("             11 -4K BE Out Error\n");           \
                        Printf("             12 -4K BE Error\n");           \
                        Printf("             13 -Hdmi Eq Error\n");           \
                        Printf("             14 -RSV00 Error\n");           \
                        Printf("             15 -RSV01 Error\n");           \
                        Printf("             16 -RSV02 Error\n");           \
                        Printf("             17 -RSV03 Error\n");           \
                        Printf("             18 -RSV04 Error\n");           \
                        Printf("             19 -RSV05 Error\n");           \
                        Printf("             20 -RSV06 Error\n");           \
                        Printf("             21 -RSV07 Error\n");           \
                        Printf("             22 -RSV08 Error\n");           \
                        Printf("             23 -RSV09 Error\n");           \
                        Printf("             24 -RSV10 Error\n");           \
                        Printf("             25 -RSV11 Error\n");           \
                        Printf("             26 -RSV12 Error\n");           \
                        Printf("             27 -RSV13 Error\n");           \
                        Printf("             28 -RSV14 Error\n");           \
                        Printf("             29 -RSV15 Error\n");           \
                        Printf("             30 -RSV16 Error\n");           \ 
                        Printf("             31 - All Error\n");

#define DUMP_ERROR_TYPE(SelfDiagTbl)                                        \
    Printf(" 0 - Main Power Error      : %d\n", SelfDiagTbl.u4MainPowerError);    \
    Printf(" 1 - DC Alert Error       : %d\n", SelfDiagTbl.u4DcAlertError);     \
    Printf(" 2 - Audio Error          : %d\n", SelfDiagTbl.u4AudError);       \
    Printf(" 3 - Tuner Demod Error        : %d\n", SelfDiagTbl.u4TuDemodError);       \
    Printf(" 4 - LD Error       : %d\n", SelfDiagTbl.u4LdError);\
    Printf(" 5 - VLED Error  : %d\n", SelfDiagTbl.u4VledError);\
    Printf(" 6 - Tcon Error    : %d\n", SelfDiagTbl.u4TconError); \
    Printf(" 7 - Panel ID Error  : %d\n", SelfDiagTbl.u4PanelIdError);\
    Printf(" 8 - Backlight Error     : %d\n", SelfDiagTbl.u4BacklightError);  \
    Printf(" 9 - Temperature Error        : %d\n", SelfDiagTbl.u4TemperatureError);      \
	Printf(" 10 - SW Error	: %d\n", SelfDiagTbl.u4SWError);	\
    Printf(" 11 - 4K BE Out Error       : %d\n", SelfDiagTbl.u44kBeOutError);     \
    Printf(" 12 - 4K BE Error		: %d\n", SelfDiagTbl.u44kBeError); 	\
    Printf(" 13 - Hdmi Eq Error		: %d\n", SelfDiagTbl.u4HdmiEqError); 	\
    Printf(" 14 - FRCTC I2C Error		: %d\n", SelfDiagTbl.u4FRCTCI2CError); 	\
    Printf(" 15 - RSV01 Error		: %d\n", SelfDiagTbl.u4Rsv01Error); 	\
    Printf(" 16 - RSV02 Error		: %d\n", SelfDiagTbl.u4Rsv02Error); 	\
    Printf(" 17 - RSV03 Error		: %d\n", SelfDiagTbl.u4Rsv03Error); 	\
    Printf(" 18 - RSV04 Error		: %d\n", SelfDiagTbl.u4Rsv04Error); 	\
    Printf(" 19 - RSV05 Error		: %d\n", SelfDiagTbl.u4Rsv05Error); 	\
    Printf(" 20 - RSV06 Error		: %d\n", SelfDiagTbl.u4Rsv06Error); 	\
    Printf(" 21 - RSV07 Error		: %d\n", SelfDiagTbl.u4Rsv07Error); 	\
    Printf(" 22 - RSV08 Error		: %d\n", SelfDiagTbl.u4Rsv08Error); 	\
    Printf(" 23 - RSV09 Error		: %d\n", SelfDiagTbl.u4Rsv09Error); 	\
    Printf(" 24 - RSV10 Error		: %d\n", SelfDiagTbl.u4Rsv10Error); 	\
    Printf(" 25 - RSV11 Error		: %d\n", SelfDiagTbl.u4Rsv11Error); 	\
    Printf(" 26 - RSV12 Error		: %d\n", SelfDiagTbl.u4Rsv12Error); 	\
    Printf(" 27 - RSV13 Error		: %d\n", SelfDiagTbl.u4Rsv13Error); 	\
    Printf(" 28 - RSV14 Error		: %d\n", SelfDiagTbl.u4Rsv14Error); 	\
    Printf(" 29 - RSV15 Error		: %d\n", SelfDiagTbl.u4Rsv15Error); 	\
    Printf(" 30 - RSV16 Error		: %d\n", SelfDiagTbl.u4Rsv16Error); 	\
    Printf(" 31 - All Error		: %d\n", SelfDiagTbl.u4AllError); 
    
#define SET_ERROR_BIT(SelfDiagTbl, ErrorType, SetBit)                       \
    switch (ErrorType)                                                      \
    {                                                                       \
    case D_MAIN_POWER_ERROR:                                                   \
        SelfDiagTbl.u4MainPowerError = SetBit;         break;                  \
    case D_DC_ALERT_ERROR:                                                    \
        SelfDiagTbl.u4DcAlertError = SetBit;          break;                  \
    case D_AUD_ERROR:                                                   \
        SelfDiagTbl.u4AudError = SetBit;            break;                  \
    case D_TU_DEMOD_ERROR:                                                       \
        SelfDiagTbl.u4TuDemodError = SetBit;            break;                  \
    case D_LD_ERROR:                                                    \
        SelfDiagTbl.u4LdError = SetBit;     break;                  \
    case D_VLED_ERROR:                                               \
        SelfDiagTbl.u4VledError = SetBit;     break;                  \
    case D_TCON_ERROR:                                                 \
        SelfDiagTbl.u4TconError = SetBit;      break;                  \
    case D_PANEL_ID_ERROR:                                               \
        SelfDiagTbl.u4PanelIdError = SetBit;    break;                  \
	case D_BACKLIGHT_ERROR : 													\
		SelfDiagTbl.u4BacklightError = SetBit;	        break; 				    \
	case D_TEMPERATURE_ERROR :												\
		SelfDiagTbl.u4TemperatureError = SetBit;		break;					\
	case D_SW_ERROR :												\
		SelfDiagTbl.u4SWError = SetBit;		break;					\
    case D_4K_BE_OUT_ERROR:                                                    \
        SelfDiagTbl.u44kBeOutError = SetBit;    break;                        \
    case D_4K_BE_ERROR:                                                    \
        SelfDiagTbl.u44kBeError = SetBit;    break;                        \
    case D_HDMI_EQ_ERROR:                                                    \
        SelfDiagTbl.u4HdmiEqError = SetBit;    break;                        \
    case D_FRCTC_I2C_ERROR:                                                    \
        SelfDiagTbl.u4FRCTCI2CError = SetBit;    break;                        \    
	default :												\
		break;	 			\
    }

#define DUMP_ERROR_COUNTER(ShutdownCounter)                                 \
    Printf("Main Power Shutdown Counter      : %d\n", ShutdownCounter[0]);     \
    Printf("DC Alert Shutdown Counter       : %d\n", ShutdownCounter[1]);     \
    Printf("Audio Shutdown Counter         : %d\n", ShutdownCounter[2]);     \
    Printf("Tuner Demod Shutdown Counter        : %d\n", ShutdownCounter[3]);     \
    Printf("LD Shutdown Counter       : %d\n", ShutdownCounter[4]);     \
    Printf("VLED Shutdown Counter  : %d\n", ShutdownCounter[5]);     \
    Printf("Tcon Shutdown Counter    : %d\n", ShutdownCounter[6]);     \
	Printf("Panel ID Shutdown Counter	  : %d\n", ShutdownCounter[7]);	\
    Printf("Backlight Shutdown Counter        : %d\n", ShutdownCounter[8]);    \
    Printf("Temperature Shutdown Counter    : %d\n", ShutdownCounter[9]);    \
    Printf("SW Shutdown Counter    : %d\n", ShutdownCounter[10]);    \
    Printf("4K BE Out Shutdown Counter    : %d\n", ShutdownCounter[11]);    \
    Printf("4K BE Shutdown Counter    : %d\n", ShutdownCounter[12]);    \
    Printf("Hdmi Eq Shutdown Counter    : %d\n", ShutdownCounter[13]);    \
    Printf("\n");

/* End of Declare */

#define DUMP_COUNTER_CLEAN_CLI_USEAGE         Printf("Usage: %s <Counter type>\n", szArgv[0]);  \
        Printf("<Counter type>  0 - Main Power Shutdown Counter\n");                               \
        Printf("                1 - DC Alert Shutdown Counter\n");                                \
        Printf("                2 - Audio Shutdown Counter\n");                                   \
        Printf("                3 - Tuner Demod Shutdown Counter\n");                                 \
        Printf("                4 - LD Shutdown Counter\n");                                \
        Printf("                5 - VLED Shutdown Counter\n");                           \
        Printf("                6 - Tcon Shutdown Counter\n");                             \
        Printf("                7 - Panel ID Shutdown Counter\n");                           \
        Printf("                8 - Backlight Shutdown Counter\n");                              \
        Printf("                9 - Temperature Shutdown Counter\n");                                 \
	    Printf("				10 - SW Shutdown Counter\n");							\
        Printf("                11 - 4K BE Out Shutdown Counter\n");                                \
        Printf("                12 - 4K BE Shutdown Counter\n");                                \
        Printf("                13 - Hdmi Eq Shutdown Counter\n");                                 \
        Printf("\n");                                                                           \
        Printf("               14 - Shutdown Retry Counter\n");                                 \
        Printf("               15 - Boot Counter\n");                                           \
        Printf("               16 - All Counters\n");

#define SET_ERROR_TYPE_NVM_ADDR(offset, ErrorType)                       \
            switch (ErrorType)                                          \
            {                                                           \
            case D_MAIN_POWER_ERROR:                                       \
                offset += SELF_DIAG_MAIN_POWER_ERR_OFFSET;                 \
                break;                                                  \
            case D_DC_ALERT_ERROR:                                        \
                offset += SELF_DIAG_DC_ALERT_ERR_OFFSET;                  \
                break;                                                  \
            case D_AUD_ERROR:                                       \
                offset += SELF_DIAG_AUD_ERR_OFFSET;                 \
                break;                                                  \
            case D_TU_DEMOD_ERROR:                                           \
                offset += SELF_DIAG_TU_DEMOD_ERR_OFFSET;                     \
                break;                                                  \
            case D_LD_ERROR:                                        \
                offset += SELF_DIAG_LD_ERR_OFFSET;                  \
                break;                                                  \
            case D_VLED_ERROR:                                   \
                offset += SELF_DIAG_VLED_ERR_OFFSET;               \
                break;                                                  \
            case D_TCON_ERROR:                                     \
                offset += SELF_DIAG_TCON_ERR_OFFSET;               \
                break;                                                  \
            case D_PANEL_ID_ERROR:                                   \
                offset += SELF_DIAG_PANEL_ID_ERR_OFFSET;             \
                break;                                                  \
            case D_BACKLIGHT_ERROR:                                          \
                offset += SELF_DIAG_BACKLIGHT_ERR_OFFSET;                    \
                break;                                                  \
            case D_TEMPERATURE_ERROR:                                         \
                offset += SELF_DIAG_TEMPERATURE_ERR_OFFSET;                   \
                break;                                                  \
            case D_SW_ERROR:                                     \
                offset += SELF_DIAG_SW_ERR_OFFSET;               \
                break;                                                  \
            case D_4K_BE_OUT_ERROR:                                     \
                offset += SELF_DIAG_4K_BE_OUT_ERR_OFFSET;               \
                break;                                                  \
            case D_4K_BE_ERROR:                                     \
                offset += SELF_DIAG_4K_BE_ERR_OFFSET;               \
                break;                                                  \
            case D_HDMI_EQ_ERROR:                                     \
                offset += SELF_DIAG_HDMI_EQ_ERR_OFFSET;               \
                break;                                                  \
            default:                                                    \
                return 0;                                               \
            }

extern BOOL _bfgSelfDiagGo;

/****************************************************************************
** Wistron Command
****************************************************************************/
static INT32 i4SelfDiagDumpEnableCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    SELF_DIAG_TABLE eSelfDiagEnable;
    
    x_memset(&eSelfDiagEnable, 0, sizeof(SELF_DIAG_TABLE));

    // Get Enable bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_DET_ENABLE_ADDR,
                      (UINT32)(VOID *)&eSelfDiagEnable, sizeof(UINT16)));
    
    // If ALL_ERROR bit == disable, Disable all enable bit
    if (eSelfDiagEnable.u4AllError == SELF_DIAG_DISABLE)
    {
        UINT32 u4Value = 0xFFFFFFFF;
        
        x_memset(&eSelfDiagEnable, u4Value, sizeof(UINT32));
    }
    
    DUMP_ERROR_TYPE(eSelfDiagEnable);
    return 1;
}

static INT32 i4SelfDiagSetEnableCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    BOOL bEnable;
    D_SELF_DIAG_ERROR_TYPE eErrorType;
    SELF_DIAG_TABLE eSelfDiagEnable;
    
    if (i4Argc < 3)
    {
        Printf("Usage: %s <Disable/Enable 1/0> <Error type>\n", szArgv[0]);
        LOG_ERROR_TYPE;
        return 0;
    }

    CHECK_SELT_DIAG_STOP;
    
    // Get Enable bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_DET_ENABLE_ADDR,
                      (UINT32)(VOID *)&eSelfDiagEnable, sizeof(UINT16)));
    
    bEnable = (BOOL)StrToInt(szArgv[1]);
    eErrorType = (D_SELF_DIAG_ERROR_TYPE)StrToInt(szArgv[2]);
    if (eErrorType == D_ALL_ERROR)
    {
        *((UINT32 *)((VOID *)&eSelfDiagEnable)) = ((bEnable) ? 0xFFFFFFFF : 0);
    }
    else
    {
        SET_ERROR_BIT(eSelfDiagEnable, eErrorType, bEnable);
        eSelfDiagEnable.u4AllError = SELF_DIAG_ENABLE;
    }
    
    UNUSED(EEPROM_Write(SELF_DIAG_DET_ENABLE_ADDR,
                        (UINT32)(VOID *)&eSelfDiagEnable, sizeof(UINT32)));
    return 1;
}

static INT32 i4SelfDiagDumpLogCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    SELF_DIAG_TABLE eSelfDiagLog;
    
    x_memset(&eSelfDiagLog, 0, sizeof(SELF_DIAG_TABLE));

    // Get Shutdown bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_LOG_ADDR,
                      (UINT32)(VOID *)&eSelfDiagLog, sizeof(UINT32)));
    DUMP_ERROR_TYPE(eSelfDiagLog);
    return 1;
}

static INT32 i4SelfDiagClearLogCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    BOOL bEnable;
    D_SELF_DIAG_ERROR_TYPE eErrorType;
    SELF_DIAG_TABLE eSelfDiagLog;
    
    if (i4Argc < 2)
    {
        Printf("Usage: %s <Error type>\n", szArgv[0]);
        LOG_ERROR_TYPE;
        return 0;
    }

    CHECK_SELT_DIAG_STOP;
    
    // Get Shutdown bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_LOG_ADDR,
                      (UINT32)(VOID *)&eSelfDiagLog, sizeof(UINT32)));
    bEnable = FALSE;
    eErrorType = (D_SELF_DIAG_ERROR_TYPE)StrToInt(szArgv[1]);
    if (eErrorType == D_ALL_ERROR)
    {
        *((UINT32 *)((VOID *)&eSelfDiagLog)) = ((bEnable) ? 0xFFFFFFFF : 0);
    }
    else
    {
        SET_ERROR_BIT(eSelfDiagLog, eErrorType, bEnable);
    }
    
    UNUSED(EEPROM_Write(SELF_DIAG_ERROR_LOG_ADDR,
                        (UINT32)(VOID *)&eSelfDiagLog, sizeof(UINT32)));
    return 1;
}

static INT32 i4SelfDiagDumpRecoveryCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    SELF_DIAG_TABLE eSelfDiagRecovery;
    
    x_memset(&eSelfDiagRecovery, 0, sizeof(SELF_DIAG_TABLE));

    // Get Recovery bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_BOOT_RETRY_ADDR,
                      (UINT32)(VOID *)&eSelfDiagRecovery, sizeof(UINT32)));
    DUMP_ERROR_TYPE(eSelfDiagRecovery);
    return 1;
}

static INT32 i4SelfDiagSetRecoveryCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    BOOL bEnable;
    D_SELF_DIAG_ERROR_TYPE eErrorType;
    SELF_DIAG_TABLE eSelfDiagRecovery;
    
    if (i4Argc < 3)
    {
        Printf("Usage: %s <Recovery/No Recovery 0/1> <Error type>\n", szArgv[0]);
        LOG_ERROR_TYPE;
        return 0;
    }

    CHECK_SELT_DIAG_STOP;

    // Get Recovery bit from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_BOOT_RETRY_ADDR,
                      (UINT32)(VOID *)&eSelfDiagRecovery, sizeof(UINT32)));
    bEnable = (BOOL)StrToInt(szArgv[1]);
    eErrorType = (D_SELF_DIAG_ERROR_TYPE)StrToInt(szArgv[2]);
    if (eErrorType == D_ALL_ERROR)
    {
        *((UINT32 *)((VOID *)&eSelfDiagRecovery)) = ((bEnable) ? 0xFFFFFFFF : 0);
    }
    else
    {
        SET_ERROR_BIT(eSelfDiagRecovery, eErrorType, bEnable);
    }
    
    UNUSED(EEPROM_Write(SELF_DIAG_BOOT_RETRY_ADDR,
                        (UINT32)(VOID *)&eSelfDiagRecovery, sizeof(UINT32)));
    return 1;
}

static INT32 i4SelfDiagDumpCounterCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UINT8  aShutdownCounter[D_ALL_ERROR];
    UINT8  u1RetryCounter = 0;
    UINT32 u4BootCounter = 0;
    
    x_memset(aShutdownCounter, 0, sizeof(aShutdownCounter));

    // Get shutdown counter from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_CNT_ADDR,
                       (UINT32)(VOID *)aShutdownCounter, sizeof(aShutdownCounter)));
    DUMP_ERROR_COUNTER(aShutdownCounter);
    
    // Get retry count from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_RETRY_CNT_ADDR,
                       (UINT32)(VOID *)&u1RetryCounter, sizeof(UINT8)));
    Printf("Retry Counter                 : %d\n", u1RetryCounter);
    
    // Get boot count from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_BOOT_CNT_ADDR,
                       (UINT32)(VOID *)&u4BootCounter, sizeof(UINT32)));
    Printf("Boot Counter                  : %u\n", u4BootCounter);
    return 1;
}

static INT32 i4SelfDiagClearCounterCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UINT8  u1RetryCounter;
    UINT32 u4BootCounter, u4ErrorType;
    UINT64 u8Offset = SELF_DIAG_ERROR_CNT_ADDR;
    
    if (i4Argc < 2)
    {
        DUMP_COUNTER_CLEAN_CLI_USEAGE;
        return 0;
    }

    CHECK_SELT_DIAG_STOP;
    
    u4ErrorType = StrToInt(szArgv[1]);
    if (u4ErrorType == (D_ALL_ERROR + 2))
    {
        UINT8 aShutdownCounter[D_ALL_ERROR];
        UINT32 u4Len = sizeof(aShutdownCounter);
        
        x_memset(aShutdownCounter, 0, u4Len);
        UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)aShutdownCounter, u4Len));
        
        // Reset retry count from EEPROM
        u1RetryCounter = 0;
        UNUSED(EEPROM_Write(SELF_DIAG_RETRY_CNT_ADDR,
                            (UINT32)(VOID *)&u1RetryCounter, sizeof(UINT8)));
    
        // Reset boot count from EEPROM
        u4BootCounter = 0;
        UNUSED(EEPROM_Write(SELF_DIAG_BOOT_CNT_ADDR,
                            (UINT32)(VOID *)&u4BootCounter, sizeof(UINT32)));
    }
    else
    {
        UINT8 u1Count = 0;

        if (u4ErrorType == (D_ALL_ERROR + 1))
        {
            UINT32 u4Counter = 0;
            
            // Reset error counter from EEPROM
            u4Counter = 0;
            UNUSED(EEPROM_Write(SELF_DIAG_BOOT_CNT_ADDR, 
                                (UINT32)(VOID *)&u4Counter, sizeof(UINT32)));
            return 1;
        }
        else if (u4ErrorType == D_ALL_ERROR)
        {
            UNUSED(EEPROM_Write(SELF_DIAG_RETRY_CNT_ADDR,
                                (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
            return 1;
        }
        else
        {
            SET_ERROR_TYPE_NVM_ADDR(u8Offset, u4ErrorType);
            // Reset error counter from EEPROM
            u1Count = 0;
            UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u1Count, sizeof(UINT8)));
        }
    }
    
    return 1;
}

static INT32 i4SelfDiagDumpThresCmd(INT32 i4Argc, const CHAR ** szArgv)
{
    INT32  i4Temperature = 0;
    UINT16 u2Value = 0;
    UINT32 u4Len;
    
    // Get Threshold default value from EEPROM
    u4Len = sizeof(UINT16);
    UNUSED(EEPROM_Read(SELF_DIAG_TEMP_LIMIT_ADDR, 
                       (UINT32)(VOID *)&u2Value, u4Len));
    u2Value = (((u2Value & 0xFF00) >> 8) | ((u2Value & 0xFF) << 8));
    i4Temperature = (u2Value & (1 << 8)) ? 
                    (0xFFFFFF00 | (u2Value & 0x00FF)) : u2Value;
    Printf("TH_SHTDN_LIMIT = 0x%x (%d) \n", u2Value, i4Temperature);
    
    return 1;
}

static INT32 i4SelfDiagSetThresCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UINT8 u1ThresType;
    UINT32 u4Value;
    UINT32 u4Len = sizeof(UINT8);
    UINT32 u4ValidMax = SELF_DIAG_SERVO_VALID_MAX;
    UINT64 u8Offset;
    
    if (i4Argc < 3)
    {
        Printf("Usage: %s <Threshold type> <value> \n", szArgv[0]);
        Printf("<Threshold type> 0 - TH_SHTDN_LIMIT \n");
        return 0;
    }

    CHECK_SELT_DIAG_STOP
    
    // Get Threshold default from EEPROM
    u1ThresType = (UINT8)StrToInt(szArgv[1]);
    u4Value = (UINT32)StrToInt(szArgv[2]);

    switch (u1ThresType)
    {
    case 0: 
        u4Len = sizeof(UINT16);
        u4ValidMax = SELF_DIAG_TEMP_VALID_MAX;
        u8Offset = SELF_DIAG_TEMP_LIMIT_ADDR;
        break;
    default:
        Printf("Invalid Threshold type !\n");
        return 0;
    }

    if (u4Value > u4ValidMax)
    {
        Printf("Invalid value !\n");
        return 0;
    }

    // Set Threshold default value to EEPROM
    if (u4Len == sizeof(UINT8))
    {
        UINT8 u1Thres = (UINT8)u4Value;
        UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u1Thres, u4Len));
    }
    else
    {
        UINT16 u2Thres = (UINT16)u4Value;
        u2Thres = (((u2Thres & 0xFF00) >> 8) | ((u2Thres & 0xFF) << 8));
        UNUSED(EEPROM_Write(u8Offset, (UINT32)(VOID *)&u2Thres, u4Len));
    }

    return 1;
}

static INT32 i4SelfDiagDumpLimitCmd(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT8 u1Value = 0;
    
    // Get Shutdown retry limitation from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_RETRY_LIMIT_ADDR,
                       (UINT32)(VOID *)&u1Value, sizeof(UINT8)));
    Printf("Retry Limitation = %d \n", u1Value);
    return 1;
}

static INT32 i4SelfDiagSetLimitCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UINT8  u1Value;
    
    if (i4Argc < 2)
    {
        Printf("Usage: %s <value> \n", szArgv[0]);
        return 0;
    }
    
    CHECK_SELT_DIAG_STOP;

    u1Value = (UINT32)StrToInt(szArgv[1]);
    if (u1Value > 0xFF)
    {
        Printf("Invalid value !\n");
        return 0;
    }

    // Get Shutdown retry limitation to EEPROM
    UNUSED(EEPROM_Write(SELF_DIAG_RETRY_LIMIT_ADDR, 
                        (UINT32)(VOID *)&u1Value, sizeof(UINT8)));
    return 1;
}

static INT32 i4SelfDiagStopCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    BOOL bSelfDiagRun = CHECK_SELT_DIAG_STATE;
    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    
    if (!bSelfDiagRun)
    {
        Printf("Self Diagnostic stops.....\n");
    }
    else
    {
        SelfDiagStop(D_SYSTEM_NORMAL);
        Printf("Self Diagnostic stops now..\n");
    }
    return 1;
}

static INT32 i4SelfDiagStartCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    BOOL bSelfDiagRun = CHECK_SELT_DIAG_STATE;
    
    UNUSED(i4Argc);
    UNUSED(szArgv);
    
    if (bSelfDiagRun)
    {
        Printf("Self Diagnostic is running\n");
    }
    else
    {
        SelfDiagStart(D_SYSTEM_NORMAL);
        Printf("Self Diagnostic runs now..\n");
    }
    return 1;
}

static INT32 i4SelfDiagPauseCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UNUSED(i4Argc);
    UNUSED(szArgv);
    
    SelfDiagPause();
    Printf("Self Diagnostic pause now..\n");
    return 1;
}

static INT32 i4SelfDiagResumeCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UNUSED(i4Argc);
    UNUSED(szArgv);
    
    SelfDiagResume();
    Printf("Self Diagnostic runs now..\n");
    return 1;
}

static INT32 i4SelfDiagShutdownCmd(INT32 i4Argc, const CHAR ** szArgv)
{ 
    UINT32  u4Value;
    PDWNC_FuncTbl *prPdwncFuncTbl;
    
    if (i4Argc < 2)
    {
        Printf("Usage: %s <value> \n", szArgv[0]);
        return 0;
    }
    
    u4Value = (UINT32)StrToInt(szArgv[1]);
    if (u4Value > 0xFF)
    {
        Printf("Invalid value !\n");
        return 0;
    }

    prPdwncFuncTbl = PDWNC_EXT_GetFunc();
    if (prPdwncFuncTbl != NULL)
    {
        prPdwncFuncTbl->pfnSet8032uPLedBlink(u4Value);
    }
    return 1;
}

static INT32 i4ESDSetPWMCmd(INT32 i4Argc, const CHAR ** szArgv)
{
#if 0    
    UINT32 u4Port, u4Clock, u4TimeBase;
    UINT32 u4NewFreq, u4Freq;
    UINT32 u4Duty, u4DutyOn, u4DutyAll;
    if (i4Argc < 4)
#endif        
    {
        Printf("Usage: %s <port> <frequence> <cycle> \n", szArgv[0]);
        return 0;
    }
#if 0
    u4Port = (UINT32)StrToInt(szArgv[1]);
    u4Freq = (UINT32)StrToInt(szArgv[2]);
    u4Duty = (UINT32)StrToInt(szArgv[3]);
    u4Clock = BSP_GetDomainClock(SRC_BUS_CLK);
    u4TimeBase = ((float)(u4Clock >> 8) / (float)u4Freq);
    u4NewFreq = (u4Clock >> 8) / (u4TimeBase + 1);
    u4DutyAll = (u4NewFreq * 255) / u4Freq;
    u4DutyOn = (u4DutyAll * u4Duty) / 100;
    
    vDrvSetPWM(u4Port, u4TimeBase, u4DutyOn, u4DutyAll);
    return 1;
#endif    
}        

/******************************************************************************
* Variable		: cli "extra" command table
******************************************************************************/
#endif

// Declare the debug on/off/level and RegTest functions
CLIMOD_DEBUG_FUNCTIONS(EXTRA)


static CLI_EXEC_T arSelfDiagEnableCmdTbl[] = {
#ifdef CC_CLI
    {"Dump",    "d",    i4SelfDiagDumpEnableCmd,    NULL,   "Dump enable bit",  CLI_GUEST},
    {"Set",     "s",    i4SelfDiagSetEnableCmd,     NULL,   "Set enable bit",   CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T arSelfDiagLogCmdTbl[] = {
#ifdef CC_CLI
    {"Dump",    "d",    i4SelfDiagDumpLogCmd,       NULL,   "Dump log",         CLI_GUEST},
    {"Clear",   "c",    i4SelfDiagClearLogCmd,      NULL,   "Clear log bit ",   CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T arSelfDiagRetryCmdTbl[] = {
#ifdef CC_CLI
    {"Dump",    "d",    i4SelfDiagDumpRecoveryCmd,  NULL,   "Dump recovery bit",CLI_GUEST},
    {"Set",     "s",    i4SelfDiagSetRecoveryCmd,   NULL,   "Set recovery bit", CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T arSelfDiagCounterCmdTbl[] = {
#ifdef CC_CLI
    {"Dump",    "d",    i4SelfDiagDumpCounterCmd,   NULL,   "Dump shutdown counter",    CLI_GUEST},
    {"Clear",   "c",    i4SelfDiagClearCounterCmd,  NULL,   "Clear shutdown counter",   CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T arSelfDiagThresCmdTbl[] = {
#ifdef CC_CLI
    {"Dump",    "d",    i4SelfDiagDumpThresCmd,     NULL,   "Dump Threshold voltage",   CLI_GUEST},
    {"Set",     "s",    i4SelfDiagSetThresCmd,      NULL,   "Set Threshold voltage",    CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T arSelfDiagLimitCmdTbl[] = {
#ifdef CC_CLI
    {"Dump",    "d",    i4SelfDiagDumpLimitCmd,     NULL,   "Dump retry limitation",    CLI_GUEST},
    {"Set",     "s",    i4SelfDiagSetLimitCmd,      NULL,   "Set retry limitation",     CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T arSelfDiagCmdTbl[] = {
#ifdef CC_CLI
    {"Enable",  "e",    NULL,   arSelfDiagEnableCmdTbl, "Set enable bit",   CLI_GUEST},
    {"Log",     "l",    NULL,   arSelfDiagLogCmdTbl,    "Set Shutdown log", CLI_GUEST},
    {"Recovery","r",    NULL,   arSelfDiagRetryCmdTbl,  "Set recovery bit", CLI_GUEST},
    {"Counter", "c",    NULL,   arSelfDiagCounterCmdTbl,"Set Shutdown counter", CLI_GUEST},
    {"Thres",   "t",    NULL,   arSelfDiagThresCmdTbl,  "Set Threshold value",  CLI_GUEST},
    {"Limit",   "m",    NULL,   arSelfDiagLimitCmdTbl,  "Set Retry limitation", CLI_GUEST},
    {"Start",   "s",    i4SelfDiagStartCmd,     NULL,   "Start Self-Diag",  CLI_GUEST},
    {"Stop",    "p",    i4SelfDiagStopCmd,      NULL,   "Stop Self-Diag",   CLI_GUEST},
    {"Pause",   "pa",   i4SelfDiagPauseCmd,     NULL,   "Pause Self-Diag",  CLI_GUEST},
    {"resume",  "re",   i4SelfDiagResumeCmd,    NULL,   "Resume Self-Diag", CLI_GUEST},
    {"Shutdown","d",    i4SelfDiagShutdownCmd,  NULL,   "Safety Shutdown",  CLI_GUEST},
#endif
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

static CLI_EXEC_T _arCustCmdTbl[] =
{
#ifdef CC_CLI
    CLIMOD_DEBUG_CLIENTRY(EXTRA),
    {"self-diag",   "sl",   NULL,   arSelfDiagCmdTbl,   "Self Diagnostic",  CLI_GUEST},
    {"ESD",         "esd",  i4ESDSetPWMCmd,     NULL,   "ESD PWM Setting",  CLI_GUEST},
#endif 	
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

CLI_MAIN_COMMAND_ITEM(Cust)
{
	"extra",
	NULL,
	NULL,
	_arCustCmdTbl,
	"custom command",
	CLI_GUEST
};

LINT_RESTORE

