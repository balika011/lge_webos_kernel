/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/06/01
 Revision: 1.0.0.0

------------------------------------------------------------------------------*/
#ifndef SONY_CN_DTV_H
#define SONY_CN_DTV_H

#include "sony_common.h"
#include "sony_i2c.h"


/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

typedef enum {
    SONY_CN_DTV_SYSTEM_UNKNOWN, /* Unknown */
    SONY_CN_DTV_SYSTEM_DTMB,    /* DTMB */
    SONY_CN_DTV_SYSTEM_DVBC     /* DVB-C */
}sony_cn_dtv_system_t;


/*-----------------------------------------------
  DTMB
-----------------------------------------------*/

/*-----------------------------------------------
  Carrier Mode
-----------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DTMB_CARRIER_MODE_C1,   /* C1 */
    SONY_CN_DTV_DTMB_CARRIER_MODE_C3780 /* C3780 */
}sony_cn_dtv_dtmb_carrier_mode_t;

/*-----------------------------------------------
  Pseudo Noise Information
-----------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DTMB_PN_420_VARIOUS,  /* PN420 various */
    SONY_CN_DTV_DTMB_PN_595_CONST,    /* PN595 constant */
    SONY_CN_DTV_DTMB_PN_945_VARIOUS,  /* PN945 various */
    SONY_CN_DTV_DTMB_PN_420_CONST,    /* PN420 constant */
    SONY_CN_DTV_DTMB_PN_945_CONST,    /* PN945 constant */
    SONY_CN_DTV_DTMB_PN_RESERVED      /* Reserved(5-7) */
}sony_cn_dtv_dtmb_pseudo_noise_t;

/*-----------------------------------------------
  Constellation 
-----------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DTMB_CNST_4QAM,    /* 4QAM */
    SONY_CN_DTV_DTMB_CNST_4QAM_NR, /* 4QAM-NR */
    SONY_CN_DTV_DTMB_CNST_16QAM,   /* 16QAM */
    SONY_CN_DTV_DTMB_CNST_32QAM,   /* 32QAM */
    SONY_CN_DTV_DTMB_CNST_64QAM,   /* 64QAM */
    SONY_CN_DTV_DTMB_CNST_RESERVED /* Reserved(5-7) */
}sony_cn_dtv_dtmb_cnst_t;

/*-----------------------------------------------
  LDPC Code Rate
-----------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DTMB_LDPC_RATE_0_4,     /* Rate 0.4 */
    SONY_CN_DTV_DTMB_LDPC_RATE_0_6,     /* Rate 0.6 */
    SONY_CN_DTV_DTMB_LDPC_RATE_0_8,     /* Rate 0.8 */
    SONY_CN_DTV_DTMB_LDPC_RATE_RESERVED /* Reserved */
}sony_cn_dtv_dtmb_ldpc_rate_t;

/*-----------------------------------------------
  FEC Interleave Mode
-----------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DTMB_FEC_IL_M_240, /* M_240 */
    SONY_CN_DTV_DTMB_FEC_IL_M_720  /* M_720 */
}sony_cn_dtv_dtmb_fec_il_t;

/*-----------------------------------------------
  DVB-C
-----------------------------------------------*/

/*-----------------------------------------------
  Constellation 
-----------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DVBC_CNST_16QAM,   /* 16QAM */
    SONY_CN_DTV_DVBC_CNST_32QAM,   /* 32QAM */
    SONY_CN_DTV_DVBC_CNST_64QAM,   /* 64QAM */
    SONY_CN_DTV_DVBC_CNST_128QAM,  /* 128QAM */
    SONY_CN_DTV_DVBC_CNST_256QAM,  /* 256QAM */
    SONY_CN_DTV_DVBC_CNST_RESERVED /* Reserved(5-7) */
}sony_cn_dtv_dvbc_cnst_t;


/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  System Information
--------------------------------------------------------------------*/
typedef struct {
    sony_cn_dtv_dtmb_cnst_t       cnst; /* Constellation */
    sony_cn_dtv_dtmb_ldpc_rate_t  rate; /* LDPC Code Rate */
    sony_cn_dtv_dtmb_fec_il_t     il;   /* FEC Interleave Mode */
} sony_cn_dtv_dtmb_system_info_t;


#endif /* SONY_CN_DTV_H */
