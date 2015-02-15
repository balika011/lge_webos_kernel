/*************************************************************************************
                  Silicon Laboratories Broadcast Si2158 Layer 1 API
   API functions prototypes used by commands and properties
   FILE: Si2158_L1_API.h
   Supported IC : Si2158
   Compiled for ROM 32 firmware 0_E_build_11
   Revision: 0.1
   Date: July 02 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    _Si2158_L1_API_H_
#define    _Si2158_L1_API_H_
#include "x_printf.h"
#include "Silabs_L0_TV_Chassis.h"

/* _demod_defines_insertion_start */
/* _demod_defines_insertion_point */

#include "Si2158_Commands.h"
#include "Si2158_Properties.h"
#include "Si2158_typedefs.h"
#include "Si2158_Commands_Prototypes.h"


#define NO_Si2158_ERROR                     0x00
#define ERROR_Si2158_PARAMETER_OUT_OF_RANGE 0x01
#define ERROR_Si2158_ALLOCATING_CONTEXT     0x02
#define ERROR_Si2158_SENDING_COMMAND        0x03
#define ERROR_Si2158_CTS_TIMEOUT            0x04
#define ERROR_Si2158_ERR                    0x05
#define ERROR_Si2158_POLLING_CTS            0x06
#define ERROR_Si2158_POLLING_RESPONSE       0x07
#define ERROR_Si2158_LOADING_FIRMWARE       0x08
#define ERROR_Si2158_LOADING_BOOTBLOCK      0x09
#define ERROR_Si2158_STARTING_FIRMWARE      0x0a
#define ERROR_Si2158_SW_RESET               0x0b
#define ERROR_Si2158_INCOMPATIBLE_PART      0x0c
/* _specific_error_value_insertion_start */
#define ERROR_Si2158_TUNINT_TIMEOUT         0x0d
#define ERROR_Si2158_xTVINT_TIMEOUT         0x0e
/* _specific_error_value_insertion_point */
//#define SiTRACE mcPRN_DBG_MSG1
//#define mcPRN_DBG_MSG1(fmt...)          DBG_PRTINF(1, fmt)
#define SiTRACE printf


/* define PART_INTEGRITY_CHECKS (here) and the expected part number, chip rev, pmajor, pminor, and romid (in Si2158_L1_API.c) to use integrity checking */
#undef PART_INTEGRITY_CHECKS
/* chip rev constants for integrity checking */
extern const char Si2158_chiprev;
extern const char Si2158_part;
extern const char Si2158_partMajorVersion;
extern const char Si2158_partMinorVersion;
extern const char Si2158_partRomid;

unsigned char Si2158_L1_API_Init    (L1_Si2158_Context *api, int add);
unsigned char Si2158_L1_API_Patch   (L1_Si2158_Context *api, int iNbBytes, unsigned char *pucDataBuffer);
unsigned char Si2158_L1_CheckStatus (L1_Si2158_Context *api);
char*         Si2158_L1_API_ERROR_TEXT(unsigned char  error_code);
char*         Si2158_L1_API_TAG_TEXT  (void);

#define Si2158_TAG Si2158_L1_API_TAG_TEXT


#ifdef    Si2158_GET_PROPERTY_STRING
void Si2158_L1_FillPropertyStringText(L1_Si2158_Context *api, unsigned int prop_code, char *separator, char *msg);
#endif /* Si2158_GET_PROPERTY_STRING */

#endif /* _Si2158_L1_API_H_ */






