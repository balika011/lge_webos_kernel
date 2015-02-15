
#include "si_common.h"
#include "x_printf.h"
//#include "si_platform.h"
//#include "si_timer.h"

//#include "si_arm.h"

#include "si_datatypes.h"
#include "si_9687ext.h"
#include "si_cbus_enums.h"

#include "sk_application.h"


#include "si_timer.h"

//------------------------------------------------------------------------------
//  Application Instance Data
//------------------------------------------------------------------------------

AppInstanceData_t app;


#if 0
extern void I2CWrite_9589( unsigned char devId, unsigned char regAddr, unsigned char wrData );
extern  unsigned char I2CRead_9589( unsigned char devId, unsigned char regAddr );
extern unsigned char I2cReadBlock_9589( unsigned char devId, unsigned char regAddr, unsigned char *pRdData, unsigned short nBytes );
extern unsigned char I2cWriteBlock_9589 ( unsigned char devId, unsigned char regAddr,  unsigned char *pWrData, unsigned short nBytes);

#endif
//------------------------------------------------------------------------------
// Array of timer values
//------------------------------------------------------------------------------



#if 0

bool_t I2cWriteBlock ( int busIndex, uint8_t devId, uint16_t regAddr, const uint8_t *pWrData, uint16_t nBytes )  //9687
{
	busIndex=0;
	return(I2cWriteBlock_9589(devId, regAddr, pWrData, nBytes ));
}

bool_t I2cReadBlock ( int busIndex, uint8_t devId, uint16_t regAddr, uint8_t *pRdData, uint16_t nBytes )     //9687
{
	busIndex=0;
	return(I2cReadBlock_9589(devId, regAddr, pRdData, nBytes ));
}
#endif

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************

 //uint32_t rMhlCurentTime, rMhlOldTime;

#if 0
void SysTickIntHandler_mhl(void)
{
      // printf("\r\n9687  SysTickIntHandler_mhl.\n");
	SiiPlatformTimerSysTickIntHandler();
}
#endif

void sk9589Loop(void)
{
	//printf("\r\n9687  sk9589Loop\n");
	//		SkAppProcess( app.powerState, app.portSelect );
	SiiSchedMainLoop();
}

void sk9589Init(void)
{
	  printf("\r\nEnter the si_9687ext.c::sk9589Init.\n");
		SkAppInitInstanceData();
	//	SkAppDevicePower(0);
#if 1
		// Initialize OS scheduler
		SiiSchedInitialize();
	
		//---------------------------------------------------------------------
		// Bind events to their handlers
		//---------------------------------------------------------------------
		// Link Device IRQ event to interrupt manager
		SiiSchedEventHandlerBind(SI_EVENT_ID_PLATFORM_DEV_INT, 0, SiiDrvDeviceManageInterrupts, 0);
	
		// Link driver interrupt handlers to their app tasks
#if INC_IPV
		SiiSchedEventHandlerBind(SI_EVENT_ID_IPV_INT, 0, SkAppTaskIpv, 0);
#endif
		SiiSchedEventHandlerBind(SI_EVENT_ID_SWITCH_INT, 0, SkAppTaskSwitch, 0);
	//	SiiSchedEventHandlerBind(SI_EVENT_ID_CBUS_INT, 0, SkAppTaskCbus, 0);
	//	SiiSchedEventHandlerBind(SI_EVENT_ID_GPIO_INT, 0, SkAppTaskGpio, 0);
		SiiSchedEventHandlerBind(SI_EVENT_ID_TX_INT, 0, SkAppSwitchOnTxMuteChange, 0);
	
		// Link platform interrupt handlers to tasks
		//SiiSchedEventHandlerBind(SI_EVENT_ID_PLATFORM_IR_INT, 0, SkAppTaskIrRemote, 0);
		//SiiSchedEventHandlerBind(SI_EVENT_ID_PLATFORM_JSTK_INT, 0, SkAppTaskIrRemote, 0);
	
		//---------------------------------------------------------------------
		// Platform Initialization
		//---------------------------------------------------------------------
		SiiPlatformTimerInit();
	/*
		// Initialize the micro controller but not the the device
		if ( !SiiPlatformInitialize() )
		{
			SkAppBlinkTilReset( ERRLED_BAD_INIT );
		}
	*/
		SkAppSignon();
	/*
		// Check starter kit EEPROM and initialize it if necessary.
		if ( SkAppEepromInitialize( true, false ))
		{
			printf("\r\nStarter Kit EEPROM initialized to defaults.\n");
		}
	*/
		//---------------------------------------------------------------------
		// Schedule background tasks
		//---------------------------------------------------------------------
	
		// Alive LED blinker
	//	SiiSchedTimerSet(500, SkAppBlinkAlive, 0, true, SI_SCHED_PRIORITY_NORMAL);
		// Power Control and Initialization Task
		SkAppDevicePower(0); //xuz 20130416
//	SiiSchedTimerSet(5, SkAppDevicePower, 0, false, SI_SCHED_PRIORITY_NORMAL);
		// SiiMon communication handler
	//	SiiSchedTimerSet(COMM_POLLING_DELAY, SkAppExtMonitorHandler, 0, true, SI_SCHED_PRIORITY_HIGH);
		// UI tasks
		SiiSchedTimerSet(UI_POLLING_DELAY, SkAppUiTask, 0, true, SI_SCHED_PRIORITY_NORMAL);
	
		//---------------------------------------------------------------------
		// Yield to OS scheduler
		//---------------------------------------------------------------------
#else
		SkAppInitInstanceData();

#endif		
		 printf("\r\nsi_9687ext.c::sk9589Init end..\n");

}













