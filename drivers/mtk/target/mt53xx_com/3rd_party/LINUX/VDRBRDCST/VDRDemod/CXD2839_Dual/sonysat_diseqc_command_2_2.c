/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command_2_2.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_diseqc_command_PosStat (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_PosStat");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x64;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
