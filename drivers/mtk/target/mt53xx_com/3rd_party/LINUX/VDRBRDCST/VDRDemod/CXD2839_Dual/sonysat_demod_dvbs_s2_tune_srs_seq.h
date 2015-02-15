/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_tune_srs_seq.h

          This file provides defines for TuneSRS sequence.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_SEQ_H
#define SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_SEQ_H

#include "sonysat_common.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"
#include "sonysat_demod_dvbs_s2_blindscan_utility.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_bt.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_cs.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_fs.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_pm.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_ss.h"
#include "sonysat_demod_dvbs_s2_tune_srs.h"

/*------------------------------------------------------------------------------
 Definitions
------------------------------------------------------------------------------*/
#define SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_POWER_MAX  500
#define SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_DATA_MAX  5000

/**
 @brief State for TuneSRS sequence.
*/
typedef enum {
    TUNE_SRS_SEQ_STATE_START,       /**< Start */
    TUNE_SRS_SEQ_STATE_SS1_FIN,     /**< SS1 fin */
    TUNE_SRS_SEQ_STATE_RF_TUNED,    /**< RF tuned */
    TUNE_SRS_SEQ_STATE_CS1_FIN,     /**< CS1 fin */
    TUNE_SRS_SEQ_STATE_CS1_FIN2,    /**< CS1 fin2 */
    TUNE_SRS_SEQ_STATE_SS2_FIN,     /**< SS2 fin */
    TUNE_SRS_SEQ_STATE_SS3_FIN,     /**< SS3 fin */
    TUNE_SRS_SEQ_STATE_CAND2,       /**< Cand2 */
    TUNE_SRS_SEQ_STATE_BT_START,    /**< Start BT */
    TUNE_SRS_SEQ_STATE_BT_END       /**< End BT */,
    TUNE_SRS_SEQ_STATE_FINISH,      /**< Finish */
    TUNE_SRS_SEQ_STATE_UNKNOWN      /**< Unknown */
} sonysat_demod_dvbs_s2_tune_srs_seq_state_t;

/**
 @brief structure for scan sequence.
*/
typedef struct {

    /* Referred from each sub sequence. */
    sonysat_demod_dvbs_s2_blindscan_seq_common_t commonParams;

    /**
     @brief Finish/Continue flag.

            - 0: Finish the sequence.
            - 1: Continue the sequence.
    */
    uint8_t isContinue;

    /**
     @brief Signal detect flag.

            After "isContinue == 0", please see this flag.
            If isDetect == 1, TuneSRS detected signal.
            If isDetect == 0, TuneSRS didn't detect signal.
    */
    uint8_t isDetect;

    /**
     @brief State of main sequence.
    */
    uint32_t seqState;

    sonysat_demod_dvbs_s2_blindscan_subseq_bt_t subseqBT; /**< Sub sequence for BT. */
    sonysat_demod_dvbs_s2_blindscan_subseq_pm_t subseqPM; /**< Sub sequence for PM. */
    sonysat_demod_dvbs_s2_blindscan_subseq_ss_t subseqSS; /**< Sub sequence for SS. */
    sonysat_demod_dvbs_s2_blindscan_subseq_fs_t subseqFS; /**< Sub sequence for FS. */
    sonysat_demod_dvbs_s2_blindscan_subseq_cs_t subseqCS; /**< Sub sequence for CS. */

    uint32_t centerFreqKHz;     /**< Center frequency in KHz from argument */
    uint32_t rangeFreqMHz;      /**< Capture range in MHz from argument */
    uint32_t minFreqKHz;        /**< Min freq calculated from centerFreqKHz and rangeFreqMHz */
    uint32_t maxFreqKHz;        /**< Max freq calculated from centerFreqKHz and rangeFreqMHz */
    uint32_t minSymbolRateKSps; /**< Min symbol rate in KSps. (Fixed to  1000) */
    uint32_t maxSymbolRateKSps; /**< Max symbol rate in KSps. (Fixed to 45000) */

    uint32_t powerMinFreqKHz1;  /**< Min frequency for 1st power search in KHz. */
    uint32_t powerMaxFreqKHz1;  /**< Max frequency for 1st power search in KHz. */
    uint32_t powerMinFreqKHz2;  /**< Min frequency for 2nd power search in KHz. */
    uint32_t powerMaxFreqKHz2;  /**< Max frequency for 2nd power search in KHz. */
    uint32_t powerMinFreqKHz3;  /**< Min frequency for 3rd power search in KHz. */
    uint32_t powerMaxFreqKHz3;  /**< Max frequency for 3rd power search in KHz. */

    sonysat_demod_dvbs_s2_blindscan_data_t * pDetectedList;    /**< Detected list */

    sonysat_demod_dvbs_s2_blindscan_data_t * pCandList1;       /**< Candidate list 1 */
    sonysat_demod_dvbs_s2_blindscan_data_t * pCandLast1;       /**< Last pointer of candidate list 1 */
    sonysat_demod_dvbs_s2_blindscan_data_t * pCandCurrent;     /**< Current pointer of candidate list 1 */

    sonysat_demod_dvbs_s2_blindscan_data_t * pCandList2;       /**< Candidate list 2 */
    sonysat_demod_dvbs_s2_blindscan_data_t * pCandLast2;       /**< Last pointer of candidate list 2 */

    sonysat_demod_dvbs_s2_blindscan_power_t * pPowerList;      /**< Power list */
    sonysat_demod_dvbs_s2_blindscan_power_t * pPowerLast;      /**< Last pointer of power list */

    /**
     @brief Power data for storage.
    */
    sonysat_demod_dvbs_s2_blindscan_power_t powerArray[SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_POWER_MAX];
    /**
     @brief Data for storage.
    */
    sonysat_demod_dvbs_s2_blindscan_data_t dataArray[SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_DATA_MAX];

    /**
     @brief Instance for detected signal.
    */
    sonysat_dvbs_s2_tune_param_t tuneParam;

} sonysat_demod_dvbs_s2_tune_srs_seq_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Prepare to start TuneSRS sequence.

 @param pSeq The sequence instance.
 @param pDemod Demodulator instance.
 @param centerFreqKHz Center frequency in KHz.
 @param freqRange Frequency range.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_tune_srs_seq_Initialize (sonysat_demod_dvbs_s2_tune_srs_seq_t * pSeq,
                                                          sonysat_demod_t * pDemod,
                                                          uint32_t centerFreqKHz,
                                                          sonysat_demod_dvbs_s2_tune_srs_freq_range_t freqRange);

/**
 @brief Execute one sequence for TuneSRS.

 @param pSeq The sequence instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_tune_srs_seq_Sequence (sonysat_demod_dvbs_s2_tune_srs_seq_t * pSeq);

#endif /* SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_SEQ_H */
