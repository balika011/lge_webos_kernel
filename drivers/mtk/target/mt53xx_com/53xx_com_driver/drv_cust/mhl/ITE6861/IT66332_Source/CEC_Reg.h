///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CEC_Reg.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _CEC_REG_H_
#define _CEC_REG_H_

//Register Map
#define	REG_VENID_L			0x00
#define	REG_VENID_H			0x01
#define	REG_DEVICEID_L			0x02
#define	REG_DEVICEID_H		0x03
#define	REG_MREVISIONID		0x04
#define	REG_SRVISIONID			0x05
//CEC Initiator Block
#define	REG07					0x07
#define	REG08					0x08
	#define	B_FIRE_FRAME			0x80
	#define	B_DBGCEC_CLR			0x40
	#define	B_CEC_OE				0x20
	#define	B_CEC_FORCE			0x10
	#define	B_CEC_SMT				0x08
	#define	B_CEC_RST				0x04
	#define	B_REFCLK_RST			0x02
	#define	B_INT_CLR				0x01

#define	REG09					0x09
	#define	B_DATAIT_SEL			0x80
	#define	B_REGION_SEL			0x40
	#define	B_RXSELF_SEL			0x20
	#define	B_REFIRE				0x10
	#define	B_ACKTRIG_SEL			0x08
	#define	B_PULSE_SEL			0x04
	#define	B_NACK_EN				0x02
	#define	B_EN100ms_CNT			0x01

#define	REG0A					0x0A
#define	REG_DATA_MIN			0x0B
#define	REG_TIMER_UNIT		0x0C
#define	REG0D					0x0D
#define	REG_TX_HEADER			0x10
#define	REG_TX_OPCODE			0x11
#define	REG_TX_OPERAND1		0x12
#define	REG_TX_OPERAND2		0x13
#define	REG_TX_OPERAND3		0x14
#define	REG_TX_OPERAND4		0x15
#define	REG_TX_OPERAND5		0x16
#define	REG_TX_OPERAND6		0x17
#define	REG_TX_OPERAND7		0x18
#define	REG_TX_OPERAND8		0x19
#define	REG_TX_OPERAND9		0x1A
#define	REG_TX_OPERAND10		0x1B
#define	REG_TX_OPERAND11		0x1C
#define	REG_TX_OPERAND12		0x1D
#define	REG_TX_OPERAND13		0x1E
#define	REG_TX_OPERAND14		0x1F
#define	REG_TX_OPERAND15		0x20
#define	REG_TX_OPERAND16		0x21
#define	REG_CEC_TARGET_ADDR 	0x22
#define	REG_CEC_OUT_NUM		0x23
//AFE BLOCK
#define	REG24					0x24
#define	REG_HEAC_CTRL			0x25
	#define	B_SPDIFO_EN			0x08
	#define	B_HEC_EN				0x04
	#define	B_ARC_RX_EN			0x02
	#define	B_ARC_TX_EN			0x01
#define	REG26					0x26
#define	REG27					0x27
#define	REG28					0x28
#define	REG29					0x29
#define	REG2A					0x2A
#define	REG2B					0x2B
#define	REG2C					0x2C
#define	REG2D					0x2D
//CEC FOLLOWER BLOCK
#define	REG_RX_HEADER			0x30
#define	REG_RX_OPCODE			0x31
#define	REG_RX_OPERAND1		0x32
#define	REG_RX_OPERAND2		0x33
#define	REG_RX_OPERAND3		0x34
#define	REG_RX_OPERAND4		0x35
#define	REG_RX_OPERAND5		0x36
#define	REG_RX_OPERAND6		0x37
#define	REG_RX_OPERAND7		0x38
#define	REG_RX_OPERAND8		0x39
#define	REG_RX_OPERAND9		0x3A
#define	REG_RX_OPERAND10		0x3B
#define	REG_RX_OPERAND11		0x3C
#define	REG_RX_OPERAND12		0x3D
#define	REG_RX_OPERAND13		0x3E
#define	REG_RX_OPERAND14		0x3F
#define	REG_RX_OPERAND15		0x40
#define	REG_RX_OPERAND16		0x41
#define	REG_FOLLOWER_RX_NUM	0x42
#define	REG_INITIATOR_TX_NUM	0x43
#define	REG_SYS_STATUS		0x44
	#define	B_READY_FIRE			0x40
	#define	B_ERROR_STATUS		0x30
	#define	B_OUT_STATUS			0x0C
		#define	B_ReceiverACK			0x00
		#define	B_ReceiverNACK			0x04
		#define	B_ReceiverRetry			0x08
		#define	B_ReceiverFail			0x0C
	#define	B_BUS_STATUS			0x02

#define	REG_MSCOUNT_L			0x45
#define	REG_MSCOUNT_M		0x46
#define	REG_MSCOUNT_H			0x47

#define	REG_DBG_STATE			0x48
#define	REG_DBG_BLOCK			0x49
#define	REG_DBG_BIT			0x4A
#define	REG_DBG_TIMING		0x4B

#define	REG_INT_STATUS		0x4C
//	#define	B_CECFAIL_INT			0x20
	#define B_TXFAIL_INT			0x20
	#define	B_RXDONE_INT			0x10
	#define	B_TXDONE_INT			0x08
	#define	B_RXFAIL_INT			0x04
	#define	B_RX_INT				0x02
	#define	B_TX_INT				0x01
#endif
