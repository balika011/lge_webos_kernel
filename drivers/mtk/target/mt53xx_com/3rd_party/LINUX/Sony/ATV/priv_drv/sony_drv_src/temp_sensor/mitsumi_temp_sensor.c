/****************************************************************************
*
* File:        mitsumi_temp_sensor.c
*
* Description: mitsumi mm3285cnre registers and functions to control registers.
*
* Author:      kl-lees
*
* History:     15 june 2009 - Creation
*
******************** Copyright (c)SONY Corporation 2009 ************************
*
* The information contained here-in is the property of SONY corporation and is 
* not to be disclosed or used without the prior written permission of SONY 
* Corporation. This copyright extends to all media in which this information may 
* be preserved including magnetic storage, computer print-out or visual display.
*
* Contains proprietary information, copyright and database rights SONY(r).
*
* Decompilation prohibited save as permitted by law. No using, disclosing, 
* reproducing, accessing or modifying without SONY(r) prior written consent.
*
****************************************************************************/

#define _MITSUMI_TEMP_SENSOR_C_

/****************************************************************************
*
*    Include files
*
****************************************************************************/
#include "sif_if.h" 
#include "mitsumi_temp_sensor.h"
#include "x_dbg.h"
#include "../../sony_drv_inc/Sony_drv_cb.h"
/****************************************************************************
*
*    Constant definitions
*
****************************************************************************/
#define MITSUMI_TEMP_SENSOR_CONFIG_CODE     0x00    /* 0 0 0 0 0 0 P1 P0 */
                                                    /* P1 P0 */
                                                    /* 0  0  -> Read Temperature - Power Up Default */
                                                    /* 0  1  -> Read/Write Configuration */
                                                    /* 1  0  -> Test Mode Configuration */
                                                    /* 1  1  -> Config NOT USED*/
/****************************************************************************
*
*   Data Declarations
*
*****************************************************************************/

UINT8 aui1_mitsumi_temp_sensor_status[2];

/****************************************************************************
*
*    Local functions definitions
*
****************************************************************************/
static UINT8 _mitsumi_temp_sensor_iic_write(UINT8 ui1_Addr, UINT8 aui1_Data);
static UINT8 _mitsumi_temp_sensor_iic_read(void);

/****************************************************************************
*
*    Public funtion definitions
*
****************************************************************************/

/****************************************************************************
*	FUNCTION: INT32 mitsumi_temp_sensor_setup(void) - PUBLIC
*	DESCRIPTION: checks IIC communication with mitsumi temp sensor
*	PRECOND.:
*	POSTCOND.: OK if successful
****************************************************************************/
INT32 mitsumi_temp_sensor_setup(void)
{
	INT32 i4_config_data = MITSUMI_TEMP_SENSOR_CONFIG_CODE;
	INT8 i1_count;

	for (i1_count = 0; i1_count < 7; i1_count++)
	{
		if ( _mitsumi_temp_sensor_iic_write( 0x00, i4_config_data) == 0 )
		{
			return TEMP_SENSOR_CONFIG_NG;
		}
	}
	return TEMP_SENSOR_CONFIG_OK;
}

/****************************************************************************
*	FUNCTION: INT32 mitsumi_temp_sensor_get_temp_sign( void )
*	DESCRIPTION: returns the sign bit (MSB) of the temperature
*	PRECOND.: None.
*	POSTCOND.: None.
****************************************************************************/
INT32 mitsumi_temp_sensor_get_temp_sign(void)
{
    /* Reads the MSB bit */             /* 1 - Negative Value */
                                        /* 0 - Positive Value */
	return ((aui1_mitsumi_temp_sensor_status[0]& 0x80)>>7);
}

/****************************************************************************
*	FUNCTION: INT32 mitsumi_temp_sensor_get_temperature( void )
*	DESCRIPTION: returns decimal part of temperature
*	PRECOND.: None.
*	POSTCOND.: None.
****************************************************************************/
INT32 mitsumi_temp_sensor_get_temperature(void)
{
	INT16 i2_temp;

	i2_temp = (((aui1_mitsumi_temp_sensor_status[0] & 0x7F)<<1) | ((aui1_mitsumi_temp_sensor_status[1]& 0x80)>>7));

	return i2_temp;
}

/****************************************************************************
*	FUNCTION: void mitsumi_temp_sensor_control() - PUBLIC
*	DESCRIPTION: Reads mitsumi temp sensor.
*	PRECOND.: None.
*	POSTCOND.: None.
****************************************************************************/
void mitsumi_temp_sensor_control(void) 
{
	_mitsumi_temp_sensor_iic_read();
}

/****************************************************************************
*	FUNCTION: void _mitsumi_temp_sensor_iic_write(uchar, uchar) - PRIVATE
*	DESCRIPTION: Writes a mitsumi temp sensor register through the IIC bus
*	PRECOND.: None.
*	POSTCOND.: None.
****************************************************************************/
static UINT8 _mitsumi_temp_sensor_iic_write(UINT8 ui1_Addr, UINT8 aui1_Data)
{
	INT32 i4_WriteTemp = 0;

	i4_WriteTemp = vSIFWrite(MITSUMI_TEMP_SENSOR_CLK_DIV, MITSUMI_TEMP_SENSOR_ADDR, ui1_Addr, &aui1_Data, 1);

	return i4_WriteTemp;
}

/****************************************************************************
*	FUNCTION: INT32 _mitsumi_temp_sensor_iic_read(void) - PRIVATE
*	DESCRIPTION: Read mitsumi temp sensor status through the IIC bus.
*	PRECOND.: None.
*	POSTCOND.: IIC return value.
****************************************************************************/
static UINT8 _mitsumi_temp_sensor_iic_read(void)
{
	UINT32 ui4_ReadTemp = 0;
	UINT8 aui1_Data[2] = {0,0};
	UINT8 ui1_Addr = 0x00;

	ui4_ReadTemp = vSIFRead(MITSUMI_TEMP_SENSOR_CLK_DIV, MITSUMI_TEMP_SENSOR_ADDR, ui1_Addr, aui1_Data, 2);

	aui1_mitsumi_temp_sensor_status[0] = aui1_Data[0];
	aui1_mitsumi_temp_sensor_status[1] = aui1_Data[1];
       
	if (ui4_ReadTemp > 0)
	{
		return TEMP_SENSOR_IIC_READ_SUCCESS;//aui1_Data;
	}	
	else
	{
#ifdef TEMP_SENSOR_LOG	
        x_dbg_stmt("[Temp Sensor] MITSUMI TEMP SENSOR READ FAIL \n");
#endif  		
		return TEMP_SENSOR_IIC_READ_FAIL;
	}
}


/****************************************************************************
*    End of file
****************************************************************************/

