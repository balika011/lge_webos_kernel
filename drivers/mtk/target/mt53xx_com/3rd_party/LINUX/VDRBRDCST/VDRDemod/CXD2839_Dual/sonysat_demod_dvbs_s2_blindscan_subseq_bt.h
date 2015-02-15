/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_subseq_bt.h

          This file provides BT sub sequence for TuneSRS/BlindScan APIs.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_BT_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_BT_H

#include "sonysat_common.h"
#include "sonysat_dtv.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"

/**
 @brief State of BT sub sequence
*/
typedef enum {
    BT_STATE_INIT,          /**< Initialized */
    BT_STATE_START,         /**< Started */
    BT_STATE_RF_TUNED,      /**< RF tuned */
    BT_STATE_WAIT_SRSFIN,   /**< Wait for SRS fin */
    BT_STATE_WAIT_TSLOCK,   /**< Wait for TS lock */
    BT_STATE_UNKNOWN        /**< Unknown state */
} sonysat_demod_dvbs_s2_blindscan_bt_state_t;

/**
 @brief Structure for BT sub sequence
*/
typedef struct {
    /**
     @brief Pointer of common parameters used by each sub sequence
    */
    sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams;

    uint8_t isEnable;                               /**< Enable flag (0: disable, 1: enable) */
    sonysat_demod_dvbs_s2_blindscan_bt_state_t state;  /**< State */
    sonysat_stopwatch_t stopwatch;                     /**< Stopwatch used in this sequence */
    SAT_STOPWATCH_ELAPSED_FCT pfStopwatch_elapsed;  /*MTK_Dual_Tuner: Add for multi pd glue layer*/
    uint32_t timeout;                               /**< Timeout time in ms. */

    uint32_t centerFreqKHz;                         /**< Center frequency in KHz set by main sequence. */
    uint32_t candSymbolRateKSps;                    /**< Candidate symbol rate in KSps set by main sequence. */
    uint32_t minCandSymbolRateKSps;                 /**< Min symbol rate of candidate in KSps set by main sequence. */
    uint32_t maxCandSymbolRateKSps;                 /**< Max symbol rate of candidate in KSps set by main sequence. */
    uint32_t minSymbolRateKSps;                     /**< Min symbol rate of search range set by main sequence. */
    uint32_t maxSymbolRateKSps;                     /**< Max symbol rate of search range set by main sequence. */

    /* Result of BT sequence. */
    uint8_t isLocked;                               /**< Lock flag (0 : Not TS locked, 1: TS locked) */
    uint32_t detSymbolRateSps;                      /**< Detected symbol rate in Sps. */
    int32_t detCarrierOffsetKHz;                    /**< Detected carrier offset frequency in KHz. */
    sonysat_dtv_system_t detSystem;                    /**< Detected system (DVB-S/S2) */

} sonysat_demod_dvbs_s2_blindscan_subseq_bt_t;

/**
 @brief Initialize BT sub sequence

 @param pSeq Instance for BT sub sequence.
 @param pCommonParams Pointer of common parameters.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_bt_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams);

/**
 @brief Start BT sub sequence

 @param pSeq Instance for BT sub sequence.
 @param centerFreqKHz Center frequency in KHz.
 @param candSymbolRateKSps Candidate symbol rate in KSps.
 @param minCandSymbolRateKSps Min symbol rate of candidate in KSps.
 @param maxCandSymbolRateKSps Max symbol rate of candidate in KSps.
 @param minSymbolRateKSps Min symbol rate of search range.
 @param maxSymbolRateKSps Max symbol rate of search range.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_bt_Start (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq,
                                                            uint32_t centerFreqKHz,
                                                            uint32_t candSymbolRateKSps,
                                                            uint32_t minCandSymbolRateKSps,
                                                            uint32_t maxCandSymbolRateKSps,
                                                            uint32_t minSymbolRateKSps,
                                                            uint32_t maxSymbolRateKSps);

/**
 @brief Execute one sequence for BT sub sequence

 @param pSeq Instance for BT sub sequence.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_bt_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq);

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_BT_H */
