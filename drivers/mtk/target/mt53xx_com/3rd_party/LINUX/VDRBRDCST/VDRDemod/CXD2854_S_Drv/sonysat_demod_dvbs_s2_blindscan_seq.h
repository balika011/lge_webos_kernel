/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_seq.h

          This file provides definitions for BlindScan sequence function.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SEQ_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SEQ_H

#include "sonysat_common.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"
#include "sonysat_demod_dvbs_s2_blindscan_utility.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_bt.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_cs.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_fs.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_pm.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_ss.h"

/**
 @brief Definition of search range in MHz.

 This value is affected to the array size in BlindScan sequence.
 Please set the value depend on users application.

 Default value is 1220(MHz).
 It is 940MHz - 2160MHz. ( with some margin )
*/
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SEARCH_RANGE_MHZ    1220

/**
 @brief Definition of array size used in this driver.

 The data size is depend on the input signal as follow.
 - Search range.
 - Power spectrum.
 - etc...

 User can get the maximum used data size by sonysat_demod_dvbs_s2_blindscan_seq_t::dataUsedMax.
 If the driver returns SONYSAT_RESULT_ERROR_OVERFLOW, please increase this value.
*/
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_DATA_MAX           10000

/**
 @brief Definition of array size used in this driver.

 This value is calculated by SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SEARCH_RANGE_MHZ.
 So user doesn't need to change this value directly.
*/
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_POWER_MAX ((SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SEARCH_RANGE_MHZ + 45) * 2)

/**
 @brief state of BlindScan sequence
*/
typedef enum {
    BLINDSCAN_SEQ_STATE_START,          /**< Start */
    BLINDSCAN_SEQ_STATE_SS1_FIN,        /**< SS1 fin */
    BLINDSCAN_SEQ_STATE_STAGE1_FIN,     /**< Stage1 fin */
    BLINDSCAN_SEQ_STATE_SS2_START,      /**< SS2 start */
    BLINDSCAN_SEQ_STATE_SS2_FIN,        /**< SS2 fin */
    BLINDSCAN_SEQ_STATE_FS2_START,      /**< FS2 start */
    BLINDSCAN_SEQ_STATE_FS2_FIN,        /**< FS2 fin */
    BLINDSCAN_SEQ_STATE_CS_PREPARING,   /**< CS preparing */
    BLINDSCAN_SEQ_STATE_CS_TUNED,       /**< CS tuned */
    BLINDSCAN_SEQ_STATE_CS_FIN,         /**< CS fin */
    BLINDSCAN_SEQ_STATE_FS3_START,      /**< FS3 start */
    BLINDSCAN_SEQ_STATE_FINISH,         /**< Finish */
    BLINDSCAN_SEQ_STATE_UNKNOWN         /**< Unknown */
} sonysat_demod_dvbs_s2_blindscan_seq_state_t;

/**
 @brief Structure for scan sequence.
*/
typedef struct {
    /**
     @brief Common parameters used by each sub sequence
    */
    sonysat_demod_dvbs_s2_blindscan_seq_common_t commonParams; 

    /**
     @brief Finish/Continue flag.
     
     - 0: Finish the sequence.
     - 1: Continue the sequence.
    */
    uint8_t isContinue;

    sonysat_demod_dvbs_s2_blindscan_seq_state_t seqState;    /**< State of BlindScan sequence. */
    sonysat_demod_dvbs_s2_blindscan_subseq_bt_t subseqBT;    /**< Data for sub sequence BT. */
    sonysat_demod_dvbs_s2_blindscan_subseq_pm_t subseqPM;    /**< Data for sub sequence PM. */
    sonysat_demod_dvbs_s2_blindscan_subseq_ss_t subseqSS;    /**< Data for sub sequence SS. */
    sonysat_demod_dvbs_s2_blindscan_subseq_fs_t subseqFS;    /**< Data for sub sequence FS. */
    sonysat_demod_dvbs_s2_blindscan_subseq_cs_t subseqCS;    /**< Data for sub sequence CS. */

    uint32_t minFreqKHz;        /**< Min frequency in KHz which set by BlindScan API */
    uint32_t maxFreqKHz;        /**< Max frequency in KHz which set by BlindScan API */
    uint32_t minSymbolRateKSps; /**< Min symbol rate in KSps which set by BlindScan API */
    uint32_t maxSymbolRateKSps; /**< Max symbol rate in KSps which set by BlindScan API */

    uint32_t minPowerFreqKHz;   /**< Min frequency in KHz which need for power spectrum */
    uint32_t maxPowerFreqKHz;   /**< Max frequency in KHz which need for power spectrum */

    sonysat_demod_dvbs_s2_blindscan_data_t * pDetectedList;    /**< Detected channel list */

    sonysat_demod_dvbs_s2_blindscan_data_t * pBandList;        /**< Band list */
    sonysat_demod_dvbs_s2_blindscan_data_t * pBandCurrent;     /**< Current processing band data */

    sonysat_demod_dvbs_s2_blindscan_data_t * pCandList1;       /**< Candidate list 1 */
    sonysat_demod_dvbs_s2_blindscan_data_t * pCandList2;       /**< Candidate list 2 */
    sonysat_demod_dvbs_s2_blindscan_data_t * pCandCurrent;     /**< The pointer of candidate */
    sonysat_demod_dvbs_s2_blindscan_data_t * pCandLast;        /**< The last pointer of candidate list */

    uint32_t candCount; /**< Candidate total count for calculate progress */
    uint32_t candIndex; /**< Candidate current number for calculate progress */

    /**
     @brief Power data for storage.
    */
    sonysat_demod_dvbs_s2_blindscan_power_t powerArray[SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_POWER_MAX];
    /**
     @brief Data for storage.
    */
    sonysat_demod_dvbs_s2_blindscan_data_t dataArray[SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_DATA_MAX];

} sonysat_demod_dvbs_s2_blindscan_seq_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Prepare to start BlindScan sequence.

 @param pSeq The sequence instance.
 @param pDemod Demodulator instance.
 @param minFreqKHz Minimum frequency of the scan range in KHz.
 @param maxFreqKHz Maximum frequency of the scan range in KHz.
 @param minSymbolRateKSps Minimum symbol rate of the scan range in KSps.
 @param maxSymbolRateKSps Maximum symbol rate of the scan range in KSps.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_seq_Initialize (sonysat_demod_dvbs_s2_blindscan_seq_t * pSeq,
                                                           sonysat_demod_t * pDemod,
                                                           uint32_t minFreqKHz,
                                                           uint32_t maxFreqKHz,
                                                           uint32_t minSymbolRateKSps,
                                                           uint32_t maxSymbolRateKSps);

/**
 @brief Execute one sequence for BlindScan.

 @param pSeq The sequence instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_seq_Sequence (sonysat_demod_dvbs_s2_blindscan_seq_t * pSeq);

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SEQ_H */
