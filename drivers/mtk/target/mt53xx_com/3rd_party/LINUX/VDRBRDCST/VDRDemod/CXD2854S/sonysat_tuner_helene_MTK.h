/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_tuner_helene_MTK.h

          This file provides the tuner control function for HELENE(MTK Special).
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_TUNER_HELENE_MTK_H
#define SONYSAT_TUNER_HELENE_MTK_H

#include "sonysat_common.h"
#include "sonysat_tuner_sat.h"

/**
 @brief Satellite LNA OFF (path through) setting.
*/
#define SONYSAT_HELENE_MTK_CONFIG_SAT_LNA_OFF            0x08000000

typedef struct _sonysat_helene_mtk_t {
    uint8_t tunerID;
} sonysat_helene_mtk_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_tuner_helene_MTK_Create(sonysat_tuner_sat_t * pTuner,
                                                 uint8_t tunerID,
                                                 sonysat_helene_mtk_t * pHelene);

#endif /* SONYSAT_TUNER_HELENE_MTK_H */
