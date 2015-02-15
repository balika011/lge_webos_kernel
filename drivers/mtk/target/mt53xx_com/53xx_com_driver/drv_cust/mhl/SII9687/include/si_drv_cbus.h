/******************************************************************************/
//!file     si_drv_cbus.h
//!brief    CBUS Driver functions
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2011, Silicon Image, Inc.  All rights reserved.
/******************************************************************************/

#ifndef __SI_DRV_CBUS_H__
#define __SI_DRV_CBUS_H__


#if INC_CBUS == 1

#include "si_common.h"
#include "si_regs_mhl9687.h"
#include "si_drv_switch.h"

//------------------------------------------------------------------------------
//  CBUS Driver SiI9687-specific
//------------------------------------------------------------------------------

#define     MHL_SCRATCHPAD_SIZE             16
#define     MHL_MAX_BUFFER_SIZE             MHL_SCRATCHPAD_SIZE // manually define highest number
#define 	MHL_WRITE_BURST_LENGTH			MHL_SCRATCHPAD_SIZE

//
// structure to hold command details from upper layer to CBUS module
//
typedef struct
{
    uint8_t reqStatus;                      // CBUS_IDLE, CBUS_PENDING etc
    uint8_t command;                        // MHL command or offset
    uint8_t offsetData;                     // Offset of register on CBUS
    uint8_t length;                         // Write Burst length, only applicable to write burst. ignored otherwise.
    uint8_t msgData[MHL_MAX_BUFFER_SIZE];   // Pointer to message data area, used for sending RCP/RAP cmds and for return data
} cbus_req_t;

//------------------------------------------------------------------------------
// Driver enums
//------------------------------------------------------------------------------

typedef enum
{
    SiiCBUS_INT                     		= 0x0001,   // A CBUS interrupt has occurred
    SiiCBUS_NACK_RECEIVED_FM_PEER 			= 0x0002,	// Peer sent a NACK
    SiiCBUS_DCAP_RDY_RECEIVED_FM_PEER		= 0x0004,   // DCAP_RDY received
    SiiCBUS_PATH_EN_RECEIVED_FM_PEER		= 0x0008,   // PATH_EN received
    SiiCBUS_DCAP_CHG_RECEIVED_FM_PEER		= 0x0010,   // DCAP_CHG received
    SiiCBUS_SCRATCHPAD_WRITTEN_BY_PEER		= 0x0020,   // DSCR_CHG received.peer writes into scrAtchpad
    SiiCBUS_REQ_WRT_RECEIVED_FM_PEER		= 0x0040,   // REQ_WRT received
    SiiCBUS_GRT_WRT_RECEIVED_FM_PEER		= 0x0080,   // GRT_WRT received
    SiiCBUS_3D_REQ_RECEIVED_FM_PEER			= 0x0100,   // GRT_WRT received

    SiiCBUS_CEC_ABORT						= 0x0200,   // DDC_ABORT
    SiiCBUS_DDC_ABORT						= 0x0400,   // DDC_ABORT
    SiiCBUS_XFR_ABORT_R						= 0x0800,   // DDC_ABORT
    SiiCBUS_XFR_ABORT_T						= 0x1000,   // DDC_ABORT

    SiiCBUS_MSC_MSG_RCVD					= 0x2000,	// MSC_MSG received
    SiiCBUS_MSC_CMD_DONE					= 0x4000,	// MSC_MSG received
    SiiCBUS_CBUS_CONNECTION_CHG				= 0x8000,	// MSC_MSG received

} SiiDrvCbusStatus_t;

//------------------------------------------------------------------------------
// CBUS Driver Manifest Constants
//------------------------------------------------------------------------------

// Version that this chip supports
#define	MHL_VER_MAJOR		(0x02 << 4)	// bits 4..7
#define	MHL_VER_MINOR		0x00		// bits 0..3

//Device Category
#define	MHL_DEV_CATEGORY_DEV_TYPE			BIT0	// bits 3..0 Sink = 0x01
#define	MHL_DEV_CATEGORY_POW_BIT			BIT4
#define	MHL_DEV_CATEGORY_POW_PLIM0			BIT5	// bits{6,5} would be {0,1} = Device delivers at least 900mA
#define	MHL_DEV_CATEGORY_POW_PLIM1			BIT6	// bits{6,5} would be {0,1} = Device delivers at least 900mA

//Adopter ID
#define MHL_ADOPTER_SIMG_ID_H				0x01
#define MHL_ADOPTER_SIMG_ID_L				0x42

//Video Link Mode
#define	MHL_DEV_VID_LINK_SUPPRGB444			0x01
#define	MHL_DEV_VID_LINK_SUPPYCBCR444		0x02
#define	MHL_DEV_VID_LINK_SUPPYCBCR422		0x04
#define	MHL_DEV_VID_LINK_SUPP_PPIXEL		0x08
#define	MHL_DEV_VID_LINK_SUPP_ISLANDS		0x10
#define	MHL_DEV_VID_LINK_SUPP_VGA			0x20

//Audio Link Mode Support
#define	MHL_DEV_AUD_LINK_2CH				0x01
#define	MHL_DEV_AUD_LINK_8CH				0x02

//Feature Flag in the devcap
#define	MHL_FEATURE_RCP_SUPPORT				BIT0
#define	MHL_FEATURE_RAP_SUPPORT				BIT1
#define	MHL_FEATURE_SP_SUPPORT				BIT2
#define MHL_FEATURE_UCP_SEND_SUPPORT		BIT3
#define MHL_FEATURE_UCP_RECEIVE_SUPPORT		BIT4


// VIDEO TYPES
#define		MHL_VT_GRAPHICS					0x00
#define		MHL_VT_PHOTO					0x02
#define		MHL_VT_CINEMA					0x04
#define		MHL_VT_GAMES					0x08
#define		MHL_SUPP_VT						0x80

//Logical Dev Map
#define	MHL_DEV_LD_DISPLAY					(0x01 << 0)
#define	MHL_DEV_LD_VIDEO					(0x01 << 1)
#define	MHL_DEV_LD_AUDIO					(0x01 << 2)
#define	MHL_DEV_LD_MEDIA					(0x01 << 3)
#define	MHL_DEV_LD_TUNER					(0x01 << 4)
#define	MHL_DEV_LD_RECORD					(0x01 << 5)
#define	MHL_DEV_LD_SPEAKER					(0x01 << 6)
#define	MHL_DEV_LD_GUI						(0x01 << 7)

#define	MHL_INT_AND_STATUS_SIZE				0x33

#define	MHL_SCRATCHPAD_SIZE				16
#define	MHL_MAX_BUFFER_SIZE				MHL_SCRATCHPAD_SIZE	// manually define highest number

// initialize MHL registers with the correct values
#define MHL_DEV_STATE						0x00
#define MHL_VERSION							(MHL_VER_MAJOR | MHL_VER_MINOR)
#define MHL_DEV_CAT_POW                  	(MHL_DEV_CATEGORY_DEV_TYPE | MHL_DEV_CATEGORY_POW_BIT | MHL_DEV_CATEGORY_POW_PLIM0)
#define MHL_ADOPTER_ID_H					(MHL_ADOPTER_SIMG_ID_H)
#define MHL_ADOPTER_ID_L					(MHL_ADOPTER_SIMG_ID_L)
#define MHL_VID_LINK_MODE					(MHL_DEV_VID_LINK_SUPPRGB444 | MHL_DEV_VID_LINK_SUPPYCBCR444 | MHL_DEV_VID_LINK_SUPPYCBCR422 | MHL_DEV_VID_LINK_SUPP_PPIXEL | MHL_DEV_VID_LINK_SUPP_ISLANDS | MHL_DEV_VID_LINK_SUPP_VGA)
#define MHL_AUD_LINK_MODE					(MHL_DEV_AUD_LINK_2CH | MHL_DEV_AUD_LINK_8CH)
#define	MHL_LOGICAL_DEVICE_MAP				(MHL_DEV_LD_DISPLAY | MHL_DEV_LD_SPEAKER)
#define MHL_LINK_CLK_FREQUENCY     			0x0F    // Bits set dynamically
#define MHL_FEATURE_SUPPORT          		(MHL_FEATURE_RCP_SUPPORT | MHL_FEATURE_RAP_SUPPORT | MHL_FEATURE_SP_SUPPORT | MHL_FEATURE_UCP_SEND_SUPPORT | MHL_FEATURE_UCP_RECEIVE_SUPPORT)
#define MHL_INT_STAT_SIZE          			(MHL_INT_AND_STATUS_SIZE)    // Bits set dynamically


bool_t      SiiDrvCbusInstanceSet( uint_t instanceIndex );
int_t       SiiDrvCbusInstanceGet( void );
//------------------------------------------------------------------------------
// Function:    SiiDrvCbusStatus
// Description: Returns a status flag word containing CBUS driver-specific
//              information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the CBUS Driver
//------------------------------------------------------------------------------
uint16_t    SiiDrvCbusStatus(void);
bool_t      SiiDrvCbusInitialize(void);
void 		SiiDrvCbusConfigure ( uint8_t port );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusInterruptStatusGet
// Description: Returns the last Interrupt Status data retrieved by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     true if new interrupt status data is available, false if not.
//              pData - Destination for interrupt status data.
//------------------------------------------------------------------------------

void SiiDrvCbusInterruptStatusGet ( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusInterruptStatusSet
// Description: Clears the interrupt variable
//------------------------------------------------------------------------------
void SiiDrvCbusInterruptStatusSet (void);

uint8_t SiiDrvCbusCecLaGet( int_t channel );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusIntrFlagGet
// Description: Returns interrupt flag
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusIntrFlagGet(void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusIntrFlagSet
// Description: reset interrupt flags
//------------------------------------------------------------------------------
void  SiiDrvCbusIntrFlagSet(void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusNackFromPeerGet
// Description: checks if peer sent a NACK
// Parameters:  channel - CBus channel
// Returns:     true if peer sent a NACK, false if not.
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusNackFromPeerGet(void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusDevCapChangedGet
// Description: Returns if the peer's device capability values are changed
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusDevCapChangedGet(void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusScratchpadWrtnGet
// Description: Returns if the peer has written the scratchpad
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusScratchpadWrtnGet (void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusReqWrtGet
// Description: Returns if the peer is requesting for scratchpad write permission
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusReqWrtGet (void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusGrtWrtGet
// Description: Returns if the peer is requesting for scratchpad write permission
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusGrtWrtGet (void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusVsDataGet
// Description: Returns the last VS cmd and data bytes retrieved by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (2 bytes).
// Returns:     true if a new VS data was available, false if not.
//              pData[0] - VS_CMD value
//              pData[1] - VS_DATA value
//------------------------------------------------------------------------------
bool_t SiiDrvCbusVsDataGet( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusDevCapReadyGet
// Description: Returns if the peer's device capability values are ready
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusDevCapReadyGet (void);

//------------------------------------------------------------------------------
// Function:    SiiDrvPathEnableGet
// Description: Returns if the peer has sent PATH_EN
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvPathEnableGet (void);

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusCmdRetDataGet
// Description: Clears the register to receive fresh response data back
// Parameters:  pData - pointer to return data buffer (2 bytes).
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusCmdRetDataGet( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusBusStatusGet
// Description: Returns the last Bus Status data retrieved by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     true if new bus status data is available, false if not.
//              pData - Destination for bus status data.
//------------------------------------------------------------------------------
bool_t SiiDrvCbusBusStatusGet( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusCecAbortReasonGet
// Description: Returns the last DDC Abort reason received by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     pData - Destination for DDC Abort reason data.
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusCecAbortReasonGet ( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusDdcAbortReasonGet
// Description: Returns the last DDC Abort reason received by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     true if a new DDC Abort reason data was available, false if not.
//              pData - Destination for DDC Abort reason data.
//------------------------------------------------------------------------------
bool_t        SiiDrvCbusDdcAbortReasonGet( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusMscAbortTransReasonGet
// Description: Returns the last MSC Abort reason received by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     pData - Destination for MSC Abort reason data.
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusMscAbortTransReasonGet ( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusMscAbortRcvrReasonGet
// Description: Returns the last MSC Abort reason received by the CBUS ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     pData - Destination for MSC Abort reason data.
//------------------------------------------------------------------------------
bool_t  SiiDrvCbusMscAbortRcvrReasonGet ( uint8_t *pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusPortSelectBitsGet
// Description: Reads the MHL selected port(s) bit-field.  This is the value
//              downloaded from NVRAM at boot-up time.
// Parameters:  None
// Returns:     MHL selected port(s) bit-field.
//
//------------------------------------------------------------------------------

uint8_t     SiiDrvCbusPortSelectBitsGet( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvInternalCBusWriteCommand
// Description: Write the specified Sideband Channel command to the CBUS.
//              Command can be a MSC_MSG command (RCP/MCW/RAP), or another command
//              such as READ_DEVCAP, GET_VENDOR_ID, SET_HPD, CLR_HPD, etc.
//
// Parameters:  channel - CBUS channel to write
//              pReq    - Pointer to a cbus_req_t structure containing the
//                        command to write
// Returns:     TRUE    - successful write
//              FALSE   - write failed
//------------------------------------------------------------------------------

bool_t      SiiDrvInternalCBusWriteCommand( cbus_req_t *pReq );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusInitialize
// Description: Attempts to initialize the CBUS. If register reads return 0xFF,
//              it declares error in initialization.
//              Initializes discovery enabling registers and anything needed in
//              config register, interrupt masks.
// Returns:     TRUE if no problem
//------------------------------------------------------------------------------

bool_t SiiDrvCbusInitialize( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusPortSelectBitsGet
// Description: Reads the MHL selected port(s) bit-field.  This is the value
//              downloaded from NVRAM at boot-up time.
// Parameters:  None
// Returns:     MHL selected port(s) bit-field.
//
//------------------------------------------------------------------------------

uint8_t     SiiDrvCbusPortSelectBitsGet( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusDevCapsRegisterGet
// Description: Returns the CBus register value
// Parameters:  regAddr - register address
// Returns:     register value
//------------------------------------------------------------------------------
uint8_t     SiiDrvCbusDevCapsRegisterGet( uint16_t regAddr );
void        CbusDrvChannelProcessInterrupts( void );

void        SiiDrvCbusProcessInterrupts(void);
void 		SiiDrvCbusChnEn ( bool_t isEnable, uint8_t port );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusWriteLocalScratchpad
// Description: Returns if the peer is requesting for scratchpad write permission
// Returns:     true/false
//------------------------------------------------------------------------------
void  SiiDrvCbusWriteLocalScratchpad ( uint8_t startOffset, uint8_t length, uint8_t* pData );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusRegisterGet
// Description: Returns the CBus register value
// Parameters:  regAddr - register address, channel
// Returns:     register value
//------------------------------------------------------------------------------

uint8_t  SiiDrvCbusRegisterGet ( uint16_t regAddr );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusRegisterSet
// Description: Sets the CBus register value
// Parameters:  regAddr - register address
//------------------------------------------------------------------------------
void  SiiDrvCbusRegisterSet ( uint8_t regAddr , uint8_t value );

//------------------------------------------------------------------------------
// Function:    SiiDrvCbus3DReqGet
// Description: Returns if the peer is requesting for 3D information
// Returns:     true/false
//------------------------------------------------------------------------------
bool_t  SiiDrvCbus3DReqGet (void);

#endif
#endif      // __SI_DRV_CBUS_H__
