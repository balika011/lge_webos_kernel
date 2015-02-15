/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_diseqc_command_1_1.h

          This file provides functions for generating "DiSEqC1.1" commands.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DISEQC_COMMAND_1_1_H
#define SONYSAT_DISEQC_COMMAND_1_1_H

#include "sonysat_common.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create DiSEqC command to select switch S1 input A (deselect input B).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS1A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S2 input A (deselect input B).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS2A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S3 input A (deselect input B).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS3A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S4 input A (deselect input B).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS4A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S1 input B (deselect input A).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS1B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S2 input B (deselect input A).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS2B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S3 input B (deselect input A).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS3B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select switch S4 input B (deselect input A).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetS4B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to write to Port group 1 (Uncommitted switches).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param data Data to send.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_WriteN1 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data);

/**
 @brief Create DiSEqC command to write channel frequency (BCD string).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param frequency Frequency in KHz.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_WriteFreq (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address,
                                             uint32_t frequency);

#endif /* SONYSAT_DISEQC_COMMAND_1_1_H */
