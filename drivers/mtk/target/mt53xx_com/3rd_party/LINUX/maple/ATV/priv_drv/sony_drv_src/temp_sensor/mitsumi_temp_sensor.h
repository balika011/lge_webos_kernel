/****************************************************************************

	TEMPERATURE SENSOR MODULE HEADER                      
	MITSUMI TEMP SENSOR

*****************************************************************************
*
* File:        mitsumi_temp_sensor.h
*
* Description: mitsumi mm3285cnre module header.
*
* Author:      kl-lees
*
* History:     15 june 2009 - Creation
*
*****************************************************************************/

#ifndef _MITSUMI_TEMP_SENSOR_H
#define _MITSUMI_TEMP_SENSOR_H

/****************************************************************************
	DEFINES
*****************************************************************************/

/* Temp Sensor Slave Address */
#define MITSUMI_TEMP_SENSOR_ADDR            0x90 

/* Temp Sensor IIC Speed */
#define MITSUMI_TEMP_SENSOR_CLK_DIV         0x100  

/* Temp Sensor Multiplication Coefficient */
#define MITSUMI_TEMP_SENSOR_COEFFICIENT     2 

/* Temp Sensor Configuration Setting */
#define TEMP_SENSOR_CONFIG_OK               1
#define TEMP_SENSOR_CONFIG_NG               0

/* Temp Sensor IIC Read Setting */
#define TEMP_SENSOR_IIC_READ_SUCCESS        1
#define TEMP_SENSOR_IIC_READ_FAIL           0

/* Debug log Enable */
//#define TEMP_SENSOR_LOG

/****************************************************************************
	FUNCTIONS
*****************************************************************************/

/*General functions prototypes*/
extern INT32 mitsumi_temp_sensor_setup(void);
extern INT32 mitsumi_temp_sensor_get_temp_sign(void);
extern INT32 mitsumi_temp_sensor_get_temperature(void);
extern void mitsumi_temp_sensor_control(void);

#endif /*_MITSUMI_TEMP_SENSOR_H */

/*****************************************************************************
*   End of file
*****************************************************************************/

