/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_singlecable_command.h"

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t addPINCode (sonysat_diseqc_message_t * pMessage,
                                 uint8_t pin);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_singlecable_command_ODU_Channel_change (sonysat_diseqc_message_t * pMessage,
                                                           sonysat_singlecable_address_t address,
                                                           sonysat_singlecable_ub_slot_t ubSlot,
                                                           uint32_t ubSlotFreqKHz,
                                                           sonysat_singlecable_bank_t bank,
                                                           uint32_t centerFreqKHz)
{
    int32_t index = 0;
    uint16_t T = 0;
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_Channel_change");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    /* Calculation T */
    T = (uint16_t)(((centerFreqKHz + ubSlotFreqKHz + 2000) / 4000) - 350);

    if (T > 0x3FFF){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
    }

    pMessage->data[0] = 0xE0;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x5A;

    data  = (uint8_t)(((uint32_t)ubSlot & 0x07) << 5);
    data |= (uint8_t)(((uint32_t)bank   & 0x07) << 2);
    data |= (uint8_t)((T >> 8) & 0x03);
    pMessage->data[3] = data;

    pMessage->data[4] = (uint8_t)(T & 0xFF);
    pMessage->length = 5;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_singlecable_command_ODU_PowerOFF (sonysat_diseqc_message_t * pMessage,
                                                     sonysat_singlecable_address_t address,
                                                     sonysat_singlecable_ub_slot_t ubSlot)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_PowerOFF");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = 0xE0;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x5A;
    pMessage->data[3] = (uint8_t)((((uint32_t)ubSlot) & 0x07) << 5);
    pMessage->data[4] = 0x00;
    pMessage->length = 5;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_singlecable_command_ODU_UbxSignal_ON (sonysat_diseqc_message_t * pMessage,
                                                         sonysat_singlecable_address_t address)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_UbxSignal_ON");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = 0xE0;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x5B;
    pMessage->data[3] = 0x00;
    pMessage->data[4] = 0x00;
    pMessage->length = 5;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_singlecable_command_ODU_Config (sonysat_diseqc_message_t * pMessage,
                                                   sonysat_singlecable_address_t address,
                                                   sonysat_singlecable_ub_slot_t ubSlot,
                                                   sonysat_singlecable_num_of_sat_pos_t numberOfSatPos,
                                                   sonysat_singlecable_num_of_input_banks_t numberOfInputBanks,
                                                   sonysat_singlecable_num_of_ub_slots_t numberOfUBSlots)
{
    int32_t index = 0;

    static const struct{
        sonysat_singlecable_num_of_sat_pos_t numberOfSatPos;
        sonysat_singlecable_num_of_input_banks_t numberOfInputBanks;
        uint8_t isWideBandRF;
        sonysat_singlecable_num_of_ub_slots_t numberOfUBSlots;
        uint8_t Config_Nb;
    } ODU_Config_table[] = {
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_2, 0x10},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_4, 0x11},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_6, 0x12},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_8, 0x13},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_2, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_2, 0x14},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_2, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_4, 0x15},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_2, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_6, 0x16},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_1, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_2, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_8, 0x17},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_8, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_2, 0x18},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_8, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_4, 0x19},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_8, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_6, 0x1A},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_8, 0, SONYSAT_SINGLECABLE_NUM_OF_UB_8, 0x1B},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_2, 0x1C},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_4, 0x1D},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_6, 0x1E},
        {SONYSAT_SINGLECABLE_NUM_OF_SAT_POS_2, SONYSAT_SINGLECABLE_NUM_OF_INPUT_BANKS_4, 1, SONYSAT_SINGLECABLE_NUM_OF_UB_8, 0x1F}
    };

    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_Config");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = 0xE0;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x5B;
    pMessage->data[3] = (uint8_t)(((((uint32_t)ubSlot) & 0x07) << 5) | (uint32_t)0x01);

    for(index = 0; index < (int32_t)(sizeof(ODU_Config_table)/sizeof(ODU_Config_table[0])); index++)
    {
        if((numberOfSatPos == ODU_Config_table[index].numberOfSatPos)         &&
           (numberOfInputBanks == ODU_Config_table[index].numberOfInputBanks) &&
           (numberOfUBSlots == ODU_Config_table[index].numberOfUBSlots))
        {
            pMessage->data[4] = ODU_Config_table[index].Config_Nb;
            pMessage->length = 5;
            /* Success */
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
        }
    }
    /* Error */
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
}

sonysat_result_t sonysat_singlecable_command_ODU_LoFreq (sonysat_diseqc_message_t * pMessage,
                                                   sonysat_singlecable_address_t address,
                                                   sonysat_singlecable_ub_slot_t ubSlot,
                                                   sonysat_singlecable_lofreq_t loFreq)
{
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_LoFreq");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
        pMessage->data[index] = 0;
    }

    pMessage->data[0] = 0xE0;
    pMessage->data[1] = (uint8_t)address;
    pMessage->data[2] = 0x5B;
    pMessage->data[3] = (uint8_t)(((((uint32_t)ubSlot) & 0x07) << 5) | 0x02);
    pMessage->data[4] = (uint8_t)loFreq;
    pMessage->length = 5;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_singlecable_command_ODU_Channel_change_MDU (sonysat_diseqc_message_t * pMessage,
                                                               sonysat_singlecable_address_t address,
                                                               sonysat_singlecable_ub_slot_t ubSlot,
                                                               uint32_t ubSlotFreqKHz,
                                                               sonysat_singlecable_bank_t bank,
                                                               uint32_t centerFreqKHz,
                                                               uint8_t pin)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_Channel_change_MDU");

    result = sonysat_singlecable_command_ODU_Channel_change (pMessage, address, ubSlot, ubSlotFreqKHz, bank, centerFreqKHz);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    pMessage->data[2] = 0x5C;

    result = addPINCode (pMessage, pin);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_singlecable_command_ODU_PowerOFF_MDU (sonysat_diseqc_message_t * pMessage,
                                                         sonysat_singlecable_address_t address,
                                                         sonysat_singlecable_ub_slot_t ubSlot,
                                                         uint8_t pin)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_PowerOFF_MDU");

    result = sonysat_singlecable_command_ODU_PowerOFF (pMessage, address, ubSlot);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    pMessage->data[2] = 0x5C;

    result = addPINCode (pMessage, pin);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_singlecable_command_ODU_UbxSignal_ON_MDU (sonysat_diseqc_message_t * pMessage,
                                                             sonysat_singlecable_address_t address,
                                                             uint8_t pin)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_UbxSignal_ON_MDU");

    result = sonysat_singlecable_command_ODU_UbxSignal_ON (pMessage, address);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    pMessage->data[2] = 0x5D;

    result = addPINCode (pMessage, pin);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_singlecable_command_ODU_Config_MDU (sonysat_diseqc_message_t * pMessage,
                                                       sonysat_singlecable_address_t address,
                                                       sonysat_singlecable_ub_slot_t ubSlot,
                                                       sonysat_singlecable_num_of_sat_pos_t numberOfSatPos,
                                                       sonysat_singlecable_num_of_input_banks_t numberOfInputBanks,
                                                       sonysat_singlecable_num_of_ub_slots_t numberOfUBSlots,
                                                       uint8_t pin)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_Config_MDU");

    result = sonysat_singlecable_command_ODU_Config (pMessage, address, ubSlot, numberOfSatPos, numberOfInputBanks, numberOfUBSlots);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    pMessage->data[2] = 0x5D;

    result = addPINCode (pMessage, pin);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_singlecable_command_ODU_LoFreq_MDU (sonysat_diseqc_message_t * pMessage,
                                                       sonysat_singlecable_address_t address,
                                                       sonysat_singlecable_ub_slot_t ubSlot,
                                                       sonysat_singlecable_lofreq_t loFreq,
                                                       uint8_t pin)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_singlecable_command_ODU_LoFreq_MDU");

    result = sonysat_singlecable_command_ODU_LoFreq (pMessage, address, ubSlot, loFreq);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    pMessage->data[2] = 0x5D;

    result = addPINCode (pMessage, pin);
    if (result != SONYSAT_RESULT_OK) {SONYSAT_TRACE_RETURN (result);}

    SONYSAT_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t addPINCode (sonysat_diseqc_message_t * pMessage,
                                 uint8_t pin)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("addPIN");

    if (!pMessage){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pMessage->length < (SONYSAT_DISEQC_MESSAGE_LENGTH - 1)){
        pMessage->data[pMessage->length] = pin;
        pMessage->length++;
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (result);
}
