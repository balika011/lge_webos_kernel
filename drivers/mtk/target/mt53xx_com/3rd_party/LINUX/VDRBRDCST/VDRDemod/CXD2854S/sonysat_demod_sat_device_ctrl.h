/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_sat_device_ctrl.h

          This file provide satellite device control function for demodulator.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_SAT_DEVICE_CONTROL_H
#define SONYSAT_DEMOD_SAT_DEVICE_CONTROL_H

#include "sonysat_common.h"
#include "sonysat_demod.h"

/**
 @brief Definition of length of DiSEqC / Single cable message.
*/
#define SONYSAT_DISEQC_MESSAGE_LENGTH 12

/**
 @brief Message structure for DiSEqC / Single cable message.
*/
typedef struct {
    uint8_t data[SONYSAT_DISEQC_MESSAGE_LENGTH];   /**< Data of the message. */
    uint8_t length;                             /**< Length of the message. */
} sonysat_diseqc_message_t;

/**
 @brief DSQOUT signal mode.
*/
typedef enum {
    SONYSAT_DSQOUT_MODE_PWM,       /**< Output PWM signal from demodulator. */
    SONYSAT_DSQOUT_MODE_ENVELOPE   /**< Output envelope signal from demodulator. */
} sonysat_dsqout_mode_t;

/**
 @brief RXEN signal mode.
*/
typedef enum {
    SONYSAT_RXEN_MODE_NORMAL,      /**< Normal mode. (Output "1" during device waits reply from slave device.) */
    SONYSAT_RXEN_MODE_INV,         /**< Inverted mode. (Output "0" during device waits reply from slave device.) */
    SONYSAT_RXEN_MODE_FIXED_LOW,   /**< Fixed to low always. */
    SONYSAT_RXEN_MODE_FIXED_HIGH   /**< Fixed to high always. */
} sonysat_rxen_mode_t;

/**
 @brief TXEN signal mode.
*/
typedef enum {
    SONYSAT_TXEN_MODE_NORMAL,      /**< Normal mode. (Output "1" during device sends DiSEqC command.) */
    SONYSAT_TXEN_MODE_INV,         /**< Inverted mode. (Output "0" during device sends DiSEqC command.) */
    SONYSAT_TXEN_MODE_FIXED_LOW,   /**< Fixed to low always. */
    SONYSAT_TXEN_MODE_FIXED_HIGH   /**< Fixed to high always. */
} sonysat_txen_mode_t;

/**
 @brief Tone burst mode.
*/
typedef enum {
    SONYSAT_TONEBURST_MODE_OFF,    /**< Not send tone burst. */
    SONYSAT_TONEBURST_MODE_A,      /**< "0" Tone burst. (Satellite A) */
    SONYSAT_TONEBURST_MODE_B       /**< "1" Data burst. (Satellite B) */
} sonysat_toneburst_mode_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Configure for "DSQOUT" signal.

 @param pDemod Demodulator instance.
 @param mode DSQOUT signal mode. (Default value is PWM)
 @param toneFreqKHz Tone signal frequency. Currently, only 22(KHz) and 44(KHz) are supported.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_DSQOUTSetting (sonysat_demod_t * pDemod,
                                                        sonysat_dsqout_mode_t mode,
                                                        uint8_t toneFreqKHz);

/**
 @brief Configure for "RXEN" signal.

 @param pDemod Demodulator instance.
 @param mode RXEN signal mode.
 @param posDelay Positive delay in ms.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_RXENSetting (sonysat_demod_t * pDemod,
                                                      sonysat_rxen_mode_t mode,
                                                      uint8_t posDelay);

/**
 @brief Configure for "TXEN" signal.

 @param pDemod Demodulator instance.
 @param mode TXEN signal mode.
 @param posDelay Positive delay in ms.
 @param negDelay Negative delay in ms.

 @return
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_TXENSetting (sonysat_demod_t * pDemod,
                                                      sonysat_txen_mode_t mode,
                                                      uint8_t posDelay,
                                                      uint8_t negDelay);

/**
 @brief Output tone signal.

        Tone signal output status is changed after calling this function immediately.

 @param pDemod Demodulator instance.
 @param isEnable Continuous tone mode.
                    - 0: Disable (Not output)
                    - 1: Enable (Output)

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_OutputTone (sonysat_demod_t * pDemod,
                                                     uint8_t isEnable);

/**
 @brief Set "Tone burst" mode to demodulator.

        Tone burst signal outputs after calling sonysat_demod_sat_device_ctrl_StartTransmit() function.

 @param pDemod Demodulator instance.
 @param toneBurstMode Tone burst mode.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_SetToneBurst (sonysat_demod_t * pDemod,
                                                       sonysat_toneburst_mode_t toneBurstMode);

/**
 @brief Set DiSEqC command to demodulator.

        DiSEqC commands outputs after calling sonysat_demod_sat_device_ctrl_StartTransmit() function.

 @param pDemod Demodulator instance.
 @param isEnable DiSEqC command mode.
                    - 0: Disable (Not output any DiSEqC command)
                    - 1: Enable (Output)
 @param pCommand1 The address of 1st DiSEqC command.
 @param count1 Transmitting count for 1st DiSEqC command.
 @param pCommand2 The address of 2nd DiSEqC command. 
                  (NULL if you want to transmit only one DiSEqC command.)
 @param count2 Transmitting count for 2nd DiSEqC command.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_SetDiseqcCommand (sonysat_demod_t * pDemod,
                                                           uint8_t isEnable,
                                                           sonysat_diseqc_message_t * pCommand1,
                                                           uint8_t count1,
                                                           sonysat_diseqc_message_t * pCommand2,
                                                           uint8_t count2);

/**
 @brief Set idle time between each transmitting data.
            - DiSEqC command.
            - Tone burst.

 @param pDemod Demodulator instance.
 @param idleTimeMs Idle time in ms.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_SetIdleTime (sonysat_demod_t * pDemod,
                                                      uint8_t idleTimeMs);

/**
 @brief Set repeat to repeat time between each command.

 @param pDemod  Demodulator instance.
 @param r2rTime Idle time in ms.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_SetR2RTime (sonysat_demod_t * pDemod,
                                                     uint32_t r2rTime);

/**
 @brief Set DiSEqC reply parameters.

 @param pDemod Demodulator instance.
 @param isEnable DiSEqC reply mode.
                    - 0: No reply mode.
                    - 1: Reply mode.
 @param replyTimeoutMs Reply timeout time in ms.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode (sonysat_demod_t * pDemod,
                                                             uint8_t isEnable,
                                                             uint16_t replyTimeoutMs);

/**
 @brief Start transmit sequence.

 @param pDemod Demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_StartTransmit (sonysat_demod_t * pDemod);

/**
 @brief Get transmit status.

 @param pDemod Demodulator instance.
 @param pStatus Transmit status.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_GetTransmitStatus (sonysat_demod_t * pDemod,
                                                            uint8_t * pStatus);

/**
 @brief Get reply message.

 @param pDemod Demodulator instance.
 @param pReplyMessage Reply message instance.

 @return SONYSAT_RESULT_OK if successful and pMessage is valid.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_GetReplyMessage (sonysat_demod_t * pDemod,
                                                          sonysat_diseqc_message_t * pReplyMessage);

/**
 @brief Get RXEN mode

 @param pDemod Demodulator instance.
 @param pIsEnable RXEN mode.
                   - 0: Disable
                   - 1: Enable

 @return SONYSAT_RESULT_OK if successful and pIsEnable is valid.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_GetRXENMode (sonysat_demod_t * pDemod,
                                                      uint8_t * pIsEnable);

/**
 @brief Get TXEN mode

 @param pDemod Demodulator instance.
 @param pIsEnable TXEN mode.
                   - 0: Disable
                   - 1: Enable

 @return SONYSAT_RESULT_OK if successful and pIsEnable is valid.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_GetTXENMode (sonysat_demod_t * pDemod,
                                                      uint8_t * pIsEnable);

/**
 @brief Change the single cable mode of demod driver.

 @param pDemod Demodulator instance.
 @param enable Single cable mode.
                   - 0: Disable
                   - 1: Enable

 @return SONYSAT_RESULT_OK if successful and pIsEnable is valid.
*/
sonysat_result_t sonysat_demod_sat_device_ctrl_EnableSinglecable (sonysat_demod_t * pDemod,
                                                            uint8_t enable);

#endif /* SONYSAT_DEMOD_SAT_DEVICE_CONTROL_H */
