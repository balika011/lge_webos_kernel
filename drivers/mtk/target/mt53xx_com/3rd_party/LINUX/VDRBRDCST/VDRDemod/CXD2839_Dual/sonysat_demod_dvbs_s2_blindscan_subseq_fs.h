/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_subseq_fs.h

          This file provides FS sub sequence for TuneSRS/BlindScan.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_FS_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_FS_H

#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_bt.h"

/**
 @brief State of FS sub sequence
*/
typedef enum {
    FS_STATE_START,         /**< Start */
    FS_STATE_WAIT_BTFIN,    /**< Wait for BT fin */
    FS_STATE_FINISH,        /**< Finish FS */
    FS_STATE_UNKNOWN        /**< Unknown */
} sonysat_demod_dvbs_s2_blindscan_fs_state_t;

/**
 @brief Structure for FS sub sequence
*/
typedef struct {

    /**
     @brief Pointer of common parameters used by each sub sequence
    */
    sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams;
    /**
     @brief Pointer of BT sub sequence instance
    */
    sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeqBT;

    uint8_t isEnable;                                   /**< Enable flag (0: disable, 1: enable). */
    sonysat_demod_dvbs_s2_blindscan_fs_state_t state;      /**< State. */

    sonysat_demod_dvbs_s2_blindscan_data_t * pCandList;    /**< Candidate list */
    sonysat_demod_dvbs_s2_blindscan_data_t * pChannelList; /**< Detected channel list */
    uint32_t minSymbolRateKSps;                         /**< Min symbol rate of search range in KSps */
    uint32_t maxSymbolRateKSps;                         /**< Max symbol rate of search range in KSps */

    sonysat_demod_dvbs_s2_blindscan_data_t * pCandCurrent; /**< Pointer of current candidate */
    sonysat_demod_dvbs_s2_blindscan_data_t * pChannelLast; /**< The last pionter of detected channel list */
    uint32_t candFreq;                                  /**< Frequency of candidate in KHz. */

    uint32_t candCount;                                 /**< Candidate number for calculate progress. */
    uint32_t candIndex;                                 /**< Candidate index for calculate progress. */

} sonysat_demod_dvbs_s2_blindscan_subseq_fs_t;

/**
 @brief Initialize FS sub sequence

 @param pSeq Instance for FS sub sequence.
 @param pSeqBT Pointer of BT sub sequence instance.
 @param pCommonParams Pointer of common parameters.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_fs_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeqBT,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams);

/**
 @brief Start CS sub sequence

 @param pSeq Instance for FS sub sequence.
 @param pCandList Candidate list.
 @param minSymbolRateKSps Min symbol rate of search range in KSps.
 @param maxSymbolRateKSps Max symbol rate of search range in KSps.
 @param pChannelList Detected channel list.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_fs_Start (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq,
                                                            sonysat_demod_dvbs_s2_blindscan_data_t * pCandList,
                                                            uint32_t minSymbolRateKSps,
                                                            uint32_t maxSymbolRateKSps,
                                                            sonysat_demod_dvbs_s2_blindscan_data_t * pChannelList);

/**
 @brief Execute one sequence for FS sub sequence

 @param pSeq Instance for FS sub sequence.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_fs_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_fs_t * pSeq);

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_SUBSEQ_FS_H */
