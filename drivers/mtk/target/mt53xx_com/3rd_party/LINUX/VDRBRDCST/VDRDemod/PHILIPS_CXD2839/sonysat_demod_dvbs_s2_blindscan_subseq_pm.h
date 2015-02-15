/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_subseq_pm.h

          This file provides PM sub sequence for TuneSRS/BlindScan.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_PM_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_PM_H

#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"

/**
 @brief State of PM sub sequence
*/
typedef enum {
    PM_STATE_START,         /**< Start */
    PM_STATE_WAITING_CSFIN, /**< Wait for CS fin */
    PM_STATE_UNKNOWN        /**< Unknown */
} sonysat_demod_dvbs_s2_blindscan_pm_state_t;

/**
 @brief Structure for PM sub sequence
*/
typedef struct {

    /**
     @brief Pointer of common parameters used by each sub sequence
    */
    sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams;

    uint8_t isEnable;                               /**< Enable flag (0: disable, 1: enable). */
    sonysat_demod_dvbs_s2_blindscan_pm_state_t state;  /**< State. */
    sonysat_stopwatch_t stopwatch;                     /**< Stopwatch for internal use. */

    int32_t freqOffsetKHz;                          /**< Frequency offset in KHz. */
    uint16_t power;                                 /**< Power value */


} sonysat_demod_dvbs_s2_blindscan_subseq_pm_t;

/**
 @brief Initialize PM sub sequence

 @param pSeq Instance for PM sub sequence.
 @param pCommonParams Pointer of common parameters.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_pm_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams);

/**
 @brief Start PM sub sequence

 @param pSeq Instance for PM sub sequence.
 @param freqOffsetKHz Frequency offset in KHz.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeq,
                                                            int32_t freqOffsetKHz);

/**
 @brief Execute one sequence for PM sub sequence

 @param pSeq Instance for PM sub sequence.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_pm_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeq);

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_PM_H */
