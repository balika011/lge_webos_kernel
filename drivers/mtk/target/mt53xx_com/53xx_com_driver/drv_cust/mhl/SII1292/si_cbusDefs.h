//***************************************************************************

//!file     si_cbusDefs.h
//!brief    CBUS API Definitions
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_CBUS_DEFS_H__
#define __SI_CBUS_DEFS_H__

#define CBUS_VER_MAJOR (0x01 << 4) // bits 4..7
#define CBUS_VER_MINOR 0x00        // bits 0..3
#define MHL_CBUS_VERSION (CBUS_VER_MAJOR | CBUS_VER_MINOR)

#define MHL_POWER_SUPPLY_CAPACITY 16      // 160 mA current
#define MHL_POWER_SUPPLY_PROVIDED 16      // 160mA 0r 0 for Wolverine.


#define MHL_DEVCAP_SIZE 16

#define		MHL_MAX_BUFFER_SIZE				SCRATCHPAD_SIZE	// manually define highest number

#define MHL_REG_CHANGE_INT_OFFSET 0x20

#define MHL_DEVICE_STATUS_CONNECTED_RDY_REG_OFFSET 0x30
#define DCAP_RDY 0x01
#define DSCR_RDY 0x02

#define MHL_DEVICE_STATUS_LINK_MODE_REG_OFFSET 0x31
#define MUTED 0x10
#define PATH_EN 0x08
#define CLK_MODE 0x07
#if 1//Keno20120820-5
#define		DCHANGE_INT										0x21
#define		EDID_CHG												0x02
#endif

#define MHL_DEV_CAP_DEV_STATE_REG_OFFSET 0x00

#define MHL_DEV_CAP_MHL_VERSION_REG_OFFSET 0x01

#define     MHL_DEV_CAP_DEV_CAT_REG_OFFSET 0x02
#define     NOT_ALLOWED 0x00
#define     SINK_DEV 0x01
#define     SOURCE_DEV 0x02
#define     DONGLE_DEV 0x03
#define     POW_YES 1
#define     POW_NO 0
#define     PLIM_500mA 0x00
#define     PLIM_900mA 0x20
#define     PLIM_1500mA 0x40
#define     PLIM_Reserved 0x60

#define     MHL_DEV_CAP_ADOPTER_IDH_REG_OFFSET 0x03

#define     MHL_DEV_CAP_ADOPTER_IDL_REG_OFFSET 0x04

#define     MHL_DEV_CAP_VID_LINK_MODE_REG_OFFSET 0x05

#define     MHL_DEV_CAP_AUD_LINK_MODE_REG_OFFSET 0x06

#define     MHL_DEV_CAP_VIDEO_TYPE_REG_OFFSET 0x07
#define     VT_GRAPHICS (0x01 << 0)
#define     VT_PHOTO (0x01 << 1)
#define     VT_CINEMA (0x01 << 2)
#define     VT_GAME (0x01 << 3)
#define     SUPP_VT (0x01 << 7)

#define     MHL_DEV_CAP_LOG_DEV_MAP_REG_OFFSET 0x08

#define     MHL_DEV_CAP_BANDWIDTH_REG_OFFSET 0x09

#define     MHL_DEV_CAP_FEATURE_FLAG_REG_OFFSET 0x0A
#define     RCP_SUPPORT (0x01 << 0)
#define     RAP_SUPPORT (0x01 << 1)

#define     MHL_DEV_CAP_DEVICE_IDH_REG_OFFSET 0x0B
#define     MHL_DEV_CAP_DEVICE_IDL_REG_OFFSET 0x0C
#define     MHL_DEV_CAP_Scratchpad_Size_REG_OFFSET 0x0D
#define     MHL_DEV_CAP_IntStat_Size_REG_OFFSET 0x0E

//------------------------------------------------------------------------------
//
// MHL Specs defined registers in device capability set
//
//
typedef struct
{
    unsigned char mhl_devcap_version;                 // 0x00
    unsigned char mhl_devcap_cbus_version;            // 0x01
    unsigned char mhl_devcap_device_category;         // 0x02
    unsigned char mhl_devcap_power_supply_capacity;   // 0x03
    unsigned char mhl_devcap_power_supply_provided;   // 0x04
    unsigned char mhl_devcap_video_link_mode_support; // 0x05
    unsigned char mhl_devcap_audio_link_mode_support; // 0x06
    unsigned char mhl_devcap_hdcp_status;             // 0x07
    unsigned char mhl_devcap_logical_device_map;      // 0x08
    unsigned char mhl_devcap_link_bandwidth_limit;    // 0x09
    unsigned char mhl_devcap_reserved_1;              // 0x0a
    unsigned char mhl_devcap_reserved_2;              // 0x0b
    unsigned char mhl_devcap_reserved_3;              // 0x0c
    unsigned char mhl_devcap_scratchpad_size;         // 0x0d
    unsigned char mhl_devcap_interrupt_size;          // 0x0e
    unsigned char mhl_devcap_devcap_size;             // 0x0f
} mhl_devcap_t;

//------------------------------------------------------------------------------
//
// MHL Specs defined registers for interrupts
//
//
typedef struct
{
    unsigned char mhl_intr_0;     // 0x00
    unsigned char mhl_intr_1;     // 0x01
    unsigned char mhl_intr_2;     // 0x02
    unsigned char mhl_intr_3;     // 0x03
} mhl_interrupt_t;

//------------------------------------------------------------------------------
//
// MHL Specs defined registers for status
//
//
typedef struct
{
    unsigned char mhl_status_0;   // 0x00
    unsigned char mhl_status_1;   // 0x01
    unsigned char mhl_status_2;   // 0x02
    unsigned char mhl_status_3;   // 0x03
} mhl_status_t;

//------------------------------------------------------------------------------
//
// MHL Specs defined registers for local scratchpad registers
//
//
typedef struct
{
    unsigned char mhl_scratchpad[16];
} mhl_scratchpad_t;

//------------------------------------------------------------------------------

//------- END OF DEFINES -------------
#endif  // __SI_CBUS_DEFS_H__
