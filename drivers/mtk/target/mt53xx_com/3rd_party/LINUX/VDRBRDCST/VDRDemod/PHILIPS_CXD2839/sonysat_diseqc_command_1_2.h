/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_diseqc_command_1_2.h

          This file provides functions for generating "DiSEqC1.2" commands.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DISEQC_COMMAND_1_2_H
#define SONYSAT_DISEQC_COMMAND_1_2_H

#include "sonysat_common.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command.h"

/**
 @brief Mode for motor control command.

 This is used by sonysat_diseqc_command_DriveEast() and sonysat_diseqc_command_DriveWest().
*/
typedef enum {
    SONYSAT_DISEQC_MOTOR_MODE_STEPPED, /**< amount is "step size". */
    SONYSAT_DISEQC_MOTOR_MODE_TIMED,   /**< amount is "timeout time" in ms. */
    SONYSAT_DISEQC_MOTOR_MODE_NONSTOP  /**< amount is ignored. */
} sonysat_diseqc_motor_mode_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create DiSEqC command to write Analogue value A0 (e.g. Skew).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param data Data to send.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_WriteA0 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data);

/**
 @brief Create DiSEqC command to write Analogue value A1.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param data Data to send.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_WriteA1 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data);

/**
 @brief Create DiSEqC command to stop Positioner movement.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Halt (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to disable Limits.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LimitsOff (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to set East Limit (& Enable recommended).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LimitE (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to set West Limit (& Enable recommended).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LimitW (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to drive Motor East (with optional timeout/steps).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param mode Motor mode.
 @param amount The amount of motor moving.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_DriveEast (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address,
                                             sonysat_diseqc_motor_mode_t mode,
                                             uint8_t amount);

/**
 @brief Create DiSEqC command to drive Motor West (with optional timeout/steps).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param mode Motor mode.
 @param amount The amount of motor moving.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_DriveWest (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address,
                                             sonysat_diseqc_motor_mode_t mode,
                                             uint8_t amount);

/**
 @brief Create DiSEqC command to store Satellite Position & Enable Limits.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param posNumber Position number.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Storenn (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t posNumber);

/**
 @brief Create DiSEqC command to drive Motor to Satellite Position nn.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param posNumber Position number.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Gotonn (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address,
                                          uint8_t posNumber);

/**
 @brief Create DiSEqC command to (re-) calculate Satellite Positions.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param pData Address of the data buffer.
 @param length Length of the data buffer.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetPosns (sonysat_diseqc_message_t * pMessage,
                                            sonysat_diseqc_framing_t framing,
                                            sonysat_diseqc_address_t address,
                                            uint8_t * pData,
                                            uint8_t length);

#endif /* SONYSAT_DISEQC_COMMAND_1_2_H */
