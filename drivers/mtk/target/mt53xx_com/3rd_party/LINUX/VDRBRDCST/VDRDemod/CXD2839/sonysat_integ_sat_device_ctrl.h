/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_integ_sat_device_ctrl.h

          This file provide functions which control satellite device.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_INTEG_SAT_DEVICE_CTRL_H
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_H

#include "sonysat_common.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_integ.h"
#include "sonysat_integ_sat_device_ctrl.h"
#include "sonysat_diseqc_command.h"
#include "sonysat_singlecable_command.h"



/**
 @brief Device voltage change time in ms.

 The time between "return from sonysat_lnbc_t::SetVoltage()" and completing voltage change.
 This value depends on the LNB controller device, circuit, etc.
 Please confirm with your implementation/environment and change it accordingly.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME 15

/**
 @brief Pre transmission wait time defined in DiSEqC1 standard.

 For one shot DiSEqC transmissions, the required wait time is >15ms, whilst for repeated messages the required wait time is >25ms.
 This timing definition is used for both transmission modes so the greater of the two times has been used.
 Please refer to "DiSEqC Update and Recommendations for Implementation Version 2.1" Figure 3 for more information.

 There is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_SPEC_PRE_TX_WAIT 26

/**
 @brief Pre transmission wait time defined in DiSEqC2 standard.

 This value is selected as the same value as pre transmission wait time defined in DiSEqC1 standard,
 because there is no clear timing description about DiSEqC2 in DiSEqC standard documents.

 There is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_SPEC_PRE_TX_WAIT 26

/**
 @brief HW setting for wait time between each event(DiSEqC command, tone burst and continuous tone) in ms for DiSEqC1 sequence.

 If SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME is set correctly,
 then there is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_TXIDLE_MS  (SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_SPEC_PRE_TX_WAIT + SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME)


/**
 @brief HW setting for wait time between each event(DiSEqC command, tone burst and continuous tone) in ms for DiSEqC2 sequence.

 If SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME is set correctly,
 then there is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_TXIDLE_MS  (SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_SPEC_PRE_TX_WAIT + SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME)

/**
 @brief HW setting for wait time between finish to receive reply message and start continuous tone.

 There is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_RXIDLE_MS   SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_SPEC_PRE_TX_WAIT

/**
 @brief HW setting for wait time between each event(Voltage change and Single cable command) in ms for Single cable sequence.

 This value(X) should be "4 < X < 22" from Singlecable standard EN50494.
 And it should be "X > SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME".
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_SINGLECABLE_TXIDLE_MS ((SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME < 5) ? 5 : SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME)
#if SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME >= 22
#error "SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME exceeds single cable standard timing requirement."
#endif /* SONYSAT_INTEG_SAT_DEVICE_CTRL_DEVICE_VOLTAGE_CHANGE_TIME >= 22 */

/**
 @brief HW setting for wait time between repeated DiSEqC1 messages (Repeat To Repeat Time) in ms.

 This value specifies time between the end of the first message and the start of the repeated message.
 There is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_R2R_TIME_MS 400

/**
 @brief HW setting for wait time between repeated Single cable messages (Repeat To Repeat Time) in ms.

 This value specifies time between the end of the first message and the top of the repeated message.
 There is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_SINGLECABLE_R2R_TIME_MS 0

/**
 @brief HW setting for DiSEqC2 reply wait time out(RTO) in ms.

 There is no need to modify this value.
*/
#define SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_RTO_TIME_MS 400

/**
 @brief Polling interval for sonysat_integ_sat_device_ctrl_TransmitToneBurst() in ms.
*/
#define SONYSAT_INTEG_DISEQC_TB_POL                        100

/**
 @brief Polling interval for sonysat_integ_sat_device_ctrl_TransmitDiseqcCommand()
        and sonysat_integ_sat_device_ctrl_TransmitDiseqcCommandWithReply() in ms.
*/
#define SONYSAT_INTEG_DISEQC_DISEQC_TRANSMIT_POL           10

/**
 @brief Polling interval for sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand() in ms.
*/
#define SONYSAT_INTEG_DISEQC_SINGLECABLE_TRANSMIT_POL      10

/**
 @brief Parameters for tuning in single cable environment.
*/
typedef struct {
    sonysat_integ_t * pInteg;                  /**< Integ part. */
    sonysat_tuner_sat_t * pTunerReal;          /**< Tuner instance. */
    sonysat_singlecable_address_t address;     /**< Address byte for single cable tune sequence. */
    sonysat_singlecable_bank_t bank;           /**< Bank for single cable tune sequence. */
    sonysat_singlecable_ub_slot_t ubSlot;      /**< UB slot number for single cable tune sequence. */
    uint32_t ubSlotFreqKHz;                 /**< UB slot center for single cable tune sequence frequency in KHz. */
    /**
     * Pointer to the function which sends the DiSEqC message by the other demodulator.
     * If this is null, this demodulator will send the DiSEqC message for the single cable control.
     * This will be used for the case when there is only one LNB controller shared between multiple tuners.
     */
    sonysat_result_t (*pTransmitByOtherDemod)(sonysat_diseqc_message_t * pMessage);
} sonysat_integ_singlecable_tuner_data_t;

/**
 @brief The data for single cable sequence.
*/
typedef struct {
    sonysat_tuner_sat_t tunerSinglecable;                        /**< Tuner instance for single cable. */
    sonysat_integ_singlecable_tuner_data_t tunerSinglecableData; /**< Single cable tuner data. */
} sonysat_integ_singlecable_data_t;

/**
 @brief Change voltage and tone.

 @param pInteg The integration part instance which already created.
 @param isVoltageHigh Voltage.
                        - 0: Low voltage.
                        - 1: High voltage.
 @param isContinuousToneOn Continuous tone state.
                            - 0: Don't output continuous tone.
                            - 1: Output continuous tone.
 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_SetVoltageTone (sonysat_integ_t * pInteg,
                                                         uint8_t isVoltageHigh,
                                                         uint8_t isContinuousToneOn);

/**
 @brief Transmit a tone burst.

 This API does following flow.
   - Stop continuous tone.
   - Change voltage.
   - Output tone burst.
   - Output continuous tone if required.

 @param pInteg The integration part instance which already created.
 @param isVoltageHigh Voltage.
                        - 0: Low voltage.
                        - 1: High voltage.
 @param toneBurstMode Tone burst mode.
 @param isContinuousToneOn Continuout tone value.
                            - 0: Don't output continuous tone.
                            - 1: Output continuous tone.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitToneBurst (sonysat_integ_t * pInteg,
                                                            uint8_t isVoltageHigh,
                                                            sonysat_toneburst_mode_t toneBurstMode,
                                                            uint8_t isContinuousToneOn);

/**
 @brief Transmit DiSEqC command.

 This API does following flow.
   - Stop continuous tone.
   - Change voltage.
   - Output DiSEqC command1.
   - Output DiSEqC command2 if required.
   - (Repeat DiSEqC commands if required.)
   - Output tone burst.
   - Output continuous tone if required.

 @param pInteg The integration part instance which already created.
 @param isVoltageHigh Voltage.
                        - 0: Low voltage.
                        - 1: High voltage.
 @param toneBurstMode Tone burst mode.
 @param isContinuousToneOn Continuout tone value.
                            - 0: Don't output continuous tone.
                            - 1: Output continuous tone.
 @param pCommand1 1st DiSEqC command.
 @param repeatCount1 Repeat count for 1st DiSEqC command.
 @param pCommand2 2nd DiSEqC command.
 @param repeatCount2 Repeat count for 2nd DiSEqC command.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitDiseqcCommand (sonysat_integ_t * pInteg,
                                                                uint8_t isVoltageHigh,
                                                                sonysat_toneburst_mode_t toneBurstMode,
                                                                uint8_t isContinuousToneOn,
                                                                sonysat_diseqc_message_t * pCommand1,
                                                                uint8_t repeatCount1,
                                                                sonysat_diseqc_message_t * pCommand2,
                                                                uint8_t repeatCount2);

/**
 @brief Transmit DiSEqC command and receive reply from device.

 This API does following flow.
   - Stop continuous tone.
   - Change voltage.
   - Output DiSEqC command.
   - Receive reply message from slave device.
   - Output tone burst.
   - Output continuous tone if required.

 @param pInteg The integration part instance which already created.
 @param isVoltageHigh Voltage.
                        - 0: Low voltage.
                        - 1: High voltage.
 @param toneBurstMode Tone burst mode.
 @param isContinuousToneOn Continuout tone value.
                            - 0: Don't output continuous tone.
                            - 1: Output continuous tone.
 @param pCommand DiSEqC command.
 @param pReply DiSEqC reply from slave device.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitDiseqcCommandWithReply (sonysat_integ_t * pInteg,
                                                                         uint8_t isVoltageHigh,
                                                                         sonysat_toneburst_mode_t toneBurstMode,
                                                                         uint8_t isContinuousToneOn,
                                                                         sonysat_diseqc_message_t * pCommand,
                                                                         sonysat_diseqc_message_t * pReply);

/**
 @brief Transmit a single cable command.

 @param pInteg The integration part instance which already created.
 @param pCommand The single cable command instance.
 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand (sonysat_integ_t * pInteg,
                                                                     sonysat_diseqc_message_t * pCommand);


/**
 @brief Enable Single cable mode.

 @param pInteg The integration part instance which already created.
 @param pSinglecableData The data for single cable mode.
 @param pTransmitByOtherDemod User registered callback is required only when the LNB controller
                              is controlled by the other demodulator. This allows the user to 
                              transmit the single cable commands to the LNBC through the other 
                              demodulator while taking care of the multi-threaded situations. 
                              Set this parameter to NULL, when the LNB controller is controlled
                              by the current demodulator. 
 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_EnableSinglecable (sonysat_integ_t * pInteg,
                                                            sonysat_integ_singlecable_data_t * pSinglecableData,
                                                            sonysat_result_t (*pTransmitByOtherDemod)(sonysat_diseqc_message_t * pMessage));

/**
 @brief Disable Single cable mode.

 @param pInteg The integration part instance which already created.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_DisableSinglecable (sonysat_integ_t * pInteg);

/**
 @brief Change tuning parameters for single cable command.

 @param pInteg The integration part instance.
 @param address The address.
 @param bank The bank.
 @param ubSlot User band slot number.
 @param ubSlotFreqKHz User band center frequency in KHz.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams (sonysat_integ_t * pInteg,
                                                                    sonysat_singlecable_address_t address,
                                                                    sonysat_singlecable_bank_t bank,
                                                                    sonysat_singlecable_ub_slot_t ubSlot,
                                                                    uint32_t ubSlotFreqKHz);

#endif /* SONYSAT_INTEG_SAT_DEVICE_CTRL_H */
