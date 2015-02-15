/*************************************************************************************
                  Silicon Laboratories Broadcast TDSY_G230D Layer 1 API
   API functions prototypes used by commands and properties
   FILE: TDSY_G230D_L1_API.h
   Supported IC : TDSY_G230D
   Compiled for ROM 32 firmware 0_E_build_11
   Revision: 0.1
   Date: July 02 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    _TDSY_G230D_L1_API_H_
#define    _TDSY_G230D_L1_API_H_
#include "x_printf.h"
#include "Silabs_L0_TV_Chassis.h"

/* _demod_defines_insertion_start */
/* _demod_defines_insertion_point */

#include "TDSY_G230D_Commands.h"
#include "TDSY_G230D_Properties.h"
#include "TDSY_G230D_typedefs.h"
#include "TDSY_G230D_Commands_Prototypes.h"


#define NO_TDSY_G230D_ERROR                     0x00
#define ERROR_TDSY_G230D_PARAMETER_OUT_OF_RANGE 0x01
#define ERROR_TDSY_G230D_ALLOCATING_CONTEXT     0x02
#define ERROR_TDSY_G230D_SENDING_COMMAND        0x03
#define ERROR_TDSY_G230D_CTS_TIMEOUT            0x04
#define ERROR_TDSY_G230D_ERR                    0x05
#define ERROR_TDSY_G230D_POLLING_CTS            0x06
#define ERROR_TDSY_G230D_POLLING_RESPONSE       0x07
#define ERROR_TDSY_G230D_LOADING_FIRMWARE       0x08
#define ERROR_TDSY_G230D_LOADING_BOOTBLOCK      0x09
#define ERROR_TDSY_G230D_STARTING_FIRMWARE      0x0a
#define ERROR_TDSY_G230D_SW_RESET               0x0b
#define ERROR_TDSY_G230D_INCOMPATIBLE_PART      0x0c
/* _specific_error_value_insertion_start */
#define ERROR_TDSY_G230D_TUNINT_TIMEOUT         0x0d
#define ERROR_TDSY_G230D_xTVINT_TIMEOUT         0x0e
#define ERROR_TDSY_G230D_CRC_CHECK_ERROR        0x0f
/* _specific_error_value_insertion_point */
//#define SiTRACE mcPRN_DBG_MSG1
//#define mcPRN_DBG_MSG1(fmt...)          DBG_PRTINF(1, fmt)
#define SiTRACE printf


/* define PART_INTEGRITY_CHECKS (here) and the expected part number, chip rev, pmajor, pminor, and romid (in TDSY_G230D_L1_API.c) to use integrity checking */
#undef PART_INTEGRITY_CHECKS
/* chip rev constants for integrity checking */
extern const char TDSY_G230D_chiprev;
extern const char TDSY_G230D_part;
extern const char TDSY_G230D_partMajorVersion;
extern const char TDSY_G230D_partMinorVersion;
extern const char TDSY_G230D_partRomid;

unsigned char TDSY_G230D_L1_API_Init    (L1_TDSY_G230D_Context *api, int add);
unsigned char TDSY_G230D_L1_API_Patch   (L1_TDSY_G230D_Context *api, int iNbBytes, unsigned char *pucDataBuffer);
unsigned char TDSY_G230D_L1_CheckStatus (L1_TDSY_G230D_Context *api);
char*         TDSY_G230D_L1_API_ERROR_TEXT(unsigned char  error_code);
char*         TDSY_G230D_L1_API_TAG_TEXT  (void);

#define TDSY_G230D_TAG TDSY_G230D_L1_API_TAG_TEXT


#ifdef    TDSY_G230D_GET_PROPERTY_STRING
void TDSY_G230D_L1_FillPropertyStringText(L1_TDSY_G230D_Context *api, unsigned int prop_code, const char *separator, char *msg);
#endif /* TDSY_G230D_GET_PROPERTY_STRING */

#endif /* _TDSY_G230D_L1_API_H_ */






