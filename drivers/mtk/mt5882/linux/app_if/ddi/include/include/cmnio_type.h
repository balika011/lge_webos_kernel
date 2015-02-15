/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   Permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 	cmnio_type.h
 *
 *  Definitions for global configurations
 *  DESCRIPTION : In this file, all global configurations, enumerations, data
			   or structures should be shared between modules positioned in
			   different layers.
			   For example, enumeration for external video supported in the system,
			   should be referenced in User Interface module (APP layer) and,
			   at the same time, referenced Video module(Driver Layer).
			   So, these kind of definition or declaration should be stated
			   here.
 *  @author	Baekwon Choi (�ֹ��, bk1472@lge.com)
 *  @version 	1.1
 *  @date	2008.06.20
 *  @see
 */

#ifndef _CMNIO_TYPE_H_

#define _CMNIO_TYPE_H_

/*-----------------------------------------------------------------------------
	���� ���
	(Control Constants)
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	#include ���ϵ�
	(File Inclusions)
------------------------------------------------------------------------------*/
#include "branches.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*-----------------------------------------------------------------------------
	��ũ�� �Լ�  ����
	(Macros Definitions)
------------------------------------------------------------------------------*/
#ifdef _CMN_VAR_CONF_C_
#define CMN_VAR(vType, vName, vValue)	vType vName = (vType) vValue
#define CMN_EXT
#else
#define CMN_VAR(vType, vName, vValue)	extern vType vName
#define CMN_EXT                         extern
#endif

/*-----------------------------------------------------------------------------
	��� ����
	(Constant Definitions)
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * NVRAM ���� ��ġ
 *   - NVM_EEPROM_ONLY   : Support only I2C-EEPROM
 *   - NVM_EEPROM_N_NAND : Support both I2C-EEPROM and NAND flash
 *
 * 2010.07.27. by gunho.lee
 *-------------------------------------------------------------------------*/
#define	NVM_EEPROM_ONLY					0
#define	NVM_EEPROM_N_NAND				1
#define TNVM_MAGIC_CODE					0x20100118

#define EEPROM_SIZE						0x8000 // 32K

/*------------------------------------------
 * NVM DB Map
 *------------------------------------------*/
/*
 * Jackee's Note 2008/11/21: Size ���� ���� ����.
 * ��� DB�� NVMDRV_PAGE_SIZE�� align ó���ϴ°� ����. �׷��� ������,
 * NVRAM read/write �� �ӵ� ���� ������ ���� �� �ִ�.
 */

/* NVM Header */
#define NVM_HEADER_SIZE					16

/* NVM Magic */
#define TNVM_MAGIC_SIZE					4

/* NVM DB */
#define SYS_DB_SIZE						400		/** 0180(384)  : System DB, should be first DB	**/
#define ANA_DB_SIZE						832		/** 0340(832) : Analog Driver + Calibration DB	**/
#define TOOL_OPTION_DB_SIZE				128		/** 0080(128)  : Tool Option DB, reserved space = 108	**/
#define FACTORY_DB_SIZE					288		/** 0100(256)  : Factory DB, reserved space = 8	256-288-320**/
#define SUMODE_10POINTWB_DB_SIZE		128		/** 0080(128) : Sevice Mode �� 10 Point White Balance DB **/
#define MODEL_INFO_DB_SIZE				48		/** 0030(48) : Model Info DB **/
#define PRESERVE_DB_SIZE				16		/** 0010(16) : Preserve DB **/
#define EMP_DB_SIZE 					128		/** 0080(128)  : DivX DRM Information  			**/
#define SWU_DB_SIZE						4096
#define MHP_DB_SIZE						4171
#define MICOM_DB_SIZE					128		/** 0080(128)  : Internal MICOM DB				**/
#define THX_DB_SIZE						68		/** 0080(128) : THX �� 10 Point White Balance DB **/
#define NEW_FACTORY_DB_SIZE				64		/** 0040(64)  : New Factory DB		**/
#define	NETLOG_DB_SIZE					804		/** 0194(404) : Net Server Log DB. size �߸���. 804�� ����. jaehan.park 101213 **/
#define	TMP_CPLIST_DB_SIZE				700		/** 700  TEMP_CPLIST_DB. GP4���� UI DB�� �̵� **/
#define	MAC_ADDRESS_SIZE				32		/** MAC Address�� eeprom�� backup **/
#define SOC_VENDOR_SIZE					64		/** 0040(64)  : Vendor data 		**/

#define UI_DB_SIZE						50000	//38888	/** 9400(37888) : UI DB(edge_enhancer,xvYCC,thx)	**/	//23284 --> 37888 -->38888
#define UI_EXPERT_DB_SIZE				38000	//33792	/** 8400(33792) : UI Expert DB(edge, xvYVV,			**/	//11700 --> 28672 --> 33792



//#if 0 //#if(NVRAM_DEV  == NVM_EEPROM_ONLY)
#if 0 //#if (PLATFORM_TYPE == BCM_PLATFORM)
#define CH_DB_SIZE						(40000)	// in case of 128KB EEPROM
#else
#define CH_DB_SIZE						(108480  + 0x8000)	/** a200(41472): Channel DB(3k for DRRT)		**/
#endif
#define DVBS_DB_SIZE					(0xC8000 - 0x8000)	/* 800 K*/
#ifdef INCLUDE_SYS_ISDB
#define CA_MAIL_DB_SIZE					800*30
#define UI_MAIL_DB_SIZE					17408
#endif




#define	BASIC_DB_SIZE					( NVM_HEADER_SIZE				\
										+ TNVM_MAGIC_SIZE				\
										+ SYS_DB_SIZE 					\
										+ ANA_DB_SIZE 					\
										+ TOOL_OPTION_DB_SIZE			\
										+ FACTORY_DB_SIZE				\
										+ SUMODE_10POINTWB_DB_SIZE		\
										+ MODEL_INFO_DB_SIZE			\
										+ PRESERVE_DB_SIZE				\
										+ EMP_DB_SIZE					\
										+ SWU_DB_SIZE					\
										+ MHP_DB_SIZE					\
										+ MICOM_DB_SIZE					\
										+ THX_DB_SIZE					\
										+ NEW_FACTORY_DB_SIZE			\
										+ NETLOG_DB_SIZE				\
										+ TMP_CPLIST_DB_SIZE			\
										+ MAC_ADDRESS_SIZE)


#define EXTEND_DVBS_DB_SIZE    			( UI_DB_SIZE					\
										+ UI_EXPERT_DB_SIZE				\
										+ CH_DB_SIZE					\
										+ DVBS_DB_SIZE )
#ifdef INCLUDE_SYS_ISDB
#define EXTEND_DB_SIZE    				( UI_DB_SIZE					\
										+ UI_EXPERT_DB_SIZE				\
										+ CA_MAIL_DB_SIZE				\
										+ UI_MAIL_DB_SIZE				\
										+ CH_DB_SIZE)

#else
#define EXTEND_DB_SIZE    				( UI_DB_SIZE					\
										+ UI_EXPERT_DB_SIZE				\
										+ CH_DB_SIZE )
#endif

#define TOTAL_DB_SIZE					(BASIC_DB_SIZE + EXTEND_DB_SIZE)

#define TOTAL_DVBS_DB_SIZE				(BASIC_DB_SIZE + EXTEND_DVBS_DB_SIZE)


/*
 * ANA_DB�� FACTORY_DB�� NVM MAP�� ����Ǵ���, ���� �����ؾ� �մϴ�.
 * NVM MAP ����� ANA_DB�� FACTORY_DB�� Reset���� �ʵ��� ������ ����� �ֽʽÿ�.
 */
/* BASE OFFSET */
#define NVM_HEADER_BASE   			( 0                                     )
#define TNVM_MAGIC_BASE				( NVM_HEADER_BASE   + NVM_HEADER_SIZE 	)
#define SYS_DB_BASE       			( TNVM_MAGIC_BASE 	+ TNVM_MAGIC_SIZE 	)
#define ANA_DB_BASE       			( SYS_DB_BASE 		+ SYS_DB_SIZE 		)
#define TOOL_OPTION_DB_BASE   		( ANA_DB_BASE       + ANA_DB_SIZE       )
#define FACTORY_DB_BASE   			( TOOL_OPTION_DB_BASE+ TOOL_OPTION_DB_SIZE )
#define	SUMODE_10POINTWB_DB_BASE	( FACTORY_DB_BASE	+ FACTORY_DB_SIZE		)
#define	MODEL_INFO_DB_BASE			(SUMODE_10POINTWB_DB_BASE	+ SUMODE_10POINTWB_DB_SIZE	)
#define	PRESERVE_DB_BASE			(MODEL_INFO_DB_BASE	+ MODEL_INFO_DB_SIZE	)
#define	EMP_DB_BASE     			( PRESERVE_DB_BASE  + PRESERVE_DB_SIZE 	)
#define	SWU_DB_BASE	 				( EMP_DB_BASE		+ EMP_DB_SIZE 		)
#define	MHP_DB_BASE	 				( SWU_DB_BASE		+ SWU_DB_SIZE 		)
#define	MICOM_DB_BASE     			( MHP_DB_BASE		+ MHP_DB_SIZE		)
#define	THX_DB_BASE					(MICOM_DB_BASE		+ MICOM_DB_SIZE		)
#define NEW_FACTORY_DB_BASE   		( THX_DB_BASE		+ THX_DB_SIZE 		)
#define	NETLOG_DB_BASE				( NEW_FACTORY_DB_BASE + NEW_FACTORY_DB_SIZE	)
#define	TMP_CPLIST_DB_BASE			( NETLOG_DB_BASE	  + NETLOG_DB_SIZE )
#define	MAC_ADDRESS_BASE			( TMP_CPLIST_DB_BASE  + TMP_CPLIST_DB_SIZE )


/* NAND FLASH ���� */
#define UI_DB_BASE        			( MAC_ADDRESS_BASE	  + MAC_ADDRESS_SIZE)
#define UI_EXPERT_DB_BASE 			( UI_DB_BASE 		+ UI_DB_SIZE 		)
#define	CH_DB_BASE        			( UI_EXPERT_DB_BASE	+ UI_EXPERT_DB_SIZE	)
#ifdef INCLUDE_SYS_ISDB
#define	CA_MAIL_DB_BASE        		( CH_DB_BASE		+ CH_DB_SIZE		)
#define	UI_MAIL_DB_BASE        		( CA_MAIL_DB_BASE	+ CA_MAIL_DB_SIZE	)
#define DVBS_DB_BASE				( UI_MAIL_DB_BASE  	+ UI_MAIL_DB_SIZE	)
#else
#define DVBS_DB_BASE				( CH_DB_BASE     	+ CH_DB_SIZE		)
#endif

/*ALEF EEPROM ���� */
#define ALEF_DB_BASE				( 0														)
#define ALEF_THX_DB_BASE			( ALEF_DB_BASE		+ ANA_DB_SIZE				)


// request from L9 SIC 
#define	SOC_VENDOR_BASE				( EEPROM_SIZE		- SOC_VENDOR_SIZE - 10)

#if ( BASIC_DB_SIZE > SOC_VENDOR_BASE)
#error "We need more EEPROM NVM size !!!!!"
#endif

/*
 * *** ���ο� ���� �߰��� �̰��� ���ּ���. ***
 * EMP_DB_SIZE, ACAP_DB�� MICOM_DB�� NVM ������ �κ����� ������Ų �����Դϴ�.
 * ����, EMP_DB_SIZE, ACAP_DB�� MICOM_DB�� total Magic Number�� �ٲ���� ���� �������� �ʰ� �߽��ϴ�.
 */

#if 0

#define  DVBS_DB_BASE				(PRESERVE_DB_BASE     + PRESERVE_DB_SIZE)
#define  DVBS_DB_SIZE				0xC8000 /* 800K */ //0xAF000 /* 700 K*/
#define NVRAM_SIZE        			( DVBS_DB_BASE     + DVBS_DB_SIZE - 1		)

#endif
/*
* stonedef - 091208
* 	SUMODE_10POINTWB_DB �� THX_DB �� valid mark �� ����ü ���� �� �ڵ����� �������
* 	�ʵ��� �Ͽ��� - ������ �ʿ��ϸ� valid mark �� �������� �ٲ���� ��
*/


/*
 * Jackee's Note 2008/11/21: Magic ���� ó�� ����.
 *		TOTAL_MAGIC�� ���� DB Reset�� �������� �ʵ��� �Ѵ�.
 *		��� ����ü ���濡 ���� �ڵ� RESET�� Ȱ���Ѵ�.
 *		Ư�� DB�� Reset�ϰ� ������, �ش� DB�� ũ�Ⱑ 0�� array�� �����ϰų�
 *		������ �����ߴ� array member�� �̸��� �����ϸ� �ȴ�.
 *		�̸� ����� ������ ���� ��ȣ �Ҵ翡 ����ߴ� ��İ� ����ϰ�, ��¥�� ������
 *		�Ϸ� ��ȣ�� �����ϰ� �ϸ� �������� ���� �� �ִ�.
 *		��а� ������ ���� ���⿡ ����Ǿ��� ����� C �ҽ��� �̵��Ѵ�.
 *		�ű� ����� ���� ������ drivers/include/nvm_ddi.h �� SYS_DB_T �� �����Ѵ�.
 *
 * Jackee's Note 2008/08/13: Valid Mark ���� ��� ����
 *		SYS_DB_VALID_MARK, ANA_DB_VALID_MARK, FACTORY_DB_VALID_MARK
 * �� ���� �����Ǵ� gSysNvmDB, gFactoryDB, gAnaNvmDB ���� �ʱ�ȭ ���� ���������
 * �ڵ� ���� ������� ����Ǿ���. ���� ������ ���� ������ ������ ����.
 * 		1. Element�� �߰� Ȥ�� ����.
 * 		2. ���� element�� �̸�, Type �Ǵ� ũ�� ����
 * 		3. ���� element�� ���� ��ġ ����.
 * 		4. Structue���� ����� user defined type�� ����
 * �̿ܿ��� Structe ���� ����� ������ ��ġ�� ��� �׸��� ��ȭ�� �����Ͽ�, valid
 * mark�� �ڵ����� ����Ǿ� nvm�� ����� ���� reset�ϰ� �˴ϴ�.
 * �ʱⰪ�� ���濡 ���� reset�� ���̻� ������� �ʽ��ϴ�.
 */

#define EDID_NVMDRV_PAGE_SIZE			8		/** for RGB/HDMI edid nvram(AT24C02BN) */
#define EDID_NVMDRV_TOTAL_SIZE			256		/** for RGB/HDMI edid nvram(AT24C02BN) */

/*-----------------------------------------------------------------------------
	�� ����
	(Type Definitions)
------------------------------------------------------------------------------*/
typedef struct
{
	char		*pName;
	UINT32		portNum;
	UINT32		slvAddr;
	UINT32		speed;
} I2C_DESC_T;

typedef struct
{
	I2C_DESC_T **ppI2cDev;
	char		*pName;
} I2C_DEV_LIST_TBL;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /*_CMNIO_TYPE_H_*/




