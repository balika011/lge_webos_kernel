/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command_1_0.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_diseqc_command_Reset (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Reset");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x00;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Standby (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Standby");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x02;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_PowerOn (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_PowerOn");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x03;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetLo (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetLo");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x20;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetVR (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetVR");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x21;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetPosA (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetPosA");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x22;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetSOA (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetSOA");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x23;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetHi (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetHi");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x24;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetHL (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetHL");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x25;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetPosB (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetPosB");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x26;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetSOB (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetSOB");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x27;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_WriteN0 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_WriteN0");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x38;
    pMessage->data[3] = data;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
