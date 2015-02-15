/*************************************************************************************
                  Silicon Laboratories Broadcast TDSY_G230D Layer 1 API
   API types used by commands and properties
   FILE: TDSY_G230D_typedefs.h
   Supported IC : TDSY_G230D
   Compiled for ROM 33 firmware 2_0_build_4
   Revision: 0.1
   Date: July 30 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   TDSY_G230D_TYPEDEFS_H
#define   TDSY_G230D_TYPEDEFS_H

#define MAX_LENGTH             80

typedef struct L1_TDSY_G230D_Context {
  L0_Context                 *i2c;
  L0_Context                  i2cObj;
  TDSY_G230D_CmdObj              *cmd;
  TDSY_G230D_CmdObj               cmdObj;
  TDSY_G230D_CmdReplyObj         *rsp;
  TDSY_G230D_CmdReplyObj          rspObj;
  TDSY_G230D_PropObj             *prop;
  TDSY_G230D_PropObj              propObj;
  TDSY_G230D_COMMON_REPLY_struct *status;
  TDSY_G230D_COMMON_REPLY_struct  statusObj;
  /*chip rev constants for integrity checking */
  unsigned char chiprev;
  unsigned char part;
  /* Last 2 digits of part number */
  unsigned char partMajorVersion;
  unsigned char partMinorVersion;
  unsigned char partRomid;
} L1_TDSY_G230D_Context;

/* _additional_definitions_start */
 #ifndef __VID_FILT_STRUCT__
#define __VID_FILT_STRUCT__
/* struct to read video filter file with varying line lengths */
typedef struct  {
  char vid_filt_len;
  unsigned char vid_filt_table [16];
} vid_filt_struct;
#endif /* __VID_FILT_STRUCT__ */
/* _additional_definitions_point */
#ifndef __FIRMWARE_STRUCT__
#define __FIRMWARE_STRUCT__
typedef struct  {
	unsigned char firmware_len;
	unsigned char firmware_table[16];
} firmware_struct;
#endif /* __FIRMWARE_STRUCT__ */

#endif /* TDSY_G230D_TYPEDEFS_H */




