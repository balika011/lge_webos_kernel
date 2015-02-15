/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/06/01
 Revision: 1.0.0.0

------------------------------------------------------------------------------*/

#ifndef SONY_CN_DTV_TUNER_H
#define SONY_CN_DTV_TUNER_H

#include "sony_cn_dtv.h"

/* Software state of this driver. */
typedef enum {
    SONY_CN_DTV_TUNER_STATE_UNKNOWN,
    SONY_CN_DTV_TUNER_STATE_SHUTDOWN,       /* Chip is in Shutdown state */
    SONY_CN_DTV_TUNER_STATE_PRE_INIT,       /* Chip is in PreInitialize state */
    SONY_CN_DTV_TUNER_STATE_INIT,           /* Chip is in Initialized state */
    SONY_CN_DTV_TUNER_STATE_SLEEP,          /* Chip is in Sleep state */
    SONY_CN_DTV_TUNER_STATE_TUNE,           /* Chip is in Tuning state */
    SONY_CN_DTV_TUNER_STATE_INVALID
} sony_cn_dtv_tuner_state_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

typedef struct sony_cn_dtv_tuner_t {
    sony_result_t (*Initialize)(struct sony_cn_dtv_tuner_t* pTuner); /* Initialize the tuner */
    sony_result_t (*Finalize)(struct sony_cn_dtv_tuner_t* pTuner);   /* Finalize the tuner */
    /*--------------------------------------------------------------------
      Tune a frequency with a certain bandwidth
      Tuner driver can block while waiting for PLL locked indication (if required).

      sony_cn_dtv_tuner_t*           pTuner     Instance of tuner control struct
      uint32_t                       frequency  RF frequency(kHz)
      sony_cn_dtv_system_t system    Broadcast system
    --------------------------------------------------------------------*/
    sony_result_t (*Tune)(struct sony_cn_dtv_tuner_t* pTuner, uint32_t frequency, sony_cn_dtv_system_t system,uint8_t autoscan);

    sony_result_t (*Sleep)(struct sony_cn_dtv_tuner_t* pTuner);      /* Sleep the tuner */

    uint32_t          frequency;  /* RF frequency(kHz) to tune */
    uint32_t          xtalFreq;   /* Xtal frequency (MHz) */
    sony_cn_dtv_system_t system;  /* Broadcast system */

    sony_i2c_t*       pI2c;       /* I2C control instance */
    uint8_t           i2cAddress; /* I2C address */

    uint32_t          flags;      /* Flags that can be used by tuner drivers */
    void*             user;       /* User defined data */

    sony_cn_dtv_tuner_state_t state; /* Software state */
    /* This state information need not be used according to chip because it is unnecessary. */

} sony_cn_dtv_tuner_t;

#endif /* SONY_CN_DTV_TUNER_H */
