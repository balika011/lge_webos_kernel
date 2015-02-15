
#ifndef POWERSEQ_H
#define POWERSEQ_H

#include "maple_drv/maple_gpio.h"

//-----------------------------------------------------------------------------
// Constant definitions & Macro
//-----------------------------------------------------------------------------
#ifdef CC_MODEL_DETECT_SUPPORT
#define CUSTOM_PANEL_ID_CHECK_ENABLE
#endif /* CC_MODEL_DETECT_SUPPORT */

#define CUSTOM_GET_AUTO_DETECT_FROM_EEPROM
//#define CUSTOM_GET_DELAY_FROM_EEPROM
//#define CUSTOM_ALL_AC_DET_LOG

// EEPROM information

// Detect time period definition
#define TCON_READY_DETECT_TIME              3000
#ifdef CC_ANDROID_TWO_WORLDS
#define POWER_SEQ_BASE_ADDR                 (0x9F60)
#else
#define POWER_SEQ_BASE_ADDR                 (0x2F60)
#endif
#define POWER_SEQ_AC_DET_TIME_ADDR          (POWER_SEQ_BASE_ADDR + 0x04)
#define POWER_SEQ_AC_DET_RETRY_ADDR         (POWER_SEQ_BASE_ADDR + 0x06)
#define POWER_SEQ_DELAY_ADDR                (POWER_SEQ_BASE_ADDR + 0x08)
#define POWER_SEQ_CHECK_PANEL_ADDR          (POWER_SEQ_BASE_ADDR + 0x3F)


#define POWER_SEQ_AC_DET_TIME_DEFAULT       10
#define POWER_SEQ_AC_DET_RETRY_LIMIT       	3
#define POWER_SEQ_DELAY_VALID_MAX           0xFFFF
#define POWER_SEQ_DELAY_VALID_MAX_1B           0xFF

#define POWER_SEQ_DELAY_DATA_SIZE           14

#define PANEL_INDEX_1	 (0x360)

// AC detect pin definition
#define SIGNAL_AC_DET                       OPCTRL(16)  //GPIO(218) //OPCTRL(8)//GPIO(2)     // GPIO 2
#define AC_DETECT_NORMAL                    1
#define AC_DETECT_ERROR                     0
#ifdef CC_SOEM_ACDET_FLOW
#define AC_DETECT_ERROR_INIT_VAL            0x80
#define AC_DETECT_NORMAL_INIT_VAL           0x00
#endif
#define SET_AC_DETECT_STANDBY() (vIO32Write4B(PDWNC_RESRV1, (u4IO32Read4B(PDWNC_RESRV1)&0xFFFF0000) | 0x0000ACDE))
#define CLR_AC_DETECT_STANDBY() (vIO32Write4B(PDWNC_RESRV1, (u4IO32Read4B(PDWNC_RESRV1)&0xFFFF0000)))
#define IS_AC_DETECT_STANDBY()  (((u4IO32Read4B(PDWNC_RESRV1)&0x0000FFFF) == 0x0000ACDE) ? 1 : 0)


// Panel format select pin definition
//#define SIGNAL_LVDS_SEL                     GPIO(3)     // GPIO 3

//#define SIGNAL_X_U_RST                      GPIO(60)


#define GET_POWER_SEQ_DELAY(Address, Delay, Value, DataLen, Default)    \
    UNUSED(EEPROM_Read(Address, (UINT32)(VOID *)&Value, DataLen));      \
    Value = (((Value & 0xFF00) >> 8) | ((Value & 0xFF) << 8));          \
    Delay = (Value == POWER_SEQ_DELAY_VALID_MAX) ? Default : Value;     \
    if (Value != Delay)                                                 \
    {                                                                   \
        Value = (((Delay & 0xFF00) >> 8) | ((Delay & 0xFF) << 8));      \
        UNUSED(EEPROM_Write(Address, (UINT32)(VOID *)&Value, DataLen)); \
    }
#define GET_POWER_SEQ_DELAY_1B(Address, Delay, Value, DataLen, Default)    \
    UNUSED(EEPROM_Read(Address, (UINT32)(VOID *)&Value, DataLen));      \
    Delay = (Value == POWER_SEQ_DELAY_VALID_MAX_1B) ? Default : Value;     \
    if (Value != Delay)                                                 \
    {                                                                   \
        Value = Delay;													\
        UNUSED(EEPROM_Write(Address, (UINT32)(VOID *)&Value, DataLen)); \
    }

#ifndef CC_MTK_LOADER
typedef enum
{
    CUSTOM_POWER_SEQ_CLEAR_SHUTDOWN_STATE,
    CUSTOM_POWER_SEQ_SET_SHUTDOWN_STATE
} CUSTOM_POWER_SEQ_ACTION;
#endif /* CC_MTK_LOADER */

//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
//void CustomPowerOnSequence(void);
//void CustomPowerOffSequence(void);
void CustomStartACDetect(void);
void CustomStopACDetect(void);
#ifdef CC_SOEM_ACDET_PAA
void ACErrorDetect(INT32 i4Gpio, BOOL fgStatus);
#endif

#endif /* POWERSEQ_H */
