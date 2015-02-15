
//#include "si_common.h"
//#include "si_osal_timer.h"

//------------------------------------------------------------------------------
// Defines for setting up the system clock.
//------------------------------------------------------------------------------
//
// Define how many clock ticks in one second.
// Note:  This should match the value of SYSTICKHZ in the main program.
//
#if 0
#define CLOCK_CONF_SECOND       1000         // 100 == 10ms

#define SYSTICKHZ               CLOCK_CONF_SECOND
#define SYSTICKMS               (1000 / SYSTICKHZ)      // Number of ms per SYSTICK
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)


#define ELAPSED_TIMER               0xFF
#define ELAPSED_TIMER1              0xFE
#define TIMER_0                     0   // DO NOT USE - reserved for TimerWait()
#define TIMER_1                     1
#define TIMER_2                     2
#define TIMER_3                     3
#define TIMER_4                     4
#define TIMER_COUNT                 5
#endif

//extern bool_t I2cWriteBlock ( int busIndex, uint8_t devId, uint16_t regAddr, const uint8_t *pWrData, uint16_t nBytes );  //9687
//extern bool_t I2cReadBlock ( int busIndex, uint8_t devId, uint16_t regAddr, uint8_t *pRdData, uint16_t nBytes );     //9687
//extern uint8_t      SiiPlatformTimerExpired ( uint8_t timer );
//extern uint16_t     SiiPlatformTimerElapsed ( uint8_t index );
//extern void         SiiPlatformTimerWait ( uint16_t m_sec );
//extern void         SiiPlatformTimerSet ( uint8_t index, uint16_t m_sec );
//extern void         SiiPlatformTimerInit (void);
//extern clock_time_t SiiPlatformTimerSysTicksGet (void);
//extern void SiiPlatformTimerSysTickIntHandler(void);
//extern void SysTickIntHandler_mhl(void);
extern void sk9589Loop(void);
extern void sk9589Init(void);




