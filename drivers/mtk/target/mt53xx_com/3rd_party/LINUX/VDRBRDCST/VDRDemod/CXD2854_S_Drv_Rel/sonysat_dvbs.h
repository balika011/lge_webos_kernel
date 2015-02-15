/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_dvbs.h

          This file provides DVB-S related type definitions.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DVBS_H
#define SONYSAT_DVBS_H

/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/
/**
 @brief DVB-S code rate.
*/
typedef enum {
    SONYSAT_DVBS_CODERATE_1_2,             /**< 1/2 */
    SONYSAT_DVBS_CODERATE_2_3,             /**< 2/3 */
    SONYSAT_DVBS_CODERATE_3_4,             /**< 3/4 */
    SONYSAT_DVBS_CODERATE_5_6,             /**< 5/6 */
    SONYSAT_DVBS_CODERATE_7_8,             /**< 7/8 */
    SONYSAT_DVBS_CODERATE_INVALID,         /**< Invalid */
} sonysat_dvbs_coderate_t;

/**
 @brief DVB-S modulation.
*/
typedef enum {
    SONYSAT_DVBS_MODULATION_QPSK,          /**< QPSK */
    SONYSAT_DVBS_MODULATION_INVALID        /**< Invalid */
} sonysat_dvbs_modulation_t;

#endif /* SONYSAT_DVBS_H */
