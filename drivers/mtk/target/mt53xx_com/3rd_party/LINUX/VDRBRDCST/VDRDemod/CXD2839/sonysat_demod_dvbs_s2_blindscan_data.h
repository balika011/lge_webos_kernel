/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_data.h

          This file provides the definition of data used in BlindScan function.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_DATA_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_DATA_H

#include "sonysat_common.h"
#include "sonysat_dtv.h"

/**
 @brief Power data
*/
typedef struct sonysat_demod_dvbs_s2_blindscan_power_t {
    struct sonysat_demod_dvbs_s2_blindscan_power_t * pNext;/**< Pointer of next item. */
    uint32_t freqKHz;                                   /**< Frequency in KHz. */
    int32_t power;                                      /**< Power data in dB x 100 */
} sonysat_demod_dvbs_s2_blindscan_power_t;

/**
 @brief Data used in BlindScan/TuneSRS sequence.
*/
typedef struct sonysat_demod_dvbs_s2_blindscan_data_t {
    struct sonysat_demod_dvbs_s2_blindscan_data_t * pNext; /**< The pointer of next item. */
    union data_t {
        /** Power data. */
        struct power_t {
            uint32_t freqKHz;           /**< Frequency in KHz. */
            int32_t power;              /**< Power in dB x 100. */
        } power;

        /** Band information data. */
        struct band_t {
            uint32_t minFreqKHz;        /**< Min frequency in KHz. */
            uint32_t maxFreqKHz;        /**< Max frequency in KHz. */
        } band;

        /** Candidate data. */
        struct candidate_t {
            uint32_t centerFreqKHz;     /**< Center frequency in KHz. */
            uint32_t symbolRateKSps;    /**< Target symbol rate in KSps. */
            uint32_t minSymbolRateKSps; /**< Minimum symbol rate in range of candidate in KSps. */
            uint32_t maxSymbolRateKSps; /**< Maximum symbol rate in range of candidate in KSps. */
        } candidate;

        /** Channel data. */
        struct channel_t {
            uint32_t centerFreqKHz;     /**< Center frequency in KHz. */
            uint32_t symbolRateKSps;    /**< Symbol rate in KSps. */
            sonysat_dtv_system_t system;   /**< System. */
        } channelInfo;
    } data;
} sonysat_demod_dvbs_s2_blindscan_data_t;

/**
 @brief Storage structure
*/
typedef struct {
    sonysat_demod_dvbs_s2_blindscan_power_t availablePowerList; /**< List of available power data. */
    sonysat_demod_dvbs_s2_blindscan_power_t * pPowerArrayTop;   /**< Top of power data array. */
    int32_t powerArrayLength;                                /**< Length of power data array. */

    sonysat_demod_dvbs_s2_blindscan_data_t availableDataList;   /**< List of available data. */
    sonysat_demod_dvbs_s2_blindscan_data_t * pDataArrayTop;     /**< Top of data array. */
    int32_t dataArrayLength;                                 /**< Length of data array. */

    int32_t currentUsedCount;                                /**< Current used data count. */
    int32_t maxUsedCount;                                    /**< Max used data count. */
    int32_t currentUsedPowerCount;                           /**< Current used power data count. */
    int32_t maxUsedPowerCount;                               /**< Max used power data count. */
} sonysat_demod_dvbs_s2_blindscan_data_storage_t;

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_DATA_H */
