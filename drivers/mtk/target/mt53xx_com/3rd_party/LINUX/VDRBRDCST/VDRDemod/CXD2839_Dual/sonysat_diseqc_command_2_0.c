/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command_2_0.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_diseqc_command_ClrReset (sonysat_diseqc_message_t * pMessage,
                                            sonysat_diseqc_framing_t framing,
                                            sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_ClrReset");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x01;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetContend (sonysat_diseqc_message_t * pMessage,
                                              sonysat_diseqc_framing_t framing,
                                              sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetContend");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x04;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Contend (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Contend");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x05;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_ClrContend (sonysat_diseqc_message_t * pMessage,
                                              sonysat_diseqc_framing_t framing,
                                              sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_ClrContend");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x06;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Address (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Address");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x07;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_MoveC (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address,
                                         uint8_t data)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_MoveC");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x08;
    pMessage->data[3] = data;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Move (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address,
                                        uint8_t data)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Move");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x09;
    pMessage->data[3] = data;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Status (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Status");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x10;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Config (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Config");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x11;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Switch0 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Switch0");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x14;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Switch1 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Switch1");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x15;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_ReadA0 (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_ReadA0");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x40;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_ReadA1 (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_ReadA1");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x41;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LOstring (sonysat_diseqc_message_t * pMessage,
                                            sonysat_diseqc_framing_t framing,
                                            sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LOstring");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x50;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LOnow (sonysat_diseqc_message_t * pMessage,
                                         sonysat_diseqc_framing_t framing,
                                         sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LOnow");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x51;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LOLo (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LOLo");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x52;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LOHi (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LOHi");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x53;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
