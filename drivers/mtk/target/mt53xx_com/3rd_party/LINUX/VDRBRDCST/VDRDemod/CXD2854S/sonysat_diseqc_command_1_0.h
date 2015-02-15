/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_diseqc_command_1_0.h

          This file provides functions for generating "DiSEqC1.0" commands.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DISEQC_COMMAND_1_0_H
#define SONYSAT_DISEQC_COMMAND_1_0_H

#include "sonysat_common.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create DiSEqC command to reset DiSEqC microcontroller.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Reset (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to switch peripheral power supply off.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Standby (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to switch peripheral power supply on.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_PowerOn (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select the Low Local Oscillator frequency.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetLo (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command select Vertical Polarisation (or Right circular).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetVR (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command select Satellite position A (or position C).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetPosA (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select Switch Option A (e.g. positions A/B).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetSOA (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select the High Local Oscillator frequency.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetHi (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select Horizontal Polarisation (or Left circular).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetHL (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select Satellite position B (or position D).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetPosB (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to select Switch Option B (e.g. positions C/D).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetSOB (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to write to Port group 0 (Committed switches).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param data Data to send.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_WriteN0 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data);

#endif /* SONYSAT_DISEQC_COMMAND_1_0_H */
