/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command_1_1.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static uint32_t dectobcd(uint32_t decimal);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t sonysat_diseqc_command_SetS1A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS1A");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x28;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS2A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS2A");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x29;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS3A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS3A");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x2A;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS4A (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS4A");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x2B;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS1B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS1B");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x2C;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS2B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS2B");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x2D;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS3B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS3B");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x2E;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetS4B (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetS4B");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x2F;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_WriteN1 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_WriteN1");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x39;
    pMessage->data[3] = data;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_WriteFreq (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address,
                                             uint32_t frequency)
{
    int32_t index = 0;
    uint32_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_WriteFreq");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x58;

    frequency = (frequency + 50) / 100;
    data = dectobcd(frequency);

    if ((data >> 24) & 0xFF){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pMessage->data[3] = (uint8_t)((data >> 16) & 0xFF);
    pMessage->data[4] = (uint8_t)((data >>  8) & 0xFF);
    pMessage->data[5] = (uint8_t)( data        & 0xFF);
    if (pMessage->data[5] == 0x00){
        /* Don't send 6th byte data(data[5]). */
        pMessage->length = 5;
    } else {
        /* Send 6th byte data(data[5]). */
        pMessage->length = 6;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static uint32_t dectobcd(uint32_t decimal)
{
    uint32_t bcd = 0;
    uint8_t r = 0;
    uint8_t index = 0;

    for (index = 0; index < 8 ; index++)
    {
        r = (uint8_t)(decimal % 10) ;
        decimal /= 10 ;
        bcd |= ((uint32_t)(r & 0x0F) << (int32_t)(4 * index)) ;
    }
    return bcd;
}
