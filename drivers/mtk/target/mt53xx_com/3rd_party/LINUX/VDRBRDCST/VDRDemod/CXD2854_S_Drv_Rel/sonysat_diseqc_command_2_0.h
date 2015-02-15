/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_diseqc_command_2_0.h

          This file provides functions for generating "DiSEqC2.0" commands.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DISEQC_COMMAND_2_0_H
#define SONYSAT_DISEQC_COMMAND_2_0_H

#include "sonysat_common.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create DiSEqC command to clear the "Reset" flag.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_ClrReset (sonysat_diseqc_message_t * pMessage,
                                            sonysat_diseqc_framing_t framing,
                                            sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to set Contention flag.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_SetContend (sonysat_diseqc_message_t * pMessage,
                                              sonysat_diseqc_framing_t framing,
                                              sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to return address only if Contention flag is set.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Contend (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to clear Contention flag.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_ClrContend (sonysat_diseqc_message_t * pMessage,
                                              sonysat_diseqc_framing_t framing,
                                              sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to return address unless Contention flag is set.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Address (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to change address only if Contention flag is set.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param data Data to send.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_MoveC (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address,
                                         uint8_t data);

/**
 @brief Create DiSEqC command to change address unless Contention flag is set.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.
 @param data Data to send.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Move (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address,
                                        uint8_t data);

/**
 @brief Create DiSEqC command to read Status register flags.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Status (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Configuration flags (peripheral hardware).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Config (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Switching state flags (Committed port).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Switch0 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Switching state flags (Uncommitted port).

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_Switch1 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Analogue value A0.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_ReadA0 (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Analogue value A1.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_ReadA1 (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read current frequency [Reply = BCD string].

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LOstring (sonysat_diseqc_message_t * pMessage,
                                            sonysat_diseqc_framing_t framing,
                                            sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read current frequency table entry number.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LOnow (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Lo frequency table entry number.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LOLo (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address);

/**
 @brief Create DiSEqC command to read Hi frequency table entry number.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_LOHi (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address);

#endif /* SONYSAT_DISEQC_COMMAND_2_0_H */
