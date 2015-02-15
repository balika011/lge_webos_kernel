/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_diseqc_command_2_2.h

          This file provides functions for generating "DiSEqC2.2" commands.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DISEQC_COMMAND_2_2_H
#define SONYSAT_DISEQC_COMMAND_2_2_H

#include "sonysat_common.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create DiSEqC command to read Positioner Status Register.

 @param pMessage The message instance.
 @param framing Framing byte.
 @param address Address byte.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_diseqc_command_PosStat (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address);

#endif /* SONYSAT_DISEQC_COMMAND_2_2_H */
