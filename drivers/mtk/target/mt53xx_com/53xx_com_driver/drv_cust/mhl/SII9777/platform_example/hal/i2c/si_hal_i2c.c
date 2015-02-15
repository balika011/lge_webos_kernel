/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file reg_access.c
 *
 * @brief register access APIs
 *
 *****************************************************************************/

/***** #include statements ***************************************************/
/*
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <sys/stat.h>
*/
#include "aardvark.h"
#include "si_datatypes.h"

/***** local macro definitions ***********************************************/

#define I2C_SLAVE_ADDR             0x40    /* this is for "Boston" only */
#define MAX_NUM_ADAPTORS           10
#define I2C_BITRATE                400

#define GPIO_HW_RESET              AA_GPIO_SS
#define GPIO_HW_RESET_DEASSERTED   AA_GPIO_SS

#define BUS_STRING                 "I2C "

/***** local type definitions ************************************************/

enum reg_access_status
{
    REG_ACCESS_OK,
    REG_ACCESS_ERROR
};

/***** local prototypes ******************************************************/

static int get_available_adaptors(int num_elements, int* num_found, unsigned short* ports, unsigned int* unique_ids, bool_t* available);
static int select_available_adaptors(int count, int availCount, unsigned short* ports, unsigned int* unique_ids, bool_t* available, char *connType);

/***** local data objects ****************************************************/

Aardvark i2cHandle = -1;

/***** public functions ******************************************************/

bool_t SiiHalI2cCreate( void )
{
	unsigned short ports[MAX_NUM_ADAPTORS];
	unsigned int   unique_ids[MAX_NUM_ADAPTORS];
	bool_t         available[MAX_NUM_ADAPTORS];
	int            num_elements = MAX_NUM_ADAPTORS;
	int            count;
	int            availCount   = 0;
	int            i;

	availCount = get_available_adaptors(num_elements, &count, ports, unique_ids, available);

	if (availCount == 0)
	{
		return false;
	}

	i = select_available_adaptors(count, availCount, ports, unique_ids, available, BUS_STRING);
	if (i == -1)
	{
		return false;
	}

	i2cHandle = aa_open(ports[i]);
	if (i2cHandle > 0)
	{
		aa_configure(i2cHandle, AA_CONFIG_GPIO_I2C);
		aa_i2c_pullup(i2cHandle, AA_I2C_PULLUP_NONE);
		aa_target_power(i2cHandle, AA_TARGET_POWER_NONE);

		// Setup the bitrate
		aa_i2c_bitrate(i2cHandle, I2C_BITRATE);

		// Set GPIO direction (of HW_RESET) to output.
		aa_gpio_direction(i2cHandle, GPIO_HW_RESET);
		aa_gpio_set(i2cHandle, GPIO_HW_RESET_DEASSERTED);
		return true;
	}

	return false;
}

void SiiHalI2cDelete( void )
{
	if( i2cHandle > 0 )
	{
		aa_close(i2cHandle);
	}
}

//------------------------------------------------------------------------------
// Function:    SiiHalI2CReadBlock
// Description: Reads a block of data from sequential registers.
//
// A count of 0 will read 256 bytes.
//------------------------------------------------------------------------------
int SiiHalI2cBlockRead(uint8_t hwId, uint16_t offset, uint8_t *buffer, uint16_t count)
{
    uint8_t page   = I2C_SLAVE_ADDR;
	int status;
	uint16_t temp_count;
	uint8_t data[2];

	uint16_t num_bytes;
	temp_count = count;
	if (temp_count == 0)
	{
		temp_count = 256;
	}

	data[0] = (offset >> 8) & 0xFF;
	data[1] = offset & 0xFF;
	
	status = aa_i2c_write_ext(i2cHandle, (unsigned short)(page >> 1), AA_I2C_NO_STOP, 2, data, (unsigned short *)&num_bytes);
	if (status == REG_ACCESS_OK)
	{
		status = aa_i2c_read_ext(i2cHandle, (unsigned short)(page >> 1), AA_I2C_NO_FLAGS, temp_count, buffer, (unsigned short *)&num_bytes);
	}

	if (status != REG_ACCESS_OK)
	{
		status = REG_ACCESS_ERROR;
	}

    return status;
}

//------------------------------------------------------------------------------
// Function:    reg_write_block
// Description: Writes a block of data to sequential registers.
//------------------------------------------------------------------------------
int SiiHalI2cBlockWrite(uint8_t hwId, uint16_t offset, const uint8_t *buffer, uint16_t count)
{
    uint8_t page   = I2C_SLAVE_ADDR;
	int status;
	uint16_t i, temp_count;

	uint8_t data[1024];
	uint16_t num_bytes;

	//sum = 0;
	temp_count = count;
	if (temp_count == 0)
	{
		temp_count = 256;
	}

	i = 0;
	data[i++] = (offset >> 8) & 0xFF;
	data[i++] = offset & 0xFF;

	for (;i <= temp_count + 1; i++)
	{
		data[i] = buffer[i - 2];
	}

	status = aa_i2c_write_ext(i2cHandle, (unsigned short)(page >> 1), AA_I2C_NO_FLAGS, i, data, (unsigned short *)&num_bytes);

	if (status != REG_ACCESS_OK)
	{
		status = REG_ACCESS_ERROR;
	}

    return status;
}

/***** local functions *******************************************************/

static int get_available_adaptors(int num_elements, int* num_found, unsigned short* ports, unsigned int* unique_ids, bool_t* available)
{
	int availCount = 0;
	int count = -1;
	int i;

	// Find all the attached devices
	count = aa_find_devices_ext(num_elements, ports, num_elements, unique_ids);

	if (count > num_elements)
	{
		count = num_elements;
	}

	for (i = 0; i < count; ++i)
	{
		// Determine if the device is in-use
		available[i] = true;
		availCount++;

		if (ports[i] & AA_PORT_NOT_FREE)
		{
			ports[i] &= ~AA_PORT_NOT_FREE;
			available[i] = false;
			availCount--;
		}
	}

	*num_found = count;
	return availCount;
}

static int select_available_adaptors(int count, int availCount, unsigned short* ports, unsigned int* unique_ids, bool_t* available, char *connType)
{
	int i;
	uint16_t portSel;

	if (availCount == 1)
	{
		for (i = 0; i < count; ++i)
		{
			if (available[i] == true)
			{
				break;
			}
		}
	}
	else
	{
		for (i = 0; i < count; ++i)
		{
			if (available[i] == true)
			{
			}
		}

		portSel = (uint16_t)_getche() - '0';

		for (i = 0; i < count; ++i)
		{
			if ((available[i] == true) && (portSel == ports[i]))
			{
				break;
			}
		}

		if (i == count)
		{
			i = -1;
		}
	}

	return i;
}

/***** end of file ***********************************************************/
