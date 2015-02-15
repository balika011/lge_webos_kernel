/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_tune_srs.h

          This file provides demodulator setting for TuneSRS function.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_H
#define SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_H

#include "sonysat_common.h"
#include "sonysat_demod.h"

/*------------------------------------------------------------------------------
 Definitions
------------------------------------------------------------------------------*/

/**
 @brief Frequency range for TuneSRS
*/
typedef enum {
    SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_FREQ_RANGE_10MHz,   /**< +/- 10MHz */
} sonysat_demod_dvbs_s2_tune_srs_freq_range_t;

#endif /* SONYSAT_DEMOD_DVBS_S2_TUNE_SRS_H */
