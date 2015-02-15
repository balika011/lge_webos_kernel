/*************************************************************************************
                  Silicon Laboratories Broadcast Si2158 Layer 1 API
   API types used by commands and properties
   FILE: Si2158_typedefs.h
   Supported IC : Si2158
   Compiled for ROM 33 firmware 2_0_build_4
   Revision: 0.1
   Date: July 30 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   Si2158_TYPEDEFS_H
#define   Si2158_TYPEDEFS_H

#define MAX_LENGTH             80

typedef struct L1_Si2158_Context {
  L0_Context                 *i2c;
  L0_Context                  i2cObj;
  Si2158_CmdObj              *cmd;
  Si2158_CmdObj               cmdObj;
  Si2158_CmdReplyObj         *rsp;
  Si2158_CmdReplyObj          rspObj;
  Si2158_PropObj             *prop;
  Si2158_PropObj              propObj;
  Si2158_COMMON_REPLY_struct *status;
  Si2158_COMMON_REPLY_struct  statusObj;
  /*chip rev constants for integrity checking */
  unsigned char chiprev;
  unsigned char part;
  /* Last 2 digits of part number */
  unsigned char partMajorVersion;
  unsigned char partMinorVersion;
  unsigned char partRomid;
} L1_Si2158_Context;

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

#endif /* Si2158_TYPEDEFS_H */




