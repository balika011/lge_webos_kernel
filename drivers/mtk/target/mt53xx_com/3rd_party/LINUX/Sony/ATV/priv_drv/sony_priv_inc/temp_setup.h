/****************************************************************************

	TEMP DRIVER HEADER            

*****************************************************************************
*
* File:        temp_sensor_setup.h
*
* Description: temp sensor module header.
*
* Author:      kl-lees
*
* History:     15 june 2009 - Creation
*
*****************************************************************************/

#ifndef _TEMP_SENSOR_SETUP_H
#define _TEMP_SENSOR_SETUP_H

/****************************************************************************
	DEFINES
*****************************************************************************/

/* Temp Sensor Thread Properties */
#define TEMP_SENSOR_THREAD_STACK_SIZE   1024
#define TEMP_SENSOR_THREAD_PRIORITY     100
#define TEMP_SENSOR_TIMER               500     /* 500ms */

/* Temp Sensor MSB Sign */
#define POSITIVE_TEMP   0
#define NEGATIVE_TEMP   1

/****************************************************************************
	FUNCTIONS
*****************************************************************************/

#endif /* _TEMP_SENSOR_H */

/*****************************************************************************
*   End of file
*****************************************************************************/

