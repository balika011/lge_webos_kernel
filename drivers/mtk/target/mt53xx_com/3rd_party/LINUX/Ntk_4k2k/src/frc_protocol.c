/*! 
********************************************************************************
*  Copyright (c) Novatek Microelectronics Corp., Ltd. All Rights Reserved. 
*  \file    frc_protocol.c
*  \brief 
*  \version ver0.2 
*  \chip    NT72314
******************************************************************************** 
*/

/*-----------------------------------------------------------------------------*/
/* Include Header Files                                                        */
/*-----------------------------------------------------------------------------*/ 
#ifndef CC_MTK_LOADER
	#include <linux/module.h> 
	#include <linux/init.h> 
	#include <linux/fs.h> 
	#include <linux/uaccess.h> 
#endif
	

	#include "x_os.h"


#include "x_gpio.h"
#include "x_pdwnc.h"
//! Standard lib
#include "Def_types.h"

//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>

#include "frc_protocol.h"
#include "hal_frc_iic.h"

#define BOOTSTRAP_PIN_CONTROL



/*-----------------------------------------------------------------------------*/
/* Local Constant Definitions                                                  									       */
/*-----------------------------------------------------------------------------*/
#define FRC_REFIX     "#FRC# "
#define PRINT_FRC_ERR(x...)	printk(FRC_REFIX x)
#define PRINT_FRC_WRN(x...)	printk(FRC_REFIX x)
#define PRINT_FRC_DBG(x...)	printk(FRC_REFIX x)



#ifdef  __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////
///////////////////    These APIs are protocol API for upper application    /////////////
////////////////////////////////////////////////////////////////////////
//void *g_platfrc_sem = NULL;	//NT72314 can use clock-stretch for I2C access sync
static bool g_bFrcInit = FALSE;

static bool HAL_FRC_SetFRCBootStrap( u32 level);

FRC_Result API_FRC_Init(void)
{
	u32 ulNT72312Version;	
	u32 ulMainVersion;

	g_bFrcInit = HAL_FRC_Unlock() ;
	if( g_bFrcInit != TRUE)
	{
		PRINT_FRC_ERR("HAL FRC is failure to initialize\n");
		return FRC_NOT_OK;
	}

	if(API_FRC_Get_Version(&ulNT72312Version) != FRC_OK)
	{
		PRINT_FRC_ERR("NT72312 Get version is failed in initialization\n");
		return FRC_NOT_OK;
	}
	
	ulMainVersion = (ulNT72312Version>>12) & 0xFFF;	

	PRINT_FRC_DBG("FRC Initialized\n");

		
	return FRC_OK;
}

bool API_FRC_IsInit(void)
{
	return g_bFrcInit;
}

FRC_Result API_FRC_UnLock(void)
{
	FRC_Result	result = FRC_OK;

	if(HAL_FRC_Unlock() != TRUE)
	{
		PRINT_FRC_ERR("FRC Is NOT UnLock\n");
		result = FRC_NOT_OK;
	}

	return result;
}

FRC_Result API_FRC_Get_Version(u32 *u32Version)
{
	FRC_Result FRCRet = FRC_OK;
	u32 u32Value;
	
	if(API_FRC_IsInit() != TRUE)
	{
		PRINT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	if(u32Version == NULL)
		return FRC_PARAMETER_ERROR;


	if(HAL_FRC_ReadReg(FRC_REG_GET_VERSION, &u32Value) != TRUE)
	{
		PRINT_FRC_ERR("Get Version fail\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}

	*u32Version = u32Value;
END_FUN:
	return FRCRet;	
}


FRC_Result API_FRC_ReadReg(u32 u32RegAddr, u32 *pu32Val)
{
	FRC_Result FRCRet = FRC_OK;

	if(API_FRC_IsInit() != TRUE)
	{
		PRINT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	// Check valid
	if (pu32Val == NULL)
	{
		PRINT_FRC_ERR( "The buffer address is invalid \n");
		return FRC_NOT_OK;
	}
	
	// Read Register
	if(HAL_FRC_ReadReg(u32RegAddr, pu32Val) != TRUE)
	{
		PRINT_FRC_ERR("FRC Read Register fail\n");
		FRCRet =FRC_NOT_OK;
		goto END_FUN;
	}
	
	// End process
END_FUN:

	return FRCRet;
}

FRC_Result API_FRC_WriteReg(u32 u32RegAddr, u32 pu32Val)
{
	FRC_Result FRCRet = FRC_OK;

	if(API_FRC_IsInit() != TRUE)
	{
		PRINT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}
	
	
	// Write Register
	if(HAL_FRC_WriteReg(u32RegAddr, pu32Val) != TRUE)
	{
		PRINT_FRC_ERR("FRC Write Register fail\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}
	
	// End process
END_FUN:

	return FRCRet;
}


FRC_Result _frc_CheckCmdQueueStatus(EN_FRC_CMD_QUEUE_STATUS_k eCheckStatus, u32 u32RetryCount)
{
//	FRC_Result FRCRet = FRC_OK;
	u32 u32Status=0;
	u32 i=0;
	
	if(HAL_FRC_ReadReg(FRC_REG_CMD_QUEUE_STATUS, &u32Status) != TRUE)
	{
		PRINT_FRC_ERR("FRC Cmd queue status register read fail\n");
		return FRC_NOT_OK;
	}
	
	while(!((u32Status & 0xFF) & eCheckStatus) && (i<u32RetryCount))	//full
	{
		i++;
		if (i>1)
		{
			PRINT_FRC_DBG("----->>>>>WAIT to try again(%d).\n",i);
		}
		OS_DelayMS(10);
		
		if(HAL_FRC_ReadReg(FRC_REG_CMD_QUEUE_STATUS, &u32Status) != TRUE)
		{
			PRINT_FRC_ERR("FRC Cmd queue status register read fail\n");
			return FRC_NOT_OK;
		}
	}

	if((!(u32Status & 0xFF)) & eCheckStatus)
	{
		PRINT_FRC_ERR("FRC Cmd queue status error.\n");
		return FRC_NOT_OK;
	}
	
	return FRC_OK;
}


FRC_Result _frc_CheckCmdQueueAvailble(u32 u32RetryCount)
{
//	FRC_Result FRCRet = FRC_OK;
	u32 u32Cmd=0;
	u32 i=0;
	
	if(HAL_FRC_ReadReg(FRC_REG_CMD_QUEUE_ADDRESS, &u32Cmd) != TRUE)
	{
		PRINT_FRC_ERR("FRC Cmd queue address register read fail\n");
		return FRC_NOT_OK;
	}

	//72314 will clear Cmd register to 0xFFFFFFFF when after put the receving cmd to queue and the queue is not full
	while((u32Cmd!=0xFFFFFFFF) && (i<u32RetryCount))	//Cmd queue is full or Last Cmd not be confirm by FRC chip
	{
		i++;
		PRINT_FRC_DBG("----->>>>>WAIT to try again(%d).\n", i);
		OS_DelayMS(10);

		if(HAL_FRC_ReadReg(FRC_REG_CMD_QUEUE_ADDRESS, &u32Cmd) != TRUE)
		{
			PRINT_FRC_ERR("FRC Cmd queue address register read fail\n");
			return FRC_NOT_OK;
		}
	}

	if(u32Cmd!=0xFFFFFFFF)
	{
		PRINT_FRC_ERR("FRC Cmd queue address error.\n");
		return FRC_NOT_OK;
	}
	
	return FRC_OK;
}



FRC_Result API_FRC_WriteCmd(u8 u8MainType, u8 u8SubType, u32 u32Value, bool bWaitCmdDone)
{
	FRC_Result FRCRet = FRC_OK;
	u32 u32Cmd=0, u32Temp;

	
	if(API_FRC_IsInit() != TRUE)
	{
		PRINT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}


	// check if cmd queue availble
	//FRCRet = _frc_CheckCmdQueueStatus(EN_FRC_CMD_QUEUE_FULL|EN_FRC_CMD_QUEUE_SUSPEND, 100); // 1s
	FRCRet = _frc_CheckCmdQueueAvailble(100);
	if(FRCRet == FRC_NOT_OK)
	{
		PRINT_FRC_ERR("FRC Cmd queue access: Cmd queue always not availble, error.\n");
		goto END_FUN;
	}	

	
	u32Temp = (u8MainType & 0x3F);
	u32Cmd += u32Temp<<24;

	if (u8MainType < 16)
	{
		u32Temp = (u8SubType & 0xFF);
		u32Cmd += u32Temp<<16;

		u32Temp = (u32Value & 0xFFFF);
		u32Cmd += u32Temp;
	}
	else
	{
		u32Temp = (u32Value & 0xFFFFFF);
		u32Cmd += u32Temp;
	}
	
	printk("\n####Send Cmd=0x%08x; u32Value=%d\n", u32Cmd, u32Value );
	if(HAL_FRC_WriteReg(FRC_REG_CMD_QUEUE_ADDRESS, u32Cmd) != TRUE)
	{
		PRINT_FRC_ERR("FRC Cmd queue access should write again\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}
	

	if (bWaitCmdDone)	//wait cmd execute done
	{
		FRCRet = _frc_CheckCmdQueueAvailble(100);
		if(FRCRet == FRC_NOT_OK)
		{
			PRINT_FRC_ERR("FRC Cmd queue access: Cmd queue always not availble, error.\n");
			goto END_FUN;
		}
		
		FRCRet = _frc_CheckCmdQueueStatus(EN_FRC_CMD_QUEUE_EMPTY, 500);		// 5s
		if(FRCRet == FRC_NOT_OK)
		{
			PRINT_FRC_ERR("FRC Cmd queue access: Cmd not finish in 5 seconds!!. Impossible!\n");
			goto END_FUN;
		}	
	}

END_FUN:

	return FRCRet;

}

FRC_Result API_FRC_ReadCmd(u8 u8MainType, u8 u8SubType, u32* pu32Value)
{
	FRC_Result FRCRet = FRC_OK;
	u32 u32Cmd=0, u32Temp;
	
	if(API_FRC_IsInit() != TRUE)
	{
		PRINT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}

	// check if cmd queue full
	//FRCRet = _frc_CheckCmdQueueStatus(EN_FRC_CMD_QUEUE_FULL|EN_FRC_CMD_QUEUE_SUSPEND, 100); // 1s
	FRCRet = _frc_CheckCmdQueueAvailble(100);
	if(FRCRet == FRC_NOT_OK)
	{
		PRINT_FRC_ERR("FRC Cmd queue access: Cmd queue always not availble, error.\n");
		goto END_FUN;
	}

	u32Temp = (u8MainType & 0xF);
	u32Cmd += u32Temp<<24;

	u32Temp = (u8SubType & 0xFF);
	u32Cmd += u32Temp<<16;

	u32Cmd |= 0x80000000;	//set u1ReadCmd bit

	if(HAL_FRC_WriteReg(FRC_REG_CMD_QUEUE_ADDRESS, u32Cmd) != TRUE)
	{
		PRINT_FRC_ERR("FRC Cmd queue access should write again\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}

	FRCRet = _frc_CheckCmdQueueAvailble(100);
	if(FRCRet == FRC_NOT_OK)
	{
		PRINT_FRC_ERR("FRC Cmd queue access: Cmd queue always not availble, error.\n");
		goto END_FUN;
	}

	//wait read cmd has been execute by FRC
	FRCRet = _frc_CheckCmdQueueStatus(EN_FRC_CMD_QUEUE_EMPTY, 500);		// 5s
	if(FRCRet == FRC_NOT_OK)
	{
		PRINT_FRC_ERR("FRC Cmd queue access: Cmd not finish in 5 seconds!!. Impossible!\n");
		goto END_FUN;
	}	

	if(HAL_FRC_ReadReg(FRC_REG_CMD_READ_VALUE, pu32Value) != TRUE)
	{
		PRINT_FRC_ERR("FRC Cmd queue access should read again\n");
		FRCRet = FRC_NOT_OK;
		goto END_FUN;
	}
	

END_FUN:

	return FRCRet;
}

////////////////////////////////////////////////////////////////////////
///////////////       These APIs for update FRC image by I2C  bus     /////////////////
////////////////////////////////////////////////////////////////////////
//#define SHOW_UPDATE_TIMING
//#define Dual_312_Image
typedef enum
{
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_PARSING_HEADER = 0,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_LOAD_SUBPROGRAM,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_PASSWORD1_TO_JUMP_SRAM,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_WAIT_PASSWORD1_READY,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_LOAD_FLOADER_TO_DDR,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_PASSWORD2_TO_JUMP_DDR,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_WAIT_PASSWORD2_READY,
	EN_PLAT_FRC_UPDATE_COMMAND_NEW_MAX,
  
}EN_PLAT_FRC_UPDATE_COMMAND_new_k;

typedef enum
{
	EN_PLAT_FRC_PASSWORD_SET_PWD1_TO_JUMP_SRAM = 0,
	EN_PLAT_FRC_PASSWORD_WAIT_PWD1_READY,
	EN_PLAT_FRC_PASSWORD_SET_PWD2_TO_JUMP_DDR,
	EN_PLAT_FRC_PASSWORD_WAIT_PWD2_READY,
	EN_PLAT_FRC_PASSWORD_SET_PWD_TO_REBOOT_IN_I2C,
	EN_PLAT_FRC_PASSWORD_MAX,
}EN_PLAT_FRC_PASSWORD_COMMAND_k;

typedef struct ST_PLAT_FRC_PASSWORD_TABLE_s
{
	EN_PLAT_FRC_PASSWORD_COMMAND_k		eCommandID;
	u32										u32Addr;
	u32                							u32PWD;
}ST_PLAT_FRC_PASSWORD_TABLE_t;

static ST_PLAT_FRC_PASSWORD_TABLE_t FRC_PasswordList[]=
{
	{EN_PLAT_FRC_PASSWORD_SET_PWD1_TO_JUMP_SRAM,	0x9F040000, 0x72310000},
	{EN_PLAT_FRC_PASSWORD_WAIT_PWD1_READY,			0x9F0A010C, 0x7231FFFF},
	{EN_PLAT_FRC_PASSWORD_SET_PWD2_TO_JUMP_DDR,		0x80001000, 0x72310000},
	{EN_PLAT_FRC_PASSWORD_WAIT_PWD2_READY,			0x80001000, 0x7231FFFF},
	{EN_PLAT_FRC_PASSWORD_SET_PWD_TO_REBOOT_IN_I2C,	0x9F040000, 0x72310000}
};


typedef struct ST_PLAT_FRC_FWHeader_s
{
	char ID_String[32];
	u32 Subprogram_Addr;
	u32 Subprogram_Size;
	u32 Subprogram_Download_Addr;
	u32 Subprogram_Checksum;
	u32 Floader_Addr;
	u32 Floader_Size;
	u32 Floader_Download_Addr;
	u32 Floader_Checksum;	
}ST_PLAT_FRC_FWHeader_t;

static ST_PLAT_FRC_FWHeader_t	  gFRCFW_Header;
static bool b312_Update_FW_Enable = FALSE;
#if !defined(CC_MTK_LOADER) 
u32 convertBuffToU32(char *tmpBuff)
{
	u32 ret = 0;
	int i = 0;

#if 0	
	printk("\n tmpbuff =");
	for(i=0;i<4;i++)
		printk("%x ", tmpBuff[i]);
	printk("\n");
#endif
	
	for(i=4;i > 0;i--)
	{
		ret = ret << 8;
		ret += ((u32)tmpBuff[i-1])&0x000000ff;
		
	}

	return ret;
}
FRC_Result _frc_Update_LoadCode(u32 StartAddr, u32 ByteSize, struct file *fp)
{
	u32 readBytes = 1;
	u8 data[256];
	u32 Len;
	u32 SizeCount = 0;
	u32 BurstLen = 256;
	
	mm_segment_t old_fs;
    ssize_t ret;
	
	if(fp != NULL)
	{
		old_fs = get_fs();
		set_fs(get_ds());
		
		while(SizeCount < ByteSize)
		{
			if((ByteSize-SizeCount)>BurstLen)
				Len = BurstLen;
			else
				Len = ByteSize-SizeCount;
			
			// readBytes = fread(data, sizeof(unsigned char), Len, fp);
			ret = fp->f_op->read(fp, data, Len, &fp->f_pos);
			readBytes = ret;
			//--PRINT_FRC_DBG("### Loading - StartAddr:%08x, readBytes=%08x [%02x, %02x, %02x,%02x][%02x, %02x, %02x,%02x]\n", StartAddr, readBytes,data[0], data[1], data[2], data[3], data[readBytes-4], data[readBytes-3], data[readBytes-2], data[readBytes-1]);
			if(HAL_FRC_WriteBurst(StartAddr, data, readBytes) != TRUE)
			{
				set_fs(old_fs);
				return FRC_NOT_OK;
			}
			SizeCount = SizeCount+readBytes;

			StartAddr += readBytes;
			if((0x9F040000<=StartAddr) && (StartAddr<=0x9F05BFFF))//--this checking only for SRAM
			{
				if (StartAddr == 0x9F04C000)            
					StartAddr = 0x9F050000; 
			}
		}
		set_fs(old_fs);
		return FRC_OK;
	}
	else
	{
		PRINT_FRC_ERR("FP is NULL \n");
		return FRC_PARAMETER_ERROR;
	}
}


FRC_Result _frc_Update_PasswordHandler(EN_PLAT_FRC_PASSWORD_COMMAND_k eCommandID)
{
	FRC_Result FRCRet = FRC_OK;
	u32 u32Addr, u32PWD;
	u32 u32CheckVal;
	u32 counter = 0;
	if(eCommandID >= EN_PLAT_FRC_PASSWORD_MAX)
	{
		FRCRet = FRC_PARAMETER_ERROR;
		PRINT_FRC_ERR("CommandID is illegal \n");
		goto END_FUN;
	}
	u32Addr = FRC_PasswordList[eCommandID].u32Addr;
	u32PWD = FRC_PasswordList[eCommandID].u32PWD;
	switch(eCommandID)
	{
		case EN_PLAT_FRC_PASSWORD_SET_PWD1_TO_JUMP_SRAM:
			if(HAL_FRC_WriteReg(u32Addr, u32PWD) != TRUE)
				FRCRet = FRC_NOT_OK;
			break;
		case EN_PLAT_FRC_PASSWORD_WAIT_PWD1_READY:
			do
			{
				//usleep(10*1000); 
				OS_DelayMS(10);
				if(HAL_FRC_ReadReg(u32Addr, &u32CheckVal) != TRUE)
					counter = 100;
				counter++;
			}while((u32CheckVal != u32PWD)&&(counter<=100));
			if(u32CheckVal != u32PWD)
				FRCRet = FRC_NOT_OK;
			break;
		case EN_PLAT_FRC_PASSWORD_SET_PWD2_TO_JUMP_DDR:
			if(HAL_FRC_WriteReg(u32Addr, u32PWD) != TRUE)
				FRCRet = FRC_NOT_OK; 
			break;
		case EN_PLAT_FRC_PASSWORD_WAIT_PWD2_READY:
			do
			{
				//usleep(500*1000); 
				OS_DelayMS(500);
				if(HAL_FRC_ReadReg(u32Addr, &u32CheckVal) != TRUE)
					counter = 100;
				counter++;
			}while((u32CheckVal != u32PWD)&&(counter<=100));
			if(u32CheckVal != u32PWD)
				FRCRet = FRC_NOT_OK;
			break;
		case EN_PLAT_FRC_PASSWORD_SET_PWD_TO_REBOOT_IN_I2C:
			if(HAL_FRC_WriteReg(u32Addr, u32PWD) != TRUE)
				FRCRet = FRC_NOT_OK;
			break;
		default:
			PRINT_FRC_ERR("CommandID is illegal \n");
			break;
	}

	END_FUN:
		return FRCRet;
}

FRC_Result _frc_Update_ParsingFWHeader(struct file *fp)
{
	FRC_Result FRCRet = FRC_OK;
	//u32 ulReadbytes;
	u32 i;
	char tmp[10];
	mm_segment_t old_fs;
    ssize_t ret;
	
	if(fp == NULL)
	{
		FRCRet = FRC_PARAMETER_ERROR;
		PRINT_FRC_ERR("FP is NULL \n");
		goto END_FUN;
	}

	old_fs = get_fs();
    set_fs(get_ds());
	
	for(i=0;i<32;i++)
	{
		ret = fp->f_op->read(fp, tmp, 1, &fp->f_pos);
		gFRCFW_Header.ID_String[i]=(char)tmp[0];
	}
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Subprogram_Addr = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Subprogram_Size = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Subprogram_Download_Addr = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Subprogram_Checksum = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Floader_Addr = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Floader_Size = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Floader_Download_Addr = convertBuffToU32(tmp);
	ret = fp->f_op->read(fp, tmp, 4, &fp->f_pos);
	gFRCFW_Header.Floader_Checksum = convertBuffToU32(tmp);
	gFRCFW_Header.Subprogram_Download_Addr = 0x9F040040;
	
	set_fs(old_fs);
#if 0
	printk("\nID string=");
	for(i=0;i<32;i++)
		printk("%c", gFRCFW_Header.ID_String[i]);
	printk("\n");
	printk("gFRCFW_Header.Subprogram_Addr=%08x\n", gFRCFW_Header.Subprogram_Addr);
	printk("gFRCFW_Header.Subprogram_Size=%08x\n", gFRCFW_Header.Subprogram_Size);
	printk("gFRCFW_Header.Subprogram_Download_Addr=%08x\n", gFRCFW_Header.Subprogram_Download_Addr);
	printk("gFRCFW_Header.Subprogram_Checksum=%08x\n", gFRCFW_Header.Subprogram_Checksum);
	printk("gFRCFW_Header.Floader_Addr=%08x\n", gFRCFW_Header.Floader_Addr);
	printk("gFRCFW_Header.Floader_Size=%08x\n", gFRCFW_Header.Floader_Size);
	printk("gFRCFW_Header.Floader_Download_Addr=%08x\n", gFRCFW_Header.Floader_Download_Addr);
	printk("gFRCFW_Header.Floader_Checksum=%08x\n", gFRCFW_Header.Floader_Checksum);
#endif
	END_FUN:
		return FRCRet;
}

FRC_Result _frc_Update_LoadSubProgram_new(struct file *fp)
{
	FRC_Result FRCRet = FRC_OK;
	
	mm_segment_t old_fs;
	// ssize_t ret;
	
	if(fp == NULL)
		return FRC_PARAMETER_ERROR;
	
	old_fs = get_fs();
    set_fs(get_ds());
	// ret = fseek(fp, 0, SEEK_SET);
	// ret = fseek(fp, gFRCFW_Header.Subprogram_Addr, SEEK_SET);
	fp->f_op->llseek(fp,gFRCFW_Header.Subprogram_Addr,0);
	set_fs(old_fs);
	if(_frc_Update_LoadCode(gFRCFW_Header.Subprogram_Download_Addr, gFRCFW_Header.Subprogram_Size, fp) != FRC_OK)
		return FRC_NOT_OK;

	return FRCRet;
}
FRC_Result _frc_Update_LoadFloader(struct file *fp)
{
	FRC_Result FRCRet = FRC_OK;
	
	mm_segment_t old_fs;
	// ssize_t ret;
	
	if(fp == NULL)
		return FRC_PARAMETER_ERROR;
	
	old_fs = get_fs();
    set_fs(get_ds());
	// ret = fseek(fp, 0, SEEK_SET);
	// ret = fseek(fp, gFRCFW_Header.Floader_Addr, SEEK_SET);
	fp->f_op->llseek(fp,gFRCFW_Header.Floader_Addr,0);
	set_fs(old_fs);
	
	if(_frc_Update_LoadCode(gFRCFW_Header.Floader_Download_Addr, gFRCFW_Header.Floader_Size, fp) != FRC_OK)
		return FRC_NOT_OK;

	return FRCRet;
}

 FRC_Result _frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_new_k enUpdateStepID, struct file *fp)
{
	FRC_Result FRCRet = FRC_OK;
	if((enUpdateStepID >= EN_PLAT_FRC_UPDATE_COMMAND_NEW_MAX) || (fp == NULL))
	{
		FRCRet = FRC_PARAMETER_ERROR;
		PRINT_FRC_ERR("CommandID is illegal \n");
		goto END_FUN;
	}
	switch(enUpdateStepID)
	{
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_PARSING_HEADER:
			FRCRet = _frc_Update_ParsingFWHeader(fp);
			break;
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_LOAD_SUBPROGRAM:
			FRCRet = _frc_Update_LoadSubProgram_new(fp);
			break;
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_PASSWORD1_TO_JUMP_SRAM:
			FRCRet = _frc_Update_PasswordHandler(EN_PLAT_FRC_PASSWORD_SET_PWD1_TO_JUMP_SRAM);
			break;
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_WAIT_PASSWORD1_READY:
			FRCRet = _frc_Update_PasswordHandler(EN_PLAT_FRC_PASSWORD_WAIT_PWD1_READY);
			break;
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_LOAD_FLOADER_TO_DDR:
			FRCRet = _frc_Update_LoadFloader(fp);
			break;
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_PASSWORD2_TO_JUMP_DDR:
			FRCRet = _frc_Update_PasswordHandler(EN_PLAT_FRC_PASSWORD_SET_PWD2_TO_JUMP_DDR);
			break;
		case EN_PLAT_FRC_UPDATE_COMMAND_NEW_WAIT_PASSWORD2_READY:
			FRCRet = _frc_Update_PasswordHandler(EN_PLAT_FRC_PASSWORD_WAIT_PWD2_READY);
			break;
		default:
			PRINT_FRC_ERR("CommandID is illegal \n");
			break;
	}
	END_FUN:
		return FRCRet;
}


FRC_Result _frc_Update_FW_new(struct file *fp)
{
	FRC_Result FRCRet = FRC_OK;
	if(fp == NULL)
	{
		FRCRet = FRC_PARAMETER_ERROR;
		PRINT_FRC_ERR("File pointer is NULL\n");
	}
	else
	{
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_PARSING_HEADER, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Parsing table error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Parse table -done\n");
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_LOAD_SUBPROGRAM, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Load sub program error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Load subprogram -done\n");
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_PASSWORD1_TO_JUMP_SRAM, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Set password1 error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Send password1 -done\n");
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_WAIT_PASSWORD1_READY, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Wait password1 response error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Check password1-done\n");
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_LOAD_FLOADER_TO_DDR, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Load IMG to DDR error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Load Floader -done\n");
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_PASSWORD2_TO_JUMP_DDR, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Set password2 error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Send password2 -done\n");
		if(_frc_Update_Procedure_new(EN_PLAT_FRC_UPDATE_COMMAND_NEW_WAIT_PASSWORD2_READY, fp) != FRC_OK)
		{
			PRINT_FRC_ERR("FRC F/W update fail (Set password2 error)\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		PRINT_FRC_DBG("Check password2 -done\n");
	}
	END_FUN:
		return FRCRet;
}

FRC_Result _frc_Update_Reset2ROM(void)
{
	if(API_FRC_IsInit() != TRUE)
	{
		PRINT_FRC_ERR("FRC IS NOT Initialized\n");
		return FRC_NOT_OK;
	}

	//write a password to FRC_REG_CMD_QUEUE_ADDRESS register
	if(HAL_FRC_WriteReg(FRC_REG_CMD_QUEUE_ADDRESS, 0x72317231) != TRUE)
	{
		PRINT_FRC_ERR("FRC write reg fail\n");
		return FRC_NOT_OK;
	}

	return FRC_OK;
}

FRC_Result API_FRC_FW_UpdateProc(struct file *fp)
{
	FRC_Result FRCRet = FRC_OK;	
	
	if(fp == NULL)
	{
		PRINT_FRC_ERR("File pointer is NULL\n");
		return FRC_PARAMETER_ERROR;
	}
	else
	{
	
#ifdef BOOTSTRAP_PIN_CONTROL
		//--Set 312 BootStrap to I2C mode
		if(HAL_FRC_SetFRCBootStrap(0) != TRUE)
		{
			FRCRet = FRC_NOT_OK;
			PRINT_FRC_ERR("NT72312 BS control fail (Low)\n");
			goto END_FUN;
		}
		
		if(HAL_FRC_Reset() != FRC_OK)
		{
			FRCRet = FRC_NOT_OK;
			PRINT_FRC_ERR("NT72312 reset error \n");
			goto END_FUN;
		}
#else
		//SW reset to 312 rom code
		if(_frc_Update_Reset2ROM() != FRC_OK)
		{
			FRCRet = FRC_NOT_OK;
			PRINT_FRC_ERR("NT72312 reset to ROM fail\n");
			goto END_FUN;
		}
#endif		
		//usleep(1000*100);
		OS_DelayMS(100);

		if(HAL_FRC_Unlock() != TRUE)
		{
			PRINT_FRC_ERR("FRC Is NOT UnLock\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
		
		if(_frc_Update_FW_new(fp) != FRC_OK)
		{
			FRCRet = FRC_NOT_OK;
				PRINT_FRC_ERR("NT72312 update fail\n");
			goto END_FUN;
		}

#ifdef BOOTSTRAP_PIN_CONTROL		
		if(HAL_FRC_SetFRCBootStrap( EN_GPIO_LEVEL_HIGH) != TRUE)
		{
			FRCRet = FRC_NOT_OK;
			PRINT_FRC_ERR("NT72312 BS control fail (High)\n");
			goto END_FUN;
		}
#endif
		
		if(HAL_FRC_Reset() != FRC_OK)
		{
			FRCRet = FRC_NOT_OK;
			PRINT_FRC_ERR("NT72312 reset error \n");
			goto END_FUN;
		}

		//usleep(1000*100);
		OS_DelayMS(100);
		
		if(HAL_FRC_Unlock() != TRUE)
		{
			PRINT_FRC_ERR("FRC Is NOT UnLock\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}

		// Dealy 2 sec to wait 312 enter main loop
		//usleep(1000*2000);
		OS_DelayMS(200);


	}
	
	END_FUN:
	
		return FRCRet;
}

//Function is identical to API_FRC_Update_OnePackage but with the file as parameter and not hardcoded.
#if 0
FRC_Result API_FRC_FW_UpdateFrimware(char path1[])
{
	FRC_Result FRCRet = FRC_OK;
	FILE *fp;
	//char path1[]="/mtd_exe/resource/FRC_FW/result_I2C.bin";	//image path
	//char path1[]= filepath;
	char *path_name;
	//bool bChangeHandshaking = FALSE;

	
	// Set Update FW Flag
	b312_Update_FW_Enable = TRUE;

	// Decide DDR type
	path_name = path1;

	// Start updating FW
	fp = fopen(path_name, "r");
	if(fp == NULL)
	{
		FRCRet = FRC_NOT_OK;
		PRINT_FRC_ERR("Can not find [result_I2C.bin]\n");
		goto END_FUN;
	}
	else
	{	
		if(API_FRC_FW_UpdateProc(fp)!= FRC_OK)
		{
			PRINT_FRC_ERR("Burning FW FAIL!!!!\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
	}
	
END_FUN:
	if(fp != NULL)
		fclose(fp);
	

	// Release Update FW Flag
	b312_Update_FW_Enable = FALSE;
	
	if(FRCRet == FRC_OK)
	{
		FRC_Result Ret;
		u32 ulVersion;
		//usleep(1000*50);
		OS_DelayMS(50);
		Ret = API_FRC_Get_Version(&ulVersion);
		PRINT_FRC_DBG("312 F/W (Version: %08x)update is successful \n",ulVersion);
		//ulVersion = (ulVersion>>12) & 0xFFF;
	}

	return FRCRet;
}

#endif

FRC_Result API_FRC_Update_OnePackage(char *Path)
{
	FRC_Result FRCRet = FRC_OK;
	struct file *fp;
	char path1[]="/mnt/sdcard/FRC_FW/result_I2C.bin";	//image path
	char *path_name;
	//bool bChangeHandshaking = FALSE;

	fp = NULL;
	// Set Update FW Flag
	b312_Update_FW_Enable = TRUE;

	// Decide DDR type
	path_name = path1;	// Path;

	// Start updating FW
	fp = filp_open(path_name, O_RDONLY , 0644); 

	if (IS_ERR(fp)) 
	{
		FRCRet = FRC_NOT_OK;
		PRINT_FRC_ERR("Can not find [result_I2C.bin]\n");
		fp = NULL;
		goto END_FUN;
	}
	else
	{	
		if(API_FRC_FW_UpdateProc(fp)!= FRC_OK)
		{
			PRINT_FRC_ERR("Burning FW FAIL!!!!\n");
			FRCRet = FRC_NOT_OK;
			goto END_FUN;
		}
	}
	
END_FUN:
	if(fp != NULL)
		filp_close(fp,NULL); 
	

	// Release Update FW Flag
	b312_Update_FW_Enable = FALSE;
	
	if(FRCRet == FRC_OK)
	{
		FRC_Result Ret;
		u32 ulVersion;
		//usleep(1000*50);
		OS_DelayMS(50);
		Ret = API_FRC_Get_Version(&ulVersion);
		//PRINT_FRC_DBG("312 F/W (Version: %08x)update is successful \n", ulVersion);
		//ulVersion = (ulVersion>>12) & 0xFFF;
	}

	return FRCRet;
}
#endif

static bool HAL_FRC_SetFRCBootStrap(u32 level)
{
	UINT32 u4Val;
	if(0 == level)
	{
		u4Val = 0;
	}else{
		u4Val = 1;
	}
	GPIO_Output((GPIO(109)),&u4Val);
	return TRUE;
}
FRC_Result HAL_FRC_Reset(void)
{
	UINT32 u4Val;
	u4Val = 0;
    GPIO_Output((GPIO(55)),&u4Val);
	OS_DelayMS(500);
	u4Val = 1;
    GPIO_Output((GPIO(55)),&u4Val);	
	return FRC_OK;	
//	return FRC_NOT_OK;
}


#ifdef  __cplusplus
}
#endif

