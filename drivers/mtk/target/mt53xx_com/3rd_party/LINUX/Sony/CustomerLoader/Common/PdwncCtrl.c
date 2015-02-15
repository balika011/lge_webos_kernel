
#include "x_pdwnc.h"
#include "x_ckgen.h"
#include "x_timer.h"
#include "x_debug.h"
#include "x_hal_io.h"
#include "x_dram.h"
#include "x_irq.h"
#include "x_util.h"
#include "x_pwr_ctrl.h"
#include "x_pinmux.h"
#include "drv_pwm.h"
#include "pdwnc_if.h"
#include "eeprom_if.h"
#include "nptv_vdec_if.h"
#include "CustomCtrl.h"
#include "PowerSeq.h"
#include "c_model.h"
#include PDWNC_EXT_IMPL
#include GPIO_INCLUDE_FILE
#include "codec_mt5365.h"
#include PINMUX_INCLUDE_FILE
#include "ostg_if.h"
#include "x_hal_5381.h"

#define SIL_SIF_BUS 0 
#define SIL_DEV_PAGE_0 0xD4

#if !defined(CC_MTK_LOADER) && !defined(CC_MTK_IR_ENABLE)
#include "u_irrc_btn_def.h"
#include "ir_if.h"
#include "sirc_ir_tbl.h"
#endif

#ifdef CC_SUPPORT_MHL
#include "mhl_if.h"
#endif
#ifdef LOAD_PANEL_FROM_FLASH_CUSTOM	
#include "panel.h"
#endif

#ifdef CC_MTK_LOADER
#include "loader_if.h"
#include "x_serial.h"
#include "x_drv_cli.h"
#include "x_ldr_env.h"
#endif

#ifndef CC_MTK_LOADER
#ifdef CC_Factory2K12
#include "../../ATV/priv_drv/sony_drv_inc/rgb_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/sig_bstr_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/temp_setup_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/a_bl_ctrl.h"
#include "../../ATV/priv_drv/sony_drv_inc/utility_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/htl_clk_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/htl_vol_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/htl_si_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/sony_tuner_if.h"
#include "../../ATV/priv_drv/sony_drv_inc/wb_pbm.h"
#endif
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//#define HTLSI_SUPPORT
#if !defined(CC_MTK_LOADER) && !defined(CC_MTK_IR_ENABLE)
#define KEY_EQUAL(x, y) ((x)==(y))

static const KEYARRAY_T arKeyArray[] =
{
    { 0x1, (sizeof(_arpulsecheckKeyMap01)/sizeof(KEYUNIT_T)), _arpulsecheckKeyMap01 },    
    { 0x1A, (sizeof(_arpulsecheckKeyMap1A)/sizeof(KEYUNIT_T)), _arpulsecheckKeyMap1A },
    { 0x77, (sizeof(_arpulsecheckKeyMap77)/sizeof(KEYUNIT_T)), _arpulsecheckKeyMap77 },
    { 0x97, (sizeof(_arpulsecheckKeyMap97)/sizeof(KEYUNIT_T)), _arpulsecheckKeyMap97 },
    { 0xA4, (sizeof(_arpulsecheckKeyMapA4)/sizeof(KEYUNIT_T)), _arpulsecheckKeyMapA4 }, 
};
#endif

// Power down event
#if defined(CUST_SOEM_DRV) || defined(CC_SIPADAN_LINUX)
typedef enum
{
    PD_UART_BR_115200 = 0, 
    PD_UART_BR_57600,
    PD_UART_BR_38400,
    PD_UART_BR_19200,
    PD_UART_BR_9600,
    PD_UART_BR_2400
}PD_UART_BR_ENUM ;
#endif

#define PDWNC_CMD_ARMT8032_IR_WAKEUP_KEY    (PDWNC_CMD_ARMT8032_MAX + 1)
#define T8032_IR_WAK_POWER                  0x01
#define T8032_IR_WAK_CH_UP_DOWN             0x02    
#define T8032_IR_WAK_NUMERIC                0x04        
#define T8032_IR_WAK_DTV                    0x08    
#define T8032_IR_WAK_ATV                    0x10    
#define T8032_IR_WAK_JUMP                   0x20
#define T8032_IR_WAK_ADTV_SW                0x40
#define T8032_IR_WAK_HOT_KEY    0x80
#define PDWNC_CMD_ARMT8032_DELAY_WAKEUP_TIMER    (PDWNC_CMD_ARMT8032_MAX + 2)
#define PDWNC_CMD_ARMT8032_SET_CUSTOM_DATA (PDWNC_CMD_ARMT8032_MAX + 3)
#define PDWNC_CMD_ARMT8032_GET_CUSTOM_DATA (PDWNC_CMD_ARMT8032_MAX + 4)

#define WAKEUP_DISABLE_SEC_CNT(x)  (IO32ReadFldAlign(PDWNC_PDMODE, FLD_DEEP_SLEEP))? (x * 0x2DC6C0) : (x * 0x19BFCC0)
#define WAKEUP_DISABLE_MSEC_CNT(x)  (IO32ReadFldAlign(PDWNC_PDMODE, FLD_DEEP_SLEEP))? (x * 0x493E0) : (x * 0x2932E0)
//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
#ifdef CC_MTK_LOADER
static void Custom_InitGPIOTimer(void);
#endif
//-----------------------------------------------------------------------------
// Imported funtions 
//-----------------------------------------------------------------------------
static BOOL bWURsnInit = TRUE;
extern UINT32 IRRC_IsPowerButtonOff(void);

extern BOOL _fgLedBlink[D_LED_TOTAL + 1];

extern BOOL _fgLedAllBlink;
extern BOOL _fgStandbyDiable;
extern UINT32 _u4gModelType;
INT32 PDWNC_SetSonyErrorCode(UINT8 u1Period, UINT8 u1Interval);
INT32 PDWNC_SetSonyErrorCodeBlinking(UINT8 u1times);
#ifndef CC_MTK_PRELOADER
void PDWNC_Custom_Resume(void);
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static PDWNC_FuncTbl rPDWNC_EXT_FuncTbl = 
{
    PDWNC_Custom_Init,                  // pfnInit
    PDWNC_Custom_SetupPowerDown,        // pfnPowerDown
    PDWNC_Custom_ReadWakeupReason,      // pfnReadWakeupReason
    PDWNC_Custom_WatchDogInit,          // pfnWatchDogInit;
    PDWNC_Custom_ErrorHandleInit,       // pfnErrorHandleInit
    PDWNC_Custom_ErrorCode,             // pfnWriteErrorCode
    NULL,                               // pfnReadErrorCode
    PDWNC_Custom_GetCurrentState,       // pfnGetCurrentState
    PDWNC_Custom_DetermineNextState,    // pfnDetermineNextState
    PDWNC_Custom_EnterNextState,        // pfnEnterNextState
    PDWNC_Custom_Set8032uPLedBlink,     // pfnSet8032uPLedBlink
    NULL,                               // pfnCustomRead
    NULL,                                // pfnCustomWrite
	PDWNC_Custom_Resume//pfnPowerResume
};

#ifdef CC_MTK_LOADER
static PFN_IRQ_HANDLER _pfnCustomNextIsr = NULL;
#endif /* CC_MTK_LOADER */

static UINT32 _fgPseudoWatchdogAssert = CC_PSEUDO_WATCHDOG_ASSERT;
//static BOOL   _fgDebug = FALSE;
#ifndef CC_MTK_IR_ENABLE
#ifdef CC_MTK_LOADER
static UINT32 _fgLdrEverReqWakeupReason = 0;
static UINT32 _fgLdrT8032WakeupReason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
#else
static UINT32 _fgEverReqWakeupReason = 0;
static UINT32 _fgT8032WakeupReason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
#endif
#endif

static BOOL   _fgIsNormalMode = TRUE;

static BOOL   _fgIsConfiguredByAP = FALSE;
static BOOL   _fgIsAlwaysPowerOn = FALSE;
static BOOL   _fgIsLastSts = FALSE;
static UINT32 _u4PowerState = STATE_AC_OFF;
#define MHL_GPIO_VBUS_EN (0xffffffff) //219

#ifdef CC_MTK_LOADER
static void LOADER_EnableHdmiDdc(void)
{

    UINT32 u4HDMIHPD0GPIO,u4HDMIHPD1GPIO;

#if (!defined(CC_HDMI_EDID_DISABLE))&&(!defined(CC_ENABLE_HDMI_JTAG))
    //BSP_PinSet(PIN_HDMI_0_SDA, PINMUX_FUNCTION1);
    //BSP_PinSet(PIN_HDMI_0_SCL, PINMUX_FUNCTION1);
    //BSP_PinSet(PIN_HDMI_1_SDA, PINMUX_FUNCTION2);
    //BSP_PinSet(PIN_HDMI_1_SCL, PINMUX_FUNCTION2);
    BSP_PinSet(PIN_HDMI_2_SDA, PINMUX_FUNCTION1);
    BSP_PinSet(PIN_HDMI_2_SCL, PINMUX_FUNCTION1);
    BSP_PinSet(PIN_HDMI_3_SDA, PINMUX_FUNCTION1);
    BSP_PinSet(PIN_HDMI_3_SCL, PINMUX_FUNCTION1);
    if(IS_IC_5880_A_ES2()) /* fix HDMI edid/hdcp write fail */
    {
        //BSP_PinSet(PIN_HDMI_1_SDA, PINMUX_FUNCTION1);
        //BSP_PinSet(PIN_HDMI_1_SCL, PINMUX_FUNCTION1);
        BSP_PinSet(PIN_HDMI_2_SDA, PINMUX_FUNCTION1);
        BSP_PinSet(PIN_HDMI_2_SCL, PINMUX_FUNCTION1);
        BSP_PinSet(PIN_HDMI_2_HPD, PINMUX_FUNCTION2);
    }
    
    //if(IS_IC_5880_A_ES2())
    //{
    //    BSP_PinSet(PIN_HDMI_2_HPD, PINMUX_FUNCTION2);
    //}
    //BSP_PinSet(PIN_HDMI_3_SDA, PINMUX_FUNCTION1);
    //BSP_PinSet(PIN_HDMI_3_SCL, PINMUX_FUNCTION1);
#endif

	if((DRVCUST_InitQuery(eHDMIHPD2GPIONUM, &u4HDMIHPD0GPIO))==0)	
	{
	   GPIO_GetIn(u4HDMIHPD0GPIO);
	} 
	if((DRVCUST_InitQuery(eHDMIHPD3GPIONUM, &u4HDMIHPD1GPIO))==0)	
    {
	 	GPIO_GetIn(u4HDMIHPD1GPIO);
    }	

}

static void LOADER_DisableHdmiDdc(void)
{


#ifndef CC_HDMI_EDID_DISABLE
    //BSP_PinSet(PIN_HDMI_0_SDA, PINMUX_FUNCTION0);
    //BSP_PinSet(PIN_HDMI_0_SCL, PINMUX_FUNCTION0);
    //BSP_PinSet(PIN_HDMI_1_SDA, PINMUX_FUNCTION0);
    //BSP_PinSet(PIN_HDMI_1_SCL, PINMUX_FUNCTION0);
    BSP_PinSet(PIN_HDMI_2_SDA, PINMUX_FUNCTION0);
    BSP_PinSet(PIN_HDMI_2_SCL, PINMUX_FUNCTION0);
    BSP_PinSet(PIN_HDMI_3_SDA, PINMUX_FUNCTION0);
    BSP_PinSet(PIN_HDMI_3_SCL, PINMUX_FUNCTION0);
    //if(IS_IC_5880_A_ES2())
    //{
    //    BSP_PinSet(PIN_HDMI_2_HPD, PINMUX_FUNCTION2);
    //}
    //BSP_PinSet(PIN_HDMI_3_SDA, PINMUX_FUNCTION0);
    //BSP_PinSet(PIN_HDMI_3_SCL, PINMUX_FUNCTION0);
    //BSP_PinSet(PIN_HDMI_0_HPD,PINMUX_FUNCTION0);
    #ifdef CC_MT5880
    //BSP_PinSet(PIN_HDMI_1_HPD,PINMUX_FUNCTION0);
    BSP_PinSet(PIN_HDMI_2_HPD,PINMUX_FUNCTION0);
    #endif
    //BSP_PinSet(PIN_HDMI_3_HPD,PINMUX_FUNCTION0);
#endif

#if  defined(CC_MT5389)
    // PD_SYS   turn off TMDS
    IO_WRITE32(0xF002C13C,0xc00, ((IO_READ32(0xF002C13C,0xc00))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 0,1
    IO_WRITE32(0xF002C13C,0x800, ((IO_READ32(0xF002C13C,0x800))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 2

    //REG_HDMI_RX_Normal_0_CFG2  turn off TMDS
    IO_WRITE32(0xf0062008, 0x100,(IO_READ32(0xF0062008,0x100))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port 0,1
    IO_WRITE32(0xf0062008, 0x0,  (IO_READ32(0xF0062008,0x0))& 0xFFFEFFFF);//Port 2

    // DDC DISABLE
    IO_WRITE32(0xf002C008 ,0xC00 ,(IO_READ32(0xf002C008,0XC00)& 0xFFFF00FF)|0x00008500); //SYS_CTRL
    IO_WRITE32(0xf002C008 ,0X800 ,(IO_READ32(0xf002C008,0X800)& 0xFFFF00FF)|0x00008500);

#elif defined(CC_MT5396)
    // PD_SYS	 turn off TMDS
    IO_WRITE32(0xF002C13C,0xc00, ((IO_READ32(0xF002C13C,0xc00))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 0
    IO_WRITE32(0xF002C13C,0x800, ((IO_READ32(0xF002C13C,0x800))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 1
    IO_WRITE32(0xF002C13C,0x400, ((IO_READ32(0xF002C13C,0x400))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 2
    IO_WRITE32(0xF002C13C,0x0,   ((IO_READ32(0xF002C13C,0x0))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 3

    //REG_HDMI_RX_Normal_0_CFG2  turn off TMDS
    IO_WRITE32(0xf0062008, 0x100,(IO_READ32(0xF0062008,0x100))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port 0
    IO_WRITE32(0xf0062008, 0x0,  (IO_READ32(0xF0062008,0x0))& 0xFFFEFFFF);//Port 1
    IO_WRITE32(0xf0062008, 0x200,(IO_READ32(0xF0062008,0x200))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port2
    IO_WRITE32(0xf0062008, 0x300,  (IO_READ32(0xF0062008,0x300))& 0xFFFEFFFF);//Port 3

    //SYS_CTRL DCC DISABLE
    IO_WRITE32(0xf002C008 ,0xC00 ,(IO_READ32(0xf002C008,0XC00)& 0xFFFF00FF)|0x00008500); //SYS_CTRL
    IO_WRITE32(0xf002C008 ,0X800 ,(IO_READ32(0xf002C008,0X800)& 0xFFFF00FF)|0x00008500);
    IO_WRITE32(0xf002C008 ,0x400 ,(IO_READ32(0xf002C008,0x400)& 0xFFFF00FF)|0x00008500); //SYS_CTRL
    IO_WRITE32(0xf002C008 ,0X0 ,(IO_READ32(0xf002C008,0X0)& 0xFFFF00FF)|0x00008500);

#elif defined(CC_MT5396)
    // PD_SYS	 turn off TMDS
    IO_WRITE32(0xF002C13C,0xc00, ((IO_READ32(0xF002C13C,0xc00))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 0
    IO_WRITE32(0xF002C13C,0x800, ((IO_READ32(0xF002C13C,0x800))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 1
    IO_WRITE32(0xF002C13C,0x400, ((IO_READ32(0xF002C13C,0x400))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 2

    //REG_HDMI_RX_Normal_0_CFG2  turn off TMDS
    IO_WRITE32(0xf0062008, 0x100,(IO_READ32(0xF0062008,0x100))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port 0
    IO_WRITE32(0xf0062008, 0x0,  (IO_READ32(0xF0062008,0x0))& 0xFFFEFFFF);//Port 1
    IO_WRITE32(0xf0062008, 0x200,(IO_READ32(0xF0062008,0x200))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port2

    //SYS_CTRL DCC DISABLE
    IO_WRITE32(0xf002C008 ,0xC00 ,(IO_READ32(0xf002C008,0XC00)& 0xFFFF00FF)|0x00008500); //SYS_CTRL
    IO_WRITE32(0xf002C008 ,0X800 ,(IO_READ32(0xf002C008,0X800)& 0xFFFF00FF)|0x00008500);
    IO_WRITE32(0xf002C008 ,0x400 ,(IO_READ32(0xf002C008,0x400)& 0xFFFF00FF)|0x00008500); //SYS_CTRL

#elif defined(CC_MT5880) 
    // PD_SYS	 turn off TMDS
    IO_WRITE32(0xF002C13C,0xc00, ((IO_READ32(0xF002C13C,0xc00))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 0
    IO_WRITE32(0xF002C13C,0x800, ((IO_READ32(0xF002C13C,0x800))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 1
    IO_WRITE32(0xF002C13C,0x400, ((IO_READ32(0xF002C13C,0x400))&0xFF00FFFF)|0x00CF0000); // PD_SYS PORT 2

    //REG_HDMI_RX_Normal_0_CFG2  turn off TMDS
    IO_WRITE32(0xf0061008, 0x100,(IO_READ32(0xF0062008,0x100))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port 0
    IO_WRITE32(0xf0061008, 0x0,  (IO_READ32(0xF0062008,0x0))& 0xFFFEFFFF);//Port 1
    IO_WRITE32(0xf0061008, 0x200,(IO_READ32(0xF0062008,0x200))& 0xFFFEFFFF); // REG_HDMI_RX_Normal_0_CFG2  Port2

    //SYS_CTRL DCC DISABLE
    IO_WRITE32(0xf002C008 ,0xC00 ,(IO_READ32(0xf002C008,0XC00)& 0xFFFF00FF)|0x00008500); //SYS_CTRL
    IO_WRITE32(0xf002C008 ,0X800 ,(IO_READ32(0xf002C008,0X800)& 0xFFFF00FF)|0x00008500);
    IO_WRITE32(0xf002C008 ,0x400 ,(IO_READ32(0xf002C008,0x400)& 0xFFFF00FF)|0x00008500); //SYS_CTRL

#endif
}
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
#ifdef CUSTOM_SELF_DIAG_ENABLE
static UINT32 u4GetLEDBlinkCode(SELF_DIAG_TABLE eErrorLog)
{
    UINT32 u4LEDBlinkTime = 0;
    
    // Self diagnostic is abnormal, record the error information
    if (eErrorLog.u2Amp12VError || eErrorLog.u2SW12VError)
    {
        u4LEDBlinkTime = 2;
    }
    else if (eErrorLog.u2AudError || eErrorLog.u2VDrop3V_5VError)
    {
        u4LEDBlinkTime = 3;
    }
    else if (eErrorLog.u2PanelPowerError)
    {
        u4LEDBlinkTime = 4;
    }
    else if (eErrorLog.u2PanelNIMError||eErrorLog.u2TouchSnrError
        #ifndef SALVIA_MODEL
        ||eErrorLog.u2RtcError
        #endif
        )
    {
        u4LEDBlinkTime = 5;
    }
    else if (eErrorLog.u2BacklightError)
    {
        u4LEDBlinkTime = 6;
    }
    #ifdef SALVIA_MODEL
    else if(eErrorLog.u2SubWooferError)
    {
        u4LEDBlinkTime = 7;
    }
    #endif
    
    return u4LEDBlinkTime;
}

static BOOL CustomCheckShutdown(void)
{
    BOOL   bfgIsEmpty = TRUE;
    BOOL   bfgShutdown = FALSE;
    UINT8  u1DataInit = 0;
    UINT8  au1Data[SELF_DIAG_EEPROM_SIZE];
    UINT8  u1RetryLimit = 0, u1RetryCount = 0;
    UINT16 u2ShutdownLog = 0, u2Recovery = 0;
    UINT32 u4Len = sizeof(au1Data);
    UINT32 i;
    UINT8 u1Val1 = 0;
    #ifdef CUSTOM_SELF_DIAG_ENABLE
    UINT8 u1Val = 0;
    UINT32 u4Reason=4;
    UINT32 u4Next = STATE_STANDBY;
    #endif
    SELF_DIAG_TABLE eShutdownLog;
    UINT32 u4LEDErrorCode;
        
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
                bfgIsEmpty = FALSE;
                break;
            }
        }
    }

    if (bfgIsEmpty)
    {
        return FALSE;
    }

    // Read the shutdown log from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_ERROR_LOG_ADDR, 
                       (UINT32)(VOID *)&u2ShutdownLog, sizeof(UINT16)));

    // No error
    if (u2ShutdownLog == 0)
    {
        return FALSE;
    }

#ifdef CUSTOM_SELF_DIAG_ENABLE
    //UINT32 u4Reason=4;

	UNUSED(EEPROM_Read(WakeupReasonFlag, 
					   (UINT32)(VOID *)&u4Reason, sizeof(UINT32)));
    if ((PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_AC_POWER) ||(PDWNC_WAKE_UP_REASON_AC_POWER == u4Reason))
    {   
        //UINT8 u1Val = 0;
        u4Reason=4;
		//clear shutdown log to eeprom
		u1Val = SELF_DIAG_DEFALUT_IS_SHUTDOWN;
        
        EEPROM_Write(WakeupReasonFlag,(UINT32)&u4Reason,sizeof(UINT32));
        UNUSED(EEPROM_Write(SELF_DIAG_IS_SHUTDOWN_ADDR,
                        (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
        // Get Shutdown state from EEPROM
        UNUSED(EEPROM_Read(SELF_DIAG_SHUTDOWN_OCCUR_ADDR,
                           (UINT32)(VOID *)&u1Val, sizeof(UINT8)));
        if (u1Val & CUSTOM_SHUTDOWN_ERROR)
        {
            Printf("%s(%d) Self_diag shutdown by CUSTOM_SHUTDOWN_ERROR (%d)\n", __FILE__, __LINE__, u1Val);
            //UINT32 u4Next = STATE_STANDBY;
            u4Next = STATE_STANDBY;
	        PDWNC_Custom_EnterNextState(u4Next);
			//PDWNC_EnterPowerDown((WAK_IRRX | WAK_UNOR), PDWNC_POWER_DOWN_IRRC);
			return TRUE;
           // bfgShutdown = TRUE;
        }
    }
#endif /* CUSTOM_SELF_DIAG_ENABLE */
    
    // Read the recovery table from EEPROM
    UNUSED(EEPROM_Read(SELF_DIAG_BOOT_RETRY_ADDR, 
                       (UINT32)(VOID *)&u2Recovery, sizeof(UINT16)));

    // There is a shutdown log but its recovery bit is disable.
    // So, system need to go to self shutdown
    if ((u2ShutdownLog &(~u2Recovery)) > 0)
    {
        Printf("%s(%d) Self_diag shutdown by RECOVERY disalbe\n", __FILE__, __LINE__);
        Printf("u2ShutdownLog(%d), u2Recovery(%d)\n", u2ShutdownLog, u2Recovery);
        bfgShutdown = TRUE;
    }

    // Start to retry from safety shutdown
    
    // Read the retry limitation from EEPROM
    // If the retry limit = 0 -> system does not receive recovery command
    UNUSED(EEPROM_Read(SELF_DIAG_RETRY_LIMIT_ADDR, 
                       (UINT32)(VOID *)&u1RetryLimit, sizeof(UINT8)));
    if (u1RetryLimit == 0)
    {
        Printf("%s(%d) Self_diag shutdown by Retry limit is 0\n", __FILE__, __LINE__);
        bfgShutdown = TRUE;
    }

    // If the retry limit > 0, read the current retry counter from EEPROM
    // Check if the counter reaches the retry limitation.
    // if yes, system does not recovery any more
    UNUSED(EEPROM_Read(SELF_DIAG_RETRY_CNT_ADDR, 
                       (UINT32)(VOID *)&u1RetryCount, sizeof(UINT8)));
    //Printf("*********u1RetryCount(%d), u1RetryLimit(%d)\n", u1RetryCount, u1RetryLimit);
    if (u1RetryCount >= u1RetryLimit)
    {
        Printf("%s(%d) Self_diag shutdown by Retry limit equal with retry cnt\n", __FILE__, __LINE__);
        Printf("u1RetryCount(%d), u1RetryLimit(%d)\n", u1RetryCount, u1RetryLimit);
        bfgShutdown = TRUE;
    }
    //UINT8 u1Val1 = 0;
    UNUSED(EEPROM_Read(SELF_DIAG_IS_SHUTDOWN_ADDR, 
                       (UINT32)(VOID *)&u1Val1, sizeof(UINT8)));
	if(!u1Val1)
	{
	bfgShutdown = FALSE;
	}
    if (bfgShutdown)
    {
        x_memcpy(&eShutdownLog, &u2ShutdownLog, sizeof(SELF_DIAG_TABLE));
        u4LEDErrorCode = u4GetLEDBlinkCode(eShutdownLog);
        PDWNC_Custom_Set8032uPLedBlink(u4LEDErrorCode);
    }
    
    return bfgShutdown;    
}
#endif /* CUSTOM_SELF_DIAG_ENABLE */



//-----------------------------------------------------------------------------
/** _IRRX_CrystalLookup()
 */
//-----------------------------------------------------------------------------
#if !defined(CC_MTK_LOADER) && !defined(CC_MTK_IR_ENABLE)
static UINT32 _SirCrystalLookup(const KEYUNIT_T *prKeyMap,
                                UINT32 u4Size, UINT32 u4Data)
{
    UINT32 u4Min, u4Max, u4Avg;

    if (prKeyMap == NULL)
    {
        return BTN_INVALID;
    }

    if(u4Size == 1)
    {
        if (KEY_EQUAL(prKeyMap[0].u4Key, u4Data))
        {
            return prKeyMap[0].u4Crystal;
        }        
        else
        {
            return BTN_INVALID;
        }
    }

    u4Min = 0;
    u4Max = u4Size - 1;

    while (u4Max > u4Min)
    {
        u4Avg = (u4Max + u4Min) / 2;

        if (KEY_EQUAL(prKeyMap[u4Min].u4Key, u4Data))
        {
            return prKeyMap[u4Min].u4Crystal;
        }

        if (KEY_EQUAL(prKeyMap[u4Max].u4Key, u4Data))
        {
            return prKeyMap[u4Max].u4Crystal;
        }

        if (KEY_EQUAL(prKeyMap[u4Avg].u4Key, u4Data))
        {
            return prKeyMap[u4Avg].u4Crystal;
        }
        else if (prKeyMap[u4Avg].u4Key > u4Data)
        {
            u4Max = u4Avg;
        }
        else if (prKeyMap[u4Avg].u4Key < u4Data)
        {
            if (u4Min == u4Avg)
            {
                u4Max = u4Avg;
            }
            else
            {
                u4Min = u4Avg;
            }
        }
    }
    return BTN_INVALID;
}

static UINT32 _SirRawKey2WakeupReason(UINT8 u1IrKey)
{
    UINT32 u4Key;
    u4Key =  _SirCrystalLookup(arKeyArray[0].prKeyMap, arKeyArray[0].u4Size,
                               (UINT32)(u1IrKey & 0x7f));
    if(u4Key == BTN_POWER || u4Key == BTN_POWER_ON)
    {
        return PDWNC_WAKE_UP_REASON_IRRC;
    }
    else if(u4Key == BTN_PRG_UP)
    {
        return PDWNC_WAKE_UP_REASON_RC_PRG_UP;
    }
    else if(u4Key == BTN_PRG_DOWN)
    {
        return PDWNC_WAKE_UP_REASON_RC_PRG_DOWN;
    }
    else if(u4Key == BTN_DIGIT_0)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_0;
    }
    else if(u4Key == BTN_DIGIT_1)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_1;
    }
    else if(u4Key == BTN_DIGIT_2)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_2;
    }
    else if(u4Key == BTN_DIGIT_3)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_3;
    }
    else if(u4Key == BTN_DIGIT_4)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_4;
    }
    else if(u4Key == BTN_DIGIT_5)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_5;
    }
    else if(u4Key == BTN_DIGIT_6)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_6;
    }
    else if(u4Key == BTN_DIGIT_7)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_7;
    }
    else if(u4Key == BTN_DIGIT_8)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_8;
    }
    else if(u4Key == BTN_DIGIT_9)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGIT_9;
    }
    else if((u4Key == BTN_TV_ANA) || (u4Key == BTN_TV))
    {
        return PDWNC_WAKE_UP_REASON_RC_ANALOG;
    }
    else if(u4Key == BTN_TV_DIG)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGITAL;
    }
    else if(u4Key == BTN_CUSTOM_68)
    {
        return PDWNC_WAKE_UP_REASON_RC_DIGITAL_ANALOG;
    }    
    else
    {
        return PDWNC_WAKE_UP_REASON_IRRC;
    }
        
}
#endif

UINT32 PDWNC_SetupPseudoWatchdogAssert(UINT32 *pu4Flag)
{
//    UNUSED(pu4Flag);
    if (pu4Flag != NULL)
    {
        _fgPseudoWatchdogAssert = *pu4Flag;
    }
    return _fgPseudoWatchdogAssert;
}

#ifndef CC_MTK_LOADER
//-----------------------------------------------------------------------------
/** _WatchDogSimIsr() a watch dog simulation interrupt routine.
 */
//-----------------------------------------------------------------------------
static void _WatchDogSimIsr(UINT16 u2VctId)
{
    UNUSED(u2VctId);
    Printf("Pseudo watchdog isr timeout\n");
#ifdef NDEBUG
    if (_fgPseudoWatchdogAssert)
    {
        Assert("_WatchDogSimIsr", __FILE__, (INT32)__LINE__);
    }
#endif /* NDEBUG */
    VERIFY(BIM_ClearIrq(VECTOR_T1));
}

//-----------------------------------------------------------------------------
/** _WatchDogBody() a watch dog thread body.
 */
//-----------------------------------------------------------------------------
static void _WatchDogBody(void *pvArg)
{
    UINT32 u4Clk, u4Sec;
    UINT32 u4Val, u4State;
    void (* pfnOldIsr)(UINT16);
    HAL_TIME_T rPrevTime, rTime, rDelta;

    UNUSED(pvArg);

    // Set simulation timer initial value.
    if (DRVCUST_OptQuery(eTimerDimmingFreq, &u4Val) != 0)
    {
        INT32 i4Ret;

        // no one use timer1, Hook interrupt
        i4Ret = x_reg_isr(VECTOR_T1, _WatchDogSimIsr, &pfnOldIsr);
        ASSERT(i4Ret == OSR_OK);
        UNUSED(i4Ret);
    
        // set initial timer, 25 seconds.
        BIM_WRITE32(REG_RW_TIMER1_LOW , 0x80000000);
        BIM_WRITE32(REG_RW_TIMER1_HIGH, 0);

        // enable timer 1
        BIM_LockModifyReg32(REG_RW_TIMER_CTRL, ~(TMR1_AUTOLD_EN), (TMR1_CNTDWN_EN));
    }

    // Disable the watchdog
    vIO32Write4B(PDWNC_WDTCTL, 0);//PDWNC_WRITE32(REG_RW_WATCHDOG_EN, 0);

#ifdef NDEBUG
    // Enable the watchdog. release version only.
    if (DRVCUST_OptGet(eReleaseEnableWatchDog))
    {
        // Set watchdog timer initial value, 25 seconds.
        vIO32Write4B(PDWNC_WDT0, 0x8000000);//PDWNC_WRITE32(REG_RW_WATCHDOG_TMR, 0x8000000);
        // Enable the watchdog, then watchdog can be disable by outside.
        vIO32Write4B(PDWNC_WDTCTL, 1); //PDWNC_WRITE32(REG_RW_WATCHDOG_EN, 1);
        u4Clk = BSP_GetDomainClock(SRC_BUS_CLK);
        u4Sec = DRVCUST_OptGet(eReleaseWatchdogTimeout);
        Printf("Watchdog enable:%d\n", (u4Sec*u4Clk));
	}
#endif

    while (1)
    {
        // Make set counter as critical section.
        // All values come from register.
        u4State = HalCriticalStart();
        u4Clk = BSP_GetDomainClock(SRC_TIMER_CLK);
        u4Sec = DRVCUST_OptGet(eReleaseWatchdogTimeout);
        u4Clk = u4Sec*u4Clk;
        if (DRVCUST_OptQuery(eTimerDimmingFreq, &u4Val) != 0)
        {
            BIM_WRITE32(REG_RW_TIMER1_LOW , u4Clk);
            BIM_WRITE32(REG_RW_TIMER1_HIGH, 0);
            BIM_LockModifyReg32(REG_RW_TIMER_CTRL, ~(TMR1_AUTOLD_EN), (TMR1_CNTDWN_EN));
        }
        vIO32Write4B(PDWNC_WDT0, WATCHDOG_SEC_CNT(u4Sec));
        
        HalCriticalEnd(u4State);

        // Measure time for debugging.
        HAL_GetTime(&rPrevTime);
        x_thread_delay(500);
        HAL_GetTime(&rTime);
        HAL_GetDeltaTime(&rDelta, &rPrevTime, &rTime);
        if (rDelta.u4Seconds >= u4Sec)
        {
            Printf("Pseudo watchdog timeout\n");
#ifdef NDEBUG
            // Assert("rDelta.u4Seconds < u4Sec", __FILE__, (INT32)__LINE__);
#endif
        }
    }
}

#endif /* CC_MTK_LOADER */

void PDWNC_EU_DetermineNextState(UINT32 u4CurrentState, UINT32 u4PowerEvent,
                                 UINT32 *pu4NextState, UINT32 *pfgEnterStandby,
                                 UINT32 *pu4PowerDownEnable)
{
    if (u4CurrentState >= STATE_TOTAL)
    {
        *pu4NextState = STATE_POWER_ON;
        if (pfgEnterStandby != NULL)
        {
            *pfgEnterStandby = 0;
        }
        return;
    }

    *pu4NextState = u4CurrentState;
    switch (u4PowerEvent & PDWNC_POWER_STATE_CHAGNE_MASK)
    {
    case PDWNC_POWER_STATE_SEMI_STANDBY:
        _fgIsNormalMode = FALSE;
        break;
    case PDWNC_POWER_STATE_NORMAL:
        Printf("Enter the normal state\n");
        *pu4NextState = STATE_POWER_ON;
        break;
    }
    
    u4PowerEvent &= PDWNC_POWER_STATE_EVENT_MASK;
    if ((u4CurrentState == STATE_POWER_ON) &&
        (u4PowerEvent != PDWNC_WAKE_UP_REASON_AC_POWER))
    {
        // Go to Standy or Power off state
        switch (u4PowerEvent)
        {
        case PDWNC_POWER_DOWN_KEYPAD:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_KEYPAD\n");
            *pu4NextState = STATE_POWER_OFF;
            break;
        case PDWNC_POWER_DOWN_IRRC:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_IRRC\n");
            *pu4NextState = STATE_STANDBY;
            break;
        default:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_UNKNOWN\n");
            *pu4NextState = STATE_POWER_ON;
            break;
        }
    }
    else
    {
        // For boot loader
        Printf("Determine Next State : system booting\n");
        
        switch (u4PowerEvent)
        {
        case PDWNC_WAKE_UP_REASON_AC_POWER:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_AC_POWER\n");
            *pu4NextState = u4CurrentState;
            if (_fgIsConfiguredByAP && _fgIsAlwaysPowerOn)
            {
                *pu4NextState = STATE_POWER_ON;
            }
            break;
            
        case PDWNC_WAKE_UP_REASON_KEYPAD:
            Printf("Determine Next State : event = PDWNC_WAKE_UP_REASON_KEYPAD\n");
            if (u4CurrentState == STATE_STANDBY)
            {
                *pu4NextState = STATE_POWER_OFF;
            }
            else if (u4CurrentState == STATE_POWER_OFF)
            {
                *pu4NextState = STATE_POWER_ON;
                _fgIsNormalMode = TRUE;
            }
            break;
            
        case PDWNC_WAKE_UP_REASON_IRRC:
        case PDWNC_WAKE_UP_REASON_RC_PRG_UP:
        case PDWNC_WAKE_UP_REASON_RC_PRG_DOWN:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_0:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_1:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_2:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_3:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_4:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_5:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_6:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_7:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_8:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_9:
        case PDWNC_WAKE_UP_REASON_RC_ANALOG:
        case PDWNC_WAKE_UP_REASON_RC_DIGITAL:
        case PDWNC_WAKE_UP_REASON_RC_DIGITAL_ANALOG:
        case PDWNC_WAKE_UP_REASON_VGA:
        case PDWNC_WAKE_UP_REASON_HDMI:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_IRRC\n");
            if (u4CurrentState == STATE_STANDBY)
            {
                *pu4NextState = STATE_POWER_ON;
                _fgIsNormalMode = TRUE;
            }
            break;
            
        case PDWNC_WAKE_UP_REASON_UART:
        case PDWNC_WAKE_UP_REASON_UART_NORMAL:
            *pu4NextState = STATE_POWER_ON;
            break;

        case PDWNC_WAKE_UP_REASON_RTC:
            if ((_fgIsNormalMode) && (u4CurrentState == STATE_STANDBY))
            {
                *pu4NextState = STATE_POWER_ON;
            }
            break;
        
        default:
            Printf("Determine Next State : event = others\n");
            break;
        }
    }

    Printf("Current = 0x%x, Event = 0x%x, Next = 0x%x\n",
           u4CurrentState, u4PowerEvent, *pu4NextState);
    
    if (pfgEnterStandby != NULL)
    {
        switch (*pu4NextState)
        {
        case STATE_STANDBY:
        case STATE_POWER_OFF:
            if (_fgIsNormalMode)
            {
                if (pu4PowerDownEnable != NULL)
                {
                    *pu4PowerDownEnable |= (WAK_IRRX | WAK_UNOR);
                    *pu4PowerDownEnable &= ~(WAK_VGA);
                    Printf("Enable IR wakeup!\n");
                }
                *pfgEnterStandby = 1;
            }
            else
            {
                // Wakeup for BGM (semi-standby)
                *pfgEnterStandby = 0;
            }
            break;
            
        case STATE_POWER_ON:
        default:
            *pfgEnterStandby = 0;
            break;
        }
    }
}

void PDWNC_PAD_DetermineNextState(UINT32 u4CurrentState, UINT32 u4PowerEvent,
                                  UINT32 *pu4NextState, UINT32 *pfgEnterStandby,
                                  UINT32 *pu4PowerDownEnable)
{
	UINT32 u4Country;
	UINT8 u1Val = 0;
	UINT8 *pSPanelIdData=NULL;
	#if 1//def SYS_OAD_SUPPORT
	DTVCFG_T rDtvCfg;
	#endif
	if (DRVCUST_InitQuery(eCountryDefaultSetting, &u4Country) != 0)
	{
		u4Country = COUNTRY_US;
	}
#if 1//def SYS_OAD_SUPPORT
	if (EEPDTV_GetCfg(&rDtvCfg) == 0)
	{
		/*if(rDtvCfg.u1Flags4 & DTVCFG_FLAG4_OAD_STANDBY)
		{
			*pu4NextState = STATE_STANDBY;
			//rDtvCfg.u1Flags4 &= ~DTVCFG_FLAG4_OAD_STANDBY;
			//UNUSED(EEPDTV_SetCfg(&rDtvCfg));
			if (pfgEnterStandby != NULL)
	        {
	            *pfgEnterStandby = 1;
	        }
			Printf("Determin Next State is STANDBY because of OAD upgrade in standby status just now \n");
			return;
		}*/
	}
#endif
    if (u4CurrentState >= STATE_TOTAL)
    {
        *pu4NextState = STATE_POWER_ON;
        if (pfgEnterStandby != NULL)
        {
            *pfgEnterStandby = 0;
        }
        return;
    }

    *pu4NextState = u4CurrentState;
    switch (u4PowerEvent & PDWNC_POWER_STATE_CHAGNE_MASK)
    {
    case PDWNC_POWER_STATE_SEMI_STANDBY:
        _fgIsNormalMode = FALSE;
        break;
    case PDWNC_POWER_STATE_NORMAL:
        Printf("Enter the normal state\n");
        *pu4NextState = STATE_POWER_ON;
        break;
    }
    u4PowerEvent &= PDWNC_POWER_STATE_EVENT_MASK;
    if ((u4CurrentState == STATE_POWER_ON) &&
        (u4PowerEvent != PDWNC_WAKE_UP_REASON_AC_POWER))
    {
        // Go to Standy state
        switch (u4PowerEvent)
        {
        case PDWNC_POWER_DOWN_KEYPAD:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_KEYPAD\n");
            *pu4NextState = STATE_POWER_OFF;
            break;
        case PDWNC_POWER_DOWN_IRRC:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_IRRC\n");
            *pu4NextState = STATE_STANDBY;
            break;
        default:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_UNKNOWN\n");
            *pu4NextState = STATE_POWER_ON;
            break;
        }
    }
    else
    {
        // For boot loader
        Printf("Country (%d)Determine Next State : system booting\n", u4Country);
        
        switch (u4PowerEvent)
        {
        case PDWNC_WAKE_UP_REASON_AC_POWER:
            Printf("Determine Next State : event = PDWNC_POWER_DOWN_AC_POWER, IsConfiguredByAP: %d, lastSts: %d, alwaysOn %d\n", _fgIsConfiguredByAP, _fgIsLastSts, _fgIsAlwaysPowerOn);
            if (!_fgIsConfiguredByAP)
            {
                *pu4NextState = u4CurrentState;
                if(u4CurrentState == STATE_POWER_OFF)
                {
                    *pu4NextState = STATE_STANDBY;
                }
                if((u4Country == COUNTRY_TW) || (u4Country == COUNTRY_BZ))
                {
				   pSPanelIdData=(UINT8 *)PANEL_GetVersion();

                   #ifdef PANEL_VERSION_LENGTH
				   for(u1Val=0;pSPanelIdData &&u1Val<PANEL_VERSION_LENGTH;u1Val++)
				   {
                      if(0xFF!=pSPanelIdData[u1Val])break;					  
				   }
				   
                    if((PANEL_VERSION_LENGTH!=u1Val) && (u4CurrentState == STATE_POWER_ON))
                    {
                        *pu4NextState = STATE_STANDBY;
                    }	
                   #endif
                    
                }
            }
            else
            {
                if(_fgIsLastSts)
                {
                    *pu4NextState = u4CurrentState;
                    if(u4CurrentState == STATE_POWER_OFF)
                    {
                        *pu4NextState = STATE_STANDBY;
                    }
                }
                else
                {
                    if (_fgIsAlwaysPowerOn)
                    {
                        *pu4NextState = STATE_POWER_ON;
                    }
                    else
                    {
                        *pu4NextState = STATE_STANDBY;
                    }
                }
            }
            UNUSED(EEPROM_Read(POWER_SEQ_FORCE_TO_STANDBY_ADDR, 
                               (UINT32)(void *)&u1Val, sizeof(UINT8)));
            if (u1Val == 0)
            {
                Printf("force power on\n");
                *pu4NextState = STATE_POWER_ON;
            }
            break;
            
        case PDWNC_WAKE_UP_REASON_KEYPAD:
            if (u4CurrentState == STATE_STANDBY)
            {			
	           *pu4NextState = STATE_POWER_OFF;
			}
			else
			{
				*pu4NextState = STATE_POWER_ON;
			}
		   _fgIsNormalMode = TRUE;
		   	break;
        case PDWNC_WAKE_UP_REASON_IRRC:
        case PDWNC_WAKE_UP_REASON_RC_PRG_UP:
        case PDWNC_WAKE_UP_REASON_RC_PRG_DOWN:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_0:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_1:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_2:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_3:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_4:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_5:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_6:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_7:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_8:
        case PDWNC_WAKE_UP_REASON_RC_DIGIT_9:
        case PDWNC_WAKE_UP_REASON_RC_ANALOG:
        case PDWNC_WAKE_UP_REASON_RC_DIGITAL:
        case PDWNC_WAKE_UP_REASON_RC_DIGITAL_ANALOG:
        case PDWNC_WAKE_UP_REASON_VGA:
        case PDWNC_WAKE_UP_REASON_HDMI:
        case PDWNC_WAKE_UP_REASON_UART:
        case PDWNC_WAKE_UP_REASON_UART_NORMAL:
            *pu4NextState = STATE_POWER_ON;
            _fgIsNormalMode = TRUE;
            break;
        case PDWNC_WAKE_UP_REASON_WATCHDOG://d131009_Haibo:direct reset can not power on automatically issue
            if(STATE_SEMI_STANDBY!=u4CurrentState)//d131010_Haibo:If now is reset by DC_MON in BGM,keep previous power state
			{
			   *pu4NextState = STATE_POWER_ON;
			}
			_fgIsNormalMode = TRUE;
            break;
        case PDWNC_WAKE_UP_REASON_RTC:
            if ((_fgIsNormalMode) && (u4CurrentState == STATE_STANDBY))
            {
                *pu4NextState = STATE_POWER_ON;
                _fgIsNormalMode = TRUE;
            }
            break;
        
        default:
            Printf("Determine Next State : event = others\n");
            break;
        }
    }

    Printf("Current = 0x%x, Event = 0x%x, Next = 0x%x\n",
           u4CurrentState, u4PowerEvent, *pu4NextState);
    
    if (pfgEnterStandby != NULL)
    {
        switch (*pu4NextState)
        {
        case STATE_STANDBY:
		case STATE_POWER_OFF:
		case STATE_SEMI_STANDBY://d131009_Haibo:RC OFF->BGM ON->AC OFF->AC ON,need to enter previous power state		
            if (_fgIsNormalMode)
            {
                if (pu4PowerDownEnable != NULL)
                {
                    *pu4PowerDownEnable |= (WAK_IRRX | WAK_UNOR);
                    *pu4PowerDownEnable &= ~(WAK_VGA);
                    Printf("Enable IR wakeup!\n");
                }
                *pfgEnterStandby = 1;
            }
            else
            {
                // Wakeup for BGM (semi-standby)
                *pfgEnterStandby = 0;
            }
            break;
            
        case STATE_POWER_ON:
        default:
            *pfgEnterStandby = 0;
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------
BOOL PDWNC_Check_Enter_SemiStandy(void)
{
    UINT32 u4WakeupReason = PDWNC_ReadWakeupReason();
    DTVCFG_T rDtvCfg;

    x_memset(&rDtvCfg, 0, sizeof(DTVCFG_T));
    UNUSED(EEPDTV_GetCfg(&rDtvCfg));
    
    if ((u4WakeupReason == PDWNC_WAKE_UP_REASON_RTC) &&
        (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT))
    {
        return TRUE;
    }

    return FALSE;
}

PDWNC_FuncTbl* PDWNC_EXT_GetFunc(void)
{
    return (&rPDWNC_EXT_FuncTbl);
}

void PDWNC_Custom_Init(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_Factory2K12
    PDWNC_T8032_CMD_T rCmd;
    PDWNC_T8032_RESPONSE_T rResponse;
    UINT8 _u1HotelModule2Status;
    x_memset(&rCmd, 0, sizeof(PDWNC_T8032_CMD_T));
    rCmd.u1Cmd = PDWNC_CMD_ARMT8032_GET_CUSTOM_DATA;
    if (PDWNC_T8032Cmd(&rCmd, &rResponse) == 0)
    {
        _u1HotelModule2Status = (rResponse.au1Data[0]);
        HTLSI_SetOnlineMode(&_u1HotelModule2Status);
        Printf("Get uP Hotel Module2 Status %d\n", _u1HotelModule2Status);
    }
    else
    {
        Printf("Set Hotel Module2 Status Faill!!!\n");
    }
    UNUSED(PDWNC_SetSonyErrorCode(CUSTOM_BLINK_TIME, CUSTOM_BLINK_INVERAL));
#endif /* CC_MTK_LOADER */
#endif
    return;
}

void PDWNC_Custom_WatchDogInit(void)
{
#ifndef CC_MTK_LOADER
    static HANDLE_T hWatchDog;

    vIO32Write4B(PDWNC_WDTCTL, 0);//PDWNC_WRITE32(REG_RW_WATCHDOG_EN, 0);
    if (x_thread_create(&hWatchDog, "WatchDog", 0x800, 
                        DRVCUST_OptGet(eReleaseWatchdogPriority),
                        _WatchDogBody, 0, NULL) != OSR_OK)
    {
        LOG(1, "\nError: Create watchdog thread fail\n");
        return;
    }
#endif /* CC_MTK_LOADER */
}

void PDWNC_Custom_ErrorHandleInit(void)
{
    return;
}
static UINT32 _IsWakeupByPowerButton(UINT32 u4WakeupStatus)
{
    SGL_GPIO_T   *prSingleGpioKeyList = NULL;
    SGL_SERVO_T  *prSingleServoKeyList = NULL;
    UINT32 u4PowerKeypadIndex = 0;
    UINT32 u4KeypadPowerKeyPressed = 0;

    if (0 == DRVCUST_InitQuery(eSingleServoKeyList, (UINT32 *)(void *)&prSingleServoKeyList))
    {
        u4PowerKeypadIndex = 0;
        while ((prSingleServoKeyList) &&
            (prSingleServoKeyList[u4PowerKeypadIndex].u4Key != 0) &&
            (prSingleServoKeyList[u4PowerKeypadIndex].u4Key != DKBIT_NONE))
        {
            if (prSingleServoKeyList[u4PowerKeypadIndex].u4Key == DKBIT_POWER)
            {
                if (u4WakeupStatus & PDWNC_SRVAD_WAKEN(PDWNC_WAK_SRVADX(prSingleServoKeyList[u4PowerKeypadIndex].u4Servo-ADIN0)))
                {
                    Printf("Wakeup by power key pad servo (0x%x)...\n", _PDWNC_ReadWakeupStatus());
                    u4KeypadPowerKeyPressed = 1;
                }
                break;
            }
            ++u4PowerKeypadIndex;
        }
    }
    if(DRVCUST_InitQuery(eSingleGpioKeyList, (UINT32 *)(void *)&prSingleGpioKeyList)==0)
    {
        u4PowerKeypadIndex = 0;
        while ((prSingleGpioKeyList) &&
               (prSingleGpioKeyList[u4PowerKeypadIndex].u4Key != 0) &&
               (prSingleGpioKeyList[u4PowerKeypadIndex].u4Key != DKBIT_NONE))
        {
            if (prSingleGpioKeyList[u4PowerKeypadIndex].u4Key & DKBIT_POWER)
            {
                if( u4WakeupStatus & ( GPIO_WAKEN(WAK_GPIOX(prSingleGpioKeyList[u4PowerKeypadIndex].u4Gpio - OPCTRL(0)))))
                {
                    Printf("Wakeup by power key pad gpio (0x%x)...\n", _PDWNC_ReadWakeupStatus());
                    u4KeypadPowerKeyPressed = 1;
                }    
                break;
            }
            ++u4PowerKeypadIndex;
        }
    }
    return u4KeypadPowerKeyPressed;
}
#if !defined(CC_MTK_LOADER) && !defined(CC_MTK_IR_ENABLE)

UINT8 aLocatelSetting = 0;
void PDWNC_Custom_GetLocatelData(UINT8 ui1SubSrc)
{
	aLocatelSetting = ui1SubSrc;
}
#endif
void PDWNC_Custom_ReadWakeupReason(UINT32* pu4Reason)
{
    //static BOOL bInit = TRUE;
    static UINT32 u4BootReason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
    UINT32 u4Val;
    DTVCFG_T rDtvCfg;
    UINT32 u4KeypadPowerKeyPressed = 0;  

    if (!bWURsnInit)
    {
        *pu4Reason = u4BootReason;
        return;
    }

    // Wakeup REASON...
    u4Val = _PDWNC_ReadWakeupStatus();     
    u4KeypadPowerKeyPressed = _IsWakeupByPowerButton(u4Val);
    if (u4Val == 0)
    {
        if(_PDWNC_ReadWatchDogStatus())
        {
            *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_WATCHDOG;
#ifdef CC_MTK_LOADER
            Printf("Boot reason: Watch dog reset!!");            
#endif
        }
        else
        {
            if (EEPDTV_GetCfg(&rDtvCfg) == 0)
            {
                if(rDtvCfg.u1Flags4 & DTVCFG_FLAG4_FAST_CLOCK)
    		    {
        			rDtvCfg.u1Flags4 &= ~DTVCFG_FLAG4_FAST_CLOCK;
        			UNUSED(EEPDTV_SetCfg(&rDtvCfg));
                }
            }
            *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_AC_POWER;
#ifdef CC_MTK_LOADER            
            Printf("Boot reason: A/C power on!!");                       
#endif
        }
    }
    else if (u4Val & WAK_RTC)
    {
#ifdef CUSTOM_SELF_DIAG_ENABLE
        if (IS_AC_DETECT_STANDBY())
        {
            *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_RTC_SPECIAL;
        }
        else
#endif /* CUSTOM_SELF_DIAG_ENABLE */
        {
            if (EEPDTV_GetCfg(&rDtvCfg) == 0)
            {
                if (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT)
                {
                    UNUSED(_fgIsNormalMode);
                    _fgIsNormalMode = FALSE; // enter semi-standby
                }
            }
            
            *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_RTC;
        }
    }
    else if (u4Val & WAK_VGA)
    {
        *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_VGA;
    }
    else if (u4Val & WAK_UNOR)//WAK_RS232) removed by wangliang
    {
        *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_UART;
    }
    else if (u4Val & WAK_UNOR)
    {
        *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_UART_NORMAL;
    }
    //else if (u4Val & (WAK_HDMI/* | WAK_CEC*/))
    //{
    //    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_HDMI;
    //}
    else if (u4KeypadPowerKeyPressed)//(u4Val & WAK_GPIO2)     // Button power
    {
        //*pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_KEYPAD;
		*pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
    }
    else // if (u4Val & WAK_IRRX)
    {
#if !defined(CC_MTK_LOADER) && !defined(CC_MTK_IR_ENABLE)

        PDWNC_T8032_CMD_T rCmd;
        PDWNC_T8032_RESPONSE_T rResponse;
        if (_fgEverReqWakeupReason == 0)
        {
            x_memset(&rCmd, 0, sizeof(PDWNC_T8032_CMD_T));
            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_READ_WAKEUP_REASON;
            *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
            
            if (PDWNC_T8032Cmd(&rCmd, &rResponse) == 0)
            {
				if(rResponse.au1Data[0] == PDWNC_WAKE_UP_REASON_UART)
				{
					*pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_UART;
					PDWNC_Custom_GetLocatelData(rResponse.au1Data[1]);
					//aLocatelSetting[7] = rResponse.au1Data[1];
				}
				else
				{
	                switch (rResponse.au1Data[2])
	                {
	                case CUSTOM_SERVICE_MODE:
	                    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_CUSTOM_1;
	                    break;
	                case CUSTOM_SELF_DIAGNOSTIC_MODE:
	                    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_CUSTOM_2;
	                    break;
	                case CUSTOM_ACTIVE_HOTEL_MODE:
	                    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_CUSTOM_3;
	                      
	                    if (EEPDTV_GetCfg(&rDtvCfg) == 0)
	                    {
							if(rDtvCfg.u1Flags4 & DTVCFG_FLAG4_HOTEL_MODE)
							{
							    Printf("[%s]Already in hotel mode\n",__FUNCTION__); 
								break;
							}

	                        //store the menu setting. 
	                        if (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_DISABLE_LOGO)
	                        {
	                            rDtvCfg.u1Flags4 |= DTVCFG_FLAG4_DISABLE_LOGO_MENU_SET;
	                        }
	                        else
	                        {
	                            rDtvCfg.u1Flags4 &= ~DTVCFG_FLAG4_DISABLE_LOGO_MENU_SET;
	                        }
	                    
	                        rDtvCfg.u1Flags2 |= DTVCFG_FLAG2_DISABLE_LOGO;
							rDtvCfg.u1Flags4 |= DTVCFG_FLAG4_HOTEL_MODE;
	                        UNUSED(EEPDTV_SetCfg(&rDtvCfg));
	                    }
	                    break;
	                case CUSTOM_EXIT_HOTEL_MODE:
	                    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_CUSTOM_4;
	                       
	                    if (EEPDTV_GetCfg(&rDtvCfg) == 0)
	                    {
	                        if (rDtvCfg.u1Flags4 & DTVCFG_FLAG4_DISABLE_LOGO_MENU_SET)
	                        {
	                            rDtvCfg.u1Flags2 |= DTVCFG_FLAG2_DISABLE_LOGO;
	                        }
	                        else
	                        {
	                            rDtvCfg.u1Flags2 &= ~DTVCFG_FLAG2_DISABLE_LOGO;
	                        }
							rDtvCfg.u1Flags4 &= ~DTVCFG_FLAG4_HOTEL_MODE;
	                        UNUSED(EEPDTV_SetCfg(&rDtvCfg));
	                    }
	                    break;
	                case CUSTOM_HOTEL_VDA_MODE:
	                    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
	                    break;                      
	                default:
	                    //david: Need to get detail spec. to return correct reason 20090913
	                    if (rResponse.au1Data[0] == PDWNC_WAKE_UP_REASON_IRRC)
	                    {
	                        *pu4Reason = _SirRawKey2WakeupReason(rResponse.au1Data[1]);
	                        Printf("IR wakeup key code: 0x%2x, reason: %d\n",
	                               rResponse.au1Data[1], *pu4Reason);
	                    }
	                    else if (rResponse.au1Data[0] == T8032_WAKEUP_REASON_CEC_ONE_TOUCH_PLAY)
	                    {
	                        *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_HDMI;
	                    }                        
	                  /*  else if(rResponse.au1Data[0] == PDWNC_WAKE_UP_REASON_UART)
	                    {
	                        *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_UART;
							PDWNC_Custom_GetLocatelData();
							aLocatelSetting[7] = rResponse.au1Data[1];
	                    }*/
	                    else if(rResponse.au1Data[0] == PDWNC_WAKE_UP_REASON_KEYPAD)
	                    {                        
	                        //*pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_KEYPAD;    
							*pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
	                    }                    
	                    break;
	                }
				}
            }

            _fgEverReqWakeupReason = 1;
            _fgT8032WakeupReason = *pu4Reason;
            Printf("Wakeup reason from T8032 is %d\n", *pu4Reason);            
        }
        else
        {
            *pu4Reason = (UINT32)_fgT8032WakeupReason;
            Printf("Wakeup reason is %d\n", *pu4Reason);
        }

#else
        *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
#ifndef CC_MTK_IR_ENABLE
        //for capsense or software keypad, keypad wakeup is 8032 wakeup, and the wakeup reason is keypad
        PDWNC_T8032_CMD_T rCmd;
        PDWNC_T8032_RESPONSE_T rResponse;
        if (_fgLdrEverReqWakeupReason == 0)
        {
            x_memset(&rCmd, 0, sizeof(PDWNC_T8032_CMD_T));
            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_READ_WAKEUP_REASON;
            *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_IRRC;
            
            if (PDWNC_T8032Cmd(&rCmd, &rResponse) == 0)
            {
                Printf("PDWNC_Custom_ReadWakeupReason 8032, 0x%x 0x%x 0x%x\n", rResponse.au1Data[0], rResponse.au1Data[1], rResponse.au1Data[2]);
                if(rResponse.au1Data[0] == PDWNC_WAKE_UP_REASON_KEYPAD)
                {
                    *pu4Reason = (UINT32)PDWNC_WAKE_UP_REASON_KEYPAD;
                }
            }

            _fgLdrEverReqWakeupReason = 1;
            _fgLdrT8032WakeupReason = *pu4Reason;
        }
        Printf("LDR Wakeup reason from T8032 is %d\n", *pu4Reason);            
#endif        
#endif
    }

    bWURsnInit = FALSE;
    u4BootReason = *pu4Reason;
}
/*static void _enable_cbus(BOOL bEn)
{
    INT32 i4Ret = 0;
    UINT8 u1ValTmp = 0;

    if(bEn == 0)
        Printf("_____MHL CBUS Hi-Z\n");
    else
        Printf("_____MHL CBUS 1K ohm\n");

    i4Ret = SIF_X_Read(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1, 0x87, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____read error_____ 0xD0 0x87\n");
    }
    if(bEn)
    {
        u1ValTmp &= (~0x0C); //Set to 1K Ohm
    }
    else
    {
        u1ValTmp |= 0x0C; //Set to Hi-Z
    }

    i4Ret = SIF_X_Write(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1 , 0x87, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____write error_____ 0xD0 0x87\n");
    }
    else
    {
        Printf("_____write 0xD0 0x87 %d byte %x\n", i4Ret, u1ValTmp);
    }
    i4Ret = SIF_X_Read(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1, 0x76, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____read error_____ 0xD0 0x76\n");
    }
    if(bEn)
    {
        u1ValTmp &= (~0x0C); //Set to 1K Ohm
    }
    else
    {
        u1ValTmp |= 0x0C; //Set to Hi-Z
    }

    i4Ret = SIF_X_Write(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1 , 0x76, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____write error_____ 0xD0 0x76\n");
    }
    else
    {
        Printf("_____write 0xD0 0x76 %d byte %x\n", i4Ret, u1ValTmp);
    }
    i4Ret = SIF_X_Read(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1, 0x73, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____read error_____ 0xD0 0x73\n");
    }
    if(bEn)
    {
        u1ValTmp &= (~0x0C); //Set to 1K Ohm
    }
    else
    {
        u1ValTmp |= 0x0C; //Set to Hi-Z
    }

    i4Ret = SIF_X_Write(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1 , 0x73, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____write error_____ 0xD0 0x73\n");
    }
    else
    {
        Printf("_____write 0xD0 0x73 %d byte %x\n", i4Ret, u1ValTmp);
    }
}*/
#ifdef CC_SUPPORT_MHL
static void _enabe_terminal(BOOL bEn)
{
    INT32 i4Ret = 0;
    UINT8 u1ValTmp = 0;

    i4Ret = SIF_X_Read(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1, 0x70, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____read error_____ 0xD0 0x70\n");
    }

    if(bEn == 0)
    {
	    //Termination Disable
        u1ValTmp = (u1ValTmp & (~0x8B)) | 0x03;
        Printf("_____MHL Termination Disable\n");
    }
    else
    {
        //Termination Enable
        u1ValTmp = (u1ValTmp & (~0x8B)) | 0x08;
        Printf("_____MHL Termination Enable\n");
    }
    i4Ret = SIF_X_Write(SIL_SIF_BUS, 0x120, SIL_DEV_PAGE_0, 1 , 0x70, & u1ValTmp, 1);
    if (i4Ret == 0)
    {
        Printf("_____write error_____ 0xD0 0x70\n");
    }
    else
    {
        Printf("_____write 0xD0 0x70 %d byte %x\n", i4Ret, u1ValTmp);
    }
}
#endif

UINT8 u1DisableWakeupCnt = 0;
void PDWNC_Custom_SetupPowerDown(UINT32 u4PowerDownEvent, UINT32 *pu4PowerDownEn)
{
    UINT32 u4Addr;
    UINT32 u4Val2,u4Val3;
    UINT8 au1Data[6] = {0,0,0,0,0,0};
    PDWNC_T8032_CMD_T rCmd;
#ifdef CC_Factory2K12
    UINT8 u1Data;
    INT32 i4Val;
    PD_UART_BR_ENUM u1PD_UART_BR = 0;
#endif
    PDWNC_Custom_EnterNextState(STATE_STANDBY);//d130703_Haibo:Update status before standby
	if(_fgIsNormalMode)//d130905_Haibo:skip the led control when now is semi standby
	{
#ifdef CC_MTK_LOADER//d130709_Haibo:FIX DTV00543062 [MS][Stellar_EU]Green LED light was still on after IR power off.	
    GPIO_CustomLedCtrlFunc(eLedLoaderPowerOff);
#else
    GPIO_CustomLedCtrlFunc(eLedSystemPowerOff);
#endif
	}
	
#if defined(CC_SUPPORT_STR_CORE_OFF) && defined(SUSPEND_GPIO_NUMBER)	
	bWURsnInit = TRUE;
	#if !defined(CC_MTK_LOADER) && !defined(CC_MTK_IR_ENABLE)
	_fgEverReqWakeupReason = 0;
	#endif
	{
	 if(PDWNC_IsSuspend())
	 {
	   Printf("Set %d as 1\n",SUSPEND_GPIO_NUMBER);
	   GPIO_SetOut(SUSPEND_GPIO_NUMBER,1);
	 }
	}
#endif

    UNUSED(u4PowerDownEvent);
    UNUSED(pu4PowerDownEn);  

	

#ifdef CC_IR_POWER_BUTTON_RECEIVE_PRELOADER
    Printf("T8032 will disable HW IRRX wakeup!\n");
#endif
    au1Data[0] = au1Data[1] = au1Data[2] = au1Data[3] = au1Data[4] = au1Data[5] = 0;
    // If the next power state is standby, enable CEC.
    // Otherwise, disable CEC
#ifdef CUSTOM_POWER_STATE_CTRL_ENABLE
    if (_u4PowerState == STATE_STANDBY)
#endif /* CUSTOM_POWER_STATE_CTRL_ENABLE */        
    {
        // Write CEC configuration to uP
        if((DRVCUST_InitGet(eT8032uPOffset) != 0) && ((u4Addr = DRVCUST_InitGet(eEepromCECMenuLanguageOffset)) != 0x0) &&
        ((DRVCUST_InitGet(eEepromCECEnableOffset)) == 0x00) && ((DRVCUST_InitGet(eEepromCECOneTouchPlayEnableOffset)) == 0x00))
        {
            EEPROM_Read(u4Addr, (UINT32)au1Data, 6);		
            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_MENU_LANGUAGE;
            rCmd.u1SubCmd = 0;
            x_memcpy(rCmd.au1Data, au1Data, 4);
            PDWNC_T8032Cmd(&rCmd, NULL);

            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_CEC;
            x_memcpy(rCmd.au1Data, &au1Data[4], 1);
            PDWNC_T8032Cmd(&rCmd, NULL);

            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_ONE_TOUCH_PLAY;
            x_memcpy(rCmd.au1Data, &au1Data[5], 1);
            PDWNC_T8032Cmd(&rCmd, NULL);		
        }
        else if((DRVCUST_InitGet(eT8032uPOffset) != 0) && ((u4Addr= DRVCUST_InitGet(eEepromCECMenuLanguageOffset)) != 0x0) &&
                ((u4Val2 = DRVCUST_InitGet(eEepromCECEnableOffset)) != 0x00) && ((u4Val3 = DRVCUST_InitGet(eEepromCECOneTouchPlayEnableOffset)) != 0x00))
        {
            EEPROM_Read(u4Addr, (UINT32)au1Data, 4);		
            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_MENU_LANGUAGE;
            rCmd.u1SubCmd = 0;
            x_memcpy(rCmd.au1Data, au1Data, 4);
            PDWNC_T8032Cmd(&rCmd, NULL);
		
            EEPROM_Read(u4Val2, (UINT32)au1Data, 1);		  
            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_CEC;
            au1Data[1]=0;//----youneng--0427
            au1Data[2]=0;//----youneng--0427
            au1Data[3]=0;//----youneng--0427
            x_memcpy(rCmd.au1Data, au1Data, 4);//----youneng--0427
            PDWNC_T8032Cmd(&rCmd, NULL);

            EEPROM_Read(u4Val3, (UINT32)au1Data, 1);		
            rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_ONE_TOUCH_PLAY;
            x_memcpy(rCmd.au1Data, au1Data, 1);
            PDWNC_T8032Cmd(&rCmd, NULL);		
        }
	
        rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_T8032_CTL_CEC; //Notify T8032 start/stop control CEC
        rCmd.u1SubCmd = 0;
        rCmd.au1Data[0] = 1;	//T8032 control CEC
        PDWNC_T8032Cmd(&rCmd, NULL);
    }

    rCmd.u1Cmd = PDWNC_CMD_ARMT8032_IR_WAKEUP_KEY;
	
#if defined(IR_US)
	    rCmd.u1SubCmd = (UINT8)T8032_IR_WAK_POWER;
#elif defined(IR_PAA)//pure analog
    rCmd.u1SubCmd = (UINT8)(T8032_IR_WAK_CH_UP_DOWN | T8032_IR_WAK_POWER | 
                            T8032_IR_WAK_NUMERIC | T8032_IR_WAK_ATV);
#else
    rCmd.u1SubCmd = (UINT8)(T8032_IR_WAK_POWER | T8032_IR_WAK_CH_UP_DOWN |
                            T8032_IR_WAK_NUMERIC | T8032_IR_WAK_DTV |
                            T8032_IR_WAK_ATV | T8032_IR_WAK_ADTV_SW);
#endif
#ifdef CC_HOTKEY_WAKEUP_SUPPORT
//add hot key SW wakeup support
     rCmd.u1SubCmd |= T8032_IR_WAK_HOT_KEY;
#endif

	if(u4IO32Read4B(PDWNC_RESRV2) == 0xaaaaaaaa)
	{
		printf("Overwrite IR wakeup key 0x%2x\n", (UINT8)(u4IO32Read4B(PDWNC_RESRV3)));
		rCmd.u1SubCmd = (UINT8)(u4IO32Read4B(PDWNC_RESRV3));
	}

    printf("Set IR wakeup key 0x%2x\n", rCmd.u1SubCmd);
    rCmd.au1Data[0] = rCmd.au1Data[1]= rCmd.au1Data[2] = rCmd.au1Data[3] = 0;	 
    PDWNC_T8032Cmd(&rCmd, NULL);

/*******************************************************/
#ifdef CC_Factory2K12
/*
T8032 command layout arrangement
SubCmd (blinking led information):
bit7 ==> 1: servo gpio; 0: opctrl gpio
bit6 ==> Led On polarity 
bit0~5 ==> gpio offset

au1Data[0] 
bit 0 ==> u1CustomData3_Module1ReplyData, NVM 0x1EC0[0], eeprom_data2,  Module1(VDA) check this bit to decide whether wakeup while receive wakeup command
bit 1 ==> u1Module2Enable, NVM 0x11BF[0],  check this bit to decide whether go to module2 (Locatel) while NVM 0x6C8 == 4
bit 2 ==> u1hotel_module2_init_mode, HTLSI_GetOnlineMode(),  check this bit to decide the initial operation mode while go to module2(Locatel)
bit 3 ==> debug mode, T8032 output debug message and only execute the uart command but not repond any data
bit 4~7 ==> u1HotelModeModule, NVM 0x6C8, eeprom_data1, Select Hotel mode (2,3 is module1 (VDA); 4 is module2 (Locatel))

au1Data[1] ==> u1Module2AvSelectPW, NVM 0x1EC6, password for enable the function of wakeup with source select
au1Data[2] ==> u1Module2HdmiPortSel, NVM 0x1EC7, HDMI port select while wakeup source select is HDMI
au1Data[3] ==> u1Module2AvSelectSrc, NVM 0x1EC8, wakeup source select
*/
    rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_CUSTOM_DATA;
    i4Val = 0;
	rCmd.au1Data[0] = 0;

    EEPROM_Read(CUSTOM_EMCS_HOTEL_STANDBY_MODE_ADDR_CMD_ENABLE, (UINT32)&i4Val, 1);	
	if((UINT8)i4Val & 0x1)
	{
		rCmd.au1Data[0] |= 0x1;
	}

    EEPROM_Read(CUSTOM_EMCS_HOTEL_ENABLE, (UINT32)&i4Val, 1);	
	if((UINT8)i4Val & 0x1)
	{
		rCmd.au1Data[0] |= 0x2;
	}
#ifndef CC_MTK_LOADER
    if(HTLSI_GetOnlineMode((UINT8*)&u1Data) == TRUE)
    {
		if(u1Data)
		{
			rCmd.au1Data[0] |= 0x4;
		}
    }
	else
	{
		Printf("HTLSI_GetOnlineMode fail, force set hotel online mode as 0\n"); 		   
	}
#endif
	if(u4IO32Read4B(PDWNC_RESRV0) == 0xa5a5a5a5)
	{
		rCmd.au1Data[0] |= 0x8;
	}

	
    if(UART_SELECTION_IS_HOTEL())
    {
        u1PD_UART_BR = PD_UART_BR_9600;
		rCmd.au1Data[0] |= (u1Data << 4) & 0xf0;
    }

    EEPROM_Read(CUSTOM_EMCS_HOTEL_STANDBY_MODE_ADDR_MAX_PROG, (UINT32)&i4Val, 1);	
	rCmd.au1Data[1] = (UINT8)i4Val;	
	
    EEPROM_Read(CUSTOM_EMCS_HOTEL_STANDBY_MODE_ADDR_SOURCE_SUB, (UINT32)&i4Val, 1);	
	rCmd.au1Data[2] = (UINT8)i4Val;	

    EEPROM_Read(CUSTOM_EMCS_HOTEL_STANDBY_MODE_ADDR_SOURCE_MAIN, (UINT32)&i4Val, 1);	
	rCmd.au1Data[3] = (UINT8)i4Val;	

    //no CUSTOM_GPIO_LED_TIMER_AMBER, use CUSTOM_GPIO_LED_STANDBY instead
    if(CUSTOM_GPIO_LED_STANDBY >= SERVO(0)) 
    {
        rCmd.u1SubCmd = (CUSTOM_GPIO_LED_STANDBY - SERVO(0)) & 0x3f;    
    }
    else if(CUSTOM_GPIO_LED_STANDBY >= OPCTRL(0)) 
    {
        rCmd.u1SubCmd = (CUSTOM_GPIO_LED_STANDBY - OPCTRL(0)) & 0x3f;            
    }
    else
    {
        rCmd.au1Data[0] = 0;
        u1PD_UART_BR = PD_UART_BR_115200;
        Printf("CUSTOM_GPIO_LED_TIMER_AMBER gpio invalid, force disable hotel mode\n");
    }
    if(CUSTOM_LED_POWER_ON_VAL)
    {
        rCmd.u1SubCmd |= 0x40;
    }
    if(CUSTOM_GPIO_LED_STANDBY >= SERVO(0))
    {
        rCmd.u1SubCmd |= 0x80;
    }

    Printf("pass eeprom data to T8032 ");  

//#define S_HOTEL_MODE_DEBUG
#ifdef S_HOTEL_MODE_DEBUG
Printf("ERROR!!!DEBUG Hotel mode setting!!!\n");
rCmd.au1Data[0] = 4;
rCmd.au1Data[1] = 0;
rCmd.au1Data[2] = 1;
rCmd.au1Data[3] = 1;
#endif

	if(0)//if(u4IO32Read4B(PDWNC_RESRV1) == 0xa5a5a5a5)
	{
		rCmd.au1Data[0] = (UINT8)(u4IO32Read4B(PDWNC_RESRV2) & 0xff);
		rCmd.au1Data[1] = (UINT8)((u4IO32Read4B(PDWNC_RESRV2) & 0xff00) >> 8);
		rCmd.au1Data[2] = (UINT8)((u4IO32Read4B(PDWNC_RESRV2) & 0xff0000) >> 16);
		rCmd.au1Data[3] = (UINT8)((u4IO32Read4B(PDWNC_RESRV2) & 0xff000000) >> 24);
		u1PD_UART_BR = PD_UART_BR_9600;
	}

    if (PDWNC_T8032Cmd(&rCmd, NULL) == 0)
    {
        Printf("OK (0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x)\n",rCmd.u1SubCmd, rCmd.au1Data[3], rCmd.au1Data[2] , rCmd.au1Data[1], rCmd.au1Data[0] );
    }
    else
    {
        Printf("failed!!\n");
    }

    printf("Set PD UART BR as %d\n", u1PD_UART_BR);
    PDWNC_SetUartPDBaudrate((UINT8)u1PD_UART_BR);
#endif //#ifdef CC_Factory2K12
/******************************************************/     
#if 1
    rCmd.u1Cmd = PDWNC_CMD_ARMT8032_DELAY_WAKEUP_TIMER;
    if(u1DisableWakeupCnt % 2)
    {
        rCmd.u1SubCmd = u1DisableWakeupCnt;// 200 ==> 4 seconds (unit 20ms)
    }
    else
    {
        rCmd.u1SubCmd = 50;// ==> 100ms (unit 20ms)
    }
    printf("Disable Wakeup %d ms\n", (rCmd.u1SubCmd) * 20);
    rCmd.au1Data[0] = (UINT8)((*pu4PowerDownEn) & 0x0000000ff);
    rCmd.au1Data[1] = (UINT8)(((*pu4PowerDownEn) >> 8) & 0x0000000ff);
    rCmd.au1Data[2] = (UINT8)(((*pu4PowerDownEn) >> 16) & 0x0000000ff);        
    rCmd.au1Data[3] = (UINT8)(((*pu4PowerDownEn) >> 24) & 0x0000000ff);            
    if(rCmd.u1SubCmd != 0)
    {
        *pu4PowerDownEn = 0;
    }
    PDWNC_T8032Cmd(&rCmd, NULL);
#else
    vIO32Write4B(PDWNC_OPDELAY, WAKEUP_DISABLE_SEC_CNT(1));
#endif
    vIO32WriteFldAlign(PDWNC_SADCCFG0, 0x1, FLD_RG_SRV_PWD_B);
    vIO32WriteFldAlign(PDWNC_SADCCFG0, 0x0, FLD_RG_SRV_INBUFF_PD);
    vIO32WriteFldAlign(PDWNC_CLKPDN, 0x0, FLD_SRVPD);
    vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x1, FLD_HWEN);

    PDWNC_InitServoADC();
#if !defined(CC_MTK_LOADER) & defined (CUSTOM_SELF_DIAG_ENABLE)
    SelfDiagUpdateOnOffCount();
#endif /* !defined(CC_MTK_LOADER) & defined (CUSTOM_SELF_DIAG_ENABLE) */

#ifdef S_HOTEL_MODE_DEBUG
    printf("Set T8032 Debug message levle as 1\n");
    PDWNC_SetUartPDBaudrate(1);
    x_thread_delay(1000);    
#endif
    return;
}

void PDWNC_Custom_ErrorCode(UINT8 u1ErrorCode)
{
    UNUSED(u1ErrorCode);
    return;
}

void PDWNC_Custom_GetCurrentState(UINT32 *pu4CurrentState)
{
    UINT64 u8Offset;
    UINT8  u1Val = 0;
    DTVCFG_T rDtvCfg;

    *pu4CurrentState = STATE_POWER_ON;
    u8Offset = (UINT64)CUSTOM_POWER_STATE_ADDR;

    if (EEPROM_Read(u8Offset, (UINT32)(void *)&u1Val, 1) == 0)
    {
        *pu4CurrentState = (UINT32)u1Val;
        if (*pu4CurrentState == STATE_AC_OFF)
        {
            *pu4CurrentState = STATE_POWER_ON;
        }
    }

    // Handel the power state in the hotel mode
    x_memset(&rDtvCfg, 0, sizeof(DTVCFG_T));
                
    if (EEPDTV_GetCfg(&rDtvCfg) == 0)
    {
        if (rDtvCfg.u1Flags & DTVCFG_FLAG_POWERON_BOOT)
        {
            _fgIsAlwaysPowerOn = TRUE;
        }
        
        if (rDtvCfg.u1Flags & DTVCFG_FLAG_POWERON_LAST)
        {
            _fgIsLastSts = TRUE;
        }
        
        if (rDtvCfg.u1Flags4 & DTVCFG_FLAG4_POWERON_CONFIGURED)
        {
            _fgIsConfiguredByAP = TRUE;
        }
    }

    _u4PowerState = *pu4CurrentState;
    Printf("[%s]Current state : 0x%x,%d %d %d\n", __FUNCTION__,*pu4CurrentState,_fgIsAlwaysPowerOn,_fgIsLastSts,_fgIsConfiguredByAP);
}

void PDWNC_Custom_DetermineNextState(UINT32 u4CurrentState, UINT32 u4PowerEvent,
                                    UINT32 *pu4NextState, UINT32 *pfgEnterStandby,
                                    UINT32 *pu4PowerDownEnable)
{
#if 0
    if (CustomPCBAQueryCountry() == DRV_CUSTOM_COUNTRY_EU)
    {
        PDWNC_EU_DetermineNextState(u4CurrentState, u4PowerEvent, pu4NextState, 
                                    pfgEnterStandby, pu4PowerDownEnable);
    }
    else
    {
        PDWNC_PAD_DetermineNextState(u4CurrentState, u4PowerEvent, pu4NextState, 
                                     pfgEnterStandby, pu4PowerDownEnable);
    }
#else
    //spec change
    PDWNC_PAD_DetermineNextState(u4CurrentState, u4PowerEvent, pu4NextState, 
                            pfgEnterStandby, pu4PowerDownEnable);

#endif
}
#ifndef CC_MTK_LOADER
static UINT8 g_u1Recording;
void vUpdateRecStatus(UINT8 u1Rec)
{
   g_u1Recording=u1Rec;
   Printf("Record %s\n",g_u1Recording?"Start":"Stop");
}
#endif

void PDWNC_Custom_EnterNextState(UINT32 u4NextState)
{
    UINT8  u1StateCtrl;
    UINT64 u8Offset;
    #ifdef BUTTON_POWER_KEY_SRV_NUM
    UINT8  u1ServValue = 0;
    #endif
#ifndef CC_MTK_LOADER
    CRIT_STATE_T rCritState;
#endif /* CC_MTK_LOADER */
    
	
    u1StateCtrl = (UINT8)u4NextState;
    u8Offset = (UINT64)CUSTOM_POWER_STATE_ADDR;
    if (EEPROM_Write(u8Offset, (UINT32)&u1StateCtrl, 1))
    {
        Printf("Write EEPROM fail at PDWNC_Custom_EnterNextState : u4NextState = 0x%x\n", u4NextState);
    }

#ifndef CC_MTK_LOADER
    rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */
    _u4PowerState = u4NextState;
#ifndef CC_MTK_LOADER
    x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */

    switch (u4NextState)
    {
    case STATE_STANDBY:
    case STATE_POWER_OFF:
#ifdef CC_MTK_LOADER        
        LOADER_Custom_TimerOff();
#else
        CustomIRLedTimerStop();
#endif /* CC_MTK_LOADER */

        /*
        [ID_1927] [STL] Power \ TV received power commend twice when long pressing
                        Unit <POWER> key under TV is standby status
        DTV00088050
        */
        if (u4NextState == STATE_POWER_OFF)
        {
#ifdef BUTTON_POWER_KEY
            while (GPIO_GetIn(BUTTON_POWER_KEY) == 0);
#endif
#ifdef BUTTON_POWER_KEY_SRV_NUM
            while(1)
            {
                u1ServValue = PDWNC_ReadServoADCChannelValue(BUTTON_POWER_KEY_SRV_NUM);
                if (u1ServValue < POWER_KEY_SRV_AD_LOW_THR || u1ServValue > POWER_KEY_SRV_AD_HI_THR)
                {
                    break;
                }
             }
#endif
        }
        break;

    case STATE_POWER_ON:
#ifdef CC_MTK_LOADER    		
        GPIO_CustomLedCtrlFunc(eLedLoaderPowerOn);
#else
        if(!g_u1Recording)
        {
		 GPIO_CustomLedCtrlFunc(eLedSystemPowerOn);
        }
		else
		{
		 Printf("Now is Recording!!\n");
		}
#endif		
        break;

	case STATE_SEMI_STANDBY:
		Printf("@@@@@@@@enter into semi standby\n");
		_fgIsNormalMode = FALSE;
		break;
		
    default:
        break;
    }

#if 0//ndef CC_MTK_LOADER 
#ifdef CC_SUPPORT_MHL
    INT32 i4Ret = -1;
    UINT32 u4Buf = 0x0;
	UINT32 u4Val =0;;
    fgSil_MHL_Enable_MLoop(FALSE);
    fgSil_ChipRst(10);
	if((u4Buf = DRVCUST_InitGet(eEepromMHLStbChargingCtrlOffset)) != 0x0)
    {
        Printf("[MHL power down]get EEP offset=%d\n",u4Buf);
        i4Ret = EEPROM_Read((UINT64)u4Buf,(UINT32)&u4Val, 1);
        if (i4Ret != 0)
        {
            u4Val=0;
            Printf("_____read error_____\n");
        }
        else
        {
            Printf("____read eep OK,u4Val=%d________\n",u4Val);
        }
    }

    if(u4Val == 0)
    {
        _enabe_terminal(FALSE);
       GPIO_SetOut(MHL_GPIO_VBUS_EN,1);
    }
	else if(u4Val == 1)
	{
	   GPIO_SetOut(MHL_GPIO_VBUS_EN,0);
	}
	
#endif
#endif
    Printf("Enter Next State (u4NextState = 0x%x)\n", u4NextState);
}

UINT32 PDWNC_Custom_CheckPowerState(void)
{
    return _u4PowerState;
}

BOOL CustomCheckCECCtrlState(void)
{
    if (_u4PowerState == STATE_POWER_OFF)
    {
        return FALSE;   // Disable CEC
    }

    // Others -> enable CEC
    return TRUE;
}

//-----------------------------------------------------------------------------
/** PDWNC_SetErrorCode to set system error code.
 *  @param u1ErrorCode the error code.
 * au1Data[0] : Servo_gpio (1) or OPCTRL (0)
 * au1Data[1] : offset gpio number of servo or opctrl
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_SetSonyErrorCode(UINT8 u1Period, UINT8 u1Interval)
{
    PDWNC_T8032_CMD_T rCmd;
    UINT32 u4T8032CtrlGpio, u4OnVal;
    rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_ERROR_CODE;//PDWNC_CMD_ARMT8032_SET_SONY_ERROR_CODE;
//    rCmd.u1SubCmd = u1ErrorCode;
    rCmd.au1Data[0] = rCmd.au1Data[1]= rCmd.au1Data[2] = rCmd.au1Data[3] = 0;    
/*    
    if(DRVCUST_InitGet(eT8032CtrlErrLed) && DRVCUST_InitGet(eT8032CtrlErrLedOnVal))
    {
*/    
       DRVCUST_InitQuery(eT8032CtrlErrLed, &u4T8032CtrlGpio);
       DRVCUST_InitQuery(eT8032CtrlErrLedOnVal, &u4OnVal);
       Printf("Set t8032 ctrl gpio %d (On val: %d, Para1: %d, Para2: %d)\n", u4T8032CtrlGpio, u4OnVal, u1Period, u1Interval);
       if(u4T8032CtrlGpio >= SERVO(0))
       {
            u4T8032CtrlGpio -= SERVO(0);
            rCmd.u1SubCmd = 1; // TYPE (1: SERVO, 0: OPCTRL)
            rCmd.au1Data[0] = (UINT8)u4T8032CtrlGpio; //OFFSET GPIO INDEX
            rCmd.au1Data[1] = (UINT8)u4OnVal; // POLARITY
            rCmd.au1Data[2] =  u1Period;
            rCmd.au1Data[3] =  u1Interval;             
       }
       else if(u4T8032CtrlGpio >= OPCTRL(0))
       {
            rCmd.u1SubCmd = 0; // TYPE (1: SERVO, 0: OPCTRL)
            u4T8032CtrlGpio -= OPCTRL(0);           
            rCmd.au1Data[0] = (UINT8)u4T8032CtrlGpio; //OFFSET GPIO INDEX
            rCmd.au1Data[1] = (UINT8)u4OnVal; // POLARITY
            rCmd.au1Data[2] =  u1Period;
            rCmd.au1Data[3] =  u1Interval;             
       }
       else
       {
            Printf("Set t8032 ctrl gpio error (gpio index error)!!!\n");
            return -1;
       }
       return PDWNC_T8032Cmd(&rCmd, NULL);
/*	   
    }
    else
    {
        Printf("Set t8032 ctrl gpio error (no setting) !!!\n");
        return -1;
    }
*/    
}

//-----------------------------------------------------------------------------
/** PDWNC_SetErrorCodeBlinking to enable system error code blinking function.
 *  @param u1ErrorCode the error code.
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_SetSonyErrorCodeBlinking(UINT8 u1times)
{
    PDWNC_T8032_CMD_T rCmd;
    rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_ERROR_CODE_BLINKING;
    rCmd.u1SubCmd = u1times;
    rCmd.au1Data[0] = rCmd.au1Data[1]= rCmd.au1Data[2] = rCmd.au1Data[3] = 0;    
    return PDWNC_T8032Cmd(&rCmd, NULL);
}


INT32 PDWNC_Custom_Set8032uPLedBlink(UINT32 u4BlinkTimes)
{
    UNUSED(PDWNC_SetSonyErrorCode(CUSTOM_BLINK_TIME, CUSTOM_BLINK_INVERAL));
    return (PDWNC_SetSonyErrorCodeBlinking(u4BlinkTimes));
}

UINT32 CustomQueryPowerState(void)
{
    static BOOL bInit = TRUE;
    UINT32 u4State;
    
    if (bInit)
    {
        PDWNC_Custom_GetCurrentState(&u4State);
        bInit = FALSE;
    }
    else
    {
#ifndef CC_MTK_LOADER
        CRIT_STATE_T rCritState;
    
        rCritState = x_crit_start();
#endif /* CC_MTK_LOADER */

        u4State = _u4PowerState;

#ifndef CC_MTK_LOADER
        x_crit_end(rCritState);
#endif /* CC_MTK_LOADER */
    }
    
    return u4State;
}

#ifdef CC_MTK_LOADER
void LOADER_Custom_TimerOn(void)
{
    UINT32 u4Val;

    // Enable timer 0
    u4Val = BIM_READ32(REG_RW_TIMER_CTRL);
    u4Val |= ((TMR1_CNTDWN_EN) | (TMR1_AUTOLD_EN));
    BIM_WRITE32(REG_RW_TIMER_CTRL, u4Val);
}

void LOADER_Custom_TimerOff(void)
{
    UINT32 u4Val;

    // disable timer 0
    u4Val = BIM_READ32(REG_RW_TIMER_CTRL);
    u4Val &= ~((TMR1_CNTDWN_EN) | (TMR1_AUTOLD_EN));
    BIM_WRITE32(REG_RW_TIMER_CTRL, u4Val);    
#ifdef CC_SUPPORT_UPGRADE_ENABLE_BY_IR 
   if(BIM_IsIrqEnabled(VECTOR_PDWNC))
   {
   BIM_DisableIrq(VECTOR_PDWNC);
   BIM_ClearIrq(VECTOR_PDWNC);
   vIO32Write4B(PDWNC_INTCLR, _PINT(PDWNC_INTNO_IRRX));
   vIO32WriteFldAlign(PDWNC_IRCLR, 0x01, FLD_IRCLR);
  }
#endif  
}
#ifdef CC_SUPPORT_UPGRADE_ENABLE_BY_IR	
static UINT16 au2IrRxData[3]={0};
static UINT16 u2Idx=0;
#endif
static void LOADER_Custom_TimerIsr(void)
{
    if (BIM_IsIrqPending(VECTOR_T1))
    {
        D_LED_TYPE eLedType;
        if (_fgLedAllBlink)
        {
            CustomLEDBlinkCtrl(D_LED_TOTAL);
        }
        else
        {
            for (eLedType = D_LED_POWER; eLedType <= D_LED_TIMER; eLedType++)
            {
                if (_fgLedBlink[eLedType] == TRUE)
                {
                    CustomLEDBlinkCtrl(eLedType);
                }
            }
        }
        //_NonOSTimerOff();        
        BIM_ClearIrq(VECTOR_T1);        
    }
#ifdef CC_SUPPORT_UPGRADE_ENABLE_BY_IR	
    else if(BIM_IsIrqPending(VECTOR_PDWNC))
	{
	    UINT8 u1IrCnt=0;
	    if(u4IO32Read4B(PDWNC_INTSTA) & _PINT(PDWNC_INTNO_IRRX))
        {
             /* read IRRX IRQ data. */
			 u1IrCnt=u1IO32Read1B(PDWNC_IRCNT);
			 if((u1IrCnt==0x0c) || (u1IrCnt==0x0f))
			 {
			    au2IrRxData[((u2Idx++)%3)]=(UINT16)u4IO32Read4B(PDWNC_IRDATA0);
			 }
            // au4IrRxData[1] = u4IO32Read4B(PDWNC_IRDATA1);
        }
		BIM_ClearIrq(VECTOR_PDWNC);
	    vIO32Write4B(PDWNC_INTCLR, _PINT(PDWNC_INTNO_IRRX));
	    vIO32WriteFldAlign(PDWNC_IRCLR, 0x01, FLD_IRCLR);
	}
#endif	
    if (_pfnCustomNextIsr) { _pfnCustomNextIsr(); }
}
#ifdef CC_SUPPORT_UPGRADE_ENABLE_BY_IR	
BOOL IsIRRXRecved(UINT16 u2KeyCode)
{
   UINT8 u1Idx=0;
   BOOL fgRet=TRUE;
   Printf("%s\n",__FUNCTION__);
   BIM_DisableIrq(VECTOR_PDWNC);
   BIM_ClearIrq(VECTOR_PDWNC);
   vIO32Write4B(PDWNC_INTCLR, _PINT(PDWNC_INTNO_IRRX));
   vIO32WriteFldAlign(PDWNC_IRCLR, 0x01, FLD_IRCLR);
   
   while(u1Idx<sizeof(au2IrRxData)/sizeof(au2IrRxData[0]))
   {
      Printf("IR_Data:%x\n",au2IrRxData[u1Idx]);
	  if(u2KeyCode==au2IrRxData[u1Idx])
	  {
	     fgRet=FALSE;
	  }
	  u1Idx++;
   }
   return fgRet;
}
#endif
#endif /* CC_MTK_LOADER */

void LOADER_Custom_EnvInitFunc(void *prLdrData, UINT32 fgValidFlag)
{
    UINT32 u4Val = 0;
	UINT32 u4Gpio,u4GpioPolarity;
#ifdef CC_MTK_LOADER	
	BOOL fgNoLogMode=FALSE;
#endif	
    if (fgValidFlag == 0)
    {
#ifdef PINMUX_INCLUDE_FILE
        //#include PINMUX_INCLUDE_FILE
#endif
//Printf("\n[david] hang for ICE...\n");
// while(u4Val);
// u4Val = 0;


#if 1
#ifdef CC_COMBINE_4K_2K
		if((DRVCUST_Get_2K_Model()&0x01)==0x01)
		{
			GPIO_SetOut(219,1);
		}
#endif
        if(sizeof(_gu4SysPinmuxInit))
        {
            for(u4Val = 0;u4Val < sizeof(_gu4SysPinmuxInit)/(sizeof(_gu4SysPinmuxInit[0])); u4Val++)
            {
                BSP_PinSet(_gu4SysPinmuxInit[u4Val][0], _gu4SysPinmuxInit[u4Val][1]);
            }
        }
    //reset hub
    if(0 == DRVCUST_InitQuery(eUSBHubResetGpio, &u4Gpio))
	{
	    if(0 == DRVCUST_InitQuery(eUSBHubResetPolarity, &u4GpioPolarity))
		{
		    GPIO_SetOut((INT32)u4Gpio,u4GpioPolarity?1:0);
            HAL_Delay_us(20000);
            GPIO_SetOut((INT32)u4Gpio, u4GpioPolarity?0:1);
            Printf("Hub:Set GPIO%d=%d.\n",u4Gpio,u4GpioPolarity?0:1);			
		}
	}
	 
#endif
#if 0
{
              INT32 fgSet=0;
        	  GPIO_Output(PANEL_I2C_SDA_GPIO,&fgSet);
			  GPIO_Output(PANEL_I2C_SCL_GPIO,&fgSet);//Keep PANEL_I2C GPIO LOW before panel on
}	//Printf("PWM for ESD\n");
#endif
#ifdef CC_AUD_SUPPORT_FIGUS
        GPIO_SetOut(115,0);   //AMP_RST
#endif
#ifdef SIGNAL_X_U_RST
        INT32 i4Value = 1;
        UNUSED(GPIO_Output(SIGNAL_X_U_RST, &i4Value));
        PRINT_TIMESTAMP_DBG_MSG("[Power On]", "X_U_RST = %d\n", i4Value);
#endif
#ifdef NDEBUG
        if(UART_SELECTION_IS_UART_OFF())
		{
			//Printf("Turn off UART log!\n");
			BSP_PinSet(PIN_U0TX, 2);
			BSP_PinSet(PIN_U0RX, 2);
			//SerTransparent();
		}		
#endif
#if 0
        // PWM0 for ESD
        // Calculation       
        UINT32 u4Clock, u4TimeBase;
        UINT32 u4NewFreq, u4Freq;
        UINT32 u4Duty, u4DutyOn, u4DutyAll;
        
        u4Freq = CUSTOM_ESD_PWM_FREQ;
        u4Duty = CUSTOM_ESD_PWM_DUTY_CYLE;
        u4Clock = BSP_GetDomainClock(SRC_BUS_CLK);
        u4TimeBase = ((u4Clock >> 8) / u4Freq);//u4TimeBase = ((float)(u4Clock >> 8) / (float)u4Freq);
        u4NewFreq = (u4Clock >> 8) / (u4TimeBase + 1);
        u4DutyAll = (u4NewFreq * 255) / u4Freq;
        u4DutyOn = (u4DutyAll * u4Duty) / 100;
    
        vDrvSetPWM(CUSTOM_ESD_PWM_PORT, u4TimeBase, u4DutyOn, u4DutyAll);
#else
      //  BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), (UINT32)0x80);        
#endif	

#ifdef CC_SOEM_BOARD//disable SIRCS internal pull-up
     vIO32WriteFldAlign(PDWNC_PADCFG4,0,FLD_LED_PWM1_PU);
#endif	
       
        return;
    }

#ifdef CC_MTK_LOADER
    UINT32 u4Value = PDWNC_ReadWakeupReason();
#ifdef CC_SUPPORT_UPGRADE_ENABLE_BY_IR		
	Custom_InitGPIOTimer();
#endif	
    if ((u4Value == PDWNC_WAKE_UP_REASON_AC_POWER) ||
        (u4Value == PDWNC_WAKE_UP_REASON_WATCHDOG))
    {
#ifdef LOAD_PANEL_FROM_FLASH_CUSTOM	
	  LoadPanelIndex();
#endif    
        LOADER_DisableHdmiDdc();
        vInternalEDIDInit();
    }
    LOADER_EnableHdmiDdc();
#ifdef CC_COMBINE_4K_2K
	if((DRVCUST_Get_2K_Model()&0x01)==0x01)
	{
		GPIO_SetOut(219,0);
	}
#endif
	//BSP_PinSet(PIN_HDMI_2_HPD_CBUS,2);
    
	fgNoLogMode=(BSP_PinGet(PIN_U0RX)!=0);
	if(fgNoLogMode && (u4Value == PDWNC_WAKE_UP_REASON_AC_POWER))
	{
	    BSP_PinSet(PIN_U0RX, 0);//swtich uart rx to normal function to wait cli input  
		if (LDR_IsEnterCli(100))
		{
		    BSP_PinSet(PIN_U0TX, 0);//swtich uart tx to normal function to output something
			CLI_Init();
			CLI_Input(NULL);
		}
		BSP_PinSet(PIN_U0TX, 2);
		BSP_PinSet(PIN_U0RX, 2);
	}
	
    if ((u4Value == PDWNC_WAKE_UP_REASON_AC_POWER) &&
        (CustomPCBAQueryCountry() == DRV_CUSTOM_COUNTRY_COL))//d130802_Haibo:for col in hotel mode,it can pwoer on/standby
    {
        LDR_DATA_T *prLoaderData = prLdrData;
        BOOL bIsBoot = FALSE;
        
        bIsBoot = (prLoaderData->rDtvCfg.u1Flags & DTVCFG_FLAG_POWERON_BOOT) ? 
                  TRUE : FALSE;
        if (!bIsBoot)
        {
            // Always enter the power down state in COL model
            PDWNC_EnterPowerDown((WAK_IRRX | WAK_UNOR), PDWNC_POWER_DOWN_IRRC);
        }
    }
#endif /* CC_MTK_LOADER */

    //GPIO_CustomReadStrapPin();
#ifdef CUSTOM_SELF_DIAG_ENABLE
    if (CustomCheckShutdown())
    {
#ifdef CC_MTK_LOADER
		// Detect if user wants to enter CLI mode
		if (LDR_IsEnterCli(100))
		{
			CLI_Init();
			CLI_Input(NULL);
		}
#endif        
        PDWNC_EnterPowerDown((WAK_IRRX | WAK_UNOR), PDWNC_POWER_DOWN_IRRC);
    }
#endif /* CUSTOM_SELF_DIAG_ENABLE */

	
    {
        UINT32 u4Current = STATE_AC_OFF;
        UINT32 u4Next = STATE_POWER_ON;
        UINT32 u4Reason;

        PDWNC_Custom_ReadWakeupReason(&u4Reason);
        PDWNC_Custom_GetCurrentState(&u4Current);
        PDWNC_Custom_DetermineNextState(u4Current, u4Reason, &u4Next, NULL, NULL);
        if (u4Next == STATE_POWER_ON)
        {
            GPIO_CustomLedCtrlFunc(eLedLoaderPowerOn);
        }
        else
        {
            if (u4Next == STATE_POWER_OFF)
            {
                _fgStandbyDiable = TRUE;
            }
        }
    }
#ifdef CC_MTK_LOADER	
#ifndef CC_SUPPORT_UPGRADE_ENABLE_BY_IR	   
    Custom_InitGPIOTimer();
#endif	
#endif
}

#ifdef CC_MTK_LOADER
static void Custom_InitGPIOTimer(void)
{
    UINT32 u4BusClock;
    UINT32 u4TimerClock;
    
    _pfnCustomNextIsr = RegisterIrqHandler(LOADER_Custom_TimerIsr);

    u4BusClock = BSP_GetDomainClock(SRC_TIMER_CLK);
    u4TimerClock = (UINT32)(u4BusClock / 1000);
    u4TimerClock *= CUSTOM_USB_UPGRADE_BLINK_PERIOD;
    
    // Setup Timer 0 and triger 1st interrupt.
    BIM_WRITE32(REG_RW_TIMER1_LLMT, u4TimerClock);
    BIM_WRITE32(REG_RW_TIMER1_HLMT, 0);
    BIM_WRITE32(REG_RW_TIMER1_LOW , u4TimerClock);
    BIM_WRITE32(REG_RW_TIMER1_HIGH, 0);

    LOADER_Custom_TimerOff();
#ifdef CC_SUPPORT_UPGRADE_ENABLE_BY_IR	
    Printf("Custom_InitGPIOTimer %d\n",BIM_IsIrqEnabled(VECTOR_PDWNC)); 
	BIM_ClearIrq(VECTOR_PDWNC);
	//u4Val=u4IO32Read4B(PDWNC_ARM_INTEN);
	//u4Val |=IS_PINTNO_INTABLE_GPIO(PDWNC_INTNO_IRRX) ? 0 : (_PINT(PDWNC_INTNO_IRRX));
	//vIO32Write4B(PDWNC_ARM_INTEN, u4Val);//enable IR int
	vIO32Write4B(PDWNC_INTCLR, _PINT(PDWNC_INTNO_IRRX));
	vIO32WriteFldAlign(PDWNC_IRCLR, 0x01, FLD_IRCLR);
	BIM_EnableIrq(VECTOR_PDWNC);
#endif
    BIM_EnableIrq(VECTOR_T1);
    HalEnableIRQ();
}
#endif /* CC_MTK_LOADER */
void PDWNC_Custom_Resume(void)
{
	 UINT32 u4WakeupReason;
	 DTVCFG_T rDtvCfg;
	 
     Printf("__%s__\n",__FUNCTION__);
	 u4WakeupReason=PDWNC_ReadWakeupReason();
	 x_memset(&rDtvCfg, 0, sizeof(DTVCFG_T));
	 
	 if(EEPDTV_GetCfg((DTVCFG_T *)&rDtvCfg) != 0)
	 {
		 Printf("x_drv_init: DTV EEPROM read failure.\n");
	 }	
	 
	 if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) && (u4WakeupReason == PDWNC_WAKE_UP_REASON_RTC))
	 {
		 Printf("%s: Quiet Boot\n",__FUNCTION__);
		 PDWNC_Custom_EnterNextState(STATE_SEMI_STANDBY);
	 }
	 else
	 {
	     PDWNC_Custom_EnterNextState(STATE_POWER_ON);//d130703_Haibo:Update status before standby
	 }
}

#endif /* CC_MTK_PRELOADER */

