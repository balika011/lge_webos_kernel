/****************************************************************************
*
* File:        temp_setup.c
*
* Description: mitsumi mm3285cnre registers and functions to control registers.
*  
* Author:      kl-lees
*
* History:     15 june 2009 - Creation
*
******************************************************************************
*
*	PUBLIC FUNCTIONS:
*
*****************************************************************************/

/****************************************************************************/
/*     			      HEADER FILES				    						*/
/****************************************************************************/
#include "x_os.h"
#include "x_assert.h"
#include "x_dbg.h"
#include "sif_if.h"
#include "../../sony_priv_inc/temp_setup.h"
#include "mitsumi_temp_sensor.h"

/****************************************************************************/
/*     			      TYPE DEFINITIONS			    						*/
/****************************************************************************/
typedef struct _TEMP_SENSOR_MEMBER_T
{
    HANDLE_T    _hTempSensorDrvThread;
    HANDLE_T    _hTempSensorDetTimer;
    HANDLE_T    _hTempSensorThreadSema;
    
}TEMP_SENSOR_MEMBER_T;

/****************************************************************************/
/*				SETTINGS				    								*/
/****************************************************************************/ 

/****************************************************************************
*			      VARIABLES                                     			*
*****************************************************************************/
static TEMP_SENSOR_MEMBER_T t_temp_sensor_drv;
static INT16 i2_TempSensorValue = 0x00;

BOOL b_mitsumi_temp_sensor_detected;
				 
/****************************************************************************
*			  	FUNCTION PROTOTYPES				    						*	
*****************************************************************************/
/*  General TEMP functions */
static void TEMP_SENSOR_DetectThread(void);
static void TEMP_SENSOR_setup(void);
static void TEMP_SENSOR_timerout(HANDLE_T h_timer, void *pv_tag);
static INT16 TEMP_SENSOR_getValue(void);

/****************************************************************************
*				FUNCTION DEFINITIONS			    						*
*****************************************************************************/

/*****************************************************************************
*	FUNCTION: void TEMP_SENSOR_Init(void)
*	DESCRIPTION: Initializes TEMP sensor control
*	PRECOND.: None
*	POSTCOND.: None
*****************************************************************************/
void TEMP_SENSOR_Init(void)
{
    TEMP_SENSOR_MEMBER_T* pt_this = &t_temp_sensor_drv;
    
    /* Create Temp Sensor sema, timer_handler & thread_handler */
    VERIFY(x_sema_create(&pt_this->_hTempSensorThreadSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    VERIFY(x_timer_create(&pt_this->_hTempSensorDetTimer) == OSR_OK);
    VERIFY(x_thread_create(&pt_this->_hTempSensorDrvThread, "TEMP_SENSOR_HDL", TEMP_SENSOR_THREAD_STACK_SIZE, TEMP_SENSOR_THREAD_PRIORITY, (x_os_thread_main_fct)TEMP_SENSOR_DetectThread, 0, NULL) == OSR_OK);
}

/*****************************************************************************
*	FUNCTION: static void TEMP_SENSOR_timeout(void)
*	DESCRIPTION: TEMP sensor current timer handler
*	PRECOND.: None
*	POSTCOND.: None
*****************************************************************************/
void TEMP_SENSOR_timerout(HANDLE_T h_timer, void *pv_tag)
{
    TEMP_SENSOR_MEMBER_T* pt_this = &t_temp_sensor_drv;
    
    if (pt_this->_hTempSensorThreadSema != NULL_HANDLE)
    {
        VERIFY(x_sema_unlock(pt_this->_hTempSensorThreadSema) == OSR_OK);
    }
}

/*****************************************************************************
*	FUNCTION: void TEMP_SENSOR_setup(void)
*	DESCRIPTION: Initializes TEMP sensor control variables
*	PRECOND.: None
*	POSTCOND.: None
*****************************************************************************/
void TEMP_SENSOR_setup(void)
{	
	b_mitsumi_temp_sensor_detected = TRUE;

	/* Configures TEMPERATURE sensor setup */
	if ( mitsumi_temp_sensor_setup() == TEMP_SENSOR_CONFIG_NG )
	{
		b_mitsumi_temp_sensor_detected = FALSE;
	}
        
}

/*****************************************************************************
*	FUNCTION: INT16 TEMP_SENSOR_getValue(void)
*	DESCRIPTION: Return current temperature.
*	PRECOND.: None.
*	POSTCOND.: None.
*****************************************************************************/
INT16 TEMP_SENSOR_getValue(void)
{
    INT16 i2_TempValue = 0x00;

	mitsumi_temp_sensor_control();
        
    /* Check the sign bit */
	if ( mitsumi_temp_sensor_get_temp_sign() == NEGATIVE_TEMP )
	{
		/* Convert -ve Temp to value 0 */
        i2_TempValue = 0x00;
	} 
    
    else 
    {
    	/* Reads TEMPERATURE from sensor */
    	i2_TempValue = mitsumi_temp_sensor_get_temperature();        
    
        /* Multiply value with co-efficient to get real value */
        /* Example: i2_TempValue = 355 --> real_value = 35.5 */
        i2_TempValue = ((i2_TempValue * 10) / MITSUMI_TEMP_SENSOR_COEFFICIENT);
            
    }
#ifdef TEMP_SENSOR_LOG
    x_dbg_stmt("[Temp Sensor] Temp_Value = %d \n", i2_TempValue); 
#endif  
         	
    /* Mitsumi temp sensor driver retuns the temperature value doubled */
	return i2_TempValue;  
}

/*****************************************************************************
*	FUNCTION: INT16 vAPIGetTempSensorValue(void)
*	DESCRIPTION: API to return current temperature.
*	PRECOND.: None.
*	POSTCOND.: None.
*****************************************************************************/
INT16 vAPIGetTempSensorValue (void)
{   
    return i2_TempSensorValue;
}

/*****************************************************************************
*	FUNCTION: void TEMP_SENSOR_DetectThread(void)
*	DESCRIPTION: TEMP sensor detection main loop
*	PRECOND.: None
*	POSTCOND.: None
*****************************************************************************/
void TEMP_SENSOR_DetectThread (void)
{
    TEMP_SENSOR_MEMBER_T* pt_this = &t_temp_sensor_drv;
    
    /* Init i2_TempSensorValue when thread starts */
    i2_TempSensorValue = 0x00;
    
    /* Init Temp Sensor */
    TEMP_SENSOR_setup();
    
    while (1)
    {
#ifdef TEMP_SENSOR_LOG
        UINT32  ui4_start_time, ui4_end_time, ui4_measure_time;
     
        ui4_start_time = x_os_get_sys_tick();
#endif
        /* Start Timer*/
        x_timer_start(pt_this->_hTempSensorDetTimer, TEMP_SENSOR_TIMER, X_TIMER_FLAG_ONCE, TEMP_SENSOR_timerout , NULL);
        
        /* Sema_lock */
        x_sema_lock(pt_this->_hTempSensorThreadSema, X_SEMA_OPTION_WAIT);

        if (b_mitsumi_temp_sensor_detected == TRUE)
        {   
            /* Get current temperature value */
            i2_TempSensorValue = TEMP_SENSOR_getValue();
        }    
        else
        {
            /* Do nothing*/
        }  
        
#ifdef TEMP_SENSOR_LOG
    ui4_end_time = x_os_get_sys_tick();
    ui4_measure_time = (ui4_end_time-ui4_start_time)*x_os_get_sys_tick_period();
    
    x_dbg_stmt("[Temp Sensor] Loop_Time = %d \n", ui4_measure_time);
#endif         
    }            
}
/*****************************************************************************
*   End of file
*****************************************************************************/

