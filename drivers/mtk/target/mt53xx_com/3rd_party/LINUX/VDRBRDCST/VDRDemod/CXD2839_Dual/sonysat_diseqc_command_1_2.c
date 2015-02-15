/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_diseqc_command_1_2.h"

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t generateDriveXData(sonysat_diseqc_motor_mode_t mode,
                                        uint8_t amount,
                                        uint8_t * pData);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_diseqc_command_WriteA0 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_WriteA0");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x48;
    pMessage->data[3] = data;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_WriteA1 (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t data)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_WriteA1");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x49;
    pMessage->data[3] = data;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Halt (sonysat_diseqc_message_t * pMessage,
                                        sonysat_diseqc_framing_t framing,
                                        sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Halt");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x60;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LimitsOff (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LimitsOff");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x63;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LimitE (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LimitE");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x66;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_LimitW (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_LimitW");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x67;
    pMessage->length = 3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_DriveEast (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address,
                                             sonysat_diseqc_motor_mode_t mode,
                                             uint8_t amount)
{
    int32_t index = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_DriveEast");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x68;
    result = generateDriveXData(mode, amount, &pMessage->data[3]);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_DriveWest (sonysat_diseqc_message_t * pMessage,
                                             sonysat_diseqc_framing_t framing,
                                             sonysat_diseqc_address_t address,
                                             sonysat_diseqc_motor_mode_t mode,
                                             uint8_t amount)
{
    int32_t index = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_DriveWest");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x69;
    result = generateDriveXData(mode, amount, &pMessage->data[3]);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Storenn (sonysat_diseqc_message_t * pMessage,
                                           sonysat_diseqc_framing_t framing,
                                           sonysat_diseqc_address_t address,
                                           uint8_t posNumber)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Storenn");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x6A;
    pMessage->data[3] = posNumber;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_Gotonn (sonysat_diseqc_message_t * pMessage,
                                          sonysat_diseqc_framing_t framing,
                                          sonysat_diseqc_address_t address,
                                          uint8_t posNumber)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_Gotonn");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x6B;
    pMessage->data[3] = posNumber;
    pMessage->length = 4;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_diseqc_command_SetPosns (sonysat_diseqc_message_t * pMessage,
                                            sonysat_diseqc_framing_t framing,
                                            sonysat_diseqc_address_t address,
                                            uint8_t * pData,
                                            uint8_t length)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_diseqc_command_SetPosns");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = (uint8_t)framing;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x6F;
    if (length == 1){
        pMessage->data[3] = *pData;
        pMessage->length = 4;
    } else if (length == 3){
        pMessage->data[3] = *(pData);
        pMessage->data[4] = *(pData + 1);
        pMessage->data[5] = *(pData + 2);
        pMessage->length = 6;
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t generateDriveXData(sonysat_diseqc_motor_mode_t mode,
                                        uint8_t amount,
                                        uint8_t * pData)
{
    SONYSAT_TRACE_ENTER ("generateDriveXData");
    switch(mode)
    {
    case SONYSAT_DISEQC_MOTOR_MODE_STEPPED:
        if((amount > 128) || (amount == 0)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            *pData = (uint8_t)((0xFF - amount) + 1);
        }
        break;

    case SONYSAT_DISEQC_MOTOR_MODE_TIMED:
        if((amount > 127) || (amount == 0)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            *pData = amount;
        }
        break;

    case SONYSAT_DISEQC_MOTOR_MODE_NONSTOP:
        /* amount value is ignored. */
        *pData = 0;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
