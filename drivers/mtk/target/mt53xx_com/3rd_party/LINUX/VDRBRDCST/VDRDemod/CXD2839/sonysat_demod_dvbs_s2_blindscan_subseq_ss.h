/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_subseq_ss.h

          This file provides SS sub sequence for TuneSRS/BlindScan.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_SS_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_SS_H

#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"

/**
 @brief State of SS sub sequence
*/
typedef enum {
    SS_STATE_INIT,              /**< Initialized */
    SS_STATE_START,             /**< Start */
    SS_STATE_RF_TUNING,         /**< RF tuning */
    SS_STATE_RF_TUNED,          /**< RF tuned */
    SS_STATE_AGC_CALCULATED,    /**< AGC calculating */
    SS_STATE_WAIT_CSFIN,        /**< Wait for CSFIN */
    SS_STATE_READ_CS,           /**< Read CS data */
    SS_STATE_END,               /**< End */
    SS_STATE_UNKNOWN            /**< Unknown */
} sonysat_demod_dvbs_s2_blindscan_ss_state_t;

/**
 @brief Structure for SS sub sequence
*/
typedef struct {

    /**
     @brief Pointer of common parameters used by each sub sequence
    */
    sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams;

    uint8_t isEnable;                                       /**< Enable flag (0: disable, 1: enable). */
    sonysat_demod_dvbs_s2_blindscan_ss_state_t state;          /**< State. */
    sonysat_stopwatch_t stopwatch;                             /**< Stopwatch for internal use. */

    uint32_t minFreqKHz;                                    /**< Min frequency in KHz. */
    uint32_t maxFreqKHz;                                    /**< Max frequency in KHz. */
    uint32_t stepFreqKHz;                                   /**< Resolution in KHz. */
    uint32_t tunerStepFreqKHz;                              /**< RF step frequency in KHz. */

    uint32_t currentFreqKHz;                                /**< Current frequency in KHz. */
    uint32_t agcLevel;                                      /**< AGC level from register. */
    int32_t agc_x100dB;                                     /**< AGC (x 100dB) calculated by tuner driver. */

    sonysat_demod_dvbs_s2_blindscan_power_t * pPowerList;      /**< Power list. */
    sonysat_demod_dvbs_s2_blindscan_power_t * pPowerListLast;  /**< The last item of power list. */

} sonysat_demod_dvbs_s2_blindscan_subseq_ss_t;

/**
 @brief Initialize SS sub sequence

 @param pSeq Instance for SS sub sequence.
 @param pCommonParams Pointer of common parameters.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_ss_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_ss_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams);

/**
 @brief Start SS sub sequence

 @param pSeq Instance for SS sub sequence.
 @param minFreqKHz Min frequency in KHz.
 @param maxFreqKHz Max frequency in KHz.
 @param stepFreqKHz Resolution in KHz.
 @param tunerStepFreqKHz RF step frequency in KHz.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_ss_Start (sonysat_demod_dvbs_s2_blindscan_subseq_ss_t * pSeq,
                                                            uint32_t minFreqKHz,
                                                            uint32_t maxFreqKHz,
                                                            uint32_t stepFreqKHz,
                                                            uint32_t tunerStepFreqKHz);

/**
 @brief Execute one sequence for SS sub sequence

 @param pSeq Instance for SS sub sequence.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_ss_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_ss_t * pSeq);

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_SS_H */
