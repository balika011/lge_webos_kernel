
/*! 
********************************************************************************
*  Copyright (c) Novatek Microelectronics Corp., Ltd. All Rights Reserved. 
*  \file    hal_frc_sample.c
*  \brief 
*  \version ver0.1 
*  \chip    NT72314
******************************************************************************** 
*/

//! Standard lib
//#include "stdio.h"
//#include <signal.h>
//#include <string.h>
//#include <unistd.h>
#include "x_os.h"

#include "Def_types.h"
#include "soc_iic.h"

#define FRC_REFIX     "#HAL_FRC# "
#define PRINT_FRC_ERR(x...)	printf(FRC_REFIX x)
#define PRINT_FRC_WRN(x...)	printf(FRC_REFIX x)
#define PRINT_FRC_DBG(x...)	printf(FRC_REFIX x)

////////////////////////////////////////////////////////////////////////
////////////////     These APIs are platform-depended, Just reference      /////////////
////////////////////////////////////////////////////////////////////////
#define IIC_BURST_LEN   (256)

#define _enFrcSubAddrMode (1)  //8bit mode
#define _u8FrcSlaveAddr  (0x34)  //72314 slave address


void OS_DelayMS(u16 u16DelayMS)
{
	//usleep( u16DelayMS * 1000 );
	x_thread_delay(u16DelayMS);//gmh 20130121
}

static bool _hal_frc_WriteByte(u8 u8OffSet, u8 u8Value)
{
	bool bResult = TRUE;
	u8 str[2];

	//memset(str, 0, 2);	//gmh 20130121
	str[0] = 0;//gmh 20130121
	str[1] = 0;//gmh 20130121

	str[0] = u8Value;

	if(!Soc_Drv_IIC_Write( _u8FrcSlaveAddr, _enFrcSubAddrMode, u8OffSet, 1, str))
		bResult = FALSE;

	return bResult;
}

bool _hal_frc_ReadByte(u8 u8OffSet, u8* pu8Value)
{
	bool bResult = TRUE;
	u8 str[2];

	//memset(str, 0, 2);	//gmh 20130121
	str[0] = 0;//gmh 20130121
	str[1] = 0;//gmh 20130121

	if(!Soc_Drv_IIC_Read( _u8FrcSlaveAddr, _enFrcSubAddrMode, u8OffSet, 1, str))
	{
		bResult = FALSE;
		return bResult;
	}

	*pu8Value = str[0];

	return bResult;
}

static bool _hal_frc_WriteNumBytes(u8 u8SubAddr, u32 u32NumOfBytes, u8 *pau8Buf)
{
	bool bResult = TRUE;
	if(!Soc_Drv_IIC_Write( _u8FrcSlaveAddr, _enFrcSubAddrMode, u8SubAddr, u32NumOfBytes, pau8Buf))
		bResult = FALSE;
	return bResult;
}

static bool _hal_frc_ReadNumBytes(u8 u8SubAddr, u32 u32NumOfBytes, u8 *pau8Buf)
{
	bool bResult = TRUE;
	if(!Soc_Drv_IIC_Read( _u8FrcSlaveAddr, _enFrcSubAddrMode, u8SubAddr, u32NumOfBytes, pau8Buf))
		bResult = FALSE;
	return bResult;
}

bool HAL_FRC_ReadReg(u32 u32Reg, u32* u32Value)
{
	bool bResult = TRUE;
	int i;
	u8 str[4];
	u32 tmp;
	//u8 retStatus=0;

	//--printf("Read Reg %08x\n", u32Reg);
	if(u32Value == NULL)
		return FALSE;
	
	for (i = 0; i < 4; i++)
	{
		str[i] = (u8)((u32Reg >> (i * 8)) & 0xFF);
	}
	
	if(!_hal_frc_WriteNumBytes(8, 4, str))
	{
		bResult = FALSE;
		return bResult;
	}

	if (!_hal_frc_ReadNumBytes(0xC, 4, str))
	{
		bResult = FALSE;
			return bResult;
	}
	
	tmp = 0;
	
	for (i = 0; i < 4; i++)
	{
		//--printf("str[%d] = %x\n", i, str[i]);
		tmp |= (str[i] << (i * 8));
	}
	
	*u32Value = tmp;
	
	if(bResult != TRUE)
	{
		//printf("FRC Read Register fail \n");
	}
	
	return bResult;
}


bool HAL_FRC_WriteReg(u32 u32Reg, u32 u32Value)
{
	bool bResult =TRUE;
	int i;
	u8 str[8];
	//u8 retStatus=0;

	for (i = 0; i < 4; i++)
	{
		str[i] 		= (u8)((u32Reg >> (i * 8)) & 0xFF);
		str[4+i] 	= (u8)((u32Value >> (i * 8)) & 0xFF);
	}
	
	if(!_hal_frc_WriteNumBytes(0, 8, str))
	{
		bResult = FALSE;
		return bResult;
	}
	
	
	return bResult;
}

bool HAL_FRC_WriteBurst(u32 u32Addr, unsigned char *data, u32 len)
{
	u8 buf[IIC_BURST_LEN];    
	u32 i;    

	if (len == 0)        
		return FALSE;
	if(data == NULL)
		return FALSE;
	  
	buf[0] = 0x0;    
	if(_hal_frc_WriteNumBytes(0x20, 1, buf)!=TRUE)
		return FALSE;
	
	for (i = 0; i < len; i++)        
		buf[i] = data[3 + ((i >> 2) << 2) - i % 4];    
	if(_hal_frc_WriteNumBytes(0x21, len, buf)!=TRUE)
		return FALSE;
	
	for (i = 0; i < 4; i++)        
		buf[i] = (unsigned char)((u32Addr >> (i * 8)) & 0xff);
	if(len<256)
		buf[i] = len -1; 
	else
		buf[i] = 0xFF;
	i++;
	buf[i] = 0x01;   
	if(_hal_frc_WriteNumBytes(0x10, 6, buf)!=TRUE)
		return FALSE;
	
	//printf("Write %d bytes to %x\n", len, u32Addr);  
	return TRUE;
}

bool HAL_FRC_Unlock(void)
{
	bool bResult = TRUE;
	bResult = _hal_frc_WriteByte(0xFE, 0x31);
	bResult = _hal_frc_WriteByte(0xFF, 0x72);

	if(bResult == FALSE)
	{
		//printf("HAL FRC UnLock fail \n");
	}
	return bResult;
}
