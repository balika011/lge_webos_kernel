/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/06/30
 Revision: 1.0.1.0

------------------------------------------------------------------------------*/
#ifndef SONY_CN_DTV_INTEG_H
#define SONY_CN_DTV_INTEG_H

#include "sony_cn_dtv.h"
#include "sony_cn_dtv_demod.h"
#include "sony_cn_dtv_tuner.h"

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Lock ID for callback event
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_INTEG_LOCK_ID_LOCK,     /* Lock event */
    SONY_CN_DTV_INTEG_LOCK_ID_UNLOCK,   /* Unlock event */
    SONY_CN_DTV_INTEG_LOCK_ID_UNKNOWN   /* Unknown event */
} sony_cn_dtv_integ_lock_id_t;


/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  integ part control data
--------------------------------------------------------------------*/
typedef struct {
    sony_cn_dtv_demod_t*                pDemod;      /* Demod part instance */
    sony_cn_dtv_tuner_t*                pTuner;      /* Tuner part instance */
    uint32_t                            frequency;   /* frequency(KHz) */
    sony_cn_dtv_system_t                system;      /* Broadcast system */
    uint8_t                             isCancel;    /* Flag for stop tune and scan sequence */
    sony_cn_dtv_integ_lock_id_t         lockInfo;    /* Lock ID for callback event */
    uint8_t                             isPreset;    /* Execute to set preset info or not */
    sony_cn_dtv_demod_preset_info_t     presetInfo;  /* Preset Information */
    void*                               user;        /* User defined void* pointer */
    uint8_t                             autoscan;
} sony_cn_dtv_integ_t;


/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Tune sequence

  pInteg   : Address of integration layer instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Tune(sony_cn_dtv_integ_t* pInteg);

/*--------------------------------------------------------------------
  Scan sequence

  pInteg       : Address of integration layer instance.
  freqEnd      : End frequency of scan sequence.
  freqInterval : Frequency interval.
  pCallback    : Callback function pointer.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Scan(sony_cn_dtv_integ_t* pInteg,
                                     uint32_t freqEnd,
                                     uint32_t freqInterval,
                                     void (*pCallback)(sony_cn_dtv_integ_t*));

/*--------------------------------------------------------------------
  Cancel Sequence

  pInteg : Address of integration layer instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Cancel(sony_cn_dtv_integ_t* pInteg);


#endif /* SONY_CN_DTV_INTEG_H */

