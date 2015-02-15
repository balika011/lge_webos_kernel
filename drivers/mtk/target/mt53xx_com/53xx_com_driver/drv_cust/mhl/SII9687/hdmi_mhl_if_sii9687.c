#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH
#include "x_hal_arm.h"
#include "mhl_if.h"
#include "si_datatypes.h"
#include "util.h"

extern void sk9589Init(void);
extern void sk9589Loop(void);
extern void SiiPlatformTimerSysTickIntHandler(void);

void vHDMIMHL_Init(void)
{
	sk9589Init();
}

void vMhl_MainLoop(void)
{
	sk9589Loop();
	SiiPlatformTimerSysTickIntHandler();
}

//-------------------------------------------------------------------------------------------------
//! @brief      Keep MCU in sleep mode until h/w interrupt arrives or timer expires.
//!
//! @param[in]  delay - maximum time to stay in sleep mode
//-------------------------------------------------------------------------------------------------

void OsSleep(clock_time_t delay)
{
    //MAP_SysCtlSleep();
   vUtDelay1ms(1);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Ensure code execution after this call won't be disrupted by an interrupt thread.
//-------------------------------------------------------------------------------------------------
UINT32 state =0;
void EnterCriticalSection(void)
{
    //SiiPlatformAllInterruptsDisable();
	//SiiPlatformInterruptDisable();
	state = HalCriticalStart();
}

//-------------------------------------------------------------------------------------------------
//! @brief      Release the block for interrupt treads execution.
//-------------------------------------------------------------------------------------------------

void LeaveCriticalSection(void)
{
    //SiiPlatformAllInterruptsEnable();
	//SiiPlatformInterruptEnable();
	HalCriticalEnd(state);
}

#if 0
void Delay_ms(UINT32 Delay_Cnt)
{
	Delay1ms((UINT32)Delay_Cnt);
}
#endif


void SendRcpKey(UINT8 key)
{
	//IT66332_MscRcpSend(key);
}

#endif
#endif


