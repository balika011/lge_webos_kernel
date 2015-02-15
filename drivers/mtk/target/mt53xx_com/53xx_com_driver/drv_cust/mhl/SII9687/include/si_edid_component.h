//***************************************************************************
//!file     si_edid_component.h
//!brief    Silicon Image EDID Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_EDID_COMPONENT_H__
#define __SI_EDID_COMPONENT_H__

#include "si_common.h"
#include "si_device_config.h"
#include "si_drv_edid.h"
#include "si_cbus_config.h"
#include "si_c99support.h"

//------------------------------------------------------------------------------
//  EDID Component Manifest Constants
//------------------------------------------------------------------------------

#define EDID_TABLE_LEN              256
#define EDID_BLOCK_LEN              128
#define EDID_VGA_TABLE_LEN          128
#define EDID_DEVBOOT_LEN            64
#define EDID_MHL_3D_SUPPORT			ENABLE
#define EDID_MHL_WRITE_BURST_LENGTH	16
#define EDID_3D_VIC_TOT_ENT			5		// Total # of WRITE BURSTS needed for sending VIC blocks
											// 1 WRITE BURST can carry 10 data bytes
#define EDID_3D_VIC_TABLE_LEN       (EDID_MHL_WRITE_BURST_LENGTH * EDID_3D_VIC_TOT_ENT)
#define EDID_3D_DTD_TOT_ENT			2		// Total # of WRITE BURSTS needed for sending DTD blocks
											// 1 WRITE BURST can carry 10 data bytes
#define EDID_3D_DTD_TABLE_LEN       (EDID_MHL_WRITE_BURST_LENGTH * EDID_3D_DTD_TOT_ENT)


//------------------------------------------------------------------------------
//  EDID Component Manifest Constants
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t     nvm_config;
    uint8_t     edid_valid;
    uint8_t     nvm_version;
    uint8_t     edid_copy_dest;
    uint8_t     hpd_hw_ctrl;
    uint8_t     ddc_filter_sel;
    uint8_t     wakeup_source;
    uint8_t     edid_vga_copy_dest;
    uint8_t     spare1[2];
    uint8_t     cec_pa_addr;
    uint8_t     spare2[1];
    uint8_t     cec_pad_h_ch0;
    uint8_t     cec_pad_l_ch0;
    uint8_t     cec_pad_h_ch1;
    uint8_t     cec_pad_l_ch1;
    uint8_t     cec_pad_h_ch2;
    uint8_t     cec_pad_l_ch2;
    uint8_t     cec_pad_h_ch3;
    uint8_t     cec_pad_l_ch3;
    uint8_t     spare3[2];
    uint8_t     cec_pad_h_ch4;
    uint8_t     cec_pad_l_ch4;
    uint8_t     spare4[4];
    uint8_t     checksum_ch0;
    uint8_t     checksum_ch1;
    uint8_t     checksum_ch2;
    uint8_t     checksum_ch3;
    uint8_t     spare5[1];
    uint8_t     checksum_ch4;
    uint8_t     spare6[2];
    uint8_t     mhdPortSelect;
    uint8_t     mhdVersionSupported;
    uint8_t     miscControl0;
    uint8_t     spare7[1];
    uint8_t     miscControl1;
    uint8_t     spare8[24];

} SiiDeviceBootData_t;

//------------------------------------------------------------------------------
//  EDID Component Instance Data
//------------------------------------------------------------------------------

typedef struct
{
	int			structVersion;
	int			lastResultCode;			// Contains the result of the last API function called
	uint16_t	statusFlags;

}	SiiEdidInstanceData_t;

typedef struct _SiiEdidInfo_t
{
	uint8_t paL;			//Physical address
	uint8_t paH;			//Physical address
	uint8_t paOffset;       //Points to where the PA resides.
	uint8_t cs;			    //CheckSum
	uint8_t csOffset;       //Points to where the CS resides.

} SiiEdidInfo_t;


extern SiiEdidInstanceData_t edidInstance[SII_NUM_EDID];
extern SiiEdidInstanceData_t *pEdid;

//------------------------------------------------------------------------------
//  Standard component functions
//------------------------------------------------------------------------------

bool_t SiiEdidConfigure ( void );
bool_t SiiEdidInitialize ( void );
bool_t SiiEdidStatus ( void );
bool_t SiiEdidStandby ( void );
bool_t SiiEdidResume ( void );
bool_t SiiEdidGetLastResult ( void );

//------------------------------------------------------------------------------
//  Component Specific functions
//------------------------------------------------------------------------------

bool_t  SiiEdidRead( uint8_t source, uint8_t *pDest);
bool_t  SiiEdidWrite( uint8_t target, const uint8_t *pSource );
bool_t  SiiEdidLoadPortRam( uint8_t source, uint8_t port );
uint8_t SiiEdidUpdateHdmiData ( uint8_t *pData, uint8_t port );
bool_t  SiiEdidValid( const uint8_t *pData, bool_t isHdmi );
bool_t  SiiEdidGetData ( uint8_t *pData, SiiEdidInfo_t *editInfo);
void    SiiEdidBootNvramReplace ( uint8_t *pSrc, uint8_t *pDst, SiiEdidInfo_t *editInfo);




#endif // __SI_EDID_COMPONENT_H__
