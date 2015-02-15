/***************************************************************************************
                  Silicon Laboratories Broadcast TDSY_G230D  API
   FILE: TDSY_G230D_firmware_2_0_build_x.h
   Supported IC : TDSY_G230D
   Date: July 26 2012
   (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.

   This is a dummy firmware patch for TDSY_G230D-A20 devices.
   When a new patch becomes available,  replace this file and all calls to it with the new patch.
   This patch is formatted in the new 16 byte download format.  The future patches will match this structure.
****************************************************************************************/
#ifndef _TDSY_G230D_FIRMWARE_2_0_BUILD_X_H_
#define _TDSY_G230D_FIRMWARE_2_0_BUILD_X_H_

#ifndef __FIRMWARE_STRUCT__
#define __FIRMWARE_STRUCT__
typedef struct firmware_struct {
	unsigned char firmware_len;
	unsigned char firmware_table[16];
} firmware_struct;
#endif /* __FIRMWARE_STRUCT__ */

firmware_struct TDSY_G230D_FW_2_0bx[] = {
{ 0 , { 0x00} }
};

#define FIRMWARE_LINES_2_0bx (sizeof(TDSY_G230D_FW_2_0bx)/(sizeof(firmware_struct)))

#endif /* _TDSY_G230D_FIRMWARE_2_0_BUILD_X_H_ */
